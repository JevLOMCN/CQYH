#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOAuctionManager.h"
#include "UILayout.h"
#include "ClientActionConditionCheck.h"
#include "UITextParser.h"
#include "MMOMsgBoxManager.h"
#include "MMOVipManager.h"
#include "MMOChatManager.h"
#include "LinkManager.h"

IMPLEMENT_CLASS(UMMOUIAuctionView)
#define SHOW_TIME 0.3

void UMMOUIAuctionView::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Auction_SearchResult);
	evts.AddItem(GameEvent_Auction_ItemSell);
	evts.AddItem(GameEvent_Auction_ItemCancelSell);
	evts.AddItem(GameEvent_Auction_ItemBuy);
	evts.AddItem(GameEvent_Auction_ListMySelling);
	evts.AddItem(GameEvent_Auction_ListMyBuying);
	evts.AddItem(GameEvent_Auction_ListFailed);
	evts.AddItem(GameEvent_Auction_ListMyBought);
	evts.AddItem(GameEvent_Auction_Reset);
	evts.AddItem(GameEvent_Auction_UpdatePageNumb);
	evts.AddItem(GameEvent_Auction_PlayerNameGot);
	evts.AddItem(GameEvent_Auction_UpdateTreeList);
	evts.AddItem(GameEvent_Auction_SearchTreeList);
	evts.AddItem(GameEvent_Auction_FilterTreeList);
	evts.AddItem(GameEvent_Auction_UpdateItemCategoryList);
	evts.AddItem(GameEvent_Auction_QuickSearch);
	
	evts.AddItem(GameEvent_UpdateMoney);	
	evts.AddItem(GameEvent_Item_Delete);

	evts.AddItem(GameEvent_Auction_QuickSearchByBag);
	evts.AddItem(GameEvent_Auction_QuickSearchByChat);

	evts.AddItem(GameEvent_Auction_ShowBag);

	evts.AddItem(GameEvent_Auction_UpdateQueryItem);
	evts.AddItem(GameEvent_Auction_ShowLinkItem);

	evts.AddItem(GameEvent_PlayerBaseInfo);
	
	EventAdapter = new TGameEventAdapter<UMMOUIAuctionView>(this, evts);

	UGFxObject* catList = MMOAuctionManager::CreateAuctionItemCategoryListDataObject(GFXPlayer);
	eventUCSetAuctionViewPage_CategoryListData(catList);

	//g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));

	// call to initialize Sell Slot
	Reset();

	UGFxObject* pageSell = GetObject( TEXT("page_sell") );
	UGFxObject* obj = NULL;
	if( pageSell )
	{
		obj = pageSell->GetObject( TEXT("txt_hint") );
		if(obj)
			obj->SetString( TEXT("text") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.AuctionSell.Explain") ));
	}

	SetSellTimeAndMoney();

	refresh = FALSE;
	currentTime = 0;
	freshTime = 0;

	if( GFXPlayer ){
		GFXPlayer->AddBeforeViewUpdateDelegate( FScriptDelegate(this, TEXT("BeforeViewUpdate")) );
	}
}

void UMMOUIAuctionView::BeforeViewUpdate(float DeltaTime)
{
	if ( this->GetVisible() && refresh )
	{
		if ( currentTime >= SHOW_TIME )
		{
			currentTime = 0;
			refresh = FALSE;
			MMOAuctionManager::getSingleton().UpdateAuctionView();

		}

		currentTime += DeltaTime;
	}

}

void UMMOUIAuctionView::UnInit()
{
	Super::UnInit();
	if( GFXPlayer ){
		GFXPlayer->RemoveBeforeViewUpdateDelegate( FScriptDelegate(EC_EventParm) );
	}
}

void UMMOUIAuctionView::PostOnShow(UBOOL v)

{
	Super::PostOnShow(v);

	if (v)
	{
		if (MMOVipManager::getSingleton().IsLimitedGamePlayMode())
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFreeMode"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFreeMode"));
			this->SetVisible(FALSE);
			return;
		}
			
		MMOFacade::getSingleton().RequestUpdateMoney();

		FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.AuctionSell.Tax"));
		eventUCSetSellItemTax(msg);
		eventUCSetDetailPageHint(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.AuctionView.Explain")));

		if (MMOAuctionManager::getSingleton().GetShowByLink() == FALSE)
		{
			eventUCShowTabPage(EATID_View);
		}
		else
		{
			MMOAuctionManager::getSingleton().SetShowByLink(FALSE);
		}

		if (MMOAuctionManager::getSingleton().GetCurrentTabPage() != EATID_Sell)
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Bag , FALSE, "");
		}
		
	}
	else
	{
		ResetSellItemInfo();
	}

	HideAuction_Msg();
}

void UMMOUIAuctionView::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//MMOGameEvent_Auction_DataUpdate* adu = (MMOGameEvent_Auction_DataUpdate*)evt;
	
	UGFxObject* obj = 0;

	if (this->GetVisible() == FALSE)
		return;
	
	switch (evt->GetEventType())
	{
	case GameEvent_Auction_Reset:
		{
			Reset();
		}
		break;


	case GameEvent_Auction_SearchResult:
		{
			MMOGameEvent_Auction_DataUpdate* auEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
			obj = MMOAuctionManager::CreateAuctionItemListDataObject(GFXPlayer, *auEvt->AuctionItemListPtr);
			if (obj)
			{
				this->eventUCSetAuctionViewPage_ListData(obj,auEvt->bUpdateCurrentPage);
				if (auEvt->AuctionItemListPtr->size() > 0)
				{
					eventUCShowItemNotice(false);
				}
				else
				{
					eventUCShowItemNotice(true);
				}
			}
		}
		break;

	case GameEvent_Auction_ItemSell:
	case GameEvent_Auction_ItemCancelSell:
	case GameEvent_Auction_ListMySelling:
		{
			MMOGameEvent_Auction_DataUpdate* auEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
			obj = MMOAuctionManager::CreateAuctionItemListDataObject(GFXPlayer, *auEvt->AuctionItemListPtr);

			if (obj)
			{
				if(MMOAuctionManager::getSingleton().GetCurrentTabPage() == EATID_Sell)
				{
					this->eventUCSetSellingPage_ListData(obj);
				}
				else
				{
					this->eventUCSetAuctionOrderPageListData(obj);
				}
			}
			
			
		}
		break;

	case GameEvent_Auction_ItemBuy:
	case GameEvent_Auction_ListMyBuying:
		{
			MMOGameEvent_Auction_DataUpdate* auEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
			obj = MMOAuctionManager::CreateAuctionItemListDataObject(GFXPlayer, *auEvt->AuctionItemListPtr);
			if (obj)
			{
				this->eventUCSetAuctionBuyPage_ListData(obj);
			}
		}
		break;

	case GameEvent_Auction_ListFailed:
		{
			MMOGameEvent_Auction_DataUpdate* auEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
			obj = MMOAuctionManager::CreateAuctionItemListDataObject(GFXPlayer, *auEvt->AuctionItemListPtr);
			if (obj)
			{
				this->eventUCSetAuctionBuyPage_FailedListData(obj);
			}
		}
		break;

	case GameEvent_UpdateMoney:
		{
			MMOGameEvent_UpdateMoney* moneyEvt = (MMOGameEvent_UpdateMoney*)(evt);
			if(moneyEvt->MoneyType == E_MONEYTYPE_02)
			{
				FString money = UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02 , MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02) );
				this->eventUCSetPlayerMoney(money);
			}
			break;
		}

	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item* itemEvt = (MMOGameEvent_Item*)(evt);
			if((itemEvt->ContainerIdx == CurrentSellingItemContainerID)
				&& (itemEvt->SlotIdx == CurrentSellingItemSlotID))
			{
				//Reset();
				ResetSellItemInfo();
			}
			break;
		}		

	case GameEvent_Auction_PlayerNameGot:
		{
			MMOGameEvent_Auction_UpdatePlayerName* playerEvt = (MMOGameEvent_Auction_UpdatePlayerName*)evt;
			UGFxObject* dat = MMOAuctionManager::CreatePlayerNameDataObject(GFXPlayer, playerEvt->PlayerID, playerEvt->PlayerName);
			eventUCSetPlayerName(dat);
		}
		break;		

	case GameEvent_Auction_UpdatePageNumb:
		{			
			MMOGameEvent_Auction_DataUpdate* auEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
			eventUCSetAuctionViewPage_PagesInfo(auEvt->CurPageNum, auEvt->MaxPageNum);
		}
		break;
	case GameEvent_Auction_UpdateTreeList:
		{
			UGFxObject* dat = MMOAuctionManager::getSingleton().CreateTreeList(GFXPlayer);
			eventUCUpdateTreeList(dat);
		}
		break;
	case GameEvent_Auction_SearchTreeList:
		{
			UGFxObject* dat = MMOAuctionManager::getSingleton().CreateSearchItem(GFXPlayer);
			eventUCUpdateTreeList(dat);

			if (MMOAuctionManager::getSingleton().GetAuctionItemVec()->size() > 0)
			{
				eventUCShowItemNotice(false);
			}
			else
			{
				eventUCShowItemNotice(true);
			}
		}
		break;
	case GameEvent_Auction_FilterTreeList:
		{
			MMOGameEvent_Auction_DataUpdate* filterEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
	
			UGFxObject* dat = MMOAuctionManager::getSingleton().CreateTreeListByFilter(GFXPlayer,filterEvt->filterType);
			eventUCUpdateTreeList(dat);

			
		}
		break;
	case GameEvent_Auction_UpdateItemCategoryList:
		{
			UGFxObject* catList = MMOAuctionManager::CreateAuctionItemCategoryListDataObject(GFXPlayer);
			eventUCSetAuctionViewPage_CategoryListData(catList);
		}
		break;

	case GameEvent_Auction_QuickSearch:
		{
			UGFxObject* dat = MMOAuctionManager::getSingleton().CreateQuickSearchItem(GFXPlayer);
			eventUCUpdateTreeList(dat);
			eventUCSetSearchName(MMOAuctionManager::getSingleton().GetSearchItemByID()->GetDisplayName());
		}
		break;
	case GameEvent_Auction_QuickSearchByBag:
		{
			MMOGameEvent_Auction_QuickSearchByBag* quickSearchEvt = (MMOGameEvent_Auction_QuickSearchByBag*)evt;
			QuickSearchByBag(quickSearchEvt->SlotType, quickSearchEvt->ContainerID, quickSearchEvt->SlotID);
		}
		break;
	case GameEvent_Auction_QuickSearchByChat:
		{
			//ASClickTabPage(EATID_View);
			eventUCShowPage(EATID_View);
			MMOGameEvent_Auction_QuickSearchByChat* quickSearchByChatEvt = (MMOGameEvent_Auction_QuickSearchByChat*)evt;
			ASTreeListPage_ClickItem(quickSearchByChatEvt->itemId);
		}
		break;
	case GameEvent_Auction_ShowBag:
		{
			ShowBagPanel();
		}
		break;
	case GameEvent_Auction_UpdateQueryItem:
		{
			UpdateQueryItem();
		}
		break;
	case GameEvent_Auction_ListMyBought:
		{
			MMOGameEvent_Auction_DataUpdate* auEvt = (MMOGameEvent_Auction_DataUpdate*)evt;
			obj = MMOAuctionManager::CreateAuctionItemListDataObject(GFXPlayer, *auEvt->AuctionItemListPtr);
			if (obj)
			{
				this->eventUCSetAuctionOrderPageListData(obj);
			}
		}
		break;
	case GameEvent_Auction_ShowLinkItem:
		{
			INT32 itemId = MMOAuctionManager::getSingleton().GetLinkItemIndex();
			eventUCShowLinkItem(itemId);
			MMOAuctionManager::getSingleton().ResetLinkItemInfo();
		}
		break;
	case GameEvent_PlayerBaseInfo:
			refresh = TRUE;
		break;
	}
}


void UMMOUIAuctionView::ResetSellItemInfo()
{
	//unlock the locked stuff
	MMOFacade::getSingleton().LockItem( CurrentSellingItemContainerID, CurrentSellingItemSlotID, -1 );

	CurrentSellingItemContainerID = -1;
	CurrentSellingItemSlotID = -1;
	eventUCResetSellItem(ESPType_AuctionSell);

	MMOAuctionManager::getSingleton().ResetSellInfo();
}

void UMMOUIAuctionView::Reset()
{
	ResetSellItemInfo();

	AuctionItemList emptyList;
	UGFxObject* datList = MMOAuctionManager::CreateAuctionItemListDataObject(GFXPlayer, emptyList);
	eventUCSetAuctionViewPage_ListData(datList,FALSE);
	eventUCSetAuctionBuyPage_FailedListData(datList);
	eventUCSetAuctionBuyPage_ListData(datList);
	eventUCSetSellingPage_ListData(datList);

	eventUCSetAuctionViewPage_PagesInfo(0, 1);
	MMOAuctionManager::getSingleton().LastActionIsNextPage(true);
}

INT UMMOUIAuctionView::GetSlotType()
{
	return E_CONSLOT_EQUIPPED;
}


UBOOL UMMOUIAuctionView::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	return FALSE; 
} 

UBOOL UMMOUIAuctionView::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{

	/*if(SlotType == ESPType_Bag)
	{
		if(MMOFacade::getSingleton().GetMainController()->PlayerInput->IsCtrlPressed())
		{
			FClientItemInfo* info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
			if(info)
			{
				MMOAuctionManager::getSingleton().SetSearchItemId(info->m_nTemplateID);
				MMOAuctionManager::getSingleton().NotifyQuickSearch();
				MMOAuctionManager::getSingleton().Send_SearchByFilter();
			}

			return TRUE;
		}
	}*/
	if(SlotType == ESPType_AuctionItem)
	{
		if(MMOFacade::getSingleton().GetMainController()->PlayerInput->IsCtrlPressed())
		{
			//const UItem* item = MMOFacade::getSingleton().FindItemTemplate(ContainerID, SlotID);
			MMOGameEvent_Chat_AppendInput evt(SlotType,ContainerID,SlotID,bDefault);
			MMOUI::PostGameEvent(&evt);
			return TRUE;
		}
	}
	
	return FALSE;
}

UBOOL UMMOUIAuctionView::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if (MMOVipManager::getSingleton().IsLimitedGamePlayMode()== FALSE && this->GetVisible())
	{
		if(SlotType == ESPType_Bag)
		{
			eventUCShowTabPage(EATID_Sell);

			SetupSellItem(ContainerID, SlotID);
			return TRUE;
		}
		else if(SlotType == ESPType_AuctionSell)
		{
			ResetSellItemInfo();
			return TRUE;
		}
	}
	
	return FALSE;
}

UBOOL UMMOUIAuctionView::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if((SrcSlotType == ESPType_Bag) && (DestSlotType == ESPType_AuctionSell))
	{
		SetupSellItem(SrcContainerID, SrcSlotID);
		return TRUE;
	}
	else if(SrcSlotType == ESPType_AuctionSell)
	{
		ResetSellItemInfo();
		return TRUE;
	}
	return FALSE;
}

class ClientActionCondition_AuctionSellItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_AuctionSellItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckBind);
		//AddConditionCheckType(ECACCT_Item_CheckCanTrade);
	}
};

void UMMOUIAuctionView::SetupSellItem(INT srcContainerID, INT srcSlotID)
{
	UGFxObject* dat = GFXPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.SellInfoData"));
	if(dat)
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(srcContainerID, srcSlotID);
		if(info)
		{
			UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);

			if(!item)
			{
				warnf(TEXT("Item not found when preparing to list on the auction: %d"), info->m_nTemplateID);
				return;
			}

			if(item->Auction == FALSE)
			{
				FString errorName = TEXT("Auction.Sell.CannotSell");
				MMOUI::GetUIManager()->delegateAddScreenMessage(
					MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table, errorName );

				return;
			}

			//listing fee
			TArray<INT> config = MMOAuctionManager::getSingleton().GetAuctionSellConfig();
			INT64 money = MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02);

			if(money < config(3)* MONEY_RATIO /*|| money < config(4)* MONEY_RATIO || money < config(5)* MONEY_RATIO*/)
			{
				FString errorName = TEXT("Auction.Msg.AuctionFree");
				MMOUI::GetUIManager()->delegateAddScreenMessage(
					MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table, errorName );

				return;
			}
			ClientActionCondition_AuctionSellItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;
			contex.ItemTemplate = item;

			if( !checker.CheckClientActionCondition(contex) )
			{
				return;
			}

			UGFxObject* slotDat = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));
			if(slotDat)
			{
				slotDat->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
				slotDat->SetInt( TEXT("slotType"), ESPType_AuctionSell );
				slotDat->SetInt( TEXT("slotIndex"), srcSlotID );
				slotDat->SetBool( TEXT("HaveData"), TRUE );

				slotDat->SetInt(TEXT("quantity"), info->GetCurStackCount(item)/*(item->ItemDuration == E_DURTYPE_STACK ? info->m_nCurDur : 1)*/);
				slotDat->SetInt(TEXT("iconIndex"), -1);		
				slotDat->SetInt(TEXT("itemColor"), item->ItemColor);

				FString cs = FString(TEXT("img://")) + MMOFacade::getSingleton().GetItemIconNamePersonalized( item, info );
				slotDat->SetString(TEXT("iconName"), cs);

				slotDat->SetInt(TEXT("containerID"), srcContainerID);
				slotDat->SetInt(TEXT("slotID"), srcSlotID);
				slotDat->SetInt(TEXT("itemTemplateID"), info->m_nTemplateID);
			}

			dat->SetObject(TEXT("ItemData"), slotDat);
			dat->SetString(TEXT("ItemName"), UITextParser::HTMLFormatItemColorString(item));

			dat->SetInt(TEXT("ActionTotalPrice"), item->AuctionDefaultPrice);

			if((item->ItemDuration == E_DURTYPE_STACK) && (info->m_nCurDur > 0))
			{
				dat->SetInt(TEXT("ActionTotalPrice"), item->AuctionDefaultPrice * info->m_nCurDur);
				dat->SetInt(TEXT("ActionUnitPrice"), ((item->AuctionDefaultPrice * info->m_nCurDur) / info->m_nCurDur));
			}
			else
			{
				dat->SetInt(TEXT("ActionTotalPrice"), item->AuctionDefaultPrice);
				dat->SetInt(TEXT("ActionUnitPrice"), item->AuctionDefaultPrice);
			}

			dat->SetInt(TEXT("TotalFixedPrice"), 0);
			//INT managePrice = MMOAuctionManager::getSingleton().GetMoneyRatio(item->AuctionFee);
			//dat->SetInt(TEXT("ManagePrice"), managePrice);
			dat->SetInt(TEXT("DefaultPrice"), item->AuctionDefaultPrice);

			eventUCSetSellingPage_SellInfo(dat);

			//first unlock the previous item
			MMOFacade::getSingleton().LockItem( CurrentSellingItemContainerID, CurrentSellingItemSlotID,-1);

			CurrentSellingItemContainerID = srcContainerID;
			CurrentSellingItemSlotID = srcSlotID;
			
			//lock the stuff
			MMOFacade::getSingleton().LockItem( srcContainerID, srcSlotID,ESPType_AuctionSell);
		}
	}
	
}


FString UMMOUIAuctionView::GetMsgText(BYTE textType,INT dwAuctionID,INT price)
{
	FString szText;

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(dwAuctionID);
	if (!item)
	{
		return szText;
	}

	if (textType == Auction_MsgTextType_Auction)
	{
		szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Msg.Auction.IsAuction")) , price, *item->GetDisplayName());
	}
	else if(textType == Auction_MsgTextType_Fixed)
	{
		szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Msg.Auction.IsFixedPrice")) , price, *item->GetDisplayName());
	}
	else
	{
		szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Msg.Auction.Auction_greaterthan_FP")));
	}
	return szText;
}

FString UMMOUIAuctionView::GetMsgTextContent(BYTE textType,INT dwAuctionID)
{
	FString szText;

	ClientAuctionItemInfo* info = MMOAuctionManager::getSingleton().GetItemFromBuyListAndBuyOutList(dwAuctionID);
	const ClientAuctionItemInfo* viewInfo = MMOAuctionManager::getSingleton().TryFindLocal(dwAuctionID);

	if (textType == Auction_MsgTextType_Auction)
	{
		if (info)
		{
			szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Auction.CashDeposit")) ,MMOAuctionManager::getSingleton().GetMoneyRatio(info->nDepositPrice));
		}
		else if(viewInfo)
		{

			szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Auction.NotCashDeposit")) ,MMOAuctionManager::getSingleton().GetMoneyRatio(viewInfo->nDepositPrice));
		}
		
	}
	else if(textType == Auction_MsgTextType_Fixed)
	{
		if (info)
		{
			szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Auction.FixedPrice.CashDeposit")) ,MMOAuctionManager::getSingleton().GetMoneyRatio(info->nDepositPrice));
		}
		else if(viewInfo)
		{
			szText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Auction.FixedPrice.NotCashDeposit")) ,MMOAuctionManager::getSingleton().GetMoneyRatio(viewInfo->nDepositPrice));
		}
	}
	
	return szText;

	
}

void UMMOUIAuctionView::CreateDelegate( FString functionName,INT dwAuctionID, FString msgText,INT price,UBOOL isAddValue,FString msgTextContent,INT type)
{
	FScriptDelegate AuctionDelegate(EC_EventParm);
	AuctionDelegate.FunctionName = *functionName;
	AuctionDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnAuctionMsg_Cancel");
	Nofunction.Object = this;

	if (type == EAMB_Normal)
	{
		ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgText ,dwAuctionID , AuctionDelegate , Nofunction );
	}
	else
	{
		ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddBigMessageBox( msgText , msgTextContent,dwAuctionID , AuctionDelegate , Nofunction );
	}
	
	if (isAddValue)
	{
		if(ConfirmMsgBox)
			ConfirmMsgBox->SetInt( TEXT("itemdataid") , price);
	}
}

//showAuction messageBox
void UMMOUIAuctionView::Show_Auction_Msg(INT dwAuctionID,INT price,Auction_MsgType msgType,Auction_MsgTextType textType,INT itemId)
{
	FString functionName="";
	FString txtType="";
	FString txtContent = "";
	BOOL isAddItem = FALSE;
	Auction_MsgBoxType type = EAMB_Normal;

	HideAuction_Msg();

	switch (msgType)
	{
	case Auction_MsgType_ViewPage_Auction_OK:
		{
			functionName = "OnViewPage_Auction_OK";
			txtType = GetMsgText(textType,itemId,price);
			txtContent = GetMsgTextContent(textType,dwAuctionID);
			isAddItem = TRUE;
			if(textType == Auction_MsgTextType_Fixed_Greaterthan)
			{
				type = EAMB_Normal;
			}
			else
			{
				type = EAMB_Big;
			}
			
		}
		break;
	case Auction_MsgType_ViewPage_FixedPrice_OK:
		{
			functionName = "OnViewPage_FixedPrice_OK";
			txtType = GetMsgText(textType,itemId,price);
			txtContent = GetMsgTextContent(textType,dwAuctionID);
			isAddItem = TRUE;
			type = EAMB_Big;
		}
		break;
	case Auction_MsgType_BuyPage_Auction_OK:
		{
			functionName = "OnBuyPage_Auction_OK";
			txtType = GetMsgText(textType,itemId,price);
			txtContent = GetMsgTextContent(textType,dwAuctionID);
			isAddItem = TRUE;
			type = EAMB_Big;
		}
		break;
	case Auction_MsgType_BuyPage_FixedPrice_OK:
		{
			functionName = "OnBuyPage_FixedPrice_OK";
			txtType = GetMsgText(textType,itemId,price);
			txtContent = GetMsgTextContent(textType,dwAuctionID);
			isAddItem = TRUE;
			type = EAMB_Big;
		}
		break;
	case Auction_MsgType_BuyPage_Failed_Auction_OK:
		{
			functionName = "OnBuyPage_FailedAuction_OK";
			txtType = GetMsgText(textType,itemId,price);
			txtContent = GetMsgTextContent(textType,dwAuctionID);
			isAddItem = TRUE;
			type = EAMB_Big;
		}
		break;
	case Auction_MsgType_BuyPage_Failed_FixedPrice_OK:
		{
			functionName = "OnBuyPage_FailedFixedPrice_OK";
			txtType = GetMsgText(textType,itemId,price);
			txtContent = GetMsgTextContent(textType,dwAuctionID);
			isAddItem = TRUE;
			type = EAMB_Big;
		}
		break;
	case Auction_MsgType_CancelSell:
		{
			functionName = "OnCancelSell_OK";
			txtType = MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Msg.Sell.IsCancelSell"));
			isAddItem = FALSE;
		}
		break;

	}
	CreateDelegate(functionName,dwAuctionID,txtType,price,isAddItem,txtContent,type);
}



void UMMOUIAuctionView::OnViewPage_Auction_OK(class UGFxObject* mc)
{
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	INT price = mc->GetInt(TEXT("itemdataid"));
	MMOAuctionManager::getSingleton().BuyFromViewList(dwAuctionID, price);
	MMOAuctionManager::getSingleton().SetUpdateCurrentPage(TRUE);
	ConfirmMsgBox = NULL;
}
void UMMOUIAuctionView::OnViewPage_FixedPrice_OK(class UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;

	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	INT64 price = mc->GetInt(TEXT("itemdataid"));
	INT64 realPrice = MMOAuctionManager::getSingleton().GetRealAuctionMoney(price);
	CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, realPrice);
	MMOAuctionManager::getSingleton().SetUpdateCurrentPage(TRUE);
	ConfirmMsgBox = NULL;
}

void UMMOUIAuctionView::OnBuyPage_Auction_OK(class UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	INT price = mc->GetInt(TEXT("itemdataid"));
	MMOAuctionManager::getSingleton().BuyFromBuyingList(dwAuctionID, price);
	ConfirmMsgBox = NULL;
}
void UMMOUIAuctionView::OnBuyPage_FixedPrice_OK(class UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	INT64 price = mc->GetInt(TEXT("itemdataid"));
	INT64 realPrice = MMOAuctionManager::getSingleton().GetRealAuctionMoney(price);
	CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, realPrice);
	ConfirmMsgBox = NULL;
}
void UMMOUIAuctionView::OnBuyPage_FailedAuction_OK(class UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	INT price = mc->GetInt(TEXT("itemdataid"));
	MMOAuctionManager::getSingleton().BuyFromFaildAuctionList(dwAuctionID, price);
	ConfirmMsgBox = NULL;
}
void UMMOUIAuctionView::OnBuyPage_FailedFixedPrice_OK(class UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	INT64 price = mc->GetInt(TEXT("itemdataid"));
	INT64 realPrice = MMOAuctionManager::getSingleton().GetRealAuctionMoney(price);
	CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, realPrice);
	ConfirmMsgBox = NULL;
}

void UMMOUIAuctionView::OnCancelSell_OK(UGFxObject* mc)
{
	//ASSERT_CHECK_ERROR_RETURN(mc);
	if (NULL == mc)
		return;
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_AuctionCancelSell(dwAuctionID);
	ConfirmMsgBox = NULL;
}

void UMMOUIAuctionView::OnAuctionMsg_Cancel(UGFxObject* mc)
{

	ConfirmMsgBox = NULL;
}

void UMMOUIAuctionView::HideAuction_Msg()
{
	if( ConfirmMsgBox != NULL )
	{
		ConfirmMsgBox->SetVisible( FALSE );
		ConfirmMsgBox = NULL;
	}
}

void UMMOUIAuctionView::QuickSearchByBag(BYTE SlotType,BYTE ContainerID,INT SlotID)
{
	if(SlotType == ESPType_Bag)
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
		if(info)
		{
			MMOAuctionManager::getSingleton().SetSearchItemId(info->m_nTemplateID);
			MMOAuctionManager::getSingleton().NotifyQuickSearch();
			MMOAuctionManager::getSingleton().Send_SearchByFilter();
		}
	}
}

//-----------------Moved-----------------------

void UMMOUIAuctionView::ASClickTabPage(INT idx)
{
	if ((EAuctionTabId)idx != MMOAuctionManager::getSingleton().GetCurrentTabPage())
	{
		//close messagebox
		HideAuction_Msg();
	}
	
	MMOAuctionManager::getSingleton().SetCurrentTabPage((EAuctionTabId)idx);
}

// buy
void UMMOUIAuctionView::ASBuyPage_AuctionClick(INT dwAuctionID, INT price, INT msgType)
{
	//MMOAuctionManager::getSingleton().BuyFromBuyingList(dwAuctionID, price);
	if (MMOAuctionManager::getSingleton().IsShowSmg_DefaultPrice(dwAuctionID,price,AS_Type_BuyPage))
	{
		INT itemId = MMOAuctionManager::getSingleton().GetitemIdByAuctionId(dwAuctionID,AS_Type_BuyPage);
		Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_Auction_OK,(Auction_MsgTextType)msgType,itemId);
	}


}

void UMMOUIAuctionView::ASBuyPage_AuctionFixedPriceClick(INT dwAuctionID, INT price)
{
	//MMOAuctionManager::getSingleton().BuyFromBuyingList(dwAuctionID, price);
	//CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, price);
	INT itemId = MMOAuctionManager::getSingleton().GetitemIdByAuctionId(dwAuctionID,AS_Type_BuyPage);
	Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_FixedPrice_OK,Auction_MsgTextType_Fixed,itemId);
}

void UMMOUIAuctionView::ASBuyPage_AuctionFailedClick(INT dwAuctionID, INT price,INT msgType)
{
	//MMOAuctionManager::getSingleton().BuyFromFaildAuctionList(dwAuctionID, price);
	if (MMOAuctionManager::getSingleton().IsShowSmg_DefaultPrice(dwAuctionID,price,AS_Type_BuyFailedPage))
	{
		INT itemId = MMOAuctionManager::getSingleton().GetitemIdByAuctionId(dwAuctionID,AS_Type_BuyFailedPage);
		Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_Failed_Auction_OK,(Auction_MsgTextType)msgType,itemId);
	}

}

void UMMOUIAuctionView::ASBuyPage_AuctionFailedFixedPriceClick(INT dwAuctionID, INT price)
{
	//MMOAuctionManager::getSingleton().BuyFromFaildAuctionList(dwAuctionID, price);
	//CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, price);
	INT itemId = MMOAuctionManager::getSingleton().GetitemIdByAuctionId(dwAuctionID,AS_Type_BuyFailedPage);
	Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_Failed_FixedPrice_OK,Auction_MsgTextType_Fixed,itemId);
}

// sell
void UMMOUIAuctionView::ASSellPage_CancelSellClick(INT dwAuctionID)
{
	Show_Auction_Msg(dwAuctionID,0,Auction_MsgType_CancelSell,(Auction_MsgTextType)0,0);
	//CMessageHandler::GetInstance().Send_AuctionCancelSell(dwAuctionID);
}

void UMMOUIAuctionView::ASSellPage_AuctionSellItemClick(UGFxObject* dat)
{
	//C2M_AUCTION_SELL* info = (C2M_AUCTION_SELL*)CMessageHandler::ms_MsgBuffer;

	AuctionSellInfo info;

	info.byConSlot = dat->GetInt(TEXT("ContainerID"));
	if (info.byConSlot < E_CONSLOT_EQUIPPED || info.byConSlot >= E_CONSLOT_MAX)
		return;

	info.byItemSlot = dat->GetInt(TEXT("SlotID"));

	INT64 actionTotalPrice = dat->GetInt(TEXT("ActionTotalPrice"));
	INT64 totalFixedPrice = dat->GetInt(TEXT("TotalFixedPrice"));
	INT64 depositePrice = dat->GetInt(TEXT("AuctionDepositPrice"));

	info.llPrice = MMOAuctionManager::getSingleton().GetRealAuctionMoney(actionTotalPrice);
	info.llBuyoutPrice = MMOAuctionManager::getSingleton().GetRealAuctionMoney(totalFixedPrice);
	info.llDepositePrice = MMOAuctionManager::getSingleton().GetRealAuctionMoney(depositePrice);
	 
	info.byAuctionTimeType = dat->GetInt(TEXT("AuctionTimeIndex"));

	BOOL byOpen = dat->GetBool(TEXT("isPublicAunction"));
	info.byOpen = byOpen==TRUE ? 1 : 0;

	if (info.llPrice <= 0)
	{	
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Msg.AuctionPriceError") ), MsgInfoType_Table, TEXT("Auction.Msg.AuctionPriceError") );
		return;
	}

	if(info.llBuyoutPrice > 0 && info.llBuyoutPrice < info.llPrice)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Msg.FixPriceLessThanAuctionPrice") ), MsgInfoType_Table, TEXT("Auction.Msg.FixPriceLessThanAuctionPrice") );
		return;
	}

	/*if(info->llBuyoutPrice > 0)
	{
	if (depositePrice > totalFixedPrice / 2)
	{
	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Msg.DepositePriceLessThanTotalPrice") ), MsgInfoType_Table, TEXT("Auction.Msg.DepositePriceLessThanTotalPrice") );
	return;
	}
	}
	else
	{
	if (depositePrice > actionTotalPrice)
	{
	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Msg.DepositePriceLessAuctionPrice") ), MsgInfoType_Table, TEXT("Auction.Msg.DepositePriceLessAuctionPrice") );
	return;
	}
	}*/

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(info.byConSlot, info.byItemSlot);
	if(item && (info.llPrice < item->AuctionDefaultPrice))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Msg.AuctionPriceLessThanDefaultPrice") ), MsgInfoType_Table, TEXT("Auction.Msg.AuctionPriceLessThanDefaultPrice") );
		return;
	}

	FScriptDelegate AuctionDelegate(EC_EventParm);
	AuctionDelegate.FunctionName = TEXT("OnSellPage_SellItem");
	AuctionDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnAuctionMsg_Cancel");
	Nofunction.Object = this;

	FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Auction.SellItem"));
	INT money = dat->GetInt(TEXT("AuctionManagePrice"));
	msg = msg.Replace(TEXT("[xx]"), *appItoa(money));


	ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msg ,item->ItemId , AuctionDelegate , Nofunction );

	//CMessageHandler::GetInstance().Send_AuctionSell(info);
	MMOAuctionManager::getSingleton().SetAuctionSellInfo(info);
}

void UMMOUIAuctionView::OnSellPage_SellItem(UGFxObject* mc)
{
	C2M_AUCTION_SELL* sellInfo = (C2M_AUCTION_SELL*)CMessageHandler::ms_MsgBuffer;
	AuctionSellInfo info = MMOAuctionManager::getSingleton().GetAuctionSellInfo();
	sellInfo->byAuctionTimeType = info.byAuctionTimeType;
	sellInfo->byConSlot = info.byConSlot;
	sellInfo->byItemSlot = info.byItemSlot;
	sellInfo->byOpen = info.byOpen;
	sellInfo->llBuyoutPrice = info.llBuyoutPrice;
	sellInfo->llDepositePrice = info.llDepositePrice;
	sellInfo->llPrice = info.llPrice;
	CMessageHandler::GetInstance().Send_AuctionSell(sellInfo);
}
// browse
void UMMOUIAuctionView::ASSearch(class UGFxObject* dt)
{
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(0);
	MMOAuctionManager::getSingleton().MaxPageNumb(1);
	MMOAuctionManager::getSingleton().UpdateSearchFilter(dt);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOUIAuctionView::ASSearch_ResetFilter()
{
	MMOAuctionManager::getSingleton().UpdateSearchFilter(NULL);
	MMOAuctionManager::getSingleton().ResetAuctionItemVec();
	MMOAuctionManager::getSingleton().NotifyUpdateItemCategoryList();
}

void UMMOUIAuctionView::ASViewPage_ClickHeaderSort(INT colIdx,UBOOL descending)
{
	MMOAuctionManager::getSingleton().UpdateSearchSortBy(colIdx, descending);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOUIAuctionView::ASViewPage_AuctionClick(INT dwAuctionID, INT price,INT msgType)
{
	//MMOAuctionManager::getSingleton().BuyFromViewList(dwAuctionID, price);
	if (MMOAuctionManager::getSingleton().IsShowSmg_DefaultPrice(dwAuctionID,price,AS_Type_ViewPage))
	{
		INT itemId = MMOAuctionManager::getSingleton().GetitemIdByAuctionId(dwAuctionID,AS_Type_ViewPage);
		Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_ViewPage_Auction_OK,(Auction_MsgTextType)msgType,itemId);
	}

}

void UMMOUIAuctionView::ASViewPage_AuctionFixedPriceClick(INT dwAuctionID, int price)
{
	//MMOAuctionManager::getSingleton().BuyFromViewList(dwAuctionID, price);
	//CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, price);
	INT itemId = MMOAuctionManager::getSingleton().GetitemIdByAuctionId(dwAuctionID,AS_Type_ViewPage);
	Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_ViewPage_FixedPrice_OK,Auction_MsgTextType_Fixed,itemId);
}

// Turn page
void UMMOUIAuctionView::ASViewPage_PrevPageClick(INT pgNum)
{
	MMOAuctionManager::getSingleton().LastActionIsNextPage(false);
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(pgNum);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOUIAuctionView::ASViewPage_NextPageClick(INT pgNum)
{
	MMOAuctionManager::getSingleton().LastActionIsNextPage(true);
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(pgNum);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}


void UMMOUIAuctionView::ASViewPage_GotoPageClick(INT pgNum)
{
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(pgNum);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOUIAuctionView::ASViewPage_CategoryClick(BYTE mainType, BYTE subType)
{

	MMOAuctionManager::getSingleton().OnCategoryClick(mainType,subType);

}

void UMMOUIAuctionView::ASOnPanelClose(const FString& PanelName)
{
	if( PanelName == MMOUI::GetUIManager()->Panel_AuctionView.ToString())
	{
		ResetSellItemInfo();
	}
	//Super::ASOnPanelClose(PanelName);
}

void UMMOUIAuctionView::ASTreeListPage_ClickItem(int itemId)
{
	MMOAuctionManager::getSingleton().SetSearchItemId(itemId);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOUIAuctionView::ASAuction_Fresh()
{
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}
void UMMOUIAuctionView::ASAuction_SearchItem(const FString& itemName)
{
	MMOAuctionManager::getSingleton().SearchItem();
	MMOAuctionManager::getSingleton().NotifySearchTreeItemList();
}

void UMMOUIAuctionView::ASAuction_FilterTreeList(int type,class UGFxObject* dt)
{
	//if (MMOAuctionManager::getSingleton().GetAuctionItemVec()->size() > 0)
	//{
	MMOAuctionManager::getSingleton().UpdateSearchFilter(dt);
	MMOAuctionManager::getSingleton().NotifyFilterTreeItem((Auction_FilterType)type);
	//}

}

void UMMOUIAuctionView::ASAuction_ReturnLastList()
{
	MMOAuctionManager::getSingleton().ResetAuctionItemVec();
}

void UMMOUIAuctionView::ShowBagPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Bag , TRUE, "");
	UMMOUIBase* rightPanel = MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Bag);
	if(!rightPanel )
	{
		return ;
	}

	//FVector2D scrSz = MMOFacade::GetScreenSize();

	INT leftW = this->eventGetWidth();
	
	FLOAT panelX; 
	FLOAT panelY;
	this->GetPosition(panelX,panelY);

	rightPanel->SetPosition(panelX + leftW, panelY);
	rightPanel->SetVisible(TRUE);

}

void UMMOUIAuctionView::ASUpdateOrderList(INT curPage)
{
	switch(curPage)
	{
	case EOPT_Buy:
		{
			CMessageHandler::GetInstance().Send_AuctionListMyBuying();
		}
		break;
	case EOPT_Sell:
		{
			CMessageHandler::GetInstance().Send_AuctionListMySelling();
		}
		break;
	}
}

void UMMOUIAuctionView::SetSellTimeAndMoney()
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");
	TArray<INT> config = MMOAuctionManager::getSingleton().GetAuctionSellConfig();

	if (object)
	{
		object->SetInt(TEXT("AuctionSellTime1"),config(0));
		object->SetInt(TEXT("AuctionSellTime2"),config(1));
		object->SetInt(TEXT("AuctionSellTime3"),config(2));

		object->SetInt(TEXT("AuctionSellMoney1"),config(3));
		object->SetInt(TEXT("AuctionSellMoney2"),config(4));
		object->SetInt(TEXT("AuctionSellMoney3"),config(5));

		eventUCSetSellTimeAndMoney(object);

	}

	
	
}

void UMMOUIAuctionView::UpdateQueryItem()
{
	UGFxObject*  dat = GFXPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.AuctionItemData"));
	
	if (dat)
	{
		QueryAuctionItemInfo info = MMOAuctionManager::getSingleton().GetQueryItemInfo();
		const ClientAuctionItemInfo* viewInfo = MMOAuctionManager::getSingleton().TryFindLocal(info.dwAuctionID);

		FString sellerName = info.szSellerName;

		if (viewInfo)
		{
			sellerName = LinkManager::getSingleton().HtmlLink_AuctionPlayer(sellerName,viewInfo->dwSellerRoleID);
		}
	
		dat->SetString(TEXT("Seller"),sellerName);
		dat->SetString(TEXT("BuyerGuildName"),info.szGuildName);
		dat->SetString(TEXT("BuyerProfession"),info.bySellerClass);
		dat->SetInt(TEXT("BuyerLevel"),info.bySellerLevel);
		dat->SetInt(TEXT("BuyerCount"),info.dwBuyerCount);
		dat->SetString(TEXT("PayTimeLimit"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Auction.PayTimeLimit")));
		

		ClientAuctionItemInfo itemInfo = info.info;

		UGFxObject* slotDat = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));
		//check(slotDat);		
		if(slotDat)
		{
			slotDat->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
			slotDat->SetInt( TEXT("slotType"), ESPType_AuctionItem );
			slotDat->SetInt( TEXT("slotIndex"), 0 );
			slotDat->SetBool( TEXT("HaveData"), TRUE );

			slotDat->SetInt(TEXT("quantity"), itemInfo.GetCurStackCount(itemInfo.ItemTemplate)/*(itemInfo.ItemTemplate->ItemDuration == E_DURTYPE_STACK ? itemInfo.m_nCurDur : 1)*/);
			slotDat->SetInt(TEXT("iconIndex"), -1);		
			slotDat->SetInt(TEXT("itemColor"), itemInfo.ItemTemplate->ItemColor);

			FString cs = FString(TEXT("img://")) + MMOFacade::getSingleton().GetItemIconNamePersonalized( itemInfo.ItemTemplate );
			slotDat->SetString(TEXT("iconName"), cs);

			slotDat->SetInt(TEXT("containerID"), -1);
			slotDat->SetInt(TEXT("slotID"), itemInfo.dwAuctionID);
			slotDat->SetInt(TEXT("itemTemplateID"), itemInfo.m_nTemplateID);
			dat->SetObject(TEXT("ItemData"), slotDat);
		}
		INT depositPrice = MMOAuctionManager::getSingleton().GetMoneyRatio(itemInfo.nDepositPrice);
		dat->SetInt(TEXT("AuctionDepositPrice"),depositPrice);

		__time32_t localTime = itemInfo.dwStartTime;
		struct tm *time = _localtime32(&localTime);
		if (time)
		{
			FString startTime = FString::Printf(TEXT("%d-%d-%d"),time->tm_year+1900,time->tm_mon+1,time->tm_mday); 
			dat->SetString(TEXT("AuctionStartTime"),startTime);
		}
		dat->SetString(TEXT("ItemName"), UITextParser::HTMLFormatItemColorString(itemInfo.ItemTemplate));

		eventUCUpdateDetailDataItem(dat);
	}
}

void UMMOUIAuctionView::ASViewPage_ItemListItemClick(INT auctionId)
{
	CMessageHandler::GetInstance().Send_AuctionQuery(auctionId);
}

void UMMOUIAuctionView::ASOnOrderPay(INT auctionId)
{
	//CMessageHandler::GetInstance().Send_AucrionPayLeftMoney(auctionId);
	const ClientAuctionItemInfo* info = MMOAuctionManager::getSingleton().GetBoughtItemById(auctionId);
	if (info)
	{
		FScriptDelegate AuctionDelegate(EC_EventParm);
		AuctionDelegate.FunctionName = TEXT("OnViewPage_Auction_PayLeft");
		AuctionDelegate.Object = this;

		FScriptDelegate Nofunction(EC_EventParm);
		Nofunction.FunctionName = TEXT("OnAuctionMsg_Cancel");
		Nofunction.Object = this;

		FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Auction.PayLeft"));
		INT money = MMOAuctionManager::getSingleton().GetMoneyRatio(info->nPrice - info->nDepositPrice);
		msg = msg.Replace(TEXT("[XX]"), *appItoa(money));
		msg = msg.Replace(TEXT("yy"), *info->ItemTemplate->GetDisplayName());

		ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msg ,auctionId , AuctionDelegate , Nofunction );
	}
}

void UMMOUIAuctionView::OnViewPage_Auction_PayLeft(UGFxObject* mc)
{
	INT dwAuctionID = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_AucrionPayLeftMoney(dwAuctionID);
	ConfirmMsgBox = NULL;
}
