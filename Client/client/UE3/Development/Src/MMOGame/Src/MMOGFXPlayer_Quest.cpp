#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Quest)


void UMMOGFXPlayer_Quest::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_Quest::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Quest);
}

void UMMOGFXPlayer_Quest::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_Quest::ASUpdateQuestList(INT group)
{
	_QuestPanel->_CurQuestStateGroup = group;
	_QuestPanel->UpdateQuestList();
}

void UMMOGFXPlayer_Quest::ASUpdateQuestDetail(INT id)
{
	_QuestPanel->_CurQuestId = id;
	_QuestPanel->UpdateQuestDetail();
}

void UMMOGFXPlayer_Quest::ASTraceQuest(INT id, UBOOL traced)
{
	MMOQuestManager::getSingleton().TraceQuest(id, traced == TRUE);
}

void UMMOGFXPlayer_Quest::ASTraceAllQuest(UBOOL traced)
{
	MMOQuestManager::getSingleton().TraceAllQuest(traced == TRUE);
}

void UMMOGFXPlayer_Quest::ASFilterLowLevelQuest(UBOOL showLowLvQuest)
{
	_QuestPanel->_ShowLowLvQuest = showLowLvQuest;
	_QuestPanel->UpdateQuestList();
}

void UMMOGFXPlayer_Quest::ASShareQuest(INT id)
{
	const UQuest* questData = MMOQuestData::getSingleton().GetQuestData(id);
	if (questData && questData->CanShare)
	{
		CMessageHandler::GetInstance().Send_ShareQuest(id); 
	}
}

void UMMOGFXPlayer_Quest::AbortQuest(INT id)
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

void UMMOGFXPlayer_Quest::ASAbortQuest(INT id)
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

void UMMOGFXPlayer_Quest::ASAcceptQuestWithoutNpc(INT id)
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

void UMMOGFXPlayer_Quest::ASCompleteQuestWithoutNpc(INT id)
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

void UMMOGFXPlayer_Quest::ASSelectAward(class UGFxObject* icon,INT idx,INT itemTemplateID)
{
}

void UMMOGFXPlayer_Quest::ASUpdateRewardQuestDetail(INT id)
{
	_QuestRewardPanel->_CurQuestId = id;
	_QuestRewardPanel->UpdateRewardQuestDetail();
}

void UMMOGFXPlayer_Quest::ASUpgradeAvailableLimit()
{
	_QuestRewardBuyTimesPanel->Show(TRUE);
}

void UMMOGFXPlayer_Quest::ASRefreshRewardQuestList(UBOOL autoRefresh)
{
	_QuestRewardPanel->RefreshNowConfirm(autoRefresh);
}

void UMMOGFXPlayer_Quest::ASCompleteAllRewardQuest()
{
	_QuestRewardPanel->CompleteAllConfirm();
}

void UMMOGFXPlayer_Quest::ASShowCompletedRewardQuest(UBOOL show)
{
	_QuestRewardPanel->ShowCompleteQuest(show);
}

void UMMOGFXPlayer_Quest::ASRefreshTopLevelRewardQuest(UBOOL refresh)
{
}

void UMMOGFXPlayer_Quest::ASAcceptRewardQuest(INT id)
{
	MMOQuestManager::getSingleton().AcceptQuest(id, FALSE, 0);
}

void UMMOGFXPlayer_Quest::ASAbandonRewardQuest(INT id)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(id);
	if (quest && quest->QuestType == E_qt_Reward)
	{
		if (quest->StarLevel < 4)
		{
			_QuestRewardPanel->AbandonLowConfirm(id);
		}
		else
		{
			_QuestRewardPanel->AbandonHighConfirm(id);
		}
	}
}

void UMMOGFXPlayer_Quest::ASUpdateRewardQuestBuyTimes(INT times)
{
	_QuestRewardBuyTimesPanel->UpdateRewardQuestBuyTimesConsume(times);
}

void UMMOGFXPlayer_Quest::ASBuyTimes(INT times)
{
	CMessageHandler::GetInstance().Send_UpgradeAwardQuestLimit(times);
}

void UMMOGFXPlayer_Quest::ASUpdateCurQuestInfo()
{
	_QuestStoryPanel->UpdateCurQuestInfo(_QuestStoryPanel->_CurQuestId);
}

void UMMOGFXPlayer_Quest::ASUpdateQuestChapterList()
{
	_QuestStoryPanel->UpdateQuestChapterList();
}

void UMMOGFXPlayer_Quest::ASUpdateQuestStageList(INT chapter)
{
	// chapter quests currently start from chapter 4, temporarily offset for now
	_QuestStoryPanel->UpdateQuestStageList(chapter);
}

void UMMOGFXPlayer_Quest::ASViewStoryQuestStageInfo(INT chapter, INT stage)
{
	_QuestStoryPanel->UpdateQuestStageInfo(chapter, stage);
}

void UMMOGFXPlayer_Quest::ASSubmitItem(INT questId, INT itemId, INT itemIndex)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	// get required attribute
	INT itemColorReq = 0;
	INT itemLevelReq = 0;
	FString itemSubTypeReq = TEXT("");
	for (INT i = 0; i < quest->CheckVar.Num(); ++i)
	{
		FQuestCheckVar checkVar = quest->CheckVar(i);
		if (checkVar.CheckVarType == TEXT("vtItemRecycle") &&
			checkVar.CheckVarID >= 0 && 
			checkVar.CheckVarID < MAX_ROLE_VARIABLE_COUNT)
		{
			QuestRecycleDataList questRecycleList = MMOQuestData::getSingleton().GetQuestRecycleList();
			QuestRecycleDataList::iterator iter = questRecycleList.begin();
			for (; iter != questRecycleList.end(); ++ iter)
			{
				if (iter->VarIndex == checkVar.CheckVarID)
				{
					itemColorReq = iter->ItemColor;
					itemLevelReq = iter->ItemLevel;
					itemSubTypeReq = iter->ItemSubType;
					break;
				}
			}
			break;
		}
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemId);
	if (!item)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item does not exist"), 
			MsgInfoType_Table, 
			TEXT("Item does not exist"));
		return;
	}

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, itemIndex);
	if (!itemInfo)
	{
		FString text = TEXT("MMOGame.EnhanceGeneral.ERROR_ITEM_NOT_FOUND");
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(text), MsgInfoType_Table, text);
		return;
	}

	if (itemInfo->m_nTemplateID != itemId)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item does not exist"), 
			MsgInfoType_Table, 
			TEXT("Item does not exist"));
		return;
	}

	if (item->ItemColor != itemColorReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item quality does not meet the requirement"), 
			MsgInfoType_Table, 
			TEXT("Item quality does not meet the requirement"));
		return;
	}

	if (item->ItemLevel != itemLevelReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item level does not meet the requirement"), 
			MsgInfoType_Table, 
			TEXT("Item level does not meet the requirement"));
		return;
	}

	if (item->ItemSubType != itemSubTypeReq)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Item type does not meet the requirement"), 
			MsgInfoType_Table, 
			TEXT("Item type does not meet the requirement"));
		return;
	}

	CMessageHandler::GetInstance().Send_Recycle(itemId, itemIndex);
}