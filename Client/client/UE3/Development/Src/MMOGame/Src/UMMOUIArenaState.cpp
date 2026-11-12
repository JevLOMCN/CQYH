#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOArenaManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIArenaState)

//=======================================================================================
void UMMOUIArenaState::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if( v ){
		UMMOUIBase* rightPanel = MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
		if( rightPanel != NULL ){
			//INT leftW = eventGetWidth();
			//INT leftH = leftPanel->eventGetHeight();

			FLOAT rightPanelX;
			FLOAT rightPanelY;
			rightPanel->GetPosition( rightPanelX, rightPanelY );

			SetPosition( rightPanelX - PanelWidth, rightPanelY + 45 );
		}
	}
	else{

	}
}
//
//====================================================================================================
void UMMOUIArenaState::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if( evt == NULL ){
		return;
	}

	switch( evt->GetEventType() )
	{
	case GameEvent_ArenaStateChanged:
		{
			if( !IsVisible ){
				break;
			}

			//
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;

			EArenaState curArenaSta = MMOArenaManager::getSingleton().GetCurArenaState();
			switch( curArenaSta )
			{
			case EAnaSta_InGame:
			case EAnaSta_WaitNextRound:
			case EAnaSta_WaitStartRound:
			case EAnaSta_InRound:
				{
					UpdatePanel();
				}
				break;
			default:
				{
					SetVisible( FALSE );
					//MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Arena_ArenaState, FALSE, "");
					return;
				}
				break;
			}
		}
		break;
	case GameEvent_ArenaDateUpdated:
		{
			if( !IsVisible ){
				break;
			}

			UpdateMyScore();
		}
		break;
	}
}

//
void UMMOUIArenaState::UpdatePanel()
{
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	UINT32 leftSec = arenaMgr.GetArenaLeftSecond();
	eventCallASSetArenaTimeCntDown( leftSec );

	//Battle record
	UpdateMyScore();

	//
	EArenaState curArenaSta = arenaMgr.GetCurArenaState();
	switch( curArenaSta )
	{
	case EAnaSta_WaitStartRound:
		{
			//Hide operation button settings
			eventCallASShowOperateBtn( TEXT(""), FALSE );
			//
			eventCallASSetArenaRoundTime( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.StatePanelWaitStart") ), -1 );
		}
		break;
	case EAnaSta_InRound:
		{
			//Hide operation button settings
			eventCallASShowOperateBtn( TEXT(""), FALSE );
			//Remaining time this round
			UpdateRoundTime();
		}
		break;
	case EAnaSta_InGame:
		{
			//Operation button settings
			eventCallASShowOperateBtn( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.BeginNextBtnText") ), TRUE );
			//Set UI hint text
			eventCallASSetPanelHintTxt( TEXT(""), -1 );
		}
		break;
	case EAnaSta_WaitNextRound:
		{
			//Hide operation button settings
			eventCallASShowOperateBtn( TEXT(""), FALSE );
			//Set UI hint text
			eventCallASSetPanelHintTxt( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.StatePanelWaitingNextRoundHint") ), -1 );
		}
		break;
	}

}

//
void UMMOUIArenaState::UpdateMyScore()
{
	const PlayerArenaData &myArenaData = MMOArenaManager::getSingleton().GetMyCurArenaData();
	FString scoreText;
	if( myArenaData._CurHighestScore > 0 ){
		FString chnNumStr = UITextParser::CvtNumToChnNum( myArenaData._CurHighestScore );
		if( myArenaData._CurHighestScore > 1 ){
			chnNumStr += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.DubbleWin") );
		}
		else{
			chnNumStr += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Win") );
		}
		scoreText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.StatePanelWinScoreText") ), *chnNumStr );
	}
	else{
		scoreText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.StatePanelWinScoreText") ), *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.NoWin") ) );
	}
	eventCallASSetArenaWin( scoreText );
}

//
void UMMOUIArenaState::UpdateRoundTime()
{
	UINT32 leftSec = MMOArenaManager::getSingleton().GetArenaCurRoundLeftSecond();
	eventCallASSetArenaRoundTime( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.StatePanelRoundTimeTitle") ), leftSec );
}
//=======================================================================================
void UMMOUIArenaState::TryNextRound()
{
	CMessageHandler::GetInstance().Send_MatchArenaRound();
}

//=======================================================================================
void UMMOUIArenaState::Init()
{
	Super::Init();

	//
	InitMc();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem( GameEvent_ArenaStateChanged );
	evts.AddItem( GameEvent_ArenaDateUpdated );
	EventAdapter = new TGameEventAdapter<UMMOUIArenaState>(this, evts);
}
//=======================================================================================
void UMMOUIArenaState::InitMc()
{

}

//========================================================================================
void UMMOUIArenaState::ASOnFightingButtonClick()
{
	TryNextRound();
}

//========================================================================================
//========================================================================================
UBOOL UMMOUIArenaState::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( strHoverControllerName == TEXT("panelMask") )
	{
		return TRUE;
	}

	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}
//========================================================================================
//========================================================================================