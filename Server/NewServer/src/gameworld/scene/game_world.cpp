#include"stdafx.h"
#include "game_world.h"

#include <time.h>

#include "btnod.h"
#include "trie.h"
#include "statis_info.h"
#include "censored_word_list.h"
#include "modifier_file_def.h"

#include "string_def.h"
#include "scene_data.h"
#include "npc_data.h"
#include "npc_pattern.h"
#include "doodad_data.h"
#include "spell_data.h"
#include "aura_data.h"
#include "missile_data.h"
#include "spell_config.h"
#include "ai_data.h"
#include "ai_manager.h"
#include "item_data.h"
#include "set_data.h"
#include "soul_data.h"
#include "refine_data.h"
#include "dungeon_data.h"
#include "attribute_data.h"
#include "attribute_pool_data.h"
#include "shop_data.h"
#include "drop_group_data.h"
#include "quest_data.h"
#include "init_reputation_data.h"
#include "npc_relation_data.h"
#include "role_relation_data.h"
#include "level_damage_mod_data.h"
#include "achievement_data.h"
#include "spirit_data.h"
#include "rune_data.h"
#include "scene_file_mgr.h"
#include "message_handler.h"
#include "robot_message_handler.h"
#include "spell_config.h"
#include "script_mgr.h"
#include "script/script_func.h"
#include "level_data.h"
#include "attr_conversion_data.h"
#include "mount_data.h"
#include "circle_quest_data.h"
#include "activity_reward_data.h"
#include "modifier_tree.h"
#include "npc_force_mgr.h"
#include "role_faction_mgr.h"
#include "role_kick_mgr.h"
#include "server_def.h"
#include "checkdata.h"
#include "statistic.h"
#include "decompose_data.h"
#include "compose_data.h"
#include "pk_drop_count_data.h"
#include "pk_drop_rate_data.h"
#include "guild_mgr.h"
#include "spirit_def.h"
#include "adventure_data.h"
#include "adventure.h"
#include "drop_generator.h"
#include "spirit.h"
#include "money_data.h"
#include "mail_template_data.h"
#include "data/post_process_data.h"
#include "npc_shop_mgr.h"
#include "server_config.h"
#include "guild_welfare_data.h"
#include "guild_building_data.h"
#include "guild_dungeon_data.h"
#include "item_mgr.h"
#include "pet_data.h"
#include "kill_npc_quest_data.h"
#include "drop_manager.h"
#include "checkin_data.h"
#include "guild_achievement.h"
#include "black_market_data.h"
#include "gamble_data.h"
#include "anti_cheat.h"
#include "guild_activity.h"

CGameWorld CGameWorld::ms_Instance;

char g_StringTable[STRING_TABLE_STR_COUNT][STRING_TABLE_STR_LEN];

extern int sol_common_open (lua_State* L);
extern int sol_global_config_open(lua_State* L);
extern int sol_guild_config_open(lua_State* L);
extern int sol_scene_object_open (lua_State* L);
extern int sol_scene_open (lua_State* L);
extern int sol_character_open (lua_State* L);
extern int sol_role_open (lua_State* L);
extern int sol_npc_open (lua_State* L);
extern int sol_hero_open (lua_State* L);
extern int sol_teleport_open (lua_State* L);
extern int sol_gameworld_open (lua_State* L);
extern int sol_doodad_open (lua_State* L);
extern int sol_item_open(lua_State* L);
extern int sol_item_bag_open(lua_State* L);
extern int sol_ai_open(lua_State* L);
extern int sol_quest_open(lua_State* L);
extern int sol_robot_message_handler_open(lua_State* L);
extern int sol_char_def_open(lua_State* L);
extern int sol_attr_list_open(lua_State* L);
extern int sol_attribute_def_open(lua_State* L);
extern int sol_math_ex_open(lua_State* L);
extern int sol_chat_def_open(lua_State* L);
extern int sol_id_def_open(lua_State* L);
extern int sol_spell_book_open(lua_State* L);
extern int sol_spell_open(lua_State* L);
extern int sol_data_manager_open(lua_State* L);
extern int sol_variable_open(lua_State* L);
extern int sol_quest_list_open(lua_State* L);
extern int sol_quest_def_open(lua_State* L);
extern int sol_score_def_open(lua_State* L);
extern int sol_title_open(lua_State* L);
extern int sol_achievement_def_open(lua_State* L);
extern int sol_achievement_list_open(lua_State* L);
extern int sol_check_data_open(lua_State* L);
extern int sol_guild_def_open(lua_State* L);
extern int sol_guild_mgr_open(lua_State* L);
#ifndef _ROBOT
extern int sol_gs_guild_open(lua_State* L);
#else
extern int sol_asimov_guild_open(lua_State* L);
extern int sol_asimov_role_open(lua_State* L);
extern int sol_general_mgr_open(lua_State* L);
#endif //_ROBOT
extern int sol_friend_open(lua_State* L);
extern int sol_error_def_open(lua_State* L);
extern int sol_gs_team_open(lua_State* L);
extern int sol_string_table_open(lua_State* L);
extern int sol_money_def_open(lua_State* L);
extern int sol_hero_def_open(lua_State* L);
extern int sol_npc_def_open(lua_State* L);
extern int sol_scene_def_open(lua_State* L);
extern int sol_spirit_def_open(lua_State* L);
extern int sol_adventure_open(lua_State* L);
extern int sol_spirit_open(lua_State* L);
extern int sol_arena_open(lua_State* L);
extern int sol_master_open(lua_State* L);
extern int sol_rank_list_def_open(lua_State* L);
extern int sol_stall_open(lua_State* L);
extern int sol_drop_generator_open(lua_State* L);
extern int sol_trade_open(lua_State* L);

extern char g_szErrorMsg[];
extern INT32 g_nErrorMsgSize;

CGameWorld::CGameWorld()
{
    m_dwGSSessionID = 0;
	m_llGameFrame = 0;
	m_dwStartTime = 0;
	m_qwCurrentTick = 0;
	m_bCooldownAvailable = TRUE;
	m_bCanCast = TRUE;
	m_bDrawDebugLine = FALSE;
	m_pTrieRoot = NULL;
	m_lstSaveRole.Clear();
	m_bFreezeTime = FALSE;
    m_qwStartTick = 0;
    m_dwCurrentTime = 0;
    memset(&m_LocalTime, 0, sizeof(m_LocalTime));
    m_bProgrammer = FALSE;
	m_bIsInLixjList = FALSE;
    m_bIsCheck = FALSE;
	m_bCheckConsume = TRUE;
    m_dwBattleTeamIDGenerate = 0;
	memset(m_nAverageLevel, 0, sizeof(m_nAverageLevel));

#ifndef _ROBOT
	m_poGPK = NULL;
#endif

}

CGameWorld::~CGameWorld()
{
    UnInit();
}

CGameWorld& CGameWorld::GetInstance(void)
{
	return ms_Instance;
}

BOOL CGameWorld::Init(void)
{
	INT32 nRetCode = 0;
	int LogLevelMask = 0;
	char szHostName[256];

	strxcpy(m_szConfigPath, g_GWConfig.szConfigPath, PATH_LEN);

	nRetCode = _InitPool();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CRole::CheckDBDataLength();
	LOG_PROCESS_ERROR(nRetCode);

	m_llGameFrame = 0;
	m_lstSaveRole.Clear();
	m_dwBattleTeamIDGenerate = 0;

	m_pTrieRoot = trie_create();
	LOG_PROCESS_ERROR(m_pTrieRoot);

	nRetCode = trie_insert(m_pTrieRoot, g_szCensoredWordList);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = trie_insert(m_pTrieRoot, g_GWConfig.szCensoredWordList);
	LOG_PROCESS_ERROR(nRetCode);

	memset(m_nAverageLevel, 0, sizeof(m_nAverageLevel));

    m_nTimeZone = get_time_zone();

#ifndef _ROBOT
	m_poGPK = SGPK::GPKCreateSvrDynCode();
	LOG_PROCESS_ERROR(m_poGPK);

#ifdef WIN64
    nRetCode = m_poGPK->LoadBinary(GPK_SERVER_BIN_FOLDER_64, GPK_CLIENT_BIN_FOLDER_64);
    LOG_PROCESS_ERROR(nRetCode > 0);
#else
    nRetCode = m_poGPK->LoadBinary(GPK_SERVER_BIN_FOLDER_32, GPK_CLIENT_BIN_FOLDER_32);
    LOG_PROCESS_ERROR(nRetCode >= 0);
#endif

#endif // _ROBOT

#ifdef _ROBOT
	m_bProgrammer = TRUE;
#else

	gethostname(szHostName, 256);

	static constexpr const char* szDeveloperList[] = {
		"sh-zhujianqiu-d", 
		"sh-lixj01-d", 
		"sh-taoqun-d",
		"sh-wubin02-d", 
		"sh-chenw01-d3", 
		"SH-LIXIAGUANG-D",
		"lixiaguangcingo",
		"SH-CHENWANG01-D", 
		"SH-ZHAOJUN-D", 
		"SH-ZHANGLINJIAN-D", 
		"SH-ZHUMING-D", 
		"SH-FANGRUYI-D",  
		"sh-zhangr05-d", 
		"SH-HUJUNJIE-D",
		"sh-zhaojun02-d",
		"SH-zhanglg01-d",
        "sh-meixz-d",
		"sh-xuzhongh-d2",
		"sh-zhuming01-d",
		"GS1",
		"GS2",
		"GS3",
		"GS4",
		"MS1",
		"BUILDS",
        "sh-luchenlei-d",
		"sh-lihuanyi-d",
		"sh-caihy-d",
	};

	static constexpr const char* szShowSpellLogList[] =
	{
		"sh-yangchen05-d",
		"sh-lixj01-d", 
        "sh-luchenlei-d",
        "sh-fangruyi-d",
	};

	static constexpr auto DevelopersCount = sizeof( szDeveloperList ) / sizeof( szDeveloperList[0] );
	m_bProgrammer = FALSE;
	for( int i = 0 ; i < DevelopersCount ; ++i )
	{
		if( 0 ==  stricmp( szHostName, szDeveloperList[i] ) )
		{
			m_bProgrammer = TRUE;
			break;
		}
	}

	static constexpr auto LixjCount = sizeof( szShowSpellLogList ) / sizeof( szShowSpellLogList[0] );
	LogLevelMask = CLog::GetInstance().GetLevelMask();

	LogLevelMask &= ~lltSpell;
	m_bIsInLixjList = FALSE;

	for( int i = 0 ; i < LixjCount ; ++i )
	{
		if( 0 ==  stricmp( szHostName, szShowSpellLogList[i] ) )
		{
			if((LogLevelMask & lltSpell) == 0)
			{
				LogLevelMask |= lltSpell;
				m_bIsInLixjList = TRUE;
				break;
			}
		}
	}
	CLog::GetInstance().SetLevelMask(LogLevelMask);


#endif

    m_qwMsBlackeMarketLimitMapUpdateTime = 0;

	nRetCode = CAntiCheatMgr::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

	srand((UINT32)time(NULL));

	INF("initialized game world successfully");

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CGameWorld::UnInit(void)
{
	INT32 nRetCode = 0;

#ifndef _ROBOT
	if(m_poGPK != NULL)
	{
		m_poGPK->Release();
		m_poGPK = NULL;
	}
#endif

	nRetCode = trie_destroy(m_pTrieRoot);
	LOG_CHECK_ERROR(nRetCode);

	m_pTrieRoot = NULL;

	nRetCode = _UnInitPool();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL CGameWorld::CleanUpScenes(void)
{
    INT32 nRetCode = 0;
    CDeleteAllScene DeleteAllScene;

    nRetCode = m_ScenePool.Traverse(DeleteAllScene);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CGameWorld::CTraverseRole::operator()(UINT32 dwRoleID, CRole* poRole)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);

#ifndef _ROBOT
	switch (poRole->GetState())
	{
	case rsPreEntered:
	case rsWaitRoleData:
	case rsWaitMSApprove:
		break;
	case rsWaitClientLoad:
	case rsJumpInGSWaitClientLoad:
		LOG_PROCESS_ERROR(poRole->GetLoadingSceneEndFrame() > CGameWorld::GetInstance().GetGameFrame());
		break;
	case rsPlaying:
	case rsInJump:
	case rsWaitUnlockRoleData:
		break;
	case rsLogout:
		if (poRole->GetLogoutFrame() > 0 && poRole->GetLogoutFrame() < CGameWorld::GetInstance().GetGameFrame())
		{
            poRole->ClearLogoutFrame();

			switch(poRole->GetLogoutType())
			{
			case rltReturnSelectRole:
                nRetCode = CCliMessageHandler::GetInstance().DoReturnSelectRole(poRole);
                LOG_PROCESS_ERROR(nRetCode);
                break;
			case rltReturnLogin:
            case rltKickout:
                nRetCode = CCliMessageHandler::GetInstance().DoLogout(poRole);
                LOG_PROCESS_ERROR(nRetCode);
                break;
			case rltDisconnect:
				nRetCode = CCliMessageHandler::GetInstance().DoDLRoleLogout(poRole, FALSE);
				LOG_CHECK_ERROR(nRetCode);

				nRetCode = poRole->Exit();
                LOG_PROCESS_ERROR(nRetCode);
				break;
			default:
				LOG_PROCESS_ERROR(FALSE);
			}
		}
		break;
	default:
		break;
	}
#endif // _ROBOT

	return TRUE;
Exit0:
	KICK_ROLE(poRole, ktDelayed);

	return TRUE;
}

#ifdef  _ROBOT
struct CSceneHasRobotChecker
{
	BOOL bHasRobot;
	BOOL operator()(CRole* poRole)
	{
		LOG_PROCESS_ERROR(poRole);
		PROCESS_ERROR(!poRole->IsDeleteSelf());

		if (poRole->IsRobot())
		{
			bHasRobot = TRUE;
			return FALSE;
		}

	Exit0:
		return TRUE;
	}
};	
#endif 
BOOL CGameWorld::CTraverseScene::operator()(UINT64 qwSceneID, CScene* poScene)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poScene);

	if (poScene)
	{
		nRetCode = poScene->MainLoop();
		LOG_PROCESS_ERROR(nRetCode);
	}

#ifdef  _ROBOT
	//CSceneHasRobotChecker oChecker;
	//oChecker.bHasRobot = FALSE;

	//nRetCode = poScene->TraverseRole(oChecker);
	//PROCESS_ERROR(nRetCode);

	//if (poScene && FALSE == oChecker.bHasRobot )
	if (poScene && poScene->GetRoleCount() <= 0 )
    {
        INF("uninit and del scene template=%d, id=%d, role count=%d ",
            poScene->GetSceneTempID(), poScene->GetSceneID(), poScene->GetRoleCount() );

		nRetCode = poScene->UnInit();
		LOG_CHECK_ERROR(nRetCode);

		nRetCode = CGameWorld::GetInstance().DelScene(poScene);
		LOG_CHECK_ERROR(nRetCode);
	}
#endif 

Exit0:
	return TRUE;
}

BOOL CGameWorld::CTraverseTeam::operator()(UINT32 dwTeamID, CTeamInGS* poTeam)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poTeam);

	if (poTeam)
	{					
		nRetCode = poTeam->Mainloop();
		LOG_PROCESS_ERROR(nRetCode);
	}

Exit0:
	return TRUE;
}

BOOL CGameWorld::CDeleteAllScene::operator()(UINT64 qwSceneID, CScene* poScene)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poScene);

	nRetCode = poScene->UnInit();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGameWorld::GetInstance().DelScene(poScene);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return TRUE;
}

BOOL CGameWorld::MainLoop(void)
{
	INT32 nRetCode = 0;
	CTraverseRole TraverseRole;
	CTraverseScene TraverseScene;

#ifndef _ROBOT
	nRetCode = m_RolePool.Traverse(TraverseRole);
	LOG_CHECK_ERROR(nRetCode);
#endif	// _ROBOT

	nRetCode = m_ScenePool.Traverse(TraverseScene);
	LOG_CHECK_ERROR(nRetCode);

    if (m_llGameFrame % GAME_FPS == 0)
        _On1SecMainLoop();

#ifndef _ROBOT
	CRoleKickMgr::GetInstance().KickPendingRoles();
#endif	// _ROBOT

#if defined(_DEBUG)
	if (m_llGameFrame % (GAME_FPS * 1800) == 0)
		_CrtCheckMemory();
#endif	// _DEBUG

	m_llGameFrame++;

	return TRUE;
}

inline bool _check_module(const char* pcszString, const char* pcszCommand)
{
	return strnicmp(pcszString, pcszCommand, strlen(pcszCommand)) == 0;
}

void CGameWorld::Reload(const char* pcszModule)
{
	INT32 nRetCode = 0;
    INT32 nScriptErrorCount = 0;
	UINT64 qwStartTick = GetTickCountEx();

    LOG_PROCESS_ERROR(pcszModule);

    g_szErrorMsg[0] = 0;
    g_nErrorMsgSize = 0;

    if (_check_module(pcszModule, "all"))
    {
		nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
        LOG_CHECK_ERROR(nRetCode);
		
		// Temporarily disable the value correction function
        //CheckConfig();

        nRetCode = CGlobalDataManager::GetInstance().ReloadAllData(FALSE);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CScriptMgr::GetInstance().Reload(nScriptErrorCount);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CAIManager::GetInstance().Reload();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CDropManager::GetInstance().Reload();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CNpcShopMgr::GetInstance().Init();
        LOG_CHECK_ERROR(nRetCode);

        if (nScriptErrorCount > 0)
            g_AppendErrorMsg("script error count %d\n", nScriptErrorCount);
    }
    else if (_check_module(pcszModule, "script"))
    {
        nRetCode = CScriptMgr::GetInstance().Reload(pcszModule + sizeof("script"));
        LOG_CHECK_ERROR(nRetCode);
    }
    else if (_check_module(pcszModule, "setting") )
    {
        nRetCode = CGlobalDataManager::GetInstance().ReloadAllData(TRUE);
        LOG_CHECK_ERROR(nRetCode);
    }

	INF("completed reload in %lld ms", GetTickCountEx() - qwStartTick);

Exit0:
	return;
}

void CGameWorld::CheckConfig()
{
    if (g_GWConfig.nGlobalExpRate < 1000)
    {
        WRN("g_GWConfig.nGlobalExpRate is too small: %d, set to 1000", g_GWConfig.nGlobalExpRate);
        g_GWConfig.nGlobalExpRate = 1000;
    }

    if (g_GWConfig.nGlobalExpRate > 100000)
    {
        WRN("g_GWConfig.nGlobalExpRate is too big: %d, set to 100000", g_GWConfig.nGlobalExpRate);
        g_GWConfig.nGlobalExpRate = 100000;
    }

    if (g_GWConfig.nGlobalDropRate < 1000)
    {
        WRN("g_GWConfig.nGlobalDropRate is too small: %d, set to 1000", g_GWConfig.nGlobalDropRate);
        g_GWConfig.nGlobalDropRate = 1000;
    }

    if (g_GWConfig.nGlobalDropRate > 100000)
    {
        WRN("g_GWConfig.nGlobalDropRate is too big: %d, set to 100000", g_GWConfig.nGlobalDropRate);
        g_GWConfig.nGlobalDropRate = 100000;
    }
}

BOOL CGameWorld::IsProgrammer(void)
{
	return m_bProgrammer;
}

BOOL CGameWorld::IsInLixjList(void)
{
	return m_bIsInLixjList;
}

BOOL CGameWorld::JumpTo(CRole* poRole, INT32 nSceneTempID, INT32 nSceneID, INT32 nX, INT32 nY, INT32 nZ, INT32 nFaceDirection)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	CScene* poSrcScene = NULL;
	const SCENE_DATA* pSceneData = NULL;
	CSceneStaticData* poTgtStaticData = NULL;
	const ZONE_DATA* pcZoneData = NULL;
	const GRID_DATA* pcGridData = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetState() == rsPlaying);
	LOG_PROCESS_ERROR(poRole->GetCoolDown().CheckReady(MakeCoolDownID(COOLDOWN_MISC, COOLDOWN_MISC_ARREST)));

	poSrcScene = poRole->GetScene();
	LOG_PROCESS_ERROR(poSrcScene);

	poTgtStaticData = CSceneFileMgr::GetInstance().GetStaticData(nSceneTempID);
	LOG_PROCESS_ERROR(poTgtStaticData);

	nRetCode = poTgtStaticData->GetLowerGrid(nX, nY, nZ, pcZoneData, pcGridData);
	LOG_PROCESS_ERROR(nRetCode);

	pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
	LOG_PROCESS_ERROR(pSceneData);

	if (nSceneID == 0)
	{
		if (pSceneData->nType == estStatic)
			nSceneID = 1;
		else if (pSceneData->nType == estUnique)
			nSceneID = 1;
		else if (poSrcScene->GetSceneTempID() == nSceneTempID && pSceneData->nType == estLine)
			nSceneID = poSrcScene->GetSceneID();
		else if (poRole->GetLastLineSceneTempID() == nSceneTempID && pSceneData->nType == estLine)
			nSceneID = poRole->GetLastLineSceneID();
	}

	if (nSceneID > 0)
	{
		UINT32 dwSessionID = poRole->GetSessionID();
		INT32 nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
		if(pSceneData->nType == estStatic)
			nSceneID = (nServerGroupID << 24) | 1;
		else if(pSceneData->nType == estLine)
			nSceneID = (nServerGroupID << 24) | nSceneID;
	}

	if (poSrcScene->GetSceneTempID() == nSceneTempID && poSrcScene->GetSceneID() == nSceneID &&
		poRole->GetDistance3D(nX, nY, nZ) <= g_GWConfig.nInstantJumpRange * POINT_PER_CM * g_GWConfig.nInstantJumpRange * POINT_PER_CM)
	{
		nRetCode = poRole->JumpTo(nX, nY, nZ, nFaceDirection);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		poRole->SaveReturnPos();
		poRole->SetState(rsInJump);

#ifndef _ROBOT
		nRetCode = CCliMessageHandler::GetInstance().DoQueryScene(poRole, nSceneTempID, nSceneID, nX, nY, nZ, nFaceDirection);
		LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT
	}

	return TRUE;
Exit0:
	CRI("failed to jump to scene %d-%d pos (%d, %d, %d)", nSceneTempID, nSceneID,  
		nX / POINT_PER_CM, nY / POINT_PER_CM, nZ / POINT_PER_CM);

#ifndef _ROBOT
	if(poRole)
		poRole->ErrorCodeNotify(nResult, 0, 0);
#endif

	return FALSE;
}

BOOL CGameWorld::JumpTo(CNpc* poNpc, INT32 nSceneTempID, INT32 nSceneID, INT32 nX, INT32 nY, INT32 nZ, INT32 nFaceDirection)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    CScene* poSrcScene = NULL;
    CScene* poTargetScene = NULL;
    const SCENE_DATA* pSceneData = NULL;
    CSceneStaticData* poTgtStaticData = NULL;
    const ZONE_DATA* pcZoneData = NULL;
    const GRID_DATA* pcGridData = NULL;

    LOG_PROCESS_ERROR(poNpc);
    LOG_PROCESS_ERROR(nSceneID != 0);

    poSrcScene = poNpc->GetScene();
    LOG_PROCESS_ERROR(poSrcScene);

    poTargetScene = CGameWorld::GetInstance().FindScene(MakeSceneObjID(nSceneTempID, nSceneID));
    LOG_PROCESS_ERROR(poTargetScene);

    poTgtStaticData = CSceneFileMgr::GetInstance().GetStaticData(nSceneTempID);
    LOG_PROCESS_ERROR(poTgtStaticData);

    nRetCode = poTgtStaticData->GetLowerGrid(nX, nY, nZ, pcZoneData, pcGridData);
    LOG_PROCESS_ERROR(nRetCode);

    pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
    LOG_PROCESS_ERROR(pSceneData);

    if (poSrcScene->GetSceneTempID() == nSceneTempID && poSrcScene->GetSceneID() == nSceneID)
    {
        nRetCode = poNpc->JumpTo(nX, nY, nZ, nFaceDirection);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = poSrcScene->RemoveNpc(poNpc);
        LOG_PROCESS_ERROR(nRetCode);

        poNpc->SetX(nX);
        poNpc->SetY(nY);
        poNpc->SetZ(nZ);
        poNpc->SetFaceDirection(nFaceDirection);

        nRetCode = poTargetScene->AddNpc(poNpc, soatInRange);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    CRI("NPC failed to jump to scene %d-%d pos (%d, %d, %d)", nSceneTempID, nSceneID,
        nX / POINT_PER_CM, nY / POINT_PER_CM, nZ / POINT_PER_CM);

    return FALSE;
}

BOOL CGameWorld::JumpToRole(CRole* poRole, const char* pcszRoleName)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSQueryRoleLocation(poRole, pcszRoleName, FALSE);
    LOG_PROCESS_ERROR(nRetCode);
#endif // _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGameWorld::TakeRole(CRole* poRole, const char* pcszRoleName)
{
    INT32 nRetCode = 0;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSTakeRole(poRole, pcszRoleName);
    LOG_PROCESS_ERROR(nRetCode);
#endif // _ROBOT

    return TRUE;
Exit0:
    return FALSE;
}

#define PRINT_POOL_SIZE(__name__)														\
    do																					\
	{																					\
        char s[1024];																	\
		INT64	llSize;																	\
        snprintf(s, 1024, "%s pool allocated",  #__name__);								\
        for (int i = (int)strlen(s); i < 32; i++)										\
            strcat(s, " ");																\
		llSize = (INT64)m_##__name__##Pool.GetSize() * m_##__name__##Pool.GetObjSize();	\
        llTotalSize += llSize;															\
        INF("%s %6d objects (%dB each) %.2fMB", s, m_##__name__##Pool.GetSize(),		\
			m_##__name__##Pool.GetObjSize(), llSize / (1024 * 1024.0f));				\
    } while (0)

struct CTraverseSceneGridInstance
{
	INT64	m_llSize;
	BOOL operator()(UINT64 qwSceneID, CScene* poScene)
	{
		LOG_PROCESS_ERROR(poScene);
		LOG_PROCESS_ERROR(poScene->GetStaticData());

		m_llSize += poScene->GetStaticData()->GetInstanceZoneTotalSize();

	Exit0:
		return TRUE;
	}
};

INT64 CGameWorld::PrintMemoryStat(void)
{
    INT64 llTotalSize = 0;
	CTraverseSceneGridInstance TraverseSceneGridInstance;

    PRINT_POOL_SIZE(Scene);
    PRINT_POOL_SIZE(Role);
    PRINT_POOL_SIZE(Npc);
    PRINT_POOL_SIZE(Doodad);
    PRINT_POOL_SIZE(Missile);
    PRINT_POOL_SIZE(Hero);
    PRINT_POOL_SIZE(Drop);
    PRINT_POOL_SIZE(Team);
    PRINT_POOL_SIZE(Region);
    PRINT_POOL_SIZE(ThreatNode);
    PRINT_POOL_SIZE(TargetNode);
    PRINT_POOL_SIZE(SpellCopy);
    PRINT_POOL_SIZE(SpellSet);
    PRINT_POOL_SIZE(SpellInstance);
    PRINT_POOL_SIZE(AuraInstance);
	PRINT_POOL_SIZE(Bullet);
    PRINT_POOL_SIZE(BTNod);
    PRINT_POOL_SIZE(BTCtx);
    PRINT_POOL_SIZE(Signal);
    PRINT_POOL_SIZE(AIProcessor);
	PRINT_POOL_SIZE(SceneCombatStatNode);
	PRINT_POOL_SIZE(Item);

	INF("total object pool size %.2fMB", llTotalSize / (1024 * 1024.0f));

	TraverseSceneGridInstance.m_llSize = 0;
	m_ScenePool.Traverse(TraverseSceneGridInstance);
	INF("grid instance allocated %.2fMB", TraverseSceneGridInstance.m_llSize / (1024 * 1024.0f));

	return llTotalSize + TraverseSceneGridInstance.m_llSize;
}

struct CTraverseRoleInScene
{
	BOOL operator()(CRole* poRole)
	{
		LOG_PROCESS_ERROR(poRole);
		PROCESS_ERROR(!poRole->IsDeleteSelf());

		INF("\t CTraverseRoleInScene role #%s:%d# <%08X> state[%d]", poRole->GetName(), poRole->GetObjID(), poRole->GetSessionID(), poRole->GetState());
	Exit0:
		return TRUE;
	}
};

struct CTraverseStatScene
{
	std::map<INT32, INT32> m_mapID2Count[MAX_SERVER_GROUP_COUNT];
	BOOL m_bDetail;

	BOOL operator()(INT32 nID, CScene* poScene)
	{
		INT32 nRetCode = FALSE;
		INT32 nTempID = 0;
		INT32 nGroupID = 0;
		std::map<INT32, INT32>::iterator it;
		LOG_PROCESS_ERROR(poScene);

		nGroupID = poScene->GetServerGroup();
		LOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_SERVER_GROUP_COUNT);

		nTempID = poScene->GetSceneTempID();
		LOG_PROCESS_ERROR(nTempID > 0);

		it = m_mapID2Count[nGroupID].find(nTempID);
		if (it != m_mapID2Count[nGroupID].end())
		{
			++m_mapID2Count[nGroupID][nTempID];
		}
		else
		{
			m_mapID2Count[nGroupID][nTempID] = 1;
		}

		INF("CTraverseStatScene scene #%d:%d#, state %d, start frame %d,role count %d, npc count %d",
			poScene->GetSceneID(), poScene->GetSceneTempID(), poScene->GetState(),
			poScene->GetStartFrame(), poScene->GetRoleCount(), poScene->GetNpcCount());

		if (m_bDetail)
		{
			CTraverseRoleInScene stChecker;
			nRetCode = poScene->TraverseRole(stChecker);
			PROCESS_ERROR(nRetCode);
		}

	Exit0:
		return TRUE;
	}
};

void CGameWorld::PrintScenePool(BOOL bDetail)
{
	INT32 nRetCode = FALSE;

	CTraverseStatScene stStater;
	stStater.m_bDetail = bDetail;
	for (INT32 nGroup = 0; nGroup < MAX_SERVER_GROUP_COUNT; ++nGroup)
	{
		stStater.m_mapID2Count[nGroup].clear();
	}

	nRetCode = m_ScenePool.Traverse(stStater);
	LOG_PROCESS_ERROR(nRetCode);

	for (INT32 nGroup = 0; nGroup < MAX_SERVER_GROUP_COUNT; ++nGroup)
	{
		for (std::map<INT32, INT32>::iterator it = stStater.m_mapID2Count[nGroup].begin(); it != stStater.m_mapID2Count[nGroup].end(); ++it)
		{
			INF("group %d scene template %d count %d", nGroup, it->first, it->second);
		}
	}

Exit0:
	return;
}

struct CTraverseStatNpc
{
	std::map<INT32, INT32> m_mapID2Count;

	BOOL operator()(INT32 nID, CNpc* poNpc)
	{
		INT32 nTempID = 0;
		std::map<INT32, INT32>::iterator it;
		LOG_PROCESS_ERROR(poNpc);
		LOG_PROCESS_ERROR(poNpc->GetNpcData());

		nTempID = poNpc->GetNpcData()->nTemplateID;

		it = m_mapID2Count.find(nTempID);
		if (it != m_mapID2Count.end())
		{
			++m_mapID2Count[nTempID];
		} 
		else
		{
			m_mapID2Count[nTempID] = 1;
		}

		return TRUE;
	Exit0:
		return FALSE;
	}
};

void CGameWorld::PrintNpcPool(void)
{
	INT32 nRetCode = FALSE;

	CTraverseStatNpc stStater;
	stStater.m_mapID2Count.clear();

	nRetCode = m_NpcPool.Traverse(stStater);
	LOG_PROCESS_ERROR( nRetCode );

	for (std::map<INT32, INT32>::iterator it = stStater.m_mapID2Count.begin(); it != stStater.m_mapID2Count.end(); ++it)
	{
		INF( "npc template %d count %d", it->first, it->second);
	}
Exit0:
	return;
}

struct CTraverseRoleChecker
{
	BOOL operator()(INT32 nID, CRole* poRole)
	{
		std::map<INT32, INT32>::iterator it;
		LOG_PROCESS_ERROR(poRole);
		PROCESS_ERROR(!poRole->IsDeleteSelf());

		INF("CTraverseRoleCheckr role #%s:%d# <%08X> state[%d]", poRole->GetName(), poRole->GetObjID(), poRole->GetSessionID(), poRole->GetState());
	Exit0:
		return TRUE;
	}
};

void CGameWorld::PrintRolePool(void)
{
	INT32 nRetCode = FALSE;

	CTraverseRoleChecker stChecker;

	nRetCode = m_RolePool.Traverse(stChecker);
	LOG_PROCESS_ERROR(nRetCode);
Exit0:
	return;
}

BOOL CGameWorld::_InitPool(void)
{
	INT32 nRetCode = 0;
    const OBJ_POOL_CONFIG& rcParam = g_ObjPoolConfig;

	// init object pool
	nRetCode = m_BTNodPool.Init(rcParam.nBTNodeCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_BTCtxPool.Init(rcParam.nBTCtxCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_SignalPool.Init(rcParam.nSignalCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_AIProcessorPool.Init(rcParam.nNpcCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_TargetNodePool.Init(rcParam.nRoleCount * 2);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ItemSpellLevelNodePool.Init(rcParam.nRoleCount * 8);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ItemSpellRuneNodePool.Init(rcParam.nRoleCount * 8);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_PlacedGridPool.Init(rcParam.nRoleCount * 5);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_SpellInstancePool.Init(rcParam.nSpellInstanceCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_NextSpellPool.Init(rcParam.nSpellInstanceCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_SpellSetPool.Init(rcParam.nSpellSetCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_AuraInstancePool.Init(rcParam.nAuraInstanceCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_BulletPool.Init(rcParam.nBulletCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ThreatNodePool.Init(rcParam.nThreadNodeCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_SceneCombatStatNodePool.Init(rcParam.nRoleCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_RolePool.Init(rcParam.nRoleCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_HeroPool.Init(8);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_NpcPool.Init(rcParam.nNpcCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_DoodadPool.Init(rcParam.nDoodadCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_MissilePool.Init(rcParam.nMissileCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_DropPool.Init(rcParam.nDropCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_TeamPool.Init(rcParam.nTeamCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_RegionPool.Init(rcParam.nRegionCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ScenePool.Init(rcParam.nSceneCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ItemPool.Init(rcParam.nItemCount);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_TeamAuctionItemPool.Init(rcParam.nTeamCount * 2);
	LOG_PROCESS_ERROR(nRetCode);

	INF("initialized all object pools successfully.");

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGameWorld::_UnInitPool(void)
{
	INT32 nRetCode = 0;

	nRetCode = m_ScenePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_RegionPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_TeamPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_DropPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_MissilePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_DoodadPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_HeroPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_NpcPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_RolePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_SceneCombatStatNodePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_ThreatNodePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_BulletPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

//	nRetCode = m_RunePool.UnInit();
//	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_AuraInstancePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_SpellSetPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_SpellInstancePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_NextSpellPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_TargetNodePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_ItemSpellLevelNodePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_ItemSpellRuneNodePool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_PlacedGridPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_BTCtxPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_BTNodPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_ItemPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_SignalPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_TeamAuctionItemPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = m_AIProcessorPool.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL CGameWorld::AddToSaveList(CRole* poRole)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);

	nRetCode = m_lstSaveRole.PushRear(&poRole->GetSaveListNode());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGameWorld::PushToSaveListRear(CRole* poRole)
{
	LOG_PROCESS_ERROR(poRole);

	if (poRole->GetSaveListNode().pPrev && poRole->GetSaveListNode().pNext)
	{
		CBaseList::Remove(&poRole->GetSaveListNode());
		m_lstSaveRole.PushRear(&poRole->GetSaveListNode());
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGameWorld::_SaveRoleData(void)
{
	INT32 nRetCode = 0;
	INT32 nSaveCount = g_GWConfig.nSaveRoleCountPerSecond;
	BASE_LIST_NODE* pNode = NULL;
	BASE_LIST_NODE* pRear = &m_lstSaveRole.Rear();

	PROCESS_SUCCESS(m_lstSaveRole.IsEmpty());

	pNode = m_lstSaveRole.Head().pNext;
	while (pNode != pRear && nSaveCount > 0)
	{
		BASE_LIST_NODE* pNextNode = pNode->pNext;

		CRole* poRole = node_cast(CRole*, GetSaveListNode(), pNode);
		LOG_PROCESS_ERROR(poRole);

		if (poRole->GetLastSaveFrame() + (g_GWConfig.nSaveRoleMinTime * GAME_FPS / 1000) < m_llGameFrame)
		{
#ifndef _ROBOT
			nRetCode = CCliMessageHandler::GetInstance().DoSaveRole(poRole, FALSE);
			LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT
			nSaveCount--;
		}

		pNode = pNextNode;
	}

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}


UINT32 CGameWorld::GenerateID(INT32 eType)
{
	return (eType << 28) + ++m_dwIndexGenerator[eType];
}

BOOL CGameWorld::CheckCensoredWord(const char* pcszText)
{
	return trie_search(m_pTrieRoot, pcszText);
}

BOOL CGameWorld::ReplaceCensoredWord(char* pszText)
{
	return trie_replace(m_pTrieRoot, pszText);
}

BOOL CGameWorld::IsDaytime(void)
{
	time_t dwTimeOff = m_dwCurrentTime + 1800;
	struct tm* tm = localtime(&dwTimeOff);

	if (tm->tm_hour % 2 != 0)
		return TRUE;

	return FALSE;
}

void CGameWorld::ToggleGlobalDropMax()
{
    CDropGenerator::GetInstance().ToggleGlobalDropMax();
}

void CGameWorld::SetGlobalDropRate(INT32 nDropRate)
{
    if (nDropRate < 1000)
        nDropRate = 1000;

    if (nDropRate > 100000)
        nDropRate = 100000;

    g_GWConfig.nGlobalDropRate = nDropRate;
}

INT32 CGameWorld::GetGlobalDropRate()
{
    return g_GWConfig.nGlobalDropRate;
}

void CGameWorld::SetGlobalExpRate(INT32 nExpRate)
{
    if (nExpRate < 1000)
        nExpRate = 1000;

    if (nExpRate > 100000)
        nExpRate = 100000;

    g_GWConfig.nGlobalExpRate = nExpRate;
}

INT32 CGameWorld::GetGlobalExpRate()
{
    return g_GWConfig.nGlobalExpRate;
}

INT32 CGameWorld::SendItemToRole(UINT32 dwRoleID, INT32 nMailTemplateID, Item* pItem, EItemLog reason)
{
#ifndef _ROBOT
    UINT32 attachlen = 0;
    const MAIL_TEMPLATE_DATA* mail_data = nullptr;
#endif
	INT32 nRetCode = errUnknownError;
	CRole* pRole = NULL;
    BOOL bRet = FALSE;

	LOG_PROCESS_ERROR(pItem);

	pRole = CGameWorld::GetInstance().FindRole(dwRoleID);
	if (pRole)
	{
		nRetCode = pRole->GetItemBag()->AddItem(pItem, reason, 0);
        if (nRetCode == errSuccess)
            return TRUE;
	}

#ifndef _ROBOT
    char attachbuffer[1024];
    attachlen = pItem->m_iteminfo.Save(attachbuffer);

    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
    LOG_PROCESS_ERROR(mail_data);

    bRet = CCliMessageHandler::GetInstance().DoMSSendSystemMailWithBufferAttach(
        dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, attachbuffer, attachlen);
#endif

Exit0:
    //TODO: External error handling is inadequate, so call DestroyItem here regardless of success to prevent leaks
    if (pItem)
    {
        BOOL bItemRet = CItemMgr::GetInstance().DestroyItem(pItem);
        LOG_CHECK_ERROR(bItemRet);
    }
	return bRet;
}

INT32 CGameWorld::SendMoneyToRole(UINT32 dwRoleID, INT32 nMailTemplateID, INT32 nMoneyType, UINT32 dwMoneyCount, EMoneyLog reason)
{
	INT32 nResult = errUnknownError;

	CRole* pRole = CGameWorld::GetInstance().FindRole(dwRoleID);
	if (pRole)
	{
		nResult = pRole->GetItemBag()->AddMoney(nMoneyType, dwMoneyCount, reason, 0);
	}

	if (!pRole || nResult != errSuccess)
	{
		const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
		LOG_PROCESS_ERROR(mail_data);

#ifndef _ROBOT
		return CCliMessageHandler::GetInstance().DoMSSendSystemMailWithSimpleAttach(dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, nMoneyType, dwMoneyCount);
#endif
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGameWorld::SendItemMail(UINT32 dwRoleID, INT32 nMailTemplateID, Item* pItem, EItemLog reason)
{
	INT32 nRetCode = 0;
	UINT32 attachlen = 0;

	LOG_PROCESS_ERROR(pItem);

	char attachbuffer[1024];
	attachlen = pItem->m_iteminfo.Save(attachbuffer);

	{
		const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
		LOG_PROCESS_ERROR(mail_data);

#ifndef _ROBOT
		nRetCode = CCliMessageHandler::GetInstance().DoMSSendSystemMailWithBufferAttach(dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, attachbuffer, attachlen);
		LOG_PROCESS_ERROR(nRetCode);
#endif
	}

Exit0:
    //TODO: External error handling is inadequate, so call DestroyItem here regardless of success to prevent leaks
    if (pItem)
    {
        BOOL bItemRet = CItemMgr::GetInstance().DestroyItem(pItem);
        LOG_CHECK_ERROR(bItemRet);
    }
	return nRetCode;
}

BOOL CGameWorld::SendItemMail(UINT32 dwRoleID, INT32 nMailTemplateID, Item* pItem, EItemLog reason, INT32 nMoneyCost)
{
	INT32 nRetCode = 0;
	char attachbuffer[1024] = { 0 };
	char content_buffer[MAIL_CONTENT_LEN] = { 0 };
	UINT32 attachlen = 0;

	LOG_PROCESS_ERROR(pItem);

	attachlen = pItem->m_iteminfo.Save(attachbuffer);

	{
		const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
		LOG_PROCESS_ERROR(mail_data);

		snprintf(content_buffer, MAIL_CONTENT_LEN, mail_data->m_szMailContent, nMoneyCost);

#ifndef _ROBOT
		nRetCode = CCliMessageHandler::GetInstance().DoMSSendSystemMailWithBufferAttach(dwRoleID, mail_data->m_szMailSubject, content_buffer, attachbuffer, attachlen);
		LOG_PROCESS_ERROR(nRetCode);
#endif
	}

Exit0:
    //TODO: External error handling is inadequate, so call DestroyItem here regardless of success to prevent leaks
    if (pItem)
    {
        BOOL bItemRet = CItemMgr::GetInstance().DestroyItem(pItem);
        LOG_CHECK_ERROR(bItemRet);
    }
	return nRetCode;

}

BOOL CGameWorld::SendMoneyMail(UINT32 dwRoleID, INT32 nMailTemplateID, INT32 nMoneyType, UINT32 dwMoneyCount, EMoneyLog reason)
{
	INT32 nRetCode = 0;

	const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
	LOG_PROCESS_ERROR(mail_data);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoMSSendSystemMailWithSimpleAttach(dwRoleID, mail_data->m_szMailSubject, mail_data->m_szMailContent, nMoneyType, dwMoneyCount);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CGameWorld::GetServerVariable(INT32 nServerGroup, UINT64 qwKey)
{
	SERVER_VAR_MAP::iterator it;

	LOG_PROCESS_ERROR(nServerGroup >= 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);

	it = m_ServerVarList[nServerGroup].find(qwKey);
	PROCESS_ERROR(it != m_ServerVarList[nServerGroup].end());

	return it->second;
Exit0:
	return 0;
}

void CGameWorld::SetServerVariable(INT32 nServerGroup, UINT64 qwKey, BOOL bAbsoluteValue, INT32 nVarValue)
{
#ifndef _ROBOT
	CCliMessageHandler::GetInstance().DoMSSetServerVariable(nServerGroup, qwKey, bAbsoluteValue, nVarValue);
#endif //_ROBOT
}

void CGameWorld::_SetServerVariable(INT32 nServerGroup, UINT64 qwKey, INT32 nValue)
{
	LOG_PROCESS_ERROR(nServerGroup >= 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);

	m_ServerVarList[nServerGroup][qwKey] = nValue;
Exit0:
	return;
}

/************************************************************************/

// Initialize the server and load configuration
BOOL _InitWorldData()
{
	//INF("---- Test Sleep 1 ----");
	//Sleep(1000 * 10);

	INT32 nRetCode = 0;
	char szFilePath[PATH_LEN];
	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", CGameWorld::GetInstance().GetConfigPath(), SETTING_FOLDER_NAME);

    nRetCode = CGlobalDataManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = g_InitReadDataScript(CGameWorld::GetInstance().GetConfigPath(), READ_DATA_SCRIPT_NAME);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CModifierTree::GetInstance().Init(MODIFIER_TREE_FILE_NAME, g_ObjPoolConfig.nExtraModifierNodeCount);
	LOG_PROCESS_ERROR(nRetCode);

//	CAttrConv::GetInstance().Init();

	nRetCode = CSpellConfig::GetInstance().Init();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ATTRIBUTE_DATA>::GetInstance().Init(ATTRIBUTE_FILE_NAME, szFilePath, g_ProcessAttributeData);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CAttributePoolDataManager::GetInstance().Init(ATTRIBUTE_POOL_FILE_NAME, szFilePath);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ITEM_DATA>::GetInstance().Init(ITEM_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<EQUIP_DATA>::GetInstance().Init(ITEM_ARMOR_FILE_NAME, szFilePath, g_ProcessEquipData);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SET_DATA>::GetInstance().Init(SET_FILE_NAME, szFilePath, g_ProcessSetData);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<SOUL_DATA>::GetInstance().Init(SOUL_FILE_NAME, szFilePath, g_ProcessSoulData);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<REFINE_DATA>::GetInstance().Init(REFINE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<REFINE_ATTR_DATA>::GetInstance().Init(REFINE_ATTR_FILE_NAME, szFilePath, g_ProcessRefineAttrData);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SHOP_DATA>::GetInstance().Init(SHOP_FILE_NAME, szFilePath, g_ProcessShopData);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CNpcShopMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SCENE_DATA>::GetInstance().Init(SCENE_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<ROLE_LEVEL_DATA>::GetInstance().Init(LEVEL_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<HERO_LEVEL_DATA>::GetInstance().Init(HERO_LEVEL_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<HERO_ATTR_DATA>::GetInstance().Init(HERO_ATTR_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	// attributes1
	nRetCode = CDataManager<ATTR_LEVEL>::GetInstance().Init(LEVEL_ATTRIBUTE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	// attributes2
	//nRetCode = CDataManager<ATTR_CONV>::GetInstance().Init(ATTRIBUTE_CONVERSION_FILE_NAME, szFilePath, g_ProcessAttrConvData);
	//LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<NPC_DATA>::GetInstance().Init(NPC_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);
#ifdef _ROBOT
	NPC_DATA* pNpcData = CDataManager<NPC_DATA>::GetInstance().GetWritableData(0);
	LOG_PROCESS_ERROR(pNpcData);
	pNpcData->nLevel = 1;
#endif //_ROBOT

	nRetCode = CDataManager<NPC_PATTERN_DATA>::GetInstance().Init(NPC_PATTERN_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<DOODAD_DATA>::GetInstance().Init(DOODAD_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPELL_ENTRY>::GetInstance().Init(SPELL_FILE_NAME, szFilePath, g_ProcessSpellEntry);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPELL_VAR>::GetInstance().Init(SPELL_VAR_FILE_NAME, szFilePath, g_ProcessSpellVar);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPELL_PROGRESS_DATA>::GetInstance().Init(SPELL_PROGRESS_FILE_NAME, szFilePath, g_ProcessSpellProgressData);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<RUNE_DATA>::GetInstance().Init(RUNE_FILE_NAME, szFilePath, g_ProcessRuneData);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<AURA_ENTRY>::GetInstance().Init(AURA_FILE_NAME, szFilePath, g_ProcessAuraData);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<TRASFORM_DATA>::GetInstance().Init(TRANSFORM_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<MISSILE_DATA>::GetInstance().Init(MISSILE_FILE_NAME, szFilePath, g_ProcessMissileData);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<NEW_AI_DATA>::GetInstance().Init(NEW_AI_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<TITLE_TEMPLATE>::GetInstance().Init(TITLE_FILE_NAME, szFilePath, g_ProcessTitleTemplate);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<DUNGEON_DATA>::GetInstance().Init(DUNGEON_DATA_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<DROP_GROUP_DATA_RAW>::GetInstance().Init(DROP_GROUP_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<NPC_DROP_DATA>::GetInstance().Init(NPC_DROP_ID_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<DOODAD_DROP_DATA>::GetInstance().Init(DOODAD_DROP_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDropManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

	// Quests
	nRetCode = CDataManager<QUEST_DATA>::GetInstance().Init(QUEST_FILE_NAME, szFilePath, g_ProcessQuestData);
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

	nRetCode = CDataManager<LEVEL_DAMAGE_MOD_DATA>::GetInstance().Init(LEVEL_DAMAGE_MOD_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	// Achievements
    nRetCode = CDataManager<ACHIEVEMENT_DATA>::GetInstance().Init(ACHIEVEMENT_FILE_NAME, szFilePath, g_ProcessAchievementData);
	LOG_PROCESS_ERROR(nRetCode);

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

	// Cycle quests
	nRetCode = CDataManager<CIRCLE_REWARD_DATA>::GetInstance().Init(QUEST_REWARD_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<CIRCLE_QUEST_DATA>::GetInstance().Init(CIRCLE_QUEST_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_BUILDING_DATA>::GetInstance().Init(GUILD_BUILDING_NAME, szFilePath, g_ProcessGuildBuilding);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_DUNGEON_DATA>::GetInstance().Init(GUILD_DUNGEON_NAME, szFilePath, g_ProcessGuildDungeon);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_WELFARE_DATA>::GetInstance().Init(GUILD_WELFARE_NAME, szFilePath, g_ProcessGuildWelfare);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().Init(GUILD_ACTIVITY_NAME, szFilePath, NULL);
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

    nRetCode = CDataManager<ADVENTURE_DATA>::GetInstance().Init(ADVENTURE_DATA_FILE_NAME, szFilePath, g_ProcessAdventureData);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<MONEY_DATA>::GetInstance().Init(MONEY_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().Init(MAIL_TEMPLATE_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_CHANGE_DATA>::GetInstance().Init(SPIRIT_CHANGE_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<HANDBOOK_DATA>::GetInstance().Init(HANDBOOK_FILE_NAME, szFilePath, g_ProcessHandbookData);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<PET_DATA>::GetInstance().Init(PET_DATA_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);
	
	// Monster-kill achievements
	nRetCode = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().Init(KILL_NPC_QUEST_FILE_NAME, szFilePath, g_ProcessKillNpcQuestData);
	LOG_PROCESS_ERROR(nRetCode);

	// Guild achievements
    nRetCode = CDataManager<GUILD_ACHIEVEMENT>::GetInstance().Init(GUILD_ACHIEVEMENT_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CDataManager<CHECKIN_DATA>::GetInstance().Init(CHECKIN_FILE_NAME, szFilePath, NULL);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CGlobalDataManager::GetInstance().PostProcessAllData();
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<BLACK_MARKET_DATA>::GetInstance().Init(BLACK_MARKET_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CDataManager<GAMBLE_DATA>::GetInstance().Init(GAMBLE_FILE_NAME, szFilePath, NULL);
    LOG_PROCESS_ERROR(nRetCode);

    INF("initialized all data manager successfully.");

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitWorldData(void)
{
	INT32 nRetCode = 0;

	nRetCode = CSpellConfig::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<ATTRIBUTE_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CAttributePoolDataManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<ITEM_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<EQUIP_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SET_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<RUNE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SOUL_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<REFINE_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<REFINE_ATTR_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SHOP_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CNpcShopMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SCENE_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<NPC_PATTERN_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<NPC_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<DOODAD_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SPELL_VAR>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SPELL_ENTRY>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SPELL_PROGRESS_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<TRASFORM_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<AURA_ENTRY>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<MISSILE_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<NEW_AI_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<TITLE_TEMPLATE>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<DROP_GROUP_DATA_RAW>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<NPC_DROP_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<DOODAD_DROP_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDropManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<ROLE_LEVEL_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<HERO_LEVEL_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<HERO_ATTR_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<ATTR_LEVEL>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

//	nRetCode = CDataManager<ATTR_CONV>::GetInstance().UnInit();
//	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<QUEST_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<QUEST_RECYCLE_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<REWARD_QUEST_MONEY_COST_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);
	
	nRetCode = CDataManager<INIT_REPUTATION_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<NPC_RELATION_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<ROLE_RELATION_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<LEVEL_DAMAGE_MOD_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<ACHIEVEMENT_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);	

	nRetCode = CDataManager<HANDBOOK_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);
	
	nRetCode = CDataManager<MOUNT_ENTRY>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_CHANGE_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<AIR_LEVEL_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<AIR_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<BLOOD_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<SPIRIT_SLOT_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_BUILDING_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_WELFARE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<DECOMPOSE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<COMPOSE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<PK_DROP_COUNT_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<PK_DROP_RATE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<ACTIVITY_REWARD_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<ADVENTURE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<MONEY_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<PET_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = CDataManager<KILL_NPC_QUEST_DATA>::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GUILD_ACHIEVEMENT>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	nRetCode = g_UnInitReadDataScript();
	LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGlobalDataManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<BLACK_MARKET_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CDataManager<GAMBLE_DATA>::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL _InitScript(void)
{
	INT32 nRetCode = 0;
	CLuaScript* poScript;

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("common.pkg", sol_common_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("global_config.pkg", sol_global_config_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("guild_config.pkg", sol_guild_config_open);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("check_data.pkg", sol_check_data_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("scene_object.pkg", sol_scene_object_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("scene.pkg", sol_scene_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("character.pkg", sol_character_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("role.pkg", sol_role_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("npc.pkg", sol_npc_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("hero.pkg", sol_hero_open);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("guild_def.pkg", sol_guild_def_open);
    LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
    nRetCode = CScriptMgr::GetInstance().RegisterPackage("gs_guild.pkg", sol_gs_guild_open);
    LOG_PROCESS_ERROR(nRetCode);
#else
    nRetCode = CScriptMgr::GetInstance().RegisterPackage("asimov_guild.pkg", sol_asimov_guild_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("asimov_role.pkg", sol_asimov_role_open);
    LOG_PROCESS_ERROR(nRetCode);
#endif // !_ROBOT

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("guild_mgr.pkg", sol_guild_mgr_open);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("teleport.pkg", sol_teleport_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("gameworld.pkg", sol_gameworld_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("doodad.pkg", sol_doodad_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("item.pkg", sol_item_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("item_bag.pkg", sol_item_bag_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("spell_book.pkg", sol_spell_book_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("spell.pkg", sol_spell_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("title.pkg", sol_title_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("spirit_def.pkg", sol_spirit_def_open);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("master.pkg", sol_master_open);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("error_def.pkg", sol_error_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("gs_team.pkg", sol_gs_team_open);
	LOG_PROCESS_ERROR(nRetCode);

#ifdef _ROBOT
	nRetCode = CScriptMgr::GetInstance().RegisterPackage("robot_message_handler.pkg", sol_robot_message_handler_open);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("ai.pkg", sol_ai_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("trade.pkg", sol_trade_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("attr_list.pkg", sol_attr_list_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("attribute_def.pkg", sol_attribute_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("char_def.pkg", sol_char_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("math_ex.pkg", sol_math_ex_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("chat_def.pkg", sol_chat_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("id_def.pkg", sol_id_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("data_manager.pkg", sol_data_manager_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("variable.pkg", sol_variable_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("quest_list.pkg", sol_quest_list_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("quest_def.pkg", sol_quest_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("score_def.pkg", sol_score_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("achievement_def.pkg", sol_achievement_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("friend.pkg", sol_friend_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("achievement_list.pkg", sol_achievement_list_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("string_table.pkg", sol_string_table_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("money_def.pkg", sol_money_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("hero_def.pkg", sol_hero_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("npc_def.pkg", sol_npc_def_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("scene_def.pkg", sol_scene_def_open);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("adventure.pkg", sol_adventure_open);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("spirit.pkg", sol_spirit_open);
	LOG_PROCESS_ERROR(nRetCode); 

    nRetCode = CScriptMgr::GetInstance().RegisterPackage("arena.pkg", sol_arena_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("rank_list_def.pkg", sol_rank_list_def_open);
	LOG_PROCESS_ERROR(nRetCode);

#ifdef _ROBOT
	nRetCode = CScriptMgr::GetInstance().RegisterPackage("general_mgr.pkg", sol_general_mgr_open);
	LOG_PROCESS_ERROR(nRetCode);
#endif

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("stall.pkg", sol_stall_open);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CScriptMgr::GetInstance().RegisterPackage("drop_generator.pkg", sol_drop_generator_open);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().RegisterFuncList(g_LuaScriptFuncTable);
	LOG_PROCESS_ERROR(nRetCode);

	poScript = CScriptMgr::GetInstance().NewScript(STRING_TABLE_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(GM_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(CREATE_ROLE_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(LOAD_ROLE_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(ROLE_LEVEL_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(CREATE_HERO_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(HERO_LEVEL_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(ITEM_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

	poScript = CScriptMgr::GetInstance().NewScript(QUEST_SCRIPT_NAME);
	LOG_PROCESS_ERROR(poScript);

    poScript = CScriptMgr::GetInstance().NewScript(GS_CS_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

	INF("initialized all scripts successfully");

	return TRUE;
Exit0:
	return FALSE;
}

BOOL _UnInitScript(void)
{
	INT32 nRetCode = 0;

	nRetCode = CScriptMgr::GetInstance().UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
}

BOOL g_InitGameWorldLib(UINT64 qwStartTime, void* poMessageNotifier, IPacketAllocator* poPacketAllocator)
{
    INT32 nRetCode = 0;
    const GAME_WORLD_CONFIG& rcParam = g_GWConfig;

    // 1
    nRetCode = CGameWorld::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    CGameWorld::GetInstance().SetStartTick(qwStartTime);
    CGameWorld::GetInstance().SetStartTime((UINT32)time(NULL));

#ifdef _ROBOT
    nRetCode = CRobotMessageHandler::GetInstance().Init((IRobotMessageNotifier*)poMessageNotifier, poPacketAllocator);
    LOG_PROCESS_ERROR(nRetCode);
#else
    nRetCode = CCliMessageHandler::GetInstance().Init((IMessageNotifier*)poMessageNotifier, poPacketAllocator);
    LOG_PROCESS_ERROR(nRetCode);
#endif  // _ROBOT

    // 2
    nRetCode = CScriptMgr::GetInstance().Init(g_GWConfig.szConfigPath);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitScript();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _InitWorldData();  // Load text configuration files
    LOG_PROCESS_ERROR(nRetCode);

    // 3
    nRetCode = CNpcForceMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CRoleFactionMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CBTManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CSceneFileMgr::GetInstance().Init();  // Map
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CAIManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CGuildMgr::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CAdventure::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

#if defined(_DEBUG) & !defined(_FINAL)
    if (!CGameWorld::GetInstance().IsProgrammer() && g_GWConfig.DebugOption.bShowErrorDialog)
    {
        MessageBoxA(NULL, g_szErrorMsg, 0, 0);
    }
#endif  // _DEBUG & !_FINAL

    return TRUE;
Exit0:
    return FALSE;
}

BOOL g_UnInitGameWorldLib(void)
{
    INT32 nRetCode = 0;

    // 3
    nRetCode = CAdventure::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CGuildMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CSceneFileMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CAIManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CBTManager::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CRoleFactionMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CNpcForceMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CModifierTree::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    CAntiCheatMgr::GetInstance().UnInit();

    // 2
    nRetCode = _UnInitWorldData();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _UnInitScript();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CScriptMgr::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

    // 1
    nRetCode = CGameWorld::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);

#ifdef _ROBOT
    nRetCode = CRobotMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);
#else
    nRetCode = CCliMessageHandler::GetInstance().UnInit();
    LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

    return TRUE;
}

const char* CGameWorld::GetString( UINT32 dwID )
{
	UINT32 dwIndex = dwID - SERVER_STRING_START_ID;

	LOG_PROCESS_ERROR( dwIndex < STRING_TABLE_STR_COUNT );
	LOG_PROCESS_ERROR( g_StringTable[dwIndex][0] != '\0' );

	return g_StringTable[dwIndex];
Exit0:
	return NULL;
}


void GSSysLogSuccess(CRole* poRole, const char* szFucntionName)
{

#ifndef _ROBOT
	PROCESS_ERROR(poRole);
	SYST("session <%08X>, roleid[%d], rolename[%s], state[%d] after %s success. ", 
		poRole->GetSessionID(), poRole->GetObjID(), poRole->GetName(), poRole->GetState(), szFucntionName);
Exit0:
#endif

	return;
}

void GSSysLogError(CRole* poRole, const char* szFucntionName)
{	

#ifndef _ROBOT
	PROCESS_ERROR(poRole);
	SYSF("session <%08X>, roleid[%d], rolename[%s], state[%d] after %s failed. ", 
		poRole->GetSessionID(), poRole->GetObjID(), poRole->GetName(), poRole->GetState(), szFucntionName);
Exit0:
#endif

	return;
}

void CGameWorld::_On1SecMainLoop(void)
{
    INT32 nRetCode = 0;
    CTraverseTeam TraverseTeam;

    memcpy(&m_LocalTime, localtime_ex(), sizeof(m_LocalTime));

    nRetCode = CGlobalDataManager::GetInstance().MainLoop();
    LOG_CHECK_ERROR(nRetCode);

#ifndef _ROBOT
    nRetCode = m_TeamPool.Traverse(TraverseTeam);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = _SaveRoleData();
    LOG_CHECK_ERROR(nRetCode);

    g_SetStatisInfo(statRoleCount, m_RolePool.GetUsedSize());
    g_SetStatisInfo(statNpcCount, m_NpcPool.GetUsedSize());
    g_SetStatisInfo(statDoodadCount, m_DoodadPool.GetUsedSize());
    g_SetStatisInfo(statMissileCount, m_MissilePool.GetUsedSize());
    g_SetStatisInfo(statSceneCount, m_ScenePool.GetUsedSize());
    g_SetStatisInfo(statDropCount, m_DropPool.GetUsedSize());
    g_SetStatisInfo(statItemCount, m_ItemPool.GetUsedSize());
	//g_UpdateProcessStatisInfo();
#endif

}
