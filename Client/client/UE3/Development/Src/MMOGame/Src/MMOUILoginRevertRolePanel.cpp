#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
#include "LoginBase.h"
#include "MMOMsgBoxManager.h"
#include "MMOGameEvent_Splash.h"

IMPLEMENT_CLASS(UMMOUILoginRevertRolePanel);


void UMMOUILoginRevertRolePanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Splash);
	EventAdapter = new TGameEventAdapter<UMMOUILoginRevertRolePanel>(this, evts);
}

void UMMOUILoginRevertRolePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
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
					UpdateRevertPanelList();
				}
				break;
			}
		}
	}
}

void UMMOUILoginRevertRolePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	
	if(v)
		UpdateRevertPanelList();
}

static INT nRevertRoleID = 0;
void UMMOUILoginRevertRolePanel::funReverCancel()
{
	nRevertRoleID = 0;
}
void UMMOUILoginRevertRolePanel::funReverOk()
{
	if(nRevertRoleID > 0)
	{
		CMessageHandler::GetInstance().Send_UnDeleteRole(nRevertRoleID);
		Show(FALSE);
	}
}
void UMMOUILoginRevertRolePanel::funDeleteCancel()
{
	nRevertRoleID = 0;
}
void UMMOUILoginRevertRolePanel::funDeleteOk()
{
	if(nRevertRoleID > 0)
	{
		CMessageHandler::GetInstance().Send_DeleteRoleForEver(nRevertRoleID);
		//Show(FALSE);
	}
}
void UMMOUILoginRevertRolePanel::DeleteRoleForever(INT nRoleID)
{
	AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();
	if( pl == NULL)
		return;
	BOOL bValid = FALSE;
	for (INT n = 0; n < pl->DeletingRoleList.Num(); ++n)
	{
		const FRoleInfo& roleInfo = pl->DeletingRoleList(n);
		if(roleInfo.dwRoleID == nRoleID)
		{
			bValid = TRUE;
			break;
		}
	}
	if(!bValid)
	{
		FScriptDelegate func(EC_EventParm);
		func.Object = NULL;
		func.FunctionName = "";
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.SelectedDeleteRoleHint"),0,func,func,FALSE);
		return;
	}

	nRevertRoleID = nRoleID;

	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = TEXT("funDeleteOk");
	funOk.Object = this;
	FScriptDelegate funCancel(EC_EventParm);
	funCancel.FunctionName = TEXT("funDeleteCancel");
	funCancel.Object = this;
	MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.ConfirmDeleteForeverRoleHint"),0,funOk,funCancel);
}
void UMMOUILoginRevertRolePanel::UnddeleteRole(INT nRoleID)
{
	AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();
	if( pl == NULL)
		return;
	BOOL bValid = FALSE;
	for (INT n = 0; n < pl->DeletingRoleList.Num(); ++n)
	{
		const FRoleInfo& roleInfo = pl->DeletingRoleList(n);
		if(roleInfo.dwRoleID == nRoleID)
		{
			bValid = TRUE;
			break;
		}
	}
	if(!bValid)
	{
		FScriptDelegate func(EC_EventParm);
		func.Object = NULL;
		func.FunctionName = "";
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.SelectedRevertRoleHint"),0,func,func,FALSE);
		return;
	}
	if(pl->RoleList.Num() >= 4)
	{
		FScriptDelegate func(EC_EventParm);
		func.Object = NULL;
		func.FunctionName = "";
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.UnDeleteRoleHintLimited"),0,func,func,FALSE);
		return;
	}
	nRevertRoleID = nRoleID;
	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = TEXT("funReverOk");
	funOk.Object = this;
	FScriptDelegate funCancel(EC_EventParm);
	funCancel.FunctionName = TEXT("funReverCancel");
	funCancel.Object = this;
	MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath( "MMOGame.Login.ConfirmRevertRoleHint"),0,funOk,funCancel);
}

void UMMOUILoginRevertRolePanel::UpdateRevertPanelList()
{
	AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();
	if( pl == NULL)
		return;

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT nCount = 0;
	for (INT n = 0; n < pl->DeletingRoleList.Num(); ++n)
	{
		UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("UI_LoginWidget.AS3.RoleInfoData"));
		if( itemObj != NULL )
		{
			const FRoleInfo& roleInfo = pl->DeletingRoleList(n);
			if(roleInfo.nOverTime != 0)
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
				// character ID
				itemObj->SetInt(_T("RoleID"), roleInfo.dwRoleID);
			}

			dataProvider->SetElementObject( nCount, itemObj );
			nCount++;
		}
	}
	if(nCount < 4)
	{
		for (INT n = nCount; n < 4; ++n)
		{
			UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("UI_LoginWidget.AS3.RoleInfoData"));
			if( itemObj != NULL )
			{
				itemObj->SetBool(TEXT("HasRole"), false );
				dataProvider->SetElementObject( n, itemObj );
			}
		}
	}
	eventCallUCUpdateDelRoleList(dataProvider);
}


void UMMOUILoginRevertRolePanel::ASRequestFindRoleID(INT nRoleID)
{
	UnddeleteRole(nRoleID);
}

void UMMOUILoginRevertRolePanel::ASRequestDelectRole(INT nRoleID)
{
	DeleteRoleForever(nRoleID);
}