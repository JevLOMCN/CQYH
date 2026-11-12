#ifndef _USTALLMANAGER_H
#define _USTALLMANAGER_H

#include "MMOSingleton.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

#include "MMOGameEvent.h"
#include "common.h"
#include "MMOUtil.h"

#define MAX_STALL_SLOT_COUNT 10
#define OPEN_STALL_LEVEL 7			// quest: 23994
#define MIN_STALL_PRICE 100			// minimum sale amount 100 gold
#define MAX_STALL_PRICE 2000000000	// maximum total price per grid cell
#define MAX_PRICE  10000000000       //maximum amount a player can own, 10 billion

enum EStallUpgradeType
{
	ESUT_DAY3 = 0,		// 3 days
	ESUT_DAY7,			// 7 days
	ESUT_DAY15,			// 15 days
	ESUT_MAX
};

enum EStallTax
{
	EST_5 = 5,			// 5%
	EST_10 = 10,		// 10%
	EST_20 = 20,		// 20%
};

struct StallItemInfo
{
	BITFIELD IsLocked:1;
	INT ItemId;
	class UItem* Item;
	INT DurType;
	INT MaxStack;
	INT CurDur;
	INT Price;
	INT ModifyTime;
	INT SrcBag;
	INT SrcSlot;

	StallItemInfo() {}
	StallItemInfo(EEventParm)
	{
		appMemzero(this, sizeof(StallItemInfo));
	}
};

typedef std::map<INT, INT, M_UE3_STL_ALLOC_MAP(INT, INT)> StallItemPriceMap;

struct StallRecordInfo
{
	FString Time;
	FString Operate;
	FString ItemName;
	INT Profit;

	StallRecordInfo()
	{
		Time = TEXT("");
		Operate = TEXT("");
		ItemName = TEXT("");
		Profit = 0;
	}
};

typedef std::list<StallRecordInfo, M_UE3_STL_ALLOC(StallRecordInfo)> StallRecordInfoList;
typedef std::set<FString, ComparFString, M_UE3_STL_ALLOC(FString)> StallAttentionSet;

typedef std::map<INT, INT, M_UE3_STL_ALLOC_MAP(INT, INT)> StallPlayerActorMap;

struct MMOGameEvent_StallEvent : public MMOGameEvent
{
public:
	enum EStallEventType
	{
		ESET_SyncStall,
		ESET_MoneyChange,
		ESET_StateChange,
		ESET_CachedPriceChange,
		ESET_StallSettingItemChange,
		ESET_StallBuyItemChange,
	};

	MMOGameEvent_StallEvent(EStallEventType eventType, INT parm = 0) :
		EventType(eventType),
		Paramter(parm)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Stall;
	}

	EStallEventType EventType;
	INT Paramter;
};

class UStallManager : public SingletonMeyers<UStallManager>, public INativeGameEventHandler
{
public:
	UStallManager();
	~UStallManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void AddStallItem(int stallSlot, int conSlot, int itemSlot, int itemCount, int price);
	void RemoveStallItem(int stallSlot);
	void AddStallBuyItem(int stallSlot, int itemId, int itemCurDur, int itemPrice, int modfiyTime);
	void ResetStallSettingItems();
	void ResetStallBuyItems();

	void ResetStallSellItem(int stallSlot);
	void SetStallSellItem(int stallSlot, const StallItemInfo& info);
	UBOOL GetStallSellItem(int stallSlot, StallItemInfo& info);

	void ResetStallBuyItem(int stallSlot);
	void SetStallBuyItem(int stallSlot, const StallItemInfo& info);
	UBOOL GetStallBuyItem(int stallSlot, StallItemInfo& info);

	void ClearOnSaleFlag();
	void SellStallItem(int stallSlot, int sellCount);
	void BuyStallItem(int stallSlot, int leftCount);
	INT  GetEmptyStallSettingSlot();
	UBOOL CheckStartStallSetting();
	void SetStallBuyPlayerId(UINT32 playerId);
	INT  GetStallBuyPlayerId();

	void ExportStallSettingPriceBinary();
	void LoadStallSettingPriceBinary();
	void UpdateStallSettingPrice(INT itemId, INT price);
	INT  GetCachedStallSettingPrice(INT itemId);
	INT  CalculateStallSettingTotalValue();

	FORCEINLINE void SetStallName(const FString& stallName)
	{
		_StallName = stallName;
	}

	FORCEINLINE FString& GetStallName()
	{
		return _StallName;
	}

	void RequestOpenStall();
	void OpenStall(AMMOPlayer* player);
	void CloseStall(AMMOPlayer* player);
	void RealOpenStall(AMMOPlayer* player);
	void RealCloseStall(AMMOPlayer* player);
	void ChangeActorStallState(AMMOPlayer* player, UINT8 stallState);

	// stall upgrade
	void SetStallUpgradeLeftTime(UINT32 leftTime);
	UINT32 GetStallUpgradeLeftTime() { return _StallUpgradeLeftTime; }
	void ChangeStallLevel(UINT32 roleId, UINT8 level);

	// trade record
	const StallRecordInfoList& GetStallRecordInfoList() { return _StallRecordInfoList; }

	// follow
	void AddAttention(const FString& stallName);
	void RemoveAttention(const FString& stallName);
	UBOOL IsAttentionStall(const FString& stallName);

	// tax rate
	void SetTax(INT tax);
	INT GetTax() { return _StallTax; }

	INT64 GetStallSellInfoPrice();

	void CreateStallActor(AMMOPlayer* player);
	void DestroyStallActor(AMMOPlayer* player);

protected:
	void _UpdateStallUpgradeLeftTime(FLOAT deltaTime);

private:
	FString _StallName;
	StallItemInfo _StallItemSellInfo[MAX_STALL_SLOT_COUNT];
	StallItemInfo _StallItemBuyInfo[MAX_STALL_SLOT_COUNT]; 
	StallItemPriceMap _StallItemPriceMap;

	INT _StallUpgradeLeftTime;
	FLOAT _StallUpgradeLeftTimeProgress;
	StallRecordInfoList _StallRecordInfoList;

	UINT32 _StallBuyPlayerId;
	StallAttentionSet _StallAttentionSet;
	StallPlayerActorMap _StallPlayerActorMap;

	UINT8 _StallTax;
};


#endif