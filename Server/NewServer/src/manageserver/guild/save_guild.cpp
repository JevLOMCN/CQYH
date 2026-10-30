#include "stdafx.h"
#include "guild_db_def.h"
#include "ms_guild.h"
#include "zlib.h"

BOOL CMsGuild::CheckDBDataLength()
{
    GUILD_DB_BASE       *pBase = NULL;
    GUILD_DB_WELFARE    *pWelfare = NULL;

    // base
    LOG_PROCESS_ERROR((sizeof(pBase->wStorageRight) / sizeof(pBase->wStorageRight[0]))
        == GUILD_POSITION_COUNT);
    LOG_PROCESS_ERROR((sizeof(m_wStorageRight) / sizeof(m_wStorageRight[0]))
        == GUILD_POSITION_COUNT);
    LOG_PROCESS_ERROR((sizeof(pBase->nVariables) / sizeof(pBase->nVariables[0]))
        == MAX_GUILD_VARIABLE_COUNT);
    LOG_PROCESS_ERROR((sizeof(m_arrVariables) / sizeof(m_arrVariables[0]))
        == MAX_GUILD_VARIABLE_COUNT);
	LOG_PROCESS_ERROR(sizeof(pBase->dwSabacEnrollTime) / sizeof(pBase->dwSabacEnrollTime[0])
		== MAX_SABAC_ENROLL_COUNT);
    LOG_PROCESS_ERROR(sizeof(pBase->byAchievement) / sizeof(pBase->byAchievement[0])
        == GUILD_ACHI_TYPE_NUM);


    // welfare
    LOG_PROCESS_ERROR(sizeof(pWelfare->WelfareAuraLevel) / sizeof(pWelfare->WelfareAuraLevel[0])
        == GUILD_MAX_WELFARE_AURA_COUNT);
    LOG_PROCESS_ERROR(sizeof(m_arrWelfareAuraLvl) / sizeof(m_arrWelfareAuraLvl[0])
        == GUILD_MAX_WELFARE_AURA_COUNT);

    return TRUE;
Exit0:
    return FALSE;
}


BOOL CMsGuild::SaveGuild(char *pData, UINT32 &dwSize)
{
    INT32       nRetCode = 0;
    UINT32      dwBlockSize = 0;
    UINT32      dwBlockOffset = 0;
    GUILD_DB_DATA   *pDBGuild = NULL;

    LOG_PROCESS_ERROR(pData);

    pDBGuild = (GUILD_DB_DATA *)pData;

    pDBGuild->dwGuildID = m_dwID;
    pDBGuild->dwVersion = GUILD_DB_CURRENT_VERSION;
    pDBGuild->dwLen = sizeof(GUILD_DB_DATA);

    for (INT32 i = 0; i < gbtTotal; ++i)
    {
        switch (i)
        {
        case gbtBase:
        {
            nRetCode = SaveBase(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtWelfare:
        {
            nRetCode = SaveWelfare(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtBuilding:
        {
            nRetCode = SaveBuilding(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtDiplomat:
        {
            nRetCode = SaveDiplomat(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtDungeon:
        {
            nRetCode = SaveDungeon(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtStorage:
        {
            nRetCode = m_storage.SaveStorage(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtHistory:
        {
            nRetCode = SaveHistory(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        case gbtStorageHistory:
        {
            nRetCode = m_storage.SaveStorageHistory(pDBGuild->GuildData + dwBlockOffset, dwBlockSize);
        }
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
            break;
        }

        LOG_PROCESS_ERROR(nRetCode);
        pDBGuild->BlockTable[i].byBlockType = i;
        pDBGuild->BlockTable[i].dwBlockSize = dwBlockSize;
        pDBGuild->BlockTable[i].dwStartOffset = dwBlockOffset;

        dwBlockOffset += dwBlockSize;
        pDBGuild->dwLen += dwBlockSize;
        dwBlockSize = 0;
    }

    pDBGuild->dwCRC = crc32(0, (const Bytef*)&(pDBGuild->dwVersion), 
        pDBGuild->dwLen - sizeof(pDBGuild->dwLen) - sizeof(pDBGuild->dwCRC));

    dwSize = pDBGuild->dwLen;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SaveBase(char *pData, UINT32 &dwSize)
{
    INT32   nRetCode = 0;
    GUILD_DB_BASE   *pBase = (GUILD_DB_BASE*)pData;
    LOG_PROCESS_ERROR(pBase);

    pBase->dwRoutineConsumeTime = m_dwRoutineConsumeTime;
    pBase->nState = m_nState;
    pBase->nMoney = m_nGuildMoney;
    pBase->nWood = m_nWood;
    pBase->nHerb = m_nHerb;
    pBase->nCrystal = m_nCrystal;
    pBase->nSilver = m_nSilver;
    pBase->dwCalculateResourceTime = m_dwCalculateResourceTime;
    for (INT32 i = 0; i < GUILD_POSITION_COUNT; ++i)
        pBase->wStorageRight[i] = m_wStorageRight[i];
    for (INT32 i = 0; i < MAX_GUILD_VARIABLE_COUNT; ++i)
        pBase->nVariables[i] = m_arrVariables[i];

	for (INT32 i = 0; i < MAX_SABAC_ENROLL_COUNT; i++)
	    pBase->dwSabacEnrollTime[i] = m_dwSabacEnrollTime[i];
    pBase->byOccupiedSabac = m_bOccupiedSabac;

    for (INT32 i = 0; i < GUILD_ACHI_TYPE_NUM; ++i)
        pBase->byAchievement[i] = m_Achievement[i];

    dwSize = sizeof(GUILD_DB_BASE);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SaveWelfare(char *pData, UINT32 &dwSize)
{
    INT32       nRetCode = 0;
    GUILD_DB_WELFARE_UPGRADE    *pUpgrade = NULL;
    GUILD_DB_WELFARE    *pWelfare = (GUILD_DB_WELFARE*)pData;
    LOG_PROCESS_ERROR(pWelfare);

    for (INT32 i = 0; i < GUILD_MAX_WELFARE_AURA_COUNT; ++i)
        pWelfare->WelfareAuraLevel[i] = m_arrWelfareAuraLvl[i];

    pWelfare->byUpgradeWelfareNum = 0;
    pUpgrade = (GUILD_DB_WELFARE_UPGRADE *)(pData + sizeof(GUILD_DB_WELFARE));
    LOG_PROCESS_ERROR(m_nUpgradeWelfareAuraCount < GUILD_MAX_UPGRADE_AURA_COUNT);
    for (INT32 i = 0; i < m_nUpgradeWelfareAuraCount; ++i)
    {
        LOG_PROCESS_ERROR(m_paWelfareUpgradeData[i]);
        pUpgrade[i].dwTime = m_paWelfareUpgradeData[i]->dwWashoutTime;
        pUpgrade[i].byType = m_paWelfareUpgradeData[i]->nType;
        ++pWelfare->byUpgradeWelfareNum;
    }

    dwSize = sizeof(GUILD_DB_WELFARE) + sizeof(GUILD_DB_WELFARE_UPGRADE) * pWelfare->byUpgradeWelfareNum;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SaveBuilding(char *pData, UINT32 &dwSize)
{
    INT32       nRetCode = 0;
    GUILD_DB_BUILDING_DATA      *pBuildingData = NULL;
    GUILD_DB_BUILDING_UPGRADE   *pUpgrade = NULL;
    GUILD_DB_BUILDING           *pBuilding = (GUILD_DB_BUILDING *)pData;
    G_BUILDING_MAP::iterator    end_iter = m_BuildingList.end();

    LOG_PROCESS_ERROR(pBuilding);

    pBuilding->byBuildingNum = 0;
    pBuilding->byUpgradeBuildingNum = 0;
    pBuilding->wBuildingBufferLength = 0;
    pBuilding->wUpgradeBufferLength = 0;

    // building
	pBuildingData = (GUILD_DB_BUILDING_DATA*)pBuilding->Building;
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        MS_G_B_DATA *pData = GetFunBuildingDataByType(i);
        if (pData && pData->nType != 0)
        {
            pBuildingData[pBuilding->byBuildingNum].qwNpcNameID = pData->qwNameID;
            pBuildingData[pBuilding->byBuildingNum].byType = pData->nType;
            pBuildingData[pBuilding->byBuildingNum].byLevel = pData->nLevel;
            pBuildingData[pBuilding->byBuildingNum].dwReviveTime = pData->dwReviveTime;
            ++pBuilding->byBuildingNum;
        }
    }
    for (G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != end_iter; ++iter)
    {
        pBuildingData[pBuilding->byBuildingNum].qwNpcNameID = iter->second.qwNameID;
        pBuildingData[pBuilding->byBuildingNum].dwReviveTime = iter->second.dwReviveTime;
        pBuildingData[pBuilding->byBuildingNum].byLevel = iter->second.nLevel;
        pBuildingData[pBuilding->byBuildingNum].byType = iter->second.nType;
        ++pBuilding->byBuildingNum;
    }
    pBuilding->wBuildingBufferLength = sizeof(GUILD_DB_BUILDING_DATA) * pBuilding->byBuildingNum;

    // upgrade
    pUpgrade = (GUILD_DB_BUILDING_UPGRADE *)(pBuilding->Building + pBuilding->wBuildingBufferLength);
    for (INT32 i = 0; i < m_nUpgradeBuildingNum; ++i)
    {
        LOG_PROCESS_ERROR(m_pUpgradeBuilding[i]);
        pUpgrade[pBuilding->byUpgradeBuildingNum].qwNpcNameID = m_pUpgradeBuilding[i]->qwNameID;
        pUpgrade[pBuilding->byUpgradeBuildingNum].dwTime = m_pUpgradeBuilding[i]->dwWashoutTime;
        pUpgrade[pBuilding->byUpgradeBuildingNum].byType = m_pUpgradeBuilding[i]->nType;
        ++pBuilding->byUpgradeBuildingNum;
    }
    pBuilding->wUpgradeBufferLength = sizeof(GUILD_DB_BUILDING_UPGRADE) * pBuilding->byUpgradeBuildingNum;

    dwSize = pBuilding->wBuildingBufferLength + pBuilding->wUpgradeBufferLength + sizeof(GUILD_DB_BUILDING);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SaveDiplomat(char *pData, UINT32 &dwSize)
{
    INT32   nRetCode = 0;
    GUILD_DB_DIPLOMAT_DATA  *pDipData = NULL;
    GUILD_DB_DIPLOMAT   *pDiplomat = (GUILD_DB_DIPLOMAT*)pData;
    LOG_PROCESS_ERROR(pDiplomat);

    pDiplomat->dwRevengeGuildID = m_dwRevengeGuildID;
    pDiplomat->dwAimID = m_dwAimID;
    pDiplomat->dwAttackCD = m_dwAttackCD;
    pDiplomat->dwIntruderID = m_dwIntruderID;
    pDiplomat->dwProtectionTime = m_dwProtectionTime;
    pDiplomat->byMatchWarCount = m_nMatchWarCount;
    pDiplomat->dwMatchWarTime = m_dwMatchWarTime;
    pDiplomat->byAllianceNum = 0;
    pDiplomat->byEnemyNum = 0;

    pDipData = (GUILD_DB_DIPLOMAT_DATA *)pDiplomat->Buffer;
    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        pDipData[pDiplomat->byAllianceNum].dwGuildID = m_arrAlliance[i].dwGuildID;
        pDipData[pDiplomat->byAllianceNum].dwEndTime = m_arrAlliance[i].dwEndTime;
        ++pDiplomat->byAllianceNum;
    }

    pDipData = (GUILD_DB_DIPLOMAT_DATA *)(pDiplomat->Buffer + pDiplomat->byAllianceNum * sizeof(GUILD_DB_DIPLOMAT_DATA));
    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        pDipData[pDiplomat->byEnemyNum].dwGuildID = m_arrEnemy[i].dwGuildID;
        pDipData[pDiplomat->byEnemyNum].dwEndTime = m_arrEnemy[i].dwEndTime;
        ++pDiplomat->byEnemyNum;
    }

    dwSize = sizeof(GUILD_DB_DIPLOMAT) + 
        sizeof(GUILD_DB_DIPLOMAT_DATA) * (pDiplomat->byAllianceNum + pDiplomat->byEnemyNum);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SaveDungeon(char *pData, UINT32 &dwSize)
{
    INT32       nRetCode = 0;
    GUILD_DUNGEON_LIST::iterator    end_iter = m_listDungeon.end();
    GUILD_DB_DUNGEON_DATA   *pDungeonData = NULL;
    GUILD_DB_DUNGEON    *pDungeon = (GUILD_DB_DUNGEON*)pData;
    LOG_PROCESS_ERROR(pDungeon);

    pDungeon->byDungeonNum = 0;
    pDungeonData = (GUILD_DB_DUNGEON_DATA *)pDungeon->Data;
    for (GUILD_DUNGEON_LIST::iterator iter = m_listDungeon.begin(); iter != end_iter; ++iter)
    {
        pDungeonData[pDungeon->byDungeonNum].dwTemplateID = iter->dwTemplateID;
        pDungeonData[pDungeon->byDungeonNum].dwCreateTime = iter->nCreateTime;
        pDungeonData[pDungeon->byDungeonNum].dwLeaderID = iter->dwLeaderID;
        pDungeonData[pDungeon->byDungeonNum].dwOwnerID = iter->dwOwnerID;
        ++pDungeon->byDungeonNum;
    }

    dwSize = sizeof(GUILD_DB_DUNGEON) + pDungeon->byDungeonNum * sizeof(GUILD_DB_DUNGEON_DATA);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SaveHistory(char *pData, UINT32 &dwSize)
{
    INT32       nRetCode = 0;
    BASE_LIST_NODE  *pNode = NULL;
    EVENT_DATA      *pEvent = NULL;
    GUILD_DB_HISTORY_EVENT  *pDBEvent = NULL;
    GUILD_DB_HISTORY        *pHistory = (GUILD_DB_HISTORY*)pData;
    LOG_PROCESS_ERROR(pHistory);

    pHistory->wNum = 0;
    pDBEvent = (GUILD_DB_HISTORY_EVENT *)pHistory->Event;

    pNode = m_History.Head().pNext;
    while (pNode != &m_History.Rear())
    {
        pEvent = (EVENT_DATA*)((char*)pNode - (char*)&(((EVENT_DATA*)0)->node));
        
        pDBEvent[pHistory->wNum].nType = pEvent->nType;
        pDBEvent[pHistory->wNum].dwOccurTime = pEvent->dwOccurTime;
        pDBEvent[pHistory->wNum].param1 = pEvent->param1;
        pDBEvent[pHistory->wNum].param2 = pEvent->param2;
        pDBEvent[pHistory->wNum].param3 = pEvent->param3;
        pDBEvent[pHistory->wNum].param4 = pEvent->param4;
        ++pHistory->wNum;

        pNode = pNode->pNext;
    }

    pNode = m_ChestHistory.Head().pNext;
    while (pNode != &m_ChestHistory.Rear())
    {
        pEvent = (EVENT_DATA*)((char*)pNode - (char*)&(((EVENT_DATA*)0)->node));

        pDBEvent[pHistory->wNum].nType = pEvent->nType;
        pDBEvent[pHistory->wNum].dwOccurTime = pEvent->dwOccurTime;
        pDBEvent[pHistory->wNum].param1 = pEvent->param1;
        pDBEvent[pHistory->wNum].param2 = pEvent->param2;
        pDBEvent[pHistory->wNum].param3 = pEvent->param3;
        pDBEvent[pHistory->wNum].param4 = pEvent->param4;
        ++pHistory->wNum;

        pNode = pNode->pNext;
    }

    dwSize = sizeof(GUILD_DB_HISTORY) + pHistory->wNum * sizeof(GUILD_DB_HISTORY_EVENT);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildStorage::SaveStorage(char *pData, UINT32 &dwSize)
{
    INT32   nRetCode = 0;
    INT32   nWriteLength = 0;
    GUILD_DB_STORAGE_ITEM   *pItem = NULL;
    GUILD_DB_STORAGE        *pStorage = (GUILD_DB_STORAGE*)pData;

    LOG_PROCESS_ERROR(pStorage);

    pStorage->wItemLength = 0;
    for (int i = 0; i < GUILD_STORAGE_MAX_TAG_NUM; ++i)
    {
        for (int j = 0; j < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG; ++j)
        {
            if (m_aaItemBuffer[i][j].byConSlot >= 0 && m_aaItemBuffer[i][j].byConSlot != 0xFF && m_aaItemBuffer[i][j].byItemSize > 0)
            {
                pItem = (GUILD_DB_STORAGE_ITEM *)(pStorage->Data + pStorage->wItemLength);

                LOG_PROCESS_ERROR(m_aaItemBuffer[i][j].byItemSize < sizeof(m_aaItemBuffer[i][j].szItemBuffer));

                pItem->byConSlot = m_aaItemBuffer[i][j].byConSlot;
                pItem->byItemSlot = m_aaItemBuffer[i][j].byItemSlot;
                pItem->byItemSize = m_aaItemBuffer[i][j].byItemSize;
                memcpy(pItem->ItemData, m_aaItemBuffer[i][j].szItemBuffer, m_aaItemBuffer[i][j].byItemSize);

                pStorage->wItemLength += (sizeof(GUILD_DB_STORAGE_ITEM) + pItem->byItemSize);
            }
        }
    }

    dwSize = sizeof(GUILD_DB_STORAGE) + pStorage->wItemLength;

    return TRUE;
Exit0:
    return FALSE; 
}

BOOL CMsGuildStorage::SaveStorageHistory(char *pData, UINT32 &dwSize)
{
    INT32       nRetCode = 0;
    BASE_LIST_NODE              *pNode = NULL;
    STORAGE_EVENT_DATA          *pSEvent = NULL;
    GUILD_DB_STORAGE_EVENT      *pEvent = NULL;
    GUILD_DB_STORAGE_HISTORY    *pHistory = (GUILD_DB_STORAGE_HISTORY*)pData;

    LOG_PROCESS_ERROR(pHistory);

    pHistory->dwLength = 0;
    pNode = m_History.Head().pNext;
    while (pNode != &m_History.Rear())
    {
        pSEvent = (STORAGE_EVENT_DATA *)((char*)pNode - (char*)&(((STORAGE_EVENT_DATA*)0)->node));
        pEvent = (GUILD_DB_STORAGE_EVENT *)(pHistory->Data + pHistory->dwLength);

        pEvent->byType = pSEvent->nType;
        pEvent->dwRoleID = pSEvent->dwRoleID;
        pEvent->dwOccurTime = pSEvent->dwOccurTime;
        pEvent->byItemSize = pSEvent->nBufferSize;
        memcpy(pEvent->ItemBuffer, pSEvent->szItemBuffer, pSEvent->nBufferSize);

        pHistory->dwLength += (sizeof(GUILD_DB_STORAGE_EVENT) + pEvent->byItemSize);

        pNode = pNode->pNext;
    }

    dwSize = sizeof(GUILD_DB_STORAGE_HISTORY) + pHistory->dwLength;

    return TRUE;
Exit0:
    return FALSE;
}
