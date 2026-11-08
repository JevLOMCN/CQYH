#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOSkillManager.h"
#include "CommonGameEvents.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
#include "MMOGameEvent_Splash.h"
#include "MMOQuestManager.h"
#include "MMOPowerManager.h"
#include "MMOClientVariableManager.h"
#include "UITextParser.h"
#include "MMOAchievementManager.h"
#include "MMOGuideManager.h"
#include "MMODataManager.h"

//IMPLEMENT_COMPARE_POINTER( UMMOAbility, __SortAbilityRequireLv, { return ( (A->SpellCommonAttribute.RequireLevel > B->SpellCommonAttribute.RequireLevel) ? 1 : -1 ); } );
IMPLEMENT_COMPARE_POINTER( UMMOAbility, __SortInUi, { return (A->SpellCommonAttribute.RequireLevel > B->SpellCommonAttribute.RequireLevel) ? \
																	(1) : ( (A->SpellCommonAttribute.RequireLevel < B->SpellCommonAttribute.RequireLevel) ? \
																			 (-1) : (A->SpellCommonAttribute.Id < B->SpellCommonAttribute.Id) ? \
																					(1) : (-1) ); } );

int  guiID = -1;

void MMOSkillManager::Init()
{
	//LoadAbilityTemplates();

	SCOPE_LOG_TIME(___INIT__, TEXT("MMOSkillManager::Init"), 0);

	MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)

	M_SCOPE_MEM_PROF(__MEMPROF__, TEXT("MMOSkillManager::Init"));

	SpellVarManager::getSingleton().InitFromFile(TEXT("spell_var.txt"), TRUE, TRUE);
	SpellProgessDataManager::getSingleton().InitFromFile(TEXT("spell_progress.txt"));

	AbilityTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("spell.txt"));	

	//
	ReflashSkillOutlineData();

	//
	LoadSpecialSkills();
	//
	LoadTransformSkillGroup();

	MMOUI::RegisterEventHandler( this, GameEvent_UnlockSkillSlot);
	MMOUI::RegisterEventHandler( this, GameEvent_LearnedNewSkill);
	MMOUI::RegisterEventHandler( this, GameEvent_ForgetSkill);
	MMOUI::RegisterEventHandler( this, GameEvent_SyncMyLearnedSkillsByBook);
	MMOUI::RegisterEventHandler( this, GameEvent_EquipSkill);
	MMOUI::RegisterEventHandler( this, GameEvent_EquipRuneSkill);
	MMOUI::RegisterEventHandler( this, GameEvent_SkillLevelUp);
	MMOUI::RegisterEventHandler( this, GameEvent_LearnedSkillExpChanged);
	MMOUI::RegisterEventHandler( this, GameEvent_EnterSelectingRole);
	MMOUI::RegisterEventHandler( this, GameEvent_ShowSkillRange);

	//
	_SkillSlotCmdDefault.FindOrAdd(0) = ECK_Slot1;
	_SkillSlotCmdDefault.FindOrAdd(1) = ECK_Slot2;
	_SkillSlotCmdDefault.FindOrAdd(2) = ECK_Slot3;
	_SkillSlotCmdDefault.FindOrAdd(3) = ECK_Slot4;
	_SkillSlotCmdDefault.FindOrAdd(4) = ECK_Slot5;
	_SkillSlotCmdDefault.FindOrAdd(5) = ECK_Slot6;
	//
	_SkillBarSlotRedirect[0] = GetSkillSlotDefaultBindEnum(0);
	_SkillBarSlotRedirect[1] = GetSkillSlotDefaultBindEnum(1);
	_SkillBarSlotRedirect[2] = GetSkillSlotDefaultBindEnum(2);
	_SkillBarSlotRedirect[3] = GetSkillSlotDefaultBindEnum(3);
	_SkillBarSlotRedirect[4] = GetSkillSlotDefaultBindEnum(4);
	_SkillBarSlotRedirect[5] = GetSkillSlotDefaultBindEnum(5);

	_SkillSlotActive.Empty();
	_SkillSlotFlag1.Empty();
	_SkillSlotIcon.Empty();

	MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)
}

void MMOSkillManager::ResetPersonalData()
{
	//变身后技能组
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( ctrl != NULL ){
		ctrl->SetTransformState( FALSE );
	}
	_InTransformSkillCache.Empty();
	//
	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if( mainActor != NULL ){
		mainActor->ResetAbilities();
	}
	//学会的主技能
	_MyLearnedMainSkills.Empty();
	_MyLearnedmainSkillsByBook.Empty();
	for( TMap< INT, TMap< INT, UMMOAbility* > >::TIterator it(_MyLearnedSkillsByEquip); it; ++it ){
		TMap< INT, UMMOAbility* > &equipAbilitiesMap = it.Value();
		equipAbilitiesMap.Empty();
	}
	_MyLearnedSkillsByEquip.Empty();
	//刚学会的技能
	_NewLearnedSkills.Empty();
	//刚解锁的技能槽
	_NewUnlockSkillSlots.Empty();
	//技能动态记录的数据
	_SkillDynData.Empty();
}

void MMOSkillManager::Shutdown()
{
	MMOUI::UnRegisterEventHandler( this, GameEvent_ShowSkillRange);
	MMOUI::UnRegisterEventHandler( this, GameEvent_EnterSelectingRole);
	MMOUI::UnRegisterEventHandler( this, GameEvent_LearnedSkillExpChanged);
	MMOUI::UnRegisterEventHandler( this, GameEvent_SkillLevelUp);
	MMOUI::UnRegisterEventHandler( this, GameEvent_EquipRuneSkill);
	MMOUI::UnRegisterEventHandler( this, GameEvent_EquipSkill);
	MMOUI::UnRegisterEventHandler( this, GameEvent_SyncMyLearnedSkillsByBook);
	MMOUI::UnRegisterEventHandler( this, GameEvent_ForgetSkill);
	MMOUI::UnRegisterEventHandler( this, GameEvent_LearnedNewSkill);
	MMOUI::UnRegisterEventHandler( this, GameEvent_UnlockSkillSlot);
	MMOUI::UnRegisterEventHandler( this, GameEvent_SyncEquippedWeapon);
	//
	for( AbilityTemplateMap::iterator itor = _abilityTemplateMap.begin(); itor != _abilityTemplateMap.end(); ++itor ){
		if( itor->second ){
			itor->second->RemoveFromRoot();
		}
	}

}
//=======================================================================================
void MMOSkillManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_SyncMyLearnedSkillsByBook:
		{
			MMOGameEvent_Any *syncSKillEvt = (MMOGameEvent_Any*)evt;
			//UINT skillKey = syncSKillEvt->IntData;
			AddMyLearnedMainSkillsBySkillBook();
			MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_SkillPow);
		}
		break;
	case GameEvent_UnlockSkillSlot:
		{
			MMOGameEvent_OneInt *newUnlockSlotEvt = (MMOGameEvent_OneInt*)evt;
			INT newSlotEnum = newUnlockSlotEvt->IntData;
			AddNewSkillSlot( newSlotEnum );

			//UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
			//if( skillSettingPanel != NULL && skillSettingPanel->IsVisible ){
			//	INT slotIdx = GetSkillSlotDefaultIdx( newSlotEnum );
			//	skillSettingPanel->UpdateSkill( slotIdx, -1 );
			//}
		}
		break;
	case GameEvent_LearnedNewSkill:
		{
			MMOGameEvent_ProcessDynSkill *newSkillEvt = (MMOGameEvent_ProcessDynSkill*)evt;
			INT isByEquipment = newSkillEvt->EquipItemID;

			UINT skillID = 0;
			UINT skillLv = 0;
			UINT skillRuneID = 0;
			MMOSkillManager::DecomposeAbilityID( newSkillEvt->UniqueSkillID, skillID, skillLv, skillRuneID );

			INT equipID = newSkillEvt->EquipItemID;
			HandleAllAddedMainSkills( skillID, skillLv, equipID );
		}
		break;
	case GameEvent_ForgetSkill:
		{
			MMOGameEvent_ProcessDynSkill *forgetSkillEvt = (MMOGameEvent_ProcessDynSkill*)evt;

			UINT skillID = 0;
			UINT skillLv = 0;
			UINT skillRuneID = 0;
			MMOSkillManager::DecomposeAbilityID( forgetSkillEvt->UniqueSkillID, skillID, skillLv, skillRuneID );

			INT equipID = forgetSkillEvt->EquipItemID;
			HandleAllDelMainSkills( skillID, equipID );
		}
		break;
	case GameEvent_EquipSkill:
		{
			MMOGameEvent_EquipSkill *equipSkillEvt = (MMOGameEvent_EquipSkill*)evt;
			//UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
			//if( skillSettingPanel != NULL && skillSettingPanel->IsVisible ){
			//	INT slotIdx = GetSkillSlotDefaultIdx( equipSkillEvt->SkillSlotEnum );
			//	skillSettingPanel->UpdateSkill( slotIdx, equipSkillEvt->SkillID );
			//}

			//
			MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_EquipSkill );
			uiEvt.MagicInt = equipSkillEvt->SkillID;
			MMOUI::PostGameEvent(&uiEvt);
		}
		break;
	case GameEvent_EquipRuneSkill:
		{
			MMOGameEvent_EquipRuneSkill* equipRuneSkillEvt = (MMOGameEvent_EquipRuneSkill*)evt;
			//UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
			//if( skillSettingPanel != NULL && skillSettingPanel->IsVisible ){
			//	INT slotIdx = GetSkillSlotDefaultIdx( equipRuneSkillEvt->SkillSlotEnum );
			//	skillSettingPanel->UpdateSkill( slotIdx, equipRuneSkillEvt->SkillID );
			//}
			//
			MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_EquipRuneSkill );
			uiEvt.MagicInt = equipRuneSkillEvt->SkillID;
			MMOUI::PostGameEvent(&uiEvt);
		}
		break;
	case GameEvent_SkillLevelUp:
		{
			MMOGameEvent_SkillLevelUp *lvUpSkillEvt = (MMOGameEvent_SkillLevelUp*)evt;
			INT toUpdateSkillID = lvUpSkillEvt->SkillID;
			INT toUpdateSkillLv = lvUpSkillEvt->NewLevel;

			INT toUpdateSkillEquipID = -1;
			if( lvUpSkillEvt->SkillRuneID == 0 ){
				//是主技能的更新,默认更新当前起效的技能
				UMMOAbility *selfLearnedAbility = FindMyLearnedMainSkillByBook( toUpdateSkillID );
				if( selfLearnedAbility != NULL ){
					toUpdateSkillEquipID = -1;
				}
				else{
					toUpdateSkillEquipID = FindCurrentActiveEquipSkill( toUpdateSkillID );
				}
			}
			
			UpdateAbility( lvUpSkillEvt->SkillID, 0, lvUpSkillEvt->NewLevel, toUpdateSkillEquipID );

			//相关的系统提示
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			UMMOAbility *ability = mainPlayer->GetMainAbilityById( lvUpSkillEvt->SkillID );
			if( ability != NULL ){
				FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillLvUpNotify") ), *(ability->ClientCommonAttribute.AbilityName), lvUpSkillEvt->NewLevel );
				//MMOUI::GetUIManager()->delegateAddScreenMessage( sMsg );
				MMOChatManager::getSingleton().AddSystemMessage( sMsg );
			}

			////技能设置面板
			//UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
			//if( skillSettingPanel != NULL && skillSettingPanel->IsVisible ){
			//	skillSettingPanel->UpdateCurSkills();
			//	skillSettingPanel->UpdateCurShowedSkillList();
			//}
				//
			MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillLevelUp );
			MMOUI::PostGameEvent(&uiEvt);

			//
			MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_SkillPow);

			// 引导任务
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_UpgradeSkill);
		}
		break;
	case GameEvent_LearnedSkillExpChanged:
		{
			MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillExpChanged );
			MMOUI::PostGameEvent(&uiEvt);
		}
		break;
	case GameEvent_EnterSelectingRole:
		{
			ResetPersonalData();
		}
		break;
	case GameEvent_ShowSkillRange:
		{
			if( TRUE ){
				//美术表现欠佳，策划要求暂时下架此功能
				break;
			}

			MMOGameEvent_OneInt *fbAchieveEvt = (MMOGameEvent_OneInt*)evt;
			INT mainSkillID = fbAchieveEvt->IntData;

			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if( mainPlayer == NULL || mainPlayer->MainController == NULL ){
				break;
			}

			//
			if( CurRangeShowedSkill != NULL ){
				mainPlayer->MainController->GuildAbilityInScene( CurRangeShowedSkill, FALSE );
				CurRangeShowedSkill = NULL;
			}

			//
			UMMOAbility* skill = mainPlayer->GetEquipedAbilityById( mainSkillID );
			if( skill == NULL ){
				break;
			}

			CurRangeShowedSkill = skill;
			mainPlayer->MainController->GuildAbilityInScene( CurRangeShowedSkill, TRUE );
		}
		break;
	}
}

//=======================================================================================
UBOOL MMOSkillManager::IsNewLearnedSkill( INT skillID )
{
	//UBOOL rst = FALSE;
	//INT cnt = _NewLearnedSkills.Num();
	//for( INT i = 0; i < cnt; ++i ){
	//	if( _NewLearnedSkills(i) == skillID ){
	//		rst = TRUE;
	//		break;
	//	}
	//}
	UBOOL rst = ( _NewLearnedSkills.Find( skillID ) != NULL );
	return rst;
}

UBOOL MMOSkillManager::IsNewLearnedSkillInSlot( INT slotEnum )
{
	UBOOL rst = FALSE;
	for( TMap< INT, INT >::TIterator it(_NewLearnedSkills); it; ++it ){
		UMMOAbility *ability = FindAbilityTemplate( it.Value() );
		if( ability == NULL ){
			continue;
		}

		UBOOL isBindInThis = IsSkillBindIn( ability->SpellCommonAttribute.BindSlot, slotEnum );
		if( isBindInThis == TRUE ){
			rst = TRUE;
			break;
		}
	}
	
	return rst;
}

//
void MMOSkillManager::AddNewLearnedSkill( UINT skillIDKey )
{
	//_NewLearnedSkills.Push( skillID );
	UMMOAbility *ability = FindAbilityTemplate( skillIDKey );
	if( ability == NULL ){
		return;
	}

	INT skillID = ability->eventGetAbilityId();
	_NewLearnedSkills.FindOrAdd( skillID ) = skillIDKey;
}

//
void MMOSkillManager::RemoveNewLearnedSkill( INT skillID )
{
	//INT cnt = _NewLearnedSkills.Num();
	//for( INT i = 0; i < cnt; ++i ){
	//	if( _NewLearnedSkills(i) == skillID ){
	//		_NewLearnedSkills.Remove( i );
	//		break;
	//	}
	//}
	_NewLearnedSkills.Remove( skillID );
}

//========================================================================================
UBOOL MMOSkillManager::IsNewUnlockSlot( INT slotEnum )
{
	//UBOOL rst = FALSE;
	//INT cnt = _NewUnlockSkillSlots.Num();
	//for( INT i = 0; i < cnt; ++i ){
	//	if( _NewUnlockSkillSlots(i) == slotEnum ){
	//		rst = TRUE;
	//		break;
	//	}
	//}
	UBOOL rst = ( _NewUnlockSkillSlots.Find( slotEnum ) != NULL );
	return rst;
}

//
void MMOSkillManager::AddNewSkillSlot( INT slotEnum )
{
	//_NewUnlockSkillSlots.Push( slotEnum );
	_NewUnlockSkillSlots.FindOrAdd( slotEnum );
}

void MMOSkillManager::RemoveNewSkillSlot( INT slotEnum )
{
	//INT cnt = _NewUnlockSkillSlots.Num();
	//for( INT i = 0; i < cnt; ++i ){
	//	if( _NewUnlockSkillSlots(i) == slotEnum ){
	//		_NewUnlockSkillSlots.Remove( i );
	//		break;
	//	}
	//}
	_NewUnlockSkillSlots.Remove( slotEnum );
}

//=========================================================================================
void MMOSkillManager::AddMyLearnedMainSkillsBySkillBook()
{
	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		return;
	}

	//
	for( TMap< INT, UMMOAbility* >::TIterator it(_MyLearnedmainSkillsByBook); it; ++it ){
		UMMOAbility *abilityLearnedByBook = it.Value();
		if( abilityLearnedByBook == NULL ){
			continue;
		}
		//
		INT skillID = abilityLearnedByBook->eventGetAbilityId();
		UMMOAbility *curMainPlayerActiveMainAbility = mainPlayer->GetMainAbilityById( skillID );
		if( curMainPlayerActiveMainAbility == NULL ){
			mainPlayer->AddAbility( abilityLearnedByBook );
		}
		else{
			INT equipID = FindCurrentActiveEquipSkill( skillID );
			if( equipID == -1 ){
				warnf( NAME_Error, TEXT("MMOSkillManager::AddMyLearnedMainSkillsBySkillBook main ability at ID[%d] add more than once, new Level[%d] to old level[%d].Function Returned"), 
					skillID, abilityLearnedByBook->SpellCommonAttribute.Level, abilityLearnedByBook->SpellCommonAttribute.Level );
				continue;
			}
			else{
				//来自装备的技能
				UpdateAbility( skillID, 0, abilityLearnedByBook->SpellCommonAttribute.Level );
			}
		}

		//
		_MyLearnedMainSkills.FindOrAdd( skillID ) = abilityLearnedByBook;
	}

	//sort
	_MyLearnedMainSkills.ValueSort<COMPARE_POINTER_CLASS(UMMOAbility, __SortInUi)>();
}
//==================================================================================================================
void MMOSkillManager::HandleAllAddedMainSkills( INT skillID, INT skillLv, INT equipID )
{
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv );

	UMMOAbility* toAddAbility = FindAbilityTemplate( uniqueSkillID );
	if( toAddAbility == NULL ){
		warnf( NAME_Error, TEXT("HandleAllAddedMainSkills NOT found ability at ID[%d] Level[%d]"), skillID, skillLv );
		return;
	}

	////
	//if( equipID != -1 ){
	//	//通过装备获得的
	//	UMMOAbility* myLearnedSkillByEquip = FindMyLearnedMainSkillByEquip( skillID );
	//	if( myLearnedSkillByEquip != NULL ){
	//		//已经通过其他装备获得过，需要更新等级
	//		INT curSkillLv = myLearnedSkillByEquip->SpellCommonAttribute.Level;
	//		if( skillLv > curSkillLv ){
	//			//前一次获得的技能等级较低，需要替换
	//			UpdateAbility( skillID, 0, curSkillLv );
	//			return;
	//		}
	//	}
	//}
	UBOOL isSkillNew = TRUE;
	if( equipID != -1 ){
		INT curActiveEquipSkillEquipID = FindCurrentActiveEquipSkill( skillID );
		isSkillNew = ( curActiveEquipSkillEquipID == -1 );

		AddMyLearnedMainSkillByEquip( uniqueSkillID, equipID );
	}
	else{
		AddMyLearnedMainSkillByBook( uniqueSkillID );
	}

	//选取真正起效的技能
	UINT toAddAbilityUniqueID;
	UMMOAbility *selfLearnedAbility = FindMyLearnedMainSkillByBook( skillID );
	if( selfLearnedAbility != NULL ){
		//自己学会的技能优先选
		toAddAbilityUniqueID = MMOSkillManager::CombinateAbilityID( selfLearnedAbility->eventGetAbilityId(), selfLearnedAbility->SpellCommonAttribute.Level );
	}
	else{
		//其次是装备上附带获取的技能
		INT curActiveEquipSkillEquipID = FindCurrentActiveEquipSkill( skillID );
		UMMOAbility *curActiveEquipAbility = FindMyLearnedMainSkillByEquip( skillID, curActiveEquipSkillEquipID );
		if( curActiveEquipAbility == NULL ){
			warnf( NAME_Error, TEXT("HandleAllAddedMainSkills NOT found ability From Equipment at ID[%d] Level[%d]"), skillID, skillLv );
			return;
		}
		toAddAbilityUniqueID = MMOSkillManager::CombinateAbilityID( curActiveEquipAbility->eventGetAbilityId(), curActiveEquipAbility->SpellCommonAttribute.Level );
	}

	UMMOAbility *curActiveAbilityLearned = FindMyLearnedMainSkill( skillID );
	if( curActiveAbilityLearned == NULL ){
		//新学会的技能，正常加入
		AddMyLearnedMainSkill( toAddAbilityUniqueID );
	}
	else{
		//已学会过的技能，只做更新
		UINT toUpdateSkillID = 0;
		UINT toUpdateSkillLv = 0;
		UINT noUsed = 0;
		MMOSkillManager::DecomposeAbilityID( toAddAbilityUniqueID, toUpdateSkillID, toUpdateSkillLv, noUsed );

		UpdateAbility( toUpdateSkillID, 0, toUpdateSkillLv, equipID );
		isSkillNew = FALSE;
	}

	//
	if( isSkillNew ){
		//新技能
		AddNewLearnedSkill( uniqueSkillID );

		MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillLearned );
		uiEvt.MagicInt = uniqueSkillID;
		MMOUI::PostGameEvent(&uiEvt);

		MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_SkillPow);

		//聊天频道提示
		FString sMsg;
		sMsg = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SkillBookLearned" )), *UITextParser::HTMLFormatMessageSkillColor( toAddAbility ) );
		if(skillID==3400)
		{
			return;
		}

		MMOChatManager::getSingleton().AddSystemMessage( sMsg ); 

		//特效提示
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if( mainPlayer == NULL ){
			return;
		}
		mainPlayer->PlaySkillLearnedEffect( mainPlayer->GetFootLocation(), mainPlayer->Rotation );
	}

	//
	UMMOAbility* ability = FindAbilityTemplate(uniqueSkillID);
	if (ability)
	{
		// 成就
		MMOGameEvent_Achievement evtAch1(adLearnSpell, skillID, 0, 0);
		MMOUI::PostGameEvent(&evtAch1);

		UINT32 abilityCount = 0;
		UINT32 activeAbilityCount = 0;
		TMap<INT, class UMMOAbility*> allLearnedSkills = GetMyLearnedMainSkills();
		for (TMap<INT, class UMMOAbility*>::TIterator iter(allLearnedSkills); iter; ++ iter)
		{
			UMMOAbility* mainAbility = iter.Value();
			if (!mainAbility)
			{
				continue;
			}

			if (mainAbility->bHasSpellProgressData && 
				mainAbility->ClientCommonAttribute.ShowOnUI)
			{
				abilityCount ++;

				if (mainAbility->SpellCommonAttribute.CastType == E_spct_Active)
				{
					activeAbilityCount ++;
				}
			}
		}
		MMOGameEvent_Achievement evtAch2(adSpellCount, abilityCount, 0, 0);
		MMOUI::PostGameEvent(&evtAch2);

		MMOGameEvent_Achievement evtAch3(adActiveSpellCount, activeAbilityCount, 0, 0);
		MMOUI::PostGameEvent(&evtAch3);

		// 新手引导
		MMOGameEvent_Guide evtGuide(EGUET_LearnSkill);
		MMOUI::PostGameEvent(&evtGuide);

		//  引导任务
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_LearnSkill, skillID);
	}
}

void MMOSkillManager::HandleAllDelMainSkills( INT skillID, INT equipID )
{
	if( equipID != -1 ){
		//通过装备获得的
		DelMyLearnedMainSkillByEquip( skillID, equipID );

		//如果有两件同一技能的装备
		INT curActiveEquipSkillEquipID = FindCurrentActiveEquipSkill( skillID );
		if( curActiveEquipSkillEquipID != -1 ){
			UMMOAbility *curActiveEquipAbility = FindMyLearnedMainSkillByEquip( skillID, curActiveEquipSkillEquipID );
			if( curActiveEquipAbility != NULL ){
				//另有一件装备提供此技能
				UMMOAbility *selfLearnedSkill = FindMyLearnedMainSkillByBook( skillID );
				if( selfLearnedSkill == NULL ){
					//此技能由装备学会，需要更新人物技能
					UpdateAbility( skillID, curActiveEquipAbility->SpellCommonAttribute.Level, curActiveEquipSkillEquipID );
				}
			}
		}
		else{
			//是否自身学过该类技能
			UMMOAbility *selfLearnedSkill = FindMyLearnedMainSkillByBook( skillID );
			if( selfLearnedSkill == NULL ){
				//没学过这技能，就得删除当前有效技能列表中删除
				DelMyLearnedMainSkill( skillID );

				//换装备时处理了
				MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillForget );
				uiEvt.MagicInt = skillID;
				MMOUI::PostGameEvent(&uiEvt);
			}
		}
	}
	else{
		//自身习得
		DelMyLearnedMainSkillByBook( skillID );
		//是否装备上也有这类技能可以代替使用
		INT curActiveEquipSkillEquipID = FindCurrentActiveEquipSkill( skillID );
		if( curActiveEquipSkillEquipID != -1 ){
			UMMOAbility *curEquipSkill = FindMyLearnedMainSkillByEquip( skillID, curActiveEquipSkillEquipID );
			if( curEquipSkill != NULL ){
				UpdateAbility( skillID, 0, curEquipSkill->SpellCommonAttribute.Level, curActiveEquipSkillEquipID );
			}
		}
		else{
			//也没有可用的装备技能
			DelMyLearnedMainSkill( skillID );
			//
			MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillForget );
			uiEvt.MagicInt = skillID;
			MMOUI::PostGameEvent(&uiEvt);
		}
	}

	//
	//DelSkillExtraData( skillID );

	//
	MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_SkillPow);
}
//==================================================================================================================
//现在起效的所有所学技能（自身学习和装备获得）
const TMap< INT, UMMOAbility* > &MMOSkillManager::GetMyLearnedMainSkills()
{
	return _MyLearnedMainSkills;
}
//-------------------------------------------------------------------------------------------------------------------
UMMOAbility* MMOSkillManager::FindMyLearnedMainSkill( INT mainSkillID )
{
	UMMOAbility **pAbility = _MyLearnedMainSkills.Find( mainSkillID );
	if( pAbility == NULL ){
		return NULL;
	}

	return *pAbility;
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::AddMyLearnedMainSkill( UINT uniqueSkillID )
{
	//UINT skillIDKey = MMOSkillManager::CombinateAbilityID( skillID, skillLv, 0 );
	UMMOAbility *toAddAbility = FindAbilityTemplate( uniqueSkillID );
	if( toAddAbility == NULL ){
		return;
	}
	UINT skillID = toAddAbility->eventGetAbilityId();

	//
	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		return;
	}
	UMMOAbility *curMainPlayerActiveMainAbility = mainPlayer->GetMainAbilityById( skillID );
	if( curMainPlayerActiveMainAbility == NULL ){
		mainPlayer->AddAbility( toAddAbility );
	}
	else{
		//mainPlayer->UpdateAbility( skillID, 0, toAddAbility->SpellCommonAttribute.Level );
		warnf( NAME_Error, TEXT("MMOSkillManager::AddMyLearnedMainSkill main ability at ID[%d] add more than once, new Level[%d] to old level[%d].Function Returned"), 
			skillID, toAddAbility->SpellCommonAttribute.Level, curMainPlayerActiveMainAbility->SpellCommonAttribute.Level );
		return;
	}

	//
	_MyLearnedMainSkills.FindOrAdd( skillID ) = toAddAbility;

	//sort
	_MyLearnedMainSkills.ValueSort<COMPARE_POINTER_CLASS(UMMOAbility, __SortInUi)>();
}

//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::DelMyLearnedMainSkill( UINT skillID )
{
	UMMOAbility *ability = FindMyLearnedMainSkill( skillID );
	if( ability == NULL ){
		return;
	}

	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		return;
	}
	mainPlayer->DelAbility( skillID, 0 );

	_MyLearnedMainSkills.Remove( skillID );
}
//==================================================================================================================
//自身习得的技能
UMMOAbility *MMOSkillManager::FindMyLearnedMainSkillByBook( INT skillID )
{
	UMMOAbility **pAbility = _MyLearnedmainSkillsByBook.Find( skillID );
	if( pAbility == NULL ){
		return NULL;
	}

	return *pAbility;
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::AddMyLearnedMainSkillByBook( UINT uniqueSkillID )
{
	UMMOAbility *toAddAbility = FindAbilityTemplate( uniqueSkillID );
	if( toAddAbility == NULL ){
		return;
	}

	//
	UINT skillID = toAddAbility->eventGetAbilityId();
	_MyLearnedmainSkillsByBook.FindOrAdd( skillID ) = toAddAbility;
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::DelMyLearnedMainSkillByBook( INT skillID )
{
	UMMOAbility *ability = FindMyLearnedMainSkillByBook( skillID );
	if( ability == NULL ){
		return;
	}

	_MyLearnedmainSkillsByBook.Remove( skillID );
	
}
//==================================================================================================================
//通过身上的装备暂时获得的技能
INT MMOSkillManager::FindCurrentActiveEquipSkill( INT skillID )
{
	TMap< INT, UMMOAbility* > *pEquipAbilitiesMap = _MyLearnedSkillsByEquip.Find( skillID );
	if( pEquipAbilitiesMap == NULL ){
		return -1;
	}

	TMap< INT, UMMOAbility* > &equipAbilitiesMap = *pEquipAbilitiesMap;
	if( equipAbilitiesMap.Num() <= 0 ){
		return -1;
	}

	UMMOAbility *curActiveEquipAblility = NULL;
	INT curActiveEquipID = -1;
	for( TMap< INT, UMMOAbility* >::TIterator it(equipAbilitiesMap); it; ++it ){
		UMMOAbility *equipAblility = it.Value();
		if( equipAblility == NULL ){
			continue;
		}

		if( curActiveEquipAblility == NULL ){
			curActiveEquipID = it.Key();
			curActiveEquipAblility = equipAblility;
		}

		if( curActiveEquipAblility->SpellCommonAttribute.Level < equipAblility->SpellCommonAttribute.Level ){
			curActiveEquipID = it.Key();
			curActiveEquipAblility = equipAblility;
		}
	}

	return curActiveEquipID;
}
//通过身上的装备暂时获得的技能
void MMOSkillManager::GetEquipmentEquipSkillBy( INT onEquipSlotIdx, TArray<UMMOAbility*> &equipSkillArray )
{
	equipSkillArray.Empty();

	for( TMap< INT, TMap< INT, UMMOAbility* > >::TIterator mapIt(_MyLearnedSkillsByEquip); mapIt; ++mapIt ){
		TMap< INT, UMMOAbility* > &equipAbilitiesMap = mapIt.Value();
		if( equipAbilitiesMap.Num() <= 0 ){
			continue;
		}

		//
		for( TMap< INT, UMMOAbility* >::TIterator abilityIt(equipAbilitiesMap); abilityIt; ++abilityIt ){
			if( onEquipSlotIdx != abilityIt.Key() ){
				continue;
			}

			UMMOAbility *equipAblility = abilityIt.Value();
			equipSkillArray.AddItem( equipAblility );
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::GetEquippmentOfSameSkill( INT skillID, TArray<INT> &sameSkillEquipIDArray )
{
	sameSkillEquipIDArray.Empty();

	TMap< INT, UMMOAbility* > equipAbilitiesMap;
	equipAbilitiesMap = _MyLearnedSkillsByEquip.FindOrAdd( skillID );
	if( equipAbilitiesMap.Num() <= 0 ){
		return;
	}

	for( TMap< INT, UMMOAbility* >::TIterator abilityIt(equipAbilitiesMap); abilityIt; ++abilityIt ){
		sameSkillEquipIDArray.AddItem( abilityIt.Key() );
	}
}
//-------------------------------------------------------------------------------------------------------------------
UMMOAbility *MMOSkillManager::FindMyLearnedMainSkillByEquip( INT skillID, INT equipID )
{
	TMap< INT, UMMOAbility* > equipAbilitiesMap;
	equipAbilitiesMap = _MyLearnedSkillsByEquip.FindOrAdd( skillID );
	if( equipAbilitiesMap.Num() <= 0 ){
		return NULL;
	}

	UMMOAbility *equipAblility = equipAbilitiesMap.FindRef( equipID );
	return equipAblility;
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::AddMyLearnedMainSkillByEquip( UINT uniqueSkillID, INT equipID )
{
	UMMOAbility *ability = FindAbilityTemplate( uniqueSkillID );
	if( ability == NULL ){
		return;
	}

	//
	UINT skillID = ability->eventGetAbilityId();

	TMap< INT, UMMOAbility* > &equipAbilitiesMap = _MyLearnedSkillsByEquip.FindOrAdd( skillID );
	equipAbilitiesMap.FindOrAdd( equipID ) = ability;

	////sort
	//_MyLearnedSkillsByEquip.ValueSort<COMPARE_POINTER_CLASS(UMMOAbility, __SortInUi)>();
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::DelMyLearnedMainSkillByEquip( INT skillID, INT equipID )
{
	TMap< INT, UMMOAbility* > &equipAbilitiesMap = _MyLearnedSkillsByEquip.FindOrAdd( skillID );
	if( equipAbilitiesMap.Num() <= 0 ){
		return;
	}

	equipAbilitiesMap.Remove( equipID );
}
//===================================================================================================================
void MMOSkillManager::SwapMyLearnedMainSkillEquipID( INT srcEquipID, INT dstEquipID )
{
	TArray<UMMOAbility*> srcEquipSkillArray;
	GetEquipmentEquipSkillBy( srcEquipID, srcEquipSkillArray );

	TArray<UMMOAbility*> dstEquipSkillArray;
	GetEquipmentEquipSkillBy( dstEquipID, dstEquipSkillArray );

	//Dest
	INT loopCnt = srcEquipSkillArray.Num();
	for( INT i = 0; i < loopCnt; ++i ){
		UMMOAbility *equipAblility = srcEquipSkillArray( i );
		if( equipAblility == NULL ){
			continue;
		}

		INT skillID = equipAblility->eventGetAbilityId();
		INT skillLv = equipAblility->SpellCommonAttribute.Level;

		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv );
		AddMyLearnedMainSkillByEquip( uniqueSkillID, dstEquipID );
	}

	//Src
	loopCnt = dstEquipSkillArray.Num();
	for( INT i = 0; i < loopCnt; ++i ){
		UMMOAbility *equipAblility = dstEquipSkillArray( i );
		if( equipAblility == NULL ){
			continue;
		}

		INT skillID = equipAblility->eventGetAbilityId();
		INT skillLv = equipAblility->SpellCommonAttribute.Level;

		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv );
		AddMyLearnedMainSkillByEquip( uniqueSkillID, srcEquipID );
	}
}
//=========================================================================================
void MMOSkillManager::SetMySkillExp( INT skillID, UINT exp )
{
	SkillDynamicData *data = FindSkillExtraData( skillID );
	if( data == NULL ){
		data = &_SkillDynData.FindOrAdd( skillID );
	}

	data->_CurExp = exp;
}
//-------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::DelSkillExtraData( INT skillID )
{
	_SkillDynData.Remove( skillID );
}
//-------------------------------------------------------------------------------------------------------------------
SkillDynamicData* MMOSkillManager::FindSkillExtraData( INT skillID )
{
	SkillDynamicData *data = _SkillDynData.Find( skillID );
	return data;
}
//-------------------------------------------------------------------------------------------------------------------
SkillDynamicData* MMOSkillManager::FindSkillExtraData( UMMOAbility *pAbility )
{
	if( pAbility == NULL ){
		return NULL;
	}

	//INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( pAbility->eventGetAbilityId(), pAbility->SpellCommonAttribute.Level, 0 );
	//return FindSkillExtraData( uniqueSkillID );
	return FindSkillExtraData( pAbility->eventGetAbilityId() );
}
//=========================================================================================
INT MMOSkillManager::GetSkillSlotDefaultBindEnum( INT idx )
{
	INT *value = _SkillSlotCmdDefault.Find(idx);
	if( value == NULL ){
		return -1;
	}

	return *value;
}

INT MMOSkillManager::GetSkillSlotDefaultIdx( INT bindEnum )
{
	const INT *value = _SkillSlotCmdDefault.FindKey( bindEnum );
	if( value == NULL ){
		return -1;
	}

	return *value;
}

//==========================================================================================
INT MMOSkillManager::GetSkillbarSlotRealCmd( INT slotIdx )
{
	if( slotIdx < 0 || slotIdx >= UCONST_SkillBarSlotMax ){
		return -1;
	}

	return _SkillBarSlotRedirect[slotIdx];
}

INT MMOSkillManager::GetSkillbarSlotIdxByKeyCmd( INT cmdIdx )
{
	for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i)
	{
		if( _SkillBarSlotRedirect[i] == cmdIdx ){
			//slot一一对应cmdIdx
			return i;
		}
	}

	return -1;
}
//===========================================================================
UBOOL MMOSkillManager::RedirectSkillSlot( INT slotIdx, INT toCmdKey, UBOOL isNeedReflashUI/* = TRUE*/ )
{
	if( slotIdx < 0 || slotIdx >= UCONST_SkillBarSlotMax ){
		return FALSE;
	}
	if( slotIdx == 0 || toCmdKey == ECK_Slot1 ){
		//0键不能换
		return FALSE;
	}

	if( toCmdKey < ECK_Slot1 || toCmdKey > ECK_Slot6 ){
		//默认只能这几个键
		return FALSE;
	}
	_SkillBarSlotRedirect[slotIdx] = toCmdKey;

	//
	if( isNeedReflashUI ){
		MMOGameEvent_OneInt evt( GameEvent_ChangeSkillSlot, toCmdKey );
		MMOUI::PostGameEvent(&evt);
	}

	return TRUE;
}

//-------------------------------------------------------------------------------
UBOOL MMOSkillManager::ExchangeRedirectSkillSlot( INT slotIdx1, INT slotIdx2, UBOOL isNeedReflashUI/* = TRUE*/ )
{
	if( slotIdx1 < 0 || slotIdx1 >= UCONST_SkillBarSlotMax ){
		return FALSE;
	}
	if( slotIdx2 < 0 || slotIdx2 >= UCONST_SkillBarSlotMax ){
		return FALSE;
	}

	if( slotIdx1 == 0 || slotIdx2 == 0 ){
		//0键不能换
		return FALSE;
	}

	INT origSlot1CmdKey = _SkillBarSlotRedirect[slotIdx1];
	_SkillBarSlotRedirect[slotIdx1] = _SkillBarSlotRedirect[slotIdx2];
	_SkillBarSlotRedirect[slotIdx2] = origSlot1CmdKey;
	//
	if( isNeedReflashUI ){
		MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_SkillSlotCmdExchanged );
		uiEvt.MagicInt = _SkillBarSlotRedirect[slotIdx1];
		uiEvt.MagicInt2 = _SkillBarSlotRedirect[slotIdx2];
		MMOUI::PostGameEvent(&uiEvt);
	}

	return TRUE;
}
//=======================================================================================
void MMOSkillManager::ReflashSkillSlotRedirection( INT32* addr, INT size )
{
	//BYTE* pp = (BYTE*)addr;
	//if( !pp || size <= 0 ){
	//	return;
	//}

	//const INT serverSingleDataSize = sizeof( INT32 );
	//CONST INT needInt32bufSize = ( UCONST_SkillBarSlotMax * sizeof( BYTE ) + serverSingleDataSize - 1 ) / serverSingleDataSize;
	//if( ! (size >= needInt32bufSize) ){
	//	warnf( NAME_Error, TEXT("[MMOSkillManager::ReflashSkillSlotRedirection] size WRONG need %d  But Get %d"), needInt32bufSize, size );
	//	return;
	//}
	INT32* pp = (INT32*)addr;
	if( !pp || size <= 0 ){
		return;
	}

	const INT serverSingleDataSize = sizeof( INT32 );
	CONST INT needInt32bufSize = UCONST_SkillBarSlotMax * sizeof( serverSingleDataSize );
	if( ! (size >= needInt32bufSize) ){
		warnf( NAME_Error, TEXT("[MMOSkillManager::ReflashSkillSlotRedirection] size WRONG need %d  But Get %d"), needInt32bufSize, size );
		return;
	}
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i ){
		INT serverData= pp[i];
		if( serverData < ECK_Slot1 || serverData > ECK_Slot12 ){
			//数据错了，用默认的
			continue;
		}
		RedirectSkillSlot( i, (ECommandKey)serverData );
	}
}

//-------------------------------------------------------------------------------
void MMOSkillManager::UploadSkillSlotRedirection()
{
	////const INT serverSingleDataSize = sizeof( INT32 );
	////CONST INT buf_size = ( ( ECVT_SkillBarSkillEnd - ECVT_SkillBarSkillBegin + 1 ) * sizeof( BYTE ) + serverSingleDataSize - 1 ) / serverSingleDataSize;
	////INT32 buf[buf_size] = {0};

	////BYTE* pSkillSlotDirection = (BYTE*)buf;
	////for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i)
	////{
	////	pSkillSlotDirection[i] = (BYTE)_SkillBarSlotRedirect[i];
	////}
	//const INT serverSingleDataSize = sizeof( INT32 );
	//CONST INT buf_size = ( ECVT_SkillBarSkillEnd - ECVT_SkillBarSkillBegin + 1 ) * serverSingleDataSize;
	//BYTE buf[buf_size] = {0};

	//INT32* pSkillSlotDirection = (INT32*)buf;
	//for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i)
	//{
	//	pSkillSlotDirection[i] = (INT32)_SkillBarSlotRedirect[i];
	//}
	//CMessageHandler::GetInstance().Send_UpdateClientVarRange( ECVT_SkillBarSkillBegin, ECVT_SkillBarSkillEnd, buf, sizeof( buf ) );
}

//==========================================================================================
UBOOL MMOSkillManager::GetSkillSlotActive( INT id )
{
	UBOOL *value = _SkillSlotActive.Find(id);
	if( value == NULL ){
		return FALSE;
	}

	return *value;
}

void MMOSkillManager::SetSkillSlotActive( INT id, UBOOL bActive )
{
	_SkillSlotActive.FindOrAdd(id) = bActive;
}




UBOOL MMOSkillManager::GetSkillSlotFlag1( INT id )
{
	UBOOL *value = _SkillSlotFlag1.Find(id);
	if( value == NULL ){
		return FALSE;
	}
	return *value;
}

void MMOSkillManager::SetSkillSlotFlag1( INT id, UBOOL bActive )
{
	_SkillSlotFlag1.FindOrAdd(id) = bActive;
}


FString MMOSkillManager::GetSkillSlotIcon( INT id )
{
	FString *value = _SkillSlotIcon.Find(id);
	if( value == NULL ){
		return TEXT("");
	}

	return *value;
}

void MMOSkillManager::SetSkillSlotIcon( INT id, const FString& IconName )
{
	_SkillSlotIcon.FindOrAdd(id) = IconName;
}


//==================================================================================================================================
//技能数据
//==================================================================================================================================

TArray<class UMMOAbility*> MMOSkillManager::GetAllAbilities() const
{
	TArray<class UMMOAbility*> lst;
	for (AbilityTemplateMap::const_iterator i = _abilityTemplateMap.begin();
		i != _abilityTemplateMap.end(); ++i)
	{
		lst.AddItem(i->second);
	}

	return lst;
}

//----------------------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::ReflashSkillOutlineData()
{
	_AllMainAbilityIDList.Empty();

	_AllMainAbilityIDList.AddZeroed( EPCT_MAX );
	for(int i = 0;i < _AllMainAbilityIDList.Num() ; ++i )
	{
		//所有技能大类
		_AllMainAbilityIDList(i).AddZeroed( ESUIType_Total );
	}

	TArray<UBOOL> ClassUsble;
	ClassUsble.AddZeroed( EPCT_MAX );

	//
	const AbilityTemplateDataIndexManager::DataTypeMap& spellDataMap = AbilityTemplateDataIndexManager::getSingleton().GetDataMap();
	for( AbilityTemplateDataIndexManager::DataTypeMap::const_iterator it = spellDataMap.begin(); it != spellDataMap.end(); ++it )
	{
		const AbilityTemplateDataIndex* readData = it->second;
		if( readData == NULL ){
			continue;
		}

		//
		INT tESUIT = GetESUITypeByAbility( readData );
		if( (tESUIT >= 0) && (0 == readData->RuneSlotID) )
		{
			//展开适用的职业 一般就对应一个职业
			ConvertSpellType(ClassUsble, readData->Class);

			for( int n = 0; n < ClassUsble.Num(); ++n )
			{
				if( ClassUsble( n ) && tESUIT < _AllMainAbilityIDList( n ).Num() )
				{
					//INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( readData->TemplateID, 1, 0 );
					_AllMainAbilityIDList( n )( tESUIT ).Push( readData->TemplateID );
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
TMap< INT, class UMMOAbility* > MMOSkillManager::GetAllMainAbilitiesSorted( INT classType, INT skillType )
{
	TMap< INT, class UMMOAbility* > allMainSkillMap;
	INT iTotal = _AllMainAbilityIDList( classType )( skillType ).Num();
	for( INT i = 0; i < iTotal; ++i ){
		UINT tempID = _AllMainAbilityIDList( classType )( skillType )( i );

		UMMOAbility* ability = NULL;
		for(int level = MAX_SPELL_LEVEL; level >= 0; --level){
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( tempID, level, 0 );
			ability = FindAbilityTemplateNoWarning( uniqueSkillID );
			if( ability != NULL ){
				break;
			}
		}

		if( ability == NULL ){
			ability = FindAbilityTemplate( MMOSkillManager::CombinateAbilityID( tempID, 0, 0 ) );
		}
		if( ability == NULL ){
			continue;
		}

		if( ability->MainSkill ){
			ability = ability->MainSkill;
		}

		INT abilityID = ability->eventGetAbilityId();
		UMMOAbility **ppToAddAbility = allMainSkillMap.Find( abilityID );

		if( ppToAddAbility == NULL ){
			allMainSkillMap.FindOrAdd( abilityID ) = ability;
		}
	}

	//sort
	allMainSkillMap.ValueSort<COMPARE_POINTER_CLASS(UMMOAbility, __SortInUi)>();

	return allMainSkillMap;
}

//----------------------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::SortAllAbilitiesByRequireLevel( TMap< INT, class UMMOAbility* > &skillMap )
{
	skillMap.ValueSort<COMPARE_POINTER_CLASS(UMMOAbility, __SortInUi)>();
}
//----------------------------------------------------------------------------------------------------------------------------------
TMap< INT, INT > MMOSkillManager::GetAllMainAbilitiesIDs( INT classType, INT skillType )
{
	TMap< INT, INT > allMainSkillMap;
	INT iTotal = _AllMainAbilityIDList( classType )( skillType ).Num();
	for( INT i = 0; i < iTotal; ++i ){
		UINT tempID = _AllMainAbilityIDList( classType )( skillType )( i );
		allMainSkillMap.FindOrAdd( tempID ) = i;
	}
	return allMainSkillMap;
}
//------------------------------------------------
UINT MMOSkillManager::CombinateAbilityID(UINT AbilityID, UINT LevelID,UINT RuneID)
{
	UINT ret = AbilityID;
	ret = ret<<8;
	ret += LevelID;
	ret = ret<<8;
	ret += RuneID;
	return ret;
}

void MMOSkillManager::DecomposeAbilityID(UINT uniqueID, UINT &outSkillID, UINT &outSkillLv,UINT &outSkillRuneID)
{
	outSkillID = uniqueID>>16;
	outSkillLv = ( ( uniqueID & 0x0000ff00 ) >> 8 );
	outSkillRuneID = uniqueID & 0x000000ff;
}

UBOOL MMOSkillManager::IsSameSeriesAbility(UINT uniqueSkillID1, UINT uniqueSkillID2)
{
	return ( ( (uniqueSkillID1 >> 16)  == ( uniqueSkillID2 >> 16 ) ) ? TRUE : FALSE );
}
//------------------------------------------------------------------------
void MMOSkillManager::ClearAbilityTemplates()
{
	AbilityTemplateMap::iterator i = _abilityTemplateMap.begin();
	for (; i != _abilityTemplateMap.end(); ++i)
	{
		i->second->RemoveFromRoot();
	}

	_AllMainAbilityIDList.Empty();
	_abilityTemplateMap.clear();

	_AllMainAbilityIDList.AddZeroed( EPCT_MAX );
	for(int i = 0;i < _AllMainAbilityIDList.Num() ; ++i )
	{
		//所有技能大类
		_AllMainAbilityIDList(i).AddZeroed( ESUIType_Total );
	}
}

//------------------------------------------------------------------------
//void MMOSkillManager::UpdateRuneDur( RUNE_DATA *tData )
//{
//	if( !tData )
//		return;
//
//	//先获得符文对应的主技能 
//	UMMOAbility* tMainActive = MMOFacade::getSingleton().GetMainPlayer()->GetMainAbilityById( tData->wSpellID );
//
//	//符文等级是与主技能相同的
//	//绕 好吧 一切为了流量
//	if( !tMainActive )
//		return;
//
//	/*
//	UINT Key = MMOSkillManager::CombinateAbilityID( tData->wSpellID ,tMainActive->SpellCommonAttribute.Level,tData->bySlot );
//	UMMOAbility* pAbility = FindAbilityTemplate(Key);
//	if( !pAbility )
//		return;
//
//	pAbility->RuneDurability = tData->nDurability;
//	pAbility->RuneDurabilityType = tData->byDurabilityType;
//	*/
//
//	UMMOAbility* tRune=NULL;
//	int byRuneSlot = tData->bySlot;
//
//	if( byRuneSlot <=0 || byRuneSlot > tMainActive->SubRuneList.Num() )
//		return;
//	tRune = tMainActive->SubRuneList( byRuneSlot-1 );
//	if( tRune )
//	{
//		tRune->RuneDurabilityType = tData->byDurabilityType;
//		tRune->RuneDurability = tData->nDurability;
//	}
//}
//------------------------------------------------------------------------
void MMOSkillManager::IniRuneData( )
{
	AbilityTemplateMap::iterator it = _abilityTemplateMap.begin();
	while(it != _abilityTemplateMap.end())
	{
		if( it->second )
		{
			//RuneSlot走的是SpellSlot协议 比Sync走的早一些 同样角色进来 会有问题
			//it->second->SetRuneSlot( 0 );
			it->second->RuneDurability = 0;
			it->second->RuneDurabilityType = 0;
		}
		++it;
	}
	return;
}

//------------------------------------------------------------------------
class UMMOAbility* MMOSkillManager::FindRune(int SpellID, int SpellLevel, int RuneSlot )
{
	UINT Key = MMOSkillManager::CombinateAbilityID( SpellID,SpellLevel,RuneSlot );
	UMMOAbility* pAbility = FindAbilityTemplate(Key);
	if( !pAbility )
		return 0;
	return pAbility;
}

//-----------------------------------------------------------------------------------------------------------------
UMMOAbility* MMOSkillManager::LoadAbility(UINT skillID, UINT skillRuneID, UINT skillLv, const FString& dataPath)
{
	INT uniqueTemplateID = CombinateAbilityID(skillID, skillLv, skillRuneID);
	AbilityTemplateMap::iterator it = _abilityTemplateMap.find(uniqueTemplateID);
	if(it != _abilityTemplateMap.end())
	{
		// 有重复的就不再创建,给出warning
		M_AddDataError(TEXT("spell.txt"), FString::Printf(TEXT("此技能数据已经加载[%s] [ID:%d,LV:1,Rune:%d]"), *dataPath, skillID, skillRuneID));
		return NULL;
	}

	UMMOAbility* abilityTemplete = NULL;
	UMMOAbility* loadedAbilityTemplete = NULL;
	for(int level = 0; level <= MAX_SPELL_LEVEL; ++level)
	{
		UINT Key = MMOSkillManager::CombinateAbilityID( skillID, level, skillRuneID );
		loadedAbilityTemplete = FindAbilityTemplateNoWarning(Key);
		if( loadedAbilityTemplete != NULL )
		{
			break;
		}
	}

	if( loadedAbilityTemplete == NULL )
	{
		abilityTemplete = Cast<UMMOAbility>(UObject::StaticLoadObject(UMMOAbility::StaticClass(), NULL, *dataPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));
	}
	else
	{
		abilityTemplete = Cast<UMMOAbility>(UObject::StaticConstructObject(loadedAbilityTemplete->GetClass(), UObject::GetTransientPackage(), NAME_None, 0, loadedAbilityTemplete));
	}
	
	if(abilityTemplete)
	{
		abilityTemplete->SpellCommonAttribute.Level = skillLv;

		UBOOL isInitSuccess = InitAbility(abilityTemplete);
		if( isInitSuccess == FALSE ){
			//abilityTemplete->MarkPendingKill();
			abilityTemplete = NULL;

			return NULL;
		}

		abilityTemplete->AddToRoot();
		AddAbilityTemplate(uniqueTemplateID, abilityTemplete);

		//玩家可见主动技能非符文
		bool IsMainActive = false;

		//技能归类
		int tESUIT = GetESUITypeByAbility( abilityTemplete );

		if( 0 == abilityTemplete->GetRuneSlot() && ESUIType_Active == tESUIT)
		{
			IsMainActive = true;
		}

		//if( IsMainActive )
		//{
		//	abilityTempleteOfLvOne->SkillGuiID = ++guiID;
		//}

		//记录主技能，方便游戏中使用查询
		TArray<UBOOL> ClassUsble;
		ClassUsble.AddZeroed( EPCT_MAX );

		if( (tESUIT >= 0)
			&& (0 == abilityTemplete->GetRuneSlot())
			&& (skillLv == 1) )
		{
			//展开适用的职业 一般就对应一个职业
			ConvertSpellType(ClassUsble, abilityTemplete->SpellCommonAttribute.Class);

			for( int n = 0; n < ClassUsble.Num(); ++n )
			{
				if( ClassUsble( n ) && tESUIT < _AllMainAbilityIDList( n ).Num() )
				{
					_AllMainAbilityIDList( n )( tESUIT ).Push( skillID );
				}
			}
		}

		return abilityTemplete;
	}
	else
	{
		// TODO:放配置
		M_AddDataError(TEXT("spell.txt"), FString::Printf(TEXT("主技能模板没有加载成功 [%s] [ID:%d,LV:%d,Rune:%d]"), *dataPath, skillID, 0, skillRuneID));
		return NULL;
	}

	return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
UMMOAbility* MMOSkillManager::LoadAbilityFromLevelOne( UINT skillID, UINT skillRuneID, const FString& dataPath )
{
	UMMOAbility* abilityTempleteOfLvOne = LoadAbility( skillID, skillRuneID, 0, dataPath );
	if(abilityTempleteOfLvOne)
	{
		//changed by JimZhang@snda.com 在模板中将等级展开
		if(abilityTempleteOfLvOne->bCanExpandAbilityByLevel)
		{
			//UBOOL isAdded = FALSE;
			//第一级的技能已经加过，这里从第二级开始
			for(int level = abilityTempleteOfLvOne->SpellCommonAttribute.Level + 1; level <= MAX_SPELL_LEVEL; level++)
			{
				UINT Key = MMOSkillManager::CombinateAbilityID(abilityTempleteOfLvOne->SpellCommonAttribute.Id, level, abilityTempleteOfLvOne->GetRuneSlot() );

				if( FindAbilityTemplateNoWarning(Key) )
				{
					// 有重复的就不再创建
					continue;
				}

				UMMOAbility* subAbility = Cast<UMMOAbility>(UObject::StaticConstructObject(abilityTempleteOfLvOne->GetClass(),
					UObject::GetTransientPackage(), NAME_None, 0, abilityTempleteOfLvOne));

				if(subAbility)
				{
					//将展开的技能等级写入
					subAbility->SpellCommonAttribute.Level = level;
					subAbility->AddToRoot();

					//if( IsMainActive )
					//{
					//	//等级上不展开
					//	subAbility->SkillGuiID = guiID;
					//}

					InitAbility(subAbility);
					AddAbilityTemplate(Key, subAbility);
				}
			}
		}

		return abilityTempleteOfLvOne;
	}
	
	return NULL;
}


UBOOL MMOSkillManager::InitAbility( UMMOAbility* ability )
{
	if(!ability)
	{
		return TRUE;
	}


	INT ID = ability->GetTemplateID();

	INT key = CombinateAbilityID(ID, ability->SpellCommonAttribute.Level, ability->GetRuneSlot());
	
	// if need level up, fill data
	if(ability->bHasSpellProgressData)
	{
		const SpellProgessData* progDat = SpellProgessDataManager::getSingleton().FindData(key);
		if(progDat)
		{
			ability->LvUpRequirePoints = progDat->ConsumePts;
			ability->LvUpRequireRoleLevel = progDat->RequireLevel;
			ability->Pow = progDat->Pow;

			//RuneSkillLevelUpConsum &consumData = _RuneSkillLevelUpMap.FindOrAdd( key );
			//consumData._ItemID = progDat->RequireItem;
			//consumData._ItemCnt = progDat->RequireItemCount;
		}
		else
		{
			// TODO:放配置
			warnf(NAME_Warning, TEXT("技能升级数据没有找到, %s ID:%d Lv:%d Rune:%d"), *ability->GetFullName(),
				ID, ability->SpellCommonAttribute.Level, ability->GetRuneSlot());
			return FALSE;
		}
	}

	const SpellVarData* spellDat = SpellVarManager::getSingleton().FindData(key);
	if(spellDat)
	{
		SetSpellVar(ability, *spellDat->Variable1, *spellDat->Value1);
		SetSpellVar(ability, *spellDat->Variable2, *spellDat->Value2);
		SetSpellVar(ability, *spellDat->Variable3, *spellDat->Value3);
		SetSpellVar(ability, *spellDat->Variable4, *spellDat->Value4);

		ability->SkillDetailDescribe = spellDat->DetailDescription;
		ability->ParseAbilityDetailDescribe();
		ability->SkillMainSpeciality = spellDat->MainSpeciality;
		ability->SkillSecondSpeciality = spellDat->SecondSpeciality;
		ability->SkillThirdSpeciality = spellDat->ThirdSpeciality;

		if( !spellDat->SpellName.IsEmpty() )
		{
			//策划要求 #10371，用spell_var的名字代替upk配置名
			ability->ClientCommonAttribute.AbilityName = spellDat->SpellName;
		}
		

		ability->DescriptiveWords = spellDat->DescriptiveWords;
		ability->UseMethod = spellDat->UseMethod;
		ability->AttackRange = spellDat->AttackRange;
		ability->EffectRange = spellDat->EffectRange;
		ability->EffectRangeEnum = spellDat->EffectRangeEnum;
	}
	else
	{
		// TODO:放配置
		warnf(NAME_Log, TEXT("技能变量没有找到, %s ID:%d Lv:%d Rune:%d"), *ability->GetFullName(),
			ID, ability->SpellCommonAttribute.Level, ability->GetRuneSlot());
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------
class UMMOAbility* MMOSkillManager::FindAbilityTemplateNoWarning(UINT templateID)
{
	AbilityTemplateMap::iterator it = _abilityTemplateMap.find(templateID);
	if(it != _abilityTemplateMap.end())
	{
		return it->second;
	}

	return NULL;
}
//------------------------------------------------------------------------
class UMMOAbility* MMOSkillManager::FindAbilityTemplate(UINT templateID)
{
	AbilityTemplateMap::iterator it = _abilityTemplateMap.find(templateID);
	if(it != _abilityTemplateMap.end())
	{
		return it->second;
	}

	UINT skillID = 0;
	UINT skillLv = 0;
	UINT skillRuneID = 0;
	MMOSkillManager::DecomposeAbilityID( templateID, skillID, skillLv, skillRuneID );
	if( skillLv > MAX_SPELL_LEVEL ){
		return NULL;
	}

	INT baseAbilityID = CombinateAbilityID(skillID, 0, skillRuneID);
	//// 先找spell.txt表项技能，如果没加载就把此表项技能先加载全（所有等级）
	//it = _abilityTemplateMap.find(baseAbilityID);

	it = _abilityTemplateMap.find(templateID);

	FString dataPath;
	if(it == _abilityTemplateMap.end() )
	{
		const AbilityTemplateDataIndex* dat = AbilityTemplateDataIndexManager::getSingleton().FindData(baseAbilityID);//只有1级数据
		if(!dat)
		{
			M_AddDataError(TEXT("spell.txt"), FString::Printf(TEXT("[MMOSkillManager::FindAbilityTemplate] 主技能模板索引数据没从spell.txt中找到[ID:%d,LV:%d,Rune:%d]"), skillID, skillLv, skillRuneID));

			return NULL;
		}
		else
		{
			dataPath = dat->ObjectPath;
			//LoadAbilityFromLevelOne(dat->TemplateID, dat->RuneSlotID, dataPath);
			LoadAbility(dat->TemplateID, dat->RuneSlotID, skillLv, dataPath);
		}

		// 加载完了再查一次，没有的话就说明真没有了
		it = _abilityTemplateMap.find(templateID);
	}

	if(it != _abilityTemplateMap.end())
	{
		return it->second;
	}

	M_AddDataError(TEXT("spell.txt"), FString::Printf(TEXT("技能模板没找到，有可能是没有展开[%s] [ID:%d,LV:%d,Rune:%d]"), *dataPath, skillID, skillLv, skillRuneID));
	return NULL;

#if 0
	FString dataPath = MMOFacade::getSingleton()._FindAbilityTemplateObjPath(templateID);
	if(!dataPath.IsEmpty())
	{
		UMMOAbility* datTmp = Cast<UMMOAbility>(UObject::StaticLoadObject(UMMOAbility::StaticClass(), NULL, *dataPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));
		if(datTmp)
		{
			datTmp->AddToRoot();
			AddAbilityTemplate(templateID, datTmp);
			return datTmp;
		}
		else
		{
			warnf(NAME_Error, TEXT("[MMOSkillManager::FindAbilityTemplate] Ability template[ID:%d,LV:%d,Rune:%d][%s] faild to load!"), skillID, skillLv, skillRuneID, *dataPath);
			return NULL;
		}
	}

#endif
}

//----------------------------------------------------------------------------------------------------------------------------------
UMMOAbility *MMOSkillManager::GetMainAbilityFor( UMMOAbility *ability )
{
	UMMOAbility* mainAbility = NULL;

	if( ability != NULL )
	{
		UINT uID		= ability->eventGetAbilityId();	
		UINT runeSlotID	= 0;
		UINT level		= ability->SpellCommonAttribute.Level;
		UINT Key		= MMOSkillManager::CombinateAbilityID(uID,level,runeSlotID);
		mainAbility     = FindAbilityTemplate(Key);
	}

	return mainAbility;
}

//----------------------------------------------------------------------------------------------------------------------------------
UMMOAbility *MMOSkillManager::GetAbilityByNextLevel( UMMOAbility *ability )
{
	UMMOAbility* nextAbility = NULL;

	if( ability != NULL )
	{
		UINT uID		= ability->eventGetAbilityId();	
		UINT runeSlotID	= 0;
		UINT level		= ability->SpellCommonAttribute.Level + 1 ;
		UINT Key		= MMOSkillManager::CombinateAbilityID(uID,level,runeSlotID);
		nextAbility     = FindAbilityTemplate(Key);
	}

	return nextAbility;
}

//----------------------------------------------------------------------------------------------------------------------------------
UMMOAbility *MMOSkillManager::GetAbilityWithRuneByNextLevel( UMMOAbility *ability, UINT maxLevel/* = MAX_SKILL_NORMAL_LEVEL*/ )
{
	UMMOAbility* nextAbility = NULL;

	if( ability != NULL )
	{
		UINT level		= ability->SpellCommonAttribute.Level + 1 ;
		if( level > maxLevel ){
			return NULL;
		}

		UINT uID		= ability->eventGetAbilityId();	
		UINT runeSlotID	= ability->GetRuneSlot();
		UINT Key		= MMOSkillManager::CombinateAbilityID(uID,level,runeSlotID);
		nextAbility     = FindAbilityTemplate(Key);
	}

	return nextAbility;
}

void MMOSkillManager::ConvertSpellType( TArray<UBOOL>& ClassUsble, const FSpell_ClassType& classType )
{
#if !SHIPPING_PC_GAME
	check(ClassUsble.Num() == EPCT_MAX);
#endif

	if(ClassUsble.Num() != EPCT_MAX)
	{
		ClassUsble.AddZeroed((INT)(EPCT_MAX) - ClassUsble.Num());
	}

	ClassUsble(0) = classType.bClassType1;
	ClassUsble(1) = classType.bClassType2;
	ClassUsble(2) = classType.bClassType3;
	ClassUsble(3) = classType.bClassType4;
	ClassUsble(4) = classType.bClassType5;	// Taoist (spell mask bit4)
	// 1.05: Lancer (mask bit6 = 64) and Sorcerer (mask bit7 = 128). Class ids G=5, H=6.
	ClassUsble(EPCT_PlayerClassG) = classType.bClassType7;
	ClassUsble(EPCT_PlayerClassH) = classType.bClassType8;
	// mask bit5 (32) is the monster/npc bit -> npc class F (=7), never index 5
	ClassUsble(EPCT_PlayerClassF) = classType.bClassType6;
}
//
UBOOL MMOSkillManager::IsSkillUsableForPlayer( UMMOAbility *pInAbility, INT playerClassType )
{
	if( pInAbility == NULL ){
		return FALSE;
	}

	TArray<UBOOL> classUsble;
	classUsble.AddZeroed( EPCT_MAX );
	ConvertSpellType( classUsble, pInAbility->SpellCommonAttribute.Class );

	if( !classUsble.IsValidIndex( playerClassType ) ){
		return FALSE;
	}

	return classUsble( playerClassType );
}

//----------------------------------------------------------------------------------------------------------------------------------
//读取技能
void MMOSkillManager::LoadAbilityTemplates(EPlayerClass playerClass)
{
	SCOPE_LOG_TIME(___INIT__, TEXT("LoadAbilityTemplates"), 0);

	MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)

	M_SCOPE_MEM_PROF(__MEMPROF__, TEXT("Facade::LoadAbilityTemplates"));

#if 0 // 延期加载
	AbilityTemplateDataIndexManager::DataTypeMap::const_iterator it = AbilityTemplateDataIndexManager::getSingleton().GetDataMap().begin();
	for (; it != AbilityTemplateDataIndexManager::getSingleton(). GetDataMap().end(); ++it)
	{
		const AbilityTemplateDataIndex* datIdx = it->second;
		if(datIdx && MMO::CanBeUsedByPlayerClass(playerClass, datIdx->Class))
		{
			if((datIdx->CastType == E_spct_Active)
				|| datIdx->CastType == E_spct_SubSpell)
			{
				const FString& dataPath = it->second->ObjectPath;
				LoadAbilityFromLevelOne(it->second->TemplateID, it->second->RuneSlotID, dataPath);
			}
		}
	}
#endif

#if 0

    //所有玩家可见主动技能（非符文）的唯一标识从0开始
    int  guiID = -1;

	//所有职业
	_AllMainAbilityIDList.AddZeroed( EPCT_MAX );
	
	TArray<UBOOL> ClassUsble;
	ClassUsble.AddZeroed( EPCT_MAX );

	for(int i = 0;i < _AllMainAbilityIDList.Num() ; ++i )
	{
		//所有技能大类
		_AllMainAbilityIDList(i).AddZeroed( ESUIType_Total );
	}

	TArray<FString> DirArray;
	GConfig->GetArray( TEXT("AbilityTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );
	MMO::FormatTempleteDataLoadingDir(DirArray);

	for (INT k = 0; k < DirArray.Num(); ++k)
	{
		const FString& cfgDir = DirArray(k);

		debugIDf(TEXT("TemplateData"), TEXT("===== Loading Ability template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if( !Package )
			{
				M_AddDataError(TEXT("spell.txt"),
					FString::Printf(TEXT("无法加载任务数据包: %s"), *pkgs(i) );
				continue;;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}
			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UMMOAbility* pAbilityTemplete = Cast<UMMOAbility>(Package->GetNetObjectAtIndex(j));
				if(pAbilityTemplete)
				{
                    //玩家可见主动技能非符文
                    bool IsMainActive = false;
					//技能归类
					int tESUIT = GetESUITypeByAbility( pAbilityTemplete );

					if( 0 == pAbilityTemplete->GetRuneSlot() && ESUIType_Active == tESUIT){
						IsMainActive = true;
					}
                    if( IsMainActive ){
                        pAbilityTemplete->SkillGuiID = ++guiID;
					}

					//changed by JimZhang@snda.com 在模板中将等级展开
					UBOOL isAdded = FALSE;
					for(int level=pAbilityTemplete->SpellCommonAttribute.Level;level<=MAX_SPELL_LEVEL;level++)
					{
						UMMOAbility* pAbility = Cast<UMMOAbility>(UObject::StaticConstructObject(pAbilityTemplete->GetClass(), UObject::GetTransientPackage(), NAME_None, 0, pAbilityTemplete));
						if(pAbility)
						{
							/*if( pAbility->GetRuneSlot() != 0 )
							{
								int UUU=0;
							}*/
							UINT Key = MMOSkillManager::CombinateAbilityID(pAbility->SpellCommonAttribute.Id, level, pAbility->GetRuneSlot() );
							//将展开的技能等级写入
							pAbility->SpellCommonAttribute.Level = level;
							pAbility->AddToRoot();
                            if( IsMainActive )
                            {
                                //等级上不展开
                                pAbility->SkillGuiID = guiID;
                            }

							AddAbilityTemplate(Key, pAbility);

							//if( AddAbilityTemplate(Key, pAbility) == FALSE )
							//{
							//	//pAbility->RemoveFromRoot();
							//}

							//所有技能索引只记录一级
							if( (isAdded == FALSE) && (tESUIT >= 0) )
							{
								//展开适用的职业 一般就对应一个职业
								ConvertSpellType(ClassUsble, pAbility->SpellCommonAttribute.Class);

								for( int n = 0;n < ClassUsble.Num(); ++ n )
								{
									if( ClassUsble( n ) && tESUIT < _AllMainAbilityIDList( n ).Num() )
									{
										_AllMainAbilityIDList( n )( tESUIT ).AddItem( pAbility->SpellCommonAttribute.Id );
									}
								}
								isAdded = TRUE;
							}
						}
					}
				}
			}
		}
	}

	//ReadSpellVar();
	//ReadRuneVar();
	//AutoLinkAbilityRune();
	//读取技能升级表 需要符文完成链接之后进行
	//ReadSpellProgress();

#endif

	MALLOC_PROFILER_GAME(GEngine->ExecMemFunctions(TEXT("MEM"));)
}

//----------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOSkillManager::SetSpellVar(UMMOAbility* pAbility,const TCHAR* pName,const TCHAR* pValue)
{
	if(!(pAbility&&pName&&pValue))return FALSE;

	FString strName = pName;
	if( !strName.StartsWith(TEXT("sat")) ){
		//不是服务器枚举，客户端也没这属性
		return  TRUE;
	}
	//获得客户端真实的属性名
	strName = strName.Right( strName.Len() - 3 );

	//INT		iValue  = appAtoi(pValue);

	UProperty* prop = pAbility->GetClass()->PropertyLink;
	UPropertyValue v;
	//v.IntValue = iValue;
	while(prop)
	{
		if(prop->GetName() == _T("SpellCommonAttribute"))
		{
			UStructProperty* a = (UStructProperty*)prop;
			UProperty* b = a->Struct->PropertyLink;
			while(b)
			{
				//pAbility->GlobalSetProperty("", pAbility->GetClass(), a,)
				if(b->GetName() == strName)
				{
					if (b->IsA(UIntProperty::StaticClass()))
					{
						v.IntValue = appAtoi(pValue);
					}
					else if (b->IsA(UFloatProperty::StaticClass()))
					{
						v.FloatValue = appAtof(pValue);
					}
					else if (b->IsA(UByteProperty::StaticClass()))
					{
						v.ByteValue = appAtoi(pValue);
					}
					else if (b->IsA(UBoolProperty::StaticClass()))
					{
						v.BoolValue = appAtoi(pValue);
					}
					else
					{
						check(FALSE);
					}


					b->SetPropertyValue((BYTE*)&pAbility->SpellCommonAttribute + b->Offset, v);
					break;
				}
				b = b->PropertyLinkNext;
			}			
			break;
		}
		prop = prop->PropertyLinkNext;
	}

	//if(		strName == _T("TargetLimitBits"		))pAbility->SpellCommonAttribute.TargetLimitBits = iValue;
	//else if(strName == _T("CasterLimitBits"		))pAbility->SpellCommonAttribute.CasterLimitBits = iValue;
	//else if(strName == _T("EffectValue"			))pAbility->SpellCommonAttribute.EffectValue = iValue;
	//else if(strName == _T("EffectPct"			))pAbility->SpellCommonAttribute.EffectPct = iValue;
	//else if(strName == _T("Speed"				))pAbility->SpellCommonAttribute.Speed = iValue;
	//else if(strName == _T("HitTag"				))pAbility->SpellCommonAttribute.HitTag = iValue;
	////else if(strName == _T("ControlTag"			))pAbility->SpellCommonAttribute.ControlTag = iValue;
	//else if(strName == _T("CancelLevel"		))pAbility->SpellCommonAttribute.CancelLevel = iValue;
	//else if(strName == _T("CancelType"			))pAbility->SpellCommonAttribute.CancelType = iValue;
	//else if(strName == _T("HitBreak"			))pAbility->SpellCommonAttribute.HitBreak = iValue;
	//else if(strName == _T("MoveBreak"			))pAbility->SpellCommonAttribute.MoveBreak = iValue;

	//else if(strName == _T("HitLevel"			))pAbility->SpellCommonAttribute.HitLevel = iValue;
	//else if(strName == _T("Power"				))pAbility->SpellCommonAttribute.Power = iValue;
	//else if(strName == _T("CastTimeLoss"		))pAbility->SpellCommonAttribute.CastTimeLoss = iValue;
	//else if(strName == _T("HardTime"			))pAbility->SpellCommonAttribute.HardTime = iValue;
	//else if(strName == _T("Interval"			))pAbility->SpellCommonAttribute.interval = iValue;
	//else if(strName == _T("PrepareTimeMax"		))pAbility->SpellCommonAttribute.PrepareTimeMax = iValue;
	//else if(strName == _T("PrepareTimeMin"		))pAbility->SpellCommonAttribute.PrepareTimeMin = iValue;
	//else if(strName == _T("AdjustFaceDirection"))pAbility->SpellCommonAttribute.AdjustFaceDirection = iValue;

	//else if(strName == _T("PrepareType"		))pAbility->SpellCommonAttribute.PrepareType = iValue;
	////else if(strName == _T("GroupCoolDown"		))pAbility->SpellCommonAttribute.GroupCoolDown = iValue;
	//else if(strName == _T("SelfCoolDown"		))pAbility->SpellCommonAttribute.SelfCoolDown = iValue;
	////else if(strName == _T("CoolDownTriggerType"))pAbility->SpellCommonAttribute.CoolDownTriggerType = iValue;
	//else if(strName == _T("DurativeCostValuePct"))pAbility->SpellCommonAttribute.DurativeCostValuePct = iValue;
	//else if(strName == _T("DurativeCostValue"	))pAbility->SpellCommonAttribute.DurativeCostValue = iValue;

	//else if(strName == _T("InitialCostValuePct"))pAbility->SpellCommonAttribute.InitialCostValuePct = iValue;
	//else if(strName == _T("InitialCostValue"	))pAbility->SpellCommonAttribute.InitialCostValue = iValue;
	//else if(strName == _T("CostType"			))pAbility->SpellCommonAttribute.CostType = iValue;
	////else if(strName == _T("SelfStance"			))pAbility->SpellCommonAttribute.SelfStance = iValue;
	////else if(strName == _T("SelfPos"			))pAbility->SpellCommonAttribute.SelfPos = iValue;

	////else if(strName == _T("TargetStance"		))pAbility->SpellCommonAttribute.TargetStance = iValue;
	////else if(strName == _T("TargetPos"			))pAbility->SpellCommonAttribute.TargetPos = iValue;
	////else if(strName == _T("TargetDirection"	))pAbility->SpellCommonAttribute.TargetDirection = iValue;
	////else if(strName == _T("TargetSex"			))pAbility->SpellCommonAttribute.TargetSex = iValue;
	////else if(strName == _T("TargetRace"			))pAbility->SpellCommonAttribute.TargetRace = iValue;

	////else if(strName == _T("TargetRelationship"	))pAbility->SpellCommonAttribute.TargetRelationship = iValue;
	////else if(strName == _T("TargetObjectType"	))pAbility->SpellCommonAttribute.TargetObjectType = iValue;
	//else if(strName == _T("OffsetDistance"		))pAbility->SpellCommonAttribute.OffsetDistance = iValue;
	//else if(strName == _T("OffsetDirection"	))pAbility->SpellCommonAttribute.OffsetDirection = iValue;
	//else if(strName == _T("MaxTargetCount"		))pAbility->SpellCommonAttribute.MaxTargetCount = iValue;

	//else if(strName == _T("UseWeaponRange"		))pAbility->SpellCommonAttribute.UseWeaponRange = iValue;
	//else if(strName == _T("RangeParam3"		))pAbility->SpellCommonAttribute.RangeParam3 = iValue;
	//else if(strName == _T("RangeParam2"		))pAbility->SpellCommonAttribute.RangeParam2 = iValue;
	//else if(strName == _T("RangeParam1"		))pAbility->SpellCommonAttribute.RangeParam1 = iValue;

	//else if(strName == _T("RangeType"			))pAbility->SpellCommonAttribute.RangeType = iValue;
	//else if(strName == _T("TolerantDistance"	))pAbility->SpellCommonAttribute.TolerantDistance = iValue;
	//else if(strName == _T("MaxDistance"		))pAbility->SpellCommonAttribute.MaxDistance = iValue;
	//else if(strName == _T("MinDistance"		))pAbility->SpellCommonAttribute.MinDistance = iValue;

	//else if(strName == _T("LockType"			))pAbility->SpellCommonAttribute.LockType = iValue;
	////else if(strName == _T("DamageType"			))pAbility->SpellCommonAttribute.DamageType = iValue;
	//else if(strName == _T("AttackType"			))pAbility->SpellCommonAttribute.AttackType = iValue;

	//else if(strName == _T("JudgeType"			))pAbility->SpellCommonAttribute.JudgeType = iValue;
	//else if(strName == _T("CastType"			))pAbility->SpellCommonAttribute.CastType = iValue;
	//else if(strName == _T("WeaponTypeL"		))pAbility->SpellCommonAttribute.WeaponTypeL = iValue;

	//else if(strName == _T("WeaponTypeR"		))pAbility->SpellCommonAttribute.WeaponTypeR = iValue;
	//else if(strName == _T("CacheTime"			))pAbility->SpellCommonAttribute.CacheTime = iValue;
	//else if(strName == _T("ResetTime"			))pAbility->SpellCommonAttribute.ResetTime = iValue;
	//else if(strName == _T("NextSpell"			))pAbility->SpellCommonAttribute.NextSpell = iValue;
	//else if(strName == _T("Xl"					))pAbility->SpellCommonAttribute.NextSpell = iValue;
	//else if(strName == _T("X2"					))pAbility->SpellCommonAttribute.NextSpell = iValue;
	//else if(strName == _T("X3"					))pAbility->SpellCommonAttribute.NextSpell = iValue;
	//else if(strName == _T("X4"					))pAbility->SpellCommonAttribute.NextSpell = iValue;
	//else{}


	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOSkillManager::AddAbilityTemplate( UINT k, UMMOAbility* ability)
{
	AbilityTemplateMap::const_iterator i = _abilityTemplateMap.find(k);
	UINT skillID;
	UINT skillLv;
	UINT skillRuneID;
	MMOSkillManager::DecomposeAbilityID( k, skillID, skillLv, skillRuneID );
	if(i != _abilityTemplateMap.end())
	{
		M_AddDataError(TEXT("spell.txt"), FString::Printf(TEXT("重复的技能模板:ID:%d,Lv:%d,Rune:%d[%s]->[%s]"), skillID, skillLv, skillRuneID,
			*ability->ClientCommonAttribute.AbilityName, *i->second->ClientCommonAttribute.AbilityName));

		//断开主技能与符文技能的连接
		//(可能被连接过了，为了使用安全丢弃这个数据)
		if( skillRuneID > 0 )
		{
			//是符文技能
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv, 0 );
			UMMOAbility *mainAbility = FindAbilityTemplateNoWarning( uniqueSkillID );
			if( mainAbility != NULL )
			{
				mainAbility->AddSubRune( skillRuneID, NULL );
			}
		}
		else{
			//是主技能
			INT runeSkillCnt = ability->SubRuneList.Num();
			for( INT i = 1; i <= runeSkillCnt; ++i )
			{
				UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv, i );
				UMMOAbility *runeAbility = FindAbilityTemplateNoWarning( uniqueSkillID );
				if( runeAbility != NULL )
				{
					runeAbility->MainSkill = NULL;
					ability->AddSubRune( i, NULL );
				}
			}
		}

		ability->RemoveFromRoot();

		return FALSE;
	}
	else
	{
		ability->ParseGroupIds();
		_abilityTemplateMap.insert(std::make_pair(k, ability));

		//连接主技能与符文技能
		if( skillRuneID > 0 )
		{
			//是符文技能
			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv, 0 );
			UMMOAbility *mainAbility = FindAbilityTemplate( uniqueSkillID );
			if( mainAbility != NULL )
			{
				mainAbility->AddSubRune( skillRuneID, ability );
			}

			ability->MainSkill = mainAbility;
		}
		//else{
		//	//是主技能
		//	//bClassType5为True是NPC技能
		//	if( !ability->SpellCommonAttribute.Class.bClassType5 )
		//	{
		//		//只有角色技能有子技能
		//		for( INT i = 1; i <= UCONST_MaxSubRuneNum; ++i )
		//		{
		//			UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillID, skillLv, i );
		//			UMMOAbility *runeAbility = FindAbilityTemplate( uniqueSkillID );
		//			if( runeAbility != NULL )
		//			{
		//				runeAbility->MainSkill = ability;
		//				ability->AddSubRune( i, runeAbility );
		//			}
		//		}
		//	}
		//}
		return TRUE;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
void MMOSkillManager::AutoLinkAbilityRune()
{
	UMMOAbility* tAbility;
	UMMOAbility* tMainAbility;
	AbilityTemplateMap::iterator it = _abilityTemplateMap.begin();

	//int iSize = _abilityTemplateMap.size();
	//int iAbilitySize = sizeof( UMMOAbility );
	//int iTotalSize = iSize * iAbilitySize;

	while(it != _abilityTemplateMap.end())
	{
		tAbility = it->second;
		//如果是一个符文 
		if( tAbility!=NULL && tAbility->GetRuneSlot() >0 )
		{
			//找到其对应的主技能
			UINT Key = MMOSkillManager::CombinateAbilityID( tAbility->SpellCommonAttribute.Id, tAbility->SpellCommonAttribute.Level,0 );
			tMainAbility = FindAbilityTemplate( Key);
			if( tMainAbility )
			{
				//添加到主技能的符文列表
				tMainAbility->AddSubRune( tAbility->GetRuneSlot(),tAbility );
			}
		}
		++it;
	}

	return;
}

//======================================================================================================================================================
//
//======================================================================================================================================================
//UI表现上对技能的归类
INT MMOSkillManager::GetESUITypeByAbility( UMMOAbility *pAbility )
{
		/*
		enum ESPELL_UI_TYPE
		{
		ESUIType_Active                    <DisplayName="ESUIType_None">,
		ESUIType_Passive                   <DisplayName="ESUIType_Passive">,
		ESUIType_Counter                   <DisplayName="ESUIType_Counter">,
		ESUIType_Chase                     <DisplayName="ESUIType_Chase">,
		ESUIType_Terminate                 <DisplayName="ESUIType_Terminate">,
		ESUIType_Total                     <DisplayName="ESUIType_Total">,
		};
		*/
		/*
		enum ESPELL_QTE_TYPE
		{
		E_spqte_None                      <DisplayName="E_spqte_None">,
		E_spqte_Counter                   <DisplayName="E_spqte_Counter">,
		E_spqte_Chase                     <DisplayName="E_spqte_Chase">,
		E_spqte_Terminate                 <DisplayName="E_spqte_Terminate">,
		E_spqte_Total                     <DisplayName="E_spqte_Total">,
		E_spqte_MAX
		};
		*/
		/*
		[ESpell_CastType]
		SCa_Type1=被动技能
		SCa_Type2=主动技能
		SCa_Type3=子技能
		SCa_Type4=可直接施放的技能
		*/
	if( ! pAbility )
		return -1;
	//隐藏的技能
	if( ! pAbility->ClientCommonAttribute.ShowOnUI )
		return -1;
	//被动技能
	if( pAbility->SpellCommonAttribute.CastType == E_spct_Passive )
		return ESUIType_Passive;
	//普通主动技能
	if( pAbility->SpellCommonAttribute.QTEType == E_spqte_None  && pAbility->SpellCommonAttribute.CastType == E_spct_Active  ) 
		return ESUIType_Active;
	//返回对应的QTE类型
	if( pAbility->SpellCommonAttribute.QTEType > 0  && pAbility->SpellCommonAttribute.CastType == E_spct_Active  )
		return ESUIType_Passive + pAbility->SpellCommonAttribute.QTEType;
	//子技能及其它
	else
		return -1;
}

//
INT MMOSkillManager::GetESUITypeByAbility( const struct AbilityTemplateDataIndex *pData )
{
	if( ! pData )
		return -1;
	//隐藏的技能
	if( ! pData->ShowOnUI )
		return -1;
	//被动技能
	if( pData->CastType == E_spct_Passive )
		return ESUIType_Passive;
	//普通主动技能
	if( pData->QTEType == E_spqte_None  && pData->CastType == E_spct_Active  ) 
		return ESUIType_Active;
	//返回对应的QTE类型
	if( pData->QTEType > 0  && pData->CastType == E_spct_Active  )
		return ESUIType_Passive + pData->QTEType;
	//子技能及其它
	else
		return -1;
}
//-----------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOSkillManager::IsSkillBindIn( const FSpell_BindSlot &skillBindData, INT hotKeyEnum )
{
	switch( hotKeyEnum )
	{
	//case ECK_MouseL: return skillBindData.bMouseL;
	//case ECK_MouseR: return skillBindData.bMouseR;
	case ECK_Slot1: return skillBindData.bKey0;
	case ECK_Slot2: return skillBindData.bKey1;
	case ECK_Slot3: return skillBindData.bKey2;
	case ECK_Slot4: return skillBindData.bKey3;
	case ECK_Slot5: return skillBindData.bKey4;
	case ECK_Slot6: return skillBindData.bKey5;
	case ECK_Slot7: return skillBindData.bKey6;
	case ECK_Slot8: return skillBindData.bKey7;
	case ECK_Slot9: return skillBindData.bKey8;
	case ECK_Slot10: return skillBindData.bKey9;
	case ECK_Slot11: return skillBindData.bKey10;
	case ECK_Slot12: return skillBindData.bKey11;
	}

	return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------------------
INT MMOSkillManager::GetSkillBindDefaultSlot( const FSpell_BindSlot &skillBindData )
{
	INT hotKeyValue = GetSkillBindHotkey( skillBindData );
	if( hotKeyValue < 0 ){
		return -1;
	}
	//
	switch( hotKeyValue )
	{
	//case ECK_MouseL: return 0;
	//case ECK_MouseR: return 1;
	case ECK_Slot1: return 0;
	case ECK_Slot2: return 1;
	case ECK_Slot3: return 2;
	case ECK_Slot4: return 3;
	case ECK_Slot5: return 4;
	case ECK_Slot6: return 5;
	case ECK_Slot7: return 6;
	case ECK_Slot8: return 7;
	case ECK_Slot9: return 8;
	case ECK_Slot10: return 9;
	case ECK_Slot11: return 10;
	case ECK_Slot12: return 11;
	}

	return -1;
}

//
INT MMOSkillManager::GetSkillBindHotkey( const FSpell_BindSlot &skillBindData )
{
	if( skillBindData.bKey0 ){
		return ECK_Slot1;
	}
	//
	if( skillBindData.bKey1 ){
		return ECK_Slot2;
	}
	
	if( skillBindData.bKey2 ){
		return ECK_Slot3;
	}
	//
	if( skillBindData.bKey3 ){
		return ECK_Slot4;
	}
	//
	if( skillBindData.bKey4 ){
		return ECK_Slot5;
	}
	//
	if( skillBindData.bKey5 ){
		return ECK_Slot6;
	}
	//
	if( skillBindData.bKey6 ){
		return ECK_Slot7;
	}
	//
	if( skillBindData.bKey7 ){
		return ECK_Slot8;
	}
	//
	if( skillBindData.bKey8 ){
		return ECK_Slot9;
	}
	//
	if( skillBindData.bKey9 ){
		return ECK_Slot10;
	}
	//
	if( skillBindData.bKey10 ){
		return ECK_Slot11;
	}
	//
	if( skillBindData.bKey11 ){
		return ECK_Slot12;
	}
	//
	return -1;
}

//======================================================================================================================================================
//交互技能
UBOOL MMOSkillManager::LoadSpecialSkills()
{
	//CTabFile* tabFile = NULL;
	//if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("specialSkill.txt"), TRUE ) ){
	//	return FALSE;
	//}

	////剔除标题行
	//for( INT i = 0+1; i< tabFile->GetRowCount(); i++ ){
	//	//TArray<FString>& ArrayString = ArrayString2D(i);
	//	UINT len = tabFile->GetColCount();
	//	if( len < 3 ){
	//		M_AddDataError(TEXT("spell.txt"), FString::Printf(
	//			TEXT("列数错误，第 %d 行"), i ));
	//		continue;
	//	}

	//	UINT uSkillID = appAtoi(tabFile->GetData( 0,i));	
	//	INT skillType = appAtoi(tabFile->GetData( 1,i));

	//	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( uSkillID, 1, 0 );
	//	SpecialSkill &skillData = _SpecialSkillsMap.FindOrAdd( uniqueSkillID );
	//	skillData._SkillID = uSkillID;
	//	skillData._SpecialSkillType = (SpecailSkillType)skillType;
	//	skillData._GetRequire = tabFile->GetData(2,i);
	//}
	//delete tabFile;

	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------------------------------
const SpecialSkill *MMOSkillManager::FindSpecialSkill( UINT uniqueSkillID )
{
	return _SpecialSkillsMap.Find( uniqueSkillID );
}

//-----------------------------------------------------------------------------------------------------------------------------------
TArray< SpecialSkill > MMOSkillManager::GetSpecialSkillOfType( SpecailSkillType specialType )
{
	TArray< SpecialSkill> lst;
	for( TMap< INT, SpecialSkill >::TConstIterator it(_SpecialSkillsMap); it; ++it ){
		const SpecialSkill *pSpecialSkill = &it.Value();
		if( pSpecialSkill != NULL && pSpecialSkill->_SpecialSkillType == specialType ){
			lst.Push( *pSpecialSkill );
		}
	}

	return lst;
}

//======================================================================================================================================================
//是否记录变身中的技能变化
UBOOL MMOSkillManager::IsSkillChangeNeedCache( INT hotKey )
{
	UMMOAbility **ppCachedSkill = _InTransformSkillCache.Find( hotKey );
	if( ppCachedSkill != NULL ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------
UMMOAbility *MMOSkillManager::GetInTransformCachedSkill( INT hotKey )
{
	UMMOAbility **ppCachedSkill = _InTransformSkillCache.Find( hotKey );
	if( ppCachedSkill != NULL ){
		return *ppCachedSkill;
	}

	return NULL;
}

UMMOAbility *MMOSkillManager::GetInTransformCachedSkillByID( INT skillTemplateID )
{
	for( TMap< INT, UMMOAbility* >::TIterator itor( _InTransformSkillCache ); itor; ++itor ){
		//ECommandKey hotKey = (ECommandKey)itor.Key();
		UMMOAbility *cachedEquipeddSkill = itor.Value();
		INT equippedSkillID = 0;
		if( cachedEquipeddSkill != NULL ){
			equippedSkillID = cachedEquipeddSkill->eventGetAbilityId();
		}
		if( equippedSkillID == skillTemplateID ){
			return cachedEquipeddSkill;
		}
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
//记录变身中的技能变化
void MMOSkillManager::InTransformStateSkillChanged( INT hotKey, INT skillMainID, INT skillLevel, INT skillRuneIdx, UBOOL isLocked )
{
	INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( skillMainID, skillLevel, skillRuneIdx );
	UMMOAbility* pNewSkill = NULL;
	if( !isLocked ){
		pNewSkill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
		if( pNewSkill == NULL ){
			M_AddDataError(TEXT("spell.txt"), FString::Printf(
				TEXT("变身技能未找到，SkillID:%d LV:%d RuneID:%d"),
				skillMainID, skillLevel, skillRuneIdx ));
		}
		else{
			//记录技能对应的快捷键,可能会被擦除，不过PlayerController的EquipNewSpellAndAutoUnequipOld会重新设置上
			pNewSkill->SlotHotKeyID = hotKey;
		}
	}

	_InTransformSkillCache.FindOrAdd( hotKey ) = pNewSkill;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
//回复记录的技能变化
void MMOSkillManager::RecoverCachedSkillsInTransform()
{
	AMMOPlayerController *mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( mainPlayerCtrl == NULL ){
		return;
	}
	
	//新清空记录
	for( TMap< INT, UMMOAbility* >::TIterator itor( _InTransformSkillCache ); itor; ++itor ){
		ECommandKey hotKey = (ECommandKey)itor.Key();
		UMMOAbility *oldEquipedSkill = itor.Value();

		UBOOL hasSkill = FALSE;
		INT skillID = 0;
		INT skillLv = 1;
		INT skillRuneID = 0;
		if( oldEquipedSkill != NULL ){
			hasSkill = TRUE;
			skillID = oldEquipedSkill->eventGetAbilityId();
			skillLv = oldEquipedSkill->SpellCommonAttribute.Level;
			skillRuneID = oldEquipedSkill->GetRuneSlot();
		}

		//recover old skill
		mainPlayerCtrl->EquipNewSpellAndAutoUnequipOld( hotKey, skillID, skillLv, skillRuneID, hasSkill );

		//
		MMOGameEvent_OneInt evt( GameEvent_ChangeSkillSlot, hotKey );
		MMOUI::PostGameEvent(&evt);
	}

	//
	_InTransformSkillCache.Empty();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
UBOOL MMOSkillManager::EquipTransformSkillGroupAndCachOldSkills( INT skillGroupID )
{
	const TransformSkillGroup *skillGroup = FindTransformSkillGroupByBuffID( skillGroupID );
	AMMOPlayerController *mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( skillGroup == NULL || mainPlayerCtrl == NULL || mainPlayer == NULL ){
		return FALSE;
	}

	UUIKeyMapManager *keyMapManager = mainPlayerCtrl->KeyMapManager;
	if( keyMapManager == NULL ){
		return FALSE;
	}
	warnf(NAME_Warning, TEXT("======!!!!==================================== [EquipTransformSkillGroupAndCachOldSkills AT %d]"), skillGroupID );
	//新清空记录
	_InTransformSkillCache.Empty();
	for( INT i = 0; i < SKILL_EQUIP_MAX; ++i ){
		ECommandKey hotKey = GetGetTransformSkillGroupBindHotKey(i);
		INT skillID = skillGroup->_MainSkillID[i];
		UMMOAbility *transformGroupSkill = NULL;
		UBOOL isTransformSkill = FALSE;
		if( skillID > 0 ){
			isTransformSkill = TRUE;
			transformGroupSkill = mainPlayer->GetMainAbilityById( skillID );
		}

		INT skillLevel = ( transformGroupSkill != NULL ? transformGroupSkill->SpellCommonAttribute.Level : 1 );
		//cache old
		INT curSkillID = keyMapManager->GetCmdIDBySlotID( hotKey );
		UMMOAbility *oldEquipedSkill = mainPlayer->GetEquipedAbilityById( curSkillID );
		_InTransformSkillCache.FindOrAdd( hotKey ) = oldEquipedSkill;

		//equip new
		mainPlayerCtrl->EquipNewSpellAndAutoUnequipOld( hotKey, skillID, skillLevel, 0, isTransformSkill );

		//change slot in skill bar
		MMOGameEvent_OneInt evt( GameEvent_ChangeSkillSlot, hotKey );
		MMOUI::PostGameEvent(&evt);
	}

	return TRUE;
}

//
UMMOAbility *MMOSkillManager::GetEquippedSkillBySkillID( INT skillTemplateID )
{
	AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainCtrl == NULL || mainPlayer == NULL ){
		return NULL;
	}

	UMMOAbility* pEquipedSkill = NULL;
	//在变身状态,取得被缓存了的装配技能
	if( mainCtrl->IsTransformed() ){
		pEquipedSkill = GetInTransformCachedSkillByID( skillTemplateID );
	}

	//正常状态
	if( pEquipedSkill == NULL ){
		UUIKeyMapManager* keyMapMgr = mainCtrl->KeyMapManager;
		if( keyMapMgr != NULL){
			pEquipedSkill = mainPlayer->GetEquipedAbilityById( skillTemplateID );
		}
	}

	//
	return pEquipedSkill;
}

//
UMMOAbility *MMOSkillManager::GetEquippedSkill( INT slotHotKey, UBOOL onlyOnSkillBar/* = FALSE*/ )
{
	AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainCtrl == NULL || mainPlayer == NULL ){
		return NULL;
	}

	UMMOAbility* pEquipedSkill = NULL;
	//在变身状态,取得被缓存了的装配技能
	if( mainCtrl->IsTransformed() && onlyOnSkillBar == FALSE ){
		pEquipedSkill = GetInTransformCachedSkill( slotHotKey );
	}

	//正常状态
	if( pEquipedSkill == NULL ){
		UUIKeyMapManager* keyMapMgr = mainCtrl->KeyMapManager;
		if( keyMapMgr != NULL){
			INT skillTemplateID = keyMapMgr->GetCmdIDBySlotID( slotHotKey );
			pEquipedSkill = mainPlayer->GetEquipedAbilityById( skillTemplateID );
		}
	}

	//
	return pEquipedSkill;
}

//
INT MMOSkillManager::GetEquippedSkillTemplateID( INT slotHotKey )
{
	AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
	if( mainCtrl == NULL ){
		return -1;
	}

	//
	INT equipedSkillID = -1;
	//在变身状态,取得被缓存了的装配技能
	if( mainCtrl->IsTransformed() ){
		UMMOAbility *cachedSkill = GetInTransformCachedSkill( slotHotKey );
		if( cachedSkill != NULL ){
			equipedSkillID = cachedSkill->eventGetAbilityId();
		}
	}

	//正常状态
	if( equipedSkillID <= 0 ){
		UUIKeyMapManager* keyMapMgr = mainCtrl->KeyMapManager;
		if( keyMapMgr != NULL){
			equipedSkillID = keyMapMgr->GetCmdIDBySlotID( slotHotKey );
		}
	}

	//
	return equipedSkillID;
}

//===============================================================================================
void MMOSkillManager::UpdateAbility( INT mainAbilityID, INT abilityRuneID, INT level, INT equipID /*= -1*/ )
{
	AMMOPlayerController *mainCtrl = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainCtrl == NULL || mainPlayer == NULL ){
		return;
	}

	//是否装备过该技能
	UMMOAbility *pEquipedSkill = GetEquippedSkillBySkillID( mainAbilityID );
	INT slotBindHotKey = ( pEquipedSkill != NULL ? pEquipedSkill->SlotHotKeyID : -1 );//一定要先得，mainPlayer->UpdateAbility会做清理

	//先更新技能数据（换高等级技能）,清除旧技能CD，hotKeyBingding信息
	mainPlayer->UpdateAbility( mainAbilityID, abilityRuneID, level );

	//后，如果先前装配的是该技能，替换快捷键绑定的技能到该技能的新等级
	if( pEquipedSkill != NULL && abilityRuneID == pEquipedSkill->GetRuneSlot() ){
		//
		UBOOL isCached = mainCtrl->IsTransformed() && IsSkillChangeNeedCache( slotBindHotKey );
		if( isCached ){
			//正装配着该技能
			InTransformStateSkillChanged( slotBindHotKey, mainAbilityID, level, abilityRuneID, FALSE );
		}
		else if( mainCtrl->KeyMapManager ){
			mainCtrl->KeyMapManager->BindingSlot( slotBindHotKey, UICT_UseAbility, mainAbilityID );
		}
	}

	//
	UINT toUpdateAbilityUniqueID = MMOSkillManager::CombinateAbilityID( mainAbilityID, level, abilityRuneID );
	if( abilityRuneID == 0 ){
		//是主技能更新
		if( equipID != -1 ){
			//更新装备获取的技能信息
			AddMyLearnedMainSkillByEquip( toUpdateAbilityUniqueID, equipID );
		}
		else{
			//更新自身学会的技能信息
			AddMyLearnedMainSkillByBook( toUpdateAbilityUniqueID );
		}

		//
		UMMOAbility *toUpdateAbility = FindAbilityTemplate( toUpdateAbilityUniqueID );
		if( toUpdateAbility != NULL ){
			//直接替换新的
			_MyLearnedMainSkills.FindOrAdd( mainAbilityID ) = toUpdateAbility;
		}
	}

	//
	MMOGameEvent_SkillUiUpdate uiEvt( MMOGameEvent_SkillUiUpdate::ESkillUiEvent_EquippedSkillUpdated );
	uiEvt.MagicInt = toUpdateAbilityUniqueID;
	MMOUI::PostGameEvent( &uiEvt );
}

//===============================================================================================
//设置主技能
void MMOSkillManager::SendEquipSpell( INT slotHotKey, INT skillID )
{
	//UMMOAbility *curEquippedSkill = GetEquippedSkill( slotHotKey );
	//INT curEquippedSkillID = ( curEquippedSkill != NULL ? curEquippedSkill->eventGetAbilityId() : 0 );
	//if( curEquippedSkill == NULL || curEquippedSkillID != skillID ){
	//	//与原设置不相同才设置
		CMessageHandler::GetInstance().Send_EquipSpell( slotHotKey, skillID );
	//}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
//设置符文技能
void MMOSkillManager::SendEquipRuneSkill( INT slotHotKey, INT runeSlotId )
{
	//UMMOAbility *curEquippedSkill = GetEquippedSkill( slotHotKey );
	////INT curEquippedSkillID = ( curEquippedSkill != NULL ? curEquippedSkill->eventGetAbilityId() : 0 );
	//INT curEquippedSkillRuneID = ( curEquippedSkill != NULL ? curEquippedSkill->GetRuneSlot() : 0 );
	//if( curEquippedSkillRuneID != runeSlotId ){
	//	//
	//	CMessageHandler::GetInstance().Send_EquipRune( slotHotKey, runeSlotId );
	//}
}

//===============================================================================================
ECommandKey MMOSkillManager::GetGetTransformSkillGroupBindHotKey( INT bindSlotIdx )
{
	switch( bindSlotIdx )
	{
//	case 0: return ECK_MouseL;
//	case 1: return ECK_MouseR;
	case 0: return ECK_Slot1;
	case 1: return ECK_Slot2;
	case 2: return ECK_Slot3;
	case 3: return ECK_Slot4;
	case 4: return ECK_Slot5;
	case 5: return ECK_Slot6;
	}

	return ECK_MAX;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
//变身后技能组
UBOOL MMOSkillManager::LoadTransformSkillGroup()
{
	//TArray<TArray<FString>> ArrayString2D;
//#if 0 // TABFILE
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("transform.txt"), TRUE ) )
	//if( !MMOFacade::getSingleton().ReadConfigString( ArrayString2D, TEXT("transform.txt")) )
//#endif
	{
		return FALSE;
	}

	INT skipRow = 2;//M_SKIP_ROWS;
	for( INT i = skipRow; i < tabFile->GetRowCount(); ++i ){
		INT buffID = appAtoi(tabFile->GetData( TEXT("AuraID"), i));
		TransformSkillGroup &skillGroup = _TransformSkillGroupMap.FindOrAdd( buffID );
		skillGroup._MainSkillID[0] = appAtoi(tabFile->GetData( TEXT("qssMouseL"), i ));
		skillGroup._MainSkillID[1] = appAtoi(tabFile->GetData( TEXT("qssMouseR"), i ));
		skillGroup._MainSkillID[2] = appAtoi(tabFile->GetData( TEXT("qssKey1"), i ));
		skillGroup._MainSkillID[3] = appAtoi(tabFile->GetData( TEXT("qssKey2"), i ));
		skillGroup._MainSkillID[4] = appAtoi(tabFile->GetData( TEXT("qssKey3"), i ));
		skillGroup._MainSkillID[5] = appAtoi(tabFile->GetData( TEXT("qssKey4"), i ));
	}
	delete tabFile;
	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------------------------------
const TransformSkillGroup *MMOSkillManager::FindTransformSkillGroupByBuffID( INT buffID )
{
	return _TransformSkillGroupMap.Find( buffID );
}
//======================================================================================================================================================

void AbilityTemplateIndexDataProcessor::ParseData( AbilityTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->TemplateID = TReadDataValue_Int<AbilityTemplateDataIndexManager>(TEXT("SpellCommonAttribute.ID"), dataRow, rowIdx);
	obj->ObjectPath = TReadDataValue_FString<AbilityTemplateDataIndexManager>(TEXT("Index"), dataRow, rowIdx);
	INT playerClass = TReadDataValue_Int<AbilityTemplateDataIndexManager>(TEXT("SpellCommonAttribute.Class"), dataRow, rowIdx);
	memcpy(&obj->Class, &playerClass, sizeof(FSpell_ClassType) > sizeof(INT) ? sizeof(FSpell_ClassType) : sizeof(INT));

	obj->RuneSlotID = TReadDataValue_Int<AbilityTemplateDataIndexManager>(TEXT("SpellCommonAttribute.RuneSlotID"), dataRow, rowIdx);

	obj->CastType = (ESPELL_CAST_TYPE)TReadDataValue_Int<AbilityTemplateDataIndexManager>(TEXT("SpellCommonAttribute.CastType"), dataRow, rowIdx);
	obj->QTEType = (ESPELL_QTE_TYPE)TReadDataValue_Int<AbilityTemplateDataIndexManager>(TEXT("SpellCommonAttribute.QTEType"), dataRow, rowIdx);
	obj->ShowOnUI = TReadDataValue_Int<AbilityTemplateDataIndexManager>(TEXT("ClientCommonAttribute.ShowOnUI"), dataRow, rowIdx) > 0;

	INT id = MMOSkillManager::CombinateAbilityID(obj->TemplateID, 0, obj->RuneSlotID);
	AbilityTemplateDataIndexManager::getSingleton().AddData(id, obj);
}

void SpellVarDataProcessor::ParseData( SpellVarData* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->TemplateID = TReadDataValue_Int<SpellVarManager>(TEXT("ID"), dataRow, rowIdx);
	obj->RuneSlotID = TReadDataValue_Int<SpellVarManager>(TEXT("RuneSlotID"), dataRow, rowIdx);
	obj->Level = TReadDataValue_Int<SpellVarManager>(TEXT("Level"), dataRow, rowIdx);
	
	obj->Variable1 = TReadDataValue_FString<SpellVarManager>(TEXT("Variable1"), dataRow, rowIdx);
	obj->Value1 = TReadDataValue_FString<SpellVarManager>(TEXT("Value1"), dataRow, rowIdx);

	obj->Variable2 = TReadDataValue_FString<SpellVarManager>(TEXT("Variable2"), dataRow, rowIdx);
	obj->Value2 = TReadDataValue_FString<SpellVarManager>(TEXT("Value2"), dataRow, rowIdx);
	
	obj->Variable3 = TReadDataValue_FString<SpellVarManager>(TEXT("Variable3"), dataRow, rowIdx);
	obj->Value3 = TReadDataValue_FString<SpellVarManager>(TEXT("Value3"), dataRow, rowIdx);

	obj->Variable4 = TReadDataValue_FString<SpellVarManager>(TEXT("Variable4"), dataRow, rowIdx);
	obj->Value4 = TReadDataValue_FString<SpellVarManager>(TEXT("Value4"), dataRow, rowIdx);

	obj->DetailDescription = TReadDataValue_FString<SpellVarManager>(TEXT("DetailDescription"), dataRow, rowIdx);
	//新版tips增加了一些内容
	obj->SpellName = TReadDataValue_FString<SpellVarManager>(TEXT("SpellName"), dataRow, rowIdx);
	obj->MainSpeciality = TReadDataValue_FString<SpellVarManager>(TEXT("MainSpeciality"), dataRow, rowIdx);
	obj->SecondSpeciality = TReadDataValue_FString<SpellVarManager>(TEXT("SecondSpeciality"), dataRow, rowIdx);
	obj->ThirdSpeciality = TReadDataValue_FString<SpellVarManager>(TEXT("ThirdSpeciality"), dataRow, rowIdx);	
	//obj->DescriptiveWords = TReadDataValue_FString<SpellVarManager>(TEXT("DescriptiveWords"), dataRow, rowIdx);
	obj->RuneDescription = TReadDataValue_FString<SpellVarManager>(TEXT("RuneDescription"), dataRow, rowIdx);
	//obj->UseMethod = TReadDataValue_FString<SpellVarManager>(TEXT("UseMethod"), dataRow, rowIdx );
	//obj->AttackRange = TReadDataValue_FString<SpellVarManager>(TEXT("AttackRange"), dataRow, rowIdx );
	//obj->EffectRange = TReadDataValue_FString<SpellVarManager>(TEXT("EffectRange"), dataRow, rowIdx );
	//obj->EffectRangeEnum = TReadDataValue_Int<SpellVarManager>(TEXT("EffectRangeEnum"), dataRow, rowIdx);

	INT id = MMOSkillManager::CombinateAbilityID(obj->TemplateID, obj->Level, obj->RuneSlotID);
	SpellVarManager::getSingleton().AddData(id, obj);
}

void SpellProgessDataProcessor::ParseData( SpellProgessData* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->TemplateID = TReadDataValue_Int<SpellProgessDataManager>(TEXT("SpellID"), dataRow, rowIdx);
	obj->RuneSlot = TReadDataValue_Int<SpellProgessDataManager>(TEXT("RuneSlot"), dataRow, rowIdx);
	obj->Level = TReadDataValue_Int<SpellProgessDataManager>(TEXT("Level"), dataRow, rowIdx);
	obj->ConsumePts = TReadDataValue_Int<SpellProgessDataManager>(TEXT("ConsumePts"), dataRow, rowIdx);
	obj->RequireLevel = TReadDataValue_Int<SpellProgessDataManager>(TEXT("RequireLevel"), dataRow, rowIdx);
	//obj->RequireItem = TReadDataValue_Int<SpellProgessDataManager>(TEXT("RequireItem"), dataRow, rowIdx);
	//obj->RequireItemCount = TReadDataValue_Int<SpellProgessDataManager>(TEXT("RequireItemCount"), dataRow, rowIdx);
	obj->Pow = TReadDataValue_Int<SpellProgessDataManager>(TEXT("Pow"), dataRow, rowIdx);

	INT id = MMOSkillManager::CombinateAbilityID(obj->TemplateID, obj->Level, obj->RuneSlot);
	SpellProgessDataManager::getSingleton().AddData(id, obj);
}

// void RuneDataProcessor::ParseData( RuneData* obj, class CTabFile* dataRow, int rowIdx )
// {
// 
// }


void MissileTemplateIndexDataProcessor::ParseData( MissileTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->TemplateID = TReadDataValue_Int<MissileTemplateDataIndexManager>(TEXT("ID"), dataRow, rowIdx);
	obj->ObjectPath = TReadDataValue_FString<MissileTemplateDataIndexManager>(TEXT("Index"), dataRow, rowIdx);

	INT id = obj->TemplateID;
	MissileTemplateDataIndexManager::getSingleton().AddData(id, obj);
}

#if WITH_EDITOR

void AbilityTemplateIndexDataProcessor::CollectReferencedResourceFromDataTable( AbilityTemplateDataIndex* obj, TSet<FString>& outResList )
{
	UObject* dat = UObject::StaticLoadObject(UMMOAbility::StaticClass(), NULL,
		*obj->ObjectPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL );
	if(dat)
	{
		_CollectReferencedResource(dat, outResList);
	}
}

void MissileTemplateIndexDataProcessor::CollectReferencedResourceFromDataTable( MissileTemplateDataIndex* obj, TSet<FString>& outResList )
{
	UObject* dat = UObject::StaticLoadObject(UMissile::StaticClass(), NULL,
		*obj->ObjectPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL );
	if(dat)
	{
		_CollectReferencedResource(dat, outResList);
	}

}

#endif