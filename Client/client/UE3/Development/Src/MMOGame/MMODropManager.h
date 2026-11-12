#ifndef __MMODROPMANAGER_H__
#define __MMODROPMANAGER_H__

#include "MMOGameEvent.h"
#include "ICanvasRenderable.h"
#include "MMODataManager.h"


struct FDropItemSet
{
	class UItem* GetDropItemTemplate() const;
	INT ItemTemplateID;
	INT DropWeight;
	INT CountMin;
	INT CountMax;
};

struct FDropGroup
{
	INT TemplateID;
	FString TemplateDesc;
	UBOOL Enabled;
	TArray<FDropItemSet> DropItemList;
};


struct DropGroupDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("drop_group.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FDropGroup* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(FDropGroup* obj)
	{
		delete obj;
	}

	static void ParseData(FDropGroup* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("DropGroupID");
	}

	static FString FormatAsString(FDropGroup* obj)
	{
		return FString::Printf(TEXT("FDropGroup[%d]%s"), obj->TemplateID, *obj->TemplateDesc);
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FDropGroup* obj, TSet<FString>& outResList)
	{
		for (INT i = 0; i < obj->DropItemList.Num(); ++i)
		{
			_CollectReferencedResource(obj->DropItemList(i).GetDropItemTemplate(), outResList);
		}		
	}
#endif
};

typedef public TDataManger< FDropGroup, DropGroupDataProcessor, INT > MMODropGroupDataManager;

#if !SHIPPING_PC_GAME
bool MMODropGroupDataManager_IsItemExists(INT itemTemplateID);
#endif

//------------------------------------------------------------------------------------------------
// MMODropManager
//------------------------------------------------------------------------------------------------

struct FDropActorInfo
{
	class AMMODropActor* DropActor;
	UGFxObject* Hud;
	FVector2D ScreenPos;
	INT Width;
	INT Height;
	UBOOL IsVisible;
	float ShowTime;
	float CannotPickCD;//if a request cannot pick up once, set a countdown
};

typedef std::map<INT, FDropActorInfo, M_UE3_STL_ALLOC_MAP(INT, FDropActorInfo) > DropActorMap;

class MMODropManager
	: public SingletonMeyers<MMODropManager>
	, public INativeGameEventHandler
	, public ICanvasRenderable
{
public:

	MMODropManager();

	class AMMODropActor* AddDropActor(bool isNew, const FVector& spawnLocation, const FVector& destLocation, INT itemTemplateID, INT itemCount, INT dropActorID, bool enableCollision = true, INT spawnOrder = 0);
	void DestroyDropActor(INT dropActorID, UBOOL removeFromWrold);
	void DestroyAllDropActor();

	class AMMODropActor* FindDropActor(INT dropActorID);

	void SetDropTipsUI(class UMMOUIDropItemTips* tipsUI);
	UMMOUIDropItemTips* GetDropTipsUI();

	void PickupDropActorByUI(INT dropActorID);

	void PickupAll() { _pickupAll = true; }

	UBOOL GetShowDropHud() const;
	void SetShowDropHud(UBOOL val);

	void ShowDropActorHUD(INT dropActorID);

	void UpdateDropHudPos(FLOAT DeltaTime);

	// Mainly used for drops that can accept/turn in quests add by zhangrui05@snda
	void SetDropTipsIcon(INT dropActorId, const FString& icon);

	void SetDropActorCannotPick(INT nDropID,FLOAT pickCD=0.5f);

	// add by zhangrui05@snda
	const DropActorMap& GetDropActorMap() { return _dropActoMap; }

	void SetVisibleForceRefresh(){_bForceRefresh=TRUE;};

#if !SHIPPING_PC_GAME
	void TestDropAll(INT dist);
#endif

public:
	//UBOOL m_bDesireShow;
	 INT m_CanPickUpDropID;
	 FDropActorInfo* m_CanPickUpInfo;
private:

	DropActorMap _dropActoMap;

	class UMMOUIDropItemTips* _dropTipsUI;

	typedef std::list<class UGFxObject*, M_UE3_STL_ALLOC(class UGFxObject*) > DropHudQueue;
	DropHudQueue _freeDropHudQueue;

	FDropActorInfo* FindDropActorInfo(INT nDropID);
	void _Destroy(FDropActorInfo& info, UBOOL removeFromWrold);

	bool	_pickupAll;
	UBOOL	_alphaMoving;
	INT     _alphaHoldFrame;
	FLOAT   _alphaCurValue;

	UBOOL   _bForceRefresh;

	virtual void OnCanvasPostRender(class UCanvas* canvas, FLOAT fDelta);
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);
};
#endif