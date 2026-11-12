#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOAchievementManager.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include <time.h>

IMPLEMENT_CLASS(UMMOUIAchievementHintPanel)

void UMMOUIAchievementHintPanel::Init()
{
	Super::Init();
}

void UMMOUIAchievementHintPanel::PostOnShow(UBOOL v)
{
	//Super::PostOnShow(v);
	if (v)
	{
		if (_CurId > 0)
		{
			FVector2D screenSize = MMOFacade::GetScreenSize();
			INT posX = screenSize.X / 2;
			INT posY = screenSize.Y - 295;
			SetPosition(posX, posY);

			if (_CurId < ACHIEVEMENT_MAX)
			{
				AchievementData data;
				if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(_CurId), data))
				{
					FString name = data.Name;
					if (name.Len() >= 6)
					{
						name = name.Mid(0, 6);
						name += TEXT("...");
					}
					eventCallSetNewAchievementData(_CurId, data.Icon, name, data.Point);
				}
			}
			else if (_CurId >= ACHIEVEMENT_MAX && _CurId < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
			{
				UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(_CurId);
				GuildAchievementData data;
				if (MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
				{
					FString name = data.Name;
					if (name.Len() >= 6)
					{
						name = name.Mid(0, 6);
						name += TEXT("...");
					}

					UINT32 point = 0;
					for (int i = 0; i < MAX_GUILD_ACHIEVEMENT_AWARD_COUNT; ++i)
					{
						if (data.RewardType[i] == GA_AWARD_POINT)
						{
							point = data.RewardCount[i];
							break;
						}
					}
					eventCallSetNewAchievementData(_CurId, data.Icon, name, point);
				}
			}
			else
			{
				UINT16 killId = HIWORD(_CurId);
				UINT16 killIndex = LOWORD(_CurId);
				KillNpcAchievementData data;
				if (MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
				{
					FString name = data.Name[killIndex];
					if (name.Len() >= 6)
					{
						name = name.Mid(0, 6);
						name += TEXT("...");
					}
					eventCallSetNewAchievementData(_CurId, data.Icon, name, data.Point[killIndex]);
				}
			}
		}
	}
	else
	{
	}
}

void UMMOUIAchievementHintPanel::ShowAchievementHintPanel(UINT32 id)
{
	_CurId = id;
	Show(TRUE);
}

void UMMOUIAchievementHintPanel::PlayAchievementHintEnd()
{
	Show(FALSE);
	ScreenMessageQueue::getSingleton().NotifyEndPlayMsg();
}

UBOOL UMMOUIAchievementHintPanel::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	// Task #18968, re-enable if click-through is needed
	//UGFxObject* control = this->GetObject(strHoverControllerName);
	//if (control)
	//{
	//	return TRUE;
	//}
	if (IsVisible)
	{
		if (strHoverControllerName==TEXT("achievement_obtain_animation_mc") ||
			strHoverControllerName==TEXT("achievement_obtain_animation_left_mc") ||
			strHoverControllerName==TEXT("achievement_obtain_animation_right_mc"))
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUIAchievementHintPanel::ASPlayAchievementHintEnd()
{
	PlayAchievementHintEnd();
}

void UMMOUIAchievementHintPanel::ASShowAchievementPanelById(INT id)
{
	// Task #18968
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIAchievementPanel* panel = (UMMOUIAchievementPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Achievement);
		if (panel)
		{
			panel->Show(TRUE);
			panel->ShowAchievementListById(id);
		}
	}
}
