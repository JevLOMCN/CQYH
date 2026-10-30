#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <list>
#include <span>
#include <vector>
#include "std_type.h"
#include "server_def.h"
#include "net.h"
#include "time_ex.h"

// Socket pointers are borrowed from INet. Session records stay at stable
// addresses while the transport stores their address in user data.
struct ServerSession
{
    UINT32 dwType = stInvalid;
    ISocketSession* pSocketSession = nullptr;
    UINT64 qwTimer = 0;
    INT32 eState = 0;
    UINT32 dwSessionID = 0;
    UINT32 dwProcessID = 0;
    bool bClosing = false;
    std::uint64_t closeAttempt = 0;
};
struct SMSSession : ServerSession {};
struct SGTSession : ServerSession {};
struct SGSSession : ServerSession {};
struct SDBSession : ServerSession {};
struct SLSSession : ServerSession {};
struct SBSSession : ServerSession {};
struct SDLSession : ServerSession {};
struct SMTSession : ServerSession {};
struct SGMSession : ServerSession {};
struct SMMSession : ServerSession {};
struct SMGSession : ServerSession {};
struct SRemoteSessionInfo
{
    UINT8 byType = 0;
    UINT32 dwProcessID = 0;
    UINT32 dwSessionID = 0;
    BOOL bAuto = FALSE;
    INT32 nState = 0;
};
struct SRemoteGuardSession
{
    ISocketSession* pSocketSession = nullptr;
    UINT64 qwTimer = 0;
    UINT32 dwIp = 0;
    BOOL bConnecting = FALSE;
    std::vector<SRemoteSessionInfo> lstProcess;
    bool bClosing = false;
};
class CGDSessionMgr
{
public:
    using Clock = std::move_only_function<UINT64()>;
    explicit CGDSessionMgr(Clock clock = GetTickCountEx);
    ~CGDSessionMgr();
    CGDSessionMgr(const CGDSessionMgr&) = delete;
    CGDSessionMgr& operator=(const CGDSessionMgr&) = delete;
    static CGDSessionMgr& GetInstance();
    BOOL Init(INet* net);
    BOOL UnInit();
    SGSSession* GetGSSession(UINT32 id);
    SGTSession* GetGTSession(UINT32 id);
    SMGSession* GetMGSession(UINT32 id);
    SLSSession& GetLSSession() { return m_LSSession; }
    SDBSession& GetDBSession() { return m_DBSession; }
    SMSSession& GetMSSession() { return m_MSSession; }
    SBSSession& GetBSSession() { return m_BSSession; }
    SDLSession& GetDLSession() { return m_DLSession; }
    SMTSession& GetMTSession() { return m_MTSession; }
    SGMSession& GetGMSession() { return m_GMSession; }
    SMMSession& GetMMSession() { return m_MMSession; }
    SRemoteGuardSession* GetRemoteGDSessionByIndex(UINT32 index);
    SRemoteGuardSession* GetRemoteGDSession(UINT32 ip);
    BOOL CheckHasSocketSession();
    BOOL CloseAllSocketSession();
    BOOL CheckSocketConnected(UINT8 type, UINT8 number);
    BOOL AddServerSession(ISocketSession* session);
    BOOL RemoveServerSession(ISocketSession* session);
    BOOL RefreshServerSession(ISocketSession* session);
    BOOL MainLoop();

private:
    void ResetSlots();
    template<class Action> void VisitKnown(Action action)
    {
        for (auto* slot : std::array<ServerSession*, 8>{&m_DBSession, &m_LSSession, &m_MSSession, &m_BSSession,
            &m_DLSession, &m_MTSession, &m_GMSession, &m_MMSession}) action(*slot);
        for (auto& slot : std::span(m_GTSession).subspan(1)) action(slot);
        for (auto& slot : std::span(m_GSSession).subspan(1)) action(slot);
        for (auto& slot : std::span(m_MGSession).subspan(1)) action(slot);
    }
    ServerSession* FindSocket(ISocketSession* session);
    bool RequestClose(ServerSession& record);
    bool ClosePending(UINT64 now, bool all);
    Clock m_clock;
    INet* m_net = nullptr;
    UINT64 m_lastPing = 0;
    std::uint64_t m_closeAttempt = 0;
    bool m_accepting = false;
    bool m_closingAll = false;
    std::array<SGTSession, MAX_GT_SESSION_COUNT> m_GTSession{};
    std::array<SGSSession, MAX_GS_SESSION_COUNT> m_GSSession{};
    std::array<SMGSession, MAX_MG_SESSION_COUNT> m_MGSession{};
    SDBSession m_DBSession;
    SLSSession m_LSSession;
    SMSSession m_MSSession;
    SBSSession m_BSSession;
    SDLSession m_DLSession;
    SMTSession m_MTSession;
    SGMSession m_GMSession;
    SMMSession m_MMSession;
    std::list<ServerSession> m_pending;
    std::array<SRemoteGuardSession, MAX_GUARD_COUNT_PER_AREA> m_remote{};
};
