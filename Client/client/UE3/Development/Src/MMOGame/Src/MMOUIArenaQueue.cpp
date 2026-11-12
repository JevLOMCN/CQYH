#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOHintButtonManager.h"
#include "MMOArenaManager.h"
#include "BattleTeamManager.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIArenaQueue)

#if 0
void UMMOUIArenaQueue::Leave()
{

	//Step = ArenaQueueStepNone;
	//MMOArenaManager::getSingleton().SetArenaStep(ArenaStep_None);
	
	if( MsgboxWindow )
	{
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonOk(MsgboxWindow);
		MsgboxWindow = NULL;
	}

	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaQueue , FALSE, "" );
}

void UMMOUIArenaQueue::NativeInitOnce()
{
	UGFxObject* obj = GetObject( TEXT("LeaveButton") );
	if(obj)
	{
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Leave") ) );
	}
}

void UMMOUIArenaQueue::ClickLeave()
{
	eventCallShowLeaveButton(FALSE);
	CMessageHandler::GetInstance().Send_ArenaQueueLeave();
}

void UMMOUIArenaQueue::Minimize()
{
	Show(FALSE);
}

void UMMOUIArenaQueue::OpenArenaQueue(INT mode,INT size)
{
	//determine whether the party size is valid (and all from the same team)
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	TeamManager& teamManager = TeamManager::getSingleton();
	UBOOL valid = TRUE;

	if( size == ArenaSize_1vs1 )
	{
		if( teamManager.HasTeam() && teamManager.GetTeamMemberCount() != size )			 
		{
			valid = FALSE;
		}
	}
	else
	{
		if( !teamManager.HasTeam() || teamManager.GetTeamMemberCount() != size )
		{
			valid = FALSE;
		}
		else
		{
			for ( INT i = 0 ; i < teamManager.GetTeamMemberCount() ; i++ )
			{
				MMOTeamMember* mem = teamManager.GetTeamMember(i);
				//if( mem && !BattleTeamManager::getSingleton().HasMember( mem->ActorServerId) )
				//{
				//	valid = FALSE;
				//	break;
				//}

				
			}
		}
	}

	if( !valid )
	{
		return;
	}

	//show team info
	//MMOArenaManager::getSingleton().SetArenaStep(ArenaStep_Queuing);
	
	//Show(TRUE);
	ArenaModeValue = mode;
	ArenaSizeValue = size;

	//add hint button
	//MMOHintButtonManager::getSingleton().AddNewButton( EHintButtonType_ArenaQueueHint , 0 , TRUE );

}

void UMMOUIArenaQueue::ClickIcon()
{
	//if( MMOArenaManager::getSingleton().GetArenaStep() == ArenaStep_WaitEnter )
	//{
	//	//create another one
	//	if( MsgboxWindow == NULL )
	//	{
	//		CreateEnterAskWindow();

	//	}
	//	
	//}
	//else if( MMOArenaManager::getSingleton().GetArenaStep() == ArenaStep_Queuing )
	//{

	//	eventCallToggleLeaveButton();
	//}
}

FString UMMOUIArenaQueue::GetQueueTimeString( INT newTime )
{
	//if(QueueTimeTextField)
	{
		INT min = newTime / 60;
		INT sec = newTime % 60;
		//cross boundary
		if( min > 100 )
		{
			min = 99;
			sec = 59;
		}

		return FString::Printf( TEXT("%02d:%02d") , min , sec );
	}
}

void UMMOUIArenaQueue::CreateEnterAskWindow()
{


	MsgboxWindow = MMOMsgBoxManager::getSingleton().AddMessageBoxEvent( 
		MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.PleaseFastEnter")) , 0 , GameEvent_MsgBoxArenaWaitEnter /*, funCancel*/ );

	if( MsgboxWindow )
	{
		UGFxObject* butt = MsgboxWindow->GetObject(TEXT("noButton_mc"));
		if(butt)
		{
			butt->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.Ignore") ) );
		}
	}
}

void UMMOUIArenaQueue::AskEnterArena()
{
	CreateEnterAskWindow();
	
	//MMOArenaManager::getSingleton().SetArenaStep( ArenaStep_WaitEnter );
	//Step = ArenaQueueStepWaitingEnter;

	eventCallShowLeaveButton(FALSE);
	

}

void UMMOUIArenaQueue::ClearUI()
{
	//MMOHintButtonManager::getSingleton().ClearAllButton();
}

void UMMOUIArenaQueue::EnterArenaConfirm(class UGFxObject* mc)
{
	CMessageHandler::GetInstance().Send_ArenaJoinAck( TRUE );
	
	//Step = ArenaQueueStepNone;
	//MMOArenaManager::getSingleton().SetArenaStep( ArenaStep_None );
	
	MsgboxWindow = NULL;

	//MMOArenaManager::getSingleton().StartArena();
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaQueue , FALSE, "" );
}

//void UMMOUIArenaQueue::LeaveArenaQueue()
//{
//
//}

void UMMOUIArenaQueue::EnterArenaCancel(class UGFxObject* mc)
{
	//CMessageHandler::GetInstance().Send_ArenaJoinAck( FALSE );
	
	//Step = ArenaQueueStepNone;
	//MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaQueue , FALSE,"" );
	
	MsgboxWindow = NULL;
}

void UMMOUIArenaQueue::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch( evt->GetEventType() )
	{
	case GameEvent_MsgBoxArenaWaitEnter:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if(cb->Agree)
				{
					EnterArenaConfirm(cb->Box);
				}
				else
				{
					EnterArenaCancel(cb->Box);
				}
			}
		}
		break;
	}
}

void UMMOUIArenaQueue::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MsgBoxArenaWaitEnter);
	EventAdapter = new TGameEventAdapter<UMMOUIArenaQueue>(this, evts);

	MMOUI::GetUIManager()->SetLeaveArenaQueueDelegate( FScriptDelegate(this, TEXT("Leave")) );
	MMOUI::GetUIManager()->SetAskEnterArenaDelegate( FScriptDelegate(this, TEXT("AskEnterArena")) );

	if(GFXPlayer)
	{
		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		if(data)
		{
			data->SetString( TEXT("baseText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.Queue") ) );
			data->SetString( TEXT("leaveText") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.LeaveQueue") ) );
			eventCallSetPanelText(data);
		}
		
	}

	GConfig->GetInt( TEXT("ArenaQueue") , TEXT("ArenaEnterTime") , EnterCoolDownTime , GDefaultGameIni );
	
	
}

void UMMOUIArenaQueue::UnInit()
{
	//MMOHintButtonManager::getSingleton().ClearAllButton();
	UMMOUIBase::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	//MMOUI::GetUIManager()->SetOpenArenaQueueDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetLeaveArenaQueueDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetAskEnterArenaDelegate(nullDelgate);
}

void UMMOUIArenaQueue::UpdateTime( float dt )
{
	//countdown to enter arena
	//if( MMOArenaManager::getSingleton().GetArenaStep() == ArenaStep_WaitEnter )
	//{
	//	INT arenaTime = MMOArenaManager::getSingleton().GetWaitEnter();
	//	INT lastArenaTime = MMOArenaManager::getSingleton().GetLastWaitEnter();
	//	//TickTime -= dt;
	//	if( arenaTime != lastArenaTime )
	//	{
	//		if( MsgboxWindow && arenaTime >= 0 )
	//		{
	//			FString sz = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.PleaseFastEnter"));
	//			sz += appItoa( arenaTime );
	//			
	//			UGFxObject* bg = MsgboxWindow->GetObject( TEXT("MyWindowsFrame_mc") );
	//			if(bg)
	//			{
	//				UGFxObject* content = bg->GetObject( TEXT("content") );
	//				if(content)
	//				{
	//					content->SetString( TEXT("htmlText") , sz );
	//				}					
	//			}
	//		}

	//		if( GFXPlayer && arenaTime >= 0 )
	//		{
	//			FString timeText = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.PleaseFastEnter"));
	//			timeText += appItoa( arenaTime );

	//			UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );

	//			FString title;
	//			if( ArenaSizeValue == ArenaSize_1vs1 )
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.size0") );
	//			}
	//			else
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.size1") );
	//			}

	//			if( ArenaModeValue == artPractice )
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Practice") );
	//			}
	//			else
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaRace") );
	//			}

	//			data->SetString( TEXT("title") , title );
	//			data->SetString( TEXT("content") , timeText );
	//			
	//			eventCallSetHintText( data );

	//		}
	//	}
	//	//LastTime = TickTime;

	//	if( arenaTime <= 0  )
	//	{
	//		//countdown finished
	//		if(MsgboxWindow)
	//		{
	//			//if there is a window, click confirm
	//			MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonOk(MsgboxWindow);
	//		}
	//		else
	//		{
	//			//if there is no window, enter directly
	//			EnterArenaConfirm(NULL);
	//		}
	//	}
	//}
	////while queuing
	//if( MMOArenaManager::getSingleton().GetArenaStep() == ArenaStep_Queuing )
	//{
	//	INT lastQueueTime = MMOArenaManager::getSingleton().GetLastQueueTime();  //(INT)(QueueTime);
	//	INT queueTime = MMOArenaManager::getSingleton().GetQueueTime();

	//	
	//	if( queueTime != lastQueueTime )
	//	{
	//		//UpdateQueueTime( newTime );
	//		FString timeText = GetQueueTimeString( queueTime );

	//		if( GFXPlayer )
	//		{
	//			UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );

	//			FString title;
	//			if( ArenaSizeValue == ArenaSize_1vs1 )
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.size0") );
	//			}
	//			else
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ArenaQueue.size1") );
	//			}

	//			if( ArenaModeValue == artPractice )
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.Practice") );
	//			}
	//			else
	//			{
	//				title += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaRace") );
	//			}

	//			data->SetString( TEXT("title") , title );
	//			data->SetString( TEXT("content") , timeText );
	//			
	//			eventCallSetHintText( data );
	//		}
	//		
	//	}
	//	
	//}

	//if( dt>=0 )
	//	TickTime += dt;
	//if( TickTime > 1 )
	//{
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( TEXT("test") );
	//	TickTime = 0;
	//}
}
#endif