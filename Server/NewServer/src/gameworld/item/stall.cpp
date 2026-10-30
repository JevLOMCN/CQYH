#include "stdafx.h"
#include "item_bag.h"
#include "stall.h"
#include "role.h"
#include "item_mgr.h"
#include "message_handler.h"
#include "game_world.h"

CStall::CStall()
{
    m_pOwner = NULL;
    Clear();
}

void CStall::Clear()
{
    m_state = essNotOpen;
    m_szStallName[0] = 0;
    m_goods.clear();

    m_dwUpgradeExpireTime = 0; //for debug, because no way to learn this skill now
}

void CStall::SetOwner(CRole* role)
{
    m_pOwner = role;
}

INT32 CStall::Open()
{
    static const INT64 stall_distance_square = CONVERT_LENGTH(g_GWConfig.nStallDistance) * CONVERT_LENGTH(g_GWConfig.nStallDistance);
    struct stTraverseRole
    {
        CRole* owner;
        bool operator()(CRole* pRole)
        {
            if (pRole->GetStall()->GetStallState() != essNotOpen)
            {
                if (owner->GetDistance3D(pRole) < stall_distance_square)
                    return false;
            }

            return true;
        }
    } traverse_role;
    struct stTraverseRegion
    {
        stTraverseRole* traverse;
        bool operator()(CRegion* pRegion)
        {
            return pRegion->TraverseRole(*traverse);
        }
    } traverse_region;

#ifndef _ROBOT
    CRegion* region = m_pOwner->GetRegion();

    LOG_PROCESS_ERROR(region);

    if (m_state != essNotOpen)
        return errStallWrongState;

    if (m_pOwner->GetMoveState() != CMS_STAND)
        return errStallWrongMoveState;

    if (m_pOwner->GetTrade()->IsTrading())
        return errStallTrading;

    if (!m_pOwner->GetGrid()->uStallLevel > 0)
        return errStallForbid;

    if (m_pOwner->IsFreeMode())
        return errFreeMode;

    if (m_pOwner->GetLevel() < g_GWConfig.nRoleLowLevel)
        return errRoleLowLevel;

    traverse_role.owner = m_pOwner;
    traverse_region.traverse = &traverse_role;
    if (!region->Traverse9Region(traverse_region))
        return errStallOthersOpen;
#endif

    m_state = essPreparing;
    m_pOwner->OpenStall();

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoBroadcastStallStateChange(m_pOwner, m_state);
#endif

    return errSuccess;

Exit0:
    return errUnknownError;
}

INT32 CStall::Close()
{
    if (m_state != essOpen)
        return errStallWrongState;

    m_state = essPreparing;

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoBroadcastStallStateChange(m_pOwner, m_state);
#endif

    return errSuccess;
}

INT32 CStall::RealOpen()
{
    if (m_state != essPreparing)
        return errStallWrongState;

    m_state = essOpen;

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoBroadcastStallStateChange(m_pOwner, m_state);
#endif

    return errSuccess;
}

INT32 CStall::RealClose()
{
    if (m_state != essPreparing && m_state != essOpen)
        return errStallWrongState;

    m_pOwner->CloseStall();

    m_state = essNotOpen;
    m_szStallName[0] = 0;
    m_goods.clear();

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoBroadcastStallStateChange(m_pOwner, m_state);
#endif

    return errSuccess;
}

INT32 CStall::AddGood(UINT8 slot, UINT8 conslot, UINT8 itemslot, INT16 itemcount, INT32 price)
{
    INT32 nResult = errItemUnknown;
    Item* item = NULL;
    INT64 this_total_price = price;

    LOG_PROCESS_ERROR(itemcount > 0);
    LOG_PROCESS_ERROR(price >= 100);

    if (m_state != essPreparing)
        return errStallWrongState;

    if (m_goods.find(slot) != m_goods.end())
        return errStallSlotExist;

    if (slot >= STALL_SLOT_COUNT)
        return errStallInvalidSlot;

    for (Goods::iterator it = m_goods.begin(); it != m_goods.end(); ++it)
    {
        Good& good = it->second;
        if (good.item->m_iteminfo.m_byConSlot == conslot &&
            good.item->m_iteminfo.m_byItemSlot == itemslot)
        {
            return errStallAlreadyInStall;
        }
    }

    item = m_pOwner->GetItemBag()->GetItem(conslot, itemslot);
    LOG_PROCESS_ERROR_RET_CODE(item, errItemNotFound);
    LOG_PROCESS_ERROR_RET_CODE(item->m_iteminfo.m_byBindType != BINDTYPE_BINDED, errItemBinded);
    LOG_PROCESS_ERROR_RET_CODE(item->m_iteminfo.m_dwRoleID == 0, errItemAlreadySigned);

    if (item->m_pTemplate->m_byDurType == DURTYPE_STACK)
    {
        LOG_PROCESS_ERROR_RET_CODE(itemcount <= item->m_iteminfo.m_nCurDur, errStallItemCountError);
    }
    else
    {
        LOG_PROCESS_ERROR_RET_CODE(itemcount == 1, errStallItemCountError);
    }

    this_total_price *= itemcount;
    LOG_PROCESS_ERROR_RET_CODE(this_total_price <= 2000000000, errStallMoneyExceeded);
    LOG_PROCESS_ERROR_RET_CODE(m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_02) + _GetTotalPrice() + this_total_price <= MAX_MONEY, errStallMoneyExceeded);

    {
        Good& good = m_goods[slot];
        good.item = item;
        good.count = itemcount;
        good.price = price;
        good.lastmodifyframe = CGameWorld::GetInstance().GetGameFrame();

#ifndef _ROBOT
        CCliMessageHandler::GetInstance().DoSendAddStallItem(m_pOwner, slot, conslot, itemslot, itemcount, price);
#endif
    }

    return errSuccess;

Exit0:
    return nResult;
}

INT32 CStall::RemoveGood(UINT8 slot)
{
    if (m_state != essPreparing)
        return errStallWrongState;

    Goods::iterator it = m_goods.find(slot);
    if (it == m_goods.end())
        return errStallSlotNotExist;

    m_goods.erase(it);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoSendRemvoeStallItem(m_pOwner, slot);
#endif

    return errSuccess;
}

INT32 CStall::ChangeName(const char* stallname)
{
    LOG_PROCESS_ERROR(stallname);
    LOG_PROCESS_ERROR(strlen(stallname) < STALL_NAME_LEN);

    if (m_state != essPreparing)
        return errStallWrongState;

    strxcpy(m_szStallName, stallname, STALL_NAME_LEN);

#ifndef _ROBOT
    CCliMessageHandler::GetInstance().DoBroadcastStallNameChange(m_pOwner, m_szStallName);
#endif

    return errSuccess;

Exit0:
    return errItemUnknown;
}

#ifndef _ROBOT

INT32 CStall::Upgrade(UINT8 type)
{
    LOG_PROCESS_ERROR(type < 3);

    if (m_state != essPreparing)
        return errStallWrongState;

    if (m_pOwner->GetItemBag()->GetMoney(MONEYTYPE_02) < STALL_UPGRADE_COST[type][1])
        return errItemNotEnoughMoneytype02;

    m_pOwner->GetItemBag()->SubMoney(MONEYTYPE_02, STALL_UPGRADE_COST[type][1], MONEYLOG_UPGRADE_STALL, 0);

    if (m_dwUpgradeExpireTime < CGameWorld::GetInstance().GetGameTime())
    {
        m_dwUpgradeExpireTime = CGameWorld::GetInstance().GetGameTime() + STALL_UPGRADE_COST[type][0] * 24 * 60 * 60;

        CCliMessageHandler::GetInstance().DoBroadcastStallLevelChange(m_pOwner, 1);

    }
    else
    {
        m_dwUpgradeExpireTime += STALL_UPGRADE_COST[type][0] * 24 * 60 * 60;
    }

    CCliMessageHandler::GetInstance().DoSyncStall(m_pOwner);

    return errSuccess;

Exit0:
    return errItemUnknown;
}

INT32 CStall::ViewGood(CRole* viewer)
{
    if (!m_pOwner->CheckDistance(viewer))
        return errStallFaraway;

    if (m_state != essOpen)
        return errStallWrongState;

    CCliMessageHandler::GetInstance().DoSendStallItemList(viewer, m_pOwner);

    return errSuccess;
}

INT32 CStall::BuyGood(CRole* buyer, UINT8 slot, INT32 itemcount, INT64 modifyframe)
{
    INT32 nRetCode = 0;
    Goods::iterator it;

    UINT32 stalllevel = m_pOwner->GetGrid()->uStallLevel;
    if (stalllevel < 1 || stalllevel > 3)
    {
        LOG_CHECK_ERROR(stalllevel >= 1 && stalllevel <= 3);
        return errItemUnknown;
    }

    INT32 tax = g_GWConfig.nStallTax[stalllevel - 1];

    if (!m_pOwner->CheckDistance(buyer))
        return errStallFaraway;

    if (m_state != essOpen)
        return errStallWrongState;

    it = m_goods.find(slot);
    if (it == m_goods.end())
        return errStallSlotNotExist;

    Good& good = it->second;
    if (good.lastmodifyframe != modifyframe)
        return errStallItemExpired;

    if (good.count < itemcount)
        return errStallNotEnoughItems;

    INT32 total_cost = good.price * itemcount;
    if (buyer->GetItemBag()->GetMoney(MONEYTYPE_02) < total_cost)
        return errNotEnoughMoney;

    INT32 total_tax = (total_cost * tax + 9999) / 10000;
    INT32 cost_excluding_tax = total_cost - total_tax;

    nRetCode = m_pOwner->GetItemBag()->TransferItemTo(buyer->GetItemBag(), good.item, itemcount, ITEMLOG_PLAYER_STALL);
    if (nRetCode == errSuccess)
    {
        // Optimization
        if (cost_excluding_tax)
        {
            buyer->GetItemBag()->SubMoney(MONEYTYPE_02, cost_excluding_tax + total_tax, MONEYLOG_PLAYER_STALL, m_pOwner->GetObjID());
            m_pOwner->GetItemBag()->AddMoney(MONEYTYPE_02, cost_excluding_tax, MONEYLOG_PLAYER_STALL, buyer->GetObjID());
        }
        else
        {
            buyer->GetItemBag()->SubMoney(MONEYTYPE_02, total_tax, MONEYLOG_PLAYER_STALL_TAX, 0);
        }

        // Default code
        //if (cost_excluding_tax)
        //{
        //    buyer->GetItemBag()->SubMoney(MONEYTYPE_02, cost_excluding_tax, MONEYLOG_PLAYER_STALL, m_pOwner->GetObjID());
        //    m_pOwner->GetItemBag()->AddMoney(MONEYTYPE_02, cost_excluding_tax, MONEYLOG_PLAYER_STALL, buyer->GetObjID());
        //}
        //buyer->GetItemBag()->SubMoney(MONEYTYPE_02, total_tax, MONEYLOG_PLAYER_STALL_TAX, 0);

        INT32 leftcount = good.count - itemcount;
        if (leftcount > 0)
        {
            good.count = leftcount;
        }
        else
        {
            m_goods.erase(it);
        }

        CCliMessageHandler::GetInstance().DoSendBuyStallItem(buyer, m_pOwner->GetObjID(), slot, leftcount);
        CCliMessageHandler::GetInstance().DoSendSoldStallItem(m_pOwner, slot, itemcount, cost_excluding_tax);
    }

    return nRetCode;
}

#endif

INT64 CStall::_GetTotalPrice() const
{
    INT64 total_price = 0;
    for (Goods::const_iterator it = m_goods.begin(); it != m_goods.end(); ++it)
    {
        total_price += it->second.price * it->second.count;
    }
    return total_price;
}

#ifdef _ROBOT

INT8 CStall::GetFirstGoodSolt()
{
    INT8 nSlot = -1;
    Goods::iterator itr;

    for (itr = m_goods.begin(); itr != m_goods.end(); ++itr)
    {
        nSlot = itr->first;
        break;
    }

    return nSlot;
}

INT64 CStall::GetFirstGoodModifyFrame()
{
    INT64 llModifyFrame = 0;
    Goods::iterator itr;

    for (itr = m_goods.begin(); itr != m_goods.end(); ++itr)
    {
        llModifyFrame = itr->second.lastmodifyframe;
        break;
    }

    return llModifyFrame;
}

INT8 CStall::GetNextAddGoodSolt()
{
    INT8 nSlot = -1;

    PROCESS_ERROR(m_goods.size() < STALL_SLOT_COUNT);

    for (Goods::iterator it = m_goods.begin(); it != m_goods.end(); ++it)
    {
        if (it->first > nSlot)
        {
            nSlot = it->first;
        }
    }

    nSlot++;
    if (nSlot >= STALL_SLOT_COUNT)
    {
        nSlot = -1;
        LOG_PROCESS_ERROR(FALSE);
    }

Exit0:
    return nSlot;
}

BOOL CStall::IsItemAlreadyInStall(UINT8 byConSlot, UINT8 byItemSlot)
{
    INT32 nResult = FALSE;
    Goods::iterator itr;
    Item* pItem = NULL;

    for (itr = m_goods.begin(); itr != m_goods.end(); ++itr)
    {
        pItem = itr->second.item;
        LOG_PROCESS_ERROR(pItem);
        if (pItem->m_iteminfo.m_byConSlot == byConSlot && pItem->m_iteminfo.m_byItemSlot == byItemSlot)
        {
            nResult = TRUE;
            break;
        }
    }

Exit0:
    return nResult;
}

BOOL CStall::SoldStallItem(UINT8 slot, INT32 itemcount)
{
    INT32 nResult = 0;
    INT32 leftcount = 0;
    Goods::iterator it;
    Good* pGood = NULL;

    it = m_goods.find(slot);
    PROCESS_ERROR(it != m_goods.end());

    pGood = &(it->second);
    LOG_PROCESS_ERROR(pGood);

    LOG_CHECK_ERROR(pGood->count >= itemcount);
    if (pGood->count >= itemcount)
    {
        leftcount = pGood->count - itemcount;
    }

    if (leftcount > 0)
    {
        pGood->count = leftcount;
    }
    else
    {
        m_goods.erase(it);
    }

    return TRUE;
Exit0:
    return FALSE;
}

void CStall::SetStallLevelUpExpireTime(UINT32 dwUpgradeExpireTime)
{
    m_dwUpgradeExpireTime = dwUpgradeExpireTime;
}

BOOL CStall::SyncStallItem(UINT8 slot, INT64 modifyframe)
{
    INT32 nResult = 0;
    INT32 leftcount = 0;
    Goods::iterator it;
    Good* pGood = NULL;

    it = m_goods.find(slot);
    LOG_PROCESS_ERROR(it != m_goods.end());

    pGood = &(it->second);
    LOG_PROCESS_ERROR(pGood);

    pGood->lastmodifyframe = modifyframe;

    return TRUE;
Exit0:
    return FALSE;
    }

#endif
