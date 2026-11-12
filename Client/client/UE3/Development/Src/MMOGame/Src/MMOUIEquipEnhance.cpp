#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOEquipManager.h"
#include "MMOChatManager.h"


IMPLEMENT_CLASS(UMMOUIEquipEnhance)

void UMMOUIEquipEnhance::BagSlotChange(INT BagId,INT SlotId)
{
	if( !IsVisible ){
		return;
	}

	//update enhance slot
	if( BagId == CurContainOfTargetEquip() && SlotId == CurBagIdxOfTargetEquip() ){
		UEquip* pCurWorkingEquip = GetBagEquip( BagId, SlotId );
		if( pCurWorkingEquip == NULL ){
			return;
		}
		//Refresh item slot
		SetWorkingSlotData( CurContainOfTargetEquip(), SlotId, pCurWorkingEquip );
		//Update UI
		UpdatePanel();
		//SetOkBtn();
	}
	//update enhance transform target equip slot
	else if( BagId == CurContainOfTransTargetEquip() && SlotId == CurBagIdxOfTransTargetEquip() ){

	}

	//
	if( BagId != E_CONSLOT_DEFAULT ){
		return;
	}
	//
	ItemInfo* pConsumeItemInfo = NULL;
	pConsumeItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_DEFAULT, SlotId );
	//pItemInfo = MMOFacade::getSingleton().FindItem( CONSLOT_DEFAULT, SlotId );
	if( pConsumeItemInfo == NULL ){
		return;
	}

	//enhance consumption
	INT itemBagIdx = CurBagIdxOfTargetEquip();
	const EnhanceData *pEnhanceData = MMOEquipManager::getSingleton().GetEnhanceConsumptionInEquipBag( itemBagIdx );
	if( pEnhanceData == NULL ){
		return;
	}

	//recheck the material count
	UGFxObject *pTfCnt = NULL;
	UGFxObject *pTfCurHave = NULL;
	INT iNewItemID = pConsumeItemInfo->m_nTemplateID;
	//find the material and update the ammount have
	if( EnumCurEnhanceMode == EEQPEHC_MakeEnhance ){
		IsMatOK = TRUE;

		for( INT i = 0; i < ENHANCE_ITEM_CONSUME; ++i ){
			INT needMatID = pEnhanceData->_EnhanceNeed[i]._Type;
			INT needMatCnt = pEnhanceData->_EnhanceNeed[i]._Value;

			pTfCnt = MakeEnhanceMatCsm_mc[i]->GetObject("tf_cnt");
			pTfCurHave = MakeEnhanceMatCsm_mc[i]->GetObject("tf_curHave");
			INT curHaveCnt = UpdateMaterialCnt( pTfCurHave, pTfCnt, needMatID, needMatCnt );
			//ReChkMatCondtionByUI();

			IsMatOK &= ( curHaveCnt >= needMatCnt );
		}//end of for
	}

	//update button state
	//SetOkBtn();
}
//
void UMMOUIEquipEnhance::ReChkMatCondtionByUI()
{
	UGFxObject *pTfCnt;
	UGFxObject *pTfCurHave;
	INT iCur;
	INT iNeed;
	FString strCnt;
	IsMatOK = TRUE;
	for( INT i = 0; i < ENHANCE_MAT_ICON_MAX; ++i){
		pTfCnt = MakeEnhanceMatCsm_mc[i]->GetObject("tf_cnt");
		strCnt = pTfCnt->GetText();
		strCnt = strCnt.Right( 1 );
		iNeed = appAtoi( *strCnt );
		//
		pTfCurHave = MakeEnhanceMatCsm_mc[i]->GetObject("tf_curHave");
		iCur = appAtoi( *pTfCurHave->GetText() );
		IsMatOK &= (iCur >= iNeed);
	}
}

//========================================================================================================================================
void UMMOUIEquipEnhance::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if( !IsVisible ){
		return;
	}
	//update text color
	UpdateFee();
	//enable/disable button
	//SetOkBtn();
}
//========================================================================================================================================
//
UBOOL UMMOUIEquipEnhance::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIEquipEnhance::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIEquipEnhance::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if( SlotType == ESPType_Bag ){
		//put in from bag
		ItemInfo *pInfo = MMOFacade::getSingleton().FindItem( ContainerID, SlotID );
		if( pInfo == NULL ){
			return TRUE;
		}
		//
		UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( pInfo->m_nTemplateID );
		if( pItem == NULL ){
			return TRUE;
		}
		//
		if( EnumCurEnhanceMode == EEQPEHC_MakeEnhance ){
			//Enhance equipment slot
			if( ContainerID == E_CONSLOT_DEFAULT || ContainerID == E_CONSLOT_HERO_EQUIPMENT ){
				SetWorkingSlotData( ContainerID, SlotID, pItem );
				UpdatePanel();

				return TRUE;
			}
			else{
				FString errorhint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError1");

				MMOChatManager::getSingleton().AddSystemMessage(  errorhint );
				MMOUI::GetUIManager()->delegateAddScreenMessage( errorhint ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.EnhcError1"));
			}
		}
	}
	else if( SlotType == ESPType_Enhance ){
		INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") ); 
		//put back to bag
		switch( panelSlotID )
		{
		case EEHCSLT_MakeEnhanceSlot:
		case EEHCSLT_TransEnhanceSlot:
			{
				ResetPanelInfo();
			}
			break;
		case EEHCSLT_TransTargetSlot:
			{
				SetTransSlotTarget( -1, -1, NULL );
			}
			break;
		}

		return TRUE;
	}

	return FALSE;
}
//
UBOOL UMMOUIEquipEnhance::OnDragDrop( class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,
									  BYTE SrcSlotType, BYTE SrcContainerID, INT SrcSlotID,
									  BYTE DestSlotType, BYTE DestContainerID, INT DestSlotID)
{
	//From bag to enhance
	if( (SrcSlotType == ESPType_Bag) && (DestSlotType == ESPType_Enhance) ){
		//put in equip from bag
		ItemInfo *pInfo = MMOFacade::getSingleton().FindItem( SrcContainerID, SrcSlotID );
		if( pInfo == NULL ){
			return TRUE;
		}
		//
		UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( pInfo->m_nTemplateID );
		if( pItem == NULL ){
			return TRUE;
		}
		//
		INT panelSlotID = (INT)DestSlotMC->GetFloat( TEXT("EquipID") ); 
		switch( panelSlotID )
		{
		case EEHCSLT_MakeEnhanceSlot:
		case EEHCSLT_TransEnhanceSlot:
			{
				if( SrcContainerID == E_CONSLOT_DEFAULT || SrcContainerID == E_CONSLOT_HERO_EQUIPMENT ){
					SetWorkingSlotData( SrcContainerID, SrcSlotID, pItem );
					UpdatePanel();
				}
				else{
					FString errorhint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError1");

					MMOChatManager::getSingleton().AddSystemMessage(  errorhint );
					MMOUI::GetUIManager()->delegateAddScreenMessage( errorhint ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.EnhcError1"));
				}
			}
			break;

		case EEHCSLT_TransTargetSlot:
			{
				FString errorhint;
				if( SrcContainerID == E_CONSLOT_DEFAULT || SrcContainerID == E_CONSLOT_HERO_EQUIPMENT ){
					if( CheckCanTransEnhance( SrcContainerID, SrcSlotID ) == TRUE ){
						SetTransSlotTarget( SrcContainerID, SrcSlotID, pItem );
						//
						//CheckOperation();
					}
					else{//cannot put other items
						errorhint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError0");
						MMOUI::GetUIManager()->delegateAddScreenMessage( errorhint,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.EnhcError0") );
					}
				}
				else{
					errorhint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError2");
					MMOUI::GetUIManager()->delegateAddScreenMessage( errorhint ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.EnhcError2"));
				}
				
				if( !errorhint.IsEmpty() ){
					MMOChatManager::getSingleton().AddSystemMessage(  errorhint );
				}
			}
			break;
		}

		return TRUE;
	}
	else if( (DestSlotType == ESPType_Bag) && (SrcSlotType == ESPType_Enhance) ){
		//drag back to bag; since nothing was actually moved, dropping onto the original position is not supported
		INT panelSlotID = (INT)SrcSlotMC->GetFloat( "EquipID" );

		switch( panelSlotID )
		{
		case EEHCSLT_MakeEnhanceSlot:
		case EEHCSLT_TransEnhanceSlot:
			{
				ResetPanelInfo();
			}
			break;
		case EEHCSLT_TransTargetSlot:
			{
				SetTransSlotTarget( -1, -1, NULL );
			}
			break;
		}

		return TRUE;
	}

	return FALSE;
}
//========================================================================================================================================
//Update
void UMMOUIEquipEnhance::UpdatePanel()
{
	switch( EnumCurEnhanceMode )
	{
	case EEQPEHC_MakeEnhance:
		UpdateMakeEnhanceInfo();
		break;
	case EEQPEHC_TransEnhance:
		UpdateTransEnhanceInfo();
		break;
	}
}
//========================================================================================================================================
void UMMOUIEquipEnhance::UpdateMakeEnhanceInfo()
{
	//Required materials
	UpdateMaterialConsume();
	//Required money
	UpdateFee();
	//Enhance item basic info
	UpdateEquipInfo();
	//
	//CheckOperation();
}

//
void UMMOUIEquipEnhance::UpdateTransEnhanceInfo()
{
	///Required money
	UpdateFee();
	//Enhance item basic info
	UpdateEquipInfo();
	//Enhance transfer needs no materials, so this condition is always true
	IsMatOK = true;
	//
	//CheckOperation();
}
//========================================================================================================================================
/*
@Update enhancement required consumables info
*/
void UMMOUIEquipEnhance::UpdateMaterialConsume()
{
	INT bagIdx = CurBagIdxOfTargetEquip();
	const EnhanceData *pEnhanceData = MMOEquipManager::getSingleton().GetEnhanceConsumptionInEquipBag( bagIdx );
	if( pEnhanceData == NULL ){
		ResetMaterialConsume( 0 );
		IsMatOK = FALSE;
		return;
	}

	//Reset material condition flag
	IsMatOK = true;
	//
	UINT8 matTypeCnt = 0;
	UGFxObject *pTfCnt;
	UGFxObject *pTfCurHave;
	for( INT i = 0; i < ENHANCE_ITEM_CONSUME; ++i){
		INT needMatID = pEnhanceData->_EnhanceNeed[i]._Type;
		INT needMatCnt = pEnhanceData->_EnhanceNeed[i]._Value;

		UItem *pItem = NULL;
		if( needMatID > 0 ){
			pItem = MMOFacade::getSingleton().FindItemTemplate( needMatID );
		}

		//pIconSlot = QHMatCsm_mc[MatIdx]->GetObject("itemSlotIcon");
		GFXPlayer->SetIconShowByItem( MakeEnhanceMatCsm_mc[i], pItem );
		//the ammount of this kind of material currently existing in bag
		pTfCnt = MakeEnhanceMatCsm_mc[i]->GetObject("tf_cnt");
		pTfCurHave = MakeEnhanceMatCsm_mc[i]->GetObject("tf_curHave");

		//
		INT curHaveCnt = UpdateMaterialCnt( pTfCurHave, pTfCnt, needMatID, needMatCnt );

		if( pItem != NULL ){
			IsMatOK &= ( curHaveCnt >= needMatCnt );
			++matTypeCnt;
		}
	}//end of for

	//ResetMaterialConsume( matTypeCnt );
}
/*
@Update material consumption number
@MatCsmNeed of -1 means MatCsmNeed uses the existing value
*/
INT UMMOUIEquipEnhance::UpdateMaterialCnt( UGFxObject *pTfCurHave, UGFxObject *pTfCnt, INT itemID, INT MatCsmNeed )
{
	INT iCurExist = 0;
	UINT uiBagCap = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	ItemInfo* pItemInfo = NULL;
	//
	if( MatCsmNeed < 0 ){
		FString strCnt = pTfCnt->GetText();
		strCnt = strCnt.Right( 1 );
		MatCsmNeed = appAtoi( *strCnt );
	}

	UBOOL showText = ( MatCsmNeed > 0 );
	pTfCurHave->SetVisible( showText );
	pTfCnt->SetVisible( showText );
	if( showText == FALSE ){
		return MatCsmNeed;
	}

	//
	for( UINT j = 0; j < uiBagCap; ++j){
		pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_DEFAULT, j );
		//if found, add the amount
		if( pItemInfo != NULL && pItemInfo->m_nTemplateID == itemID ){
			iCurExist += pItemInfo->m_nCurDur;
		}
	}
	//set the current amount text color
	if( iCurExist < MatCsmNeed ){
		pTfCurHave->SetInt( TEXT("textColor") , 0xFF0000 );//red GFXPlayer->OfflineFontColor
	}
	else{
		pTfCurHave->SetInt( TEXT("textColor") , 0x00FF00 );//green
	}
	//set the current amount and needed amount
	if( iCurExist <= 99 ){
		pTfCurHave->SetText( appItoa( iCurExist ) );
	}
	else{
		pTfCurHave->SetText( TEXT("*") );
	}
	pTfCnt->SetText( FString::Printf( TEXT("/%d"), MatCsmNeed ) );

	return iCurExist;
}
//========================================================================================================================================
//Update enhancement cost
void UMMOUIEquipEnhance::UpdateFee()
{
	//enhance consumption
	const EnhanceData *pEnhanceData = MMOEquipManager::getSingleton().GetEnhanceConsumptionInEquipBag( CurBagIdxOfTargetEquip() );

	INT moneyCnt = -1;
	EMONEY_TYPE moneyType = E_MONEYTYPE_01;
	if( pEnhanceData != NULL ){
		if( pEnhanceData->_EnhanceNeedMny1 > 0 ){
			moneyType = E_MONEYTYPE_01;
			moneyCnt = pEnhanceData->_EnhanceNeedMny1;
		}
		else{
			moneyType = E_MONEYTYPE_02;
			moneyCnt = pEnhanceData->_EnhanceNeedMny2;
		}
	}

	//Update condition flags
	IsMnyOK = ( pEnhanceData != NULL && moneyCnt <= Player->GetMoney( moneyType ) );
	//bExpOK = ( iExp <= Player->ActorAttributes->LevelAttributes.CurrentExp );
	//
	FString strMny;
	strMny = ( moneyCnt < 0 ? TEXT("--") : appItoa( moneyCnt ) );

	UINT color = 0xFFFF0000;
	if( EnumCurEnhanceMode == EEQPEHC_MakeEnhance ){
		color = ( IsMnyOK || moneyCnt < 0 ) ? 0xFFFFE0A6 : 0xFFFF0000;
		MakeEnhanceMnyCsm_tf->SetInt( TEXT("textColor"), color );
		MakeEnhanceMnyCsm_tf->SetText( strMny );

		MakeEnhanceMnyType_mc->GotoAndStopI( moneyType );
	}
	else if( EnumCurEnhanceMode == EEQPEHC_TransEnhance ){
		color = ( IsMnyOK || moneyCnt < 0 ) ? 0xFFFFE0A6 : 0xFFFF0000;
		TransEnhanceMnyCsm_tf->SetInt( TEXT("textColor"), color );
		TransEnhanceMnyCsm_tf->SetText( strMny );
	}
}
//========================================================================================================================================
//Update enhance item info
void UMMOUIEquipEnhance::UpdateEquipInfo()
{
	INT bagIdx = CurBagIdxOfTargetEquip();
	INT bagTypeID = CurContainOfTargetEquip();

	ItemInfo* pEnhanceItemInfo = MMOFacade::getSingleton().FindItem( bagTypeID, bagIdx );
	INT curEnhanceLv = pEnhanceItemInfo ? pEnhanceItemInfo->m_wRefineLevel : -1;
	INT curEnhanceMaxLv = -1;

	MMOEquipManager *pEquipMgr = &MMOEquipManager::getSingleton();
	if( EnumCurEnhanceMode == EEQPEHC_MakeEnhance )
	{
		const EnhanceResult *pCurResult = NULL;
		const EnhanceResult *pNextResult = NULL;

		//AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		UEquip* pEquip = GetBagEquip( bagTypeID, bagIdx );

		if( pEquip != NULL && curEnhanceLv >= 0)
		{
			pCurResult = pEquipMgr->GetEnhanceResult( pEquip->ItemSubType, pEquip->ItemSubSubType, curEnhanceLv );
			pNextResult = pEquipMgr->GetEnhanceResult( pEquip->ItemSubType, pEquip->ItemSubSubType, curEnhanceLv+1 );
			curEnhanceMaxLv = pEquip->MaxRefineLevel;
		}

		//Update condition flags
		IsDegreeOK = ( curEnhanceLv < curEnhanceMaxLv );
		//Enhancement level
		MakeEnhanceCurDgr_tf->SetText( curEnhanceLv >= 0 ? appItoa( curEnhanceLv ) : TEXT("--") );
		MakeEnhanceMaxDgr_tf->SetText( curEnhanceMaxLv >= 0 ? appItoa( curEnhanceMaxLv ) : TEXT("--") );

		//enhancement attributes
		//FString strLookUpKey = TEXT("MMOGame.attribute.attribute");
		//FString strPropValuePrefix = TEXT("+");
		INT curPropIdx = 0;
		INT nextPropIdx = 0;
		for( INT i = 0; i < ENHANCE_EQUIP_PROPERTY; ++i )
		{
			//Current enhancement attribute bonus
			if( NULL != pCurResult )
			{
				const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( pCurResult->_Prop[i] );
				if(NULL != propAttr)
				{
					if(curPropIdx < ENHANCE_PROP_MAX )
					{
						CurEnhanceProp_tf[curPropIdx]->SetVisible( TRUE );
						CurEnhanceProp_tf[curPropIdx]->SetText( propAttr->strText );

						++curPropIdx;
					}
				}
			}

			//Attribute bonus after enhancement
			if( NULL != pNextResult )
			{
				const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( pNextResult->_Prop[i] );
				if(NULL != propAttr)
				{
					if(nextPropIdx < ENHANCE_PROP_MAX  )
					{
						MakeEnhanceProp_tf[nextPropIdx]->SetVisible( TRUE );
						MakeEnhanceProp_tf[nextPropIdx]->SetText( propAttr->strText + TEXT(" ") );
						++nextPropIdx;
					}
				}
			}
			
		}

		for( ; curPropIdx < ENHANCE_PROP_MAX; ++curPropIdx )
		{
			CurEnhanceProp_tf[curPropIdx]->SetVisible( FALSE );
			//CurEnhancePropValue_tf[curPropIdx]->SetVisible( FALSE );
		}
		//
		for( ; nextPropIdx < ENHANCE_PROP_MAX; ++nextPropIdx )
		{
			MakeEnhanceProp_tf[nextPropIdx]->SetVisible( FALSE );
			//MakeEnhancePropValue_tf[nextPropIdx]->SetVisible( FALSE );
		}
	}
	else if( EnumCurEnhanceMode == EEQPEHC_TransEnhance )
	{
		//Update condition flags
		IsDegreeOK = TRUE;

		//Current enhancement level
		TransEnhanceCurDgr_tf->SetText( curEnhanceLv >= 0 ? appItoa( curEnhanceLv ) : TEXT("--") );
	}
}
//========================================================================================================================================
//
void UMMOUIEquipEnhance::ResetMaterialConsume( INT startIdx )
{
	//UGFxObject *pIconSlot;
	UGFxObject *pTfCnt;
	UGFxObject *pTfCurHave;

	for( UINT8 i = startIdx; i < ENHANCE_MAT_ICON_MAX; ++i){
		//pIconSlot = QHMatCsm_mc[i]->GetObject("ic_slot");
		GFXPlayer->SetIconShowByItem( MakeEnhanceMatCsm_mc[i], NULL );
		//
		pTfCnt = MakeEnhanceMatCsm_mc[i]->GetObject("tf_cnt");
		pTfCnt->SetText("");
		//
		pTfCurHave = MakeEnhanceMatCsm_mc[i]->GetObject("tf_curHave");
		pTfCurHave->SetText( "" );
	}
}
//========================================================================================================================================
//Check whether the operation can be performed
UBOOL UMMOUIEquipEnhance::CheckCanGo()
{
	UBOOL bRet = IsMatOK && IsMnyOK && IsDegreeOK;
	if( EnumCurEnhanceMode == EEQPEHC_TransEnhance ){
		bRet &= IsTransToItemOK;
	}
	return bRet;
}

//Check whether it is the item currently used by this UI
UBOOL UMMOUIEquipEnhance::CheckInUse( INT ItemInBagID )
{
	if( IsVisible && (ItemInBagID == CurBagIdxOfTargetEquip() || ItemInBagID == CurBagIdxOfTransTargetEquip()) ){
		return true;
	}
	return false;
}

UBOOL UMMOUIEquipEnhance::CheckCanTransEnhance( INT bagSlotCon, INT bagSlotIdx )
{
	//AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();

	INT slotIdx = CurBagIdxOfTargetEquip();
	INT slotType = CurContainOfTargetEquip();
	UEquip* pEnhanceFromEquip = GetBagEquip( slotType, slotIdx );
	if( pEnhanceFromEquip == NULL ){
		return FALSE;
	}

	UEquip* pCheckEquip = GetBagEquip( bagSlotCon, bagSlotIdx );
	if( pCheckEquip == NULL ){
		return FALSE;
	}

	return (pEnhanceFromEquip->MaxRefineLevel >= pCheckEquip->MaxRefineLevel);
}
//========================================================================================================================================
//
INT UMMOUIEquipEnhance::CurBagIdxOfTargetEquip()
{
	return CurWorkingItemBagID;
}

//
INT UMMOUIEquipEnhance::CurBagIdxOfTransTargetEquip()
{
	return CurTransEnhanceToItemBagID;
}

INT UMMOUIEquipEnhance::CurContainOfTargetEquip()
{
	return CurWorkingItemContainID;
}

//
INT UMMOUIEquipEnhance::CurContainOfTransTargetEquip()
{
	return CurTransEnhanceToItemContainID;
}

//========================================================================================================================================
UEquip *UMMOUIEquipEnhance::GetBagEquip( int bagSlotCon, int bagSlotID )
{
	UItem *pItem = NULL;
	UEquip* pEquip = NULL;
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( bagSlotCon == E_CONSLOT_DEFAULT ){
		if( pMainPlayer->ItemManager->BagItemList.IsValidIndex( bagSlotID ) ){
			pItem = pMainPlayer->ItemManager->BagItemList( bagSlotID );
		}	
	}
	else if( bagSlotCon == E_CONSLOT_HERO_EQUIPMENT ){
		if( pMainPlayer->ItemManager->BagHeroList.IsValidIndex( bagSlotID ) ){
			pItem = pMainPlayer->ItemManager->BagHeroList( bagSlotID );
		}
	}

	if( pItem != NULL ){
		pEquip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( pItem->ItemId ) );
	}
	
	return pEquip;
}

//========================================================================================================================================
//
void UMMOUIEquipEnhance::MakeEnhanceItem()
{
	UINT16 pakageSlotID = (UINT16)CurBagIdxOfTargetEquip();
	UINT16 pakageSlotType = (UINT16)CurContainOfTargetEquip();

	FString errorHint;
	FString errorString;
	switch( EnumCurEnhanceMode )
	{
	case EEQPEHC_MakeEnhance:
		{
			INT bagIdx = CurBagIdxOfTargetEquip();
			INT bagTypeID = CurContainOfTargetEquip();
			if( bagIdx < 0 || bagTypeID < 0 ){
				errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError7");
				errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError7");
				break;
			}
			//full level
			const EnhanceData *pEnhanceData = MMOEquipManager::getSingleton().GetEnhanceConsumptionInEquipBag( bagIdx );
			if( pEnhanceData == NULL ){
				errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError6");
				errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError6");
				break;
			}
			//
			if( !IsMatOK ){
				errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError3");
				errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError3");
				break;
			}
			//
			if( !IsMnyOK ){
				if( pEnhanceData->_EnhanceNeedMny1 > 0 ){
					errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError4");
					errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError4");
				}
				else{
					errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError5");
					errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError5");
				}

				break;
			}
			//
			if( !IsDegreeOK ){
				errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError6");
				errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError6");
				break;
			}
		}
		break;

	case EEQPEHC_TransEnhance:
		{
			INT transItemBagIdx = CurBagIdxOfTargetEquip();
			INT transItemBagTypeID = CurContainOfTargetEquip();
			if( transItemBagIdx < 0 || transItemBagTypeID < 0 ){
				errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError8");
				errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError8");
			}
			//
			INT transTargetBagIdx = CurBagIdxOfTransTargetEquip();
			INT transTargetBagTypeID = CurContainOfTransTargetEquip();
			if( transTargetBagIdx < 0 || transTargetBagTypeID < 0 ){
				errorHint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcError9");
				errorString = TEXT("MMOGame.EnhanceGeneral.EnhcError9");
			}
		}
		break;
	}
	//
	if( !errorHint.IsEmpty() ){
		MMOChatManager::getSingleton().AddSystemMessage(  errorHint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( errorHint ,MsgInfoType_Table,errorString);

		return;
	}

	//CMessageHandler::GetInstance().Send_Refine( pakageSlotType, pakageSlotID );
}

//
void UMMOUIEquipEnhance::NotifyMakeEnhanceItemResult( UBOOL isSucceed )
{
	FString hint;
	if( isSucceed ){
		//INT bagIdx = CurBagIdxOfTargetEquip();
		//INT bagTypeID = CurContainOfTargetEquip();
		//ItemInfo* pEnhanceItemInfo = MMOFacade::getSingleton().FindItem( bagTypeID, bagIdx );
		//UEquip* pEquip = GetBagEquip( bagTypeID, bagIdx );
		//if( pEnhanceItemInfo != NULL && pEquip != NULL ){
		//	FString formatStr = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcGeneral");
		//	hint = FString::Printf( *formatStr, *pEquip->GetDisplayName(), pEnhanceItemInfo->m_wRefineLevel );
		//}
		//animation
		MakeEnhanceSuccessAnim_mc->SetVisible( TRUE );
		MakeEnhanceSuccessAnim_mc->GotoAndPlayI( 1 );

		hint = MMOUI::ParseLocalizedPropertyPath( "MMOGame.EnhanceGeneral.EnhcGeneral");
	}

	if( !hint.IsEmpty() ){
		MMOChatManager::getSingleton().AddSystemMessage(  hint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.EnhcGeneral"));
	}
}

//
void UMMOUIEquipEnhance::TransferEnhanceItem()
{
	//UINT16 PakageSlotID = (UINT16)CurBagIdxOfTargetEquip();

	//CMessageHandler::GetInstance().Send_Unrefine( PakageSlotID, bAll );
}

//========================================================================================================================================
//Put in the equipment to enhance (cancel enhance)
void UMMOUIEquipEnhance::SetWorkingSlotData( INT bagSlotCon, INT bagSlotID, UItem *slotItem )
{
	UGFxObject *slot_mc = NULL;
	
	switch( EnumCurEnhanceMode )
	{
	case EEQPEHC_MakeEnhance:
		slot_mc = MakeEnhanceItem_mc;
		break;
	case EEQPEHC_TransEnhance:
		slot_mc = TransEnhanceFromItem_mc;
		break;
	}
		//if set none
	if( slotItem == NULL || bagSlotID < 0 ){
		bagSlotID = -1;
		slotItem = NULL;
	}
	//unlock it
	//UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	if( CurWorkingItemBagID >= 0){
		MMOFacade::getSingleton().LockItem( CurWorkingItemContainID, CurWorkingItemBagID, -1 );
	}

	CurWorkingItemContainID = bagSlotCon;
	CurWorkingItemBagID = bagSlotID;

	//lock it
	if( CurWorkingItemBagID >= 0 && CurWorkingItemContainID >= 0 ){
		MMOFacade::getSingleton().LockItem( CurWorkingItemContainID, CurWorkingItemBagID, ESPType_Enhance );
	}

	//set UI ItemSlot
	GFXPlayer->SetItemSlotData( slot_mc, slotItem, ESPType_Enhance, CurWorkingItemContainID, CurWorkingItemBagID );
}
//========================================================================================================================================
//
void UMMOUIEquipEnhance::SetTransSlotTarget( INT bagSlotCon, INT bagSlotID, UItem *slotItem )
{
	UGFxObject *slot_mc = NULL;

	//unlock it
	UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	if( CurTransEnhanceToItemBagID >= 0 && pBagPanel != NULL ){
		MMOFacade::getSingleton().LockItem( CurTransEnhanceToItemContainID, CurTransEnhanceToItemBagID, -1 );
	}
	//
	CurTransEnhanceToItemContainID = bagSlotCon;
	CurTransEnhanceToItemBagID = bagSlotID;
	//lock it
	if( CurTransEnhanceToItemBagID >= 0 && pBagPanel != NULL && CurTransEnhanceToItemContainID >= 0 ){
		MMOFacade::getSingleton().LockItem( CurTransEnhanceToItemContainID, CurTransEnhanceToItemBagID, ESPType_Enhance );
	}

	//set UI ItemSlot
	GFXPlayer->SetItemSlotData( slot_mc, slotItem, ESPType_Enhance, CurTransEnhanceToItemContainID, CurTransEnhanceToItemBagID );

	//if in unEnhance panel update the condition mark
	IsTransToItemOK = ( CurTransEnhanceToItemBagID >= 0  && CurTransEnhanceToItemContainID >= 0 );
}
//========================================================================================================================================
//
void UMMOUIEquipEnhance::PostOnShow(UBOOL v)
{
	Super::PostOnShow( v );

	if( v ){
		//!!!!!!!!!! This is deprecated, do not display
		Show( FALSE );

		//show the default enhance panel
		ResetUIMode();
		//reset all working condition
		ResetCondition();
	}
	else{
		//clear items putted in this panel
		ResetEnhanceItems();
	}
}
//========================================================================================================================================
//
void UMMOUIEquipEnhance::ShowDefaultContent( int mode)
{
	switch( mode )
	{
	case 0:
		EnumCurEnhanceMode = EEQPEHC_MakeEnhance;
		break;
	case 1:
		EnumCurEnhanceMode = EEQPEHC_TransEnhance;
		break;
	}
	ResetPanelInfo();
}
//========================================================================================================================================
//
void UMMOUIEquipEnhance::ResetUIMode()
{
	EnumCurEnhanceMode = EEQPEHC_MakeEnhance;
	//
	ResetPanelInfo();
	//
	eventChangeContentTabBtn( 0 );
}

//Reset current UI display content
void UMMOUIEquipEnhance::ResetPanelInfo()
{
	switch( EnumCurEnhanceMode )
	{
	case EEQPEHC_MakeEnhance:
		ResetMakeEnhanceInfo();
		break;
	case EEQPEHC_TransEnhance:
		ResetTransEnhanceInfo();
		break;
	}
}

//
void UMMOUIEquipEnhance::ResetMakeEnhanceInfo()
{
	SetWorkingSlotData( -1, -1, NULL );

	//Update must compute cost and other data after the item is Set, so update* must be called afterwards
	UpdateFee();
	UpdateEquipInfo();
	ResetMaterialConsume( 0 );
	//
	//CheckOperation();
}

//
void UMMOUIEquipEnhance::ResetTransEnhanceInfo()
{
	SetWorkingSlotData( -1, -1, NULL );
	SetTransSlotTarget( -1, -1, NULL );
	//Update must compute cost and other data after the item is Set, so update* must be called afterwards
	UpdateFee();
	UpdateEquipInfo();
	//
	//CheckOperation();
}

//
void UMMOUIEquipEnhance::ResetCondition()
{
	IsMatOK = false;
	IsMnyOK = false;
	IsDegreeOK = false;
	IsTransToItemOK = false;
}

//
//
void UMMOUIEquipEnhance::ResetEnhanceItems()
{
	UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );

	//
	if( CurWorkingItemBagID >= 0 && pBagPanel != NULL ){
		MMOFacade::getSingleton().LockItem( E_CONSLOT_DEFAULT, CurWorkingItemBagID, -1 );
	}
	CurWorkingItemBagID = -1;

	//
	if( CurTransEnhanceToItemBagID >= 0 && pBagPanel != NULL ){
		MMOFacade::getSingleton().LockItem( E_CONSLOT_DEFAULT, CurTransEnhanceToItemBagID, -1 );
	}
	CurTransEnhanceToItemBagID = -1;
}
//========================================================================================================================================
//Last modified
void UMMOUIEquipEnhance::CheckOperation()
{
	//Button state
	SetOkBtn();
}
//
void UMMOUIEquipEnhance::SetOkBtn()
{
	UBOOL isOK;
	isOK = CheckCanGo();
	switch( EnumCurEnhanceMode )
	{
	case EEQPEHC_MakeEnhance:
		MakeEnhance_btn->SetBool( TEXT("enabled"), isOK );
		break;
	case EEQPEHC_TransEnhance:
		TransEnhance_btn->SetBool( TEXT("enabled"), isOK );
		break;
	}
}

//
void UMMOUIEquipEnhance::Init()
{
	Super::Init();

	InitMc();
	//
	EnumCurEnhanceMode = EEQPEHC_MakeEnhance;
}

//
void UMMOUIEquipEnhance::InitMc()
{
	//Enhance UI
	MakeEnhancePanel = GetObject( TEXT("MakeEnhancePanel_mc") );
	if( MakeEnhancePanel != NULL ){	
		MakeEnhanceItem_mc = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceItem_mc") );
		MakeEnhanceItem_mc->SetInt( TEXT("EquipID"), EEHCSLT_MakeEnhanceSlot );
		MakeEnhanceItem_mc->SetInt( TEXT("SlotType"), ESPType_Enhance );
		//animation
		MakeEnhanceSuccessAnim_mc = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceSuccessAnim_mc") );
		//Required material icon
		MakeEnhanceMatCsm_mc[0] = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceMatCsm_mc1") );
		//MakeEnhanceMatCsm_mc[0]->SetFloat( "EquipID", 200 );
		MakeEnhanceMatCsm_mc[0]->SetInt( TEXT("SlotType") , ESPType_Enhance );
		MakeEnhanceMatCsm_mc[1] = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceMatCsm_mc2") );
		//MakeEnhanceMatCsm_mc[1]->SetFloat( "EquipID", 200 );
		MakeEnhanceMatCsm_mc[1]->SetInt( TEXT("SlotType") , ESPType_Enhance );
		MakeEnhanceMatCsm_mc[2] = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceMatCsm_mc3") );
		//MakeEnhanceMatCsm_mc[2]->SetFloat( "EquipID", 200 );
		MakeEnhanceMatCsm_mc[2]->SetInt( TEXT("SlotType") , ESPType_Enhance );
		//Enhance fee
		MakeEnhanceMnyType_mc = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceMnyType_mc") );
		MakeEnhanceMnyCsm_tf = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceMnyCsm_tf") );
		//Current enhancement level
		MakeEnhanceCurDgr_tf = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceCurDgr_tf") );
		//Max enhance level
		MakeEnhanceMaxDgr_tf = MakeEnhancePanel->GetObject( TEXT("MakeEnhanceMaxDgr_tf") );
		//
		for( INT i = 0; i < ENHANCE_PROP_MAX; ++i )
		{
			FString obName = FString::Printf( TEXT("CurEnhanceProp_tf%d" ), i );
			CurEnhanceProp_tf[i] = MakeEnhancePanel->GetObject( obName );
			//CurEnhancePropValue_tf[i] = MakeEnhancePanel->GetObject("CurEnhancePropValue_tf"$i);
			obName = FString::Printf( TEXT("MakeEnhanceProp_tf%d" ), i );
			MakeEnhanceProp_tf[i] = MakeEnhancePanel->GetObject( obName );
			//MakeEnhancePropValue_tf[i] = MakeEnhancePanel->GetObject("MakeEnhancePropValue_tf"$i);
		}
		//Enhance button
		MakeEnhance_btn = MakeEnhancePanel->GetObject( TEXT("MakeEnhance_btn") );
	}

	//Enhance conversion UI
	TransEnhancePanel = GetObject( TEXT("TransEnhancePanel_mc") );
	if( TransEnhancePanel != NULL ){
		//Weapon to be converted
		TransEnhanceFromItem_mc = TransEnhancePanel->GetObject( TEXT("TransEnhanceFromItem_mc") );
		TransEnhanceFromItem_mc->SetInt( TEXT("EquipID"), EEHCSLT_TransEnhanceSlot );
		TransEnhanceFromItem_mc->SetInt( TEXT("SlotType"), ESPType_Enhance );
		//Equipment that needs the enhancement applied
		TransEnhanceToItem_mc = TransEnhancePanel->GetObject( TEXT("TransEnhanceToItem_mc") );
		TransEnhanceToItem_mc->SetInt( TEXT("EquipID"), EEHCSLT_TransTargetSlot );
		TransEnhanceToItem_mc->SetInt( TEXT("SlotType"), ESPType_Enhance );
		//Remove enhance fee
		TransEnhanceMnyCsm_tf = TransEnhancePanel->GetObject( TEXT("TransEnhanceMnyCsm_tf") );
		//Current enhancement level
		TransEnhanceCurDgr_tf = TransEnhancePanel->GetObject( TEXT("TransEnhanceCurDgr_tf") );
		//Transfer enhance button
		TransEnhance_btn = TransEnhancePanel->GetObject( TEXT("TransEnhance_btn") );
	}
}