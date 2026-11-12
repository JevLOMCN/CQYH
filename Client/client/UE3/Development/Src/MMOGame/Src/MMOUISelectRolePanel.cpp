#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
#include "LoginBase.h"
#include "MMOMsgBoxManager.h"
#include "MMOGameEvent_Splash.h"
#include "MMOVipManager.h"

IMPLEMENT_CLASS(UMMOUISelectRolePanel);


void UMMOUISelectRolePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Splash);
	EventAdapter = new TGameEventAdapter<UMMOUISelectRolePanel>(this, evts);

}

void UMMOUISelectRolePanel::UnInit()
{
	Super::UnInit();

}




void UMMOUISelectRolePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	Super::OnNativeGameEvent(evt);

	if(evt->GetEventType() == GameEvent_Splash )
	{
		MMOGameEvent_Splash* loginEvt = (MMOGameEvent_Splash*)evt;
		if(loginEvt != NULL)
		{
			switch(loginEvt->EvtType)
			{	
			case MMOGameEvent_Splash::ESET_UpdateRoleListUI:
				{
					UpdateRoleList();
				}
				break;
			}
		}
	}
}

void UMMOUISelectRolePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v==TRUE)
	{
		//Default to the last logged-in character
		setLastSelectRole();

		UpdateRoleList();
		eventCallEffectshow();
//--		MMO::ActiveKismetRemoteEvent(TEXT("StopAll"));
//		MMO::ActiveKismetRemoteEvent(TEXT("3-LOOP"));
		MMO::ActiveKismetRemoteEvent(TEXT("Select_Loop"));
		LoginBase::getSingleton().IsCreateOption = FALSE;

		//Play time
		UINT32 leftSec = MMOVipManager::getSingleton().GetMyLeftGameTimeInSec();
		FString titleStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Recharge.HintLeftTime") );
		eventCallASSetGameLeftTime( titleStr, (INT)leftSec );

		
	}
}

void UMMOUISelectRolePanel::ASRoleMove(INT offX,INT offY)
{
	RotActor = MMOFacade::getSingleton().GetMainActor();
	if(RotActor){
		FRotator newRot;
		newRot = RotActor->Rotation;
		newRot.Yaw -= offX*100;
		RotActor->SetDesiredRotation(newRot, FALSE, TRUE, -1.f, TRUE);
		RotActor->SetRotation(newRot);
	}
}

void UMMOUISelectRolePanel::PostAdvance(FLOAT DeltaTime)
{
	//Avatar rotation state
	FRotator newRot;

	if( AvatarTurn == EATF_None )
		return;
	else if( AvatarTurn == EATF_TurnLeft )
	{
		if( RotActor != NULL )
		{
			warnf(TEXT("UMMOUISelectRolePanel::PostAdvance1(RotActor->Rotation=%d)"), RotActor->Rotation.Yaw);
			newRot = RotActor->Rotation;
			newRot.Yaw += TurnRotateSpeed * DeltaTime;
			RotActor->SetDesiredRotation(newRot, FALSE, TRUE, -1.f, TRUE);
			RotActor->SetRotation(newRot);
			warnf(TEXT("UMMOUISelectRolePanel::PostAdvance2(RotActor->Rotation=%d)"), RotActor->Rotation.Yaw);
		}
	}
	else if( AvatarTurn == EATF_TurnRight )
	{
		if( RotActor != NULL )
		{
			warnf(TEXT("UMMOUISelectRolePanel::PostAdvance3(RotActor->Rotation=%d)"), RotActor->Rotation.Yaw);
			newRot = RotActor->Rotation;
			newRot.Yaw -= TurnRotateSpeed * DeltaTime;
			RotActor->SetDesiredRotation(newRot, FALSE, TRUE, -1.f, TRUE);
			RotActor->SetRotation(newRot);
			warnf(TEXT("UMMOUISelectRolePanel::PostAdvance4(RotActor->Rotation=%d)"), RotActor->Rotation.Yaw);
		}
	}
	warnf(TEXT("UMMOUISelectRolePanel::PostAdvance()"));
}

void UMMOUISelectRolePanel::SelectRole(INT SelectRoleIndex)
{
	LoginBase::getSingleton().SelectRole(SelectRoleIndex);
}


void UMMOUISelectRolePanel::CallBack_PreLoad()
{
	INT SelectRoleIndex = LoginBase::getSingleton().SelectRoleIndex;

	AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* MainPlayer		= MMOFacade::getSingleton().GetMainPlayer();

	if( PC ==NULL || MainPlayer==NULL )
		return;

	if( SelectRoleIndex < 0 || SelectRoleIndex >= PC->RoleList.Num() )
		return;

	FScriptDelegate FunNone(EC_EventParm);
	MMOUI::GetUIManager()->SetCallBack_PreLoadDelegate(FunNone);
	MMO::ActiveGameState(EGameState_Loading);
	PC->SendRequest_SelectPlayer( SelectRoleIndex );

	//MMO::ProShowLoading(TRUE);
}

void UMMOUISelectRolePanel::EnterGame(INT SelectRoleIndex)
{
	extern UMMOGameState* g_PreStateObj;
	if(g_PreStateObj==NULL)
	{
		MMOFacade::getSingleton().IsReceivedGameData = FALSE;
		MMOUI::GetUIManager()->SetCallBack_PreLoadDelegate(FScriptDelegate(this, TEXT("CallBack_PreLoad")));
		MMO::PreActiveGameState(EGameState_Loading);
	}
	//AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
	//AMMOPlayer* MainPlayer		= MMOFacade::getSingleton().GetMainPlayer();

	//if( PC ==NULL || MainPlayer==NULL )
	//	return;

	//if( SelectRoleIndex < 0 || SelectRoleIndex >= PC->RoleList.Num() )
	//	return;

	//PC->SendRequest_SelectPlayer( SelectRoleIndex );
	////MMO::ProShowLoading(TRUE);
	//MMO::ActiveGameState(EGameState_Loading);
}

void UMMOUISelectRolePanel::UpdateRoleList()
{
	LoginBase::getSingleton().EnterSelectRole();
	UpdateRoleListUI();
	//MMO::ActiveKismetRemoteEvent(TEXT("LoginEvent"));
	LoginBase::getSingleton().SelectRole(LoginBase::getSingleton().SelectRoleIndex);
	eventCallUCSetSelectRole( LoginBase::getSingleton().SelectRoleIndex );
}

void UMMOUISelectRolePanel::UpdateRoleListUI()
{
	AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* MainPlayer		= MMOFacade::getSingleton().GetMainPlayer();

	if( PC ==NULL || MainPlayer==NULL )
		return;

	INT nCount = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	for ( int i = 0 ; i < PC->RoleList.Num() ; ++i )
	{
		UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("UI_LoginWidget.AS3.RoleInfoData"));
		if( itemObj != NULL )
		{
			const FRoleInfo& roleInfo = PC->RoleList( i );

			if(roleInfo.nOverTime != 0)
				continue;

			if( nCount < 4)
			{
				FString infoStr;
				itemObj->SetBool(TEXT("HasRole"), true );
				//Name
				itemObj->SetString(TEXT("RoleName"), roleInfo.RoleName );
				//level
				int iLevel = roleInfo.byLevel;
				if( iLevel <= 0 )
				{
					iLevel = 1;
				}
				infoStr =  MMOUI::ParseLocalizedPropertyPath( "MMOGame.Common.RoleLevel" ) + FString(TEXT(" "));
				infoStr +=appItoa( iLevel );
				itemObj->SetString(TEXT("Level"), infoStr );
				//Class name
				infoStr =  MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa( roleInfo.byCareer ) );
				itemObj->SetString(TEXT("CareerName"), infoStr );
				//gender
				itemObj->SetInt(TEXT("Sex"), roleInfo.bySex -1  );
				//class
				itemObj->SetInt(TEXT("Career"), roleInfo.byCareer );
				// Deletion time
				itemObj->SetInt(_T("OverTime"), 0);
				// character ID
				itemObj->SetInt(_T("RoleID"), roleInfo.dwRoleID);
			}

			dataProvider->SetElementObject( nCount, itemObj );
			nCount++;
		}
	}
	INT SelectRoleIndex = LoginBase::getSingleton().SelectRoleIndex;
	if (SelectRoleIndex < 0)
	{
		LoginBase::getSingleton().SelectRole(0);
	}
	else if(SelectRoleIndex >= PC->RoleList.Num())
	{
		LoginBase::getSingleton().SelectRole(PC->RoleList.Num() - 1);
	}
	if(nCount < 4)
	{
		for ( int i = nCount ; i < 4 ; ++i )
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("UI_LoginWidget.AS3.RoleInfoData"));
			if( itemObj != NULL )
			{
				itemObj->SetBool(TEXT("HasRole"), false );
				dataProvider->SetElementObject( i, itemObj );
			}
		}
		if(nCount == 0)
		{
			//No character
			LoginBase::getSingleton().SetHiddenInLogin( MainPlayer, true );
		}
	}
	//Update
	eventCallUpdateRoleList(dataProvider);
}


void UMMOUISelectRolePanel::TurnBtnUp()
{
	AvatarTurn = EATF_None;
	RotActor = NULL;
}
void UMMOUISelectRolePanel::SelectTurnRightPress()
{
	AvatarTurn = EATF_TurnRight;
	RotActor = MMOFacade::getSingleton().GetMainActor();

}
void UMMOUISelectRolePanel::SelectTurnLeftPress()
{
	AvatarTurn = EATF_TurnLeft;
	RotActor = MMOFacade::getSingleton().GetMainActor();
}

void UMMOUISelectRolePanel::DeleteRole()
{
	AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* MainPlayer		= MMOFacade::getSingleton().GetMainPlayer();
	if( PC ==NULL || MainPlayer==NULL )
		return;

	INT SelectRoleIndex = LoginBase::getSingleton().SelectRoleIndex;
	if (SelectRoleIndex < 0 || SelectRoleIndex >= PC->RoleList.Num())
		return;
/*
	if (PC->RoleList(SelectRoleIndex).nOverTime > 0)
	{
		CMessageHandler::GetInstance().Send_UnDeleteRole(PC->RoleList(SelectRoleIndex).dwRoleID);
		return;
	}*/
	if(PC->DeletingRoleList.Num() >= 4)
	{
		FScriptDelegate func(EC_EventParm);
		func.Object = NULL;
		func.FunctionName = "";
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.DeleteRoleHintLimited"),0,func,func,FALSE);
		return;
	}
	FScriptDelegate funYesToDelete(EC_EventParm);
	funYesToDelete.Object = this;
	funYesToDelete.FunctionName = TEXT( "CallBackDeleteRole" );

	FScriptDelegate funNull(EC_EventParm);
	funNull.Object = this;

	RandomNums = TEXT("");
	for (int i=0; i < 4; ++i )
	{
		RandomNums += appItoa( appRand()%10 );
	}

	FString str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.AskToDelete"), *RandomNums );

	MMOMsgBoxManager::getSingleton().AddModalInputMsgbox( MMOUI::ParseLocalizedPropertyPath( "MMOGame.MessageBox.DeleteCole"),
		SelectRoleIndex, str, funYesToDelete, funNull, eInputFormatNumOnly );
}

void UMMOUISelectRolePanel::CallBackDeleteRole( UGFxObject* mc )
{
	if( !mc )return;

	FString InputText = mc->GetObject( "textInput" )->GetString("text");

	if( InputText ==  RandomNums )
	{
		INT roleId = mc->GetInt( TEXT("iteminfoid") );
		if( roleId >= 0 )
		{
			CMessageHandler::GetInstance().Send_DeleteRole( roleId );
			return;
		}
	}
	FScriptDelegate funcNULL(EC_EventParm);
	MMOMsgBoxManager::getSingleton().AddMessageBox(_T("Incorrect verification code."), 0, funcNULL, funcNULL);
}


void UMMOUISelectRolePanel::ASCheckFirstOnSelect(UGFxObject *mc)
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


void UMMOUISelectRolePanel::ASRequestReturnLogin()
{
	//call after OnDisconnected

	if(CMessageHandler::GetInstance().IsConnection()){
		CMessageHandler::GetInstance().Send_ReturnLogin();
		CMessageHandler::GetInstance().CloseConnection(true);
	}
	else
	{
		AMMOPlayerController* pc	= MMOFacade::getSingleton().GetMainController();
		if(pc){
			pc->ReturnLogin();
		}
	}
	//MMOUI::GetUIManager()->SendReturnLogin();
}

void UMMOUISelectRolePanel::ASRequestFindRole()
{
	MMOUI::GetUIManager()->ToggleShowPanel(MMOUI::GetUIManager()->Panel_Login_Revert,"");
}

void UMMOUISelectRolePanel::ASRequestJump2CreateRole()
{
	LoginBase::getSingleton().IsCreateOption = FALSE;
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, TRUE, "");
}

void UMMOUISelectRolePanel::setLastSelectRole()
{
	/*if (LoginBase::getSingleton().SelectRoleIndex >0 )
	return;
	else 
	LoginBase::getSingleton().SelectRole(0);*/

	if (LoginBase::getSingleton().SelectRoleIndex != -1)
		return;

	AMMOPlayerController* PC	= MMOFacade::getSingleton().GetMainController();
	if( PC ==NULL )
		return;

	INT nIdx = 0;
	INT saveTime = 0;

	for ( int i = 0 ; i < PC->RoleList.Num() ; ++i )
	{
		const FRoleInfo& roleInfo = PC->RoleList( i );
		if (roleInfo.nSaveTime >= saveTime)
		{
			saveTime = roleInfo.nSaveTime;
			nIdx = i;
		}
	}

	LoginBase::getSingleton().SelectRole(nIdx);
	eventCallUCSetSelectRole(nIdx);
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUISelectRolePanel::EnterScene()
{
}
void UMMOUISelectRolePanel::ChangeScene()
{
}
void UMMOUISelectRolePanel::EnterLogin()
{
}