class MMOUIAuctionView extends MMOUIBaseSlotContainer
	native(UI);

var int CurrentSellingItemContainerID;
var int CurrentSellingItemSlotID;

var GFxObject ConfirmMsgBox;

var float currentTime;
var bool refresh;

var float freshTime;

enum Auction_MsgType
{
	Auction_MsgType_ViewPage_Auction_OK,
	Auction_MsgType_ViewPage_FixedPrice_OK,
	Auction_MsgType_BuyPage_Auction_OK,
	Auction_MsgType_BuyPage_FixedPrice_OK,
	Auction_MsgType_BuyPage_Failed_Auction_OK,
	Auction_MsgType_BuyPage_Failed_FixedPrice_OK,
	Auction_MsgType_CancelSell,
};

enum Auction_MsgTextType
{
	Auction_MsgTextType_Auction,
	Auction_MsgTextType_Fixed_Greaterthan,
	Auction_MsgTextType_Fixed,
};
event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

native function BeforeViewUpdate( float DeltaTime );
native function int  GetSlotType();
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

//  show msg
//native final function Show_Auction_Msg(int dwAuctionID, int price,Auction_MsgType msgType,Auction_MsgTextType textType);
native final function OnAuctionMsg_Cancel(GFxObject mc);

native final function OnViewPage_Auction_OK(GFxObject mc);
native final function OnViewPage_FixedPrice_OK(GFxObject mc);
native final function OnBuyPage_Auction_OK(GFxObject mc);
native final function OnBuyPage_FixedPrice_OK(GFxObject mc);
native final function OnBuyPage_FailedAuction_OK(GFxObject mc);
native final function OnBuyPage_FailedFixedPrice_OK(GFxObject mc);
native final function OnCancelSell_OK(GFxObject mc);
native final function OnViewPage_Auction_PayLeft(GFxObject mc);

native final function OnSellPage_SellItem(GFxObject mc);
//native function string GetMsgText(INT msg);


native final function HideAuction_Msg();

// Search auction info
native final function ASSearch(GFxObject dt);
native final function ASSearch_ResetFilter();

// Tab name changed
native final function ASClickTabPage(int idx);


// Buy page
native final function ASBuyPage_AuctionClick(int dwAuctionID, int price,int msgType);
native final function ASBuyPage_AuctionFixedPriceClick(int dwAuctionID, int price);

native final function ASBuyPage_AuctionFailedClick(int dwAuctionID, int price,int msgType);
native final function ASBuyPage_AuctionFailedFixedPriceClick(int dwAuctionID, int price);


// Sell page
native final function ASSellPage_CancelSellClick(int dwAuctionID);
native final function ASSellPage_AuctionSellItemClick(GFxObject dt);


// Browse page
native final function ASViewPage_ClickHeaderSort(int colIdx, bool descending);
native final function ASViewPage_AuctionClick(int dwAuctionID, int price,int msgType);
native final function ASViewPage_AuctionFixedPriceClick(int dwAuctionID, int price);
native final function ASViewPage_CategoryClick(BYTE mainType, BYTE subType);
native final function ASViewPage_ItemListItemClick(int dwAuctionID);

// page
native final function ASViewPage_PrevPageClick(int pgNum);
native final function ASViewPage_NextPageClick(int pgNum);
native final function ASViewPage_GotoPageClick(int pgNum);

//closeCancel item lock on close
native function ASOnPanelClose( string PanelName );

native final function ASTreeListPage_ClickItem(int itemId);
native final function ASAuction_Fresh();
native final function ASAuction_SearchItem(string itemName);

native final function ASAuction_FilterTreeList(int type,GFxObject dt);

native final function ASAuction_ReturnLastList();

//OrderPage
native final function ASUpdateOrderList(int curPage);
native final function ASOnOrderPay(int id);

event UCSetAuctionOrderPageListData(GfxObject dat)
{
	ActionScriptVoid("UCSetAuctionOrderPageListData");
}

event UCShowTabPage(int idx)
{
	ActionScriptVoid("UCShowTabPage");
}

event UCShowPage(int idx)
{
	ActionScriptVoid("UCShowPage");
}

// view page
		
event UCSetAuctionViewPage_SearchFilterData(GfxObject dat)
{
	ActionScriptVoid("UCSetAuctionViewPage_SearchFilterData");
}

event UCSetAuctionViewPage_ListData( GfxObject dat,bool updateCurrentPage )
{
	ActionScriptVoid("UCSetAuctionViewPage_ListData");
}

		
event UCSetAuctionViewPage_CategoryListData( GfxObject dat )
{
	ActionScriptVoid("UCSetAuctionViewPage_CategoryListData");
}
	
event UCSetAuctionViewPage_PagesInfo(int curPage, int maxPage)
{
	ActionScriptVoid("UCSetAuctionViewPage_PagesInfo");
}

event UCSetDetailPageHint(string info)
{
	ActionScriptVoid("UCSetDetailPageHint");
}
// buy page
event UCSetAuctionBuyPage_ListData(GfxObject dat)
{
	ActionScriptVoid("UCSetAuctionBuyPage_ListData");
}

event UCSetAuctionBuyPage_FailedListData(GfxObject dat)
{
	ActionScriptVoid("UCSetAuctionBuyPage_FailedListData");
}


// selling page
		
event UCSetSellingPage_ListData(GfxObject dat)
{
	ActionScriptVoid("UCSetSellingPage_ListData");
}
	
		
event UCSetSellingPage_SellInfo(GfxObject dat)
{
	ActionScriptVoid("UCSetSellingPage_SellInfo");
}

event UCSetPlayerMoney(string money)
{
	ActionScriptVoid("UCSetPlayerMoney");
}

event UCResetSellItem(int slotType)
{
	ActionScriptVoid("UCResetSellItem");	
}

event UCSetPlayerName(GfxObject dat)
{
	ActionScriptVoid("UCSetPlayerName");
}

event UCUpdateTreeList(GfxObject dat)
{
	ActionScriptVoid("UCUpdateTreeList");
}

event UCShowItemNotice(bool v)
{
	ActionScriptVoid("UCShowItemNotice");
}

event UCSetSearchName(string n)
{
	ActionScriptVoid("UCSetSearchName");
}
event UCSetSellItemTax(string tax)
{
	ActionScriptVoid("UCSetSellItemTax");
}

event UCSetSellTimeAndMoney(GfxObject dat)
{
	ActionScriptVoid("UCSetSellTimeAndMoney");
}

event UCUpdateDetailDataItem(GfxObject dat)
{
	ActionScriptVoid("UCUpdateDetailDataItem");
}

event UCShowLinkItem(int id)
{
	ActionScriptVoid("UCShowLinkItem");
}

DefaultProperties
{
	IsDynamicLoad=true
	CurrentSellingItemContainerID=-1
	CurrentSellingItemSlotID=-1
	
	
}
