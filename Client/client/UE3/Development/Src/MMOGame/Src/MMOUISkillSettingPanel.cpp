#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "MMOChatManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOSkillManager.h"
#include "MMOEquipManager.h"
#include "ScaleformEngine.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"
//#include "UMMOKeyManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUISkillSettingPanel)

//do some data updates when displaying
void UMMOUISkillSettingPanel::PreOnShow(UBOOL v)
{
	Super::PreOnShow(v);

	if(v){
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_skillbook_op"));
		//If visible, refresh the UI display
		UpdatePanel( ESkillPnl_Active );

		//Close new ability prompt
		UMMOUIMenuBar *menuBarPanel = (UMMOUIMenuBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar_Menubar );
		if( menuBarPanel != NULL ){
			menuBarPanel->ShowMenuBarHint( EMenuType_Skill, EMenuType_Child_Skill, FALSE );
		}

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowSkillSettingPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else{
		HidePanel();
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_skillbook_cls"));
	}
}
void UMMOUISkillSettingPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
	}
}
void UMMOUISkillSettingPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL)
		return;

	switch(evt->GetEventType())
	{
	case GameEvent_SkillUiUpdate:
		{
			MMOGameEvent_SkillUiUpdate* skillEvent = (MMOGameEvent_SkillUiUpdate*)evt;
			switch(skillEvent->EventType)
			{
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillLearned:
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillForget:
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_EquipSkill:
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_EquipRuneSkill:
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillLevelUp:
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillSlotCmdExchanged:
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillExpChanged:
				{
					UpdateActiveSkillList();
				}
				break;
			}
		}
		break;
	default:
		break;
	}
}
//
void UMMOUISkillSettingPanel::HidePanel()
{
	//HideActiveSkillDetail();
}
//===============================================================================================================
void UMMOUISkillSettingPanel::UpdatePanel( INT panelIdx )
{
	UpdateActiveSkillPanel();

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_ShowSkillLevelUpPanel);
	MMOUI::PostGameEvent(&evtGuide);

}

//===============================================================================================================
//Active Skills
//===============================================================================================================
void UMMOUISkillSettingPanel::UpdateActiveSkillPanel()
{
	UpdateActiveSkillList();
	//
	//HideActiveSkillDetail();
	//
	if( ShowLearnedActiveSkillsChkBtn != NULL ){
		ShowLearnedActiveSkillsChkBtn->SetBool( TEXT("selected"), !IsShowAllActiveSkills );
	}
	
	if( ShowAllActiveSkillsChkBtn != NULL ){
		ShowAllActiveSkillsChkBtn->SetBool( TEXT("selected"), IsShowAllActiveSkills );
	}
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.SkillJobTitle")));
}
//===============================================================================================================
void UMMOUISkillSettingPanel::UpdateActiveSkillList()
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	MMOFacade &myFacade = MMOFacade::getSingleton();
	UMMOKeyManager &keyMgr = *UMMOKeyManager::getSingleton();

	TMap< INT, INT > equipSkillsMap;
	for( INT i = 0; i < SKILL_EQUIP_MAX; ++i ){
		INT slotDefaultHotKey = mmoSkillMgr.GetSkillbarSlotRealCmd( i );
		INT bindSkillID = mmoSkillMgr.GetEquippedSkillTemplateID( slotDefaultHotKey );

		equipSkillsMap.FindOrAdd( slotDefaultHotKey ) = bindSkillID;
	}

	//all learned skills
	TMap< INT, class UMMOAbility* > myLearnedMainSkills = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();
	UBOOL isShowAll = IsShowAllActiveSkills;

	//Get all skills to be showed and classificate
	TMap< INT, class UMMOAbility* > allMyMainSkillsClassed;//[SKILL_EQUIP_MAX];
	if( isShowAll ){
		TMap< INT, class UMMOAbility* > allMyMainSkills = MMOSkillManager::getSingleton().GetAllMainAbilitiesSorted( Player->GetClassId(), ESUIType_Active );
		for( TMap< INT, class UMMOAbility* >::TIterator it(allMyMainSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();
			//
			//if(  ( mainAbility->SpellCommonAttribute.CastType == E_spct_Active ) && 
			if( ( mainAbility->SpellCommonAttribute.QTEType == E_spqte_None ) && 
				( mainAbility->ClientCommonAttribute.ShowOnUI == true ) )
			{
				allMyMainSkillsClassed.FindOrAdd( it.Key() ) = it.Value();
				//INT skillSetIdx = mmoSkillMgr.GetSkillBindDefaultSlot( mainAbility->SpellCommonAttribute.BindSlot );
				//if( skillSetIdx < 0 || skillSetIdx >= SKILL_EQUIP_MAX ){
				//	continue;
				//}

				//
				//allMyMainSkillsClassed[skillSetIdx].FindOrAdd( it.Key() ) = it.Value();
			}
		}

		//Reportedly passive abilities also need to be shown
		TMap< INT, class UMMOAbility* > allMyPassiveSkills = MMOSkillManager::getSingleton().GetAllMainAbilitiesSorted( Player->GetClassId(), ESUIType_Passive );
		for( TMap< INT, class UMMOAbility* >::TIterator it(allMyPassiveSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();
			if( ( mainAbility->ClientCommonAttribute.ShowOnUI == true ) )
			{
				allMyMainSkillsClassed.FindOrAdd( it.Key() ) = it.Value();
			}
		}

		//Sort
		MMOSkillManager::getSingleton().SortAllAbilitiesByRequireLevel( allMyMainSkillsClassed );
	}
	else{
		//Show only learned ones
		//mmoSkillMgr.SortAllAbilitiesByRequireLevel( myLearnedMainSkills );
		for( TMap< INT, class UMMOAbility* >::TIterator it(myLearnedMainSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();
			//
			//if( ( mainAbility->SpellCommonAttribute.CastType == E_spct_Active ) && 
			if( ( mainAbility->SpellCommonAttribute.QTEType == E_spqte_None ) && 
				( mainAbility->ClientCommonAttribute.ShowOnUI == true ) )
			{
				allMyMainSkillsClassed.FindOrAdd( it.Key() ) = it.Value();
				//INT skillSetIdx = mmoSkillMgr.GetSkillBindDefaultSlot( mainAbility->SpellCommonAttribute.BindSlot );
				//if( skillSetIdx < 0 || skillSetIdx >= SKILL_EQUIP_MAX ){
				//	continue;
				//}
				////
				//allMyMainSkillsClassed[skillSetIdx].FindOrAdd( it.Key() ) = it.Value();
			}
		}
	}

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if( dataProvider == NULL ){
		return;
	}

	TMap< INT, class UMMOAbility* > &thisClassSkills = allMyMainSkillsClassed;
	INT loopAddedCnt = 0;
	for( TMap< INT, class UMMOAbility* >::TIterator it(thisClassSkills); it; ++it )
	{
		UMMOAbility *mainAbility = it.Value();

		//
		INT mainSkillID = mainAbility->eventGetAbilityId();
		UMMOAbility **ppMyLearnedSkill = myLearnedMainSkills.Find( mainSkillID );
		//Abilities in GetAllMainAbilitiesSorted have no expanded level upgrade data, so get the info from the table where ability levels are already expanded
		if( ppMyLearnedSkill != NULL ){
			mainAbility = *ppMyLearnedSkill;
		}
		//Hide unlearned ones
		UBOOL isDisabled = ( ppMyLearnedSkill == NULL );

		//Prepare data, put into the refresh array
		UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_SkillTree.SkillTreeAS3.ActiveSkillListItemSingleData") );
		//check(itemSingleObj != NULL);
		if(itemSingleObj == NULL){
			continue;
		}
		//
		itemSingleObj->SetInt( TEXT("ItemIdx"), loopAddedCnt );
		itemSingleObj->SetInt( TEXT("SlotType"), ESPType_SkillSetting );
		//
		UBOOL isNew = mmoSkillMgr.IsNewLearnedSkill( mainSkillID );
		itemSingleObj->SetBool( TEXT("IsNew"), isNew );
		//
		//UBOOL IsCurrent = ( pSlotEquippedSkillID != NULL && mainSkillID == *pSlotEquippedSkillID );
		//itemSingleObj->SetBool( TEXT("IsCurrent"), IsCurrent );
		//
		FString skillName = mainAbility->ClientCommonAttribute.AbilityName;
		itemSingleObj->SetString( TEXT("SkillName"), skillName );
		//
		INT skillLevel = ( ppMyLearnedSkill != NULL ? (*ppMyLearnedSkill)->SpellCommonAttribute.Level : 0 );
		itemSingleObj->SetInt( TEXT("SkillLv"), skillLevel );
		//
		INT skillRequireLevel = mainAbility->SpellCommonAttribute.RequireLevel;
		itemSingleObj->SetInt( TEXT("SkillRequireLv"), skillRequireLevel );
		//
		itemSingleObj->SetBool( TEXT("IsDisabled"), isDisabled );
		//
		INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, skillLevel, 0 );
		itemSingleObj->SetInt( TEXT("SkillID"), uniqueSkillID );
		//
		FString expStr;
		if( ppMyLearnedSkill != NULL ){
			//UMMOAbility* nextLvSkill = MMOFacade::getSingleton().GetAbilityWithRuneByNextLevel( *ppMyLearnedSkill );
			//if( nextLvSkill == NULL ){
			if( (*ppMyLearnedSkill)->SpellCommonAttribute.Level >= MAX_SPELL_LEVEL ){
				//Max level
				expStr = TEXT("--/--");
			}
			else{
				//SkillDynamicData* skillDynData = mmoSkillMgr.FindSkillExtraData( uniqueSkillID );
				SkillDynamicData* skillDynData = mmoSkillMgr.FindSkillExtraData( mainSkillID );
				if( skillDynData != NULL ){
					expStr = FString::Printf( TEXT("%d/%d"), skillDynData->_CurExp, (*ppMyLearnedSkill)->LvUpRequirePoints );
				}
				else{
					expStr = FString::Printf( TEXT("%d/%d"), 0, (*ppMyLearnedSkill)->LvUpRequirePoints );
				}
			}
		}
		else{
			expStr = TEXT("--/--");
		}
		itemSingleObj->SetString( TEXT("SkillExp"), expStr );
		////
		//UBOOL canLevelUp = !isDisabled;
		//UINT nextLevelSkillUniqueID = MMOSkillManager::CombinateAbilityID( mainSkillID, skillLevel+1, 0 );
		//const SpellProgessData* pLevelUpData = SpellProgessDataManager::getSingleton().FindData( nextLevelSkillUniqueID );
		//if( pLevelUpData != NULL ){
		//	//INT myCurLevelUpPts = myFacade.GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
		//	//canLevelUp = ( canLevelUp && ( myCurLevelUpPts >= pLevelUpData->ConsumePts ) );
		//	canLevelUp = ( canLevelUp && ( Player->ActorAttributes->LevelAttributes.Level >= pLevelUpData->RequireLevel ) );
		//}
		//else{
		//	canLevelUp = FALSE;
		//}
		//itemSingleObj->SetBool( TEXT("CanLvUp"), canLevelUp );
		//
		UBOOL isMaxLv = ( mmoSkillMgr.GetAbilityWithRuneByNextLevel( mainAbility ) == NULL );
		itemSingleObj->SetBool( TEXT("IsMaxLv"), isMaxLv );
		////Inscription ability
		uniqueSkillID = 0;
		INT runeSkillIdx = 0;
		UBOOL isSkillActive = FALSE;
		UMMOAbility* equpRuneSkill = MMOEquipManager::getSingleton().GetCurWeaponRuneSkillOfMainSkillID( mainSkillID, isSkillActive );
		if( isSkillActive == TRUE && equpRuneSkill != NULL ){
			INT runeSkillMainSkillID = equpRuneSkill->GetTemplateID();
			runeSkillIdx = equpRuneSkill->GetRuneSlot();
			uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, equpRuneSkill->SpellCommonAttribute.Level, runeSkillIdx );
		}
		itemSingleObj->SetInt( TEXT("RuneSkillID"), uniqueSkillID );
		itemSingleObj->SetString( TEXT("SkillInscriptionType"), appItoa( runeSkillIdx ) );
			
		//
		itemSingleObj->SetString( TEXT("SkillIconName"), mainAbility->GetAbilityIconName() );

		//
		dataProvider->SetElementObject( loopAddedCnt, itemSingleObj );
		++loopAddedCnt;
	}

	eventSetActiveSkillList( dataProvider );
}
////===============================================================================================================
//UBOOL UMMOUISkillSettingPanel::IsShowFitWeaponSkillHint( INT skillSetIdx )
//{
//	if( skillSetIdx < 0 || skillSetIdx >= SKILL_EQUIP_MAX ){
//		return FALSE;
//	}
//
//	if( !IsAutoCheckSkillStyle ){
//		return FALSE;
//	}
//
//	int slotHotKey = MMOSkillManager::getSingleton().GetSkillSlotBindEnum( skillSetIdx );
//	INT skillID = MMOSkillManager::getSingleton().GetEquippedSkillTemplateID( slotHotKey );
//
//	UBOOL isShow = FALSE;
//	UBOOL isSkillActive = FALSE;
//	UMMOAbility* equpRuneSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkill( skillSetIdx, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
//	//Check weapon rune
//	if( equpRuneSkill != NULL ){
//		//FClientItemInfo *itemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
//		INT runeSkillMainSkillID = equpRuneSkill->GetTemplateID();//equpRuneSkill->MainSkill->GetTemplateID();
//		if( ( runeSkillMainSkillID != skillID && isSkillActive == TRUE ) && skillID > 0 ){
//			//Ability pattern does not match
//			isShow = IsAutoCheckSkillStyle;
//		}
//	}
//
//	return isShow;
//}
//===============================================================================================================
void UMMOUISkillSettingPanel::ShowActiveSkillDetailOf( UGFxObject *mc, INT skillUniqueID, INT showPosX, INT showPosY )
{
	//
	UpdateActiveSkillDetail( skillUniqueID );
	//
	if( CurActiveDetalSkillMainSkillMc != NULL ){
		CurActiveDetalSkillMainSkillMc->SetBool( TEXT("selected"), FALSE );
	}

	CurActiveDetalSkillMainSkillMc = mc;
	if( CurActiveDetalSkillMainSkillMc != NULL ){
		CurActiveDetalSkillMainSkillMc->SetBool( TEXT("selected"), TRUE );
	}
	//
	INT panelPosX = GetInt( TEXT("x") );
	INT panelPosY = GetInt( TEXT("y") );
	INT subPanelPosX = ( ActiveSkillsListPanel != NULL ? ActiveSkillsListPanel->GetInt( TEXT("x") ) : GetInt( TEXT("x") ) );
	INT subPanelPosY = ( ActiveSkillsListPanel != NULL ? ActiveSkillsListPanel->GetInt( TEXT("y") ) : GetInt( TEXT("y") ) );
	INT screenSizeX = FGFxEngine::GetEngine()->GetRenderViewport()->GetSizeX();
	INT screenSizeY = FGFxEngine::GetEngine()->GetRenderViewport()->GetSizeY();

	//Determine X; if beyond the right edge of the screen, flip to display on the left
	INT tuneClap = 8;//8 is a correction, makes it appear closer when shown on the right
	INT listPanelArrowPosX = showPosX - tuneClap;
	INT listPanelPosX = listPanelArrowPosX + (ListArrowMcWidth - ListArrowOffsetX);
	if( panelPosX + subPanelPosX + listPanelPosX + ActiveSkillsDetaiListPanelWidth > screenSizeX ){
		listPanelArrowPosX -= ActiveSkillsListItemWidth + ListArrowMcWidth;
		listPanelArrowPosX += tuneClap;
		listPanelPosX = listPanelArrowPosX + ListArrowOffsetX - ActiveSkillsDetaiListPanelWidth;

		ActiveDetailSkillListPanelArrowMc->GotoAndStopI( 11 );
	}
	else{
		ActiveDetailSkillListPanelArrowMc->GotoAndStopI( 1 );
	}

	//Determine Y; if beyond the bottom of the screen, move up
	INT listPanelArrowPosY = showPosY;// + ActiveSkillsListItemHalfHeight - ListArrowMcHeight/2;
	INT listPanelPosY = listPanelArrowPosY - ListArrowOffsetY;
	if( panelPosY + subPanelPosY + listPanelPosY + ActiveSkillsDetaiListPanelHeight > screenSizeY ){
		listPanelPosY = listPanelArrowPosY + ListArrowMcHeight + ListArrowOffsetY - ActiveSkillsDetaiListPanelHeight;
	}
	//
	ActiveDetailSkillListPanelArrowMc->SetVisible( TRUE );
	ActiveDetailSkillListPanelArrowMc->SetPosition( listPanelArrowPosX, listPanelArrowPosY );
	//
	ActiveSkillsDetailListPanel->SetPosition( listPanelPosX, listPanelPosY );
	ActiveSkillsDetailListPanel->SetVisible( TRUE );
}

//
void UMMOUISkillSettingPanel::HideActiveSkillDetail()
{
	if( CurActiveDetalSkillMainSkillMc != NULL ){
		CurActiveDetalSkillMainSkillMc->SetBool( TEXT("selected"), FALSE );
		CurActiveDetalSkillMainSkillMc = NULL;
	}
	else{
		return;
	}
	//
	if( ActiveDetailSkillListPanelArrowMc != NULL ){
		ActiveDetailSkillListPanelArrowMc->SetVisible( FALSE );
	}
	//
	if( ActiveSkillsDetailListPanel != NULL ){
		ActiveSkillsDetailListPanel->SetVisible( FALSE );
	}
}
//
void UMMOUISkillSettingPanel::UpdateActiveSkillDetail( INT skillUniqueID )
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();

	//
	UMMOAbility *skill = mmoSkillMgr.FindAbilityTemplate( skillUniqueID );
	if( skill == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUISkillSettingPanel::UpdateActiveSkillDetail FindAbilityTemplate is NULL%d]"), skillUniqueID );
		return;
	}

	if( skill->MainSkill != NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUISkillSettingPanel::UpdateActiveSkillDetail skill is not a main skill]") );
	}

	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	INT childSkillCnt = skill->SubRuneList.Num();
	INT addCnt = 0;
	for( INT i = 0; i < childSkillCnt; ++i ){
		UMMOAbility *childSkill = skill->SubRuneList(i);
		if( childSkill == NULL ){
			continue;
		}
		//
		if(  childSkill->ClientCommonAttribute.ShowOnUI == FALSE ){
			continue;
		}

		//
		UGFxObject* itemObj = GFXPlayer->CreateObject( TEXT("UI_SkillTree.SkillTreeAS3.ActiveDetailSkillsListItemData") );
		//check(itemObj != NULL);
		if(itemObj == NULL)
			continue;
		//
		FString skillName = childSkill->ClientCommonAttribute.AbilityName;
		skillName += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillNameLinkChar") ), *skill->ClientCommonAttribute.AbilityName );

		itemObj->SetString( TEXT("SkillName"), skillName );
		////
		INT skillLevel = 1;//childSkill->SpellCommonAttribute.Level;
		//itemObj->SetInt( TEXT("SkillLv"), skillLevel );
		//
		INT skillID = childSkill->eventGetAbilityId();
		INT skillSubID = childSkill->GetRuneSlot();
		INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLevel, skillSubID );
		itemObj->SetInt( TEXT("SkillID"), uniqueSkillID );
		//
		itemObj->SetString( TEXT("SkillIconName"), childSkill->GetAbilityIconName() );

		dataProvider->SetElementObject( addCnt, itemObj );
		++addCnt;
	}

	eventSetDetailActiveSkillList( dataProvider );
}
//===============================================================================================================
void UMMOUISkillSettingPanel::ShowLearnedActiveSkills()
{
	if( IsShowAllActiveSkills == FALSE ){
		return;
	}
	IsShowAllActiveSkills = !ShowLearnedActiveSkillsChkBtn->GetBool( TEXT("selected") );
	
	//
	UpdateActiveSkillPanel();
}

void UMMOUISkillSettingPanel::ShowAllActiveSkills()
{
	if( IsShowAllActiveSkills == TRUE ){
		return;
	}
	IsShowAllActiveSkills = ShowAllActiveSkillsChkBtn->GetBool( TEXT("selected") );

	//
	UpdateActiveSkillPanel();
}

//===============================================================================================================
//Special Skills
//===============================================================================================================
void UMMOUISkillSettingPanel::UpdateSpecialSkillPanel()
{
	UpdateSpecialSkillList();
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.SkillInterTitle")));
}

//===============================================================================================================
void UMMOUISkillSettingPanel::UpdateSpecialSkillList()
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	MMOFacade &myFacade = MMOFacade::getSingleton();

	//
	INT maxSkillCnt;
	TArray< SpecialSkill > allSpecialSkills[ESpcSkill_MAX];
	allSpecialSkills[ESpcSkill_ShenFa] = mmoSkillMgr.GetSpecialSkillOfType( ESpcSkill_ShenFa );
	maxSkillCnt = allSpecialSkills[ESpcSkill_ShenFa].Num();

	allSpecialSkills[ESpcSkill_FanYing] = mmoSkillMgr.GetSpecialSkillOfType( ESpcSkill_FanYing );
	maxSkillCnt = maxSkillCnt >= allSpecialSkills[ESpcSkill_FanYing].Num() ? maxSkillCnt : allSpecialSkills[ESpcSkill_FanYing].Num();

	allSpecialSkills[ESpcSkill_ShengCun] = mmoSkillMgr.GetSpecialSkillOfType( ESpcSkill_ShengCun );
	maxSkillCnt = maxSkillCnt >= allSpecialSkills[ESpcSkill_ShengCun].Num() ? maxSkillCnt : allSpecialSkills[ESpcSkill_ShengCun].Num();

	allSpecialSkills[ESpcSkill_ZhanShu] = mmoSkillMgr.GetSpecialSkillOfType( ESpcSkill_ZhanShu );
	maxSkillCnt = maxSkillCnt >= allSpecialSkills[ESpcSkill_ZhanShu].Num() ? maxSkillCnt : allSpecialSkills[ESpcSkill_ZhanShu].Num();

	//
	TMap< INT, class UMMOAbility* > myLearnedMainSkills = mmoSkillMgr.GetMyLearnedMainSkills();

	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	for( INT i = 0; i < maxSkillCnt; ++i ){
		UGFxObject* itemObj = GFXPlayer->CreateObject( TEXT("UI_SkillTree.SkillTreeAS3.SpecialSkillListItemData") );
		//check(itemObj != NULL);
		if(itemObj == NULL)
			continue;

		INT SkowMask = 0;
		for( INT j = 0; j < ESpcSkill_MAX; ++j ){
			TArray< SpecialSkill > &typedSpecialSkills = allSpecialSkills[j];

			if( !typedSpecialSkills.IsValidIndex( i ) ){
				continue;
			}
			//
			SpecialSkill &curSpecialSkill = typedSpecialSkills(i);
			UINT skillUniqueID = MMOSkillManager::CombinateAbilityID( curSpecialSkill._SkillID, 1, 0 );
			UMMOAbility *skill = mmoSkillMgr.FindAbilityTemplate( skillUniqueID );
			if( skill != NULL ){
				//Prepare data, put into the refresh array
				UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_SkillTree.SkillTreeAS3.SpecialSkillListItemSingleData") );
				//check(itemSingleObj != NULL);
				if(itemSingleObj == NULL)
					continue;
				//
				SkowMask |= 1<<j;

				//
				itemSingleObj->SetInt( TEXT("ItemIdx"), j );
				//
				itemSingleObj->SetInt( TEXT("TipsType"), ESPType_SkillSetting );
				//
				UBOOL canDrag = ( j == ESpcSkill_ShengCun || j == ESpcSkill_ZhanShu );
				itemSingleObj->SetBool( TEXT("CanDrag"), canDrag );
				//
				UMMOAbility **ppMyLearnedSkill = myLearnedMainSkills.Find( curSpecialSkill._SkillID );
				//UBOOL isDisabled = ( ppMyLearnedSkill == NULL );
				UBOOL isDisabled = !canDrag;//For TEST!!!!!!!!!!!!!!!
				itemSingleObj->SetBool( TEXT("IsDisabled"), isDisabled );
				//
				FString skillName = skill->ClientCommonAttribute.AbilityName;
				itemSingleObj->SetString( TEXT("SkillName"), skillName );
				//
				FString skillHint;
				if( isDisabled ){
					skillHint = curSpecialSkill._GetRequire;
				}
				else{
					skillHint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.SpecialSkill_SkillLearned") );
				}
				itemSingleObj->SetString( TEXT("SkillRequire"), skillHint );
				//
				itemSingleObj->SetInt( TEXT("SkillID"), skillUniqueID );
				//
				itemSingleObj->SetString( TEXT("SkillIconName"), skill->GetAbilityIconName() );

				//
				FString objName = TEXT("SingleData");
				objName += appItoa(j+1);
				itemObj->SetObject( objName, itemSingleObj );
			}
			else{
				//itemSingleObj->SetBool( TEXT("CanDrag"), FALSE );
				//itemSingleObj->SetBool( TEXT("IsDisabled"), TRUE );
			}
		}//end of for j

		//
		itemObj->SetInt( TEXT("ShowMask"), SkowMask );
		//
		dataProvider->SetElementObject( i, itemObj );
	}// end of for i

	eventSetSpecialSkillList( dataProvider );
}

//========================================================================================================================================
UBOOL UMMOUISkillSettingPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{
	return FALSE;
} 

//
UBOOL UMMOUISkillSettingPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

//
UBOOL UMMOUISkillSettingPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	return FALSE;
}

//
UBOOL UMMOUISkillSettingPanel::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
									BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
									BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	//MMOFacade &mmoFacade = MMOFacade::getSingleton();
	//if( srcSlotType == ESPType_SkillSetting && ESPType_SkillBar == destSlotType && srcSlotMC != NULL && destSlotMC != NULL ){
	//	//Drag and drop new ability
	//	UINT uniqueSkillID = (UINT)srcSlotMC->GetInt(TEXT("itemIndex"));
	//	INT toEquipSkillSlotIdx = (INT)destSlotMC->GetInt( TEXT("slotIndex") );

	//	UMMOUISkillBar* skillbar = Cast<UMMOUISkillBar>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar ) );
	//	if( skillbar != NULL ){
	//		skillbar->SetBarSlot( toEquipSkillSlotIdx, ESBarSlotTp_Skill, uniqueSkillID );
	//	}

	//	return TRUE;
	//}

	return FALSE;
}
//===============================================================================================================
void UMMOUISkillSettingPanel::Init()
{
	Super::Init();

	Player = MMOFacade::getSingleton().GetMainPlayer();
	ctrl = MMOFacade::getSingleton().GetMainController();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_SkillUiUpdate);
	EventAdapter = new TGameEventAdapter<UMMOUISkillSettingPanel>(this, evts);

	//Active Skills
	InitActiveSkillMc();
}

//---------------------------------------------
//Active ability
void UMMOUISkillSettingPanel::InitActiveSkillMc()
{
	ActiveSkillsListPanel = GetObject( TEXT("ActiveSkillListPanelMc") );
	if( ActiveSkillsListPanel == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUISkillSettingPanel::Init ActiveSkillsListPanel is NULL]") );
		return;
	}

	//ActiveSkillsDetailListPanel = ActiveSkillsListPanel->GetObject( TEXT("DetailActiveSkillListPanelMc") );
	//ActiveSkillsDetaiListPanelWidth = ActiveSkillsDetailListPanel->GetInt( TEXT("width") );
	//ActiveSkillsDetaiListPanelHeight = ActiveSkillsDetailListPanel->GetInt( TEXT("height") );

	//ActiveDetailSkillListPanelArrowMc = ActiveSkillsListPanel->GetObject( TEXT("ActiveDetailSkillListPanelArrowMc") );
	//ListArrowMcWidth = ActiveDetailSkillListPanelArrowMc->GetInt( TEXT("width") );
	//ListArrowMcHeight = ActiveDetailSkillListPanelArrowMc->GetInt( TEXT("height") );

	CurActiveDetalSkillMainSkillMc = NULL;

	//ShowSkillSettingBtn = ActiveSkillsListPanel->GetObject( TEXT("ShowSkillSettingBtn") );
	//ShowSkillSettingBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillShowSkillSetting") ) );

	IsShowAllActiveSkills = FALSE;
	ShowLearnedActiveSkillsChkBtn = ActiveSkillsListPanel->GetObject( TEXT("ShowLearnedActiveSkillsChkBtn") );
	if( ShowLearnedActiveSkillsChkBtn != NULL ){
		ShowLearnedActiveSkillsChkBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillShowLearned") ) );
		ShowLearnedActiveSkillsChkBtn->SetBool( TEXT("selected"), !IsShowAllActiveSkills );
	}

	ShowAllActiveSkillsChkBtn = ActiveSkillsListPanel->GetObject( TEXT("ShowAllActiveSkillsChkBtn") );
	ShowAllActiveSkillsChkBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillShowAll") ) );
	ShowAllActiveSkillsChkBtn->SetBool( TEXT("selected"), IsShowAllActiveSkills );

	UGFxObject *panelHint = ActiveSkillsListPanel->GetObject( TEXT("txt_tip") );
	if( panelHint != NULL ){
		panelHint->SetText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillPanelHintText") ) );
	}
}

//===============================================================================================================
//===============================================================================================================
extern bool g_displayDragDropIcon;

//============================================================================================================================
void UMMOUISkillSettingPanel::ASLevelUpSkill( int uniqueSkillID )
{
	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_SkillLevelUp);
	MMOUI::PostGameEvent(&evtGuide);

	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer && mainPlayer->ActorAttributes->LevelAttributes.Level < SkillLevelUpLimitLv ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ERROR_skill_chara_low_level") ),MsgInfoType_Table,TEXT("MMOGame.SkillTree.ERROR_skill_chara_low_level") );
		return;
	}

	//
	UMMOAbility *skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( skill == NULL ){
		return;
	}
	UINT skillID = 0;
	UINT skillLv = 0;
	UINT skillRuneID = 0;
	MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );

	UMMOAbility *skillNextLevel = MMOSkillManager::getSingleton().FindAbilityTemplate( MMOSkillManager::CombinateAbilityID( skillID, skillLv+1, skillRuneID ) );
	if( skillNextLevel == NULL ){
		return;
	}
	//not enough cultivation points
	INT levelUpPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
	if( levelUpPts < skillNextLevel->LvUpRequirePoints ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ERROR_skill_level_up_needPts") ),MsgInfoType_Table,TEXT("MMOGame.SkillTree.ERROR_skill_level_up_needPts") );
		return;
	}
	//hide Tips
	MMOUI::GetUIManager()->delegateHideTips();

	CMessageHandler::GetInstance().Send_SpellLevelUp( (UINT16)skillID );
}

//
void UMMOUISkillSettingPanel::ASShowTips_SkillLvUpBtn( UGFxObject *mc, INT uniqueSkillID )
{
	if( mc == NULL ){
		return;
	}

	FString hint;
	//hint = UITextParser::HTMLBeginParagraph();
	//hint += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillLvUpHint") );
	//hint += UITextParser::HTMLEndParagraph();
	//cultivation points
	UMMOAbility *skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( skill == NULL ){
		return;
	}
	UMMOAbility *skillNextLevel = MMOSkillManager::getSingleton().GetAbilityByNextLevel( skill );
	if( skillNextLevel == NULL ){
		return;
	}
	//
	INT levelUpPts = skillNextLevel->LvUpRequirePoints;
	INT curLevelUpPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
	UBOOL isPtsLow = ( curLevelUpPts < levelUpPts );

	hint += UITextParser::HTMLBeginParagraph();
	if( isPtsLow ){
		hint += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")));
	}

	hint += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkill_level_up_needPts") ), levelUpPts );

	if( isPtsLow ){
		hint += UITextParser::HTMLEndFont();
	}
	hint += UITextParser::HTMLEndParagraph();

	//character level requirement tip
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	INT limitedLevel = ( SkillLevelUpLimitLv >= skillNextLevel->LvUpRequireRoleLevel ? SkillLevelUpLimitLv : skillNextLevel->LvUpRequireRoleLevel );
	if( mainPlayer && mainPlayer->ActorAttributes->LevelAttributes.Level < limitedLevel ){
		hint += UITextParser::HTMLBeginParagraph();
		hint += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")));
		hint += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillLvUpLimit") ), limitedLevel );
		hint += UITextParser::HTMLEndFont();
		hint += UITextParser::HTMLEndParagraph();
	}

	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//
void UMMOUISkillSettingPanel::ASHideTips_SkillLvUpBtn()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================================================================
void UMMOUISkillSettingPanel::ASShowTips_SkillTreeActiveSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOUISkillSettingPanel::ASHideTips_SkillTreeActiveSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================================
void UMMOUISkillSettingPanel::ASShowTips_SkillTreeRuneSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOUISkillSettingPanel::ASHideTips_SkillTreeRuneSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================================================================
void UMMOUISkillSettingPanel::ASSetSkill( INT skillSetIdx, INT uniqueSkillID )
{
	UMMOUISkillBar *skillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Skillbar );
	if( skillBarPanel != NULL ){
		INT curCmdKeyInSkillBarIdx = MMOSkillManager::getSingleton().GetSkillbarSlotIdxByKeyCmd( skillSetIdx );
		skillBarPanel->TryToEquipOnSkill( curCmdKeyInSkillBarIdx, uniqueSkillID );
	}
}
//============================================================================================================================
void UMMOUISkillSettingPanel::ASShowSkillDetail( UGFxObject *mc, INT skillUniqueID, INT showPosX, INT showPosY )
{
	//not needed
	//if( _SkillSetting == NULL ){
	//	return;
	//}

	//_SkillSetting->ShowActiveSkillDetailOf( mc, skillUniqueID, showPosX, showPosY );
}

//
void UMMOUISkillSettingPanel::ASHideActiveDetailSkillsListPanel()
{
	HideActiveSkillDetail();
}
//
void UMMOUISkillSettingPanel::ASOnScroolingActiveSkillList()
{
	HideActiveSkillDetail();
}
//============================================================================================================================
void UMMOUISkillSettingPanel::ASShowLearnedActiveSkills()
{
	ShowLearnedActiveSkills();
}

void UMMOUISkillSettingPanel::ASShowAllActiveSkills()
{
	ShowAllActiveSkills();
}

void UMMOUISkillSettingPanel::ASShowSkillSetting()
{
	if( ActiveSkillsDetailListPanel != NULL ){
		HideActiveSkillDetail();
	}

	//
	UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
	if( skillSettingPanel == NULL ){
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_SkillSetting,TRUE, "");
	}
	else{
		skillSettingPanel->Show( TRUE );
	}

}
void UMMOUISkillSettingPanel::ASClickTab(int nIndex)
{
	UpdatePanel(nIndex);
}

void UMMOUISkillSettingPanel::ASStartDrag(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	//Super::ASStartDrag( SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID );

	////
	//switch( SrcSlotType )
	//{
	//case ESPType_SkillSetting:
	//	{
	//		UMMOUISkillBar *pSkillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar );
	//		if( pSkillBarPanel == NULL ){
	//			break;
	//		}

	//		pSkillBarPanel->HintForDraggingItem( ESPType_SkillSetting, 0 );
	//	}
	//	break;
	//}
}
//===============================================================================================================
//===============================================================================================================