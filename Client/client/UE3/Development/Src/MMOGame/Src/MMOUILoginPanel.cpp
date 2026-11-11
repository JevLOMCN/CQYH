#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
#include "LoginBase.h"
#include "MMOMsgBoxManager.h"
#include "MMOLoginManager.h"
#include "WebHtmlManager.h"
#include "../Launch/SDOLogin.h"

IMPLEMENT_CLASS(UMMOUILoginPanel);

IMPLEMENT_CLASS(USeqAct_HideSDOLogin);
IMPLEMENT_CLASS(USeqAct_ShowSDOLogin);

extern bool g_isSDOLogin;
bool g_isFirstLogin = true; 

void UMMOUILoginPanel::Init()
{
	Super::Init();

	TArray<FString> ipNameList = LoginBase::getSingleton().GetLoginIPNameList();

	FString strServerList = TEXT("");
	for( INT i = 0; i< ipNameList.Num(); i++)
	{
		strServerList = strServerList + *ipNameList(i) + TEXT("=");
	}
	eventCallSetServerList(strServerList);


// 	UMMOSplashGFXPlayer *p = Cast<UMMOSplashGFXPlayer>(MMOUI::GetUIManager()->FindMoviePlayer(MMOUI::GetUIManager()->MoviePlayer_Login));
// 	if(p)
// 	{
// 		p->GetVariableObject(TEXT("root"))->SetBool("visible",FALSE);
// 	}

	if(g_isSDOLogin)
	{
		eventCallShowLoginInputBoxes(FALSE);
	}

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ShowLoginSetUp);
	evts.AddItem(GameEvent_ShowLoginInputBox);
	evts.AddItem(GameEvent_GetMessageAck);
	EventAdapter = new TGameEventAdapter<UMMOUILoginPanel>(this, evts);
	
}

void UMMOUILoginPanel::ShowFreePkDlg()
{
	//warnf(NAME_Warning, TEXT("!!!! UMMOUILoginPanel::ShowFreePkDlg !!!!"));

	static UBOOL bFirst = TRUE;
	if(bFirst)
	{

		extern bool g_isSDOLogin;
		if(g_isSDOLogin)
		{
			//extern void HideSDOLoginDialogWnd();
			HideSDOLoginDialogWnd();	
		}
		else
		{
			eventCallShowLoginInputBoxes(FALSE);
		}
		eventCallShowLoginSetup(FALSE);

		FScriptDelegate funOk(EC_EventParm);
		funOk.FunctionName = TEXT("funOk");
		funOk.Object = this;
		FScriptDelegate funCancel(EC_EventParm);
		funCancel.FunctionName = TEXT("funCancel");
		funCancel.Object = this;
		UGFxObject* obj = MMOMsgBoxManager::getSingleton().AddModalMessageBox(TEXT("本游戏区是自由对战模式,遵守游戏规则,可获得畅快游戏体验,是否同意自由对战"),0,funOk,funCancel);
		
		TArray<struct FASValue> asFunArgs;
		FASValue asTitleName(EC_EventParm);
		asTitleName.Type = AS_String;
		asTitleName.S = TEXT("提示");
		asFunArgs.AddItem( asTitleName );
		if(obj) obj->Invoke( TEXT("SetWindowTitle"), asFunArgs );

		bFirst = FALSE;
	}
	else
	{
		if(g_isSDOLogin)
		{
			//extern void ShowLoginDialog();
			ShowLoginDialog();
		}
	}
}


void UMMOUILoginPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v==TRUE){
		
		LoginBase::getSingleton().CreateListRoleIni();
		LoginBase::getSingleton().IsFromLogin = TRUE;
		MMO::ActiveKismetRemoteEvent(TEXT("Login_Loop"));
//--		MMO::ActiveKismetRemoteEvent(TEXT("StopAll"));
//		MMO::ActiveKismetRemoteEvent(TEXT("2"));

//		eventCallEffectshow();
		eventCallUpdateUser(MMOFacade::getSingleton().GetMainController()->SaveUserName);

		if(g_isFirstLogin)
		{
			eventCallPlayLogo();
		}
		else
		{

			if(MMOMsgBoxManager::getSingleton().GetMsgNumByType(EMsgboxType_Modal)==0)
			{
				//extern void ShowSDOLoginDialogWnd(bool bForce);
				ShowSDOLoginDialogWnd(true);
			}
			
		}
		eventCallResetFocus();
	}
}

void UMMOUILoginPanel::funCancel()
{
	MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("exit"));
}

void UMMOUILoginPanel::funOk()
{
	extern bool g_isSDOLogin;
	if(g_isSDOLogin)
	{
		//extern void ShowLoginDialog();
		ShowLoginDialog();	
		g_isFirstLogin = false;
	}
	else
	{
		eventCallShowLoginInputBoxes(TRUE);
	}
	eventCallShowLoginSetup(TRUE);

}

void UMMOUILoginPanel::OnFocusGained()
{
	if(!g_isSDOLogin){
		eventCallResetFocus();
	}
}

void UMMOUILoginPanel::OnFocusLost()
{

}


extern BYTE g_loginServerGroup;
extern TCHAR g_sdoLoginIP[MAX_PATH];

void UMMOUILoginPanel::LoginGame( class UGFxObject* loginData ) 
{

	if(loginData==NULL)return;

	FString strUser		= loginData->GetString(TEXT("name"));
	FString strPassword = loginData->GetString(TEXT("pwd"));
	FString strServer	= loginData->GetString(TEXT("ip"));
	FString loginIP		= TEXT("");
	FString groupNumb = loginData->GetString(TEXT("groupNumb"));
	g_loginServerGroup = (BYTE)appAtoi(*groupNumb);

	debugf(TEXT("****** g_loginServerGroup:%s"), *groupNumb);

	if(strUser==TEXT(""))
	{
		FScriptDelegate funNone(EC_EventParm);
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MsgBox.text10")),0,funNone,funNone,FALSE);
		//class'MMOUIManager'.static.AddModalMessageBox( ParseLocalizedPropertyPath("MMOGame.MsgBox.text10") , 0 ,none ,none,false);
		return;
	}

	// oscar@snda 如果有传命令行参数的IP就不要从列表选择
	FString ipString;
	if(Parse(appCmdLine(), TEXT("-ip="), ipString, FALSE))
	{
		loginIP = g_sdoLoginIP;
	}
	else
	{
		LoginBase::getSingleton().GetIPSelected(strServer,loginIP);
	}

	if( strServer.Len()>0  )
	{
		MMOFacade::getSingleton().GetMainController()->SaveUserName = strUser;
		MMOFacade::getSingleton().GetMainController()->SaveConfig();
	}

	g_isFirstLogin = false;
	MMOFacade::getSingleton().GetMainController()->LoginGame(loginIP , LoginBase::getSingleton().GetLoginPort(), strUser,strPassword);
}



void UMMOUILoginPanel::ASOpenSystemSetting()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionConfig,TRUE, "");
}

void UMMOUILoginPanel::ASGameRecharge()
{
	//HideSDOLoginDialogWnd();
	//WebHtmlManager::getSingleton().OpenRechargeYuanBao();
	WebHtmlManager::getSingleton().OpenRechargeYuanBaoByIE();
}

void UMMOUILoginPanel::ASPlayCGBik()
{
	MMO::ActiveKismetRemoteEvent(TEXT("Play_Bik"));
}

void USeqAct_ShowSDOLogin::Activated()
{
	Super::Activated();
	ShowSDOLoginDialogWnd(true);
}


void USeqAct_HideSDOLogin::Activated()
{
	Super::Activated();
	HideSDOLoginDialogWnd();
}

void UMMOUILoginPanel::ASQuitGame()
{
	MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("exit"));
}

void UMMOUILoginPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	Super::OnNativeGameEvent(evt);

	switch(evt->GetEventType())
	{	
	case GameEvent_ShowLoginSetUp:
		{
			MMOGameEvent_Login_ShowSetUp* e = (MMOGameEvent_Login_ShowSetUp*)evt;
			eventCallShowLoginSetup(e->show);
		}
		break;
	case GameEvent_ShowLoginInputBox:
		{
			MMOGameEvent_Login_ShowInputBox* e = (MMOGameEvent_Login_ShowInputBox*)evt;
			eventCallShowLoginInputBoxes(e->show);
		}
		break;
	case GameEvent_GetMessageAck:
		{
			if(g_isSDOLogin && MMO::GetCurrentGameState()==EGameState_Login)
			{
				if(MMOMsgBoxManager::getSingleton().GetMsgNumByType(EMsgboxType_Modal)==0)
				{
					//extern void ShowSDOLoginDialogWnd(bool bForce);
					ShowSDOLoginDialogWnd(true);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUILoginPanel::EnterScene()
{
}
void UMMOUILoginPanel::ChangeScene()
{
}
void UMMOUILoginPanel::EnterLogin()
{
}