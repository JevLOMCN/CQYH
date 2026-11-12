#ifndef _MMOHELPERDATA_H_
#define _MMOHELPERDATA_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include "UnString.h"
#include "MMOFacade.h"
#include "MMODataManager.h"

enum
{
	MAX_HELP_BASE_CLASS_COUNT = 20,
};

// help data
struct HelperData
{
	UINT32 Id;
	FString PanelName;
	UINT32 BaseClass;
	UINT32 SubClass;
};

struct HelperDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("help.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static HelperData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(HelperData* obj);
	static void ParseData(HelperData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(HelperData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(HelperData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<HelperData, HelperDataProcessor> HelperDataManager;
typedef std::map<FString, HelperData, ComparFString, STLAllocator<std::pair<FString, struct HelperData>, UE3STLAllocatorPolicy>> HelperDataMap;

class MMOHelperData : public SingletonMeyers<MMOHelperData>
{
public:
	MMOHelperData();
	virtual ~MMOHelperData();

	void Init();
	void Shutdown();
	void Clear();

	void AddHelperData(const HelperData& data);
	UBOOL GetHelperData(const FString& name, HelperData& data);
	HelperDataMap& GetHelperDataMap();

protected:
	UBOOL _LoadHelperData(const TCHAR* filePath);
	void _UnloadHelperData();

	HelperDataMap _HelperDataMap;
};

#endif