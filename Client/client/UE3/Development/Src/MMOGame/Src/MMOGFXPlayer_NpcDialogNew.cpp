#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "UITextParser.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_NpcDialogNew)

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_NpcDialogNew::ASSelectDiagOption(INT optionID)
{
	_DialogPanel->SelectDiagOption(optionID);
}

void UMMOGFXPlayer_NpcDialogNew::ASNPCDialogPage(INT pageIndex)
{
	_DialogPanel->CurrentDialogPage = pageIndex;
	MMOQuestManager::getSingleton().SetQuestDialogCurrentPage(pageIndex);
	//reset time
	MMOQuestManager::getSingleton().RestPageTime();

	//animation needs to replay on page turn
	_DialogPanel->eventCallUCPlay(MMOQuestManager::getSingleton().GetPageTotalTime()*1000);
}

void UMMOGFXPlayer_NpcDialogNew::ASCloseDialog()
{
	_DialogPanel->CloseDialog();
	//need to clear here
	MMOQuestManager::getSingleton().ResetContent();
}

void UMMOGFXPlayer_NpcDialogNew::ASSelectAward(class UGFxObject* icon,INT idx,INT itemTemplateID)
{
	debugf(TEXT("----[ASSelectAward] %d %d"), idx, itemTemplateID);
	_DialogPanel->SelectedAwardIndex = idx;

	if (_DialogPanel->SelectedAwardIndex >= 0)
	{
		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_SelectQuestAward);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

void UMMOGFXPlayer_NpcDialogNew::ASAcceptQuest(INT questID, UBOOL share, INT shareSrcId)
{
	_DialogPanel->AcceptAuest(questID, share, shareSrcId);
}

void UMMOGFXPlayer_NpcDialogNew::AbortQuest(INT questID)
{
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
	CMessageHandler::GetInstance().Send_AbandonQuest(questID);
}

void UMMOGFXPlayer_NpcDialogNew::ASAbortQuest(INT questID)
{
	if(questID == INVALID_QUEST_ID)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questID);
	if (!quest)
	{
		return;
	}

	if(!MMOQuestData::getSingleton().IsQuestAccepted(questID))
	{
		return;
	}

	if (!quest->CanAbandon)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CanNotAbandon"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Quest.CanNotAbandon"));
		return;
	}

	FString text =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AborbQuestConfirm"));
	text = FString::Printf(*text, *UITextParser::HTMLFormatQuestName(quest));

	FScriptDelegate funcOK(EC_EventParm);
	funcOK.FunctionName = TEXT("AbortQuestOK");
	funcOK.Object = this;

	FScriptDelegate funcCanel(EC_EventParm);
	funcCanel.FunctionName = TEXT("AbortQuestCanel");
	funcCanel.Object = this;

	MMOMsgBoxManager::getSingleton().AddMessageBox(text, questID, funcOK, funcCanel);
}

void UMMOGFXPlayer_NpcDialogNew::ASSubmitQuest(INT questID)
{
	_DialogPanel->TrySubmitQuest(questID);
}

void UMMOGFXPlayer_NpcDialogNew::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_NpcDialogNew::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_DialogNew);
}