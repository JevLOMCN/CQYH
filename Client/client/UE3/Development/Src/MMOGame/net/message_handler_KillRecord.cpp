#include "stdafx.h"
#include "message_handler.h"
#include "money_def.h"
#include "MMOKillRecordManager.h"

using namespace ExternalMessage;


void CMessageHandler::Parse_g2c_update_consecutive_kill_npc_count(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT))
		return;
	G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT* msg = (G2C_UPDATE_CONSECUTIVE_KILL_NPC_COUNT*)buf;
	if(msg == NULL)
		return;
	MMOKillRecordManager::getSingleton().NotifyNewKillMonster(msg->byCount);
}
void CMessageHandler::Parse_g2c_update_consecutive_kill_npc_end(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_UPDATE_CONSECUTIVE_KILL_NPC_END))
		return;
	G2C_UPDATE_CONSECUTIVE_KILL_NPC_END* msg = (G2C_UPDATE_CONSECUTIVE_KILL_NPC_END*)buf;
	if(msg == NULL)
		return;
	MMOKillRecordManager::getSingleton().NotifyKillMonsterReward(msg->byCount,msg->nExp);
}
void CMessageHandler::Parse_g2c_update_momentary_kill_npc_end(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_UPDATE_MOMENTARY_KILL_NPC_END))
		return;
	G2C_UPDATE_MOMENTARY_KILL_NPC_END* msg = (G2C_UPDATE_MOMENTARY_KILL_NPC_END*)buf;
	if(msg == NULL)
		return;
	MMOKillRecordManager::getSingleton().NotifyKillMonsterMomentaryReward(msg->byCount,msg->nExp);
}
void CMessageHandler::Parse_g2c_update_consecutive_kill_player_count(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT))
		return;
	G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT* msg = (G2C_UPDATE_CONSECUTIVE_KILL_PLAYER_COUNT*)buf;
	if(msg == NULL)
		return;
	MMOKillRecordManager::getSingleton().NotifyNewKillPlayer(msg->byCount);
}