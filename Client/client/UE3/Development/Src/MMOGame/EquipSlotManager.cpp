#include "stdafx.h"
#include "MMOFacade.h"
#include "EquipSlotManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"		
#include "CommonGameEvents.h"
#include "MMOGuideManager.h"
#include "MMOChatManager.h"
#include "MMOGameClasses.h"
#include "TeamManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOEquipManager.h"
#include "MMOAttributeAccountManager.h"
#include "UITextParser.h"
#include "MMOPowerManager.h"
#include "MMOQuestManager.h"
#include "MMOClientVariableManager.h"

#define USE_TEST_CONFIG 0


EquipSlotManager::EquipSlotManager()
{
	MMOUI::RegisterEventHandler(this,GameEvent_EquipSlotSyncLevel);
	MMOUI::RegisterEventHandler(this,GameEvent_EquipSlotEnhanceLvUp);
	m_iEnhanceFlag = 0;

	//Init();
}

EquipSlotManager::~EquipSlotManager()
{
	MMOUI::UnRegisterEventHandler(this,GameEvent_EquipSlotSyncLevel);
	MMOUI::UnRegisterEventHandler(this,GameEvent_EquipSlotEnhanceLvUp);
}

void EquipSlotManager::Init()
{
	ReadEnhancePropConfig();
	ReadEquipSlotEnhanceConfig();
}

void EquipSlotManager::Shutdown()
{

}

void EquipSlotManager::Clear()
{
	//for(INT i = 0; i < EQUIP_SLOT_COUNT; i++)
	//{
	//	m_EquipSlotEnhanceConsumeMap[i].clear();
	//}
	////_EnhancePropCfg.Empty();

	//EquipSlotMaxEnhanceTemplate::iterator itor = m_EquipSlotMaxEnhanceTemplate.begin();
	//for(; itor != m_EquipSlotMaxEnhanceTemplate.end(); ++itor)
	//{
	//	itor->second.clear();
	//}
	//m_EquipSlotMaxEnhanceTemplate.clear();
}

//==========================================================================================================================
void EquipSlotManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_EquipSlotSyncLevel:
		{
			MMOGameEvent_Any* equipSlotEvent = (MMOGameEvent_Any*)evt;
			//INT slotID = equipSlotEvent->IntData;
			
			MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_EquipSlotEnhancePow);

			//UMMOUIEquipSlotEnhancePanel* panel = Cast<UMMOUIEquipSlotEnhancePanel>(MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_EquipSlotEnhance));
			UMMOUICharacter* panel = Cast<UMMOUICharacter>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Character));
			if( panel != NULL && panel->IsVisible && panel->_CurType == EChaPT_Enhance && panel->_EnhcPanel )
			{
				panel->_EnhcPanel->UpdateUIViews();
			}
		}
		break;
	case GameEvent_EquipSlotEnhanceLvUp:
		{
			MMOGameEvent_EquipSlotEnhanceResult* equipSlotEnhaceResultEvt = (MMOGameEvent_EquipSlotEnhanceResult*)evt;
			if( !equipSlotEnhaceResultEvt->IsSuccess ){
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.Failed")),MsgInfoType_Table,TEXT("MMOGame.EquipEnhance.Success"));
				MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.Failed")));
				break;
			}

			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.Success")),MsgInfoType_Table,TEXT("MMOGame.EquipEnhance.Success"));
			MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.Success")));

			//
			EquipSlotProperties *pMyEnhancedSlotProp = GetMyEquipSlotProperty( equipSlotEnhaceResultEvt->EnhanceSlotID );
			if( pMyEnhancedSlotProp != NULL )
			{
				pMyEnhancedSlotProp->CurEnhanceLevel = equipSlotEnhaceResultEvt->NewEnhanceLv;
			}

			//
			UMMOUICharacter* panel = Cast<UMMOUICharacter>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Character));
			if( panel != NULL && panel->IsVisible && panel->_CurType == EChaPT_Enhance && panel->_EnhcPanel )
			{
				panel->_EnhcPanel->UpdateUIViews();
			}

			//播放粒子特效
			//MMOFacade::getSingleton().GetMainPlayer()->ShowEquipLevelUpEffect(_MyEquipSlotProperties[slotId].cur_level,slotId == 0);
			MMOFacade::getSingleton().GetMainPlayer()->NotifyRefreshEquipLevel( equipSlotEnhaceResultEvt->EnhanceSlotID );


			//服务器不发广播就本地模拟一个
			/*if(!_MyEquipSlotProperties[slotId].bSysTip)
			{
				FString MainPlayerName = MMOFacade::getSingleton().GetMainPlayer()->ActorName;
				FString MsgContent = FString::Printf(TEXT("<#P0:%s><#E:%d><#EL:%d><#T:MMOGame.Charactor.RefineSuccess>"),*MainPlayerName,slotId,_MyEquipSlotProperties[slotId].cur_level);
				MMOChatManager::getSingleton().ReceiveMessage(0, cctTvShow, MsgContent, TEXT(""), -1);
			}*/

			//
			MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_EquipSlotEnhancePow);

			//
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Strong);
			/*
			// 成就计数
			for (INT32 nIndex = 0; nIndex < NES_TOTAL; nIndex++)
			{
			}
			*/
		}
		break;
	default:
		break;
	}
}

//================================================================================================================
void EquipSlotManager::SyncEquipSlotLevel(UINT16 level[NES_TOTAL])
{
	check( NES_TOTAL == EQUIP_SLOT_COUNT );
	for(INT i = 0; i < EQUIP_SLOT_COUNT; i++)
	{
		_MyEquipSlotProperties[i].CurEnhanceLevel = (UINT)level[i];
	}

	MMOGameEvent_Any evt( GameEvent_EquipSlotSyncLevel );
	MMOUI::PostGameEvent(&evt);
}

//=========================================================================================================
void EquipSlotManager::ReadEnhancePropConfig()
{
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("refine_attr.txt"), TRUE ) )
	{
		warnf(TEXT("EquipSlotManager::ReadRefineConfig refine_attr.txt error"));
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	INT i = 0,j = 0;
	FString EquipSlotPrefix(TEXT("E_"));
	INT rowCnt = tabFile->GetRowCount();
	for(i = 0 + 3; i < rowCnt; ++i)
	{
		INT refineLv = appAtoi( tabFile->GetData( TEXT("RefineLevel"), i ) );
		INT refineDataClassId = mmoFacade.FindClientEnumValue( tabFile->GetData( TEXT("Class"), i ), TEXT("ECHAR_CLASS_TYPE"));
		INT slotID = mmoFacade.FindClientEnumValue( tabFile->GetData( TEXT("NormalEquipSlot"), i ), TEXT("ENORMAL_EQUIP_SLOT"), 1 );	

		//记录额外强化加成数据索引
		if( slotID == NES_TOTAL && refineDataClassId >= 0 && refineDataClassId < EPCT_MAX )
		{
			INT allLevelCnt = _RefineExtrPropLvIdx[refineDataClassId].Num();
			UBOOL isAdded = FALSE;
			for( INT j = allLevelCnt - 1; j >= 0; --j )
			{
				if( _RefineExtrPropLvIdx[refineDataClassId]( j ) < refineLv )
				{
					if( j + 1 == allLevelCnt )
					{
						_RefineExtrPropLvIdx[refineDataClassId].Push( refineLv );
					}
					else
					{
						_RefineExtrPropLvIdx[refineDataClassId].InsertItem( refineLv, j + 1 );
					}

					isAdded = TRUE;
					break;
				}
			}

			if( isAdded != TRUE )
			{
				_RefineExtrPropLvIdx[refineDataClassId].InsertItem( refineLv, 0 );
			}
		}

		//
		UINT keyID = CombineRefineKey( refineDataClassId, slotID, refineLv );
		EquipSlotEnhancePropCfgData &dataElement = _EnhancePropCfg.FindOrAdd( keyID );

		dataElement.ClassId = refineDataClassId;
		dataElement.EquipSlotId = slotID;
		dataElement.RefineLv = refineLv;

		for(j = 0; j < ENHANCE_EFFECT_COUNT; ++j)
		{
			dataElement.EffectProperties[j] = appAtoi( tabFile->GetData( 3 + j, i ) );
		}
	}

	delete tabFile;
}

//----------------------------------------------------------------------------------------------------------------
void EquipSlotManager::ReadEquipSlotEnhanceConfig()
{
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("refine.txt"), TRUE ) )
	{
		warnf(TEXT("EquipSlotManager::ReadRefineConfig refine_attr.txt error"));
		return;
	}

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	INT rowCnt = tabFile->GetRowCount();
	for( INT i = 0 + 3; i < rowCnt; ++i)
	{
		INT refineLv = appAtoi( tabFile->GetData( TEXT("RefineLevel"), i ) );

		EquipSlotEnhanceCfgData &dataElement = _EnhanceEquipSlotCfg.FindOrAdd( refineLv );
		dataElement.PlayerRequireLv		= appAtoi( tabFile->GetData( TEXT("RequireLevel"), i ) );
		dataElement.BaseSuccessRate		= appAtoi( tabFile->GetData( TEXT("SuccessRate"), i ) );
		dataElement.SuccessRateFactor	= appAtoi( tabFile->GetData( TEXT("BasePow"), i ) );
		dataElement.MoneyCost			= appAtoi( tabFile->GetData( TEXT("Cost"), i ) );

		INT bBroadCast = appAtoi( tabFile->GetData( TEXT("BroadCast"), i ) );
		if(bBroadCast > 0)
		{
			dataElement.IsSysTip = TRUE;
		}
		else
		{
			dataElement.IsSysTip = FALSE;
		}
	}

	delete tabFile;
}
//----------------------------------------------------------------------------------------------------------------
const EquipSlotEnhancePropCfgData *EquipSlotManager::FindEnhancePropCfg( INT classID, INT partID, INT refineLv )
{
	UINT keyID = CombineRefineKey( classID, partID, refineLv );
	EquipSlotEnhancePropCfgData *pDataElement = _EnhancePropCfg.Find( keyID );
	if( pDataElement == NULL )
	{
		return NULL;
	}

	return pDataElement;
}

//----------------------------------------------------------------------------------------------------------------
const EquipSlotEnhanceCfgData *EquipSlotManager::FindEnhanceConsumeCfg( INT refineLv )
{
	EquipSlotEnhanceCfgData *pDataElement = _EnhanceEquipSlotCfg.Find( refineLv );
	if( pDataElement == NULL )
	{
		return NULL;
	}

	return pDataElement;
}

//==========================================================================================================================
//UBOOL EquipSlotManager::CheckMyMaterialCondition(INT slotId)
//{
//	MMOFacade &mmoFacade = MMOFacade::getSingleton();
//	AMMOPlayer *pMainPlayer = mmoFacade.GetMainPlayer();
//	if( pMainPlayer == NULL )
//	{
//		return FALSE;
//	}
//	INT playerClass = pMainPlayer->GetClassId();
//
//	for(INT i = 0; i < ENHANCE_CONSUME_COUNT; i++)
//	{
//		const EquipSlotEnhancePropCfgData *pSlotEnhanceLvProp = FindEnhancePropCfg( playerClass, slotId, _MyEquipSlotProperties[slotId].CurEnhanceLevel );
//		if( pSlotEnhanceLvProp != NULL && pSlotEnhanceLvProp->MatId[i] > 0 )
//		{
//			INT iCount = mmoFacade.GetItemCountInBag(pSlotEnhanceLvProp->MatId[i], E_CONSLOT_DEFAULT);;
//			if(iCount < pSlotEnhanceLvProp->MatCnt[i])
//				return FALSE;
//		}
//	}
//
//	return TRUE;
//}

//==========================================================================================================================
UBOOL EquipSlotManager::CheckEnhanceConsume( INT containerID, INT slotID, INT checkSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );
	if( pInfo == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EnhanceGeneral.ERROR_ITEM_NOT_FOUND") ),MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.ERROR_ITEM_NOT_FOUND") );
		return FALSE;
	}


	//是否是装备
	UItem *pItem = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	UEquip *pEquip = Cast<UEquip>( pItem );
	if( pEquip == NULL ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EnhanceGeneral.ERROR_NOT_A_EQUIP") ) ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.ERROR_NOT_A_EQUIP"));
		return FALSE;
	}

	//
	UBOOL canEquipToThisSlot = CheckEnhanceConsume( pEquip, checkSlotID );
	if( !canEquipToThisSlot ){
		//需要对应装备位置的装备
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EnhanceGeneral.ERROR_UNFIT_CONSUME_EQUIP") ) ,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.ERROR_UNFIT_CONSUME_EQUIP"));
		return FALSE;
	}

	return canEquipToThisSlot;
}
//==========================================================================================================================
UBOOL EquipSlotManager::CheckEnhanceConsume( UEquip *pEquip, INT checkSlotID )
{
	if( pEquip == NULL ){
		return FALSE;
	}

	//检查是对应
	INT equipConID = -1;
	INT equipSlotID = -1;
	INT equipType =  pEquip->GetEquipAppearenceTemplate()->EquipType;
	MMOUI::GetEquippedSlotByEquipType( equipType, equipConID, equipSlotID);
	UBOOL canEquipToThisSlot = ( equipConID == E_CONSLOT_EQUIPPED );
	if( equipSlotID == checkSlotID ){
		canEquipToThisSlot |= TRUE;
	}
	else{
		if( ( ( checkSlotID == E_NES_FINGER_R || checkSlotID == E_NES_FINGER_L ) && equipType == EET_Normal_Finger ) || 
			( ( checkSlotID == E_NES_WRIST_R || checkSlotID == E_NES_WRIST_L ) && equipType == EET_Normal_Wrist ) ){
			canEquipToThisSlot |= TRUE;
		}
		else{
			canEquipToThisSlot = FALSE;
		}
	}

	//
	return canEquipToThisSlot;
}
//==========================================================================================================================
EquipSlotProperties* EquipSlotManager::GetMyEquipSlotProperty(INT slotID)
{
	if(slotID < EQUIP_SLOT_COUNT && slotID >= 0)
	{
		return &_MyEquipSlotProperties[slotID];
	}
	else
	{
		return NULL;
	}
}

//==========================================================================================================================
INT EquipSlotManager::GetMyEquipSlotLevel(INT slotID)
{
	EquipSlotProperties *pMyEquipSlotProp = GetMyEquipSlotProperty( slotID );
	if( pMyEquipSlotProp == NULL )
	{
		return -1;
	}
	else
	{
		return _MyEquipSlotProperties[slotID].CurEnhanceLevel;
	}
}

//==========================================================================================================================
INT EquipSlotManager::GetCurEnhanceHoleLv()
{
	INT curMyEnhanceHoleLv = MAX_ENHANCE_LEVEL;
	for( INT i = 0 ; i < EQUIP_SLOT_COUNT ; i++)
	{
		EquipSlotProperties *pCurEnhanceSlotLv = GetCurMySlotEnhanceState(i);
		if( pCurEnhanceSlotLv == NULL )
		{
			continue;
		}
		////时装格子不强化
		//if( i == NES_AVATAR )
		//{
		//	continue;
		//}

		INT curSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;
		if( curMyEnhanceHoleLv > curSlotEnhanceLv ){
			curMyEnhanceHoleLv = curSlotEnhanceLv;
		}
	}

	return curMyEnhanceHoleLv;
}
//================================================================================================================
UBOOL EquipSlotManager::IsMaxSlotEnhanceLevel( INT enhanceClassID, INT slotID, INT enhanceLv )
{
	const EquipSlotEnhancePropCfgData *pSlotEnhanceLvProp = FindEnhancePropCfg( enhanceClassID, slotID, enhanceLv );
	return ( pSlotEnhanceLvProp != NULL ? FALSE : TRUE );
}

//================================================================================================================
void EquipSlotManager::CvtEnhanceLevel( INT enhanceLv, INT &ehcMajorLevel, INT &ehcMinorLevel, INT &ehcMinorMinorLv )
{
	if( enhanceLv < 1 ){
		ehcMajorLevel = 0;
		ehcMinorLevel = 0;
		ehcMinorMinorLv = 0;
		return;
	}
	else{
		enhanceLv -= 1;
	}

	ehcMajorLevel = enhanceLv / 30;
	ehcMinorLevel = ( enhanceLv % 30 ) / 10;
	ehcMinorMinorLv = ( enhanceLv % 30 ) % 10 + 1;
}
//==========================================================================================================================
FString EquipSlotManager::GetEquipPropertyNameById(INT id)
{
	const ItemRandomAttr* pAttr = MMOFacade::getSingleton().GetRandomAttributById(id);
	if(NULL != pAttr)
		return pAttr->strText;
	return TEXT("");
}

//==========================================================================================================================
FString EquipSlotManager::GetEquipSlotNameById(INT id)
{
	static UEnum* enumClass = NULL;
	if( !enumClass )
	{
		enumClass = FindObject<UEnum>(UServerEnum::StaticClass(),TEXT("ENORMAL_EQUIP_SLOT"), TRUE);
	}
	check(enumClass != NULL);

	FString enumVal = enumClass->GetEnum(id).ToString();
	return MMOUI::ParseLocalizedPropertyPath(FString(TEXT("Properties.ENORMAL_EQUIP_SLOT.")) + enumVal);
}

//==========================================================================================================================
void EquipSlotManager::SendEnhanceEquipSlot( INT refineSlot, INT consumeConID, INT consumeSlotID )
{
	if( consumeConID >= 0 && consumeSlotID >= 0 )
	{
		CMessageHandler::GetInstance().Send_Refine( (UINT8)refineSlot, TRUE, (UINT8)consumeConID,  (UINT8)consumeSlotID );
	}
	else
	{
		CMessageHandler::GetInstance().Send_Refine( (UINT8)refineSlot, FALSE, 0xff, 0xff );
	}
}

//==========================================================================================================================
//==========================================================================================================================

//================================================================================================================
INT EquipSlotManager::GetEnhanceActiveLevel() const
{
	INT lv = 0x7fffffff;
	for(INT i = 0; i < EQUIP_SLOT_COUNT; ++i)
	{
		INT cv = _MyEquipSlotProperties[i].CurEnhanceLevel / 10;
		lv = (cv < lv) ? cv : lv;
	}
	return lv;
}
//==========================================================================================================================
EquipSlotEnhanceActiveTemplate& EquipSlotManager::GetEquipSlotEnhanceActiveTemplate(INT lv)
{
	check( lv >= 0 && lv < MAX_ACTIVE_LEVEL );
	return m_EquipSlotEnhanceActiveData[lv];
}
//==========================================================================================================================
void EquipSlotManager::ApplyEnhancePropertyToEquip()
{
	//AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	////UActorAttribute* actorAttributes = mainPlayer->ActorAttributes;
	//for(INT i = 0; i < EQUIP_SLOT_COUNT; ++i)
	//{
	//	UEquip* pEquip = mainPlayer->ItemManager->GetEquipBySlot( i, true );
	//	if(!pEquip)
	//		continue;

	//	EquipSlotProperties& current_slot_property = GetEquipSlotProperty(i);
	//	EnhanceEffect& current_enhance_effect = GetEquipSlotEffectData(i);
	//	EnhanceEffectValueTemplate& current_effect_data = current_enhance_effect.effect_values_list(current_slot_property.cur_level);

	//	for(INT j = 0; j < ENHANCE_EFFECT_COUNT; j++)
	//	{
	//		const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( current_enhance_effect.effect_title.propertiesIds[j] );
	//		mainPlayer->ActorAttributes->AttributeList(current_enhance_effect.effect_title.propertiesIds[j]) += current_effect_data.propertiesValues[j];
	//	}
	//}
}