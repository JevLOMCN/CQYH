#include "stdafx.h"
#include "MMOAchievementManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "LinkManager.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include "MMOGuideManager.h"
#include "MMOStats.h"
#include "MMOHandbookManager.h"
#include "MMOJourneyManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

MMOAchievementManager::MMOAchievementManager()
{
	MMOUI::RegisterEventHandler(this, GameEvent_Achievement);

	_BeginSyncAchievementCheck = FALSE;
	_IsCompare = FALSE;
}

MMOAchievementManager::~MMOAchievementManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_Achievement);
}

void MMOAchievementManager::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOAchievementManager::Init"), 0);
	MMOAchievementData::getSingleton().Init();
}

void MMOAchievementManager::Shutdown()
{
	MMOAchievementData::getSingleton().Shutdown();
}

void MMOAchievementManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!evt)
	{
		return;
	}

	switch (evt->GetEventType())
	{
	case GameEvent_Achievement:
		{
			MMOGameEvent_Achievement* evtAch = (MMOGameEvent_Achievement*)evt;
			if (evtAch)
			{
				if (evtAch->_EventType > (INT)adInvalid && evtAch->_EventType < (INT)adTotal)
				{
					MMOAchievementData::getSingleton().UpdateCommonAchievementInfo(evtAch->_EventType, evtAch->_Value1, evtAch->_Value2, evtAch->_Value3);

					UMMOUIManager* uiManager = MMOUI::GetUIManager();
					if (uiManager)
					{
						UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanel(uiManager->Panel_Achievement);
						if (panel && panel->IsVisible)
						{
							MMOGameEvent_Achievement evtAch(MMOGameEvent_Achievement::EAET_UpdateAchievementList);
							MMOUI::PostGameEvent(&evtAch);
						}
					}
				}
			}
		}
		break;
	}
}

void MMOAchievementManager::Clear()
{
	MMOAchievementData::getSingleton().Clear();
	_BeginSyncAchievementCheck = FALSE;
	_AchievementCompleteSet.clear();
	_IsCompare = FALSE;
	_RoleNameCompare = TEXT("");
}

void MMOAchievementManager::Update(FLOAT deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_AchievementTime);
}

void MMOAchievementManager::SetCompare(UBOOL isCompare)
{
	_IsCompare = isCompare;
}

void MMOAchievementManager::CompleteCommonAchievement(UINT32 id)
{
	if (!_BeginSyncAchievementCheck)
	{
		return;
	}

	// Cache in a Set to avoid duplicate sends
	AchievementIdSet::iterator iter = _AchievementCompleteSet.find(id);
	if (iter == _AchievementCompleteSet.end())
	{
		CMessageHandler::GetInstance().Send_AchievementComplete(id);
		_AchievementCompleteSet.insert(id);
	}
}

void MMOAchievementManager::NotifyAchievementListUpdate(const char* list)
{
	MMOAchievementData::getSingleton().UpdateCommonAchievementList((ROLE_DB_ACHIEVEMENT_DATA*)list);
	_BeginSyncAchievementCheck = TRUE;
	_AchievementCompleteSet.clear();
}

void MMOAchievementManager::NotifyAchievementVariableUpdate(INT32 index, INT32 value)
{
	MMOAchievementData::getSingleton().UpdateAchievementVariable(index, value);
}

void MMOAchievementManager::NotifyAchievementComplete(UINT32 id, UINT32 date)
{
	MMOAchievementData::getSingleton().UpdateAchievementCompleteInfo(id, date);

	if (id < ACHIEVEMENT_MAX)
	{
		// Delete one for each one received
		AchievementIdSet::iterator iter = _AchievementCompleteSet.find(id);
		if (iter != _AchievementCompleteSet.end())
		{
			_AchievementCompleteSet.erase(iter);
		}
	}

	// Achievements with rewards
	if (MMOAchievementData::getSingleton().IsAchievementHasReward(id))
	{
		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_GetAchievement, id);
		MMOUI::PostGameEvent(&evtGuide);

		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			uiManager->delegateUpdateMenuEffect(1, EMenuType_Child_Achievement, TRUE, TRUE);
			UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Achievement);
			if (panel)
			{
				panel->SetRewardAchievementId(id);
			}
		}

		MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievementRewards, id);
		MMOUI::PostGameEvent(&evtJourney);
	}

	// Not in game state, no effects or prompts
	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		// Hint
		//MMOScreenMsgMgr::getSingleton().PushMsg(EScnMsgType_Achievement, id);
		MessageData *pNewMsgData = new MessageData();
		pNewMsgData->MessageType = ESMT_Achievement;
		pNewMsgData->MagicValue = id;
		ScreenMessageQueue::getSingleton().AddMessage(ESMC_TopCenter, pNewMsgData);

		// Message
		FString msg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Notify")), 
			*LinkManager::HtmlLink_Achievement(id));
		MMOChatManager::getSingleton().AddSystemMessage(msg);

		// Effect
		AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if (actor && player)
		{
			FLOAT h = 0;
			if (player->CylinderComponent)
			{
				h = player->CylinderComponent->CollisionHeight;
			}
			FVector pos = player->GetFootLocation();
			pos.Z += h * 2;

			player->PlayAchievementEffect(pos, actor->Rotation);
		}
	}

	// Event
	MMOGameEvent_Achievement evtAch1(adAchievement, id, 0, 0);
	MMOUI::PostGameEvent(&evtAch1);

	MMOGameEvent_Achievement evtAch2(adAchievementCount, MMOAchievementData::getSingleton().GetAchievementCompleteCount(), 0, 0);
	MMOUI::PostGameEvent(&evtAch2);

	MMOGameEvent_Achievement evtAch3(MMOGameEvent_Achievement::EAET_UpdateAchievementList);
	MMOUI::PostGameEvent(&evtAch3);
}

void MMOAchievementManager::NotifyAchievementCompareListUpdate(UINT32 roleId, const char* list)
{
	_IsCompare = TRUE;
	_RoleNameCompare = TEXT("");
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(roleId);
	if (actor)
	{
		_RoleNameCompare = actor->GetActorName();// >ActorName;
	}

	MMOAchievementData::getSingleton().UpdateAchievementCompareList((ROLE_DB_ACHIEVEMENT_DATA*)list);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Achievement, TRUE, "");
}

void MMOAchievementManager::NotifyAchievementRewardObtain(UINT32 id)
{
	MMOAchievementData::getSingleton().UpdateAchievementRewardObtain(id);

	if (id < ACHIEVEMENT_MAX)
	{
		AchievementData data;
		if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
		{
			if (data.RewardType[0] == adtItem)
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.GetAward"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Achievement.GetAward"));
			}
		}
	}

	// #20861
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		uiManager->delegateUpdateMenuEffect(1, EMenuType_Child_Achievement, TRUE, FALSE);
	}

	MMOGameEvent_Achievement evtAch(MMOGameEvent_Achievement::EAET_UpdateAchievementList);
	MMOUI::PostGameEvent(&evtAch);

	MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievementRewards, id);
	MMOUI::PostGameEvent(&evtJourney);
}

void MMOAchievementManager::NotifySyncKillNpcAchievementData(UINT16 killCount[MAX_KILL_NPC_QUEST_COUNT], UINT8 complete[MAX_KILL_NPC_QUEST_COUNT])
{
	MMOAchievementData::getSingleton().UpdateKillNpcAchievementList(killCount, complete);
}

void MMOAchievementManager::NotifySyncGuildAchievementInfo(char info[GUILD_MAX_ACHI_ARRAY_LENGTH])
{
	MMOAchievementData::getSingleton().UpdateGuildAchievementList(info);
}

void MMOAchievementManager::NotifyUpdateGuildAchievementInfo(char info[GUILD_ACHI_TYPE_NUM])
{
	MMOAchievementData::getSingleton().UpdateGuildAchievementInfo(info);
}

void MMOAchievementManager::NotifyUpdateKillNpcAchievementInfo(UINT16 killId, UINT16 killCount)
{
	MMOAchievementData::getSingleton().UpdateKillNpcAchievementInfo(killId, killCount);

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanel(uiManager->Panel_Achievement);
		if (panel && panel->IsVisible)
		{
			MMOGameEvent_Achievement evtAch(MMOGameEvent_Achievement::EAET_UpdateAchievementList);
			MMOUI::PostGameEvent(&evtAch);
		}
	}
}

void MMOAchievementManager::NotifyPlayerLevelChange(INT lastLevel, INT curLevel)
{
	MMOGameEvent_Achievement evtAch(adLevel, curLevel, 0, 0);
	MMOUI::PostGameEvent(&evtAch);

	MMOAchievementData::getSingleton().UpdateTraceAchievement(curLevel);

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanel(uiManager->Panel_Achievement);
		if (panel && panel->IsVisible)
		{
			MMOGameEvent_Achievement evtAch(MMOGameEvent_Achievement::EAET_UpdateAchievementList);
			MMOUI::PostGameEvent(&evtAch);
		}
	}
}
