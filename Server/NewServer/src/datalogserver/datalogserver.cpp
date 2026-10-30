#include "stdafx.h"
#include "mysql_owner.h"
#include "datalogserver.h"
#include "dump.h"

#include "global_config.h"
#include "server_config.h"
#include "obj_pool_config.h"
#include "process_option.h"
#include "linux_compatible.h"
#include "version.h"
#include "ifile.h"
#include "statis_info.h"
#include "data_manager.h"

#include "net.h"
#include "cer_message.h"
#include "zlib.h"

#include "dl_session_mgr.h"
#include "dl_message_handler.h"

#include "gameworld/game_world.h"

#include "db/dl_sql_stmt_list.h"

#include "rune_data.h"
#include "soul_data.h"
#include "refine_data.h"
#include "shop_data.h"
#include "level_data.h"
#include "init_reputation_data.h"
#include "spirit_data.h"
#include "npc_relation_data.h"
#include "role_relation_data.h"
#include "mount_data.h"
#include "circle_quest_data.h"
#include "pk_drop_rate_data.h"
#include "pk_drop_count_data.h"
#include "compose_data.h"
#include "decompose_data.h"
#include "activity_reward_data.h"
#include "adventure_data.h"
#include "money_data.h"
#include "mail_template_data.h"
#include "guild_building_data.h"
#include "set_data.h"
#include "scene_data.h"
#include "npc_data.h"
#include "doodad_data.h"
#include "achievement_data.h"
#include "db_init.h"
#include "mysql.h"

#define CURRENT_LOG_DB_VERSION	(5)

CDatalogServer CDatalogServer::ms_Instance;

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

        return;
Exit0:
        SAFE_DELETE_ARRAY(m_pMathTable);
        return;

    }
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bPassiveShutdown) {}

    char*   m_pMathTable;
};

CDatalogServer::CDatalogServer()
{
	m_pNet = NULL;
	m_pGSSocketAcceptor = NULL;
}

CDatalogServer::~CDatalogServer()
{
	UnInit();
}

CDatalogServer& CDatalogServer::GetInstance(void)
{
	return ms_Instance;
}

BOOL CDatalogServer::_InitLogDataBase()
{
    BOOL bRet = FALSE;
    cqyh::MysqlPtr mysql;
    MYSQL* res_mysql = NULL;

    mysql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(mysql.get());

    res_mysql = mysql_real_connect(
        mysql.get(),
        m_dbsetting.szServerAddr,
        m_dbsetting.szLoginName,
        m_dbsetting.szPassword,
        NULL,
        m_dbsetting.dwServerPort,
        NULL,
        CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS);
    LOG_PROCESS_ERROR(res_mysql);

    if (!ExistDB(mysql.get(), m_dbsetting.szDBName))
    {
        bRet = CreateDB(mysql.get(), m_dbsetting.szDBName);
        LOG_PROCESS_ERROR(bRet);
    }

    bRet = UpdateDB(mysql.get(), m_dbsetting.szDBName, "logdata", CURRENT_LOG_DB_VERSION);
    LOG_PROCESS_ERROR(bRet);


    return TRUE;

Exit0:

    return FALSE;
}

BOOL CDatalogServer::_InitMathTable(void)
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

BOOL CDatalogServer::Init(INetEventHandler* poMessageHandler)
{
	INT32 nRetCode = 0;
	struct in_addr addr;
    INTRANET_INIT_PARAM IntranetInitParam;
    SIMPLE_INIT_PARAM SimpleInitParam;

	LOG_PROCESS_ERROR(poMessageHandler);

	m_nGameTime = 0;

	nRetCode = m_PacketAllocator.Init(g_ServerConfig.DL.Net.nSmallPacketCount,
		g_ServerConfig.DL.Net.nLargePacketCount);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitMathTable();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitScript();
    LOG_PROCESS_ERROR(nRetCode);

	// internal
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.DL.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.DL.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.DL.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.DL.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.DL.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.DL.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nServerPingTimeout;
	m_pNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poMessageHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.DL.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.DL.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.DL.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.DL.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.DL.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.DL.Net.nPacketBufferSize;
	m_pNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poMessageHandler, &SimpleInitParam);
#endif
	LOG_PROCESS_ERROR(m_pNet);

	m_pGSSocketAcceptor = m_pNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pGSSocketAcceptor);

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, FALSE, 0);
	LOG_PROCESS_ERROR(addr.s_addr);
	addr.s_addr = 0;

	nRetCode = m_pGSSocketAcceptor->Open(addr.s_addr, g_ServerConfig.DL.dwPort);
	LOG_PROCESS_ERROR(nRetCode);

	// DB

	strxcpy(m_dbsetting.szServerAddr, g_ServerConfig.DB.szLogDBIPAddress, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szLoginName, g_ServerConfig.DB.szLogDBAccount, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szPassword, g_ServerConfig.DB.szLogDBPassword, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szDBName, g_ServerConfig.DB.szLogDBName, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szCharSet, g_ServerConfig.DB.szLogDBCharset, DB_SETTING_STRING_LEN);
    m_dbsetting.dwServerPort = g_ServerConfig.DB.dwLogDBPort;

	if (g_ServerConfig.DB.bUseIndependentDB)
	{
		char szHostName[256];
		char* psz = szHostName;
		gethostname(szHostName, 256);

		while (*psz)
		{
			if (*psz == '-')
				*psz = '_';
			psz++;
		}
        snprintf(m_dbsetting.szDBName, DB_SETTING_STRING_LEN, "%s_log", szHostName);
	}

    nRetCode = _InitLogDataBase();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDatalogServer::GetInstance().GetSqlMgr().Init(
		m_dbsetting.szServerAddr,
		m_dbsetting.szLoginName,
		m_dbsetting.szPassword,
		m_dbsetting.szDBName,
		m_dbsetting.szCharSet,
		m_dbsetting.dwServerPort,
        g_DLSqlStmtList, dsqlTotal, 
        MAX_DB_WORKER_THREAD_COUNT);
	LOG_PROCESS_ERROR(nRetCode);

    INF("opened GS listen port at %s:%d", inet_ntoa(addr), g_ServerConfig.DL.dwPort);

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CDatalogServer::UnInit(void)
{
	INT32 nRetCode = 0;

    nRetCode = _UnInitScript();
    LOG_CHECK_ERROR(nRetCode);

	if (m_pGSSocketAcceptor)
	{
		nRetCode = m_pGSSocketAcceptor->Close();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_pNet->DestroyAcceptor(m_pGSSocketAcceptor);
		LOG_CHECK_ERROR(nRetCode);

		m_pGSSocketAcceptor = NULL;
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

BOOL CDatalogServer::MainLoop(void)
{
	INT32 nRetCode = 0;
	UINT64 qwCurrentTick = GetTickCountEx();
	UINT32 dwCurrentTime = (UINT32)time(NULL);

	CGameWorld::GetInstance().SetGameTick(qwCurrentTick);
	CGameWorld::GetInstance().SetGameTime(dwCurrentTime);

	nRetCode = CDLSessionMgr::GetInstance().MainLoop();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_pNet->MainLoop();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDLMessageHandler::GetInstance().MainLoop();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_SqlMgr.MainLoop();
	LOG_CHECK_ERROR(nRetCode);

	UINT64 qwEndTick = GetTickCountEx();
	if (qwEndTick - qwCurrentTick < (1000 / GAME_FPS))
		Sleep(16);
	else
		CRI("too slow main loop %d", (INT32)(qwEndTick - qwCurrentTick));

	return TRUE;
}

BOOL CDatalogServer::Close(void)
{
	INT32 nRetCode = 0;
	CDLSessionMgr& rSessionMgr = CDLSessionMgr::GetInstance();

	nRetCode = CDLMessageHandler::GetInstance().DoMSCloseAck();
	LOG_CHECK_ERROR(nRetCode);

	while (CDLSessionMgr::GetInstance().GetMSSession().bConnected)
	{
		nRetCode = MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	CDLSessionMgr::GetInstance().CloseAllSession();
	while (CDLSessionMgr::GetInstance().CheckSession())
	{
		nRetCode = MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

	CDLSessionMgr::GetInstance().SetServerState(sgsClosed);

	return TRUE;
Exit0:
	return FALSE;
}

/************************************************************************/

BOOL InitGameWorldData()
{
	INT32 nRetCode = 0;
	char szFilePath[PATH_LEN];
	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);

	nRetCode = g_InitReadDataScript(g_GWConfig.szConfigPath, READ_DATA_SCRIPT_NAME);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ITEM_DATA>::GetInstance().Init(ITEM_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SET_DATA>::GetInstance().Init(SET_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<RUNE_DATA>::GetInstance().Init(RUNE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SOUL_DATA>::GetInstance().Init(SOUL_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<REFINE_DATA>::GetInstance().Init(REFINE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SHOP_DATA>::GetInstance().Init(SHOP_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SCENE_DATA>::GetInstance().Init(SCENE_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ROLE_LEVEL_DATA>::GetInstance().Init(LEVEL_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<HERO_LEVEL_DATA>::GetInstance().Init(HERO_LEVEL_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<NPC_DATA>::GetInstance().Init(NPC_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<DOODAD_DATA>::GetInstance().Init(DOODAD_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<TRASFORM_DATA>::GetInstance().Init(TRANSFORM_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<TITLE_TEMPLATE>::GetInstance().Init(TITLE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<DUNGEON_DATA>::GetInstance().Init(DUNGEON_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<QUEST_RECYCLE_DATA>::GetInstance().Init(QUEST_RECYCLE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<REWARD_QUEST_MONEY_COST_DATA>::GetInstance().Init(REWARD_QUEST_MONEY_COST_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<INIT_REPUTATION_DATA>::GetInstance().Init(INIT_REPUTATATION_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<NPC_RELATION_DATA>::GetInstance().Init(NPC_RELATION_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ROLE_RELATION_DATA>::GetInstance().Init(ROLE_RELATION_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ACHIEVEMENT_DATA>::GetInstance().Init(ACHIEVEMENT_FILE_NAME, szFilePath, NULL);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<MOUNT_ENTRY>::GetInstance().Init(MOUNT_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_DATA>::GetInstance().Init(SPIRIT_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<AIR_LEVEL_DATA>::GetInstance().Init(AIR_LEVEL_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<AIR_DATA>::GetInstance().Init(AIR_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<BLOOD_DATA>::GetInstance().Init(BLOOD_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_SLOT_DATA>::GetInstance().Init(SPIRIT_SLOT_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<CIRCLE_REWARD_DATA>::GetInstance().Init(QUEST_REWARD_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<CIRCLE_QUEST_DATA>::GetInstance().Init(CIRCLE_QUEST_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<QUEST_DATA>::GetInstance().Init(QUEST_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<GUILD_BUILDING_DATA>::GetInstance().Init(GUILD_BUILDING_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<DECOMPOSE_DATA>::GetInstance().Init(DECOMPOSE_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<COMPOSE_DATA>::GetInstance().Init(COMPOSE_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<PK_DROP_COUNT_DATA>::GetInstance().Init(PK_DROP_COUNT_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<PK_DROP_RATE_DATA>::GetInstance().Init(PK_DROP_RATE_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ACTIVITY_REWARD_DATA>::GetInstance().Init(ACTIVITY_REWARD_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ADVENTURE_DATA>::GetInstance().Init(ADVENTURE_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<MONEY_DATA>::GetInstance().Init(MONEY_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().Init(MAIL_TEMPLATE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_CHANGE_DATA>::GetInstance().Init(SPIRIT_CHANGE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL SetSpecialDLConfig()
{
    OBJ_POOL_CONFIG& rcParam = g_ObjPoolConfig;

	rcParam.nExtraModifierNodeCount = 65536;
	rcParam.nBTNodeCount = 1;
	rcParam.nBTCtxCount = 1;
	rcParam.nAITemplateCount = 1;
	rcParam.nSpellInstanceCount = 1;
	rcParam.nSpellSetCount = 1;
	rcParam.nAuraInstanceCount = 1;
	rcParam.nRuneCount = 1;
	rcParam.nBulletCount = 1;
	rcParam.nThreadNodeCount = 1;
	rcParam.nRoleCount = 1;
	rcParam.nNpcCount = 1;
	rcParam.nDoodadCount = 1;
	rcParam.nMissileCount = 1;
	rcParam.nDropCount = 1;
	rcParam.nTeamCount = 1;
	rcParam.nRegionCount = 1;
	rcParam.nSceneCount = 1;

	return TRUE;
}

/************************************************************************/

BOOL _InitServer(void)
{
	INT32 nRetCode = 0;
	CDatalogServer& rDLServer = CDatalogServer::GetInstance();
	CDLMessageHandler& rMessageHandler = CDLMessageHandler::GetInstance();

	nRetCode = CLog::GetInstance().Init("datalog_server", 
        g_ServerConfig.DL.Log.nLogLevelMask, 
		g_ServerConfig.DL.Log.nLogOutputMask, 
        g_ServerConfig.DL.Log.bEnableLogThread, 
        g_ServerConfig.DL.Log.bPrintThreadID,
        g_ServerConfig.DL.Log.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);

    INF("initializing data log server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);

	nRetCode = CheckDiskRemainCapacity();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDatalogServer::GetInstance().Init(&rMessageHandler);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = rMessageHandler.Init(rDLServer.GetInternalNet(), rDLServer.GetPacketAllocator(), rDLServer.GetGSSocketAcceptor());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDLSessionMgr::GetInstance().Init(rDLServer.GetInternalNet());
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = SetSpecialDLConfig();
	LOG_PROCESS_ERROR(nRetCode);

	//{
	//	UINT32 t = time(NULL);
	//	UINT64 qwStartTick = GetTickCountEx();
	//	for (INT32 nIndex = 0; nIndex < 100000; nIndex++)
	//	{
	//		//nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertLogin, NULL, NULL, "dccsss", 
	//		//	t, 2, 2, "name", GetLoginStringByType(dlLogin), "127.0.0.1");
	//		//LOG_CHECK_ERROR(nRetCode);

	//		nRetCode = CDatalogServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(dsqlInsertServerMonitorNpcDie, NULL, NULL, "dhcdd",
	//			t, 1, 1, 0, nIndex);
	//		LOG_CHECK_ERROR(nRetCode);
	//	}

	//	while (!CDatalogServer::GetInstance().GetSqlMgr().IsCompleted())
	//	{
	//		CDatalogServer::GetInstance().GetSqlMgr().MainLoop();
	//	}

	//	INT64 llInterval = GetTickCountEx() - qwStartTick;
	//	INF("%.2f", 100000.0 * 1000 / llInterval);
	//}


	nRetCode = g_InitGameWorldLib(GetTickCountEx(), rDLServer.GetDLMessageNotifier(), rDLServer.GetPacketAllocator());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitServer(void)
{
	INT32 nRetCode = 0;

	nRetCode = CGameWorld::GetInstance().CleanUpScenes();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = g_UnInitGameWorldLib();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDLSessionMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDLMessageHandler::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDatalogServer::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = g_ClosePakFolder();
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

	SetWindowPos(GetConsoleWindow(), HWND_TOP, 700, 0, 0, 0, SWP_NOSIZE);
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
    ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
    {
        char szTitle[MAX_PATH];
        snprintf(szTitle, MAX_PATH, "DL Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

	/************************************************************************/
	// main loop
	while (CDLSessionMgr::GetInstance().GetServerState() != sgsClosing)
	{
#if defined(WIN32)
		__try
		{
			nRetCode = CDatalogServer::GetInstance().MainLoop();
			LOG_PROCESS_ERROR(nRetCode);
		}
		__except (DumpExceptionFilter(GetExceptionInformation()))
		{
		}
#elif defined(__linux__)
		nRetCode = CDatalogServer::GetInstance().MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
#endif 
	}

	/************************************************************************/
	// exit
	nRetCode = CDatalogServer::GetInstance().Close();
	LOG_PROCESS_ERROR(nRetCode);
    INF("DL is exiting");

Exit0:
    INF("DL has exited");

    nRetCode = _UnInitServer();
	LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif

	return 0; 
}



