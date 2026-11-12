#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "TeamManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUITeamHint)


void UMMOUITeamHint::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_OtherPlayerLevelUp);
	evts.AddItem(GameEvent_TeamMemberUpdate);
	evts.AddItem(GameEvent_TeamJoin);
	evts.AddItem(GameEvent_TeamRemoveMember);
	evts.AddItem(GameEvent_TeamLeave);
	evts.AddItem(GameEvent_TeamLeaveAsk);
	evts.AddItem(GameEvent_EnterSelectingRole);
	evts.AddItem(GameEvent_TeamExpand);
	EventAdapter = new TGameEventAdapter<UMMOUITeamHint>(this, evts);

	eventCallHideWidget(TRUE);
}

void UMMOUITeamHint::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL)
		return;
	switch( evt->GetEventType() )
	{
	case GameEvent_TeamJoin:
		{
			eventCallHideWidget(FALSE);
			UpdateHint();
		}
		break;
	case GameEvent_TeamRemoveMember:
	case GameEvent_TeamMemberUpdate:
		{
			UpdateHint();
		}
		break;
	case GameEvent_EnterSelectingRole:
	case GameEvent_TeamLeave:	
		{
			eventCallHideWidget(TRUE);
		}
		break;
	case GameEvent_TeamLeaveAsk:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if( cb->Agree )
				{
					AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
					if(player)
					{
						TeamManager::getSingleton().LeaveTeam( player->ServerID );
					}
					
				}

			}
		}
		break;
	case GameEvent_TeamExpand:
		{
			MMOGameEvent_TeamExpand* evts = (MMOGameEvent_TeamExpand*)(evt);
			eventCallShowByExpand(evts->show);
		}
		break;
	}

}

void UMMOUITeamHint::UpdateHint()
{
	if( GFXPlayer )
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );		
		if( data )
		{
			if( TeamManager::getSingleton().HasTeam() )
			{
				switch(TeamManager::getSingleton().GetAssignType())
				{
				case etaFreeForAll:
					data->SetString( TEXT("teamAssignText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.FreeAll") ) );
					break;
				case etaTeamAuction:

					{
						INT curColor = TeamManager::getSingleton().GetAuctionItemColorLimit();
						INT curIdx = TeamManager::getSingleton().GetTeamAuctionItemColorIndex( curColor );
						TeamAuctionItemColorLimitList& list = TeamManager::getSingleton().GetLimitColorMap();
						FString str = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.TeamAuction") );
					
					
						data->SetString( TEXT("teamAssignText") , str );
					}
					break;
				case etaLeaderPick:
					{
						data->SetString( TEXT("teamAssignText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.LeaderPick") ) );
					}
					break;
				}
				
				FString memberNum = FString::Printf( TEXT("%d/%d") , TeamManager::getSingleton().GetTeamMemberCount() , TeamManager::getSingleton().GetMemberLimit() );
				data->SetString( TEXT("teamMemberNumText") , memberNum );
				FString teamSize = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TeamSize")) , TeamManager::getSingleton().GetMemberLimit() );
				data->SetString( TEXT("teamSizeText") , teamSize );

			}
			else
			{

			}

			eventCallSetData(data);
		}
	}
}

void UMMOUITeamHint::ASTeamHintLeave()
{
	//MMOMsgBoxManager::getSingleton().AddModalMessageBoxEvent( TEXT("Leave party?") , 0 , GameEvent_TeamLeaveAsk );
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam,player->ServerID);
		MMOUI::PostGameEvent(&et);
	}
}

void UMMOUITeamHint::ASTeamExpand(UBOOL v)
{
	MMOGameEvent_TeamExpand msg(v);
	MMOUI::PostGameEvent( &msg );
}

UBOOL UMMOUITeamHint::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(	strHoverControllerName=="bg")
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

