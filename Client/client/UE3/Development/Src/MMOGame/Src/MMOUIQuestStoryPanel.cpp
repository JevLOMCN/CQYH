#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "TeamManager.h"
#include "MMOMapDataManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIQuestStoryPanel)

void UMMOUIQuestStoryPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Quest);
	EventAdapter = new TGameEventAdapter<UMMOUIQuestStoryPanel>(this, evts);

	_CurQuestId = INVALID_QUEST_ID;
}

void UMMOUIQuestStoryPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.StoryQuest")));
		_CurQuestId = MMOQuestManager::getSingleton().GetCurTrunkQuestId();
		UpdateCurQuestInfo(_CurQuestId);
		MMOFacade::getSingleton().PlayUISound(TEXT("TaskSheet_Op"));
	}
	else
	{
		MMOFacade::getSingleton().PlayUISound(TEXT("TaskSheet_Cls"));
	}
}

void UMMOUIQuestStoryPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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

	switch(questEvent->EventType)
	{
	case MMOGameEvent_Quest::EQET_AcceptQuest:
		{
			_CurQuestId = questEvent->QuestId;
			UpdateCurQuestInfo(_CurQuestId);
		}
		break;
	}
}

void UMMOUIQuestStoryPanel::ItemChange(INT ContainerID,INT SlotID)
{
	if (!IsVisible)
	{
		return;
	}
	UpdateCurQuestInfo(_CurQuestId);
}

void UMMOUIQuestStoryPanel::ItemDelete(INT ContainerID, INT SlotID)
{
	if (!IsVisible)
	{
		return;
	}
	UpdateCurQuestInfo(_CurQuestId);
}

void UMMOUIQuestStoryPanel::PlayerLevelChange( INT lastLv , INT newLv )
{
	if (!IsVisible)
	{
		return;
	}
	UpdateCurQuestInfo(_CurQuestId);
}

void UMMOUIQuestStoryPanel::UpdateCurQuestInfo(int questId)
{
	// #23806
	TArray<UQuest*> acceptedArray = MMOQuestData::getSingleton().GetAcceptedQuestsByType(E_qt_Trunk);
	if (acceptedArray.Num() == 0)
	{
		return;
	}
	
	const UQuest* quest = (questId == INVALID_QUEST_ID) ? acceptedArray(0) : MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	UGFxObject* object = MMOQuestManager::getSingleton().GetQuestDetailData(quest->QuestTempID, GFXPlayer, FALSE);
	if (object)
	{
		eventCallUpdateQuestStoryDetailInfo(object);
	}
}

void UMMOUIQuestStoryPanel::UpdateQuestChapterList()
{
	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	int index = 0;
	FString chapterList = TEXT("");
	StoryQuestChapterList storyQuestChapterList = MMOQuestData::getSingleton().GetStoryQuestChapterList();
	StoryQuestChapterList::iterator iterChapter = storyQuestChapterList.begin();
	for (; iterChapter != storyQuestChapterList.end(); ++ iterChapter)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("quest_chapter"), *iterChapter);
			object->SetString(TEXT("quest_chapter_name"), FString::Printf(TEXT("Chapter %d: "), *iterChapter));
		}

		objectArray->SetElementObject(index, object);
		++ index;
	}

	eventCallUpdateQuestStoryChapterList(objectArray);
}

bool less_stage(const UINT32& stage1, const UINT32& stage2)
{
	return stage1 < stage2;
}

void UMMOUIQuestStoryPanel::UpdateQuestStageList(UINT32 chapter)
{
	StoryQuestChapterStageMap storyQuestChapterStageMap = MMOQuestData::getSingleton().GetStoryQuestChapterStageMap();
	StoryQuestChapterStageMap::iterator iterChapter = storyQuestChapterStageMap.find(chapter);
	if (iterChapter != storyQuestChapterStageMap.end())
	{
		UGFxObject* objectArray = GFXPlayer->CreateArray();
		if (objectArray)
		{
			int index = 0;
			StoryQuestStageList storyQuestStageList = iterChapter->second;
			storyQuestStageList.sort(less_stage);
			StoryQuestStageList::iterator iterStage = storyQuestStageList.begin();
			for (; iterStage != storyQuestStageList.end(); ++ iterStage)
			{
				StoryQuestData data;
				if (!MMOQuestData::getSingleton().GetStoryQuestData(iterChapter->first, *iterStage, data))
				{
					continue;
				}

				UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
				if (object)
				{
					object->SetString(TEXT("label"), data.StageName);
					object->SetInt(TEXT("index"), index);
					object->SetInt(TEXT("type"), 0);
					object->SetInt(TEXT("deepth"), 0);
					object->SetBool(TEXT("visible"), true);

					object->SetInt(TEXT("quest_chapter"), data.Chapter);
					object->SetInt(TEXT("quest_stage"), data.Stage);

					FString stageName = FString::Printf(TEXT("Act %d: "), *iterStage);
					stageName += data.StageName;
					object->SetString(TEXT("quest_stage_name"), stageName);

					objectArray->SetElementObject(index, object);
					++ index;
				}
			}

			eventCallUpdateQuestStoryStageList(objectArray);
		}
	}
}

void UMMOUIQuestStoryPanel::UpdateQuestStageInfo(UINT32 chapter, UINT32 stage)
{
	StoryQuestData data;
	if (MMOQuestData::getSingleton().GetStoryQuestData(chapter, stage, data))
	{
		eventCallUpdateQuestStoryStageInfo(data.StageName, data.StageDesc);
	}
	else
	{
		eventCallUpdateQuestStoryStageInfo(TEXT(""), TEXT(""));
	}
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIQuestStoryPanel::ASUpdateCurQuestInfo()
{
	UpdateCurQuestInfo(_CurQuestId);
}

void UMMOUIQuestStoryPanel::ASUpdateQuestChapterList()
{
	UpdateQuestChapterList();
}

void UMMOUIQuestStoryPanel::ASUpdateQuestStageList(INT chapter)
{
	// chapter quests currently start from chapter 4, temporarily offset for now
	UpdateQuestStageList(chapter);
}

void UMMOUIQuestStoryPanel::ASViewStoryQuestStageInfo(INT chapter, INT stage)
{
	UpdateQuestStageInfo(chapter, stage);
}
