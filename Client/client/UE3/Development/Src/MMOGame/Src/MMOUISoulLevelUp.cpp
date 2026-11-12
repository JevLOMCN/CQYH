#include "MMOFacade.h"
#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "net/message_handler.h"
#include "ClientActionConditionCheck.h"
#include "MMOEquipManager.h"
#include "item_def.h"
#include "UITextParser.h"
#include "UILayout.h"

IMPLEMENT_CLASS(UMMOUISoulLevelUp)

void UMMOUISoulLevelUp::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
	}
	else{
		ResetUI();
	}
}

//========================================================================================
void UMMOUISoulLevelUp::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_SoulLevelUp:
		{
			MMOGameEvent_OneInt *soulLvUpEvt = (MMOGameEvent_OneInt*)evt;

			if( IsVisible ){
				OnNotifySoulLevelUpResult( soulLvUpEvt->IntData );
			}
		}
		break;
	}
}

//========================================================================================================================================
void UMMOUISoulLevelUp::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	//
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );

	if( _ToLvUpSoul._ConID == bagId && _ToLvUpSoul._SlotID == slotId ){
		//Soul to upgrade
		if( pItemInfo == NULL ){
			UnSetLevelUpSoul();
		}
		else{
			SetLevelUpSoul( bagId, slotId );
		}
	}
	else if( _HelpLvUpSoulItem._ConID == bagId && _HelpLvUpSoulItem._SlotID == slotId ){
		//Item that raises upgrade success rate
	}
	else{
		//Soul absorbed for upgrade
		for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
			if( _SacrificeSoul[i]._ConID == bagId && _SacrificeSoul[i]._SlotID == slotId ){
				if( pItemInfo == NULL ){
					UnSetLevelUpSoulSacrifice( i );
				}
				else{
					INT unSetCnt = AutoUnSetPackedSoulScrifice( bagId, slotId );
					if( unSetCnt > 0 ){
						//The placed spirit stack count is insufficient
						MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_NOT_ENOUGH_SOUL") ),MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_NOT_ENOUGH_SOUL") );
					}
					break;
				}
			}
		}
	}//end of if else

}

//========================================================================================================================================
void UMMOUISoulLevelUp::ClearUI()
{

}

//========================================================================================================================================
void UMMOUISoulLevelUp::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{

}
//========================================================================================================================================
//
UBOOL UMMOUISoulLevelUp::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUISoulLevelUp::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUISoulLevelUp::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	if( SlotType == ESPType_Bag ){
		//put in from bag
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( ContainerID, SlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		//
		ClientActionCondition_SoulLevelUp checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		AutoSetLevelUpSoulSacrifice( ContainerID, SlotID );
		
		return TRUE;
	}
	else{
		INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") ); 
		//put back to bag
		if( SlotType == ESPType_SoulLevelUp ){
			switch( panelSlotID )
			{
			case ESOLPST_LvUpSoul:
				UnSetLevelUpSoul();
				break;
			case ESOLPST_LvUpScrificSoul1:
				UnSetLevelUpSoulSacrifice( 0 );
				break;
			case ESOLPST_LvUpScrificSoul2:
				UnSetLevelUpSoulSacrifice( 1 );
				break;
			case ESOLPST_LvUpScrificSoul3:
				UnSetLevelUpSoulSacrifice( 2 );
				break;
			case ESOLPST_LvUpScrificSoul4:
				UnSetLevelUpSoulSacrifice( 3 );
				break;
			case ESOLPST_LvUpScrificSoul5:
				UnSetLevelUpSoulSacrifice( 4 );
				break;
			case ESOLPST_LvUpHelpItem:
				UnSetLevelUpSoulSuccessHelp();
				break;
			}

			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

//
UBOOL UMMOUISoulLevelUp::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}

	//from bag to spirit fusion
	if( srcSlotType == ESPType_Bag ){
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( srcContainerID, srcSlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		//
		ClientActionCondition_SoulLevelUp checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		INT panelSlotID = (INT)destSlotMC->GetFloat( TEXT("EquipID") );
		if( destSlotType == ESPType_SoulLevelUp ){
			switch( panelSlotID )
			{
			case ESOLPST_LvUpSoul:
				SetLevelUpSoul( srcContainerID, srcSlotID );
				break;
			case ESOLPST_LvUpScrificSoul1:
				SetLevelUpSoulSacrifice( srcContainerID, srcSlotID, 0 );
				break;
			case ESOLPST_LvUpScrificSoul2:
				SetLevelUpSoulSacrifice( srcContainerID, srcSlotID, 1 );
				break;
			case ESOLPST_LvUpScrificSoul3:
				SetLevelUpSoulSacrifice( srcContainerID, srcSlotID, 2 );
				break;
			case ESOLPST_LvUpScrificSoul4:
				SetLevelUpSoulSacrifice( srcContainerID, srcSlotID, 3 );
				break;
			case ESOLPST_LvUpScrificSoul5:
				SetLevelUpSoulSacrifice( srcContainerID, srcSlotID, 4 );
				break;
			case ESOLPST_LvUpHelpItem:
				SetLevelUpSoulSuccessHelp( srcContainerID, srcSlotID );
				break;
			}
		}

		return TRUE;
	}
	else if( destSlotType == ESPType_Bag ){
		//drag back to bag; since nothing was actually moved, dropping onto the original position is not supported
		INT panelSlotID = (INT)srcSlotMC->GetFloat( TEXT("EquipID") );
		if( srcSlotType == ESPType_SoulLevelUp ){
			switch( panelSlotID )
			{
			case ESOLPST_LvUpSoul:
				UnSetLevelUpSoul();
				break;
			case ESOLPST_LvUpScrificSoul1:
				UnSetLevelUpSoulSacrifice( 0 );
				break;
			case ESOLPST_LvUpScrificSoul2:
				UnSetLevelUpSoulSacrifice( 1 );
				break;
			case ESOLPST_LvUpScrificSoul3:
				UnSetLevelUpSoulSacrifice( 2 );
				break;
			case ESOLPST_LvUpScrificSoul4:
				UnSetLevelUpSoulSacrifice( 3 );
				break;
			case ESOLPST_LvUpScrificSoul5:
				UnSetLevelUpSoulSacrifice( 4 );
				break;
			case ESOLPST_LvUpHelpItem:
				UnSetLevelUpSoulSuccessHelp();
				break;
			}
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================================================================
void UMMOUISoulLevelUp::ResetUI()
{
	ResetSoulLevelUp();
}
//=============================================================================================================================
//soul level up
void UMMOUISoulLevelUp::UpdateSoulLevelUp()
{
	//Spirit to upgrade
	SetLevelUpSoul( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID );
	//Absorbed spirit
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID >= 0 ){
			FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _SacrificeSoul[i]._ConID, _SacrificeSoul[i]._SlotID );
			if( pItemInfo == NULL ){
				UnSetLevelUpSoulSacrifice( i );
			}
			else{
				SetLevelUpSoulSacrifice( _SacrificeSoul[i]._ConID, _SacrificeSoul[i]._SlotID, i );
			}
		}
		else{
			UnSetLevelUpSoulSacrifice( i );
		}
	}
}

//=============================================================================================================================
void UMMOUISoulLevelUp::UpdateSoulInfo()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	if( _NextLevelSoulPropTf == NULL || _CurLevelSoulPropTf == NULL ){
		return;
	}
	_NextLevelSoulPropTf->SetString( TEXT("htmlText"), TEXT("") );
	_CurLevelSoulPropTf->SetString( TEXT("htmlText"), TEXT("") );

	FClientItemInfo *pInfo = mmoFacade.FindItem( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID );
	if( pInfo == NULL ){
		//warnf(NAME_Error, TEXT("======!!!!======= [UpdateSoulInfo soul FClientItemInfo Not FOUND]") );
		return;
	}

	const SoulInfo* pSoulInfo = equipMgr.FindSoulInfoByItemTemplateID( pInfo->m_nTemplateID );
	if( pSoulInfo == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UpdateSoulInfo soul SoulInfo Not FOUND]") );
		return;
	}

	////
	//UEnum* enumClass = equipMgr.GetAttrEnumClass();
	//if( enumClass == NULL ){
	//	warnf(NAME_Error, TEXT("======!!!!======= [UpdateSoulInfo enumClass Not FOUND]") );
	//	return;
	//}

	//current soul property
	const ItemRandomAttr* propAttr = mmoFacade.GetRandomAttributById( pSoulInfo->_AttrID );
	if( propAttr == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UpdateSoulInfo ItemAttr Not FOUND]") );
		return;
	}
	_CurLevelSoulPropTf->SetString( TEXT("htmlText"), propAttr->strText );

	//next level prop
	const SoulInfo* pSoulInfoNextLv = equipMgr.FindSoulInfoByItemTemplateID( pSoulInfo->_UpgradeSoulID );
	if( pSoulInfoNextLv == NULL ){
		//highest level
		return;
	}

	propAttr = mmoFacade.GetRandomAttributById( pSoulInfoNextLv->_AttrID );
	if( propAttr == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UpdateSoulInfo ItemAttr Not FOUND]") );
		return;
	}
	_NextLevelSoulPropTf->SetString( TEXT("htmlText"), propAttr->strText );
}

//=============================================================================================================================
INT UMMOUISoulLevelUp::UpdateSuccessRatio()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	//get soul information
	FClientItemInfo *pLvUpSoulItemInfo = mmoFacade.FindItem( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID );
	if( pLvUpSoulItemInfo == NULL ){
		//warnf(NAME_Error, TEXT("======!!!!======= [UpdateSuccessRatio FClientItemInfo Not FOUND]") );
		_SuccessRitoTf->SetText( TEXT("") );
		return 0;
	}

	const SoulInfo* pLvUpSoulInfo = equipMgr.FindSoulInfoByItemTemplateID( pLvUpSoulItemInfo->m_nTemplateID );
	if( pLvUpSoulInfo == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UpdateSuccessRatio SoulInfo Not FOUND]") );
		return 0;
	}

	//get next level soul information
	const SoulInfo* pNextLvSoulInfo = equipMgr.FindSoulInfoByItemTemplateID( pLvUpSoulInfo->_UpgradeSoulID );
	if( pNextLvSoulInfo == NULL ){
		//Consider it max level
		_SuccessRitoTf->SetText( TEXT("") );
		return 0;
	}

	INT curValue = 0;
	//Absorbed soul
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID < 0 || _SacrificeSoul[i]._SlotID < 0 ){
			continue;
		}

		//
		FClientItemInfo *pInfo = mmoFacade.FindItem( _SacrificeSoul[i]._ConID, _SacrificeSoul[i]._SlotID );
		if( pInfo == NULL ){
			warnf(NAME_Error, TEXT("======!!!!======= [UpdateSuccessRatio FClientItemInfo Not FOUND]") );
			continue;
		}

		const SoulInfo* pSoulInfo = equipMgr.FindSoulInfoByItemTemplateID( pInfo->m_nTemplateID );
		if( pSoulInfo == NULL ){
			warnf(NAME_Error, TEXT("======!!!!======= [UpdateSuccessRatio SoulInfo Not FOUND]") );
			continue;
		}

		//
		curValue += pSoulInfo->_PriceValue;
	}

	//
	INT totalValue = pNextLvSoulInfo->_PriceValue;
	if( totalValue == 0 ){
		warnf(NAME_Error, TEXT("======!!!!======= [SoulInfo data%d ERROR in PriceValue]"), pNextLvSoulInfo->_ItemID );
		totalValue = 1;
	}
	INT rate = curValue * 100 / totalValue;
	if( rate > 100 ){
		rate = 100;
	}

	//Success rate bonus from success-rate-boosting item
	_HelpLvUpSoulItem;

	//
	FString ritoText = appItoa( rate );
	ritoText += TEXT("%");
	_SuccessRitoTf->SetText( ritoText );

	return rate;
}

//=============================================================================================================================
UBOOL UMMOUISoulLevelUp::UnSetLevelUpSoul()
{
	return SetLevelUpSoul( -1, -1 );
}

//=============================================================================================================================
UBOOL UMMOUISoulLevelUp::SetLevelUpSoul( INT containerID, INT slotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	//Set soul to upgrade
	FClientItemInfo *pSoulItemInfo = NULL;
	UItem *pSoulItem = NULL;
	const SoulInfo* pSoulInfo = NULL;
	if( containerID >= 0 && slotID >= 0 ){
		pSoulItemInfo = mmoFacade.FindItem( containerID, slotID );
		if( pSoulItemInfo == NULL ){
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_ITEM_NOT_FOUND") ),MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_ITEM_NOT_FOUND") );
			return FALSE;
		}

		//
		pSoulItem = mmoFacade.FindItemTemplate( pSoulItemInfo->m_nTemplateID );
		if( pSoulItem == NULL ){
			warnf(NAME_Error, TEXT("======!!!!======= [SetLevelUpSoul soul Template Not FOUND]") );
			return FALSE;
		}

		//
		pSoulInfo = equipMgr.FindSoulInfoByItemTemplateID( pSoulItemInfo->m_nTemplateID );
		if( pSoulInfo == NULL ){
			//The item placed is not a spirit item
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_NOT_A_SOUL") ) ,MsgInfoType_Table, TEXT("MMOGame.Soul.ERROR_NOT_A_SOUL"));
			return FALSE;
		}

		//
		UBOOL isAlreadyInUse = pSoulItemInfo->IsDragIntoOtherUI > -1;
		if( isAlreadyInUse && ( _ToLvUpSoul._ConID != containerID && _ToLvUpSoul._SlotID != slotID ) ){
			//Being used (probably in the spirit absorption slot or in a trade)
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_ITEM_IN_USE") ) ,MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_ITEM_IN_USE") );
			return FALSE;
		}
	}


	//Unlock the soul previously placed in the bag
	//UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	if( _ToLvUpSoul._ConID >= 0 && _ToLvUpSoul._SlotID >= 0 ){
		//if( pBagPanel != NULL && _ToLvUpSoul._ConID == E_CONSLOT_DEFAULT ){
			MMOFacade::getSingleton().LockItem( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID, -1 );
		//}
	}

	//Clear, remove soul
	if( ( pSoulItemInfo == NULL ) && ( _ToLvUpSoul._ConID >= 0 && _ToLvUpSoul._SlotID >= 0 ) ){
		//if( pBagPanel != NULL && _ToLvUpSoul._ConID == E_CONSLOT_DEFAULT ){
			MMOFacade::getSingleton().LockItem( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID, -1 );
		//}

		//GFXPlayer->SetItemSlotData( _ToLvUpSoulMc, NULL, ESPType_SoulLevelUp, -1, -1 );
		GFXPlayer->eventCallSetItemData( _ToLvUpSoulMc , 0 , TEXT("") , -1, ESPType_SoulLevelUp, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );

		//
		_ToLvUpSoul._ConID = -1;
		_ToLvUpSoul._SlotID = -1;

		//Refresh info
		UpdateSoulInfo();

		//Refresh success rate
		UpdateSuccessRatio();

		return TRUE;
	}

	//
	_ToLvUpSoul._ConID = containerID;
	_ToLvUpSoul._SlotID = slotID;

	//set UI ItemSlot
	//GFXPlayer->SetItemSlotData( _ToLvUpSoulMc, pSoulItem, ESPType_SoulLevelUp, _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID );
	if( pSoulItem != NULL ){
		GFXPlayer->eventCallSetItemData( _ToLvUpSoulMc, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pSoulItem ), pSoulItem->ItemId, ESPType_SoulLevelUp, TEXT(""), pSoulItem->ItemColor+1, _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID );
	}
	else{
		GFXPlayer->eventCallSetItemData( _ToLvUpSoulMc , 0 , TEXT("") , -1, ESPType_SoulLevelUp, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
	}
	
	//Lock the soul previously placed in the bag
	//if( pBagPanel != NULL && _ToLvUpSoul._ConID == E_CONSLOT_DEFAULT ){
		MMOFacade::getSingleton().LockItem( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID, ESPType_SoulLevelUp );
	//}

	//Refresh info
	UpdateSoulInfo();

	//Refresh success rate
	UpdateSuccessRatio();

	return TRUE;
}

//=============================================================================================================================
UBOOL UMMOUISoulLevelUp::UnSetLevelUpSoulSacrifice( INT targetSlotID )
{
	return SetLevelUpSoulSacrifice( -1, -1, targetSlotID );
}

//=============================================================================================================================
INT UMMOUISoulLevelUp::AutoSetLevelUpSoulSacrifice( int containerID, int slotID )
{
	INT idx = 0;
	for( ; idx < MAX_SacrificeSoul; ++idx ){
		if( _SacrificeSoul[idx]._ConID < 0 ){
			break;
		}
	}

	//
	if( idx < MAX_SacrificeSoul ){
		UBOOL isSuccess = SetLevelUpSoulSacrifice( containerID, slotID, idx );
		if( isSuccess != TRUE ){
			idx = -1;
		}
	}
	else{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_SOUL_SACRIFICE_FULL") ) ,MsgInfoType_Table, TEXT("MMOGame.Soul.ERROR_SOUL_SACRIFICE_FULL"));
		idx = -1;
	}

	return idx;
}

//=============================================================================================================================
UBOOL UMMOUISoulLevelUp::SetLevelUpSoulSacrifice( INT containerID, INT slotID, INT targetSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	//
	if( targetSlotID < 0 || targetSlotID >= MAX_SacrificeSoul ){
		return FALSE;
	}
	UGFxObject *pSlotMc = _SacrificeSoulMc[targetSlotID];
	FOperationItemBagPos &toSacrifySoulPos = _SacrificeSoul[targetSlotID];

	//Set absorbed soul
	FClientItemInfo *pSoulItemInfo = NULL;
	UItem *pSoulItem = NULL;
	const SoulInfo* pSoulInfo = NULL;
	if( containerID >= 0 && slotID >= 0 ){
		pSoulItemInfo = mmoFacade.FindItem( containerID, slotID );
		if( pSoulItemInfo == NULL ){
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_ITEM_NOT_FOUND") ) ,MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_ITEM_NOT_FOUND"));
			return FALSE;
		}

		//
		pSoulItem = mmoFacade.FindItemTemplate( pSoulItemInfo->m_nTemplateID );
		if( pSoulItem == NULL ){
			warnf(NAME_Error, TEXT("======!!!!======= [SetLevelUpSoul soul Template Not FOUND]") );
			return FALSE;
		}

		//
		pSoulInfo = equipMgr.FindSoulInfoByItemTemplateID( pSoulItemInfo->m_nTemplateID );
		if( pSoulInfo == NULL ){
			//The item placed is not a spirit item
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_NOT_A_SOUL") ) ,MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_NOT_A_SOUL"));
			return FALSE;
		}

		//
		UBOOL isAlreadyInUse = pSoulItemInfo->IsDragIntoOtherUI > -1;
		if( isAlreadyInUse ){
			//
			UBOOL isInLevelUpSlot = IsInLevelUpSoulSlot( containerID, slotID );
			if( pSoulItemInfo->IsDragIntoOtherUI == ESPType_SoulLevelUp && !isInLevelUpSlot ){
				UBOOL isEnough = IsEnoughSoulsCanAddMoreScrifice( containerID, slotID );
				if( !isEnough ){
					//The placed spirit stack count is insufficient
					MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_NOT_ENOUGH_SOUL") ),MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_NOT_ENOUGH_SOUL") );
					return FALSE;
				}
			}
			else{
				//Being used (probably in the spirit upgrade slot or in a trade)
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Soul.ERROR_ITEM_IN_USE") ) ,MsgInfoType_Table,TEXT("MMOGame.Soul.ERROR_ITEM_IN_USE"));
				return FALSE;
			}
		}
	}


	//Unlock the soul previously placed in the bag
	//UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	if( ( pSoulItemInfo == NULL ) && ( toSacrifySoulPos._ConID >= 0 && toSacrifySoulPos._SlotID >= 0 ) ){
		//Clear, remove soul
		//if( pBagPanel != NULL && toSacrifySoulPos._ConID == E_CONSLOT_DEFAULT ){
			INT puttedCnt = 0;
			for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
				if( _SacrificeSoul[i]._ConID == toSacrifySoulPos._ConID && _SacrificeSoul[i]._SlotID == toSacrifySoulPos._SlotID ){
					++puttedCnt;
				}
			}

			if( puttedCnt <= 1 ){
				MMOFacade::getSingleton().LockItem( toSacrifySoulPos._ConID, toSacrifySoulPos._SlotID, -1 );
			}	
		//}

		//GFXPlayer->SetItemSlotData( pSlotMc, NULL, ESPType_SoulLevelUp, -1, -1 );
		GFXPlayer->eventCallSetItemData( pSlotMc , 0 , TEXT("") , -1, ESPType_SoulLevelUp, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );

		//
		toSacrifySoulPos._ConID = -1;
		toSacrifySoulPos._SlotID = -1;

		//Refresh success rate
		UpdateSuccessRatio();

		return TRUE;
	}

	//
	toSacrifySoulPos._ConID = containerID;
	toSacrifySoulPos._SlotID = slotID;

	//set UI ItemSlot
	//GFXPlayer->SetItemSlotData( pSlotMc, pSoulItem, ESPType_SoulLevelUp, toSacrifySoulPos._ConID, toSacrifySoulPos._SlotID );
	if( pSoulItem != NULL ){
		GFXPlayer->eventCallSetItemData( pSlotMc, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pSoulItem ), pSoulItem->ItemId, ESPType_SoulLevelUp, TEXT(""), pSoulItem->ItemColor+1, toSacrifySoulPos._ConID, toSacrifySoulPos._SlotID );
	}
	else{
		GFXPlayer->eventCallSetItemData( pSlotMc , 0 , TEXT("") , -1, ESPType_SoulLevelUp, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
	}

	//Lock the soul previously placed in the bag
	//if( pBagPanel != NULL && toSacrifySoulPos._ConID == E_CONSLOT_DEFAULT ){
		MMOFacade::getSingleton().LockItem( toSacrifySoulPos._ConID, toSacrifySoulPos._SlotID, ESPType_SoulLevelUp );
	//}

	//Refresh success rate
	UpdateSuccessRatio();

	return TRUE;
}

//
UBOOL UMMOUISoulLevelUp::UnSetLevelUpSoulSuccessHelp()
{
	return SetLevelUpSoulSuccessHelp( -1, -1 );
}

UBOOL UMMOUISoulLevelUp::SetLevelUpSoulSuccessHelp( INT containerID, INT slotID )
{
	return TRUE;
}

//=============================================================================================================================
void UMMOUISoulLevelUp::ConfirmSoulLevelUp()
{
	//No spirit to upgrade
	if( _ToLvUpSoul._ConID < 0 || _ToLvUpSoul._SlotID < 0 ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.ERROR_SOUL_NOT_INPUT") ) ,MsgInfoType_Table,TEXT("MMOGame.SoulLevelUp.ERROR_SOUL_NOT_INPUT") );
		return;
	}

	STLIntVector scarificeSoulContVector;
	STLIntVector scarificeSoulSlotVector;
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID < 0 || _SacrificeSoul[i]._SlotID < 0 ){
			continue;
		}
		//
		scarificeSoulContVector.push_back( _SacrificeSoul[i]._ConID );
		scarificeSoulSlotVector.push_back( _SacrificeSoul[i]._SlotID );
	}

	//No absorbed spirit, upgrade cannot succeed
	if( scarificeSoulContVector.empty() ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.ERROR_NEED_SACRIFICE_SOUL") ),MsgInfoType_Table,TEXT("MMOGame.SoulLevelUp.ERROR_NEED_SACRIFICE_SOUL") );
		return;
	}
	//
	UINT scarificeSoulCnt = scarificeSoulContVector.size();
	CMessageHandler::GetInstance().Send_UpgradeSoul( _ToLvUpSoul._ConID, _ToLvUpSoul._SlotID, scarificeSoulCnt, scarificeSoulContVector, scarificeSoulSlotVector );
}

//=============================================================================================================================
void UMMOUISoulLevelUp::OnNotifySoulLevelUpResult( int result )
{
	if( result != FALSE ){
		//Success; BagSlotChange notification handles the refresh
		//Continuous upgrade not supported, so clear it here
		UnSetLevelUpSoul();

		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.ERROR_SOUL_LEVELUP_SUCCEED") ) ,MsgInfoType_Table,TEXT("MMOGame.SoulLevelUp.ERROR_SOUL_LEVELUP_SUCCEED"));
	}
	else{
		//Failed
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.ERROR_SOUL_LEVELUP_FAILED") ),MsgInfoType_Table,TEXT("MMOGame.SoulLevelUp.ERROR_SOUL_LEVELUP_FAILED") );
	}
}
//=============================================================================================================================
void UMMOUISoulLevelUp::ResetSoulLevelUp()
{
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID >= 0 && _SacrificeSoul[i]._SlotID >= 0 ){
			UnSetLevelUpSoulSacrifice( i );
		}
	}
	//
	UnSetLevelUpSoul();
}
//=============================================================================================================================
UBOOL UMMOUISoulLevelUp::IsInLevelUpSoulSlot( INT conID, INT slotID )
{
	return ( _ToLvUpSoul._ConID == conID && _ToLvUpSoul._SlotID == slotID );
}
//
UBOOL UMMOUISoulLevelUp::IsInScrificeSoulSlot( INT conID, INT slotID )
{
	UBOOL isIn = FALSE;
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID == conID && _SacrificeSoul[i]._SlotID == slotID ){
			isIn = TRUE;
			break;
		}
	}

	return isIn;
}
//=============================================================================================================================
UBOOL UMMOUISoulLevelUp::IsEnoughSoulsCanAddMoreScrifice( INT conID, INT slotID )
{
	if( conID < 0 || slotID < 0 ){
		return FALSE;
	}

	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( conID, slotID );
	if( pItemInfo == NULL ){
		return FALSE;
	}

	UINT thisSoulAddCnt = 0;
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID == conID && _SacrificeSoul[i]._SlotID == slotID ){
			++thisSoulAddCnt;
		}
	}

	//
	if( thisSoulAddCnt >= (UINT)pItemInfo->m_nCurDur ){
		return FALSE;
	}

	return TRUE;
}
//=============================================================================================================================
INT UMMOUISoulLevelUp::AutoUnSetPackedSoulScrifice( INT conID, INT slotID )
{
	if( conID < 0 || slotID < 0 ){
		return 0;
	}

	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( conID, slotID );
	if( pItemInfo == NULL ){
		return 0;
	}

	UINT thisSoulAddCnt = 0;
	UINT allSoulCnt = (UINT)pItemInfo->m_nCurDur;
	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		if( _SacrificeSoul[i]._ConID == conID && _SacrificeSoul[i]._SlotID == slotID ){
			++thisSoulAddCnt;
		}
		//
		if( thisSoulAddCnt > allSoulCnt ){
			UnSetLevelUpSoulSacrifice( i );
		}
	}

	INT resultCnt = (thisSoulAddCnt > allSoulCnt ? thisSoulAddCnt - allSoulCnt : 0);

	return resultCnt;
}
//=============================================================================================================================
void UMMOUISoulLevelUp::Init()
{
	Super::Init();

	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
	//
	InitSoulLevelUpMc();

	//
	ResetUI();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_SoulLevelUp);
	EventAdapter = new TGameEventAdapter<UMMOUISoulLevelUp>(this, evts);
}

//=============================================================================================================================
void UMMOUISoulLevelUp::InitSoulLevelUpMc()
{
	_ToLvUpSoulMc = GetObject( TEXT("ToLvUpSoulMc") );
	_ToLvUpSoulMc->SetInt( TEXT("EquipID"), ESOLPST_LvUpSoul );
	_ToLvUpSoulMc->SetInt( TEXT("SlotType"), ESPType_SoulLevelUp );

	for( INT i = 0; i < MAX_SacrificeSoul; ++i ){
		FString objName = TEXT("SacrificeSoulMc");
		objName += appItoa( i );
		_SacrificeSoulMc[i] = GetObject( objName );

		_SacrificeSoulMc[i]->SetInt( TEXT("EquipID"), ESOLPST_LvUpScrificSoul1 + i );
		_SacrificeSoulMc[i]->SetInt( TEXT("SlotType"), ESPType_SoulLevelUp );
	}

	_HelpLvUpSoulItemMc = GetObject( TEXT("HelpLvUpSoulItemMc") );
	_HelpLvUpSoulItemMc->SetInt( TEXT("EquipID"), ESOLPST_LvUpHelpItem );
	_HelpLvUpSoulItemMc->SetInt( TEXT("SlotType"), ESPType_SoulLevelUp );
	_HelpLvUpSoulItemMc->SetVisible( FALSE );

	_SuccessRitoTf = GetObject( TEXT("SuccessRitoTf") );
	_CurLevelSoulPropTf = GetObject( TEXT("CurLevelSoulPropTf") );
	_NextLevelSoulPropTf = GetObject( TEXT("NextLevelSoulPropTf") );

	
	_ConfirmLevelUpBtn = GetObject( TEXT("ConfirmLevelUpBtn") );
	_ConfirmLevelUpBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.ConfirmLevelUpBtnText") ) );

	_ResetBtn = GetObject( TEXT("ResetBtn") );
	_ResetBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SoulLevelUp.ResetBtnText") ) );
}
//============================================================================================================
void UMMOUISoulLevelUp::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}

//===============================================================================================================
//===============================================================================================================
//soul level up
void UMMOUISoulLevelUp::ASConfirmSoulLevelUp()
{
	ConfirmSoulLevelUp();
}

//===============================================================================================================
void UMMOUISoulLevelUp::ASResetSoulLevelUp()
{
	ResetSoulLevelUp();
}
//===============================================================================================================
//===============================================================================================================