#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOBattleManager.h"
#include "net/message_handler.h"

IMPLEMENT_CLASS(UMMOUIBattleSummary)

//
UBOOL UMMOUIBattleSummary::IsNeedShow()
{
	return IsNeedToShow;
}
//
void UMMOUIBattleSummary::NeedToShow()
{
	IsNeedToShow = TRUE;
	QueryData();
}
//
void UMMOUIBattleSummary::TryToShow()
{
	if( IsNeedToShow ){
		Show( TRUE );
	}
}
//
void UMMOUIBattleSummary::QueryData()
{
	CMessageHandler::GetInstance().Send_GetFightSummary();
}
//
void UMMOUIBattleSummary::ClearUI()
{
	IsNeedToShow = FALSE;
}
//
void UMMOUIBattleSummary::PreOnShow( UBOOL v )
{
	Super::PreOnShow( v );

	if( v ){
		UpdateBattleSummary( EBSUM_ScorePanel );

		IsNeedToShow = FALSE;
	}
}
//
void UMMOUIBattleSummary::ResetBattleSummaryData()
{
	MMOBattleManager::getSingleton().ResetEnteredBattle();
}
//
void UMMOUIBattleSummary::ResetBattleSummaryShow()
{
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();

	UpdateBattleScoreList();
	//
	UBOOL canLeave = pBattleManager->IsBattleEnding();
	LeaveBattle_btn->SetVisible( canLeave );
	//hide button
	HideBattle_btn->SetVisible( !canLeave );
}
//
void UMMOUIBattleSummary::UpdateBattleSummary( int tabIdx )
{
	//test
	//MMOBattleManager::getSingleton().TestAddData();

	if( tabIdx < 0 || tabIdx >= EBSUM_MAX ){
		return;
	}

	CurTabPanel = tabIdx;
	//
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	//
	UpdateAllTeamScore();
	//leave button
	UBOOL canLeave = pBattleManager->IsBattleEnding();
	LeaveBattle_btn->SetVisible( canLeave );
	//hide button
	HideBattle_btn->SetVisible( !canLeave );
	//update current showed panel
	if( CurTabPanel == EBSUM_ScorePanel ){
		//update score list
		UpdateBattleScoreList();
	}
	else if( CurTabPanel == EBSUM_PlayDescPanel ){
		//best to refresh only the first time the current battlefield is opened
		UpdateBattlePlayDesc();
	}
	//show it's panel
	eventShowContent( CurTabPanel );
}

//
void UMMOUIBattleSummary::UpdateAllTeamScore()
{
	for( INT i = EBTLTMS_Team1; i < EBTLTMS_MAX; ++i ){
		UpdateTeamScore( i );
	}
	
	//MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	//if( pBattleManager->GetCurBattleSidesCnt() == 3 ){
	//	//three-way battle
	//	TeamScore_mc->GotoAndStopI(2);
	//	//third party
	//	UGFxObject *team2Score_tf = TeamScore_mc->GetObject("Team_tf2");
	//	if( team2Score_tf != NULL ){
	//		team2Score_tf->SetText( appItoa( pBattleManager->GetBattleTeamScore( 2 ) ) );
	//	}
	//}
	//else{
	//	//two-way battle
	//	TeamScore_mc->GotoAndStopI(1);
	//}
	////own side
	//UGFxObject *myTeamScore_tf = TeamScore_mc->GetObject("Team_tf0");
	//if( myTeamScore_tf != NULL ){
	//	myTeamScore_tf->SetText( appItoa( pBattleManager->GetBattleTeamScore( EBTLTMS_Team0 ) ) );
	//}
	////opposing side
	//UGFxObject *team1Score_tf = TeamScore_mc->GetObject("Team_tf1");
	//if( team1Score_tf != NULL ){
	//	team1Score_tf->SetText( appItoa( pBattleManager->GetBattleTeamScore( EBTLTMS_Team1 ) ) );
	//}
}

//
void UMMOUIBattleSummary::UpdateTeamScore( BYTE teamID )
{
	enumBattleTeamSide teamSide = (enumBattleTeamSide)teamID;

	if( teamID >= EBTLTMS_MAX ){
		return;
	}

	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	INT score = pBattleMgr->GetBattleTeamScore( teamSide );

	eventUCSetScore( teamID, score );
	//UGFxObject *teamScore_tf = NULL;
	//switch( teamSide )
	//{
	//case EBTLTMS_Team0:
	//	{
	//		teamScore_tf = TeamScore_mc->GetObject("Team_tf0");
	//	}
	//	break;

	//case EBTLTMS_Team1:
	//	{
	//		teamScore_tf = TeamScore_mc->GetObject("Team_tf1");
	//	}
	//	break;

	//default:
	//	break;
	//}

	////
	//if( teamScore_tf != NULL ){
	//	teamScore_tf->SetText( appItoa( score ) );
	//}
}
//
void UMMOUIBattleSummary::UpdateBattleScoreList()
{
	FBattlePlayer *pBattlePlayer = NULL;
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	//sort
	pBattleManager->SortedPlayersByPointsInTeam();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	//INT cnt = pBattleManager->GetPlayer();
	INT lastTeamID = -1;
	INT myTeamID = -1;
	INT winTeamID = pBattleManager->GetWinTeamID();
	MMOBattleManager::BattlePlayerIterator iter = pBattleManager->BeginIterateBattlePlayer();
	for ( INT idx = 0; iter; ++iter)
	{
		pBattlePlayer = &iter.Value();

		//
		UGFxObject* itemTitleObj = NULL;
		if( pBattlePlayer->_BtlTeamID != lastTeamID ){

			////blank line
			//if( lastTeamID != -1 ){
			//	itemTitleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.TreeListData") );
			//	itemTitleObj->SetInt( TEXT("index"), idx );
			//	itemTitleObj->SetInt( TEXT("type"), 2 );
			//	itemTitleObj->SetInt( TEXT("deepth"), 0 );
			//	itemTitleObj->SetBool( TEXT("visible"), TRUE );

			//	dataProvider->SetElementObject( idx, itemTitleObj );
			//	++idx;
			//}

			//add title
			lastTeamID = pBattlePlayer->_BtlTeamID;

			itemTitleObj = GFXPlayer->CreateObject( TEXT("UI_BattleSummary.BattleSummaryAS3.BtlScoreListTitleData") );
			//check(itemTitleObj != NULL);
			if (itemTitleObj == NULL)
				continue;
			itemTitleObj->SetInt( TEXT("index"), idx );
			itemTitleObj->SetInt( TEXT("type"), 2 );
			itemTitleObj->SetInt( TEXT("deepth"), 0 );
			itemTitleObj->SetBool( TEXT("visible"), TRUE );

			itemTitleObj->SetInt( TEXT("TeamID"), lastTeamID );
			itemTitleObj->SetInt( TEXT("WinTeamID"), winTeamID );
			itemTitleObj->SetBool( TEXT("IsMyTeam"), lastTeamID == myTeamID );

			FString teamNameLookUp = TEXT("MMOGame.BattleSummary.BattleType");
			teamNameLookUp += appItoa(1);
			teamNameLookUp += TEXT("Team");
			teamNameLookUp += appItoa( lastTeamID );
			itemTitleObj->SetString( TEXT("TeamName"), MMOUI::ParseLocalizedPropertyPath( teamNameLookUp ) );

			dataProvider->SetElementObject( idx, itemTitleObj );
			++idx;
		}

		//member score
		UGFxObject* itemObj = GFXPlayer->CreateObject( TEXT("UI_BattleSummary.BattleSummaryAS3.BtlScoreListItemData") );
		//check(itemObj != NULL);	
		if(itemObj == NULL)
			continue;
		itemObj->SetInt( TEXT("index"), idx );
		itemObj->SetInt( TEXT("type"), 0 );
		itemObj->SetInt( TEXT("deepth"), 1 );
		itemObj->SetBool( TEXT("visible"), TRUE );

		itemObj->SetInt( TEXT("PlayerID"), pBattlePlayer->_ID );
		itemObj->SetInt( TEXT("TeamID"), lastTeamID );
		itemObj->SetInt( TEXT("PlayerIconIdx"), pBattlePlayer->_ClassID );

		FString classNameLookupKey = GFXPlayer->GetASHeadIconFrame( pBattlePlayer->_ClassID, pBattlePlayer->_Sex == EGT_Female );
		itemObj->SetString( TEXT("PlayerClassName"), classNameLookupKey );

		itemObj->SetInt( TEXT("PlayerLv"), pBattlePlayer->_Lv );
		FString playerName;
		//playerName = FString::Printf( TEXT("[%s]:%s"), *pBattlePlayer->_ServerName, *pBattlePlayer->_PlayerName );
		playerName = pBattlePlayer->_PlayerName;
		itemObj->SetString( TEXT("PlayerName"), playerName );
		itemObj->SetInt( TEXT("Hunted"), pBattlePlayer->_Score._Kills );
		itemObj->SetInt( TEXT("BeKilled"), pBattlePlayer->_Score._BeKilled );
		itemObj->SetInt( TEXT("Damages"), pBattlePlayer->_Score._Damages );
		itemObj->SetInt( TEXT("Heals"), pBattlePlayer->_Score._Heals );
		itemObj->SetInt( TEXT("BattleScore"), pBattlePlayer->_Score._Sum );
		itemObj->SetInt( TEXT("AwardPoints"), pBattlePlayer->_Awards._Points );

		dataProvider->SetElementObject( idx, itemObj );
		++idx;
	}

	eventFlashBattleScoreList(dataProvider);
}
//
void UMMOUIBattleSummary::UpdateBattlePlayDesc()
{
	FBattleTemplate *pBattle = MMOBattleManager::getSingleton().GetEnteredBattle();
	if( pBattle == NULL ){
		WinDesc_tf->SetString( TEXT("htmlText"), TEXT("") );
		PlayDesc_tf->SetString( TEXT("htmlText"), TEXT("") );
		//BattlePlayImg_ld->SetString( "source", TEXT("") );

		return;
	}

	WinDesc_tf->SetString( TEXT("htmlText"), pBattle->WinDesc );
	PlayDesc_tf->SetString( TEXT("htmlText"), pBattle->ScoreDesc );
	//Play image
	//FString strImg;
	//strImg = FString::Printf( TEXT("img://MMO_UIImg.battlePlayDesc_%d"), pBattle->IconIdx );
	//BattlePlayImg_ld->SetString( "source", strImg );
}
//
void UMMOUIBattleSummary::NotifyBattlePlayerChange()
{

}
//
void UMMOUIBattleSummary::NotifyBattleEnd()
{
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	//
	UBOOL canLeave = pBattleManager->IsBattleEnding();
	LeaveBattle_btn->SetVisible( canLeave );
	//hide button
	HideBattle_btn->SetVisible( !canLeave );
	//
	const FBattlePlayer *pMyScore = pBattleManager->GetMyBattleScoreInfo();
	if( pMyScore == NULL ){
		return;
	}

	BYTE myTeamID = pMyScore->_BtlTeamID;
	BYTE winTeamID = pBattleManager->GetWinTeamID();

	//leave button
	FString btnStr;
	btnStr = ( ( myTeamID == winTeamID ) ? MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm1") : MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm2") );
	LeaveBattle_btn->SetString( TEXT("label"), btnStr );
}
//
void UMMOUIBattleSummary::LeaveBattle()
{
	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	pBattleMgr->SetBattleState( EBTLS_NONE );

	CMessageHandler::GetInstance().Send_LeaveScene();
}

//
void UMMOUIBattleSummary::Init()
{
	Super::Init();

	InitMc();
}

void UMMOUIBattleSummary::InitMc()
{
	UGFxObject* tabBtn = NULL;
	tabBtn = GetObject( TEXT("tab0_mc") );
	if( tabBtn ){
		tabBtn->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm3" ) );
	}	

	tabBtn = GetObject( TEXT("tab1_mc") );
	if( tabBtn ){
		tabBtn->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm4" ) );
	}

	LeaveBattle_btn = GetObject( TEXT("LeaveBattle_btn") );
	HideBattle_btn = GetObject( TEXT("Toggle_btn") );

	BattleScorePanelMC = GetObject( TEXT("BattleScorePanelMC") );
	TeamScore_mc = BattleScorePanelMC->GetObject( TEXT("TeamScore_mc") );
	if( TeamScore_mc != NULL ){
		TeamScore0_mc =TeamScore_mc->GetObject( TEXT("teamNum_mc0") );
		TeamScore1_mc =TeamScore_mc->GetObject( TEXT("teamNum_mc1") );
	}

	for( INT i = 0; i < BtlScoreColumnMax; ++i ){
		FString obName = FString::Printf( TEXT("ColTitle_tf%d"), i + 1 );
		ColTitle_tf[i] = BattleScorePanelMC->GetObject( obName );
	}

	//
	BattleDescPanelMC = GetObject( TEXT("BattleDescPanelMC") );
	if( BattleDescPanelMC != NULL ){
		WinDesc_tf = BattleDescPanelMC->GetObject( TEXT("WinDesc_tf") );
		PlayDesc_tf = BattleDescPanelMC->GetObject( TEXT("PlayDesc_tf") );
	}
}

//==========================================================================================================
//==========================================================================================================
void UMMOUIBattleSummary::ToggleBattleSummary()
{
	if( !IsVisible ){
		//_BattleSummary.NativeShow( true );
		QueryData();
	}
	else{
		NativeShow( FALSE );
	}
}
//==========================================================================================================
void UMMOUIBattleSummary::ASUpdateBattleSummary( INT tabIdx )
{
	UpdateBattleSummary( tabIdx );
}
//==========================================================================================================
void UMMOUIBattleSummary::ASLeaveBattle()
{
	LeaveBattle();
}
//==========================================================================================================
void UMMOUIBattleSummary::ASNotifyBattleTimesUp()
{

}
//==========================================================================================================
//==========================================================================================================