#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "Shop.h"

using namespace ExternalMessage;

//------------------------------------------------------------------------
void CMessageHandler::Send_ViewSoldItems()
{
	C2G_VIEW_SOLD_ITEMS msg;
	msg.wMsg = c2g_view_sold_items;
	_Send(&msg, sizeof(msg));
}
//------------------------------------------------------------------------
void CMessageHandler::Send_BuybackItem(UINT8 bySlot)
{
	C2G_BUY_BACK_ITEM msg;
	msg.wMsg = c2g_buy_back_item;
	msg.bySlot = bySlot;
	_Send(&msg, sizeof(msg));

	// Refresh once more after buying back
	Send_ViewSoldItems();
}

//------------------------------------------------------------------------
void CMessageHandler::Send_RequestShop( UINT32 crc )
{
	C2G_CHECK_NPC_SHOP_DATA msg;
	msg.wMsg = c2g_check_npc_shop_data;
	msg.dwCrc = crc;
	_Send(&msg, sizeof(msg));
}


//blackMarket
void CMessageHandler::Send_BlackMarketListInfo()
{
	C2G_BLACK_MARKET_LIMIT_INFO msg;
	msg.wMsg = c2g_black_market_limit_info;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_BlackMarketItemCoolDown()
{
	C2G_BLACK_MARKET_COOLDOWN_INFO msg;
	msg.wMsg = c2g_black_market_cooldown_info;
	_Send(&msg, sizeof(msg));
	
}
void CMessageHandler::Send_BlackMarketBuy(INT nBlackMarketProductID)
{
	C2G_BLACK_MARKET_BUY msg ;
	msg.wMsg = c2g_black_market_buy;
	msg.nBlackMarketProductID = nBlackMarketProductID;
	_Send(&msg, sizeof(msg));
}
	 
void CMessageHandler::Parse_g2c_black_market_limit_info(const char* buf, UINT32 size)
{
	G2C_BLACK_MARKET_LIMIT_INFO* msg = (G2C_BLACK_MARKET_LIMIT_INFO*)buf;
	if (!buf || size < sizeof(G2C_BLACK_MARKET_LIMIT_INFO))
		return;
	if (msg->nLimitInfoCount > (size - sizeof(*msg)) / sizeof(msg->LimitInfo[0]))
		return;

	size_t offset = 0;

	for (int i = 0; i < msg->nLimitInfoCount; ++i)
	{
		BLACK_MARKET_LIMIT_INFO* blackItem = (BLACK_MARKET_LIMIT_INFO*)(((BYTE*)msg->LimitInfo) + offset);
		ShopManager::getSingleton().UpdateBlackMarketItemListCount(blackItem);
		offset += sizeof(BLACK_MARKET_LIMIT_INFO);
	}

	if (msg->nLimitInfoCount > 0)
	{
		MMOGameEvent_Any evt(GameEvent_ShopUpdate);
		MMOUI::PostGameEvent(&evt);
	}

}

void CMessageHandler::Parse_g2c_black_market_cooldown_info(const char* buf, UINT32 size)
{
	G2C_BLACK_MARKET_COOLDOWN_INFO* msg = (G2C_BLACK_MARKET_COOLDOWN_INFO*)buf;
	if (!buf || size < sizeof(G2C_BLACK_MARKET_COOLDOWN_INFO))
		return;
	if (msg->nCooldownInfoCount > (size - sizeof(*msg)) / sizeof(msg->CooldownInfo[0]))
		return;
	size_t offset = 0;
	for(int i = 0; i < msg->nCooldownInfoCount; i++)
	{
		BLACK_MARKET_COOLDOWN_INFO* coolDownItem = (BLACK_MARKET_COOLDOWN_INFO*)(((BYTE*)msg->CooldownInfo) + offset);
		ShopManager::getSingleton().UpdateBlackMarketItemListCoolDown(coolDownItem);
		offset += sizeof(BLACK_MARKET_COOLDOWN_INFO);
	}
	if (msg->nCooldownInfoCount > 0)
	{
		MMOGameEvent_Any evt(GameEvent_BlackMarket_UpdateItemCoolDown);
		MMOUI::PostGameEvent(&evt);
	}
	
}

void CMessageHandler::Parse_g2c_black_market_buy(const char* buf, UINT32 size)
{
	G2C_BLACK_MARKET_BUY* msg = (G2C_BLACK_MARKET_BUY*)buf;
	if (!buf || size < sizeof(G2C_BLACK_MARKET_BUY))
		return;

	BLACK_MARKET_LIMIT_INFO iteminfo;
	iteminfo.nBlackMarketProductID = msg->nBlackMarketProductID;
	iteminfo.nCount = msg->nCount;
	ShopManager::getSingleton().UpdateBlackMarketItemListCount(&iteminfo);

	MMOGameEvent_BlackMarket_UpdateItemCount evt(msg->nBlackMarketProductID,msg->nCount);
	MMOUI::PostGameEvent(&evt);

	//Purchase succeeded. Query cd
	//Send_BlackMarketItemCoolDown();
}

void CMessageHandler::Parse_g2c_sync_npc_shop_data(const char* buf, UINT32 size)
{
	G2C_SYNC_NPC_SHOP_DATA* msg = (G2C_SYNC_NPC_SHOP_DATA*)buf;
	if (!buf || size < sizeof(G2C_SYNC_NPC_SHOP_DATA))
		return;
	if (msg->dwItemCount > (size - sizeof(*msg)) / sizeof(msg->ShopData[0]))
		return;
	if(!msg)
	{
		return;
	}

	UINT32 curCrc = ShopManager::getSingleton().GetCrc();
	if( curCrc != msg->dwCrc )
	{
		ShopManager::getSingleton().SetCrc(msg->dwCrc);
		ShopManager::getSingleton().ClearCurShop();
		ShopManager::getSingleton().AddShopItem( msg->dwItemCount , msg->ShopData );
		ShopManager::getSingleton().SaveShopData( msg->dwItemCount , msg->ShopData );
	}
	
	//msg->ShopData
}

//------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_view_sold_items(const char* buf, UINT32 size)
{
	G2C_VIEW_SOLD_ITEMS *msg = (G2C_VIEW_SOLD_ITEMS*)buf;
	if (!buf || size < sizeof(G2C_VIEW_SOLD_ITEMS))
		return;

	// Use the Index sent over as the marker
	MMOFacade::getSingleton().ClearSoldItems();

	INT BuffSize = 0;
	
	for (int i = 0; i < msg->byCount; ++i)
	{
        ItemInfo iteminfo;
        if ((UINT32)BuffSize > size - sizeof(*msg))
            return;
        INT itemlen = iteminfo.Load(msg->acItems + BuffSize, size - sizeof(*msg) - BuffSize);
        if (itemlen == 0)
            return;

		BuffSize += itemlen;
		MMOFacade::getSingleton().AddSoldItem(&iteminfo);
	}

	MMOFacade::getSingleton().NotifySoldItemChange();
}

