#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "MMOMsgButtonManager.h"
#include "CommonGameEvents.h"
#include "MMOChatManager.h"
#include "TeamManager.h"
#include "MMOGuideManager.h"
#include "MMOQuestManager.h"
#include "ActivityManager.h"
#include "UITextParser.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

//-------------------------------------------------------------------------------
void CMessageHandler::Send_TeamLeave(INT RoleId)
{
	C2M_TEAM_LEAVE msg;
	msg.wMsg = c2m_team_leave;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
		return;
	msg.dwLeaveRoleID = RoleId;
	_Send(&msg, sizeof(msg));
}

//-------------------------------------------------------------------------------
void CMessageHandler::Send_CreateTeam(const FString& TeamName,INT AssignType,INT TeamMaxCount)
{
	C2M_TEAM_JOIN  msg;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(!player)
		return;

	msg.wMsg = c2m_team_join;
	msg.dwTargetRoleID = player->ServerID;
	//msg.byAssignType = AssignType;
	_Send(&msg, sizeof(msg));

	//strncpy(msg.aszTeamName, TCHAR_TO_UTF8(*TeamName), TEAM_NAME_LEN);
	//msg.aszTeamName[TEAM_NAME_LEN - 1] = 0;
	//msg.byAssignType = AssignType;
	//msg.dwMaxTeamMemberCount = TeamMaxCount;
}
//-----------------------------------------------------------
void CMessageHandler::Request_TeamList()
{
	C2G_ADJACENT_ROLE_NOTEAM msg;
	msg.wMsg = c2g_adjacent_team;
	_Send(&msg, sizeof(msg));
}
//-----------------------------------------------------------
void CMessageHandler::Request_PlayerList()
{
	C2G_ADJACENT_TEAM msg;
	msg.wMsg = c2g_adjacent_role_noteam;
	_Send(&msg, sizeof(msg));
}


//-----------------------------------------------------------

void CMessageHandler::Send_EnterTeam( INT RoleId)
{
	C2M_TEAM_JOIN  msg;

	msg.wMsg = c2m_team_join;
	msg.dwTargetRoleID = RoleId;
	//msg.byAssignType = AssignType;
	_Send(&msg, sizeof(msg));

	/*if( !iCreate )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TeamRequestHasSend")),MsgInfoType_Table,TEXT("MMOGame.Team.TeamRequestHasSend") );
	}*/
}

void CMessageHandler::Send_TeamInvite(UINT32 roleId,UINT8 AssignType)
{
	C2M_TEAM_INVITE msg;
	msg.wMsg = c2m_team_invite;
	msg.dwTargetRoleID = roleId;
	msg.byAssignType = AssignType;
	_Send(&msg, sizeof(msg));
}
//----------------------------------------------------

//void CMessageHandler::Send_GetTeamList( int PageNum )
//{
	//C2G_GET_TEAMITEM msg;
	//msg.byMsg = c2g_get_teamitem; 
	//msg.nPageNum = PageNum;
	//_Send(&msg, sizeof(msg));
//}
//------------------------------------------------------------------------
void CMessageHandler::Send_TeamAnswer( INT RoleId , UBOOL Agree, INT TeamOperateType)
{
	C2M_TEAM_ANSWER msg;
	msg.wMsg = c2m_team_answer;

	if( Agree)
		msg.byAnswerResult = etarAgree;
	else
		msg.byAnswerResult = etarDisAgree;
	msg.byAnswerType = TeamOperateType;
	msg.dwAnswerRequestRoleID = RoleId;
	//msg.dwRoleID = RoleId;
	_Send(&msg, sizeof(msg) );
}
//------------------------------------------------------------------------//change leader, change team attributes
void CMessageHandler::Send_OperateTeam( INT LeadId , INT AssignType , INT TeamMemberLimit , FString TeamName , INT auctionItemColor )
{
	C2M_TEAM_CHANGEINFO msg;
	msg.wMsg = c2m_team_changeinfo; 
	//msg.byAssignType = AssignType;
	msg.nTeamMemberLimit = TeamMemberLimit;
	msg.byItemAuctionColorLimit = auctionItemColor;
	msg.dwLeadID = LeadId;
	strxcpy( msg.szTeamName , *TeamName, TEAM_NAME_LEN );
	
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_QueryTeam(UINT32 roleId)
{
	C2M_TEAM_QUERY msg;
	msg.wMsg = c2m_team_query;
	msg.dwTargetRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

//----------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_team_join(const char* buf, UINT32 size)
{ 
	//Team creation succeeded or at login time
	M2C_TEAM_JOIN* msg = (M2C_TEAM_JOIN*)buf;

	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if( !pl )
		return;

	TeamManager& teamMgr = TeamManager::getSingleton();
	teamMgr.CreateMyTeam( msg->dwTeamID );
	teamMgr.AssignType = msg->byAssignType;
	teamMgr.MemberLimit = msg->nMemberSizeLimit;
	teamMgr.SetTeamName( UTF8_TO_TCHAR(msg->szTeamName) );
	teamMgr.SetTeamTargetType(msg->byActivityType);
	teamMgr.SetTeamTargetId(msg->dwActivityID);
	
	
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();


	AMMOHud* hud = MMOFacade::getSingleton().GetMyHud();
	
	
	if( teamMgr.TeamID != msg->dwTeamID )
		return;


	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	teamMgr.GetTeamMemberList().Empty();
	//teamMgr.ImTeamLeader = ( pl->ServerID == msg->dwLeadID );
	//teamMgr.TeamLeaderServeId = msg->dwLeadID;
	teamMgr.SetTeamLeaderRoleId( msg->dwLeadID );

	if( teamMgr.IsTeamLeader() )
	{
		if(MMO::GetCurrentGameState() == EGameState_InGame)
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.CreateTeam") ) ,MsgInfoType_Table,TEXT("MMOGame.Team.CreateTeam"));

		// Guide quest
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Team);

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_CreateTeam);
		MMOUI::PostGameEvent(&evtGuide);

		if (teamMgr.GetIsExpand() == FALSE)
		{
			teamMgr.TeamExpand();

		}
			
	}

	for ( UINT32 i = 0 ; i < msg->byMemberCount ; i++ )
	{
		TEAM_MEMBER_BASEINFO MI = msg->stTeamMemberList[i];
		MMOTeamMember Member;
		Member.ActorName = UTF8_TO_TCHAR(MI.szRoleName);
		Member.ActorServerId = MI.dwRoleID;
		Member.ClassId = MI.byClass;
		Member.Sex = MI.bySex;
		Member.isLeader = ( msg->dwLeadID == MI.dwRoleID );
		Member.Online = ( MI.byState == etsOnline );

		//Look up whether this person is in the player list
		AMMOPlayer* mem = Cast<AMMOPlayer>( MMOFacade::getSingleton().GetCharacter(Member.ActorServerId) );
		if( mem )
		{
			mem->TeamID = msg->dwTeamID;
			Member.Lv = mem->ActorAttributes->LevelAttributes.Level;
			Member.HP = mem->ActorAttributes->GetCurHp();
			if( mem->ActorAttributes->GetMaxHp() != 0 )
				Member.HPPecent = (FLOAT)mem->ActorAttributes->GetCurHp() / mem->ActorAttributes->GetMaxHp();
			else
				Member.HPPecent = 0.001;

			if( mem->ActorAttributes->GetMaxMp() != 0 )
				Member.MPPecent = (FLOAT)mem->ActorAttributes->GetCurMp() / mem->ActorAttributes->GetMaxMp();
			else
				Member.MPPecent = 0.001;

		}
		else
		{
			Member.Lv = 1;
			Member.HP = 1;
			Member.HPPecent = 0.001;
			Member.MPPecent = 0.001;
		}
		Member.UIPos = -1;

		teamMgr.AddTeamMember(Member);

		if( teamMgr.IsTeamLeader() && !Member.isLeader )
		{
			if(MMO::GetCurrentGameState() == EGameState_InGame)
				MMOUI::GetUIManager()->delegateAddScreenMessage( Member.ActorName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.PlayerAddTeam") ) ,MsgInfoType_Table,TEXT("MMOGame.Team.PlayerAddTeam"));
		}

		if(mem)
		{
			MMOUI::GetUIManager()->delegateUpdateHintText(mem,ESHU_ALL);
		}
	}

	if( !teamMgr.IsTeamLeader() )
	{
		if(MMO::GetCurrentGameState() == EGameState_InGame)
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.IAddTeam") ) ,MsgInfoType_Table,TEXT("MMOGame.Team.IAddTeam"));
	}

	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_HudInfo_TeamHint , TRUE, "");
	MMOGameEvent_Any evt(GameEvent_TeamJoin);
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_TargetChange);
	MMOUI::PostGameEvent(&et);
}
////----------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_team_changeinfo(const char* buf, UINT32 size)
{
	M2C_TEAM_CHANGEINFO* msg = (M2C_TEAM_CHANGEINFO*)buf;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if( pl && msg )
	{
		TeamManager& teamMgr = TeamManager::getSingleton();
		if( teamMgr.HasTeam() && teamMgr.TeamID == msg->dwTeamID )
		{
			teamMgr.SetTeamLeaderRoleId( msg->dwLeadID );

			//if(teamMgr.AssignType != msg->byAssignType)
			//{
			//	if(!teamMgr.IsTeamLeader())
			//	{
			//		MMOTeamMember*pkMate = teamMgr.GetMemberById( msg->dwLeadID);
			//		if(pkMate)
			//		{
			//			if(etaFreeForAll == msg->byAssignType)
			//			{
			//				FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamMessage.PickTypeChange")),*(pkMate->ActorName),*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.FreeAll")));
			//				MMOUI::GetUIManager()->delegateAddScreenMessage(strHint,MsgInfoType_Normal);
			//				//MMOChatManager::getSingleton().AddHintMessage(strHint);
			//				MMOChatManager::getSingleton().AddSystemMessage(strHint);
			//			}
			//			else if(etaTeamAuction == msg->byAssignType)
			//			{
			//				FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamMessage.PickTypeChange")),*(pkMate->ActorName),*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamMessage.PickTeamAuction")));
			//				MMOUI::GetUIManager()->delegateAddScreenMessage(strHint,MsgInfoType_Normal);
			//				//MMOChatManager::getSingleton().AddHintMessage(strHint);
			//				MMOChatManager::getSingleton().AddSystemMessage(strHint);
			//			}
			//		}
			//	}
			//}
			if(teamMgr.GetAuctionItemColorLimit() != msg->byAuctionItemColorLimit)
			{
				if(!teamMgr.IsTeamLeader())
				{
					MMOTeamMember*pkMate = teamMgr.GetMemberById( msg->dwLeadID);
					if(pkMate)
					{
						if(etaTeamAuction == teamMgr.AssignType &&  msg->byAuctionItemColorLimit != teamMgr.GetAuctionItemColorLimit())
						{
							INT curColor = msg->byAuctionItemColorLimit;
							INT curIdx = teamMgr.GetTeamAuctionItemColorIndex( curColor );
							TeamAuctionItemColorLimitList& list = teamMgr.GetLimitColorMap();
							FString str = MMOUI::ParseLocalizedPropertyPath( list(curIdx).ColorStringText );
							FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamMessage.PickItemAuctionColor")),*(pkMate->ActorName),*str);
							MMOUI::GetUIManager()->delegateAddScreenMessage(strHint,MsgInfoType_Normal);
							//MMOChatManager::getSingleton().AddHintMessage(strHint);
							MMOChatManager::getSingleton().AddSystemMessage(strHint);
						}
					}
				}
			}
			//teamMgr.AssignType = msg->byAssignType;
			teamMgr.MemberLimit = msg->nTeamMemberLimit;
			teamMgr.SetAuctionItemColorLimit( msg->byAuctionItemColorLimit );
			teamMgr.SetTeamName( UTF8_TO_TCHAR(msg->szTeamName) );

			MMOGameEvent_Any evt( GameEvent_TeamMemberUpdate );
			MMOUI::PostGameEvent(&evt);
		}

	}
}
//-----------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_team_sync_member(const char* buf, UINT32 size)
{
	G2C_TEAM_SYNC_MEMBER* msg = (G2C_TEAM_SYNC_MEMBER*)buf;

	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	if( pl == NULL ){
		return;
	}

	TeamManager& pTeamMgr = TeamManager::getSingleton();
	
	MMOTeamMember *pTeammate = pTeamMgr.GetMemberById( msg->dwRoleID );
	if( pTeammate == NULL ){
		return;
	}

	pTeammate->Lv = msg->nLevel;
	MMOGameEvent_OtherPlayerLevelUp evt;
	evt.roleID = msg->dwRoleID;
	evt.lv = msg->nLevel;
	MMOUI::PostGameEvent(&evt);

	FVector serverPos( msg->wX, msg->wY, msg->wZ);
	//FServerGridWorld::ConvertPosServer2Client(serverPos.X,serverPos.Y,serverPos.Z,serverPos);
	ConvertCoordN2C( serverPos );
	pTeammate->WorldPos = serverPos;

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	INT curMyMapID = pMapDataMgr->GetCurMap().ServerID;
	if( pTeammate->MapID != curMyMapID ){
		pTeammate->MapID = curMyMapID;

		pMapDataMgr->AddOrUpdateMapDymcActorGlobal( pTeammate->ActorServerId, EACTMUI_GlobalTeammate, pTeammate->MapID, pTeammate->ActorName );
	}
	//pTeammate->Lv = msg->nLevel;
	//pTeammate->HPPecent = ( msg->nCurHp > 0 ? (msg->nCurHp / msg->nMaxHp) : 0 );
	//pTeammate->HPPecent = ( msg->nCurMp > 0 ? (msg->nCurMp / msg->nMaxMp) : 0 );

}

//
void CMessageHandler::Parse_m2c_team_sync_memberinfo(const char* buf, UINT32 size)
{
	M2C_TEAM_SYNC_MEMBERINFO* msg = (M2C_TEAM_SYNC_MEMBERINFO*)buf;

	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	if( pl == NULL ){
		return;
	}

	TeamManager& pTeamMgr = TeamManager::getSingleton();

	MMOTeamMember *pTeammate = pTeamMgr.GetMemberById( msg->stMember.dwRoleID );
	if( pTeammate == NULL ){
		return;
	}

	//INT memberLastMapID = pTeammate->MapID;
	//if( memberLastMapID != msg->stMember.mapID ){
	//	pTeammate->MapID = msg->stMember.;

	//	pMapDataMgr->AddOrUpdateMapDymcActorGlobal( pTeammate->ActorServerId, EACTMUI_GlobalTeammate, pTeammate->MapID, pTeammate->ActorName );
	//}

	INT curMyMapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	//if( pTeammate->MapID != curMyMapID ){
		FVector serverPos( msg->stMember.nX, msg->stMember.nY, msg->stMember.nZ);
		//FServerGridWorld::ConvertPosServer2Client(serverPos.X,serverPos.Y,serverPos.Z,serverPos);
		ConvertCoordN2C( serverPos );
		
		pTeammate->WorldPos = serverPos;
	//}

	pTeammate->Lv = msg->stMember.nLevel;
	pTeammate->HP = msg->stMember.nCurHp;
	pTeammate->HPPecent = ( msg->stMember.nCurHp == 0 ) ? (0.001) : (FLOAT)msg->stMember.nCurHp / msg->stMember.nMaxHp;
	
	pTeammate->MPPecent = ( msg->stMember.nCurMp == 0 ) ? (0.001) : (FLOAT)msg->stMember.nCurMp / msg->stMember.nMaxMp;

	//
	pTeammate->MapID = msg->stMember.dwSceneTempID;
	pTeammate->MapLineID = msg->stMember.nSceneID;

	MMOGameEvent_OneInt evt( GameEvent_TeamMemberUpdate, msg->stMember.dwRoleID );
	MMOUI::PostGameEvent(&evt);


}
////----------------------------------------------------------------------------
//void CMessageHandler::Parse_m2c_team_sync_memberinfo(const char* buf, UINT32 size)
//{
//	M2C_TEAM_SYNC_MEMBERINFO* msg = (M2C_TEAM_SYNC_MEMBERINFO*)buf;
//	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
//	UTeamManager* TM = pl->TeamManager;
//	if( TM && TM->TeamID == msg->dwTeamID )
//	{
//		for (int i = 0 ; i < msg->byMemberCount ; i++ )
//		{
//			//if( TM->TeamMemberList(i).ActorServerId == msg->dwMemberID )
//			{
//
//				int pos = TM->GetMemberPosByMemberId( msg->stMemberList[i].dwRoleID );
//				MMOFacade::getSingleton().SetTeamMemberOnline( msg->stMemberList[i].dwRoleID , msg->stMemberList[i].byState == etsOnline ,pos  );
//				MMOFacade::getSingleton().SetTeamMemberLv( msg->stMemberList[i].dwRoleID , msg->stMemberList[i].nLevel , pos);
//				MMOFacade::getSingleton().SetTeamMemberHP( msg->stMemberList[i].dwRoleID , msg->stMemberList[i].nCurHp , msg->stMemberList[i].nMaxHp , pos );
//				MMOFacade::getSingleton().SetTeamMemberMP( msg->stMemberList[i].dwRoleID , msg->stMemberList[i].nCurMp , msg->stMemberList[i].nMaxMp , pos );
//				MMOFacade::getSingleton().NotifyUpdateTeamMate( msg->stMemberList[i].dwRoleID , pos );
//				MMOFacade::getSingleton().NotifyUpdateMyTeam();
//			}
//		}
//
//	}
//}
////----------------------------------------------------------------------------
//void CMessageHandler::Parse_g2c_teamitem_list(const char* buf, UINT32 size)
//{
//    G2C_TEAMITEM_LIST* msg = (G2C_TEAMITEM_LIST*)buf;
//    AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
//    pl->TeamManager->TeamInfoList.Empty();
//    //pl->TeamManager->TeamInfoList.Add( msg->shTeamCount );
//    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//
//    for(UINT32 i = 0;i< msg->dwCount; i++)
//    {			
//        FMMOTeamInfo temp (EC_EventParm);
//        temp.TeamID = msg->astTeamItemList[i].dwTeamID;
//        //temp.LeadLv = msg->astTeamItemList[i];
//        temp.TeamName =  (msg->astTeamItemList[i].aszTeamName) ;
//        temp.MemberCount = msg->astTeamItemList[i].dwCurMemberCount;
//        temp.MaxMemberCount = msg->astTeamItemList[i].dwMaxMemberCount;
//        pl->TeamManager->TeamInfoList.AddItem(temp);
//    }
//    AMMOHud* hud = Cast<AMMOHud>(ctrl->myHUD);
//    if ( hud && hud->HudMovie&&hud->HudMovie->_Team)
//    {
//        hud->HudMovie->_Team->eventUpdateTeamContent();
//    }
//
//}
////----------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_team_leave(const char* buf, UINT32 size)
{
	M2C_TEAM_LEAVE* msg = (M2C_TEAM_LEAVE*)buf;
	
	if( msg->dwTeamID >= 0 )
	{
		//clear all teammate map data
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.ILeaveTeam")) ,MsgInfoType_Table,TEXT("MMOGame.Team.ILeaveTeam"));
		//
		TeamManager::getSingleton().LeaveMyTeam();

		//MMOFacade::getSingleton().NotifyUpdateMyTeam();
		MMOGameEvent_Any evt(GameEvent_TeamLeave);
		MMOUI::PostGameEvent(&evt);

	}
}
//
void CMessageHandler::Parse_m2c_team_answer(const char* buf, UINT32 size)
{
	M2C_TEAM_ANSWER* msg = (M2C_TEAM_ANSWER*)buf;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( !ctrl)
		return;
	if( msg->byAnswerType ==  etatInvite )
	{
		TeamManager::getSingleton().InviteTeamRequest( UTF8_TO_TCHAR( msg->szRequestRoleName ) , msg->dwAnswerRequestRoleID );
		
	}
	else if( msg->byAnswerType == etatEnter )
	{
		TeamManager::getSingleton().AddTeamRequest(UTF8_TO_TCHAR( msg->szRequestRoleName ) , msg->dwAnswerRequestRoleID);
		//MMOUI::GetUIManager()->delegateAddTeamRequest( UTF8_TO_TCHAR( msg->szRequestRoleName ) , msg->dwAnswerRequestRoleID );
		
	}

}
////----------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_team_add_member(const char* buf, UINT32 size)
{
	M2C_TEAM_ADD_MEMBER* msg = ( M2C_TEAM_ADD_MEMBER* ) buf;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	if( pl )
	{
		//It is self, so return
		if( pl->ServerID == msg->stAddMember.dwRoleID)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.IAddTeam")),MsgInfoType_Table,TEXT("MMOGame.Team.IAddTeam") );
			return;
		}

		TeamManager& teamMgr = TeamManager::getSingleton();
		if( msg->dwTeamID != teamMgr.TeamID )
			return;

		MMOTeamMember mb;
		mb.ActorName = UTF8_TO_TCHAR( msg->stAddMember.szRoleName );
		mb.ClassId = msg->stAddMember.byClass;
		mb.Sex = msg->stAddMember.bySex;
		mb.ActorServerId = msg->stAddMember.dwRoleID;
		mb.HPPecent = 0.1;
		mb.Lv = 1;
		mb.MPPecent = 0.1;
		mb.Online = TRUE;
		mb.isLeader = FALSE;
		mb.UIPos = -1;
		teamMgr.AddTeamMember( mb );
		
		if( mb.ActorServerId != pl->ServerID ){
			//add teammate map data
			pMapDataMgr->AddOrUpdateMapDymcActorGlobal( mb.ActorServerId, EACTMUI_GlobalTeammate, -1, mb.ActorName );
		}

		FString text = mb.ActorName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.PlayerAddTeam") );
		MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Table,TEXT("MMOGame.Team.PlayerAddTeam"));

		MMOChatManager::getSingleton().AddSystemMessage(text);
		//MMOFacade::getSingleton().NotifyUpdateMyTeam();
		MMOGameEvent_Any evt(GameEvent_TeamAddMember);
		MMOUI::PostGameEvent(&evt);

		AMMOActor* pActor = MMOFacade::getSingleton().GetCharacter(msg->stAddMember.dwRoleID);
		if(pActor)
		{
			MMOUI::GetUIManager()->delegateUpdateHintText(pActor,ESHU_ALL);
		}
	}
}
////----------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_team_remove_member(const char* buf, UINT32 size)
{
	M2C_TEAM_REMOVE_MEMBER* msg = ( M2C_TEAM_REMOVE_MEMBER* ) buf;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if( pl )
	{
		TeamManager& teamMgr = TeamManager::getSingleton();
		if( msg->dwTeamID != teamMgr.TeamID )
			return;

		MMOTeamMember* member = teamMgr.GetMemberById( msg->dwRemoveMemberID );
		if( member )
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( member->ActorName + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.PlayerLeaveTeam")) ,MsgInfoType_Table,TEXT("MMOGame.Team.PlayerLeaveTeam"));
			MMOChatManager::getSingleton().AddSystemMessage(member->ActorName + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.PlayerLeaveTeam")));
		}

		teamMgr.RemoveTeamMember( msg->dwRemoveMemberID );

		AMMOActor* pActor = MMOFacade::getSingleton().GetCharacter(msg->dwRemoveMemberID);
		if(pActor)
		{
			MMOUI::GetUIManager()->delegateUpdateHintText(pActor,ESHU_ALL);
		}
		MMOGameEvent_Any evt(GameEvent_TeamRemoveMember);
		MMOUI::PostGameEvent(&evt);
	}
}
////----------------------------------------------------------------------------initial sync once
//void CMessageHandler::Parse_g2c_sync_teaminfo(const char* buf, UINT32 size)
//{
//	G2C_SYNC_TEAMINFO* msg = (G2C_SYNC_TEAMINFO*)buf;
//	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
//
//	if( pl && pl->TeamManager)
//	{
//		UTeamManager* TM = pl->TeamManager;
//
//		TM->CreateMyTeam( msg->dwTeamID );
//		TM->AssignType = msg->byAssignType;
//		TM->MemberLimit = msg->nMaxMemberCount;
//		TM->TeamName  = UTF8_TO_TCHAR( msg->szTeamName);
//		
//
//		for (UINT32 i = 0 ;i<msg->nMemberCount  ;i++ )
//		{
//			TEAM_MEMBER MI = msg->stMemberTable[i];
//			FMMOTeamMember Member(EC_EventParm);
//			Member.ActorName = UTF8_TO_TCHAR(MI.szRoleName);
//			Member.ActorServerId = MI.dwRoleID ;
//			Member.Lv = MI.btLevel ;
//			Member.ClassId = MI.btClass;
//			Member.Sex = MI.btSex;
//			Member.isLeader = false;
//			Member.Online = false;
//			Member.HPPecent = 0.1;
//			Member.MPPecent = 0.1;
//			Member.UIPos = -1;
//			TM->AddTeamMember(Member);
//			TM->SetTeamLeaderRoleId( msg->dwLeader );
//			TM->AssignType = msg->byAssignType;
//			TM->MemberLimit = msg->nMaxMemberCount;
//		}
//	}
//	MMOFacade::getSingleton().NotifyUpdateMyTeam();
//}

//-----------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_adjacent_role_noteam(const char* buf, UINT32 size)
{
	//Client judges locally, this protocol is no longer needed
	//G2C_SYNC_ADJACENT_ROLE_NOTEAM* msg = (G2C_SYNC_ADJACENT_ROLE_NOTEAM*)buf;
	//for (UINT32 i = 0 ; i<msg->byCount  ;i++ )
	//{
	//	UTeamManager* TM = pl->TeamManager;
	//	TM->
	//}
	
}
//-----------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_role_team(const char* buf, UINT32 size)
{
	G2C_SYNC_ROLE_TEAM* msg = (G2C_SYNC_ROLE_TEAM*)buf;

	AMMOActor* pl = MMOFacade::getSingleton().GetCharacter(msg->dwRoleID);
    if(pl != NULL)
	{
		pl->TeamID = msg->dwTeamID;
		MMOGameEvent_Any evt( GameEvent_TeamNearPlayerChange );
		MMOUI::PostGameEvent(&evt);
	}
}
//-----------------------------------------------------------
void CMessageHandler::Parse_m2c_team_sync_memberstate(const char* buf, UINT32 size)
{
	M2C_TEAM_SYNC_MEMBERSTATE* msg = (M2C_TEAM_SYNC_MEMBERSTATE*)buf;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	
	INT pos = TeamManager::getSingleton().GetMemberPosByMemberId(msg->dwRoleID);
	if( pos >= 0 )
	{
		MMOTeamMember* teamMember = TeamManager::getSingleton().GetTeamMember(pos);
		if(teamMember)
		{
			teamMember->Online = ( msg->byState == etsOnline );
			//MMOFacade::getSingleton().NotifyUpdateMyTeam();
			MMOGameEvent_OneInt evt( GameEvent_TeamMemberUpdate, msg->dwRoleID );
			MMOUI::PostGameEvent(&evt);
		}
		
	}
}
//-----------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_adjacent_team(const char* buf, UINT32 size)
{
	    G2C_SYNC_ADJACENT_TEAM* msg = (G2C_SYNC_ADJACENT_TEAM*)buf;
	    AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	    TeamManager::getSingleton().GetTeamInfoList().Empty();
	    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	
	    for(UINT32 i = 0;i< msg->byCount; i++)
	    {			
	        MMOTeamInfo temp ;
	        temp.TeamID = msg->stTeamList[i].dwTeamID;
	        temp.LeadLv = msg->stTeamList[i].btCaptainLevel;
	        temp.TeamName =  UTF8_TO_TCHAR(msg->stTeamList[i].szTeamName) ;
			temp.TeamLeaderName = UTF8_TO_TCHAR(msg->stTeamList[i].szCaptainName);
	        temp.MemberCount = msg->stTeamList[i].btMemberCount;
	        temp.MaxMemberCount = msg->stTeamList[i].btMaxCount;
			temp.TeamLeaderServeId = msg->stTeamList[i].dwCaptainID;
			const ActivityData* activityData = ActivityManager::getSingleton().GetActivityData( msg->stTeamList[i].dwActivityID );
			if(activityData)
			{
				temp.Targe = activityData->sActivityName;
			}
			else
			{
				const FBActivityData* pkData = ActivityManager::getSingleton().GetFBActivityData( msg->stTeamList[i].dwActivityID );
				if(pkData)
					temp.Targe = pkData->sFBName;
				else
					temp.Targe = TEXT("");
			}			
	        TeamManager::getSingleton().GetTeamInfoList().AddItem(temp);
	    }
	    AMMOHud* hud = Cast<AMMOHud>(ctrl->myHUD);

		MMOFacade::getSingleton().NotifyUpdateTeamContent();
	        
	    
}

//-----------------------------------------------------------
void CMessageHandler::Parse_g2c_battle_team_change_leader(const char* buf, UINT32 size)
{
	G2C_BATTLE_TEAM_CHANGE_LEADER* msg = (G2C_BATTLE_TEAM_CHANGE_LEADER*)buf;

	TeamManager& teamMgr = TeamManager::getSingleton();
	teamMgr.SetTeamLeaderRoleId( msg->dwNewLeader );
}

void CMessageHandler::Parse_m2c_team_sync_team_info(const char* buf, UINT32 size)
{
	M2C_TEAM_SYNC_TEAM_INFO* msg = (M2C_TEAM_SYNC_TEAM_INFO*)buf;
	if (msg)
	{
		if (msg->dwLeaderID >0 && msg->dwTeamID >= 0)
		{
			TeamManager::getSingleton().NotifyEnterTeam();
		}
		else
		{
			TeamManager::getSingleton().NotifyCreateTeam();
		}
	}
}
