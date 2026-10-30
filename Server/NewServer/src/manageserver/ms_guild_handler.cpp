#include "stdafx.h"

#include "zlib.h"
#include "message_handler.h"

#include "m2c_message.h"
#include "c2m_message.h"

#include "guild_config.h"
#include "game_world_config.h"

#include "onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "manageserver.h"
#include "guild/ms_guild_mgr.h"
#include "./guild/guild_storage.h"
#include "./guild/ms_guild_mgr.h"
#include "mail_template_data.h"
#include "chat/ms_chat.h"
#include "script_mgr.h"
#include "server_var.h"
#include "client/user.h"
#include "guild_activity.h"
#include "guild_building_data.h"

using namespace ExternalMessage;

BOOL CMessageHandler::DoNSCreateGuild(UINT32 dwRoleID, const char* pcszGuildName, const char* pcszAnnounce, BOOL bConsumeItem)
{
    INT32       nRetCode = 0;
    CreateGuildParam        cgp;

    LOG_PROCESS_ERROR(pcszGuildName);
    LOG_PROCESS_ERROR(pcszAnnounce);
    memset(&cgp, 0, sizeof(cgp));

    cgp.dwRoleID = dwRoleID;
    cgp.bConsumeItem = bConsumeItem;
    strxcpy(cgp.szAnnounce, pcszAnnounce, GUILD_ANNOUNCE_LEN);

    nRetCode = CMessageHandler::GetInstance().DoNSNewName(ntGuildName, dwRoleID, pcszGuildName, (const char*)&cgp, sizeof(cgp));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildQueryName(UINT32 dwClientSessionID, CMsGuild* poMSGuild, UINT32 dwCRC)
{
    INT32       nRetCode = 0;
    UINT32      dwNewCRC = 0;
    M2C_QUERY_GUILD_NAME_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(poMSGuild);

    pMsg = (M2C_QUERY_GUILD_NAME_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_QUERY_GUILD_NAME_ACK));
    LOG_PROCESS_ERROR(pMsg);

    memset(pMsg, 0, sizeof(M2C_QUERY_GUILD_NAME_ACK));

    pMsg->wMsg = m2c_query_guild_name_ack;
    pMsg->dwGuildID = poMSGuild->GetID();
    strxcpy(pMsg->szGuildName, poMSGuild->GetName(), sizeof(pMsg->szGuildName));
    pMsg->dwLeaderID = poMSGuild->GetLeaderID();
    pMsg->dwCreateTime = poMSGuild->GetCreateTime();
    pMsg->byMemberCount = poMSGuild->GetMemberNum();
    pMsg->byMainLevel = poMSGuild->GetLevel();
    pMsg->byHouseLevel = poMSGuild->GetBuildingMaxLevelByType(GUILD_B_HOUSE);
    pMsg->bySabacState = 0;

    if (poMSGuild->IsOccupiedSabac())
        pMsg->bySabacState = 2;
    else if (poMSGuild->IsEnrolledSabac())
        pMsg->bySabacState = 1;

    dwNewCRC = crc32(0, (const Bytef*)pMsg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
    if (dwNewCRC != dwCRC)
    {
        nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_QUERY_GUILD_NAME_ACK));
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        if (pMsg)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientCreateGuild(UINT32 dwClientSessionID, const char* pGuildName)
{
    INT32   nRetCode = 0;
    M2C_CREATE_GUILD_ACK* pMsg = (M2C_CREATE_GUILD_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_CREATE_GUILD_ACK));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pGuildName);

    pMsg->wMsg = m2c_create_guild_ack;
    strxcpy(pMsg->szGuildName, pGuildName, GUILD_NAME_LEN);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_CREATE_GUILD_ACK));
    LOG_PROCESS_ERROR(nRetCode);
    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientViewGuildListAck(UINT32 dwClientSessionID, UINT8 byGroup, UINT8 byViewType, UINT32 dwLastGuildID)
{
    INT32       nRetCode = 0;
    CMsGuild* pMsGuild = NULL;
    MS_ROLE_DATA* pRoleData = NULL;
    M2C_VIEW_GUILD_LIST_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_VIEW_GUILD_LIST_ACK) + MAX_VIEW_GUILD_LIST_COUNT * sizeof(GUILD_VIEW_SUMMARY) < MAX_MESSAGE_SIZE);

    pMsg = (M2C_VIEW_GUILD_LIST_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_view_guild_list_ack;
    pMsg->byCount = 0;

    if (dwLastGuildID)
    {
        pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwLastGuildID);
        LOG_PROCESS_ERROR(pMsGuild);

        if (GUILD_VIEW_UP == byViewType)
            pMsGuild = CMsGuildMgr::GetInstance().GetPrevGuildOnLvlRank(pMsGuild);
        else if (GUILD_VIEW_DOWN == byViewType)
            pMsGuild = CMsGuildMgr::GetInstance().GetNextGuildOnLvlRank(pMsGuild);
    }
    else
    {
        pMsGuild = CMsGuildMgr::GetInstance().GetFirstGuildOnLvlRank();
    }

    for (; pMsGuild && pMsg->byCount < MAX_VIEW_GUILD_LIST_COUNT; )
    {
        if (byGroup == pMsGuild->GetSvrGroup())
        {
            pMsg->info[pMsg->byCount].dwGuildID = pMsGuild->GetID();
            pMsg->info[pMsg->byCount].byGuildLevel = pMsGuild->GetLevel();
            pMsg->info[pMsg->byCount].byCurrentMemberCount = (UINT8)pMsGuild->GetMemberNum();
            pMsg->info[pMsg->byCount].dwCreateTime = pMsGuild->GetCreateTime();
            strxcpy(pMsg->info[pMsg->byCount].szGuildName, pMsGuild->GetName(), GUILD_NAME_LEN);
            strxcpy(pMsg->info[pMsg->byCount].szCreaterName, pMsGuild->GetCreaterName(), ROLE_NAME_LEN);
            strxcpy(pMsg->info[pMsg->byCount].szAnnounce, pMsGuild->GetAnnounce(), GUILD_ANNOUNCE_LEN);

            if (g_GWConfig.bFilterCensoredWord)
            {
                nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->info[pMsg->byCount].szAnnounce);
                LOG_CHECK_ERROR(nRetCode);
            }

            pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsGuild->GetLeaderID());
            if (pRoleData)
            {
                strxcpy(pMsg->info[pMsg->byCount].szLeaderName, pRoleData->szRoleName, ROLE_NAME_LEN);
            }
            else
            {
                LOG_CHECK_ERROR("user AFK" && FALSE);
                memset(pMsg->info[pMsg->byCount].szLeaderName, 0, ROLE_NAME_LEN);
            }

            for (INT32 nType = GUILD_B_MAIN; nType < GUILD_B_NUM; ++nType)
            {
                pMsg->info[pMsg->byCount].arrBuildingLevel[nType] = pMsGuild->GetBuildingMaxLevelByType(nType);
            }
            if (pMsGuild->GetProtectionTime() > CManageServer::GetInstance().GetGameTime())
                pMsg->info[pMsg->byCount].dwProtectionTime = pMsGuild->GetProtectionTime() - CManageServer::GetInstance().GetGameTime();
            else
                pMsg->info[pMsg->byCount].dwProtectionTime = 0;
            ++pMsg->byCount;
        }
        if (GUILD_VIEW_UP == byViewType)
            pMsGuild = CMsGuildMgr::GetInstance().GetPrevGuildOnLvlRank(pMsGuild);
        else if (GUILD_VIEW_DOWN == byViewType)
            pMsGuild = CMsGuildMgr::GetInstance().GetNextGuildOnLvlRank(pMsGuild);
    }
    pMsg->byViewType = byViewType;

    pMsg->wSize = sizeof(M2C_VIEW_GUILD_LIST_ACK) + sizeof(GUILD_VIEW_SUMMARY) * pMsg->byCount;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMoreGuildInfoAck(UINT32 dwClientSessionID, CMsGuild* poMSGuild)
{
    INT32   nRetCode = 0;
    M2C_MORE_GUILD_INFO_ACK* pMsg = (M2C_MORE_GUILD_INFO_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_MORE_GUILD_INFO_ACK));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poMSGuild);

    pMsg->wMsg = m2c_more_guild_info_ack;
    pMsg->byMatchCount = poMSGuild->GetMatchWarCount();
    pMsg->nMatchWarConsumption = g_GuildConfig.nGuildMatchWarMoneyConsumption;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MORE_GUILD_INFO_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFindGuildAck(UINT32 dwClientSessionID, UINT8 byGroup, CMsGuild* pMsGuild, char* szLeaderName)
{
    INT32   nRetCode = 0;
    M2C_FIND_GUILD_ACK* pMsg = (M2C_FIND_GUILD_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_FIND_GUILD_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_find_guild_ack;
    pMsg->data.dwGuildID = pMsGuild->GetID();
    pMsg->data.byGuildLevel = pMsGuild->GetLevel();
    pMsg->data.byCurrentMemberCount = (UINT8)pMsGuild->GetMemberNum();
    pMsg->data.dwCreateTime = pMsGuild->GetCreateTime();
    strxcpy(pMsg->data.szGuildName, pMsGuild->GetName(), GUILD_NAME_LEN);
    strxcpy(pMsg->data.szCreaterName, pMsGuild->GetCreaterName(), ROLE_NAME_LEN);
    strxcpy(pMsg->data.szAnnounce, pMsGuild->GetAnnounce(), GUILD_ANNOUNCE_LEN);
    strxcpy(pMsg->data.szLeaderName, szLeaderName, ROLE_NAME_LEN);
    if (pMsGuild->GetProtectionTime() > CManageServer::GetInstance().GetGameTime())
        pMsg->data.dwProtectionTime = pMsGuild->GetProtectionTime() - CManageServer::GetInstance().GetGameTime();
    else
        pMsg->data.dwProtectionTime = 0;

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->data.szAnnounce);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FIND_GUILD_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;

}

BOOL CMessageHandler::DoClientGetGuildDataNtf(UINT32 dwClientSessionID, CMsGuild* poMsGuild, char* szLeaderName)
{
    INT32   nRetCode = 0;
    CMsGuildMember* poMsGuildMember = NULL;
    MS_ROLE_DATA* pRoleData = NULL;
    MS_ONLINE_ROLE_DATA* pstOnlineRoleData = NULL;
    UINT32      dwLastLoginTime = 0;
    CMsGuild* pTempGuild = NULL;
    EVENT_DATA* pEventData = NULL;
    GUILD_UPGRADE_TRIGGER* pTriggerData = NULL;
    GUILD_UPGRADE_TRIGGER* pUTrigger = NULL;
    INT32       nCurrentTime = CManageServer::GetInstance().GetGameTime();
    MS_G_B_DATA* pBData = NULL;
    INT32       nIndex = 0;
    M2C_GUILD_INFO_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_INFO_NTF) + sizeof(GUILD_MEMBER_INFO) * GUILD_MAX_MEMBER_COUNT < MAX_MESSAGE_SIZE);

    pMsg = (M2C_GUILD_INFO_NTF*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_info_ntf;

    pMsg->dwGuildID = poMsGuild->GetID();
    strxcpy(pMsg->szGuildName, poMsGuild->GetName(), GUILD_NAME_LEN);
    pMsg->byLevel = poMsGuild->GetLevel();
    pMsg->dwGuildMoney = poMsGuild->GetMoney();
    pMsg->dwCreateTime = poMsGuild->GetCreateTime();
    pMsg->dwLeaderID = poMsGuild->GetLeaderID();

    if (poMsGuild->GetWarAttackCD() > CManageServer::GetInstance().GetGameTime())
        pMsg->dwAttackCD = poMsGuild->GetWarAttackCD() - CManageServer::GetInstance().GetGameTime();
    else
        pMsg->dwAttackCD = 0;

    if (poMsGuild->GetProtectionTime() > CManageServer::GetInstance().GetGameTime())
        pMsg->dwProtectionTime = poMsGuild->GetProtectionTime() - CManageServer::GetInstance().GetGameTime();
    else
        pMsg->dwProtectionTime = 0;

    if (szLeaderName)
        strxcpy(pMsg->szLeaderName, szLeaderName, ROLE_NAME_LEN);
    else
        memset(pMsg->szLeaderName, 0, ROLE_NAME_LEN);

    strxcpy(pMsg->szCreaterName, poMsGuild->GetCreaterName(), ROLE_NAME_LEN);
    strxcpy(pMsg->szNotice, poMsGuild->GetNotice(), GUILD_NOTICE_LEN);

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szNotice);
        LOG_CHECK_ERROR(nRetCode);
    }

    // manage
    pMsg->wStorageRight[GUILD_POSITION_LEADER - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_LEADER);
    pMsg->wStorageRight[GUILD_POSITION_SUBLEADER - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_SUBLEADER);
    pMsg->wStorageRight[GUILD_POSITION_ELDER - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_ELDER);
    pMsg->wStorageRight[GUILD_POSITION_BISHOP - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_BISHOP);
    pMsg->wStorageRight[GUILD_POSITION_DEACON - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_DEACON);
    pMsg->wStorageRight[GUILD_POSITION_SUBDEACON - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_SUBDEACON);
    pMsg->wStorageRight[GUILD_POSITION_CIVILIAN - 1] = poMsGuild->GetStorageRightByPosition(GUILD_POSITION_CIVILIAN);

    // history event
    pMsg->stEventInfo.byEventCount = 0;
    pEventData = poMsGuild->GetFirstGuildEventData();
    while (pEventData)
    {
        pMsg->stEventInfo.stEventInfo[pMsg->stEventInfo.byEventCount].byType = pEventData->nType;
        pMsg->stEventInfo.stEventInfo[pMsg->stEventInfo.byEventCount].param1 = pEventData->param1;
        pMsg->stEventInfo.stEventInfo[pMsg->stEventInfo.byEventCount].param2 = pEventData->param2;
        pMsg->stEventInfo.stEventInfo[pMsg->stEventInfo.byEventCount].param3 = pEventData->param3;
        pMsg->stEventInfo.stEventInfo[pMsg->stEventInfo.byEventCount].param4 = pEventData->param4;
        pMsg->stEventInfo.stEventInfo[pMsg->stEventInfo.byEventCount++].dwTime = pEventData->dwOccurTime;
        if (pMsg->stEventInfo.byEventCount == GUILD_DATA_NTF_HISTORY_NUM)
            break;
        pEventData = poMsGuild->GetNextGuildEventData(pEventData);
    }

    pMsg->stChestInfo.byCount = 0;
    pEventData = poMsGuild->FirstChestEvent();
    while (pEventData)
    {
        if (pMsg->stChestInfo.byCount > (GUILD_MAX_CHEST_NTF_NUM - 3) &&
            (pEventData->nType == GUILD_CHEST_WAR_SACK_A_1 || pEventData->nType == GUILD_CHEST_WAR_SACK_D_1))
            break;

        pMsg->stChestInfo.stEvent[pMsg->stChestInfo.byCount].byType = pEventData->nType;
        pMsg->stChestInfo.stEvent[pMsg->stChestInfo.byCount].param1 = pEventData->param1;
        pMsg->stChestInfo.stEvent[pMsg->stChestInfo.byCount].param2 = pEventData->param2;
        pMsg->stChestInfo.stEvent[pMsg->stChestInfo.byCount].param3 = pEventData->param3;
        pMsg->stChestInfo.stEvent[pMsg->stChestInfo.byCount].param4 = pEventData->param4;
        pMsg->stChestInfo.stEvent[pMsg->stChestInfo.byCount++].dwTime = pEventData->dwOccurTime;
        if (pMsg->stChestInfo.byCount == GUILD_MAX_CHEST_NTF_NUM)
            break;
        pEventData = poMsGuild->NextChestEvent(pEventData);
    }

    // diplomat
    pMsg->byAllianceCount = 0;
    for (INT32 i = 0; i < poMsGuild->GetAllianceNum() && poMsGuild->GetAllianceDataByIndex(i); ++i)
    {
        pTempGuild = CMsGuildMgr::GetInstance().FindGuildByID(poMsGuild->GetAllianceDataByIndex(i)->dwGuildID);
        if (pTempGuild)
        {
            pMsg->stAlliance[pMsg->byAllianceCount].byType = GUILD_DIPLOMAT_ALLIANCE;
            pMsg->stAlliance[pMsg->byAllianceCount].byLevel = pTempGuild->GetLevel();
            pMsg->stAlliance[pMsg->byAllianceCount].dwGuildID = pTempGuild->GetID();
            pMsg->stAlliance[pMsg->byAllianceCount].byMemberCount = static_cast<UINT8>(pTempGuild->GetMemberNum());
            pMsg->stAlliance[pMsg->byAllianceCount].dwLastTime = poMsGuild->GetAllianceDataByIndex(i)->dwEndTime;
            strxcpy(pMsg->stAlliance[pMsg->byAllianceCount].szGuildName, pTempGuild->GetName(), GUILD_NAME_LEN);
            ++pMsg->byAllianceCount;
        }
    }

    pMsg->byEnemyCount = 0;
    for (INT32 i = 0; i < poMsGuild->GetEnemyNum() && poMsGuild->GetEnemyDataByIndex(i); ++i)
    {
        pTempGuild = CMsGuildMgr::GetInstance().FindGuildByID(poMsGuild->GetEnemyDataByIndex(i)->dwGuildID);
        if (pTempGuild)
        {
            pMsg->stEnemy[pMsg->byEnemyCount].byType = GUILD_DIPLOMAT_ENEMY;
            pMsg->stEnemy[pMsg->byEnemyCount].byLevel = pTempGuild->GetLevel();
            pMsg->stEnemy[pMsg->byEnemyCount].dwGuildID = pTempGuild->GetID();
            pMsg->stEnemy[pMsg->byEnemyCount].byMemberCount = static_cast<UINT8>(pTempGuild->GetMemberNum());
            pMsg->stEnemy[pMsg->byEnemyCount].dwLastTime = poMsGuild->GetEnemyDataByIndex(i)->dwEndTime;
            strxcpy(pMsg->stEnemy[pMsg->byEnemyCount].szGuildName, pTempGuild->GetName(), GUILD_NAME_LEN);
            ++pMsg->byEnemyCount;
        }
    }

    pMsg->byMemberCount = 0;
    poMsGuildMember = poMsGuild->GetFirstGuildMember();
    while (poMsGuildMember)
    {
        pstOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poMsGuildMember->GetRoleID());
        if (pstOnlineRoleData)
        {
            pRoleData = pstOnlineRoleData->pRoleData;
            dwLastLoginTime = 0;
        }
        else
        {
            pRoleData = CRoleDataMgr::GetInstance().FindRole(poMsGuildMember->GetRoleID());
            if (pRoleData)
            {
                dwLastLoginTime = pRoleData->nLastLoginTime;
            }
        }
        if (pRoleData)
        {
            pMsg->arrMemberData[pMsg->byMemberCount].dwRoleID = poMsGuildMember->GetRoleID();
            pMsg->arrMemberData[pMsg->byMemberCount].byPosition = poMsGuildMember->GetPosition();
            pMsg->arrMemberData[pMsg->byMemberCount].byClass = pRoleData->byClass;
            pMsg->arrMemberData[pMsg->byMemberCount].byLevel = pRoleData->byLevel;
            strxcpy(pMsg->arrMemberData[pMsg->byMemberCount].szRoleName, pRoleData->szRoleName, ROLE_NAME_LEN);
            pMsg->arrMemberData[pMsg->byMemberCount].nLastLoginTime = dwLastLoginTime;
            pMsg->arrMemberData[pMsg->byMemberCount].nSceneID = pRoleData->nSceneTempID;
            pMsg->arrMemberData[pMsg->byMemberCount].nNextSalary = poMsGuildMember->GetSalary();
            pMsg->arrMemberData[pMsg->byMemberCount].byFlag = poMsGuildMember->GetFlag();
            ++pMsg->byMemberCount;
        }
        poMsGuildMember = poMsGuild->GetNextGuildMember();
    }

    pMsg->nHerb = poMsGuild->GetHerb();
    pMsg->nWood = poMsGuild->GetWood();
    pMsg->nCrystal = poMsGuild->GetCryStal();
    pMsg->nSilver = poMsGuild->GetSilver();
    pMsg->nProsperity = poMsGuild->GetProsperity();

    for (INT32 i = 0; i < MAX_GUILD_VARIABLE_COUNT; ++i)
    {
        pMsg->arrVariables[i] = poMsGuild->GetVariable(i);
    }

    nIndex = 0;
    memset(pMsg->arrBData, 0, sizeof(pMsg->arrBData));
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        pBData = poMsGuild->GetFunBuildingDataByType(i);
        if (pBData && pBData->nType != 0)
        {
            pMsg->arrBData[nIndex].qwNameID = pBData->qwNameID;
            pMsg->arrBData[nIndex].nType = pBData->nType;
            pMsg->arrBData[nIndex].nLevel = pBData->nLevel;
            //pMsg->arrBData[nIndex].nLevel = pBData->nBuildingLevel;
            ++nIndex;
        }
    }
    pBData = poMsGuild->FirstUnFunBuilding();
    while (pBData)
    {
        pMsg->arrBData[nIndex].qwNameID = pBData->qwNameID;
        pMsg->arrBData[nIndex].nType = pBData->nType;
        pMsg->arrBData[nIndex].nLevel = pBData->nLevel;
        ++nIndex;

        pBData = poMsGuild->NextUnFunBuilding();
    }

    for (INT32 i = 1; i < GUILD_MAX_WELFARE_AURA_COUNT; ++i)
    {
        pMsg->arrWelfareAuraLvl[i] = poMsGuild->GetWelfareAuraLevelByType(i);
    }

    memset(pMsg->arrBUData, 0, sizeof(pMsg->arrBUData));
    memset(pMsg->arrWelfareAuraUpgradeTime, 0, sizeof(pMsg->arrWelfareAuraUpgradeTime));
    memset(pMsg->arrWelfareUpgradeAuraID, 0, sizeof(pMsg->arrWelfareUpgradeAuraID));

    memset(pMsg->arrBUData, 0, sizeof(pMsg->arrBUData));
    for (INT32 i = 0; i < poMsGuild->GetUpgradeBuildingNum(); ++i)
    {
        pUTrigger = poMsGuild->GetUpgradeBuilding(i);
        LOG_PROCESS_ERROR(pUTrigger);

        pMsg->arrBUData[i].qwNameID = pUTrigger->qwNameID;
        pMsg->arrBUData[i].nTime = pUTrigger->dwWashoutTime - nCurrentTime;
    }

    for (INT32 i = 0; i < poMsGuild->GetUpgradeWelfareNum(); ++i)
    {
        pTriggerData = poMsGuild->GetUpgradeWelfare(i);
        LOG_PROCESS_ERROR(pTriggerData);

        pMsg->arrWelfareUpgradeAuraID[i] = pTriggerData->nType;
        pMsg->arrWelfareAuraUpgradeTime[i] = pTriggerData->dwWashoutTime - nCurrentTime;
    }

    pMsg->wSize = sizeof(M2C_GUILD_INFO_NTF) + pMsg->byMemberCount * sizeof(GUILD_MEMBER_INFO);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientApplyToJoinGuildAck(UINT32 dwClientSessionID, UINT32 dwGuildID)
{
    INT32   nRetCode = 0;
    M2C_APPLY_TO_JOIN_GUILD_ACK* pMsg = (M2C_APPLY_TO_JOIN_GUILD_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_APPLY_TO_JOIN_GUILD_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_apply_to_join_guild_ack;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_APPLY_TO_JOIN_GUILD_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGetApplyGuildListAck(UINT32 dwClientSessionID, CMsGuild* pMsGuild)
{
    INT32   nRetCode = 0;
    MS_ROLE_DATA* pRoleData = NULL;
    APP_NODE* pNode = NULL;
    M2C_GET_APPLY_GUILD_LIST_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GET_APPLY_GUILD_LIST_ACK) + sizeof(GUILD_APPLICANT_INFO) * MAX_APPLY_GUILD_ROLE_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GET_APPLY_GUILD_LIST_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_get_apply_guild_list_ack;
    pMsg->wAppCount = 0;

    pNode = pMsGuild->GetFirstRoleJoinGuildApp();
    while (pNode)
    {
        LOG_CHECK_ERROR(pMsg->wAppCount < MAX_APPLY_GUILD_ROLE_NUM);
        if (pMsg->wAppCount == MAX_APPLY_GUILD_ROLE_NUM)
            break;

        if (EN_GUILD_APPLICATION == pNode->nType)
        {
            pRoleData = CRoleDataMgr::GetInstance().FindRole(pNode->dwID_1);
            LOG_CHECK_ERROR(pRoleData);
            if (pRoleData)
            {
                pMsg->info[pMsg->wAppCount].dwRoleID = pRoleData->dwRoleID;
                strxcpy(pMsg->info[pMsg->wAppCount].szRoleName, pRoleData->szRoleName, ROLE_NAME_LEN);
                pMsg->info[pMsg->wAppCount].byClass = pRoleData->byClass;
                pMsg->info[pMsg->wAppCount].byLevel = pRoleData->byLevel;
                pMsg->info[pMsg->wAppCount].dwEndTime = pNode->dwWashOutTime;
                ++pMsg->wAppCount;
            }
        }

        pNode = pMsGuild->GetNextRoleJoinGuildApp(pNode);
    }

    pMsg->wSize = sizeof(M2C_GET_APPLY_GUILD_LIST_ACK) + sizeof(GUILD_APPLICANT_INFO) * pMsg->wAppCount;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientRevertGuildApplyAck(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    M2C_REVERT_GUILD_APPLY_ACK* pMsg = (M2C_REVERT_GUILD_APPLY_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_REVERT_GUILD_APPLY_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_revert_guild_apply_ack;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_REVERT_GUILD_APPLY_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientDelGuildApp(UINT32 dwClientSessionID, UINT32 dwGuildID)
{
    INT32   nRetCode = 0;
    M2C_DEL_GUILD_APP* pMsg = (M2C_DEL_GUILD_APP*)m_pPacketAllocator->Alloc(sizeof(M2C_DEL_GUILD_APP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_del_guild_app;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_DEL_GUILD_APP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildAddMemberNtf(CMsGuild* pData, MS_ROLE_DATA& roleData, UINT8 byPosition, INT32 nNextSalary, UINT8 byFlag)
{
    INT32   nRetCode = 0;
    MS_ONLINE_ROLE_DATA* pstOnlineRoleData = NULL;
    M2C_GUILD_ADD_MEMBER_NTF* pMsg = (M2C_GUILD_ADD_MEMBER_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ADD_MEMBER_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_add_member_ntf;
    pMsg->info.dwRoleID = roleData.dwRoleID;
    strxcpy(pMsg->info.szRoleName, roleData.szRoleName, ROLE_NAME_LEN);
    pMsg->info.byLevel = roleData.byLevel;
    pMsg->info.byClass = roleData.byClass;
    pMsg->info.byPosition = byPosition;
    pMsg->info.nSceneID = roleData.nSceneTempID;
    pMsg->info.nNextSalary = nNextSalary;
    pMsg->info.byFlag = 0;

    pstOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(roleData.dwRoleID);
    if (pstOnlineRoleData)
        pMsg->info.nLastLoginTime = 0;
    else
        pMsg->info.nLastLoginTime = roleData.nLastLoginTime;

    nRetCode = _BroadcastGuild(pData, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ADD_MEMBER_NTF), roleData.dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildDelMemberNtf(CMsGuild* pMsGuild, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_DEL_MEMBER_NTF* pMsg = (M2C_GUILD_DEL_MEMBER_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_DEL_MEMBER_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_del_member_ntf;
    pMsg->dwRoleID = dwRoleID;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_DEL_MEMBER_NTF), dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientBeInvitedToJoinGuild(UINT32 dwClientSessionID, UINT32 dwRoleID, char* pRoleName, char* pGuildName)
{
    INT32   nRetCode = 0;
    M2C_BE_INVITED_TO_JOIN_GUILD* pMsg = (M2C_BE_INVITED_TO_JOIN_GUILD*)m_pPacketAllocator->Alloc(sizeof(M2C_BE_INVITED_TO_JOIN_GUILD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_be_invited_to_join_guild;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szRoleName, pRoleName, ROLE_NAME_LEN);
    strxcpy(pMsg->szGuildName, pGuildName, GUILD_NAME_LEN);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_BE_INVITED_TO_JOIN_GUILD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoCLientInviteToJoinGuildAck(UINT32 dwClientSessionID, char* pRoleName, INT32 nReplyType)
{
    INT32   nRetCode = 0;
    M2C_INVITE_TO_JOIN_GUILD_ACK* pMsg = (M2C_INVITE_TO_JOIN_GUILD_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_INVITE_TO_JOIN_GUILD_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_invite_to_join_guild_ack;
    strxcpy(pMsg->szRoleName, pRoleName, ROLE_NAME_LEN);
    pMsg->byReplyType = static_cast<UINT8>(nReplyType);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_INVITE_TO_JOIN_GUILD_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientDisbandGuildNtf(CMsGuild* pMsGuild)
{
    INT32   nRetCode = 0;
    M2C_DISBAND_GUILD_NTF* pMsg = (M2C_DISBAND_GUILD_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_DISBAND_GUILD_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_disband_guild_ntf;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_DISBAND_GUILD_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildNoticeChangeNtf(CMsGuild* pMsGuild)
{
    INT32   nRetCode = 0;
    M2C_GUILD_NOTICE_CHANGE* pMsg = (M2C_GUILD_NOTICE_CHANGE*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_NOTICE_CHANGE) + GUILD_NOTICE_LEN);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_notice_change;
    pMsg->wSize = (UINT16)strlen(pMsGuild->GetNotice()) + 1;
    LOG_PROCESS_ERROR(pMsg->wSize <= GUILD_NOTICE_LEN);
    strxcpy(pMsg->szNotice, pMsGuild->GetNotice(), pMsg->wSize);

    pMsg->wSize += sizeof(M2C_GUILD_NOTICE_CHANGE);

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szNotice);
        LOG_CHECK_ERROR(nRetCode);
    }

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildForbidChatNtf(CMsGuild* pMsGuild, UINT32 dwRoleID, INT32 nOp)
{
    INT32   nRetCode = 0;
    M2C_GUILD_FORBID_CHAT_NTF* pMsg = (M2C_GUILD_FORBID_CHAT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_FORBID_CHAT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_forbid_chat_ntf;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byOp = nOp;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_FORBID_CHAT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildChangePositionNtf(CMsGuild* pMsGuild, UINT32 dwRoleID, INT32 nPosition)
{
    INT32   nRetCode = 0;
    M2C_GUILD_CHANGE_POSITION_NTF* pMsg = (M2C_GUILD_CHANGE_POSITION_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_CHANGE_POSITION_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_change_position_ntf;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byPosition = nPosition;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_CHANGE_POSITION_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildRoleLeaveNtf(CMsGuild* pMsGuild, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_ROLE_LEAVE_NTF* pMsg = (M2C_GUILD_ROLE_LEAVE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ROLE_LEAVE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_role_leave_ntf;
    pMsg->dwRoleID = dwRoleID;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ROLE_LEAVE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildRoleLeaveAck(UINT32 dwClientSessionID, INT32 nLeaveType)
{
    INT32   nRetCode = 0;
    M2C_GUILD_ROLE_LEAVE_ACK* pMsg = (M2C_GUILD_ROLE_LEAVE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ROLE_LEAVE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_role_leave_ack;
    pMsg->byLeaveType = nLeaveType;


    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ROLE_LEAVE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildDemiseNtf(CMsGuild* pMsGuild, UINT32 dwOldLeaderID, UINT32 dwNewLeaderID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_DEMISE_NTF* pMsg = (M2C_GUILD_DEMISE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_DEMISE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_demise_ntf;
    pMsg->dwOldLeaderID = dwOldLeaderID;
    pMsg->dwNewLeaderID = dwNewLeaderID;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_DEMISE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildContributeAck(UINT32 dwClientSessionID, INT32 nType, INT32 nValue)
{
    INT32   nRetCode = 0;
    M2C_GUILD_CONTRIBUTE_ACK* pMsg = (M2C_GUILD_CONTRIBUTE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_CONTRIBUTE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_contribute_ack;
    pMsg->nType = nType;
    pMsg->nValue = nValue;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_CONTRIBUTE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildAttrModifyNtf(CMsGuild* pMsGuild, INT32 nAttrType, UINT32 dwAttrValue)
{
    INT32   nRetCode = 0;
    M2C_GUILD_ATTR_MODIFY_NTF* pMsg = (M2C_GUILD_ATTR_MODIFY_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ATTR_MODIFY_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_attr_modify_ntf;
    pMsg->nGuildAttrType = nAttrType;
    pMsg->dwGuildAttrValue = dwAttrValue;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ATTR_MODIFY_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildApplyAllianceAck(UINT32 dwClientSessionID, UINT32 dwGuildID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_APPLY_ALLIANCE_ACK* pMsg = (M2C_GUILD_APPLY_ALLIANCE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_APPLY_ALLIANCE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_apply_alliance_ack;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_APPLY_ALLIANCE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGetApplyUnionListAck(UINT32 dwClientSessionID, CMsGuild* pMsGuild)
{
    INT32   nRetCode = 0;
    M2C_GUILD_GET_APPLY_UNION_LIST_ACK* pMsg = NULL;
    MS_ROLE_DATA* pRoleData = NULL;
    DIPLOMAT_APP_NODE* pNode = NULL;
    CMsGuild* pApplicantGuild = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_GET_APPLY_UNION_LIST_ACK) + sizeof(APPLY_ALLIANCE_INFO) * GUILD_MAX_APPLY_ALLIANCE_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GUILD_GET_APPLY_UNION_LIST_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_get_apply_union_list_ack;
    pMsg->byCount = 0;

    pNode = pMsGuild->GetFirstAllianceApp();
    while (pNode)
    {
        LOG_CHECK_ERROR(pMsg->byCount < GUILD_MAX_APPLY_ALLIANCE_NUM);
        if (pMsg->byCount == GUILD_MAX_APPLY_ALLIANCE_NUM)
            break;
        pApplicantGuild = CMsGuildMgr::GetInstance().FindGuildByID(pNode->dwID_1);
        LOG_CHECK_ERROR(pApplicantGuild);
        if (pApplicantGuild)
        {
            pMsg->info[pMsg->byCount].byLevel = pApplicantGuild->GetLevel();
            pMsg->info[pMsg->byCount].dwGuildID = pApplicantGuild->GetID();
            strxcpy(pMsg->info[pMsg->byCount].szGuildName, pApplicantGuild->GetName(), GUILD_NAME_LEN);
            pRoleData = CRoleDataMgr::GetInstance().FindRole(pApplicantGuild->GetLeaderID());
            if (pRoleData)
            {
                strxcpy(pMsg->info[pMsg->byCount].szLeaderName, pRoleData->szRoleName, ROLE_NAME_LEN);
            }
            pMsg->info[pMsg->byCount].byMemberCount = (UINT8)pApplicantGuild->GetMemberNum();
            pMsg->info[pMsg->byCount].dwWashoutTime = pNode->dwWashOutTime;
            ++pMsg->byCount;
        }
        pNode = pMsGuild->GetNextAllianceApp(pNode);
    }

    pMsg->wSize = sizeof(M2C_GUILD_GET_APPLY_UNION_LIST_ACK) + pMsg->byCount * sizeof(APPLY_ALLIANCE_INFO);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildReplyUnionAppAck(UINT32 dwClientSessionID, UINT32 dwGuildID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_REPLY_UNION_APP_ACK* pMsg = (M2C_GUILD_REPLY_UNION_APP_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_REPLY_UNION_APP_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_reply_union_app_ack;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_REPLY_UNION_APP_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildAddDiplomatNtf(UINT8 byType, CMsGuild* pMsGuild, UINT32 dwGuildID, UINT32 dwLastTime, UINT8 byLevel, UINT8 byMemberCount, char* szGuildName)
{
    INT32   nRetCode = 0;
    M2C_GUILD_ADD_DIPLOMAT_NTF* pMsg = (M2C_GUILD_ADD_DIPLOMAT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ADD_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_add_diplomat_ntf;
    pMsg->byType = byType;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwLastTime = dwLastTime;
    pMsg->byLevel = byLevel;
    pMsg->byMemberCount = byMemberCount;
    strxcpy(pMsg->szGuildName, szGuildName, GUILD_NAME_LEN);

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ADD_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildDelDiplomatNtf(UINT8 byType, CMsGuild* pMsGuild, UINT32 dwGuildID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_DEL_DIPLOMAT_NTF* pMsg = (M2C_GUILD_DEL_DIPLOMAT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_DEL_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_del_diplomat_ntf;
    pMsg->byType = byType;
    pMsg->dwGuildID = dwGuildID;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_DEL_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildNotifyNtf(CMsGuild* poMSGuild, INT32 nType, INT32 nRight)
{
    INT32       nRetCode = 0;
    M2C_GUILD_NOTIFY_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(poMSGuild);

    pMsg = (M2C_GUILD_NOTIFY_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_NOTIFY_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_notify_ntf;
    pMsg->byType = nType;

    nRetCode = _BroadcastGuildCheckRight(poMSGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_NOTIFY_NTF), nRight);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildChangeStorageRightNtf(CMsGuild* pMsGuild, INT32 nPosition, UINT16 wRight)
{
    INT32   nRetCode = 0;
    M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF* pMsg = (M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_change_storage_right_ntf;
    pMsg->byPosition = nPosition;
    pMsg->wRight = wRight;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoClientGuildViewStorage(UINT32 dwClientSessionID, CMsGuild* poMsGuild, UINT8 byStorageTag)
{
    INT32   nRetCode = 0;
    M2C_GUILD_VIEW_STORAGE_ACK* pMsg = NULL;
    char* pBuff = NULL;
    GUILD_STORAGE_ITEM_DATA* pItemInfo = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_VIEW_STORAGE_ACK) + sizeof(GUILD_STORAGE_ITEM_INFO) * GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GUILD_VIEW_STORAGE_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poMsGuild);

    pMsg->wMsg = m2c_guild_view_storage_ack;
    pMsg->byStorageTag = byStorageTag;

    pMsg->wItemCount = 0;
    pItemInfo = poMsGuild->GetStorage().GetFirstConSlotItem(byStorageTag);
    while (pItemInfo)
    {
        LOG_CHECK_ERROR(pMsg->wItemCount < GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG);
        if (pMsg->wItemCount == GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG)
            break;
        memcpy(pMsg->info[pMsg->wItemCount].szItemBuffer, pItemInfo->szItemBuffer,
            sizeof(pItemInfo->szItemBuffer) > sizeof(pMsg->info[pMsg->wItemCount].szItemBuffer) ? sizeof(pMsg->info[pMsg->wItemCount].szItemBuffer) : sizeof(pItemInfo->szItemBuffer));

        pMsg->info[pMsg->wItemCount].byConSlot = pItemInfo->byConSlot;
        pMsg->info[pMsg->wItemCount].byItemSlot = pItemInfo->byItemSlot;
        ++pMsg->wItemCount;

        pItemInfo = poMsGuild->GetStorage().GetNextConSlotItem();
    }

    pMsg->wSize = pMsg->wItemCount * sizeof(GUILD_STORAGE_ITEM_INFO) + sizeof(M2C_GUILD_VIEW_STORAGE_ACK);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildStoraveEvent(UINT32 dwClientSessionID, CMsGuild* poMsGuild)
{
    INT32   nRetCode = 0;
    M2C_GUILD_STORAGE_EVENT* pMsg = NULL;
    char* pBuff = NULL;
    INT32   nStorageEventCount = 0;
    STORAGE_EVENT_DATA* pEventData = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_STORAGE_EVENT) + sizeof(MSG_GUILD_STORAGE_EVENT) * GUILD_MAX_STORAGE_EVENT_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GUILD_STORAGE_EVENT*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(poMsGuild);

    pMsg->wMsg = m2c_guild_storage_event;

    pMsg->byEventCount = 0;
    pEventData = poMsGuild->GetStorage().GetFirstStorageEventData();
    while (pEventData)
    {
        LOG_CHECK_ERROR(pMsg->byEventCount < GUILD_MAX_STORAGE_EVENT_NUM);
        if (pMsg->byEventCount == GUILD_MAX_STORAGE_EVENT_NUM)
            break;
        pMsg->info[pMsg->byEventCount].byType = pEventData->nType;
        pMsg->info[pMsg->byEventCount].dwTime = pEventData->dwOccurTime;
        pMsg->info[pMsg->byEventCount].dwRoleID = pEventData->dwRoleID;
        memcpy(pMsg->info[pMsg->byEventCount].szItemBuffer, pEventData->szItemBuffer,
            sizeof(pMsg->info[pMsg->byEventCount].szItemBuffer) > sizeof(pEventData->szItemBuffer) ? sizeof(pEventData->szItemBuffer) : sizeof(pMsg->info[pMsg->byEventCount].szItemBuffer));
        ++pMsg->byEventCount;
        pEventData = poMsGuild->GetStorage().GetNextStorageEventData(pEventData);
    }

    pMsg->wSize = pMsg->byEventCount * sizeof(MSG_GUILD_STORAGE_EVENT) + sizeof(M2C_GUILD_STORAGE_EVENT);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildPushItemAck(UINT32 dwClientSessionID, INT32 nConSlot, INT32 nSlot, INT32 nItemSize, const char* pItemData)
{
    INT32   nRetCode = 0;
    M2C_GUILD_PUSH_ITEM_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_PUSH_ITEM_ACK) + nItemSize < MAX_MESSAGE_SIZE);

    pMsg = (M2C_GUILD_PUSH_ITEM_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(nItemSize < MAX_MESSAGE_SIZE / 2);    // An item cannot be this large

    pMsg->wMsg = m2c_guild_push_item_ack;
    pMsg->byStorageConSlot = nConSlot;
    pMsg->byStorageSlot = nSlot;
    pMsg->wItemSize = nItemSize;

    memcpy(pMsg->buff, pItemData, nItemSize);
    pMsg->wSize = pMsg->wItemSize + sizeof(M2C_GUILD_PUSH_ITEM_ACK);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildPopItem(UINT32 dwClientSessionID, UINT8 byConSlot, UINT8 bySlot)
{
    INT32   nRetCode = 0;
    M2C_GUILD_POP_ITEM_ACK* pMsg = (M2C_GUILD_POP_ITEM_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_POP_ITEM_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_pop_item_ack;
    pMsg->byStorageConSlot = byConSlot;
    pMsg->byStorageSlot = bySlot;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_POP_ITEM_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildMoveItem(UINT32 dwClientSessionID, INT32 nSourceConSlot, INT32 nSourceItemSlot, INT32 nTargetConSlot, INT32 nTargetItemSlot)
{
    INT32   nRetCode = 0;
    M2C_GUILD_MOVE_ITEM_ACK* pMsg = (M2C_GUILD_MOVE_ITEM_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_MOVE_ITEM_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_move_item_ack;
    pMsg->bySourceConSlot = nSourceConSlot;
    pMsg->bySourceSlot = nSourceItemSlot;
    pMsg->byTargetConSlot = nTargetConSlot;
    pMsg->byTargetSlot = nTargetItemSlot;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_MOVE_ITEM_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAddHistoryEventNtf(CMsGuild* pMsGuild, UINT8 byType, UINT32 dwParam1, UINT32 dwParam2, UINT32 dwParam3, UINT32 dwParam4, UINT32 dwOccurTime)
{
    INT32   nRetCode = 0;
    M2C_GUILD_ADD_HISTORY_EVENT_NTF* pMsg = NULL;
    LOG_PROCESS_ERROR(pMsGuild);

    pMsg = (M2C_GUILD_ADD_HISTORY_EVENT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ADD_HISTORY_EVENT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_add_history_event_ntf;
    pMsg->byType = byType;
    pMsg->dwParam1 = dwParam1;
    pMsg->dwParam2 = dwParam2;
    pMsg->dwParam3 = dwParam3;
    pMsg->dwParam4 = dwParam4;
    pMsg->dwOccurTime = dwOccurTime;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ADD_HISTORY_EVENT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAddStorageEventNtf(CMsGuild* pMsGuild, UINT8 byType, UINT32 dwRoleID, UINT32 dwOccurTime, INT32 nLength, const char* pcszItemBuffer)
{
    INT32   nRetCode = 0;
    INT32   nActuallyItemBufferLength = 0;
    M2C_GUILD_ADD_STORAGE_EVENT_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(pMsGuild);
    LOG_PROCESS_ERROR(pcszItemBuffer);

    pMsg = (M2C_GUILD_ADD_STORAGE_EVENT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ADD_STORAGE_EVENT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_add_storage_event_ntf;
    pMsg->byType = byType;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwOccurTime = dwOccurTime;
    nActuallyItemBufferLength = nLength > sizeof(pMsg->szItemBuffer) ? sizeof(pMsg->szItemBuffer) : nLength;
    memcpy(pMsg->szItemBuffer, pcszItemBuffer, nActuallyItemBufferLength);

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ADD_STORAGE_EVENT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildMemberSalaryChange(UINT32 dwClientSessionID, UINT32 dwSalary)
{
    INT32   nRetCode = 0;
    M2C_GUILD_MEMBER_SALARY_CHANGE* pMsg = (M2C_GUILD_MEMBER_SALARY_CHANGE*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_MEMBER_SALARY_CHANGE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_member_salary_change;
    pMsg->dwSalary = dwSalary;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_MEMBER_SALARY_CHANGE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientSyncSocialAttrNtf(CMsGuild* pMsGuild, UINT32 dwRoleID, UINT8 byLevel, UINT32 dwValue)
{
    INT32   nRetCode = 0;
    M2C_SYNC_SOCIAL_ATTR_NTF* pMsg = (M2C_SYNC_SOCIAL_ATTR_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_SYNC_SOCIAL_ATTR_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_sync_social_attr_ntf;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byLevel = byLevel;
    pMsg->dwValue = dwValue;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_SYNC_SOCIAL_ATTR_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildMoreHistoryAck(UINT32 dwClientSessionID, UINT32 dwGuildID)
{
    INT32       nRetCode = 0;
    CMsGuild* poMsGuild = NULL;
    EVENT_DATA* pEventData = NULL;
    INT32       nEventCount = 0;
    M2C_GUILD_MORE_HISTORY* pMsg = NULL;

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_MORE_HISTORY) + sizeof(GUILD_EVENT_INFO) * GUILD_MAX_HISTORY_EVENT_NUM < MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(0 != dwGuildID);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);

    pEventData = poMsGuild->GetFirstGuildEventData();
    while (pEventData)
    {
        ++nEventCount;
        if (nEventCount == GUILD_DATA_NTF_HISTORY_NUM)
            break;
        pEventData = poMsGuild->GetNextGuildEventData(pEventData);
    }
    PROCESS_ERROR(pEventData);

    pMsg = (M2C_GUILD_MORE_HISTORY*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_more_history;

    pMsg->wEventNum = 0;
    //pEventData = poMsGuild->GetNextGuildEventData(pEventData);
    while (pEventData)
    {
        LOG_CHECK_ERROR(pMsg->wEventNum < GUILD_MAX_HISTORY_EVENT_NUM);
        if (pMsg->wEventNum == GUILD_MAX_HISTORY_EVENT_NUM)
            break;
        pMsg->stEventInfo[pMsg->wEventNum].byType = pEventData->nType;
        pMsg->stEventInfo[pMsg->wEventNum].param1 = pEventData->param1;
        pMsg->stEventInfo[pMsg->wEventNum].param2 = pEventData->param2;
        pMsg->stEventInfo[pMsg->wEventNum].param3 = pEventData->param3;
        pMsg->stEventInfo[pMsg->wEventNum].param4 = pEventData->param4;
        pMsg->stEventInfo[pMsg->wEventNum++].dwTime = pEventData->dwOccurTime;
        pEventData = poMsGuild->GetNextGuildEventData(pEventData);
    }

    pMsg->wSize = sizeof(M2C_GUILD_MORE_HISTORY) + pMsg->wEventNum * sizeof(GUILD_EVENT_INFO);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildMoreChestAck(UINT32 dwClientSessionID, UINT32 dwGuildID)
{
    INT32       nRetCode = 0;
    CMsGuild* poMsGuild = NULL;
    EVENT_DATA* pEventData = NULL;
    INT32       nEventCount = 0;
    M2C_GUILD_MORE_CHEST* pMsg = NULL;

    LOG_PROCESS_ERROR(0 != dwGuildID);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);

    pEventData = poMsGuild->FirstChestEvent();
    while (pEventData)
    {
        ++nEventCount;
        if (nEventCount == GUILD_MAX_CHEST_NTF_NUM)
            break;
        pEventData = poMsGuild->NextChestEvent(pEventData);
    }
    LOG_PROCESS_ERROR(pEventData);

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_MORE_CHEST) + sizeof(GUILD_EVENT_INFO) * GUILD_MAX_CHEST_EVENT_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GUILD_MORE_CHEST*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_more_chest;

    pMsg->wEventNum = 0;
    pEventData = poMsGuild->FirstChestEvent();
    while (pEventData)
    {
        LOG_CHECK_ERROR(pMsg->wEventNum < GUILD_MAX_CHEST_EVENT_NUM);
        if (pMsg->wEventNum == GUILD_MAX_CHEST_EVENT_NUM)
            break;
        pMsg->stEvent[pMsg->wEventNum].byType = pEventData->nType;
        pMsg->stEvent[pMsg->wEventNum].param1 = pEventData->param1;
        pMsg->stEvent[pMsg->wEventNum].param2 = pEventData->param2;
        pMsg->stEvent[pMsg->wEventNum].param3 = pEventData->param3;
        pMsg->stEvent[pMsg->wEventNum].param4 = pEventData->param4;
        pMsg->stEvent[pMsg->wEventNum++].dwTime = pEventData->dwOccurTime;
        pEventData = poMsGuild->NextChestEvent(pEventData);
    }

    pMsg->wSize = sizeof(M2C_GUILD_MORE_CHEST) + pMsg->wEventNum * sizeof(GUILD_EVENT_INFO);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildJoinGuildAck(UINT32 dwClientSessionID, const char* pcszGuildName)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    M2C_GUILD_JOIN_GUILD_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(pcszGuildName);

    pMsg = (M2C_GUILD_JOIN_GUILD_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_JOIN_GUILD_NTF));
    pMsg->wMsg = m2c_guild_join_guild_ntf;
    strxcpy(pMsg->szGuildName, pcszGuildName, GUILD_NAME_LEN);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_JOIN_GUILD_NTF));
    LOG_PROCESS_ERROR(nRetCode);
    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildUpgradeWelfareNtf(CMsGuild* pMsGuild, INT32 nType, INT32 nTime)
{
    INT32   nRetCode = 0;
    M2C_GUILD_UPGRADE_WELFARE_NTF* pMsg = (M2C_GUILD_UPGRADE_WELFARE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_UPGRADE_WELFARE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_upgrade_welfare_ntf;
    pMsg->byType = nType;
    pMsg->nTime = nTime;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_UPGRADE_WELFARE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildUpdateWelfareNtf(CMsGuild* pMsGuild, INT32 nType, UINT8 byLevel)
{
    INT32   nRetCode = 0;
    M2C_GUILD_UPDATE_WELFARE_NTF* pMsg = (M2C_GUILD_UPDATE_WELFARE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_UPDATE_WELFARE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_update_welfare_ntf;
    pMsg->byType = nType;
    pMsg->byLevel = byLevel;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_UPDATE_WELFARE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildCancelWelfareUpgradeNtf(CMsGuild* pMsGuild, UINT32 dwID)
{
    INT32   nRetCode = 0;
    M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF* pMsg = (M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_cancel_welfare_upgrade_ntf;
    pMsg->dwID = dwID;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildUpgradeBuildingNtf(CMsGuild* pMsGuild, UINT64 qwNameID, INT32 nType, INT32 nTime)
{
    INT32   nRetCode = 0;
    M2C_GUILD_UPGRADE_BUILDING_NTF* pMsg = (M2C_GUILD_UPGRADE_BUILDING_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_UPGRADE_BUILDING_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_upgrade_building_ntf;
    pMsg->qwNpcNameID = qwNameID;
    pMsg->byType = nType;
    pMsg->nTime = nTime;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_UPGRADE_BUILDING_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildUpdateBuildingNtf(CMsGuild* pMsGuild, UINT64 qwNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    M2C_GUILD_UPDATE_BUILDING_NTF* pMsg = (M2C_GUILD_UPDATE_BUILDING_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_UPDATE_BUILDING_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_update_building_ntf;
    pMsg->qwNpcNameID = qwNameID;
    pMsg->byType = nType;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_UPDATE_BUILDING_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildCancelBuildingUpgradeNtf(CMsGuild* pMsGuild, UINT64 qwNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF* pMsg = (M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_cancel_building_upgrade_ntf;
    pMsg->qwNpcNameID = qwNameID;
    pMsg->byType = nType;

    // broadcast except itself
    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildDelBuildingNtf(CMsGuild* poMSGuild, UINT64 qwNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    M2C_GUILD_DEL_BUILDING_NTF* pMsg = (M2C_GUILD_DEL_BUILDING_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_DEL_BUILDING_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_del_building_ntf;
    pMsg->qwNpcNameID = qwNameID;
    pMsg->byType = nType;

    // broadcast except itself
    nRetCode = _BroadcastGuild(poMSGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_DEL_BUILDING_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildGetDungeonListAck(MS_ONLINE_ROLE_DATA* pstonlineRoleData, CMsGuild* poMsGuild)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    GUILD_DUNGEON* pGDungeon = NULL;
    M2C_GUILD_GET_DUNGEON_LIST_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(poMsGuild);
    LOG_PROCESS_ERROR(pstonlineRoleData);
    LOG_PROCESS_ERROR(pstonlineRoleData->pRoleData);

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_GET_DUNGEON_LIST_ACK) + sizeof(GUILD_DUNGEON_INFO) * GUILD_MAX_DUNGEON_COUNT < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GUILD_GET_DUNGEON_LIST_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_get_dungeon_list_ack;
    pMsg->byDungeonCount = 0;

    pGDungeon = poMsGuild->GetFirstDungeon();
    while (pGDungeon)
    {
        LOG_CHECK_ERROR(pMsg->byDungeonCount < GUILD_MAX_DUNGEON_COUNT);
        if (pMsg->byDungeonCount == GUILD_MAX_DUNGEON_COUNT)
            break;
        pMsg->info[pMsg->byDungeonCount].dwTemplateID = pGDungeon->dwTemplateID;
        pMsg->info[pMsg->byDungeonCount].dwOwnerID = pGDungeon->dwOwnerID;
        pMsg->info[pMsg->byDungeonCount].nCreateTime = pGDungeon->nCreateTime;
        ++pMsg->byDungeonCount;

        pGDungeon = poMsGuild->GetNextDungeon();
    }

    pMsg->wSize = sizeof(M2C_GUILD_GET_DUNGEON_LIST_ACK) + sizeof(GUILD_DUNGEON_INFO) * pMsg->byDungeonCount;

    nRetCode = _SendClient(pstonlineRoleData->dwCliSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildOpenDungeonAck(UINT32 dwClientSessionID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;

    M2C_GUILD_OPEN_DUNGEON_ACK* pMsg = (M2C_GUILD_OPEN_DUNGEON_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_OPEN_DUNGEON_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_open_dungeon_ack;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_OPEN_DUNGEON_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildDungeonAwardNtf(SCENE_INFO* poMsScene, UINT32 dwOwnerID, INT32 nCount, UINT32 arrItemID[], UINT32 arrItemCount[])
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    M2C_GUILD_DUNGEON_AWARD* pMsg = NULL;

    LOG_PROCESS_ERROR(poMsScene);
    LOG_PROCESS_ERROR(nCount <= GUILD_DUNGEON_AWARD_NUM);

    LOG_PROCESS_ERROR(sizeof(M2C_GUILD_DUNGEON_AWARD) + sizeof(GUILD_DUNGEON_AWARD_INFO) * GUILD_DUNGEON_AWARD_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2C_GUILD_DUNGEON_AWARD*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_dungeon_award;
    pMsg->wSize = sizeof(M2C_GUILD_DUNGEON_AWARD);
    pMsg->dwOwnerID = dwOwnerID;
    pMsg->byItemCount = 0;

    for (INT32 i = 0; i < nCount; ++i)
    {
        LOG_CHECK_ERROR(pMsg->byItemCount < GUILD_DUNGEON_AWARD_NUM);
        if (pMsg->byItemCount == GUILD_DUNGEON_AWARD_NUM)
            break;
        pMsg->arrItem[pMsg->byItemCount].dwItemID = arrItemID[i];
        pMsg->arrItem[pMsg->byItemCount].byCount = arrItemCount[i];
        ++pMsg->byItemCount;
    }
    pMsg->wSize += (pMsg->byItemCount * sizeof(GUILD_DUNGEON_AWARD_INFO));

    nRetCode = _BroadcastScene(poMsScene, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientBattleAchievementNtf(CMsGuild* pMsGuild, INT32 nNum, GUILD_WAR_PERSONAL_ACHIEVEMENT arrInfo[])
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    M2C_GUILD_BATTLE_ACHIEVEMENT* pMsg = NULL;

    LOG_PROCESS_ERROR(pMsGuild);
    LOG_PROCESS_ERROR(nNum <= GUILD_MAX_MEMBER_COUNT);

    pMsg = (M2C_GUILD_BATTLE_ACHIEVEMENT*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_battle_achievement;
    pMsg->wSize = sizeof(M2C_GUILD_BATTLE_ACHIEVEMENT);

    pMsg->byNum = 0;
    for (INT32 i = 0; i < nNum; ++i)
    {
        pMsg->info[pMsg->byNum].dwRoleID = arrInfo[i].dwRoleID;
        pMsg->info[pMsg->byNum].nKillNum = arrInfo[i].nKillNum;
        pMsg->info[pMsg->byNum].nDealCount = arrInfo[i].nDeadCount;
        pMsg->info[pMsg->byNum].nSalary = arrInfo[i].nSalary;
        ++pMsg->byNum;
    }

    pMsg->wSize += (pMsg->byNum * sizeof(GUILD_BATTLE_ACHIEVEMENT_DATA));

    nRetCode = _BroadcastGuild(pMsGuild, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoClientGuildSyncBuildingStateNtf(CMsGuild* poMsGuild, UINT64 qwNameID, UINT32 dwReviveTime)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    M2C_GUILD_BUILDING_STATE_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(poMsGuild);

    pMsg = (M2C_GUILD_BUILDING_STATE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_BUILDING_STATE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_building_state_ntf;
    pMsg->qwNameID = qwNameID;
    pMsg->dwReviveTime = dwReviveTime;

    nRetCode = _BroadcastGuild(poMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_BUILDING_STATE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildSyncWarCDNtf(CMsGuild* poMsGuild, UINT32 dwCD)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    M2C_GUILD_SYNC_WAR_CD_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(poMsGuild);
    pMsg = (M2C_GUILD_SYNC_WAR_CD_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_SYNC_WAR_CD_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_sync_war_cd_ntf;
    pMsg->dwWarCD = dwCD;

    nRetCode = _BroadcastGuild(poMsGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_SYNC_WAR_CD_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildAchievementAck(UINT32 dwClientSessionID, CMsGuild* poMsGuild)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    M2C_GUILD_ACHIEVEMENT_ACK* pMsg = NULL;

    LOG_PROCESS_ERROR(poMsGuild);
    LOG_PROCESS_ERROR(poMsGuild->GetAchievementData());

    pMsg = (M2C_GUILD_ACHIEVEMENT_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_ACHIEVEMENT_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_achievement_ack;
    for (INT32 i = 0; i < GUILD_ACHI_TYPE_NUM; ++i)
    {
        pMsg->acAchievement[i] = poMsGuild->GetAchievementData()[i];
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_ACHIEVEMENT_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientGuildJoinWarNtf(CMsGuild* poGuild, INT32 nWarCharacter, UINT32 dwTargetGuildID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    M2C_GUILD_JOIN_WAR_NTF* pMsg = NULL;

    LOG_PROCESS_ERROR(poGuild);
    pMsg = (M2C_GUILD_JOIN_WAR_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_GUILD_JOIN_WAR_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_guild_join_war_ntf;
    pMsg->byWarCharacter = (UINT8)nWarCharacter;
    pMsg->dwTargetGuildID = dwTargetGuildID;

    nRetCode = _BroadcastGuild(poGuild, (PACKET_BUFFER*)pMsg, sizeof(M2C_GUILD_JOIN_WAR_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientEnrollSabac(INT32 nServerGroup, UINT32 dwGuildID)
{
    INT32 nRetCode = 0;
    M2C_ENROLL_SABAC* pMsg = NULL;

    pMsg = (M2C_ENROLL_SABAC*)m_pPacketAllocator->Alloc(sizeof(M2C_ENROLL_SABAC));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_enroll_sabac;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _BroadcastGroupClient(nServerGroup, (PACKET_BUFFER*)pMsg, sizeof(M2C_ENROLL_SABAC));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;

}

BOOL CMessageHandler::DoClientOccupySabac(UINT32 dwCliSessionID, INT32 nServerGroup, UINT32 dwOldGuildID, UINT32 dwNewGuildID)
{
    INT32 nRetCode = 0;
    M2C_OCCUPY_SABAC* pMsg = NULL;

    pMsg = (M2C_OCCUPY_SABAC*)m_pPacketAllocator->Alloc(sizeof(M2C_OCCUPY_SABAC));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_occupy_sabac;
    pMsg->dwOldGuildID = dwOldGuildID;
    pMsg->dwNewGuildID = dwNewGuildID;

    if (dwCliSessionID == 0)
    {
        nRetCode = _BroadcastGroupClient(nServerGroup, (PACKET_BUFFER*)pMsg, sizeof(M2C_OCCUPY_SABAC));
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = _SendClient(dwCliSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_OCCUPY_SABAC));
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// Erect statue
BOOL CMessageHandler::DoClientErectStatue(INT32 nServerGroup, UINT32 dwCliSessionID)
{
    INT32 nRetCode = 0;
    M2C_ERECT_STATUE* pMsg = NULL;
    UINT32 dwRoleID = 0;
    MS_ROLE_DATA* pRoleData = NULL;
    INT32 nVarValue = 0;

    dwRoleID = CServerVar::GetInstance().GetVariable(nServerGroup, svtSabacWinner0);
    if (dwRoleID != INVALID_OBJECT_ID)
    {
        pMsg = (M2C_ERECT_STATUE*)m_pPacketAllocator->Alloc(sizeof(M2C_ERECT_STATUE));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = m2c_erect_statue;
        pMsg->dwRoleID = dwRoleID;

        pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
        PROCESS_ERROR(pRoleData);

        pMsg->byClass = pRoleData->byClass;
        pMsg->bySex = pRoleData->bySex;
        strxcpy(pMsg->szRoleName, pRoleData->szRoleName, ROLE_NAME_LEN);

        nVarValue = CServerVar::GetInstance().GetVariable(nServerGroup, svtSabacWinner1);
        pMsg->byFaceType = (nVarValue & 0xFF000000) >> 24;
        pMsg->byHairType = (nVarValue & 0x00FF0000) >> 16;
        pMsg->byHairColor = (nVarValue & 0x0000FF00) >> 8;
        pMsg->bySkinColor = (nVarValue & 0x000000FF);

        nVarValue = CServerVar::GetInstance().GetVariable(nServerGroup, svtSabacWinner2);
        pMsg->byTitleID = (nVarValue & 0xFF000000) >> 24;
        pMsg->byRefineFx = (nVarValue & 0x00FF0000) >> 16;

        pMsg->anAvatar[NES_MAIN_HAND] = CServerVar::GetInstance().GetVariable(nServerGroup, svtSabacWinner3);
        pMsg->anAvatar[NES_BODY] = CServerVar::GetInstance().GetVariable(nServerGroup, svtSabacWinner4);
        pMsg->anAvatar[NES_BACK] = CServerVar::GetInstance().GetVariable(nServerGroup, svtSabacWinner5);

        if (dwCliSessionID == 0)
        {
            nRetCode = _BroadcastGroupClient(nServerGroup, (PACKET_BUFFER*)pMsg, sizeof(M2C_ERECT_STATUE));
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            nRetCode = _SendClient(dwCliSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_ERECT_STATUE));
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// Sabac member enters the game
BOOL CMessageHandler::DoClientSabacWinnerLogin(MS_ONLINE_ROLE_DATA* pOnlineRoleData)
{
    INT32 nRetCode = 0;
    M2C_SABAC_WINNER_LOGIN* pMsg = NULL;

    LOG_PROCESS_ERROR(pOnlineRoleData && pOnlineRoleData->pRoleData);

    pMsg = (M2C_SABAC_WINNER_LOGIN*)m_pPacketAllocator->Alloc(sizeof(M2C_SABAC_WINNER_LOGIN));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_sabac_winner_login;
    strxcpy(pMsg->szRoleName, pOnlineRoleData->pRoleData->szRoleName, ROLE_NAME_LEN);

    nRetCode = _BroadcastGroupClient(pOnlineRoleData->pRoleData->byGroup, (PACKET_BUFFER*)pMsg, sizeof(M2C_SABAC_WINNER_LOGIN));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


void CMessageHandler::OnClientGuildQueryName(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    C2M_QUERY_GUILD_NAME* pMsg = (C2M_QUERY_GUILD_NAME*)pPacketBuffer;
    CMsGuild* poMsGuild = NULL;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildQueryFailed);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild->GetSvrGroup() == pstOnlineRole->pRoleData->byGroup, errGuildQueryFailed); // TODO: revisit if cross-server maps are added

    nRetCode = DoClientGuildQueryName(pstOnlineRole->dwCliSessionID, poMsGuild, pMsg->dwCRC);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    if (GetTypeFromID(pMsg->dwGuildID) != otGuild)
        ERR("Query Guild Name Failed, GuildID[%u]", pMsg->dwGuildID);
    CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, pMsg->dwGuildID, 0);
    return;
}


void CMessageHandler::OnClientViewGuildList(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;

    C2M_VIEW_GUILD_LIST* pMsg = (C2M_VIEW_GUILD_LIST*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);

    nRetCode = DoClientViewGuildListAck(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->byGroup, pMsg->byViewType, pMsg->dwLastGuildID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildViewGuildFailed);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientMoreGuildInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;

    C2M_MORE_GUILD_INFO* pMsg = (C2M_MORE_GUILD_INFO*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);
    LOG_PROCESS_ERROR(poMsGuild->GetSvrGroup() == pstOnlineRole->pRoleData->byGroup);

    nRetCode = DoClientMoreGuildInfoAck(pstOnlineRole->dwCliSessionID, poMsGuild);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientFindGuild(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuild* pData = NULL;
    MS_ROLE_DATA* pLeaderData = NULL;

    C2M_FIND_GUILD* pMsg = (C2M_FIND_GUILD*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    if (pMsg->dwGuildID)
    {
        pData = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwGuildID);
    }
    else // find by name
    {
        pData = CMsGuildMgr::GetInstance().FindGuildByName(pMsg->szGuildName);
    }
    PROCESS_ERROR_RET_CODE(pData, errGuildNotFind);
    PROCESS_ERROR_RET_CODE(pData->GetSvrGroup() == pstOnlineRole->pRoleData->byGroup, errGuildNotFind);

    pLeaderData = CRoleDataMgr::GetInstance().FindRole(pData->GetLeaderID());
    PROCESS_ERROR_RET_CODE(pLeaderData, errGuildLeaderNotExist);

    nRetCode = DoClientFindGuildAck(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->byGroup, pData, pLeaderData->szRoleName);
    PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientApplyToJoinGuild(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;

    C2M_APPLY_TO_JOIN_GUILD* pMsg = (C2M_APPLY_TO_JOIN_GUILD*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR_RET_CODE(!pstOnlineRole->pUser->bFreeMode, errFreeMode);

    if (0 == pMsg->dwGuildID)
    {
        pMsGuild = CMsGuildMgr::GetInstance().FindGuildByName(pMsg->szGuildName);
        LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);
        LOG_PROCESS_ERROR_RET_CODE(pMsGuild->GetSvrGroup() == pstOnlineRole->pRoleData->byGroup, errGuildNotFind);
    }
    else
    {
        pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwGuildID);
        LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);
        LOG_PROCESS_ERROR_RET_CODE(pMsGuild->GetSvrGroup() == pstOnlineRole->pRoleData->byGroup, errGuildNotFind);
    }

    nResult = pMsGuild->RoleDeliverApplication(*pstOnlineRole);
    PROCESS_ERROR(nResult == errSuccess);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGetApplyGuildList(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    INT32   nRetCode = 0;
    CMsGuildMember* poMsGuildMember = NULL;
    CMsGuild* pMsGuild = NULL;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    poMsGuildMember = pMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    // check right
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_RECRUIT, errGuildNoRight);

    nRetCode = CMessageHandler::GetInstance().DoClientGetApplyGuildListAck(pstOnlineRole->dwCliSessionID, pMsGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientRevertGuildApply(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_REVERT_GUILD_APPLY* pMsg = (C2M_REVERT_GUILD_APPLY*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    if (pMsg->byRevert == GUILD_REVERT_REFUSE)
    {
        nResult = pMsGuild->RefuseRoleApply(pMsg->dwRoleID, *pstOnlineRole);
        LOG_PROCESS_ERROR(nResult == errSuccess);
    }
    else if (pMsg->byRevert == GUILD_REVERT_AGREE)
    {
        nResult = pMsGuild->AgreeRoleApply(pstOnlineRole->pRoleData->dwRoleID, pMsg->dwRoleID);
        PROCESS_ERROR(nResult == errSuccess);
    }

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientInviteToJoinGuild(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    MS_ROLE_DATA* pTargetRole = NULL;
    CMsGuild* poGuild = NULL;
    C2M_INVITE_TO_JOIN_GUILD* pMsg = (C2M_INVITE_TO_JOIN_GUILD*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poGuild, errGuildNotFind);

    pTargetRole = CRoleDataMgr::GetInstance().FindRoleByName(pMsg->szRoleName);
    LOG_PROCESS_ERROR_RET_CODE(pTargetRole, errGuildRoleNotExist);
    LOG_PROCESS_ERROR_RET_CODE(poGuild->GetSvrGroup() == pTargetRole->byGroup, errGuildRoleNotExist);

    nResult = poGuild->InviteToJoinGuild(*pstOnlineRole, pMsg->szRoleName);
    PROCESS_ERROR(nResult == errSuccess);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientRevertGuildInvitation(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_REVERT_GUILD_INVITATION* pMsg = (C2M_REVERT_GUILD_INVITATION*)pPacketBuffer;
    MS_ROLE_DATA* pRoleData = NULL;
    MS_ONLINE_ROLE_DATA* pInviterRoleData = NULL;

    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    if (GUILD_REVERT_REFUSE == pMsg->byResult)
    {
        pstOnlineRole->dwGuildInviterID = 0;    // Clear the invitation record after either accepting or rejecting. Allan Tao [22.4.2013]

        pInviterRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
        if (pInviterRoleData)
        {
            nRetCode = CMessageHandler::GetInstance().DoCLientInviteToJoinGuildAck(pInviterRoleData->dwCliSessionID,
                pstOnlineRole->pRoleData->szRoleName, GUILD_REPLY_REFUSE);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
        }
    }
    else if (GUILD_REVERT_AGREE == pMsg->byResult)
    {
        pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
        LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGuildRoleNotExist);

        PROCESS_ERROR_RET_CODE(pRoleData->dwGuildID != 0, errGuildNotFind);
        pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pRoleData->dwGuildID);
        LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

        nResult = pMsGuild->AgreeToJoinGuild(pMsg->dwRoleID, *pstOnlineRole);
        PROCESS_ERROR(errSuccess == nResult);
    }

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientDisbandGuild(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->DisbandGuild(*pstOnlineRole);
    LOG_PROCESS_ERROR(nResult == errSuccess);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildNoticeChange(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_NOTICE_CHANGE* pMsg = (C2M_GUILD_NOTICE_CHANGE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->ChangeNotice(*pstOnlineRole, pMsg->szNotice);
    LOG_PROCESS_ERROR(nResult == errSuccess);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildAnnounceChange(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_ANNOUNCE_CHANGE* pMsg = (C2M_GUILD_ANNOUNCE_CHANGE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->ChangeAnnounce(*pstOnlineRole, pMsg->szAnnounce);
    LOG_PROCESS_ERROR(nResult == errSuccess);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildForbidChat(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_FORBID_CHAT* pMsg = (C2M_GUILD_FORBID_CHAT*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->ForbidChat(*pstOnlineRole, pMsg->byOperation, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildChangePosition(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_CHANGE_POSITION* pMsg = (C2M_GUILD_CHANGE_POSITION*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->ChangePosition(*pstOnlineRole, pMsg->byPosition, pMsg->dwRoleID);
    PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildLeave(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->LeaveGuild(*pstOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildBanishRole(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;

    C2M_GUILD_BANISH_ROLE* pMsg = (C2M_GUILD_BANISH_ROLE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);


    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->BanishSomebody(*pstOnlineRole, pMsg->dwRoleID);
    PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildDemise(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_DEMISE* pMsg = (C2M_GUILD_DEMISE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->Demise(*pstOnlineRole, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildDiplomatApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nResult = errUnknownError;
    CMsGuild* poGuild = NULL;
    CMsGuild* poTargetGuild = NULL;

    C2M_GUILD_DIPLOMAT_APP* pMsg = (C2M_GUILD_DIPLOMAT_APP*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poGuild, errGuildNotFind);

    poTargetGuild = CMsGuildMgr::GetInstance().FindGuildByName(pMsg->szGuildName);
    PROCESS_ERROR_RET_CODE(poTargetGuild, errGuildNotFind);
    PROCESS_ERROR_RET_CODE(poGuild->GetSvrGroup() == poTargetGuild->GetSvrGroup(), errGuildNotFind);

    nResult = poGuild->Diplomat(*pstOnlineRole, pMsg->byOpType, pMsg->byTimeType, poTargetGuild);
    PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGetApplyUnion(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;

    C2M_GUILD_GET_APPLY_UNION_LIST* pMsg = (C2M_GUILD_GET_APPLY_UNION_LIST*)pPacketBuffer;

    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    poMsGuildMember = pMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    //PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_DIPLOMAT, errGuildNoRight);

    nRetCode = CMessageHandler::GetInstance().DoClientGetApplyUnionListAck(pstOnlineRole->dwCliSessionID, pMsGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientReplyUnionApp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;

    C2M_GUILD_REPLY_UNION_APP* pMsg = (C2M_GUILD_REPLY_UNION_APP*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->ReplyDiplomatApp(*pstOnlineRole, pMsg->byOp, pMsg->dwGuildID);
    PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientCancelAlliance(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poGuild = NULL;

    C2M_GUILD_CANCEL_ALLIANCE* pMsg = (C2M_GUILD_CANCEL_ALLIANCE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);

    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR(poGuild);

    nRetCode = poGuild->IsAlliance(pMsg->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNotAlliance);

    nRetCode = CMsGuildMgr::GetInstance().DelAlliance(pstOnlineRole->pRoleData->dwGuildID, pMsg->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddAllianceAppFailed);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientChangeStorageRight(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_CHANGE_STORAGE_RIGHT* pMsg = (C2M_GUILD_CHANGE_STORAGE_RIGHT*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->ChangeStorageRight(*pstOnlineRole, pMsg->byPosition, pMsg->wRight);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildMoveItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_MOVE_ITEM* pMsg = (C2M_GUILD_MOVE_ITEM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    nResult = pMsGuild->MoveItem(*pstOnlineRole, pMsg->bySourceConSlot, pMsg->bySourceSlot, pMsg->byTargetConSlot, pMsg->byTargetSlot);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildMoreHistory(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    C2M_GUILD_MORE_HISTORY* pMsg = (C2M_GUILD_MORE_HISTORY*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = DoClientGuildMoreHistoryAck(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwGuildID);
    PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreHistory);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildMoreChest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = DoClientGuildMoreChestAck(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreChest);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildUpgradeWelfare(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    MS_GUILD_WAR_DATA* pWarData = NULL;
    C2M_GUILD_UPGRADE_WELFARE* pMsg = (C2M_GUILD_UPGRADE_WELFARE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    poMsGuildMember = pMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMsGuildMember->GetPosition() && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_UPGRADE_WELFARE, errGuildNoRight);

    nRetCode = pMsGuild->GetWarCharacter();
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == GUILD_WAR_NONE, errGuildWarTime);

    nResult = pMsGuild->BeginUpgradeWelfareBuffer(pstOnlineRole->pRoleData->dwRoleID, pMsg->byType);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildCancelWelfareUpgrade(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* pMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    C2M_GUILD_CANCEL_WELFARE_UPGRADE* pMsg = (C2M_GUILD_CANCEL_WELFARE_UPGRADE*)pPacketBuffer;

    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);

    poMsGuildMember = pMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMsGuildMember->GetPosition() && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_UPGRADE_WELFARE, errGuildNoRight);
    //LOG_PROCESS_ERROR_RET_CODE(!pMsGuild->IsBuildingDisable(GUILD_B_MARTIAL), errGuildBuildingDead);

    nResult = pMsGuild->CancelUpgradeWelfareBuffer(pMsg->byType);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

//void CMessageHandler::OnClientGuildUpgradeBuilding(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
//{
//    INT32   nRetCode = 0;
//    INT32   nResult = errUnknownError;
//    CMsGuild    *pMsGuild = NULL;
//    CMsGuildMember          *poMsGuildMember = NULL;
//    MS_GUILD_WAR_DATA       *pWarData = NULL;
//    C2M_GUILD_UPGRADE_BUILDING  *pMsg = (C2M_GUILD_UPGRADE_BUILDING *)pPacketBuffer;
//    LOG_PROCESS_ERROR(pMsg);
//    LOG_PROCESS_ERROR(pstOnlineRole);
//    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
//
//    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
//    LOG_PROCESS_ERROR_RET_CODE(pMsGuild, errGuildNotFind);
//
//    poMsGuildMember = pMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
//    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);
//
//    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMsGuildMember->GetPosition() && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
//    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_UPGRADE_BUILDING, errGuildNoRight);
//    pWarData = CMsGuildMgr::GetInstance().FindBattleByDefend(pMsGuild->GetID());
//    LOG_PROCESS_ERROR_RET_CODE(!pWarData, errGuildWarTime);
//    LOG_PROCESS_ERROR_RET_CODE(!pMsGuild->IsBuildingDisable(GUILD_B_MAIN), errGuildBuildingDead);
//
//    nResult = pMsGuild->BeginUpgradeBuilding(pMsg->byType);
//    LOG_PROCESS_ERROR(errSuccess == nResult);
//
//    return ;
//Exit0:
//    if(nResult == errUnknownError)
//        CMessageHandler::GetInstance().KickUser(pstOnlineRole->dwCliSessionID);
//    else
//        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
//    return ;
//}

void CMessageHandler::OnClientGuildCancelBuildingUpgrade(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    C2M_GUILD_CANCEL_BUILDING_UPGRADE* pMsg = (C2M_GUILD_CANCEL_BUILDING_UPGRADE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMsGuildMember = poMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMsGuildMember->GetPosition() && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_UPGRADE_BUILDING, errGuildNoRight);
    //LOG_PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_MAIN), errGuildBuildingDead);

    nResult = poMsGuild->CancelUpgradeBuilding(pMsg->qwNameID, pMsg->byType);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

//void CMessageHandler::OnClientGuildDelBuilding(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
//{
//    INT32   nRetCode = 0;
//    INT32   nResult = errUnknownError;
//    CMsGuild    *poMsGuild = NULL;
//    CMsGuildMember          *poMsGuildMember = NULL;
//    C2M_GUILD_DEL_BUILDING  *pMsg = (C2M_GUILD_DEL_BUILDING *)pPacketBuffer;
//    LOG_PROCESS_ERROR(pMsg);
//    LOG_PROCESS_ERROR(pstOnlineRole);
//    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
//
//    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
//    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);
//
//    poMsGuildMember = poMsGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
//    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);
//
//    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_LEADER == poMsGuildMember->GetPosition() || GUILD_POSITION_SUBLEADER == poMsGuildMember->GetPosition() , errGuildNoRight);
//    //LOG_PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_MAIN), errGuildBuildingDead);
//
//    nRetCode = poMsGuild->IsOneLevelBornBuilding(pMsg->byType);
//    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildCannotDel);
//
//    nResult = poMsGuild->DelBuilding(pstOnlineRole->pRoleData->dwRoleID, pMsg->qwNameID, pMsg->byType);
//    LOG_PROCESS_ERROR(errSuccess == nResult);
//
//    return ;
//Exit0:
//    if(nResult == errUnknownError)
//        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
//    else
//        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
//    return ;
//}

void CMessageHandler::OnClientGuildGetDungeonList(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nRetCode = DoClientGuildGetDungeonListAck(pstOnlineRole, poMsGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildDeclareWar(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    C2M_GUILD_DECLARE_WAR* pMsg = NULL;
    CMsGuild* poDefender = NULL;
    CMsGuild* poIntruder = NULL;
    const GUILD_BUILDING_DATA* pTemplate = NULL;
    char    szAllTVContent[512] = { 0 };
    time_t  tCurrentTime = CManageServer::GetInstance().GetGameTime();
    struct  tm* pTM = NULL;

    CMsGuildMember* poMember = NULL;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pMsg = (C2M_GUILD_DECLARE_WAR*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);


    poDefender = CMsGuildMgr::GetInstance().FindGuildByName(pMsg->szGuildName);
    LOG_PROCESS_ERROR_RET_CODE(poDefender, errGuildNotFind);

    poIntruder = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poIntruder, errGuildNotFind);
    PROCESS_ERROR_RET_CODE(poDefender->GetSvrGroup() == poIntruder->GetSvrGroup(), errGuildNotFind);

    LOG_PROCESS_ERROR_RET_CODE(poDefender != poIntruder, errGuildCannotDeclaredYourself);

    poMember = poIntruder->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(poMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMember->GetPosition() && poMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMember->GetPosition()] & G_POS_RIGHT_DIPLOMAT, errGuildNoRight);

    // todo_tq
    LOG_PROCESS_ERROR_RET_CODE(poIntruder->GetLevel() >= g_GuildConfig.nGuildSpecifyWarLevelLimit, errGuildWarLevelLimit);
    LOG_PROCESS_ERROR_RET_CODE(poDefender->GetLevel() >= g_GuildConfig.nGuildSpecifyWarLevelLimit, errGuildWarLevelLimit);

    PROCESS_ERROR_RET_CODE(poIntruder->GetWarAttackCD() <= tCurrentTime, errGuildAttackCD);
    nRetCode = CMsGuildMgr::GetInstance().IsWarToday(poDefender->GetID());
    PROCESS_ERROR_RET_CODE(nRetCode == FALSE, errGuildBeFightingToday);
    PROCESS_ERROR_RET_CODE(poDefender->GetProtectionTime() <= tCurrentTime, errGuildInProtectionTime);

    LOG_PROCESS_ERROR_RET_CODE(!poIntruder->IsAlliance(poDefender->GetID()), errGuildAllianceCannotBeDeclared);
    LOG_PROCESS_ERROR_RET_CODE(!poDefender->IsAlliance(poIntruder->GetID()), errGuildAllianceCannotBeDeclared);

    pTemplate = GetGuildBuildingData(GUILD_B_MAIN, poDefender->GetLevel());
    LOG_PROCESS_ERROR(pTemplate);
    LOG_PROCESS_ERROR_RET_CODE(poIntruder->GetMoney() >= pTemplate->nParam1, errGuildSubMoneyFailed);
    LOG_PROCESS_ERROR_RET_CODE(poIntruder->GetHerb() >= pTemplate->nParam2, errGuildNoMoreWood);
    LOG_PROCESS_ERROR_RET_CODE(poIntruder->GetWood() >= pTemplate->nParam3, errGuildNoMoreHerb);
    LOG_PROCESS_ERROR_RET_CODE(poIntruder->GetCryStal() >= pTemplate->nParam4, errGuildNoMoreCrystal);
    LOG_PROCESS_ERROR_RET_CODE(poIntruder->GetSilver() >= pTemplate->nParam5, errGuildNoMoreSilver);

    nRetCode = poIntruder->SubMoney(pTemplate->nParam1, GUILD_CHEST_DECLARE_WAR);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubMoneyFailed);
    nRetCode = poIntruder->SubHerb(pTemplate->nParam2, GUILD_CHEST_DECLARE_WAR);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubResFailed);
    nRetCode = poIntruder->SubWood(pTemplate->nParam3, GUILD_CHEST_DECLARE_WAR);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubResFailed);
    nRetCode = poIntruder->SubCrystal(pTemplate->nParam4, GUILD_CHEST_DECLARE_WAR);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubResFailed);
    nRetCode = poIntruder->SubSilver(pTemplate->nParam5, GUILD_CHEST_DECLARE_WAR);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubResFailed);

    // clear protection time
    nRetCode = poIntruder->SetProtectionTime(0);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = poIntruder->SetWarAttackCD(tCurrentTime + g_GuildConfig.nGuildWarAttackCD);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().DeclareWar(poIntruder->GetSvrGroup(), poIntruder->GetID(), poDefender->GetID());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildDeclareFailed);

    pTM = localtime(&tCurrentTime);
    LOG_PROCESS_ERROR(pTM);
    {
        snprintf(szAllTVContent, 512, "<#P0:%s><#P1:%s><#T:%d>",
            poIntruder->GetName(), poDefender->GetName(), g_GuildConfig.dwDeclaredGuildWarToAllID);

        nRetCode = CMSChatMgr::GetInstance().Say(0, cctGroup, cltLighting, szAllTVContent, poIntruder->GetSvrGroup());
        LOG_PROCESS_ERROR(nRetCode);

        //nRetCode = CMSChatMgr::GetInstance().Say(0, cctGroup, cltLighting, szAllTVContent, poIntruder->GetSvrGroup());
        //LOG_PROCESS_ERROR(nRetCode);
    }
    {
        time_t  tTime = tCurrentTime + 86400;
        pTM = localtime(&tTime);
        LOG_PROCESS_ERROR(pTM);

        char mail_content[MAIL_CONTENT_LEN] = { 0 };
        const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildWarDeclaredToAttack);
        LOG_PROCESS_ERROR(mail_data);

        snprintf(mail_content, MAIL_CONTENT_LEN, mail_data->m_szMailContent, poDefender->GetName(), pTM->tm_mon + 1, pTM->tm_mday);
        nRetCode = poIntruder->BroadcastMail(mail_data->m_szMailSubject, mail_content);
        LOG_PROCESS_ERROR(nRetCode);

        mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildWarDeclaredToDefence);
        LOG_PROCESS_ERROR(mail_data);
        memset(mail_content, 0, sizeof(mail_content));
        snprintf(mail_content, MAIL_CONTENT_LEN, mail_data->m_szMailContent, poIntruder->GetName(), pTM->tm_mon + 1, pTM->tm_mday);
        nRetCode = poDefender->BroadcastMail(mail_data->m_szMailSubject, mail_content);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildAchievement(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAchievementAck(pstOnlineRole->dwCliSessionID, poMsGuild);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientGuildActivity(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    INT64       llEndTime = 0;
    CMsGuild* poGuild = NULL;
    CMsGuildMember* poMember = NULL;
    MS_G_B_DATA* pBuilding = NULL;
    const GUILD_ACTIVITY_DATA* pData = NULL;
    INT32       nActivityBeginTime = NULL;
    C2M_GUILD_ACTIVITY* pMsg = (C2M_GUILD_ACTIVITY*)pPacketBuffer;
    CLuaScript* poScript = NULL;

    UINT32      dwCurrentDayTime = CManageServer::GetInstance().GetGameTime() + get_time_zone();
    dwCurrentDayTime /= ONE_DAY_SECOND;
    dwCurrentDayTime *= ONE_DAY_SECOND;
    dwCurrentDayTime -= get_time_zone();

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);

    poGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poGuild, errGuildNoGuild);

    poMember = poGuild->GetMember(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(poMember);
    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMember->GetPosition() && poMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMember->GetPosition()] & G_POS_RIGHT_ACTIVITY, errGuildNoRight);

    pData = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().GetData(GUILD_ACTIVITY_ID(pMsg->nActivityID, poGuild->GetLevel()));
    LOG_PROCESS_ERROR_RET_CODE(pData, errGuildActivityConfigError);

    if (pData->nBaseBuildingType > GUILD_B_NONE)
    {
        LOG_PROCESS_ERROR_RET_CODE(pData->nBaseBuildingType > GUILD_B_NONE && pData->nBaseBuildingType < GUILD_B_FUNATIONAL_NUM, errGuildConfigError);
        pBuilding = poGuild->GetFunBuildingDataByType(pData->nBaseBuildingType);
        LOG_PROCESS_ERROR_RET_CODE(pBuilding, errGuildBaseBuildingFailed);
        LOG_PROCESS_ERROR_RET_CODE(pBuilding->nLevel >= pData->nBaseBuildingLevel, errGuildBaseBuildingFailed);
    }
    if (poGuild->GetVariable(pData->nGuildVariableIndex) != 0)
    {
        nActivityBeginTime = poGuild->GetVariable(pData->nGuildVariableIndex) - pData->nLastTime;
    }
    LOG_PROCESS_ERROR_RET_CODE((UINT32)nActivityBeginTime < dwCurrentDayTime, errGuildAcAlreadyOpen);
    LOG_PROCESS_ERROR_RET_CODE(poGuild->GetMoney() >= pData->nConsumeMoney, errGuildNoMoreGuildMoney);
    LOG_PROCESS_ERROR_RET_CODE(poGuild->GetHerb() >= pData->nConsumeHerb, errGuildNoMoreHerb);
    LOG_PROCESS_ERROR_RET_CODE(poGuild->GetWood() >= pData->nConsumeWood, errGuildNoMoreWood);
    LOG_PROCESS_ERROR_RET_CODE(poGuild->GetCryStal() >= pData->nConsumeCrystal, errGuildNoMoreCrystal);
    LOG_PROCESS_ERROR_RET_CODE(poGuild->GetSilver() >= pData->nConsumeSilver, errGuildNoMoreSilver);

    poGuild->SubMoney(pData->nConsumeMoney, GUILD_HE_TYPE_OPEN_ACTIVITY);
    poGuild->SubHerb(pData->nConsumeHerb);
    poGuild->SubWood(pData->nConsumeWood);
    poGuild->SubCrystal(pData->nConsumeCrystal);
    poGuild->SubSilver(pData->nConsumeSilver);

    llEndTime = CManageServer::GetInstance().GetGameTime() + pData->nLastTime;

    poGuild->SetVariable(pData->nGuildVariableIndex, llEndTime);
    CServerVar::GetInstance().SetVariable(poGuild->GetSvrGroup(), poGuild->GetID(), 1);

    nRetCode = poGuild->AddGuildChestEvent(GUILD_CHEST_ACTIVITY, pstOnlineRole->pRoleData->dwRoleID, pMsg->nActivityID, GUILD_ATTR_MONEY, pData->nConsumeMoney);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = poGuild->AddGuildChestEvent(GUILD_CHEST_ACTIVITY, pstOnlineRole->pRoleData->dwRoleID, pMsg->nActivityID, GUILD_ATTR_HERB, pData->nConsumeHerb);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = poGuild->AddGuildChestEvent(GUILD_CHEST_ACTIVITY, pstOnlineRole->pRoleData->dwRoleID, pMsg->nActivityID, GUILD_ATTR_WOOD, pData->nConsumeWood);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = poGuild->AddGuildChestEvent(GUILD_CHEST_ACTIVITY, pstOnlineRole->pRoleData->dwRoleID, pMsg->nActivityID, GUILD_ATTR_CRYSTAL, pData->nConsumeCrystal);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = poGuild->AddGuildChestEvent(GUILD_CHEST_ACTIVITY, pstOnlineRole->pRoleData->dwRoleID, pMsg->nActivityID, GUILD_ATTR_SILVER, pData->nConsumeSilver);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    { // Call script to send lighting, tvshow, etc.
        char        szScriptFuncName[64] = { 0 };
        snprintf(szScriptFuncName, 64, "OpenActivity_%d", pMsg->nActivityID);

        poScript = CScriptMgr::GetInstance().FindScript(GUILD_ACTIVITY_SCRIPT_NAME);
        LOG_PROCESS_ERROR(poScript);

        poScript->CallFunction(szScriptFuncName, "dddd", poGuild->GetSvrGroup(), poGuild->GetID(), poGuild->GetLevel(), pData->ncltLightingID);
    }

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}
