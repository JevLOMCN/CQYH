#ifndef _GOLDSTORE_MANAGER_H_
#define _GOLDSTORE_MANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameUIClasses.h"
#include "MMODataManager.h"
#include "MMOUtil.h"
#include "net/message_handler.h"

#define DELAYTIME 1

enum EGoldTabType
{
	EGTT_Invalid = -1,
	EGTT_buyView = 0,
	EGTT_historyView = 1,
	EGTT_myOrderView = 2,
};

enum EMoneyOrderType
{
	EMOT_Item = 0,
	EMOT_Money = 1,

};

enum EMyMoneyType
{
	EMMT_Buy = 1,
	EMMT_Sell = 2,
};

enum EGoldViewType
{
	EGVT_Buy = 1,
	EGVT_Sell = 2,
};

struct Client_ExchangeItemInfo 
{
	Client_ExchangeItemInfo() : dwOrderID(0), dwFinishTime(0), byOrderType(0), byOrderStatus(0),
		  nPrice(0), nCount(0), nCurCount(0), nOrderTime(0) {}

	UINT64      dwOrderID;
	UINT32      dwFinishTime;
    UINT8       byOrderType;
	UINT8       byOrderStatus;
    INT32       nPrice;
    INT32       nCount;
    INT32       nCurCount;
	UINT32      nOrderTime;
	
	UINT32 ConvertFromServer_Order(EXCHANGE_ORDER_INFO* info);
};

struct ExchangePriceInfo
{
	ExchangePriceInfo() : nPrice(0), nCount(0), nType(0) {}

	INT32       nPrice;
	INT32       nCount;
	INT         nType;
	UINT32 ConvertFromServer(EXCHANGE_PRICE_INFO* info);
};

struct GoldStoreMyLogsData
{
	GoldStoreMyLogsData() : dwTime(0), byOrderType(0), nPrice(0), nCount(0) {}

	UINT32      dwTime;
	UINT8       byOrderType;
	INT32       nPrice;
	INT32       nCount;
};

typedef std::vector<GoldStoreMyLogsData*, M_UE3_STL_ALLOC(GoldStoreMyLogsData*) > GoldStoreMyLogs;
typedef std::list<ExchangePriceInfo, M_UE3_STL_ALLOC(ExchangePriceInfo)> GoldStoreDataList;
typedef std::list<Client_ExchangeItemInfo, M_UE3_STL_ALLOC(Client_ExchangeItemInfo)> MyOrderDataList;

struct MMOGameEvent_GoldStore_DataUpdate_My : public MMOGameEvent
{
protected:
	const EGameEvent _goldStoreEventType;

public:

	MMOGameEvent_GoldStore_DataUpdate_My(EGameEvent t)
		: _goldStoreEventType(t)
		, GoldStoreListPtr(NULL)

	{}

	virtual EGameEvent GetEventType() const{ return _goldStoreEventType; }

	MyOrderDataList* GoldStoreListPtr;

};

struct MMOGameEvent_GoldStore_UpdateMoney : public MMOGameEvent
{
protected:
	const EGameEvent _goldStoreEventType;

public:

	MMOGameEvent_GoldStore_UpdateMoney(EGameEvent t)
		: _goldStoreEventType(t)
		, playerMoney(0)
		, playerRealMoney(0)

	{}

	virtual EGameEvent GetEventType() const{ return _goldStoreEventType; }

	INT64 playerMoney;
	INT32 playerRealMoney;

};

struct MMOGameEvent_GoldStore_UpdateOrderCount : public MMOGameEvent
{
protected:
	const EGameEvent _goldStoreEventType;

public:

	MMOGameEvent_GoldStore_UpdateOrderCount(EGameEvent t)
		: _goldStoreEventType(t)
		, moneyCount(0)
		, sellMoneyCont(0)

	{}

	virtual EGameEvent GetEventType() const{ return _goldStoreEventType; }

	INT32 moneyCount;
	INT32 sellMoneyCont;

};

struct MMOGameEvent_GoldStore_DataUpdate : public MMOGameEvent
{
protected:
	const EGameEvent _goldStoreEventType;

public:

	MMOGameEvent_GoldStore_DataUpdate(EGameEvent t)
		: _goldStoreEventType(t)
		, GoldStoreListPtr(NULL)

	{}

	virtual EGameEvent GetEventType() const{ return _goldStoreEventType; }

	GoldStoreDataList GoldStoreListPtr;

};

struct MMOGameEvent_GoldStore_DefaultPrice : public MMOGameEvent
{

public:

	MMOGameEvent_GoldStore_DefaultPrice(INT price, INT count)
		: price(price)
		, count(count)

	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_GoldStore_DefaultPrice; }

	INT price;
	INT count;

};



class MMOGoldStoreManager : public SingletonMeyers<MMOGoldStoreManager>, public INativeGameEventHandler
{
protected:
	//TArray<Client_ExchangeItemInfo> _goldMyBuyling;
	//TArray<Client_ExchangeItemInfo> _goldMySelling;
	MyOrderDataList _goldMyOrders;
	GoldStoreDataList _goldStoreBuyItemList;
	GoldStoreDataList _goldStoreSellItemList;
	GoldStoreMyLogs _MyLogsVec;

	INT _currentPage;
	INT _maxPageNum;
	EGoldTabType _currentTabPage;
	bool _isUpdateOrderCount;

	static void addToList(GoldStoreDataList &lst,const ExchangePriceInfo& item);
	static void addToMyList(MyOrderDataList&lst,const Client_ExchangeItemInfo& item);
	static bool removeFromMyList(MyOrderDataList&lst, UINT64 id);
	 
	Client_ExchangeItemInfo* tryFindMyItem(UINT64 id);
	
	Client_ExchangeItemInfo* FindMyGoldStoreItem( MyOrderDataList&lst, UINT64 id);

	bool Contain(TArray<Client_ExchangeItemInfo>&lst,UINT64 id);

	INT mMyCurrentOrderType;
	BOOL mUpdate;
	float delayTime;
	INT mCurrentBuyType;

public:
	MMOGoldStoreManager();
	virtual ~MMOGoldStoreManager();
	void Init();
	void Reset();
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void setCurrentPageNum(INT page){_currentPage = page;}
	INT  getCurrentPageNum(){return _currentPage;}

	void SetCurrentTabPage(EGoldTabType currentTabPage);
	EGoldTabType GetCurrentTabPage() const { return _currentTabPage; }

	void SetIsUpdateOrderCount(bool v){_isUpdateOrderCount = v;}

	void ConvertServerGoldStoreItemList(GoldStoreDataList& arr, UINT8 beginCount,UINT8 endCount, EXCHANGE_PRICE_INFO* info,INT32 type);
	void ConvertServerGoldStoreItemList_My(MyOrderDataList& arr, UINT8 count,EXCHANGE_ORDER_INFO* info);
	//server event 
	void OnServer_AddOrder(EXCHANGE_ORDER_INFO* item);
	void OnServer_CancelSell(UINT64 id);
	void OnServer_Listprices(UINT8 sellCount,UINT8 buyCount,EXCHANGE_PRICE_INFO* info);
	void Onserver_ListMyOrders(UINT8 orderType,UINT8 count,EXCHANGE_ORDER_INFO* info);

	void NotifySellingList();
	void NotifyBuyingList();
	//void NotifyMySellingList();
	//void NotifyMyBuyingList();
	void NotifyMyOrders();
	void NotifyUpdatePlayerMoney();
	void NotifyUpdatePlayerRealMoney(INT32 realMoney);
	void NotifyUpdateMyOrderCount(INT32 count);
	void NotifyUpdateMySellCount(INT32 count);
	void NotifyMyList(UINT8 orderType);

	UGFxObject* CreateGoldStoreItemListDataObject( UMMOGFXPlayer* gfxPlayer, const GoldStoreDataList& itemList);
	UGFxObject* CreateMyGoldStoreItemListDataObject( UMMOGFXPlayer* gfxPlayer,  MyOrderDataList& itemList);

	INT GetMyOrderCount(MyOrderDataList& itemList);
	INT64 GetMyOrderIdByIndex(INT index);
	Client_ExchangeItemInfo* GetMyOrderInfoByIndex(INT index);

	INT GetMyCurrentOrderType(){return mMyCurrentOrderType;}
	void SetMyCurrentOrderType(INT type){mMyCurrentOrderType = type;}

	void Update(float deltaTime);

	INT getCurrentBuyType(){return mCurrentBuyType;}
	void setCurrentBuyType(INT v){mCurrentBuyType = v;}

	void AddMyLogs(INT32 count,EXCHANGE_LOG_INFO* myLogs);
	void NotifyUpdateMyLogs();
	const GoldStoreMyLogs& GetMyLogsVec() const;
	void ClearMylogs();

	static bool CompareMoney_Less(const ExchangePriceInfo& leftData, const ExchangePriceInfo& rightData);
	static bool CompareMoney_Large(const ExchangePriceInfo& leftData, const ExchangePriceInfo& rightData);
	static bool CompareOrderTime(const Client_ExchangeItemInfo& leftData, const Client_ExchangeItemInfo& rightData);

	void GetDefaultPrice(); 
};

#endif