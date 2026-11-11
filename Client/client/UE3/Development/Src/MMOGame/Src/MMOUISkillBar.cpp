#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "MMOSkillManager.h"
#include "MMOCoolDownManager.h"
#include "MMOEquipManager.h"
#include "MMOClientVariableManager.h"

IMPLEMENT_CLASS(UMMOUISkillBar);

//控件宽度
#define ExpBarMaxWidth (432)
#define SKILL_BAR_ITEM_SLOT_SPEC_PREFIX (0xffff)
#define SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID (-1)

bool g_drawMaterialGlobe = false;
bool g_drawMaterialGlobeHalf = true;

//=====================================================================================
void UMMOUISkillBar::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if(show){
		//
		if( Player != NULL && Player->ActorAttributes != NULL ){
			PlayerExpChange( Player->ActorAttributes->LevelAttributes.CurrentExp, Player->ActorAttributes->LevelAttributes.MaxExp,0 );
		}
		//
		MMOGameEvent_Any evt(GameEvent_RequestMainPlayerData);
		MMOUI::PostGameEvent(&evt);
	}
	else{
	}
}
//==============================================================================================================
void UMMOUISkillBar::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_ShowNewGlobe:
		{
			eventCallShowGlobe(!g_drawMaterialGlobeHalf);
		}
		break;

	case GameEvent_Refresh_SkillSlotLoop:
		{
			MMOGameEvent_OneInt *skillLoopStateEvt = (MMOGameEvent_OneInt*)evt;
			if( skillLoopStateEvt->IntData != 0 ){
				RefreshAllSkillSlotLoopEffect( skillLoopStateEvt->IntData );
			}
			else{
				RefreshAllSkillSlotLoopEffect( 0 );
			}
		}
		break;
	case GameEvent_Sync_SkillEquippedSkills:
	case GameEvent_Refresh_SkillSlotIcon:
		{
			ResetAllSkillSlots();
		}
		break;
	case GameEvent_MainPlayerDataChange:
		{
			MMOGameEvent_MainPlayerDataChange* e = (MMOGameEvent_MainPlayerDataChange*)evt;
			switch(e->DataChangeEventType)
			{
			case MMOGameEvent_MainPlayerDataChange::EMSET_HP:
				{
					PlayerHPChange(e->CurrentIntValue, e->MaxIntValue);
				}
				break;

			case MMOGameEvent_MainPlayerDataChange::EMSET_MP:
				{
					PlayerMPChange(e->CurrentIntValue, e->MaxIntValue);
				}
				break;
			}
		}
		break;
	case GameEvent_ChangeSkillSlot:
		{
			MMOGameEvent_OneInt* e = (MMOGameEvent_OneInt*)evt;
			if(e)
			{
				UpdateSkillSlot(e->IntData);
			}

			//上传一下技能栏数据
			if( IsSlotDirty( -1 ) ){
				Send_BarSlotData();
			}
		}
		break;
	case GameEvent_SyncEquippedWeapon:
		{
			SetSlotUpdateType( ESBarSlotUpdTp_WeaponType );
			UpdateAbilityEnableState();
		}
		break;
	case GameEvent_Equip:
		{
			MMOGameEvent_Item *itemEquipEvt = (MMOGameEvent_Item*)evt;

			if( itemEquipEvt->SlotIdx == E_NES_MAIN_HAND ){
				SetSlotUpdateType( ESBarSlotUpdTp_WeaponType );
				UpdateAbilityEnableState();
			}
		}
		break;
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			if( IsVisible ){
				UpdateItemSlotForItemDeleted( itemDelEvt->ItemTemplateID );
			}
		}
		break;
	case GameEvent_Item_SlotSwapped:
		{
			MMOGameEvent_ItemSwap *itemSwapEvt = (MMOGameEvent_ItemSwap*)evt;
			OnItemSwapped( itemSwapEvt->SrcConID, itemSwapEvt->SrcSlotID, itemSwapEvt->DstConID, itemSwapEvt->DstSlotID );
		}
		break;
	case GameEvent_SortItemsInBagSuccess:
		{
			MMOGameEvent_Any *itemsRerodered = (MMOGameEvent_Any*)evt;
			ResetAllItemsByIDs();
		}
		break;
	case GameEvent_Item_SlotUpdated:
		{
			MMOGameEvent_Item *itemUpdatedEvt = (MMOGameEvent_Item*)evt;
			if( itemUpdatedEvt == NULL ){
				break;
			}

			OnItemSlotChanged( itemUpdatedEvt->ContainerIdx, itemUpdatedEvt->SlotIdx );
		}
		break;
	case GameEvent_EnterGameSuccess:
		{
			if( !IsBarTypeUpdated ){
				UpdateCurBarShowType();
			}

			//eventUCShowNewSkillHintBtn( FALSE );
		}
		break;
	case GameEvent_FinishLoading:
		{
			//新人默认物品栏
			if( Player != NULL && Player->ActorAttributes->LevelAttributes.Level <= 1 && Player->ActorAttributes->LevelAttributes.CurrentExp < 10 ){
				InitItemBarItemForNewComing();
			}
		}
		break;
	case GameEvent_EquipRuneSkill:
		{
			MMOGameEvent_EquipRuneSkill* equipRuneSkillEvt = (MMOGameEvent_EquipRuneSkill*)evt;

			UpdateSkillSlot( equipRuneSkillEvt->SkillSlotEnum );
		}
		break;
	case GameEvent_SkillLevelUp:
		{
			MMOGameEvent_SkillLevelUp *lvUpSkillEvt = (MMOGameEvent_SkillLevelUp*)evt;

			UMMOAbility* tAbility = Player->GetEquipedAbilityById( lvUpSkillEvt->SkillID );
			if( tAbility != NULL ){
				UpdateSkillSlot( tAbility->SlotHotKeyID );
			}
		}
		break;
	case GameEvent_SkillUiUpdate:
		{
			MMOGameEvent_SkillUiUpdate* skillEvent = (MMOGameEvent_SkillUiUpdate*)evt;
			switch(skillEvent->EventType)
			{
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_EquippedSkillUpdated:
				{
					UINT uniqueSkillID = skillEvent->MagicInt;
					UINT skillID = 0;
					UINT skillLv = 0;
					UINT skillRuneID = 0;
					MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );

					UMMOAbility* tAbility = Player->GetEquipedAbilityById( skillID );
					if( tAbility != NULL ){
						UpdateSkillSlot( tAbility->SlotHotKeyID );
					}
				}
				break;
			//case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillForget:
			//	{
			//		INT skillID = skillEvent->MagicInt;

			//		UMMOAbility* tAbility = Player->GetEquipedAbilityById( skillID );
			//		if( tAbility == NULL ){
			//			UnequipAllSkills( skillID, TRUE );
			//		}
			//		else{
			//			UpdateSkillSlot( tAbility->SlotHotKeyID );
			//		}
			//	}
			//	break;
			}
		}
		break;
	case GameEvent_NewCD:
		{
			MMOGameEvent_NewCD* msg = (MMOGameEvent_NewCD*)evt;
			if( msg )
			{
				UpdateAllItemSlotCDByItemUse( msg->CDType , msg->CDId );
			}
		}
		break;
	case GameEvent_ExpExtraChanged:
		{
			MMOGameEvent_OneInt *e = (MMOGameEvent_OneInt*)evt;

			if( Exp_Extra > 0 ){
				Exp_Extra += e->IntData;
				if (Exp_Extra <= 0){
					eventCallShowExpExtraBuff(FALSE);
				}

				INT changeExp = GetPlayerExp_Extra();
				eventCallSetExpExtra( changeExp );
			}
		}
		break;
	case GameEvent_ExpExtraInit:
		{
			MMOGameEvent_OneInt *e = (MMOGameEvent_OneInt*)evt;
			Exp_Extra = e->IntData;

			INT changeExp = GetPlayerExp_Extra();
			eventCallSetExpExtra( changeExp );

			if (Exp_Extra <= 0){
				eventCallShowExpExtraBuff(FALSE);
			}
			else{
				eventCallShowExpExtraBuff(TRUE);
			}
		}
		break;
	default:
		break;
	}
}
//===============================================================================================================
void UMMOUISkillBar::OnItemSlotChanged( INT containerID, INT slotID )
{
	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem( containerID, slotID );
	if( containerID != SKILL_BAR_ONLY_ITEM_CONTAINER ){
		//只接受包裹里的物品
		return;
	}

	if( itemInfo == NULL ){
		return;
	}
	INT itemID = itemInfo->m_nTemplateID;
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Item || slotData.AidID != itemID ){
			continue;
		}

		UItem *pItem = MMOFacade::getSingleton().FindItemTemplate( itemID );
		UBOOL isMutiItemsOnSkillBar = MMOEquipManager::getSingleton().IsItemMultiOnSkillBar( pItem );
		if( isMutiItemsOnSkillBar ){
			//没有固定位置，重新设置一下
			INT inBagFirstItemSlotIdx = -1;
			FClientItemInfo *slotItemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer( slotData.AidID, SKILL_BAR_ONLY_ITEM_CONTAINER, inBagFirstItemSlotIdx );
			INT itemPosID = (INT)MMOEquipManager::CombinateItemPosID( SKILL_BAR_ONLY_ITEM_CONTAINER, inBagFirstItemSlotIdx );
			SetBarSlot( i, ESBarSlotTp_Item, itemPosID );
			//SetBarSlot( i, ESBarSlotTp_Item, itemID );
		}
		else{
			//有固定位置，重新设置一下
			INT changedItemPosID = (INT)MMOEquipManager::CombinateItemPosID( containerID, slotID );
			if( slotData.ID == changedItemPosID ){
				//重新设置一下
				SetBarSlot( i, ESBarSlotTp_Item, slotData.ID );
			}
		}
	}

	//上传
	if( IsSlotDirty( -1 ) ){
		Send_BarSlotData();
	}
}
//
void UMMOUISkillBar::OnItemSwapped( INT srcConIdx, INT srcSlotIdx, INT dstConIdx, INT dstSlotIdx )
{
	if( srcConIdx != SKILL_BAR_ONLY_ITEM_CONTAINER && dstConIdx != SKILL_BAR_ONLY_ITEM_CONTAINER ){
		return;
	}
	INT changedSrcItemPosID = (INT)MMOEquipManager::CombinateItemPosID( srcConIdx, srcSlotIdx );
	INT changedDestItemPosID = (INT)MMOEquipManager::CombinateItemPosID( dstConIdx, dstSlotIdx );

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Item ){
			continue;
		}

		INT changeToPosID = 0;
		if( changedSrcItemPosID == slotData.ID ){
			changeToPosID = ( dstConIdx == SKILL_BAR_ONLY_ITEM_CONTAINER ? changedDestItemPosID : changedSrcItemPosID );
			
			SetBarSlot( i, ESBarSlotTp_Item, changeToPosID );
		}
		else if( changedDestItemPosID == slotData.ID ){
			changeToPosID = ( srcConIdx == SKILL_BAR_ONLY_ITEM_CONTAINER ? changedSrcItemPosID : changedDestItemPosID );

			SetBarSlot( i, ESBarSlotTp_Item, changeToPosID );
		}
	}

	//上传
	if( IsSlotDirty( -1 ) ){
		Send_BarSlotData();
	}
}

//===============================================================================================================
void UMMOUISkillBar::UpdateItemSlotForItemDeleted( INT itemTemplateID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	INT conID = SKILL_BAR_ONLY_ITEM_CONTAINER;
	INT slotID = -1;

	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Item ){
			continue;
		}

		if( itemTemplateID != slotData.AidID ){
			continue;
		}
		UItem *putItem = mmoFacade.FindItemTemplate( slotData.AidID );
		if( putItem == NULL ){
			SetBarSlotToNoData( i );
			continue;
		}

		INT newItemPosID = -1;
		INT newItemSlotID = -1;

		INT curBarItemConIdx = -1;
		INT curBarItemSlotIdx = -1;
		MMOEquipManager::DecomposeItemPosID( (UINT)slotData.ID, curBarItemConIdx, curBarItemSlotIdx );
		FClientItemInfo *putItemInfo = mmoFacade.FindItem( curBarItemConIdx, curBarItemSlotIdx );
		if( putItemInfo == NULL ){
			//物品被删除了
			UBOOL isMutiItemsOnSkillBar = mmoEquipMgr.IsItemMultiOnSkillBar( putItem );
			if( isMutiItemsOnSkillBar ){
				putItemInfo = mmoFacade.FindFirstItemPosInContainer( itemTemplateID, SKILL_BAR_ONLY_ITEM_CONTAINER, newItemSlotID );

				if( putItemInfo == NULL ){
					newItemPosID = SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID;
				}
				else{
					newItemPosID = (INT)MMOEquipManager::CombinateItemPosID( SKILL_BAR_ONLY_ITEM_CONTAINER, newItemSlotID );
				}
			}
		}
		else{
			newItemPosID = slotData.ID;
		}
		//重新设置一下
		//SetBarSlot( i, ESBarSlotTp_Item, slotData.ID );
		SetBarSlot( i, ESBarSlotTp_Item, newItemPosID );
	}

	//上传
	if( IsSlotDirty( -1 ) ){
		Send_BarSlotData();
	}
}

//===============================================================================================================
//快捷栏上所有物品链接重置为包裹中第一个所记录物品ID的位置
void UMMOUISkillBar::ResetAllItemsByIDs()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	INT conID = SKILL_BAR_ONLY_ITEM_CONTAINER;
	INT slotID = -1;

	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Item ){
			continue;
		}
		INT itemTemplateID = slotData.AidID;

		UItem *putItem = mmoFacade.FindItemTemplate( itemTemplateID );
		if( putItem == NULL ){
			SetBarSlotToNoData( i );
			continue;
		}
		
		INT newItemSlotID = -1;
		INT newItemPosID = -1;
		FClientItemInfo *putItemInfo = mmoFacade.FindFirstItemPosInContainer( itemTemplateID, SKILL_BAR_ONLY_ITEM_CONTAINER, newItemSlotID );
		if( putItemInfo == NULL ){
			newItemPosID = SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID;
		}
		else{
			newItemPosID = (INT)MMOEquipManager::CombinateItemPosID( SKILL_BAR_ONLY_ITEM_CONTAINER, newItemSlotID );
		}

		//重新设置一下
		SetBarSlot( i, ESBarSlotTp_Item, newItemPosID );
	}

	//上传
	if( IsSlotDirty( -1 ) ){
		Send_BarSlotData();
	}
}
//==============================================================================================================
void UMMOUISkillBar::PlayerExpChange( INT exp, INT maxExp,INT ChangeExp )
{
	if( GFXPlayer == NULL ){
		return;
	}

	UGFxObject* expData = GFXPlayer->CreateObject( TEXT("Object") );
	if( expData == NULL ){
		return;
	}

	INT expPercent = 0;
	expPercent = ( ( maxExp > 0 ) ? ( FLOAT( exp ) / maxExp * 100 ) : 0 );
	expPercent = Clamp( expPercent, 0, 100 );

	//实际要留一点
	INT expWidth = Clamp( ExpBarMaxWidth * expPercent / 100 , 1, ExpBarMaxWidth );
	expData->SetInt( TEXT("maskWidth"), expWidth );

	expData->SetInt( TEXT("maxExp"), maxExp );
	expData->SetInt( TEXT("curExpBarX"), 0 );

	expData->SetString( TEXT("expPercentText"), FString::Printf(TEXT("%d%%"), expPercent) );

	
	FString extText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skillbar.ExpText") ), exp, maxExp );
	
	UBOOL isTodayExpFull = MMOFacade::getSingleton().TodayExpIsFull();
	UBOOL isExpFull = isTodayExpFull || ( exp >= maxExp );
	if( isExpFull ){
		if( isTodayExpFull ){
			extText += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skillbar.expfull") );
		}
		//else{
		//	extText = TEXT("");
		//}
		expData->SetString( TEXT("frameText") , TEXT("full") );
	}
	else{
		expData->SetString( TEXT("frameText") , TEXT("normal") );
	}

	expData->SetString( TEXT("expText") , extText );

	eventCallUCSetExpText( expData );

	INT extraExp = GetPlayerExp_Extra();
	eventCallSetExpExtra( extraExp );
}
//----------------------------------------------------------------------
INT UMMOUISkillBar::GetPlayerExp_Extra()
{
	if( Player == NULL || Player->ActorAttributes == NULL ){
		return 0;
	}

	INT currentExp = Player->ActorAttributes->LevelAttributes.CurrentExp;
	INT maxExp = Player->ActorAttributes->LevelAttributes.MaxExp;

	INT exp_Extra_Percent = 0;
	if( Exp_Extra <= 0 ){
		return exp_Extra_Percent;
	}
	exp_Extra_Percent = FLOAT( currentExp + Exp_Extra * 2) / maxExp * 100;
	exp_Extra_Percent = Clamp( exp_Extra_Percent, 1, 100);

	return exp_Extra_Percent;
}
//==============================================================================================================
void UMMOUISkillBar::PlayerHPChange( INT newHP, INT maxHP )
{
	FLOAT hpPercent;
    if( maxHP <= 0 || newHP <= 0 ){
        hpPercent = 0.01;
    }
    else{
        hpPercent = FLOAT(newHP) / maxHP ;
	}
	hpPercent = Clamp( hpPercent, 0.01f, 1.0f );

	//
	UBOOL bTailHP;
    if( LastHP <=  newHP ){
        bTailHP = false;
	}
    else{
        bTailHP = true;
	}

    if( HpBar ){
        UGFxObject* hpTxt = HpBar->GetObject( TEXT("Hptxt_mc"));
        if( hpTxt != NULL ){
            hpTxt->SetText( FString::Printf( TEXT("%d"), newHP ) );
        }

		TArray<	FASValue> args;
		FASValue asval;

        asval.Type = AS_Number;
        asval.N = hpPercent;
        args.AddItem( asval );

        asval.Type = AS_Boolean;
        asval.B = bTailHP;
        args.AddItem( asval );

        HpBar->Invoke( "HPTail",args);
    }
    LastHP = newHP;

	if( g_drawMaterialGlobeHalf ){
		MaterialGlobeHalf_Blood->SetScalarParameterValue( TEXT("Percent"), hpPercent );	
	}

	UpdateCurBarShowType();
}

//==============================================================================================================
void UMMOUISkillBar::PlayerMPChange( INT newMP,INT maxMP )
{
	FLOAT mpPercent;
    if( maxMP < 0 || newMP<0 ){
        mpPercent = 0.0;
    }
    else{
        mpPercent = FLOAT(newMP) / maxMP;
	}

	bool bTailMP;
    if( LastMP <= newMP ){
        bTailMP = false;
	}
    else{
        bTailMP = false;
	}

    mpPercent = Clamp( mpPercent, 0.01f, 1.0f );
    if( MpBar != NULL ){
        /*UGFxObject* mpTxt = MpBar->GetObject( TEXT("mptxt_mc"));
        if(mpTxt)
        {
            mpTxt->SetText( FString::Printf(TEXT("%d"), NewMP ) );
        }*/
		TArray<	FASValue> args;
		FASValue asval;

        asval.Type = AS_Number;
        asval.N = mpPercent;
        args.AddItem( asval );

        asval.Type = AS_Boolean;
        asval.B = bTailMP;
        args.AddItem( asval );

        asval.Type = AS_Int;
        asval.I = newMP;
        args.AddItem( asval );

        MpBar->Invoke( TEXT("MPTail"), args );
    }
    LastMP = newMP;

	//
	SetSlotUpdateType( ESBarSlotUpdTp_Mana );
    UpdateAbilityEnableState();

	if( g_drawMaterialGlobeHalf != NULL ){
		UMaterialInstance* mat = GetMPBallMaterila();
		mat->SetScalarParameterValue( TEXT("Percent"), mpPercent );	
	}

	UpdateCurBarShowType();
}

void UMMOUISkillBar::PlayerAttributeChange(INT AtrrId,INT Value)
{
	AMMOPlayer* pMainPlayer= MMOFacade::getSingleton().GetMainPlayer();
	if( !pMainPlayer  || !pMainPlayer->ActorAttributes )
		return;

	if(AtrrId==catMaxMp)
	{
		if(pMainPlayer->ActorAttributes->GetCurMp()>pMainPlayer->ActorAttributes->GetMaxMp())
		{
			pMainPlayer->ActorAttributes->SetMP(pMainPlayer->ActorAttributes->GetMaxMp());
		}
		PlayerMPChange(pMainPlayer->ActorAttributes->GetCurMp(),pMainPlayer->ActorAttributes->GetMaxMp());
	}
	else if(AtrrId==catMaxHp)
	{
		if(pMainPlayer->ActorAttributes->GetCurHp()>pMainPlayer->ActorAttributes->GetMaxHp())
		{
			pMainPlayer->ActorAttributes->SetHP(pMainPlayer->ActorAttributes->GetMaxHp());
		}
		PlayerHPChange(pMainPlayer->ActorAttributes->GetCurHp(),pMainPlayer->ActorAttributes->GetMaxHp());
	}

	return;
}
//==============================================================================================================
void UMMOUISkillBar::PlayerLevelChange( INT lastLv, INT newLv )
{
	if( PlayerLevelTf == NULL ){
		return;
	}
	
	PlayerLevelTf->SetText( FString::Printf( TEXT( "Lv%d") , newLv ) );	
}
//==============================================================================================================
void UMMOUISkillBar::PlayerPkModeChange( INT PkMode )
{
    if( PlayerPKBtn != NULL )
    {
        TArray<	FASValue> args;
        FASValue asval;
        asval.Type = AS_Number;
        asval.N = PkMode;
        args.AddItem( asval );

        this->Invoke( TEXT("SetPkMode"), args );
    }	
}

//==============================================================================================================
void UMMOUISkillBar::PlayerBaseInfoChange()
{
    if( Player == NULL ){
		return;
	}

    if( PlayerNameTf != NULL ){
        PlayerNameTf->SetText( Player->GetActorName() );
    }

    if( PlayerProMc != NULL ){
        PlayerProMc->GotoAndStopI( Player->GetClassId() + 1 );
    }
}
//==============================================================================================================
void UMMOUISkillBar::BuffEnableAbility()
{
	SetSlotUpdateType( ESBarSlotUpdTp_Buff );
	UpdateAbilityEnableState();
}
//=====================================================================================
UBOOL UMMOUISkillBar::OnDragDrop( class UGFxObject* srcSlotMC, class UGFxObject* destSlotMC,
	BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
	BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	if( destSlotType == ESPType_SkillBar ){
		//放置到技能栏
		if( srcSlotType == ESPType_SkillBar ){
			//技能栏交换位置
			if( srcSlotMC == NULL || destSlotMC == NULL ){
				return TRUE;
			}

			INT srcSlotIdx = srcSlotMC->GetInt( TEXT("slotIndex") );
			INT destSlotIdx = destSlotMC->GetInt (TEXT("slotIndex") );
			if( srcSlotIdx == destSlotIdx ){
				return TRUE;
			}

			SwapBarSlotData( srcSlotIdx, destSlotIdx );

			if( IsSlotDirty( -1 ) ){
				Send_BarSlotData();
			}

			return TRUE;
		}

		if( srcSlotType == ESPType_Mount && ESPType_SkillBar == destSlotType ){
			//坐骑
			if( srcSlotMC != NULL ){
				INT mountID = srcSlotMC->GetInt( TEXT("ItemTemplateID") );

				if( destSlotMC != NULL ){
					INT desSlotIdx = destSlotMC->GetInt( TEXT("slotIndex") );
					SetBarSlot( desSlotIdx, ESBarSlotTp_Mount, mountID );

					if( IsSlotDirty( -1 ) ){
						Send_BarSlotData();
					}
				}
			}

			return TRUE;
		}

		if( srcSlotType == ESPType_Bag && ESPType_SkillBar == destSlotType ){
			//物品
			if( srcContainerID == CONSLOT_DEFAULT ){
				//来自背包
				FClientItemInfo* item = MMOFacade::getSingleton().FindItem( srcContainerID , srcSlotID );
				if( item != NULL && destSlotMC != NULL ){
					INT desSlotIdx = destSlotMC->GetInt( TEXT("slotIndex") );
					//SetBarSlot( desSlotIdx, ESBarSlotTp_Item, item->m_nTemplateID );
					INT itemPosID = (INT)MMOEquipManager::CombinateItemPosID( srcContainerID, srcSlotID );
					SetBarSlot( desSlotIdx, ESBarSlotTp_Item, itemPosID );

					if( IsSlotDirty( -1 ) ){
						Send_BarSlotData();
					}
				}
			}

			return TRUE;
		}

		if( srcSlotType == ESPType_SkillSetting && ESPType_SkillBar == destSlotType && srcSlotMC != NULL && destSlotMC != NULL ){
			//拖放新技能
			UINT uniqueSkillID = (UINT)srcSlotMC->GetInt(TEXT("itemIndex"));
			INT toEquipSkillSlotIdx = (INT)destSlotMC->GetInt( TEXT("slotIndex") );

			//先卸载相同的
			UnequipSkill( uniqueSkillID, toEquipSkillSlotIdx );

			TryToEquipOnSkill( toEquipSkillSlotIdx, uniqueSkillID );
			//SetBarSlot( toEquipSkillSlotIdx, ESBarSlotTp_Skill, uniqueSkillID );

			return TRUE;
		}
	}

	if( srcSlotType == ESPType_SkillBar && destSlotType != ESPType_SkillBar ){
		//拖出去的视为丢弃设置
		INT srcSlotIdx = srcSlotMC->GetInt( TEXT("slotIndex") );

		//FSlotContainer &srcSlotData = SkillBarSlotData( srcSlotIdx );
		//if( srcSlotData.Type == ESBarSlotTp_Skill ){
		//	TryToEquipOnSkill( srcSlotIdx, 0 );
		//}

		SetBarSlot( srcSlotIdx, ESBarSlotTp_Invalid, 0 );

		if( IsSlotDirty( -1 ) ){
			Send_BarSlotData();
		}

		return TRUE;
	}

	return FALSE;
}
//=====================================================================================
UBOOL UMMOUISkillBar::OnLeftMouseClick( class UGFxObject* mc, BYTE slotType, BYTE containerID, INT slotID, UBOOL bDefault )
{
	if( slotType != ESPType_SkillBar ){
		return FALSE;
	}

	INT slotIdx = mc->GetInt(TEXT("slotIndex"));

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	if( slotData.Type == ESBarSlotTp_Item ){
		UBOOL isOperated = DoSlotAction( slotIdx, TRUE );
	}
	return TRUE;
}

UBOOL UMMOUISkillBar::OnRightMouseClick(class UGFxObject* mc, BYTE slotType, BYTE containerID, INT slotID, UBOOL bDefault )
{
	if( slotType != ESPType_SkillBar ){
		return FALSE;
	}

	INT slotIdx = mc->GetInt(TEXT("slotIndex"));

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	if( slotData.Type == ESBarSlotTp_Item ){
		UBOOL isOperated = DoSlotAction( slotIdx, TRUE );
	}
	return TRUE;
}

//------------------------------------------------------------------
UBOOL UMMOUISkillBar::OnLeftMousePress( class UGFxObject* mc, BYTE slotType, BYTE containerID, INT slotID, UBOOL bDefault )
{
	return FALSE;
	//if( slotType != ESPType_SkillBar ){
	//	return FALSE;
	//}

	//INT slotIdx = mc->GetInt(TEXT("slotIndex"));
	//if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
	//	return TRUE;
	//}
	//FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	//if( slotData.Type != ESBarSlotTp_Skill ){
	//	//只有技能需要处理Press连续施法
	//	return TRUE;
	//}

	//UBOOL isOperated = DoSlotAction( slotIdx, FALSE );

	//return TRUE;
}
//=====================================================================================
void UMMOUISkillBar::UpdateSkillBarKeyDown( UBOOL bRelease, INT inCmdID, INT slotCmdKey )
{
	INT slotIdx = GetSkillBarSlotIdx( slotCmdKey );
	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		if( slotCmdKey != ECK_NormalAtk ){
			warnf( NAME_Error, TEXT("UpdateSkillBarKeyDown [%d] overflow"), slotCmdKey );
		}

		return;
	}

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	UGFxObject *slotObj = slotData.SlotObject;

	if( slotObj != NULL ){
		slotObj->SetString( TEXT("State"), bRelease ? TEXT("up") : TEXT("down") );
	}
}
//------------------------------------------------------------------
UBOOL UMMOUISkillBar::DoSlotAction( INT slotIdx, UBOOL isReleased )
{
	AMMOPlayerController *pMainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( pMainPlayerCtrl == NULL || Player == NULL ){
		return FALSE;
	}

	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		warnf( NAME_Error, TEXT("OnLeftMouseClick [%d] overflow"), slotIdx );
		return FALSE;
	}
	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	//
	UBOOL isOperated = FALSE;
	switch( slotData.Type )
	{
	case ESBarSlotTp_Skill:
		{
			UINT skillID;
			UINT skillLv;
			UINT skillRuneID;
			MMOSkillManager::DecomposeAbilityID( slotData.ID, skillID, skillLv, skillRuneID );

			UUIKeyMapManager* kmm = pMainPlayerCtrl->KeyMapManager;
			if( kmm == NULL ){
				isOperated = FALSE;
				break;
			}
			
			ECommandKey cmdKey = GetSkillBarSlotCmdKey( slotIdx );
			kmm->OnSlotKey( cmdKey, isReleased );
			//pMainPlayerCtrl->UseAbilityById( skillID, pMainPlayerCtrl->GetTraceActor(), FALSE );
			isOperated = TRUE;
		}
		break;
	case ESBarSlotTp_Item:
		{
			if( !isReleased ){
				break;
			}

			INT toUseItemConIdx = -1;
			INT toUseItemSlotIdx = -1;
			MMOEquipManager::DecomposeItemPosID( (UINT)slotData.ID, toUseItemConIdx, toUseItemSlotIdx );
			SendOpenPack( slotData.AidID );

			//
			FClientItemInfo *slotItemInfo = MMOFacade::getSingleton().FindItem( toUseItemConIdx, toUseItemSlotIdx );
			if( slotItemInfo != NULL && slotItemInfo->m_nTemplateID != slotData.AidID ){
				UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( slotData.AidID );
				UBOOL isMutiItemsOnSkillBar = MMOEquipManager::getSingleton().IsItemMultiOnSkillBar( pItem );
				if( isMutiItemsOnSkillBar ){
					warnf( NAME_Warning, TEXT("======!!!!======= [Skill Bar Item Slot[%d] item pos[%d] in NOT a [%d], REFLASH!]"), slotIdx, toUseItemSlotIdx, slotData.AidID );
					//数据差异，以物品TemplateID为准
					INT inBagFirstItemSlotIdx = -1;
					FClientItemInfo *slotItemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer( slotData.AidID, SKILL_BAR_ONLY_ITEM_CONTAINER, inBagFirstItemSlotIdx );
					INT itemPosID = (INT)MMOEquipManager::CombinateItemPosID( SKILL_BAR_ONLY_ITEM_CONTAINER, inBagFirstItemSlotIdx );
					//重新刷新一下
					SetBarSlot( slotIdx, ESBarSlotTp_Item, itemPosID );

					toUseItemConIdx = SKILL_BAR_ONLY_ITEM_CONTAINER;
					toUseItemSlotIdx = inBagFirstItemSlotIdx;
				}
			}

			UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Bag );
			if( pBagPanel != NULL ){
				pBagPanel->UseContainerItem( toUseItemConIdx, toUseItemSlotIdx );
			}
			isOperated = TRUE;
		}
		break;
	case ESBarSlotTp_Mount:
		{
			if( !isReleased ){
				break;
			}

			if( Player != NULL && Player->ActorAttributes ){
				if( slotData.ID == Player->ActorAttributes->MountID ){
					CMessageHandler::GetInstance().Send_EquipMount( 0 );
					isOperated = TRUE;
				}
				else{
					CMessageHandler::GetInstance().Send_EquipMount( slotData.ID );
					isOperated = TRUE;
				}
			}	
		}
		break;
	default:
		{
			if( !isReleased ){
				break;
			}

			isOperated = FALSE;
		}
		break;
	}

	return isOperated;
}
//==============================================================================================================
#pragma pack(push, 1)
struct FSKILL_BAR_SLOT
{
	BYTE type;
	INT id;
};
#pragma pack(pop)

//设置Slot
void UMMOUISkillBar::RefreshSkillBarByServerData( INT32* addr, INT  size )
{
	FSKILL_BAR_SLOT* pp = ( FSKILL_BAR_SLOT* )addr;
	if( pp == NULL || size <= 0 ){
		return;
	}
	check( size >= sizeof( FSKILL_BAR_SLOT ) * UCONST_SkillBarSlotMax );
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		SkillBarSlotType slotType = (SkillBarSlotType)pp[i].type;
		if( slotType == ESBarSlotTp_Skill ){
			//技能由其他服务器消息来刷新
			continue;
		}

		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type == ESBarSlotTp_Skill ){
			//技能的slot不在这里同步，走技能同步(为了防止两边数据不一样造成技能怎么也装配不上slot，不一样是不对的)
			warnf(NAME_Error, TEXT("======!!!!======= [RefreshSkillBarByServerData server slot[%d] is different from local.]"), i);
			continue;
		}

		//
		INT slotContentID = pp[i].id;
		if( slotType == ESBarSlotTp_MAX ){
			//hack，用作不绑定特定位置的物品快捷键
			//先强制写入数据，便于SetBarSlot中能取到数据
			slotData.Type = ESBarSlotTp_Item;
			slotData.AidID = slotContentID;
			slotData.ID = SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID;
			//
			slotType = ESBarSlotTp_Item;
			slotContentID = SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID;
		}

		SetBarSlot( i, slotType, slotContentID );
	}
}

void UMMOUISkillBar::Send_BarSlotData()
{
	CONST INT buf_size = ( ECVT_SkillBarSlotEnd - ECVT_SkillBarSlotBegin + 1 ) << 2;
	BYTE buf[buf_size] = {0};
	check( buf_size >= sizeof( FSKILL_BAR_SLOT ) * UCONST_SkillBarSlotMax );
	FSKILL_BAR_SLOT* psbs = ( FSKILL_BAR_SLOT* )buf;
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		psbs[i].type = SkillBarSlot[i].Type;
		psbs[i].id = SkillBarSlot[i].ID;
		if( psbs[i].type == ESBarSlotTp_Item && psbs[i].id == -1 && SkillBarSlot[i].AidID > 0 ){
			//不绑定具体位置的物品
			psbs[i].type = ESBarSlotTp_MAX;
			psbs[i].id = SkillBarSlot[i].AidID;
		}
	}

	CMessageHandler::GetInstance().Send_UpdateClientVarRange( ECVT_SkillBarSlotBegin, ECVT_SkillBarSlotEnd, buf, sizeof(buf) );
	ClearnSlotDirty( -1 );
}
//===============================================================================================================
UBOOL UMMOUISkillBar::IsSlotDirty( INT idx )
{
	if( 0 <= idx && idx < UCONST_SkillBarSlotMax ){
		return ( ( IsBarSlotDataNeedUpload & ( 1 << idx ) ) > 0 );
	}
	else{
		for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
		{
			if( ( IsBarSlotDataNeedUpload & ( 1 << i ) ) > 0 ){
				return TRUE;
			}
		}
	}

	return FALSE;
}
//------------------------------------------------------------------
void UMMOUISkillBar::MarkSlotDirty( INT idx )
{
	if( idx < 0 || UCONST_SkillBarSlotMax <= idx ){
		IsBarSlotDataNeedUpload = 0xffffffff;
	}
	else{
		IsBarSlotDataNeedUpload |= 1 << idx;
	}
}
//------------------------------------------------------------------
void UMMOUISkillBar::ClearnSlotDirty( INT idx )
{
	if( idx < 0 || UCONST_SkillBarSlotMax <= idx ){
		IsBarSlotDataNeedUpload = 0;
	}
	else{
		IsBarSlotDataNeedUpload &= ~( 1 << idx );
	}
}
//===============================================================================================================
UBOOL UMMOUISkillBar::TryToEquipOnSkill( INT toSetBarSlotIdx, INT uniqueSkillID )
{
	UBOOL isSuccessed = FALSE;
	ECommandKey cmdKey = GetSkillBarSlotCmdKey( toSetBarSlotIdx );//MMOSkillManager::getSingleton().GetSkillbarSlotRealCmd( toSetBarSlotIdx );

	if( uniqueSkillID == 0 ){
		//不装备技能
		MMOSkillManager::getSingleton().SendEquipSpell( cmdKey, 0 );

		isSuccessed = TRUE;
		return isSuccessed;
	}

	//
	TMap< INT, class UMMOAbility* > myLearnedMainSkills = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();
	UMMOAbility *skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( skill == NULL ){
		UINT skillID;
		UINT skillLv;
		UINT skillRuneID;
		MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );
		warnf(NAME_Error, TEXT("======!!!!======= [TryToEquipOnSkill [ID:%d,LV:%d,RuneID:%d] To slot[%d] is NULL]"), skillID, skillLv, skillRuneID, toSetBarSlotIdx );
		return isSuccessed;
	}

	INT skillID = skill->eventGetAbilityId();
	UBOOL isDisabled = ( myLearnedMainSkills.Find( skillID ) == NULL );
	if( isDisabled ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillSetting.Error_Skill_Not_Learned") ) ,MsgInfoType_Table,TEXT("MMOGame.SkillSetting.Error_Skill_Not_Learned") );
		return isSuccessed;
	}

	//设置主技能
	MMOSkillManager::getSingleton().SendEquipSpell( cmdKey, skillID );

	//清理新技能列表
	MMOSkillManager::getSingleton().RemoveNewLearnedSkill( skillID );

	isSuccessed = TRUE;
	return isSuccessed;
}
//===============================================================================================================
void UMMOUISkillBar::ResetAllSkillSlots()
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		ECommandKey cmdKey = GetSkillBarSlotCmdKey( i );
		UMMOAbility* curEquippedSkill = mmoSkillMgr.GetEquippedSkill( cmdKey );
		if( curEquippedSkill == NULL ){
			continue;
		}
		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( curEquippedSkill->eventGetAbilityId(), curEquippedSkill->SpellCommonAttribute.Level, curEquippedSkill->GetRuneSlot() );

		SetBarSlotToSkillData( i, uniqueSkillID );
	}
}
//===============================================================================================================
UBOOL UMMOUISkillBar::UnequipSkill( INT uniqueSkillID, INT exceptIdx /*= -1*/ )
{
	UBOOL isSuccess = FALSE;
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		if( i == exceptIdx ){
			continue;
		}

		FSlotContainer &slotData = SkillBarSlotData( i );
		if (slotData.Type == ESBarSlotTp_Skill && MMOSkillManager::IsSameSeriesAbility( slotData.ID, uniqueSkillID ) ){
			TryToEquipOnSkill( i, 0 );
			//正确的逻辑来讲，只可能找到一个
			isSuccess = TRUE;
			//break;
		}
	}

	return isSuccess;
}
//---------------------------------------------------------------------------------------
UBOOL UMMOUISkillBar::UnequipAllSkills( INT skillID, UBOOL isClient/* = FALSE*/ )
{
	UBOOL isSuccess = FALSE;
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Skill ){
			continue;
		}
		UINT curSlotSkillID = 0;
		UINT curSlotSkillLv = 0;
		UINT curSlotSkillRuneID = 0;
		MMOSkillManager::DecomposeAbilityID( slotData.ID, curSlotSkillID, curSlotSkillLv, curSlotSkillRuneID );
		if( skillID == curSlotSkillID ){
			if( isClient ){
				SetBarSlotToNoData( i, 0 );
			}
			else{
				TryToEquipOnSkill( i, 0 );
			}

			//正确的逻辑来讲，只可能找到一个
			isSuccess = TRUE;
			break;
		}
	}

	//上传
	if( IsSlotDirty( -1 ) ){
		Send_BarSlotData();
	}

	return isSuccess;
}
//===============================================================================================================
void UMMOUISkillBar::UpdateSkillSlot( INT slotCmdKey )
{
	if( slotCmdKey < 0 || slotCmdKey >= ECK_Count ){
		warnf( NAME_Error, TEXT("UpdateSkillSlot slotCmdKey[%d] overflow"), slotCmdKey );
		return;
	}
	AMMOPlayerController *pMainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if( pMainPlayerCtrl == NULL ){
		return;
	}

	INT slotIndex = GetSkillBarSlotIdx( slotCmdKey );

	if( slotIndex < 0 || Player == NULL ){
		return;
	}

	UUIKeyMapManager* keyMapMgr = pMainPlayerCtrl->KeyMapManager;
	if( !keyMapMgr ){
		return;
	}

	SetAllSlotUpdateType();

	INT tAbilityId = keyMapMgr->GetCmdIDBySlotID( slotCmdKey );
	UMMOAbility* pAbility = Player->GetEquipedAbilityById( tAbilityId );
	if( pAbility != NULL ){
		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( tAbilityId, pAbility->SpellCommonAttribute.Level, pAbility->GetRuneSlot() );
		SetBarSlotToSkillData( slotIndex, uniqueSkillID );
	}
	else{
		FSlotContainer &origSlotData = SkillBarSlotData( slotIndex );
		if( origSlotData.Type == ESBarSlotTp_Skill ){
			//只清除技能数据
			SetBarSlotToNoData( slotIndex );
		}
	}
}
//==============================================================================================================
//@Desc：技能设置只会 发通知给服务器，实际在UpdateSkillSlot中设置
UBOOL UMMOUISkillBar::SetBarSlot( INT slotIdx, SkillBarSlotType slotType, INT templeteID )
{
	UBOOL isSuccess = FALSE;

	FSlotContainer &origSlotData = SkillBarSlotData( slotIdx );
	if( origSlotData.Type == ESBarSlotTp_Skill ){
		if( !( slotType == ESBarSlotTp_Skill && MMOSkillManager::getSingleton().IsSameSeriesAbility( templeteID, origSlotData.ID ) ) ){
			//技能装配需要另外再走一条消息,原来装配的技能需要通知服务器取消
			TryToEquipOnSkill( slotIdx, 0 );
		}
	}

	switch( slotType )
	{
	case ESBarSlotTp_Item:
		{
			//INT bagSlotID = -1;
			//FClientItemInfo *slotItemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer( templeteID, SKILL_BAR_ONLY_ITEM_CONTAINER, bagSlotID );
			//INT bagSlotID = MMOFacade::getSingleton().FindFirstItemPosInContainer();templeteID;
			isSuccess = SetBarSlotToItemData( slotIdx, templeteID );
		}
		break;
	//case ESBarSlotTp_MAX:
	//	{
	//		//hack，用作不绑定特定位置的物品快捷键
	//		FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	//		slotData.Type = ESBarSlotTp_Item;
	//		slotData.AidID = templeteID;
	//		isSuccess = SetBarSlotToItemData( slotIdx, templeteID );
	//	}
	//	break;
	case ESBarSlotTp_Mount:
		{
			isSuccess = SetBarSlotToMountData( slotIdx, templeteID );
		}
		break;
	case ESBarSlotTp_Skill:
		{
			//isSuccess = SetBarSlotToSkillData( slotIdx, templeteID );
			isSuccess = TryToEquipOnSkill( slotIdx, templeteID );
		}
		break;
	case ESBarSlotTp_Invalid:
	default:
		{
			isSuccess = SetBarSlotToNoData( slotIdx );
		}
		break;
	}

	return isSuccess;
}
//==============================================================================================================
UBOOL UMMOUISkillBar::SetBarSlotToSkillData( INT slotIdx, UINT uniqueSkillID )
{
	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	UBOOL isSetSuccessful = FALSE;
	UGFxObject *pSlotMc = slotData.SlotObject;
	if( pSlotMc == NULL ){
		return isSetSuccessful;
	}

	UGFxObject *pSlotData = pSlotMc->GetObject( TEXT("SlotData") );
	if( pSlotData != NULL && pSlotData->GetString( TEXT("SlotRenderClassName") ) != TEXT("SkillSlotRender") ){
		pSlotData = NULL;
	}

	if( pSlotData == NULL ){
		pSlotData = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.SkillSlotData"));
		if(pSlotData){
			pSlotData->SetString(TEXT("SlotRenderClassName"),  TEXT("SkillSlotRender"));
		}
	}

	if( pSlotData == NULL ){
		return isSetSuccessful;
	}

	//
	UMMOAbility *pAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( pAbility != NULL ){
		INT skillID = pAbility->eventGetAbilityId();
		pSlotData->SetInt( TEXT("slotType"), ESPType_SkillBar );
		pSlotData->SetInt( TEXT("slotIndex"), slotIdx );

		pSlotData->SetBool( TEXT("HaveData"), TRUE );
		pSlotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Skill );
		
		FString iconName = TEXT("img://");
		FString curSkillIcon = MMOSkillManager::getSingleton().GetSkillSlotIcon( uniqueSkillID );
		iconName += curSkillIcon.IsEmpty() ? pAbility->GetAbilityIconName() : curSkillIcon;
		pSlotData->SetString( TEXT("iconName"), iconName );
		pSlotData->SetInt( TEXT("itemTemplateID"), uniqueSkillID );
		//pSlotData->SetInt( TEXT("SkillLv"), item->ItemColor );

		//GFXPlayer->eventCallSetItemData( pSlotMc, 0, pAbility->GetAbilityIconName(), skillID, ESPType_ItemBar, TEXT("") );
		//pSlotMc->SetInt( TEXT("EquipID"), ESBarSlotTp_Skill );
		isSetSuccessful = TRUE;
	}
	else{
		SetBarSlotToNoData( slotIdx );

		isSetSuccessful = FALSE;
		return isSetSuccessful;
	}
	pSlotMc->SetObject( TEXT("SlotData"), pSlotData );

	TArray<struct FASValue> asArgs;
	pSlotMc->Invoke(TEXT("UpdateData"), asArgs);

	if( isSetSuccessful ){
		//先清除原有的,技能栏上不支持一个技能放多个栏位
		ClearAllSlotOfSkill( uniqueSkillID, slotIdx );

		slotData.Type = ESBarSlotTp_Skill;
		slotData.ID = uniqueSkillID;
		MarkSlotDirty( slotIdx );

		//刷新灰态
		CheckSlotEnableState( slotIdx, TRUE );

		//刷新CD
		ECommandKey cmdKey = GetSkillBarSlotCmdKey( slotIdx );
		UpdateAbilityCDInSlot( pAbility, cmdKey );

		//某些特效
		CheckSlotLoopEffect( slotIdx );
	}

	return isSetSuccessful;
}
//===============================================================================================================
UBOOL UMMOUISkillBar::SetBarSlotToItemData( INT slotIdx, INT itemPosID )
{
	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	UBOOL isSetSuccessful = FALSE;
	UGFxObject *pSlotMc = slotData.SlotObject;
	if( pSlotMc == NULL ){
		return isSetSuccessful;
	}

	UGFxObject *pSlotData = pSlotMc->GetObject( TEXT("SlotData") );
	if( pSlotData != NULL && pSlotData->GetString( TEXT("SlotRenderClassName") ) != TEXT("NormalItemSlotRender") ){
		pSlotData = NULL;
	}

	if( pSlotData == NULL ){
		pSlotData = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));
		if(pSlotData){
			pSlotData->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
		}
	}

	if( pSlotData == NULL ){
		return isSetSuccessful;
	}

	INT itemTemplateID = -1;
	INT containerIdx = -1;
	INT bagSlotIdx = -1;
	MMOEquipManager::DecomposeItemPosID( (UINT)itemPosID, containerIdx, bagSlotIdx );

	FClientItemInfo *pItemInfo = NULL;
	
	if( itemPosID != SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID ){
		if( containerIdx != SKILL_BAR_ONLY_ITEM_CONTAINER ){
			warnf( NAME_Error, TEXT("SetBarSlotToItemData by item POS:[%d] not in bag"), itemPosID );
			return isSetSuccessful;
		}
		pItemInfo = MMOFacade::getSingleton().FindItem( containerIdx, bagSlotIdx );
	}

	if( pItemInfo == NULL ){
		if( slotData.Type == ESBarSlotTp_Item ){
			//用原来的ID
			itemTemplateID = slotData.AidID;
		}
	}
	else{
		itemTemplateID = pItemInfo->m_nTemplateID;
	}

	UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( itemTemplateID );
	UBOOL canUse = MMOEquipManager::getSingleton().IsItemUsableOnSkillBar( pItem );
	if( !canUse ){
		return isSetSuccessful;
	}

	if( pItem != NULL ){
		pSlotData->SetInt( TEXT("slotType"), ESPType_SkillBar );
		pSlotData->SetInt( TEXT("slotIndex"), slotIdx );

		pSlotData->SetBool( TEXT("HaveData"), TRUE );
		pSlotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Item );

		INT count = 1;
		
		if( canUse ){
			count = MMOFacade::getSingleton().GetItemCountInBag( pItem->GetTemplateID(), E_CONSLOT_DEFAULT );
			pSlotData->SetInt( TEXT("quantity"), count );

			SetSlotDisableState( slotData.State, ESBarSlotUpdTp_NoData, count > 0 ? FALSE : TRUE );

			pSlotData->SetInt( TEXT("maxStackNum"), pItem->MaxStackNum );
		}
		else{
			pSlotData->SetInt( TEXT("quantity"), 0 );
			//pSlotData->SetInt( TEXT("IsGray"), pItemInfo == NULL );
			SetSlotDisableState( slotData.State, ESBarSlotUpdTp_NoData, pItemInfo != NULL ? FALSE : TRUE );
			pSlotData->SetInt( TEXT("maxStackNum"), 1 );
		}


		FString cs = FString(TEXT("img://")) + MMOFacade::getSingleton().GetItemIconNamePersonalized( pItem, pItemInfo );
		pSlotData->SetString( TEXT("iconName"), cs );
		pSlotData->SetInt( TEXT("containerID"), bagSlotIdx );//SKILL_BAR_ONLY_ITEM_CONTAINER );
		//pSlotData->SetInt( TEXT("slotID"), bagSlotIdx );
		pSlotData->SetInt( TEXT("itemTemplateID"), itemTemplateID );
		pSlotData->SetInt( TEXT("itemColor"), pItem->ItemColor );

		isSetSuccessful = TRUE;
		//KeyMap->BindingSlot( ECK_F1 + slotIdx , UICT_UseItem , item->GetTemplateID() );
	}
	else{
		SetBarSlotToNoData( slotIdx );

		isSetSuccessful = FALSE;
		return isSetSuccessful;
	}

	pSlotMc->SetObject( TEXT("SlotData"), pSlotData );

	TArray<struct FASValue> asArgs;
	pSlotMc->Invoke(TEXT("UpdateData"), asArgs);

	//
	if( isSetSuccessful ){
		slotData.Type = ESBarSlotTp_Item;
		slotData.ID = itemPosID;
		slotData.AidID = itemTemplateID;
		//上传服务器
		//Send_BarSlotData();
		MarkSlotDirty( slotIdx );

		//刷新灰态
		CheckSlotEnableState( slotIdx, TRUE );

		//刷新CD
		SetItemCDInSlot( slotIdx );

		//某些特效
		CheckSlotLoopEffect( slotIdx );
	}

	return isSetSuccessful;
}
//===============================================================================================================
UBOOL UMMOUISkillBar::SetBarSlotToMountData( INT slotIdx, INT mountID )
{
	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	UBOOL isSetSuccessful = FALSE;
	UGFxObject *pSlotMc = slotData.SlotObject;
	if( pSlotMc == NULL ){
		return isSetSuccessful;
	}

	UGFxObject *pSlotData = pSlotMc->GetObject( TEXT("SlotData") );
	if( pSlotData != NULL && pSlotData->GetString( TEXT("SlotRenderClassName") ) != TEXT("MountSlotRender") ){
		pSlotData = NULL;
	}

	if( pSlotData == NULL ){
		pSlotData = GFXPlayer->CreateObject(TEXT("ActionScript3.Mount.MountSlotData"));
		if(pSlotData){
			pSlotData->SetString(TEXT("SlotRenderClassName"),  TEXT("MountSlotRender"));
		}
	}

	if( pSlotData == NULL ){
		return isSetSuccessful;
	}

	MountData* pMount = MMOFacade::getSingleton().GetMountData( mountID );
	if( pMount != NULL ){
		pSlotData->SetInt( TEXT("slotType"), ESPType_SkillBar );
		pSlotData->SetInt( TEXT("slotIndex"), slotIdx );

		pSlotData->SetBool( TEXT("HaveData"), TRUE );
		pSlotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Mount );

		//INT count = MMOFacade::getSingleton().GetItemCountInBag( item->GetTemplateID() );
		pSlotData->SetInt( TEXT("quantity"), 0 );
		FString cs = FString(TEXT("img://")) + pMount->IconName;
		pSlotData->SetString( TEXT("iconName"), cs );
		pSlotData->SetInt( TEXT("itemTemplateID"), pMount->ID );
		pSlotData->SetString( TEXT("tipsHtml"), pMount->Script );

		pSlotData->SetInt( TEXT("IsEnable"), 1 );
		pSlotData->SetInt( TEXT("classesLevel"), pMount->LevelLimit );
		pSlotData->SetInt( TEXT("classesQuality"), pMount->Quality );
		pSlotData->SetInt( TEXT("itemColor"), pMount->Quality );

		isSetSuccessful = TRUE;
	}
	else{
		SetBarSlotToNoData( slotIdx );

		isSetSuccessful = FALSE;
		return isSetSuccessful;
	}

	pSlotMc->SetObject( TEXT("SlotData"), pSlotData );

	TArray<struct FASValue> asArgs;
	pSlotMc->Invoke( TEXT("UpdateData"), asArgs );

	if( isSetSuccessful ){
		slotData.Type = ESBarSlotTp_Mount;
		slotData.ID = mountID;

		//上传服务器
		//Send_BarSlotData();
		MarkSlotDirty( slotIdx );

		//刷新灰态
		CheckSlotEnableState( slotIdx, TRUE );

		//取消CD
		SetItemSlotCD( pSlotMc, 0, 0 );

		//某些特效
		CheckSlotLoopEffect( slotIdx );
	}

	return isSetSuccessful;
}
//
UBOOL UMMOUISkillBar::SetBarSlotToNoData( INT slotIdx, UBOOL isInitialize /*= FALSE*/ )
{
	FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	UBOOL isSetSuccessful = FALSE;
	UGFxObject *pSlotMc = slotData.SlotObject;
	if( pSlotMc == NULL ){
		return isSetSuccessful;
	}

	UGFxObject *pSlotData = pSlotMc->GetObject( TEXT("SlotData") );
	if( pSlotData == NULL ){
		pSlotData = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.SkillSlotData"));
		if(pSlotData){
			pSlotData->SetString(TEXT("SlotRenderClassName"),  TEXT("SkillSlotRender"));
		}
	}

	if( pSlotData != NULL ){
		pSlotData->SetInt( TEXT("slotType"), ESPType_SkillBar );
		pSlotData->SetInt( TEXT("slotIndex"), slotIdx );

		pSlotData->SetBool( TEXT("HaveData"), FALSE );
		pSlotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Invalid );
		pSlotData->SetString( TEXT("iconName"), TEXT("") );
		pSlotData->SetString( TEXT("tipsHtml"), TEXT("") );
		pSlotData->SetInt( TEXT("itemTemplateID"), 0 );

		pSlotMc->SetObject( TEXT("SlotData"), pSlotData );

		TArray<struct FASValue> asArgs;
		pSlotMc->Invoke( TEXT("UpdateData"), asArgs );

		isSetSuccessful = TRUE;
	}
	else{
		isSetSuccessful = FALSE;
	}
	
	if( slotData.Type != ESBarSlotTp_Invalid ){
		//if( slotData.Type == ESBarSlotTp_Skill ){
		//	//技能需要另外通知一条消息
		//	TryToEquipOnSkill( slotIdx, 0 );
		//}
		slotData.Type = ESBarSlotTp_Invalid;
		slotData.ID = -1;
		slotData.AidID = -1;

		if( isInitialize == FALSE ){
			MarkSlotDirty( slotIdx );
		}

		//刷新灰态
		CheckSlotEnableState( slotIdx, TRUE );

		//取消CD
		SetItemSlotCD( pSlotMc, 0, 0 );

		//某些特效
		CheckSlotLoopEffect( slotIdx );
	}

	return isSetSuccessful;
}
//==============================================================================================================
void  UMMOUISkillBar::SwapBarSlotData( INT slotIdx1, INT slotIdx2 )
{
	if( slotIdx1 < 0 || UCONST_SkillBarSlotMax <= slotIdx1 ){
		return;
	}
	if( slotIdx2 < 0 || UCONST_SkillBarSlotMax <= slotIdx2 ){
		return;
	}

	FSlotContainer &slot1Data = SkillBarSlotData( slotIdx1 );
	FSlotContainer &slot2Data = SkillBarSlotData( slotIdx2 );

	INT slot1Type = slot1Data.Type;
	//INT slot1IDtoSet = ( slot1Type == ESBarSlotTp_Item ? slot1Data.AidID : slot1Data.ID );
	INT slot1IDtoSet = slot1Data.ID;

	INT slot2Type = slot2Data.Type;
	//INT slot2IDtoSet = ( slot2Type == ESBarSlotTp_Item ? slot2Data.AidID : slot2Data.ID );
	INT slot2IDtoSet = slot2Data.ID;

	if( ( slot1Type == ESBarSlotTp_Item && slot1IDtoSet == -1 ) || ( slot2Type == ESBarSlotTp_Item && slot2IDtoSet== -1 ) ){
		//有物品用完后，做快捷键交换的情况
		if( slot1Type == ESBarSlotTp_Skill ){
			//后面会直接交换数据，这里得手动清理技能
			TryToEquipOnSkill( slotIdx1, 0 );
		}
		if( slot2Type == ESBarSlotTp_Skill ){
			TryToEquipOnSkill( slotIdx2, 0 );
		}
		//物品如果此时没在背包内，无法完成交换快捷键，这里需要特殊处理下，强制数据交换，让SetBarSlot去完成数据刷新
		SwapBarSlotDataRaw( slotIdx1, slotIdx2 );
	}
	else{
		if( slot2Type == ESBarSlotTp_Skill ){
			//slot1会在SetBarSlot（slotIdx1....）自动判定卸除slot1的技能
			TryToEquipOnSkill( slotIdx2, 0 );
		}
	}
	
	SetBarSlot( slotIdx1, (SkillBarSlotType)slot2Type, slot2IDtoSet );

	SetBarSlot( slotIdx2, (SkillBarSlotType)slot1Type, slot1IDtoSet );

	////技能需处理技能装配
	//FSlotContainer &newSlot1Data = SkillBarSlotData( slotIdx1 );
	//FSlotContainer &newSlot2Data = SkillBarSlotData( slotIdx2 );
	//if( newSlot1Data.Type == ESBarSlotTp_Skill && newSlot2Data.Type == ESBarSlotTp_Skill ){
	//	TryToEquipOnSkill( slotIdx1, newSlot1Data.ID );
	//	TryToEquipOnSkill( slotIdx2, newSlot2Data.ID );
	//}
	//if( newSlot1Data.Type == ESBarSlotTp_Skill && newSlot2Data.Type != ESBarSlotTp_Skill ){
	//	TryToEquipOnSkill( slotIdx1, newSlot1Data.ID );
	//	TryToEquipOnSkill( slotIdx2, 0 );
	//}
	//if( newSlot1Data.Type != ESBarSlotTp_Skill && newSlot2Data.Type == ESBarSlotTp_Skill ){
	//	TryToEquipOnSkill( slotIdx1, 0 );
	//	TryToEquipOnSkill( slotIdx2, newSlot2Data.ID );
	//}

}
//==============================================================================================================
void UMMOUISkillBar::SwapBarSlotDataRaw( INT slotIdx1, INT slotIdx2 )
{
	FSlotContainer &slot1Data = SkillBarSlotData( slotIdx1 );
	FSlotContainer &slot2Data = SkillBarSlotData( slotIdx2 );

	//
	FSlotContainer tempData;
	tempData.ID = slot1Data.ID;
	tempData.AidID = slot1Data.AidID;
	tempData.Type = slot1Data.Type;

	//
	slot1Data.ID = slot2Data.ID;
	slot1Data.AidID = slot2Data.AidID;
	slot1Data.Type = slot2Data.Type;
	MarkSlotDirty( slotIdx1 );

	//
	slot2Data.ID = tempData.ID;
	slot2Data.AidID = tempData.AidID;
	slot2Data.Type = tempData.Type;
	MarkSlotDirty( slotIdx2 );
}

//==============================================================================================================
void UMMOUISkillBar::ClearAllSlotOfItem( INT itemTemplateID )
{
	for ( INT i = 0 ; i < UCONST_SkillBarSlotMax; ++i )
	{
		if( SkillBarSlot[i].Type == ESBarSlotTp_Item && SkillBarSlot[i].AidID == itemTemplateID ){
			SetBarSlotToNoData( i );
		}
	}
}
//==============================================================================================================
void UMMOUISkillBar::ClearAllSlotOfSkill( INT uniqueSkillID, INT exceptSlotIdx /*= -1*/ )
{
	for ( INT i = 0 ; i < UCONST_SkillBarSlotMax; ++i )
	{
		if( exceptSlotIdx == i ){
			continue;
		}

		if( SkillBarSlot[i].Type == ESBarSlotTp_Skill && MMOSkillManager::IsSameSeriesAbility( SkillBarSlot[i].ID, uniqueSkillID ) ){
			SetBarSlotToNoData( i );
		}
	}
}
//==============================================================================================================
void UMMOUISkillBar::CheckSlotEnableState( INT slotIdx, UBOOL isForceCheck /*= FALSE*/ )
{
	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		return;
	}

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	switch( slotData.Type )
	{
	case ESBarSlotTp_Item:
		{
			CheckItemEnableState( slotIdx, isForceCheck );
		}
		break;
	case ESBarSlotTp_Mount:
		{
			//总是可操作的
			SetSkillBarSlotDisableState( slotIdx, 0 );
		}
		break;
	case ESBarSlotTp_Skill:
		{
			CheckAbilityEnableState( slotIdx, isForceCheck );
		}
		break;
	case ESBarSlotTp_Invalid:
	default:
		{
			//总是可操作的
			SetSkillBarSlotDisableState( slotIdx, 0 );
		}
		break;
	}
}
//==============================================================================================================
//更新bar上所有放置了skill的slot可操作状态
void UMMOUISkillBar::UpdateAbilityEnableState()
{
	for ( INT i = 0 ; i < UCONST_SkillBarSlotMax; i++ )
	{
		CheckAbilityEnableState( i, NeedRefreshSkillSlotDisable );
	}

	ResetSlotUpdateType();
}
//------------------------------------------------------------------
void UMMOUISkillBar::CheckAbilityEnableState( INT slotIdx, UBOOL isForceCheck /*= FALSE*/ )
{
	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		return;
	}

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	if( slotData.Type != ESBarSlotTp_Skill ){
		return;
	}

	INT curSlotDisStat;
	if( isForceCheck ){
		curSlotDisStat = 0;
	}
	else{
		curSlotDisStat = GetSkillBarSlotDisableState( slotIdx );
	}

	INT origSlotDisStat = curSlotDisStat;

	UMMOAbility* tAbility = GetSkillBarSlotSkill( slotIdx );
	if( tAbility )
	{
		//能量检测
		if( IsSlotUpdateType( ESBarSlotUpdTp_Mana ) || isForceCheck ) 
		{
			UBOOL unfit = false;

			unfit = !tAbility->CanUseManaCheck(Player);
			SetSlotDisableState( curSlotDisStat, ESBarSlotUpdTp_Mana, unfit );

			//UnsetSlotUpdateType( ESBarSlotUpdTp_Mana );
		}

		//检测右手武器
		if( IsSlotUpdateType( ESBarSlotUpdTp_WeaponType ) || isForceCheck ) 
		{
			UBOOL isWeaponUnFit = FALSE;
			UEquip* equip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( E_CONSLOT_EQUIPPED , E_NES_MAIN_HAND ) );
			INT weaponTypeMask = 1;
			if(equip && equip->GetEquipAppearenceTemplate())
			{
				weaponTypeMask = 1 << equip->GetEquipAppearenceTemplate()->WeaponType;
			}

			INT& abilityWeaponType = (int&)(tAbility->SpellCommonAttribute.WeaponTypeR);
			if( ( weaponTypeMask & abilityWeaponType ) != weaponTypeMask )
			{
				isWeaponUnFit = TRUE;	
			}

			SetSlotDisableState( curSlotDisStat, ESBarSlotUpdTp_WeaponType, isWeaponUnFit );
			//UnsetSlotUpdateType( ESBarSlotUpdTp_WeaponType );
		}

		//技能状态检测
		if( IsSlotUpdateType( ESBarSlotUpdTp_Buff ) || isForceCheck ) 
		{
			UBOOL isBuffDisabled = !MMOFacade::getSingleton().IsAbilityCanUse( tAbility );

			SetSlotDisableState( curSlotDisStat, ESBarSlotUpdTp_Buff, isBuffDisabled );
			//UnsetSlotUpdateType( ESBarSlotUpdTp_Buff );
		}
	}

	if( curSlotDisStat != origSlotDisStat || isForceCheck == TRUE )
	{
		SetSkillBarSlotDisableState( slotIdx, curSlotDisStat );
	}
}
//------------------------------------------------------------------
//获得制定slot上的技能，如果有的话(现在只是放了技能的slot)
UMMOAbility *UMMOUISkillBar::GetSkillBarSlotSkill( INT slotIdx )
{
	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		return NULL;
	}

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	if( slotData.Type == ESBarSlotTp_Skill ){
		ECommandKey cmdKey = GetSkillBarSlotCmdKey( slotIdx );
		return MMOSkillManager::getSingleton().GetEquippedSkill( cmdKey, TRUE );
	}

	if( slotData.Type == ESBarSlotTp_Item ){
		return NULL;
		//INT itemConID = E_CONSLOT_DEFAULT;
		//INT itemSlotID = -1;
		//FClientItemInfo *putItemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer( slotData.ID, conID, slotID );
		//if( putItemInfo != NULL ){
		//}
	}

	return NULL;
}
//==============================================================================================================
//更新bar上所有放置了item的slot可操作状态
void UMMOUISkillBar::UpdateItemEnableState()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	for ( INT i = 0 ; i < UCONST_SkillBarSlotMax; i++ )
	{
		CheckItemEnableState( i, FALSE );
	}
}
//------------------------------------------------------------------
void UMMOUISkillBar::CheckItemEnableState( INT slotIdx, UBOOL isForceCheck /*= FALSE*/ )
{
	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		return;
	}

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	if( slotData.Type != ESBarSlotTp_Item ){
		return;
	}

	INT curSlotDisStat;
	if( isForceCheck ){
		curSlotDisStat = 0;
	}
	else{
		curSlotDisStat = slotData.State;
	}

	INT origSlotDisStat = curSlotDisStat;

	INT curBarItemConIdx = -1;
	INT curBarItemSlotIdx = -1;
	MMOEquipManager::DecomposeItemPosID( (UINT)slotData.ID, curBarItemConIdx, curBarItemSlotIdx );
	FClientItemInfo *slotItemInfo = MMOFacade::getSingleton().FindItem( curBarItemConIdx, curBarItemSlotIdx );

	INT itemTempletID = slotData.AidID;
	UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( itemTempletID );
	
	if( pItem != NULL ){
		UBOOL canUse = MMOEquipManager::getSingleton().IsItemUsableOnSkillBar( pItem );
		if( canUse ){
			INT count = MMOFacade::getSingleton().GetItemCountInBag( pItem->GetTemplateID(), E_CONSLOT_DEFAULT );
			SetSlotDisableState( curSlotDisStat, ESBarSlotUpdTp_NoData, count > 0 ? FALSE : TRUE );
		}
		else{
			SetSlotDisableState( curSlotDisStat, ESBarSlotUpdTp_NoData, slotItemInfo != NULL ? FALSE : TRUE );
		}
	}
	else{
		SetSlotDisableState( curSlotDisStat, ESBarSlotUpdTp_NoData, TRUE );
	}

	if( curSlotDisStat != origSlotDisStat || isForceCheck ){
		SetSkillBarSlotDisableState( slotIdx, curSlotDisStat );
	}
}
//==============================================================================================================
void UMMOUISkillBar::CoverAllSlotUpdateTypeAndUpdate()
{
	SkillSlotUpdState = 0xffffffff;
	UpdateAbilityEnableState();
}
//==============================================================================================================
void UMMOUISkillBar::ClearUI()
{
	Super::ClearUI();

	NeedRefreshSkillSlotDisable = TRUE;
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		SetBarSlotToNoData( i, TRUE );
	}
	//
	IsBarTypeUpdated = FALSE;
	//
	CloseHintForAllDragging();
	//
	ClearSkillListPanel();
}
//------------------------------------------------------------------
//清空所有slot数据并上传服务器
void UMMOUISkillBar::ClearAllSkillSlotData()
{
	//for(INT i = 0; i < UCONST_SkillBarSlotMax; ++i)
	//{
	//	SetBarSlotToNoData( i );
	//}
}
//------------------------------------------------------------------
void UMMOUISkillBar::ClearSkillListPanel()
{
	CurShowedSkillListSlotIdx = -1;
}
//==============================================================================================================
//技能栏放入默认物品
void UMMOUISkillBar::InitItemBarItemForNewComing()
{
	//新人物第一次进游戏
	const INT defaultSlot1 = 6;
	const INT defaultSlot2 = 11;
	if( !( SkillBarSlot[defaultSlot1].Type == ESBarSlotTp_Invalid && SkillBarSlot[defaultSlot2].Type == ESBarSlotTp_Invalid ) ) {
		return;
	}

	//
	INT itemContainIdx = SKILL_BAR_ONLY_ITEM_CONTAINER;
	INT itemSlotIdx = -1;
	FClientItemInfo *putItemInfo = NULL;
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	INT itemID = 0;
	INT inBagItemSlotIdx = -1;
	//药水
	GConfig->GetInt( TEXT("SkillBar"), TEXT("PuttedOnBarItem1"), itemID, GDefaultGameIni );

	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer( itemID, SKILL_BAR_ONLY_ITEM_CONTAINER, inBagItemSlotIdx );
	if( pItemInfo != NULL ){
		INT itemPosID = (INT)MMOEquipManager::CombinateItemPosID( SKILL_BAR_ONLY_ITEM_CONTAINER, inBagItemSlotIdx );
		SetBarSlot( defaultSlot1, ESBarSlotTp_Item, itemPosID );
	}

	////回城卷
	//GConfig->GetInt( TEXT("SkillBar"), TEXT("PuttedOnBarItem2"), itemID, GDefaultGameIni );
	//SetBarSlot( defaultSlot2, ESBarSlotTp_Item, itemID );

	//这里必须所有保证原先的数据本地已经有了，不然会被刷掉
	if( IsSlotDirty( -1 ) ){
		Send_BarSlotData();
	}
}
//------------------------------------------------------------------
//UI初始化
void UMMOUISkillBar::Init()
{
	Super::Init();
	SKILL_BAR_ONLY_ITEM_CONTAINER = E_CONSLOT_DEFAULT;

	InitMc();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MainPlayerDataChange);
	evts.AddItem(GameEvent_ChangeSkillSlot);
	evts.AddItem(GameEvent_EquipRuneSkill);
	evts.AddItem(GameEvent_SkillLevelUp);
	//evts.AddItem(GameEvent_ForgetSkill);
	evts.AddItem(GameEvent_EnterGameSuccess);
	evts.AddItem(GameEvent_FinishLoading);
	evts.AddItem(GameEvent_NewCD);
	evts.AddItem(GameEvent_Refresh_SkillSlotLoop);
	evts.AddItem(GameEvent_SkillUiUpdate);
	evts.AddItem(GameEvent_Refresh_SkillSlotIcon);
	evts.AddItem(GameEvent_ShowNewGlobe);
	evts.AddItem(GameEvent_SyncEquippedWeapon);
	evts.AddItem(GameEvent_Equip);
	evts.AddItem(GameEvent_ExpExtraChanged);
	evts.AddItem(GameEvent_ExpExtraInit);
	evts.AddItem(GameEvent_Sync_SkillEquippedSkills);
	evts.AddItem(GameEvent_Item_Delete);
	evts.AddItem(GameEvent_Item_SlotSwapped);
	evts.AddItem(GameEvent_SortItemsInBagSuccess);
	evts.AddItem(GameEvent_Item_SlotUpdated);
	EventAdapter = new TGameEventAdapter<UMMOUISkillBar>(this, evts);

	MMOUI::GetUIManager()->SetUpdateSkillBarKeyDownDelegate(FScriptDelegate(this, TEXT("UpdateSkillBarKeyDown")));
	MMOUI::GetUIManager()->SetFlashHPBarDelegate(FScriptDelegate(this, TEXT("FlashHPBar")));
	MMOUI::GetUIManager()->SetUpdatePetStateDelegate(FScriptDelegate(this, TEXT("UpdatePetState")));
	//
	MMOFacade::getSingleton().GetMyHud()->AddDrawHUD(this);
}
//------------------------------------------------------------------
void UMMOUISkillBar::InitMc()
{
	NewSkillHintBtn = GetObject( TEXT("NewSkillHintBtn") );
	if( NewSkillHintBtn != NULL ){
		NewSkillHintBtn->SetString( TEXT("label"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillBar.NewSkillHintBtnHint") ) );
	}

	//
	PlayerNameTf = GetObject( TEXT("PlayerName_mc") );
	PlayerProMc = GetObject( TEXT("playerProIcon") );
	HpBar = GetObject( TEXT("HpBar_mc") );
	MpBar = GetObject( TEXT("MpBar_mc") );

	//
	PlayerLevelTf = GetObject( TEXT( "leveltext" ) );
	PlayerPKBtn = GetObject( TEXT( "btnPkFlagOption" ) );
	//
	eventInitSlotMc();
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		SetBarSlotToNoData( i, TRUE );
	}
	//for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i ){
	//	UGFxObject *skillSlotMc = SkillBarSlotMc( i );
	//	if( skillSlotMc == NULL ){
	//		continue;
	//	}

	//	skillSlotMc->SetInt( TEXT("slotType"), ESPType_SkillBar );
	//	skillSlotMc->SetInt( TEXT("EquipID"), i );
	//	skillSlotMc->SetInt( TEXT("slotIndex"), i );
	//	skillSlotMc->SetInt( TEXT("itemid"), i );
	//	skillSlotMc->SetBool( TEXT("bDisableDrag"), FALSE );
	//}
}
//------------------------------------------------------------------
void UMMOUISkillBar::InitSkillBarSlotHotKey()
{
	UMMOKeyManager* ukm = UMMOKeyManager::getSingleton();
	if( ukm == NULL ){
		return;
	}

	FString keyname;
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i ){
		//FSlotContainer &slotData = SkillBarSlotData( slotIdx );
		UGFxObject *slotBg = GetObject( FString::Printf( TEXT("slot_bg_%d") , i ) );
		if( slotBg == NULL ){
			continue;
		}
		UGFxObject *keyBind = slotBg->GetObject( TEXT("keyBind") );
		if( keyBind == NULL ){
			continue;
		}

		UMMOKeyManager::KKeyworkd* pKKey = ukm->FindKeyworld( (UMMOKeyManager::EKeyFunction)( UMMOKeyManager::KF_BarSkill1+i ), FALSE );
		ukm->KeyToString( pKKey, keyname, 1 );

		keyBind->SetText( keyname );

		//if(pKKey && i<2)//只有前两个键有图标
		//{
		//	UGFxObject *keyBindImg = skillSlotMc->GetObject( TEXT("keyBindImg") );

		//	if(pKKey->code==VK_LBUTTON && keyBindImg)
		//	{							
		//		keyBindImg->GotoAndStop(TEXT("VK_LBUTTON"));
		//	}
		//	else if(pKKey->code==VK_RBUTTON && keyBindImg)
		//	{							
		//		keyBindImg->GotoAndStop(TEXT("VK_RBUTTON"));
		//	}
		//	else if(keyBindImg)
		//	{
		//		keyBindImg->GotoAndStopI(0);
		//	}
		//}
	}
}
//==============================================================================================================
void UMMOUISkillBar::UnInit()
{
	Super::UnInit();

	MMOUI::GetUIManager()->SetUpdateSkillBarKeyDownDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetFlashHPBarDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetUpdatePetStateDelegate(FScriptDelegate(EC_EventParm));
	MMOFacade::getSingleton().GetMyHud()->RemoveDrawHUD(this);
}

//==============================================================================================================
//技能栏slot待更新状态标记
UBOOL UMMOUISkillBar::IsSlotUpdateType( INT type )
{
	return ( SkillSlotUpdState & ( 1 << type ) );
}
//------------------------------------------------------------------
void UMMOUISkillBar::SetSlotUpdateType( INT type )
{
	SkillSlotUpdState |= ( 1 << type );
}
void UMMOUISkillBar::UnsetSlotUpdateType( INT type )
{
	SkillSlotUpdState &= ~( 1 << type );
}
//------------------------------------------------------------------
void UMMOUISkillBar::ResetSlotUpdateType()
{
	SkillSlotUpdState = 0;
}
void UMMOUISkillBar::SetAllSlotUpdateType()
{
	SkillSlotUpdState = 0xffffffff;
}
//==============================================================================================================
//记录slot的当前激活状态
void UMMOUISkillBar::SetSlotDisableState( INT &slotState, INT type, UBOOL isToSet )
{
	if( isToSet ){
		slotState |= ( 1 << type );
	}
	else{
		slotState &= ~( 1 << type );
	}
}
//==============================================================================================================
//技能栏上个slot数据
FSlotContainer &UMMOUISkillBar::SkillBarSlotData( INT idx )
{
	if( idx < 0 || UCONST_SkillBarSlotMax <= idx ){
		warnf( NAME_Error, TEXT("SkillBarSlotData [%d] overflow"), idx );
		return SkillBarSlot[0];
	}

	return SkillBarSlot[idx];
}
//------------------------------------------------------------------
INT UMMOUISkillBar::GetSkillBarSlotDisableState( INT idx )
{
	if( idx < 0 || UCONST_SkillBarSlotMax <= idx ){
		warnf( NAME_Error, TEXT("SkillBarSlotData [%d] overflow"), idx );
		return SkillBarSlot[0].State;
	}
	return SkillBarSlotData(idx).State;
}
void UMMOUISkillBar::SetSkillBarSlotDisableState( INT idx, INT toSetState )
{
	if( idx < 0 || UCONST_SkillBarSlotMax <= idx ){
		warnf( NAME_Error, TEXT("SetSkillBarSlotDisableState [%d] overflow"), idx );
		return;
	}
	
	FSlotContainer &slotData = SkillBarSlotData( idx );

	slotData.State = toSetState;
	eventCallAsToGrayMc( slotData.SlotObject, slotData.State != 0 );
	//eventCallAsToGraySlotIcon( slotData.SlotObject, slotData.State != 0 );//这种方式悬浮时有问题
}
//------------------------------------------------------------------
UGFxObject *UMMOUISkillBar::SkillBarSlotMc( INT idx )
{
	if( idx < 0 || UCONST_SkillBarSlotMax <= idx ){
		warnf( NAME_Error, TEXT("SkillBarSlotMc [%d] overflow"), idx );
		return NULL;
	}
	return SkillBarSlotData(idx).SlotObject;
}
//==============================================================================================================
//技能栏与技能slot编号的对应转换
INT UMMOUISkillBar::GetSkillBarSlotIdx( INT eCommandKey )
{
	switch( eCommandKey )
	{
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
	case ECK_NormalAtk: return -1;//普攻技能，不在Skillbar上
	}

	//warnf( NAME_Error, TEXT("GetSkillBarSlotIdx enum[%d] is not mapped on skill bar"), eCommandKey );
	return -1;
}

ECommandKey UMMOUISkillBar::GetSkillBarSlotCmdKey( INT slotIdx )
{
	switch( slotIdx )
	{
	case 0: return ECK_Slot1;
	case 1: return ECK_Slot2;
	case 2: return ECK_Slot3;
	case 3: return ECK_Slot4;
	case 4: return ECK_Slot5;
	case 5: return ECK_Slot6;
	case 6: return ECK_Slot7;
	case 7: return ECK_Slot8;
	case 8: return ECK_Slot9;
	case 9: return ECK_Slot10;
	case 10: return ECK_Slot11;
	case 11: return ECK_Slot12;
	}

	warnf( NAME_Error, TEXT("GetSkillBarSlotCmdKey index[%d] is overflow"), slotIdx );
	return ECK_Slot1;
}
//===============================================================================================================
//使用技能后的刷新(不支持栏上放置了多个相同技能)
void UMMOUISkillBar::SetAbilityCDInSlot( INT slotCmdKey, INT abilityTemplateID, INT leftMiniSec, INT maxMiniSec )
{
	return;//不要了
	//INT slotIdx = GetSkillBarSlotIdx( slotCmdKey );
	//if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
	//	if( slotCmdKey != ECK_NormalAtk ){
	//		warnf( NAME_Error, TEXT("SetAbilityCDInSlot SlotCmd[%d] overflow"), slotCmdKey );
	//	}
	//	
	//	return;
	//}
	//FSlotContainer &slotData = SkillBarSlotData( slotIdx );

	//if( slotData.Type != ESBarSlotTp_Skill ){
	//	warnf( NAME_Warning, TEXT("[UMMOUISkillBar::SetAbilityCDInSlot slot is NOT a skill slot]"), slotIdx );
	//	return;
	//}

	//INT curSlotAbilityID = slotData.ID;
	//UINT skillID;
	//UINT skillLv;
	//UINT skillRuneID;
	//MMOSkillManager::DecomposeAbilityID( curSlotAbilityID, skillID, skillLv, skillRuneID );
	//
	//if( skillID != abilityTemplateID ){
	//	warnf( NAME_Warning, TEXT("[UMMOUISkillBar::SetAbilityCDInSlot slot[%d] is NOT a skill[ID:%d,LV:%d,RuneID:%d] ]"), slotIdx, skillID, skillLv, skillRuneID );
	//	return;
	//}

	////
	//SetItemSlotCD( slotData.SlotObject, leftMiniSec, maxMiniSec );
}
//--------------------------------------------------------------------------
//刷新技能栏上此技能的CD时间
void UMMOUISkillBar::UpdateAbilityCDInSlot( UMMOAbility* inAbility, INT forceUpdateSlotCmdKey )
{
	if( inAbility == NULL ){
		return;
	}
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( inAbility->eventGetAbilityId(), inAbility->SpellCommonAttribute.Level, inAbility->GetRuneSlot() );

	INT updateSlotCmdKey = forceUpdateSlotCmdKey;
	if( GetSkillBarSlotIdx( updateSlotCmdKey ) < 0 ){
		updateSlotCmdKey = inAbility->SlotHotKeyID;
		if( updateSlotCmdKey < 0 ){
			//此技能没有装备
			warnf( NAME_Warning, TEXT("[UMMOUISkillBar::UpdateAbilityCDInSlot skill[%d] is not on skill bar]"), inAbility->eventGetAbilityId() );
			return;
		}
	}

	FLOAT leftSec = inAbility->CoolDownInfo.SpellTimeLeft;
	leftSec = ( leftSec < inAbility->CoolDownInfo.GroupTimeLeft ? inAbility->CoolDownInfo.GroupTimeLeft : leftSec );

	FLOAT totalSec = inAbility->CoolDownInfo.SpellTimeMax;
	totalSec = ( totalSec < inAbility->CoolDownInfo.GroupTimeMax ? inAbility->CoolDownInfo.GroupTimeMax : totalSec );
	//
	if( leftSec >= 0 ){
		SetAbilityCDInSlot( updateSlotCmdKey, uniqueSkillID, leftSec * 1000, totalSec * 1000 );
	}

}
//===============================================================================================================
//使用物品后的刷新
void UMMOUISkillBar::SetItemCDInSlot( INT slotIdx )
{
	if( slotIdx < 0 || UCONST_SkillBarSlotMax <= slotIdx ){
		warnf( NAME_Error, TEXT("SetItemCDInSlot [%d] overflow"), slotIdx );
		return;
	}

	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	if( slotData.Type != ESBarSlotTp_Item ){
		warnf( NAME_Warning, TEXT("[UMMOUISkillBar::SetItemCDInSlot slot is NOT a item slot]"), slotIdx );
		return;
	}

	UGFxObject *pSlot = slotData.SlotObject;
	UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( slotData.AidID );
	if( pItem == NULL ){
		//空格子
		StopItemSlotCD( pSlot );
		return;
	}

	UMMOAbility* pItemSkill = pItem->GetToUseAbility();
	if( pItem->ItemType == TEXT("ITEMTYPE_USABLE") && pItemSkill != NULL ){
		//可放技能的物品
	}
	else{
		//只判断了组
		FString groupCoolDown = pItem->GroupCoolDown;
		if( !groupCoolDown.IsEmpty() ){
			INT pos = groupCoolDown.InStr( TEXT("-") );
			INT groupId = appAtoi( *groupCoolDown.Left(pos) );

			CoolDownData* cdData = MMOCoolDownManager::getSingleton().GetCDData( COOLDOWN_GROUP, groupId );
			if( cdData != NULL ){
				SetItemSlotCD( pSlot, cdData->CurValue, cdData->MaxValue );
			}
			else{
				SetItemSlotCD( pSlot, 0, 0 );
			}
		}
	}
}
//--------------------------------------------------------------------------
void UMMOUISkillBar::UpdateAllItemSlotCDByItemUse( INT type, INT cdID )
{
	if( Player == NULL ){
		return;
	}
	
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Item ){
			continue;
		}

		UItem* pItem = MMOFacade::getSingleton().FindItemTemplate( slotData.AidID );
		if( pItem == NULL ){
			continue;
		}
		
		FString groupCoolDown = pItem->GroupCoolDown;
		if( type == COOLDOWN_GROUP && !groupCoolDown.IsEmpty() ){
			//组CD
			INT pos = groupCoolDown.InStr( TEXT("-") );
			INT groupId = appAtoi( *groupCoolDown.Left(pos) );

			if( groupId == cdID ){
				CoolDownData* cdData = MMOCoolDownManager::getSingleton().GetCDData( COOLDOWN_GROUP, cdID );
				if( cdData != NULL ){
					SetItemSlotCD( slotData.SlotObject, cdData->CurValue, cdData->MaxValue );
				}
			}
		}

		if( type == COOLDOWN_ITEM ){
			UMMOAbility* skill = pItem->GetToUseAbility();
			//单个的道具CD，目前没，先不处理
		}
	}
}
//===============================================================================================================
UBOOL UMMOUISkillBar::SkipCaptureInputKey( const FString& key, BYTE eIE, const FString& strHoverControllerName, FVector2D ClickPosInMovieSpace )
{
    if( strHoverControllerName == TEXT( "glass_mc" ) ||
		strHoverControllerName == TEXT( "ActionBarFx_mc" )
		)
	{
        return TRUE;
    }

    return Super::SkipCaptureInputKey( key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}
//===============================================================================================================
//技能条显示模式按职业而不同
void UMMOUISkillBar::UpdateCurBarShowType()
{
	if( Player == NULL ){
		return;
	}

	if( GFXPlayer == NULL ){
		return;
	}

	UGFxObject* career = GFXPlayer->CreateObject( TEXT("Object") );
	if( career && Player->ActorAttributes)
	{
		INT playerClassType = Player->GetClassId();
		career->SetInt( TEXT("Career"), playerClassType );
		FString strHpHead = FString::Printf(TEXT("%d/%d,"),Player->ActorAttributes->GetCurHp(), Player->ActorAttributes->GetMaxHp());
		career->SetString( TEXT("HpTip"), strHpHead+MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillBar.HpTip") ) );
		FString strMpHead = FString::Printf(TEXT("%d/%d,"),Player->ActorAttributes->GetCurMp(), Player->ActorAttributes->GetMaxMp());
		career->SetString( TEXT("MpTip"), strMpHead+MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.SkillBar.MpTip") ) + appItoa( playerClassType ) ) );
	}

	eventCallUCSetCareer( career );
}
//==============================================================================================================
void UMMOUISkillBar::FlashHPBar()
{
	if( ActionBarFx == NULL ){
		ActionBarFx = GetObject( TEXT("ActionBarFx_mc") );
	}

	if( ActionBarFx != NULL ){
		ActionBarFx->GotoAndPlayI(1);//(TEXT("start"));
		ActionBarFx->SetBool("visible",TRUE);
	}
}
//==============================================================================================================
void UMMOUISkillBar::UpdatePetState(INT stateID)
{
	MMOFacade::getSingleton().PetState = stateID;
	eventCallUCupdatePetSkill(stateID-1);
}


void UMMOUISkillBar::UpdateBloodTextVisible()
{
	if( HpBar != NULL ){
		UGFxObject* hpTxt = HpBar->GetObject( TEXT("Hptxt_mc") );
		if( hpTxt != NULL ){
			hpTxt->SetVisible( MMOUI::GetUIManager()->DrawBloodNumber );
		}
	}

	if( MpBar != NULL ){
		UGFxObject* mpTxt = MpBar->GetObject( TEXT("mptxt_mc"));
		if( mpTxt != NULL ){
			mpTxt->SetVisible( MMOUI::GetUIManager()->DrawBloodNumber );
		}
	}
}
//===============================================================================================================
void UMMOUISkillBar::HintForDraggingSkill( UMMOAbility *dragSkill )
{
	//if( dragSkill == NULL ){
	//	return;
	//}

	//INT cmdKeyValue = MMOSkillManager::getSingleton().GetSkillBindHotkey( dragSkill->SpellCommonAttribute.BindSlot );
	//INT curSlotIdx = MMOSkillManager::getSingleton().GetSkillbarSlotIdxByKeyCmd( cmdKeyValue );
	//if( curSlotIdx <= 0 ){
	//	//此键位不能被拖拽
	//	return;
	//}

	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i ){
		//if( i == 0 ){
		//	continue;
		//}
		eventCallASHintForSkillSlot( i, TRUE );
	}

	IsCurSkillDragHint = TRUE;

#if !FINAL_RELEASE
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("[HintForDraggingSkill ]") );
	CLEAR_WARN_COLOR();
#endif
}

//----------------------------------------------------------------------------
void UMMOUISkillBar::CloseHintForDraggingSkill()
{
	if( IsCurSkillDragHint == FALSE ){
		return;
	}
	IsCurSkillDragHint = FALSE;

	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i ){
		//if( i == 0 ){
		//	continue;
		//}
		eventCallASHintForSkillSlot( i, FALSE );
	}

#if !FINAL_RELEASE
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("[CloseHintForDraggingSkill]") );
	CLEAR_WARN_COLOR();
#endif
}

//===============================================================================================================
void UMMOUISkillBar::HintForDraggingItem( INT fromSlotPanelType, INT itemSlotID )
{
	IsCurItemDragHint = TRUE;

	eventCallASHintForItemSlot( TRUE );

#if !FINAL_RELEASE
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("[HintForDraggingItem AT:%d, %d]"), fromSlotPanelType, itemSlotID );
	CLEAR_WARN_COLOR();
#endif
}

//----------------------------------------------------------------------------
void UMMOUISkillBar::CloseHintForDraggingItem()
{
	if( IsCurItemDragHint == FALSE ){
		return;
	}
	IsCurItemDragHint = FALSE;

	eventCallASHintForItemSlot( FALSE );

#if !FINAL_RELEASE
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("[CloseHintForDraggingItem]") );
	CLEAR_WARN_COLOR();
#endif
}

//===============================================================================================================
void UMMOUISkillBar::CloseHintForAllDragging()
{
	CloseHintForDraggingSkill();
	CloseHintForDraggingItem();
}
//===============================================================================================================
void UMMOUISkillBar::RefreshAllSkillSlotLoopEffect( INT certainSkillID )
{
	for( INT i = 0; i < UCONST_SkillBarSlotMax; ++i )
	{
		FSlotContainer &slotData = SkillBarSlotData( i );
		if( slotData.Type != ESBarSlotTp_Skill ){
			continue;
		}

		if( certainSkillID != 0 ){
			UINT skillID;
			UINT skillLv;
			UINT skillRuneID;
			MMOSkillManager::DecomposeAbilityID( slotData.ID, skillID, skillLv, skillRuneID );
			if( skillID == certainSkillID ){
				CheckSlotLoopEffect( i );
			}
		}
		else{
			CheckSlotLoopEffect( i );
		}
	}
}
//--------------------------------------------------------------
void UMMOUISkillBar::CheckSlotLoopEffect( INT slotIdx )
{
	FSlotContainer &slotData = SkillBarSlotData( slotIdx );
	switch( slotData.Type )
	{
	case ESBarSlotTp_Skill:
		{
			MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
			ECommandKey cmdKey = GetSkillBarSlotCmdKey( slotIdx );
			UMMOAbility* curEquippedSkill = mmoSkillMgr.GetEquippedSkill( cmdKey );
			if( curEquippedSkill != NULL ){
				INT skillID = curEquippedSkill->eventGetAbilityId();

				UBOOL bActive = mmoSkillMgr.GetSkillSlotActive( skillID );
				eventCallSkillActivate( slotIdx, 1, bActive);

				bActive = mmoSkillMgr.GetSkillSlotFlag1( skillID );
				eventCallSkillActivate( slotIdx, 0, bActive);
			}
			else{
				StopAllLoopEffect( slotIdx );
			}
		}
		break;
	case ESBarSlotTp_Item:
	case ESBarSlotTp_Mount:
	default:
		{
			StopAllLoopEffect( slotIdx );
		}
		break;
	}
}
//------------------------------------------------------------
void UMMOUISkillBar::StopAllLoopEffect( INT slotIdx )
{
	eventCallSkillActivate( slotIdx, 1, FALSE );
	eventCallSkillActivate( slotIdx, 0, FALSE );
}
//===============================================================================================================
//血球，魔球渲染
UBOOL UMMOUISkillBar::DrawMyHUD(class AMMOHud* H)
{
	if(!GetVisible() || !GDrawGFx)
	{
		return FALSE;
	}

	FLOAT drawX = 0.0f;
	FLOAT drawY = 0.0f;

	const FLOAT C_TargetSize = 128.0f;
	const FLOAT C_FrameWidth = 182.0f;
	const FLOAT C_FrameHeight = 124.0f;

	H->Canvas->SetDrawColor(255, 255, 255, 255);

	FVector2D scrSz = MMOFacade::getSingleton().GetScreenSize();

	if(g_drawMaterialGlobeHalf)
	{
		UGFxObject* bgObj = GetObject(TEXT("HpBar_mc"));


		drawX = eventCallGetBloodGlobeDrawX();//(scrSz.X - C_TargetSize) * 0.5f;
		drawY = eventCallGetBloodGlobeDrawY();//scrSz.Y - C_TargetSize * 2.0f;
		bgObj->GetAbsolutePosition(drawX, drawY);
		//drawY = scrSz.Y - 137;
		// draw blood globe
		// 		MaterialGlobeHalf_Blood->SetScalarParameterValue(TEXT("Percent") ,
		// 			(sin(((FLOAT)0 * 0.2f + GWorld->GetTimeSeconds() * 0.2f)) + 1.0f) * 0.5f);				

		H->Canvas->DrawMaterialTile(MaterialGlobeHalf_Blood, drawX, drawY, 1, C_TargetSize, C_TargetSize, 0, 0, 1, 1);


		//drawX = eventCallGetEnergyGlobeDrawX();//(scrSz.X - C_TargetSize) * 0.5f;
		//drawY = scrSz.Y + eventCallGetEnergyGlobeDrawY();//scrSz.Y - C_TargetSize * 2.0f;
		//bgObj->GetAbsolutePosition(drawX, drawY);
		//drawY = scrSz.Y - 137;


		UMaterialInstance* mat = GetMPBallMaterila();
		//mat->SetScalarParameterValue(TEXT("Percent") , (sin(((FLOAT)1 * 0.2f + GWorld->GetTimeSeconds() * 0.5f)) + 1.0f) * 0.5f);				
		H->Canvas->DrawMaterialTile(mat, drawX - 1, drawY, 1, C_TargetSize, C_TargetSize, 0, 0, 1, 1);
		// 		DrawBox2D(H->Canvas->Canvas, FVector2D(drawX, drawY), FVector2D(drawX + C_TargetSize, drawY + C_TargetSize), FLinearColor(1.0f, 1.0f, 0, 1.0f));


		// draw glass & frame
		H->Canvas->DrawTile(GlobeGlassTexture, drawX + 1, drawY, 2, C_TargetSize, C_TargetSize,
			2, 0, 128, 128, FLinearColor(1.0f, 1.0f, 1.f));

		//  		FLOAT frameOffset = (C_TargetSize - C_FrameWidth) * 0.5f;
		// 		H->Canvas->DrawTile(GlobeFrameTexture, drawX + frameOffset, drawY, 2,	q
		// 			C_FrameWidth, C_FrameHeight, 0, 0, C_FrameWidth, C_FrameHeight, FLinearColor(1.0f, 1.0f, 1.f));

		// 		DrawBox2D(H->Canvas->Canvas, FVector2D(drawX + frameOffset, drawY), 
		// 			FVector2D(drawX + frameOffset + C_FrameWidth, drawY + C_FrameHeight), FLinearColor(0.0f, 1.0f, 0, 1.0f));

		// 		for (int offsetY = 0; offsetY < 10; ++offsetY)
		// 		{
		// 			DrawBox2D(H->Canvas->Canvas, FVector2D(drawX + offsetY * 2, scrSz.Y - offsetY), FVector2D(drawX + 39, scrSz.Y - 137), FLinearColor(1.0f, 0.0f, 1, 1.0f));
		// 		}
	}

	if(g_drawMaterialGlobe)
	{
		drawX = 100.0f;
		drawY = 300.0f;

		FLOAT frameOffset = (C_TargetSize - C_FrameWidth) * 0.5f;

		for (INT i = 0; i < ARRAY_COUNT(MaterialGlobe); ++i)
		{
			MaterialGlobe[i]->SetScalarParameterValue(TEXT("Percent") , (sin(((FLOAT)i * 0.2f + GWorld->GetTimeSeconds() * 0.5f)) + 1.0f) * 0.5f);		
			H->Canvas->DrawMaterialTile(MaterialGlobe[i], drawX + i * C_TargetSize * 1.2f, drawY, 1, C_TargetSize, C_TargetSize, 0, 0, 1, 1);

			H->Canvas->DrawTile(GlobeGlassTexture, drawX + i * C_TargetSize * 1.2f, drawY, 2,
				C_TargetSize, C_TargetSize, 0, 0, 128, 128, FLinearColor(1.0f, 1.0f, 1.f));

			H->Canvas->DrawTile(GlobeFrameTextureFull, drawX + i * C_TargetSize * 1.2f + frameOffset, drawY, 2,
				C_FrameWidth, C_FrameHeight, 0, 0, C_FrameWidth, C_FrameHeight, FLinearColor(1.0f, 1.0f, 1.f));
		}

	}

	return TRUE;
}

void UMMOUISkillBar::DrawHUDPostGFx(class AHUD* H)
{	

}

UMaterialInstance* UMMOUISkillBar::GetMPBallMaterila() const
{
	UMaterialInstance* mat = MaterialGlobeHalf_Blood;

	BYTE classType = MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->ClassType;
	switch(classType)
	{
	case clsWarrior:
		{
			mat = MaterialGlobeHalf_Warrior;
		}
		break;

	case clsMage:
		{
			mat = MaterialGlobeHalf_Mage;
		}
		break;

	case clsRogue:
		{
			mat = MaterialGlobeHalf_Rogue;
		}
		break;

	case clsArcher:
	case clsTaoist:
		{
			mat = MaterialGlobeHalf_Archer;
		}
		break;

	// 1.05 backport: Dragon Lancer uses the Warrior globe, Sorcerer the Mage globe
	case clsSpear:
		{
			mat = MaterialGlobeHalf_Warrior;
		}
		break;

	case clsSorcerer:
		{
			mat = MaterialGlobeHalf_Mage;
		}
		break;

	}

	return mat;
}

//===============================================================================================================
//===============================================================================================================
//void UMMOUISkillBar::ASOnPanelClose(const FString& PanelName)
//{
	//	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Option_InGameMenu);
	//MMOUI::GetUIManager()->delegateShowInGameMenu();
//}
////===============================================================================================================
//void UMMOUISkillBar::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
//{
//	if( SrcSlotType == ESPType_SkillBar || DestSlotType == ESPType_SkillBar )
//	{
//		OnDragDrop(SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID);
//		return;
//	}
//	else
//	{
//		UMMOGFXPlayer::OnDragDrop(SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID);
//	}
//}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void UMMOUISkillBar::ASStartDrag( UGFxObject* srcItemSlot, BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID)
{
	//Super::ASStartDrag( srcItemSlot, srcSlotType, srcContainerID, srcSlotID );

	//
	if( srcItemSlot == NULL ){
		return;
	}

	switch( srcSlotType )
	{
	case ESPType_SkillBar:
		{
			INT skillBarContainType = srcItemSlot->GetInt( TEXT("EquipID") );
			if( skillBarContainType == ESBarSlotTp_Skill ){
				INT uniqueSkillID = srcItemSlot->GetInt(TEXT("itemIndex"));

				UMMOAbility *pDragingAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
				if( pDragingAbility != NULL ){
					HintForDraggingSkill( pDragingAbility );
				}
			}
			else if(  skillBarContainType == ESBarSlotTp_Item  ){
				INT itemSlotIdx = srcItemSlot->GetInt( TEXT("slotIndex") );
				HintForDraggingItem( ESPType_SkillBar, itemSlotIdx );
			}
		}
		break;
		//case ESPType_SkillBar:
		//	{
		//		INT skillID = srcItemSlot->GetInt( TEXT("itemIndex") );
		//		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		//		if( mainPlayer != NULL ){
		//			UMMOAbility *dragSkill = mainPlayer->GetEquipedAbilityById( skillID );
		//			Skillbar->HintForDraggingSkill( dragSkill );
		//		}
		//	}
		//	break;
		//case ESPType_ItemBar:
		//	{
		//		//INT fromSlotType = srcItemSlot->GetInt( TEXT("slotType") );
		//		INT itemSlotIdx = srcItemSlot->GetInt( TEXT("slotIndex") );
		//		Skillbar->HintForDraggingItem( ESPType_ItemBar, itemSlotIdx );
		//	}
		//	break;
	}
}

//---------------------------------------------------------------------------------------
void UMMOUISkillBar::ASSlot_StartDrag(class UGFxObject* mcSlot)
{
	//Super::ASSlot_StartDrag( mcSlot );

	if( mcSlot == NULL ){
		return;
	}

	//
	INT slotType = mcSlot->GetInt(TEXT("slotType"));
	switch( slotType )
	{
		//case ESPType_SkillSetting:
	case ESPType_SkillBar:
		{
			INT slotIdx = (INT)mcSlot->GetInt( TEXT("slotIndex") );
			HintForDraggingItem( slotType, slotIdx );
			//INT skillBarContainType = mcSlot->GetInt( TEXT("EquipID") );
			//if( skillBarContainType == ESBarSlotTp_Skill ){
			//	INT uniqueSkillID = mcSlot->GetInt(TEXT("itemIndex"));

			//	UMMOAbility *pDragingAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			//	if( pDragingAbility != NULL ){
			//		Skillbar->HintForDraggingSkill( pDragingAbility );
			//	}
			//}
			//else if(  skillBarContainType == ESBarSlotTp_Item  ){
			//	INT itemSlotIdx = mcSlot->GetInt( TEXT("slotIndex") );
			//	Skillbar->HintForDraggingItem( ESPType_SkillBar, itemSlotIdx );
			//}
		}
		break;
		//case ESPType_ItemBar:
		//	{
		//		INT itemSlotIdx = datSlot->GetInt(TEXT("slotID"));
		//		Skillbar->HintForDraggingItem( ESPType_ItemBar, itemSlotIdx );
		//	}
		//	break;
	}
}
//===================================================================
void UMMOUISkillBar::ASOnClickNewSkillLearnedBtn()
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASOnClickNewSkillLearnedBtn No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	////
	//Skillbar->OnClickNewSkillLearnedBtn();
}

void UMMOUISkillBar::ASChangePetState(INT stateID)
{
	INT E_PS = stateID+1;
	if(E_PS>=E_ps_Idle && E_PS<=E_ps_Fight)
	{
		CMessageHandler::GetInstance().Send_Change_Pet_State(E_PS);
	}
}

//===================================================================
//技能设置
void UMMOUISkillBar::ASShowTips_SkillSettingActive( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	INT uniqueSkillID = mc->GetInt( TEXT("UniqueSkillID") );
	if( uniqueSkillID > 0 ){
		MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_SkillTemplate, mc );
	}
}
//===================================================================
void UMMOUISkillBar::ASHideTips_SkillSettingActive( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===================================================================
void UMMOUISkillBar::ASSetSkill( int skillKeyType, int skillID )
{
	TryToEquipOnSkill( skillKeyType, skillID );
}
//===================================================================
void UMMOUISkillBar::ASCloseSkillList()
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASCloseSkillList No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	//Skillbar->ShowSkillListPanel( FALSE, 0 );
}
//===================================================================
void UMMOUISkillBar::ASShowPanel( INT idx )
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASShowPanel No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	//Skillbar->ShowSkillListPanel( TRUE, idx );
}

//===================================================================
void UMMOUISkillBar::ASHideShortcutButtons()
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASHideShortcutButtons No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	//Skillbar->ShowSkillListPanel( FALSE, 0 );
}

//===============================================================================================================
//===============================================================================================================

void UMMOUISkillBar::SendOpenPack(INT itemId)
{
	MMOFacade::getSingleton().SendOpenPackage( itemId);
}