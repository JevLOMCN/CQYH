#ifndef _MMOEQUIPREMAKEMANAGER_H_
#define _MMOEQUIPREMAKEMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOGameClasses.h"
#include "MMODataManager.h"

struct MMOGameEvent_RemakeResult
	:public MMOGameEvent_Any
{
	MMOGameEvent_RemakeResult(EGameEvent evtType,INT result,INT itemid)
		:MMOGameEvent_Any(evtType)
		,_Result(result),_ItemID(itemid)
	{

	}

	INT _Result;
	INT _ItemID;
};

struct GambleTemplateDataKey
{
	union
	{
		struct  
		{
			INT nGambleType		:8;
			INT nGambleLevel	:24;
		};
		UINT nID;
	};

	GambleTemplateDataKey()
	{
		nGambleType = -1;
		nGambleLevel = 0;
	}
	GambleTemplateDataKey(INT rType,INT rLevel)
	{
		nGambleType = rType;
		nGambleLevel = rLevel;
	}
};

struct GambleTemplateDataValue
{
	GambleTemplateDataValue()
		: nGambleType(0), nGambleLevel(0), nCostItemID(0), nCostItemCount(0) {}

	INT nGambleType;
	INT nGambleLevel;
	INT nCostItemID;
	INT nCostItemCount;
};


struct GambleTemplateIndexDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("gamble.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}


	static GambleTemplateDataValue* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		GambleTemplateDataValue* item = new GambleTemplateDataValue;
		needAddToMap = false;
		return item;
	}

	static void DestroyDataObject(GambleTemplateDataValue* obj)
	{
		delete obj;
	}

	static void ParseData(GambleTemplateDataValue* obj, class CTabFile* dataRow, int rowIdx);

	static FString FormatAsString(GambleTemplateDataValue* obj)
	{
		return FString::Printf(TEXT("GambleTemplateDataIndex GambleType:%d GambleLevel:%d nCostItemID:%d nCostItemCount:%d"),
			obj->nGambleType, obj->nGambleLevel, obj->nCostItemID , obj->nCostItemCount);
	}

	static FString GetTemplateIDName()
	{
		return TEXT("GambleType");
	}

	static void PostInitData(){}
	static INT ItemSubTypeToGambleType(ITEM_SUBTYPE eItemSubType);
	static INT GambleTypeStringToEnum(FString str);

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GambleTemplateDataValue* obj, TSet<FString>& outResList) {};
#endif
};

class GambleTemplateDataIndexManager
	: public TDataManger< GambleTemplateDataValue, GambleTemplateIndexDataProcessor >
{
public:

	GambleTemplateDataIndexManager() {}
	virtual ~GambleTemplateDataIndexManager() {}
};



class MMOEquipRemakeManager: public SingletonMeyers<MMOEquipRemakeManager>
{
public:
	void Init();
	void Shutdown();

	void	OnRemake(UINT8 byItemSubType,INT nLevel);
	void	OnRemakeResultSuccess(INT nItemID);
	void	OnRemakeResultFailed();

	UBOOL   GetMaterialIDAndCost(UINT8 byItemSubType ,INT nLevel, INT& nItemID, INT& nCostCount);

private:
};

#endif