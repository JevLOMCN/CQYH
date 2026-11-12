
#ifndef _LOADMAPTIPSMANAGER_H_
#define _LOADMAPTIPSMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameUIClasses.h"
#include "common.h"
#include "MMOUtil.h"
#include "MMODataManager.h"
/*
No.	map ID	applicable class	min level	max level	spawn weight	Tips content
	1	0	0	0	100	100	at most twenty Chinese characters
	2	101	1	10	20	200	at most twenty Chinese characters
*/

struct LoadMapTipsData
{
	INT nID;
	INT nMapId;
	INT nClass;
	INT nMinLevel;
	INT nMaxLevel;
	INT nWeight;
	FString strTips;
	FString Image;
};

struct LoadMapTipsDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("loadmaptips.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static LoadMapTipsData* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(LoadMapTipsData* obj);

	static void ParseData(LoadMapTipsData* obj, class CTabFile* dataRow, int rowIdx);


	static FString GetTemplateIDName()
	{
		return TEXT("No");
	}

	static FString FormatAsString(LoadMapTipsData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(LoadMapTipsData* obj, TSet<FString>& outResList)
	{
		_CollectReferencedResource(LoadUEObject<UTexture2D>(obj->Image, TEXT("loadmaptips.txt")), outResList);
	}
#endif
};

typedef TDataManger< LoadMapTipsData, LoadMapTipsDataProcessor >   LoadMapTipsDataManager;

typedef std::vector<LoadMapTipsData, M_UE3_STL_ALLOC(LoadMapTipsData) >		VecLoadMapTipsData;

class LoadMapTipsManager : public SingletonMeyers<LoadMapTipsManager>
{
public:
	LoadMapTipsManager();
	virtual ~LoadMapTipsManager();

	void Init();
	void Shutdown();
	
	void AddTipsData(LoadMapTipsData& data);
	FString RandomMapTips();
	FString RandomImage();
	const LoadMapTipsData* RandomData() const;

	void    SetNextMapName(FString mapName);
	void    SetNextMapID(INT nMapID);
	INT     RandomNO(std::list<INT>&listRand) const;
private:
	VecLoadMapTipsData vecMapTips;
	INT				nNextMapID;//map ID to jump to
};



#endif