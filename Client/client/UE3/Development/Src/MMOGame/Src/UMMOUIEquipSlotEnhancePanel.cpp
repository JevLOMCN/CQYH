#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "EquipSlotManager.h"
#include "ClientActionConditionCheck.h"
#include "MMOEquipManager.h"
#include "MMOGuideManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIEquipSlotEnhancePanel)

void UMMOUIEquipSlotEnhancePanel::PostOnShow(UBOOL v)
{
	//Super::PostOnShow(v);

	if(v)
	{
		InitToDefaultShow();
		//
		GrayAllInBagNoNeedForEnhance( v );
		//
		UpdateUIViews();
		
		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowStrongPanel);
		MMOUI::PostGameEvent(&evtGuide);

		//MMOFacade::getSingleton().PlayUISound(TEXT("TaskSheet_Op"));
	}
	else
	{
		GrayAllInBagNoNeedForEnhance( v );
		//
		ClearUI();

		//MMOFacade::getSingleton().PlayUISound(TEXT("TaskSheet_Cls"));
	}
}

//==================================================================================================================
void UMMOUIEquipSlotEnhancePanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_MySpiritLevelUp:
		{
			//if( IsVisible ){
				MMOGameEvent_OneInt* sLvUpEvt = (MMOGameEvent_OneInt*)evt;
				UpdateCurSlotDetail();
			//}
		}
		break;
	case GameEvent_Item_Delete:
		{
			MMOGameEvent_Item *itemDelEvt = (MMOGameEvent_Item*)evt;

			//if( IsVisible ){
				BagSlotChange( itemDelEvt->ContainerIdx, itemDelEvt->SlotIdx );
			//}
		}
		break;
	}
}

//============================================================================================
void UMMOUIEquipSlotEnhancePanel::UpdateUIViews()
{
	if( Player == NULL )
	{
		return;
	}
	UpdateSummary();

	//
	UpdateCurSlotDetail();
}

//========================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::BagSlotChange(INT bagId,INT slotId)
{
	//
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( bagId, slotId );
	//
	if( bagId == E_CONSLOT_DEFAULT )
	{
		if( pItemInfo != NULL )
		{
			UItem *pItemInBag = MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID );
			if( pItemInBag != NULL )
			{
				UBOOL isNeedToGray = IsNeedToGrayForEquipSlotEnhance( pItemInBag, _PanelCurSelSlotID );
				MMOFacade::getSingleton().GrayItem( ESGMark_EquipSlotEnhance, E_CONSLOT_DEFAULT, slotId, isNeedToGray );
			}
		}
		else
		{
			if( bagId == _ToEnhanceConsume._ConID && slotId == _ToEnhanceConsume._SlotID )
			{
				UnSetEnhanceConsumeItem();
			}
		}

	}

	////
	//if( pItemInfo == NULL )
	//{
	//	//Whatever, just go through it directly
	//	UpdateCurSlotDetail();
	//}
	//else
	//{
	//	EquipSlotProperties *pCurEnhanceSlotLv = EquipSlotManager::getSingleton().GetCurMySlotEnhanceState( _PanelCurSelSlotID );
	//	if( pCurEnhanceSlotLv == NULL )
	//	{
	//		return;
	//	}

	//	INT curSelSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;
	//	const EquipSlotEnhancePropCfgData *pNextSlotEnhanceLvProp = EquipSlotManager::getSingleton().FindEnhanceConsumeCfg( curSelSlotEnhanceLv + 1 );
	//	if( pNextSlotEnhanceLvProp == NULL )
	//	{
	//		return;
	//	}

	//	//
	//	UINT itemID = pItemInfo->m_nTemplateID;
	//	for( INT i = 0; i < ENHANCE_CONSUME_COUNT; i++)
	//	{
	//		if( itemID == pNextSlotEnhanceLvProp->MatId[i] )
	//		{
	//			UpdateCurSlotDetail();
	//			break;
	//		}
	//	}
	//}
}

//=======================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::UpdateCurSlotDetail()
{
	if( Player == NULL )
	{
		return;
	}

	UINT playerLevel = Player->ActorAttributes->heavenAttributes.heavenLevel;
	UpdateDetail( _PanelEnhanceClassID, playerLevel, _PanelCurSelSlotID );
}


//------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipSlotEnhancePanel::UpdateDetail( INT enhanceClassID, INT playerLv, INT curSelSlotID )
{
	if( curSelSlotID > EQUIP_SLOT_COUNT || curSelSlotID < 0 )
	{
		return;
	}

	UGFxObject* gfxObj = GFXPlayer->CreateObject(TEXT("UI_EquipEnhance.EquipEnhanceAS3.EquipSlotPropertyData"));
	if(!gfxObj)
		return;

	EquipSlotManager &eSlotMgr = EquipSlotManager::getSingleton();
	EquipSlotProperties *pCurEnhanceSlotLv = eSlotMgr.GetCurMySlotEnhanceState(curSelSlotID);
	if( pCurEnhanceSlotLv == NULL )
	{
		return;
	}

	INT curSelSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;

	if(curSelSlotEnhanceLv == MAX_ENHANCE_LEVEL)
	{
		//Top level special handling
		gfxObj->SetString(TEXT("IconPath"),TEXT("IconPath_Max_Max"));
		gfxObj->SetInt(TEXT("CurLevel"),0);
		gfxObj->SetString(TEXT("LevelDesc"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MaxLevel")));
		gfxObj->SetString(TEXT("ShotDesc"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MaxLevelDesc")));
		gfxObj->SetInt(TEXT("Quality"),5);
	}
	else
	{
		INT ehcMajorLevel = 0;
		INT ehcMinorLevel = 0;
		INT ehcMinorMinorLv = 0;
		eSlotMgr.CvtEnhanceLevel( curSelSlotEnhanceLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
		gfxObj->SetInt(TEXT("EnhcClass"),ehcMinorLevel);
		gfxObj->SetInt(TEXT("Quality"),ehcMajorLevel);
		gfxObj->SetInt(TEXT("CurLevel"),ehcMinorMinorLv);
		if( ehcMajorLevel == 0 && ehcMinorLevel == 0 && ehcMinorMinorLv == 0 ){
			gfxObj->SetString(TEXT("LevelDesc"),TEXT(""));
		}
		else{
			gfxObj->SetString(TEXT("LevelDesc"),MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.EquipSlotEnhance.Level_%d"),ehcMinorLevel)));
		}
		
		gfxObj->SetString(TEXT("ShotDesc"),TEXT("+"));
	}

	const EquipSlotEnhancePropCfgData *pCurSelSlotProp = eSlotMgr.FindEnhancePropCfg( enhanceClassID, curSelSlotID, curSelSlotEnhanceLv );

	//
	FString strValue = eSlotMgr.GetEquipSlotNameById(curSelSlotID);
	gfxObj->SetString(TEXT("EquipSlotName"),strValue);
	FString strName;
	
	const ItemRandomAttr* curEnhancePropAttr[ENHANCE_EFFECT_COUNT];
	//Current attributes
	INT i = 0;
	if(curSelSlotEnhanceLv > 0 && pCurSelSlotProp != NULL)
	{
		for(i = 0; i < ENHANCE_EFFECT_COUNT; i++)
		{
			const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( pCurSelSlotProp->EffectProperties[i] );
			curEnhancePropAttr[i] = propAttr;
			if( propAttr != NULL )
			{
				strValue = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.CHARACTERATTRTEXT.")) + propAttr->attrType);

				strName = FString::Printf(TEXT("EquipEnhanceProperty0_%d"),i);
				gfxObj->SetString(strName,strValue);

				strName = FString::Printf(TEXT("EquipEnhanceEffect_0_%d"),i);
				//gfxObj->SetInt(strName,propAttr->value);
				gfxObj->SetString(strName, MMOFacade::getSingleton().GetRandomAttributeFormatedValue(propAttr));
			}
			else
			{
				strName = FString::Printf(TEXT("EquipEnhanceEffect_0_%d"),i);
				//gfxObj->SetInt(strName,0);
				gfxObj->SetString(strName, TEXT(""));
				strName = FString::Printf(TEXT("EquipEnhanceProperty0_%d"),i);
				gfxObj->SetString(strName,TEXT(""));
			}
		}

	}
	else
	{
		for(i = 0; i < ENHANCE_EFFECT_COUNT; i++)
		{
			strName = FString::Printf(TEXT("EquipEnhanceProperty0_%d"),i);
			gfxObj->SetString(strName,TEXT(""));
			strName = FString::Printf(TEXT("EquipEnhanceEffect_0_%d"),i);
			gfxObj->SetString(strName,TEXT(""));
		}
	}

	//Next level attributes
	const EquipSlotEnhancePropCfgData *pNextSlotEnhanceLvProp = eSlotMgr.FindEnhancePropCfg( enhanceClassID, curSelSlotID, curSelSlotEnhanceLv + 1 );
	if( pNextSlotEnhanceLvProp != NULL )
	{
		for(i = 0; i < ENHANCE_EFFECT_COUNT; i++)
		{
			const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( pNextSlotEnhanceLvProp->EffectProperties[i] );
			if( propAttr != NULL )
			{
				strValue = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.CHARACTERATTRTEXT.")) + propAttr->attrType);

				strName = FString::Printf(TEXT("EquipEnhanceProperty1_%d"),i);
				gfxObj->SetString(strName,strValue);

				strName = FString::Printf(TEXT("EquipEnhanceEffect_1_%d"),i);
				//gfxObj->SetInt(strName,propAttr->value);
				//for( INT j = 0; j < ENHANCE_EFFECT_COUNT; j++ )
				//{
				//	if( curEnhancePropAttr[j] == NULL ){
				//		continue;
				//	}

				//	if( propAttr->attrType != curEnhancePropAttr[j]->attrType ){
				//		continue;
				//	}

				//	//
				//	INT isBatter = MMOFacade::IsSameClassRandomAttributeBetter( propAttr, curEnhancePropAttr[j] );
				//	if( isBatter > 0 ){
				//		break;
				//	}
				//	else if( isBatter == 0 ){

				//	}
				//	else{

				//	}
				//	break;
				//}

				gfxObj->SetString(strName,MMOFacade::getSingleton().GetRandomAttributeFormatedValue(propAttr));
			}
			else
			{
				strName = FString::Printf(TEXT("EquipEnhanceEffect_1_%d"),i);
				//gfxObj->SetInt(strName,0);
				gfxObj->SetString(strName,TEXT(""));
				strName = FString::Printf(TEXT("EquipEnhanceProperty1_%d"),i);
				gfxObj->SetString(strName,TEXT(""));
			}
		}
	}
	else
	{
		for(i = 0; i < ENHANCE_EFFECT_COUNT; i++)
		{
			strName = FString::Printf(TEXT("EquipEnhanceProperty1_%d"),i);
			gfxObj->SetString(strName,TEXT(""));
			strName = FString::Printf(TEXT("EquipEnhanceEffect_1_%d"),i);
			gfxObj->SetString(strName,TEXT(""));
		}
	}

	//Required conditions
	const EquipSlotEnhanceCfgData *pNextSlotEnhanceLvConsume = eSlotMgr.FindEnhanceConsumeCfg( curSelSlotEnhanceLv + 1 );
	if( pNextSlotEnhanceLvConsume != NULL )
	{
		//level requirement
		if( pNextSlotEnhanceLvConsume->PlayerRequireLv <= playerLv )
		{
			gfxObj->SetBool(TEXT("CanEnhance"),TRUE);
			gfxObj->SetString(TEXT("NextEnhcLevelHint"),TEXT(""));
		}
		else
		{
			gfxObj->SetBool(TEXT("CanEnhance"),FALSE);
			FString errorHint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceGeneral.EnhcNeedHighLvHint")), pNextSlotEnhanceLvConsume->PlayerRequireLv );
			gfxObj->SetString(TEXT("NextEnhcLevelHint"),errorHint);
			gfxObj->SetString(TEXT("EnhanceDesc"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceGeneral.EnhcCannotDesc")));
		}

		//Gold requirement
		INT iCost = pNextSlotEnhanceLvConsume->MoneyCost;
		gfxObj->SetString(TEXT("MoneyCostHtml"), UITextParser::HTMLMoneyIconString( MONEYTYPE_02, iCost ));
		gfxObj->SetString(TEXT("MoneyDescText"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MoneyConsumeType1")));

		//Base success rate
		pNextSlotEnhanceLvConsume->BaseSuccessRate;
	}
	else
	{
		gfxObj->SetBool(TEXT("CanEnhance"),FALSE);
		gfxObj->SetString(TEXT("EnhanceDesc"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceGeneral.MaxLevel")));
		gfxObj->SetString(TEXT("MoneyCostHtml"), TEXT("") );
		gfxObj->SetString(TEXT("MoneyDescText"), TEXT("") );
	}

	//Upgrade materials
	//MMOFacade &mmoFacade = MMOFacade::getSingleton();
	//for(i = 0; i < ENHANCE_CONSUME_COUNT; i++)
	//{
	//	UGFxObject* matConsume_mc = showPanel_mc->GetObject(FString::Printf(TEXT("MatConsume_mc_%d"),i));
	//	check( matConsume_mc != NULL );
	//	matConsume_mc->GetObject(TEXT("valueAmount"))->SetVisible(FALSE);

	//	UItem* item = NULL;
	//	if( pNextSlotEnhanceLvProp != NULL && pNextSlotEnhanceLvProp->MatId[i] > 0 && pNextSlotEnhanceLvProp->PlayerRequireLv <= playerLv)
	//	{
	//		item = MMOFacade::getSingleton().FindItemTemplate(pNextSlotEnhanceLvProp->MatId[i]);
	//		check( item != NULL );
	//		//GFXPlayer->SetIconShowByItem( matConsume_mc, item );
	//		INT CurrentHave = mmoFacade.GetItemCountInBag(pNextSlotEnhanceLvProp->MatId[i], E_CONSLOT_DEFAULT);
	//		GFXPlayer->SetItemSlotData(matConsume_mc, item, ESPType_EquipSlotEnhance);
	//		gfxObj->SetInt(FString::Printf(TEXT("MatId_%d"),i),item->GetTemplateID());
	//		gfxObj->SetInt(FString::Printf(TEXT("MatHave_%d"),i),(CurrentHave > 99)?99:CurrentHave);
	//		gfxObj->SetInt(FString::Printf(TEXT("MatNeed_%d"),i),pNextSlotEnhanceLvProp->MatCnt[i]);
	//	}
	//	else
	//	{
	//		GFXPlayer->SetItemSlotData(matConsume_mc, NULL, ESPType_EquipSlotEnhance);
	//		gfxObj->SetInt(FString::Printf(TEXT("MatId_%d"),i),0);
	//	}
	//}

	//
	UpdateCurSuccessRate();

	//
	eventCallUpdateDetailPanel(gfxObj);
}

//=======================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::UpdateCurSuccessRate()
{
	EquipSlotProperties *pCurEnhanceSlotLv = EquipSlotManager::getSingleton().GetCurMySlotEnhanceState( _PanelCurSelSlotID );
	if( pCurEnhanceSlotLv == NULL )
	{
		return;
	}

	INT curSelSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;

	const EquipSlotEnhanceCfgData *pToEnhanceLvConsume = EquipSlotManager::getSingleton().FindEnhanceConsumeCfg( curSelSlotEnhanceLv + 1 );

	FLOAT totalRate = 0;
	if( pToEnhanceLvConsume != NULL ){
		totalRate = (FLOAT)pToEnhanceLvConsume->BaseSuccessRate;
		if( _ToEnhanceConsume._ConID >= 0 && _ToEnhanceConsume._SlotID >= 0 ){
			FClientItemInfo *pSuccessHelpItem = MMOFacade::getSingleton().FindItem( _ToEnhanceConsume._ConID, _ToEnhanceConsume._SlotID );
			if( pSuccessHelpItem != NULL ){
				totalRate += (FLOAT)pSuccessHelpItem->Pow * 100 / (FLOAT)( pToEnhanceLvConsume->SuccessRateFactor );
			}
		}
	}

	//
	if( totalRate > 10000 ){
		totalRate = 10000;
	}

	//
	if( _SuccessRateTf != NULL ){
		_SuccessRateTf->SetText( FString::Printf( TEXT("%d%%"), (INT)totalRate / 100 ) );
	}
	
}

//=======================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::UpdateSummary()
{
	//Other info besides the main display area, such as player name
	UGFxObject* gfxObj = GFXPlayer->CreateObject(TEXT("UI_EquipEnhance.EquipEnhanceAS3.EquipSlotIconData"));
	gfxObj->SetString(TEXT("WindowTitle"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.WidgetTitle")));
	gfxObj->SetString(TEXT("EnhanceHint_0"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceHint.EnhanceHint0")));
	gfxObj->SetString(TEXT("EnhanceHint_1"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceHint.EnhanceHint1")));
	UGFxObject* pathArray = GFXPlayer->CreateArray();
	UpdateSummaryUI(pathArray);

	gfxObj->SetObject(TEXT("EquipSlotDataArray"), pathArray);
	eventCallUpdateSummaryPanel(gfxObj);

}

//------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipSlotEnhancePanel::UpdateSummaryUI(UGFxObject* PathArray)
{
	if( Player == NULL )
	{
		return;
	}

	EquipSlotManager &eSlotMgr = EquipSlotManager::getSingleton();
	for( INT i = 0 ; i < EQUIP_SLOT_COUNT ; i++)
	{
		EquipSlotProperties *pCurEnhanceSlotLv = eSlotMgr.GetCurMySlotEnhanceState(i);
		if( pCurEnhanceSlotLv == NULL )
		{
			continue;
		}
		INT curSlotEnhanceLv = pCurEnhanceSlotLv->CurEnhanceLevel;

		//
		UGFxObject* gfxObj = GFXPlayer->CreateObject("UI_EquipEnhance.EquipEnhanceAS3.UIEquipSlotData");

		if(curSlotEnhanceLv == MAX_ENHANCE_LEVEL)
		{
			//Top level special handling
			INT ehcMajorLevel = 0;
			INT ehcMinorLevel = 0;
			INT ehcMinorMinorLv = 0;
			eSlotMgr.CvtEnhanceLevel( curSlotEnhanceLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
			gfxObj->SetInt(TEXT("EnhcClass"),ehcMinorLevel);
			//gfxObj->SetString(TEXT("iconPath"),TEXT("IconPath_Max_Max"));
			//gfxObj->SetString(TEXT("selectBg"),TEXT("IconBg_Max_Max"));
			gfxObj->SetInt(TEXT("level"),ehcMinorMinorLv);
			gfxObj->SetString(TEXT("shotDesc"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EquipSlotEnhance.MaxLevelDesc")));
			gfxObj->SetInt(TEXT("quality"),ehcMajorLevel);
		}
		else
		{
			INT ehcMajorLevel = 0;
			INT ehcMinorLevel = 0;
			INT ehcMinorMinorLv = 0;
			eSlotMgr.CvtEnhanceLevel( curSlotEnhanceLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
			//gfxObj->SetString(TEXT("iconPath"),FString::Printf(TEXT("IconPath_%d_%d"),ehcMajorLevel,ehcMinorLevel));
			gfxObj->SetInt(TEXT("EnhcClass"),ehcMinorLevel);
			//gfxObj->SetString(TEXT("selectBg"),FString::Printf(TEXT("IconBg_%d_%d"),ehcMajorLevel,ehcMinorLevel));
			gfxObj->SetInt(TEXT("level"),ehcMinorMinorLv);
			gfxObj->SetString(TEXT("shotDesc"),MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.EquipSlotEnhance.LevelDesc_%d"),ehcMinorLevel)));
			gfxObj->SetInt(TEXT("quality"),ehcMajorLevel);
		}

		if(i == _PanelCurSelSlotID)
		{
			gfxObj->SetBool(TEXT("select"),TRUE);
		}
		else
		{
			gfxObj->SetBool(TEXT("select"),FALSE);
		}

		PathArray->SetElementObject(i,gfxObj);
	}
}

//=======================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::SelectEnhanceSlot( INT slotID )
{
	if( _PanelCurSelSlotID != slotID && _PanelCurSelSlotID >= 0 )
	{
		UnSetEnhanceConsumeItem();
	}

	//
	_PanelCurSelSlotID = slotID;

	//
	UpdateCurSlotDetail();
	//
	GrayAllInBagNoNeedForEnhance( TRUE );

	//
	MMOGameEvent_Guide evtGuide(EGUET_ChooseHeadPart);
	MMOUI::PostGameEvent(&evtGuide);
}

//=======================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::CallAsToSelectEnhanceSlot( INT slotID, UBOOL isToSelected )
{
	if( slotID < 0 || slotID >= EQUIP_SLOT_COUNT ){
		return;
	}
	FString slotName = TEXT("itemSlot_");
	slotName += appItoa( slotID );
	UGFxObject *pSlotMc = _MainPlane->GetObject( slotName );
	if( pSlotMc == NULL ){
		return;
	}

	pSlotMc->SetBool( TEXT("selected"), isToSelected );
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipSlotEnhancePanel::ToEnhanceCurSelectedSlot()
{
	if(EquipSlotManager::getSingleton().CheckFlag(EES_INCONFIRM))
	{
		return;
	}

	//
	if( Player == NULL )
	{
		return;
	}

	//
	EquipSlotProperties *pCurEnhanceSlotLv = EquipSlotManager::getSingleton().GetCurMySlotEnhanceState(_PanelCurSelSlotID);
	if( pCurEnhanceSlotLv == NULL )
	{
		return;
	}

	INT playerClass = Player->GetClassId();
	UBOOL isMaxLv = EquipSlotManager::getSingleton().IsMaxSlotEnhanceLevel( playerClass, _PanelCurSelSlotID, pCurEnhanceSlotLv->CurEnhanceLevel + 1 );
	if(isMaxLv)
	{
		FString localMsg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.EnhanceGeneral.EnhcError6"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(localMsg,MsgInfoType_Table,TEXT("MMOGame.EnhanceGeneral.EnhcError6"));
		return;
	}

	//
	RealEnhanceCurEquipSlot();
}

//-----------------------------------------------------------------------------------
void UMMOUIEquipSlotEnhancePanel::RealEnhanceCurEquipSlot()
{
	EquipSlotManager::getSingleton().SendEnhanceEquipSlot( _PanelCurSelSlotID, _ToEnhanceConsume._ConID, _ToEnhanceConsume._SlotID );
}
//=======================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::ShowEquipSlotTips( INT enhanceClassID, INT enhanceLv, INT slotID )
{
	//const EquipSlotEnhancePropCfgData *pCurSelSlotProp = EquipSlotManager::getSingleton().FindEnhancePropCfg( enhanceClassID, slotID, enhanceLv );
	//if( pCurSelSlotProp == NULL )
	//{
	//	return;
	//}

	//UGFxObject* gfxObj = GFXPlayer->CreateObject(TEXT("Object"));
	//gfxObj->SetInt(TEXT("CurLevel"),pCurSelSlotProp->RefineLv);
	////gfxObj->SetInt(TEXT("MaxLevel"),current_slot_property.max_level);
	//gfxObj->SetString(TEXT("IconPath"),TEXT("here is some pic path"));

	//FString strValue = EquipSlotManager::getSingleton().GetEquipSlotNameById(slotID);
	//gfxObj->SetString(TEXT("EquipSlotName"),strValue);
	//FString strName;

	//INT i = 0;
	//if(pCurSelSlotProp->RefineLv > 0)
	//{
	//	for(i = 0; i < ENHANCE_EFFECT_COUNT; i++)
	//	{
	//		const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( pCurSelSlotProp->EffectProperties[i] );
	//		if( propAttr != NULL )
	//		{
	//			strValue = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Hero.")) + propAttr->attrType);

	//			strName = FString::Printf(TEXT("EquipEnhanceProperty_%d"),i);
	//			gfxObj->SetString(strName,strValue);

	//			strName = FString::Printf(TEXT("EquipEnhanceEffect_%d"),i);
	//			gfxObj->SetInt(strName,propAttr->value);
	//		}
	//		else
	//		{
	//			strName = FString::Printf(TEXT("EquipEnhanceProperty_%d"),i);
	//			gfxObj->SetString(strName,TEXT(""));
	//			strName = FString::Printf(TEXT("EquipEnhanceEffect_%d"),i);
	//			gfxObj->SetInt(strName,0);
	//		}
	//	}
	//}
	//else
	//{
	//	for(i = 0; i < ENHANCE_EFFECT_COUNT; i++)
	//	{
	//		strName = FString::Printf(TEXT("EquipEnhanceProperty_%d"),i);
	//		gfxObj->SetString(strName,TEXT(""));
	//		strName = FString::Printf(TEXT("EquipEnhanceEffect_%d"),i);
	//		gfxObj->SetInt(strName,0);
	//	}
	//}

	//eventCallUpdateTips(gfxObj);
}

//=============================================================================================================================
void UMMOUIEquipSlotEnhancePanel::ShowActiveTips()
{
	UGFxObject* gfxObj = GFXPlayer->CreateObject(TEXT("Object"));

	INT current_active_lv = EquipSlotManager::getSingleton().GetEnhanceActiveLevel();
	INT i = 0;
	if(current_active_lv < 1)
	{
		gfxObj->SetBool(TEXT("ActiveMask"),FALSE);
		EquipSlotEnhanceActiveTemplate& data = EquipSlotManager::getSingleton().GetEquipSlotEnhanceActiveTemplate(0);

		for(i = 0; i < data.ActiveEffectList.Num(); i++)
		{
			FString strName = EquipSlotManager::getSingleton().GetEquipPropertyNameById(data.ActiveEffectList(i).AttributeId);
			gfxObj->SetString(FString::Printf(TEXT("PropertyName_0_%d"),i),strName);
			gfxObj->SetInt(FString::Printf(TEXT("PropertyValue_0_%d"),i),data.ActiveEffectList(i).Value);
		}
	}
	else
	{
		EquipSlotEnhanceActiveTemplate& data = EquipSlotManager::getSingleton().GetEquipSlotEnhanceActiveTemplate(current_active_lv - 1);

		for(i = 0; i < data.ActiveEffectList.Num(); i++)
		{
			FString strName = EquipSlotManager::getSingleton().GetEquipPropertyNameById(data.ActiveEffectList(i).AttributeId);
			gfxObj->SetString(FString::Printf(TEXT("PropertyName_0_%d"),i),strName);
			gfxObj->SetInt(FString::Printf(TEXT("PropertyValue_0_%d"),i),data.ActiveEffectList(i).Value);
		}

		if(current_active_lv >= MAX_ACTIVE_LEVEL)
		{
			gfxObj->SetBool(TEXT("MaxLvMask"),TRUE);
		}
		else
		{
			gfxObj->SetBool(TEXT("MaxLvMask"),FALSE);
			EquipSlotEnhanceActiveTemplate& next_data = EquipSlotManager::getSingleton().GetEquipSlotEnhanceActiveTemplate(current_active_lv);

			for(i = 0; i < next_data.ActiveEffectList.Num(); i++)
			{
				FString strName = EquipSlotManager::getSingleton().GetEquipPropertyNameById(next_data.ActiveEffectList(i).AttributeId);
				gfxObj->SetString(FString::Printf(TEXT("PropertyName_1_%d"),i),strName);
				gfxObj->SetInt(FString::Printf(TEXT("PropertyValue_1_%d"),i),next_data.ActiveEffectList(i).Value);
			}
		}
		
		gfxObj->SetBool(TEXT("ActiveMask"),TRUE);
	}

	eventCallUpdateActiveTips(gfxObj);
}

//=============================================================================================================================
void UMMOUIEquipSlotEnhancePanel::UnSetEnhanceConsumeItem()
{
	SetEnhanceConsumeItem( -1, -1 );
}

//---------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipSlotEnhancePanel::SetEnhanceConsumeItem( INT containerID, INT slotID )
{
	if( containerID < 0 || slotID < 0 ){
		if( _ToEnhanceConsume._ConID >= 0 && _ToEnhanceConsume._SlotID >= 0 ){
			//Clear, remove
			//if( pBagPanel != NULL && _ToEnhanceConsume._ConID == E_CONSLOT_DEFAULT ){
			MMOFacade::getSingleton().LockItem( _ToEnhanceConsume._ConID, _ToEnhanceConsume._SlotID, -1 );
			//}

			//GFXPlayer->SetItemSlotData( _ToEmbedEquipMc, NULL, ESPType_SoulEmbed, -1, -1 );
			GFXPlayer->eventCallSetItemData( _ToEnhanceConsumeMc , 0 , TEXT("") , -1, ESPType_EquipSlotEnhance, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
		}

		//
		_ToEnhanceConsume._ConID = -1;
		_ToEnhanceConsume._SlotID = -1;

		//Refresh success rate
		UpdateCurSuccessRate();

		return TRUE;
	}

	//Check whether the item is suitable
	UBOOL isItemOK = EquipSlotManager::getSingleton().CheckEnhanceConsume( containerID, slotID, _PanelCurSelSlotID );
	if( isItemOK != TRUE ){
		return TRUE;
	}

	//
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	FClientItemInfo *pInfo = mmoFacade.FindItem( containerID, slotID );

	UItem *pConsumeItem = NULL;
	if( pInfo != NULL ){
		pConsumeItem = mmoFacade.FindItemTemplate( pInfo->m_nTemplateID );
	}

	//Unlock the spirit previously placed in the bag
	if( _ToEnhanceConsume._ConID >= 0 && _ToEnhanceConsume._SlotID >= 0 ){
		//if( pBagPanel != NULL && _ToEnhanceConsume._ConID == E_CONSLOT_DEFAULT ){
		mmoFacade.LockItem( _ToEnhanceConsume._ConID, _ToEnhanceConsume._SlotID, -1 );
		//}
	}

	_ToEnhanceConsume._ConID = containerID;
	_ToEnhanceConsume._SlotID = slotID;

	//set UI ItemSlot
	if( pConsumeItem != NULL ){
		//GFXPlayer->SetItemSlotData( _ToEmbedEquipMc, pEmbedItem, ESPType_SoulEmbed, _ToEmbedEquip._ConID, _ToEmbedEquip._SlotID );
		GFXPlayer->eventCallSetItemData( _ToEnhanceConsumeMc, 1, MMOFacade::getSingleton().GetItemIconNamePersonalized( pConsumeItem, pInfo ), pConsumeItem->ItemId, ESPType_EquipSlotEnhance, TEXT(""), pConsumeItem->ItemColor+1, _ToEnhanceConsume._ConID, _ToEnhanceConsume._SlotID );
	}

	//Lock items in the bag
	//if( pBagPanel != NULL && _ToEnhanceConsume._ConID == E_CONSLOT_DEFAULT ){
	mmoFacade.LockItem( _ToEnhanceConsume._ConID, _ToEnhanceConsume._SlotID, ESPType_EquipSlotEnhance );
	//}

	//Refresh success rate
	UpdateCurSuccessRate();

	return TRUE;
}

//==============================================================================================================================================
UBOOL UMMOUIEquipSlotEnhancePanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipSlotEnhancePanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	switch( SlotType )
	{
	case ESPType_Bag:
		{
			//put in from bag
			FClientItemInfo *pItemInfo = mmoFacade.FindItem( ContainerID, SlotID );
			if( pItemInfo == NULL ){
				return TRUE;
			}

			//
			ClientActionCondition_EnhanceItem checker;

			FClientActionConditionContex contex;
			contex.Item = pItemInfo;

			if( !checker.CheckClientActionCondition(contex) ){
				return TRUE;
			}

			//
			SetEnhanceConsumeItem( ContainerID, SlotID );

			return TRUE;
		}
		break;
	case ESPType_EquipSlotEnhance:
		{
			INT panelSlotID = (INT)mc->GetFloat( TEXT("EquipID") ); 
			switch( panelSlotID )
			{
			case ESlotEhsSltTpy_EhsConsume1:
				{
					UnSetEnhanceConsumeItem();
				}
				break;
			}

			return TRUE;
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipSlotEnhancePanel::OnDragDrop( class UGFxObject* srcSlotMC,class UGFxObject* destSlotMC,
												BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID,
												BYTE destSlotType, BYTE destContainerID, INT destSlotID )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	if( destSlotMC == NULL || srcSlotMC == NULL ){
		return FALSE;
	}

	if( srcSlotType == ESPType_Bag && destSlotType == ESPType_EquipSlotEnhance ){
		//Enter from bag
		FClientItemInfo *pItemInfo = mmoFacade.FindItem( srcContainerID, srcSlotID );
		if( pItemInfo == NULL ){
			return TRUE;
		}

		//check whether in a UI (this UI or another operation UI)
		//
		ClientActionCondition_EnhanceItem checker;

		FClientActionConditionContex contex;
		contex.Item = pItemInfo;

		if( !checker.CheckClientActionCondition(contex) ){
			return TRUE;
		}

		//
		INT panelSlotID = (INT)destSlotMC->GetFloat( TEXT("EquipID") );
		switch( panelSlotID )
		{
		case ESlotEhsSltTpy_EhsConsume1:
			{
				SetEnhanceConsumeItem( srcContainerID, srcSlotID );
			}
			break;
		}

		return TRUE;
	}

	if( srcSlotType == ESPType_EquipSlotEnhance && destSlotType == ESPType_Bag ){
		//drag back to bag; since nothing was actually moved, dropping onto the original position is not supported
		INT panelSlotID = (INT)srcSlotMC->GetFloat( "EquipID" );
		switch( panelSlotID )
		{
		case ESlotEhsSltTpy_EhsConsume1:
			{
				UnSetEnhanceConsumeItem();
			}
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipSlotEnhancePanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

//=============================================================================================================================
void UMMOUIEquipSlotEnhancePanel::GrayAllInBagNoNeedForEnhance( UBOOL isToGray )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();
	if( Player == NULL ){
		return;
	}

	//
	UINT bagCapability = Player->ItemManager->CurCapacity( E_CONSLOT_DEFAULT );
	for( UINT j = 0; j < bagCapability; ++j){
		if( isToGray == FALSE ){
			mmoFacade.GrayItem( ESGMark_EquipSlotEnhance, E_CONSLOT_DEFAULT, j, FALSE );
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


		UBOOL isNeedToGray = IsNeedToGrayForEquipSlotEnhance( pItemInBag, _PanelCurSelSlotID );
		mmoFacade.GrayItem( ESGMark_EquipSlotEnhance, E_CONSLOT_DEFAULT, j, isNeedToGray );
	}
}

//----------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIEquipSlotEnhancePanel::IsNeedToGrayForEquipSlotEnhance( UItem *pItemInBag, INT slotID )
{
	if( pItemInBag == NULL ){
		return FALSE;
	}
	UEquip *pEquip = Cast<UEquip>( pItemInBag );

	//
	UBOOL isItemOK = EquipSlotManager::getSingleton().CheckEnhanceConsume( pEquip, slotID );

	return (!isItemOK);
}

//----------------------------------------------------------------------------------------------------------------------------
FString UMMOUIEquipSlotEnhancePanel::FormatEnhanceGuideTips()
{
	FString strValue = MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.EquipSlotEnhance.SlotEquipName") ) + appItoa( _PanelCurSelSlotID ) );
	FString hint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.GuideHint") ), *strValue );
	return hint;
}
//===================================================================================================================================
//void UMMOUIEquipSlotEnhancePanel::Init()
void UMMOUIEquipSlotEnhancePanel::InitByParent( UMMOUIBase *parentPanel )
{
	//Super::Init();

	if( parentPanel == NULL ){
		return;
	}

	_ContainerPanel = parentPanel;
	GFXPlayer = _ContainerPanel->GFXPlayer;
	ctrl = _ContainerPanel->ctrl;
	Player = _ContainerPanel->Player;

	//check( EventAdapter == NULL );
	//TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_MySpiritLevelUp);
	//evts.AddItem(GameEvent_Item_Delete);
	//EventAdapter = new TGameEventAdapter<UMMOUIEquipSlotEnhancePanel>(this, evts);
	//
	_ToEnhanceConsume._ConID = -1;
	_ToEnhanceConsume._SlotID = -1;
	//
	InitMc();
	//Refresh the equipment info in the equipment slots each time entering
	//EquipSlotManager::getSingleton().ResetEquipSlotItem();
}

//
void UMMOUIEquipSlotEnhancePanel::UnInit()
{
	_ContainerPanel = NULL;
	GFXPlayer = NULL;
	ctrl = NULL;
	Player = NULL;
}
//
void UMMOUIEquipSlotEnhancePanel::InitToDefaultShow()
{
	if( Player != NULL )
	{
		_PanelEnhanceClassID = Player->GetClassId();
	}
	else{
		_PanelEnhanceClassID = 0;
	}

	//
	//if( _PanelCurSelSlotID != 0 ){
	//	CallAsToSelectEnhanceSlot( _PanelCurSelSlotID, FALSE );
	//}
	
	_PanelCurSelSlotID = 0;
	CallAsToSelectEnhanceSlot( _PanelCurSelSlotID, TRUE );

	//
	if( _ToEnhanceConsume._ConID >= 0 && _ToEnhanceConsume._SlotID >= 0 ){
		UnSetEnhanceConsumeItem();
	}
}
//----------------------------------------------------------------------------------------------------------------------------------------------
void UMMOUIEquipSlotEnhancePanel::InitMc()
{
	if( _ContainerPanel == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOUIEquipSlotEnhancePanel InitMc _ContainerPanel is NULL]") );
		return;
	}

	_MainPlane = _ContainerPanel->GetObject( TEXT("EnhancePanel_mc") );
	if( _MainPlane == NULL ){
		return;
	}

	_ToEnhanceConsumeMc = _MainPlane->GetObject( TEXT("MatConsume_mc_0") );
	if( _ToEnhanceConsumeMc != NULL ){
		_ToEnhanceConsumeMc->SetInt( TEXT("EquipID"), ESlotEhsSltTpy_EhsConsume1 );
		_ToEnhanceConsumeMc->SetInt( TEXT("SlotType"), ESPType_EquipSlotEnhance );
	}

	//
	_SuccessRateTf = _MainPlane->GetObject( TEXT("SuccessRateTf") );
}
//===================================================================================================================================
void UMMOUIEquipSlotEnhancePanel::ClearUI()
{
	//
	UnSetEnhanceConsumeItem();

}

//===================================================================================================================================
//void UMMOUIEquipSlotEnhancePanel::ClearUI()
//{
//	//const INT C_MaxEquipItemCount = 16;
//	UGFxObject* showPanel = GetObject(TEXT("showPanel_mc"));
//	if( showPanel )
//	{
//		for (INT i = 0; i < EQUIP_SLOT_COUNT; ++i)
//		{
//			/*UGFxObject* obj = showPanel->GetObject(FString::Printf(TEXT("itemSlot_%d"), i));
//
//			if(obj != NULL)
//				GFXPlayer->SetItemSlotData(obj, NULL, ESPType_Charactor, E_CONSLOT_EQUIPPED, i);
//
//			UGFxObject* level_mc = showPanel->GetObject( FString::Printf( TEXT("slotLevel_%d"), i));
//			if( level_mc )
//			{
//				UGFxObject* lv_lab = level_mc->GetObject(TEXT("label_lv"));
//				if(lv_lab != NULL)
//					lv_lab->SetText(TEXT(""));
//			}*/
//		}
//	}
//}

//===============================================================================================================
//===============================================================================================================
void UMMOUIEquipSlotEnhancePanel::ASSelectEquipSlot(INT slotId)
{
	SelectEnhanceSlot( slotId );
}

//=====================================================================================================
void UMMOUIEquipSlotEnhancePanel::ASEnhanceEquipSlot()
{
	ToEnhanceCurSelectedSlot();
}

//=====================================================================================================
void UMMOUIEquipSlotEnhancePanel::EnhanceEquipSlotCancel()
{
	EquipSlotManager::getSingleton().ClearFlag(EES_INCONFIRM);
}

//=====================================================================================================
void UMMOUIEquipSlotEnhancePanel::EnhanceEquipSlotOK()
{
	EquipSlotManager::getSingleton().ClearFlag(EES_INCONFIRM);

	//
	RealEnhanceCurEquipSlot();
}

//=====================================================================================================
void UMMOUIEquipSlotEnhancePanel::ASShowTips_EquipSlot(INT id)
{

}

void UMMOUIEquipSlotEnhancePanel::ASHideTips_EquipSlot(INT id)
{

}

//=====================================================================================================
void UMMOUIEquipSlotEnhancePanel::ASShowTips_ActiveIcon(class UGFxObject* mc)
{
	ShowActiveTips();
}
//=====================================================================================================
void UMMOUIEquipSlotEnhancePanel::ASShowTips_EnhanceGuide( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	FString tipsText = FormatEnhanceGuideTips();
	mc->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

void UMMOUIEquipSlotEnhancePanel::ASHideTips_EnhanceGuide( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===============================================================================================================
//===============================================================================================================