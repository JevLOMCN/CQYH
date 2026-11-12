#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"
#include "MMOJourneyManager.h"
#include "MMOGuideData.h"
#include "MMOAchievementData.h"
#include "MMOAchievementManager.h"
#include "MMOGuildData.h"
#include "MMOAssistManager.h"

IMPLEMENT_CLASS(UMMOUIJourneyTracePanel)
extern UMMOGFXPlayer_RootBase* g_rootGFXPlayer;
void UMMOUIJourneyTracePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Journey);
	evts.AddItem(GameEvent_Quest);
	evts.AddItem(GameEvent_EnterScene);
	evts.AddItem(GameEvent_QuestModeChange);
	EventAdapter = new TGameEventAdapter<UMMOUIJourneyTracePanel>(this, evts);
}

void UMMOUIJourneyTracePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		UBOOL enable = MMOAssistManager::getSingleton().IsQuestMode();
		UBOOL expand = MMOJourneyManager::getSingleton().IsJourneyTraceExpand();
		eventCallSetTracePanelEnable(enable && expand);
	}
}

void UMMOUIJourneyTracePanel::OnFocusGained()
{
	UMMOUIBase::OnFocusGained();	
	TArray<struct FASValue> args;
	Invoke(TEXT("UCSetFocus"), args);	
}

void UMMOUIJourneyTracePanel::OnFocusLost()
{
	UMMOUIBase::OnFocusLost();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if (ctrl)
	{
		ctrl->FocusOnInput(this);
	}

}

void UMMOUIJourneyTracePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_Journey:
		{
			MMOGameEvent_Journey* journeyEvent = (MMOGameEvent_Journey*)evt;
			if (journeyEvent)
			{
				switch (journeyEvent->EventType)
				{
				case MMOGameEvent_Journey::EJET_LevelChange:
					{
						INT lastLevel = journeyEvent->_Value1;
						INT curLevel = journeyEvent->_Value2;
						UpdateJourneyQuestLevel(lastLevel, curLevel);

						_CurJourneyId = MMOJourneyData::getSingleton().GetJourneyIdByLevel(curLevel);
						UpdateJourneyTraceList();
						UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);

						INT32 lastJourneyId = MMOJourneyData::getSingleton().GetJourneyIdByLevel(lastLevel);
						if (lastJourneyId > 0 && _CurJourneyId > lastJourneyId)
						{
							eventCallShowChangeLevelHint(TRUE, TEXT("Page back to see unfinished quests and achievements from the previous stage"));
						}
						else
						{
							eventCallShowChangeLevelHint(FALSE, TEXT(""));
						}
					}
					break;

				case MMOGameEvent_Journey::EJET_KillMonster:
					{
						/*
						if (!MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_MONSTER))
						{
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_MONSTER);
							UpdateJourneyTraceInfo(_CurJourneyId, INVALID_QUEST_ID);
						}
						*/

						// #17549
						if (!MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_MONSTER) ||
							!MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_BOSS))
						{
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_MONSTER);
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_BOSS);

							UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);
						}
					}
					break;

				case MMOGameEvent_Journey::EJET_KillBoss:
					{
						/*
						if (!MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_BOSS))
						{
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_BOSS);
							UpdateJourneyTraceInfo(_CurJourneyId, INVALID_QUEST_ID);
						}
						*/

						// #17549
						if (!MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_MONSTER) ||
							!MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_BOSS))
						{
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_MONSTER);
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_BOSS);
							UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);
						}
					}
					break;

				case MMOGameEvent_Journey::EJET_AddAchievement:
					{
						UINT32 id = journeyEvent->_Value1;
						if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
						{
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_GUILD);
							UpdateJourneyTraceInfo(_CurJourneyId, JT_GUILD);
						}
						else
						{
							MMOJourneyManager::getSingleton().ActivateJourneyType(JT_ACHIEVEMENT);
							UpdateJourneyTraceInfo(_CurJourneyId, JT_ACHIEVEMENT);
						}
					}
					break;

				case MMOGameEvent_Journey::EJET_RemoveAchievement:
				case MMOGameEvent_Journey::EJET_UpdateAchievement:
				case MMOGameEvent_Journey::EJET_UpdateAchievementRewards:
					{
						UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);
					}
					break;

				case MMOGameEvent_Journey::EJET_ReputationChange:
					{
						JourneyData data;
						if (MMOJourneyData::getSingleton().GetJourneyData(_CurJourneyId, data))
						{
							if (journeyEvent->_Value1 == data.ForceId)
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

									FString tips = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.JourneyTrace.ReputationDesc")), (curReputation - minReputation), (maxReputation - minReputation));
									UBOOL hasGetReward = MMOFacade::getSingleton().GetPlayerAwardState(MMOFacade::getSingleton().GetMainPlayer(), reputationTemp->ForceID);
									eventCallUpdateJourneyReputation(curReputation, maxReputation, minReputation, *tips, curReputation >= maxReputation, hasGetReward);
								}
							}
						}
					}
					break;

				case MMOGameEvent_Journey::EJET_GetAwards:
					{
						JourneyData data;
						if (MMOJourneyData::getSingleton().GetJourneyData(_CurJourneyId, data))
						{
							if (journeyEvent->_Value1 == data.ForceId)
							{
								UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);
							}
						}
					}
					break;
				}
			}
		}
		break;

	case GameEvent_Quest:
		{
			MMOGameEvent_Quest* questEvent = (MMOGameEvent_Quest*)evt;
			if (questEvent)
			{
				switch (questEvent->EventType)
				{
				case MMOGameEvent_Quest::EQET_AcceptQuest:
				case MMOGameEvent_Quest::EQET_AbandonQuest:
				case MMOGameEvent_Quest::EQET_CompleteQuest:
				case MMOGameEvent_Quest::EQET_UpdateQuestTraceList:
					{
						UpdateJourneyQuestState(questEvent->EventType, questEvent->QuestId);
						UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);
					}
					break;
					
				case MMOGameEvent_Quest::EQET_UpdateQuest:
				case MMOGameEvent_Quest::EQET_UpdateQuestTime:
					{
						UpdateJourneyQuestData(questEvent->QuestId);
					}
					break;
				}
			}
		}
		break;

	case GameEvent_EnterScene:
		{
			UpdateJourneyTraceList();
			UpdateJourneyTraceInfo(_CurJourneyId, _CurExpandType);
		}
		break;

	case GameEvent_QuestModeChange:
		{
			UBOOL enable = MMOAssistManager::getSingleton().IsQuestMode();
			eventCallSetTracePanelEnable(enable);
		}
		break;
	}
}

void UMMOUIJourneyTracePanel::UpdateJourneyTraceList()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	JourneyDataMap journeyDataMap = MMOJourneyData::getSingleton().GetJourneyDataMap();
	JourneyDataMap::iterator iter = journeyDataMap.begin();
	int index = 0;
	for (; iter != journeyDataMap.end(); ++ iter)
	{
		JourneyData data = iter->second;

		UActorAttribute* attri = mainPlayer->ActorAttributes;
		if (attri && attri->LevelAttributes.Level >= data.MinLevel)
		{
			objectArray->SetElementString(index, data.Title);
			index ++;
		}
	}

	eventCallUpdateJourneyTraceList(objectArray, _CurJourneyId);
}

UGFxObject* UMMOUIJourneyTracePanel::_CreateJourneyElement(INT journeyType, INT journeyStar, const FString& hintText, UBOOL isComplete, UBOOL isExpand)
{
	UGFxObject* journeyElement = GFXPlayer->CreateObject(TEXT("UI_Journey.AS3.JourneyTraceItemData"));
	if (journeyElement)
	{
		journeyElement->SetInt(TEXT("itemType"), journeyType);
		journeyElement->SetBool(TEXT("isComplete"), isComplete);
		journeyElement->SetInt(TEXT("category"), journeyType);
		journeyElement->SetBool(TEXT("isExpand"), isExpand);
		journeyElement->SetInt(TEXT("recommandLevel"), journeyStar);
		journeyElement->SetString(TEXT("hintText"), hintText);

		UGFxObject* questArray = GFXPlayer->CreateArray();
		if (questArray)
		{
			journeyElement->SetObject(TEXT("questTraceData"), questArray);
		}
	}

	return journeyElement;
}

void UMMOUIJourneyTracePanel::_SetJourneyElementExpand(UGFxObject* journeyElement, UBOOL isExpand)
{
	if (journeyElement)
	{
		journeyElement->SetBool(TEXT("isExpand"), isExpand);
	}
}

void UMMOUIJourneyTracePanel::_AddAcceptedQuestObject(UGFxObject* journeyElement, INT questId, INT& index)
{
	if (journeyElement)
	{
		UGFxObject* questArray = journeyElement->GetObject(TEXT("questTraceData"));
		if (questArray)
		{
			UGFxObject* questObject = MMOQuestManager::getSingleton().GetQuestTraceData(questId, GFXPlayer, EQTLT_Accepted);
			if (questObject)
			{
				questArray->SetElementObject(index, questObject);
				index ++;
			}
		}
	}
}

void UMMOUIJourneyTracePanel::_AddAvailableQuestObject(UGFxObject* journeyElement, INT questId, INT& index)
{
	if (journeyElement)
	{
		UGFxObject* questArray = journeyElement->GetObject(TEXT("questTraceData"));
		if (questArray)
		{
			UGFxObject* questObject = MMOQuestManager::getSingleton().GetQuestTraceData(questId, GFXPlayer, EQTLT_Available);
			if (questObject)
			{
				questArray->SetElementObject(index, questObject);
				index ++;
			}
		}
	}
}

void UMMOUIJourneyTracePanel::_AddMonsterObject(UGFxObject* journeyElement, INT monsterId, const FString& monsterName, INT monsterType, INT mapId, const FString& mapRegion, FVector2D point)
{
	if (journeyElement)
	{
		FString hintText = journeyElement->GetString(TEXT("hintText"));
		hintText += TEXT("<br>");
		UNPCTemplate* npc = MMOFacade::getSingleton().FindNPCTemplate(monsterId);
		if (npc)
		{
			hintText += FString::Printf(TEXT("<font color='#ffff00'>%s</font>"), 
				*LinkManager::HtmlLink_OpenMap(monsterName == TEXT("") ? npc->GetDisplayName() : monsterName, monsterId, mapId, mapRegion, point));

			FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(mapId);
			if (mapInfo)
			{
				hintText += TEXT("(");
				hintText += mapInfo->MapDisplayName;
				hintText += TEXT(")");
			}

		}
		journeyElement->SetString(TEXT("hintText"), hintText);
	}
}

void UMMOUIJourneyTracePanel::_AddBossObject(UGFxObject* journeyElement, INT bossId, const FString& bossName, INT bossType, INT mapId, const FString& mapRegion, FVector2D point)
{
	if (journeyElement)
	{
		FString hintText = journeyElement->GetString(TEXT("hintText"));
		hintText += TEXT("\n");
		UNPCTemplate* npc = MMOFacade::getSingleton().FindNPCTemplate(bossId);
		if (npc)
		{
			hintText += FString::Printf(bossType == JMT_BOSS ? TEXT("<font color='#ff4800'>%s</font>") : TEXT("<font color='#ffa200'>%s</font>"), 
				*LinkManager::HtmlLink_OpenMap(bossName == TEXT("") ? npc->GetDisplayName() : bossName, bossId, mapId, mapRegion, point));

			FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(mapId);
			if (mapInfo)
			{
				hintText += TEXT("(");
				hintText += mapInfo->MapDisplayName;
				hintText += TEXT(")");
			}
		}
		journeyElement->SetString(TEXT("hintText"), hintText);
	}
}

void UMMOUIJourneyTracePanel::_AddAchievementObject(UGFxObject* journeyElement, INT id, INT& index)
{
	if (journeyElement)
	{
		UGFxObject* questArray = journeyElement->GetObject(TEXT("questTraceData"));
		if (questArray)
		{
			UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
			if (object)
			{
				if (id < ACHIEVEMENT_MAX)
				{
					AchievementData data;
					if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
					{
						AchievementInfo info;
						if (MMOAchievementData::getSingleton().GetAchievementInfo(id, info))
						{
							object->SetInt(TEXT("achievement_id"), id);
							object->SetString(TEXT("achievement_trace_name"), *LinkManager::HtmlLink_Achievement(id));
							object->SetInt(TEXT("achievement_detail"), data.DetailType);
							if (data.DetailType == ADT_CONDITION)
							{
								for (INT i = 0; i < MAX_CONDITION_TYPE_COUNT; ++ i)
								{
									if (data.ConditionType[i] != adInvalid)
									{
										FString variable = FString(TEXT("achievement_condition_name")) + appItoa(i);
										object->SetString(variable, data.ConditionDesc[i]);
										variable = FString(TEXT("achievement_condition_complete")) + appItoa(i);
										object->SetBool(variable, (info.CurComplete & (1 << i)) > 0);
									}
									else
									{
										FString variable = FString(TEXT("achievement_condition_name")) + appItoa(i);
										object->SetString(variable, TEXT(""));
										variable = FString(TEXT("achievement_condition_complete")) + appItoa(i);
										object->SetBool(variable, FALSE);
									}
								}
							}
							else if (data.DetailType == ADT_PROGRESS)
							{
								UINT32 cur = abs(info.CurProgress);
								UINT32 max = abs(info.MaxProgress);
								cur = cur >= max ? max : cur;
								object->SetInt(TEXT("achievement_progress_cur"), cur);
								object->SetInt(TEXT("achievement_progress_max"), max == 0 ? 1 : max);
							}
						}
					}
				}
				else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
				{
					UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
					GuildAchievementData data;
					if (MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
					{
						AchievementInfo info;
						if (MMOAchievementData::getSingleton().GetAchievementInfo(id, info))
						{
							object->SetInt(TEXT("achievement_id"), id);
							object->SetString(TEXT("achievement_trace_name"), *LinkManager::HtmlLink_Achievement(id));
							object->SetInt(TEXT("achievement_detail"), ADT_NULL);
							UINT32 cur = abs(info.CurProgress);
							UINT32 max = abs(info.MaxProgress);
							cur = cur >= max ? max : cur;
							object->SetInt(TEXT("achievement_progress_cur"), cur);
							object->SetInt(TEXT("achievement_progress_max"), max == 0 ? 1 : max);
						}
					}
				}
				else
				{
					UINT16 killId = HIWORD(id);
					UINT16 killIndex = LOWORD(id);
					KillNpcAchievementData data;
					if (MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
					{
						AchievementInfo info;
						if (MMOAchievementData::getSingleton().GetAchievementInfo(id, info))
						{
							object->SetInt(TEXT("achievement_id"), id);
							object->SetString(TEXT("achievement_trace_name"), *LinkManager::HtmlLink_Achievement(id));
							object->SetInt(TEXT("achievement_detail"), ADT_PROGRESS);
							UINT32 cur = abs(info.CurProgress);
							UINT32 max = abs(info.MaxProgress);
							cur = cur >= max ? max : cur;
							object->SetInt(TEXT("achievement_progress_cur"), cur);
							object->SetInt(TEXT("achievement_progress_max"), max == 0 ? 1 : max);
						}
					}
				}

				questArray->SetElementObject(index, object);
				index ++;
			}
		}
	}
}

void UMMOUIJourneyTracePanel::_AddAwardString(INT itemId, INT itemCount, FString& awardsString)
{
	if (itemCount == 0)
	{
		return;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemId);
	if (!item)
	{
		return;
	}
		
	awardsString += item->GetDisplayName();
	awardsString += TEXT(",");
}

void UMMOUIJourneyTracePanel::PlayJourneySound(INT index)
{
	JourneyData data;
	if (MMOJourneyData::getSingleton().GetJourneyData(_CurJourneyId, data))
	{
		if (index >= 0 && index < data.SoundList.Num())
		{
			if (data.SoundList(index) != TEXT(""))
			{
				USoundCue* sound = LoadUEObject<USoundCue>(data.SoundList(index), TEXT("journey.txt"));
				if (sound)
				{
					MMOFacade::getSingleton().GetMainActor()->PlaySound(sound);
				}
			}
		}
	}
}

void UMMOUIJourneyTracePanel::UpdateJourneyQuestLevel(INT oldLevel, INT newLevel)
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	if (newLevel >= GUILD_CREATER_LEVEL)
	{
		MMOJourneyManager::getSingleton().ActivateJourneyType(JT_GUILD);
	}

	// Activate acceptable quest node
	const QuestTraceList& availableQuestList = MMOQuestData::getSingleton().GetAvailableQuestTraceList();
	QuestTraceList::const_iterator iter = availableQuestList.begin();
	for (; iter != availableQuestList.end(); ++iter)
	{
		INT questId = *iter;
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
		if (!quest)
		{
			continue;
		}

		if (quest->QuestType == E_qt_Trunk)
		{
			MMOJourneyManager::getSingleton().ActivateJourneyType(JT_STORY);
		}
		else if (quest->QuestType == E_qt_Activity ||
			quest->QuestType == E_qt_Circle ||
			quest->QuestType == E_qt_Daily ||
			quest->QuestType == E_qt_Reward ||
			quest->QuestType == E_qt_Master ||
			quest->QuestType == E_qt_Wedding ||
			quest->QuestType == E_qt_Adventure)
		{
			MMOJourneyManager::getSingleton().ActivateJourneyType(JT_ACTIVITY);
		}
		else if (quest->QuestType == E_qt_Guide)
		{
			MMOJourneyManager::getSingleton().ActivateJourneyType(JT_GUIDE);
		}
	}
}

void UMMOUIJourneyTracePanel::UpdateJourneyQuestState(INT questEvent, INT questId)
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	if (questEvent == MMOGameEvent_Quest::EQET_AcceptQuest)
	{
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
		if (quest)
		{
			if (quest->QuestType == E_qt_Trunk)
			{
				MMOJourneyManager::getSingleton().ActivateJourneyType(JT_STORY);
				_CurExpandType = JT_STORY;
			}
			else if (quest->QuestType == E_qt_Emegency)
			{
				MMOJourneyManager::getSingleton().ActivateJourneyType(JT_EMERGENCY);
				_CurExpandType = JT_EMERGENCY;
			}
			else if (quest->QuestType == E_qt_Activity ||
				quest->QuestType == E_qt_Circle ||
				quest->QuestType == E_qt_Daily ||
				quest->QuestType == E_qt_Reward ||
				quest->QuestType == E_qt_Master ||
				quest->QuestType == E_qt_Wedding ||
				quest->QuestType == E_qt_Adventure)
			{
				MMOJourneyManager::getSingleton().ActivateJourneyType(JT_ACTIVITY);
				_CurExpandType = JT_ACTIVITY;
			}
			else if (quest->QuestType == E_qt_Guide)
			{
				MMOJourneyManager::getSingleton().ActivateJourneyType(JT_GUIDE);
				_CurExpandType = JT_GUIDE;
			}
			else if (quest->QuestType == E_qt_Guild)
			{
				MMOJourneyManager::getSingleton().ActivateJourneyType(JT_GUILD);
				_CurExpandType = JT_GUILD;
			}
		}
	}
	else if (questEvent == MMOGameEvent_Quest::EQET_AbandonQuest ||
		questEvent == MMOGameEvent_Quest::EQET_CompleteQuest)
	{
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
		if (quest)
		{
			if (quest->QuestType == E_qt_Emegency)
			{
				// Quest #17291
				bool hasEmegencyQuest = false;
				const QuestTraceList& acceptedQuestList = MMOQuestData::getSingleton().GetAcceptedQuestTraceList();
				QuestTraceList::const_iterator iter = acceptedQuestList.begin();
				for (; iter != acceptedQuestList.end(); ++iter)
				{
					if (MMOQuestManager::getSingleton().IsEmergencyQuestBeTraced((INT)(*iter)))
					{
						hasEmegencyQuest = true;
						break;
					}
				}

				if (hasEmegencyQuest)
				{
					MMOJourneyManager::getSingleton().ActivateJourneyType(JT_EMERGENCY);
				}
				else
				{
					MMOJourneyManager::getSingleton().DeactiveJourneyType(JT_EMERGENCY);

					// #20745
					if (_CurExpandType == JT_EMERGENCY)
					{
						_CurExpandType = JT_STORY;
					}
				}
			}
			else if (quest->QuestType == E_qt_Guide)
			{
				// Quest #25265
				bool hasGuideQuest = false;
				const QuestTraceList& availableQuestList = MMOQuestData::getSingleton().GetAvailableQuestTraceList();
				QuestTraceList::const_iterator iter = availableQuestList.begin();
				for (; iter != availableQuestList.end(); ++iter)
				{
					const UQuest* quest = MMOQuestData::getSingleton().GetQuestData((INT)(*iter));
					if (quest)
					{
						if (quest->QuestType == E_qt_Guide)
						{
							hasGuideQuest = true;
							break;
						}
					}
				}

				if (hasGuideQuest)
				{
					MMOJourneyManager::getSingleton().ActivateJourneyType(JT_GUIDE);
				}
				else
				{
					MMOJourneyManager::getSingleton().DeactiveJourneyType(JT_GUIDE);

					// #20745
					if (_CurExpandType == JT_GUIDE)
					{
						_CurExpandType = JT_STORY;
					}
				}
			}
		}
	}
	else if (MMOGameEvent_Quest::EQET_UpdateQuestTraceList)
	{
		if (questId == INVALID_QUEST_ID)
		{
			// Quest #17291
			bool hasEmegencyQuest = false;
			const QuestTraceList& acceptedQuestList = MMOQuestData::getSingleton().GetAcceptedQuestTraceList();
			QuestTraceList::const_iterator iter = acceptedQuestList.begin();
			for (; iter != acceptedQuestList.end(); ++iter)
			{
				if (MMOQuestManager::getSingleton().IsEmergencyQuestBeTraced((INT)(*iter)))
				{
					hasEmegencyQuest = true;
					break;
				}
			}

			if (hasEmegencyQuest)
			{
				MMOJourneyManager::getSingleton().ActivateJourneyType(JT_EMERGENCY);
			}
			else
			{
				MMOJourneyManager::getSingleton().DeactiveJourneyType(JT_EMERGENCY);

				// #20745
				if (_CurExpandType == JT_EMERGENCY)
				{
					_CurExpandType = JT_STORY;
				}
			}
		}

		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
		if (quest)
		{
			if (quest->QuestType == E_qt_Trunk)
			{
				_CurExpandType = JT_STORY;
			}
			else if (quest->QuestType == E_qt_Emegency)
			{
				if (MMOQuestManager::getSingleton().IsEmergencyQuestBeTraced(questId))
				{
					MMOJourneyManager::getSingleton().ActivateJourneyType(JT_EMERGENCY);
					_CurExpandType = JT_EMERGENCY;
				} 
				else
				{
					_CurExpandType = JT_STORY;
				}
			}
			else if (quest->QuestType == E_qt_Activity ||
				quest->QuestType == E_qt_Circle ||
				quest->QuestType == E_qt_Daily ||
				quest->QuestType == E_qt_Reward ||
				quest->QuestType == E_qt_Master ||
				quest->QuestType == E_qt_Wedding ||
				quest->QuestType == E_qt_Adventure)
			{
				_CurExpandType = JT_ACTIVITY;
			}
			else if (quest->QuestType == E_qt_Guide)
			{
				_CurExpandType = JT_GUIDE;
			}
			else if (quest->QuestType == E_qt_Guild)
			{
				_CurExpandType = JT_GUILD;
			}
		}
	}
}

void UMMOUIJourneyTracePanel::UpdateJourneyTraceInfo(INT journeyId, INT expandType)
{
	JourneyData data;
	if (!MMOJourneyData::getSingleton().GetJourneyData(journeyId, data))
	{
		return;
	}

	UGFxObject* journeyObject = GFXPlayer->CreateObject(TEXT("UI_Journey.AS3.JourneyTraceData"));
	if (!journeyObject)
	{
		return;
	}

	journeyObject->SetString(TEXT("journeyTraceIcon"), data.Icon);

	UGFxObject* journeyTalkArray = GFXPlayer->CreateArray();
	if (journeyTalkArray)
	{
		for (INT i = 0; i < data.TalkList.Num(); ++ i)
		{
			journeyTalkArray->SetElementString(i, data.TalkList(i));
		}

		journeyObject->SetObject(TEXT("journeyTraceTips"), journeyTalkArray);
	}

	journeyObject->SetObject(TEXT("chapterProgressData"), NULL);
	journeyObject->SetBool(TEXT("chapterComplete"), FALSE);
	journeyObject->SetBool(TEXT("hasGetReward"), FALSE);
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
		
		UGFxObject* progressObject = GFXPlayer->CreateObject(TEXT("Object"));
		if (progressObject)
		{
			progressObject->SetInt(TEXT("curReputation"), curReputation);
			progressObject->SetInt(TEXT("minReputation"), minReputation);
			progressObject->SetInt(TEXT("maxReputation"), maxReputation);

			FString tips = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.JourneyTrace.ReputationDesc")), (curReputation - minReputation), (maxReputation - minReputation));
			progressObject->SetString(TEXT("tipsStr"), *tips);

			journeyObject->SetObject(TEXT("chapterProgressData"), progressObject);
		}
		journeyObject->SetBool(TEXT("chapterComplete"), curReputation >= maxReputation);

		FString awardsStr = TEXT("");
		if (MMOFacade::getSingleton().GetMainActor())
		{
			switch(MMOFacade::getSingleton().GetMainActor()->GetClassId())
			{
			case EPCT_PlayerClassG: // 1.05 Lancer: reuses Warrior reputation rewards (no Spear columns)
			case EPCT_PlayerClassA://Warrior
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						_AddAwardString(reputationTemp->_WarriorAwardItemID[i], reputationTemp->_WarriorAwardItemCount[i], awardsStr);
					}
				}
				break;
			case EPCT_PlayerClassH: // 1.05 Sorcerer: reuses Mage reputation rewards
			case EPCT_PlayerClassB://Mage
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						_AddAwardString(reputationTemp->_MageAwardItemID[i], reputationTemp->_MageAwardItemCount[i], awardsStr);
					}
				}
				break;
			case EPCT_PlayerClassC://Assassin
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						_AddAwardString(reputationTemp->_RogueAwardItemID[i], reputationTemp->_RogueAwardItemCount[i], awardsStr);
					}
				}
				break;
			case EPCT_PlayerClassD://Archer
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						_AddAwardString(reputationTemp->_ArcherAwardItemID[i], reputationTemp->_ArcherAwardItemCount[i], awardsStr);
					}
				}
				break;
			case EPCT_PlayerClassE://Taoist
				{
					for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
					{
						_AddAwardString(reputationTemp->_TaoistAwardItemID[i], reputationTemp->_TaoistAwardItemCount[i], awardsStr);
					}
				}
				break;
			}
		}

		if (awardsStr.EndsWith(TEXT(",")))
		{
			awardsStr = awardsStr.Mid(0, awardsStr.Len() - 1);
			awardsStr += TEXT(". ");
		}

		if (MMOFacade::getSingleton().GetPlayerAwardState(MMOFacade::getSingleton().GetMainPlayer(), reputationTemp->ForceID))
		{
			journeyObject->SetString(TEXT("journeyAwardsStr"), TEXT("Reward already claimed"));
			journeyObject->SetBool(TEXT("hasGetReward"), TRUE);
		}
		else
		{
			journeyObject->SetString(TEXT("journeyAwardsStr"), awardsStr);
			journeyObject->SetBool(TEXT("hasGetReward"), FALSE);
		}
	}

	// Achievement reward
	AchievementIdSet achievementSet = MMOAchievementData::getSingleton().GetAchievementReward();
	if (achievementSet.size() > 0)
	{
		UINT32 id = *(achievementSet.begin());
		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
			{
				UGFxObject* achievementRewardsObject = GFXPlayer->CreateObject(TEXT("Object"));
				if (achievementRewardsObject)
				{
					achievementRewardsObject->SetString(TEXT("achievement_name"), data.Name);

					FString rewardText = TEXT("Reward: ");
					if (data.RewardType[0] == adtItem)
					{
						UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.RewardValue[0]);
						if (item)
						{
							rewardText += item->GetDisplayName();
						}
					}
					else if (data.RewardType[0] == adtTitle)
					{
						TitleData* title = MMOFacade::getSingleton().GetTitleData(data.RewardValue[0]);
						if (title)
						{
							rewardText += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Title"))), *(title->TitleName));
						}
					}
					else if (data.RewardType[0] == adtExp)
					{
						rewardText += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Exp"))), data.RewardValue[0]);
					}

					achievementRewardsObject->SetString(TEXT("achievement_reward_desc"), rewardText);
					achievementRewardsObject->SetInt(TEXT("achievement_id"), id);
					journeyObject->SetObject(TEXT("achievementRewardData"), achievementRewardsObject);
				}
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
			GuildAchievementData data;
			if (baseInfo.Id > 0 &&
				MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
			{
				UGFxObject* achievementRewardsObject = GFXPlayer->CreateObject(TEXT("Object"));
				if (achievementRewardsObject)
				{
					achievementRewardsObject->SetString(TEXT("achievement_name"), data.Name);

					// Due to UI layout, only show one reward
					FString rewardText = TEXT("Reward: ");
					if (data.RewardType[0] == GA_AWARD_ITEM)
					{
						UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.RewardTemp[0]);
						if (item)
						{
							rewardText += item->GetDisplayName();
						}
					}
					else if (data.RewardType[0] == GA_AWARD_TITLE)
					{
						TitleData* title = MMOFacade::getSingleton().GetTitleData(data.RewardTemp[0]);
						if (title)
						{
							rewardText += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Title"))), *(title->TitleName));
						}
					}
					else if (data.RewardType[0] == GA_AWARD_EXP)
					{
						rewardText += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Exp"))), data.RewardCount[0]);
					}
					else if (data.RewardType[0] == GA_AWARD_POINT)
					{
						rewardText += FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Point"))), data.RewardCount[0]);
					}

					achievementRewardsObject->SetString(TEXT("achievement_reward_desc"), rewardText);
					achievementRewardsObject->SetInt(TEXT("achievement_id"), id);
					journeyObject->SetObject(TEXT("achievementRewardData"), achievementRewardsObject);
				}
			}
		}
		else
		{
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
			{
				UGFxObject* achievementRewardsObject = GFXPlayer->CreateObject(TEXT("Object"));
				if (achievementRewardsObject)
				{
					achievementRewardsObject->SetString(TEXT("achievement_name"), data.Name[killIndex]);

					if (data.RewardExp[killIndex] > 0)
					{
						FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Exp"))), data.RewardExp[killIndex]);
						achievementRewardsObject->SetString(TEXT("achievement_reward_desc"), text);
					}

					achievementRewardsObject->SetInt(TEXT("achievement_id"), id);
					journeyObject->SetObject(TEXT("achievementRewardData"), achievementRewardsObject);
				}
			}
		}
	}

	UGFxObject* journeyArray = GFXPlayer->CreateArray();
	if (journeyArray)
	{
		UGFxObject* storyElement = NULL;
		UGFxObject* monsterElement = NULL;
		UGFxObject* achievementElement = NULL;
		UGFxObject* emergencyElement = NULL;
		UGFxObject* bossElement = NULL;
		UGFxObject* activityElement = NULL;
		UGFxObject* guideElement = NULL;
		UGFxObject* guildElement = NULL;

		int elementIndex = 0;
		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_STORY) && data.StoryQuestStar > 0)
		{
			FString hintText = FString::Printf(TEXT("Complete <font color='#5dceee'>%s</font> to gain more experience"), 
				*LinkManager::HtmlLink_OpenPanel(TEXT("Story Quest"), TEXT("QuestStory")));
			storyElement = _CreateJourneyElement(JT_STORY, data.StoryQuestStar, hintText, FALSE, TRUE);
			journeyArray->SetElementObject(elementIndex, storyElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_MONSTER) && data.MonsterStar > 0)
		{
			FString hintText = TEXT("Kill monsters for lots of experience and items");
			monsterElement = _CreateJourneyElement(JT_MONSTER, data.MonsterStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, monsterElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_ACHIEVEMENT) && data.AchievementStar > 0)
		{
			FString hintText = FString::Printf(TEXT("Complete <font color='#5dceee'>%s</font> for bonus experience"),
				*LinkManager::HtmlLink_AchievementClass(TEXT("Achievement"), data.AchievementBaseClass, data.AchievementSubClass));
			achievementElement = _CreateJourneyElement(JT_ACHIEVEMENT, data.AchievementStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, achievementElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_EMERGENCY) && data.EmegencyQuestStar > 0)
		{
			FString hintText = FString::Printf(TEXT("Enter the area <font color='#5dceee'>%s</font> for bonus experience"),
				*LinkManager::HtmlLink_OpenPanel(TEXT("Event"), TEXT("Quest")));
			emergencyElement = _CreateJourneyElement(JT_EMERGENCY, data.EmegencyQuestStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, emergencyElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_BOSS) && data.BossStar > 0)
		{
			FString hintText = TEXT("Bosses may drop rare equipment");
			bossElement = _CreateJourneyElement(JT_BOSS, data.BossStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, bossElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_ACTIVITY) && data.ActivityQuestStar > 0)
		{
			FString hintText = FString::Printf(TEXT("Complete <font color='#5dceee'>%s</font> for more rewards"), 
				*LinkManager::HtmlLink_OpenPanel(TEXT("Event Quest"), TEXT("Quest")));
			activityElement = _CreateJourneyElement(JT_ACTIVITY, data.ActivityQuestStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, activityElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_GUIDE) && data.GuideQuestStar > 0)
		{
			FString hintText = FString::Printf(TEXT("Do <font color='#5dceee'>%s</font> to learn how to use new features"), 
				*LinkManager::HtmlLink_OpenPanel(TEXT("Tutorial Quest"), TEXT("Quest")));
			guideElement = _CreateJourneyElement(JT_GUIDE, data.GuideQuestStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, guideElement);
			elementIndex ++;
		}

		if (MMOJourneyData::getSingleton().IsJourneyTypeActivate(JT_GUILD) && data.GuildStar > 0)
		{
			FString hintText = TEXT("");
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			if (baseInfo.Id > 0)
			{
				hintText = TEXT("Join guild activities to grow stronger");
			} 
			else
			{
				hintText = FString::Printf(TEXT("Join <font color='#5dceee'>%s</font> and fight for supremacy"),
					*LinkManager::HtmlLink_OpenPanel(TEXT("Guild"), TEXT("GuildMain")));
			}
			guildElement = _CreateJourneyElement(JT_GUILD, data.GuildStar, hintText, FALSE, FALSE);
			journeyArray->SetElementObject(elementIndex, guildElement);
			elementIndex ++;
		}

		// accepted quests
		int storyIndex = 0;
		int emergencyIndex = 0;
		int activityIndex = 0;
		int guideIndex = 0;
		int guildIndex = 0;
		int achievementIndex = 0;
		bool hasAcceptedStoryQuest = false;
		bool hasAvailableStoryQuest = false;

		const QuestTraceList& acceptedQuestList = MMOQuestData::getSingleton().GetAcceptedQuestTraceList();
		QuestTraceList::const_iterator iter = acceptedQuestList.begin();
		for(; iter != acceptedQuestList.end(); ++ iter)
		{
			INT questId = *iter;
			const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
			if (!quest)
			{
				continue;
			}

			if (quest->QuestType == E_qt_Trunk)
			{
				if (quest->QuestLevel >= data.MinLevel && quest->QuestLevel <= data.MaxLevel)
				{
					hasAcceptedStoryQuest = true;
					_AddAcceptedQuestObject(storyElement, questId, storyIndex);
				}
			}
			else if (quest->QuestType == E_qt_Emegency)
			{
				// Quest #19995
				//if (quest->QuestLevel >= data.MinLevel && quest->QuestLevel <= data.MaxLevel)
				//{
				//	_AddAcceptedQuestObject(emergencyElement, questId, emergencyIndex);
				//}
				_AddAcceptedQuestObject(emergencyElement, questId, emergencyIndex);
			}
			else if (quest->QuestType == E_qt_Activity || 
				quest->QuestType == E_qt_Circle || 
				quest->QuestType == E_qt_Daily ||
				quest->QuestType == E_qt_Reward ||
				quest->QuestType == E_qt_Master ||
				quest->QuestType == E_qt_Wedding ||
				quest->QuestType == E_qt_Adventure)
			{
				if (quest->QuestLevel <= data.MaxLevel)
				{
					_AddAcceptedQuestObject(activityElement, questId, activityIndex);
				}
			}
			else if (quest->QuestType == E_qt_Guide)
			{
				if (quest->QuestLevel >= data.MinLevel && quest->QuestLevel <= data.MaxLevel)
				{
					_AddAcceptedQuestObject(guideElement, questId, guideIndex);
				}
			}
			else if (quest->QuestType == E_qt_Guild)
			{
				if (quest->QuestLevel <= data.MaxLevel)
				{
					_AddAcceptedQuestObject(guildElement, questId, guildIndex);
				}
			}
		}

		// Acceptable quests (recommended quests)
		const QuestTraceList& availableQuestList = MMOQuestData::getSingleton().GetAvailableQuestTraceList();
		iter = availableQuestList.begin();
		for(; iter != availableQuestList.end(); ++ iter)
		{
			INT questId = *iter;
			const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
			if (!quest)
			{
				continue;
			}

			if (storyIndex == 0 && quest->QuestType == E_qt_Trunk)
			{
				if (quest->QuestLevel >= data.MinLevel && quest->QuestLevel <= data.MaxLevel)
				{
					hasAvailableStoryQuest = true;
					_AddAvailableQuestObject(storyElement, questId, storyIndex);
				}
			}
			else if (activityIndex == 0 && 
				(quest->QuestType == E_qt_Activity || 
				quest->QuestType == E_qt_Circle || 
				quest->QuestType == E_qt_Daily ||
				quest->QuestType == E_qt_Reward ||
				quest->QuestType == E_qt_Master ||
				quest->QuestType == E_qt_Wedding ||
				quest->QuestType == E_qt_Adventure))
			{
				if (quest->QuestLevel <= data.MaxLevel)
				{
					_AddAvailableQuestObject(activityElement, questId, activityIndex);
				}
			}
			else if (guideIndex == 0 && quest->QuestType == E_qt_Guide)
			{
				if (quest->QuestLevel >= data.MinLevel && quest->QuestLevel <= data.MaxLevel)
				{
					_AddAvailableQuestObject(guideElement, questId, guideIndex);
				}
			}
			else if (guildIndex == 0 && quest->QuestType == E_qt_Guild)
			{
				if (quest->QuestLevel <= data.MaxLevel)
				{
					_AddAvailableQuestObject(guildElement, questId, guildIndex);
				}
			}
		}

		// #16342
		if (!hasAcceptedStoryQuest && !hasAvailableStoryQuest)
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				// #20691
				//UActorAttribute* attri = mainPlayer->ActorAttributes;
				//if (attri && attri->LevelAttributes.Level ==  data.MaxLevel)
				//{
					if (storyElement)
					{
						FString hintText = TEXT("");
						INT32 maxJourneyId = MMOJourneyData::getSingleton().GetMaxJourneyId();
						INT32 maxActivatedJourneyId = MMOJourneyData::getSingleton().GetMaxActivateJourneyId();
						if (data.Id < maxJourneyId)
						{
							if (data.Id == maxActivatedJourneyId)
							{
								hintText = FString::Printf(TEXT("Hero, please train hard first. Come back at level %d to experience the next chapters"), data.MaxLevel + 1);
							}
							else if (data.Id < maxActivatedJourneyId)
							{
								hintText = TEXT("All quests in the current chapter are complete");
							}
						}
						else
						{
							hintText = TEXT("All quests complete");
						}
						storyElement->SetString(TEXT("hintText"), *hintText);
					}
				//}
			}
		}

		// monster related
		for (int i = 0; i < data.MonsterList.Num(); ++ i)
		{
			if (data.MonsterList(i).MonsterId == 0)
			{
				continue;
			}

			_AddMonsterObject(monsterElement, 
				data.MonsterList(i).MonsterId, 
				data.MonsterList(i).MonsterName, 
				data.MonsterList(i).MonsterType,
				data.MonsterList(i).MonsterMapId, 
				data.MonsterList(i).MonsterMapRegion,
				data.MonsterList(i).MonsterPoint);
		}

		// boss related
		for (int i = 0; i < data.BossList.Num(); ++ i)
		{
			if (data.BossList(i).MonsterId == 0)
			{
				continue;
			}

			_AddBossObject(bossElement,  
				data.BossList(i).MonsterId, 
				data.BossList(i).MonsterName,
				data.BossList(i).MonsterType, 
				data.BossList(i).MonsterMapId, 
				data.BossList(i).MonsterMapRegion,
				data.BossList(i).MonsterPoint);
		}

		// achievement related
		AchievementIdSet personalSet = MMOAchievementData::getSingleton().GetPersonalAchievementBeTraced();
		AchievementIdSet::iterator personalIter = personalSet.begin();
		for (; personalIter != personalSet.end(); ++ personalIter)
		{
			UINT32 id = *personalIter;
			_AddAchievementObject(achievementElement, id, achievementIndex);
		}

		AchievementIdSet guildSet = MMOAchievementData::getSingleton().GetGuildAchievementBeTraced();
		AchievementIdSet::iterator guildIter = guildSet.begin();
		for (; guildIter != guildSet.end(); ++ guildIter)
		{
			UINT32 id = *guildIter;
			_AddAchievementObject(achievementElement, id, achievementIndex);
		}

		// Expanded item
		_SetJourneyElementExpand(storyElement, expandType == JT_STORY);
		_SetJourneyElementExpand(monsterElement, expandType == JT_MONSTER);
		_SetJourneyElementExpand(achievementElement, expandType == JT_ACHIEVEMENT);
		_SetJourneyElementExpand(emergencyElement, expandType == JT_EMERGENCY);
		_SetJourneyElementExpand(bossElement, expandType == JT_BOSS);
		_SetJourneyElementExpand(activityElement, expandType == JT_ACTIVITY);
		_SetJourneyElementExpand(guideElement, expandType == JT_GUIDE);
		_SetJourneyElementExpand(guildElement, expandType == JT_GUILD);
		_CurExpandType = expandType;

		journeyObject->SetObject(TEXT("journeyTraceList"), journeyArray);

	}

	eventCallUpdateJourneyTraceInfo(journeyObject);
}

void UMMOUIJourneyTracePanel::UpdateJourneyQuestData(INT questId)
{
	UGFxObject* object = MMOQuestManager::getSingleton().GetQuestTraceData(questId, GFXPlayer, EQTLT_Accepted);
	if (object)
	{
		eventCallUpdateJourneyQuestData(object);
	}
}
//////////////////////////////////////////////////////////////////////////

void UMMOUIJourneyTracePanel::ASGotoChapter(int journeyId)
{
	if (_CurJourneyId == journeyId)
	{
		return;
	}

	_CurJourneyId = journeyId;
	UpdateJourneyTraceInfo(journeyId, JT_STORY);
}

void UMMOUIJourneyTracePanel::ASUpdateNPCTips(int index)
{
	PlayJourneySound(index);
}

void UMMOUIJourneyTracePanel::ASShowAwardsPanel(int journeyId)
{
	JourneyData data;
	if (!MMOJourneyData::getSingleton().GetJourneyData(journeyId, data))
	{
		return;
	}

	ReputationTemp* reputationTemp = MMOFacade::getSingleton().GetReputationTempByForceID(data.ForceId);
	if (!reputationTemp)
	{
		return;
	}

	int curReputation = 0;
	int minReputation = 0;
	int maxReputation = 1;
	if (reputationTemp->ListLevel > 0)
	{
		curReputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), reputationTemp->ForceID);
		minReputation = reputationTemp->InitValue;
		maxReputation = reputationTemp->MaxValue;
	}

	/*
	if (curReputation >= maxReputation)
	{
		if (MMOFacade::getSingleton().GetPlayerAwardState(MMOFacade::getSingleton().GetMainPlayer(), reputationTemp->ForceID))
		{
			UMMOUIJourneyViewPanel* panel = (UMMOUIJourneyViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_JourneyView);
			if (panel)
			{
				panel->_CurJourneyId = journeyId;
				panel->Show(TRUE);
			}
		}
		else
		{
			CMessageHandler::GetInstance().Send_ReceiveReputationAward(data.ForceId);
		}
	}
	else
	{
		UMMOUIJourneyViewPanel* panel = (UMMOUIJourneyViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_JourneyView);
		if (panel)
		{
			panel->_CurJourneyId = journeyId;
			panel->Show(TRUE);
		}
	}
	*/

	// #20435
	UMMOUIJourneyViewPanel* panel = (UMMOUIJourneyViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_JourneyView);
	if (panel)
	{
		panel->SetCurJourneyId(journeyId);
		panel->Show(TRUE);
	}
}

void UMMOUIJourneyTracePanel::ASSetCurItemState(int journeyType, UBOOL expand)
{
	if (expand)
	{
		if (_CurExpandType != journeyType)
		{
			_CurExpandType = journeyType;
		}
	}
	else
	{
		if (_CurExpandType == journeyType)
		{
			_CurExpandType = -1;
		}
	}
}

void UMMOUIJourneyTracePanel::ASObtainAchievementReward(INT achievementId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (mainPlayer->eventIsDead())
	{
		FString text = TEXT("Cannot claim rewards while dead");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return;
	}

	if (MMOAchievementData::getSingleton().IsAchievementRewardObtain(achievementId))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAchievementRewarded"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return;
	}

	if (achievementId < ACHIEVEMENT_MAX)
	{
		CMessageHandler::GetInstance().Send_ObtainRewardItem(achievementId); 
	}
	else if (achievementId >= ACHIEVEMENT_MAX && achievementId < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(achievementId);
		CMessageHandler::GetInstance().Send_ObtainGuildReward(dataId);
	}
	else
	{
		UINT16 killId = HIWORD(achievementId);
		UINT8 killIndex = LOWORD(achievementId);
		CMessageHandler::GetInstance().Send_KillNpcAchievementComplete(killId, killIndex); 
	}
}

void UMMOUIJourneyTracePanel::ASShowJourneyTracePanle(UBOOL show)
{
	MMOJourneyManager::getSingleton().SetJourneyTraceExpand(show);
}

UBOOL UMMOUIJourneyTracePanel::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if (key == TEXT("LeftMouseButton") || key == TEXT("RightMouseButton"))
		{
			if (g_rootGFXPlayer && g_rootGFXPlayer->GetlockMouseCursorForUI() == false)
			{
				if (strHoverControllerName == TEXT("list_bg_mc") )
				{
					return TRUE;
				}
				if (strHoverControllerName == TEXT("hint_tf") )
				{
					return TRUE;
				}
				if (strHoverControllerName == TEXT("quest_trace_detail_tf") )
				{
					return TRUE;
				}
				if (strHoverControllerName == TEXT("quest_trace_caption_tf") )
				{
					return TRUE;
				}
				if (strHoverControllerName == TEXT("achievement_reward_mc") )
				{
					return TRUE;
				}
			}
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}