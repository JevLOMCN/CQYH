class MMOGFXPlayer_Auction extends MMOGFXPlayer
	dependson(MMOUIBase)
	native(UI)
	config(MMOGame);

var MMOUIAuctionView _auctionPanel;

function ASRegisterAuctionView(MMOUIAuctionView mc)
{
	_auctionPanel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_AuctionView);
}

// Search auction info
native final function ASSearch(GFxObject dt);//moved
native final function ASSearch_ResetFilter();//moved

// Tab name changed
native final function ASClickTabPage(int idx);//moved


// Buy page
native final function ASBuyPage_AuctionClick(int dwAuctionID, int price,int msgType);//moved
native final function ASBuyPage_AuctionFixedPriceClick(int dwAuctionID, int price);//moved

native final function ASBuyPage_AuctionFailedClick(int dwAuctionID, int price,int msgType);//moved
native final function ASBuyPage_AuctionFailedFixedPriceClick(int dwAuctionID, int price);//moved


// Sell page
native final function ASSellPage_CancelSellClick(int dwAuctionID);//moved
native final function ASSellPage_AuctionSellItemClick(GFxObject dt);//moved


// Browse page
native final function ASViewPage_ClickHeaderSort(int colIdx, bool descending);//moved
native final function ASViewPage_AuctionClick(int dwAuctionID, int price,int msgType);//moved
native final function ASViewPage_AuctionFixedPriceClick(int dwAuctionID, int price);//moved
native final function ASViewPage_CategoryClick(BYTE mainType, BYTE subType);//moved


// page
native final function ASViewPage_PrevPageClick(int pgNum);//moved
native final function ASViewPage_NextPageClick(int pgNum);//moved
native final function ASViewPage_GotoPageClick(int pgNum);//moved

//closeCancel item lock on close
native function ASOnPanelClose( string PanelName );//moved

native final function ASTreeListPage_ClickItem(int itemId);//moved
native final function ASAuction_Fresh();
native final function ASAuction_SearchItem(string itemName);//moved

native final function ASAuction_FilterTreeList(int type,GFxObject dt);//moved

native final function ASAuction_ReturnLastList();//moved
DefaultProperties
{
	MovieInfoStr="UI_AuctionHouse.UI_AuctionHouse"
	Priority=5
}
