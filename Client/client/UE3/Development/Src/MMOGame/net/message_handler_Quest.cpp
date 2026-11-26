#include "stdafx.h"
#include "message_handler.h"
#include "MMOQuestManager.h"
#include "MMOFacade.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

using namespace ExternalMessage;

void CMessageHandler::Send_AcceptQuest(INT questId, INT questType, INT shareSrcId)
{
	C2G_ACCEPT_QUEST msg;
	msg.wMsg = c2g_accept_quest;
	msg.nQuestID = questId;
	msg.nAcceptType = questType;
	msg.nAcceptSrcID = shareSrcId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AbandonQuest(INT QuestId)
{
	C2G_ABANDON_QUEST msg;
	msg.wMsg = c2g_abandon_quest;
	msg.nQuestID = QuestId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CompleteQuest(INT QuestId,INT selectaward)
{
	C2G_COMPLETE_QUEST msg;
	msg.wMsg = c2g_complete_quest;
	msg.nQuestID = QuestId;
	msg.nSelectedAwardIdx = selectaward;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ShareQuest(INT QuestId)
{
	C2G_SHARE_QUEST msg;
	msg.wMsg = c2g_share_quest;
	msg.nQuestID = QuestId;
	msg.nShareType = qatShare;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_QuestNPCTalk(INT questID, INT npcServerID)
{
	C2G_TALK_TO_NPC msg;
	msg.wMsg = c2g_talk_to_npc;
	msg.dwTalkNpcID = npcServerID;
	msg.nQuestID = questID;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_Explore()
{
	C2G_EXPLORE_ZONE msg;
	msg.wMsg = c2g_explore_zone;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_FlyToTarget(INT questId, INT destIndex)
{
//	C2G_FLY_TO_QUEST_ZONE msg;
//	msg.wMsg = c2g_fly_to_quest_zone;
//	msg.nQuestID = questId;
//	msg.nZoneIndex = destIndex;
//	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GenerateCircleQuestList()
{
//	C2G_GENERATE_CIRCLE_QUEST msg;
//	msg.wMsg = c2g_generate_circle_quest;
//	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_UpgradeAwardQuestLimit(INT maxLimit)
{
//	C2G_BUY_DAILY_QUEST_COMPLETE_TIME msg;
//	msg.wMsg = c2g_buy_daily_quest_complete_time;
//	msg.nTime = maxLimit;
//	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CompleteAllRewardQuest()
{
//	C2G_DAILY_QUEST_COMPLETE_SHOT_CUT msg;
//	msg.wMsg = c2g_daily_quest_complete_shot_cut;
//	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RefreshRewardQuestList(UBOOL useMoney)
{
//	C2G_BRUSH_DAILY_QUEST msg;
//	msg.wMsg = c2g_brush_daily_quest;
//	msg.bUseMoney = useMoney;
//	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_Recycle(INT itemTempId, INT index)
{
//	C2G_QUEST_RECYCLE msg;
//	msg.wMsg = c2g_quest_recycle;
//	msg.nTemplateID = itemTempId;
//	msg.nIndex = index;
//	_Send(&msg, sizeof(msg));
}

//////////////////////////////////////////////////////////////////////////

void CMessageHandler::Parse_g2c_accept_quest(const char* buf, UINT32 size)
{
	G2C_ACCEPT_QUEST* msg = (G2C_ACCEPT_QUEST*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyAcceptQuest(msg->nQuestID);
}

void CMessageHandler::Parse_g2c_abandon_quest(const char* buf, UINT32 size)
{
	G2C_ABANDON_QUEST* msg = (G2C_ABANDON_QUEST*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyAbandonQuest(msg->nQuestID);
}

void CMessageHandler::Parse_g2c_complete_quest(const char* buf, UINT32 size)
{
	G2C_COMPLETE_QUEST* msg = (G2C_COMPLETE_QUEST*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyCompleteQuest(msg->nQuestID);
}

void CMessageHandler::Parse_g2c_notify_next_circle_quest(const char* buf, UINT32 size)
{
	G2C_NOTIFY_NEXT_CIRCLE_QUEST* msg = (G2C_NOTIFY_NEXT_CIRCLE_QUEST*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyNextCircleQuest( msg->nQuestID, msg->nCircleID);
}

void CMessageHandler::Parse_g2c_sync_quest_list(const char* buf, UINT32 size)
{
	G2C_SYNC_QUEST_LIST* msg = (G2C_SYNC_QUEST_LIST*)buf;
	if (!buf || size < sizeof(*msg) + sizeof(ROLE_DB_QUEST_DATA))
		return;
	ROLE_DB_QUEST_DATA* questData = (ROLE_DB_QUEST_DATA*)msg->szData;
	if (questData->byAcceptedQuestCount >
		(size - sizeof(*msg) - sizeof(*questData)) / sizeof(questData->AcceptQuestList[0]))
		return;
	MMOQuestManager::getSingleton().NotifyUpdateQuestList(questData);
}

void CMessageHandler::Parse_g2c_share_quest(const char* buf, UINT32 size)
{
	G2C_SHARE_QUEST* msg = (G2C_SHARE_QUEST*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyShareQuest(msg->nQuestID, msg->nSrcID);
}

void CMessageHandler::Parse_g2c_buy_daily_quest_complete_time(const char* buf, UINT32 size)
{
	G2C_BUY_DAILY_QUEST_COMPLETE_TIME* msg = (G2C_BUY_DAILY_QUEST_COMPLETE_TIME*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyUpgradeRewardQuestLimit(msg->nCurrentRemainTime);
}

void CMessageHandler::Parse_g2c_brush_daily_quest(const char* buf, UINT32 size)
{
	G2C_BRUSH_DAILY_QUEST* msg = (G2C_BRUSH_DAILY_QUEST*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyRefreshRewardQuestList(msg->nQuestID, msg->bClearCDTime);
}

void CMessageHandler::Parse_g2c_quest_recycle(const char* buf, UINT32 size)
{
	G2C_QUEST_RECYCLE* msg = (G2C_QUEST_RECYCLE*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	UMMOUIQuestItemCollectionPanel* panel = (UMMOUIQuestItemCollectionPanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestItemCollection);
	if (panel)
	{
		panel->Show(FALSE);
	}
}

void CMessageHandler::Parse_g2c_set_quest_failed(const char* buf, UINT32 size)
{
	G2C_SET_QUEST_FAILED* msg = (G2C_SET_QUEST_FAILED*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOQuestManager::getSingleton().NotifyQuestFailed(msg->nQuestID);
}