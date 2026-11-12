#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "TeamManager.h"
#include "ActivityManager.h"

IMPLEMENT_CLASS(UMMOUITeamChangeTargetPanel)

void UMMOUITeamChangeTargetPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		UGFxObject* dropDownState = GFXPlayer->CreateObject(TEXT("Object"));
		dropDownState->SetInt(TEXT("SelectedIndex"), CurDifficulty);
		dropDownState->SetString(TEXT("StringData"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TargetDifficultyDropDown")));
		if(CurTab <= 0)
		{
			UpdateTeamTargetListActivityFB();
			dropDownState->SetBool(TEXT("EnableDropDown"), FALSE);
		}
		else if(CurTab == 1)
		{
			UpdateTeamTargetListActivity();
			dropDownState->SetBool(TEXT("EnableDropDown"), FALSE);
		}
		else if(CurTab == 2)
		{
			UpdateTeamTargetListFB(CurDifficulty);
			dropDownState->SetBool(TEXT("EnableDropDown"), TRUE);
		}

		eventUpdateDifficultyFilterDropDown(dropDownState);
	}
	else
	{

	}
}

void UMMOUITeamChangeTargetPanel::UpdateTeamTargetListActivityFB()
{
	CurTab = 0;
	INT idx = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	SetActivityTargetList(dataProvider, idx);
	SetFBTargetList(dataProvider, idx, -1);
	eventUpdateTargetList(dataProvider);
}

void UMMOUITeamChangeTargetPanel::UpdateTeamTargetListActivity()
{
	CurTab = 1;
	INT idx = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	SetActivityTargetList(dataProvider, idx);
	eventUpdateTargetList(dataProvider);
}

void UMMOUITeamChangeTargetPanel::UpdateTeamTargetListFB(INT nDifficulty)
{
	CurTab = 2;
	//CurDifficulty = nDifficulty;
	CurDifficulty = 3;//only the extreme difficulty is opened
	INT idx = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	SetFBTargetList(dataProvider, idx, nDifficulty);
	eventUpdateTargetList(dataProvider);
}

void UMMOUITeamChangeTargetPanel::SetActivityTargetList(UGFxObject* dataProvider, INT& idx)
{
	FString level_format = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.LevelFormat"));

	const ActivityDataList& activityData = ActivityManager::getSingleton().GetCompleteActivityDataList();
	ActivityDataList::const_iterator itr  = activityData.begin();
	for (; itr != activityData.end(); ++itr)
	{
		if((*itr).nPersonNumMin == 1)
			continue;
		UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
		//Activities have no difficulty, set to easiest for now
		data->SetInt(TEXT("difficulty"),0);
		data->SetString(TEXT("text_0"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TargetType_Activity")));
		data->SetString(TEXT("text_1"), itr->sActivityName);
		data->SetString(TEXT("text_2"), FString::Printf(*level_format, itr->nCheckLevelMin, itr->nCheckLevelMax));
		data->SetInt(TEXT("TargetId"), itr->nActivityID);
		data->SetInt(TEXT("TargetType"), 0);
		dataProvider->SetElementObject(idx++, data);
	}
}

void UMMOUITeamChangeTargetPanel::SetFBTargetList(UGFxObject* dataProvider, INT& idx, INT nDifficulty)
{
	FString level_format = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.LevelFormat"));

	const FBActivityDataList& fbData = ActivityManager::getSingleton().GetFBActivityDataList();
	FBActivityDataList::const_iterator itor = fbData.begin();
	for(; itor != fbData.end(); ++itor)
	{
		if(itor->nPersonNumMin <= 1)
			continue;
		if(nDifficulty < 0 || (itor->nFBDifficulty - 2 == nDifficulty) )
		{
			UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
			data->SetInt(TEXT("difficulty"),itor->nFBDifficulty - 2);
			data->SetString(TEXT("text_0"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.TargetType_FB")));
			data->SetString(TEXT("text_1"), itor->sFBName);
			data->SetString(TEXT("text_2"), FString::Printf(*level_format, itor->nCheckLevelMin, itor->nCheckLevelMax));
			data->SetInt(TEXT("TargetId"), itor->nFBID);
			data->SetInt(TEXT("TargetType"), 1);
			dataProvider->SetElementObject(idx++, data);
		}
	}
}

void UMMOUITeamChangeTargetPanel::ASClickChangeTargetOk(INT targetType, INT targetId)
{
	if(targetId >= 0 && targetType >= 0)
	{
		if(TeamManager::getSingleton().HasTeam())
		{
			CMessageHandler::GetInstance().Send_ChangeTeamActivity(targetId, targetType + 1);
		}
		else
		{
			TeamManager::getSingleton().SetTeamTargetType(targetType + 1);
			TeamManager::getSingleton().SetTeamTargetId(targetId);
			MMOGameEvent_TeamCommon evt(MMOGameEvent_TeamCommon::ETeamEventType_TargetChange);
			MMOUI::PostGameEvent(&evt);
		}

	}
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamTarget, FALSE, "");
}

void UMMOUITeamChangeTargetPanel::ASCancelChangeTarget()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamTarget, FALSE, "");
}

void UMMOUITeamChangeTargetPanel::ASShowActivityFBTargetList()
{

	UpdateTeamTargetListActivityFB();
	
}

void UMMOUITeamChangeTargetPanel::ASShowActivityTargetList()
{
	
	UpdateTeamTargetListActivity();
	
}

void UMMOUITeamChangeTargetPanel::ASShowFBTargetList(INT nDifficulty)
{
	UpdateTeamTargetListFB(nDifficulty);

}