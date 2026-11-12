#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "MMOMapDataManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_FBEndAll)

//------------------------------------------------------------------------
void UMMOGFXPlayer_FBEndAll::SendPlayFBAgain()
{
	CMessageHandler::GetInstance().Send_PlayFBAgain();
	//need to resend again....
	INT curFBDiff = MMOMapDataManager::getSingleton().GetCurFBDiff();
	CMessageHandler::GetInstance().Send_SelMapDifficulty( curFBDiff );
	CMessageHandler::GetInstance().Send_EnterDungeon( curFBDiff );
}
//------------------------------------------------------------------------
void UMMOGFXPlayer_FBEndAll::SendLeaveFB()
{
	CMessageHandler::GetInstance().Send_LeaveScene();
}
//
void UMMOGFXPlayer_FBEndAll::WaitFBtoSummary()
{
	AMMOPlayer *pMe = MMOFacade::getSingleton().GetMainPlayer();
	if( pMe == NULL ){
		return;
	}

	//FString strHint;
	//strHint = MMOUI::ParseLocalizedPropertyPath("MMOGame.FBGeneral.FBGeneral1");
	//FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	//strHint = strHint.Replace( TEXT( "MapName" ), *mapInfo.MapDisplayName );

	//INT curFBDiff = MMOMapDataManager::getSingleton().GetCurFBDiff();
	//INT iDiffStrIdx = ( curFBDiff - ENUM_SCENE_DIFFICULTY::esdNormal1 );//pFBEnter->CvtToUiDiffIdx( pFBEnter->CurDiff );
	//FString strIdx = FString::Printf( TEXT("MMOGame.FBDiff.FBDiff%d"), iDiffStrIdx );
	//strHint = strHint.Replace( TEXT( "FBDiff" ), *MMOUI::ParseLocalizedPropertyPath( strIdx ) );
	////chat box output prompt
	//MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *strHint, *pMe->RoleName ) );
	//
	ShowCountDownBar();
}

void UMMOGFXPlayer_FBEndAll::ShowCountDownBar()
{
	UMMOUIProgressBarWidget* progressBar;
	progressBar = Cast<UMMOUIProgressBarWidget>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_FBSummaryProgressBar ) );
	if( progressBar )
	{
		FScriptDelegate fun;
		//fun.FunctionName = "ReadyToFBEndAll";
		fun.FunctionName = "OpenFBBattleComfirm";
		fun.Object = this;
		progressBar->Show(TRUE);
		progressBar->eventOpen( FBEndPreMSec , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral0") ) , 0, fun );

	}
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::ReadyToFBEndAll()
{
	if( _FBSummary == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::ReadyToFBEndAll _FBSummary is NULL]") );
		return;
	}
	_FBSummary->PlayAnimInTime( FBSummaryMSec );

	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::ReadyToFBEndAll _FBAward is NULL]") );
		return;
	}
	_FBAward->bPlayed = false;
}

//========================================================================================================
//see the summary, after the time specified the UI will close
void UMMOGFXPlayer_FBEndAll::ReviewFBSummary()
{
	if( _FBSummary == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::ReviewFBSummary _FBSummary is NULL]") );
		return;
	}
	_FBSummary->OpenInTime( FBSummaryMSec );
}

//========================================================================================================
//see the summary displayed last time, no time limited
void UMMOGFXPlayer_FBEndAll::OpenFBSummary()
{
	if( _FBSummary == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::OpenFBSummary _FBSummary is NULL]") );
		return;
	}
	_FBSummary->Show( TRUE );
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::HideFBSummary()
{
	if( _FBSummary == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::HideFBSummary _FBSummary is NULL]") );
		return;
	}
	_FBSummary->Show( FALSE );
}

//========================================================================================================
//called by AS to display the FBAward and begin to play
void UMMOGFXPlayer_FBEndAll::NextToPlayFBAward()
{
	if( _FBSummary == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::NextToPlayFBAward _FBSummary is NULL]") );
		return;
	}
	_FBSummary->TimeUpToClose();

	//start lottery
	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::NextToPlayFBAward _FBAward is NULL]") );
		return;
	}
	_FBAward->OpenToPlay( FBAwardMSec );//5 seconds

	//show the hide button
	if( _FBBattleConfirm == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::NextToPlayFBAward _FBBattleConfirm is NULL]") );
		return;
	}
	_FBBattleConfirm->CloseFBuiBtn->SetVisible( true );
}

//========================================================================================================
//ship the FBSummay UI to show the FBAward, but not to play
void UMMOGFXPlayer_FBEndAll::OpenFBAward()
{
	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::ClickGetFBAward _FBAward is NULL]") );
		return;
	}
	_FBAward->Show( TRUE );
}
void UMMOGFXPlayer_FBEndAll::HideFBAward()
{
	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::ClickGetFBAward _FBAward is NULL]") );
		return;
	}
	_FBAward->Show( FALSE );
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::ClickGetFBAward( int iIdx )
{
	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::ClickGetFBAward _FBAward is NULL]") );
		return;
	}
	
	_FBAward->ClickAward( iIdx );
}

//========================================================================================================
//Back to FBSummary from FBAward
void UMMOGFXPlayer_FBEndAll::FBAwardPlayTimeOver()
{
	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::FBAwardPlayTimeOver _FBAward is NULL]") );
		return;
	}
	_FBAward->TimeUpToShowAll();

	//open next window
	OpenFBBattleComfirm();
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::OpenFBBattleComfirm()
{
	if( _FBBattleConfirm == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::OpenFBBattleComfirm _FBBattleConfirm is NULL]") );
		return;
	}
	//INT leftTime = FBEndTotalMSec - ( FBEndPreMSec + FBSummaryMSec + FBAwardMSec);
	INT leftTime = FBEndTotalMSec - FBEndPreMSec;
	_FBBattleConfirm->OpenInTime( leftTime );
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::GetFBAward( int iIdx)
{
	if( _FBAward == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_FBEndAll::GetFBAward _FBAward is NULL]") );
		return;
	}
	_FBAward->ClickAward( iIdx );
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::WantPlayFbAgain()
{
	SendPlayFBAgain();

	//close all FB UI
	CloseAllFBEndUI();
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::LeaveFB()
{
	MMOGameEvent_Guide evtGuide(EGUET_ExitDungeon);
	MMOUI::PostGameEvent(&evtGuide);

	SendLeaveFB();

	//close all FB UI
	CloseAllFBEndUI();

}

//========================================================================================================
//Close dungeon rating, dungeon lottery, dungeon exit reset windows
void UMMOGFXPlayer_FBEndAll::CloseAllFBEndUI()
{
	
	if( _FBAward != NULL && _FBAward->IsVisible ){
		_FBAward->ResetUI();
	}
	if( _FBBattleConfirm != NULL && _FBBattleConfirm->IsVisible ){
		_FBBattleConfirm->eventCallASStopAndHideFB();
	}

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEndAll_FBAward, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary, FALSE, "");

	UMMOUIProgressBarWidget* progressBar;
	progressBar = Cast<UMMOUIProgressBarWidget>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_FBSummaryProgressBar ) );
	if( progressBar )
	{
		progressBar->eventClose( TRUE );
	}
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_FBSummaryProgressBar, FALSE, "");


	//if( _FBSummary != NULL && _FBSummary->IsVisible ){
	//	_FBSummary->Show( FALSE );
	//}

	////
	//if( _FBAward != NULL && _FBAward->IsVisible ){
	//	_FBAward->Show( FALSE );
	//	//clear all FB award data
	//	_FBAward->ResetUI();
	//}
	//if( _FBBattleConfirm != NULL && _FBBattleConfirm->IsVisible ){
	//	_FBBattleConfirm->eventCallASStopAndHideFB();
	//}
	////Data
	////ResetFBSummary();
	////ResetFBAward();
}

//========================================================================================================
void UMMOGFXPlayer_FBEndAll::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_FBEndAll::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_FBEndAll);
}

void UMMOGFXPlayer_FBEndAll::UnInit()
{
	Super::UnInit();
}