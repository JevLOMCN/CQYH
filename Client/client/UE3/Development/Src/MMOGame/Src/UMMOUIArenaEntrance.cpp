#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOArenaManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIArenaEntrance)

//=======================================================================================
void UMMOUIArenaEntrance::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if( v ){
		//UpdateArenaData();
		CMessageHandler::GetInstance().Send_GetArenaData();
	}
	else{

	}
}
//
//====================================================================================================
void UMMOUIArenaEntrance::OnNativeGameEvent( struct MMOGameEvent* evt )
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

			//
			UpdateArenaData();
		}
		break;
	case GameEvent_ArenaDateUpdated:
		{
			if( !IsVisible ){
				break;
			}
			//
			UpdateArenaData();
		}
	}
}

//=======================================================================================
void UMMOUIArenaEntrance::UpdateArenaData()
{
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	const PlayerArenaData &myArenaData = arenaMgr.GetMyCurArenaData();

	EArenaState curArenaSta = arenaMgr.GetCurArenaState();
	switch( curArenaSta )
	{
	case EAnaSta_Invlid:
	case EAnaSta_WaitApply:
	case EAnaSta_Applied:
		{
			FString timeFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaStartTimeFormat") );
			FString formattedTimeStr = FString::Printf( *timeFormat, arenaMgr.AreanBeginHour(), 0, arenaMgr.AreanEndHour(), 0 );

			FString timeTileFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.PanelTopArenaBeginTime") );
			FString timeTitleStr = FString::Printf( *timeTileFormat, *formattedTimeStr );
			eventCallASSetPanelTopTitleText( timeTitleStr );
			//
			eventCallASSetPanelTopText( TEXT(""), FALSE );
		}
		break;
	case EAnaSta_InGame:
	case EAnaSta_WaitNextRound:
	case EAnaSta_WaitStartRound:
	case EAnaSta_InRound:
	case EAnaSta_ToGetAward:
	case EAnaSta_EndWithAwarded:
	case EAnaSta_EndNoAward:
		{
			//"Your highest win streak"
			eventCallASSetPanelTopTitleText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.PanelTopArenaScoreTitle") ) );
			//Battle record
			FString scoreText;
			if( myArenaData._AllTimeHightScore > 0 ){
				FString chnNumStr = UITextParser::CvtNumToChnNum( myArenaData._AllTimeHightScore );
				if( myArenaData._AllTimeHightScore > 1 ){
					chnNumStr += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.DubbleWin") );
				}
				else{
					chnNumStr += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Win") );
				}
				scoreText = chnNumStr;
			}
			else{
				scoreText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.NoWin") );
			}
			eventCallASSetPanelTopText( scoreText, TRUE );
		}
		break;
	default:
		{
			eventCallASSetPanelTopTitleText( TEXT("") );
			eventCallASSetPanelTopText( TEXT(""), FALSE );
		}
		break;
	}
	//


	//Current arena stake
	UINT64 curArenaMny = arenaMgr.GetCurArenaMoney();
	eventCallASUpdateArenaTotalMoney( UITextParser::HTMLMoneyIconString( MONEYTYPE_02, curArenaMny ) );

	//Own total spending in the arena
	eventCallASSetPlayerName( Player->GetActorName() );

	eventCallASUpdateMyArenaCost( UITextParser::HTMLMoneyIconString( MONEYTYPE_02, myArenaData._SpendMoney ) );

	//Arena participation reward
	INT curLevel = 0;
	FLOAT percent = 0;
	UINT32 curMnyCvt = (UINT32)( curArenaMny / AREANA_AWARD_MONEY_FACTOR );
	for( INT i = 0; i < EArean_RankMax; ++i ){
		const ArenaRankPrize &defalutPrize = arenaMgr.GetArenaDefaultPrize( i );
		UINT64 targetMny = defalutPrize._RankMoneyNeed;
		if( curMnyCvt >= targetMny ){
			curLevel = i + 1;
			if( i < EArean_RankMax - 1 ){
				const ArenaRankPrize &nextPrize = arenaMgr.GetArenaDefaultPrize( i + 1 );
				percent = FLOAT( curMnyCvt - targetMny ) / ( nextPrize._RankMoneyNeed - targetMny );
			}
			else{
				percent = 1;
			}
		}
		else if( i == 0 ){
			percent = FLOAT(curMnyCvt) / targetMny;
		}
	}

	eventCallASSetArenaAwardLevel( curLevel, percent );

	//arena win/loss reward
	UpdateArenaMnyReward();

	//
	UpdatePanelBaseInfo();

	//arena win/loss
	UpdateArenaWinResult();
}
//=======================================================================================
void UMMOUIArenaEntrance::UpdateArenaMnyReward()
{
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	//UINT64 curArenaMny = arenaMgr.GetCurArenaMoney();
	for( INT i = 0; i < EArean_WinPrizeRankMax; ++i ){
		const ArenaWinPrize &winPrizeData = arenaMgr.GetArenaWinPrizeData( i );
		UINT32 winMny = (UINT32)winPrizeData._MoneyToAward;

		//
		dataProvider->SetElementString( i, UITextParser::HTMLMoneyIconString( MONEYTYPE_02, winMny ) );
	}

	//
	eventCallASUpdateCurWinMoney( dataProvider );
}
//=======================================================================================
void UMMOUIArenaEntrance::UpdatePanelBaseInfo()
{
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();
	EArenaState curArenaSta = arenaMgr.GetCurArenaState();
	switch( curArenaSta )
	{
	case EAnaSta_Applied:
		{
			//"Registration successful"
			eventCallASSetPanelBottomText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.EnterSuccessPanelText") ) );
			//"Cancel participation"
			eventCallASSetArenaEnterBtn( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.CanelEnterBtnText") ), TRUE );
			//Entry fee
			eventCallASSetArenaEntryFee( TEXT(""), FALSE );
		}
		break;
	case EAnaSta_ToGetAward:
		{
			//"Claim reward"
			eventCallASSetArenaEnterBtn( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.GetAwardBtnText") ), TRUE );
			//Entry fee
			eventCallASSetArenaEntryFee( TEXT(""), FALSE );
		}
		break;
	case EAnaSta_EndWithAwarded:
		{
			//"Claimed"
			eventCallASSetPanelBottomText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.AwardedPanelText") ) );
			//Entry fee
			eventCallASSetArenaEntryFee( TEXT(""), FALSE );
			//
			eventCallASSetArenaEnterBtn( TEXT(""), FALSE );
		}
		break;
	case EAnaSta_WaitApply:
		{
			//"Registration costs"
			eventCallASSetPanelBottomText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.EnterFeeTitle") ) );
			//Entry fee
			UINT32 entryFee = arenaMgr.GetArenaEntryFee();
			FString strEntryFee = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, entryFee );
			eventCallASSetArenaEntryFee( strEntryFee, TRUE );
			//"Register"
			eventCallASSetArenaEnterBtn( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.EnterBtnText") ), TRUE );
		}
		break;
	default:
		{
			//
			eventCallASSetArenaEnterBtn( TEXT(""), FALSE );
			eventCallASSetPanelBottomText( TEXT("") );
			eventCallASSetArenaEntryFee( TEXT(""), FALSE );
		}
		break;
	}
}

//=======================================================================================
void UMMOUIArenaEntrance::UpdateArenaWinResult()
{
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	UGFxObject* arenaResult = GFXPlayer->CreateObject( TEXT("UI_PVP_Bonus.AS3.BattleResultData") );
	if( arenaResult == NULL ){
		warnf( NAME_Error, TEXT("UMMOUIArenaEntrance::UpdateArenaWinResult create UI_PVP_Bonus.AS3.BattleResultData FAILED") );
		return;
	}
	//Current highest win streak
	INT hightestScore = arenaMgr.GetArenaHighestScore();
	arenaResult->SetString( TEXT("topStreak"), appItoa( hightestScore ) );

	//Own record
	const PlayerArenaData &myArenaData = arenaMgr.GetMyCurArenaData();
	arenaResult->SetString( TEXT("myStreak"), appItoa( myArenaData._AllTimeHightScore ) );

	//
	INT winLevel = myArenaData._WinLevel;

	//
	INT panelRewardType = 0;
	UBOOL isShowAward = FALSE;
	EArenaState curArenaSta = arenaMgr.GetCurArenaState();
	switch( curArenaSta )
	{
	case EAnaSta_Invlid:
	case EAnaSta_WaitApply:
	case EAnaSta_Applied:
	case EAnaSta_InGame:
	case EAnaSta_WaitNextRound:
	case EAnaSta_WaitStartRound:
	case EAnaSta_InRound:
		panelRewardType = 0;
		arenaResult->SetString( TEXT("battleInfo"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.WaitResultHint") ) );
		break;
	case EAnaSta_EndWithAwarded:
		panelRewardType = 1;
		break;
	default:
		{
			//Own reward
			if( winLevel >= EArean_WinPrizeRank1 && winLevel < EArean_RankMax ){
				panelRewardType = 3;
				isShowAward = TRUE;
				//Rank reward
				FString toWinRankLookUp = TEXT("MMOGame.Arena.WinRank");
				toWinRankLookUp += appItoa( winLevel );
				FString winRankStr = MMOUI::ParseLocalizedPropertyPath( toWinRankLookUp );
				arenaResult->SetString( TEXT("rewardInfo"), winRankStr );
			}
			else{
				panelRewardType = 2;
				isShowAward = TRUE;
				//No rank reward
				arenaResult->SetString( TEXT("rewardInfo"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.NoWin") ) );
			}
		}
		break;
	}

	arenaResult->SetInt( TEXT("battleState"), panelRewardType );

	if( isShowAward == FALSE ){
		//arenaResult->SetObject( TEXT("battleReward"), NULL );
	}
	else{
		//Specific reward
		UGFxObject* arrayData = GFXPlayer->CreateArray();
		INT rewardCnt = 0;
		if( arrayData != NULL ){
			//Rank reward, money
			FString winMoneyStr;
			if( winLevel >= EArean_WinPrizeRank1 && winLevel < EArean_RankMax ){
				//const ArenaWinPrize &winPrize = arenaMgr.GetArenaWinPrizeData( winLevel );
				winMoneyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, myArenaData._WinMoney/*winPrize._MoneyToAward*/ );

				//
				UGFxObject* arenaReward = GFXPlayer->CreateObject( TEXT("UI_PVP_Bonus.AS3.BattleRewardData") );
				if( arenaReward == NULL ){
					warnf( NAME_Error, TEXT("UMMOUIArenaEntrance::UpdateArenaWinResult create UI_PVP_Bonus.AS3.BattleResultData FAILED") );
				}
				else{
					arenaReward->SetString( TEXT("iconName"), winMoneyStr );
					arenaReward->SetInt( TEXT("itemID"), MONEYTYPE_02 );
					arenaReward->SetInt( TEXT("itemNum"), myArenaData._WinMoney/*winPrize._MoneyToAward*/ );
					arrayData->SetElementObject( rewardCnt++, arenaReward );
				}
			}

			//Registration reward, item
			INT curdefaultPrizeRank = arenaMgr.GetArenaCurPrizeRank();
			if( curdefaultPrizeRank >= EArean_Rank1 ){
				//const ArenaRankPrize &defalutPrize = arenaMgr.GetArenaDefaultPrize( curdefaultPrizeRank );
				UItem *prizeItem = MMOFacade::getSingleton().FindItemTemplate( myArenaData._WinItemID );
				if( prizeItem != NULL ){
					UGFxObject* arenaItemReward = GFXPlayer->CreateObject( TEXT("UI_PVP_Bonus.AS3.BattleRewardData") );
					if( arenaItemReward == NULL ){
						warnf( NAME_Error, TEXT("UMMOUIArenaEntrance::UpdateArenaWinResult create UI_PVP_Bonus.AS3.BattleResultData FAILED") );
					}
					else{
						arenaItemReward->SetString( TEXT("iconName"), MMOFacade::getSingleton().GetItemIconNamePersonalized( prizeItem ) );
						arenaItemReward->SetInt( TEXT("itemID"), prizeItem->GetTemplateID() );
						UINT32 itemCnt = arenaMgr.GetArenaEnterCnt();
						arenaItemReward->SetInt( TEXT("itemNum"), itemCnt );
						arrayData->SetElementObject( rewardCnt++, arenaItemReward );
					}
				}
			}

			//
			arenaResult->SetObject( TEXT("battleReward"), arrayData );
		}
	}

	//
	eventCallASSetArenaProcess( arenaResult );
}
//=======================================================================================
void UMMOUIArenaEntrance::InitArenaAwards()
{
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if( dataProvider == NULL ){
		warnf( NAME_Error, TEXT("UMMOUIArenaEntrance::InitArenaAwards Create GFXPlayer Array FAILED") );
		return;
	}
	//
	INT addedItem = 0;
	for( INT i = 0; i < EArean_RankMax; ++i ){
		const ArenaRankPrize &defalutPrize = arenaMgr.GetArenaDefaultPrize( i );
		UItem *prizeItem = MMOFacade::getSingleton().FindItemTemplate( defalutPrize._AwardPrizeID );

		//
		if( prizeItem == NULL ){
			warnf( NAME_Error, TEXT("UMMOUIArenaEntrance::InitArenaAwards config award item NotFOUND at:%d"), defalutPrize._AwardPrizeID );
			continue;
		}

		//
		UGFxObject* arenaRewardInView = GFXPlayer->CreateObject( TEXT("UI_PVP_Bonus.AS3.RewardItemData") );
		if( arenaRewardInView == NULL ){
			warnf( NAME_Error, TEXT("UMMOUIArenaEntrance::InitArenaAwards create UI_PVP_Bonus.AS3.RewardItemData FAILED") );
		}
		else{
			UINT64 limitMny = (UINT64)defalutPrize._RankMoneyNeed;
			limitMny *= AREANA_AWARD_MONEY_FACTOR;
			arenaRewardInView->SetString( TEXT("rewardMnyRequire"), UITextParser::HTMLMoneyIconString( MONEYTYPE_02, limitMny ) );

			arenaRewardInView->SetInt( TEXT("itemID"), prizeItem->GetTemplateID() );
			arenaRewardInView->SetString( TEXT("iconName"), MMOFacade::getSingleton().GetItemIconNamePersonalized( prizeItem ) );

			arenaRewardInView->SetString( TEXT("description"), defalutPrize._Desc );

			dataProvider->SetElementObject( addedItem++, arenaRewardInView );
		}
	}

	eventCallASSetAllArenaDefaultRewardInPreview( dataProvider );
}
//=======================================================================================
void UMMOUIArenaEntrance::ApplyForArena()
{
	FString applyError;
	UBOOL isOK = MMOArenaManager::getSingleton().CanApplyArena( ctrl, applyError );
	if( isOK != TRUE ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( applyError ), MsgInfoType_Table, applyError );
		return;
	}

	CMessageHandler::GetInstance().Send_ApplyForArena();
}
//=======================================================================================
void UMMOUIArenaEntrance::CanelApplyForArena()
{
	CMessageHandler::GetInstance().Send_QuitArena();
}
//=======================================================================================
void UMMOUIArenaEntrance::ToGetArenaReward()
{

}
//=======================================================================================
void UMMOUIArenaEntrance::Init()
{
	Super::Init();

	//
	InitMc();

	//
	InitArenaAwards();
	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem( GameEvent_ArenaStateChanged );
	evts.AddItem( GameEvent_ArenaDateUpdated );
	EventAdapter = new TGameEventAdapter<UMMOUIArenaEntrance>(this, evts);
}
//=======================================================================================
void UMMOUIArenaEntrance::InitMc()
{
	eventCallASInitRewardViewItemSlot( 0, ESPType_PVPArena, EArenaSlot_ViewSlot1 );
	eventCallASInitRewardViewItemSlot( 1, ESPType_PVPArena, EArenaSlot_ViewSlot2 );
	eventCallASInitRewardViewItemSlot( 2, ESPType_PVPArena, EArenaSlot_ViewSlot3 );
	eventCallASInitRewardViewItemSlot( 3, ESPType_PVPArena, EArenaSlot_ViewSlot4 );

	eventCallASInitResultRewardItemSlot( 0, ESPType_PVPArena, EArenaSlot_ResultSlot1 );
}

//========================================================================================
//========================================================================================
UBOOL UMMOUIArenaEntrance::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	//`log( "strHoverControllerName : "$strHoverControllerName );
	if( IsVisible )
	{
		if( strHoverControllerName == TEXT("panelMask") )
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

//========================================================================================
void UMMOUIArenaEntrance::ASOnActionButtonClick()
{
	EArenaState curArenaSta = MMOArenaManager::getSingleton().GetCurArenaState();
	switch( curArenaSta )
	{
	case EAnaSta_Applied:
		{
			CanelApplyForArena();
		}
		break;
	case EAnaSta_WaitApply:
		{
			ApplyForArena();
		}
		break;
	case EAnaSta_ToGetAward:
		{
			ToGetArenaReward();
		}
		break;
	default:
		break;
	}
}
//========================================================================================
void UMMOUIArenaEntrance::ASToggleShowBonusPanel( UBOOL isToShow )
{
	if( isToShow == TRUE ){
		//arena win/loss reward
		UpdateArenaMnyReward();
		//arena win/loss
		UpdateArenaWinResult();
	}

}
//========================================================================================
//========================================================================================