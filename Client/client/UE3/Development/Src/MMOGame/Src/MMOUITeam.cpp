#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "team_def.h"
#include "MMOMsgButtonManager.h"
#include "TeamManager.h"
#include "CommonGameEvents.h"
#include "MMOGuideManager.h"
#include "ActivityManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUITeam)
//IMPLEMENT_CLASS(UMMOUIBase)
#define MAX_UPDATE_TIME 60
#define ValidGfxObjectFunction( mc , func )  if( mc ){ (mc)->func;}

void UMMOUITeam::SendTeamList()
{
	CMessageHandler::GetInstance().Request_TeamList();
}
//------------------------------------------------------------------------
void UMMOUITeam::SendTeamAdd( INT RoleId,INT TeamID )
{
	CMessageHandler::GetInstance().Send_EnterTeam(RoleId);
}

//------------------------------------------------------------------------
void UMMOUITeam::SendTeamAddAnswer(INT RoleId,UBOOL agree)
{
	CMessageHandler::GetInstance().Send_TeamAnswer(RoleId ,agree,etatEnter );
}
//------------------------------------------------------------------------
void UMMOUITeam::SendTeamInviteAnswer(INT RoleId,UBOOL agree)
{
	CMessageHandler::GetInstance().Send_TeamAnswer(RoleId ,agree,etatInvite );
}
//------------------------------------------------------------------------
void UMMOUITeam::CreateTeam(const FString& TeamName,INT AssignType)
{
	CMessageHandler::GetInstance().Send_EnterTeam( Player->ServerID );

	CMessageHandler::GetInstance().Send_ChangeTeamActivity(TeamManager::getSingleton().GetTeamTargetId(), TeamManager::getSingleton().GetTeamTargetType());
}

void UMMOUITeam::NativeInitOnce()
{
	//if( TeamExpandButton )
	//	TeamExpandButton->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.ExpandTeam") ) );
	//if( SaveNameButton )
	//	SaveNameButton->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Save") ) );
	//if( InvitePlayerButton )
	//	InvitePlayerButton->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.AddTeam") ) );
	//if( EnterTeamButton )
	//	EnterTeamButton->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.AddTeam") ) );

	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if( data )
	{
		data->SetString( TEXT("panelName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Team") ) );
		data->SetString( TEXT("buttonTeamExpand") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.ExpandTeam") ) );
		data->SetString( TEXT("buttonSaveName") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Save") ) );
		data->SetString( TEXT("buttonLeaveTeam") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.LeaveTeam") ) );
		data->SetString( TEXT("buttonEnterTeam") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.AddTeam") ) );
		data->SetString( TEXT("buttonInvitePlayer") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.InviteTeam") ) );
		data->SetString( TEXT("tab0") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.MyTeam") ) );
		data->SetString( TEXT("tab1") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.NearbyPlayer") ) );
		data->SetString( TEXT("tab2") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.NearbyTeam") ) );
		eventCallSetPanelText(data);
	}

	//obj = GetObject( TEXT("tab0_mc") );
	//if( obj )
	//	obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.MyTeam") ) );

	//obj = GetObject( TEXT("tab1_mc") );
	//if( obj )
	//	obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath(  TEXT("MMOGame.Team.NearbyPlayer") ) );

	//obj = GetObject( TEXT("tab2_mc") );
	//if( obj )
	//	obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.NearbyTeam") ) );
}
//------------------------------------------------------------------------
//void UMMOUITeam::LeaveTeam( INT RoleId )
//{
//	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
//	INT mapType = UGlobalMapInfo::GetSingleton()->FindMapInfoByName(playerCtrl->MapName).ServerMapType ;
//	if( mapType == E_est_Static || mapType == E_est_Line  )
//	{
//		CMessageHandler::GetInstance().Send_TeamLeave(RoleId);
//	}
//	else
//	{
//		//Not on a field map, cannot leave team
//		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.SceneCannotLeaveTeam") ) );
//	}
//	
//	//TM->LeaveMyTeam();
//}

//------------------------------------------------------------------------
void UMMOUITeam::InviteAddTeam(INT RoleId)
{
	//CMessageHandler::GetInstance().Send_EnterTeam( RoleId );
	TeamManager::getSingleton().QueryRoleHasTeam(RoleId);
}
//------------------------------------------------------------------------------------------------------------------------
void UMMOUITeam::UpdateMyTeam()
{

	if( !MemberItem5 || !MemberItem20   )
	{
		return;
	}
	

	MemberItem5->SetVisible(false);
	MemberItem20->SetVisible(false);

	TeamManager& teamMgr = TeamManager::getSingleton();
	if(TeamChangeTargetButton)
		TeamChangeTargetButton->SetBool("enabled",true);
	if(TeamClearTargetButton)
		TeamClearTargetButton->SetBool("enabled",true);
	

	if( teamMgr.HasTeam() )
	{
		ValidGfxObjectFunction( TeamCreateOrLeaveButton , SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.LeaveTeam") ) ) );
		
		ValidGfxObjectFunction( TeamNameTextField , SetVisible(TRUE) );
		//ValidGfxObjectFunction( TeamNameFrame , SetVisible(TRUE) );
		
		//Team leader
		if( teamMgr.IsTeamLeader() )
		{
			//ValidGfxObjectFunction( AssignTypeButtonFree , SetBool( TEXT("enabled") , true ) );
			//ValidGfxObjectFunction( AssignTypeButtonTurn , SetBool( TEXT("enabled") , true ) );
			ValidGfxObjectFunction( AutoTeamButton , SetBool( TEXT("enabled") , true ) );
			ValidGfxObjectFunction( SaveNameButton , SetVisible(true) );
			ValidGfxObjectFunction( TeamNameTextField , SetBool( TEXT("enabled") , true ) );
			//ValidGfxObjectFunction( TeamExpandButton , SetVisible(true) );
			ValidGfxObjectFunction( TeamChangeTargetButton, SetVisible(true) );
			ValidGfxObjectFunction( TeamClearTargetButton, SetVisible(true) );
			ValidGfxObjectFunction( TeamAuctionColorLimit , SetVisible(true) );
			ValidGfxObjectFunction( TeamAuctionColorLimit , SetBool( TEXT("enabled") , true ) );
			CenterCreateLeaveButton(false);

			eventCallSetAutoTeamVisible(true);
		}
		else
		{
			//Only need to update team name if not team leader
			//ValidGfxObjectFunction( TeamNameTextField , SetText( teamMgr.GetTeamName() ) );

			//ValidGfxObjectFunction( AssignTypeButtonFree , SetBool( TEXT("enabled") , false ) );
			ValidGfxObjectFunction( AutoTeamButton , SetBool( TEXT("enabled") , false ) );
			//ValidGfxObjectFunction( AssignTypeButtonTurn , SetBool( TEXT("enabled") , false ) );
			ValidGfxObjectFunction( SaveNameButton , SetVisible(false) );
			ValidGfxObjectFunction( TeamNameTextField , SetBool( TEXT("enabled") , false ));
			//ValidGfxObjectFunction( TeamExpandButton , SetVisible(false) );
			ValidGfxObjectFunction( TeamChangeTargetButton, SetVisible(false) );
			ValidGfxObjectFunction( TeamClearTargetButton, SetVisible(false) );
			ValidGfxObjectFunction( TeamAuctionColorLimit , SetBool( TEXT("enabled") , false ) );
			CenterCreateLeaveButton(true);

			eventCallSetAutoTeamVisible(false);
		}

		//Team loot
		if( teamMgr.GetAssignType() == etaFreeForAll )
		{
			if( AssignTypeButtonFree )
			{
				AssignTypeButtonFree->SetBool( "selected" , TRUE );
			}
		}
		else if (teamMgr.GetAssignType() == etaTeamAuction)
		{
			if( AssignTypeButtonTurn )
			{
				AssignTypeButtonTurn->SetBool( "selected" , TRUE );
			}
		}
		else
		{
			if(AssignTypeLeader)
			{
				AssignTypeLeader->SetBool( "selected" , TRUE );
			}
		}

		eventCallSetTeamAuctionColorLimit( teamMgr.GetCurTeamAuctionItemColorIndex() );
				
	}
	else
	{
		//MMOFacade::getSingleton().NotifyUpdateTeamMateNum( 0 );
		ValidGfxObjectFunction( TeamCreateOrLeaveButton , SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.CreateTeam") )) );
		ValidGfxObjectFunction( TeamNameTextField , SetText( teamMgr.GetTeamName() ) );
		//ValidGfxObjectFunction( AssignTypeButtonFree , SetBool( TEXT("enabled") , false ) );
		//ValidGfxObjectFunction( AssignTypeButtonTurn , SetBool( TEXT("enabled") , false ) );
		ValidGfxObjectFunction( SaveNameButton , SetVisible(false) );
		//ValidGfxObjectFunction( TeamNameFrame , SetVisible(FALSE) );
		ValidGfxObjectFunction( TeamNameTextField , SetVisible(FALSE) );
		//ValidGfxObjectFunction( TeamExpandButton , SetVisible(false) );
		ValidGfxObjectFunction( TeamChangeTargetButton, SetVisible(true) );
		ValidGfxObjectFunction( TeamClearTargetButton, SetVisible(true) );
		ValidGfxObjectFunction( AutoTeamButton , SetBool( TEXT("enabled") , true ) );
		eventCallSetAutoTeamVisible(true);
		CenterCreateLeaveButton(true);
	}

	_UpdateMyTeamMember();

	if (TeamManager::getSingleton().getAutoTeam())
	{
		ValidGfxObjectFunction( AutoTeamButton , SetBool( TEXT("selected") , true ) );
	}
	else
	{
		ValidGfxObjectFunction( AutoTeamButton , SetBool( TEXT("selected") , false ) );
	}
}

void UMMOUITeam::_UpdateMyTeamMember()
{
	TeamManager& teamMgr = TeamManager::getSingleton();
	UMMOUIManager* UIMgr = MMOUI::GetUIManager();
	INT MyTeamMemberCount = teamMgr.GetTeamMemberCount();

	if( teamMgr.GetTeamMemberCount() > NormalTeamNum/*teamMgr.GetMemberLimit() >= LargeTeamNum*/ ) //large team
	{
		MemberItem20->SetVisible(true);	
		//ValidGfxObjectFunction( TeamExpandButton , SetVisible(false) );
		CenterCreateLeaveButton(true);

		for( INT i = 0 ; i< LargeTeamNum ; i++ )
		{
			UGFxObject* ListItem = MemberItem20->GetObject( FString::Printf( TEXT("MyTeamItem%d") , i ) );
			if( ListItem == NULL)
			{
				continue;
			}
			ListItem->SetVisible(false);

			if( i < MyTeamMemberCount ) //this teammate exists
			{
				MMOTeamMember* teamMember = teamMgr.GetTeamMember(i);
				if(teamMember)
				{
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("team_name") ) , SetText( teamMember->ActorName ) );
					/*ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderclass") ) , SetText( 
						MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), teamMember->ClassId ) ) )); */
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("teamLeaderTag") ) , SetVisible( teamMember->isLeader ) );
					//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderlv") ) , SetText( FString::Printf( TEXT("%d"), teamMember->Lv) ) ); 
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("mc_chapter") ) , GotoAndStop(FString::Printf( TEXT("_%d"), teamMember->ClassId)) );

					eventCallUCSetOnLineState(i,LargeTeamNum,teamMember->Online);
					if( teamMember->Online)
					{
						ValidGfxObjectFunction( ListItem->GetObject( TEXT("tag_offline_mc") ) , SetVisible(false) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("team_name") ) , SetInt( TEXT("textColor") , UIMgr->CareerFontColor( teamMember->ClassId) ) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderclass") ) , SetInt( TEXT("textColor") , UIMgr->CareerFontColor( teamMember->ClassId ) ) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderlv") ) , SetInt( TEXT("textColor") , UIMgr->CareerFontColor( teamMember->ClassId) ) );
					}
					else
					{
						ValidGfxObjectFunction( ListItem->GetObject( TEXT("tag_offline_mc") ) , SetVisible(true) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("team_name") ) , SetInt( TEXT("textColor") , UIMgr->OfflineFontColor ) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderclass") ) , SetInt( TEXT("textColor") , UIMgr->OfflineFontColor ) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderlv") ) , SetInt( TEXT("textColor") , UIMgr->OfflineFontColor ) );
					}

					ListItem->SetVisible(true);
				}					
			}
		}
	}
	else //small team
	{
		MemberItem5->SetVisible(true);
		for( INT i = 0 ; i< NormalTeamNum ; i++ )
		{
			UGFxObject* ListItem = MemberItem5->GetObject( FString::Printf( TEXT("MyTeamItem%d"),i));
			if( ListItem == NULL )
			{
				continue;
			}
			ListItem->SetVisible(false);

			if( i < MyTeamMemberCount ) //
			{
				MMOTeamMember* teamMember = teamMgr.GetTeamMember(i);
				if(teamMember)
				{
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("team_name") ) , SetText( teamMember->ActorName ) );
					/*ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderclass") ) , SetText(
						MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(teamMember->ClassId)  ) )); */
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("teamLeaderTag") ) , SetVisible( teamMember->isLeader ) );
					//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderlv") ) , SetText( appItoa( teamMember->Lv ) ) ); 
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("playerIcon") ) , SetString( TEXT("source") , 
						MMOFacade::getSingleton().GetPlayerIcon( teamMember->ClassId , teamMember->Sex ) ) );
					ValidGfxObjectFunction( ListItem->GetObject( TEXT("mc_chapter") ) , GotoAndStop(FString::Printf( TEXT("_%d"), teamMember->ClassId)) );

					ListItem->SetVisible(true);
					switch( teamMember->ClassId )
					{
					case EPCT_PlayerClassA:
						ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("warrior")) );
						break;
					case EPCT_PlayerClassB:
						ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("mage")) );
						break;
					case EPCT_PlayerClassC:
						ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("assassin")) );
						break;
					case EPCT_PlayerClassD:
						ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("archer")) );
						break;
					case EPCT_PlayerClassG: // 1.05 Lancer: no dedicated frame, reuse warrior
							ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("warrior")) );
							break;
						case EPCT_PlayerClassH: // 1.05 Sorcerer: no dedicated frame, reuse mage
							ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("mage")) );
							break;
						case EPCT_PlayerClassE:
						ValidGfxObjectFunction( ListItem->GetObject(TEXT("avarta")) , GotoAndStop(TEXT("taoist")) );
						break;
					}
					eventCallUCSetOnLineState(i,NormalTeamNum,teamMember->Online);
					if( teamMember->Online )
					{
						ValidGfxObjectFunction( ListItem->GetObject( TEXT("tag_offline_mc") ) , SetVisible(false) );
						ValidGfxObjectFunction( ListItem->GetObject( TEXT("tF_offline") ) , SetVisible(false) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("team_name") ) , SetInt( TEXT("textColor") , UIMgr->CareerFontColor(teamMember->ClassId) ) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderclass") ) , SetInt( TEXT("textColor") , UIMgr->CareerFontColor(teamMember->ClassId) ) );
					}
					else
					{
						ValidGfxObjectFunction( ListItem->GetObject( TEXT("tag_offline_mc") ) , SetVisible(true) );
						ValidGfxObjectFunction( ListItem->GetObject( TEXT("tF_offline") ) , SetVisible(true) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("team_name") ) , SetInt( TEXT("textColor") , UIMgr->OfflineFontColor ) );
						//ValidGfxObjectFunction( ListItem->GetObject( TEXT("leaderclass") ) , SetInt( TEXT("textColor") , UIMgr->OfflineFontColor ) );
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------
void UMMOUITeam::CenterCreateLeaveButton(UBOOL center)
{
	////This key moves two positions
	//if( center )
	//{
	//	ValidGfxObjectFunction( TeamCreateOrLeaveButton , SetFloat( TEXT("x") , 111 ) ); 
	//}
	//else 
	//{
	//	ValidGfxObjectFunction( TeamCreateOrLeaveButton , SetFloat( TEXT("x") , 199 ) ); 
	//}
}

//------------------------------------------------------------------------

void UMMOUITeam::SetNearPlayerPage()
{
	if(!(IsVisible&&NearbyPlayerPanel&&NearbyPlayerPanel->GetBool(TEXT("visible"))))
		return;

	TeamManager::getSingleton().GetNearbyPlayerList();
	INT NearbyPlayerNum = TeamManager::getSingleton().GetNearbyPlayerListCount();
	NearPlayerMaxPage = NearbyPlayerNum / MaxNearPlayerPerPage;
	/*if( NearbyPlayerNum%MaxNearPlayerPerPage > 0 )
	NearPlayerMaxPage++;*/
	NearPlayerMaxPage = Clamp(NearPlayerMaxPage,1,10000 );
	NearPlayerCurPage = Clamp(NearPlayerCurPage , 0 , NearPlayerMaxPage-1);
	
	SetCurNearPlayerPage();
}

void UMMOUITeam::SetCurNearPlayerPage()
{
	if(NearPage)
	{
		
		if (NearPlayerCurPage <= 1)
			NearPlayerCurPage = 1;

		FString page = FString::Printf(TEXT("%d/%d"), NearPlayerCurPage,NearPlayerMaxPage);
		NearPage->GetObject(TEXT("tF_pageNum"))->SetText(page);
	}
}

void UMMOUITeam::UpdateNearbyPlayerContent()
{
	if(!(IsVisible&&NearbyPlayerPanel&&NearbyPlayerPanel->GetBool(TEXT("visible"))))
		return;

	UGFxObject* PlayerItem = NULL;
	
	//if( NearPlayerCurPage < 1 )
	//	NearPlayerCurPage = 1;
	//if( NearPlayerCurPage > NearPlayerMaxPage)
	//	NearPlayerCurPage = NearPlayerMaxPage;

	
	//if(NearPlayerPageTextField)
		//NearPlayerPageTextField->SetText( FString::Printf(TEXT("%d/%d"), NearPlayerCurPage+1,NearPlayerMaxPage) );
	INT NearbyPlayerNum = TeamManager::getSingleton().GetNearbyPlayerListCount();

	INT i = NearPlayerCurPage <= 1 ? 0 : NearPlayerCurPage*MaxNearPlayerPerPage + 0;

	INT index = 0;

	for ( i  ; i < (NearPlayerCurPage+1)*MaxNearPlayerPerPage ; i++ )
	{
		PlayerItem = NearbyPlayerPanel->GetObject( FString::Printf(TEXT("TeamContentItem%d"),index /*i - NearPlayerCurPage*MaxNearPlayerPerPage*/) );
		if( PlayerItem == NULL)
			continue;

		
		PlayerItem->SetVisible(false);
		if( i <  NearbyPlayerNum )
		{
			MMONearbyPlayer* tPlay = TeamManager::getSingleton().GetNearPlayer(i);
			if( tPlay )
			{
				PlayerItem->GetObject(TEXT("Playername"))->SetText( tPlay->ActorName );
				//PlayerItem->GetObject(TEXT("Playerlv"))->SetText( FString::Printf(TEXT("%d"), tPlay->Lv) );
				PlayerItem->GetObject(TEXT("Playerclass"))->SetText(
					MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"), tPlay->ClassId) ));

				if(tPlay->InTeam)
					PlayerItem->GetObject(TEXT("PlayerStatus"))->SetText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.InTeam")));
				else
					PlayerItem->GetObject(TEXT("PlayerStatus"))->SetText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.NoTeam")));
				PlayerItem->SetVisible(true);
			}
		}
		index++;
		
	}
	
}


//------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------
void UMMOUITeam::UpdateTeamContent()
{

	INT TeamListNum = TeamManager::getSingleton().GetNearbyTeamCount();

	NearTeamMaxPage = TeamListNum / MaxNearTeamPerPage;
	if( TeamListNum % MaxNearTeamPerPage > 0 )
		NearTeamMaxPage ++;

	NearTeamMaxPage = Clamp( NearTeamMaxPage , 1 , 10000 );
	NearTeamCurPage = Clamp( NearTeamCurPage , 0 , NearTeamMaxPage-1 );

	if(NearTeamPageTextField)
		NearTeamPageTextField->GetObject(TEXT("tF_pageNum"))->SetText(FString::Printf( TEXT("%d/%d") , NearTeamCurPage+1 , NearTeamMaxPage ));

	for( INT i = 0; i< MaxNearTeamPerPage ;i++ )
	{
		INT index = i + MaxNearTeamPerPage*NearTeamCurPage;
		UGFxObject* mc = NearbyTeamPanel->GetObject( FString::Printf( TEXT("TeamContentItem%d") , i ) );
		if( mc == NULL )
			continue;
		mc->SetVisible(false);

		if( TeamListNum <= index )
			continue;			
		mc->SetVisible(true);

		MMOTeamInfo* teamInfo = TeamManager::getSingleton().GetNearTeam(index);
		if( teamInfo )
		{
			mc->GetObject("team_name")->SetText( teamInfo->TeamName );
			//mc->GetObject("leader_name")->SetText( teamInfo->TeamLeaderName );
			mc->GetObject("leader_name")->SetText( teamInfo->Targe );
			mc->GetObject("TeammateCount")->SetText( FString::Printf( TEXT("%d/%d") , teamInfo->MemberCount , teamInfo->MaxMemberCount ) ); 

		}
		
	}
}

void UMMOUITeam::InviteTeamCancel(class UGFxObject* mc)
{
	if( mc )
	{
		INT MsgInfoId = mc->GetInt( TEXT("iteminfoid") );
		SendTeamInviteAnswer( MsgInfoId , false );
	}
	
}

void UMMOUITeam::InviteTeamRequest(const FString& RoleName,INT RoleId)
{
	if( AutoTeam )
	{
		SendTeamInviteAnswer( RoleId , TRUE );
	}
	else
	{
        if (MMOUI::GetUIManager()->DeclineInviteTeam)
        {
            SendTeamInviteAnswer( RoleId , FALSE );
        }
        else
        {
		    FString title = RoleName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Inviteadd") );

		    MMOMsgButtonManager::getSingleton().AddNewButton( title , RoleId , RoleName , GameEvent_InvitedTeam , E_MsgBtnShow_Team );
        }
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
void UMMOUITeam::OnFocusLost()
{
	UMMOUIBase::OnFocusLost();

	//MMOUI::GetUIManager()->ShowPanel( MMOUI::GetUIManager()->Panel_RClickMenu , FALSE );
}

void UMMOUITeam::ClearUI()
{

}

void UMMOUITeam::TeamExpand()
{	
	if(!TeamNameTextField)
		return;
	if( TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader() )
	{
		FString teamName = TeamNameTextField->GetText();
		TeamManager::getSingleton().SendOperateTeam( TeamManager::getSingleton().GetAssignType() , LargeTeamNum , teamName , TeamManager::getSingleton().GetAuctionItemColorLimit() );
	}
}

void UMMOUITeam::SaveName()
{
	if(!TeamNameTextField)
	{
		return;
	}
	FString TeamName = TeamNameTextField->GetText();

	if( MMOUI::HasCensoredWord(TeamName) )
	{		
		//Team name invalid, restore original name
		TeamNameTextField->SetText( TeamManager::getSingleton().GetTeamName() );
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TeamNameInvalid")),MsgInfoType_Table,TEXT("MMOGame.Team.TeamNameInvalid") );
		return;
	}

	TeamManager::getSingleton().SendOperateTeam( TeamManager::getSingleton().GetAssignType() , LargeTeamNum , TeamName , TeamManager::getSingleton().GetAuctionItemColorLimit() );

}

//void UMMOUITeam::MoveLeader(INT roleid)
//{	
//	if(!TeamNameTextField)
//		return;
//	if( TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader() )
//	{
//		FString TeamName = TeamNameTextField->GetText();
//		SendOperateTeam( roleid  , TeamManager::getSingleton().GetAssignType() , TeamManager::getSingleton().GetMemberLimit() , TeamName );
//	}
//}

void UMMOUITeam::InviteNearbyPlayer()
{
	MMONearbyPlayer* pl = TeamManager::getSingleton().GetNearPlayer(SelectPlayerIndex);
	if(pl)
	{
		InviteAddTeam( pl->ActorServerId );
	}
	
}

void UMMOUITeam::SelectNearbyPlayer(INT id)
{
	INT index = NearPlayerCurPage - 1;
	if (index <= 0)
	{
		index = 0;
	}
	SelectPlayerIndex = id + index*MaxNearPlayerPerPage;
}

void UMMOUITeam::EnterNearbyTeam()
{
	if( TeamManager::getSingleton().HasTeam() )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.YouhaveTeam")) , MsgInfoType_Table , TEXT("MMOGame.Team.YouhaveTeam") );
		return;
	}
	MMOTeamInfo* team = TeamManager::getSingleton().GetNearTeam(SelectTeamIndex);
	if(team)
	{
		InviteAddTeam( team->TeamLeaderServeId );
	}
	
}

void UMMOUITeam::CreateOrLeaveTeam()
{
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if(!pl)
	{
		return;
	}
	if( !TeamManager::getSingleton().HasTeam() )
	{
		/*if(TeamManager::getSingleton().GetTeamTargetId() < 0 || TeamManager::getSingleton().GetTeamTargetType() < 0)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.NotEffectTarget")),MsgInfoType_Table,TEXT("MMOGame.Team.NotEffectTarget"));
		}
		else*/
		//{
			//CreateTeam( pl->ActorName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.ofTeam") ) , 0 );
		//}
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamSet, TRUE, "");
		
	}
	else
	{
		//TeamManager::getSingleton().LeaveTeam( pl->ServerID );
		MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam,pl->ServerID);
		MMOUI::PostGameEvent(&et);
	}
}

//void UMMOUITeam::InviteNearbyPlayer()
//{
//	MMONearbyPlayer* pl = TeamManager::getSingleton().GetNearPlayer(SelectPlayerIndex);
//	if(pl)
//	{
//		InviteAddTeam( pl->ActorServerId );
//	}
//
//}

void UMMOUITeam::AddTeamRequest(const FString& RoleName,INT RoleId)
{
	if(AutoTeam)
	{
		SendTeamAddAnswer(RoleId,true);
	}
	else
	{
		FString title = RoleName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Hopeadd") );
		//FScriptDelegate OnClickOK(EC_EventParm);
		//OnClickOK.Object = this;
		//OnClickOK.FunctionName = "AddTeamOk" ;

		//FScriptDelegate OnClickCancel(EC_EventParm);
		//OnClickCancel.Object = this;
		//OnClickCancel.FunctionName = "AddTeamCancel" ;

		MMOMsgButtonManager::getSingleton().AddNewButton( title , RoleId , RoleName , GameEvent_AddedTeam , E_MsgBtnShow_Team );
				
	}
}

void UMMOUITeam::ChangeAssignTypeFree()
{
	/*TeamManager& teamMgr = TeamManager::getSingleton();
	if( teamMgr.HasTeam() && teamMgr.IsTeamLeader() )
	{
		TeamManager::getSingleton().SendOperateTeam( etaFreeForAll , LargeTeamNum , teamMgr.GetTeamName() , TeamManager::getSingleton().GetAuctionItemColorLimit() );
	}*/
	
}

void UMMOUITeam::ChangeAssignTypeTurn()
{
	/*TeamManager& teamMgr = TeamManager::getSingleton();
	if(	teamMgr.HasTeam() && teamMgr.IsTeamLeader() )
	{
		TeamManager::getSingleton().SendOperateTeam( etaTeamAuction , LargeTeamNum , teamMgr.GetTeamName() , TeamManager::getSingleton().GetAuctionItemColorLimit() );
	}	*/
}


void UMMOUITeam::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if(show)
	{
		eventCallSetAutoTeamVisible(true);

		TeamContentTab(TabPage);
		UpdateTeamTargetUI();

		ValidGfxObjectFunction( TeamNameTextField , SetText( TeamManager::getSingleton().GetTeamName() ) );

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowTeamPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else
	{
	}
}

void UMMOUITeam::AddTeamOk(class UGFxObject* mc)
{
	if(mc)
	{
		INT MsgInfoId = mc->GetInt( TEXT("iteminfoid") );
		SendTeamAddAnswer( MsgInfoId , TRUE );
	}
	
}

void UMMOUITeam::AddTeamCancel(class UGFxObject* mc)
{
	if(mc)
	{
		INT MsgInfoId = mc->GetInt( TEXT("iteminfoid") );
		SendTeamAddAnswer( MsgInfoId , false );
	}

	
}

void UMMOUITeam::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{
// 	case GameEvent_ActorDestroyed:
// 		{
// 			MMOGameEvent_ActorDestroyed* actorEvt = (MMOGameEvent_ActorDestroyed*)evt;
// 			if(actorEvt->Actor && actorEvt->Actor->IsA(AMMOPlayer::StaticClass()))
// 			{
// 				UpdateNearbyPlayerContent();
// 			}
// 			break;
// 		}
	case GameEvent_TeamNearPlayerChange:
		{
			UpdateNearbyPlayerContent();
			break;
		}
	case GameEvent_TeamMemberUpdate:
	case GameEvent_TeamJoin:
	case GameEvent_TeamRemoveMember:
	case GameEvent_TeamLeave:
		{
			if(IsVisible)
			{
				UpdateMyTeam();

				if (TabPage == EUITeam_NearbyTeam)
				{
					bNearTeam = TRUE;
				}
				else
				{
					bNearTeam = FALSE;
					updateTime = 0;
				}
			}
			break;
		}
	case GameEvent_TeamNameChange:
		ValidGfxObjectFunction( TeamNameTextField , SetText( TeamManager::getSingleton().GetTeamName() ) );
		break;
	case GameEvent_AddedTeam:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if( cb->Agree )
				{
					AddTeamOk(cb->Box);
				}
				else
				{
					AddTeamCancel(cb->Box);
				}

			}
		}
		break;
	case GameEvent_InvitedTeam:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if( cb->Agree )
				{
					TeamManager::getSingleton().InviteTeamOk(cb->Box);
				}
				else
				{
					InviteTeamCancel(cb->Box);
				}

			}
		}
		break;
	case GameEvent_TeamCommon:
		{
			MMOGameEvent_TeamCommon* cb = (MMOGameEvent_TeamCommon*)(evt);
			if(cb)
			{
				if(cb->EventType == MMOGameEvent_TeamCommon::ETeamEventType_TargetChange)
				{
					UpdateTeamTargetUI();
				}
			}
		}
		break;
	case GameEvent_SendTeamExpand:
		{
			TeamExpand();
		}
		break;
	}

}

void UMMOUITeam::UpdateTeamTargetUI()
{
	UINT32 targetType = TeamManager::getSingleton().GetTeamTargetType();
	INT targetId = TeamManager::getSingleton().GetTeamTargetId();

	if(targetType == etaatInvalid || targetId < 0)
	{
		eventUpdateTeamTargetText(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.CurNoTarget") ));
		return;
	}

	if(targetType == etaatActivity)
	{
		//activity
		const ActivityData* ad = ActivityManager::getSingleton().GetActivityData(targetId);
		if(ad)
		{
			FString strAim = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.CurTargetIs") ),*(ad->sActivityName));
			eventUpdateTeamTargetText(strAim);
		}

	}
	else if(targetType == etaatDungeon)
	{
		//fb
		const FBActivityData* fd = ActivityManager::getSingleton().GetFBActivityData(targetId);
		if(fd)
		{
			FString strAim = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.CurTargetIs") ),*(fd->sFBName));
			eventUpdateTeamTargetText(strAim);
		}
	}
}

void UMMOUITeam::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_TeamMemberUpdate);
	evts.AddItem(GameEvent_TeamJoin);
	evts.AddItem(GameEvent_TeamRemoveMember);
	evts.AddItem(GameEvent_TeamLeave);	
	evts.AddItem(GameEvent_AddedTeam);
	evts.AddItem(GameEvent_InvitedTeam);
	evts.AddItem(GameEvent_TeamNameChange);
	evts.AddItem(GameEvent_TeamCommon);
	evts.AddItem(GameEvent_TeamNearPlayerChange);
	evts.AddItem(GameEvent_SendTeamExpand);
	//evts.AddItem(GameEvent_ActorDestroyed);

	EventAdapter = new TGameEventAdapter<UMMOUITeam>(this, evts);

	if(GFXPlayer)
	{
		UGFxObject* menuArray = GFXPlayer->CreateArray();
		if(menuArray)
		{
			TeamAuctionItemColorLimitList& list = TeamManager::getSingleton().GetLimitColorMap();
			
			for( INT i = 0 ; i < list.Num() ; i++ )
			{
				//FString text = MMOUI::ParseLocalizedPropertyPath( list(i).ColorStringText );
				FString text = UITextParser::HTMLBeginFont( UITextParser::HTMLGetItemColor( (EItemColor)list(i).Color ) )
					+ MMOUI::ParseLocalizedPropertyPath( list(i).ColorStringText )
					+ UITextParser::HTMLEndFont();

				menuArray->SetElementString( i , text );
			}
			eventCallSetDropDownMenu(menuArray);
		}
	}
	
	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}
	bNearTeam = FALSE;
	updateTime = 0;
}

void UMMOUITeam::UnInit()
{
	Super::UnInit();
	//if(TeamNameTextField)
	//{
	//	TeamNameTextField->SetText(TEXT(""));
	//}
	
}

//------------------------moved-------------------------------
void UMMOUITeam::ASClickClearTeamTarget()
{
	if(TeamManager::getSingleton().HasTeam())
	{
		CMessageHandler::GetInstance().Send_ChangeTeamActivity(-1, etaatInvalid);
	}
	else
	{
		TeamManager::getSingleton().SetTeamTargetId(-1);
		MMOGameEvent_TeamCommon evt(MMOGameEvent_TeamCommon::ETeamEventType_TargetChange);
		MMOUI::PostGameEvent(&evt);
	}
}
void UMMOUITeam::ASClickChangeTeamTarget()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamTarget, TRUE, "");
}

void UMMOUITeam::ASClickShowTeamInvite()
{
	if(TeamManager::getSingleton().HasTeam())
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamInvite, TRUE, "");
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotInTeam")),MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errTeamNotInTeam"));
	}
}
void UMMOUITeam::ASClickSelectPlayer(int playerId)
{
	TeamManager::getSingleton().AddInvitedPlayer(playerId);
}
void UMMOUITeam::ASClickUnSelectPlayer(int playerId)
{
	TeamManager::getSingleton().RemoveInvitedPlayer(playerId);
}
void UMMOUITeam::ASTeamMemberUIRightClick( INT x , INT y , INT index )
{	
	MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_TeamMember, x , y , 0 , "" , index );
}
void UMMOUITeam::ASTeamMemberUILeftClick( int x , int y , int index )
{
	TeamManager::getSingleton().SelectedNearTeamMate(index,FALSE);
}
void UMMOUITeam::ASAutoTeam( UBOOL sel )
{
	TeamManager::getSingleton().setAutoTeam(sel);

	//
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUITeam::ASTeamAuctionLimitChange( INT sel )
{

	if( TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader() )
	{
		INT colorLimit = TeamManager::getSingleton().GetTeamAuctionItemColorByIdx( sel );
		TeamManager::getSingleton().SendOperateTeam( TeamManager::getSingleton().GetAssignType() , LargeTeamNum , TeamManager::getSingleton().GetTeamName() , colorLimit );
	}
}

void UMMOUITeam::TeamContentTab(int tabid)
{	
		//local int i;

	TabPage = tabid;

	if( tabid == EUITeam_MyTeam ) 
	{
		NearbyPlayerPanel->SetVisible(false);
		NearbyTeamPanel->SetVisible(false);
		myTeamPanel->SetVisible(true);
		NearTeamCurPage = 0;
		UpdateMyTeam();
	}
	else if( tabid == EUITeam_NearbyPlayer ) //nearby players
	{
		NearbyPlayerPanel->SetVisible(true);
		NearbyTeamPanel->SetVisible(false);
		myTeamPanel->SetVisible(false);
		NearPlayerCurPage = 0;
		SetNearPlayerPage();
		UpdateNearbyPlayerContent();
	}
	else if( tabid == EUITeam_NearbyTeam )
	{
		NearbyPlayerPanel->SetVisible(false);
		NearbyTeamPanel->SetVisible(true);
		myTeamPanel->SetVisible(false);
		//TM.TeamInfoList.Remove( 0,TM.TeamInfoList.Length);
		UpdateTeamContent();
		SendTeamList( );
	}
	
}

void UMMOUITeam::BeforeViewUpdate(float DeltaTime)
{
	if( bNearTeam )
	{
		if(updateTime >= MAX_UPDATE_TIME)
		{
			SendTeamList();
			updateTime = 0;
		}
		updateTime += DeltaTime;
	}
}

void UMMOUITeam::NearbyPlayerPrePage()
{
	NearPlayerCurPage--;

	if (NearPlayerCurPage <= 0)
		NearPlayerCurPage = 0;
	
	SetCurNearPlayerPage();

	/*if (NearPlayerCurPage > 0)*/
		UpdateNearbyPlayerContent();
	
	

}

void UMMOUITeam::NearbyPlayerNextPage()
{
	NearPlayerCurPage++;

	if (NearPlayerCurPage >= NearPlayerMaxPage)
		NearPlayerCurPage = NearPlayerMaxPage;

	SetCurNearPlayerPage();

	/*if (NearPlayerCurPage < NearPlayerMaxPage )*/
		UpdateNearbyPlayerContent();

}