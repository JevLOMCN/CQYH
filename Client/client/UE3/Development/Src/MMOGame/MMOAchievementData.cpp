#include "stdafx.h"
#include "MMOAchievementData.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "LinkManager.h"
#include "ScreenMsgQueue.h"
#include "ActivityManager.h"
#include "MMOQuestManager.h"
#include "MMOAchievementManager.h"
#include "MMOHeavenManager.h"
#include "MMOKillRecordManager.h"
#include "MMOGuideManager.h"
#include "MMOJourneyManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

const FString MMOAchievementData::S_AchievementClassName[AC_TOTAL] = 
{
	TEXT("Role"),
	TEXT("Accumulation"),
	TEXT("Fight"),
	TEXT("Adventure"),
	TEXT("Date"),
	TEXT("Epic"),
	TEXT("GuildA"),
	TEXT("GuildB"),
};

//////////////////////////////////////////////////////////////////////////
AchievementData* AchievementDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void AchievementDataProcessor::DestroyDataObject(AchievementData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void AchievementDataProcessor::ParseData(AchievementData* obj, class CTabFile* dataRow, int rowIdx)
{
	AchievementData data;

	data.Id = TReadDataValue_Int<AchievementDataManager>(TEXT("ID"), dataRow, rowIdx);
	data.Name = TReadDataValue_FString<AchievementDataManager>(TEXT("Name"), dataRow, rowIdx);
	data.Desc = TReadDataValue_FString<AchievementDataManager>(TEXT("Desc"), dataRow, rowIdx);
	data.Icon = TReadDataValue_FString<AchievementDataManager>(TEXT("Icon"), dataRow, rowIdx);
	data.BaseClass = TReadDataValue_Int<AchievementDataManager>(TEXT("BaseClass"), dataRow, rowIdx);
	data.SubClass = TReadDataValue_Int<AchievementDataManager>(TEXT("SubClass"), dataRow, rowIdx);
	data.DetailType = TReadDataValue_Int<AchievementDataManager>(TEXT("UIDetail"), dataRow, rowIdx);
	data.Hide = TReadDataValue_Int<AchievementDataManager>(TEXT("Hide"), dataRow, rowIdx);
	data.AutoTrace = TReadDataValue_Int<AchievementDataManager>(TEXT("AutoTrace"), dataRow, rowIdx);
	data.MinTraceLevel = TReadDataValue_Int<AchievementDataManager>(TEXT("MinTraceLevel"), dataRow, rowIdx);
	data.MaxTraceLevel = TReadDataValue_Int<AchievementDataManager>(TEXT("MaxTraceLevel"), dataRow, rowIdx);

	FString rowName = TEXT("");
	for (INT i = 0; i < MAX_CONDITION_TYPE_COUNT; ++ i)
	{
		rowName = FString::Printf(TEXT("ConditionDesc%d"), i);
		data.ConditionDesc[i] = TReadDataValue_FString<AchievementDataManager>(*rowName, dataRow, rowIdx);
		if (data.ConditionDesc[i] == TEXT("0"))
		{
			data.ConditionDesc[i] = TEXT("");
		}

		rowName = FString::Printf(TEXT("ConditionType%d"), i);
		data.ConditionType[i] = MMOFacade::getSingleton().FindClientEnumValue(TReadDataValue_FString<AchievementDataManager>(*rowName, dataRow, rowIdx),
			TEXT("EACHIENEMENT_DEF"));

		rowName = FString::Printf(TEXT("ConditionValue%d1"), i);
		if (data.ConditionType[i] == adVariable)
		{
			data.ConditionValue1[i] = MMOFacade::getSingleton().FindClientEnumValue(TReadDataValue_FString<AchievementDataManager>(*rowName, dataRow, rowIdx),
				TEXT("EACHIEVEMENT_VARIABLE_DEF"));
		}
		else
		{
			data.ConditionValue1[i] = TReadDataValue_Int<AchievementDataManager>(*rowName, dataRow, rowIdx);
		}

		rowName = FString::Printf(TEXT("ConditionValue%d2"), i);
		if (data.ConditionType[i] == adEquipBodyColor)
		{
			FString partString = TReadDataValue_FString<AchievementDataManager>(*rowName, dataRow, rowIdx);
			data.ConditionValue2[i] = MMOAchievementData::getSingleton().GetEquipPartId(partString);
		}
		else
		{
			data.ConditionValue2[i] = TReadDataValue_Int<AchievementDataManager>(*rowName, dataRow, rowIdx);
		}

		rowName = FString::Printf(TEXT("ConditionValue%d3"), i);
		data.ConditionValue3[i] = TReadDataValue_Int<AchievementDataManager>(*rowName, dataRow, rowIdx);
	}

	for (INT i = 0; i < MAX_REWARD_COUNT; ++ i)
	{
		rowName = FString::Printf(TEXT("RewardType%d"), i);
		data.RewardType[i] = MMOFacade::getSingleton().FindClientEnumValue(TReadDataValue_FString<AchievementDataManager>(*rowName, dataRow, rowIdx), 
			TEXT("EACHIEVEMENT_REWARD_TYPE"));

		rowName = FString::Printf(TEXT("RewardID%d"), i);
		data.RewardValue[i] = TReadDataValue_Int<AchievementDataManager>(*rowName, dataRow, rowIdx);
	}

	for (INT i = 0; i < MAX_PRE_ACHIEVEMENT_COUNT; ++ i)
	{
		rowName = FString::Printf(TEXT("PreAchievementID%d"), i + 1);
		data.PreAchievement[i] = TReadDataValue_Int<AchievementDataManager>(*rowName, dataRow, rowIdx);
	}

	data.Point = TReadDataValue_Int<AchievementDataManager>(TEXT("AchievementPts"), dataRow, rowIdx);

	MMOAchievementData::getSingleton().AddCommonAchievementData(data);
}

void AchievementDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
KillNpcAchievementData* KillNpcAchievementDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void KillNpcAchievementDataProcessor::DestroyDataObject(KillNpcAchievementData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void KillNpcAchievementDataProcessor::ParseData(KillNpcAchievementData* obj, class CTabFile* dataRow, int rowIdx)
{
	KillNpcAchievementData data;

	data.KillId = TReadDataValue_Int<KillNpcAchievementDataManager>(TEXT("QuestID"), dataRow, rowIdx);
	data.NpcTemplateId = TReadDataValue_Int<KillNpcAchievementDataManager>(TEXT("NpcTemplateID"), dataRow, rowIdx);
	data.NpcGroupId = TReadDataValue_Int<KillNpcAchievementDataManager>(TEXT("NpcGroupID"), dataRow, rowIdx);
	data.Desc = TReadDataValue_FString<KillNpcAchievementDataManager>(TEXT("Desc"), dataRow, rowIdx);
	data.Icon = TReadDataValue_FString<KillNpcAchievementDataManager>(TEXT("Icon"), dataRow, rowIdx);
	data.BaseClass = TReadDataValue_Int<KillNpcAchievementDataManager>(TEXT("BaseClass"), dataRow, rowIdx);
	data.SubClass = TReadDataValue_Int<KillNpcAchievementDataManager>(TEXT("SubClass"), dataRow, rowIdx);

	FString rowName = TEXT("");
	for (INT i = 0; i < MAX_KILL_NPC_ACHIEVEMENT_COUNT; ++ i)
	{
		rowName = FString::Printf(TEXT("Name%d"), i + 1);
		data.Name[i] = TReadDataValue_FString<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);
		if (data.Name[i] == TEXT("0"))
		{
			data.Name[i] = TEXT("");
		}

		rowName = FString::Printf(TEXT("KillCount%d"), i + 1);
		data.KillCount[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("AchievementPts%d"), i + 1);
		data.Point[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);
		
		rowName = FString::Printf(TEXT("RewardExp%d"), i + 1);
		data.RewardExp[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("Hide%d"), i + 1);
		data.Hide[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("AutoTrace%d"), i + 1);
		data.AutoTrace[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("MinTraceLevel%d"), i + 1);
		data.MinTraceLevel[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("MaxTraceLevel%d"), i + 1);
		data.MaxTraceLevel[i] = TReadDataValue_Int<KillNpcAchievementDataManager>(*rowName, dataRow, rowIdx);
	}

	MMOAchievementData::getSingleton().AddKillNpcAchievementData(data);
}

void KillNpcAchievementDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
GuildAchievementData* GuildAchievementDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GuildAchievementDataProcessor::DestroyDataObject(GuildAchievementData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuildAchievementDataProcessor::ParseData(GuildAchievementData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuildAchievementData data;

	data.Id = TReadDataValue_Int<GuildAchievementDataManager>(TEXT("ID"), dataRow, rowIdx);
	data.Name = TReadDataValue_FString<GuildAchievementDataManager>(TEXT("Name"), dataRow, rowIdx);
	data.Desc = TReadDataValue_FString<GuildAchievementDataManager>(TEXT("Desc"), dataRow, rowIdx);
	data.Icon = TReadDataValue_FString<GuildAchievementDataManager>(TEXT("Icon"), dataRow, rowIdx);
	data.BaseClass = TReadDataValue_Int<GuildAchievementDataManager>(TEXT("BaseClass"), dataRow, rowIdx);
	data.SubClass = TReadDataValue_Int<GuildAchievementDataManager>(TEXT("SubClass"), dataRow, rowIdx);
	data.Hide = TReadDataValue_Int<GuildAchievementDataManager>(TEXT("Hide"), dataRow, rowIdx);

	FString rowName = TEXT("");
	for (INT i = 0; i < MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT; ++ i)
	{
		rowName = FString::Printf(TEXT("ConditionType%d"), i + 1);
		data.ConditionType[i] = MMOAchievementData::getSingleton().GetGuildConditionType(TReadDataValue_FString<GuildAchievementDataManager>(*rowName, dataRow, rowIdx));

		rowName = FString::Printf(TEXT("Param%d"), i + 1);
		data.ConditionParam[i] = TReadDataValue_Int<GuildAchievementDataManager>(*rowName, dataRow, rowIdx);
	}

	for (INT i = 0; i < MAX_GUILD_ACHIEVEMENT_AWARD_COUNT; ++ i)
	{
		rowName = FString::Printf(TEXT("AwardType%d"), i + 1);
		data.RewardType[i] = MMOAchievementData::getSingleton().GetGuildAwardType(TReadDataValue_FString<GuildAchievementDataManager>(*rowName, dataRow, rowIdx));

		rowName = FString::Printf(TEXT("TemplateID%d"), i + 1);
		data.RewardTemp[i] = TReadDataValue_Int<GuildAchievementDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("Num%d"), i + 1);
		data.RewardCount[i] = TReadDataValue_Int<GuildAchievementDataManager>(*rowName, dataRow, rowIdx);
	}

	MMOAchievementData::getSingleton().AddGuildAchievementData(data);
}

void GuildAchievementDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
GuildConditionData* GuildConditionDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void GuildConditionDataProcessor::DestroyDataObject(GuildConditionData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuildConditionDataProcessor::ParseData(GuildConditionData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuildConditionData data;

	data.ConditionType = MMOAchievementData::getSingleton().GetGuildConditionType(TReadDataValue_FString<GuildConditionDataManager>(TEXT("CdtnType"), dataRow, rowIdx));
	data.ConditionPos = TReadDataValue_Int<GuildConditionDataManager>(TEXT("Pos"), dataRow, rowIdx);
	data.ConditionParam = TReadDataValue_Int<GuildConditionDataManager>(TEXT("Param"), dataRow, rowIdx);
	
	MMOAchievementData::getSingleton().AddGuildConditionData(data);
}

void GuildConditionDataProcessor::PostInitData()
{
}

//////////////////////////////////////////////////////////////////////////
MMOAchievementData::MMOAchievementData()
{
}

MMOAchievementData::~MMOAchievementData()
{
}

void MMOAchievementData::Init()
{
	_LoadAchievementData(TEXT("achievement.txt"));
	_LoadKillNpcAchievementData(TEXT("kill_npc_quest.txt"));
	_LoadGuildAchievementData(TEXT("guild_achievement.txt"));
	_LoadGuildConditionData(TEXT("guild_achi_cdtn.txt"));
	_AchievementInfoMap.clear();
	_AchievementClassMap.clear();
}

void MMOAchievementData::Shutdown()
{
	_AchievementClassMap.clear();
	_AchievementInfoMap.clear();
	_UnloadGuildConditionData();
	_UnloadGuildAchievementData();
	_UnloadKillNpcAchievementData();
	_UnloadAchievementData();
}

UINT32 MMOAchievementData::GetAchievementClassKey(UINT32 baseClass, UINT32 subClass)
{
	return (baseClass << 16) | subClass;
}

void MMOAchievementData::_AddAchievementClass(UINT32 baseClass, UINT32 subClass, UINT32 id)
{
	UINT32 classKey = GetAchievementClassKey(baseClass, subClass);
	AchievementClassMap::iterator iterClass = _AchievementClassMap.find(classKey);
	if (iterClass != _AchievementClassMap.end())
	{
		AchievementIdList& list = iterClass->second;
		list.push_back(id);
	}
	else
	{
		AchievementIdList list;
		list.clear();
		list.push_back(id);
		_AchievementClassMap.insert(std::make_pair(classKey, list));
	}
}

void MMOAchievementData::_InitCommonAchievementInfo()
{
	AchievementDataMap::iterator iterData = _AchievementDataMap.begin();
	for (; iterData != _AchievementDataMap.end(); ++ iterData)
	{
		AchievementData data = iterData->second;

		AchievementInfo info;
		info.Id = data.Id;
		if (data.DetailType == ADT_PROGRESS)
		{
			// If it is a counting achievement, only set the first condition
			if (data.ConditionType[0] == adTotal)
			{
				// adTotal is a condition that cannot be completed for now
				info.MaxProgress = 1;
			}
			else if (data.ConditionType[0] == adForceReputation ||
				data.ConditionType[0] == adVariousForceCount ||
				data.ConditionType[0] == adSpellLevelSpecified ||
				data.ConditionType[0] == adSpellLevel ||
				data.ConditionType[0] == adVariable ||
				data.ConditionType[0] == adUseItemIDCount || 
				data.ConditionType[0] == adTotalMoneyType ||
				//data.ConditionType[0] == adKillNpcIDCount ||
				//data.ConditionType[0] == adKillNpcGroupIDCount ||
				data.ConditionType[0] == adKilledByNpcIDCount ||
				data.ConditionType[0] == adColorItemDeCompose ||
				data.ConditionType[0] == adColorItemCompose ||
				data.ConditionType[0] == adRanklist)
			{
				info.MaxProgress = data.ConditionValue2[0];
			}
			else
			{
				info.MaxProgress = data.ConditionValue1[0];
			}
		}
		else
		{
			// Invalid conditions are considered complete
			for (INT i = 0; i < MAX_CONDITION_TYPE_COUNT; ++ i)
			{
				if (data.ConditionType[i] == adInvalid)
				{
					info.CurComplete |= 1 << i;
				}
			}
		}
		_AchievementInfoMap.insert(std::make_pair(info.Id, info));
		_AddAchievementClass(data.BaseClass, data.SubClass, info.Id);
	}
}

void MMOAchievementData::_InitKillNpcAchievementInfo()
{
	KillNpcAchievementDataMap::iterator iterData = _KillNpcAchievementDataMap.begin();
	for (; iterData != _KillNpcAchievementDataMap.end(); ++ iterData)
	{
		KillNpcAchievementData data = iterData->second;

		for (int i = 0; i < MAX_KILL_NPC_ACHIEVEMENT_COUNT; ++ i)
		{
			if (data.KillCount[i] > 0)
			{
				AchievementInfo info;
				info.Id = GetKillNpcAchievementId(data.KillId, i);
				info.MaxProgress = data.KillCount[i];
				_AchievementInfoMap.insert(std::make_pair(info.Id, info));
				_AddAchievementClass(data.BaseClass, data.SubClass, info.Id);
			}
		}
	}
}

void MMOAchievementData::_InitGuildAchievementInfo()
{
	GuildAchievementDataMap::iterator iterData = _GuildAchievementDataMap.begin();
	for (; iterData != _GuildAchievementDataMap.end(); ++ iterData)
	{
		GuildAchievementData data = iterData->second;

		AchievementInfo info;
		info.Id = ACHIEVEMENT_MAX + data.Id;
		
		// Invalid conditions are considered complete
		for (INT i = 0; i < MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT; ++ i)
		{
			if (data.ConditionType[i] == GUILD_ACHI_NONE)
			{
				info.CurComplete |= 1 << i;
			}
		}

		_AchievementInfoMap.insert(std::make_pair(info.Id, info));
		_AddAchievementClass(data.BaseClass, data.SubClass, info.Id);
	}
}

const AchievementClassMap& MMOAchievementData::GetAchievementClassMap()
{
	return _AchievementClassMap;
}

void MMOAchievementData::Clear()
{
	memset(_AchievementVariable, 0, sizeof(INT32) * MAX_ACHIEVEMENT_STATISTIC_COUNT);
	memset(_BaseClassCompleteCount, 0, sizeof(UINT32) * AC_TOTAL);
	memset(_BaseClassTotalCount, 0, sizeof(UINT32) * AC_TOTAL);

	_AchievementInfoMap.clear();
	_AchievementClassMap.clear();
	_AchievementCompleteList.clear();

	_InitCommonAchievementInfo();
	_InitKillNpcAchievementInfo();
	_InitGuildAchievementInfo();

	_AchievementCompleteList.clear();
	memset(_BaseClassCompleteCount, 0, sizeof(UINT32) * AC_TOTAL);
	memset(_BaseClassTotalCount, 0, sizeof(UINT32) * AC_TOTAL);

	_AchievementRewardSet.clear();

	_PersonalAchievementTraceSet.clear();
	_GuildAchievementTraceSet.clear();
}

const AchievementIdSet& MMOAchievementData::GetAchievementReward()
{
	return _AchievementRewardSet;
}

INT MMOAchievementData::GetEquipPartId(const FString& enumStr)
{
	if (enumStr == TEXT("NES_MAIN_HAND"))
	{
		return NES_MAIN_HAND;
	} 
	else if (enumStr == TEXT("NES_HEAD"))
	{
		return NES_HEAD;
	} 
	else if (enumStr == TEXT("NES_SHOULDER"))
	{
		return NES_SHOULDER;
	} 
	else if (enumStr == TEXT("NES_BODY"))
	{
		return NES_BODY;
	} 
	//else if (enumStr == TEXT("NES_LEG"))
	//{
	//	return NES_LEG;
	//} 
	else if (enumStr == TEXT("NES_ARM"))
	{
		return NES_ARM;
	} 
	else if (enumStr == TEXT("NES_WAIST"))
	{
		return NES_WAIST;
	} 
	else if (enumStr == TEXT("NES_FOOT"))
	{
		return NES_FOOT;
	} 
	else if (enumStr == TEXT("NES_BACK"))
	{
		return NES_BACK;
	} 
	//else if (enumStr == TEXT("NES_AVATAR"))
	//{
	//	return NES_AVATAR;
	//} 
	else if (enumStr == TEXT("NES_NECK"))
	{
		return NES_NECK;
	} 
	else if (enumStr == TEXT("NES_FINGER_L"))
	{
		return NES_FINGER_L;
	} 
	else if (enumStr == TEXT("NES_FINGER_R"))
	{
		return NES_FINGER_R;
	} 
	else if (enumStr == TEXT("NES_WRIST_L"))
	{
		return NES_WRIST_L;
	} 
	else if (enumStr == TEXT("NES_WRIST_R"))
	{
		return NES_WRIST_R;
	}

	return NES_TOTAL;
}

INT MMOAchievementData::GetGuildConditionType(const FString& enumStr)
{
	if (enumStr == TEXT("GUILD_ACHI_NONE"))
	{
		return GUILD_ACHI_NONE;
	} 
	else if (enumStr == TEXT("GUILD_ACHI_MEMBER"))
	{
		return GUILD_ACHI_MEMBER;
	} 
	else if (enumStr == TEXT("GUILD_ACHI_ENEMY"))
	{
		return GUILD_ACHI_ENEMY;
	} 
	else if (enumStr == TEXT("GUILD_ACHI_ALLIANCE"))
	{
		return GUILD_ACHI_ALLIANCE;
	} 
	else if (enumStr == TEXT("GUILD_ACHI_TAKE_POS_1"))
	{
		return GUILD_ACHI_TAKE_POS_1;
	} 
	else if (enumStr == TEXT("GUILD_ACHI_TAKE_POS_2"))
	{
		return GUILD_ACHI_TAKE_POS_2;
	} 
	
	return GUILD_ACHI_NONE;
}

INT MMOAchievementData::GetGuildAwardType(const FString& enumStr)
{
	if (enumStr == TEXT("GA_AWARD_NONE"))
	{
		return GA_AWARD_NONE;
	} 
	else if (enumStr == TEXT("GA_AWARD_POINT"))
	{
		return GA_AWARD_POINT;
	} 
	else if (enumStr == TEXT("GA_AWARD_EXP"))
	{
		return GA_AWARD_EXP;
	} 
	else if (enumStr == TEXT("GA_AWARD_ITEM"))
	{
		return GA_AWARD_ITEM;
	} 
	else if (enumStr == TEXT("GA_AWARD_TITLE"))
	{
		return GA_AWARD_TITLE;
	}

	return GA_AWARD_NONE;
}

UBOOL MMOAchievementData::_LoadAchievementData(const TCHAR* filePath)
{
	_AchievementDataMap.clear();
	AchievementDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOAchievementData::_UnloadAchievementData()
{
	_AchievementDataMap.clear();
}

void MMOAchievementData::AddCommonAchievementData(const AchievementData& data)
{
	_AchievementDataMap.insert(std::make_pair(data.Id, data));
}

UBOOL MMOAchievementData::GetCommonAchievementData(UINT16 id, AchievementData& data)
{
	AchievementDataMap::iterator iter = _AchievementDataMap.find(id);
	if (iter != _AchievementDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

const AchievementDataMap& MMOAchievementData::GetCommonAchievementDataMap()
{
	return _AchievementDataMap;
}

UBOOL MMOAchievementData::_LoadKillNpcAchievementData(const TCHAR* filePath)
{
	_KillNpcAchievementDataMap.clear();
	KillNpcAchievementDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOAchievementData::_UnloadKillNpcAchievementData()
{
	_KillNpcAchievementDataMap.clear();
}

void MMOAchievementData::AddKillNpcAchievementData(const KillNpcAchievementData& data)
{
	_KillNpcAchievementDataMap.insert(std::make_pair(data.KillId, data));
}

UBOOL MMOAchievementData::GetKillNpcAchievementData(UINT16 killId, KillNpcAchievementData& data)
{
	KillNpcAchievementDataMap::iterator iter = _KillNpcAchievementDataMap.find(killId);
	if (iter != _KillNpcAchievementDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

const KillNpcAchievementDataMap& MMOAchievementData::GetKillNpcAchievementDataMap()
{
	return _KillNpcAchievementDataMap;
}

UBOOL MMOAchievementData::GetAchievementInfo(UINT32 id, AchievementInfo& info)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
	if (iter != _AchievementInfoMap.end())
	{
		info = iter->second;
		return TRUE;
	}

	return FALSE;
}

UINT32 MMOAchievementData::GetBaseClassAchievementCompleteCount(UINT32 baseClass)
{
	return baseClass < AC_TOTAL ? _BaseClassCompleteCount[baseClass] : 0;
}

UINT32 MMOAchievementData::GetBaseClassAchievementTotalCount(UINT32 baseClass)
{
	return baseClass < AC_TOTAL ? _BaseClassTotalCount[baseClass] : 0;
}

UINT32 MMOAchievementData::GetSubClassAchievementCompleteCount(UINT32 baseClass, UINT32 subClass)
{
	UINT32 count = 0;
	AchievementIdList::iterator iter = _AchievementCompleteList.begin();
	for (; iter != _AchievementCompleteList.end(); ++ iter)
	{
		UINT32 id = *iter;
		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (GetCommonAchievementData(LOWORD(id), data))
			{
				if (data.BaseClass == baseClass && data.SubClass == subClass)
				{
					if (IsAchievementObtain(id))
					{
						count ++;
					}
				}
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			UINT16 dataId = GetGuildAchievementDataId(id);
			GuildAchievementData data;
			if (GetGuildAchievementData(dataId, data))
			{
				if (data.BaseClass == baseClass && data.SubClass == subClass)
				{
					if (IsAchievementObtain(id))
					{
						count ++;
					}
				}
			}
		}
		else
		{
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (GetKillNpcAchievementData(killId, data))
			{
				if (data.BaseClass == baseClass && data.SubClass == subClass)
				{
					if (IsAchievementObtain(id))
					{
						count ++;
					}
				}
			}
		}
	}
	return count;
}

UINT32 MMOAchievementData::GetSubClassAchievementTotalCount(UINT32 baseClass, UINT32 subClass)
{
	UINT32 count = 0;
	UINT32 classKey = GetAchievementClassKey(baseClass, subClass);
	AchievementClassMap::iterator iterClass = _AchievementClassMap.find(classKey);
	if (iterClass != _AchievementClassMap.end())
	{
		AchievementIdList list = iterClass->second;
		count = list.size();
	}
	return count;
}

bool GreatDate(UINT32 id1, UINT32 id2)
{
	AchievementInfo info1;
	AchievementInfo info2;
	if (MMOAchievementData::getSingleton().GetAchievementInfo(id1, info1) &&
		MMOAchievementData::getSingleton().GetAchievementInfo(id2, info2))
	{
		return info1.Date > info2.Date;
	}

	return false;
}

void MMOAchievementData::_RefreshAchievementCompleteList()
{
	// Recently obtained achievements
	_AchievementCompleteList.sort(GreatDate);

	// Special handling for achievements with adAchievement conditions
	AchievementIdList::iterator iterList = _AchievementCompleteList.begin();
	for (; iterList != _AchievementCompleteList.end(); ++ iterList)
	{
		MMOGameEvent_Achievement evtAch1(adAchievement, *iterList, 0, 0);
		MMOUI::PostGameEvent(&evtAch1);
	}

	MMOGameEvent_Achievement evtAch2(adAchievementCount, _AchievementCompleteList.size(), 0, 0);
	MMOUI::PostGameEvent(&evtAch2);
}

void MMOAchievementData::UpdateCommonAchievementList(ROLE_DB_ACHIEVEMENT_DATA* list)
{
	if (!list)
	{
		return;
	}

	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id >= ACHIEVEMENT_MAX)
		{
			continue;
		}

		AchievementData data;
		if (!GetCommonAchievementData(LOWORD(id), data))
		{
			continue;
		}

		// Sync obtain time and reward-claimed flag
		iter->second.Date = (UINT32)(list->wAchievement[data.Id]);
		iter->second.IsObtain = iter->second.Date != 0 ? TRUE : FALSE;
		iter->second.IsCompassable = iter->second.IsObtain;
		UINT32 rewardFlag = (list->dwAchievementReward[data.Id / 32] >> (data.Id % 32)) & 0x00000001;
		iter->second.IsGetReward = rewardFlag == 1 ? TRUE : FALSE;

		if (iter->second.IsObtain)
		{
			if (data.DetailType == ADT_PROGRESS)
			{
				// If it is a counting achievement, only set the first condition
				if (data.ConditionType[0] == adTotal)
				{
					// adTotal is a condition that cannot be completed for now
					iter->second.CurProgress = 1;
				}
				else if (data.ConditionType[0] == adForceReputation ||
					data.ConditionType[0] == adVariousForceCount ||
					data.ConditionType[0] == adSpellLevelSpecified ||
					data.ConditionType[0] == adSpellLevel ||
					data.ConditionType[0] == adVariable ||
					data.ConditionType[0] == adUseItemIDCount || 
					data.ConditionType[0] == adTotalMoneyType ||
					//data.ConditionType[0] == adKillNpcIDCount ||
					//data.ConditionType[0] == adKillNpcGroupIDCount ||
					data.ConditionType[0] == adKilledByNpcIDCount ||
					data.ConditionType[0] == adColorItemDeCompose ||
					data.ConditionType[0] == adColorItemCompose ||
					data.ConditionType[0] == adRanklist)
				{
					iter->second.CurProgress = data.ConditionValue2[0];
				}
				else
				{
					iter->second.CurProgress = data.ConditionValue1[0];
				}
			}
			else
			{
				iter->second.CurComplete = 0x3F;
			}

			_AchievementCompleteList.push_back(iter->first);

			if (!(iter->second.IsGetReward))
			{
				// If there is a reward, add to the reward achievement list
				for (int i = 0; i < MAX_REWARD_COUNT; ++ i)
				{
					if (data.RewardType[i] > 0)
					{
						_AchievementRewardSet.insert(id);
						break;
					}
				}
			}
		}

		// Statistics
		if (data.BaseClass < AC_TOTAL)
		{
			_BaseClassTotalCount[data.BaseClass] ++;
			_BaseClassCompleteCount[data.BaseClass] += iter->second.IsObtain ? 1 : 0;
		}
	}

	for (INT i = 0; i < MAX_ACHIEVEMENT_STATISTIC_COUNT; ++ i)
	{
		UpdateAchievementVariable(i, list->nVarList[i]);
	}

	_RefreshAchievementCompleteList();
}

void MMOAchievementData::UpdateKillNpcAchievementList(UINT16 killCount[MAX_KILL_NPC_QUEST_COUNT], UINT8 complete[MAX_KILL_NPC_QUEST_COUNT])
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id < KILL_NPC_ACHIEVEMENT_MIN)
		{
			continue;
		}

		UINT16 killId = HIWORD(id);
		UINT16 killIndex = LOWORD(id);

		KillNpcAchievementData data;
		if (!GetKillNpcAchievementData(killId, data))
		{
			continue;
		}

		if (killId >= MAX_KILL_NPC_QUEST_COUNT)
		{
			continue;
		}

		iter->second.Date = 0;
		iter->second.CurProgress = killCount[killId] >= data.KillCount[killIndex] ? data.KillCount[killIndex] : killCount[killId];
		iter->second.IsObtain = iter->second.CurProgress >= iter->second.MaxProgress;
		iter->second.IsCompassable = iter->second.IsObtain;
		iter->second.IsGetReward = complete[killId] & (1 << killIndex);

		if (iter->second.IsObtain)
		{
			_AchievementCompleteList.push_back(id);

			if (!(iter->second.IsGetReward))
			{
				// If there is a reward, add to the reward achievement list
				if (killIndex < MAX_KILL_NPC_ACHIEVEMENT_COUNT)
				{
					if (data.RewardExp[killIndex] > 0)
					{
						_AchievementRewardSet.insert(id);
					}
				}
			}
		}

		// Statistics
		if (data.BaseClass < AC_TOTAL)
		{
			_BaseClassTotalCount[data.BaseClass] ++;
			_BaseClassCompleteCount[data.BaseClass] += iter->second.IsObtain ? 1 : 0;
		}
	}

	_RefreshAchievementCompleteList();
}

void MMOAchievementData::UpdateAchievementVariable(INT32 index, INT32 value)
{
	if (index < 0 || index >= MAX_ACHIEVEMENT_STATISTIC_COUNT)
	{
		return;
	}

	_AchievementVariable[index] = value;
	/*
	if (index == avdActivity)
	{
		ActivityManager::getSingleton().NotifyActivityVariableErvtActivity(value);
	}
	else if (index == avdActivityReward)
	{
		ActivityManager::getSingleton().NotifyActivityVariableErvtActivityReward(value);
	}
	else 
	*/
	if (index == avdRewardQuestCompleteCount)
	{
		ActivityManager::getSingleton().NotifyActivityVariableErvtQuestComNum(value);
	}
	else if (index == avdQuestFlytoZone)
	{
		MMOQuestManager::getSingleton().NotifyFlyShoesUseTime(value);
	}
	else if (index == avdLastOfflineTime)
	{
		MMOHeavenManager::getSingleton().SetLastOffLineTime(value);
	}
	/*
	else if (index == avdKillPlayerLevelDuarion1 || 
		index == avdKillPlayerLevelDuarion2 || 
		index == avdKillPlayerLevelDuarion3)
	{
		switch (index)
		{
		case avdKillPlayerLevelDuarion1:
			break;
		case avdKillPlayerLevelDuarion2:
			break;
		case avdKillPlayerLevelDuarion3:
			break;
		}
	}
	*/
	else if (index == avdMaxConsecutiveKillNpcCount)
	{
		MMOKillRecordManager::getSingleton().NotifyKillMonsterRecord(value);
	}
	else if (index == avdMaxMomentaryKillNpcCount)
	{
		MMOKillRecordManager::getSingleton().NotifyKillMonsterMomentaryRecord(value);
	}
	else if (index == avdMaxConsecutiveKillPlayerCount)
	{
		MMOKillRecordManager::getSingleton().NotifyKillPlayerRecord(value);
	}
	else if (index == 111 || index == 112)
	{// Newbie gift pack and blessing oil achievement
		MMOGameEvent_Achievement evtAch(adUseItemIDCount, index, value, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdKillWarriorCount ||
		index == avdKillMageCount ||			
		index == avdKillRogueCount ||			
		index == avdKillArcherCount)
	{// Kill count
		UINT32 count = _AchievementVariable[avdKillWarriorCount] + 
			_AchievementVariable[avdKillMageCount] + 
			_AchievementVariable[avdKillRogueCount] + 
			_AchievementVariable[avdKillArcherCount];
		MMOGameEvent_Achievement evtAch(adKillPlayerTotalCount, count, 0, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdComposePurpleItem)
	{// Purple equipment synthesis count
		MMOGameEvent_Achievement evtAch(adColorItemCompose, ITEMCOLOR_PURPLE, _AchievementVariable[avdComposePurpleItem], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdComposeOrangeItem)
	{// Orange equipment synthesis count
		MMOGameEvent_Achievement evtAch(adColorItemCompose, ITEMCOLOR_ORANGE, _AchievementVariable[avdComposeOrangeItem], 0);
		MMOUI::PostGameEvent(&evtAch);

	}
	else if (index == avdDecomposePurpleItem)
	{// Purple equipment decomposition count
		MMOGameEvent_Achievement evtAch(adColorItemDeCompose, ITEMCOLOR_PURPLE, _AchievementVariable[avdDecomposePurpleItem], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdDecomposeOrangeItem)
	{// Orange equipment decomposition count
		MMOGameEvent_Achievement evtAch(adColorItemDeCompose, ITEMCOLOR_ORANGE, _AchievementVariable[avdDecomposeOrangeItem], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdRankPow)
	{// Leaderboard
		MMOGameEvent_Achievement evtAch(adRanklist, avdRankPow, _AchievementVariable[avdRankPow], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdRankLevel)
	{// Leaderboard
		MMOGameEvent_Achievement evtAch(adRanklist, avdRankLevel, _AchievementVariable[avdRankLevel], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdRankMoney)
	{// Leaderboard
		MMOGameEvent_Achievement evtAch(adRanklist, avdRankMoney, _AchievementVariable[avdRankMoney], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdRankPkValue)
	{// Leaderboard
		MMOGameEvent_Achievement evtAch(adRanklist, avdRankPkValue, _AchievementVariable[avdRankPkValue], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdRankMasterExp)
	{// Leaderboard
		MMOGameEvent_Achievement evtAch(adRanklist, avdRankMasterExp, _AchievementVariable[avdRankMasterExp], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdRankAchievementPts)
	{// Leaderboard
		MMOGameEvent_Achievement evtAch(adRanklist, avdRankAchievementPts, _AchievementVariable[avdRankAchievementPts], 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (index == avdKillNpcTotalCount)
	{
		MMOGameEvent_Achievement evtAch(adVariable, index, value, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else
	{
		MMOGameEvent_Achievement evtAch(adVariable, index, value, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
}

void MMOAchievementData::UpdateAchievementCompareList(ROLE_DB_ACHIEVEMENT_DATA* list)
{
	if (!list)
	{
		return;
	}

	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (GetCommonAchievementData(LOWORD(id), data))
			{
				iter->second.DateCompare = (UINT32)(list->wAchievement[data.Id]);
				iter->second.IsObtainCompare = iter->second.DateCompare != 0 ? TRUE : FALSE;
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			// Guild achievements have no obtain time and cannot be compared; written here for now, waiting for the server to add the feature
			UINT16 dataId = GetGuildAchievementDataId(id);
			GuildAchievementData data;
			if (GetGuildAchievementData(dataId, data))
			{
				iter->second.DateCompare = 0;
				iter->second.IsObtainCompare = FALSE;
			}
		}
		else
		{
			// Monster-kill achievements have no obtain time and cannot be compared; written here for now, waiting for the server to add the feature
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (GetKillNpcAchievementData(killId, data))
			{
				iter->second.DateCompare = 0;
				iter->second.IsObtainCompare = FALSE;
			}
		}
	}
}

UINT32 MMOAchievementData::GetAchievementCompleteCount()
{
	UINT32 count = 0;
	for (INT i = 0; i < AC_TOTAL; ++ i)
	{
		count += GetBaseClassAchievementCompleteCount(i);
	}
	return count;
}

void MMOAchievementData::GetAchievementRecentComplete(INT (&id)[MAX_RECENT_COMPLETE_COUNT], INT (&date)[MAX_RECENT_COMPLETE_COUNT])
{
	int index = 0;
	memset(id, 0, sizeof(INT) * MAX_RECENT_COMPLETE_COUNT);
	AchievementIdList::iterator iter = _AchievementCompleteList.begin();
	for (; iter != _AchievementCompleteList.end(); ++ iter)
	{
		if (index < MAX_RECENT_COMPLETE_COUNT)
		{
			AchievementInfo info;
			if (GetAchievementInfo(*iter, info))
			{
				id[index] = info.Id;
				date[index] = info.Date;
				index ++;
			}
		}
	}
}

UINT32 MMOAchievementData::GetAchievementLatestComplete()
{
	if (_AchievementCompleteList.empty())
	{
		return 0;
	}

	AchievementIdList::iterator iter = _AchievementCompleteList.begin();
	if (iter != _AchievementCompleteList.end())
	{
		return *iter;
	}

	return 0;
}

UINT32 MMOAchievementData::GetSubClassAchievementCompleteCountCompare(UINT32 baseClass, UINT32 subClass)
{
	UINT32 count = 0;
	UINT32 classKey = GetAchievementClassKey(baseClass, subClass);
	AchievementClassMap::iterator iterClass = _AchievementClassMap.find(classKey);
	if (iterClass != _AchievementClassMap.end())
	{
		AchievementIdList list = iterClass->second;
		AchievementIdList::iterator iterList = list.begin();
		for (; iterList != list.end(); ++ iterList)
		{
			AchievementInfo info;
			if (GetAchievementInfo(*iterList, info))
			{
				if (info.IsObtainCompare)
				{
					count ++;
				}
			}
		}
	}

	return count;
}

UINT32 MMOAchievementData::GetSubClassAchievementTotalCountCompare(UINT32 baseClass, UINT32 subClass)
{
	UINT32 count = 0;
	UINT32 classKey = GetAchievementClassKey(baseClass, subClass);
	AchievementClassMap::iterator iterClass = _AchievementClassMap.find(classKey);
	if (iterClass != _AchievementClassMap.end())
	{
		AchievementIdList list = iterClass->second;
		count = list.size();
	}
	return count;
}

UINT32 MMOAchievementData::GetAchievementPoint()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		return (UINT32)(mainPlayer->GetMoney(MONEYTYPE_ACHIEVEMENTPTS));
	}

	return 0;
}

UINT32 MMOAchievementData::GetAchievementPointCompare()
{
	UINT32 point = 0;
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		if (!iter->second.IsObtainCompare)
		{
			continue;
		}

		UINT32 id = (UINT32)(iter->first);
		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (GetCommonAchievementData(LOWORD(id), data))
			{
				point += data.Point;
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			// Guild achievements have no obtain time and cannot be compared; written here for now, waiting for the server to add the feature
			UINT16 dataId = GetGuildAchievementDataId(id);
			GuildAchievementData data;
			if (GetGuildAchievementData(dataId, data))
			{
				for (int i = 0; i < MAX_GUILD_ACHIEVEMENT_AWARD_COUNT; ++ i)
				{
					if (data.RewardType[i] == GA_AWARD_POINT)
					{
						point += data.RewardCount[i];
						break;
					}
				}
			}
		}
		else
		{
			// Monster-kill achievements have no obtain time and cannot be compared; written here for now, waiting for the server to add the feature
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (GetKillNpcAchievementData(killId, data))
			{
				point += data.Point[killIndex];
			}
		}
	}
	return point;
}

void MMOAchievementData::UpdateCommonAchievementInfo(INT type, INT value1, INT value2, INT value3)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id >= ACHIEVEMENT_MAX)
		{
			continue;
		}

		if (iter->second.IsObtain)
		{
			continue;
		}

		AchievementData data;
		if (!GetCommonAchievementData(LOWORD(id), data))
		{
			continue;
		}

		// Update condition
		if (data.DetailType == ADT_PROGRESS)
		{
			// If it is a counting achievement, only update the first condition
			if (data.ConditionType[0] == type)
			{
				if (data.ConditionType[0] == adTotal)
				{
					iter->second.CurProgress = 0;
				}
				else if (data.ConditionType[0] == adForceReputation ||
					data.ConditionType[0] == adSpellLevelSpecified ||
					data.ConditionType[0] == adSpellLevel ||
					data.ConditionType[0] == adVariable ||
					data.ConditionType[0] == adUseItemIDCount || 
					data.ConditionType[0] == adTotalMoneyType ||
					//data.ConditionType[0] == adKillNpcIDCount ||
					//data.ConditionType[0] == adKillNpcGroupIDCount ||
					data.ConditionType[0] == adKilledByNpcIDCount ||
					data.ConditionType[0] == adColorItemDeCompose ||
					data.ConditionType[0] == adColorItemCompose ||
					data.ConditionType[0] == adRanklist)
				{
					if (value1 == data.ConditionValue1[0])
					{
						iter->second.CurProgress = value2;
					}
				}
				else if (data.ConditionType[0] == adVariousForceCount)
				{
					/*
					INT32 forceCount = 0;
					for (INT32 forceId = 0; forceId < MAX_NPC_FORCE_ID; ++ forceId)
					{
						INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), forceId);
						if (reputation >= data.ConditionValue1[0])
						{
							forceCount ++;
						}
					}

					iter->second.CurProgress = forceCount;
					*/
				}
				else
				{
					iter->second.CurProgress = value1;
				}
			}

			iter->second.IsCompassable = iter->second.CurProgress >= iter->second.MaxProgress;
		}
		else
		{
			for (INT i = 0; i < MAX_CONDITION_TYPE_COUNT; ++ i)
			{
				if (type == data.ConditionType[i])
				{
					if (data.ConditionType[i] == adTotal)
					{
						iter->second.CurComplete = 0;
					}
					else if (data.ConditionType[i] == adForceReputation ||
						data.ConditionType[i] == adSpellLevel ||
						data.ConditionType[i] == adVariable ||
						data.ConditionType[i] == adUseItemIDCount || 
						data.ConditionType[i] == adTotalMoneyType ||
						//data.ConditionType[i] == adKillNpcIDCount ||
						//data.ConditionType[i] == adKillNpcGroupIDCount ||
						data.ConditionType[i] == adKilledByNpcIDCount ||
						data.ConditionType[i] == adColorItemDeCompose ||
						data.ConditionType[i] == adColorItemCompose)
					{
						if (value1 == data.ConditionValue1[i] && value2 >= data.ConditionValue2[i])
						{
							iter->second.CurComplete |= 1 << i;
							break;
						}
					}
					else if (data.ConditionType[i] == adEquipBodyColor ||
						data.ConditionType[i] == adSpellLevelSpecified)
					{
						if (value1 == data.ConditionValue1[i] && value2 == data.ConditionValue2[i])
						{
							iter->second.CurComplete |= 1 << i;
							break;
						}
					}
					else if (data.ConditionType[i] == adRanklist)
					{
						// Because it is a ranking, anything smaller than value2 also counts
						if (value1 == data.ConditionValue1[i] && value2 > 0 && value2 <= data.ConditionValue2[i])
						{
							iter->second.CurComplete |= 1 << i;
							break;
						}
					}
					else if (data.ConditionType[i] == adVariousForceCount)
					{
						/*
						INT32 forceCount = 0;
						for (INT32 forceId = 0; forceId < MAX_NPC_FORCE_ID; ++ forceId)
						{
							INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), forceId);
							if (reputation >= data.ConditionValue1[i])
							{
								forceCount ++;
							}
						}

						if (forceCount >= data.ConditionValue2[i])
						{
							iter->second.CurComplete |= 1 << i;
							break;
						}
						*/
					}
					else if (data.ConditionType[i] == adBagSlotCount ||
						data.ConditionType[i] == adBankSlotCount)
					{
						if (value1 >= data.ConditionValue1[i])
						{
							iter->second.CurComplete |= 1 << i;
							break;
						}
					}
					else
					{
						if (value1 == data.ConditionValue1[i])
						{
							iter->second.CurComplete |= 1 << i;
							break;
						}
					}
				}
			}

			iter->second.IsCompassable = iter->second.CurComplete == 0x3F;
		}

		if (iter->second.IsCompassable)
		{
			// Cache in a Map to avoid duplicate sends
			MMOAchievementManager::getSingleton().CompleteCommonAchievement(id);
		}

		AchievementIdSet::iterator iterTrace = _PersonalAchievementTraceSet.find(id);
		if (iterTrace != _PersonalAchievementTraceSet.end())
		{
			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);
		}
	}
}

void MMOAchievementData::UpdateGuildAchievementList(char info[GUILD_MAX_ACHI_ARRAY_LENGTH])
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id < ACHIEVEMENT_MAX || id >= KILL_NPC_ACHIEVEMENT_MIN)
		{
			continue;
		}

		UINT16 dataId = GetGuildAchievementDataId(id);
		GuildAchievementData data;
		if (!GetGuildAchievementData(dataId, data))
		{
			continue;
		}

		// Only achievements whose reward has been claimed are synced here
		iter->second.Date = 0;
		iter->second.IsGetReward = FALSE;
		if ((dataId / 8) < GUILD_MAX_ACHI_ARRAY_LENGTH)
		{
			iter->second.IsGetReward = (info[dataId / 8] >> (dataId % 8)) & 0x01;
		}

		iter->second.CurComplete = iter->second.IsGetReward ? 0xF : iter->second.CurComplete;
		iter->second.IsObtain = iter->second.CurComplete == 0xF;
		iter->second.IsCompassable = iter->second.IsObtain;

		// Server-online achievement completion sync and in-game reward claiming use the same protocol, so it can only be distinguished this way to avoid double counting
		if (MMO::GetCurrentGameState() != EGameState_InGame)
		{
			if (iter->second.IsObtain)
			{
				_AchievementCompleteList.push_back(id);
			}

			// Statistics
			if (data.BaseClass < AC_TOTAL)
			{
				_BaseClassTotalCount[data.BaseClass] ++;
				_BaseClassCompleteCount[data.BaseClass] += iter->second.IsObtain ? 1 : 0;
			}
		}
	}

	_RefreshAchievementCompleteList();
}

void MMOAchievementData::UpdateGuildAchievementInfo(char info[GUILD_ACHI_TYPE_NUM])
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id < ACHIEVEMENT_MAX || id >= KILL_NPC_ACHIEVEMENT_MIN)
		{
			continue;
		}

		if (iter->second.IsObtain)
		{
			continue;
		}

		UINT16 dataId = GetGuildAchievementDataId(id);
		GuildAchievementData data;
		if (!GetGuildAchievementData(dataId, data))
		{
			continue;
		}

		for (INT i = 0; i < MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT; ++ i)
		{
			UINT16 condType = data.ConditionType[i];
			UINT16 condParam = data.ConditionParam[i];

			if (condType > GUILD_ACHI_NONE && condType < GUILD_ACHI_TYPE_NUM)
			{
				UINT16 condPos = 0;
				if (GetGuildConditionPos(condType, condParam, condPos))
				{
					iter->second.CurComplete |= (info[condType] & (1 << condPos)) > 0 ? (1 << i) : 0;
				}
			}
		}

		iter->second.IsObtain = iter->second.CurComplete == 0xF;
		iter->second.IsCompassable = iter->second.IsObtain;

		if (iter->second.IsCompassable)
		{
			MMOAchievementManager::getSingleton().NotifyAchievementComplete(id, 0);
		}

		AchievementIdSet::iterator iterTrace = _GuildAchievementTraceSet.find(id);
		if (iterTrace != _GuildAchievementTraceSet.end())
		{
			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);
		}
	}
}

void MMOAchievementData::ClearGuildAchievementInfo()
{
	// fix bug  #23398 Guild achievements must be cleared when there is no guild (disbanded, kicked, left)
	_BaseClassCompleteCount[AC_GUILD1] = 0;
	_BaseClassCompleteCount[AC_GUILD2] = 0;
	_BaseClassCompleteCount[AC_GUILD3] = 0;
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id < ACHIEVEMENT_MAX || id >= KILL_NPC_ACHIEVEMENT_MIN)
		{
			continue;
		}

		UINT16 dataId = GetGuildAchievementDataId(id);
		GuildAchievementData data;
		if (!GetGuildAchievementData(dataId, data))
		{
			continue;
		}

		if (iter->second.IsGetReward)
		{
			if (data.BaseClass < AC_TOTAL)
			{
				_BaseClassCompleteCount[data.BaseClass] += 1;
			}

			continue;
		}

		iter->second.Date = 0;
		iter->second.CurComplete = 0;
		// Initialize completion conditions, invalid conditions are considered complete
		for (INT i = 0; i < MAX_GUILD_ACHIEVEMENT_CONDITION_COUNT; ++i)
		{
			if (data.ConditionType[i] == GUILD_ACHI_NONE)
			{
				iter->second.CurComplete |= 1 << i;
			}
		}

		iter->second.IsObtain = FALSE;
		iter->second.IsCompassable = FALSE;
		iter->second.IsGetReward = FALSE;

		AchievementIdList::iterator iterComplete = _AchievementCompleteList.begin();
		for (; iterComplete != _AchievementCompleteList.end(); ++iterComplete)
		{
			if (id == (UINT32)(*iterComplete))
			{
				_AchievementCompleteList.erase(iterComplete);
				break;
			}
		}

		AchievementIdSet::iterator iterReward = _AchievementRewardSet.find(id);
		if (iterReward != _AchievementRewardSet.end())
		{
			_AchievementRewardSet.erase(iterReward);
		}
	}
}

void MMOAchievementData::UpdateKillNpcAchievementInfo(UINT16 killId, UINT16 killCount)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);
		if (id < KILL_NPC_ACHIEVEMENT_MIN)
		{
			continue;
		}

		UINT16 curKillId = HIWORD(id);
		UINT16 curKillIndex = LOWORD(id);

		if (curKillId != killId)
		{
			continue;
		}

		if (iter->second.IsObtain)
		{
			continue;
		}

		KillNpcAchievementData data;
		if (!GetKillNpcAchievementData(curKillId, data))
		{
			continue;
		}

		iter->second.CurProgress = killCount >= data.KillCount[curKillIndex] ? data.KillCount[curKillIndex] : killCount;
		// The server does not notify whether monster-kill achievements are complete, judge here by count
		iter->second.IsObtain = iter->second.CurProgress >= iter->second.MaxProgress;
		iter->second.IsCompassable = iter->second.IsObtain;
		
		if (iter->second.IsCompassable)
		{
			MMOAchievementManager::getSingleton().NotifyAchievementComplete(id, 0);
		}

		AchievementIdSet::iterator iterTrace = _PersonalAchievementTraceSet.find(id);
		if (iterTrace != _PersonalAchievementTraceSet.end())
		{
			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);
		}
	}
}

void MMOAchievementData::UpdateAchievementCompleteInfo(UINT32 id, UINT32 date)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
	if (iter != _AchievementInfoMap.end())
	{
		iter->second.IsCompassable = TRUE;
		iter->second.IsObtain = TRUE;
		iter->second.Date = date;

		_AchievementCompleteList.push_front(iter->first);

		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (GetCommonAchievementData(LOWORD(id), data))
			{
				if (data.BaseClass < AC_TOTAL)
				{
					_BaseClassCompleteCount[data.BaseClass] += iter->second.IsObtain ? 1 : 0;
				}

				if (!(iter->second.IsGetReward))
				{
					// If there is a reward, add to the reward achievement list
					for (int i = 0; i < MAX_REWARD_COUNT; ++ i)
					{
						if (data.RewardType[i] > 0)
						{
							_AchievementRewardSet.insert(id);
							break;
						}
					}
				}
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			UINT16 dataId = GetGuildAchievementDataId(id);
			GuildAchievementData data;
			if (GetGuildAchievementData(dataId, data))
			{
				if (data.BaseClass < AC_TOTAL)
				{
					_BaseClassCompleteCount[data.BaseClass] += iter->second.IsObtain ? 1 : 0;
				}

				if (!(iter->second.IsGetReward))
				{
					// If there is a reward, add to the reward achievement list
					for (int i = 0; i < MAX_GUILD_ACHIEVEMENT_AWARD_COUNT; ++ i)
					{
						if (data.RewardType[i] != GA_AWARD_NONE)
						{
							_AchievementRewardSet.insert(id);
							break;
						}
					}
				}
			}
		}
		else
		{
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (GetKillNpcAchievementData(killId, data))
			{
				if (data.BaseClass < AC_TOTAL)
				{
					_BaseClassCompleteCount[data.BaseClass] += iter->second.IsObtain ? 1 : 0;
				}

				if (!(iter->second.IsGetReward))
				{
					// If there is a reward, add to the reward achievement list
					if (killIndex < MAX_KILL_NPC_ACHIEVEMENT_COUNT)
					{
						if (data.RewardExp[killIndex] > 0)
						{
							_AchievementRewardSet.insert(id);
						}
					}
				}
			}
		}

		// Cancel tracking
		RemoveTraceAchievement(id);
	}
}

UBOOL MMOAchievementData::IsAchievementObtain(UINT32 id)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
	if (iter != _AchievementInfoMap.end())
	{
		return iter->second.IsObtain;
	}

	return FALSE;
}

UBOOL MMOAchievementData::IsAchievementHasReward(UINT32 id)
{
	if (id < ACHIEVEMENT_MAX)
	{
		AchievementData data;
		if (GetCommonAchievementData(LOWORD(id), data))
		{
			for (int i = 0; i < MAX_REWARD_COUNT; ++ i)
			{
				if (data.RewardType[i] > 0)
				{
					return TRUE;
				}
			}
		}
	}
	else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		UINT16 dataId = GetGuildAchievementDataId(id);
		GuildAchievementData data;
		if (GetGuildAchievementData(dataId, data))
		{
			for (int i = 0; i < MAX_GUILD_ACHIEVEMENT_AWARD_COUNT; ++ i)
			{
				if (data.RewardType[i] > 0)
				{
					return TRUE;
				}
			}
		}
	}
	else
	{
		UINT16 killId = HIWORD(id);
		UINT16 killIndex = LOWORD(id);
		KillNpcAchievementData data;
		if (GetKillNpcAchievementData(killId, data))
		{
			if (killIndex < MAX_KILL_NPC_ACHIEVEMENT_COUNT)
			{
				if (data.RewardExp[killIndex] > 0)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

UBOOL MMOAchievementData::IsAchievementRewardObtain(UINT32 id)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
	if (iter != _AchievementInfoMap.end())
	{
		return iter->second.IsGetReward;
	}

	return FALSE;
}

void MMOAchievementData::UpdateAchievementRewardObtain(UINT32 id)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
	if (iter != _AchievementInfoMap.end())
	{
		iter->second.IsGetReward = TRUE;

	}

	// Delete one for each one claimed
	AchievementIdSet::iterator iterReward = _AchievementRewardSet.find(id);
	if (iterReward != _AchievementRewardSet.end())
	{
		_AchievementRewardSet.erase(iterReward);
	}
}

UINT32 MMOAchievementData::GetKillNpcAchievementId(UINT16 killId, UINT16 killIndex)
{
	return (killId << 16) | killIndex;
}

UBOOL MMOAchievementData::_LoadGuildAchievementData(const TCHAR* filePath)
{
	_GuildAchievementDataMap.clear();
	GuildAchievementDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOAchievementData::_UnloadGuildAchievementData()
{
	_GuildAchievementDataMap.clear();
}

void MMOAchievementData::AddGuildAchievementData(const GuildAchievementData& data)
{
	if (data.Id == 0)
	{
		return;
	}

	_GuildAchievementDataMap.insert(std::make_pair(data.Id, data));
}

UBOOL MMOAchievementData::GetGuildAchievementData(UINT16 id, GuildAchievementData& data)
{
	GuildAchievementDataMap::iterator iter = _GuildAchievementDataMap.find(id);
	if (iter != _GuildAchievementDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

const GuildAchievementDataMap& MMOAchievementData::GetGuildAchievementDataMap()
{
	return _GuildAchievementDataMap;
}

UINT16 MMOAchievementData::GetGuildAchievementDataId(UINT32 infoId)
{
	return (LOWORD(infoId) - ACHIEVEMENT_MAX);
}
//////////////////////////////////////////////////////////////////////////
UBOOL MMOAchievementData::_LoadGuildConditionData(const TCHAR* filePath)
{
	_GuildConditionDataMap.clear();
	GuildConditionDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOAchievementData::_UnloadGuildConditionData()
{
	_GuildConditionDataMap.clear();
}

void MMOAchievementData::AddGuildConditionData(const GuildConditionData& data)
{
	UINT32 condId = (data.ConditionType << 16) | data.ConditionParam;
	_GuildConditionDataMap.insert(std::make_pair(condId, data));
}

UBOOL MMOAchievementData::GetGuildConditionPos(UINT16 condType, UINT16 condParam, UINT16& condPos)
{
	UINT32 condId = (condType << 16) | condParam;
	GuildConditionDataMap::iterator iter = _GuildConditionDataMap.find(condId);
	if (iter != _GuildConditionDataMap.end())
	{
		condPos = iter->second.ConditionPos;
		return TRUE;
	}

	return FALSE;
}

void MMOAchievementData::AddTraceAchievement(UINT32 id)
{
	if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		if (_GuildAchievementTraceSet.size() < MAX_TRACE_ACHIEVEMENT_COUNT)
		{
			_GuildAchievementTraceSet.insert(id);

			AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
			if (iter != _AchievementInfoMap.end())
			{
				iter->second.IsTraced = TRUE;
			}

			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_AddAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);

			// Save achievement tracking
			int index = 0;
			UINT32 traceId = 0;
			AchievementIdSet::iterator iterSet = _GuildAchievementTraceSet.begin();
			for (; iterSet != _GuildAchievementTraceSet.end(); ++ iterSet)
			{
				if (index < MAX_TRACE_ACHIEVEMENT_COUNT)
				{
					traceId |= (*iterSet) << (index * 16);
				}

				index ++;
			}
			MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Achievement_Trace_Guild, traceId);
		}
	}
	else
	{
		if (_PersonalAchievementTraceSet.size() < MAX_TRACE_ACHIEVEMENT_COUNT)
		{
			_PersonalAchievementTraceSet.insert(id);

			AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
			if (iter != _AchievementInfoMap.end())
			{
				iter->second.IsTraced = TRUE;
			}

			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_AddAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);

			// Save achievement tracking
			int index = 0;
			AchievementIdSet::iterator iterSet = _PersonalAchievementTraceSet.begin();
			for (; iterSet != _PersonalAchievementTraceSet.end(); ++ iterSet)
			{
				if (index < MAX_TRACE_ACHIEVEMENT_COUNT)
				{
					MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)(ECVT_Achievement_Trace_Personal1 + index), *iterSet);
				}

				index ++;
			}
		}
	}
}

void MMOAchievementData::RemoveTraceAchievement(UINT32 id)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.find(id);
	if (iter != _AchievementInfoMap.end())
	{
		iter->second.IsTraced = FALSE;
	}

	if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		AchievementIdSet::iterator iterSet = _GuildAchievementTraceSet.find(id);
		if (iterSet != _GuildAchievementTraceSet.end())
		{
			_GuildAchievementTraceSet.erase(iterSet);

			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_RemoveAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);
		}

		// Save achievement tracking
		int index = 0;
		UINT32 traceId = 0;
		iterSet = _GuildAchievementTraceSet.begin();
		for (; iterSet != _GuildAchievementTraceSet.end(); ++ iterSet)
		{
			if (index < MAX_TRACE_ACHIEVEMENT_COUNT)
			{
				traceId |= (*iterSet) << (index * 16);
			}

			index ++;
		}
		MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Achievement_Trace_Guild, traceId);
	}
	else
	{
		AchievementIdSet::iterator iterSet = _PersonalAchievementTraceSet.find(id);
		if (iterSet != _PersonalAchievementTraceSet.end())
		{
			_PersonalAchievementTraceSet.erase(iterSet);

			MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_RemoveAchievement, id);
			MMOUI::PostGameEvent(&evtJourney);
		}

		// Save achievement tracking
		int index = 0;
		INT32 traceId[MAX_TRACE_ACHIEVEMENT_COUNT];
		appMemset(&traceId, 0, sizeof(INT32) * MAX_TRACE_ACHIEVEMENT_COUNT);
		
		iterSet = _PersonalAchievementTraceSet.begin();
		for (; iterSet != _PersonalAchievementTraceSet.end(); ++ iterSet)
		{
			if (index < MAX_TRACE_ACHIEVEMENT_COUNT)
			{
				traceId[index] = *iterSet;
			}

			index ++;
		}

		for (INT i = 0; i < MAX_TRACE_ACHIEVEMENT_COUNT; ++ i)
		{
			MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)(ECVT_Achievement_Trace_Personal1 + i), traceId[i]);
		}
	}
}

void MMOAchievementData::UpdateTraceAchievement(UINT32 level)
{
	AchievementInfoMap::iterator iter = _AchievementInfoMap.begin();
	for (; iter != _AchievementInfoMap.end(); ++ iter)
	{
		UINT32 id = (UINT32)(iter->first);

		if (iter->second.IsObtain)
		{
			iter->second.IsTraced = FALSE;
			RemoveTraceAchievement(id);
			
			continue;
		}

		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (GetCommonAchievementData(LOWORD(id), data))
			{
				if (data.AutoTrace)
				{
					if (level >= data.MinTraceLevel && level <= data.MaxTraceLevel)
					{
						iter->second.IsTraced = _PersonalAchievementTraceSet.size() < MAX_TRACE_ACHIEVEMENT_COUNT;
						AddTraceAchievement(id);
					}
					else
					{
						iter->second.IsTraced = FALSE;
						RemoveTraceAchievement(id);
					}
				}
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			// Guild achievements are not auto-tracked
			continue;
		}
		else
		{
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (GetKillNpcAchievementData(killId, data))
			{
				if (data.AutoTrace[killIndex])
				{
					if (level >= data.MinTraceLevel[killIndex] && level <= data.MaxTraceLevel[killIndex])
					{
						iter->second.IsTraced = _PersonalAchievementTraceSet.size() < MAX_TRACE_ACHIEVEMENT_COUNT;
						AddTraceAchievement(id);
					}
					else
					{
						iter->second.IsTraced = FALSE;
						RemoveTraceAchievement(id);
					}
				}
			}
		}
	}
}

const AchievementIdSet& MMOAchievementData::GetPersonalAchievementBeTraced()
{
	return _PersonalAchievementTraceSet;
}

const AchievementIdSet& MMOAchievementData::GetGuildAchievementBeTraced()
{
	return _GuildAchievementTraceSet;
}

