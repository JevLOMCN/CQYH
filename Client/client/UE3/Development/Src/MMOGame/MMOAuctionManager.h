#ifndef _AUCTION_MANAGER_H_
#define _AUCTION_MANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameUIClasses.h"
#include "MMODataManager.h"
#include "MMOUtil.h"
#include "auction_def.h"

#if SHIPPING_PC_GAME
#	define M_DEBUG_AUCTION 0
#elif defined(_DEBUG)
#	define M_DEBUG_AUCTION 1
#else
#	define M_DEBUG_AUCTION 0
#endif

#if M_DEBUG_AUCTION
#	define M_LogAuction warnf
#else
#	define M_LogAuction(...)
#endif

#define INVALID_SELECTEDINDEX -1
#define INVALID_PROFESSION 0

#define MONEY_RATIO 10000

#define AUCTION_SELL_CONFIG 6

enum EAuctionTabId
{
	EATID_Invalid = -1,
	EATID_View = 0,
	EATID_Buy = 1,
	EATID_Sell = 2,
	EATID_Order = 3,
};

enum Auction_ScreenMsg_Type
{
	AS_Type_ViewPage,
	AS_Type_BuyPage,
	AS_Type_BuyFailedPage,
};

enum Auction_FilterType
{
	EAFT_Invalid = -1,
	EAFT_Profession = 1,
	EAFT_ItemColor = 2,
	EAFT_Level = 3,
	EAFT_Name = 4,
	EAFT_ALL = 5,
};

enum Auction_SearchTyep
{
	EAST_Invalid = -1,
	EAST_ItemId,
	EAST_ItemType,
};

enum Auction_OrderPageType
{
	EOPT_Buy = 1,
	EOPT_Sell = 2,
};

enum Auction_MsgBoxType
{
	EAMB_Normal,
	EAMB_Big,
};
class AuctionSearchFilter
{
public:

	BYTE	MainType;		// main type (EAuctionTypeMain)
	BYTE	SubType;		// sub type (EAuctionTypeSub_Weapon_Warrior, EAuctionTypeSub_Weapon_assassin, ...)

	FString	SearchName;		// search name
	TArray<INT> FindIDList;
	BYTE	PlayerClass;	// class
	
	// Level range
	INT		LevelMin;
	INT		LevelMax;
	
	INT		ItemColor;		// quality
	
	EAuctionStatus AuctionState;	// status
	//INT		SearchPageNumb;
	UINT8	SortType;		// sort type
	UBOOL	SortAscendent;	// ascending/descending

	Auction_SearchTyep searchType;

	AuctionSearchFilter();
	void Reset();
	UGFxObject* ToGfx(UGFxMoviePlayer* gfxPlayer) const;
	void FromGfx(UGFxObject* obj);
};

struct ClientAuctionItemInfo : public FClientItemInfo
{
	UINT32      dwAuctionID;
	UINT32      dwSellerRoleID;
	UINT32      dwBuyerRoleID;
	//INT32       nSellerLevel;
	INT64       nPrice;
	INT64       nBuyoutPrice;
	INT64       nDepositPrice; //deposit
	UINT32      dwStartTime;
	UINT32      dwFinishTime;
	UINT8       byAuctionStatus;
	UItem*		ItemTemplate;

	UINT32 ConvertFromServer(AUCTION_ITEM_INFO* info);

	INT64 Inc5Percent(INT64 price) const
	{
		//LOG_PROCESS_ERROR(price > 0);

		//if (price < 20)
			//return price + 1;
		//else
		return /*price + */ceil(price * 1.05);

//Exit0:
		//return 1;
	}

	INT64 GetNextPrice() const
	{
		INT64 nextprice = nPrice;//Inc5Percent(nPrice);


		if (nBuyoutPrice && nextprice > nBuyoutPrice)
			return nBuyoutPrice;
		else
			return nextprice;
	}
};

struct QueryAuctionItemInfo 
{
	UINT32      dwAuctionID;
	UINT8       byFound;
	UINT32      dwBuyerCount;
	FString     szSellerName;
	UINT8       bySellerLevel;
	FString     bySellerClass;
	FString     szGuildName;
	ClientAuctionItemInfo info;
};

struct AuctionSellInfo
{
	UINT8       byConSlot;
	UINT8       byItemSlot;
	UINT8       byAuctionTimeType;
	UINT8       byOpen;
	INT64       llPrice;
	INT64       llBuyoutPrice;
	INT64       llDepositePrice;
};

typedef std::vector<INT> AuctionItemVec;
typedef std::map<BYTE,AuctionItemVec*> AuctionItemMap_Sub;
typedef std::map<BYTE,AuctionItemMap_Sub*> AuctionItemMap;

typedef std::list<ClientAuctionItemInfo, M_UE3_STL_ALLOC(ClientAuctionItemInfo)>AuctionItemList;
//
// MMOGameEvent_Auction_DataUpdate
//
struct MMOGameEvent_Auction_DataUpdate : public MMOGameEvent
{
protected:
	const EGameEvent _auctionEventType;

public:

	MMOGameEvent_Auction_DataUpdate(EGameEvent t)
		: _auctionEventType(t)
		, AuctionItemListPtr(NULL)
		, CurPageNum(1)
		, MaxPageNum(1)
		,filterType(EAFT_Invalid)
		,bUpdateCurrentPage(FALSE)
	{}

	virtual EGameEvent GetEventType() const{ return _auctionEventType; }

	AuctionItemList* AuctionItemListPtr;

	INT CurPageNum;
	INT MaxPageNum;

	BOOL bUpdateCurrentPage;

	Auction_FilterType filterType;
};


struct MMOGameEvent_Auction_UpdatePlayerName : public MMOGameEvent
{
public:

	MMOGameEvent_Auction_UpdatePlayerName(INT playerID, const FString& playerName)
		: PlayerID(playerID)
		, PlayerName(playerName)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_Auction_PlayerNameGot; }

	INT PlayerID;
	FString PlayerName;
};

struct MMOGameEvent_Auction_QuickSearchByBag : public MMOGameEvent
{
public:

	MMOGameEvent_Auction_QuickSearchByBag(BYTE slotType,BYTE containerID,INT slotID)
		: SlotType(slotType)
		, ContainerID(containerID)
		, SlotID(slotID)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_Auction_QuickSearchByBag; }

	BYTE SlotType;
	BYTE ContainerID;
	INT SlotID;
};

struct MMOGameEvent_Auction_QuickSearchByChat : public MMOGameEvent
{
public:

	MMOGameEvent_Auction_QuickSearchByChat(UINT32 id)
		: itemId(id)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_Auction_QuickSearchByChat; }

	UINT32 itemId;
};

struct FStringValuePair
{
	FString StringVal;
	BYTE Val;

	FStringValuePair()
		: Val(-1)
	{
	}

	bool IsValid()
	{
		return Val != (BYTE)-1;
	}
};

//
// FAuctionCategoryItem
//
struct FAuctionCategoryItem
{
	FStringValuePair MainType;

	TArray<FStringValuePair> SubType;
};

struct FAuctionCategoryListProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("AuctionCategory.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FAuctionCategoryItem* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(FAuctionCategoryItem* obj)
	{
		delete obj;
	}

	static void ParseData(FAuctionCategoryItem* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("Index");
	}

	static FString FormatAsString(FAuctionCategoryItem* obj)
	{
		return FString::Printf(TEXT("FAuctionCategoryItem[%s]%d"), *obj->MainType.StringVal, obj->MainType.Val);
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FAuctionCategoryItem* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

class AuctionCategoryList : public TDataManger<FAuctionCategoryItem, FAuctionCategoryListProcessor>
{
public:

	AuctionCategoryList(){}
	virtual ~AuctionCategoryList(){}
};

//
// MMOAuctionManager
//
class MMOAuctionManager : public SingletonMeyers<MMOAuctionManager>, public INativeGameEventHandler
{
public:
	MMOAuctionManager();
	virtual ~MMOAuctionManager();

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void Init();
	void Shutdown();

	void LoadFailedList();
	void SaveFailedList();

	void RequistInitUIData();

	// Called after login succeeds or after switching character
	void Reset();

	void ResetPageNumber();

	void SetFilterCategory(BYTE mainType, BYTE subType);
	void UpdateSearchFilter(UGFxObject* dt);
	void UpdateSearchSortBy(INT colIdx, UBOOL decending);
	void UpdateSearchPageNumb(INT numb);
	void Send_SearchByFilter();

	static void ConvertServerAuctionItemList(AuctionItemList& arr, UINT8 count, AUCTION_ITEM_INFO* serverItemList);
	static UGFxObject* CreateAuctionItemListDataObject(UMMOGFXPlayer* gfxPlayer, const AuctionItemList& itemList);
	static UGFxObject* CreateAuctionItemCategoryListDataObject(UMMOGFXPlayer* gfxPlayer);
	static UGFxObject* CreatePlayerNameDataObject(UMMOGFXPlayer* gfxPlayer, INT id, const FString& name);

	static UINT8 ConvertColIndexToServerSortBy(INT idx);


	void OnServer_SearchResult(UINT8 count, AUCTION_ITEM_INFO* serverItemList);
	void OnServer_QueryAuctionItem(M2C_AUCTION_QUERY* item);
	void OnServer_ItemSell(AUCTION_ITEM_INFO* item);
	void OnServer_ItemCancelSell(UINT32 id);
	void OnServer_ItemBuy(AUCTION_ITEM_INFO* item);
	void OnServer_ListMySelling(UINT8 count, AUCTION_ITEM_INFO* serverItemList);
	void OnServer_ListMyBuying(UINT8 count, AUCTION_ITEM_INFO* serverItemList);
	void OnServer_AuctionFinish(M2C_AUCTION_FINISHED* msg);


	UGFxObject* CreateViewListItemDataObject(UMMOGFXPlayer* gfxPlayer) const;
	UGFxObject* CreateMyBuyingListItemDataObject(UMMOGFXPlayer* gfxPlayer) const;
	UGFxObject* CreateMyBuyingFailedListItemDataObject(UMMOGFXPlayer* gfxPlayer) const;
	UGFxObject* CreateMySellingListItemDataObject(UMMOGFXPlayer* gfxPlayer) const;

	void SetCurrentTabPage(EAuctionTabId currentTabPage);
	EAuctionTabId GetCurrentTabPage() const { return _currentTabPage; }

	TArray<INT> GetFailedAuctionList() const;
	void SetFailedAuctionList(const TArray<INT>& lst);

	void BuyFromViewList(INT auctionID, INT price);
	void BuyFromBuyingList(INT auctionID, INT price);
	void BuyFromFaildAuctionList(INT auctionID, INT price);

	void QueryFailedItems();

	BOOL Check_IsMySellingItem(INT dwAuctionID);
	BOOL IsShowSmg_DefaultPrice(INT dwAuctionID,INT price,Auction_ScreenMsg_Type type);
	BOOL Check_DefaultAuctionPrice(  const ClientAuctionItemInfo& info,INT price);

	INT MaxPageNumb() const { return _maxPageNumb; }
	void MaxPageNumb(INT val) { _maxPageNumb = val; }
	
	INT CurrentPageNum() const { return _currentPageNum; }
	void CurrentPageNum(INT val) { _currentPageNum = val; }

	bool LastActionIsNextPage() const { return _lastActionIsNextPage; }
	void LastActionIsNextPage(bool val);

	bool CheckItemIsAuction(UINT32 itemId);//check whether you are the highest bidder on the item

	ItemInfo* GetAuctionItemInfo(INT itemId);

	void NotifyUpdateCurrentPageInfo();
	void NotifyCreateTreeItemList();
	void NotifySearchTreeItemList();
	void NotifyFilterTreeItem(Auction_FilterType nType);
	void NotifyUpdateItemCategoryList();
	void NotifyQuickSearch();
	void NotifyQuickSearchByBag(BYTE SlotType,BYTE ContainerID,INT SlotID);
	void NotifyUpdateMyBuyingList();
	void NotifyUpdateBoughtList();

	void AddAuctionItem(UItem* node);
	void SearchItem();

	UGFxObject* CreateSearchItem(UMMOGFXPlayer* gfxPlayer);
	UGFxObject* CreateQuickSearchItem(UMMOGFXPlayer* gfxPlayer);
	UGFxObject* CreateTreeList(UMMOGFXPlayer* gfxPlayer);
	UGFxObject* CreateTreeListByFilter(UMMOGFXPlayer* gfxPlayer,Auction_FilterType filterType);

	void CreateItemFromVec(AuctionItemVec* vec,UGFxObject* datList,UMMOGFXPlayer* gfxPlayer,Auction_FilterType filterType = EAFT_Invalid);
	void CreateQuickSearchItemFromVec(AuctionItemVec* vec,UGFxObject* datList,UMMOGFXPlayer* gfxPlayer);

	void UpdateAuctionItemVec(AuctionItemVec* vec);
	void SetSearchItemId(INT id);

	bool CkeckIsShowTreeList(BYTE mainType, BYTE subType);
	bool ChechIsCreateItem(UItem* item,Auction_FilterType filterType);

	AuctionItemVec* GetAuctionItemVec();
	void ResetAuctionItemVec();
	void AuctionItemVec_AddItem(INT32 id);

	bool ChechIsCreateItem_Class(UItem* item);
	bool ChechIsCreateItem_ItemColor(UItem* item);
	bool ChechIsCreateItem_LV(UItem* item);
	bool CheckIsCreateItem_Name(UItem* item);

	bool IsShowTreeList(Auction_FilterType type);

	void UpdateFilterByName();

	void ResetSearchFilter();

	void OnCategoryClick(BYTE mainType, BYTE subType);

	UGFxObject* CreateItemSolt(UItem* item,UMMOGFXPlayer* gfxPlayer,INT idx);

	UItem* GetSearchItemByID();

	INT GetitemIdByAuctionId(INT auctionId,Auction_ScreenMsg_Type type);

	INT GetMoneyRatio(INT64 nMoney);
	INT64 GetRealAuctionMoney(INT64 price);

	ClientAuctionItemInfo* GetItemFromBuyListAndBuyOutList(INT itemId);

	TArray<INT> GetAuctionSellConfig(){return AuctionSellConfig;}

	void NotifyUpdateQueryItem();

	QueryAuctionItemInfo GetQueryItemInfo(){ return currentQueryAuctionInfo;}
	const ClientAuctionItemInfo* TryFindLocal(INT auctionID) const;	

	BOOL GetShowByLink(){return bShowByLink;}
	void SetShowByLink(BOOL v){bShowByLink = v;}

	void NotifyUpdatePayLeftItem();

	const ClientAuctionItemInfo* GetBoughtItemById(INT auctionId);

	INT32 GetLinkItemId(){return linkItemId;}
	void SetLinkItemId(INT32 v){linkItemId = v;}

	void SetLinkItemShow(BOOL v){bShowLinkItem = v;}
	BOOL GetLinkItemShow(){return bShowLinkItem;}

	INT GetLinkItemIndex(){return linkItemIndex;}
	void SetLinkItemIndex(INT n){linkItemIndex = n;}
	void ResetLinkItemInfo();

	UINT32 GetClientRealTime();

	void UpdateAuctionView();

	void SetUpdateCurrentPage(bool v){bUpdateCurrentPage = v;}
	BOOL GetUpdateCurrentPage(){return bUpdateCurrentPage;}

	static bool CompareOrderTime(const ClientAuctionItemInfo& leftData, const ClientAuctionItemInfo& rightData);

	void ResetSellInfo();
	void SetAuctionSellInfo(AuctionSellInfo info);
	AuctionSellInfo GetAuctionSellInfo(){return sellInfo;}

protected:

	EAuctionTabId _currentTabPage;
	AuctionSearchFilter _searchFilter;		// filter data
	
	AuctionItemList _auctionViewItemList;
	AuctionItemList _auctionMySellingItemList;
	AuctionItemList _auctionMyBuyingItemList;

	AuctionItemList _auctionMyFailedAuctionItemList;
	TArray<INT> _auctionMyFailedAuctionItemIDList;

	AuctionItemList _auctionMyBoughtItemList;

	INT _currentPageNum;
	INT _maxPageNumb;
	bool _lastActionIsNextPage;
	INT _peakMaxPage; // highest page count in the current search


	static void TryAddToList(AuctionItemList&lst, const ClientAuctionItemInfo& info);
	static bool TryRemoveFromList(AuctionItemList&lst, INT auctionID);
	static bool Contain(const AuctionItemList&lst, INT auctionID);

	static bool TryRemoveFromList(TArray<INT>&lst, INT auctionID);

	static const ClientAuctionItemInfo* FindAuctionItem(const AuctionItemList&lst, INT auctionID);

	void NotifySearchResult(BOOL update = FALSE);
	void NotifyFailedList();
	void NotifyMySellingList();
	void NotifyMyBuyingList();
	void NotifyUpdatePageInfo();
	void NotifyShowLinkItem();
	
	void Buy(const ClientAuctionItemInfo& info, INT price);

	static void SendQueryPlayerInfo(const TArray<INT>& idList);

	AuctionItemMap _auctionItemMap;
	AuctionItemVec _auctionItemVec;
	INT32 _searchItemId;

	TArray<INT> AuctionSellConfig;
	
	QueryAuctionItemInfo currentQueryAuctionInfo;

	BOOL bShowByLink;
	
	INT32 linkItemId;
	INT linkItemIndex;
	BOOL bShowLinkItem;

	BOOL bUpdateCurrentPage;

	AuctionSellInfo sellInfo;
};

#endif