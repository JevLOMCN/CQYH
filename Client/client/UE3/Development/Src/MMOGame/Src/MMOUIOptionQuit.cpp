#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "net/message_handler.h"
#include "MMOMsgBoxManager.h"
#include "TeamManager.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOUIOptionQuit)


void UMMOUIOptionQuit::PostOnShow(UBOOL v)
{
    Super::PostOnShow(v);
}

void UMMOUIOptionQuit::ASOptionQuitImmed()
{
	if( _ReturnType==1 )
	{
		MMOUI::GetUIManager()->ImmReturnLogin();
		//class'MMOUIManager'.static.ShowPanelEx(class'MMOUIManager'.default.Panel_OptionMenu,false, "");
	}
	else
	{
		//Save before exiting
		MMOChatManager::getSingleton().SaveRecord();
		MMOUI::GetUIManager()->ImmQuitGame();
	}

	if( TeamManager::getSingleton().HasTeam() )
	{
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if(player)
		{
			CMessageHandler::GetInstance().Send_TeamLeave( player->ServerID );
		}		
	}
}