#include "stdafx.h"
#include <time.h>
#include "message_handler.h"
#include "imessage_notifier.h"
#include "c2g_message.h"
#include "g2m_message.h"
#include "g2c_message.h"
#include "arena_def.h"

#include "game_world.h"

using namespace ExternalMessage;

#ifndef _ROBOT


void CCliMessageHandler::OnArenaJoinSeason(CRole* poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    struct tm   *pTm  = NULL;
    time_t      tTime = 0;
    C2G_ARENA_JOIN_SEASON   *pMsg = (C2G_ARENA_JOIN_SEASON*)pData;

    LOG_PROCESS_ERROR(poRole);

    // check join time
    tTime = CGameWorld::GetInstance().GetGameTime();
    pTm = localtime(&tTime);
    LOG_PROCESS_ERROR(pTm);

    //LOG_PROCESS_ERROR(pTm->tm_wday == ARENA_WEEKLY_DAY_ONE || pTm->tm_wday == ARENA_WEEKLY_DAY_TWO || pTm->tm_wday == ARENA_WEEKLY_DAY_THREE);
    //LOG_PROCESS_ERROR(pTm->tm_hour > ARENA_APPLY_HOUR_BEGIN && pTm->tm_hour < ARENA_APPLY_HOUR_END);

    LOG_PROCESS_ERROR(ARENA_JOIN_SEASON_LVL_LIMIT <= poRole->GetLevel());
    LOG_PROCESS_ERROR(ARENA_JOIN_SEASON_MONEY_CONSUMPTION <= poRole->GetItemBag()->GetMoney(MONEYTYPE_02));

    nRetCode = poRole->GetArena().IsInSeason();
    LOG_PROCESS_ERROR(!nRetCode);

	nRetCode = poRole->GetItemBag()->SubMoney(MONEYTYPE_02, ARENA_JOIN_SEASON_MONEY_CONSUMPTION, MONEYLOG_ARENA_JOIN_SEASON, 0);
    LOG_PROCESS_ERROR(errSuccess == nRetCode);

    nRetCode = poRole->GetArena().JoinNewSeason();
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnArenaQuitSeason(CRole* poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    C2G_ARENA_QUIT_SEASON   *pMsg = (C2G_ARENA_QUIT_SEASON*)pData;

    LOG_PROCESS_ERROR(poRole);

    LOG_PROCESS_ERROR(ARENA_JOIN_SEASON_LVL_LIMIT <= poRole->GetLevel());

    nRetCode = poRole->GetArena().IsInSeason();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poRole->GetArena().QuitSeason();
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnArenaMatch(CRole* poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    struct tm   *pTm  = NULL;
    time_t      tTime = 0;
    C2G_ARENA_MATCH   *pMsg = (C2G_ARENA_MATCH*)pData;

    LOG_PROCESS_ERROR(poRole);

    tTime = CGameWorld::GetInstance().GetGameTime();
    pTm = localtime(&tTime);
    LOG_PROCESS_ERROR(pTm);

    //LOG_PROCESS_ERROR(pTm->tm_wday == ARENA_WEEKLY_DAY_ONE || pTm->tm_wday == ARENA_WEEKLY_DAY_TWO || pTm->tm_wday == ARENA_WEEKLY_DAY_THREE);
    //LOG_PROCESS_ERROR(pTm->tm_hour > ARENA_MATCH_HOUR_BEGIN && pTm->tm_hour < ARENA_MATCH_HOUR_END);

    LOG_PROCESS_ERROR(ARENA_JOIN_SEASON_LVL_LIMIT <= poRole->GetLevel());

    nRetCode = poRole->GetArena().IsInSeason();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoMSArenaMatch(poRole->GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

void CCliMessageHandler::OnArenaGetReward(CRole* poRole, const char *pData)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    struct tm   *pTm  = NULL;
    time_t      tTime = 0;
    C2G_ARENA_GET_REWARD    *pMsg = (C2G_ARENA_GET_REWARD*)pData;

    LOG_PROCESS_ERROR(poRole);

    tTime = CGameWorld::GetInstance().GetGameTime();
    pTm = localtime(&tTime);
    LOG_PROCESS_ERROR(pTm);

    //LOG_PROCESS_ERROR(pTm->tm_wday == ARENA_WEEKLY_DAY_ONE || pTm->tm_wday == ARENA_WEEKLY_DAY_TWO || pTm->tm_wday == ARENA_WEEKLY_DAY_THREE);
    //LOG_PROCESS_ERROR(pTm->tm_hour >= ARENA_MATCH_HOUR_END);
    //LOG_PROCESS_ERROR(pTm->tm_min >= ARENA_GET_REWARD_MINUTE);

    LOG_PROCESS_ERROR(ARENA_JOIN_SEASON_LVL_LIMIT <= poRole->GetLevel());

    nRetCode = poRole->GetArena().CanGetCommonReward();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CCliMessageHandler::GetInstance().DoMSArenaReward(poRole->GetObjID());
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_ROLE(poRole, ktLogged);
    else
		poRole->ErrorCodeNotify(nResult, 0, 0);
    return;
}

BOOL CCliMessageHandler::DoClientArenaSyncData(CRole *poRole)
{
    INT32       nRetCode = 0;
    G2C_ARENA_SYNC_DATA     *pMsg = NULL;
    struct tm   *pTm = NULL;
    time_t      tTime = 0;
    
    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_ARENA_SYNC_DATA*)m_poPacketAllocator->Alloc(sizeof(G2C_ARENA_SYNC_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_arena_sync_data;
    pMsg->bySeasonNum       = poRole->GetArena().GetSeasonNum();
    pMsg->byIsInSeason      = poRole->GetArena().IsInSeason();
    pMsg->byCurSeasonVicTime = poRole->GetArena().GetCurSeasonVicTime();
    pMsg->byPointSeasonVicTime = poRole->GetArena().GetPointSeasonVicTime();

    tTime = CGameWorld::GetInstance().GetGameTime();
    pTm = localtime(&tTime);
    LOG_PROCESS_ERROR(pTm);

    //if (   (pTm->tm_wday == ARENA_WEEKLY_DAY_ONE || pTm->tm_wday == ARENA_WEEKLY_DAY_TWO || pTm->tm_wday == ARENA_WEEKLY_DAY_THREE) 
    //    && (pTm->tm_hour > ARENA_MATCH_HOUR_BEGIN && pTm->tm_hour < ARENA_MATCH_HOUR_END))
    //{
        pMsg->byIsJJCMatchTime = TRUE;
    //}

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ARENA_SYNC_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoClientArenaJoinSeasonAck(CRole *poRole, UINT8 bySeasonNum)
{
    INT32       nRetCode = 0;
    G2C_ARENA_JOIN_NEW_SEASON_ACK     *pMsg = NULL;
    
    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_ARENA_JOIN_NEW_SEASON_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_ARENA_JOIN_NEW_SEASON_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_arena_join_new_season_ack;
    pMsg->bySeasonNum = bySeasonNum;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ARENA_JOIN_NEW_SEASON_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoClientArenaQuitSeasonAck(CRole *poRole, UINT8 bySeasonNum, UINT8 byPointVictoryTimes)
{
    INT32       nRetCode = 0;
    G2C_ARENA_QUIT_SEASON_ACK     *pMsg = NULL;
    
    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_ARENA_QUIT_SEASON_ACK*)m_poPacketAllocator->Alloc(sizeof(G2C_ARENA_QUIT_SEASON_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_arena_quit_season_ack;
    pMsg->bySeasonNum = bySeasonNum;
    pMsg->byPointVictoryTimes = byPointVictoryTimes;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ARENA_QUIT_SEASON_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoClientArenaWin(CRole *poRole)
{
    INT32       nRetCode = 0;
    G2C_ARENA_WIN     *pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);
    pMsg = (G2C_ARENA_WIN*)m_poPacketAllocator->Alloc(sizeof(G2C_ARENA_WIN));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_arena_win;

    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ARENA_WIN));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoClientArenaDefeat(CRole *poRole)
{
    INT32       nRetCode = 0;
    G2C_ARENA_DEFEAT     *pMsg = NULL;

    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_ARENA_DEFEAT*)m_poPacketAllocator->Alloc(sizeof(G2C_ARENA_DEFEAT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_arena_defeat;
    
    nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ARENA_DEFEAT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}











BOOL CCliMessageHandler::DoMSArenaSyncData(UINT32 dwRoleID, UINT8 byPointSeasonTimes, INT32 nPointSeasonEndTime)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_ARENA_SYNC_DATA *pMsg = (InternalMessage::G2M_ARENA_SYNC_DATA*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ARENA_SYNC_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_arena_sync_data;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byPointSeasonTimes = byPointSeasonTimes;
    pMsg->nPointSeasonEndTime = nPointSeasonEndTime;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ARENA_SYNC_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSArenaJoinSeason(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_ARENA_JOIN_SEASON    *pMsg = (InternalMessage::G2M_ARENA_JOIN_SEASON*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ARENA_JOIN_SEASON));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_arena_join_season;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ARENA_JOIN_SEASON));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSArenaMatch(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;

    InternalMessage::G2M_ARENA_MATCH    *pMsg = (InternalMessage::G2M_ARENA_MATCH*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ARENA_MATCH));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_arena_match;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ARENA_MATCH));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSArenaWin(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_ARENA_WIN *pMsg = (InternalMessage::G2M_ARENA_WIN*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ARENA_WIN));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_arena_win;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ARENA_WIN));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSArenaDefeated(UINT32 dwRoleID, UINT8 byPointSeasonTimes, INT32 nPointSeasonEndTime)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_ARENA_DEFEATED *pMsg = (InternalMessage::G2M_ARENA_DEFEATED*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ARENA_DEFEATED));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_arena_defeated;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byPointSeasonTimes = byPointSeasonTimes;
    pMsg->nPointSeasonEndTime = nPointSeasonEndTime;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ARENA_DEFEATED));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMSArenaReward(UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    InternalMessage::G2M_ARENA_REWARD *pMsg = (InternalMessage::G2M_ARENA_REWARD*)m_poPacketAllocator->Alloc(sizeof(InternalMessage::G2M_ARENA_REWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = InternalMessage::g2m_arena_reward;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = m_poNotifier->SendToMS((PACKET_BUFFER*)pMsg, sizeof(InternalMessage::G2M_ARENA_REWARD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

#endif


