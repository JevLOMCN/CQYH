#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "ActivityManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "TeamManager.h"
#include "MMOMapDataManager.h"
#include "MMOQuestManager.h"
#include "client_option_def.h"


IMPLEMENT_CLASS(UMMOUIActivityQuitPanel)


void UMMOUIActivityQuitPanel::Init()
{
	Super::Init();
	
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ActivityActive);
	evts.AddItem(GameEvent_ActivityRewardState);
	EventAdapter = new TGameEventAdapter<UMMOUIActivityQuitPanel>(this, evts);
	
}

void UMMOUIActivityQuitPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
#if 0
		// version 2.6 temporarily changed to exit the game directly (defect: 11644)
		MMOUI::GetUIManager()->SendQuitGame();
		Show(FALSE);
#endif

#if 1
		UpdateQuitDataPanel();
		FVector2D scrSz = MMOFacade::GetScreenSize();
		SetPosition((scrSz.X - eventGetWidth())/2,(scrSz.Y - eventGetHeight())/2);
#endif
		eventCallShowRewardBar(CMessageHandler::GetInstance().GetClientOption() & CLIENT_OPTION_BIT_ACTIVITY_VALUE);
	}
}

void UMMOUIActivityQuitPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if(gameEvent == NULL){
		return;
	}

	switch(gameEvent->GetEventType())
	{
	case GameEvent_ActivityActive:
		{
			UpdateQuitActivityValue();
		}break;
	case GameEvent_ActivityRewardState:
		{
			UpdateQuitRewardItem();
		}break;
	}
}

void  UMMOUIActivityQuitPanel::UpdateQuitDataPanel()
{
	ActivityManager::getSingleton().FilterTodayActivity();
	UpdateQuitActivityList();
	UpdateQuitRewardItem();
	UpdateQuitRememberToday();
	UpdateQuitActivityValue();
	//eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.QuitHintActivityTitle")));
}
void  UMMOUIActivityQuitPanel::UpdateQuitActivityList()
{
	UGFxObject* dataArray = GFXPlayer->CreateArray();
	ActivityManager::getSingleton().SetQuitHintActivityList(GFXPlayer,dataArray);
	if(dataArray)
		eventCallUpdateActivityQuitList(dataArray);
}
void  UMMOUIActivityQuitPanel::UpdateQuitRewardItem()
{
	UGFxObject* dataArray = GFXPlayer->CreateArray();
	if (dataArray == 0)
		return;
	for (INT n = 0; n < 5; ++n)
	{
		UGFxObject* object = GFXPlayer->CreateObject("UI_ActivityWidget.ActivityAS3.ActivityRewardBarItemData");
		if(object)
		{
			const RewardBarItem* pkItem = ActivityManager::getSingleton().GetRewardBarCondition(n);
			if(pkItem)
			{
				object->SetInt("nState",ActivityManager::getSingleton().GetRewardState(n));
				object->SetInt("nType",pkItem->nBoxLevel);
				FString strInfo = FString::Printf(TEXT("%d"),pkItem->nVitality);
				object->SetString("condition",strInfo);
				dataArray->SetElementObject(n,object);
			}
		}
	}
	eventCallUpdateQuitActivityReward(dataArray);

}
void  UMMOUIActivityQuitPanel::UpdateQuitActivityValue()
{
	FString strInfo = FString::Printf(TEXT("%d"),ActivityManager::getSingleton().GetActiveValue());
	eventCallUpdateQuitActivityValue(strInfo);
}
void UMMOUIActivityQuitPanel::SortActivity(INT nType)
{
	if(nCurSortType == nType)
		nSortAdd = nSortAdd?0:1;
	else
	{
		nCurSortType = nType;
		nSortAdd = 0;
	}
	ActivityManager::getSingleton().SortTodayActivityList(nCurSortType,nSortAdd);
	UpdateQuitActivityList();
}
void  UMMOUIActivityQuitPanel::UpdateQuitRememberToday()
{
	UGFxObject* dataArray = GFXPlayer->CreateArray();
	if (dataArray == 0)
		return;
	INT  nIndex = 0;

	INT nAcceptNum = MMOQuestData::getSingleton().GetAcceptedQuests().size();

	INT nCanAccept = MMOQuestData::getSingleton().GetAvailableQuestCount() + nAcceptNum;

	FString conditionDesc = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.TodayQuestHint")),nAcceptNum, nCanAccept);

	dataArray->SetElementString(nIndex ++ ,conditionDesc);

	conditionDesc = FString::Printf(TEXT("<font color='#14a800'>%s</font>"), *LinkManager::HtmlLink_OpenPanel(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.ViewQuest")),TEXT("Quest")));

	dataArray->SetElementString(nIndex ++ ,conditionDesc);

	INT nPowerValue = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_FATIGUE);
	FString strPower = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.ActiveValueHint")),nPowerValue);

	dataArray->SetElementString(nIndex ++ ,strPower);
	eventCallUpdateRememberContent(dataArray);
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIActivityQuitPanel::ASClickQuitListHeadSort(INT type)
{
	SortActivity(type);
}
void UMMOUIActivityQuitPanel::ASClickOpenActivityPanel()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Activity,TRUE, "");
}

void UMMOUIActivityQuitPanel::ASClickBackGame()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE, "");
}
void UMMOUIActivityQuitPanel::ASClickBackLogin()
{
	MMOUI::GetUIManager()->SendReturnLogin();
}
void UMMOUIActivityQuitPanel::ASClickQuitGame()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_ActivityQuit,FALSE, "");
	MMOUI::GetUIManager()->SendQuitGame();

	//UMMOUIActivityQuitPanel* UI = (UMMOUIActivityQuitPanel*)MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_ActivityQuit);
	//if (UI)
	//    UI->Show(FALSE);
}
void UMMOUIActivityQuitPanel::ASClickQuitGetReward(INT nIndex)
{
	INT nState = ActivityManager::getSingleton().GetRewardState(nIndex);
	if(nState == 1){
		const RewardBarItem* rewardData = ActivityManager::getSingleton().GetRewardBarCondition(nIndex);
		if( rewardData != NULL ){
			CMessageHandler::GetInstance().Send_GetActivityReward(rewardData->nID);
		}
	}
	else if (nState == 0)
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.rewardNoGetHint")),MsgInfoType_Table,TEXT("MMOGame.Activity.rewardNoGetHint"));
	else if (nState == 2)
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.rewardHaveGetHint")),MsgInfoType_Table,TEXT("MMOGame.Activity.rewardHaveGetHint"));
}