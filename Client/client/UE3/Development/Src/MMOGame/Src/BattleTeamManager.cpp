#include "stdafx.h"

#include "BattleTeamManager.h"
#include "MMOMsgButtonManager.h"
#include "MMOFacade.h"

//BattleTeamManager::BattleTeamManager()
//	:_HasTeam(false)
//	,_TeamLeaderID(-1)
//	,_TeamName(TEXT(""))
//	,_IconID(0)
//	,_TeamID(-1)
//{
//	Init();
//}
//
//BattleTeamManager::~BattleTeamManager()
//{
//	Shutdown();
//}
//
//void BattleTeamManager::Init()
//{
//	MMOUI::RegisterEventHandler( this , GameEvent_BattleTeam );
//	MMOUI::RegisterEventHandler( this , GameEvent_BattleTeamInviteMember );
//}
//
//void BattleTeamManager::Shutdown()
//{
//
//}
//
//void BattleTeamManager::OnNativeGameEvent(struct MMOGameEvent* evt)
//{
//
//	switch( evt->GetEventType() )
//	{
//	case GameEvent_BattleTeam:
//		{
//			MMOGameEvent_BattleTeam* battleTeamEvt = (MMOGameEvent_BattleTeam*)evt;
//			if(battleTeamEvt == NULL)
//			{
//				return;
//			}
//
//			switch( battleTeamEvt->EvtType )
//			{	
//				case MMOGameEvent_BattleTeam::EBET_InviteEnterTeam:
//
//				InviteEnterTeam( battleTeamEvt->roleID , battleTeamEvt->roleName );
//				break;
//			}
//		}
//		break;
//	case GameEvent_BattleTeamInviteMember:
//		{
//			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
//			if(cb)
//			{
//				if( cb->Agree )
//				{
//					AcceptJoinBattleTeam( cb->Box );
//				}
//
//			}
//		}
//	}
//}
//
////-------------------------------------------------------------------------------------
//void BattleTeamManager::InviteEnterTeam(INT roleID,const FString& roleName)
//{
//	//FScriptDelegate AcceptJoinBattleTeam(EC_EventParm);
//	//AcceptJoinBattleTeam.FunctionName = TEXT("AcceptJoinBattleTeam");
//	//AcceptJoinBattleTeam.Object = this;
//
//	//FScriptDelegate Nofunction(EC_EventParm);
//	//Nofunction.FunctionName = TEXT("");
//	//Nofunction.Object = this;
//
//	FString szText = roleName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleTeam.InviteYou"));
//	//MMOMsgBoxManager::getSingleton().AddMessageBox( szText , roleID , AcceptJoinBattleTeam , Nofunction );
//
//	MMOMsgButtonManager::getSingleton().AddNewButton( szText , roleID , roleName , GameEvent_BattleTeamInviteMember , E_MsgBtnShow_Team );
//}
//
//void BattleTeamManager::AcceptJoinBattleTeam( UGFxObject* mc )
//{
//	if( mc )
//	{
//		INT roleID = mc->GetInt( TEXT("iteminfoid") );
//		CMessageHandler::GetInstance().Send_JoinBattleTeamAnswer( roleID );
//	}
//
//}
//
//void BattleTeamManager::TestAddNewMember(INT roleID , const FString& roleName)
//{
//	_TeamName = TEXT("");
//	_IconID = 0;
//	
//	BattleTeamMember tm;
//	tm.ClassID = 0;
//	tm.PlayerName = roleName;
//	tm.ServerID = roleID;
//	tm.Sex = 0;
//	tm.SingleScore = 0; //this does not exist
//	tm.TeamScore = 0;//this does not exist
//	tm.Level = 1;
//
//	_MemberList.AddItem(tm);
//
//	
//	NotifyBattleTeam();
//}
//
//void BattleTeamManager::CreateMyBattleTeam( INT teamID )
//{
//	_HasTeam = true;
//	_TeamID = teamID;
//}
//
//void BattleTeamManager::ClearMyTeam()
//{
//	_HasTeam = false;
//	_MemberList.Empty();
//	_TeamID = -1;
//	_IconID = -1;
//	_TeamLeaderID = -1;
//	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
//
//	NotifyBattleTeam();
//}
//
//BattleTeamMember* BattleTeamManager::GetMemberByRoleId( INT roleId )
//{
//	for( INT i = 0 ; i < _MemberList.Num() ; i++ )
//	{
//		if( _MemberList(i).ServerID == roleId )
//		{
//			return &_MemberList(i);
//		}
//	}
//	return NULL;
//}
//
//void BattleTeamManager::LeaveBattleTeam( INT roleID )
//{
//	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
//	if( player && player->ServerID == roleID )
//	{
//		ClearMyTeam();
//	}
//	else
//	{
//		for ( INT i = 0 ; i < _MemberList.Num() ; i++ )
//		{
//			if( _MemberList(i).ServerID == roleID )
//			{
//				_MemberList.Remove(i);
//
//				break;
//			}
//		}
//	}
//
//	NotifyBattleTeam();
//}
//
//void BattleTeamManager::NotifyBattleTeam()
//{
//	MMOGameEvent_BattleTeam evt(MMOGameEvent_BattleTeam::EBET_UpdateMemberList);
//	MMOUI::PostGameEvent(&evt);
//}
//
//UBOOL BattleTeamManager::HasMember( INT serveID )
//{
//	for ( INT i = 0 ; i < _MemberList.Num() ; i++ )
//	{
//		if( _MemberList(i).ServerID == serveID )
//		{
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//
//void BattleTeamManager::SetTeamInfo( M2C_ARENA_TEAM_INFO* msg )
//{
//	if(!_HasTeam)
//	{
//		return;
//	}
//
//	_MemberList.Empty();
//
//	_TeamID = msg->dwArenaTeamID;
//	_TeamName = UTF8_TO_TCHAR(msg->szName);
//	_IconID = msg->wIconID;
//	if( msg->byMemberCount > 0 )
//	{
//		//Server sets member 0 as leader (creator)
//		_TeamLeaderID = msg->stMemberList[0].dwRoleID;
//	}
//	
//	
//	for ( INT i = 0 ; i < msg->byMemberCount ; i++ )
//	{
//		BattleTeamMember tm;
//		tm.ClassID = msg->stMemberList[i].byClass;
//		tm.PlayerName = UTF8_TO_TCHAR(msg->stMemberList[i].szRoleName);
//		tm.ServerID = msg->stMemberList[i].dwRoleID;
//		tm.Sex = msg->stMemberList[i].bySex;
//		tm.SingleScore = 0; //this does not exist
//		tm.TeamScore = 0;//this does not exist
//		tm.Level = msg->stMemberList[i].byLevel;
//
//		_MemberList.AddItem(tm);
//		//_MemberList.AddItem(tm);
//		//_MemberList.AddItem(tm);
//	}
//
//	for ( INT j = 0 ; j < atptTotal ; j++ )
//	{
//		_AchievementTable[j].Match = msg->StatData[j].nTotal;
//		_AchievementTable[j].Rank = msg->StatData[j].nRank;
//		_AchievementTable[j].Score = msg->StatData[j].nScore;
//		_AchievementTable[j].Win = msg->StatData[j].nWin;
//		_AchievementTable[j].WinRate = (FLOAT)msg->StatData[j].nWin / msg->StatData[j].nTotal * 100;
//	}
//
//
//	NotifyBattleTeam();
//}
//
//UBOOL BattleTeamManager::IsTeamLeader()
//{
//	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
//	if(player)
//	{
//		return ( _TeamLeaderID == player->ServerID );
//	}
//	return FALSE;
//}