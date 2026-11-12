#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOStats.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuideOptionManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOUIGuideOption)

void UMMOUIGuideOption::Init()
{
	Super::Init();
	
}

void UMMOUIGuideOption::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);
	if (show)
	{
	}
}

void UMMOUIGuideOption::UnInit()
{
	Super::UnInit();
}

void UMMOUIGuideOption::OnNativeGameEvent(struct MMOGameEvent* evt)
{		
}

void UMMOUIGuideOption::ASShowGuideOptionTips(UGFxObject* mc)
{
	if (!mc)
	{
		return;
	}

	mc->SetString( TEXT("tipsHtml"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FirstOption.Option")));
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_PlainText, mc);
}

void UMMOUIGuideOption::ASHideGuideOptionTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIGuideOption::ASApplyGuideOption(INT optionType)
{
	MMOGuideOptionManager::getSingleton().SetGameControlType(GC_MIR2);

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		uiManager->QuestMode = (optionType == 0);
		uiManager->SaveConfigToServer();
	}

	MMOGameEvent_Any evt(GameEvent_QuestModeChange);
	MMOUI::PostGameEvent(&evt);
}

void UMMOUIGuideOption::ASGuideOptionPanelClose()
{
	MMOGuideOptionManager::getSingleton().SetGameControlType(GC_MIR2);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_GuideOption, FALSE, "");

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		uiManager->SaveConfigToServer();
	}

	MMOGameEvent_Any evt(GameEvent_QuestModeChange);
	MMOUI::PostGameEvent(&evt);

	// tutorial event: movement teaching
	MMOGameEvent_Guide evtGuide(EGUET_Move);
	MMOUI::PostGameEvent(&evtGuide);
}