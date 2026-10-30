#include "stdafx.h"
#include "balanceserver.h"
#include "dump.h"

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"
#include "string_ex.h"

#include "net.h"

#include "bs_session_mgr.h"
#include "external_message_handler.h"
#include "internal_message_handler.h"
#include "external_message_size.h"

#include "ifile.h"
//#define _MORE_NET_BIND_

CBalanceServer CBalanceServer::ms_Instance;

CBalanceServer::CBalanceServer()
{
	m_pExternalNet = NULL;
	m_pInternalNet = NULL;
	m_pGTSocketAcceptor = NULL;
	m_nExternalAddrCount = 0;
	memset(m_pClientSocketAcceptor, 0, sizeof(m_pClientSocketAcceptor));
	memset(m_dwExternalAddr, 0, sizeof(m_dwExternalAddr));
}

CBalanceServer::~CBalanceServer()
{
	UnInit();
}

CBalanceServer& CBalanceServer::GetInstance(void)
{
	return ms_Instance;
}

BOOL CBalanceServer::Init(INetEventHandler* poInterMessageHandler, INetEventHandler* poExternalMessageHandler)
{
	INT32 nRetCode = 0;
    ASIO_INIT_PARAM AsioInitParam;
	SIMPLE_INIT_PARAM SimpleInitParam;

	struct in_addr addr;
	INTRANET_INIT_PARAM IntranetInitParam;

	INT32 nExternalIndex = 0;
	INT32 nExternaladdrCount = 0;
	struct in_addr external_addr[8];

	LOG_PROCESS_ERROR(poInterMessageHandler);
	LOG_PROCESS_ERROR(poExternalMessageHandler);

	nRetCode = m_PacketAllocator.Init(g_ServerConfig.BS.Net.nSmallPacketCount,
		g_ServerConfig.BS.Net.nLargePacketCount);
	LOG_PROCESS_ERROR(nRetCode);

	// external
    AsioInitParam.nMaxSessionCount = g_ServerConfig.BS.nMaxClientSessionCount;
    AsioInitParam.nThreadCount = g_ServerConfig.BS.nWorkThreadCount;
    AsioInitParam.pnMessageSizeList = g_nC2SMessageSize;

	AsioInitParam.nMaxOperationPerLoop = g_ServerConfig.BS.Net.nMaxOperationPerLoop;
	AsioInitParam.nRecvLoopBufferSize = g_ServerConfig.BS.Net.nRecvLoopBufferSize;
	AsioInitParam.nSendLoopBufferSize = g_ServerConfig.BS.Net.nSendLoopBufferSize;

    m_pExternalNet = g_CreateNetInterface(netAsio, &m_PacketAllocator, poExternalMessageHandler, &AsioInitParam);
	LOG_PROCESS_ERROR(m_pExternalNet);

	// internal
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.BS.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.BS.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.BS.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.BS.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.BS.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.BS.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nServerPingTimeout;
    m_pInternalNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poInterMessageHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.BS.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.BS.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.BS.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.BS.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.BS.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.BS.Net.nPacketBufferSize;
    m_pInternalNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poInterMessageHandler, &SimpleInitParam);
#endif
	LOG_PROCESS_ERROR(m_pInternalNet);

	for (m_nExternalAddrCount = 0; m_nExternalAddrCount < MAX_ADAPTER_COUNT; m_nExternalAddrCount++)
	{
		m_dwExternalAddr[m_nExternalAddrCount] =
			GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, FALSE, m_nExternalAddrCount);
		if (m_dwExternalAddr[m_nExternalAddrCount] == 0)
			break;

		m_pClientSocketAcceptor[m_nExternalAddrCount] = m_pExternalNet->CreateAcceptor();
		LOG_PROCESS_ERROR(m_pClientSocketAcceptor[m_nExternalAddrCount]);

		nRetCode = m_pClientSocketAcceptor[m_nExternalAddrCount]->Open(
			m_dwExternalAddr[m_nExternalAddrCount], g_ServerConfig.BS.dwExternalPort);
		LOG_PROCESS_ERROR(nRetCode);

		INF("opened %d external listen port at %s:%d",
			m_nExternalAddrCount, inet_ntoa(*(in_addr*)&m_dwExternalAddr[m_nExternalAddrCount]), g_ServerConfig.BS.dwExternalPort);
	}
	LOG_PROCESS_ERROR(m_nExternalAddrCount > 0);

#if defined(_DEBUG)
	{
		m_dwExternalAddr[m_nExternalAddrCount] = 0x0100007F;

		m_pClientSocketAcceptor[m_nExternalAddrCount] = m_pExternalNet->CreateAcceptor();
		LOG_PROCESS_ERROR(m_pClientSocketAcceptor[m_nExternalAddrCount]);

		nRetCode = m_pClientSocketAcceptor[m_nExternalAddrCount]->Open(
			m_dwExternalAddr[m_nExternalAddrCount], g_ServerConfig.BS.dwExternalPort);
		LOG_PROCESS_ERROR(nRetCode);

		INF("opened %d external listen port at %s:%d",
			m_nExternalAddrCount, inet_ntoa(*(in_addr*)&m_dwExternalAddr[m_nExternalAddrCount]), g_ServerConfig.BS.dwExternalPort);

		m_nExternalAddrCount++;
	}
#endif	// _DEBUG

	m_pGTSocketAcceptor = m_pInternalNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pGTSocketAcceptor);

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
	LOG_PROCESS_ERROR(addr.s_addr);

	nRetCode = m_pGTSocketAcceptor->Open(addr.s_addr, g_ServerConfig.BS.dwInternalPort);
	LOG_PROCESS_ERROR(nRetCode);

    INF("opened GT listen port at %s:%d", inet_ntoa(addr), g_ServerConfig.BS.dwInternalPort);

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CBalanceServer::UnInit(void)
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

	if (m_pGTSocketAcceptor)
	{
		nRetCode = m_pGTSocketAcceptor->Close();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_pInternalNet->DestroyAcceptor(m_pGTSocketAcceptor);
		LOG_CHECK_ERROR(nRetCode);

		m_pGTSocketAcceptor = NULL;
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

BOOL CBalanceServer::MainLoop(void)
{
	INT32 nRetCode = 0;

	nRetCode = CBSSessionMgr::GetInstance().MainLoop();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pInternalNet->MainLoop();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pExternalNet->MainLoop();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBalanceServer::Close(void)
{
	INT32 nRetCode = 0;
	CBSSessionMgr& rSessionMgr = CBSSessionMgr::GetInstance();

	nRetCode = CBSInternalMessageHandler::GetInstance().DoMSCloseAck();
	LOG_CHECK_ERROR(nRetCode);

	CBSSessionMgr::GetInstance().CloseAllSession();
	while (CBSSessionMgr::GetInstance().CheckSession())
	{
		nRetCode = MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	CBSSessionMgr::GetInstance().SetServerState(sgsClosed);

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

BOOL _InitServer(void)
{
	INT32 nRetCode = 0;
	CBalanceServer& rBSServer = CBalanceServer::GetInstance();
	CBSExternalMessageHandler& rExternalMessageHandler = CBSExternalMessageHandler::GetInstance();
	CBSInternalMessageHandler& rInternalMessageHandler = CBSInternalMessageHandler::GetInstance();

	nRetCode = CLog::GetInstance().Init("balance_server", 
        g_ServerConfig.BS.Log.nLogLevelMask, 
		g_ServerConfig.BS.Log.nLogOutputMask, 
        g_ServerConfig.BS.Log.bEnableLogThread, 
        g_ServerConfig.BS.Log.bPrintThreadID,
        g_ServerConfig.BS.Log.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);

    INF("initializing balance server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);

	nRetCode = CBalanceServer::GetInstance().Init(&rInternalMessageHandler, &rExternalMessageHandler);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rExternalMessageHandler.Init(rBSServer.GetExternalNet(), rBSServer.GetPacketAllocator());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rInternalMessageHandler.Init(rBSServer.GetInternalNet(), rBSServer.GetPacketAllocator(), rBSServer.GetGTSocketAcceptor());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CBSSessionMgr::GetInstance().Init(rBSServer.GetExternalNet(), rBSServer.GetInternalNet());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitServer(void)
{
	INT32 nRetCode = 0;

	nRetCode = CBSSessionMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CBSInternalMessageHandler::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CBSExternalMessageHandler::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CBalanceServer::GetInstance().UnInit();
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

	SetWindowPos(GetConsoleWindow(), HWND_TOP, 100, 0, 0, 0, SWP_NOSIZE);
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

#if (defined(WIN32) || defined(WIN64))
    ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
    {
        char szTitle[MAX_PATH];
        snprintf(szTitle, MAX_PATH, "Balance Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

	/************************************************************************/
	// main loop
	while (CBSSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
		nRetCode = CBalanceServer::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);

		Sleep(16);
	}

	/************************************************************************/
	// exit
	nRetCode = CBalanceServer::GetInstance().Close();
	LOG_PROCESS_ERROR(nRetCode);
    INF("BS is exiting");

Exit0:
    INF("BS has exited");

    nRetCode = _UnInitServer();
	LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif //WIN32

	return 0; 
}
