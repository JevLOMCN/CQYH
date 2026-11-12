#include "stdafx.h"
#include "EngineAudioDeviceClasses.h"
#include "MMOFacade.h"
#include "MMOJourneyData.h"
#include "UITextParser.h"
#include "MasterStudentManager.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"		
#include "CommonGameEvents.h"
#include "MMOAchievementManager.h"
#include "MMOGuideManager.h"
#include "MMOChatManager.h"
#include "MMOGameClasses.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuildManager.h"
#include "DoodadManager.h"
#include "MMODropManager.h"
#include "MMOQuestManager.h"
#include "FriendManager.h"
#include "ActivityManager.h"
#include "MMOVipManager.h"
#include "MMOSkillManager.h"
#include "MMOClientVariableManager.h"

//////////////////////////////////////////////////////////////////////////
JourneyData* JourneyDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	JourneyData* obj = JourneyDataManager::getSingleton().FindAndModifyData(templateID);
	if (obj)
	{
		needAddToMap = false;
	}
	else
	{
		obj = new JourneyData();
		if (obj)
		{
			obj->Id = templateID;
		}
		needAddToMap = true;
	}

	return obj;
}

void JourneyDataProcessor::DestroyDataObject(JourneyData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void JourneyDataProcessor::ParseData(JourneyData* obj, class CTabFile* dataRow, int rowIdx)
{
	JourneyData data;

	data.Id = TReadDataValue_Int<JourneyDataManager>(TEXT("JourneyId"), dataRow, rowIdx);
	data.Title = TReadDataValue_FString<JourneyDataManager>(TEXT("JourneyTitle"), dataRow, rowIdx);
	data.NpcName = TReadDataValue_FString<JourneyDataManager>(TEXT("NpcName"), dataRow, rowIdx);
	data.Icon = TReadDataValue_FString<JourneyDataManager>(TEXT("JourneyIcon"), dataRow, rowIdx);
	data.Desc = TReadDataValue_FString<JourneyDataManager>(TEXT("JourneyDesc"), dataRow, rowIdx);
	data.MinLevel = TReadDataValue_Int<JourneyDataManager>(TEXT("MinLevel"), dataRow, rowIdx);
	data.MaxLevel = TReadDataValue_Int<JourneyDataManager>(TEXT("MaxLevel"), dataRow, rowIdx);
	data.ForceId = TReadDataValue_Int<JourneyDataManager>(TEXT("ForceId"), dataRow, rowIdx);
	data.StoryQuestStar = TReadDataValue_Int<JourneyDataManager>(TEXT("StoryQuestStar"), dataRow, rowIdx);
	data.MonsterStar = TReadDataValue_Int<JourneyDataManager>(TEXT("MonsterStar"), dataRow, rowIdx);
	data.EmegencyQuestStar = TReadDataValue_Int<JourneyDataManager>(TEXT("EventStar"), dataRow, rowIdx);
	data.BossStar = TReadDataValue_Int<JourneyDataManager>(TEXT("BossStar"), dataRow, rowIdx);
	data.ActivityQuestStar = TReadDataValue_Int<JourneyDataManager>(TEXT("ActivityQuestStar"), dataRow, rowIdx);
	data.GuideQuestStar = TReadDataValue_Int<JourneyDataManager>(TEXT("GuideQuestStar"), dataRow, rowIdx);
	data.AchievementStar = TReadDataValue_Int<JourneyDataManager>(TEXT("AchievementStar"), dataRow, rowIdx);
	data.GuildStar = TReadDataValue_Int<JourneyDataManager>(TEXT("GuildStar"), dataRow, rowIdx);
	data.AchievementBaseClass = TReadDataValue_Int<JourneyDataManager>(TEXT("AchievementBaseClass"), dataRow, rowIdx);
	data.AchievementSubClass = TReadDataValue_Int<JourneyDataManager>(TEXT("AchievementSubClass"), dataRow, rowIdx);

	FString dataStr = TReadDataValue_FString<JourneyDataManager>(TEXT("JourneyAward"), dataRow, rowIdx);
	TArray<FString> dataArray;
	dataArray.Empty();
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++ i)
	{
		data.AwardList.AddItem(appAtoi(*(dataArray(i))));
	}
	
	for (INT i = 0; i < MAX_JOURNEY_TALK_COUNT; ++ i)
	{
		FString text = FString::Printf(TEXT("JourneyTalk%d"), i);
		FString talk = TReadDataValue_FString<JourneyDataManager>(*text, dataRow, rowIdx);
		if (talk != TEXT("0") && talk != TEXT(""))
		{
			data.TalkList.AddItem(talk);
		}

		text = FString::Printf(TEXT("JourneySound%d"), i);
		FString sound = TReadDataValue_FString<JourneyDataManager>(*text, dataRow, rowIdx);
		if (sound != TEXT("0") && sound != TEXT(""))
		{
			data.SoundList.AddItem(sound);

			// used for the release build resource list
			if (obj)
			{
				obj->SoundList.AddItem(sound);
			}
		}
	}

	for (INT i = 0; i < MAX_JOURNEY_MONSTER_COUNT; ++ i)
	{
		MonsterMapData mapData;

		FString text = FString::Printf(TEXT("Monster%d"), i);
		mapData.MonsterId = TReadDataValue_Int<JourneyDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("MonsterName%d"), i);
		mapData.MonsterName = TReadDataValue_FString<JourneyDataManager>(*text, dataRow, rowIdx);
		if (mapData.MonsterName == TEXT("0"))
		{
			mapData.MonsterName = TEXT("");
		}

		text = FString::Printf(TEXT("MonsterType%d"), i);
		mapData.MonsterType = TReadDataValue_Int<JourneyDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("MonsterMap%d"), i);
		mapData.MonsterMapId = TReadDataValue_Int<JourneyDataManager>(*text, dataRow, rowIdx);

		text = FString::Printf(TEXT("MonsterMapRegion%d"), i);
		mapData.MonsterMapRegion = TReadDataValue_FString<JourneyDataManager>(*text, dataRow, rowIdx);
		if (mapData.MonsterMapRegion == TEXT("0"))
		{
			mapData.MonsterMapRegion = TEXT("");
		}

		mapData.MonsterPoint.X = 0.0;
		mapData.MonsterPoint.Y = 0,0;
		text = FString::Printf(TEXT("MonsterPoint%d"), i);
		FString dataStr = TReadDataValue_FString<JourneyDataManager>(*text, dataRow, rowIdx);
		TArray<FString> dataArray;
		dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
		if (dataArray.Num() >= 2)
		{
			mapData.MonsterPoint.X = appAtoi(*(dataArray(0))) * 100;
			mapData.MonsterPoint.Y = appAtoi(*(dataArray(1))) * 100;
		}

		if (mapData.MonsterType == JMT_NORMAL)
		{
			data.MonsterList.AddItem(mapData);
		}
		else
		{
			data.BossList.AddItem(mapData);
		}
	}
	
	if (data.Id != 0)
	{
		MMOJourneyData::getSingleton().AddJourneyData(data);
	}
}

void JourneyDataProcessor::PostInitData()
{
}

#if WITH_EDITOR

void JourneyDataProcessor::CollectReferencedResourceFromDataTable(JourneyData* obj, TSet<FString>& outResList)
{
	if (obj)
	{
		for (INT i = 0; i < obj->SoundList.Num(); ++ i)
		{
			if (obj->SoundList(i) != TEXT(""))
			{
				//UObject* data = UObject::StaticLoadObject(USoundCue::StaticClass(), NULL, *(obj->SoundList(i)), NULL, LOAD_NoWarn | LOAD_Quiet, NULL);
				USoundCue* data = LoadUEObject<USoundCue>(obj->SoundList(i), TEXT("journey.txt"));
				if (data)
				{
					_CollectReferencedResource(data, outResList);
				}
			}
		}
	}
}

#endif

//////////////////////////////////////////////////////////////////////////
MMOJourneyData::MMOJourneyData()
{
}

MMOJourneyData::~MMOJourneyData()
{
}

void MMOJourneyData::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOJourneyData::Initialize"), 0);

	LoadJourneyData(TEXT("journey.txt"));
}

void MMOJourneyData::Shutdown()
{
	UnloadJourneyData();
}

void MMOJourneyData::Clear()
{
	ClearJourneyTypeActivate();
}

void MMOJourneyData::AddJourneyData(const JourneyData& data)
{
	_JourneyDataMap.insert(std::make_pair(data.Id, data));
}

UBOOL MMOJourneyData::GetJourneyData(INT32 journeyId, JourneyData& data)
{
	JourneyDataMap::iterator iter = _JourneyDataMap.find(journeyId);
	if (iter != _JourneyDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}

	return FALSE;
}

UBOOL MMOJourneyData::LoadJourneyData(const TCHAR* filePath)
{
	_JourneyDataMap.clear();
	JourneyDataManager::getSingleton().InitFromFile(filePath, true);
	return TRUE;
}

void MMOJourneyData::UnloadJourneyData()
{
	_JourneyDataMap.clear();
}

void MMOJourneyData::ActivateJourneyType(JourneyType journeyType)
{
	JourneyTypeActivateMap::iterator iter = _JourneyTypeActivateMap.find(journeyType);
	if (iter != _JourneyTypeActivateMap.end())
	{
		iter->second = TRUE;
	}
}

void MMOJourneyData::DeactiveJourneyType(JourneyType journeyType)
{
	JourneyTypeActivateMap::iterator iter = _JourneyTypeActivateMap.find(journeyType);
	if (iter != _JourneyTypeActivateMap.end())
	{
		iter->second = FALSE;
	}
}

UBOOL MMOJourneyData::IsJourneyTypeActivate(JourneyType journeyType)
{
	JourneyTypeActivateMap::iterator iter = _JourneyTypeActivateMap.find(journeyType);
	if (iter != _JourneyTypeActivateMap.end())
	{
		return iter->second;
	}

	return FALSE;
}

void MMOJourneyData::ClearJourneyTypeActivate()
{
	_JourneyTypeActivateMap.clear();
	_JourneyTypeActivateMap.insert(std::make_pair(JT_STORY, TRUE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_MONSTER, FALSE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_ACHIEVEMENT, FALSE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_EMERGENCY, FALSE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_BOSS, FALSE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_ACTIVITY, FALSE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_GUIDE, FALSE));
	_JourneyTypeActivateMap.insert(std::make_pair(JT_GUILD, FALSE));
}

INT32 MMOJourneyData::GetJourneyIdByLevel(INT level)
{
	INT32 journeyId = 0;
	JourneyDataMap::iterator iter = _JourneyDataMap.begin();
	for (; iter != _JourneyDataMap.end(); ++ iter)
	{
		JourneyData data = iter->second;
		if (level >= data.MinLevel && level <= data.MaxLevel)
		{
			journeyId = data.Id;
			break;
		}
	}

	return journeyId;
}

INT32 MMOJourneyData::GetMaxJourneyId()
{
	INT32 journeyId = 0;
	JourneyDataMap::iterator iter = _JourneyDataMap.begin();
	for (; iter != _JourneyDataMap.end(); ++ iter)
	{
		JourneyData data = iter->second;
		if (data.Id > journeyId)
		{
			journeyId = data.Id;
		}
	}

	return journeyId;
}

INT32 MMOJourneyData::GetMaxActivateJourneyId()
{
	INT32 journeyId = 0;
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		JourneyDataMap::iterator iter = _JourneyDataMap.begin();
		for (; iter != _JourneyDataMap.end(); ++ iter)
		{
			JourneyData data = iter->second;

			UActorAttribute* attri = mainPlayer->ActorAttributes;
			if (attri && attri->LevelAttributes.Level >= data.MinLevel)
			{
				journeyId = data.Id;
			}
		}
	}

	return journeyId;
}

INT32 MMOJourneyData::GetFirstRewardJourneyId()
{
	INT32 journeyId = 0;
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return journeyId;
	}

	JourneyDataMap::iterator iter = _JourneyDataMap.begin();
	for (; iter != _JourneyDataMap.end(); ++ iter)
	{
		JourneyData data = iter->second;

		UActorAttribute* attri = mainPlayer->ActorAttributes;
		if (attri && attri->LevelAttributes.Level >= data.MinLevel)
		{
			ReputationTemp* reputationTemp = MMOFacade::getSingleton().GetReputationTempByForceID(data.ForceId);
			if (reputationTemp)
			{
				int curReputation = 0;
				int minReputation = 0;
				int maxReputation = 1;
				if (reputationTemp->ListLevel > 0)
				{
					curReputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), reputationTemp->ForceID);
					minReputation = reputationTemp->InitValue;
					maxReputation = reputationTemp->MaxValue;
				}

				if (curReputation >= maxReputation)
				{
					if (!MMOFacade::getSingleton().GetPlayerAwardState(mainPlayer, data.ForceId))
					{
						journeyId = data.Id;
					}
				}
			}
		}
	}

	return journeyId;
}

UBOOL MMOJourneyData::GetJourneyDataByReputation(INT32 forceId, JourneyData& data)
{
	JourneyDataMap::iterator iter = _JourneyDataMap.begin();
	for (; iter != _JourneyDataMap.end(); ++ iter)
	{
		if (forceId == iter->second.ForceId)
		{
			data = iter->second;
			return TRUE;
		}
	}

	return FALSE;
}

INT32 MMOJourneyData::GetReputationAwardsCount(INT32 forceId)
{
	INT32 count = 0;
	ReputationTemp* reputationTemp = MMOFacade::getSingleton().GetReputationTempByForceID(forceId);
	if (reputationTemp)
	{
		if (MMOFacade::getSingleton().GetMainActor())
		{
			switch(MMOFacade::getSingleton().GetMainActor()->GetClassId())
			{
			case EPCT_PlayerClassG: // 1.05 Lancer: reuses Warrior reputation rewards (no Spear columns)
			case EPCT_PlayerClassA://Warrior
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						if (reputationTemp->_WarriorAwardItemID[i] > 0 && reputationTemp->_WarriorAwardItemCount[i] > 0)
						{
							count ++;
						}
					}
				}
				break;
			case EPCT_PlayerClassH: // 1.05 Sorcerer: reuses Mage reputation rewards
			case EPCT_PlayerClassB://Mage
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						if (reputationTemp->_MageAwardItemID[i] > 0 && reputationTemp->_MageAwardItemCount[i] > 0)
						{
							count ++;
						}
					}
				}
				break;
			case EPCT_PlayerClassC://Assassin
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						if (reputationTemp->_RogueAwardItemID[i] > 0 && reputationTemp->_RogueAwardItemCount[i] > 0)
						{
							count ++;
						}
					}
				}
				break;
			case EPCT_PlayerClassD://Archer
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						if (reputationTemp->_ArcherAwardItemID[i] > 0 && reputationTemp->_ArcherAwardItemCount[i] > 0)
						{
							count ++;
						}
					}
				}
				break;
			case EPCT_PlayerClassE://Taoist
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						if (reputationTemp->_TaoistAwardItemID[i] > 0 && reputationTemp->_TaoistAwardItemCount[i] > 0)
						{
							count ++;
						}
					}
				}
				break;
			}
		}
	}

	return count;
}
