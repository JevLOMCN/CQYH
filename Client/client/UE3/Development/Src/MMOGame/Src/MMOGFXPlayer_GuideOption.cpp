#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "UITextParser.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuideOptionManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_GuideOption)

void UMMOGFXPlayer_GuideOption::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_GuideOption::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_GuideOption);
}

void UMMOGFXPlayer_GuideOption::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_GuideOption::ASShowGuideOptionTips(UGFxObject* mc)
{
	if( mc == NULL ){
		return;
	}

	mc->SetString( TEXT("tipsHtml"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FirstOption.Option")));
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_PlainText, mc);
}

void UMMOGFXPlayer_GuideOption::ASHideGuideOptionTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOGFXPlayer_GuideOption::ASOnPanelClose(const FString& PanelName)
{
	if( PanelName == MMOUI::GetUIManager()->Panel_GuideOption.ToString())
	{
		ASGuideOptionPanelClose();
	}
	Super::ASOnPanelClose(PanelName);
}

void UMMOGFXPlayer_GuideOption::ASSetGuideOption(INT optionType)
{
	MMOGuideOptionManager::getSingleton().SetGameControlType(GC_MIR2);

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		uiManager->QuestMode = (optionType == 0);
		uiManager->SaveConfigToServer();
	}
}

void UMMOGFXPlayer_GuideOption::ASGuideOptionPanelClose()
{
	MMOGuideOptionManager::getSingleton().SetGameControlType(GC_MIR2);
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_GuideOption,FALSE,"");

	// tutorial event: movement teaching
	MMOGameEvent_Guide evtGuide(EGUET_Move);
	MMOUI::PostGameEvent(&evtGuide);
}