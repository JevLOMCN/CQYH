#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "FSMIO.h"
#include "MMOMsgBoxManager.h"
#include "MMOPerformance.h"
#include "MMOChatManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_SceneHint)

//void UMMOGFXPlayer_SceneHint::PostAdvance(float DeltaTime )
//{
//	UGFxMoviePlayer::PostAdvance(DeltaTime);
//}

void UMMOGFXPlayer_SceneHint::BeforeViewUpdate(FLOAT DeltaTime)
{
////	UpdateHintPos(DeltaTime);
//
//	if(MMO::GetCurrentGameState() != EGameState_InGame)
//	{
//		return;
//	}
//	/*
//	performance tuning tips
//	*/
//
//	static FLOAT g_lastCheckTime = 0.0f;
//	static FLOAT s_averageFPS = 0;
//
//	if( GSystemSettings.bAutoAdjustPerformance || GSystemSettings.bHintBeforeAutoAdjust )
//	{
//		if(g_lastCheckTime > GSystemSettings.AutoPerformanceCheckTime
//			&& (GSystemSettings.CompatLevelComposite > 1))
//		{
//
//			/*extern FLOAT GAverageFPS;
//			if(GAverageFPS < GSystemSettings.LowerCheckFPS)*/
//			
//			// don't check tips during cutscenes
//			if(!MMOFacade::getSingleton().GetMainController()->IsPlayingMatinee())
//			{
//				//FLOAT fInGameStateFps = MMOFacade::getSingleton().GetGameSateFps();
//
//				s_averageFPS /= g_lastCheckTime;
//
//				if(s_averageFPS < GSystemSettings.LowerCheckFPS)
//				{
//					if(GSystemSettings.bHintBeforeAutoAdjust)
//						MMOMsgBoxManager::getSingleton().AutoAdjustSystemSettingMessageBox();
//					else if( GSystemSettings.bAutoAdjustPerformance ) 
//					{
//						INT nLevel = GSystemSettings.CompatLevelComposite;
//						AutoLowScaleLevel();//GSystemSettings.AutoAdjustCompatLevel();
//					}
//				}
//			}
//
//			g_lastCheckTime = 0.0f;
//			s_averageFPS = 0;
//
//			MMOFacade::getSingleton().ResetGameSateFps();
//		}
//		else
//		{
//			g_lastCheckTime += DeltaTime;
//
//			FLOAT fInGameStateFps = MMOFacade::getSingleton().GetGameSateFps();
//
//			s_averageFPS += fInGameStateFps * DeltaTime;
//		}
//	}
//	else
//	{
//			g_lastCheckTime = 0.0f;
//	}
//
//	//HideNearPlayer(DeltaTime);
}
//
//void UMMOGFXPlayer_SceneHint::UpdateHintText(class AMMOActor* pl)
//{
//	//ANPC* npc = Cast<ANPC>(pl);
//	if(SceneHint==NULL)return;
//	if(pl==NULL)return;
//
//	SceneHint->RefleshHints(pl->ServerID);
//
//	//if(npc)
//	//{		
//	//	SceneHint->CreateNpcHint(npc);
//	//}
//	//else
//	//{
//	//	SceneHint->RefleshHints(pl->ServerID);
//	//}
//
//}
//
//void UMMOGFXPlayer_SceneHint::UpdateHintPos(float DeltaTime)
//{
//	if(SceneHint)
//	{		
//		SceneHint->UpdateHints_Pos(DeltaTime);
//	}
//}
//
//void UMMOGFXPlayer_SceneHint::ShowHintBtn(UBOOL bShow)
//{
//	if(SceneHint)
//	{		
//		SceneHint->ShowHintBtn(bShow);
//	}
//}

void UMMOGFXPlayer_SceneHint::Init()
{
	Super::Init();

//	MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_SceneHint);

//	MMOUI::GetUIManager()->SetUpdateHintTextDelegate(FScriptDelegate(this, TEXT("UpdateHintText")));
//	//MMOUI::GetUIManager()->SetUpdateHintPosDelegate(FScriptDelegate(this, TEXT("UpdateHintPos")));
//	MMOUI::GetUIManager()->SetShowHintBtnDelegate(FScriptDelegate(this, TEXT("ShowHintBtn")));
}

void UMMOGFXPlayer_SceneHint::UnInit()
{
	Super::UnInit();

//	MMOUI::GetUIManager()->SetUpdateHintTextDelegate(FScriptDelegate(EC_EventParm));
//	//MMOUI::GetUIManager()->SetUpdateHintPosDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetShowHintBtnDelegate(FScriptDelegate(EC_EventParm));
}
//
//void UMMOGFXPlayer_SceneHint::ASRightClickTargetInfo(INT x,INT y,INT serverID)
//{
//	AActor* Target = MMOFacade::getSingleton().GetActor(serverID);
//	AMMOActor* targetActor =	 Cast<AMMOActor>(Target);
//	//MMOTargetManager::getSingleton().SetSelectMMOActor(targetActor);
//	FSMIO::getSingleton().Target.SetSelectMMOActor(targetActor);
//	if( targetActor )
//	{
//		MMOUI::GetUIManager()->delegateOpenRClickMenu(
//			ERCMType_TargetInfo , x , y, targetActor->ServerID, targetActor->RoleName, 0 );
//	}
//}
//void UMMOGFXPlayer_SceneHint::HideNearPlayer(FLOAT DeltaTime)
//{
//	static INT nNearPlayerHideHint = -1;
//	if(nNearPlayerHideHint < 0)
//		GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerHideHint") , nNearPlayerHideHint , GDefaultGameIni );
//
//	if(nNearPlayerHideHint > 0)
//	{
//		UMMOKeyManager* ukm = UMMOKeyManager::getSingleton();
//		
//		if(ukm == NULL || !( MMOUI::GetUIManager()->HintHideNearPlayer))
//			return;
//
//		static FLOAT fTimeDetailPass = 0.0f;
//		static INT nNearPlayerHintPerSeconds = 0;
//		static INT nNearPlayerFps = 0;
//		static INT nNearPlayerCount = 0;
//		if(nNearPlayerHintPerSeconds == 0)
//			GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerHintPerSeconds") , nNearPlayerHintPerSeconds , GDefaultGameIni );
//		if(nNearPlayerFps == 0)
//			GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerFps") , nNearPlayerFps , GDefaultGameIni );
//		if(nNearPlayerCount == 0)
//			GConfig->GetInt( TEXT("SettingHint") , TEXT("NearPlayerCount") , nNearPlayerCount , GDefaultGameIni );
//
//		fTimeDetailPass += DeltaTime;
//		if (fTimeDetailPass >= nNearPlayerHintPerSeconds)
//		{
//			fTimeDetailPass = 0.0f;
//
//			FLOAT fInGameStateFps = MMOFacade::getSingleton().GetGameSateFps();
//			if(fInGameStateFps < nNearPlayerFps)
//			{
//				if(nNearPlayerCount <= MMOFacade::getSingleton().GetPlayerCount())
//				{
//					FString strKey;
//					UMMOKeyManager::KKeyworkd* pKKey = ukm->FindKeyworld(UMMOKeyManager::KF_ShowHidePlayer, FALSE );
//					if(pKKey)
//						ukm->KeyToString(pKKey,strKey);
//				
//					if(!strKey.IsEmpty())
//					{
//						FString text = FString::Printf( TEXT("Press hotkey: %s to hide nearby players") , *(strKey) );
//						MMOChatManager::getSingleton().AddSystemMessage( text );
//						MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Normal);
//					}
//				}
//			}
//		}
//	}
//}