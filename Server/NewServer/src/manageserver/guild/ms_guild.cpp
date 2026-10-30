#include "stdafx.h"
#include "ms_guild.h"

#include <time.h>
#include "string_ex.h"
#include "time_ex.h"
#include "guild_config.h"

#include "role_data_mgr.h"
#include "onlinerole/online_role_data.h"
#include "onlinerole/online_role_data_mgr.h"
#include "message_handler.h"
#include "manageserver.h"
#include "ms_guild_mgr.h"
#include "mail/mail_mgr.h"
#include "application_mgr.h"
#include "db/prep_stmt.h"
#include "guild_welfare_data.h"
#include "guild_building_data.h"
#include "chat/ms_chat.h"
#include "script_mgr.h"
#include "ranklist/ranklist.h"
#include "mail_template_data.h"
#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"
#include "game_world_config.h"
#include "db/pre_save_list.h"
#include "guild_achievement.h"
#include "guild_achievement_cdtn.h"
#include "ms_guild_achi_mgr.h"
#include "guild_activity.h"

CMsGuild::CMsGuild()
{
    m_dwID = 0;
    memset(m_szGuildName, 0, sizeof(m_szGuildName));
    m_nSvrGroupID = 0;
    m_dwLeaderID = 0;
    memset(m_szCreaterName, 0, sizeof(m_szCreaterName));

    m_dwCreateTime = 0;
    m_nGuildMoney = 0;
    m_nProsperity = 0;

    m_nHerb = 0;
    m_nWood = 0;
    m_nCrystal = 0;
    m_nSilver = 0;
    m_dwCalculateResourceTime = 0;

    m_nState = GUILD_STATE_NONE;
    m_dwRoutineConsumeTime = 0;

    memset(m_szAnnounce, 0, sizeof(m_szAnnounce));
    memset(m_szNotice, 0, sizeof(m_szNotice));
    memset(m_wStorageRight, 0, sizeof(m_wStorageRight));

    m_nAllianceNum = 0;
    m_nEnemyNum = 0;
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));

    m_dwAttackCD = 0;
    m_dwAimID = 0;
    m_dwIntruderID = 0;
    m_dwProtectionTime = 0;
    m_dwRevengeGuildID = 0;

    m_nMatchWarCount = 0;
    m_dwMatchWarTime = 0;
    m_dwMatchingGuild = 0;

    m_nHistoryNum = 0;
    m_nChestHistoryNum = 0;
    m_History.Clear();
    m_ChestHistory.Clear();
    ClearNode(&m_NodeSortByLvl);
    ClearNode(&m_LadderNode);

    m_memberList.clear();
    m_memberListCursor = m_memberList.end();

    memset(m_arrWelfareAuraLvl, 0, sizeof(m_arrWelfareAuraLvl));
    memset(m_paWelfareUpgradeData, 0, sizeof(m_paWelfareUpgradeData));
    m_nUpgradeWelfareAuraCount = 0;

    memset(m_FunBuilding, 0, sizeof(m_FunBuilding));
    m_BuildingList.clear();
    m_BCursor = m_BuildingList.end();
    m_nUpgradeBuildingNum = 0;
    memset(m_pUpgradeBuilding, 0, sizeof(m_pUpgradeBuilding));

    m_listDungeon.clear();
    m_DungeonListCursor = m_listDungeon.end();
    m_nDungeonUpdateTime = 0;

	memset(m_dwSabacEnrollTime, 0, sizeof(m_dwSabacEnrollTime));
	m_bOccupiedSabac = FALSE;

    memset(m_Achievement, 0, sizeof(m_Achievement));

    m_byWarCharacter = GUILD_WAR_NONE;
}

CMsGuild::~CMsGuild()
{
    m_dwID = 0;
    memset(m_szGuildName, 0, sizeof(m_szGuildName));
    m_nSvrGroupID = 0;
    m_dwLeaderID = 0;
    memset(m_szCreaterName, 0, sizeof(m_szCreaterName));

    m_dwCreateTime = 0;
    m_nGuildMoney = 0;
    m_nProsperity = 0;

    m_nHerb = 0;
    m_nWood = 0;
    m_nCrystal = 0;
    m_nSilver = 0;
    m_dwCalculateResourceTime = 0;

    m_nState = GUILD_STATE_NONE;
    m_dwRoutineConsumeTime = 0;

    memset(m_szAnnounce, 0, sizeof(m_szAnnounce));
    memset(m_szNotice, 0, sizeof(m_szNotice));
    memset(m_wStorageRight, 0, sizeof(m_wStorageRight));

    m_nAllianceNum = 0;
    m_nEnemyNum = 0;
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));

    m_dwAttackCD = 0;
    m_dwAimID = 0;
    m_dwIntruderID = 0;
    m_dwProtectionTime = 0;
    m_dwRevengeGuildID = 0;

    m_nMatchWarCount = 0;
    m_dwMatchWarTime = 0;
    m_dwMatchingGuild = 0;

    m_nHistoryNum = 0;
    m_nChestHistoryNum = 0;
    m_History.Clear();
    m_ChestHistory.Clear();
    ClearNode(&m_NodeSortByLvl);
    ClearNode(&m_LadderNode);

    m_memberList.clear();
    m_memberListCursor = m_memberList.end();

    memset(m_arrWelfareAuraLvl, 0, sizeof(m_arrWelfareAuraLvl));
    memset(m_paWelfareUpgradeData, 0, sizeof(m_paWelfareUpgradeData));
    m_nUpgradeWelfareAuraCount = 0;

    memset(m_FunBuilding, 0, sizeof(m_FunBuilding));
    m_BuildingList.clear();
    m_BCursor = m_BuildingList.end();
    m_nUpgradeBuildingNum = 0;
    memset(m_pUpgradeBuilding, 0, sizeof(m_pUpgradeBuilding));

    m_listDungeon.clear();
    m_DungeonListCursor = m_listDungeon.end();
    m_nDungeonUpdateTime = 0;

    m_bOccupiedSabac = FALSE;

    m_byWarCharacter = GUILD_WAR_NONE;
}

BOOL CMsGuild::Load(GUILD_DB& db_data)
{
    INT32   nRetCode = 0;
    INT32   nLength = 0;

    //init
    LOG_PROCESS_ERROR(Init());

    m_dwID = db_data.dwGuildID;
    m_nSvrGroupID = db_data.bySvrGroup;
    m_dwLeaderID = db_data.dwLeaderID;
    strxcpy(m_szCreaterName, db_data.szCreaterName, ROLE_NAME_LEN);
    m_dwCreateTime = db_data.dwCreateTime;
    strxcpy(m_szGuildName, db_data.szGuildName, GUILD_NAME_LEN);
    strxcpy(m_szAnnounce, db_data.szAnnounce, GUILD_ANNOUNCE_LEN);
    strxcpy(m_szNotice, db_data.szNotice, GUILD_NOTICE_LEN);

    nRetCode = LoadGuild(db_data.BlobBuffer);
    LOG_PROCESS_ERROR(nRetCode);

    //////////////////////////////////////////////////////////////////////////
    // todo_tq: temporarily commented out, Redmine 23026
    // todo_tq: temporarily placed here. Allan Tao [17.4.2014]
    //_CalculateOfflineData();
    //////////////////////////////////////////////////////////////////////////

    return TRUE;
Exit0:
    return FALSE;
}

void CMsGuild::LoadMember(CMsGuildMember &data)
{
    m_memberList.push_back(data);
}

INT32 CalcOfflineData(UINT32 dwCurHour, UINT32 dwDisableHour, INT32 nType, INT32 nLevel, UINT32 dwLastCalcHour, UINT32 dwWashoutHour)
{
    const GUILD_BUILDING_DATA     *pTemplate = NULL;
    const GUILD_BUILDING_DATA     *pNextTemplate = NULL;
    const GUILD_BUILDING_DATA       *pDisableTemplate = NULL;

    pTemplate = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR(pTemplate);
    pNextTemplate = GetGuildBuildingData(nType, nLevel + 1);
    LOG_PROCESS_ERROR(pNextTemplate);
    pDisableTemplate = GetGuildBuildingData(nType, 0);
    LOG_PROCESS_ERROR(pDisableTemplate);

    if (dwCurHour > dwLastCalcHour)
        dwCurHour -= dwLastCalcHour;
    else
        return 0;

    if (dwDisableHour > dwLastCalcHour)
        dwDisableHour -= dwLastCalcHour;
    else
        dwDisableHour = 0;

    if (dwWashoutHour > dwLastCalcHour)
        dwWashoutHour -= dwLastCalcHour;
    else
        dwWashoutHour = 0;

    if (dwWashoutHour <= dwDisableHour)
    {
        return (dwCurHour - dwDisableHour) * (pNextTemplate->nParam1 - pTemplate->nParam1) + dwDisableHour * pDisableTemplate->nParam1;
    }
    else
    {
        return (dwCurHour - dwWashoutHour) * (pNextTemplate->nParam1 - pTemplate->nParam1) /*+ (dwWashoutHour - dwDisableHour) * pTemplate->nParam1 */ + dwDisableHour * pDisableTemplate->nParam1;
    }

Exit0:
    return 0;
}

BOOL CMsGuild::_CalculateOfflineData()
{
    const GUILD_BUILDING_DATA     *pTemplate = NULL;
    const GUILD_BUILDING_DATA     *pNextTemplate = NULL;
    INT32           nRetCode = 0;
    UINT32          dwLastCalculateResourceHour = m_dwCalculateResourceTime;
    INT32           nAddValue = 0;
    UINT64          qwNameID = 0;
    MS_G_B_DATA     *pBuildingData = NULL;
    INT32           nLevel = 0;
    UINT32          dwCurTime = CManageServer::GetInstance().GetGameTime();
    UINT32          dwCurrentHour = (dwCurTime + m_dwID) / 3600;
    UINT32          dwDisableHour = 0;
    UINT32          dwUpgradeHour = 0;

    INT32           nResType[] = { GUILD_B_HERB , GUILD_B_WOOD, GUILD_B_CRYSTAL, GUILD_B_SILVER };
    
    for (INT32 nType = 0; nType < sizeof(nResType) / sizeof(INT32); ++nType)
    {
        if (dwCurTime < m_FunBuilding[nType].dwReviveTime)
            continue;

        dwCurrentHour = (dwCurTime + m_dwID) / 3600;
        if (dwCurrentHour <= dwLastCalculateResourceHour)
            continue;
        dwCurrentHour -= dwLastCalculateResourceHour;

        dwDisableHour = m_FunBuilding[nType].dwReviveTime / 3600;
        if (dwDisableHour <= dwLastCalculateResourceHour)
            dwDisableHour = 0;
        else
            dwDisableHour -= dwLastCalculateResourceHour;

        pTemplate = GetGuildBuildingData(m_FunBuilding[nType].nType, nLevel);
        LOG_PROCESS_ERROR(pTemplate);
        pNextTemplate = GetGuildBuildingData(m_FunBuilding[nType].nType, 0);
        LOG_PROCESS_ERROR(pNextTemplate);

        nAddValue = (dwCurrentHour - dwDisableHour) * pTemplate->nParam1 + dwDisableHour * pNextTemplate->nParam1;

        if (nType == GUILD_B_HERB)
            nRetCode = AddHerb(nAddValue, FALSE);
        else if (nType == GUILD_B_WOOD)
            nRetCode = AddWood(nAddValue, FALSE);
        else if (nType == GUILD_B_CRYSTAL)
            nRetCode = AddCrystal(nAddValue, FALSE);
        else if (nType == GUILD_B_SILVER)
            nRetCode = AddSilver(nAddValue, FALSE);
        LOG_CHECK_ERROR(nRetCode);
    }

    dwCurrentHour = (dwCurTime + m_dwID) / 3600;
    for (INT32 i = 0; i < m_nUpgradeBuildingNum; ++i)
    {
        if (m_pUpgradeBuilding[i])
        {
            if (m_pUpgradeBuilding[i]->nType != GUILD_B_HERB
                && m_pUpgradeBuilding[i]->nType != GUILD_B_WOOD
                && m_pUpgradeBuilding[i]->nType != GUILD_B_CRYSTAL
                && m_pUpgradeBuilding[i]->nType != GUILD_B_SILVER)
                continue;

            if (dwCurrentHour <= dwLastCalculateResourceHour)
                continue;

            dwUpgradeHour = (m_pUpgradeBuilding[i]->dwWashoutTime + m_dwID) / 3600;
            if (dwCurrentHour <= dwUpgradeHour)
                continue;

            pBuildingData = GetBuildingDataByNpcID(qwNameID);
            if (pBuildingData)
                dwDisableHour = (pBuildingData->dwReviveTime + m_dwID) / 3600;
            else
                dwDisableHour = 0;

            nAddValue = CalcOfflineData(dwCurrentHour, dwDisableHour, m_pUpgradeBuilding[i]->nType,
                nLevel, dwLastCalculateResourceHour, dwUpgradeHour);

            if (m_pUpgradeBuilding[i]->nType == GUILD_B_HERB)
                nRetCode = AddHerb(nAddValue, FALSE);
            else if (m_pUpgradeBuilding[i]->nType == GUILD_B_WOOD)
                nRetCode = AddWood(nAddValue, FALSE);
            else if (m_pUpgradeBuilding[i]->nType == GUILD_B_CRYSTAL)
                nRetCode = AddCrystal(nAddValue, FALSE);
            else if (m_pUpgradeBuilding[i]->nType == GUILD_B_SILVER)
                nRetCode = AddSilver(nAddValue, FALSE);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::Init()
{
    m_dwID = 0;
    memset(m_szGuildName, 0, sizeof(m_szGuildName));
    m_nSvrGroupID = 0;
    m_dwLeaderID = 0;
    memset(m_szCreaterName, 0, sizeof(m_szCreaterName));

    m_dwCreateTime = CManageServer::GetInstance().GetGameTime();
    m_nGuildMoney = g_GuildConfig.nInitGuildMoney;
    m_nProsperity = 0;

    m_nHerb = 0;
    m_nWood = 0;
    m_nCrystal = 0;
    m_nSilver = 0;
    m_dwCalculateResourceTime = 0;

    m_nState = GUILD_STATE_NONE;
    m_dwRoutineConsumeTime = 0;

    memset(m_szAnnounce, 0, sizeof(m_szAnnounce));
    memset(m_szNotice, 0, sizeof(m_szNotice));
    memset(m_wStorageRight, 0, sizeof(m_wStorageRight));

    m_nAllianceNum = 0;
    m_nEnemyNum = 0;
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));

    m_dwAttackCD = 0;
    m_dwAimID = 0;
    m_dwIntruderID = 0;
    m_dwProtectionTime = 0;
    m_dwRevengeGuildID = 0;

    m_nMatchWarCount = 0;
    m_dwMatchWarTime = 0;
    m_dwMatchingGuild = 0;

    m_nHistoryNum = 0;
    m_nChestHistoryNum = 0;
    m_History.Clear();
    m_ChestHistory.Clear();
    ClearNode(&m_NodeSortByLvl);
    ClearNode(&m_LadderNode);

    m_storage.Init(this);

    m_memberList.clear();
    m_memberListCursor = m_memberList.end();

    memset(m_arrWelfareAuraLvl, 0, sizeof(m_arrWelfareAuraLvl));
    memset(m_paWelfareUpgradeData, 0, sizeof(m_paWelfareUpgradeData));
    m_nUpgradeWelfareAuraCount = 0;

    memset(m_FunBuilding, 0, sizeof(m_FunBuilding));
    m_BuildingList.clear();
    m_BCursor = m_BuildingList.end();
    m_nUpgradeBuildingNum = 0;
    memset(m_pUpgradeBuilding, 0, sizeof(m_pUpgradeBuilding));

    m_listDungeon.clear();
    m_DungeonListCursor = m_listDungeon.end();

    m_nDungeonUpdateTime = CManageServer::GetInstance().GetGameTime() + get_time_zone();
    m_nDungeonUpdateTime /= ONE_DAY_SECOND;
    m_nDungeonUpdateTime *= ONE_DAY_SECOND;
    m_nDungeonUpdateTime -= get_time_zone();
    m_nDungeonUpdateTime += g_GuildConfig.nGuildDungeonUpdateTime;

	memset(m_dwSabacEnrollTime, 0, sizeof(m_dwSabacEnrollTime));
	m_bOccupiedSabac = FALSE;

    memset(m_Achievement, 0, sizeof(m_Achievement));

    m_byWarCharacter = GUILD_WAR_NONE;
    m_IntruderSet.clear();

    return TRUE;
}

BOOL CMsGuild::UnInit()
{
    m_dwID = 0;
    memset(m_szGuildName, 0, sizeof(m_szGuildName));
    m_nSvrGroupID = 0;
    m_dwLeaderID = 0;
    memset(m_szCreaterName, 0, sizeof(m_szCreaterName));

    m_dwCreateTime = CManageServer::GetInstance().GetGameTime();
    m_nGuildMoney = g_GuildConfig.nInitGuildMoney;
    m_nProsperity = 0;

    m_nHerb = 0;
    m_nWood = 0;
    m_nCrystal = 0;
    m_nSilver = 0;
    m_dwCalculateResourceTime = 0;

    m_nState = GUILD_STATE_NONE;
    m_dwRoutineConsumeTime = 0;

    memset(m_szAnnounce, 0, sizeof(m_szAnnounce));
    memset(m_szNotice, 0, sizeof(m_szNotice));
    memset(m_wStorageRight, 0, sizeof(m_wStorageRight));

    m_nAllianceNum = 0;
    m_nEnemyNum = 0;
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));

    m_dwAttackCD = 0;
    m_dwAimID = 0;
    m_dwIntruderID = 0;
    m_dwProtectionTime = 0;
    m_dwRevengeGuildID = 0;

    m_nMatchWarCount = 0;
    m_dwMatchWarTime = 0;
    m_dwMatchingGuild = 0;

    m_nHistoryNum = 0;
    m_nChestHistoryNum = 0;
    m_History.Clear();
    m_ChestHistory.Clear();
    ClearNode(&m_NodeSortByLvl);
    ClearNode(&m_LadderNode);

    m_storage.UnInit();

    m_memberList.clear();
    m_memberListCursor = m_memberList.end();

    memset(m_arrWelfareAuraLvl, 0, sizeof(m_arrWelfareAuraLvl));
    memset(m_paWelfareUpgradeData, 0, sizeof(m_paWelfareUpgradeData));
    m_nUpgradeWelfareAuraCount = 0;

    m_nUpgradeBuildingNum = 0;
    memset(m_pUpgradeBuilding, 0, sizeof(m_pUpgradeBuilding));

    m_listDungeon.clear();
    m_DungeonListCursor = m_listDungeon.end();
    m_nDungeonUpdateTime = CManageServer::GetInstance().GetGameTime() / ONE_DAY_SECOND;
    m_nDungeonUpdateTime = m_nDungeonUpdateTime * ONE_DAY_SECOND + g_GuildConfig.nGuildDungeonUpdateTime;

    m_bOccupiedSabac = FALSE;

    return TRUE;
}

BOOL CMsGuild::Create(UINT32 dwID, INT32 nSvrGroup, UINT32 dwRoleID, const char *pcszRoleName, const char *pcszGuildName, const char *pcszAnnounce)
{
    INT32       nRetCode = 0;
    INT32       nLuaRet = 0;
    CLuaScript* poScript = NULL;

    // clear
    Init();

    // init
    m_dwID = dwID;
    m_nSvrGroupID = nSvrGroup;
    m_dwLeaderID = dwRoleID;
    strxcpy(m_szCreaterName, pcszRoleName, ROLE_NAME_LEN);
    strxcpy(m_szGuildName, pcszGuildName, GUILD_NAME_LEN);
    strxcpy(m_szAnnounce, pcszAnnounce, GUILD_ANNOUNCE_LEN);

    m_dwCreateTime = CManageServer::GetInstance().GetGameTime();
    m_nGuildMoney = g_GuildConfig.nInitGuildMoney;

    m_nWood = g_GuildConfig.nWood;
    m_nHerb = g_GuildConfig.nHerb;
    m_nCrystal = g_GuildConfig.nCrystal;
    m_nSilver = g_GuildConfig.nSilver;
    m_dwCalculateResourceTime = ((UINT64)m_dwID + CManageServer::GetInstance().GetGameTime()) / 3600;

    memset(m_FunBuilding, 0, sizeof(m_FunBuilding));
    m_BuildingList.clear();
    m_nUpgradeBuildingNum = 0;
    memset(m_pUpgradeBuilding, 0, sizeof(m_pUpgradeBuilding));

    // init building
    poScript = CScriptMgr::GetInstance().FindScript(GUILD_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("InitGuildBuilding", "d:d", m_dwID, &nLuaRet);
    LOG_PROCESS_ERROR(nRetCode);
    LOG_PROCESS_ERROR(nLuaRet == errSuccess);

    memset(m_arrWelfareAuraLvl, 0, sizeof(m_arrWelfareAuraLvl));
    memset(m_paWelfareUpgradeData, 0, sizeof(m_paWelfareUpgradeData));

    memcpy(m_wStorageRight, g_GuildConfig.nInitGuildStorageRight, sizeof(UINT16)*GUILD_POSITION_COUNT);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::Destory()
{
    INT32   nRetCode = 0;
    INT32   nResult = errGuildFailed;
    MS_ROLE_DATA* pRoleData = NULL;
    
    MEMBER_LIST::iterator member_iter;
    MEMBER_LIST::iterator mem_ber_end_iter;  
    UINT32  dwCD = g_GuildConfig.dwReJoinGuildCD + CManageServer::GetInstance().GetGameTime();

    APP_NODE    *pOldNode = NULL;
    APP_NODE    *pNode = NULL;
    DIPLOMAT_APP_NODE   *pOldDipNode = NULL;
    DIPLOMAT_APP_NODE   *pDipNode = NULL;
    EVENT_DATA  *pOldEventData = NULL;
    EVENT_DATA  *pEventData = NULL;
    STORAGE_EVENT_DATA  *pOldStorageEventData = NULL;
    STORAGE_EVENT_DATA  *pStorageEventData = NULL;

    GUILD_STORAGE_ITEM_DATA    *pItemInfo = NULL;
    INT32       nItemNum = 0;

    // delete role join guild application
    pNode = GetFirstRoleJoinGuildApp();
    while (pNode)
    {
        pOldNode = pNode;
        pNode = GetNextRoleJoinGuildApp(pOldNode);
        LOG_CHECK_ERROR(CApplicationMgr::GetInstance().RemoveApplication(pOldNode));
        if (pOldNode)
            LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteGuildApp(pOldNode));
    }

    // delete diplomat application
    pDipNode = GetFirstAllianceApp();
    while (pDipNode)
    {
        pOldDipNode = pDipNode;
        pDipNode = GetNextAllianceApp(pOldDipNode);
        LOG_CHECK_ERROR(CApplicationMgr::GetInstance().RemoveApplication(pOldDipNode));
        if (pOldDipNode)
            LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteDiplomatApp(pOldDipNode));
    }

    // delete diplomat relation


    // release history event data
    pEventData = GetFirstGuildEventData();
    while (pEventData)
    {
        pOldEventData = pEventData;
        pEventData = GetNextGuildEventData(pOldEventData);
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteEventData(pOldEventData));
    }
    m_History.Clear();

    pEventData = FirstChestEvent();
    while (pEventData)
    {
        pOldEventData = pEventData;
        pEventData = NextChestEvent(pOldEventData);
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteEventData(pOldEventData));
    }
    m_ChestHistory.Clear();

    for (INT32 i = 0; i < GetLevel(); i++)
    {
        pItemInfo = GetStorage().GetFirstConSlotItem(i);
        while (pItemInfo)
        {
            ItemInfo    tempItemInfo;
            tempItemInfo.Load(pItemInfo->szItemBuffer);

            const ITEM_DATA* itemdata = NULL;
            if (tempItemInfo.m_nTemplateID < 99)
                continue;

            itemdata = GetItemData(tempItemInfo.m_nTemplateID);
            LOG_CHECK_ERROR(itemdata);
            if (itemdata == NULL)
                nItemNum = 1;
            else
                nItemNum = itemdata->m_byDurType == DURTYPE_STACK ? tempItemInfo.m_nCurDur : 1;

            if (itemdata && itemdata->m_byLog)
            {
                nRetCode = CMessageHandler::GetInstance().DoDLLogItem(
                    GetID(),
                    GetSvrGroup(),
                    tempItemInfo.m_llGuid,
                    tempItemInfo.m_nTemplateID,
                    nItemNum,
                    ITEMLOG_GUILD_DESTORY,
                    ITEMACTION_DEL,
                    0);
                LOG_CHECK_ERROR(nRetCode);
            }

            if (itemdata && itemdata->m_byStat)
            {
                nRetCode = CMessageHandler::GetInstance().DoDLStatItem(GetSvrGroup(), tempItemInfo.m_nTemplateID, nItemNum, 0);
                LOG_CHECK_ERROR(nRetCode);
            }

            pItemInfo = GetStorage().GetNextConSlotItem();
        }
    }

    // storage history
    pStorageEventData = m_storage.GetFirstStorageEventData();
    while (pStorageEventData)
    {
        pOldStorageEventData = pStorageEventData;
        pStorageEventData = m_storage.GetNextStorageEventData(pOldStorageEventData);
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteStorageEventData(pOldStorageEventData));
    }
    m_storage.UnInit();

    // deal and update member memory data
    mem_ber_end_iter = m_memberList.end();
    for (member_iter = m_memberList.begin(); member_iter != mem_ber_end_iter; ++member_iter)
    {
        nResult = ForcedSalariedWithMail(member_iter->GetRoleID());
        LOG_CHECK_ERROR(errSuccess == nResult);

        MS_ROLE_DATA* pTempRoleData = CRoleDataMgr::GetInstance().FindRole(member_iter->GetRoleID());
        if (pTempRoleData)
        {
            pTempRoleData->dwGuildID = 0;
            pTempRoleData->dwJoinGuildCD = dwCD;
        }
    }

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteGMemberByGuildID, NULL, NULL, "ud", GetID());
    LOG_CHECK_ERROR(nRetCode);

    // not true delete
    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteGuild, NULL, NULL, "ud", GetID());
    LOG_CHECK_ERROR(nRetCode);

    // delete all member
    m_memberList.clear();

    return TRUE;
}

INT32 CMsGuild::GetLevel()
{
    return m_FunBuilding[GUILD_B_MAIN].nLevel;
}

void CMsGuild::SetNotice(const char *pcszNotice)
{
    strxcpy(m_szNotice, pcszNotice, GUILD_NOTICE_LEN);
}

BOOL CMsGuild::MainLoop(UINT32 dwCurrentTime)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGuildFailed;
    INT32       nIncrementValue = 0;

    UINT32      dwTick = m_dwID + dwCurrentTime;  // Guild timer runs every 5 seconds with staggered timing. Allan Tao [8.5.2013]
    if ( dwTick % 5 == 0 )
    {
        // 60 sec timer
        if (dwTick % 60 == 0)
        {
            nRetCode = _DiplomatWashOut( dwCurrentTime, 60 ) ;
            LOG_CHECK_ERROR(nRetCode);
        }

        // The 2015.12.6 version does not need this feature, Redmine 23026
        //if (dwTick % g_GuildConfig.nAddResourceInterval == 0 && m_dwCalculateResourceTime < dwTick / 3600)
        //{
        //    nIncrementValue = GetResourceIncrement(GUILD_B_WOOD);
        //    nRetCode = AddWood(nIncrementValue);
        //    LOG_CHECK_ERROR(nRetCode);

        //    nIncrementValue = GetResourceIncrement(GUILD_B_HERB);
        //    nRetCode = AddHerb(nIncrementValue);
        //    LOG_CHECK_ERROR(nRetCode);

        //    nIncrementValue = GetResourceIncrement(GUILD_B_CRYSTAL);
        //    nRetCode = AddCrystal(nIncrementValue);
        //    LOG_CHECK_ERROR(nRetCode);

        //    nIncrementValue = GetResourceIncrement(GUILD_B_SILVER);
        //    nRetCode = AddSilver(nIncrementValue);
        //    LOG_CHECK_ERROR(nRetCode);

        //    m_dwCalculateResourceTime = dwTick / 3600;
        //}

        if (dwTick % 1800 == 0)
        {
            UINT32  dwDayTime = dwCurrentTime + get_time_zone();
            dwDayTime /= ONE_DAY_SECOND;
            dwDayTime *= ONE_DAY_SECOND;
            dwDayTime -= get_time_zone();

            time_t      tCurrentTime = dwCurrentTime;
            struct tm   *pTm = localtime(&tCurrentTime);
            if (pTm)
            {
                if (g_GuildConfig.nRoutineConsumeTime == pTm->tm_hour && m_dwRoutineConsumeTime != dwDayTime)
                {
                    INT32 nTotalRoutineConsume = 0;
                    
                    //////////////////////////////////////////////////////////////////////////
                    const GUILD_BUILDING_DATA *pBuildingTemplate = NULL;

                    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
                    {
                        pBuildingTemplate = GetGuildBuildingData(m_FunBuilding[i].nType, m_FunBuilding[i].nLevel);
                        LOG_CHECK_ERROR(pBuildingTemplate);
                        if (pBuildingTemplate)
                            nTotalRoutineConsume += pBuildingTemplate->nRoutineConsumption;
                    }

                    G_BUILDING_MAP::iterator end_iter = m_BuildingList.end();
                    for (G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != end_iter; ++iter)
                    {
                        pBuildingTemplate = GetGuildBuildingData(iter->second.nType, iter->second.nLevel);
                        LOG_CHECK_ERROR(pBuildingTemplate);
                        if (pBuildingTemplate)
                            nTotalRoutineConsume += pBuildingTemplate->nRoutineConsumption;
                    }

                    if (GetMoney() >= nTotalRoutineConsume)
                    {
                        SubMoney(nTotalRoutineConsume, GUILD_CHEST_ROUTINE_CONSUME);
                        m_nState &= (~GUILD_STATE_STOP_WELFARE);
                    }
                    else
                    {
                        SubMoney(GetMoney(), GUILD_CHEST_ROUTINE_CONSUME);
                        m_nState |= GUILD_STATE_STOP_WELFARE;

                        //nRetCode = BroadcastMail(mtiGuildNoMoneyToMaintenance);
                        //LOG_CHECK_ERROR(nRetCode);
                    }
                    nRetCode = CMessageHandler::GetInstance().DoGSGuildUpdateState(m_dwID, m_nState);
                    LOG_CHECK_ERROR(nRetCode);

                    m_dwRoutineConsumeTime = dwDayTime;
                }
            }
        }

		//if ((dwCurrentTime%86400 >= GUILD_UPDATE_RANKLIST_STARTTIME) && (dwCurrentTime%86400 <= GUILD_UPDATE_RANKLIST_ENDIME))				// Refresh this guild’s ranking data within two minutes starting at 4 a.m. daily
		//{
		//	if (m_dwID%1023 == dwCurrentTime%1023)
		//	{
		//		nRetCode = CRankListMgr::GetInstance().UpdateGuildRank(this);
		//		LOG_CHECK_ERROR(nRetCode);
		//	}
		//}
    }

	if (m_dwID % 3600 == dwCurrentTime % 3600)
	{
		nRetCode = CRankListMgr::GetInstance().UpdateGuildRank(this);
		LOG_CHECK_ERROR(nRetCode);
	}

    return TRUE;
}

#include <map>

INT32 CMsGuild::GetMaxMemberNum()
{
    INT32       nNum = 0;
    INT32       nLevel = 0;
    const GUILD_BUILDING_DATA *pBuildingTemplate = NULL;
    const GUILD_BUILDING_DATA *pZeroTemplate = NULL;

    pZeroTemplate = GetGuildBuildingData(GUILD_B_HOUSE, 0);
    LOG_PROCESS_ERROR(pZeroTemplate);

    pBuildingTemplate = GetGuildBuildingData(GUILD_B_HOUSE, m_FunBuilding[GUILD_B_HOUSE].nLevel);
    LOG_PROCESS_ERROR(pBuildingTemplate);

    if (m_FunBuilding[GUILD_B_HOUSE].dwReviveTime > 0)
        return pZeroTemplate->nParam1;
    else
        return pBuildingTemplate->nParam1;

Exit0:
    return 0;
}

BOOL CMsGuild::IsMember(UINT32 dwRoleID)
{
    LOG_PROCESS_ERROR(GetMember(dwRoleID));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::CanGuildChat(UINT32 dwRoleID)
{
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(dwRoleID);
    LOG_PROCESS_ERROR(poMsGuildMember);

    PROCESS_ERROR(!(poMsGuildMember->GetFlag() & GUILD_FLAG_IS_CHAT_FORBIDDEN));

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMsGuild::RoleDeliverApplication(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;

    LOG_PROCESS_ERROR_RET_CODE(m_nSvrGroupID == onlineRoleData.pRoleData->byGroup, errGuildNotFind);
    PROCESS_ERROR_RET_CODE( 0 == onlineRoleData.pRoleData->dwGuildID, errGuildAlreadyJoin);
    PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->dwJoinGuildCD < CManageServer::GetInstance().GetGameTime(), errGuildJoinCD);
    PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->byLevel >= g_GuildConfig.nCanJoinGuildLevel, errGuildJoinGuildLevelLimit);
    PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->byApplicationCount < ROLE_MAX_APPLICATION, errGuildAppFull);
    PROCESS_ERROR_RET_CODE( !_IsInGuildAppList(onlineRoleData.pRoleData->dwRoleID), errGuildAlreadyApply);

    // add
    ++onlineRoleData.pRoleData->byApplicationCount;

    nRetCode = _AddRoleJoinGuildApp(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildApplyGuildFailed);

    // notify
    nRetCode = CMessageHandler::GetInstance().DoClientGuildNotifyNtf(this, GUILD_NOTIFY_ROLE_APP, G_POS_RIGHT_INVITE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoClientApplyToJoinGuildAck(onlineRoleData.dwCliSessionID, m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::RefuseRoleApply(UINT32 dwRoleID, MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    //CMsGuild::MS_GUILD_APPLICANT_LIST::iterator   *pAppIter = NULL;
    MS_ROLE_DATA    *pRoleData = NULL;
    MS_ONLINE_ROLE_DATA     *pOnlineRole = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_RECRUIT, errGuildNoRight);
    LOG_PROCESS_ERROR_RET_CODE(_IsInGuildAppList(dwRoleID), errGuildAppNotExist);

    pRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    if (pRoleData)      // Character deletion, etc.
    {
        --pRoleData->byApplicationCount;
    }

    nRetCode = _DelRoleJoinGuildApp(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildDeleteGuildAppFailed);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
    if (pOnlineRole)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientDelGuildApp(pOnlineRole->dwCliSessionID, GetID());
        LOG_PROCESS_ERROR(nRetCode);
    }

    // send mail
    if (pRoleData)
    {
        char mail_content[MAIL_CONTENT_LEN] = {0};
        const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildApplyFailed);
        LOG_PROCESS_ERROR(mail_data);

        snprintf(mail_content, MAIL_CONTENT_LEN, mail_data->m_szMailContent, m_szGuildName);

        nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, mail_data->m_szMailSubject, mail_content);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientRevertGuildApplyAck(onlineRoleData.dwCliSessionID, dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    if ( errGuildAppNotExist == nResult )
        CMessageHandler::GetInstance().DoClientRevertGuildApplyAck(onlineRoleData.dwCliSessionID, dwRoleID);
    return nResult;
}

INT32 CMsGuild::AgreeRoleApply(UINT32 dwInviterID, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ROLE_DATA    *pRoleData = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRoleData = NULL;
    MS_ONLINE_ROLE_DATA *pInviterRoleData = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;
    MS_GUILD_WAR_DATA   *pWarData = NULL;

    PROCESS_ERROR_RET_CODE(_IsInGuildAppList(dwRoleID), errGuildAppNotExist);
    poMsGuildMember = GetMember(dwInviterID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_RECRUIT, errGuildNoRight);

    pRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, errGuildRoleNotExist);    // Character deletion, etc.
    PROCESS_ERROR_RET_CODE(pRoleData->dwJoinGuildCD < CManageServer::GetInstance().GetGameTime(), errGuildJoinCD);
    PROCESS_ERROR_RET_CODE(pRoleData->byLevel >= g_GuildConfig.nCanJoinGuildLevel, errGuildJoinGuildLevelLimit);
    PROCESS_ERROR_RET_CODE(0 == pRoleData->dwGuildID, errGuildAlreadyJoin);
    PROCESS_ERROR_RET_CODE(m_memberList.size() < GetMaxMemberNum(), errGuildMemberCountFull);

    // remove applicantion
    nRetCode = _DelRoleJoinGuildApp(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildDeleteGuildAppFailed);
    --pRoleData->byApplicationCount;

    // add member Allan Tao[8.4.2013] 
    nRetCode = AddMember(pRoleData, GUILD_POSITION_CIVILIAN);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddMemberFailed);

    pInviterRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwInviterID);
    if (pInviterRoleData)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientRevertGuildApplyAck(pInviterRoleData->dwCliSessionID, dwRoleID);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::InviteToJoinGuild(MS_ONLINE_ROLE_DATA &onlineRoleData, const char *pcszRoleName)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember      *poMsGuildMember = NULL;
    MS_ONLINE_ROLE_DATA *pTargetOnlineRole = NULL;
    MS_ROLE_DATA        *pTargetRoleData = NULL;

    pTargetOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByName(pcszRoleName);
    if (!pTargetOnlineRole)
    {
        pTargetRoleData = CRoleDataMgr::GetInstance().FindRoleByName(pcszRoleName);
        if (pTargetRoleData)
            PROCESS_ERROR_RET_CODE(pTargetOnlineRole, errGuildTargetNotOnline);
        else
            PROCESS_ERROR_RET_CODE(pTargetRoleData, errGuildRoleNotExist);
    }

    PROCESS_ERROR_RET_CODE(m_nSvrGroupID == pTargetOnlineRole->pRoleData->byGroup, errGuildRoleNotExist);
    PROCESS_ERROR_RET_CODE(0 == pTargetOnlineRole->pRoleData->dwGuildID , errGuildAlreadyJoin);
    PROCESS_ERROR_RET_CODE(pTargetOnlineRole->pRoleData->byLevel >= g_GuildConfig.nCanJoinGuildLevel, errGuildJoinGuildLevelLimit);
    PROCESS_ERROR_RET_CODE(pTargetOnlineRole->pRoleData->dwJoinGuildCD < CManageServer::GetInstance().GetGameTime(), errGuildTargetJoinCD);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_INVITE, errGuildNoRight);

    PROCESS_ERROR_RET_CODE(m_memberList.size() < GetMaxMemberNum(), errGuildMemberCountFull);

    pTargetOnlineRole->dwGuildInviterID = onlineRoleData.pRoleData->dwRoleID;

    // Send invitation
    nRetCode = CMessageHandler::GetInstance().DoClientBeInvitedToJoinGuild(pTargetOnlineRole->dwCliSessionID, onlineRoleData.pRoleData->dwRoleID, 
        onlineRoleData.pRoleData->szRoleName, m_szGuildName);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoErrorNotify(onlineRoleData.dwCliSessionID, errGuildInviteSuccessfully, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::AgreeToJoinGuild(UINT32 dwInviterID, MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA *pInviterRoleData = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;
    MS_ROLE_DATA    *pLeaderRoleData = NULL;
    MS_GUILD_WAR_DATA   *pWarData = NULL;
    pInviterRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwInviterID);

    onlineRoleData.dwGuildInviterID = 0;    // Clear the invitation record after either acceptance or rejection. Allan Tao [22.4.2013]

    poMsGuildMember = GetMember(dwInviterID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_INVITE, errGuildNoRight);

    LOG_PROCESS_ERROR(onlineRoleData.pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->dwJoinGuildCD < CManageServer::GetInstance().GetGameTime(), errGuildJoinCD);
    PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->byLevel >= g_GuildConfig.nCanJoinGuildLevel, errGuildJoinGuildLevelLimit);
    PROCESS_ERROR_RET_CODE( 0 == onlineRoleData.pRoleData->dwGuildID , errGuildAlreadyJoin);
    PROCESS_ERROR_RET_CODE(m_memberList.size() < GetMaxMemberNum(), errGuildMemberCountFull);

    // add member Allan Tao[8.4.2013] 
    nRetCode = AddMember(onlineRoleData.pRoleData, GUILD_POSITION_CIVILIAN);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    if (pInviterRoleData)
    {
        nRetCode = CMessageHandler::GetInstance().DoCLientInviteToJoinGuildAck(pInviterRoleData->dwCliSessionID, 
            onlineRoleData.pRoleData->szRoleName, GUILD_REPLY_AGREE);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    }

    return errSuccess;
Exit0:
    if (pInviterRoleData)
        CMessageHandler::GetInstance().DoCLientInviteToJoinGuildAck(pInviterRoleData->dwCliSessionID, 
        onlineRoleData.pRoleData->szRoleName, GUILD_REPLY_REFUSE);
    return nResult;
}

INT32 CMsGuild::LeaveGuild(MS_ROLE_DATA &roleData)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(roleData.dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE( GUILD_POSITION_LEADER != poMsGuildMember->GetPosition(), errGuildPosLeader);   

    nResult = _LeaveGuild(GUILD_LEAVE_ACTIVE, roleData.dwRoleID);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_LEAVE, roleData.dwRoleID, 0, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_LEAVE, roleData.dwRoleID, 0, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::BanishSomebody(MS_ONLINE_ROLE_DATA &onlineRoleData, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;
    CMsGuildMember  *poMsTargetGuildMember = NULL;
    MS_ROLE_DATA    *pTargetRoleData = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);
    poMsTargetGuildMember = GetMember(dwRoleID);
    PROCESS_ERROR_RET_CODE(poMsTargetGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < poMsTargetGuildMember->GetPosition(), errGuildPositionLow);

    nResult = ForcedSalariedWithMail(dwRoleID);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    nResult = _LeaveGuild(GUILD_LEAVE_PASSIVE, dwRoleID);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_BANISH, dwRoleID, onlineRoleData.pRoleData->dwRoleID, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_BANISH, onlineRoleData.pRoleData->dwRoleID, dwRoleID, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

    pTargetRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    if (pTargetRoleData)
    {
        char mail_content[MAIL_CONTENT_LEN] = {0};
        const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildBeBanished);
        LOG_PROCESS_ERROR(mail_data);

        snprintf(mail_content, MAIL_CONTENT_LEN, mail_data->m_szMailContent, onlineRoleData.pRoleData->szRoleName);

        nRetCode = CMailMgr::GetInstance().SendSystemMail(pTargetRoleData, mail_data->m_szMailSubject, mail_content);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::DisbandGuild(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32   nResult = errGuildFailed;
    INT32   nRetCode = 0;
    CMsGuildMember  *poMsGuildMember = NULL;
    char        szMailContent[MAIL_CONTENT_LEN] = {0};
    const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildDisband);
    LOG_PROCESS_ERROR(mail_data);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() == GUILD_POSITION_LEADER, errGuildNoRight);
    LOG_PROCESS_ERROR_RET_CODE(GetLevel() < g_GuildConfig.nGuildDisbandLevelLimit, errGuildGuildHighLevel);
    PROCESS_ERROR_RET_CODE(0 == m_nAllianceNum, errGuildDelHaveAlliance);
    PROCESS_ERROR_RET_CODE(0 == m_nEnemyNum, errGuildDelHaveEnemy);

    snprintf(szMailContent, MAIL_CONTENT_LEN, mail_data->m_szMailContent, onlineRoleData.pRoleData->szRoleName, GetName());
    nRetCode = BroadcastMail(mail_data->m_szMailSubject, szMailContent);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);

	nRetCode = CRankListMgr::GetInstance().DelRankNode(m_nSvrGroupID, m_dwID);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().DestroyGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildDestoryFailed);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::Demise(MS_ONLINE_ROLE_DATA &onlineRoleData, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;
    CMsGuildMember  *poMsTargetGuildMember = NULL;

    LOG_PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->dwRoleID != dwRoleID, errGuildCannotDemiseMyself);
    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    poMsTargetGuildMember = GetMember(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsTargetGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_LEADER == poMsGuildMember->GetPosition(), errGuildNotLeader);

    nRetCode = poMsGuildMember->SetPosition(GUILD_POSITION_CIVILIAN, FALSE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSetPositionFailed);
    nRetCode = poMsTargetGuildMember->SetPosition(GUILD_POSITION_LEADER, FALSE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSetPositionFailed);

    m_dwLeaderID = dwRoleID;

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_DEMISE, onlineRoleData.pRoleData->dwRoleID, dwRoleID, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    // update mysql
    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_LEADER, GetID());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_DEMISE, onlineRoleData.pRoleData->dwRoleID, dwRoleID, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildDemiseNtf(this, onlineRoleData.pRoleData->dwRoleID, dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ChangeNotice(MS_ONLINE_ROLE_DATA &onlineRoleData, const char *pcszNotice)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;

    LOG_PROCESS_ERROR(pcszNotice);
    LOG_PROCESS_ERROR_RET_CODE(strlen(pcszNotice) < GUILD_NOTICE_LEN, errGuildNoticeSizeError);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_NOTICE, errGuildNoRight);

    memset(m_szNotice, 0, GUILD_NOTICE_LEN);
    strxcpy(m_szNotice, pcszNotice, sizeof(m_szNotice));
 
    // update mysql
    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_NOTICE, GetID());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildNoticeChangeNtf(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ChangeAnnounce(MS_ONLINE_ROLE_DATA &onlineRoleData, const char *pcszAnnounce)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;

    LOG_PROCESS_ERROR(pcszAnnounce);
    LOG_PROCESS_ERROR_RET_CODE(strlen(pcszAnnounce) < GUILD_ANNOUNCE_LEN, errGuildAnnounceSizeError);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_NOTICE, errGuildNoRight);

    memset(m_szAnnounce, 0, GUILD_ANNOUNCE_LEN);
    strxcpy(m_szAnnounce, pcszAnnounce, sizeof(m_szAnnounce));

    // update mysql
    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_ANNOUNCE, GetID());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = CMessageHandler::GetInstance().DoErrorNotify(onlineRoleData.dwCliSessionID, errGuildChangeAnnounceSucc, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ForbidChat(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nOperation, UINT32 dwRoleID)
{
    INT32           nRetCode = 0;
    UINT8           byFlag = 0;
    INT32           nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;
    CMsGuildMember  *poMsTargetGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_FORBID_CHAT, errGuildNoRight);

    poMsTargetGuildMember = GetMember(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsTargetGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsTargetGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(poMsTargetGuildMember->GetPosition() >= poMsGuildMember->GetPosition(), errGuildPositionLow);   // Lower position values grant greater authority. Allan Tao [23.4.2013]
    

    if (GUILD_CHAT_FORBID == nOperation )
    {
        // todo_tq 11: consider this. Allan Tao [19.11.2013]
        byFlag = poMsTargetGuildMember->GetFlag();
        byFlag |= GUILD_FLAG_IS_CHAT_FORBIDDEN;
        nRetCode = poMsTargetGuildMember->SetFlag(byFlag);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSetChatFlagError);
    }
    else if (GUILD_CHAT_UNFORBID == nOperation)
    {
        byFlag = poMsTargetGuildMember->GetFlag();
        byFlag &= (~GUILD_FLAG_IS_CHAT_FORBIDDEN);
        nRetCode = poMsTargetGuildMember->SetFlag(byFlag);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSetChatFlagError);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientGuildForbidChatNtf(this, dwRoleID, nOperation);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ChangePosition(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nPosition, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;
    CMsGuildMember  *poMsTargetGuildMember = NULL;
    UINT8       byOldPosition;

    LOG_PROCESS_ERROR_RET_CODE(nPosition > GUILD_POSITION_LEADER && nPosition < GUILD_POSITION_TOTAL, errGuildClientParamError);

    PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData->dwRoleID != dwRoleID, errGuildCannotSetSelfPosition);
    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_CHANGE_POS, errGuildNoRight);
    PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < nPosition, errGuildPositionLow);

    PROCESS_ERROR_RET_CODE(GetMemberNumInAPost(nPosition) < ARR_GUILD_POS_HUMAN_NUM[nPosition - 1], errGuildPosHumanFull);

    poMsTargetGuildMember = GetMember(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsTargetGuildMember, errGuildNotMember);
    PROCESS_ERROR_RET_CODE(poMsTargetGuildMember->GetPosition() != GUILD_POSITION_LEADER, errGuildNotLeader);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < poMsTargetGuildMember->GetPosition(), errGuildPositionLow);
    PROCESS_ERROR_RET_CODE(poMsTargetGuildMember->GetPosition() != nPosition, errGuildSamePosition);

    byOldPosition = poMsTargetGuildMember->GetPosition();
    nRetCode = poMsTargetGuildMember->SetPosition(nPosition, TRUE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSetPositionFailed);

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_POSITION_CHANGE, dwRoleID, onlineRoleData.pRoleData->dwRoleID, byOldPosition, nPosition);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_POSITION_CHANGE, onlineRoleData.pRoleData->dwRoleID, dwRoleID, byOldPosition, nPosition);
    LOG_CHECK_ERROR(nRetCode);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::Diplomat(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nOpType, INT32 nTimeType, CMsGuild *pMsTargetGuild)
{
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;

    LOG_PROCESS_ERROR_RET_CODE(pMsTargetGuild, errGuildFailed);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(pMsTargetGuild->m_dwID != m_dwID, errGuildCannotAddYourself);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_DIPLOMAT, errGuildNoRight);

    if (GUILD_DIPLOMAT_ALLIANCE == nOpType)
    {
        nResult = _DiplomatAlliance(onlineRoleData, pMsTargetGuild, nTimeType);
    }
    else if (GUILD_DIPLOMAT_ENEMY == nOpType)
    {
        nResult = _DiplomatEnemy(onlineRoleData, pMsTargetGuild, nTimeType);
    }
    PROCESS_ERROR(errSuccess == nResult);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ReplyDiplomatApp(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nOp, UINT32 dwApplicantID)
{
    INT32       nResult = errUnknownError;
    CMsGuild    *pApplicantGuild = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_DIPLOMAT, errGuildNoRight);

    pApplicantGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwApplicantID);
    LOG_PROCESS_ERROR_RET_CODE(pApplicantGuild, errGuildNotFind);

    if (GUILD_REVERT_AGREE == nOp)
    {
        nResult = _AgreeApplyAlliance(onlineRoleData, pApplicantGuild);
    }
    else if (GUILD_REVERT_REFUSE == nOp)
    {
        nResult = _RefuseApplyAlliance(onlineRoleData, pApplicantGuild);
    }
    PROCESS_ERROR(errSuccess == nResult);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ChangeStorageRight(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nPosition, UINT16 wRight)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild    *pMsTargetGuild = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(nPosition > GUILD_POSITION_NONE && nPosition < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() > GUILD_POSITION_NONE && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_SET_STORAGE_RIGHT, errGuildNoRight);

    m_wStorageRight[nPosition - 1] = wRight;

    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildChangeStorageRightNtf(this, nPosition, wRight);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::DrawSalary(MS_ONLINE_ROLE_DATA &onlineRoleData)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;
    INT32       nSalary = 0;
    INT32       nCurrentWeekTime = CManageServer::GetInstance().GetGameTime()/(g_GuildConfig.nDrawSalaryPeriod);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    PROCESS_ERROR_RET_CODE(poMsGuildMember->GetSalary() != 0, errGuildNoSalary);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetDrawSalaryTime() != nCurrentWeekTime, errGuildDrawSalaryAlready);

    nSalary = poMsGuildMember->GetSalary() *  g_GuildConfig.nGuildDrawSalaryPercent / 100;
    nRetCode = poMsGuildMember->SetDrawSalaryTime(nCurrentWeekTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildDrawSalaryFailed);
    nRetCode = poMsGuildMember->SubSalary(nSalary, FALSE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildDrawSalaryFailed);

    // send to gs to add
    nRetCode = CMessageHandler::GetInstance().DoGSGuildDrawSalary(onlineRoleData.dwGSSessionID, 
        onlineRoleData.pRoleData->dwRoleID, nSalary);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::ForcedSalariedWithMail(UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errGuildFailed;
    CMsGuildMember  *poMsGuildMember = NULL;

    INT32       nSalary = 0;
    INT32       nCurrentWeekTime = CManageServer::GetInstance().GetGameTime()/(g_GuildConfig.nDrawSalaryPeriod);
    MS_ROLE_DATA    *pRoleData = NULL;
    const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildDrawSalary);
    LOG_PROCESS_ERROR(mail_data);

    poMsGuildMember = GetMember(dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildFailed);

    PROCESS_ERROR_RET_CODE(poMsGuildMember->GetSalary() != 0, errSuccess);

    nSalary = poMsGuildMember->GetSalary() *  g_GuildConfig.nGuildDrawSalaryPercent / 100;
    nRetCode = poMsGuildMember->SubSalary(nSalary);
    LOG_CHECK_ERROR(nRetCode);

    pRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    if (pRoleData)
    {
         nRetCode = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(pRoleData, mail_data->m_szMailSubject, mail_data->m_szMailContent, MONEYTYPE_02, nSalary);
        LOG_CHECK_ERROR(nRetCode);
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::Contribute(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nType, INT32 nMoney)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;

    LOG_PROCESS_ERROR_RET_CODE(onlineRoleData.pRoleData, errUnknownError);
    LOG_PROCESS_ERROR_RET_CODE(nMoney >= 0, errGuildFailed);
    switch(nType)
    {
    case GUILD_ATTR_MONEY:
        nRetCode = AddMoney(nMoney, GUILD_CHEST_CONTRIBUTE);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddMoneyFailed);
        nRetCode = AddGuildChestEvent(GUILD_CHEST_CONTRIBUTE, onlineRoleData.pRoleData->dwRoleID, nMoney, 0, 0);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);
        break;
    case GUILD_ATTR_WOOD:
        nRetCode = AddWood(nMoney);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddWoodFailed);
        break;
    case GUILD_ATTR_HERB:
        nRetCode = AddHerb(nMoney);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddHerbFailed);
        break;
    case GUILD_ATTR_CRYSTAL:
        nRetCode = AddCrystal(nMoney);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddCrystalFailed);
        break;
    case GUILD_ATTR_SILVER:
        nRetCode = AddSilver(nMoney);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddSilverFailed);
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::PrePushItem(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nSourceConSlot, INT32 nSourceItemSlot, INT32 nConSlot, INT32 nItemSlot)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild    *pMsTargetGuild = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotFind);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() > GUILD_POSITION_NONE && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    // todo_tq 11: this unexpectedly uses position minus one as an array index; investigate later. Allan Tao [19.11.2013]
    PROCESS_ERROR_RET_CODE(m_wStorageRight[poMsGuildMember->GetPosition()-1] & (1 << (nConSlot + 6)), errGuildNoRight);


    nRetCode = m_storage.PrePushItem(nConSlot, nItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildPrePushItemAck(onlineRoleData.dwGSSessionID, onlineRoleData.pRoleData->dwRoleID, nSourceConSlot, nSourceItemSlot, nConSlot, nItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    if (errGuildMsgError == nResult)
    {
        nResult = m_storage.UnPrePushItem(nConSlot, nItemSlot);
        LOG_CHECK_ERROR(nResult == errSuccess);
    }
    return nResult;
}

INT32 CMsGuild::PushItem(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nGSResult, INT32 nConSlot, INT32 nItemSlot, INT32 nItemSize, char *pbuff)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild    *pMsTargetGuild = NULL;
    CMsGuildMember  *poMsGuildMember = NULL;

    LOG_PROCESS_ERROR(pbuff);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nGSResult, nGSResult);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() > GUILD_POSITION_NONE && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(m_wStorageRight[poMsGuildMember->GetPosition()-1] & (1 << (nConSlot + 6)), errGuildNoRight);

    nRetCode = m_storage.PushItem(onlineRoleData.pRoleData, nConSlot, nItemSlot, nItemSize, pbuff);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildPushItemAck(onlineRoleData.dwCliSessionID, nConSlot, nItemSlot, nItemSize, pbuff);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    nResult = m_storage.UnPrePushItem(nConSlot, nItemSlot);
    LOG_CHECK_ERROR(errSuccess == nResult);
    return nResult;
}

INT32 CMsGuild::PrePopItem(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nConSlot, INT32 nItemSlot, INT32 nTargetConSlot, INT32 nTargetItemSlot)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR(poMsGuildMember->GetPosition() > GUILD_POSITION_NONE && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL);
    PROCESS_ERROR_RET_CODE(m_wStorageRight[poMsGuildMember->GetPosition()-1] & (1 << nConSlot), errGuildNoRight);

    nRetCode = m_storage.PrePopItem(nConSlot, nItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildPrePopItemAck(onlineRoleData.dwGSSessionID, 
        onlineRoleData.pRoleData->dwRoleID, nTargetConSlot, nTargetItemSlot, nConSlot, nItemSlot);
    LOG_PROCESS_ERROR(nRetCode);

    return errSuccess;
Exit0:
    if (errGuildMsgError == nResult)
    {
        nResult = m_storage.UnPrePopItem(nConSlot, nItemSlot);
        LOG_CHECK_ERROR(nResult == errSuccess);
    }
    return nResult;
}

INT32 CMsGuild::PopItem(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nGSResult, INT32 nSourceConSlot, INT32 nSourceItemSlot, INT32 nTargetConSlot, INT32 nTargetItemSlot)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;
    GUILD_STORAGE_ITEM_DATA      stItemData;
    INT32           nBufferSize = 0;

    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nGSResult, nGSResult);

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() > GUILD_POSITION_NONE && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(m_wStorageRight[poMsGuildMember->GetPosition()-1] & (1 << nSourceConSlot), errGuildNoRight);

    nBufferSize = sizeof(stItemData.szItemBuffer);
    nRetCode = m_storage.PopItem(onlineRoleData.pRoleData, nSourceConSlot, nSourceItemSlot, nBufferSize, stItemData.szItemBuffer);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildPopItem(onlineRoleData.dwCliSessionID, nSourceConSlot, nSourceItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildPopItemAck(onlineRoleData.dwGSSessionID, onlineRoleData.pRoleData->dwRoleID, nTargetConSlot, nTargetItemSlot, nBufferSize, stItemData.szItemBuffer);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    nResult = m_storage.UnPrePopItem(nSourceConSlot, nSourceItemSlot);
    LOG_CHECK_ERROR(errSuccess == nResult);
    return nResult;
}

INT32 CMsGuild::MoveItem(MS_ONLINE_ROLE_DATA &onlineRoleData, INT32 nSourceConSlot, INT32 nSourceItemSlot, INT32 nTargetConSlot, INT32 nTargetItemSlot)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuildMember  *poMsGuildMember = NULL;

    poMsGuildMember = GetMember(onlineRoleData.pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    LOG_PROCESS_ERROR(poMsGuildMember->GetPosition() > GUILD_POSITION_NONE && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL);
    LOG_PROCESS_ERROR_RET_CODE(m_wStorageRight[poMsGuildMember->GetPosition()-1] & (1 << nSourceConSlot), errGuildNoRight);
    LOG_PROCESS_ERROR_RET_CODE(m_wStorageRight[poMsGuildMember->GetPosition()-1] & (1 << nTargetConSlot), errGuildNoRight);

    nResult = m_storage.MoveItem(nSourceConSlot, nSourceItemSlot, nTargetConSlot, nTargetItemSlot);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildMoveItem(onlineRoleData.dwCliSessionID, 
        nSourceConSlot, nSourceItemSlot, nTargetConSlot, nTargetItemSlot);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 _InsertPowList(INT32 arr[], INT32 &nPowListNum, INT32 nPow)
{
    INT32       nTempPow = 0;

    for (INT32 i = 0; i < nPowListNum; ++i)
    {
        if (arr[i] < nPow)
        {
            nTempPow = arr[i];
            arr[i] = nPow;
            nPow = nTempPow;
        }
    }

    arr[nPowListNum] = nPow;
    ++nPowListNum;

    return nPow;
}

INT32 _UpdatePowList(INT32 arr[], INT32 &nPowListNum, INT32 nPow)
{
    INT32       nTempPow = 0;

    for (INT32 i = 0; i < nPowListNum; ++i)
    {
        if (arr[i] < nPow)
        {
            nTempPow = arr[i];
            arr[i] = nPow;
            nPow = nTempPow;
        }
    }
    return arr[nPowListNum - 1];
}

INT32 CMsGuild::GetPow()
{
    INT32           nRetCode = 0;
    INT32           nTotalPow = 0;
    INT32           nPowLimit = 0;
    INT32           nCorePowNum = 0;
    INT32           CorePow[MAX_GUILD_CORE_MEMBER] = { 0 };

    CMsGuildMember  *poMsGuildMember = NULL;
    MEMBER_LIST::iterator end_iter = m_memberList.end();

    for (MEMBER_LIST::iterator iter = m_memberList.begin(); iter != end_iter; ++iter)
    {
        if (iter->GetPow() == 0)
            continue;

        if (nCorePowNum < MAX_GUILD_CORE_MEMBER)
        {
            nPowLimit = _InsertPowList(CorePow, nCorePowNum, iter->GetPow());
        }
        else
        {
            if (nPowLimit >= iter->GetPow())
                continue;
            nPowLimit = _UpdatePowList(CorePow, nCorePowNum, iter->GetPow());
        }

        nTotalPow += iter->GetPow();
    }

    for (INT32 i = 0; i < nCorePowNum; ++i)
    {
        nTotalPow += CorePow[i];
    }

    return nTotalPow >> 1;
}

// lua
BOOL CMsGuild::LuaBroadcastMsgNotifyType(INT32 nType)
{
    INT32       nRetCode = 0;

    nRetCode = CMessageHandler::GetInstance().DoClientGuildNotifyNtf(this, nType, G_POS_RIGHT_NONE);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LuaAddProtectionTime(UINT32 dwTime)
{
    INT32       nRetCode = 0;

    if (GetProtectionTime() >= CManageServer::GetInstance().GetGameTime())
    {
        nRetCode = SetProtectionTime(GetProtectionTime() + dwTime);
        LOG_CHECK_ERROR(nRetCode);
    }
    else
    {
        nRetCode = SetProtectionTime(CManageServer::GetInstance().GetGameTime() + dwTime);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LuaLeaveGuild(INT32 nLeaveType, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;

    LOG_PROCESS_ERROR(m_dwLeaderID != dwRoleID);

    if (GUILD_LEAVE_PASSIVE == nLeaveType)
    {
        nRetCode = ForcedSalariedWithMail(dwRoleID);
        LOG_PROCESS_ERROR(errSuccess == nRetCode);
    }

    nRetCode = _LeaveGuild(nLeaveType, dwRoleID);
    LOG_PROCESS_ERROR(errSuccess == nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::LuaBroadcastJoinWar(INT32 nWarCharacter, UINT32 dwTargetGuildID)
{
    return CMessageHandler::GetInstance().DoClientGuildJoinWarNtf(this, nWarCharacter, dwTargetGuildID);
}

BOOL CMsGuild::LuaAddBuilding(const char *pcszNameID, INT32 nType, INT32 nLevel, UINT32 dwReviveTime)
{
    UINT64      qwNameID = 0;
    LOG_PROCESS_ERROR(pcszNameID);

    if (nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM)
    {
        strncpy((char*)&m_FunBuilding[nType].qwNameID, pcszNameID, sizeof(m_FunBuilding[nType].qwNameID));
        m_FunBuilding[nType].nType = nType;
        m_FunBuilding[nType].nLevel = nLevel;
        m_FunBuilding[nType].dwReviveTime = dwReviveTime;
    }
    else
    {
        strncpy((char*)&qwNameID, pcszNameID, sizeof(qwNameID));
        m_BuildingList.insert(std::make_pair(qwNameID, MS_G_B_DATA(qwNameID, nType, nLevel, dwReviveTime)));
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::_IsInGuildAppList(UINT32 dwRoleID)
{
    APP_NODE    *pNode = NULL;
    pNode = CApplicationMgr::GetInstance().Find(dwRoleID, m_dwID);
    PROCESS_ERROR(pNode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::_AddRoleJoinGuildApp(UINT32 dwRoleID)
{
    APP_NODE    *pNode = CMsGuildMgr::GetInstance().CreateGuildApp();
    LOG_PROCESS_ERROR(pNode);

    pNode->dwID_1 = dwRoleID;
    pNode->dwID_2 = m_dwID;
    pNode->dwWashOutTime = g_GuildConfig.nGuildApplicationLastTime + CManageServer::GetInstance().GetGameTime();
    pNode->nType = EN_GUILD_APPLICATION;
    pNode->fp = CB_GuildAppTimeout;

    LOG_PROCESS_ERROR(CApplicationMgr::GetInstance().InsertApplication(pNode));

    return TRUE;
Exit0:
    if (pNode)
    {
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteGuildApp(pNode));
    }
    return FALSE;
}

BOOL CMsGuild::_DelRoleJoinGuildApp(UINT32 dwRoleID)
{
    APP_NODE    *pNode = NULL;
    INT32       nRetCode = 0;

    pNode = CApplicationMgr::GetInstance().Find(dwRoleID, m_dwID);
    LOG_PROCESS_ERROR(pNode);

    LOG_PROCESS_ERROR(CApplicationMgr::GetInstance().RemoveApplication(pNode));
    LOG_PROCESS_ERROR(pNode);
    nRetCode = CMsGuildMgr::GetInstance().DeleteGuildApp(pNode);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pNode)
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteGuildApp(pNode));
    return FALSE;
}

CMsGuildMember *CMsGuild::GetFirstGuildMember()
{
    CMsGuildMember    *poMsGuildMember = NULL;

    PROCESS_ERROR(0 != m_memberList.size());
    m_memberListCursor = m_memberList.begin();
    LOG_PROCESS_ERROR(m_memberListCursor != m_memberList.end());

    poMsGuildMember = &(*m_memberListCursor);
    m_memberListCursor++;

    return poMsGuildMember;
Exit0:
    return NULL;
}

CMsGuildMember *CMsGuild::GetNextGuildMember()
{
    CMsGuildMember    *poMsGuildMember = NULL;
    PROCESS_ERROR(m_memberListCursor != m_memberList.end());

    poMsGuildMember = &(*m_memberListCursor);
    ++m_memberListCursor;

    return poMsGuildMember;
Exit0:
    return NULL;
}

INT32 CMsGuild::GetUpgradeWelfareNum()
{
    return m_nUpgradeWelfareAuraCount;
}

GUILD_UPGRADE_TRIGGER* CMsGuild::GetUpgradeWelfare(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < m_nUpgradeWelfareAuraCount);
    return m_paWelfareUpgradeData[nIndex];
Exit0:
    return NULL;
}

INT32 CMsGuild::GetUpgradeBuildingNum()
{
    return m_nUpgradeBuildingNum;
}

GUILD_UPGRADE_TRIGGER* CMsGuild::GetUpgradeBuilding(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < m_nUpgradeBuildingNum);
    return m_pUpgradeBuilding[nIndex];
Exit0:
    return NULL;
}

GUILD_DUNGEON* CMsGuild::GetFirstDungeon()
{
    GUILD_DUNGEON    *pDungeon = NULL;

    FilterDungeon();

    PROCESS_ERROR(0 != m_listDungeon.size());
    m_DungeonListCursor = m_listDungeon.begin();
    LOG_PROCESS_ERROR(m_DungeonListCursor != m_listDungeon.end());

    pDungeon = &(*m_DungeonListCursor);
    m_DungeonListCursor++;

    return pDungeon;
Exit0:
    return NULL;
}

GUILD_DUNGEON* CMsGuild::GetNextDungeon()
{
    GUILD_DUNGEON    *pDungeon = NULL;
    PROCESS_ERROR(m_DungeonListCursor != m_listDungeon.end());

    pDungeon = &(*m_DungeonListCursor);
    ++m_DungeonListCursor;

    return pDungeon;
Exit0:
    return NULL;
}

APP_NODE *CMsGuild::GetFirstRoleJoinGuildApp()
{
    APP_NODE    *pNode = NULL;

    pNode = CApplicationMgr::GetInstance().FirstAppNodeByID2(m_dwID);
    PROCESS_ERROR(pNode);

    return pNode;
Exit0:
    return NULL;
}

APP_NODE *CMsGuild::GetNextRoleJoinGuildApp(APP_NODE *pNode)
{
    APP_NODE    *pNextNode = NULL;
    pNextNode = CApplicationMgr::GetInstance().NextAppNodeByID2(pNode);
    PROCESS_ERROR(pNextNode);

    return pNextNode;
Exit0:
    return NULL;
}

DIPLOMAT_APP_NODE *CMsGuild::GetFirstAllianceApp()
{
    DIPLOMAT_APP_NODE *pNode = NULL;
    pNode = (DIPLOMAT_APP_NODE*)CApplicationMgr::GetInstance().FirstAppNodeByID2(m_dwID);
    PROCESS_ERROR(pNode);

    while (pNode)
    {
        if (pNode->nType == EN_DIPLOMAT_APPLICATION)
            break;
        pNode = (DIPLOMAT_APP_NODE*)CApplicationMgr::GetInstance().NextAppNodeByID2(pNode);
    }
    return pNode;
Exit0:
    return NULL;
}

DIPLOMAT_APP_NODE *CMsGuild::GetNextAllianceApp(DIPLOMAT_APP_NODE* pNode)
{
    DIPLOMAT_APP_NODE   *pNextNode = NULL;
    pNextNode = (DIPLOMAT_APP_NODE*)CApplicationMgr::GetInstance().NextAppNodeByID2(pNode);
    PROCESS_ERROR(pNextNode);
    while (pNextNode)
    {
        if (pNextNode->nType == EN_DIPLOMAT_APPLICATION)
            break;
        pNextNode = (DIPLOMAT_APP_NODE*)CApplicationMgr::GetInstance().NextAppNodeByID2(pNextNode);
    }

    return pNextNode;
Exit0:
    return NULL;
}

EVENT_DATA *CMsGuild::GetFirstGuildEventData()
{
    EVENT_DATA *pData = NULL;
    BASE_LIST_NODE *pNode = m_History.Head().pNext;

    LOG_PROCESS_ERROR(pNode != &m_History.Rear());

    pData = node_cast(EVENT_DATA*, node, pNode);

    return pData;
Exit0:
    return NULL;
}

EVENT_DATA *CMsGuild::GetNextGuildEventData(EVENT_DATA *pData)
{
    BASE_LIST_NODE  *pNode = NULL;

    LOG_PROCESS_ERROR(pData);

    pNode = pData->node.pNext;
    PROCESS_ERROR(pNode != &m_History.Rear());

    pData = node_cast(EVENT_DATA*, node, pNode);

    return pData;
Exit0:
    return NULL;
}

EVENT_DATA* CMsGuild::FirstChestEvent()
{
    EVENT_DATA *pData = NULL;
    BASE_LIST_NODE *pNode = m_ChestHistory.Head().pNext;

    PROCESS_ERROR(pNode != &m_ChestHistory.Rear());

    pData = node_cast(EVENT_DATA*, node, pNode);

    return pData;
Exit0:
    return NULL;
}

EVENT_DATA* CMsGuild::NextChestEvent(EVENT_DATA *pData)
{
    BASE_LIST_NODE  *pNode = NULL;

    LOG_PROCESS_ERROR(pData);

    pNode = pData->node.pNext;
    PROCESS_ERROR(pNode != &m_ChestHistory.Rear());

    pData = node_cast(EVENT_DATA*, node, pNode);

    return pData;
Exit0:
    return NULL;
}

MS_G_B_DATA* CMsGuild::FirstUnFunBuilding()
{
    m_BCursor = m_BuildingList.begin();
    PROCESS_ERROR(m_BCursor != m_BuildingList.end());

    return &(m_BCursor->second);
Exit0:
    return NULL;
}

MS_G_B_DATA* CMsGuild::NextUnFunBuilding()
{
    ++m_BCursor;
    PROCESS_ERROR(m_BCursor != m_BuildingList.end());

    return &(m_BCursor->second);
Exit0:
    return NULL;
}

BOOL CMsGuild::AddGuildEvent(INT32 nType, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4)
{
    INT32   nRetCode = 0;
    EVENT_DATA *pEventData = NULL;
    BASE_LIST_NODE  *pNode = NULL;
    
    if (m_nHistoryNum == GUILD_MAX_HISTORY_EVENT_NUM)
    {
        pNode = m_History.PopHead();
        LOG_PROCESS_ERROR(pNode);
        --m_nHistoryNum;
        pEventData = (EVENT_DATA*)((char*)pNode - (char*)&(((EVENT_DATA*)0)->node));
    }
    else
    {
        pEventData = CMsGuildMgr::GetInstance().CreateEventData();
    }
    LOG_PROCESS_ERROR(pEventData);

    ClearNode(&(pEventData->node));
    pEventData->nType = nType;
    pEventData->param1 = param1;
    pEventData->param2 = param2;
    pEventData->param3 = param3;
    pEventData->param4 = param4;
    pEventData->dwOccurTime = CManageServer::GetInstance().GetGameTime();

    nRetCode = m_History.PushRear(&pEventData->node);
    LOG_PROCESS_ERROR(nRetCode);
    ++m_nHistoryNum;

    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_MAIN, m_dwID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientAddHistoryEventNtf(this, 
        nType, param1, param2, param3, param4, pEventData->dwOccurTime);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    // release
    if (pEventData)
    {
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteEventData(pEventData));
    }

    return FALSE;
}

BOOL CMsGuild::AddGuildChestEvent(INT32 nType, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4)
{
    INT32   nRetCode = 0;
    BASE_LIST_NODE  *pNode = NULL;
    EVENT_DATA      *pEventData = NULL;
    
    if (m_nChestHistoryNum == GUILD_MAX_CHEST_EVENT_NUM)
    {
        pNode = m_ChestHistory.PopHead();
        LOG_PROCESS_ERROR(pNode);
        --m_nChestHistoryNum;
        pEventData = (EVENT_DATA*)((char*)pNode - (char*)&(((EVENT_DATA*)0)->node));
    }
    else
    {
        pEventData = CMsGuildMgr::GetInstance().CreateEventData();
    }
    LOG_PROCESS_ERROR(pEventData);

    ClearNode(&pEventData->node);
    pEventData->nType = nType;
    pEventData->param1 = param1;
    pEventData->param2 = param2;
    pEventData->param3 = param3;
    pEventData->param4 = param4;
    pEventData->dwOccurTime = CManageServer::GetInstance().GetGameTime();

    nRetCode = m_ChestHistory.PushRear(&pEventData->node);
    LOG_PROCESS_ERROR(nRetCode);
    ++m_nChestHistoryNum;

    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_MAIN, m_dwID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientAddHistoryEventNtf(this, 
        nType, param1, param2, param3, param4, pEventData->dwOccurTime);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    // release
    if (pEventData)
    {
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteEventData(pEventData));
    }

    return FALSE;
}

BOOL CMsGuild::BroadcastMail(INT32 nMailTemplateID)
{
    INT32       nRetCode = 0;
    MS_ROLE_DATA    *pRoleData = NULL;
    MEMBER_LIST::iterator    end_iter = m_memberList.end();
    const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(nMailTemplateID);
    LOG_PROCESS_ERROR(mail_data);

    for (MEMBER_LIST::iterator iter = m_memberList.begin(); iter != end_iter; ++iter)
    {
        if (iter->GetPosition() != GUILD_POSITION_LEADER && iter->GetPosition() != GUILD_POSITION_SUBLEADER)
            continue;

        pRoleData = CRoleDataMgr::GetInstance().FindRole(iter->GetRoleID());
        LOG_CHECK_ERROR(pRoleData);
        if (pRoleData)
        {
            nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, mail_data->m_szMailSubject, mail_data->m_szMailContent); 
            LOG_CHECK_ERROR(nRetCode);   
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::BroadcastMail(const char *pcszTitle, const char *pcszContent)
{
    INT32       nRetCode = 0;
    MS_ROLE_DATA    *pRoleData = NULL;
    MEMBER_LIST::iterator    end_iter = m_memberList.end();

    LOG_PROCESS_ERROR(pcszTitle);
    LOG_PROCESS_ERROR(pcszContent);
    for (MEMBER_LIST::iterator iter = m_memberList.begin(); iter != end_iter; ++iter)
    {
        if (iter->GetPosition() != GUILD_POSITION_LEADER && iter->GetPosition() != GUILD_POSITION_SUBLEADER)
            continue;

        pRoleData = CRoleDataMgr::GetInstance().FindRole(iter->GetRoleID());
        LOG_CHECK_ERROR(pRoleData);
        if (pRoleData)
        {
            nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, pcszTitle, pcszContent); 
            LOG_CHECK_ERROR(nRetCode);   
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::_AddMember(UINT32 dwRoleID, INT32 nPosition)
{
    INT32       nResult = errGuildFailed;
    INT32       nRetCode = 0;
    CMsGuildMember      oMsGuildMember;
    oMsGuildMember.Init(GetID(), dwRoleID, nPosition, 0, 0, 0, 0);

    // add to mysql
    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertGMember, NULL, NULL, "uducuducucdud",
        oMsGuildMember.GetRoleID(), (UINT8)GetSvrGroup(), oMsGuildMember.GetGuildID(), (UINT8)oMsGuildMember.GetPosition(), (UINT8)oMsGuildMember.GetFlag(),
        oMsGuildMember.GetSalary(), oMsGuildMember.GetDrawSalaryTime());
    LOG_PROCESS_ERROR(nRetCode);

    m_memberList.push_back(oMsGuildMember);

    nRetCode = SetProsperity(GetProsperity() + g_GuildConfig.nGuildMemberBaseProsperity);
    LOG_CHECK_ERROR(nRetCode);

    CMSGuildAchiMgr::GetInstance().RecordAchiCondition(GUILD_ACHI_MEMBER, this);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMsGuild::AddMember(MS_ROLE_DATA* pRoleData, INT32 nPosition)
{
    INT32       nResult = errGuildFailed;
    INT32       nRetCode = 0;
    MS_GUILD_WAR_DATA   *pWarData = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRoleData = NULL;
    MS_ROLE_DATA        *pLeaderRole = NULL;

    LOG_PROCESS_ERROR(pRoleData);

    PROCESS_ERROR_RET_CODE(0 == pRoleData->dwGuildID, errGuildAlreadyJoin);
    PROCESS_ERROR_RET_CODE(m_memberList.size() < GetMaxMemberNum(), errGuildMemberCountFull);

    pRoleData->dwGuildID = m_dwID;

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_JOIN, pRoleData->dwRoleID, 0, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);
    
    nRetCode = _AddMember(pRoleData->dwRoleID, nPosition);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddMemberFailed);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_JOIN, pRoleData->dwRoleID, 0, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

    // ntf to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildAddMemberNtf(pRoleData->dwRoleID, nPosition, m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    // synac to member
    nRetCode = CMessageHandler::GetInstance().DoClientGuildAddMemberNtf(this, *pRoleData, nPosition, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pRoleData->dwRoleID);
    if (pOnlineRoleData)
    {
        pLeaderRole = CRoleDataMgr::GetInstance().FindRole(m_dwLeaderID);
        LOG_CHECK_ERROR(pLeaderRole);
        if (pLeaderRole)
        {
            nRetCode = CMessageHandler::GetInstance().DoClientGetGuildDataNtf(pOnlineRoleData->dwCliSessionID, this, pLeaderRole->szRoleName);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

            // todo_tq: members joining during a guild war
            //pWarData = CMsGuildMgr::GetInstance().FindBattleByAttack(GetID());
            //if (pWarData)
            //{
            //    nRetCode = CMessageHandler::GetInstance().DoClientGuildSyncWarStageNtf(this, this->GetID(), pWarData->dwDefenderID, pWarData->nStage, g_GuildConfig.nGuildWarWaitingTime);
            //    LOG_PROCESS_ERROR(nRetCode);
            //}

            //pWarData = CMsGuildMgr::GetInstance().FindBattleByDefend(GetID());
            //if (pWarData)
            //{
            //    nRetCode = CMessageHandler::GetInstance().DoClientGuildSyncWarStageNtf(this, pWarData->dwIntruderID, this->GetID(), pWarData->nStage, g_GuildConfig.nGuildWarWaitingTime);
            //    LOG_PROCESS_ERROR(nRetCode);
            //}

        }
        nRetCode = CMessageHandler::GetInstance().DoClientGuildJoinGuildAck(pOnlineRoleData->dwCliSessionID, m_szGuildName);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    }

    return errSuccess;
Exit0:
    return nResult;
}

void CMsGuild::DelMember(UINT32 dwRoleID)
{
    MEMBER_LIST::iterator    end_iter = m_memberList.end();
    for (MEMBER_LIST::iterator iter = m_memberList.begin(); iter != end_iter; ++iter)
    {
        if (iter->GetRoleID() == dwRoleID)
        {
            m_memberList.erase(iter);
            return ;
        }
    }
    return ;
}

CMsGuildMember* CMsGuild::GetMember(UINT32 dwRoleID)
{
    MEMBER_LIST::iterator    end_iter = m_memberList.end();
    for (MEMBER_LIST::iterator iter = m_memberList.begin(); iter != end_iter; ++iter)
    {
        if (iter->GetRoleID() == dwRoleID)
        {
            return &(*iter);
        }
    }

    return NULL;
}

INT32 CMsGuild::GetMemberNumInAPost(UINT8 byPosition)
{
    INT32       count = 0;
    MEMBER_LIST::iterator  iter = m_memberList.begin();
    MEMBER_LIST::iterator    end_iter = m_memberList.end();
    for ( ; iter != end_iter; ++iter)
    {
        if (iter->GetPosition() == byPosition)
        {
            ++count;
        }
    }
    return count;
}

INT32 CMsGuild::GetMatchWarCount()
{
    if (m_dwMatchWarTime / 86400 == CManageServer::GetInstance().GetGameTime() / 86400)
        return m_nMatchWarCount;
    return 0;    
}

INT32 CMsGuild::GetBuildingMaxLevelByType(INT32 nType)
{
    if (nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM)
    {
        if (m_FunBuilding[nType].dwReviveTime > 0)
            return 0;
        else
            return m_FunBuilding[nType].nLevel;
    }
    else
    {
        INT32       nLevel = 0;
        INT32       nTempLevel = 0;
        G_BUILDING_MAP::iterator    end_iter = m_BuildingList.end();

        for (G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != end_iter; ++iter)
        {
            if (iter->second.nType == nType)
            {
                if (iter->second.dwReviveTime > 0)
                    nTempLevel = 0;
                else
                    nTempLevel = iter->second.nLevel;

                if (nTempLevel > nLevel)
                    nLevel = nTempLevel;
            }
        }

        return nLevel;
    }
}

INT32 CMsGuild::GetBuildingNumByType(INT32 nType)
{
    if (nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM)
    {
        if (m_FunBuilding[nType].nType == 0)
            return 0;
        else
            return 1;
    }
    else
    {
        INT32       nNum = 0;
        G_BUILDING_MAP::iterator    end_iter = m_BuildingList.end();

        for (G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != end_iter; ++iter)
        {
            if (iter->second.nType == nType)
                ++nNum;
        }

        return nNum;
    }
}

INT32 CMsGuild::GetUpgradeBuildingNumByType(INT32 nType)
{
    INT32       nNum  = 0;
    for (INT32 i = 0; i < m_nUpgradeBuildingNum; ++i)
    {
        LOG_PROCESS_ERROR(m_pUpgradeBuilding[i]);
        if (m_pUpgradeBuilding[i]->nType == nType)
        {
            ++nNum;
        }
    }
Exit0:
    return nNum;
}

MS_G_B_DATA *CMsGuild::GetFunBuildingDataByType(INT32 nType)
{
    LOG_PROCESS_ERROR(nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM);

    return &m_FunBuilding[nType];
Exit0:
    return NULL;
}

MS_G_B_DATA* CMsGuild::GetBuildingDataByNpcID(UINT64 qwNameID)
{
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        if (m_FunBuilding[i].qwNameID == qwNameID)
            return &m_FunBuilding[i];
    }
    {
        G_BUILDING_MAP::iterator    iter = m_BuildingList.find(qwNameID);
        PROCESS_ERROR(iter != m_BuildingList.end());

        return &(iter->second);
    }
Exit0:
    return NULL;
}

BOOL CMsGuild::IsExistBuilding(UINT64 qwNameID)
{
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        if (m_FunBuilding[i].qwNameID == qwNameID)
            return TRUE;
    }

    {
        G_BUILDING_MAP::iterator    iter = m_BuildingList.find(qwNameID);
        PROCESS_ERROR(iter != m_BuildingList.end());
    }
    return TRUE;
Exit0:
    return FALSE;
}

GUILD_UPGRADE_TRIGGER* CMsGuild::GetUpgradeBuildingByNpcID(UINT64 qwNameID)
{
    for (INT32 i = 0; i < m_nUpgradeBuildingNum; ++i)
    {
        LOG_PROCESS_ERROR(m_pUpgradeBuilding[i]);
        if (m_pUpgradeBuilding[i]->qwNameID == qwNameID)
        {
            return m_pUpgradeBuilding[i];
        }
    }
Exit0:
    return NULL;
}

BOOL CMsGuild::IsExistUpgradeBuilding(UINT64 qwNameID)
{
    for (INT32 i = 0; i < m_nUpgradeBuildingNum; ++i)
    {
        LOG_PROCESS_ERROR(m_pUpgradeBuilding[i]);
        if (m_pUpgradeBuilding[i]->qwNameID == qwNameID)
        {
            return TRUE;
        }
    }
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddUpgradeBuilding(UINT64 qwNameID, GUILD_UPGRADE_TRIGGER *pTri)
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(pTri);
    LOG_PROCESS_ERROR(m_nUpgradeBuildingNum < GUILD_MAX_UPGRADE_BUILDING_COUNT);

    nRetCode = IsExistUpgradeBuilding(qwNameID);
    LOG_PROCESS_ERROR(nRetCode == 0);

    m_pUpgradeBuilding[m_nUpgradeBuildingNum++] = pTri;

    return TRUE;
Exit0:
    return FALSE;
}

void CMsGuild::DelUpgradeBuilding(UINT64 qwNameID)
{
    INT32   nIndex = 0;
    for (nIndex = 0; nIndex < m_nUpgradeBuildingNum; ++nIndex)
    {
        LOG_PROCESS_ERROR(m_pUpgradeBuilding[nIndex]);
        if (m_pUpgradeBuilding[nIndex]->qwNameID == qwNameID)
            break;
    }
    
    m_pUpgradeBuilding[nIndex] = m_pUpgradeBuilding[--m_nUpgradeBuildingNum];
Exit0:
    return;
}

INT32 CMsGuild::BeginUpgradeWelfareBuffer(UINT32 dwRoleID, INT32 nType)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_G_B_DATA *pBuilding = NULL;
    const GUILD_WELFARE_DATA  *pWelfareTemplate = NULL;
    GUILD_UPGRADE_TRIGGER   *pTrigger = NULL;
    CManageServer   &ms = CManageServer::GetInstance();

    LOG_PROCESS_ERROR_RET_CODE(0 < nType && nType < GUILD_MAX_WELFARE_AURA_COUNT, errGuildWelfareAuraIDError);
    pWelfareTemplate = GetGuildWelfareData(nType, m_arrWelfareAuraLvl[nType]);
    LOG_PROCESS_ERROR_RET_CODE(pWelfareTemplate, errGuildConfigError);

    // check
    LOG_PROCESS_ERROR_RET_CODE(GetMoney() >= pWelfareTemplate->nUpgradeMoneyConsumption, errGuildNoMoreGuildMoney);
    LOG_PROCESS_ERROR_RET_CODE(GetWood() >= pWelfareTemplate->nUpgradeWoodConsumption, errGuildNoMoreWood);
    LOG_PROCESS_ERROR_RET_CODE(GetHerb() >= pWelfareTemplate->nUpgradeHerbConsumption, errGuildNoMoreHerb);
    LOG_PROCESS_ERROR_RET_CODE(GetSilver() >= pWelfareTemplate->nUpgradeSilverConsumption, errGuildNoMoreCrystal);
    LOG_PROCESS_ERROR_RET_CODE(GetCryStal() >= pWelfareTemplate->nUpgradeCrystalConsumption, errGuildNoMoreSilver);
    LOG_PROCESS_ERROR_RET_CODE(pWelfareTemplate->nMaxLevel != m_arrWelfareAuraLvl[nType], errGuildMaxLevel);
    LOG_PROCESS_ERROR_RET_CODE(0 <= m_nUpgradeWelfareAuraCount 
        && m_nUpgradeWelfareAuraCount < GUILD_MAX_UPGRADE_AURA_COUNT, errGuildUpgradingCountLimit);

    if (pWelfareTemplate->dwConditionBuildingID > GUILD_B_NONE)
    {
        LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < pWelfareTemplate->dwConditionBuildingID 
            && pWelfareTemplate->dwConditionBuildingID < GUILD_B_FUNATIONAL_NUM, errGuildConfigError);
        pBuilding = GetFunBuildingDataByType(pWelfareTemplate->dwConditionBuildingID);
        LOG_PROCESS_ERROR_RET_CODE(pBuilding, errGuildBaseBuildingFailed);
        LOG_PROCESS_ERROR_RET_CODE(pBuilding->nLevel >= pWelfareTemplate->nConditionBuildingLvl, errGuildBaseBuildingFailed);
    }

    for (INT32 i = 0; i < m_nUpgradeWelfareAuraCount; ++i)
    {
        LOG_PROCESS_ERROR_RET_CODE(m_paWelfareUpgradeData[i], errGuildFailed);
        if (m_paWelfareUpgradeData[i]->nType == nType)
        {
            LOG_PROCESS_ERROR_RET_CODE(FALSE, errGuildWelfareUpgrading);
        }
    }

    // consume
    nRetCode = SubMoney(pWelfareTemplate->nUpgradeMoneyConsumption, GUILD_CHEST_W_UPGRADE, FALSE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreGuildMoney);
    nRetCode = SubWood(pWelfareTemplate->nUpgradeWoodConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreWood);
    nRetCode = SubHerb(pWelfareTemplate->nUpgradeHerbConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreHerb);
    nRetCode = SubCrystal(pWelfareTemplate->nUpgradeCrystalConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreCrystal);
    nRetCode = SubSilver(pWelfareTemplate->nUpgradeSilverConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreSilver);

    // begin upgrade
    pTrigger = CMsGuildMgr::GetInstance().CreateUpgradeTriggerData();
    LOG_PROCESS_ERROR_RET_CODE(pTrigger, errGuildFailed);

    pTrigger->dwGuildID = m_dwID;
    pTrigger->nType = nType;
    pTrigger->nIndex = m_nUpgradeWelfareAuraCount;
    pTrigger->dwRoleID   = dwRoleID;
    pTrigger->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + pWelfareTemplate->nUpgradeTime;
	pTrigger->nTriggerType = tttInvalid;
    pTrigger->fp = CB_UpgradeWelfareTimeout;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildFailed);

    m_paWelfareUpgradeData[m_nUpgradeWelfareAuraCount] = pTrigger;
    ++m_nUpgradeWelfareAuraCount;

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_W_UPGRADE, dwRoleID, nType, m_arrWelfareAuraLvl[nType], 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    // send to client
    nRetCode = CMessageHandler::GetInstance().DoClientGuildUpgradeWelfareNtf(this, nType, pWelfareTemplate->nUpgradeTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    if (pTrigger)
    {
        nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(pTrigger);
        LOG_CHECK_ERROR(nRetCode);
    }
    return nResult;
}

BOOL CMsGuild::CompleteUpgradeWelfareBuffer(GUILD_UPGRADE_TRIGGER *pTrigger)
{
    INT32       nRetCode = 0;
    UINT32      dwRoleID = 0;
    const GUILD_WELFARE_DATA  *pTempData = NULL;
    CManageServer   &ms = CManageServer::GetInstance();
    INT32       nIndex = 0;
    INT32       nType = 0;

    LOG_PROCESS_ERROR(pTrigger);
    LOG_PROCESS_ERROR(0 < pTrigger->nType && pTrigger->nType < GUILD_MAX_WELFARE_AURA_COUNT);
    LOG_PROCESS_ERROR(pTrigger->nIndex >= 0 && pTrigger->nIndex < GUILD_MAX_UPGRADE_AURA_COUNT);
    LOG_PROCESS_ERROR(m_paWelfareUpgradeData[pTrigger->nIndex] == pTrigger);

    nIndex = pTrigger->nIndex;
    nType = pTrigger->nType;

    pTempData = GetGuildWelfareData(nType, m_arrWelfareAuraLvl[nType]);
    LOG_PROCESS_ERROR(pTempData);

    ++m_arrWelfareAuraLvl[nType];

    dwRoleID = m_paWelfareUpgradeData[nIndex]->dwRoleID;
    nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(m_paWelfareUpgradeData[nIndex]);
    LOG_CHECK_ERROR(nRetCode);

    m_paWelfareUpgradeData[nIndex] = m_paWelfareUpgradeData[--m_nUpgradeWelfareAuraCount];

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_CHECK_ERROR(nRetCode);

    // add history
    nRetCode = AddGuildEvent(GUILD_HE_TYPE_SKILL_LVLUP, dwRoleID, nType, m_arrWelfareAuraLvl[nType], 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildUpdateWelfareID(m_dwID, nType, m_arrWelfareAuraLvl[nType]);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildUpdateWelfareNtf(this, nType, m_arrWelfareAuraLvl[nType]);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

//INT32 CMsGuild::CanUpgradeWelfareBuffer(UINT32 dwBufferID)
//{
//    return errSuccess;
//}

INT32 CMsGuild::CancelUpgradeWelfareBuffer(INT32 nType)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    const GUILD_WELFARE_DATA  *pTempData = NULL;
    CManageServer   &ms = CManageServer::GetInstance();

    LOG_PROCESS_ERROR_RET_CODE(0 < nType && nType < GUILD_MAX_WELFARE_AURA_COUNT, errGuildWelfareAuraIDError);
    pTempData = GetGuildWelfareData(nType, m_arrWelfareAuraLvl[nType]);
    LOG_PROCESS_ERROR_RET_CODE(pTempData, errGuildConfigError);

    for (INT32 i = 0; i < m_nUpgradeWelfareAuraCount; ++i)
    {
        LOG_PROCESS_ERROR_RET_CODE(m_paWelfareUpgradeData[i], errGuildInternalError);
        if (m_paWelfareUpgradeData[i]->nType == nType)
        {
            nRetCode = AddMoney(pTempData->nUpgradeMoneyConsumption * g_GuildConfig.nWelfareBufferUpgradeRefundPercent / 100, GUILD_CHEST_CANCEL_WELFARE_UPGRADE, FALSE);
            LOG_CHECK_ERROR(nRetCode);
            nRetCode = AddWood(pTempData->nUpgradeWoodConsumption * g_GuildConfig.nWelfareBufferUpgradeRefundPercent / 100);
            LOG_CHECK_ERROR(nRetCode);
            nRetCode = AddHerb(pTempData->nUpgradeHerbConsumption * g_GuildConfig.nWelfareBufferUpgradeRefundPercent / 100);
            LOG_CHECK_ERROR(nRetCode);
            nRetCode = AddCrystal(pTempData->nUpgradeCrystalConsumption * g_GuildConfig.nWelfareBufferUpgradeRefundPercent / 100);
            LOG_CHECK_ERROR(nRetCode);
            nRetCode = AddSilver(pTempData->nUpgradeSilverConsumption * g_GuildConfig.nWelfareBufferUpgradeRefundPercent / 100);
            LOG_CHECK_ERROR(nRetCode);
            
            nRetCode = CTimeTrigger::GetInstance().DeleteTrigger(m_paWelfareUpgradeData[i]);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(m_paWelfareUpgradeData[i]);
            LOG_CHECK_ERROR(nRetCode);

            m_paWelfareUpgradeData[i] = m_paWelfareUpgradeData[--m_nUpgradeWelfareAuraCount];

            // update mysql
            nRetCode = SqlUpdateGuild(this);
            LOG_CHECK_ERROR(nRetCode);

            break;
        }
    }

    //nRetCode = CMessageHandler::GetInstance().DoClientGuildUpdateWelfareNtf(this, nType, m_arrWelfareAuraLvl[nType]);
    //LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode= CMessageHandler::GetInstance().DoClientGuildCancelWelfareUpgradeNtf(this, nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::CreateBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    const GUILD_BUILDING_DATA     *pTemplateData = NULL;
    GUILD_UPGRADE_TRIGGER   *pTrigger   = NULL;
    CManageServer   &ms = CManageServer::GetInstance();
    MS_G_B_DATA     *pBaseBuilding = NULL;
    INT32       nNum = 0;
    INT32       nUpgradeNum = 0;

    LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < nType && nType < GUILD_B_NUM, errGuildBuildingIDError);

    nRetCode = IsExistBuilding(qwNpcNameID);
    LOG_PROCESS_ERROR_RET_CODE(!nRetCode, errGuildBuildingAlreadyExist);

    pTemplateData = GetGuildBuildingData(nType, 0);
    LOG_PROCESS_ERROR_RET_CODE(pTemplateData, errGuildBuildingConfigError);

    nNum = GetBuildingNumByType(nType);
    LOG_PROCESS_ERROR_RET_CODE(nNum >= 0, errGuildBMaxNum);
    nUpgradeNum = GetUpgradeBuildingNumByType(nType);
    LOG_PROCESS_ERROR_RET_CODE(nUpgradeNum >= 0, errGuildBMaxNum);
    LOG_PROCESS_ERROR_RET_CODE(nUpgradeNum + nNum < pTemplateData->nMaxNum, errGuildBMaxNum);

    LOG_PROCESS_ERROR_RET_CODE(GetMoney() >= pTemplateData->nMoneyConsumption, errGuildNoMoreGuildMoney);
    LOG_PROCESS_ERROR_RET_CODE(GetWood() >= pTemplateData->nWoodConsumption, errGuildNoMoreWood);
    LOG_PROCESS_ERROR_RET_CODE(GetHerb() >= pTemplateData->nHerbConsumption, errGuildNoMoreHerb);
    LOG_PROCESS_ERROR_RET_CODE(GetSilver() >= pTemplateData->nSilverConsumption, errGuildNoMoreSilver);
    LOG_PROCESS_ERROR_RET_CODE(GetCryStal() >= pTemplateData->nCrystalConsumption, errGuildNoMoreCrystal);

    if (pTemplateData->dwConditionBuildingID > GUILD_B_NONE)
    {
        LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < pTemplateData->dwConditionBuildingID 
            && pTemplateData->dwConditionBuildingID < GUILD_B_FUNATIONAL_NUM, errGuildConfigError);
        pBaseBuilding = GetFunBuildingDataByType(pTemplateData->dwConditionBuildingID);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding, errGuildBaseBuildingFailed);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding->nLevel >= pTemplateData->nConditionBuildingLvl, errGuildBaseBuildingFailed);
    }

    nRetCode = IsExistUpgradeBuilding(qwNpcNameID);
    LOG_PROCESS_ERROR_RET_CODE(!nRetCode, errGuildBuildingUpgrading);

    // consume
    nRetCode = SubMoney(pTemplateData->nMoneyConsumption, GUILD_CHEST_B_UPGRADE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreGuildMoney);
    nRetCode = SubWood(pTemplateData->nWoodConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreWood);
    nRetCode = SubHerb(pTemplateData->nHerbConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreHerb);
    nRetCode = SubCrystal(pTemplateData->nCrystalConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreCrystal);
    nRetCode = SubSilver(pTemplateData->nSilverConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreSilver);

    // begin upgrade
    pTrigger = CMsGuildMgr::GetInstance().CreateUpgradeTriggerData();
    LOG_PROCESS_ERROR_RET_CODE(pTrigger, errGuildInternalError);

    pTrigger->dwGuildID = m_dwID;
    pTrigger->nType = nType;
    pTrigger->qwNameID = qwNpcNameID;
    pTrigger->dwRoleID = dwRoleID;
    pTrigger->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + pTemplateData->nUpgradeTime;
	pTrigger->nTriggerType = tttInvalid;
	pTrigger->fp = CB_UpgradeBuildingTimeout;

    nRetCode = AddUpgradeBuilding(qwNpcNameID, pTrigger);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_CREATE_BUILDING, dwRoleID, nType, GUILD_ATTR_MONEY, pTemplateData->nMoneyConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_CREATE_BUILDING, dwRoleID, nType, GUILD_ATTR_HERB, pTemplateData->nHerbConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_CREATE_BUILDING, dwRoleID, nType, GUILD_ATTR_WOOD, pTemplateData->nWoodConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_CREATE_BUILDING, dwRoleID, nType, GUILD_ATTR_CRYSTAL, pTemplateData->nCrystalConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_CREATE_BUILDING, dwRoleID, nType, GUILD_ATTR_SILVER, pTemplateData->nSilverConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    // send to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildUpgradeBuildingNtf(GetID(), qwNpcNameID, nType, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    // send to client
    nRetCode = CMessageHandler::GetInstance().DoClientGuildUpgradeBuildingNtf(this, qwNpcNameID, nType, pTemplateData->nUpgradeTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    if (pTrigger)
    {
        nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(pTrigger);
        LOG_CHECK_ERROR(nRetCode);

        DelUpgradeBuilding(qwNpcNameID);
    }

    return nResult;
}

INT32 CMsGuild::BeginUpgradeBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    const GUILD_BUILDING_DATA     *pTemplateData = NULL;
    GUILD_UPGRADE_TRIGGER   *pTrigger   = NULL;
    CManageServer   &ms = CManageServer::GetInstance();
    MS_G_B_DATA     *pBuildingData  = NULL;
    MS_G_B_DATA     *pBaseBuilding = NULL;
    INT32       nNum = 0;

    LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < nType && nType < GUILD_B_NUM, errGuildBuildingIDError);

    pBuildingData = GetBuildingDataByNpcID(qwNpcNameID);
    LOG_PROCESS_ERROR_RET_CODE(pBuildingData, errGuildBuildingNotExist);

    pTemplateData = GetGuildBuildingData(nType, pBuildingData->nLevel);
    LOG_PROCESS_ERROR_RET_CODE(pTemplateData, errGuildBuildingConfigError);

    LOG_PROCESS_ERROR(pTemplateData->nMaxLevel <= GUILD_MAX_BUILDING_LEVEL);
    LOG_PROCESS_ERROR_RET_CODE(pBuildingData->nLevel < pTemplateData->nMaxLevel, errGuildBuildingMaxLevel);

    LOG_PROCESS_ERROR_RET_CODE(m_nUpgradeBuildingNum < GUILD_MAX_UPGRADE_BUILDING_COUNT, errGuildUpgradingCountLimit);

    LOG_PROCESS_ERROR_RET_CODE(GetMoney() >= pTemplateData->nMoneyConsumption, errGuildNoMoreGuildMoney);
    LOG_PROCESS_ERROR_RET_CODE(GetWood() >= pTemplateData->nWoodConsumption, errGuildNoMoreWood);
    LOG_PROCESS_ERROR_RET_CODE(GetHerb() >= pTemplateData->nHerbConsumption, errGuildNoMoreHerb);
    LOG_PROCESS_ERROR_RET_CODE(GetSilver() >= pTemplateData->nSilverConsumption, errGuildNoMoreSilver);
    LOG_PROCESS_ERROR_RET_CODE(GetCryStal() >= pTemplateData->nCrystalConsumption, errGuildNoMoreCrystal);

    if (pTemplateData->dwConditionBuildingID > GUILD_B_NONE)
    {
        LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < pTemplateData->dwConditionBuildingID 
            && pTemplateData->dwConditionBuildingID < GUILD_B_FUNATIONAL_NUM, errGuildConfigError);
        pBaseBuilding = GetFunBuildingDataByType(pTemplateData->dwConditionBuildingID);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding, errGuildBaseBuildingFailed);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding->nLevel >= pTemplateData->nConditionBuildingLvl, errGuildBaseBuildingFailed);
    }

    nRetCode = IsExistUpgradeBuilding(qwNpcNameID);
    LOG_PROCESS_ERROR_RET_CODE(!nRetCode, errGuildBuildingUpgrading);

    // consume
    nRetCode = SubMoney(pTemplateData->nMoneyConsumption, GUILD_CHEST_B_UPGRADE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreGuildMoney);
    nRetCode = SubWood(pTemplateData->nWoodConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreWood);
    nRetCode = SubHerb(pTemplateData->nHerbConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreHerb);
    nRetCode = SubCrystal(pTemplateData->nCrystalConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreCrystal);
    nRetCode = SubSilver(pTemplateData->nSilverConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildNoMoreSilver);

    // begin upgrade
    pTrigger = CMsGuildMgr::GetInstance().CreateUpgradeTriggerData();
    LOG_PROCESS_ERROR_RET_CODE(pTrigger, errGuildInternalError);

    pTrigger->dwGuildID = m_dwID;
    pTrigger->nType = nType;
    pTrigger->dwRoleID = dwRoleID;
    pTrigger->qwNameID = qwNpcNameID;
    pTrigger->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + pTemplateData->nUpgradeTime;
	pTrigger->nTriggerType = tttInvalid;
	pTrigger->fp = CB_UpgradeBuildingTimeout;

    nRetCode = AddUpgradeBuilding(qwNpcNameID, pTrigger);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildInternalError);

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_B_UPGRADE, dwRoleID, nType, GUILD_ATTR_MONEY, pTemplateData->nMoneyConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_B_UPGRADE, dwRoleID, nType, GUILD_ATTR_HERB, pTemplateData->nHerbConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_B_UPGRADE, dwRoleID, nType, GUILD_ATTR_WOOD, pTemplateData->nWoodConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_B_UPGRADE, dwRoleID, nType, GUILD_ATTR_CRYSTAL, pTemplateData->nCrystalConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_B_UPGRADE, dwRoleID, nType, GUILD_ATTR_SILVER, pTemplateData->nSilverConsumption);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    // send to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildUpgradeBuildingNtf(GetID(), qwNpcNameID, nType, pBuildingData->nLevel);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    // send to client
    nRetCode = CMessageHandler::GetInstance().DoClientGuildUpgradeBuildingNtf(this, qwNpcNameID, nType, pTemplateData->nUpgradeTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    if (pTrigger)
    {
        nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(pTrigger);
        LOG_CHECK_ERROR(nRetCode);

        DelUpgradeBuilding(qwNpcNameID);
    }

    return nResult;
}

BOOL CMsGuild::CompleteUpgradeBuilding(GUILD_UPGRADE_TRIGGER* pData)
{
    INT32       nRetCode = 0;
    UINT32      dwRoleID = 0;
    CManageServer   &ms = CManageServer::GetInstance();
    MS_G_B_DATA     *pBuildingData = NULL;
    INT32       nType = 0;
    INT32       nLevel = 0;
    UINT64      qwNameID = 0;

    LOG_PROCESS_ERROR(pData);
    LOG_PROCESS_ERROR(GUILD_B_NONE < pData->nType && pData->nType < GUILD_B_NUM);
    nRetCode = IsExistUpgradeBuilding(pData->qwNameID);
    LOG_PROCESS_ERROR(nRetCode);

    nType       = pData->nType;
    dwRoleID    = pData->dwRoleID;
    qwNameID    = pData->qwNameID;


    if (nType < GUILD_B_FUNATIONAL_NUM)
    {
        pBuildingData = GetFunBuildingDataByType(nType);
        LOG_PROCESS_ERROR(pBuildingData);

        pBuildingData->qwNameID = qwNameID;
        pBuildingData->nType = nType;
        ++pBuildingData->nLevel;
        nLevel = pBuildingData->nLevel;
    }
    else
    {
        pBuildingData = GetBuildingDataByNpcID(qwNameID);
        if (pBuildingData)
        {
            ++pBuildingData->nLevel;
            nLevel = pBuildingData->nLevel;
        }
        else
        {
            nLevel = 1;
            m_BuildingList.insert(std::make_pair(qwNameID, MS_G_B_DATA(qwNameID, nType, nLevel)));
        }
    }
    
    nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(pData);
    LOG_CHECK_ERROR(nRetCode);

    DelUpgradeBuilding(qwNameID);

    nRetCode = UpdateBuildingProsperity(nType, nLevel);
    LOG_CHECK_ERROR(nRetCode);

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_CHECK_ERROR(nRetCode);

    // add history
    if (1 == nLevel)
    {
        nRetCode = AddGuildEvent(GUILD_HE_TYPE_BUILDING_CREATE, dwRoleID, nType, nLevel, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = AddGuildEvent(GUILD_HE_TYPE_BUILDING_LVLUP, dwRoleID, nType, nLevel, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoGSGuildUpdateBuilding(GetID(), qwNameID, nType, nLevel);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildUpdateBuildingNtf(this, qwNameID, nType);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMsGuild::CancelUpgradeBuilding(UINT64 qwNpcNameID, INT32 nType)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    INT32       nLevel = 0;
    MS_G_B_DATA *pBuildingData = NULL;
    GUILD_UPGRADE_TRIGGER   *pUBuildingData = NULL;
    const GUILD_BUILDING_DATA  *pTemplateData = NULL;
    CManageServer   &ms = CManageServer::GetInstance();

    LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < nType && nType < GUILD_B_NUM, errGuildBuildingIDError);
    
    pUBuildingData = GetUpgradeBuildingByNpcID(qwNpcNameID);
    LOG_PROCESS_ERROR_RET_CODE(pUBuildingData, errGuildUpgradeDataNotExist);

    pBuildingData = GetBuildingDataByNpcID(qwNpcNameID);
    if (pBuildingData)
        nLevel = pBuildingData->nLevel;
    else
        nLevel = 0;

    pTemplateData = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR_RET_CODE(pTemplateData, errGuildBuildingIDError);

    nRetCode = AddMoney(pTemplateData->nMoneyConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100, GUILD_CHEST_CANCEL_BUILDING_UPGRADE, FALSE);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddWood(pTemplateData->nWoodConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddHerb(pTemplateData->nHerbConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddCrystal(pTemplateData->nCrystalConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddSilver(pTemplateData->nSilverConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CTimeTrigger::GetInstance().DeleteTrigger(pUBuildingData);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(pUBuildingData);
    LOG_CHECK_ERROR(nRetCode);

    DelUpgradeBuilding(qwNpcNameID);

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildCancelBuildingUpgradeNtf(this, qwNpcNameID, nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::DelBuilding(UINT32 dwRoleID, UINT64 qwNpcNameID, INT32 nType)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CManageServer   &ms = CManageServer::GetInstance();
    MS_G_B_DATA *pBuildingData = NULL;
    GUILD_UPGRADE_TRIGGER   *pUBuildingData = NULL;
    const GUILD_BUILDING_DATA  *pTemplateData = NULL;
    INT32       nLevel  = 0;
    INT32       nActualLevel = 0;

    LOG_PROCESS_ERROR_RET_CODE(GUILD_B_NONE < nType && nType < GUILD_B_NUM, errGuildBuildingIDError);
    LOG_PROCESS_ERROR_RET_CODE(nType != GUILD_B_MAIN && nType != GUILD_B_DEITY && nType != GUILD_B_BUSINESS && nType != GUILD_B_WONDER, errGuildCannotDel);
    nRetCode = IsExistBuilding(qwNpcNameID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildClientParamError);

    pBuildingData = GetBuildingDataByNpcID(qwNpcNameID);
    if (pBuildingData)
        nLevel = pBuildingData->nLevel;
    else
        nLevel = 0;
    nActualLevel = nLevel;

    pTemplateData = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR_RET_CODE(pTemplateData, errGuildBuildingIDError);

    pUBuildingData = GetUpgradeBuildingByNpcID(qwNpcNameID);
    if (pUBuildingData)
    {
        nRetCode = AddMoney(pTemplateData->nMoneyConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100, GUILD_CHEST_CANCEL_BUILDING_UPGRADE, FALSE);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = AddWood(pTemplateData->nWoodConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = AddHerb(pTemplateData->nHerbConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = AddCrystal(pTemplateData->nCrystalConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = AddSilver(pTemplateData->nSilverConsumption * g_GuildConfig.nBuildingUpgradeRefundPercent / 100);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CTimeTrigger::GetInstance().DeleteTrigger(pUBuildingData);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CMsGuildMgr::GetInstance().DeleteUpgradeTriggerData(pUBuildingData);
        LOG_CHECK_ERROR(nRetCode);

        DelUpgradeBuilding(qwNpcNameID);

        nRetCode = CMessageHandler::GetInstance().DoClientGuildCancelBuildingUpgradeNtf(this, qwNpcNameID, nType);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    }

    nRetCode = AddMoney(pTemplateData->nTotalMoneyConsumption * g_GuildConfig.nDelBuildingRefundPercent / 100, GUILD_CHEST_DEL_BUILDING, FALSE);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddWood(pTemplateData->nTotalWoodConsumption * g_GuildConfig.nDelBuildingRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddHerb(pTemplateData->nTotalHerbConsumption * g_GuildConfig.nDelBuildingRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddCrystal(pTemplateData->nTotalCrystalConsumption * g_GuildConfig.nDelBuildingRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = AddSilver(pTemplateData->nTotalSilverConsumption * g_GuildConfig.nDelBuildingRefundPercent / 100);
    LOG_CHECK_ERROR(nRetCode);

    _DelBuilding(qwNpcNameID);

    nRetCode = SetProsperity(GetProsperity() - pTemplateData->nProsperity);
    LOG_CHECK_ERROR(nRetCode);

    // update mysql
    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSqlError);

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_BUILDING_DEL, dwRoleID, nType, nActualLevel, 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildDelBuilding(GetID(), qwNpcNameID, nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildDelBuildingNtf(this, qwNpcNameID, nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

BOOL CMsGuild::_IsInAllianceAppList(UINT32 dwApplicantID)
{
    PROCESS_ERROR(CApplicationMgr::GetInstance().Find(dwApplicantID, m_dwID));
    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMsGuild::_GetAllianceAppTimeType(UINT32 dwApplicantID)
{
    DIPLOMAT_APP_NODE    *pNode = (DIPLOMAT_APP_NODE*)CApplicationMgr::GetInstance().Find(dwApplicantID, m_dwID);
    LOG_PROCESS_ERROR(pNode);
    LOG_PROCESS_ERROR(pNode->nType == EN_DIPLOMAT_APPLICATION);
    return pNode->nTimeType;
Exit0:
    return -1;
}

BOOL CMsGuild::_AddAllianceApplication(UINT32 dwApplicantID, INT32 nTimeType)
{
    DIPLOMAT_APP_NODE    *pNode = CMsGuildMgr::GetInstance().CreateDiplomatApp();
    LOG_PROCESS_ERROR(pNode);

    pNode->dwID_1 = dwApplicantID;
    pNode->dwID_2 = m_dwID;
    pNode->dwWashOutTime = g_GuildConfig.nDiplomatAppLastTime + CManageServer::GetInstance().GetGameTime();
    pNode->nTimeType = nTimeType;
    pNode->nType = EN_DIPLOMAT_APPLICATION;
    pNode->fp = CB_DiplomatAppTimeout;

    LOG_PROCESS_ERROR(CApplicationMgr::GetInstance().InsertApplication(pNode));

    return TRUE;
Exit0:
    if (pNode)
    {
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteDiplomatApp(pNode));
    }
    return FALSE;
}

BOOL CMsGuild::_DelAllianceApplication(UINT32 dwApplicantID)
{
    INT32   nResult = errUnknownError;
    INT32   nRetCode = 0;
    DIPLOMAT_APP_NODE   *pNode = NULL;

    pNode = (DIPLOMAT_APP_NODE*)CApplicationMgr::GetInstance().Find(dwApplicantID, m_dwID);
    LOG_PROCESS_ERROR(pNode);
    LOG_PROCESS_ERROR(pNode->nType == EN_DIPLOMAT_APPLICATION);

    nRetCode = CApplicationMgr::GetInstance().RemoveApplication(pNode);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().DeleteDiplomatApp(pNode);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CMsGuild::_DelBuilding(UINT64 qwNpcNameID)
{
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        if (m_FunBuilding[i].qwNameID == qwNpcNameID)
        {
            m_FunBuilding[i].qwNameID = 0;
            m_FunBuilding[i].nType = 0;
            m_FunBuilding[i].nLevel = 0;
            m_FunBuilding[i].dwReviveTime = 0;
        }
    }

    m_BuildingList.erase(qwNpcNameID);
}

BOOL CMsGuild::IsAlliance(UINT32 dwGuildID)
{
    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        if (m_arrAlliance[i].dwGuildID == dwGuildID)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CMsGuild::IsEnemy(UINT32 dwGuildID)
{
    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        if (m_arrEnemy[i].dwGuildID == dwGuildID)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CMsGuild::AddAlliance(UINT32  dwGuildID, UINT32 dwEndTime)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGuildFailed;

    m_arrAlliance[m_nAllianceNum].dwGuildID = dwGuildID;
    m_arrAlliance[m_nAllianceNum++].dwEndTime = dwEndTime;

    nRetCode = SqlUpdateGuild(this);
    LOG_CHECK_ERROR(nRetCode);

    //AddGuildEvent(GUILD_HE_TYPE_UNION, dwGuildID, 0, 0, 0);   todo_tq
    CMSGuildAchiMgr::GetInstance().RecordAchiCondition(GUILD_ACHI_ALLIANCE, this);

    return TRUE;
}

BOOL CMsGuild::DelAlliance(UINT32 dwGuildID)
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(m_nAllianceNum >= 0 && m_nAllianceNum <= GUILD_MAX_ALLIANCE_NUM);
    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        if (dwGuildID == m_arrAlliance[i].dwGuildID)
        {
            m_arrAlliance[i] = m_arrAlliance[--m_nAllianceNum];

            nRetCode = CMessageHandler::GetInstance().DoClientGuildDelDiplomatNtf(GUILD_DIPLOMAT_ALLIANCE, this, dwGuildID);
            LOG_CHECK_ERROR(nRetCode);
            break;
        }
    }

    nRetCode = SqlUpdateGuild(this);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddEnemy(UINT32 dwGuildID, UINT32 dwEndTime)
{
    INT32       nRetCode =  0;
    INT32       nResult = errGuildFailed;

    m_arrEnemy[m_nEnemyNum].dwGuildID = dwGuildID;
    m_arrEnemy[m_nEnemyNum++].dwEndTime = dwEndTime;

    nRetCode = SqlUpdateGuild(this);
    LOG_CHECK_ERROR(nRetCode);

    //AddGuildEvent(GUILD_HE_TYPE_ENEMY, dwGuildID, 0, 0, 0);       todo_tq
    CMSGuildAchiMgr::GetInstance().RecordAchiCondition(GUILD_ACHI_ENEMY, this);

    return TRUE;
}

void CMsGuild::DelEnemy(UINT32 dwGuildID)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(m_nEnemyNum >= 0 && m_nEnemyNum <= GUILD_MAX_ENEMY_NUM);

    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        if (dwGuildID == m_arrEnemy[i].dwGuildID)
        {
            m_arrEnemy[i] = m_arrEnemy[--m_nEnemyNum];

            nRetCode = CMessageHandler::GetInstance().DoClientGuildDelDiplomatNtf(GUILD_DIPLOMAT_ENEMY, this, dwGuildID);
            LOG_CHECK_ERROR(nRetCode);

            break;
        }
    }

    nRetCode = SqlUpdateGuild(this);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return ;
}

// guild war
BOOL CMsGuild::SetAim(UINT32 dwGuildID, BOOL bAttackCD)
{
    INT32       nRetCode = 0;

    m_dwAimID = dwGuildID;

    if (bAttackCD)
    {
        m_dwAttackCD = CManageServer::GetInstance().GetGameTime() + g_GuildConfig.nGuildWarAttackCD;
        nRetCode = CMessageHandler::GetInstance().DoClientGuildSyncWarCDNtf(this, m_dwAttackCD);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SetIntruder(UINT32 dwGuildID)
{
    m_dwIntruderID = dwGuildID;
    m_dwProtectionTime = CManageServer::GetInstance().GetGameTime() + g_GuildConfig.nGuildWarProtectionTime;
    return TRUE;
}

BOOL CMsGuild::SetRevengeObj(UINT32 dwGuildID)
{
    m_dwRevengeGuildID = dwGuildID;
    return TRUE;
}

BOOL CMsGuild::SetProtectionTime(UINT32 dwTime)
{
    INT32 nRetCode = 0;
    INT32 nShowTime = 0;

    m_dwProtectionTime = dwTime;

    if (dwTime > CManageServer::GetInstance().GetGameTime())
        nShowTime = dwTime - CManageServer::GetInstance().GetGameTime();
    else
        nShowTime = 0;

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_PROTECTION_TIME, nShowTime);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CMsGuild::SetWarAttackCD(UINT32 dwTime)
{
    INT32 nRetCode = 0;
    INT32 nShowTime = 0;
    m_dwAttackCD = dwTime;

    if (dwTime > CManageServer::GetInstance().GetGameTime())
        nShowTime = dwTime - CManageServer::GetInstance().GetGameTime();
    else
        nShowTime = 0;
    nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_ATTACK_CD, nShowTime);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CMsGuild::BeginMatchWar(UINT32 dwGuildID)
{
    if (m_dwMatchWarTime / 86400 == CManageServer::GetInstance().GetGameTime() / 86400)
    {
        LOG_PROCESS_ERROR(m_nMatchWarCount < g_GuildConfig.nGuildMatchWarCountLimit);
        ++m_nMatchWarCount;
    }
    else
    {
        m_dwMatchWarTime = CManageServer::GetInstance().GetGameTime();
        m_nMatchWarCount = 1;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::CanBeginNewMatchWar()
{
    if (m_dwMatchWarTime / 86400 == CManageServer::GetInstance().GetGameTime() / 86400)
    {
        PROCESS_ERROR(m_nMatchWarCount < g_GuildConfig.nGuildMatchWarCountLimit);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::DisableBuilding(UINT64 qwNameID, UINT32 dwReviveTime)
{
    INT32       nRetCode = 0;
    G_B_REVIVE_TRIGGER       *pTrigger = NULL;
    MS_G_B_DATA *pData = GetBuildingDataByNpcID(qwNameID);
    LOG_PROCESS_ERROR(pData);

    pData->dwReviveTime = dwReviveTime;

    pTrigger = CMsGuildMgr::GetInstance().NewBGReviveTrigger();
    LOG_PROCESS_ERROR(pTrigger);

    pTrigger->dwGuildID = GetID();
    pTrigger->qwNameID = qwNameID;
    pTrigger->dwWashoutTime = pData->dwReviveTime;
	pTrigger->nTriggerType = tttInvalid;
    pTrigger->fp = CB_BuildingEnable;

    nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pTrigger);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = AddGuildEvent(GUILD_HE_TYPE_BUILDING_DESTORY, pData->nType, 0, 0, 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::IsOneLevelBornBuilding(INT32 nType)
{
    INT32   nRetCode = 0;

    PROCESS_ERROR(nType != GUILD_B_MAIN && 
        nType != GUILD_B_HERB && 
        nType != GUILD_B_WOOD && 
        nType != GUILD_B_CRYSTAL && 
        nType != GUILD_B_SILVER && 
        nType != GUILD_B_GATE && 
        nType != GUILD_B_WALL);

    return TRUE;
Exit0:
    return FALSE;
}

UINT32 CMsGuild::AddDungeon(UINT32 dwTemplateID, UINT32 dwOwnerID, UINT32 dwLeaderID)
{
    INT32       nRetCode = 0;
    CManageServer   &ms = CManageServer::GetInstance();
    GUILD_DUNGEON   data;

    FilterDungeon();

    data.dwTemplateID = dwTemplateID;
    data.dwLeaderID = dwLeaderID;
    data.dwOwnerID = dwOwnerID;
    data.nCreateTime = CManageServer::GetInstance().GetGameTime();
    m_listDungeon.push_back(data);

    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR(nRetCode);

    return GetID();
Exit0:
    return 0;
}

BOOL CMsGuild::DelDungeon(UINT32 dwTemplateID)
{
    INT32       nRetCode = 0;
    CManageServer   &ms = CManageServer::GetInstance();

    FilterDungeon();
    for (GUILD_DUNGEON_LIST::iterator iter = m_listDungeon.begin(); iter != m_listDungeon.end(); ++iter)
    {
        if (iter->dwTemplateID == dwTemplateID)
        {
            m_listDungeon.erase(iter);
            break;
        }
    }

    nRetCode = SqlUpdateGuild(this);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::CheckSameDungeonType(UINT32 dwTemplateID)
{
    FilterDungeon();
    for (GUILD_DUNGEON_LIST::iterator iter = m_listDungeon.begin(); iter != m_listDungeon.end(); ++iter)
    {
        if (iter->dwTemplateID == dwTemplateID)
        {
            return FALSE;
        }
    }
    return TRUE;
}

GUILD_DUNGEON* CMsGuild::FindDunGeon(UINT32 dwTemplateID)
{
    FilterDungeon();

    for (GUILD_DUNGEON_LIST::iterator iter = m_listDungeon.begin(); iter != m_listDungeon.end(); ++iter)
    {
        if (iter->dwTemplateID == dwTemplateID)
        {
            return &(*iter);
        }
    }
    return NULL;
}

BOOL CMsGuild::FilterDungeon()
{
    if ((UINT32)(m_nDungeonUpdateTime + ONE_DAY_SECOND) > CManageServer::GetInstance().GetGameTime())
        return TRUE;

    m_nDungeonUpdateTime = CManageServer::GetInstance().GetGameTime() + get_time_zone();
    m_nDungeonUpdateTime /= ONE_DAY_SECOND;
    m_nDungeonUpdateTime *= ONE_DAY_SECOND;
    m_nDungeonUpdateTime -= get_time_zone();
    m_nDungeonUpdateTime += g_GuildConfig.nGuildDungeonUpdateTime;

    for (GUILD_DUNGEON_LIST::iterator iter = m_listDungeon.begin(); iter != m_listDungeon.end(); )
    {
        if (iter->nCreateTime < m_nDungeonUpdateTime)
        {
            iter = m_listDungeon.erase(iter);
        }
        else
            ++iter;
    }

    return TRUE;
}


BOOL CMsGuild::_DiplomatWashOut( UINT32 dwCurrentTime, UINT32 dwCheckTime ) 
{
    INT32   nRetCode = 0;
    UINT32  dwTargetGuildID = 0;
    INT32   nResult = errGuildFailed;

    UINT32  arrDelAlliance[GUILD_MAX_ALLIANCE_NUM];
    INT32   nDelAllianceNum = 0;
    UINT32  arrDelEnemy[GUILD_MAX_ENEMY_NUM];
    INT32   nDelEnemyNum = 0;

    LOG_PROCESS_ERROR(m_nAllianceNum <= GUILD_MAX_ALLIANCE_NUM);
    LOG_PROCESS_ERROR(m_nEnemyNum <= GUILD_MAX_ENEMY_NUM);

    // Expire allied and enemy guilds
    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        if (m_arrAlliance[i].dwEndTime > dwCurrentTime + dwCheckTime)
            continue;
        arrDelAlliance[nDelAllianceNum++] = m_arrAlliance[i].dwGuildID;
    }
    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        if (m_arrEnemy[i].dwEndTime > dwCurrentTime + dwCheckTime)
            continue;
        arrDelEnemy[nDelEnemyNum++] = m_arrEnemy[i].dwGuildID;
    }

    // begin delete
    for (INT32 i = 0; i < nDelAllianceNum; ++i)
    {
        nRetCode = CMsGuildMgr::GetInstance().DelAlliance(m_dwID, arrDelAlliance[i]);
        LOG_CHECK_ERROR(nRetCode);
    }

    for (INT32 i = 0; i < nDelEnemyNum; ++i)
    {
        nRetCode = CMsGuildMgr::GetInstance().DelEnemy(GetID(), arrDelEnemy[i]);
        LOG_CHECK_ERROR(nRetCode);
    }

Exit0:
    return TRUE;
}

INT32 CMsGuild::_LeaveGuild(INT32 nLeaveType, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ROLE_DATA *pRoleData = NULL;
    MS_ONLINE_ROLE_DATA *pOnlineRoleData = NULL;
    UINT32 dwCD = g_GuildConfig.dwReJoinGuildCD + CManageServer::GetInstance().GetGameTime();

    LOG_PROCESS_ERROR_RET_CODE(m_dwLeaderID != dwRoleID, errUnknownError);

    // todo_tq guild: check guild maps and related teleport logic. Allan Tao [15.4.2013]

    pRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    if (pRoleData)
    {
        pRoleData->dwGuildID = 0;
        pRoleData->dwJoinGuildCD = dwCD;
    }

    DelMember(dwRoleID);

    // update mysql ,cd
    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteGMember, NULL, NULL, "ud", dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    // to all gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildDelMemberNtf(dwRoleID, m_dwID, dwCD);
    LOG_CHECK_ERROR(nRetCode);

    // ntf to client
    nRetCode = CMessageHandler::GetInstance().DoClientGuildRoleLeaveNtf(this, dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    // send to self
    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
    if (pOnlineRoleData)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildRoleLeaveAck(pOnlineRoleData->dwCliSessionID, nLeaveType);
        LOG_CHECK_ERROR(nRetCode);
    }

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::_DiplomatAlliance(MS_ONLINE_ROLE_DATA &onlineRoleData, CMsGuild *pMsTargetGuild, INT32 nTimeType)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    LOG_PROCESS_ERROR_RET_CODE(nTimeType > GUILD_DIPLOMAT_TIME_BEGIN && nTimeType < GUILD_DIPLOMAT_TIME_END, errGuildDiplomatTimeError);
    PROCESS_ERROR_RET_CODE(g_GuildConfig.nAllianceConsumeMoney[nTimeType - 1] < GetMoney() , errGuildMoneyLack);

    PROCESS_ERROR_RET_CODE(m_nAllianceNum < GUILD_MAX_ALLIANCE_NUM, errGuildAllianceFull);
    PROCESS_ERROR_RET_CODE(pMsTargetGuild->m_nAllianceNum < GUILD_MAX_ALLIANCE_NUM, errGuildAllianceFull);

    PROCESS_ERROR_RET_CODE(!pMsTargetGuild->IsAlliance(m_dwID), errGuildIsAlliance);
    PROCESS_ERROR_RET_CODE(!pMsTargetGuild->IsEnemy(m_dwID), errGuildIsEnemy);
    PROCESS_ERROR_RET_CODE(!pMsTargetGuild->_IsInAllianceAppList(m_dwID), errGuildAllianceAppAlready);

    nRetCode = CMsGuildMgr::GetInstance().IsWarRelation(GetID(), pMsTargetGuild->GetID());
    PROCESS_ERROR_RET_CODE(nRetCode == FALSE, errGuildWarTime);

    nRetCode = SubMoney(g_GuildConfig.nAllianceConsumeMoney[nTimeType - 1], GUILD_CHEST_ALLIANCE);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubMoneyFailed);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_ALLIANCE, pMsTargetGuild->GetID(), g_GuildConfig.nAllianceConsumeMoney[nTimeType - 1], 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    nRetCode = pMsTargetGuild->_AddAllianceApplication(m_dwID, nTimeType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddAllianceAppFailed);

    // notify
    nRetCode = CMessageHandler::GetInstance().DoClientGuildNotifyNtf(pMsTargetGuild, GUILD_NOTIFY_DIPLOMAT_APP, G_POS_RIGHT_DIPLOMAT);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildApplyAllianceAck(onlineRoleData.dwCliSessionID, pMsTargetGuild->m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::_DiplomatEnemy(MS_ONLINE_ROLE_DATA &onlineRoleData, CMsGuild *pMsTargetGuild, INT32 nTimeType)
{
    INT32       nRetCode = 0;
    INT32   nResult = errUnknownError;
    UINT32          dwEndTime = 0;
    UINT32          dwLastTime = 0;

    char    szSourceMailContent[MAIL_CONTENT_LEN] = {0};
    char    szTargetMailContent[MAIL_CONTENT_LEN] = {0};
    char    szAllTVContent[512] = {0};
    const MAIL_TEMPLATE_DATA* mail_data = NULL;

    LOG_PROCESS_ERROR_RET_CODE(nTimeType > GUILD_DIPLOMAT_TIME_BEGIN && nTimeType < GUILD_DIPLOMAT_TIME_END, errGuildDiplomatTimeError);
    PROCESS_ERROR_RET_CODE(g_GuildConfig.nEnemyConsumeMoney[nTimeType - 1] < GetMoney(), errGuildMoneyLack);

    LOG_PROCESS_ERROR_RET_CODE(m_nEnemyNum < GUILD_MAX_ENEMY_NUM, errGuildAllianceFull);
    LOG_PROCESS_ERROR_RET_CODE(pMsTargetGuild->m_nEnemyNum < GUILD_MAX_ENEMY_NUM, errGuildAllianceFull);

    PROCESS_ERROR_RET_CODE(!pMsTargetGuild->IsAlliance(m_dwID), errGuildIsAlliance);
    PROCESS_ERROR_RET_CODE(!pMsTargetGuild->IsEnemy(m_dwID), errGuildIsEnemy);

    dwLastTime = g_GuildConfig.nGuildDiplomatLastTime[nTimeType - 1];
    dwEndTime = dwLastTime + CManageServer::GetInstance().GetGameTime();

    nRetCode = SubMoney(g_GuildConfig.nEnemyConsumeMoney[nTimeType - 1], GUILD_CHEST_ARMY);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSubMoneyFailed);

    nRetCode = pMsTargetGuild->AddEnemy(m_dwID, dwEndTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEnemyFailed);
    nRetCode = AddEnemy(pMsTargetGuild->m_dwID, dwEndTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEnemyFailed);

    // to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildAddDiplomat(GUILD_DIPLOMAT_ENEMY, m_dwID, pMsTargetGuild->m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    // ntf 
    nRetCode = CMessageHandler::GetInstance().DoClientGuildAddDiplomatNtf(GUILD_DIPLOMAT_ENEMY, pMsTargetGuild, m_dwID, dwLastTime, 
        GetLevel(), (UINT8)m_memberList.size(), m_szGuildName);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAddDiplomatNtf(GUILD_DIPLOMAT_ENEMY, this, pMsTargetGuild->m_dwID, dwLastTime, 
        pMsTargetGuild->GetLevel(), (UINT8)pMsTargetGuild->m_memberList.size(), pMsTargetGuild->m_szGuildName);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    // Place this later so the client can resolve the other guild’s name by ID
    nRetCode = AddGuildEvent(GUILD_HE_TYPE_ENEMY, m_dwID, pMsTargetGuild->m_dwID, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = pMsTargetGuild->AddGuildEvent(GUILD_HE_TYPE_ENEMY, pMsTargetGuild->m_dwID, m_dwID, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_ENEMY, pMsTargetGuild->m_dwID, 0, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = AddGuildChestEvent(GUILD_CHEST_ARMY, pMsTargetGuild->GetID(), g_GuildConfig.nEnemyConsumeMoney[nTimeType - 1], 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddChestEventFailed);

    // ccstTvShow & mail
    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildEnemy);
    LOG_PROCESS_ERROR(mail_data);

    snprintf(szSourceMailContent, MAIL_CONTENT_LEN, mail_data->m_szMailContent, pMsTargetGuild->GetName(), dwLastTime/ONE_DAY_SECOND);
    snprintf(szTargetMailContent, MAIL_CONTENT_LEN, mail_data->m_szMailContent, this->GetName(), dwLastTime/ONE_DAY_SECOND );

    nRetCode = this->BroadcastMail(mail_data->m_szMailSubject, szSourceMailContent);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);
    nRetCode = pMsTargetGuild->BroadcastMail(mail_data->m_szMailSubject, szTargetMailContent);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);

    snprintf(szAllTVContent, 512, "<#P0:%s><#P1:%s><#T:%d>", this->GetName(), pMsTargetGuild->GetName(), g_GuildConfig.dwGuildEnemyTVID);
    nRetCode = CMSChatMgr::GetInstance().Say(0, cctGroup, cltTvShow, szAllTVContent, this->GetSvrGroup());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendTVMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::_AgreeApplyAlliance(MS_ONLINE_ROLE_DATA &onlineRoleData, CMsGuild *pApplicantGuild)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ROLE_DATA    *pRoleData = NULL;
    UINT32          dwEndTime = 0;
    UINT32          dwLastTime = 0;
    INT32           nTimeType = 0;

    char    szSourceMailContent[MAIL_CONTENT_LEN] = {0};
    char    szTargetMailContent[MAIL_CONTENT_LEN] = {0};
    char    szAllTVContent[512] = {0};
    const MAIL_TEMPLATE_DATA* mail_data = NULL;

    nTimeType = _GetAllianceAppTimeType(pApplicantGuild->GetID());
    LOG_PROCESS_ERROR_RET_CODE(nTimeType != -1, errGuildAllianceAppNotExist);
    LOG_PROCESS_ERROR_RET_CODE(nTimeType > 0 && nTimeType < 5, errGuildAllianceAppTimeTypeError);

    dwLastTime = g_GuildConfig.nGuildDiplomatLastTime[nTimeType - 1];
    dwEndTime = dwLastTime + CManageServer::GetInstance().GetGameTime();

    nRetCode = _DelAllianceApplication(pApplicantGuild->m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAllianceAppAlreadyDel);

    LOG_PROCESS_ERROR_RET_CODE(!pApplicantGuild->IsAlliance(m_dwID), errGuildIsAlliance);
    LOG_PROCESS_ERROR_RET_CODE(!pApplicantGuild->IsEnemy(m_dwID), errGuildIsEnemy);

    LOG_PROCESS_ERROR_RET_CODE(m_nAllianceNum < GUILD_MAX_ALLIANCE_NUM, errGuildAllianceFull);
    LOG_PROCESS_ERROR_RET_CODE(pApplicantGuild->m_nAllianceNum < GUILD_MAX_ALLIANCE_NUM, errGuildAllianceFull);

    // add
    nRetCode = pApplicantGuild->AddAlliance(m_dwID, dwEndTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddAllianceAppFailed);
    nRetCode = AddAlliance(pApplicantGuild->m_dwID, dwEndTime);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddAllianceAppFailed);

    // ntf to gs
    nRetCode = CMessageHandler::GetInstance().DoGSGuildAddDiplomat(GUILD_DIPLOMAT_ALLIANCE, m_dwID, pApplicantGuild->m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    // ntf to client
    nRetCode = CMessageHandler::GetInstance().DoClientGuildAddDiplomatNtf(GUILD_DIPLOMAT_ALLIANCE, this, pApplicantGuild->m_dwID, dwLastTime, 
        pApplicantGuild->GetLevel(), (UINT8)pApplicantGuild->m_memberList.size(), pApplicantGuild->m_szGuildName);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);
    nRetCode = CMessageHandler::GetInstance().DoClientGuildAddDiplomatNtf(GUILD_DIPLOMAT_ALLIANCE, pApplicantGuild, m_dwID, dwLastTime, 
        GetLevel(), (UINT8)m_memberList.size(), m_szGuildName);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    // Place this later so the client can resolve the other guild’s name by ID
    nRetCode = AddGuildEvent(GUILD_HE_TYPE_UNION, m_dwID, pApplicantGuild->m_dwID, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = pApplicantGuild->AddGuildEvent(GUILD_HE_TYPE_UNION, pApplicantGuild->m_dwID, m_dwID, 0, 0);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    nRetCode = CMessageHandler::GetInstance().DoDLGuildEvent(GetSvrGroup(),
        CManageServer::GetInstance().GetGameTime(), GetID(), GUILD_HE_TYPE_UNION, pApplicantGuild->m_dwID, 0, 0, 0);
    LOG_CHECK_ERROR(nRetCode);

    // ccstTvShow & mail
    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildAlliance);
    LOG_PROCESS_ERROR(mail_data);

    snprintf(szSourceMailContent, MAIL_CONTENT_LEN, mail_data->m_szMailContent, pApplicantGuild->GetName(), dwLastTime / ONE_DAY_SECOND);
    snprintf(szTargetMailContent, MAIL_CONTENT_LEN, mail_data->m_szMailContent, this->GetName(), dwLastTime / ONE_DAY_SECOND );

    nRetCode = this->BroadcastMail(mail_data->m_szMailSubject, szSourceMailContent);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);
    nRetCode = pApplicantGuild->BroadcastMail(mail_data->m_szMailSubject, szTargetMailContent);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);

    snprintf(szAllTVContent, 512, "<#P0:%s><#P1:%s><#T:%d>", this->GetName(), pApplicantGuild->GetName(), g_GuildConfig.dwGuildAllianceTVID);
    nRetCode = CMSChatMgr::GetInstance().Say(0, cctGroup, cltTvShow, szAllTVContent, this->GetSvrGroup());
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendTVMsgError);

    return errSuccess;
Exit0:
    return nResult;
}

INT32 CMsGuild::_RefuseApplyAlliance(MS_ONLINE_ROLE_DATA &onlineRoleData, CMsGuild *pApplicantGuild)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ROLE_DATA    *pRoleData = NULL;

    nRetCode = _DelAllianceApplication(pApplicantGuild->m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAllianceAppAlreadyDel);

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pApplicantGuild->m_dwLeaderID);
    if (pRoleData)
    {
        char content[MAIL_CONTENT_LEN] = {0};
        const MAIL_TEMPLATE_DATA* mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildApplyAllianceFailed);
        LOG_PROCESS_ERROR(mail_data);

        snprintf(content, MAIL_CONTENT_LEN, mail_data->m_szMailContent, m_szGuildName);
        nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, mail_data->m_szMailSubject, content);
        LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildSendMailFailed);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientGuildReplyUnionAppAck(onlineRoleData.dwCliSessionID, pApplicantGuild->m_dwID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddEventFailed);

    return errSuccess;
Exit0:
    return nResult;
}

BOOL CMsGuild::AddMoney(INT32 nMoney, GUILD_HISTORY_EVENT_TYPE eReason, BOOL bIsSave)
{
    INT32   nRetCode = 0;
    INT32   nResult = errGuildFailed;
    INT32   nMaxMoney = 0;
    INT32   nOldMoney = m_nGuildMoney;

    LOG_PROCESS_ERROR(nMoney >= 0);
    nMaxMoney = GetMaxStorage(GUILD_B_STORAGE);
    LOG_PROCESS_ERROR(nMaxMoney > 0);
    
    if (m_nGuildMoney + nMoney > nMaxMoney)
        m_nGuildMoney = nMaxMoney;
    else
        m_nGuildMoney += nMoney;

    if (bIsSave)
    {
        nRetCode = SqlUpdateGuild(this);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoDLGuildChest(GetSvrGroup(), CManageServer::GetInstance().GetGameTime(), GetID(), m_nGuildMoney - nOldMoney, eReason);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_MONEY, GetMoney());
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_MONEY, GetMoney());
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    m_nGuildMoney = nOldMoney;
    return FALSE;
}

BOOL CMsGuild::SubMoney(INT32 nMoney, GUILD_HISTORY_EVENT_TYPE eReason, BOOL bIsSave)
{
    INT32   nRetCode = 0;
    INT32   nResult = errGuildFailed;
    INT32   nActualDel = 0;

    LOG_PROCESS_ERROR(nMoney >= 0);

    if (m_nGuildMoney >= nMoney)
    {
        m_nGuildMoney -= nMoney;
        nActualDel = nMoney;
    }
    else
    {
        m_nGuildMoney = 0;
        nActualDel = m_nGuildMoney;
    }

    if (bIsSave)
    {
        nRetCode = SqlUpdateGuild(this);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoDLGuildChest(GetSvrGroup(), CManageServer::GetInstance().GetGameTime(), GetID(), 0-nActualDel, eReason);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_MONEY, GetMoney());
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_MONEY, GetMoney());
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMsGuild::GetResourceIncrement(INT32 nType)
{
    INT32   nLevel = 0;
    const GUILD_BUILDING_DATA     *pTemplate = NULL;

    LOG_PROCESS_ERROR(nType == GUILD_B_WOOD ||
        nType == GUILD_B_HERB || nType == GUILD_B_CRYSTAL ||
        nType == GUILD_B_SILVER);

    if (m_FunBuilding[nType].dwReviveTime > 0)
        nLevel = 0;
    else
        nLevel = m_FunBuilding[nType].nLevel;

    pTemplate = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR(pTemplate);

    return pTemplate->nParam1;
Exit0:
    return 0;
}

INT32 CMsGuild::GetMaxStorage(INT32 nType)
{
    INT32   nLevel = 0;
    const GUILD_BUILDING_DATA     *pTemplate = NULL;

    LOG_PROCESS_ERROR(nType == GUILD_B_STORAGE ||
        nType == GUILD_B_WOOD_STORAGE || nType == GUILD_B_HERB_STORAGE ||
        nType == GUILD_B_CRYSTAL_STORAGE || nType == GUILD_B_SILVER_STORAGE);

    if (m_FunBuilding[nType].dwReviveTime > 0)
        nLevel = 0;
    else
        nLevel = m_FunBuilding[nType].nLevel;

    pTemplate = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR(pTemplate);

    return pTemplate->nParam1;
Exit0:
    return 0;
}

BOOL CMsGuild::SetProsperity(INT32 nValue)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(nValue > 0);
    m_nProsperity = nValue;

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_PROSPERITY, m_nProsperity);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::InitProsperity()
{
    const   GUILD_BUILDING_DATA     *pBuildingData = NULL;
    m_nProsperity = 0;

    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        if (m_FunBuilding[i].nType != 0)
        {
            pBuildingData = GetGuildBuildingData(i, m_FunBuilding[i].nLevel);
            LOG_PROCESS_ERROR(pBuildingData);
            m_nProsperity += pBuildingData->nProsperity;
        }
    }

    for (G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != m_BuildingList.end(); ++iter)
    {
        pBuildingData = GetGuildBuildingData(iter->second.nType, iter->second.nLevel);
        LOG_PROCESS_ERROR(pBuildingData);
        m_nProsperity += pBuildingData->nProsperity;
    }

    m_nProsperity += (g_GuildConfig.nGuildMemberBaseProsperity * (INT32)m_memberList.size());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::UpdateBuildingProsperity(INT32 nType, INT32 nLevel)
{
    INT32       nRetCode = 0;
    const GUILD_BUILDING_DATA   *pTemplate = NULL;
    const GUILD_BUILDING_DATA   *pLowLevelTemplate = NULL;

    pTemplate = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR(pTemplate);

    pLowLevelTemplate = GetGuildBuildingData(nType, nLevel - 1);
    LOG_PROCESS_ERROR(pLowLevelTemplate);

    nRetCode = SetProsperity(GetProsperity() + pTemplate->nProsperity - pLowLevelTemplate->nProsperity);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddWood(INT32 nWood, BOOL bSendMsg)
{
    INT32       nRetCode = 0;
    INT32       nMaxWood = 0;
    INT32       nOldWood = m_nWood;

    LOG_PROCESS_ERROR(nWood >= 0);

    nMaxWood = GetMaxStorage(GUILD_B_WOOD_STORAGE);
    LOG_PROCESS_ERROR(nMaxWood > 0);

    if (m_nWood + nWood > nMaxWood)
    {
        m_nWood = nMaxWood;
    }
    else
    {
        m_nWood += nWood;
    }

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_WOOD, m_nWood);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_WOOD, m_nWood);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    m_nWood = nOldWood;
    return FALSE;
}

BOOL CMsGuild::SubWood(INT32 nWood, BOOL bSendMsg)
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(nWood >= 0);
    LOG_PROCESS_ERROR(m_nWood >= nWood);

    m_nWood -= nWood;

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_WOOD, m_nWood);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_WOOD, m_nWood);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddHerb(INT32 nHerb, BOOL bSendMsg)
{
    INT32       nRetCode = 0;
    INT32       nMaxHerb = 0;
    INT32       nOldHerb = m_nHerb;

    LOG_PROCESS_ERROR(nHerb >= 0);

    nMaxHerb = GetMaxStorage(GUILD_B_HERB_STORAGE);
    LOG_PROCESS_ERROR(nMaxHerb > 0);

    if (m_nHerb + nHerb > nMaxHerb)
    {
        m_nHerb = nMaxHerb;
    }
    else
    {
        m_nHerb += nHerb;
    }

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_HERB, m_nHerb);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_HERB, m_nHerb);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    m_nHerb = nOldHerb;
    return FALSE;
}

BOOL CMsGuild::SubHerb(INT32 nHerb, BOOL bSendMsg )
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(nHerb >= 0);
    LOG_PROCESS_ERROR(m_nHerb >= nHerb);

    m_nHerb -= nHerb;

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_HERB, m_nHerb);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_HERB, m_nHerb);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddCrystal(INT32 nCrystal, BOOL bSendMsg )
{
    INT32   nRetCode = 0;
    INT32   nMaxCrystal = 0;
    INT32   nOldCrystal = m_nCrystal;

    LOG_PROCESS_ERROR(nCrystal >= 0);

    nMaxCrystal = GetMaxStorage(GUILD_B_CRYSTAL_STORAGE);
    LOG_PROCESS_ERROR(nMaxCrystal > 0);

    if (m_nCrystal + nCrystal > nMaxCrystal)
    {
        m_nCrystal = nMaxCrystal;
    }
    else
    {
        m_nCrystal += nCrystal;
    }

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_CRYSTAL, m_nCrystal);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_CRYSTAL, m_nCrystal);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    m_nCrystal = nOldCrystal;
    return FALSE;
}

BOOL CMsGuild::SubCrystal(INT32 nCrystal, BOOL bSendMsg )
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(nCrystal >= 0);
    LOG_PROCESS_ERROR(m_nCrystal >= nCrystal);

    m_nCrystal -= nCrystal;

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_CRYSTAL, m_nCrystal);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_CRYSTAL, m_nCrystal);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddSilver(INT32 nSilver, BOOL bSendMsg)
{
    INT32       nRetCode = 0;
    INT32       nMaxSilver = 0;
    INT32       nOldSilver = m_nSilver;

    LOG_PROCESS_ERROR(nSilver >= 0);

    nMaxSilver = GetMaxStorage(GUILD_B_SILVER_STORAGE);
    LOG_PROCESS_ERROR(nMaxSilver > 0);

    if (m_nSilver + nSilver > nMaxSilver)
    {
        m_nSilver = nMaxSilver;
    }
    else
    {
        m_nSilver += nSilver;
    }

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_SILVER, m_nSilver);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_SILVER, m_nSilver);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    m_nSilver = nOldSilver;
    return FALSE;
}

BOOL CMsGuild::SubSilver(INT32 nSilver, BOOL bSendMsg)
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(nSilver >= 0);
    LOG_PROCESS_ERROR(m_nSilver >= nSilver);

    m_nSilver -= nSilver;

    nRetCode = CMessageHandler::GetInstance().DoGSSyncGuildAttrNtf(GetID(), GUILD_ATTR_SILVER, m_nSilver);
    LOG_CHECK_ERROR(nRetCode);

    if (bSendMsg)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, GUILD_ATTR_SILVER, m_nSilver);
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CMsGuild::GetVariable(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_GUILD_VARIABLE_COUNT);
    return m_arrVariables[nIndex];
Exit0:
    return 0;
}

void CMsGuild::SetVariable(INT32 nIndex, INT32 nValue)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_GUILD_VARIABLE_COUNT);
    m_arrVariables[nIndex] = nValue;

#ifndef _ROBOT
    nRetCode = CMessageHandler::GetInstance().DoGSGuildSyncVariableNtf(GetID(), nIndex, nValue);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildAttrModifyNtf(this, nIndex, nValue);
    LOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
    return;
}

//void CMsGuild::ChangeMoney(INT32 byOpType, UINT32 dwMoney)
//{
//    INT32   nResult = errGuildFailed;
//
//    if (GUILD_MONEY_OP_ADD == byOpType)
//    {
//        m_nGuildMoney += dwMoney;
//        if (m_nGuildMoney > MAX_MONEY * 2)
//            m_nGuildMoney = MAX_MONEY * 2;
//    }
//    else if (GUILD_MONEY_OP_DEC == byOpType)
//    {
//        if (m_nGuildMoney >= dwMoney)
//            m_nGuildMoney -= dwMoney;
//        else
//            m_nGuildMoney = 0;
//    }
//    else
//        return ;
//
//    nResult = _UpdateGuildMoneyToMysql(m_dwID);
//    LOG_CHECK_ERROR(nResult);
//
//    return ;    
//}

void CB_GuildAppTimeout(APP_NODE *pNode)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGuildFailed;
    MS_ROLE_DATA    *pRoleData = NULL;
    MS_ONLINE_ROLE_DATA     *pOnlineRole = NULL;

    LOG_PROCESS_ERROR(pNode);
    LOG_PROCESS_ERROR_RET_CODE(pNode->nType == EN_GUILD_APPLICATION, errUnknownError);
    
    pRoleData = CRoleDataMgr::GetInstance().FindRole(pNode->dwID_1);
    LOG_PROCESS_ERROR(pRoleData);

    LOG_PROCESS_ERROR(pRoleData->byApplicationCount>0);
    --pRoleData->byApplicationCount;

    LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteGuildApp(pNode));

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pRoleData->dwRoleID);
    if (pOnlineRole)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientDelGuildApp(pOnlineRole->dwCliSessionID, pNode->dwID_2);
        LOG_PROCESS_ERROR(nRetCode);
    }

    //pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pRoleData->dwRoleID);
    //if (pOnlineRoleData)
    //{
    //    nRetCode = CMessageHandler::GetInstance().DoClientRevertGuildApplyAck(pOnlineRoleData->dwCliSessionID, dwRoleID);
    //    LOG_PROCESS_ERROR(nRetCode);
    //}

    return ;
Exit0:
    if (pNode && nResult != errUnknownError)
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteGuildApp(pNode));
    return ;
}

void CB_DiplomatAppTimeout(APP_NODE *pNode)
{
    INT32       nResult = errGuildFailed;
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pNode);
    LOG_PROCESS_ERROR_RET_CODE(pNode->nType == EN_DIPLOMAT_APPLICATION, errUnknownError);

    nRetCode = CMsGuildMgr::GetInstance().DeleteDiplomatApp((DIPLOMAT_APP_NODE*)pNode);
    LOG_CHECK_ERROR(nRetCode);

    return ;
Exit0:
    if (pNode && nResult != errUnknownError)
        LOG_CHECK_ERROR(CMsGuildMgr::GetInstance().DeleteGuildApp((DIPLOMAT_APP_NODE*)pNode));
    return ;
}

INT32 CB_UpgradeWelfareTimeout(BASE_TRIGGER_NODE *pNode)
{
    INT32       nRetCode = 0;
    GUILD_UPGRADE_TRIGGER        *pData = NULL;
    CMsGuild            *pMsGuild = NULL;
    LOG_PROCESS_ERROR(pNode);
    
    pData = (GUILD_UPGRADE_TRIGGER*)pNode;
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pData->dwGuildID);
    LOG_PROCESS_ERROR(pMsGuild);

    nRetCode = pMsGuild->CompleteUpgradeWelfareBuffer(pData);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return TIME_TRIGGER_RET_END;
}

INT32 CB_UpgradeBuildingTimeout(BASE_TRIGGER_NODE *pNode)
{
    INT32       nRetCode = 0;
    GUILD_UPGRADE_TRIGGER        *pData = NULL;
    CMsGuild            *pMsGuild = NULL;
    LOG_PROCESS_ERROR(pNode);

    pData = (GUILD_UPGRADE_TRIGGER*)pNode;
    pMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pData->dwGuildID);
    LOG_PROCESS_ERROR(pMsGuild);

    nRetCode = pMsGuild->CompleteUpgradeBuilding(pData);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return TIME_TRIGGER_RET_END;
}


INT32 CB_BuildingEnable(BASE_TRIGGER_NODE *pNode)
{
    MS_G_B_DATA * pBuildingData{};
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild    *poMSGuild = NULL;
    G_B_REVIVE_TRIGGER  *pTrigger = (G_B_REVIVE_TRIGGER *)pNode;

    LOG_PROCESS_ERROR(pTrigger);

    poMSGuild = CMsGuildMgr::GetInstance().FindGuildByID(pTrigger->dwGuildID);
    LOG_PROCESS_ERROR(poMSGuild);

    pBuildingData = poMSGuild->GetBuildingDataByNpcID(pTrigger->qwNameID);
    LOG_PROCESS_ERROR(pBuildingData);

    pBuildingData->dwReviveTime = 0;

    nRetCode = poMSGuild->AddGuildEvent(GUILD_HE_TYPE_BUILDING_REVIVE, pBuildingData->nType, 0, 0, 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildSyncBuildingStateNtf(poMSGuild->GetID(), pTrigger->qwNameID, pBuildingData->dwReviveTime);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMsGuildMgr::GetInstance().DeleteBGReviveTrigger(pTrigger);
    LOG_PROCESS_ERROR(pTrigger);

Exit0:
    return TIME_TRIGGER_RET_END;
}

BOOL SqlUpdateGuild(CMsGuild *poMsGuild)
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(poMsGuild);

    nRetCode = CPreSaveList::GetInstance().SaveToList(PRE_SAVE_GUILD_MAIN, poMsGuild->GetID());
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}


void CMsGuild::EnrollSabac(UINT32 dwEnrollTime)
{
	INT32 nRetCode = 0;
	INT32 nIndex = 0;
	UINT32 dwTime = dwEnrollTime + CManageServer::GetInstance().GetTimeZone();
	dwTime /= 86400;

	for (; nIndex < MAX_SABAC_ENROLL_COUNT; nIndex++)
	{
		UINT32 dwTempTime = m_dwSabacEnrollTime[nIndex] + CManageServer::GetInstance().GetTimeZone();
		dwTempTime /= 86400;

		if (dwTime - dwTempTime > 3)
			break;
	}
	LOG_PROCESS_ERROR(nIndex < MAX_SABAC_ENROLL_COUNT);
	m_dwSabacEnrollTime[nIndex] = dwEnrollTime;

	nRetCode = CMessageHandler::GetInstance().DoGSEnrollSabac(m_dwID, m_dwSabacEnrollTime[nIndex]);
	LOG_CHECK_ERROR(nRetCode);

	if (!m_bOccupiedSabac)
	{
		nRetCode = CMessageHandler::GetInstance().DoClientEnrollSabac(m_nSvrGroupID, m_dwID);
		LOG_CHECK_ERROR(nRetCode);
	}

	nRetCode = SqlUpdateGuild(this);
	LOG_CHECK_ERROR(nRetCode);

	return;
Exit0:
	return;
}

void CMsGuild::OccupySabac(BOOL bOccupied)
{
	m_bOccupiedSabac = bOccupied;

	if (m_bOccupiedSabac)
		CManageServer::GetInstance().SetSabacGuildID(m_nSvrGroupID, m_dwID);

	SqlUpdateGuild(this);
}

BOOL CMsGuild::IsEnrolledSabac(void)
{
	UINT32 dwTime = (CManageServer::GetInstance().GetGameTime() + CManageServer::GetInstance().GetTimeZone()) / 86400;

	for (INT32 nIndex = 0; nIndex < MAX_SABAC_ENROLL_COUNT; nIndex++)
	{
		UINT32 dwEnrollTime = (m_dwSabacEnrollTime[nIndex] + CManageServer::GetInstance().GetTimeZone()) / 86400;

		if (dwTime == dwEnrollTime +3)
			return TRUE;
	}

	return FALSE;
}

BOOL CMsGuild::CheckAchievement(INT32 nID)
{
    const   GUILD_ACHIEVEMENT   *pTemplate = NULL;
    const   GUILD_ACHI_CDTN     *pCdtnTemplate = NULL;

    pTemplate = GetGuildAchievement(nID);
    LOG_PROCESS_ERROR(pTemplate);

    for (INT32 i = 0; i < GuildAchiCdtnTableSize; ++i)
    {
        if (pTemplate->CdtnData[i].nCdtnType == 0)
            continue;
        pCdtnTemplate = GetGuildAchiCdtn(pTemplate->CdtnData[i].nCdtnType, pTemplate->CdtnData[i].nParam1);
        LOG_PROCESS_ERROR(pCdtnTemplate);

        PROCESS_ERROR((m_Achievement[pCdtnTemplate->nCdtnType] & (1 << pCdtnTemplate->nPos)) != 0);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SetAchievementCondition(INT32 nCdtnType, INT32 nIndex)
{
    INF("Guild Achievement Debug: Guild[%u] CdtnType[%d], Index[%d]", m_dwID, nCdtnType, nIndex);

    LOG_PROCESS_ERROR(nCdtnType < GUILD_ACHI_TYPE_NUM);

    m_Achievement[nCdtnType] |= (1 << nIndex);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::SetWarCharacter(INT32 nC)
{
    INT32       nRetCode = 0;
    m_byWarCharacter |= nC;

    nRetCode = CMessageHandler::GetInstance().DoGSGuildSyncWarCharacter(GetID(), m_byWarCharacter);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::ClearWarCharacter(INT32 nC)
{
    INT32       nRetCode = 0;
    m_byWarCharacter &= (~nC);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildSyncWarCharacter(GetID(), m_byWarCharacter);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuild::AddIntruder(UINT32 dwGuildID)
{
    m_IntruderSet.insert(dwGuildID);
    return TRUE;
}

BOOL CMsGuild::ClearIntruder()
{
    m_IntruderSet.clear();
    return TRUE;
}

UINT32 CMsGuild::FirstIntruder()
{
    m_IntruderCursor = m_IntruderSet.begin();
    PROCESS_ERROR(m_IntruderSet.end() != m_IntruderCursor);

    return *m_IntruderCursor;
Exit0:
    return 0;
}

UINT32 CMsGuild::NextIntruder()
{
    ++m_IntruderCursor;
    PROCESS_ERROR(m_IntruderSet.end() != m_IntruderCursor);

    return *m_IntruderCursor;
Exit0:
    return 0;
}
