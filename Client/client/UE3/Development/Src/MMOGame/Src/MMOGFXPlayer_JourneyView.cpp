#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOJourneyManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_JourneyView)

void UMMOGFXPlayer_JourneyView::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_JourneyView::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_JourneyView);
}


void UMMOGFXPlayer_JourneyView::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_JourneyView::ASUpdateJourneyInfo(int journeyId)
{
	if (_JourneyViewPanel)
	{
		_JourneyViewPanel->SetCurJourneyId(journeyId);
		_JourneyViewPanel->UpdateJourneyInfo(journeyId);
	}
}

void UMMOGFXPlayer_JourneyView::ASGetCompleteAwards(int journeyId)
{
	JourneyData data;
	if (MMOJourneyData::getSingleton().GetJourneyData(journeyId, data))
	{
		if (MMOFacade::getSingleton().GetBagEmptySlotCount() < MMOJourneyData::getSingleton().GetReputationAwardsCount(data.ForceId))
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Not enough bag space to claim the reward"),
				MsgInfoType_Table,
				TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAchievementRewardBagFull"));
		}
		else
		{
			CMessageHandler::GetInstance().Send_ReceiveReputationAward(data.ForceId);

			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				if (!MMOFacade::getSingleton().GetPlayerAwardState(mainPlayer, data.ForceId))
				{
					if (_JourneyCompletePanel)
					{
						_JourneyCompletePanel->Show(TRUE);
						_JourneyCompletePanel->eventCallJourneyComplete(data.Title);
					}
				}
			}
		}
	}
}

void UMMOGFXPlayer_JourneyView::ASJourneyComplete()
{
	if (_JourneyCompletePanel)
	{
		_JourneyCompletePanel->Show(FALSE);
	}
}
