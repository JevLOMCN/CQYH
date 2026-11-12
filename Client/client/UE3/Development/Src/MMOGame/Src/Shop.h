#ifndef __SHOP_H__
#define __SHOP_H__

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "MMODataManager.h"

#define ShopItemCostTypeNum 2

enum ShopType
{
	InvalidShop = -1,
	NormalShop,
	BlackMarket,

};
struct ServerShopItemData
{
	UItem* ShopItem;
	INT ShopSlot;
	INT CostType[ShopItemCostTypeNum];
	INT CostValue[ShopItemCostTypeNum];
	INT RequireForceID;
	INT Reputation;
	INT RequireGuildBuildingID;
	INT BuildingLevel;
	INT ItemCount;
	INT LimitType;
	INT MaxCoolDownTime;
	INT CurrentCoolDown;
	INT32 ShopTypeSell;

};

struct ServerShopData
{
	INT _ShopId;
	FString _ShopName;
	TArray<ServerShopItemData> _ShowItemList;
};

typedef TMap<INT,ServerShopData> ShopDataMap;


struct BlackMarketDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("black_market.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}


	static ServerShopItemData* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(ServerShopItemData* obj)
	{
		if(obj)
		{
			delete obj;
		}
	}

	static void ParseData(ServerShopItemData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("BlackMarketID");
	}

	static FString FormatAsString(ServerShopItemData* obj)
	{
		return FString::Printf(TEXT("ServerShopItemData[%s]%d"), obj->ShopSlot, obj->ShopSlot);
	}

	static void PostInitData(){}
};

class BlackMarketData : public TDataManger<ServerShopItemData, BlackMarketDataProcessor>
{
public:

	BlackMarketData(){}
	virtual ~BlackMarketData(){}
};


struct MMOGameEvent_BlackMarket_UpdateItemCount : public MMOGameEvent
{
public:

	MMOGameEvent_BlackMarket_UpdateItemCount(INT id, INT count)
		: id(id)
		, count(count)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_BlackMarket_UpdateItemCount; }

	INT id;
	INT count;
};

class ShopManager : public SingletonMeyers<ShopManager>
{
public:

	ShopManager();
	virtual ~ShopManager(){};
	void Init();

	ServerShopData* FindShopData(INT shopID) const;
	ServerShopData* FindBlackMarketData(INT ID) const;

	const ServerShopItemData* FindShopItem( INT shopTemplateID, INT itemIdx ) const;
	const ServerShopItemData* GetCurShopItem( INT itemIdx );
	ServerShopItemData* GetCurShopItemByItemTempId( INT itemTempId );

	//Now it comes from the server
	void AddShopItem( INT count , NPC_SHOP_DATA* data );
	void SetCrc( UINT32 crc ){ _crc = crc; }
	UINT32 GetCrc(){ return _crc; }

	void SetCurShop( INT shopId );
	void CheckShopData();
	INT GetCurShopId(){ return _curShopId; }
	void UpdateShopItemList();
	void ClearCurShop();

	void ClearCurShopItemList();
	INT GetCurShopItemNum();
	TArray<ServerShopItemData>* GetNormalItemlist(){ return &_normalShopItemList; }
	TArray<ServerShopItemData>* GetMaterialItemlist(){ return &_materialShopItemList; }
	TArray<ServerShopItemData>* GetEquipItemlist(){ return &_equipShopItemList; }

	void SaveShopData( INT count , NPC_SHOP_DATA* data );
	void LoadShopData();

	void ShowShopType();

	TArray<INT>* GetShopTypeList(){ return &_shopType; }

	void AddItemToBlackMarket(ServerShopItemData& obj, INT shopId);

	void SetCurShopType(ShopType t) {_curShopType = t;}
	ShopType GetCurShopType(){ return _curShopType;}

	void UpdateBlackMarketItemListCount(BLACK_MARKET_LIMIT_INFO* item);
	void UpdateBlackMarketItemListCoolDown(BLACK_MARKET_COOLDOWN_INFO* item);
	INT GetCurShopSellType();

	BOOL GetShopShow(){return bShowShop;}
	void SetShopShow(BOOL v){bShowShop = v;}

protected:

	ShopDataMap _shopDataMap;
	ShopDataMap _blackMarketDataMap;//black market

	UINT32 _crc;
	UBOOL _hasCheckShopCrc;
	UBOOL _hasLoadShop;
	ServerShopData* _curShop;
	INT _curShopId;
	TArray<ServerShopItemData> _equipShopItemList;
	TArray<ServerShopItemData> _materialShopItemList;
	TArray<ServerShopItemData> _normalShopItemList;

	//void _AddShopData(const FShopData& dat);
	//typedef std::map<INT, FShopData, M_UE3_STL_ALLOC_MAP(INT, FShopData) > ShopDataMap;
	TArray<INT> _shopType;

	ShopType _curShopType;

	BOOL bShowShop;

};


#endif