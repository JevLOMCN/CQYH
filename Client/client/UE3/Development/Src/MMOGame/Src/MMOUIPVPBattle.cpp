#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOBattleManager.h"
#include "MMOChatManager.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
//#include "MMOMsgButtonManager.h"
#include "MMOBattleManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIPVPBattle)

void UMMOUIPVPBattle::ClearUI()
{
	//load dynamicly
	IsDynamicLoad = TRUE;
	//reset data used in this ui
	ResetBattleData();
	//reset selected random battles
	MMOBattleManager::getSingleton().ResetRandomBattleData();
}

//
void UMMOUIPVPBattle::PreOnShow( UBOOL show )
{
	Super::PreOnShow( show );

	//if( show && !IsWaitingBattle() && ConfirmMsgBox == NULL ){
	if( show && ConfirmMsgBox == NULL ){
		//Clear
			ResetBattleData();
		//ResetBattleShow();

		//Set battleground list
		UpdateBattleList();

		//
		if( !IsRandomMode ){
			//Set the currently selected battleground UI
			UpdateCurBattle( SelectedBtlID );
		}
		else{
			//Set random battleground UI
			UpdateRandomBattlePanel();
		}

		//check button state
		CheckJoinButtons();
		//update Free/random view visibility
		eventSetBattlePanelMode( IsRandomMode );
	}
	else{

	}
}
//
void UMMOUIPVPBattle::UpdateTime( float dt )
{
	if( ConfirmMsgBoxCountDownTime > 0 ){
		ConfirmMsgBoxCountDownTime -= dt;

		if( ConfirmMsgBox ){
			if( Player ){
				UGFxObject *yesButton = ConfirmMsgBox->GetObject( TEXT("yesButton_mc") );
				if( yesButton != NULL ){
					yesButton->SetBool( TEXT("enabled"), !Player->eventIsDead() );
				}
			}

			if( ConfirmMsgBoxCountDownTime >= 0 ){
				MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
				INT readyJoinID = pBattleManager->GetJoinedBattleID();
				INT leftSec = (INT)ConfirmMsgBoxCountDownTime;
				FString msgInfo = ConfirmMsgBoxString + appItoa( leftSec ) + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg6") );

				UGFxObject* bgMc = ConfirmMsgBox->GetObject( TEXT("MyWindowsFrame_mc") );
				if( bgMc != NULL ){
					UGFxObject* contentTf = bgMc->GetObject( TEXT("content") );
					if( contentTf != NULL ){
						contentTf->SetString( TEXT("htmlText") , msgInfo );
					}					
				}
			}
			else{
				//UMMOGFXPlayer_MessageBox *mvPlayer = (UMMOGFXPlayer_MessageBox*)MMOUI::FindMoviePlayer( MMOUI::GetUIManager()->MoviePlayer_MessageBox );
				//if( mvPlayer != NULL && mvPlayer->_MsgBoxMgr != NULL ){
				//	mvPlayer->_MsgBoxMgr->GetMessageAck( false, ConfirmMsgBox );
				//}
				MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo(ConfirmMsgBox);
			}
		}
	}
}

//
void UMMOUIPVPBattle::ResetBattleData()
{
	MMOBattleManager &battleMgr = MMOBattleManager::getSingleton();
	if( battleMgr.IsWaiting() == TRUE ){
		IsRandomMode = ( SelectedBtlID == RANDOM_BATTLE_ID );

		SelectedBtlID = battleMgr.GetJoinedBattleID();

		SelectedBtnIdx = battleMgr.GetBattleIdxByID( SelectedBtlID );
		CurBtlListPage = 0;	
	}
	else{
		IsRandomMode = FALSE;

		FBattleTemplate *pBattleInfo = battleMgr.GetBattleTemplateIn( 0 );
	SelectedBtlID = ( pBattleInfo != NULL ? pBattleInfo->ID : -1 );

	SelectedBtnIdx = ( pBattleInfo != NULL ? 0 : -1 );
	CurBtlListPage = 0;
	}
}
//
void UMMOUIPVPBattle::ResetBattleShow()
{
	//
	eventSetBattlePanelMode( IsRandomMode );
	//Select battle buttons
	for( INT i = 0; i < BtlListMax; ++i ){
		BtlBtns[i]->SetString( TEXT("label"), "" );
		BtlBtns[i]->SetBool( TEXT("enabled"), FALSE );
	}
	//detail panel
	if( IsRandomMode ){
		UpdateRandomBattleList();
	}
	else{
		//page
		//INT cnt = MMOFacade::getSingleton().GetBattleTemplateCnt();
		//INT pageCnt = cnt / ListMax + ( ( cnt % ListMax != 0 ) ? 1 : 0 );
		//FString pageText;
		//pageText.Printf( TEXT( "%d / %d" ), 0, 0 );
		//BtlPageTf->SetText( pageText );
		//
		ResetCurBattleShow();
	}

	//join button
	BtlTeamJoinBtn->SetBool( TEXT("enabled"), FALSE );
	BtlSingleJoinBtn->SetBool( TEXT("enabled"), FALSE );

	//leave button
	BtlLeaveBtn->SetBool( TEXT("enabled"), FALSE );
}

//
void UMMOUIPVPBattle::CheckJoinButtons()
{
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	UBOOL isWaitingBattle = pBattleManager->IsWaiting();

	UBOOL canJoin = ( ConfirmMsgBox == NULL );
	if( IsRandomMode ){
		UBOOL canRandomJoin = FALSE;
		canRandomJoin = ( pBattleManager->GetRandomMinLv() <= Player->ActorAttributes->LevelAttributes.Level )
			&& ( pBattleManager->GetRandomMin() <= pBattleManager->GetRandomCur() )
			&& ( pBattleManager->GetRandomCur() <= pBattleManager->GetRandomMax() );
		//
		canJoin &= !isWaitingBattle && canRandomJoin;
	}
	else{
		canJoin &= !isWaitingBattle && SelectedBtlID > 0;
	}

	//
	UBOOL canTeamJoin = canJoin && TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader();
	BtlTeamJoinBtn->SetBool( TEXT("enabled"), canTeamJoin );

	UBOOL canSingleJoin = canJoin && !TeamManager::getSingleton().HasTeam();
	BtlSingleJoinBtn->SetBool( TEXT("enabled"), canSingleJoin );

	//BtlQuickSelectBtn->SetBool( TEXT("enabled"), canSingleJoin || canTeamJoin );
	//leave button
	BtlLeaveBtn->SetBool( TEXT("enabled"), isWaitingBattle );
}
//
void UMMOUIPVPBattle::ResetCurBattleShow()
{
	BtlPicImgLoder->SetString( "source", "" );
	BtlDescTf->SetText( "" );
	for( INT i = 0; i < BtlItemMax; ++i ){
		BtlAwardItemsIcon[i]->SetVisible( FALSE );
	}
	BtlAwardExpTf->SetText( "" );
	BtlLevelTf->SetText( "" );
}
//
void UMMOUIPVPBattle::UpdateBattleList()
{
	FBattleTemplate *pBattle = NULL;
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	INT cnt = pBattleManager->GetBattleTemplateCnt();
	if( cnt <= 0 ){
		//no battles
		return;
	}

	//INT pageCnt = cnt / BtlListMax + ( ( cnt % BtlListMax != 0 ) ? 1 : 0 );
	//FString pageText;
	//pageText = FString::Printf( TEXT("%d / %d"), CurBtlListPage + 1, pageCnt );
	//BtlPageTf->SetText( pageText );
	//
	INT idx = CurBtlListPage * BtlListMax;
	FString battleName;
	UBOOL canSelect;
	UBOOL isSelected;
	for( INT i = 0; i < BtlListMax; ++i, ++idx ){
		pBattle = pBattleManager->GetBattleTemplateIn( idx );

		if( pBattle != NULL ){
			battleName = pBattle->DisplayName;
			canSelect = TRUE;
			isSelected = ( SelectedBtlID == pBattle->ID ) ? TRUE : FALSE;
			//battle Icon
			BtlBtns[i]->GetObject( TEXT("icon_mc") )->GotoAndStopI( pBattle->IconIdx );
		}
		else{
			battleName = "";
			canSelect = FALSE;
			isSelected = FALSE;
			//
			BtlBtns[i]->SetVisible( canSelect );
			continue;
		}
		//name
		BtlBtns[i]->SetString( TEXT("label"), battleName );
		//active
		BtlBtns[i]->SetBool( TEXT("enabled"), canSelect );
		//selected
		BtlBtns[i]->SetBool( TEXT("selected"), isSelected );
	}
}
//
void UMMOUIPVPBattle::QuickSelectBattle()
{
	if( IsRandomMode == FALSE ){
		MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
		//if level is too low
		if( pBattleManager->GetRandomMinLv() > Player->ActorAttributes->LevelAttributes.Level ){
			//too little
			FString alarm = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleErr3") ), pBattleManager->GetRandomMinLv() );
			MMOUI::GetUIManager()->delegateAddScreenMessage( alarm,MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleErr3") );

			if( SelectedBtnIdx >= 0 ){
				BtlBtns[SelectedBtnIdx]->SetBool( TEXT("selected"), TRUE );
			}

			return;
		}
		//Set mode to random
		IsRandomMode = TRUE;
		//set selected battle button to nothing
		SelectedBtlID = -1;
		SelectedBtnIdx = -1;

		//
		UpdateRandomBattlePanel();

		//check button state
		CheckJoinButtons();
		//update Free/random view visibility
		eventSetBattlePanelMode( IsRandomMode );
	}
}

void UMMOUIPVPBattle::UpdateRandomBattlePanel()
{
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();

		//update the all battle list
		UpdateRandomBattleList();

		//updat random battle description
		FBattleTemplate *pBattle = NULL;
		pBattle = pBattleManager->GetRandomBattle();
	if( pBattle == NULL ){
		warnf( NAME_Error, TEXT("GetRandomBattle not found at UMMOUIPVPBattle::UpdateRandomBattlePanel") );
		return;
	}

		RdmBtlDescTf->SetText( pBattle->Desc );
		//Awards
		INT itemCnt = pBattle->Reward.Items.Num();
		INT rewardNum = 0;
		for( INT i = 0; i < RdmBtlItemMax; ++i ){
			if( i >= itemCnt ){
				RdmBtlAwardItemsIcon[i]->SetVisible( FALSE );
			}
			else{
				RdmBtlAwardItemsIcon[i]->SetVisible( TRUE );

				rewardNum = pBattle->Reward.Items(i).Num;
				INT itemID = pBattle->Reward.Items(i).ID;
				if( itemID < E_MONEYTYPE_MAX ){//it is money
					if( itemID >= E_MONEYTYPE_01 ){
						FString moneyName;
						FString moneyIcon = UITextParser::GetMoneyIconImgAndName( moneyName, itemID );

						FString iconDesc = TEXT("");
						if( rewardNum > 1 ){
							iconDesc = appItoa( rewardNum );
							iconDesc += moneyName;
						}
						else{
							iconDesc = moneyName;
						}

						GFXPlayer->SetNoninteractiveIconSlot( RdmBtlAwardItemsIcon[i], itemID, moneyIcon, iconDesc, ESPType_PVPBattle );
					}
				}
				else{
					UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( itemID );

					FString iconDesc = TEXT("");
					if( rewardNum > 1 ){
						iconDesc = appItoa( rewardNum );
					}
					GFXPlayer->SetNoninteractiveIconSlotByItem( RdmBtlAwardItemsIcon[i], pItem, iconDesc, ESPType_PVPBattle );
				}
			}	
		}//end of for

		//exp reward
		//RdmBtlAwardExpTf->SetText( appItoa( pBattle->Reward.Exp ) );
		//
		RdmBtlLevelTf->SetText( appItoa( pBattleManager->GetRandomMinLv() ) );
}

//
void UMMOUIPVPBattle::UpdateRandomBattleList()
{
	FBattleTemplate *pBattle = NULL;
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	INT cnt = pBattleManager->GetBattleTemplateCnt();
	for ( INT i = 0 ; i < cnt; ++i)
	{
		pBattle = pBattleManager->GetBattleTemplateIn( i );
		//check(pBattle != NULL);
		if(pBattle == NULL)
			continue;

		UGFxObject* itemObj = GFXPlayer->CreateObject( TEXT("ActionScript3.Battle.BattlesListItemData") );
		//check(itemObj != NULL);
		if(itemObj == NULL)
			continue;

		itemObj->SetInt( TEXT("BattleID"), pBattle->ID );
		itemObj->SetString( TEXT("BattleName"), pBattle->DisplayName );
		itemObj->SetInt( TEXT("BattleIconIdx"), pBattle->IconIdx );
		itemObj->SetBool( TEXT("BattleSelected"), pBattle->IsSelected );

		dataProvider->SetElementObject( i, itemObj );
	}

	eventFlashRandomBattleList(dataProvider);
}
//
void UMMOUIPVPBattle::UpdateCurBattle( INT battleID )
{
	FBattleTemplate *pBattle = NULL;
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	pBattle = pBattleManager->FindBattleTemplateBy( battleID );
	if( pBattle == NULL ){
		ResetCurBattleShow();
		return;
	}
	//
	BattleNameTf->SetText( pBattle->DisplayName );
	//picture
	FString strImg;
	strImg = FString::Printf( TEXT("img://MMO_UIImg.battle_%d"), pBattle->ID );
	BtlPicImgLoder->SetString( "source", strImg );
	//description
	BtlDescTf->SetText( pBattle->Desc );
	//award items
	INT itemCnt = pBattle->Reward.Items.Num();
	INT rewardNum = 0;
	for( INT i = 0; i < BtlItemMax; ++i ){
		//pBattle = pBattleManager->GetBattleTemplateIn( i );
		if( i >= itemCnt ){
			BtlAwardItemsIcon[i]->SetVisible( FALSE );
		}
		else{
			BtlAwardItemsIcon[i]->SetVisible( TRUE );

			rewardNum = pBattle->Reward.Items(i).Num;
			INT itemID = pBattle->Reward.Items(i).ID;
			if( itemID < E_MONEYTYPE_MAX ){//it is money
				if( itemID >= E_MONEYTYPE_01 ){
					FString moneyName;
					FString moneyIcon = UITextParser::GetMoneyIconImgAndName( moneyName, itemID );

					FString iconDesc = TEXT("");
					if( rewardNum > 1 ){
						iconDesc = appItoa( rewardNum );
						iconDesc += moneyName;
					}
					else{
						iconDesc = moneyName;
					}

					GFXPlayer->SetNoninteractiveIconSlot( RdmBtlAwardItemsIcon[i], itemID, moneyIcon, iconDesc, ESPType_PVPBattle );
				}
			}
			else{
				UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( itemID );
				
				FString iconDesc = TEXT("");
				if( rewardNum > 1 ){
					iconDesc = appItoa( rewardNum );
				}
				GFXPlayer->SetNoninteractiveIconSlotByItem( BtlAwardItemsIcon[i], pItem, iconDesc, ESPType_PVPBattle );
			}

		}	
	}
	//award experience
	//BtlAwardExpTf->SetText( appItoa( pBattle->Reward.Exp ) );
	//battle level
	FExportMap *mapExportInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( pBattle->ID );
	if( mapExportInfo != NULL ){
		FString levelStringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.LevelStringFormat") );
		BtlLevelTf->SetText( FString::Printf( *levelStringFormat, mapExportInfo->MinPlayerLevel, mapExportInfo->MaxPlayerLevel ) );
	}
	else{
		BtlLevelTf->SetText( TEXT("No Data") );
	}
}
//
void UMMOUIPVPBattle::UpdateMyTeam()
{
	if( !IsVisible ){
		return;
	}

	CheckJoinButtons();
}
//
void UMMOUIPVPBattle::PlayerLevelChange( INT lastLv , INT newLv )
{
	if( !IsVisible || !IsRandomMode ){
		return;
	}

	CheckJoinButtons();
}
//
void UMMOUIPVPBattle::SelectBattle( INT idx )
{
	FBattleTemplate *pBattle = NULL;
	SelectedBtnIdx = idx;
	pBattle = MMOBattleManager::getSingleton().GetBattleTemplateIn( CurBtlListPage * BtlListMax + idx );
	if( pBattle == NULL ){
		return;
	}
	//get the selected
	SelectedBtlID = pBattle->ID;
	//
	UpdateCurBattle( SelectedBtlID );
	//if back from random mode
	if( IsRandomMode ){
		//back to select battle view
		IsRandomMode = FALSE;
		//check button state
		CheckJoinButtons();
		//update Free/random view visibility
		eventSetBattlePanelMode( IsRandomMode );
	}
}

//
UBOOL UMMOUIPVPBattle::ChkIsMyLevelForBattle( int battleID )
{
	UBOOL isLevelOK = FALSE;
	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( battleID );
	if( Player == NULL ){
		return FALSE;
	}

	if( mapInfo.ServerID == SelectedBtlID ){
		INT myLevel = Player->ActorAttributes->LevelAttributes.Level;
		isLevelOK = ( myLevel >= mapInfo.MinPlayerLevel && myLevel <= mapInfo.MaxPlayerLevel );
	}

	INT teammateCnt = TeamManager::getSingleton().GetTeamMemberCount();
	for( INT i = 0; i < teammateCnt ; ++i ){
		MMOTeamMember* tMember = TeamManager::getSingleton().GetTeamMember(i);
		if(tMember)
		{
			isLevelOK &= ( tMember->Lv >= mapInfo.MinPlayerLevel && tMember->Lv <= mapInfo.MaxPlayerLevel );
		}

	}

	return isLevelOK;
}
//
void UMMOUIPVPBattle::MultiSelectBattle( INT id, UBOOL isCheck )
{
	//check random count
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	INT curSltCnt = pBattleManager->GetRandomCur();
	if( pBattleManager->GetRandomMin() >= curSltCnt && isCheck == FALSE ){
		//too little
		FString alarm = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleErr1") ), pBattleManager->GetRandomMin() );
		MMOUI::GetUIManager()->delegateAddScreenMessage( alarm ,MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleErr1"));
	}
	if( curSltCnt >= pBattleManager->GetRandomMax() && isCheck == TRUE ){
		//too many
		FString alarm = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleErr2") ), pBattleManager->GetRandomMax() );
		MMOUI::GetUIManager()->delegateAddScreenMessage( alarm ,MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleErr2"));
	}

	MMOBattleManager::getSingleton().ToggleRandomBattles( id, isCheck );

	//check button state
	CheckJoinButtons();
}
//-----------------
void UMMOUIPVPBattle::NotifyEnterBattle()
{
	//MMOBattleManager::getSingleton().SetWaiting( FALSE );
	//
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT( "AgreeEnterBattle" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "DiagreeEnterBattle" );
	//popup message
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();
	INT readyJoinID = pBattleManager->GetJoinedBattleID();
	FString msgInfo;
	if( readyJoinID > 0 ){
		//join a specific battle
		FBattleTemplate *pBattle = MMOBattleManager::getSingleton().FindBattleTemplateBy( readyJoinID );
		if( pBattle != NULL ){
			FString formatStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg2") );
			msgInfo = FString::Printf( *formatStr, *pBattle->DisplayName );
		}
	}
	else if( readyJoinID == 0 ){
		//join a random battle
		msgInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg1") );
	}

	//CountDown 30 sec
	ConfirmMsgBoxCountDownTime = 30;
	ConfirmMsgBoxString = msgInfo;
	msgInfo += appItoa( (INT)ConfirmMsgBoxCountDownTime ) + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg6") );
	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgInfo, readyJoinID, funYes, funNo );

	CheckJoinButtons();
	//MMOMsgButtonManager::getSingleton().AddNewButton( msgInfo, readyJoinID, funYes, funNo );
}
//
void UMMOUIPVPBattle::AgreeEnterBattle()
{
	//Already entered battleground, restore dynamic loading so it gets destroyed
	IsDynamicLoad = TRUE;

	MMOBattleManager::getSingleton().ConfirmJoinBattle( TRUE );
	//
	ConfirmMsgBox = NULL;
	//
	if( IsVisible ){
		Show( FALSE );
	}
}
//
void UMMOUIPVPBattle::DiagreeEnterBattle()
{
	//Restore dynamic loading
	IsDynamicLoad = TRUE;

	MMOBattleManager::getSingleton().ConfirmJoinBattle( FALSE );
	//
	ConfirmMsgBox = NULL;

	CheckJoinButtons();
	//Equivalent to leaving the battleground queue
	NotifyLeaveBattle();
}
//----------------------------------------------------
//void UMMOUIPVPBattle::TeamJoinBattle()
//{
//	if( IsRandomMode ){
//		//random battle
//		MMOBattleManager::getSingleton().RandomJoinBattle();
//	}
//	else if( SelectedBtlID > 0 ){
//		//free select battle
//		MMOBattleManager::getSingleton().TeamJoinBattle( SelectedBtlID );
//	}
//}
//void UMMOUIPVPBattle::SingleJoinBattle()
//{
//	if( IsRandomMode ){
//		//random battle
//		MMOBattleManager::getSingleton().RandomJoinBattle();
//	}
//	else if( SelectedBtlID > 0 ){
//		//free select battle
//		MMOBattleManager::getSingleton().SingleJoinBattle( SelectedBtlID );
//	}
//}
void UMMOUIPVPBattle::JoinSingleBattle()
{
	if( IsRandomMode ){
		//random battle
		MMOBattleManager::getSingleton().RandomJoinBattle();
	}
	else if( SelectedBtlID > 0 ){
		//free select battle
		UBOOL isLevelOK = ChkIsMyLevelForBattle( SelectedBtlID );
		if( isLevelOK == TRUE ){
			MMOBattleManager::getSingleton().JoinSingleBattle( SelectedBtlID );
		}
		else{
			//level error
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleErr4"),MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleErr4") );
		}
	}
}
//
void UMMOUIPVPBattle::NotifyJoinedBattle( int battleID )
{
	//No dynamic for now, switch to dynamic loading after entering the battleground
	IsDynamicLoad = FALSE;

	//disable join others
	BtlTeamJoinBtn->SetBool( TEXT("enabled"), FALSE );
	BtlSingleJoinBtn->SetBool( TEXT("enabled"), FALSE );
	//BtlQuickSelectBtn->SetBool( TEXT("enabled"), FALSE );
	//enable leave
	BtlLeaveBtn->SetBool( TEXT("enabled"), TRUE );

	//hint
	FString hint;
	UBOOL bRet = FALSE;
	if( battleID == RANDOM_BATTLE_ID ){
		//joined random battle
		hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg3"), bRet );
	}
	else if( battleID > 0 ){
		//joined selected battle
		FBattleTemplate *pBattle = MMOBattleManager::getSingleton().FindBattleTemplateBy( battleID );
		if( pBattle != NULL ){
			hint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg4"), bRet ), *pBattle->DisplayName );
		}	
	}

#if SHIPPING_PC_GAME
	if(bRet)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleMsg4"));
		MMOChatManager::getSingleton().AddSystemMessage( hint );
	}
#else
	MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleMsg4"));
	MMOChatManager::getSingleton().AddSystemMessage( hint );
#endif
}
//----------------------------------------------------
void UMMOUIPVPBattle::LeaveBattle()
{
	//Leave the queue, restore dynamic loading
	IsDynamicLoad = TRUE;

	//set data and send message
	MMOBattleManager::getSingleton().LeaveWaitingBattle();
	//hint
	FString hint;
	hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleMsg5") );

	MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,TEXT("MMOGame.BattleField.BattleMsg5"));

	MMOChatManager::getSingleton().AddSystemMessage( hint );
}
//
void UMMOUIPVPBattle::NotifyLeaveBattle()
{
	//MMOBattleManager::getSingleton().SetWaiting( FALSE );
	//MMOBattleManager::getSingleton().SetJoinedBattleID( -1 );

	CheckJoinButtons();
}
//----------------------------------------------------
void UMMOUIPVPBattle::BattlePageUp()
{
	INT cnt = MMOBattleManager::getSingleton().GetBattleTemplateCnt();
	INT pageCnt = cnt / BtlListMax + ( ( cnt % BtlListMax != 0 ) ? 1 : 0 );

	if( CurBtlListPage >= pageCnt - 1 ){
		return;
	}
	++CurBtlListPage;
	//update show
	UpdateBattleList();
}
//
void UMMOUIPVPBattle::BattlePageDown()
{
	if( CurBtlListPage <= 0 ){
		return;
	}
	--CurBtlListPage;
	//update show
	UpdateBattleList();
}
//
UBOOL UMMOUIPVPBattle::IsWaitingBattle()
{
	return MMOBattleManager::getSingleton().IsWaiting();
}
//
void UMMOUIPVPBattle::Init()
{
	Super::Init();
	if( GFXPlayer ){
		GFXPlayer->AddPostAdvanceDelegate( FScriptDelegate(this, TEXT("PostAdvance")) );
	}

	InitBattleMc();
}
void UMMOUIPVPBattle::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemovePostAdvanceDelegate( FScriptDelegate(EC_EventParm) );
	}
}

//
void UMMOUIPVPBattle::InitBattleMc()
{
	//BattlePanelMC = GetObject( "BattlePanelMC" );

	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );

	for( INT i = 0; i < BtlListMax; ++i ){
		FString obName = FString::Printf( TEXT("BtlBtns%d"), i );
		//BtlBtns[i] = BattlePanelMC.GetObject( "BtlBtns"$i );
		BtlBtns[i] = GetObject( obName );
	}
	//BtlQuickSelectBtn = BattlePanelMC.GetObject( "BtlQuickSelectBtn" );
	BtlQuickSelectBtn = GetObject( TEXT("BtlQuickSelectBtn") );
	//Single select
	//BtlFreeSelectPanel = BattlePanelMC.GetObject( "FreeSelect_mc" );
	BtlFreeSelectPanel = GetObject( TEXT("FreeSelect_mc") );

	//BtlPageTf = BtlFreeSelectPanel.GetObject( "BtlPageTf" );
	BattleNameTf = BtlFreeSelectPanel->GetObject( TEXT("BattleNameTf") );
	BtlPicImgLoder = BtlFreeSelectPanel->GetObject( TEXT("BtlPicImgLoder") );
	BtlDescTf = BtlFreeSelectPanel->GetObject( TEXT("BtlDescTf") );
	for( INT i = 0; i < BtlItemMax; ++i ){
		FString obName = FString::Printf( TEXT("BtlAwardItemsIcon%d"), i );
		BtlAwardItemsIcon[i] = BtlFreeSelectPanel->GetObject( obName );
	}
	BtlAwardExpTf = BtlFreeSelectPanel->GetObject( TEXT("BtlAwardExpTf") );
	BtlAwardExpTf->SetVisible( false );
	BtlLevelTf = BtlFreeSelectPanel->GetObject( TEXT("BtlLevelTf") );
	//Random
	//RdmBtlSelectPanel = BattlePanelMC.GetObject( "RandomSelect_mc" );
	RdmBtlSelectPanel = GetObject( TEXT("RandomSelect_mc") );

	RdmBtlDescTf = RdmBtlSelectPanel->GetObject( TEXT("BtlDescTf") );
	for( INT i = 0; i < RdmBtlItemMax; ++i ){
		FString obName = FString::Printf( TEXT("BtlAwardItemsIcon%d"), i );
		RdmBtlAwardItemsIcon[i] = RdmBtlSelectPanel->GetObject( obName );
	}
	RdmBtlAwardExpTf = RdmBtlSelectPanel->GetObject( TEXT("BtlAwardExpTf") );
	RdmBtlAwardExpTf->SetVisible( false );
	RdmBtlLevelTf = RdmBtlSelectPanel->GetObject( TEXT("RdmBtlLevelTf") );

	//
	//BtlTeamJoinBtn = BattlePanelMC.GetObject( "BtlTeamJoinBtn" );
	//BtlSingleJoinBtn = BattlePanelMC.GetObject( "BtlSingleJoinBtn" );
	//BtlLeaveBtn = BattlePanelMC.GetObject( "BtlLeaveBtn" );
	BtlTeamJoinBtn = GetObject( TEXT("BtlTeamJoinBtn") );
	BtlTeamJoinBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BtlTeamJoinBtnText") ) );

	BtlSingleJoinBtn = GetObject( TEXT("BtlSingleJoinBtn") );
	BtlSingleJoinBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BtlSingleJoinBtnText") ) );

	BtlLeaveBtn = GetObject( TEXT("BtlLeaveBtn") );
	BtlLeaveBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BtlLeaveBtnText") ) );
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIPVPBattle::PostAdvance( FLOAT DeltaTime )
{
	//Super::PostAdvance( DeltaTime );
	if( !IsWaitingBattle() ){
		TickForTestInteractNPCDistance(DeltaTime);
	}

	if( ConfirmMsgBox != NULL ){
		UpdateTime( DeltaTime );
	}
}
//===============================================================================================================
//===============================================================================================================