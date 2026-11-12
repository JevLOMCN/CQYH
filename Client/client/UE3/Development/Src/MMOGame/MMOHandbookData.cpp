#include "stdafx.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOHandbookData.h"

using namespace ExternalMessage;

//////////////////////////////////////////////////////////////////////////
HandbookData* HandbookDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	HandbookData* obj = HandbookDataManager::getSingleton().FindAndModifyData(templateID);
	if (obj)
	{
		needAddToMap = false;
	}
	else
	{
		obj = new HandbookData();
		if (obj)
		{
			obj->Id = templateID;
		}
		needAddToMap = true;
	}

	return obj;
}

void HandbookDataProcessor::DestroyDataObject(HandbookData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void HandbookDataProcessor::ParseData(HandbookData* obj, class CTabFile* dataRow, int rowIdx)
{
	HandbookData data;

	data.Id = TReadDataValue_Int<HandbookDataManager>(TEXT("HandbookID"), dataRow, rowIdx);
	data.Type = MMOFacade::getSingleton().FindClientEnumValue(TReadDataValue_FString<HandbookDataManager>(TEXT("HandbookType"), dataRow, rowIdx),
		TEXT("EHANDBOOK_TYPE"));
	data.Index = TReadDataValue_Int<HandbookDataManager>(TEXT("HandbookIndex"), dataRow, rowIdx);

	FString dataStr = TReadDataValue_FString<HandbookDataManager>(TEXT("ID"), dataRow, rowIdx);
	TArray<FString> dataArray;
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++ i)
	{
		data.TemplateId.AddItem(appAtoi(*(dataArray(i))));
	}

	data.AchievementId = TReadDataValue_Int<HandbookDataManager>(TEXT("AchievementId"), dataRow, rowIdx);
	data.Name = TReadDataValue_FString<HandbookDataManager>(TEXT("Name"), dataRow, rowIdx);
	data.Text0 = TReadDataValue_FString<HandbookDataManager>(TEXT("Text0"), dataRow, rowIdx);
	data.Text1 = TReadDataValue_FString<HandbookDataManager>(TEXT("Text1"), dataRow, rowIdx);
	data.Text2 = TReadDataValue_FString<HandbookDataManager>(TEXT("Text2"), dataRow, rowIdx);
	data.Sound = TReadDataValue_FString<HandbookDataManager>(TEXT("Sound"), dataRow, rowIdx);
	if (data.Sound == TEXT("0"))
	{
		data.Sound = TEXT("");
	}

	// used for the release build resource list
	if (obj)
	{
		obj->Sound = data.Sound;
	}

	data.Icon = TReadDataValue_FString<HandbookDataManager>(TEXT("Icon"), dataRow, rowIdx);

	dataStr = TReadDataValue_FString<HandbookDataManager>(TEXT("Data0"), dataRow, rowIdx);
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++ i)
	{
		data.Data0.AddItem(appAtoi(*(dataArray(i))));
	}

	dataStr = TReadDataValue_FString<HandbookDataManager>(TEXT("Data1"), dataRow, rowIdx);
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++ i)
	{
		data.Data1.AddItem(appAtoi(*(dataArray(i))));
	}

	data.Scale = TReadDataValue_Float<HandbookDataManager>(TEXT("Scale"), dataRow, rowIdx);

	MMOHandbookData::getSingleton().AddHandbookData(data);
}

void HandbookDataProcessor::PostInitData()
{
}

#if WITH_EDITOR
void HandbookDataProcessor::CollectReferencedResourceFromDataTable(HandbookData* obj, TSet<FString>& outResList)
{
	if (obj)
	{
		//UObject* data = UObject::StaticLoadObject(USoundCue::StaticClass(), NULL, *(obj->Sound), NULL, LOAD_NoWarn | LOAD_Quiet, NULL);
		USoundCue* data = LoadUEObject<USoundCue>(obj->Sound, TEXT("handbook.txt"));
		if (data)
		{
			_CollectReferencedResource(data, outResList);
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
MMOHandbookData::MMOHandbookData()
{
	_HandbookDataMap.clear();
	_HandbookDataListMap.clear();
}

MMOHandbookData::~MMOHandbookData()
{
}

void MMOHandbookData::Init()
{
	Clear();
	LoadHandbookData(TEXT("handbook.txt"));
}

void MMOHandbookData::Shutdown()
{
	UnloadHandbookData();
	Clear();
}

void MMOHandbookData::Clear()
{
	appMemzero(_Handbook, sizeof(UINT32) * (htTotal * MAX_HANKBOOK_COUNT_PER_TYPE / 32));
}

UBOOL MMOHandbookData::LoadHandbookData(const TCHAR* filePath)
{
	_HandbookDataMap.clear();
	_HandbookDataListMap.clear();
	HandbookDataManager::getSingleton().InitFromFile(filePath, true);
	return FALSE;
}

void MMOHandbookData::UnloadHandbookData()
{
	_HandbookDataMap.clear();
	_HandbookDataListMap.clear();
}

UINT32 MMOHandbookData::_GetHandbookRealId(UINT32 type, UINT32 index)
{
	return  type << 16 | index;
}

void MMOHandbookData::AddHandbookData(const HandbookData& data)
{
	HandbookDataMap::iterator iterMap = _HandbookDataMap.find(_GetHandbookRealId(data.Type, data.Index));
	if (iterMap == _HandbookDataMap.end())
	{
		_HandbookDataMap.insert(std::make_pair(_GetHandbookRealId(data.Type, data.Index), data));
	}

	HandbookDataListMap::iterator iterList = _HandbookDataListMap.find(data.Type);
	if (iterList != _HandbookDataListMap.end())
	{
		HandbookDataList& list = iterList->second;
		list.push_back(data);
	}
	else
	{
		HandbookDataList list;
		list.clear();
		list.push_back(data);
		_HandbookDataListMap.insert(std::make_pair(data.Type, list));
	}
}

UBOOL MMOHandbookData::GetHandbookData(UINT32 type, UINT32 index, HandbookData& data)
{
	HandbookDataMap::iterator iter = _HandbookDataMap.find(_GetHandbookRealId(type, index));
	if (iter != _HandbookDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

UBOOL MMOHandbookData::GetHandbookDataList(UINT32 type, HandbookDataList& list)
{
	list.clear();
	HandbookDataListMap::iterator iter = _HandbookDataListMap.find(type);
	if (iter != _HandbookDataListMap.end())
	{
		list = iter->second;
		return TRUE;
	}

	return FALSE;
}

UBOOL MMOHandbookData::GetHandbookDataListByKey(UINT32 type, const FString& key, HandbookDataList& dataList)
{
	dataList.clear();
	HandbookDataListMap::iterator iterMap = _HandbookDataListMap.find(type);
	if (iterMap != _HandbookDataListMap.end())
	{
		HandbookDataList list = iterMap->second;
		HandbookDataList::iterator iterList = list.begin();
		for (; iterList != list.end(); ++ iterList)
		{
			HandbookData data = *iterList;
			if (data.Name.InStr(*key) >= 0)
			{
				dataList.push_back(data);
			}
		}
	}

	return dataList.size() > 0;
}

UBOOL MMOHandbookData::IsHandbookObtain(UINT32 type, UINT32 index)
{
	if (type >= htTotal)
	{
		return FALSE;
	}

	if (index >= MAX_HANKBOOK_COUNT_PER_TYPE)
	{
		return false;
	}

	return ((_Handbook[type][index / 32] >> (index % 32)) & 0x00000001) == 1;
}

UINT32 MMOHandbookData::GetCurHandbookCount(UINT32 type)
{
	UINT32 count = 0;
	HandbookDataListMap::iterator iterMap = _HandbookDataListMap.find(type);
	if (iterMap != _HandbookDataListMap.end())
	{
		HandbookDataList list = iterMap->second;
		HandbookDataList::iterator iterList = list.begin();
		for (; iterList != list.end(); ++ iterList)
		{
			HandbookData data = *iterList;
			if (IsHandbookObtain(data.Type, data.Index))
			{
				count ++;
			}
			else
			{
				continue;
			}
		}
	}
	
	return count;
}

UINT32 MMOHandbookData::GetMaxHandbookCount(UINT32 type)
{
	UINT32 count = 0;
	HandbookDataListMap::iterator iter = _HandbookDataListMap.find(type);
	if (iter != _HandbookDataListMap.end())
	{
		HandbookDataList list = iter->second;
		count = list.size();
	}

	return count;
}

void MMOHandbookData::SyncHandbookInfo(ROLE_DB_ACHIEVEMENT_DATA* data)
{
	if (!data)
	{
		return;
	}

	appMemcpy(_Handbook, data->dwHandbook, sizeof(UINT32) * (htTotal * MAX_HANKBOOK_COUNT_PER_TYPE / 32));
}

void MMOHandbookData::UpdateHandbookInfo(INT32 type, INT32 index)
{
	if (type > htInvalid && type < htTotal && index >= 0 && index < MAX_HANKBOOK_COUNT_PER_TYPE)
	{
		_Handbook[type][index / 32] |= (1 << (index % 32));
	}
}

UBOOL MMOHandbookData::GetMapHandbookInfoByName(const FString& name, UINT32& type, UINT32& index)
{
	for (UINT32 i = htMap_Wild; i <= htMap_Activity; ++ i)
	{
		HandbookDataList list;
		if (GetHandbookDataList(i, list))
		{
			HandbookDataList::iterator iterList = list.begin();
			for (; iterList != list.end(); ++ iterList)
			{
				HandbookData data = *iterList;
				if (data.Name == name)
				{
					type = data.Type;
					index = data.Index;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOHandbookData::GetMonsterHandbookInfoByName(const FString& name, UINT32& type, UINT32& index)
{
	for (UINT32 i = htNpc_Normal; i <= htNPc_Boss; ++ i)
	{
		HandbookDataList list;
		if (GetHandbookDataList(i, list))
		{
			HandbookDataList::iterator iterList = list.begin();
			for (; iterList != list.end(); ++ iterList)
			{
				HandbookData data = *iterList;
				if (data.Name == name)
				{
					type = data.Type;
					index = data.Index;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOHandbookData::GetEquipHandbookInfoById(INT itemId, UINT32& type, UINT32& index)
{
	for (UINT32 i = htEquip_Weapon; i <= htEquip_Armor; ++ i)
	{
		HandbookDataList list;
		if (GetHandbookDataList(i, list))
		{
			HandbookDataList::iterator iterList = list.begin();
			for (; iterList != list.end(); ++ iterList)
			{
				HandbookData data = *iterList;
				if (data.TemplateId.Num() > 0)
				{
					if ((INT)(data.TemplateId(0)) == itemId)
					{
						type = data.Type;
						index = data.Index;
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOHandbookData::GetStoryHandbookInfoById(INT itemId, UINT32& type, UINT32& index)
{
	HandbookDataList list;
	if (GetHandbookDataList(htItem_Story, list))
	{
		HandbookDataList::iterator iterList = list.begin();
		for (; iterList != list.end(); ++ iterList)
		{
			HandbookData data = *iterList;
			if (data.TemplateId.Num() > 0)
			{
				if ((INT)(data.TemplateId(0)) == itemId)
				{
					type = data.Type;
					index = data.Index;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}