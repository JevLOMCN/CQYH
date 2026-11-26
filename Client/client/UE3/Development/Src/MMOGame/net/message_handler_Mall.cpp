#include "stdafx.h"
#include "message_handler.h"
#include "money_def.h"
#include "MallManager.h"
#include "MallController.h"
#include "MMOGoldStoreManager.h"

using namespace ExternalMessage;

#define  _TEST_MALL_  0


void CMessageHandler::Send_MallAskRealMoney()
{
	C2M_CHECK_REAL_MONEY msg;
	msg.wMsg = c2m_check_real_money;
	_Send( &msg , sizeof(C2M_CHECK_REAL_MONEY) );
}
void CMessageHandler::Send_MallAskCRC(INT nCrc)
{
	C2M_CHECK_REAL_MONEY_SHOP_DATA  msg;
	msg.wMsg = c2m_check_real_money_shop_data;
	msg.dwCrc = nCrc;
	_Send( &msg , sizeof(C2M_CHECK_REAL_MONEY_SHOP_DATA) );
}
void CMessageHandler::Send_MallBuyGoods(INT nGoodsID,INT nNum)
{
	C2M_REAL_MONEY_BUY msg = {};
	msg.wMsg = c2m_real_money_buy;
	msg.nItemID = nGoodsID;
	msg.nCount = nNum;
	ZeroMemory(msg.szRoleName,ROLE_NAME_LEN);
	_Send( &msg , sizeof(C2M_REAL_MONEY_BUY) );
}

void CMessageHandler::Send_RequestExchangeLog()
{
	C2M_REAL_MONEY_SHOP_LOG msg;
	msg.wMsg = c2m_real_money_shop_log;
	_Send( &msg , sizeof(C2M_REAL_MONEY_SHOP_LOG) );
}

void CMessageHandler::Send_MallGiveFriend(FString roleName, INT nGoodsID,INT nNum)
{
	C2M_REAL_MONEY_BUY msg = {};
	msg.wMsg = c2m_real_money_buy;
	msg.nItemID = nGoodsID;
	msg.nCount = nNum;
	strxcpy(msg.szRoleName , *roleName, ROLE_NAME_LEN );
	_Send( &msg , sizeof(C2M_REAL_MONEY_BUY) );
}
void CMessageHandler::Send_MallReqBuyInfo()
{
	C2M_SALE_INFO msg;
	msg.wMsg = c2m_sale_info;
	_Send( &msg , sizeof(C2M_SALE_INFO) );
}
void CMessageHandler::Parse_m2c_sync_real_money(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(M2C_SYNC_REAL_MONEY))
		return;
	M2C_SYNC_REAL_MONEY* msg = (M2C_SYNC_REAL_MONEY*)buf;
	if(msg == NULL)
		return;
	//FString strMoney = UTF8_TO_TCHAR(msg->szBalance);
	//if(strMoney.InStr("|") < 0 || strMoney.GetCharArray().Num() < 4)
	//	return;
	//TCHAR* EndPtr = NULL;
	//INT nMoney = appStrtoi(*strMoney + 4, &EndPtr, 10);

	MallManager::getSingleton().NotifyRealMoney(msg->nBalance);
	MallController::getSingleton().NotifyRealMoneyChange();
	//MMOGoldStoreManager::getSingleton().NotifyUpdatePlayerRealMoney(msg->nBalance);

	MMOGameEvent_Any evt(GameEvent_RealMoneyChange);
	MMOUI::PostGameEvent(&evt);
	
	
}

void CMessageHandler::Parse_m2c_sync_real_money_shop_data(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(M2C_SYNC_REAL_MONEY_SHOP_DATA))
		return;
	M2C_SYNC_REAL_MONEY_SHOP_DATA* msg = (M2C_SYNC_REAL_MONEY_SHOP_DATA*)buf;
	if (msg->dwItemCount > (size - sizeof(*msg)) / sizeof(msg->Items[0]))
		return;

	MallManager::getSingleton().NotifyGoodsList(msg);
	MallController::getSingleton().NotifyGoodsListChange();
}

void CMessageHandler::Parse_m2c_real_money_buy(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(M2C_REAL_MONEY_BUY))
		return;
	M2C_REAL_MONEY_BUY* msg = (M2C_REAL_MONEY_BUY*)buf;
	if(msg == NULL)
		return;
	MallManager::getSingleton().NotifyRealMoney(msg->nMoneyLeft);
	MallController::getSingleton().NotifyBuyResult(msg->nItemID);
}

void CMessageHandler::Parse_m2c_sale_info(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(M2C_SALE_INFO))
		return;
	M2C_SALE_INFO* msg = (M2C_SALE_INFO*)buf;
	if (msg->nSaleInfoCount < 0 || (UINT32)msg->nSaleInfoCount > (size - sizeof(*msg)) / sizeof(msg->stSaleInfo[0]))
		return;
	if(msg == NULL)
		return;
	MallManager::getSingleton().NotifyBuyInfo(msg);
	MallController::getSingleton().NotifyBuyInfoChange();
}

void CMessageHandler::Parse_m2c_real_money_shop_log(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(M2C_REAL_MONEY_SHOP_LOG))
		return;
	M2C_REAL_MONEY_SHOP_LOG* msg = (M2C_REAL_MONEY_SHOP_LOG*)buf;
	if (msg->nShopLogCount < 0 || (UINT32)msg->nShopLogCount > (size - sizeof(*msg)) / sizeof(msg->stShopLog[0]))
		return;
	if(!msg)
		return;

	MallManager::getSingleton().UpdateMoneyLog( msg->stShopLog , msg->nShopLogCount );

	//for ( INT i = 0 ; i < msg->nShopLogCount ; i++ )
	//{
	//	msg->stShopLog[i]
	//}
}