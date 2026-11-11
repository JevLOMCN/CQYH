#include "stdafx.h"
#include "sol_bindings.h"
#include "message_handler.h"

#include "imessage_notifier.h"
#include "g2c_message.h"
#include "g2m_message.h"
#include "g2o_message.h"
#include "external_message_size.h"


#include "server_def.h"

#include "scene.h"
#include "region.h"
#include "role.h"
#include "hero.h"
#include "spell_config.h"
#include "script_mgr.h"
#include "game_world.h"
#include "level_data.h"
#include "rsa_sign.h"
#include "guild_mgr.h"
#include "gs_guild.h"
#include "server_config.h"
#include "anti_cheat.h"

#ifndef _ROBOT

using namespace ExternalMessage;

#define MAX_SESSION_PER_SEND   (1024)
static UINT32 s_dwSessionIDList[MAX_GT_SESSION_COUNT][MAX_SESSION_PER_SEND];
static UINT32 s_dwSessionCount[MAX_GT_SESSION_COUNT];

#define REGISTER_MESSAGE(__msg_id__, __func__, __playing_msg__)     \
	do                                                              \
{                                                               \
	m_MessageInfo[__msg_id__].pFunc = __func__;                 \
	m_MessageInfo[__msg_id__].bPlayingMsg = __playing_msg__;    \
} while (0)

CCliMessageHandler::CCliMessageHandler()
{
	m_poNotifier = NULL;
    m_poPacketAllocator = NULL;
    memset(m_MessageInfo, 0, sizeof(m_MessageInfo));
}

CCliMessageHandler::~CCliMessageHandler()
{
    UnInit();
}

CCliMessageHandler& CCliMessageHandler::GetInstance(void)
{
	static CCliMessageHandler s_MessageHandler;

	return s_MessageHandler;
}

BOOL CCliMessageHandler::Init(IMessageNotifier* poNotifier, IPacketAllocator* poPacketAllocator)
{
	LOG_PROCESS_ERROR(poNotifier);

	m_poNotifier = poNotifier;
	m_poPacketAllocator = poPacketAllocator;

	m_TraverseRegion.poHandler = this;

	memset(m_MessageInfo, 0, sizeof(m_MessageInfo));

	REGISTER_MESSAGE(c2g_loadfinish,                	&CCliMessageHandler::OnLoadFinish,          FALSE);
	REGISTER_MESSAGE(c2g_gpk_auth_data,                	&CCliMessageHandler::OnGpkAuthData,         FALSE);

	REGISTER_MESSAGE(c2g_role_walk,                 	&CCliMessageHandler::OnRoleWalk,            TRUE);
	REGISTER_MESSAGE(c2g_role_run,                 		&CCliMessageHandler::OnRoleRun,				TRUE);
    REGISTER_MESSAGE(c2g_role_stop,                 	&CCliMessageHandler::OnRoleStop,            TRUE);
	REGISTER_MESSAGE(c2g_role_rotate,               	&CCliMessageHandler::OnRoleRotate,          TRUE);
	REGISTER_MESSAGE(c2g_update_pos,                	&CCliMessageHandler::OnUpdatePos,           TRUE);
    REGISTER_MESSAGE(c2g_ping,                      	&CCliMessageHandler::OnPing,                TRUE);
	REGISTER_MESSAGE(c2g_query_char_data,           	&CCliMessageHandler::OnQueryCharData,       TRUE);
	REGISTER_MESSAGE(c2g_teleport,                  	&CCliMessageHandler::OnTeleport,            TRUE);
	REGISTER_MESSAGE(c2g_change_pk_mode,            	&CCliMessageHandler::OnChangePkMode,        TRUE);
    REGISTER_MESSAGE(c2g_leave_scene,	            	&CCliMessageHandler::OnLeaveScene,          TRUE);
    REGISTER_MESSAGE(c2g_revive,		            	&CCliMessageHandler::OnRevive,		        TRUE);
    REGISTER_MESSAGE(c2g_combat_pose_switch,		    &CCliMessageHandler::OnCombatPoseSwitch,    TRUE);
	REGISTER_MESSAGE(c2g_cancel_cutscene,				&CCliMessageHandler::OnCancelCutScene,      TRUE);
	REGISTER_MESSAGE(c2g_role_teleport,					&CCliMessageHandler::OnRoleTeleport,		TRUE);
	REGISTER_MESSAGE(c2g_query_role_location,			&CCliMessageHandler::OnQueryRoleLocation,	TRUE);
	REGISTER_MESSAGE(c2g_recall_team_member,			&CCliMessageHandler::OnRecallTeamMember,	TRUE);
	REGISTER_MESSAGE(c2g_role_mine,						&CCliMessageHandler::OnRoleMine,			TRUE);

//	REGISTER_MESSAGE(c2g_role_cast_spell,           	&CCliMessageHandler::OnCastSpell,           TRUE);
	REGISTER_MESSAGE(c2g_cast_spell_ex,		        	&CCliMessageHandler::OnCastSpellEx,         TRUE);
	REGISTER_MESSAGE(c2g_cast_switch_spell,	        	&CCliMessageHandler::OnCastSwitchSpell,     TRUE);

	REGISTER_MESSAGE(c2g_directly_cast_spell,	    	&CCliMessageHandler::OnDirectlyCastSpell,   TRUE);
	REGISTER_MESSAGE(c2g_role_cancel_cast,          	&CCliMessageHandler::OnCancelCast,          TRUE);
	REGISTER_MESSAGE(c2g_select_focus,              	&CCliMessageHandler::OnSelectFocus,         TRUE);
	REGISTER_MESSAGE(c2g_role_adjust_target_point,  	&CCliMessageHandler::OnAdjustTargetPt,      TRUE);
	REGISTER_MESSAGE(c2g_equip_spell,					&CCliMessageHandler::OnEquipSpell,		    TRUE);
	REGISTER_MESSAGE(c2g_cancel_aura,					&CCliMessageHandler::OnCancelAura,		    TRUE);	
	REGISTER_MESSAGE(c2g_spell_log,						&CCliMessageHandler::OnSpellLog,		    TRUE);	
	REGISTER_MESSAGE(c2g_clear_spell_cache,				&CCliMessageHandler::OnClearSpellCache,	    TRUE);	

	REGISTER_MESSAGE(c2g_query_role_detail_data,      	&CCliMessageHandler::OnQueryRoleDetailedData,  TRUE);
	REGISTER_MESSAGE(c2g_query_role_pow,		      	&CCliMessageHandler::OnQueryRolePow,  TRUE);

	REGISTER_MESSAGE(c2g_use_item,                  	&CCliMessageHandler::OnUseItem,             TRUE);
	REGISTER_MESSAGE(c2g_move_item,                 	&CCliMessageHandler::OnMoveItem,            TRUE);
	REGISTER_MESSAGE(c2g_split_item,                	&CCliMessageHandler::OnSplitItem,           TRUE);
	REGISTER_MESSAGE(c2g_delete_item,               	&CCliMessageHandler::OnDeleteItem,          TRUE);
	REGISTER_MESSAGE(c2g_pickup_item,               	&CCliMessageHandler::OnPickUpItem,          TRUE);
	REGISTER_MESSAGE(c2g_buy_item,                  	&CCliMessageHandler::OnBuyItem,             TRUE);
	REGISTER_MESSAGE(c2g_sell_item,                 	&CCliMessageHandler::OnSellItem,            TRUE);
	REGISTER_MESSAGE(c2g_view_sold_items,           	&CCliMessageHandler::OnViewSoldItems,       TRUE);
	REGISTER_MESSAGE(c2g_buy_back_item,             	&CCliMessageHandler::OnBuyBackItem,         TRUE);
	REGISTER_MESSAGE(c2g_repair_item,               	&CCliMessageHandler::OnRepairItem,          TRUE);
    REGISTER_MESSAGE(c2g_repair_all_equipped_item,      &CCliMessageHandler::OnRepairAllEquippedItem,TRUE);
    REGISTER_MESSAGE(c2g_special_repair_item,           &CCliMessageHandler::OnSpecialRepairItem,   TRUE);
//	REGISTER_MESSAGE(c2g_refine,                    	&CCliMessageHandler::OnRefine,              TRUE);
	REGISTER_MESSAGE(c2g_buy_container_slot,        	&CCliMessageHandler::OnBuyContainerSlot,    TRUE);
    REGISTER_MESSAGE(c2g_organize_bag,              	&CCliMessageHandler::OnOrganizeBag,         TRUE);
//    REGISTER_MESSAGE(c2g_identify_item,             	&CCliMessageHandler::OnIdentifyItem,        TRUE);
//    REGISTER_MESSAGE(c2g_randomize_item,            	&CCliMessageHandler::OnRandomizeItem,       TRUE);
//    REGISTER_MESSAGE(c2g_confirm_randomize_item,    	&CCliMessageHandler::OnConfirmRandomizeItem,TRUE);
//    REGISTER_MESSAGE(c2g_sign_item,                     &CCliMessageHandler::OnSignItem,		    TRUE);
//    REGISTER_MESSAGE(c2g_use_moneytype01_only,		  &CCliMessageHandler::OnUseMoneytype01Only,  TRUE);
	REGISTER_MESSAGE(c2g_check_npc_shop_data, &CCliMessageHandler::OnCheckNpcShopData, TRUE);

	REGISTER_MESSAGE(c2g_open_stall,                	&CCliMessageHandler::OnOpenStall,           TRUE);
	REGISTER_MESSAGE(c2g_close_stall,               	&CCliMessageHandler::OnCloseStall,          TRUE);
	REGISTER_MESSAGE(c2g_real_open_stall,           	&CCliMessageHandler::OnRealOpenStall,       TRUE);
	REGISTER_MESSAGE(c2g_real_close_stall,          	&CCliMessageHandler::OnRealCloseStall,      TRUE);
	REGISTER_MESSAGE(c2g_add_stall_item,            	&CCliMessageHandler::OnAddStallItem,        TRUE);
	REGISTER_MESSAGE(c2g_remove_stall_item,         	&CCliMessageHandler::OnRemoveStallItem,     TRUE);
	REGISTER_MESSAGE(c2g_change_stall_name,         	&CCliMessageHandler::OnChangeStallName,     TRUE);
    REGISTER_MESSAGE(c2g_upgrade_stall,                 &CCliMessageHandler::OnUpgradeStall,        TRUE);
	REGISTER_MESSAGE(c2g_view_stall_item,           	&CCliMessageHandler::OnViewStallItem,       TRUE);
	REGISTER_MESSAGE(c2g_buy_stall_item,            	&CCliMessageHandler::OnBuyStallItem,        TRUE);

	REGISTER_MESSAGE(c2g_operate_doodad,            	&CCliMessageHandler::OnOperateDoodad,       TRUE);
    REGISTER_MESSAGE(c2g_black_market_limit_info,       &CCliMessageHandler::OnBlackMarketLimitInfo,TRUE);
    REGISTER_MESSAGE(c2g_black_market_cooldown_info,    &CCliMessageHandler::OnBlackMarketCooldownInfo, TRUE);
    REGISTER_MESSAGE(c2g_black_market_buy,              &CCliMessageHandler::OnBlackMarketBuy,      TRUE);
    REGISTER_MESSAGE(c2g_gamble,                        &CCliMessageHandler::OnGamble,              TRUE);
    REGISTER_MESSAGE(c2g_exercise_info,                 &CCliMessageHandler::OnExerciseInfo,        TRUE);
    REGISTER_MESSAGE(c2g_add_exercise_time,             &CCliMessageHandler::OnAddExerciseTime,     TRUE);
    REGISTER_MESSAGE(c2g_get_exercise_exp,              &CCliMessageHandler::OnGetExerciseExp,      TRUE);
	REGISTER_MESSAGE(c2g_trade_ask,						&CCliMessageHandler::OnTradeAsk,		    TRUE);
	REGISTER_MESSAGE(c2g_trade_accept,					&CCliMessageHandler::OnTradeAccept,		    TRUE);
	REGISTER_MESSAGE(c2g_trade_set_item,				&CCliMessageHandler::OnTradeSetItem,	    TRUE);
	REGISTER_MESSAGE(c2g_trade_set_money,				&CCliMessageHandler::OnTradeSetMoney,	    TRUE);
	REGISTER_MESSAGE(c2g_trade_lock,					&CCliMessageHandler::OnTradeLock,		    TRUE);
	REGISTER_MESSAGE(c2g_trade_confirm,					&CCliMessageHandler::OnTradeConfirm,	    TRUE);
	REGISTER_MESSAGE(c2g_trade_unlock,					&CCliMessageHandler::OnTradeUnlock,		    TRUE);
	REGISTER_MESSAGE(c2g_trade_cancel,					&CCliMessageHandler::OnTradeCancel,		    TRUE);

	REGISTER_MESSAGE(c2g_accept_quest,              	&CCliMessageHandler::OnAcceptQuest,         TRUE);
	REGISTER_MESSAGE(c2g_abandon_quest,             	&CCliMessageHandler::OnAbandonQuest,        TRUE);
	REGISTER_MESSAGE(c2g_complete_quest,            	&CCliMessageHandler::OnCompleteQuest,       TRUE);
	REGISTER_MESSAGE(c2g_interact_with_npc,         	&CCliMessageHandler::OnInteractWithNpc,     TRUE);
    REGISTER_MESSAGE(c2g_stop_interact_with_npc,        &CCliMessageHandler::OnStopInteractWithNpc, TRUE);
	REGISTER_MESSAGE(c2g_select_dialog_option,      	&CCliMessageHandler::OnSelectDialogOption,  TRUE);
	REGISTER_MESSAGE(c2g_share_quest,					&CCliMessageHandler::OnShareQuest,	        TRUE);
//	REGISTER_MESSAGE(c2g_confirm_publish_quest,			&CCliMessageHandler::OnConfirmPublishQuest, TRUE);
	REGISTER_MESSAGE(c2g_talk_to_npc,					&CCliMessageHandler::OnTalkToNpc,	        TRUE);
	REGISTER_MESSAGE(c2g_explore_zone,					&CCliMessageHandler::OnExploreZone,	        TRUE);
//	REGISTER_MESSAGE(c2g_fly_to_quest_zone,				&CCliMessageHandler::OnFlyToQuestZone,      TRUE);
//	REGISTER_MESSAGE(c2g_generate_circle_quest,			&CCliMessageHandler::OnGenerateCircleQuest,	TRUE);
//	REGISTER_MESSAGE(c2g_buy_daily_quest_complete_time,	&CCliMessageHandler::OnBuyDailyQuestCompleteTime, TRUE);
//	REGISTER_MESSAGE(c2g_daily_quest_complete_shot_cut,	&CCliMessageHandler::OnDailyQuestCompleteShotCut, TRUE);
//	REGISTER_MESSAGE(c2g_brush_daily_quest,				&CCliMessageHandler::OnBrushDailyQuest,		TRUE);
//	REGISTER_MESSAGE(c2g_quest_recycle,					&CCliMessageHandler::OnQuestRecycle,		TRUE);
	REGISTER_MESSAGE(c2g_complete_kill_npc_quest,		&CCliMessageHandler::OnCompleteKillNpcQuest,TRUE);

//	REGISTER_MESSAGE(c2g_select_scene_difficulty,		&CCliMessageHandler::OnSelectSceneDifficulty,TRUE);
	REGISTER_MESSAGE(c2g_enter_dungeon,             	&CCliMessageHandler::OnEnterDungeon,        TRUE);
	REGISTER_MESSAGE(c2g_chat_message,					&CCliMessageHandler::OnChatMessage,		    TRUE);
	REGISTER_MESSAGE(c2g_change_line,					&CCliMessageHandler::OnChangeLine,		    TRUE);

	REGISTER_MESSAGE(c2g_adjacent_role_noteam,	    	&CCliMessageHandler::OnAdjacentRoleNoTeam,  TRUE);
	REGISTER_MESSAGE(c2g_adjacent_team,					&CCliMessageHandler::OnAdjacentTeam,		TRUE);

	REGISTER_MESSAGE(c2g_team_auction_bid,				&CCliMessageHandler::OnTeamAuctionBid,		TRUE);
	REGISTER_MESSAGE(c2g_team_auction_abandon,			&CCliMessageHandler::OnTeamAuctionAbandon,	TRUE);

	REGISTER_MESSAGE(c2g_friend_change_group_name,		&CCliMessageHandler::OnFriendChangeGroupName,TRUE);
	REGISTER_MESSAGE(c2g_return_select_role,	    	&CCliMessageHandler::OnReturnRoleSelect,	TRUE);

//	REGISTER_MESSAGE(c2g_dungeon_select_reward,	    	&CCliMessageHandler::OnDungeonSelectReward,	TRUE);
//	REGISTER_MESSAGE(c2g_dungeon_replay,				&CCliMessageHandler::OnDungeonReplay,	    TRUE);

	REGISTER_MESSAGE(c2g_title_equip,               	&CCliMessageHandler::OnEquipTitle,          TRUE);
	REGISTER_MESSAGE(c2g_title_unequip,             	&CCliMessageHandler::OnUnEquipTitle,        TRUE);

	REGISTER_MESSAGE(c2g_complete_achievement,      	&CCliMessageHandler::OnCompleteAchievement, TRUE);	
	REGISTER_MESSAGE(c2g_compare_achievement,       	&CCliMessageHandler::OnCompareAchievement,  TRUE);	
	REGISTER_MESSAGE(c2g_reward_achievement,        	&CCliMessageHandler::OnRewardAchievement,   TRUE);	

//	REGISTER_MESSAGE(c2g_query_combat_stat,				&CCliMessageHandler::OnQueryCombatStat,     TRUE);
	
	REGISTER_MESSAGE(c2g_on_mount,						&CCliMessageHandler::OnOnMount,             TRUE);
	REGISTER_MESSAGE(c2g_off_mount,						&CCliMessageHandler::OnOffMount,            TRUE);

	//REGISTER_MESSAGE(c2g_hire_hero,						&CCliMessageHandler::OnHireHero,            TRUE);
	//REGISTER_MESSAGE(c2g_fire_hero,						&CCliMessageHandler::OnFireHero,            TRUE);
	//REGISTER_MESSAGE(c2g_summon_hero,					&CCliMessageHandler::OnSummonHero,          TRUE);
	//REGISTER_MESSAGE(c2g_recall_hero,					&CCliMessageHandler::OnRecallHero,          TRUE);
	//REGISTER_MESSAGE(c2g_query_hero_attr_list,			&CCliMessageHandler::OnQueryHeroAttrList,   TRUE);
	//REGISTER_MESSAGE(c2g_change_hero_ai,				&CCliMessageHandler::OnChangeHeroAi,        TRUE);
	//REGISTER_MESSAGE(c2g_change_hero_name,				&CCliMessageHandler::OnChangeHeroName,      TRUE);

    // master
    REGISTER_MESSAGE(c2g_master_twitter,			&CCliMessageHandler::OnMasterTwitter,			TRUE);
	REGISTER_MESSAGE(c2g_query_master_reward_exp,	&CCliMessageHandler::OnQueryMasterRewardExp,	TRUE);
	REGISTER_MESSAGE(c2g_receive_master_reward_exp, &CCliMessageHandler::OnReceiveMasterRewardExp,	TRUE);

	// guild
    REGISTER_MESSAGE(c2g_update_client_variable,    &CCliMessageHandler::OnUpdateClientVaraible,TRUE);
	REGISTER_MESSAGE(c2g_guild_create,              &CCliMessageHandler::OnGuildCreate,         TRUE);
	REGISTER_MESSAGE(c2g_guild_draw_salary,         &CCliMessageHandler::OnGuildDrawSalary,     TRUE);
	REGISTER_MESSAGE(c2g_guild_contribute_fund,     &CCliMessageHandler::OnGuildContribute, TRUE);
    REGISTER_MESSAGE(c2g_guild_view_storage,        &CCliMessageHandler::OnGuildViewStorage,    TRUE);
    REGISTER_MESSAGE(c2g_guild_push_item,           &CCliMessageHandler::OnGuildPushItem,       TRUE);
    REGISTER_MESSAGE(c2g_guild_pop_item,            &CCliMessageHandler::OnGuildPopItem,        TRUE);
    REGISTER_MESSAGE(c2g_guild_create_building,     &CCliMessageHandler::OnGuildCreateBuilding, TRUE);
    REGISTER_MESSAGE(c2g_guild_upgrade_building,    &CCliMessageHandler::OnGuildUpgradeBuilding,TRUE);
    REGISTER_MESSAGE(c2g_guild_del_building,        &CCliMessageHandler::OnGuildDelBuilding,    TRUE);
//    REGISTER_MESSAGE(c2g_guild_buy_aura,            &CCliMessageHandler::OnGuildBuyAura,        TRUE);
//    REGISTER_MESSAGE(c2g_guild_change_weather,      &CCliMessageHandler::OnGuildChangeWeather,  TRUE);
    REGISTER_MESSAGE(c2g_guild_enter_map,           &CCliMessageHandler::OnGuildEnterMap,       TRUE);
//    REGISTER_MESSAGE(c2g_guild_open_dungeon,        &CCliMessageHandler::OnGuildOpenDungeon,    TRUE);
//    REGISTER_MESSAGE(c2g_guild_enter_dungeon,       &CCliMessageHandler::OnGuildEnterDungeon,   TRUE);
    REGISTER_MESSAGE(c2g_guild_reward_achievement, &CCliMessageHandler::OnGuildRewardAchievement, TRUE);

    //REGISTER_MESSAGE(c2g_arena_join_season,         &CCliMessageHandler::OnArenaJoinSeason,     TRUE);
    //REGISTER_MESSAGE(c2g_arena_quit_season,         &CCliMessageHandler::OnArenaQuitSeason,     TRUE);
    //REGISTER_MESSAGE(c2g_arena_match,               &CCliMessageHandler::OnArenaMatch,          TRUE);
    //REGISTER_MESSAGE(c2g_arena_get_reward,          &CCliMessageHandler::OnArenaGetReward,      TRUE);

	//REGISTER_MESSAGE(c2g_teleport_by_platform,			&CCliMessageHandler::OnTeleportByPlatform,  TRUE);

	// spirit
	//REGISTER_MESSAGE(c2g_spirit_exp_change,				&CCliMessageHandler::OnSpiritExpChange,     TRUE);
	//REGISTER_MESSAGE(c2g_spirit_add_air,				&CCliMessageHandler::OnSpiritAddAir,        TRUE);	
	//REGISTER_MESSAGE(c2g_spirit_merge_air,				&CCliMessageHandler::OnSpiritMergeAir,      TRUE);	
	//REGISTER_MESSAGE(c2g_spirit_move_air,				&CCliMessageHandler::OnSpiritMoveAir,       TRUE);	
	//REGISTER_MESSAGE(c2g_spirit_exchange_air,			&CCliMessageHandler::OnSpiritExchangeAir,   TRUE);	
	//REGISTER_MESSAGE(c2g_spirit_buy_air_slot,			&CCliMessageHandler::OnSpiritBuySlot,		TRUE);	
	//REGISTER_MESSAGE(c2g_spirit_brush_slot,				&CCliMessageHandler::OnSpiritBrushSlot,		TRUE);	
	//REGISTER_MESSAGE(c2g_spirit_blood_open,				&CCliMessageHandler::OnSpiritBloodOpen,		TRUE);	

    REGISTER_MESSAGE(c2g_unshield,                  	&CCliMessageHandler::OnUnShieldRole,        TRUE);

    // Soul fusion - create sockets / insert gems / remove gems
    REGISTER_MESSAGE(c2g_upgrade_soul,              	&CCliMessageHandler::OnUpgradeSoul,         TRUE);
    REGISTER_MESSAGE(c2g_add_soul,                  	&CCliMessageHandler::OnAddSoul,             TRUE);
    REGISTER_MESSAGE(c2g_remove_soul,               	&CCliMessageHandler::OnRemoveSoul,          TRUE);

    // Soul forging - normal inscription refinement
    REGISTER_MESSAGE(c2g_rune_item,                     &CCliMessageHandler::OnRuneItem,            TRUE);

    // Equipment upgrade
    REGISTER_MESSAGE(c2g_weapon_level_up_put_in,        &CCliMessageHandler::OnWeaponLevelUpPutIn,  TRUE);
    REGISTER_MESSAGE(c2g_weapon_level_up_take_out,      &CCliMessageHandler::OnWeaponLevelUpTakeOut,TRUE);

    // Decomposition and crafting
    REGISTER_MESSAGE(c2g_decompose,              	    &CCliMessageHandler::OnDecompose,           TRUE);
    REGISTER_MESSAGE(c2g_compose,              	        &CCliMessageHandler::OnCompose,             TRUE);

	REGISTER_MESSAGE(c2g_logout,	                	&CCliMessageHandler::OnLoginOut,	        TRUE);
	REGISTER_MESSAGE(c2g_logout_cancel,					&CCliMessageHandler::OnLoginOutCancel,		FALSE);

    // vip
//    REGISTER_MESSAGE(c2g_subscribe_vip,					&CCliMessageHandler::OnSubscribeVIP,		TRUE);

	// activity
//    REGISTER_MESSAGE(c2g_activity_reward,				&CCliMessageHandler::OnActivityReward,		TRUE);
    
	// client info
	REGISTER_MESSAGE(c2g_client_hardware_info,			&CCliMessageHandler::OnHardwareInfo,		TRUE);
	REGISTER_MESSAGE(c2g_client_fps_info,				&CCliMessageHandler::OnClientFpsInfo,		TRUE);

	// survey
	REGISTER_MESSAGE(c2g_query_survey_path,				&CCliMessageHandler::OnQuerySurveyPath,		TRUE);

	REGISTER_MESSAGE(c2g_receive_reputation_award,		&CCliMessageHandler::OnReceiveReputationAward, TRUE);

    // pet
    REGISTER_MESSAGE(c2g_change_pet_state,              &CCliMessageHandler::OnPetChangeState,      TRUE);

	// checkin
    REGISTER_MESSAGE(c2g_checkin,					    &CCliMessageHandler::OnCheckIn,				TRUE);

	// anti-cheat
    REGISTER_MESSAGE(c2g_jigsaw_answer,				    &CCliMessageHandler::OnJigsawAnswer,		TRUE);

	REGISTER_MESSAGE(c2g_weapon_level_up_take_out_immediately, &CCliMessageHandler::OnWeaponLevelUpTakeOutImmediately, TRUE);

	REGISTER_MESSAGE(c2g_potion_gamble,                 &CCliMessageHandler::OnPotionGamble, TRUE);
	REGISTER_MESSAGE(c2g_gamble_advance,                &CCliMessageHandler::OnGambleAdvance, TRUE);
	REGISTER_MESSAGE(c2g_spell_book_gamble,             &CCliMessageHandler::OnSpellBookGamble, TRUE);

	REGISTER_MESSAGE(c2g_arena_reward,                  &CCliMessageHandler::OnArenaReward, TRUE);

	REGISTER_MESSAGE(c2g_leave_guild,                   &CCliMessageHandler::OnLeavelGuild, TRUE);
	REGISTER_MESSAGE(c2g_query_guild_war_record,        &CCliMessageHandler::OnQueryGuildWarRecord, TRUE);

	// Advanced inscription refinement and confirmation
	REGISTER_MESSAGE(c2g_rune_item_advance,             &CCliMessageHandler::OnRuneItemAdvance, TRUE);
	REGISTER_MESSAGE(c2g_rune_item_advance_confirm,     &CCliMessageHandler::OnRuneItemConfirm, TRUE);

	REGISTER_MESSAGE(c2g_sell_resource_item,            &CCliMessageHandler::OnSellResourceItem, TRUE);

	REGISTER_MESSAGE(c2g_dice_bet,                      &CCliMessageHandler::OnDiceBet, TRUE);
	REGISTER_MESSAGE(c2g_lovers_teleprot,               &CCliMessageHandler::OnLoversTeleprot, TRUE);
	REGISTER_MESSAGE(c2g_wing_op,                       &CCliMessageHandler::OnWingOp, TRUE);




    return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::UnInit(void)
{
	m_poNotifier = NULL;
	m_poPacketAllocator = NULL;

	return TRUE;
}

/************************************************************************/
//void CCliMessageHandler::OnGMCommand(CRole* poRole, const char* pData)
//{
//	INT32 nRetCode = 0;
//	C2G_GM_COMMAND* pMsg = (C2G_GM_COMMAND*)pData;
//	CLuaScript* poScript = NULL;
//    BOOL bGMRet = FALSE;
//
//	if (strnicmp(pMsg->szGMCommand, "Reload", sizeof("Reload")) == 0)
//	{
//		CGameWorld::GetInstance().Reload("");
//	}
//	else
//	{
//		poScript = CScriptMgr::GetInstance().FindScript(GM_SCRIPT_NAME);
//		LOG_PROCESS_ERROR(poScript);
//
//		cqyh::lua::push_object(poScript->GetLuaState(), poRole, "CRole");
//		lua_setglobal(poScript->GetLuaState(), "TargetRole");
//
//        nRetCode = poScript->CallFunction("OnGMCommand", "s:b", pMsg->szGMCommand, &bGMRet);
//		LOG_PROCESS_ERROR(nRetCode);
//		LOG_PROCESS_ERROR(bGMRet);
//	}
//
//	nRetCode = CCliMessageHandler::GetInstance().DoDLGmCommand(poRole, CGameWorld::GetInstance().GetGameTime(), g_ServerConfig.LS.nAreaID, pMsg->szGMCommand, TRUE);
//	LOG_CHECK_ERROR(nRetCode);
//
//	DBG("role %s executed gm command %s", poRole->GetName(), pMsg->szGMCommand);
//		
//	return;
//Exit0:
//	DBG("role %s failed to execute gm command %s", poRole->GetName(), pMsg->szGMCommand);
//
//	nRetCode = CCliMessageHandler::GetInstance().DoDLGmCommand(poRole, CGameWorld::GetInstance().GetGameTime(), g_ServerConfig.LS.nAreaID, pMsg->szGMCommand, FALSE);
//	LOG_CHECK_ERROR(nRetCode);
//
//	return;
//}

void CCliMessageHandler::OnLoadFinish(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_LOADFINISH* pMsg = (C2G_LOADFINISH*)pData;
	CScene* poScene = NULL;
	UINT32 dwClientSessionID = 0;

	LOG_PROCESS_ERROR(rsWaitClientLoad == poRole->GetState() || rsJumpInGSWaitClientLoad == poRole->GetState());
	LOG_PROCESS_ERROR(pMsg->nErrCode == errSuccess);
	LOG_PROCESS_ERROR(poRole->GetScene() == NULL);

	dwClientSessionID = poRole->GetSessionID();
	poScene = CGameWorld::GetInstance().FindScene(MakeSceneObjID(poRole->GetSceneTempID(), poRole->GetSceneID()));
	LOG_PROCESS_ERROR(poScene != NULL);

	nRetCode = CCliMessageHandler::GetInstance().DoSyncFrame(poRole);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = poScene->AddRole(poRole, soatLogin);
	LOG_PROCESS_ERROR(nRetCode);

	// the role does not get its own appear data: tell the client which wing is worn (login, scene change, jump)
	poRole->SyncWingInfo(FALSE);

	{
		CAura* pAura = NULL;
		CAuraInstance* pAuraInstance = NULL;

		pAuraInstance = poRole->GetFirstAura(g_GWConfig.nSceneFinishBuffID);
		if (pAuraInstance)
			poRole->RemoveAura(pAuraInstance, FALSE);
		
		pAura = CSpellConfig::GetInstance().FindAuraTemplate(g_GWConfig.nEnterSceneBuffID);
		if (pAura)
		{
			pAuraInstance = poRole->AppendAura(pAura, poRole, poRole->GetObjID(), NULL, 0, 0, 0, 0);
			LOG_CHECK_ERROR(pAuraInstance);
		}

		poRole->AppendAllPendingAura();
	}

    if (poRole->IsInJump() || poRole->IsInLogin())
    {   // pet
        poRole->RecoverPet();
    }

    poRole->ClearInJump();
    poRole->ClearInLogin();

	nRetCode = CCliMessageHandler::GetInstance().DoSyncDoorState(poRole, poScene);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = CCliMessageHandler::GetInstance().DoSyncSceneVariables(poRole, poScene);
	LOG_CHECK_ERROR(nRetCode);

	GS_SYS_LOG_SUCCESS(poRole);
	return;
Exit0:
	GS_SYS_LOG_ERROR(poRole);
    KICK_ROLE(poRole, ktLogged);

	return;
}

void CCliMessageHandler::OnReturnRoleSelect(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_RETURN_SELECT_ROLE* pMsg = (C2G_RETURN_SELECT_ROLE*)pData;
	const GRID_DATA* pcGrid = NULL;

//	pcGrid = poRole->GetGrid();
//	LOG_PROCESS_ERROR(pcGrid);
//	PROCESS_ERROR_RET_CODE(pcGrid->uPkType == gptSafe, errLogoutNotPermit);

	nRetCode = poRole->Logout(rltReturnSelectRole, 0);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);

	return;
}

void CCliMessageHandler::OnUpdateClientVaraible(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_UPDATE_CLIENT_VARIABLE* pMsg = (C2G_UPDATE_CLIENT_VARIABLE*)pData;

	nRetCode = poRole->SetClientVariable(pMsg->byVarIndex, pMsg->nVarValue);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
    KICK_ROLE(poRole, ktLogged);

	return;
}

void CCliMessageHandler::OnQueryCombatStat(CRole* poRole, const char* pData)
{
    INT32 nRetCode = 0;

    nRetCode = DoSyncCombatStat(poRole);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

/************************************************************************/
BOOL CCliMessageHandler::CTraverseRole::operator()(CRole* poRole)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);
	{
		UINT32 dwClientSessionID = poRole->GetSessionID();
		UINT32 dwGTSessionID = GET_GT_INDEX_FROM_SESSION_ID(dwClientSessionID);

		LOG_PROCESS_ERROR(s_dwSessionCount[dwGTSessionID] < MAX_SESSION_PER_SEND);
		s_dwSessionIDList[dwGTSessionID][s_dwSessionCount[dwGTSessionID]++] = dwClientSessionID;
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::_BroadcastRegion(CRegion* poRegion, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;

	LOG_PROCESS_ERROR(poRegion);
	LOG_PROCESS_ERROR(pPacketBuffer);

	memset(s_dwSessionCount, 0, sizeof(s_dwSessionCount));

	nRetCode = poRegion->TraverseRole(m_TraverseRole);
	LOG_PROCESS_ERROR(nRetCode);

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		if (s_dwSessionCount[nIndex] == 0)
			continue;

		pMsg = m_poPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg, pPacketBuffer, dwSize);

		nRetCode = m_poNotifier->SendToUsers(nIndex, s_dwSessionIDList[nIndex], s_dwSessionCount[nIndex], pMsg, dwSize);
		if (!nRetCode)
			m_poPacketAllocator->Free(pMsg);
		pMsg = NULL;
	}

	m_poPacketAllocator->Free(pPacketBuffer);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free(pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::CTraverseRegion::operator()(CRegion* poRegion)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRegion);

	nRetCode = poRegion->TraverseRole(poHandler->m_TraverseRole);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::_Broadcast9Region(CRegion* poRegion, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;

	LOG_PROCESS_ERROR(poRegion);
	LOG_PROCESS_ERROR(pPacketBuffer);

	memset(s_dwSessionCount, 0, sizeof(s_dwSessionCount));
	nRetCode = poRegion->Traverse9Region(m_TraverseRegion);
	LOG_PROCESS_ERROR(nRetCode);

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		if (s_dwSessionCount[nIndex] == 0)
			continue;

		pMsg = m_poPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg, pPacketBuffer, dwSize);

		nRetCode = m_poNotifier->SendToUsers(nIndex, s_dwSessionIDList[nIndex], s_dwSessionCount[nIndex], pMsg, dwSize);
		if (!nRetCode)
			m_poPacketAllocator->Free(pMsg);

		pMsg = NULL;
	}

	m_poPacketAllocator->Free(pPacketBuffer);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free(pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::_BroadcastScene(CScene* poScene, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;

	LOG_PROCESS_ERROR(poScene);
	LOG_PROCESS_ERROR(pPacketBuffer);

	memset(s_dwSessionCount, 0, sizeof(s_dwSessionCount));

	nRetCode = poScene->TraverseRole(m_TraverseRole);
	LOG_PROCESS_ERROR(nRetCode);

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		if (s_dwSessionCount[nIndex] == 0)
			continue;

		pMsg = m_poPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg, pPacketBuffer, dwSize);

		nRetCode = m_poNotifier->SendToUsers(nIndex, s_dwSessionIDList[nIndex], s_dwSessionCount[nIndex], pMsg, dwSize);
		if (!nRetCode)
			m_poPacketAllocator->Free(pMsg);

		pMsg = NULL;
	}

	m_poPacketAllocator->Free(pPacketBuffer);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free(pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::_BroadcastTeam(CTeamInGS* poTeam, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;

	LOG_PROCESS_ERROR(poTeam);
	LOG_PROCESS_ERROR(pPacketBuffer);

	memset(s_dwSessionCount, 0, sizeof(s_dwSessionCount));

	for (INT32 nIndex = 0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
	{
		UINT32 dwClientSession = 0;
		UINT32 dwGTSession = 0;
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(poTeam->GetMemberTable(nIndex).dwRoleID);
		if (poRole == NULL)
			continue;

		dwClientSession = poRole->GetSessionID();
		dwGTSession = GET_GT_INDEX_FROM_SESSION_ID(dwClientSession);
		s_dwSessionIDList[dwGTSession][s_dwSessionCount[dwGTSession]++] = poRole->GetSessionID();
	}

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		if (s_dwSessionCount[nIndex] == 0)
			continue;

		pMsg = m_poPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg, pPacketBuffer, dwSize);

		nRetCode = m_poNotifier->SendToUsers(nIndex, s_dwSessionIDList[nIndex], s_dwSessionCount[nIndex], pMsg, dwSize);
		if (!nRetCode)
			m_poPacketAllocator->Free(pMsg);

		pMsg = NULL;
	}

	m_poPacketAllocator->Free(pPacketBuffer);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free(pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::_BroadcastTeamToSameSceneMember(CRole* poRole, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{	
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;
	CTeamInGS* pstTeam = NULL;

	LOG_PROCESS_ERROR(poRole);
	
	pstTeam = CGameWorld::GetInstance().FindTeam(poRole->GetBattleTeamID());
	if(pstTeam == NULL)
	{
		pstTeam = CGameWorld::GetInstance().FindTeam(poRole->GetTeamID());
		PROCESS_SUCCESS(pstTeam == NULL);
	}

	memset(s_dwSessionCount, 0, sizeof(s_dwSessionCount));

	for (INT32 nIndex = 0; nIndex < pstTeam->GetMaxMemberCount(); nIndex++)
	{
		UINT32 dwClientSession = 0;
		UINT32 dwGTSession = 0;
		CRole* poMember = CGameWorld::GetInstance().FindValidRole(pstTeam->GetMemberTable(nIndex).dwRoleID);
		if (poMember == NULL)
			continue;

		if(poMember->GetObjID() == poRole->GetObjID())
			continue;

		if(poMember->GetScene() != poRole->GetScene())
			continue;

		dwClientSession = poMember->GetSessionID();
		dwGTSession = GET_GT_INDEX_FROM_SESSION_ID(dwClientSession);
		s_dwSessionIDList[dwGTSession][s_dwSessionCount[dwGTSession]++] = poMember->GetSessionID();
	}

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		if (s_dwSessionCount[nIndex] == 0)
			continue;

		pMsg = m_poPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg, pPacketBuffer, dwSize);

		nRetCode = m_poNotifier->SendToUsers(nIndex, s_dwSessionIDList[nIndex], s_dwSessionCount[nIndex], pMsg, dwSize);
		if (!nRetCode)
			m_poPacketAllocator->Free(pMsg);

		pMsg = NULL;
	}

Exit1:
	m_poPacketAllocator->Free(pPacketBuffer);
	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free(pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::_BroadcastTeamToInSceneMember(CTeamInGS* poTeam, CScene* poScene, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	PACKET_BUFFER* pMsg = NULL;

	LOG_PROCESS_ERROR(poScene);
	LOG_PROCESS_ERROR(poTeam);
	
	memset(s_dwSessionCount, 0, sizeof(s_dwSessionCount));

	for (INT32 nIndex = 0; nIndex < poTeam->GetMaxMemberCount(); nIndex++)
	{
		UINT32 dwClientSession = 0;
		UINT32 dwGTSession = 0;
		CRole* poMember = CGameWorld::GetInstance().FindValidRole(poTeam->GetMemberTable(nIndex).dwRoleID);
		if (poMember == NULL)
			continue;

		if(poMember->GetScene() != poScene)
			continue;

		dwClientSession = poMember->GetSessionID();
		dwGTSession = GET_GT_INDEX_FROM_SESSION_ID(dwClientSession);
		s_dwSessionIDList[dwGTSession][s_dwSessionCount[dwGTSession]++] = poMember->GetSessionID();
	}

	for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
	{
		if (s_dwSessionCount[nIndex] == 0)
			continue;

		pMsg = m_poPacketAllocator->Alloc(dwSize);
		LOG_PROCESS_ERROR(pMsg);

		memcpy(pMsg, pPacketBuffer, dwSize);

		nRetCode = m_poNotifier->SendToUsers(nIndex, s_dwSessionIDList[nIndex], s_dwSessionCount[nIndex], pMsg, dwSize);
		if (!nRetCode)
			m_poPacketAllocator->Free(pMsg);

		pMsg = NULL;
	}

Exit1:
	m_poPacketAllocator->Free(pPacketBuffer);
	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free(pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::_BroadcaseGroupClient(INT32 nServerGroup, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_BROADCAST_GROUP_CLIENT* pMsg = NULL;

	LOG_PROCESS_ERROR(pPacketBuffer);

	pMsg = (InternalMessage::G2M_BROADCAST_GROUP_CLIENT*)m_poPacketAllocator->Alloc(
		sizeof(InternalMessage::G2M_BROADCAST_GROUP_CLIENT) + dwSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_broadcast_group_client;
	pMsg->byServerGroup = (UINT8)nServerGroup;
	memcpy(pMsg->MessageData, pPacketBuffer, dwSize);

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, 
		sizeof(InternalMessage::G2M_BROADCAST_GROUP_CLIENT) + dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	m_poPacketAllocator->Free(pPacketBuffer);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoErrorCodeNotify(CRole* poRole, INT32 nErrCode, INT32 nParam1, INT32 nParam2)
{
	INT32 nRetCode = 0;
	G2C_ERROR_NOTIFY* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ERROR_NOTIFY*)m_poPacketAllocator->Alloc(sizeof(G2C_ERROR_NOTIFY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_error_notify;

	pMsg->nErrCode = nErrCode;
	pMsg->nParam1 = nParam1;
	pMsg->nParam2 = nParam2;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ERROR_NOTIFY));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncReputation(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_REPUTATION* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_REPUTATION*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_REPUTATION));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_reputation;
	pMsg->qwActiveState = poRole->GetReputation().GetActiveState();
	pMsg->qwAwardState = poRole->GetReputation().GetAwardState();

	for (INT32 nIndex = 0; nIndex < MAX_NPC_FORCE_ID; nIndex++)
	{
		pMsg->ReputationTable[nIndex].byForceID = (UINT8)nIndex;
		pMsg->ReputationTable[nIndex].shReputation = (INT16)poRole->GetReputation().GetValue(nIndex);
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_REPUTATION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoModifyReputation(CRole* poRole, INT32 nForceID, INT32 nNewValue)
{
	INT32 nRetCode = 0;
	G2C_MODIFY_REPUTATION* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_MODIFY_REPUTATION*)m_poPacketAllocator->Alloc(sizeof(G2C_MODIFY_REPUTATION));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_modify_reputation;
	pMsg->byForceID = (UINT8)nForceID;
	pMsg->nReputation = nNewValue;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_MODIFY_REPUTATION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncGameDataBegin(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_GAME_DATA_BEGIN* pMsg = NULL;
    const ROLE_LEVEL_DATA* data = NULL;
	CHero* poHero = NULL;

	LOG_PROCESS_ERROR(poRole);

	data = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(poRole->GetLevel());
	LOG_PROCESS_ERROR(data);

	pMsg = (G2C_SYNC_GAME_DATA_BEGIN*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_GAME_DATA_BEGIN));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_game_data_begin;
	pMsg->byClass = poRole->GetClass();
	pMsg->byLevel = poRole->GetLevel();
	pMsg->bySex = poRole->GetSex();
    pMsg->byPetState = (UINT8)poRole->GetPetState();
	pMsg->dwExp = poRole->GetExp();
    pMsg->dwExtraExp = poRole->GetExtraExp();
	pMsg->wPosX = ConvertCoordS2N(poRole->GetX());
	pMsg->wPosY = ConvertCoordS2N(poRole->GetY());
	pMsg->wPosZ = ConvertCoordS2N(poRole->GetZ());
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->dwSceneID = poRole->GetSceneID();
	pMsg->dwSceneTempID = poRole->GetSceneTempID();
	pMsg->wFaceDirection = poRole->GetFaceDirection();
	pMsg->dwMaxExp = data->m_nMaxExp;
    pMsg->dwTodayExp = poRole->GetTodayExp();
    pMsg->dwTodayMaxExp = data->m_nDailyExpLimit;
	pMsg->byMoveState = poRole->GetMoveState();
	pMsg->dwTeamID = poRole->GetTeamID();
	pMsg->dwPkValue = poRole->GetPkValue();
	pMsg->byPkMode = poRole->GetPkMode();
    pMsg->dwVIPTime[vlLevel0] = 0;
    pMsg->dwVIPTime[vlLevel1] = poRole->GetVIPTime(vlLevel1);
    pMsg->dwVIPTime[vlLevel2] = poRole->GetVIPTime(vlLevel2);
    pMsg->dwVIPTime[vlLevel3] = poRole->GetVIPTime(vlLevel3);
	pMsg->dwServerTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->dwServerStartTime = g_ServerConfig.GroupConfig[poRole->GetServerGroup()].dwStartDate;
	pMsg->wMaxLevel = g_GWConfig.nLevelLimit;    // Set the maximum game level

	pMsg->dwHeroID = 0;
	pMsg->byHeroClass = clsInvalid;
	pMsg->byHeroSex = cstInvalid;
	pMsg->byHeroLevel = 0;

	pMsg->wMountID = poRole->GetCurrentMountID();

	pMsg->nActivityLevel = poRole->GetActivity();
	pMsg->nActicityReward = poRole->GetActivityReward();

	poHero = poRole->GetHero();
	if(poHero != NULL)
	{
		pMsg->dwHeroID = poHero->GetObjID();
		pMsg->byHeroClass = poHero->GetClass();
		pMsg->byHeroSex = poHero->GetSex();
		pMsg->byHeroLevel = poHero->GetLevel();
		pMsg->nHeroGrowth = poHero->GetGrowth();
		pMsg->byHeroAiType = poHero->GetAiType();
		strxcpy(pMsg->szHeroName, poHero->GetName(), ROLE_NAME_LEN);
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_GAME_DATA_BEGIN));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncGameDataEnd(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_GAME_DATA_END* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_GAME_DATA_END*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_GAME_DATA_END));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_game_data_end;
	pMsg->dwRoleID = poRole->GetObjID();

	LOG_PROCESS_ERROR(poRole);

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_GAME_DATA_END));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSaveRole(CRole* poRole, BOOL bSaveFile)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(poRole);

	nRetCode = m_poNotifier->SaveRoleData(poRole, bSaveFile);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoReturnSelectRole(CRole* poRole)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(poRole);

	nRetCode = m_poNotifier->ReturnSelectRole(poRole->GetSessionID());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoLogout(CRole* poRole)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(poRole);

	nRetCode = m_poNotifier->Logout(poRole->GetSessionID());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoDLRoleLogout(CRole* poRole, BOOL bInJump)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_ROLE_LOGOUT* pMsg = NULL;
	UINT32 dwSessionID = 0;
	UINT32 dwSize = 0;
	CGSGuild* pGuild = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_ROLE_LOGOUT*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_role_logout;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->dwSessionID = poRole->GetSessionID();
	pMsg->bInJump = bInJump;

	nRetCode = poRole->Save(pMsg->szData, pMsg->dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	dwSize = sizeof(InternalMessage::G2O_ROLE_LOGOUT) + pMsg->dwSize;

	pGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
	if (pGuild)
	{
		strxcpy(pMsg->szGuildName, pGuild->GetName(), GUILD_NAME_LEN);
	}
	else
	{
		strxcpy(pMsg->szGuildName, "none_guild", GUILD_NAME_LEN);
	}

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSaveAndUnlockRole(CRole* poRole)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poRole);

	nRetCode = m_poNotifier->SaveRoleData(poRole, FALSE);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_poNotifier->UnlockRole(poRole);
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::DoClientLogoutTimer(CRole* poRole, UINT8 byStart, UINT8 byDelayTime)
{
	INT32 nRetCode = 0;
	G2C_LOGOUT_TIMER* pMsg = NULL;
	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_LOGOUT_TIMER*)m_poPacketAllocator->Alloc(sizeof(G2C_LOGOUT_TIMER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_logout_timer;
	pMsg->byStart = byStart;
	pMsg->byDelayTime = byDelayTime;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_LOGOUT_TIMER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncVIPState(CRole* poRole)
{
    INT32 nRetCode = 0;
    G2C_SYNC_VIP_STATE* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_VIP_STATE*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_VIP_STATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_vip_state;
    for (INT32 nIndex = vlLevel1; nIndex < vlTotal; nIndex++)
    {
        pMsg->dwVIPTime[nIndex] = poRole->GetVIPTime(nIndex);
    }

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_VIP_STATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncVIPLevel(CRole* poRole)
{
    INT32 nRetCode = 0;
    G2C_SYNC_VIP_LEVEL* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_VIP_LEVEL*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_VIP_LEVEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_vip_level;
    pMsg->dwRoleID = poRole->GetObjID();
    pMsg->byVIPLevel = (UINT8)poRole->GetVIPLevel();

    nRetCode = _Broadcast9Region(poRole->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_VIP_LEVEL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// activity
BOOL CCliMessageHandler::DoActivityChange(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_ACTIVITY_CHANGE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ACTIVITY_CHANGE*)m_poPacketAllocator->Alloc(sizeof(G2C_ACTIVITY_CHANGE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_activity_change;
	pMsg->wActivity = (UINT16)poRole->GetActivity();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ACTIVITY_CHANGE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoActivityReward(CRole* poRole, INT32 nIndex)
{
	INT32 nRetCode = 0;
	G2C_ACTIVITY_REWARD* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ACTIVITY_REWARD*)m_poPacketAllocator->Alloc(sizeof(G2C_ACTIVITY_REWARD));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_activity_reward;
	pMsg->wIndex = (UINT16)nIndex;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ACTIVITY_REWARD));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoPlayAction(CCharacter* pChar, UINT32 dwActionID)
{
	INT32 nRetCode = 0;
	G2C_PLAY_ACTION* pMsg = NULL;

	LOG_PROCESS_ERROR(pChar);

	pMsg = (G2C_PLAY_ACTION*)m_poPacketAllocator->Alloc(sizeof(G2C_PLAY_ACTION));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_play_action;
	pMsg->dwCharID = pChar->GetObjID();
	pMsg->byActionID = (UINT8)dwActionID;

	nRetCode = _Broadcast9Region(pChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_PLAY_ACTION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateSceneWeather(CScene* poScene)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_WEATHER* pMsg = NULL;

	LOG_PROCESS_ERROR(poScene);

	pMsg = (G2C_UPDATE_WEATHER*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_WEATHER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_weather;
	pMsg->byWeather = (UINT8)poScene->GetWeather();

	nRetCode = _BroadcastScene(poScene, (PACKET_BUFFER*)pMsg, sizeof(G2C_UPDATE_WEATHER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLChatMessage(CRole* poRole, UINT32 dwTargetID, const char* pcszMessage, INT32 nMessageSize)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_CHAT_MESSAGE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pcszMessage);

	dwSessionID = poRole->GetSessionID();
	nSize = sizeof(InternalMessage::G2O_CHAT_MESSAGE) + nMessageSize;

	pMsg = (InternalMessage::G2O_CHAT_MESSAGE*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_chat_message;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwTargetID = dwTargetID;
	pMsg->dwSourceID = poRole->GetObjID();
	strxcpy(pMsg->szMessage, pcszMessage, nMessageSize);
	pMsg->nMessageSize = nMessageSize;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, nSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLCastSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel, INT32 nRuneSlot, UINT32 dwTargetID, INT32 nTargetX, INT32 nTargetY, INT32 nTargetZ, INT32 nMp, INT32 nCdRemain, INT32 nErrCode, UINT32 dwClientTime, BOOL bFromServer, BOOL bShiftPressed)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_CAST_SPELL* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	dwSessionID = poRole->GetSessionID();

	pMsg = (InternalMessage::G2O_CAST_SPELL*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_CAST_SPELL));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_cast_spell;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);

	pMsg->dwCasterID = poRole->GetObjID();
	pMsg->byCasterLevel = poRole->GetLevel();
	pMsg->dwTargetID = dwTargetID;
	pMsg->dwSpellID = nSpellID;
	pMsg->bySpellLevel = nSpellLevel;
	pMsg->byRuneSlot = nRuneSlot;
	pMsg->bySpellSerial = poRole->GetSpellSerial();
	pMsg->nTargetX = nTargetX;
	pMsg->nTargetY = nTargetY;
	pMsg->nTargetZ = nTargetZ;
	pMsg->nMp = nMp;
	pMsg->nCdRemain = nCdRemain;
	pMsg->nErrCode = nErrCode;
	pMsg->dwClientTime = dwClientTime;
	pMsg->byFromServer = bFromServer;
	pMsg->byShiftPressed = bShiftPressed;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_CAST_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLSpellEffect(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel, INT32 nRuneSlot, UINT32 dwTargetID, INT32 nSpellSerial, INT32 nParam1, INT32 nParam2, INT32 nFlag)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_SPELL_EFFECT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	dwSessionID = poRole->GetSessionID();

	pMsg = (InternalMessage::G2O_SPELL_EFFECT*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_SPELL_EFFECT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_spell_effect;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwCasterID = poRole->GetObjID();
	pMsg->dwTargetID = dwTargetID;
	pMsg->dwSpellID = nSpellID;
	pMsg->bySpellLevel = nSpellLevel;
	pMsg->byRuneSlot = nRuneSlot;
	pMsg->bySpellSerial = nSpellSerial;
	pMsg->nParam1 = nParam1;
	pMsg->nParam2 = nParam2;
	pMsg->wFlag = nFlag;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_SPELL_EFFECT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLLearnSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_LEARN_SPELL* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	dwSessionID = poRole->GetSessionID();

	pMsg = (InternalMessage::G2O_LEARN_SPELL*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_LEARN_SPELL));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_learn_spell;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nSpellID = nSpellID;
	pMsg->bySpellLevel = nSpellLevel;
	pMsg->byRoleLevel = poRole->GetLevel();
	pMsg->nRoleExp = poRole->GetExp();

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_LEARN_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLLevelUp(CRole* poRole, UINT32 dwRoleID, INT32 nLevel)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_LEVEL_UP* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	dwSessionID = poRole->GetSessionID();

	pMsg = (InternalMessage::G2O_LEVEL_UP*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_LEVEL_UP));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_level_up;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = dwRoleID;
	pMsg->nLevel = nLevel;
	pMsg->nLevelOnlineTime = poRole->GetLevelOnlineTime();
	pMsg->nTotalOnlineTime = poRole->GetTotalOnlineTime();

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_LEVEL_UP));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLDie(CRole* poRole, INT32 nSceneTemplateID, INT32 nSceneID, UINT32 dwKillerID, const char* pKillName, INT32 nKillerLevel, INT32 nKillClass, INT64 llX, INT64 llY, INT64 llZ)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_DIE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pKillName);

	dwSessionID = poRole->GetSessionID();

	pMsg = (InternalMessage::G2O_DIE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_DIE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_die;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);

	pMsg->nSceneTemplateID = nSceneTemplateID;
	pMsg->nSceneID = nSceneID;
	pMsg->llX = llX;
	pMsg->llY = llY;
	pMsg->llZ = llZ;

	pMsg->dwKillerID = dwKillerID;
	pMsg->nKillerType = GetTypeFromID(dwKillerID);
	pMsg->nKillLevel = nKillerLevel;
	pMsg->nKillClass = nKillClass;

	pMsg->dwVicID =poRole->GetObjID();
	pMsg->nVicLevel = poRole->GetLevel();
	pMsg->nVicClass = poRole->GetClass();

	strxcpy(pMsg->szKillerName, pKillName, ROLE_NAME_LEN);
	strxcpy(pMsg->szVicName, poRole->GetName(), ROLE_NAME_LEN);

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_DIE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLBuyConSlot(CRole* poRole, INT32 nConType, INT32 nConSlotSize)
{
	INT32 nRetCode = 0;
	INT32 nSize = 0;
	UINT32 dwSessionID = 0;
	InternalMessage::G2O_BUY_CONSLOT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	dwSessionID = poRole->GetSessionID();

	pMsg = (InternalMessage::G2O_BUY_CONSLOT*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_BUY_CONSLOT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_buy_conslot;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nConSize = nConSlotSize;
	pMsg->nConType = nConType;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_BUY_CONSLOT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLLogItem(UINT32 dwRoleID, INT32 nGroupID, INT64 Guid, 
	INT32 nTemplateID, INT32 nItemCount, INT32 nReason, INT32 nAction, UINT64 qwUserDefinedID)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    InternalMessage::G2O_LOG_ITEM* pMsg = NULL;

    pMsg = (InternalMessage::G2O_LOG_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_LOG_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2o_log_item;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->qwItemGuid = Guid;
    pMsg->nItemTemplateID = nTemplateID;
    pMsg->nItemCount = nItemCount;
    pMsg->nReason = nReason;
    pMsg->nAction = nAction;
    pMsg->qwUserDefinedID = qwUserDefinedID;

    nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_LOG_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDLStatItem(INT32 nGroupID, INT32 nTemplateID, INT32 nItemCount, INT32 nAction)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    InternalMessage::G2O_STAT_ITEM* pMsg = NULL;

    pMsg = (InternalMessage::G2O_STAT_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_STAT_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2o_stat_item;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;
    pMsg->nItemTemplateID = nTemplateID;
    pMsg->nItemCount = nItemCount;
    pMsg->nAction = nAction;

    nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_STAT_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDLRuneItem(UINT32 dwRoleID, INT32 nGroupID, INT32 nRoleLevel, INT32 nEquipTemplateID, INT32 nRuneItemID, INT32 nRuneID)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    InternalMessage::G2O_RUNE_ITEM* pMsg = NULL;

    pMsg = (InternalMessage::G2O_RUNE_ITEM*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_RUNE_ITEM));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2o_rune_item;

    pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nRoleLevel = nRoleLevel;
    pMsg->nEquipTemplateID = nEquipTemplateID;
    pMsg->nRuneItemID = nRuneItemID;
    pMsg->nRuneID = nRuneID;

    nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_RUNE_ITEM));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDLChangeItem(INT32 nGroupID, INT32 dwRoleID, INT32 nReason, const ItemInfo& item)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    InternalMessage::G2O_CHANGE_ITEM* pMsg = NULL;

    pMsg = (InternalMessage::G2O_CHANGE_ITEM*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2o_change_item;

    pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
    pMsg->nGroupID = nGroupID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nReason = nReason;
    pMsg->qwItemGuid = item.m_llGuid;
    pMsg->nItemTemplateID = item.m_nTemplateID;
    item.SaveString(pMsg->szItem, MAX_MESSAGE_SIZE - sizeof(InternalMessage::G2O_CHANGE_ITEM));

    nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_CHANGE_ITEM) + strlen(pMsg->szItem) + 1);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDLAddMoney(UINT32 dwSessionID, UINT32 dwSrcID, UINT32 dwTarID, INT32 nMoneyType, INT32 nMoneyCount, INT32 nReason, INT32 nParam)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_ADD_MONEY* pMsg = NULL;

	pMsg = (InternalMessage::G2O_ADD_MONEY*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_ADD_MONEY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_add_money;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);

	pMsg->dwSrcRoleID = dwSrcID;
	pMsg->dwTarRoleID = dwTarID;
	pMsg->nReason = nReason;
	pMsg->nParam = nParam;

	pMsg->nMoneyType = nMoneyType;
	pMsg->nMoneyCount = nMoneyCount;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_ADD_MONEY));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLSubMoney(UINT32 dwSessionID, UINT32 dwSrcID, UINT32 dwTarID, INT32 nMoneyType, INT32 nMoneyCount, INT32 nReason, INT32 nParam)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_SUB_MONEY* pMsg = NULL;

	pMsg = (InternalMessage::G2O_SUB_MONEY*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_SUB_MONEY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_sub_money;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwSrcRoleID = dwSrcID;
    pMsg->dwTarRoleID = dwTarID;
	pMsg->nReason = nReason;
	pMsg->nParam = nParam;

	pMsg->nMoneyType = nMoneyType;
	pMsg->nMoneyCount = nMoneyCount;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_SUB_MONEY));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLAcceptQuest(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_ACCEPT_QUEST* pMsg = NULL;
	UINT32 dwSessionID = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_ACCEPT_QUEST*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_ACCEPT_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_accept_quest;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwSrcID = poRole->GetObjID();
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_ACCEPT_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLCompleteQuest(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_COMPLETE_QUEST* pMsg = NULL;
	UINT32 dwSessionID = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_COMPLETE_QUEST*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_COMPLETE_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_complete_quest;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwSrcID = poRole->GetObjID();
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_COMPLETE_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLAbandonQuest(CRole* poRole, INT32 nQuestID)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_ABANDON_QUEST* pMsg = NULL;
	UINT32 dwSessionID = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_ABANDON_QUEST*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_ABANDON_QUEST));
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_abandon_quest;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwSrcID = poRole->GetObjID();
	pMsg->nQuestID = nQuestID;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_ABANDON_QUEST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLCompleteAchievement(CRole* poRole, INT32 nAchievementID)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_COMPLETE_ACHIEVEMENT* pMsg = NULL;
	UINT32 dwSessionID = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_COMPLETE_ACHIEVEMENT*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_COMPLETE_ACHIEVEMENT));
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_complete_achievement;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nAchievementID = nAchievementID;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_COMPLETE_ACHIEVEMENT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLGmCommand(CRole* poRole, const char* szCommand, BOOL bResult)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_GM_COMMAND* pMsg = NULL;
	UINT32 dwSessionID = 0;
	INT32 nSize = 0;
	INT32 nCommandSize = 0;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(szCommand);

	nCommandSize = strlen(szCommand) + 1;
	nSize = nCommandSize + sizeof(InternalMessage::G2O_GM_COMMAND);

	pMsg = (InternalMessage::G2O_GM_COMMAND*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_gm_command;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->bResult = bResult;
	strxcpy(pMsg->szName, poRole->GetName(), ROLE_NAME_LEN);
	strxcpy(pMsg->szCommand, szCommand, nCommandSize);

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, nSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLRolePosition(CRole* poRole)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_ROLE_POSITION* pMsg = NULL;
	UINT32 dwSessionID = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_ROLE_POSITION*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_ROLE_POSITION));
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	pMsg->wMsg = InternalMessage::g2o_role_position;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nSceneTemplateID= poRole->GetSceneTempID();
	pMsg->nSceneID = poRole->GetSceneID();
	pMsg->nX = poRole->GetX();
	pMsg->nY = poRole->GetY();
	pMsg->nZ = poRole->GetZ();

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_ROLE_POSITION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLKickRole(INT32 eKickType, const char* pcszUserName, UINT32 dwClientSessionID, const char* pcszRoleName, UINT32 dwRoleID, 
                  const char* pcszFileName, INT32 nLineNum, const char* pcszFuncName)
{

    return FALSE;
}

BOOL CCliMessageHandler::DoDLNpcDie(INT32 nServerGroupID, UINT32 dwNpcID, BOOL bDieIDMonitor)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_NPC_DIE* pMsg = NULL;

	pMsg = (InternalMessage::G2O_NPC_DIE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_NPC_DIE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_npc_die;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = nServerGroupID;
	pMsg->dwNpcTemplateID = dwNpcID;
	pMsg->bDieIDMonitor = bDieIDMonitor;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_NPC_DIE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLNpcMoney(INT32 nServerGroupID, UINT32 dwNpcID, BOOL bMoneyStat, INT32 nMoneyCount)
{
    INT32 nRetCode = 0;
    InternalMessage::G2O_NPC_MONEY* pMsg = NULL;

    pMsg = (InternalMessage::G2O_NPC_MONEY*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_NPC_MONEY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2o_npc_money;

    pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
    pMsg->nGroupID = nServerGroupID;
    pMsg->dwNpcTemplateID = dwNpcID;
    pMsg->bMoneyStat = bMoneyStat;
    pMsg->nMoneyCount = nMoneyCount;

    nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_NPC_MONEY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDLConsecutiveKillNpcCount(UINT32 dwSessionID, INT32 nConsecutiveKillNpcCount)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_CONSECUTIVE_KILL_NPC_COUNT* pMsg = NULL;

	pMsg = (InternalMessage::G2O_CONSECUTIVE_KILL_NPC_COUNT*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_CONSECUTIVE_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(pMsg);


	pMsg->wMsg = InternalMessage::g2o_consecutive_kill_npc_count;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->nConsecutiveKillNpcCount = nConsecutiveKillNpcCount;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_CONSECUTIVE_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;	
}

BOOL CCliMessageHandler::DoDLMomentaryKillNpcCount(UINT32 dwSessionID, INT32 nMomentaryKillNpcCount)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_MOMENTARY_KILL_NPC_COUNT* pMsg = NULL;

	pMsg = (InternalMessage::G2O_MOMENTARY_KILL_NPC_COUNT*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_MOMENTARY_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(pMsg);


	pMsg->wMsg = InternalMessage::g2o_momentary_kill_npc_count;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->nMomentaryKillNpcCount = nMomentaryKillNpcCount;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_MOMENTARY_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;	
}

BOOL CCliMessageHandler::DoDLHardwareInfo(UINT32 dwSessionID, UINT32 dwRoleID, void* pData)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_CLIENT_HARDWARE_INFO* pMsg = NULL;
	C2G_CLIENT_HARDWARE_INFO* pClientHardwareInfo = NULL;

	pMsg = (InternalMessage::G2O_CLIENT_HARDWARE_INFO*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_CLIENT_HARDWARE_INFO));
	LOG_PROCESS_ERROR(pMsg);
	LOG_PROCESS_ERROR(pData);

	pClientHardwareInfo = (C2G_CLIENT_HARDWARE_INFO*)pData;
	LOG_PROCESS_ERROR(pClientHardwareInfo);

	pMsg->wMsg = InternalMessage::g2o_client_hardware_info;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);
	pMsg->dwRoleID = dwRoleID;

	memcpy(&pMsg->stcpuinfo, &pClientHardwareInfo->stcpuinfo, sizeof(pMsg->stcpuinfo));
	memcpy(&pMsg->stmemoryinfo, &pClientHardwareInfo->stmemoryinfo, sizeof(pMsg->stmemoryinfo));
	memcpy(&pMsg->stdiskinfo, &pClientHardwareInfo->stdiskinfo, sizeof(pMsg->stdiskinfo));
	memcpy(&pMsg->stvideoinfo, &pClientHardwareInfo->stvideoinfo, sizeof(pMsg->stvideoinfo));
	memcpy(&pMsg->stnetworkadapterinfo, &pClientHardwareInfo->stnetworkadapterinfo, sizeof(pMsg->stnetworkadapterinfo));

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_CLIENT_HARDWARE_INFO));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}
BOOL CCliMessageHandler::DoDLFpsInfo(UINT32 dwSessionID, UINT32 dwRoleID, UINT8 byGameFps, UINT8 byCpuFps, UINT8 byGpuFps, UINT16 wGateDelay, UINT16 wGameDelay)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_CLIENT_FPS_INFO* pMsg = NULL;

	pMsg = (InternalMessage::G2O_CLIENT_FPS_INFO*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_CLIENT_FPS_INFO));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2o_client_fps_info;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);

	pMsg->dwRoleID = dwRoleID;
	pMsg->byGameFps = byGameFps;
	pMsg->byCpuFps	= byCpuFps;
	pMsg->byGpuFps = byGpuFps;
	pMsg->wGateDelay = wGateDelay;
	pMsg->wGameDelay = wGameDelay;

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_CLIENT_FPS_INFO));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDLAntiCheatInfo(CRole* poRole, INT32 nAction, INT32 nAnswerIdx, INT32 nErrCode, JigsawImage* poImg, JigsawAnswer* poAnswer)
{
	INT32 nRetCode = 0;
	InternalMessage::G2O_ROLE_ANTI_CHEAT_INFO* pMsg = NULL;
	UINT32 dwSessionID = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (InternalMessage::G2O_ROLE_ANTI_CHEAT_INFO*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2O_ROLE_ANTI_CHEAT_INFO));
	LOG_PROCESS_ERROR(pMsg);

	dwSessionID = poRole->GetSessionID();

	memset(pMsg, 0, sizeof(InternalMessage::G2O_ROLE_ANTI_CHEAT_INFO));

	pMsg->wMsg = InternalMessage::g2o_role_anti_cheat_info;

	pMsg->dwTime = CGameWorld::GetInstance().GetGameTime();
	pMsg->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(dwSessionID);

	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->nSceneTemplateID = poRole->GetSceneTempID();
	pMsg->dwJigsawSn = poRole->GetJigsawSn();
	pMsg->byAction = nAction;
	pMsg->nErrCode = nErrCode;
	pMsg->nRoleAnswer = nAnswerIdx;

	if (poImg != NULL)
	{
		pMsg->nImageIndex = poImg->nImageIndex;
	}

	if (poAnswer != NULL)
	{
		pMsg->nRealAnswer = poAnswer->nAnswerIdx;
		pMsg->nRealAnswerBlockX = poAnswer->nBlockX;
		pMsg->nRealAnswerBlockY = poAnswer->nBlockY;
	}

	nRetCode = m_poNotifier->SendToDL((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2O_ROLE_ANTI_CHEAT_INFO));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

#include "profile.h"
BOOL CCliMessageHandler::OnRecv(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pData;

	_mm_prefetch((const char*)(m_MessageInfo + pHeader->wMsg), _MM_HINT_T0);

	LOG_PROCESS_ERROR(pHeader->wMsg > c2g_message_begin && pHeader->wMsg < c2g_message_end);
	LOG_PROCESS_ERROR(m_MessageInfo[pHeader->wMsg].pFunc);

	if (m_MessageInfo[pHeader->wMsg].bPlayingMsg)
	{
		if (poRole->GetState() == rsPlaying && !poRole->GetAIProcessor())
		{
			(this->*m_MessageInfo[pHeader->wMsg].pFunc)(poRole, pData);
		}
	}
	else
	{
		(this->*m_MessageInfo[pHeader->wMsg].pFunc)(poRole, pData);
	}

#ifndef _FINAL
	if (m_MessageInfo[pHeader->wMsg].bPrint)
	{
		DBG("recv %d %s", pHeader->wMsg, g_szC2SMessageName[pHeader->wMsg]);
	}
#endif	// _FINAL

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCliMessageHandler::RealKickUser(UINT32 dwRoleID)
{
	INT32 nRetCode = 0;

	CRole* pRole = CGameWorld::GetInstance().FindRole(dwRoleID);
	LOG_PROCESS_ERROR(pRole);

	INF("RealKickUser processing role %d<%X>", dwRoleID,pRole->GetSessionID());

	LOG_PROCESS_ERROR(m_poNotifier);
	nRetCode = m_poNotifier->_KickUser(pRole->GetSessionID());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	INF("RealKickUser processing role %d failed!", dwRoleID);
	return FALSE;
}

void CCliMessageHandler::OnLoginOut(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_LOGOUT* pMsg = (C2G_LOGOUT*)pData;
	const GRID_DATA* pcGrid = NULL;
	UINT32 dwDelayTime = ROLE_DELAY_LOGOUT_FRAME;

	LOG_PROCESS_ERROR(poRole);

	pcGrid = poRole->GetGrid();
	LOG_PROCESS_ERROR(pcGrid);

	if(pcGrid->uPkType != gptSafe)
		dwDelayTime = ROLE_DELAY_LOGOUT_FRAME_ABNORMAL;

	nRetCode = poRole->Logout(rltReturnLogin, dwDelayTime);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CCliMessageHandler::OnLoginOutCancel(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0; 
	C2G_LOGOUT_CANCEL* pMsg = (C2G_LOGOUT_CANCEL*)pData;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetState() == rsLogout);
	PROCESS_ERROR(poRole->GetLogoutType() == rltReturnLogin);
	
	poRole->ClearLogoutFrame();
	poRole->SetState(rsPlaying);

	nRetCode = DoClientLogoutTimer(poRole, FALSE, 0);
	LOG_PROCESS_ERROR(nRetCode);
	
Exit0:
	return;
}

void CCliMessageHandler::OnSubscribeVIP(CRole* poRole, const char* pData)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;

    C2G_SUBSCRIBE_VIP* pMsg = (C2G_SUBSCRIBE_VIP*)pData;

    nRetCode = poRole->SubscribeVIP(pMsg->byVIPLevel, pMsg->byMonth);
    PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);

    return;
}

void CCliMessageHandler::OnHardwareInfo(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_CLIENT_HARDWARE_INFO* pMsg = (C2G_CLIENT_HARDWARE_INFO*)pData;

	nRetCode = CCliMessageHandler::GetInstance().DoDLHardwareInfo(
		poRole->GetSessionID(), poRole->GetObjID(), (void*)pData);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	return;
}

void CCliMessageHandler::OnClientFpsInfo(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_CLIENT_FPS_INFO* pMsg = (C2G_CLIENT_FPS_INFO*)pData;

	nRetCode = CCliMessageHandler::GetInstance().DoDLFpsInfo(poRole->GetSessionID(), poRole->GetObjID(), 
		pMsg->byGameFps, pMsg->byCpuFps, pMsg->byGpuFps, pMsg->wGateDelay, pMsg->wGameDelay);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	return;
}

void CCliMessageHandler::OnQuerySurveyPath(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_QUERY_SURVEY_PATH* pMsg = (C2G_QUERY_SURVEY_PATH*)pData;

	nRetCode = CCliMessageHandler::GetInstance().DoQuerySurveyPath(poRole);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	return;
}

void CCliMessageHandler::OnReceiveReputationAward(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_RECEIVE_REPUTATION_AWARD* pMsg = (C2G_RECEIVE_REPUTATION_AWARD*)pData;

	nRetCode = poRole->GetReputation().ReceiveAward(pMsg->byForceID);
	LOG_CHECK_ERROR(nRetCode == errRepReceiveAwardSuccess);

Exit0:
	poRole->ErrorCodeNotify(nRetCode, pMsg->byForceID, 0);

	return;
}

void CCliMessageHandler::OnPetChangeState(CRole *poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    UINT32      dwNpcID = 0;
    INT32       nNewPetState = 0;
    CNpc        *poNpc = NULL;
    C2G_CHANGE_PET_STATE *pMsg = (C2G_CHANGE_PET_STATE*)pData;

    if (pMsg->byState == psInvalid)
    {
        LOG_PROCESS_ERROR(poRole->GetPetState() != psInvalid);
        nNewPetState = (poRole->GetPetState() == psIdle ? psFight : psIdle);
        poRole->SetPetState(nNewPetState);
    }
    else
    {
        PROCESS_ERROR_RET_CODE(pMsg->byState != poRole->GetPetState(), errPetStateSame);
        LOG_PROCESS_ERROR(pMsg->byState > psInvalid && pMsg->byState < psTotal);
        poRole->SetPetState(pMsg->byState);
    }

    PROCESS_SUCCESS(0 == poRole->GetPetCount());

    for (INT32 nIndex = 0; nIndex < poRole->GetPetCount(); ++nIndex)
    {
        dwNpcID = poRole->GetPetNpcID(nIndex);
        poNpc = CGameWorld::GetInstance().FindNpc(dwNpcID);
        LOG_PROCESS_ERROR(poNpc);

        nRetCode = poNpc->ChangePetAI(poRole->GetPetState());
        LOG_CHECK_ERROR(nRetCode);
    }

	poRole->ErrorCodeNotify(errPetChangeStateSuccess, poRole->GetPetState(), 0);
    return;
Exit1:
    return;
Exit0:
	poRole->ErrorCodeNotify(nRetCode, nResult, 0);
    return;
}

BOOL CCliMessageHandler::DoMSCallLuaFunction(const char* pcszScriptName, const char* pcszFuncName, INT32 nValue1, INT32 nValue2, INT32 nValue3)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_CALL_LUA_FUNCTION* pMsg = 
		(InternalMessage::G2M_CALL_LUA_FUNCTION*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_CALL_LUA_FUNCTION));

	LOG_PROCESS_ERROR(pcszScriptName);
	LOG_PROCESS_ERROR(pcszFuncName);

	pMsg->wMsg = InternalMessage::g2m_call_lua_function;
	strxcpy(pMsg->chFileName, pcszScriptName, PATH_LEN);
	strxcpy(pMsg->chFuncName, pcszFuncName, fUNC_NAME_LEN);

	pMsg->nValue1 = nValue1;
	pMsg->nValue2 = nValue2;
	pMsg->nValue3 = nValue3;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_CALL_LUA_FUNCTION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSSendSystemMail(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_SEND_SYSTEM_MAIL* pMsg = (InternalMessage::G2M_SEND_SYSTEM_MAIL*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_SEND_SYSTEM_MAIL));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_send_system_mail;
	pMsg->dwReceiverRoleID = dwRoleID;
	strxcpy(pMsg->szMailSubject, szMailSubject, MAIL_SUBJECT_LEN);
	strxcpy(pMsg->szMailContent, szMailContent, MAIL_CONTENT_LEN);

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_SEND_SYSTEM_MAIL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSSendSystemMailWithSimpleAttach(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent, INT32 nItemID, INT32 nItemCount)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_SIMPLE_ATTACH* pMsg = (InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_SIMPLE_ATTACH*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_SIMPLE_ATTACH));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_send_system_mail_with_simple_attach;
	pMsg->dwReceiverRoleID = dwRoleID;
	strxcpy(pMsg->szMailSubject, szMailSubject, MAIL_SUBJECT_LEN);
	strxcpy(pMsg->szMailContent, szMailContent, MAIL_CONTENT_LEN);
	pMsg->nItemID = nItemID;
	pMsg->nItemCount = nItemCount;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_SIMPLE_ATTACH));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSSendSystemMailWithBufferAttach(UINT32 dwRoleID, const char* szMailSubject, const char* szMailContent, const char* pItemBuffer, UINT8 byBufferLen)
{
    INT32 nRetCode = 0;
    InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_BUFFER_ATTACH* pMsg = (InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_BUFFER_ATTACH*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_BUFFER_ATTACH) + byBufferLen);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_send_system_mail_with_buffer_attach;
    pMsg->dwReceiverRoleID = dwRoleID;
    strxcpy(pMsg->szMailSubject, szMailSubject, MAIL_SUBJECT_LEN);
    strxcpy(pMsg->szMailContent, szMailContent, MAIL_CONTENT_LEN);
    pMsg->byBufferLen = byBufferLen;
    memcpy(pMsg->ItemBuffer, pItemBuffer, byBufferLen);

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_SEND_SYSTEM_MAIL_WITH_BUFFER_ATTACH) + byBufferLen);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncCombatStat(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_COMBAT_STAT* pMsg = NULL;
	COMBAT_STAT_NODE* pStatNode = NULL;
    CBaseList& rCombatStatList = poRole->GetScene()->GetCombatStatList();
    COMBAT_STAT_NODE* pNode = NULL;
    INT32 nRoleCount = 0;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(poRole->GetScene());

	pMsg = (G2C_SYNC_COMBAT_STAT*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_combat_stat;
    pMsg->wSize = sizeof(G2C_SYNC_COMBAT_STAT);

    pNode = (COMBAT_STAT_NODE*)rCombatStatList.Head().pNext;
    while (pNode != &rCombatStatList.Rear())
    {
        pMsg->DataList[nRoleCount].dwRoleID = pNode->dwRoleID;
        pMsg->DataList[nRoleCount].byFaction = pNode->byFaction;
        memcpy(pMsg->DataList[nRoleCount].Data, pNode->m_Data, sizeof(pNode->m_Data));

        pMsg->wSize += sizeof(G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA);
        nRoleCount++;
        pNode = (COMBAT_STAT_NODE*)pNode->pNext;
    }

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMSCloseScene(UINT8 byServerGroup, INT32 nSceneTempID, INT32 nSceneID)
{
	INT32 nRetCode = 0;
	InternalMessage::G2M_CLOSE_SCENE *pMsg = (InternalMessage::G2M_CLOSE_SCENE*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_CLOSE_SCENE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = InternalMessage::g2m_close_scene;
	pMsg->byServerGroup = byServerGroup;
	pMsg->nSceneTempID = nSceneTempID;
	pMsg->nSceneID = nSceneID;

	nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_CLOSE_SCENE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

void CCliMessageHandler::OnGpkAuthData(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_GPK_AUTH_DATA* pMsg = (C2G_GPK_AUTH_DATA*)pData;
	const unsigned char* p = NULL;

	SGPK::IGPKCSAuth* poGpk = poRole->GetGpkAuth();
	LOG_PROCESS_ERROR(poGpk);

	nRetCode = poGpk->CheckAuthReply(&p, (unsigned char*)pMsg->AuthData, pMsg->wSize - sizeof(FLEX_MESSAGE_HEADER));
	LOG_PROCESS_ERROR(nRetCode == CHECK_SUCCEED);

	return;
Exit0:
	GS_SYS_LOG_ERROR(poRole);
    KICK_ROLE(poRole, ktLogged);

	return;
}

BOOL CCliMessageHandler::DoGpkDynCode(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_GPK_DYN_CODE* pMsg = NULL;
	SGPK::IGPKCSAuth* poGpkAuth = NULL;
	INT32 nCodeLen = 0;
	const unsigned char* p = NULL;

	LOG_PROCESS_ERROR(poRole);

	poGpkAuth = poRole->GetGpkAuth();
	LOG_PROCESS_ERROR(poGpkAuth);

	pMsg = (G2C_GPK_DYN_CODE*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_gpk_dyn_code;
	pMsg->wSize = sizeof(G2C_GPK_DYN_CODE);

	nCodeLen = poGpkAuth->GetCSAuthDynCode(&p);
	LOG_PROCESS_ERROR(nCodeLen > 0);

	memcpy(pMsg->DynCode, p, nCodeLen);
	pMsg->wSize += nCodeLen;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoGpkAuthData(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_GPK_AUTH_DATA* pMsg = NULL;
	SGPK::IGPKCSAuth* poGpkAuth = NULL;
	INT32 nCodeLen = 0;
	const unsigned char* p = NULL;

	LOG_PROCESS_ERROR(poRole);

	poGpkAuth = poRole->GetGpkAuth();
	LOG_PROCESS_ERROR(poGpkAuth);

	pMsg = (G2C_GPK_AUTH_DATA*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_gpk_auth_data;
	pMsg->wSize = sizeof(G2C_GPK_AUTH_DATA);

	nCodeLen = poGpkAuth->GetAuthData(&p, NULL, NULL);
	LOG_PROCESS_ERROR(nCodeLen > 0);

	memcpy(pMsg->AuthData, p, nCodeLen);
	pMsg->wSize += nCodeLen;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncDataError(CRole* poRole)
{
    INT32 nRetCode = 0;
    G2C_SYNC_DATA_ERROR* pMsg = NULL;
    extern char g_szErrorMsg[];
    extern INT32 g_nErrorMsgSize;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_DATA_ERROR*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(g_nErrorMsgSize < MAX_MESSAGE_SIZE);

    pMsg->wMsg = g2c_sync_data_error;
    pMsg->wSize = sizeof(G2C_SYNC_DATA_ERROR) + g_nErrorMsgSize;
    strxcpy(pMsg->szErrorMsg, g_szErrorMsg, g_nErrorMsgSize);

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoDrawDebugLine(CSceneObject* poObj, INT32 nType, INT32 nSrcX, INT32 nSrcY, INT32 nSrcZ, INT32 nDstX, INT32 nDstY, INT32 nDstZ)
{
	INT32 nRetCode = 0;
	G2C_DRAW_DEBUG_LINE* pMsg = NULL;

	LOG_PROCESS_ERROR(poObj);

	pMsg = (G2C_DRAW_DEBUG_LINE*)m_poPacketAllocator->Alloc(sizeof(G2C_DRAW_DEBUG_LINE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_draw_debug_line;
	pMsg->byType = nType;

	pMsg->wSrcX = ConvertCoordS2N(nSrcX);
	pMsg->wSrcY = ConvertCoordS2N(nSrcY);
	pMsg->wSrcZ = ConvertCoordS2N(nSrcZ);

	pMsg->wDstX = ConvertCoordS2N(nDstX);
	pMsg->wDstY = ConvertCoordS2N(nDstY);
	pMsg->wDstZ = ConvertCoordS2N(nDstZ);

	nRetCode = _Broadcast9Region(poObj->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_DRAW_DEBUG_LINE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoUpdateConsecutiveKillNpcCount(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_consecutive_kill_npc_count;
	pMsg->byCount = poRole->GetConsecutiveKillNpcCount();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateConsecutiveKillNpcEnd(CRole* poRole, INT32 nExp)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_CONSECUTIVE_KILL_NPC_END* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_CONSECUTIVE_KILL_NPC_END*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_CONSECUTIVE_KILL_NPC_END));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_consecutive_kill_npc_end;
	pMsg->byCount = poRole->GetConsecutiveKillNpcCount();
	pMsg->nExp = nExp;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_UPDATE_CONSECUTIVE_KILL_NPC_END));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateMomentaryKillNpcEnd(CRole* poRole, INT32 nCount, INT32 nExp)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_MOMENTARY_KILL_NPC_END* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_MOMENTARY_KILL_NPC_END*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_MOMENTARY_KILL_NPC_END));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_momentary_kill_npc_end;
	pMsg->byCount = nCount;
	pMsg->nExp = nExp;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_UPDATE_MOMENTARY_KILL_NPC_END));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoUpdateConsecutiveKillPlayerCount(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT*)m_poPacketAllocator->Alloc(sizeof(G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_update_consecutive_kill_player_count;
	pMsg->byCount = poRole->GetConsecutiveKillPlayerCount();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoQuerySurveyPath(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_QUERY_SURVEY_PATH* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_QUERY_SURVEY_PATH*)m_poPacketAllocator->Alloc(sizeof(G2C_QUERY_SURVEY_PATH));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_query_survey_path;

	memset(pMsg->szQuerySurveyPath, 0, sizeof(pMsg->szQuerySurveyPath));

	for (INT32 nSurveyIndex = 0; nSurveyIndex < MAX_SURVEY_COUNT; nSurveyIndex++)
	{
		if (g_GWConfig.Survey[nSurveyIndex].nMaxLevel <= poRole->GetLevel())
		{
			strxcpy(pMsg->szQuerySurveyPath[nSurveyIndex], g_GWConfig.Survey[nSurveyIndex].szSurveyPath, MAX_SURVEY_PATH_LEN);
		}
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_QUERY_SURVEY_PATH));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncMsgPaoPao(CRole *poRole, INT32 nType, UINT32 dwParam1)
{
    INT32 nRetCode = 0;
    G2C_SYNC_MSG_PAOPAO* pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_MSG_PAOPAO*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_MSG_PAOPAO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_msg_paopao;
    pMsg->wType = nType;
    pMsg->dwParam1 = dwParam1;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_MSG_PAOPAO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);

    return FALSE;
}

BOOL CCliMessageHandler::DoSyncPetLevel(CNpc* poNpc, UINT32 dwNpcID, UINT8 byLevel)
{
    INT32 nRetCode = 0;
    G2C_SYNC_PET_LEVEL* pMsg = NULL;

    LOG_PROCESS_ERROR(poNpc);

    pMsg = (G2C_SYNC_PET_LEVEL*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_PET_LEVEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_pet_level;
    pMsg->dwNpcID = dwNpcID;
    pMsg->byLevel = byLevel;

    nRetCode = _Broadcast9Region(poNpc->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_PET_LEVEL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoSyncNpcTransformState(CNpc *poNpc, INT32 nState)
{
    INT32 nRetCode = 0;
    G2C_SYNC_NPC_TRANSFORM_STATE* pMsg = NULL;

    LOG_PROCESS_ERROR(poNpc);

    pMsg = (G2C_SYNC_NPC_TRANSFORM_STATE*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_NPC_TRANSFORM_STATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_npc_transform_state;
    pMsg->dwNpcID = poNpc->GetObjID();
    pMsg->byTransformState = nState;

    nRetCode = _Broadcast9Region(poNpc->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_NPC_TRANSFORM_STATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

void CCliMessageHandler::OnDiceBet(CRole* poRole, const char* pData)
{
}

void CCliMessageHandler::OnLoversTeleprot(CRole* poRole, const char* pData)
{
}



#endif  // _ROBOT
