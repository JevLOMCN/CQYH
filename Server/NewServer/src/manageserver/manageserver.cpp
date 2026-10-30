#include "stdafx.h"
#include "mysql_owner.h"
#include "manageserver.h"

#include <time.h>

#include "global_config.h"
#include "server_config.h"
#include "game_world_config.h"
#include "process_option.h"
#include "version.h"
#include "time_ex.h"
#include "report.h"
#include "dump.h"
#include "ifile.h"
#include "script_mgr.h"
#include "statis_info.h"
#include "sql_mgr.h"
#include "math_ex.h"
#include "zlib.h"
#include "trie.h"
#include "censored_word_list.h"
#include "guild_achievement.h"
#include "guild_achievement_cdtn.h"
#include "net.h"
#include "cer_message.h"

#include "ms_session_mgr.h"
#include "client/usermgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "scene_data.h"
#include "gs_info_mgr.h"
#include "scene/scenemgr.h"
#include "team/team.h"
#include "gs_rsakey_mgr.h"
#include "team/room.h"
#include "master/master_mgr.h"
#include "mail/mail_mgr.h"
#include "timer/timermgr.h"
#include "master_gift.h"
#include "queuematch/battlequeuemgr.h"
#include "guild/ms_guild_mgr.h"
#include "math_ex.h"
#include "./script/ms_script_func.h"
#include "auction/auction_mgr.h"
#include "exchange/exchange_mgr.h"
#include "exchange/exchange_history.h"
#include "db_init.h"
#include "drop/ms_drop_mgr.h"
#include "ranklist/ranklist.h"
#include "guild_building_data.h"
#include "guild_welfare_data.h"
#include "guild_dungeon_data.h"
#include "real_money_shop/real_money_shop.h"
#include "real_money_shop/oa_bill_mgr.h"
#include "ms_adventure.h"
#include "data/arena_data.h"
#include "award/award_mgr.h"
#include "./gm/gm_manager.h"
#include "data/activity_data.h"
#include "achievement_data.h"
#include "dungeon_data.h"
#include "db/ms_sql_stmt_list.h"
#include "mail_template_data.h"
#include "server_var.h"
#include "money_data.h"
#include "arena/arena_mgr.h"
#include "data/ms_post_process_data.h"
#include "fcm/fcm.h"
#include "./db/pre_save_list.h"
#include "guild/ms_guild_achi_mgr.h"
#include "black_market_data.h"
#include "blackmarket/ms_black_market_mgr.h"
#include "guild_activity.h"
#include "rune_data.h"
#include "guild_def.h"
#include "chat/ms_chat.h"

#define CURRENT_DB_VERSION	(8)

#define DAILY_TRIGGER_TIME	(3600 * 4)

#define PROCESS_QUEUE_INTERVAL (10)

CManageServer CManageServer::ms_Instance;

char g_StringTable[STRING_TABLE_STR_COUNT][STRING_TABLE_STR_LEN];
UINT32 CManageServer::ms_dwDBGroup;

extern int sol_common_open(lua_State* L);
extern int sol_error_def_open(lua_State* L);
extern int sol_role_data_open(lua_State* L);
extern int sol_role_data_mgr_open(lua_State* L);
extern int sol_online_role_data_open(lua_State* L);
extern int sol_master_mgr_open(lua_State* L);
extern int sol_string_table_open(lua_State *L);
extern int sol_guild_def_open(lua_State* L);
extern int sol_ms_guild_open(lua_State *L);
extern int sol_ms_guild_mgr_open(lua_State* L);
extern int sol_ms_guild_member_open(lua_State* L);
extern int sol_ranklist_open(lua_State* L);
extern int sol_chat_def_open(lua_State* L);
extern int sol_arena_mgr_open(lua_State* L);
extern int sol_global_config_open(lua_State* L);
extern int sol_user_open(lua_State* L);
extern int sol_ms_chat_open(lua_State* L);

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

        CManageServer::GetInstance().GetPacketAllocator()->Free(pPacketBuffer);
        return;
Exit0:
        SAFE_DELETE_ARRAY(m_pMathTable);

        CManageServer::GetInstance().GetPacketAllocator()->Free(pPacketBuffer);
        return;

    }
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bPassiveShutdown) {}

    char*   m_pMathTable;
};

CManageServer::CManageServer()
{
    m_pNet = NULL;
    m_pGTSocketAcceptor = NULL;
    m_pGSSocketAcceptor = NULL;
    m_pLSSocketAcceptor = NULL;
    m_pDBSocketAcceptor = NULL;
	m_pBSSocketAcceptor = NULL;
    m_pDLSocketAcceptor = NULL;
	m_pMMSocketAcceptor = NULL;
    m_pMonitorForRoleNumberAcceptor = NULL;
    m_bForceClose = FALSE;
    m_dwThreadCount = 0;
    m_dwCurrentTime = 0;
    m_qwCurrentTick = 0;
	m_dwNextDailyTime = 0;
    m_qwStartTick = 0;
    m_qwGameFrame = 0;
    memset(&m_dbsetting, 0, sizeof(m_dbsetting));
	m_pTrieRoot = NULL;
	m_bUpdateLastRank = FALSE;
	m_nUpdateLastRankServerGroup = 0;
	m_nUpdateLastRankType = rlInvalid;
}

CManageServer::~CManageServer()
{
    UnInit();
}

CManageServer& CManageServer::GetInstance(void)
{
    return ms_Instance;
}

BOOL CManageServer::_InitMathTable(void)
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

BOOL CManageServer::_MainLoop(void)
{
    INT32 nRetCode = 0;
    static char s_szCmd[1024];
    static INT32 s_nCmdLen = 0;

    nRetCode = m_pNet->MainLoop();
    LOG_CHECK_ERROR(nRetCode);

	if (m_qwGameFrame % GAME_FPS == 0)
	{
		_On1SecMainLoop();
	}

    if (m_qwGameFrame % (GAME_FPS * 60) == 0)
    {
        _On1MinMainLoop();
    }

	if (m_dwCurrentTime > m_dwNextDailyTime)
	{
		_OnDailyMainLoop();

		m_dwNextDailyTime = ((m_dwCurrentTime + m_nTimeZone - DAILY_TRIGGER_TIME) / 86400 + 1) * 86400 - m_nTimeZone + DAILY_TRIGGER_TIME;
	}

    // Timer
    nRetCode = CTimerMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // Timer trigger
    nRetCode = CTimeTrigger::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // 
    nRetCode = CApplicationMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // Exchange
    nRetCode = CAuctionMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // Item drops
    nRetCode = CMsDropMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // 
    nRetCode = CMSAdventure::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // SQL
    nRetCode = CManageServer::GetInstance().GetSqlMgr().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // 
    nRetCode = CPreSaveList::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // Recharge interface
    if (g_ServerConfig.MS.bOABillingEnable ||
        g_ServerConfig.MS.bTimeCardEnabled ||
        g_ServerConfig.MS.bMonthCardEnabled)
    {
        nRetCode = COABillMgr::GetInstance().MainLoop();
        LOG_CHECK_ERROR(nRetCode);
    }

    // Anti-addiction
    if (g_ServerConfig.MS.bFcmEnable)
    {
        nRetCode = CFcmMgr::GetInstance().MainLoop();
        LOG_CHECK_ERROR(nRetCode);
    }

    // Login queue
	if (CManageServer::GetInstance().GetGameFrame() % (PROCESS_QUEUE_INTERVAL * GAME_FPS / MAX_SERVER_GROUP_COUNT) == 0)
	{
		INT32 nServerGroup = CManageServer::GetInstance().GetGameFrame() % (PROCESS_QUEUE_INTERVAL * GAME_FPS);
		nServerGroup /= (PROCESS_QUEUE_INTERVAL * GAME_FPS / MAX_SERVER_GROUP_COUNT);
		if (IsValidGroup(nServerGroup))
			CUserMgr::GetInstance().ProcessQueue(nServerGroup);
	}

    // Global data
    nRetCode = CGlobalDataManager::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    // Black market
    nRetCode = CMSBlackMarketMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

#if defined(_DEBUG)
	if (m_qwGameFrame % (GAME_FPS * 1800) == 0)
	{
		_CrtCheckMemory();
	}
#endif	// _DEBUG

    return TRUE;
Exit0:
    return FALSE;
}

void CManageServer::_On1SecMainLoop(void)
{
    INT32   nRetCode = 0;

	nRetCode = CMSSessionMgr::GetInstance().MainLoop();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CArenaMgr::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CUserMgr::GetInstance().CheckAllUserTimeCard();
    LOG_CHECK_ERROR(nRetCode);

    if (CMSSessionMgr::GetInstance().GetServerState() == sgsAllReady)
    {
        nRetCode = CExchangeMgr::GetInstance().MainLoop();
        LOG_CHECK_ERROR(nRetCode);
    }

	if (m_bUpdateLastRank)
	{
		nRetCode = CRankListMgr::GetInstance().UpdateLastRank(m_nUpdateLastRankServerGroup, m_nUpdateLastRankType);
		LOG_CHECK_ERROR(nRetCode);

		m_nUpdateLastRankType++;
		if (m_nUpdateLastRankType >= rlTotal)
		{
			m_nUpdateLastRankServerGroup++;
            while (!IsValidGroup(m_nUpdateLastRankServerGroup) && (m_nUpdateLastRankServerGroup < MAX_SERVER_GROUP_COUNT))
				m_nUpdateLastRankServerGroup++;

			if (m_nUpdateLastRankServerGroup < MAX_SERVER_GROUP_COUNT)
				m_nUpdateLastRankType = rlBegin;
			else
				m_bUpdateLastRank = FALSE;
		}
	}

	g_SetStatisInfo(statRoleCount, CUserMgr::GetInstance().Count());
	g_SetStatisInfo(statSceneCount, CSceneMgr::GetInstance().GetSceneCount());

Exit0:
	return;
}

void CManageServer::_On1MinMainLoop(void)
{
    static BOOL init_list = FALSE;
    typedef std::list<std::pair<INT32, INT32> > INT_PAIR_LIST;
    static INT_PAIR_LIST item_list, rune_list, Luck2Necklace_list;

    CServerVar& var = CServerVar::GetInstance();
    UINT32 cur_time = CManageServer::GetInstance().GetGameTime();

    // Initialize item drop, equipment, inscription refinement, and Luck +2 necklace limits
    if (!init_list)
    {
        const ITEM_DATA* item_data;
        item_data = CDataManager<ITEM_DATA>::GetInstance().GetFirstData();
        while (item_data)
        {
            if (item_data->m_nTemplateID && item_data->m_nDropLimitDuration[0] > 0 && item_data->m_nDropLimitCount[0] > 0)
            {
                item_list.push_back(std::make_pair(item_data->m_nTemplateID, item_data->m_nDropLimitDuration[0] * 3600));
            }
            item_data = CDataManager<ITEM_DATA>::GetInstance().GetNextData();
        }

        item_data = CDataManager<EQUIP_DATA>::GetInstance().GetFirstData();
        while (item_data)
        {
            if (item_data->m_nTemplateID && item_data->m_nDropLimitDuration[0] > 0 && item_data->m_nDropLimitCount[0] > 0)
            {
                item_list.push_back(std::make_pair(item_data->m_nTemplateID, item_data->m_nDropLimitDuration[0] * 3600));
            }
            item_data = CDataManager<EQUIP_DATA>::GetInstance().GetNextData();
        }

        const RUNE_DATA* rune_data;
        rune_data = CDataManager<RUNE_DATA>::GetInstance().GetFirstData();
        while (rune_data)
        {
            if (rune_data->nRuneID && rune_data->nDropLimitDuration && rune_data->nDropLimitCount)
            {
                rune_list.push_back(std::make_pair(rune_data->nRuneID, rune_data->nDropLimitDuration * 3600));
            }
            rune_data = CDataManager<RUNE_DATA>::GetInstance().GetNextData();
        }

        // Custom code: check Luck +2 necklace production
        if (g_ServerConfig.CC.nMinLuck2AttributelID > 0 && 
            g_ServerConfig.CC.nMaxLuck2AttributelID > 0 &&
            g_ServerConfig.CC.nItemDropLimitLuck2NecklaceDuration > 0 && 
            g_ServerConfig.CC.nItemDropLimitLuck2NecklaceCount > 0)
        {
            for (const auto ItemID : g_ServerConfig.CC.nLuck2NecklaceItemID)
            {
                Luck2Necklace_list.push_back(std::make_pair(ItemID, g_ServerConfig.CC.nItemDropLimitLuck2NecklaceDuration * 3600));
            }
        }

        init_list = TRUE;
    }

    // Refresh item drop, equipment, and inscription refinement limits
    for (INT32 group = 0; group < MAX_SERVER_GROUP_COUNT; group++)
    {
        if (IsValidGroup(group))
        {
            for (INT_PAIR_LIST::iterator it = item_list.begin(); it != item_list.end(); ++it)
            {
                INT32 last_refresh_time = var.GetVariable(group, MakeServerKey(esvtItemDropLimitType0RefreshTime, it->first));
                if (last_refresh_time + it->second <= cur_time)
                {
                    var.SetVariable(group, MakeServerKey(esvtItemDropLimitType0RefreshTime, it->first), cur_time);
                    var.SetVariable(group, MakeServerKey(esvtItemDropLimitType0, it->first), 0);
                }
            }

            for (INT_PAIR_LIST::iterator it = rune_list.begin(); it != rune_list.end(); ++it)
            {
                INT32 last_refresh_time = var.GetVariable(group, MakeServerKey(esvtRuneRefreshTime, it->first));
                if (last_refresh_time + it->second <= cur_time)
                {
                    var.SetVariable(group, MakeServerKey(esvtRuneRefreshTime, it->first), cur_time);
                    var.SetVariable(group, MakeServerKey(esvtRune, it->first), 0);
                }
            }

            // Luck +2 necklace limit
            for (INT_PAIR_LIST::iterator it = Luck2Necklace_list.begin(); it != Luck2Necklace_list.end(); ++it)
            {
                INT32 last_refresh_time = var.GetVariable(group, MakeServerKey(esvtItemDropLimitLuck2NecklaceRefreshTime, it->first));
                if (last_refresh_time + it->second <= cur_time)
                {
                    var.SetVariable(group, MakeServerKey(esvtItemDropLimitLuck2NecklaceRefreshTime, it->first), cur_time);
                    var.SetVariable(group, MakeServerKey(esvtItemDropLimitLuck2Necklace, it->first), 0);
                }
            }
        }
    }

    // Luck +2 necklace limit
    //for (INT_PAIR_LIST::iterator it = Luck2Necklace_list.begin(); it != Luck2Necklace_list.end(); ++it)
    //{
    //    INT32 last_refresh_time = var.GetVariable(g_ServerConfig.CC.nItemDropLimitLuck2NecklaceServerGroup, MakeServerKey(esvtItemDropLimitLuck2NecklaceRefreshTime, it->first));
    //    if (last_refresh_time + it->second <= cur_time)
    //    {
    //        var.SetVariable(g_ServerConfig.CC.nItemDropLimitLuck2NecklaceServerGroup, MakeServerKey(esvtItemDropLimitLuck2NecklaceRefreshTime, it->first), cur_time);
    //        var.SetVariable(g_ServerConfig.CC.nItemDropLimitLuck2NecklaceServerGroup, MakeServerKey(esvtItemDropLimitLuck2Necklace, it->first), 0);
    //    }
    //}
}

void CManageServer::_OnDailyMainLoop(void)
{
	INT32 nRetCode = 0;

	CRankListMgr::GetInstance().UpdateAverageLevel();
	CMessageHandler::GetInstance().DoGSUpdateAverageLevel(0, CRankListMgr::GetInstance().GetAverageLevel());

	m_nUpdateLastRankServerGroup = 0;
    while (!IsValidGroup(m_nUpdateLastRankServerGroup) && (m_nUpdateLastRankServerGroup < MAX_SERVER_GROUP_COUNT))
		m_nUpdateLastRankServerGroup++;
	if (m_nUpdateLastRankServerGroup < MAX_SERVER_GROUP_COUNT)
		m_bUpdateLastRank = TRUE;
	else
		m_bUpdateLastRank = FALSE;
	m_nUpdateLastRankType = rlBegin;

    // Modify GameWorld global variables
    CServerVar& var = CServerVar::GetInstance();
    for (INT32 group = 0; group < MAX_SERVER_GROUP_COUNT; group++)
    {
        if (IsValidGroup(group))
        {
            // Example
            //var.SetVariable(group, MakeServerKey(esvtDestinyWeaponCount, 2000024), 0);  // 2000024 - destiny weapon gift box; reset count to zero
        }
    }

	INF("daily main loop finished");

	return;
}

BOOL CManageServer::Init(INetEventHandler* poNetEventHandler)
{
    INT32 nRetCode = 0;

#if defined(WIN32)
    INTRANET_INIT_PARAM IntranetInitParam;
#else
    SIMPLE_INIT_PARAM SimpleInitParam;
#endif

    m_bForceClose = FALSE;

#ifdef WIN32
//	SetUnhandledExceptionFilter(DumpExceptionFilter);
#endif

    memset(g_StringTable, 0, sizeof(g_StringTable));
    m_dwCurrentTime = (UINT32)time(NULL);
    m_qwCurrentTick = GetTickCountEx();
    m_qwGameFrame = 0;

	m_nTimeZone = get_time_zone();
	m_dwNextDailyTime = ((m_dwCurrentTime + m_nTimeZone - DAILY_TRIGGER_TIME) / 86400 + 1) * 86400 - m_nTimeZone + DAILY_TRIGGER_TIME;

    // DB
    strxcpy(m_dbsetting.szServerAddr, g_ServerConfig.DB.szDBIPAddress, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szLoginName, g_ServerConfig.DB.szDBAccount, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szPassword, g_ServerConfig.DB.szDBPassword, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szDBName, g_ServerConfig.DB.szDBName, DB_SETTING_STRING_LEN);
    strxcpy(m_dbsetting.szCharSet, g_ServerConfig.DB.szDBCharset, DB_SETTING_STRING_LEN);
    m_dbsetting.dwServerPort = g_ServerConfig.DB.dwDBPort;
    m_dwThreadCount = g_ServerConfig.DB.dwThreadCount;
	m_bUpdateLastRank = FALSE;
	m_nUpdateLastRankServerGroup = 0;
	m_nUpdateLastRankType = rlInvalid;

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

        strxcpy(m_dbsetting.szDBName, szHostName, DB_SETTING_STRING_LEN);
    }

	nRetCode = _InitDataBase();
	LOG_PROCESS_ERROR(nRetCode);

    // MySQL
    nRetCode = CManageServer::GetInstance().GetSqlMgr().Init(
        m_dbsetting.szServerAddr,
        m_dbsetting.szLoginName,
        m_dbsetting.szPassword,
        m_dbsetting.szDBName,
        m_dbsetting.szCharSet,
        m_dbsetting.dwServerPort,
        g_MSSqlStmtList, msqlTotal,
        MAX_DB_WORKER_THREAD_COUNT);
    LOG_PROCESS_ERROR(nRetCode);

	if (g_ServerConfig.MS.bDBRegisterGroup)
	{
		nRetCode = _RegisterGroupInfo();
		LOG_PROCESS_ERROR(nRetCode);
	}

    nRetCode = m_PacketAllocator.Init(g_ServerConfig.MS.Net.nSmallPacketCount,
        g_ServerConfig.MS.Net.nLargePacketCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitMathTable();
    LOG_PROCESS_ERROR(nRetCode);

    // Internal network
#if defined(WIN32)
    IntranetInitParam.nMaxAcceptor = g_ServerConfig.MS.Net.nMaxAcceptor;
    IntranetInitParam.nMaxConnector = g_ServerConfig.MS.Net.nMaxConnector;
    IntranetInitParam.nMaxSessionCount = g_ServerConfig.MS.Net.nMaxSessionCount;
    IntranetInitParam.nSendBufSize = g_ServerConfig.MS.Net.nSendBufSize;
    IntranetInitParam.nRecvBufSize = g_ServerConfig.MS.Net.nRecvBufSize;
    IntranetInitParam.nPacketBufferSize = g_ServerConfig.MS.Net.nPacketBufferSize;
    IntranetInitParam.nDisconnectTimeout = g_ServerConfig.Common.nShareMemDisconnectTimeout;
    m_pNet = g_CreateNetInterface(netIntranet, &m_PacketAllocator, poNetEventHandler, &IntranetInitParam);
#else
    SimpleInitParam.nMaxAcceptor = g_ServerConfig.MS.Net.nMaxAcceptor;
    SimpleInitParam.nMaxConnector = g_ServerConfig.MS.Net.nMaxConnector;
    SimpleInitParam.nMaxSessionCount = g_ServerConfig.MS.Net.nMaxSessionCount;
    SimpleInitParam.nSendBufSize = g_ServerConfig.MS.Net.nSendBufSize;
    SimpleInitParam.nRecvBufSize = g_ServerConfig.MS.Net.nRecvBufSize;
    SimpleInitParam.nPacketBufferSize = g_ServerConfig.MS.Net.nPacketBufferSize;
    m_pNet = g_CreateNetInterface(netSimple, &m_PacketAllocator, poNetEventHandler, &SimpleInitParam);
#endif
    LOG_PROCESS_ERROR(m_pNet);

	m_pTrieRoot = trie_create();
	LOG_PROCESS_ERROR(m_pTrieRoot);

	nRetCode = trie_insert(m_pTrieRoot, g_szCensoredWordList);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = trie_insert(m_pTrieRoot, g_GWConfig.szCensoredWordList);
	LOG_PROCESS_ERROR(nRetCode);

	m_qwStartTick = GetTickCountEx();

    INF("MS initialized successfully");

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CManageServer::Open(void)
{
    INT32 nRetCode = 0;
    UINT32 dwAddr;
    struct in_addr addr;

    LOG_PROCESS_ERROR(m_pNet);

    dwAddr = inet_addr(g_ServerConfig.MS.szAddress);
    LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

    // GT
    m_pGTSocketAcceptor = m_pNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pGTSocketAcceptor);

    nRetCode = m_pGTSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwGTPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened GT listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwGTPort);

    // GS
    m_pGSSocketAcceptor = m_pNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pGSSocketAcceptor);

    nRetCode = m_pGSSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwGSPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened GS listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwGSPort);

    // LS
    m_pLSSocketAcceptor = m_pNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pLSSocketAcceptor);

    nRetCode = m_pLSSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwLSPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened LS listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwLSPort);

    // DB
    m_pDBSocketAcceptor = m_pNet->CreateAcceptor();
    LOG_PROCESS_ERROR(m_pDBSocketAcceptor);

    nRetCode = m_pDBSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwDBPort);
    LOG_PROCESS_ERROR(nRetCode);

    INF("opened DB listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwDBPort);

    // BS
	m_pBSSocketAcceptor = m_pNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pBSSocketAcceptor);

	nRetCode = m_pBSSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwBSPort);
	LOG_PROCESS_ERROR(nRetCode);

	INF("opened BS listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwBSPort);

    // DL
	m_pDLSocketAcceptor = m_pNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pDLSocketAcceptor);

	nRetCode = m_pDLSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwDLPort);
	LOG_PROCESS_ERROR(nRetCode);

	INF("opened DL listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwDLPort);

    // MM
	m_pMMSocketAcceptor = m_pNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pMMSocketAcceptor);

	nRetCode = m_pMMSocketAcceptor->Open(dwAddr, g_ServerConfig.MS.dwMMPort);
	LOG_PROCESS_ERROR(nRetCode);

	INF("opened MM listen port at %s:%d", g_ServerConfig.MS.szAddress, g_ServerConfig.MS.dwMMPort);

    // Monitor character count
	m_pMonitorForRoleNumberAcceptor = m_pNet->CreateAcceptor();
	LOG_PROCESS_ERROR(m_pMonitorForRoleNumberAcceptor);

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, FALSE, 0);
	LOG_PROCESS_ERROR(addr.s_addr);

	nRetCode = m_pMonitorForRoleNumberAcceptor->Open(addr.s_addr, g_ServerConfig.MS.dwOnlineRoleNumPort);
	LOG_PROCESS_ERROR(nRetCode);

	INF("opened Monitor for role number listen port at %s:%d", inet_ntoa(addr), g_ServerConfig.MS.dwOnlineRoleNumPort);

    INF("MS Open successfully");

    return TRUE;
Exit0:
    INF("MS Open failed");

    return FALSE;
}

BOOL CManageServer::UnInit(void)
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

    if (m_pGSSocketAcceptor)
    {
        nRetCode = m_pGSSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_pNet->DestroyAcceptor(m_pGSSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pGSSocketAcceptor = NULL;
    }

    if (m_pLSSocketAcceptor)
    {
        nRetCode = m_pLSSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_pNet->DestroyAcceptor(m_pLSSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pLSSocketAcceptor = NULL;
    }

    if (m_pDBSocketAcceptor)
    {
        nRetCode = m_pDBSocketAcceptor->Close();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_pNet->DestroyAcceptor(m_pDBSocketAcceptor);
        LOG_CHECK_ERROR(nRetCode);

        m_pDBSocketAcceptor = NULL;
    }

	if (m_pBSSocketAcceptor)
	{
		nRetCode = m_pBSSocketAcceptor->Close();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_pNet->DestroyAcceptor(m_pBSSocketAcceptor);
		LOG_CHECK_ERROR(nRetCode);

		m_pBSSocketAcceptor = NULL;
	}

	if (m_pDLSocketAcceptor)
	{
		nRetCode = m_pDLSocketAcceptor->Close();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_pNet->DestroyAcceptor(m_pDLSocketAcceptor);
		LOG_CHECK_ERROR(nRetCode);

		m_pDLSocketAcceptor = NULL;
	}

	if (m_pMMSocketAcceptor)
	{
		nRetCode = m_pMMSocketAcceptor->Close();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_pNet->DestroyAcceptor(m_pMMSocketAcceptor);
		LOG_CHECK_ERROR(nRetCode);

		m_pMMSocketAcceptor = NULL;
	}

	if (m_pMonitorForRoleNumberAcceptor)
	{
		nRetCode = m_pMonitorForRoleNumberAcceptor->Close();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = m_pNet->DestroyAcceptor(m_pMonitorForRoleNumberAcceptor);
		LOG_CHECK_ERROR(nRetCode);

		m_pMonitorForRoleNumberAcceptor = NULL;
	}

	if (g_ServerConfig.MS.bDBRegisterGroup)
    {
        nRetCode = _UnRegisterGroupInfo();
        LOG_CHECK_ERROR(nRetCode);
    }

    if (m_pNet)
    {
        nRetCode = m_pNet->UnInit();
        LOG_CHECK_ERROR(nRetCode);

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

    nRetCode = CManageServer::GetInstance().GetSqlMgr().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE; 
}

BOOL CManageServer::Close()
{
    INT32 nRetCode = 0;
    
    if (!m_bForceClose)
    {
        nRetCode = CMessageHandler::GetInstance().DoClose();
        LOG_PROCESS_ERROR(nRetCode);

        while (!CMessageHandler::GetInstance().CheckAllClose())
        {
            nRetCode = MainLoop();
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
   
    CMSSessionMgr::GetInstance().CloseAllSession();
    while (CMSSessionMgr::GetInstance().CheckSession())
    {
        nRetCode = MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
    }
    
    CMSSessionMgr::GetInstance().SetServerState(sgsClosed);
    INF("MS is closed");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CManageServer::MainLoop(void)
{
    UINT64 qwEndTick{};
    INT32 nRetCode = 0;

    m_dwCurrentTime = (UINT32)time(NULL);
    m_qwCurrentTick = GetTickCountEx();
	memcpy(&m_LocalTime, localtime_ex(), sizeof(m_LocalTime));

    if ((m_qwGameFrame * 1000) < (m_qwCurrentTick - m_qwStartTick) * GAME_FPS)
    {
        nRetCode = _MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        ++m_qwGameFrame;
    }

#ifndef _DEBUG
	static UINT64 s_qwLastCheckPacketTick = 0;

	if (m_qwCurrentTick - s_qwLastCheckPacketTick > 8000)
	{
		s_qwLastCheckPacketTick = m_qwCurrentTick;
		m_PacketAllocator.CheckError();
	}
#endif	// _FINAL

    qwEndTick = GetTickCountEx();
    if (qwEndTick - m_qwCurrentTick < 100)
        Sleep(1);
    else /*if (qwEndTick - m_qwCurrentTick > 100)*/
        CRI("too slow main loop %lld", qwEndTick - m_qwCurrentTick);

    return TRUE;
Exit0:
    return FALSE;
}

inline bool _check_module(const char* pcszString, const char* pcszCommand)
{
	return strnicmp(pcszString, pcszCommand, strlen(pcszCommand)) == 0;
}

BOOL CManageServer::Reload(const char* pcszModule)
{
    INT32 nRetCode = 0;
    INT32 nScriptErrorCount = 0;

    LOG_PROCESS_ERROR(pcszModule);

	nRetCode = g_ReloadPakFolder(DEFAULT_PAK_PATH);
	LOG_PROCESS_ERROR(nRetCode);

    if (_check_module(pcszModule, "all"))
    {
		nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
		LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGlobalDataManager::GetInstance().ReloadAllData(FALSE);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CScriptMgr::GetInstance().Reload(nScriptErrorCount);
        LOG_CHECK_ERROR(nRetCode);
    }
    else if (_check_module(pcszModule, "script"))
    {
        nRetCode = CScriptMgr::GetInstance().Reload(pcszModule + sizeof("script"));
        LOG_CHECK_ERROR(nRetCode);
    }
    else if (_check_module(pcszModule, "setting"))
    {
        nRetCode = CGlobalDataManager::GetInstance().ReloadAllData(TRUE);
        LOG_CHECK_ERROR(nRetCode);
    }

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CManageServer::_SelectGroup(std::list<void*>& rResult, void* pUserData)
{
	const char* pcszIp = (const char*)pUserData;

	for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
	{
		UINT8* pbyGroup = (UINT8*)(*it);
		const char* pcszGroupId = (const char*)(*it) + sizeof(UINT8);
		if (IsValidGroup(*pbyGroup))
		{
			if (strncmp(pcszGroupId, pcszIp, COMMON_NAME_LEN) != 0)
			{
				CRI("server group %d is occupied by %s", *pbyGroup, pcszGroupId);
				return FALSE;
			}

			ms_dwDBGroup |= (1 << *pbyGroup);
		}
	}

	return TRUE;
}

BOOL CManageServer::_RegisterGroupInfo(void)
{
    INT32 nRetCode = 0;
	char szIp[COMMON_NAME_LEN];
    struct in_addr addr;

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
    LOG_PROCESS_ERROR(addr.s_addr);

    snprintf(szIp, COMMON_NAME_LEN, "%s:%d", inet_ntoa(addr), g_ServerConfig.MS.dwDBPort);
	ms_dwDBGroup = 0;

	nRetCode = m_SqlMgr.SyncExecSqlStmt(mysqlSelectGroup, _SelectGroup, szIp, "");
	LOG_PROCESS_ERROR(nRetCode);

	for (INT32 nServerGroup = 1; nServerGroup < MAX_SERVER_GROUP_COUNT; ++nServerGroup)
    {
		if (!IsValidGroup(nServerGroup))
			continue;

		INF("try to register group %d to DB...", nServerGroup);

		if (ms_dwDBGroup & (1 << nServerGroup))
		{
			nRetCode = m_SqlMgr.SyncExecSqlStmt(mysqlUpdateGroup, NULL, NULL, "ucs", (UINT8)nServerGroup, szIp);
			LOG_PROCESS_ERROR(nRetCode);

			INF("updated old registration of group %d", nServerGroup);
			continue;
		}

		nRetCode = m_SqlMgr.SyncExecSqlStmt(mysqlInsertGroup, NULL, NULL, "ucs", (UINT8)nServerGroup, szIp);
		LOG_PROCESS_ERROR(nRetCode);

		INF("registered group %d to DB successfully.", nServerGroup);
    }

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CManageServer::_UnRegisterGroupInfo(void)
{
    INT32 nRetCode = 0;
    char szIp[COMMON_NAME_LEN];
    struct in_addr addr;

	addr.s_addr = GetIp(g_ServerConfig.Common.szIntranetAddr, g_ServerConfig.Common.szIntranetMask, TRUE, 0);
    LOG_PROCESS_ERROR(addr.s_addr);

	snprintf(szIp, COMMON_NAME_LEN, "%s:%d", inet_ntoa(addr), g_ServerConfig.MS.dwDBPort);

	nRetCode = m_SqlMgr.SyncExecSqlStmt(mysqlDeleteGroup, NULL, NULL, "s", szIp);
	LOG_PROCESS_ERROR(nRetCode);

	INF("unregistered all group from DB successfully.");

	return TRUE;
Exit0:

    return FALSE;
}

BOOL CManageServer::_InitDataBase()
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

    bRet = UpdateDB(mysql.get(), m_dbsetting.szDBName, "", CURRENT_DB_VERSION);
    LOG_PROCESS_ERROR(bRet);


    return TRUE;

Exit0:

    return FALSE;
}

#ifdef _WIN32
void _signal_handler(int sig)
{
    signal(SIGINT, _signal_handler);
    signal(SIGBREAK, _signal_handler);
}
#endif  // _WIN32

BOOL _InitScript(void)
{
    INT32 nRetCode = 0;
    CLuaScript* poScript;

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("common.pkg", sol_common_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("error_def.pkg", sol_error_def_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("role_data.pkg", sol_role_data_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("role_data_mgr.pkg", sol_role_data_mgr_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("online_role_data.pkg", sol_online_role_data_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("master_mgr.pkg", sol_master_mgr_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("string_table.pkg", sol_string_table_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("guild_def.pkg", sol_guild_def_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("ms_guild.pkg", sol_ms_guild_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("ms_guild_mgr.pkg", sol_ms_guild_mgr_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("ms_guild_member.pkg", sol_ms_guild_member_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("ranklist.pkg", sol_ranklist_open);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("chat_def.pkg", sol_chat_def_open);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("arena_mgr.pkg", sol_arena_mgr_open);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("user.pkg", sol_user_open);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("ms_chat.pkg", sol_ms_chat_open);
    LOG_PROCESS_ERROR(nRetCode);

	// for final_gm/ms.lua
	nRetCode = CScriptMgr::GetInstance().RegisterPackage("global_config.pkg", sol_global_config_open);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterFuncList(g_LuaScriptFuncTable);
    LOG_PROCESS_ERROR(nRetCode);

    poScript = CScriptMgr::GetInstance().NewScript(STRING_TABLE_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    poScript = CScriptMgr::GetInstance().NewScript(MASTER_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(ARENA_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    poScript = CScriptMgr::GetInstance().NewScript(MS_GM_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    poScript = CScriptMgr::GetInstance().NewScript(GUILD_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    poScript = CScriptMgr::GetInstance().NewScript(GUILD_ACTIVITY_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    poScript = CScriptMgr::GetInstance().NewScript(MS_CS_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(ACTIVITY_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

    INF("initialized all scripts successfully");

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _InitTimer()
{
	INT32 nRetCode = 0;
	
	STimer* pstTimer = CTimerMgr::GetInstance().CreateTimer();
	LOG_PROCESS_ERROR(pstTimer);

	pstTimer->m_nInterval = g_ServerConfig.MS.nSceneCleanTime / MS_CLEAN_SCENE_FRE;
	pstTimer->m_nRemainTrigCount = MAX_TRIG_COUNT;
	pstTimer->m_pFunc = &SceneMgrTimerTrig;

	nRetCode = CTimerMgr::GetInstance().StartTimer(pstTimer);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _InitServer(void)
{
    INT32 nRetCode = 0;
    CHAR szFilePath[PATH_LEN];
    CMessageHandler& rMessageHandler = CMessageHandler::GetInstance();
    CManageServer& rManageServer = CManageServer::GetInstance();

    nRetCode = CLog::GetInstance().Init("manager_server", 
        g_ServerConfig.MS.Log.nLogLevelMask, 
        g_ServerConfig.MS.Log.nLogOutputMask, 
        g_ServerConfig.MS.Log.bEnableLogThread, 
        g_ServerConfig.MS.Log.bPrintThreadID,
        g_ServerConfig.MS.Log.bFlushFile);
	LOG_PROCESS_ERROR(nRetCode);

	g_InitReportSetting(GetServerShortName(stMS), 1, g_ServerConfig.Common.szReportAbsolutePath);

#ifdef _FINAL
	INF("initializing manage server %d.%d.%d final", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#else
	INF("initializing manage server %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL

	//for (INT32 i = 0; i < 1024; ++i)
	//{
	//	nRetCode = REPORT(1, "%s %d", "test init ms", i);
	//	//if (g_Random(1,2) == 1)
	//	//	Sleep(g_Random(7));
	//	INF("%s %d return %d", "test init ms", i, nRetCode);
	//}

	nRetCode = CheckDiskRemainCapacity();
	LOG_PROCESS_ERROR(nRetCode);

    // Create internal network
    nRetCode = rManageServer.Init(&rMessageHandler);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CPreSaveList::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CTimerMgr::GetInstance().Init((UINT32)time(NULL));
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CTimeTrigger::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CUserMgr::GetInstance().Init(g_ServerConfig.MS.nInitUserCount);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CRoleDataMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CSceneMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMSGuildAchiMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().Init(g_GWConfig.szConfigPath);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _InitScript();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_InitReadDataScript(g_GWConfig.szConfigPath, READ_DATA_SCRIPT_NAME);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGlobalDataManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<SCENE_DATA>::GetInstance().Init(SCENE_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().Init(MAIL_TEMPLATE_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<MS_MASTER_GIFT_TEMPLATE>::GetInstance().Init(MASTER_LVL_UP_GIFT_NAME, szFilePath, g_ProcessMasterGift);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<GUILD_BUILDING_DATA>::GetInstance().Init(GUILD_BUILDING_NAME, szFilePath, NULL);    // check on gameserver
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<GUILD_DUNGEON_DATA>::GetInstance().Init(GUILD_DUNGEON_NAME, szFilePath, NULL);      // check on gameserver
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<GUILD_WELFARE_DATA>::GetInstance().Init(GUILD_WELFARE_NAME, szFilePath, NULL);      // check on gameserver
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().Init(GUILD_ACTIVITY_NAME, szFilePath, NULL);      // check on gameserver
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<REAL_MONEY_SHOP_DATA>::GetInstance().Init(REAL_MONEY_SHOP_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<ARENA_SCORE>::GetInstance().Init(ARENA_SCORE_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
	nRetCode = CDataManager<DUNGEON_DATA>::GetInstance().Init(DUNGEON_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
	nRetCode = CDataManager<ACTIVITY_DATA>::GetInstance().Init(ACTIVITY_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
	nRetCode = CDataManager<ACHIEVEMENT_DATA>::GetInstance().Init(ACHIEVEMENT_FILE_NAME, szFilePath, g_ProcessAchievement);
	LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<GUILD_ACHI_CDTN>::GetInstance().Init(GUILD_ACHI_CDTN_NAME, szFilePath, g_ProcessGuildAchiCdtn);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<GUILD_ACHIEVEMENT>::GetInstance().Init(GUILD_ACHIEVEMENT_NAME, szFilePath, g_ProcessGuildAchievement);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<BLACK_MARKET_DATA>::GetInstance().Init(BLACK_MARKET_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CRealMoneyShop::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    if (g_ServerConfig.MS.bOABillingEnable ||
        g_ServerConfig.MS.bTimeCardEnabled ||
        g_ServerConfig.MS.bMonthCardEnabled)
    {
        nRetCode = COABillMgr::GetInstance().Init();
        LOG_PROCESS_ERROR(nRetCode);
    }

    if (g_ServerConfig.MS.bFcmEnable)
    {
        nRetCode = CFcmMgr::GetInstance().Init();
        LOG_PROCESS_ERROR(nRetCode);
    }

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<ITEM_DATA>::GetInstance().Init(ITEM_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<EQUIP_DATA>::GetInstance().Init(ITEM_ARMOR_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<MONEY_DATA>::GetInstance().Init(MONEY_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", g_GWConfig.szConfigPath, SETTING_FOLDER_NAME);
    nRetCode = CDataManager<RUNE_DATA>::GetInstance().Init(RUNE_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMsDropMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGSInfoMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = COnlineRoleDataMgr::GetInstance().Init(g_ServerConfig.MS.nInitOnlineRoleCount);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CRSAKeyMgr::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMasterMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CTeamMgr::GetInstance().Init(g_ServerConfig.MS.nInitTeamCount);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMailMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CBattleQueueMgr::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CAuctionMgr::GetInstance().Init(4, 1024);    // Default code: 4 threads, at most 1024 auction objects
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CApplicationMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CRankListMgr::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CServerVar::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CExchangeMgr::GetInstance().Init(1024);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CExchangeHistory::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGlobalDataManager::GetInstance().PostProcessAllData();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _InitTimer();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CArenaMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	if (g_ServerConfig.MS.bOAAwardEnable)
	{
		nRetCode = CAwardManager::GetInstance().Init();
		LOG_PROCESS_ERROR(nRetCode);
	}

    nRetCode = CGMManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

#ifdef _WIN32
    signal(SIGINT, _signal_handler);
    signal(SIGBREAK, _signal_handler);
#endif  // _WIN32

    // Create and connect the internal network - open the network port after preparation
    nRetCode = rManageServer.Open();
    LOG_PROCESS_ERROR(nRetCode);

    // Initialize session fields
    nRetCode = CMSSessionMgr::GetInstance().Init(rManageServer.GetNetInterface());
    LOG_PROCESS_ERROR(nRetCode);

    // Initialize functions
    nRetCode = rMessageHandler.Init(rManageServer.GetNetInterface(), rManageServer.GetPacketAllocator(), 
        rManageServer.GetGTSocketAcceptor(), rManageServer.GetGSSocketAcceptor(), rManageServer.GetLSSocketAcceptor(), 
        rManageServer.GetDBSocketAcceptor(),rManageServer.GetBSSocketAcceptor(), rManageServer.GetDLSocketAcceptor(),
		rManageServer.GetMMSocketAcceptor(),
        rManageServer.GetMonitorForRoleNumberAcceptor());
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL _UnInitServer(void)
{
    INT32 nRetCode = 0;

    nRetCode = CServerVar::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CRankListMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGMManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CApplicationMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMailMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CAuctionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CExchangeMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CExchangeHistory::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CTimerMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CTimeTrigger::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CBattleQueueMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CTeamMgr::GetInstance().UnInit();	// why not locate in _UnInitServer?
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().Uninit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMasterMgr::GetInstance().Uninit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CArenaMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = COnlineRoleDataMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGSInfoMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<SCENE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<MS_MASTER_GIFT_TEMPLATE>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CSceneMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CRoleDataMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CUserMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CPreSaveList::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMSSessionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<REAL_MONEY_SHOP_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CRealMoneyShop::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    if (g_ServerConfig.MS.bOABillingEnable ||
        g_ServerConfig.MS.bTimeCardEnabled ||
        g_ServerConfig.MS.bMonthCardEnabled)
    {
        nRetCode = COABillMgr::GetInstance().UnInit();
        LOG_CHECK_ERROR(nRetCode);
    }

    if (g_ServerConfig.MS.bFcmEnable)
    {
        nRetCode = CFcmMgr::GetInstance().UnInit();
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CMSGuildAchiMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<ITEM_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<EQUIP_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<MONEY_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_ACHIEVEMENT>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_ACHI_CDTN>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_BUILDING_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_DUNGEON_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_WELFARE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<RUNE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMsDropMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<ARENA_SCORE>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);
	if (g_ServerConfig.MS.bOAAwardEnable)
	{
		nRetCode = CAwardManager::GetInstance().UnInit();
		LOG_CHECK_ERROR(nRetCode);
	}

    nRetCode = CGlobalDataManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = g_UnInitReadDataScript();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CManageServer::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = g_ClosePakFolder();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CLog::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CheckGameFrame(void)
{
	static UINT32 s_qwLastDumpTick = 0;

	if (CMSSessionMgr::GetInstance().GetServerState() != sgsAllReady ||
		CManageServer::GetInstance().GetGameFrame() < GAME_FPS * 180 ||
		IsDebuggerPresent())
		return FALSE;

	UINT64 qwCurrentTick = GetTickCountEx();
	UINT64 qwStartTick = CManageServer::GetInstance().GetStartTick();
	INT64 llCurrentFrame = CManageServer::GetInstance().GetGameFrame();

	if ((qwCurrentTick - qwStartTick > (llCurrentFrame + GAME_FPS / 2) * 1000LL / GAME_FPS) &&
		(qwCurrentTick - s_qwLastDumpTick > 1000 * 3000))
	{
		CRI("check game frame %I64d tick diff %I64d ", llCurrentFrame, qwCurrentTick - qwStartTick - llCurrentFrame * 1000 / GAME_FPS);
		s_qwLastDumpTick = qwCurrentTick;
		return TRUE;
	}
	return FALSE;
}

INT32 main(INT32 argc, TCHAR** argv) 
{ 
    INT32 nRetCode = 0;

#if defined(WIN32)
	nRetCode = StartDumpThread(CheckGameFrame, FALSE, "log");
    PROCESS_ERROR(nRetCode);

    SetWindowPos(GetConsoleWindow(), HWND_TOP, 500, 0, 0, 0, SWP_NOSIZE);
#endif

    // Temporarily added debug code
    //Sleep(1000 * 10);

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

	nRetCode = g_InitStatisInfo(GetCurrentProcessId());
	LOG_PROCESS_ERROR(nRetCode);

    /************************************************************************/
    // initialize
    nRetCode = _InitServer();
    LOG_PROCESS_ERROR(nRetCode);

#if (defined(WIN32) || defined(WIN64))
    {
        char szTitle[MAX_PATH];
#ifdef _FINAL
		snprintf(szTitle, MAX_PATH, "Manage Server, ver %d.%d.%d final, area %d", 
			MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER, g_ServerConfig.Common.nAreaID);
#else
		snprintf(szTitle, MAX_PATH, "Manage Server, ver %d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER);
#endif	// _FINAL
        SetConsoleTitle(szTitle);
        INF(szTitle);
    }
#endif  // WIN32

    /************************************************************************/
    // main loop
    while (CMSSessionMgr::GetInstance().GetServerState() != sgsClosing)
    {
#if defined(WIN32)
        __try
        {
            nRetCode = CManageServer::GetInstance().MainLoop();
            LOG_PROCESS_ERROR(nRetCode);
        }
        __except(DumpExceptionFilter(GetExceptionInformation()))
        {
        }
#elif defined (__linux__)
        nRetCode = CManageServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);
#endif 
    }

    /************************************************************************/
    // exit
	INF("MS is exiting");

	nRetCode = CManageServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
	INF("MS has exited");

	nRetCode = _UnInitServer();
    LOG_CHECK_ERROR(nRetCode);

#if defined(WIN32)
    StopDumpThread();
#endif 

    return 0; 
}

const char* CManageServer::GetString( UINT32 dwID )
{
    UINT32 dwIndex = dwID - SERVER_STRING_START_ID;

    LOG_PROCESS_ERROR( dwIndex < STRING_TABLE_STR_COUNT );
    LOG_PROCESS_ERROR( g_StringTable[dwIndex][0] != '\0' );

    return g_StringTable[dwIndex];
Exit0:
    return NULL;
}

BOOL CManageServer::CheckCensoredWord(const char* pcszText)
{
	return trie_search(m_pTrieRoot, pcszText);
}

BOOL CManageServer::ReplaceCensoredWord(char* pszText)
{
	return trie_replace(m_pTrieRoot, pszText);
}

void MSSysLogSuccess(MS_USER* poUser, const char* szFucntionName)
{
	PROCESS_ERROR(poUser);

	if(poUser->eState == USER_STATE_QUEUEFINISHED || poUser->eState == USER_STATE_AUTHED)
		SYST("session<%08X>, user[%s], state[%d] after %s success. ", 
			poUser->dwCliSessionID, poUser->szUserName, poUser->eState, szFucntionName);
	else
		SYST("session<%08X>, user[%s], state[%d], roleid[%d], after %s success.", 
		poUser->dwCliSessionID, poUser->szUserName, poUser->eState, poUser->dwCurRoleID, szFucntionName);
Exit0:
	return;
}

void MSSysLogError(MS_USER* poUser, const char* szFucntionName)
{
	PROCESS_ERROR(poUser);

	if(poUser->eState == USER_STATE_QUEUEFINISHED || poUser->eState == USER_STATE_AUTHED)
		SYSF("session<%08X>, user[%s], state[%d] after %s failed. ", 
			poUser->dwCliSessionID, poUser->szUserName, poUser->eState, szFucntionName);
	else
		SYSF("session<%08X>, user[%s], state[%d], roleid[%d], after %s failed.", 
		poUser->dwCliSessionID, poUser->szUserName, poUser->eState, poUser->dwCurRoleID, szFucntionName);
Exit0:
	return;
}
