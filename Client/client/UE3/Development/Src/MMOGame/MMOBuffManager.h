/**
 * jimZhang@sdna.com 2013.1.22 
 *
 */

#ifndef __MMOBUFFMANAGER_H__
#define __MMOBUFFMANAGER_H__
#define MAXBUFFSLOT 14
#define MAXBUFFSLOTEX 8

#define BUFFID_SAFEZONE		2503
#define SERVERID_SAFEZONE	2503999

#define BUFFID_MOUNT		2501
#define SERVERID_MOUNT		2501999

#define BUFFID_DEFAULTSKILL		2502
#define SERVERID_DEFAULTSKILL	2502999

#define SHOW_BUFF_LOG 0

//#include "stdafx.h"
#include <list>
#include <vector>
#include <map>

#include "MMOUtil.h"
#include "MMODataManager.h"


//using namespace std;
class UBuff;

struct BuffData;

typedef std::map<INT, BuffData*, M_UE3_STL_ALLOC_MAP(INT, BuffData*) > STLInt2BuffDataMap;

struct BuffDataMap
{
	STLInt2BuffDataMap Buffs;
};

typedef std::map<INT, BuffDataMap*, M_UE3_STL_ALLOC_MAP(INT, BuffDataMap*) > STLInt2BuffDataMapMap;

class MMOBuffManager 
{
	STLInt2BuffDataMapMap	ActiveBuffsMap;

	//STLInt2BuffDataMap	ActiveBuffs;
	BuffData*			ShowBuffs[MAXBUFFSLOT];	
	BuffData*			ShowBuffsEx[MAXBUFFSLOTEX];	//function-type buffs
	BuffData*			ShowTargetBuffs[MAXBUFFSLOT];	
	TArray<INT>			DisableUseNormalAbilityBuffType;
	TArray<INT>			DisableUseAllAbilityBuffType;

private:
	TArray<INT>			PostDeleteBuffObjID;
	TArray<INT>			PostDeleteBuffBuffID;

public:
	MMOBuffManager();
	virtual ~MMOBuffManager(){}

public:
	void RefreshSlotBuffEx(INT ObjId);
	void HideSlotBuffEx();

private:
	void RefreshSlotBuff(INT ObjId);
	void RefreshSlotBuff_Target(INT ObjId);

	void AddBuffData(BuffData* InBuff);
	BuffData* GetActiveBuff(INT ObjId,INT InId);
	void DeleteActiveBuff(INT ObjId);
	void DeleteActiveBuff(INT ObjId,INT InId);
	void DeleteBuffDataMap(BuffDataMap* pBuffDataMap, INT CharactorID);

public:
	//BuffData* GetActiveBuff(INT ObjId,INT InId);
	void ClearAllBuff();
	BuffData* GetBuffBySlot(INT nSlot);
	BuffData* GetBuffExBySlot(INT nSlot);
	BuffData* GetTargetBuffBySlot(INT nSlot);
	BuffDataMap* GetBuffMap(INT ObjectId);
	void AddOrChangeBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose=FALSE);
	void AddNewBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose=FALSE);
	void ChangeBuff(INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose=FALSE);
	void ModifyBuff(BuffData* newBuff, INT CreatorId, INT ObjId, INT Id, INT TemplateId, INT Level, FLOAT Duration, FLOAT MaxDuration, INT StackCount, UBOOL bAutoClose=FALSE);

	void DeleteBuff(INT ObjId, INT Id);
	void DeleteBuffByCharacterID(INT ObjId);
	void TickBuffData(FLOAT DeltaTime);
	void UpdateTargetBuff(INT ObjId);

	UBOOL	CheckBuffEnableUseNormalAbility( UBuff* buff );
	UBOOL	CheckBuffEnableUseAllAbility( UBuff* buff );
};



//------------------------------------------------------------------------------------------------
// BuffTemplateDataIndex
//------------------------------------------------------------------------------------------------
struct BuffTemplateDataIndex
{
	FString ObjectPath;
	INT TemplateID;
};


struct BuffTemplateIndexDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("aura.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}


	static BuffTemplateDataIndex* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		BuffTemplateDataIndex* item = new BuffTemplateDataIndex;
		needAddToMap = false;
		return item;
	}

	static void DestroyDataObject(BuffTemplateDataIndex* obj)
	{
		delete obj;
	}

	static void ParseData(BuffTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("TemplateID");
	}

	static FString FormatAsString(BuffTemplateDataIndex* obj)
	{
		return obj->ObjectPath;
	}

	static void PostInitData(){}

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(BuffTemplateDataIndex* obj, TSet<FString>& outResList);
#endif
};

class BuffTemplateDataIndexManager
	: public TDataManger< BuffTemplateDataIndex, BuffTemplateIndexDataProcessor >
{
public:

	BuffTemplateDataIndexManager() {}
	virtual ~BuffTemplateDataIndexManager() {}
};







#endif