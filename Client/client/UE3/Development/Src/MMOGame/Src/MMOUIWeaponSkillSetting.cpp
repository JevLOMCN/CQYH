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
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIWeaponSkillSetting)

void UMMOUIWeaponSkillSetting::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		//UMMOUIWeaponSkill *pWeaponSkillPanel = (UMMOUIWeaponSkill*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkill );
		//if( pWeaponSkillPanel != NULL ){
		//	_ToOpWeapon._ConID = pWeaponSkillPanel->_ToOpWeapon._ConID;
		//	_ToOpWeapon._SlotID = pWeaponSkillPanel->_ToOpWeapon._SlotID;
		//	_CurOperationSkillSlot = pWeaponSkillPanel->_CurOperationSkillSlot;

		//	ShowWeaponSkillInfoPanel( TRUE, _CurOperationSkillSlot, _ToOpWeapon._ConID, _ToOpWeapon._SlotID )
		//}
	}
	else{
		ResetUI();
	}
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::ShowWeaponSkillInfoPanel( UBOOL isShow, INT skillSlotIdx, INT containerID, INT slotID )
{
	if( isShow ){
		//_WeaponSkillSettingPanelMc->SetVisible( TRUE );
		_ToOpWeapon._ConID = containerID;
		_ToOpWeapon._SlotID = slotID;
		////If the opened info is not for the same weapon ability slot
		//if( _CurOperationSkillSlot >= 0 && ( skillSlotIdx != _CurOperationSkillSlot ) && ( _SkillLevelUpItem._ConID >= 0 && _SkillLevelUpItem._SlotID >= 0 ) ){
		//	UnSetWeaponSkillLevelUpItem();
		//}

		_CurOperationSkillSlot = skillSlotIdx;
		SetInt( TEXT("SkillSlotType"), _CurOperationSkillSlot );
		//
		UpdateWeaponSkillInfoPanel( skillSlotIdx );
	}

	//
	//SetVisible( isShow );
	Show( isShow );
}

//
INT UMMOUIWeaponSkillSetting::GetCurOperationSkillSlot()
{
	return _CurOperationSkillSlot;
}
//============================================================================================================
void UMMOUIWeaponSkillSetting::ResetUI()
{
	//
	_ToOpWeapon._ConID = -1;
	_ToOpWeapon._SlotID = -1;
	_CurOperationSkillSlot = -1;
	SetInt( TEXT("SkillSlotType"), _CurOperationSkillSlot );
	_CurLevelUpRequireOK = FALSE;
	_CurLevelUpConsumeRequireOK = FALSE;
	//
	_CurLevelUpItemID = 0;
	//
	//UnSetWeaponSkillLevelUpItem();

	//
	UpdateWeaponSkillInfoPanel( _CurOperationSkillSlot );

	//Unlock bag inscription item
	if( _SkillReplaceItem._ConID >= 0 && _SkillReplaceItem._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _SkillReplaceItem._ConID, _SkillReplaceItem._SlotID, -1 );
		_SkillReplaceItem._ConID = -1;
		_SkillReplaceItem._SlotID = -1;
	}

	if( _ReplaceWSkillConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ReplaceWSkillConfirmBox );
	}
}

//========================================================================================
void UMMOUIWeaponSkillSetting::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	//case GameEvent_WeaponSkillLevelUp:
	//	{
	//		MMOGameEvent_OneInt *weaponSkillLvUpEvt = (MMOGameEvent_OneInt*)evt;

	//		NotifyLevleUpWeaponSkill( weaponSkillLvUpEvt->IntData );
	//	}
	//	break;
	//case GameEvent_WeaponSkillReplace:
	//	{
	//		MMOGameEvent_OneInt *weaponSkillReplaceEvt = (MMOGameEvent_OneInt*)evt;

	//		NotifyReplaceWeaponSkill( weaponSkillReplaceEvt->IntData );
	//	}
	//	break;
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			if( IsVisible ){
				BagSlotChange( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx );
			}
		}
		break;
	case GameEvent_SkillLevelUp:
		{
			if( IsVisible ){
				UpdateWeaponSkillInfoPanel( _CurOperationSkillSlot );
			}
		}
	}
}
//========================================================================================================================================
void UMMOUIWeaponSkillSetting::BagSlotChange( INT bagId, INT slotId )
{
	if( !IsVisible ){
		return;
	}

	//Weapon changed, refresh weapon abilities
	if( _ToOpWeapon._ConID == bagId && _ToOpWeapon._SlotID == slotId ){
		UpdateWeaponSkillInfoPanel( _CurOperationSkillSlot );
	}

	INT slotHotKey = MMOSkillManager::getSingleton().GetSkillSlotDefaultBindEnum( _CurOperationSkillSlot );
	//Replaced ability item changed
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
	if( pItemInfo != NULL && _CurLevelUpItemID == pItemInfo->m_nTemplateID ){
		UBOOL isSkillActive = FALSE;
		UMMOAbility *weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillByCmdKey( slotHotKey, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
		AutoCheckAndSetWeaponSkillLeveUpItem( weaponSkill );
		//
		UpdateLevelUpBtnState();
	}
	else{
		INT cnt = _SkillLevelUpItems.Num();
		for( INT i = 0; i < cnt; ++i ){
			const FOperationItemBagPos &itemBagPos = _SkillLevelUpItems(i);
			if( itemBagPos._ConID == bagId && itemBagPos._SlotID == slotId ){
				UBOOL isSkillActive = FALSE;
				UMMOAbility *weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillByCmdKey( slotHotKey, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
				AutoCheckAndSetWeaponSkillLeveUpItem( weaponSkill );

				//
				UpdateLevelUpBtnState();
				break;
			}
		}
	}
	//if( _SkillLevelUpItem._ConID == bagId && _SkillLevelUpItem._SlotID == slotId && IsVisible ){
	//	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
	//	if( pItemInfo == NULL ){
	//		UnSetWeaponSkillLevelUpItem();
	//	}
	//	else{
	//		//Reset count
	//		SetWeaponSkillLevelUpItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID );
	//	}

	//	//Attribute UI data
	//	UpdateWeaponSkillInfoPanel( _CurOperationSkillSlot );
	//}
}

//========================================================================================================================================
//void UMMOUIWeaponSkillSetting::PlayerMoneyChange(BYTE MoneyType, int Money, INT OldMoney)
//{
//
//}
//========================================================================================================================================
void UMMOUIWeaponSkillSetting::OnFocusGained()
{
	if( IsVisible ){
		UMMOUIBaseSlotContainer::OnFocusGained();
	}
	else{
		UMMOUIBase::OnFocusGained();
	}
}

//
void UMMOUIWeaponSkillSetting::OnFocusLost()
{
	if( IsVisible ){
		UMMOUIBaseSlotContainer::OnFocusLost();
	}
	else{
		UMMOUIBase::OnFocusLost();
	}
}
//========================================================================================================================================
//
UBOOL UMMOUIWeaponSkillSetting::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIWeaponSkillSetting::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIWeaponSkillSetting::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	//MMOFacade &mmoFacade = MMOFacade::getSingleton();

	//if( slotType == ESPType_Bag ){
	//	//put in from bag
	//	FClientItemInfo *pItemInfo = mmoFacade.FindItem( containerID, slotID );
	//	if( pItemInfo == NULL ){
	//		return TRUE;
	//	}

	//	UItem *pItem = mmoFacade.FindItemTemplate( pItemInfo->m_nTemplateID );
	//	if( pItem == NULL ){
	//		return TRUE;
	//	}
	//	//
	//	ClientActionCondition_SkillSetting checker;

	//	FClientActionConditionContex contex;
	//	contex.Item = pItemInfo;

	//	if( !checker.CheckClientActionCondition(contex) ){
	//		return TRUE;
	//	}

	//	FString errorHint = SetWeaponSkillLevelUpItem( containerID, slotID );
	//	if( !errorHint.IsEmpty() ){
	//		MMOUI::GetUIManager()->delegateAddScreenMessage( *errorHint );
	//	}

	//	return TRUE;
	//}
	//else{
	//	INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") ); 
	//	//put back to bag
	//	if( slotType == ESPType_WeaponSkillSetting ){
	//		if( panelSlotID == EWpSkillSetting_Skill ){
	//			return TRUE;
	//		}
	//		else if( panelSlotID == EWpSkillSetting_SkillLeveUpItem ){
	//			//Weapon ability currently being replaced
	//			UnSetWeaponSkillLevelUpItem();
	//		}

	//		return TRUE;
	//	}

	//	return FALSE;
	//}

	return FALSE;
}

//=================================================================================================
UBOOL UMMOUIWeaponSkillSetting::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}
	//
	if( srcSlotType == ESPType_Bag ){
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( srcContainerID, srcSlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		
		ClientActionCondition_SkillSetting checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		INT panelSlotID = (INT)destSlotMC->GetFloat( TEXT("EquipID") );
		if( destSlotType == ESPType_WeaponSkillSetting ){
			if( panelSlotID == EWpSkillSetting_Skill ){
				//Weapon currently being soul-forged
				ReplaceWeaponSkill( _CurOperationSkillSlot, srcContainerID, srcSlotID );
			}
			//else if( panelSlotID == EWpSkillSetting_SkillLeveUpItem ){
			//	//Place upgrade materials
			//	FString errorHint = SetWeaponSkillLevelUpItem( srcContainerID, srcSlotID );
			//	if( !errorHint.IsEmpty() ){
			//		MMOUI::GetUIManager()->delegateAddScreenMessage( *errorHint );
			//	}
			//}
		}

		return TRUE;
	}
	else if( destSlotType == ESPType_Bag ){
		INT panelSlotID = (INT)srcSlotMC->GetFloat( TEXT("EquipID") );
		if( srcSlotType == ESPType_WeaponSkillSetting ){
			if( panelSlotID == EWpSkillSetting_Skill ){
				return TRUE;
			}
			//else if( panelSlotID == EWpSkillSetting_SkillLeveUpItem ){
			//	//Ability upgrade materials
			//	UnSetWeaponSkillLevelUpItem();
			//}
		}

		return TRUE;
	}

	return FALSE;
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::UpdateWeaponSkillInfoPanel( INT skillSlotIdx )
{
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();

	_CurLevelUpRequireOK = FALSE;

	UBOOL isSkillActive = FALSE;
	INT slotHotKey = MMOSkillManager::getSingleton().GetSkillSlotDefaultBindEnum( skillSlotIdx );
	UMMOAbility *weaponSkill = mmoEquipMgr.GetWeaponRuneSkillByCmdKey( slotHotKey, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );

	//Current soul value
	INT curSoulPoints = Player->GetMoney( E_MONEYTYPE_SOUL_PTS );
	if( _RequireMoneyCurConditionTf != NULL ){
		_RequireMoneyCurConditionTf->SetString( TEXT("text"), appItoa( curSoulPoints ) );
	}

	//Upgrade materials
	AutoCheckAndSetWeaponSkillLeveUpItem( weaponSkill );

	if( !isSkillActive || weaponSkill == NULL || weaponSkill->MainSkill == NULL ){
		//Not yet activated or no main ability
		GFXPlayer->eventCallSetItemData( _CurWeaponSkillSlot, 0, TEXT(""), -1 , ESPType_WeaponSkillSetting, TEXT("") );
		_WeaponSkillNameTf->SetString( TEXT("text"), TEXT("") );
		_CurSkillLvTf->SetString( TEXT("text"), TEXT("") );
		_NextSkillLvTf->SetString( TEXT("text"), TEXT("") );
		_CurConditionTf->SetString( TEXT("text"), TEXT("") );
		_RequireConditionTf->SetString( TEXT("htmlText"), TEXT("") );

		return;
	}

	//Ability icon
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( weaponSkill->eventGetAbilityId(), weaponSkill->SpellCommonAttribute.Level, weaponSkill->GetRuneSlot() );
	GFXPlayer->eventCallSetItemData( _CurWeaponSkillSlot , 0 , weaponSkill->GetAbilityIconName() , uniqueSkillID, ESPType_WeaponSkillSetting ,TEXT("") );

	//Ability name
	FString skillName;
	skillName = weaponSkill->ClientCommonAttribute.AbilityName;
	if( weaponSkill->MainSkill != NULL ){
		skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ActiveSkillNameLinkChar") ), *weaponSkill->MainSkill->ClientCommonAttribute.AbilityName );
	}
	_WeaponSkillNameTf->SetString( TEXT("text"), skillName );

	//Current ability level
	INT curSkillLv = weaponSkill->SpellCommonAttribute.Level;
	_CurSkillLvTf->SetString( TEXT("text"), appItoa( curSkillLv ) );

	//Upgrade target ability level
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	UMMOAbility *nextLvSkill = mmoSkillMgr.GetAbilityWithRuneByNextLevel( weaponSkill );
	UBOOL isMaxLv = ( nextLvSkill == NULL );
	if( isMaxLv ){
		_NextSkillLvTf->SetString( TEXT("text"), TEXT("--") );
	}
	else{
		_NextSkillLvTf->SetString( TEXT("text"), appItoa( curSkillLv + 1 ) );
	}

	//Current main ability level
	FString curConditionText;
	const UMMOAbility *myLearnedMainSkill = mmoSkillMgr.FindMyLearnedMainSkill( weaponSkill->eventGetAbilityId() );
	INT curParentSkillLv = 0;
	if( myLearnedMainSkill != NULL ){
		curParentSkillLv = myLearnedMainSkill->SpellCommonAttribute.Level;
		curConditionText = weaponSkill->MainSkill->eventGetAbilityName();
		curConditionText += appItoa( curParentSkillLv );
		curConditionText += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.SkillLvPostfix") );
	}
	else{
		curConditionText = weaponSkill->MainSkill->eventGetAbilityName();
		curConditionText += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_MainSkillNotLearnd") );
	}
	_CurConditionTf->SetString( TEXT("text"), curConditionText );

	//Required main ability level
	FString requireText = weaponSkill->MainSkill->eventGetAbilityName();
	requireText += appItoa( curSkillLv + 1 );
	requireText += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.SkillLvPostfix") );

	if( curParentSkillLv < curSkillLv + 1 ){
		//_RequireConditionTf->SetInt( TEXT("textColor"), requireText );
		requireText = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")) ) + requireText + UITextParser::HTMLEndFont();
		_CurLevelUpRequireOK = FALSE;
	}
	else{
		//_RequireConditionTf->SetInt( TEXT("textColor"), requireText );
		requireText = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("NormalContentColor")) ) + requireText + UITextParser::HTMLEndFont();
		_CurLevelUpRequireOK = TRUE;
	}
	_RequireConditionTf->SetString( TEXT("htmlText"), requireText );

	//
	UpdateLevelUpBtnState();

	//
	_MaxLevelTf->SetVisible( isMaxLv );
	_LevelUpBtn->SetVisible( !isMaxLv );
	_CurSkillLvTf->SetVisible( !isMaxLv );
	_NextSkillLvTf->SetVisible( !isMaxLv );
	_CurConditionTf->SetVisible( !isMaxLv );
	_RequireConditionTf->SetVisible( !isMaxLv );
	//_ReplaceRequireConditionTf->SetVisible( !isMaxLv );
	//_ReplaceCurConditionTf->SetVisible( !isMaxLv );
	_WeaponSkillLevelUpSlot->SetVisible( !isMaxLv );
	_StaticTextContainerMc->SetVisible( !isMaxLv );
	_WeaponSkillLevelUpCntTf->SetVisible( !isMaxLv );
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::AutoCheckAndSetWeaponSkillLeveUpItem( UMMOAbility *weaponSkill )
{
	//cleanup
	_SkillLevelUpItems.Empty();
	if( _WeaponSkillLevelUpCntTf != NULL ){
		_WeaponSkillLevelUpCntTf->SetString( TEXT("htmlText"), TEXT("") );
	}
	if( _RequireMoneyConditionTf != NULL ){
		_RequireMoneyConditionTf->SetString( TEXT("htmlText"), TEXT("--") );
	}

	if( weaponSkill == NULL ){
		GFXPlayer->eventCallSetItemData( _WeaponSkillLevelUpSlot , 0 , TEXT("") , -1, ESPType_WeaponSkillSetting, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	//Get inscription upgrade cost data
	INT targetSkillLv = weaponSkill->SpellCommonAttribute.Level + 1;
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( weaponSkill->eventGetAbilityId(), targetSkillLv, weaponSkill->GetRuneSlot() );
	RuneSkillLevelUpConsum *runeSkillLevelUpItemData = MMOSkillManager::getSingleton().GetRuneSkillLevelUpConsum( uniqueSkillID );
	if( runeSkillLevelUpItemData == NULL ){
		GFXPlayer->eventCallSetItemData( _WeaponSkillLevelUpSlot , 0 , TEXT("") , -1, ESPType_WeaponSkillSetting, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		return;
	}

	//Set icon
	_CurLevelUpItemID = runeSkillLevelUpItemData->_ItemID;
	UItem *pItemToHelpLevelUp = mmoFacade.FindItemTemplate( _CurLevelUpItemID );
	if( pItemToHelpLevelUp == NULL ){
		GFXPlayer->eventCallSetItemData( _WeaponSkillLevelUpSlot , 0 , TEXT("") , -1, ESPType_WeaponSkillSetting, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		return;
	}
	GFXPlayer->eventCallSetItemData( _WeaponSkillLevelUpSlot, 1, mmoFacade.GetItemIconNamePersonalized( pItemToHelpLevelUp ), pItemToHelpLevelUp->ItemId, ESPType_WeaponSkillSetting, TEXT(""), pItemToHelpLevelUp->ItemColor+1, -1, -1 );

	//Set count
	FString cntString;
	INT levelUpNeedItemCnt = runeSkillLevelUpItemData->_ItemCnt;
	//Whether max level
	if( levelUpNeedItemCnt > 0 ){
		INT totalHaveCnt = 0;
		//Iterate the bag to find the count of this kind of item
		UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
		for( UINT j = 0; j < bagCapability; ++j){
			const FClientItemInfo *pItemInfo = mmoFacade.FindItem( E_CONSLOT_DEFAULT, j );
			//if found, add the amount
			if( pItemInfo != NULL && pItemInfo->m_nTemplateID == _CurLevelUpItemID ){
				totalHaveCnt += pItemInfo->m_nCurDur;

				FOperationItemBagPos newPos;
				newPos._ConID = E_CONSLOT_DEFAULT;
				newPos._SlotID = j;
				_SkillLevelUpItems.Push( newPos );
			}
		}
		//Whether enough; mark red if insufficient
		if( totalHaveCnt < levelUpNeedItemCnt ){
			cntString = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")));
			//
			_CurLevelUpConsumeRequireOK = FALSE;
		}
		else{
			cntString = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("NormalContentColor")));
			//
			_CurLevelUpConsumeRequireOK = TRUE;
		}
		cntString += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ItemCntLink") ), levelUpNeedItemCnt, totalHaveCnt );
		cntString += UITextParser::HTMLEndFont();
	}

	if( _WeaponSkillLevelUpCntTf != NULL ){
		_WeaponSkillLevelUpCntTf->SetString( TEXT("htmlText"), cntString );
	}

	//
	//UpdateLevelUpBtnState();

	////Required money
	//if( _RequireMoneyConditionTf != NULL ){
	//	_RequireMoneyConditionTf->SetString( TEXT("htmlText"), cntString );
	//}
}

//--------------------------------------------------------------
void UMMOUIWeaponSkillSetting::UpdateLevelUpBtnState()
{
	UBOOL isOK = _CurLevelUpRequireOK && _CurLevelUpConsumeRequireOK;
	_LevelUpBtn->SetBool( TEXT("enabled"), isOK );
}
//============================================================================================================
void UMMOUIWeaponSkillSetting::UnSetWeaponSkillLevelUpItem()
{
	SetWeaponSkillLevelUpItem( -1, -1 );
}

//---------------------------------------------------------------------
FString UMMOUIWeaponSkillSetting::SetWeaponSkillLevelUpItem( INT containerID, INT slotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();

	FString rstHint;
	//UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	if( containerID < 0 || slotID < 0 ){
		if( _SkillLevelUpItem._ConID >= 0 && _SkillLevelUpItem._SlotID >= 0 ){
			//
			//if( pBagPanel != NULL && _SkillLevelUpItem._ConID == E_CONSLOT_DEFAULT ){
				MMOFacade::getSingleton().LockItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID, -1 );
			//}

			//GFXPlayer->SetItemSlotData( _ToEmbedEquipMc, NULL, ESPType_SoulEmbed, -1, -1 );
			GFXPlayer->eventCallSetItemData( _WeaponSkillLevelUpSlot , 0 , TEXT("") , -1, ESPType_WeaponSkillSetting, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		_SkillLevelUpItem._ConID = -1;
		_SkillLevelUpItem._SlotID = -1;

		//refresh related display
		if( _WeaponSkillLevelUpCntTf != NULL ){
			_WeaponSkillLevelUpCntTf->SetString( TEXT("htmlText"), TEXT("") );
		}

		return rstHint;
	}

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );
	if( pInfo == NULL ){
		rstHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") );
		return rstHint;
	}

	//Whether a soul-forged weapon is placed in the UI
	if( _ToOpWeapon._ConID < 0 ||  _ToOpWeapon._SlotID < 0 ){
		rstHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_NEED_WEAPON_INPUT") );
		return rstHint;
	}

	//Ability of the soul-forged weapon in this slot
	UBOOL isSkillActive = FALSE;
	INT slotHotKey = MMOSkillManager::getSingleton().GetSkillSlotDefaultBindEnum( _CurOperationSkillSlot );
	UMMOAbility *weaponSkill = mmoEquipMgr.GetWeaponRuneSkillByCmdKey( slotHotKey, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
	if( weaponSkill == NULL ){
		rstHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_WEAPON_SKILL_NOT_EXIST") );
		return rstHint;
	}

	//Get inscription upgrade cost data
	INT targetSkillLv = weaponSkill->SpellCommonAttribute.Level + 1;
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( weaponSkill->eventGetAbilityId(), targetSkillLv, weaponSkill->GetRuneSlot() );
	RuneSkillLevelUpConsum *runeSkillLevelUpItemData = MMOSkillManager::getSingleton().GetRuneSkillLevelUpConsum( uniqueSkillID );
	if( runeSkillLevelUpItemData == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIWeaponSkillSetting::SetWeaponSkillLevelUpItem GetRuneSkillLevelUpConsum[%d-%d-%d] NOT Found]"), weaponSkill->eventGetAbilityId(), weaponSkill->SpellCommonAttribute.Level, weaponSkill->GetRuneSlot() );
		//Max level
		rstHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_MAX") );
		return rstHint;
	}

	//Whether it is a dark attribute stone
	if(pInfo->m_nTemplateID != runeSkillLevelUpItemData->_ItemID ){
		UItem* pItem = mmoFacade.FindItemTemplate( runeSkillLevelUpItemData->_ItemID );
		rstHint = TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NOT_A_SUITABLE_ITEM"); 
		if( pItem != NULL ){
			FString itemName = pItem->GetDisplayName();
			rstHint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NOT_A_SUITABLE_ITEM") ), *itemName );
		}
		
		return rstHint;
	}

	//unlock the equipment previously placed in the UI from the bag
	if( _SkillLevelUpItem._ConID >= 0 && _SkillLevelUpItem._SlotID >= 0 ){
		//if( pBagPanel != NULL && _SkillLevelUpItem._ConID == E_CONSLOT_DEFAULT){
			MMOFacade::getSingleton().LockItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID, -1 );
		//}
	}

	_SkillLevelUpItem._ConID = containerID;
	_SkillLevelUpItem._SlotID = slotID;

	//set UI ItemSlot
	//Calculate count
	//if( !WeaponSkillLevelUpItemCnt.IsValidIndex( weaponSkill->SpellCommonAttribute.Level - 1 ) ){
	//	//All considered max level
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_MAX") ) );
	//	return FALSE;
	//}
	////
	//UINT levelUpNeedItemCnt = WeaponSkillLevelUpItemCnt( weaponSkill->SpellCommonAttribute.Level - 1 );
	INT levelUpNeedItemCnt = runeSkillLevelUpItemData->_ItemCnt;

	//Whether max level
	if( levelUpNeedItemCnt <= 0 ){
		rstHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_MAX") );
		return rstHint;
	}

	if( pInfo->m_nCurDur < levelUpNeedItemCnt ){
		UItem* pItem = mmoFacade.FindItemTemplate( runeSkillLevelUpItemData->_ItemID );
		if( pItem != NULL ){
			FString itemName = pItem->GetDisplayName();
			rstHint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NEED_ITEM") ), levelUpNeedItemCnt, *itemName );
		}
		//Can be placed, but the upgrade will fail
	}
	//UINT itemCnt = ( pInfo->m_nCurDur < levelUpNeedItemCnt ? pInfo->m_nCurDur : levelUpNeedItemCnt );
	//debugf( TEXT("===============[UMMOUIWeaponSkillSetting::SetWeaponSkillLevelUpItem] %d->%d"), pInfo->m_nCurDur, itemCnt );
	FString cntString;
	if( pInfo->m_nCurDur < levelUpNeedItemCnt ){
		cntString = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")));
	}
	else{
		cntString = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("NormalContentColor")));
	}
	cntString += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ItemCntLink") ), pInfo->m_nCurDur, levelUpNeedItemCnt );
	cntString += UITextParser::HTMLEndFont();
	if( _WeaponSkillLevelUpCntTf != NULL ){
		_WeaponSkillLevelUpCntTf->SetString( TEXT("htmlText"), cntString );
	}

	//set
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	GFXPlayer->eventCallSetItemData( _WeaponSkillLevelUpSlot, 1, mmoFacade.GetItemIconNamePersonalized( pItemToOperation, pInfo ), pItemToOperation->ItemId, 
		ESPType_WeaponSkillSetting, TEXT(""), pItemToOperation->ItemColor+1, _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID );

	//lock the spirit previously placed in the bag
	//if( pBagPanel != NULL ){
		MMOFacade::getSingleton().LockItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID, ESPType_WeaponSkill );
	//}

	//refresh related display

	return rstHint;
}




//============================================================================================================
void UMMOUIWeaponSkillSetting::LevleUpWeaponSkill( INT skillSlotIdx )
{
	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_WeaponSkillLvUp);
	MMOUI::PostGameEvent(&evtGuide);

	//
	UBOOL isOK = UpdateLevelUpStateAndHint( skillSlotIdx );
	if( !isOK ){
		return;
	}

	//
	//CMessageHandler::GetInstance().Send_OpenSkillSlot( _ToOpWeapon._ConID, _ToOpWeapon._SlotID, skillSlotIdx );
	warnf( NAME_Error, TEXT("======!!!!======= [UMMOUIWeaponSkillSetting::LevleUpWeaponSkill no server function " ) );
}

//-----------------------------------------------------------------------------------------------------------
UBOOL UMMOUIWeaponSkillSetting::UpdateLevelUpStateAndHint( INT skillSlotIdx )
{
	//Check whether there is a weapon to upgrade
	if( _ToOpWeapon._ConID < 0 || _ToOpWeapon._SlotID < 0 ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET"));
		return FALSE;
	}

	//Check the count of consumables (dark attribute stones)
	UBOOL isSkillActive = FALSE;
	INT slotHotKey = MMOSkillManager::getSingleton().GetSkillSlotDefaultBindEnum( skillSlotIdx );
	UMMOAbility *weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillByCmdKey( slotHotKey, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
	if( weaponSkill == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET") ),MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET") );
		return FALSE;
	}

	//UINT cntIdx = weaponSkill->SpellCommonAttribute.Level - 1;
	//if( !WeaponSkillLevelUpItemCnt.IsValidIndex( cntIdx ) ){
	//	//All considered max level
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_MAX") ) );
	//	return;
	//}
	//INT needCnt = WeaponSkillLevelUpItemCnt( cntIdx );

	INT targetSkillLv = weaponSkill->SpellCommonAttribute.Level + 1;
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( weaponSkill->eventGetAbilityId(), targetSkillLv, weaponSkill->GetRuneSlot() );
	RuneSkillLevelUpConsum *runeSkillLevelUpItemData = MMOSkillManager::getSingleton().GetRuneSkillLevelUpConsum( uniqueSkillID );
	if( runeSkillLevelUpItemData == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET") );
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIWeaponSkillSetting::SetWeaponSkillLevelUpItem GetRuneSkillLevelUpConsum[%d-%d-%d] NOT Found]"), weaponSkill->eventGetAbilityId(), weaponSkill->SpellCommonAttribute.Level, weaponSkill->GetRuneSlot() );
		return FALSE;
	}
	INT needCnt = runeSkillLevelUpItemData->_ItemCnt;
	UItem *pLevelUpConsumItem = MMOFacade::getSingleton().FindItemTemplate( runeSkillLevelUpItemData->_ItemID );
	if( pLevelUpConsumItem == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIWeaponSkillSetting::SetWeaponSkillLevelUpItem FindItemTemplate[%d] NOT Found]"), runeSkillLevelUpItemData->_ItemID );
		return FALSE;
	}
	//Whether max level
	if( needCnt <= 0 ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_MAX") ),MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_MAX"));
		return FALSE;
	}

	//Whether upgrade requirements are met
	if( !_CurLevelUpRequireOK ){
		const UMMOAbility *requireMainSkill = weaponSkill->MainSkill;
		if( requireMainSkill != NULL ){
			INT requireMainSkillLv = weaponSkill->SpellCommonAttribute.Level + 1;
			FString hintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_REQUIRE_SKILL_LEVEL") );
			FString hint = FString::Printf( *hintFormat, requireMainSkillLv, *requireMainSkill->ClientCommonAttribute.AbilityName );
			MMOUI::GetUIManager()->delegateAddScreenMessage( *hint ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_REQUIRE_SKILL_LEVEL"));
		}
		return FALSE;
	}

	//Check whether there are items to upgrade
	//if( _SkillLevelUpItem._ConID < 0 || _SkillLevelUpItem._SlotID < 0 ){
	//	FString hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NEED_ITEM") );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( FString::Printf( *hint, needCnt, *pLevelUpConsumItem->GetDisplayName() ) );
	//	return;
	//}

	////Check whether there are enough items for the upgrade
	//FClientItemInfo *pInfo = MMOFacade::getSingleton().FindItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID );
	//if( pInfo == NULL ){
	//	return;
	//}
	//
	//if( pInfo->m_nCurDur < needCnt ){
	//	FString hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NEED_ITEM") );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( FString::Printf( *hint, needCnt, *pLevelUpConsumItem->GetDisplayName() ) );
	//	return;
	//}
	INT cnt = _SkillLevelUpItems.Num();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	INT totalHaveCnt = 0;
	for( INT i = 0; i < cnt; ++i ){
		const FOperationItemBagPos &itemBagPos = _SkillLevelUpItems(i);
		const FClientItemInfo *pItemInfo = mmoFacade.FindItem( itemBagPos._ConID, itemBagPos._SlotID );
		if( pItemInfo != NULL ){
			totalHaveCnt += pItemInfo->m_nCurDur;
		}
	}
	if( totalHaveCnt < needCnt ){
		FString hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NEED_ITEM") );
		MMOUI::GetUIManager()->delegateAddScreenMessage( FString::Printf( *hint, needCnt, *pLevelUpConsumItem->GetDisplayName() ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_LEVEL_UP_NEED_ITEM"));
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillSetting::NotifyLevleUpWeaponSkill( INT result )
{
	if( result ){
		if( IsVisible ){
			//FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID );
			//if( pItemInfo == NULL ){
			//	UnSetWeaponSkillLevelUpItem();
			//}
			//else{
			//	FString errorHint = SetWeaponSkillLevelUpItem( _SkillLevelUpItem._ConID, _SkillLevelUpItem._SlotID );
			//	if( !errorHint.IsEmpty() ){
			//		MMOUI::GetUIManager()->delegateAddScreenMessage( errorHint );
			//	}
			//}
			//Attribute UI data
			UpdateWeaponSkillInfoPanel( _CurOperationSkillSlot );
		}
		////Refresh abilities; abilities follow the weapon, and an equipped weapon cannot have its weapon ability upgraded, so no refresh needed
		//UBOOL isSkillActive = FALSE;
		//UMMOAbility *weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillByCmdKey( _CurOperationSkillSlot, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
		//if( weaponSkill != NULL && Player != NULL ){
		//	INT weaponSkillID = weaponSkill->eventGetAbilityId();
		//	UMMOAbility *curEquipedSkill = Player->GetEquipedAbilityById( weaponSkillID );

		//	INT weaponSkillRuneID = weaponSkill->GetRuneSlot();
		//	INT curEquippedSkillRuneID = curEquipedSkill->GetRuneSlot();
		//	if( curEquipedSkill != NULL && weaponSkillRuneID == curEquippedSkillRuneID ){
		//		//This ability is equipped, needs refresh after a successful upgrade
		//		UMMOUISkillBar* pSkillBar = (UMMOUISkillBar*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Skillbar);
		//		if( pSkillBar != NULL ){
		//			pSkillBar->UpdateSkillSlot( curEquipedSkill->SlotID );
		//		}
		//	}
		//}
	}
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::ReplaceWeaponSkill( INT skillSlotIdx, INT containerID, INT slotID )
{
	if( _ReplaceWSkillConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ReplaceWSkillConfirmBox );
	}

	//
	UBOOL isOK = CheckReplaceWeaponSkill( skillSlotIdx, containerID, slotID );
	if( isOK != TRUE ){
		return;
	}

	//Prompt to confirm replacement
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT( "OnConfirmReplaceWeaponSkill" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT( "OnCancelReplaceWeaponSkill" );

	FString msgText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ReplaceWeaponSkillConfirmText") );
	_ReplaceWSkillConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgText, skillSlotIdx, funYes, funNo );
	//if( _ReplaceWSkillConfirmBox != NULL ){
	//	_ReplaceWSkillConfirmBox->SetInt( TEXT("iteminfoid") , containerID );
	//	_ReplaceWSkillConfirmBox->SetInt( TEXT("itemdataid") , slotID );
	//}
	_SkillReplaceItem._ConID = containerID;
	_SkillReplaceItem._SlotID = slotID;
	//Lock bag inscription item
	if( _SkillReplaceItem._ConID >= 0 && _SkillReplaceItem._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _SkillReplaceItem._ConID, _SkillReplaceItem._SlotID, ESPType_WeaponSkillSetting );
	}
}

//
UBOOL UMMOUIWeaponSkillSetting::CheckReplaceWeaponSkill( INT skillSlotIdx, INT containerID, INT slotID )
{
	//
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();

	//set equipment
	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );
	if( pInfo == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_ITEM_NOT_FOUND"));
		return FALSE;
	}

	//Whether it is an ability item
	UItem *pItemToOperation = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	if( pItemToOperation == NULL || pItemToOperation->ItemType != TEXT("ITEMTYPE_RUNE") ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_SKILL_RUNE") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_SKILL_RUNE"));
		return FALSE;
	}
	UMMOAbility *itemAbility = pItemToOperation->GetToUseAbility();
	if( itemAbility == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_SKILL_RUNE") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_NOT_A_SKILL_RUNE"));
		return FALSE;
	}

	//
	UBOOL isSkillActive = FALSE;
	UMMOAbility *weaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( skillSlotIdx, _ToOpWeapon._ConID, _ToOpWeapon._SlotID, isSkillActive );
	if( weaponSkill == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_SETTING_NEED_TARGET"));
		return FALSE;
	}
	//Whether the inscription ability meets class requirements
	//if( itemAbility->SpellCommonAttribute.Class != pItemToOperation->RequireClass ){
	FClientItemInfo *pWeaponInfo = mmoFacade.FindItem( _ToOpWeapon._ConID, _ToOpWeapon._SlotID );
	if( pWeaponInfo == NULL ){
		return FALSE;
	}
	UItem *pWeaponItem = mmoFacade.FindItemTemplate( pWeaponInfo->m_nTemplateID );

	const FSpell_ClassType &newReqTyp = itemAbility->SpellCommonAttribute.Class;
	const FSpell_ClassType &useReqTyp = pWeaponItem->RequireClass;
	
	if( MMO::IsClassTypeMatch(newReqTyp, useReqTyp) )
	{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_REPLACE_NOT_FIT_CLASS") ) ,MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_REPLACE_NOT_FIT_CLASS") );
			return FALSE;
	}
	//Cannot replace in an ability that cannot bind this key
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	INT slotHotKey = mmoSkillMgr.GetSkillSlotDefaultBindEnum( skillSlotIdx );
	UBOOL isBindInThis = mmoSkillMgr.IsSkillBindIn( itemAbility->SpellCommonAttribute.BindSlot, slotHotKey );
	if( !isBindInThis ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ERROR_SKILL_REPLACE_NOT_FIT_HOTKEY") ),MsgInfoType_Table,TEXT("MMOGame.WeaponSkill.ERROR_SKILL_REPLACE_NOT_FIT_HOTKEY") );
		return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillSetting::OnConfirmReplaceWeaponSkill( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}
	_ReplaceWSkillConfirmBox = NULL;

	//
	INT containerID = _SkillReplaceItem._ConID;
	INT slotID = _SkillReplaceItem._SlotID;
	UBOOL isOK = CheckReplaceWeaponSkill( _CurOperationSkillSlot, containerID, slotID );
	if( isOK != TRUE ){
		return;
	}
	//
	//CMessageHandler::GetInstance().Send_AddSkillRune(  _ToOpWeapon._ConID, _ToOpWeapon._SlotID, _CurOperationSkillSlot, containerID, slotID );
	warnf( NAME_Error, TEXT("======!!!!======= [UMMOUIWeaponSkillSetting::OnConfirmReplaceWeaponSkill no server function " ) );
}

//-----------------------------------------------------------------------------------------------------------
void UMMOUIWeaponSkillSetting::OnCancelReplaceWeaponSkill( UGFxObject *mc )
{
	_ReplaceWSkillConfirmBox = NULL;

	//Unlock bag inscription item
	if( _SkillReplaceItem._ConID >= 0 && _SkillReplaceItem._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _SkillReplaceItem._ConID, _SkillReplaceItem._SlotID, -1 );
		_SkillReplaceItem._ConID = -1;
		_SkillReplaceItem._SlotID = -1;
	}

	return;
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::NotifyReplaceWeaponSkill( INT result )
{
	if( _SkillReplaceItem._ConID >= 0 && _SkillReplaceItem._SlotID >= 0 ){
		MMOFacade::getSingleton().LockItem( _SkillReplaceItem._ConID, _SkillReplaceItem._SlotID, -1 );
		_SkillReplaceItem._ConID = -1;
		_SkillReplaceItem._SlotID = -1;
	}

	FString hint;
	FString strString;
	if( result ){
		//Success
		hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ReplaceWeaponSkillSuccess") );
		strString = TEXT("MMOGame.WeaponSkill.ReplaceWeaponSkillSuccess");
		//Data refresh is done in the function that listens to bag items
	}
	else{
		hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ReplaceWeaponSkillFailed") );
		strString = TEXT("MMOGame.WeaponSkill.ReplaceWeaponSkillFailed");
	}

	if( !hint.IsEmpty() ){
		MMOChatManager::getSingleton().AddSystemMessage( hint );
		MMOUI::GetUIManager()->delegateAddScreenMessage( hint ,MsgInfoType_Table,strString);
	}
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::Init()
{
	Super::Init();

	TArray<struct FASValue> asFunArgs;
	FASValue asTitleName(EC_EventParm);
	asTitleName.Type = AS_String;
	asTitleName.S = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkillSetting.UITitleName") );
	asFunArgs.AddItem( asTitleName );
	this->Invoke( TEXT("SetWindowTitle"), asFunArgs );

	//
	_ReplaceWSkillConfirmBox = NULL;

	//
	InitWeaponSkillSettingPanelMc();

	//
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_WeaponSkillLevelUp);
	//evts.AddItem(GameEvent_WeaponSkillReplace);
	evts.AddItem(GameEvent_Item_Delete);
	evts.AddItem(GameEvent_SkillLevelUp);
	EventAdapter = new TGameEventAdapter<UMMOUIWeaponSkillSetting>(this, evts);
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::InitWeaponSkillSettingPanelMc()
{
	_CurWeaponSkillSlot = GetObject( TEXT("CurWeaponSkillSlot") );
	_CurWeaponSkillSlot->SetInt( TEXT("EquipID"), EWpSkillSetting_Skill );
	_CurWeaponSkillSlot->SetInt( TEXT("SlotType"), ESPType_WeaponSkillSetting );
	_CurWeaponSkillSlot->SetBool( TEXT("DisableDrag"), TRUE );

	_WeaponSkillNameTf = GetObject( TEXT("WeaponSkillNameTf") );

	_MaxLevelTf = GetObject( TEXT("MaxLevelTf") );
	_MaxLevelTf->SetString( TEXT("text"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.MaxWeaponsSkillLevelTfText") ) );

	_PanelHintTf = GetObject( TEXT("PanelHintTf") );
	if( _PanelHintTf != NULL ){
		_PanelHintTf->SetString( TEXT("htmlText"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.WepaonSkillSettingPanelHintHtml") ) );
	}

	_StaticTextContainerMc = GetObject( TEXT("StaticTextContainerMc") );
	_CurSkillLvTf = GetObject( TEXT("CurSkillLvTf") );
	_NextSkillLvTf = GetObject( TEXT("NextSkillLvTf") );

	_RequireConditionTf = GetObject( TEXT("RequireConditionTf") );
	_CurConditionTf = GetObject( TEXT("CurConditionTf") );

	_RequireMoneyConditionTf = GetObject( TEXT("RequireMoneyConditionTf") );
	_RequireMoneyConditionTf->SetVisible( FALSE );
	_RequireMoneyCurConditionTf = GetObject( TEXT("RequireMoneyCurConditionTf") );
	_RequireMoneyCurConditionTf->SetVisible( FALSE );

	_LevelUpBtn = GetObject( TEXT("LevelUpBtn") );
	_LevelUpBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.LevelUpBtnText") ) );

	_WeaponSkillLevelUpSlot = GetObject( TEXT("WeaponSkillLevelUpSlotMc") );
	_WeaponSkillLevelUpSlot->SetInt( TEXT("EquipID"), EWpSkillSetting_SkillLeveUpItem );
	_WeaponSkillLevelUpSlot->SetInt( TEXT("SlotType"), ESPType_WeaponSkillSetting );
	_WeaponSkillLevelUpSlot->SetBool( TEXT("DisableDrag"), TRUE );

	_WeaponSkillLevelUpCntTf = GetObject( TEXT("WeaponSkillLevelUpCntTf") );
}

//============================================================================================================
void UMMOUIWeaponSkillSetting::NativeInitLoad()
{	
	Super::NativeInitLoad();

	g_layoutList.push_back( new UILayoutRightLeft( this, MMOUI::GetUIManager()->Panel_WeaponSkill ) );
}

//===============================================================================================================
void UMMOUIWeaponSkillSetting::ASLevleUpWeaponSkill( INT skillSlotIdx )
{
	LevleUpWeaponSkill( skillSlotIdx );
}