#include "stdafx.h"
#include "mobileserver.h"
#include "dump.h"

#include "time_ex.h"
#include "report.h"
#include "ifile.h"

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"

#include "external_message_size.h"
#include "net.h"

#include "mm_session_mgr.h"

CMobileServer CMobileServer::ms_Instance;

CMobileServer::CMobileServer()
{
	m_pNet = NULL;
	m_pSocketAcceptor = NULL;
}

CMobileServer::~CMobileServer()
{
	UnInit();
}

CMobileServer& CMobileServer::GetInstance(void)
{
	return ms_Instance;
}

BOOL CMobileServer::Init(INetEventHandler* poMessageHandler)
{
	INT32 nRetCode = 0;
	INTRANET_INIT_PARAM IntranetInitParam;

	LOG_PROCESS_ERROR(poMessageHandler);

	nRetCode = m_PacketAllocator.Init(g_ServerConfig.MM.Net.nSmallPacketCount,
		g_ServerConfig.MM.Net.nLargePacketCount);
	LOG_PROCESS_ERROR(nRetCode);

	// internal
	IntranetInitParam.nMaxAcceptor = g_ServerConfig.MM.Net.nMaxAcceptor;
	IntranetInitParam.nMaxConnector = g_ServerConfig.MM.Net.nMaxConnector;
	IntranetInitParam.nMaxSessionCount = g_ServerConfig.MM.Net.nMaxSessionCount;
	IntranetInitParam.nSendBufSize = g_ServerConfig.MM.Net.nSendBufSize;
	IntranetInitParam.nRecvBufSize = g_ServerConfig.MM.Net.nRecvBufSize;
	IntranetInitParam.nPacketBufferSize = g_ServerConfig.MM.Net.nPacketBufferSize;
	IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;

	m_pNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poMessageHandler, &IntranetInitParam);
	LOG_PROCESS_ERROR(m_pNet);
	
	m_pSocketAcceptor = m_pNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pSocketAcceptor);

	nRetCode = m_pSocketAcceptor->Open(inet_addr(g_ServerConfig.MM.szAddress), g_ServerConfig.MM.dwMGPort);
	LOG_PROCESS_ERROR(nRetCode);

	INF("opened MG listen port at %s:%d", g_ServerConfig.MM.szAddress, g_ServerConfig.MM.dwMGPort);

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CMobileServer::UnInit(void)
{
	INT32 nRetCode = 0;

	if (m_pNet)
	{
		if (m_pSocketAcceptor)
		{
			m_pNet->DestroyAcceptor(m_pSocketAcceptor);
			m_pSocketAcceptor = NULL;
		}

		nRetCode = g_DestroyNetInterface(netIntranet, m_pNet);
		LOG_CHECK_ERROR(nRetCode);

		m_pNet = NULL;
	}

	nRetCode = m_PacketAllocator.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL CMobileServer::MainLoop(void)
{
	UINT64 qwEndTick{};
	INT32 nRetCode = 0;
	UINT64 qwCurrentTick = GetTickCountEx();
	static UINT64 s_qwLastSessionTick = 0;

	if (qwCurrentTick - s_qwLastSessionTick > 1000)
	{
		nRetCode = CMMSessionMgr::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);

		s_qwLastSessionTick = qwCurrentTick;
	}

	nRetCode = m_pNet->MainLoop();
	LOG_PROCESS_ERROR(nRetCode);

	qwEndTick = GetTickCountEx();
	if (qwEndTick - qwCurrentTick < 10)
		Sleep(1);
	else if (qwEndTick - qwCurrentTick > 50)
		CRI("too slow main loop %lld", qwEndTick - qwCurrentTick);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CMobileServer::Close(void)
{
	INT32 nRetCode = 0;
	CMMSessionMgr& rSessionMgr = CMMSessionMgr::GetInstance();

	nRetCode = CMMMesageHandler::GetInstance().DoMSCloseAck();
	LOG_CHECK_ERROR(nRetCode);

	CMMSessionMgr::GetInstance().CloseAllSession();

	while (CMMSessionMgr::GetInstance().CheckSession())
	{
		nRetCode = MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	CMMSessionMgr::GetInstance().SetServerState(sgsClosed);
	INF("MM is closed");

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

BOOL _InitServer(void)
{
	INT32 nRetCode = 0;
	CMobileServer& rGateServer = CMobileServer::GetInstance();
	CMMMesageHandler& rMessageHandler = CMMMesageHandler::GetInstance();

	nRetCode = CLog::GetInstance().Init("mobile_server",
		g_ServerConfig.MM.Log.nLogLevelMask,
		g_ServerConfig.MM.Log.nLogOutputMask,
		g_ServerConfig.MM.Log.bEnableLogThread,
		g_ServerConfig.MM.Log.bPrintThreadID,
		g_ServerConfig.MM.Log.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);
	g_InitReportSetting(GetServerShortName(stGT), 0, g_ServerConfig.Common.szReportAbsolutePath);

#ifdef _FINAL
	INF("initializing mobile server %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
	INF("initializing mobile server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL

	nRetCode = CMobileServer::GetInstance().Init(&rMessageHandler);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rMessageHandler.Init(rGateServer.GetNet(), rGateServer.GetPacketAllocator());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CMMSessionMgr::GetInstance().Init(rGateServer.GetNet());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitServer(void)
{
	INT32 nRetCode = 0;

	nRetCode = CMobileServer::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CMMMesageHandler::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CMMSessionMgr::GetInstance().UnInit();
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
	while (CMMSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
		nRetCode = CMobileServer::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	/************************************************************************/
	// exit
	INF("MM is exiting");
	nRetCode = CMobileServer::GetInstance().Close();
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("MM has exited");

	nRetCode = _UnInitServer();
	LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
	StopDumpThread();
#endif //WIN32
	return 0;
}
