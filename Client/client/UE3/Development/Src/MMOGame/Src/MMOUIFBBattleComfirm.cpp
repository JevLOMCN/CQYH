#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuideManager.h"
#include "MMOMapDataManager.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOUIFBBattleComfirm)

void UMMOUIFBBattleComfirm::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( show ){
		FVector2D scrSz = MMOFacade::GetScreenSize();

		SetPosition( scrSz.X, scrSz.Y );

		UpdateUIMode();
	}
	else{
		if( ConfirmMsgBox != NULL ){
			ConfirmMsgBox->SetVisible( FALSE );
			ConfirmMsgBox = NULL;
		}
	}
}

//
void UMMOUIFBBattleComfirm::ResetUI()
{
	//btnMcPlayAgain->SetString( TEXT("label"), *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral7") ) );
	//
	if( BackFBSummaryBtn != NULL ){
		LeaveBtn->SetString( TEXT("label"), *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral8") ) );
	}
	//
	if( BackFBSummaryBtn != NULL ){
		BackFBSummaryBtn->SetString( TEXT("label"), *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral9") ) );
	}
	
	//BackFBAwardBtn->SetString( TEXT("label"), *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral10") ) );

	//
	if( CloseFBuiBtn != NULL ){
		CloseFBuiBtn->SetString( TEXT("label"), *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral11") ) );
	}
}

//
void UMMOUIFBBattleComfirm::ConfirmLeaveFromTeleport( INT teleportID )
{
	if( ConfirmMsgBox != NULL ){
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT( "AgreeLeaveFromTeleport" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "DiagreeFromTeleport" );
	//popup message

	FString msgInfo;
	msgInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral6") );

	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgInfo, teleportID, funYes, funNo );
}

//
void UMMOUIFBBattleComfirm::AgreeLeaveFromTeleport( UGFxObject *pMc )
{
	if( pMc == ConfirmMsgBox && ConfirmMsgBox != NULL ){
		int svrLinkID = ConfirmMsgBox->GetInt( TEXT("iteminfoid") );
		CMessageHandler::GetInstance().Send_Teleport( svrLinkID );
	}
	//
	ConfirmMsgBox = NULL;
}

//
void UMMOUIFBBattleComfirm::DiagreeFromTeleport( UGFxObject *pMc )
{
	ConfirmMsgBox = NULL;
}

void UMMOUIFBBattleComfirm::UpdateUIMode()
{
	
	//if( TeamManager::getSingleton().HasTeam() )
	//{
	//	if( TeamManager::getSingleton().IsTeamLeader() )
	//	{
	//		UIMode = EUI_FBBattleComf_Captain;
	//	}
	//	else{
	//		UIMode = EUI_FBBattleComf_Normal;
	//	}
	//}
	//else{//single mode
	//	UIMode = EUI_FBBattleComf_Captain;
	//}
	////Update ui apparence
	//if( btnMcPlayAgain == NULL )
	//{
	//	return;
	//}
	//switch( UIMode )
	//{
	//case EUI_FBBattleComf_Captain:
	//	btnMcPlayAgain->SetVisible( true );
	//	break;
	//	//
	//case EUI_FBBattleComf_Normal: 
	//case EUI_FBBattleComf_None:
	//	btnMcPlayAgain->SetVisible( false );
	//	break;
	//}
}

//========================================================================================
void UMMOUIFBBattleComfirm::Init()
{
	Super::Init();

	//
	InitFBBattleComfirmMc();
}

//========================================================================================
void UMMOUIFBBattleComfirm::InitFBBattleComfirmMc()
{
	LeaveBtn = GetObject( TEXT("bt_leave") );
	//
	BackFBSummaryBtn = GetObject( TEXT("bt_backFBSummary") );
	if( BackFBSummaryBtn != NULL ){
		BackFBSummaryBtn->SetVisible( FALSE );
	}
	//BackFBAwardBtn = GetObject("bt_backFBAward");
	//
	CloseFBuiBtn = GetObject( TEXT("bt_closeFBui") );
	if( CloseFBuiBtn != NULL ){
		CloseFBuiBtn->SetVisible( FALSE );
	}

	UpdateUIMode();
}

//========================================================================================
//Open the UI during the countdown
void UMMOUIFBBattleComfirm::OpenInTime( int iMSec )
{
	eventCallASStartCloseCountDown( iMSec );
	ResetUI();

	Show( TRUE );

	MMOGameEvent_Guide evtGuide(EGUET_ExitDungeonConfirm);
	MMOUI::PostGameEvent(&evtGuide);
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIFBBattleComfirm::ReadyToFBEndAll()
{
	UMMOUIFBSummary *pFBSummaryPanel = (UMMOUIFBSummary*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary );
	if( pFBSummaryPanel != NULL ){
		pFBSummaryPanel->PlayAnimInTime( UCONST_FBSummaryMSec );
	}

	UMMOUIFBAward *pFBAwardPanel = (UMMOUIFBAward*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBAward );
	if( pFBAwardPanel != NULL ){
		pFBAwardPanel->bPlayed = false;
	}
}

//========================================================================================================
//see the summary, after the time specified the UI will close
void UMMOUIFBBattleComfirm::ReviewFBSummary()
{
	UMMOUIFBSummary *pFBSummaryPanel = (UMMOUIFBSummary*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary );
	if( pFBSummaryPanel != NULL ){
		pFBSummaryPanel->OpenInTime( UCONST_FBSummaryMSec );
	}
}

//========================================================================================================
//see the summary displayed last time, no time limited
void UMMOUIFBBattleComfirm::OpenFBSummary()
{
	UMMOUIFBSummary *pFBSummaryPanel = (UMMOUIFBSummary*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary );
	if( pFBSummaryPanel != NULL ){
		pFBSummaryPanel->Show( TRUE );
	}
}

//========================================================================================================
void UMMOUIFBBattleComfirm::HideFBSummary()
{
	UMMOUIFBSummary *pFBSummaryPanel = (UMMOUIFBSummary*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary );
	if( pFBSummaryPanel != NULL ){
		pFBSummaryPanel->Show( FALSE );
	}
}
//========================================================================================================
void UMMOUIFBBattleComfirm::SendPlayFBAgain()
{
	CMessageHandler::GetInstance().Send_PlayFBAgain();
	//need to resend again....
	INT curFBDiff = MMOMapDataManager::getSingleton().GetCurFBDiff();
	CMessageHandler::GetInstance().Send_SelMapDifficulty( curFBDiff );
	CMessageHandler::GetInstance().Send_EnterDungeon( curFBDiff );
}
//------------------------------------------------------------------------
void UMMOUIFBBattleComfirm::SendLeaveFB()
{
	CMessageHandler::GetInstance().Send_LeaveScene();
}
//========================================================================================================
void UMMOUIFBBattleComfirm::WaitFBtoSummary()
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
	//
	ShowCountDownBar();
}
//========================================================================================================
void UMMOUIFBBattleComfirm::ShowCountDownBar()
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
		progressBar->eventOpen( UCONST_FBEndPreMSec , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral0") ) , 0, fun );

	}
}
//========================================================================================================
void UMMOUIFBBattleComfirm::OpenFBBattleComfirm()
{
	INT leftTime = UCONST_FBEndTotalMSec - UCONST_FBEndPreMSec;
	OpenInTime( leftTime );
}
//========================================================================================================
void UMMOUIFBBattleComfirm::WantPlayFbAgain()
{
	SendPlayFBAgain();

	//close all FB UI
	CloseAllFBEndUI();
}

//========================================================================================================
void UMMOUIFBBattleComfirm::LeaveFB()
{
	MMOGameEvent_Guide evtGuide(EGUET_ExitDungeon);
	MMOUI::PostGameEvent(&evtGuide);

	SendLeaveFB();

	//close all FB UI
	CloseAllFBEndUI();

}

////========================================================================================================
////ship the FBSummay UI to show the FBAward, but not to play
void UMMOUIFBBattleComfirm::OpenFBAward()
{
	UMMOUIFBAward *pFBAwardPanel = (UMMOUIFBAward*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBAward );
	if( pFBAwardPanel != NULL ){
		pFBAwardPanel->Show( TRUE );
	}
}
//------------------------------------------------------------------------------------------------------------
void UMMOUIFBBattleComfirm::HideFBAward()
{
	UMMOUIFBAward *pFBAwardPanel = (UMMOUIFBAward*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBAward );
	if( pFBAwardPanel != NULL && pFBAwardPanel->IsVisible ){
		pFBAwardPanel->Show( FALSE );
	}
}

//========================================================================================================
//Close dungeon rating, dungeon lottery, dungeon exit reset windows
void UMMOUIFBBattleComfirm::CloseAllFBEndUI()
{
	UMMOUIFBAward *pFBAwardPanel = (UMMOUIFBAward*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBAward );
	if( pFBAwardPanel != NULL && pFBAwardPanel->IsVisible ){
		pFBAwardPanel->ResetUI();
		pFBAwardPanel->Show( FALSE );
	}

	UMMOUIFBBattleComfirm *pFBBattleConfirm = (UMMOUIFBBattleComfirm*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm );
	if( pFBBattleConfirm != NULL && pFBBattleConfirm->IsVisible ){
		pFBBattleConfirm->eventCallASStopAndHideFB();
		pFBBattleConfirm->Show( FALSE );
	}

	MMOUI::GetUIManager()->ShowPanel(MMOUI::GetUIManager()->Panel_FBEndAll_FBSummary, FALSE, "");

	UMMOUIProgressBarWidget* progressBar = Cast<UMMOUIProgressBarWidget>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_DynamicUI_FBSummaryProgressBar ) );
	if( progressBar != NULL ){
		progressBar->eventClose( TRUE );
		progressBar->Show( FALSE );
	}

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

//===============================================================================================================
//===============================================================================================================