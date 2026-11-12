#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "EquipSlotManager.h"

IMPLEMENT_CLASS(UMMOUIDurabilityShowing)

//
void UMMOUIDurabilityShowing::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	//check(NULL != evt);
	if (NULL != evt)
	{
		switch (evt->GetEventType())
		{
		case GameEVent_SceneLoadFinish:
			DescideShow();
			break;
		case GameEvent_EnterSelectingRole:
			HasLowDurabilityEquip = 0;
			break;;
		}
	}
}
//
void UMMOUIDurabilityShowing::DescideShow()
{
	
	// Do not check hints during cutscenes
	AMMOPlayerController* pCtrl = MMOFacade::getSingleton().GetMainController();
	if(pCtrl && pCtrl->IsPlayingMatinee()==FALSE)
	{
		UBOOL isToShow = ( HasLowDurabilityEquip != 0 );
		if(IsVisible != isToShow)
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DurabilityShowing, isToShow, "");
		}
	}

	////
	//if(MMO::GetCurrentGameState() != EGameState_InGame){
	//	return;
	//}

	////
	//if( !IsVisible && isToShow ){
	//	Show( TRUE );
	//}
	//else if( IsVisible && !isToShow ){
	//	Show( FALSE );
	//}
}

//========================================================================
void UMMOUIDurabilityShowing::PostOnShow(UBOOL v)
{
	Super::PreOnShow( v );

	UpdatePosition();

	//if( v ){
	//	UMMOUIBase* rightPanel = MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	//	if( rightPanel != NULL ){
	//		//INT leftW = eventGetWidth();
	//		//INT leftH = leftPanel->eventGetHeight();

	//		FLOAT rightPanelX;
	//		FLOAT rightPanelY;
	//		rightPanel->GetPosition( rightPanelX, rightPanelY );

	//		SetPosition( rightPanelX - PanelWidth, rightPanelY + 45 );
	//	}

	//	UpdateAllEquipDurState();
	//}

}

void UMMOUIDurabilityShowing::UpdatePosition()
{
	if(IsVisible)
	{
		UMMOUIBase* rightPanel = MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
		if( rightPanel != NULL )
		{
			FLOAT rightPanelX;
			FLOAT rightPanelY;
			rightPanel->GetPosition( rightPanelX, rightPanelY );
			SetPosition( rightPanelX - PanelWidth, rightPanelY + 45 );
		}
		UpdateAllEquipDurState();
	}	
}

//========================================================================
void UMMOUIDurabilityShowing::UpdateAllEquipDurState()
{
	for( INT i = E_NES_MAIN_HAND; i < E_NES_TOTAL; ++i ){
		//if( i == E_NES_AVATAR ){
		//	//none for now
		//	continue;
		//}

		//Ring
		if( i == E_NES_FINGER_L || i == E_NES_FINGER_R ){
			if( i == E_NES_FINGER_R ){
				continue;
			}
			SetEquipDurabilityStateFromTwo( EEquipDurComp_FINGER );

			continue;
		}

		//Bracelet
		if( i == E_NES_WRIST_L || i == E_NES_WRIST_R ){
			if( i == E_NES_WRIST_R ){
				continue;
			}
			SetEquipDurabilityStateFromTwo( EEquipDurComp_WRIST );

			continue;
		}

		//other
		FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED , i );
		if( pItemInfo != NULL ){
			UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
			SetEquipDurabilityState( (ENORMAL_EQUIP_SLOT)i, pItemInfo, pItem );
		}
		else{
			SetEquipDurabilityState( (ENORMAL_EQUIP_SLOT)i, NULL, NULL );
		}
	}
}

//========================================================================
void UMMOUIDurabilityShowing::UpdateEquipDurState( ENORMAL_EQUIP_SLOT componentID )
{
	switch( componentID )
	{
	case E_NES_MAIN_HAND:
	case E_NES_HEAD:
	case E_NES_SHOULDER:
	case E_NES_BODY:
	//case E_NES_LEG:
	case E_NES_ARM:
	case E_NES_WAIST:
	case E_NES_FOOT:
	case E_NES_BACK:
	//case E_NES_AVATAR:
	case E_NES_NECK:
		{
			FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, componentID );
			if( pItemInfo != NULL ){
				UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
				SetEquipDurabilityState( componentID, pItemInfo, pItem );
			}
			else{
				SetEquipDurabilityState( componentID, NULL, NULL );
			}
		}
		break;
	case E_NES_FINGER_L:
	case E_NES_FINGER_R:
		{
			SetEquipDurabilityStateFromTwo( EEquipDurComp_FINGER );
		}
	break;
	case E_NES_WRIST_L:
	case E_NES_WRIST_R:
		{
			SetEquipDurabilityStateFromTwo( EEquipDurComp_WRIST );
		}
	break;
	}
}
//========================================================================
void UMMOUIDurabilityShowing::SetEquipDurabilityStateFromTwo( EEquipDurComp durComponentID )
{
	ENORMAL_EQUIP_SLOT realLeftEquippedSlotID;
	ENORMAL_EQUIP_SLOT realRightEquippedSlotID;
	if( durComponentID == EEquipDurComp_FINGER ){
		realLeftEquippedSlotID = E_NES_FINGER_L;
		realRightEquippedSlotID = E_NES_FINGER_R;
	}
	else if( durComponentID == EEquipDurComp_WRIST ){
		realLeftEquippedSlotID = E_NES_WRIST_L;
		realRightEquippedSlotID = E_NES_WRIST_R;
	}
	else{
		return;
	}

	//Choose the lower of left and right
	FClientItemInfo* pLeftItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, realLeftEquippedSlotID );
	FClientItemInfo* pRightItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, realRightEquippedSlotID );
	if( pLeftItemInfo != NULL && pRightItemInfo != NULL ){
		FLOAT leftDurPercent = (FLOAT)pLeftItemInfo->m_nCurDur / pLeftItemInfo->m_nMaxDur;
		FLOAT rightDurPercent = (FLOAT)pRightItemInfo->m_nCurDur / pRightItemInfo->m_nMaxDur;
		if( leftDurPercent < rightDurPercent ){
			UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pLeftItemInfo->m_nTemplateID );
			SetEquipDurabilityState( realLeftEquippedSlotID, pLeftItemInfo, pItem );
		}
		else{
			UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pRightItemInfo->m_nTemplateID );
			SetEquipDurabilityState( realLeftEquippedSlotID, pRightItemInfo, pItem );
		}
	}
	else if( pLeftItemInfo == NULL && pRightItemInfo != NULL ){
		UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pRightItemInfo->m_nTemplateID );
		SetEquipDurabilityState( realLeftEquippedSlotID, pRightItemInfo, pItem );
	}
	else if( pLeftItemInfo != NULL && pRightItemInfo == NULL ){
		UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( pLeftItemInfo->m_nTemplateID );
		SetEquipDurabilityState( realLeftEquippedSlotID, pLeftItemInfo, pItem );
	}
	else{
		SetEquipDurabilityState( realLeftEquippedSlotID, NULL, NULL );
	}
}
//---------------------------------------------------------------------------------------------------------
void UMMOUIDurabilityShowing::SetEquipDurabilityState( ENORMAL_EQUIP_SLOT componentID, ItemInfo *pEquipItemInfo, UItem *pItem )
{
	EEquipState curDurState = EEquipDurSta_Normal;
	if( pItem != NULL && pEquipItemInfo != NULL ){
		INT cvtCurDurability = ( pEquipItemInfo->m_nCurDur + EQUIP_DUR_MULTIPLIER -1 ) / EQUIP_DUR_MULTIPLIER;
		INT cvtCurMaxDurability = ( pEquipItemInfo->m_nMaxDur + EQUIP_DUR_MULTIPLIER -1 ) / EQUIP_DUR_MULTIPLIER;
		INT cvtMaxDurability = pItem->DefaultDuration;
		//
		if( cvtCurDurability <= (INT)(0.2 * cvtCurMaxDurability) ){
			curDurState = EEquipDurSta_Yellow;
		}
		////
		//if( cvtMaxDurability > 0 && ( cvtCurMaxDurability <= (INT)(0.2 * cvtMaxDurability) ) ){
		//	curDurState = EEquipDurSta_Red;
		//}

		//if( cvtCurMaxDurability != 0 ){
		//	if( cvtCurDurability == 0 ){
		//		curDurState = EEquipDurSta_Red;
		//	}
		//	else if( cvtCurDurability <= (INT)(0.2 * cvtCurMaxDurability) ){
		//		curDurState = EEquipDurSta_Yellow;
		//	}
		//}
	}

	//
	EEquipDurComp equipDurCompID = CvtEquipSlot2DurabilityComponent( componentID );
	INT oldDurabilityState = HasLowDurabilityEquip;
	if( curDurState != EEquipDurSta_Normal ){
		HasLowDurabilityEquip |= (1 << equipDurCompID);
	}
	else{
		HasLowDurabilityEquip &= ~(1 << equipDurCompID);
	}

	if( oldDurabilityState != HasLowDurabilityEquip ){
		eventUCCallSetComponentState( equipDurCompID, curDurState );
	}
	
}
//
EEquipDurComp UMMOUIDurabilityShowing::CvtEquipSlot2DurabilityComponent( ENORMAL_EQUIP_SLOT equipSlotID )
{
	EEquipDurComp durCompID;
	switch( equipSlotID )
	{
	case E_NES_MAIN_HAND: durCompID = EEquipDurComp_MAIN_HAND; break;
	case E_NES_HEAD: durCompID = EEquipDurComp_HEAD; break;
	case E_NES_SHOULDER: durCompID = EEquipDurComp_SHOULDER; break;
	case E_NES_BODY: durCompID = EEquipDurComp_BODY; break;
	//case E_NES_LEG: durCompID = EEquipDurComp_LEG; break;
	case E_NES_ARM: durCompID = EEquipDurComp_ARM; break;
	case E_NES_WAIST: durCompID = EEquipDurComp_WAIST; break;
	case E_NES_FOOT: durCompID = EEquipDurComp_FOOT; break;
	case E_NES_BACK: durCompID = EEquipDurComp_BACK; break;
		//case E_NES_AVATAR:
	case E_NES_NECK: durCompID = EEquipDurComp_NECK; break;
	case E_NES_FINGER_L: durCompID = EEquipDurComp_FINGER; break;
	case E_NES_FINGER_R: durCompID = EEquipDurComp_FINGER; break;
	case E_NES_WRIST_L: durCompID = EEquipDurComp_WRIST; break;
	case E_NES_WRIST_R: durCompID = EEquipDurComp_WRIST; break;
	default:
		durCompID = EEquipDurComp_MAX;
		break;
	}

	return durCompID;
}
//========================================================================
void UMMOUIDurabilityShowing::Init()
{
	Super::Init();

	InitMc();

	//check(NULL == EventAdapter);
	if (NULL == EventAdapter)
	{
		TArray<EGameEvent> evts;
		evts.AddItem(GameEVent_SceneLoadFinish);
		evts.AddItem(GameEvent_EnterSelectingRole);
		EventAdapter = new TGameEventAdapter<UMMOUIDurabilityShowing>(this, evts);
	}
}

void UMMOUIDurabilityShowing::InitMc()
{
	Component_MAIN_HAND = GetObject( TEXT("DurabilityComponentMc0") );
	Component_HEAD = GetObject( TEXT("DurabilityComponentMc1") );
	Component_SHOULDER = GetObject( TEXT("DurabilityComponentMc2") );
	Component_BODY = GetObject( TEXT("DurabilityComponentMc3") );
	Component_LEG = GetObject( TEXT("DurabilityComponentMc4") );
	Component_ARM = GetObject( TEXT("DurabilityComponentMc5") );
	Component_WAIST = GetObject( TEXT("DurabilityComponentMc6") );
	Component_FOOT = GetObject( TEXT("DurabilityComponentMc7") );
	Component_BACK = GetObject( TEXT("DurabilityComponentMc8") );
	Component_NECK = GetObject( TEXT("DurabilityComponentMc10") );
	Component_FINGER = GetObject( TEXT("DurabilityComponentMc11") );
	Component_WRIST = GetObject( TEXT("DurabilityComponentMc12") );
}

void UMMOUIDurabilityShowing::UnInit()
{
}

//===============================================================================================================
//===============================================================================================================
UBOOL UMMOUIDurabilityShowing::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(	strHoverControllerName==TEXT("DurabilityShowing")||
			strHoverControllerName==TEXT("DurabilityComponentMc0")||
			strHoverControllerName==TEXT("DurabilityComponentMc1")||
			strHoverControllerName==TEXT("DurabilityComponentMc2")||
			strHoverControllerName==TEXT("DurabilityComponentMc3")||
			strHoverControllerName==TEXT("DurabilityComponentMc4")||
			strHoverControllerName==TEXT("DurabilityComponentMc5")||
			strHoverControllerName==TEXT("DurabilityComponentMc6")||
			strHoverControllerName==TEXT("DurabilityComponentMc7")||
			strHoverControllerName==TEXT("DurabilityComponentMc8")||
			strHoverControllerName==TEXT("DurabilityComponentMc9")||
			strHoverControllerName==TEXT("DurabilityComponentMc10")||
			strHoverControllerName==TEXT("DurabilityComponentMc11")||
			strHoverControllerName==TEXT("DurabilityComponentMc12")	)
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

void UMMOUIDurabilityShowing::ASHintDurabilityDetail( UGFxObject *mc )
{
	UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	if( tipsPanel != NULL ){
		INT curTotalEnhanceLevel = EquipSlotManager::getSingleton().GetCurEnhanceHoleLv();
		tipsPanel->ShowTipsHTML( TFT_EquipDurability, mc );
	}
}
//===============================================================================================================
void UMMOUIDurabilityShowing::ASHideDurabilityDetail()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===============================================================================================================
//===============================================================================================================