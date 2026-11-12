#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOMapDataManager.h"
#include "FSMIO.h"


IMPLEMENT_CLASS(UMMOUITeamMate)

//------------------------------------------------------------------------
void UMMOUITeamMate::UpdateTeamMate( INT UIPos , INT Pos )
{
	
	TeamManager& teamMgr = TeamManager::getSingleton();
	if( teamMgr.HasTeam() && teamMgr.GetTeamMemberCount() > Pos )
	{
		MMOTeamMember* teamMember = teamMgr.GetTeamMember(Pos);
		if(!teamMember)
			return;

		if( teamMember->UIPos ==  UIPos )
		{
			teamMember->HPPecent = Clamp<FLOAT>( teamMember->HPPecent , 0.01 , 1 );
			teamMember->MPPecent = Clamp<FLOAT>( teamMember->MPPecent , 0.01 , 1 );
			if( teamMgr.GetMemberLimit() >= LargeTeamNum )
			{
				UGFxObject* mc = LargeTeamMatePanel->GetObject( FString::Printf( TEXT("teamMate%d"), UIPos ) );
				if( mc && mc->GetObject(TEXT("fill_mc")))
				{
					mc->GetObject(TEXT("fill_mc"))->SetFloat( TEXT("width") , teamMember->HPPecent*LargeHpWidth  );
				}
			}
			else
			{
				UGFxObject* mc = NormalTeamMatePanel->GetObject( FString::Printf( TEXT("teamMate%d"), UIPos ) );
				if(mc)
				{
					if(mc->GetObject( TEXT("maskHp") ))
						mc->GetObject( TEXT("maskHp") )->SetFloat( TEXT("width") , teamMember->HPPecent*HPFullWidth );
					//mc->GetObject( TEXT("maskMp") )->SetFloat( TEXT("width") , teamMember->MPPecent*MPFullWidth );
					//if(mc->GetObject( TEXT("tf_level") ))
						//mc->GetObject( TEXT("tf_level") )->SetText( FString::Printf(TEXT("%d") , teamMember->Lv)  );
				}

				
			}				
		}
	}

}

void UMMOUITeamMate::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL)
		return;
	switch( evt->GetEventType() )
	{
	case GameEvent_TeamJoin:
	case GameEvent_TeamRemoveMember:
	case GameEvent_TeamMemberUpdate:
	case GameEvent_TeamLeave:	
		{
			INT myTeamMemberCount = TeamManager::getSingleton().GetTeamMemberCount();
			UpdateTeamMateNum( myTeamMemberCount );
			if( myTeamMemberCount != 0 )
			{
				UpdateAllTeamMate();
				//this->Show(TRUE);
			}
			
		}
		break;
	case GameEvent_OtherPlayerLevelUp:
		{
			MMOGameEvent_OtherPlayerLevelUp* levelup = (MMOGameEvent_OtherPlayerLevelUp*)(evt);

			if(levelup )
			{
				INT pos = TeamManager::getSingleton().GetMemberPosByMemberId( levelup->roleID );
				if( pos >= 0 )
				{
					MMOTeamMember* teamMember = TeamManager::getSingleton().GetTeamMember(pos);
					if(teamMember)
					{
						teamMember->Lv = levelup->lv;
						UpdateTeamMate( teamMember->UIPos , pos );
					}
				}
			}
		}
		break;
	case GameEvent_TeamExpand:
		{
			MMOGameEvent_TeamExpand* evts = (MMOGameEvent_TeamExpand*)(evt);

			IsShow = evts->show;

			TeamManager& teamMgr = TeamManager::getSingleton();

			if( teamMgr.GetTeamMemberCount() > NormalTeamNum ) //large group
			{
				if (LargeTeamMatePanel)
				{
					LargeTeamMatePanel->SetVisible(evts->show);
				}
			}
			else
			{
				if (NormalTeamMatePanel)
				{
					NormalTeamMatePanel->SetVisible(evts->show);
				}
			}
			
			
			
		}
		break;
	case GameEvent_TeamCommon:
		{
			MMOGameEvent_TeamCommon* cb = (MMOGameEvent_TeamCommon*)(evt);
			if(cb&&cb->serverID > 0)
			{
				AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
				if(playerCtrl == NULL)
					return;
				FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
				if( pMapInfo == NULL )
					return;
		
				INT mapType = pMapInfo->ServerMapType;

				if(cb->EventType == MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam)
				{
					AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
					if( !pl)
						return;
					if(confirmLeaveTeam)
					{
						confirmLeaveTeam->SetVisible(FALSE);
						confirmLeaveTeam = NULL;
					}

					FScriptDelegate funOk;
					funOk.Object = this;
					funOk.FunctionName = "ClickLeaveTeamConfirmOK";

					FScriptDelegate funCancel;
					funCancel.Object = this;
					funCancel.FunctionName = "ClickLeaveTeamConfirmCancel";

					FString strHint;
					if(pl->ServerID == cb->serverID)
					{
// 						if( mapType == E_est_GuildDungeon || mapType == E_est_Dungeon )
// 							strHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeamOperation.LeaveSelfTeamDungeon") );
// 						else
							strHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeamOperation.LeaveSelfTeam") );
					}
					else
					{
						MMOTeamMember* pkMate = TeamManager::getSingleton().GetMemberById(cb->serverID);
						if(pkMate)
						{
// 							FExportMap *pMatMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( pkMate->MapID );
// 							if( pMatMapInfo == NULL )
// 								return;
// 							if(pMatMapInfo->ServerMapType == E_est_GuildDungeon || pMatMapInfo->ServerMapType == E_est_Dungeon )
// 								strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeamOperation.KickOutTeamMateDungeon") ),*(pkMate->ActorName));
// 							else
								strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeamOperation.KickOutTeamMate") ),*(pkMate->ActorName));
						}
					}
					confirmLeaveTeam = MMOMsgBoxManager::getSingleton().AddModalMessageBox(strHint, 0, funOk, funCancel);
					if(confirmLeaveTeam)
						confirmLeaveTeam->SetInt(TEXT("serverID"),cb->serverID);

				}
				else if(cb->EventType == MMOGameEvent_TeamCommon::ETeamEventType_MoveLeader)
				{
					if(confirmMoveLeader)
					{
						confirmMoveLeader->SetVisible(FALSE);
						confirmMoveLeader = NULL;
					}

					FScriptDelegate funOk;
					funOk.Object = this;
					funOk.FunctionName = "ClickMoveLeaderConfirmOK";

					FScriptDelegate funCancel;
					funCancel.Object = this;
					funCancel.FunctionName = "ClickMoveLeaderConfirmCancel";

					MMOTeamMember* pkMate = TeamManager::getSingleton().GetMemberById(cb->serverID);
					if(pkMate)
					{
						FString strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TeamOperation.MoveLeaderHint") ),*(pkMate->ActorName));
						confirmMoveLeader = MMOMsgBoxManager::getSingleton().AddModalMessageBox(strHint, 0, funOk, funCancel);
						if(confirmMoveLeader)
							confirmMoveLeader->SetInt(TEXT("serverID"),cb->serverID);
					}
				}
			}
			break;
		}
	}

}


void UMMOUITeamMate::ClickMoveLeaderConfirmOK()
{
	if(confirmMoveLeader)
	{
		INT nServer = confirmMoveLeader->GetInt(TEXT("serverID"));
		TeamManager::getSingleton().MoveLeader( nServer );
	}
	confirmMoveLeader = NULL;
}
void UMMOUITeamMate::ClickMoveLeaderConfirmCancel()
{
	confirmMoveLeader = NULL;
}
void UMMOUITeamMate::ClickLeaveTeamConfirmOK()
{
	if(confirmLeaveTeam)
	{
		INT nServer = confirmLeaveTeam->GetInt(TEXT("serverID"));
		TeamManager::getSingleton().LeaveTeam( nServer );
	}
	confirmLeaveTeam = NULL;
}
void UMMOUITeamMate::ClickLeaveTeamConfirmCancel()
{
	confirmLeaveTeam = NULL;
}
//------------------------------------------------------------------------
void UMMOUITeamMate::UpdateAllTeamMate(  )
{
	if( !Player)
	{
		return;
	}
	TeamManager& teamMgr = TeamManager::getSingleton();

	if(  teamMgr.HasTeam() )
	{
		if( teamMgr.GetTeamMemberCount() > NormalTeamNum ) //large group
		{
			//Hide small group, show large group
			if( NormalTeamMatePanel )
			{
				NormalTeamMatePanel->SetVisible( FALSE );
			}
			if( !LargeTeamMatePanel )
			{
				return;
			}
			LargeTeamMatePanel->SetVisible(IsShow);

		}
		else //small group
		{
			if( !NormalTeamMatePanel )
			{
				return;
			}
			if( LargeTeamMatePanel )
			{
				LargeTeamMatePanel->SetVisible( FALSE );
			}
			NormalTeamMatePanel->SetVisible( IsShow );
			
		}	
		UpdateTeamMate();
	}


}

BOOL UpdateTeamMateData(MMOTeamMember* teamMember)
{
	if(!teamMember)
		return FALSE;

	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if(!pl)
		return FALSE;

	if(pl->ServerID == teamMember->ActorServerId)
		return FALSE;

	return TRUE;
}

void UMMOUITeamMate::UpdateTeamMate()
{
	UGFxObject* datList = GFXPlayer->CreateArray();
	if(datList == NULL)
		return ;

	TeamManager& teamMgr = TeamManager::getSingleton();

	INT idx = 0;

	//Team leader
	MMOTeamMember* leader = teamMgr.GetTeamLeader();
	if(UpdateTeamMateData(leader))
	{
		datList->SetElementObject(idx, teamMgr.CreateTeamMate(leader,GFXPlayer));
		leader->UIPos = idx;
		idx++;

	}


	for (int i=0 ; i< teamMgr.GetTeamMemberCount() ; i++ )
	{
		MMOTeamMember* teamMember = teamMgr.GetTeamMember(i);

		if(UpdateTeamMateData(teamMember) == FALSE)
			continue;

		if (teamMember->isLeader)
			continue;

		datList->SetElementObject(idx, teamMgr.CreateTeamMate(teamMember,GFXPlayer));

		teamMember->UIPos = idx;
		idx++;
	}

	eventUCUpdateTeamMenber(datList);
}


//--------------------------------------------------------------------
void UMMOUITeamMate::ClearUI()
{
	UpdateTeamMateNum(0);
}

//--------------------------------------------------------------------

void UMMOUITeamMate::UpdateTeamMateNum(INT TeamMateNum)
{

	TeamManager& teamMgr = TeamManager::getSingleton();
	if( teamMgr.HasTeam() /*&& teamMgr.GetTeamMemberCount() <= NormalTeamNum*/ )
	{
		TeamMateNum = TeamMateNum - 1;
	}
	
	if( LargeTeamMatePanel )
	{
		for(INT i = 0 ; i < LargeLineNum ; i++)
		{
			UGFxObject* mc = LargeTeamMatePanel->GetObject( FString::Printf(TEXT( "teamMate%d" ) , i ));
			if( !mc )
			{
				continue;
			}
			if( i < TeamMateNum ) 
			{
				mc->SetVisible(true);				
			}
			else
			{
				mc->SetVisible(false);			
			}
		}
	}

	if( NormalTeamMatePanel )
	{
		for(INT i = 0 ; i < LineNum ; i++)
		{
			UGFxObject* mc = NormalTeamMatePanel->GetObject( FString::Printf(TEXT( "teamMate%d" ) , i ));
			if( !mc )
			{
				continue;
			}
			if( i < TeamMateNum ) 
			{
				mc->SetVisible(true);			
			}
			else
			{
				mc->SetVisible(false);				
			}
		}
	}

}

void UMMOUITeamMate::Init()
{
	Super::Init();

	NormalTeamMatePanel = GetObject( TEXT("TeamMate4") );
	LargeTeamMatePanel = GetObject( TEXT("TeamMate20") );

	if( NormalTeamMatePanel )
	{
		UGFxObject* mate = NULL;
		for( INT i = 0 ; i < LineNum ; i++ )
		{
			FString ctrlName = FString( TEXT("teamMate") ) + appItoa(i);
			mate = NormalTeamMatePanel->GetObject(ctrlName);
			if(mate)
			{
				mate->SetVisible(false);	
			}
		}		
		if(mate)
		{
			UGFxObject* maskHp = mate->GetObject("maskHp");
			if(maskHp)
			{
				HPFullWidth = maskHp->GetInt("width");
			}
		}		
	}
	
	if(LargeTeamMatePanel)
	{
		UGFxObject* mate = NULL;
		for( INT i = 0 ; i < LargeLineNum ; i++)
		{
			FString ctrlName = FString( TEXT("teamMate") ) + appItoa(i);
			mate = LargeTeamMatePanel->GetObject(ctrlName);
			if(mate)
			{
				mate->SetVisible(false);
			}				
		}

		if(mate)
		{
			UGFxObject* maskHp = mate->GetObject("maskMp");
			if(maskHp)
			{
				MPFullWidth = maskHp->GetInt("width");
			}
		}
	}


	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_OtherPlayerLevelUp);
	evts.AddItem(GameEvent_TeamMemberUpdate);
	evts.AddItem(GameEvent_TeamJoin);
	evts.AddItem(GameEvent_TeamRemoveMember);
	evts.AddItem(GameEvent_TeamLeave);	
	evts.AddItem(GameEvent_TeamExpand);
	evts.AddItem(GameEvent_TeamCommon);
	EventAdapter = new TGameEventAdapter<UMMOUITeamMate>(this, evts);
}

void UMMOUITeamMate::ASTeamMemberRightClick( INT x , INT y , INT index )
{	
	MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_TeamMate, x , y , 0 , "" , index );
}

void UMMOUITeamMate::ASTeamMemberLeftClick( INT x , INT y , INT index )
{
	TeamManager::getSingleton().SelectedNearTeamMate( index );
}

//========================================================================================================
void UMMOUITeamMate::ASShowTeammateHint( UGFxObject *mc, INT index )
{
	if( mc == NULL ){
		return;
	}

	//
	INT teammateIdx = TeamManager::getSingleton().GetMemberPosByUIPos( index );
	MMOTeamMember* teammate = TeamManager::getSingleton().GetTeamMember( teammateIdx );
	if( teammate == NULL ){
		return;
	}

	FSMIO::getSingleton().Target.SetUITargetActor(teammate->ActorServerId);

	//
	FString hintString;
	//class
	hintString = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf( TEXT("%d"), teammate->ClassId ) );
	hintString += TEXT("<br>");

	//location
	if( teammate->Online ){
		FExportMap teammateMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( teammate->MapID );
		INT idx =  MMOMapDataManager::getSingleton().CheckRegionIn( teammate->MapID, teammate->WorldPos );
		FInMapRegion *pCurReginInCurMap = MMOMapDataManager::getSingleton().GetReginInMap(teammate->MapID, idx );
		if( pCurReginInCurMap == NULL ){
			hintString += teammateMapInfo.MapDisplayName;
		}
		else{

			hintString += pCurReginInCurMap->DisplayName;
		}
		if( teammateMapInfo.ServerMapType == E_est_Line ){
			FString stringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.SceneLineListString") );
			UINT sceneLineID = 0;
			UINT sceneServerID = 0;
			MMOMapDataManager::ParseServerMapLine( teammate->MapLineID, sceneServerID, sceneLineID );
			hintString += FString::Printf( *stringFormat, sceneLineID );
		}
	}
	else{
		hintString += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Offline") );
	}
	mc->SetString( TEXT("tipsHtml"), hintString );

	//
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//========================================================================================================
void UMMOUITeamMate::ASHideTeammateHint()
{
	FSMIO::getSingleton().Target.SetUITargetActor(0);
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUITeamMate::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);
}

