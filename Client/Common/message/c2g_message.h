#ifndef _C2G_MESSAGE_H_
#define _C2G_MESSAGE_H_

#include "external_message_header.h"

#pragma pack(push, 1)
// tolua_begin
namespace ExternalMessage
{
	enum C2G_MESSAGE_ID
	{
		c2g_message_begin = 8 + 1,
   
		// login
		c2g_return_select_role,
        c2g_leave_scene,
		c2g_loadfinish,
		c2g_gpk_auth_data,

		// move
		c2g_update_pos,
        c2g_role_stop,
		c2g_role_rotate,
		c2g_role_run,
		c2g_role_walk,
		c2g_query_char_data,
		c2g_query_role_detail_data,
		c2g_query_role_pow,
		c2g_teleport,
        c2g_ping,
		c2g_revive,
		c2g_cancel_cutscene,
		c2g_role_teleport,          // 可能是传送戒指触发
		c2g_query_role_location,
		c2g_recall_team_member,
		c2g_role_mine,

		// spell
		c2g_select_focus,
		c2g_directly_cast_spell,
		c2g_cast_switch_spell,
		c2g_cast_spell_ex,
		c2g_role_cancel_cast,
		c2g_learn_spell,
		c2g_role_adjust_target_point,
		c2g_change_pk_mode,              // 切换攻击模式
		c2g_equip_spell,
		c2g_combat_pose_switch,
		c2g_cancel_aura,
		c2g_spell_log,
		c2g_clear_spell_cache,

		// item
		c2g_move_item,	  // 43, 移动物品
		c2g_use_item,	  // 44, 使用物品
		c2g_split_item,	  // 45, 拆解物品
		c2g_delete_item,  // 46, 丢弃物品
		c2g_pickup_item,  // 47, 拾取物品
		c2g_buy_item,
		c2g_sell_item,
		c2g_view_sold_items,
		c2g_buy_back_item,
		c2g_repair_item,				// 修理物品
        c2g_repair_all_equipped_item,   // 商店修理全身
        c2g_special_repair_item,        // 特殊修理
		c2g_refine,						// obseleted
		c2g_buy_container_slot,
        c2g_organize_bag,
		c2g_identify_item,				// obsoleted
		c2g_randomize_item,				// obsoleted
		c2g_confirm_randomize_item,		// obsoleted
		c2g_sign_item,					// obsoleted
		c2g_verify_item,				// obsoleted
		c2g_use_moneytype01_only,		// obsoleted
        c2g_check_npc_shop_data,

		// stall
		c2g_open_stall,         // NotOpen -> Preparing
		c2g_close_stall,        // Open -> Preparing
		c2g_real_open_stall,    // Preparing -> Open
		c2g_real_close_stall,   // Preparing/Open -> NotOpen
		c2g_add_stall_item,
		c2g_remove_stall_item,
		c2g_change_stall_name,
        c2g_upgrade_stall,
		c2g_view_stall_item,
		c2g_buy_stall_item,

		// quest
		c2g_accept_quest,
		c2g_complete_quest,
		c2g_abandon_quest,
		c2g_operate_doodad,
		c2g_interact_with_npc,
        c2g_stop_interact_with_npc,
		c2g_select_dialog_option,
		c2g_share_quest,
		c2g_confirm_publish_quest,			// obsoleted
		c2g_talk_to_npc,
		c2g_explore_zone,
		c2g_fly_to_quest_zone,				// obsoleted
		c2g_generate_circle_quest,			// obsoleted
		c2g_buy_daily_quest_complete_time,	// obsoleted
		c2g_daily_quest_complete_shot_cut,	// obsoleted
		c2g_brush_daily_quest,				// obsoleted
		c2g_quest_recycle,					// obsoleted

		c2g_complete_kill_npc_quest,

		// chat
		c2g_chat_message,

		// dungeon
		c2g_select_scene_difficulty,		// obsoleted
		c2g_enter_dungeon,

		// line
		c2g_change_line,

		// friend
		c2g_friend_change_group_name,

		// misc
		c2g_update_client_variable,
		c2g_query_combat_stat,				// obsoleted

		c2g_adjacent_role_noteam,
		c2g_adjacent_team,

		// team auction
		c2g_team_auction_bid,
		c2g_team_auction_abandon,

		// dungeon
		c2g_dungeon_select_reward,			// obsoleted
		c2g_dungeon_replay,					// obsoleted

		// title
		c2g_title_equip,
		c2g_title_unequip,

		// trade
		c2g_trade_ask,
		c2g_trade_accept,
		c2g_trade_set_money,
		c2g_trade_set_item,
		c2g_trade_lock,
		c2g_trade_confirm,
		c2g_trade_unlock,
		c2g_trade_cancel,

		// achievement
		c2g_complete_achievement,
		c2g_compare_achievement,
		c2g_reward_achievement,

        // master
        c2g_master_twitter,
		c2g_receive_master_reward_exp,
		c2g_query_master_reward_exp,

		// guild
		c2g_guild_create,
		c2g_guild_draw_salary,
		c2g_guild_contribute_fund,

        c2g_guild_view_storage,
        c2g_guild_push_item,
        c2g_guild_pop_item,
        c2g_guild_create_building,
        c2g_guild_upgrade_building,
        c2g_guild_del_building,
        c2g_guild_buy_aura,				// obsoleted
        c2g_guild_change_weather,		// obsoleted
        c2g_guild_enter_map,

        c2g_guild_open_dungeon,			// obsoleted
        c2g_guild_enter_dungeon,		// obsoleted

        c2g_guild_reward_achievement,

        // arena
		c2g_arena_join_season,			// obsoleted
		c2g_arena_quit_season,			// obsoleted
		c2g_arena_match,				// obsoleted
		c2g_arena_get_reward,			// obsoleted

		// spirit
		c2g_spirit_exp_change,			// obsoleted

		c2g_spirit_add_air,				// obsoleted
		c2g_spirit_merge_air,			// obsoleted
		c2g_spirit_move_air,			// obsoleted
		c2g_spirit_buy_air_slot,		// obsoleted
		c2g_spirit_exchange_air,		// obsoleted

		c2g_spirit_brush_slot,			// obsoleted
		c2g_spirit_blood_open,			// obsoleted

		// teleport
		c2g_teleport_by_platform,		// obsoleted

		// mount
		c2g_on_mount,					// obsoleted
		c2g_off_mount,					// obsoleted

		// hero
		c2g_hire_hero,					// obsoleted
		c2g_fire_hero,					// obsoleted
		c2g_query_hero_attr_list,		// obsoleted

		c2g_summon_hero,				// obsoleted
		c2g_recall_hero,				// obsoleted

		c2g_change_hero_ai,				// obsoleted
		c2g_change_hero_name,			// obsoleted

        // shield list
        c2g_unshield,

        // 融灵
        c2g_upgrade_soul,    // 
        c2g_add_soul,		 // 镶嵌宝石
        c2g_remove_soul,	 // 拆除宝石

        // 铭文
        c2g_rune_item,       // 普通铭文洗练

        // 装备升级
        c2g_weapon_level_up_put_in,
        c2g_weapon_level_up_take_out,

        // 装备分解合成
        c2g_decompose,
        c2g_compose,

		// activity
		c2g_activity_reward,			// obsoleted

		//logout
		c2g_logout,
		c2g_logout_cancel,

        // vip
        c2g_subscribe_vip,				// obsoleted

		// client info
		c2g_client_hardware_info,
		c2g_client_fps_info,

		// survey
		c2g_query_survey_path,

		// reputation award
		c2g_receive_reputation_award,

        // pet
        c2g_change_pet_state,

		// checkin
		c2g_checkin,

        // black market
        c2g_black_market_limit_info,
        c2g_black_market_cooldown_info,
        c2g_black_market_buy,

        // gamble
        c2g_gamble,    // 已经废弃

        // exercise
        c2g_exercise_info,
        c2g_add_exercise_time,
        c2g_get_exercise_exp,

		// anti-cheat
		c2g_jigsaw_answer,    // 回答挂机问题

        // 升级武器立即取回
        c2g_weapon_level_up_take_out_immediately,

        // 技能书重著
        c2g_spell_book_gamble,    // 187, 技能书重著

        c2g_arena_reward,

        c2g_leave_guild,			   // 189, 退出公会
        c2g_query_guild_war_record,    // 190, 查询公会战记录

        c2g_rune_item_advance,      	// 铭文高级洗练
        c2g_rune_item_advance_confirm,  // 确认高级洗练

        c2g_potion_gamble,
        c2g_gamble_advance,   	  // 194, 装备重著

        c2g_sell_resource_item,   // 一键出售资源包物品?

		c2g_dice_bet,			// 取消投点?
		c2g_lovers_teleprot,    // 夫妻传送?

		c2g_wing_op,            // wing panel request (1.05 UnrealWing)

		c2g_message_end = 512,
	};

	struct C2G_GPK_AUTH_DATA : FLEX_MESSAGE_HEADER
	{
		char			AuthData[0];
	};

    struct C2G_ROLE_STOP : MESSAGE_HEADER
    {

    };

	struct C2G_UPDATE_POS : MESSAGE_HEADER
	{
        UINT16              wDelay;
		UINT16              wCurrentX;
		UINT16              wCurrentY;
		UINT16              wCurrentZ;
	};

	struct C2G_ROLE_ROTATE : MESSAGE_HEADER
	{
		UINT16           wFaceDirection;
		INT32			 nDuration;
	};

	struct C2G_ROLE_WALK : MESSAGE_HEADER
	{
		UINT16            wDestX;
		UINT16            wDestY;
	};

	struct C2G_ROLE_RUN : MESSAGE_HEADER
	{
		UINT16            wDestY;
		UINT16            wDestX;
	};

	struct C2G_ROLE_ADJUST_TARGET_POINT : MESSAGE_HEADER
	{
		UINT16            wTargetX;
		UINT16            wTargetY;
		UINT16            wTargetZ;
	};

	struct C2G_CHAT_MESSAGE : FLEX_MESSAGE_HEADER
	{
		UINT32			dwTargetID;
		UINT8			byLocation;
		char			szMessage[0];
	};

	struct C2G_SELECT_SCENE_DIFFICULTY : MESSAGE_HEADER
	{
		UINT8            bySelectedDifficulty;
	};

	struct C2G_ENTER_DUNGEON : MESSAGE_HEADER
	{
		UINT8            bySelectedDifficulty;
	};

	struct C2G_CHANGE_LINE : MESSAGE_HEADER
	{
		UINT8			 byLineIndex;
	};

	struct C2G_LEARN_SPELL : MESSAGE_HEADER
	{
		UINT16              wSpellID;
		UINT8               bySpellLevel;
	};

	struct C2G_CAST_SPELL_EX: MESSAGE_HEADER
	{
		UINT16				wSpellID;
		UINT8				bySerial;
		UINT8				byRotateToTarget;
		UINT32			    dwTargetID;
		UINT16			    wTargetX;
		UINT16			    wTargetY;
		UINT16			    wTargetZ;
	};

	struct C2G_DIRECTLY_CAST_SPELL : MESSAGE_HEADER
	{
		UINT16				dwSpellID;
		UINT8				bySpellLevel;
		UINT32			    dwTargetID;
		UINT16			    wTargetX;
		UINT16			    wTargetY;
		UINT16			    wTargetZ;
	};

	struct C2G_CAST_SWITCH_SPELL: MESSAGE_HEADER
	{
		UINT16				wSpellID;
	};

	struct C2G_ROLE_CANCEL_CAST : MESSAGE_HEADER
	{
		UINT16				wSpellID;
	};

	struct C2G_EQUIP_SPELL: MESSAGE_HEADER
	{
		UINT8			bySlot;
		UINT16			wSpellID;
	};

	struct C2G_COMBAT_POSE_SWITCH : MESSAGE_HEADER
	{
		UINT8			byCombatPoseOn;
		UINT8			byClientFlag;
	};

	struct C2G_CANCEL_AURA : MESSAGE_HEADER
	{
		UINT32			dwInstanceID;
	};

	struct C2G_SPELL_LOG : MESSAGE_HEADER
	{
		UINT32			dwTime;
		UINT16			wSpellID;
		INT32			nCurrentMp;
		INT32			nCdRemain;
		UINT32			dwTargetID;
		UINT16		    wTargetX;
		UINT16		    wTargetY;
		UINT16		    wTargetZ;
		UINT8			byNotEnoughMp;
		UINT8			byInCd;
		UINT8			byIsShiftPressed;
	};

	struct C2G_CLEAR_SPELL_CACHE : MESSAGE_HEADER
	{
		INT32			nTag;
	};

	struct C2G_CHANGE_PK_MODE : MESSAGE_HEADER
	{
		UINT8 byMode;
	};

	struct C2G_QUERY_CHAR_DATA : MESSAGE_HEADER
	{
		UINT32           dwCharID;
		UINT32           dwCRC;
	};

	struct C2G_QUERY_ROLE_DETAIL_DATA : MESSAGE_HEADER
	{
		UINT32          dwRoleID;
		UINT32          dwAttrCRC;
		UINT32          dwEquipCRC;
        UINT32          dwSpiritCRC;
        UINT32          dwSpellCRC;
	};

	struct C2G_QUERY_ROLE_POW : MESSAGE_HEADER
	{
		UINT32           dwRoleID;
	};

	struct C2G_SELECT_FOCUS : MESSAGE_HEADER
	{
		UINT32           dwFocusID;
	};

	struct C2G_ON_MOUNT : MESSAGE_HEADER
	{
		INT32			nMountID;
	};

	struct C2G_OFF_MOUNT : MESSAGE_HEADER
	{
	};

	struct C2G_HIRE_HERO : MESSAGE_HEADER
	{
		UINT8			byClass;
		UINT8			bySex;
	};

	struct C2G_FIRE_HERO : MESSAGE_HEADER
	{
	};

	struct C2G_QUERY_HERO_ATTR_LIST : MESSAGE_HEADER
	{
	};

	struct C2G_SUMMON_HERO : MESSAGE_HEADER
	{
	};

	struct C2G_RECALL_HERO : MESSAGE_HEADER
	{
	};

	struct C2G_CHANGE_HERO_AI : MESSAGE_HEADER
	{
		UINT8		byAiType;
	};

	struct C2G_CHANGE_HERO_NAME : MESSAGE_HEADER
	{
        char        szNewName[ROLE_NAME_LEN];
	};

	//struct C2G_GM_COMMAND : FLEX_MESSAGE_HEADER
	//{
	//	char             szGMCommand[0];
	//};

	struct C2G_UPDATE_CLIENT_VARIABLE : MESSAGE_HEADER
	{
		UINT8           byVarIndex;
		INT32           nVarValue;
	};

	struct C2G_QUERY_COMBAT_STAT : MESSAGE_HEADER
	{
	};

	struct C2G_OPERATE_DOODAD : MESSAGE_HEADER
	{
		UINT32          dwDoodadID;
	};

	struct C2G_USE_ITEM : MESSAGE_HEADER
	{
		UINT8           byConSlot;
		UINT8           byItemSlot;
	};

	struct C2G_MOVE_ITEM : MESSAGE_HEADER
	{
		UINT8           bySrcConSlot;
		UINT8           bySrcItemSlot;
		UINT8           byDstConSlot;
		UINT8           byDstItemSlot;
	};

	struct C2G_SPLIT_ITEM : MESSAGE_HEADER
	{
		UINT8           bySrcConSlot;
		UINT8           bySrcItemSlot;
		UINT8           byDstConSlot;
		UINT8           byDstItemSlot;
		INT16           shItemCount;
	};

	struct C2G_DELETE_ITEM : MESSAGE_HEADER
	{
		UINT8           byConSlot;
		UINT8           byItemSlot;
		INT16           shItemCount;
	};

	struct C2G_VERIFY_ITEM : FLEX_MESSAGE_HEADER
	{
		UINT8			byAttrCount;
		char			szAttr[0];
	};

    struct C2G_USE_MONEYTYPE01_ONLY : MESSAGE_HEADER
    {
        UINT8           byUseMoneytype01Only;
    };

    struct C2G_CHECK_NPC_SHOP_DATA : MESSAGE_HEADER
    {
        UINT32          dwCrc;
    };

	struct C2G_PICKUP_ITEM : MESSAGE_HEADER
	{
		UINT32          dwDropID;
		UINT8           byPickMode;
	};

	struct C2G_SIGN_ITEM : MESSAGE_HEADER
	{
		UINT8           byConSlot;
		UINT8           byItemSlot;
	};

	struct C2G_BUY_ITEM : MESSAGE_HEADER
	{
        INT32           nShopID;
		INT32           nShopItemIndex;
		INT16           shItemCount;
	};

	struct C2G_SELL_ITEM : MESSAGE_HEADER
	{
		UINT8           byConSlot;
		UINT8           byItemSlot;
		INT16           shItemCount;
	};

	struct C2G_VIEW_SOLD_ITEMS : MESSAGE_HEADER
	{
	};

	struct C2G_BUY_BACK_ITEM : MESSAGE_HEADER
	{
		UINT8           bySlot;
	};

	struct C2G_REPAIR_ITEM : MESSAGE_HEADER
	{
		UINT8           byConSlot;
		UINT8           byItemSlot;
	};

    struct C2G_REPAIR_ALL_EQUIPPED_ITEM : MESSAGE_HEADER
    {
    };

    struct C2G_SPECIAL_REPAIR_ITEM : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byItemSlot;
    };

	struct C2G_REFINE : MESSAGE_HEADER
	{
        UINT8           byRefineSlot;
        UINT8           byAddEquip;
        UINT8           byConSlot;
        UINT8           byItemSlot;
	};

	struct C2G_BUY_CONTAINER_SLOT : MESSAGE_HEADER
	{
		UINT8           byConSlot;
        UINT8           byCount;
	};

    struct C2G_ORGANIZE_BAG : MESSAGE_HEADER
    {
        UINT8           byConSlot;
    };

    struct C2G_IDENTIFY_ITEM : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byItemSlot;
    };

    struct C2G_RANDOMIZE_ITEM : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byItemSlot;
    };

    struct C2G_CONFIRM_RANDOMIZE_ITEM : MESSAGE_HEADER
    {
    };

	struct C2G_OPEN_STALL : MESSAGE_HEADER
	{
	};

	struct C2G_CLOSE_STALL : MESSAGE_HEADER
	{
	};

	struct C2G_REAL_OPEN_STALL : MESSAGE_HEADER
	{
	};

	struct C2G_REAL_CLOSE_STALL : MESSAGE_HEADER
	{
	};

	struct C2G_ADD_STALL_ITEM : MESSAGE_HEADER
	{
		UINT8           btStallSlot;
		UINT8           byConSlot;
		UINT8           byItemSlot;
		INT16           shItemCount;
		INT32           nPrice;
	};

	struct C2G_REMOVE_STALL_ITEM : MESSAGE_HEADER
	{
		UINT8           btStallSlot;
	};

	struct C2G_CHANGE_STALL_NAME : MESSAGE_HEADER
	{
		char            szStallName[STALL_NAME_LEN];
	};

    struct C2G_UPGRADE_STALL : MESSAGE_HEADER
    {
        UINT8           btUpgradeCostType;
    };

	struct C2G_VIEW_STALL_ITEM : MESSAGE_HEADER
	{
		UINT32          dwRoleID;
	};

	struct C2G_BUY_STALL_ITEM : MESSAGE_HEADER
	{
		UINT32          dwRoleID;
		UINT8           btStallSlot;
		INT16           shItemCount;
		INT64           llModfiyFrame;
	};

	struct C2G_TELEPORT : MESSAGE_HEADER
	{
		UINT32          dwTeleportID;
	};

	struct C2G_LOADFINISH : MESSAGE_HEADER
	{
		INT32           nErrCode;
	};

	struct C2G_QUEST_RECYCLE : MESSAGE_HEADER
	{
		INT32			nTemplateID;
		INT32			nIndex;
	};

	struct C2G_COMPLETE_KILL_NPC_QUEST : MESSAGE_HEADER
	{
		UINT16			wQuestID;
		UINT8			byIndex;
	};

	struct C2G_ACCEPT_QUEST : MESSAGE_HEADER
	{
		INT32           nQuestID;
		INT32           nAcceptType;
		INT32           nAcceptSrcID;
	};

	struct C2G_ABANDON_QUEST : MESSAGE_HEADER
	{
		INT32           nQuestID;
	};

	struct C2G_COMPLETE_QUEST : MESSAGE_HEADER
	{
		INT32           nQuestID;
		INT32           nSelectedAwardIdx;
	};

	struct C2G_INTERACT_WITH_NPC : MESSAGE_HEADER
	{
		UINT32          dwNpcID;
	};

    struct C2G_STOP_INTERACT_WITH_NPC : MESSAGE_HEADER
    {

    };

	struct C2G_SHARE_QUEST : MESSAGE_HEADER
	{
		INT32			nQuestID;
		INT32			nShareType;
	};

	struct C2G_SELECT_DIALOG_OPTION : MESSAGE_HEADER
	{
		INT32            nParam;
	};

	struct C2G_RETURN_SELECT_ROLE : MESSAGE_HEADER
	{
		UINT32           dwRoleID;
	};


	struct C2G_FRIEND_CHANGE_GROUP_NAME : MESSAGE_HEADER
	{
		UINT8               byGroupID;
		char                szGroupName[FRIEND_GROUP_NAME_LEN];
	};

	struct C2G_ADJACENT_ROLE_NOTEAM : MESSAGE_HEADER
	{
	};

	struct C2G_ADJACENT_TEAM : MESSAGE_HEADER
	{
	};

	struct C2G_TEAM_AUCTION_BID : MESSAGE_HEADER
	{
		UINT32  dwAuctionIndex;
		UINT32	dwOfferPrice;
	};

	struct C2G_TEAM_AUCTION_ABANDON : MESSAGE_HEADER
	{
		UINT32  dwAuctionIndex;
	};

	struct C2G_DUNGEON_SELECT_REWARD : MESSAGE_HEADER
	{
		UINT8 byRewardType;
	};

	struct C2G_DUNGEON_REPLAY : MESSAGE_HEADER
	{

	};

	struct C2G_TRADE_ASK : MESSAGE_HEADER
	{
		UINT32 dwRoleID;
	};

	struct C2G_TRADE_ACCEPT : MESSAGE_HEADER
	{
		UINT32 dwRoleID;
	};

	struct C2G_TRADE_SET_ITEM : MESSAGE_HEADER
	{
		UINT8           byTradeSlot;
		UINT8           bAdd;
		UINT8           byConSlot;
		UINT8           byItemSlot;
	};

	struct C2G_TRADE_SET_MONEY : MESSAGE_HEADER
	{
		INT32           nMoney;
	};

	struct C2G_TRADE_LOCK : MESSAGE_HEADER
	{
	};

	struct C2G_TRADE_CONFIRM : MESSAGE_HEADER
	{
	};

	struct C2G_TRADE_UNLOCK : MESSAGE_HEADER
	{
	};

	struct C2G_TRADE_CANCEL : MESSAGE_HEADER
	{
	};

	struct  C2G_LOGOUT : MESSAGE_HEADER
	{
	};

	struct C2G_LOGOUT_CANCEL : MESSAGE_HEADER
	{
	};

	struct C2G_TITLE_EQUIP : MESSAGE_HEADER
	{
		UINT8               nTemplateTitleID;
	};

	struct C2G_TITLE_UNEQUIP : MESSAGE_HEADER
	{
	};

	struct C2G_CONFIRM_PUBLISH_QUEST : MESSAGE_HEADER
	{
		INT32				nQuestID;
		UINT32				dwMasterID;
	};

	struct C2G_TALK_TO_NPC : MESSAGE_HEADER
	{
		INT32				nQuestID;
		INT32				dwTalkNpcID;
	};

	struct C2G_EXPLORE_ZONE : MESSAGE_HEADER
	{
	};

	struct C2G_FLY_TO_QUEST_ZONE : MESSAGE_HEADER
	{
		INT32				nQuestID;
		INT32				nZoneIndex;
	};

	struct C2G_GENERATE_CIRCLE_QUEST : MESSAGE_HEADER
	{
	};

	struct C2G_BUY_DAILY_QUEST_COMPLETE_TIME : MESSAGE_HEADER
	{
		INT32				nTime;
	};

	struct C2G_DAILY_QUEST_COMPLETE_SHOT_CUT : MESSAGE_HEADER
	{
	};

	struct C2G_BRUSH_DAILY_QUEST : MESSAGE_HEADER
	{
		BOOL				bUseMoney;
	};

	struct C2G_COMPLETE_ACHIEVEMENT : MESSAGE_HEADER
	{
		INT32				nAchievementID;
	};

	struct C2G_REWARD_ACHIEVEMENT : MESSAGE_HEADER
	{
		INT32				nAchievementID;		
	};

	struct C2G_COMPARE_ACHIEVEMENT : MESSAGE_HEADER
	{
		UINT32				dwRoleID;
	};

    struct C2G_MASTER_TWITTER : MESSAGE_HEADER
    {
        UINT8       byTwitter;
    };

	struct C2G_RECEIVE_MASTER_REWARD_EXP : MESSAGE_HEADER
	{
		UINT32		dwRoleID;
	};

	struct C2G_QUERY_MASTER_REWARD_EXP : MESSAGE_HEADER
	{

	};

	struct C2G_GUILD_CREATE : FLEX_MESSAGE_HEADER
	{
		char        szGuildName[GUILD_NAME_LEN];
		char        szAnnounce[0];
	};

	struct C2G_GUILD_DRAW_SALARY : MESSAGE_HEADER
	{

	};

	struct C2G_GUILD_CONTRIBUTE_FUND : MESSAGE_HEADER
	{
		INT32       nMoney;
	};

    struct C2G_GUILD_VIEW_STORAGE : MESSAGE_HEADER 
    {
        UINT8       byStorageTag;
    };

    struct C2G_GUILD_PUSH_ITEM : MESSAGE_HEADER 
    {
        UINT16      wItemConSlot;
        UINT16      wItemSlot;
        UINT8       byStorageConSlot;
        UINT8       byStorageSlot;
    };

    struct C2G_GUILD_POP_ITEM : MESSAGE_HEADER
    {
        UINT8       byStorageConSlot;
        UINT8       byStorageSlot;
        UINT16      wItemConSlot;
        UINT16      wItemSlot;
    };

    struct C2G_GUILD_CREATE_BUILDING : MESSAGE_HEADER 
    {
        UINT32      dwNpcID;
        UINT8       byBuildingType;
    };

    struct C2G_GUILD_UPGRADE_BUILDING : MESSAGE_HEADER 
    {
        UINT8       byUpgradeType;      // UPGRADE_BUILDING_TYPE
        UINT64      qwNpcNameID;
        UINT8       byBuildingType;
    };

    struct C2G_GUILD_DEL_BUILDING : MESSAGE_HEADER
    {
        UINT8       byDelType;      // UPGRADE_BUILDING_TYPE
        UINT64      qwNameID;
    };

    struct C2G_GUILD_BUY_AURA  : MESSAGE_HEADER
    {
        INT8        byType;
    };

    struct C2G_GUILD_CHANGE_WEATHER : MESSAGE_HEADER
    {
        INT8        byWeatherType;
        INT8        byTimeType;
    };

    struct C2G_GUILD_ENTER_MAP : MESSAGE_HEADER
    {
        UINT8       byMapType;      // GUILD_ENTER_MAP_TYPE
        UINT32      dwSceneID;
    };

    struct C2G_GUILD_OPEN_DUNGEON : MESSAGE_HEADER
    {
        UINT32      dwTemplateID;
        UINT32      dwLeaderID;
    };

    struct C2G_GUILD_ENTER_DUNGEON : MESSAGE_HEADER 
    {
        UINT32      dwTemplateID;
    };

    struct C2G_GUILD_REWARD_ACHIEVEMENT : MESSAGE_HEADER
    {
        INT32    nID;
    };

    //////////////////////////////////////////////////////////////////////////
    // arena
	struct C2G_ARENA_JOIN_SEASON : MESSAGE_HEADER
	{
	};

	struct C2G_ARENA_QUIT_SEASON : MESSAGE_HEADER
	{
	};

	struct C2G_ARENA_MATCH : MESSAGE_HEADER
	{
	};

	struct C2G_ARENA_GET_REWARD : MESSAGE_HEADER
	{

	};


    // spirit
	struct C2G_SPIRIT_EXP_CHANGE: MESSAGE_HEADER
	{
		BOOL		bIsOnline;
		BOOL		bUseMoney;
	};

	struct C2G_SPIRIT_ADD_AIR : MESSAGE_HEADER
	{
		BOOL	bQuick;
		UINT8	dwIndex;
	};

	struct C2G_SPIRIT_MERGE_AIR : MESSAGE_HEADER
	{
		INT32	nSrcCon;
	};

	struct C2G_SPIRIT_BUY_AIR_SLOT : MESSAGE_HEADER
	{
		INT32	nBagType;
		INT32	nCount;
	};

	struct C2G_SPIRIT_MOVE_AIR : MESSAGE_HEADER
	{
		INT32	nSrcBagType;
		INT32	nSrcIndex;
		INT32	nTarBagType;
		INT32	nTarIndex;
	};

	struct C2G_TELEPORT_BY_PLATFORM : MESSAGE_HEADER
	{
		UINT8	bySrcPlatform;
		UINT8	byTgtPlatform;
	};

	struct C2G_SPIRIT_EXCHANGE_AIR : MESSAGE_HEADER
	{
		UINT16	wAirID;
	};

	struct C2G_SPIRIT_BRUSH_SLOT : MESSAGE_HEADER
	{
		INT32	nPage;
		INT32	nIndex;
	};

	struct C2G_SPIRIT_BLOOD_OPEN : MESSAGE_HEADER
	{
		INT32	nPage;
	};

    struct C2G_UNSHIELD : MESSAGE_HEADER
    {
        UINT32 dwRoleID;
    };

    struct C2G_UPGRADE_SOUL : MESSAGE_HEADER
    {
        UINT8           byTargetConSlot;
        UINT8           byTargetItemSlot;
        UINT8           bySrcCount;
        UINT8           bySrcConSlot[5];
        UINT8           bySrcItemSlot[5];
    };

    struct C2G_ADD_SOUL: MESSAGE_HEADER
    {
        UINT8           byEquipConSlot;
        UINT8           byEquipItemSlot;
        UINT8           bySocketIndex;
        UINT8           bySoulConSlot;
        UINT8           bySoulItemSlot;
    };

    struct C2G_REMOVE_SOUL : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byItemSlot;
        UINT8           bySocketIndex;
    };

    struct C2G_RUNE_ITEM : MESSAGE_HEADER
    {
        UINT8           byWeaponConSlot;
        UINT8           byWeaponItemSlot;
        INT32           nRuneItemID;
    };

    struct C2G_WEAPON_LEVEL_UP_PUT_IN : MESSAGE_HEADER
    {

    };

    struct C2G_WEAPON_LEVEL_UP_TAKE_OUT : MESSAGE_HEADER
    {

    };

	struct C2G_ACTIVITY_REWARD : MESSAGE_HEADER
	{
		INT32			nIndex;
	};

    struct C2G_DECOMPOSE : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byItemSlot;
    };

    struct C2G_COMPOSE : MESSAGE_HEADER
    {
        INT32           nFormulaID;
    };

    struct C2G_LEAVE_SCENE : MESSAGE_HEADER
    {
        
    };

    struct C2G_PING : MESSAGE_HEADER
    {
        UINT32      dwPingTick;
    };

    struct C2G_REVIVE : MESSAGE_HEADER
    {
		UINT8		byType;
    };

    struct C2G_CANCEL_CUTSCENE : MESSAGE_HEADER
    {
    };

	struct C2G_ROLE_TELEPORT : MESSAGE_HEADER
	{
		UINT16		wDestX;
		UINT16		wDestY;
	};

	struct C2G_DICE_BET : MESSAGE_HEADER
    {
		UINT16 bytype;
		UINT16 bycount;
    };

	struct C2G_QUERY_ROLE_LOCATION : FLEX_MESSAGE_HEADER
	{
		char		szRoleName[0];
	};

	struct C2G_RECALL_TEAM_MEMBER : MESSAGE_HEADER
	{

	};

	struct C2G_ROLE_MINE : MESSAGE_HEADER
	{
		UINT16		wDestX;
		UINT16		wDestY;
	};

    struct C2G_SUBSCRIBE_VIP : MESSAGE_HEADER
    {
        UINT8       byVIPLevel;
        UINT8       byMonth;
    };

	struct  C2G_CLIENT_HARDWARE_INFO : MESSAGE_HEADER
	{
		struct stCpuInfo
		{
			UINT8	NumberOfCores;
			UINT8	NumberOfLogicalProcessors;
			UINT16	Family;
			UINT16	Level;
			UINT16	Revision;
			UINT16	AddressWidth;
			UINT16	MaxClockSpeed;
			UINT16	L2CacheSize;
			UINT16	L3CacheSize;
			char	Name[MAX_DEVICE_STRING_LEN];
			char	Version[MAX_DEVICE_STRING_LEN]; // os version
			char	Stepping[MAX_DEVICE_STRING_LEN]; // role name
		};

		struct stMemoryInfo
		{
			UINT16 MemoryType;
			UINT32 Speed; // role id
			UINT64 Capacity;
		};

		struct stDiskInfo
		{
			char	Model[MAX_DEVICE_STRING_LEN];
			char	MediaType[MAX_DEVICE_STRING_LEN]; // BUILD_NUMBER
			UINT64	Size; // total virtual memory
		};

		struct stNetworkAdapterInfo
		{
			char	Name[MAX_DEVICE_STRING_LEN];
			char	AdapterType[MAX_DEVICE_STRING_LEN];
			char	NetworkAdapterInfo[MAX_NETWORK_ADAPTERINFO_STRING_LEN];
		};

		struct stVideoInfo
		{
			char	VideoProcessor[MAX_DEVICE_STRING_LEN];
			char	Name[MAX_DEVICE_STRING_LEN];
			UINT16	VideoMemoryType;
			UINT32	AdapterRAM;
			UINT32	CurrentBitsPerPixel;
			UINT32	CurrentHorizontalResolution;
			UINT32	CurrentRefreshRate;
			UINT32	CurrentVerticalResolution;
		};

		stCpuInfo				stcpuinfo;
		stMemoryInfo			stmemoryinfo;
		stDiskInfo				stdiskinfo;
		stVideoInfo				stvideoinfo;
		stNetworkAdapterInfo	stnetworkadapterinfo;
	};

	struct C2G_CLIENT_FPS_INFO : MESSAGE_HEADER
	{
		UINT8		byGameFps;
		UINT8		byCpuFps;
		UINT8		byGpuFps;
		UINT16		wGateDelay;
		UINT16		wGameDelay;
	};

	struct C2G_QUERY_SURVEY_PATH : MESSAGE_HEADER
	{
	}; 

	struct C2G_RECEIVE_REPUTATION_AWARD : MESSAGE_HEADER
	{
		UINT8		byForceID;
	};

    struct C2G_CHANGE_PET_STATE : MESSAGE_HEADER
    {
        UINT8       byState;
    };

	struct C2G_CHECKIN : MESSAGE_HEADER
	{
	};

    struct C2G_BLACK_MARKET_LIMIT_INFO : MESSAGE_HEADER
    {
    };

    struct C2G_BLACK_MARKET_COOLDOWN_INFO : MESSAGE_HEADER
    {
    };

    struct C2G_BLACK_MARKET_BUY : MESSAGE_HEADER
    {
        INT32       nBlackMarketProductID;
    };

    struct C2G_GAMBLE : MESSAGE_HEADER
    {
        UINT8       byEquipType;
    };

    struct C2G_EXERCISE_INFO : MESSAGE_HEADER
    {
    };

    struct C2G_ADD_EXERCISE_TIME : MESSAGE_HEADER
    {
        INT32       nHour;
    };

    struct C2G_GET_EXERCISE_EXP : MESSAGE_HEADER
    {
    };

	struct C2G_JIGSAW_ANSWER : MESSAGE_HEADER
	{
		UINT8		byIndex;
	};

    struct C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY : MESSAGE_HEADER
    {

    };

    struct C2G_SPELL_BOOK_GAMBLE : MESSAGE_HEADER
    {

    };

    struct C2G_LEAVE_GUILD : MESSAGE_HEADER
    {
    };

    struct C2G_QUERY_GUILD_WAR_RECORD : MESSAGE_HEADER
    {
        UINT8           byPage;	
	};

    struct C2G_RUNE_ITEM_ADVANCE : MESSAGE_HEADER
    {
        UINT8           byWeaponConSlot;
        UINT8           byWeaponItemSlot;
        INT32           nRuneItemID;
    };

    struct C2G_RUNE_ITEM_ADVANCE_CONFIRM : MESSAGE_HEADER
    {
        UINT8           byWeaponConSlot;
        UINT8           byWeaponItemSlot;
    };

    struct C2G_POTION_GAMBLE : MESSAGE_HEADER
    {
        INT32           nGambleLevel;
    };

    struct C2G_GAMBLE_ADVANCE : MESSAGE_HEADER
    {
        INT32           nGambleType;
        INT32           nGambleLevel;
    };

    struct C2G_SELL_RESOURCE_ITEM : MESSAGE_HEADER
    {
    };

	struct C2G_LOVERS_TELEPROT : MESSAGE_HEADER
	{

	};

	// wing panel (1.05 UnrealWing)
	struct C2G_WING_OP : MESSAGE_HEADER
	{
		INT32			nOp;		// 0: query (reply g2c_wing_info)  1: wear wing nParam  2: take the wing off  3: upgrade the wing level
		INT32			nParam;
	};
};


// tolua_end
#pragma pack(pop)

#endif  // _C2G_MESSAGE_H_
