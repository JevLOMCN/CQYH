#include <winsock2.h>
#include "std_type.h"
#include "process_error.h"
#include "session_mgr.h"
#include "net.h"
#include "mt_config.h"
#include "maintain_files.h"

#include <algorithm>
#include <bit>
#include <charconv>
#include <chrono>
#include <limits>
#include <optional>
#include <span>
#include <string_view>
#include <utility>

namespace
{
constexpr auto never = (std::numeric_limits<UINT64>::max)();
struct Flag
{
    bool& value;
    explicit Flag(bool& flag) : value(flag) { value = true; }
    ~Flag() { value = false; }
};
std::optional<UINT32> address(std::span<const char> field)
{
    const auto end = std::ranges::find(field, '\0');
    if (end == field.end()) return {};
    return maintain::parse_address({field.data(), static_cast<std::size_t>(end - field.begin())});
}
bool validTiming(const MAINTAIN_CONFIG& config = g_MTConfig)
{
    return config.nPingTimeout >= 0 && config.nReconnectTime >= 0 && config.nServerPingInterval >= 0;
}
bool closeSocket(ISocketSession* socket)
{
    try { return socket->Close() != FALSE; }
    catch (const std::exception& error) { ERR("maintain socket close failed: %s", error.what()); }
    catch (...) { ERR("maintain socket close threw an exception"); }
    return false;
}
}

CSessionMgr CSessionMgr::ms_Instance;
CSessionMgr::CSessionMgr() : CSessionMgr(maintain::make_session_hooks()) {}
CSessionMgr::CSessionMgr(maintain::SessionHooks hooks, Clock clock)
    : m_hooks(std::move(hooks)), m_clock(std::move(clock)) {}
CSessionMgr::~CSessionMgr() { UnInit(); }
CSessionMgr& CSessionMgr::GetInstance() { return ms_Instance; }
UINT64 CSessionMgr::SteadyTick()
{
    return static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
UINT64 CSessionMgr::Now() { return m_clock(); }
UINT64 CSessionMgr::Deadline(INT32 milliseconds)
{
    const auto now = Now();
    const auto duration = static_cast<UINT64>((std::max)(milliseconds, 0));
    return duration > never - now ? never : now + duration;
}
BOOL CSessionMgr::Init(INet* net)
{
    if (!net || m_pNet || m_reloading || m_closing || m_polling || !m_clock ||
        !m_hooks.expanded || !m_hooks.changed || !m_hooks.ping) return FALSE;
    m_pNet = net;
    m_stopping = false;
    if (Reload()) return TRUE;
    m_pNet = nullptr;
    m_stopping = true;
    return FALSE;
}
BOOL CSessionMgr::Reload()
{
    return Reload(g_MTConfig);
}
BOOL CSessionMgr::Reload(const MAINTAIN_CONFIG& candidate)
try
{
    if (!m_pNet || m_stopping || m_reloading || m_closing || !validTiming(candidate)) return FALSE;
    const Flag reloading{m_reloading};
    auto next = std::make_unique<Slots>();
    for (std::size_t area = 1; area < next->size(); ++area)
    {
        const auto& configuration = candidate.AreaConfig[area];
        if (!configuration.nAreaID) continue;
        if (configuration.nAreaID != static_cast<INT32>(area)) return FALSE;
        for (std::size_t guard = 1; guard < (*next)[area].size(); ++guard)
        {
            const auto& configured = configuration.guardList[guard];
            if (!configured.szAddress[0]) continue;
            const auto ip = address(configured.szAddress);
            if (!ip || configured.nPort <= 0 || configured.nPort > 65535) return FALSE;
            for (std::size_t previous = 1; previous < guard; ++previous)
                if ((*next)[area][previous].dwIp == *ip) return FALSE;
            auto& session = (*next)[area][guard];
            session.dwArea = static_cast<UINT32>(area);
            session.dwIp = *ip;
            session.wPort = static_cast<UINT16>(configured.nPort);
            session.poStatisInfo = std::make_unique<stProcessStatisInfo>();
        }
    }
    auto* const net = m_pNet;
    const auto generation = m_generation;
    // Keep live state untouched until every endpoint and allocation is valid.
    if (!CloseAll(false) || m_pNet != net || m_stopping || m_generation != generation) return FALSE;
    if (m_sessions) *m_sessions = std::move(*next); // Stable record addresses across reloads.
    else m_sessions = std::move(next);
    m_lastPing = 0;
    ++m_generation;
    return TRUE;
}
catch (const std::exception& error)
{
    ERR("cannot reload maintain sessions: %s", error.what());
    return FALSE;
}

void CSessionMgr::Reset()
{
    if (m_sessions)
        for (auto& area : *m_sessions) for (auto& session : area) session = SRemoteGuardSession{};
    m_lastPing = 0;
    m_deferredClear = false;
}
BOOL CSessionMgr::UnInit()
{
    m_stopping = true;
    m_pNet = nullptr;
    ++m_generation;
    if (m_closing) { m_deferredClear = true; return TRUE; }
    const bool success = CloseAll(false);
    Reset();
    return success;
}
SRemoteGuardSession* CSessionMgr::GetRemoteGDSession(INT32 area, UINT32 ip)
{
    if (!m_pNet || !m_sessions || area <= 0 || area >= MAX_AREA_COUNT || !ip) return nullptr;
    auto& row = (*m_sessions)[area];
    const auto entries = std::span(row).subspan(1);
    const auto found = std::ranges::find(entries, ip, &SRemoteGuardSession::dwIp);
    return found == entries.end() ? nullptr : &*found;
}
bool CSessionMgr::Close(SRemoteGuardSession& session, bool notify)
{
    auto* const socket = std::exchange(session.pSocketSession, nullptr);
    session.bConnected = FALSE;
    session.closing = false;
    session.dwTimer = never;
    session.lstProcess.clear(); // Closes owned hibernation streams.
    if (!socket) return true;
    socket->SetUserData(nullptr);
    bool success = closeSocket(socket);
    if (notify && m_pNet && !m_stopping && !m_reloading)
        success = m_hooks.changed(session, emctUnConnect) && success;
    return success;
}
bool CSessionMgr::CloseAll(bool notify)
{
    if (m_closing) return true;
    const Flag closing{m_closing};
    bool success = true;
    if (m_sessions)
        for (auto& area : *m_sessions) for (auto& session : area)
        {
            try { success = Close(session, notify) && success; }
            catch (const std::exception& error) { ERR("cannot close maintain session: %s", error.what()); success = false; }
            catch (...) { ERR("maintain session close threw an exception"); success = false; }
        }
    if (m_deferredClear) Reset();
    return success;
}
BOOL CSessionMgr::CloseAllSession()
{
    ++m_generation;
    return CloseAll(true);
}
BOOL CSessionMgr::DoAllPing()
try
{
    if (!m_pNet || m_stopping || !m_sessions) return FALSE;
    bool success = true;
    const auto generation = m_generation;
    for (auto& area : *m_sessions) for (auto& session : area)
    {
        if (session.pSocketSession && session.bConnected && !session.closing)
            success = m_hooks.ping(session) && success;
        if (generation != m_generation || !m_pNet || m_stopping) return success;
    }
    return success;
}
catch (const std::exception& error) { ERR("maintain ping failed: %s", error.what()); return FALSE; }
catch (...) { ERR("maintain ping threw an exception"); return FALSE; }
BOOL CSessionMgr::MainLoop()
try
{
    if (!m_pNet || m_stopping || !m_sessions || !validTiming()) return FALSE;
    if (m_polling || m_reloading || m_closing) return TRUE;
    const Flag polling{m_polling};
    const auto now = Now();
    const auto generation = m_generation;
    bool success = true;
    if (now < m_lastPing || now - m_lastPing > static_cast<UINT64>(g_MTConfig.nServerPingInterval))
    {
        success = DoAllPing() != FALSE;
        if (generation != m_generation || !m_pNet || m_stopping) return success;
        m_lastPing = now;
    }
    for (auto& area : *m_sessions) for (auto& session : area)
    {
        if (session.pSocketSession && session.dwTimer < now)
        {
            success = Close(session, true) && success;
            if (generation != m_generation || !m_pNet || m_stopping) return success;
            session.dwTimer = Deadline(g_MTConfig.nReconnectTime);
        }
        else if (session.dwIp && !session.pSocketSession && session.dwTimer < now &&
            m_hooks.expanded(static_cast<INT32>(session.dwArea)))
        {
            if (generation != m_generation || !m_pNet || m_stopping) return success;
            auto* const net = m_pNet;
            session.dwTimer = Deadline(g_MTConfig.nReconnectTime);
            auto* const socket = net->Connect(session.dwIp, session.wPort);
            if (generation != m_generation || m_pNet != net || m_stopping)
            {
                if (socket) { socket->SetUserData(nullptr); closeSocket(socket); }
                return success;
            }
            session.dwTimer = Deadline(socket ? g_MTConfig.nPingTimeout : g_MTConfig.nReconnectTime);
            if (!socket) { success = false; continue; }
            session.pSocketSession = socket;
            session.bConnected = FALSE;
            session.closing = false;
            socket->SetUserData(&session);
            success = m_hooks.changed(session, emctConnecting) && success;
            if (generation != m_generation || !m_pNet || m_stopping) return success;
        }
        if (generation != m_generation || !m_pNet || m_stopping) return success;
    }
    return success;
}
catch (const std::exception& error)
{
    ERR("maintain session poll failed: %s", error.what());
    return FALSE;
}
catch (...) { ERR("maintain session poll threw an exception"); return FALSE; }
