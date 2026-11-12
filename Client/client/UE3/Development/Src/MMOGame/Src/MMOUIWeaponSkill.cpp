#include "MMOFacade.h"
#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "net/message_handler.h"
#include "ClientActionConditionCheck.h"
#include "MMOEquipManager.h"
#include "MMOSkillManager.h"
#include "MMOMsgBoxManager.h"
#include "item_def.h"
#include "UITextParser.h"
#include "MMOChatManager.h"
#include "UILayout.h"
#include "MMOGuideManager.h"
#include "MMOQuestManager.h"
#include "money_def.h"
#include "UITextParser.h"
#include "ScreenMsgQueue.h"
#include "MMOClientVariableManager.h"
#include "MMOGuildData.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIWeaponSkill)

void UMMOUIWeaponSkill::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowWeaponSkillPanel);
		MMOUI::PostGameEvent(&evtGuide);
		////Cannot be opened before level 10
		//if( Player && Player->ActorAttributes ){
		//	if( Player->ActorAttributes->LevelAttributes.Level < 10 ){
		//		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SkillTree.CanNotOpenHint")),MsgInfoType_Table,TEXT("MMOGame.SkillTree.CanNotOpenHint"));
		//		Show( FALSE );
		//		return;
		//	}
		//}

		_IsAutoChangeSkill = MMOEquipManager::getSingleton().IsAutoChangeSkills();
		//_AutoChangeSkillChkBtn->SetBool( TEXT("selected"), _IsAutoChangeSkill );

		//UpdateConsumeItem();
		UpdateFee();

		UBOOL isOnGuildMap = ( MMOMapDataManager::getSingleton().GetCurMap().ServerID == GUILD_TERRITORY_MAP_ID );
		ChangeToGuildMode( isOnGuildMap );
		UpdateGuildNeeds();
		//
		PrePutWeapon();
		//
		GrayAllBagNotEquip( TRUE );

	}
	else{
		ResetUI();
		//
		GrayAllBagNotEquip( FALSE );
		//
		ResetAllPopupBox();
	}
}

//============================================================================================================
void UMMOUIWeaponSkill::ChangeToSealMode( UBOOL isToSet )
{
	_IsSealMode = isToSet;
	//
	if( _ToOpWeapon._ConID >= 0 && _ToOpWeapon._SlotID >= 0 ){
		UpdateCurWeaponSkills();
	}
	//
	GrayAllBagNotEquip( TRUE );
}
//
void UMMOUIWeaponSkill::ChangeToNoOpMode( UBOOL isToSet )
{
	_IsNoOpMode = isToSet;
	//
	//eventUCDisableThisPanelOp( _IsNoOpMode );
	//
	GrayAllBagNotEquip( TRUE );
}
//
void UMMOUIWeaponSkill::ChangeToGuildMode( UBOOL isToSet )
{
	_IsGuildMode = isToSet;
	//
	eventCallASSetIsGuildEdition( _IsGuildMode );
}
//============================================================================================================
void UMMOUIWeaponSkill::ResetUI()
{
	_IsUnlockingSkillSlot = -1;
	ChangeToSealMode( FALSE );
	ChangeToNoOpMode( FALSE );
	//
	UnSetCurWeapon();
	//
	UnSetCurConsumeID();
	////
	//UMMOUIWeaponSkillSetting *pWeaponSkillSettingPanel = (UMMOUIWeaponSkillSetting*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkillSetting );
	//if( pWeaponSkillSettingPanel != NULL && pWeaponSkillSettingPanel->IsVisible ){
	//	pWeaponSkillSettingPanel->ShowWeaponSkillInfoPanel( FALSE, -1, -1, -1 );
	//}
}
//
void UMMOUIWeaponSkill::ResetAllPopupBox()
{
	if( _ConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmBox );
		_ConfirmBox = NULL;
	}
}
//========================================================================================
void UMMOUIWeaponSkill::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_WeaponSkillAdd:
		{
			MMOGameEvent_OneInt *weaponSkillAddEvt = (MMOGameEvent_OneInt*)evt;
			if( IsVisible ){
				NotifyTryWeaponSkillResult( weaponSkillAddEvt->IntData );
			}
		}
		break;
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			if( IsVisible ){
				BagSlotChange( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx );
			}
		}
		break;
	case GameEvent_SkillUiUpdate:
		{
			MMOGameEvent_SkillUiUpdate* skillEvent = (MMOGameEvent_SkillUiUpdate*)evt;
			switch(skillEvent->EventType)
			{
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillSlotCmdExchanged:
				{
					if( IsVisible ){
						UpdateCurWeaponSkills();
					}
				}
				break;
			}
		}
		break;
	case GameEvent_BagSortEnd:
		{
			if( IsVisible ){
				GrayAllBagNotEquip( TRUE );
			}
		}
		break;
	}
}
//========================================================================================================================================
void UMMOUIWeaponSkill::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	//Weapon changed, refresh weapon abilities
	if( _ToOpWeapon._ConID == bagId && _ToOpWeapon._SlotID == slotId ){
		UpdateCurWeaponSkills();
	}

	if( E_CONSLOT_DEFAULT == bagId ){
		MMOFacade &mmoFacade = MMOFacade::getSingleton();

		const FClientItemInfo *pItemInfo = mmoFacade.FindItem( E_CONSLOT_DEFAULT, slotId );
		if( pItemInfo != NULL ){
			//if( IsWeaponSkillConsumeItem( pItemInfo->m_nTemplateID ) ){
			INT curCnsmItemID = GetCurConsumeID();
			if( curCnsmItemID == pItemInfo->m_nTemplateID ){
				UpdateConsumeItem();
			}
			else{
				UItem *pItemInBag = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
				UBOOL isNeedToGray = IsNeedToGray( pItemInBag, pItemInfo );
				mmoFacade.GrayItem( ESGMark_WeaponSkill, E_CONSLOT_DEFAULT, slotId, isNeedToGray );
			}
		}
		else{
			UpdateConsumeItem();
		}
	}
}
//========================================================================================================================================
void UMMOUIWeaponSkill::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if( IsVisible ){
		switch( MoneyType )
		{
		case E_MONEYTYPE_02:	UpdateFee(); break;
		case E_MONEYTYPE_GUILD: UpdateGuildNeeds(); break;
		}
	}
}

//========================================================================================================================================
void UMMOUIWeaponSkill::OnFocusGained()
{
	if( IsVisible ){
		UMMOUIBaseSlotContainer::OnFocusGained();

		//GrayAllBagNotEquip( TRUE );
	}
	else{
		UMMOUIBase::OnFocusGained();
	}
}

//
void UMMOUIWeaponSkill::OnFocusLost()
{
	if( IsVisible ){
		UMMOUIBaseSlotContainer::OnFocusLost();

		//GrayAllBagNotEquip( FALSE );
	}
	else{
		UMMOUIBase::OnFocusLost();
	}
}

//========================================================================================================================================
//
UBOOL UMMOUIWeaponSkill::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIWeaponSkill::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIWeaponSkill::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	if( mc == NULL ){
		return FALSE;
	}
	if( _IsNoOpMode == TRUE ){
		return TRUE;
	}

	if( _IsSealMode == TRUE ){
		return TRUE;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	switch( SlotType )
	{
	case ESPType_Bag:
	case ESPType_Charactor:
		{
			//put in from bag
			FClientItemInfo *pItemInfo = mmoFacade.FindItem( ContainerID, SlotID );
			if( pItemInfo == NULL ){
				return TRUE;
			}

			UItem *pItem = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
			if( pItem == NULL ){
				return TRUE;
			}

			//
			ClientActionCondition_WeaponSkill checker;

			FClientActionConditionContex contex;
			contex.Item = pItemInfo;

			if( !checker.CheckClientActionCondition(contex) ){
				return TRUE;
			}

			//Whether it is a weapon
			if( pItem->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
				SetCurWeapon( ContainerID, SlotID );
			}
			else if( pItem->ItemType == TEXT("ITEMTYPE_RUNE") ){
				//AutoPutTryGetWeaponSkillItemConsume( ContainerID, SlotID );
				SetCurConsumeID( pItemInfo->m_nTemplateID );
			}
			//else{
			//	FString errorLookUpString = TEXT("MMOGame.WeaponSkill.ERROR_SLOT_NOT_FOUND");
			//	MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( errorLookUpString ), MsgInfoType_Table, errorLookUpString );
			//}

			return TRUE;
		}
		break;
	case ESPType_WeaponSkill:
		{
			INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") );
			switch( panelSlotID )
			{
			case EWpSkill_WeaponOp:
				{
					//Weapon currently being soul-forged
					UnSetCurWeapon();
				}
				break;
			case EWpSkill_WepSkillCusm1:
			case EWpSkill_WepSkillCusm2:
				{
					//INT targetSlotIdx = panelSlotID - EWpSkill_WepSkillCusm1;
					//UnSetWeaponSkillConsumeItem( targetSlotIdx );
					UnSetCurConsumeID();
				}
				break;
			default:
				break;
			}

			return TRUE;
		}
		break;
	}


	return FALSE;
}

//
UBOOL UMMOUIWeaponSkill::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	if( _IsNoOpMode == TRUE ){
		return TRUE;
	}

	//
	switch( srcSlotType )
	{
	case ESPType_Bag:
	case ESPType_Charactor:
		{
			if( destSlotType != ESPType_WeaponSkill ){
				break;
			}
			if( destSlotMC == NULL ){
				break;;
			}

			FClientItemInfo *pItemInfo = mmoFacade.FindItem( srcContainerID, srcSlotID );
			if( pItemInfo == NULL ){
				return TRUE;
			}


			ClientActionCondition_WeaponSkill checker;

			FClientActionConditionContex contex;
			contex.Item = pItemInfo;

			if( !checker.CheckClientActionCondition(contex) ){
				return TRUE;
			}

			//
			INT panelSlotID = (INT)destSlotMC->GetFloat( TEXT("EquipID") );
			if( destSlotType == ESPType_WeaponSkill ){
				switch( panelSlotID )
				{
				case EWpSkill_WeaponOp:
					{
						//Weapon currently being soul-forged EWpSkill_WepSkillCusm1
						SetCurWeapon( srcContainerID, srcSlotID );
					}
					break;
				case EWpSkill_WepSkillCusm1:
				case EWpSkill_WepSkillCusm2:
					{
						//INT targetSlotIdx = panelSlotID - EWpSkill_WepSkillCusm1;
						//SetWeaponSkillConsumeItem( targetSlotIdx, srcContainerID, srcSlotID );
						SetCurConsumeID( pItemInfo->m_nTemplateID );
					}
					break;
				default:
					break;
				}
			}

			return TRUE;
		}
		break;
	case ESPType_WeaponSkill:
		{
			if( srcSlotMC == NULL ){
				break;
			}

			INT panelSlotID = (INT)srcSlotMC->GetFloat( TEXT("EquipID") );
			if( destSlotType != ESPType_WeaponSkill ){
				switch( panelSlotID )
				{
				case EWpSkill_WeaponOp:
					{
						//Weapon currently being soul-forged
						UnSetCurWeapon();
					}
					break;
				case EWpSkill_WepSkillCusm1:
				case EWpSkill_WepSkillCusm2:
					{
						//INT unsetSlotIdx = panelSlotID - EWpSkill_WepSkillCusm1;
						//UnSetWeaponSkillConsumeItem( unsetSlotIdx );
						UnSetCurConsumeID();
					}
					break;
				default:
					break;
				}
			}

			return TRUE;
		}
		break;
	}

	return FALSE;
}
//============================================================================================================
void UMMOUIWeaponSkill::UpdateCurWeaponSkills()
{
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();

	//
	UBOOL isEmpty = FALSE;
	if( _ToOpWeapon._ConID < 0 || _ToOpWeapon._SlotID < 0 ){
		//This weapon has no ability, clear ability display
		isEmpty = TRUE;
	}

	UEquip *pEquip = NULL;
	if( !isEmpty ){
		pEquip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( _ToOpWeapon._ConID, _ToOpWeapon._SlotID ) );
	}

	INT maxRuneCount = 0;
	if( pEquip != NULL ){
		maxRuneCount = pEquip->MaxRuneCount;
	}
	eventCallSetWeaponSkillTotalCount( maxRuneCount );
	//
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if( dataProvider == NULL ){
		return;
	}
	INT weaponSkillIdx = 0;
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		UBOOL isSkillActive = FALSE;
		UMMOAbility* pWeaponSkill = NULL;
		if( !isEmpty ){
			pWeaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( i, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
		}

		if( pWeaponSkill == NULL ){
			continue;
		}
		
		//
		UGFxObject* weaponSkillDataObj = GFXPlayer->CreateObject( TEXT("UI_WeaponSkill.WeaponSkillAS3.WeaponSkillData") );
		if( weaponSkillDataObj == NULL ){
			continue;
		}

		INT mainSkillID = pWeaponSkill->eventGetAbilityId();
		INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, pWeaponSkill->SpellCommonAttribute.Level, pWeaponSkill->GetRuneSlot() );
		weaponSkillDataObj->SetInt( TEXT("skillID"), uniqueSkillID );

		FString displayAbilityName = TEXT("");
		UMMOAbility *pMainSkill = mmoSkillMgr.GetMainAbilityFor( pWeaponSkill );
		if( pMainSkill != NULL ){
			displayAbilityName = pMainSkill->eventGetAbilityName();
		}
		displayAbilityName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pWeaponSkill->eventGetAbilityName() ) );
		weaponSkillDataObj->SetString( TEXT("skillName"), displayAbilityName );
		weaponSkillDataObj->SetString( TEXT("iconName"), pWeaponSkill->GetAbilityIconName() );

		//rune description
		const SpellVarData* spellDat = SpellVarManager::getSingleton().FindData( uniqueSkillID );
		if( spellDat ){
			weaponSkillDataObj->SetString( TEXT("skillDec") , spellDat->RuneDescription );
		}

		weaponSkillDataObj->SetInt( TEXT("tipsType"), ESPType_WeaponSkill );
		//
		dataProvider->SetElementObject( weaponSkillIdx, weaponSkillDataObj );
		++weaponSkillIdx;
	}

	eventCallASUpdateSkillInfoOfWeapon( dataProvider );
}
//============================================================================================================
void UMMOUIWeaponSkill::UnSetCurConsumeID()
{
	_CurConsumeItemTemplateID = -1;

	UpdateConsumeItem();
}
//============================================================================================================
void UMMOUIWeaponSkill::SetCurConsumeID( INT itemID )
{
	UItem *pConsumeItem = MMOFacade::getSingleton().FindItemTemplate( itemID );
	if( pConsumeItem == NULL || pConsumeItem->ItemType != TEXT("ITEMTYPE_RUNE") ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_NEED_ITEM_INPUT") ), MsgInfoType_Table, TEXT("MMOGame.WeaponSkill.ERROR_NEED_ITEM_INPUT") );
		return;
	}

	_CurConsumeItemTemplateID = itemID;

	UpdateConsumeItem();
}
INT UMMOUIWeaponSkill::GetCurConsumeID()
{
	return _CurConsumeItemTemplateID;
}
//============================================================================================================
UBOOL UMMOUIWeaponSkill::IsWeaponSkillConsumeItem( INT itemID )
{
	//UBOOL isItem = ( itemID == WEAPON_SKILL_CONSUME_ITEM_ID );
	//isItem |= (itemID == WEAPON_SKILL_LIMIT_CONSUME_ITEM_ID );

	//return isItem;
	return TRUE;
}
//============================================================================================================
void UMMOUIWeaponSkill::UpdateConsumeItem()
{
	INT curCnsmItemID = GetCurConsumeID();
	if( curCnsmItemID < 0 ){
		//_ConsumeItemSlotMc->SetInt( TEXT("ItemTemplateID"), 0 );
		//GFXPlayer->eventCallSetItemData( _ConsumeItemSlotMc, 0, TEXT(""), -1, ESPType_WeaponSkill, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		eventCallASSetConsumeItemCnt( 1, 0 );
		eventCallASSetConsumeItem( -1, TEXT(""), 0, ESPType_WeaponSkill );
		return;
	}

	UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( curCnsmItemID );
	if( pItem == NULL ){
		warnf( NAME_Error, TEXT("UMMOUIWeaponSkill::UpdateConsumeItem find no item template data.") );
		return;
	}

	INT itemCnt = MMOFacade::getSingleton().GetItemCountInBag( curCnsmItemID, E_CONSLOT_DEFAULT );

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_DropWeaponSkill);
	MMOUI::PostGameEvent(&evtGuide);

	//GFXPlayer->eventCallSetItemData( _ConsumeItemSlotMc, 1, mmoFacade.GetItemIconNamePersonalized( pItem ), itemTemplateID, ESPType_WeaponSkill, TEXT(""), pItem->ItemColor+1, 0, 0 );
	eventCallASSetConsumeItemCnt( 1, itemCnt );
	eventCallASSetConsumeItem( curCnsmItemID, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItem ), pItem->ItemColor+1, ESPType_WeaponSkill );
}
//============================================================================================================
void UMMOUIWeaponSkill::UpdateFee()
{
	if( Player == NULL ){
		return;
	}

	FString moneyStr;
	FString moneyColor;
	INT curMny = Player->GetMoney(E_MONEYTYPE_02);
	if( RUNE_ITEM_COST <= curMny ){
		moneyColor = TEXT("0xFFFFE0A6");
	}
	else{
		moneyColor = UITextParser::GetTipsColor( TEXT("AlertColor") );
	}

	moneyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, RUNE_ITEM_COST, moneyColor );
	eventCallASSetMoneyNeed( moneyStr );
}
//============================================================================================================
void UMMOUIWeaponSkill::UpdateGuildNeeds()
{
	if( Player == NULL ){
		return;
	}

	if( !_IsGuildMode ){
		return;
	}
	INT RUNE_ITEM_GUILD_COST = 1000;
	FString moneyStr;
	FString moneyColor;
	INT curMny = Player->GetMoney(E_MONEYTYPE_GUILD);
	if( RUNE_ITEM_GUILD_COST <= curMny ){
		moneyColor = TEXT("0xFFFFE0A6");
	}
	else{
		moneyColor = UITextParser::GetTipsColor( TEXT("AlertColor") );
	}

	moneyStr = UITextParser::HTMLBeginFont( moneyColor );
	moneyStr += appItoa( RUNE_ITEM_GUILD_COST );
	moneyStr += UITextParser::HTMLEndFont();

	eventCallASSetGuildNeeds( moneyStr );
}
////============================================================================================================
//void UMMOUIWeaponSkill::UnSetWeaponSkillConsumeItem( INT idx )
//{
//	SetWeaponSkillConsumeItem( idx, -1, -1);
//}
////------------------------------------------------------------------------------------------------------------
//void UMMOUIWeaponSkill::ResetAllPuttedWeaponSkillConsumeItem()
//{
//	for( INT i = 0; i < UCONST_MAX_WeaponSkill_CnsmItem; ++i ){
//		if( _WepSkillConsItem[i]._ConID >= 0 && _WepSkillConsItem[i]._SlotID >= 0 ){
//			UnSetWeaponSkillConsumeItem( i );
//		}
//	}
//}
////============================================================================================================
//UBOOL UMMOUIWeaponSkill::AutoPutTryGetWeaponSkillItemConsume( INT bagSlotCon, INT bagSlotID )
//{
//	for( INT i = 0; i < UCONST_MAX_WeaponSkill_CnsmItem; ++i ){
//		if( _WepSkillConsItem[i]._ConID < 0 && _WepSkillConsItem[i]._SlotID < 0 ){
//			return SetWeaponSkillConsumeItem( i, bagSlotCon, bagSlotID );
//		}
//	}
//
//	FString errorName = TEXT("MMOGame.EquipLevelUp.ConsumesIsFull");
//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
//	return FALSE;
//}
////------------------------------------------------------------------------------------------------------------
//UBOOL UMMOUIWeaponSkill::SetWeaponSkillConsumeItem( INT idx, INT contID, INT slotID )
//{
//	if( idx < 0 || idx >= UCONST_MAX_WeaponSkill_CnsmItem ){
//		return FALSE;
//	}
//
//	if( contID < 0 || slotID < 0 ){
//		if( _WepSkillConsItem[idx]._ConID >= 0 && _WepSkillConsItem[idx]._SlotID >= 0 ){
//			MMOFacade::getSingleton().LockItem( _WepSkillConsItem[idx]._ConID, _WepSkillConsItem[idx]._SlotID, -1 );
//
//			//eventUCSetSkillSlotByItem( idx, isSlotLocked, TEXT(""), -1, -1, -1 );
//			GFXPlayer->eventCallSetItemData( _WepSkillConsItemMc[idx] , 0 , TEXT("") , -1, ESPType_WeaponSkill, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
//		}
//
//		//
//		_WepSkillConsItem[idx]._ConID = -1;
//		_WepSkillConsItem[idx]._SlotID = -1;
//
//		//Refresh related display
//		UpdateTryGetWeaponSkillBtn();
//
//		return FALSE;
//	}
//
//	//Check whether it is a suitable ability item
//	UBOOL isOK = CheckConsumeItemForWeaponSkill( contID, slotID );
//	if( !isOK ){
//		return isOK;
//	}
//
//	//Unlock the equipment previously placed in the UI from the bag
//	if( _WepSkillConsItem[idx]._ConID >= 0 && _WepSkillConsItem[idx]._SlotID >= 0 ){
//		//if( pBagPanel != NULL && _SkillLevelUpItem._ConID == E_CONSLOT_DEFAULT){
//		MMOFacade::getSingleton().LockItem( _WepSkillConsItem[idx]._ConID, _WepSkillConsItem[idx]._SlotID, -1 );
//		//}
//	}
//
//	_WepSkillConsItem[idx]._ConID = contID;
//	_WepSkillConsItem[idx]._SlotID = slotID;
//	
//	//Set UI ItemSlot
//	FClientItemInfo *pSkillItemInfo = MMOFacade::getSingleton().FindItem( contID, slotID );
//	if( pSkillItemInfo != NULL ){
//		UItem *pSkillItemTplt =  MMOFacade::getSingleton().FindItemTemplate( pSkillItemInfo->m_nTemplateID );
//		if( pSkillItemTplt != NULL ){
//			//eventUCSetSkillSlotByItem( idx, isSlotLocked, MMOFacade::getSingleton().GetItemIconNamePersonalized( pSkillItemTplt, pSkillItemInfo ), contID, slotID, pSkillItemInfo->m_nTemplateID );
//			GFXPlayer->eventCallSetItemData( _WepSkillConsItemMc[idx], 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pSkillItemTplt, pSkillItemInfo ), pSkillItemInfo->m_nTemplateID, 
//				ESPType_WeaponSkill, TEXT(""), pSkillItemTplt->ItemColor+1, _WepSkillConsItem[idx]._ConID, _WepSkillConsItem[idx]._SlotID );
//		}
//	}
//
//	//Lock the ability item previously placed in the bag
//	MMOFacade::getSingleton().LockItem( _WepSkillConsItem[idx]._ConID, _WepSkillConsItem[idx]._SlotID, ESPType_WeaponSkill );
//
//	//Refresh related display
//	UpdateTryGetWeaponSkillBtn();
//
//	// Newbie guide
//	MMOGameEvent_Guide evtGuide(EGUET_DropWeaponSkill);
//	MMOUI::PostGameEvent(&evtGuide);
//
//	return TRUE;
//}
////============================================================================================================
//UBOOL UMMOUIWeaponSkill::CheckConsumeItemForWeaponSkill( INT contID, INT slotID )
//{
//	FString errorLookUpString;
//
//	MMOFacade &mmoFacade = MMOFacade::getSingleton();
//	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
//
//	//Whether it is an ability item
//	FClientItemInfo *pItemInfo = mmoFacade.FindItem( contID, slotID );
//	if( pItemInfo == NULL ){
//		errorLookUpString = TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND");
//		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( errorLookUpString ), MsgInfoType_Table, errorLookUpString );
//		return FALSE;
//	}
//
//	UItem *pSkillItemTplt = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
//	if( pSkillItemTplt == NULL ){
//		errorLookUpString = TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND");
//		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( errorLookUpString ), MsgInfoType_Table, errorLookUpString );
//		return FALSE;
//	}
//
//	UMMOAbility *itemAbility = pSkillItemTplt->GetToUseAbility();
//	if( itemAbility == NULL || pSkillItemTplt->ItemType != TEXT("ITEMTYPE_RUNE") ){
//		errorLookUpString = TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_SKILL_RUNE");
//		MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( errorLookUpString ), MsgInfoType_Table, errorLookUpString );
//		return FALSE;
//	}
//
//	return TRUE;
//}
//============================================================================================================
void UMMOUIWeaponSkill::PrePutWeapon()
{
	ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	//UEquip *pWeapon = Cast<UEquip>( pItem );
	if( pItemInfo == NULL ){
		return;
	}

	SetCurWeapon( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
}

//---------------------------------------------------------------------
void UMMOUIWeaponSkill::UnSetCurWeapon()
{
	SetCurWeapon( -1, -1 );

	////Update related UI
	//UMMOUIWeaponSkillSetting *pWeaponSkillSettingPanel = (UMMOUIWeaponSkillSetting*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkillSetting );
	//if( pWeaponSkillSettingPanel != NULL && pWeaponSkillSettingPanel->IsVisible ){
	//	pWeaponSkillSettingPanel->ShowWeaponSkillInfoPanel( FALSE, -1, -1, -1 );
	//}
}

//---------------------------------------------------------------------
UBOOL UMMOUIWeaponSkill::SetCurWeapon( INT containerID, INT slotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();

	//UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	if( containerID < 0 || slotID < 0 ){
		if( _ToOpWeapon._ConID >= 0 && _ToOpWeapon._SlotID >= 0 ){
			//
			//if( pBagPanel != NULL && _ToOpWeapon._ConID == E_CONSLOT_DEFAULT ){
				MMOFacade::getSingleton().LockItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, -1 );
			//}

			//GFXPlayer->SetItemSlotData( _ToEmbedEquipMc, NULL, ESPType_SoulEmbed, -1, -1 );
			GFXPlayer->eventCallSetItemData( _WeaponSlotMc , 0 , TEXT("") , -1, ESPType_WeaponSkill, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		_ToOpWeapon._ConID = -1;
		_ToOpWeapon._SlotID = -1;

		//refresh related display
		UpdateCurWeaponSkills();
		//ResetAllPuttedWeaponSkillConsumeItem();

		return TRUE;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );
	if( pInfo == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	//Whether it is a weapon
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	UEquip *pWeaponToOperation = Cast<UEquip>( pItemToOperation );
	if( pWeaponToOperation == NULL || pWeaponToOperation->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_WEAPON") ),MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_WEAPON") );
		return FALSE;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( _ToOpWeapon._ConID >= 0 && _ToOpWeapon._SlotID >= 0 ){
		//if( pBagPanel != NULL && _ToOpWeapon._ConID == E_CONSLOT_DEFAULT ){
			MMOFacade::getSingleton().LockItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, -1 );
		//}
		//
		//ResetAllPuttedWeaponSkillConsumeItem();
	}

	_ToOpWeapon._ConID = containerID;
	_ToOpWeapon._SlotID = slotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( _WeaponSlotMc, 1, mmoFacade.GetItemIconNamePersonalized( pWeaponToOperation, pInfo ), pWeaponToOperation->ItemId, 
									 ESPType_WeaponSkill, TEXT(""), pWeaponToOperation->ItemColor+1, _ToOpWeapon._ConID, _ToOpWeapon._SlotID );

	//lock the spirit previously placed in the bag
	//if( pBagPanel != NULL ){
		MMOFacade::getSingleton().LockItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, ESPType_WeaponSkill );
	//}

	////If this UI is open it needs to be closed
	//UMMOUIWeaponSkillSetting *pWeaponSkillSettingPanel = (UMMOUIWeaponSkillSetting*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkillSetting );
	//if( pWeaponSkillSettingPanel != NULL && pWeaponSkillSettingPanel->IsVisible ){
	//	pWeaponSkillSettingPanel->ShowWeaponSkillInfoPanel( FALSE, -1, -1, -1 );
	//}

	//refresh related display
	UpdateCurWeaponSkills();

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_DropWeaponToWeaponSkill);
	MMOUI::PostGameEvent(&evtGuide);

	return TRUE;
}

//============================================================================================================
void UMMOUIWeaponSkill::TryGetWeaponSkill( UBOOL isNeedCheckConfirm )
{
	FString errorName = CheckAllConditionsToGo();
	if( !errorName.IsEmpty() ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}

	
	if(isNeedCheckConfirm)
	{
		
		MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );

		FString MessageTxt = TEXT("");
		for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
		{
			INT weaponRuneSkillID = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( i, pItemInfo );
			INT weaponRuneSkillType = weaponRuneSkillID/10;
			const WeaponSkillCfg *pWeaponSkillCfgData = mmoEquipMgr.GetWeaponSkillCfg( weaponRuneSkillID );
			if( pWeaponSkillCfgData != NULL && pWeaponSkillCfgData->_IsRare )
			{
				FString BaseKey = FString(TEXT("MMOGame.WeaponSkill.ConfirmChangeWeaponSkillHint"));
				if(weaponRuneSkillType==2541)
				{
					if(MMOFacade::getSingleton().PetState>E_ps_Invalid)
					{
						FString KeyTxt = BaseKey + appItoa(weaponRuneSkillID);
						MessageTxt += MMOUI::ParseLocalizedPropertyPath(KeyTxt) + TEXT("\n");
					}
				}
				else if(weaponRuneSkillType==3003||weaponRuneSkillType==3008)
				{
					FString KeyTxt = BaseKey + appItoa(weaponRuneSkillID);
					MessageTxt += MMOUI::ParseLocalizedPropertyPath(KeyTxt) + TEXT("\n");
				}
				else
				{
					MessageTxt += MMOUI::ParseLocalizedPropertyPath(BaseKey)+ TEXT("\n");
				}
			}
			_CurWeaponCurSkill( i ) = weaponRuneSkillID;
		}


		if(MessageTxt.Len()>0)
		{
			MessageTxt += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.WeaponSkill.ConfirmChangeWeaponSkillHintEnd"));
			PopupConfirmBox(MessageTxt);
			return;
		}
	}

	//for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){

	//	if(isNeedCheckConfirm)
	//	{
	//		INT weaponRuneSkillID = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( i, pItemInfo );
	//		INT weaponRuneSkillType = weaponRuneSkillID/10;
	//		if(weaponRuneSkillType==2541||weaponRuneSkillType==3003||weaponRuneSkillType==3008)
	//		{
	//			if(weaponRuneSkillType==2541 && MMOFacade::getSingleton().PetState==E_ps_Invalid)//no pet, 2541 series gives no prompt
	//			{
	//				isNeedCheckConfirm = FALSE;
	//			}
	//			nWeaponSkillPromptID = weaponRuneSkillID;
	//		}
	//	}

	//	 const WeaponSkillCfg *pWeaponSkillCfgData = mmoEquipMgr.GetWeaponSkillCfg( weaponRuneSkillID );
	//	 if( pWeaponSkillCfgData != NULL && pWeaponSkillCfgData->_IsRare ){
	//		 isNeedConfirm = TRUE;
	//	 }
	//	 _CurWeaponCurSkill( i ) = weaponRuneSkillID;
	//}
	//
	////
	//if( isNeedCheckConfirm && isNeedConfirm ){

	//	FString MessageTxt = FString(TEXT("MMOGame.WeaponSkill.ConfirmChangeWeaponSkillHint"));
	//	if(nWeaponSkillPromptID>0)
	//	{
	//		MessageTxt = MessageTxt + appItoa(nWeaponSkillPromptID);
	//	}
	//	PopupConfirmBox(MMOUI::ParseLocalizedPropertyPath(MessageTxt));
	//	return;
	//}

	INT curConsumeItemID = GetCurConsumeID();
	CMessageHandler::GetInstance().Send_TryGetWeaponRune( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, curConsumeItemID );

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_EmbedWeaponSkill);
	MMOUI::PostGameEvent(&evtGuide);
}
//===============================================================================================================
void UMMOUIWeaponSkill::PopupConfirmBox(const FString& MessageTxt)
{
	if( _ConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmBox );
		_ConfirmBox = NULL;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT( "ConfirmChangeWeaponSkill" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "CancelChangeWeaponSkill" );

	FString strTitle = TEXT("");//MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpTitle") );


	
	//FString strContentKey = FString(TEXT("MMOGame.WeaponSkill.ConfirmChangeWeaponSkillHint")) + appItoa(nPromptID);
	//UINT16  nPromptID = MMOFacade::getSingleton().WeaponSkillPromptID();
	//if(nPromptID>0)
	//{
	//	strContentKey = strContentKey +appItoa(nPromptID);
	//}	
	//FString strContent = MMOUI::ParseLocalizedPropertyPath( strContentKey );

	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( MessageTxt, 0, funYes, funNo );
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkill::ConfirmChangeWeaponSkill( UGFxObject *pMc )
{
	if( _ConfirmBox == NULL ){
		return;
	}
	//INT rechargeMoney = pMc->GetInt( TEXT("iteminfoid") );
	_ConfirmBox = NULL;
	//
	TryGetWeaponSkill( FALSE );
}

//---------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkill::CancelChangeWeaponSkill( UGFxObject *pMc )
{
	_ConfirmBox = NULL;
}
//============================================================================================================
FString UMMOUIWeaponSkill::CheckAllConditionsToGo()
{
	FString errorName = TEXT("");
	INT curCnsmItemID = GetCurConsumeID();
	if( curCnsmItemID < 0 ){
		return TEXT("MMOGame.WeaponSkill.ERROR_NEED_ITEM_INPUT");
	}

	INT itemCnt = MMOFacade::getSingleton().GetItemCountInBag( curCnsmItemID, E_CONSLOT_DEFAULT );
	if( itemCnt <= 0 ){
		return TEXT("MMOGame.WeaponSkill.ERROR_NO_ENOUGH_CONSUMES");
	}

	if( _ToOpWeapon._ConID < 0 || _ToOpWeapon._SlotID < 0 ){
		return TEXT("MMOGame.WeaponSkill.ERROR_NEED_WEAPON_INPUT");
	}
	
	if( Player ){
		INT curMyMoney = Player->GetMoney( MONEYTYPE_02 );
		if( curMyMoney < RUNE_ITEM_COST ){
			return TEXT("MMOGame.WeaponSkill.ERROR_NO_ENOUGH_MONEY");
		}
	}

	return errorName;
}
//-----------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkill::NotifyTryWeaponSkillResult( INT result )
{
	UBOOL hasPlayedAnim = FALSE;
	FString hint;
	FString strString;
	if( result != 0 ){
		////Success
		//strString = TEXT("MMOGame.WeaponSkill.AddWeaponSkillSuccess");
		//hint = MMOUI::ParseLocalizedPropertyPath( strString );

		MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
		MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
		FString chatHintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ResultSuccessChatHint") );
		if( pItemInfo != NULL ){
			UBOOL isHasSkill = FALSE;
			for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
			{
				UBOOL isSkillSlotActive = FALSE;
				UMMOAbility* pWeaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( i, pItemInfo, isSkillSlotActive );
				if( pWeaponSkill == NULL ){
					continue;
				}

				//Name
				FString skillName;
				UMMOAbility *pMainSkill = mmoSkillMgr.GetMainAbilityFor( pWeaponSkill );
				if( pMainSkill != NULL ){
					skillName += pMainSkill->eventGetAbilityName();
				}
				skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pWeaponSkill->eventGetAbilityName() ) );

				//
				isHasSkill = TRUE;

				//Play effect
				INT weaponSkillID = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( i, pItemInfo );
				if( weaponSkillID != _CurWeaponCurSkill( i ) ){
					hasPlayedAnim = TRUE;

					eventCallASPlayNewSkillGainEffect( i );

					//
					MessageData *pNewMsgData = new MessageData();
					pNewMsgData->MessageType = ESMT_WeaponSkillGained;
					INT mainSkillID = pWeaponSkill->eventGetAbilityId();
					INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, pWeaponSkill->SpellCommonAttribute.Level, pWeaponSkill->GetRuneSlot() );
					pNewMsgData->MagicValue = uniqueSkillID;

					ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );

					//Question prompt
					MMOChatManager::getSingleton().AddSystemMessage(  FString::Printf( *chatHintFormat, *skillName ) );
				}
			}

			if( isHasSkill == FALSE ){
				MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ResultFailedChatHint") ) );
			}
			else if( hasPlayedAnim == FALSE ){
				UBOOL isSkillSlotActive = FALSE;
				INT defaultPlayIdx = 0;
				UMMOAbility* pWeaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( defaultPlayIdx, pItemInfo, isSkillSlotActive );
				if( pWeaponSkill != NULL ){
					//Name
					FString skillName;
					UMMOAbility *pMainSkill = mmoSkillMgr.GetMainAbilityFor( pWeaponSkill );
					if( pMainSkill != NULL ){
						skillName += pMainSkill->eventGetAbilityName();
					}
					skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pWeaponSkill->eventGetAbilityName() ) );

					//
					eventCallASPlayNewSkillGainEffect( defaultPlayIdx );
					MMOChatManager::getSingleton().AddSystemMessage(  FString::Printf( *chatHintFormat, *skillName ) );

					//
					MessageData *pNewMsgData = new MessageData();
					pNewMsgData->MessageType = ESMT_WeaponSkillGained;
					INT mainSkillID = pWeaponSkill->eventGetAbilityId();
					INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, pWeaponSkill->SpellCommonAttribute.Level, pWeaponSkill->GetRuneSlot() );
					pNewMsgData->MagicValue = uniqueSkillID;

					ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );
				}
			}
		}

		//Change ability
		//MMOEquipManager::getSingleton().EquipSkillsToFitCurWeapon( TRUE );

		//  tutorial quest
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Posy);
	}
	else{
		strString = TEXT("MMOGame.WeaponSkill.AddWeaponSkillFailed");
		hint = MMOUI::ParseLocalizedPropertyPath( strString );
	}

	//
	if( !hint.IsEmpty() ){
		MMOChatManager::getSingleton().AddSystemMessage( hint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( hint, 1, strString );
	}
}
//============================================================================================================
void UMMOUIWeaponSkill::OnClickAutoChangeSkillChkBtn()
{
	if( _AutoChangeSkillChkBtn == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [OnClickAutoChangeSkillChkBtn _AutoChangeSkillChkBtn is NULL]") );
		return;
	}

	_IsAutoChangeSkill = _AutoChangeSkillChkBtn->GetBool( TEXT("selected") );
	MMOEquipManager::getSingleton().SetAutoChangeSkills( _IsAutoChangeSkill );
}
//=================================================================================================================================
void UMMOUIWeaponSkill::GrayAllBagNotEquip( UBOOL isToGray )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	if( Player == NULL ){
		return;
	}

	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	for( UINT j = 0; j < bagCapability; ++j){
		//
		if( isToGray == FALSE || _IsNoOpMode ){
			mmoFacade.GrayItem( ESGMark_WeaponSkill, E_CONSLOT_DEFAULT, j, FALSE );
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
		mmoFacade.GrayItem( ESGMark_WeaponSkill, E_CONSLOT_DEFAULT, j, isNeedToGray );
	}
}

//
UBOOL UMMOUIWeaponSkill::IsNeedToGray( UItem *pItemInBag, const FClientItemInfo *pItemInfo )
{
	if( pItemInBag == NULL || pItemInfo == NULL ){
		return FALSE;
	}
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL || mainPlayer->ActorAttributes == NULL ){
		return FALSE;
	}
	EPlayerClass playerClass = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;

	UBOOL noNeedToGray = ( pItemInBag->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON") );

	if( noNeedToGray == FALSE ){
		//if a replace skill item
		noNeedToGray |= ( pItemInBag->ItemType == TEXT("ITEMTYPE_RUNE") );
		//noNeedToGray = noNeedToGray ? MMO::CanBeUsedByPlayerClass( playerClass, pItemInBag->RequireClass) : noNeedToGray;
	}

	return (!noNeedToGray);
}
//============================================================================================================
void UMMOUIWeaponSkill::UpdateTryGetWeaponSkillBtn()
{
	FString errorName = CheckAllConditionsToGo();

	if( _ConfirmTryGetSkillBtn != NULL ){
		_ConfirmTryGetSkillBtn->SetBool( TEXT("enabled"), errorName.IsEmpty() );
	}
}
//============================================================================================================
void UMMOUIWeaponSkill::Init()
{
	Super::Init();

	//
	_CurWeaponCurSkill.AddZeroed( EQUIP_RUNE_SKILL_MAX );
	//
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
	//
	InitWeaponSkillPanelMc();
	//UpdateCurWeaponSkills();
	//
	_ToOpWeapon._ConID = -1;
	_ToOpWeapon._SlotID = -1;

	//for( INT i = 0; i < UCONST_MAX_WeaponSkill_CnsmItem; ++i ){
	//	_WepSkillConsItem[i]._ConID = -1;
	//	_WepSkillConsItem[i]._SlotID = -1;
	//}
	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_WeaponSkillAdd);
	evts.AddItem(GameEvent_Item_Delete);
	evts.AddItem(GameEvent_SkillUiUpdate);
	evts.AddItem(GameEvent_BagSortEnd);
	EventAdapter = new TGameEventAdapter<UMMOUIWeaponSkill>(this, evts);
}

//============================================================================================================
void UMMOUIWeaponSkill::InitWeaponSkillPanelMc()
{
	_WeaponSlotMc = GetObject( TEXT("WeaponSlotMc") );
	if( _WeaponSlotMc != NULL ){
		_WeaponSlotMc->SetInt( TEXT("EquipID"), EWpSkill_WeaponOp );
		_WeaponSlotMc->SetInt( TEXT("SlotType"), ESPType_WeaponSkill );
	}

	_ConsumeItemSlotMc = GetObject( TEXT("ItemslotMc") );
	if( _ConsumeItemSlotMc != NULL ){
		//_ConsumeItemSlotMc->SetBool( TEXT("DisableDrag"), TRUE );
		_ConsumeItemSlotMc->SetInt( TEXT("EquipID"), EWpSkill_WepSkillCusm1 );
		_ConsumeItemSlotMc->SetInt( TEXT("SlotType"), ESPType_WeaponSkill );
	}
	
	//for( INT i = 0; i < UCONST_MAX_WeaponSkill_CnsmItem; ++i ){
	//	FString objName = TEXT("WeaponSkillMc");
	//	objName += appItoa( i );
	//	UGFxObject *pItemSlot = GetObject( objName );
	//	_WepSkillConsItemMc[i] = pItemSlot;
	//	//
	//	if(pItemSlot != NULL ){
	//		pItemSlot->SetInt( TEXT("EquipID"), EWpSkill_WepSkillCusm1 + i );
	//		pItemSlot->SetInt( TEXT("SlotType"), ESPType_WeaponSkill );
	//	}
	//}

	//
	_ConfirmTryGetSkillBtn = GetObject( TEXT("btn_baptize") );
}

//============================================================================================================
void UMMOUIWeaponSkill::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}


//============================================================================================================
void UMMOUIWeaponSkill::ASClickToTryGetWeaponSkill()
{
	TryGetWeaponSkill( TRUE );
}
//===============================================================================================================
void UMMOUIWeaponSkill::ASShowTips_WeaponSkill( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	INT uniqueSkillID = mc->GetInt( TEXT("UniqueSkillID") );
	if( uniqueSkillID > 0 ){
		MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplate, mc);
	}
}

//--------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkill::ASHideTips_WeaponSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//===============================================================================================================
void UMMOUIWeaponSkill::ASShowTips_ConsumeItem( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_AwardIcon, mc );
}

//--------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkill::ASHideTips_ConsumeItem()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===============================================================================================================
void UMMOUIWeaponSkill::ASOnClickAutoChangeSkillChkBtn()
{
	OnClickAutoChangeSkillChkBtn();
}
