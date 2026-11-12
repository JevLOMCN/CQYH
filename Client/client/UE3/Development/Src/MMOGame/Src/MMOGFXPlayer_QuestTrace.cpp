#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_QuestTrace)

void UMMOGFXPlayer_QuestTrace::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_QuestTrace::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_QuestTrace);
}

void UMMOGFXPlayer_QuestTrace::ASUpdateAcceptedQuestTraceList()
{
	if (_QuestTracePanel)
	{
		_QuestTracePanel->_QuestState = EQS_Accept;
		_QuestTracePanel->UpdateAcceptedQuestTraceList();
	}
}

void UMMOGFXPlayer_QuestTrace::ASUpdateAvailableQuestTraceList()
{
	if (_QuestTracePanel)
	{
		_QuestTracePanel->_QuestState = EQS_Available;
		_QuestTracePanel->UpdateAvailableQuestTraceList();
	}
}

// Flash has various sync problems, changed to C++ timing
/*
void UMMOGFXPlayer_QuestTrace::ASUpdateQuestTimeout(INT questId, INT timeSecondsLeft)
{
	MMOQuestManager::getSingleton().UpdateQuestTime(questId, timeSecondsLeft);
}

void UMMOGFXPlayer_QuestTrace::ASOnTimeLimitQuestTimeout(INT questId)
{
	MMOQuestManager::getSingleton().OnQuestTimeUp(questId);
}
*/

void UMMOGFXPlayer_QuestTrace::ASUseQuestItem(int questId, int itemId)
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

	// use item to complete quest
	if (quest->FinishItem)
	{
		INT itemSlot = -1;
		FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(itemId, E_CONSLOT_DEFAULT, itemSlot);
		if (itemInfo && itemSlot >= 0)
		{
			CMessageHandler::GetInstance().Send_UseItem(E_CONSLOT_DEFAULT, itemSlot, itemId);
		}
		else
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CanNotFindQuestItem"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Quest.CanNotFindQuestItem"));
		}
	}
}

void UMMOGFXPlayer_QuestTrace::ASDelayComplete(int questId)
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

	INT selectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(questId);
	MMOQuestManager::getSingleton().CompleteQuest(questId, selectedAwardIndex);
}