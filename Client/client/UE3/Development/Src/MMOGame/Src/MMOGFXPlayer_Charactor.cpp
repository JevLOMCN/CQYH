#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "EquipSlotManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Charactor)

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Charactor::PostAdvance ( FLOAT DeltaTime )
{
	UGFxMoviePlayer::PostAdvance(DeltaTime);


// 	if(NeedInitSceneUI)
// 	{
// 		SysModelEquip();
// 		NeedInitSceneUI = false;
// 	}

	FRotator newRot;

	if( TurnButton == TBS_Left && ShowAvatar)
	{
		newRot = ShowAvatar->Rotation;
		newRot.Yaw -= 200;
		ShowAvatar->SetRotation(newRot);
	}
	else	if( TurnButton == TBS_Right && ShowAvatar)
	{
		newRot = ShowAvatar->Rotation;
		newRot.Yaw += 200;
		ShowAvatar->SetRotation(newRot);
	}

}
//-----------------------------------------------------------------------------------------------------------------------

void UMMOGFXPlayer_Charactor::ASTitleTabClick(INT ShowType)
{

}

void UMMOGFXPlayer_Charactor::ASUseCurrentTitle( INT TitleId )
{
	//TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(TitleId);
	//if(pTitle){
	//	FString TitleName = pTitle->TitleName;// MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.TitleName"))  + appItoa(TitleId) );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.EquipTitleMsg")) +  TitleName );
	//}
	CMessageHandler::GetInstance().Send_EquipTitle(TitleId);
}

void UMMOGFXPlayer_Charactor::ASClickTitle( INT TitleId )
{
	Character->ASClickTitle( TitleId );
}

//=====================================================================================================
void UMMOGFXPlayer_Charactor::ASShowTotalEnhancProp()
{
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	if( tipsPanel != NULL && pMainPlayer != NULL ){
		INT curTotalEnhanceLevel = EquipSlotManager::getSingleton().GetCurEnhanceHoleLv();
		tipsPanel->ShowEquipSlotEnhanceTips( pMainPlayer->GetClassId(), curTotalEnhanceLevel );
	}
}

//----------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Charactor::ASHideTotalEnhancProp()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//=====================================================================================================
//=====================================================================================================
//Physique enhancement
void UMMOGFXPlayer_Charactor::ASSelectEquipSlot(INT slotId)
{
	if( Character == NULL || Character->_EnhcPanel == NULL )
	{
		return;
	}

	Character->_EnhcPanel->SelectEnhanceSlot( slotId );
}

//=====================================================================================================
void UMMOGFXPlayer_Charactor::ASEnhanceEquipSlot()
{
	if( Character == NULL || Character->_EnhcPanel == NULL )
	{
		return;
	}

	Character->_EnhcPanel->ToEnhanceCurSelectedSlot();
}

//=====================================================================================================
void UMMOGFXPlayer_Charactor::EnhanceEquipSlotCancel()
{
	EquipSlotManager::getSingleton().ClearFlag(EES_INCONFIRM);
}

//=====================================================================================================
void UMMOGFXPlayer_Charactor::EnhanceEquipSlotOK()
{
	EquipSlotManager::getSingleton().ClearFlag(EES_INCONFIRM);

	//
	if( Character == NULL || Character->_EnhcPanel == NULL )
	{
		return;
	}

	Character->_EnhcPanel->RealEnhanceCurEquipSlot();
}

//=====================================================================================================
void UMMOGFXPlayer_Charactor::ASShowTips_EquipSlot(INT id)
{

}

void UMMOGFXPlayer_Charactor::ASHideTips_EquipSlot(INT id)
{

}

//=====================================================================================================
void UMMOGFXPlayer_Charactor::ASShowTips_ActiveIcon(class UGFxObject* mc)
{
	if( Character == NULL || Character->_EnhcPanel == NULL )
	{
		return;
	}

	Character->_EnhcPanel->ShowActiveTips();
}
//=====================================================================================================
void UMMOGFXPlayer_Charactor::ASShowTips_EnhanceGuide( UGFxObject *mc )
{
	if( Character == NULL || Character->_EnhcPanel == NULL || mc == NULL )
	{
		return;
	}

	FString tipsText = Character->_EnhcPanel->FormatEnhanceGuideTips();
	mc->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

void UMMOGFXPlayer_Charactor::ASHideTips_EnhanceGuide( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//=====================================================================================================
void UMMOGFXPlayer_Charactor::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Charactor::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Charactor);

	//MMOUI::GetUIManager()->SetUpdateReputationDataDelegate(FScriptDelegate(this, TEXT("UpdateReputationData")));
	check(FALSE && "UMMOGFXPlayer_Charactor::Init()");
	//MMOUI::GetUIManager()->SetUpdateTitleDataDelegate(FScriptDelegate(this, TEXT("UpdateTitleData")));
	//MMOUI::GetUIManager()->SetDisplayReputationDataDelegate(FScriptDelegate(this, TEXT("DisplayReputationData")));
}

void UMMOGFXPlayer_Charactor::UnInit()
{
	Super::UnInit();

	//MMOUI::GetUIManager()->SetUpdateReputationDataDelegate(FScriptDelegate(EC_EventParm));
	check(FALSE && "UMMOGFXPlayer_Charactor::Init()");
	//MMOUI::GetUIManager()->SetUpdateTitleDataDelegate(FScriptDelegate(EC_EventParm));
	//MMOUI::GetUIManager()->SetDisplayReputationDataDelegate(FScriptDelegate(EC_EventParm));
}