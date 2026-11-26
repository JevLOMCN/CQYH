#include "stdafx.h"
#include "message_handler.h"
#include "MMOAchievementManager.h"
#include "MMOHandbookManager.h"

using namespace ExternalMessage;

// send
//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Send_AchievementComplete(UINT32 id)
{
#if M_DEBUG_ACHIEVEMENT
	G2C_COMPLETE_ACHIEVEMENT *msg = (G2C_COMPLETE_ACHIEVEMENT*)ms_MsgBuffer;
	msg->nAchievementID = id;
	msg->byMsg = g2c_complete_achievement;
	Parse_g2c_complete_achievement((const char*)msg, sizeof(G2C_COMPLETE_ACHIEVEMENT));
#else
	C2G_COMPLETE_ACHIEVEMENT msg;
	msg.wMsg = c2g_complete_achievement;
	msg.nAchievementID = id;
	_Send(&msg, sizeof(msg));
#endif
}

void CMessageHandler::Send_AchievementCompare(UINT32 roleId)
{
#if M_DEBUG_ACHIEVEMENT
#else
	C2G_COMPARE_ACHIEVEMENT msg;
	msg.wMsg = c2g_compare_achievement;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
#endif
}


void CMessageHandler::Send_ObtainRewardItem(UINT32 id)
{
	C2G_REWARD_ACHIEVEMENT msg;
	msg.wMsg = c2g_reward_achievement;
	msg.nAchievementID = id;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_KillNpcAchievementComplete(UINT16 id, UINT8 index)
{
	C2G_COMPLETE_KILL_NPC_QUEST msg;
	msg.wMsg = c2g_complete_kill_npc_quest;
	msg.wQuestID = id;
	msg.byIndex = index;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildAchievement()
{
	C2M_GUILD_ACHIEVEMENT msg;
	msg.wMsg = c2m_guild_achievement;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ObtainGuildReward(UINT32 id)
{
	C2G_GUILD_REWARD_ACHIEVEMENT msg;
	msg.wMsg = c2g_guild_reward_achievement;
	msg.nID = id;
	_Send(&msg, sizeof(msg));
}

// receive
//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Parse_g2c_sync_achievement_list(const char* buf, UINT32 size)
{
	G2C_SYNC_ACHIEVEMENT_LIST* msg = (G2C_SYNC_ACHIEVEMENT_LIST*)buf;
	if (!buf || size < sizeof(*msg) + sizeof(ROLE_DB_ACHIEVEMENT_DATA))
		return;
	MMOAchievementManager::getSingleton().NotifyAchievementListUpdate(msg->szData);
	MMOHandbookManager::getSingleton().NotifySyncHandbookInfo((ROLE_DB_ACHIEVEMENT_DATA*)(msg->szData));
}

void CMessageHandler::Parse_g2c_complete_achievement(const char* buf, UINT32 size)
{
	G2C_COMPLETE_ACHIEVEMENT* msg = (G2C_COMPLETE_ACHIEVEMENT*)buf;
	MMOAchievementManager::getSingleton().NotifyAchievementComplete(msg->nAchievementID, msg->dwCompleteTime);
}

void CMessageHandler::Parse_g2c_compare_achievement(const char* buf, UINT32 size)
{
	G2C_COMPARE_ACHIEVEMENT* msg = (G2C_COMPARE_ACHIEVEMENT*)buf;
	if (!buf || size < sizeof(*msg) + sizeof(ROLE_DB_ACHIEVEMENT_DATA))
		return;
	MMOAchievementManager::getSingleton().NotifyAchievementCompareListUpdate(msg->dwRoleID, msg->acAchievement);
}

void CMessageHandler::Parse_g2c_reward_achievement(const char* buf, UINT32 size)
{
	G2C_REWARD_ACHIEVEMENT* msg = (G2C_REWARD_ACHIEVEMENT*)buf;
	MMOAchievementManager::getSingleton().NotifyAchievementRewardObtain(msg->nAchievementID);
}

void CMessageHandler::Parse_g2c_update_achievement_variable(const char* buf, UINT32 size)
{
	G2C_UPDATE_ACHIEVEMENT_VARIABLE* msg = (G2C_UPDATE_ACHIEVEMENT_VARIABLE*)buf;
	MMOAchievementManager::getSingleton().NotifyAchievementVariableUpdate(msg->nIndex, msg->nValue);
}

void CMessageHandler::Parse_g2c_activate_hankbook(const char* buf, UINT32 size)
{
	G2C_ACTIVATE_HANKBOOK* msg = (G2C_ACTIVATE_HANKBOOK*)buf;
	MMOHandbookManager::getSingleton().NotifyUpdateHandbookInfo(msg->nType, msg->nIndex);
}

//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Parse_g2c_sync_kill_npc_quest(const char* buf, UINT32 size)
{
	G2C_SYNC_KILL_NPC_QUEST* msg = (G2C_SYNC_KILL_NPC_QUEST*)buf;
	MMOAchievementManager::getSingleton().NotifySyncKillNpcAchievementData(msg->wKillCount, msg->byCompleteList);
}

void CMessageHandler::Parse_g2c_sync_kill_npc_count(const char* buf, UINT32 size)
{
	G2C_SYNC_KILL_NPC_COUNT* msg = (G2C_SYNC_KILL_NPC_COUNT*)buf;
	MMOAchievementManager::getSingleton().NotifyUpdateKillNpcAchievementInfo(msg->wQuestID, msg->wKillCount);
}

void CMessageHandler::Parse_g2c_complete_kill_npc_quest(const char* buf, UINT32 size)
{
	G2C_COMPLETE_KILL_NPC_QUEST* msg = (G2C_COMPLETE_KILL_NPC_QUEST*)buf;
	UINT32 id = MMOAchievementData::getSingleton().GetKillNpcAchievementId(msg->wQuestID, msg->byIndex);
	MMOAchievementManager::getSingleton().NotifyAchievementRewardObtain(id);
}

void CMessageHandler::Parse_g2c_reset_kill_npc_quest(const char* buf, UINT32 size)
{
	G2C_RESET_KILL_NPC_QUEST* msg = (G2C_RESET_KILL_NPC_QUEST*)buf;
	MMOAchievementManager::getSingleton().NotifyUpdateKillNpcAchievementInfo(msg->wQuestID, 0);
}

//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Parse_g2c_sync_guild_achievement(const char* buf, UINT32 size)
{
	G2C_SYNC_GUILD_ACHIEVEMENT* msg = (G2C_SYNC_GUILD_ACHIEVEMENT*)buf;
	MMOAchievementManager::getSingleton().NotifySyncGuildAchievementInfo(msg->acAchievement);
}

void CMessageHandler::Parse_m2c_guild_achievement_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_ACHIEVEMENT_ACK* msg = (M2C_GUILD_ACHIEVEMENT_ACK*)buf;
	MMOAchievementManager::getSingleton().NotifyUpdateGuildAchievementInfo(msg->acAchievement);
}




