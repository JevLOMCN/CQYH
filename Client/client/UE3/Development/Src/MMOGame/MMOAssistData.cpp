#include "stdafx.h"
#include "MMOFacade.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOAssistData.h"

using namespace ExternalMessage;

//////////////////////////////////////////////////////////////////////////
AssistDropData* AssistDropDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void AssistDropDataProcessor::DestroyDataObject(AssistDropData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void AssistDropDataProcessor::ParseData(AssistDropData* obj, class CTabFile* dataRow, int rowIdx)
{
	AssistDropData data;
	
	data.Id = TReadDataValue_Int<AssistDropDataManager>(TEXT("ID"), dataRow, rowIdx);
	if (data.Id == 0)
	{
		return;
	}

	data.ItemClass = TReadDataValue_Int<AssistDropDataManager>(TEXT("ItemClass"), dataRow, rowIdx);
	data.Valuable = TReadDataValue_Int<AssistDropDataManager>(TEXT("Valuable"), dataRow, rowIdx);
	data.AutoPick = TReadDataValue_Int<AssistDropDataManager>(TEXT("AutoPick"), dataRow, rowIdx);
	data.Visible = TReadDataValue_Int<AssistDropDataManager>(TEXT("Visible"), dataRow, rowIdx);
	data.Group = TReadDataValue_Int<AssistDropDataManager>(TEXT("Group"), dataRow, rowIdx);
	
	MMOAssistData::getSingleton().AddAssistDropData(data);

	AssistDropInfo info;
	info.Id = data.Id;
	info.Valuable = data.Valuable;
	info.AutoPick = data.AutoPick;
	info.Visible = data.Visible;
	MMOAssistData::getSingleton().AddAssistDropInfo(info);
}

void AssistDropDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
AssistItemData* AssistItemDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void AssistItemDataProcessor::DestroyDataObject(AssistItemData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void AssistItemDataProcessor::ParseData(AssistItemData* obj, class CTabFile* dataRow, int rowIdx)
{
	AssistItemData data;

	data.ItemId = TReadDataValue_Int<AssistItemDataManager>(TEXT("ItemId"), dataRow, rowIdx);
	data.ItemType = TReadDataValue_Int<AssistItemDataManager>(TEXT("ItemType"), dataRow, rowIdx);
	data.Sort = TReadDataValue_Int<AssistItemDataManager>(TEXT("Sort"), dataRow, rowIdx);

	MMOAssistData::getSingleton().AddAssistItemData(data);

}

void AssistItemDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
MMOAssistData::MMOAssistData()
{
}

MMOAssistData::~MMOAssistData()
{
}

void MMOAssistData::Init()
{
	Clear();
	_LoadAssistDropData(TEXT("AssistDrop.txt"));
	_LoadAssistItemData(TEXT("AssistItem.txt"));
}

void MMOAssistData::Shutdown()
{
	_UnloadAssistItemData();
	_UnloadAssistDropData();
	Clear();
}

void MMOAssistData::Clear()
{
}

UBOOL MMOAssistData::_LoadAssistDropData(const TCHAR* filePath)
{
	_AssistDropDataMap.clear();
	_AssistDropInfoMap.clear();
	AssistDropDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOAssistData::_UnloadAssistDropData()
{
	_AssistDropDataMap.clear();
	_AssistDropInfoMap.clear();
}

void MMOAssistData::AddAssistDropData(const AssistDropData& data)
{
	_AssistDropDataMap.insert(std::make_pair(data.Id, data));
}

UBOOL MMOAssistData::GetAssistDropData(UINT32 id, AssistDropData& data)
{
	AssistDropDataMap::iterator iter = _AssistDropDataMap.find(id);
	if (iter != _AssistDropDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

void MMOAssistData::AddAssistDropInfo(const AssistDropInfo& info)
{
	AssistDropInfoMap::iterator iter = _AssistDropInfoMap.find(info.Id);
	if (iter != _AssistDropInfoMap.end())
	{
		iter->second = info;
	}
	else
	{
		_AssistDropInfoMap.insert(std::make_pair(info.Id, info));
	}
}

AssistDropDataMap& MMOAssistData::GetAssistDropDataMap()
{
	return _AssistDropDataMap;
}

AssistDropInfoMap& MMOAssistData::GetAssistDropInfoMap()
{
	return _AssistDropInfoMap;
}

void MMOAssistData::UpdateAssistDropInfo(UINT32 id, UBOOL valuable, UBOOL autoPick, UBOOL visible)
{
	AssistDropInfoMap::iterator iter = _AssistDropInfoMap.find(id);
	if (iter != _AssistDropInfoMap.end())
	{
		iter->second.Valuable = valuable;
		iter->second.AutoPick = autoPick;
		iter->second.Visible = visible;
	}
}

void MMOAssistData::UpdateAssistDropInfoDefault()
{
	AssistDropDataMap::iterator iter = _AssistDropDataMap.begin();
	for (; iter != _AssistDropDataMap.end(); ++ iter)
	{
		UpdateAssistDropInfo(iter->first, 
			iter->second.Valuable, 
			iter->second.AutoPick, 
			iter->second.Visible);
	}
}

UBOOL MMOAssistData::IsDropValuable(UINT32 id)
{
	AssistDropInfoMap::iterator iter = _AssistDropInfoMap.find(id);
	if (iter != _AssistDropInfoMap.end())
	{
		return iter->second.Valuable;
	}

	return FALSE;
}

UBOOL MMOAssistData::IsDropAutoPick(UINT32 id)
{
	AssistDropInfoMap::iterator iter = _AssistDropInfoMap.find(id);
	if (iter != _AssistDropInfoMap.end())
	{
		return iter->second.AutoPick;
	}

	// Default to auto-pickup if not configured in the table
	return TRUE;
}

UBOOL MMOAssistData::IsDropVisible(UINT32 id)
{
	AssistDropInfoMap::iterator iter = _AssistDropInfoMap.find(id);
	if (iter != _AssistDropInfoMap.end())
	{
		return iter->second.Visible;
	}

	// Default to visible if not configured in the table
	return TRUE;
}

bool less_sort(const AssistItemData& data1, const AssistItemData& data2)
{
	return data1.Sort < data2.Sort;
}

UBOOL MMOAssistData::_LoadAssistItemData(const TCHAR* filePath)
{
	_AssistItemDataMap.clear();
	_AssistItemSlowHpList.clear();
	_AssistItemSlowMpList.clear();

	AssistItemDataManager::getSingleton().InitFromFile(filePath, true);

	_AssistItemSlowHpList.sort(less_sort);
	_AssistItemSlowMpList.sort(less_sort);

	return TRUE;
}

void MMOAssistData::_UnloadAssistItemData()
{
	_AssistItemDataMap.clear();
	_AssistItemSlowHpList.clear();
	_AssistItemSlowMpList.clear();
}

void MMOAssistData::AddAssistItemData(const AssistItemData& data)
{
	_AssistItemDataMap.insert(std::make_pair(data.ItemId, data));

	if (data.ItemType == EAIT_SLOW_HP || data.ItemType == EAIT_SLOW_HPMP)
	{
		_AssistItemSlowHpList.push_back(data);
	}

	if (data.ItemType == EAIT_SLOW_MP || data.ItemType == EAIT_SLOW_HPMP)
	{
		_AssistItemSlowMpList.push_back(data);
	}
}

AssistItemDataMap& MMOAssistData::GetAssistItemDataMap()
{
	return _AssistItemDataMap;
}

UBOOL MMOAssistData::GetAssistItemData(UINT32 id, AssistItemData& data)
{
	AssistItemDataMap::iterator iter = _AssistItemDataMap.find(id);
	if (iter != _AssistItemDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

UBOOL MMOAssistData::GetAssistItemInfo(UINT8 protectType, AssistItemInfo& info)
{
	if (protectType < EPT_SLOW_HP || protectType >= EPT_TOTAL)
	{
		return FALSE;
	}
	
	info = _AssistItemInfo[protectType];
	return TRUE;
}

void MMOAssistData::UpdateAssistItemInfo(UINT8 protectType, const AssistItemInfo& info)
{
	if (protectType < EPT_SLOW_HP || protectType >= EPT_TOTAL)
	{
		return;
	}

	_AssistItemInfo[protectType] = info;
}

void MMOAssistData::UpdateAssistItemInfoDefault()
{
	for (INT i = EPT_SLOW_HP; i < EPT_TOTAL; ++ i)
	{
		_AssistItemInfo[i].ItemId = 0;
		_AssistItemInfo[i].Enable = FALSE;
		_AssistItemInfo[i].ProtectType = i;
		_AssistItemInfo[i].Value = 0;
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer && mainPlayer->ActorAttributes)
		{
			switch (i)
			{
			case EPT_SLOW_HP:
				_AssistItemInfo[i].Value = DEFAULT_PROTECT_VALUE;
				break;

			case EPT_FAST_HP1:
			case EPT_FAST_HP2:
			case EPT_PROTECT_HP:
				{
					// #20091
					//_AssistItemInfo[i].Value = (UINT32)((FLOAT)(mainPlayer->ActorAttributes->GetMaxHp()) * 0.5);
					_AssistItemInfo[i].Value = 0;
				}
				break;

			case EPT_SLOW_MP:
				_AssistItemInfo[i].Value = DEFAULT_PROTECT_VALUE;
				break;

			case EPT_FAST_MP1:
			case EPT_FAST_MP2:
			case EPT_PROTECT_MP:
				{
					// #20091
					//_AssistItemInfo[i].Value = (UINT32)((FLOAT)(mainPlayer->ActorAttributes->GetMaxMp()) * 0.5);
					_AssistItemInfo[i].Value = 0;
				}
				break;
			}
		}
		_AssistItemInfo[i].Time = DEFAULT_PROTECT_TIME;
	}
}

AssistItemDataList& MMOAssistData::GetAssistItemSlowHpList()
{
	return _AssistItemSlowHpList;
}

AssistItemDataList& MMOAssistData::GetAssistItemSlowMpList()
{
	return _AssistItemSlowMpList;
}

UINT32 MMOAssistData::GetAssistItemIdByIndex(UINT8 protectType, UINT8 index)
{
	UINT8 itemType = EAIT_MAX;
	switch (protectType)
	{
	case EPT_SLOW_HP:
		itemType = EAIT_SLOW_HP;
		break;
	case EPT_SLOW_MP:
		itemType = EAIT_SLOW_MP;
		break;
	case EPT_FAST_HP1:
	case EPT_FAST_MP1:
	case EPT_FAST_HP2:
	case EPT_FAST_MP2:
		itemType = EAIT_FAST_HPMP;
		break;
	case EPT_PROTECT_HP:
	case EPT_PROTECT_MP:
		itemType = EAIT_PROTECT_HPMP;
		break;
	}

	AssistItemDataMap::iterator iter = _AssistItemDataMap.begin();
	for (; iter != _AssistItemDataMap.end(); ++ iter)
	{
		if (iter->second.ItemType == itemType && iter->second.Sort == (index + 1))
		{
			return iter->first;
		}
	}

	return 0;
}

UINT8 MMOAssistData::GetAssistItemIndexById(UINT8 protectType, UINT32 itemId)
{
	if (protectType == EPT_SLOW_HP || protectType == EPT_SLOW_MP)
	{
		return 0;
	}

	UINT8 itemType = EAIT_MAX;
	switch (protectType)
	{
	case EPT_FAST_HP1:
	case EPT_FAST_MP1:
	case EPT_FAST_HP2:
	case EPT_FAST_MP2:
		itemType = EAIT_FAST_HPMP;
		break;
	case EPT_PROTECT_HP:
	case EPT_PROTECT_MP:
		itemType = EAIT_PROTECT_HPMP;
		break;
	}

	AssistItemDataMap::iterator iter = _AssistItemDataMap.begin();
	for (; iter != _AssistItemDataMap.end(); ++iter)
	{
		if (iter->second.ItemType == itemType && iter->second.ItemId == itemId)
		{
			return (iter->second.Sort - 1);
		}
	}

	return 0;
}