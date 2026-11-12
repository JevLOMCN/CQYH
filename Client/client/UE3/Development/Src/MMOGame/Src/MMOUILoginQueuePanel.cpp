#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "LoginBase.h"
#include "MMOMsgBoxManager.h"
#include "MMOLoginManager.h"
#include "MMOMapDataManager.h"
#include "MMOStats.h"


IMPLEMENT_CLASS(UMMOUILoginQueuePanel);


void UMMOUILoginQueuePanel::Init()
{
	Super::Init();
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_UpdateLoginQueue);
	EventAdapter = new TGameEventAdapter<UMMOUILoginQueuePanel>(this, evts);
	ResetQueue();
}
void UMMOUILoginQueuePanel::UnInit()
{

}
void UMMOUILoginQueuePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	Super::OnNativeGameEvent(evt);

	switch(evt->GetEventType())
	{	
	case GameEvent_UpdateLoginQueue:
		{
			MMOGameEvent_Login_Queue* e = (MMOGameEvent_Login_Queue*)evt;
			UpdateQueueTime(e->nTotal,e->nSelfPos);
		}
		break;
	}
}

void UMMOUILoginQueuePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		extern bool g_isSDOLogin;
		if(g_isSDOLogin)
		{
			extern void HideSDOLoginDialogWnd();
			HideSDOLoginDialogWnd();	
		}
		else
		{
			MMOGameEvent_Login_ShowInputBox evt( FALSE );
			MMOUI::PostGameEvent(&evt);
		}
	}
	else
	{
		ResetQueue();
	}
	
}

void UMMOUILoginQueuePanel::UpdateQueueTime(INT32 total, INT32 pos)
{
	UINT32 time = 0;
	BOOL showTime = FALSE;

	//Current local time
	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);
	FLOAT curTimeSec = MMOFacade::getSingleton().ConvertSystemTimeToTmTSecond( curTime );

	//Time difference from last time
	FLOAT secDiff = abs(curTimeSec - MMOLoginManager::getSingleton().GetLastQueueTime());

	//Rank difference
	FLOAT posDiff = abs(MMOLoginManager::getSingleton().GetLastQueuePos() - pos);

	FLOAT myPosTime = 0.0f;

	if(posDiff != 0)
	{
		//Time needed for one position
		myPosTime = (secDiff / posDiff) > 0 ? (secDiff / posDiff) : 1;
	}

	//Approximate time needed for one position
	if (secDiff >= 0 && posDiff >= 0)
	{
		time = myPosTime * pos;
	}
	else
	{
		showTime = TRUE;
	}

	warnf(TEXT("LoginQueue cur:%d total:%d enter time:%d S %d M deltaTime:%0.1f deltaPos:%.1f"), pos, total, time, time / 60, secDiff, posDiff);

	eventCallUpdateData(pos,time);

	if (showTime)
	{
		eventCallUCUpdateTime(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Login.QueueTime")));
	}
		
	MMOLoginManager::getSingleton().SetLastQueueTime(curTimeSec);
	MMOLoginManager::getSingleton().SetLastQueuePos(pos);
}

void UMMOUILoginQueuePanel::ResetQueue()
{
	MMOLoginManager::getSingleton().SetLastQueueTime(0);
	MMOLoginManager::getSingleton().SetLastQueuePos(0);
}

void UMMOUILoginQueuePanel::ASRequestCancelLoginQueue()
{
	CMessageHandler::GetInstance().CloseConnection(true);

	extern bool g_isSDOLogin;
	if(g_isSDOLogin)
	{
		extern void ShowLoginDialog();
		ShowLoginDialog();	
	}
	else
	{
		MMOGameEvent_Login_ShowInputBox evt( TRUE );
		MMOUI::PostGameEvent(&evt);
	}
	MMOGameEvent_Login_ShowSetUp evt( TRUE );
	MMOUI::PostGameEvent(&evt);

	this->SetVisible(FALSE);
}