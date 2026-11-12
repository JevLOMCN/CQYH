#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMODeathManager.h"

IMPLEMENT_CLASS(UMMOUIDeathMessage)


void UMMOUIDeathMessage::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	//TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_MsgBoxArenaWaitEnter);
	//EventAdapter = new TGameEventAdapter<UMMOUIArenaQueue>(this, evts);

	MMOUI::GetUIManager()->SetCallShowWaitingReviveDelegate(FScriptDelegate(this, TEXT("NativeCallShowWaitingRevive")));

}

void UMMOUIDeathMessage::UnInit()
{
	UMMOUIBase::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetCallShowWaitingReviveDelegate(nullDelgate);

}

void UMMOUIDeathMessage::Refresh()
{
	//TArray<	FASValue> args;
	//Invoke(TEXT("InitLoad"),args);

	UGFxObject* DeathData = GFXPlayer->CreateObject( TEXT("Object") );
	if(DeathData)
	{
		REVIVE_INFO* pInfo = MMODeathManager::getSingleton().GetReviveInfo(crtInstant);
		DeathData->SetInt(TEXT("ReqMoney"), pInfo->nRequireItemCount );
		
		pInfo = MMODeathManager::getSingleton().GetReviveInfo(crtAtCurrentPos);
		DeathData->SetInt(TEXT("ReqItemID"),	pInfo->nRequireItemID);
		DeathData->SetInt(TEXT("ReqItemCount"), pInfo->nRequireItemCount);
		DeathData->SetBool(TEXT("PlayerKilled"),MMODeathManager::getSingleton().GetPlayerKilled());
		DeathData->SetInt(TEXT("WaitTime"),MMODeathManager::getSingleton().GetWaitTime()/1000);
		eventCallUpdateData(DeathData);
	}

}

//do some data updates when displaying
void UMMOUIDeathMessage::NativeOnShow(UBOOL v)
{
	Refresh();
}

void UMMOUIDeathMessage::NativeCallShowWaitingRevive(INT CountDownTime)
{
	eventCallShowWaitingRevive(CountDownTime);
}

void UMMOUIDeathMessage::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		NativeOnShow(v);

		warnf(NAME_Warning, TEXT("ProcessDefaultDie1========================================================") );
		AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (pMainPlayer && pMainPlayer->WorldInfo )
		{
			warnf(NAME_Warning, TEXT("ProcessDefaultDie2") );
			AMMOGame* Game = Cast<AMMOGame>(pMainPlayer->WorldInfo->Game);
			if (Game)
			{
				warnf(NAME_Warning, TEXT("ProcessDefaultDie3") );
				Game->EnableDeathEffects(TRUE);
			}
		}
	} 
}

UBOOL UMMOUIDeathMessage::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(strHoverControllerName=="btn_Revive_Coin" || strHoverControllerName=="btn_Revive_Around" || strHoverControllerName=="btn_Revive_Free" || strHoverControllerName=="ReviveCtrl_mc")
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

void  UMMOUIDeathMessage::ASRelife(INT inReviveType,INT itemID)
{
	if( IsVisible )
	{
		MMOFacade::getSingleton().GetMainController()->RequestRevive(inReviveType, itemID);
	}
}
void UMMOUIDeathMessage::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (NULL != evt)
	{
		//switch(evt->GetEventType())
		//{
		//case  GameEvent_UpdateChannelTab:
		//	{
		//		this->eventCallUpdateChannelTab(MMOChatManager::getSingleton().GetChatFilterChannelListNum());
		//	}
		//	break;
		//}
	} 
}