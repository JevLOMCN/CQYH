#include "stdafx.h"
#include "TipsFormatter.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "MMOBuffManager.h"
#include "MMOAttributeAccountManager.h"
#include "MMOEquipManager.h"
#include "MMOSkillManager.h"
#include "MMOHeavenManager.h"
#include "AbilityProcessor.h"
#include "MMOPlayerInfoManager.h"
#include "UStallManager.h"
#include "EquipSlotManager.h"
#include "MMOAuctionManager.h"
#include "TeamManager.h"
#include "MMOPowerManager.h"
#include "Shop.h"
#include "EquipDecomposeManager.h"
#include "MMOGuildData.h"
#include "MMOGoldStoreManager.h"
#include "MailManager.h"

TipsFormatter::TipsFormatterMap TipsFormatter::_tipsFormatterMap;


bool InitFormatterMap()
{
#define M_RGE_FMTTER(e, cls) { static cls g_##cls; TipsFormatter::RegTipsFormatter(e, &g_##cls); }

	M_RGE_FMTTER(TFT_ItemSlot, TipsFormatter_Item);
	M_RGE_FMTTER(TFT_SkillEquipped, TipsFormatter_SkillEquipped);
	M_RGE_FMTTER(TFT_SkillSlotTemplate, TipsFormatter_SkillSlotTemplate);
	M_RGE_FMTTER(TFT_SkillOfMyWithNext, TipsFormatter_SkillOfMyWithNext);
	M_RGE_FMTTER(TFT_SkillTemplate, TipsFormatter_SkillTemplate);
	M_RGE_FMTTER(TFT_SkillTemplateWithNext, TipsFormatter_SkillTemplateWithNext);
	M_RGE_FMTTER(TFT_Tab, TipsFormatter_Tab);
	M_RGE_FMTTER(TFT_Attribute, TipsFormatter_Attribute);
	M_RGE_FMTTER(TFT_GoldStore, TipsFormatter_GoldStore);
	M_RGE_FMTTER(TFT_MenuBar, TipsFormatter_MenuBar);
	M_RGE_FMTTER(TFT_AwardIcon, TipsFormatter_AwardIcon);
	M_RGE_FMTTER(TFT_Other, TipsFormatter_Other);
	M_RGE_FMTTER(TFT_RepairAll, TipsFormatter_RepairAll);
	M_RGE_FMTTER(TFT_BuffSlot, TipsFormatter_BuffSlot);
	M_RGE_FMTTER(TFT_MountSlot, TipsFormatter_MountSlot);
	M_RGE_FMTTER(TFT_RuneActive, TipsFormatter_RuneActive);
	M_RGE_FMTTER(TFT_RuneButton, TipsFormatter_RuneButton);
	M_RGE_FMTTER(TFT_PlainText ,TipsFormatter_PlainText);
	M_RGE_FMTTER(TFT_NormalText ,TipsFormatter_NormalText);
	M_RGE_FMTTER(TFT_FreeStyle ,TipsFormatter_FreeStyle);
	M_RGE_FMTTER(TFT_AutoSizeTips ,TipsFormatter_AutoSizeTips);
	M_RGE_FMTTER(TFT_AvartaRuneButton, TipsFormatter_AvartaRuneButton);
	M_RGE_FMTTER( TFT_ItemBar , TipsFormatter_ItemBar );
	M_RGE_FMTTER( TFT_LockedSlot , TipsFormatter_LockedSlot );
	M_RGE_FMTTER( TFT_HeavenGasSlot , TipsFormatter_HeavenGasSlot );
	M_RGE_FMTTER( TFT_EquipSlotEnhance , TipsFormatter_EquipSlotEnhance );
	M_RGE_FMTTER( TFT_Link , TipsFormatter_LinkTips );
	M_RGE_FMTTER( TFT_EquipDurability , TipsFormatter_EquipDurability );
	return true;
}

static bool g_InitTipsFormatter = InitFormatterMap();

void TipsFormatter::RegTipsFormatter( ItemSlotType TipsType, ITipsFormatter* fmt )
{
	_tipsFormatterMap[TipsType] = fmt;
}

ITipsFormatter* TipsFormatter::GetTipsFormatter( ItemSlotType TipsType )
{
	TipsFormatterMap::iterator i = _tipsFormatterMap.find(TipsType);
	if(i != _tipsFormatterMap.end())
	{
		return i->second;
	}

	return 0;
}

void TipsFormatter::FormatTips(ItemSlotType TipsType, class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	ITipsFormatter* fmt = TipsFormatter::GetTipsFormatter(TipsType);
	if(fmt)
	{
		fmt->FormatTips(SrcObj, TipsPanel, SubPanel );
	}
	else
	{
		warnf(NAME_Error, TEXT("[TipsFormatter::FormatTips] formater not found:%d"), (INT)TipsType);
	}

}

//=============================================================================
//many places use the same CommonTip control for convenience; AutoResize is implemented in AS
void FillConstWidthCommonTip( class UGFxObject * commonTip, const FString &strContent, INT styleType = CM_TIPS_Style1 )
{
	if( commonTip == NULL )
	{
		return;
	}
	//
	TArray<struct FASValue> arguments;

	FASValue asValContent(EC_EventParm);
	asValContent.Type = AS_String;
	asValContent.S = strContent;
	arguments.AddItem(asValContent);

	FASValue asValTipsShowStyle(EC_EventParm);
	asValTipsShowStyle.Type = AS_Int;
	asValTipsShowStyle.I = styleType;
	arguments.AddItem(asValTipsShowStyle);

	commonTip->Invoke( TEXT("SetConstWidthTips"), arguments );
}

//
void FillAutoSizeCommonTip( class UGFxObject * commonTip, const FString &strContent )
{
	if( commonTip == NULL )
	{
		return;
	}
	//
	TArray<struct FASValue> arguments;

	FASValue asVal(EC_EventParm);
	asVal.Type = AS_String;
	asVal.S = strContent;

	arguments.AddItem(asVal);
	commonTip->Invoke( TEXT("SetAutoSizeContent"), arguments );
}
//=============================================================================

//------------------------------------------------------------------------
//TipsFormatter_Item UMMOUITips*
//------------------------------------------------------------------------
void TipsFormatter_Item::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	LOG_PROCESS_ERROR(SrcObj && TipsPanel && SubPanel);

	INT ContainerID;
	INT SlotID;
	INT SlotType;
	INT ItemId;
	INT tempID;

	ContainerID = SrcObj->GetInt(TEXT("containerID"));
	SlotID = SrcObj->GetInt(TEXT("slotID"));
	SlotType = SrcObj->GetInt(TEXT("SlotType"));
	ItemId = SrcObj->GetInt(TEXT("itemid"));
	tempID = SrcObj->GetInt(TEXT("ItemTemplateID"));
	if( tempID <= 0 )
	{ 
		tempID = SrcObj->GetInt(TEXT("itemIndex"));
	}

	ItemInfo* itemInfo = NULL;
	switch( SlotType )
	{
	case ESPType_ViewProfile:
		{
			if( ContainerID == E_CONSLOT_PROFILE )
			{
				PlayerDetailInfo* detailInfo = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
				if(detailInfo)
				{
					itemInfo = &detailInfo->EquipList[ItemId];
				}
			}
			else
			{
				itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
			}
		}
		break;
	case ESPType_ShopSell:
		{
			itemInfo = MMOFacade::getSingleton().FindSoldItem(SlotID);
		}
		break;
	case ESPType_AuctionItem:
		{
			itemInfo = MMOAuctionManager::getSingleton().GetAuctionItemInfo(SlotID);
			MMOPowerManager::getSingleton().UpdateEquipPower(static_cast<FClientItemInfo*>(itemInfo));
		}
		break;
	case ESPType_TeamAuctionList:
		{
			INT nSlotIndex = SrcObj->GetInt(TEXT("slotIndex"));
			TArray<MMOTeamAuctionItem>& auctionItemList = TeamManager::getSingleton().GetTeamAuctionItemList();
			if( auctionItemList.Num() > nSlotIndex &&nSlotIndex >= 0)
			{
				itemInfo = &auctionItemList(nSlotIndex).Item;
			}
		}
		break;
	case ESPType_TeamAuctionPanel:
		{
			MMOTeamAuctionItem* teamAuctionItem = TeamManager::getSingleton().GetCurAuctionItem();
			if(teamAuctionItem)
			{
				itemInfo = &teamAuctionItem->Item;
			}
		}
		break;
	case ESPType_EquipCompose:
		{
			itemInfo = MMOFacade::getSingleton().FindFirstItemPosInContainer(tempID, ContainerID, SlotID);
		}
		break;
	case ESPType_SkillBar:
		{
			INT trueBagSlotID = ContainerID;
			itemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_DEFAULT, trueBagSlotID );
		}
		break;
	case ESPType_GuildWarehouse:
		{
			//itemInfo = MMOFacade::getSingleton().FindItem( ESPType_GuildWarehouse + E_CONSLOT_MAX, SlotID );

			UMMOUIGuildWarehouse* panel = (UMMOUIGuildWarehouse*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Warehouse);
			if (panel && panel->IsVisible)
			{
				itemInfo = MMOGuildData::getSingleton().GetWarehouseItem(panel->_CurPage, SlotID);
			}
		}
		break;
	case ESPType_ReadMail:
		{
			ClientMailDetail* mail = MailManager::getSingleton().GetCurReadMail();
			if (mail && mail->byBufferAttach > 0 )
			{
				ItemInfo iteminfoTemp;
				iteminfoTemp.Load(mail->acAttachItem);
				itemInfo = &iteminfoTemp;
			}
			else
			{
				itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
			}
		}
		break;
	default:
		{
			itemInfo = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
		}
		break;
	}

	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	LOG_PROCESS_ERROR(GFXPlayer);
	if( itemInfo && 0 == tempID )
	{
		tempID = itemInfo->m_nTemplateID;
	}

	UItem* item = MMOFacade::getSingleton().FindItemTemplate(tempID);
	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
	LOG_PROCESS_ERROR(item && itemObj);
	FormatItemTip( GFXPlayer, itemObj, item, itemInfo, ItemId, SlotType, FALSE );

	//rune item shows ability
	UBOOL isSkillItem = FALSE;//( item->ItemType == TEXT("ITEMTYPE_RUNE") );
	isSkillItem |= ( item->ItemType == TEXT("ITEMTYPE_SPELLBOOK") );
	if( isSkillItem )
	{
		if( item->GetToUseAbility() != NULL )
		{
			TipsFormatter_SkillTemplate::FormatAppendSkillTipsWithNoMainSkillLevel( item->GetToUseAbility(), TipsPanel, SubPanel );
			TipsPanel->ShowNextSkillTip = TRUE;
		}
	}
	//compose item blueprint
	else if( item->ItemType == TEXT("ITEMTYPE_FORMULA") ){
		EquipComposeTemplate& toComposeTemplate = EquipDecomposeManager::getSingleton().GetComposeTemplateById( tempID );
		UItem* pComposeItem = MMOFacade::getSingleton().FindItemTemplate( toComposeTemplate.equipId );

		if( pComposeItem != NULL ){
			UGFxObject* composeItemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
			if( composeItemObj != NULL ){
				FormatItemTip( GFXPlayer, composeItemObj, pComposeItem, NULL, toComposeTemplate.equipId, SlotType, TRUE );
				TipsPanel->eventCallUpdateItemTipCompareData( 0, composeItemObj );
			}
		}
	}

	//if there is a Slot tip, cancel it now
	if( (ESPType_Bag == SlotType && E_CONSLOT_DEFAULT == ContainerID)  || (ESPType_BagResource == SlotType && E_CONSLOT_RESOURCE == ContainerID))
	{
		FClientItemInfo *pClientItem = (FClientItemInfo*)itemInfo;
		if( pClientItem && pClientItem->NeedHintMode )
		{
			pClientItem->NeedHintMode = ItemSlotHint_Normal;
			MMOGameEvent_BagSlotHintChange msg( ContainerID , SlotID , pClientItem );
			MMOUI::PostGameEvent(&msg);
			//MMOFacade::getSingleton().HintItem( ContainerID, SlotID, FALSE );
		}		
	}

	UEquip* equip = Cast<UEquip>( item );
	if( equip == NULL )
	{
		goto Exit1;
	}

	//equipment comparison
	if( IsItemTipsNeedCompareToCurEquipped( SlotType ) )
	{
		INT destCon = E_CONSLOT_MAX, destSlot = 0;
		if(equip->GetEquipAppearenceTemplate())
		{
			MMOUI::GetEquippedSlotByEquipType( equip->GetEquipAppearenceTemplate()->EquipType, destCon, destSlot);
		}

		if( destCon == E_CONSLOT_EQUIPPED )
		{
			const INT MaxCompareCnt = 2;
			ItemInfo* toCompareItemInfo[MaxCompareCnt] = { NULL, NULL };
			toCompareItemInfo[0] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, destSlot );

			//
			INT curCompareCnt = 0;
			if( toCompareItemInfo[0] != NULL ){
				++curCompareCnt;
			}

			if( destSlot == E_NES_FINGER_L ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_FINGER_R );
			}
			if( destSlot == E_NES_FINGER_R ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_FINGER_L );
			}
			if( destSlot == E_NES_WRIST_L ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_WRIST_R );
			}
			if( destSlot == E_NES_WRIST_R ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_WRIST_L );
			}

			//
			if( curCompareCnt > 0 ){
				itemObj->SetBool(TEXT("bShowItemEffect"), FALSE);
				//TipsPanel->ShowCompareTip = TRUE;
			}

			for( INT i = 0; i < curCompareCnt; ++i ){
				if( toCompareItemInfo[i] == NULL ){
					continue;
				}
				item = MMOFacade::getSingleton().FindItemTemplate( toCompareItemInfo[i]->m_nTemplateID );

				UGFxObject* compareItemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
				FormatItemTip( GFXPlayer, compareItemObj, item, toCompareItemInfo[i], toCompareItemInfo[i]->m_nTemplateID, SlotType, TRUE );
				TipsPanel->eventCallUpdateItemTipCompareData( i, compareItemObj );
			}
		}
	}
Exit1:
	TipsPanel->eventCallUpdateItemTipData( itemObj );

Exit0:
	return;
}

//
UBOOL TipsFormatter_Item::IsItemTipsNeedCompareToCurEquipped( INT tipsFromSlotType )
{
	UBOOL isNeed = FALSE;
	switch( tipsFromSlotType )
	{
	case ESPType_SkillBar:
	case ESPType_ItemBar:
	case ESPType_Charactor:
		{
			isNeed = FALSE;
		}
		break;
	case ESPType_Bag:
	case ESPType_TradeP2P:
	case ESPType_Quest:
	case ESPType_EquipGainHintBox:
	case ESPType_AuctionSell:
	case ESPType_AuctionItem:
	case ESPType_Warehouse:
	case ESPType_GuildWarehouse:
	case ESPType_Shop:
	case ESPType_ShopSell:
	case ESPType_TeamAuctionPanel:
	case ESPType_TeamAuctionList:
	case ESPType_SoulEmbed:
	case ESPType_SoulUnEmbed:
	case ESPType_StallBuy:
	case ESPType_WeaponSkill:
	case ESPType_WeaponSkillRplc:
	case ESPType_NPCDialog:
	case ESPType_BagResource:

		{
			isNeed = TRUE;
		}
		break;
	default:
		{
			isNeed = TRUE;
		}
		break;
	}

	return isNeed;
}
//------------------------------------------------------------------------
//FormatItemTip
//------------------------------------------------------------------------
void TipsFormatter_Item::FormatItemTip(UGFxMoviePlayer* pGFXPlayer, UGFxObject* itemObj, UItem* item, ItemInfo* itemInfo, INT ItemId, INT SlotType, UBOOL isCompareTip)
{
	FString itemColorName = TEXT("");
	FString itemName = TEXT("");

	LOG_PROCESS_ERROR(itemObj && item);
	LOG_PROCESS_ERROR(pGFXPlayer);

	itemObj->SetBool(TEXT("IsCompare"), isCompareTip);

	// equipment name
	if( itemInfo != NULL && itemInfo->m_byIdentified == IDENTIFY_TYPE_LEVEL_UP ){
		itemName = TEXT("*");
	}
	itemName += UITextParser::HTMLFormatItemName( item );
	//equipment upgrade level
	//if( itemInfo != NULL && itemInfo->m_byLevel > 0 ){
	//	itemName += TEXT("+");
	//	itemName += appItoa( itemInfo->m_byLevel );
	//}

	itemObj->SetString( TEXT("ItemName"), itemName );
	//whether equipped

	if(isCompareTip && itemInfo && E_CONSLOT_EQUIPPED == itemInfo->m_byConSlot)
		itemObj->SetBool(TEXT("IsEquiped"), TRUE);
	else
		itemObj->SetBool(TEXT("IsEquiped"), FALSE);

	//equipment type
	itemObj->SetString( TEXT("PartName"), UITextParser::HTMLFormatItemType( item ) );

	//item color
	itemColorName = FString( TEXT("lv") ) + appItoa( item->ItemColor );
	itemObj->SetString( TEXT("ColorName"), itemColorName );

	//usable class
	itemObj->SetString( TEXT("Profession"), UITextParser::HTMLFormatItemRequireProfession( item ) );
	//binding info
	itemObj->SetString( TEXT("Bind"), UITextParser::HTMLFormatItemBindType( item, itemInfo ) );
	//item level
	itemObj->SetString( TEXT("ItemLevel"), UITextParser::HTMLFormatItemLevel( item ));
	////usage level
	//itemObj->SetString( TEXT("RequireLevel"), UITextParser::HTMLFormatItemRequireLevel( item ));
	//usage restrictions (including level)
	itemObj->SetString( TEXT("RequireLevel"), UITextParser::HTMLFormatItemRequireAll( item ));
	//weight attribute
	itemObj->SetString( TEXT("WeightParameter"), UITextParser::HTMLFormatItemWeightAttribute( item ) ); 
	//purity (ore only)
	itemObj->SetString( TEXT("PurityParameter"), UITextParser::HTMLFormatItemPurityAttribute( item ) );
	//gender
	itemObj->SetString(TEXT("Gender"), UITextParser::HTMLFormatItemGender( item ));
	//whether decomposable
	itemObj->SetString(TEXT("ForbidDecompose"), UITextParser::HTMLFormatItemDecompose( item ));

	UEquip* equip = Cast<UEquip>( item );
	if( equip )
	{
		//AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		//INT playerClassID = ( (mainPlayer != NULL) ? mainPlayer->GetClassId() : 0 );
		// equipment Icon
		if( SlotType == ESPType_ViewProfile ){
			PlayerDetailInfo* detailInfo = MMOPlayerInfoManager::getSingleton().FindCurPlayerDetailInfo();
			INT playerSex = ( detailInfo != NULL ? detailInfo->Sex : EGT_Male );
			INT playerActClass = ( detailInfo != NULL ? detailInfo->ClassId : EPCT_PlayerClassA );
			itemObj->SetString( TEXT("IconName"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item, itemInfo, playerActClass ) );
		}
		else{
			itemObj->SetString( TEXT("IconName"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item, itemInfo ) );
		}
		
		//Whether it is equipment
		itemObj->SetBool( TEXT("IsEquip"), TRUE );
		//combat power
		if( SlotType != ESPType_EquipCompose 
			&& SlotType != ESPType_TeamAuctionPanel 
			&&SlotType != ESPType_TeamAuctionList 
			//&& SlotType != ESPType_ChatMsgDisplay 
			&& SlotType != ESPType_Shop
			&& SlotType != ESPType_ShopSell
			&& SlotType != ESPType_ReadMail)
		{
			itemObj->SetString(TEXT("Power"), UITextParser::HTMLFormatItemPower(itemInfo));
		}
		else
		{
			itemObj->SetString(TEXT("Power"), UITextParser::HTMLFormatItemPower(equip));
		}

		//attributes
		if(itemInfo && itemInfo->m_byIdentified)
		{
			FString mixParaText;
			FString randParaText;
			UITextParser::HTMLFormatItemBaseAndRandomAttributes( equip, itemInfo, mixParaText, randParaText );
			//main attributes, displayed mixed with random attributes
			itemObj->SetString( TEXT("MainParameter"), mixParaText );
			//Random attribute
			itemObj->SetString( TEXT("RandomParameter"), randParaText );
			itemObj->SetBool(TEXT("bUnIdentified"), FALSE);
		}
		else{
			//main attributes
			itemObj->SetString( TEXT("MainParameter"), UITextParser::HTMLFormatItemAttributes(equip, itemInfo));
			//Random attribute
			if(itemInfo) // only show if not template data
			{
				itemObj->SetBool(TEXT("bUnIdentified"), TRUE);
				itemObj->SetString(TEXT("unIdentifiedText"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ItemTips.UnIdentify") ) );
			}
			else
			{
				if( SlotType == ESPType_EquipCompose ){
					itemObj->SetBool(TEXT("bUnIdentified"), TRUE);
					itemObj->SetString(TEXT("unIdentifiedText"), MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.ItemTips.UnIdentify") ) );
				}
				else{
					itemObj->SetBool(TEXT("bUnIdentified"), FALSE);
				}
			}
		}
		//luck value attribute
		itemObj->SetString( TEXT("LuckyParameter"), UITextParser::HTMLFormatItemLuckyAttribute( equip, itemInfo ) ); 
		//enhancement attributes
		itemObj->SetString( TEXT("EnhanceParameter"), UITextParser::HTMLFormatItemEnhanceAttribute( equip, itemInfo ) ); 
		//Inscription
		UGFxObject* pSkillDesc = pGFXPlayer->CreateArray();
		UGFxObject* pSkillName = pGFXPlayer->CreateArray();
		UGFxObject* pSkillIconName = pGFXPlayer->CreateArray();
		BOOL ShowSkill = UITextParser::HTMLFormatItemWeaponSkillAttribute( equip, itemInfo, pSkillName, pSkillDesc, pSkillIconName );
		itemObj->SetBool(TEXT("ShowSkill"), ShowSkill);
		if( ShowSkill )
		{
			itemObj->SetObject(TEXT("SkillIconName"), pSkillIconName);
			itemObj->SetObject(TEXT("SkillName"), pSkillName);
			itemObj->SetObject(TEXT("SkillDesc"), pSkillDesc);
		}

		//Spirit fusion attribute
		UGFxObject* pSoulIconType = pGFXPlayer->CreateArray();
		UGFxObject* pSoulIconName = pGFXPlayer->CreateArray();
		UGFxObject* pSoulName = pGFXPlayer->CreateArray();
		BOOL ShowSoul = UITextParser::HTMLFormatItemSoulAttribute(item, itemInfo, pSoulName, pSoulIconType, pSoulIconName);
		itemObj->SetBool(TEXT("ShowSoul"), ShowSoul);
		if(ShowSoul)
		{
			itemObj->SetObject(TEXT("SoulIconType"), pSoulIconType);
			itemObj->SetObject(TEXT("SoulIconName"), pSoulIconName);
			itemObj->SetObject(TEXT("SoulName"), pSoulName);
		}
		//set attributes
		const EquipSuit* pSuit = MMOEquipManager::getSingleton().FindEquipSuitByPartOfSuit(item->ItemId);
		itemObj->SetBool(TEXT("bShowSuit"), (NULL != pSuit));
		if(NULL != pSuit)
			itemObj->SetString(TEXT("SuitParameter"), UITextParser::HTMLFormatEquipSuitAttribute(equip));
		//weather attributes
		itemObj->SetString(TEXT("WeatherParameter"), UITextParser::HTMLFormatEquipWeatherAttribute(equip));
		//Signature
		itemObj->SetString(TEXT("SignParameter"), UITextParser::HTMLFormatEquipSignAttribute(itemInfo));
		//repair state
		itemObj->SetString(TEXT("RepairState"), UITextParser::HTMLFormatEquipRepairState(equip, itemInfo));
		//upgrade points
		if( equip->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON") ){
			itemObj->SetString(TEXT("LevelUpPoint"), UITextParser::HTMLFormatItemLevelUpPoints(itemInfo));
		}
		else{
			itemObj->SetString(TEXT("LevelUpPoint"), TEXT("") );
		}
		//whether skill book is learned
		itemObj->SetString( TEXT("IsGotten"), TEXT("") );
	}
	else
	{
		//Not equipment
		itemObj->SetString( TEXT("IconName"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item, itemInfo ) );
		itemObj->SetBool( TEXT("IsEquip"), FALSE );
		itemObj->SetBool(TEXT("bUnIdentified"), FALSE);
		itemObj->SetString(TEXT("Power"), TEXT(""));
		itemObj->SetBool(TEXT("bShowSuit"), FALSE);
		itemObj->SetString( TEXT("RepairState"), TEXT("") );
		itemObj->SetString(TEXT("LevelUpPoint"), TEXT("") );
		//whether skill book is learned
		itemObj->SetString( TEXT("IsGotten"), UITextParser::HTMLFormatItemGottenType( item ) );
	}

	//additional attributes such as item description related info
	itemObj->SetString( TEXT("AddParameter"), UITextParser::HTMLFormatItemDescription( item ) ); 

	switch( SlotType )
	{
	case ESPType_StallBuy:
		{
			//reset price
			StallItemInfo stallItemInfo;
			if (UStallManager::getSingleton().GetStallBuyItem(ItemId, stallItemInfo))
			{
				itemObj->SetString( TEXT("Money"), UITextParser::HTMLFormatStallItemPrice(stallItemInfo) );
			}
			else
			{
				itemObj->SetString( TEXT("Money"), TEXT("") );
			}
		}
		break;
	case ESPType_StallSetting:
		{
			StallItemInfo stallItemInfo;
			if (UStallManager::getSingleton().GetStallSellItem(ItemId, stallItemInfo))
			{
				itemObj->SetString(TEXT("Money"), UITextParser::HTMLFormatStallItemPrice(stallItemInfo));
			}
			else{
				itemObj->SetString( TEXT("Money"), TEXT("") );
			}
		}
		break;
	case ESPType_MallStore:
		{
			//mall items do not show price
			itemObj->SetString( TEXT("Money"), TEXT(""));
		}
		break;
	case ESPType_Shop:
		{
			//get goods info from goods
			ServerShopItemData* shopItem = ShopManager::getSingleton().GetCurShopItemByItemTempId( item->GetTemplateID() );
			FString reputationText = UITextParser::HTMLFormatItemRequireReputation( shopItem );
			FString guildText = UITextParser::HTMLFormatItemRequireGuild( shopItem );
			if( !guildText.IsEmpty() ){
				itemObj->SetString( TEXT("ReputationText") , reputationText );
			}
			else{
				itemObj->SetString( TEXT("ReputationText") , reputationText + TEXT("<br>") + guildText );
			}
			itemObj->SetString( TEXT("Money"),  UITextParser::HTMLFormatItemPrice( item, NULL ) );
		}
		break;
	default:
		{
			//repair state, show repair price
			if( itemInfo && equip ){
				UBOOL isCurRepairing = MMOFacade::getSingleton().GetIsRepairing();
				UBOOL isCurSpecialRepairing = MMOFacade::getSingleton().GetIsSpecialRepairing();

				FString repairPriceText;
				if( isCurRepairing || isCurSpecialRepairing ){
					if( itemInfo->m_nMaxDur <= 0 || !equip->CanRepair ){
						repairPriceText = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( TEXT("AlertColor") ) );
						repairPriceText += MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.Item.RepairCannot") );
						repairPriceText += UITextParser::HTMLEndFont();
					}
					else{
						repairPriceText = MMOUI::ParseLocalizedPropertyPath( TEXT("Properties.Item.RepairPrice") );
						repairPriceText += UITextParser::HTMLMoneyIconString( MONEYTYPE_02, MMOEquipManager::getSingleton().GetEquipRepairCost( equip, itemInfo, isCurSpecialRepairing ) );
					}
					itemObj->SetString( TEXT("Money"), repairPriceText );
				}
				else{
					itemObj->SetString( TEXT("Money"), UITextParser::HTMLFormatItemPrice( item, itemInfo ) );
				}
			}
			else{
				//gold type, sell price
				itemObj->SetString( TEXT("Money"), UITextParser::HTMLFormatItemPrice( item, itemInfo ) );
			}
		}
		break;
	}

	// show repair price in repair state
	if( itemInfo && MMOFacade::getSingleton().GetIsRepairing() )
	{
		itemObj->SetString(TEXT("ForbidSell"), TEXT(""));
	}
	else // template data can only be displayed, not sold
	{
		itemObj->SetString(TEXT("ForbidSell"), UITextParser::HTMLFormatItemForbidSell(item,itemInfo));
	}

	itemObj->SetString(TEXT("ForbidTrade"), TEXT(""));//UITextParser::HTMLFormatItemForbidTrade(item));

	//current durability
	itemObj->SetString( TEXT("Duration"), UITextParser::HTMLFormatItemDuration( item, itemInfo ));

Exit0:
	if(itemObj)
		itemObj->SetBool(TEXT("bShowItemEffect"), FALSE);
	return;
}
//------------------------------------------------------------------------
//TipsFormatter_ItemBar
//------------------------------------------------------------------------
void TipsFormatter_ItemBar::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer;
	if( !GFXPlayer ){
		return;
	}

	INT slotContainID = SrcObj->GetInt(TEXT("itemIndex"));
	INT itemBarContainType = SrcObj->GetInt( TEXT("EquipID") );

	if( itemBarContainType == ESBarSlotTp_Item ){
		FString AddAttri = TEXT("");

		UItem* item = MMOFacade::getSingleton().FindItemTemplate(slotContainID);

		if( !item || !GFXPlayer )
		{
			return;
		}

		//pass data into Tip DataObj
		UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
		FormatItemTip( GFXPlayer, itemObj, item, NULL );

		TipsPanel->eventCallUpdateItemTipData( itemObj );
	}
	else if( itemBarContainType == ESBarSlotTp_Skill ){
		AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
		if( Player == NULL ){
			return;
		}

		UMMOAbility* skill = Player->GetMainAbilityById( slotContainID );
		TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel);
	}
	else if( itemBarContainType == ESBarSlotTp_Mount)
	{
		if(SrcObj){
			INT MountID	= SrcObj->GetInt(TEXT("itemTemplateID")); 
			MountData* pMountData = MMOFacade::getSingleton().GetMountData(MountID);

			UGFxObject* mountObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.MountTipData"));
			if(pMountData && mountObj){
				mountObj->SetString(TEXT("MountName"),pMountData->Name);
				mountObj->SetString(TEXT("IconName"),pMountData->IconName);
				mountObj->SetString(TEXT("ColorName"), FString::Printf(TEXT("lv%d"), pMountData->Quality ) );
				mountObj->SetString(TEXT("MountDescribe"),pMountData->Script);
				TipsPanel->eventCallUpdateMountTipData( mountObj );
			}
		}
	}


}

//------------------------------------------------------------------------
//TipsFormatter_LinkTips
//------------------------------------------------------------------------
void TipsFormatter_LinkTips::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	INT itemID = 0;

	//
	ItemInfo* itemInfo = NULL;
	FClientItemInfo newItemInfo;

	INT linkType = appAtoi( *SrcObj->GetElementString( 1 ) );
	INT fromPanelType = ESPType_Invalid;
	switch( linkType )
	{
	case ELinkType_ItemNav:
		{
			fromPanelType = ESPType_QuestTraceDisplay;

			INT itemIdStartIndex = 2;
			itemID = appAtoi( *SrcObj->GetElementString( itemIdStartIndex ) );
		}
		break;
	case ELinkType_ItemTip:
	case ELinkType_ChatItemtTip:
		{
			itemID = appAtoi(*SrcObj->GetElementString(2));
			fromPanelType = ESPType_ChatMsgDisplay;

			INT ItemInfoStartIndex = 3;
			newItemInfo.FromGFXStringArray( SrcObj, ItemInfoStartIndex );
			itemInfo = &newItemInfo;

			if( itemInfo && 0 == itemID )
			{
				itemID = itemInfo->m_nTemplateID;
			}
		}
		break;
	case ELinkTpye_AuctionItem:
		{
			itemID = appAtoi(*SrcObj->GetElementString(2));
			fromPanelType = ESPType_ChatMsgDisplay;

			INT ItemInfoStartIndex = 4;
			newItemInfo.FromGFXStringArray( SrcObj, ItemInfoStartIndex );
			itemInfo = &newItemInfo;

			if( itemInfo && 0 == itemID )
			{
				itemID = itemInfo->m_nTemplateID;
			}
		}
		break;

	}

	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	LOG_PROCESS_ERROR(GFXPlayer);

	UItem* item = MMOFacade::getSingleton().FindItemTemplate( itemID );

	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
	LOG_PROCESS_ERROR(item && itemObj);

	//
	FormatItemTip( GFXPlayer, itemObj, item, itemInfo, itemID, fromPanelType, FALSE );
	TipsPanel->eventCallUpdateItemTipData( itemObj );

	UEquip* equip = Cast<UEquip>( item );
	//equipment comparison
	if( equip != NULL )
	{
		INT destCon = E_CONSLOT_MAX, destSlot = 0;
		if(equip->GetEquipAppearenceTemplate())
		{
			MMOUI::GetEquippedSlotByEquipType( equip->GetEquipAppearenceTemplate()->EquipType, destCon, destSlot);
		}

		if( destCon == E_CONSLOT_EQUIPPED )
		{
			const INT MaxCompareCnt = 2;
			ItemInfo* toCompareItemInfo[MaxCompareCnt] = { NULL, NULL };
			toCompareItemInfo[0] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, destSlot );

			//
			INT curCompareCnt = 0;
			if( toCompareItemInfo[0] != NULL ){
				++curCompareCnt;
			}

			if( destSlot == E_NES_FINGER_L ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_FINGER_R );
			}
			if( destSlot == E_NES_FINGER_R ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_FINGER_L );
			}
			if( destSlot == E_NES_WRIST_L ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_WRIST_R );
			}
			if( destSlot == E_NES_WRIST_R ){
				toCompareItemInfo[curCompareCnt++] = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_WRIST_L );
			}

			//
			if( curCompareCnt > 0 ){
				itemObj->SetBool(TEXT("bShowItemEffect"), FALSE);
				//TipsPanel->ShowCompareTip = TRUE;
			}

			for( INT i = 0; i < curCompareCnt; ++i ){
				if( toCompareItemInfo[i] == NULL ){
					continue;
				}
				item = MMOFacade::getSingleton().FindItemTemplate( toCompareItemInfo[i]->m_nTemplateID );

				UGFxObject* compareItemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
				FormatItemTip( GFXPlayer, compareItemObj, item, toCompareItemInfo[i], toCompareItemInfo[i]->m_nTemplateID, fromPanelType, TRUE );
				TipsPanel->eventCallUpdateItemTipCompareData( i, compareItemObj );
			}
		}
	}

	//rune item shows ability
	UBOOL isSkillItem = FALSE;//( item->ItemType == TEXT("ITEMTYPE_RUNE") );
	isSkillItem |= ( item->ItemType == TEXT("ITEMTYPE_SPELLBOOK") );
	if( isSkillItem )
	{
		if( item->GetToUseAbility() != NULL )
		{
			TipsFormatter_SkillTemplate::FormatAppendSkillTipsWithNoMainSkillLevel( item->GetToUseAbility(), TipsPanel, SubPanel );
			TipsPanel->ShowNextSkillTip = TRUE;
		}
	}
	//compose item blueprint
	else if( item->ItemType == TEXT("ITEMTYPE_FORMULA") ){
		EquipComposeTemplate& toComposeTemplate = EquipDecomposeManager::getSingleton().GetComposeTemplateById( itemID );
		UItem* pComposeItem = MMOFacade::getSingleton().FindItemTemplate( toComposeTemplate.equipId );

		if( pComposeItem != NULL ){
			UGFxObject* composeItemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
			if( composeItemObj != NULL ){
				FormatItemTip( GFXPlayer, composeItemObj, pComposeItem, NULL, toComposeTemplate.equipId, fromPanelType, TRUE );
				TipsPanel->eventCallUpdateItemTipCompareData( 0, composeItemObj );
			}
		}
	}

Exit0:
	return;
}

//------------------------------------------------------------------------
//TipsFormatter_SkillEquipped
//------------------------------------------------------------------------
void TipsFormatter_SkillEquipped::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	//check(NULL != SrcObj && NULL != TipsPanel && SubPanel != NULL);
	if(!(NULL != SrcObj && NULL != TipsPanel && SubPanel != NULL))
		return;

	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( !GFXPlayer )
		return;

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if( !Player )
		return;

	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));
	INT uniqueSkillID = SrcObj->GetInt(TEXT("itemIndex"));
	UINT skillID;
	UINT skillLv;
	UINT skillRuneID;
	MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );
	UMMOAbility* skill = Player->GetEquipedAbilityById( skillID );

	TipsFormatter_SkillTemplate::FormatSkillTipsWithEquippedSkillInfo( skill, TipsPanel, SubPanel);
}

//------------------------------------------------------------------------
//TipsFormatter_SkillSlotTemplate
//------------------------------------------------------------------------
void TipsFormatter_SkillSlotTemplate::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( NULL == GFXPlayer || NULL == TipsPanel ) return;

	UINT lookupKey = SrcObj->GetInt( TEXT("itemIndex") );//MMO::Facade::CombinateID( skillID, skillLv, runeSlotID );
	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));
	UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate( lookupKey );

	if(NULL != skill)
	{
		INT panelId = SrcObj->GetInt( TEXT("iteminfoid") );
		if( panelId == ESPType_ViewProfile )
		{
			TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel );
		}
		else
		{
			TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel );
		}
	}
	else
	{
		TipsPanel->SetVisible(FALSE);
	}
}

void TipsFormatter_SkillOfMyWithNext::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( NULL == GFXPlayer || NULL == TipsPanel ) return;

	UINT lookupKey = SrcObj->GetInt( TEXT("itemIndex") );//MMO::Facade::CombinateID( skillID, skillLv, runeSlotID );
	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));
	UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate(lookupKey);
	if( skill == NULL )
	{
		warnf(NAME_Error, TEXT("[TipsFormatter_SkillOfMyWithNext::FormatTips] skill not found(ID:%d)"), lookupKey );
		TipsPanel->SetVisible( FALSE );
		return;
	}

	//
	TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel);

	//next level ability
	TipsFormatter_SkillTemplate::FormatAppendSkillTipsForSkillNextLv( skill, TipsPanel, SubPanel );
}


//------------------------------------------------------------------------
//TipsFormatter_SkillTemplate
//------------------------------------------------------------------------
void TipsFormatter_SkillTemplate::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( NULL == GFXPlayer || NULL == TipsPanel ) return;

	UINT lookupKey = SrcObj->GetInt( TEXT("UniqueSkillID") );//MMO::Facade::CombinateID( skillID, skillLv, runeSlotID );
	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));
	UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate( lookupKey );

	if(NULL != skill)
	{
		INT panelId = SrcObj->GetInt( TEXT("iteminfoid") );
		if( panelId == ESPType_ViewProfile )
		{
			TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel );
		}
		else
		{
			TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel );
		}
	}
	else
	{
		TipsPanel->SetVisible(FALSE);
	}



}

//------------------------------------------------------------------------
void TipsFormatter_SkillTemplate::FormatSkillTips(class UMMOAbility* skill, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( !skill || !GFXPlayer || !TipsPanel || !SubPanel)
	{
		SubPanel->SetVisible(FALSE);
		return;
	}

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("Object"));
	if( dataObj )
	{
		//
		_FormatTipsText( dataObj, skill );

		//whether next level
		dataObj->SetString(TEXT("NextLevel"), TEXT("") );		            

		//whether to show next level ability requirements
		dataObj->SetBool(TEXT("HasUpLevNeed"), FALSE);
	}

	TipsPanel->eventCallUpdateSkillTipData(dataObj);
}
//------------------------------------------------------------------------
void TipsFormatter_SkillTemplate::FormatSkillTipsWithNoMainSkillLevel(class UMMOAbility* skill, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( !skill || !GFXPlayer || !TipsPanel || !SubPanel)
	{
		SubPanel->SetVisible(FALSE);
		return;
	}

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("Object"));
	if( dataObj )
	{
		//
		_FormatTipsText( dataObj, skill );
		//
		dataObj->SetString(TEXT("NextLevel"), TEXT("") );
		dataObj->SetString( TEXT("CombineLevel"), TEXT("") );
		dataObj->SetString( TEXT("SkillLevel"), TEXT("") );

		//whether next level
		dataObj->SetString(TEXT("NextLevel"), TEXT("") );		            

		//whether to show next level ability requirements
		dataObj->SetBool(TEXT("HasUpLevNeed"), FALSE);
	}

	TipsPanel->eventCallUpdateSkillTipData(dataObj);
}
//------------------------------------------------------------------------
void TipsFormatter_SkillTemplate::FormatAppendSkillTipsForSkillNextLv(UMMOAbility* originSkill, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( !originSkill || !GFXPlayer || !TipsPanel || !SubPanel )
	{
		SubPanel->SetVisible(FALSE);
		return;
	}

	UMMOAbility* nextLvSkill = MMOFacade::getSingleton().GetAbilityWithRuneByNextLevel( originSkill );
	if(NULL == nextLvSkill)
	{
		TipsPanel->ShowNextSkillTip = FALSE;
		return;
	}

	TipsPanel->ShowNextSkillTip = TRUE;

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("Object"));
	if( dataObj  )
	{
		//
		_FormatTipsText( dataObj, nextLvSkill, originSkill );

		//whether next level
		dataObj->SetString(TEXT("NextLevel"), TEXT("<font color='#FF0000'>Next Level</font>") );
		dataObj->SetString( TEXT("CombineLevel"), TEXT("") );

		//whether to show next level ability requirements
		INT skillRuneIdx = nextLvSkill->GetRuneSlot();
		UBOOL isRuneSKill = ( skillRuneIdx > 0 );
		dataObj->SetBool(TEXT("HasUpLevNeed"), !isRuneSKill );
		if( isRuneSKill )
		{
			dataObj->SetString(TEXT("SkillLevel"), TEXT(""));
			dataObj->SetString(TEXT("UpLevNeedSoul"), TEXT(""));
			dataObj->SetString(TEXT("UpLevNeedLev"), TEXT(""));
		}
		else
		{
			//dataObj->SetString(TEXT("UpLevNeedSoul"), UITextParser::HTMLFormatSkillTipsNextNeedMaxProgress(nextLvSkill));
			dataObj->SetString(TEXT("UpLevNeedSoul"), UITextParser::HTMLFormatSkillTipsNextNeedMaxProgress(originSkill));
			dataObj->SetString(TEXT("UpLevNeedLev"), UITextParser::HTMLFormatSkillTipsNextNeedLevel(originSkill));
		}
	}

	TipsPanel->eventCallUpdateNextSkillTipData(dataObj);
}

//------------------------------------------------------------------------
void TipsFormatter_SkillTemplate::FormatAppendSkillTipsWithNoMainSkillLevel(class UMMOAbility* skill, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( !GFXPlayer || !TipsPanel || !SubPanel )
	{
		if( SubPanel != NULL )
		{
			SubPanel->SetVisible(FALSE);
		}

		return;
	}

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("Object"));
	if( dataObj  )
	{
		//
		_FormatTipsText( dataObj, skill, NULL );

		//
		dataObj->SetString(TEXT("NextLevel"), TEXT("") );
		dataObj->SetString( TEXT("CombineLevel"), TEXT("") );
		dataObj->SetString( TEXT("SkillLevel"), TEXT("") );

		//ability unlock info
		dataObj->SetString( TEXT("SkillUnlock"), TEXT("") );

		//whether to show next level ability requirements
		dataObj->SetBool( TEXT("HasUpLevNeed"), FALSE );
		dataObj->SetString( TEXT("UpLevNeedSoul"), TEXT("") );
		dataObj->SetString( TEXT("UpLevNeedLev"), TEXT("") );
	}

	TipsPanel->eventCallUpdateNextSkillTipData(dataObj);
}

//------------------------------------------------------------------------
void TipsFormatter_SkillTemplate::FormatSkillTipsWithEquippedSkillInfo(class UMMOAbility* skill, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( !skill || !GFXPlayer || !TipsPanel || !SubPanel)
	{
		SubPanel->SetVisible(FALSE);
		return;
	}

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("Object"));
	if( dataObj )
	{
		//
		_FormatTipsText( dataObj, skill );

		if( skill->GetRuneSlot() > 0 ){
			//rune ability
			INT tipsSkillID = skill->eventGetAbilityId();
			for( int i = 0; i < SKILL_EQUIP_MAX; ++i ){
				//Current weapon ability
				UBOOL isSkillActive = FALSE;
				UMMOAbility* weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillBySlotIdx( i, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
				//If this weapon carries an ability activated for this key slot
				if( weaponSkill != NULL && isSkillActive ){
					INT weaponSkillID = weaponSkill->eventGetAbilityId();
					if( tipsSkillID == weaponSkillID ){
						//modify rune level
						dataObj->SetString( TEXT("RuneLev") , UITextParser::HTMLFormatSkillTipsRuneLv( weaponSkill ) );
						break;
					}
				}
			}
		}
		//whether next level
		dataObj->SetString(TEXT("NextLevel"), TEXT("") );		            

		//whether to show next level ability requirements
		dataObj->SetBool(TEXT("HasUpLevNeed"), FALSE);
	}

	TipsPanel->eventCallUpdateSkillTipData(dataObj);
}

//------------------------------------------------------------------------
//mainskill may be the skill
void TipsFormatter_SkillTemplate::_FormatTipsText( UGFxObject* dataObj, UMMOAbility* skill, UMMOAbility* originSkill )
{
	if( !dataObj || !skill )
	{
		return;
	}
	UBOOL isRuneSkill = ( skill->GetRuneSlot() > 0 );
	//
	UMMOAbility* mainSkill = NULL;
	if( isRuneSkill ){
		mainSkill = MMOSkillManager::getSingleton().GetMainAbilityFor( skill );
	}
	else{
		mainSkill = skill;
	}

	if( mainSkill == NULL ){
		return;
	}

	//whether self-learned ability
	UMMOAbility* myLearnedSkill = MMOSkillManager::getSingleton().FindMyLearnedMainSkill( mainSkill->eventGetAbilityId() );

	//ability name
	dataObj->SetString( TEXT("SkillName") , mainSkill->eventGetAbilityName() );
	//ability Icon
	dataObj->SetString( TEXT("IconName") , skill->GetAbilityIconName() );
	//effective level
	dataObj->SetString( TEXT("CombineLevel") , UITextParser::HTMLFormatSkillTipsCombineLv( skill, NULL, originSkill ) );
	//ability unlock info
	dataObj->SetString( TEXT("SkillUnlock"), UITextParser::HTMLFormatSkillTipsUnlock( skill ) );
	//whether rune ability
	dataObj->SetBool( TEXT("HasRune"), isRuneSkill );
	////ability level
	//if( isRuneSkill ){
	//	if( myLearnedSkill == NULL ){
	//		//not learned
	//		dataObj->SetString( TEXT("SkillLevel"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.SkillLevNoLevel")) );
	//	}
	//	else{
	//		//dataObj->SetString( TEXT("SkillLevel"), UITextParser::HTMLFormatSkillTipsSkillLv( myLearnedSkill ) );
	//		dataObj->SetString( TEXT("SkillLevel"), UITextParser::HTMLFormatSkillTipsSkillLv( skill, originSkill ) );
	//	}
	//}
	//else{
	//	dataObj->SetString( TEXT("SkillLevel"), UITextParser::HTMLFormatSkillTipsSkillLv( skill, originSkill ) );
	//}

	//
	if( isRuneSkill ){
		//rune name
		dataObj->SetString( TEXT("RuneName") , skill->eventGetAbilityName() );
		//rune level
		dataObj->SetString( TEXT("RuneLev") , UITextParser::HTMLFormatSkillTipsRuneLv( skill , originSkill ) );
		//rune description
		INT key = MMOSkillManager::CombinateAbilityID( skill->eventGetAbilityId(), skill->SpellCommonAttribute.Level, skill->GetRuneSlot() );
		const SpellVarData* spellDat = SpellVarManager::getSingleton().FindData( key );
		if(spellDat){
			dataObj->SetString( TEXT("RuneDescribe") , spellDat->RuneDescription );
		}
	}

	//primary trait, secondary trait, tertiary trait
	if( originSkill != NULL ){
		dataObj->SetString( TEXT("MainSpeciality") , UITextParser::HTMLFormatSkillTipsChangeSpecility( skill->GetMainSpeciality() , originSkill->GetMainSpeciality() ) );
		dataObj->SetString( TEXT("SecondSpeciality") , UITextParser::HTMLFormatSkillTipsChangeSpecility( skill->GetSecondSpeciality() , originSkill->GetSecondSpeciality() ) );
		dataObj->SetString( TEXT("ThirdSpeciality") , UITextParser::HTMLFormatSkillTipsChangeSpecility( skill->GetThirdSpeciality() , originSkill->GetThirdSpeciality() ) );
	}
	else{
		dataObj->SetString( TEXT("MainSpeciality") , skill->GetMainSpeciality() );
		dataObj->SetString( TEXT("SecondSpeciality") , skill->GetSecondSpeciality() );
		dataObj->SetString( TEXT("ThirdSpeciality") , skill->GetThirdSpeciality() );
	}

	//ability description
	dataObj->SetString( TEXT("SkillDescribe"), UITextParser::HTMLFormatSkillTipsDesc( skill , originSkill ) );

	////image address for this attack effect, 42 kinds
	//INT effectRangeEnum = Clamp<INT>( skill->EffectRangeEnum , 0 , 41 );
	//FString iconPath;
	//if( effectRangeEnum != 0 ){
	//	iconPath = FString( TEXT("MMO_UIIcon.SkillRange.") ) + appItoa( effectRangeEnum );
	//}
	//else{
	//	iconPath = TEXT("");//TEXT("MMO_UIIcon.icon.blank");
	//}

	//dataObj->SetString( TEXT("EffectRangeTexPath") , iconPath );
	////brief description
	//dataObj->SetString( TEXT("DescriptiveWords") , skill->DescriptiveWords );
	////effect description
	//FString effectRangeText = _ParseAttackRange(skill->EffectRange);
	//FString attackRangeText = _ParseAttackRange(skill->AttackRange);

	//dataObj->SetString( TEXT("UseMethod") , FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.UseMethod")), *skill->UseMethod ) );
	//dataObj->SetString( TEXT("AttackRange") , FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.SkillDis")), *attackRangeText ) );
	//dataObj->SetString( TEXT("EffectRange") , FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.SkillRange")), *effectRangeText ) );

	////ability cost
	//dataObj->SetString( TEXT("SkillCost"), UITextParser::HTMLFormatSkillTipCost( skill ));

	////ability CD
	//FString cd;
	//if( skill->SpellCommonAttribute.SelfCoolDown > 0 ){
	//	cd = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Tips.CdTime")), skill->SpellCommonAttribute.SelfCoolDown );
	//}
	//dataObj->SetString( TEXT("CDTime"), cd );
}

FString TipsFormatter_SkillTemplate::_ParseAttackRange( FString parseText )
{
	FString effectRangeText;

	FString keyWord = TEXT("catAttackRange");
	INT replacePos = parseText.InStr( keyWord );
	if( replacePos >= 0 )
	{
		// JO*(WJOSIJERg
		//FLOAT range = (FLOAT)MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catAttackRange , AttrListenerType_Mine );
		////FLOAT range = (FLOAT)MMOFacade::getSingleton().GetPlayerAttribute(catAttackRange);
		//effectRangeText = parseText.Left(replacePos) 
		//	+ FString::Printf( TEXT("%.1f") , range/POINT_PER_CM/100 )
		//	+ parseText.RightChop( replacePos + keyWord.Len() );
		// JO*(WJOSIJERg end
	}
	else
	{
		effectRangeText = parseText;
	}

	return effectRangeText;
}

//---------------------------------------------------------------------------------------------------
void TipsFormatter_SkillTemplateWithNext::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if( NULL == GFXPlayer || NULL == TipsPanel ) return;

	UINT lookupKey = SrcObj->GetInt( TEXT("itemIndex") );//SrcObj->GetInt( TEXT("UniqueSkillID") );//MMO::Facade::CombinateID( skillID, skillLv, runeSlotID );
	if( lookupKey == 0 ){
		lookupKey = SrcObj->GetInt( TEXT("UniqueSkillID") );
	}

	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));
	UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate( lookupKey );

	if(NULL != skill)
	{
		UBOOL isRuneSKill = ( skill->GetRuneSlot() > 0 );
		if( isRuneSKill )
		{
			TipsFormatter_SkillTemplate::FormatSkillTipsWithNoMainSkillLevel( skill, TipsPanel, SubPanel);
		}
		else
		{
			TipsFormatter_SkillTemplate::FormatSkillTips( skill, TipsPanel, SubPanel);
		}
	}
	else
	{
		TipsPanel->SetVisible(FALSE);
		return;
	}

	//
	TipsFormatter_SkillTemplate::FormatAppendSkillTipsForSkillNextLv(skill, TipsPanel, SubPanel);
}


//------------------------------------------------------------------------
//TipsFormatter_LockedSlot
//------------------------------------------------------------------------
void TipsFormatter_LockedSlot::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	INT ContainerID = SrcObj->GetInt(TEXT("containerID"));
	INT SlotID = SrcObj->GetInt(TEXT("slotID"));
	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));

	FString tipsHtml;

	switch ( SlotType )
	{
	case ESPType_Bag:	
	case ESPType_Shop:
	case ESPType_ShopSell:   
	case ESPType_Enhance:
	case ESPType_TradeP2P:
	case ESPType_Quest:
	case ESPType_SoulEmbed:
	case ESPType_SoulLevelUp:
	case ESPType_SoulUnEmbed:
	case ESPType_ItemCollection:
	case ESPType_BagResource:
		tipsHtml = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.ItemLock") );
		break;
	case ESPType_StallSetting:
	case ESPType_StallBuy:
		tipsHtml = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.StallLock") );
		break;
	case ESPType_SkillPanel:
	case ESPType_SkillBar:
		tipsHtml = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.SkillLock") );
		break;
	case ESPType_Mount:
		tipsHtml = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.MountLock") );
		break;
	case ESPType_Warehouse:
		tipsHtml = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.WarehouseLock") );
		break;
	default:
		tipsHtml = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Lock.NormalLock") );
		break;

	}   

	//
	FillConstWidthCommonTip( SubPanel, tipsHtml );

	return;
}


//------------------------------------------------------------------------
//TipsFormatter_PlainText
//------------------------------------------------------------------------
void TipsFormatter_PlainText::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	FString tipsHtml = SrcObj->GetString(TEXT("tipsHtml"));

	//
	FillConstWidthCommonTip( SubPanel, tipsHtml );

	return;
}

void TipsFormatter_NormalText::FormatTips(class UGFxObject* SrcObj, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	if( !SrcObj )
		return;

	FString tipsHtml = SrcObj->GetString(TEXT("text"));

	//
	FillConstWidthCommonTip( SubPanel, tipsHtml );

	return;
}

//------------------------------------------------------------------------
//TipsFormatter_Tab
//------------------------------------------------------------------------
void TipsFormatter_Tab::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	if( !TipsPanel || !SrcObj )
		return;

	FString caption = SrcObj->GetString(TEXT("tabCaption"));
	if( caption.IsEmpty() || caption == TEXT("null") )
	{
		caption = SrcObj->GetString(TEXT("label"));
	}
	//
	FillConstWidthCommonTip( SubPanel, caption );
}

void TipsFormatter_Attribute::FormatTips(class UGFxObject* SrcObj, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	if( !TipsPanel )
		return;

	INT groupId = SrcObj->GetInt( TEXT("itemdataid") );
	INT indexId = SrcObj->GetInt( TEXT("itemid") );
	INT panelId = SrcObj->GetInt( TEXT("iteminfoid") );

	if( panelId == ESPType_Charactor )
	{
		MMOAttributeAccountManager::getSingleton().SetAttrOwnerMode(AttrListenerType_Mine);
	}
	else
	{
		MMOAttributeAccountManager::getSingleton().SetAttrOwnerMode(AttrListenerType_OtherPlayer);
	}
	FString caption = MMOAttributeAccountManager::getSingleton().GetAttributeDesc(groupId,indexId);

	if(caption.IsEmpty())
	{
		return;
	}

	//
	FillConstWidthCommonTip( SubPanel, caption );
}

void TipsFormatter_GoldStore::FormatTips(class UGFxObject* SrcObj, class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	if( !TipsPanel )
		return;

	INT idx  = SrcObj->GetInt( TEXT("itemid") );
	Client_ExchangeItemInfo* info = MMOGoldStoreManager::getSingleton().GetMyOrderInfoByIndex(idx);
	if (!info)
		return;

	__time32_t  time = info->nOrderTime;
	struct tm *today = _localtime32(&time);
	FString timeStr = "";
	if (today)
	{
		timeStr =   FString::Printf(TEXT("%02d:%02d"), 
					//today->tm_year + 1900,
					//today->tm_mon + 1, 
					//today->tm_mday,
					today->tm_hour, 
					today->tm_min
					//today->tm_sec
					);
	}
	
	

	INT curCount = info->nCount - info->nCurCount;
	INT count = info->nCount;

	FString id_High = appItoa(info->dwOrderID >> 32);
	FString id_Low =  appItoa(info->dwOrderID);
	FString id = id_High + id_Low;

	FString caption = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GoldStore.Tips")) ,*id ,*timeStr,count,curCount);

	if(caption.IsEmpty())
	{
		return;
	}

	//
	FillConstWidthCommonTip( SubPanel, caption );
}

//------------------------------------------------------------------------
//TipsFormatter_MenuBar
//------------------------------------------------------------------------
void TipsFormatter_MenuBar::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	INT objectId = 0; // TODO:

	UGFxObject* TipsText =  SubPanel->GetObject("tips_item_text");	

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	UGamePlayDataManager* DataManager = Player->GpDataManager;
	for( int i = 0;i< DataManager->MenuInfoList.Num();i++)
	{
		if( DataManager->MenuInfoList(i).menuType == objectId )
		{
			FillConstWidthCommonTip( SubPanel, DataManager->MenuInfoList(i).MenuName );
			break;
		}
	}
}

//------------------------------------------------------------------------
//TipsFormatter_AwardIcon
//------------------------------------------------------------------------
void TipsFormatter_AwardIcon::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	INT objectId = SrcObj->GetInt(TEXT("ItemTemplateID"));

	UItem* item = MMOFacade::getSingleton().FindItemTemplate( objectId );
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 

	if( item == NULL || !GFXPlayer )
	{
		TipsPanel->SetVisible(FALSE);
		return;
	}

	//pass data into Tip DataObj
	UGFxObject* itemObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.ItemTipData"));
	FormatItemTip( GFXPlayer, itemObj, item, NULL );

	TipsPanel->eventCallUpdateItemTipData( itemObj );

}

//------------------------------------------------------------------------
//TipsFormatter_Other
//------------------------------------------------------------------------
void TipsFormatter_Other::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	FillConstWidthCommonTip( SubPanel, TEXT("What is Other Tips?") );
}

//------------------------------------------------------------------------
//TipsFormatter_RepairAll
//------------------------------------------------------------------------
void TipsFormatter_RepairAll::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	FString moneyString = UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , MMOEquipManager::getSingleton().CountEquipRepairCostAll() );
	//
	FillConstWidthCommonTip( SubPanel, FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Shop.RepairAll") ) , *moneyString ) );
}
//------------------------------------------------------------------------
//TipsFormatter_BuffSlot
//------------------------------------------------------------------------
void TipsFormatter_BuffSlot::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	if(SrcObj==NULL)return;

	INT slotId = SrcObj->GetInt(TEXT("curBuffSlotID")); 
	INT ownerId = SrcObj->GetInt(TEXT("BuffOwnerID")); 
	INT buffID	= SrcObj->GetInt(TEXT("curBuffID")); 

	FString ShowText;
	UGFxObject* TipsText =  SubPanel->GetObject("tips_item_text");	


	//check whether in buffmanager
	BuffData* pBuff	= NULL;
	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player && Player->ServerID ==ownerId){
		pBuff	=  MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffBySlot(slotId);
		if(pBuff==NULL || pBuff->Buff==NULL || pBuff->Buff->TemplateID != buffID)
		{
			pBuff	=  MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffExBySlot(slotId);
		}
	}
	else{
		pBuff	=  MMOFacade::getSingleton().GetMMOBuffManager()->GetTargetBuffBySlot(slotId);
	}



	//use the same tips resource as mount
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if(pBuff && pBuff->Buff && GFXPlayer ){
		UGFxObject* mountObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.MountTipData"));
		if(mountObj){
			mountObj->SetString(TEXT("MountName"),pBuff->BuffName);
			mountObj->SetString(TEXT("IconName"), pBuff->DisplayIconName );
			mountObj->SetString(TEXT("ColorName"), pBuff->BuffColor );// TEXT("lv0") );
			mountObj->SetString(TEXT("MountDescribe"), pBuff->BuffDescription);
			TipsPanel->eventCallUpdateMountTipData( mountObj );
		}
	}
	else
	{
		INT nLevel	= SrcObj->GetInt(TEXT("curBuffLevel")); 
		UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(buffID, nLevel);
		if(buff && GFXPlayer)
		{
			UGFxObject* mountObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.MountTipData"));
			if(mountObj){
				mountObj->SetString(TEXT("MountName"),buff->BuffName.ToString());
				mountObj->SetString(TEXT("IconName"), buff->GetBuffIconName());
				mountObj->SetString(TEXT("ColorName"), TEXT("lv0"));
				mountObj->SetString(TEXT("MountDescribe"),buff->BuffDescription);
				TipsPanel->eventCallUpdateMountTipData(mountObj);
			}
		}
	}

	////read template buff to show tips
	//if( pBuff )
	//   {
	//       ShowText = UITextParser::HTMLFormatBuffTips(pBuff->Buff);
	//	//
	//	FillConstWidthCommonTip( SubPanel, ShowText );
	//   }
	//else
	//{
	//	INT nLevel	= SrcObj->GetInt(TEXT("curBuffLevel")); 
	//	UBuff* buff = MMOFacade::getSingleton().FindBuffTemplate(buffID, nLevel);
	//	if(buff)
	//	{
	//		ShowText = UITextParser::HTMLFormatBuffTips(buff);
	//		//
	//		FillConstWidthCommonTip( SubPanel, ShowText );
	//	}
	//}
}


//------------------------------------------------------------------------
//TipsFormatter_BuffSlot
//------------------------------------------------------------------------
void TipsFormatter_MountSlot::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 
	if(SrcObj && GFXPlayer){
		INT MountID	= SrcObj->GetInt(TEXT("ItemTemplateID")); 
		MountData* pMountData = MMOFacade::getSingleton().GetMountData(MountID);

		UGFxObject* mountObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.MountTipData"));
		if(pMountData && mountObj){
			mountObj->SetString(TEXT("MountName"),pMountData->Name);
			mountObj->SetString(TEXT("IconName"),pMountData->IconName);
			mountObj->SetString(TEXT("ColorName"), FString::Printf(TEXT("lv%d"), pMountData->Quality ) );
			mountObj->SetString(TEXT("MountDescribe"),pMountData->Script);
			TipsPanel->eventCallUpdateMountTipData( mountObj );
		}
	}
}

//------------------------------------------------------------------------
//TipsFormatter_RuneActive
//------------------------------------------------------------------------
void TipsFormatter_RuneActive::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	FString ShowText;
	//FString ShowTextR;

	INT SlotType = SrcObj->GetInt(TEXT("SlotType")); 
	int RuneSlot = SrcObj->GetInt(TEXT("RuneSlot")); 
	int ActiveId = SrcObj->GetInt(TEXT("ActiveId"));

	//rune requirements
	ShowText = UITextParser::HTMLFormatRuneItemTips(ActiveId, RuneSlot);
	//ShowTextR= UITextParser::HTMLFormatRuneItemTips(ActiveId, RuneSlot);

	//
	FillConstWidthCommonTip( SubPanel, ShowText );
}

//------------------------------------------------------------------------
//TipsFormatter_RuneButton
//------------------------------------------------------------------------
void TipsFormatter_RuneButton::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	INT SlotID = SrcObj->GetInt(TEXT("slotID"));
	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));
	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 

	if( !GFXPlayer )
	{
		return;
	}

	INT abilityId = SrcObj->GetInt(TEXT("ActiveId"));
	//0~5
	INT runeId = SrcObj->GetInt(TEXT("RuneSlot"));

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if( !Player )
		return;
	UMMOAbility* skill =  Player->GetMainAbilityById( abilityId );
	if( !skill  )
		return;
	UMMOAbility* rune = NULL;

	if( runeId >= 0 && skill->SubRuneList.Num() >0 && runeId < skill->SubRuneList.Num() )
	{
		rune = skill->SubRuneList( runeId );
	}

	if( NULL == rune ||  NULL == rune->MainSkill)
	{  
		SubPanel->SetVisible(FALSE);
		return;
	}

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.SkillTipData"));
	if( dataObj  )
	{
		//ability name
		dataObj->SetString( TEXT("SkillName"), rune->MainSkill->eventGetAbilityName() );
		//ability level
		FString sLv = FString(" Lv")+ appItoa( rune->SpellCommonAttribute.Level );
		dataObj->SetString( TEXT("SkillLevel"), sLv );
		//ability Icon
		dataObj->SetString( TEXT("IconName"), rune->GetAbilityIconName() );
		//ability cost
		dataObj->SetString( TEXT("SkillCost"), UITextParser::HTMLFormatSkillTipCost( rune ));
		//ability CD
		FString cd = FString::Printf( TEXT("%.2f s"), rune->SpellCommonAttribute.SelfCoolDown );
		dataObj->SetString( TEXT("CDTime"), cd );
		//ability equipment restriction
		dataObj->SetString( TEXT("SkillLimit"), UITextParser::HTMLFormatSkillTipsLimit( rune )  ); 
		//ability unlock info
		dataObj->SetString( TEXT("SkillUnlock"), UITextParser::HTMLFormatSkillTipsUnlock( rune ) );
		//ability description, fetch the main ability's description
		dataObj->SetString( TEXT("SkillDescribe"), rune->MainSkill->eventGetAbilityDescribe() );
		//ability damage combo
		dataObj->SetString( TEXT("SkillDamage"), UITextParser::HTMLFormatSkillTipsDamage( rune ) ); 
		//whether current ability has a rune equipped
		dataObj->SetBool( TEXT("HasRune"), TRUE );
		//RuneId
		dataObj->SetInt( TEXT("RuneId"), runeId+1 /*rune->GetSkillGuiRuneID()*/ );
		//rune name
		dataObj->SetString( TEXT("RuneName"),rune->eventGetAbilityName() );
		//rune description
		dataObj->SetString( TEXT("RuneDescribe"), rune->eventGetAbilityDescribe() );
		//whether to show Aid or next level
		dataObj->SetBool( TEXT("HasNext"), FALSE );
		//whether to show next level ability, respond to key press
		dataObj->SetBool( TEXT("ShowNext"), FALSE );

	}

	TipsPanel->eventCallUpdateSkillTipData( dataObj );
}

//------------------------------------------------------------------------
//TipsFormatter_AvartaRuneButton
//------------------------------------------------------------------------
void TipsFormatter_AvartaRuneButton::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	//Avarta's Rune is actually an unconventional, non-growing, purchasable main ability wearing a rune disguise, what a pain
	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if( !Player )
		return;

	INT AvartaId = SrcObj->GetInt(TEXT("AvartaId"));
	//used for SubList index here, subtract 1 first
	INT AvartaRuneId = SrcObj->GetInt(TEXT("RuneSlot"))-1;

	UINT Key = MMOSkillManager::CombinateAbilityID( AvartaId, 1 ,0 );
	UMMOAbility* skill = MMOSkillManager::getSingleton().FindAbilityTemplate(Key);
	UMMOAbility* rune = NULL;

	if( skill && AvartaRuneId >= 0 && AvartaRuneId < skill->SubRuneList.Num() )
		rune =  skill->SubRuneList( AvartaRuneId );

	UGFxMoviePlayer* GFXPlayer = TipsPanel->GFXPlayer; 

	if( NULL == rune || NULL == GFXPlayer )
	{  
		SubPanel->SetVisible(FALSE);
		return;
	}

	UGFxObject* dataObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Tips.SkillTipData"));
	if( dataObj  )
	{
		//ability name
		dataObj->SetString( TEXT("SkillName"), rune->eventGetAbilityName() );
		//ability level
		FString sLv = FString(" Lv")+ appItoa( rune->SpellCommonAttribute.Level );
		dataObj->SetString( TEXT("SkillLevel"), sLv );
		//ability Icon
		dataObj->SetString( TEXT("IconName"), rune->GetAbilityIconName() );
		//ability cost
		dataObj->SetString( TEXT("SkillCost"), UITextParser::HTMLFormatSkillTipCost( rune ));
		//ability CD
		FString cd = FString::Printf( TEXT("%.2f s"), rune->SpellCommonAttribute.SelfCoolDown );
		dataObj->SetString( TEXT("CDTime"), cd );
		//ability equipment restriction
		dataObj->SetString( TEXT("SkillLimit"), UITextParser::HTMLFormatSkillTipsLimit( rune )  ); 
		//ability unlock info
		dataObj->SetString( TEXT("SkillUnlock"), UITextParser::HTMLFormatSkillTipsUnlock( rune ) );
		//ability description, fetch the main ability's description
		dataObj->SetString( TEXT("SkillDescribe"), rune->eventGetAbilityDescribe() );
		//ability damage combo
		dataObj->SetString( TEXT("SkillDamage"), UITextParser::HTMLFormatSkillTipsDamage( rune ) ); 
		//whether current ability has a rune equipped
		dataObj->SetBool( TEXT("HasRune"), FALSE );
		//whether to show Aid or next level
		dataObj->SetBool( TEXT("HasNext"), FALSE );
		//whether to show next level ability, respond to key press
		dataObj->SetBool( TEXT("ShowNext"), FALSE );

	}

	TipsPanel->eventCallUpdateSkillTipData( dataObj );
}

//------------------------------------------------------------------------
//TipsFormatter_FreeStyle
//------------------------------------------------------------------------
void TipsFormatter_FreeStyle::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	FString ShowText;
	//FString ShowTextR;

	if( SrcObj )
	{
		ShowText  = UITextParser::HTMLFormatObject(SrcObj);
		//ShowTextR = UITextParser::HTMLFormatObject(SrcObj);
		INT showStyle = SrcObj->GetInt( TEXT("style") );
		//
		FillConstWidthCommonTip( SubPanel, ShowText, showStyle );
	}
}
//------------------------------------------------------------------------
//TipsFormatter_AutoSizeTips
//------------------------------------------------------------------------
void TipsFormatter_AutoSizeTips::FormatTips( class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel )
{
	if( SrcObj == NULL ){
		return;
	}

	FString tipsHtml = SrcObj->GetString(TEXT("tipsHtml"));
	FillAutoSizeCommonTip( SubPanel, tipsHtml );
}
//------------------------------------------------------------------------
//TipsFormatter_HeavenGasSlot
//------------------------------------------------------------------------
void TipsFormatter_HeavenGasSlot::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	INT ContainerID = SrcObj->GetInt(TEXT("containerID"));
	INT SlotID = SrcObj->GetInt(TEXT("slotID"));
	INT SlotType = SrcObj->GetInt(TEXT("SlotType"));

	if(SlotType == ESPType_HeavenGasEquip || SlotType == ESPType_HeavenGasBag || SlotType == ESPType_HeavenGasBank)
	{
		FString tipsHtml;
		tipsHtml.Empty();
		MMOHeavenManager::getSingleton().GetHeavenGasItemTips(tipsHtml,ContainerID,SlotID);

		//
		FillConstWidthCommonTip( SubPanel, tipsHtml );	
	}

	return;
}

//------------------------------------------------------------------------
//TipsFormatter_EquipSlotEnhance
//------------------------------------------------------------------------
void TipsFormatter_EquipSlotEnhance::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	INT enhanceClassID = SrcObj->GetInt(TEXT("tipsVal1"));
	INT curEnhanceLv = SrcObj->GetInt(TEXT("tipsVal2"));

	EquipSlotManager &eSlotMgr = EquipSlotManager::getSingleton();
	const TArray<INT> &enhanceHoleLv = eSlotMgr.GetSortedEnhanceExtrPropLvIdx( enhanceClassID );

	INT curEnhanceHoleLv = 0;
	INT nxtEnhanceHoleLv = 0;

	INT allLevelCnt = enhanceHoleLv.Num();
	if( allLevelCnt < 1 ){
		return;
	}

	UBOOL isMaxEnhanceHoleLv = TRUE;
	INT nxtLvIdx = 0;
	for( INT nxtLvIdx = 0; nxtLvIdx < allLevelCnt; ++nxtLvIdx ){
		INT requireEnhanceLv = enhanceHoleLv( nxtLvIdx );
		if( curEnhanceLv < requireEnhanceLv ){
			curEnhanceHoleLv = ( nxtLvIdx < 1 ) ? 0 : enhanceHoleLv( nxtLvIdx - 1 );
			nxtEnhanceHoleLv = enhanceHoleLv( nxtLvIdx );

			isMaxEnhanceHoleLv = FALSE;
			break;
		}
	}

	//
	const EquipSlotEnhancePropCfgData *pCurHoleLvEhcProp = NULL;
	const EquipSlotEnhancePropCfgData *pNxtHoleLvEhcProp = NULL;
	UBOOL needNextLevelShowing = FALSE;
	if( isMaxEnhanceHoleLv == FALSE ){
		if( curEnhanceHoleLv > 0 ){
			needNextLevelShowing = TRUE;
			pCurHoleLvEhcProp = eSlotMgr.FindEnhancePropCfg( enhanceClassID, NES_TOTAL, curEnhanceHoleLv );
		}

		pNxtHoleLvEhcProp = eSlotMgr.FindEnhancePropCfg( enhanceClassID, NES_TOTAL, nxtEnhanceHoleLv );
	}
	else{
		//curEnhanceHoleLv = ( allLevelCnt >= 1 ) ? enhanceHoleLv( allLevelCnt - 1 ) : 0;
		curEnhanceHoleLv = enhanceHoleLv( allLevelCnt - 1 );
		pCurHoleLvEhcProp = eSlotMgr.FindEnhancePropCfg( enhanceClassID, NES_TOTAL, curEnhanceHoleLv );
		needNextLevelShowing = FALSE;
	}

	//
	FString tipHtml;
	const INT titleFontSize = 18;
	//
	if( pCurHoleLvEhcProp != NULL ){
		INT ehcMajorLevel = 0;
		INT ehcMinorLevel = 0;
		INT ehcMinorMinorLv = 0;
		eSlotMgr.CvtEnhanceLevel( curEnhanceHoleLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
		//
		FString enhanceLvColor = TEXT("EnhanceLvColor");
		enhanceLvColor += appItoa( ehcMajorLevel );

		//enhancement stage title
		tipHtml += UITextParser::HTMLBeginParagraph();
		tipHtml += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( *enhanceLvColor ), titleFontSize );

		FString enhanceLvName = TEXT("MMOGame.EquipSlotEnhance.LevelStName_");
		enhanceLvName += appItoa( ehcMinorLevel );
		enhanceLvName = MMOUI::ParseLocalizedPropertyPath( enhanceLvName );
		tipHtml += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvTitle") ), *enhanceLvName );

		tipHtml += UITextParser::HTMLEndFont();
		tipHtml += UITextParser::HTMLEndParagraph();

		//enhancement level requirement for attribute activation
		tipHtml += UITextParser::HTMLBeginParagraph();
		//"Condition:"
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvRequireTitle") );
		//
		//FString fontFormat = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("NormalContentColor")) );
		//"All slots enhanced to"
		//tipHtml += fontFormat;
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvRequire") );
		//tipHtml += UITextParser::HTMLEndFont();
		//
		tipHtml += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( *enhanceLvColor ) );
		tipHtml += MMOUI::ParseLocalizedPropertyPath( FString::Printf( TEXT("MMOGame.EquipSlotEnhance.Level_%d"), ehcMinorLevel ) );
		tipHtml += UITextParser::HTMLEndFont();
		//
		tipHtml += UITextParser::HTMLEndParagraph();

		//each attribute
		tipHtml += FormatEnhanceProp( pCurHoleLvEhcProp, MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvPropTitle") ), TEXT("")/*fontFormat*/ );
	}

	//
	if( needNextLevelShowing == TRUE ){
		tipHtml += UITextParser::HTMLNewLine();
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvNextTipTitle") );
		tipHtml += UITextParser::HTMLNewLine();
	}

	//
	if( pNxtHoleLvEhcProp != NULL ){
		INT ehcMajorLevel = 0;
		INT ehcMinorLevel = 0;
		INT ehcMinorMinorLv = 0;
		eSlotMgr.CvtEnhanceLevel( nxtEnhanceHoleLv, ehcMajorLevel, ehcMinorLevel, ehcMinorMinorLv );
		//
		FString enhanceLvColor = TEXT("EnhanceLvColor");
		enhanceLvColor += appItoa( ehcMajorLevel );

		//enhancement stage title
		tipHtml += UITextParser::HTMLBeginParagraph();
		tipHtml += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( *enhanceLvColor ), titleFontSize );

		FString enhanceLvName = TEXT("MMOGame.EquipSlotEnhance.LevelStName_");
		enhanceLvName += appItoa( ehcMinorLevel );
		enhanceLvName = MMOUI::ParseLocalizedPropertyPath( enhanceLvName );
		tipHtml += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvTitle") ), *enhanceLvName );

		tipHtml += UITextParser::HTMLEndFont();
		tipHtml += UITextParser::HTMLEndParagraph();

		//enhancement level requirement for attribute activation
		tipHtml += UITextParser::HTMLBeginParagraph();
		//"Condition:"
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvRequireTitle") );
		//
		FString fontFormat = UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("DisableColor")) );
		//"All slots enhanced to"
		tipHtml += fontFormat;
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvRequire") );
		tipHtml += UITextParser::HTMLEndFont();
		//
		tipHtml += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor( *enhanceLvColor ) );
		tipHtml += MMOUI::ParseLocalizedPropertyPath( FString::Printf( TEXT("MMOGame.EquipSlotEnhance.Level_%d"), ehcMinorLevel ) );
		tipHtml += UITextParser::HTMLEndFont();
		//
		tipHtml += UITextParser::HTMLEndParagraph();

		//each attribute
		tipHtml += FormatEnhanceProp( pNxtHoleLvEhcProp, MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipSlotEnhance.HoleLvPropTitle") ), fontFormat );
	}

	//
	FillAutoSizeCommonTip( SubPanel, tipHtml );		

	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
FString TipsFormatter_EquipSlotEnhance::FormatEnhanceProp( const struct EquipSlotEnhancePropCfgData* pEhcProp, const FString &profixStr, const FString &fontFormat )
{
	FString strHtml;
	if( pEhcProp == NULL ){
		return strHtml;
	}

	const INT propProfixLenMax = 3;
	const INT propTitleLenMax = 8;

	UBOOL hasFontFormat = ( fontFormat.Len() > 0 );
	//
	INT curEnhancePropCnt = 0;
	for( INT i = 0; i < ENHANCE_EFFECT_COUNT; i++){
		const ItemRandomAttr* propAttr = MMOFacade::getSingleton().GetRandomAttributById( pEhcProp->EffectProperties[i] );
		if( propAttr == NULL ){
			continue;
		}
		++curEnhancePropCnt;

		strHtml += UITextParser::HTMLBeginParagraph();

		//profix title
		INT charCnt = 0;
		if( curEnhancePropCnt == 1 ){
			strHtml += profixStr;
			charCnt = profixStr.Len();
		}
		else{
			//prop title space
			if( charCnt < propProfixLenMax ){
				INT cnt = propProfixLenMax - charCnt;
				for( INT j = cnt; j > 1; --j ){
					strHtml += TEXT("    ");//4 spaces
				}
				if( cnt >= 1 ){
					strHtml += TEXT("   ");//3 spaces
				}
			}
		}

		//
		if( hasFontFormat ){
			strHtml += fontFormat;
		}

		//prop title
		FString propString = MMOUI::ParseLocalizedPropertyPath( FString( TEXT( "MMOGame.CHARACTERATTRTEXT." ) ) + propAttr->attrType );
		strHtml += propString;

		//prop value space
		charCnt = propString.Len();
		if( charCnt < propTitleLenMax ){
			for( INT j = propTitleLenMax - charCnt; j > 0; --j ){
				strHtml += TEXT("    ");
			}
		}
		//prop value
		propString = TEXT("+");
		propString += MMOFacade::getSingleton().GetRandomAttributeFormatedValue( propAttr ) ;
		strHtml += propString;

		//
		if( hasFontFormat ){
			strHtml += UITextParser::HTMLEndFont();
		}
		strHtml += UITextParser::HTMLEndParagraph();
	}

	return strHtml;
}

//------------------------------------------------------------------------
//TipsFormatter_EquipSlotEnhance
//------------------------------------------------------------------------
void TipsFormatter_EquipDurability::FormatTips(class UGFxObject* SrcObj,  class UMMOUITips* TipsPanel, class UGFxObject* SubPanel)
{
	TArray< FString > curDurHintStr;
	TArray< FString > curMaxDurHintStr;
	FString durHintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipDurability.DetailFormat") );
	//
	INT curDurLowCnt = 0;
	INT curMaxDurLowCnt = 0;
	UINT repairCost = 0;
	MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();
	for( INT i = E_NES_MAIN_HAND; i < E_NES_TOTAL; ++i ){
		//if( i == E_NES_AVATAR ){
		//	//none for now
		//	continue;
		//}

		//other
		FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED , i );
		if( pItemInfo != NULL ){
			FString equipName;

			UEquip *pEquip = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( pItemInfo->m_nTemplateID ) );
			if( pEquip == NULL ){
				continue;
			}

			INT curDurPect = equipMgr.GetEquipCurDurPect( pItemInfo );

			if( curDurPect <= 20 ){
				++curDurLowCnt;
				equipName = MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Equipslot.EquipName") ) + appItoa(i) );
				curDurHintStr.Push( FString::Printf( *durHintFormat, *equipName, curDurPect ) );
				if( curDurPect < 10 ){
					curDurHintStr.Last() += TEXT(" ");
				}

				//
				repairCost += MMOEquipManager::getSingleton().GetEquipRepairCost( pEquip, pItemInfo );
			}
			INT curMaxDurPect = equipMgr.GetEquipCurMaxDurPect( pItemInfo, pEquip );
			if( curMaxDurPect <= 20 ){
				++curMaxDurLowCnt;
				if( equipName.IsEmpty() ){
					equipName = MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Equipslot.EquipName") ) + appItoa(i) );
					curMaxDurHintStr.Push( FString::Printf( *durHintFormat, *equipName, curMaxDurPect ) );
				}
			}
		}
	}
	//
	FString tipHtml;
	//
	if( curDurLowCnt > 0 && curMaxDurLowCnt > 0 ){
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipDurability.DurTipsTitleFormat") );
	}
	else if( curDurLowCnt > 0 ){
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipDurability.CurDurTipsTitle") );
	}
	else if( curMaxDurLowCnt > 0 ){
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipDurability.CurMaxDurTipsTitle") );
	}
	//
	INT maxLineCnt = ( curDurLowCnt > curMaxDurLowCnt ? curDurLowCnt : curMaxDurLowCnt );
	if( maxLineCnt > 0 ){
		tipHtml += TEXT("<br>");
	}

	INT curDurLowIdx = 0;
	INT curMaxDurLowIdx = 0;
	FString textGap = MMOUI::ParseLocalizedPropertyPath( TEXT( "MMOGame.EquipDurability.DetailColumeGap" ) );
	for( ; ( curDurLowIdx < curDurLowCnt && curMaxDurLowIdx < curMaxDurLowCnt ); ){
		if( curDurLowIdx < curDurLowCnt ){
			tipHtml += curDurHintStr( curDurLowIdx++ );
		}

		if( curMaxDurLowIdx < curMaxDurLowCnt ){
			tipHtml += textGap;
			tipHtml += curMaxDurHintStr( curMaxDurLowIdx++ );
		}

		//
		tipHtml += TEXT("<br>");
	}

	//current repair fee
	if( curDurLowCnt > 0  ){
		tipHtml += UITextParser::HTMLNewLine();
		tipHtml += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipDurability.RepairCostTitle") );
		tipHtml += UITextParser::HTMLMoneyIconString( MONEYTYPE_02 , curDurLowCnt );
	}

	//
	FillAutoSizeCommonTip( SubPanel, tipHtml );		

	return;
}
//====================================================================================================================================================================