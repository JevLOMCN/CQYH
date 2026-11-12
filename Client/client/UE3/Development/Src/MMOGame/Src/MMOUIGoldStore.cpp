#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGoldStoreManager.h"
#include "MMOStats.h"
#include "MMOMsgBoxManager.h"
#include "MallManager.h"
#include "WebHtmlManager.h"
#include "MMOVipManager.h"
#include "exchange_def.h"

IMPLEMENT_CLASS(UMMOUIGoldStore)
#define FRESHTIME 2

void UMMOUIGoldStore::Init()
{
	Super::Init();
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_GoldStore_ListBuying);
	evts.AddItem(GameEvent_GoldStore_ListSelling);
	evts.AddItem(GameEvent_GoldStore_AddOrder);
	evts.AddItem(GameEvent_GoldStore_CancelSell);
	//evts.AddItem(GameEvent_GoldStore_ListBuying_My);
	//evts.AddItem(GameEvent_GoldStore_ListSelling_My);
	evts.AddItem(GameEvent_GoldStore_ListMyOrder);
	evts.AddItem(GameEvent_GoldStore_UpdatePlayerMoney);
	evts.AddItem(GameEvent_RealMoneyChange);
	evts.AddItem(GameEvent_GoldStore_UpdateMyOrderCount);
	evts.AddItem(GameEvent_GoldStore_UpdateMySellCount);
	evts.AddItem(GameEvent_GoldStore_SetNoticeInfo);
	evts.AddItem(GameEvent_GoldStore_UpdateMyLogs);
	evts.AddItem(GameEvent_GoldStore_DefaultPrice);
	EventAdapter = new TGameEventAdapter<UMMOUIGoldStore>(this, evts);
	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}
	bDefault = FALSE;
	bFresh = FALSE;
	freshTime=0;
}

void UMMOUIGoldStore::PreOnShow(UBOOL show)
{
	Super::PostOnShow(show);

}

void UMMOUIGoldStore::PostOnShow(UBOOL v)
{
	if (v)
	{
		if (MMOVipManager::getSingleton().IsLimitedGamePlayMode())
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFreeMode"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Warning,error);
			this->SetVisible(FALSE);
			return;
		}

		eventUCShowPageByIndex(0);
		FString NoticeInfo = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.NoticeInfo"));
		FString Explain = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.Explain"));
		FString record = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.Record"));
		FString orderExplan = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GoldStore.orderExplan"));
		eventUCSetNoticeInfo(NoticeInfo,Explain,record,orderExplan);
		//ShowDefaultPrice();
		bFresh = TRUE;
	}
	else
	{
		bFresh = FALSE;
		freshTime = 0;
	}
	NativeOnShow(v);
	Super::PostOnShow(v);

	bDefault = v;
}

void UMMOUIGoldStore::NativeOnShow(UBOOL v)
{
	if (v)
	{
		MMOGoldStoreManager::getSingleton().NotifyUpdatePlayerMoney();
		CMessageHandler::GetInstance().Send_MallAskRealMoney();
	}
}

void UMMOUIGoldStore::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}
void UMMOUIGoldStore::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	UGFxObject* obj = 0;
	switch(evt->GetEventType())
	{
	case GameEvent_GoldStore_ListBuying:
		{
			MMOGameEvent_GoldStore_DataUpdate* gsEvt = (MMOGameEvent_GoldStore_DataUpdate*)evt;
			obj = MMOGoldStoreManager::getSingleton().CreateGoldStoreItemListDataObject(GFXPlayer, gsEvt->GoldStoreListPtr);
			if (obj)
			{
				this->eventUCSetGoldStore_ListBuying(obj);
			}

			FristShowPrice();

		}
		break;
	case GameEvent_GoldStore_ListSelling:
		{
			
			MMOGameEvent_GoldStore_DataUpdate* gsEvt = (MMOGameEvent_GoldStore_DataUpdate*)evt;
			obj = MMOGoldStoreManager::getSingleton().CreateGoldStoreItemListDataObject(GFXPlayer, gsEvt->GoldStoreListPtr);
			if (obj)
			{
				this->eventUCSetGoldStore_ListSelling(obj);
			}

			FristShowPrice();
		}
		break;
	case GameEvent_GoldStore_AddOrder:
		{
			this->eventUCResetGoldStorePage();
			//ASFreshListView(1);
		}
		break;
	case GameEvent_GoldStore_CancelSell:
		{
			MMOGameEvent_GoldStore_DataUpdate_My* gsEvt = (MMOGameEvent_GoldStore_DataUpdate_My*)evt;
			obj = MMOGoldStoreManager::getSingleton().CreateMyGoldStoreItemListDataObject(GFXPlayer, *gsEvt->GoldStoreListPtr);
			if (obj)
			{
				this->eventUCSetGoldStore_CancelSell(obj);
			}
		}
		break;
	/*case GameEvent_GoldStore_ListBuying_My:
		{
			MMOGameEvent_GoldStore_DataUpdate_My* gsEvt = (MMOGameEvent_GoldStore_DataUpdate_My*)evt;
			obj = MMOGoldStoreManager::getSingleton().CreateMyGoldStoreItemListDataObject(GFXPlayer, *gsEvt->GoldStoreListPtr);
			if (obj)
			{
				this->eventUCSetGoldStore_ListBuying_My(obj);
			}
		}
		break;
	case GameEvent_GoldStore_ListSelling_My:
		{
			MMOGameEvent_GoldStore_DataUpdate_My* gsEvt = (MMOGameEvent_GoldStore_DataUpdate_My*)evt;
			obj = MMOGoldStoreManager::getSingleton().CreateMyGoldStoreItemListDataObject(GFXPlayer, *gsEvt->GoldStoreListPtr);
			if (obj)
			{
				this->eventUCSetGoldStore_ListSelling_My(obj);
			}
		}
		break;*/
	case GameEvent_GoldStore_ListMyOrder:
		{
			MMOGameEvent_GoldStore_DataUpdate_My* gsEvt = (MMOGameEvent_GoldStore_DataUpdate_My*)evt;
			obj = MMOGoldStoreManager::getSingleton().CreateMyGoldStoreItemListDataObject(GFXPlayer, *gsEvt->GoldStoreListPtr);
			if (obj)
			{
				this->eventCallUCSetGoldStore_OrderList(obj);
			}
		}
		break;
	case GameEvent_GoldStore_UpdatePlayerMoney:
		{
			MMOGameEvent_GoldStore_UpdateMoney* gsEvt = (MMOGameEvent_GoldStore_UpdateMoney*) evt;
			FString money = FString::Printf(TEXT("%I64d"),gsEvt->playerMoney);
			this->eventUCSetPlayerMoney(money);
		}
		break;
	case GameEvent_RealMoneyChange:
		{
			//MMOGameEvent_GoldStore_UpdateMoney* gsEvt = (MMOGameEvent_GoldStore_UpdateMoney*) evt;
			if (this->GetVisible())
			{
				this->eventUCSetPlayerRealMoney(MallManager::getSingleton().GetRealMoney());
			}
			
		}
		break;
	case GameEvent_GoldStore_UpdateMyOrderCount:
		{
			MMOGameEvent_GoldStore_UpdateOrderCount* gsEvt = (MMOGameEvent_GoldStore_UpdateOrderCount*) evt;
			this->eventUCSetMyOrderCount(gsEvt->moneyCount);
		}
		break;
	case GameEvent_GoldStore_UpdateMySellCount:
		{
			MMOGameEvent_GoldStore_UpdateOrderCount* gsEvt = (MMOGameEvent_GoldStore_UpdateOrderCount*) evt;
			this->eventUCSetMySellCount(gsEvt->sellMoneyCont);
		}
		break;
	case GameEvent_GoldStore_UpdateMyLogs:
			UpdateMyLogs();
		break;
	case GameEvent_GoldStore_DefaultPrice:
		{
			MMOGameEvent_GoldStore_DefaultPrice* gsEvt = (MMOGameEvent_GoldStore_DefaultPrice*) evt;
			eventCallUCSetDefaultPrice(gsEvt->price,gsEvt->count);
		}
		break;
	}
}

void  UMMOUIGoldStore::ASGoldStoreClickTabPage(int nIndex,int type)
{
	MMOGoldStoreManager::getSingleton().SetCurrentTabPage((EGoldTabType)nIndex);
	//ASFreshListView(type);
}

void UMMOUIGoldStore::ASFreshListView(int type)
{
	switch(MMOGoldStoreManager::getSingleton().GetCurrentTabPage())
	{
	case EGTT_buyView:
		{
			CMessageHandler::GetInstance().Send_ListPrice();
		}
		break;
	case EGTT_historyView:
		{
			CMessageHandler::GetInstance().Send_List_MyLogs();
		}
		break;
	case EGTT_myOrderView:
		{
			CMessageHandler::GetInstance().Send_List_MyOrder();
		}
		break;
	}
}

void UMMOUIGoldStore::ASGoldStoreBuy(class UGFxObject* dt)
{
	INT32 price = dt->GetInt(TEXT("buy_UnitPrice"));
	INT32 count = dt->GetInt(TEXT("buy_Number"));

	/*if(CheckGoldStoreOrder(price,count))
	{
	return;
	}*/
	HideGoldStore_Msg();

	if (count <= 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Exchange.Buy.ErrorCount")),MsgInfoType_Warning,TEXT("MMOGame.Exchange.Buy.ErrorCount"));
		return;
	}

	FScriptDelegate GoldStoreDelegate(EC_EventParm);
	GoldStoreDelegate.FunctionName = TEXT("OnGoldStoreMsg_Buy_Ok");
	GoldStoreDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnGoldStoreMsg_Buy_No");
	Nofunction.Object = this;

	float priceTemp =  price;
	FString str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.Buy_Confirm")) , priceTemp/100 , count );
	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox(str  , price , GoldStoreDelegate , Nofunction );

	if(ConfirmMsgBox)
		ConfirmMsgBox->SetInt( TEXT("itemdataid") , count);
	
}

void UMMOUIGoldStore::OnGoldStoreMsg_Buy_Ok(UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;

	INT32 price = mc->GetInt(TEXT("iteminfoid"));
	INT32 count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_AddOrder(eotBuy,MONEYTYPE_02,price,count);
}

void UMMOUIGoldStore::OnGoldStoreMsg_Buy_No(UGFxObject* mc)
{
	ConfirmMsgBox = NULL;
}

void UMMOUIGoldStore::ASGoldStoreSell(class UGFxObject* dt)
{
	INT32 price = dt->GetInt(TEXT("sell_UnitPrice"));
	INT32 count = dt->GetInt(TEXT("sell_Number"));

	HideGoldStore_Msg();

	if (count <= 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Exchange.Sell.ErrorCount")),MsgInfoType_Warning,TEXT("MMOGame.Exchange.Sell.ErrorCount"));
		return;
	}


	FScriptDelegate GoldStoreDelegate(EC_EventParm);
	GoldStoreDelegate.FunctionName = TEXT("OnGoldStoreMsg_Sell_Ok");
	GoldStoreDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnGoldStoreMsg_Sell_No");
	Nofunction.Object = this;

	float priceTemp = price ;
	FString str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.Sell_Confirm")) , priceTemp/ 100 , count );
	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox(str  , price , GoldStoreDelegate , Nofunction );

	if(ConfirmMsgBox)
	{
		ConfirmMsgBox->SetInt( TEXT("itemdataid") , count);
	}

	
}

void UMMOUIGoldStore::OnGoldStoreMsg_Sell_Ok(UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT32 price = mc->GetInt(TEXT("iteminfoid"));
	INT32 count = mc->GetInt(TEXT("itemdataid"));
	CMessageHandler::GetInstance().Send_AddOrder(eotSell,MONEYTYPE_02,price,count);
}
void UMMOUIGoldStore::OnGoldStoreMsg_Sell_No(UGFxObject* mc)
{
	ConfirmMsgBox = NULL;
}

void UMMOUIGoldStore::ASGoldStoreShowMyOrder(int type)
{
	MMOGoldStoreManager::getSingleton().SetMyCurrentOrderType(type);
	CMessageHandler::GetInstance().Send_List_MyOrder();
	MMOGoldStoreManager::getSingleton().SetIsUpdateOrderCount(FALSE);
}

void UMMOUIGoldStore::ASGoldStoreCancelSell(int id)
{
	HideGoldStore_Msg();

	FScriptDelegate GoldStoreDelegate(EC_EventParm);
	GoldStoreDelegate.FunctionName = TEXT("OnGoldStoreMsg_Cancel_Ok");
	GoldStoreDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnGoldStoreMsg_Cancel");
	Nofunction.Object = this;

	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Exchange.CancelOrder") )  , id , GoldStoreDelegate , Nofunction );

	
}

void UMMOUIGoldStore::OnGoldStoreMsg_Cancel_Ok(UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT index = mc->GetInt(TEXT("iteminfoid"));
	INT64 id = MMOGoldStoreManager::getSingleton().GetMyOrderIdByIndex(index);
	CMessageHandler::GetInstance().Send_CancelOrder(id);
}

void UMMOUIGoldStore::OnGoldStoreMsg_Cancel(UGFxObject* mc)
{
	ConfirmMsgBox = NULL;
}

void UMMOUIGoldStore::HideGoldStore_Msg()
{
	if( ConfirmMsgBox != NULL )
	{
		ConfirmMsgBox->SetVisible( FALSE );
		ConfirmMsgBox = NULL;
	}
}

void UMMOUIGoldStore::ASBuyMoney()
{
	WebHtmlManager::getSingleton().OpenRechargeYuanBao();
	//WebHtmlManager::getSingleton().OpenRechargeYuanBaoByIE();
	//MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.GoldAddHint")),MsgInfoType_Table,TEXT("MMOGame.Mall.GoldAddHint"));
}

void UMMOUIGoldStore::ASSoldMoney()
{

	NativeSoldMoney();

	//FScriptDelegate funYes(EC_EventParm);
	//funYes.Object = this;
	//funYes.FunctionName = TEXT("NativeSoldMoney");

	//FScriptDelegate funNo(EC_EventParm);
	//funNo.Object = this;

	//UGFxObject* MsgboxWindow = MMOMsgBoxManager::getSingleton().AddModalMessageBox(
	//	MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.YuanBao.Sell")), 0, funYes, funNo, TRUE);
	//if(MsgboxWindow)
	//{
	//	TArray<struct FASValue> args;
	//	FASValue align(EC_EventParm);
	//	align.Type = AS_String;
	//	align.S = TEXT("left");
	//	args.AddItem(align);

	//	FASValue sizeValue(EC_EventParm);
	//	sizeValue.Type = AS_Int;
	//	sizeValue.I = 14;
	//	args.AddItem(sizeValue);

	//	FASValue leading(EC_EventParm);
	//	leading.Type = AS_Null;
	//	args.AddItem(leading);

	//	MsgboxWindow->Invoke(TEXT("setContentTextFormat"),args);
	//}

	////MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.GoldAddHint")),MsgInfoType_Table,TEXT("MMOGame.Mall.GoldAddHint"));
}

void UMMOUIGoldStore::NativeSoldMoney()
{
	WebHtmlManager::getSingleton().OpenSellYuanBao();


	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("SoldMoneySuccess");

	FScriptDelegate funcNULL(EC_EventParm);

	UGFxObject* MsgboxWindow = MMOMsgBoxManager::getSingleton().AddModalMessageBox( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.YuanBao.BuyMsg")), 0, funYes, funcNULL, FALSE);
	if(MsgboxWindow)
	{
		UGFxObject* yesButton_mc = MsgboxWindow->GetObject(TEXT("yesButton_mc"));
		if(yesButton_mc)
		{
			//yesButton_mc->SetPosition(56.0f,100.0f);
			yesButton_mc->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.YuanBao.BuyBtn"))  );
		}
		//UGFxObject* noButton_mc = MsgboxWindow->GetObject(TEXT("noButton_mc"));
		//if(noButton_mc)
		//{
		//	noButton_mc->SetPosition(197.0f,100.0f);
		//	noButton_mc->SetString( TEXT("label") , TEXT("Having a problem")  );
		//}
	}
}

void UMMOUIGoldStore::SoldMoneySuccess()
{
	CMessageHandler::GetInstance().Send_MallAskRealMoney();
}




bool UMMOUIGoldStore::CheckGoldStoreOrder(INT money,INT count)
{
	bool v = false;

	if (money <= 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GoldStore.MoneyNotEnough")),MsgInfoType_Table,TEXT("MMOGame.GoldStore.MoneyNotEnough"));
		v = true;
	}
	else if(count <= 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GoldStore.CountNotEnough")),MsgInfoType_Table,TEXT("MMOGame.GoldStore.CountNotEnough"));
		v = true;
	}
	return v;
	
}

void  UMMOUIGoldStore::ASOnExchangeTypeChanged(int type)
{
	MMOGoldStoreManager::getSingleton().setCurrentBuyType(type);
	//ShowDefaultPrice();
}

void UMMOUIGoldStore::UpdateMyLogs()
{
	UGFxObject* datList = GFXPlayer->CreateArray();
	if(datList == NULL)
		return ;

	INT idx = 0;
	const GoldStoreMyLogs& myLogs = MMOGoldStoreManager::getSingleton().GetMyLogsVec();
	GoldStoreMyLogs::const_iterator it = myLogs.begin();

	for (; it != myLogs.end(); ++it)
	{
		UGFxObject*  dat = GFXPlayer->CreateObject(TEXT("UI_GoldStore.AS.GoldStoreItemData"));
		if(dat == NULL )
			continue;
		dat->SetInt(TEXT("GoldStoreID"), idx);
		dat->SetInt(TEXT("ExchangeType"),(*it)->byOrderType);
		dat->SetInt(TEXT("Coupon"), (*it)->nPrice);
		dat->SetInt(TEXT("dealTime"),(*it)->dwTime);
		dat->SetInt(TEXT("Count"),(*it)->nCount);
		if((*it)->byOrderType == eotBuy) 
		{
			dat->SetString(TEXT("ExchangeTypeStr"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.ItemSell")));
		}
		else
		{
			dat->SetString(TEXT("ExchangeTypeStr"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.ItemBuy")));
		}

		datList->SetElementObject(idx, dat);
		
		++idx;
	}
	eventUCSetGoldStore_RecordList(datList);
}

void UMMOUIGoldStore::ShowDefaultPrice()
{
	 MMOGoldStoreManager::getSingleton().GetDefaultPrice();
}

void UMMOUIGoldStore::FristShowPrice()
{
	if( bDefault)
	{
		ShowDefaultPrice();
		bDefault = FALSE;
	}
}

void UMMOUIGoldStore::ASOnExchangeTypeClick(INT type)
{
	MMOGoldStoreManager::getSingleton().setCurrentBuyType(type);
	ShowDefaultPrice();
}

void UMMOUIGoldStore::BeforeViewUpdate(float DeltaTime)
{
	if (IsVisible && bFresh)
	{
		if (freshTime >= FRESHTIME)
		{
			ASFreshListView(0);
			freshTime = 0;
		}
		freshTime += DeltaTime;
	}
}