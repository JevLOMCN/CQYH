#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net\message_handler.h"
#include "CommonGameEvents.h"
#include "MMOPlayerInfoManager.h"
#include "MMOAttributeAccountManager.h"
#include "MMOGameEvent.h"
#include "MMOFacade.h"
#include "MMOSkillManager.h"
#include "EquipSlotManager.h"
#include "MMOEquipManager.h"
#include "UITextParser.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIPlayerInfo)
IMPLEMENT_CLASS(UMMOGFXPlayer_PlayerInfo)

#define itemRecover 13

void UMMOGFXPlayer_PlayerInfo::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_PlayerInfo::Init()");
	//MMOUI::RegisterMoviePlayer( this, MMOUI::GetUIManager()->MoviePlayer_PlayerInfo );

}

void UMMOGFXPlayer_PlayerInfo::UnInit()
{
	Super::UnInit();

}
void UMMOGFXPlayer_PlayerInfo::ASOnPanelClose(const FString& PanelName)
{

}
void UMMOGFXPlayer_PlayerInfo::ASShow_HeavenGasViewTips(class UGFxObject* obj)
{

}
void UMMOGFXPlayer_PlayerInfo::ASShow_HeavenGasPowerViewTips(class UGFxObject* obj)
{

}
void UMMOGFXPlayer_PlayerInfo::ASShowTips_HeavenBloodViewSlot(class UGFxObject* obj,INT nIndex,INT nPanel)
{
	
}
void UMMOGFXPlayer_PlayerInfo::ASShowTips_SkillTreeActiveSkillView(class UGFxObject* obj)
{
	
}
void UMMOGFXPlayer_PlayerInfo::ASHideTips_SkillTreeActiveSkillView()
{
	
}

void UMMOUIPlayerInfo::ASOnPanelClose(const FString& PanelName)
{
	if( PanelName == MMOUI::GetUIManager()->Panel_PlayerInfo.ToString())
	{
		MMOPlayerInfoManager::getSingleton().setIsShowInfoByRankList(false);
	}
	//Super::ASOnPanelClose(PanelName);
}

void UMMOUIPlayerInfo::ASShow_HeavenGasViewTips(class UGFxObject* obj)
{
	if(obj == NULL)
		return;
	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
	if(playerDI == NULL)
		return;
	INT nGasIndex = obj->GetInt( TEXT("ItemTemplateID") );

	if(nGasIndex >= 0 && nGasIndex < playerDI->GasItemList.Num())
	{
		const GasItem* pkItem = &(playerDI->GasItemList(nGasIndex));
		if (pkItem)
		{
			if (pkItem->nTempID > 0)
			{
				FString tips;
				MMOHeavenManager::getSingleton().GetHeavenGasTipsByGasID(tips,pkItem->nTempID,pkItem->nCurExp);
				obj->SetString( TEXT("tipsHtml"), tips );
				MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, obj );
			}
		}
	}
}
void UMMOUIPlayerInfo::ASShow_HeavenGasPowerViewTips(class UGFxObject* obj)
{

}
void UMMOUIPlayerInfo::ASShowTips_HeavenBloodViewSlot(class UGFxObject* obj,INT nIndex,INT nPanel)
{
	if( obj == NULL ){
		return;
	}
	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
	if(playerDI == NULL)
		return;
	FString tips;
	{
		INT nSlotIndex = nPanel*MAX_BLOODSEA_SLOT_PER_PAGE + nIndex;
		if(nSlotIndex >= playerDI->arrSlotBlood.Num())
			return;
		const BloodSlot* pkSlot = &(playerDI->arrSlotBlood(nSlotIndex));
		if(pkSlot == NULL)
			return;
		if (pkSlot->nValue == 0)
		{
			tips = TEXT("Not enabled");
		}
		else if (MAX_BLOODSEA_SLOT_NUMBER == pkSlot->nValue)
		{
			tips += pkSlot->sName;
			tips += " ";
			tips +=  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodSlotOPHint1"));
		}
		else
		{
			tips += pkSlot->sName;
			tips += " ";
			tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodSlotOPHint3")),pkSlot->nValue,MAX_BLOODSEA_SLOT_NUMBER);
		}

		const HeavenBloodConfig* pkConfig =  MMOHeavenManager::getSingleton().GetHeavenBloodData(nPanel+1,nIndex+1,pkSlot->nValue);
		if(pkConfig)
		{
			tips += UITextParser::HTMLNewLine();
			tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
			MMOHeavenManager::getSingleton().GenerateGasTipsByNodeID(tips,pkConfig->nAttValue1,pkConfig->nAttValue2);
			tips += UITextParser::HTMLEndFont();
		}
		const HeavenBloodConfig* pkNextConfig = MMOHeavenManager::getSingleton().GetHeavenBloodData(nPanel+1,nIndex+1,pkSlot->nValue + 1);
		if(pkNextConfig)
		{
			if(pkNextConfig->nAttValue1 == 0 && pkNextConfig->nAttValue2 == 0)
				return;
			tips += UITextParser::HTMLBeginFont(TEXT("#555555"));
			tips += UITextParser::HTMLNewLine();
			tips +=  FString::Printf(TEXT("Tier %d"),pkSlot->nValue + 1);
			tips += UITextParser::HTMLNewLine();
			MMOHeavenManager::getSingleton().GenerateGasTipsByNodeID(tips,pkNextConfig->nAttValue1,pkNextConfig->nAttValue2);
			tips += UITextParser::HTMLEndFont();
		}
	}
	obj->SetString( TEXT("tipsHtml"), tips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, obj );
}
void UMMOUIPlayerInfo::ASShowTips_SkillTreeActiveSkillView(class UGFxObject* obj)
{
	if( obj == NULL ){
		return;
	}

	INT uniqueSkillID = obj->GetInt( TEXT("UniqueSkillID") );
	if( uniqueSkillID > 0 ){
		MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplate, obj);
	}
}
void UMMOUIPlayerInfo::ASHideTips_SkillTreeActiveSkillView()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//----------------UMMOUIPlayerInfo-------------

void UMMOUIPlayerInfo::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v == false)
	{
		MMOPlayerInfoManager::getSingleton().setIsShowInfoByRankList(false);
	}
}

void UMMOUIPlayerInfo::Init()
{

	Super::Init();
	
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_PlayerDetailInfoAttrChange);
	evts.AddItem(GameEvent_PlayerDetailInfoEquipChange);
	evts.AddItem(GameEvent_PlayerBaseInfo);
	evts.AddItem(GameEvent_PlayerDetailInfoSpellChange);
	evts.AddItem(GameEvent_PlayerDetailInfoSpiritChange);

	EventAdapter = new TGameEventAdapter<UMMOUIPlayerInfo>(this, evts);


	//Copy the character UI here
	INT slotValue[E_NES_TOTAL] = {};
	slotValue[0] = E_NES_SHOULDER;
	slotValue[1] = E_NES_HEAD;
	slotValue[2] = E_NES_NECK;
	slotValue[3] = E_NES_ARM;
	slotValue[4] = E_NES_BODY;
	slotValue[5] = E_NES_BACK; //cloak
	slotValue[6] = E_NES_WRIST_L;
	slotValue[7] = E_NES_WAIST; //belt
	slotValue[8] = E_NES_WRIST_R;
	slotValue[9] = E_NES_FINGER_L;
	//slotValue[10] = E_NES_LEG;
	slotValue[10] = E_NES_FINGER_R;
	slotValue[11] = E_NES_MAIN_HAND;
	slotValue[12] = E_NES_FOOT;

	UGFxObject* showPanel = GetObject( TEXT("showPanel_mc") );
	if( showPanel )
	{
		for( INT i = 0; i < E_NES_TOTAL; ++i )
		{
			FString slotName = TEXT("itemSlot_");
			UGFxObject* slot = showPanel->GetObject( slotName + appItoa(i) );
			if( slot )
			{
				slot->SetInt( TEXT("SlotType") , ESPType_ViewProfile );
				slot->SetInt( TEXT("EquipID") , E_CONSLOT_PROFILE );
				slot->SetInt( TEXT("itemid") , slotValue[i] );
				slot->SetString( TEXT("tipsHtml") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Equipslot.SlotTips") )+appItoa(i) ) );
				slot->SetBool( TEXT("bDisableDrag") ,TRUE);
			}
		}
	}
	InitRecover();

	UGFxObject* textData = GFXPlayer->CreateObject( TEXT("Object") );
	textData->SetString( TEXT("battlePointTips") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Charactor.BattlePointTips") ) );
	textData->SetString( TEXT("damagePointTips") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Charactor.DamagePointTips")) );
	textData->SetString( TEXT("lifePointTips") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Charactor.LifePointTips")) );
	textData->SetInt( TEXT("panelId") , ESPType_ViewProfile );
	eventCallSetPanelText(textData);

}

void UMMOUIPlayerInfo::InitRecover()
{
	UGFxObject* showPanel = GetObject( TEXT("showPanel_mc") );
	UGFxObject* slot = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , itemRecover ) );
	if(slot)
	{
		slot->SetInt( TEXT("SlotType") , ESPType_Recover );
		slot->SetInt( TEXT("EquipID") , E_CONSLOT_RECOVER );
		slot->SetInt( TEXT("itemid") , 0 );
		slot->SetString( TEXT("tipsHtml") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Equipslot.SlotTips") )+appItoa(itemRecover) ) );
		
	}
}



void UMMOUIPlayerInfo::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{
	case GameEvent_PlayerDetailInfoAttrChange:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if( msg && msg->IntData == _RoleId )
			{
				UpdatePlayerInfo();
			}
		}
		break;
	case GameEvent_PlayerDetailInfoEquipChange:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if( msg && msg->IntData == _RoleId )
			{
				UpdateEquipInfo();
				UpdateRefineInfo();
			}
		}		
		break;
	case GameEvent_PlayerDetailInfoSpellChange:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if( msg && msg->IntData == _RoleId )
			{
				UpdateSpellInfo();
			}
		}
		break;
	case GameEvent_PlayerDetailInfoSpiritChange:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			if( msg && msg->IntData == _RoleId )
			{
				UpdateSpriteInfo();
				UpdateHeavenBloodPanel();
			}
		}
		break;
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* msg = (MMOGameEvent_PlayerBaseInfo*)evt;
			if( msg && msg->_RoldID == _RoleId )
			{
				PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo( );
				if(playerDI)
				{
					PlayerBaseInfo* baseinfo = msg->_PlayerBaseInfo;
					if( baseinfo )
					{
						playerDI->Level = baseinfo->_Level;
						playerDI->ClassId = baseinfo->_RoleClass;
						playerDI->Sex = baseinfo->_Sex;
						
						FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
						if(curMapInfo.HideRoleName)
						{
							playerDI->RoleName = FString(" ");
						}
						else{
							playerDI->RoleName = baseinfo->_RoleName;
						}
					}

					//Level change changes attributes
					UpdatePlayerInfo();
					UpdateBaseInfo();
					//Equipment icons change with quest class and gender
					UpdateEquipInfo();
				}


			}
		}
		break;
	}
}

void UMMOUIPlayerInfo::InitPlayerRoleId( INT roleId )
{
	_RoleId = roleId;
	MMOPlayerInfoManager::getSingleton().SetCurRole(roleId);

	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
	if(playerDI)
	{
		CMessageHandler::GetInstance().Send_QueryRoleDetailInfo( roleId , playerDI->AttrCRC , playerDI->EquipCRC , playerDI->SpellCRC , playerDI->SpiritCRC );
	}
	else
	{
		CMessageHandler::GetInstance().Send_QueryRoleDetailInfo( roleId , 0 , 0 , 0 , 0 );
	}

	PlayerBaseInfo* baseinfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( _RoleId , TRUE );
	if( baseinfo && playerDI )
	{
		playerDI->Level = baseinfo->_Level;
		playerDI->ClassId = baseinfo->_RoleClass;
		playerDI->Sex = baseinfo->_Sex;

		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if(curMapInfo.HideRoleName)
		{
			playerDI->RoleName = FString(" ");
		}
		else{
			playerDI->RoleName = baseinfo->_RoleName;
		}
		eventCallUCSetWindowTitle(playerDI->RoleName);
	}
		
	eventCallUCUpdateInfoState(MMOPlayerInfoManager::getSingleton().ShowDetailCurRole());
	UpdatePlayerInfo();
	UpdateEquipInfo();
}

void UMMOUIPlayerInfo::UpdateBaseInfo()
{
	PlayerDetailInfo* di = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();

	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if( data && di )
	{
		data->SetString( TEXT("name") , di->RoleName );
		data->SetString( TEXT("levelText") , appItoa( di->Level ) + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Character.Lv") ) );
		FString className = MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( di->ClassId ) );
		data->SetString( TEXT("classText") , className );
		data->SetInt( TEXT("classId") , di->ClassId );
		data->SetInt( TEXT("sex") , di->Sex );

		eventCallUpdateBaseInfo(data);
	}

}

void UMMOUIPlayerInfo::UpdatePlayerInfo()
{
	MMOAttributeAccountManager& AttributeMgr = MMOAttributeAccountManager::getSingleton();
	AttributeGroupArray& attriGroup = AttributeMgr.GetAttributeGroupArray();
		
	AttributeMgr.SetAttrOwnerMode(AttrListenerType_OtherPlayer);


	//attributes
	UGFxObject* dataArray = GFXPlayer->CreateArray();
	for ( INT i = 1 ; i < attriGroup.Num() ; i++ )
	{
		UGFxObject* groupData = GFXPlayer->CreateObject( TEXT("Object") );
		UGFxObject* attributeArray = GFXPlayer->CreateArray();

		for ( INT j = 0 ; j < attriGroup(i).AttributeList.Num() ; j++ )
		{
			UGFxObject* itemData = GFXPlayer->CreateObject( TEXT("Object") );
			itemData->SetString( TEXT("itemName") , AttributeMgr.GetAttributeName(i,j) );

			itemData->SetString( TEXT("itemValue") , AttributeMgr.GetAttributeValue(i,j) );

			attributeArray->SetElementObject( j , itemData );
		}

		groupData->SetString( TEXT("titleName") , attriGroup(i).GroupName );
		groupData->SetObject( TEXT("attriList") , attributeArray );

		dataArray->SetElementObject( i-1 , groupData );

	}
	eventCallSetAttributeData(dataArray);

	//combat power
	UGFxObject* dmgData = GFXPlayer->CreateObject( TEXT("Object") );
	if( dmgData )
	{
		PlayerDetailInfo* di = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
		dmgData->SetString( TEXT("damagePoint") , MMOAttributeAccountManager::getSingleton().GetDamagePoint() );
		dmgData->SetString( TEXT("lifePoint") , MMOAttributeAccountManager::getSingleton().GetLifePoint() );
		if(di)
		{
			//
			dmgData->SetString( TEXT("battlePoint") , appItoa( di->Power ) );
		}
		
		eventCallUpdateBattlePoint(dmgData);
	}
}

void UMMOUIPlayerInfo::UpdateEquipInfo()
{
	UGFxObject* showPanel = GetObject( TEXT("showPanel_mc") );
	if( !showPanel )
	{
		return;
	}

	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindPlayerDetailInfo( _RoleId );
	if(playerDI)
	{
		INT playerSex = ( playerDI != NULL ? playerDI->Sex : EGT_Male );
		INT playerActClass = ( playerDI != NULL ? playerDI->ClassId : EPCT_PlayerClassA );

		for( INT i = 0 ; i < E_NES_TOTAL ; i++)
		{
			UGFxObject* mc = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , i )) ;
			if( mc != NULL )
			{
				INT serverSlotID = mc->GetInt( TEXT("itemid") );

				if( serverSlotID >= 0 )
				{
					INT itemTempId =  playerDI->EquipList[serverSlotID].m_nTemplateID;
					UItem* equip = ( itemTempId != 0 ? MMOFacade::getSingleton().FindItemTemplate(itemTempId) : NULL);
					if( equip != NULL )
					{
						//GFXPlayer->SetItemSlotData( mc , equip , ESPType_ViewProfile , E_CONSLOT_PROFILE, serverSlotID );
						UBOOL useLargeIcon = ( mc != NULL ? mc->GetBool( TEXT("useLargeIcon") ) : FALSE );

						FString iconPath;
						if( useLargeIcon )
						{
							iconPath = equip->GetLargeIconName( playerSex == EGT_Male, playerActClass );
						}
						else
						{
							iconPath = MMOFacade::getSingleton().GetItemIconNamePersonalized( equip, NULL, playerActClass );
						}
						GFXPlayer->eventCallSetItemData( mc, 1, iconPath, equip->ItemId, ESPType_ViewProfile, TEXT(""), equip->ItemColor + 1, E_CONSLOT_PROFILE, serverSlotID );
					}
					else
					{
						//GFXPlayer->SetItemSlotData( mc , NULL , ESPType_ViewProfile , E_CONSLOT_PROFILE, serverSlotID );
						GFXPlayer->eventCallSetItemData( mc, 0, TEXT(""), -1, ESPType_ViewProfile, TEXT(""), ITEMCOLOR_INVALID, E_CONSLOT_PROFILE, serverSlotID );
					}

				}
			}
		}
	}
	else
	{
		for( INT i = 0 ; i < E_NES_TOTAL ; i++)
		{
			UGFxObject* mc = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , i )) ;
			if( mc != NULL )
			{
				INT serverSlotID = mc->GetInt( TEXT("itemid") );

				if( serverSlotID >= 0 )
				{
					//UEquip* e = Player->ItemManager->GetEquipBySlot( serverSlotID, true );
					GFXPlayer->SetItemSlotData( mc , NULL , ESPType_ViewProfile , E_CONSLOT_PROFILE, serverSlotID );	

				}
			}
		}
	}

}

UMMOAbility*  UMMOUIPlayerInfo::FindLearnedSkill( TArray<UMMOAbility*>& skillList , INT skillId )
{

	for ( INT i = 0 ; i < skillList.Num() ; i++ )
	{
		UMMOAbility* skill = skillList(i);
		if( skill && skill->GetTemplateID() == skillId )
		{
			return skillList(i);
		}
	}
	
	return NULL;
}

void UMMOUIPlayerInfo::UpdateSpellInfo()
{
	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindPlayerDetailInfo( _RoleId );
	if(playerDI == NULL)
		return;
	PlayerBaseInfo* baseinfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( _RoleId , FALSE );
	if( baseinfo && playerDI )
	{
		playerDI->Level = baseinfo->_Level;
		playerDI->ClassId = baseinfo->_RoleClass;
		playerDI->Sex = baseinfo->_Sex;

		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if(curMapInfo.HideRoleName)
		{
			playerDI->RoleName = FString(" ");
		}
		else{
			playerDI->RoleName = baseinfo->_RoleName;
		}
	}

	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	MMOFacade &myFacade = MMOFacade::getSingleton();
	UMMOKeyManager &keyMgr = *UMMOKeyManager::getSingleton();

	TMap< INT, INT > equipSkillsMap;

	//Get all skills and classificate
	TMap< INT, class UMMOAbility* > allMyMainSkillsClassed[SKILL_EQUIP_MAX];
	INT learnedSkillCnt =  playerDI->SkillList.Num();
	for( INT i = 0; i < learnedSkillCnt; ++i ){
		UMMOAbility *learnedAbility = playerDI->SkillList( i );
		INT skillSetIdx = mmoSkillMgr.GetSkillBindDefaultSlot( learnedAbility->SpellCommonAttribute.BindSlot );
		if( skillSetIdx < 0 || skillSetIdx >= SKILL_EQUIP_MAX ){
			continue;
		}

		//
		INT skillID = learnedAbility->eventGetAbilityId();
		allMyMainSkillsClassed[skillSetIdx].FindOrAdd( skillID ) = learnedAbility;
	}
	
	//
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT addRowCnt = 0;

	for( INT i = 0; i < SKILL_EQUIP_MAX; ++i ){
		TMap< INT, class UMMOAbility* > &thisClassSkills = allMyMainSkillsClassed[i];
		if( thisClassSkills.Num() <= 0 ){
			continue;
		}
		mmoSkillMgr.SortAllAbilitiesByRequireLevel( thisClassSkills );

		//Group
		UGFxObject* rowDataObj = GFXPlayer->CreateObject( TEXT("ActionScript3.PlayerInfoWidget.SkillViewItemData") );
		if(rowDataObj == NULL)
			continue;

		//create a new row for content
		INT loopAddedCnt = 0;
		UGFxObject* singleRowDatas = GFXPlayer->CreateArray();
		if(singleRowDatas == NULL)
			continue;
		//ShowAllActiveSkillsChkBtn->GetBool( TEXT("selected") );
		for( TMap< INT, class UMMOAbility* >::TIterator it(thisClassSkills); it; ++it ){
			UMMOAbility *thisAbility = it.Value();
			UMMOAbility *mainAbility = thisAbility;
			//
			INT mainSkillID = mainAbility->eventGetAbilityId();

			//Prepare data, put into the refresh array
			UGFxObject* itemSingleObj = GFXPlayer->CreateObject( TEXT("ActionScript3.PlayerInfoWidget.SkillViewSingleItemData") );
			//
			if(itemSingleObj == NULL)
				continue;
			itemSingleObj->SetInt( TEXT("ItemIdx"), loopAddedCnt );
			//
			UBOOL IsNew = mmoSkillMgr.IsNewLearnedSkill( mainSkillID );
			itemSingleObj->SetBool( TEXT("IsNew"), IsNew );
			//
			INT skillSetIdx = mmoSkillMgr.GetSkillBindDefaultSlot( mainAbility->SpellCommonAttribute.BindSlot );
			itemSingleObj->SetInt( TEXT("SkillSlotType"), skillSetIdx );
			//
			INT *pSlotBindSkillID = equipSkillsMap.Find( skillSetIdx );
			UBOOL IsCurrent = ( pSlotBindSkillID != NULL && mainSkillID == *pSlotBindSkillID );
			itemSingleObj->SetBool( TEXT("IsCurrent"), IsCurrent );
			//
			FString skillName = mainAbility->ClientCommonAttribute.AbilityName;
			itemSingleObj->SetString( TEXT("SkillName"), skillName );
			//
			INT skillLevel = mainAbility->SpellCommonAttribute.Level ;
			itemSingleObj->SetInt( TEXT("SkillLv"), skillLevel );
			//
			INT skillRequireLevel = mainAbility->SpellCommonAttribute.RequireLevel;
			itemSingleObj->SetInt( TEXT("SkillRequireLv"), skillRequireLevel );
			//
			itemSingleObj->SetBool( TEXT("IsDisabled"), FALSE );
			//
			itemSingleObj->SetBool( TEXT("CanLvUp"), FALSE );
			//
			UBOOL isMaxLv = ( mmoSkillMgr.GetAbilityWithRuneByNextLevel( mainAbility ) == NULL );
			itemSingleObj->SetBool( TEXT("IsMaxLv"), isMaxLv );
			//
			INT uniqueSkillID = MMOSkillManager::CombinateAbilityID( mainSkillID, skillLevel, 0 );
			itemSingleObj->SetInt( TEXT("SkillID"), uniqueSkillID );
			//
			INT runeSkillIdx = thisAbility->GetRuneSlot();
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
		rowDataObj->SetInt( TEXT("TitleType"), i );
		FString keyName;
		
		//CQ_SKILL_HJJ;Begin
		//if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER){
		//	keyMgr.KeyToString(keyMgr.FindKeyworld(( UMMOKeyManager::EKeyFunction)( UMMOKeyManager::KF_BarSkillEx1 + i ), FALSE ), keyName, 1 );
		//}
		//else{
		keyMgr.KeyToString(keyMgr.FindKeyworld(( UMMOKeyManager::EKeyFunction)( UMMOKeyManager::KF_BarSkill1 + i ), FALSE ), keyName, 1 );
		//}
		//CQ_SKILL_HJJ;End
		rowDataObj->SetString( TEXT("TitleString"), keyName );
		//
		rowDataObj->SetObject( TEXT("SingleItemsData"), singleRowDatas );

		//
		dataProvider->SetElementObject( addRowCnt, rowDataObj );
		++addRowCnt;
	}
	eventCallUpdateSkillData(dataProvider);
}

void UMMOUIPlayerInfo::UpdateRefineInfo()
{
	if(!GFXPlayer)
	{
		return;
	}
	
	UGFxObject* data = GFXPlayer->CreateObject(TEXT("UI_EquipEnhance.EquipEnhanceAS3.EquipSlotIconData"));
	if(data)
	{
		PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindPlayerDetailInfo( _RoleId );

		if(playerDI)
		{
			UGFxObject* pathArray = GFXPlayer->CreateArray();
			EquipSlotManager &eSlotMgr = EquipSlotManager::getSingleton();
			for( INT i = 0 ; i < NES_TOTAL ; i++ )
			{
				INT curSlotEnhanceLv = playerDI->RefineLevels[i];
				//
				UGFxObject* gfxObj = GFXPlayer->CreateObject("UI_EquipEnhance.EquipEnhanceAS3.UIEquipSlotData");

				if(curSlotEnhanceLv == MAX_ENHANCE_LEVEL)
				{
					//Top level special handling
					gfxObj->SetString(TEXT("iconPath"),TEXT("IconPath_Max_Max"));
					gfxObj->SetString(TEXT("selectBg"),TEXT("IconBg_Max_Max"));
					gfxObj->SetInt(TEXT("level"),0);
					gfxObj->SetString(TEXT("shotDesc"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MaxLevelDesc")));
					gfxObj->SetInt(TEXT("quality"),5);
				}
				else
				{
					INT ehcMajorLevel = 0;
					INT ehcMinorLevel = 0;
					INT ehcMinorMinorLv = 0;
					eSlotMgr.CvtEnhanceLevel( curSlotEnhanceLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
					gfxObj->SetString(TEXT("iconPath"),FString::Printf(TEXT("IconPath_%d_%d"),ehcMajorLevel,ehcMinorLevel));
					gfxObj->SetString(TEXT("selectBg"),FString::Printf(TEXT("IconBg_%d_%d"),ehcMajorLevel,ehcMinorLevel));
					gfxObj->SetInt(TEXT("level"),ehcMinorMinorLv);
					gfxObj->SetString(TEXT("shotDesc"),MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.EquipSlotEnhance.LevelDesc_%d"),ehcMinorLevel)));
					gfxObj->SetInt(TEXT("quality"),ehcMajorLevel);
				}

				gfxObj->SetBool(TEXT("select"),FALSE);

				pathArray->SetElementObject(i,gfxObj);
			}

			data->SetObject(TEXT("EquipSlotDataArray"), pathArray);
			
		}
		else
		{
			UGFxObject* refineList = GFXPlayer->CreateArray();
			if(refineList)
			{				
				data->SetObject( TEXT("refineList") , refineList );
			}
			
		}

		eventCallUpdateRefineData(data);
	}
	
}

void UMMOUIPlayerInfo::UpdateSpriteInfo()
{
	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();

	if(playerDI == NULL)
		return;

	UGFxObject* pkArrData = GFXPlayer->CreateArray();
	if(pkArrData == NULL)
		return;
	for (INT n = 0; n < UCONST_GasEquipSlotMaxNum;++n)
	{
		UGFxObject* pkData = GFXPlayer->CreateObject(TEXT("ActionScript3.PlayerInfoWidget.HeavenGasViewData"));
		if(pkData == NULL)
			return;
		FString hint0;
		FString hint1;
		FString strIcon;
		bool    bLocked = false;
		INT		nPower = 0;
		pkData->SetInt("nIndex",n);
		//open
		if(n < playerDI->GasItemList.Num())
		{
			const GasItem*pkItem = &(playerDI->GasItemList(n));
			if(pkItem == NULL)
				return;
			if(pkItem->nTempID > 0)
			{
				const HeavenGasItemConfig* pkConfig = MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
				if(pkConfig != NULL)
				{
					hint0 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkConfig->nQuality));
					hint0 += pkConfig->name;
					hint0 += UITextParser::HTMLEndFont();
					hint1 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkConfig->nQuality));
					hint1 += FString::Printf(TEXT("Lv %d"),pkConfig->nLevel);
					hint1 += UITextParser::HTMLEndFont();

					strIcon = FString::Printf(TEXT("../%s"),*(pkConfig->Icon0));
					nPower = MMOHeavenManager::getSingleton().GetGasPowerByID(pkItem->nTempID);
				}
			}
			else if(pkItem->nUniqueID >= 0)
			{
				hint0 = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)1));
				hint0 += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.EquipGasUnLock"));
				hint0 += UITextParser::HTMLEndFont();
				hint1 = "";
			}
			else
			{
				hint0 = UITextParser::HTMLBeginFont(TEXT("#FF0000"));
				hint0 += TEXT("Locked");
				hint0 += UITextParser::HTMLEndFont();
				hint1 = "";
				bLocked = true;
			}
		}
		pkData->SetString("sLabel0",hint0);
		pkData->SetString("sLabel1",hint1);
		pkData->SetString("sIconName",strIcon);
		pkData->SetBool("bLocked",bLocked);
		pkData->SetInt("nTempID",n);
		pkData->SetInt("nWarPower",nPower);
		pkArrData->SetElementObject(n,pkData);
	}
	eventCallUCSetHeavenViewGasData(pkArrData);
}
void UMMOUIPlayerInfo::UpdateHeavenBloodPanel()
{
	PlayerDetailInfo* playerDI = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();

	if(playerDI == NULL)
		return;

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(!dataProvider)
		return;

	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	INT nLevel = mainPlayer->ActorAttributes->heavenAttributes.heavenLevel;

	for (INT i = 0;i < MAX_BLOODSEA_PAGE; ++i)
	{
		UGFxObject* data = GFXPlayer->CreateObject("ActionScript3.PlayerInfoWidget.HeavenBloodViewData");
		if(!data)
			continue;
		const HeavenBloodConfig* pkBlood = 	MMOHeavenManager::getSingleton().GetHeavenBloodData(i+1,1,1);
		if(pkBlood == NULL)
			continue;
		BOOL bActive = playerDI->nBloodState[i];
		data->SetBool("bActivate",bActive);
		if(!bActive)
		{
			FString strCon = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.OpenNeedHeavenLevel")),pkBlood->nPageLevelLimit);
			data->SetString("sConditionHint",strCon);
		}
		FString strCon = TEXT("MMOGame.Heaven.BloodName");
		strCon += appItoa(i+1);
		data->SetString("sName",MMOUI::ParseLocalizedPropertyPath(strCon));

		UGFxObject* InfoArray = GFXPlayer->CreateArray();
		UGFxObject* InfoArrayName = GFXPlayer->CreateArray();
		if(InfoArray == NULL || InfoArrayName == NULL)
			continue;
		INT nComplete = 0;
		for (INT n = 0; n < MAX_BLOODSEA_SLOT_PER_PAGE; ++n)
		{
			INT nIndex = (i*MAX_BLOODSEA_SLOT_PER_PAGE + n);
			if(nIndex >= playerDI->arrSlotBlood.Num())
				continue;
			const BloodSlot* pkSlot = &(playerDI->arrSlotBlood(nIndex));
			if(pkSlot)
			{
				InfoArray->SetElementInt(n,pkSlot->nValue);
				InfoArrayName->SetElementString(n,pkSlot->sName);
				nComplete += pkSlot->nValue;
			}
			else
			{
				InfoArray->SetElementInt(n,0);
				InfoArrayName->SetElementString(n,TEXT(""));
			}
		}
		data->SetInt("nCompleteState", nComplete);
		data->SetObject("arrLevel",InfoArray);
		data->SetObject("arrSlotName",InfoArrayName);
		dataProvider->SetElementObject(i,data);
	}
	eventCallUCSetHeavenViewBloodData(dataProvider);
}