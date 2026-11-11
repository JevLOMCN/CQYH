#ifndef _STRING_DEF_H_
#define _STRING_DEF_H_

/// @file string_def.h
/// @brief definition of 

#define COMMON_NAME_LEN         (256)                           ///< @line
#define USER_DATA_LEN           (256)                           ///< @line
#define MS_NAME_LEN             (33)                            ///< @line
#define QUEST_DESC_LEN	        (4096)                          ///< @line
#define PATH_LEN		        (256)                           ///< @line

// 默认代码 65, 改成 33
//#define USER_NAME_LEN           (65)                            ///< @line
#define USER_NAME_LEN           (33)                            ///< @line
#define USER_PASSWORD_LEN       (32)                            ///< @line

#define ROLE_NAME_LEN           (25)                            ///< @line
#define OA_SESSION_ID_LEN		(64)                            ///< @line
#define MAC_ADDRESS_LEN         (18)                            ///< @line
#define OA_ADULT_FIELD_LEN      (2)                             ///< @line
#define OA_PGT_FIELD_LEN        (64)                            ///< @line
#define OA_DYNAMIC_PASSWORD_LEN (7)                             ///< @line
#define OA_IP_LEN               (16)                            ///< @line
#define MAX_GM_COMMAND_LEN      (4096)                          ///< @line
#define MAX_EMBEDDED_SCRIPT_LEN (32)                            ///< @line
#define SHOP_NAME_LEN           (64)                            ///< @line

#define MAX_TITLE_NAME_LEN      (64)                            ///< @line
#define MAX_SPELL_NAME_LEN      (64)                            ///< @line
#define MAX_NPC_NAME_LEN        (64)                            ///< @line
#define MAX_DOODAD_NAME_LEN     (64)                            ///< @line
#define MAX_AURA_NAME_LEN       (64)                            ///< @line
#define MAX_ITEM_NAME_LEN       (64)                            ///< @line
#define GUILD_NAME_LEN          (25)                            ///< @line
#define TEAM_NAME_LEN           (32)                            ///< @line
#define ROOM_DESCRIBE_LEN       (32)                            ///< @line
#define MAX_ROLE_DATA_LEN		(16000)                         ///< @line
#define DB_SETTING_STRING_LEN	(64)

#define STALL_NAME_LEN          (33)

#define fUNC_NAME_LEN			(32)

#define MAIL_SUBJECT_LEN        (61)
#define MAIL_CONTENT_LEN        (421)

#define GUILD_ANNOUNCE_LEN      (100 + 1)
#define GUILD_NOTICE_LEN        (200 + 1)

#define	AIR_NAME_LEN			(32)

#define GM_CMD_NAME_LEN         (64)
#define GM_TOOL_PASSWORD_LEN    (33)
#define GM_REASON_LEN           (512)
#define GM_ANNOUNCE_LENGTH      (512)

#define OA_BILLING_ORDER_ID_LEN (33)
#define OA_BILLING_UNIQUE_ID_LEN (33)

#define MAX_DEVICE_STRING_LEN	(256)
#define MAX_NETWORK_ADAPTERINFO_STRING_LEN	(20)

#define MAX_SURVEY_PATH_LEN		(128)
#define MAX_SURVEY_COUNT		(10)
#define CHEAT_DETAIL_LEN		(512)

#define GPK_IP_LEN				(16)
#define GPK_STRATEGY_NAME_LEN	(64)

#define SETTING_FOLDER_NAME             "setting"

#define SCENE_DATA_FILE_NAME	        "scene"
#define NPC_DATA_FILE_NAME              "npc"
#define NPC_PATTERN_DATA_FILE_NAME		"npc_pattern"
#define DOODAD_DATA_FILE_NAME           "doodad"
#define SPELL_FILE_NAME			        "spell"
#define SPELL_VAR_FILE_NAME		        "spell_var"
#define SPELL_NODE_FILE_NAME	        "spell_node"
#define SPELL_PROGRESS_FILE_NAME	    "spell_progress"
#define RUNE_FILE_NAME			        "rune"
#define AURA_FILE_NAME			        "aura"
#define TRANSFORM_FILE_NAME			    "transform"
#define NEW_AI_FILE_NAME                "new_ai"
#define AI_FILE_NAME			        "ai"
#define MODIFIERSEQ_FILE_NAME	        "modifier_seq"
#define CHECKERSEQ_FILE_NAME	        "checker_seq"
#define MISSILE_FILE_NAME		        "missile"
#define ITEM_FILE_NAME                  "item"
#define ITEM_ARMOR_FILE_NAME            "item_armor"
#define SET_FILE_NAME                   "set"
#define RUNE_FILE_NAME                  "rune"
#define SOUL_FILE_NAME                  "soul"
#define REFINE_FILE_NAME                "refine"
#define REFINE_ATTR_FILE_NAME           "refine_attr"
#define ATTRIBUTE_FILE_NAME             "attribute"
#define ATTRIBUTE_POOL_FILE_NAME        "attribute_pool"
#define SHOP_ITEM_FILE_NAME             "shop_item"
#define SHOP_FILE_NAME                  "shop"
#define DROP_GROUP_FILE_NAME            "drop_group"
#define NPC_DROP_LEVEL_DATA_FILE_NAME   "drop_npc_level"
#define NPC_DROP_SCENE_DATA_FILE_NAME   "drop_npc_scene"
#define NPC_DROP_ID_DATA_FILE_NAME      "drop_npc_id"
#define NPC_DROP_ID_CLASS_DATA_FILE_NAME "drop_npc_id_class"
#define DOODAD_DROP_DATA_FILE_NAME      "doodad_drop_data"
#define MODIFIER_TREE_FILE_NAME	        "setting/modifier_tree.mdf"
#define QUEST_FILE_NAME	                "quest"
#define QUEST_RECYCLE_FILE_NAME			"quest_recycle"
#define REWARD_QUEST_MONEY_COST_NAME	"reward_quest_money_cost"
#define INIT_REPUTATATION_FILE_NAME     "reputation"
#define NPC_RELATION_FILE_NAME          "npc_relation"
#define ROLE_RELATION_FILE_NAME         "role_relation"
#define LEVEL_DAMAGE_MOD_FILE_NAME      "level_damage_mod"
#define ACHIEVEMENT_FILE_NAME			"achievement"
#define CHECKIN_FILE_NAME				"checkin"
#define	HANDBOOK_FILE_NAME				"handbook"
#define MOUNT_FILE_NAME					"mount"
#define WINGS_FILE_NAME					"wings"
#define	SPIRIT_FILE_NAME				"spirit"
#define	BLOOD_FILE_NAME					"blood"
#define	SPIRIT_SLOT_FILE_NAME			"spirit_slot"
#define	AIR_FILE_NAME					"air"
#define	AIR_LEVEL_FILE_NAME				"air_level"
#define QUEST_REWARD_FILE_NAME			"quest_reward"
#define CIRCLE_QUEST_FILE_NAME			"circle_quest"
#define DAILY_QUEST_FILE_NAME			"daily_quest"
#define DUNGEON_DATA_FILE_NAME			"dungeon"
#define DECOMPOSE_DATA_FILE_NAME        "decompose"
#define COMPOSE_DATA_FILE_NAME          "compose"
#define PK_DROP_COUNT_DATA_FILE_NAME    "pk_drop_count"
#define PK_DROP_RATE_DATA_FILE_NAME     "pk_drop_rate"
#define	ACTIVITY_REWARD_DATA_FILE_NAME	"activity_reward"
#define ADVENTURE_DATA_FILE_NAME        "adventure"
#define ARENA_SCORE_FILE_NAME			"arena_score"
#define	ACTIVITY_DATA_FILE_NAME			"activity"
#define MONEY_DATA_FILE_NAME            "money"
#define LEVEL_FILE_NAME					"level"
#define HERO_LEVEL_FILE_NAME			"hero_level"
#define HERO_ATTR_FILE_NAME				"hero_attribute1"
#define LEVEL_ATTRIBUTE_FILE_NAME		"attributes1"
#define ATTRIBUTE_CONVERSION_FILE_NAME	"attributes2"
#define MAIL_TEMPLATE_FILE_NAME         "mail_template"
#define NAV_FOLDER_NAME                 "navmeshconfig"
#define NAV_FILE_EXT_NAME               ".dtNavMesh"
#define SPIRIT_CHANGE_FILE_NAME         "spirit_change"
#define TITLE_FILE_NAME                 "title"
#define MASTER_LVL_UP_GIFT_NAME			"master_gift"
#define PET_DATA_FILE_NAME              "pet"
#define KILL_NPC_QUEST_FILE_NAME		"kill_npc_quest"
#define BLACK_MARKET_FILE_NAME          "black_market"
#define GAMBLE_FILE_NAME                "gamble"

#define GUILD_BUILDING_NAME             "guild_building"
#define GUILD_WELFARE_NAME              "guild_welfare"
#define GUILD_DUNGEON_NAME              "guild_dungeon"
#define GUILD_ACHIEVEMENT_NAME          "guild_achievement"
#define GUILD_ACHI_CDTN_NAME            "guild_achi_cdtn"
#define GUILD_ACTIVITY_NAME             "guild_activity"

#define GM_LOG_NOTE_NAME                "gm_log_note"

#define REAL_MONEY_SHOP_DATA_FILE_NAME  "real_money_shop"

#define STRING_TABLE_SCRIPT_NAME        "script/stringtable/main.lua"
#define GM_SCRIPT_NAME                  "script/gs_gm/main.lua"
#define MS_GM_SCRIPT_NAME               "script/ms_gm/main.lua"
#define MS_CS_SCRIPT_NAME               "script/final_gm/ms.lua"
#define GS_CS_SCRIPT_NAME               "script/final_gm/gs.lua"
#define GM_PERMISSION_NAME              "script/gm_server/main.lua"
#define CREATE_ROLE_SCRIPT_NAME         "script/create_role/main.lua"
#define LOAD_ROLE_SCRIPT_NAME			"script/load_role/main.lua"
#define CREATE_HERO_SCRIPT_NAME         "script/hero_create/main.lua"
#define ROLE_LEVEL_SCRIPT_NAME          "script/level/main.lua"
#define HERO_LEVEL_SCRIPT_NAME          "script/hero_level/main.lua"
#define AI_SCRIPT_FOLDER                "script/ai"
#define NEW_AI_SCRIPT_FOLDER            "script/newai"
#define SCENE_SCRIPT_FOLDER             "script/scene"
#define NPC_PATTERN_SCRIPT_NAME         "script/npc_pattern/main.lua"
#define READ_DATA_SCRIPT_NAME			"script/read_data/main.lua"
#define INTERACT_FUNC_NAME              "interact"
#define INTERACT_SCRIPT_FOLDER          "script/interact"
#define ITEM_SCRIPT_NAME                "script/item/main.lua"
#define CHECK_WORLD_DATA				"script/check_data/check_data.lua"	

#define MASTER_SCRIPT_NAME              "script/master/main.lua"
#define QUEST_SCRIPT_NAME				"script/quest/main.lua"
#define ARENA_SCRIPT_NAME				"script/arena/main.lua"	
#define GUILD_SCRIPT_NAME               "script/guild/main.lua"
#define GUILD_ACTIVITY_SCRIPT_NAME      "script/guild/guild_activity.lua"
#define ACTIVITY_SCRIPT_NAME			"script/activity/main.lua"
#define EXCHANGE_SCRIPT_NAME			"script/exchange/main.lua"
#define DL_LOAD_ROLE_SCRIPT_NAME		"script/dl_loadrole/main.lua"
#define GPK_PUNISH_SCRIPT_NAME			"script/gpk_punish/main.lua"
#define DOODAD_SCRIPT_FILE_NAME			"script/doodad/main.lua"

#define SCENE_FILE_FOLDER               "scene"

#define DUNGEON_REPLAY_START_POS_NAME	"PReStart"
#define ENTER_POS_SIDE_NAME		        "PEnter_"

#define GPK_SERVER_BIN_FOLDER_64		"dyncodebin64/server"
#define GPK_CLIENT_BIN_FOLDER_64		"dyncodebin64/client"

#define GPK_SERVER_BIN_FOLDER_32        "dyncodebin/server"
#define GPK_CLIENT_BIN_FOLDER_32        "dyncodebin/client"

#define DEPLOY_RESULT_FILE_NAME				"version.txt"   // generated in _GenerateDeployBatFile(), check in ProcessDeployServerVersion()
#define HOT_UPDATE_PATCH_RESULT_FILE_NAME	"patch.txt"		// generated in _GenerateHotUpdateBatFile(), check in ProcessHotUpdatePatch()
#define CURRENT_VERSION_FILE_NAME			"current_server_version.txt"   // generated in _GenerateDeployBatFile()
#define CURRENT_PATCH_FILE_NAME				"current_server_patch.txt"   // generated in _GenerateHotUpdateBatFile()

#define ANTI_CHEAT_FILE_FOLDER			"cheat"

// tolua_begin

#define SERVER_STRING_START_ID  (1000000)                       ///< @line
#define STRING_TABLE_STR_LEN    (512)                           ///< @line
#define STRING_TABLE_STR_COUNT  (2048)                          ///< @line

enum ENUM_SERVER_STRING
{
    essInvalid = SERVER_STRING_START_ID,

    // item
    essItemRefineSuccess,
    essItemComposeSuccess,
    essItemComposeSuccessDouble,
    essItemRuneSuccess,
    essItemMaterial,
    essItemLevelUp,
    essItemLuckOil,
    essItemGambleSuccess,
    essItemSpellBookGambleSuccess,
    essItemDropByNpc,

    //
    essBossKilled,

	// award
	essAwardOpenDialog,

	//
	essRoleKilled,

	// 
	essSceneLineRecycleWarning,

    essTotal
};

// tolua_end

#endif	// _STRING_DEF_H_
