#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
#include "MMOVipManager.h"

IMPLEMENT_CLASS(UMMOUISpecialExpApply)

//================================================================================================
void UMMOUISpecialExpApply::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		UpdatePanel();
	}
	else{
	}
}
//========================================================================================================================================
void UMMOUISpecialExpApply::PlayerMoneyChange( BYTE MoneyType, INT DeltaMoney )
{
	//if( !IsVisible ){
	//	return;
	//}

	////
	//INT64 curMoney = MMOFacade::getSingleton().GetPlayerMoney64( E_MONEYTYPE_02 );
	//eventCallAsSetCurMoney( appItoa( curMoney ) );
}
//========================================================================================
void UMMOUISpecialExpApply::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_SyncGameLeftTime:
		{
			//UpdateFee();
		}
		break;
	case GameEvent_ApplySpecialExpRst:
		{
			MMOGameEvent_OneInt *curSpecialExp = (MMOGameEvent_OneInt*)evt;

			if( curSpecialExp->IntData > 0 ){
				INT curApplyCnt = eventCallAsGetCurApplyCnt();

				FString strString = TEXT("MMOGame.SpecialExp.ApplyExpSuccess");
				FString hint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( strString ), curApplyCnt, curApplyCnt );
				MMOUI::GetUIManager()->delegateAddScreenMessage( hint, MsgInfoType_Table, strString );

				Show( FALSE );
			}
		}
		break;
	}
}
//===============================================================================================================
void UMMOUISpecialExpApply::UpdatePanel()
{
	UpdateFee();
}
//===============================================================================================================
void UMMOUISpecialExpApply::UpdateFee()
{
	INT32 curMoney = MMOVipManager::getSingleton().GetMyLeftGameTimeInSec();
	FLOAT curMoneyF = (FLOAT)curMoney / 3600;
	eventCallAsSetCurMoney( FString::Printf( TEXT("%.1f"), curMoneyF ) );

	INT curApplyCnt = eventCallAsGetCurApplyCnt();
	FLOAT curCost = curApplyCnt * _CurConvertRate;
	eventCallAsSetCurCost( FString::Printf( TEXT("%.1f"), curCost ) );
}
//===============================================================================================================
//===============================================================================================================
void UMMOUISpecialExpApply::ASApplyCntChanged( INT curInput )
{
	UpdateFee();
}
//===============================================================================================================
void UMMOUISpecialExpApply::ASTryToApply()
{
	INT32 curMoney = MMOVipManager::getSingleton().GetMyLeftGameTimeInSec();

	INT curApplyCnt = eventCallAsGetCurApplyCnt();
	if(curApplyCnt < 1 || curApplyCnt > 9999 )
	{
		FString strString = TEXT("MMOGame.SpecialExp.ErrApplyExpRange");
		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( strString ), MsgInfoType_Table, strString );
		return;
	}

	FLOAT curCost = curApplyCnt * _CurConvertRate;
	INT32 curCostInSec = curCost * 3600;//convert to seconds
	if( curCostInSec > curMoney ){
		//Not enough
		TryRechargeGameTime();
		return;
	}

	CMessageHandler::GetInstance().Send_ApplySpecialExp( curApplyCnt );
}
//===============================================================================================================
//===============================================================================================================
void UMMOUISpecialExpApply::Init()
{
	Super::Init();
	check(NULL == EventAdapter);
	if (NULL == EventAdapter){
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_SyncGameLeftTime);
		evts.AddItem(GameEvent_ApplySpecialExpRst);
		EventAdapter = new TGameEventAdapter<UMMOUISpecialExpApply>(this, evts);
	}
	//
	InitMc();
}
//
void UMMOUISpecialExpApply::InitMc()
{
	eventCallAsSetAnnouncement( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SpecialExp.Announcements") ) );
}
//
void UMMOUISpecialExpApply::TryRechargeGameTime()
{
	CloseAllPopupMsgBox();

	//
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ConfirmRechargeGameTime");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "CancelRechargeGameTime" );

	FString strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SpecialExp.HintRecharge") );

	_ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( strContent, 0, funYes, funNo );
}
//==============================================================================================================
void UMMOUISpecialExpApply::ConfirmRechargeGameTime( UGFxObject* mc )
{
	if( mc == NULL ){
		return;
	}

	//WebHtmlManager::getSingleton().OpenRechargeYuanBao();
	UMMOUIRecharge* rechargePanel = (UMMOUIRecharge*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Recharge );
	if( rechargePanel != NULL ){
		rechargePanel->Show( TRUE );
		return;
	}
	_ConfirmMsgBox = NULL;
}
//==============================================================================================================
void UMMOUISpecialExpApply::CancelRechargeGameTime( UGFxObject* mc )
{
	_ConfirmMsgBox = NULL;
}
//===============================================================================================================
void UMMOUISpecialExpApply::TryToGetCurAllSpecialExp( INT accessableExp, INT totalExp )
{
	CloseAllPopupMsgBox();

	_TotalAppliedSpclExp = totalExp;
	_CurSpecialExpAccessable = accessableExp;

	//
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ConfirmGetAllCurSpecialExp");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "CancelGetAllCurSpecialExp" );

	FString strFormat = TEXT("MMOGame.SpecialExp.CurExp");
	FString strContent = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( strFormat ), _CurSpecialExpAccessable, _TotalAppliedSpclExp - _CurSpecialExpAccessable );
	strContent += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SpecialExp.ConfirmGetAllExp") );

	_ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( strContent, 0, funYes, funNo );
	//if( _ConfirmMsgBox != NULL ){
	//	_ConfirmMsgBox->SetInt( TEXT("itemdataid"), ss );
	//}
}
//==============================================================================================================
void UMMOUISpecialExpApply::ConfirmGetAllCurSpecialExp( UGFxObject* mc )
{
	if( mc == NULL ){
		return;
	}

	CMessageHandler::GetInstance().Send_GetCurAllSpecialExp();

	_ConfirmMsgBox = NULL;
}
//==============================================================================================================
void UMMOUISpecialExpApply::CancelGetAllCurSpecialExp( UGFxObject* mc )
{
	_ConfirmMsgBox = NULL;
}
//==============================================================================================================
void UMMOUISpecialExpApply::CloseAllPopupMsgBox()
{
	if( _ConfirmMsgBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmMsgBox );
		_ConfirmMsgBox = NULL;
	}
}
//==============================================================================================================