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

IMPLEMENT_CLASS(UMMOUIWeaponSkillRplc)
void UMMOUIWeaponSkillRplc::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		////Cannot be opened before level 10
		//if( Player && Player->ActorAttributes ){
		//	if( Player->ActorAttributes->LevelAttributes.Level < 10 ){
		//		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SkillTree.CanNotOpenHint")),MsgInfoType_Table,TEXT("MMOGame.SkillTree.CanNotOpenHint"));
		//		Show( FALSE );
		//		return;
		//	}
		//}

		//UpdateConsumeItem();
		UpdateFee();

		UBOOL isOnGuildMap = ( MMOMapDataManager::getSingleton().GetCurMap().ServerID == GUILD_TERRITORY_MAP_ID );
		ChangeToGuildMode( isOnGuildMap );
		UpdateGuildNeeds();
		//
		PrePutWeapon();
		//
		UpdateTryReplaceWeaponSkillBtn();
		UpdateReplaceWeaponSkillBtn();
		//
		GrayAllBagNotEquip( TRUE );

	}
	else{
		//
		ResetUI();
		//
		GrayAllBagNotEquip( FALSE );
		//
		ResetAllPopupBox();
	}
}

//============================================================================================================
void UMMOUIWeaponSkillRplc::ChangeToGuildMode( UBOOL isToSet )
{
	_IsGuildMode = isToSet;
	//
	eventCallASSetWSkillRplcIsGuildEdition( _IsGuildMode );
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::ResetUI()
{
	UnSetCurWeapon();
	//
	UnSetCurConsumeID();
}
//
void UMMOUIWeaponSkillRplc::ResetCachedRplcWeaponRune()
{
	MMOEquipManager::getSingleton().ResetWeaponRplcCachedRune();
}
//
void UMMOUIWeaponSkillRplc::ResetAllPopupBox()
{
	if( _ConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmBox );
		_ConfirmBox = NULL;
	}
}
//========================================================================================
void UMMOUIWeaponSkillRplc::OnNativeGameEvent(struct MMOGameEvent* evt)
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
				NotifyReplaceWeaponSkillResult( weaponSkillAddEvt->IntData );
			}
		}
		break;
	case GameEvent_WeaponSkillRplcCacheChanged:
		{
			MMOGameEvent_OneInt *weaponSkillAddEvt = (MMOGameEvent_OneInt*)evt;

			if( IsVisible ){
				NotifyTryReplaceWeaponSkillResult( weaponSkillAddEvt->IntData );
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
void UMMOUIWeaponSkillRplc::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	//Weapon changed, refresh weapon abilities
	if( _ToOpWeapon._ConID == bagId && _ToOpWeapon._SlotID == slotId ){
		UpdateAllWeaponSkills();
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
				mmoFacade.GrayItem( ESGMark_WeaponSkillRplc, E_CONSLOT_DEFAULT, slotId, isNeedToGray );
			}
		}
		else{
			UpdateConsumeItem();
		}
	}
}
//========================================================================================================================================
void UMMOUIWeaponSkillRplc::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
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
void UMMOUIWeaponSkillRplc::OnFocusGained()
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
void UMMOUIWeaponSkillRplc::OnFocusLost()
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
UBOOL UMMOUIWeaponSkillRplc::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIWeaponSkillRplc::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIWeaponSkillRplc::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	if( mc == NULL ){
		return FALSE;
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
			//	FString errorLookUpString = TEXT("MMOGame.WeaponSkillRplc.ERROR_SLOT_NOT_FOUND");
			//	MMOUI::GetUIManager()->delegateAddScreenMessage( *MMOUI::ParseLocalizedPropertyPath( errorLookUpString ), MsgInfoType_Table, errorLookUpString );
			//}

			return TRUE;
		}
		break;
	case ESPType_WeaponSkillRplc:
		{
			INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") );
			switch( panelSlotID )
			{
			case EWpSklRplcSltTp_WeaponOp:
				{
					//Weapon currently being soul-forged
					UnSetCurWeapon();
				}
				break;
			case EWpSklRplcSltTp_WpSkillCusm1:
			case EWpSklRplcSltTp_WpSkillCusm2:
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
UBOOL UMMOUIWeaponSkillRplc::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	//
	switch( srcSlotType )
	{
	case ESPType_Bag:
	case ESPType_Charactor:
		{
			if( destSlotType != ESPType_WeaponSkillRplc ){
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
			if( destSlotType == ESPType_WeaponSkillRplc ){
				switch( panelSlotID )
				{
				case EWpSklRplcSltTp_WeaponOp:
					{
						//Weapon currently being soul-forged EWpSkill_WepSkillCusm1
						SetCurWeapon( srcContainerID, srcSlotID );
					}
					break;
				case EWpSklRplcSltTp_WpSkillCusm1:
				case EWpSklRplcSltTp_WpSkillCusm2:
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
	case ESPType_WeaponSkillRplc:
		{
			if( srcSlotMC == NULL ){
				break;
			}

			INT panelSlotID = (INT)srcSlotMC->GetFloat( TEXT("EquipID") );
			if( destSlotType != ESPType_WeaponSkillRplc ){
				switch( panelSlotID )
				{
				case EWpSklRplcSltTp_WeaponOp:
					{
						//Weapon currently being soul-forged
						UnSetCurWeapon();
					}
					break;
				case EWpSklRplcSltTp_WpSkillCusm1:
				case EWpSklRplcSltTp_WpSkillCusm2:
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
void UMMOUIWeaponSkillRplc::UpdateAllWeaponSkills()
{
	UpdateRplcCacheWeaponSkills();
	UpdateCurWeaponSkills();
}
//------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::UpdateRplcCacheWeaponSkills()
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
	eventCallAsSetWSkillRplcSkillTotalCount( maxRuneCount );
	//
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if( dataProvider == NULL ){
		return;
	}
	INT weaponSkillIdx = 0;
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		UMMOAbility* pWeaponSkill = NULL;
		if( !isEmpty ){
			pWeaponSkill = mmoEquipMgr.GetWeaponRplcCacheRuneSkillBySlotIdx( i, _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
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

		weaponSkillDataObj->SetInt( TEXT("tipsType"), ESPType_WeaponSkillRplc );
		//
		dataProvider->SetElementObject( weaponSkillIdx, weaponSkillDataObj );
		++weaponSkillIdx;
	}

	eventCallASUpdatetWSkillRplcSkillInfo( dataProvider );
}
//------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::UpdateCurWeaponSkills()
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
	eventCallASSetWSkillRplcCurSkillTotalCount( maxRuneCount );
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

		weaponSkillDataObj->SetInt( TEXT("tipsType"), ESPType_WeaponSkillRplc );
		//
		dataProvider->SetElementObject( weaponSkillIdx, weaponSkillDataObj );
		++weaponSkillIdx;
	}

	eventCallASUpdateWSkillRplcCurSkillInfo( dataProvider );
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::UnSetCurConsumeID()
{
	_CurConsumeItemTemplateID = -1;

	UpdateConsumeItem();
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::SetCurConsumeID( INT itemID )
{
	UItem *pConsumeItem = MMOFacade::getSingleton().FindItemTemplate( itemID );
	if( pConsumeItem == NULL || pConsumeItem->ItemType != TEXT("ITEMTYPE_RUNE") ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ERROR_NEED_ITEM_INPUT") ), MsgInfoType_Table, TEXT("MMOGame.WeaponSkillRplc.ERROR_NEED_ITEM_INPUT") );
		return;
	}

	_CurConsumeItemTemplateID = itemID;

	UpdateConsumeItem();

	UpdateTryReplaceWeaponSkillBtn();
}
INT UMMOUIWeaponSkillRplc::GetCurConsumeID()
{
	return _CurConsumeItemTemplateID;
}
//============================================================================================================
UBOOL UMMOUIWeaponSkillRplc::IsWeaponSkillConsumeItem( INT itemID )
{
	//UBOOL isItem = ( itemID == WEAPON_SKILL_CONSUME_ITEM_ID );
	//isItem |= (itemID == WEAPON_SKILL_LIMIT_CONSUME_ITEM_ID );

	//return isItem;
	return TRUE;
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::UpdateConsumeItem()
{
	INT curCnsmItemID = GetCurConsumeID();
	if( curCnsmItemID < 0 ){
		//_ConsumeItemSlotMc->SetInt( TEXT("ItemTemplateID"), 0 );
		//GFXPlayer->eventCallSetItemData( _ConsumeItemSlotMc, 0, TEXT(""), -1, ESPType_WeaponSkillRplc, TEXT(""), ITEMCOLOR_INVALID, -1, -1 );
		eventCallASUpdateWSkillRplcItemNum( 1, 0 );
		eventCallASSetWSkillRplcCnsmItemData( -1, TEXT(""), 0, ESPType_WeaponSkillRplc );
		return;
	}

	UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( curCnsmItemID );
	if( pItem == NULL ){
		warnf( NAME_Error, TEXT("UMMOUIWeaponSkillRplc::UpdateConsumeItem find no item template data.") );
		return;
	}

	INT itemCnt = MMOFacade::getSingleton().GetItemCountInBag( curCnsmItemID, E_CONSLOT_DEFAULT );

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_DropWeaponSkill);
	MMOUI::PostGameEvent(&evtGuide);

	//GFXPlayer->eventCallSetItemData( _ConsumeItemSlotMc, 1, mmoFacade.GetItemIconNamePersonalized( pItem ), itemTemplateID, ESPType_WeaponSkillRplc, TEXT(""), pItem->ItemColor+1, 0, 0 );
	eventCallASUpdateWSkillRplcItemNum( 1, itemCnt );
	eventCallASSetWSkillRplcCnsmItemData( curCnsmItemID, MMOFacade::getSingleton().GetItemIconNamePersonalized( pItem ), pItem->ItemColor+1, ESPType_WeaponSkillRplc );

	//
	UpdateTryReplaceWeaponSkillBtn();
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::UpdateFee()
{
	if( Player == NULL ){
		return;
	}

	FString moneyStr;
	FString moneyColor;
	INT curMny = Player->GetMoney(E_MONEYTYPE_02);
	if( RUNE_ITEM_COST_HIGH <= curMny ){
		moneyColor = TEXT("0xFFFFE0A6");
	}
	else{
		moneyColor = UITextParser::GetTipsColor( TEXT("AlertColor") );
	}

	moneyStr = UITextParser::HTMLMoneyIconString( MONEYTYPE_02, RUNE_ITEM_COST_HIGH, moneyColor );
	eventCallASSetWSkillRplcMoneyNeed( moneyStr );

	//
	UpdateTryReplaceWeaponSkillBtn();
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::UpdateGuildNeeds()
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

	eventCallASSetWSkillRplcGuildNeeds( moneyStr );
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::PrePutWeapon()
{
	ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	//UEquip *pWeapon = Cast<UEquip>( pItem );
	if( pItemInfo == NULL ){
		return;
	}

	SetCurWeapon( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
}

//---------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::UnSetCurWeapon()
{
	SetCurWeapon( -1, -1 );

	////Update related UI
	//UMMOUIWeaponSkillSetting *pWeaponSkillSettingPanel = (UMMOUIWeaponSkillSetting*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkillSetting );
	//if( pWeaponSkillSettingPanel != NULL && pWeaponSkillSettingPanel->IsVisible ){
	//	pWeaponSkillSettingPanel->ShowWeaponSkillInfoPanel( FALSE, -1, -1, -1 );
	//}
}

//---------------------------------------------------------------------
UBOOL UMMOUIWeaponSkillRplc::SetCurWeapon( INT containerID, INT slotID )
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
			GFXPlayer->eventCallSetItemData( _WeaponSlotMc , 0 , TEXT("") , -1, ESPType_WeaponSkillRplc, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
			//
			//ResetCachedRplcWeaponRune();
		}

		//
		_ToOpWeapon._ConID = -1;
		_ToOpWeapon._SlotID = -1;

		//refresh related display
		UpdateAllWeaponSkills();
		//ResetAllPuttedWeaponSkillConsumeItem();
		UpdateTryReplaceWeaponSkillBtn();

		return TRUE;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );
	if( pInfo == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ERROR_ITEM_NOT_FOUND") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkillRplc.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	////
	//INT weaponRuneCnt = MMOEquipManager::getSingleton().GetWeaponCurSkillSlotCnt( pInfo );
	//if( weaponRuneCnt <= 1 ){
	//	FString errStrName = TEXT("MMOGame.WeaponSkillRplc.errItemNotDoubleRune");
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errStrName ), MsgInfoType_Table, errStrName );
	//	return FALSE;
	//}

	//Whether it is a weapon
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	UEquip *pWeaponToOperation = Cast<UEquip>( pItemToOperation );
	if( pWeaponToOperation == NULL || pWeaponToOperation->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ERROR_NOT_A_WEAPON") ),MsgInfoType_Table,TEXT("MMOGame.WeaponSkillRplc.ERROR_NOT_A_WEAPON") );
		return FALSE;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( _ToOpWeapon._ConID >= 0 && _ToOpWeapon._SlotID >= 0 ){
		//if( pBagPanel != NULL && _ToOpWeapon._ConID == E_CONSLOT_DEFAULT ){
		MMOFacade::getSingleton().LockItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, -1 );
		//ResetCachedRplcWeaponRune();
		//}
		//
		//ResetAllPuttedWeaponSkillConsumeItem();
	}

	_ToOpWeapon._ConID = containerID;
	_ToOpWeapon._SlotID = slotID;

	//set UI ItemSlot
	GFXPlayer->eventCallSetItemData( _WeaponSlotMc, 1, mmoFacade.GetItemIconNamePersonalized( pWeaponToOperation, pInfo ), pWeaponToOperation->ItemId, 
		ESPType_WeaponSkillRplc, TEXT(""), pWeaponToOperation->ItemColor+1, _ToOpWeapon._ConID, _ToOpWeapon._SlotID );

	//lock the spirit previously placed in the bag
	//if( pBagPanel != NULL ){
	MMOFacade::getSingleton().LockItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, ESPType_WeaponSkillRplc );
	//}

	////If this UI is open it needs to be closed
	//UMMOUIWeaponSkillSetting *pWeaponSkillSettingPanel = (UMMOUIWeaponSkillSetting*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkillSetting );
	//if( pWeaponSkillSettingPanel != NULL && pWeaponSkillSettingPanel->IsVisible ){
	//	pWeaponSkillSettingPanel->ShowWeaponSkillInfoPanel( FALSE, -1, -1, -1 );
	//}

	//refresh related display
	UpdateAllWeaponSkills();
	UpdateTryReplaceWeaponSkillBtn();

	return TRUE;
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::ReplaceWeaponSkill( INT checkConfirmState )
{
	FString errorName = CheckAllConditionsToReplace();
	if( !errorName.IsEmpty() ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}

	UBOOL isPopMsgBox = FALSE;
	switch( checkConfirmState )
	{
	case EWpSklRplcCfmSta_Info:
		{
			isPopMsgBox = PopupReplaceConfirmInfoBox();
		}
		break;
	case EWpSklRplcCfmSta_SideEfct:
		{
			isPopMsgBox = PopupReplaceConfirmSideEfctBox();
		}
		break;
	}

	if( isPopMsgBox ){
		return;
	}

	//INT curConsumeItemID = GetCurConsumeID();
	CMessageHandler::GetInstance().Send_ConfirmReplaceWeaponRune( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
}
//============================================================================================================
UBOOL UMMOUIWeaponSkillRplc::PopupReplaceConfirmInfoBox()
{
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );

	FString toBeRplcSkillName = TEXT("");
	FString rplcSkillName = TEXT("");
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		UMMOAbility* pRplcWeaponSkill = mmoEquipMgr.GetWeaponRplcCacheRuneSkillBySlotIdx( i, pItemInfo );
		if( pRplcWeaponSkill == NULL ){
			continue;
		}
		//Name
		if( pRplcWeaponSkill != NULL ){
			rplcSkillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pRplcWeaponSkill->eventGetAbilityName() ) );
		}

		//
		UBOOL isSkillSlotActive = FALSE;
		UMMOAbility* pCurWeaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( i, pItemInfo, isSkillSlotActive );
		if( pCurWeaponSkill != NULL ){
			toBeRplcSkillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pCurWeaponSkill->eventGetAbilityName() ) );
		}
		
		//
		_CurWeaponCurSkill( i ) = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( i, pItemInfo );
	}


	if( rplcSkillName.Len() > 0 )
	{
		FString hintStringName = TEXT("MMOGame.WeaponSkillRplc.ConfirmReplaceHintCommon");
		FString hintString = MMOUI::ParseLocalizedPropertyPath( hintStringName );
		FString msgText = FString::Printf( *hintString, *toBeRplcSkillName, *rplcSkillName );

		PopupConfirmBox( msgText, EWpSklRplcCfmSta_Info );
		return TRUE;
	}

	return FALSE;
}
//============================================================================================================
UBOOL UMMOUIWeaponSkillRplc::PopupReplaceConfirmSideEfctBox()
{
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );

	FString MessageTxt = TEXT("");
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		INT weaponRuneSkillID = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( i, pItemInfo );
		INT weaponRuneSkillType = weaponRuneSkillID / 10;
		const WeaponSkillCfg *pWeaponSkillCfgData = mmoEquipMgr.GetWeaponSkillCfg( weaponRuneSkillID );

		//
		if( pWeaponSkillCfgData != NULL && pWeaponSkillCfgData->_IsRare ){
			//No prompt for the same rare inscription
			INT tryRplcWeaponRuneSkillID = mmoEquipMgr.GetWeaponRplcCacheRuneSkillID( i, pItemInfo );
			UBOOL hasSameRare = ( tryRplcWeaponRuneSkillID == weaponRuneSkillID );
			for( INT j = 0; j < EQUIP_RUNE_SKILL_MAX && !hasSameRare; ++j ){
				if( i == j ){
					continue;
				}

				tryRplcWeaponRuneSkillID = mmoEquipMgr.GetWeaponRplcCacheRuneSkillID( j, pItemInfo );
				hasSameRare = ( tryRplcWeaponRuneSkillID == weaponRuneSkillID );
			}

			if( hasSameRare ){
				continue;
			}
			//Prompt that a rare inscription, or an inscription that changes the existing state, will be replaced
			FString BaseKey = TEXT("MMOGame.WeaponSkillRplc.ConfirmChangeWeaponSkillHint");
			if( weaponRuneSkillType == 2541 ){
				if( MMOFacade::getSingleton().PetState > E_ps_Invalid ){
					FString KeyTxt = BaseKey + appItoa( weaponRuneSkillID );
					MessageTxt += MMOUI::ParseLocalizedPropertyPath( KeyTxt ) + TEXT("\n");
				}
			}
			else if( weaponRuneSkillType == 3003 || weaponRuneSkillType == 3008 ){
				FString KeyTxt = BaseKey + appItoa( weaponRuneSkillID );
				MessageTxt += MMOUI::ParseLocalizedPropertyPath( KeyTxt ) + TEXT("\n");
			}
			else{
				MessageTxt += MMOUI::ParseLocalizedPropertyPath( BaseKey )+ TEXT("\n");
			}
		}
		_CurWeaponCurSkill( i ) = weaponRuneSkillID;
	}


	if( MessageTxt.Len() > 0 ){
		MessageTxt += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ConfirmChangeWeaponSkillHintEnd") );
		PopupConfirmBox( MessageTxt, EWpSklRplcCfmSta_SideEfct );
		return TRUE;
	}

	return FALSE;
}
//===============================================================================================================
void UMMOUIWeaponSkillRplc::PopupConfirmBox( const FString& MessageTxt, INT messageBoxCallState )
{
	if( _ConfirmBox != NULL && messageBoxCallState != EWpSklRplcCfmSta_SideEfct ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmBox );
		_ConfirmBox = NULL;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;

	switch( messageBoxCallState )
	{
	case EWpSklRplcCfmSta_Info:
	case EWpSklRplcCfmSta_SideEfct:
		{
			funYes.FunctionName = TEXT( "ConfirmReplaceWeaponSkill" );
			funNo.FunctionName = TEXT( "CancelReplaceWeaponSkill" );
		}
		break;
	case EWpSklRplcCfmSta_TryCnfmRare:
		{
			funYes.FunctionName = TEXT( "ConfirmTryReplaceWeaponSkill" );
			funNo.FunctionName = TEXT( "CancelTryReplaceWeaponSkill" );
		}
		break;
	}

	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( MessageTxt, messageBoxCallState, funYes, funNo );
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::ConfirmReplaceWeaponSkill( UGFxObject *pMc )
{
	if( _ConfirmBox == NULL ){
		return;
	}

	INT confirmState = pMc->GetInt( TEXT("iteminfoid") );
	_ConfirmBox = NULL;
	//
	INT newConfirmState = EWpSklRplcCfmSta_None;
	switch( confirmState )
	{
	case EWpSklRplcCfmSta_Info:
		{
			newConfirmState = EWpSklRplcCfmSta_SideEfct;
		}
		break;
	case EWpSklRplcCfmSta_SideEfct:
		{
			newConfirmState = EWpSklRplcCfmSta_None;
		}
		break;
	}

	ReplaceWeaponSkill( newConfirmState );
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::CancelReplaceWeaponSkill( UGFxObject *pMc )
{
	_ConfirmBox = NULL;
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::TryReplaceWeaponSkill( INT checkConfirmState )
{
	FString errorName = CheckAllConditionsToTry();
	if( !errorName.IsEmpty() ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}

	UBOOL isPopMsgBox = FALSE;
	switch( checkConfirmState )
	{
	case EWpSklRplcCfmSta_TryCnfmRare:
		{
			isPopMsgBox = PopupTryReplaceConfirmRareSkillBox();
		}
		break;
	}

	if( isPopMsgBox ){
		return;
	}

	INT curConsumeItemID = GetCurConsumeID();
	CMessageHandler::GetInstance().Send_TryReplaceWeaponRune( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, curConsumeItemID );
}
//============================================================================================================
UBOOL UMMOUIWeaponSkillRplc::PopupTryReplaceConfirmRareSkillBox()
{
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
	if( pItemInfo == NULL ){
		return FALSE;
	}

	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	FString messageTxt = TEXT("");
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		INT weaponRuneSkillID = mmoEquipMgr.GetWeaponRplcCacheRuneSkillID( i, pItemInfo );
		if( weaponRuneSkillID == 0 ){
			continue;
		}
		//
		_CurRplcWeaponSkill( i ) = weaponRuneSkillID;
		//
		const WeaponSkillCfg *pWeaponSkillCfgData = mmoEquipMgr.GetWeaponSkillCfg( weaponRuneSkillID );
		if( pWeaponSkillCfgData != NULL && pWeaponSkillCfgData->_IsRare ){
			INT skillID = weaponRuneSkillID / 10;
			UBOOL needSkipThis = ( skillID == 2541 );
			if( !needSkipThis ){
				//Filter out this ID, no prompt needed
				//
				INT curWeaponRuneSkillID = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( i, pItemInfo );
				UBOOL hasSameRare = ( curWeaponRuneSkillID == weaponRuneSkillID );

				//
				for( INT j = 0; j < EQUIP_RUNE_SKILL_MAX && !hasSameRare; ++j ){
					if( i == j ){
						continue;
					}

					curWeaponRuneSkillID = mmoEquipMgr.GetWeaponRuneSkillIDBySlotIdx( j, pItemInfo );
					hasSameRare = ( curWeaponRuneSkillID == weaponRuneSkillID );
				}

				if( !hasSameRare ){
					FString BaseKey = TEXT("MMOGame.WeaponSkillRplc.ConfirmChangeWeaponSkillHint");
					messageTxt += MMOUI::ParseLocalizedPropertyPath( BaseKey )+ TEXT("\n");
					messageTxt += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ConfirmChangeWeaponSkillHintEnd") );

					break;
				}
			}//if not skip
		}// if isRare
	}

	if( !messageTxt.IsEmpty() ){
		PopupConfirmBox( messageTxt, EWpSklRplcCfmSta_TryCnfmRare );
		return TRUE;
	}

	return FALSE;
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::ConfirmTryReplaceWeaponSkill( UGFxObject *pMc )
{
	if( _ConfirmBox == NULL ){
		return;
	}

	INT confirmState = pMc->GetInt( TEXT("iteminfoid") );
	_ConfirmBox = NULL;
	//
	INT newConfirmState = EWpSklRplcCfmSta_None;
	switch( confirmState )
	{
	case EWpSklRplcCfmSta_TryCnfmRare:
		{
			newConfirmState = EWpSklRplcCfmSta_None;
		}
		break;
	}

	TryReplaceWeaponSkill( newConfirmState );
}
//---------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::CancelTryReplaceWeaponSkill( UGFxObject *pMc )
{
	_ConfirmBox = NULL;
}
//============================================================================================================
FString UMMOUIWeaponSkillRplc::CheckAllConditionsToTry()
{
	FString errorName = TEXT("");
	INT curCnsmItemID = GetCurConsumeID();
	if( curCnsmItemID < 0 ){
		return TEXT("MMOGame.WeaponSkillRplc.ERROR_NEED_ITEM_INPUT");
	}

	INT itemCnt = MMOFacade::getSingleton().GetItemCountInBag( curCnsmItemID, E_CONSLOT_DEFAULT );
	if( itemCnt <= 0 ){
		return TEXT("MMOGame.WeaponSkillRplc.ERROR_NO_ENOUGH_CONSUMES");
	}

	if( _ToOpWeapon._ConID < 0 || _ToOpWeapon._SlotID < 0 ){
		return TEXT("MMOGame.WeaponSkillRplc.ERROR_NEED_WEAPON_INPUT");
	}

	//
	FClientItemInfo *pCurWeapon = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
	INT weaponRuneCnt = MMOEquipManager::getSingleton().GetWeaponCurSkillSlotCnt( pCurWeapon );
	if( weaponRuneCnt <= 1 ){
		return TEXT("MMOGame.WeaponSkillRplc.errItemNotDoubleRune");
	}

	//
	if( Player ){
		INT curMyMoney = Player->GetMoney( MONEYTYPE_02 );
		if( curMyMoney < RUNE_ITEM_COST_HIGH ){
			return TEXT("MMOGame.WeaponSkillRplc.ERROR_NO_ENOUGH_MONEY");
		}
	}

	return errorName;
}
//============================================================================================================
FString UMMOUIWeaponSkillRplc::CheckAllConditionsToReplace()
{
	UBOOL hasRplcSkill = FALSE;
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();
	
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
	if( pItemInfo == NULL ){
		warnf( NAME_Error, TEXT("CheckAllConditionsToReplace find NO weapon!") );
		return TEXT("MMOGame.WeaponSkillRplc.ERROR_NEED_ITEM_INPUT");
	}

	//
	INT weaponRuneCnt = MMOEquipManager::getSingleton().GetWeaponCurSkillSlotCnt( pItemInfo );
	if( weaponRuneCnt <= 1 ){
		return TEXT("MMOGame.WeaponSkillRplc.errItemNotDoubleRune");
	}

	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		INT weaponRuneSkillID = equipMgr.GetWeaponRplcCacheRuneSkillID( i, pItemInfo );
		if( weaponRuneSkillID > 0 ){
			hasRplcSkill = TRUE;
			break;
		}
	}
	if( !hasRplcSkill ){
		return TEXT("MMOGame.WeaponSkillRplc.WeaponNoReplaceSkills");
	}

	return TEXT("");
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::NotifyReplaceWeaponSkillResult( INT result )
{
	UBOOL hasPlayedAnim = FALSE;
	FString hint;
	FString strString;
	if( result != 0 ){
		MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
		MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
		FString chatHintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ReplaceSuccess") );
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

					eventCallASUpdateWSkillRplcEffect( FALSE, i );

					//
					MessageData *pNewMsgData = new MessageData();
					pNewMsgData->MessageType = ESMT_WeaponSkillGained;
					INT mainSkillID = pWeaponSkill->eventGetAbilityId();
					INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, pWeaponSkill->SpellCommonAttribute.Level, pWeaponSkill->GetRuneSlot() );
					pNewMsgData->MagicValue = uniqueSkillID;

					ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );

					//text tips
					MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *chatHintFormat, *skillName ) );
				}
			}

			if( isHasSkill == FALSE ){
				MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ReplaceFailed") ) );
			}
		}
	}
	else{
		strString = TEXT("MMOGame.WeaponSkillRplc.ReplaceFailed");
		hint = MMOUI::ParseLocalizedPropertyPath( strString );
	}

	//
	if( !hint.IsEmpty() ){
		MMOChatManager::getSingleton().AddSystemMessage( hint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( hint, 1, strString );
	}

	//
	ResetCachedRplcWeaponRune();

	INT cnt = _CurRplcWeaponSkill.Num();
	for( INT i = 0; i < cnt; ++i ){
		_CurRplcWeaponSkill(i) = 0;

	}
	UpdateRplcCacheWeaponSkills();

	//
	UpdateReplaceWeaponSkillBtn();
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::NotifyTryReplaceWeaponSkillResult( INT result )
{
	UBOOL hasPlayedAnim = FALSE;
	FString hint;
	FString strString;
	if( result != 0 ){
		MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
		MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();

		//
		UpdateRplcCacheWeaponSkills();

		//
		FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
		FString chatHintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.TryReplaceSuccess") );
		if( pItemInfo != NULL ){
			UBOOL isHasSkill = FALSE;
			INT defaultPlayIdx = 0;
			UINT curMinRarePri = 0xffffffff;
			for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
			{
				UMMOAbility* pWeaponSkill = mmoEquipMgr.GetWeaponRplcCacheRuneSkillBySlotIdx( i, pItemInfo );
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
				if( weaponSkillID != _CurRplcWeaponSkill( i ) ){
					hasPlayedAnim = TRUE;

					eventCallASUpdateWSkillRplcEffect( TRUE, i );

					////
					//MessageData *pNewMsgData = new MessageData();
					//pNewMsgData->MessageType = ESMT_WeaponSkillGained;
					//INT mainSkillID = pWeaponSkill->eventGetAbilityId();
					//INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, pWeaponSkill->SpellCommonAttribute.Level, pWeaponSkill->GetRuneSlot() );
					//pNewMsgData->MagicValue = uniqueSkillID;

					//ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );

					//text tips
					MMOChatManager::getSingleton().AddSystemMessage(  FString::Printf( *chatHintFormat, *skillName ) );
				}

				//Pick the one with the lowest rarity
				const WeaponSkillCfg *pWeaponSkillCfgData = mmoEquipMgr.GetWeaponSkillCfg( _CurRplcWeaponSkill( i ) );
				if( pWeaponSkillCfgData != NULL && curMinRarePri > pWeaponSkillCfgData->_RarePriority ){
					curMinRarePri = pWeaponSkillCfgData->_RarePriority;
					defaultPlayIdx = i;
				}
			}

			if( isHasSkill == FALSE ){
				MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.ResultFailedChatHint") ) );
			}
			else if( hasPlayedAnim == FALSE ){
				UMMOAbility* pWeaponSkill = mmoEquipMgr.GetWeaponRplcCacheRuneSkillBySlotIdx( defaultPlayIdx, pItemInfo );
				if( pWeaponSkill != NULL ){
					//Name
					FString skillName;
					UMMOAbility *pMainSkill = mmoSkillMgr.GetMainAbilityFor( pWeaponSkill );
					if( pMainSkill != NULL ){
						skillName += pMainSkill->eventGetAbilityName();
					}
					skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pWeaponSkill->eventGetAbilityName() ) );

					//
					eventCallASUpdateWSkillRplcEffect( TRUE, defaultPlayIdx );

					////
					//MessageData *pNewMsgData = new MessageData();
					//pNewMsgData->MessageType = ESMT_WeaponSkillGained;
					//INT mainSkillID = pWeaponSkill->eventGetAbilityId();
					//INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, pWeaponSkill->SpellCommonAttribute.Level, pWeaponSkill->GetRuneSlot() );
					//pNewMsgData->MagicValue = uniqueSkillID;

					//ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );

					//text tips
					MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *chatHintFormat, *skillName ) );
				}
			}
		}
	}
	else{
		strString = TEXT("MMOGame.WeaponSkillRplc.TryReplaceFailed");
		hint = MMOUI::ParseLocalizedPropertyPath( strString );
	}

	//
	if( !hint.IsEmpty() ){
		MMOChatManager::getSingleton().AddSystemMessage( hint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( hint, 1, strString );
	}

	//
	UpdateTryReplaceWeaponSkillBtn();
	UpdateReplaceWeaponSkillBtn();
}
//=================================================================================================================================
void UMMOUIWeaponSkillRplc::GrayAllBagNotEquip( UBOOL isToGray )
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
			mmoFacade.GrayItem( ESGMark_WeaponSkillRplc, E_CONSLOT_DEFAULT, j, FALSE );
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
		mmoFacade.GrayItem( ESGMark_WeaponSkillRplc, E_CONSLOT_DEFAULT, j, isNeedToGray );
	}
}
//
UBOOL UMMOUIWeaponSkillRplc::IsNeedToGray( UItem *pItemInBag, const FClientItemInfo *pItemInfo )
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
void UMMOUIWeaponSkillRplc::UpdateReplaceWeaponSkillBtn()
{
	FString errorName = CheckAllConditionsToReplace();

	eventCallASEnableWSkillRplcCnfmRplc( errorName.IsEmpty() );
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::UpdateTryReplaceWeaponSkillBtn()
{
	FString errorName = CheckAllConditionsToTry();

	eventCallASEnableWSkillRplcTryRplc( errorName.IsEmpty() );
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::Init()
{
	Super::Init();

	//
	_CurWeaponCurSkill.AddZeroed( EQUIP_RUNE_SKILL_MAX );
	_CurRplcWeaponSkill.AddZeroed( EQUIP_RUNE_SKILL_MAX );
	//
	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillRplc.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );
	//
	InitPanelMc();
	//
	_ToOpWeapon._ConID = -1;
	_ToOpWeapon._SlotID = -1;

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_WeaponSkillAdd);
	evts.AddItem(GameEvent_WeaponSkillRplcCacheChanged);
	evts.AddItem(GameEvent_Item_Delete);
	evts.AddItem(GameEvent_SkillUiUpdate);
	evts.AddItem(GameEvent_BagSortEnd);
	EventAdapter = new TGameEventAdapter<UMMOUIWeaponSkillRplc>(this, evts);
}

//============================================================================================================
void UMMOUIWeaponSkillRplc::InitPanelMc()
{
	_WeaponSlotMc = GetObject( TEXT("ToRplcWeaponSlotMc") );
	if( _WeaponSlotMc != NULL ){
		_WeaponSlotMc->SetInt( TEXT("EquipID"), EWpSklRplcSltTp_WeaponOp );
		_WeaponSlotMc->SetInt( TEXT("SlotType"), ESPType_WeaponSkillRplc );
	}

	_ConsumeItemSlotMc = GetObject( TEXT("RplcConsumItemSlotMc") );
	if( _ConsumeItemSlotMc != NULL ){
		//_ConsumeItemSlotMc->SetBool( TEXT("DisableDrag"), TRUE );
		_ConsumeItemSlotMc->SetInt( TEXT("EquipID"), EWpSklRplcSltTp_WpSkillCusm1 );
		_ConsumeItemSlotMc->SetInt( TEXT("SlotType"), ESPType_WeaponSkillRplc );
	}

	////
	//_TryReplaceWSkillBtn = GetObject( TEXT("BtnTryRplace") );
	//_ConfirmReplaceWSkillBtn = GetObject( TEXT("BtnRplace") );
}
//============================================================================================================
//============================================================================================================
void UMMOUIWeaponSkillRplc::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));
}
//============================================================================================================
void UMMOUIWeaponSkillRplc::ASClickToReplaceWeaponSkill()
{
	ReplaceWeaponSkill( EWpSklRplcCfmSta_Info );
}

//============================================================================================================
void UMMOUIWeaponSkillRplc::ASClickToTryReplaceWeaponSkill()
{
	TryReplaceWeaponSkill( EWpSklRplcCfmSta_TryCnfmRare );
}
//===============================================================================================================
void UMMOUIWeaponSkillRplc::ASShowTips_WeaponSkill( UGFxObject *mc )
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
void UMMOUIWeaponSkillRplc::ASHideTips_WeaponSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//===============================================================================================================
void UMMOUIWeaponSkillRplc::ASShowTips_ConsumeItem( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_AwardIcon, mc );
}
//--------------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillRplc::ASHideTips_ConsumeItem()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================
//============================================================================================================