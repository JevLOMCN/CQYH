#ifndef _MMOHANDBOOKDATA_H_
#define _MMOHANDBOOKDATA_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include <queue>
#include "UnString.h"
#include "MMOFacade.h"
#include "MMODataManager.h"
#include "role_db_def.h"

enum
{
	MAX_HANDBOOK_COUNT_PER_ROW = 4,
};

struct HandbookData
{
	UINT32 Id;						// codex id
	UINT32 Type;					// category
	UINT32 Index;					// index
	TArray<UINT32> TemplateId;		// template id, npc, item...
	UINT32 AchievementId;			// achievement Id obtained
	FString Name;					// codex name
	FString Text0;					
	FString Text1;
	FString Text2;
	TArray<UINT32> Data0;
	TArray<UINT32> Data1;
	FString Sound;
	FString Icon;
	FLOAT Scale;

	HandbookData()
	{
		Id = 0;
		Type = 0;
		Index = 0;
		AchievementId = 0;
		Name = TEXT("");
		Text0 = TEXT("");				
		Text1 = TEXT("");
		Text2 = TEXT("");
		Sound = TEXT("");
		Icon = TEXT("");
		Scale = 1.0;
	}
};


struct HandbookDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("handbook.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static HandbookData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(HandbookData* obj);
	static void ParseData(HandbookData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("HandbookID");
	}

	static FString FormatAsString(HandbookData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(HandbookData* obj, TSet<FString>& outResList);
#endif
};

typedef TDataManger<HandbookData, HandbookDataProcessor> HandbookDataManager;
typedef std::map<UINT32, struct HandbookData, M_UE3_STL_ALLOC_MAP(UINT32, HandbookData)> HandbookDataMap;
typedef std::list<HandbookData, M_UE3_STL_ALLOC(HandbookData)> HandbookDataList;
typedef std::map<UINT32, HandbookDataList, M_UE3_STL_ALLOC_MAP(UINT32, HandbookDataList)> HandbookDataListMap;

class MMOHandbookData : public SingletonMeyers<MMOHandbookData>
{
public:
	MMOHandbookData();
	virtual ~MMOHandbookData();

	void Init();
	void Shutdown();
	void Clear();

	void AddHandbookData(const HandbookData& data);
	UBOOL GetHandbookData(UINT32 type, UINT32 index, HandbookData& data);
	UBOOL GetHandbookDataList(UINT32 type, HandbookDataList& list);
	UBOOL GetHandbookDataListByKey(UINT32 type, const FString& key, HandbookDataList& dataList);

	void SyncHandbookInfo(ROLE_DB_ACHIEVEMENT_DATA* data);
	void UpdateHandbookInfo(INT32 type, INT32 index);

	UBOOL IsHandbookObtain(UINT32 type, UINT32 index);
	UINT32 GetCurHandbookCount(UINT32 type);
	UINT32 GetMaxHandbookCount(UINT32 type);

	UBOOL GetMapHandbookInfoByName(const FString& name, UINT32& type, UINT32& index);
	UBOOL GetMonsterHandbookInfoByName(const FString& name, UINT32& type, UINT32& index);
	UBOOL GetEquipHandbookInfoById(INT itemId, UINT32& type, UINT32& index);
	UBOOL GetStoryHandbookInfoById(INT itemId, UINT32& type, UINT32& index);

	UBOOL LoadHandbookData(const TCHAR* filePath);
	void UnloadHandbookData();

protected:
	UINT32 _GetHandbookRealId(UINT32 type, UINT32 index);

private:
	HandbookDataMap _HandbookDataMap;
	HandbookDataListMap _HandbookDataListMap;
	UINT32 _Handbook[htTotal][MAX_HANKBOOK_COUNT_PER_TYPE / 32];
};

#endif