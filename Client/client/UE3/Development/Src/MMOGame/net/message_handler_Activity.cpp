#include "stdafx.h"
#include "message_handler.h"
#include "ActivityManager.h"
#include "TeamManager.h"

void CMessageHandler::Send_GetActivityReward(INT nIndex)
{
	C2G_ACTIVITY_REWARD msg;
	msg.wMsg = c2g_activity_reward;
	msg.nIndex = nIndex;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_QueryActivityTeamList(INT nActivity, INT nPage, UINT nActivityType)
{
	/*C2M_TEAM_QUERY_ACTIVITY_TEAM msg;
	msg.wMsg = c2m_team_query_activity_team;
	msg.nActivityID = nActivity;
	msg.dwActivityType = nActivityType;
	msg.nPgae = nPage;
	_Send(&msg, sizeof(msg));*/
}

void CMessageHandler::Send_ChangeTeamActivity(INT nActivity, UINT nActivityType)
{
	/*C2M_TEAM_CHANGE_ACTIVITY_INFO msg;
	msg.wMsg = c2m_team_change_activity_info;
	msg.nActivityID = nActivity;
	msg.dwActivityType = nActivityType;
	_Send(&msg, sizeof(msg));*/
}

void CMessageHandler::Parse_m2c_activity_query_team(const char* buf, UINT32 size)
{
	//M2C_ROOM_QUERY_TEAM* msg = (M2C_ROOM_QUERY_TEAM*)buf;
	ActivityManager::getSingleton().UpdateActivityTeamList(buf);
}

void CMessageHandler::Parse_m2c_activity_change_dungeon(const char* buf, UINT32 size)
{
	M2C_ACTIVITY_CHANGE_DUNGEON* msg = (M2C_ACTIVITY_CHANGE_DUNGEON*)buf;
	TeamManager::getSingleton().SetTeamTargetType(msg->dwActivityType);
	TeamManager::getSingleton().SetTeamTargetId(msg->nActivityID);

	MMOGameEvent_Activity evt(MMOGameEvent_Activity::EActivity_ForceRequireActivityTeam);
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_TargetChange);
	MMOUI::PostGameEvent(&et);
}

void CMessageHandler::Parse_g2c_activity_change(const char* buf, UINT32 size) 
{
	G2C_ACTIVITY_CHANGE* msg = (G2C_ACTIVITY_CHANGE*)buf; 
	if(msg)
	{
		ActivityManager::getSingleton().NotifyActivityVariableErvtActivity(msg->wActivity);
	}
}

void CMessageHandler::Parse_g2c_activity_reward(const char* buf, UINT32 size) 
{
	G2C_ACTIVITY_REWARD* msg = (G2C_ACTIVITY_REWARD*)buf; 
	if(msg)
	{
		//Reward claimed successfully
		INT32 RewardBit = ActivityManager::getSingleton().GetRewardBit();
		RewardBit |= (1 << msg->wIndex);
		ActivityManager::getSingleton().NotifyActivityVariableErvtActivityReward(RewardBit);
	}
}