#include "stdafx.h"
#include "FSMIO.h"
#include "MMOGuildData.h"
#include "MMOGuildManager.h"
#include "MMOMapDataManager.h"

void AMMOVolume_GuildBattle::OnActive(class AMMOActor* actor,UBOOL bOn)
{
	// fix bug  #23525
	// todo: found before the Dec 10 test that this Volume also exists in guild territory,
	// but unsure whether the server uses this Volume's data, so LD was not asked to delete it; client protects it temporarily
	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId != SABAC_MAP_ID)
	{
		return;
	}

	if (actor && actor->IsA(AMMOPlayer::StaticClass()))
	{
		if (bOn)
		{
			actor->inSabacZoneCount ++;
		} 
		else
		{ 
			actor->inSabacZoneCount --;
		}

		MMOUI::GetUIManager()->delegateUpdateHintText(actor);

		if (actor->IsMainPlayer())
		{
			// bug: #14676
			if (MMOGuildManager::getSingleton().GetSabacState() > 0)
			{
				FString curSabacOccupyGuildName = TEXT("");
				MMOGuildManager::getSingleton().GetSabacOccupyGuildName(curSabacOccupyGuildName);

				UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
				if (panel)
				{
					AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
					//panel->Show(mainPlayer != NULL && mainPlayer->inSabacZoneCount > 0);
					panel->Show(FALSE);
					panel->UpdateGuildWarHintInfo(TEXT(""), curSabacOccupyGuildName, TEXT("Time left in the Conquest War: "), MMOGuildManager::getSingleton().GetGuildWarTime());
				}
			}
		}
	}
}