#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIFBSummary)

#define ValidSetChildInfoText( P , S , C ) if( P->GetObject(TEXT(S)) ) { P->GetObject(TEXT(S))->SetText(C); }

void UMMOUIFBSummary::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		MoveCenterToScreenCenter( 0, 0 );

		UpdateData();
	}
	else{
	}
}

void UMMOUIFBSummary::UpdateSummaryPoints()
{
	//ClearanceTimePt_mc
	INT h, m, s;
	FBSummaryData *SumData = MMOFacade::getSingleton().GetFBSummary();
	if( SumData == NULL ){
		return;
	}
	//test
	/*
	SumData->m_uiClearanceTime = 123;
	SumData->m_uiClearanceTimePt = 3;
	SumData->m_uiKillSkillPt = 4;
	SumData->m_uiKilled = 100;
	SumData->m_uiDestroyed = 121;
	SumData->m_uiStrategy = 122;
	SumData->m_uiAccomplishPt = 1;
	SumData->m_uiGradedPt = 5;
	SumData->m_uiGradedExp = 200;
	SumData->m_uiVIPExp = 201;
	SumData->m_uiGuildExp = 202;
	SumData->m_uiActivityExp = 302;
	SumData->m_uiFirstTimeExp = 301;
	SumData->m_uiCouplesExp = 0;
	*/
	//Clear time
	//ClearanceTimePt_mc
	eventCallASSetRank( E_FB_TIME, SumData->m_uiClearanceTimePt );
	h = SumData->m_uiClearanceTime / 3600;
	m = SumData->m_uiClearanceTime % 3600 / 60;
	s = SumData->m_uiClearanceTime % 60;
	ValidSetChildInfoText( ClearanceTime_mc, "tF_current" , FString::Printf( TEXT("%d:%02d:%02d"), h, m, s ) );
	//Kill technique
	//KillSkillPt_mc
	eventCallASSetRank( E_FB_SKILL, SumData->m_uiKillSkillPt + 1 );
	ValidSetChildInfoText( Killed_mc, "tF_current" , FString::Printf( TEXT("%d"), SumData->m_uiKilled ) );
	ValidSetChildInfoText( Destroyed_mc, "tF_current" , FString::Printf( TEXT("%d"), SumData->m_uiDestroyed ) );
	//ValidSetChildInfoText( Strategy_mc, "tF_current" , FString::Printf( TEXT("%d"), SumData->m_uiStrategy ) );
	//Achievement
	//var GFxOBject AccomplishPt_mc;//score
	eventCallASSetRank( E_FB_ACHIVEMENT, SumData->m_uiAccomplishPt );
	UpdateFBAchievement();
	
	//var array<int> Success;
	//Overall rating
	eventCallASSetRank( E_FB_SUM, SumData->m_uiGradedPt );
	ValidSetChildInfoText( GradedExp_mc, "tF_current" , FString::Printf( TEXT("%dEXP"), SumData->m_uiGradedExp ) );
	//Extra bonus
	ValidSetChildInfoText( VIPExp_mc, "tF_current" , FString::Printf( TEXT("%dEXP"), SumData->m_uiVIPExp ) );
	ValidSetChildInfoText( GuildExp_mc, "tF_current" , FString::Printf( TEXT("%dEXP"), SumData->m_uiGuildExp ) );
	ValidSetChildInfoText( ActivityExp_mc, "tF_current" , FString::Printf( TEXT("%dEXP"), SumData->m_uiActivityExp ) );
	ValidSetChildInfoText( FirstTimeExp_mc, "tF_current" , FString::Printf( TEXT("%dEXP"), SumData->m_uiFirstTimeExp ) );
	ValidSetChildInfoText( CouplesExp_mc, "tF_current" , FString::Printf( TEXT("%dEXP"), SumData->m_uiCouplesExp ) );
	//
	ValidSetChildInfoText( SumUpExp_mc, "tF_exp" , FString::Printf( TEXT("%dEXP"), SumData->GetSumUpExp() ) );
}
//
void UMMOUIFBSummary::UpdateFBAchievement()
{
	MMOMapDataManager &mmoMapDataMgr = MMOMapDataManager::getSingleton();

	TArray<FBAchievementsState> &curFBAchievScore = mmoMapDataMgr.GetFBAchievementsScoreData();

	INT curFBAchievementCnt = curFBAchievScore.Num();
	INT idx = 1;
	for( UINT i = 0; idx <= curFBAchievementCnt; ++idx, ++i ){
		FBAchievementsState &achiScore = curFBAchievScore(i);
		const FDungeonTarget *achiTemplate = achiScore._achievementTemplate;
		if( achiTemplate == NULL ){
			eventUCCallSetFBAchievement( idx, TEXT(""), TEXT(""), -1 );
			continue;
		}
		//
		INT curCompletLv = achiTemplate->TargetLevel;
		if( achiTemplate->IsHighTargetValue ){
			if( achiScore._score < achiTemplate->TargetValue ){
				curCompletLv = EFBAchivType_Invalid;
			}
		}
		else{
			if( achiScore._score > achiTemplate->TargetValue ){
				curCompletLv = EFBAchivType_Invalid;
			}
		}
		
		eventUCCallSetFBAchievement( idx, achiTemplate->TargetName, achiTemplate->TargetDesc, curCompletLv );
	}
	//
	for( INT leftIdx = idx; leftIdx <= MAX_Dungeon_Target; ++leftIdx ){
		eventUCCallSetFBAchievement( leftIdx, TEXT(""), TEXT(""), -1 );
	}
}
//
void UMMOUIFBSummary::PlayerExpChange( INT Exp, INT MaxExp,INT ChangeExp )
{
	INT ExpPercent = 0;

	if( MaxExp == 0 ){
		ExpPercent = 0;
	}
	else{
		ExpPercent = FLOAT( Exp) / MaxExp * 100;
	}

	ExpPercent = Clamp( ExpPercent, 0, 100);

	eventCallASSetCurExpPercent( ExpPercent );
	//if( ExpBar_mc->GetObject(TEXT("mask")) )
	//	ExpBar_mc->GetObject(TEXT("mask"))->SetFloat(TEXT("width"), ExpWidth);
}
//
void UMMOUIFBSummary::PlayerLevelUp(INT NewLv)
{
	bShowLevelUp = true;
	//iLastLevel = NewLv;
}

//=======================================================================================================================================
void UMMOUIFBSummary::UpdateData()
{
	//
	UpdateSummaryPoints();
	//
	UGFxObject *expBarRoleNameMc = ExpBarTitle_mc->GetObject( TEXT("NameTf") );
	if( expBarRoleNameMc != NULL ){
		expBarRoleNameMc->SetString( TEXT("text"), Player->ActorName );
	}

	//
	UGFxObject *expBarRoleLevelMc = ExpBarTitle_mc->GetObject( TEXT("LevelTf") );
	if( expBarRoleLevelMc != NULL ){
		FString levelString = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBSummaryRoleLv") ), Player->ActorAttributes->LevelAttributes.Level );
		expBarRoleLevelMc->SetString( TEXT("text"), levelString );
	}

	//
	PlayerExpChange( Player->ActorAttributes->LevelAttributes.CurrentExp, Player->ActorAttributes->LevelAttributes.MaxExp,0 );
	eventCallASStartExpBar();
}

//
void UMMOUIFBSummary::TimeUpToClose()
{
	bShowCloseBtn = true;
	Show( FALSE );
}
//
void UMMOUIFBSummary::PlayAnimInTime( int time )
{
	bShowCloseBtn = false;

	OpenInTime( time );
	//
	GotoAndPlayI(1);

	ExpBarTitle_mc->SetVisible( false );
	ExpBar_mc->SetVisible( false );
	//eventCallASStartCloseCountDown( time );
}
//
void UMMOUIFBSummary::OpenInTime( int time )
{
	eventCallASStartCloseCountDown( time );
	//
	Show( TRUE );
}

//=======================================================================================================================================
void UMMOUIFBSummary::Init()
{
	Super::Init();

	//
	InitFBSummaryMc();
}

//
void UMMOUIFBSummary::InitFBSummaryMc()
{
	//Statistics, from top to bottom, left to right, then the bottom
	ClearanceTimePt_mc = GetObject( TEXT("titleTime_mc") );
	//SetSummaryRowTitle( ClearanceTimePt_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow1") );

	ClearanceTime_mc = GetObject( TEXT("detailScore0_mc") );
	//SetSummaryRowTitle( ClearanceTime_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow2") );
	//Kill technique
	KillSkillPt_mc = GetObject( TEXT("titleKill_mc") );
	//SetSummaryRowTitle( KillSkillPt_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow3") );
	Killed_mc = GetObject( TEXT("detailScore1_mc") );
	//SetSummaryRowTitle( Killed_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow4") );
	Destroyed_mc = GetObject( TEXT("detailScore2_mc") );
	//SetSummaryRowTitle( Destroyed_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow5") );
	//Strategy_mc = GetObject( TEXT("detailScore3_mc") );
	//SetSummaryRowTitle( Strategy_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow6") );
	//Achievement
	AccomplishPt_mc = GetObject( TEXT("titleAchivement_mc") );
	//SetSummaryRowTitle( AccomplishPt_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow7") );
	//Total score
	GradedExp_mc = GetObject( TEXT("GradedExp_mc") );
	//SetSummaryRowTitle( GradedExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow8") );
	//Extra bonus
	VIPExp_mc = GetObject( TEXT("extraScore0_mc") );
	//SetSummaryRowTitle( VIPExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow9") );
	GuildExp_mc = GetObject( TEXT("extraScore1_mc") );
	//SetSummaryRowTitle( GuildExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow10") );
	ActivityExp_mc = GetObject( TEXT("extraScore2_mc") );
	//SetSummaryRowTitle( ActivityExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow11") );
	CouplesExp_mc = GetObject( TEXT("extraScore3_mc") );
	//SetSummaryRowTitle( CouplesExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow12") );
	FirstTimeExp_mc = GetObject( TEXT("extraScore4_mc") );
	//SetSummaryRowTitle( FirstTimeExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow13") );
	//
	SumUpExp_mc = GetObject( TEXT("expGet_mc") );
	//SetSummaryRowTitle( SumUpExp_mc, TEXT("MMOGame.FBGeneral.FBSummaryRow14") );
	//Experience bar
	ExpBarTitle_mc = GetObject( TEXT("ExpBarTitle_mc") );
	ExpBar_mc = GetObject( TEXT("ExpBar_mc") );
	//Level up hint
	//LevelUp_mc = GetObject( "LevelUp_mc" );
	//iLastLevel = Player.ActorAttributes.LevelAttributes.Level;
	//Skip button
	//Skip_btn = GetObject( "Skip_btn" );
	//Skip_btn.SetBool( "visible", false );
}

//
void UMMOUIFBSummary::SetSummaryRowTitle( UGFxObject *rowMc, const FString &lookUpString )
{
	if( rowMc != NULL ){
		UGFxObject *textMc = rowMc->GetObject( TEXT("tF_title") );
		if( textMc != NULL ){
			textMc->SetString( TEXT("text"), MMOUI::ParseLocalizedPropertyPath( lookUpString ) );
		}
	}
}

//===============================================================================================================
//===============================================================================================================
//called by AS to display the FBAward and begin to play
void UMMOUIFBSummary::NextToPlayFBAward()
{
	TimeUpToClose();

	//start lottery
	UMMOUIFBAward *pFBAwardPanel = (UMMOUIFBAward*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBAward );
	if( pFBAwardPanel != NULL ){
		pFBAwardPanel->OpenToPlay( UCONST_FBAwardMSec );//5 seconds
	}

	//show the hide button
	UMMOUIFBBattleComfirm *pFBBattleConfirm = (UMMOUIFBBattleComfirm*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm );
	if( pFBBattleConfirm != NULL && pFBBattleConfirm->IsVisible ){
		pFBBattleConfirm->CloseFBuiBtn->SetVisible( true );
	}
}
//===============================================================================================================
//===============================================================================================================