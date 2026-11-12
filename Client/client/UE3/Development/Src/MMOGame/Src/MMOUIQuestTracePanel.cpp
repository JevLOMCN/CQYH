#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"

IMPLEMENT_CLASS(UMMOUIQuestTracePanel)

void UMMOUIQuestTracePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Quest);
	evts.AddItem(GameEVent_SceneLoadFinish);
	EventAdapter = new TGameEventAdapter<UMMOUIQuestTracePanel>(this, evts);
}

void UMMOUIQuestTracePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		// This panel is no longer needed
		Show(FALSE);
		return;

		if (!_IsExpand)
		{
			Show(FALSE);
			return;
		}

		UpdatePosition();
	}
}

void UMMOUIQuestTracePanel::OnFocusGained()
{
	UMMOUIBase::OnFocusGained();	
	TArray<struct FASValue> args;
	Invoke(TEXT("UCSetFocus"), args);	
}

void UMMOUIQuestTracePanel::OnFocusLost()
{
	UMMOUIBase::OnFocusLost();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if (ctrl)
	{
		ctrl->FocusOnInput(this);
	}

}

void UMMOUIQuestTracePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (!IsVisible)
	{
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Quest:
		{
			MMOGameEvent_Quest* questEvt = (MMOGameEvent_Quest*)evt;
			if(questEvt == NULL)
			{
				return;
			}

			switch(questEvt->EventType)
			{
			case MMOGameEvent_Quest::EQET_AcceptQuest:
				{
					//AddAcceptedQuest(questEvt->QuestId);
					UpdateAcceptedQuestTraceList();
				}
				break;
			case MMOGameEvent_Quest::EQET_UpdateQuest:
			case MMOGameEvent_Quest::EQET_UpdateQuestTime:
				{
					UpdateAcceptedQuest(questEvt->QuestId);
				}
				break;
			case MMOGameEvent_Quest::EQET_AbandonQuest:
			case MMOGameEvent_Quest::EQET_CompleteQuest:
				{
					//RemoveAcceptedQuest(questEvt->QuestId);
					UpdateAcceptedQuestTraceList();
				}
				break;
			case MMOGameEvent_Quest::EQET_UpdateQuestTraceList:
				{
					UpdateQuestTraceList();
				}
			}
		}
		break;

	case GameEVent_SceneLoadFinish:
		{
			UpdateQuestTraceList();
		}
		break;
	}
}

void UMMOUIQuestTracePanel::UpdatePosition()
{
	UMMOUIMinimap* minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Minimap);
	if (minimapPanel == NULL)
	{
		return;
	}

	FLOAT offsetVer;
	if (minimapPanel->IsReputationTrace)
	{
		offsetVer = minimapPanel->PanelBottomWithReputation;
	}
	else
	{
		offsetVer = minimapPanel->PanelBottomWithoutReputation;
	}

	FVector2D newPos;
	GetPosition(newPos.X, newPos.Y);

	UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
	if (fbTaskPanel && fbTaskPanel->IsVisible)
	{
		FVector2D fbTaskPanelPos;
		fbTaskPanel->GetPosition(fbTaskPanelPos.X, fbTaskPanelPos.Y);
		offsetVer = fbTaskPanelPos.Y + fbTaskPanel->GetFloat(TEXT("height"));
	}

	newPos.Y = offsetVer + 2;
	SetPosition(newPos.X, newPos.Y);
}

void UMMOUIQuestTracePanel::UpdateQuestTraceList()
{
	/*
	const QuestTraceList& questTraceList = MMOQuestData::getSingleton().GetAcceptedQuestTraceList();
	UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_DialogNew);
	if (dialog && !dialog->IsVisible && questTraceList.empty())
	{
		eventCallSetQuestTraceType(EQS_Available);
	}
	else
	{
		eventCallSetQuestTraceType(EQS_Accept);
	}
	*/
	eventCallSetQuestTraceType(EQS_Accept);
}

void UMMOUIQuestTracePanel::AddAcceptedQuest(INT questId)
{
	UGFxObject* object = MMOQuestManager::getSingleton().GetQuestTraceData(questId, GFXPlayer, EQTLT_Accepted);
	if (object)
	{
		eventCallAddAcceptedQuest(object);
	}
}

void UMMOUIQuestTracePanel::RemoveAcceptedQuest(INT questId)
{
	eventCallRemoveAcceptedQuest(questId);
}

void UMMOUIQuestTracePanel::UpdateAcceptedQuest(INT questId)
{
	UGFxObject* object = MMOQuestManager::getSingleton().GetQuestTraceData(questId, GFXPlayer, EQTLT_Accepted);
	if (object)
	{
		eventCallUpdateAcceptedQuest(object);
	}
}

void UMMOUIQuestTracePanel::UpdateAcceptedQuestTraceList()
{
	if (_QuestState != EQS_Accept)
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	const QuestTraceList& questTraceList = MMOQuestData::getSingleton().GetAcceptedQuestTraceList();
	QuestTraceList::const_iterator iter = questTraceList.begin();
	for(; iter != questTraceList.end(); ++ iter)
	{
		UGFxObject* object = MMOQuestManager::getSingleton().GetQuestTraceData(*iter, GFXPlayer, EQTLT_Accepted);
		if (object)
		{
			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAcceptedQuestTraceList(objectArray);
}

void UMMOUIQuestTracePanel::UpdateAvailableQuestTraceList()
{
	if (_QuestState != EQS_Available)
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	const QuestTraceList& questTraceList = MMOQuestData::getSingleton().GetAvailableQuestTraceList();
	QuestTraceList::const_iterator iter = questTraceList.begin();
	for(; iter != questTraceList.end(); ++ iter)
	{
		UGFxObject* object = MMOQuestManager::getSingleton().GetQuestTraceData(*iter, GFXPlayer, EQTLT_Available);
		if (object)
		{
			objectArray->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAvailableQuestTraceList(objectArray);
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIQuestTracePanel::ASUpdateAcceptedQuestTraceList()
{
	_QuestState = EQS_Accept;
	UpdateAcceptedQuestTraceList();
}

void UMMOUIQuestTracePanel::ASUpdateAvailableQuestTraceList()
{
	_QuestState = EQS_Available;
	UpdateAvailableQuestTraceList();
}

// Flash has various sync problems, changed to C++ timing
/*
void UMMOUIQuestTracePanel::ASUpdateQuestTimeout(INT questId, INT timeSecondsLeft)
{
	MMOQuestManager::getSingleton().UpdateQuestTime(questId, timeSecondsLeft);
}

void UMMOUIQuestTracePanel::ASOnTimeLimitQuestTimeout(INT questId)
{
	MMOQuestManager::getSingleton().OnQuestTimeUp(questId);
}
*/

void UMMOUIQuestTracePanel::ASUseQuestItem(int questId, int itemId)
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

void UMMOUIQuestTracePanel::ASDelayComplete(int questId)
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