#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOArenaManager.h"

IMPLEMENT_CLASS(UMMOUIArenaHint)

#if 0
void UMMOUIArenaHint::Leave()
{
	
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaHint , FALSE, "" );
}


void UMMOUIArenaHint::ClickLeave()
{
	//eventCallShowLeaveButton(FALSE);
	//CMessageHandler::GetInstance().Send_ArenaQueueLeave();
	//MMOArenaManager::getSingleton().LeaveArena();
}


void UMMOUIArenaHint::ClickIcon()
{

	eventCallToggleLeaveButton();
	
}


void UMMOUIArenaHint::ClearUI()
{
	//MMOHintButtonManager::getSingleton().ClearAllButton();
}

void UMMOUIArenaHint::Init()
{

	//MMOUI::GetUIManager()->SetLeaveArenaQueueDelegate( FScriptDelegate(this, TEXT("Leave")) );
	//MMOUI::GetUIManager()->SetAskEnterArenaDelegate( FScriptDelegate(this, TEXT("AskEnterArena")) );

	if(GFXPlayer)
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		if(data)
		{
			data->SetString( TEXT("baseText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Arena") ) );
			data->SetString( TEXT("leaveText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaLeave") ) );
			eventCallSetPanelText(data);
		}
		
	}

}

void UMMOUIArenaHint::UnInit()
{
	//MMOHintButtonManager::getSingleton().ClearAllButton();
	UMMOUIBase::UnInit();

	//FScriptDelegate nullDelgate(EC_EventParm);
	//MMOUI::GetUIManager()->SetOpenArenaQueueDelegate(nullDelgate);
	//MMOUI::GetUIManager()->SetLeaveArenaQueueDelegate(nullDelgate);
	//MMOUI::GetUIManager()->SetAskEnterArenaDelegate(nullDelgate);
}

void UMMOUIArenaHint::StartFight( INT arenaTime )
{
	Step = ArenaHintStepFighting;
	ArenaTime = arenaTime;
}

void UMMOUIArenaHint::UpdateTime( float dt )
{
	//Arena
	if( Step == ArenaHintStepFighting )
	{
		INT lastQueueTime = (INT)(ArenaTime);
		ArenaTime -= dt;

		INT newTime = (INT)(ArenaTime);
		if( newTime != lastQueueTime )
		{
			//UpdateQueueTime( newTime );
			FString timeText = UMMOUIArenaQueue::GetQueueTimeString( newTime );

			if( GFXPlayer )
			{
				UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );

				FString title;

				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaTime") );
				title += timeText;

				data->SetString( TEXT("title") , title );
				data->SetString( TEXT("content") , timeText );

				eventCallSetHintText( data );
			}

		}

		if( ArenaTime <= 0 )
		{
			Step = ArenaHintStepOver;
			MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaHint , FALSE, "" );
		}

	}


}
#endif