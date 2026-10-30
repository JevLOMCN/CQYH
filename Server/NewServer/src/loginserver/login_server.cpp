#include "stdafx.h"
#include "login_server.h"
#include "dump.h"

#include "linux_compatible.h"

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"

#include "ls_session_mgr.h"
#include "oa_auth_mgr.h"
#include "ifile.h"

CLoginServer CLoginServer::ms_Instance;
CLoginServer::CLoginServer()
{
    m_pNet = NULL;
    m_pGTSocketAcceptor = NULL;
}

CLoginServer::~CLoginServer()
{
    UnInit();
}

BOOL CLoginServer::Init(INetEventHandler* pNetEventHandler)
{
    INT32 nRetCode = 0;
    INTRANET_INIT_PARAM IntranetInitParam;
    SIMPLE_INIT_PARAM SimpleInitParam;
    struct in_addr addr;

    LOG_PROCESS_ERROR(pNetEventHandler);

    nRetCode = m_PacketAllocator.Init(g_ServerConfig.LS.Net.nSmallPacketCount,
        g_ServerConfig.LS.Net.nLargePacketCount);
    LOG_PROCESS_ERROR(nRetCode);

#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.LS.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.LS.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.LS.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.LS.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.LS.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.LS.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
    m_pNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, pNetEventHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.LS.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.LS.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.LS.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.LS.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.LS.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.LS.Net.nPacketBufferSize;
    m_pNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, pNetEventHandler, &SimpleInitParam);
#endif
    LOG_PROCESS_ERROR(m_pNet);

    INF("initialized net lib successfully");

    m_pGTSocketAcceptor = m_pNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pGTSocketAcceptor);

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
    LOG_PROCESS_ERROR(addr.s_addr);

    nRetCode = m_pGTSocketAcceptor->Open(addr.s_addr, g_ServerConfig.LS.dwPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened GT listen port at %s:%d", inet_ntoa(addr), g_ServerConfig.LS.dwPort);
    INF("LS initialized successfully");

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CLoginServer::UnInit(void)
{
    INT32 nRetCode = 0;

    if (m_pGTSocketAcceptor)
    {
        nRetCode = m_pGTSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_pNet->DestroyAcceptor(m_pGTSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pGTSocketAcceptor = NULL;
    }

    if (m_pNet)
    {
#if defined(WIN32)
        nRetCode = g_DestroyNetInterface(netIntranet, m_pNet);
#elif defined(__linux__)
        nRetCode = g_DestroyNetInterface(netSimple, m_pNet);
#endif
        LOG_CHECK_ERROR(nRetCode);
        m_pNet = NULL;
    }

    nRetCode = m_PacketAllocator.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

CLoginServer& CLoginServer::GetInstance(void)
{
    return ms_Instance;
}

BOOL CLoginServer::Close(void)
{
    INT32 nRetCode = 0;
    CLSSessionMgr& rLSSessionMgr = CLSSessionMgr::GetInstance();

    nRetCode = CLSMessageHandler::GetInstance().DoMSCloseAck();
    LOG_PROCESS_ERROR(nRetCode);
    
    CLSSessionMgr::GetInstance().CloseAllSession();
    while (CLSSessionMgr::GetInstance().CheckSession())
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    rLSSessionMgr.SetServerState(sgsClosed);
    INF("LS is closed");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLoginServer::MainLoop(void)
{
    INT32 nRetCode = 0;

    nRetCode = CLSSessionMgr::GetInstance().MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pNet->MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

	if(g_ServerConfig.LS.bOAAuthorizeEnable)
	{
		nRetCode = COAAuthMgr::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

    return TRUE;
Exit0:
    return FALSE;
}

/************************************************************************/

BOOL _InitServer(void)
{
    INT32 nRetCode = 0;
    CLoginServer& rLoginServer = CLoginServer::GetInstance();
    CLSMessageHandler& rMessageHandler = CLSMessageHandler::GetInstance();

    nRetCode = CLog::GetInstance().Init("login_server", 
        g_ServerConfig.LS.Log.nLogLevelMask, 
        g_ServerConfig.LS.Log.nLogOutputMask, 
        g_ServerConfig.LS.Log.bEnableLogThread, 
        g_ServerConfig.LS.Log.bPrintThreadID,
        g_ServerConfig.LS.Log.bFlushFile);
    LOG_PROCESS_ERROR(nRetCode);

    INF("initializing login server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);

	nRetCode = g_InitStatisInfo(GetCurrentProcessId());
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = rLoginServer.Init(&rMessageHandler);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = rMessageHandler.Init(rLoginServer.GetNetInterface(), rLoginServer.GetPacketAllocator());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CLSSessionMgr::GetInstance().Init(rLoginServer.GetNetInterface());
    LOG_PROCESS_ERROR(nRetCode);

	if(g_ServerConfig.LS.bOAAuthorizeEnable)
	{
		nRetCode = COAAuthMgr::GetInstance().Init();
		LOG_PROCESS_ERROR(nRetCode);
	}

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer(void)
{
    INT32 nRetCode = 0;

	if(g_ServerConfig.LS.bOAAuthorizeEnable)
	{
		nRetCode = COAAuthMgr::GetInstance().UnInit();
		LOG_CHECK_ERROR(nRetCode);
	}

    nRetCode = CLSSessionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLSMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLoginServer::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

INT32 main(INT32 argc, TCHAR** argv)
{ 
    INT32 nRetCode = 0;

#if defined(WIN32)
	nRetCode = StartDumpThread(NULL, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    SetWindowPos(GetConsoleWindow(), HWND_TOP, 300, 0, 0, 0, SWP_NOSIZE);
#endif

	nRetCode = g_OpenPakFolder(DEFAULT_PAK_PATH);
	if (nRetCode)
		INF("open pak folder %s successfully", DEFAULT_PAK_PATH);
	else
		INF("cannot open pak folder %s", DEFAULT_PAK_PATH);

    /************************************************************************/
    // process options
    nRetCode = ProcessOption(argc, argv);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
    PROCESS_ERROR(nRetCode);


    /************************************************************************/
    // initialize
    nRetCode = _InitServer();
    LOG_PROCESS_ERROR(nRetCode);

#if (defined(WIN32) || defined(WIN64))
    ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
    {
        char szTitle[MAX_PATH];
#ifdef _FINAL
		snprintf(szTitle, MAX_PATH, "Login Server, ver %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
        snprintf(szTitle, MAX_PATH, "Login Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

    /************************************************************************/
    // main loop
    while (CLSSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
        nRetCode = CLoginServer::GetInstance().MainLoop();
        PROCESS_ERROR(nRetCode);
        
        Sleep(1);
    }

    /************************************************************************/
    // exit
    INF("LS is exiting");

	nRetCode = CLoginServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("LS has exited");

	nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif

    return 0; 
}
