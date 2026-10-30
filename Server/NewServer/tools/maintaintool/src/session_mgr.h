#pragma once

#include <array>
#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include "std_type.h"
#include "server_def.h"
#include "statis_info_def.h"
#include "session_state.h"

class INet;
class ISocketSession;
struct MAINTAIN_CONFIG;

struct SProcessInfo
{
    UINT8 byType = 0;
    UINT32 dwProcessID = 0, dwSessionID = 0;
    BOOL bAuto = FALSE;
    INT32 nState = 0;
    std::unique_ptr<std::ofstream> hibernate;
};

inline constexpr INT32 ACCUMU_MAX_TIMES = 100;
struct stProcessStatisInfo : stStatisInfo
{
    stStatisInfo oAccumuStatis[ACCUMU_MAX_TIMES]{};
    INT32 nAccumuIndex = 0;
    struct STAT_DATA { UINT64 qwMin = 0, qwMax = 0, qwTotal = 0; };
    using ProcessTotals = std::array<STAT_DATA, statTotal>;
    std::unordered_map<UINT32, ProcessTotals> totals;
};

struct SRemoteGuardSession
{
    ISocketSession* pSocketSession = nullptr; // Borrowed from INet.
    UINT64 dwTimer = 0;
    UINT32 dwArea = 0, dwIp = 0;
    UINT16 wPort = 0;
    BOOL bConnected = FALSE;
    bool closing = false;
    std::vector<SProcessInfo> lstProcess;
    std::unique_ptr<stProcessStatisInfo> poStatisInfo;
};

namespace maintain
{
struct SessionHooks
{
    std::move_only_function<bool(INT32)> expanded;
    std::move_only_function<bool(SRemoteGuardSession&, E_MT_CONNECT_STATE)> changed;
    std::move_only_function<bool(SRemoteGuardSession&)> ping;
};
SessionHooks make_session_hooks();
}

class CSessionMgr
{
public:
    using Clock = std::move_only_function<UINT64()>;
    CSessionMgr();
    explicit CSessionMgr(maintain::SessionHooks hooks, Clock clock = SteadyTick);
    ~CSessionMgr();
    CSessionMgr(const CSessionMgr&) = delete;
    CSessionMgr& operator=(const CSessionMgr&) = delete;

    BOOL Init(INet* net);
    BOOL UnInit();
    static CSessionMgr& GetInstance();
    BOOL Reload();
    BOOL Reload(const MAINTAIN_CONFIG& candidate);
    SRemoteGuardSession* GetRemoteGDSession(INT32 area, UINT32 ip);
    BOOL CloseAllSession();
    BOOL DoAllPing();
    BOOL MainLoop();
    UINT64 Now();
    UINT64 Deadline(INT32 milliseconds);
    static UINT64 SteadyTick();

private:
    using Slots = std::array<std::array<SRemoteGuardSession, MAX_GUARD_COUNT_PER_AREA>, MAX_AREA_COUNT>;
    bool CloseAll(bool notify);
    bool Close(SRemoteGuardSession& session, bool notify);
    void Reset();

    static CSessionMgr ms_Instance;
    INet* m_pNet = nullptr;
    UINT64 m_lastPing = 0, m_generation = 0;
    bool m_stopping = true, m_reloading = false, m_closing = false, m_polling = false, m_deferredClear = false;
    maintain::SessionHooks m_hooks;
    Clock m_clock;
    std::unique_ptr<Slots> m_sessions;
};
