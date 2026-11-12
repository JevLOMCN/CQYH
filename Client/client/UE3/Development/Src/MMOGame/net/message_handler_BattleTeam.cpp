#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "BattleTeamManager.h"
#include "MMOArenaManager.h"
#include "RankListManager.h"

using namespace ExternalMessage;

#define OfflineModeTest 0
void CMessageHandler::CreateBattleTeam()
{
	//BattleTeamManager::getSingleton().CreateMyBattleTeam( 1 );
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

	//BattleTeamManager::getSingleton().TestAddNewMember( player->ServerID , player->RoleName );
}

void CMessageHandler::InviteBattleTeam( int roleID , FString roleName )
{
	//BattleTeamManager::getSingleton().TestAddNewMember( roleID , roleName );
}

void CMessageHandler::RemoveBattleTeam( int roleID )
{
	//BattleTeamManager::getSingleton().LeaveBattleTeam( roleID );
}

void CMessageHandler::LeaveBattleTeam()
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	//BattleTeamManager::getSingleton().LeaveBattleTeam( player->ServerID );
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_join_arena_team(const char* buf, UINT32 size)
{
	//M2C_JOIN_ARENA_TEAM* msg = (M2C_JOIN_ARENA_TEAM*)buf;

	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

	////New member joined
	//if( player && BattleTeamManager::getSingleton().HasTeam() && player->ServerID != msg->stJoinMember.dwRoleID )
	//{		
	//	Send_RequestBattleTeamInfo( msg->dwArenaTeamID );
	//}
	//
	////Self joined
	//else if( player && !BattleTeamManager::getSingleton().HasTeam() && player->ServerID == msg->stJoinMember.dwRoleID )
	//{
	//	
	//	BattleTeamManager::getSingleton().CreateMyBattleTeam( msg->dwArenaTeamID );
	//	Send_RequestBattleTeamInfo( msg->dwArenaTeamID );

	//	MMOGameEvent_BattleTeam evt(MMOGameEvent_BattleTeam::EBET_TurnTab);
	//	evt.tab = BattleTeamPage_5v5;
	//	MMOUI::PostGameEvent(&evt);
	//}
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_join_arena_team_ask(const char* buf, UINT32 size)
{
	//M2C_JOIN_ARENA_TEAM_ASK* msg = (M2C_JOIN_ARENA_TEAM_ASK*)buf;

	//MMOGameEvent_BattleTeam evt(MMOGameEvent_BattleTeam::EBET_InviteEnterTeam);
	//evt.roleID = msg->dwInviteRoleID;
	//evt.roleName = UTF8_TO_TCHAR( msg->szRoleName );
	//MMOUI::PostGameEvent(&evt);
}


//----------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_leave_arena_team(const char* buf, UINT32 size)
{
	//M2C_LEAVE_ARENA_TEAM* msg = (M2C_LEAVE_ARENA_TEAM*)buf;

	//BattleTeamManager::getSingleton().LeaveBattleTeam( msg->dwRoleID );
	
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_arena_team_info(const char* buf, UINT32 size)
{
	//M2C_ARENA_TEAM_INFO* msg = (M2C_ARENA_TEAM_INFO*)buf;
	//
	//if( BattleTeamManager::getSingleton().GetTeamID() == msg->dwArenaTeamID )
	//{
	//	BattleTeamManager::getSingleton().SetTeamInfo( msg );
	//}
	//else
	//{
	//	MMOArenaManager::getSingleton().AddOrUpdateArenaTeamInfo(msg);
	//	RankListManager::getSingleton().NotifyUpdateRankList();
	//}
	
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_arena_simple_info(const char* buf, UINT32 size)
{
	/*
	G2C_SYNC_ARENA_SIMPLE_INFO* msg = (G2C_SYNC_ARENA_SIMPLE_INFO*)buf;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player && msg->dwRoleID == player->ServerID )
	{
		player->PersonalArenaScore = msg->dwPersonalArenaScore;
		if( msg->dwArenaTeamID != 0 )
		{
			BattleTeamManager::getSingleton().CreateMyBattleTeam(msg->dwArenaTeamID);
			Send_RequestBattleTeamInfo( msg->dwArenaTeamID );
		}
		else
		{
			BattleTeamManager::getSingleton().ClearMyTeam();
		}		
	}
	else
	{
		AMMOPlayer* player = Cast<AMMOPlayer>( MMOFacade::getSingleton().GetCharacter(msg->dwRoleID) );
		if(player)
		{
			player->PersonalArenaScore = msg->dwPersonalArenaScore;
		}
	}
	*/
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_CreateBattleTeam( const FString& teamName , int iconID )
{
	//C2G_CREATE_ARENA_TEAM msg;
	//msg.wMsg = c2g_create_arena_team;
	//msg.wIconID = iconID;
	//strxcpy( msg.szName , *teamName, TEAM_NAME_LEN );
	//if( !OfflineModeTest )
	//{
	//	_Send(&msg,sizeof(msg));
	//}
	//else
	//{
	//	CreateBattleTeam();
	//}

}

void CMessageHandler::Send_JoinBattleTeam( FString roleName )
{
	//C2M_INVITE_NEW_ARENA_TEAM_MEMBER msg;
	//msg.wMsg = c2m_invite_new_arena_team_member;
	//strxcpy( msg.szRoleName , *roleName, ROLE_NAME_LEN );
	//
	//_Send(&msg,sizeof(msg));
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_JoinBattleTeam(INT roleID)
{
	//C2M_JOIN_ARENA_TEAM msg;
	//msg.wMsg = c2m_join_arena_team;
	//msg.dwInviteRoleID = roleID;

	//if( !OfflineModeTest )
	//{
	//	_Send(&msg,sizeof(msg));
	//}
	//else
	//{
	//	InviteBattleTeam( roleID , TEXT("test") );
	//}
	//
}
//Reply to team invitation
//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_JoinBattleTeamAnswer(INT roleID)
{
	//C2M_JOIN_ARENA_TEAM_ANSWER msg;
	//msg.wMsg = c2m_join_arena_team_answer;
	//msg.dwTargetRoleID = roleID;
	//msg.byAnswer = etarAgree;
	//_Send(&msg,sizeof(msg));
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_LeaveBattleTeam( INT roleID )
{
	//C2M_LEAVE_ARENA_TEAM msg;
	//msg.wMsg = c2m_leave_arena_team;
	//msg.dwRoleID = roleID;
	//if( !OfflineModeTest )
	//{
	//	_Send(&msg,sizeof(msg));
	//}
	//else
	//{
	//	RemoveBattleTeam(roleID);
	//}
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_RequestBattleTeamInfo( INT teamId )
{
	//C2M_QUERY_ARENA_TEAM_INFO msg;
	//msg.wMsg = c2m_query_arena_team_info;
	//msg.dwTeamID = teamId;
	//_Send(&msg, sizeof(msg));
}
