#include "stdafx.h"
#include "ms_guild.h"
#include "guild_db_def_v_1.h"

#include "ms_guild_mgr.h"
#include "manageserver.h"
#include "server_config.h"

using namespace GUILD_DEF_VER_1;

BOOL CMsGuild::LoadGuild_v1(const char *pcData)
{
    INT32       nRetCode = 0;
    GUILD_DB_DATA   *pData = NULL;
    UINT32      dwRealSize = 0;

    LOG_PROCESS_ERROR(pcData);
    pData = (GUILD_DB_DATA *)pcData;

    LOG_PROCESS_ERROR(pData->dwVersion == 1);

    dwRealSize = pData->dwLen - sizeof(GUILD_DB_DATA);

    for (INT32 i = 0; i < gbtTotal; ++i)
    {
        GUILD_DB_DATA_BLOCK *pBlock = &pData->BlockTable[i];

        const char *pcBlockData = &pData->GuildData[pBlock->dwStartOffset];
        UINT32      dwBlockSize = pBlock->dwBlockSize;
        LOG_PROCESS_ERROR(dwBlockSize + pBlock->dwStartOffset <= dwRealSize);

        if (dwBlockSize == 0)
            continue;

        switch (pBlock->byBlockType)
        {
        case gbtBase:
        {
            nRetCode = LoadBase_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtWelfare:
        {
            nRetCode = LoadWelfare_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtBuilding:
        {
            nRetCode = LoadBuilding_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtDiplomat:
        {
            nRetCode = LoadDiplomat_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtDungeon:
        {
            nRetCode = LoadDungeon_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtStorage:
        {
            nRetCode = m_storage.LoadStorage_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtHistory:
        {
            nRetCode = LoadHistory_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case gbtStorageHistory:
        {
            nRetCode = m_storage.LoadStorageHistory_v1(pcBlockData, dwBlockSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
            break;
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LoadBase_v1(const char *pcData, UINT32 dwSize)
{
    const GUILD_DB_BASE   *pBase = (const GUILD_DB_BASE*)pcData;

    LOG_PROCESS_ERROR(pBase);
    LOG_PROCESS_ERROR(GUILD_POSITION_COUNT >= sizeof(pBase->wStorageRight) / sizeof(pBase->wStorageRight[0]));
    LOG_PROCESS_ERROR(MAX_GUILD_VARIABLE_COUNT >= sizeof(pBase->nVariables) / sizeof(pBase->nVariables[0]));

    m_dwRoutineConsumeTime = pBase->dwRoutineConsumeTime;
    m_nState = pBase->nState;
    m_nGuildMoney = pBase->nMoney;
    m_nWood = pBase->nWood;
    m_nHerb = pBase->nHerb;
    m_nCrystal = pBase->nCrystal;
    m_nSilver = pBase->nSilver;
    m_dwCalculateResourceTime = pBase->dwCalculateResourceTime;
    for (INT32 i = 0; i < GUILD_POSITION_COUNT; ++i)
        m_wStorageRight[i] = pBase->wStorageRight[i];
    for (INT32 i = 0; i < MAX_GUILD_VARIABLE_COUNT; ++i)
        m_arrVariables[i] = pBase->nVariables[i];
    m_dwSabacEnrollTime[0] = pBase->dwSabacEnrollTime;

	m_bOccupiedSabac = (BOOL)pBase->byOccupiedSabac;
	if (g_ServerConfig.GroupConfig[m_nSvrGroupID].bClearGlobalData)
		m_bOccupiedSabac = FALSE;
	if (m_bOccupiedSabac)
		CManageServer::GetInstance().SetSabacGuildID(m_nSvrGroupID, m_dwID);

    LOG_PROCESS_ERROR(sizeof(GUILD_DB_BASE) == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LoadWelfare_v1(const char *pcData, UINT32 dwSize)
{
    INT32               nRetCode = 0;
    const GUILD_DB_WELFARE    *pWelfare = (const GUILD_DB_WELFARE*)pcData;
    const GUILD_DB_WELFARE_UPGRADE    *pUpgrade = NULL;
    LOG_PROCESS_ERROR(pWelfare);
    LOG_PROCESS_ERROR((sizeof(GUILD_DB_WELFARE) + pWelfare->byUpgradeWelfareNum * sizeof(GUILD_DB_WELFARE_UPGRADE)) == dwSize);

    LOG_PROCESS_ERROR(GUILD_MAX_WELFARE_AURA_COUNT >= sizeof(pWelfare->WelfareAuraLevel) / sizeof(pWelfare->WelfareAuraLevel[0]));

    for (INT32 i = 0; i < GUILD_MAX_WELFARE_AURA_COUNT; ++i)
        m_arrWelfareAuraLvl[i] = pWelfare->WelfareAuraLevel[i];

    pUpgrade = (const GUILD_DB_WELFARE_UPGRADE *)(pcData + sizeof(pWelfare->WelfareAuraLevel) + sizeof(pWelfare->byUpgradeWelfareNum));
    LOG_PROCESS_ERROR(pUpgrade);
    LOG_PROCESS_ERROR(pWelfare->byUpgradeWelfareNum <= GUILD_MAX_UPGRADE_AURA_COUNT);
    for (INT32 i = 0; i < pWelfare->byUpgradeWelfareNum; ++i)
    {
        // todo_tq_26
        m_paWelfareUpgradeData[i] = CMsGuildMgr::GetInstance().CreateUpgradeTriggerData();
        LOG_PROCESS_ERROR(m_paWelfareUpgradeData[i]);

        m_paWelfareUpgradeData[i]->dwGuildID = m_dwID;
        m_paWelfareUpgradeData[i]->nType = pUpgrade[i].byType;
        m_paWelfareUpgradeData[i]->dwWashoutTime = pUpgrade[i].dwTime;
		m_paWelfareUpgradeData[i]->nTriggerType = tttInvalid;
		m_paWelfareUpgradeData[i]->fp = CB_UpgradeWelfareTimeout;

        nRetCode = CTimeTrigger::GetInstance().InsertTrigger(m_paWelfareUpgradeData[i]);
        LOG_PROCESS_ERROR(nRetCode);

        ++m_nUpgradeWelfareAuraCount;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LoadBuilding_v1(const char *pcData, UINT32 dwSize)
{
    INT32       nRetCode = 0;
    const GUILD_DB_BUILDING           *pBuilding = (const GUILD_DB_BUILDING*)pcData;
    const GUILD_DB_BUILDING_DATA      *pBuildingData = NULL;
    const GUILD_DB_BUILDING_UPGRADE   *pUpgrade = NULL;

    LOG_PROCESS_ERROR(pBuilding);
    LOG_PROCESS_ERROR((pBuilding->wBuildingBufferLength + pBuilding->wUpgradeBufferLength + sizeof(GUILD_DB_BUILDING)) == dwSize);

    pBuildingData = (const GUILD_DB_BUILDING_DATA *)pBuilding->Building;
    LOG_PROCESS_ERROR(pBuildingData);
    for (INT32 i = 0; i < pBuilding->byBuildingNum; ++i)
    {
        if (pBuildingData[i].byType < GUILD_B_FUNATIONAL_NUM)
        {
            MS_G_B_DATA *pData = GetFunBuildingDataByType(pBuildingData[i].byType);
            LOG_PROCESS_ERROR(pData);

            pData->nType = pBuildingData[i].byType;
            pData->nLevel = pBuildingData[i].byLevel;
            pData->qwNameID = pBuildingData[i].qwNpcNameID;
            pData->dwReviveTime = pBuildingData[i].dwReviveTime;
        }
        else
        {
            MS_G_B_DATA data;
            data.qwNameID = pBuildingData[i].qwNpcNameID;
            data.nType = pBuildingData[i].byType;
            data.nLevel = pBuildingData[i].byLevel;
            data.dwReviveTime = pBuildingData[i].dwReviveTime;

            m_BuildingList.insert(std::make_pair(pBuildingData[i].qwNpcNameID, data));
        }
    }
    LOG_PROCESS_ERROR(pBuilding->wBuildingBufferLength == pBuilding->byBuildingNum * sizeof(GUILD_DB_BUILDING_DATA));

    pUpgrade = (const GUILD_DB_BUILDING_UPGRADE*)(pBuilding->Building + pBuilding->wBuildingBufferLength);
    LOG_PROCESS_ERROR(pUpgrade);
    m_nUpgradeBuildingNum = pBuilding->byUpgradeBuildingNum;
    LOG_PROCESS_ERROR(pBuilding->byUpgradeBuildingNum <= GUILD_MAX_UPGRADE_BUILDING_COUNT);
    for (INT32 i = 0; i < pBuilding->byUpgradeBuildingNum; ++i)
    {
        GUILD_UPGRADE_TRIGGER   *pTrigger = CMsGuildMgr::GetInstance().CreateUpgradeTriggerData();
        LOG_PROCESS_ERROR(pTrigger);

        pTrigger->dwGuildID = m_dwID;
        pTrigger->qwNameID = pUpgrade[i].qwNpcNameID;
        pTrigger->nType = pUpgrade[i].byType;
        pTrigger->dwWashoutTime = pUpgrade[i].dwTime;
		pTrigger->nTriggerType = tttInvalid;
        pTrigger->fp = CB_UpgradeBuildingTimeout;

        nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
        LOG_PROCESS_ERROR(nRetCode);

        m_pUpgradeBuilding[i] = pTrigger;
    }
    LOG_PROCESS_ERROR(pBuilding->wUpgradeBufferLength == pBuilding->byUpgradeBuildingNum * sizeof(GUILD_DB_BUILDING_UPGRADE));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LoadDiplomat_v1(const char *pcData, UINT32 dwSize)
{
    const GUILD_DB_DIPLOMAT_DATA    *pData = NULL;
    const GUILD_DB_DIPLOMAT *pDiplomat = (const GUILD_DB_DIPLOMAT*)pcData;
    LOG_PROCESS_ERROR(pDiplomat);

    LOG_PROCESS_ERROR(GUILD_MAX_ALLIANCE_NUM >= pDiplomat->byAllianceNum);
    LOG_PROCESS_ERROR(GUILD_MAX_ENEMY_NUM >= pDiplomat->byEnemyNum);

    //m_dwActiveEnemy = pDiplomat->dwActiveEnemy;       Version 5 removed the m_dwActiveEnemy field
    m_dwRevengeGuildID = pDiplomat->dwRevengeGuildID;
    m_dwAimID = pDiplomat->dwAimID;
    m_dwAttackCD = pDiplomat->dwAttackCD;
    m_dwIntruderID = pDiplomat->dwIntruderID;
    m_dwProtectionTime = pDiplomat->dwProtectionTime;
    m_nMatchWarCount = pDiplomat->byMatchWarCount;
    m_dwMatchWarTime = pDiplomat->dwMatchWarTime;

    m_nAllianceNum = 0;
    m_nEnemyNum = 0;

    pData = (const GUILD_DB_DIPLOMAT_DATA *)pDiplomat->Buffer;
    for (INT32 i = 0; i < pDiplomat->byAllianceNum; ++i)
    {
        m_arrAlliance[i].dwGuildID = pData[i].dwGuildID;
        m_arrAlliance[i].dwEndTime = pData[i].dwEndTime;
        ++m_nAllianceNum;
    }

    pData = (const GUILD_DB_DIPLOMAT_DATA *)(pDiplomat->Buffer + pDiplomat->byAllianceNum * sizeof(GUILD_DB_DIPLOMAT_DATA));
    for (INT32 i = 0; i < pDiplomat->byEnemyNum; ++i)
    {
        m_arrEnemy[i].dwGuildID = pData[i].dwGuildID;
        m_arrEnemy[i].dwEndTime = pData[i].dwEndTime;
        ++m_nEnemyNum;
    }

    LOG_PROCESS_ERROR(dwSize == (sizeof(GUILD_DB_DIPLOMAT) + (pDiplomat->byAllianceNum + pDiplomat->byEnemyNum) * sizeof(GUILD_DB_DIPLOMAT_DATA)));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LoadDungeon_v1(const char *pcData, UINT32 dwSize)
{
    const GUILD_DB_DUNGEON_DATA *pData = NULL;
    const GUILD_DB_DUNGEON      *pDungeon = (const GUILD_DB_DUNGEON*)pcData;
    LOG_PROCESS_ERROR(pDungeon);
    LOG_PROCESS_ERROR(pDungeon->byDungeonNum <= GUILD_MAX_DUNGEON_COUNT);

    pData = (const GUILD_DB_DUNGEON_DATA *)pDungeon->Data;
    for (INT32 i = 0; i < pDungeon->byDungeonNum; ++i)
    {
        GUILD_DUNGEON   data;
        data.dwTemplateID = pData[i].dwTemplateID;
        data.dwLeaderID = pData[i].dwLeaderID;
        data.dwOwnerID = pData[i].dwOwnerID;
        data.nCreateTime = pData[i].dwCreateTime;
        m_listDungeon.push_back(data);
    }

    LOG_PROCESS_ERROR(dwSize == (sizeof(GUILD_DB_DUNGEON) + pDungeon->byDungeonNum * sizeof(GUILD_DB_DUNGEON_DATA)));
    FilterDungeon();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildStorage::LoadStorage_v1(const char *pcData, UINT32 dwSize)
{
    INT32   nRetCode = 0;
    INT32   nLength = 0;
    INT32   nReadLength = 0;
    const GUILD_DB_STORAGE *pStorage = (const GUILD_DB_STORAGE*)pcData;
    const GUILD_DB_STORAGE_ITEM *pItem = NULL;

    LOG_PROCESS_ERROR(pStorage);
    LOG_PROCESS_ERROR((pStorage->wItemLength + sizeof(GUILD_DB_STORAGE)) == dwSize);

    memset(m_aaItemBuffer, 0, sizeof(m_aaItemBuffer));
    nLength = pStorage->wItemLength;
    while (nLength > nReadLength)
    {
        pItem = (const GUILD_DB_STORAGE_ITEM *)(pStorage->Data + nReadLength);
        LOG_PROCESS_ERROR(nLength >= (nReadLength + pItem->byItemSize + sizeof(GUILD_DB_STORAGE_ITEM)));

        LOG_PROCESS_ERROR(pItem->byConSlot < GUILD_STORAGE_MAX_TAG_NUM);
        LOG_PROCESS_ERROR(pItem->byItemSlot < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG);
        LOG_PROCESS_ERROR(pItem->byItemSize < 
            sizeof(m_aaItemBuffer[pItem->byConSlot][pItem->byItemSlot].szItemBuffer));

        m_aaItemBuffer[pItem->byConSlot][pItem->byItemSlot].byConSlot = pItem->byConSlot;
        m_aaItemBuffer[pItem->byConSlot][pItem->byItemSlot].byItemSlot = pItem->byItemSlot;
        m_aaItemBuffer[pItem->byConSlot][pItem->byItemSlot].byItemSize = pItem->byItemSize;
        memcpy(m_aaItemBuffer[pItem->byConSlot][pItem->byItemSlot].szItemBuffer, 
            pItem->ItemData, pItem->byItemSize);

        nReadLength += (sizeof(GUILD_DB_STORAGE_ITEM) + pItem->byItemSize);
    }

    LOG_PROCESS_ERROR(nLength == nReadLength);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LoadHistory_v1(const char *pcData, UINT32 dwSize)
{
    INT32       nRetCode = 0;
    EVENT_DATA  *pEventData = NULL;
    const GUILD_DB_HISTORY_EVENT    *pEvent = NULL;
    const GUILD_DB_HISTORY    *pHistory = (const GUILD_DB_HISTORY*)pcData;
    LOG_PROCESS_ERROR(pHistory);

    pEvent = (const GUILD_DB_HISTORY_EVENT *)pHistory->Event;
    for (INT32 i = 0; i < pHistory->wNum; ++i)
    {
        pEventData = CMsGuildMgr::GetInstance().CreateEventData();
        LOG_PROCESS_ERROR(pEventData);

        ClearNode(&pEventData->node);
        pEventData->nType = pEvent[i].nType;
        pEventData->dwOccurTime = pEvent[i].dwOccurTime;
        pEventData->param1 = pEvent[i].param1;
        pEventData->param2 = pEvent[i].param2;
        pEventData->param3 = pEvent[i].param3;
        pEventData->param4 = pEvent[i].param4;

        if (pEvent[i].nType > GUILD_HE_TYPE_BEGIN && pEvent[i].nType < GUILD_HE_TYPE_END)
        {
            nRetCode = m_History.PushRear(&pEventData->node);
            LOG_PROCESS_ERROR(nRetCode);
            ++m_nHistoryNum;
        }
        else if (pEvent[i].nType > GUILD_CHEST_BEGIN && pEvent[i].nType < GUILD_CHEST_END)
        {
            nRetCode = m_ChestHistory.PushRear(&pEventData->node);
            LOG_PROCESS_ERROR(nRetCode);
            ++m_nChestHistoryNum;
        }
    }

    LOG_PROCESS_ERROR(dwSize == (sizeof(GUILD_DB_HISTORY) + sizeof(GUILD_DB_HISTORY_EVENT) * pHistory->wNum));

    return TRUE;
Exit0:
    if (pEventData)
        CMsGuildMgr::GetInstance().DeleteEventData(pEventData);
    return FALSE;
}

BOOL CMsGuildStorage::LoadStorageHistory_v1(const char *pcData, UINT32 dwSize)
{
    INT32       nRetCode = 0;
    INT32       nLength = 0;
    INT32       nReadLength = 0;
    STORAGE_EVENT_DATA  *pEventData = NULL;
    const GUILD_DB_STORAGE_EVENT    *pEvent = NULL;
    const GUILD_DB_STORAGE_HISTORY  *pHistory = (const GUILD_DB_STORAGE_HISTORY*)pcData;

    LOG_PROCESS_ERROR(pHistory);
    LOG_PROCESS_ERROR((pHistory->dwLength + sizeof(GUILD_DB_STORAGE_HISTORY)) == dwSize);
    
    nLength = pHistory->dwLength;
    while (nLength > nReadLength)
    {
        pEvent = (const GUILD_DB_STORAGE_EVENT *)(pHistory->Data + nReadLength);
        LOG_PROCESS_ERROR(nLength >= (nReadLength + pEvent->byItemSize + sizeof(GUILD_DB_STORAGE_EVENT)));
        LOG_PROCESS_ERROR(pEvent->byItemSize < sizeof(pEventData->szItemBuffer));

        pEventData = CMsGuildMgr::GetInstance().CreateStorageEventData();
        LOG_PROCESS_ERROR(pEventData);

        pEventData->nType = pEvent->byType;
        pEventData->dwRoleID = pEvent->dwRoleID;
        pEventData->dwOccurTime = pEvent->dwOccurTime;
        pEventData->nBufferSize = pEvent->byItemSize;
        memcpy(pEventData->szItemBuffer, pEvent->ItemBuffer, pEvent->byItemSize);
        ClearNode(&pEventData->node);

        m_History.PushRear(&pEventData->node);
        ++m_nHistoryNum;

        nReadLength += (sizeof(GUILD_DB_STORAGE_EVENT) + pEvent->byItemSize);
    }
    LOG_PROCESS_ERROR(nLength == nReadLength);

    return TRUE;
Exit0:
    return FALSE;
}
