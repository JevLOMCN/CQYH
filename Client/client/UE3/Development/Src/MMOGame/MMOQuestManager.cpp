#include "stdafx.h"
#include "EngineAudioDeviceClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
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
#include "MMOStats.h"
#include "MMOClientVariableManager.h"
#include "FriendManager.h"
#include "MMOQuestManager.h"
#include "MMOSkillManager.h"
#include "MMOAssistManager.h"

MMOQuestManager::MMOQuestManager()
{
	MMOUI::RegisterEventHandler(this, GameEvent_Quest);
	MMOUI::RegisterEventHandler(this, GameEvent_NPCInteract);
	MMOUI::RegisterEventHandler(this, GameEvent_UseFlyItem);
	MMOUI::RegisterEventHandler(this, GameEvent_NPCAppear);
	MMOUI::RegisterEventHandler(this, GameEvent_NPCDestroyed);
	MMOUI::RegisterEventHandler(this, GameEvent_EnterScene);
	MMOUI::RegisterEventHandler(this, GameEVent_SceneLoadFinish);
	MMOUI::RegisterEventHandler(this, GameEvent_QuestModeChange);
	MMOUI::RegisterEventHandler(this, GameEvent_VariableChanged);

	_CurFlyQuestId = INVALID_QUEST_ID;
	_CurFlyDestId = -1;
	_LastFlyShoesUseTimes = 0;

	_IsAutoRefresh = FALSE;
	_HasRewardQuest = FALSE;
	_RewardQuestBrushLeftTime = REWARD_QUEST_BRUSH_CD_INTERVAL;
	_RewardQuestBrushLeftTimeProcess = 0.0f;

	_CurQuestGridType = -1;
	appMemzero(_CurQuestGridQuestId, sizeof(INT) * MAX_QUEST_PER_ZONE);
	_QuestGridQueryTimeProcess = 0.0f;

	_CurPage = 0;
	_PageTime = 0;
	_PageTimeProcess = 0;
	_QuestDialongContext = NULL;
	_CurPageQuestId = INVALID_QUEST_ID;

	_LimitedTimeQuestProcess = 0.0f;

	_FirstMatinee = FALSE;

	_CurTrunkQuestId = INVALID_QUEST_ID;
	_CurOtherQuestId = INVALID_QUEST_ID;

	_QuestCoolDownTimeProcess = 0.0f;

	_NeedUpdateQuestStatus = FALSE;
	_QuestStatusTimeProcess = 0.0f;

	_FirstEnterScene = FALSE;
}

MMOQuestManager::~MMOQuestManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_Quest);
	MMOUI::UnRegisterEventHandler(this, GameEvent_NPCInteract);
	MMOUI::UnRegisterEventHandler(this, GameEvent_UseFlyItem);
	MMOUI::UnRegisterEventHandler(this, GameEvent_NPCAppear);
	MMOUI::UnRegisterEventHandler(this, GameEvent_NPCDestroyed);
	MMOUI::UnRegisterEventHandler(this, GameEvent_EnterScene);
	MMOUI::UnRegisterEventHandler(this, GameEVent_SceneLoadFinish);
	MMOUI::UnRegisterEventHandler(this, GameEvent_QuestModeChange);
	MMOUI::UnRegisterEventHandler(this, GameEvent_VariableChanged);
}

void MMOQuestManager::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOQuestManager::Initialize"), 0);

	MMOQuestData::getSingleton().Init();

	GConfig->GetString(TEXT("TextColor"), TEXT("QuestCondComplete"), _CondCompleteColor, GUIColorIni);
	GConfig->GetString(TEXT("TextColor"), TEXT("QuestCondIncomplete"), _CondNotCompleteColor, GUIColorIni);
	_CondComplete = UITextParser::HTMLBeginFont(_CondCompleteColor);
	_CondComplete += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CondCompleted"));
	_CondComplete += UITextParser::HTMLEndFont();

	_QuestDialongContext = new DialogContext();
	GConfig->GetInt( TEXT("QuestPageTime") , TEXT("PageTime") , _PageTime , GDefaultGameIni );
}

void MMOQuestManager::Shutdown()
{
	SAFE_DELETE(_QuestDialongContext);
	MMOQuestData::getSingleton().Shutdown();
}

void MMOQuestManager::Clear()
{
	MMOQuestData::getSingleton().Clear();

	_CurFlyQuestId = INVALID_QUEST_ID;
	_CurFlyDestId = -1;
	_LastFlyShoesUseTimes = 0;

	_IsAutoRefresh = FALSE;
	_HasRewardQuest = FALSE;
	_RewardQuestBrushLeftTime = REWARD_QUEST_BRUSH_CD_INTERVAL;
	_RewardQuestBrushLeftTimeProcess = 0.0f;

	_CurQuestGridType = -1;
	appMemzero(_CurQuestGridQuestId, sizeof(INT) * MAX_QUEST_PER_ZONE);
	_QuestGridQueryTimeProcess = 0.0f;
	
	_CurPage = 0;
	_PageTimeProcess = 0;
	_CurPageQuestId = INVALID_QUEST_ID;

	_LimitedTimeQuestProcess = 0.0f;

	_FirstMatinee = FALSE;

	_QuestCoolDownTimeProcess = 0.0f;

	_NeedUpdateQuestStatus = FALSE;
	_QuestStatusTimeProcess = 0.0f;

	_FirstEnterScene = FALSE;

	ResetContent();

	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_ClearQuestDetail);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::_UpdateRewardQuest(FLOAT deltaTime)
{
	if (_HasRewardQuest)
	{
		_RewardQuestBrushLeftTimeProcess += deltaTime;
		if (_RewardQuestBrushLeftTimeProcess > 60.0f)
		{
			if (_RewardQuestBrushLeftTime > 0)
			{
				_RewardQuestBrushLeftTime -= (INT)(_RewardQuestBrushLeftTimeProcess / 60.0f);
			}

			if (_RewardQuestBrushLeftTime <= 0)
			{
				_RewardQuestBrushLeftTime = 0;
				CMessageHandler::GetInstance().Send_RefreshRewardQuestList(FALSE);
			}

			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateRewardQuestBrushTime);
			MMOUI::PostGameEvent(&evt);
			_RewardQuestBrushLeftTimeProcess = 0.0;
		}
	}
}

void MMOQuestManager::_ExploreZoneFromGrid(ZONE_DATA* zoneData, GRID_DATA* gridData)
{
	if (!zoneData || !gridData)
	{
		return;
	}

	if (!gridData->bQuest)
	{
		return;
	}
	
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(zoneData->nQuestID[0]);
	if (!quest)
	{
		return;
	}

	if (_CurQuestGridQuestId[0] != zoneData->nQuestID[0])
	{
		_CurQuestGridQuestId[0] = zoneData->nQuestID[0];
		CMessageHandler::GetInstance().Send_Explore();
	}

}

void MMOQuestManager::_AcceptQuestFromGrid(ZONE_DATA* zoneData, GRID_DATA* gridData)
{
	if (!zoneData || !gridData)
	{
		return;
	}

	if (!gridData->bQuest)
	{
		return;
	}
	
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(zoneData->nQuestID[0]);
	if (!quest)
	{
		return;
	}

	if (quest->StartType != EAQT_Zone)
	{
		return;
	}

	// non-emergency quest
	if (quest->QuestType != E_qt_Emegency)
	{
		if (_CurQuestGridQuestId[0] != zoneData->nQuestID[0])
		{
			_CurQuestGridQuestId[0] = zoneData->nQuestID[0];
			if (MMOQuestData::getSingleton().IsQuestAvailable(zoneData->nQuestID[0]))
			{
				if (quest->StartWithDialog)
				{
					UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
					if (dialog)
					{
						dialog->TryPopupQuestWithoutNpc(zoneData->nQuestID[0]);
					}
				}
				else
				{
					AcceptQuest(zoneData->nQuestID[0], FALSE, 0);
				}
			}
		}

		return;
	}

	// emergency quest
	for (INT i = 0; i < MAX_QUEST_PER_ZONE; ++ i)
	{
		quest = MMOQuestData::getSingleton().GetQuestData(zoneData->nQuestID[i]);
		if (!quest)
		{
			continue;
		}

		INT questId = quest->GetTemplateID();
		if (quest->StartNPCNewId > 0)
		{
			// check whether NPC is in view
			UBOOL npcAppear = FALSE;
			STLInt2IntMap* npcMap = MMOFacade::getSingleton().GetCharacterMap();
			if (npcMap)
			{
				STLInt2IntMap::iterator iter = npcMap->begin();
				for (; iter != npcMap->end(); ++ iter)
				{
					ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iter->second));
					if (npc && npc->NPCID == quest->StartNPCNewId)
					{
						npcAppear = TRUE;
						break;
					}
				}
			}

			if (npcAppear)
			{
				if (_CurQuestGridQuestId[i] != questId)
				{
					_CurQuestGridQuestId[i] = questId;
					if (MMOQuestData::getSingleton().IsQuestAvailable(questId))
					{
						// for quests with StartNpc, if StartNpc enters view and the quest was not accepted, accept it again
						CMessageHandler::GetInstance().Send_AcceptQuest(0, qatEmegency, 0);
					}
					else if (MMOQuestData::getSingleton().IsQuestAccepted(questId))
					{
						MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, questId);
						MMOUI::PostGameEvent(&evt);
					}
				}
			} 
			else
			{
				if (_CurQuestGridQuestId[i] != 0)
				{
					_CurQuestGridQuestId[i] = 0;
					MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, INVALID_QUEST_ID);
					MMOUI::PostGameEvent(&evt);
				}
			}
		}
		else
		{
			// update quest tracking
			if (_CurQuestGridQuestId[i] != questId)
			{
				_CurQuestGridQuestId[i] = questId;
				if (MMOQuestData::getSingleton().IsQuestAvailable(questId))
				{
					CMessageHandler::GetInstance().Send_AcceptQuest(0, qatEmegency, 0);
				}
				else if (MMOQuestData::getSingleton().IsQuestAccepted(questId))
				{
					MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, questId);
					MMOUI::PostGameEvent(&evt);
				}
			}
		}
	}
}

void MMOQuestManager::_CompleteQuestFromGrid(ZONE_DATA* zoneData, GRID_DATA* gridData)
{
	if (!zoneData || !gridData)
	{
		return;
	}

	if (!gridData->bQuest)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(zoneData->nQuestID[0]);
	if (!quest)
	{
		return;
	}

	if (quest->FinishType != ESQT_Zone)
	{
		return;
	}

	if (_CurQuestGridQuestId[0] != zoneData->nQuestID[0])
	{
		_CurQuestGridQuestId[0] = zoneData->nQuestID[0];
		if (MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()) &&
			!MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID()))
		{
			if (quest->FinishWithDialog)
			{
				UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
				if (dialog)
				{
					dialog->TryPopupQuestWithoutNpc(quest->GetTemplateID());
				}
			}
			else
			{
				INT selectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(quest->GetTemplateID());
				CompleteQuest(quest->GetTemplateID(), selectedAwardIndex);
			}
		}
	}
}

void MMOQuestManager::_CheckQuestFromGrid()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	// check quests on Guid
	ZONE_DATA* zoneData = NULL;
	GRID_DATA* gridData = NULL;
	gServerGridWorld.GetZoneAndGrid(mainPlayer->Location, zoneData, gridData);
	if (!zoneData || !gridData)
	{
		return;
	}

	if (!gridData->bQuest)
	{
		// check the previous data here, then clear it, to avoid calling UpdateQuestTraceList every time
		UBOOL foundQuest = FALSE;
		for (INT i = 0; i < MAX_QUEST_PER_ZONE; ++i)
		{
			if (_CurQuestGridQuestId[i] > 0)
			{
				foundQuest = TRUE;
				break;
			}
		}

		if (foundQuest)
		{
			appMemzero(_CurQuestGridQuestId, sizeof(INT) * MAX_QUEST_PER_ZONE);
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, INVALID_QUEST_ID);
			MMOUI::PostGameEvent(&evt);
		}

		// cannot clear quest type here, because grid cells do not exactly match Volumes
		//_CurQuestGridType = -1;
		return;	
	}

	if (_CurQuestGridType == EQTT_Explore)
	{
		_ExploreZoneFromGrid(zoneData, gridData);
	}
	else if (_CurQuestGridType == EQTT_Accept)
	{
		_AcceptQuestFromGrid(zoneData, gridData);
	}
	else if (_CurQuestGridType == EQTT_Complete)
	{
		_CompleteQuestFromGrid(zoneData, gridData);
	}
	else
	{
		// if _CurQuestGridType == -1, the character is already inside a QuestVolume upon entering the map or logging in
		// check here: if it is an emergency quest, handle it directly
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(zoneData->nQuestID[0]);
		if (quest && quest->QuestType == E_qt_Emegency)
		{
			_AcceptQuestFromGrid(zoneData, gridData);
		}
	}
}

void MMOQuestManager::_UpdateQuestFromGrid(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	_QuestGridQueryTimeProcess += deltaTime;
	if (_QuestGridQueryTimeProcess > QUEST_GRID_QUERY_TIME)
	{
		_CheckQuestFromGrid();
		_QuestGridQueryTimeProcess = 0.0;
	}
}

void MMOQuestManager::_UpdateQuestCoolDown(FLOAT deltaTime)
{
	_QuestCoolDownTimeProcess += deltaTime;
	if (_QuestCoolDownTimeProcess > QUEST_CD_TIME)
	{
		MMOQuestData::getSingleton().UpdateEmergencyQuestCoolDown((INT)(_QuestCoolDownTimeProcess * 1000));
		_QuestCoolDownTimeProcess = 0.0;
	}
}

void MMOQuestManager::_UpdateQuestStatusTime(FLOAT deltaTime)
{
	_QuestStatusTimeProcess += deltaTime;
	if (_QuestStatusTimeProcess > QUEST_UPDATE_TIME)
	{
		_UpdateQuestStatus();
		_QuestStatusTimeProcess = 0.0;
	}
}

void MMOQuestManager::_UpdateQuestTime(FLOAT deltaTime)
{
	_LimitedTimeQuestProcess += deltaTime;
	if (_LimitedTimeQuestProcess > 1.0f)
	{
		MMOQuestData::getSingleton().UpdateQuestTime((INT)(_LimitedTimeQuestProcess / 1.0f));
		_LimitedTimeQuestProcess = 0.0;
	}
}

void MMOQuestManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMOQuestManager");

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		_UpdateRewardQuest(deltaTime);
		_UpdateQuestPage(deltaTime);
		_UpdateQuestFromGrid(deltaTime);
	}

	// the server also counts time during Loading, the client must sync
	_UpdateQuestTime(deltaTime);
	_UpdateQuestCoolDown(deltaTime); 
	_UpdateQuestStatusTime(deltaTime);
}

void MMOQuestManager::_UpdateQuestPage(float deltaTime)
{
	if (GetMaxDialogPage() > MIN_PAGENUMBER && _CurPage < GetMaxDialogPage() - 1)
	{
		_PageTimeProcess += deltaTime;
		if (_PageTimeProcess >= _PageTime)
		{
			//change current page number
			SetQuestDialogCurrentPage(++ _CurPage);

			//send page-turn message
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestPage,_CurPageQuestId);
			MMOUI::PostGameEvent(&evt);

			_PageTimeProcess = 0;
		}
	}
}

void MMOQuestManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch (evt->GetEventType())
	{
	case GameEvent_NPCInteract:
		{
			MMOGameEvent_NPCInteract* npcEvent = (MMOGameEvent_NPCInteract*)evt;
			check(npcEvent);
			_OnNPCInteract(npcEvent->NPCServerID, npcEvent->NPCTemplateID);
		}
		break;

	case GameEvent_EnterScene:
		{
			if (!_FirstEnterScene)
			{
				_ProcessFirstEnterScene();
				_FirstEnterScene = TRUE;
			}

			// fix bug #23546
			UpdateAllQuestTargets();
			UpdateQuestStatus();

			// fix bug #19940
			// update quest tracking
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, INVALID_QUEST_ID);
			MMOUI::PostGameEvent(&evt);
		}
		break;

	case GameEVent_SceneLoadFinish:
		{
			_LevelLoaded();
		}
		break;

	case GameEvent_Quest:
		{
			MMOGameEvent_Quest* questEvt = (MMOGameEvent_Quest*)evt;
			if(questEvt == NULL)
			{
				return;
			}

			const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questEvt->QuestId);
			if (!quest)
			{
				return;
			}

			switch (questEvt->EventType)
			{
			case MMOGameEvent_Quest::EQET_EnterQuestZone:
				{
					// TODO:
					// this message is not received when using flying shoes, on character death return to spawn point, on exiting the game, etc.
					// so _CurQuestGridType still needs to be obtained from GRID_DATA
					_CurQuestGridType = questEvt->QuestGridType;
					_CheckQuestFromGrid();
				}
				break;

			case MMOGameEvent_Quest::EQET_LeaveQuestZone:
				{
					// this message is not received when using flying shoes, on character death return to spawn point, on exiting the game, etc.
					// so it still needs to be cleared in _CheckQuestFromGrid
					_CurQuestGridType = -1;

					if (quest->StartType == EAQT_Zone || quest->FinishType == ESQT_Zone)
					{
						UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
						if (dialog)
						{
							dialog->CloseDialog(FALSE);	
						}
					}
				}
				break;
			}
		}
		break;

	case GameEvent_UseFlyItem:
		{
			MMOGameEvent_MsgboxCallBack* msg = (MMOGameEvent_MsgboxCallBack*)(evt);
			if (msg)
			{
				if (msg->Agree)
				{
					AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
					if (mainPlayer)
					{
						if (mainPlayer->GetMoney(MONEYTYPE_02) < FLY_TO_QUEST_ZONE_COST)
						{
							MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.NotEnoughGoldToUseFlyShoestring")),
								MsgInfoType_Table,
								TEXT("MMOGame.Quest.NotEnoughGoldToUseFlyShoestring"));
						}
						else
						{
							if (_CurFlyQuestId != INVALID_QUEST_ID && _CurFlyDestId != -1)
							{
								CMessageHandler::GetInstance().Send_FlyToTarget(_CurFlyQuestId, _CurFlyDestId);
							}
						}
					}
				}
				_CurFlyQuestId = INVALID_QUEST_ID;
				_CurFlyDestId = -1;
			}
		}
		break;

	case GameEvent_NPCAppear:
		{
			//MMOGameEvent_NPCAppear* npcEvent = (MMOGameEvent_NPCAppear*)evt;
			//if (npcEvent->Actor && npcEvent->Actor->IsA(ANPC::StaticClass()))
			//{
			//	ANPC* npc = Cast<ANPC>(npcEvent->Actor);
			//	if (npc)
			//	{
			//		//UpdateNpcName(npc);
			//	}
			//}
		}
		break;

	case GameEvent_NPCDestroyed:
		{
			//MMOGameEvent_NPCDestroyed* npcEvent = (MMOGameEvent_NPCDestroyed*)evt;
			//if (npcEvent->Actor && npcEvent->Actor->IsA(ANPC::StaticClass()))
			//{
			//	ANPC* npc = Cast<ANPC>(npcEvent->Actor);
			//	if (npc)
			//	{
			//		//UpdateNpcName(npc);
			//	}
			//}
		}
		break;

	case GameEvent_QuestModeChange:
		{
			UpdateQuestStatus();
		}
		break;

	case GameEvent_VariableChanged:
		{
			MMOGameEvent_VariableChange* varChgEvent = (MMOGameEvent_VariableChange*)evt;
			if (varChgEvent)
			{
				UpdateVariable(varChgEvent->VarType, varChgEvent->VarReferID, varChgEvent->VarID, varChgEvent->VarValue);
			}
		}
		break;
	}
}

void MMOQuestManager::UpdateNpcHp(ANPC* npc, INT hp)
{
	if (!npc)
	{
		return;
	}

	if(npc->GetActorType()==EACT_Monster && npc->Quality <= E_nq_Normal5)
	{
		return;
	}
	else
	{
		QuestTraceList questList;
		questList.clear();
		if (MMOQuestData::getSingleton().GetNormalQuestListByMonsterBeTraced(npc->NPCID, questList))
		{
			QuestTraceList::iterator iter = questList.begin();
			for (; iter != questList.end(); ++ iter)
			{
				if (MMOQuestData::getSingleton().IsSingleMonsterQuest(*iter))
				{
					MMOGameEvent_Quest questEvent(MMOGameEvent_Quest::EQET_UpdateQuest, *iter);
					MMOUI::PostGameEvent(&questEvent);
				}
			}
		}

		questList.clear();
		if (MMOQuestData::getSingleton().GetEmergencyQuestListByMonsterBeTraced(npc->NPCID, questList))
		{
			QuestTraceList::iterator iter = questList.begin();
			for (; iter != questList.end(); ++iter)
			{
				if (MMOQuestData::getSingleton().IsSingleMonsterQuest(*iter))
				{
					MMOGameEvent_Quest questEvent(MMOGameEvent_Quest::EQET_UpdateQuest, *iter);
					MMOUI::PostGameEvent(&questEvent);
				}
			}
		}
	}
}

void MMOQuestManager::UpdateNpcName(ANPC* npc)
{
	MMOUI::GetUIManager()->delegateUpdateHintText(npc);

	//if (!npc)
	//{
	//	return;
	//}

	//ULocalPlayer* localPlayer = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
	//if (!localPlayer)
	//{
	//	return;
	//}

	//UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
	//if (!sceneHint)
	//{
	//	return;
	//}

	//FLOAT HudNameDisplayRangeSquared2D = 0.0f;
	//GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);
	//sceneHint->UpdateHints_NPC(npc, localPlayer, TRUE, HudNameDisplayRangeSquared2D * 2.0f);
}

void MMOQuestManager::_UpdateQuestStatus()
{
	if (!_NeedUpdateQuestStatus)
	{
		return;
	}

	_NeedUpdateQuestStatus = FALSE;

	// NPC
	STLInt2IntMap* characterMap = MMOFacade::getSingleton().GetCharacterMap();
	if (characterMap)
	{
		STLInt2IntMap::iterator iterCharacter = characterMap->begin();
		for (; iterCharacter != characterMap->end(); ++iterCharacter)
		{
			ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iterCharacter->second));
			if (npc)
			{
				UpdateQuestNpcStatus(npc);
				//UpdateNpcName(npc);
			}
		}
	}

	MMOFacade::getSingleton().RefleshAllNPCHints();

	// Doodad
	DoodadManager::getSingleton().RefreshDoodadEffects();
	DropActorMap dropActorMap = MMODropManager::getSingleton().GetDropActorMap();
	DropActorMap::iterator iterDrop = dropActorMap.begin();
	for (; iterDrop != dropActorMap.end(); ++iterDrop)
	{
		FDropActorInfo& dropInfo = iterDrop->second;
		if (dropInfo.DropActor)
		{
			UpdateQuestDropItemStatus(dropInfo.DropActor);
		}
	}

	MMOGameEvent_Any evt(GameEvent_QuestStateChanged);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::UpdateQuestStatus(UBOOL bForceUpdate)
{
	_NeedUpdateQuestStatus = TRUE;
	if (bForceUpdate)
	{
		_UpdateQuestStatus();
	}
}

void MMOQuestManager::UpdateQuestTargets(INT32 questId)
{
	if (questId == INVALID_QUEST_ID)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	if (quest->FinishType == ESQT_Auto)
	{
		// condition met, auto-complete quest (entering an area to complete a quest does not count)
		if (MMOQuestData::getSingleton().IsQuestAccepted(questId) && 
			MMOQuestData::getSingleton().IsQuestCompassable(questId))
		{
			if (quest->FinishWithDialog)
			{
				UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
				if (dialog)
				{
					dialog->TryPopupQuestWithoutNpc(questId);
				}
			}
			else
			{
				INT selectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(questId);
				CompleteQuest(questId, selectedAwardIndex);
			}
		}
	}
	else if (MMOQuestData::getSingleton().IsQuestRecycle(questId))
	{
		// item recycling quest completes directly
		if (MMOQuestData::getSingleton().IsQuestAccepted(questId) && 
			MMOQuestData::getSingleton().IsQuestCompassable(questId))
		{
			INT selectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(questId);
			CompleteQuest(questId, selectedAwardIndex);
		}
	}

	MMOGameEvent_Quest questEvent(MMOGameEvent_Quest::EQET_UpdateQuest, questId);
	MMOUI::PostGameEvent(&questEvent);
}

void MMOQuestManager::UpdateAllQuestTargets()
{
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::iterator iter = acceptedQuests.begin();
	for (; iter != acceptedQuests.end(); ++iter)
	{
		UpdateQuestTargets(iter->first);
	}
}

void MMOQuestManager::_OnNPCInteract(INT serverId, INT npcId)
{
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::const_iterator iter = acceptedQuests.begin();
	for (; iter != acceptedQuests.end(); ++ iter)
	{
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(iter->second.Id);
		if (quest && quest->TalkNpcID == npcId)
		{
			CMessageHandler::GetInstance().Send_QuestNPCTalk(iter->first, serverId);
		}
	}
}

void MMOQuestManager::GetQuestStateIcon(INT type, EQuestHUDStatus state, UBOOL isSmall, FString& icon)
{
	if (!isSmall)
	{
		if (!MMOAssistManager::getSingleton().IsQuestMode())
		{
			icon = TEXT("");
			return;
		}
	}

	switch (state)
	{
	case EQHUDS_WillAcceptable:
		icon = isSmall ? TEXT("img://UI_Quest.t_small_gray") : icon = TEXT("img://UI_Quest.t_big_gray");
		break;

	case EQHUDS_Acceptable:
		{
			switch (type)
			{
			case E_qt_Trunk:
			case E_qt_Branch:
			case E_qt_Guide:
				icon = isSmall ? TEXT("img://UI_Quest.t_small_yellow") : TEXT("img://UI_Quest.t_big_yellow");
				break;

			case E_qt_Activity:
			case E_qt_Reward:
			case E_qt_Circle:
				icon = isSmall ? TEXT("img://UI_Quest.t_small_green") : TEXT("img://UI_Quest.t_big_green");
				break;

			case E_qt_Master:
			case E_qt_Guild:
			case E_qt_Emegency:
				icon = isSmall ? TEXT("img://UI_Quest.t_small_red") : TEXT("img://UI_Quest.t_big_red");
				break;

			case E_qt_Daily:
			case E_qt_Adventure:
				icon = isSmall ? TEXT("img://UI_Quest.t_small_blue") : TEXT("img://UI_Quest.t_big_blue");
				break;
			}
		}
		break;

	case EQHUDS_CanSubmit:
		{
			switch (type)
			{
			case E_qt_Trunk:
			case E_qt_Branch:
			case E_qt_Guide:
				icon = isSmall ? TEXT("img://UI_Quest.w_small_yellow") : TEXT("img://UI_Quest.w_big_yellow");
				break;

			case E_qt_Activity:
			case E_qt_Reward:
			case E_qt_Circle:
				icon = isSmall ? TEXT("img://UI_Quest.w_small_green") : TEXT("img://UI_Quest.w_big_green");
				break;

			case E_qt_Master:
			case E_qt_Guild:
			case E_qt_Emegency:
				icon = isSmall ? TEXT("img://UI_Quest.w_small_red") : TEXT("img://UI_Quest.w_big_red");
				break;

			case E_qt_Daily:
			case E_qt_Adventure:
				icon = isSmall ? TEXT("img://UI_Quest.w_small_blue") : TEXT("img://UI_Quest.w_big_blue");
				break;
			}
		}
		break;

	case EQHUDS_NotValidForSubmit:
		icon = isSmall ? TEXT("img://UI_Quest.w_small_gray") : TEXT("img://UI_Quest.w_big_gray");
		break;
	}
}

void MMOQuestManager::_GetMonsterHp(INT npcId, INT& curHp, INT& maxHp)
{
	curHp = 0;
	maxHp = 0;
	STLInt2IntMap* npcMap = MMOFacade::getSingleton().GetCharacterMap();
	if (npcMap)
	{
		STLInt2IntMap::iterator iter = npcMap->begin();
		for (; iter != npcMap->end(); ++ iter)
		{
			ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iter->second));
			if (npc && npc->IsMonster() && npc->ActorAttributes && npc->NPCID == npcId)
			{
				curHp = npc->ActorAttributes->GetCurHp();
				maxHp = npc->ActorAttributes->GetMaxHp();
				return;
			}
		}
	}
}

void MMOQuestManager::UpdateQuestNpcStatus(class ANPC* npc)
{
	if (!npc)
	{
		return;
	}

	if (!npc->Template)
	{
		return;
	}

	if (npc->IsMonster())
	{
		return;
	}

	if (MMOFacade::getSingleton().IsAttackable(MMOFacade::getSingleton().GetMainPlayer(), npc))
	{
		return;
	}

	QuestDataMap allQuests = MMOQuestData::getSingleton().GetQuestDataMap();
	QuestDataMap::const_iterator iter = allQuests.begin();
	for (; iter != allQuests.end(); ++ iter)
	{
		const UQuest* quest = iter->second;
		if (!quest)
		{
			continue;
		}

		// emergency quests cannot be accepted from NPC
		if (quest->StartNPCNewId > 0 && quest->StartNPCNewId == npc->Template->TemplateID && quest->QuestType != E_qt_Emegency)
		{
			int index = npc->AcceptableQuestList.FindItemIndex((UQuest*)quest);
			if (index < 0)
			{
				npc->AcceptableQuestList.AddItem((UQuest*)quest);
			}
		}

		//UNPCTemplate* finishNpc = ((UQuest*)quest)->GetFinishNpc();
		//if (finishNpc && finishNpc->TemplateID == npc->Template->TemplateID)
		if(quest->FinishNpcNewId == npc->Template->TemplateID)
		{
			int index = npc->CompassableQuestList.FindItemIndex((UQuest*)quest);
			if (index < 0)
			{
				npc->CompassableQuestList.AddItem((UQuest*)quest);
			}	
		}
	}

	npc->DetachHeadUp();
	BYTE type = E_qt_MAX;
	EQuestHUDStatus state = EQHUDS_None;

	// available quests
	for (INT i = 0; i < npc->AcceptableQuestList.Num(); ++ i)
	{
		UQuest* quest = npc->AcceptableQuestList(i);		
		if (!quest)
		{
			continue;
		}

		if (MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID()))
		{
			type = quest->QuestType;	
			state = EQHUDS_Acceptable;		
			break;
		}						
	}

	// bounty quest NPC
	if (npc->NPCID == REWARD_QUEST_NPC_ID)
	{
		RewardQuestList questList = MMOQuestData::getSingleton().GetRewardQuestList();
		RewardQuestList::iterator iter = questList.begin();
		for (; iter != questList.end(); ++ iter)
		{
			if (MMOQuestData::getSingleton().IsQuestAvailable(iter->QuestId))
			{
				type = E_qt_Reward;		
				state = EQHUDS_Acceptable;	
				break;
			}	
		}
	}

	// completable quests
	for (INT i = 0; i < npc->CompassableQuestList.Num(); ++ i)
	{
		UQuest* quest = npc->CompassableQuestList(i);
		if (!quest)
		{
			continue;	
		}

		if (MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
		{
			type = quest->QuestType;	
			state = EQHUDS_CanSubmit;			
			break;
		} 

		if (MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID()) && 
			!MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
		{
			type = quest->QuestType;
			state = EQHUDS_NotValidForSubmit;
		}
	}

	FString icon = TEXT("");
	GetQuestStateIcon(type, state, FALSE, icon);
	_SetNpcIcon(npc, icon);

	// #15962 check whether quest has a transit NPC
	for (INT i = 0; i < npc->CompassableQuestList.Num(); ++ i)
	{
		UQuest* quest = npc->CompassableQuestList(i);
		if (!quest)
		{
			continue;	
		}

		_CheckTransitQuestNpc(quest);
	}
}

void MMOQuestManager::_SetNpcIcon(class ANPC* npc, const FString& iconName)
{
	UMMOUISceneHint* scenezHintPanel = 
		Cast<UMMOUISceneHint>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_SceneHint));
	if (scenezHintPanel)
	{
		scenezHintPanel->SetIconName(npc, iconName);
	}
}

void MMOQuestManager::_CheckTransitQuestNpc(const UQuest* quest)
{
	if (!quest)
	{
		return;
	}

	UBOOL showMark = MMOQuestData::getSingleton().IsQuestAccepted(quest->QuestTempID) && 
		!MMOQuestData::getSingleton().IsQuestCompleted(quest->QuestTempID);

	FString naKey = TEXT("<#NA:");
	FString naIcon = TEXT("img://UI_Quest.a_big");
	ParseContext parseContext;
	DialogContext dlgContext;

	// quest description
	if (!quest->QuestDescript.IsEmpty())
	{
		INT naStart = quest->QuestDescript.InStr(naKey);
		if (naStart >= 0)
		{
			FString naSubStr = quest->QuestDescript.Mid(naStart + naKey.Len());
			TCHAR* naSubText = (TCHAR*)(*naSubStr);
			INT naIndex = 0;
			INT npcId = 0;
			INT mapId = 0;
			ITextParser::ParseNPC(parseContext, naSubText, naIndex, naSubStr.Len(), npcId, mapId);
			if (npcId > 0)
			{
				STLInt2IntMap* characterMap = MMOFacade::getSingleton().GetCharacterMap();
				if (characterMap)
				{
					STLInt2IntMap::iterator iterCharacter = characterMap->begin();
					for (; iterCharacter != characterMap->end(); ++ iterCharacter)
					{
						ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iterCharacter->second));
						if (npc && npc->GetTemplateID() == npcId)
						{
							_SetNpcIcon(npc, showMark ? naIcon : TEXT(""));
						}
					}
				}
			}
		}
	}

	// quest variables
	for (INT i = 0; i < quest->CheckVar.Num(); ++i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType != TEXT("vtInvalid") && !checkVar.Description.IsEmpty())
		{
			INT naStart = checkVar.Description.InStr(naKey);
			if (naStart >= 0)
			{
				FString naSubStr = checkVar.Description.Mid(naStart + naKey.Len());
				TCHAR* naSubText = (TCHAR*)(*naSubStr);
				INT naIndex = 0;
				INT npcId = 0;
				INT mapId = 0;
				ITextParser::ParseNPC(parseContext, naSubText, naIndex, naSubStr.Len(), npcId, mapId);
				if (npcId > 0)
				{
					STLInt2IntMap* characterMap = MMOFacade::getSingleton().GetCharacterMap();
					if (characterMap)
					{
						STLInt2IntMap::iterator iterCharacter = characterMap->begin();
						for (; iterCharacter != characterMap->end(); ++ iterCharacter)
						{
							ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iterCharacter->second));
							if (npc && npc->GetTemplateID() == npcId)
							{
								_SetNpcIcon(npc, showMark ? naIcon : TEXT(""));
							}
						}
					}
				}
			}
		}
	}

	// quest items
	for (INT i = 0; i < quest->CheckItem.Num(); ++ i)
	{
		FQuestCheckItem checkItem = quest->CheckItem(i);					
		if (!checkItem.Description.IsEmpty())
		{
			INT naStart = checkItem.Description.InStr(naKey);
			if (naStart >= 0)
			{
				FString naSubStr = checkItem.Description.Mid(naStart + naKey.Len());
				TCHAR* naSubText = (TCHAR*)(*naSubStr);
				INT naIndex = 0;
				INT npcId = 0;
				INT mapId = 0;
				ITextParser::ParseNPC(parseContext, naSubText, naIndex, naSubStr.Len(), npcId, mapId);
				if (npcId > 0)
				{
					STLInt2IntMap* characterMap = MMOFacade::getSingleton().GetCharacterMap();
					if (characterMap)
					{
						STLInt2IntMap::iterator iterCharacter = characterMap->begin();
						for (; iterCharacter != characterMap->end(); ++ iterCharacter)
						{
							ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iterCharacter->second));
							if (npc && npc->GetTemplateID() == npcId)
							{
								_SetNpcIcon(npc, showMark ? naIcon : TEXT(""));
							}
						}
					}
				}
			}
		}
	}
}

void MMOQuestManager::UpdateQuestDropItemStatus(class AMMODropActor* drop)
{
	if (!drop)
	{
		return;
	}

	if (!drop->TemplateItemID)
	{
		return;
	}

	BYTE type = E_qt_MAX;
	EQuestHUDStatus state = EQHUDS_None;

	// available quests
	QuestDataMap allQuests = MMOQuestData::getSingleton().GetQuestDataMap();
	for (auto iter = allQuests.begin(); iter != allQuests.end(); ++ iter)
	{
		const UINT32 questId = iter->first;
		UQuest* quest = iter->second;
		if (!quest)
		{
			continue;	
		}

		if (quest->StartItemID != drop->TemplateItemID)
		{
			continue;
		}

		// if already in bag
		INT itemSlot = -1;
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(drop->TemplateItemID, E_CONSLOT_DEFAULT, itemSlot);
		if (itemInfo && itemSlot >= 0)
		{
			continue;
		}

		if (quest->StartItemID == drop->TemplateItemID &&
			MMOQuestData::getSingleton().IsQuestAvailable(questId))
		{
			type = quest->QuestType;
			state = EQHUDS_Acceptable;
			break;
		}
		
		if (quest->FinishItemID == drop->TemplateItemID &&
			MMOQuestData::getSingleton().IsQuestAccepted(quest->QuestTempID) && 
			MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
		{
			type = quest->QuestType;
			state = EQHUDS_CanSubmit;	
			break;
		}
		
		if (quest->FinishItemID == drop->TemplateItemID &&
			MMOQuestData::getSingleton().IsQuestAccepted(quest->QuestTempID) && 
			!MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
		{
			type = quest->QuestType;
			state = EQHUDS_NotValidForSubmit;
		}
	}

	FString icon = TEXT("");
	GetQuestStateIcon(type, state, FALSE, icon);
 	MMODropManager::getSingleton().SetDropTipsIcon(drop->DropID, icon);
}

void MMOQuestManager::TraceAllQuest(bool traceOn)
{
	MMOQuestData::getSingleton().TraceAllQuest(traceOn);
	UpdateQuestStatus(TRUE);

	// update quest tracking
	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, INVALID_QUEST_ID);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::TraceQuest(INT questId, bool traceOn)
{
	MMOQuestData::getSingleton().TraceQuest(questId, traceOn);

	// update quest tracking
	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateQuestTraceList, questId);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::UpdateQuestBuff(INT buffId)
{
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::iterator iter = acceptedQuests.begin();
	for (; iter != acceptedQuests.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(data.Id);
		if (quest && quest->CheckBuff == buffId)
		{
			UpdateQuestTargets(data.Id);
			UpdateQuestStatus();
		}
	}
}

void MMOQuestManager::UpdateQuestReputation()
{
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::iterator iter = acceptedQuests.begin();
	for (; iter != acceptedQuests.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(data.Id);
		if (quest && quest->CheckReputationForceID > 0)
		{
			UpdateQuestTargets(quest->GetTemplateID());
			UpdateQuestStatus();
		}
	}
}

void MMOQuestManager::UpdateQuestItem(INT32 itemId, UBOOL isAdd)
{
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::iterator iter = acceptedQuests.begin();
	for (; iter != acceptedQuests.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(data.Id);
		if (!quest)
		{
			continue;
		}

		for (INT i = 0; i < quest->CheckItem.Num(); ++i)
		{
			const FQuestCheckItem* checkItem = &quest->CheckItem(i);
			UItem* item = ((UQuest*)quest)->GetCheckItem(i);
			if (item && item->GetTemplateID() == itemId)
			{
				UpdateQuestTargets(quest->GetTemplateID());
				// items can be added or removed, so quest state must be evaluated at all times
				UpdateQuestStatus();

				// tips
				if (isAdd)
				{
					INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(itemId, E_CONSLOT_DEFAULT);
					itemCount = Min(itemCount, checkItem->CheckItemCnt);
					FString itemName = item->GetDisplayName();
					FString desc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.Report2")), *itemName, itemCount, checkItem->CheckItemCnt);
					MMOUI::GetUIManager()->delegateAddScreenMessage(desc,MsgInfoType_Table,TEXT("MMOGame.Quest.Report2"));
				}
				break;
			}
		}
	}
}

void MMOQuestManager::UpdateAllQuestItem()
{
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::iterator iter = acceptedQuests.begin();
	for (; iter != acceptedQuests.end(); ++ iter)
	{
		AcceptedQuestData& data = iter->second;
		const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(data.Id);
		if (!quest)
		{
			continue;
		}

		for (INT i = 0; i < quest->CheckItem.Num(); ++i)
		{
			const FQuestCheckItem* checkItem = &quest->CheckItem(i);	
			if (checkItem->CheckItemID > 0)
			{
				INT itemSlot = -1;
				FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(checkItem->CheckItemID, E_CONSLOT_DEFAULT, itemSlot);
				INT equipSlot = -1;
				FClientItemInfo* equipInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(checkItem->CheckItemID, E_CONSLOT_EQUIPPED, equipSlot);
				if ((itemInfo && itemSlot >= 0) || (equipInfo && equipSlot >= 0))
				{
					UpdateQuestTargets(quest->GetTemplateID());
					UpdateQuestStatus();
					break;
				}
			}
		}
	}
}

void MMOQuestManager::UpdateVariable(UINT8 varType, INT32 referId, UINT16 index, INT32 val)
{
	switch (varType)
	{
	case vtRole:
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer && referId == mainPlayer->ServerID)
			{
				UpdateRoleVariable(index, val);
			}

			// LD uses variable 132 to trigger the first-time-in-game Matinee, but the level is not loaded yet, so handle it with a delay
			if (index == 132 && val == 1)
			{
				_FirstMatinee = TRUE;
			}
			else
			{
				if (mainPlayer && !mainPlayer->eventIsDead())
				{
					MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Role>(index, referId, val, false);
				}
			}
		}
		break;

	case vtQuest:
		UpdateQuestVariable(referId, index, val);
		break;

	case vtKillNpcCount:
		UpdateQuestNpcKillCount(referId, index, val);
		break;

	case vtKillDoodad:
		UpdateQuestDoodadKillCount(referId, index, val);
		break;

	case vtKillPlayerCount:
		UpdateQuestPlayerKillCount(referId, index, val);
		break;

	default:
		break;
	}
}

void MMOQuestManager::UpdateQuestVariable(INT questId, UINT16 index, INT val)
{
	MMOQuestData::getSingleton().UpdateQuestVariable(questId, index, val);
	UpdateQuestTargets(questId);
	UpdateQuestStatus();
}

void MMOQuestManager::UpdateAllClientVariable(INT32 clientVariable[MAX_CLIENT_VARIABLE_COUNT])
{
	MMOQuestData::getSingleton().UpdateAllClientVariable(clientVariable);
}

void MMOQuestManager::UpdateRoleVariable(UINT16 index, INT val)
{
	MMOQuestData::getSingleton().UpdateRoleVariable(index, val);
}

void MMOQuestManager::UpdateClientVariable(UINT16 index, INT val, INT classVal)
{
	MMOQuestData::getSingleton().UpdateClientVariable(index, val, classVal);
}

void MMOQuestManager::UpdateQuestNpcKillCount(INT questId, UINT16 index, INT value)
{
	MMOQuestData::getSingleton().UpdateQuestNpcKillCount(questId, index, value);
	UpdateQuestTargets(questId);

	// tips
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	const AcceptedQuestData* acceptedQuest = MMOQuestData::getSingleton().GetAcceptedQuestData(questId);
	if (quest && acceptedQuest && index < quest->CheckKillNpc.Num())
	{
		const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(index);
		if (killNpc)
		{
			if (killNpc->CheckKillNpcNewId > 0)
			{
				UNPCTemplate* npc = ((UQuest*)quest)->GetCheckKillNpc(index);
				if (npc)
				{
					FString monsterName = npc->GetDisplayName();
					FString desc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.Report1")),
						*monsterName, acceptedQuest->KillNpcCount[index], killNpc->CheckKillNpcCnt);
					MMOUI::GetUIManager()->delegateAddScreenMessage(desc, MsgInfoType_Table, TEXT("MMOGame.Quest.Report1"));
				}
			}
			else if (killNpc->CheckKillNpcGroupId > 0)
			{
				FString desc = FString::Printf(TEXT("%s: %d/%d"),
					*(killNpc->Description), acceptedQuest->KillNpcCount[index], killNpc->CheckKillNpcCnt);
				MMOUI::GetUIManager()->delegateAddScreenMessage(desc, MsgInfoType_Table, TEXT("MMOGame.Quest.Report1"));
			}
		}
	}
}

void MMOQuestManager::UpdateQuestDoodadKillCount(INT questId, UINT16 index, INT val)
{
	UpdateQuestTargets(questId);

	// tips
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	AcceptedQuestDataMap::iterator iter = acceptedQuests.find(questId);
	if (iter != acceptedQuests.end())
	{
		AcceptedQuestData& data = iter->second;
		if (index < MAX_CONDITION_PER_QUEST)
		{
			const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
			if (quest && index < quest->CheckKillDoodad.Num())
			{
				const FQuestCheckKillDoodad* checkKillDoodad = &quest->CheckKillDoodad(index);			
				if (checkKillDoodad && checkKillDoodad->CheckKillDoodadID > 0)
				{
					UDoodad* doodad = DoodadManager::getSingleton().FindDoodadTemplate(checkKillDoodad->CheckKillDoodadID);
					if (doodad)
					{
						INT killDoodadCount = data.KillDoodadCount[index];
						if (killDoodadCount > 0)
						{
							killDoodadCount = Min(killDoodadCount, checkKillDoodad->CheckKillDoodadCnt);
						}

						// condition met, update quest state
						if (killDoodadCount >= checkKillDoodad->CheckKillDoodadCnt)
						{
							UpdateQuestStatus();
						}

						FString doodadName = doodad->DoodadName.ToString();
						FString desc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.Report1")),
							*doodadName, data.KillDoodadCount[index], checkKillDoodad->CheckKillDoodadCnt);
						MMOUI::GetUIManager()->delegateAddScreenMessage(desc,MsgInfoType_Table,TEXT("MMOGame.Quest.Report1"));
					}
				}
			}
		}
	}
}

void MMOQuestManager::UpdateQuestPlayerKillCount(INT questId, UINT16 index, INT val)
{
	MMOQuestData::getSingleton().UpdateQuestPlayerKillCount(questId, index, val);
	UpdateQuestTargets(questId);
	UpdateQuestStatus();
}

void MMOQuestManager::UpdateQuestReachLevel(INT curLevel)
{
	MMOQuestData::getSingleton().UpdateQuestReachLevel(curLevel);
}

bool MMOQuestManager::AcceptQuest(INT questId, UBOOL share, INT shareSrcId) const
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return false;
	}

	// emergency quests cannot be accepted from here
	if (quest->QuestType == E_qt_Emegency)
	{
		return false;
	}

	if (MMOQuestData::getSingleton().IsQuestAccepted(questId))
	{
		return false;
	}

	if (!MMOQuestData::getSingleton().IsQuestAvailable(questId))
	{
		return false;
	}

	CMessageHandler::GetInstance().Send_AcceptQuest(questId, share ? qatShare : qatNpc, shareSrcId);
	return true;
}

bool MMOQuestManager::CompleteQuest(INT questId, INT selectedAwardIndex)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return false;
	}
	
	if (MMOQuestData::getSingleton().IsQuestCompleted(questId))
	{
		return false;
	}

	if (!MMOQuestData::getSingleton().IsQuestCompassable(questId))
	{
		return false;
	}
	
	// for quests completed directly without a popup, the character cannot choose optional rewards, so find the default here
	if (selectedAwardIndex == -1)
	{
		selectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(questId);
	}

	CMessageHandler::GetInstance().Send_CompleteQuest(questId, selectedAwardIndex);
	return true;
}

void MMOQuestManager::InitRewardQuestList()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UActorAttribute* attri = mainPlayer->ActorAttributes;
	if (attri && attri->LevelAttributes.Level >= GENERATE_REWARD_QUEST_LEVEL)
	{
		CMessageHandler::GetInstance().Send_RefreshRewardQuestList(FALSE);
	}
}

void MMOQuestManager::RefreshRewardQuestList(UBOOL autoRefresh)
{
	_IsAutoRefresh = autoRefresh;
	CMessageHandler::GetInstance().Send_RefreshRewardQuestList(TRUE);
}

void MMOQuestManager::SetRewardQuestRefreshTime(INT time)
{
	_RewardQuestBrushLeftTime = time;
}

UBOOL MMOQuestManager::FlyToDestination(INT questId, INT destId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData((INT)questId);
	if (!quest)
	{
		return FALSE;
	}

	if (!MMOQuestData::getSingleton().IsQuestAccepted(questId))
	{
		return FALSE;
	}

	if (destId < 0 || destId > 7)
	{
		return FALSE;
	}

	_CurFlyQuestId = questId;
	_CurFlyDestId = destId;

	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoesCost")), 
		*UITextParser::HTMLMoneyIconString(MONEYTYPE_02, FLY_TO_QUEST_ZONE_COST));
	if (_LastFlyShoesUseTimes > 0)
	{
		text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoesConfirm"));
	}

	MMOMsgBoxManager::getSingleton().AddMessageBoxEvent(text, 0, GameEvent_UseFlyItem);

	return TRUE;
}

void MMOQuestManager::PlayQuestSound(INT questId, ANPC* npc)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	// disable quest sound effects while cutscene is playing
	if (mainPlayer->IsInMatinee)
	{
		return;
	}

	// per request #5998, SoundCue configured in Quest takes priority over the generic SoundCue configured on NPC
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
	UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
	UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
	UBOOL isQuestCompleted = MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID());

	if (isQuestAvailable)
	{
		USoundCue* questSoundAccept = ((UQuest*)quest)->GetQuestSoundAccept();
		if (questSoundAccept)
		{
			//quest->QuestSoundAccept->SoundClass = FName(TEXT("None"));
			MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
			MMOFacade::getSingleton().GetMainActor()->PlaySound(questSoundAccept);
		}
		else if (npc && npc->Template && npc->Template->AvatarTemplate && npc->Template->AvatarTemplate->QuestSoundAccept)
		{
			MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
			npc->PlaySound(npc->Template->AvatarTemplate->QuestSoundAccept);
		}
	}
	else if (isQuestAccepted && !isQuestCompassable)
	{
		USoundCue* questSoundUnderway = ((UQuest*)quest)->GetQuestSoundUnderway();
		if (questSoundUnderway)
		{
			MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
			MMOFacade::getSingleton().GetMainActor()->PlaySound(questSoundUnderway);
		}
		else if (npc && npc->Template && npc->Template->AvatarTemplate && npc->Template->AvatarTemplate->QuestSoundUnderway)
		{
			MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
			npc->PlaySound(npc->Template->AvatarTemplate->QuestSoundUnderway);
		}
	}
	else if (isQuestAccepted && isQuestCompassable)
	{
		USoundCue* questSoundSubmit = ((UQuest*)quest)->GetQuestSoundSubmit();
		if (questSoundSubmit)
		{
			MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
			MMOFacade::getSingleton().GetMainActor()->PlaySound(questSoundSubmit);
		}
		else if (npc && npc->Template && npc->Template->AvatarTemplate && npc->Template->AvatarTemplate->QuestSoundSubmit)
		{
			MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
			npc->PlaySound(npc->Template->AvatarTemplate->QuestSoundSubmit);
		}
	}
}

#define CHECK_TIME_INIT() \
	DOUBLE StartTime = 0.0; \
	DOUBLE CurrentTime = 0.0; \
	DOUBLE DeltaTime = 0.0;

#define CHECK_TIME_BEGIN() StartTime = appSeconds();

#define CHECK_TIME_END(strTag) \
	CurrentTime = appSeconds(); \
	DeltaTime = CurrentTime - StartTime; \
	debugf(TEXT("-----------------CheckTime:%5.2f (s) %s"), DeltaTime, *strTag);

void MMOQuestManager::NotifyAcceptQuest(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	MMOQuestData::getSingleton().AcceptQuest(questId);
	UpdateQuestStatus();

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		// if it is a main quest
		if (quest->QuestType == E_qt_Trunk)
		{
			_CurTrunkQuestId = questId;
		}
		else
		{
			_CurOtherQuestId = questId;
		}

		// if it is an emergency quest
		if (quest->QuestType == E_qt_Emegency)
		{
			UMMOUINewEventPanel* newEventPanel = (UMMOUINewEventPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_NewEvent);
			if (newEventPanel)
			{
				newEventPanel->ShowNewEvent((INT)quest->EmergencyTipsType, TEXT(""));
			}

			FString emergencyStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ChatMsgEmergencyCompleteQuest"));
			emergencyStr += FString::Printf(TEXT("%d/%d"), MMOQuestData::getSingleton().GetEmergencyQuestCompleteCount(), MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY);
			MMOUI::GetUIManager()->delegateAddScreenMessage(emergencyStr, MsgInfoType_Table, TEXT("MMOGame.Quest.ChatMsgEmergencyCompleteQuest"));

			MMOGameEvent_Guide evtGuide(EGUET_Emergency);
			MMOUI::PostGameEvent(&evtGuide);
		}

		// if it is a bounty quest
		if (quest->QuestType == E_qt_Reward)
		{
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateRewardQuestLimit);
			MMOUI::PostGameEvent(&evt);
		}

		// if it is a circle quest
		FString acceptStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ChatMsgRecieveQuest"));
		acceptStr += quest->QuestName;
		if (quest->CircleID > 0)
		{
			CircleQuestInfo info;
			if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
			{
				// ring index starts from 1 #19224
				acceptStr += FString::Printf(TEXT("(%02d)"), info.CircleIndex + 1);
			}
		}

		// whether to show dialogue after accepting
		if (quest->StartOverWithDialog)
		{
			UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
			if (dialog)
			{
				dialog->TryPopupQuestOver(questId);
			}
		}

		// whether to show narration after accepting
		USoundCue* questSoundAccepted = ((UQuest*)quest)->GetQuestSoundAccepted();
		if (questSoundAccepted)
		{
			UMMOUIHelperPlayerPanel* panel = (UMMOUIHelperPlayerPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_HelperPlayer);
			if (panel)
			{
				UTexture2D* questSoundAcceptedIcon = ((UQuest*)quest)->GetQuestSoundAcceptedIcon();
				panel->ShowPlayerPanel(questSoundAcceptedIcon == NULL ? TEXT("") : questSoundAcceptedIcon->GetPathName(), questSoundAccepted);
			}
		}

		MMOChatManager::getSingleton().AddChatMessage(ECC_System, acceptStr, TEXT(""));
		MMOUI::GetUIManager()->delegateAddScreenMessage(acceptStr, MsgInfoType_Table, TEXT("MMOGame.Quest.ChatMsgRecieveQuest"));
		MMOFacade::getSingleton().PlayUISound(TEXT("iQuestActivate"));
	}

	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_AcceptQuest, questId);
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_Guide evtGuide(EGUET_AcceptQuest, questId);
	MMOUI::PostGameEvent(&evtGuide);
}

void MMOQuestManager::NotifyAbandonQuest(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	MMOQuestData::getSingleton().AbandonQuest(questId);
	UpdateQuestStatus();

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		if (quest->QuestType != E_qt_Trunk)
		{
			_CurOtherQuestId = INVALID_QUEST_ID;
		}

		if (quest->QuestType == E_qt_Reward)
		{
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateRewardQuestLimit);
			MMOUI::PostGameEvent(&evt);
		}

		if (quest->QuestType != E_qt_Emegency)
		{// emergency quest auto-abandoned due to timeout, no prompt
			FString acceptStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ChatMsgAbandonQuest"));
			acceptStr += quest->QuestName;
			MMOChatManager::getSingleton().AddChatMessage(ECC_System, acceptStr, TEXT(""));
			MMOUI::GetUIManager()->delegateAddScreenMessage(acceptStr, MsgInfoType_Table, TEXT("MMOGame.Quest.ChatMsgAbandonQuest"));
			MMOFacade::getSingleton().PlayUISound(TEXT("igQuestFailed"));
		}
	}

	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_AbandonQuest, questId);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::NotifyCompleteQuest(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	MMOQuestData::getSingleton().CompleteQuest(questId);
	UpdateQuestStatus();

	// auto-accept next
	if (quest->AutoStartNextID)
	{
		// the next quest to be auto-accepted must not be configured with an accept NPC, otherwise the server reports "npc too far")
		const UQuest* nextQuest = MMOQuestData::getSingleton().GetQuestData(quest->AutoStartNextID);
		if (nextQuest && nextQuest->StartNPCNewId == 0)
		{
			AcceptQuest(quest->AutoStartNextID, FALSE, 0);
		}
	}

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		// if it is a normal quest
		if (quest->QuestType != E_qt_Trunk)
		{
			_CurOtherQuestId = INVALID_QUEST_ID;
		}

		// if it is an emergency quest
		if (quest->QuestType == E_qt_Emegency)
		{
			UMMOUINewEventPanel* newEventPanel = (UMMOUINewEventPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_NewEvent);
			if (newEventPanel)
			{
				newEventPanel->ShowNewEvent((INT)quest->EmergencyTipsType, quest->QuestName);
			}

			FString emergencyStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ChatMsgEmergencyCompleteQuest"));
			emergencyStr += FString::Printf(TEXT("%d/%d"), MMOQuestData::getSingleton().GetEmergencyQuestCompleteCount(), MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY);
			MMOUI::GetUIManager()->delegateAddScreenMessage(emergencyStr, MsgInfoType_Table, TEXT("MMOGame.Quest.ChatMsgEmergencyCompleteQuest"));

			// fix bug:#25370
			// when the number of emergency quests completed today reaches 20, clear all emergency quests accepted by players
			if (MMOQuestData::getSingleton().IsEmegencyQuestAllCompleteToday())
			{
				MMOQuestData::getSingleton().AbandonAllEmergencyQuest();
			}
		}

		if (quest->QuestType == E_qt_Reward)
		{
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateRewardQuestLimit);
			MMOUI::PostGameEvent(&evt);
		}

		// whether to show dialogue after completion
		if (quest->FinishOverWithDialog)
		{
			UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
			if (dialog)
			{
				dialog->TryPopupQuestOver(questId);
			}
		}

		// whether to show narration after accepting
		USoundCue* questSoundComplete = ((UQuest*)quest)->GetQuestSoundComplete();
		if (questSoundComplete)
		{
			UMMOUIHelperPlayerPanel* panel = (UMMOUIHelperPlayerPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_HelperPlayer);
			if (panel)
			{
				UTexture2D* questSoundCompleteIcon = ((UQuest*)quest)->GetQuestSoundCompleteIcon();
				panel->ShowPlayerPanel(questSoundCompleteIcon == NULL ? TEXT("") : questSoundCompleteIcon->GetPathName(), questSoundComplete);
			}
		}

		FString completeStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ChatMsgCompleteQuest"));
		completeStr += quest->QuestName;
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, completeStr, TEXT(""));
		MMOUI::GetUIManager()->delegateAddScreenMessage(completeStr,MsgInfoType_Table,TEXT("MMOGame.Quest.ChatMsgCompleteQuest"));
		MMOFacade::getSingleton().PlayUISound(TEXT("iQuestComplete"));
	}

	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_CompleteQuest, questId);
	MMOUI::PostGameEvent(&evt);

	// Achievement
	MMOGameEvent_Achievement evtAch1(adCompleteQuest, questId, 0, 0);
	MMOUI::PostGameEvent(&evtAch1);
	MMOGameEvent_Achievement evtAch2(adCompleteQuestCount, MMOQuestData::getSingleton().GetCompletedQuestCount(), 0, 0);
	MMOUI::PostGameEvent(&evtAch2);

	// Guide
	MMOGameEvent_Guide evtGuide(EGUET_CompleteQuest, questId);
	MMOUI::PostGameEvent(&evtGuide);
}

void MMOQuestManager::NotifyQuestFailed(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	MMOQuestData::getSingleton().SetQuestFailed(questId);
	UpdateQuestStatus();

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		// if it is a normal quest
		if (quest->QuestType != E_qt_Trunk)
		{
			_CurOtherQuestId = INVALID_QUEST_ID;
		}

		FString questStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ChatMsgQuestFailed"));
		questStr = quest->QuestName + questStr;
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, questStr, TEXT(""));
		MMOUI::GetUIManager()->delegateAddScreenMessage(questStr, MsgInfoType_Table, TEXT("MMOGame.Quest.ChatMsgQuestFailed"));
		MMOFacade::getSingleton().PlayUISound(TEXT("igQuestFailed"));
	}
}

void MMOQuestManager::NotifyUpdateQuestList(ROLE_DB_QUEST_DATA* questData)
{
	MMOQuestData::getSingleton().UpdateQuestList(questData);

	// fix bug #23546
	// at this moment, if any quest is in submittable state and auto-submit, CompleteQuest sent to the server gets no response
	// probably because the character has not entered the scene yet
	// move it to GameEvent_EnterScene in MMOQuestManager::OnNativeGameEvent for handling
	//UpdateAllQuestTargets();
	//UpdateQuestStatus();
}

void MMOQuestManager::NotifyShareQuest(INT questId, INT shareSrcId)
{
	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_ShareQuest, questId, shareSrcId);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::NotifyNextCircleQuest(INT questId, INT circleId)
{
	MMOQuestData::getSingleton().SetNextCircleQuest(questId, circleId);
}

void MMOQuestManager::NotifyUpgradeRewardQuestLimit(INT limit)
{
	MMOQuestData::getSingleton().SetRewardQuestCostCount(limit);
	MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_UpdateRewardQuestLimit);
	MMOUI::PostGameEvent(&evt);
}

void MMOQuestManager::NotifyFlyShoesUseTime(INT times)
{
	_LastFlyShoesUseTimes = times;
}

void MMOQuestManager::NotifyRefreshRewardQuestList(const INT32 (&rewardQuestId)[MAX_REWARD_QUEST_COUNT_PER_ROLE], UBOOL isClear)
{
	MMOQuestData::getSingleton().UpdateRewardQuestList(rewardQuestId);

	_HasRewardQuest = MMOQuestData::getSingleton().GetRewardQuestList().size() > 0;
	if (isClear)
	{
		_RewardQuestBrushLeftTime = REWARD_QUEST_BRUSH_CD_INTERVAL;
	}

	// whether to refresh until three stars or above (#9924)
	if (_IsAutoRefresh)
	{
		if (MMOQuestData::getSingleton().GetAcceptedRewardQuestCount() < MAX_REWARD_QUEST_COUNT_PER_ROLE)
		{
			if (MMOQuestData::getSingleton().GetTopLevelRewardQuestCount() > 0)
			{
				_IsAutoRefresh = FALSE;
			}
			else
			{
				AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
				if (mainPlayer && mainPlayer->GetMoney(MONEYTYPE_02) < REWARD_QUEST_BRUSH_COST)
				{
					// stop refreshing when out of money
					_IsAutoRefresh = FALSE;
				}
				else
				{
					RefreshRewardQuestList(TRUE);
				}
			}
		}
	}

	MMOGameEvent_Quest evt1(MMOGameEvent_Quest::EQET_UpdateRewardQuestList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Quest evt2(MMOGameEvent_Quest::EQET_UpdateRewardQuestBrushTime);
	MMOUI::PostGameEvent(&evt2);
}

void MMOQuestManager::NotifyPlayerLevelChange(INT curLevel)
{
	UpdateQuestStatus();

	if (curLevel >= GENERATE_CIRCLE_QUEST_LEVEL)
	{
		if (!MMOQuestData::getSingleton().HasCircleQuest())
		{
			CMessageHandler::GetInstance().Send_GenerateCircleQuestList();
		}
	}

	if (curLevel >= GENERATE_REWARD_QUEST_LEVEL)
	{
		CMessageHandler::GetInstance().Send_RefreshRewardQuestList(FALSE);
	}

	QuestDataList questList = MMOQuestData::getSingleton().GetLevelTriggerQuestList();
	QuestDataList::iterator iter = questList.begin();
	for (; iter != questList.end(); ++ iter)
	{
		UQuest* quest = *iter;
		if (quest && curLevel >= quest->QuestLevel)
		{
			AcceptQuest(quest->QuestTempID, FALSE, 0);
		}
	}

	UpdateQuestReachLevel(curLevel);
}

// UI
//////////////////////////////////////////////////////////////////////////
void MMOQuestManager::_SetAcceptedQuestTraceListItemCaption(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}
	
	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	// quest title
	FString fontColor;
	GConfig->GetString(TEXT("TextColor"), TEXT("QuestCaption"), fontColor, GUIColorIni);

	// quest level
	FString questLevel = UITextParser::HTMLBeginFont(fontColor) 
		+ appItoa(quest->QuestLevel)
		+ FString(TEXT(" Lv"))
		+ UITextParser::HTMLEndFont();

	FString questTitle = LinkManager::HtmlLink_QuestTraceTitle(quest);
	// if it is a circle quest
	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
		{
			// ring index starts from 1 #19224
			questTitle += FString::Printf(TEXT("(%02d)"), info.CircleIndex + 1);
		}
	}
	fontColor = TEXT("#e5a122");
	FString questCaption = TEXT("");
	questCaption += UITextParser::HTMLBeginFont(fontColor) 
		+ questTitle
		+ UITextParser::HTMLEndFont();

	// completion status
	GConfig->GetString(TEXT("TextColor"), TEXT("QuestCondComplete"), fontColor, GUIColorIni);
	UBOOL hasConditions = MMOQuestData::getSingleton().HasConditions(quest->GetTemplateID());
	UBOOL isCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
	if (hasConditions)
	{
		if (isCompassable)
		{
			if (!quest->IsTimeLimit())
			{
				// timed quests only count as complete on delivery, do not show (completed) here as it would be misleading, see Bug6111
				questCaption += UITextParser::HTMLBeginFont(fontColor) 
					+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CondCompleted")) 
					+ UITextParser::HTMLEndFont();
			}

			// manual submit button
			object->SetBool(TEXT("quest_is_delay_complete"), quest->FinishType == ESQT_Manual);
		}
		else if (acceptedQuest->IsFailed)
		{
			fontColor = TEXT("#FF0000");
			questCaption += UITextParser::HTMLBeginFont(fontColor)
				+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.QuestFailed"))
				+ UITextParser::HTMLEndFont();
		}
	}
	else
	{
		questCaption += UITextParser::HTMLBeginFont(fontColor) 
			+ MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CondCompleted")) 
			+ UITextParser::HTMLEndFont();

		// manual submit button
		object->SetBool(TEXT("quest_is_delay_complete"), quest->FinishType == ESQT_Manual);
	}
	object->SetString(TEXT("quest_caption"), questCaption);
	object->SetString(TEXT("quest_level"), questLevel);

	// new quest marker, shown only once
	object->SetBool(TEXT("quest_is_new"), acceptedQuest->IsNewQuest);
	acceptedQuest->IsNewQuest = false;

	object->SetBool(TEXT("quest_is_compassable"), isCompassable);
	object->SetInt(TEXT("quest_id"), quest->GetTemplateID());
	object->SetInt(TEXT("quest_type"), quest->QuestType);
	// quest items
	object->SetInt(TEXT("quest_item_id"), 0);
	object->SetPureString(TEXT("quest_item_icon"), TEXT(""));
	UItem* finishItem = ((UQuest*)quest)->GetFinishItem();
	if (finishItem)
	{
		object->SetInt(TEXT("quest_item_id"), finishItem->GetTemplateID());
		object->SetPureString(TEXT("quest_item_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized(finishItem));
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemDesc(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	UBOOL IsCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
	if (IsCompassable)
	{
		if (!quest->QuestCompletedDescript.IsEmpty())
		{
			// quest description
			UITextParser::getSingleton().Parse(parseContext, dlgContext, quest->QuestCompletedDescript);
			dlgContext.TempText.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
			object->SetPureString(TEXT("quest_detail"), dlgContext.TempText);
		}
	} 
	else
	{
		if (!quest->QuestDescript.IsEmpty())
		{
			// quest description
			UITextParser::getSingleton().Parse(parseContext, dlgContext, quest->QuestDescript);
			dlgContext.TempText.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
			object->SetPureString(TEXT("quest_detail"), dlgContext.TempText);
		}
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckTalkNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	if (quest->TalkNpcID == 0)
	{
		return;
	}

	const UNPCTemplate* npcTemp = MMOFacade::getSingleton().FindNPCTemplate(quest->TalkNpcID);
	if (!npcTemp)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	INT mapId = quest->TalkNpcMap;
	FString npcName = LinkManager::HtmlLink_NPCNav(npcTemp->GetDisplayName(), npcTemp->GetTemplateID(), mapId, FALSE);
	npcName = FString::Printf(TEXT("<font color='#FFF468'>%s</font>"), *npcName);

	targetDesc = TEXT("");
	targetDesc += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.TalkToNPC")), *npcName);

	if (quest->TalkNpcFly)
	{
		targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
		destId ++;
	}

	UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
	if (object)
	{
		object->SetPureString(TEXT("quest_target_desc"), targetDesc);
		object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_Null);
		object->SetBool(TEXT("quest_target_complete"), FALSE);
		objectArray->SetElementObject(index, object);
		index ++;
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckLevel(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	if (quest->CheckLevel <= 0)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UActorAttribute* attri = mainPlayer->ActorAttributes;
	if (!attri)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc = TEXT("");

	if (attri->LevelAttributes.Level < quest->CheckLevel)
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.LevelLimitWithColor")),
			*_CondNotCompleteColor, quest->CheckLevel);
	} 
	else
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.LevelLimitWithColor")),
			*_CondCompleteColor, quest->CheckLevel);
		targetDesc += _CondComplete;
	}

	UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
	if (object)
	{
		object->SetPureString(TEXT("quest_target_desc"), targetDesc);
		object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_Null);
		object->SetBool(TEXT("quest_target_complete"), attri->LevelAttributes.Level >= quest->CheckLevel);
		objectArray->SetElementObject(index, object);
		index ++;
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckVariable(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckVar.Num(); ++ i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType == TEXT("vtQuest") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_VARIABLE_PER_QUEST)
		{
			targetDesc = TEXT("");
			if(acceptedQuest->QuestVariable[checkVar.CheckVarID] != checkVar.CheckVarCnt)
			{
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().Parse(parseContext, dlgContext, checkVar.Description);
				dlgContext.TempText.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
				targetDesc = dlgContext.TempText;
			}
			else
			{
				// condition met, remove underline
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
				targetDesc += _CondComplete;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);
				object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_Null);
				object->SetBool(TEXT("quest_target_complete"), acceptedQuest->QuestVariable[checkVar.CheckVarID] == checkVar.CheckVarCnt);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
		else if (checkVar.CheckVarType == TEXT("vtClient") &&
			checkVar.CheckVarID == ECVT_Quest)
		{
			// whether class-specific description exists
			for (int j = 0; j < quest->CheckClassVar.Num(); ++ j)
			{
				FQuestCheckClassVar checkClassVar = quest->CheckClassVar(j);
				INT classId = MMOFacade::getSingleton().GetMainActor()->GetClassId();
				if (classId == j && checkClassVar.CheckClassVarCnt > 0)
				{
					checkVar.Description = checkClassVar.CheckClassVarDesc;
					break;
				}
			}

			targetDesc = TEXT("");
			if((acceptedQuest->ClientVariable[checkVar.CheckVarID] & (INT32)(1 << checkVar.CheckVarCnt)) > 0)
			{
				// condition met, remove underline
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
				targetDesc += _CondComplete;
			}
			else
			{
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().Parse(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);
				object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_Null);
				object->SetBool(TEXT("quest_target_complete"), (acceptedQuest->ClientVariable[checkVar.CheckVarID] & (INT32)(1 << checkVar.CheckVarCnt)) > 0);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
		else if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT)
		{
			targetDesc = TEXT("");
			if (acceptedQuest->RoleVariable[checkVar.CheckVarID] != checkVar.CheckVarCnt)
			{
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().Parse(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
			}
			else
			{
				// condition met, remove underline
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
				targetDesc += _CondComplete;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);
				object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_Null);
				object->SetBool(TEXT("quest_target_complete"), acceptedQuest->RoleVariable[checkVar.CheckVarID] == checkVar.CheckVarCnt);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckItem(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckItem.Num(); ++ i)
	{
		const FQuestCheckItem* checkItem = &quest->CheckItem(i);	
		UItem* item = ((UQuest*)quest)->GetCheckItem(i);					
		if (item)
		{
			INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(item->GetTemplateID(), E_CONSLOT_DEFAULT);
			if (itemCount > 0)
			{
				itemCount = Min(itemCount, checkItem->CheckItemCnt);
			}

			FString itemName;
			if (itemCount < checkItem->CheckItemCnt)
			{
				if (checkItem->Description.IsEmpty())
				{
					itemName = LinkManager::HtmlLink_ItemNav(item->GetDisplayName(), item->GetTemplateID(), 
						checkItem->CheckItemNav, checkItem->CheckItemMap, FALSE);
					itemName = FString::Printf(TEXT("<font color='#FFF468'>%s</font>"), *itemName);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, checkItem->Description);
					itemName = dlgContext.TempText;
				}

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*itemName, *_CondNotCompleteColor, itemCount, checkItem->CheckItemCnt);
				targetDesc = FString::Printf(TEXT("%s"), *itemName);
			}
			else
			{
				// condition met, remove underline
				if (checkItem->Description.IsEmpty())
				{
					itemName = item->GetDisplayName();
					itemName = FString::Printf(TEXT("<font color='#FFF468'>%s</font>"), *itemName);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkItem->Description);
					itemName = dlgContext.TempText;
				}

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*itemName, *_CondCompleteColor, itemCount, checkItem->CheckItemCnt);
				targetDesc = FString::Printf(TEXT("%s"), *itemName);

				targetDesc += _CondComplete;
			}

			if (checkItem->CheckItemFly)
			{
				targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));
			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object && checkItem->CheckItemCnt > 0)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);
				object->SetInt(TEXT("quest_target_progress_type"), checkItem->CheckItemCnt == 1 ? EQTTPT_Null : EQTTPT_CountProgress);
				object->SetInt(TEXT("quest_target_progress_cur"), itemCount);
				object->SetInt(TEXT("quest_target_progress_max"), checkItem->CheckItemCnt);
				object->SetBool(TEXT("quest_target_complete"), itemCount >= checkItem->CheckItemCnt);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckKillNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckKillNpc.Num(); ++ i)
	{
		const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(i);
		UNPCTemplate* npc = ((UQuest*)quest)->GetCheckKillNpc(i);	
		if (npc || (killNpc && killNpc->CheckKillNpcGroupId > 0))
		{		
			FString monsterColor = _CondNotCompleteColor;
			INT killCount = acceptedQuest->KillNpcCount[i];
			if(killCount > 0)
			{
				killCount = Min(killCount, killNpc->CheckKillNpcCnt);
			}

			FString monsterName;
			if (killCount < killNpc->CheckKillNpcCnt)
			{
				if (killNpc->Description.IsEmpty())
				{
					FString name = (npc != NULL) ? npc->GetDisplayName() : TEXT("");
					monsterName = LinkManager::HtmlLink_NavPointName(name, killNpc->CheckKillNpcNav, killNpc->CheckKillNpcMap, FALSE);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, killNpc->Description);
					monsterName = dlgContext.TempText;
				}

				monsterName.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*monsterName, *_CondNotCompleteColor, acceptQuest->KillNpcCount[i], checkMonster->CheckKillNpcCnt);	
				targetDesc = FString::Printf(TEXT("%s"), *monsterName);
			}
			else
			{
				// condition met, remove underline
				if (killNpc->Description.IsEmpty())
				{
					monsterName = (npc != NULL) ? npc->GetDisplayName() : TEXT("");
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, killNpc->Description);
					monsterName = dlgContext.TempText;
				}

				monsterName.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*monsterName, *_CondCompleteColor, acceptQuest->KillNpcCount[i], checkMonster->CheckKillNpcCnt);
				targetDesc = FString::Printf(TEXT("%s"), *monsterName);

				targetDesc += _CondComplete;
			}

			if (killNpc->CheckKillNpcFly)
			{
				targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object && killNpc->CheckKillNpcCnt > 0)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);

				int curProgress = 0;
				int maxProgress = 0;
				if (npc && MMOQuestData::getSingleton().IsSingleMonsterQuest(quest->GetTemplateID()))
				{
					_GetMonsterHp(npc->TemplateID, curProgress, maxProgress);
					object->SetInt(TEXT("quest_target_progress_type"), curProgress == 0 ? EQTTPT_Null : EQTTPT_HpProgress);
				} 
				else
				{
					curProgress = acceptedQuest->KillNpcCount[i];
					maxProgress = killNpc->CheckKillNpcCnt;
					object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_CountProgress);
				}
				object->SetInt(TEXT("quest_target_progress_cur"), curProgress);
				object->SetInt(TEXT("quest_target_progress_max"), maxProgress);
				object->SetBool(TEXT("quest_target_complete"), killCount >= killNpc->CheckKillNpcCnt);

				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckKillDoodad(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckKillDoodad.Num(); ++i)
	{
		const FQuestCheckKillDoodad* checkKillDoodad = &quest->CheckKillDoodad(i);
		if (checkKillDoodad->CheckKillDoodadID)
		{		
			FString doodadName = TEXT("");
			class UDoodad* doodad = DoodadManager::getSingleton().FindDoodadTemplate(checkKillDoodad->CheckKillDoodadID);
			if (doodad)
			{
				doodadName = doodad->DoodadName.ToString();
			}

			INT killCount = acceptedQuest->KillDoodadCount[i];
			if (killCount > 0)
			{
				killCount = Min(killCount, checkKillDoodad->CheckKillDoodadCnt);
			}

			FString doodadDesc;
			if (killCount < checkKillDoodad->CheckKillDoodadCnt)
			{
				if (checkKillDoodad->CheckKillDoodadDesc.IsEmpty())
				{
					doodadDesc = LinkManager::HtmlLink_DoodadNav(doodadName,
						checkKillDoodad->CheckKillDoodadID, 
						checkKillDoodad->CheckKillDoodadNav,
						checkKillDoodad->CheckKillDoodadMap,
						FALSE);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, checkKillDoodad->CheckKillDoodadDesc);
					doodadDesc = dlgContext.TempText;
				}

				doodadDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*doodadDesc, *_CondNotCompleteColor, acceptQuest->KillDoodadCount[i], checkKillDoodad->CheckKillDoodadCnt);	
				targetDesc = FString::Printf(TEXT("%s"), *doodadDesc);
			}
			else
			{
				// condition met, remove underline
				if (checkKillDoodad->CheckKillDoodadDesc.IsEmpty())
				{
					doodadDesc = doodadName;
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkKillDoodad->CheckKillDoodadDesc);
					doodadDesc = dlgContext.TempText;
				}

				doodadDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*doodadDesc, *_CondCompleteColor, acceptQuest->KillDoodadCount[i], checkKillDoodad->CheckKillDoodadCnt);
				targetDesc = FString::Printf(TEXT("%s"), *doodadDesc);

				targetDesc += _CondComplete;
			}

			if (checkKillDoodad->CheckKillDoodadFly)
			{
				targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object && checkKillDoodad->CheckKillDoodadCnt > 1)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);
				object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_CountProgress);
				object->SetInt(TEXT("quest_target_progress_cur"), acceptedQuest->KillDoodadCount[i]);
				object->SetInt(TEXT("quest_target_progress_max"), checkKillDoodad->CheckKillDoodadCnt);
				object->SetBool(TEXT("quest_target_complete"), killCount >= checkKillDoodad->CheckKillDoodadCnt);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckKillPlayer(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckKillPlayer.Num(); ++ i)
	{
		const FQuestCheckKillPlayer* checkKillPlayer = &quest->CheckKillPlayer(i);
		if (checkKillPlayer->CheckKillPlayerCnt > 0)
		{
			INT killCount = acceptedQuest->KillPlayerCount[i];
			if(killCount > 0)
			{
				killCount = Min(killCount, checkKillPlayer->CheckKillPlayerCnt);
			}

			FString playerName;
			if (killCount < checkKillPlayer->CheckKillPlayerCnt)
			{
				if (checkKillPlayer->CheckKillPlayerDesc.IsEmpty())
				{
					//playerName = LinkManager::HtmlLink_NPCNav(npc->GetDisplayName(),
					//	npc->TemplateID, checkKillPlayer->CheckKillNpcMap);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, checkKillPlayer->CheckKillPlayerDesc);
					playerName = dlgContext.TempText;
				}

				playerName.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*playerName, *_CondNotCompleteColor, acceptQuest->KillPlayerCount[i], checkKillPlayer->CheckKillPlayerCnt);	
				targetDesc = FString::Printf(TEXT("%s"), *playerName);
			}
			else
			{
				// condition met, remove underline
				if (checkKillPlayer->CheckKillPlayerDesc.IsEmpty())
				{
					//playerName = checkKillPlayer->CheckKillNpcNew->GetDisplayName();
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkKillPlayer->CheckKillPlayerDesc);
					playerName = dlgContext.TempText;
				}

				playerName.ReplaceInline(TEXT("#1EFF00"), TEXT("#FFF468"));

				// changed to show count in progress bar, so not shown here
				//targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
				//	*playerName, *_CondCompleteColor, acceptQuest->KillPlayerCount[i], checkKillPlayer->CheckKillPlayerCnt);
				targetDesc = FString::Printf(TEXT("%s"), *playerName);
				targetDesc += _CondComplete;
			}

			UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
			if (object && checkKillPlayer->CheckKillPlayerCnt > 1)
			{
				object->SetPureString(TEXT("quest_target_desc"), targetDesc);
				object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_CountProgress);
				object->SetInt(TEXT("quest_target_progress_cur"), acceptedQuest->KillPlayerCount[i]);
				object->SetInt(TEXT("quest_target_progress_max"), checkKillPlayer->CheckKillPlayerCnt);
				object->SetBool(TEXT("quest_target_complete"), killCount >= checkKillPlayer->CheckKillPlayerCnt);
				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTargetCheckReputation(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	if (quest->CheckReputationForceID <= 0)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	FString reputationName;
	FString reputationLevel;
	// get reputation name
	UMMOUICharacter* panel = Cast<UMMOUICharacter>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_Character ) );
	if (panel)
	{
		reputationName = panel->GetReputationForceName(quest->CheckReputationForceID);
		reputationLevel = panel->GetReputationLevelName(quest->CheckReputationForceID,quest->CheckReputation);
	}

	INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), quest->CheckReputationForceID);
	if (reputation < quest->CheckReputation)
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ReputationWithColor")),
			*reputationName,
			*_CondNotCompleteColor, 
			*reputationLevel);
	} 
	else
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ReputationWithColor")),
			*reputationName,
			*_CondCompleteColor, 
			*reputationLevel);
		targetDesc += _CondComplete;
	}

	UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceTargetData"));
	if (object)
	{
		object->SetPureString(TEXT("quest_target_desc"), targetDesc);
		object->SetInt(TEXT("quest_target_progress_type"), EQTTPT_Null);
		object->SetBool(TEXT("quest_target_complete"), reputation >= quest->CheckReputation);
		objectArray->SetElementObject(index, object);
		index ++;
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTarget(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	UBOOL IsCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
	if (IsCompassable)
	{
		return;
	}

	UGFxObject* objectArray = gfxPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int destId = 0;
	if (quest->StartNPCNewId > 0 && quest->StartNpcFly)
	{
		// if the start-quest NPC requires flying shoes
		destId ++;
	}

	int index = 0;
	_SetAcceptedQuestTraceListItemTargetCheckTalkNpc(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckLevel(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckVariable(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckItem(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckKillNpc(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckKillDoodad(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckKillPlayer(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetAcceptedQuestTraceListItemTargetCheckReputation(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);

	object->SetObject(TEXT("quest_targets"), objectArray);
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemTime(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	if (quest->IsTimeLimit() && !acceptedQuest->IsFailed)
	{
		object->SetInt(TEXT("quest_time_limit"), acceptedQuest->LeftTime);
	}
}

void MMOQuestManager::_GetQuestAwardMoneyInfo(INT moneyType, FString& moneyName, FString& moneyIcon)
{
	switch (moneyType)
	{
	case MONEYTYPE_01:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_01"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.MoneyGold_S"); //width='26' height='24'
		break;

	case MONEYTYPE_02:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_02"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.MoneyCash_S");
		break;

	case MONEYTYPE_04:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_04"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.MoneyBullion");
		break;

	case MONEYTYPE_ARENA:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_ARENA"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.ArenaPoint");
		break;

	case MONEYTYPE_BF:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_BF"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.BattleHonor");
		break;

	case MONEYTYPE_MORAL:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_MORAL"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.Faith");
		break;

	case MONEYTYPE_GUILD:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_GUILD"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.GuildHonor");
		break;

	case MONEYTYPE_SOUL_PTS:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_SOUL_PTS"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.Soul");
		break;

	case MONEYTYPE_ACHIEVEMENTPTS:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_ACHIEVEMENTPTS"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.Achievement");
		break;

		// the following three have no resources, temporarily use the family contribution format
	case MONEYTYPE_MASTER_EXP:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_MASTER_EXP"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.GuildHonor");
		break;

	case MONEYTYPE_SPELL_PTS:
		moneyName = MMOUI::ParseLocalizedPropertyPath(TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_SPELL_PTS"));
		moneyIcon = TEXT("img://MMO_UIIcon.MoneyIcon.GuildHonor");
		break;
	}
}

void MMOQuestManager::_SetAcceptedQuestTraceListItemAward(const UQuest* quest, AcceptedQuestData* acceptedQuest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	object->SetString(TEXT("quest_award_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardCaption")));

	// gold, experience, activity
	INT moneyType = quest->AwardCurrencyType;
	INT money = quest->AwardCurrencyAmount;
	INT moneyTypeEx = quest->AwardCurrencyTypeEx;
	INT moneyEx = quest->AwardCurrencyAmountEx;
	INT exp = quest->AwardExp;
	INT activity = quest->AwardActivity;
	
	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				CircleQuestRewardData awardData;
				if (MMOQuestData::getSingleton().GetCircleQuestRewardData(info.CircleId, info.CircleIndex, mainPlayer->ActorAttributes->LevelAttributes.Level, awardData))
				{
					moneyType = awardData.MoneyType;
					money = awardData.Money;
					moneyTypeEx = awardData.MoneyTypeEx;
					moneyEx = awardData.MoneyEx;
					exp = awardData.Exp;
				}
			}
		}
	}

	FString moneyName = TEXT("");
	FString moneyIcon = TEXT("img://MMO_ItemIcon_Supplies.img_Money");
	_GetQuestAwardMoneyInfo(moneyType, moneyName, moneyIcon);
	object->SetString(TEXT("quest_award_money_icon"), moneyIcon);
	object->SetString(TEXT("quest_award_money"), money > 0 ? appItoa(money) : TEXT(""));

	moneyIcon = TEXT("img://MMO_ItemIcon_Supplies.img_Money");
	_GetQuestAwardMoneyInfo(moneyTypeEx, moneyName, moneyIcon);
	object->SetString(TEXT("quest_award_money_icon_ex"), moneyIcon);
	object->SetString(TEXT("quest_award_money_ex"), moneyEx > 0 ? appItoa(moneyEx) : TEXT(""));

	object->SetString(TEXT("quest_award_exp_icon"), TEXT("img://MMO_UIIcon.exp2"));
	object->SetInt(TEXT("quest_award_exp"), exp);

	object->SetString(TEXT("quest_award_activity_icon"), TEXT("img://MMO_ItemIcon_Supplies.item_147"));
	object->SetInt(TEXT("quest_award_activity"), activity);

	// item rewards and optional quest rewards are shown side by side in trace; designers guarantee item rewards plus optional item rewards do not exceed 4
	UGFxObject* awardArray = gfxPlayer->CreateArray();
	if (awardArray)
	{
		int awardIndex = 0;

		QuestAwardItemInfoList awardList;
		MMOQuestData::getSingleton().GetAwardItemList(quest->GetTemplateID(), awardList);
		for(size_t i = 0; i < awardList.size(); ++i)
		{
			const QuestAwardItemInfo& awardInfo = awardList[i];
			UGFxObject* awardIconData = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestAwardIconData"));
			check(awardIconData != NULL);

			awardIconData->SetInt(TEXT("item_template_id"), awardInfo.ItemTemplate->GetTemplateID());
			awardIconData->SetInt(TEXT("item_count"), awardInfo.Count);
			awardIconData->SetInt(TEXT("item_index"), awardInfo.Index);
			awardIconData->SetString(TEXT("icon_img_name"), MMOFacade::getSingleton().GetItemIconNamePersonalized( awardInfo.ItemTemplate ));
			awardIconData->SetString(TEXT("item_name"), awardInfo.ItemTemplate->GetDisplayName());

			awardArray->SetElementObject(awardIndex, awardIconData);	
			awardIndex ++;
		}

		QuestAwardItemInfoList selectableAwardList;
		MMOQuestData::getSingleton().GetSelectableAwardItemList(quest->GetTemplateID(), selectableAwardList);
		for (size_t i = 0; i < selectableAwardList.size(); ++i)
		{
			const QuestAwardItemInfo& awardInfo = selectableAwardList[i];
			UGFxObject* awardIconData = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestAwardIconData"));
			check(awardIconData != NULL);

			awardIconData->SetInt(TEXT("item_template_id"), awardInfo.ItemTemplate->GetTemplateID());
			awardIconData->SetInt(TEXT("item_count"), awardInfo.Count);
			awardIconData->SetInt(TEXT("item_index"), awardInfo.Index);
			awardIconData->SetPureString(TEXT("icon_img_name"), MMOFacade::getSingleton().GetItemIconNamePersonalized( awardInfo.ItemTemplate ));
			awardIconData->SetPureString(TEXT("item_name"), awardInfo.ItemTemplate->GetDisplayName());

			awardArray->SetElementObject(awardIndex, awardIconData);	
			awardIndex ++;		
		}

		object->SetObject(TEXT("quest_awards"), awardArray);
	}
}

void MMOQuestManager::_SetAvailableQuestTraceListItemCaption(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	// quest title
	FString fontColor;
	GConfig->GetString(TEXT("TextColor"), TEXT("QuestCaption"), fontColor, GUIColorIni);

	// quest level
	FString questLevel = UITextParser::HTMLBeginFont(fontColor) 
		+ appItoa(quest->QuestLevel)
		+ FString(TEXT(" Lv"))
		+ UITextParser::HTMLEndFont();

	FString questTitle = LinkManager::HtmlLink_QuestTraceTitle(quest);
	// if it is a circle quest
	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
		{
			// ring index starts from 1 #19224
			questTitle += FString::Printf(TEXT("(%02d)"), info.CircleIndex + 1);
		}
	}
	fontColor = TEXT("#e5a122");
	FString questCaption = TEXT("");
	questCaption += UITextParser::HTMLBeginFont(fontColor) 
		+ questTitle
		+ UITextParser::HTMLEndFont()
		+ UITextParser::HTMLBeginFont(TEXT("#FFF468"))
		+ TEXT("(Available)")
		+ UITextParser::HTMLEndFont();

	object->SetPureString(TEXT("quest_caption"), questCaption);
	object->SetString(TEXT("quest_level"), questLevel);

	object->SetInt(TEXT("quest_id"), quest->GetTemplateID());
	object->SetInt(TEXT("quest_type"), quest->QuestType);
}

void MMOQuestManager::_SetAvailableQuestTraceListItemDesc(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	FString startDesc = TEXT("");
	if (quest->StartType == EAQT_Npc)
	{
		// Quest-giver NPC
		UNPCTemplate* startNpc = ((UQuest*)quest)->GetStartNpc();
		if (startNpc)
		{
			FString npcName = LinkManager::HtmlLink_NPCNav(startNpc->GetDisplayName(), 
				startNpc->TemplateID, 
				quest->StartNPCMap,
				FALSE);

			FString mapName = TEXT("");
			FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(quest->StartNPCMap);
			if (mapInfo)
			{
				mapName = mapInfo->MapDisplayName;
			}

			if (mapName != TEXT(""))
			{
				startDesc = FString::Printf(TEXT("<font color='#FFFFFF'>%s</font> <font color='#FFF468'>%s</font> has something to tell you, go and see them!"), *mapName, *npcName);
			}
			else
			{
				startDesc = FString::Printf(TEXT("<font color='#FFF468'>%s</font> has something to tell you, go and see them!"), *npcName);
			}
			
			int destId = 0;
			if (quest->StartNpcFly)
			{
				startDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}
		}
	}
	else if (quest->StartType == EAQT_Item)
	{
		// accept items
		object->SetInt(TEXT("quest_item_id"), 0);
		object->SetPureString(TEXT("quest_item_icon"), TEXT(""));
		UItem* startItem = ((UQuest*)quest)->GetStartItem();
		if (startItem)
		{
			object->SetInt(TEXT("quest_item_id"), startItem->GetTemplateID());
			object->SetPureString(TEXT("quest_item_icon"), startItem->GetDisplayName());

			FString itemName = LinkManager::HtmlLink_Item(startItem);
			startDesc = FString::Printf(TEXT("<font color='#FFFFFF'>%s</font> <font color='#FFF468'>%s</font>"),
				*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.QuestPublicer")), *itemName);
		}
	}
	else if (quest->StartType == EAQT_Zone)
	{
		ParseContext parseContext;
		DialogContext dlgContext;
		UITextParser::getSingleton().Parse(parseContext, dlgContext, quest->QuestDescript);
		startDesc = FString::Printf(TEXT("<font color='#FFF468'>%s</font>"), *(dlgContext.TempText));
	}
	else if (quest->StartType == EAQT_Level)
	{
		ParseContext parseContext;
		DialogContext dlgContext;
		UITextParser::getSingleton().Parse(parseContext, dlgContext, quest->QuestDescript);
		startDesc = FString::Printf(TEXT("<font color='#FFF468'>%s</font>"), *(dlgContext.TempText));
	}

	object->SetPureString(TEXT("quest_detail"), startDesc);
}

INT MMOQuestManager::GetCurQuestGridQuestId(INT index)
{
	if (index >= 0 && index < MAX_QUEST_PER_ZONE)
	{
		return _CurQuestGridQuestId[index];
	}

	return INVALID_QUEST_ID;
}

UBOOL MMOQuestManager::IsEmergencyQuestBeTraced(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return FALSE;
	}

	if (quest->QuestType != E_qt_Emegency)
	{
		return FALSE;
	}

	for (INT i = 0; i < MAX_QUEST_PER_ZONE; ++i)
	{
		if (questId == _CurQuestGridQuestId[i])
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

UBOOL MMOQuestManager::IsMonsterBeTraced(INT monsterId)
{
	QuestTraceList questList;
	questList.clear();
	if (MMOQuestData::getSingleton().GetNormalQuestListByMonsterBeTraced(monsterId, questList))
	{
		return TRUE;
	}

	questList.clear();
	if (MMOQuestData::getSingleton().GetEmergencyQuestListByMonsterBeTraced(monsterId, questList))
	{
		QuestTraceList::iterator iter = questList.begin();
		for (; iter != questList.end(); ++iter)
		{
			INT questId = *iter;
			for (INT i = 0; i < MAX_QUEST_PER_ZONE; ++i)
			{
				if (questId == _CurQuestGridQuestId[i])
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

UGFxObject* MMOQuestManager::GetQuestTraceData(INT questId, UMMOGFXPlayer* gfxPlayer, EQuestTraceListType type)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return NULL;
	}	

	if (!gfxPlayer)
	{
		return NULL;
	}

	UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestTraceData"));
	if (!object)
	{
		return NULL;
	}

	if (type == EQTLT_Accepted)
	{
		AcceptedQuestData* acceptQuest = MMOQuestData::getSingleton().GetAcceptedQuestData(questId);
		if (!acceptQuest)
		{
			return NULL;
		}

		if (!acceptQuest->IsTraced)
		{
			return NULL;
		}

		if (quest->QuestType == E_qt_Emegency)
		{
			UBOOL bFound = FALSE;
			for (INT i = 0; i < MAX_QUEST_PER_ZONE; ++ i)
			{
				if (questId == _CurQuestGridQuestId[i])
				{
					bFound = TRUE;
				}
			}

			if (!bFound)
			{
				return NULL;
			}
		}

		_SetAcceptedQuestTraceListItemCaption(quest, acceptQuest, gfxPlayer, object);
		_SetAcceptedQuestTraceListItemDesc(quest, acceptQuest, gfxPlayer, object);
		_SetAcceptedQuestTraceListItemTarget(quest, acceptQuest, gfxPlayer, object);
		_SetAcceptedQuestTraceListItemTime(quest, acceptQuest, gfxPlayer, object);
		_SetAcceptedQuestTraceListItemAward(quest, acceptQuest, gfxPlayer, object);
	}
	else
	{
		_SetAvailableQuestTraceListItemCaption(quest, gfxPlayer, object);
		_SetAvailableQuestTraceListItemDesc(quest, gfxPlayer, object);
	}

	return object;
}

void MMOQuestManager::_SetQuestDetailBaseInfo(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
	UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
	UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
	UBOOL isQuestCompleted = MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID());

	object->SetInt(TEXT("quest_id"), quest->GetTemplateID());
	object->SetInt(TEXT("quest_type"), quest->QuestType);
	object->SetBool(TEXT("quest_is_accepted"), isQuestAccepted);
	object->SetBool(TEXT("quest_is_available"), isQuestAvailable);
	object->SetBool(TEXT("quest_is_compassable"), isQuestCompassable);
	object->SetBool(TEXT("quest_is_completed"),isQuestCompleted);
	object->SetBool(TEXT("quest_can_abort"), quest->CanAbandon);
	object->SetBool(TEXT("quest_can_share"), quest->CanShare && TeamManager::getSingleton().HasTeam());

	UBOOL isQuestAvailableWithoutNpc = isQuestAvailable && (quest->StartType == EAQT_Item || quest->StartType == EAQT_Zone || quest->StartType == EAQT_Level);
	UBOOL isQuestCompleteWithoutNpc = isQuestCompassable && (quest->FinishType == ESQT_Item || quest->FinishType == ESQT_Zone || quest->FinishType == ESQT_Auto || quest->FinishType == ESQT_Manual);
	object->SetBool(TEXT("quest_without_npc"), isQuestAvailableWithoutNpc || isQuestCompleteWithoutNpc);

	// emergency quest attributes
	object->SetBool(TEXT("quest_is_urgent"), quest->QuestType == E_qt_Emegency);
	FString emergencyStr = FString::Printf(TEXT("%d/%d"), MMOQuestData::getSingleton().GetEmergencyQuestCompleteCount(), MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY);
	object->SetString(TEXT("quest_urgent"), emergencyStr);
}

void MMOQuestManager::_SetQuestDetailCaption(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	// chapter
	FString ChapterName = TEXT("");
	FString ChapterDesc = TEXT("");
	FString StageName = TEXT("");
	FString StageDesc = TEXT("");
	FString ChapterIcon = TEXT("");
	StoryQuestData data;
	if (MMOQuestData::getSingleton().GetStoryQuestData(quest->QuestChapter, quest->QuestStage, data))
	{
		ChapterName = data.ChapterName;
		ChapterDesc = data.ChapterDesc;
		StageName = data.StageName;
		StageDesc = data.StageDesc;
		ChapterIcon = data.ChapterIcon;
	}
		
	object->SetString(TEXT("quest_chapter_name"), ChapterName);
	object->SetString(TEXT("quest_chapter_desc"), ChapterDesc);
	object->SetString(TEXT("quest_stage_name"), StageName);
	object->SetString(TEXT("quest_stage_desc"), StageDesc);
	object->SetString(TEXT("quest_chapter_icon"), ChapterIcon);

	// title
	FString questCaption = UITextParser::HTMLFormatQuestName(quest, false, true);
	if (MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID()))
	{
		questCaption += _CondComplete;
	}
	object->SetString(TEXT("quest_caption"), questCaption);
}

void MMOQuestManager::_SetQuestDetailDesc(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object, UBOOL isDialog)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	object->SetString(TEXT("quest_desc_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.DescCaption")));

	/*
	UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
	UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
	UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());

	ParseContext parseContext;
	DialogContext dlgContext;
	FString questDesc = TEXT("");
	if (isDialog)
	{
		if (isQuestAvailable)
		{
			questDesc = quest->AcceptQuestDialog;
		} 
		else if (isQuestAccepted && !isQuestCompassable)
		{
			questDesc = quest->PreSubmitQuestDialog;
		}
		else
		{
			questDesc = quest->CompleteQuestDialog;
		}
	} 
	else
	{
		if (isQuestAvailable)
		{
			questDesc = quest->AcceptQuestDialog;
		} 
		else
		{
			questDesc = quest->AcceptQuestDesc;
		}
	}

	UITextParser::getSingleton().Parse(parseContext, dlgContext, questDesc);
	*/

	DialogContext dlgContext;
	ParseQuestDialog(quest->GetTemplateID(), isDialog, dlgContext);

	dlgContext.TempText.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
	object->SetString(TEXT("quest_desc"), dlgContext.TempText);

	// circle quest
	FString questCircle = TEXT("");
	if (quest->CircleID > 0)
	{
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			CircleQuestData data;
			if (MMOQuestData::getSingleton().GetCircleQuestData(quest->CircleID, mainPlayer->ActorAttributes->LevelAttributes.Level, data))
			{
				CircleQuestInfo info;
				if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
				{
					questCircle = FString::Printf(TEXT("Current ring quest count: %d/%d"), info.CircleIndex + 1, data.CircleSize);
				}
			}
		}
	}
	object->SetString(TEXT("quest_circle"), questCircle);
}

void MMOQuestManager::_SetQuestDetailCondition(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	UGFxObject* conditionArray = gfxPlayer->CreateArray();
	if (!conditionArray)
	{
		return;
	}

	object->SetString(TEXT("quest_conditions_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ConditionsCaption")));

	INT index = 0;
	FString conditionDesc;

	// level requirement
	if (quest->QuestLevel >= 0)
	{
		conditionDesc = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.LevelReq"))), quest->QuestLevel);
		conditionArray->SetElementString(index, conditionDesc);
		++ index;
	}

	// gold requirement
	if (quest->MoneyAmountLimit)
	{
		FString moneyName;
		switch(quest->MoneyTypeLimit)
		{
		case MONEYTYPE_01:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_01"));
			break;
		case MONEYTYPE_02:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_02"));
			break;
		case MONEYTYPE_03:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_03"));
			break;
		case MONEYTYPE_04:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_04"));
			break;
		case MONEYTYPE_FATIGUE:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_FATIGUE"));
			break;
		case MONEYTYPE_SOUL_PTS:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_SOUL_PTS"));
			break;
		case MONEYTYPE_MASTER_EXP:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_MASTER_EXP"));
			break;
		default:
			moneyName = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.EMONEY_TYPE.E_MONEYTYPE_01"));
			break;
		}

		conditionDesc = FString::Printf(TEXT("%s>%d"), *moneyName, quest->MoneyAmountLimit);
		conditionArray->SetElementString(index, conditionDesc);
		++ index;
	}

	// gender requirement
	if (quest->SexLimit)
	{
		FString genderName;
		switch(quest->SexLimit)
		{
		case EGT_Male:
			genderName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.MaleLimit"));
			break;
		case EGT_Female:
			genderName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FemaleLimit"));
			break;
		case EGT_Asexua:
			genderName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.MaleOrFemale"));
			break;
		}
		conditionDesc = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.MaleOrFemale"))), *genderName);
		conditionArray->SetElementString(index, conditionDesc);
		++ index;
	}

	// item requirement
	for (INT i = 0; i < quest->ItemLimit.Num(); ++ i)
	{
		UItem* item = ((UQuest*)quest)->GetLimitItem(i);
		if (item)
		{
			INT itemCount = quest->ItemLimit(i).CheckItemCnt;
			FString itemName = item->GetDisplayName();

			conditionDesc = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ItemReq"))), itemCount, *itemName);
			conditionArray->SetElementString(index, conditionDesc);
			++ index;
		}
	}

	// accept
	if (quest->StartType == EAQT_Npc)
	{
		UNPCTemplate* startNpc = ((UQuest*)quest)->GetStartNpc();
		if (startNpc)
		{
			INT mapID = quest->StartNPCMap;
			FString startDesc = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AcceptTxtFormat"));
			conditionDesc = FString::Printf(*startDesc, *LinkManager::HtmlLink_NPCNav(startNpc->GetDisplayName(), startNpc->TemplateID, mapID, FALSE));

			int destId = 0;
			if (quest->StartNpcFly)
			{
				conditionDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			conditionDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
			conditionArray->SetElementString(index, conditionDesc);
			++ index;
		}
	} 
	else if (quest->StartType == EAQT_Item)
	{
		UItem* startItem = ((UQuest*)quest)->GetStartItem();
		if (startItem)
		{
			FString startDesc = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AcceptTxtFormat"));
			conditionDesc = FString::Printf(*startDesc, *LinkManager::HtmlLink_Item(startItem));
			conditionDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
			conditionArray->SetElementString(index, conditionDesc);
			++ index;
		}
	}
	else if (quest->StartType == EAQT_Level)
	{
		FString startDesc = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AcceptLevel"));
		conditionDesc = FString::Printf(*startDesc, quest->QuestLevel);
		conditionDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
		conditionArray->SetElementString(index, conditionDesc);
		++ index;
	}

	object->SetObject(TEXT("quest_conditions"), conditionArray);
}

void MMOQuestManager::_SetQuestDetailTargetCheckTalkNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	if (quest->TalkNpcID <= 0)
	{
		return;
	}

	const UNPCTemplate* npcTemp = MMOFacade::getSingleton().FindNPCTemplate(quest->TalkNpcID);
	if (!npcTemp)
	{
		return;
	}

	INT mapID = quest->TalkNpcMap;
	FString npcName = LinkManager::HtmlLink_NPCNav(npcTemp->GetDisplayName(), npcTemp->GetTemplateID(), mapID, FALSE);
	npcName = FString::Printf(TEXT("<font color='#14a800'>%s</font>"), *npcName);

	targetDesc += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.TalkToNPCDarkLink")), *npcName);

	if (quest->TalkNpcFly)
	{
		targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
		destId ++;
	}

	targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
	objectArray->SetElementString(index, targetDesc);
	index ++;
}

void MMOQuestManager::_SetQuestDetailTargetCheckLevel(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, 
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	if(quest->CheckLevel <= 0)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	UActorAttribute* attri = mainPlayer->ActorAttributes;
	if (!attri)
	{
		return;
	}
				
	if (attri->LevelAttributes.Level < quest->CheckLevel)
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.LevelLimitWithColor")),
			*_CondNotCompleteColor, quest->CheckLevel);
	} 
	else
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.LevelLimitWithColor")),
			*_CondCompleteColor, quest->CheckLevel);
		targetDesc += _CondComplete;
	}

	objectArray->SetElementString(index, targetDesc);
	index ++;
}

void MMOQuestManager::_SetQuestDetailTargetCheckVariable(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, 
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckVar.Num(); ++i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType == TEXT("vtQuest") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_VARIABLE_PER_QUEST)
		{
			if (acceptedQuest->QuestVariable[checkVar.CheckVarID] != checkVar.CheckVarCnt)
			{
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().Parse(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;	
			}
			else
			{
				// condition met, remove underline
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
				targetDesc += _CondComplete;
			}

			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
		else if (checkVar.CheckVarType == TEXT("vtClient") &&
			checkVar.CheckVarID == ECVT_Quest)
		{
			// whether class-specific description exists
			for (int j = 0; j < quest->CheckClassVar.Num(); ++ j)
			{
				FQuestCheckClassVar checkClassVar = quest->CheckClassVar(j);
				INT classId = MMOFacade::getSingleton().GetMainActor()->GetClassId();
				if (classId == j && checkClassVar.CheckClassVarCnt > 0)
				{
					checkVar.Description = checkClassVar.CheckClassVarDesc;
					break;
				}
			}

			if((acceptedQuest->ClientVariable[checkVar.CheckVarID] & (INT32)(1 << checkVar.CheckVarCnt)) > 0)
			{
				// condition met, remove underline
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
				targetDesc += _CondComplete;	
			}
			else
			{
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().Parse(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
			}

			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
		else if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT)
		{
			if (acceptedQuest->RoleVariable[checkVar.CheckVarID] != checkVar.CheckVarCnt)
			{
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().Parse(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;	
			}
			else
			{
				// condition met, remove underline
				dlgContext.TempText = TEXT("");
				UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkVar.Description);
				targetDesc = dlgContext.TempText;
				targetDesc += _CondComplete;
			}

			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
	}
}

void MMOQuestManager::_SetQuestDetailTargetCheckItem(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, 
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckItem.Num(); ++i)
	{
		const FQuestCheckItem* checkItem = &quest->CheckItem(i);
		UItem* item = ((UQuest*)quest)->GetCheckItem(i);
		if (item)
		{
			INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(item->GetTemplateID(), E_CONSLOT_DEFAULT);
			if (itemCount > 0)
			{
				itemCount = Min(itemCount, checkItem->CheckItemCnt);
			}

			FString itemName;
			if (itemCount < checkItem->CheckItemCnt)
			{
				if (checkItem->Description.IsEmpty())
				{
					itemName = LinkManager::HtmlLink_ItemNav(item->GetDisplayName(), item->GetTemplateID(), 
						checkItem->CheckItemNav, checkItem->CheckItemMap, FALSE);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, checkItem->Description);	
					itemName = dlgContext.TempText;
				}
				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*itemName, *_CondNotCompleteColor, itemCount, checkItem->CheckItemCnt);
			}
			else
			{
				// condition met, remove underline
				if (checkItem->Description.IsEmpty())
				{
					itemName = item->GetDisplayName();
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkItem->Description);
					itemName = dlgContext.TempText;
				}
				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*itemName, *_CondCompleteColor, itemCount, checkItem->CheckItemCnt);
				targetDesc += _CondComplete;
			}

			if (checkItem->CheckItemFly)
			{
				targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
	}
}

void MMOQuestManager::_SetQuestDetailTargetCheckKillNpc(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, 
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckKillNpc.Num(); ++i)
	{
		const FQuestCheckKillNpc* killNpc = &quest->CheckKillNpc(i);
		UNPCTemplate* npc = ((UQuest*)quest)->GetCheckKillNpc(i);					
		if (npc || (killNpc && killNpc->CheckKillNpcGroupId > 0))
		{
			INT monsterCount = acceptedQuest->KillNpcCount[i];
			if (monsterCount > 0)
			{
				monsterCount = Min(monsterCount, killNpc->CheckKillNpcCnt);
			}

			FString monsterName;
			if (monsterCount < killNpc->CheckKillNpcCnt)
			{
				if (killNpc->Description.IsEmpty())
				{
					FString name = (npc != NULL) ? npc->GetDisplayName() : TEXT("");
					monsterName = LinkManager::HtmlLink_NavPointName(name,
						killNpc->CheckKillNpcNav, killNpc->CheckKillNpcMap, FALSE);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, killNpc->Description);
					monsterName = dlgContext.TempText;
				}

				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*monsterName, *_CondNotCompleteColor, acceptedQuest->KillNpcCount[i], killNpc->CheckKillNpcCnt);
			}
			else
			{
				// condition met, remove underline
				if (killNpc->Description.IsEmpty())
				{
					monsterName = (npc != NULL) ? npc->GetDisplayName() : TEXT("");
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, killNpc->Description);
					monsterName = dlgContext.TempText;
				}
				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*monsterName, *_CondCompleteColor, acceptedQuest->KillNpcCount[i], killNpc->CheckKillNpcCnt);
				targetDesc += _CondComplete;
			}

			if (killNpc->CheckKillNpcFly)
			{
				targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
	}

}

void MMOQuestManager::_SetQuestDetailTargetCheckKillDoodad(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, 
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckKillDoodad.Num(); ++i)
	{
		const FQuestCheckKillDoodad* checkKillDoodad = &quest->CheckKillDoodad(i);			
		if (checkKillDoodad->CheckKillDoodadID)
		{
			FString doodadName = TEXT("");
			class UDoodad* doodad = DoodadManager::getSingleton().FindDoodadTemplate(checkKillDoodad->CheckKillDoodadID);
			if (doodad)
			{
				doodadName = doodad->DoodadName.ToString();
			}

			INT doodadCount = acceptedQuest->KillDoodadCount[i];
			if (doodadCount > 0)
			{
				doodadCount = Min(doodadCount, checkKillDoodad->CheckKillDoodadCnt);
			}

			FString doodadDesc;
			if (doodadCount < checkKillDoodad->CheckKillDoodadCnt)
			{
				if (checkKillDoodad->CheckKillDoodadDesc.IsEmpty())
				{
					doodadDesc = LinkManager::HtmlLink_DoodadNav(doodadName, 
						checkKillDoodad->CheckKillDoodadID,
						checkKillDoodad->CheckKillDoodadNav,
						checkKillDoodad->CheckKillDoodadMap,
						FALSE);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, checkKillDoodad->CheckKillDoodadDesc);
					doodadDesc = dlgContext.TempText;
				}

				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*doodadDesc, *_CondNotCompleteColor, acceptedQuest->KillDoodadCount[i], checkKillDoodad->CheckKillDoodadCnt);
			}
			else
			{
				// condition met, remove underline
				if (checkKillDoodad->CheckKillDoodadDesc.IsEmpty())
				{
					doodadDesc = doodadName;
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkKillDoodad->CheckKillDoodadDesc);
					doodadDesc = dlgContext.TempText;
				}
				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*doodadDesc, *_CondCompleteColor, acceptedQuest->KillDoodadCount[i], checkKillDoodad->CheckKillDoodadCnt);
				targetDesc += _CondComplete;
			}

			if (checkKillDoodad->CheckKillDoodadFly)
			{
				targetDesc += LinkManager::HtmlLink_NavFly(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.FlyShoes")), quest->GetTemplateID(), destId);
				destId ++;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
	}
}

void MMOQuestManager::_SetQuestDetailTargetCheckKillPlayer(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId,
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	for (INT i = 0; i < quest->CheckKillPlayer.Num(); ++i)
	{
		const FQuestCheckKillPlayer* checkKillPlayer = &quest->CheckKillPlayer(i);			
		if (checkKillPlayer->CheckKillPlayerCnt > 0)
		{
			INT killCount = acceptedQuest->KillPlayerCount[i];
			if (killCount > 0)
			{
				killCount = Min(killCount, checkKillPlayer->CheckKillPlayerCnt);
			}

			FString killPlayerName;
			if (killCount < checkKillPlayer->CheckKillPlayerCnt)
			{
				if (checkKillPlayer->CheckKillPlayerDesc.IsEmpty())
				{
					//monsterName = LinkManager::HtmlLink_NPCNav(checkKillPlayer->CheckKillNpcNew->GetDisplayName(),
					//	checkKillPlayer->CheckKillNpcNew->GetTemplateID(), checkKillPlayer->CheckKillNpcMap);
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().Parse(parseContext, dlgContext, checkKillPlayer->CheckKillPlayerDesc);
					killPlayerName = dlgContext.TempText;
				}

				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*killPlayerName, *_CondNotCompleteColor, acceptedQuest->KillPlayerCount[i], checkKillPlayer->CheckKillPlayerCnt);
			}
			else
			{
				// condition met, remove underline
				if (checkKillPlayer->CheckKillPlayerDesc.IsEmpty())
				{
					//monsterName = checkKillPlayer->CheckKillNpcNew->GetDisplayName();
				}
				else
				{
					dlgContext.TempText = TEXT("");
					UITextParser::getSingleton().ParseContent(parseContext, dlgContext, checkKillPlayer->CheckKillPlayerDesc);
					killPlayerName = dlgContext.TempText;
				}
				targetDesc = FString::Printf(TEXT("%s <font color='%s'>(%d/%d)</font>"),
					*killPlayerName, *_CondCompleteColor, acceptedQuest->KillNpcCount[i], checkKillPlayer->CheckKillPlayerCnt);
				targetDesc += _CondComplete;
			}

			targetDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
			objectArray->SetElementString(index, targetDesc);
			index ++;
		}
	}
}

void MMOQuestManager::_SetQuestDetailTargetCheckReputation(const UQuest* quest, AcceptedQuestData* acceptedQuest, int& destId, 
	UMMOGFXPlayer* gfxPlayer, UGFxObject* objectArray, int& index)
{
	if (!quest)
	{
		return;
	}

	if (!acceptedQuest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!objectArray)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;
	FString targetDesc;

	if (quest->CheckReputationForceID <= 0)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	FString reputationName;
	FString reputationLevel;
	// get reputation name
	UMMOUICharacter* panel = Cast<UMMOUICharacter>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Character));
	if (panel)
	{
		reputationName = panel->GetReputationForceName(quest->CheckReputationForceID);
		reputationLevel = panel->GetReputationLevelName(quest->CheckReputationForceID,quest->CheckReputation);
	}

	INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), quest->CheckReputationForceID);
	if (reputation < quest->CheckReputation)
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ReputationWithColor")),
			*reputationName,
			*_CondNotCompleteColor, 
			*reputationLevel);
	} 
	else
	{
		targetDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ReputationWithColor")),
			*reputationName,
			*_CondCompleteColor, 
			*reputationLevel);
		targetDesc += _CondComplete;
	}

	objectArray->SetElementString(index, targetDesc);
	index ++;
}

void MMOQuestManager::_SetQuestDetailTarget(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	AcceptedQuestData* acceptedQuest = MMOQuestData::getSingleton().GetAcceptedQuestData(quest->GetTemplateID());
	if (!acceptedQuest)
	{
		return;
	}

	UGFxObject* objectArray = gfxPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	object->SetString(TEXT("quest_targets_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.TaskRequire")));

	int destId = 0;
	if (quest->StartNPCNewId > 0 && quest->StartNpcFly)
	{
		// if the start-quest NPC requires flying shoes
		destId ++;
	}

	int index = 0;
	_SetQuestDetailTargetCheckTalkNpc(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckLevel(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckVariable(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckItem(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckKillNpc(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckKillDoodad(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckKillPlayer(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);
	_SetQuestDetailTargetCheckReputation(quest, acceptedQuest, destId, gfxPlayer, objectArray, index);

	object->SetObject(TEXT("quest_targets"), objectArray);
}

void MMOQuestManager::_SetQuestDetailSubmit(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	ParseContext parseContext;
	DialogContext dlgContext;

	object->SetPureString(TEXT("quest_submit_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.TraceCompleteCaption")));

	FString submitDesc = TEXT("");
	if (MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
	{
		/*
		if (quest->GetFinishNpc())
		{
			INT mapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
			FString npcName = LinkManager::HtmlLink_NPCNav(quest->GetFinishNpc()->GetDisplayName(),
				quest->GetFinishNpc()->TemplateID, mapID);

			submitDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.SubmitWithColor")), *npcName);
		}
		*/
		if (!quest->QuestCompletedDescript.IsEmpty())
		{
			UITextParser::getSingleton().Parse(parseContext, dlgContext, quest->QuestCompletedDescript);
			submitDesc = dlgContext.TempText;
		}
	} 
	else
	{
		UNPCTemplate* finishNpc = ((UQuest*)quest)->GetFinishNpc();
		UItem* finishItem = ((UQuest*)quest)->GetFinishItem();
		if (finishNpc)
		{
			submitDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.Submit")),
				*(finishNpc->GetDisplayName()));
		}
		else if (finishItem)
		{
			submitDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.Submit")),
				*(finishItem->GetDisplayName()));
		}
	}

	submitDesc.ReplaceInline(TEXT("#1EFF00"), TEXT("#448407"));
	object->SetPureString(TEXT("quest_submit"), submitDesc);
}

void MMOQuestManager::_SetQuestDetailAward(const UQuest* quest, UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (!quest)
	{
		return;
	}

	if (!gfxPlayer)
	{
		return;
	}

	if (!object)
	{
		return;
	}

	object->SetString(TEXT("quest_award_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardCaption")));

	QuestAwardItemInfoList awardList;
	MMOQuestData::getSingleton().GetAwardItemList(quest->GetTemplateID(), awardList);
	if (awardList.size() > 0)
	{
		UGFxObject* awardArray = gfxPlayer->CreateArray();
		if (awardArray)
		{
			for(size_t i = 0; i < awardList.size(); ++i)
			{
				const QuestAwardItemInfo& awardInfo = awardList[i];
				UGFxObject* awardIconData = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestAwardIconData"));
				check(awardIconData != NULL);

				awardIconData->SetInt(TEXT("item_template_id"), awardInfo.ItemTemplate->GetTemplateID());
				awardIconData->SetInt(TEXT("item_count"), awardInfo.Count);
				awardIconData->SetInt(TEXT("item_index"), awardInfo.Index);
				awardIconData->SetString(TEXT("icon_img_name"), MMOFacade::getSingleton().GetItemIconNamePersonalized( awardInfo.ItemTemplate ));
				awardIconData->SetString(TEXT("item_name"), awardInfo.ItemTemplate->GetDisplayName());

				awardArray->SetElementObject(i, awardIconData);		
			}

			object->SetObject(TEXT("quest_awards"), awardArray);
		}
	}

	// optional quest rewards
	QuestAwardItemInfoList selectableAwardList;
	MMOQuestData::getSingleton().GetSelectableAwardItemList(quest->GetTemplateID(), selectableAwardList);
	if (selectableAwardList.size() > 0)
	{
		object->SetString(TEXT("quest_award_select_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardOneOfCaption")));
		UGFxObject* selectableAwardArray = gfxPlayer->CreateArray();
		if (selectableAwardArray)
		{
			for(size_t i = 0; i < selectableAwardList.size(); ++i)
			{
				const QuestAwardItemInfo& awardInfo = selectableAwardList[i];
				UGFxObject* awardIconData = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestAwardIconData"));
				check(awardIconData != NULL);

				awardIconData->SetInt(TEXT("item_template_id"), awardInfo.ItemTemplate->GetTemplateID());
				awardIconData->SetInt(TEXT("item_count"), awardInfo.Count);
				awardIconData->SetInt(TEXT("item_index"), awardInfo.Index);
				awardIconData->SetPureString(TEXT("icon_img_name"), MMOFacade::getSingleton().GetItemIconNamePersonalized( awardInfo.ItemTemplate ));
				awardIconData->SetPureString(TEXT("item_name"), awardInfo.ItemTemplate->GetDisplayName());

				selectableAwardArray->SetElementObject(i, awardIconData);		
			}
			object->SetObject(TEXT("quest_award_select"), selectableAwardArray);
		}
	}

	// gold, experience, activity
	// bug fix: 10796
	INT moneyType = quest->AwardCurrencyType;
	INT money = quest->AwardCurrencyAmount;
	INT moneyTypeEx = quest->AwardCurrencyTypeEx;
	INT moneyEx = quest->AwardCurrencyAmountEx;
	INT exp = quest->AwardExp;
	INT activity = quest->AwardActivity;

	if (quest->CircleID > 0)
	{
		CircleQuestInfo info;
		if (MMOQuestData::getSingleton().GetCircleQuestInfo(quest->CircleID, info))
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				CircleQuestRewardData awardData;
				if (MMOQuestData::getSingleton().GetCircleQuestRewardData(info.CircleId, info.CircleIndex, mainPlayer->ActorAttributes->LevelAttributes.Level, awardData))
				{
					moneyType = awardData.MoneyType;
					money = awardData.Money;
					moneyTypeEx = awardData.MoneyTypeEx;
					moneyEx = awardData.MoneyEx;
					exp = awardData.Exp;
				}
			}
		}
	}

	FString moneyName = TEXT("");
	FString moneyIcon = TEXT("img://MMO_ItemIcon_Supplies.img_Money");
	_GetQuestAwardMoneyInfo(moneyType, moneyName, moneyIcon);
	FString moneyCap = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardMoneyCaption")), *moneyName);
	object->SetString(TEXT("quest_award_money_caption"), moneyCap);
	object->SetString(TEXT("quest_award_money_icon"), moneyIcon);
	object->SetString(TEXT("quest_award_money"), money > 0 ? UITextParser::HTMLMoneyIconString(moneyType, money) : TEXT(""));

	moneyName = TEXT("");
	moneyIcon = TEXT("img://MMO_ItemIcon_Supplies.img_Money");
	_GetQuestAwardMoneyInfo(moneyTypeEx, moneyName, moneyIcon);
	moneyCap = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardMoneyCaption")), *moneyName);
	object->SetString(TEXT("quest_award_money_caption_ex"), moneyEx > 0 ? moneyCap : TEXT(""));
	object->SetString(TEXT("quest_award_money_icon_ex"), moneyIcon);
	object->SetString(TEXT("quest_award_money_ex"), moneyEx > 0 ? UITextParser::HTMLMoneyIconString(moneyTypeEx, moneyEx) : TEXT(""));

	object->SetString(TEXT("quest_award_exp_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardExpCaption")));
	object->SetString(TEXT("quest_award_exp_icon"), TEXT("img://MMO_ItemIcon_Supplies.item_144"));
	object->SetInt(TEXT("quest_award_exp"), exp);

	object->SetString(TEXT("quest_award_activity_caption"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AwardActivityCaption")));
	object->SetString(TEXT("quest_award_activity_icon"), TEXT("img://MMO_ItemIcon_Supplies.item_147"));
	object->SetInt(TEXT("quest_award_activity"), activity);
}

UGFxObject* MMOQuestManager::GetQuestDetailData(INT questId, UMMOGFXPlayer* gfxPlayer, UBOOL isDialog)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return NULL;
	}

	UGFxObject* object = gfxPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestDetailData"));
	if (!object)
	{
		return NULL;
	}

	_SetQuestDetailBaseInfo(quest, gfxPlayer, object);
	_SetQuestDetailCaption(quest, gfxPlayer, object);
	_SetQuestDetailDesc(quest, gfxPlayer, object, isDialog);

	UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(questId);
	UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(questId);

	if (isQuestAvailable)
	{
		_SetQuestDetailCondition(quest, gfxPlayer, object);
	}
	else if (isQuestAccepted)
	{
		_SetQuestDetailTarget(quest, gfxPlayer, object);
	}

	_SetQuestDetailSubmit(quest, gfxPlayer, object);
	_SetQuestDetailAward(quest, gfxPlayer, object);

	return object;
}

//////////////////////////////////////////////////////////////////////////
INT MMOQuestManager::GetMaxDialogPage()
{
	if(_QuestDialongContext)
	{
		return _QuestDialongContext->DialogText.Num();
	}

	return 0;
}

void MMOQuestManager::ClearContext()
{
	if(_QuestDialongContext)
	{
		_QuestDialongContext->Clear();
	}
}

void MMOQuestManager::ParseQuestDialog(INT questId, UBOOL isDialog,DialogContext &dialogContext)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return ;
	}

	UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
	UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
	UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());

	ParseContext parseContext;
	//DialogContext dlgContext;
	FString questDesc = TEXT("");
	if (isDialog)
	{
		if (isQuestAvailable)
		{
			questDesc = quest->AcceptQuestDialog;
		} 
		else if (isQuestAccepted && !isQuestCompassable)
		{
			questDesc = quest->PreSubmitQuestDialog;
		}
		else
		{
			questDesc = quest->CompleteQuestDialog;
		}
	} 
	else
	{
		if (isQuestAvailable)
		{
			questDesc = quest->AcceptQuestDialog;
		} 
		else if (isQuestAccepted && !isQuestCompassable)
		{
			questDesc = quest->AcceptQuestDesc;
		}
		else
		{
			// fix bug #24195
			questDesc = TEXT("");
		}
	}
	
	//ClearContext();
	UITextParser::getSingleton().Parse(parseContext, dialogContext, questDesc);
}

void MMOQuestManager::SetQuestDialogCurrentPage(INT page)
{
	if (page < 0 || page >= MAX_PAGENUMBER)
	{
		return;
	}

	_CurPage = page;

	if (_CurPageQuestId != INVALID_QUEST_ID)
	{
		// disable quest sound effects while cutscene is playing
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer && !mainPlayer->IsInMatinee)
		{
			const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(_CurPageQuestId);
			if (quest)
			{
				UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
				UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
				UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());

				if (isQuestAvailable)
				{
					if (quest->QuestPageSoundAccept.Num() > 0 && quest->QuestPageSoundAccept(_CurPage).PageSoundPath != TEXT(""))
					{
						USoundCue* questPageSoundAccept = ((UQuest*)quest)->GetQuestPageSoundAccept(_CurPage);
						if (questPageSoundAccept)
						{
							MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
							MMOFacade::getSingleton().GetMainActor()->PlaySound(questPageSoundAccept);
						}
					}
				}
				else if (isQuestAccepted && isQuestCompassable)
				{
					if (quest->QuestPageSoundSubmit.Num() > 0 && quest->QuestPageSoundSubmit(_CurPage).PageSoundPath != TEXT(""))
					{
						USoundCue* questPageSoundSubmit = ((UQuest*)quest)->GetQuestPageSoundSubmit(_CurPage);
						if (questPageSoundSubmit)
						{
							MMOFacade::getSingleton().StopAllVoiceSound(TRUE, 0.0, 0.0);
							MMOFacade::getSingleton().GetMainActor()->PlaySound(questPageSoundSubmit);
						}
					}
				}
			}
		}
	}

	if (_CurPage > 0)
	{
		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_QuestPageDown);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

FString MMOQuestManager::GetContextContent(INT i)
{
	if (_QuestDialongContext->DialogText.Num() > 0 && 
		i < _QuestDialongContext->DialogText.Num())
	{
		return _QuestDialongContext->DialogText(i);
	}

	return "";
}

void MMOQuestManager::ResetContent()
{
	ClearContext();
	SetPageQuestId(INVALID_QUEST_ID);
	SetQuestDialogCurrentPage(0);
	RestPageTime();
}

void MMOQuestManager::_LevelLoaded()
{
	if (_FirstMatinee)
	{
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Role>(132, mainPlayer->ServerID, 1, false);
		}

		_FirstMatinee = FALSE;
	}
}

void MMOQuestManager::SetCurTrunkQuestId(INT questId)
{
	_CurTrunkQuestId = questId;
}

void MMOQuestManager::SetCurOtherQuestId(INT questId)
{
	_CurOtherQuestId = questId;
}

void MMOQuestManager::AddQuestCoolDown(INT questId, UINT32 coolDown)
{
	MMOQuestData::getSingleton().AddEmergencyQuestCoolDown(questId, coolDown);
}

void MMOQuestManager::_ProcessFirstEnterScene()
{
	// quest #25538
	QuestTraceList questList = MMOQuestData::getSingleton().GetReAcceptQuestList();
	QuestTraceList::iterator iter = questList.begin();
	for (; iter != questList.end(); ++iter)
	{
		AcceptQuest((INT)(*iter), FALSE, 0);
	}
}