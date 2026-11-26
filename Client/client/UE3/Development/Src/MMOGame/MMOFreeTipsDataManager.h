#ifndef __MMOFREETIPSDATAMANAGER_H__
#define __MMOFREETIPSDATAMANAGER_H__

#include "MMOGameClasses.h"
#include "MMODataManager.h"
#include "MMOTemplateObjectFinder.h"

struct FFreeTipsData
{
	INT		ID;
	FString MovieName;
	FString PanelName;
	FString ValueName;
	FString ScriptTxt;

	/** Constructors */
	FFreeTipsData() : ID(0) {}
	FFreeTipsData(EEventParm) : ID(0) {}
};


struct FreeTipsDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("FreeTipsDataProcessor");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FFreeTipsData* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(FFreeTipsData* obj);

	static void ParseData(FFreeTipsData* obj, class CTabFile* dataRow, int rowIdx);

	template<typename DataMgr>
	static void TParseData(FFreeTipsData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	//static FString FormatAsString(FFreeTipsData* obj)
	//{
	//	return obj->GetFullName();
	//}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FFreeTipsData* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger< FFreeTipsData, FreeTipsDataProcessor > MMOFreeTipsDataManager;

#endif
