#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOEquipManager.h"
#include "MMOChatManager.h"
#include "UILayout.h"
#include "UITextParser.h"
#include "MMOQuestManager.h"
#include "ClientActionConditionCheck.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOUIEquipLevelUp)

//========================================================================================================================================
void UMMOUIEquipLevelUp::PostOnShow(UBOOL v)
{
	Super::PostOnShow( v );

	if( v ){
		GrayAllBagItemsNoNeed( TRUE );
	}
	else{
		ResetUI();

		GrayAllBagItemsNoNeed( FALSE );

		UMMOUIMutiLineMsgBox *pMutiLineMsgBox = (UMMOUIMutiLineMsgBox*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_MutiLineMsgBox );
		if( pMutiLineMsgBox != NULL && pMutiLineMsgBox->IsVisible ){
			pMutiLineMsgBox->Show( FALSE );
		}
	}
}

//
void UMMOUIEquipLevelUp::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			if( IsVisible ){
				BagSlotChange( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx );
			}
		}
		break;
	}
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	ItemInfo *pInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
	UBOOL isDeleted = FALSE;
	if( pInfo == NULL ){
		isDeleted = TRUE;
	}
	//update enhance slot
	if( IsCurLevelUpEquip( bagId, slotId ) ){
		if( isDeleted ){
			UnSetLevelUpEquip();
		}
		else{
			SetLevelUpEquip( bagId, slotId );
		}

		return;
	}
	
	INT curInPanelPos = IsCurLevelUpConsumeItem( bagId, slotId );
	if( curInPanelPos >= 0 ){
		if( isDeleted ){
			UnSetLevelUpConsume( curInPanelPos );
		}
		else{
			SetLevelUpConsume( curInPanelPos, bagId, slotId );
		}
		return;
	}

	curInPanelPos = IsCurLevelUpHelpItem( bagId, slotId );
	if( curInPanelPos >= 0 ){
		if( isDeleted ){
			UnSetLevelUpHelpItem( curInPanelPos );
		}
		else{
			SetLevelUpHelpItem( curInPanelPos, bagId, slotId );
		}

		return;
	}
}

//========================================================================================================================================
UBOOL UMMOUIEquipLevelUp::IsCurLevelUpEquip( INT bagSlotCon, INT bagSlotID )
{
	return ( _ToOpEquip._ConID == bagSlotCon && _ToOpEquip._SlotID == bagSlotID );
}

INT UMMOUIEquipLevelUp::IsCurLevelUpConsumeItem( INT bagSlotCon, INT bagSlotID )
{
	for( INT i = 0; i < TO_CONSUM_MAT_MAX; ++i ){
		if( _ToConsumItem[i]._ConID == bagSlotCon && _ToConsumItem[i]._SlotID == bagSlotID ){
			return i;
		}
	}

	return -1;
}

INT UMMOUIEquipLevelUp::IsCurLevelUpHelpItem( INT bagSlotCon, INT bagSlotID )
{
	for( INT i = 0; i < TO_HELP_ITEM_MAX; ++i ){
		if( _ToHelpItem[i]._ConID == bagSlotCon && _ToHelpItem[i]._SlotID == bagSlotID ){
			return i;
		}
	}

	return -1;
}
//========================================================================================================================================
void UMMOUIEquipLevelUp::PlayerMoneyChange( BYTE MoneyType, INT DeltaMoney )
{
	if( !IsVisible ){
		return;
	}
	//update text color
	UpdateFee();
}
//========================================================================================================================================
//
UBOOL UMMOUIEquipLevelUp::OnLeftMousePress( class UGFxObject* mc, BYTE slotType, BYTE containerID, INT slotID, UBOOL bDefault )
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIEquipLevelUp::OnLeftMouseClick( class UGFxObject* mc, BYTE slotType, BYTE containerID, INT slotID, UBOOL bDefault )
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIEquipLevelUp::OnRightMouseClick( class UGFxObject* mc, BYTE slotType, BYTE containerID, INT slotID, UBOOL bDefault )
{
	if(bDefault)return FALSE;

	if( slotType == ESPType_Bag ){
		//put in from bag
		FClientItemInfo *pInfo = MMOFacade::getSingleton().FindItem( containerID, slotID );
		if( pInfo == NULL ){
			return TRUE;
		}
		//
		UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( pInfo->m_nTemplateID );
		if( pItem == NULL ){
			return TRUE;
		}

		//
		ClientActionCondition_EquipLevelUp checker;

		FClientActionConditionContex contex;
		contex.Item = pInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		if( IsItemCanLevelUp( pItem ) ){
			SetLevelUpEquip( containerID, slotID );
		}
		else if( IsItemConsumeForLevelUp( pItem ) ){
			AutoSetLevelUpConsumes( containerID, slotID );
		}
		else if( IsItemCanHelpLevelUp( pItem ) ){
			AutoSetLevelUpHelpItems( containerID, slotID );
		}

		return TRUE;
	}
	else if( slotType == ESPType_EquipLevelUp ){
		INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") ); 
		switch( panelSlotID )
		{
		case EEpLvUpPnTp_ToOp:
			{
				UnSetLevelUpEquip();
			}
			break;
		case EEpLvUpPnTp_HelpItem1:
		case EEpLvUpPnTp_HelpItem2:
		case EEpLvUpPnTp_HelpItem3:
		case EEpLvUpPnTp_HelpItem4:
		case EEpLvUpPnTp_HelpItem5:
			{
				INT idx = panelSlotID - EEpLvUpPnTp_HelpItem1;
				UnSetLevelUpHelpItem( idx );
			}
			break;
		case EEpLvUpPnTp_CnsmItem1:
		case EEpLvUpPnTp_CnsmItem2:
		case EEpLvUpPnTp_CnsmItem3:
		case EEpLvUpPnTp_CnsmItem4:
		case EEpLvUpPnTp_CnsmItem5:
			{
				INT idx = panelSlotID - EEpLvUpPnTp_CnsmItem1;
				UnSetLevelUpConsume( idx );
			}
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//
UBOOL UMMOUIEquipLevelUp::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	if( (srcSlotType == ESPType_Bag) && (destSlotType == ESPType_EquipLevelUp) ){
		//put in equip from bag
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( srcContainerID, srcSlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		ClientActionCondition_EquipLevelUp checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		INT panelSlotID = (INT)destSlotMC->GetFloat( TEXT("EquipID") ); 
		switch( panelSlotID )
		{
		case EEpLvUpPnTp_ToOp:
			{
				SetLevelUpEquip( srcContainerID, srcSlotID );
			}
			break;
		case EEpLvUpPnTp_HelpItem1:
		case EEpLvUpPnTp_HelpItem2:
		case EEpLvUpPnTp_HelpItem3:
		case EEpLvUpPnTp_HelpItem4:
		case EEpLvUpPnTp_HelpItem5:
			{
				INT idx = panelSlotID - EEpLvUpPnTp_HelpItem1;
				SetLevelUpHelpItem( idx, srcContainerID, srcSlotID );
			}
			break;
		case EEpLvUpPnTp_CnsmItem1:
		case EEpLvUpPnTp_CnsmItem2:
		case EEpLvUpPnTp_CnsmItem3:
		case EEpLvUpPnTp_CnsmItem4:
		case EEpLvUpPnTp_CnsmItem5:
			{
				INT idx = panelSlotID - EEpLvUpPnTp_CnsmItem1;
				SetLevelUpConsume( idx, srcContainerID, srcSlotID );
			}
			break;
		}

		return TRUE;
	}
	else if( (destSlotType == ESPType_Bag) && (srcSlotType == ESPType_EquipLevelUp) ){
		//Cancel placed item
		INT panelSlotID = (INT)srcSlotMC->GetFloat( "EquipID" );
		switch( panelSlotID )
		{
		case EEpLvUpPnTp_ToOp:
			{
				UnSetLevelUpEquip();
			}
			break;
		case EEpLvUpPnTp_HelpItem1:
		case EEpLvUpPnTp_HelpItem2:
		case EEpLvUpPnTp_HelpItem3:
		case EEpLvUpPnTp_HelpItem4:
		case EEpLvUpPnTp_HelpItem5:
			{
				INT idx = panelSlotID - EEpLvUpPnTp_HelpItem1;
				UnSetLevelUpHelpItem( idx );
			}
			break;
		case EEpLvUpPnTp_CnsmItem1:
		case EEpLvUpPnTp_CnsmItem2:
		case EEpLvUpPnTp_CnsmItem3:
		case EEpLvUpPnTp_CnsmItem4:
		case EEpLvUpPnTp_CnsmItem5:
			{
				INT idx = panelSlotID - EEpLvUpPnTp_CnsmItem1;
				UnSetLevelUpConsume( idx );
			}
			break;
		}

		return TRUE;
	}

	return FALSE;
}
//========================================================================================================================================
void UMMOUIEquipLevelUp::UpdatePanel()
{
	UpdateFee();
}

//========================================================================================================================================
//Reset current UI display content
void UMMOUIEquipLevelUp::ResetPanel()
{
	UnSetLevelUpEquip();

	ResetAllLevelUpConsumes();
	ResetAllLevelUpHelpItems();

	UpdateFee();
}
//========================================================================================================================================
//Update enhancement cost
void UMMOUIEquipLevelUp::UpdateFee()
{
	INT moneyCnt = -1;
	if( _ToOpEquip._ConID >= 0 && _ToOpEquip._SlotID >= 0 ){
		FClientItemInfo *pEquipInfo = MMOFacade::getSingleton().FindItem( _ToOpEquip._ConID, _ToOpEquip._SlotID );
		if( pEquipInfo != NULL ){
			const EquipLvUpCfg *pLevelUpData = NULL;//MMOEquipManager::getSingleton().FindEquipLevelUpData( pEquipInfo->m_byLevel );
			if( pLevelUpData != NULL ){
				moneyCnt = pLevelUpData->_MnyCost;
			}

			//Update condition flags
			IsMnyOK = ( pLevelUpData != NULL && moneyCnt <= Player->GetMoney( E_MONEYTYPE_02 ) );
		}
	}
	else{
		moneyCnt = 0;
		IsMnyOK = FALSE;
	}

	//
	FString strMny = TEXT("");
	if( moneyCnt >= 0 ){
		strMny = UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02, moneyCnt );
	}
	eventCallASSetUpgradeCostMoney( strMny );
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::UnSetLevelUpEquip()
{
	SetLevelUpEquip( -1, -1 );
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::SetLevelUpEquip( INT bagSlotCon, INT bagSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	if( bagSlotCon < 0 || bagSlotID < 0 ){
		if( _ToOpEquip._ConID >= 0 && _ToOpEquip._SlotID >= 0 ){
			//
			MMOFacade::getSingleton().LockItem( _ToOpEquip._ConID, _ToOpEquip._SlotID, -1 );

			GFXPlayer->eventCallSetItemData( _ToOpEquipSlot, 0, TEXT(""), -1, ESPType_EquipLevelUp, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		_ToOpEquip._ConID = -1;
		_ToOpEquip._SlotID = -1;

		//
		UpdateFee();

		return TRUE;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( bagSlotCon, bagSlotID );
	if( pInfo == NULL ){
		MMOFacade::getSingleton().LockItem( _ToOpEquip._ConID, _ToOpEquip._SlotID, -1 );
		GFXPlayer->eventCallSetItemData( _ToOpEquipSlot, 0, TEXT(""), -1, ESPType_EquipLevelUp, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	//Whether it is a weapon
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	UBOOL isFitToPutIn = IsItemCanLevelUp( pItemToOperation );
	if( !isFitToPutIn ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.errNotFitLevelUp") ), MsgInfoType_Table, TEXT("MMOGame.EquipLevelUp.errNotFitLevelUp") );
		return FALSE;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( _ToOpEquip._ConID >= 0 && _ToOpEquip._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _ToOpEquip._ConID, _ToOpEquip._SlotID, -1 );
	}

	_ToOpEquip._ConID = bagSlotCon;
	_ToOpEquip._SlotID = bagSlotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( _ToOpEquipSlot, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItemToOperation, pInfo ), pItemToOperation->ItemId, 
		ESPType_EquipLevelUp, TEXT(""), pItemToOperation->ItemColor+1, _ToOpEquip._ConID, _ToOpEquip._SlotID );
	
	//lock it
	if( _ToOpEquip._ConID >= 0 && _ToOpEquip._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _ToOpEquip._ConID, _ToOpEquip._SlotID, ESPType_EquipLevelUp );
	}

	//
	UpdateFee();

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::IsItemCanLevelUp( const UItem *pItem )
{
	if( pItem == NULL ){
		return FALSE;
	}

	const UEquip *pWeaponToOperation = ConstCast<UEquip>( pItem );
	if( pWeaponToOperation != NULL && pWeaponToOperation->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
		return TRUE;
	}

	return FALSE;
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::UnSetLevelUpConsume( INT idx )
{
	SetLevelUpConsume( idx, -1, -1 );
}

//---------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipLevelUp::ResetAllLevelUpConsumes()
{
	for( INT i = 0; i < TO_CONSUM_MAT_MAX; ++i ){
		UnSetLevelUpConsume( i );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::AutoSetLevelUpConsumes( INT bagSlotCon, INT bagSlotID )
{
	for( INT i = 0; i < TO_CONSUM_MAT_MAX; ++i ){
		if( _ToConsumItem[i]._ConID < 0 && _ToConsumItem[i]._SlotID < 0 ){
			SetLevelUpConsume( i, bagSlotCon, bagSlotID );
			return TRUE;
		}
	}

	FString errorName = TEXT("MMOGame.EquipLevelUp.ConsumesIsFull");
	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
	return FALSE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::SetLevelUpConsume( INT idx, INT bagSlotCon, INT bagSlotID )
{
	if( idx < 0 || idx >= TO_CONSUM_MAT_MAX ){
		return FALSE;
	}

	FOperationItemBagPos &opItemPos = _ToConsumItem[idx];
	UGFxObject *opSlot = _ToConsumItemSlot[idx];

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	if( bagSlotCon < 0 || bagSlotID < 0 ){
		if( opItemPos._ConID >= 0 && opItemPos._SlotID >= 0 ){
			//
			MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, -1 );
			GFXPlayer->eventCallSetItemData( opSlot, 0, TEXT(""), -1, ESPType_EquipLevelUp, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		opItemPos._ConID = -1;
		opItemPos._SlotID = -1;

		return TRUE;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( bagSlotCon, bagSlotID );
	if( pInfo == NULL ){
		MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, -1 );
		GFXPlayer->eventCallSetItemData( opSlot, 0, TEXT(""), -1, ESPType_EquipLevelUp, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	//Whether it is a weapon
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	UBOOL isFitToPutIn = IsItemConsumeForLevelUp( pItemToOperation );
	if( !isFitToPutIn ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.errNotFitConsumeItem") ), MsgInfoType_Table, TEXT("MMOGame.EquipLevelUp.errNotFitConsumeItem") );
		return FALSE;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( opItemPos._ConID >= 0 && opItemPos._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, -1 );
	}

	opItemPos._ConID = bagSlotCon;
	opItemPos._SlotID = bagSlotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( opSlot, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItemToOperation, pInfo ), pItemToOperation->ItemId, 
		ESPType_EquipLevelUp, TEXT(""), pItemToOperation->ItemColor+1, opItemPos._ConID, opItemPos._SlotID );

	//lock it
	if( opItemPos._ConID >= 0 && opItemPos._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, ESPType_EquipLevelUp );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::IsItemConsumeForLevelUp( const UItem *pItem )
{
	if( pItem == NULL ){
		return FALSE;
	}

	if( pItem->ItemSubType == TEXT("ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP") ){
		return TRUE;
	}

	return FALSE;
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::UnSetLevelUpHelpItem( INT idx )
{
	SetLevelUpHelpItem( idx, -1, -1 );
}

//---------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipLevelUp::ResetAllLevelUpHelpItems()
{
	for( INT i = 0; i < TO_HELP_ITEM_MAX; ++i ){
		UnSetLevelUpHelpItem( i );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::AutoSetLevelUpHelpItems( INT bagSlotCon, INT bagSlotID )
{
	for( INT i = 0; i < TO_HELP_ITEM_MAX; ++i ){
		if( _ToHelpItem[i]._ConID < 0 && _ToHelpItem[i]._SlotID < 0 ){
			SetLevelUpHelpItem( i, bagSlotCon, bagSlotID );
			return TRUE;
		}
	}

	FString errorName = TEXT("MMOGame.EquipLevelUp.HelpItemsIsFull");
	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
	return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::SetLevelUpHelpItem( INT idx, INT bagSlotCon, INT bagSlotID )
{
	if( idx < 0 || idx >= TO_HELP_ITEM_MAX ){
		return FALSE;
	}

	FOperationItemBagPos &opItemPos = _ToHelpItem[idx];
	UGFxObject *opSlot = _ToHelpItemSlot[idx];

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	if( bagSlotCon < 0 || bagSlotID < 0 ){
		if( opItemPos._ConID >= 0 && opItemPos._SlotID >= 0 ){
			//
			MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, -1 );

			GFXPlayer->eventCallSetItemData( opSlot, 0, TEXT(""), -1, ESPType_EquipLevelUp, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		opItemPos._ConID = -1;
		opItemPos._SlotID = -1;

		return TRUE;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( bagSlotCon, bagSlotID );
	if( pInfo == NULL ){
		MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, -1 );
		GFXPlayer->eventCallSetItemData( opSlot, 0, TEXT(""), -1, ESPType_EquipLevelUp, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		//MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") ), MsgInfoType_Table, TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	//Whether it is an accessory
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	UBOOL isFitToPutIn = IsItemCanHelpLevelUp( pItemToOperation );
	if( !isFitToPutIn ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.errNotFitHelpItem") ), MsgInfoType_Table, TEXT("MMOGame.EquipLevelUp.errNotFitHelpItem") );
		return FALSE;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( opItemPos._ConID >= 0 && opItemPos._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, -1 );
	}

	opItemPos._ConID = bagSlotCon;
	opItemPos._SlotID = bagSlotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( opSlot, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItemToOperation, pInfo ), pItemToOperation->ItemId, 
		ESPType_EquipLevelUp, TEXT(""), pItemToOperation->ItemColor+1, opItemPos._ConID, opItemPos._SlotID );

	//lock it
	if( opItemPos._ConID >= 0 && opItemPos._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( opItemPos._ConID, opItemPos._SlotID, ESPType_EquipLevelUp );
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipLevelUp::IsItemCanHelpLevelUp( const UItem *pItem )
{
	if( pItem == NULL ){
		return FALSE;
	}

	if( pItem->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_NECKLACE") ||
		pItem->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_RING") ||
		pItem->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_BRACELET") ){
		return TRUE;
	}

	return FALSE;
}
//========================================================================================================================================
void UMMOUIEquipLevelUp::GrayAllBagItemsNoNeed( UBOOL isToGray )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	if( Player == NULL ){
		return;
	}

	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	for( UINT j = 0; j < bagCapability; ++j){
		//
		if( isToGray == FALSE ){
			mmoFacade.GrayItem( ESGMark_EquipLevelUp, E_CONSLOT_DEFAULT, j, FALSE );
			continue;
		}

		//needs graying out
		const FClientItemInfo *pItemInfo = mmoFacade.FindItem( E_CONSLOT_DEFAULT, j );
		if( pItemInfo == NULL ){
			continue;
		}

		UItem *pItemInBag = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
		if( pItemInBag == NULL ){
			continue;
		}

		UBOOL isNeedToGray = IsNeedToGray( pItemInBag, pItemInfo );
		mmoFacade.GrayItem( ESGMark_EquipLevelUp, E_CONSLOT_DEFAULT, j, isNeedToGray );
	}
}

//========================================================================================================================================
UBOOL UMMOUIEquipLevelUp::IsNeedToGray( UItem *pItemInBag, const FClientItemInfo *pItemInfo )
{
	if( pItemInBag == NULL || pItemInfo == NULL ){
		return FALSE;
	}

	UBOOL isUsefulItem = IsItemCanLevelUp( pItemInBag );
	isUsefulItem |= IsItemConsumeForLevelUp( pItemInBag );
	isUsefulItem |= IsItemCanHelpLevelUp( pItemInBag );

	return (!isUsefulItem);
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::ConfirmLevelUpEquip()
{
	FString errStrName;
	FString errorHint;
	UBOOL isOk = CheckCanGo( errStrName, errorHint );
	if( !isOk ){
		MMOChatManager::getSingleton().AddSystemMessage( errorHint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( errorHint ,MsgInfoType_Table, errStrName);

		return;
	}

	if( _ConfirmBox != NULL ){
		return;
	}

	UBOOL isShowConfirmBox = FALSE;
	GConfig->GetBool( TEXT("PopUpMsgBox"), TEXT("ConfirmEquipLevelUp") , isShowConfirmBox , GDefaultGameIni );
	if( isShowConfirmBox ){
		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = this;
		funYes.FunctionName = TEXT( "ConfirmLevelUpMessageAndGo" );

		FScriptDelegate funNo(EC_EventParm);
		funNo.Object = this;
		funNo.FunctionName = TEXT( "CancelLevelUpMessage" );

		FString strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpTitle") );
		FString strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpContent") );

		_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBoxWithCheck( strTitle, 0, strContent, funYes, funNo, TRUE );
		if( _ConfirmBox != NULL ){
			_ConfirmBox->SetBool( TEXT("forbidHint"), FALSE );;
		}
	}
	else{
		//CMessageHandler::GetInstance().Send_EquipLevelUp( _ToOpEquip, _ToConsumItem, _ToHelpItem );
		warnf( NAME_Error, TEXT("[UMMOUIEquipLevelUp::ConfirmLevelUpEquip] Sending server function has been removed") );
	}
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIEquipLevelUp::ConfirmLevelUpMessageAndGo( UGFxObject *pMc )
{
	if( _ConfirmBox == NULL ){
		return;
	}

	UBOOL isNotShowAgain = _ConfirmBox->GetBool( TEXT("forbidHint") );
	GConfig->SetBool( TEXT("PopUpMsgBox"), TEXT("ConfirmEquipLevelUp"), !isNotShowAgain, GDefaultGameIni );

	_ConfirmBox = NULL;
	//
	//CMessageHandler::GetInstance().Send_EquipLevelUp( _ToOpEquip, _ToConsumItem, _ToHelpItem );
	warnf( NAME_Error, TEXT("[UMMOUIEquipLevelUp::ConfirmLevelUpMessageAndGo] Sending server function has been removed") );
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIEquipLevelUp::CancelLevelUpMessage( UGFxObject *pMc )
{
	_ConfirmBox = NULL;
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::NotifyLevelUpResult( INT succeedType, INT duribilityEnum )
{
	warnf( NAME_Error, TEXT("[UMMOUIEquipLevelUp::NotifyLevelUpResult] No this function") );
	//switch( succeedType )
	//{
	//case eelrCrash:
	//	{
	//		InitFaildCrashConfirmBox();
	//	}
	//	break;
	//case eelrPlus1:
	//case eelrPlus2:
	//case eelrPlus3:
	//	{
	//		InitSuccessConfirmBox( succeedType - eelrPlus1 + 1, duribilityEnum );

	//		// Tutorial quest
	//		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_UpgradeEquip);
	//	}
	//	break;
	//case eelrUnchanged:
	//	{
	//		InitFaildUnchangedConfirmBox();
	//	}
	//	break;
	//default:
	//	return;
	//}

	////For Test
	//ShowLevelUpResultConfirmBox( TRUE );
}

//======================================================================================================================================
void UMMOUIEquipLevelUp::InitSuccessConfirmBox( INT propAdd, INT duribilityEnum )
{
	return;
	//FString hintTitleName = TEXT( "MMOGame.EquipLevelUp.ResultSuccess" );
	//FString hintTitleText = MMOUI::ParseLocalizedPropertyPath( hintTitleName );
	//MMOChatManager::getSingleton().AddSystemMessage( hintTitleText);

	////
	//FString hintTxt;
	//FClientItemInfo *pEquipInfo = MMOFacade::getSingleton().FindItem( _ToOpEquip._ConID, _ToOpEquip._SlotID );
	//if( pEquipInfo != NULL ){
	//	UEquip *pEquip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( pEquipInfo->m_nTemplateID ) );
	//	if( pEquip != NULL ){
	//		hintTxt = pEquip->GetDisplayName();
	//		hintTxt += TEXT( "+" );
	//		hintTxt += TEXT("NO");//appItoa( pEquipInfo->m_byLevel );

	//		//
	//		hintTxt += TEXT("\n");
	//		if( pEquip->FixAttrbutes.WeaponPhyAtkLo > 0 || pEquip->FixAttrbutes.WeaponPhyAtkHi > 0){
	//			hintTxt += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.SucessHint2_1") ), propAdd );
	//		}
	//		else{
	//			hintTxt += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.SucessHint2_2") ), propAdd );
	//		}
	//		////
	//		//if( pEquipInfo->m_byLevel == EQUIP_LEVEL_MAX ){
	//		//	FString screenHint;
	//		//	FString fullLevelHintName = TEXT("MMOGame.EquipLevelUp.FullLevelHint");
	//		//	screenHint = FString::Printf( MMOUI::ParseLocalizedPropertyPath( fullLevelHintName ), *Player->RoleName, *pEquip->GetDisplayName() );
	//		//	MMOUI::GetUIManager()->delegateAddScreenMessage( screenHint, MsgInfoType_Table, fullLevelHintName );
	//		//}
	//	}
	//}

	////
	//INT duribilityValue = 0;
	//FString strLink = TEXT("");//TEXT("-");
	//switch( duribilityEnum )
	//{
	//case eeldMinus2: duribilityValue = -2; break;
	//case eeldMinus1: duribilityValue = -1; break;
	//case eeldPlus0: duribilityValue = 0; strLink = TEXT(""); break;
	//case eeldPlus1: duribilityValue = 1; strLink = TEXT("+"); break;
	//case eeldMinus3: duribilityValue = -3; break;
	//}
	//
	//if( duribilityValue != 0 ){
	//	hintTxt += TEXT("\n");
	//	hintTxt += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.SucessHint3") );
	//	hintTxt += strLink;
	//	hintTxt += appItoa( duribilityValue );
	//}
	//else{
	//	hintTxt += TEXT("\n");
	//	hintTxt += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.SucessHint3_2") );
	//}

	////
	//UMMOUIMutiLineMsgBox *pMutiLineMsgBox = (UMMOUIMutiLineMsgBox*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_MutiLineMsgBox );
	//if( pMutiLineMsgBox != NULL ){
	//	pMutiLineMsgBox->ShowContent( 0, hintTitleText, hintTxt );
	//}
}

//======================================================================================================================================
void UMMOUIEquipLevelUp::InitFaildCrashConfirmBox()
{
	FString hintTitleName = TEXT( "MMOGame.EquipLevelUp.ResultFailedCrash" );
	FString hintTitleText = MMOUI::ParseLocalizedPropertyPath( hintTitleName );

	MMOUI::GetUIManager()->delegateAddScreenMessage( hintTitleText ,MsgInfoType_Table, hintTitleName );
	MMOChatManager::getSingleton().AddSystemMessage( hintTitleText );

	//UGFxObject *initData = GFXPlayer->CreateObject( TEXT("UI_EquipLevelUp.AS3.UpgradeResultData") );
	//if( initData == NULL ){
	//	return;
	//}
	//initData->SetInt( TEXT("resultType"), 2 );
	//initData->SetString( TEXT("messageTitle"), hintTitleText );
	//initData->SetString( TEXT("messageContent"), hintString );

	//eventCallASSetPopUpBox( initData );
}

//======================================================================================================================================
void UMMOUIEquipLevelUp::InitFaildUnchangedConfirmBox()
{
	FString hintTitleName = TEXT( "MMOGame.EquipLevelUp.ResultFailed" );
	FString hintTitleText = MMOUI::ParseLocalizedPropertyPath( hintTitleName );
	MMOChatManager::getSingleton().AddSystemMessage( hintTitleText);

	FString hintStringName = TEXT( "MMOGame.EquipLevelUp.ResultNoChanged" );
	FString hintString = MMOUI::ParseLocalizedPropertyPath( hintStringName );

	//
	UMMOUIMutiLineMsgBox *pMutiLineMsgBox = (UMMOUIMutiLineMsgBox*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_MutiLineMsgBox );
	if( pMutiLineMsgBox != NULL ){
		pMutiLineMsgBox->ShowContent( 0, hintTitleText, hintString );
	}
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::ShowLevelUpResultConfirmBox( UBOOL isToShow )
{
	eventCallAsShowPopUpBox( isToShow );
}

//========================================================================================================================================
//Check whether the operation can be performed
UBOOL UMMOUIEquipLevelUp::CheckCanGo( FString &errStrName, FString &errorHint )
{
	UBOOL bRet = IsMnyOK;

	if( _ToOpEquip._ConID < 0 || _ToOpEquip._SlotID < 0 ){
		errStrName = TEXT("MMOGame.EquipLevelUp.NoLevelUpEquip");
		errorHint =  MMOUI::ParseLocalizedPropertyPath( errStrName );
		return FALSE;
	}

	//full level
	FClientItemInfo *pEquipInfo = MMOFacade::getSingleton().FindItem( _ToOpEquip._ConID, _ToOpEquip._SlotID );
	if( pEquipInfo != NULL ){
		if(FALSE){//if( pEquipInfo->m_byLevel >= EQUIP_LEVEL_MAX ){
			errStrName = TEXT("MMOGame.EquipLevelUp.EquipLevelMax");
			errorHint =  MMOUI::ParseLocalizedPropertyPath( errStrName );
			return FALSE;
		}
	}

	//no enough money
	if( IsMnyOK == FALSE ){
		errStrName = TEXT("MMOGame.EquipLevelUp.NoEnoughMny");
		errorHint =  MMOUI::ParseLocalizedPropertyPath( errStrName );
		return FALSE;
	}

	return TRUE;
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::ResetCurOperation()
{
	ResetAllLevelUpConsumes();
	ResetAllLevelUpHelpItems();
	UnSetLevelUpEquip();
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::Init()
{
	Super::Init();

	//
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );

	//
	InitMc();

	//
	UpdateFee();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Item_Delete);
	EventAdapter = new TGameEventAdapter<UMMOUIEquipLevelUp>(this, evts);
}

//========================================================================================================================================
void UMMOUIEquipLevelUp::InitMc()
{
	_ToOpEquipSlot = GetObject( TEXT("weapon_slot_mc") );
	if( _ToOpEquipSlot != NULL ){
		_ToOpEquipSlot->SetInt( TEXT("EquipID"), EEpLvUpPnTp_ToOp );
		_ToOpEquipSlot->SetInt( TEXT("SlotType"), ESPType_EquipLevelUp );
	}

	FString objNameFormat = TEXT("ironstone_slot_");
	UGFxObject *tempObj = NULL;
	for( INT i = 0; i < TO_CONSUM_MAT_MAX; ++i ){
		FString objName = objNameFormat + appItoa( i );
		tempObj = GetObject( objName );
		if( tempObj != NULL ){
			tempObj->SetInt( TEXT("EquipID"), EEpLvUpPnTp_CnsmItem1 + i );
			tempObj->SetInt( TEXT("SlotType"), ESPType_EquipLevelUp );
		}
		_ToConsumItemSlot[i] = tempObj;
	}

	objNameFormat = TEXT("jewelry_slot_");
	for( INT i = 0; i < TO_HELP_ITEM_MAX; ++i ){
		FString objName = objNameFormat + appItoa( i );
		tempObj = GetObject( objName );
		if( tempObj != NULL ){
			tempObj->SetInt( TEXT("EquipID"), EEpLvUpPnTp_HelpItem1 + i );
			tempObj->SetInt( TEXT("SlotType"), ESPType_EquipLevelUp );
		}
		_ToHelpItemSlot[i] = tempObj;
	}
}

//============================================================================================================
void UMMOUIEquipLevelUp::ResetUI()
{
	ResetCurOperation();
	CloseAllPopUpPanel();
}

void UMMOUIEquipLevelUp::CloseAllPopUpPanel()
{
	if( _ConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmBox );
		_ConfirmBox = NULL;
	}
}
//============================================================================================================
void UMMOUIEquipLevelUp::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIEquipLevelUp::ASConfirmUpgrade()
{
	ConfirmLevelUpEquip();
}

//==================================================================================================
void UMMOUIEquipLevelUp::ASCancelUpgrade()
{
	ResetCurOperation();
}
//==================================================================================================
void UMMOUIEquipLevelUp::ASShowResultConfirmBox( UBOOL isToShow )
{
	ShowLevelUpResultConfirmBox( isToShow );
}
//
void UMMOUIEquipLevelUp::ASConfirmMessage()
{
	ShowLevelUpResultConfirmBox( FALSE );
}
//===============================================================================================================
//===============================================================================================================