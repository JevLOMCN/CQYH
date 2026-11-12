#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "UStallManager.h"
#include "MMOPowerManager.h"

#define USE_OLD_STALL 0

using namespace ExternalMessage;

void CMessageHandler::Send_OpenStall( )
{
	C2G_OPEN_STALL msg;
	msg.wMsg = c2g_open_stall;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CloseStall()
{
	C2G_CLOSE_STALL msg;
	msg.wMsg = c2g_close_stall;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RealOpenStall( )
{
	C2G_REAL_OPEN_STALL msg;
	msg.wMsg = c2g_real_open_stall;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RealCloseStall()
{
	C2G_REAL_CLOSE_STALL msg;
	msg.wMsg = c2g_real_close_stall;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_AddStallItem(UINT8 stallSlot, UINT8 conSlot, UINT8 itemSlot, INT16 itemCount, INT32 price)
{
	C2G_ADD_STALL_ITEM msg;
	msg.wMsg = c2g_add_stall_item;
	msg.btStallSlot = stallSlot;
	msg.byConSlot = conSlot;
	msg.byItemSlot = itemSlot;
	msg.shItemCount = itemCount;
	msg.nPrice = price;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RemoveStallItem(UINT8 stallSlot)
{
	C2G_REMOVE_STALL_ITEM msg;
	msg.wMsg = c2g_remove_stall_item;
	msg.btStallSlot = stallSlot;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ChangeStallName(const TCHAR* szStallName)
{
	C2G_CHANGE_STALL_NAME msg;
	msg.wMsg = c2g_change_stall_name;
	strxcpy(msg.szStallName, szStallName, STALL_NAME_LEN);
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ViewStallItem(UINT32 roleId)
{
	C2G_VIEW_STALL_ITEM msg;
	msg.wMsg = c2g_view_stall_item;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_BuyStallItem(UINT32 roleId, UINT8 stallSlot, INT16 itemCount, INT32 modifyFrame)
{
	C2G_BUY_STALL_ITEM msg;
	msg.wMsg = c2g_buy_stall_item;
	msg.dwRoleID = roleId;
	msg.btStallSlot = stallSlot;
	msg.shItemCount = itemCount;
	msg.llModfiyFrame = modifyFrame;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_UpgradeStall(UINT8 timeType)
{
	C2G_UPGRADE_STALL msg;
	msg.wMsg = c2g_upgrade_stall;
	msg.btUpgradeCostType = timeType;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_sync_stall(const char* buf, UINT32 size)
{
	G2C_SYNC_STALL* msg =  (G2C_SYNC_STALL*)buf;
	UStallManager::getSingleton().SetStallUpgradeLeftTime(msg->dwUpgradeExpireTime);
}

void CMessageHandler::Parse_g2c_stall_state_change(const char* buf, UINT32 size)
{
	G2C_STALL_STATE_CHANGE* msg =  (G2C_STALL_STATE_CHANGE*)buf;
	
	AMMOPlayer* player = Cast<AMMOPlayer>( MMOFacade::getSingleton().GetCharacter(msg->dwRoleID));
	UStallManager::getSingleton().ChangeActorStallState(player, msg->btStallState);
	UIHudProcess(OnStallStateChange(msg->btStallState));
	MMOUI::GetUIManager()->delegateUpdateHintText( player );
}

void CMessageHandler::Parse_g2c_add_stall_item(const char* buf, UINT32 size)
{
	G2C_ADD_STALL_ITEM* msg =  (G2C_ADD_STALL_ITEM*)buf;
	
	UStallManager::getSingleton().AddStallItem(msg->btStallSlot, 
		msg->byConSlot,
		msg->byItemSlot,
		msg->shItemCount,
		msg->nPrice);
    UMMOUIBag* bagPanel = (UMMOUIBag*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Bag);
    if (bagPanel)
    {
        bagPanel->eventRefreshBag();
    }
}

void CMessageHandler::Parse_g2c_remove_stall_item(const char* buf, UINT32 size)
{
	G2C_REMOVE_STALL_ITEM* msg =  (G2C_REMOVE_STALL_ITEM*)buf;
	
	UStallManager::getSingleton().RemoveStallItem(msg->btStallSlot);
	UMMOUIBag* bagPanel = (UMMOUIBag*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Bag);
	if (bagPanel)
	{
        bagPanel->eventRefreshBag();
    }
}

void CMessageHandler::Parse_g2c_change_stall_name(const char* buf, UINT32 size)
{
    G2C_CHANGE_STALL_NAME* msg =  (G2C_CHANGE_STALL_NAME*)buf;
	AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(msg->dwRoleID));
	if (player)
	{
		player->StallName = FString(UTF8_TO_TCHAR(msg->szStallName));
	}
}

void CMessageHandler::Parse_g2c_view_stall_item(const char* buf, UINT32 size)
{
	G2C_VIEW_STALL_ITEM* msg =  (G2C_VIEW_STALL_ITEM*)buf;

	UStallManager::getSingleton().SetStallBuyPlayerId(msg->dwRoleID);
	UStallManager::getSingleton().ResetStallBuyItems();
	
	INT bufSize = 0;
	for (int i = 0; i < msg->btStallItemCount; ++ i)
	{
		STALL_ITEM_INFO* tStallItemInfo = (STALL_ITEM_INFO*)(msg->acStallItems + bufSize);
		if (!tStallItemInfo)
		{
			continue;
		}

		ItemInfo iteminfo;
        INT buflen = iteminfo.Load(tStallItemInfo->acItem);

		bufSize += sizeof(STALL_ITEM_INFO) + buflen;

        //Cannot use other players' own ConId, so temporarily store data into CONSLOT_STALL_BUY here
        iteminfo.m_byConSlot = E_CONSLOT_STALL_BUY;
        iteminfo.m_byItemSlot = tStallItemInfo->btStallSlot;
        //Add to ItemInfo
        FClientItemInfo* itemInfo = MMOFacade::getSingleton().UpdateItem(&iteminfo);
		if (itemInfo)
		{
			itemInfo->m_nCurDur = iteminfo.m_nCurDur;
		}

		const UItem* item = MMOFacade::getSingleton().FindItemTemplate(iteminfo.m_nTemplateID);
		if (!item)
		{
			warnf(NAME_Error, TEXT("item not find [%d]"), iteminfo.m_nTemplateID);
			continue;
		}

		if (item->ItemType == TEXT("ITEMTYPE_EQUIP") && itemInfo)
		{
			MMOPowerManager::getSingleton().UpdateEquipPower(itemInfo);
		}

        UStallManager::getSingleton().AddStallBuyItem(tStallItemInfo->btStallSlot, 
			iteminfo.m_nTemplateID, 
			iteminfo.m_nCurDur, 
			tStallItemInfo->nPrice, 
			tStallItemInfo->llModifyFrame);
	}

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallBuy, TRUE, "");
}

void CMessageHandler::Parse_g2c_buy_stall_item(const char* buf, UINT32 size)
{
	G2C_BUY_STALL_ITEM* msg = (G2C_BUY_STALL_ITEM*)buf;
	UStallManager::getSingleton().BuyStallItem(msg->btStallSlot, msg->nLeftItemCount);
}

void CMessageHandler::Parse_g2c_sold_stall_item(const char* buf, UINT32 size)
{
	G2C_SOLD_STALL_ITEM* msg = (G2C_SOLD_STALL_ITEM*)buf;
	UStallManager::getSingleton().SellStallItem(msg->btStallSlot, msg->nItemCount);
}

void CMessageHandler::Parse_g2c_change_stall_level(const char* buf, UINT32 size)
{
	G2C_CHANGE_STALL_LEVEL* msg =  (G2C_CHANGE_STALL_LEVEL*)buf;
	UStallManager::getSingleton().ChangeStallLevel(msg->dwRoleID, msg->byStallLevel);
}

