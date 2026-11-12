#include "stdafx.h"
#include "MMOGameUIClasses.h"

#include "net/message_handler.h"
#include "MMOFacade.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOUIHeavenGasPractisePanel)


void UMMOUIHeavenGasPractisePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Heaven);
	EventAdapter = new TGameEventAdapter<UMMOUIHeavenGasPractisePanel>(this, evts);
}
void UMMOUIHeavenGasPractisePanel::PreOnShow(UBOOL v)
{
	Super::PreOnShow(v);
	if(v)
	{
		UMMOUISkillTree* panel = Cast<UMMOUISkillTree>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_SkillTree ) );
		if(panel&&panel->IsVisible)
		{
			FLOAT x ,y;
			panel->GetPosition(x,y);
			SetPosition(x + panel->eventGetWidth() - 280,y);
		}
		UpdateHeavenGasPractisePanel();
		INT nMoney = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_AIR);
		eventCallUCUpdateGasMoney(nMoney);

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowGasPraPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
}
void UMMOUIHeavenGasPractisePanel::OnClickBtnPractiseGas(INT nBtnIndex)
{
	CMessageHandler::GetInstance().Send_NormalPractiseGas(nBtnIndex);
}

void UMMOUIHeavenGasPractisePanel::UpdateHeavenGasPractisePanel()
{
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.PractiseGasTitle")));
	for (INT n = 0; n < 5; ++n)
	{
		eventCallUpdatePractisePhrase(n,MMOHeavenManager::getSingleton().GetBtnPractiseState(n));
	}
}

void UMMOUIHeavenGasPractisePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == 0)
		return;
	MMOGameEvent_Heaven* pkEvent = (MMOGameEvent_Heaven*)evt;
	if(evt->GetEventType() == GameEvent_Heaven)
	{
		if(pkEvent->EventType == MMOGameEvent_Heaven::EHeavenEvent_PractisePhrase)
		{
			for (INT n = 0; n < 5; ++n)
			{
				eventCallUpdatePractisePhrase(n,MMOHeavenManager::getSingleton().GetBtnPractiseState(n));
			}
		}
		else if(pkEvent->EventType == MMOGameEvent_Heaven::EHeavenEvent_PractiseAnimation)
		{
			eventCallUCPlayPractiseAnimation(0);
		} 
	}
}
void UMMOUIHeavenGasPractisePanel::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if (MoneyType == (BYTE)E_MONEYTYPE_AIR)
	{
		eventCallUCUpdateGasMoney(MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)MoneyType));
	}
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIHeavenGasPractisePanel::ASClickBtnPractiseGas(INT nBtnIndex)
{
	OnClickBtnPractiseGas(nBtnIndex);
}

void UMMOUIHeavenGasPractisePanel::ASClickOneKeyPractise()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	INT nLevel = mainPlayer->ActorAttributes->heavenAttributes.heavenLevel;
	if(nLevel >= MMOHeavenManager::oneKeyPracticeLevel)
		CMessageHandler::GetInstance().Send_PractiseGasOneKey();
	else
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.CannotAutoPractise") ));
}

void UMMOUIHeavenGasPractisePanel::ASClickOpenExchangePanel()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenGasExchang, "");
}
//===============================================================================================================
//===============================================================================================================