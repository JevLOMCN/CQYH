#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOAuctionManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Auction)


void UMMOGFXPlayer_Auction::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Auction::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Auction);
}

void UMMOGFXPlayer_Auction::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_Auction::ASClickTabPage(INT idx)
{
	if ((EAuctionTabId)idx != MMOAuctionManager::getSingleton().GetCurrentTabPage())
	{
		//close messagebox
		_auctionPanel->HideAuction_Msg();
	}
	MMOAuctionManager::getSingleton().SetCurrentTabPage((EAuctionTabId)idx);
}

// buy
void UMMOGFXPlayer_Auction::ASBuyPage_AuctionClick(INT dwAuctionID, INT price, INT msgType)
{
	//MMOAuctionManager::getSingleton().BuyFromBuyingList(dwAuctionID, price);
	/*if (MMOAuctionManager::getSingleton().IsShowSmg_DefaultPrice(dwAuctionID,price,((Auction_ScreenMsg_Type)1)))
	{
		_auctionPanel->Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_Auction_OK,msgType);
	}*/
	

}

void UMMOGFXPlayer_Auction::ASBuyPage_AuctionFixedPriceClick(INT dwAuctionID, INT price)
{
	//MMOAuctionManager::getSingleton().BuyFromBuyingList(dwAuctionID, price);
	//CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, price);
	//_auctionPanel->Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_FixedPrice_OK,0);
}

void UMMOGFXPlayer_Auction::ASBuyPage_AuctionFailedClick(INT dwAuctionID, INT price,INT msgType)
{
	//MMOAuctionManager::getSingleton().BuyFromFaildAuctionList(dwAuctionID, price);
	if (MMOAuctionManager::getSingleton().IsShowSmg_DefaultPrice(dwAuctionID,price,((Auction_ScreenMsg_Type)2)))
	{
		//_auctionPanel->Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_Failed_Auction_OK,msgType);
	}

}

void UMMOGFXPlayer_Auction::ASBuyPage_AuctionFailedFixedPriceClick(INT dwAuctionID, INT price)
{
	//MMOAuctionManager::getSingleton().BuyFromFaildAuctionList(dwAuctionID, price);
	//CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, price);
	//_auctionPanel->Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_BuyPage_Failed_FixedPrice_OK,0);
}

// sell
void UMMOGFXPlayer_Auction::ASSellPage_CancelSellClick(INT dwAuctionID)
{
	//_auctionPanel->Show_Auction_Msg(dwAuctionID,0,Auction_MsgType_CancelSell,0);
	//CMessageHandler::GetInstance().Send_AuctionCancelSell(dwAuctionID);
}

void UMMOGFXPlayer_Auction::ASSellPage_AuctionSellItemClick(UGFxObject* dat)
{
	UINT8 containerID = dat->GetInt(TEXT("ContainerID"));
	if (containerID < E_CONSLOT_EQUIPPED || containerID >= E_CONSLOT_MAX)
		return;

	UINT8 slotId = dat->GetInt(TEXT("SlotID"));
	INT actionTotalPrice = dat->GetInt(TEXT("ActionTotalPrice"));
	INT totalFixedPrice = dat->GetInt(TEXT("TotalFixedPrice"));
	
	if(totalFixedPrice > 0 && totalFixedPrice < actionTotalPrice)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(
			MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Msg.FixPriceLessThanAuctionPrice")));
		return;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(containerID, slotId);
	if(item && (actionTotalPrice < item->AuctionDefaultPrice))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(
			MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Msg.AuctionPriceLessThanDefaultPrice")));
		return;
	}

	//CMessageHandler::GetInstance().Send_AuctionSell(containerID, slotId, actionTotalPrice, totalFixedPrice);
}

// browse
void UMMOGFXPlayer_Auction::ASSearch(class UGFxObject* dt)
{
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(0);
	MMOAuctionManager::getSingleton().MaxPageNumb(1);
	MMOAuctionManager::getSingleton().UpdateSearchFilter(dt);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

 void UMMOGFXPlayer_Auction::ASSearch_ResetFilter()
 {
	 MMOAuctionManager::getSingleton().UpdateSearchFilter(NULL);
	 MMOAuctionManager::getSingleton().ResetAuctionItemVec();
	 MMOAuctionManager::getSingleton().NotifyUpdateItemCategoryList();
 }

void UMMOGFXPlayer_Auction::ASViewPage_ClickHeaderSort(INT colIdx,UBOOL descending)
{
	MMOAuctionManager::getSingleton().UpdateSearchSortBy(colIdx, descending);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOGFXPlayer_Auction::ASViewPage_AuctionClick(INT dwAuctionID, INT price,INT msgType)
{
	//MMOAuctionManager::getSingleton().BuyFromViewList(dwAuctionID, price);
	if (MMOAuctionManager::getSingleton().IsShowSmg_DefaultPrice(dwAuctionID,price,((Auction_ScreenMsg_Type)0)))
	{
		//_auctionPanel->Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_ViewPage_Auction_OK,msgType);
	}

}

void UMMOGFXPlayer_Auction::ASViewPage_AuctionFixedPriceClick(INT dwAuctionID, int price)
{
	//MMOAuctionManager::getSingleton().BuyFromViewList(dwAuctionID, price);
	//CMessageHandler::GetInstance().Send_AuctionBuy(dwAuctionID, price);
	//_auctionPanel->Show_Auction_Msg(dwAuctionID,price,Auction_MsgType_ViewPage_FixedPrice_OK,0);
}

// Turn page
void UMMOGFXPlayer_Auction::ASViewPage_PrevPageClick(INT pgNum)
{
	MMOAuctionManager::getSingleton().LastActionIsNextPage(false);
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(pgNum);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOGFXPlayer_Auction::ASViewPage_NextPageClick(INT pgNum)
{
	MMOAuctionManager::getSingleton().LastActionIsNextPage(true);
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(pgNum);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}


void UMMOGFXPlayer_Auction::ASViewPage_GotoPageClick(INT pgNum)
{
	MMOAuctionManager::getSingleton().UpdateSearchPageNumb(pgNum);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOGFXPlayer_Auction::ASViewPage_CategoryClick(BYTE mainType, BYTE subType)
{
	
	MMOAuctionManager::getSingleton().OnCategoryClick(mainType,subType);
	
}

void UMMOGFXPlayer_Auction::ASOnPanelClose(const FString& PanelName)
{
	if( PanelName == MMOUI::GetUIManager()->Panel_AuctionView.ToString())
	{
		_auctionPanel->ResetSellItemInfo();
	}
	Super::ASOnPanelClose(PanelName);
}

void UMMOGFXPlayer_Auction::ASTreeListPage_ClickItem(int itemId)
{
	MMOAuctionManager::getSingleton().SetSearchItemId(itemId);
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}

void UMMOGFXPlayer_Auction::ASAuction_Fresh()
{
	MMOAuctionManager::getSingleton().Send_SearchByFilter();
}
void UMMOGFXPlayer_Auction::ASAuction_SearchItem(const FString& itemName)
{
	MMOAuctionManager::getSingleton().SearchItem();
	MMOAuctionManager::getSingleton().NotifySearchTreeItemList();
}

void UMMOGFXPlayer_Auction::ASAuction_FilterTreeList(int type,class UGFxObject* dt)
{
	//if (MMOAuctionManager::getSingleton().GetAuctionItemVec()->size() > 0)
	//{
		MMOAuctionManager::getSingleton().UpdateSearchFilter(dt);
		MMOAuctionManager::getSingleton().NotifyFilterTreeItem((Auction_FilterType)type);
	//}

}

void UMMOGFXPlayer_Auction::ASAuction_ReturnLastList()
{
	MMOAuctionManager::getSingleton().ResetAuctionItemVec();
}

