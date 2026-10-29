#include "stdafx.h"
#include "dbserver.h"
#include "dump.h"

#include <time_ex.h>

#include "global_config.h"
#include "server_config.h"
#include "process_option.h"
#include "version.h"
#include "statis_info.h"

#include "report.h"
#include "net.h"

#include "db_session_mgr.h"
#include "ifile.h"
#include "new_role_config.h"

CDBServer CDBServer::ms_Instance;

CDBServer::CDBServer()
{
    m_pNet = NULL;
    m_pSocketAcceptor = NULL;
    m_threads = NULL;
    m_nOnlineUserCount = 0;
    m_bForceClose = FALSE;
    m_dwThreadCount = 0;
    memset(&m_dbsetting, 0, sizeof(m_dbsetting));
}

CDBServer::~CDBServer()
{
    UnInit();
}

BOOL CDBServer::Init(INetEventHandler* poNetEventHandler)
{
    INT32 nRetCode = 0;
    struct in_addr addr;
#if defined(WIN32)
    INTRANET_INIT_PARAM IntranetInitParam;
#else
    SIMPLE_INIT_PARAM SimpleInitParam;
#endif
    LOG_PROCESS_ERROR(poNetEventHandler);


    nRetCode = m_PacketAllocator.Init(g_ServerConfig.DB.Net.nSmallPacketCount,
        g_ServerConfig.DB.Net.nLargePacketCount);
    LOG_PROCESS_ERROR(nRetCode);

    // logic
    nRetCode = m_memmgr.Init(g_ServerConfig.DB.dwMaxCache);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_rolemgr.Init(g_ServerConfig.DB.dwMaxCache);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_usermgr.Init(g_ServerConfig.DB.dwMaxCache);
    LOG_PROCESS_ERROR(nRetCode);

    // internal
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.DB.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.DB.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.DB.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.DB.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.DB.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.DB.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
    m_pNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poNetEventHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.DB.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.DB.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.DB.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.DB.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.DB.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.DB.Net.nPacketBufferSize;
    m_pNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &SimpleInitParam);
#endif
    LOG_PROCESS_ERROR(m_pNet);

    m_pSocketAcceptor = m_pNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pSocketAcceptor);

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
    LOG_PROCESS_ERROR(addr.s_addr);

    nRetCode = m_pSocketAcceptor->Open(addr.s_addr, g_ServerConfig.DB.dwPort);
    LOG_PROCESS_ERROR(nRetCode);

    m_bForceClose = FALSE;

    INF("opened listen port at %s:%d", inet_ntoa(addr), g_ServerConfig.DB.dwPort);

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CDBServer::UnInit(void)
{
    INT32 nRetCode = 0;
    BOOL bRet = FALSE;

    if (m_pSocketAcceptor)
    {
        nRetCode = m_pSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_pNet->DestroyAcceptor(m_pSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pSocketAcceptor = NULL;
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

    if (m_threads)
    {
        for (UINT32 i = 0; i < m_dwThreadCount; ++i)
            m_threads[i].RequestStop();
        for (UINT32 i=0; i<m_dwThreadCount; i++)
        {
            m_threads[i].Wait();
            bRet = m_threads[i].UnInit();
            LOG_CHECK_ERROR(bRet);
        }
        SAFE_DELETE_ARRAY(m_threads);
    }

    bRet = m_memmgr.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_rolemgr.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_usermgr.UnInit();
    LOG_CHECK_ERROR(bRet);

    return TRUE; 
}

BOOL CDBServer::InitDataBaseAndWorkerThread(const char* serveraddr,
                                            const char* loginname,
                                            const char* password,
                                            const char* dbname,
                                            const char* charset,
                                            UINT32 serverport)
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(m_threads == NULL);

    strxcpy(m_dbsetting.szServerAddr, serveraddr, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szLoginName, loginname, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szPassword, password, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szDBName, dbname, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szCharSet, charset, DB_SETTING_STRING_LEN);
    m_dbsetting.dwServerPort = serverport;

    m_dwThreadCount = g_ServerConfig.DB.dwThreadCount;
    m_threads = new CDBWorkerThread[m_dwThreadCount];
    for (UINT32 i=0; i<m_dwThreadCount; i++)
    {
        bRet = m_threads[i].Init(g_ServerConfig.DB.dwMaxCache);
        LOG_PROCESS_ERROR(bRet);
    }

    for (UINT32 i=0; i<m_dwThreadCount; i++)
    {
        bRet = m_threads[i].Start();
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CDBServer::Close(void)
{
    INT32 nRetCode = 0;
    CDBSessionMgr & rDBSessionMgr = CDBSessionMgr::GetInstance();
    UINT64 qwStartTick = GetTickCountEx();
    
    while (m_nOnlineUserCount > 0)
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        if (m_bForceClose && GetTickCountEx() - qwStartTick > g_ServerConfig.DB.dwDBCloseTimeout)
        {
            CRI("CDBServer::Close break MainLoop since dwDBCloseTimeout %d", g_ServerConfig.DB.dwDBCloseTimeout );
            break;
        }
    }

    nRetCode = CDBMessageHandler::GetInstance().DoMSCloseAck();
    LOG_CHECK_ERROR(nRetCode);

    CDBSessionMgr::GetInstance().CloseAllSession();
    while (CDBSessionMgr::GetInstance().CheckSession())
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }

    CDBSessionMgr::GetInstance().SetServerState(sgsClosed);
    INF("DB is closed");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CDBServer::MainLoop(void)
{
    INT32 nRetCode = 0;
    BOOL bRet = FALSE;

    nRetCode = CDBSessionMgr::GetInstance().MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pNet->MainLoop();
    LOG_PROCESS_ERROR(nRetCode);

    //real main loop for dbserver
    for (UINT32 i=0; i<m_dwThreadCount; i++)
    {
        for (;;)
        {
            CDBUser* dbuser = m_threads[i].GetFinishedUserTask();

            if (dbuser == NULL)
                break;

            dbuser->OnListRole();
        }
    }
    for (UINT32 i=0; i<m_dwThreadCount; i++)
    {
        for (;;)
        {
            CDBRole* dbrole = m_threads[i].GetFinishedRoleDbTask();

            if (dbrole == NULL)
                break;

            switch (dbrole->m_dbtask)
            {
            case CDBRole::DB_TASK_CREATE_ROLE:    // Create character
                dbrole->OnCreateRole();
                break;
            case CDBRole::DB_TASK_UPDATE_ROLE_DELETE_TIME:
            case CDBRole::DB_TASK_UPDATE_ROLE_DELETE_TIME_WITHOUT_DATA:
                dbrole->OnUpdateRoleDeleteTime();
                break;
            case CDBRole::DB_TASK_DELETE_ROLE:
            case CDBRole::DB_TASK_DELETE_ROLE_WITHOUT_DATA:
                dbrole->OnDeleteRole();
                break;
            case CDBRole::DB_TASK_LOAD_ROLE:
                dbrole->OnLoadRole();
                break;
            default:
                LOG_PROCESS_ERROR(FALSE);
            }
        }

        for (;;)
        {
            CDBRole* dbrole = m_threads[i].GetFinishedRoleLockTask();

            if (dbrole == NULL)
                break;

            switch (dbrole->m_lockstate)
            {
            case CDBRole::LOCK_STATE_LOCKING:
                dbrole->m_lockstate = CDBRole::LOCK_STATE_LOCKED;
                break;
            case CDBRole::LOCK_STATE_LOCKING_AND_UNLOCKING:
                dbrole->m_lockstate = CDBRole::LOCK_STATE_UNLOCKING;
                bRet = m_threads[i].AddRoleLockTask(dbrole);
                LOG_CHECK_ERROR(bRet);
                break;
            case CDBRole::LOCK_STATE_UNLOCKING:
                dbrole->m_lockstate = CDBRole::LOCK_STATE_UNLOCKED;
                bRet = m_rolemgr.Push(dbrole);
                LOG_CHECK_ERROR(bRet);
                bRet = dbrole->RefreshBaseData() != NULL;
                LOG_CHECK_ERROR(bRet);
                break;
            case CDBRole::LOCK_STATE_UNLOCKING_AND_LOCKING:
                dbrole->m_lockstate = CDBRole::LOCK_STATE_LOCKING;
                bRet = m_threads[i].AddRoleLockTask(dbrole);
                LOG_CHECK_ERROR(bRet);
                break;
            default:
                LOG_PROCESS_ERROR(FALSE);
            }
        }
    }
    //end for real main loop
    g_SetStatisInfo(statRoleCount, CDBServer::GetInstance().m_nOnlineUserCount);

    return TRUE;
Exit0:
    return FALSE;
}

/************************************************************************/

BOOL _InitServer(void)
{
    INT32 nRetCode = 0;
    CDBMessageHandler& rDBMessageHandler = CDBMessageHandler::GetInstance();
    CDBServer& rDBServer = CDBServer::GetInstance();

    nRetCode = CLog::GetInstance().Init("db_server", 
        g_ServerConfig.DB.Log.nLogLevelMask, 
        g_ServerConfig.DB.Log.nLogOutputMask, 
        g_ServerConfig.DB.Log.bEnableLogThread, 
        g_ServerConfig.DB.Log.bPrintThreadID,
        g_ServerConfig.DB.Log.bFlushFile);
    LOG_PROCESS_ERROR(nRetCode);

	g_InitReportSetting(GetServerShortName(stDB), 1, g_ServerConfig.Common.szReportAbsolutePath);

    INF("initializing db server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
	
    nRetCode = rDBServer.Init(&rDBMessageHandler);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = rDBMessageHandler.Init(rDBServer.m_pNet, &rDBServer.m_PacketAllocator);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDBSessionMgr::GetInstance().Init(rDBServer.m_pNet);
	LOG_PROCESS_ERROR(nRetCode);

#if defined(_DEBUG) & !defined(_FINAL)
	{
		char szHostName[256];
		BOOL bProgrammer = FALSE;

		gethostname(szHostName, 256);

		static constexpr const char* szDeveloperList[] = {
			"sh-zhujianqiu-d",
			"sh-lixj01-d",
			"sh-wubin02-d",
		};

		int DevelopersCount = sizeof(szDeveloperList) / sizeof(szDeveloperList[0]);
		for (int i = 0; i < DevelopersCount; ++i)
		{
			if (0 == stricmp(szHostName, szDeveloperList[i]))
			{
				bProgrammer = TRUE;
				break;
			}
		}

		if (bProgrammer)
		{
			g_NewRoleConfig.nSceneTempID = 1;
			g_NewRoleConfig.nX = 1000;
			g_NewRoleConfig.nY = 1000;
			g_NewRoleConfig.nZ = 20000;
			ConvertCoordO2N(g_NewRoleConfig.nX, g_NewRoleConfig.nY, g_NewRoleConfig.nZ);
		}
	}

#endif

    return TRUE;
Exit0:

    return FALSE;
}

BOOL _UnInitServer(void)
{    
    INT32 nRetCode;

    nRetCode = CDBSessionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDBMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDBServer::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

INT32 main(INT32 argc, TCHAR** argv) 
{ 
    INT32 nRetCode = 0;

#if defined(WIN32) | defined(WIN64)
	nRetCode = StartDumpThread(NULL, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    SetWindowPos(GetConsoleWindow(), HWND_TOP, 200, 0, 0, 0, SWP_NOSIZE);
#endif
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
    {
        char szTitle[MAX_PATH];
        snprintf(szTitle, MAX_PATH, "DB Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

    /************************************************************************/
    // main loop
    while (CDBSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
#if defined(WIN32)
		__try
		{
			nRetCode = CDBServer::GetInstance().MainLoop();
			LOG_PROCESS_ERROR(nRetCode);
		}
		__except (DumpExceptionFilter(GetExceptionInformation()))
		{
		}
#elif defined (__linux__)
		nRetCode = CDBServer::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
#endif 

        Sleep(1);
    }

    /************************************************************************/
    // exit
    INF("DB is exiting");
    
    nRetCode = CDBServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("DB has exited");

	nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif

    return 0; 
}

void DBSysLogSuccess(CDBUser* poDBuser, const char* szFucntionName)
{
	PROCESS_ERROR(poDBuser);

	if(poDBuser->m_pPlayingRole)
	{
		SYST("session<%08X>, user[%s], state[%d], taskstate[%d] after %s success. playing role[%d], dbtask[%d], lockstate[%d]", 
			poDBuser->m_dwClientSessionID, poDBuser->m_szUserName, poDBuser->m_state, poDBuser->m_dbstate, szFucntionName, 
			poDBuser->m_pPlayingRole->m_dwRoleID, poDBuser->m_pPlayingRole->m_dbtask, poDBuser->m_pPlayingRole->m_lockstate);
	}
	else
	{
		SYST("session<%08X>, user[%s], state[%d], taskstate[%d] after %s success.", 
			poDBuser->m_dwClientSessionID, poDBuser->m_szUserName, poDBuser->m_state, poDBuser->m_dbstate, szFucntionName);
	}
Exit0:
	return;
}

void DBSysLogError(CDBUser* poDBuser, const char* szFucntionName)
{
	PROCESS_ERROR(poDBuser);

	if(poDBuser->m_pPlayingRole)
	{
		SYST("session<%08X>, user[%s], state[%d], taskstate[%d] after %s failed. playing role[%d], dbtask[%d], lockstate[%d]", 
			poDBuser->m_dwClientSessionID, poDBuser->m_szUserName, poDBuser->m_state, poDBuser->m_dbstate, szFucntionName, 
			poDBuser->m_pPlayingRole->m_dwRoleID, poDBuser->m_pPlayingRole->m_dbtask, poDBuser->m_pPlayingRole->m_lockstate);
	}
	else
	{
		SYST("session<%08X>, user[%s], state[%d], taskstate[%d] after %s failed.", 
			poDBuser->m_dwClientSessionID, poDBuser->m_szUserName, poDBuser->m_state, poDBuser->m_dbstate, szFucntionName);
	}
Exit0:
	return;
}


