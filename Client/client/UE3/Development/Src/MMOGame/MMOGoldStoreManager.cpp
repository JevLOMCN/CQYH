#include "stdafx.h"
#include "MMOGoldStoreManager.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "CommonGameEvents.h"
#include "UITextParser.h"
#include "exchange_def.h"

#define MAX_ORDER_COUNT 100
bool MMOGoldStoreManager::CompareMoney_Less(const ExchangePriceInfo& leftData, const ExchangePriceInfo& rightData)
{
	return leftData.nPrice < rightData.nPrice;
}

bool MMOGoldStoreManager::CompareMoney_Large(const ExchangePriceInfo& leftData, const ExchangePriceInfo& rightData)
{
	return leftData.nPrice > rightData.nPrice;
}

bool MMOGoldStoreManager::CompareOrderTime(const Client_ExchangeItemInfo& leftData, const Client_ExchangeItemInfo& rightData)
{
	if (leftData.dwFinishTime != 0 )
	{
		return leftData.dwFinishTime > rightData.dwFinishTime;
	}
	else if(rightData.dwFinishTime != 0)
	{
		return leftData.dwFinishTime > rightData.dwFinishTime;
	}
	else
	{
		return leftData.nOrderTime > rightData.nOrderTime;
	}
	
}

UINT32 Client_ExchangeItemInfo::ConvertFromServer_Order(EXCHANGE_ORDER_INFO* info)
{

	dwOrderID = info->qwOrderID;
	nOrderTime = info->dwOrderTime;
	byOrderType = info->byOrderType;
	byOrderStatus = info->byOrderStatus;
	nPrice = info->nPrice;
	nCount = info->nCount;
	nCurCount = info->nCurCount;
	
	return sizeof(EXCHANGE_ORDER_INFO);
}

UINT32 ExchangePriceInfo::ConvertFromServer(EXCHANGE_PRICE_INFO* info)
{
	nPrice = info->nPrice;
	nCount = info->nCount;
	return sizeof(EXCHANGE_PRICE_INFO);
}
//--------------------------------------------------------------
MMOGoldStoreManager::MMOGoldStoreManager()
	:_currentPage(0)
	,_maxPageNum(0)
	,_currentTabPage(EGTT_Invalid)
	,_isUpdateOrderCount(FALSE)
{
	
}


MMOGoldStoreManager::~MMOGoldStoreManager()
{
}

void MMOGoldStoreManager::Init()
{
	Reset();
	mUpdate = FALSE;
	mMyCurrentOrderType = EMMT_Buy;
	mCurrentBuyType = EGVT_Buy;
}

void MMOGoldStoreManager::Reset()
{
	_goldStoreBuyItemList.clear();
	_goldStoreSellItemList.clear();
	//_goldMySelling.Empty();
	//_goldMyBuyling.Empty();
	_goldMyOrders.clear();
	_currentPage = 0;
	_maxPageNum = 0;
	_currentTabPage = EGTT_Invalid;
	_isUpdateOrderCount = FALSE;
}

void MMOGoldStoreManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}

void MMOGoldStoreManager::addToList(GoldStoreDataList &lst,const ExchangePriceInfo& item)
{
	GoldStoreDataList::iterator it = lst.begin();

	for (; it != lst.end(); ++it)
	{
		if ((*it).nPrice == item.nPrice)
		{
			(*it) = item;
			return;
		}
	}

	lst.push_back(item);
}


void MMOGoldStoreManager::addToMyList(MyOrderDataList&lst,const Client_ExchangeItemInfo& item)
{
	MyOrderDataList::iterator itor = lst.begin();

	while(itor != lst.end())
	{
		if((*itor).dwOrderID == item.dwOrderID)
		{
			(*itor) = item;
			return;
		}
		itor++;
	}

	lst.push_back(item);
}

bool MMOGoldStoreManager::removeFromMyList(MyOrderDataList&lst,UINT64 id)
{
	MyOrderDataList::iterator itor = lst.begin();
	while(itor != lst.end())
	{
		if((*itor).dwOrderID == id)
		{
			lst.erase(itor);
			return true;
		}
		itor++;
	}
	return false;

	//for(int i = 0; i < lst.Num(); ++i)
	//{
	//	if (lst(i).dwOrderID == id)
	//	{
	//		lst.Remove(i);
	//		return true;
	//	}
	//}
	//return false;
}


Client_ExchangeItemInfo* MMOGoldStoreManager::tryFindMyItem(UINT64 id) 
{
	 Client_ExchangeItemInfo* item = FindMyGoldStoreItem(_goldMyOrders,id);
	if (item)
	{
		return item;
	}

	/*{
		const Client_ExchangeItemInfo* item = FindMyGoldStoreItem(_goldMySelling,id);
		if (item)
		{
			return item;
		}
	}*/
	return NULL;
}


Client_ExchangeItemInfo* MMOGoldStoreManager::FindMyGoldStoreItem( MyOrderDataList&lst, UINT64 id)
{
	MyOrderDataList::iterator itor = lst.begin();

	while(itor != lst.end())
	{
		if((*itor).dwOrderID == id)
		{
			return &(*itor);
		}
		itor++;
	}

	return NULL;

	/*for(int i = 0; i < lst.Num(); i++)
	{
		if (lst(i).dwOrderID == id)
		{
			return &lst(i);
		}
	}
	return NULL;*/
}

void MMOGoldStoreManager::SetCurrentTabPage(EGoldTabType currentTabPage)
{
	/*if (_currentTabPage == currentTabPage)
	{
	return;
	}*/

	_currentTabPage = currentTabPage;

	switch(_currentTabPage)
	{
	case EGTT_buyView:
		{
			CMessageHandler::GetInstance().Send_ListPrice();
			_isUpdateOrderCount = FALSE;
		}
		break;
	case EGTT_historyView:
		{
			CMessageHandler::GetInstance().Send_List_MyLogs();
			_isUpdateOrderCount = FALSE;
		}
		break;
	case EGTT_myOrderView:
		{
			//CMessageHandler::GetInstance().Send_List_MyOrder(eotBuy,EMOT_Money);
			CMessageHandler::GetInstance().Send_List_MyOrder();
			_isUpdateOrderCount = TRUE;
		}
		break;
	}
}

bool MMOGoldStoreManager::Contain(TArray<Client_ExchangeItemInfo>&lst,UINT64 id)
{
	for(int i = 0; i < lst.Num(); ++i)
	{
		if (id == lst(i).dwOrderID)
		{
			return true;
		}
	}
	return false;
}

void MMOGoldStoreManager::Update(float t)
{
	CTRL_TICK("MMOGoldStoreManager");

	if (mUpdate)
	{
		if (delayTime >= DELAYTIME)
		{
			if (_currentTabPage == EGTT_buyView )
			{
				//if (mCurrentBuyType == EGVT_Buy)
				//{
					//CMessageHandler::GetInstance().Send_ListPrice(eotBuy,MONEYTYPE_02);
				//}
				//else
				//{
					//CMessageHandler::GetInstance().Send_ListPrice(eotSell,MONEYTYPE_02);
				//}
				
			}
			delayTime = 0;
			mUpdate = FALSE;
		}

		delayTime += t;
	}
	
}

void MMOGoldStoreManager::OnServer_AddOrder(EXCHANGE_ORDER_INFO* item)
{
	//MMOGameEvent_GoldStore_DataUpdate_My evt(GameEvent_GoldStore_AddOrder);

	//Client_ExchangeItemInfo info;
	//info.ConvertFromServer_Order(item);

	//switch(mCurrentBuyType)
	//{
	//case EGVT_Buy:
	//	{
	//		_goldMyBuyling.AddItem(info);
	//		evt.GoldStoreListPtr = &_goldMyBuyling;
	//		//CMessageHandler::GetInstance().Send_ListPrice(eotSell,MONEYTYPE_02);
	//	}
	//	break;
	//case EGVT_Sell:
	//	{
	//		_goldMySelling.AddItem(info);
	//		evt.GoldStoreListPtr = &_goldMySelling;
	//		//CMessageHandler::GetInstance().Send_ListPrice(eotBuy,MONEYTYPE_02);
	//	}
	//	break;
	//}
	/*if (info.byOrderStatus != eosEffective)
	{
	info.dwFinishTime = 0;
	}
	else
	{
	__time32_t endTime = info.nOrderTime + EXCHANGE_TIME;
	_localtime32(&endTime);

	__time32_t timeLeft;
	_time32(&timeLeft);
	_localtime32(&timeLeft);

	info.dwFinishTime = (endTime - timeLeft) <= 0 ? 0 : (endTime - timeLeft);
	}
	_goldMyOrders.push_back(info);
	evt.GoldStoreListPtr = &_goldMyOrders;

	MMOUI::PostGameEvent(&evt);*/

	MMOUI::GetUIManager()->delegateAddScreenMessage(
		MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Exchange.Order.OK")),MsgInfoType_Normal);

	CMessageHandler::GetInstance().Send_ListPrice();

	mUpdate = TRUE;
}

void MMOGoldStoreManager::OnServer_CancelSell(UINT64 nId)
{
	/*bool isInMyBuyingList = false;
	bool isInMySellingList = false;*/

	//MMOGameEvent_GoldStore_DataUpdate_My evt(GameEvent_GoldStore_CancelSell);
	/*isInMyBuyingList = removeFromMyList(_goldMyBuyling,nId);
	isInMySellingList = removeFromMyList(_goldMySelling,nId);

	if (isInMyBuyingList)
	{
		evt.GoldStoreListPtr = &_goldMyBuyling;
		NotifyUpdateMyOrderCount(GetMyOrderCount(_goldMyBuyling));
	}
	else if (isInMySellingList)
	{
		evt.GoldStoreListPtr = &_goldMySelling;
		NotifyUpdateMySellCount(GetMyOrderCount(_goldMySelling));
	}
	else
	{
		return;
	}*/
	
	/*if(removeFromMyList(_goldMyOrders,nId))
	{
	evt.GoldStoreListPtr = &_goldMyOrders;
	NotifyUpdateMyOrderCount(GetMyOrderCount(_goldMyOrders));
	}
	else
	return;

	MMOUI::PostGameEvent(&evt);*/

	CMessageHandler::GetInstance().Send_List_MyOrder();

	MMOUI::GetUIManager()->delegateAddScreenMessage(
		MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Exchange.Sell.Cancel")),MsgInfoType_Normal);
}

void MMOGoldStoreManager::OnServer_Listprices(UINT8 sellCount,UINT8 buyCount,EXCHANGE_PRICE_INFO* info)
{
	_goldStoreSellItemList.clear();
	_goldStoreBuyItemList.clear();

	size_t offset = 0;

	for (INT i = 0; i < (sellCount + buyCount); i++)
	{
		ExchangePriceInfo itemInfo;
		EXCHANGE_PRICE_INFO* priceInfo = (EXCHANGE_PRICE_INFO*)(((BYTE*)info) + offset);
		offset += itemInfo.ConvertFromServer(priceInfo);

		if(i < sellCount)
		{
			itemInfo.nType = eotSell;
			_goldStoreBuyItemList.push_back(itemInfo);
		}
		else if( i >= sellCount)
		{
			itemInfo.nType = eotBuy;
			_goldStoreSellItemList.push_back(itemInfo);
		}
		

	}
	_goldStoreSellItemList.sort(CompareMoney_Large);
	_goldStoreBuyItemList.sort(CompareMoney_Less);
	NotifySellingList();
	NotifyBuyingList();
}

void MMOGoldStoreManager::ConvertServerGoldStoreItemList(GoldStoreDataList& arr, UINT8 beginCount,UINT8 endCount,EXCHANGE_PRICE_INFO* info,INT32 type )
{
	arr.clear();
	size_t offset = 0;

	for (INT i = beginCount; i < endCount; ++i)
	{	
		ExchangePriceInfo itemInfo;
		EXCHANGE_PRICE_INFO* priceInfo = (EXCHANGE_PRICE_INFO*)(((BYTE*)info) + offset);
		offset += itemInfo.ConvertFromServer(priceInfo);
		itemInfo.nType = type;
		arr.push_back(itemInfo);
	}
}

void MMOGoldStoreManager::ConvertServerGoldStoreItemList_My(MyOrderDataList& arr, UINT8 count,EXCHANGE_ORDER_INFO* info)
{
	arr.clear();

	/*__time32_t timeLeft;
	_time32(&timeLeft);
	_localtime32(&timeLeft);*/

	size_t offset = 0;

	if (count >= MAX_ORDER_COUNT)
	{
		count = MAX_ORDER_COUNT;
	}

	for (INT i = 0; i < count; ++i)
	{
		Client_ExchangeItemInfo itemInfo;
		EXCHANGE_ORDER_INFO* orderInfo = (EXCHANGE_ORDER_INFO*)(((BYTE*)info) + offset);

		offset += itemInfo.ConvertFromServer_Order(orderInfo);

		/*__time32_t endTime = orderInfo->dwFinishTime;
		_localtime32(&endTime);

		if (timeLeft >= endTime)
		{
			continue;
		}*/
		if (itemInfo.byOrderStatus != eosEffective)
		{
			itemInfo.dwFinishTime = 0;
		}
		else
		{
			__time32_t endTime = itemInfo.nOrderTime + EXCHANGE_TIME;
			_localtime32(&endTime);

			__time32_t timeLeft;
			_time32(&timeLeft);
			_localtime32(&timeLeft);

			itemInfo.dwFinishTime = (endTime - timeLeft) <= 0 ? 0 : (endTime - timeLeft);
		}

		arr.push_back(itemInfo);
	}

	//Sort
	arr.sort(CompareOrderTime);
}


void MMOGoldStoreManager::NotifyBuyingList()
{
	MMOGameEvent_GoldStore_DataUpdate evt(GameEvent_GoldStore_ListBuying);
	evt.GoldStoreListPtr = _goldStoreBuyItemList;
	MMOUI::PostGameEvent(&evt);
}

void MMOGoldStoreManager::NotifySellingList()
{
	MMOGameEvent_GoldStore_DataUpdate evt(GameEvent_GoldStore_ListSelling);
	evt.GoldStoreListPtr = _goldStoreSellItemList;
	MMOUI::PostGameEvent(&evt);
}

void MMOGoldStoreManager::Onserver_ListMyOrders(UINT8 orderType,UINT8 count,EXCHANGE_ORDER_INFO* info)
{
	//switch (orderType)
	//{
	//case eotBuy:
	//	{
	//		ConvertServerGoldStoreItemList_My(_goldMyBuyling, count, info);
	//		//NotifyMyBuyingList();
	//	}
	//	break;
	//case eotSell:
	//	{
	//		ConvertServerGoldStoreItemList_My(_goldMySelling, count, info);
	//		//NotifyMySellingList();
	//	}
	//	break;
	//}
	
	ConvertServerGoldStoreItemList_My(_goldMyOrders,count,info);
	NotifyMyList(orderType);
}

void MMOGoldStoreManager::NotifyMyList(UINT8 orderType)
{
	/*switch(mMyCurrentOrderType)
	{
	case eotBuy:
	NotifyMyBuyingList();
	break;
	case eotSell:
	NotifyMySellingList();
	break;
	}*/
	NotifyMyOrders();
	NotifyUpdateMyOrderCount(GetMyOrderCount(_goldMyOrders));
	//NotifyUpdateMySellCount(GetMyOrderCount(_goldMySelling));
	
}

//void MMOGoldStoreManager::NotifyMySellingList()
//{
//	MMOGameEvent_GoldStore_DataUpdate_My evt(GameEvent_GoldStore_ListSelling_My);
//	evt.GoldStoreListPtr = &_goldMySelling;
//	MMOUI::PostGameEvent(&evt);
//}
//void MMOGoldStoreManager::NotifyMyBuyingList()
//{
//	MMOGameEvent_GoldStore_DataUpdate_My evt(GameEvent_GoldStore_ListBuying_My);
//	evt.GoldStoreListPtr = &_goldMyBuyling;
//	MMOUI::PostGameEvent(&evt);
//}

UGFxObject* MMOGoldStoreManager::CreateGoldStoreItemListDataObject( UMMOGFXPlayer* gfxPlayer, const GoldStoreDataList& itemList)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;

	UINT32 timeLeft = time(0);
	INT idx = 0;

	GoldStoreDataList::const_iterator it = itemList.begin();
	for (; it != itemList.end(); ++it)
	{	
		const ExchangePriceInfo priceInfo = *it;

		UGFxObject*  dat = gfxPlayer->CreateObject(TEXT("UI_GoldStore.AS.GoldStoreItemData"));
		//check(dat);
		if(dat == NULL)
			continue;
		dat->SetInt(TEXT("GoldStoreID"), idx);
		dat->SetInt(TEXT("Coupon"), priceInfo.nPrice);
		dat->SetInt(TEXT("Count"),priceInfo.nCount);
		dat->SetInt(TEXT("times"),0);
		dat->SetInt(TEXT("ExchangeType"),priceInfo.nType);
		if(priceInfo.nType == eotSell) 
		{
			dat->SetString(TEXT("ExchangeTypeStr"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.ItemBuy")));
		}
		else
		{
			dat->SetString(TEXT("ExchangeTypeStr"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.ItemSell")));
		}
		
		datList->SetElementObject(idx, dat);
		++idx;
	}
	return datList;
}


UGFxObject* MMOGoldStoreManager::CreateMyGoldStoreItemListDataObject( UMMOGFXPlayer* gfxPlayer, MyOrderDataList& itemList)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;

	UINT32 timeLeft = time(0);
	INT idx = 0;

	MyOrderDataList::iterator itor = itemList.begin();

	while(itor != itemList.end())
	{
		 Client_ExchangeItemInfo info = (*itor);

		//__time32_t endTime = info.nOrderTime + EXCHANGE_TIME;
		//_localtime32(&endTime);

		//__time32_t timeLeft;
		//_time32(&timeLeft);
		//_localtime32(&timeLeft);

		/*if (timeLeft >= endTime)
		{
		continue;
		}*/

		UGFxObject*  dat = gfxPlayer->CreateObject(TEXT("UI_GoldStore.AS.GoldStoreItemData"));
		//check(dat);
		if(dat == NULL)
			continue;

		//INT time = (endTime - timeLeft) <= 0 ? 0 : (endTime - timeLeft);

		dat->SetInt(TEXT("GoldStoreID"), idx);
		dat->SetInt(TEXT("GoldStoreID_High"), info.dwOrderID >> 32);
		dat->SetInt(TEXT("GoldStoreID_Low"), info.dwOrderID);
		dat->SetInt(TEXT("Coupon"), info.nPrice);
		dat->SetInt(TEXT("Count"),info.nCount);
		dat->SetInt(TEXT("dealTime"),info.nOrderTime);
	//	dat->SetInt(TEXT("orderTime"),info.dwFinishTime);
		dat->SetInt(TEXT("times"), info.dwFinishTime);
		dat->SetInt(TEXT("orderState"),info.byOrderStatus);
		dat->SetInt(TEXT("CurCount"),info.nCurCount);
		dat->SetInt(TEXT("ExchangeType"),info.byOrderType);

		if(info.byOrderType == eotSell) 
		{
			dat->SetString(TEXT("ExchangeTypeStr"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.ItemBuy")));
		}
		else
		{
			dat->SetString(TEXT("ExchangeTypeStr"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MoneyExchange.ItemSell")));
		}

		datList->SetElementObject(idx, dat);
		++idx;

		itor++;
	}

	return datList;
}

void MMOGoldStoreManager::NotifyUpdatePlayerMoney()
{
	MMOGameEvent_GoldStore_UpdateMoney evt(GameEvent_GoldStore_UpdatePlayerMoney);
	evt.playerMoney = MMOFacade::getSingleton().GetPlayerMoney64(E_MONEYTYPE_02);
	MMOUI::PostGameEvent(&evt);
}
void MMOGoldStoreManager::NotifyUpdatePlayerRealMoney(INT32 realMoney)
{
	MMOGameEvent_GoldStore_UpdateMoney evt(GameEvent_GoldStore_UpdatePlayerRealMoney);
	evt.playerRealMoney = realMoney;
	MMOUI::PostGameEvent(&evt);
}

void MMOGoldStoreManager::NotifyUpdateMyOrderCount(INT32 count)
{
	MMOGameEvent_GoldStore_UpdateOrderCount evt(GameEvent_GoldStore_UpdateMyOrderCount);
	evt.moneyCount = count;
	MMOUI::PostGameEvent(&evt);
}

void MMOGoldStoreManager::NotifyUpdateMySellCount(INT32 count)
{
	MMOGameEvent_GoldStore_UpdateOrderCount evt(GameEvent_GoldStore_UpdateMySellCount);
	evt.sellMoneyCont = count;
	MMOUI::PostGameEvent(&evt);
}

INT MMOGoldStoreManager::GetMyOrderCount(MyOrderDataList& itemList)
{
	INT totalNumber = 0;

	//for (int i = 0; i < itemList.Num(); i++)
	//{
	//	Client_ExchangeItemInfo itemInfo = itemList(i);
	//	totalNumber += itemInfo.nCount;
	//}
	return totalNumber;
}

void MMOGoldStoreManager::ClearMylogs()
{
	for (INT i = 0; i < (INT)_MyLogsVec.size(); ++i)
	{
		if (_MyLogsVec[i] != NULL)
		{
			delete _MyLogsVec[i];
		}
	}
	_MyLogsVec.clear();
}

void MMOGoldStoreManager::AddMyLogs(INT32 count,EXCHANGE_LOG_INFO* myLogs)
{
	ClearMylogs();
	for(int i = 0; i < count; i++)
	{
		GoldStoreMyLogsData* logs = new GoldStoreMyLogsData();
		
		__time32_t lastTime = myLogs[i].dwTime;
		_localtime32(&lastTime);

		logs->dwTime = lastTime;
		logs->byOrderType = myLogs[i].byOrderType;
		logs->nCount = myLogs[i].nCount;
		logs->nPrice = myLogs[i].nPrice;

		_MyLogsVec.push_back(logs);
	}
}
void MMOGoldStoreManager::NotifyUpdateMyLogs()
{
	MMOGameEvent_Any evt(GameEvent_GoldStore_UpdateMyLogs);
	MMOUI::PostGameEvent(&evt);
}
const GoldStoreMyLogs& MMOGoldStoreManager::GetMyLogsVec() const
{
	return _MyLogsVec;
}

void MMOGoldStoreManager::GetDefaultPrice()
{
	INT price = 0;
	INT count = 0;
	GoldStoreDataList::iterator it ;
	GoldStoreDataList list;
	if (mCurrentBuyType == EGVT_Buy)
	{
		list = _goldStoreBuyItemList;
	}
	else
	{
		list = _goldStoreSellItemList;
	}

	it = list.begin();
	if (it != list.end())
	{
		price = (*it).nPrice;
		count = (*it).nCount;
	}

	MMOGameEvent_GoldStore_DefaultPrice evt (price,count);
	MMOUI::PostGameEvent(&evt);

}
void MMOGoldStoreManager::NotifyMyOrders()
{
	MMOGameEvent_GoldStore_DataUpdate_My evt(GameEvent_GoldStore_ListMyOrder);
	evt.GoldStoreListPtr = &_goldMyOrders;
	MMOUI::PostGameEvent(&evt);
}

INT64 MMOGoldStoreManager::GetMyOrderIdByIndex(INT index)
{
	MyOrderDataList::iterator itor = _goldMyOrders.begin();
	for (INT i = 0; itor != _goldMyOrders.end(); i++)
	{
		if (i == index )
		{
			return (*itor).dwOrderID;
		}
		itor++;
	}
	return -1;
}

Client_ExchangeItemInfo* MMOGoldStoreManager::GetMyOrderInfoByIndex(INT index)
{

	MyOrderDataList::iterator itor = _goldMyOrders.begin();
	for (INT i = 0; itor != _goldMyOrders.end(); i++)
	{
		if (i == index )
		{
			return &(*itor);
		}
		itor++;
	}
	return NULL;
}