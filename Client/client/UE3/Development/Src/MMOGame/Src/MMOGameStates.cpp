#include "stdafx.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "ScaleformEngine.h"
#include "MMOGameEvent_Splash.h"
#include "MailManager.h"

#include "MasterStudentManager.h"
#include "FSMIO.h"
#include "MMOQuestManager.h"
#include "MMOAchievementManager.h"
#include "MMOMapDataManager.h"
#include "CommonGameEvents.h"
#include "MMOGuildManager.h"
#include "MMOBattleManager.h"
#include "MMOGuideManager.h"
#include "DoodadManager.h"
#include "EngineAudioDeviceClasses.h"
#include "error_code_name.h"

#include "TeamManager.h"
#include "MMOMsgButtonManager.h"
#include "FriendChatManager.h"
#include "FriendManager.h"
#include "MMOHeavenManager.h"
#include "MMOCoolDownManager.h"
#include "MMOArenaManager.h"
#include "MMONavigationManager.h"
#include "MMOMsgBoxManager.h"
#include "DebugMMO.h"
#include "MMOHurtManager.h"
#include "AbilityProcessor.h"
#include "MasterStudentManager.h"
#include "MMOGuideOptionManager.h"
#include "LoginBase.h"
#include "MMOHelperManager.h"
#include "MMOSystemActivateManager.h"
#include "MMOHandbookManager.h"
#include "MallManager.h"
#include "MMOPowerManager.h"
#include "ActivityManager.h"
#include "MMOJourneyManager.h"
#include "MMOGameEvent_Splash.h"
#include "Mir2Attack.h"
#include "MMOAssistManager.h"
#include "..\UStallManager.h"
#include "MMOChatManager.h"
#include "MMOJigsawManager.h"
#include "TestCase.h"
#include "MMOAuctionManager.h"

IMPLEMENT_CLASS(UMMOGameState)
IMPLEMENT_CLASS(UMMOGameState_Login)
IMPLEMENT_CLASS(UMMOGameState_Loading)
IMPLEMENT_CLASS(UMMOGameState_InGame)
IMPLEMENT_CLASS(UMMOGameState_Selecting)


class LoginEventProcessor : public INativeGameEventHandler
{
public:

	LoginEventProcessor()
	{
		MMOUI::RegisterEventHandler(this, GameEvent_Splash);
	}

	virtual ~LoginEventProcessor()
	{

	}

	void GameEvent_CreateSuccess()
	{
		AMMOPlayerController* PC = MMOFacade::getSingleton().GetMainController();
		if(PC)
		{
			LoginBase::getSingleton().SelectRole(PC->RoleList.Num()-1);
		}


		//MMO::ActiveKismetRemoteEvent(TEXT("StopAll"));
		INT nClass = LoginBase::getSingleton().CreateRoleClass;
		INT nSex   = LoginBase::getSingleton().CreateRoleSex;
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, TRUE, "");
	}

	void GameEvent_EnterGameSuccess()
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
	}

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt)
	{
		if(evt->GetEventType() == GameEvent_Splash )
		{
			MMOGameEvent_Splash* loginEvt = (MMOGameEvent_Splash*)evt;
			if(loginEvt != NULL)
			{

				switch(loginEvt->EvtType)
				{		
				case MMOGameEvent_Splash::ESET_ShowSplashUI:
					//eventSetUIPlayerVisible(loginEvt->ShowSplashUI);
					break;

				case MMOGameEvent_Splash::ESET_LoginGameSuccess:
					//GameEvent_LoginGameSuccess();		
					LoginBase::getSingleton().CreateListRoleIni();
					break;

				case MMOGameEvent_Splash::ESET_CreateRoleSuccess:
					//				SelectRoleIndex = loginEvt->SelectRoleIndex;
					GameEvent_CreateSuccess();			
					break;


				case MMOGameEvent_Splash::ESET_EnterGameSuccess:

					GameEvent_EnterGameSuccess();
					break;			

				case MMOGameEvent_Splash::ESET_EnterGameEnd:
					//GameEvent_EnterGameEnd();
					break;			

				case MMOGameEvent_Splash::ESET_CreateListRoleCheck:

					MMO::ActiveGameState( EGameState_Selecting );
					LoginBase::getSingleton().IsFromLogin = FALSE;
					LoginBase::getSingleton().EnterSelectRole();
					LoginBase::getSingleton().SelectRole(LoginBase::getSingleton().SelectRoleIndex);

					break;			

					// 				case MMOGameEvent_Splash::ESET_UpdateRoleListUI:
					// 					{
					// 						if(SelectPanel)SelectPanel->UpdateRoleList();
					// 
					// 						if(RevertPanel&&RevertPanel->IsVisible)RevertPanel->UpdateRevertPanelList();
					// 					}
					// 					break;
				};
			}
		}
	}

};

//------------------------------------------------------------------------
// UMMOGameState
//------------------------------------------------------------------------
BYTE UMMOGameState::GetID()
{
	return EGameState_None;
}

void UMMOGameState::OnPreEnter()
{

}

void UMMOGameState::OnEnter()
{

}

void UMMOGameState::OnLeave()
{

}

void UMMOGameState::OnAftLeave()
{

}

void UMMOGameState::OnUpdate()
{

}

void UMMOGameState::NativeInit()
{
}

void UMMOGameState::ActiveState(BYTE sta)
{
	MMO::ActiveGameState((EGameState)sta);
}

BYTE UMMOGameState::GetCurrentGameState()
{
	return MMO::GetCurrentGameState();
}


//------------------------------------------------------------------------
// UMMOGameState_Login
//------------------------------------------------------------------------
BYTE UMMOGameState_Login::GetID()
{
	return EGameState_Login;
}

void UMMOGameState_Login::NativeInit()
{
	Super::NativeInit();

	static LoginEventProcessor g_LoginEventProcessor;
}

extern bool g_isSDOLogin;

void UMMOGameState_Login::OnUpdate()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
}

extern bool g_isLogSuccess;
extern void ResetRenderTarget();

void UMMOGameState_Login::OnEnter()
{
	if(GSystemSettings.AutoFPSLevel != 5)
	{
		FlushRenderingCommands();
		GSystemSettings.AutoFPSLevel = 5;
		ResetRenderTarget();
	}

	// keep this last
#ifdef _DEBUG
	RunTestCase();
#endif

	if(GIsEditor)
	{
		return;
	}


#if 0
	TArray<FString>  tArrUrlStr;
	tArrUrlStr.AddItem(_T("http://wenjuan.sdo.com:9091/survey?surveyid=13834&appid=1&areaid=1&groupid=1"));
	WebHtmlManager::getSingleton().NotifyOpenSurveyPage(tArrUrlStr);
#endif
	
	CMessageHandler::GetInstance().SetDisableSend(false);

	MMO::ActiveKismetRemoteEvent(TEXT("ReturnLogin"));

	MMOFacade::SetMouseCursor(EMC_Arrow);

	if(g_isSDOLogin)
	{
		check(EventAdapter == NULL);
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_ShowOptionPanel);
		evts.AddItem(GameEvent_HideOptionPanel);
		EventAdapter = new TGameEventAdapter<UMMOGameState_Login>(this, evts);

		//MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("PAUSE"));
		
		//if(ErrorCodeCauseReturnLogin != 0)
		//{
		//	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);

		//	//FString enumVal = InEnum->GetEnum(ErrorCodeCauseReturnLogin).ToString();
		//	FString enumVal(g_szErrorCodeNameList[ErrorCodeCauseReturnLogin]);
		//	FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal));
		//	
		//	::MessageBox(NULL, *localVal, TEXT(""), MB_OK|MB_SYSTEMMODAL);
		//}

// 		static bool firstLogin = true;
// 
// 		if(firstLogin)
// 		{
// 			firstLogin = false;
// 		}
// 		else
// 		{
// 			extern void ShowLoginDialog();
// 			ShowLoginDialog();
// 		}
	}

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{
		ctrl->GotoState(TEXT("LoginState"));
	}

#if 1
	MMOUI::GetUIManager()->ShowAllPanels(FALSE);

	//if(LoginMovie)
	//{
	//	LoginMovie->eventSetUIPlayerVisible(TRUE);
	//	UGFxObject* rootObj = LoginMovie->GetVariableObject(TEXT("root"));
	//	if(rootObj)
	//	{
	//		rootObj->GotoAndPlayI( 0 );
	//	}

	//	//the first login goes directly into the Login screen, no need to call this here
	//	static UBOOL IsFirstOpenGame = TRUE;
	//	if(  IsFirstOpenGame )
	//	{
	//		IsFirstOpenGame = FALSE;
	//	}
	//	else
	//		LoginMovie->StageJump(ESS_LoginEnd);
	//	FGFxEngine::GetEngine()->SetFocusMovie(LoginMovie->pMovie);
	//}




	MMOFacade::getSingleton().NotifyEnterLogin();
	MMOHeavenManager::getSingleton().NotifyGameStateChange();
	MallManager::getSingleton().NotifyGameStateChange();

	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MessageBox"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MsgInfo"), TRUE, "");

#endif

	if(!GIsMMOBenchmarking && ErrorCodeCauseReturnLogin != 0)
	{
		//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);

		//FString enumVal = InEnum->GetEnum(ErrorCodeCauseReturnLogin).ToString();
		FString enumVal = GetErrorCodeEnumString(ErrorCodeCauseReturnLogin);//FString enumVal(g_szErrorCodeNameList[ErrorCodeCauseReturnLogin]);
		FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal));

		FScriptDelegate nullFun(EC_EventParm);
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal, 0, nullFun, nullFun,FALSE);

		ErrorCodeCauseReturnLogin = 0;
	}

	if(g_isSDOLogin)
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");

		//extern HWND GGameWindow;
		//ShowWindow(GGameWindow, SW_HIDE);

		g_isLogSuccess = false;
	}
	else
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
	}

	// clear main player effect
	EffectProcessor::GetSingleton().DeleteAllComponent();

	MMOQuestManager::getSingleton().Clear();

}

void UMMOGameState_Login::OnLeave()
{
	//LoginMovie->eventSetUIPlayerVisible(FALSE);	
	//MMOTargetManager::getSingleton().Clear();
	FSMIO::getSingleton().State.ClearAllState();
	if(g_isSDOLogin)
	{
		if(EventAdapter)
		{
			EventAdapter->Delete();
			EventAdapter = NULL;
		}
		//MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("PAUSE"));
	}
	
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  FALSE, TEXT(""));
}

void UMMOGameState_Login::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(g_isSDOLogin)
	{
		switch(evt->GetEventType())
		{
		case GameEvent_ShowOptionPanel:
			extern void HideSDOLoginDialogWnd();
			HideSDOLoginDialogWnd();
			break;

		case GameEvent_HideOptionPanel:
			extern void ShowSDOLoginDialogWnd(bool bForce);
			ShowSDOLoginDialogWnd(false);
			break;
		}
	}
}

//------------------------------------------------------------------------
// UMMOGameState_Selecting
//------------------------------------------------------------------------
BYTE UMMOGameState_Selecting::GetID()
{
	return EGameState_Selecting;
}

extern FString GetBinkMoviePath(const FString& fileName);

void UMMOGameState_Selecting::OnEnter()
{
	if(GSystemSettings.AutoFPSLevel != 5)
	{
		FlushRenderingCommands();
		GSystemSettings.AutoFPSLevel = 5;
		ResetRenderTarget();
	}

	CMessageHandler::GetInstance().SetDisableSend(false);

	MMOFacade::SetMouseCursor(EMC_Arrow);

	// clear main player effect
	EffectProcessor::GetSingleton().DeleteAllComponent();

	// clear related items when switching characters

	MMOFacade::getSingleton().ClearSoldItems();
	MMOFacade::getSingleton().ClearItems();
	MMOFacade::getSingleton().ClearPlayerEquip();
	MMOFacade::getSingleton().ClearMainPlayer();

	IdleAnimName = MMOFacade::getSingleton().GetMainPlayer()->PawnAnimInfo.IdleAnim;
	MMOFacade::getSingleton().GetMainPlayer()->PawnAnimInfo.IdleAnim = LoginIdleAnimName;
	MMOFacade::getSingleton().GetMainPlayer()->RefreshAnim();
	//MMOFacade::getSingleton().GetMainPlayer()->SetIdleAnim(LoginIdleAnimName);
	MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->byCombatPoseOn = TRUE;
	MMOFacade::getSingleton().GetMainPlayer()->bHoldWeapon = FALSE;

	MailManager::getSingleton().ClearAllMails();
	//master/apprentice related data
	MasterStudentManager::getSingleton().ClearAll();
	MMOQuestManager::getSingleton().Clear();
	//scene
	MMOMapDataManager::getSingleton().ClearTeleportDynamicData();
	
	//MMOArenaManager::getSingleton().ClearArena();
	// Achievement
	MMOAchievementManager::getSingleton().Clear();
	// clan
	MMOGuildManager::getSingleton().Clear();
	// Tutorial
	MMOGuideManager::getSingleton().Clear();
	// battlefield
	MMOBattleManager::getSingleton().SetBattleState( EBTLS_NONE );
	// cd
	MMOCoolDownManager::getSingleton().ClearAllCD();
	// help system
	MMOHelperManager::getSingleton().Clear();
	// new system unlocked
	MMOSystemActivateManager::getSingleton().Clear();

	MMOHandbookManager::getSingleton().Clear();

	MMOUI::GetUIManager()->ShowAllPanels(FALSE);

	TeamManager::getSingleton().ClearTeamInfo();
	MMOMsgButtonManager::getSingleton().ClearAll();
	FriendChatManager::getSingleton().ClearAll();
	FriendManager::getSingleton().ClearAll();
	MMOHeavenManager::getSingleton().NotifyGameStateChange();
	MallManager::getSingleton().NotifyGameStateChange();
	//clear hurt data
	MMOHurtManager::getSingleton().RestHurt();
	//Activity
	ActivityManager::getSingleton().Clear();

	MMOJourneyManager::getSingleton().Clear();
	MMOAssistManager::getSingleton().Clear();
	UStallManager::getSingleton().Clear();

	MMOJigsawManager::getSingleton().RemoveMessageButtionBar();

	//auction house
	MMOAuctionManager::getSingleton().Reset();

 	LoginBase::getSingleton().EnterSelectRole();
 	LoginBase::getSingleton().SelectRole(LoginBase::getSingleton().SelectRoleIndex);

	// clear all UI data
	for(UIMaps::TIterator PairIt(AllUIMaps); PairIt; ++PairIt)
	{
		if(PairIt.Value()){
			PairIt.Value()->ClearUI();
		}
	}

	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MsgInfo"), TRUE, "");


	AMMOPlayer* pMainPlayer   = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer)
	{
		pMainPlayer->ActorAttributes->IsGreyName = FALSE;
	}

	MMOGameEvent_Any enterGameEvt(GameEvent_EnterSelectingRole);
	MMOUI::PostGameEvent(&enterGameEvt);

	AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
	

	if (PC && PC->RoleList.Num()>0)
	{

//--	MMO::ActiveKismetRemoteEvent(TEXT("StopAll"));
		if(	LoginBase::getSingleton().IsFromLogin == TRUE)
		{
			LoginBase::getSingleton().IsFromLogin = FALSE;
			//MMO::ActiveKismetRemoteEvent(TEXT("2-3"));//after this kismet ends, an action triggers showing Panel_Login_Select
			MMO::ActiveKismetRemoteEvent(TEXT("Login-Select"));

#if BETA_7_7
			if (GFileManager->FileSize(*GetBinkMoviePath("open_ok")) <= 0)
			{
				MMO::ActiveKismetRemoteEvent(TEXT("PlayOpenOk"));
			}
			else
			{
				MMO::ActiveKismetRemoteEvent(TEXT("DontPlayOpenOk"));
			}
#endif

		}
		else
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, TRUE, "");
			MMO::ActiveKismetRemoteEvent(TEXT("ReturnSelect"));


#if BETA_7_7
			MMO::ActiveKismetRemoteEvent(TEXT("DontPlayOpenOk"));
#endif

		}

		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Queue, FALSE, "");
	}
	else
	{

//--	MMO::ActiveKismetRemoteEvent(TEXT("StopAll"));
		if(	LoginBase::getSingleton().IsFromLogin == TRUE)
		{
			//LoginBase::getSingleton().IsFromLogin = FALSE;
			//GFullScreenMovie->GameThreadPlayMovie(MM_PlayOnceFromStream, TEXT("FirstLogin.bik"));
			//GFullScreenMovie->GameThreadWaitForMovie();
			//GFullScreenMovie->GameThreadStopMovie();

			//MMO::ActiveKismetRemoteEvent(TEXT("2-0-A"));//after this kismet ends, an action triggers showing Panel_Login_Create
			MMO::ActiveKismetRemoteEvent(TEXT("Login-Select-Create"));
		}
		else
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, TRUE, "");
		}

		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  FALSE, "");
	}
	GSystemSettings.SetForceUseLodZeroLevel( TRUE );
}

void UMMOGameState_Selecting::OnLeave()
{

// 	UMMOSplashGFXPlayer *p = Cast<UMMOSplashGFXPlayer>(MMOUI::GetUIManager()->FindMoviePlayer(MMOUI::GetUIManager()->MoviePlayer_Login));
// 	if(p)
// 	{
// 		//MMOGameEvent_Splash loginEvt;
// 		//loginEvt.EvtType = MMOGameEvent_Splash::ESET_ShowSplashUI;
// 		//loginEvt.ShowSplashUI = FALSE;
// 		//p->OnNativeGameEvent(&loginEvt);
// 		//p->ShowAllUI(FALSE);
// 		p->eventSetUIPlayerVisible(FALSE);
// 		p->Advance(0.0f);
// 	}

	MMOGameEvent_Any gameEvt(GameEvent_LeaveSelectingRole);
	MMOUI::PostGameEvent(&gameEvt);
	
	//MMOGameEvent_Splash loginEvt;
	//loginEvt.EvtType = MMOGameEvent_Splash::ESET_ShowSplashUI;
	//loginEvt.ShowSplashUI = FALSE;
	//MMOUI::PostGameEvent(&loginEvt);

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
	MMOUI::GetUIManager()->delegateUpdatePetState(E_ps_Invalid);
	//MMOFacade::getSingleton().GetMainPlayer()->SetIdleAnim(IdleAnimName);

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		mainPlayer->PawnAnimInfo.IdleAnim = IdleAnimName;
		mainPlayer->RefreshAnim();
	}
	GSystemSettings.SetForceUseLodZeroLevel( FALSE );
}

//------------------------------------------------------------------------
// UMMOGameState_Loading
//------------------------------------------------------------------------
BYTE UMMOGameState_Loading::GetID()
{
	return EGameState_Loading;
}

void UMMOGameState_Loading::OnPreEnter()
{
	if (LoadMovie)
	{
		PreLoading=TRUE;
		LoadMovie->GetVariableObject(TEXT("root"))->SetVisible(TRUE);
		LoadMovie->eventSetVisible(TRUE);
		LoadMovie->OnEnter();
		LoadMovie->eventPreEnter();
	}
}

void UMMOGameState_Loading::OnEnter()
{
	GIsLoadingMaps = TRUE;
	GEngine->bSmoothFrameRate = FALSE;

	if (LoadMovie&&PreLoading==FALSE)
	{
		LoadMovie->GetVariableObject(TEXT("root"))->SetVisible(TRUE);
		LoadMovie->eventSetVisible(TRUE);
		LoadMovie->OnEnter();
		LoadMovie->eventAftEnter();
	}
	MMOFacade::getSingleton().PlayUISound(TEXT("menu_EnterWorld"));
	AInstancedShadowPlaneActor::ReleaseAllShadowPlaneActor();
}

extern FLOAT g_lastCheckTime;
extern FLOAT s_averageFPS;

void UMMOGameState_Loading::OnLeave()
{
	SET_WARN_COLOR(COLOR_PURPLE);
	warnf(NAME_Log, TEXT("UMMOGameState_Loading::OnLeave"));
	CLEAR_WARN_COLOR();

	GIsLoadingMaps = FALSE;
	UObject::SetStartRecordLoadInfo(FALSE);
	GEngine->bSmoothFrameRate = TRUE;
	PreLoading=FALSE;
	if (LoadMovie)
	{
		//	LoadMovie->OnLeave();
		//	LoadMovie->GetVariableObject(TEXT("root"))->SetVisible(FALSE);
		//	LoadMovie->eventSetVisible(FALSE);
		LoadMovie->eventPreLeave();
	}
	// After Loading ends, clear the ambient music that needs to keep playing during Loading
	if (GWorld && GWorld->GetWorldInfo())
	{
		GWorld->GetWorldInfo()->ClearAcrossLevelMusicByCueName(TEXT("Bgm_RoleRoom"));	// zhaojun@snda, trick
	}

	// only check after a delay once loading is done
	g_lastCheckTime = 0;
	s_averageFPS = 0;
}

void UMMOGameState_Loading::OnAftLeave()
{
	//GIsLoadingMaps = FALSE;
	//GEngine->bSmoothFrameRate = TRUE;

	//if (LoadMovie)
	//{
	//	LoadMovie->OnLeave();
	//	LoadMovie->GetVariableObject(TEXT("root"))->SetVisible(FALSE);
	//	LoadMovie->eventSetVisible(FALSE);
	//}
	//// After Loading ends, clear the ambient music that needs to keep playing during Loading
	//if (GWorld && GWorld->GetWorldInfo())
	//{
	//	GWorld->GetWorldInfo()->ClearAcrossLevelMusicByCueName(TEXT("Bgm_RoleRoom"));	// zhaojun@snda, trick
	//}
}

void UMMOGameState_Loading::OnUpdate()
{
	//if (LoadMovie)
	//{
	//	LoadMovie->OnUpdate();
	//}
}
//------------------------------------------------------------------------
// UMMOGameState_InGame
//------------------------------------------------------------------------
BYTE UMMOGameState_InGame::GetID()
{
	return EGameState_InGame;
}

extern UBOOL g_focusInTextInput;
extern bool bUseGMCmd;

void UMMOGameState_InGame::OnEnter()
{
	if(GIsEditor)
	{
		return;
	}

	if(GFileManager->FileSize(TEXT("XF80EAC3DF9B")) != INDEX_NONE)
	{
		debugf(TEXT("GM input enabled!"));
		bUseGMCmd = TRUE;
	}

	g_focusInTextInput = FALSE;
	FGFxEngine::GetEngine()->PlayerControlRequireReleaseEvt = FALSE;
	
	UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
	if(Chat)
	{
		Chat->ShowInputArea(FALSE);
	}

	static bool initUI = false;
	if(!initUI)
	{
		initUI = true;
		MMOUI::StartAllPanels();
	}

	g_updateMainPlayerHudName = true;



	//scene teleporter update
	MMOMapDataManager::getSingleton().ActiveCurSceneTeleports();
	//update map region (must be before NotifyEnterScene, the map uses this data)
	MMOMapDataManager::getSingleton().InitCurWorldMapRegionData();
	Mir2Attack::getSingleton().InitPlayerClassX();
	MMOFacade::getSingleton().NotifyEnterScene();
	// TODO: restore all UI according to the last display state
 	MMOUI::GetUIManager()->ShowAllPanels(FALSE,TRUE);
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("Buff"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("Minimap"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("Chat"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_HudInfo_MyInfo , TRUE, "" );
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_HudInfo_TeamHint , TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Friend_FriendChatHint , TRUE, "");

	//arena queue
	//if( MMOArenaManager::getSingleton().GetArenaStep() == ArenaStep_Queuing || MMOArenaManager::getSingleton().GetArenaStep() == ArenaStep_WaitEnter )
	//{
	//	//somehow feels this is not right, does the state change when switching maps?
	//	MMOArenaManager::getSingleton().OpenArenaQueue( 0 , FALSE );
	//}
	
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_NoticeInfo , TRUE, "", 4);
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MsgInfo"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("SceneHint"), TRUE, "");
	//MMOUI::GetUIManager()->ShowPanelEx(TEXT("FriendChatLevel"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MenuBar"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("TeamMate"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("DamageLevel"), TRUE, "");
	//MMOUI::GetUIManager()->ShowPanel(TEXT("BattleState"), TRUE, "");	//commented out for now, requested by Chen Kai
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("HintButtonBar"), TRUE, "");	
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MsgButtonBar"), TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(TEXT("MessageBox"), TRUE, "");	
	//MMOUI::GetUIManager()->delegateShowHintSceneWidget();
    //Drop default visibility changes according to config
    //MMOUI::GetUIManager()->ShowPanelByDefaultVisible( TEXT("DropItemTips"), "" );

	//whether to show HurtUI
	MMOUI::GetUIManager()->ShowHurtPanel(MMOUI::GetUIManager()->DrawHurt);

	//show skillbar last, because the last one shown sets the focusplayer and responds to the keyboard
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Skillbar, TRUE, TEXT(""));

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_PickWidget_DorpItemTips, TRUE, "");	
   
	//if in dead state
	if(  MMOFacade::getSingleton().GetMainPlayer()->eventIsDead() ){
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death,TRUE,"");
	}

	// the server doesn't send this when switching scenes, so update it ourselves
	MailManager::getSingleton().NotifyNewMailCount();
	//whether it is a battlefield, show the scoreboard
	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	if( pBattleMgr->IsInBattle() ){
		//show battle UI
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard, TRUE, "");
		//clear battle enter data
		pBattleMgr->ResetJoinedBattleData();
	}

	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_QuestTrace, TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Journey, TRUE, "");

	MMOHeavenManager::getSingleton().NotifyGameStateChange();
	MasterStudentManager::getSingleton().NotifyEnterGame();
	MallManager::getSingleton().NotifyGameStateChange();
	//load recent contacts of the mail system
	MailManager::getSingleton().LoadData();

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{
		ctrl->GotoState(NAME_None);
		ctrl->eventClearAllCmd();
	}

	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	//if(player){

	//	player->RefreshCombatPose(TRUE);
	//}

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		player->ResetPlaceActorToGroundTimer();
	}

	// navigation
	MMONavigationManager::getSingleton().InitPathEffect();
	FSMIO::getSingleton().Ctrl.SetKeyType(KT_NONE);
	

	// achievements (when G2C_SYNC_REPUTATION is received, an achievement with existing reputation may already be complete; sending achievement completion to the server then is not received, so send it when entering the game)
	for (int i = 0; i < MAX_NPC_FORCE_ID; ++ i)
	{
		INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), i);
		MMOGameEvent_Achievement evtAch1(adForceReputation, i, reputation, 0);
		MMOUI::PostGameEvent(&evtAch1);
	}

	MMOGameEvent_Achievement evtAch2(adVariousForceCount, 0, 0, 0);
	MMOUI::PostGameEvent(&evtAch2);

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_EnterGame);
	MMOUI::PostGameEvent(&evtGuide);

	MMOAssistManager::getSingleton().NotifyEnterGame();

	MMOGameEvent_SceneLoadFinish evt;
	MMOUI::PostGameEvent(&evt);

	// must be sent after LoadingFinish
	CMessageHandler::GetInstance().Send_CombatPoseOn(FALSE,0);

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Queue, FALSE, "");

	AInstancedShadowPlaneActor::InitAllShadowPlaneActor();

	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MasterInvite,TRUE,"");
	MMOGameEvent_MasterStudent evtMaster(EMasterStudentType::EMST_MasterEnterGame);
	MMOUI::PostGameEvent(&evtMaster);


	M_DumpUsedTabHeaders();
}

void UMMOGameState_InGame::OnLeave()
{
	if (GWorld != NULL)
	{
		// added by zhaojun@snda 2014-12-30, stop the bgm
		AWorldInfo *WorldInfo = GWorld->GetWorldInfo();
		if (WorldInfo != NULL && WorldInfo->MusicComp != NULL)
		{
			WorldInfo->MusicComp->Stop();
		}
	}

#if M_ENABLE_ASYNC_PROC_CHAR
	CMessageHandler::GetInstance().ClearAllCharAppearList();
#endif

	// Tutorial
	MMOGuideManager::getSingleton().Clear();

	MMONavigationManager::getSingleton().ShutdownPathEffect();

	MMOUI::GetUIManager()->ShowAllPanels(FALSE,TRUE);
	//MMOTargetManager::getSingleton().ClearDesireTarget();
	//MMOTargetManager::getSingleton().Clear();
	FSMIO::getSingleton().State.ClearAllState();
	MMOHeavenManager::getSingleton().NotifyGameStateChange();
	//MMO::TargetManager::getSingleton().Clear();
	MMOMsgBoxManager::getSingleton().ClearAll();

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(ctrl)
	{
		ctrl->GotoState(TEXT("LoginState"));
	}

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player)
	{
		player->DisablePlaceActorToGroundTimer();
	}
	//if(player && player->IsInSafeZone)
	//{
	//	player->IsInSafeZone = FALSE;
	//	CMessageHandler::GetInstance().Send_CombatPoseOn(FALSE);
	//	//player->ReflashWeaponSocket();
	//	//player->RefreshAnimSet();
	//}

	// clear ability andd effect processor first
	AbilityProcessor* ap = MMOFacade::getSingleton().GetAbilityProcessor();
	if (ap)
	{
		ap->ClearAllAbilityNode();
	}

	EffectProcessor::GetSingleton().DeleteAllComponent(TRUE);

	MMOFacade::getSingleton().ClearAllCharacter();


	MMOFacade::getSingleton().ClearNPCTemplates();	

	MMOGameEvent_LeaveGame evt;
	MMOUI::PostGameEvent(&evt);

	extern bool g_disableAllInput;
	g_disableAllInput = false;

	g_updateMainPlayerHudName = true;
	FGFxEngine::GetEngine()->PlayerControlRequireReleaseEvt = FALSE;

	MMOPowerManager::getSingleton().Reset();

	MMOChatManager::getSingleton().SaveRecord();
}
