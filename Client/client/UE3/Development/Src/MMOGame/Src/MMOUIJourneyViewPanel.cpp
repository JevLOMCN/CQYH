#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"
#include "MMOJourneyManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOUIJourneyViewPanel)

void UMMOUIJourneyViewPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Journey);
	EventAdapter = new TGameEventAdapter<UMMOUIJourneyViewPanel>(this, evts);
}

void UMMOUIJourneyViewPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		if (_CurJourneyId == 0)
		{
			_CurJourneyId = MMOJourneyData::getSingleton().GetMaxActivateJourneyId();
		}

		UpdateJourneyList();
		UpdateJourneyInfo(_CurJourneyId);

		if (MMOJourneyData::getSingleton().GetFirstRewardJourneyId() == _CurJourneyId)
		{
			// 新手引导
			MMOGameEvent_Guide evtGuide(EGUET_OpenJourneyView);
			MMOUI::PostGameEvent(&evtGuide);
		}
	}
	else
	{
		_CurJourneyId = 0;
	}
}

void UMMOUIJourneyViewPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!IsVisible)
	{
		return;
	}

	switch(evt->GetEventType())
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
						JourneyData data;
						if (MMOJourneyData::getSingleton().GetJourneyData(_CurJourneyId, data))
						{
							if (journeyEvent->_Value1 == data.ForceId)
							{
								UpdateJourneyInfo(_CurJourneyId);
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
								UpdateJourneyInfo(_CurJourneyId);
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
}

void UMMOUIJourneyViewPanel::UpdateJourneyList()
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
			UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_JourneyView.AS3.JourneyViewData"));
			if (object)
			{
				object->SetString(TEXT("label"), data.Title);
				object->SetInt(TEXT("index"), index);
				object->SetInt(TEXT("type"), 0);
				object->SetInt(TEXT("deepth"), 0);
				object->SetBool(TEXT("visible"), true);
				object->SetInt(TEXT("journey_view_id"), data.Id);
				object->SetString(TEXT("journey_view_title"), data.Title);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}

	eventCallUpdateJourneyList(objectArray, _CurJourneyId - 1);
}

void UMMOUIJourneyViewPanel::_AddAwardObject(UGFxObject* arrayRewards, INT itemId, INT itemCount, INT& index)
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

	UGFxObject* element = GFXPlayer->CreateObject(TEXT("Object"));
	if (element)
	{
		element->SetInt(TEXT("item_id"), item->GetTemplateID());
		element->SetString(TEXT("item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));
		element->SetInt(TEXT("item_count"), itemCount);

		arrayRewards->SetElementObject(index, element);
		index ++;
	}
}

void UMMOUIJourneyViewPanel::SetCurJourneyId(INT journeyId)
{
	if (journeyId > MMOJourneyData::getSingleton().GetMaxActivateJourneyId())
	{
		return;
	}

	_CurJourneyId = journeyId;
}

void UMMOUIJourneyViewPanel::UpdateJourneyInfo(INT journeyId)
{
	JourneyData data;
	if (!MMOJourneyData::getSingleton().GetJourneyData(journeyId, data))
	{
		eventCallUpdateJourneyInfo(NULL);
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("UI_JourneyView.AS3.JourneyViewData"));
	if (!object)
	{
		return;
	}

	object->SetInt(TEXT("journey_view_id"), data.Id);
	object->SetString(TEXT("journey_view_icon"), data.Icon);
	object->SetString(TEXT("journey_view_desc"), data.Desc);
	object->SetString(TEXT("journey_view_title"), data.NpcName);

	object->SetObject(TEXT("journey_view_progress"), NULL);
	object->SetBool(TEXT("journey_view_complete"), FALSE);
	object->SetBool(TEXT("journey_view_get_awards"), FALSE);
	object->SetObject(TEXT("journey_view_rewards"), NULL);
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

			object->SetObject(TEXT("journey_view_progress"), progressObject);
			object->SetBool(TEXT("journey_view_complete"), curReputation >= maxReputation);
		}

		UGFxObject* arrayRewards = GFXPlayer->CreateArray();
		if (arrayRewards)
		{
			int index = 0;
			if (MMOFacade::getSingleton().GetMainActor())
			{
				switch(MMOFacade::getSingleton().GetMainActor()->GetClassId())
				{
				case EPCT_PlayerClassG: // 1.05 Lancer: reuses Warrior reputation rewards (no Spear columns)
			case EPCT_PlayerClassA://战士
					{
						for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
						{
							_AddAwardObject(arrayRewards, reputationTemp->_WarriorAwardItemID[i], reputationTemp->_WarriorAwardItemCount[i], index);
						}
					}
					break;
				case EPCT_PlayerClassH: // 1.05 Sorcerer: reuses Mage reputation rewards
			case EPCT_PlayerClassB://法师
					{
						for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
						{
							_AddAwardObject(arrayRewards, reputationTemp->_MageAwardItemID[i], reputationTemp->_MageAwardItemCount[i], index);
						}
					}
					break;
				case EPCT_PlayerClassC://刺客
					{
						for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
						{
							_AddAwardObject(arrayRewards, reputationTemp->_RogueAwardItemID[i], reputationTemp->_RogueAwardItemCount[i], index);
						}
					}
					break;
				case EPCT_PlayerClassD://弓手
					{
						for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
						{
							_AddAwardObject(arrayRewards, reputationTemp->_ArcherAwardItemID[i], reputationTemp->_ArcherAwardItemCount[i], index);
						}
					}
					break;
				case EPCT_PlayerClassE://道士
					{
						for (INT i = 0; i < MAX_REPUTATION_AWARD_COUNT; ++ i)
						{
							_AddAwardObject(arrayRewards, reputationTemp->_TaoistAwardItemID[i], reputationTemp->_TaoistAwardItemCount[i], index);
						}
					}
					break;
				}
			}

			object->SetObject(TEXT("journey_view_rewards"), arrayRewards);
			object->SetBool(TEXT("journey_view_get_awards"), MMOFacade::getSingleton().GetPlayerAwardState(MMOFacade::getSingleton().GetMainPlayer(), reputationTemp->ForceID));
		}
	}

	eventCallUpdateJourneyInfo(object);
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIJourneyViewPanel::ASUpdateJourneyInfo(int journeyId)
{
	_CurJourneyId = journeyId;
	UpdateJourneyInfo(journeyId);
}

void UMMOUIJourneyViewPanel::ASGetCompleteAwards(int journeyId)
{
	JourneyData data;
	if (MMOJourneyData::getSingleton().GetJourneyData(journeyId, data))
	{
		CMessageHandler::GetInstance().Send_ReceiveReputationAward(data.ForceId);
	}
}
