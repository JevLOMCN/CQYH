#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOEquipManager.h"
#include "MMOFacade.h"
#include "MMOSkillManager.h"
#include "MMOMsgBoxManager.h"
#include "ClientActionConditionCheck.h"
#include "MMOAttributeAccountManager.h"
#include "MMOQuestManager.h"
#include "MMOChatManager.h"
#include "MMOClientVariableManager.h"
#include "MMODataManager.h"

//=================================================================================================================================
EquipSuitPart::EquipSuitPart():
_ItemID(-1),
_EquipSlot(0)
{
}

const FString &EquipSuitPart::GetDisplayName() const
{
	//if( _Name.IsEmpty() ){
	//	UItem* pEquip = MMOFacade::getSingleton().FindItemTemplate( _ItemID );
	//	if( pEquip != NULL ){
	//		_Name = pEquip->GetDisplayName();
	//	}
	//}

	return _Name;
}
//=================================================================================================================================
EquipSuitProp::EquipSuitProp():
_PropID(-1),
_Condition(0)
{
}

EquipSuit::EquipSuit():
_ID(-1),
_PartsCnt(0),
_PropsCnt(0)
{
}

SoulInfo::SoulInfo():
_ID(-1),
_AttrID(-1),
_MatchSocket(0),
_SoulLevel(0),
_PriceValue(0),
_UpgradeSoulID(0),
_DetouchPrice(0)
{
}

MMOEquipManager::MMOEquipManager()
{
	_IsAutoChangeSkills = FALSE;
	_IsAllowedChangeSkills = FALSE;
	_ConfirmEquipWeaponSkill = NULL;
}

MMOEquipManager::~MMOEquipManager()
{

}

void MMOEquipManager::Initialize()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::Initialize"), 0);

	//Set
	ReadEquipSuit();

	//Spirit fusion
	ReadSoulInfoTemplates();
	//Inscription (must be read before the item table, so it is loaded in MMOFacade)
	//ReadWeaponRuneSkillData();

	//Weapon upgrade
	//EquipLevelUpDataManager::getSingleton().InitFromFile( TEXT("equip_levelup.txt") );
	//ReadEquipLevelUpData();//no longer needed

	//
	//GConfig->GetBool(TEXT("Skill"), TEXT("AutoChangeSkill"), _IsAutoChangeSkills, GGameIni);
	_IsAutoChangeSkills = TRUE;
	_ConfirmEquipWeaponSkill = NULL;

	GConfig->GetBool(TEXT("Skill"), TEXT("AllowedChangeSkills"), _IsAllowedChangeSkills, GGameIni);

	MMOUI::RegisterEventHandler( this, GameEvent_Item_Gain );
	//MMOUI::RegisterEventHandler( this, GameEvent_Item_Delete );

	MMOUI::RegisterEventHandler( this, GameEvent_SyncEquippedWeapon);
	MMOUI::RegisterEventHandler( this, GameEvent_Equip );
	MMOUI::RegisterEventHandler( this, GameEvent_Item_SlotSwapped );
	MMOUI::RegisterEventHandler( this, GameEvent_Item_Delete );

	MMOUI::RegisterEventHandler( this, GameEvent_EquipDurChanged );

	MMOUI::RegisterEventHandler( this, GameEvent_MsgBoxChangeSkillStyle );
	MMOUI::RegisterEventHandler( this, GameEvent_SkillLevelUp );

	MMOUI::RegisterEventHandler( this, GameEvent_WeaponLevelUpPutIn );
	MMOUI::RegisterEventHandler( this, GameEvent_WeaponLevelUpTakeOut );
	MMOUI::RegisterEventHandler( this, GameEvent_WeaponLevelUpResult );
	MMOUI::RegisterEventHandler( this, GameEVent_SceneLoadFinish );
}

void MMOEquipManager::Shutdown()
{
	if( _ConfirmEquipWeaponSkill != NULL ){
		//MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( _ConfirmEquipWeaponSkill );
		_ConfirmEquipWeaponSkill = NULL;
	}
	//
	MMOUI::UnRegisterEventHandler( this, GameEVent_SceneLoadFinish );
	MMOUI::UnRegisterEventHandler( this, GameEvent_WeaponLevelUpResult );
	MMOUI::UnRegisterEventHandler( this, GameEvent_WeaponLevelUpTakeOut );
	MMOUI::UnRegisterEventHandler( this, GameEvent_WeaponLevelUpPutIn );

	MMOUI::UnRegisterEventHandler( this, GameEvent_SkillLevelUp );
	MMOUI::UnRegisterEventHandler( this, GameEvent_MsgBoxChangeSkillStyle );

	MMOUI::UnRegisterEventHandler( this, GameEvent_EquipDurChanged );

	MMOUI::UnRegisterEventHandler( this, GameEvent_Item_Delete );
	MMOUI::UnRegisterEventHandler( this, GameEvent_Item_SlotSwapped );
	MMOUI::UnRegisterEventHandler( this, GameEvent_Equip );
	MMOUI::UnRegisterEventHandler( this, GameEvent_SyncEquippedWeapon);

	//MMOUI::UnRegisterEventHandler( this, GameEvent_Item_Delete );
	MMOUI::UnRegisterEventHandler( this, GameEvent_Item_Gain );
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SoulUnEmbed );
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SoulEmbed );
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SoulLevelUp );
}
//=================================================================================================================================
void MMOEquipManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_Item_Gain:
		{
			MMOGameEvent_Item *itemGainEvt = (MMOGameEvent_Item*)evt;
			DescideToShowEquipNewHint( itemGainEvt->ContainerIdx, itemGainEvt->SlotIdx );
		}
		break;
	case GameEvent_SyncEquippedWeapon:
		{
			ModifyEquippedSkillsAccordingSyncEquippedWeapon();
		}
		break;
	//case GameEvent_EnterSelectingRole:
	//	{
	//		ResetCachedRplcWeaponRune();
	//	}
	//	break;
	case GameEVent_SceneLoadFinish:
		{
			ResetWeaponRplcCachedRune();
		}
		break;
	case GameEvent_EquipDurChanged:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			//Equipped item changed
			UMMOUIDurabilityShowing *durabilityShowingPanel = (UMMOUIDurabilityShowing*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DurabilityShowing );
			if( durabilityShowingPanel != NULL ){
				durabilityShowingPanel->UpdateEquipDurState( (ENORMAL_EQUIP_SLOT)itemDelEvt->SlotIdx );
				durabilityShowingPanel->DescideShow();
			}

			////Durability reached zero, handle equipping/unequipping ability on equipment
			//FClientItemInfo* toCheckEquippedItem = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, itemDelEvt->SlotIdx );
			//if( toCheckEquippedItem != NULL ){
			//	if( toCheckEquippedItem ->m_nCurDur <= 0 ){
			//		CheckEquipmentHandleWeaponMainSkill( itemDelEvt->SlotIdx );
			//	}
			//}
			//else{
			//	CheckEquipmentHandleWeaponMainSkill( itemDelEvt->SlotIdx );
			//}
			
		}
		break;
	case GameEvent_Equip:
		{
			//MMOGameEvent_Item *itemEquipEvt = (MMOGameEvent_Item*)evt;

			//if( _IsAllowedChangeSkills != TRUE ){
			//	break;
			//}

			//if( itemEquipEvt->SlotIdx == E_NES_MAIN_HAND && _ConfirmEquipWeaponSkill == NULL ){
			//	//Whether to automatically switch ability
			//	//UBOOL isAutoChangeSkill = FALSE;
			//	if( _IsAutoChangeSkills ){
			//		EquipSkillsToFitCurWeapon( TRUE );
			//	}
			//	else{
			//		ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( itemEquipEvt->ContainerIdx, itemEquipEvt->SlotIdx );
			//		if( pItemInfo == NULL ){
			//			break;
			//		}
			//		
			//		//
			//		UBOOL needChangeSkills = CheckEquipWeaponSkillNeedHint();
			//		//Need to switch ability
			//		if( needChangeSkills == TRUE ){
			//			//Prompt to switch ability
			//			FString msgText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillSetting.IsConfirmAutoChangeSkillMsgHint") );
			//			_ConfirmEquipWeaponSkill = MMOMsgBoxManager::getSingleton().AddMessageBoxEvent( msgText, 0, GameEvent_MsgBoxChangeSkillStyle );
			//		}
			//		else{
			//			EquipSkillsToFitCurWeapon( TRUE );
			//		}
			//	}
			//}

			//Durability update

		}
		break;
	case GameEvent_MsgBoxChangeSkillStyle:
		{
			//MMOGameEvent_MsgboxCallBack* msg = (MMOGameEvent_MsgboxCallBack*)(evt);
			//if (msg != NULL && msg->Box != NULL ){
			//	//int roleID = msg->Box->GetInt(TEXT("iteminfoid"));
			//	if( msg->Agree ){
			//		EquipSkillsToFitCurWeapon( TRUE );
			//	}
			//	else{
			//		EquipSkillsToFitCurWeapon( FALSE );
			//	}
			//}

			//
			_ConfirmEquipWeaponSkill = NULL;
		}
		break;
	//case GameEvent_UnEquip:
	//	{
	//		MMOGameEvent_Item *itemEquipEvt = (MMOGameEvent_Item*)evt;
	//		//if( itemEquipEvt->SlotIdx == E_NES_MAIN_HAND ){
	//		//	EquipSkillsToFitCurWeapon( FALSE );
	//		//}
	//	}
	//	break;
	//case GameEvent_Item_SlotSwapped:
	//	{
	//		MMOGameEvent_ItemSwap *itemSwapEvt = (MMOGameEvent_ItemSwap*)evt;
	//		OnItemSwapped( itemSwapEvt->SrcConID, itemSwapEvt->SrcSlotID, itemSwapEvt->DstConID, itemSwapEvt->DstSlotID );
	//	}
	//	break;
	//case GameEvent_Item_Delete:
	//	{
	//		MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;
	//		OnItemSwapped( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx, E_CONSLOT_MAX, -1 );
	//	}
	//	break;
	case GameEvent_WeaponLevelUpPutIn:
		{
			FString hintStringName = TEXT( "MMOGame.EquipLevelUp.LvUpWeaponSendToServerSuccess" );
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( hintStringName ), MsgInfoType_Table, hintStringName );
		}
		break;
	case GameEvent_WeaponLevelUpTakeOut:
		{
			FString hintStringName = TEXT( "MMOGame.EquipLevelUp.LvUpWeaponGetBackFromServerSuccess" );
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( hintStringName ), MsgInfoType_Table, hintStringName );
		}
		break;
	case GameEvent_WeaponLevelUpResult:
		{
			MMOGameEvent_OneInt *resltEvent = (MMOGameEvent_OneInt*)evt;
			FString hintStringName;
			if( resltEvent->IntData > 0 ){
				hintStringName = TEXT( "MMOGame.EquipLevelUp.ResultSuccess" );
			}
			else{
				hintStringName = TEXT( "MMOGame.EquipLevelUp.ResultFailed" );
			}
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( hintStringName ), MsgInfoType_Table, hintStringName );
			//MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( hintStringName ) );
			MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( hintStringName ) );
			// Guide quest
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_UpgradeEquip);
		}
		break;
	//Inscription ability
	case GameEvent_SkillLevelUp:
		{
			MMOGameEvent_SkillLevelUp *lvUpSkillEvt = (MMOGameEvent_SkillLevelUp*)evt;
			if( lvUpSkillEvt == NULL ){
				break;
			}
			
			INT levelUpSkillID = lvUpSkillEvt->SkillID;
			INT levelUpTo = lvUpSkillEvt->NewLevel;
			//
			AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if( mainCtrl == NULL || mainPlayer == NULL ){
				break;;
			}

			UMMOAbility *pEquipedSkill = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID( levelUpSkillID );
			if( pEquipedSkill == NULL ){
				//Do not care about abilities not equipped on the body
				break;
			}

			INT slotBindHotKey = pEquipedSkill->SlotHotKeyID;
			INT curEquippedRuneIDOfThisSkill = pEquipedSkill->GetRuneSlot();
			INT curEquippedLevelOfThisSkill = pEquipedSkill->SpellCommonAttribute.Level;
			if( curEquippedRuneIDOfThisSkill <= 0 ){
				break;
			}

			if( curEquippedLevelOfThisSkill == levelUpTo ){
				break;
			}

			//
			MMOSkillManager::getSingleton().UpdateAbility( levelUpSkillID, curEquippedRuneIDOfThisSkill, levelUpTo );
		}
		break;
	}
}

//-----------------------------------------------------------------------------------------------------
void MMOEquipManager::SetAutoChangeSkills( UBOOL isTrue )
{
	//_IsAutoChangeSkills = isTrue;
	//GConfig->SetBool( TEXT("Skill"), TEXT("AutoChangeSkill"), _IsAutoChangeSkills, GGameIni );
}
//
void MMOEquipManager::SetAllowedChangeSkills( UBOOL isTrue )
{
	_IsAllowedChangeSkills = isTrue;
	GConfig->SetBool( TEXT("Skill"), TEXT("AllowedChangeSkills"), _IsAllowedChangeSkills, GGameIni );
}
//-----------------------------------------------------------------------------------------------------
UBOOL MMOEquipManager::CheckEquipWeaponSkillNeedHint()
{
	UBOOL isNeedHint = FALSE;

	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	for( int i = 0; i < SKILL_EQUIP_MAX; ++i ){
		INT slotHotKey = mmoSkillMgr.GetSkillSlotDefaultBindEnum( i );
		//Current ability
		UMMOAbility* curEquippedSkill = mmoSkillMgr.GetEquippedSkill( slotHotKey );
		INT curEquippedSkillID = ( curEquippedSkill != NULL ? curEquippedSkill->eventGetAbilityId() : 0 );

		//Current weapon ability
		UBOOL isSkillActive = FALSE;
		UMMOAbility* weaponSkill = GetWeaponRuneSkillByCmdKey( slotHotKey, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );

		//If this weapon carries an ability activated for this key slot
		if( weaponSkill != NULL ){
			INT weaponSkillID = ( weaponSkill->MainSkill ? weaponSkill->MainSkill->eventGetAbilityId() : weaponSkill->eventGetAbilityId() );

			//The currently configured ability differs from the one on the weapon, or is the same but the weapon's ability is not unlocked
			if( curEquippedSkillID != weaponSkillID && isSkillActive ){
				//An ability does not fit the pattern and needs replacing
				isNeedHint = TRUE;
				break;
			}
		}
	}

	return isNeedHint;
}
//-----------------------------------------------------------------------------------------------------
UBOOL MMOEquipManager::EquipSkillsToFitCurWeapon( UBOOL canEquipWeaponSkill )
{
	UBOOL isSkillChanged = FALSE;
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	for( int i = 0; i < SKILL_EQUIP_MAX; ++i ){
		INT slotHotKey = mmoSkillMgr.GetSkillSlotDefaultBindEnum( i );
		//Current ability
		UMMOAbility* curEquippedSkill = mmoSkillMgr.GetEquippedSkill( slotHotKey );
		INT curEquippedSkillID = ( curEquippedSkill != NULL ? curEquippedSkill->eventGetAbilityId() : 0 );

		//Current weapon ability
		UBOOL isSkillActive = FALSE;
		UMMOAbility* weaponSkill = GetWeaponRuneSkillByCmdKey( slotHotKey, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
		//If this weapon carries an ability activated for this key slot
		if( canEquipWeaponSkill && ( weaponSkill != NULL && isSkillActive ) ){
			INT weaponSkillID = ( weaponSkill->MainSkill ? weaponSkill->MainSkill->eventGetAbilityId() : weaponSkill->eventGetAbilityId() );
			//Set ability
			if( curEquippedSkill == NULL || curEquippedSkillID != weaponSkillID ){
				//Set main ability
				CMessageHandler::GetInstance().Send_EquipSpell( slotHotKey, weaponSkillID );
				//
				isSkillChanged = TRUE;
			}
		}
		else if( curEquippedSkill != NULL ){
			//Disable some abilities
			INT curRuneSkillSlotId = curEquippedSkill->GetRuneSlot();
			if( curRuneSkillSlotId > 0 ){
				if( weaponSkill != NULL ){
					INT weaponSkillID = ( weaponSkill->MainSkill ? weaponSkill->MainSkill->eventGetAbilityId() : weaponSkill->eventGetAbilityId() );
					INT weaponRuneSkillSlotId = weaponSkill->GetRuneSlot();

					if( curEquippedSkillID == weaponSkillID && weaponRuneSkillSlotId == curRuneSkillSlotId ){
						//Same ability, no operation needed
						continue;
					}
				}

				//
				isSkillChanged = TRUE;
			}
		}

		//UUIKeyMapManager *keyMapManager = MMOFacade::getSingleton().GetMainController()->KeyMapManager;
		//AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		//AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
		//if( keyMapManager == NULL || mainPlayer == NULL || mainCtrl == NULL ){
		//	return FALSE;
		//}

		//if( canEquipWeaponSkill && weaponSkill != NULL && isSkillActive == TRUE ){
		//	INT weaponSkillID = ( weaponSkill->MainSkill ? weaponSkill->MainSkill->eventGetAbilityId() : weaponSkill->eventGetAbilityId() );
		//	if( curSkillID != weaponSkill->MainSkill->eventGetAbilityId() ){
		//		//Set main ability
		//		CMessageHandler::GetInstance().Send_EquipSpell( slotHotKey, weaponSkillID );
		//	}
		//	//Set sub ability
		//	INT runeSlotId = weaponSkill->GetRuneSlot();
		//	UMMOAbility* curEquipedSkill = mainPlayer->GetMainAbilityById( curSkillID );
		//	if( curEquipedSkill != NULL && curEquipedSkill->GetRuneSlot() != runeSlotId ){
		//		CMessageHandler::GetInstance().Send_EquipRune( slotHotKey, runeSlotId );
		//	}

		//	//
		//	isEquipedWeaponSkill = TRUE;
		//}
		//else if( canEquipWeaponSkill == FALSE ){
		//	//If automatic ability replacement is not allowed, block abilities that do not fit the soul-casting system
		//	if( weaponSkill == NULL || ( curSkillID != weaponSkill->MainSkill->eventGetAbilityId() ) ){
		//		//The weapon does not have the previously equipped ability
		//		CMessageHandler::GetInstance().Send_EquipRune( slotHotKey, 0 );
		//	}
		//	else{
		//		//The weapon has the previously equipped ability but the sub ability differs
		//		INT runeSlotId = weaponSkill->GetRuneSlot();
		//		UMMOAbility* curEquipedSkill = mainPlayer->GetMainAbilityById( curSkillID );
		//		if( curEquipedSkill != NULL && curEquipedSkill->GetRuneSlot() != runeSlotId ){
		//			CMessageHandler::GetInstance().Send_EquipRune( slotHotKey, 0 );
		//		}
		//	}
		//}
	}

	return isSkillChanged;
}
//
//-----------------------------------------------------------------------------------------------------
void MMOEquipManager::ModifyEquippedSkillsAccordingSyncEquippedWeapon()
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	AMMOPlayerController *mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( mainPlayerCtrl == NULL ){
		return;
	}

	//
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	if( pItemInfo == NULL ){
		return;
	}

	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i )
	{
		UBOOL isSkillSlotActive = FALSE;
		UMMOAbility* pWeaponSkill = GetWeaponRuneSkillBySlotIdx( i, pItemInfo, isSkillSlotActive );
		if( isSkillSlotActive == FALSE || pWeaponSkill == NULL ){
			continue;
		}

		INT weaponSkillMainID = pWeaponSkill->eventGetAbilityId();
		UMMOAbility* curEquipedSkill = mmoSkillMgr.GetEquippedSkillBySkillID( weaponSkillMainID );
		if( curEquipedSkill == NULL ){
			continue;
		}

		INT curEuippedSlot = curEquipedSkill->SlotHotKeyID;

		INT activeSkillLevel = pWeaponSkill->SpellCommonAttribute.Level;
		INT weaponSkillRunSlotID = pWeaponSkill->GetRuneSlot();

		mainPlayerCtrl->EquipNewSpellAndAutoUnequipOld( curEuippedSlot, weaponSkillMainID, activeSkillLevel, weaponSkillRunSlotID, FALSE );
	}

	////
	//for( int i = 0; i < SKILL_EQUIP_MAX; ++i ){
	//	INT slotHotKey = mmoSkillMgr.GetSkillSlotDefaultBindEnum( i );
	//	//Current ability
	//	UMMOAbility* curEquippedSkill = mmoSkillMgr.GetEquippedSkill( slotHotKey );
	//	if( curEquippedSkill == NULL ){
	//		continue;
	//	}
	//	INT curSkillMainID = curEquippedSkill->eventGetAbilityId();

	//	//Current weapon ability
	//	UBOOL isSkillActive = FALSE;
	//	UMMOAbility* weaponSkill = GetWeaponRuneSkillByCmdKey( slotHotKey, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
	//	if( isSkillActive == FALSE || weaponSkill == NULL ){
	//		continue;
	//	}
	//	INT weaponSkillMainID = weaponSkill->eventGetAbilityId();
	//	//INT weaponSkillLevel = weaponSkill->SpellCommonAttribute.Level;
	//	INT activeSkillLevel = curEquippedSkill->SpellCommonAttribute.Level;
	//	INT weaponSkillRunSlotID = weaponSkill->GetRuneSlot();

	//	//If the equipped ability differs from the weapon ability, do nothing
	//	if( weaponSkillMainID != curSkillMainID ){
	//		continue;
	//	}
	//	mainPlayerCtrl->EquipNewSpellAndAutoUnequipOld( slotHotKey, curSkillMainID, activeSkillLevel, weaponSkillRunSlotID, FALSE );
	//}
}
//-----------------------------------------------------------------------------------------------------
void MMOEquipManager::UpdateSkillLevelByCurWeapon()
{
	UUIKeyMapManager *keyMapManager = MMOFacade::getSingleton().GetMainController()->KeyMapManager;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
	if( keyMapManager == NULL || mainPlayer == NULL || mainCtrl == NULL ){
		return;
	}

	//
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	for( int i = 0; i < SKILL_EQUIP_MAX; ++i ){
		INT slotHotKey = mmoSkillMgr.GetSkillSlotDefaultBindEnum( i );

		//Current weapon ability
		UBOOL isSkillActive = FALSE;
		UMMOAbility* weaponSkill = GetWeaponRuneSkillByCmdKey( slotHotKey, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
		//If this weapon carries an ability activated for this key slot
		if( weaponSkill != NULL && isSkillActive ){
			INT weaponSkillID = ( weaponSkill->MainSkill ? weaponSkill->MainSkill->eventGetAbilityId() : weaponSkill->eventGetAbilityId() );
			INT weaponRuneSkillSlotId = weaponSkill->GetRuneSlot();

			//Current ability
			UMMOAbility* curEquipedSkill = mmoSkillMgr.GetEquippedSkill( slotHotKey );
			INT curSkillID = -1;
			INT curRuneSkillSlotId = -1;
			if( curEquipedSkill != NULL ){
				curSkillID = curEquipedSkill->eventGetAbilityId();
				curRuneSkillSlotId = curEquipedSkill->GetRuneSlot();
			}

			//Set ability
			if( curSkillID == weaponSkillID && weaponRuneSkillSlotId == curRuneSkillSlotId ){
				if( curEquipedSkill->MainSkill != NULL ){
					//The weapon only has rune abilities
					UMMOAbility* curEquipedSkillMainSkill = curEquipedSkill->MainSkill;
					curEquipedSkillMainSkill->SubRuneList( curRuneSkillSlotId ) = weaponSkill;
				}
			}
		}//end of if weapon skill exit
	}//end of loop weapon skills
}
//=================================================================================================================================
//Enhancement
const EnhanceData* MMOEquipManager::GetEnhanceConsumption( UINT curEnhanceLv )
{
	if( !_EnhanceConsumptionContainer.IsValidIndex( curEnhanceLv ) ){
		return NULL;
	}

	return &_EnhanceConsumptionContainer( curEnhanceLv );
}

//
const EnhanceResult* MMOEquipManager::GetEnhanceResult( const FString & equipSubType, const FString &equipSubSubType, UINT enhanceLv )
{
	UINT32 key = GetEnhanceResultKey( equipSubType, equipSubSubType );

	if( _EnhanceEquipProp.Find( key ) == NULL ){
		return NULL;
	}

	TArray<EnhanceResult> &enhanceRstList = _EnhanceEquipProp.FindOrAdd( key );
	INT dataIdx = INT( enhanceLv - 1 );
	if( !enhanceRstList.IsValidIndex( dataIdx ) ){
		return NULL;
	}

	return &enhanceRstList( dataIdx );
}

//
UINT32 MMOEquipManager::GetEnhanceResultKey( const FString & equipSubType, const FString &equipSubSubType )
{
	//static UEnum* itemSubTypeEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("ITEMTYPE_EQUIP"), TRUE);
	//INT itemSubTypeIdx = itemSubTypeEnum->FindEnumIndex( equipSubType );
	//INT itemSubTypeIdx = FindClientEnumValue( equipSubType, TEXT("ITEM_SUBTYPE"));
	FString strKey = equipSubType + equipSubSubType;

	if( _EnhanceKey.Find( strKey ) == NULL ){
		UINT32 &key = _EnhanceKey.FindOrAdd( strKey );
		key = _EnhanceKey.Num();
	}

	return _EnhanceKey.FindOrAdd( strKey );
}

//
const EnhanceData* MMOEquipManager::GetEnhanceConsumptionInEquipBag( UINT bagIdx )
{
	ItemInfo* pEnhanceItemInfo = MMOFacade::getSingleton().FindItem( CONSLOT_DEFAULT, bagIdx );
	if( pEnhanceItemInfo == NULL ){
		return NULL;
	}
	
	return GetEnhanceConsumption( pEnhanceItemInfo->m_wRefineLevel );
}

//
const EnhanceResult* MMOEquipManager::GetEnhanceResultInEquipBag( UINT bagIdx )
{
	ItemInfo* pEnhanceItemInfo = MMOFacade::getSingleton().FindItem( CONSLOT_DEFAULT, bagIdx );
	if( pEnhanceItemInfo == NULL ){
		return NULL;
	}

	//
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	UEquip* pEquip = Cast<UEquip>( pMainPlayer->ItemManager->BagItemList( bagIdx ) );
	if( pEquip == NULL ){
		return NULL;
	}

	return GetEnhanceResult( pEquip->ItemSubType, pEquip->ItemSubSubType, pEnhanceItemInfo->m_wRefineLevel );
}

//=============================================================================================================================
//Read item enhancement info
#if 0
UBOOL MMOEquipManager::ReadItemEnhanceConsumption()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadItemEnhanceConsumption"), 0);

	TArray<TArray<FString>> ArrayString2D;
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("item_refine.txt"), TRUE ) )
	//if( !MMOFacade::getSingleton().ReadConfigString( ArrayString2D, TEXT("item_refine.txt"), true) )
	{
		return FALSE;
	}

	_EnhanceConsumptionContainer.Empty();

	UINT rowCnt =  tabFile->GetRowCount();//  ArrayString2D.Num();
	//_EnhanceConsumptionContainer.SetNum(rowCnt);

	//
	EnhanceData data;
	//Remove the header row
	UINT skipRow = M_SKIP_ROWS;
	for( UINT i = skipRow; i < rowCnt; ++i )
	{
		//TArray<FString>& ArrayString = ArrayString2D(i);
		UINT len = tabFile->GetColCount(); // ArrayString.Num();
		if( len < 10 )
		{
			M_AddDataError(TEXT("item_refine.txt"), FString::Printf(TEXT("Invalid enhancement data, row %d"),i));
			_EnhanceConsumptionContainer.Push( data );
			continue;
		}

		//
		UINT uCurEnhanceLv =appAtoi(tabFile->GetData(0,i));	

		data._EnhanceNeedMny1		= appAtoi( tabFile->GetData(1,i) );
		data._EnhanceNeedMny2		= appAtoi( tabFile->GetData(2,i) );
		data._TransferNeedMny2		= appAtoi( tabFile->GetData(3,i) );
		data._EnhanceNeed[0]._Type	= appAtoi( tabFile->GetData(4,i) );
		data._EnhanceNeed[0]._Value = appAtoi( tabFile->GetData(5,i) );
		data._EnhanceNeed[1]._Type	= appAtoi( tabFile->GetData(6,i) );
		data._EnhanceNeed[1]._Value = appAtoi( tabFile->GetData(7,i) );
		data._EnhanceNeed[2]._Type	= appAtoi( tabFile->GetData(8,i) );
		data._EnhanceNeed[2]._Value = appAtoi( tabFile->GetData(9,i) );

		//MMOFacade::getSingleton().AddEnhanceData( uEnhanceLv, &data );
		_EnhanceConsumptionContainer.InsertItem( data, uCurEnhanceLv );
	}
	delete tabFile;
	return TRUE;

	//FString enhanceTxt;
	//// load the DefaultEngine.ini config file into a string for later parsing (ConvertAbsolutePathToUserPath will use
	//// original location since MacUserDir hasn't been set yet)
	//// can't use GDefaultEngineIni, because that may be something that doesn't have the tag
	//if (!appLoadFileToString(enhanceTxt, *( appGameConfigDir() + TEXT("item_refine.txt")), GFileManager))
	//{
	//	// appMsgf won't write to a log if GWarn is NULL, which it should be at this point
	//	//appMsgf(AMT_OK, TEXT("Failed to find default engine .ini file to retrieve Application Support subdirectory to use. Force quitting."));
	//	//exit(1);
	//	return FALSE;
	//}

	//UBOOL	res		=	TRUE;
	//INT		size	=	enhanceTxt.Len();
	//TCHAR*	pData	=	*enhanceTxt;
	//TCHAR*	pDataNew=	new TCHAR[size+1];
	//UBOOL	returnLine = FALSE;

	////Find how many rows there are
	//INT m_nLine = 0;
	//for(int i=2;i<=size;i++)
	//{
	//	if(pData[i]=='\r')m_nLine++;
	//}

	//int nStart = 0;
	//TCHAR Out[512];
	////Read the header
	////level
	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	////0~11
	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	//res=ReadStrNode(pData,nStart,Out,&returnLine);

	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	//res=ReadStrNode(pData,nStart,Out,&returnLine);

	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	//res=ReadStrNode(pData,nStart,Out,&returnLine);

	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	//res=ReadStrNode(pData,nStart,Out,&returnLine);

	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	//res=ReadStrNode(pData,nStart,Out,&returnLine);

	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	//res=ReadStrNode(pData,nStart,Out,&returnLine);
	////
	////Initialize
	//MMO::Facade::InitEnhanceData( m_nLine-1,12 );
	//for(int i=1;i<m_nLine;i++)
	//{
	//	//Read each row
	//	//Minimum level required for the item
	//	res=ReadStrNode(pData,nStart,Out,&returnLine);
	//	UINT uItemMinLv = appAtoi(Out);
	//	//
	//	TArray<FString> cntArray;
	//	for( int j=0; j<12; ++j )
	//	{
	//		//Number of materials required for enhancement level j "x,x,x,x,x"
	//		res=ReadStrNode(pData,nStart,Out,&returnLine);
	//		//
	//		FString cntData = Out;
	//		cntData = cntData.TrimQuotes();//x,x,x,x,x
	//		cntData.ParseIntoArray( &cntArray, TEXT(","), 0 );
	//		//
	//		EnhanceData data;
	//		data.byMatCnt[0] = appAtoi( *cntArray(0) );
	//		data.byMatCnt[1] = appAtoi( *cntArray(1) );
	//		data.byMatCnt[2] = appAtoi( *cntArray(2) );
	//		data.byMatCnt[3] = appAtoi( *cntArray(3) );
	//		data.byMatCnt[4] = appAtoi( *cntArray(4) );
	//		MMOFacade::getSingleton().AddEnhanceData( i-1, j, &data );
	//	}//end of for j
	//}//end of for i

	//////test
	////for(int i=0;i<m_nLine-1;i++){
	////	TArray<FString> cntArray;
	////	for( int j=0; j<12; ++j ){
	////		_EnhanceConsumptionContainer(i)(j).byMatCnt[0];
	////		warnf(NAME_Warning, TEXT("(%d,%d)ItemCnt(%d, %d, %d, %d, %d)\n"),
	////			i, j, 
	////			_EnhanceConsumptionContainer(i)(j).byMatCnt[0],
	////			_EnhanceConsumptionContainer(i)(j).byMatCnt[1],
	////			_EnhanceConsumptionContainer(i)(j).byMatCnt[2],
	////			_EnhanceConsumptionContainer(i)(j).byMatCnt[3],
	////			_EnhanceConsumptionContainer(i)(j).byMatCnt[4]);
	////	}
	////}
	//return res;
}
#endif
//
#if 0
UBOOL MMOEquipManager::ReadItemEnhanceResult()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadItemEnhanceResult"), 0);

	TArray<TArray<FString>> ArrayString2D;
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("refine_attr.txt"), TRUE ) )
	//if( !MMOFacade::getSingleton().ReadConfigString( ArrayString2D, TEXT("refine_attribute.txt"), true) )
	{
		return FALSE;
	}

	_EnhanceEquipProp.Empty();
	//_enhanceWeaponProp.Empty();
	//Remove the header row and one invalid row
	UINT rowCnt = tabFile->GetRowCount();  //ArrayString2D.Num();
	UINT lastLv = 0;
	UINT skipRow = M_SKIP_ROWS;
	for( UINT i = skipRow; i < rowCnt; ++i )
	{
		//TArray<FString>& ArrayString = ArrayString2D(i);

		//
		UINT len = tabFile->GetColCount();// ArrayString.Num();
		if( len < 8 )
		{
			M_AddDataError(TEXT("refine_attribute.txt"), FString::Printf(TEXT("======!!!!======= [ReadItemEnhanceResult ERROR]AT %d"), i ));
			continue;
		}

		//
		UINT uEnhanceLv = appAtoi( tabFile->GetData(TEXT("RefineLevel"),i) );

		//equipSubType		= ArrayString(1);
		//equipSubSubType		= ArrayString(2);
		UINT32 resultKey = GetEnhanceResultKey( tabFile->GetData(TEXT("Class"),i), tabFile->GetData(TEXT("NormalEquipSlot"),i) );
		//
		EnhanceResult data;
		data._Prop[0]	= appAtoi( tabFile->GetData(TEXT("Attr1"),i) );
		data._Prop[1]	= appAtoi( tabFile->GetData(TEXT("Attr2"),i) );
		data._Prop[2]	= appAtoi( tabFile->GetData(TEXT("Attr3"),i) );
		data._Prop[3]	= appAtoi( tabFile->GetData(TEXT("Attr4"),i) );
		data._Prop[4]	= appAtoi( tabFile->GetData(TEXT("Attr5"),i) );

		//MMOFacade::getSingleton().AddEnhanceData( uEnhanceLv, &data );
		TArray<EnhanceResult> &enhanceRstList =_EnhanceEquipProp.FindOrAdd( resultKey );
		if( uEnhanceLv != enhanceRstList.Num() + 1 )
		{
			M_AddDataError(TEXT("refine_attribute.txt"), FString::Printf(TEXT("======!!!!======= [ReadItemEnhanceResult ERROR]AT %d"), i ));
		}
		enhanceRstList.Push( data );
	}
	delete tabFile;
	return TRUE;
}
#endif
//=============================================================================================================================
//Read item enhancement info
UBOOL MMOEquipManager::ReadEquipSuit()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadEquipSuit"), 0);

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("set.txt"), TRUE ) ){
		return FALSE;
	}

	//_EquipSuitLookup.Empty();

	//
	UINT rowCnt = tabFile->GetRowCount();
	UINT skipRow = M_SKIP_ROWS;
	for( UINT i = skipRow; i < rowCnt; ++i )
	{
		INT suitID = appAtoi( tabFile->GetData( TEXT("SetID"), i ) );
		EquipSuit &data = _EquipSuits.FindOrAdd( suitID );
		data._ID		= suitID;
		data._SuitName	= tabFile->GetData( TEXT("SetName"), i );
		//suit parts
		UINT readIdxBegin = 2;
		UINT partsCnt = 0;
		for( UINT j = 0; j < MAX_SET_PART_COUNT; ++j ){
			data._Parts[partsCnt]._ItemID	= appAtoi( tabFile->GetData( readIdxBegin + j, i ) );
			if( data._Parts[partsCnt]._ItemID <= 0 ){
				data._Parts[partsCnt]._ItemID = -1;
				continue;
			}

			//
			++partsCnt;
		}
		data._PartsCnt = partsCnt;

		//suit prop
		readIdxBegin = 2 + MAX_SET_PART_COUNT;
		UINT propCnt = 0;
		const UINT propDataCnt = 2;
		for( UINT j = 0; j < MAX_SET_ATTR_COUNT; ++j ){
			data._Props[propCnt]._Condition	= appAtoi( tabFile->GetData( readIdxBegin + j*propDataCnt, i ) );
			if( data._Props[propCnt]._Condition <= 0 ){
				data._Props[propCnt]._Condition = 0;
				continue;
			}

			data._Props[propCnt]._PropID = appAtoi( tabFile->GetData( readIdxBegin + j*propDataCnt + 1, i ) );

			//
			++propCnt;
		}
		data._PropsCnt = propCnt;
	}

	delete tabFile;

	return TRUE;
}
//=============================================================================================================================
EquipSuit* MMOEquipManager::FindEquipSuitByPartOfSuit( INT itemID )
{
	//for( EquipSuitContainer::TConstIterator it(_EquipSuits); it; ++it ){
	//	const EquipSuit *pEquipSuit = &it.Value();
	for( EquipSuitContainer::TIterator it(_EquipSuits); it; ++it ){
		EquipSuit *pEquipSuit = &it.Value();
		UINT suitPartsCnt = pEquipSuit->_PartsCnt;

		for( UINT j = 0; j < suitPartsCnt; ++j ){
			if( pEquipSuit->_Parts[j]._ItemID == itemID ){
				if( pEquipSuit->_Parts[j]._Name.IsEmpty() ){
					pEquipSuit = FindOrLoadFullEquipSuitBySuitID( pEquipSuit->_ID );
				}

				return pEquipSuit;
			}
		}
	}
	//
	//const INT *suitID = _EquipSuitLookup.Find( itemID );
	//if( suitID != NULL ){
	//	return FindOrLoadFullEquipSuitBySuitID( *suitID );
	//}

	return NULL;
}

EquipSuit* MMOEquipManager::FindOrLoadFullEquipSuitBySuitID( INT suitID )
{
	EquipSuit *pEquipSuit = _EquipSuits.Find( suitID );
	if( pEquipSuit == NULL ){
		return NULL;
	}

	//
	for( INT j = 0; j < pEquipSuit->_PartsCnt; ++j ){
		if( pEquipSuit->_Parts[j]._Name.IsEmpty() ){
			INT itemID = pEquipSuit->_Parts[j]._ItemID;
			UEquip* pEquip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( itemID ) );
			if( pEquip == NULL ){
				warnf(NAME_Error, TEXT("======!!!!======= [FindOrLoadFullEquipSuitBySuitID ERROR, equip%d not found]"), itemID );
				pEquipSuit->_Parts[j]._ItemID = -1;
				break;
			}

			INT destCon = E_CONSLOT_MAX, destSlot = 0;
			if( pEquip->GetEquipAppearenceTemplate() == NULL){
				warnf(NAME_Error, TEXT("======!!!!======= [FindOrLoadFullEquipSuitBySuitID ERROR, equip%d EquipAppearenceTemplate is NULL]"), itemID );
				pEquipSuit->_Parts[j]._ItemID = -1;
				break;
			}
			MMOUI::GetEquippedSlotByEquipType( pEquip->GetEquipAppearenceTemplate()->EquipType, destCon, destSlot);

			//if( destCon != E_CONSLOT_EQUIPPED ){
			//	warnf(NAME_Error, TEXT("======!!!!======= [ReadEquipSuit ERROR, equip%d cannot equiped]"), data._Parts[partsCnt]._ItemID );
			//	data._Parts[partsCnt]._ItemID = -1;
			//	continue;
			//}

			pEquipSuit->_Parts[j]._Name = pEquip->GetDisplayName();
			pEquipSuit->_Parts[j]._EquipSlot = destSlot;
		}
	}

	return pEquipSuit;
}

////Enhancement data
//void Facade::InitEnhanceData( UINT itemLvCnt, UINT enhanceLvCnt)
//{
//	TArray<EnhanceData> rowData;
//	EnhanceData etyData;
//	//
//	if( _EnhanceConsumptionContainer.Num() > 0 ){
//		for( UINT i = 0; i < (UINT)_EnhanceConsumptionContainer.Num(); ++i ){
//			_EnhanceConsumptionContainer(i).Empty();
//		}
//		_EnhanceConsumptionContainer.Empty();
//	}
//	//
//	for( UINT i = 0; i < enhanceLvCnt; ++i ){
//		rowData.AddItem(etyData);
//	}
//	for( UINT i = 0; i < itemLvCnt; ++i ){
//		_EnhanceConsumptionContainer.AddItem(rowData);
//	}
//}
//void Facade::AddEnhanceData( UINT enhanceLv, EnhanceData* data)
//{
//	if( enhanceLv < (UINT)_EnhanceConsumptionContainer.Num() ){
//		_EnhanceConsumptionContainer(enhanceLv) = *data;
//	}
//	else{
//		UINT iCnt = enhanceLv + 1 - _EnhanceConsumptionContainer.Num();
//		for( UINT i = 0; i < iCnt; ++i ){
//			_EnhanceConsumptionContainer.AddItem( *data );
//		}
//	}	
//}
//const EnhanceData* Facade::GetEnhanceData( UINT enhanceLv)
//{
//	if( !_EnhanceConsumptionContainer.IsValidIndex( enhanceLv ) ){
//		return NULL;
//	}
//
//	return &_EnhanceConsumptionContainer(enhanceLv);
//}

//=============================================================================================================================
//Read spirit fusion info
UBOOL MMOEquipManager::ReadSoulInfoTemplates()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadSoulInfoTemplates"), 0);

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("soul.txt"), TRUE ) ){
		return FALSE;
	}

	_SoulInfoTemplateMap.Empty();
	//Remove the header row
	//Skip number 0
	UINT rowCnt = tabFile->GetRowCount();
	UINT idx = 0;
	//Actual data starts from row 4; header row, N/A row, row 0
	UINT skipRow = M_SKIP_ROWS;
	for( UINT i = skipRow ; i < rowCnt; ++i, ++idx )
	{
		INT soulItemID = appAtoi( tabFile->GetData( TEXT("TemplateID"), i ) );
		SoulInfo &data = _SoulInfoTemplateMap.FindOrAdd( soulItemID );
		data._ID = idx;
		data._ItemID = soulItemID;
		data._AttrID = appAtoi( tabFile->GetData( TEXT("AttrID"), i ) );
		data._MatchSocket = appAtoi( tabFile->GetData( TEXT("MatchSocket"), i ) );
		data._SoulLevel = appAtoi( tabFile->GetData( TEXT("SoulLevel"), i ) );
		data._PriceValue = appAtoi( tabFile->GetData( TEXT("PriceValue"), i ) );
		data._UpgradeSoulID = appAtoi( tabFile->GetData( TEXT("UpgradeSoulID"), i ) );
		data._DetouchPrice = appAtoi( tabFile->GetData( TEXT("RemovePrice"), i ) );
	}

	delete tabFile;
	return TRUE;
}

//
const SoulInfo* MMOEquipManager::FindSoulInfoByItemTemplateID( INT itemID )
{
	return _SoulInfoTemplateMap.Find( itemID );
}
//=============================================================================================================================
//Weapon ability (rune ability)
UBOOL MMOEquipManager::ReadWeaponRuneSkillData()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadWeaponRuneSkillData"), 0);

	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("rune.txt"), TRUE ) ){
		return FALSE;
	}

	_EquipRuneSkills.Empty();

	UINT rowCnt = tabFile->GetRowCount();
	//Skip one header row and one useless data row
	UINT skipRow = M_SKIP_ROWS;
	for( UINT i = skipRow; i < rowCnt; ++i ) // Row 3 is the default value, no need to read it in
	{

		INT weaponSkillID		= appAtoi( tabFile->GetData( TEXT("RuneID"), i ) );

		INT skillID			= appAtoi( tabFile->GetData( TEXT("SpellID"), i ) );
		INT skillRuneIdx	= appAtoi( tabFile->GetData( TEXT("RuneSlot"), i ) );
		//INT skillLv			= appAtoi( tabFile->GetData( TEXT("RuneLevel"), i ) );
		//UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv, skillRuneIdx );

		WeaponSkillCfg &curCfg = _EquipRuneSkills.FindOrAdd( weaponSkillID );
		curCfg._SkillID = skillID;
		curCfg._SkillRuneID = skillRuneIdx;
		curCfg._Level = appAtoi( tabFile->GetData( TEXT("Level"), i ) );
		curCfg._IsRare = appAtoi( tabFile->GetData( TEXT("IsRare"), i ) ) > 0;
		curCfg._RarePriority = appAtoi( tabFile->GetData( TEXT("Priority"), i ) );
	}

	delete tabFile;
	return TRUE;
}
//UBOOL MMOEquipManager::ReadWeaponRuneSkillData()
//{
//	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadWeaponRuneSkillData"), 0);
//
//	TArray<TArray<FString>> ArrayString2D;
//	if( !MMOFacade::getSingleton().ReadConfigString( ArrayString2D, TEXT("weapon_rune_id.txt"), true) )
//	{
//		return FALSE;
//	}
//
//	//
//	TMap< FString, INT > classLookUpMap;
//	classLookUpMap.FindOrAdd( TEXT("clsWarrior") ) = EPCT_PlayerClassA;//warrior
//	classLookUpMap.FindOrAdd( TEXT("clsMage") ) = EPCT_PlayerClassB;//mage
//	classLookUpMap.FindOrAdd( TEXT("clsRogue") ) = EPCT_PlayerClassC;//assassin
//	classLookUpMap.FindOrAdd( TEXT("clsArcher") ) = EPCT_PlayerClassD;//archer
//
//	_EquipRuneSkills.Empty();
//
//	UINT rowCnt = ArrayString2D.Num();
//	//Skip one header row and one useless data row
//	for( UINT i=0+3; i < rowCnt; ++i ) // Row 3 is the default value, no need to read it in
//	{
//		TArray<FString>& ArrayString = ArrayString2D(i);
//
//		//
//		UINT len = ArrayString.Num();
//		if( len < 5 )
//		{
//			M_AddDataError(TEXT("weapon_rune_id.txt"), FString::Printf(TEXT("Incorrect data, row %d."), i + 1));
//			continue;
//		}
//
//		//
//		INT *pClassEnum = classLookUpMap.Find( ArrayString(0) );
//		if( pClassEnum == NULL )
//		{
//			M_AddDataError(TEXT("weapon_rune_id.txt"), FString::Printf(TEXT("Incorrect player class, row %d."), i + 1 ));
//			continue;
//		}
//
//		INT weaponSkillSlotIdx = appAtoi( *ArrayString(1) );
//		INT weaponSkillTypeInSlot = appAtoi( *ArrayString(2) );
//
//		UINT weaponSkillKey = MMOEquipManager::CombinateWeaponSkillID( *pClassEnum, weaponSkillSlotIdx, weaponSkillTypeInSlot );
//		//
//		INT skillID = appAtoi( *ArrayString(3) );
//		INT skillRuneID = appAtoi( *ArrayString(4) );
//		UINT uniqueSkillIdWithLvOne = MMOSkillManager::CombinateAbilityID( skillID, 1, skillRuneID );
//
//		_EquipRuneSkills.FindOrAdd( weaponSkillKey ) = uniqueSkillIdWithLvOne;
//	}
//
//	return TRUE;
//}
//=============================================================================================================================
UBOOL MMOEquipManager::GetSkillUniqueIDOnWeapon( UINT &outuniqueSkillID, UINT weaponSkillID )
{
	outuniqueSkillID = 0;
	const WeaponSkillCfg *pWeaponSkillCfgData = GetWeaponSkillCfg( weaponSkillID );
	if( pWeaponSkillCfgData == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [GetSkillUniqueIDOnWeapon ERROR, weaponSkillID:%d in Rune.txt not found]"), weaponSkillID );
		return FALSE;
	}
	
	UMMOAbility *myThisAbility = MMOSkillManager::getSingleton().FindMyLearnedMainSkill( pWeaponSkillCfgData->_SkillID );
	if( myThisAbility != NULL ){
		outuniqueSkillID = MMOSkillManager::CombinateAbilityID( pWeaponSkillCfgData->_SkillID, myThisAbility->SpellCommonAttribute.Level, pWeaponSkillCfgData->_SkillRuneID );
	}
	else{
		outuniqueSkillID = MMOSkillManager::CombinateAbilityID( pWeaponSkillCfgData->_SkillID, 0, pWeaponSkillCfgData->_SkillRuneID );
	}
	return TRUE;
}
////=============================================================================================================================
//const WeaponSkillCfg *MMOEquipManager::GetWeaponSkillCfgByItemInfo( const ItemInfo *pItemInfo, INT skillBindSlot )
//{
//	if( pItemInfo == NULL ){
//		return NULL;
//	}
//	if( skillBindSlot < 0 || skillBindSlot >= EQUIP_RUNE_SKILL_MAX ){
//		return NULL;
//	}
//
//	UINT weaponSkillID = pItemInfo->m_wRune[skillBindSlot];
//	//if( weaponSkillID == RUNE_BIND_SLOT_CLOSE ){
//	if( weaponSkillID <= 0 ){
//		return NULL;
//	}
//
//	//
//	const WeaponSkillCfg *pWeaponSkillCfgData = GetWeaponSkillCfg( weaponSkillID );
//
//	return pWeaponSkillCfgData;
//}
//=============================================================================================================================
const WeaponSkillCfg *MMOEquipManager::GetWeaponSkillCfg( UINT weaponSkillID )
{
	WeaponSkillCfg *pWeaponSkillCfgData = _EquipRuneSkills.Find( weaponSkillID );

	return pWeaponSkillCfgData;
}
//=============================================================================================================================
UMMOAbility* MMOEquipManager::GetCurWeaponRuneSkillOfMainSkillID( INT mainSkillID, UBOOL &outIsSkillActive )
{
	ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	if( pItemInfo == NULL ){
		return NULL;
	}

	UMMOAbility* weaponSkill = NULL;
	//
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		weaponSkill = GetWeaponRuneSkillBySlotIdx( i, pItemInfo, outIsSkillActive );
		if( weaponSkill == NULL ){
			continue;
		}

		INT weaponSkillMainID = weaponSkill->eventGetAbilityId();
		if( weaponSkillMainID == mainSkillID ){
			return weaponSkill;
		}
	}

	return NULL;
}
//=============================================================================================================================
INT MMOEquipManager::GetWeaponRuneSkillIDBySlotIdx( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo )
{
	if( pItemInfo == NULL ){
		return 0;
	}
	if( weaponSkillSlotIdx < 0 || weaponSkillSlotIdx >= EQUIP_RUNE_SKILL_MAX ){
		return 0;
	}

	//
	UINT weaponSkillID = pItemInfo->m_wRune[weaponSkillSlotIdx];

	return weaponSkillID;
}
//=============================================================================================================================
UMMOAbility* MMOEquipManager::GetWeaponRuneSkillBySlotIdx( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo, UBOOL &outIsSkillActive )
{
	outIsSkillActive = FALSE;
	UINT weaponSkillID = GetWeaponRuneSkillIDBySlotIdx( weaponSkillSlotIdx, pItemInfo );
	//if( weaponSkillID == RUNE_BIND_SLOT_CLOSE ){
	if( weaponSkillID <= 0 ){
		return NULL;
	}

	outIsSkillActive = TRUE;
	//
	UINT skillUniqueID;
	UBOOL isFound = GetSkillUniqueIDOnWeapon( skillUniqueID, weaponSkillID );
	if( isFound == FALSE ){
		return NULL;
	}

	UMMOAbility* weaponSkill = MMOSkillManager::getSingleton().FindAbilityTemplate( skillUniqueID );

	return weaponSkill;
}
//=============================================================================================================================
INT MMOEquipManager::GetWeaponRplcCacheRuneSkillID( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo )
{
	if( weaponSkillSlotIdx < 0 || weaponSkillSlotIdx >= EQUIP_RUNE_SKILL_MAX ){
		return 0;
	}

	//
	UINT weaponSkillID = _WeaponRplcCachedRune[weaponSkillSlotIdx];

	return weaponSkillID;
}
//---------------------------------------------------------------------------------------------------------------------------
void MMOEquipManager::SetWeaponRplcCachedRune( INT weaponSkillSlotIdx, INT weaponSkillID )
{
	if( weaponSkillSlotIdx < 0 || weaponSkillSlotIdx >= EQUIP_RUNE_SKILL_MAX ){
		return;
	}

	//UBOOL willNotify = FALSE;
	//if( _WeaponRplcCachedRune[weaponSkillSlotIdx] != weaponSkillID ){
	//	willNotify = TRUE;
	//}
	_WeaponRplcCachedRune[weaponSkillSlotIdx] = weaponSkillID;

	//if( willNotify ){
	//	MMOGameEvent_OneInt evt( GameEvent_WeaponSkillRplcCacheChanged, msg->byResult );
	//	MMOUI::PostGameEvent(&evt);
	//}
}
//---------------------------------------------------------------------------------------------------------------------------
void MMOEquipManager::NotifyWeaponRplcCachedRuneChg( UINT16 (&weaponSkillIDs)[MAX_RUNE_COUNT], UBOOL isSuccess )
{
	INT cnt = EQUIP_RUNE_SKILL_MAX < MAX_RUNE_COUNT ? EQUIP_RUNE_SKILL_MAX : MAX_RUNE_COUNT;//currently there are only 2, the server definition is still 6, only take 2 here
	for( INT i = 0; i < cnt; ++i )
	{
		_WeaponRplcCachedRune[i] = (UINT)weaponSkillIDs[i];
	}

	MMOGameEvent_OneInt evt( GameEvent_WeaponSkillRplcCacheChanged, isSuccess );
	MMOUI::PostGameEvent(&evt);
}
//=============================================================================================================================
UMMOAbility* MMOEquipManager::GetWeaponRplcCacheRuneSkillBySlotIdx( INT weaponSkillSlotIdx, const ItemInfo *pItemInfo )
{
	UINT weaponSkillID = GetWeaponRplcCacheRuneSkillID( weaponSkillSlotIdx, pItemInfo );

	//
	UINT skillUniqueID;
	UBOOL isFound = GetSkillUniqueIDOnWeapon( skillUniqueID, weaponSkillID );
	if( isFound == FALSE ){
		return NULL;
	}

	UMMOAbility* weaponSkill = MMOSkillManager::getSingleton().FindAbilityTemplate( skillUniqueID );

	return weaponSkill;
}
//=============================================================================================================================
INT MMOEquipManager::GetWeaponCurSkillSlotCnt( const ItemInfo *pItemInfo )
{
	INT curSlotCnt = 0;
	if( pItemInfo == NULL ){
		return curSlotCnt;
	}

	//
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		//if( pItemInfo->m_wRune[i] != RUNE_BIND_SLOT_CLOSE ){
		if( pItemInfo->m_wRune[i] > 0 ){
			++curSlotCnt;
		}
	}

	return curSlotCnt;
}
//=============================================================================================================================
//UMMOAbility* MMOEquipManager::GetWeaponRuneSkillBase( UINT itemTemplateID, INT skillBindSlot, INT weaponSkillTypeInSlot, INT weaponSkillLv )
//{
//	//Get the rune ability type on the weapon
//	if( skillBindSlot < 0 || skillBindSlot >= MAX_RUNE_COUNT ){
//		return NULL;
//	}
//
//	if( weaponSkillTypeInSlot <= 0 ){
//		return NULL;
//	}
//
//	//Get the classes that can use the item
//	UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( itemTemplateID );
//	//UEquip *pWeapon = Cast<UEquip>( pItem );
//	if( pItem == NULL ){
//		return NULL;
//	}
//	INT playerClass = -1;
//	//Warrior
//	if( pItem->RequireClass.bClassType1 ){
//		playerClass = EPCT_PlayerClassA;
//	}
//	//Mage
//	if( pItem->RequireClass.bClassType2 ){
//		playerClass = EPCT_PlayerClassB;
//	}
//	//Assassin
//	if( pItem->RequireClass.bClassType3 ){
//		playerClass = EPCT_PlayerClassC;
//	}
//	//Archer
//	if( pItem->RequireClass.bClassType4 ){
//		playerClass = EPCT_PlayerClassD;
//	}
//	//
//	if( playerClass < 0 ){
//		M_AddDataError(TEXT("weapon_rune_id.txt"), FString::Printf(
//			TEXT("Weapon ID[%d] has no RequireClass configured"), itemTemplateID ));
//		return NULL;
//	}
//	//Find the corresponding ability ID
//	INT uniqueSkillID;
//	UBOOL isFound = GetSkillUniqueIDOnWeapon( uniqueSkillID, weaponSkillID );
//	if( isFound != TRUE ){
//		return NULL;
//	}
//
//	//
//	UINT skillID = 0;
//	UINT skillLv = 0;
//	UINT skillRuneID = 0;
//	MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );
//
//	//Whether already activated
//	UBOOL outIsSkillActive = FALSE;
//	if( weaponSkillLv > 0 ){
//		skillLv = weaponSkillLv;
//		outIsSkillActive = TRUE;
//	}
//	else{
//		skillLv = 1;
//	}
//
//	//Recombine and look up the ability again
//	uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv, skillRuneID );
//	UMMOAbility* runeSkill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
//
//	return runeSkill;
//
//	//INT bindSlotEnum = mmoSkillMgr.GetSkillSlotDefaultBindEnum( skillBindSlot );
//	//UBOOL isSkillFit = mmoSkillMgr.IsSkillBindIn( runeSkill->SpellCommonAttribute.BindSlot, bindSlotEnum );
//	//if( !isSkillFit ){
//	//	return NULL;
//	//}
//}
//=============================================================================================================================
UINT MMOEquipManager::CombinateWeaponSkillID(INT clientClassEnum, UINT weaponSkillSlotIdx, UINT weaponSkillTypeInSlot )
{
	UINT ret = (UINT)clientClassEnum;
	ret = ret<<8;
	ret += weaponSkillSlotIdx & 0x000000ff;
	ret = ret<<8;
	ret += weaponSkillTypeInSlot & 0x000000ff;
	return ret;
}

void MMOEquipManager::DecomposeWeaponSkillID(UINT weaponSkillID, UINT &outClientClassEnum, UINT &outWeaponSkillSlotIdx, UINT &outWeaponSkillTypeInSlot )
{
	outClientClassEnum = weaponSkillID>>16;
	outWeaponSkillSlotIdx = weaponSkillID & 0x0000ff00;
	outWeaponSkillTypeInSlot = weaponSkillID & 0x000000ff;
}

//======================================================================================================================================================
void MMOEquipManager::CacheMaxDurForEquipRepair( ItemInfo *equipInfo )
{
	if( equipInfo == NULL ){
		return;
	}
	EquipMaxDur data;
	data._ContainerID = equipInfo->m_byConSlot;
	data._SlotID = equipInfo->m_byItemSlot;
	data._MaxDur = equipInfo->m_nMaxDur;

	_EquipRepairDurability.Push( data );
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
void MMOEquipManager::CacheMaxDurForAllEquipRepair()
{
	ClearLastCachedRepairEquip();

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	for( INT i = 0 ; i < E_NES_TOTAL ; i++)
	{
		FClientItemInfo* equipInfo = mmoFacade.FindItem( CONSLOT_EQUIPPED , i );
		if( equipInfo != NULL && equipInfo->m_nMaxDur > 0 && ( equipInfo->m_nCurDur != equipInfo->m_nMaxDur ) )
		{
			CacheMaxDurForEquipRepair( equipInfo );
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void MMOEquipManager::ClearLastCachedRepairEquip()
{
	_EquipRepairDurability.Empty();
}
//======================================================================================================================================================
INT MMOEquipManager::CountEquipRepairCostAll()
{
	INT cost = 0;
	for( INT i = 0 ; i < E_NES_TOTAL ; i++)
	{
		FClientItemInfo* equip = MMOFacade::getSingleton().FindItem( CONSLOT_EQUIPPED , i );
		if( equip )
		{
			UEquip* equipTemp =  Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( equip->m_nTemplateID ) );
			if( equipTemp )
			{
				cost += GetEquipRepairCost( equipTemp, equip );
			}
		}

	}

	return cost;
}

//=========================================================================================================
INT MMOEquipManager::GetEquipRepairCost( const UEquip *pEquip, const ItemInfo *pItemInfo, UBOOL isSpecialRepair/* = FALSE*/ ) const
{
	if( pEquip == NULL || pItemInfo == NULL ){
		return 0;
	}

	if( pEquip->DefaultDuration == 0 ){
		return 0;
	}

	UINT64 repairCostFactor = (UINT64)pEquip->RepairCost;
	if( isSpecialRepair ){
		repairCostFactor = (UINT64)pEquip->RepairCostSpecial;
	}

	UINT64 repairCost = (UINT64)( pItemInfo->m_nMaxDur - pItemInfo->m_nCurDur ) * (UINT64)repairCostFactor;
	repairCost /= (UINT64)( pEquip->DefaultDuration * EQUIP_DUR_MULTIPLIER );

	if( repairCost > 0x8fffff ){
		warnf( NAME_Error, TEXT("GetEquipRepairCost overflow, cut to 0x8FFFFFFF(%x)."), repairCost );
		repairCost = 0x8fffff;
	}

	return (INT)repairCost;
}

//=========================================================================================================

UINT32 MMOEquipManager::CountEquipLuckTotal()
{
	UINT32 total = 0;
	for (INT i = 0; i < E_NES_TOTAL; ++ i)
	{
		FClientItemInfo* equipInfo = MMOFacade::getSingleton().FindItem(CONSLOT_EQUIPPED, i);
		if (equipInfo)
		{
			total += equipInfo->m_byLuck;

			for (INT j = 0; j < MAX_RANDOM_ATTR_COUNT; ++ j)
			{
				if (equipInfo->m_wRandomAttrID[j] == 0)
				{
					continue;
				}

				const ItemRandomAttr* attr = MMOFacade::getSingleton().GetRandomAttributById(equipInfo->m_wRandomAttrID[j]);
				if (attr && attr->attrType == TEXT("E_cat_Luck"))
				{
					total += attr->value;
				}
			}
		}
	}

	return total;
}

//========================================================================
UBOOL MMOEquipManager::CheckItemUseableForMe( const UItem *pItem ) const
{
	if( pItem == NULL ){
		return ITEM_Use_Err_Undefine;
	}

	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		return ITEM_Use_Err_Undefine;
	}

	INT mySex = mainPlayer->GetSex();
	if( ( pItem->RequireGender == EGT_Male || pItem->RequireGender == EGT_Female ) && mySex != pItem->RequireGender ){
		return ITEM_Use_Err_Gender;
	}

	INT playerClass = mainPlayer->GetClassId();
	if( !MMO::CanBeUsedByPlayerClass( (EPlayerClass)playerClass, pItem->RequireClass ) ){
		return ITEM_Use_Err_RequireClass;
	}

	//Below are checks for attributes that can change
	INT playerLv = mainPlayer->GetActorLevel();
	if( playerLv < pItem->RequireLevel ){
		return ITEM_Use_Err_LowLevel;
	}

	UBOOL isUsable = ITEM_Use_Err_Undefine;
	const UEquip *pEquip = ConstCast<UEquip>( pItem );
	if( pEquip != NULL ){
		isUsable = CheckEquipPropRequireForMe( pEquip );
	}
	else{
		isUsable = ITEM_Use_Err_None;
	}

	return isUsable;
}

//========================================================================
UBOOL MMOEquipManager::CheckEquipPropRequireForMe( const UEquip *pEquip ) const
{
	MMOAttributeAccountManager &attriMgr = MMOAttributeAccountManager::getSingleton();
	if( pEquip != NULL ){
		INT curPhyAtk = attriMgr.GetOwnerAttribute( catMaxPhyAtk, AttrListenerType_Mine );
		if( curPhyAtk < pEquip->RequireMaxPhyAtk ){
			return ITEM_Use_Err_LowPhyAtk;
		}

		INT curMagAtk = attriMgr.GetOwnerAttribute( catMaxMagAtk, AttrListenerType_Mine );
		if( curMagAtk < pEquip->RequireMaxMagAtk ){
			return ITEM_Use_Err_LowMagAtk;
		}

		INT curTaoAtk = attriMgr.GetOwnerAttribute( catMaxTaoAtk, AttrListenerType_Mine );
		if( curTaoAtk < pEquip->RequireMaxTaoAtk ){
			return ITEM_Use_Err_LowTaoAtk;
		}
	}

	return ITEM_Use_Err_None;
}

//
UBOOL MMOEquipManager::DescideToShowEquipNewHint( INT conID, INT slotID )
{
	if( MMOUI::GetUIManager()->GetCurrentGameState() != EGameState_InGame ){
		return FALSE;
	}
	
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	AMMOPlayer *pMainPlayer = mmoFacade.GetMainPlayer();
	if( pMainPlayer == NULL ){
		return FALSE;
	}

	FClientItemInfo *pNewEquipItemInfo = mmoFacade.FindItem( conID, slotID );
	if( pNewEquipItemInfo == NULL ){
		return FALSE;
	}

	UEquip *pNewEquipTemplate = Cast< UEquip >( mmoFacade.FindItemTemplate( pNewEquipItemInfo->m_nTemplateID ) );
	if( pNewEquipTemplate == NULL ){
		return FALSE;
	}

	//
	EPlayerClass playerClass = (EPlayerClass)pMainPlayer->ActorAttributes->ClassType;
	UBOOL canEquip = MMO::CanBeUsedByPlayerClass( playerClass, pNewEquipTemplate->RequireClass );
	if( !canEquip ){
		return FALSE;
	}

	//
	if(  pNewEquipTemplate->GetEquipAppearenceTemplate() == NULL ){
		return FALSE;
	}

	INT toEquipConID = 0;
	INT toEquipSlotID = 0;
	MMOUI::GetEquippedSlotByEquipType( pNewEquipTemplate->GetEquipAppearenceTemplate()->EquipType, toEquipConID, toEquipSlotID );
	if( toEquipConID != E_CONSLOT_EQUIPPED ){
		return FALSE;
	}
	
	const FClientItemInfo* pCurEquipItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, toEquipSlotID);
	if( pCurEquipItemInfo != NULL ){
		UEquip *pCurEquipTemplate = Cast< UEquip >( mmoFacade.FindItemTemplate( pCurEquipItemInfo->m_nTemplateID ) );
		if( pCurEquipTemplate == NULL ){
			return FALSE;
		}

		//
		if( pCurEquipTemplate->FixAttrbutes.BasePow >= pNewEquipTemplate->FixAttrbutes.BasePow ){
			return FALSE;
		}
	}

	UMMOUIEquipGainHintBox *equipGainHint = (UMMOUIEquipGainHintBox*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_EquipGainHintBox );
	if( equipGainHint != NULL ){
		equipGainHint->AddNewEquipGain( conID, slotID );
		//
		//if( !equipGainHint->IsVisible ){
			equipGainHint->Show( TRUE );
		//}
		//else{
		//	equipGainHint->TryShowNextOrHide();
		//}
	}
	
	//
	pNewEquipItemInfo->NeedHintMode |= ItemSlotHint_PowerEquip;
	MMOGameEvent_BagSlotHintChange msg( conID , slotID , pNewEquipItemInfo );
	MMOUI::PostGameEvent(&msg);
	//
	//switch( conID )
	//{
	//case E_CONSLOT_DEFAULT:
	//	{
	//		UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Bag );
	//		if( pBagPanel != NULL )
	//		{
	//			pBagPanel->HintItem( conID, slotID, pNewEquipItemInfo );
	//		}
	//	}
	//	break;
	//}
	//
	return TRUE;
}

//========================================================================================
UBOOL MMOEquipManager::ShortcutToPutIntoUIPanel( INT fromSlotType, INT containerID, INT slotID )
{
	AMMOPlayerController *pMainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( pMainPlayerCtrl == NULL ){
		return FALSE;
	}

	UBOOL isShiftDown = pMainPlayerCtrl->eventIsLeftShiftDown();
	if( isShiftDown == FALSE ){
		return FALSE;
	}

	//
	switch( fromSlotType )
	{
	case ESPType_ViewProfile:
	case ESPType_ShopSell:
	case ESPType_AuctionItem:
	case ESPType_TeamAuctionList:
	case ESPType_TeamAuctionPanel:
		{
			containerID = fromSlotType + E_CONSLOT_MAX;
		}
		break;
	}
	FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem( containerID, slotID );
	if( pItemInfo == NULL ){
		return FALSE;
	}

	//
	UItem* pItemInBag = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
	if( pItemInBag == NULL ){
		return FALSE;
	}

	//Whether it is an inscription weapon
	UBOOL isEquip = ( pItemInBag->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON") );
	if( isEquip ){
		UMMOUIWeaponSkill *pWeaponSkillPanel = (UMMOUIWeaponSkill*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_WeaponSkill );
		if( pWeaponSkillPanel != NULL ){
			//UBOOL canOp = pWeaponSkillPanel->IsEquipPuttedInCanOp( containerID, slotID );
			//if( canOp ){
			//	ClientActionCondition_WeaponSkill checker;

			//	FClientActionConditionContex contex;
			//	contex.Item = pItemInfo;

			//	if( checker.CheckClientActionCondition(contex) ){
			//		pWeaponSkillPanel->Show( TRUE );
			//		pWeaponSkillPanel->SetCurWeapon( containerID, slotID );
			//	}
			//}
			//else{
				pWeaponSkillPanel->ChangeToNoOpMode( TRUE );
				pWeaponSkillPanel->ChangeToSealMode( FALSE );
				pWeaponSkillPanel->Show( TRUE );
				//View only, cannot operate
				pWeaponSkillPanel->SetCurWeapon( containerID, slotID );
			//}
		}
		return TRUE;
	}

	//Only Vip can
	////Whether it is socketed equipment that can be inlaid with gems
	//const UEquip *pEmbedEquip = ConstCast<UEquip>( pItemInBag );
	//if( fromSlotType == ESPType_Bag && pEmbedEquip != NULL ){
	//	ClientActionCondition_SoulEmbedOrUnEmbed checker;

	//	FClientActionConditionContex contex;
	//	contex.Item = pItemInfo;

	//	if( checker.CheckClientActionCondition(contex) ){
	//		for(INT i = 0; i < MAX_SOCKET_COUNT; ++i){
	//			//Check sockets inlaid with spirit stones
	//			if( pItemInfo->GetSocketType(i) != E_SOCKET_NONE ){
	//				UMMOUISoulEmbed *soulEmbedPanel = (UMMOUISoulEmbed*)MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_SoulEmbed, TRUE, "");
	//				if( soulEmbedPanel != NULL ){
	//					soulEmbedPanel->SetSoulEmbedEquip( containerID, slotID );
	//				}
	//				return TRUE;
	//			}
	//		}
	//	}//end of if CheckClientActionCondition
	//}

	return FALSE;
}
//======================================================================
UINT MMOEquipManager::CombinateItemPosID( INT containerID, INT slotID )
{
	INT ret = containerID;
	ret = ret<<16;
	ret += ( slotID & 0x0000ffff );
	return ret;
}

void MMOEquipManager::DecomposeItemPosID( UINT itemPosID, INT &containerID, INT &slotID )
{
	containerID = itemPosID>>16;
	slotID = ( itemPosID & 0x0000ffff );
}
//======================================================================
UBOOL MMOEquipManager::IsItemUsableOnSkillBar( UItem *pItem )
{
	if( pItem == NULL ){
		return FALSE;
	}

	FString itemType = pItem->ItemType;
	if( itemType == TEXT("ITEMTYPE_EQUIP") ){
		return FALSE;
	}
	//INT itemID = pItem->GetTemplateID();
	//if( itemID == || itemID == || itemID == ){
	//	return FALSE;
	//}

	return TRUE;
}
//======================================================================
UBOOL MMOEquipManager::IsItemMultiOnSkillBar( UItem *pItem )
{
	if( pItem == NULL ){
		return FALSE;
	}

	FString itemType = pItem->ItemType;
	if( itemType == TEXT("ITEMTYPE_EQUIP") ){
		return FALSE;
	}

	return TRUE;
}
//======================================================================
void MMOEquipManager::BeforeServerEquipSwapHandleWeaponMainSkill( INT srcConIdx, INT srcSlotIdx, INT dstConIdx, INT dstSlotIdx )
{
	if( srcConIdx != E_CONSLOT_EQUIPPED && dstConIdx != E_CONSLOT_EQUIPPED ){
		return;
	}

	//
	INT toCheckEquipSkillEquipSlot = -1;
	if( srcConIdx == E_CONSLOT_EQUIPPED && dstConIdx != E_CONSLOT_EQUIPPED ){
		toCheckEquipSkillEquipSlot = srcSlotIdx;
	}

	if( srcConIdx != E_CONSLOT_EQUIPPED && dstConIdx == E_CONSLOT_EQUIPPED ){
		toCheckEquipSkillEquipSlot = dstSlotIdx;
	}

	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();

	//
	if( toCheckEquipSkillEquipSlot != -1 ){
		UMMOUISkillBar *pSkillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar );
		TArray< UMMOAbility* > thisEquipSkillArray;
		mmoSkillMgr.GetEquipmentEquipSkillBy( toCheckEquipSkillEquipSlot, thisEquipSkillArray );

		//Which abilities this equipment has
		INT loopCnt = thisEquipSkillArray.Num();
		for( INT i = 0; i < loopCnt; ++i ){
			UMMOAbility *equipAblility = thisEquipSkillArray( i );
			if( equipAblility == NULL ){
				continue;
			}

			INT skillID = equipAblility->eventGetAbilityId();

			TArray< INT > sameSkillEquipmentArray;
			mmoSkillMgr.GetEquippmentOfSameSkill( skillID, sameSkillEquipmentArray );

			if( sameSkillEquipmentArray.Num() <= 1 && pSkillBarPanel != NULL ){
				UMMOAbility *skillBookLearnedSkill = mmoSkillMgr.FindMyLearnedMainSkillByBook( skillID );
				if( skillBookLearnedSkill == NULL ){
					//Only one piece of equipment provides this ability
					pSkillBarPanel->UnequipAllSkills( skillID );
				}
			}
		}
	}

	//If equipment with abilities swaps positions, the recorded ability-to-equipment-slot info must also be adjusted
	if( srcConIdx == E_CONSLOT_EQUIPPED && dstConIdx == E_CONSLOT_EQUIPPED ){
		mmoSkillMgr.SwapMyLearnedMainSkillEquipID( srcConIdx, dstConIdx );
	}
}
//
void MMOEquipManager::CheckEquipmentHandleWeaponMainSkill( INT toCheckEquipSkillEquipSlot )
{
	if( toCheckEquipSkillEquipSlot < 0 ){
		return;
	}

	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	TArray< UMMOAbility* > thisEquipSkillArray;
	mmoSkillMgr.GetEquipmentEquipSkillBy( toCheckEquipSkillEquipSlot, thisEquipSkillArray );

	//Which abilities this equipment has
	INT loopCnt = thisEquipSkillArray.Num();
	for( INT i = 0; i < loopCnt; ++i ){
		UMMOAbility *equipAblility = thisEquipSkillArray( i );
		if( equipAblility == NULL ){
			continue;
		}

		INT skillID = equipAblility->eventGetAbilityId();

		//
		TArray< INT > sameSkillEquipmentArray;
		mmoSkillMgr.GetEquippmentOfSameSkill( skillID, sameSkillEquipmentArray );

		UBOOL isEffected = FALSE;
		INT loopCnt2 = sameSkillEquipmentArray.Num();
		for( INT i = 0; i < loopCnt2; ++i ){
			INT sameSkillEquipSlotIdx = sameSkillEquipmentArray( i );
			FClientItemInfo* toCheckEquippedItem = mmoFacade.FindItem( E_CONSLOT_EQUIPPED, sameSkillEquipSlotIdx );
			if( toCheckEquippedItem == NULL ){
				continue;
			}

			if( toCheckEquippedItem->m_nCurDur > 0 ){
				isEffected = TRUE;
			}
		}

		//
		if( !isEffected ){
			UMMOAbility *skillBookLearnedSkill = mmoSkillMgr.FindMyLearnedMainSkillByBook( skillID );
			if( skillBookLearnedSkill == NULL ){
				//Only one piece of equipment provides this ability
				UMMOUISkillBar *pSkillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar );
				if( pSkillBarPanel != NULL ){
					pSkillBarPanel->UnequipAllSkills( skillID );
				}
			}
		}
	}
}
//======================================================================
UBOOL MMOEquipManager::ReadEquipLevelUpData()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOEquipManager::ReadEquipLevelUpData"), 0);

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("equip_levelup.txt"), TRUE ) ){
		return FALSE;
	}

	_EquipLevelUpCfg.Empty();

	UINT rowCnt = tabFile->GetRowCount();
	UINT skipRow = M_SKIP_ROWS;
	//Actual data starts from row 3; header row, N/A row
	_EquipLevelUpCfg.Add( rowCnt > skipRow ? (rowCnt - skipRow) : 0 );
	for( UINT i = skipRow ; i < rowCnt; ++i )
	{
		INT curLevel = appAtoi( tabFile->GetData( TEXT("EquipLevel"), i ) );
		EquipLvUpCfg *data = AddNewEquipLevelUpData( curLevel );
		if( data == NULL ){
			warnf( NAME_Error, TEXT("ReadEquipLevelUpData at %d alloc EquipLvUpCfg Failed."), curLevel );
			continue;
		}

		data->_Level = curLevel;
		data->_MnyCost = appAtoi( tabFile->GetData( TEXT("LevelupCost"), i ) );
		data->_PowerAdd = appAtoi( tabFile->GetData( TEXT("Pow"), i ) );
	}

	delete tabFile;
	return TRUE;
}

//=============================================================================================
const EquipLvUpCfg* MMOEquipManager::FindEquipLevelUpData( INT level )
{
	INT curDataCnt = _EquipLevelUpCfg.Num();
	if( level >= curDataCnt ){
		return NULL;
	}

	return &_EquipLevelUpCfg( level );
}

EquipLvUpCfg* MMOEquipManager::AddNewEquipLevelUpData( INT level )
{
	INT curDataCnt = _EquipLevelUpCfg.Num();
	if( level > curDataCnt ){
		_EquipLevelUpCfg.Add( level - curDataCnt );
	}

	curDataCnt = _EquipLevelUpCfg.Num();
	if( level > curDataCnt ){
		return NULL;
	}

	return &_EquipLevelUpCfg( level );
}

////=============================================================================================
//void EquipLevelUpDataProcessor::ParseData( EquipLvUpCfg* obj, class CTabFile* dataRow, int rowIdx );
//{
//	obj->_Level = TReadDataValue_Int<EquipLevelUpDataManager>(TEXT("EquipLevel"), dataRow, rowIdx);
//	obj->_MnyCost = TReadDataValue_Int<EquipLevelUpDataManager>(TEXT("LevelupCost"), dataRow, rowIdx);
//}