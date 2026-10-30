#include "stdafx.h"
#include "arena.h"
#include "game_world.h"
#include "message_handler.h"
#include "role.h"

CArena::CArena() :
m_pOwner(NULL),
m_nJoinTime(0),
m_bIsInSeason(0),
m_nSeasonNum(0),
m_byCurSeasonVictoryTimes(0),
m_byPointSeasonVictoryTimes(0),
m_nCurSeasonEndTime(0),
m_nPointSeasonEndTime(0),
m_nHP(0)
{

}

CArena::~CArena()
{

}

BOOL CArena::Init(CRole *pOwner)
{
    LOG_PROCESS_ERROR(pOwner);
    m_pOwner = pOwner;

    m_nJoinTime = 0;
    m_bIsInSeason = FALSE;
    m_nSeasonNum = 0;
    m_byPointSeasonVictoryTimes = 0;
    m_nPointSeasonEndTime = 0;
    m_nCurSeasonEndTime = 0;
    m_byCurSeasonVictoryTimes = 0;
    m_nHP = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArena::UnInit()
{
    m_pOwner = NULL;
    m_nJoinTime = 0;
    m_bIsInSeason = FALSE;
    m_nSeasonNum = 0;
    m_byPointSeasonVictoryTimes = 0;
    m_nPointSeasonEndTime = 0;
    m_nCurSeasonEndTime = 0;
    m_byCurSeasonVictoryTimes = 0;
    m_nHP = 0;

    return TRUE;
//Exit0:
//    return FALSE;
}

BOOL CArena::IsInSeason()
{
    if (m_nJoinTime == (CGameWorld::GetInstance().GetGameTime() / 86400) && m_bIsInSeason)
        return TRUE;
    return FALSE;
}

BOOL CArena::JoinNewSeason()
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(m_pOwner);

    if (m_nJoinTime != CGameWorld::GetInstance().GetGameTime() / 86400)
    {
        m_nJoinTime = CGameWorld::GetInstance().GetGameTime() / 86400;
        m_byPointSeasonVictoryTimes = 0;
        m_nPointSeasonEndTime = 0;
        m_nCurSeasonEndTime = 0;
        m_byCurSeasonVictoryTimes = 0;
        m_nSeasonNum = 0;
    }
    
    m_bIsInSeason = TRUE;
    ++m_nSeasonNum;

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSArenaJoinSeason(m_pOwner->GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoClientArenaJoinSeasonAck(m_pOwner, m_nSeasonNum);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArena::QuitSeason()
{
    return Defeated();
}

BOOL CArena::CanGetCommonReward()
{
    if (m_nJoinTime == (CGameWorld::GetInstance().GetGameTime() / 86400) && m_nSeasonNum > 0)
        return TRUE;
    return FALSE;
}

BOOL CArena::GetCommonReward(UINT32 dwItemID, INT32 nMoney)
{
    INT32       nRetCode = 0;
    INT32       nItemNum = 0;
    
    LOG_PROCESS_ERROR(m_pOwner);

    m_nSeasonNum = 0;

	nRetCode = m_pOwner->GetItemBag()->AddItem(dwItemID, m_nSeasonNum, ITEMLOG_ARENA_COMMON_REWARD, 0, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    if (nMoney > 0)
    {
		nRetCode = m_pOwner->GetItemBag()->AddMoney(MONEYTYPE_02, nMoney, MONEYLOG_ARENA_RANK_REWARD, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }    

#ifndef _ROBOT
	nRetCode = m_pOwner->ErrorCodeNotify(errArenaGetRewardSucc, 0, 0);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArena::Win()
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(m_pOwner);

    ++m_byCurSeasonVictoryTimes;
    m_nCurSeasonEndTime = CGameWorld::GetInstance().GetGameTime();
    m_nHP = m_pOwner->GetHp() * (RATE_MAX + g_GWConfig.nArenaWinIncHpPercent) / RATE_MAX; 

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSArenaWin(m_pOwner->GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoClientArenaWin(m_pOwner);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CArena::Defeated()
{
    INT32       nRetCode = 0;
    LOG_PROCESS_ERROR(m_pOwner);

    nRetCode = IsInSeason();
    LOG_PROCESS_ERROR(nRetCode);

    m_bIsInSeason = FALSE;
    m_nHP = 0;
    if (m_byCurSeasonVictoryTimes > m_byPointSeasonVictoryTimes)
    {
        m_byPointSeasonVictoryTimes = m_byCurSeasonVictoryTimes;
        m_nPointSeasonEndTime = m_nCurSeasonEndTime;
        m_nCurSeasonEndTime = 0;
        m_byCurSeasonVictoryTimes = 0;
    }

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSArenaDefeated(m_pOwner->GetObjID(), m_byPointSeasonVictoryTimes, m_nPointSeasonEndTime);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoClientArenaQuitSeasonAck(m_pOwner, m_nSeasonNum, m_byPointSeasonVictoryTimes);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}





