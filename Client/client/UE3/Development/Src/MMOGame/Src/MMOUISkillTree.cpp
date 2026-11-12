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

IMPLEMENT_CLASS(UMMOUISkillTree)

const INT nHeavenGasMainTapLevel = 60;
const INT nHeavenBloodTapLevel = 60;
//do some data updates when displaying
void UMMOUISkillTree::PreOnShow(UBOOL v)
{
	Super::PreOnShow(v);

	if(v){
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_skillbook_op"));
		//If visible, refresh the UI display
		UpdatePanel( ESkillPnl_HeavenMain );

		////Close new ability prompt
		//UMMOUIMenuBar *menuBarPanel = (UMMOUIMenuBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar_Menubar );
		//if( menuBarPanel != NULL ){
		//	menuBarPanel->HintNewSkillLearned( FALSE );
		//}
		//
		INT levelUpPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
		eventCallSetXiuLianValue(levelUpPts);
		//UpdatePanel( ESkillPnl_SpecialSkills );
	}
	else{
		HidePanel();
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenPractise,FALSE, "");
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_skillbook_cls"));
	}
}
void UMMOUISkillTree::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		/*AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if(mainPlayer&&mainPlayer->ActorAttributes)
		{
			if(mainPlayer->ActorAttributes->LevelAttributes.Level < 10)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.SkillTree.CanNotOpenHint")),MsgInfoType_Table,TEXT("MMOGame.SkillTree.CanNotOpenHint"));
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_SkillTree,FALSE, "");
				return;
			}
		}*/

	/*	FVector2D scrSz = MMOFacade::GetScreenSize();
		SetPosition((scrSz.X - eventGetWidth())/2,(scrSz.Y - eventGetHeight())/2);*/
	}
}
void UMMOUISkillTree::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL)
		return;

	switch(evt->GetEventType())
	{
	case GameEvent_Heaven:
		{
			MMOGameEvent_Heaven* pkEvent = (MMOGameEvent_Heaven*)evt;
			switch(pkEvent->EventType)
			{
			case MMOGameEvent_Heaven::EHeavenEvent_SynData:
				{
					if(_GasPractiseMain)
					{
						_GasPractiseMain->InitContainer(GFXPlayer);
					}
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_UpdateLevel:
				{
					UpdateHeavenMainPanel();
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_PractisePhrase:
				{/*
					if(_GasPractisePanel)
						_GasPractisePanel->eventCallUpdatePractisePhrase(MMOHeavenManager::getSingleton().GetPractisePhrase());	*/
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_UpdateExp:
				{
					if(_MainHeaven)
					{
						_MainHeaven->UpdateHeavenExpBar();
						_MainHeaven->eventCallUCPlayMainAnimation(0);
					}
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_UpdateGasItem:
				{
					if(_GasPractiseMain)
						_GasPractiseMain->UpdateSlotItem(GFXPlayer,pkEvent->SlotContainerID,pkEvent->SlotID);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_UpdateGasSlot:
				{
					if(_GasPractiseMain)
						_GasPractiseMain->UpdateSlotState(GFXPlayer,pkEvent->SlotContainerID,pkEvent->SlotID);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_TargetSlotState:
				{
					if(_GasPractiseMain)
						_GasPractiseMain->UpdateTargetState(GFXPlayer);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_BloodStateOpen:
				{
					if(_BloodPanel)
						_BloodPanel->eventCallUpdateHeavenBloodLock(pkEvent->SlotContainerID - 1,pkEvent->nValue);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_BloodStateUpdate:
				{
					MMOUI::GetUIManager()->delegateHideTips();
					if(_BloodPanel)
						_BloodPanel->eventCallUpdateBloodSlotState(pkEvent->SlotContainerID - 1,pkEvent->SlotID - 1,pkEvent->nValue);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_LogInfo:
				{
					if( _MainHeaven)
						 _MainHeaven->UpdateHeavenLogInfo(GFXPlayer);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_UpdateNeedTime:
				{
					UpdateHeavenMainPanel();
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_PractiseAnimation:
				{
					if(_GasPractiseMain)
						_GasPractiseMain->eventPlayAnimationGas(1,0,0);
					break;
				}
			case MMOGameEvent_Heaven::EHeavenEvent_MergeAnimation:
				{
					if(_GasPractiseMain)
						_GasPractiseMain->eventPlayAnimationGas(0,0,0);
					break;
				}
			default:
				break;
			}
		}
		break;
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
				{
					if( CurPanel == ESkillPnl_Active ){
						UpdateActiveSkillList();
					}
				}
				break;
			}
		}
		break;
	default:
		break;
	}
}
void UMMOUISkillTree::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	if(MoneyType == (BYTE)E_MONEYTYPE_SPELL_PTS)
	{
		eventCallSetXiuLianValue(MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)MoneyType));
		if(_MainHeaven)
			_MainHeaven->UpdateXiuLianValue();
	}
	else if (MoneyType == (BYTE)E_MONEYTYPE_SOUL_PTS)
	{
		if(_MainHeaven)
			_MainHeaven->UpdateSoulValue();
	}
}
//
void UMMOUISkillTree::HidePanel()
{
	switch( CurPanel )
	{
	case ESkillPnl_Active:
		{
			HideActiveSkillDetail();
		}
		break;
	case ESkillPnl_SpecialSkills:
		{
			//HideActiveSkillDetail();
		}
		break;
	case ESkillPnl_HeavenMain:
		{
		}
		break;
	case ESkillPnl_GasMain:
		{
		}
		break;
	case ESkillPnl_HeavenBlood:
		{
		}
		break;
	}
}
//===============================================================================================================
void UMMOUISkillTree::UpdatePanel( INT panelIdx )
{
	if( panelIdx < 0 || panelIdx >= MAX_SKILL_TREE_PANEL ){
		return;
	}

	CurPanel = panelIdx;
	INT nLevel = 1;
	if(Player&&Player->ActorAttributes)
		nLevel = Player->ActorAttributes->LevelAttributes.Level;

	UMMOUIMenuBar* menuBar = Cast<UMMOUIMenuBar>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Skillbar_Menubar));
	if (!menuBar)
	{
		return;
	}
	BOOL bSkill = menuBar->IsMenuActivated(EMenuType_Child_Skill);
	BOOL bHeavenMain = menuBar->IsMenuActivated(EMenuType_Child_Heaven);
	eventSetTabVisible(bSkill,bHeavenMain,nLevel >= nHeavenGasMainTapLevel,nLevel >= nHeavenBloodTapLevel);

	eventCallSetSkillTabState(panelIdx);
	BOOL bVisiGas = FALSE;
	switch( panelIdx )
	{
	case ESkillPnl_Active:
		{
			UpdateActiveSkillPanel();

			// Tutorial
			MMOGameEvent_Guide evtGuide(EGUET_ShowSkillLevelUpPanel);
			MMOUI::PostGameEvent(&evtGuide);
		}
		break;
	case ESkillPnl_SpecialSkills:
		{
			UpdateSpecialSkillPanel();

		}
		break;
	case ESkillPnl_HeavenMain:
		{
			UpdateHeavenMainPanel();

			// Tutorial
			MMOGameEvent_Guide evtGuide(EGUET_ShowHeavenPanel);
			MMOUI::PostGameEvent(&evtGuide);
		}
		break;
	case ESkillPnl_GasMain:
		{
			UpdateHeavenGasMainPanel();
			bVisiGas = TRUE;

			// Tutorial
			MMOGameEvent_Guide evtGuide(EGUET_ShowGasPraPanel);
			MMOUI::PostGameEvent(&evtGuide);
		}
		break;
	case ESkillPnl_HeavenBlood:
		{
			UpdateHeavenBloodPanel();
		}
		break;
	}
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenPractise,bVisiGas, "");
}

//===============================================================================================================
//Active Skills
//===============================================================================================================
void UMMOUISkillTree::UpdateActiveSkillPanel()
{
	UpdateActiveSkillList();
	//
	//HideActiveSkillDetail();
	//
	ShowLearnedActiveSkillsChkBtn->SetBool( TEXT("selected"), !IsShowAllActiveSkills );
	ShowAllActiveSkillsChkBtn->SetBool( TEXT("selected"), IsShowAllActiveSkills );
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.SkillJobTitle")));
}
//===============================================================================================================
void UMMOUISkillTree::UpdateActiveSkillList()
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
	TMap< INT, class UMMOAbility* > allMyMainSkillsClassed[SKILL_EQUIP_MAX];
	if( isShowAll ){
		TMap< INT, class UMMOAbility* > allMyMainSkills = MMOSkillManager::getSingleton().GetAllMainAbilitiesSorted( Player->GetClassId(), ESUIType_Active );
		for( TMap< INT, class UMMOAbility* >::TIterator it(allMyMainSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();
			//
			if(  ( mainAbility->SpellCommonAttribute.CastType == E_spct_Active )
				&& ( mainAbility->SpellCommonAttribute.QTEType == E_spqte_None ) 
				&& (  mainAbility->ClientCommonAttribute.ShowOnUI == true ) )
			{
				INT skillSetIdx = mmoSkillMgr.GetSkillBindDefaultSlot( mainAbility->SpellCommonAttribute.BindSlot );
				if( skillSetIdx < 0 || skillSetIdx >= SKILL_EQUIP_MAX ){
					continue;
				}

				//
				allMyMainSkillsClassed[skillSetIdx].FindOrAdd( it.Key() ) = it.Value();
			}
		}
	}
	else{
		//Show only learned ones
		//mmoSkillMgr.SortAllAbilitiesByRequireLevel( myLearnedMainSkills );
		for( TMap< INT, class UMMOAbility* >::TIterator it(myLearnedMainSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();
			//
			if(  ( mainAbility->SpellCommonAttribute.CastType == E_spct_Active )
				&& ( mainAbility->SpellCommonAttribute.QTEType == E_spqte_None ) 
				&& (  mainAbility->ClientCommonAttribute.ShowOnUI == true ) )
			{
				INT skillSetIdx = mmoSkillMgr.GetSkillBindDefaultSlot( mainAbility->SpellCommonAttribute.BindSlot );
				if( skillSetIdx < 0 || skillSetIdx >= SKILL_EQUIP_MAX ){
					continue;
				}
				//
				allMyMainSkillsClassed[skillSetIdx].FindOrAdd( it.Key() ) = it.Value();
			}
		}
	}

	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT addRowCnt = 0;

	for( INT i = 0; i < SKILL_EQUIP_MAX; ++i ){
		TMap< INT, class UMMOAbility* > &thisClassSkills = allMyMainSkillsClassed[i];
		if( thisClassSkills.Num() <= 0 ){
			continue;
		}

		//Do not insert data if not displayed
		UBOOL isHasData = FALSE;
		for( TMap< INT, class UMMOAbility* >::TIterator it(thisClassSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();

			//
			INT mainSkillID = mainAbility->eventGetAbilityId();
			UMMOAbility **ppMyLearnedSkill = myLearnedMainSkills.Find( mainSkillID );
			if( !isShowAll ){
				//Show only learned
				if( ppMyLearnedSkill != NULL ){
					isHasData = TRUE;
					break;
				}
			}
			else{
				isHasData = TRUE;
				break;
			}
		}

		if( isHasData == FALSE ){
			continue;
		}

		//Group
		UGFxObject* rowDataObj = GFXPlayer->CreateObject( TEXT("UI_SkillTree.SkillTreeAS3.ActiveSkillListItemData") );
		//check(rowDataObj != NULL);
		if(rowDataObj == NULL)
			continue;
		//create a new row for content
		INT loopAddedCnt = 0;
		UGFxObject* singleRowDatas = GFXPlayer->CreateArray();
		//check(singleRowDatas != NULL);
		if(singleRowDatas == NULL)
			continue;

		INT curCmdKey = mmoSkillMgr.GetSkillSlotDefaultBindEnum( i );
		INT *pSlotEquippedSkillID = equipSkillsMap.Find( curCmdKey );

		//ShowAllActiveSkillsChkBtn->GetBool( TEXT("selected") );
		for( TMap< INT, class UMMOAbility* >::TIterator it(thisClassSkills); it; ++it ){
			UMMOAbility *mainAbility = it.Value();

			//
			INT mainSkillID = mainAbility->eventGetAbilityId();
			UMMOAbility **ppMyLearnedSkill = myLearnedMainSkills.Find( mainSkillID );

			if( !isShowAll ){
				//Show only learned
				if( ppMyLearnedSkill == NULL ){
					continue;
				}
			}

			//Abilities in GetAllMainAbilitiesSorted have no expanded level upgrade data, so get the info from the table where ability levels are already expanded
			if( ppMyLearnedSkill != NULL ){
				mainAbility = *ppMyLearnedSkill;
			}
			//Hide unlearned ones
			UBOOL isDisabled = ( ppMyLearnedSkill == NULL );

			//Prepare data, put into the refresh array
			UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("UI_SkillTree.SkillTreeAS3.ActiveSkillListItemSingleData") );
			//check(itemSingleObj != NULL);
			if(itemSingleObj == NULL)
				continue;
			//
			itemSingleObj->SetInt( TEXT("ItemIdx"), loopAddedCnt );
			//
			UBOOL isNew = mmoSkillMgr.IsNewLearnedSkill( mainSkillID );
			itemSingleObj->SetBool( TEXT("IsNew"), isNew );
			//
			INT skillSetIdx = i;//mmoSkillMgr.GetSkillSlotDefaultBindEnum( mainAbility->SpellCommonAttribute.BindSlot );
			//itemSingleObj->SetInt( TEXT("SkillSlotType"), skillSetIdx );
			//
			UBOOL IsCurrent = ( pSlotEquippedSkillID != NULL && mainSkillID == *pSlotEquippedSkillID );
			itemSingleObj->SetBool( TEXT("IsCurrent"), IsCurrent );
			//
			FString skillName = mainAbility->ClientCommonAttribute.AbilityName;
			itemSingleObj->SetString( TEXT("SkillName"), skillName );
			//
			INT skillLevel = ( ppMyLearnedSkill != NULL ? (*ppMyLearnedSkill)->SpellCommonAttribute.Level : mainAbility->SpellCommonAttribute.Level );
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
			UBOOL canLevelUp = !isDisabled;
			UINT nextLevelSkillUniqueID = MMOSkillManager::CombinateAbilityID( mainSkillID, skillLevel+1, 0 );
			const SpellProgessData* pLevelUpData = SpellProgessDataManager::getSingleton().FindData( nextLevelSkillUniqueID );
			if( pLevelUpData != NULL ){
				//INT myCurLevelUpPts = myFacade.GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
				//canLevelUp = ( canLevelUp && ( myCurLevelUpPts >= pLevelUpData->ConsumePts ) );
				canLevelUp = ( canLevelUp && ( Player->ActorAttributes->LevelAttributes.Level >= pLevelUpData->RequireLevel ) );
			}
			else{
				canLevelUp = FALSE;
			}
			//canLevelUp = ( canLevelUp && ( levelUpPts >= mainAbility->LvUpRequirePoints ) );
			//canLevelUp = ( canLevelUp && ( Player->ActorAttributes->LevelAttributes.Level >= mainAbility->LvUpRequireRoleLevel ) );
			itemSingleObj->SetBool( TEXT("CanLvUp"), canLevelUp );
			//
			UBOOL isMaxLv = ( mmoSkillMgr.GetAbilityWithRuneByNextLevel( mainAbility ) == NULL );
			itemSingleObj->SetBool( TEXT("IsMaxLv"), isMaxLv );
			//Inscription ability
			uniqueSkillID = 0;
			INT runeSkillIdx = 0;
			UBOOL isSkillActive = FALSE;
			UMMOAbility* equpRuneSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillByCmdKey( curCmdKey, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
			if( isSkillActive == TRUE && equpRuneSkill != NULL ){
				INT runeSkillMainSkillID = equpRuneSkill->GetTemplateID();
				if( runeSkillMainSkillID == mainSkillID ){
					runeSkillIdx = equpRuneSkill->GetRuneSlot();
					uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, equpRuneSkill->SpellCommonAttribute.Level, runeSkillIdx );
				}
			}
			itemSingleObj->SetInt( TEXT("RuneSkillID"), uniqueSkillID );
			itemSingleObj->SetString( TEXT("SkillInscriptionType"), appItoa( runeSkillIdx ) );
			
			//
			itemSingleObj->SetString( TEXT("SkillIconName"), mainAbility->GetAbilityIconName() );

			//
			singleRowDatas->SetElementObject( loopAddedCnt, itemSingleObj );
			++loopAddedCnt;
		}

		//title
		rowDataObj->SetInt( TEXT("DataCnt"), loopAddedCnt );
		//type
		rowDataObj->SetInt( TEXT("TitleType"), curCmdKey );
		//titleText
		//FString keyNameLookUp = TEXT("MMOGame.SkillTree.ActiveSkillKeyTpye");
		//keyNameLookUp += appItoa( i );
		//rowDataObj->SetString( TEXT("TitleString"), *MMOUI::ParseLocalizedPropertyPath( keyNameLookUp ) );
		FString keyName;
		//INT curSkillInSkillBarIdx = mmoSkillMgr.GetSkillbarSlotIdxByKeyCmd( curCmdKey );
		//if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER){
		//	keyMgr.KeyToString(keyMgr.FindKeyworld(( UMMOKeyManager::EKeyFunction)( UMMOKeyManager::KF_BarSkillEx1 + curSkillInSkillBarIdx ), FALSE ), keyName, 1 );
		//}
		//else{
		//	keyMgr.KeyToString(keyMgr.FindKeyworld(( UMMOKeyManager::EKeyFunction)( UMMOKeyManager::KF_BarSkill1 + curSkillInSkillBarIdx ), FALSE ), keyName, 1 );
		//}
		rowDataObj->SetString( TEXT("TitleString"), keyName );
		//
		rowDataObj->SetObject( TEXT("SingleItemsData"), singleRowDatas );

		//
		dataProvider->SetElementObject( addRowCnt, rowDataObj );
		++addRowCnt;
	}

	eventSetActiveSkillList( dataProvider );
}
////===============================================================================================================
//UBOOL UMMOUISkillTree::IsShowFitWeaponSkillHint( INT skillSetIdx )
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
void UMMOUISkillTree::ShowActiveSkillDetailOf( UGFxObject *mc, INT skillUniqueID, INT showPosX, INT showPosY )
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
	INT subPanelPosX = ActiveSkillsListPanel->GetInt( TEXT("x") );
	INT subPanelPosY = ActiveSkillsListPanel->GetInt( TEXT("y") );
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
void UMMOUISkillTree::HideActiveSkillDetail()
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
void UMMOUISkillTree::UpdateActiveSkillDetail( INT skillUniqueID )
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();

	//
	UMMOAbility *skill = mmoSkillMgr.FindAbilityTemplate( skillUniqueID );
	if( skill == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUISkillTree::UpdateActiveSkillDetail FindAbilityTemplate is NULL%d]"), skillUniqueID );
		return;
	}

	if( skill->MainSkill != NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUISkillTree::UpdateActiveSkillDetail skill is not a main skill]") );
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
void UMMOUISkillTree::ShowLearnedActiveSkills()
{
	if( IsShowAllActiveSkills == FALSE ){
		return;
	}
	IsShowAllActiveSkills = !ShowLearnedActiveSkillsChkBtn->GetBool( TEXT("selected") );
	
	//
	UpdateActiveSkillPanel();
}

void UMMOUISkillTree::ShowAllActiveSkills()
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
void UMMOUISkillTree::UpdateSpecialSkillPanel()
{
	UpdateSpecialSkillList();
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.SkillInterTitle")));
}

//===============================================================================================================
void UMMOUISkillTree::UpdateSpecialSkillList()
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
				itemSingleObj->SetInt( TEXT("TipsType"), ESPType_SkillTree );
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
UBOOL UMMOUISkillTree::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE;
} 

//
UBOOL UMMOUISkillTree::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if( !mc ){
		return FALSE;
	}

	//
	AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( mainPlayerCtrl == NULL ){
		return FALSE;
	}
	if(CurPanel == ESkillPnl_GasMain && _GasPractiseMain)
		return _GasPractiseMain->OnLeftMouseClick(mc,SlotType,ContainerID,SlotID,FALSE);
	UINT uniqueSkillID = mc->GetInt("itemIndex");
	//UINT skillID;
	//UINT skillLv;
	//UINT skillRuneID;
	//MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );

	//INT skillType = mc->GetInt( TEXT("EquipID") );
	const SpecialSkill *specialSkill = MMOSkillManager::getSingleton().FindSpecialSkill( uniqueSkillID );
	if( specialSkill == NULL ){
		return TRUE;
	}

	//Whether already learned
	TMap< INT, class UMMOAbility* > myLearnedMainSkills = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();
	UBOOL isLearned = ( myLearnedMainSkills.Find( specialSkill->_SkillID ) != NULL );
	if( isLearned == FALSE ){
		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillSetting.Error_Skill_Not_Learned") );
		return TRUE;
	}

	//
	mainPlayerCtrl->UseAbilityById( specialSkill->_SkillID, mainPlayerCtrl->GetTraceActor(), FALSE );

	return TRUE;
}

//
UBOOL UMMOUISkillTree::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	if(CurPanel == ESkillPnl_GasMain && _GasPractiseMain)
		return _GasPractiseMain->OnRightMouseClick(mc,SlotType,ContainerID,SlotID,bDefault);
	if(CurPanel == ESkillPnl_HeavenMain && _MainHeaven)
		return _MainHeaven->OnRightMouseClick(mc,SlotType,ContainerID,SlotID,bDefault);
	return FALSE;
}

//
UBOOL UMMOUISkillTree::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
									BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
									BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}
	if(CurPanel == ESkillPnl_GasMain && _GasPractiseMain)
		return _GasPractiseMain->OnDragDrop(srcSlotMC,destSlotMC,srcSlotType,srcContainerID,srcSlotID,destSlotType,destContainerID,destSlotID);
	if(CurPanel == ESkillPnl_HeavenMain && _MainHeaven)
		return _MainHeaven->OnDragDrop(srcSlotMC,destSlotMC,srcSlotType,srcContainerID,srcSlotID,destSlotType,destContainerID,destSlotID);
	////
	//if( destSlotType == ESPType_ItemBar && srcSlotType == ESPType_SkillTree && srcSlotMC != NULL && destSlotMC != NULL ){
	//	//Get the ability to set on the item bar
	//	UINT uniqueSkillID = srcSlotMC->GetInt("itemIndex");
	//	const SpecialSkill *specialSkillInfo = MMOSkillManager::getSingleton().FindSpecialSkill( uniqueSkillID );
	//	if( specialSkillInfo == NULL ){
	//		return TRUE;
	//	}

	//	//Only survival abilities and tactical abilities can be dragged onto the item bar
	//	if( specialSkillInfo->_SpecialSkillType == ESpcSkill_ShengCun || specialSkillInfo->_SpecialSkillType == ESpcSkill_ZhanShu ){
	//		UMMOUISkillBar* skillbar = Cast<UMMOUISkillBar>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar ) );
	//		if( skillbar != NULL ){
	//			UMMOAbility *specialSkill = MMOSkillManager::getSingleton().FindMyLearnedMainSkill( specialSkillInfo->_SkillID );
	//			INT itemBarSlotIdx = destSlotMC->GetInt(TEXT("itemid"));

	//			skillbar->SetItemBarSkillData( destSlotMC , specialSkill , itemBarSlotIdx );
	//		}
	//	}

	//	return TRUE;
	//}

	return FALSE;
}
//===============================================================================================================
void UMMOUISkillTree::UpdateHeavenMainPanel()
{
	if(_MainHeaven)
	{
		_MainHeaven->UpdateHeavenMain();
		UGFxObject* object = GFXPlayer->CreateObject("UI_SkillTree.SkillTreeAS3.HeavenNextConvertData");
		_MainHeaven->UpdateNextConverter(object);
		_MainHeaven->UpdateHeavenLogInfo(GFXPlayer);
		_MainHeaven->UpdateSlotState(GFXPlayer);
	}

	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.WindowTitle")));
}
void UMMOUISkillTree::UpdateHeavenGasMainPanel()
{
	if(_GasPractiseMain)
	{
		_GasPractiseMain->InitContainer(GFXPlayer);
		_GasPractiseMain->UpdateHeavenGasMainPanel();
		_GasPractiseMain->UpdateTargetState(GFXPlayer);
		_GasPractiseMain->UpdateEquipSlotStateShow(GFXPlayer,sasbtEquip,-1);
		_GasPractiseMain->UpdateGasEquipPower();
	}
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasMainTitle")));
}
void UMMOUISkillTree::UpdateHeavenBloodPanel()
{
	if(_BloodPanel)
		_BloodPanel->UpdateHeavenBloodPanel(GFXPlayer);
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.BloodMainTitle")));
}
void UMMOUISkillTree::UpdateHeavenExchangePanel()
{
	eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ExchangeTitle")));
}
void UMMOUISkillTree::UpdateHeavenLog()
{
	if(_MainHeaven)
		_MainHeaven->UpdateHeavenLogInfo(GFXPlayer);
}
void UMMOUISkillTree::ConverterConfirmShow()
{
	if(_MainHeaven)
		_MainHeaven->ConverterConfirmShow();
}

void UMMOUISkillTree::OnClickOneKeyMerge()
{
	if(_GasPractiseMain)
		_GasPractiseMain->OnClickOneKeyMerge();
}
void UMMOUISkillTree::OnClickLevelState(INT nLevelIndex,INT nPanelIndex)
{
	if(_BloodPanel)
		_BloodPanel->OnClickLevelState(nLevelIndex,nPanelIndex);
}
void UMMOUISkillTree::OnClickActivateBlood(INT nTypeIndex)
{
	if(_BloodPanel)
		_BloodPanel->OnClickActivateBlood(nTypeIndex);
}
//===============================================================================================================
void UMMOUISkillTree::Init()
{
	Super::Init();

	Player = MMOFacade::getSingleton().GetMainPlayer();
	ctrl = MMOFacade::getSingleton().GetMainController();
	KeyMapManager = ctrl->KeyMapManager;

	//check(EventAdapter == NULL);
	//TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_MainPlayerDataChange);
	//EventAdapter = new TGameEventAdapter<UMMOUISkillPanel>(this, evts);
	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Heaven);
	evts.AddItem(GameEvent_SkillUiUpdate);
	EventAdapter = new TGameEventAdapter<UMMOUISkillTree>(this, evts);

	//Temporarily disabled
	UGFxObject *specialSkillTabBtn = GetObject( TEXT("tab2_mc") );
	if( specialSkillTabBtn != NULL ){
		specialSkillTabBtn->SetBool( TEXT("enabled"), FALSE );
	}

	//Active Skills
	InitActiveSkillMc();

	//heaven init
	if(_GasPractiseMain)
		_GasPractiseMain->eventInitHeavenGasMain();
}

//---------------------------------------------
//Active ability
void UMMOUISkillTree::InitActiveSkillMc()
{
	ActiveSkillsListPanel = GetObject( TEXT("ActiveSkillListPanelMc") );
	if( ActiveSkillsListPanel == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUISkillTree::Init ActiveSkillsListPanel is NULL]") );
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
	ShowLearnedActiveSkillsChkBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillShowLearned") ) );
	ShowLearnedActiveSkillsChkBtn->SetBool( TEXT("selected"), !IsShowAllActiveSkills );

	ShowAllActiveSkillsChkBtn = ActiveSkillsListPanel->GetObject( TEXT("ShowAllActiveSkillsChkBtn") );
	ShowAllActiveSkillsChkBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillShowAll") ) );
	ShowAllActiveSkillsChkBtn->SetBool( TEXT("selected"), IsShowAllActiveSkills );

	UGFxObject *panelHint = ActiveSkillsListPanel->GetObject( TEXT("txt_tip") );
	if( panelHint != NULL ){
		panelHint->SetText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillPanelHintText") ) );
	}
}
void UMMOUISkillTree::BagSlotChange(INT BagId,INT SlotId)
{
	if(BagId == E_CONSLOT_DEPOSIT&&_MainHeaven)
		_MainHeaven->UpdateSlotState(GFXPlayer);
}

//===============================================================================================================
//===============================================================================================================
void UMMOUISkillTree::ASLevelUpSkill( int uniqueSkillID )
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
void UMMOUISkillTree::ASShowTips_SkillLvUpBtn( UGFxObject *mc, INT uniqueSkillID )
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
void UMMOUISkillTree::ASHideTips_SkillLvUpBtn()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================================================================
void UMMOUISkillTree::ASShowTips_SkillTreeActiveSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOUISkillTree::ASHideTips_SkillTreeActiveSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================================
void UMMOUISkillTree::ASShowTips_SkillTreeRuneSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOUISkillTree::ASHideTips_SkillTreeRuneSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================================
void UMMOUISkillTree::ASSetSkill( INT skillSetIdx, INT uniqueSkillID )
{
	UMMOUISkillBar *skillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Skillbar );
	if( skillBarPanel != NULL ){
		INT curCmdKeyInSkillBarIdx = MMOSkillManager::getSingleton().GetSkillbarSlotIdxByKeyCmd( skillSetIdx );
		skillBarPanel->TryToEquipOnSkill( curCmdKeyInSkillBarIdx, uniqueSkillID );
	}
}
//============================================================================================================================
void UMMOUISkillTree::ASShowSkillDetail( UGFxObject *mc, INT skillUniqueID, INT showPosX, INT showPosY )
{
	//not needed
	//ShowActiveSkillDetailOf( mc, skillUniqueID, showPosX, showPosY );
}

//
void UMMOUISkillTree::ASHideActiveDetailSkillsListPanel()
{
	HideActiveSkillDetail();
}
//
void UMMOUISkillTree::ASOnScroolingActiveSkillList()
{
	HideActiveSkillDetail();
}
//============================================================================================================================
void UMMOUISkillTree::ASShowLearnedActiveSkills()
{
	ShowLearnedActiveSkills();
}

void UMMOUISkillTree::ASShowAllActiveSkills()
{
	ShowAllActiveSkills();
}

void UMMOUISkillTree::ASShowSkillSetting()
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
//-----------------------heaven----------------------------------------------
void UMMOUISkillTree::ASClickConverter()
{
	if(MMOHeavenManager::getSingleton().GetTodayConvertTimes() >= MMOHeavenManager::getSingleton().GetTodayConvertMaxTimes())
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.HeavenMessage.ConvertTimesLimited") ));
		return;
	}
	ConverterConfirmShow();
}
void UMMOUISkillTree::ASClickOneKeyMergeGas()
{
	OnClickOneKeyMerge();
}
void UMMOUISkillTree::ASClickBtnLevelState(INT nLevelIndex,INT nPanelIndex)
{
	OnClickLevelState(nLevelIndex + 1,nPanelIndex + 1);
}
void UMMOUISkillTree::ASActivateBlood(INT nType)
{
	OnClickActivateBlood(nType);
}
void UMMOUISkillTree::ASOneKeyGoBlood(INT nType)
{
	if(nType < 0 || nType > MAX_BLOODSEA_PAGE)
		return;
	std::vector<INT> nNeedFreshIndex;
	for(INT n = 0; n < MAX_BLOODSEA_SLOT_PER_PAGE; ++n)
	{
		const BloodSlot* pkSlot = MMOHeavenManager::getSingleton().GetBloodSlot(nType,n+1);
		if(pkSlot)
		{
			if(pkSlot->nValue <= 0)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.CannotAutoFreshBlood") ));
				return;
			}
			else if(pkSlot->nValue != MAX_BLOODSEA_SLOT_NUMBER)
			{
				nNeedFreshIndex.push_back(n+1);
			}
		}
	}
	for(INT n = 0; n < (INT)nNeedFreshIndex.size(); ++n)
	{
		CMessageHandler::GetInstance().Send_HeavenBloodLevel(nNeedFreshIndex[n],nType);
	}
}
void UMMOUISkillTree::ASClickTab(int nIndex)
{
	UpdatePanel(nIndex);
}
void UMMOUISkillTree::ASClickOpenGasPractisePanel()
{
	UMMOUIHeavenGasPractisePanel* pkAim = Cast<UMMOUIHeavenGasPractisePanel>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_HeavenPractise));
	if(pkAim&&pkAim->IsVisible)
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenPractise,FALSE, "");
	else
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenPractise,TRUE, "");
}
void UMMOUISkillTree::ASClickOpenHeavenLog()
{
	UpdateHeavenLog();
}
void UMMOUISkillTree::ASShowTips_HeavenLevel(class UGFxObject*mc ,INT nLevel)
{
	if( mc == NULL ){
		return;
	}
	FString hint;

	MMOHeavenManager::getSingleton().GetHeavenTips(hint,nLevel);

	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenLevel()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUISkillTree::ASUpdateNeedTime(int nTime)
{

}
void UMMOUISkillTree::ASShowTips_HeavenBloodSlot(class UGFxObject* mc,INT nSlotIndex,INT nPanelIndex)
{
	if( mc == NULL ){
		return;
	}
	FString hint;
	MMOHeavenManager::getSingleton().GetHeavenBloodTips(hint,nPanelIndex+1,nSlotIndex+1);
	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenBloodSlot()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUISkillTree::ASShowTips_HeavenExpBar(class UGFxObject* mc)
{
	if( mc == NULL ){
		return;
	}
	const HeavenLevelConfig* pkLevel =  MMOHeavenManager::getSingleton().GetHeavenCurLevelData();
	if(pkLevel == NULL)
		return;
	INT nExp = 0;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
		nExp = mainPlayer->ActorAttributes->heavenAttributes.heavenExp;

	FString hint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ExpValueBar") ),nExp,pkLevel->nMaxExp);

	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenExpBar()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUISkillTree::ASShowTips_HeavenGasSlot(class UGFxObject* mc)
{
	if( mc == NULL ){
		return;
	}
	INT containerId = mc->GetInt(TEXT("containerID"));
	INT nSlotID     = mc->GetInt(TEXT("slotID"));

	return;
	FString hint;
	const GasItem* pkItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(containerId,nSlotID);
	if(pkItem)
	{
		const HeavenGasItemConfig* pkTemp = MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
		if(pkTemp)
		{
			hint += pkTemp->name;
			hint += '\n';
			hint += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ExpValue")),pkItem->nCurExp);
			hint += '\n';
			hint += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasTemplateValue")),pkItem->nTempID);
		}
	}
	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenGasSlot()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUISkillTree::ASShowTips_HeavenGasExchangeSlot(class UGFxObject* mc,INT nGasID)
{
	if(mc == NULL)
		return;
	FString tips;
	MMOHeavenManager::getSingleton().GetHeavenGasTipsByGasID(tips,nGasID);
	mc->SetString( TEXT("tipsHtml"), tips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenExchangeGasSlot()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUISkillTree::ASShowTips_HeavenGasPower(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	FString tips;
	MMOHeavenManager::getSingleton().GetEquipGasPowerDescrible(tips);
	if(tips.IsEmpty())
		return;
	mc->SetString( TEXT("tipsHtml"), tips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenGasPower()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOUISkillTree::ASShowTips_HeavenHintCtrl(class UGFxObject* mc,INT nIdex)
{
	if (mc == NULL)
		return;
	FString hintTips;
	switch(nIdex)
	{
	case 0:hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintNormalXiuLian"));break;
	case 1:
		{
			hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintVip"));
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(mainPlayer&&mainPlayer->ActorAttributes&&mainPlayer->GetVipType() > 0)
				hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintVip"));
			break;
		}
	case 2:hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSoulPool"));break;
	case 3:hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintXiuLianPool"));break;
	case 4:
		{
			hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSpeedHint1"));
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(mainPlayer&&mainPlayer->ActorAttributes&&mainPlayer->GetVipType() > 0)
				hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSpeedHint2"));
			break;
		}
	case 5:
		{
			hintTips = TEXT("Converts automatically when the countdown ends");
			break;
		}
	}
	mc->SetString( TEXT("tipsHtml"), hintTips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOUISkillTree::ASHideTips_HeavenHintCtrl()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

extern bool g_displayDragDropIcon;

void UMMOUISkillTree::ASStartDrag(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	FString iconImage;

	GasItem*pkGasItem = NULL;
	UMMOAbility *skill = NULL;
	UItem* itemTemp = NULL;

	//whether it is an ability
	if(SrcSlotType == ESPType_SkillTree)
	{
		INT uniqueSkillID = SrcItemSlot->GetInt(TEXT("itemIndex"));
		skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
		iconImage = FString::Printf(TEXT("img://%s"), *skill->GetAbilityIconName());
	}
	else if(ESPType_HeavenGasEquip == SrcSlotType || ESPType_HeavenGasBag == SrcSlotType || ESPType_HeavenGasBank == SrcSlotType)
	{
		//whether it is realm qi
		pkGasItem = MMOHeavenManager::getSingleton().GetItemBySlot(SrcContainerID,SrcSlotID);
		//needs an icon
		if(pkGasItem)
		{
			const HeavenGasItemConfig*	pkGasTemp = MMOHeavenManager::getSingleton().GetGasTemplate(pkGasItem->nTempID);
			if(pkGasTemp)
				iconImage = FString::Printf(TEXT("../%s"),*(pkGasTemp->Icon0));
		}
	}
	else if (ESPType_HeavenSoul == SrcSlotType)
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEPOSIT, 0);
		if(info)
		{
			itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
			if(itemTemp)
			{
				iconImage = FString::Printf(TEXT("img://%s"), *MMOFacade::getSingleton().GetItemIconNamePersonalized( itemTemp ));
			}
		}
	}

	if( GFXPlayer != NULL && (skill || pkGasItem || itemTemp) )
	{
		UGFxObject* dropIcon = GFXPlayer->GetDragIcon();
		if(dropIcon)
		{
			g_displayDragDropIcon = true;
			FIntPoint pt = MMOFacade::GetMouseCursorPosition();
			dropIcon->SetPosition(pt.X , pt.Y);
			dropIcon->SetVisible(true);
			dropIcon->SetInt(TEXT("containerID"), E_CONSLOT_MAX);
			dropIcon->SetInt(TEXT("slotID"), -1);
			dropIcon->SetInt(TEXT("SlotType"), ESPType_Invalid);
			UGFxObject* uiLoader = dropIcon->GetObject( TEXT("IconLoader_mc") );
			if( uiLoader )
			{
				uiLoader->SetString(TEXT("source"), iconImage);
				//uiLoader->SetVisible((pkGasItem == NULL)?false:true);
			}
			UGFxObject* uiGasIcon = dropIcon->GetObject( TEXT("gasIcon_mc") );
			if(uiGasIcon)
			{
				if(_GasPractiseMain)
				{
					_GasPractiseMain->OnStartDragGas(SrcContainerID,SrcSlotID,TRUE);
				}
				// 
				// 				UGFxObject* gasIcon0 = uiGasIcon->GetObject( TEXT("par_star1") );
				// 				UGFxObject* gasIcon1 = uiGasIcon->GetObject( TEXT("par_star2") );
				// 				UGFxObject* gasIcon2 = uiGasIcon->GetObject( TEXT("par_ripple1") );
				// 				UGFxObject* gasIcon3 = uiGasIcon->GetObject( TEXT("par_ripple2") );
				// 
				// 				if(pkGasItem)
				// 				{
				// 					const HeavenGasItemConfig* pkGas = MMOHeavenManager::getSingleton().GetGasTemplate(pkGasItem->nTempID);
				// 					if(pkGas)
				// 					{
				// 						if(gasIcon0)
				// 							gasIcon0->SetString(TEXT("source"), pkGas->Icon1);
				// 						if(gasIcon1)
				// 							gasIcon1->SetString(TEXT("source"), pkGas->Icon1);
				// 						if(gasIcon2)
				// 							gasIcon2->SetString(TEXT("source"), pkGas->Icon0);
				// 						if(gasIcon3)
				// 							gasIcon3->SetString(TEXT("source"), pkGas->Icon0);
				// 
				// 						TArray<struct FASValue> args;
				// 						FASValue gasColor(EC_EventParm);
				// 						gasColor.Type = AS_Int;
				// 						gasColor.I = pkGas->nQuality;
				// 						args.AddItem(gasColor);
				// 						uiGasIcon->Invoke("SetIconType",args);
				// 					}
				// 				}
				uiGasIcon->SetVisible(false/*(pkGasItem != NULL)*/);
			}
		}

		MMOFacade::ShowMouseCursor(FALSE);
	}
}
//===============================================================================================================
//===============================================================================================================