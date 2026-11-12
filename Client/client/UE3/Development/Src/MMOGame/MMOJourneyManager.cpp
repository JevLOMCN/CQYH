#include "stdafx.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOJourneyManager.h"
#include "MMOQuestManager.h"
#include "MMOChatManager.h"
#include "MMOGuideManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

//////////////////////////////////////////////////////////////////////////
MMOJourneyManager::MMOJourneyManager()
{
	_IsExpand = TRUE;
	MMOUI::RegisterEventHandler(this, GameEvent_Journey);
}

MMOJourneyManager::~MMOJourneyManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_Journey);
}

void MMOJourneyManager::Init()
{
	MMOJourneyData::getSingleton().Init();
	_IsExpand = TRUE;
}

void MMOJourneyManager::Shutdown()
{
	MMOJourneyData::getSingleton().Shutdown();
}

void MMOJourneyManager::Clear()
{
	MMOJourneyData::getSingleton().Clear();
}

void MMOJourneyManager::Update(FLOAT deltaTime)
{
}

void MMOJourneyManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!evt)
	{
		return;
	}

	switch (evt->GetEventType())
	{
	case GameEvent_Journey:
		{
			MMOGameEvent_Journey* journeyEvent = (MMOGameEvent_Journey*)evt;
			if (journeyEvent)
			{
				switch (journeyEvent->EventType)
				{
				case MMOGameEvent_Journey::EJET_ReputationChange:
					{
						// #16373
						INT32 forceId = journeyEvent->_Value1;
						INT newReputation = journeyEvent->_Value2;
						INT oldReputation = journeyEvent->_Value3;

						JourneyData journeyData;
						if (MMOJourneyData::getSingleton().GetJourneyDataByReputation(forceId, journeyData))
						{
							if (newReputation - oldReputation > 0)
							{
								FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Reputation.ReputationDataAdd"));
								MMOChatManager::getSingleton().AddSystemMessage(FString::Printf(*text, *(journeyData.Title), newReputation - oldReputation)); 
							}
							else if (newReputation - oldReputation < 0)
							{
								FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Reputation.ReputationDataDec"));
								MMOChatManager::getSingleton().AddSystemMessage(FString::Printf(*text, *(journeyData.Title), oldReputation - newReputation)); 
							}
						}

						ReputationTemp* reputationTemp = MMOFacade::getSingleton().GetReputationTempByForceID(forceId);
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
								// #17524
								//FString text = TEXT("%s reputation is full");
								//MMOChatManager::getSingleton().AddHintMessage(FString::Printf(*text, *(journeyData.Title))); 
								//MMOUI::GetUIManager()->delegateAddScreenMessage(FString::Printf(*text, *(journeyData.Title)), MsgInfoType_Ex1, TEXT("")); 

								UMMOUIJourneyViewPanel* panel = (UMMOUIJourneyViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_JourneyView);
								if (panel)
								{
									panel->SetCurJourneyId(journeyData.Id);
								}

								if (journeyData.Id <= MMOJourneyData::getSingleton().GetMaxActivateJourneyId())
								{
									// Tutorial
									MMOGameEvent_Guide evtGuide(EGUET_ReputationFull);
									MMOUI::PostGameEvent(&evtGuide);
								}
							}
						}
					}
					break;

				case MMOGameEvent_Journey::EJET_GetAwards:
					{
						// Tutorial
						MMOGameEvent_Guide evtGuide(EGUET_GetJourneyAwards);
						MMOUI::PostGameEvent(&evtGuide);
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
			}
		}
		break;
	}
}

void MMOJourneyManager::LoadData(UINT32 journeyData)
{
	MMOJourneyData::getSingleton().ClearJourneyTypeActivate();

	for (int i = 0; i < JT_TOTAL; ++ i)
	{
		UINT32 flag = journeyData & (1 << i);
		if (flag > 0)
		{
			MMOJourneyData::getSingleton().ActivateJourneyType((JourneyType)i);
		}
		else
		{
			MMOJourneyData::getSingleton().DeactiveJourneyType((JourneyType)i);
		}
	}

	// ensure the story is unlocked
	MMOJourneyData::getSingleton().ActivateJourneyType(JT_STORY);

	_IsExpand = (HIWORD(journeyData) & 0x01) == 0;
}

void MMOJourneyManager::SaveData()
{
	UINT32 journeyData = 0;

	// low 16 bits store the activation type
	for (int i = 0; i < JT_TOTAL; ++ i)
	{
		journeyData |= MMOJourneyData::getSingleton().IsJourneyTypeActivate((JourneyType)i) ? (1 << i) : 0;
	}

	// high 16 bits store whether expanded: 0: expanded, 1: collapsed
	journeyData |= _IsExpand ? 0 : (1 << 16);

	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Journey_Trace, journeyData);
}

void MMOJourneyManager::ActivateJourneyType(JourneyType journeyType)
{
	MMOJourneyData::getSingleton().ActivateJourneyType(journeyType);
	SaveData();

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_ActivateJourneyType, journeyType);
	MMOUI::PostGameEvent(&evtGuide);
}

void MMOJourneyManager::DeactiveJourneyType(JourneyType journeyType)
{
	MMOJourneyData::getSingleton().DeactiveJourneyType(journeyType);
	SaveData();
}

void MMOJourneyManager::NotifyPlayerLevelChange(INT lastLevel, INT curLevel)
{
	MMOGameEvent_Journey evt(MMOGameEvent_Journey::EJET_LevelChange, lastLevel, curLevel);
	MMOUI::PostGameEvent(&evt);
}

void MMOJourneyManager::SetJourneyTraceExpand(UBOOL expand)
{
	_IsExpand = expand;
	SaveData();
}

UBOOL MMOJourneyManager::IsJourneyTraceExpand()
{
	return _IsExpand;
}