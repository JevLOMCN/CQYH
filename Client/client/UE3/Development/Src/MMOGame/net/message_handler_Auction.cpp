#include "stdafx.h"
#include "message_handler.h"
#include "MMOAuctionManager.h"

void CMessageHandler::Send_AuctionSearch(C2M_AUCTION_SEARCH* dt)
{
	dt->wMsg = c2m_auction_search;
	_Send(dt, sizeof(C2M_AUCTION_SEARCH));
}

void CMessageHandler::Send_AuctionQuery(UINT32 id)
{
	C2M_AUCTION_QUERY msg;
	msg.wMsg = c2m_auction_query;
	msg.dwAuctionID = id;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AuctionSell(C2M_AUCTION_SELL* info)
{
	info->wMsg = c2m_auction_sell;
	_Send(info, sizeof(C2M_AUCTION_SELL));
}

void CMessageHandler::Send_AuctionCancelSell(UINT32 id)
{
	C2M_AUCTION_CANCEL_SELL msg;
	msg.wMsg = c2m_auction_cancel_sell;
	msg.dwAuctionID = id;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AuctionBuy(UINT32 id, INT64 price)
{
	C2M_AUCTION_BUY msg;
	msg.wMsg = c2m_auction_buy;
	msg.dwAuctionID = id;
	msg.llBidPrice = price;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AuctionListMySelling()
{
	C2M_AUCTION_LIST_MY_SELLING msg;
	msg.wMsg = c2m_auction_list_my_selling;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AuctionListMyBuying()
{
	C2M_AUCTION_LIST_MY_BUYING msg;
	msg.wMsg = c2m_auction_list_my_buying;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AuctionQueryByType(UINT8 mainType, UINT8 subType, UINT8 bySortBy, UINT8 byAscending)
{
	C2M_AUCTION_SEARCH_TYPE msg;
	msg.wMsg = c2m_auction_search_type;
	msg.byAuctionType = mainType;
	msg.byAuctionSubType = subType;
	msg.bySortBy = bySortBy;
	msg.byAscending = byAscending;
	_Send(&msg,sizeof(msg));
}
void CMessageHandler::Send_AucrionPayLeftMoney(UINT32 auctionId)
{
	C2M_AUCTION_PAY_LEFT_MONEY msg;
	msg.wMsg = c2m_auction_pay_left_money;
	msg.dwAuctionID = auctionId;
	_Send(&msg,sizeof(msg));
}

void CMessageHandler::Parse_m2c_auction_search(const char* buf, UINT32 size)
{
	M2C_AUCTION_SEARCH* msg = (M2C_AUCTION_SEARCH*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_SEARCH))
		return;
	MMOAuctionManager::getSingleton().OnServer_SearchResult(msg->btAuctionItemCount, (AUCTION_ITEM_INFO*)msg->acAuctionItems, size - sizeof(*msg));
}

void CMessageHandler::Parse_m2c_auction_query(const char* buf, UINT32 size)
{
	M2C_AUCTION_QUERY* msg = (M2C_AUCTION_QUERY*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_QUERY))
		return;
	MMOAuctionManager::getSingleton().OnServer_QueryAuctionItem(msg, size);
}

void CMessageHandler::Parse_m2c_auction_sell(const char* buf, UINT32 size)
{
	M2C_AUCTION_SELL* msg = (M2C_AUCTION_SELL*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_SELL))
		return;
	MMOAuctionManager::getSingleton().OnServer_ItemSell(&msg->stAuction, size - sizeof(*msg) + sizeof(msg->stAuction));
}

void CMessageHandler::Parse_m2c_auction_cancel_sell(const char* buf, UINT32 size)
{
	M2C_AUCTION_CANCEL_SELL* msg = (M2C_AUCTION_CANCEL_SELL*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_CANCEL_SELL))
		return;
	MMOAuctionManager::getSingleton().OnServer_ItemCancelSell(msg->dwAuctionID);
}

void CMessageHandler::Parse_m2c_auction_buy(const char* buf, UINT32 size)
{
	M2C_AUCTION_BUY* msg = (M2C_AUCTION_BUY*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_BUY))
		return;
	MMOAuctionManager::getSingleton().OnServer_ItemBuy(&msg->stAuction, size - sizeof(*msg) + sizeof(msg->stAuction));
}

void CMessageHandler::Parse_m2c_auction_list_my_selling(const char* buf, UINT32 size)
{
	M2C_AUCTION_LIST_MY_SELLING* msg = (M2C_AUCTION_LIST_MY_SELLING*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_LIST_MY_SELLING))
		return;
	MMOAuctionManager::getSingleton().OnServer_ListMySelling(msg->byCount, (AUCTION_ITEM_INFO*)msg->acAuctionItems, size - sizeof(*msg));
}

void CMessageHandler::Parse_m2c_auction_list_my_buying(const char* buf, UINT32 size)
{
	M2C_AUCTION_LIST_MY_BUYING* msg = (M2C_AUCTION_LIST_MY_BUYING*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_LIST_MY_BUYING))
		return;
	MMOAuctionManager::getSingleton().OnServer_ListMyBuying(msg->byCount, (AUCTION_ITEM_INFO*)msg->acAuctionItems, size - sizeof(*msg));
}

void CMessageHandler::Parse_m2c_auction_finished(const char* buf, UINT32 size)
{
	M2C_AUCTION_FINISHED* msg = (M2C_AUCTION_FINISHED*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_FINISHED))
		return;
	MMOAuctionManager::getSingleton().OnServer_AuctionFinish(msg);
}

void CMessageHandler::Parse_m2c_auction_pay_left_money(const char* buf, UINT32 size)
{
	M2C_AUCTION_PAY_LEFT_MONEY* msg = (M2C_AUCTION_PAY_LEFT_MONEY*)buf;
	if (!buf || size < sizeof(M2C_AUCTION_PAY_LEFT_MONEY))
		return;
	MMOAuctionManager::getSingleton().NotifyUpdatePayLeftItem();
}

//void CMessageHandler::Parse_m2c_auction_search(const char* buf,UINT32 size)
//{
//	M2C_AUCTION_SEARCH* msg = (M2C_AUCTION_SEARCH*)buf;
//	MMOAuctionManager::getSingleton().ResetAuctionItemVec();
//	for (int i = 0; i < msg->nItemCount; i++)
//	{
//		MMOAuctionManager::getSingleton().AuctionItemVec_AddItem(msg->nItemID[i]);
//	}
//	MMOAuctionManager::getSingleton().SetFilterCategory(msg->byAuctionType, msg->byAuctionSubType);
//	MMOAuctionManager::getSingleton().NotifySearchTreeItemList();
//	
//
//}
