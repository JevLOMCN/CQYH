#include "stdafx.h"
#include "message_handler.h"
#include "MMOQuestManager.h"
#include "ClientActionConditionCheck.h"
#include "UITextParser.h"
#include "MMOAchievementManager.h"
#include "MMOFacade.h"
#include "LinkManager.h"
#include "MMOGameUIClasses.h"
#include "money_def.h"
#include "MMOChatManager.h"
#include "CommonGameEvents.h"
#include "MMOPowerManager.h"
#include "MMOGuideManager.h"
#include "MMOEquipManager.h"
#include "FriendManager.h"
#include "MMOHandbookManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

#define M_DEBUG_ITEM 0

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_MoveItem( INT DstConSlot , INT DstItemSlot , INT SrcConSlot , INT SrcItemSlot )
{
	FClientActionConditionContex contex;
	//contex.Item = MMOFacade::getSingleton().FindItem(DstConSlot, DstItemSlot);
	ClientActionCondition_ItemMove checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	if( DstConSlot == E_CONSLOT_EQUIPPED || SrcConSlot == E_CONSLOT_EQUIPPED ){
		MMOEquipManager::getSingleton().BeforeServerEquipSwapHandleWeaponMainSkill( SrcConSlot, SrcItemSlot, DstConSlot, DstItemSlot );
	}

    C2G_MOVE_ITEM msg;

    msg.wMsg = c2g_move_item;
    msg.byDstConSlot = DstConSlot;
    msg.byDstItemSlot = DstItemSlot;
    msg.bySrcConSlot = SrcConSlot;
    msg.bySrcItemSlot = SrcItemSlot;

    _Send(&msg, sizeof(msg));

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

#if defined(_DEBUG) && M_DEBUG_ITEM
	static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECONTAINER_SLOT"), TRUE);
	warnf(NAME_Warning, TEXT("=========Send_MoveItem [%s][%d] -> [%s][%d]"),
		*InEnum->GetEnum(SrcConSlot).ToString(),
		SrcItemSlot,
		*InEnum->GetEnum(DstConSlot).ToString(),
		DstItemSlot);
#endif
}
//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_UseItem( INT ConSlot, INT ItemSlot, INT TemplateID )
{
	FClientActionConditionContex contex;
	contex.Item = MMOFacade::getSingleton().FindItem(ConSlot, ItemSlot);
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(TemplateID);
	if(itemTemp)
	{
		UGamePlayDataManager::GetSingleton()->PlayItemActionSounds(
			MMOFacade::getSingleton().GetMainPlayer(), itemTemp->SoundActionType, EActionSound_Use);
	}

	C2G_USE_ITEM msg;
	msg.wMsg = c2g_use_item;
	msg.byConSlot = ConSlot;
	msg.byItemSlot = ItemSlot;

	_Send(&msg, sizeof(msg));

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

	MMOGameEvent_Guide evtGuide(EGUET_UseItem);
	MMOUI::PostGameEvent(&evtGuide);

	MMOHandbookManager::getSingleton().NotifyUseStoryScroll(TemplateID);

}
//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_DeleteItem( INT ConSlot , INT ItemSlot , INT ItemCount , INT TemplateId )
{
	FClientActionConditionContex contex(ECACCT_UseItem_CheckStallState);	
	if(!CheckClientActionCondition(contex))
	{
		return;
	}

	if( ConSlot == E_CONSLOT_EQUIPPED ){
		MMOEquipManager::getSingleton().BeforeServerEquipSwapHandleWeaponMainSkill( ConSlot, ItemSlot, E_CONSLOT_MAX, -1 );
	}

	C2G_DELETE_ITEM msg;
	msg.wMsg = c2g_delete_item;
	msg.byConSlot = ConSlot;
	msg.byItemSlot = ItemSlot;
	msg.shItemCount = ItemCount;

	_Send(&msg, sizeof(msg));

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_BuyContainerSlot( INT ConSlot , INT Count )
{
	C2G_BUY_CONTAINER_SLOT msg;
	msg.wMsg = c2g_buy_container_slot;
	msg.byConSlot = ConSlot;
	msg.byCount = Count;

	_Send(&msg, sizeof(msg));

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}

void CMessageHandler::Send_RepairAll()
{
	FClientActionConditionContex contex;
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	C2G_REPAIR_ALL_EQUIPPED_ITEM msg;
	msg.wMsg = c2g_repair_all_equipped_item;
	_Send(&msg, sizeof(msg));
}


//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_RepairItem(INT ConSlot , INT ItemSlot)
{
	FClientActionConditionContex contex;
	contex.Item = MMOFacade::getSingleton().FindItem(ConSlot, ItemSlot);
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	C2G_REPAIR_ITEM msg;
	msg.wMsg = c2g_repair_item;
	msg.byConSlot = ConSlot;
	msg.byItemSlot = ItemSlot;

	_Send(&msg, sizeof(msg));

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);
}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_SpecRepairItem(INT ConSlot , INT ItemSlot)
{
	FClientActionConditionContex contex;
	contex.Item = MMOFacade::getSingleton().FindItem(ConSlot, ItemSlot);
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	C2G_SPECIAL_REPAIR_ITEM msg;
	msg.wMsg = c2g_special_repair_item;
	msg.byConSlot = ConSlot;
	msg.byItemSlot = ItemSlot;

	_Send(&msg, sizeof(msg));

	//MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);
}
//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_IdentifyItem( INT ConSlot, INT ItemSlot )
{
	//FClientActionConditionContex contex;
	//contex.Item = MMOFacade::getSingleton().FindItem(ConSlot, ItemSlot);
	//ClientActionCondition_ItemAction checker;
	//if(!checker.CheckClientActionCondition(contex))
	//{
	//	return;
	//}

	//C2G_IDENTIFY_ITEM msg;
	//msg.wMsg = c2g_identify_item;
	//msg.byConSlot = ConSlot;
	//msg.byItemSlot = ItemSlot;

	//_Send(&msg, sizeof(msg));

	//MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}

//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_SignatureItem( INT ConSlot, INT ItemSlot )
{
	//FClientActionConditionContex contex;
	//contex.Item = MMOFacade::getSingleton().FindItem(ConSlot, ItemSlot);
	//ClientActionCondition_ItemAction checker;
	//if(!checker.CheckClientActionCondition(contex))
	//{
	//	return;
	//}

	//C2G_SIGN_ITEM msg;
	//msg.wMsg = c2g_sign_item;
	//msg.byConSlot = ConSlot;
	//msg.byItemSlot = ItemSlot;
	//_Send(&msg, sizeof(msg));

	//MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}

////---------------------------------------------------------------------
void CMessageHandler::Send_SellItem(INT ConSlot , INT ItemSlot , INT ItemCount , INT TemplateId )
{
	FClientActionConditionContex contex;
	contex.Item = MMOFacade::getSingleton().FindItem(ConSlot, ItemSlot);
	if(!contex.Item)
	{
		warnf(NAME_Error, TEXT("Item not found ContainerID:%d SlotID:%d"), ConSlot, ItemSlot);
		return;
	}

	contex.ItemTemplate = MMOFacade::getSingleton().FindItemTemplate(contex.Item->m_nTemplateID);
	if(!contex.ItemTemplate)
	{
		warnf(NAME_Error, TEXT("Item template not found ContainerID:%d SlotID:%d ItemTemplateID:%d"), ConSlot, ItemSlot, contex.Item->m_nTemplateID);
		return;
	}

	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	INT money = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_01);
	money += contex.ItemTemplate->SellPrice;
	if(money > MAX_MONEY)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(
			MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.MaxMoneyLimit")),MsgInfoType_Table,TEXT("MMOGame.Bag.MaxMoneyLimit"));
		return;
	}

	C2G_SELL_ITEM msg;
	msg.wMsg = c2g_sell_item;
	msg.byConSlot = ConSlot;
	msg.byItemSlot = ItemSlot;
	msg.shItemCount = ItemCount;
	_Send(&msg, sizeof(msg) );

	// 姣忔鍗栨帀灏卞埛鏂颁竴
	Send_ViewSoldItems();

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}


void CMessageHandler::Send_SplitItem( UINT16 srcConSlot , UINT16 srcItemSlot, UINT16 dstConSlot , UINT16 dstItemSlot, INT32 count )
{
	FClientActionConditionContex contex;
	contex.Item = MMOFacade::getSingleton().FindItem(srcConSlot, srcItemSlot);
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	C2G_SPLIT_ITEM msg;
	msg.wMsg = c2g_split_item;
	msg.bySrcConSlot = srcConSlot;
	msg.bySrcItemSlot = srcItemSlot;
	msg.byDstConSlot = dstConSlot;
	msg.byDstItemSlot = dstItemSlot;
	msg.shItemCount = count;
	_Send(&msg, sizeof(msg) );

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}

void CMessageHandler::Send_SortBag(UINT16 containerID)
{
	C2G_ORGANIZE_BAG msg;
	msg.wMsg = c2g_organize_bag;
	msg.byConSlot = containerID;
	_Send(&msg, sizeof(C2G_ORGANIZE_BAG));

	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);

}

//======================================================================================================
//void CMessageHandler::Send_EquipLevelUp( const FOperationItemBagPos &equipPos, 
//	const FOperationItemBagPos (&materialPos)[MAX_EQUIP_LEVEL_UP_MATERIAL_COUNT], 
//	const FOperationItemBagPos (&helpItemPos)[MAX_EQUIP_LEVEL_UP_LUCK_JEWELRY_COUNT] )
//{
//	C2G_EQUIP_LEVEL_UP msg;
//	msg.wMsg = c2g_equip_level_up;
//
//	msg.byEquipConSlot = equipPos._ConID;
//	msg.byEquipItemSlot = equipPos._SlotID;
//
//	INT itemCnt = 0;
//	for( INT i = 0; i < MAX_EQUIP_LEVEL_UP_MATERIAL_COUNT; ++i ){
//		if( materialPos[i]._ConID > 0 && materialPos[i]._SlotID > 0 ){
//			msg.byMaterialConSlot[itemCnt] = materialPos[i]._ConID;
//			msg.byMaterialItemSlot[itemCnt] = materialPos[i]._SlotID;
//
//			++itemCnt;
//		}
//	}
//	msg.byMaterialCount = itemCnt;
//
//	itemCnt = 0;
//	for( INT i = 0; i < MAX_EQUIP_LEVEL_UP_LUCK_JEWELRY_COUNT; ++i ){
//		if( helpItemPos[i]._ConID > 0 && helpItemPos[i]._SlotID > 0 ){
//			msg.byJewelryConSlot[itemCnt] = helpItemPos[i]._ConID;
//			msg.byJewelryItemSlot[itemCnt] = helpItemPos[i]._SlotID;
//
//			++itemCnt;
//		}
//	}
//	msg.byLuckJewelryCount = itemCnt;
//
//	_Send( &msg, sizeof(C2G_EQUIP_LEVEL_UP) );
//}

//===============================================================================================
void CMessageHandler::Send_EquipLevelUpShortcut_PutIn( int conID, int slotID )
{
	C2G_WEAPON_LEVEL_UP_PUT_IN msg;
	msg.wMsg = c2g_weapon_level_up_put_in;

	//msg.byEquipConSlot = conID;
	//msg.byEquipItemSlot = slotID;

	_Send( &msg, sizeof(C2G_WEAPON_LEVEL_UP_PUT_IN) );
}

void CMessageHandler::Send_EquipLevelUpShortcut_TakeOut()
{
	C2G_WEAPON_LEVEL_UP_TAKE_OUT msg;
	msg.wMsg = c2g_weapon_level_up_take_out;

	//msg.byEquipConSlot = conID;
	//msg.byEquipItemSlot = slotID;

	_Send( &msg, sizeof(C2G_WEAPON_LEVEL_UP_TAKE_OUT) );
}

void CMessageHandler:: Send_EquipLevelUpRightNow_TakeOut()
{
	C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY msg;
	msg.wMsg = c2g_weapon_level_up_take_out_immediately;

	_Send( &msg, sizeof(C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY) );
}

void CMessageHandler::Send_SellAllItemInBagResource()
{
	C2G_SELL_RESOURCE_ITEM msg;
	msg.wMsg = c2g_sell_resource_item;
	_Send( &msg, sizeof(C2G_SELL_RESOURCE_ITEM) );
	MMOUI::GetUIManager()->SetItemSlotWaitingState(TRUE);
}
//------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_weapon_level_up_put_in(const char* buf, UINT32 size)
{
	G2C_WEAPON_LEVEL_UP_PUT_IN* msg = (G2C_WEAPON_LEVEL_UP_PUT_IN*)buf;
	if (!buf || size < sizeof(G2C_WEAPON_LEVEL_UP_PUT_IN))
		return;
	if( msg == NULL ){
		return;
	}

	MMOGameEvent_Any weaponLvUpEvent( GameEvent_WeaponLevelUpPutIn );
	MMOUI::PostGameEvent( &weaponLvUpEvent );
}
void CMessageHandler::Parse_g2c_weapon_level_up_take_out(const char* buf, UINT32 size)
{
	G2C_WEAPON_LEVEL_UP_TAKE_OUT* msg = (G2C_WEAPON_LEVEL_UP_TAKE_OUT*)buf;
	if (!buf || size < sizeof(G2C_WEAPON_LEVEL_UP_TAKE_OUT))
		return;
	if( msg == NULL ){
		return;
	}

	MMOGameEvent_Any weaponLvUpEvent( GameEvent_WeaponLevelUpTakeOut );
	MMOUI::PostGameEvent( &weaponLvUpEvent );
}
void CMessageHandler::Parse_g2c_weapon_level_up_result(const char* buf, UINT32 size)
{
	G2C_WEAPON_LEVEL_UP_RESULT* msg = (G2C_WEAPON_LEVEL_UP_RESULT*)buf;
	if (!buf || size < sizeof(G2C_WEAPON_LEVEL_UP_RESULT))
		return;
	if( msg == NULL ){
		return;
	}

	MMOGameEvent_OneInt weaponLvUpEvent( GameEvent_WeaponLevelUpResult, msg->byResult );
	MMOUI::PostGameEvent( &weaponLvUpEvent );
}
////------------------------------------------------------------------------------------------------------
//void CMessageHandler::Parse_g2c_equip_level_up(const char* buf, UINT32 size)
//{
//	G2C_EQUIP_LEVEL_UP* msg = (G2C_EQUIP_LEVEL_UP*)buf;
//	if (!buf || size < sizeof(G2C_EQUIP_LEVEL_UP))
//		return;
//
//	UMMOUIEquipLevelUp *pEquipLvUp = (UMMOUIEquipLevelUp*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_EquipLevelUp );
//	if( pEquipLvUp != NULL ){
//		pEquipLvUp->NotifyLevelUpResult( msg->byResult, msg->byDurResult );
//	}
//}

//======================================================================================================
/*
//寮哄寲
void CMessageHandler::Send_Refine( UINT16 wItemCon, UINT16 wItemSlot )
{
	FClientActionConditionContex contex;
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	C2G_REFINE_ITEM msg;
	msg.wMsg = c2g_refine_item;
	msg.byConSlot = wItemCon;
	msg.byItemSlot = wItemSlot;
	_Send( &msg, sizeof(msg) );
}

//enhance result
void CMessageHandler::Parse_g2c_refine_item(const char* buf, UINT32 size)
{
	G2C_REFINE_ITEM* msg = (G2C_REFINE_ITEM*)buf;
	if (!buf || size < sizeof(G2C_REFINE_ITEM))
		return;

	UMMOUIEquipEnhance *pEnhanceUi = (UMMOUIEquipEnhance*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_EquipEnhance );
	if( pEnhanceUi != NULL ){
		pEnhanceUi->NotifyMakeEnhanceItemResult( (UBOOL)(msg->byResult) );
	}
}

//
void CMessageHandler::Send_Unrefine( UINT16 wItemSlot, UINT8 bAll )
{
	FClientActionConditionContex contex;
	ClientActionCondition_ItemAction checker;
	if(!checker.CheckClientActionCondition(contex))
	{
		return;
	}

	//鎹寮哄寲娌℃湁浜嗗摝
	//C2G_UNREFINE_ITEM msg;
	//msg.wMsg = c2g_unrefine_item;
	//msg.bAll = bAll;
	//msg.wItemSlot = wItemSlot;
	//_Send( &msg, sizeof(msg) );
}

//
void CMessageHandler::Parse_g2c_transfer_refine_item(const char* buf, UINT32 size)
{
	G2C_TRANSFER_REFINE_ITEM* msg = (G2C_TRANSFER_REFINE_ITEM*)buf;
	if (!buf || size < sizeof(G2C_TRANSFER_REFINE_ITEM))
		return;

}
*/
//////////////////////////////////////////////////////////////////////////
////瑙ｆ瀽浠撳簱灏哄
void CMessageHandler::Parse_g2c_sync_container_sizes(const char* buf, UINT32 size)
{
	G2C_SYNC_CONTAINER_SIZES* msg = (G2C_SYNC_CONTAINER_SIZES*)buf;
	if (!buf || size < sizeof(G2C_SYNC_CONTAINER_SIZES))
		return;
	
	MMOFacade::getSingleton().SetContainerSize( msg->abySizes );

	//AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetMainActor());
	if( player && player->ItemManager )
	{
		//	player->eventCreateItemManager();
		//player->ItemManager->CurCapacity(CONSLOT_EQUIPMENT)= msg->abySizes[CONSLOT_EQUIPMENT];
		player->ItemManager->CurCapacity(CONSLOT_DEFAULT) = msg->abySizes[CONSLOT_DEFAULT];
		//player->ItemManager->CurCapacity(CONSLOT_MATERIAL) = msg->abySizes[CONSLOT_MATERIAL];
		//player->ItemManager->CurCapacity(CONSLOT_QUEST)= msg->abySizes[CONSLOT_QUEST];
		player->ItemManager->CurCapacity(CONSLOT_BANK) = msg->abySizes[CONSLOT_BANK];
		player->ItemManager->CurCapacity(CONSLOT_RESOURCE) = msg->abySizes[CONSLOT_RESOURCE];//资源包

		player->ItemManager->BagItemList.Empty();
		player->ItemManager->BagQuestList.Empty();
		player->ItemManager->BagBankList.Empty();
		player->ItemManager->BagDepositList.Empty();
		player->ItemManager->BagResourceList.Empty();
	
		player->ItemManager->BagItemList.AddZeroed( UCONST_MaxBagSlotTotalCount ); 
		player->ItemManager->BagQuestList.AddZeroed( UCONST_MaxBagSlotTotalCount ); //锟斤拷锟斤拷锟斤拷锟斤拷
		player->ItemManager->BagBankList.AddZeroed( UCONST_MaxWarehouseSlotCount );
		player->ItemManager->BagDepositList.AddZeroed( /*UCONST_MaxBagDepositCount*/DEPOSIT_ITEM_SLOT_TOTAL ); 
		player->ItemManager->BagResourceList.AddZeroed(UCONST_MaxBagResourceSlotTotalCount);

		// 鎴愬氨绯荤粺鐩戝惉: 浠撳簱锛岃儗鍖呮牸瀛愭暟閲忓彉鏇		
		MMOGameEvent_Achievement evtAch1(adBagSlotCount, msg->abySizes[CONSLOT_DEFAULT], 0, 0);
		MMOUI::PostGameEvent(&evtAch1);
		MMOGameEvent_Achievement evtAch2(adBankSlotCount, msg->abySizes[CONSLOT_BANK], 0, 0);
		MMOUI::PostGameEvent(&evtAch2);

		MMOGameEvent_Guide evtGuide(EGUET_OpenGas);
		MMOUI::PostGameEvent(&evtGuide);
	}

	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);

}
//------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_lose_pk_item(const char* buf, UINT32 size)
{
	//目前先这么写，等服务器改协议
	G2C_LOSE_PK_ITEM* msg = (G2C_LOSE_PK_ITEM*)buf;
	if (!buf || size < sizeof(G2C_LOSE_PK_ITEM))
		return;
	if(msg)
	{
		ItemInfo iteminfo;
		if (!iteminfo.Load(msg->acItem, size - sizeof(*msg)))
        return;
		FClientItemInfo clientItemInfo;
		memset(&clientItemInfo, 0, sizeof(FClientItemInfo));
		memcpy(&clientItemInfo, &iteminfo, sizeof(ItemInfo));

		UItem* item = MMOFacade::getSingleton().FindItemTemplate( iteminfo.m_nTemplateID );
		FriendManager::getSingleton().AddKilledDropItem( item , &clientItemInfo );
	}
	//
}


void CMessageHandler::Parse_g2c_destroy_death_item(const char* buf, UINT32 size)
{
	G2C_DESTROY_DEATH_ITEM* msg = (G2C_DESTROY_DEATH_ITEM*)buf;
	if (!buf || size < sizeof(G2C_DESTROY_DEATH_ITEM))
		return;
	ItemInfo item;
	if (!item.Load(msg->acItem, size - sizeof(*msg)))
        return;

	UItem* itemTemplate = MMOFacade::getSingleton().FindItemTemplate(item.m_nTemplateID);
	if (itemTemplate && itemTemplate->IsA(UEquip::StaticClass()))
	{
		BYTE MeshType = EBMT_None;
		UEquip* equip = (UEquip*)itemTemplate;
		/*
		UEquipAppearence* ea = equip->GetEquipAppearenceTemplate();
		if (ea)
		{
			AMMOActor* MainPlayer = MMOFacade::getSingleton().GetMainActor();
			check(MainPlayer);
			MainPlayer->DetachEquip(ea->EquipType);
		}
		*/

		// fix bug #23121
		if (equip)
		{
			for (INT i = 0; i < equip->MeshList.Num(); ++i)
			{
				if (equip->MeshList(i))
				{
					MeshType = equip->MeshList(i)->MeshType;
				}
			}
		}

		AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainActor();
		if (mainPlayer && MeshType >= 0 && MeshType < EBMT_None)
		{
			mainPlayer->DetachEquip(MeshType);
		}

		// todo : show message
		FString msg = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Item.Destroy")),*(itemTemplate->ItemName));
		MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOGame.Item.Destroy"));
	}

	
}



//------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_item_delete(const char* buf, UINT32 size)
{
	G2C_ITEM_DELETE* msg = (G2C_ITEM_DELETE*)buf;
	if (!buf || size < sizeof(G2C_ITEM_DELETE))
		return;
	AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();

	UItem* titem = MMOFacade::getSingleton().FindItemTemplate( msg->byConSlot, msg->byItemSlot );
	FClientItemInfo* clientItem = MMOFacade::getSingleton().FindItem( msg->byConSlot , msg->byItemSlot );

	if( player && player->ItemManager)
	{
		if( msg->byConSlot == CONSLOT_EQUIPPED )
		{
			INT EquipType = GetClientEquipTypeByServer(msg->byItemSlot);
			player->ItemManager->RequestTakeOff(EquipType, true);
			player->ItemManager->UpdateOwnerData();
			//player->ItemManager->EquippedItemList_Normal( EquipType) = NULL;
			if( ctrl->myHUD &&  ((AMMOHud*)ctrl->myHUD)->HudMovie  &&  ((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar &&((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager )
			{
				((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->RequestTakeOff(EquipType, true);
				((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->UpdateOwnerData();
			}
		}
		//else if(msg->wConSlot == CONSLOT_HERO_EQUIPPED)
		//{
		//	AMMOActor* pHero = MMOFacade::getSingleton().GetMainHero();
		//	if(pHero){
		//		INT EquipType = GetClientEquipTypeByServer(msg->wItemSlot);
		//		pHero->ItemManager->RequestTakeOff( EquipType, true );
		//		//if( ctrl->myHUD &&  ((AMMOHud*)ctrl->myHUD)->HudMovie  &&  ((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar &&((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager )
		//		//{
		//		//	((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->RequestTakeOff( EquipType  , true );	
		//		//	((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->UpdateOwnerData();
		//		//}
		//	}
		//}
		else
		{
			//取消闪烁提示
			if( clientItem && clientItem->NeedHintMode )
			{
				clientItem->NeedHintMode = ItemSlotHint_Normal;
				MMOGameEvent_BagSlotHintChange msg( msg->byConSlot , msg->byItemSlot , clientItem );
				MMOUI::PostGameEvent(&msg);
			}	

			player->ItemManager->DeleteItemBySlot(msg->byConSlot,msg->byItemSlot);
		}
	}
	//MMOFacade::getSingleton().NotifyItemDelete(msg->wConSlot,msg->wItemSlot);

	INT Count = 1;

	if( titem && clientItem )
	{
	/*	switch(titem->ItemDuration)
		{
		case E_DURTYPE_STACK:
			if(clientItem->IsOnSale)
			{
				Count = clientItem->OriDur;
			}
			else
			{
				Count = clientItem->m_nCurDur;
			}
			
			break;
		
		}*/
		Count = clientItem->GetCurStackCount(titem);
		FString msg = FString::Printf(TEXT("%s%s x%d"),
			*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemTips.LostItem")),
			*LinkManager::HtmlLink_ColorItem(titem, clientItem),
			Count );

		MMOChatManager::getSingleton().AddSystemMessage( msg );
	}

	MMOFacade::getSingleton().DeleteItem(msg->byConSlot,msg->byItemSlot);
	if( titem ){
		MMOGameEvent_Item itemEvt(GameEvent_Item_Delete, msg->byConSlot, msg->byItemSlot );
		itemEvt.ItemTemplateID = titem->GetTemplateID();
		MMOUI::PostGameEvent( &itemEvt );

		MMOGameEvent_Any evt(GameEvent_BagWeight);
		MMOUI::PostGameEvent(&evt);

		MMOQuestManager::getSingleton().UpdateQuestItem(titem->GetTemplateID(), FALSE);
	}
	else{
		warnf(NAME_Warning, TEXT("CMessageHandler::Parse_g2c_item_delete Item is already not exist(%d,%d)!"), msg->byConSlot, msg->byItemSlot);
	}

	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);

	//添加魔血石提示
	if (msg->byConSlot == CONSLOT_RECOVER)
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Recover.Delete")), *titem->ItemName);
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,text);
	}

	
}

//------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_item_change(const char* buf, UINT32 size)
{
	G2C_ITEM_CHANGE* msg = (G2C_ITEM_CHANGE*)buf;
	if (!buf || size < sizeof(G2C_ITEM_CHANGE))
		return;
	//check(sizeof(ITEM_INFO) == (msg->wSize - sizeof(G2C_ITEM_CHANGE)));

	ItemInfo iteminfo;
    if (!iteminfo.Load(msg->acItem, size - sizeof(*msg)))
        return;
	FClientItemInfo clientItemInfo;
	memset(&clientItemInfo, 0, sizeof(FClientItemInfo));
	memcpy(&clientItemInfo, &iteminfo, sizeof(ItemInfo));
	
    AMMOActor* player = MMOFacade::getSingleton().GetMainActor();

    if( !player )
    {
        return;
    }

	UItem* titem = MMOFacade::getSingleton().FindItemTemplate( iteminfo.m_nTemplateID );

	if( titem == NULL )
	{
		debugf(NAME_Error, TEXT("[CMessageHandler::Parse_g2c_item_change] can't find itemTemplate"));
		return;
	}

	FClientItemInfo* clientItem = MMOFacade::getSingleton().FindItem( iteminfo.m_byConSlot , iteminfo.m_byItemSlot );
	
	UBOOL isNewItem = FALSE;
	INT Count = 1;
	if(clientItem)
	{
		//已有道具
		switch(titem->ItemDuration)
		{
		case E_DURTYPE_STACK:

            if( clientItem->IsOnSale )
            {
                Count = iteminfo.m_nCurDur  - clientItem->OriDur;
            }
            else
            {
                Count = iteminfo.m_nCurDur  - clientItem->m_nCurDur;
            }
			
			if( Count > 0 )
			{
				FString msg;
				msg = FString::Printf(TEXT("%s%s x%d"),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemTips.ObtainNew")),
					*LinkManager::HtmlLink_ColorItem(titem, &clientItemInfo),
					Count );
				MMOChatManager::getSingleton().AddSystemMessage( msg );
				isNewItem = TRUE;
			}
			else if( Count < 0 )
			{
				FString msg;
				msg = FString::Printf(TEXT("%s%s x%d"),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemTips.LostItem")),
					*LinkManager::HtmlLink_ColorItem(titem, &clientItemInfo),
					-Count );
				MMOChatManager::getSingleton().AddSystemMessage( msg );
			}
			break;
		}

		if( titem->ItemType == TEXT("ITEMTYPE_EQUIP") )
		{
			if( clientItem->m_byLuck > iteminfo.m_byLuck && player->ActorAttributes->PkValue > 0)
			{
				//幸运值降低了，PK可能降低幸运值
				FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Combat.LuckReduce")) + appItoa(abs(clientItem->m_byLuck - iteminfo.m_byLuck));
				//MMOChatManager::getSingleton().AddHintMessage( msg );
				FString str2 = TEXT("MMOGame.Combat.LuckReduce");
				MMOUI::GetUIManager()->delegateAddScreenMessage( msg , MsgInfoType_Table , str2 );
			}

			if( clientItem->m_nMaxDur > iteminfo.m_nMaxDur )
			{
				//最大耐久度掉了
				INT cvtOrigMaxDurability = ( clientItem->m_nMaxDur + EQUIP_DUR_MULTIPLIER - 1 ) / EQUIP_DUR_MULTIPLIER;
				INT cvtCurMaxDurability = ( iteminfo.m_nMaxDur + EQUIP_DUR_MULTIPLIER - 1 ) / EQUIP_DUR_MULTIPLIER;
				INT durDownValue = cvtOrigMaxDurability - cvtCurMaxDurability;
				if (durDownValue > 0)
				{
					FString hintTextName = TEXT("MMOGame.EquipDurability.DurDownHint");
					FString hintText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( hintTextName ), *titem->GetDisplayName(), durDownValue );
					MMOUI::GetUIManager()->delegateAddScreenMessage( hintText , MsgInfoType_Table , hintTextName );

					MMOChatManager::getSingleton().AddSystemMessage( hintText );
				}
				
			}
		}
	}
	else
	{
		//新道具
		Count = iteminfo.GetCurStackCount(titem);

		//if( titem->ItemType != TEXT("ITEMTYPE_EQUIP") )
		//{
			FString msg = FString::Printf(TEXT("%s%s x%d"),
				*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemTips.ObtainNew")),
				*LinkManager::HtmlLink_ColorItem(titem, &clientItemInfo),
				Count );

			MMOChatManager::getSingleton().AddSystemMessage( msg );
		//}

		isNewItem = TRUE;
	} 

	FClientItemInfo* newIteminfo = MMOFacade::getSingleton().UpdateItem(&iteminfo);	

	if( titem->ItemType == TEXT("ITEMTYPE_EQUIP") )
	{
		MMOPowerManager::getSingleton().UpdateEquipPower(newIteminfo);
		
	}
	
	if (NULL != clientItem && clientItem->m_dwRoleID != 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.SignSucess")) ,MsgInfoType_Table,TEXT("MMOGame.Bag.SignSucess"));
	}
	else
		warnf(NAME_Warning, TEXT("CMessageHandler::Parse_g2c_item_change Item is not exist!"));
	
   
	if( titem)
	{
		UItem* factItem = NULL;
		if( iteminfo.m_byConSlot == CONSLOT_EQUIPPED )
		{
			factItem = player->ItemManager->EquippedItemList_Normal( iteminfo.m_byItemSlot );
			if( !factItem || factItem->ItemId != iteminfo.m_nTemplateID  )
			{
				factItem = player->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer(iteminfo.m_byItemSlot), true, titem );

				player->ItemManager->UpdateOwnerData();
			}

			//equip durability changed
			MMOGameEvent_Item itemEvt( GameEvent_EquipDurChanged, CONSLOT_EQUIPPED, iteminfo.m_byItemSlot );
			MMOUI::PostGameEvent( &itemEvt );
		}
		else if(iteminfo.m_byConSlot == CONSLOT_HERO_EQUIPPED)
		{
			factItem = player->ItemManager->eventAddItemToBag( titem, iteminfo.m_byItemSlot , iteminfo.m_byConSlot,true);
			MMOUI::GetUIManager()->delegateRefreshHeroSlot();
		}
		else
		{
			//锟斤拷锟斤拷一锟斤拷True锟斤拷锟斤拷锟结播锟斤拷锟斤拷品拾取锟斤拷锟斤拷  
			factItem = player->ItemManager->eventAddItemToBag( titem, iteminfo.m_byItemSlot , iteminfo.m_byConSlot,true);
		}

		if( factItem )
		{
			//add by jimzhang@snda.com-----------------------------
			factItem->DefaultDuration = iteminfo.m_nCurDur;
			factItem->MaxStackNum	  = iteminfo.m_nMaxDur;
			//-----------------------------------------------------
			//factItem->byPrefixLevel = tmpInfo->m_byPrefixLevel;

			//factItem->AttributeList.Empty();
			//factItem->AttributeList.AddZeroed( catTotal);

			//for ( int j = 0 ; j< tmpInfo->m_byAttrCount; ++j)
			//{
			//	int shAttrType = tmpInfo->m_attrs[j].shAttrType;
			//	if( factItem->AttributeList.Num() > shAttrType &&  shAttrType> 0)
			//		factItem->AttributeList( shAttrType ) = tmpInfo->m_attrs[j].shParam;
			//}
		}

	}

	//
	MMOFacade::getSingleton().NotifyBagSlotChange( iteminfo.m_byConSlot,iteminfo.m_byItemSlot );
	MMOGameEvent_Item itemEvt( GameEvent_Item_SlotUpdated, iteminfo.m_byConSlot, iteminfo.m_byItemSlot );
	MMOUI::PostGameEvent( &itemEvt );
	debugf( TEXT("[CMessageHandler::Parse_g2c_item_change] %d"), iteminfo.m_nTemplateID );

	//
	if (isNewItem)
	{
		MMOGameEvent_Item itemEvt(GameEvent_Item_Gain, iteminfo.m_byConSlot, iteminfo.m_byItemSlot);
		MMOUI::PostGameEvent(&itemEvt);

		// 成就
		if (titem->ItemType == TEXT("ITEMTYPE_EQUIP"))
		{
			MMOGameEvent_Achievement evtAch1(adEquip, iteminfo.m_nTemplateID, 0, 0);
			MMOUI::PostGameEvent(&evtAch1);
			MMOGameEvent_Achievement evtAch2(adEquipColor, titem->ItemColor, 0, 0);
			MMOUI::PostGameEvent(&evtAch2);
		}

		MMOGameEvent_Achievement evtAch3(adGetItem, iteminfo.m_nTemplateID, 0, 0);
		MMOUI::PostGameEvent(&evtAch3);

		// 任务
		MMOQuestManager::getSingleton().UpdateQuestItem(iteminfo.m_nTemplateID, TRUE);
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_GetItem, iteminfo.m_nTemplateID);

		// 新手引导
		MMOGameEvent_Guide evtGuide(EGUET_GetItem, iteminfo.m_nTemplateID);
		MMOUI::PostGameEvent(&evtGuide);

		if (MMOFacade::getSingleton().IsBagFull())
		{
			MMOGameEvent_Guide evtGuide(EGUET_BagFull);
			MMOUI::PostGameEvent(&evtGuide);
		}
	}

	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);

	MMOGameEvent_Any evt(GameEvent_BagWeight);
	MMOUI::PostGameEvent(&evt);
}
//------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_equip_dur_change(const char* buf, UINT32 size)
{
	G2C_EQUIP_DUR_CHANGE* msg = (G2C_EQUIP_DUR_CHANGE*)buf;
	if (!buf || size < sizeof(G2C_EQUIP_DUR_CHANGE))
		return;
	MMOFacade::getSingleton().UpdateItemDurability( msg->byConSlot, msg->byEquipSlot, msg->nDur );
}
//------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_item_swap(const char* buf, UINT32 size)
{
	G2C_ITEM_SWAP* msg = (G2C_ITEM_SWAP*)buf;
	if (!buf || size < sizeof(G2C_ITEM_SWAP))
		return;
	AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	debugf( TEXT("[CMessageHandler::Parse_g2c_item_swap] [%d.%d] -> [%d.%d]"), 
		msg->bySrcConSlot, msg->bySrcItemSlot, msg->byDstConSlot, msg->byDstItemSlot );

	//交换
	MMOFacade::getSingleton().SwapItem(*msg);

	//test log
	//if( msg->byDstConSlot == E_CONSLOT_EQUIPPED )
	//{
	//	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(msg->byDstConSlot, msg->byDstItemSlot);
	//	if( itemTemp )
	//	{
	//		debugf( TEXT("[换装测试] 装备%s 从[%d.%d] -> [%d.%d]"), *itemTemp->ItemName,
	//			msg->bySrcConSlot, msg->bySrcItemSlot, msg->byDstConSlot, msg->byDstItemSlot );
	//	}
	//	
	//}
	

	//重设套装战斗力
	if( msg->bySrcConSlot == E_CONSLOT_EQUIPPED || msg->byDstConSlot == E_CONSLOT_EQUIPPED )
	{
		MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_SuitPow );
		MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_AllEquippedPow );	
	}
	
// 	UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(msg->bySrcConSlot, msg->bySrcItemSlot);
// 	if(itemTemp)
// 	{
// 		UGamePlayDataManager::GetSingleton()->PlayItemActionSounds(player, itemTemp->SoundActionType, EActionSound_MousePutdown);
// 	}
// 	else
// 	{
// 		itemTemp = MMOFacade::getSingleton().FindItemTemplate(msg->byDstConSlot, msg->byDstItemSlot);
// 		if(itemTemp)
// 		{
// 			UGamePlayDataManager::GetSingleton()->PlayItemActionSounds(player, itemTemp->SoundActionType, EActionSound_MousePutdown);
// 		}
// 	}

	if(msg->byDstConSlot == CONSLOT_HERO_EQUIPPED)
	{
		if( msg->bySrcConSlot != CONSLOT_DEFAULT)
		{
			warnf(L"Error, src is not equipment");
			return;
		}
		UItem* pBagItem = player->ItemManager->BagItemList(msg->bySrcItemSlot );
		player->ItemManager->BagItemList(msg->bySrcItemSlot ) = player->ItemManager->EquippedItemList_Hero(msg->byDstItemSlot);
		player->ItemManager->EquippedItemList_Hero(msg->byDstItemSlot) = (UEquip*)pBagItem;
		MMOUI::GetUIManager()->delegateRefreshHeroSlot();
	}
	else if(msg->bySrcConSlot == CONSLOT_HERO_EQUIPPED)
	{
		UItem* pBagItem = player->ItemManager->BagItemList(msg->byDstItemSlot );
		player->ItemManager->BagItemList(msg->byDstItemSlot ) = player->ItemManager->EquippedItemList_Hero(msg->bySrcItemSlot);
		player->ItemManager->EquippedItemList_Hero(msg->bySrcItemSlot) = (UEquip*)pBagItem;
		MMOUI::GetUIManager()->delegateRefreshHeroSlot();
	}

	if( msg->byDstConSlot == CONSLOT_EQUIPPED ) 
	{
		if( msg->bySrcConSlot != CONSLOT_DEFAULT)
		{
			warnf(L"Error, src is not equipment");
			return;
		}	
		INT EquipType = GetClientEquipTypeByServer(msg->byDstItemSlot);
		UItem* pDstItem = player->ItemManager->GetEquipBySlot( EquipType, msg->byDstConSlot == CONSLOT_EQUIPPED ) ;
		UItem* pSrcItem = player->ItemManager->BagItemList(msg->bySrcItemSlot);
		INT SrcItemId = 0;
		if (pSrcItem)
		{
			SrcItemId = pSrcItem->ItemId;
		}
		player->ItemManager->RequestTakeOn( msg->bySrcItemSlot, EquipType, msg->byDstConSlot == CONSLOT_EQUIPPED );
		player->ItemManager->BagItemList( msg->bySrcItemSlot ) = pDstItem;

		if( ctrl->myHUD && ( (AMMOHud*)ctrl->myHUD)->HudMovie  &&  ((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar&&((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager )
		{
			UItem* pAvatarItemTemplate = ( MMOFacade::getSingleton().FindItemTemplate( SrcItemId ));
			if(pAvatarItemTemplate)
				((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->RequestTakeOn( -1, EquipType  , msg->byDstConSlot == CONSLOT_EQUIPPED , pAvatarItemTemplate ,false );
			((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->UpdateOwnerData();
		}
		player->ItemManager->UpdateOwnerData();

		// 改走新手引导 MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Character , TRUE, "" );

		MMOGameEvent_Item itemEvt(GameEvent_Equip, msg->byDstConSlot, msg->byDstItemSlot );
		MMOUI::PostGameEvent( &itemEvt );

		//equip durability changed
		MMOGameEvent_Item itemDurChangedEvt( GameEvent_EquipDurChanged, CONSLOT_EQUIPPED, msg->byDstItemSlot );
		MMOUI::PostGameEvent( &itemDurChangedEvt );

		MMOGameEvent_Guide evtGuide(EGUET_EquipWeapon, msg->byDstConSlot);
		MMOUI::PostGameEvent(&evtGuide);

		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(msg->byDstConSlot, msg->byDstItemSlot);
		if (itemTemp)
		{
			MMOGameEvent_Achievement evtAch(adEquipBodyColor, itemTemp->ItemColor, msg->byDstItemSlot, 0);
			MMOUI::PostGameEvent(&evtAch);

			// 引导任务
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_EquipItem, itemTemp->GetTemplateID());
		}

		MMOGameEvent_Achievement evtAch(adItemLuck, MMOEquipManager::getSingleton().CountEquipLuckTotal(), 0, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if( msg->bySrcConSlot == CONSLOT_EQUIPPED)
	{
		INT EquipType = GetClientEquipTypeByServer(msg->bySrcItemSlot);
		UEquip* pSrcItem = Cast<UEquip>( player->ItemManager->GetEquipBySlot( EquipType, msg->bySrcConSlot == CONSLOT_EQUIPPED )) ;//
		UEquip* pDstItem = Cast<UEquip>(player->ItemManager->BagItemList( msg->byDstItemSlot ) );
		if( pSrcItem == NULL  )
		{
			warnf(NAME_Error, TEXT("Error, src equip item is null"));
			return;
		}
		if(  pDstItem   )
		{
			if(pDstItem->GetEquipAppearenceTemplate() && pSrcItem->GetEquipAppearenceTemplate())
			{
				if( pDstItem->GetEquipAppearenceTemplate()->EquipType != pSrcItem->GetEquipAppearenceTemplate()->EquipType )
				{
					warnf(NAME_Error, TEXT("Error, two equips is not fit"));
					return;
				}
				INT DesItemId = player->ItemManager->BagItemList(msg->byDstItemSlot )->ItemId;
				player->ItemManager->RequestTakeOn( msg->byDstItemSlot, EquipType, msg->bySrcConSlot == CONSLOT_EQUIPPED );
				player->ItemManager->BagItemList(msg->byDstItemSlot ) = pSrcItem;
				if( ctrl->myHUD &&  ((AMMOHud*)ctrl->myHUD)->HudMovie  &&  ((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar &&((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager )
				{
					UItem* pAvatarItemTemplate = ( MMOFacade::getSingleton().FindItemTemplate( DesItemId )  );
					if(pAvatarItemTemplate)
						((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->RequestTakeOn( -1, EquipType  , msg->byDstConSlot == CONSLOT_EQUIPPED , pAvatarItemTemplate ,false );
					((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->UpdateOwnerData();
				}
			}

			//equip durability changed
			MMOGameEvent_Item itemEvt( GameEvent_EquipDurChanged, CONSLOT_EQUIPPED, msg->byDstItemSlot );
			MMOUI::PostGameEvent( &itemEvt );
		}
		else
		{
			player->ItemManager->RequestTakeOff(EquipType, msg->bySrcConSlot == CONSLOT_EQUIPPED);
			player->ItemManager->BagItemList(msg->byDstItemSlot ) = pSrcItem;
			if( ctrl->myHUD &&  ((AMMOHud*)ctrl->myHUD)->HudMovie  &&  ((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar &&((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager )
			{
				((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->RequestTakeOff(EquipType, true);
				((AMMOHud*)ctrl->myHUD)->HudMovie->ShowAvatar->ItemManager->UpdateOwnerData();
			}

			//equip durability changed
			MMOGameEvent_Item itemEvt( GameEvent_EquipDurChanged, CONSLOT_EQUIPPED, msg->bySrcItemSlot );
			MMOUI::PostGameEvent( &itemEvt );
		}
		
		player->ItemManager->UpdateOwnerData();

		MMOGameEvent_Item itemEvt(GameEvent_Equip, msg->bySrcConSlot, msg->bySrcItemSlot );
		MMOUI::PostGameEvent( &itemEvt );

		MMOGameEvent_Achievement evtAch(adItemLuck, MMOEquipManager::getSingleton().CountEquipLuckTotal(), 0, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else
	{
		player->ItemManager->RequestExchangeItem(msg->bySrcConSlot, msg->bySrcItemSlot, msg->byDstConSlot, msg->byDstItemSlot);		
	}

 	MMOFacade::getSingleton().NotifyBagSlotChange(msg->byDstConSlot,msg->byDstItemSlot);
	MMOFacade::getSingleton().NotifyBagSlotChange(msg->bySrcConSlot,msg->bySrcItemSlot);

	//
	MMOGameEvent_ItemSwap itemSwappedEvt( msg->bySrcConSlot, msg->bySrcItemSlot, msg->byDstConSlot, msg->byDstItemSlot );
	MMOUI::PostGameEvent( &itemSwappedEvt );

	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);

	MMOGameEvent_Any evt(GameEvent_BagWeight);
	MMOUI::PostGameEvent(&evt);

	// 背包内道具变动，通知任务系统
	if (msg->bySrcConSlot == CONSLOT_DEFAULT || msg->byDstConSlot == CONSLOT_DEFAULT)
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(msg->byDstConSlot, msg->byDstItemSlot);
		if (itemTemp)
		{
			MMOQuestManager::getSingleton().UpdateQuestItem(itemTemp->GetTemplateID(), FALSE);
		}
	}

#if defined(_DEBUG) && M_DEBUG_ITEM
	static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECONTAINER_SLOT"), TRUE);
	warnf(NAME_Warning, TEXT("=========item_swap [%s][%d] -> [%s][%d]"),
		*InEnum->GetEnum(msg->bySrcConSlot).ToString(),
		msg->wSrcItemSlot,
		*InEnum->GetEnum(msg->byDstConSlot).ToString(),
		msg->wDstItemSlot);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sign_item(const char* buf, UINT32 size)
{
	G2C_SIGN_ITEM* msg = (G2C_SIGN_ITEM*)buf;
	if (!buf || size < sizeof(G2C_SIGN_ITEM))
		return;
	//ItemInfo* info = MMOFacade::getSingleton().FindItem(msg->wConSlot, msg->wItemSlot);

	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);

}

//--------------------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_container_size_change(const char* buf, UINT32 size)
{
	G2C_CONTAINER_SIZE_CHANGE* msg = (G2C_CONTAINER_SIZE_CHANGE*)buf;
	if (!buf || size < sizeof(G2C_CONTAINER_SIZE_CHANGE))
		return;

	

	MMOFacade::getSingleton().SetContainerSize( ECONTAINER_SLOT( msg->byConSlot ) , msg->byConSize );

	AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetMainActor());
	if( player && player->ItemManager )
	{

		player->ItemManager->CurCapacity(msg->byConSlot)= msg->byConSize;
		
		// 鎴愬氨绯荤粺鐩戝惉: 浠撳簱锛岃儗鍖呮牸瀛愭暟閲忓彉	
		if ( msg->byConSlot == E_CONSLOT_BANK )
		{
			MMOGameEvent_Achievement evtAch(adBankSlotCount, msg->byConSize, 0, 0);
			MMOUI::PostGameEvent(&evtAch);

			MMOUI::GetUIManager()->delegateUpdateWarehouseCapSlot();
		}
		else if(msg->byConSlot == E_CONSLOT_RESOURCE)
		{

			MMOGameEvent_Any evtAch(GaemEvent_BagResourceSizeChange);
			MMOUI::PostGameEvent(&evtAch);
			
		}
		else
		{
			MMOUI::GetUIManager()->delegateUpdateBagCapSlot(msg->byConSlot);

			if ( msg->byConSlot == E_CONSLOT_DEFAULT )
			{
				MMOGameEvent_Achievement evtAch(adBagSlotCount, msg->byConSize, 0, 0);
				MMOUI::PostGameEvent(&evtAch);

				// 引导任务
				MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Extendbag);
			}
		}
	}
	
	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);

}

//
void CMessageHandler::Parse_g2c_luck_oil(const char* buf, UINT32 size)
{
	G2C_LUCK_OIL* msg = (G2C_LUCK_OIL*)buf;
	if (!buf || size < sizeof(G2C_LUCK_OIL))
		return;
	FString str;
	FString str2;
	if( msg->chResult > 0 )
	{
		str = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.LuckAdd")) + appItoa(msg->chResult);
		str2 = TEXT("MMOGame.Bag.LuckAdd");
	}
	else if( msg->chResult < 0 )
	{
		str = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.LuckReduce")) + appItoa(abs(msg->chResult));
		str2 = TEXT("MMOGame.Bag.LuckReduce");
	}
	else
	{
		str = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.LuckNoChange"));
		str2 = TEXT("MMOGame.Bag.LuckNoChange");
	}

	MMOChatManager::getSingleton().AddSystemMessage( str );
	MMOUI::GetUIManager()->delegateAddScreenMessage( str , MsgInfoType_Table , str2 );
}


//瑙ｆ瀽鍖呰９
void CMessageHandler::Parse_g2c_sync_item_bag(const char* buf, UINT32 size)
{
	G2C_SYNC_ITEM_BAG *msg = (G2C_SYNC_ITEM_BAG*)buf;
	if (!buf || size < sizeof(G2C_SYNC_ITEM_BAG))
		return;

	AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( !player  || !player->GpDataManager  )
		return;	

	BOOL bShort = MMOFacade::getSingleton().GetPlayerSort();
	if (!bShort)
	{
		MMOFacade::getSingleton().ClearItems();
		MMOFacade::getSingleton().ClearPlayerEquip();
	}
	else
	{
		//MMOGameEvent_ClearBag clearBagEvt;
		//MMOUI::PostGameEvent(&clearBagEvt);

		if (MMOFacade::getSingleton().GetPlayerSortType() == IST_Bag)
		{
			MMOFacade::getSingleton().ClearItemsByContainerId(E_CONSLOT_DEFAULT);
			

			if(player->ItemManager)
			{
				INT num = player->ItemManager->BagItemList.Num();
				for(INT i=0;i<num;i++)
				{
					player->ItemManager->DeleteItemBySlot(E_CONSLOT_DEFAULT,i);
				}
			}

			
		}
		else if (MMOFacade::getSingleton().GetPlayerSortType() == IST_BagResource)
		{
			MMOFacade::getSingleton().ClearItemsByContainerId(E_CONSLOT_RESOURCE);
			if(player->ItemManager)
			{
				for (int i = 0 ; i < player->ItemManager->BagResourceList.Num();i++)
				{
					player->ItemManager->DeleteItemBySlot(E_CONSLOT_RESOURCE,i);
				}
			}
		}
		else
		{
			MMOFacade::getSingleton().ClearItemsByContainerId(E_CONSLOT_BANK);
			if(player->ItemManager)
			{
				INT num = player->ItemManager->BagBankList.Num();
				for(INT i=0;i<num;i++)
				{
					player->ItemManager->DeleteItemBySlot(E_CONSLOT_BANK,i);
				}
			}
		}
	}



	INT BuffSize = 0;
	INT sortConId = MMOFacade::getSingleton().GetPlayerSortType();


	while (BuffSize < (INT)(size - sizeof(G2C_SYNC_ITEM_BAG)))
	{
        ItemInfo iteminfo;
        INT buflen = iteminfo.Load(msg->acItems + BuffSize, size - sizeof(*msg) - BuffSize);
        if (buflen == 0)
            return;

		BuffSize += buflen;
	
		if (bShort)
		{
			if ( iteminfo.m_byConSlot == sortConId || iteminfo.m_byConSlot == E_CONSLOT_RESOURCE)
			{
				INT containerId = iteminfo.m_byConSlot == E_CONSLOT_RESOURCE ? E_CONSLOT_RESOURCE:sortConId;
				FClientItemInfo* tmpInfo = MMOFacade::getSingleton().UpdateItemByContainerId(&iteminfo,containerId);
				player->ItemManager->eventAddItemToBag( MMOFacade::getSingleton().FindItemTemplate(iteminfo.m_nTemplateID), iteminfo.m_byItemSlot, iteminfo.m_byConSlot );
				
			}
			
		}
		else
		{
			FClientItemInfo* tmpInfo = MMOFacade::getSingleton().AddItem( &iteminfo , FALSE );

			UItem* item = MMOFacade::getSingleton().FindItemTemplate(iteminfo.m_nTemplateID);
			if(!item)
			{
				warnf(NAME_Error, TEXT("item not find [%d]"), iteminfo.m_nTemplateID);
				continue;
			}

			//装备需要计算战斗力
			//if( item->ItemType == TEXT("ITEMTYPE_EQUIP") && tmpInfo )
			//{
			//MMOPowerManager::getSingleton().UpdateEquipPower( tmpInfo );
			//}

			UItem* factItem = NULL;
			switch( iteminfo.m_byConSlot )
			{
			case CONSLOT_EQUIPPED:
				{
					INT slot = GetClientEquipTypeByServer( iteminfo.m_byItemSlot);					
					factItem = player->ItemManager->RequestTakeOn( -1, slot, true, item );
					//sync equipped weapon, equip weapon skill( server skill sync message does not contain skill runeSlotID )
					//It maybe better if server message could contain runeSlotID.
					if( iteminfo.m_byItemSlot == E_NES_MAIN_HAND ){
						MMOGameEvent_Any syncEquippedWeaponEvt( GameEvent_SyncEquippedWeapon );
						MMOUI::PostGameEvent( &syncEquippedWeaponEvt );
					}

					//equip durability changed
					MMOGameEvent_Item itemEvt( GameEvent_EquipDurChanged, CONSLOT_EQUIPPED, iteminfo.m_byItemSlot );
					MMOUI::PostGameEvent( &itemEvt );
				}
				break;
			case CONSLOT_HERO_EQUIPPED:
				{
					factItem = player->ItemManager->eventAddItemToBag( item, iteminfo.m_byItemSlot, iteminfo.m_byConSlot );
					MMOUI::GetUIManager()->delegateRefreshHeroSlot();
				}
				break;
			default:	
				factItem = player->ItemManager->eventAddItemToBag( item, iteminfo.m_byItemSlot, iteminfo.m_byConSlot );
				break;
			}

			if( factItem )
			{
				//add by jimzhang@snda.com-----------------------------
				factItem->DefaultDuration = iteminfo.m_nCurDur;
				factItem->MaxStackNum	  = iteminfo.m_nMaxDur;

			}

#if defined(_DEBUG) && M_DEBUG_ITEM

			static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECONTAINER_SLOT"), TRUE);
			warnf(NAME_Warning, TEXT("=========Sync bag[%s][%d] ->[%d][%s]"),
				*InEnum->GetEnum(iteminfo.m_byConSlot).ToString(),
				iteminfo.m_byItemSlot,
				item->ItemId,
				*item->GetDisplayName());
#endif
			MMOFacade::getSingleton().NotifyBagSlotChange(iteminfo.m_byConSlot, iteminfo.m_byItemSlot);
		}

		
	}

	MMOGameEvent_Any lockModeEvt(GameEvent_SortItemsInBagSuccess);
	
	switch( MMOFacade::getSingleton().GetLockSortMode() )
	{
	case LSM_SortBag:
	case IST_BagResource:
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.SortEnd") )  );		
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Bag.SortEnd") ) 
			, MsgInfoType_Table , TEXT("MMOGame.Bag.SortEnd") );
		//
		lockModeEvt.EventType = GameEvent_SortItemsInBagSuccess;
		MMOUI::PostGameEvent(&lockModeEvt);
		break;
	case LSM_SortWarehouse:
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Warehouse.SortEnd") ) );
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Warehouse.SortEnd") ) 
			, MsgInfoType_Table , TEXT("MMOGame.Warehouse.SortEnd") );
		break;
	default:
		break;
	}

	if (MMOFacade::getSingleton().IsBagFull())
	{
		// 新手引导
		MMOGameEvent_Guide evtGuide(EGUET_BagFull);
		MMOUI::PostGameEvent(&evtGuide);
	}

	////
	//MMOGameEvent_Any evt( GameEvent_SyncItemSuccess );
	//MMOUI::PostGameEvent(&evt);

	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);
	
	if (!bShort)
	{
		player->ItemManager->UpdateOwnerData();
		MMOQuestManager::getSingleton().UpdateAllQuestItem();
	}
	else
	{
		MMOGameEvent_Any sortEvt(GameEvent_BagSortEnd);
		MMOUI::PostGameEvent(&sortEvt);
		//MMOFacade::getSingleton().SetPlayerSortType(IST_Invalid);
	}
}
//=================================================================================================================
void CMessageHandler::Send_TryGetWeaponRune( INT weaponConSlot, INT weaponItemSlot, INT itemConsumeID )
{
	C2G_RUNE_ITEM msg;
	msg.wMsg = c2g_rune_item;
	msg.byWeaponConSlot = (UINT8)weaponConSlot;
	msg.byWeaponItemSlot = (UINT8)weaponItemSlot;
	msg.nRuneItemID = itemConsumeID;
	_Send( &msg, sizeof(msg) );
}
//-----------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_rune_item(const char* buf, UINT32 size)
{
	G2C_RUNE_ITEM *msg = (G2C_RUNE_ITEM*)buf;
	if (!buf || size < sizeof(G2C_RUNE_ITEM))
		return;

	MMOGameEvent_OneInt evt( GameEvent_WeaponSkillAdd, msg->byResult );
	MMOUI::PostGameEvent(&evt);
}
//=================================================================================================================
void CMessageHandler::Send_TryReplaceWeaponRune( INT weaponConSlot, INT weaponItemSlot, INT itemConsumeID )
{
	C2G_RUNE_ITEM_ADVANCE msg;
	msg.wMsg = c2g_rune_item_advance;
	msg.byWeaponConSlot = (UINT8)weaponConSlot;
	msg.byWeaponItemSlot = (UINT8)weaponItemSlot;
	msg.nRuneItemID = itemConsumeID;
	_Send( &msg, sizeof(msg) );
}
//-----------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_rune_item_advance(const char* buf, UINT32 size)
{
	G2C_RUNE_ITEM_ADVANCE *msg = (G2C_RUNE_ITEM_ADVANCE*)buf;
	if (!buf || size < sizeof(G2C_RUNE_ITEM_ADVANCE))
		return;

	MMOEquipManager::getSingleton().NotifyWeaponRplcCachedRuneChg( msg->wRune, msg->byResult );
}
//=================================================================================================================
void CMessageHandler::Send_ConfirmReplaceWeaponRune( INT weaponConSlot, INT weaponItemSlot )
{
	C2G_RUNE_ITEM_ADVANCE_CONFIRM msg;
	msg.wMsg = c2g_rune_item_advance_confirm;
	msg.byWeaponConSlot = (UINT8)weaponConSlot;
	msg.byWeaponItemSlot = (UINT8)weaponItemSlot;
	_Send( &msg, sizeof(msg) );
}
//-----------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_rune_item_advance_confirm(const char* buf, UINT32 size)
{
	G2C_RUNE_ITEM_ADVANCE_CONFIRM *msg = (G2C_RUNE_ITEM_ADVANCE_CONFIRM*)buf;
	if (!buf || size < sizeof(G2C_RUNE_ITEM_ADVANCE_CONFIRM))
		return;

	MMOGameEvent_OneInt evt( GameEvent_WeaponSkillAdd, msg->byResult );
	MMOUI::PostGameEvent(&evt);
}
//=================================================================================================================
void CMessageHandler::Parse_g2c_identify_item(const char* buf, UINT32 size)
{
	G2C_IDENTIFY_ITEM* msg = (G2C_IDENTIFY_ITEM*)buf;
	if (!buf || size < sizeof(G2C_IDENTIFY_ITEM))
		return;
	if(msg)
	{
		//服务器说没枚举，就当bool处理
		if( msg->byResult )
		{
			//suc
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.IdenItemSuccess")),MsgInfoType_Table,TEXT("MMOGame.Bag.IdenItemSuccess") );
			
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.IdenItemFailure")) ,MsgInfoType_Table,TEXT("MMOGame.Bag.IdenItemFailure"));
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_repair_item(const char* buf, UINT32 size)
{
	G2C_REPAIR_ITEM* msg = (G2C_REPAIR_ITEM*)buf;
	if (!buf || size < sizeof(G2C_REPAIR_ITEM))
		return;

	if(msg)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Shop.Repaired")) ,MsgInfoType_Table,TEXT("MMOGame.Shop.Repaired"));
	}
}

void CMessageHandler::Parse_g2c_sell_resource_item(const char* buf, UINT32 size)
{
	G2C_SELL_RESOURCE_ITEM* msg = (G2C_SELL_RESOURCE_ITEM*)buf;
	if (!buf || size < sizeof(G2C_SELL_RESOURCE_ITEM))
		return;
	if (msg->nSoldCount < 0 ||
		(UINT32)msg->nSoldCount > (size - sizeof(*msg)) / sizeof(msg->itemslots[0]))
		return;
	if (msg)
	{
		AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
		if (player && player->ItemManager)
		{
			for(int i = 0; i< msg->nSoldCount; i++)
			{
				MMOFacade::getSingleton().DeleteItem(E_CONSLOT_RESOURCE,msg->itemslots[i],FALSE);
				player->ItemManager->DeleteItemBySlot(E_CONSLOT_RESOURCE,msg->itemslots[i]);
			}

			MMOGameEvent_Any evt(GameEvent_BagResSellAll);
			MMOUI::PostGameEvent(&evt);
		}
	}
	MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);
}
