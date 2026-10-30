#ifndef _LOG_SERVER_DEFINE_H_
#define _LOG_SERVER_DEFINE_H_

#include "id_def.h"
#include "chat_def.h"
#include "item_def.h"
#include "quest_def.h"
#include "char_def.h"
#include "money_def.h"

#define MAX_DL_RECORD_FRIEND_COUNT	(32)
#define MAX_DL_RECORD_ITEM_COUNT	(64)
#define MAX_DL_RECORD_MESSAGE_LEN	(1024 * 16)
#define MAX_DL_FILE_CHECK_TIME_INTERVAL	(300)

enum DL_CHAT_TYPE
{
	dctInvalid = -1,

	dctRole,
	dctGuild,
	dctTeam,
	dctScreen,
	dctScene,
	dctGroup,
	dctServer,	

	dctTotal,
};

enum DL_QUEST_TYPE
{
	dqtInvalid = -1,

	dqtAccept,
	dqtComplete,
	dqtAbandon,

	dqtTotal,
};

enum DL_SCENE_STATE_TYPE
{
	dsstInvalid = -1,

	dsstCreate,
	dsstEnter,
	dsstLeave,
	dsstDestory,

	dsstTotal,
};

enum DL_ITEM_SOURCE
{
	disInvalid = -1,

	disGenerate,
	disTrade,
	disDestory,
	disSplit,

	disTotal,
};

enum DL_LOGIN
{
	dlInvalid = -1,

	dlLogin,
	dlLogout,

	dlTotal,
};

enum DL_TEAM_ACTION_TYPE
{
	dtatInvalid = -1,

	dtatCreateTeam,
	dtatJoinTeam,
	dtatLeaveTeam,
	dtatBreakDownTeam,

	dtatTotal,
};

enum DL_LOG_MODULE_TYPE
{
	dlmtInvalid = -1,

	dlmtItem,								// Item transfers
	dlmtChat,								// Chat
	dlmtQuest,								// Quest
	dlmtAchievement,						// Achievement
	dlmtLevelUp,							// Level-up
	dlmtDie,								// Death
	dlmtScene,								// Scene creation and destruction
	dlmtConSlot,							// Bag slots
	dlmtRoleLogout_basic,					// Basic character information on logout
	//dlmtRoleLogout_friend,					// Friend information on logout
	//dlmtRoleLogout_master,					// Mentorship information on logout
	dlmtRoleLogout_itembag_default,			// Default bag information on logout
	dlmtRoleLogout_itembag_equipped,		// Equipped item information on logout
	dlmtLogin,								// Account login and logout
	//dlmtGmCommand,							// GM command
	dlmtSubMoney,							// Gold transaction records
	dlmtTeam,								// Team
	dlmtRolePosition,						// Player position information
	dlmtClientHardwareInfo,					// Player hardware information
	dlmtClientFpsInfo,						// Player display frame-rate information
	dlmtSpellEffect,						// Spell casting
	dlmtCastSpell,							// Cast spell
	dlmtLearnSpell,							// Cast spell
    dlmtGuildEvent,                         // Guild event
    dlmtGuildChest,                         // Guild funds
    dlmtGuildNumStat,                       // Guild statistics
    dlmtGuildMoneyStat,                     // Guild statistics
    dlmtGuildRiceStat,                      // Guild statistics
    dlmtGuildWoodStat,                      // Guild statistics
    dlmtGuildStoneStat,                     // Guild statistics
    dlmtGuildMetalStat,                     // Guild statistics
    dlmtGuildBuildingStat,                  // Guild statistics
    dlmtBillingRequest,                     // Billing request
    dlmtBillingResult,                      // Billing result
    dlmtRuneItem,                           // Inscription refinement
    dlmtChangeItem,                         // Item change

    dlmtStatRealMoney,                      // Statistics: vouchers spent
    dlmtStatQuestComplete,                  // Statistics: quests completed
    dlmtStatNpcDie,                         // Statistics: NPC deaths
    dlmtStatNpcMoney,                       // Statistics: NPC gold drops
    dlmtStatMoney,                          // Statistics: currency
    dlmtStatMomentaryKillNpc,               // Statistics:
    dlmtStatConsecutiveKillNpc,             // Statistics:
    dlmtStatItem,                           // Item production and consumption counts
	dlmtAntiCheat,							// Anti-cheat

	dlmtTotal
};

static constexpr const char* s_pcszModuleName[] = {
	"Item",
	"Chat",
	"Quest",
	"Achievement",
	"LevelUp",
	"Die",
	"Scene",
	"BagSlot",
	"RoleLogout_Basic",
	//"RoleLogout_Friend",
	//"RoleLogout_Master",
	"RoleLogout_Itembag_Default",
	"RoleLogout_Itembag_Equipped",
	"AccountLogin",
	//"GM",
	"Money",
	"Team",
	"Position",
	"ClientHardwareInfo",
	"ClientFpsInfo",
	"SpellEffect",
	"CastSpell",
	"LearnSpell",
    "GuildEvent",
    "GuildChest",
    "GuildNumStat",
    "GuildMoneyStat",
    "GuildRiceStat",
    "GuildWoodStat",
    "GuildStoneStat",
    "GuildMetalStat",
    "GuildBuildingStat",
    "BillingRequest",
    "BillingResult",
    "RuneItem",
    "ChangeItem",

    "StatRealMoney",
    "StatQuestComplete",
    "StatNpcDie",
    "StatNpcMoney",
    "StatMoney",
    "StatMomentaryKillNpc",
    "StatConsecutiveKillNpc",
    "StatItem",
	"AntiCheat",
};

static constexpr const char* s_pcszLoginName[] = {
	"Login",
	"Logout"
};

static constexpr const char* s_pcszTeamAction[] = {
	"CreateTeam",
	"JoinTeam",
	"LeaveTeam",
	"BreakDownTeam",
};

static constexpr const char* s_pcszQuestAction[dqtTotal] = {
	"Accept",
	"Complete",
	"Abandon",
};

static constexpr const char* s_pcszSceneStateType[] = {
	"Create",
	"Enter",
	"Leave",
	"Destory",
};

inline const char* GetDLChatTypeFromID(INT32 nTargetID)
{
	switch (GetTypeFromID(nTargetID))
	{
	case otRole:
		return "Role";
		break;
	case otChannel:
		return "Channel";
		break;
	case otGuild:
		return "Guild";
		break;
	case otTeam:
		return "Team";
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	switch(nTargetID)
	{
	case cctScreen:
		return "Nearby";
		break;
	case cctScene:
		return "Scene";
		break;
	case cctGroup:
		return "Group";
		break;
	case cctServer:
		return "Server";
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}
Exit0:
	return "error";
}

inline const char* GetLoginStringByType(INT32 nType)
{
	LOG_PROCESS_ERROR(nType >= 0 && nType < sizeof(s_pcszLoginName) / sizeof(s_pcszLoginName[0]));
	
	return s_pcszLoginName[nType];
Exit0:
	return "error";
}

inline const char* GetTeamActionByType(INT32 nType)
{
	LOG_PROCESS_ERROR(nType >= 0 && nType < sizeof(s_pcszTeamAction) / sizeof(s_pcszTeamAction[0]));

	return s_pcszTeamAction[nType];
Exit0:
	return "error";
}

inline const char* GetQuestActionByType(INT32 nType)
{
	LOG_PROCESS_ERROR(nType >= 0 && nType < sizeof(s_pcszQuestAction) / sizeof(s_pcszQuestAction[0]));

	return s_pcszQuestAction[nType];
Exit0:
	return "error";
}

//inline char* GetQuestTypeByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszQuestType) / sizeof(s_pcszQuestType[0]));
//
//	return s_pcszQuestType[nType];
//Exit0:
//	return "error";
//}

inline const char* GetSceneTypeTypeByType(INT32 nType)
{
	LOG_PROCESS_ERROR(nType >= 0 && nType < sizeof(s_pcszSceneStateType) / sizeof(s_pcszSceneStateType[0]));

	return s_pcszSceneStateType[nType];
Exit0:
	return "error";
}

//inline char* GetConSlotByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszConSlotType) / sizeof(s_pcszConSlotType[0]));
//
//	return s_pcszConSlotType[nType];
//Exit0:
//	return "error";
//}
//
//inline char* GetItemSourceByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType >= 0 && nType < sizeof(s_pcszItemSourceType) / sizeof(s_pcszItemSourceType[0]));
//
//	return s_pcszItemSourceType[nType];
//Exit0:
//	return "error";
//}

//inline char* GetObjectTypeByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszObjectType) / sizeof(s_pcszObjectType[0]));
//
//	return s_pcszObjectType[nType];
//Exit0:
//	return "error";
//}

//inline char* GetItemLogByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszItemLogType) / sizeof(s_pcszItemLogType[0]));
//
//	return s_pcszItemLogType[nType];
//Exit0:
//	return "error";
//}

//inline char* GetMoneyLogByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszMoneyLogType) / sizeof(s_pcszMoneyLogType[0]));
//
//	return s_pcszMoneyLogType[nType];
//Exit0:
//	return "error";
//}

//inline char* GetClassTypeByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszClassType) / sizeof(s_pcszClassType[0]));
//
//	return s_pcszClassType[nType];
//Exit0:
//	return "error";
//}

//inline char* GetSexTypeByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszSexType) / sizeof(s_pcszSexType[0]));
//
//	return s_pcszSexType[nType];
//Exit0:
//	return "error";
//}

//inline char* GetMoneyTypeByType(INT32 nType)
//{
//	LOG_PROCESS_ERROR(nType > 0 && nType < sizeof(s_pcszMoneyType) / sizeof(s_pcszMoneyType[0]));
//
//	return s_pcszMoneyType[nType];
//Exit0:
//	return "error";
//}

#endif // _LOG_SERVER_DEFINE_H_
