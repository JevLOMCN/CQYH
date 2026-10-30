#include "stdafx.h"
#include "gameserver.h"
#include "dump.h"

#include <TlHelp32.h>
#include <time.h>
#include "time_ex.h"
#include "report.h"
#include "ifile.h"
#include "zlib.h"
#include "math_ex.h"

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"
#include "cer_message.h"

#include "gameworld/game_world.h"
#include "gameworld/message_handler.h"
#include "gs_message_handler.h"
#include "gs_session_mgr.h"

#include "gameserver.h"

CGameServer CGameServer::ms_Instance;

class CCertMessageHandler : public INetEventHandler
{
public:
    CCertMessageHandler() { m_pMathTable = NULL; }
    ~CCertMessageHandler() { SAFE_DELETE_ARRAY(m_pMathTable) }

    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession) {}
    virtual void OnConnected(ISocketSession* pSocketSession) {}
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize) 
    {
        INT32 nRetCode = 0;
        UINT32 dwDataSize = sizeof(g_MathTable);
        InternalMessage::CER_SYNC_CERTIFICATION* pMsg = (InternalMessage::CER_SYNC_CERTIFICATION*)pPacketBuffer;

        m_pMathTable = new char[dwDataSize];
        LOG_PROCESS_ERROR(m_pMathTable);

        nRetCode = uncompress((Bytef*)m_pMathTable, (uLongf*)&dwDataSize, (const Bytef*)pMsg->szMathTable, dwSize - sizeof(InternalMessage::CER_SYNC_CERTIFICATION));
        LOG_PROCESS_ERROR(nRetCode == Z_OK);

        CGameServer::GetInstance().GetPacketAllocator()->Free(pPacketBuffer);
        return;
Exit0:
        SAFE_DELETE_ARRAY(m_pMathTable);

        CGameServer::GetInstance().GetPacketAllocator()->Free(pPacketBuffer);
        return;

    }
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bPassiveShutdown) {}

    char*   m_pMathTable;
};

CGameServer::CGameServer(void)
{
    m_poNet = 0;
    m_dwIndex = 0;
    m_qwStartTick = 0;
    m_bForceClose = FALSE;
}

CGameServer::~CGameServer(void)
{
    UnInit();
}

BOOL CGameServer::_InitMathTable(void)
{
    INT32 nRetCode = 0;
    UINT32 dwAddr;
    INet* pNet = NULL;
    ISocketSession* pCSSession = NULL;
    SIMPLE_INIT_PARAM SimpleInitParam;
    CCertMessageHandler CertMessageHandler;

    SimpleInitParam.nMaxAcceptor = 1;
    SimpleInitParam.nMaxConnector = 1;
    SimpleInitParam.nMaxSessionCount = 1;
    SimpleInitParam.nSendBufSize = 1024 * 1024 * 1;
    SimpleInitParam.nRecvBufSize = 1024 * 1024 * 1;
    SimpleInitParam.nPacketBufferSize = 4;

    pNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, &CertMessageHandler, &SimpleInitParam);
    LOG_PROCESS_ERROR(pNet);

    dwAddr = inet_addr(g_ServerConfig.Common.szCertServerAddr);
    LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

    pCSSession = pNet->Connect(dwAddr, g_ServerConfig.Common.dwCSPort);
    LOG_PROCESS_ERROR(pCSSession);

    while (CertMessageHandler.m_pMathTable == NULL)
    {
        nRetCode = pNet->MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = InitMathEx(CertMessageHandler.m_pMathTable);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = pCSSession->Close();
    LOG_CHECK_ERROR(nRetCode);

    g_DestroyNetInterface(netSimple, pNet);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGameServer::Init(INetEventHandler* poNetEventHandler)
{
	INT32 nRetCode = 0;
    INTRANET_INIT_PARAM IntranetInitParam;
    SIMPLE_INIT_PARAM SimpleInitParam;
    CGameWorld& rGameWorld = CGameWorld::GetInstance();

	nRetCode = m_PacketAllocator.Init(g_ServerConfig.GS.Net.nSmallPacketCount, g_ServerConfig.GS.Net.nLargePacketCount);
	LOG_PROCESS_ERROR(nRetCode);

    // Initialize mathematics
    nRetCode = _InitMathTable();
    LOG_PROCESS_ERROR(nRetCode);

    // Internal network
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.GS.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.GS.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.GS.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.GS.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.GS.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.GS.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
    m_poNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poNetEventHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.GS.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.GS.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.GS.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.GS.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.GS.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.GS.Net.nPacketBufferSize;
    m_poNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &SimpleInitParam);
#endif
	LOG_PROCESS_ERROR(m_poNet);

    m_qwStartTick = GetTickCountEx();
    m_bForceClose = FALSE;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGameServer::UnInit(void)
{
    INT32 nRetCode = 0;

    nRetCode = UnInitMathEx();
    LOG_CHECK_ERROR(nRetCode);

    if (m_poNet)
    {
#if defined(WIN32)
        nRetCode = g_DestroyNetInterface(netIntranet, m_poNet);
#elif defined(__linux__)
        nRetCode = g_DestroyNetInterface(netSimple, m_poNet);
#endif
        LOG_CHECK_ERROR(nRetCode);
        m_poNet = NULL;
    }

	return TRUE;
}

BOOL CGameServer::Close(void)
{
    INT32 nRetCode = 0;
    CKickAllRole KickAllRole;
    CGSSessionMgr& rGSSessionMgr = CGSSessionMgr::GetInstance();

    if (m_bForceClose)
    {
        INF("GS %d ForceClose, so KickAllRole", CGSSessionMgr::GetInstance().GetSelfSessionID());
        nRetCode = CGameWorld::GetInstance().TraverseRole(KickAllRole);
        LOG_CHECK_ERROR(nRetCode);
        LOG_CHECK_ERROR(CGameWorld::GetInstance().GetRoleCount() == 0);
    }
    else
    {
        while (CGameWorld::GetInstance().GetRoleCount() > 0)
        {
            nRetCode = MainLoop();
            LOG_PROCESS_ERROR(nRetCode);
        }
        INF("GS %d finish MainLoop when closing since RoleCount 0", CGSSessionMgr::GetInstance().GetSelfSessionID());
    }

    nRetCode = CGSMessageHandler::GetInstance().DoMSCloseBeginAck();
    LOG_CHECK_ERROR(nRetCode);

    INF("GS %d now CloseAllSession", CGSSessionMgr::GetInstance().GetSelfSessionID());
    CGSSessionMgr::GetInstance().CloseAllSession();

    while (CGSSessionMgr::GetInstance().CheckSession())
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }
    CGSSessionMgr::GetInstance().SetServerState(sgsClosed);
    INF("GS %d is closed", CGSSessionMgr::GetInstance().GetSelfSessionID());

    return TRUE;
Exit0:
    return FALSE;
}

void CGameServer::SetForceClose(void)
{
    m_bForceClose = TRUE;
}

BOOL CGameServer::MainLoop(void)
{
	UINT64 qwEndTick{};
	INT32 nRetCode = 0;
	CGameWorld& rGameWorld = CGameWorld::GetInstance();

	UINT64 qwCurrentTick = GetTickCountEx();
	rGameWorld.SetGameTick(qwCurrentTick);

	INT64 llGameLoop = rGameWorld.GetGameFrame();
	if ((llGameLoop * 1000LL) < (qwCurrentTick - m_qwStartTick) * GAME_FPS)
	{
		if (!rGameWorld.IsFreezeTime())
			rGameWorld.SetGameTime((UINT32)time(NULL));
		
		rGameWorld.MainLoop();    // Main loop

		nRetCode = CGSSessionMgr::GetInstance().MainLoop();  // Internal network checks for disconnects
		LOG_PROCESS_ERROR(nRetCode);
	}

    LOG_PROCESS_ERROR(m_poNet);
    nRetCode = m_poNet->MainLoop();    // Network loop
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _FINAL

	static UINT64 s_qwLastCheckPacketTick = 0;

	if (qwCurrentTick - s_qwLastCheckPacketTick > 8000)
	{
		s_qwLastCheckPacketTick = qwCurrentTick;
		m_PacketAllocator.CheckError();
	}
#endif	// _FINAL

	qwEndTick = GetTickCountEx();
	if (qwEndTick - qwCurrentTick < (1000 / GAME_FPS))
		Sleep(1);
	else
		CRI("too slow main loop %d", (INT32)(qwEndTick - qwCurrentTick));

	return TRUE;
Exit0:
	return FALSE;
}

CGameServer& CGameServer::GetInstance(void)
{
	return ms_Instance;
}

bool CGameServer::CKickAllRole::operator()(UINT32 dwRoleID, CRole* poRole)
{
    INT32 nRetCode = 0;
    UINT32 dwClientSessionID = poRole->GetSessionID();
    
    nRetCode = CGSMessageHandler::GetInstance().DoDBSaveRoleData(poRole, TRUE);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGSMessageHandler::GetInstance().DoDBUnlockRoleData(poRole);
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoDLRoleLogout(poRole, FALSE);
    LOG_CHECK_ERROR(nRetCode);

	poRole->GetAchievementList().SetVariable(avdLastOfflineTime, CGameWorld::GetInstance().GetGameTime());

    poRole->SetState(rsWaitUnlockRoleData);

    // A missing scene indicates a serious bug
    CScene* poScene = poRole->GetScene();
    LOG_CHECK_ERROR(poScene);

    if (poScene)
    {
        nRetCode = poScene->RemoveRole(poRole);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = poRole->UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGameWorld::GetInstance().DelRole(poRole);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGSSessionMgr::GetInstance().DetachRoleFromSession(dwClientSessionID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGSMessageHandler::GetInstance().DoMSLeaveGSAck(dwClientSessionID, dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

/************************************************************************/
BOOL _InitServer(void)
{
    INT32 nRetCode = 0;
    CGameWorld& rGameWorld = CGameWorld::GetInstance();
    CGSMessageHandler& rGSMessageHandler = CGSMessageHandler::GetInstance();
    CGameServer& rGameServer = CGameServer::GetInstance();

    nRetCode = CLog::GetInstance().Init("game_server", 
        g_ServerConfig.GS.Log.nLogLevelMask, 
        g_ServerConfig.GS.Log.nLogOutputMask, 
        g_ServerConfig.GS.Log.bEnableLogThread, 
        g_ServerConfig.GS.Log.bPrintThreadID,
        g_ServerConfig.GS.Log.bFlushFile);
    LOG_PROCESS_ERROR(nRetCode);
	g_InitReportSetting(GetServerShortName(stGS), 0, g_ServerConfig.Common.szReportAbsolutePath);

#ifdef _FINAL
	INF("initializing game server %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
	INF("initializing game server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL

	nRetCode = CheckDiskRemainCapacity();
	LOG_PROCESS_ERROR(nRetCode);

    // Initialize: create internal network
	nRetCode = rGameServer.Init(&rGSMessageHandler);
	LOG_PROCESS_ERROR(nRetCode);

    // Initialize internal network functions
	nRetCode = rGSMessageHandler.Init(rGameServer.GetNetInterface(), rGameServer.GetPacketAllocator(), rGameServer.GetMessageNotifier());
	LOG_PROCESS_ERROR(nRetCode);

    // Initialize session fields
	nRetCode = CGSSessionMgr::GetInstance().Init(rGameServer.GetNetInterface());
	LOG_PROCESS_ERROR(nRetCode);

    //INF("---- Test Sleep 1 ----");
    //Sleep(1000 * 10000);

    // Initialize the server and load configuration
    nRetCode = g_InitGameWorldLib(GetTickCountEx(), rGameServer.GetMessageNotifier(), rGameServer.GetPacketAllocator());
    LOG_PROCESS_ERROR(nRetCode);

    CGSSessionMgr::GetInstance().SetServerState(sgsSelfReady);

    if (CGSSessionMgr::GetInstance().GetDBSession().byReady && (CGSSessionMgr::GetInstance().GetDLSession().byReady || g_ServerConfig.MS.nInitDLCount == 0))
    {
        nRetCode = CGSMessageHandler::GetInstance().DoMSReady();
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer(void)
{
    INT32 nRetCode = 0;
    CGameWorld& rGameWorld = CGameWorld::GetInstance();
    CGSMessageHandler& rGSMessageHandler = CGSMessageHandler::GetInstance();
    CGameServer& rGameServer = CGameServer::GetInstance();

    nRetCode = CGameWorld::GetInstance().CleanUpScenes();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = g_UnInitGameWorldLib();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGSSessionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGSMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = rGameServer.UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = g_ClosePakFolder();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CheckGameFrame(void)
{
    int a = 0;

    static UINT32 s_qwLastDumpTick = 0;

    if (CGSSessionMgr::GetInstance().GetServerState() != sgsAllReady || 
        CGameWorld::GetInstance().GetGameFrame() < GAME_FPS * 180 ||
        IsDebuggerPresent())
        return FALSE;

    UINT64 qwCurrentTick = GetTickCountEx();
    UINT64 qwStartTick = CGameServer::GetInstance().GetStartTick();
    INT64 llCurrentFrame = CGameWorld::GetInstance().GetGameFrame();

	if ((qwCurrentTick - qwStartTick > (llCurrentFrame + GAME_FPS / 2) * 1000LL / GAME_FPS) &&
		(qwCurrentTick - s_qwLastDumpTick > 1000 * 3000))
    {
		CRI("check game frame %I64d tick diff %I64d ", llCurrentFrame, qwCurrentTick - qwStartTick - llCurrentFrame * 1000 / GAME_FPS);
        s_qwLastDumpTick = qwCurrentTick;
        return TRUE;
    }
    return FALSE;
}

struct _CrtMemBlockHeader1
{
	struct _CrtMemBlockHeader1 * pBlockHeaderNext;
	struct _CrtMemBlockHeader1 * pBlockHeaderPrev;
	char *                      szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	* and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	* maintained in the debug heap.
	*/
	int                         nBlockUse;
	size_t                      nDataSize;
#else  /* _WIN64 */
	size_t                      nDataSize;
	int                         nBlockUse;
#endif  /* _WIN64 */
	long                        lRequest;
	unsigned char               gap[4];
	/* followed by:
	*  unsigned char           data[nDataSize];
	*  unsigned char           anotherGap[nNoMansLandSize];
	*/
};

INT32 main(INT32 argc, char** argv)
{
    INT32 nRetCode = 0;

#if defined(WIN32)

	nRetCode = StartDumpThread(CheckGameFrame, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    SetDumpOption(g_ServerConfig.Common.bShowDumpDialog);

    SetWindowPos(GetConsoleWindow(), HWND_TOP, 600, 0, 0, 0, SWP_NOSIZE);
#endif

    //INF("---- Test Sleep 1 ----");
    //Sleep(1000 * 10);

	nRetCode = g_OpenPakFolder(DEFAULT_PAK_PATH);
	if (nRetCode)
		INF("open pak folder %s successfully", DEFAULT_PAK_PATH);
	else
		INF("cannot open pak folder %s", DEFAULT_PAK_PATH);

    nRetCode = ProcessOption(argc, argv);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
    PROCESS_ERROR(nRetCode);

	nRetCode = g_InitStatisInfo(GetCurrentProcessId());
	LOG_PROCESS_ERROR(nRetCode);

    /************************************************************************/
    // initialize
    nRetCode = _InitServer();
    LOG_PROCESS_ERROR(nRetCode);

    /************************************************************************/
    // main loop
    while (CGSSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
#if defined(WIN32)
        __try
        {
		    nRetCode = CGameServer::GetInstance().MainLoop();
		    LOG_PROCESS_ERROR(nRetCode);
        }
        __except(DumpExceptionFilter(GetExceptionInformation()))
        {
        }
#elif defined(__linux__)
        nRetCode = CGameServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
#endif 
    }

    /************************************************************************/
    // exit
    INF("GS %d is exiting", CGSSessionMgr::GetInstance().GetSelfSessionID());

    nRetCode = CGameServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("GS has exited");
Exit1:
	nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif
    return 0; 
}


