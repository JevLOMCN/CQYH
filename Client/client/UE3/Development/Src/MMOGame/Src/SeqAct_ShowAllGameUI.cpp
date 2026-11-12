#include "..\stdafx.h"
#include "MMOFacade.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(USeqAct_ShowAllGameUI)


void USeqAct_ShowAllGameUI::Activated()
{
	if(this->InputLinks(0).bHasImpulse)
	{
		MMOUI::ShowInGameUI(TRUE);

		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_KismetEnd);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else if(this->InputLinks(1).bHasImpulse)
	{
		MMOUI::ShowInGameUI(FALSE);
	}

	Super::Activated();
}
