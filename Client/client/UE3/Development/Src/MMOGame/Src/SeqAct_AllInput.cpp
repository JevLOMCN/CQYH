#include "..\stdafx.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "CommonGameEvents.h"

IMPLEMENT_CLASS(USeqAct_AllInput)
IMPLEMENT_CLASS(USeqAct_ShowPlayer)
IMPLEMENT_CLASS(USeqAct_ShowAllNPC)
IMPLEMENT_CLASS(USeqAct_ShowAllPC)
IMPLEMENT_CLASS(USeqAct_ShowMatineeSubtitle)
IMPLEMENT_CLASS(USeqAct_UpdateMatineeSubtitle)


extern bool g_disableAllInput;

extern bool g_hideAllNPC;
extern bool g_hideAllPC;


void USeqAct_AllInput::Activated()
{
	if(this->InputLinks(0).bHasImpulse)
	{
		g_disableAllInput = false;
	}
	else if(this->InputLinks(1).bHasImpulse)
	{
		g_disableAllInput = true;
	}

	if(!GIsEditor)
	{
		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
		if(ctrl)
		{
			//MMOTargetManager::getSingleton().Clear();
			FSMIO::getSingleton().State.ClearAllState();
			ctrl->StopMove(TRUE);
			ctrl->FlushPressedKeys();
			if(g_disableAllInput==false)
			{
				if(Player && Player->eventIsDead())
				{
					MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death,TRUE,"");
					//MMOUI::GetUIManager()->delegateShowDeath();
				}
			}
		}

		if(Player)
		{
			Player->setPhysics(PHYS_Walking);
			Player->ResetPlaceActorToGroundTimer();
		}
	}

	Super::Activated();
}

void USeqAct_ShowPlayer::Activated()
{
	//UBOOL hidePlayer = FALSE;
	//if(this->InputLinks(0).bHasImpulse)
	//{
	//	hidePlayer = FALSE;
	//}
	//else if(this->InputLinks(1).bHasImpulse)
	//{
	//	hidePlayer = TRUE;
	//}

	//AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	//if(ctrl)
	//{
	//	//ctrl->Pawn->SetHidden(hidePlayer);
	//	AMMOActor* MainActor = Cast<AMMOActor>(ctrl->Pawn);
	//	if (MainActor)
	//	{
	//		MainActor->SetVisible(!hidePlayer);
	//	}

	//}

	if (this->InputLinks(0).bHasImpulse)
	{
		MMOFacade::getSingleton().GetMainController()->SetShowMainPlayer(TRUE);
	}
	else if (this->InputLinks(1).bHasImpulse)
	{
		MMOFacade::getSingleton().GetMainController()->SetShowMainPlayer(FALSE);
	}

	Super::Activated();
}

void USeqAct_ShowAllPC::Activated()
{
	//UBOOL hidePlayer = FALSE;
	//if(this->InputLinks(0).bHasImpulse)
	//{
	//	hidePlayer = FALSE;
	//}
	//else if(this->InputLinks(1).bHasImpulse)
	//{
	//	hidePlayer = TRUE;
	//}

	//g_hideAllPC = hidePlayer != 0;

	//UBOOL selfVis = MMOFacade::getSingleton().GetMainActor()->bHidden;

	//TObjectIterator<AMMOPlayer> It;
	//for (; It; ++It)
	//{
	//	AMMOPlayer* pc = *It;
	//	pc->SetHidden(hidePlayer);
	//}


	//MMOFacade::getSingleton().GetMainActor()->SetHidden(selfVis);

	if (this->InputLinks(0).bHasImpulse)
	{
		MMOFacade::getSingleton().GetMainController()->SetShowTheOtherPlayers(TRUE);
		MMOFacade::getSingleton().GetMainController()->SetShowDrops(TRUE);
	}
	else if (this->InputLinks(1).bHasImpulse)
	{
		MMOFacade::getSingleton().GetMainController()->SetShowTheOtherPlayers(FALSE);
		MMOFacade::getSingleton().GetMainController()->SetShowDrops(FALSE);
	}

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player)
	{
		Player->setPhysics(PHYS_Walking);
		Player->ResetPlaceActorToGroundTimer();
	}

	Super::Activated();
}

void USeqAct_ShowAllNPC::Activated()
{
	//UBOOL hidePlayer = FALSE;
	//if(this->InputLinks(0).bHasImpulse)
	//{
	//	hidePlayer = FALSE;
	//}
	//else if(this->InputLinks(1).bHasImpulse)
	//{
	//	hidePlayer = TRUE;
	//}

	//g_hideAllNPC =  hidePlayer != 0;

	//TObjectIterator<ANPC> It;
	//for (; It; ++It)
	//{
	//	ANPC* pc = *It;
	//	pc->SetHidden(hidePlayer);
	//}

	if (this->InputLinks(0).bHasImpulse)
	{
		MMOFacade::getSingleton().GetMainController()->SetShowNpc(TRUE);
	}
	else if (this->InputLinks(1).bHasImpulse)
	{
		MMOFacade::getSingleton().GetMainController()->SetShowNpc(FALSE);
	}

	Super::Activated();
}



void USeqAct_ShowMatineeSubtitle::Activated()
{
	bool show = false;
	if(this->InputLinks(0).bHasImpulse)
	{
		show = true;
	}
	else if(this->InputLinks(1).bHasImpulse)
	{
		show = false;
	}
	
	// Clear subtitles first when showing
	{
		MMOGameEvent_MatineeSubtitle evts(FString(TEXT("")), MMOGameEvent_MatineeSubtitle::EMSET_UpdateSubtitle);
		MMOUI::PostGameEvent(&evts);
	}

	{
		MMOGameEvent_MatineeSubtitle evt(show, MMOGameEvent_MatineeSubtitle::EMSET_Show);
		MMOUI::PostGameEvent(&evt);
	}

	Super::Activated();
}

void USeqAct_UpdateMatineeSubtitle::Activated()
{
	if(this->InputLinks(0).bHasImpulse)
	{
		MMOGameEvent_MatineeSubtitle evt(Subtitle, MMOGameEvent_MatineeSubtitle::EMSET_UpdateSubtitle);
		MMOUI::PostGameEvent(&evt);
	}

	Super::Activated();
}