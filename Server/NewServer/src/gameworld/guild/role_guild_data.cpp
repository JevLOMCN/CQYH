#include "stdafx.h"
#include "role_guild_data.h"

#include "guild_config.h"
#include "game_world.h"
#include "message_handler.h"

CRoleGuildData::CRoleGuildData()
{
    m_nDonateMoneyPerDay = 0;
    m_nDonateMoneyDayTime = 0;

    m_nDonateResPerDay = 0;
    m_nDonateResDayTime = 0;
    m_nSalary = 0;
    m_dwCD = 0;
    memset(m_Achievement, 0, sizeof(m_Achievement));
    m_poRole = NULL;
}

CRoleGuildData::~CRoleGuildData()
{
}

BOOL CRoleGuildData::Init(CRole *poRole)
{
    LOG_PROCESS_ERROR(poRole);

    m_nDonateMoneyPerDay = 0;
    m_nDonateMoneyDayTime = 0;

    m_nDonateResPerDay = 0;
    m_nDonateResDayTime = 0;

    m_nSalary = 0;

    m_RecordList.clear();
    m_Cursor = m_RecordList.end();
    m_dwCD = 0;
    memset(m_Achievement, 0, sizeof(m_Achievement));

    m_poRole = poRole;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CRoleGuildData::UnInit()
{
    m_poRole = NULL;
    return TRUE;
}

BOOL CRoleGuildData::AddBuyAuraCount(INT32 nType)
{
    UINT32      dwDayTime = 0;
    GUILD_BUY_AURA_RECORD   data;

    dwDayTime = CGameWorld::GetInstance().GetGameTime() + get_time_zone();
    dwDayTime /= ONE_DAY_SECOND;

    for (GUILD_AURA__RECORD_LIST::iterator iter = m_RecordList.begin(); iter != m_RecordList.end(); ++iter)
    {
        if (nType == iter->nType)
        {
            if (dwDayTime != iter->nTime)
            {
                iter->byCount = 1;
                iter->nTime = dwDayTime;
                PROCESS_SUCCESS(TRUE);
            }
            else
            {
                ++iter->byCount;
                PROCESS_SUCCESS(TRUE);
            }
        }
    }

    data.nType = nType;
    data.nTime = dwDayTime;
    data.byCount = 1;
    m_RecordList.push_back(data);
    
Exit1:
    return TRUE;
}

INT32 CRoleGuildData::GetBuyAuraCount(INT32 nType)
{
    INT32       dwDayTime = CGameWorld::GetInstance().GetGameTime() + get_time_zone();
    dwDayTime /= ONE_DAY_SECOND;

    for (GUILD_AURA__RECORD_LIST::iterator iter = m_RecordList.begin(); iter != m_RecordList.end(); ++iter)
    {
        if (nType == iter->nType)
        {
            if (dwDayTime == iter->nTime)
            {
                return iter->byCount;
            }
            break;
        }
    }

Exit1:
    return 0;
}

GUILD_BUY_AURA_RECORD* CRoleGuildData::GetFirstRecord()
{
    m_Cursor = m_RecordList.begin();
    PROCESS_ERROR(m_Cursor != m_RecordList.end());

    return &(*m_Cursor);
Exit0:
    return NULL;
}

GUILD_BUY_AURA_RECORD* CRoleGuildData::GetNextRecord()
{
    ++m_Cursor;
    PROCESS_ERROR(m_Cursor != m_RecordList.end());

    return &(*m_Cursor);
Exit0:
    return NULL;
}

INT32 CRoleGuildData::CalculateFundLimit(INT32 nPosition, INT32 nMoney)
{
    UINT32  nCurrentDay = CGameWorld::GetInstance().GetGameTime() + get_time_zone();
    nCurrentDay /= ONE_DAY_SECOND;
    INT32       nFinalMoney = 0;

    LOG_PROCESS_ERROR(nPosition > GUILD_POSITION_NONE && nPosition < GUILD_POSITION_TOTAL);

    // day limit
    if (nCurrentDay == m_nDonateMoneyDayTime)
    {
        PROCESS_ERROR(m_nDonateMoneyPerDay <= g_GuildConfig.nMaxDonateMoneyPerDay[nPosition]);

        nFinalMoney = g_GuildConfig.nMaxDonateMoneyPerDay[nPosition] - m_nDonateMoneyPerDay > nMoney ? nMoney : g_GuildConfig.nMaxDonateMoneyPerDay[nPosition] - m_nDonateMoneyPerDay;
        m_nDonateMoneyPerDay += nFinalMoney;
    }
    else
    {
        nFinalMoney = nMoney > g_GuildConfig.nMaxDonateMoneyPerDay[nPosition] ? g_GuildConfig.nMaxDonateMoneyPerDay[nPosition] : nMoney;
        m_nDonateMoneyPerDay = nFinalMoney;
        m_nDonateMoneyDayTime = nCurrentDay;
    }

    return nFinalMoney;
Exit0:
    return 0;
}

INT32 CRoleGuildData::CalculateResLimit(INT32 nPosition, INT32 nRes)
{
    UINT32  nCurrentDay = CGameWorld::GetInstance().GetGameTime() + get_time_zone();
    nCurrentDay /= ONE_DAY_SECOND;
    INT32       nFinalRes = 0;

    LOG_PROCESS_ERROR(nPosition > GUILD_POSITION_NONE && nPosition < GUILD_POSITION_TOTAL);

    // day limit
    if (nCurrentDay == m_nDonateResDayTime)
    {
        PROCESS_ERROR(m_nDonateResPerDay <= g_GuildConfig.nMaxDonateResPerDay[nPosition]);

        nFinalRes = g_GuildConfig.nMaxDonateResPerDay[nPosition] - m_nDonateResPerDay > nRes ? nRes : g_GuildConfig.nMaxDonateResPerDay[nPosition] - m_nDonateResPerDay;
        m_nDonateResPerDay += nFinalRes;
    }
    else
    {
        nFinalRes = nRes > g_GuildConfig.nMaxDonateResPerDay[nPosition] ? g_GuildConfig.nMaxDonateResPerDay[nPosition] : nRes;
        m_nDonateResPerDay = nFinalRes;
        m_nDonateResDayTime = nCurrentDay;
    }

    return nFinalRes;
Exit0:
    return 0;
}

void CRoleGuildData::SetCD(UINT32 dwCD)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(m_poRole);

    m_dwCD = dwCD;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncGuild(m_poRole);
    LOG_CHECK_ERROR(nRetCode);
#endif // _ROBOT

Exit0:
    return;
}


BOOL CRoleGuildData::CanRewardAchievement(INT32 nID)
{
    INT32   nX = nID / 8;
    INT32   nY = nID % 8;

    LOG_PROCESS_ERROR(nX < GUILD_MAX_ACHI_ARRAY_LENGTH);

    return (m_Achievement[nX] & (1 << nY)) == FALSE;
Exit0:
    return FALSE;
}

BOOL CRoleGuildData::SetAchievementReward(INT32 nID)
{
    INT32   nX = nID / 8;
    INT32   nY = nID % 8;

    LOG_PROCESS_ERROR(nX < GUILD_MAX_ACHI_ARRAY_LENGTH);

    return m_Achievement[nX] |= (1 << nY);
Exit0:
    return FALSE;
}

