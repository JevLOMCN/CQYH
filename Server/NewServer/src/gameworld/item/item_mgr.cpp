#include "stdafx.h"
#include "item_mgr.h"
#include "server_config.h"
#include "data_manager.h"
#include "item.h"
#include "game_world.h"
#include "attribute_pool_data.h"
#include "rune_data.h"
#include "spell_config.h"
#include "spell.h"

UINT64 CreateItemGuid()
{
    //ItemGuid is 8 bytes, laid out from left to right:
    //1-4:  Time
    //5:    Region ID
    //6:    GameServer ID
    //7-8:  Sequence number

    static UINT16 item_seed = 0;
    static UINT64 last_game_time = 0;
    static UINT32 area_id = g_ServerConfig.Common.nAreaID;
    static UINT32 gs_id = CGameWorld::GetInstance().GetGsSessionID();
    UINT64 cur_game_time = CGameWorld::GetInstance().GetGameTime();

    if (last_game_time < cur_game_time)
    {
        item_seed = 0;
        last_game_time = cur_game_time;
    }
    else
    {
        if (item_seed < 0xFFFF)
            item_seed++;
        else
            CRI("Create Item GUID Error!!! GameTime:%I64d, AreaID: %u, GsID: %u, ItemSeed: FFFF", (UINT32)cur_game_time, area_id, gs_id);
    }

    return (cur_game_time << 32) | (area_id << 22) | (gs_id << 16) | item_seed;
}

CItemMgr::CItemMgr()
{
}

CItemMgr& CItemMgr::GetInstance(void)
{
    static CItemMgr s_ItemMgr;

    return s_ItemMgr;
}

Item* CItemMgr::GenerateItem(INT32 nTemplateID, INT32 nItemCount)
{
    const ITEM_DATA* itemdata = NULL;

    LOG_PROCESS_ERROR(nTemplateID >= 99);

    itemdata = GetItemData(nTemplateID);
    LOG_PROCESS_ERROR(itemdata);

    return GenerateItem(itemdata, nItemCount);

Exit0:
	CRI("failed to generate item %d count %d", nTemplateID, nItemCount);

    return NULL;
}

Item* CItemMgr::GenerateItem(const ITEM_DATA* itemdata, INT32 nItemCount, BOOL bRandomDur/*=FALSE*/)
{
    Item* item = NULL;
    UINT64 guid = 0;
    LOG_PROCESS_ERROR(itemdata);
    LOG_PROCESS_ERROR(itemdata->m_nTemplateID >= 99);

    guid = CreateItemGuid();
    item = CGameWorld::GetInstance().NewItem(guid);
    LOG_PROCESS_ERROR(item);

    item->m_pTemplate = itemdata;
    item->m_bAttrEffective = FALSE;
    memset(&item->m_iteminfo, 0, sizeof(ItemInfo));

    item->m_iteminfo.m_llGuid = guid;
    item->m_iteminfo.m_nTemplateID = itemdata->m_nTemplateID;
    item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
    switch (itemdata->m_byDurType)
    {
    case DURTYPE_STACK:
        if (nItemCount > itemdata->m_nDur)
            item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
        else
            item->m_iteminfo.m_nCurDur = nItemCount;
        break;
    case DURTYPE_EQUIP:
        if (bRandomDur)
        {
            item->m_iteminfo.m_nMaxDur = itemdata->m_nDur * g_Random(EQUIP_MAX_DUR_RANDOM_MIN, EQUIP_MAX_DUR_RANDOM_MAX) / RATE_MAX;
            item->m_iteminfo.m_nCurDur = item->m_iteminfo.m_nMaxDur * g_Random(EQUIP_MAX_DUR_RANDOM_MIN, RATE_MAX) / RATE_MAX;
        }
        else
        {
            item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
            item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
        }
        break;
    case DURTYPE_USE:
        item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
        break;
    case DURTYPE_DEPOSIT:
        item->m_iteminfo.m_nCurDur = 0;
        break;
    case DURTYPE_RECOVER:
        item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
        item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
        break;
    case DURTYPE_PURITY:
        item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
        item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
        break;
    case DURTYPE_NONE:
        item->m_iteminfo.m_nCurDur = 1;
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
    }
    item->m_iteminfo.m_byBindType = itemdata->m_byBindType;

    // Generate random attributes / sockets
    if (itemdata->m_byType == ITEMTYPE_EQUIP)
    {
        const EQUIP_DATA* equipdata = (const EQUIP_DATA*)itemdata;

        item->m_iteminfo.m_byIdentified = equipdata->m_byIdentified;

        if (equipdata->m_byIdentified)
        {
            GenerateRandomAttributes(item);  // Random attributes
        }

        // Sockets are temporarily disabled
        //_GenerateSockets(item->m_iteminfo, equipdata->m_nSocketChance, equipdata->m_bySocketCountMin, equipdata->m_bySocketCountMax, equipdata->m_nColorSocketRate);
    }

    return item;

Exit0:
	if (itemdata)
		CRI("failed to generate item %d count %d", itemdata->m_nTemplateID, nItemCount);

    if (item)
        CGameWorld::GetInstance().DelItem(item);

    return NULL;
}

Item* CItemMgr::LoadItem(const ItemInfo& iteminfo)
{
    Item* item = NULL;
    const ITEM_DATA* itemdata = GetItemData(iteminfo.m_nTemplateID);
    LOG_PROCESS_ERROR(itemdata);

    item = CGameWorld::GetInstance().NewItem(iteminfo.m_llGuid);
    if (item == NULL)
    {
        Item* existing_item = CGameWorld::GetInstance().FindItem(iteminfo.m_llGuid);
        if (existing_item)
            CRI("Dupe Item Found: GUID:%I64u, [New: TemplateID:%d, Dur:%d] [Existing: TemplateID:%d, Dur:%d]",
            iteminfo.m_llGuid, iteminfo.m_nTemplateID, iteminfo.m_nCurDur,
            existing_item->m_iteminfo.m_nTemplateID, existing_item->m_iteminfo.m_nCurDur);
        else
            CRI("NewItem Failed");
    }
    LOG_PROCESS_ERROR(item);

    item->m_pTemplate = itemdata;
    item->m_iteminfo = iteminfo;
    item->m_bAttrEffective = FALSE;

    return item;

Exit0:
    return NULL;
}

Item* CItemMgr::CopyItem(const Item* item)
{
    Item* newitem = NULL;
    UINT64 guid = 0;

    LOG_PROCESS_ERROR(item);

    guid = CreateItemGuid();
    newitem = CGameWorld::GetInstance().NewItem(guid);
    LOG_PROCESS_ERROR(newitem);

    *newitem = *item;
    newitem->m_iteminfo.m_llGuid = guid;

    return newitem;

Exit0:
    return NULL;
}

BOOL CItemMgr::DestroyItem(Item* item)
{
    BOOL bRet = CGameWorld::GetInstance().DelItem(item);
    if (!bRet)
    {
        CRI("CItemMgr::DestroyItem Failed: Guid:%I64u, ItemID:%d", item->m_iteminfo.m_llGuid, item->m_iteminfo.m_nTemplateID);
    }
    return bRet;
}

// Generate random sockets
void CItemMgr::_GenerateSockets(ItemInfo& iteminfo, INT32 chance, UINT8 countmin, UINT8 countmax, INT32 colorrate)
{
    //Random socket count, 0-count
    //Random socket colors (excluding multicolor sockets; other colors are equally likely)
    //Equipment template data determines the chance of a multicolor socket; if selected, convert the last socket
    //Therefore, if the initial random count is zero, no multicolor socket can appear

    UINT32 temp_socket_type = SOCKET_NONE;
    INT32 actual_socket_count = 0;

    iteminfo.m_dwSocketCombination = 0;

    LOG_PROCESS_ERROR(chance >= 0 && chance <= RATE_MAX);

    //if (g_Random(RATE_MAX) >= chance)    // Default code (high probability)
    if (g_Random(RATE_MAX) < chance)    // Modified code (low probability)
        return;

    LOG_PROCESS_ERROR(countmin <= countmax);
    LOG_PROCESS_ERROR(countmax <= MAX_SOCKET_COUNT);
    PROCESS_ERROR(countmax > 0);

    actual_socket_count = g_Random(countmin, countmax + 1);

    if (actual_socket_count == 0)
        return;

    for (UINT8 i = 0; i < actual_socket_count; i++)
    {
        if (i == actual_socket_count - 1 && g_Random(RATE_MAX) < colorrate)
        {
            temp_socket_type = SOCKET_COLORFUL;
        }
        else
        {
            temp_socket_type = g_Random(SOCKET_COLORFUL - 1) + 1;
        }

        iteminfo.m_dwSocketCombination |= temp_socket_type << (i * 8);
    }

Exit0:
    return;
}

// Generate random attributes
void CItemMgr::GenerateRandomAttributes(Item* item, INT32 modifyrate/* = RATE_MAX*/, INT32 nServerGroup/* = 0*/)
{
    const EQUIP_DATA* equip_data = (const EQUIP_DATA*)item->m_pTemplate;
    INT32 attr_count = 0;

    // Calculate probability
    //INT32 final_rate = (INT32)((INT64)equip_data->m_nRandomAttributeChance * modifyrate / RATE_MAX);  Default code
    INT32 final_rate = equip_data->m_nRandomAttributeChance;  // Modified code: remove the calculation

    // Default code
    if (g_Random(RATE_MAX) >= final_rate)
        return;

    // Check the Luck +2 necklace drop limit
    BOOL bIsLuck2Equip = FALSE;
    BOOL bIsLuck2NecklaceLimit = FALSE;
    if (equip_data->m_bySubType == ITEMSUBTYPE_EQUIP_NECKLACE)
    {
        if (CheckEquipIsLuck2Item(equip_data->m_nTemplateID) == TRUE)
        {
            INT32 total = 0;
            for (const auto ItemID : g_ServerConfig.CC.nLuck2NecklaceItemID)
            {
                total += CGameWorld::GetInstance().GetServerVariable(nServerGroup, MakeServerKey(esvtItemDropLimitLuck2Necklace, ItemID));
            }

            if (total >= g_ServerConfig.CC.nItemDropLimitLuck2NecklaceCount)
            {
                bIsLuck2NecklaceLimit = TRUE;
            }
        }
    }

    // For non-Luck +2 necklaces, or Luck +2 necklaces below the limit, use the unchanged default flow
    if (bIsLuck2NecklaceLimit == FALSE)
    {
        for (INT32 i = 0; i < MAX_RANDOM_ATTR_COUNT; i++)
        {
            if (equip_data->m_nAttributePoolID[i] && g_Random(RATE_MAX) < equip_data->m_nAttributePoolChance[i])
            {
                INT32 random_number;
                const ATTRIBUTE_POOL* pool = CAttributePoolDataManager::GetInstance().GetPool(equip_data->m_nAttributePoolID[i]);
                LOG_PROCESS_ERROR(pool);

                random_number = g_Random(pool->m_nTotalWeight);

                for (INT32 j = 0; j < pool->m_nDataCount; j++)
                {
                    if (random_number < pool->m_PoolData[j].m_nWeight)
                    {
                        UINT16 tmpRandomAttrID = g_Random(pool->m_PoolData[j].m_nAttributeIDMin, pool->m_PoolData[j].m_nAttributeIDMax + 1);

                        item->m_iteminfo.m_wRandomAttrID[attr_count++] = tmpRandomAttrID;

                        // If this is a necklace with the Luck +2 attribute, increment the variable
                        if (equip_data->m_bySubType == ITEMSUBTYPE_EQUIP_NECKLACE &&
                            tmpRandomAttrID >= g_ServerConfig.CC.nMinLuck2AttributelID && 
                            tmpRandomAttrID <= g_ServerConfig.CC.nMaxLuck2AttributelID)
                        {
                            CGameWorld::GetInstance().SetServerVariable(nServerGroup, MakeServerKey(esvtItemDropLimitLuck2Necklace, equip_data->m_nTemplateID), FALSE, 1);
                        }

                        break;
                    }
                    else
                    {
                        random_number -= pool->m_PoolData[j].m_nWeight;
                    }
                }
            }
        }
    }
    // Only Luck +2 necklaces that have reached the limit enter this branch
    else
    {
        for (INT32 i = 0; i < MAX_RANDOM_ATTR_COUNT; i++)
        {
            if (equip_data->m_nAttributePoolID[i] && g_Random(RATE_MAX) < equip_data->m_nAttributePoolChance[i])
            {
                INT32 random_number;
                const ATTRIBUTE_POOL* pool = CAttributePoolDataManager::GetInstance().GetPool(equip_data->m_nAttributePoolID[i]);
                LOG_PROCESS_ERROR(pool);

                random_number = g_Random(pool->m_nTotalWeight);

                for (INT32 j = 0; j < pool->m_nDataCount; j++)
                {
                    if (random_number < pool->m_PoolData[j].m_nWeight)
                    {
                        // Choose a random attribute ID
                        UINT16 tmpRandomAttrID = g_Random(pool->m_PoolData[j].m_nAttributeIDMin, pool->m_PoolData[j].m_nAttributeIDMax + 1);

                        // Skip the Luck +2 attribute; otherwise assign the attribute
                        if (tmpRandomAttrID >= g_ServerConfig.CC.nMinLuck2AttributelID && tmpRandomAttrID <= g_ServerConfig.CC.nMaxLuck2AttributelID)
                        {
                            random_number -= pool->m_PoolData[j].m_nWeight;
                        }
                        else
                        {
                            item->m_iteminfo.m_wRandomAttrID[attr_count++] = tmpRandomAttrID;
                            break;
                        }
                    }
                    else
                    {
                        random_number -= pool->m_PoolData[j].m_nWeight;
                    }
                }
            }
        }
    }

    // Default code
    //for (INT32 i = 0; i < MAX_RANDOM_ATTR_COUNT; i++)
    //{
    //    if (equip_data->m_nAttributePoolID[i] && g_Random(RATE_MAX) < equip_data->m_nAttributePoolChance[i])
    //    {
    //        INT32 random_number;
    //        const ATTRIBUTE_POOL* pool = CAttributePoolDataManager::GetInstance().GetPool(equip_data->m_nAttributePoolID[i]);
    //        LOG_PROCESS_ERROR(pool);
    //
    //        random_number = g_Random(pool->m_nTotalWeight);
    //
    //        for (INT32 j = 0; j < pool->m_nDataCount; j++)
    //        {
    //            if (random_number < pool->m_PoolData[j].m_nWeight)
    //            {
    //                item->m_iteminfo.m_wRandomAttrID[attr_count++] = g_Random(pool->m_PoolData[j].m_nAttributeIDMin, pool->m_PoolData[j].m_nAttributeIDMax + 1);
    //                break;
    //            }
    //            else
    //            {
    //                random_number -= pool->m_PoolData[j].m_nWeight;
    //            }
    //        }
    //    }
    //}

Exit0:
    return;
}

void CItemMgr::GenerateRandomSockets(Item* item, INT32 modifyrate/* = RATE_MAX*/)
{
    const EQUIP_DATA* equip_data = (const EQUIP_DATA*)item->m_pTemplate;
    INT32 final_chance = (INT32)((INT64)equip_data->m_nSocketChance * modifyrate / RATE_MAX);
    if (final_chance > RATE_MAX)
        final_chance = RATE_MAX;

    _GenerateSockets(item->m_iteminfo, final_chance, equip_data->m_bySocketCountMin, equip_data->m_bySocketCountMax, equip_data->m_nColorSocketRate);
}

BOOL CItemMgr::RegisterRuneData(const RUNE_DATA* data)
{
    LOG_PROCESS_ERROR(data);

    if (data->nRuneID)
    {
        // Zero normal-drop weights are valid in the shipped rune table.
        LOG_PROCESS_ERROR(data->dwWeight[0][0] <= 1000000);    // For now, always use index [0][0]
        LOG_PROCESS_ERROR(CSpellConfig::GetInstance().FindSpellTemplate(data->nSpellID, 0, 0));
        LOG_PROCESS_ERROR(data->nClass >= clsPlayerBegin && data->nClass < clsPlayerEnd);

        m_vec_rune_data[data->nClass].push_back(data);
    }

    return TRUE;

Exit0:
    return FALSE;
}

// Normal inscription refinement
INT32 CItemMgr::GenerateRandomRuneData(INT32 nServerGroup, Item* item, INT32 cls, INT32 level, INT32 runegroup)
{
    UINT16* p_this_random_rune_id = NULL;  // Inscription currently being refined
    UINT16* p_another_rune_id = NULL;      // Other inscription to preserve
    const RUNE_DATA* another_rune_data = NULL;
    const EQUIP_DATA* equip_data = NULL;

    LOG_PROCESS_ERROR(item);
    LOG_PROCESS_ERROR(item->m_pTemplate->m_byType == ITEMTYPE_EQUIP);

    equip_data = (const EQUIP_DATA*)item->m_pTemplate;

    // Default code
    {
        // Randomly choose the slot to refine; check whether slot 2 has an unlock value and spending exceeds m_dwSecondRuneCost (both use a 1200-attempt threshold)
        //if (item->m_iteminfo.m_wRune[1] || (item->m_iteminfo.m_dwCostMoney >= equip_data->m_dwSecondRuneCostMax))
        //{
        //    if (g_Random(2) == 0)
        //    {
        //        p_this_random_rune_id = &item->m_iteminfo.m_wRune[0];  // Pointer
        //        p_another_rune_id = &item->m_iteminfo.m_wRune[1];
        //        another_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(*p_another_rune_id);
        //    }
        //    else
        //    {
        //        p_this_random_rune_id = &item->m_iteminfo.m_wRune[1];
        //        p_another_rune_id = &item->m_iteminfo.m_wRune[0];
        //        another_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(*p_another_rune_id);
        //    }
        //}
        //else
        //{
        //    p_this_random_rune_id = &item->m_iteminfo.m_wRune[0];
        //}
    }

    // Custom code
    {
        // If both slots exist and the second-slot condition is met, refine slot 1
        if (item->m_iteminfo.m_wRune[0] > 0 &&                                    // Slot 1 exists
            item->m_iteminfo.m_wRune[1] > 0 &&                                    // Slot 2 exists
            equip_data->m_dwSecondRuneCostMax > 0 &&                              // Slot 2 has an unlock condition
            item->m_iteminfo.m_dwCostMoney >= equip_data->m_dwSecondRuneCostMax)  // Second-slot unlock condition is met
        {
            p_this_random_rune_id = &item->m_iteminfo.m_wRune[0];  // Pointer
            p_another_rune_id = &item->m_iteminfo.m_wRune[1];
            another_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(*p_another_rune_id);
        }
        // If slot 1 exists and the second-slot condition is met, refine slot 2
        else if (item->m_iteminfo.m_wRune[0] > 0 &&                               // Slot 1 exists
            equip_data->m_dwSecondRuneCostMax > 0 &&                              // Slot 2 has an unlock condition
            item->m_iteminfo.m_dwCostMoney >= equip_data->m_dwSecondRuneCostMax)  // Second-slot unlock condition is met
        {
            p_this_random_rune_id = &item->m_iteminfo.m_wRune[1];  // Pointer
            p_another_rune_id = &item->m_iteminfo.m_wRune[0];
            another_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(*p_another_rune_id);
        }
        // If there is no inscription, refine slot 1
        else
        {
            p_this_random_rune_id = &item->m_iteminfo.m_wRune[0];
        }
    }


    // Repeat 10 times
    for (INT32 loop_times = 0; loop_times < 10; loop_times++)
    {
        // Generate an inscription for the class
        UINT16 temp_rune_id = _GenerateRuneID(cls, level, runegroup);
        LOG_PROCESS_ERROR(temp_rune_id);

        // Get the inscription template
        const RUNE_DATA* temp_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(temp_rune_id);
        LOG_PROCESS_ERROR(temp_rune_data);

        // Check inscription refinement restrictions
        INT32 rune_count = CGameWorld::GetInstance().GetServerVariable(nServerGroup, MakeServerKey(esvtRune, temp_rune_id));
        if (temp_rune_data->nDropLimitCount && rune_count >= temp_rune_data->nDropLimitCount)
        {
            INF("Rune Limit Exceeded: ServerGroup:%d, RuneID:%d, CurRuneCount:%d", nServerGroup, temp_rune_id, rune_count);
            continue;
        }

        if (another_rune_data == NULL ||
            (temp_rune_data->nSpellID != another_rune_data->nSpellID))
        {
            *p_this_random_rune_id = temp_rune_id;  // Assign the new inscription to the slot
            return temp_rune_id;
        }
    }

    LOG_PROCESS_ERROR(FALSE);

Exit0:
    return 0;
}

INT32 CItemMgr::GenerateRandomRuneDataAdvanced(INT32 nServerGroup, Item* item, INT32 cls, INT32 level, INT32 runegroup)
{
    UINT16* p_another_rune_id = NULL;      // Other inscription to preserve
    const RUNE_DATA* another_rune_data = NULL;
    const EQUIP_DATA* equip_data = NULL;

    LOG_PROCESS_ERROR(item);
    LOG_PROCESS_ERROR(item->m_pTemplate->m_byType == ITEMTYPE_EQUIP);

    equip_data = (const EQUIP_DATA*)item->m_pTemplate;

    // Check whether the equipment has unlocked slot 2
    if (item->m_iteminfo.m_wRune[1] > 0 || 
        item->m_iteminfo.m_dwCostMoney >= equip_data->m_dwSecondRuneCostMax)
    {
        // Refine slot 2 and retrieve slot 1 inscription information
        p_another_rune_id = &item->m_iteminfo.m_wRune[0];
        another_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(*p_another_rune_id);
    }
    else
    {
        return 0;  // The client should not send this packet for a weapon without two slots; a forced packet implies a direct call
    }

    // Repeat 10 times
    for (INT32 loop_times = 0; loop_times < 10; loop_times++)
    {
        // Generate an inscription for the class
        UINT16 temp_rune_id = _GenerateRuneID(cls, level, runegroup);
        LOG_PROCESS_ERROR(temp_rune_id);

        // Get the inscription template
        const RUNE_DATA* temp_rune_data = CDataManager<RUNE_DATA>::GetInstance().GetData(temp_rune_id);
        LOG_PROCESS_ERROR(temp_rune_data);

        // Check inscription refinement restrictions
        INT32 rune_count = CGameWorld::GetInstance().GetServerVariable(nServerGroup, MakeServerKey(esvtRune, temp_rune_id));
        if (temp_rune_data->nDropLimitCount && rune_count >= temp_rune_data->nDropLimitCount)
        {
            INF("Rune Limit Exceeded: ServerGroup:%d, RuneID:%d, CurRuneCount:%d", nServerGroup, temp_rune_id, rune_count);
            continue;
        }

        if (another_rune_data == NULL ||
            (temp_rune_data->nSpellID != another_rune_data->nSpellID))
        {
            return temp_rune_id;  // // Advanced refinement: return the result without changing inscription data yet
        }
    }

    LOG_PROCESS_ERROR(FALSE);

Exit0:
    return 0;
}

// Parameters: class, VIP level, inscription group
UINT16 CItemMgr::_GenerateRuneID(INT32 cls, INT32 level, INT32 runegroup)
{
    UINT32 random_number;
    UINT32 weight = 0;
    std::vector<const RUNE_DATA*> temp_vec_rune_data;

    LOG_PROCESS_ERROR(cls >= 0 && cls <clsPlayerTotal);

    // Calculate total weight for the class
    for (std::vector<const RUNE_DATA*>::iterator it = m_vec_rune_data[cls].begin(); it != m_vec_rune_data[cls].end(); ++it)
    {
        const RUNE_DATA* data = *it;
        if (level >= data->nLevel && runegroup >= data->nGroup)
        {
            temp_vec_rune_data.push_back(data);
            //weight += data->dwWeight;
            weight += data->dwWeight[0][0];    // For now, always use index [0][0]
        }
    }
    LOG_PROCESS_ERROR(!temp_vec_rune_data.empty());

    // Choose a random weight
    random_number = g_Random(weight);

    // Weighted selection
    for (INT32 i = 0; i < temp_vec_rune_data.size(); i++)
    {
        //if (random_number < temp_vec_rune_data[i]->dwWeight)
        if (random_number < temp_vec_rune_data[i]->dwWeight[0][0])    // For now, always use index [0][0]
            return (UINT16)temp_vec_rune_data[i]->nRuneID;

        //random_number -= temp_vec_rune_data[i]->dwWeight;
        random_number -= temp_vec_rune_data[i]->dwWeight[0][0];    // For now, always use index [0][0]
    }

    LOG_PROCESS_ERROR(FALSE);

Exit0:
    return 0;
}


// ========== Custom code begins

// Check whether the item is a Luck +2 necklace
BOOL CItemMgr::CheckEquipIsLuck2Item(INT32 itemID)
{
    for (const auto id : g_ServerConfig.CC.nLuck2NecklaceItemID)
    {
        if (id == itemID)
        {
            return TRUE;
        }
    }
    return FALSE;
}

// Custom function specifically for Luck +2
Item* CItemMgr::GenerateItem(INT32 nServerGroup, const ITEM_DATA* itemdata, INT32 nItemCount, BOOL bRandomDur)
{
    Item* item = NULL;
    UINT64 guid = 0;
    LOG_PROCESS_ERROR(itemdata);
    LOG_PROCESS_ERROR(itemdata->m_nTemplateID >= 99);

    guid = CreateItemGuid();
    item = CGameWorld::GetInstance().NewItem(guid);
    LOG_PROCESS_ERROR(item);

    item->m_pTemplate = itemdata;
    item->m_bAttrEffective = FALSE;
    memset(&item->m_iteminfo, 0, sizeof(ItemInfo));

    item->m_iteminfo.m_llGuid = guid;
    item->m_iteminfo.m_nTemplateID = itemdata->m_nTemplateID;
    item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
    switch (itemdata->m_byDurType)
    {
        case DURTYPE_STACK:
            if (nItemCount > itemdata->m_nDur)
                item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
            else
                item->m_iteminfo.m_nCurDur = nItemCount;
            break;
        case DURTYPE_EQUIP:
            if (bRandomDur)
            {
                item->m_iteminfo.m_nMaxDur = itemdata->m_nDur * g_Random(EQUIP_MAX_DUR_RANDOM_MIN, EQUIP_MAX_DUR_RANDOM_MAX) / RATE_MAX;
                item->m_iteminfo.m_nCurDur = item->m_iteminfo.m_nMaxDur * g_Random(EQUIP_MAX_DUR_RANDOM_MIN, RATE_MAX) / RATE_MAX;
            }
            else
            {
                item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
                item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
            }
            break;
        case DURTYPE_USE:
            item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
            break;
        case DURTYPE_DEPOSIT:
            item->m_iteminfo.m_nCurDur = 0;
            break;
        case DURTYPE_RECOVER:
            item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
            item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
            break;
        case DURTYPE_PURITY:
            item->m_iteminfo.m_nCurDur = itemdata->m_nDur;
            item->m_iteminfo.m_nMaxDur = itemdata->m_nDur;
            break;
        case DURTYPE_NONE:
            item->m_iteminfo.m_nCurDur = 1;
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
    }
    item->m_iteminfo.m_byBindType = itemdata->m_byBindType;

    // Generate random attributes / sockets
    if (itemdata->m_byType == ITEMTYPE_EQUIP)
    {
        const EQUIP_DATA* equipdata = (const EQUIP_DATA*)itemdata;

        item->m_iteminfo.m_byIdentified = equipdata->m_byIdentified;

        if (equipdata->m_byIdentified)
        {
            GenerateRandomAttributes(item, RATE_MAX, nServerGroup);
        }
    }

    return item;

Exit0:
    if (itemdata)
        CRI("failed to generate item %d count %d", itemdata->m_nTemplateID, nItemCount);

    if (item)
        CGameWorld::GetInstance().DelItem(item);

    return NULL;
}

// ========== Custom code ends
