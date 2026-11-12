#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "TeamManager.h"
#include "MMOMapDataManager.h"
#include "MMOGuideManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIQuestPanel)

void UMMOUIQuestPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Quest);
	EventAdapter = new TGameEventAdapter<UMMOUIQuestPanel>(this, evts);

	UGFxObject* mc = NULL;
	mc = GetObject(TEXT("tab0_mc"));

	if (mc)
	{
		mc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.QuestCategoryAccepted")));
	}

	mc = GetObject(TEXT("tab1_mc"));
	if (mc)
	{
		mc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.QuestCategoryAvailable")));
	}

	mc = GetObject(TEXT("tab2_mc"));
	if (mc)
	{
		mc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.QuestCategoryCompleted")));
	}

	mc = GetObject(TEXT("btn_share"));
	if (mc)
	{
		mc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.ShareQuest")));
	}

	mc = GetObject(TEXT("btn_abort"));
	if (mc)
	{
		mc->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AbortQuest")));
	}

	_CurQuestStateGroup = EQS_Null;
	_CurQuestId = INVALID_QUEST_ID;
	_ShowLowLvQuest = 1;
}

void UMMOUIQuestPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		/*
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			UActorAttribute* attri = mainPlayer->ActorAttributes;
			if (attri && attri->LevelAttributes.Level < 10)
			{
				FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.LevelLimit"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Quest.LevelLimit"));

				Show(FALSE);
				return;
			}
		}
		*/

		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.Quest")));

		_CurQuestId = MMOQuestManager::getSingleton().GetCurOtherQuestId();
		_CurQuestStateGroup = MMOQuestData::getSingleton().GetQuestState(_CurQuestId);
		if (_CurQuestStateGroup == EQS_Null)
		{
			_CurQuestStateGroup = EQS_Accept;
		}

		UpdateQuestList();
		UpdateQuestDetail();

		MMOFacade::getSingleton().PlayUISound(TEXT("TaskSheet_Op"));
	}
	else
	{
		MMOFacade::getSingleton().PlayUISound(TEXT("TaskSheet_Cls"));
	}
	
	MMOGameEvent_Guide evtGuide(EGUET_ShowQuestPanel, v ? 1 : 0);
	MMOUI::PostGameEvent(&evtGuide);
}

void UMMOUIQuestPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

	if (gameEvent->GetEventType() != GameEvent_Quest)
	{
		return;
	}

	MMOGameEvent_Quest* questEvent = (MMOGameEvent_Quest*)gameEvent;
	if (questEvent == NULL)
	{
		return;
	}

	if (questEvent->QuestId == INVALID_QUEST_ID || questEvent->QuestId != _CurQuestId)
	{
		return;
	}

	switch(questEvent->EventType)
	{
	case MMOGameEvent_Quest::EQET_AcceptQuest:
	case MMOGameEvent_Quest::EQET_UpdateQuest:
	case MMOGameEvent_Quest::EQET_AbandonQuest:
	case MMOGameEvent_Quest::EQET_CompleteQuest:
		{
			UpdateQuestList();
			UpdateQuestDetail();
		}
		break;

	case MMOGameEvent_Quest::EQET_ClearQuestDetail:
		{
			_CurQuestId = INVALID_QUEST_ID;
			UpdateQuestDetail();
		}
		break;
	}
}

void UMMOUIQuestPanel::ItemChange(INT ContainerID,INT SlotID)
{
	if (!IsVisible)
	{
		return;
	}
	UpdateQuestList();
	UpdateQuestDetail();
}

void UMMOUIQuestPanel::ItemDelete(INT ContainerID, INT SlotID)
{
	if (!IsVisible)
	{
		return;
	}
	UpdateQuestList();
	UpdateQuestDetail();
}

void UMMOUIQuestPanel::PlayerLevelChange( INT lastLv , INT newLv )
{
	if (!IsVisible)
	{
		return;
	}
	UpdateQuestList();
	UpdateQuestDetail();
}


void UMMOUIQuestPanel::_UpdateQuestSubListByMap(UINT32 mapId, UGFxObject* questArray, INT& index)
{
	FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(mapId);
	if (!mapInfo)
	{
		return;
	}

	QuestDataList curMapQuestList;
	QuestMapDataMap questMapDataMap = MMOQuestData::getSingleton().GetQuestMapDataMap();
	QuestMapDataMap::iterator iterMap = questMapDataMap.find(mapId);
	if (iterMap != questMapDataMap.end())
	{
		curMapQuestList = iterMap->second;
	}

	int questAvailableCount = 0;
	int questAcceptedCount = 0;
	int questCompassableCount = 0;

	QuestDataList questList;
	questList.clear();
	QuestDataList::iterator iterList = curMapQuestList.begin();
	for (; iterList != curMapQuestList.end(); ++ iterList)
	{
		UQuest* quest = *iterList;
		if (!quest)
		{
			continue;
		}
		
		// Main quests go under story quests
		if (quest->QuestType == E_qt_Trunk)
		{
			continue;
		}

		// Activity quests go under the activity category
		if (quest->QuestType == E_qt_Activity)
		{
			continue;
		}

		// Quests above level 40 are not shown
		if (quest->QuestLevel > 40)
		{
			continue;
		}

		if (_CurQuestStateGroup == EQS_Accept && MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID()))
		{
			questList.push_back(quest);
			questAcceptedCount ++;

			if (MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
			{
				questCompassableCount ++;
			}
		}
		else if (_CurQuestStateGroup == EQS_Available && MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID()))
		{
			questList.push_back(quest);
			questAvailableCount ++;
		}
	}

	// If this type has no quests, do not show
	if (questList.size() == 0)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestData"));
	if (!object)
	{
		return;
	}

	// fix bug #23940
	if (_CurQuestId == INVALID_QUEST_ID)
	{
		iterList = questList.begin();
		const UQuest* quest = *iterList;
		if (quest)
		{
			_CurQuestId = quest->GetTemplateID();
		}
	}

	UBOOL expanded = FALSE;
	iterList = questList.begin();
	for (; iterList != questList.end(); ++iterList)
	{
		const UQuest* quest = *iterList;
		if (!quest)
		{
			continue;
		}

		if (_CurQuestId == quest->GetTemplateID())
		{
			expanded = TRUE;
			break;
		}
	}

	// Sub-list header
	object->SetInt(TEXT("quest_item_type"), EQIT_Group);
	object->SetBool(TEXT("quest_is_expanded"), expanded);
	
	FString questCountStr = TEXT("");
	if (_CurQuestStateGroup == EQS_Accept)
	{
		questCountStr = FString::Printf(TEXT("(%d/%d)"), questCompassableCount, questAcceptedCount);
	}
	else if (_CurQuestStateGroup == EQS_Available)
	{
		questCountStr = FString::Printf(TEXT("(%d)"), questAvailableCount);
	}
	object->SetString(TEXT("quest_caption"), mapInfo->MapDisplayName + questCountStr);

	questArray->SetElementObject(index, object);
	++ index;

	// Sub-list item
	INT playerLevel = MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->LevelAttributes.Level;
	INT lowLevelDiffCfg = 0;
	GConfig->GetInt(TEXT("Quest"), TEXT("LowLevelDiff"), lowLevelDiffCfg, GGameIni);

	iterList = questList.begin();
	for (; iterList != questList.end(); ++ iterList)
	{
		const UQuest* quest = *iterList;
		if (!quest)
		{
			continue;
		}

		object = GFXPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestData"));
		if (!object)
		{
			continue;
		}

		// Main quests go under story quests
		if (quest->QuestType == E_qt_Trunk)
		{
			continue;
		}

		// limit level check
		if (!_ShowLowLvQuest)
		{
			if (playerLevel - quest->QuestLevel > lowLevelDiffCfg)
			{
				continue;
			}
		}

		UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
		UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
		UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
		UBOOL isQuestCompleted = MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID());

		object->SetInt(TEXT("quest_item_type"), EQIT_Title);
		object->SetInt(TEXT("quest_type"), quest->QuestType);
		object->SetInt(TEXT("quest_id"), quest->QuestTempID);

		FString questCaption = TEXT("");
		static UEnum* questEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("EQUEST_TYPE"), TRUE);
		FString enumVal = questEnum->GetEnum(quest->QuestType).ToString();
		questCaption += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.QuestTitleDisplay_QuestType.")) + enumVal);
		questCaption += quest->QuestName;
		object->SetString(TEXT("quest_caption"), questCaption);
		object->SetString(TEXT("quest_level"), FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.QuestTitleDisplay_QuestType.Lv")), quest->QuestLevel));
		object->SetBool(TEXT("quest_is_accepted"), isQuestAccepted);
		object->SetBool(TEXT("quest_is_available"), isQuestAvailable);
		object->SetBool(TEXT("quest_is_compassable"), isQuestCompassable);
		object->SetBool(TEXT("quest_is_completed"),isQuestCompleted);
		object->SetBool(TEXT("quest_is_traceable"), _CurQuestStateGroup == EQS_Accept);
		object->SetBool(TEXT("quest_is_select"), quest->GetTemplateID() == _CurQuestId);
		object->SetBool(TEXT("quest_can_abort"), quest->CanAbandon && _CurQuestStateGroup == EQS_Accept);
		object->SetBool(TEXT("quest_can_share"), quest->CanShare && _CurQuestStateGroup == EQS_Accept);

		const AcceptedQuestData* acceptedData = MMOQuestData::getSingleton().GetAcceptedQuestData(quest->GetTemplateID());
		if (acceptedData)
		{
			object->SetBool(TEXT("quest_is_failed"),acceptedData->IsFailed);
			object->SetBool(TEXT("quest_is_traced"), acceptedData->IsTraced);
		}

		questArray->SetElementObject(index, object);
		++ index;
	}
}


void UMMOUIQuestPanel::_UpdateQuestSubListByType(EQUEST_TYPE questType, UGFxObject* questArray, INT& index)
{
	TArray<UQuest*> questList;
	switch (_CurQuestStateGroup)
	{
	case EQS_Accept:
		questList = MMOQuestData::getSingleton().GetAcceptedQuestsByType(questType);
		break;
	case EQS_Available:
		questList = MMOQuestData::getSingleton().GetAvailableQuestsByType(questType);
		break;
	}

	// If this type has no quests, do not show
	if (questList.Num() == 0)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestData"));
	if (!object)
	{
		return;
	}

	// fix bug #23940
	if (_CurQuestId == INVALID_QUEST_ID)
	{
		const UQuest* quest = questList(0);
		if (quest)
		{
			_CurQuestId = quest->GetTemplateID();
		}
	}

	UBOOL expanded = FALSE;
	for (INT i = 0; i < questList.Num(); ++i)
	{
		const UQuest* quest = questList(i);
		if (!quest)
		{
			continue;
		}

		if (_CurQuestId == quest->GetTemplateID())
		{
			expanded = TRUE;
			break;
		}
	}

	// Sub-list header
	object->SetInt(TEXT("quest_item_type"), EQIT_Group);
	object->SetInt(TEXT("quest_type"), questType);
	object->SetBool(TEXT("quest_is_expanded"), expanded);
	static UEnum* questEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("EQUEST_TYPE"), TRUE);
	FString questTypeName = questEnum->GetEnum(questType).ToString();
	
	FString questCountStr = TEXT("");
	if (_CurQuestStateGroup == EQS_Accept)
	{
		TArray<UQuest*> compassablelArray = MMOQuestData::getSingleton().GetCompassableQuestsByType(questType);
		int questCompleteCount = compassablelArray.Num();
		TArray<UQuest*> acceptedArray = MMOQuestData::getSingleton().GetAcceptedQuestsByType(questType);
		int questAcceptedCount = acceptedArray.Num();
		questCountStr = FString::Printf(TEXT("(%d/%d)"), questCompleteCount, questAcceptedCount);
	} 
	else if (_CurQuestStateGroup == EQS_Available)
	{
		TArray<UQuest*> availableArray = MMOQuestData::getSingleton().GetAvailableQuestsByType(questType);
		int questAvailableCount = availableArray.Num();
		questCountStr = FString::Printf(TEXT("(%d)"), questAvailableCount);
	}

	object->SetString(TEXT("quest_caption"), MMOUI::ParseLocalizedPropertyPath(FString(TEXT("Properties.EQUEST_TYPE.")) + questTypeName) + questCountStr);
	questArray->SetElementObject(index, object);
	++ index;

	// Sub-list item
	INT playerLevel = MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->LevelAttributes.Level;
	INT lowLevelDiffCfg = 0;
	GConfig->GetInt(TEXT("Quest"), TEXT("LowLevelDiff"), lowLevelDiffCfg, GGameIni);

	for (INT i = 0; i < questList.Num(); ++ i)
	{
		const UQuest* quest = questList(i);
		if (!quest)
		{
			continue;
		}
		
		object = GFXPlayer->CreateObject(TEXT("ActionScript3.Quest.QuestData"));
		if (!object)
		{
			continue;
		}

		// Main quests go under story quests
		if (quest->QuestType == E_qt_Trunk)
		{
			continue;
		}

		// limit level check
		if (!_ShowLowLvQuest)
		{
			if (playerLevel - quest->QuestLevel > lowLevelDiffCfg)
			{
				continue;
			}
		}

		UBOOL isQuestAvailable = MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID());
		UBOOL isQuestAccepted = MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID());
		UBOOL isQuestCompassable = MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID());
		UBOOL isQuestCompleted = MMOQuestData::getSingleton().IsQuestCompleted(quest->GetTemplateID());
		
		object->SetInt(TEXT("quest_item_type"), EQIT_Title);
		object->SetInt(TEXT("quest_type"), questType);
		object->SetInt(TEXT("quest_id"), quest->QuestTempID);

		FString questCaption = TEXT("");
		static UEnum* questEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("EQUEST_TYPE"), TRUE);
		FString enumVal = questEnum->GetEnum(quest->QuestType).ToString();
		questCaption += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.QuestTitleDisplay_QuestType.")) + enumVal);
		questCaption += quest->QuestName;
		object->SetString(TEXT("quest_caption"), questCaption);
		object->SetString(TEXT("quest_level"), FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.QuestTitleDisplay_QuestType.Lv")), quest->QuestLevel));
		object->SetBool(TEXT("quest_is_accepted"), isQuestAccepted);
		object->SetBool(TEXT("quest_is_available"), isQuestAvailable);
		object->SetBool(TEXT("quest_is_compassable"), isQuestCompassable);
		object->SetBool(TEXT("quest_is_completed"),isQuestCompleted);
		object->SetBool(TEXT("quest_is_traceable"), _CurQuestStateGroup == EQS_Accept);
		object->SetBool(TEXT("quest_is_select"), quest->GetTemplateID() == _CurQuestId);
		object->SetBool(TEXT("quest_can_abort"), quest->CanAbandon && _CurQuestStateGroup == EQS_Accept);
		object->SetBool(TEXT("quest_can_share"), quest->CanShare && _CurQuestStateGroup == EQS_Accept);

		const AcceptedQuestData* acceptedData = MMOQuestData::getSingleton().GetAcceptedQuestData(quest->GetTemplateID());
		if (acceptedData)
		{
			object->SetBool(TEXT("quest_is_failed"),acceptedData->IsFailed);
			object->SetBool(TEXT("quest_is_traced"), acceptedData->IsTraced);
		}

		questArray->SetElementObject(index, object);
		++ index;
	}
}

void UMMOUIQuestPanel::UpdateQuestList()
{
	UGFxObject* questArray = GFXPlayer->CreateArray();
	if (!questArray)
	{
		return;
	}

	int index = 0;
	
	QuestMapDataMap questMapDataMap = MMOQuestData::getSingleton().GetQuestMapDataMap();
	QuestMapDataMap::iterator iter = questMapDataMap.begin();
	for (; iter != questMapDataMap.end(); ++ iter)
	{
		_UpdateQuestSubListByMap(iter->first, questArray, index);
	}

	//_UpdateQuestSubList(E_qt_Trunk, questArray, index);
	//_UpdateQuestSubList(E_qt_Branch, questArray, index);
	//_UpdateQuestSubList(E_qt_Daily, questArray, index);
	//_UpdateQuestSubList(E_qt_Reward, questArray, index);
	//_UpdateQuestSubList(E_qt_Circle, questArray, index);
	//_UpdateQuestSubList(E_qt_Guild, questArray, index);
	//_UpdateQuestSubList(E_qt_Master, questArray, index);
	//_UpdateQuestSubList(E_qt_Guide, questArray, index);
	
	// Adventure and activity quests are shown separately
	if (_CurQuestStateGroup == EQS_Accept)
	{
		_UpdateQuestSubListByType(E_qt_Adventure, questArray, index);
		_UpdateQuestSubListByType(E_qt_Emegency, questArray, index);
	}
	_UpdateQuestSubListByType(E_qt_Activity, questArray, index);

	INT complete = MMOQuestData::getSingleton().GetCompletedQuestCount();
	AcceptedQuestDataMap acceptedQuests = MMOQuestData::getSingleton().GetAcceptedQuests();
	INT total = acceptedQuests.size() + MMOQuestData::getSingleton().GetAvailableQuestCount() + complete;
	eventCallUpdateQuestList(questArray, complete, total);

	//  #20829
	if (index == 0)
	{
		_CurQuestId = INVALID_QUEST_ID;
	}
}

void UMMOUIQuestPanel::UpdateQuestDetail()
{
	if (_CurQuestStateGroup == EQS_Null)
	{
		eventCallUpdateQuestDetail(NULL);
		return;
	}

	if (_CurQuestId == INVALID_QUEST_ID)
	{
		eventCallUpdateQuestDetail(NULL);
		return;
	}
	
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(_CurQuestId);
	if (!quest)
	{
		eventCallUpdateQuestDetail(NULL);
		return;
	}	

	UGFxObject* object = MMOQuestManager::getSingleton().GetQuestDetailData(_CurQuestId, GFXPlayer, FALSE);
	eventCallUpdateQuestDetail(object);
}

//////////////////////////////////////////////////////////////////////////


void UMMOUIQuestPanel::ASUpdateQuestList(INT group)
{
	_CurQuestStateGroup = group;
	_CurQuestId = INVALID_QUEST_ID;
	UpdateQuestList();
	UpdateQuestDetail();
}

void UMMOUIQuestPanel::ASUpdateQuestDetail(INT id)
{
	_CurQuestId = id;
	UpdateQuestDetail();
}

void UMMOUIQuestPanel::ASTraceQuest(INT id, UBOOL traced)
{
	MMOQuestManager::getSingleton().TraceQuest(id, traced == TRUE);
}

void UMMOUIQuestPanel::ASTraceAllQuest(UBOOL traced)
{
	MMOQuestManager::getSingleton().TraceAllQuest(traced == TRUE);
}

void UMMOUIQuestPanel::ASFilterLowLevelQuest(UBOOL showLowLvQuest)
{
	_ShowLowLvQuest = showLowLvQuest;
	UpdateQuestList();
}

void UMMOUIQuestPanel::ASShareQuest(INT id)
{
	const UQuest* questData = MMOQuestData::getSingleton().GetQuestData(id);
	if (questData && questData->CanShare)
	{
		CMessageHandler::GetInstance().Send_ShareQuest(id); 
	}
}

void UMMOUIQuestPanel::AbortQuest(INT id)
{
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
	CMessageHandler::GetInstance().Send_AbandonQuest(id);
}

void UMMOUIQuestPanel::ASAbortCurQuest(INT id)
{
	if(id == INVALID_QUEST_ID)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(id);
	if (!quest)
	{
		return;
	}

	if(!MMOQuestData::getSingleton().IsQuestAccepted(id))
	{
		return;
	}

	if (!quest->CanAbandon)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CanNotAbandon"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Quest.CanNotAbandon"));
		return;
	}

	FString text =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AborbQuestConfirm"));
	text = FString::Printf(*text, *UITextParser::HTMLFormatQuestName(quest));

	FScriptDelegate funcOK(EC_EventParm);
	funcOK.FunctionName = TEXT("AbortQuestOK");
	funcOK.Object = this;

	FScriptDelegate funcCanel(EC_EventParm);
	funcCanel.FunctionName = TEXT("AbortQuestCanel");
	funcCanel.Object = this;

	MMOMsgBoxManager::getSingleton().AddMessageBox(text, id, funcOK, funcCanel);
}

void UMMOUIQuestPanel::ASAcceptQuestWithoutNpc(INT id)
{
	if (id == INVALID_QUEST_ID)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(id);
	if (!quest)
	{
		return;
	}

	UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
	if (dialog)
	{
		dialog->TryPopupQuestWithoutNpc(id);
	}
}

void UMMOUIQuestPanel::ASCompleteQuestWithoutNpc(INT id)
{
	if (id == INVALID_QUEST_ID)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(id);
	if (!quest)
	{
		return;
	}

	INT selectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(id);
	MMOQuestManager::getSingleton().CompleteQuest(quest->GetTemplateID(), selectedAwardIndex);
}