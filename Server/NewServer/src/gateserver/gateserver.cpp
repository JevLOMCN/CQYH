#include "stdafx.h"
#include "gateserver.h"
#include "dump.h"

#include "time_ex.h"
#include "report.h"

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"

#include "external_message_size.h"
#include "net.h"

#include "gt_session_mgr.h"
#include "ifile.h"

CGateServer CGateServer::ms_Instance;

CGateServer::CGateServer()
{
    m_pExternalNet = NULL;
    m_pInternalNet = NULL;
    m_pGSSocketAcceptor = NULL;

	m_dwInternalAddr = 0;
	m_dwInternalPort = 0;

	m_nExternalAddrCount = 0;
	memset(m_pClientSocketAcceptor, 0, sizeof(m_pClientSocketAcceptor));
	memset(m_dwExternalAddr, 0, sizeof(m_dwExternalAddr));
	memset(m_dwExternalPort, 0, sizeof(m_dwExternalPort));
}

CGateServer::~CGateServer()
{
    UnInit();
}

CGateServer& CGateServer::GetInstance(void)
{
    return ms_Instance;
}

BOOL CGateServer::Init(INetEventHandler* poInterMessageHandler, INetEventHandler* poExternalMessageHandler)
{
    INT32 nRetCode = 0;
    ASIO_INIT_PARAM AsioInitParam;
    SIMPLE_INIT_PARAM SimpleInitParam;

    INTRANET_INIT_PARAM IntranetInitParam;

    LOG_PROCESS_ERROR(poInterMessageHandler);
    LOG_PROCESS_ERROR(poExternalMessageHandler);

    nRetCode = m_PacketAllocator.Init(g_ServerConfig.GT.Net.nSmallPacketCount, g_ServerConfig.GT.Net.nLargePacketCount);
    LOG_PROCESS_ERROR(nRetCode);

    // External network
    AsioInitParam.nMaxSessionCount = g_ServerConfig.GT.nMaxClientSessionCount;
    AsioInitParam.nThreadCount = g_ServerConfig.GT.nWorkThreadCount;
    AsioInitParam.pnMessageSizeList = g_nC2SMessageSize;
	AsioInitParam.nMaxOperationPerLoop = g_ServerConfig.GT.Net.nMaxOperationPerLoop;
	AsioInitParam.nRecvLoopBufferSize = g_ServerConfig.GT.Net.nRecvLoopBufferSize;
	AsioInitParam.nSendLoopBufferSize = g_ServerConfig.GT.Net.nSendLoopBufferSize;
    m_pExternalNet = g_CreateNetInterface(netAsio, &m_PacketAllocator, poExternalMessageHandler, &AsioInitParam);
    LOG_PROCESS_ERROR(m_pExternalNet);

    // Internal network
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.GT.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.GT.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.GT.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.GT.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.GT.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.GT.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
    m_pInternalNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poInterMessageHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.GT.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.GT.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.GT.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.GT.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.GT.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.GT.Net.nPacketBufferSize;
    m_pInternalNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poInterMessageHandler, &SimpleInitParam);
#endif
    LOG_PROCESS_ERROR(m_pInternalNet);

	for (m_nExternalAddrCount = 0; m_nExternalAddrCount < MAX_ADAPTER_COUNT; m_nExternalAddrCount++)
	{
		m_dwExternalAddr[m_nExternalAddrCount] =
			GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, FALSE, m_nExternalAddrCount);
		if (m_dwExternalAddr[m_nExternalAddrCount] == 0)
			break;

        // External network - create acceptor
		m_pClientSocketAcceptor[m_nExternalAddrCount] = m_pExternalNet->CreateAcceptor();
		LOG_PROCESS_ERROR(m_pClientSocketAcceptor[m_nExternalAddrCount]);

		m_dwExternalPort[m_nExternalAddrCount] = (g_ServerConfig.GT.dwStartExternalPort + g_ServerConfig.MS.nInitBSCount) % 65536;
		do 
		{
			nRetCode = m_pClientSocketAcceptor[m_nExternalAddrCount]->Open(
				m_dwExternalAddr[m_nExternalAddrCount], m_dwExternalPort[m_nExternalAddrCount]);
			if (nRetCode)
			{
				INF("opened %d external listen port at %s:%d", 
					m_nExternalAddrCount, inet_ntoa(*(in_addr*)&m_dwExternalAddr[m_nExternalAddrCount]), m_dwExternalPort[m_nExternalAddrCount]);
				break;
			}
			m_dwExternalPort[m_nExternalAddrCount]++;
		} while (m_dwExternalPort[m_nExternalAddrCount] < 65536);
	}
	LOG_PROCESS_ERROR(m_nExternalAddrCount > 0);
	
    m_pGSSocketAcceptor = m_pInternalNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pGSSocketAcceptor);

	m_dwInternalAddr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
	LOG_PROCESS_ERROR(m_dwInternalAddr);

    // Create shared memory
    m_dwInternalPort = g_ServerConfig.GT.dwStartInternalPort;
	nRetCode = m_pGSSocketAcceptor->Open(m_dwInternalAddr, m_dwInternalPort);
    while(!nRetCode)
    {
		m_dwInternalPort = (m_dwInternalPort + 1) % 65536;
		nRetCode = m_pGSSocketAcceptor->Open(m_dwInternalAddr, m_dwInternalPort);
    }
    
	INF("opened internal listen port at %s:%d", inet_ntoa(*(in_addr*)&m_dwInternalAddr), m_dwInternalPort);

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CGateServer::UnInit(void)
{
    INT32 nRetCode = 0;

	for (INT32 nIndex = 0; nIndex < m_nExternalAddrCount; nIndex++)
	{
		if (m_pClientSocketAcceptor[nIndex])
		{
			nRetCode = m_pClientSocketAcceptor[nIndex]->Close();
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = m_pExternalNet->DestroyAcceptor(m_pClientSocketAcceptor[nIndex]);
			LOG_CHECK_ERROR(nRetCode);

			m_pClientSocketAcceptor[nIndex] = NULL;
		}
	}
	m_nExternalAddrCount = 0;

    if (m_pGSSocketAcceptor)
    {
        nRetCode = m_pGSSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_pInternalNet->DestroyAcceptor(m_pGSSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pGSSocketAcceptor = NULL;
    }

    if (m_pExternalNet)
    {
        nRetCode = g_DestroyNetInterface(netAsio, m_pExternalNet);
        LOG_CHECK_ERROR(nRetCode);
        m_pExternalNet = NULL;
    }

    if (m_pInternalNet)
    {
#if defined(WIN32)
        nRetCode = g_DestroyNetInterface(netIntranet, m_pInternalNet);
#elif defined(__linux__)
        nRetCode = g_DestroyNetInterface(netSimple, m_pInternalNet);
#endif
        LOG_CHECK_ERROR(nRetCode);
        m_pInternalNet = NULL;
    }

    nRetCode = m_PacketAllocator.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE; 
}

BOOL CGateServer::MainLoop(void)
{
    UINT64 qwEndTick = 0;
    INT32 nRetCode = 0;
	UINT64 qwCurrentTick = GetTickCountEx();
	static UINT64 s_qwLastSessionTick = 0;

	if (qwCurrentTick - s_qwLastSessionTick > 1000)
	{
		nRetCode = CGTSessionMgr::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);

		s_qwLastSessionTick = qwCurrentTick;
	}

    nRetCode = m_pInternalNet->MainLoop();    // Internal network
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pExternalNet->MainLoop();    // External network
    LOG_PROCESS_ERROR(nRetCode);

	qwEndTick = GetTickCountEx();
	if (qwEndTick - qwCurrentTick < 10)
        Sleep(1);
	else if (qwEndTick - qwCurrentTick > 50)
		CRI("too slow main loop %lld", qwEndTick - qwCurrentTick);

    g_SetStatisInfo(statRoleCount, CGTSessionMgr::GetInstance().GetClientSessionCount());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGateServer::Close(void)
{
    INT32 nRetCode = 0;
    CGTSessionMgr& rSessionMgr = CGTSessionMgr::GetInstance();

    nRetCode = rSessionMgr.CloseAllClientSession();
    LOG_PROCESS_ERROR(nRetCode);

    while (!rSessionMgr.IsAllClientSessionClosed())
    {
        nRetCode = CGateServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CInternalMessageHandler::GetInstance().DoMSCloseAck();
    LOG_CHECK_ERROR(nRetCode);

    CGTSessionMgr::GetInstance().CloseAllSession();
    while (CGTSessionMgr::GetInstance().CheckSession())
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    CGTSessionMgr::GetInstance().SetServerState(sgsClosed);
    INF("GT %d is closed", CGTSessionMgr::GetInstance().GetSelfSessionID());

    return TRUE;
Exit0:
    return FALSE;
}

/************************************************************************/

BOOL _InitServer(void)
{
    INT32 nRetCode = 0;
    CGateServer& rGateServer = CGateServer::GetInstance();
    CExternalMessageHandler& rExternalMessageHandler = CExternalMessageHandler::GetInstance();
    CInternalMessageHandler& rInternalMessageHandler = CInternalMessageHandler::GetInstance();

    nRetCode = CLog::GetInstance().Init("gate_server", 
        g_ServerConfig.GT.Log.nLogLevelMask, 
        g_ServerConfig.GT.Log.nLogOutputMask, 
        g_ServerConfig.GT.Log.bEnableLogThread, 
        g_ServerConfig.GT.Log.bPrintThreadID,
        g_ServerConfig.GT.Log.bFlushFile);
    LOG_PROCESS_ERROR(nRetCode);
	g_InitReportSetting(GetServerShortName(stGT), 0, g_ServerConfig.Common.szReportAbsolutePath);

#ifdef _FINAL
	INF("initializing gate server %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
	INF("initializing gate server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL

    // Create internal and external networks
    nRetCode = CGateServer::GetInstance().Init(&rInternalMessageHandler, &rExternalMessageHandler);
    LOG_PROCESS_ERROR(nRetCode);

    // Initialize external network information
    nRetCode = rExternalMessageHandler.Init(rGateServer.GetExternalNet(), rGateServer.GetPacketAllocator());
    LOG_PROCESS_ERROR(nRetCode);

    // Initialize internal network information
    nRetCode = rInternalMessageHandler.Init(rGateServer.GetInternalNet(), rGateServer.GetPacketAllocator());
    LOG_PROCESS_ERROR(nRetCode);

    // Initialize session fields
    nRetCode = CGTSessionMgr::GetInstance().Init(rGateServer.GetExternalNet(), rGateServer.GetInternalNet());
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer(void)
{
    INT32 nRetCode = 0;

	nRetCode = CGateServer::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CInternalMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CExternalMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CGTSessionMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

INT32 main(INT32 argc, TCHAR** argv) 
{ 
    INT32 nRetCode = 0;
    UINT32 dwLastSleepTick = 0;

    //Sleep(1000 * 10);

#if defined(WIN32)
	nRetCode = StartDumpThread(NULL, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    SetWindowPos(GetConsoleWindow(), HWND_TOP, 400, 0, 0, 0, SWP_NOSIZE);
#endif // WIN32
	
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
    while (CGTSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
        nRetCode = CGateServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    /************************************************************************/
    // exit
    INF("GT %d is exiting", CGTSessionMgr::GetInstance().GetSelfSessionID());
    nRetCode = CGateServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("GT has exited");

	nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif //WIN32
    return 0; 
}

void GTSysLogSuccess(CLIENT_SESSION* pstSession, const char* szFucntionName)
{
	PROCESS_ERROR(pstSession);

	if(!pstSession->bConnected)
	{
		SYST("session<%08X>, state[%d] after %s success. shutdowntype[%d],", 
			pstSession->dwSessionID, pstSession->eState, szFucntionName, pstSession->byShutDownType);
	}
	else
	{
		SYST("session<%08X>, state[%d] after %s success.", 
			pstSession->dwSessionID, pstSession->eState, szFucntionName);
	}

Exit0:
	return;
}

void GTSysLogError(CLIENT_SESSION* pstSession, const char* szFucntionName)
{
	PROCESS_ERROR(pstSession);

	if(!pstSession->bConnected)
	{
		SYSF("session<%08X>, state[%d] after %s failed. shutdowntype[%d],", 
			pstSession->dwSessionID, pstSession->eState, szFucntionName, pstSession->byShutDownType);
	}
	else
	{
		SYSF("session<%08X>, state[%d] after %s failed.", 
			pstSession->dwSessionID, pstSession->eState, szFucntionName);
	}

Exit0:
	return;
}



