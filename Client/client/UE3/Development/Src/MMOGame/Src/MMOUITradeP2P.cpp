#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ClientActionConditionCheck.h"
#include "item_def.h"
#include "MMOChatManager.h"
#include "MMOMsgButtonManager.h"
#include "FSMIO.h"
#include "MMOMapDataManager.h"
#include "MMOMsgBoxManager.h"
#include "UILayout.h"
#include "UITextParser.h"
#include "FriendManager.h"

IMPLEMENT_CLASS(UMMOUITradeP2P)

void UMMOUITradeP2P::PreOnShow(UBOOL show)
{
	Super::PreOnShow(show);

	//eventOnShowP2P(show);
	if (show){
		//ResetData();
		ResetShow();
	}
	else{
		ctrl = MMOFacade::getSingleton().GetMainController();

		//UMMOGFXPlayer_MessageBox *mMsgBoxPlayer = (UMMOGFXPlayer_MessageBox*)MMOUI::FindMoviePlayer( MMOUI::GetUIManager()->MoviePlayer_MessageBox );
		if (ctrl != NULL && ctrl->TradeTargetID != 0){
			if (ConfirmTradeBox != NULL){
				//Confirm box closed
				//mMsgBoxPlayer->_MsgBoxMgr->GetMessageAck( false, ConfirmTradeBox );
				MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo(ConfirmTradeBox);
			}
			else if (MyTradeState != etsFree && MyTradeState != etsSuccess){
				CancelP2PTrade();
			}
		}

		//Unlock confirm box
		if (ConfirmUnlockBox != NULL){
			MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonOk(ConfirmUnlockBox);
		}

		//A money input box is still open
		if (InputMnyBox != NULL && IsInputingMny){
			//mMsgBoxPlayer->_MsgBoxMgr->GetMessageAck( false, InputMnyBox );
			MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo(InputMnyBox);

			InputMnyBox = NULL;
		}
	}
	//
	ResetData();
}

void UMMOUITradeP2P::UpdateCondition()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	AMMOPlayerController *pCtrlMe = mmoFacade.GetMainController();
	//if( pCtrlMe == NULL || Player == NULL || MyTradeState == etsFree || AlreadyCanceled || IsBeApplyed ){
	if (pCtrlMe == NULL || Player == NULL || pCtrlMe->TradeTargetID == 0 || AlreadyCanceled || IsBeApplyed){
		return;
	}

	AMMOPlayer *pChara = (AMMOPlayer*)mmoFacade.GetCharacter(pCtrlMe->TradeTargetID);
	if (pChara == NULL){
		//CMessageHandler::GetInstance().Send_TradeCancel();
		//AlreadyCanceled = TRUE;
		return;
	}

	FLOAT disSqu = Player->Location.DistanceSquared(pChara->Location);
	if (disSqu >= MaxTradeDistanceSqu){
		CMessageHandler::GetInstance().Send_TradeCancel();
		AlreadyCanceled = TRUE;
	}
}
//
UBOOL UMMOUITradeP2P::OnLeftMousePress(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault)
{
	if (bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUITradeP2P::OnLeftMouseClick(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault)
{
	if (bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUITradeP2P::OnRightMouseClick(class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault)
{
	if (bDefault)return FALSE;

	//if locked operation is not allowed
	if (IsMyLocked){
		return TRUE;
	}
	//
	if (SlotType == ESPType_TradeP2P){
		if (SlotID >= 0 && SlotID < UCONST_TRADE_ITEM_MAX && MyTradeItems[SlotID].hasData){
			//CMessageHandler::GetInstance().Send_TradeSetItem( SlotID, FALSE, MyTradeItems[SlotID].Data.FromContainID, MyTradeItems[SlotID].Data.FromSlotID );
			FString errMsg = TEXT("MMOGame.TradeP2P.ERROR30");
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(errMsg), MsgInfoType_Table, errMsg);
		}
		return TRUE;
	}
	else if (SlotType == ESPType_BagResource)
	{
		FString errMsg = TEXT("MMOGame.TradeP2P.CanotBagResource");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errMsg ), MsgInfoType_Table, errMsg );
		return TRUE;
	}
	else if( SlotType == ESPType_Bag || SlotType == ESPType_Warehouse /*|| SlotType == ESPType_BagResource*/)
	{
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem( ContainerID, SlotID );
		if( itemInfo ){

			UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemInfo->m_nTemplateID);

			if (item)
			{
				//Whether special item
				if (item->TradeNeedIntimacy)
				{
					if (FriendManager::getSingleton().CheckIntimacy(ctrl->TradeTargetID) == FALSE)
					{
						MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.NeedIntimacy"), MsgInfoType_Warning, TEXT("MMOGame.NeedIntimacy"));
						return TRUE;
					}
				}

				INT putInSlotID = -1;
				for (INT i = 0; i < UCONST_TRADE_ITEM_MAX; ++i){
					if (!MyTradeItems[i].hasData){
						putInSlotID = i;
						break;
					}
				}
				//if too many item to trade
				if (putInSlotID == -1){
					MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR20"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR20"));
					return TRUE;
				}
				//
				UBOOL isAdded = NotifyAddTradeItem(putInSlotID, ContainerID, SlotID);
				if (isAdded){
					CMessageHandler::GetInstance().Send_TradeSetItem(putInSlotID, TRUE, ContainerID, SlotID);
				}
			}

		}
		return TRUE;
	}

	return FALSE;
}
//
//
UBOOL UMMOUITradeP2P::OnDragDrop(class UGFxObject* SrcSlotMC, class UGFxObject* DestSlotMC,
	BYTE SrcSlotType, BYTE SrcContainerID, INT SrcSlotID,
	BYTE DestSlotType, BYTE DestContainerID, INT DestSlotID)
{
	//if locked operation is not allowed
	if (IsMyLocked){
		return TRUE;
	}
	//
	if( DestSlotType == ESPType_TradeP2P && (SrcSlotType == ESPType_Bag  || SrcSlotType == ESPType_BagResource)){
		//if( DestContainerID == CONSLOT_P2P_MY ){
		//	//from other container to trade
		//	UBOOL isAdded = NotifyAddTradeItem( DestSlotID, SrcContainerID, SrcSlotID );
		//	if( isAdded ){
		//		//send message
		//		CMessageHandler::GetInstance().Send_TradeSetItem( DestSlotID, TRUE, SrcContainerID, SrcSlotID );
		//	}
		//}
		//else{
		//	//not allow put in other's container
		//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.TradeP2P.ERROR23") ,MsgInfoType_Table,TEXT("MMOGame.TradeP2P.ERROR23"));
		//}
		//Cannot drag trade items
		if (MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID)){
			FString errMsg = TEXT("MMOGame.TradeP2P.ERROR31");
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(errMsg), MsgInfoType_Table, errMsg);
		}

		return TRUE;
	}
	else if( ( SrcSlotType == ESPType_TradeP2P && SrcContainerID == CONSLOT_P2P_MY &&(DestSlotType == ESPType_Bag || SrcSlotType == ESPType_BagResource) ) )
			//|| ( SrcSlotType == ESPType_TradeP2P && DestSlotType == ESPType_Warehouse ) )
	{
		//move items from trade
		if (MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID)){
			//CMessageHandler::GetInstance().Send_TradeSetItem( SrcSlotID, FALSE, DestContainerID, DestSlotID);
			FString errMsg = TEXT("MMOGame.TradeP2P.ERROR31");
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(errMsg), MsgInfoType_Table, errMsg);
		}
		return TRUE;
	}

	return FALSE;
}
//
UBOOL UMMOUITradeP2P::NotifyAddTradeItem(INT putInSlotID, INT fromContainID, INT fromSlotID)
{
	UItem *pItem = NULL;
	pItem = MMOFacade::getSingleton().FindItemTemplate(fromContainID, fromSlotID);
	if (pItem == NULL){
		return FALSE;
	}

	//if putInSlotID is valid
	if (putInSlotID >= UCONST_TRADE_ITEM_MAX){
		//trade too mang items
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR20"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR20"));
		return FALSE;
	}
	else if (putInSlotID < 0){
		return FALSE;
	}

	//
	FP2PTradeItem *pTradeItem = NULL;
	pTradeItem = &MyTradeItems[putInSlotID];
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem(fromContainID, fromSlotID);

	//it the item to put in is already in
	if (CheckInTrade(fromContainID, fromSlotID)){
		return FALSE;
	}
	//if the destination is occupied
	if (pTradeItem->hasData){
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR22"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR22"));
		return FALSE;
		//swap two
		//mark the old item
		//PendingDelContainID = pTradeItem->Data.FromContainID;
		//PendingDelSlotID = pTradeItem->Data.FromSlotID;
		////delete the old first
		//CMessageHandler::GetInstance().Send_TradeSetItem( putInSlotID, FALSE, PendingDelContainID, PendingDelSlotID );
	}
	//check can be trade
	ClientActionCondition_TradeItem checker;

	FClientActionConditionContex contex;
	contex.Item = pItemInfo;
	contex.ItemTemplate = pItem;

	if (!checker.CheckClientActionCondition(contex)){
		return FALSE;
	}
	//
	pTradeItem->Data.ItemID = pItem->ItemId;
	pTradeItem->Data.FromContainID = fromContainID;
	pTradeItem->Data.FromSlotID = fromSlotID;
	pTradeItem->hasData = TRUE;
	//lock original item
	UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel("Bag");
	if (pBagPanel != NULL && fromContainID == E_CONSLOT_DEFAULT){
		MMOFacade::getSingleton().LockItem(fromContainID, fromSlotID, ESPType_TradeP2P);
	}

	return TRUE;
}
//
UBOOL UMMOUITradeP2P::RealAddTradeItem(INT containID, INT slotID, INT itemID)
{
	//
	FP2PTradeItem *pTradeItem = NULL;
	if (containID == CONSLOT_P2P_MY){
		pTradeItem = &MyTradeItems[slotID];
	}
	else if (containID == CONSLOT_P2P_TG){
		pTradeItem = &TgTradeItems[slotID];
	}

	if (!pTradeItem)
	{
		return FALSE;
	}

	//
	UItem *pItem = NULL;
	pItem = MMOFacade::getSingleton().FindItemTemplate(itemID);
	if (pItem == NULL){
		pTradeItem->hasData = FALSE;
		if (containID == CONSLOT_P2P_MY){
			UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel("Bag");
			if (pBagPanel != NULL && MyTradeItems[slotID].Data.FromContainID == E_CONSLOT_DEFAULT){
				MMOFacade::getSingleton().LockItem(MyTradeItems[slotID].Data.FromContainID, MyTradeItems[slotID].Data.FromSlotID, -1);
			}
		}
		return FALSE;
	}
	//
	pTradeItem->hasData = TRUE;
	//
	//MMOFacade::getSingleton().AddItem( itemInfo );
	UGFxObject *pGfxObj = GetTradeSlot(containID, slotID);
	GFXPlayer->SetItemSlotData(pGfxObj, pItem, ESPType_TradeP2P, containID, slotID);
	//
	FString nameItem = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pItem->ItemColor));
	nameItem += pItem->GetDisplayName();
	nameItem += UITextParser::HTMLEndFont();
	pGfxObj = GetTradeSlotName(containID, slotID);
	if (pGfxObj != NULL){
		pGfxObj->SetString(TEXT("htmlText"), nameItem);
	}

	return TRUE;
}
//
UBOOL UMMOUITradeP2P::RealDelTradeItem(UBOOL isMy, INT putOutSlotID)
{
	FP2PTradeItem *pTradeItem = NULL;
	BYTE ContainerType;
	//
	if (putOutSlotID < 0 || putOutSlotID >= UCONST_TRADE_ITEM_MAX){
		return FALSE;
	}
	if (isMy){
		pTradeItem = &MyTradeItems[putOutSlotID];
		ContainerType = CONSLOT_P2P_MY;
	}
	else{
		pTradeItem = &TgTradeItems[putOutSlotID];
		ContainerType = CONSLOT_P2P_TG;
	}
	//if nothing in there
	if (!pTradeItem->hasData){
		return TRUE;
	}
	//unlock original item
	UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel("Bag");
	if (pBagPanel != NULL && pTradeItem->Data.FromContainID == E_CONSLOT_DEFAULT){
		//if( PendingDelContainID < 0 || PendingDelSlotID < 0 ){
		MMOFacade::getSingleton().LockItem(pTradeItem->Data.FromContainID, pTradeItem->Data.FromSlotID, -1);
		//}
		//else{
		//	MMOFacade::getSingleton().LockItem( PendingDelContainID, PendingDelSlotID, FALSE );
		//	PendingDelContainID = -1;
		//	PendingDelSlotID = -1;
		//}
	}
	//
	pTradeItem->hasData = FALSE;
	pTradeItem->Data.ItemID = -1;
	pTradeItem->Data.FromContainID = -1;
	pTradeItem->Data.FromSlotID = -1;
	//
	MMOFacade::getSingleton().DeleteItem(ContainerType, putOutSlotID);
	UGFxObject *pGfxObj = GetTradeSlot(ContainerType, putOutSlotID);
	GFXPlayer->SetItemSlotData(pGfxObj, NULL, ESPType_TradeP2P, ContainerType, putOutSlotID);
	//
	pTradeItem->Ui.Name->SetString(TEXT("htmlText"), TEXT(""));

	return TRUE;
}
//
void UMMOUITradeP2P::InputP2PTradeMoney()
{
	if (MyTradeMny > 0){
		//Restriction, can only input once
		FString errMsg = TEXT("MMOGame.TradeP2P.ERROR29");
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(errMsg), MsgInfoType_Table, errMsg);
		return;
	}
	//input box is already opend, or is locked
	if (IsInputingMny){
		return;
	}

	if (IsMyLocked){
		FString errMsg = TEXT("MMOGame.TradeP2P.ERROR24");
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(errMsg), MsgInfoType_Table, errMsg);
		return;
	}
	//
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("OnMyTradeMoneyChanged");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("OnMyTradeMoneyInputCanceled");
	//popup input message
	InputMnyBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox(TEXT(""), 0, MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeMsg0")), funYes, funNo, eInputFormatNumOnly);
	//UINT myMoney = Player->GetMoney(E_MONEYTYPE_02);
	INT bitCnt = 20;
	//myMoney += 9;
	//while( myMoney >= 10 )
	//{
	//	myMoney /= 10;
	//	++bitCnt;
	//}
	MMOMsgBoxManager::getSingleton().RestrictInput(InputMnyBox, eInputFormatNumOnly, bitCnt);
	//
	IsInputingMny = TRUE;
}
//
void UMMOUITradeP2P::OnMyTradeMoneyChanged(UGFxObject *mc)
{
	//now can open the money input again
	IsInputingMny = FALSE;
	InputMnyBox = NULL;
	//can NOt do this when in lock
	if (IsMyLocked){
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR24"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR24"));
		return;
	}
	//
	if (IsVisible == FALSE){
		return;
	}
	//
	FString inputMny = mc->GetObject("textInput")->GetString("text");
	INT64 tradeMoney = (INT64)appAtoi(*inputMny);
	if (tradeMoney < 0){
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR26"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR26"));
		return;
	}
	if (tradeMoney >= 4294967295){
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR27"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR27"));
		return;
	}

	UINT64 intTemp = (UINT64)tradeMoney * TRADE_TAX + 9999;
	UINT32 tex = (UINT32)(intTemp / 10000);
	INT64 nowMny = MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02);
	if (nowMny < 0){
		warnf(NAME_Error, TEXT("[UMMOUITradeP2P::OnMyTradeMoney my money is minus zero]"));
		nowMny = 0;
	}

	if (tradeMoney + tex > nowMny){
		//not enough money
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.ERROR25"), MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR25"));
		//reduce the money
		tradeMoney = appFloor((nowMny * 10000) / (10000 + TRADE_TAX));
		//tex = nowMny - tradeMoney;
	}
	//send message
	CMessageHandler::GetInstance().Send_TradeSetMoney(tradeMoney);
	////set ui
	//NotifyTradeMoneyChanged( TRUE, appAtoi( *inputMny ) );
}
void UMMOUITradeP2P::OnMyTradeMoneyInputCanceled(UGFxObject *mc)
{
	IsInputingMny = FALSE;
	InputMnyBox = NULL;
	//
	if (IsVisible == FALSE){
		return;
	}
}
//
void UMMOUITradeP2P::NotifyTradeMoneyChanged(UBOOL isMy, INT newMoney)
{
	if (isMy){
		if (newMoney < 0){
			warnf(NAME_Error, TEXT("======!!!!========= [UMMOUITradeP2P]notify Traded money maybe to large"));
			newMoney = 0;
		}
		MyTradeMny = newMoney;

		FString moneyStr;
		moneyStr = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, MyTradeMny);
		MyInputMnyBtn->SetString(TEXT("label"), moneyStr);
		//tex
		UINT64 intTemp = (UINT64)MyTradeMny * TRADE_TAX + 9999;
		UINT64 tex = (UINT64)(intTemp / 10000);//appCeil( (FLOAT)MyTradeMny * TradeTexRate );

		FString moneyColor;
		INT64 curMny = MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02);
		if ((UINT64)(tex + MyTradeMny) <= (UINT64)curMny){
			moneyColor = TEXT("0xFFFFE0A6");
		}
		else{
			moneyColor = UITextParser::GetTipsColor(TEXT("AlertColor"));
		}
		moneyStr = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, tex, moneyColor);

		TradeTexTf->SetString(TEXT("htmlText"), moneyStr);
	}
	else{
		TgTradeMny = newMoney;

		FString moneyStr;
		moneyStr = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, TgTradeMny);
		TgInputMnyBtn->SetString(TEXT("label"), moneyStr);
	}
}
//isFailed of 0 means success
void UMMOUITradeP2P::NotifyTradeResult(UBOOL isFailed)
{
	FString hintStr;
	FString strString;
	if (isFailed == FALSE){
		hintStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeSucceed"));
		strString = TEXT("MMOGame.TradeP2P.TradeSucceed");
		//hint tex rate
		if (MyTradeMny > 0){
			FString rateHint = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeSucceedRate"));
			INT tex = (INT32)(((INT64)MyTradeMny * TRADE_TAX + 9999) / 10000);
			FString texStr;
			texStr = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, tex);

			//FScriptDelegate funNone(EC_EventParm);
			//MMOUI::GetUIManager()->delegateAddModalMessageBox( rateHint, 0, funNone, funNone );
			MMOChatManager::getSingleton().AddSystemMessage(FString::Printf(*rateHint, *texStr));
		}

		//
		MyTradeState = etsSuccess;
		//
	}
	else if (isFailed == TRUE){
		strString = TEXT("MMOGame.TradeP2P.TradeFailed0");
		hintStr = MMOUI::ParseLocalizedPropertyPath(strString);
		//
		MMOFacade &mmoFacade = MMOFacade::getSingleton();
		AMMOPlayerController *pCtrlMe = mmoFacade.GetMainController();
		if (pCtrlMe == NULL){
			return;
		}

		//
		if (MyTradeState == etsAsking){
		}
		else if (MyTradeState == etsBeingAsked){
			//Remove prompt button
			if (TradeNotificationMsgButtonIdx >= 0){
				//MMOMsgButtonManager::getSingleton().RemoveButton( TradeNotificationMsgButtonIdx );
				MMOMsgButtonManager::getSingleton().RemoveOneTypeButtons(E_MsgBtnShow_Trade);
				TradeNotificationMsgButtonIdx = -1;
			}

			//If currently confirming to accept the trade
			AlreadyCanceled = TRUE;
		}

		//
		CheckError(pCtrlMe->TradeTargetID, TRUE);
		//
		MyTradeState = etsFree;
	}

	//hint screen
	MMOUI::GetUIManager()->delegateAddScreenMessage(hintStr, MsgInfoType_Table, strString);
	//unlock all trade items
	for (INT i = 0; i < UCONST_TRADE_ITEM_MAX; ++i){
		if (!MyTradeItems[i].hasData){
			continue;
		}
		//if( MyTradeItems[i].Data.FromContainID == E_CONSLOT_DEFAULT ){
		MMOFacade::getSingleton().LockItem(MyTradeItems[i].Data.FromContainID, MyTradeItems[i].Data.FromSlotID, -1);
		//}
	}
	//hide ui
	//Show( FALSE );
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TradeP2P, FALSE, TEXT(""));
}
//
void UMMOUITradeP2P::ConfirmP2PTrade()
{
	//Re-confirm trade items
	//send message
	CMessageHandler::GetInstance().Send_TradeConfirm();
	//forbiden Unlock
	BtnLockTradeBtn->SetBool(TEXT("enabled"), false);
	//forbiden confirm btn
	ConfirmTradeBtn->SetBool(TEXT("enabled"), false);
}
//
void UMMOUITradeP2P::CancelP2PTrade()
{
	//send message
	CMessageHandler::GetInstance().Send_TradeCancel();
}
//
void UMMOUITradeP2P::NotifyLockP2PTrade(UBOOL isMy, UBOOL isLocked)
{
	//if anybody unlock when both in locked state, then unlock all
	if (IsMyLocked == TRUE && IsTgLocked == TRUE && isLocked == FALSE){
		FString btnStr = MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.TradeLockBtnStr");
		BtnLockTradeBtn->SetString(TEXT("label"), btnStr);
		BtnLockTradeBtn->SetBool(TEXT("enabled"), true);

		IsMyLocked = FALSE;
		MyTradeLockMc->SetVisible(IsMyLocked);

		SetIsTgLocked(FALSE);

		ConfirmTradeBtn->SetBool(TEXT("enabled"), false);

		//hint
		FString hint = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeMsgStateAllUnLock"));
		MMOChatManager::getSingleton().AddSystemMessage(hint);
		MMOUI::GetUIManager()->delegateAddScreenMessage(hint, MsgInfoType_Table, TEXT("MMOGame.TradeP2P.TradeMsgStateAllUnLock"));

		return;
	}
	//
	if (isMy){
		IsMyLocked = isLocked;
		//
		MyTradeLockMc->SetVisible(IsMyLocked);
		//
		FString btnStr;
		FString hint;
		FString strString;
		if (IsMyLocked){
			//Self locks trade
			btnStr = MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.TradeUnlockBtnStr");
			hint = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeMsgStateMyLock"));
			strString = TEXT("MMOGame.TradeP2P.TradeMsgStateMyLock");
		}
		else{
			//Self unlocks trade
			btnStr = MMOUI::ParseLocalizedPropertyPath("MMOGame.TradeP2P.TradeLockBtnStr");
			hint = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeMsgStateAllUnLock"));
			strString = TEXT("MMOGame.TradeP2P.TradeMsgStateAllUnLock");
		}
		BtnLockTradeBtn->SetString(TEXT("label"), btnStr);
		if (etsConfirmed == MyTradeState){
			BtnLockTradeBtn->SetBool(TEXT("enabled"), TRUE);
		}
		//hint
		MMOChatManager::getSingleton().AddSystemMessage(hint);
		MMOUI::GetUIManager()->delegateAddScreenMessage(hint, MsgInfoType_Table, strString);
	}
	else{
		SetIsTgLocked(isLocked);

		//hint
		FString hint;
		FString strString;
		if (IsTgLocked){
			//Other party locks trade
			strString = TEXT("MMOGame.TradeP2P.TradeMsgStateOtherLock");
			hint = MMOUI::ParseLocalizedPropertyPath(strString);
		}
		else{
			//Other party unlocks trade
			strString = TEXT("MMOGame.TradeP2P.TradeMsgStateOtherUnLock");
			hint = MMOUI::ParseLocalizedPropertyPath(strString);
		}

		MMOChatManager::getSingleton().AddSystemMessage(hint);
		MMOUI::GetUIManager()->delegateAddScreenMessage(hint, MsgInfoType_Table, strString);
	}
	//check if confirm trade can be click
	bool enableBtn = (CheckCanConfirm() == TRUE);
	ConfirmTradeBtn->SetBool(TEXT("enabled"), enableBtn);
}
//
void UMMOUITradeP2P::NotifyTargetTradeLockState(UBOOL isLocked)
{
	SetIsTgLocked(isLocked);

	//check if confirm trade can be click
	bool enableBtn = (CheckCanConfirm() == TRUE);
	ConfirmTradeBtn->SetBool(TEXT("enabled"), enableBtn);
}
//==============================================================================================================
void UMMOUITradeP2P::ReplyUnlockTradeContinue(UGFxObject *mc)
{
	//del confirm box
	ConfirmUnlockBox = NULL;
}
//==============================================================================================================
void UMMOUITradeP2P::ReplyUnlockTradeCancel(UGFxObject *mc)
{
	//del confirm box
	ConfirmUnlockBox = NULL;

	//
	CMessageHandler::GetInstance().Send_TradeCancel();
}
//
UBOOL UMMOUITradeP2P::CheckCanConfirm()
{
	UBOOL canConfirm = IsMyLocked && IsTgLocked ? TRUE : FALSE;
	return canConfirm;
}
//
void UMMOUITradeP2P::LockP2PTrade()
{
	//send message
	if (!IsMyLocked){
		//lock it
		CMessageHandler::GetInstance().Send_TradeLock();
	}
	else{
		//unlock it
		CMessageHandler::GetInstance().Send_TradeUnLock();
	}

}
//
UBOOL UMMOUITradeP2P::CheckError(INT roleID, UBOOL needDisplayError)
{
	UBOOL isOK = TRUE;
	FString errorStr;
	FString strString;

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	AMMOPlayer *pTarget = (AMMOPlayer*)mmoFacade.GetCharacter(roleID);
	if (pTarget == NULL){
		strString = TEXT("MMOGame.TradeP2P.ERROR28");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);

		MMOUI::GetUIManager()->delegateAddScreenMessage(errorStr, MsgInfoType_Table, strString);
		MMOChatManager::getSingleton().AddSystemMessage(errorStr);

		return FALSE;
	}

	AMMOPlayer *pMe = mmoFacade.GetMainPlayer();
	AMMOPlayerController *pCtrlMe = mmoFacade.GetMainController();
	if (pCtrlMe == NULL || pMe == NULL){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUITradeP2P::CheckError] no ME"));
		return FALSE;
	}

	INT actorLevel = pMe->GetActorLevel();
	if (isOK && actorLevel < s_ActorLvAllowGameSystem){
		strString = TEXT("MMOErrorInfo.ERROR_CODE_DEF.actorLevelTooLow");
		errorStr = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(strString), s_ActorLvAllowGameSystem);
		isOK = FALSE;
	}

	//Left to the server to judge, client does not know the other party's level
	//actorLevel = pTarget->GetActorLevel();
	//if( isOK && actorLevel < s_ActorLvAllowGameSystem ){
	//	strString = TEXT("MMOGame.TradeP2P.errTargetLowLevel");
	//	errorStr = MMOUI::ParseLocalizedPropertyPath( strString );
	//	isOK = FALSE;
	//}

	FLOAT dis = pMe->Location.Distance(pTarget->Location);
	if (isOK && dis >= pCtrlMe->MaxInteractiveDistance){
		strString = TEXT("MMOGame.TradeP2P.ERROR0");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);
		isOK = FALSE;
	}
	if (isOK && (pTarget->eventIsDead() || pMe->eventIsDead())){
		strString = TEXT("MMOGame.TradeP2P.ERROR1");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);
		isOK = FALSE;
	}
	if (isOK && (pTarget->eventIsStallState() || pMe->eventIsStallState())){
		strString = TEXT("MMOGame.TradeP2P.ERROR2");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);
		isOK = FALSE;
	}
	if (isOK && pMe->IsInFighting()){
		strString = TEXT("MMOGame.TradeP2P.ERROR3");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);
		isOK = FALSE;
	}
	if (isOK && pTarget->IsInFighting()){
		strString = TEXT("MMOGame.TradeP2P.ERROR4");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);
		//errorStr = MMOUI::ParseLocalizedPropertyPath( FString::Printf( TEXT("MMOGame.TradeP2P.ERROR4"), pTarget->GetActorName() );
		isOK = FALSE;
	}

	FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	if (isOK && mapInfo.ServerMapType == E_est_BattleField){
		strString = TEXT("MMOGame.TradeP2P.ERROR6");
		errorStr = MMOUI::ParseLocalizedPropertyPath(strString);
		isOK = FALSE;
	}

	if (needDisplayError && !errorStr.IsEmpty()){
		MMOUI::GetUIManager()->delegateAddScreenMessage(errorStr, MsgInfoType_Table, strString);
		MMOChatManager::getSingleton().AddSystemMessage(errorStr);
	}
	return isOK;
}
//
//Whether this item is being traded
//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( "MMOGame.TradeP2P.ERROR21") );
UBOOL UMMOUITradeP2P::CheckInTrade(INT containID, INT slotID)
{
	UBOOL isInTrade;
	isInTrade = FALSE;
	if (!IsVisible){
		return isInTrade;
	}
	//
	for (INT i = 0; i < UCONST_TRADE_ITEM_MAX; ++i){
		if (!MyTradeItems[i].hasData){
			continue;
		}
		if (MyTradeItems[i].Data.FromContainID == containID && MyTradeItems[i].Data.FromSlotID == slotID){
			isInTrade = TRUE;
			break;
		}
	}
	return isInTrade;
}
//
void UMMOUITradeP2P::ResetShow()
{
	AMMOPlayerController* plyerCtrl = MMOFacade::getSingleton().GetMainController();
	//player name
	MyNameTf->SetText( GetPlayerRoleName() );
	//items
	UGFxObject *pGfxObj = NULL;
	for (INT i = 0; i < UCONST_TRADE_ITEM_MAX; ++i){
		MyTradeItems[i].Ui.Name->SetString(TEXT("htmlText"), TEXT(""));
		pGfxObj = GetTradeSlot(CONSLOT_P2P_MY, i);
		GFXPlayer->SetItemSlotData(pGfxObj, NULL, ESPType_TradeP2P, CONSLOT_P2P_MY, i);
		MyTradeItems[i].Ui.Mask->SetVisible(FALSE);
		MyTradeItems[i].Ui.Name->SetString(TEXT("htmlText"), TEXT(""));

		TgTradeItems[i].Ui.Name->SetString(TEXT("htmlText"), TEXT(""));
		pGfxObj = GetTradeSlot(CONSLOT_P2P_TG, i);
		GFXPlayer->SetItemSlotData(pGfxObj, NULL, ESPType_TradeP2P, CONSLOT_P2P_TG, i);
		TgTradeItems[i].Ui.Name->SetString(TEXT("htmlText"), TEXT(""));
		//TgTradeItems[i].Ui.Mask->SetVisible( FALSE );
	}
	//tex
	INT64 tex = ((INT64)MyTradeMny * TRADE_TAX + 9999) / 10000;//appCeil( (FLOAT)MyTradeMny * TradeTexRate );

	FString moneyStr;
	FString moneyColor;
	INT64 curMyMoney = MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02);
	if (tex + MyTradeMny <= curMyMoney){
		moneyColor = TEXT("0xFFFFE0A6");
	}
	else{
		moneyColor = UITextParser::GetTipsColor(TEXT("AlertColor"));
	}
	moneyStr = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, tex, moneyColor);

	TradeTexTf->SetString(TEXT("htmlText"), moneyStr);

	FString textStr;
	textStr = FString::Printf(TEXT("%d%%"), (INT)(TRADE_TAX / 100));
	TradeTexRateTf->SetText(textStr);
	//money
	moneyStr = UITextParser::HTMLMoneyIconString(MONEYTYPE_02, 0);
	MyInputMnyBtn->SetString(TEXT("label"), moneyStr);
	TgInputMnyBtn->SetString(TEXT("label"), moneyStr);
	//lock button
	textStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeLockBtnStr"));
	BtnLockTradeBtn->SetString(TEXT("label"), textStr);
	BtnLockTradeBtn->SetBool(TEXT("enabled"), true);
	//trade mask
	MyTradeLockMc->SetVisible(FALSE);
	TgTradeLockMc->SetVisible(FALSE);
	//confirm button
	ConfirmTradeBtn->SetBool(TEXT("enabled"), false);
}
//
void UMMOUITradeP2P::ResetData()
{
	for (INT i = 0; i < UCONST_TRADE_ITEM_MAX; ++i){
		if (MyTradeItems[i].hasData){
			MMOFacade::getSingleton().LockItem(MyTradeItems[i].Data.FromContainID, MyTradeItems[i].Data.FromSlotID, -1);
		}
		MyTradeItems[i].hasData = false;
		MyTradeItems[i].Data.ItemID = -1;
		MyTradeItems[i].Data.FromContainID = -1;
		MyTradeItems[i].Data.FromSlotID = -1;
		MMOFacade::getSingleton().DeleteItem(CONSLOT_P2P_MY, i);

		TgTradeItems[i].hasData = false;
		TgTradeItems[i].Data.ItemID = -1;
		TgTradeItems[i].Data.FromContainID = -1;
		TgTradeItems[i].Data.FromSlotID = -1;
		MMOFacade::getSingleton().DeleteItem(CONSLOT_P2P_TG, i);
	}
	//
	MyTradeMny = 0;
	TgTradeMny = 0;
	//
	IsMyLocked = FALSE;
	IsTgLocked = FALSE;
	IsInputingMny = FALSE;
}
//
void UMMOUITradeP2P::ApplyDoTrade(INT roleID)
{
	UBOOL isOK = CheckError(roleID, TRUE);
	//if( MyTradeState != etsFree ){
	if (Player->IsInTrading()){
		FString errorStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.ERROR8"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(errorStr, MsgInfoType_Table, TEXT("MMOGame.TradeP2P.ERROR8"));
		MMOChatManager::getSingleton().AddSystemMessage(errorStr);

		isOK &= FALSE;
	}

	if (!isOK){
		return;
	}

	//Cancel previous trade partner
	if (ctrl->TradeTargetID != 0 /*&& ctrl->TradeTargetID != roleID*/){
		CMessageHandler::GetInstance().Send_TradeCancel();
	}
	//
	CMessageHandler::GetInstance().Send_RequistTrade(roleID);
	//hint in chat channel
	//AMMOPlayer *pChara = (AMMOPlayer*)MMOFacade::getSingleton().GetCharacter( roleID );
	//if( pChara != NULL ){
	//	FString hint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TradeP2P.TradeMsg2") ), *pChara->GetActorName() );
	//	MMOChatManager::getSingleton().AddSystemMessage( hint );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table, TEXT("MMOGame.TradeP2P.TradeMsg2"));
	//}

	IsBeApplyed = FALSE;
	TradeNotificationMsgButtonIdx = -1;

	AlreadyCanceled = FALSE;
}
//receiv apply for do deal
void UMMOUITradeP2P::NotifyApplyDoTrade(INT roleID)
{
	AMMOPlayer *pTarget = (AMMOPlayer*)MMOFacade::getSingleton().GetCharacter(roleID);
	if (pTarget == NULL){
		return;
	}
	//hint in chat channel
	FString hint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TradeP2P.TradeMsg3") ), *pTarget->GetActorName() );
	MMOChatManager::getSingleton().AddSystemMessage(hint);
	// and on screen
	MMOUI::GetUIManager()->delegateAddScreenMessage(hint, MsgInfoType_Table, TEXT("MMOGame.TradeP2P.TradeMsg3"));
	//
	FString desc = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeMsg1"));
	//


	FString targetName;
	targetName = pTarget->GetActorName().IsEmpty() ? pTarget->ActorName : pTarget->GetActorName();

	TradeNotificationMsgButtonIdx = MMOMsgButtonManager::getSingleton().AddNewButton(FString::Printf(*desc, *targetName), roleID, targetName, GameEvent_InviteTrade, E_MsgBtnShow_Trade);

	IsBeApplyed = TRUE;
	AlreadyCanceled = FALSE;
}
//
void UMMOUITradeP2P::ReplyAgreeTrade(UGFxObject *mc)
{
	int roleID;
	if (mc == NULL || ctrl == NULL){
		return;
	}
	////
	////if( MyTradeState == etsFree ){
	//if( !Player->IsInTrading() ){
	//	FString errorHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TradeP2P.ERROR7") );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( errorHint ,MsgInfoType_Table,TEXT("MMOGame.TradeP2P.ERROR7"));
	//	MMOChatManager::getSingleton().AddSystemMessage( errorHint );
	//	//
	//	CMessageHandler::GetInstance().Send_TradeCancel();

	//	return;
	//}

	//del confirm box
	ConfirmTradeBox = NULL;
	//
	TradeNotificationMsgButtonIdx = -1;

	//
	roleID = mc->GetInt("iteminfoid");
	//Trade canceled
	if (AlreadyCanceled){
		FString strName = TEXT("MMOGame.TradeP2P.ERROR7");
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(strName), MsgInfoType_Table, strName);
		return;
	}

	UBOOL isSuccess = CheckError(roleID, FALSE);
	if (isSuccess){
		CMessageHandler::GetInstance().Send_AcceptTradeRequist(roleID);
		//
		Player->SetInTrading(TRUE);
		ctrl->TradeTargetID = roleID;
	}
	else{
		CMessageHandler::GetInstance().Send_TradeCancel();
	}
	//test
	//DoTrade(roleID);
}
//
void UMMOUITradeP2P::ReplyRefuseTrade(UGFxObject *mc)
{
	int roleID;
	if (mc == NULL){
		return;
	}

	////if( MyTradeState == etsFree ){
	//if( !Player->IsInTrading() ){
	//	FString errorHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.TradeP2P.ERROR7") );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( errorHint ,MsgInfoType_Table,TEXT("MMOGame.TradeP2P.ERROR7"));
	//	MMOChatManager::getSingleton().AddSystemMessage( errorHint );

	//	//
	//	CMessageHandler::GetInstance().Send_TradeCancel();

	//	return;
	//}

	//del confirm box
	ConfirmTradeBox = NULL;
	//
	TradeNotificationMsgButtonIdx = -1;

	//
	roleID = mc->GetInt("iteminfoid");
	//Trade canceled
	if (AlreadyCanceled){
		//FString strName = TEXT("MMOGame.TradeP2P.ERROR7");
		//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( strName ), MsgInfoType_Table, strName );
		return;
	}
	//
	CMessageHandler::GetInstance().Send_TradeCancel();


}
//
void UMMOUITradeP2P::DoTrade(INT roleID, INT roleLv)
{
	//ResetData();
	//
	TgPlayerID = roleID;
	//show target player name
	FString targetName;
	FString targetClass;
	INT targetSex = EGT_Male;
	INT targetFriendship = 0;
	AMMOActor *pTarget = MMOFacade::getSingleton().GetCharacter(roleID);
	if (pTarget != NULL){
		targetName = pTarget->GetActorName();
		//
		INT playerClass = pTarget->GetClassId();
		FString classNameStr = TEXT("MMOGame.School.SchoolName");
		classNameStr += appItoa(playerClass);
		targetClass = MMOUI::ParseLocalizedPropertyPath(classNameStr);
		targetSex = pTarget->GetSex();
		//
		targetFriendship = FriendManager::getSingleton().GetPlayerIntimacy(roleID);
	}
	eventCallAsSetTradeInfo(targetName, targetClass, targetSex, roleLv, targetFriendship);
	//
	if (TgNameTf){
		TgNameTf->SetText(targetName);
	}
	//set target
	if (ctrl != NULL){
		ctrl->SetSelectTarget(pTarget);
	}


	//MMOFacade::getSingleton().GetMainController()->SetDesiredTraceActor( pTarget );
	//
	eventOpen();
}
//
BYTE UMMOUITradeP2P::MyContainerType()
{
	return CONSLOT_P2P_MY;
}
BYTE UMMOUITradeP2P::TgContainerType()
{
	return CONSLOT_P2P_TG;
}

//
void UMMOUITradeP2P::Init()
{
	Super::Init();
	if (GFXPlayer){
		GFXPlayer->AddPostAdvanceDelegate(FScriptDelegate(this, TEXT("PostAdvance")));
	}

	InitMc();
	//
	MMOUI::GetUIManager()->SetApplyDoTradeDelegate(FScriptDelegate(this, TEXT("ApplyDoTrade")));

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_InviteTrade);
	evts.AddItem(GameEvent_ServerError);
	EventAdapter = new TGameEventAdapter<UMMOUITradeP2P>(this, evts);
}

void UMMOUITradeP2P::UnInit()
{
	Super::UnInit();
	if (GFXPlayer){
		GFXPlayer->RemovePostAdvanceDelegate(FScriptDelegate(EC_EventParm));
	}
}

void UMMOUITradeP2P::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch (evt->GetEventType())
	{

	case GameEvent_InviteTrade:
	{
		MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
		if (cb)
		{
			if (cb->Agree)
			{
				ReplyAgreeTrade(cb->Box);
			}
			else
			{
				ReplyRefuseTrade(cb->Box);
			}
		}
	}
		break;
	case GameEvent_ServerError:
	{
		MMOGameEvent_ServerError *errorEvt = (MMOGameEvent_ServerError*)evt;
		if (errorEvt != NULL && errorEvt->ErrCode == errTradeBagFull){
			AMMOPlayer *pChara = (AMMOPlayer*)MMOFacade::getSingleton().GetCharacter(errorEvt->Param1);
			FString errorKeyName;
			if (pChara != NULL && pChara->IsMainPlayer()){
				////Own bag is full, re-enable confirm trade
				//ConfirmTradeBtn->SetBool( TEXT("enabled"), TRUE );
				errorKeyName = TEXT("MMOGame.TradeP2P.errMybagIsFull");
				CancelP2PTrade();
			}
			else{
				errorKeyName = TEXT("MMOGame.TradeP2P.errOthersbagIsFull");
			}
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(errorKeyName), MsgInfoType_Warning, errorKeyName);
		}
	}
	}
}

//
void UMMOUITradeP2P::InitMc()
{
	Player = MMOFacade::getSingleton().GetMainPlayer();

	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.UITitleName"));
	asFunArgs.AddItem(asTitleName);
	this->Invoke(TEXT("SetWindowTitle"), asFunArgs);
	//GetObject( TEXT("MyWindowsFrame_mc") )->GetObject( TEXT("tF_windowTitle") )->SetText( asTitleName.S );

	MyTradeMc = GetObject(TEXT("myTrade_mc"));
	TgTradeMc = GetObject(TEXT("tgTrade_mc"));

	TradeTexTf = GetObject(TEXT("tf_tradeTex"));
	TradeTexRateTf = GetObject(TEXT("tf_tradeTexRate"));

	ConfirmTradeBtn = GetObject(TEXT("btnConfirmTrade"));
	BtnCancelTradeBtn = GetObject(TEXT("btnCancelTrade"));
	BtnLockTradeBtn = GetObject(TEXT("btnLockTrade"));
	// my trade
	MyNameTf = MyTradeMc->GetObject(TEXT("tf_playerName"));
	MyInputMnyBtn = MyTradeMc->GetObject(TEXT("btnInputMny"));
	MyTradeLockMc = MyTradeMc->GetObject(TEXT("tradeLock_mc"));
	// trade target
	TgNameTf = TgTradeMc->GetObject(TEXT("tf_playerName"));
	TgInputMnyBtn = TgTradeMc->GetObject(TEXT("btnInputMny"));
	TgTradeLockMc = TgTradeMc->GetObject(TEXT("tradeLock_mc"));

	for (INT i = 0; i < UCONST_TRADE_ITEM_MAX; ++i){
		FString obName = FString::Printf(TEXT("tradeItem%d"), i);
		MyTradeItems[i].Ui.ParentUI = MyTradeMc->GetObject(obName);
		MyTradeItems[i].Ui.Name = MyTradeItems[i].Ui.ParentUI->GetObject(TEXT("tf_itemText"));
		MyTradeItems[i].Ui.Mask = MyTradeItems[i].Ui.ParentUI->GetObject(TEXT("mask_mc"));
		MyTradeItems[i].Ui.Item = MyTradeItems[i].Ui.ParentUI->GetObject(TEXT("slot_mc"));
		MyTradeItems[i].Ui.ParentUI->SetInt(TEXT("containerID"), MyContainerType());
		MyTradeItems[i].Ui.ParentUI->SetInt(TEXT("slotID"), i);
		MyTradeItems[i].Ui.ParentUI->SetInt(TEXT("SlotType"), ESPType_TradeP2P);

		obName = FString::Printf(TEXT("tradeItem%d"), i);
		TgTradeItems[i].Ui.ParentUI = TgTradeMc->GetObject(obName);
		TgTradeItems[i].Ui.Name = TgTradeItems[i].Ui.ParentUI->GetObject(TEXT("tf_itemText"));
		TgTradeItems[i].Ui.Mask = TgTradeItems[i].Ui.ParentUI->GetObject(TEXT("mask_mc"));
		//TgTradeItems[i.Ui->Mask->SetVisible( false );
		TgTradeItems[i].Ui.Item = TgTradeItems[i].Ui.ParentUI->GetObject(TEXT("slot_mc"));
		//TgTradeItems[i]->Ui.Item->SetBool( "enabled", false );
		TgTradeItems[i].Ui.ParentUI->SetInt(TEXT("containerID"), TgContainerType());
		TgTradeItems[i].Ui.ParentUI->SetInt(TEXT("slotID"), i);
		TgTradeItems[i].Ui.ParentUI->SetInt(TEXT("SlotType"), ESPType_TradeP2P);
	}
}
//============================================================================================================
void UMMOUITradeP2P::NativeInitLoad()
{
	Super::NativeInitLoad();

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}

UGFxObject *UMMOUITradeP2P::GetTradeSlot(INT containID, INT slotID)
{
	UGFxObject *parentUI = NULL;
	FP2PTradeItem *pTradeItem = NULL;
	if (containID == CONSLOT_P2P_MY){
		pTradeItem = &MyTradeItems[slotID];
		parentUI = GetObject(TEXT("myTrade_mc"));
	}
	else if (containID == CONSLOT_P2P_TG){
		pTradeItem = &TgTradeItems[slotID];
		parentUI = GetObject(TEXT("tgTrade_mc"));
	}

	if (parentUI == NULL){
		return NULL;
	}

	FString obName = FString::Printf(TEXT("tradeItem%d"), slotID);
	pTradeItem->Ui.ParentUI = parentUI->GetObject(obName);
	if (pTradeItem->Ui.ParentUI == NULL){
		return NULL;
	}

	pTradeItem->Ui.Item = pTradeItem->Ui.ParentUI->GetObject(TEXT("slot_mc"));
	return pTradeItem->Ui.Item;
}

UGFxObject *UMMOUITradeP2P::GetTradeSlotName(INT containID, INT slotID)
{
	UGFxObject *parentUI = NULL;
	FP2PTradeItem *pTradeItem = NULL;
	if (containID == CONSLOT_P2P_MY){
		pTradeItem = &MyTradeItems[slotID];
		parentUI = GetObject(TEXT("myTrade_mc"));
	}
	else if (containID == CONSLOT_P2P_TG){
		pTradeItem = &TgTradeItems[slotID];
		parentUI = GetObject(TEXT("tgTrade_mc"));
	}

	if (parentUI == NULL){
		return NULL;
	}

	FString obName = FString::Printf(TEXT("tradeItem%d"), slotID);
	pTradeItem->Ui.ParentUI = parentUI->GetObject(obName);
	if (pTradeItem->Ui.ParentUI == NULL){
		return NULL;
	}

	pTradeItem->Ui.Name = pTradeItem->Ui.ParentUI->GetObject(TEXT("tf_itemText"));
	return pTradeItem->Ui.Name;
}
//==========================================================================
void UMMOUITradeP2P::SetIsTgLocked(UBOOL isToLock)
{
	if (IsTgLocked && !isToLock && ConfirmUnlockBox == NULL){
		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = this;
		funYes.FunctionName = TEXT("ReplyUnlockTradeContinue");

		FScriptDelegate funNo(EC_EventParm);
		funNo.Object = this;
		funNo.FunctionName = TEXT("ReplyUnlockTradeCancel");

		FString msgTxt = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeUnlockConfirm"));
		ConfirmUnlockBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(msgTxt, 0, funYes, funNo);

		if (ConfirmUnlockBox != NULL){
			UGFxObject* buttonMc = ConfirmUnlockBox->GetObject(TEXT("yesButton_mc"));
			if (buttonMc != NULL){
				buttonMc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeUnlockConfirmYesBtn")));
			}

			buttonMc = ConfirmUnlockBox->GetObject(TEXT("noButton_mc"));
			if (buttonMc != NULL){
				buttonMc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeP2P.TradeUnlockConfirmNoBtn")));
			}
		}
	}

	IsTgLocked = isToLock;
	TgTradeLockMc->SetVisible(IsTgLocked);
}
//===============================================================================================================
//===============================================================================================================
void UMMOUITradeP2P::PostAdvance(float DeltaTime)
{
	//Super::PostAdvance(DeltaTime);
	//
	UpdateCondition();
}
//===============================================================================================================
//===============================================================================================================