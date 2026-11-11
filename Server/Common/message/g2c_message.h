#ifndef _G2C_MESSAGE_H_
#define _G2C_MESSAGE_H_

#include "external_message_header.h"

#pragma pack(push, 1)
namespace ExternalMessage
{
    enum G2C_MESSAGE_ID
    {
        g2c_message_begin = 8,

        // misc
        g2c_error_notify,
        g2c_gpk_dyn_code,
        g2c_gpk_auth_data,

        // login
        g2c_sync_game_data_begin,
        g2c_sync_data_error,
        g2c_sync_attr_list,
        g2c_sync_container_sizes,
        g2c_sync_refine,
        g2c_sync_item_bag,
        g2c_sync_spell_book,
        g2c_sync_spell_slot,
        g2c_sync_quest_list,
        g2c_sync_kill_npc_quest,
        g2c_sync_reputation,
        g2c_sync_stall,
        g2c_sync_title,
        g2c_sync_master,
        g2c_sync_guild,
        g2c_sync_spirit,
        g2c_sync_mount_list,
        g2c_sync_achievement_list,
        g2c_sync_purchase_aura_record,
        g2c_sync_censored_word_list,
        g2c_sync_guild_achievement,
        g2c_sync_game_data_end,

        g2c_sync_role_attr_data,
        g2c_sync_role_equip_data,
        g2c_sync_role_spirit_data,
        g2c_sync_role_spell_data,

        g2c_sync_frame,
        g2c_enter_scene,
        g2c_leave_scene,
        g2c_changescene_enter,
        g2c_return_select_role,
        g2c_sync_next_jump_point,

        // move
        g2c_sync_role_pos,
        g2c_ping_ack,
        g2c_char_walk,
        g2c_char_run,
        g2c_char_stop,
        g2c_char_passive_move,
        g2c_object_rotate,
        g2c_char_teleport,
        g2c_char_freeze,
        g2c_char_client_show_time,
        g2c_char_hard,
        g2c_char_die,
        g2c_char_revive,
        g2c_revive_info,
        g2c_activate_revive_pt,
        g2c_char_mine,

        g2c_char_appear,
        g2c_sync_char_list,
        g2c_object_disappear,
        g2c_object_disappear_list,
        g2c_sync_role_data,
        g2c_sync_npc_data,
        g2c_sync_npc_ex_data,
        g2c_sync_hero_data,
        g2c_sync_shield_list,
        g2c_sync_flag_pos,
        g2c_sync_doodad_open_count,
        g2c_sync_aura_brief,
        g2c_sync_pet_level,
        g2c_sync_npc_transform_state,

        // base attr
        g2c_role_level_up,
        g2c_exp_change,
        g2c_extra_exp_change,
        g2c_sync_attr_change,
        g2c_sync_hp,
        g2c_sync_mp,
        g2c_sync_pk_value,
        g2c_sync_pk_result,
        g2c_sync_grey_name,
        g2c_sync_equip_title,
        g2c_sync_combo_kill,
        g2c_sync_msg_paopao,
        g2c_sync_role_pow,
        g2c_sync_master_reward_exp,

        // spell
        g2c_role_learn_spell,
        g2c_role_remove_spell,

        g2c_role_learn_item_spell,
        g2c_role_remove_item_spell,

        g2c_char_precast_spell,  // Queue a spell?
        g2c_char_prepare_spell,  // Prepare a spell?
        g2c_char_ex_cast_spell,  // Cast extended spell?
        g2c_char_cast_spell,     // Cast a spell?
        //		g2c_char_spell_pre_effect,
        g2c_char_spell_effect,
        g2c_char_break_cast,
        g2c_role_cast_over,
        g2c_equip_spell,
        g2c_equip_rune,
        g2c_unlock_spell_slot,
        g2c_leech_soul,
        g2c_directly_hp_change_effect,
        g2c_combat_pose_switch,
        g2c_clear_spell_cache,
        g2c_sync_missile_list,

        g2c_add_cooldown,    // Add cooldown
        g2c_sync_pk_mode,
        g2c_enter_combat,
        g2c_leave_combat,
        g2c_sync_faction,

        g2c_addto_threatlist,
        g2c_delfrom_threatlist,

        g2c_new_aura,    // Add buff aura
        g2c_del_aura,
        g2c_aura_change,
        g2c_aura_effect,
        g2c_sync_aura_list,

        g2c_char_select_focus,    // 119, 
        g2c_focus_info,
        g2c_missile_appear,
        g2c_missile_move,

        g2c_spell_level_up,
        g2c_sync_spell_pts,

        // item
        g2c_item_change,
        g2c_item_swap,
        g2c_item_delete,
        g2c_destroy_death_item,
        g2c_lose_pk_item,
        g2c_view_sold_items,
        g2c_repair_item,
        g2c_pickup_item,
        g2c_pickup_money,
        g2c_container_size_change,
        g2c_sync_role_avatar,
        g2c_sync_hero_avatar,
        g2c_sign_item,
        g2c_luck_oil,
        g2c_sync_npc_shop_data,
        g2c_equip_dur_change,

        // mount
        g2c_activate_mount,
        g2c_char_on_mount,
        g2c_char_off_mount,

        // money
        g2c_sync_money,
        g2c_money_change,

        // drop
        g2c_generate_drop_from_object,
        g2c_sync_drop_list,

        // doodad
        g2c_doodad_appear,
        g2c_sync_doodad_list,
        g2c_operate_doodad_begin,
        g2c_operate_doodad_end,

        // stall
        g2c_stall_state_change,
        g2c_add_stall_item,
        g2c_remove_stall_item,
        g2c_change_stall_name,
        g2c_change_stall_level,
        g2c_view_stall_item,
        g2c_buy_stall_item,
        g2c_sold_stall_item,

        // quest
        g2c_accept_quest,
        g2c_abandon_quest,
        g2c_complete_quest,
        g2c_interact_result,
        g2c_notify_next_circle_quest,
        g2c_buy_daily_quest_complete_time,
        g2c_brush_daily_quest,
        g2c_share_quest,
        g2c_publish_quest,
        g2c_quest_recycle,
        g2c_set_quest_failed,

        g2c_sync_kill_npc_count,
        g2c_complete_kill_npc_quest,
        g2c_reset_kill_npc_quest,

        // reputation
        g2c_modify_reputation,

        g2c_chat_message,

        // dungeon
        g2c_query_scene_difficulty,
        g2c_select_scene_difficulty,

        g2c_sync_door_state,
        g2c_play_action,

        // variables
        g2c_sync_scene_variables,
        g2c_sync_role_variables,
        g2c_sync_client_variables,
        g2c_update_variable,
        g2c_update_weather,

        // pk
        g2c_update_consecutive_kill_npc_count,
        g2c_update_consecutive_kill_npc_end,
        g2c_update_momentary_kill_npc_end,

        g2c_update_consecutive_kill_player_count,

        g2c_update_kill_buf_player_count,

        // team
        g2c_team_sync_member,			  // 
        g2c_sync_adjacent_role_noteam,    // 
        g2c_sync_adjacent_team,			  // 
        g2c_sync_role_team,				  // 193

        // team aution
        g2c_team_auction_add_item,
        g2c_team_auction_begin,
        g2c_team_auction_notify,
        g2c_team_auction_end,
        g2c_team_auction_abandon,

        //battle team
        g2c_battle_team_join,
        g2c_battle_team_add,
        g2c_battle_team_remove,
        g2c_battle_team_change_leader,

        // friend
        g2c_friend_group_list,


        // dungeon
        g2c_scene_finish,
        g2c_sync_dungeon_goal,
        g2c_dungeon_finish_data,
        g2c_dungeon_finish_score,
        g2c_dungeon_reward,

        // title
        g2c_add_title,
        g2c_del_title,

        // trade
        g2c_trade_state_change,
        g2c_trade_set_item,
        g2c_trade_set_money,

        // achievement
        g2c_complete_achievement,
        g2c_compare_achievement,
        g2c_reward_achievement,
        g2c_update_achievement_variable,    // 217
        g2c_activate_hankbook,

        // guild
        g2c_sync_guild_name,
        g2c_guild_buy_aura_ack,
        g2c_guild_draw_salary_ack,
        g2c_guild_contribute_ack,

        // arena
        g2c_arena_sync_data,
        g2c_arena_join_new_season_ack,
        g2c_arena_quit_season_ack,
        g2c_arena_win,
        g2c_arena_defeat,


        // spirit
        g2c_spirit_exp_change,
        g2c_spirit_up,
        g2c_spirit_air_change,
        g2c_spirit_air_levelup,
        g2c_spirit_slot_size_change,
        g2c_spirit_brush_slot,
        g2c_spirit_notify_exp_change_time,
        g2c_spirit_blood_open,

        // hero
        g2c_hire_hero,
        g2c_fire_hero,
        g2c_hero_exp_change,
        g2c_hero_level_up,
        g2c_sync_hero_attr_list,
        g2c_hero_attr_change,
        g2c_change_hero_ai,
        g2c_summon_hero,
        g2c_recall_hero,

        g2c_sync_combat_stat,
        g2c_sync_teleport_platform,
        g2c_active_platform,

        // shield list
        g2c_shield_ack,
        g2c_unshield_ack,

        // Soul fusion
        g2c_upgrade_soul,
        g2c_add_soul,
        g2c_remove_soul,

        // Soul forging
        g2c_rune_item,

        // Equipment upgrade
        g2c_weapon_level_up_put_in,
        g2c_weapon_level_up_take_out,
        g2c_weapon_level_up_result,

        // Equipment decomposition and crafting
        g2c_decompose,
        g2c_compose,

        // refine
        g2c_refine,

        // randomize
        g2c_randomize_item,

        // identify
        g2c_identify_item,

        g2c_draw_debug_line,

        g2c_logout_timer,

        // activity
        g2c_activity_change,
        g2c_activity_reward,

        // vip
        g2c_sync_vip_state,
        g2c_sync_vip_level,

        // sabac
        g2c_sabac_state_change,
        g2c_occupy_sabac,
        g2c_sync_sabac_enroll_list,

        // survey
        g2c_query_survey_path,

        // checkin
        g2c_sync_checkin_info,
        g2c_checkin,

        // black market
        g2c_black_market_limit_info,
        g2c_black_market_cooldown_info,
        g2c_black_market_buy,

        // gamble
        g2c_gamble,

        // exercise
        g2c_exercise_info,
        g2c_add_exercise_time,
        g2c_get_exercise_exp,

        // anti cheat
        g2c_jigsaw,

        g2c_receive_master_reward_exp_ack,

        g2c_spell_book_gamble,

        g2c_sync_arena,
        g2c_arena_status,
        g2c_arena_result,
        g2c_countdown,

        g2c_rune_item_advance,			// Advanced inscription refinement response
        g2c_rune_item_advance_confirm,  // Advanced refinement confirmation response

        g2c_potion_gamble,

        g2c_sell_resource_item,

        g2c_char_wing,                  // wing (back ornament) equipped / removed, broadcast like g2c_char_on_mount

        g2c_wing_info,                  // wing panel data (level / owned wings / worn wing), reply of c2g_wing_op

        g2c_message_end = 512,
    };

    struct G2C_ERROR_NOTIFY : MESSAGE_HEADER
    {
        INT32            nErrCode;
        INT32            nParam1;
        INT32            nParam2;
    };

    struct G2C_SYNC_FRAME : MESSAGE_HEADER
    {
        INT32           nFrame;
    };

    struct G2C_GPK_DYN_CODE : FLEX_MESSAGE_HEADER
    {
        char			DynCode[0];
    };

    struct G2C_GPK_AUTH_DATA : FLEX_MESSAGE_HEADER
    {
        char			AuthData[0];
    };

    struct G2C_ENTER_SCENE : MESSAGE_HEADER
    {
        INT32               nSceneTempID;
        INT32               nSceneID;
        UINT8				byWeather;
        UINT16              wX;
        UINT16              wY;
        UINT16              wZ;
    };

    struct G2C_SYNC_NEXT_JUMP_POINT : MESSAGE_HEADER
    {
        UINT16				wJumpX;
        UINT16				wJumpY;
        UINT16				wJumpZ;
    };

    struct G2C_LEAVE_SCENE : MESSAGE_HEADER
    {
    };

    struct G2C_CHANGESCENE_ENTER : MESSAGE_HEADER
    {
        UINT32           dwGameServerID;
        INT32            nSceneTempID;
        INT32            nSceneID;
        UINT16           wDestX;
        UINT16           wDestY;
        UINT16           wDestZ;
        UINT16           wFaceDirection;
        UINT8			 byIsInJump;
    };

    struct G2C_FRAME_SIGNAL : MESSAGE_HEADER
    {
    };

    struct G2C_SYNC_ROLE_POS : MESSAGE_HEADER
    {
        UINT32				dwRoleID;
        UINT16              wCurrentX;
        UINT16              wCurrentY;
        UINT16              wCurrentZ;
        UINT16              wFaceDirection;
        UINT8               byMoveState;
    };

    struct G2C_PING_ACK : MESSAGE_HEADER
    {
        UINT32              dwPingTick;
    };

    struct G2C_CHAR_WALK : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        UINT16			wSpeed;
        UINT16          wDestX;
        UINT16          wDestY;
    };

    struct G2C_CHAR_RUN : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        UINT16			wSpeed;
        UINT16          wDestX;
        UINT16          wDestY;
    };

    struct G2C_CHAR_STOP : MESSAGE_HEADER
    {
        UINT32          dwCharID;
        UINT8           byForce;
        UINT16          wCurrentX;
        UINT16          wCurrentY;
        UINT16          wCurrentZ;
    };

    struct G2C_CHAR_PASSIVE_MOVE : MESSAGE_HEADER
    {
        UINT32          dwCharID;
        UINT8           byMoveState;
        UINT16          wDestX;
        UINT16          wDestY;
        UINT16          wDestZ;
        UINT16          wFaceDirection;
        UINT16          wDuration;
    };

    struct G2C_CHAR_FREEZE : MESSAGE_HEADER
    {
        UINT32          dwCharID;
        UINT8           byFreezed;
        UINT16          wCurrentX;
        UINT16          wCurrentY;
        UINT16          wCurrentZ;
    };

    struct G2C_CHAR_CLIENT_SHOW_TIME : MESSAGE_HEADER
    {
        UINT32          dwCharID;
        UINT8           byShowTime;
        UINT16          wCurrentX;
        UINT16          wCurrentY;
        UINT16          wCurrentZ;
    };

    struct G2C_CHAR_HARD : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT16           wDuration;
    };

    struct G2C_OBJECT_ROTATE : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT16           wDuration;
        UINT16           wFaceDirection;
    };

    struct G2C_SYNC_FLAG_POS : MESSAGE_HEADER
    {
        UINT8			byFaction;
        UINT32          dwRoleID;
        UINT16          wCurrentX;
        UINT16          wCurrentY;
        UINT16          wCurrentZ;
    };

    struct G2C_SYNC_DOODAD_OPEN_COUNT : MESSAGE_HEADER
    {
        UINT32			dwDoodadID;
        UINT32          dwCharID;
        UINT8			byOpenCount;
    };

    struct SPELL_DATA
    {
        UINT16          wID;
        UINT8			byRuneSlot;
        UINT8           bySpellLevel;
        UINT16          wCurPts;
    };

    struct G2C_SYNC_SPELL_BOOK : FLEX_MESSAGE_HEADER
    {
        SPELL_DATA       astSpellData[0];
    };

    struct SPELL_SLOT_DATA
    {
        UINT8			bySlotID;
        UINT16			wSpellID;
        UINT8			byLocked;
    };

    struct G2C_SYNC_SPELL_SLOT : MESSAGE_HEADER
    {
        SPELL_SLOT_DATA astSlot[qssSlotCount];
    };

    struct G2C_ROLE_LEARN_SPELL : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        UINT16           wSpellID;
        UINT8            bySpellLevel;
    };

    struct G2C_ROLE_REMOVE_SPELL : MESSAGE_HEADER
    {
        UINT16           wSpellID;
    };

    struct G2C_ROLE_LEARN_ITEM_SPELL : MESSAGE_HEADER
    {
        UINT16           wSpellID;
        UINT8            bySpellLevel;
        UINT8			 byItemSlot;
    };

    struct G2C_ROLE_REMOVE_ITEM_SPELL : MESSAGE_HEADER
    {
        UINT16           wSpellID;
        UINT8			 byItemSlot;
    };

    struct G2C_ROLE_LEVEL_UP : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        UINT8            byLevel;
    };

    struct G2C_EXP_CHANGE : MESSAGE_HEADER
    {
        INT32           nExp;
        INT32		    nExpChange;
        INT32           nMaxExp;
        INT32           nTodayExp;
        INT32           nTodayMaxExp;
        INT32           nExtraExp;
    };

    struct G2C_EXTRA_EXP_CHANGE : MESSAGE_HEADER
    {
        INT32			nExtraExp;
    };

    struct G2C_SYNC_GAME_DATA_BEGIN : MESSAGE_HEADER
    {
        UINT32              dwRoleID;
        UINT32              dwSceneTempID;
        UINT32              dwSceneID;
        UINT8               byClass;
        UINT8               bySex;
        UINT8               byLevel;
        UINT8               byPetState;
        UINT32				dwHeroID;
        UINT8               byHeroClass;
        UINT8               byHeroSex;
        UINT8               byHeroLevel;
        UINT8               byHeroAiType;
        INT32				nHeroGrowth;
        char				szHeroName[ROLE_NAME_LEN];
        UINT16              wPosX;
        UINT16              wPosY;
        UINT16              wPosZ;
        UINT16              wFaceDirection;
        UINT8               byMoveState;
        UINT32              dwExp;
        UINT32              dwMaxExp;
        UINT32              dwTodayExp;
        UINT32              dwTodayMaxExp;
        UINT32              dwExtraExp;
        UINT32              dwTeamID;
        UINT32				dwPkValue;
        UINT8				byPkMode;
        UINT32              dwVIPTime[vlTotal];
        UINT32				dwServerTime;
        INT32				nActivityLevel;
        INT32				nActicityReward;
        UINT16				wMountID;
        UINT32				dwServerStartTime;
        UINT16				wMaxLevel;
    };

    struct G2C_SYNC_DATA_ERROR : FLEX_MESSAGE_HEADER
    {
        char                szErrorMsg[0];
    };

    struct G2C_SYNC_CENSORED_WORD_LIST : FLEX_MESSAGE_HEADER
    {
        char				szWordList[0];
    };

    struct G2C_SYNC_GUILD_ACHIEVEMENT : MESSAGE_HEADER
    {
        char                acAchievement[GUILD_MAX_ACHI_ARRAY_LENGTH];
    };

    struct G2C_SYNC_GAME_DATA_END : MESSAGE_HEADER
    {
        UINT32              dwRoleID;
    };

    struct G2C_PLAY_ACTION : MESSAGE_HEADER
    {
        UINT32              dwCharID;
        UINT8				byActionID;
    };

    struct ATTR_DATA : MESSAGE_HEADER
    {
        UINT8            byAttrID;
        INT32            nAttrValue;
    };

    struct G2C_SYNC_ATTR_LIST : FLEX_MESSAGE_HEADER
    {
        ATTR_DATA        astAttrData[0];
    };

    struct G2C_SYNC_ATTR_CHANGE : MESSAGE_HEADER
    {
        UINT8            byAttrID;
        INT32            nAttrValue;
    };

    struct G2C_SYNC_PK_VALUE : MESSAGE_HEADER
    {
        UINT32			dwRoleID;
        UINT32			dwPkValue;
    };

    struct G2C_SYNC_PK_RESULT : MESSAGE_HEADER
    {
        UINT8           byPKResult;
        UINT32          dwKillerRoleID;
        UINT32          dwVictimRoleID;
        INT16           shChangePkValue;
    };

    struct G2C_SYNC_GREY_NAME : MESSAGE_HEADER
    {
        UINT32			dwRoleID;
        UINT8			bGreyName;
    };

    struct G2C_SYNC_EQUIP_TITLE : MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        UINT8           byTitleID;
    };

    struct G2C_SYNC_COMBO_KILL : MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        UINT8			byFaction;
        char			szName[ROLE_NAME_LEN];
        INT32			nComboKill;
    };

    struct G2C_SYNC_MSG_PAOPAO : MESSAGE_HEADER
    {
        INT16           wType;
        UINT32          dwParam1;
    };

    struct G2C_SYNC_ROLE_POW : MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        INT32			nPow;
    };

    struct G2C_SYNC_MASTER_REWARD_EXP : MESSAGE_HEADER
    {
        UINT32          dwMasterID;
        INT32			nStudentRewardExp;
        INT32			nStudentRewardMoney;
        struct TEACHER_REWARD_EXP
        {
            UINT32			dwRoleID;
            INT32			nExp;
            INT32			nMasterExp;
            INT32			nMoney;
        };
        TEACHER_REWARD_EXP	TeacherRewardExp[MAX_STUDENT_NUM];
    };

    struct G2C_CHAR_PRECAST_SPELL : MESSAGE_HEADER
    {
        UINT32           dwCasterID;
        UINT16           wSpellID;
        UINT8            bySpellLevel;
        UINT8            byRuneSlot;
    };

    struct G2C_CHAR_PREPARE_SPELL : MESSAGE_HEADER
    {
        UINT32           dwCasterID;
        UINT16           wSpellID;
        UINT8            bySpellLevel;
        UINT8            byRuneSlot;
        UINT32           dwTargetID;
        UINT16           wTargetX;
        UINT16           wTargetY;
        UINT16           wTargetZ;
        UINT16			 wWeaponSpeedFactor;
        UINT16			 wAtkSpeedFactor;
        UINT8            bySerial;
    };

    struct TARGET_DATA
    {
        UINT32           dwTargetID;
        UINT16           wFlag;
        UINT16           wEffectDelayTime;
    };

    struct SPELL_CAST_INFO
    {
        UINT32           dwCasterID;
        UINT16           wSpellID;
        UINT8            bySpellLevel;
        UINT8            byRuneSlot;
        UINT8            bySerial;
        UINT8            byCastID;

    };

    struct G2C_CHAR_EX_CAST_SPELL : FLEX_MESSAGE_HEADER
    {
        SPELL_CAST_INFO	 SpellCastInfo;
        UINT8			 byEffectSerial;
        UINT8			 byTargetCount;
        TARGET_DATA		 astTargetList[0];
    };

    struct G2C_CHAR_CAST_SPELL : FLEX_MESSAGE_HEADER
    {
        SPELL_CAST_INFO	 SpellCastInfo;
        UINT8			 byTargetCount;
        TARGET_DATA		 astTargetList[0];
    };

    struct G2C_CHAR_SPELL_EFFECT : MESSAGE_HEADER
    {
        SPELL_CAST_INFO	 SpellCastInfo;
        UINT8			 byIsMissileEffect;
        UINT32           dwTargetID;
        INT32            nParam1;
        INT16            shParam2;
        UINT16           wStiffTime;
        UINT16           wFlag;
        UINT8			 byEffectSerial;
    };

    struct G2C_CHAR_BREAK_CAST : MESSAGE_HEADER
    {
        SPELL_CAST_INFO	 SpellCastInfo;
        UINT8			 byIsBreak;
    };

    struct G2C_ROLE_CAST_OVER : MESSAGE_HEADER
    {
        UINT16           wSpellID;
        UINT8            bySerial;
    };

    struct G2C_EQUIP_SPELL : MESSAGE_HEADER
    {
        UINT8			bySlot;
        UINT16          wSpellID;
        UINT8			byRuneSlot;
        UINT8			bySpellLevel;
    };

    struct G2C_EQUIP_RUNE : MESSAGE_HEADER
    {
        UINT16			wSpellID;
        UINT8			byRuneSlot;
        UINT8			bySpellLevel;
    };

    struct G2C_UNLOCK_SPELL_SLOT : MESSAGE_HEADER
    {
        UINT8			bySlot;
    };

    struct G2C_LEECH_SOUL : MESSAGE_HEADER
    {
        UINT32			dwFromObjID;
        UINT32			dwToObjID;
        INT32			nSoulPts;
    };

    struct G2C_DIRECTLY_HP_CHANGE_EFFECT : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        INT32			nHpChange;
    };

    struct G2C_COMBAT_POSE_SWITCH : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        UINT8			byCombatPoseOn;
        UINT8			byClientFlag;
    };

    struct G2C_CLEAR_SPELL_CACHE : MESSAGE_HEADER
    {
        INT32			nTag;
    };

    struct AURA_DATA
    {
        UINT16           wID;
        UINT32           dwInstanceID;
        UINT8            byStackCount;
        UINT32           dwRemainDuration;
        UINT32			 dwMaxDuration;
    };

    struct G2C_SYNC_AURA_LIST : FLEX_MESSAGE_HEADER
    {
        AURA_DATA		astAuraData[0];
    };

    struct G2C_NEW_AURA : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT16           wAuraID;
        UINT32           dwInstanceID;
        UINT32			 dwCasterID;
        UINT32           dwDuration;
        UINT8            byStackCount;
    };

    struct G2C_DEL_AURA : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT32           dwInstanceID;
    };

    struct G2C_AURA_CHANGE : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        AURA_DATA		 AuraData;
    };

    struct G2C_AURA_EFFECT : MESSAGE_HEADER
    {
        UINT32           dwInstanceID;
        UINT16           wAuraID;
        UINT32			 dwCasterID;
        UINT32			 dwTargetID;
        INT32			 nEffectValue;
    };

    struct G2C_CHAR_DIE : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT8            byType;
    };

    struct G2C_CHAR_REVIVE : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT8            byType;
    };

    //	struct G2C_REVIVE_RESPONSE : MESSAGE_HEADER
    //	{
    //		INT32			nDelayTime;
    //	};

    struct REVIVE_INFO
    {
        UINT8			byReviveType;
        INT32			nRequireItemID;
        INT32			nRequireItemCount;
    };

    struct G2C_REVIVE_INFO : MESSAGE_HEADER
    {
        REVIVE_INFO		ReviveInfo[crtTotal];
        INT32			nWaitTime;
        UINT32			dwKillerID;
    };

    struct G2C_ACTIVATE_REVIVE_PT : MESSAGE_HEADER
    {
        INT32			nX;
        INT32			nY;
        INT32			nZ;
    };

    struct G2C_CHAR_MINE : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        UINT16			wDestX;
        UINT16			wDestY;
        UINT16			wMineInterval;
    };

    struct G2C_SYNC_HP : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        INT32           nCurrentHp;
        INT32			nMaxHp;
    };

    struct G2C_SYNC_MP : MESSAGE_HEADER
    {
        INT32           nCurrentMp;
    };

    struct G2C_CHAR_SELECT_FOCUS : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        UINT32           dwFocusID;
    };

    struct G2C_FOCUS_INFO : FLEX_MESSAGE_HEADER
    {
        UINT32           dwFocusID;
        INT32            nCurrentHp;
        INT32            nCurrentMp;
        INT32            nMaxHp;
        INT32            nMaxMp;
        UINT8            byAuraCount;
        AURA_DATA        astAuraData[0];
    };

    struct G2C_ADD_COOLDOWN : MESSAGE_HEADER
    {
        UINT32           dwCooldownID;
        UINT32           dwCooldownValue;
    };

    struct G2C_SYNC_PK_MODE : MESSAGE_HEADER
    {
        UINT32			dwRoleID;
        UINT8			byPkMode;
    };

    struct G2C_ENTER_COMBAT : MESSAGE_HEADER
    {
        UINT32          dwCharID;
    };

    struct G2C_LEAVE_COMBAT : MESSAGE_HEADER
    {
        UINT32          dwCharID;
    };

    struct G2C_SYNC_FACTION : MESSAGE_HEADER
    {
        UINT32			dwRoleID;
        UINT8			byFaction;
    };

    struct G2C_ADDTO_THREATLIST : MESSAGE_HEADER
    {
        UINT32 dwCharID;
    };

    struct G2C_DELFROM_THREATLIST : MESSAGE_HEADER
    {
        UINT32 dwCharID;
    };

    //2019-6-9
    struct G2C_SYNC_ARENA_SIMPLE_INFO : MESSAGE_HEADER
    {
        INT32       nTime;
    };

    struct CHAR_APPEAR_DATA
    {
        UINT32          dwCharID;
        UINT8           byMoveState;
        UINT16          wCurrentX;
        UINT16          wCurrentY;
        UINT16          wCurrentZ;
        UINT16          wFaceDirection;
        UINT8           byLifePercent;
        UINT8			byMountID;
        unsigned char	byCombatPoseOn : 1;
        unsigned char	bGreyName : 1;
        unsigned char   bPet : 1;
        unsigned char	uReserved : 5;
        UINT16          wWingID;
    };

    struct AURA_BRIEF_DATA
    {
        UINT16			wTemplateID;
        UINT32			dwInstanceID;
    };

    struct G2C_SYNC_AURA_BRIEF : MESSAGE_HEADER
    {
        UINT32				dwCharID;
        AURA_BRIEF_DATA		BriefData[adgTotal];
    };

    struct G2C_SYNC_PET_LEVEL : MESSAGE_HEADER
    {
        UINT32          dwNpcID;
        UINT8           byLevel;
    };

    struct G2C_SYNC_NPC_TRANSFORM_STATE : MESSAGE_HEADER
    {
        UINT32          dwNpcID;
        UINT8           byTransformState;
    };

    struct G2C_CHAR_APPEAR : MESSAGE_HEADER
    {
        UINT8               byAppearType;
        CHAR_APPEAR_DATA    stAppearData;
    };

    struct G2C_SYNC_CHAR_LIST : FLEX_MESSAGE_HEADER
    {
        CHAR_APPEAR_DATA astAppearData[0];
    };

    struct G2C_OBJECT_DISAPPEAR : MESSAGE_HEADER
    {
        UINT32              dwObjID;
        UINT8               byDisappearType;
    };

    struct G2C_OBJECT_DISAPPEAR_LIST : FLEX_MESSAGE_HEADER
    {
        UINT32           dwObjIDList[0];
    };

    struct MISSILE_APPEAR_DATA : MESSAGE_HEADER
    {
        UINT32           dwMissileID;
        UINT32           dwOwnerID;
        UINT16           wTemplateID;
        UINT16           wCurrentX;
        UINT16           wCurrentY;
        UINT16           wCurrentZ;
        UINT16			 wLifeFrm;
        UINT16           wFaceDirection;
        UINT16           wDestX;
        UINT16           wDestY;
        UINT16           wDestZ;
    };

    struct G2C_MISSILE_APPEAR : MESSAGE_HEADER
    {
        MISSILE_APPEAR_DATA stAppearData;
    };

    struct G2C_MISSILE_MOVE : MESSAGE_HEADER
    {
        UINT32          dwMissileID;
        UINT16          wRemainFrm;
        INT32			nFaceDirection;
        UINT16          wDestX;
        UINT16          wDestY;
        UINT16          wDestZ;
    };

    struct G2C_SYNC_MISSILE_LIST : FLEX_MESSAGE_HEADER
    {
        MISSILE_APPEAR_DATA astAppearData[0];
    };

    struct G2C_SPELL_LEVEL_UP : MESSAGE_HEADER
    {
        UINT16			wSpellID;
        UINT8			byLevel;
    };

    struct G2C_SYNC_SPELL_PTS : MESSAGE_HEADER
    {
        UINT16			wSpellID;
        UINT16			wPts;
    };

    struct G2C_CHAR_TELEPORT : MESSAGE_HEADER
    {
        UINT32           dwCharID;
        INT32            nSceneID;
        UINT16           wDestX;
        UINT16           wDestY;
        UINT16           wDestZ;
    };

    struct G2C_SYNC_ROLE_DATA : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        char			 szName[ROLE_NAME_LEN];
        UINT8            byClass;
        UINT8            bySex;
        UINT8            byHairType;
        UINT8            byHairColor;
        UINT8            byFaceType;
        UINT8            bySkinColor;
        UINT8			 byFaction;
        UINT32			 dwPkValue;
        UINT8            byVIPLevel;

        UINT8            byRefineFx;
        INT32            anAvatar[NES_VISIBLE_TOTAL];

        INT32            nMaxHp;
        INT32            nMaxMp;
        UINT32           dwTeamID;
        UINT8			 byPkMode;
        UINT8            byTitleID;
        UINT32			 dwCreateTime;

        UINT8            byStallState;
        char             szStallName[STALL_NAME_LEN];
        UINT8            byStallLevel;

        UINT8            byMasterState;
        UINT32           dwGuildID;
    };

    struct G2C_SYNC_NPC_DATA : MESSAGE_HEADER
    {
        UINT32              dwNpcID;
        INT32               nTemplateID;
        UINT8               byQuality;
        UINT8			    byLevel;
        INT32               nMaxHp;
    };

    struct G2C_SYNC_NPC_EX_DATA : G2C_SYNC_NPC_DATA
    {
        UINT8               byType;             //NPC_TRANSFORM_STATE
        UINT8               byPetLevel;
        UINT32              dwHostID;
        char                szHostName[ROLE_NAME_LEN];
    };

    struct G2C_SYNC_HERO_DATA : MESSAGE_HEADER
    {
        UINT32           dwHeroID;
        char			 szName[ROLE_NAME_LEN];
        char			 szMasterName[ROLE_NAME_LEN];
        UINT8            byClass;
        UINT8            bySex;
        UINT8            byLevel;

        INT32            anAvatar[NES_VISIBLE_TOTAL];

        INT32            nMaxHp;
        INT32            nMaxMp;
    };

    struct G2C_SYNC_MONEY : MESSAGE_HEADER
    {
        INT64            allMoneys[MONEYTYPE_COUNT];
        INT64            allMoneysLimit[MONEYTYPE_COUNT];

        INT32            anDailyEarned[MONEYTYPE_COUNT];
        INT32            anDailyEarnedLimit[MONEYTYPE_COUNT];

        INT32            anDailyUsed[MONEYTYPE_COUNT];
        INT32            anDailyUsedLimit[MONEYTYPE_COUNT];

        INT32            anWeeklyEarned[MONEYTYPE_COUNT];
        INT32            anWeeklyEarnedLimit[MONEYTYPE_COUNT];

        INT32            anWeeklyUsed[MONEYTYPE_COUNT];
        INT32            anWeeklyUsedLimit[MONEYTYPE_COUNT];

        UINT8            byUseMoneytype01Only;
    };

    struct G2C_SYNC_CONTAINER_SIZES : MESSAGE_HEADER
    {
        UINT8            abySizes[CONSLOT_COUNT];
    };

    struct G2C_SYNC_ROLE_TEAM : MESSAGE_HEADER
    {
        UINT32 dwRoleID;
        UINT32 dwTeamID;
    };

    struct G2C_TEAM_AUCTION_ADD_ITEM : FLEX_MESSAGE_HEADER
    {
        UINT32	dwAuctionIndex;
        char	acItemInfo[0];
    };

    struct G2C_TEAM_AUCTION_BEGIN : MESSAGE_HEADER
    {
        UINT32	dwAuctionIndex;
        UINT32	dwBottomPrice;
        UINT8	byBidderCount;
        UINT32	dwRemainTime;
    };

    struct G2C_TEAM_AUCTION_NOTIFY : MESSAGE_HEADER
    {
        UINT32	dwAuctionIndex;
        UINT32	dwBidderRoleID;
        UINT32	dwOfferPrice;
        UINT32	dwRemainTime;
    };

    struct G2C_TEAM_AUCTION_ABANDON : MESSAGE_HEADER
    {
        UINT32	dwAuctionIndex;
        UINT32	dwRoleID;
    };

    struct G2C_TEAM_AUCTION_END : MESSAGE_HEADER
    {
        UINT32	dwAuctionIndex;
        UINT32	dwWinnerRoleID;
        UINT32	dwTax;
    };

    struct G2C_SYNC_ROLE_ATTR_DATA : FLEX_MESSAGE_HEADER
    {
        UINT32			 dwRoleID;
        INT32			 nFatigue;
        ATTR_DATA        astAttrData[0];
    };

    struct G2C_SYNC_ROLE_EQUIP_DATA : FLEX_MESSAGE_HEADER
    {
        UINT32			dwRoleID;
        UINT16          awRefineLevels[NES_TOTAL];
        UINT8           byItemCount;
        char            acItems[0];
    };

    struct G2C_SYNC_ROLE_SPIRIT_DATA : FLEX_MESSAGE_HEADER
    {
        UINT32			dwRoleID;
        char            szData[0];
    };

    struct G2C_SYNC_ROLE_SPELL_DATA : FLEX_MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        SPELL_DATA      SpellData[0];
    };

    struct G2C_SYNC_SHIELD_LIST : FLEX_MESSAGE_HEADER
    {
        UINT8   byRoleCount;
        UINT32  arrRoleID[0];
    };

    struct G2C_SYNC_REFINE : MESSAGE_HEADER
    {
        UINT16  awRefineLevel[NES_TOTAL];
    };

    struct G2C_SYNC_ITEM_BAG : FLEX_MESSAGE_HEADER
    {
        UINT8           byShowHat;
        UINT8           byItemCount;
        char            acItems[0];
    };

    struct G2C_ITEM_CHANGE : FLEX_MESSAGE_HEADER
    {
        char            acItem[0];
    };

    struct G2C_SIGN_ITEM : MESSAGE_HEADER
    {
        UINT8			byConSlot;
        UINT8			byItemSlot;
        UINT8			bySign[64];
    };

    struct G2C_LUCK_OIL : MESSAGE_HEADER
    {
        INT8            chResult;   //-1,0,1
    };

    struct NPC_SHOP_DATA
    {
        INT32 m_nShopID;
        char m_szShopName[SHOP_NAME_LEN];
        INT32 m_nItemID;
        INT32 m_nItemCount;
        INT32 m_nCostItemID1;
        INT32 m_nCostValue1;
        INT32 m_nCostItemID2;
        INT32 m_nCostValue2;
        INT32 m_nRequireForceID;
        INT32 m_nReputation;
        INT32 m_nRequireGuildBuildingID;
        INT32 m_nBuildingLevel;
        INT32 m_nShopTypeSell;
    };

    struct G2C_SYNC_NPC_SHOP_DATA : FLEX_MESSAGE_HEADER
    {
        UINT32      dwCrc;
        UINT32      dwItemCount;
        NPC_SHOP_DATA ShopData[0];
    };

    struct G2C_ITEM_SWAP : MESSAGE_HEADER
    {
        UINT8           bySrcConSlot;
        UINT8           bySrcItemSlot;
        UINT8           byDstConSlot;
        UINT8           byDstItemSlot;
    };

    struct G2C_ITEM_DELETE : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byItemSlot;
    };

    struct G2C_DESTROY_DEATH_ITEM : FLEX_MESSAGE_HEADER
    {
        char            acItem[0];
    };

    struct G2C_LOSE_PK_ITEM : FLEX_MESSAGE_HEADER
    {
        char            acItem[0];
    };

    struct G2C_VIEW_SOLD_ITEMS : FLEX_MESSAGE_HEADER
    {
        UINT8           byCount;
        char            acItems[0];
    };

    struct G2C_REPAIR_ITEM : MESSAGE_HEADER
    {
    };

    struct G2C_PICKUP_ITEM : FLEX_MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        char            acItem[0];
    };

    struct G2C_PICKUP_MONEY : MESSAGE_HEADER
    {
        INT32           nMoneyType;
        INT32           nCount;
    };

    struct G2C_CONTAINER_SIZE_CHANGE : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byConSize;
    };

    struct G2C_MONEY_CHANGE : MESSAGE_HEADER
    {
        INT32           nMoneyType;
        INT64           llMoney;
        INT32           nDailyEarned;
        INT32           nDailyUsed;
        INT32           nWeeklyEarned;
        INT32           nWeeklyUsed;
    };

    struct G2C_EQUIP_DUR_CHANGE : MESSAGE_HEADER
    {
        UINT8           byConSlot;
        UINT8           byEquipSlot;
        INT32           nDur;
    };

    struct G2C_ACTIVATE_MOUNT : MESSAGE_HEADER
    {
        INT32			nMountID;
    };

    struct G2C_CHAR_ON_MOUNT : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        UINT8			byMountID;
    };

    struct G2C_CHAR_OFF_MOUNT : MESSAGE_HEADER
    {
        UINT32			dwCharID;
    };

    struct G2C_CHAR_WING : MESSAGE_HEADER
    {
        UINT32			dwCharID;
        UINT16			wWingID;    // 0: no wing
    };

    // wing panel (1.05 UnrealWing)
    struct G2C_WING_INFO : MESSAGE_HEADER
    {
        INT32			nWingLevel;     // wings_level.txt level
        UINT32			dwOwnedMask;    // bit N: wing id (39501 + N) is owned
        UINT16			wCurrentWing;   // worn wing id, 0: none
        UINT8			byUpgraded;     // 1: the level has just been raised (play the level up effect)
    };

    struct G2C_SYNC_DOOR_STATE : MESSAGE_HEADER
    {
        UINT64           qwDoorState;
    };

    struct G2C_SYNC_SCENE_VARIABLES : FLEX_MESSAGE_HEADER
    {
        struct VARIABLE_DATA
        {
            UINT16          wVarIndex;
            INT32           nValue;
        };
        VARIABLE_DATA   VarList[0];
    };

    struct G2C_UPDATE_VARIABLE : MESSAGE_HEADER
    {
        UINT8           byType;
        UINT16          wIndex;
        INT32           nReferID;
        INT32           nValue;
    };

    struct G2C_UPDATE_KILL_BUF_PLAYER_COUNT : MESSAGE_HEADER
    {
        INT32			nQuestID;
        INT32			nCount;
    };

    struct G2C_UPDATE_WEATHER : MESSAGE_HEADER
    {
        UINT8           byWeather;
    };

    struct G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT : MESSAGE_HEADER
    {
        UINT8			byCount;
    };

    struct G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT : MESSAGE_HEADER
    {
        UINT8			byCount;
    };

    struct G2C_UPDATE_CONSECUTIVE_KILL_NPC_END : MESSAGE_HEADER
    {
        UINT8			byCount;
        INT32			nExp;
    };

    struct G2C_UPDATE_MOMENTARY_KILL_NPC_END : MESSAGE_HEADER
    {
        UINT8			byCount;
        INT32			nExp;
    };

    struct G2C_SYNC_CLIENT_VARIABLES : MESSAGE_HEADER
    {
        INT32           nVariableList[MAX_CLIENT_VARIABLE_COUNT];
    };

    struct G2C_SYNC_ROLE_VARIABLES : FLEX_MESSAGE_HEADER
    {
        struct VARIABLE_DATA
        {
            UINT16          wVarIndex;
            INT32           nValue;
        };
        VARIABLE_DATA   VarList[0];
    };

    struct G2C_CHAT_MESSAGE : FLEX_MESSAGE_HEADER
    {
        UINT32			dwTargetID;
        UINT32			dwSrcID;
        unsigned		uLocation : 4;
        unsigned		uReserved : 4;
        char            szMessage[0];
    };

    struct G2C_SYNC_ROLE_AVATAR : MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        UINT8           byConSlot;
        UINT8           byItemSlot;
        INT32           nTemplateID;
        UINT8           byRefineFx;
    };

    struct G2C_SYNC_HERO_AVATAR : MESSAGE_HEADER
    {
        UINT32           dwHeroID;
        UINT8            byConSlot;
        UINT8            byItemSlot;
        INT32            nTemplateID;
    };

    struct DROP_INFO
    {
        UINT32          dwDropID;
        UINT16          wDestX;
        UINT16          wDestY;
        UINT16          wDestZ;
        INT32           nTemplateID;
        INT32           nCount;
    };

    struct G2C_GENERATE_DROP_FROM_OBJECT : MESSAGE_HEADER
    {
        UINT32          dwSrcID;
        DROP_INFO       drop;
    };

    struct G2C_SYNC_DROP_LIST : FLEX_MESSAGE_HEADER
    {
        UINT8           byIsNew;
        INT32           nCount;
        DROP_INFO       astDrops[0];
    };

    /************************************************************************/
    // Doodad
    struct DOODAD_APPEAR_DATA
    {
        UINT32              dwDoodadID;
        INT32               nTemplateID;
        UINT16              wCurrentX;
        UINT16              wCurrentY;
        UINT16              wCurrentZ;
        UINT16              wFaceDirection;
        BYTE                byRemainOpenCount;
    };

    struct G2C_DOODAD_APPEAR : MESSAGE_HEADER
    {
        DOODAD_APPEAR_DATA	AppearData;
    };

    struct G2C_SYNC_DOODAD_LIST : FLEX_MESSAGE_HEADER
    {
        DOODAD_APPEAR_DATA astAppearData[0];
    };

    struct G2C_OPERATE_DOODAD_BEGIN : MESSAGE_HEADER
    {
        UINT32          dwCharID;
        UINT32          dwDoodadID;
        UINT16          wDuration;
    };

    struct G2C_OPERATE_DOODAD_END : MESSAGE_HEADER
    {
        UINT32          dwCharID;
        UINT32          dwDoodadID;
        UINT8           byIsBreak;
    };

    /************************************************************************/

    struct G2C_SYNC_STALL : MESSAGE_HEADER
    {
        UINT32           dwUpgradeExpireTime;
    };

    struct G2C_STALL_STATE_CHANGE : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        UINT8            btStallState;
    };

    struct G2C_ADD_STALL_ITEM : MESSAGE_HEADER
    {
        UINT8            btStallSlot;
        UINT8            byConSlot;
        UINT8            byItemSlot;
        INT16            shItemCount;
        INT32            nPrice;
    };

    struct G2C_REMOVE_STALL_ITEM : MESSAGE_HEADER
    {
        UINT8            btStallSlot;
    };

    struct G2C_CHANGE_STALL_NAME : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        char             szStallName[STALL_NAME_LEN];
    };

    struct G2C_CHANGE_STALL_LEVEL : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        UINT8            byStallLevel;
    };

    struct STALL_ITEM_INFO
    {
        UINT8            btStallSlot;
        INT32            nPrice;
        INT64            llModifyFrame;
        char             acItem[0];
    };

    struct G2C_VIEW_STALL_ITEM : FLEX_MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        UINT8            btStallItemCount;
        char             acStallItems[0];
    };

    struct G2C_BUY_STALL_ITEM : MESSAGE_HEADER
    {
        UINT32           dwRoleID;
        UINT8            btStallSlot;
        INT32            nLeftItemCount;
    };

    struct G2C_SOLD_STALL_ITEM : MESSAGE_HEADER
    {
        UINT8            btStallSlot;
        INT32            nItemCount;
        INT32            nTotalIncome;
    };

    /************************************************************************/
    // quest
    struct G2C_SYNC_QUEST_LIST : FLEX_MESSAGE_HEADER
    {
        char            szData[0];
    };

    struct G2C_SYNC_KILL_NPC_QUEST : MESSAGE_HEADER
    {
        UINT16			wKillCount[MAX_KILL_NPC_QUEST_COUNT];
        UINT8			byCompleteList[MAX_KILL_NPC_QUEST_COUNT];
    };

    struct G2C_ACCEPT_QUEST : MESSAGE_HEADER
    {
        INT32               nQuestID;
    };

    struct G2C_PUBLISH_QUEST : MESSAGE_HEADER
    {
        INT32				nQuestID;
    };

    struct G2C_ABANDON_QUEST : MESSAGE_HEADER
    {
        INT32               nQuestID;
    };

    struct G2C_COMPLETE_QUEST : MESSAGE_HEADER
    {
        INT32               nQuestID;
    };

    struct G2C_SHARE_QUEST : MESSAGE_HEADER
    {
        INT32				nQuestID;
        INT32				nSrcID;
    };

    struct G2C_NOTIFY_NEXT_CIRCLE_QUEST : MESSAGE_HEADER
    {
        INT32				nCircleID;
        INT32				nQuestID;
    };

    struct G2C_BUY_DAILY_QUEST_COMPLETE_TIME : MESSAGE_HEADER
    {
        INT32				nCurrentRemainTime;
    };

    struct G2C_BRUSH_DAILY_QUEST : MESSAGE_HEADER
    {
        BOOL                bClearCDTime;
        INT32				nQuestID[MAX_REWARD_QUEST_COUNT_PER_ROLE];
    };

    struct G2C_QUEST_RECYCLE : MESSAGE_HEADER
    {
        INT32			nTemplateID;
        INT32			nIndex;
    };

    struct G2C_SET_QUEST_FAILED : MESSAGE_HEADER
    {
        INT32				nQuestID;
    };

    struct G2C_SYNC_KILL_NPC_COUNT : MESSAGE_HEADER
    {
        UINT16			wQuestID;
        UINT16			wKillCount;
    };

    struct G2C_COMPLETE_KILL_NPC_QUEST : MESSAGE_HEADER
    {
        UINT16			wQuestID;
        UINT8			byIndex;
    };

    struct G2C_RESET_KILL_NPC_QUEST : MESSAGE_HEADER
    {
        UINT16			wQuestID;
    };

    /************************************************************************/

    struct G2C_INTERACT_RESULT : FLEX_MESSAGE_HEADER
    {
        UINT32          dwNpcID;
        char            szDialogContent[0];
    };

    struct REPUTATION_INFO : MESSAGE_HEADER
    {
        UINT8            byForceID;
        INT16            shReputation;
    };

    struct G2C_SYNC_REPUTATION : MESSAGE_HEADER
    {
        UINT64				qwActiveState;
        UINT64				qwAwardState;
        REPUTATION_INFO		ReputationTable[MAX_NPC_FORCE_ID];
    };

    struct G2C_MODIFY_REPUTATION : MESSAGE_HEADER
    {
        UINT8            byForceID;
        INT32            nReputation;
    };

    struct G2C_SYNC_TEAMMEMBER : MESSAGE_HEADER
    {
        UINT32           dwTeamID;
        UINT32           dwMemberID;
        UINT8            byLevel;
        INT32            nMaxHp;
        INT32            nMaxMp;
        INT32            nCurHp;
        INT32            nCurMp;
    };

    struct G2C_TEAM_SYNC_MEMBER : MESSAGE_HEADER
    {
        UINT32              dwRoleID;
        INT32               nLevel;
        INT32               nMaxHp;
        INT32               nMaxMp;
        INT32               nCurHp;
        INT32               nCurMp;
        UINT16              wX;
        UINT16              wY;
        UINT16              wZ;
    };

    struct GS_TEAM_MEMBER_BASEINFO
    {
        UINT32                  dwRoleID;
        char                    szRoleName[ROLE_NAME_LEN];
        UINT8                   bySex;
        UINT8					byLevel;
        UINT8                   byClass;
    };

    struct G2C_BATTLE_TEAM_JOIN : FLEX_MESSAGE_HEADER
    {
        UINT64						dwBattleTeamID;
        UINT8						byMemberCount;
        GS_TEAM_MEMBER_BASEINFO		lstRoleMember[0];
    };

    struct G2C_BATTLE_TEAM_ADD : MESSAGE_HEADER
    {
        UINT64						dwBattleTeamID;
        GS_TEAM_MEMBER_BASEINFO     stMember;
    };

    struct G2C_BATTLE_TEAM_REMOVE : MESSAGE_HEADER
    {
        UINT64			dwBattleTeamID;
        UINT32          dwRoleID;
    };

    struct G2C_BATTLE_TEAM_CHANGE_LEADER : MESSAGE_HEADER
    {
        UINT32			dwNewLeader;
    };

    struct ADJACENT_ROLE
    {
        UINT32 dwRoleID;
        CHAR   szRoleName[ROLE_NAME_LEN];
        UINT8  btSex;
        UINT8  btClass;
    };

    struct G2C_SYNC_ADJACENT_ROLE_NOTEAM : FLEX_MESSAGE_HEADER
    {
        UINT8            byCount;
        ADJACENT_ROLE    stRoleList[0];
    };

    struct ADJACENT_TEAM_ITEM
    {
        UINT32 dwTeamID;
        CHAR   szTeamName[TEAM_NAME_LEN];
        CHAR   szCaptainName[ROLE_NAME_LEN];
        UINT32 dwCaptainID;
        UINT8  btCaptainLevel;
        UINT8  btCaptainClass;
        UINT8  btMemberCount;
        UINT8  btMaxCount;
        UINT8  btActivityType;
        UINT32 dwActivityID;
    };

    struct G2C_SYNC_ADJACENT_TEAM : FLEX_MESSAGE_HEADER
    {
        UINT8                 byCount;
        ADJACENT_TEAM_ITEM    stTeamList[0];
    };

    struct G2C_QUERY_SCENE_DIFFICULTY : MESSAGE_HEADER
    {
        UINT32          dwSceneTemplateID;
        UINT8           byDungeonDifficultyMask;
        UINT8           byDungeonEnterCount;
    };

    struct G2C_SELECT_SCENE_DIFFICULTY : MESSAGE_HEADER
    {
        UINT8            bySelectedDifficulty;
    };

    struct G2C_RETURN_SELECT_ROLE : MESSAGE_HEADER
    {
        INT32            nErrCode;
        UINT32           dwRoleID;
    };

    struct FRIEND_GROUP_INFO
    {
        UINT8                   byGroupID;
        char                    szGroupName[FRIEND_GROUP_NAME_LEN];
    };

    struct G2C_FRIEND_GROUP_LIST : FLEX_MESSAGE_HEADER
    {
        UINT8                   byGroupCount;
        FRIEND_GROUP_INFO       stGroupInfo[0];
    };

    struct G2C_SCENE_FINISH : MESSAGE_HEADER
    {
    };

    struct G2C_DUNGEON_FINISH_DATA : MESSAGE_HEADER
    {
        INT32           nTotalTime;
        UINT8           byTimeScore;
        INT32           nKillNpcCount;
        INT32           nDestroyDoodadCount;
        UINT8           byAttackScore;
        UINT8           byGoalResult;
        UINT8           byGoalScore;
        UINT32          byTotalScore;

        INT32           nNormalExp;
        INT32           nTeamExp;
        INT32           nGuildExp;
        INT32           nMasterExp;
        INT32           nVipExp;
        INT32           nFirstPassExp;
        INT32           nActivityExp;
        INT32           nTotalExp;
    };

    struct G2C_SYNC_DUNGEON_GOAL : MESSAGE_HEADER
    {
        INT32           nBossKillCount;
        INT32           nEliteNpcKillCount;
    };

    struct SCENE_SCORE
    {
        UINT32          dwRoleID;
        UINT32          dwTotalScore;
    };

    struct G2C_DUNGEON_FINISH_SCORE : FLEX_MESSAGE_HEADER
    {
        UINT8           byCount;
        SCENE_SCORE     stScore[0];
    };

    struct G2C_DUNGEON_REWARD : MESSAGE_HEADER
    {
        UINT32          dwRoleID;
        UINT8           byRewardType;

        UINT32          dwRewardID;
        UINT32          dwRewardCount;
    };

    // --------------- title ---------------------------
    struct TITLE_DATA
    {
        UINT8       m_nTemplateID;
        INT32       m_nExpireTime;
    };

    struct G2C_SYNC_TITLE : FLEX_MESSAGE_HEADER
    {
        UINT8       nEquipedTitleID;
        UINT8       nTitleCount;
        TITLE_DATA  stTitleList[0];
    };

    struct G2C_SYNC_MOUNT_LIST : FLEX_MESSAGE_HEADER
    {
        UINT8	byMountID[0];
    };

    struct G2C_ADD_TITLE : MESSAGE_HEADER
    {
        UINT8       nTitleID;
        INT32       nExpireTime;
    };


    struct G2C_DEL_TITLE : MESSAGE_HEADER
    {
        UINT8       nTitleID;
        UINT8       nDelType;
    };

    // master
    struct G2C_SYNC_MASTER : MESSAGE_HEADER
    {
        UINT32      dwCD;
        UINT8       byState;
        UINT8       byTwitter;
    };

    struct G2C_SYNC_GUILD : MESSAGE_HEADER
    {
        UINT32      dwCD;
    };

    struct G2C_TRADE_STATE_CHANGE : MESSAGE_HEADER
    {
        UINT32      dwRoleID;
        UINT8       byTradeState;
        INT32       nRoleLevel;
    };

    struct G2C_TRADE_SET_ITEM : FLEX_MESSAGE_HEADER
    {
        UINT32      dwRoleID;
        UINT8       byTradeSlot;
        UINT8       bAdd;
        char        acItem[0];
    };

    struct G2C_TRADE_SET_MONEY : MESSAGE_HEADER
    {
        UINT32      dwRoleID;
        INT32       nMoney;
    };

    struct G2C_HIRE_HERO : MESSAGE_HEADER
    {
        UINT32		dwHeroID;
        UINT8		byHeroClass;
        UINT8		byHeroSex;
        UINT8		byHeroLevel;
        UINT8		byHeroAiType;
        INT32		nGrowth;
    };

    struct G2C_FIRE_HERO : MESSAGE_HEADER
    {
        UINT32           dwHeroID;
    };

    struct G2C_HERO_EXP_CHANGE : MESSAGE_HEADER
    {
        UINT32           dwHeroID;
        UINT32           dwExp;
        INT32			 nExpChange;
        UINT32           dwMaxExp;
    };

    struct G2C_HERO_LEVEL_UP : MESSAGE_HEADER
    {
        UINT32           dwHeroID;
        UINT8            byLevel;
    };

    struct G2C_SYNC_HERO_ATTR_LIST : FLEX_MESSAGE_HEADER
    {
        UINT32			dwHeroID;
        ATTR_DATA       astAttrData[0];
    };

    struct G2C_HERO_ATTR_CHANGE : MESSAGE_HEADER
    {
        UINT8            byAttrID;
        INT32            nAttrValue;
    };

    struct G2C_CHANGE_HERO_AI : MESSAGE_HEADER
    {
        UINT32			dwHeroID;
        INT32			nAiType;
    };

    struct G2C_SUMMON_HERO : MESSAGE_HEADER
    {
    };

    struct G2C_RECALL_HERO : MESSAGE_HEADER
    {
    };

    struct G2C_SYNC_COMBAT_STAT : FLEX_MESSAGE_HEADER
    {
        struct ROLE_STAT_DATA
        {
            UINT32 dwRoleID;
            UINT8 byFaction;
            INT32 Data[escsTotal];
        };
        ROLE_STAT_DATA  DataList[0];
    };

    //----------------- achievement ----------------------
    struct G2C_COMPLETE_ACHIEVEMENT : MESSAGE_HEADER
    {
        INT32		nAchievementID;
        UINT32		dwCompleteTime;
    };

    struct G2C_COMPARE_ACHIEVEMENT : FLEX_MESSAGE_HEADER
    {
        UINT32		dwRoleID;
        INT32		nCount;
        char		acAchievement[0];
    };

    struct G2C_SYNC_ACHIEVEMENT_LIST : FLEX_MESSAGE_HEADER
    {
        INT32		nCount;
        char        szData[0];
    };

    struct G2C_REWARD_ACHIEVEMENT : MESSAGE_HEADER
    {
        INT32		nAchievementID;
    };

    struct G2C_UPDATE_ACHIEVEMENT_VARIABLE : MESSAGE_HEADER
    {
        INT32		nIndex;
        INT32		nValue;
    };

    struct G2C_ACTIVATE_HANKBOOK : MESSAGE_HEADER
    {
        INT32		nType;
        INT32		nIndex;
    };

    // guild
    struct G2C_SYNC_GUILD_NAME : MESSAGE_HEADER
    {
        UINT32      dwRoleID;
        UINT32      dwGuildID;
    };

    struct G2C_GUILD_BUY_AURA_ACK : MESSAGE_HEADER
    {
        INT8        byType;
        UINT8       byCount;
    };

    struct GUILD_BUY_AURA_RECORD_INFO
    {
        INT32           nType;
        UINT8           byCount;
    };

    struct G2C_SYNC_PURCHASE_AURA_RECORD : FLEX_MESSAGE_HEADER
    {
        INT16       wCount;
        GUILD_BUY_AURA_RECORD_INFO        data[0];
    };

    struct G2C_GUILD_DRAW_SALARY_ACK : MESSAGE_HEADER
    {
        INT32       nSalary;
    };

    struct G2C_GUILD_CONTRIBUTE_ACK : MESSAGE_HEADER
    {
        INT32       nMoney;
        INT32       nContribution;
    };

    // arena
    struct G2C_SYNC_ARENA : MESSAGE_HEADER
    {
        INT32       nTime;
    };

    struct G2C_ARENA_STATUS : MESSAGE_HEADER
    {
        INT32       nStatus;
    };

    struct G2C_ARENA_RESULT : MESSAGE_HEADER
    {
        UINT8       byWin;
        INT32       nTime;
        INT32       nDmg;
        INT32       nRewardMoney;
        INT32       nScore;
    };

    struct G2C_COUNTDOWN : MESSAGE_HEADER
    {
        INT32       nType;
        INT32       nTime;
    };
    struct G2C_ARENA_SYNC_DATA : MESSAGE_HEADER
    {
        UINT8       bySeasonNum;
        UINT8       byIsInSeason;
        UINT8       byIsJJCMatchTime;
        UINT8       byCurSeasonVicTime;
        UINT8       byPointSeasonVicTime;
    };

    struct G2C_ARENA_JOIN_NEW_SEASON_ACK : MESSAGE_HEADER
    {
        UINT8       bySeasonNum;
    };

    struct G2C_ARENA_QUIT_SEASON_ACK : MESSAGE_HEADER
    {
        UINT8       bySeasonNum;
        UINT8       byPointVictoryTimes;
    };

    struct G2C_ARENA_WIN : MESSAGE_HEADER
    {
    };

    struct G2C_ARENA_DEFEAT : MESSAGE_HEADER
    {
    };

    //----------------- spirit ----------------------
    // quest
    struct G2C_SYNC_SPIRIT : FLEX_MESSAGE_HEADER
    {
        char            szData[0];
    };

    struct G2C_SPIRIT_EXP_CHANGE : MESSAGE_HEADER
    {
        INT32		nExp;
    };

    struct G2C_SPIRIT_UP : MESSAGE_HEADER
    {
        INT32		nLevel;
    };

    struct G2C_SPIRIT_AIR_CHANGE : MESSAGE_HEADER
    {
        INT32		nBagType;
        INT32		nAirID;
        INT32		nIndex;
        INT32		nExp;
    };

    struct G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME : MESSAGE_HEADER
    {
        INT32		nRemainTime;
        INT32		nResultCode;
    };

    struct G2C_SPIRIT_BLOOD_OPEN : MESSAGE_HEADER
    {
        INT32		nPage;
    };

    struct G2C_SYNC_TELEPORT_PLATFORM : MESSAGE_HEADER
    {
        UINT8 nTeleportPlatform[16];
    };

    struct G2C_ACTIVE_PLATFORM : MESSAGE_HEADER
    {
        UINT8 byActivePlatformID;
    };

    struct G2C_SPIRIT_AIR_LEVELUP : MESSAGE_HEADER
    {
        UINT8		byLevel;
    };

    struct G2C_SPIRIT_SLOT_SIZE_CHANGE : MESSAGE_HEADER
    {
        INT32		nType;
        INT32		nSize;
    };

    struct G2C_SPIRIT_BRUSH_SLOT : MESSAGE_HEADER
    {
        INT32		nPage;
        INT32		nIndex;
        INT32		nNumber;
    };


    struct G2C_SHIELD_ACK : MESSAGE_HEADER
    {
        UINT32      dwRoleID;
        char        szRoleName[ROLE_NAME_LEN];
    };

    struct G2C_UNSHIELD_ACK : MESSAGE_HEADER
    {
        UINT32      dwRoleID;
    };

    struct G2C_UPGRADE_SOUL : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
    };

    struct G2C_ADD_SOUL : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
    };

    struct G2C_REMOVE_SOUL : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
    };

    struct G2C_RUNE_ITEM : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
    };

    struct G2C_WEAPON_LEVEL_UP_PUT_IN : MESSAGE_HEADER
    {
    };

    struct G2C_WEAPON_LEVEL_UP_TAKE_OUT : MESSAGE_HEADER
    {
    };

    struct G2C_WEAPON_LEVEL_UP_RESULT : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
    };

    struct G2C_DECOMPOSE : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
        INT32       nOriginalItemID;
        INT32       nItemID[MAX_DECOMPOSE_ITEM_COUNT];
        INT32       nItemCount[MAX_DECOMPOSE_ITEM_COUNT];
    };

    struct G2C_COMPOSE : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success, 2 = success with critical hit
        INT32       nResultItemID;
    };

    struct G2C_REFINE : MESSAGE_HEADER
    {
        UINT8       bySlot;
        UINT8       byResult;//0 = failure, 1 = success
        UINT16      wRefineLevel;
    };

    struct G2C_RANDOMIZE_ITEM : MESSAGE_HEADER
    {
        UINT16      wRandomAttrID[MAX_RANDOM_ATTR_COUNT];
    };

    struct G2C_IDENTIFY_ITEM : MESSAGE_HEADER
    {
        UINT8       byResult;//0 = failure, 1 = success
    };

    struct G2C_DRAW_DEBUG_LINE : MESSAGE_HEADER
    {
        UINT8		byType;
        UINT16		wSrcX;
        UINT16		wSrcY;
        UINT16		wSrcZ;
        UINT16		wDstX;
        UINT16		wDstY;
        UINT16		wDstZ;
    };

    struct G2C_LOGOUT_TIMER : MESSAGE_HEADER
    {
        UINT8	    byStart;							//0:cancel logout success  1:start logout timer
        UINT8	    byDelayTime;
    };

    struct  G2C_ACTIVITY_CHANGE : MESSAGE_HEADER
    {
        UINT16		wActivity;
    };

    struct G2C_ACTIVITY_REWARD : MESSAGE_HEADER
    {
        UINT16		wIndex;
    };

    struct G2C_SYNC_VIP_STATE : MESSAGE_HEADER
    {
        UINT32      dwVIPTime[vlTotal];
    };

    struct G2C_SYNC_VIP_LEVEL : MESSAGE_HEADER
    {
        UINT32      dwRoleID;
        UINT8       byVIPLevel;
    };

    struct G2C_SABAC_STATE_CHANGE : MESSAGE_HEADER
    {
        UINT8		byStarted;
    };

    struct G2C_OCCUPY_SABAC : MESSAGE_HEADER
    {
        UINT32		dwOldGuildID;
        UINT32		dwNewGuildID;
    };

    struct G2C_SYNC_SABAC_ENROLL_LIST : FLEX_MESSAGE_HEADER
    {
        struct ENROLL_DATA
        {
            UINT32	dwGuildID;
            UINT32	dwEnrollTime;
        };
        ENROLL_DATA EnrollList[0];
    };

    struct G2C_QUERY_SURVEY_PATH : MESSAGE_HEADER
    {
        char		szQuerySurveyPath[MAX_SURVEY_COUNT][MAX_SURVEY_PATH_LEN];
    };

    struct G2C_SYNC_CHECKIN_INFO : MESSAGE_HEADER
    {
        UINT8	byLastCheckInIndex;
        UINT8	byCanCheckInToday;
        UINT8	byCheckInValid;
    };

    struct G2C_CHECKIN : MESSAGE_HEADER
    {
        UINT8	byIndex;
    };

    struct BLACK_MARKET_LIMIT_INFO
    {
        INT32   nBlackMarketProductID;
        INT32   nCount;
    };

    struct G2C_BLACK_MARKET_LIMIT_INFO : FLEX_MESSAGE_HEADER
    {
        INT32   nLimitInfoCount;
        BLACK_MARKET_LIMIT_INFO LimitInfo[0];
    };

    struct BLACK_MARKET_COOLDOWN_INFO
    {
        INT32   nBlackMarketProductID;
        UINT32  dwCooldown;
    };

    struct G2C_BLACK_MARKET_COOLDOWN_INFO : FLEX_MESSAGE_HEADER
    {
        INT32   nCooldownInfoCount;
        BLACK_MARKET_COOLDOWN_INFO CooldownInfo[0];
    };

    struct G2C_BLACK_MARKET_BUY : MESSAGE_HEADER
    {
        INT32   nBlackMarketProductID;
        INT32   nCount;
    };

    struct G2C_GAMBLE : MESSAGE_HEADER
    {
        INT32   nResult;//1 = success, 0 = failure
        INT32   nItemID;
    };

    struct G2C_EXERCISE_INFO : MESSAGE_HEADER
    {
        INT32   nTotalHour;
        INT32   nCurHour;
    };

    struct G2C_ADD_EXERCISE_TIME : MESSAGE_HEADER
    {
        INT32   nTotalHour;
    };

    struct G2C_GET_EXERCISE_EXP : MESSAGE_HEADER
    {
        INT32   nHour;
        INT32   nExp;
    };

    struct JIGSAW_ANSWER_DATA
    {
        char cAnswerData[JIGSAW_ANSWER_DATA_SIZE];
    };

    struct G2C_JIGSAW : MESSAGE_HEADER
    {
        UINT32	dwTimeoutInterval;
        UINT8	byBlockX;
        UINT8	byBlockY;
        char	cImageData[JIGSAW_DATA_SIZE];
        JIGSAW_ANSWER_DATA	AnswerData[JIGSAW_ANSWER_COUNT];
    };

    struct G2C_RECEIVE_MASTER_REWARD_EXP_ACK : MESSAGE_HEADER
    {
        UINT8	bySuccess;
    };

    struct G2C_SPELL_BOOK_GAMBLE : MESSAGE_HEADER
    {
        INT32   nResult;//1 = success, 0 = failure
        INT32   nItemID;
    };
    struct G2C_RUNE_ITEM_ADVANCE : MESSAGE_HEADER
    {
        UINT8   byResult; //0 = failure, 1 = success
        UINT16  wRune[MAX_RUNE_COUNT];
    };

    struct G2C_RUNE_ITEM_ADVANCE_CONFIRM : MESSAGE_HEADER
    {
        UINT8   byResult; //0 = failure, 1 = success
    };

    struct G2C_POTION_GAMBLE : MESSAGE_HEADER
    {
        INT32   nResult;//1 = success, 0 = failure
        INT32   nItemID;
    };

    struct G2C_SELL_RESOURCE_ITEM : FLEX_MESSAGE_HEADER
    {
        INT32   nSoldCount;
        UINT8   itemslots[0];
    };
};
#pragma pack(pop)

#endif  // _G2C_MESSAGE_H_
