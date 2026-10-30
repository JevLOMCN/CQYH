#include "stdafx.h"
#include "mgateserver.h"
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

#include "mg_session_mgr.h"
#include "ifile.h"

CMGateServer CMGateServer::ms_Instance;

CMGateServer::CMGateServer()
{
	m_pExternalNet = NULL;
	m_pInternalNet = NULL;

	m_dwInternalAddr = 0;
	m_dwInternalPort = 0;

	m_nExternalAddrCount = 0;
	memset(m_pClientSocketAcceptor, 0, sizeof(m_pClientSocketAcceptor));
	memset(m_dwExternalAddr, 0, sizeof(m_dwExternalAddr));
	memset(m_dwExternalPort, 0, sizeof(m_dwExternalPort));
}

CMGateServer::~CMGateServer()
{
	UnInit();
}

CMGateServer& CMGateServer::GetInstance(void)
{
	return ms_Instance;
}

BOOL CMGateServer::Init(INetEventHandler* poInterMessageHandler, INetEventHandler* poExternalMessageHandler)
{
	INT32 nRetCode = 0;
	ASIO_INIT_PARAM AsioInitParam;
	SIMPLE_INIT_PARAM SimpleInitParam;

	INTRANET_INIT_PARAM IntranetInitParam;

	LOG_PROCESS_ERROR(poInterMessageHandler);
	LOG_PROCESS_ERROR(poExternalMessageHandler);

	nRetCode = m_PacketAllocator.Init(g_ServerConfig.MG.Net.nSmallPacketCount,
		g_ServerConfig.MG.Net.nLargePacketCount);
	LOG_PROCESS_ERROR(nRetCode);

	// external
	AsioInitParam.nMaxSessionCount = g_ServerConfig.MG.nMaxClientSessionCount;
	AsioInitParam.nThreadCount = g_ServerConfig.MG.nWorkThreadCount;
	AsioInitParam.pnMessageSizeList = g_nC2SMessageSize;
	AsioInitParam.nMaxOperationPerLoop = g_ServerConfig.MG.Net.nMaxOperationPerLoop;
	AsioInitParam.nRecvLoopBufferSize = g_ServerConfig.MG.Net.nRecvLoopBufferSize;
	AsioInitParam.nSendLoopBufferSize = g_ServerConfig.MG.Net.nSendLoopBufferSize;
	m_pExternalNet = g_CreateNetInterface(netAsio, &m_PacketAllocator, poExternalMessageHandler, &AsioInitParam);
	LOG_PROCESS_ERROR(m_pExternalNet);

	// internal
#if defined(WIN32)
	IntranetInitParam.nMaxAcceptor = g_ServerConfig.MG.Net.nMaxAcceptor;
	IntranetInitParam.nMaxConnector = g_ServerConfig.MG.Net.nMaxConnector;
	IntranetInitParam.nMaxSessionCount = g_ServerConfig.MG.Net.nMaxSessionCount;
	IntranetInitParam.nSendBufSize = g_ServerConfig.MG.Net.nSendBufSize;
	IntranetInitParam.nRecvBufSize = g_ServerConfig.MG.Net.nRecvBufSize;
	IntranetInitParam.nPacketBufferSize = g_ServerConfig.MG.Net.nPacketBufferSize;
	IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
	m_pInternalNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poInterMessageHandler, &IntranetInitParam);
#else
	SimpleInitParam.nMaxAcceptor = g_ServerConfig.MG.Net.nMaxAcceptor;
	SimpleInitParam.nMaxConnector = g_ServerConfig.MG.Net.nMaxConnector;
	SimpleInitParam.nMaxSessionCount = g_ServerConfig.MG.Net.nMaxSessionCount;
	SimpleInitParam.nSendBufSize = g_ServerConfig.MG.Net.nSendBufSize;
	SimpleInitParam.nRecvBufSize = g_ServerConfig.MG.Net.nRecvBufSize;
	SimpleInitParam.nPacketBufferSize = g_ServerConfig.MG.Net.nPacketBufferSize;
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

		m_dwExternalPort[m_nExternalAddrCount] = (g_ServerConfig.MG.dwStartExternalPort + g_ServerConfig.MS.nInitBSCount) % 65536;
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

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CMGateServer::UnInit(void)
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

BOOL CMGateServer::MainLoop(void)
{
	UINT64 qwEndTick{};
	INT32 nRetCode = 0;
	UINT64 qwCurrentTick = GetTickCountEx();
	static UINT64 s_qwLastSessionTick = 0;

	if (qwCurrentTick - s_qwLastSessionTick > 1000)
	{
		nRetCode = CMGSessionMgr::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);

		s_qwLastSessionTick = qwCurrentTick;
	}

	nRetCode = m_pInternalNet->MainLoop();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pExternalNet->MainLoop();
	LOG_PROCESS_ERROR(nRetCode);

	qwEndTick = GetTickCountEx();
	if (qwEndTick - qwCurrentTick < 10)
		Sleep(1);
	else if (qwEndTick - qwCurrentTick > 50)
		CRI("too slow main loop %lld", qwEndTick - qwCurrentTick);

	g_SetStatisInfo(statRoleCount, CMGSessionMgr::GetInstance().GetClientSessionCount());

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMGateServer::Close(void)
{
	INT32 nRetCode = 0;
	CMGSessionMgr& rSessionMgr = CMGSessionMgr::GetInstance();

	nRetCode = rSessionMgr.CloseAllClientSession();
	LOG_PROCESS_ERROR(nRetCode);

	while (!rSessionMgr.IsAllClientSessionClosed())
	{
		nRetCode = CMGateServer::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	nRetCode = CMGInternalMessageHandler::GetInstance().DoMMCloseAck();
	LOG_CHECK_ERROR(nRetCode);

	CMGSessionMgr::GetInstance().CloseAllSession();
	while (CMGSessionMgr::GetInstance().CheckSession())
	{
		nRetCode = MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	CMGSessionMgr::GetInstance().SetServerState(sgsClosed);
	INF("GT %d is closed", CMGSessionMgr::GetInstance().GetSelfSessionID());

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

BOOL _InitServer(void)
{
	INT32 nRetCode = 0;
	CMGateServer& rGateServer = CMGateServer::GetInstance();
	CMGExternalMessageHandler& rExternalMessageHandler = CMGExternalMessageHandler::GetInstance();
	CMGInternalMessageHandler& rInternalMessageHandler = CMGInternalMessageHandler::GetInstance();

	nRetCode = CLog::GetInstance().Init("mgate_server",
		g_ServerConfig.MG.Log.nLogLevelMask,
		g_ServerConfig.MG.Log.nLogOutputMask,
		g_ServerConfig.MG.Log.bEnableLogThread,
		g_ServerConfig.MG.Log.bPrintThreadID,
		g_ServerConfig.MG.Log.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);
	g_InitReportSetting(GetServerShortName(stGT), 0, g_ServerConfig.Common.szReportAbsolutePath);

#ifdef _FINAL
	INF("initializing gate server %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
	INF("initializing gate server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL

	nRetCode = CMGateServer::GetInstance().Init(&rInternalMessageHandler, &rExternalMessageHandler);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rExternalMessageHandler.Init(rGateServer.GetExternalNet(), rGateServer.GetPacketAllocator());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rInternalMessageHandler.Init(rGateServer.GetInternalNet(), rGateServer.GetPacketAllocator());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CMGSessionMgr::GetInstance().Init(rGateServer.GetExternalNet(), rGateServer.GetInternalNet());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitServer(void)
{
	INT32 nRetCode = 0;

	nRetCode = CMGateServer::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CMGInternalMessageHandler::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CMGExternalMessageHandler::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CMGSessionMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CLog::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

INT32 main(INT32 argc, TCHAR** argv)
{
	INT32 nRetCode = 0;
	UINT32 dwLastSleepTick = 0;

#if defined(WIN32)
	nRetCode = StartDumpThread(NULL, FALSE, "log");
	PROCESS_ERROR(nRetCode);

	SetWindowPos(GetConsoleWindow(), HWND_TOP, 1940, 5, 0, 0, SWP_NOSIZE);
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
	while (CMGSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
		nRetCode = CMGateServer::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	/************************************************************************/
	// exit
	INF("MG %d is exiting", CMGSessionMgr::GetInstance().GetSelfSessionID());
	nRetCode = CMGateServer::GetInstance().Close();
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("MG has exited");

	nRetCode = _UnInitServer();
	LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
	StopDumpThread();
#endif //WIN32
	return 0;
}

void MGSysLogSuccess(MG_CLIENT_SESSION* pstSession, const char* szFucntionName)
{
	PROCESS_ERROR(pstSession);

	if (!pstSession->bConnected)
	{
		SYST("session<%08X> after %s success. shutdowntype[%d],",
			pstSession->dwSessionID, szFucntionName, pstSession->byShutDownType);
	}
	else
	{
		SYST("session<%08X> after %s success.",
			pstSession->dwSessionID, szFucntionName);
	}

Exit0:
	return;
}

void MGSysLogError(MG_CLIENT_SESSION* pstSession, const char* szFucntionName)
{
	PROCESS_ERROR(pstSession);

	if (!pstSession->bConnected)
	{
		SYSF("session<%08X> after %s failed. shutdowntype[%d],",
			pstSession->dwSessionID, szFucntionName, pstSession->byShutDownType);
	}
	else
	{
		SYSF("session<%08X> after %s failed.",
			pstSession->dwSessionID, szFucntionName);
	}

Exit0:
	return;
}
