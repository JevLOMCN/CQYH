#include "MMOFacade.h"
#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "net/message_handler.h"
#include "ClientActionConditionCheck.h"
#include "MMOUIBackDecoratedMgr.h"
#include "item_def.h"
#include "UILayout.h"
#include "MMOMsgBoxManager.h"
#include "UITextParser.h"
#include "TabFile.h"

IMPLEMENT_CLASS(UMMOUIBackDecorated)

//========================================================================================================================================
void UMMOUIBackDecorated::PreOnShow( UBOOL show )
{
	Super::PreOnShow( show );

	if( show ){
		GrayAllInBagNoNeed( TRUE );
	}
	else{
		ResetUI();

		GrayAllInBagNoNeed( FALSE );
	}
}
//========================================================================================================================================

void UMMOUIBackDecorated::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;
			switch( _CurPanelType )
			{
			case EBackDecorate_MakeUp:
				{
					BagSlotChangeOnMakeUp( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID, itemDelEvt->ItemTemplateID );
				}
				break;
			case EBackDecorate_Upgrade:
				{
					BagSlotChangeOnUpgrade( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID, itemDelEvt->ItemTemplateID );
				}
				break;
			}
		}
		break;
	}
}
//========================================================================================================================================
void UMMOUIBackDecorated::GrayAllInBagNoNeed( UBOOL isToGray )
{
	switch( _CurPanelType )
	{
	case EBackDecorate_MakeUp:
		{
			GrayAllInBagNoNeedForMakeUp( isToGray );
		}
		break;
	case EBackDecorate_Upgrade:
		{
			GrayAllInBagNoNeedForUpgrade( isToGray );
		}
		break;
	default:
		return;
	}
}
//========================================================================================================================================
void UMMOUIBackDecorated::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	switch( _CurPanelType )
	{
	case EBackDecorate_MakeUp:
		{
			BagSlotChangeOnMakeUp( bagId, slotId, -1 );
		}
		break;
	case EBackDecorate_Upgrade:
		{
			BagSlotChangeOnUpgrade( bagId, slotId, -1 );
		}
		break;
	default:
		warnf(NAME_Error, TEXT("======!!!!======= [BagSlotChange _CurPanelType is WRONG]") );
		return;
	}

}
//========================================================================================================================================
void UMMOUIBackDecorated::UpdatePanel()
{
	switch( _CurPanelType )
	{
	case EBackDecorate_MakeUp:
		{
			UpdateMakeUpDetails();
		}
		break;
	case EBackDecorate_Upgrade:
		{
			UpdateUpgradeDetails();
		}
		break;
	default:
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIBackDecorated _CurPanelType is WRONG]") );
		return;
	}

	//
	SetTitleName();
}

//========================================================================================================================================
void UMMOUIBackDecorated::ResetUI()
{
	ResetPanel( _CurPanelType );
	//
	_CurPanelType = EBackDecorate_MakeUp;
}
//----------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIBackDecorated::ResetPanel( INT tabIdx )
{
	switch( tabIdx )
	{
	case EBackDecorate_MakeUp:
		{
			ResetMakeUp();

			GrayAllInBagNoNeedForMakeUp( FALSE );
		}
		break;
	case EBackDecorate_Upgrade:
		{
			ResetUpgrade();

			GrayAllInBagNoNeedForUpgrade( FALSE );
		}
		break;
	default:
		break;
	}
}
//========================================================================================================================================
UBOOL UMMOUIBackDecorated::OnLeftMousePress( class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault ) 
{ 
	if( bDefault ){
		return FALSE;
	}

	return FALSE; 
} 
//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::OnLeftMouseClick( class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault )
{
	if( bDefault ){
		return FALSE;
	}

	return FALSE; 
}
//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::OnRightMouseClick( class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault )
{
	if( bDefault ){
		return FALSE;
	}

	UBOOL rtn = FALSE;
	switch( _CurPanelType )
	{
	case EBackDecorate_MakeUp:
		{
			rtn = OnRightMouseClickMakeUp( mc, SlotType, ContainerID, SlotID, bDefault );
		}
		break;
	case EBackDecorate_Upgrade:
		{
			rtn = OnRightMouseClickUpgrade( mc, SlotType, ContainerID, SlotID, bDefault );
		}
		break;
	default:
		break;
	}

	return rtn;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}

	UBOOL rtn = FALSE;
	//from bag to spirit fusion
	if( srcSlotType == ESPType_Bag ){
		switch( destSlotType )
		{
		case ESPType_BackDecoratedMakeUp:
			{
				rtn = OnDragDropMakeUp( srcSlotMC, destSlotMC, srcSlotType, srcContainerID, srcSlotID, destSlotType, destContainerID, destSlotID );
			}
			break;
		case ESPType_BackDecoratedUpgrade:
			{
				rtn = OnDragDropUpgrade( srcSlotMC, destSlotMC, srcSlotType, srcContainerID, srcSlotID, destSlotType, destContainerID, destSlotID );
			}
			break;
		default:
			{
				rtn = FALSE;
			}
			break;
		}

		return rtn;
	}
	else if( destSlotType == ESPType_Bag ){
		//drag back to bag; since nothing was actually moved, dropping onto the original position is not supported
		switch( srcSlotType )
		{
		case ESPType_BackDecoratedMakeUp:
			{
				rtn = OnDragDropMakeUp( srcSlotMC, destSlotMC, srcSlotType, srcContainerID, srcSlotID, destSlotType, destContainerID, destSlotID );
			}
			break;
		case ESPType_BackDecoratedUpgrade:
			{
				rtn = OnDragDropUpgrade( srcSlotMC, destSlotMC, srcSlotType, srcContainerID, srcSlotID, destSlotType, destContainerID, destSlotID );
			}
			break;
		default:
			{
				rtn = FALSE;
			}
			break;
		}

		return rtn;
	}

	return FALSE;
}
//========================================================================================================================================
//void UMMOUIBackDecorated::PlayerMoneyChange(BYTE MoneyType, int Money, INT OldMoney)
//{
//	if( !IsVisible ){
//		return;
//	}
//
//	switch( _CurPanelType )
//	{
//	case EBackDecorate_MakeUp:
//		{
//			UpdateMakeUpFee();
//		}
//		break;
//	case EBackDecorate_Upgrade:
//		{
//			UpdateUpgradeFee();
//		}
//		break;
//	default:
//		warnf(NAME_Error, TEXT("======!!!!======= [BagSlotChange _CurPanelType is WRONG]") );
//		return;
//	}
//}

//============================================================================================================
void UMMOUIBackDecorated::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back( new UILayoutLeftRight( this, MMOUI::GetUIManager()->Panel_Bag ) );
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIBackDecorated::ASOnTabClick( INT idx )
{
	if( idx < 0 || idx >= EBackDecorate_MAX ){
		warnf( NAME_Error, TEXT("UMMOUIBackDecorated::ASOnTabClick idx[%d] overflow"), idx );
		return;
	}

	if( _CurPanelType == idx ){
		return;
	}

	BackDecoratePanelType oldType = (BackDecoratePanelType)_CurPanelType;
	switch( oldType )
	{
	case EBackDecorate_MakeUp:
		{
			GrayAllInBagNoNeedForMakeUp( FALSE );
		}
		break;
	case EBackDecorate_Upgrade:
		{
			GrayAllInBagNoNeedForUpgrade( FALSE );
		}
		break;
	}

	//
	_CurPanelType = idx;
	switch( _CurPanelType )
	{
	case EBackDecorate_MakeUp:
		{
			GrayAllInBagNoNeedForMakeUp( TRUE );
			UnsetMakeUpItemComsuption();
		}
		break;
	case EBackDecorate_Upgrade:
		{
			GrayAllInBagNoNeedForUpgrade( TRUE );
			UnsetToUpgradeItem();
		}
		break;
	}
}
//===============================================================================
void UMMOUIBackDecorated::ASMakeUpConfirm()
{

}
//===============================================================================
void UMMOUIBackDecorated::ASUpgradeConfirm()
{

}
//========================================================================================================================================
//MakeUp
//========================================================================================================================================
void UMMOUIBackDecorated::UpdateMakeUpDetails()
{
	if( _ConsumptionIDForMakeUp == 0 ){
		eventCallAsSetMakeUpResultItem( TEXT(""), -1, 1 );
		FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, 0 );
		eventCallAsSetMakeUpMoney( mnyStr );

		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	INT inBagSlotIdx = -1;
	FClientItemInfo *pMakeUpConsumptionItemInfo = mmoFacade.FindFirstItemPosInContainer( _ConsumptionIDForMakeUp, E_CONSLOT_DEFAULT, inBagSlotIdx );
	if( pMakeUpConsumptionItemInfo == NULL ){
		return;
	}

	const BackDecoratedMakeUpCfg *pMakeUpCfg = MMOUIBackDecoratedMgr::getSingleton().FindBackDecoratedMakeUpCfg( pMakeUpConsumptionItemInfo->m_nTemplateID );
	if( pMakeUpCfg == NULL ){
		return;
	}

	//crafted back ornament
	UItem *pResultItem = mmoFacade.FindItemTemplate( pMakeUpCfg->_MakedItemID );
	if( pResultItem != NULL ){
		FString iconName = pResultItem->GetIconName();
		INT itemID = pResultItem->ItemId;
		INT itemColor = pResultItem->ItemColor + 1;
		eventCallAsSetMakeUpResultItem( iconName, itemID, itemColor );
	}

	//required money
	FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, pMakeUpCfg->_MakeUpMnyCosts );
	eventCallAsSetMakeUpMoney( mnyStr );
}
//========================================================================================================================================
void UMMOUIBackDecorated::UpdateMakeUpFee()
{
	if( _ConsumptionIDForMakeUp == 0 ){
		FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, 0 );
		eventCallAsSetMakeUpMoney( mnyStr );
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	INT inBagSlotIdx = -1;
	FClientItemInfo *pMakeUpConsumptionItemInfo = mmoFacade.FindFirstItemPosInContainer( _ConsumptionIDForMakeUp, E_CONSLOT_DEFAULT, inBagSlotIdx );
	if( pMakeUpConsumptionItemInfo == NULL ){
		return;
	}

	const BackDecoratedMakeUpCfg *pMakeUpCfg = MMOUIBackDecoratedMgr::getSingleton().FindBackDecoratedMakeUpCfg( pMakeUpConsumptionItemInfo->m_nTemplateID );
	if( pMakeUpCfg == NULL ){
		return;
	}

	//required money
	UINT64 curMoney = mmoFacade.GetPlayerMoney64(  E_MONEYTYPE_02 );
	FString colorStr;
	if( pMakeUpCfg->_MakeUpMnyCosts <= curMoney ){
		colorStr = TEXT("0xFFFFE0A6");
	}
	else{
		colorStr = UITextParser::GetTipsColor( TEXT("AlertColor") );
	}
	FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, pMakeUpCfg->_MakeUpMnyCosts, colorStr );
	eventCallAsSetMakeUpMoney( mnyStr );
}
//========================================================================================================================================
void UMMOUIBackDecorated::BagSlotChangeOnMakeUp( INT bagId, INT slotId, INT itemID )
{
	if( !IsVisible ){
		return;
	}

	if( _ConsumptionIDForMakeUp == 0 ){
		return;
	}

	if( itemID == _ConsumptionIDForMakeUp && _ConsumptionIDForMakeUp != 0 ){
		INT newConID = -1;
		INT newSlotID = -1;
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindFirstItemPos( _ConsumptionIDForMakeUp , &newConID , &newSlotID );
		if( pItemInfo == NULL ){
			UnsetMakeUpItemComsuption();
		}
		else{
			SetMakeUpItemComsuption( newConID, newSlotID );
		}
		
		return;
	}

	//
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( E_CONSLOT_DEFAULT == bagId ){
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( bagId, slotId );
		if( pItemInfo != NULL ){
			UItem *pItemInBag = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
			UBOOL isNeedToGray = IsNeedToGrayForMakeUp( pItemInBag, pItemInfo );
			mmoFacade.GrayItem( ESGMark_BackMakeUp, E_CONSLOT_DEFAULT, slotId, isNeedToGray );
		}
	}
}
//========================================================================================================================================
UBOOL UMMOUIBackDecorated::UnsetMakeUpItemComsuption()
{
	return SetMakeUpItemComsuption( -1, -1 );
}
//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::SetMakeUpItemComsuption( int containerID, int slotID )
{
	if( containerID < 0 || slotID < 0 ){
		_ConsumptionIDForMakeUp = 0;

		GFXPlayer->eventCallSetItemData( _MakeUpItemConsumpution , 0 , TEXT("") , -1, ESPType_BackDecoratedMakeUp, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		eventCallAsSetMakeUpConsumptionInfo( TEXT(""), -1, 1, 0, 0 );

		UpdateMakeUpDetails();
		return  TRUE;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo *pItemInfo = mmoFacade.FindItem( containerID, slotID );
	if( pItemInfo == NULL ){
		if( _ConsumptionIDForMakeUp != 0 ){
			UnsetMakeUpItemComsuption();
		}
		return FALSE;
	}

	const BackDecoratedMakeUpCfg *pMakeUpCfg = MMOUIBackDecoratedMgr::getSingleton().FindBackDecoratedMakeUpCfg( pItemInfo->m_nTemplateID );
	if( pMakeUpCfg == NULL ){
		return FALSE;
	}

	//
	UItem *pConsumptionItem = mmoFacade.FindItemTemplate( pMakeUpCfg->_ConsumptionItemID );
	if( pConsumptionItem != NULL ){
		FString iconName = pConsumptionItem->GetIconName();
		INT itemID = pConsumptionItem->ItemId;
		INT itemColor = pConsumptionItem->ItemColor + 1;
		INT inBagTotalCnt = mmoFacade.GetItemCountInBag( itemID, E_CONSLOT_DEFAULT );

		_ConsumptionIDForMakeUp = pMakeUpCfg->_ConsumptionItemID;
		
		GFXPlayer->eventCallSetItemData( _MakeUpItemConsumpution, 1, mmoFacade.GetItemIconNamePersonalized( pConsumptionItem, pItemInfo ), pConsumptionItem->ItemId, 
			ESPType_BackDecoratedMakeUp, TEXT(""), pConsumptionItem->ItemColor+1, containerID, slotID );
		eventCallAsSetMakeUpConsumptionInfo( iconName, itemID, itemColor, inBagTotalCnt, pMakeUpCfg->_ConsumptionItemCnt );
	}
	else{
		_ConsumptionIDForMakeUp = 0;

		GFXPlayer->eventCallSetItemData( _MakeUpItemConsumpution , 0 , TEXT("") , -1, ESPType_BackDecoratedMakeUp, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		eventCallAsSetMakeUpConsumptionInfo( TEXT(""), -1, 1, 0, 0 );
	}

	//
	UpdateMakeUpDetails();

	return TRUE;
}
//========================================================================================================================================
void UMMOUIBackDecorated::GrayAllInBagNoNeedForMakeUp( UBOOL isToGray )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( Player == NULL ){
		return;
	}

	//
	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	for( UINT j = 0; j < bagCapability; ++j){
		if( isToGray == FALSE ){
			mmoFacade.GrayItem( ESGMark_BackMakeUp, E_CONSLOT_DEFAULT, j, FALSE );
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


		UBOOL isNeedToGray = IsNeedToGrayForMakeUp( pItemInBag, pItemInfo );
		mmoFacade.GrayItem( ESGMark_BackMakeUp, E_CONSLOT_DEFAULT, j, isNeedToGray );
	}
}
//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::IsNeedToGrayForMakeUp( UItem *pItemInBag, const FClientItemInfo *pItemInfo )
{
	if( pItemInBag == NULL || pItemInfo == NULL ){
		return FALSE;
	}

	//
	UBOOL noNeedToGray = FALSE;
	//whether it is a back ornament crafting material
	const BackDecoratedMakeUpCfg *pMakeUpCfg = MMOUIBackDecoratedMgr::getSingleton().FindBackDecoratedMakeUpCfg( pItemInfo->m_nTemplateID );
	noNeedToGray = ( pMakeUpCfg != NULL );

	return (!noNeedToGray);
}

UBOOL UMMOUIBackDecorated::OnRightMouseClickMakeUp( class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault )
{
	if( bDefault ){
		return FALSE;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	//UBOOL rst = FALSE;

	switch( SlotType )
	{
	case ESPType_Bag:
		{
			//put in from bag
			FClientItemInfo *pItemInfo = mmoFacade.FindItem( ContainerID, SlotID );
			if( pItemInfo == NULL ){
				return TRUE;
			}

			//
			ClientActionCondition_SoulEmbedOrUnEmbed checker;

			FClientActionConditionContex contex;
			contex.Item = pItemInfo;

			if( !checker.CheckClientActionCondition(contex) ){
				return TRUE;
			}

			//
			SetMakeUpItemComsuption( ContainerID, SlotID );

			return TRUE;
		}
		break;
	case ESPType_BackDecoratedMakeUp:
		{
			UnsetMakeUpItemComsuption(); 

			return TRUE;
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::OnDragDropMakeUp( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}

	if( srcSlotType == ESPType_Bag && destSlotType == ESPType_BackDecoratedMakeUp ){
		//from bag to spirit fusion
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( srcContainerID, srcSlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		//check whether in a UI (this UI or another operation UI)
		//
		ClientActionCondition_BackDecoratedMakeUp checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		SetMakeUpItemComsuption( srcContainerID, srcSlotID );

		return TRUE;
	}

	if( srcSlotType == ESPType_BackDecoratedMakeUp ){
		if( destSlotType != ESPType_BackDecoratedMakeUp ){
			//INT panelSlotID = (INT)srcSlotMC->GetFloat( "EquipID" );
			UnsetMakeUpItemComsuption();
		}

		return TRUE;
	}

	return FALSE;
}
//========================================================================================================================================
void UMMOUIBackDecorated::ResetMakeUp()
{
	if( _ConsumptionIDForMakeUp != 0 ){
		UnsetMakeUpItemComsuption();
	}
}
//========================================================================================================================================
void UMMOUIBackDecorated::InitMakeUpPanelMc()
{
	UGFxObject *parentPanel = GetObject( TEXT("makeUpPage_mc") );
	if( parentPanel == NULL ){
		return;
	}

	_ItemForMakeUpSlot = parentPanel->GetObject( TEXT("decoration_slot_mc") );
	if( _ItemForMakeUpSlot != NULL ){
		_ItemForMakeUpSlot->SetInt( TEXT("EquipID"), EBackDecorateSlot_MakeUp );
		_ItemForMakeUpSlot->SetInt( TEXT("SlotType"), ESPType_BackDecoratedMakeUp );
		_ItemForMakeUpSlot->SetBool( TEXT("DisableDrag"), TRUE );
	}

	_MakeUpItemConsumpution = parentPanel->GetObject( TEXT("materials_slot_mc") );
	if( _MakeUpItemConsumpution != NULL ){
		_MakeUpItemConsumpution->SetInt( TEXT("EquipID"), EBackDecorateSlot_MakeUpCnpt );
		_MakeUpItemConsumpution->SetInt( TEXT("SlotType"), ESPType_BackDecoratedMakeUp );
		_MakeUpItemConsumpution->SetBool( TEXT("DisableDrag"), FALSE );
	}
}
//========================================================================================================================================
//Upgrade
//========================================================================================================================================
void UMMOUIBackDecorated::UpdateUpgradeDetails()
{
	if( _ItemForUpgrade._ConID < 0 || _ItemForUpgrade._SlotID < 0 ){
		eventCallAsSetUpgradeConsumptionItem( TEXT(""), -1, 1, 0, 0 );
		eventCallAsSetUpgradedResultItem( TEXT(""), -1, 1 );
		FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, 0 );
		eventCallAsSetUpgradeMoney( mnyStr );
		eventCallAsSetUpgradeSuccessRate( 0 );
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo *pBackItemInfo = mmoFacade.FindItem( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID );
	if( pBackItemInfo == NULL ){
		return;
	}

	INT lv = 1;
	const BackDecoratedUpgradeCfg *pUpgradeCfg = MMOUIBackDecoratedMgr::getSingleton().FindUpgradeCfg( pBackItemInfo->m_nTemplateID, lv );
	if( pUpgradeCfg == NULL ){
		return;
	}

	//required materials
	UItem *pConsumptionItem = mmoFacade.FindItemTemplate( pUpgradeCfg->_ConsumptionItemID );
	if( pConsumptionItem != NULL ){
		FString iconName = pConsumptionItem->GetIconName();
		INT itemID = pConsumptionItem->ItemId;
		INT itemColor = pConsumptionItem->ItemColor + 1;
		INT inBagTotalCnt = mmoFacade.GetItemCountInBag( itemID, E_CONSLOT_DEFAULT );
		eventCallAsSetUpgradeConsumptionItem( iconName, itemID, itemColor, inBagTotalCnt, pUpgradeCfg->_ConsumptionItemCnt );
	}

	//upgraded back ornament
	UItem *pResultItem = mmoFacade.FindItemTemplate( pUpgradeCfg->_UpgradedItemID );
	if( pResultItem != NULL ){
		FString iconName = pResultItem->GetIconName();
		INT itemID = pResultItem->ItemId;
		INT itemColor = pResultItem->ItemColor + 1;
		eventCallAsSetUpgradedResultItem( iconName, itemID, itemColor );
	}

	//required money
	FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, pUpgradeCfg->_UpgradeMnyCosts );
	eventCallAsSetUpgradeMoney( mnyStr );

	//success rate
	eventCallAsSetUpgradeSuccessRate( pUpgradeCfg->_SuccessRate );
}
//========================================================================================================================================
void UMMOUIBackDecorated::UpdateUpgradeFee()
{
	if( _ItemForUpgrade._ConID < 0 || _ItemForUpgrade._SlotID < 0 ){
		FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, 0 );
		eventCallAsSetUpgradeMoney( mnyStr );

		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo *pBackItemInfo = mmoFacade.FindItem( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID );
	if( pBackItemInfo == NULL ){
		return;
	}

	INT lv = 1;
	const BackDecoratedUpgradeCfg *pUpgradeCfg = MMOUIBackDecoratedMgr::getSingleton().FindUpgradeCfg( pBackItemInfo->m_nTemplateID, lv );
	if( pUpgradeCfg == NULL ){
		return;
	}

	//required money
	UINT64 curMoney = mmoFacade.GetPlayerMoney64(  E_MONEYTYPE_02 );
	FString colorStr;
	if( pUpgradeCfg->_UpgradeMnyCosts <= curMoney ){
		colorStr = TEXT("0xFFFFE0A6");
	}
	else{
		colorStr = UITextParser::GetTipsColor( TEXT("AlertColor") );
	}
	FString mnyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, pUpgradeCfg->_UpgradeMnyCosts, colorStr );
	eventCallAsSetUpgradeMoney( mnyStr );
}
//========================================================================================================================================
void UMMOUIBackDecorated::BagSlotChangeOnUpgrade( INT bagId, INT slotId, INT itemID )
{
	if( !IsVisible ){
		return;
	}

	if( _ItemForUpgrade._ConID < 0 || _ItemForUpgrade._SlotID < 0 ){
		return;
	}
	
	if( bagId == _ItemForUpgrade._ConID && slotId == _ItemForUpgrade._SlotID ){
		SetToUpgradeItem( bagId, slotId );
		return;
	}

	FClientItemInfo *pCurUpgradeItemInfo = MMOFacade::getSingleton().FindItem( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID );
	if( pCurUpgradeItemInfo == NULL ){
		UnsetToUpgradeItem();
		return;
	}

	INT lv = 1;
	const BackDecoratedUpgradeCfg *pUpgradeCfg = MMOUIBackDecoratedMgr::getSingleton().FindUpgradeCfg( pCurUpgradeItemInfo->m_nTemplateID, lv );
	if( pUpgradeCfg != NULL && pUpgradeCfg->_ConsumptionItemID == itemID ){
		UItem *pConsumptionItem = MMOFacade::getSingleton().FindItemTemplate( pUpgradeCfg->_ConsumptionItemID );
		if( pConsumptionItem != NULL ){
			FString iconName = pConsumptionItem->GetIconName();
			INT itemID = pConsumptionItem->ItemId;
			INT itemColor = pConsumptionItem->ItemColor + 1;
			INT inBagTotalCnt = MMOFacade::getSingleton().GetItemCountInBag( itemID, E_CONSLOT_DEFAULT );
			eventCallAsSetUpgradeConsumptionItem( iconName, itemID, itemColor, inBagTotalCnt, pUpgradeCfg->_ConsumptionItemCnt );
		}
		return;
	}

	//
	if( E_CONSLOT_DEFAULT == bagId ){
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
		if( pItemInfo != NULL ){
			UItem *pItemInBag = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
			UBOOL isNeedToGray = IsNeedToGrayForUpgrade( pItemInBag, pItemInfo );
			MMOFacade::getSingleton().GrayItem( ESGMark_BackUpgrade, E_CONSLOT_DEFAULT, slotId, isNeedToGray );
		}
	}
}
//========================================================================================================================================
UBOOL UMMOUIBackDecorated::UnsetToUpgradeItem()
{
	return SetToUpgradeItem( -1, -1 );
}
//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::SetToUpgradeItem( INT containerID, INT slotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	if( containerID < 0 || slotID < 0 ){
		if( _ItemForUpgrade._ConID >= 0 && _ItemForUpgrade._SlotID >= 0 ){

			MMOFacade::getSingleton().LockItem( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID, -1 );

			GFXPlayer->eventCallSetItemData( _ItemForUpgradeSlot , 0 , TEXT("") , -1, ESPType_BackDecoratedUpgrade, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		_ItemForUpgrade._ConID = -1;
		_ItemForUpgrade._SlotID = -1;

		//refresh related display
		UpdateUpgradeDetails();

		return TRUE;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );
	if( pInfo == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") ), MsgInfoType_Table, TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") );
		return FALSE;
	}
	UItem *pItem = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );

	//whether it is an upgradable back ornament
	INT lv = 1;
	const BackDecoratedUpgradeCfg *upgradeCfg = MMOUIBackDecoratedMgr::getSingleton().FindUpgradeCfg( pInfo->m_nTemplateID, lv );
	if( upgradeCfg == NULL || pItem == NULL ){
		FString errStrName = TEXT("MMOGame.BackDecorated.err_Not_UpgradeItem");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrName ), MsgInfoType_Table, errStrName );
		return FALSE;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( _ItemForUpgrade._ConID >= 0 && _ItemForUpgrade._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID, -1 );
	}

	_ItemForUpgrade._ConID = containerID;
	_ItemForUpgrade._SlotID = slotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( _ItemForUpgradeSlot, 1, mmoFacade.GetItemIconNamePersonalized( pItem, pInfo ), pItem->ItemId, 
		ESPType_BackDecoratedUpgrade, TEXT(""), pItem->ItemColor+1, _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID );

	//lock
	MMOFacade::getSingleton().LockItem( _ItemForUpgrade._ConID, _ItemForUpgrade._SlotID, ESPType_BackDecoratedUpgrade );

	//refresh related display
	UpdateUpgradeDetails();

	return TRUE;
}
//========================================================================================================================================
void UMMOUIBackDecorated::GrayAllInBagNoNeedForUpgrade( UBOOL isToGray )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( Player == NULL ){
		return;
	}

	//
	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	for( UINT j = 0; j < bagCapability; ++j){
		if( isToGray == FALSE ){
			mmoFacade.GrayItem( ESGMark_BackUpgrade, E_CONSLOT_DEFAULT, j, FALSE );
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

		UBOOL isNeedToGray = IsNeedToGrayForUpgrade( pItemInBag, pItemInfo );
		mmoFacade.GrayItem( ESGMark_BackUpgrade, E_CONSLOT_DEFAULT, j, isNeedToGray );
	}
}
//----------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::IsNeedToGrayForUpgrade( UItem *pItemInBag, const FClientItemInfo *pItemInfo )
{
	if( pItemInBag == NULL || pItemInfo == NULL ){
		return FALSE;
	}

	//
	UBOOL noNeedToGray = FALSE;
	//whether it is a back ornament crafting material
	const BackDecoratedMakeUpCfg *pMakeUpCfg = MMOUIBackDecoratedMgr::getSingleton().FindBackDecoratedMakeUpCfg( pItemInfo->m_nTemplateID );
	noNeedToGray = ( pMakeUpCfg != NULL );

	return (!noNeedToGray);
}
//========================================================================================================================================
UBOOL UMMOUIBackDecorated::OnRightMouseClickUpgrade( class UGFxObject* mc, BYTE SlotType, BYTE ContainerID, INT SlotID, UBOOL bDefault )
{
	if( bDefault ){
		return FALSE;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	//UBOOL rst = FALSE;

	switch( SlotType )
	{
	case ESPType_Bag:
		{
			//put in from bag
			FClientItemInfo *pItemInfo = mmoFacade.FindItem( ContainerID, SlotID );
			if( pItemInfo == NULL ){
				return TRUE;
			}

			//
			ClientActionCondition_SoulEmbedOrUnEmbed checker;

			FClientActionConditionContex contex;
			contex.Item = pItemInfo;

			if( !checker.CheckClientActionCondition(contex) ){
				return TRUE;
			}

			//
			SetToUpgradeItem( ContainerID, SlotID );

			return TRUE;
		}
		break;
	case ESPType_BackDecoratedUpgrade:
		{
			//INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") ); 
			UnsetToUpgradeItem();

			return TRUE;
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
UBOOL UMMOUIBackDecorated::OnDragDropUpgrade( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}

	if( srcSlotType == ESPType_Bag && destSlotType == ESPType_BackDecoratedUpgrade ){
		//from bag to spirit fusion
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( srcContainerID, srcSlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		//
		ClientActionCondition_BackDecoratedUpgrade checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		//INT panelSlotID = (INT)destSlotMC->GetFloat( TEXT("EquipID") );
		SetToUpgradeItem( srcContainerID, srcSlotID );
		return TRUE;
	}

	if( srcSlotType == ESPType_BackDecoratedUpgrade ){
		if( destSlotType != ESPType_BackDecoratedUpgrade ){
			//INT panelSlotID = (INT)srcSlotMC->GetFloat( "EquipID" );
			UnsetToUpgradeItem();
		}

		return TRUE;
	}

	return FALSE;
}
//========================================================================================================================================
void UMMOUIBackDecorated::ResetUpgrade()
{
	if( _ItemForUpgrade._ConID >= 0 && _ItemForUpgrade._SlotID >= 0 ){
		UnsetToUpgradeItem();
	}
}
//============================================================================================================
void UMMOUIBackDecorated::InitUpgradePanelMc()
{
	UGFxObject *parentPanel = GetObject( TEXT("upgradePage_mc") );
	if( parentPanel == NULL ){
		return;
	}

	_ItemForUpgradeSlot = parentPanel->GetObject( TEXT("decoration_slot_mc") );
	if( _ItemForUpgradeSlot != NULL ){
		_ItemForUpgradeSlot->SetInt( TEXT("EquipID"), EBackDecorateSlot_Upgrade );
		_ItemForUpgradeSlot->SetInt( TEXT("SlotType"), ESPType_BackDecoratedUpgrade );
		_ItemForUpgradeSlot->SetBool( TEXT("DisableDrag"), FALSE );
	}

	_UpgradeItemConsumpution = parentPanel->GetObject( TEXT("materials_slot_mc") );
	if( _UpgradeItemConsumpution != NULL ){
		_UpgradeItemConsumpution->SetInt( TEXT("EquipID"), EBackDecorateSlot_UpgradeCnpt );
		_UpgradeItemConsumpution->SetInt( TEXT("SlotType"), ESPType_BackDecoratedUpgrade );
		_UpgradeItemConsumpution->SetBool( TEXT("DisableDrag"), TRUE );
	}

	_UpgradedItemSlot = parentPanel->GetObject( TEXT("upgrade_decoration_slot_mc") );
	if( _UpgradedItemSlot != NULL ){
		_UpgradedItemSlot->SetInt( TEXT("EquipID"), EBackDecorateSlot_UpgradedItem );
		_UpgradedItemSlot->SetInt( TEXT("SlotType"), ESPType_BackDecoratedUpgrade );
		_UpgradedItemSlot->SetBool( TEXT("DisableDrag"), TRUE );
	}
}
//============================================================================================================
void UMMOUIBackDecorated::Init()
{
	Super::Init();

	//
	InitMakeUpPanelMc();
	InitUpgradePanelMc();

	SetTitleName();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_WeaponSkillLevelUp);
	//evts.AddItem(GameEvent_WeaponSkillReplace);
	evts.AddItem(GameEvent_Item_Delete);
	EventAdapter = new TGameEventAdapter<UMMOUIBackDecorated>(this, evts);
}
//============================================================================================================
void UMMOUIBackDecorated::SetTitleName()
{
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;

	switch( _CurPanelType )
	{
	case EBackDecorate_MakeUp:
		{
			asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BackDecorated.UITitleNameMakeUp") );
		}
		break;
	case EBackDecorate_Upgrade:
		{
			asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BackDecorated.UITitleNameUpgrade") );
		}
		break;
	default:
		{
			asTitleName.S = TEXT("");
		}
		break;
	}

	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
}
