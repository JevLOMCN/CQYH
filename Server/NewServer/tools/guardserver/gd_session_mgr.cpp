#include "stdafx.h"
#include "gd_session_mgr.h"
#include "guard_config.h"
#include "gd_message_handler.h"
#include "process_option.h"
#include <algorithm>
#include <chrono>
#include <exception>
#include <limits>
#include <memory>
#include <utility>

namespace
{
constexpr auto closed_timer = std::numeric_limits<UINT64>::max();
UINT64 deadline(UINT64 now, INT32 milliseconds)
{
    const auto delay = static_cast<UINT64>(std::chrono::milliseconds(milliseconds).count());
    return delay > closed_timer - now ? closed_timer : now + delay;
}
bool close_socket(ISocketSession* socket)
{
    try { return socket->Close() != FALSE; }
    catch (const std::exception& error) { ERR("Guard socket close failed: %s", error.what()); return false; }
    catch (...) { ERR("Guard socket close failed with an unknown exception"); return false; }
}
}
CGDSessionMgr::CGDSessionMgr(Clock clock) : m_clock(std::move(clock)) { ResetSlots(); }
CGDSessionMgr::~CGDSessionMgr() { UnInit(); }
CGDSessionMgr& CGDSessionMgr::GetInstance()
{
    static CGDSessionMgr instance;
    return instance;
}
void CGDSessionMgr::ResetSlots()
{
    m_GTSession = {};
    m_GSSession = {};
    m_MGSession = {};
    for (std::size_t index = 1; index < m_GTSession.size(); ++index)
    { m_GTSession[index].dwType = stGT; m_GTSession[index].dwSessionID = static_cast<UINT32>(index); }
    for (std::size_t index = 1; index < m_GSSession.size(); ++index)
    { m_GSSession[index].dwType = stGS; m_GSSession[index].dwSessionID = static_cast<UINT32>(index); }
    for (std::size_t index = 1; index < m_MGSession.size(); ++index)
    { m_MGSession[index].dwType = stMG; m_MGSession[index].dwSessionID = static_cast<UINT32>(index); }
    m_DBSession = {}; m_DBSession.dwType = stDB;
    m_LSSession = {}; m_LSSession.dwType = stLS;
    m_MSSession = {}; m_MSSession.dwType = stMS;
    m_BSSession = {}; m_BSSession.dwType = stBS;
    m_DLSession = {}; m_DLSession.dwType = stDL;
    m_MTSession = {}; m_MTSession.dwType = stMT;
    m_GMSession = {}; m_GMSession.dwType = stGM;
    m_MMSession = {}; m_MMSession.dwType = stMM;
}
BOOL CGDSessionMgr::Init(INet* net)
{
    if (!net || m_net || !m_clock || g_GuardConfig.nRegisterGuardTimeout < 0 ||
        g_GuardConfig.nServerPingInterval < 0 || g_GuardConfig.nServerPingTimeout < 0 || g_GuardConfig.nPingGuardTimeout < 0)
        return FALSE;
    ResetSlots();
    m_lastPing = 0;
    m_accepting = true;
    m_net = net;
    return TRUE;
}
BOOL CGDSessionMgr::UnInit()
{
    const bool success = CloseAllSocketSession();
    VisitKnown([](ServerSession& record)
    {
        if (record.pSocketSession && record.pSocketSession->GetUserData() == &record)
            record.pSocketSession->SetUserData(nullptr);
    });
    m_pending.clear();
    ResetSlots();
    for (auto& remote : m_remote)
    {
        remote.pSocketSession = nullptr;
        remote.qwTimer = 0;
        remote.dwIp = 0;
        remote.bConnecting = FALSE;
        remote.bClosing = false;
        remote.lstProcess.clear();
    }
    m_net = nullptr;
    m_lastPing = 0;
    m_accepting = false;
    return success;
}
SGSSession* CGDSessionMgr::GetGSSession(UINT32 id)
{ return id > 0 && id < m_GSSession.size() ? &m_GSSession[id] : nullptr; }
SGTSession* CGDSessionMgr::GetGTSession(UINT32 id)
{ return id > 0 && id < m_GTSession.size() ? &m_GTSession[id] : nullptr; }
SMGSession* CGDSessionMgr::GetMGSession(UINT32 id)
{ return id > 0 && id < m_MGSession.size() ? &m_MGSession[id] : nullptr; }
SRemoteGuardSession* CGDSessionMgr::GetRemoteGDSessionByIndex(UINT32 index)
{ return index > 0 && index < m_remote.size() ? &m_remote[index] : nullptr; }
SRemoteGuardSession* CGDSessionMgr::GetRemoteGDSession(UINT32 ip)
{
    if (ip == 0) return nullptr;
    const auto slots = std::span(m_remote).subspan(1);
    const auto found = std::ranges::find(slots, ip, &SRemoteGuardSession::dwIp);
    return found == slots.end() ? nullptr : &*found;
}
ServerSession* CGDSessionMgr::FindSocket(ISocketSession* session)
{
    ServerSession* found = nullptr;
    VisitKnown([&](ServerSession& record) { if (record.pSocketSession == session) found = &record; });
    if (found) return found;
    const auto pending = std::ranges::find(m_pending, session, &ServerSession::pSocketSession);
    return pending == m_pending.end() ? nullptr : &*pending;
}
bool CGDSessionMgr::RequestClose(ServerSession& record)
{
    auto* socket = record.pSocketSession;
    if (!socket || record.bClosing) return true;
    const auto old_timer = record.qwTimer;
    record.bClosing = true;
    record.qwTimer = closed_timer;
    const bool result = close_socket(socket);
    // A synchronous disconnect callback may have erased the pending record or
    // changed a stable slot. Never dereference the original record after Close.
    if (!result)
        if (auto* remaining = FindSocket(socket))
        { remaining->bClosing = false; remaining->qwTimer = old_timer; }
    return result;
}
bool CGDSessionMgr::ClosePending(UINT64 now, bool all)
{
    if (++m_closeAttempt == 0)
    {
        for (auto& entry : m_pending) entry.closeAttempt = 0;
        m_closeAttempt = 1;
    }
    const auto attempt = m_closeAttempt;
    bool success = true;
    for (;;)
    {
        const auto next = std::ranges::find_if(m_pending, [&](const ServerSession& record)
        { return record.pSocketSession && !record.bClosing && record.closeAttempt != attempt && (all || record.qwTimer < now); });
        if (next == m_pending.end()) break;
        next->closeAttempt = attempt;
        success = RequestClose(*next) && success;
        // Re-find after each callback: it may remove this or another list node.
    }
    return success;
}
BOOL CGDSessionMgr::CloseAllSocketSession()
{
    m_accepting = false;
    if (m_closingAll) return TRUE;
    m_closingAll = true;
    const auto reset = [](bool* closing) noexcept { *closing = false; };
    const std::unique_ptr<bool, decltype(reset)> guard(&m_closingAll, reset);
    bool success = true;
    VisitKnown([&](ServerSession& record) { success = RequestClose(record) && success; });
    success = ClosePending(0, true) && success;
    for (auto& remote : std::span(m_remote).subspan(1))
        if (auto* socket = remote.pSocketSession; socket && !remote.bClosing)
        {
            remote.bClosing = true;
            // Remote records are not ServerSession objects; do not leave their
            // unrelated user data visible to local-server disconnect handlers.
            socket->SetUserData(nullptr);
            const bool result = close_socket(socket);
            if (!result && remote.pSocketSession == socket) remote.bClosing = false;
            success = result && success;
        }
    return success;
}
BOOL CGDSessionMgr::CheckHasSocketSession()
{
    bool connected = false;
    VisitKnown([&](const ServerSession& record) { connected = connected || record.pSocketSession != nullptr; });
    return connected || !m_pending.empty() || std::ranges::any_of(std::span(m_remote).subspan(1),
        [](const auto& remote) { return remote.pSocketSession != nullptr; });
}
BOOL CGDSessionMgr::CheckSocketConnected(UINT8 type, UINT8 number)
{
    ServerSession* record = nullptr;
    switch (type)
    {
    case stGT: record = GetGTSession(number); break;
    case stGS: record = GetGSSession(number); break;
    case stMG: record = GetMGSession(number); break;
    default:
        if (number != 1) return FALSE;
        VisitKnown([&](ServerSession& entry) { if (entry.dwType == type) record = &entry; });
        break;
    }
    return record && record->pSocketSession;
}
BOOL CGDSessionMgr::AddServerSession(ISocketSession* session)
{
    if (!m_net || !m_accepting || !session || g_GuardConfig.nRegisterGuardTimeout < 0 || FindSocket(session)) return FALSE;
    try
    {
        ServerSession record;
        record.pSocketSession = session;
        record.qwTimer = deadline(m_clock(), g_GuardConfig.nRegisterGuardTimeout);
        record.dwSessionID = 1;
        m_pending.push_back(record);
        return TRUE;
    }
    catch (const std::exception& error) { ERR("Guard session acceptance failed: %s", error.what()); return FALSE; }
}
BOOL CGDSessionMgr::RemoveServerSession(ISocketSession* session)
{
    if (!session) return FALSE;
    const auto found = std::ranges::find(m_pending, session, &ServerSession::pSocketSession);
    if (found == m_pending.end()) return FALSE;
    m_pending.erase(found);
    return TRUE;
}
BOOL CGDSessionMgr::RefreshServerSession(ISocketSession* session)
{
    if (!m_net || !session || g_GuardConfig.nPingGuardTimeout < 0) return FALSE;
    auto* record = FindSocket(session);
    if (!record || record->bClosing || session->GetUserData() != record) return FALSE;
    try
    {
        record->qwTimer = deadline(m_clock(), g_GuardConfig.nPingGuardTimeout);
        return TRUE;
    }
    catch (const std::exception& error) { ERR("Guard keepalive failed: %s", error.what()); return FALSE; }
}
BOOL CGDSessionMgr::MainLoop()
{
    if (!m_net) return FALSE;
    try
    {
        const auto now = m_clock();
        bool success = true;
        const auto interval = std::chrono::milliseconds(g_GuardConfig.nServerPingInterval);
        if (g_GuardConfig.nServerPingInterval < 0) return FALSE;
        if (now > m_lastPing && now - m_lastPing > static_cast<UINT64>(interval.count()))
        {
            success = CGDMessageHandler::GetInstance().DoAllPing() != FALSE;
            m_lastPing = now;
        }
        VisitKnown([&](ServerSession& record)
        {
            if (record.pSocketSession && !record.bClosing && record.qwTimer < now)
            {
                CRI("%s %u ping time out", GetServerName(record.dwType), record.dwSessionID);
                success = RequestClose(record) && success;
            }
        });
        return ClosePending(now, false) && success;
    }
    catch (const std::exception& error) { ERR("Guard session loop failed: %s", error.what()); return FALSE; }
}
