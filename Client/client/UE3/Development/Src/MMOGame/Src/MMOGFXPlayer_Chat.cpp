#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "ScaleformEngine.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Chat)

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Chat::ASOnPanelClose(const FString& PanelName)
{
	// on ESC remove own focus, basically yielding to the Skillbar
	if(PanelName == TEXT("Chat"))
	{
		UMMOUIBase* PanelSkilbar = MMOUI::FindUIPanel(TEXT("Skillbar"));
		
		if(PanelSkilbar)
		{
			//MMOUI::MoveToTop(PanelSkilbar);
			//FGFxEngine::GetEngine()->SetFocusMovie(PanelSkilbar->GFXPlayer->pMovie);
			MMOUI::RemoveTop(Chat);
		}

		if(Chat)
		{
			Chat->ShowInputArea(FALSE);
			//Super::ASOnPanelClose(PanelName);
		}
	}
	else if( PanelName == MMOUI::GetUIManager()->Panel_Chat_ChatOption.ToString() )
	{
		Super::ASOnPanelClose(PanelName);
	}
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Chat::ASFocusInTextInput(UBOOL In)
{
	AMMOPlayerController* Ctrl = MMOFacade::getSingleton().GetMainController();

	Super::ASFocusInTextInput(In);

	if( In)
	{
		//Ctrl->SetInTextInput(In);
		Ctrl->FocusOnInput(Chat);
	}
	else
	{
		Ctrl->FocusOutInput();
		//Ctrl->SetInTextInput(In);
	}
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Chat::PostAdvance(FLOAT inDeltaTime )
{
	
	if( inDeltaTime >= 0.0 && Chat && Chat->IsVisible )
	{
		Chat->TickTime(inDeltaTime);
	}
}

void UMMOGFXPlayer_Chat:: ASChatClickChatOption()
{
	if( _ChatOption )
	{
		_ChatOption->ToogleVisible();
	}
	
}
//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Chat::ASOnChatEnter()
{
	if(Chat)
	{
		//Chat->ShowInputArea(TRUE);

		//TArray<struct FASValue> Args;

		//Chat->Invoke(TEXT("SetTextInputFocus"),Args);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void UMMOGFXPlayer_Chat::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Chat::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Chat);
}

void UMMOGFXPlayer_Chat::ASonRollChatPanel(UBOOL focus)
{
	//MMOChatManager::getSingleton().SetFocus(focus);
}

void UMMOGFXPlayer_Chat:: ASChatShowTips(class UGFxObject *mc, BYTE type)
{
	if( mc == NULL ){
		return;
	}
	switch(type)
	{
	case CTT_CHAT:
		mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.Hide") ) );
		break;
	case CTT_CHAT_CLEAN:
		mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.Clean") ) );
		break;
	}
	
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_Chat::ASChatHideTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}