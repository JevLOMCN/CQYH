#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
#include "MMOGameEvent_Splash.h"
#include "LoginBase.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_NewLogin);

void UMMOGFXPlayer_NewLogin::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_NewLogin::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_NewLogin);	

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Splash);
	EventAdapter = new TGameEventAdapter<UMMOGFXPlayer_NewLogin>(this, evts);
}

void UMMOGFXPlayer_NewLogin::UnInit()
{
	Super::UnInit();

}

//void UMMOGFXPlayer_NewLogin::ASRoleMove(INT offX,INT offY)
//{
//	SelectPanel->ASRoleMove(offX,offY);
//}

void UMMOGFXPlayer_NewLogin::ASCheckFirst(UGFxObject *mc)
{
	if(mc){
		AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
		if(PC && PC->RoleList.Num()==0 ){
			mc->SetBool("IsFirst",TRUE);
		}
		else
		{
			mc->SetBool("IsFirst",FALSE);
		}
	}	
}


//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_NewLogin::PostAdvance ( FLOAT DeltaTime )
{
	UGFxMoviePlayer::PostAdvance(DeltaTime);
	if(SelectPanel)SelectPanel->PostAdvance(DeltaTime);
}


void UMMOGFXPlayer_NewLogin::EnterCharactorSelect()
{

}

void UMMOGFXPlayer_NewLogin::ListWidgetMovieEnd()
{

}

void UMMOGFXPlayer_NewLogin::EnterCreateRole()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, TRUE, "");
}

void UMMOGFXPlayer_NewLogin::CreateRoleCancel()
{
	LoginBase::getSingleton().IsCreateOption = FALSE;
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, TRUE, "");
}

void UMMOGFXPlayer_NewLogin::ASOpenSystemSetting()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionConfig,TRUE, "");
}

void UMMOGFXPlayer_NewLogin::ASQuitGame()
{
	MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("exit"));
}

//----------------------------------------------------------------------------
void UMMOGFXPlayer_NewLogin::ASRequestReturnLogin()
{
	//call after OnDisconnected

	CMessageHandler::GetInstance().CloseConnection(true);
	//MMOUI::GetUIManager()->SendReturnLogin();
}

void UMMOGFXPlayer_NewLogin::ASRequestFindRole()
{
	MMOUI::GetUIManager()->ToggleShowPanel(MMOUI::GetUIManager()->Panel_Login_Revert,"");
}
void UMMOGFXPlayer_NewLogin::ASRequestFindRoleID(INT nRoleID)
{
	if(RevertPanel)
		RevertPanel->UnddeleteRole(nRoleID);
}
void UMMOGFXPlayer_NewLogin::ASRequestDelectRole(INT nRoleID)
{
	if(RevertPanel)
		RevertPanel->DeleteRoleForever(nRoleID);
}
void UMMOGFXPlayer_NewLogin::ASRequestJump2CreateRole()
{
	LoginBase::getSingleton().IsCreateOption = FALSE;
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, TRUE, "");
}

//----------------------------------------------------------------------------
void UMMOGFXPlayer_NewLogin::GameEvent_LoginGameSuccess()
{
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User, FALSE, "");
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, TRUE, "");
}
void UMMOGFXPlayer_NewLogin::GameEvent_CreateSuccess()
{
	AMMOPlayerController* PC = MMOFacade::getSingleton().GetMainController();
	if(PC){
		LoginBase::getSingleton().SelectRole(PC->RoleList.Num()-1);
	}


	//MMO::ActiveKismetRemoteEvent(TEXT("StopAll"));
	INT nClass = LoginBase::getSingleton().CreateRoleClass;
	INT nSex   = LoginBase::getSingleton().CreateRoleSex;
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, TRUE, "");
}

void UMMOGFXPlayer_NewLogin::GameEvent_EnterGameSuccess()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
}
void UMMOGFXPlayer_NewLogin::GameEvent_EnterGameEnd()
{

}

void UMMOGFXPlayer_NewLogin::OnNativeGameEvent(struct MMOGameEvent* evt)
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

//			case MMOGameEvent_Splash::ESET_JumpCreateRole:
////				StageJump( ESS_CreateRole );
//				break;

			case MMOGameEvent_Splash::ESET_EnterGameSuccess:

				GameEvent_EnterGameSuccess();
				break;			

			case MMOGameEvent_Splash::ESET_EnterGameEnd:
				// nothing to do GameEvent_EnterGameEnd();
				break;			

			case MMOGameEvent_Splash::ESET_CreateListRoleCheck:

				MMO::ActiveGameState( EGameState_Selecting );
				LoginBase::getSingleton().IsFromLogin = FALSE;
				LoginBase::getSingleton().EnterSelectRole();
				LoginBase::getSingleton().SelectRole(LoginBase::getSingleton().SelectRoleIndex);

				break;			

			case MMOGameEvent_Splash::ESET_UpdateRoleListUI:
				{
					if(SelectPanel)SelectPanel->UpdateRoleList();

					if(RevertPanel&&RevertPanel->IsVisible)RevertPanel->UpdateRevertPanelList();
				}
				break;
			};
		}
	}
}


UBOOL UMMOGFXPlayer_NewLogin::NativeCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	UMMOUIBase* UI = MMOUI::GetTopmostPanel();
	if(UI && UI->FriendlyName == MMOUI::GetUIManager()->Panel_OptionConfig)
	{
		return FALSE;
	}
	return TRUE;
}


