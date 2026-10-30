#include "stdafx.h"
#include "gs_guild.h"

#ifndef _ROBOT

#include <algorithm>
#include "guild_building_data.h"
#include "id_def.h"
#include "role.h"
#include "game_world.h"
#include "message_handler.h"
#include "guild_mgr.h"
#include "npc.h"
#include "scene.h"
#include "scene_file_mgr.h"
#include "guild_activity.h"

CGSGuild::CGSGuild() : 
m_dwID(INVALID_OBJECT_ID),
m_nState(GUILD_STATE_NONE),
m_dwLeaderID(INVALID_OBJECT_ID),
m_nMoney(0),
m_nHerb(0),
m_nWood(0),
m_nCrystal(0),
m_nSilver(0),
m_nAllianceNum(0),
m_nEnemyNum(0)
{
    m_MemberList.clear();
    memset(m_FunBuildingList, 0, sizeof(m_FunBuildingList));
    m_BuildingList.clear();
    m_arrUBuildingList.clear();
    memset(m_szName, 0, sizeof(m_szName));
    memset(m_arrWelfareAuraLevel, 0, sizeof(m_arrWelfareAuraLevel));
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));
	memset(m_dwSabacEnrollTime, 0, sizeof(m_dwSabacEnrollTime));
	m_bOccupiedSabac = FALSE;
	m_nServerGroup = 0;
}

CGSGuild::~CGSGuild()
{
    m_dwID = INVALID_OBJECT_ID;
    m_nState = GUILD_STATE_NONE;
    m_dwLeaderID = INVALID_OBJECT_ID;
    m_nAllianceNum = 0;
    m_nEnemyNum = 0;

    m_MemberList.clear();
    memset(m_FunBuildingList, 0, sizeof(m_FunBuildingList));
    m_BuildingList.clear();
    m_arrUBuildingList.clear();
    memset(m_szName, 0, sizeof(m_szName));
    memset(m_arrWelfareAuraLevel, 0, sizeof(m_arrWelfareAuraLevel));
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));
}

BOOL CGSGuild::Init()
{
    m_dwID = INVALID_OBJECT_ID;
    m_nState = GUILD_STATE_NONE;
    m_dwLeaderID = INVALID_OBJECT_ID;

    m_nMoney = 0;
    m_nHerb = 0;
    m_nWood = 0;
    m_nCrystal = 0;
    m_nSilver = 0;
    m_nAllianceNum = 0;
    m_nEnemyNum = 0;

    m_MemberList.clear();
    m_MemberIterator = m_MemberList.end();
    memset(m_FunBuildingList, 0, sizeof(m_FunBuildingList));
    m_BuildingList.clear();
    m_BCursor = m_BuildingList.end();
    m_arrUBuildingList.clear();
    m_UBCursor = m_arrUBuildingList.end();
    memset(m_szName, 0, sizeof(m_szName));
    memset(m_arrWelfareAuraLevel, 0, sizeof(m_arrWelfareAuraLevel));
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));
	memset(m_dwSabacEnrollTime, 0, sizeof(m_dwSabacEnrollTime));
	m_bOccupiedSabac = FALSE;
	m_nServerGroup = 0;

    m_nWarCharacter = GUILD_WAR_NONE;

    return TRUE;
}

BOOL CGSGuild::UnInit()
{
    m_dwID = INVALID_OBJECT_ID;
    m_nState = GUILD_STATE_NONE;
    m_dwLeaderID = INVALID_OBJECT_ID;
    m_nAllianceNum = 0;
    m_nEnemyNum = 0;

    m_MemberList.clear();
    m_MemberIterator = m_MemberList.end();
    m_arrUBuildingList.clear();
    m_UBCursor = m_arrUBuildingList.end();
    memset(m_szName, 0, sizeof(m_szName));
    memset(m_arrWelfareAuraLevel, 0, sizeof(m_arrWelfareAuraLevel));
    memset(m_arrAlliance, 0, sizeof(m_arrAlliance));
    memset(m_arrEnemy, 0, sizeof(m_arrEnemy));
    memset(m_arrVariables, 0, sizeof(m_arrVariables));
    m_bOccupiedSabac = FALSE;

    return TRUE;
}

BOOL CGSGuild::Create(UINT32 dwGuildID, INT32 nState, UINT32 dwLeaderID, const char *pcszGuildName, 
                      INT32 nAllianceNum, UINT32 arrAlliance[], INT32 nEnemyNum, UINT32 arrEnemy[], 
                      INT8 *arrWelfareLvl, INT32 *arrVariables, INT32 nServerGroup)
{
    INT32   nRetCode = 0;

    LOG_PROCESS_ERROR(pcszGuildName);
    LOG_PROCESS_ERROR(nAllianceNum <= sizeof(m_arrAlliance));
    LOG_PROCESS_ERROR(nEnemyNum <= sizeof(m_arrEnemy));

    nRetCode = Init();
    LOG_PROCESS_ERROR(nRetCode);

    m_dwID = dwGuildID;
    m_dwLeaderID = dwLeaderID;
    m_nState = nState;
	m_nServerGroup = nServerGroup;

    strxcpy(m_szName, pcszGuildName, sizeof(m_szName));

    if (arrWelfareLvl)
    {
        for (INT32 i = 0; i < GUILD_MAX_WELFARE_AURA_COUNT; ++i)
        {
            m_arrWelfareAuraLevel[i] = arrWelfareLvl[i];
        }
    }
    
    if (arrAlliance)
    {
        for (INT32 i = 0; i < nAllianceNum; ++i)
        {
            m_arrAlliance[m_nAllianceNum++] = arrAlliance[i];
        }
    }

    if (arrEnemy)
    {
        for (INT32 i = 0; i < nEnemyNum; ++i)
        {
            m_arrEnemy[m_nEnemyNum++] = arrEnemy[i];
        }
    }

    if (arrVariables)
    {
        for (INT32 i = 0; i < MAX_GUILD_VARIABLE_COUNT; ++i)
        {
            m_arrVariables[i] = arrVariables[i];
        }
    }
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSGuild::Destory()
{
    CRole   *poRole = NULL;
    CGSGuild    *poTargetGSGuild = NULL;
    INT32   nRetCode = 0;

    for (MEMBER_LIST::iterator iter = m_MemberList.begin(); iter != m_MemberList.end(); ++iter)
    {
        poRole = CGameWorld::GetInstance().FindRole(iter->dwRoleID);
        if (poRole)
        {
            if (poRole->GetScene() && poRole->GetScene()->GetSceneID() == m_dwID)
            {
                nRetCode = poRole->JumpToReturnPos();           // redmine bug: 7299
                LOG_CHECK_ERROR(nRetCode);
            }

            poRole->SetGuildID(INVALID_OBJECT_ID);
#ifndef _ROBOT
            nRetCode = CCliMessageHandler::GetInstance().DoSyncGuildName(poRole, 0);
            LOG_CHECK_ERROR(nRetCode);
#endif
        }
    }

    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        poTargetGSGuild = CGuildMgr::GetInstance().FindGuild(m_arrAlliance[i]);
        LOG_CHECK_ERROR(poTargetGSGuild);
        if (poTargetGSGuild)
        {
            nRetCode = poTargetGSGuild->DelAlliance(m_dwID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        poTargetGSGuild = CGuildMgr::GetInstance().FindGuild(m_arrEnemy[i]);
        LOG_CHECK_ERROR(poTargetGSGuild);
        if (poTargetGSGuild)
        {
            nRetCode = poTargetGSGuild->DelEnemy(m_dwID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    nRetCode = UnInit();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

GS_GUILD_MEMBER* CGSGuild::GetMember(UINT32 dwRoleID)
{
    MEMBER_LIST::iterator iter = std::find(m_MemberList.begin(), m_MemberList.end(), dwRoleID);
    PROCESS_ERROR(iter != m_MemberList.end());

    return &(*iter);
Exit0:
    return NULL;
}

INT32 CGSGuild::GetMaxMemberCount()
{
    const GUILD_BUILDING_DATA *pBuildingTemplate = NULL;
    const GUILD_BUILDING_DATA *pZeroTemplate = NULL;

    pZeroTemplate = GetGuildBuildingData(GUILD_B_HOUSE, 0);
    LOG_PROCESS_ERROR(pZeroTemplate);

    pBuildingTemplate = GetGuildBuildingData(GUILD_B_HOUSE, m_FunBuildingList[GUILD_B_HOUSE].nLevel);
    LOG_PROCESS_ERROR(pBuildingTemplate);

    if (m_FunBuildingList[GUILD_B_HOUSE].dwReviveTime > 0)
        return pZeroTemplate->nParam1;
    else
        return pBuildingTemplate->nParam1;
Exit0:
    return 0;
}

BOOL CGSGuild::AddMember(UINT32 dwRoleID, INT32 nPosition)
{
    GS_GUILD_MEMBER data;
    data.dwRoleID = dwRoleID;
    data.nPosition = nPosition;
    m_MemberList.push_back(data);
    return TRUE;
}

BOOL CGSGuild::DelMember(UINT32 dwRoleID)
{
    MEMBER_LIST::iterator iter = std::find(m_MemberList.begin(), m_MemberList.end(), dwRoleID);
    LOG_PROCESS_ERROR(iter != m_MemberList.end());
    m_MemberList.erase(iter);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSGuild::IsMember(UINT32 dwRoleID)
{
    MEMBER_LIST::iterator iter = find(m_MemberList.begin(), m_MemberList.end(), dwRoleID);
    LOG_PROCESS_ERROR(iter != m_MemberList.end());

    return TRUE;
Exit0:
    return FALSE;
}

UINT32 CGSGuild::GetFirstMember(void)
{
	m_MemberIterator = m_MemberList.begin();
	if (m_MemberIterator != m_MemberList.end())
		return m_MemberIterator->dwRoleID;
	else
		return INVALID_OBJECT_ID;
}

UINT32 CGSGuild::GetNextMember(void)
{
	++m_MemberIterator;
	if (m_MemberIterator != m_MemberList.end())
		return m_MemberIterator->dwRoleID;
	else
		return INVALID_OBJECT_ID;
}

INT32 CGSGuild::GetWelfareLevelByType(INT32 nType)
{
    LOG_PROCESS_ERROR(0 < nType && nType < GUILD_MAX_WELFARE_AURA_COUNT);

    return m_arrWelfareAuraLevel[nType];
Exit0:
    return 1;
}

BOOL CGSGuild::SetWelfareLevelByType(INT32 nType, INT32 nLevel)
{
    LOG_PROCESS_ERROR(0 < nType && nType < GUILD_MAX_WELFARE_AURA_COUNT);

    m_arrWelfareAuraLevel[nType] = nLevel;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSGuild::IsActiveWelfareBuffer(INT32 nType)
{
    LOG_PROCESS_ERROR(0 < nType && nType < GUILD_MAX_WELFARE_AURA_COUNT);
    LOG_PROCESS_ERROR(0 < m_arrWelfareAuraLevel[nType]);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CGSGuild::GetBuildingMaxLevelByType(INT32 nType)
{
    if (nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM)
        if (m_FunBuildingList[nType].dwReviveTime > 0)
            return 0;
        else
            return m_FunBuildingList[nType].nLevel;
    else
    {
        INT32   nLevel = 0;
        GS_G_BUILDING_MAP::iterator    end_iter = m_BuildingList.end();
        for (GS_G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != end_iter; ++iter)
        {
            if (iter->second.nType == nType && iter->second.dwReviveTime == 0)
            {
                if (iter->second.nLevel > nLevel)
                {
                    nLevel = iter->second.nLevel;
                }
            }
        }
        return nLevel;
    }
}

BOOL CGSGuild::IsBuildingEnable(NAMEID qwNameID)
{
    INT32   nRetCode = 0;
    GS_G_B_DATA *pBuilding = GetBuildingDataByNpcID(qwNameID);
    LOG_PROCESS_ERROR(pBuilding);
    PROCESS_ERROR(pBuilding->dwReviveTime == 0);

    return TRUE;
Exit0:
    return FALSE;
}

GS_G_B_DATA* CGSGuild::GetFunBuildingDataByType(INT32 nType)
{
    LOG_PROCESS_ERROR(nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM);
    return &m_FunBuildingList[nType];
Exit0:
    return NULL;
}

GS_G_B_DATA* CGSGuild::GetBuildingDataByNpcID(NAMEID qwNpcNameID)
{
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        if (m_FunBuildingList[i].qwNameID == qwNpcNameID)
            return &m_FunBuildingList[i];
    }

    {
        GS_G_BUILDING_MAP::iterator iter = m_BuildingList.find(qwNpcNameID);
        PROCESS_ERROR(iter != m_BuildingList.end());

        return &(iter->second);
    }

Exit0:
    return NULL;
}

GS_G_B_DATA* CGSGuild::GetBuildingDataByString(const char *pcszNameID)
{
    NAMEID      qwNameID = 0;

    LOG_PROCESS_ERROR(pcszNameID);
    strncpy((char*)&qwNameID, pcszNameID, sizeof(qwNameID));

    return GetBuildingDataByNpcID(qwNameID);
Exit0:
    return NULL;
}

INT32 CGSGuild::GetBuildingNumByType(INT32 nType)
{
    if (nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM)
    {
        if (m_FunBuildingList[nType].nType == 0)
            return 0;
        else
            return 1;
    }
    else
    {
        INT32       nNum = 0;
        GS_G_BUILDING_MAP::iterator end_iter = m_BuildingList.end();

        for (GS_G_BUILDING_MAP::iterator iter = m_BuildingList.begin(); iter != end_iter; ++iter)
        {
            if (iter->second.nType == nType)
            {
                ++nNum;
            }
        }

        return nNum;
    }
}


BOOL CGSGuild::AddBuilding(NAMEID qwNameID, INT32 nType, INT32 nLevel)
{
    if (nType > GUILD_B_NONE && nType < GUILD_B_FUNATIONAL_NUM)
    {
        m_FunBuildingList[nType].qwNameID = qwNameID;
        m_FunBuildingList[nType].nType = nType;
        m_FunBuildingList[nType].nLevel = nLevel;
        m_FunBuildingList[nType].dwReviveTime = 0;
    }
    else
    {
        GS_G_BUILDING_MAP::iterator    iter = m_BuildingList.find(qwNameID);
        LOG_PROCESS_ERROR(iter == m_BuildingList.end());

        m_BuildingList.insert(std::make_pair(qwNameID, GS_G_B_DATA(qwNameID, nType, nLevel)));
    }

    return TRUE;
Exit0:
    return FALSE;
}

void CGSGuild::DelBuilding(NAMEID qwNameID)
{
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        if (m_FunBuildingList[i].qwNameID == qwNameID)
        {
            m_FunBuildingList[i].nType = 0;
            m_FunBuildingList[i].qwNameID = 0;
            m_FunBuildingList[i].nLevel = 0;
            m_FunBuildingList[i].dwReviveTime = 0;
            return;
        }
    }

    m_BuildingList.erase(qwNameID);
}

GS_G_B_DATA* CGSGuild::FirstUnFunBuilding()
{
    m_BCursor = m_BuildingList.begin();
    PROCESS_ERROR(m_BCursor != m_BuildingList.end());

    return &(m_BCursor->second);
Exit0:
    return NULL;
}

GS_G_B_DATA* CGSGuild::NextUnFunBuilding()
{
    ++m_BCursor;
    PROCESS_ERROR(m_BCursor != m_BuildingList.end());

    return &(m_BCursor->second);
Exit0:
    return NULL;
}

GS_G_B_DATA* CGSGuild::GetUpgradBuildingDataByNpcID(NAMEID qwNpcNameID)
{
    GS_U_G_BUILDING_MAP::iterator iter = m_arrUBuildingList.find(qwNpcNameID);
    PROCESS_ERROR(iter != m_arrUBuildingList.end());

    return &(iter->second);
Exit0:
    return NULL;
}

BOOL CGSGuild::AddUpgradeBuilding(NAMEID qwNameID, INT32 nType, INT32 nLevel)
{
    INT32   nRetCode = 0;
    GS_U_G_BUILDING_MAP::iterator    iter = m_arrUBuildingList.find(qwNameID);
    LOG_PROCESS_ERROR(iter == m_arrUBuildingList.end());

    m_arrUBuildingList.insert(std::make_pair(qwNameID, GS_G_B_DATA(qwNameID, nType, nLevel)));

    return TRUE;
Exit0:
    return FALSE;
}

void CGSGuild::DelUpgradeBuilding(NAMEID qwNameID)
{
    m_arrUBuildingList.erase(qwNameID);
}

GS_G_B_DATA* CGSGuild::FirstUpgradeBuilding()
{
    m_UBCursor = m_arrUBuildingList.begin();
    PROCESS_ERROR(m_UBCursor != m_arrUBuildingList.end());

    return &(m_UBCursor->second);
Exit0:
    return NULL;
}

GS_G_B_DATA* CGSGuild::NextUpgradeBuilding()
{
    ++m_UBCursor;
    PROCESS_ERROR(m_UBCursor != m_arrUBuildingList.end());

    return &(m_UBCursor->second);
Exit0:
    return NULL;
}

BOOL CGSGuild::AddAlliance(UINT32 dwTargetID)
{
    LOG_PROCESS_ERROR(m_nAllianceNum >= 0 && m_nAllianceNum < GUILD_MAX_ALLIANCE_NUM);

    m_arrAlliance[m_nAllianceNum++] = dwTargetID;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSGuild::DelAlliance(UINT32 dwTargetID)
{
    LOG_PROCESS_ERROR(m_nAllianceNum > 0 && m_nAllianceNum <= GUILD_MAX_ALLIANCE_NUM);

    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        if (dwTargetID == m_arrAlliance[i])
        {
            m_arrAlliance[i] = m_arrAlliance[m_nAllianceNum-1];
            --m_nAllianceNum;
            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

BOOL CGSGuild::IsAlliance(UINT32 dwTargetID)
{
    LOG_PROCESS_ERROR(m_nAllianceNum >= 0 && m_nAllianceNum <= GUILD_MAX_ALLIANCE_NUM);

    for (INT32 i = 0; i < m_nAllianceNum; ++i)
    {
        if (dwTargetID == m_arrAlliance[i])
        {
            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

BOOL CGSGuild::AddEnemy(UINT32 dwTargetID)
{
    LOG_PROCESS_ERROR(m_nEnemyNum >= 0 && m_nEnemyNum < GUILD_MAX_ENEMY_NUM);

    m_arrEnemy[m_nEnemyNum++] = dwTargetID;
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSGuild::DelEnemy(UINT32 dwTargetID)
{
    LOG_PROCESS_ERROR(m_nEnemyNum > 0 && m_nEnemyNum <= GUILD_MAX_ENEMY_NUM);

    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        if (dwTargetID == m_arrEnemy[i])
        {
            m_arrEnemy[i] = m_arrEnemy[m_nEnemyNum-1];
            --m_nEnemyNum;
            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

BOOL CGSGuild::IsEnemy(UINT32 dwTargetID)
{
    LOG_PROCESS_ERROR(m_nEnemyNum >= 0 && m_nEnemyNum <= GUILD_MAX_ENEMY_NUM);

    for (INT32 i = 0; i < m_nEnemyNum; ++i)
    {
        if (dwTargetID == m_arrEnemy[i])
        {
            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

INT32 CGSGuild::GetVariable(INT32 nIndex)
{
    LOG_PROCESS_ERROR( 0 <= nIndex && nIndex < MAX_GUILD_VARIABLE_COUNT);

    return m_arrVariables[nIndex];
Exit0:
    return 0;
}

BOOL CGSGuild::SetVariable(INT32 nIndex, INT32 nValue)
{
    LOG_PROCESS_ERROR(0 <= nIndex && nIndex < MAX_GUILD_VARIABLE_COUNT);

    m_arrVariables[nIndex] = nValue;
    return TRUE;
Exit0:
    return FALSE;
}

INT32 CGSGuild::GetRes(INT32 nResType)
{
    switch (nResType)
    {
    case GUILD_ATTR_MONEY:
        return m_nMoney;
    case GUILD_ATTR_HERB:
        return m_nHerb;
    case GUILD_ATTR_WOOD:
        return m_nWood;
    case GUILD_ATTR_CRYSTAL:
        return m_nCrystal;
    case GUILD_ATTR_SILVER:
        return m_nSilver;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

Exit0:
    return 0;
}

INT32 CGSGuild::GetMaxRes(INT32 nResType)
{
    switch (nResType)
    {
    case GUILD_ATTR_MONEY:
        return GetMaxStorage(GUILD_B_STORAGE);
    case GUILD_ATTR_HERB:
        return GetMaxStorage(GUILD_B_HERB_STORAGE);
    case GUILD_ATTR_WOOD:
        return GetMaxStorage(GUILD_B_WOOD_STORAGE);
    case GUILD_ATTR_CRYSTAL:
        return GetMaxStorage(GUILD_B_CRYSTAL_STORAGE);
    case GUILD_ATTR_SILVER:
        return GetMaxStorage(GUILD_B_SILVER_STORAGE);
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

Exit0:
    return 0;
}

INT32 CGSGuild::GetMaxStorage(INT32 nType)
{
    INT32   nLevel = 0;
    const GUILD_BUILDING_DATA     *pTemplate = NULL;

    LOG_PROCESS_ERROR(nType == GUILD_B_STORAGE ||
        nType == GUILD_B_WOOD_STORAGE || nType == GUILD_B_HERB_STORAGE ||
        nType == GUILD_B_CRYSTAL_STORAGE || nType == GUILD_B_SILVER_STORAGE);

    if (m_FunBuildingList[nType].dwReviveTime > 0)
        nLevel = 0;
    else
        nLevel = m_FunBuildingList[nType].nLevel;

    pTemplate = GetGuildBuildingData(nType, nLevel);
    LOG_PROCESS_ERROR(pTemplate);

    return pTemplate->nParam1;
Exit0:
    return 0;
}

BOOL CGSGuild::HasEnrolledSabac(void)
{
	UINT32 dwTime = CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone();
	dwTime = dwTime / 86400;

	for (INT32 nIndex = 0; nIndex < MAX_SABAC_ENROLL_COUNT; nIndex++)
	{
		UINT32 dwEnrollTime = m_dwSabacEnrollTime[nIndex] + CGameWorld::GetInstance().GetTimeZone();
		dwEnrollTime = dwEnrollTime / 86400;
		if (dwTime == dwEnrollTime)
			return TRUE;
	}

	return FALSE;
}

BOOL CGSGuild::IsJoinSabacToday(void)
{
	UINT32 dwTime = (CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone()) / 86400;

	for (INT32 nIndex = 0; nIndex < MAX_SABAC_ENROLL_COUNT; nIndex++)
	{
		UINT32 dwEnrollTime = (m_dwSabacEnrollTime[nIndex] + CGameWorld::GetInstance().GetTimeZone()) / 86400;

		if (dwTime == dwEnrollTime + 3)
			return TRUE;
	}

	return FALSE;
}

void CGSGuild::SetSabacEnrollTime(UINT32 dwTime)
{
	UINT32 dwCurrentTime = CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone();
	dwCurrentTime /= 86400;

	for (INT32 nIndex = 0; nIndex < MAX_SABAC_ENROLL_COUNT; nIndex++)
	{
		UINT32 dwEnrollTime = m_dwSabacEnrollTime[nIndex] + CGameWorld::GetInstance().GetTimeZone();
		dwEnrollTime /= 86400;
		if (dwCurrentTime - dwEnrollTime > 3)
		{
			m_dwSabacEnrollTime[nIndex] = dwTime;
			break;
		}
	}
Exit0:
	return;
}

BOOL CGSGuild::IsActivityOpen(INT32 nActivityID)
{
    INT32       nRetCode = 0;
    const GUILD_ACTIVITY_DATA   *pData = NULL;

    pData = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().GetData(GUILD_ACTIVITY_ID(nActivityID, GetLevel()));
    LOG_PROCESS_ERROR(pData);

    return GetVariable(pData->nGuildVariableIndex) >= CGameWorld::GetInstance().GetGameTime();
Exit0:
    return FALSE;
}

INT32 CGSGuild::GetActivityMaxRepeatNum(INT32 nActivityID)
{
    INT32       nRetCode = 0;
    const GUILD_ACTIVITY_DATA   *pData = NULL;

    pData = CDataManager<GUILD_ACTIVITY_DATA>::GetInstance().GetData(GUILD_ACTIVITY_ID(nActivityID, GetLevel()));
    LOG_PROCESS_ERROR(pData);

    return pData->nRepeatNum;
Exit0:
    return 0;
}

#endif // !_ROBOT


