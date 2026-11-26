#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOEquipRemakeMgr.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"

//=======================================================================================
void MMOEquipRemakeManager::Init()
{
	GambleTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("gamble.txt"),true);
}
//=======================================================================================
void MMOEquipRemakeManager::Shutdown()
{
}


void MMOEquipRemakeManager::OnRemake(UINT8 byItemSubType,INT nLevel)
{
	if (MMOFacade::getSingleton().IsBagFull())
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakeBagFull")),MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakeBagFull")); 
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakeBagFull")) );
		return ;
	}

	INT eGamble = GambleTemplateIndexDataProcessor::ItemSubTypeToGambleType((ITEM_SUBTYPE)byItemSubType);
	const GambleTemplateDataValue * pInfo = GambleTemplateDataIndexManager::getSingleton().FindData(GambleTemplateDataKey(eGamble,nLevel).nID);

	if (pInfo == NULL)
	{
		return ;
	}

	INT nItemID = pInfo->nCostItemID;
	INT itemCount = MMOFacade::getSingleton().GetItemCountInBag(nItemID,E_CONSLOT_DEFAULT);

	if (itemCount < pInfo->nCostItemCount)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakeNoMaterial")),MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakeNoMaterial")); 
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakeNoMaterial")) );
		return;
	}

	CMessageHandler::GetInstance().Send_Remake(eGamble,nLevel);
}

void MMOEquipRemakeManager::OnRemakeResultSuccess(INT nItemID)
{
	MMOGameEvent_RemakeResult evt(GameEvent_EquipRemakeResult,TRUE,nItemID);
	MMOUI::PostGameEvent(&evt);
}

void MMOEquipRemakeManager::OnRemakeResultFailed()
{
	MMOGameEvent_RemakeResult evt(GameEvent_EquipRemakeResult,FALSE,0);
	MMOUI::PostGameEvent(&evt);

	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakFailed")),MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakFailed")); 
	MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errEquipRemakFailed")) );
	return;
}

UBOOL MMOEquipRemakeManager::GetMaterialIDAndCost(UINT8 byItemSubType ,INT nLevel, INT& nItemID, INT& nCostCount)
{
	nItemID = 0;
	nCostCount = 0;
	INT eGamble = GambleTemplateIndexDataProcessor::ItemSubTypeToGambleType((ITEM_SUBTYPE)byItemSubType);
	const GambleTemplateDataValue * pInfo = GambleTemplateDataIndexManager::getSingleton().FindData(GambleTemplateDataKey(eGamble,nLevel).nID);
	if (pInfo != NULL)
	{
		nItemID = pInfo->nCostItemID;
		nCostCount = pInfo->nCostItemCount;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

INT GambleTemplateIndexDataProcessor::ItemSubTypeToGambleType(ITEM_SUBTYPE eItemSubType)
{
	switch (eItemSubType)
	{
	case ITEMSUBTYPE_EQUIP_WEAPON:			{return egtEquipWeapon;}
	case ITEMSUBTYPE_EQUIP_HAT:				{return egtEquipHat;}
	case ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR:	{return egtEquipShoulderArmor;}
	case ITEMSUBTYPE_EQUIP_CHEST_ARMOR:		{return egtEquipChestArmor;}
	case ITEMSUBTYPE_EQUIP_WRIST_ARMOR:		{return egtEquipWristArmor;}
	case ITEMSUBTYPE_EQUIP_BELT:			{return egtEquipBelt;}
	case ITEMSUBTYPE_EQUIP_SHOES:			{return egtEquipShoes;}
	case ITEMSUBTYPE_EQUIP_CLOAK:			{return egtEquipCloak;}
	case ITEMSUBTYPE_EQUIP_NECKLACE:		{return egtEquipNecklace;}
	case ITEMSUBTYPE_EQUIP_RING:			{return egtEquipRing;}
	case ITEMSUBTYPE_EQUIP_BRACELET:		{return egtEquipBracelet;}
	case ITEMSUBTYPE_INVALID:				{return egtInvalid;}
	default:								{return egtInvalid;}
	}
}


void GambleTemplateIndexDataProcessor::ParseData( GambleTemplateDataValue* obj, class CTabFile* dataRow, int rowIdx )
{	
	obj->nGambleType = TReadDataValue_ServerEnum<GambleTemplateDataIndexManager>(TEXT("GambleType"),dataRow,rowIdx,TEXT("EEGambleType"));
	obj->nGambleLevel = TReadDataValue_Int<GambleTemplateDataIndexManager>(TEXT("GambleLevel"), dataRow, rowIdx);
	obj->nCostItemID = TReadDataValue_Int<GambleTemplateDataIndexManager>(TEXT("CostItemID"), dataRow, rowIdx);
	obj->nCostItemCount = TReadDataValue_Int<GambleTemplateDataIndexManager>(TEXT("CostItemCount"), dataRow, rowIdx);

	GambleTemplateDataIndexManager::getSingleton().AddData(GambleTemplateDataKey(obj->nGambleType,obj->nGambleLevel).nID, obj);
}


