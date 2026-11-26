#ifndef __DOODADMANAGER_H__
#define __DOODADMANAGER_H__

#include "MMOGameEvent.h"
#include "MMOGameClasses.h"
#include "MMOQuestData.h"
#include "MMOUtil.h"
#include "MMODataManager.h"
#include <deque>

typedef std::map<INT, struct FDoodadDropData*, M_UE3_STL_ALLOC_MAP(INT, struct FDoodadDropData*) > DoodadDropTemplateMap;
typedef std::map<INT, class UDoodad*, M_UE3_STL_ALLOC_MAP(INT, class UDoodad*) > DoodadTemplateMap;


struct MMOGameEvent_Doodad : public MMOGameEvent
{
public:
	MMOGameEvent_Doodad(AMMODoodadActor* actor, BYTE oldOperCnt)
		: DoodadActor(actor)
		, OldOperateCount(oldOperCnt)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Doodad;
	}

	AMMODoodadActor* DoodadActor;
	BYTE OldOperateCount;
};

//------------------------------------------------------------------------------------------------
// Doodad drop config data
//------------------------------------------------------------------------------------------------

struct FDoodadDropSet
{
	FDoodadDropSet() : DropChance(0), DropGroupID(0) {}
	INT DropChance;
	INT DropGroupID;

	const struct FDropGroup* GetDropGroup() const;
};

struct FDoodadDropData
{
public:
	FDoodadDropData() : DoodadTemplateID(0), DoodadLevel(0), SceneDifficultyClient(0) {}

	static const INT C_MaxDropSetCount = 16;
	static UINT CombineID(INT templateID, INT lv, INT sceneDiff);
	static void DecomposeID(UINT id, INT& templateID, INT& lv, INT& sceneDiff);

protected:

	UINT DoodadTemplateID;
	BYTE DoodadLevel;
	BYTE SceneDifficultyClient;
	FDoodadDropSet DropSets[C_MaxDropSetCount];

public:

	UINT GetTemplateID() const { return CombineID(DoodadTemplateID, DoodadLevel / 5, SceneDifficultyClient - 1); }

	UINT GetDoodadTemplateID() const { return DoodadTemplateID; }
	void SetDoodadTemplateID(UINT val) { DoodadTemplateID = val; }

	class UDoodad* GetDoodadTempalte() const;

	BYTE GetDoodadLevel() const { return DoodadLevel; }
	void SetDoodadLevel(BYTE val) { DoodadLevel = val; }

	BYTE GetSceneDifficulty() const { return SceneDifficultyClient; }
	void SetSceneDifficulty(BYTE val) { SceneDifficultyClient = val; }

	INT GetDropSetCount() const { return C_MaxDropSetCount; }
	void SetDropSet(INT idx, const FDoodadDropSet& dropSetData)
	{
		if(idx >= 0 && idx < C_MaxDropSetCount)
		{
			DropSets[idx] = dropSetData;
		}
		else
		{
			warnf(NAME_Error, TEXT("[FDoodadDropData::SetDropSet] Index out of bound for doodad drop[%s] idx[%d]"), *ToString(), idx);
		}

	}

	const FDoodadDropSet& GetDropSet(INT idx) const
	{
		if(idx >= 0 && idx < C_MaxDropSetCount)
		{
			return DropSets[idx];
		}
		else
		{
			warnf(NAME_Error, TEXT("[FDoodadDropData::GetDropSet] Index out of bound for doodad drop[%s] idx[%d]"), *ToString(), idx);
			return DropSets[0];
		}
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Doodad[%d] Lv[%d] SceneDiff[%d]"), DoodadTemplateID, DoodadLevel, SceneDifficultyClient);
	}
};


struct DoodadDropDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("doodad_drop_data.txt");
	}

	static UINT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FDoodadDropData* CreateDataObject(UINT templateID, bool& needAddToMap)
	{
		FDoodadDropData* dat = new FDoodadDropData();
		appMemSet(*dat, 0);
		dat->SetDoodadTemplateID(templateID);
		needAddToMap = false;
		return dat;
	}

	static void DestroyDataObject(FDoodadDropData* obj)
	{
		delete obj;
	}

	static void ParseData(FDoodadDropData* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("DoodadTemplateID");
	}

	static FString FormatAsString(FDoodadDropData* obj)
	{
		return FString::Printf(TEXT("DoodadDropData ID:%d Lv:%d Diff:%d"), obj->GetDoodadTemplateID(), obj->GetDoodadLevel(), obj->GetSceneDifficulty());
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FDoodadDropData* obj, TSet<FString>& outResList);
#endif
};

typedef TDataManger< FDoodadDropData, DoodadDropDataProcessor, UINT > MMODoodadDropDataManager;

typedef std::map<INT, class AMMODoodadActor*, M_UE3_STL_ALLOC_MAP(INT, class AMMODoodadActor*) > DoodadActorMap;

//------------------------------------------------------------------------------------------------
// Doodad template object manage
//------------------------------------------------------------------------------------------------
struct FDoodadDataHolder
{
	INT DoodadID;
	FString DoodadObjectPath;
	UDoodad* DoodadTemplate;

	FDoodadDataHolder()
		: DoodadID(0)
		, DoodadTemplate(0)
	{
	}

	class UDoodad* GetDoodadTemplate();
};


struct DoodadDataHolderDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("doodad.txt");
	}

	static UINT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FDoodadDataHolder* CreateDataObject(UINT templateID, bool& needAddToMap)
	{
		FDoodadDataHolder* dat = new FDoodadDataHolder();		
		dat->DoodadID = templateID;
		needAddToMap = true;
		return dat;
	}

	static void DestroyDataObject(FDoodadDataHolder* obj)
	{
		delete obj;
	}

	static void ParseData(FDoodadDataHolder* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(FDoodadDataHolder* obj)
	{
		return FString::Printf(TEXT("DoodadDataHolder ID:%d Path:%s"),
			obj->DoodadID, *obj->DoodadObjectPath);
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FDoodadDataHolder* obj, TSet<FString>& outResList);
#endif
};

typedef TDataManger< FDoodadDataHolder, DoodadDataHolderDataProcessor, INT > MMODoodadTemplateDataManager;


class DoodadManager : public SingletonMeyers<DoodadManager>, public INativeGameEventHandler
{
public:
	DoodadManager();
	virtual ~DoodadManager();

	void Init();
	void Clear();
    void Tick();

	INT SearchBody(const FVector& Location, FLOAT Radius) const;

	void AddDoodadDropTemplate(INT k, struct FDoodadDropData* dat);
	struct FDoodadDropData* FindDoodadDropTemplate(INT templateID);

	//Doodad
// 	void AddDoodadTemplate( INT key, class UDoodad* node);
	class UDoodad* FindDoodadTemplate(INT templateID);
	void LoadAllDoodadTemplate();

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void RefreshDoodadEffects();

	void UpdateDoodadOperateCount(INT doodadID, BYTE newCount, UINT32 dwCharID);

	// add by zhangrui05@snda
	const DoodadActorMap& GetDoodadActorMap() { return _doodadActoMap; }
	void LoadDoodadData();

#if !SHIPPING_PC_GAME
		void TestDoodadEffect();
#endif

	class AMMODoodadActor* AddDoodadActor(const FVector& inLocation, const class UDoodad* doodadTemplate, const FRotator& rot, INT doodadActorID, BYTE remainOpenCount);
	class AMMODoodadActor* FindDoodadActor(INT doodadActorID);
	
	void DestroyDoodadActor(INT doodadActorID, UBOOL removeFromWrold, INT disappearType);
	void DestroyAllDoodadActor();

	void OnDoodadBeginOperate(INT InDoodadId, INT OperatorId, FLOAT InDuration);
	void OnDoodadEndOperate(INT InDoodadId, UBOOL bBreak);

protected:

	//DoodadTemplateMap _doodadTemplateMap;
	DoodadDropTemplateMap _DoodadDropMap;

	DoodadActorMap _doodadActoMap;

	void SetupQuestDoodadEffect(AMMODoodadActor* actor);
};

#endif