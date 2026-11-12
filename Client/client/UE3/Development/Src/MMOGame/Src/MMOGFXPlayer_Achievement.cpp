#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOAchievementManager.h"


IMPLEMENT_CLASS(UMMOGFXPlayer_Achievement)

void UMMOGFXPlayer_Achievement::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Achievement::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Achievement);
}

void UMMOGFXPlayer_Achievement::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_Achievement::ASUpdateCagetoryData(INT type)
{
	if (_AchievementPanel)
	{
		_AchievementPanel->UpdateAchievementCagetory(type);
	}
}

void UMMOGFXPlayer_Achievement::ASUpdateSummaryData(INT type)
{
	if (_AchievementPanel)
	{
		_AchievementPanel->UpdateAchievementSummary(type);
	}
}

void UMMOGFXPlayer_Achievement::ASUpdateAchievementData(INT baseClass, INT subClass)
{
	if (_AchievementPanel)
	{
		_AchievementPanel->UpdateAchievementData(baseClass, subClass);
	}
}

void UMMOGFXPlayer_Achievement::ASUpdateAchievementList(INT baseClass, INT subClass)
{
	if (_AchievementPanel)
	{
		_AchievementPanel->UpdateAchievementList(baseClass, subClass);
	}
}

void UMMOGFXPlayer_Achievement::ASShowAchievementListByClass(INT baseClass, INT subClass)
{
	if (_AchievementPanel)
	{
		_AchievementPanel->ShowAchievementListByClass(baseClass, subClass);
	}
}

void UMMOGFXPlayer_Achievement::ASShowAchievementListById(INT id)
{
	if (_AchievementPanel)
	{
		_AchievementPanel->ShowAchievementListById(id);
	}
}

void UMMOGFXPlayer_Achievement::ASShowAchievementPanelById(INT id)
{
	if (MMOUI::GetUIManager())
	{
		UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Achievement);
		if (panel)
		{
			panel->Show(TRUE);
			panel->ShowAchievementListById(id);
		}
	}
}

void UMMOGFXPlayer_Achievement::ASObtainRewardItem(INT id)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (mainPlayer->eventIsDead())
	{
		FString text = TEXT("Cannot claim rewards while dead");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return;
	}

	if (MMOAchievementData::getSingleton().IsAchievementRewardObtain(id))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAchievementRewarded"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return;
	}

	if (id < ACHIEVEMENT_MAX)
	{
		CMessageHandler::GetInstance().Send_ObtainRewardItem(id); 
	}
	else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
		CMessageHandler::GetInstance().Send_ObtainGuildReward(dataId);
	}
	else
	{
		UINT16 killId = HIWORD(id);
		UINT8 killIndex = LOWORD(id);
		CMessageHandler::GetInstance().Send_KillNpcAchievementComplete(killId, killIndex); 
	}
}

void UMMOGFXPlayer_Achievement::ASPlayAchievementHintEnd()
{
	if (_AchievementHintPanel)
	{
		_AchievementHintPanel->PlayAchievementHintEnd();
	}
}