#include "stdafx.h"
#include "message_handler.h"

#include "time_ex.h"
#include "dump.h"
#include "zlib.h"

#include "t2m_message.h"
#include "m2t_message.h"
#include "m2c_message.h"
#include "c2m_message.h"

#include "chat_def.h"

#include "ms_session_mgr.h"
#include "team/team.h"
#include "team/room.h"
#include "client/usermgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "scene_data.h"
#include "manageserver.h"
#include "scene/scenemgr.h"
#include "master/master_mgr.h"
#include "mail/mail_mgr.h"
#include "queuematch/battlequeuemgr.h"
#include "guild/ms_guild_mgr.h"
#include "./guild/guild_storage.h"
#include "./guild/ms_guild_mgr.h"
#include "auction/auction_mgr.h"
#include "exchange/exchange_mgr.h"
#include "award/award_mgr.h"
#include "real_money_shop/oa_bill_mgr.h"
#include "real_money_shop/real_money_shop.h"
#include "lua_script.h"
#include "script_mgr.h"
#include "ranklist/ranklist.h"
#include "gm/gm_def.h"
#include "gm/gm_manager.h"
#include "game_world_config.h"

#include "server_config.h"

#include "arena/arena_mgr.h"
#include "db/ms_sql_stmt_list.h"

using namespace ExternalMessage;

void CMessageHandler::OnGTClientMsg(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    MS_USER* pUser = NULL;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;
    InternalMessage::T2M_CLIENT_MSG* pMsg = (InternalMessage::T2M_CLIENT_MSG*)pPacketBuffer;

    MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pMsg->szData;
    LOG_PROCESS_ERROR(m_ExternalMessageFuncList[pHeader->wMsg]);

    pUser = CUserMgr::GetInstance().FindUserBySessionID(pMsg->dwClientSessionID);
    LOG_PROCESS_ERROR(pUser);
    LOG_PROCESS_ERROR(pUser->eState == USER_STATE_INGAME);  // An invalid state is an expected condition

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->dwCliSessionID == pMsg->dwClientSessionID);

#if defined(WIN32)
    __try
    {
        (this->*m_ExternalMessageFuncList[pHeader->wMsg])(pPacketBuffer + sizeof(InternalMessage::T2M_CLIENT_MSG), dwSize, pstOnlineRole);
    }
    __except (DumpExceptionFilter(GetExceptionInformation()))
    {
        goto Exit0;
    }
#elif defined(__linux__)
    (this->*m_ExternalMessageFuncList[pHeader->wMsg])(pPacketBuffer + sizeof(InternalMessage::T2M_CLIENT_MSG), dwSize, pstOnlineRole);
#endif

    return;
Exit0:
    CRI("failed to process client message %d size %d", pHeader->wMsg, dwSize);

    if (pMsg->dwClientSessionID)
        KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnClientQueryRoleInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_QUERY_ROLE_INFO* pMsg = (C2M_QUERY_ROLE_INFO*)pPacketBuffer;

    CMsGuild* pGuild = NULL;
    MS_ROLE_DATA* pRoleData = NULL;

    if (pstOnlineRole->dwLastQueryRoleInfoTime + 1 < CManageServer::GetInstance().GetGameTime())
    {
        pstOnlineRole->dwLastQueryRoleInfoTime = CManageServer::GetInstance().GetGameTime();
        pstOnlineRole->byQueryRoleInfoCount = 0;
    }
    ++pstOnlineRole->byQueryRoleInfoCount;
    LOG_PROCESS_ERROR(pstOnlineRole->byQueryRoleInfoCount < MAX_QUERY_ROLE_INFO_PER_SEC);

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    PROCESS_ERROR(pRoleData);

    pGuild = CMsGuildMgr::GetInstance().FindGuildByID(pRoleData->dwGuildID);
    if (pGuild)
    {
        nRetCode = DoClientSyncRoleInfo(pstOnlineRole->dwCliSessionID, pRoleData, pGuild->GetName(), pMsg->dwCRC);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = DoClientSyncRoleInfo(pstOnlineRole->dwCliSessionID, pRoleData, NULL, pMsg->dwCRC);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return;
Exit0:
    CRI("query error role id %d", pMsg->dwRoleID);
    return;
}

// ===== Join team (used when either party already has a team)
void CMessageHandler::OnClientTeamJoin(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errTeamFailed;
    MS_USER* pstReqUser = NULL;
    MS_USER* pstTargetUser = NULL;
    MS_ONLINE_ROLE_DATA* pstTgtOnlineRole = NULL;
    CTeam* poTeam = NULL;
    C2M_TEAM_JOIN* pMsg = (C2M_TEAM_JOIN*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);

    pstReqUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(pstReqUser);
    PROCESS_ERROR_RET_CODE(pstReqUser->eState == USER_STATE_INGAME, errTeamNotPermit);

    LOG_PROCESS_ERROR_RET_CODE(!pstReqUser->bFreeMode, errFreeMode);

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pstReqUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    pstTgtOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTargetRoleID);
    PROCESS_ERROR_RET_CODE(pstTgtOnlineRole, errTeamOffLine);
    LOG_PROCESS_ERROR(pstTgtOnlineRole->pRoleData);
    PROCESS_ERROR_RET_CODE(pstTgtOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    // Self-team request; trigger is unknown, so ignore it for now
    if (pstOnlineRole->pRoleData->dwRoleID == pMsg->dwTargetRoleID)
    {
        UINT32 dwNewTeamID = 0;
        LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwTeamID == 0, errTeamAlreadyInTeam);

        dwNewTeamID = GenerateTeamID();
        poTeam = CTeamMgr::GetInstance().New(dwNewTeamID);
        LOG_PROCESS_ERROR(poTeam);

        nRetCode = poTeam->Init(GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID),
            dwNewTeamID, pstOnlineRole->pRoleData->szRoleName, 11, etaFreeForAll, ITEMCOLOR_WHITE);
        LOG_PROCESS_ERROR(nRetCode);

        poTeam->AddMember(pMsg->dwTargetRoleID);
        poTeam->SetLeader(pMsg->dwTargetRoleID);

        nRetCode = DoClientTeamJoin(pstOnlineRole->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoGSTeamNew(poTeam);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoDLCreateTeam(CManageServer::GetInstance().GetGameTime(), pstOnlineRole->pRoleData->byGroup,
            pstOnlineRole->pRoleData->dwRoleID, dwNewTeamID);
        LOG_CHECK_ERROR(nRetCode);
    }
    else
    {
        UINT32 dwTeamID = INVALID_OBJECT_ID;
        UINT8  eAnswerType = etatInvite;

        pstTargetUser = pstTgtOnlineRole->pUser;
        PROCESS_ERROR_RET_CODE(pstTargetUser->eState == USER_STATE_INGAME, errTeamOffLine);
        PROCESS_ERROR_RET_CODE(pstTgtOnlineRole->pRoleData->dwTeamID == 0 ||
            pstOnlineRole->pRoleData->dwTeamID == 0, errTeamAlreadyInTeam);

        LOG_PROCESS_ERROR(GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID) == GET_GROUP_INDEX_FROM_SESSION_ID(pstTgtOnlineRole->dwCliSessionID));

        if (pstTgtOnlineRole->pRoleData->dwTeamID > 0)
        {
            eAnswerType = etatEnter;

            poTeam = CTeamMgr::GetInstance().Find(pstTgtOnlineRole->pRoleData->dwTeamID);
            LOG_PROCESS_ERROR_RET_CODE(poTeam, errTeamNotExist);

            pstTgtOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poTeam->GetLeader());
            LOG_PROCESS_ERROR_RET_CODE(pstTgtOnlineRole, errTeamOffLine);
            LOG_PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() < poTeam->GetMemberSizeLimit(), errTeamFull);
        }
        else if (pstOnlineRole->pRoleData->dwTeamID > 0)
        {
            eAnswerType = etatInvite;

            poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
            LOG_PROCESS_ERROR_RET_CODE(poTeam, errTeamNotExist);

            LOG_PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() < poTeam->GetMemberSizeLimit(), errTeamFull);
            PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwRoleID == poTeam->GetLeader(), errTeamNotLeader);
        }

        for (INT32 nIndex = 1; nIndex < MAX_JOIN_TEAM_ROLE_COUNT; nIndex++)
        {
            pstTgtOnlineRole->dwJoinTeamRoleID[nIndex - 1] = pstTgtOnlineRole->dwJoinTeamRoleID[nIndex];
        }

        pstTgtOnlineRole->dwJoinTeamRoleID[MAX_JOIN_TEAM_ROLE_COUNT - 1] = pstOnlineRole->pRoleData->dwRoleID;
        nRetCode = DoClientTeamAnswer(pstTgtOnlineRole->dwCliSessionID, eAnswerType,
            pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName);
        LOG_PROCESS_ERROR(nRetCode);

        if (eAnswerType == etatInvite)
            DoErrorNotify(pstOnlineRole->dwCliSessionID, errTeamInvitationSent, 0, 0);    // 3841
        else
            DoErrorNotify(pstOnlineRole->dwCliSessionID, errTeamApplicationSent, 0, 0);
    }

    return;
Exit0:
    if (nResult == errTeamFailed)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

// Leave team
void CMessageHandler::OnClientTeamLeave(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errTeamFailed;
    MS_USER* pstReqUser = NULL;

    MS_ONLINE_ROLE_DATA* pstLeaveRole = NULL;
    UINT32 dwLeaveClientSessionID = 0;
    UINT32 dwOldLeader;
    CTeam* poTeam = NULL;
    C2M_TEAM_LEAVE* pMsg = (C2M_TEAM_LEAVE*)pPacketBuffer;
    const SCENE_DATA* pstSceneData;

    LOG_PROCESS_ERROR(pstOnlineRole);
    pstReqUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(pstReqUser);
    PROCESS_ERROR_RET_CODE(pstReqUser->eState == USER_STATE_INGAME, errTeamOffLine);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);


    pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pstOnlineRole->pRoleData->nSceneTempID);
    LOG_PROCESS_ERROR(pstSceneData);
    LOG_PROCESS_ERROR(pstSceneData->nType != estDungeon && pstSceneData->nType != estGuildDungeon);

    if (pstOnlineRole->pRoleData->dwRoleID == pMsg->dwLeaveRoleID)
    {
        poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR_RET_CODE(poTeam, errTeamNotExist);

        dwOldLeader = poTeam->GetLeader();
        if (dwOldLeader == pMsg->dwLeaveRoleID)
        {
            nRetCode = poTeam->SelectLeader(TRUE);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errTeamNotPermit);
        }

        nRetCode = poTeam->DelMember(pMsg->dwLeaveRoleID);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoClientTeamLeave(pstOnlineRole->dwCliSessionID, poTeam->GetTeamID());
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoDLLeaveTeam(CManageServer::GetInstance().GetGameTime(), pstOnlineRole->pRoleData->byGroup,
            pMsg->dwLeaveRoleID, poTeam->GetTeamID(), poTeam->GetLeader(), poTeam->GetMemberCount());
        LOG_CHECK_ERROR(nRetCode);

        if (poTeam->GetMemberCount() == 0)
        {
            nRetCode = DoDLBreakDwonTeam(CManageServer::GetInstance().GetGameTime(),
                pstOnlineRole->pRoleData->byGroup, poTeam->GetTeamID());
            LOG_CHECK_ERROR(nRetCode);

            if (poTeam->GetActivityType() != etaatInvalid)
            {
                nRetCode = CTeamMgr::GetInstance().DeleteActivityTeam(poTeam->GetActivityType(), poTeam->GetActivityID(), poTeam->GetTeamID());
                LOG_PROCESS_ERROR(nRetCode);
            }

            nRetCode = poTeam->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CTeamMgr::GetInstance().Delete(poTeam);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            nRetCode = DoClientRemoveMember(pMsg->dwLeaveRoleID, poTeam);
            LOG_CHECK_ERROR(nRetCode);

            if (dwOldLeader != poTeam->GetLeader())
            {
                nRetCode = DoClientTeamChangeInfo(poTeam);
                LOG_CHECK_ERROR(nRetCode);

                nRetCode = DoGSTeamChangeInfo(poTeam);
                LOG_PROCESS_ERROR(nRetCode);
            }
        }
    }
    else
    {
        poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR(poTeam);
        LOG_PROCESS_ERROR(poTeam->GetLeader() == pstOnlineRole->pRoleData->dwRoleID);
        LOG_PROCESS_ERROR(poTeam->FindMember(pMsg->dwLeaveRoleID));

        nRetCode = DoDLLeaveTeam(CManageServer::GetInstance().GetGameTime(), pstOnlineRole->pRoleData->byGroup,
            pMsg->dwLeaveRoleID, poTeam->GetTeamID(), poTeam->GetLeader(), poTeam->GetMemberCount());
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = poTeam->DelMember(pMsg->dwLeaveRoleID);
        LOG_PROCESS_ERROR(nRetCode);

        pstLeaveRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwLeaveRoleID);
        if (pstLeaveRole && pstLeaveRole->pRoleData)
        {
            nRetCode = DoClientTeamLeave(pstLeaveRole->dwCliSessionID, poTeam->GetTeamID());
            LOG_CHECK_ERROR(nRetCode);
        }

        nRetCode = DoClientRemoveMember(pMsg->dwLeaveRoleID, poTeam);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = DoGSTeamRemoveMember(pMsg->dwLeaveRoleID, poTeam);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    //if(nResult == errTeamFailed)
    //	KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    //else
    CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

// Team response
void CMessageHandler::OnClientTeamAnswer(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errTeamFailed;
    MS_USER* pstAnswerUser = NULL;
    INT32 nIndex = 0;

    MS_USER* pstRequstUser = NULL;
    MS_ONLINE_ROLE_DATA* pstRequstRole = NULL;

    CTeam* poTeam = NULL;
    C2M_TEAM_ANSWER* pMsg = (C2M_TEAM_ANSWER*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);

    pstAnswerUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(pstAnswerUser);
    PROCESS_ERROR(pstAnswerUser->eState == USER_STATE_INGAME);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    for (; nIndex < MAX_JOIN_TEAM_ROLE_COUNT; nIndex++)
    {
        if (pstOnlineRole->dwJoinTeamRoleID[nIndex] == pMsg->dwAnswerRequestRoleID)
            break;
    }
    PROCESS_ERROR_RET_CODE(nIndex < MAX_JOIN_TEAM_ROLE_COUNT, errTeamRequestNotAvailable);

    pstRequstRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwAnswerRequestRoleID);
    PROCESS_ERROR_RET_CODE(pstRequstRole, errTeamOffLine);
    LOG_PROCESS_ERROR(pstRequstRole->pRoleData);

    pstOnlineRole->dwWaitReqRoleID = 0;
    PROCESS_ERROR_RET_CODE(pstRequstRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    // I decline the team request
    if (pMsg->byAnswerResult == etarDisAgree)
    {
        // Decline invitation
        if (pMsg->byAnswerType == etatInvite)
        {
            CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, errTeamRefusedInvitation,
                pstOnlineRole->pRoleData->dwRoleID, pstRequstRole->pRoleData->dwRoleID);

            CMessageHandler::GetInstance().DoErrorNotify(pstRequstRole->dwCliSessionID, errTeamInvitationRefused,
                pstOnlineRole->pRoleData->dwRoleID, pstRequstRole->pRoleData->dwRoleID);
        }
        // Decline an application
        else if (pMsg->byAnswerType == etatEnter)
        {
            CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, errTeamRefusedApplication,
                pstOnlineRole->pRoleData->dwRoleID, pstRequstRole->pRoleData->dwRoleID);

            CMessageHandler::GetInstance().DoErrorNotify(pstRequstRole->dwCliSessionID, errTeamApplicationRefused,
                pstOnlineRole->pRoleData->dwRoleID, pstRequstRole->pRoleData->dwRoleID);
        }
        PROCESS_SUCCESS(TRUE);
    }

    // The following handles accepting the team request
    pstRequstUser = pstRequstRole->pUser;
    PROCESS_ERROR_RET_CODE(pstRequstUser->eState == USER_STATE_INGAME, errTeamOffLine);

    PROCESS_ERROR_RET_CODE(pstRequstRole->pRoleData->dwTeamID == 0 ||
        pstOnlineRole->pRoleData->dwTeamID == 0, errTeamAlreadyInTeam);

    // If neither party has a team, create one
    if (pstOnlineRole->pRoleData->dwTeamID == 0 && pstRequstRole->pRoleData->dwTeamID == 0)
    {
        UINT32 dwNewTeamID = 0;
        LOG_PROCESS_ERROR(GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID) == GET_GROUP_INDEX_FROM_SESSION_ID(pstRequstRole->dwCliSessionID));

        dwNewTeamID = GenerateTeamID();
        poTeam = CTeamMgr::GetInstance().New(dwNewTeamID);
        LOG_PROCESS_ERROR(poTeam);

        // Additional code: get team mode
        UINT8 byAssignType = pstOnlineRole->m_TeamInvite[pMsg->dwAnswerRequestRoleID];

        nRetCode = poTeam->Init(GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID),
            //dwNewTeamID, pstRequstRole->pRoleData->szRoleName, 11, etaFreeForAll, ITEMCOLOR_WHITE);    // Original code: default to free-for-all loot
            dwNewTeamID, pstRequstRole->pRoleData->szRoleName, 11, byAssignType, ITEMCOLOR_WHITE);       // New code: set team mode
        LOG_PROCESS_ERROR(nRetCode);

        poTeam->AddMember(pstRequstRole->pRoleData->dwRoleID);    // Add the other player to the new team
        poTeam->AddMember(pstOnlineRole->pRoleData->dwRoleID);    // Add this player to the new team
        poTeam->SetLeader(pstRequstRole->pRoleData->dwRoleID);    // The other player is team leader

        nRetCode = DoClientTeamJoin(pstRequstRole->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoClientTeamJoin(pstOnlineRole->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoGSTeamNew(poTeam);
        LOG_PROCESS_ERROR(nRetCode);

        // New code: reaching here means team formation succeeded; clear data
        pstOnlineRole->m_TeamInvite.clear();

        // Write log
        nRetCode = DoDLCreateTeam(CManageServer::GetInstance().GetGameTime(),
            pstRequstRole->pRoleData->byGroup, pstRequstRole->pRoleData->dwRoleID, dwNewTeamID);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoDLJoinTeam(CManageServer::GetInstance().GetGameTime(),
            pstOnlineRole->pRoleData->byGroup, pstOnlineRole->pRoleData->dwRoleID, dwNewTeamID,
            poTeam->GetLeader(), poTeam->GetMemberCount());
        LOG_CHECK_ERROR(nRetCode);
    }
    // The other player has a team; this player does not
    else if (pstRequstRole->pRoleData->dwTeamID > 0)
    {
        poTeam = CTeamMgr::GetInstance().Find(pstRequstRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR(poTeam);
        LOG_PROCESS_ERROR(poTeam->GetGroupID() == GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID));
        PROCESS_ERROR_RET_CODE(poTeam->GetLeader() == pstRequstRole->pRoleData->dwRoleID, errTeamNotLeader);
        PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() < poTeam->GetMemberSizeLimit(), errTeamFull);

        nRetCode = poTeam->AddMember(pstOnlineRole->pRoleData->dwRoleID);    // Add this player
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoClientTeamJoin(pstOnlineRole->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoClientTeamAddMember(pstOnlineRole, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoGSTeamAddMember(pstOnlineRole->pRoleData, poTeam);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoDLJoinTeam(CManageServer::GetInstance().GetGameTime(), pstOnlineRole->pRoleData->byGroup,
            pstOnlineRole->pRoleData->dwRoleID, poTeam->GetTeamID(), poTeam->GetLeader(), poTeam->GetMemberCount());
        LOG_CHECK_ERROR(nRetCode);
    }
    // This player has a team and responds to the other player’s join request
    else
    {
        poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR(poTeam);
        LOG_PROCESS_ERROR(poTeam->GetGroupID() == GET_GROUP_INDEX_FROM_SESSION_ID(pstRequstRole->dwCliSessionID));
        PROCESS_ERROR_RET_CODE(poTeam->GetLeader() == pstOnlineRole->pRoleData->dwRoleID, errTeamNotLeader);

        if (poTeam->GetActivityID() > 0)
        {
        }
        else
        {
            PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() < poTeam->GetMemberSizeLimit(), errTeamFull);
        }

        nRetCode = poTeam->AddMember(pstRequstRole->pRoleData->dwRoleID);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoClientTeamJoin(pstRequstRole->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoClientTeamAddMember(pstRequstRole, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoGSTeamAddMember(pstRequstRole->pRoleData, poTeam);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoDLJoinTeam(CManageServer::GetInstance().GetGameTime(), pstRequstRole->pRoleData->byGroup,
            pstRequstRole->pRoleData->dwRoleID, poTeam->GetTeamID(), poTeam->GetLeader(), poTeam->GetMemberCount());
        LOG_CHECK_ERROR(nRetCode);
    }

Exit1:
    return;
Exit0:
    if (nResult == errTeamFailed)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

// Update team information
void CMessageHandler::OnClientTeamChangeInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errTeamFailed;
    CTeam* poTeam = NULL;
    MS_USER* poUser = NULL;
    C2M_TEAM_CHANGEINFO* pMsg = (C2M_TEAM_CHANGEINFO*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);

    poUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(poUser);
    PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
    LOG_PROCESS_ERROR(poTeam);
    LOG_PROCESS_ERROR(poTeam->GetTeamID() == pstOnlineRole->pRoleData->dwTeamID);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->dwRoleID == poTeam->GetLeader());

    PROCESS_ERROR_RET_CODE(poTeam->GetMemberCount() <= pMsg->nTeamMemberLimit, errTeamNotPermit);
    PROCESS_ERROR_RET_CODE(poTeam->FindMember(pMsg->dwLeadID), errTeamNotInTeam);

    LOG_PROCESS_ERROR(pMsg->szTeamName);
    LOG_PROCESS_ERROR(pMsg->byAssignType < etaTotal);
    LOG_PROCESS_ERROR(pMsg->byItemAuctionColorLimit< ITEMCOLOR_TOTAL && pMsg->byItemAuctionColorLimit > ITEMCOLOR_INVALID);

    poTeam->SetMemberSizeLimit(pMsg->nTeamMemberLimit);    // This line is disabled by default; enable it temporarily for testing
    poTeam->SetLeader(pMsg->dwLeadID);
    poTeam->SetAssignType(pMsg->byAssignType);
    poTeam->SetTeamName(pMsg->szTeamName);
    poTeam->SetAuctionItemColorLimit(pMsg->byItemAuctionColorLimit);

    nRetCode = DoClientTeamChangeInfo(poTeam);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = DoGSTeamChangeInfo(poTeam);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errTeamFailed)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientActivityTeamChangeDungeon(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;

    CTeam* poTeam = NULL;
    C2M_TEAM_CHANGE_ACTIVITY_INFO* pMsg = (C2M_TEAM_CHANGE_ACTIVITY_INFO*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
    LOG_PROCESS_ERROR(poTeam);
    LOG_PROCESS_ERROR(poTeam->GetLeader() == pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(pMsg->dwActivityType >= etaatInvalid && pMsg->dwActivityType < etaatTotal);

    if (poTeam->GetActivityType() != etaatInvalid)
    {
        nRetCode = CTeamMgr::GetInstance().DeleteActivityTeam(poTeam->GetActivityType(), poTeam->GetActivityID(), pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR(nRetCode);
    }

    if (pMsg->dwActivityType != etaatInvalid)
    {
        nRetCode = CTeamMgr::GetInstance().AddActivityTeam(pMsg->dwActivityType, pMsg->nActivityID, pstOnlineRole->pRoleData->dwTeamID);
        LOG_PROCESS_ERROR(nRetCode);

        poTeam->SetActivityType(pMsg->dwActivityType);
        poTeam->SetActivityID(pMsg->nActivityID);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientActivityChangeDungeon(poTeam, pMsg->dwActivityType, pMsg->nActivityID, TRUE);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSTeamChangeInfo(poTeam);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:

    return;
}
void CMessageHandler::OnClientActivityTeamQueryTeam(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    INT32* pTeamHead = NULL;

    C2M_TEAM_QUERY_ACTIVITY_TEAM* pMsg = (C2M_TEAM_QUERY_ACTIVITY_TEAM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pMsg->nPgae >= 0 && pMsg->nPgae < MAX_TEAM_PAGE_COUNT);

    nRetCode = CMessageHandler::GetInstance().DoClientActivityQueryTeam(pstOnlineRole->dwCliSessionID, pMsg->dwActivityType, pMsg->nActivityID, pMsg->nPgae);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errTeamFailed)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}
void CMessageHandler::OnClientActivityTeamRemove(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    CTeam* pTeam = NULL;

    C2M_TEAM_REMOVE_ACTIVITY_TEAM* pMsg = (C2M_TEAM_REMOVE_ACTIVITY_TEAM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
    LOG_PROCESS_ERROR(pTeam);
    LOG_PROCESS_ERROR(pTeam->GetLeader() == pstOnlineRole->pRoleData->dwRoleID);

    nRetCode = CTeamMgr::GetInstance().DeleteActivityTeam(pTeam->GetActivityType(), pTeam->GetActivityID(), pTeam->GetTeamID());
    LOG_PROCESS_ERROR(nRetCode);

    pTeam->SetActivityType(etaatInvalid);
    pTeam->SetActivityID(0);

    return;
Exit0:
    return;
}

// Query team information
void CMessageHandler::OnClientTeamQuery(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    CTeam* poTeam = NULL;
    MS_ONLINE_ROLE_DATA* targetrole = NULL;

    C2M_TEAM_QUERY* pMsg = (C2M_TEAM_QUERY*)pPacketBuffer;

    // Get the other player object pointer
    targetrole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTargetRoleID);
    LOG_PROCESS_ERROR(targetrole);

    // Get the other player’s team pointer
    poTeam = CTeamMgr::GetInstance().Find(targetrole->pRoleData->dwTeamID);

    // The other player has no team
    if (poTeam == NULL)
    {
        DoClientTeamSyncTeamInfo(0, "", ITEMCOLOR_WHITE, 0, pstOnlineRole);
    }
    // The other player has a team
    else
    {
        DoClientTeamSyncTeamInfo(poTeam->GetTeamID(), poTeam->GetTeamName(), poTeam->GetAuctionItemColorLimit(), poTeam->GetLeader(), pstOnlineRole);
    }

Exit0:
    return;
}

// Invite to team (used when neither party has a team)
void CMessageHandler::OnClientTeamInvite(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errTeamFailed;
    UINT8 eAnswerType = etatInvite;
    MS_USER* pstReqUser = NULL;
    MS_USER* pstTargetUser = NULL;
    MS_ONLINE_ROLE_DATA* pstTgtOnlineRole = NULL;
    CTeam* poTeam = NULL;

    C2M_TEAM_INVITE* pMsg = (C2M_TEAM_INVITE*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);

    pstReqUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(pstReqUser);
    PROCESS_ERROR_RET_CODE(pstReqUser->eState == USER_STATE_INGAME, errTeamNotPermit);

    LOG_PROCESS_ERROR_RET_CODE(!pstReqUser->bFreeMode, errFreeMode);

    // Get current player information
    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pstReqUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    // Get target player information
    pstTgtOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTargetRoleID);
    PROCESS_ERROR_RET_CODE(pstTgtOnlineRole, errTeamOffLine);
    LOG_PROCESS_ERROR(pstTgtOnlineRole->pRoleData);
    PROCESS_ERROR_RET_CODE(pstTgtOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errTeamInBattleQueue);

    // Team with self
    if (pstOnlineRole->pRoleData->dwRoleID == pMsg->dwTargetRoleID)
    {
        UINT32 dwNewTeamID = 0;
        LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->dwTeamID == 0, errTeamAlreadyInTeam);

        dwNewTeamID = GenerateTeamID();
        poTeam = CTeamMgr::GetInstance().New(dwNewTeamID);
        LOG_PROCESS_ERROR(poTeam);

        nRetCode = poTeam->Init(GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID),
            dwNewTeamID, pstOnlineRole->pRoleData->szRoleName, 11, pMsg->byAssignType, ITEMCOLOR_WHITE);
        LOG_PROCESS_ERROR(nRetCode);

        poTeam->AddMember(pMsg->dwTargetRoleID);
        poTeam->SetLeader(pMsg->dwTargetRoleID);

        nRetCode = DoClientTeamJoin(pstOnlineRole->dwCliSessionID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = DoGSTeamNew(poTeam);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = DoDLCreateTeam(CManageServer::GetInstance().GetGameTime(), pstOnlineRole->pRoleData->byGroup,
            pstOnlineRole->pRoleData->dwRoleID, dwNewTeamID);
        LOG_CHECK_ERROR(nRetCode);
    }
    // Invite another player when neither is in a team
    else
    {
        UINT32 dwTeamID = INVALID_OBJECT_ID;

        pstTargetUser = pstTgtOnlineRole->pUser;
        PROCESS_ERROR_RET_CODE(pstTargetUser->eState == USER_STATE_INGAME, errTeamOffLine);
        PROCESS_ERROR_RET_CODE(pstTgtOnlineRole->pRoleData->dwTeamID == 0 ||
            pstOnlineRole->pRoleData->dwTeamID == 0, errTeamAlreadyInTeam);

        LOG_PROCESS_ERROR(GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID) == GET_GROUP_INDEX_FROM_SESSION_ID(pstTgtOnlineRole->dwCliSessionID));

        for (INT32 nIndex = 1; nIndex < MAX_JOIN_TEAM_ROLE_COUNT; nIndex++)
        {
            pstTgtOnlineRole->dwJoinTeamRoleID[nIndex - 1] = pstTgtOnlineRole->dwJoinTeamRoleID[nIndex];
        }
        
        pstTgtOnlineRole->dwJoinTeamRoleID[MAX_JOIN_TEAM_ROLE_COUNT - 1] = pstOnlineRole->pRoleData->dwRoleID;

        // Additional code: record team mode when inviting another player
        pstTgtOnlineRole->m_TeamInvite[pstOnlineRole->pRoleData->dwRoleID] = pMsg->byAssignType;

        nRetCode = DoClientTeamAnswer(pstTgtOnlineRole->dwCliSessionID, eAnswerType, pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName);
        LOG_PROCESS_ERROR(nRetCode);

        if (eAnswerType == etatInvite)
            DoErrorNotify(pstOnlineRole->dwCliSessionID, errTeamInvitationSent, 0, 0);    // 3841
        else
            DoErrorNotify(pstOnlineRole->dwCliSessionID, errTeamApplicationSent, 0, 0);
    }

    return;
Exit0:
    if (nResult == errTeamFailed)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}
// ===== Team end

// Friends
void CMessageHandler::OnClientFriendAdd(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nResult = errSuccess;
    INT32   nRetCode = 0;
    C2M_FRIEND_ADD* pMsg = (C2M_FRIEND_ADD*)pPacketBuffer;
    MS_ROLE_DATA* friend_role = NULL;
    MS_ONLINE_ROLE_DATA* online_friend_role = NULL;
    MS_USER* poUser = NULL;
    BOOL bRet = 0;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(IsValidGroup(pstOnlineRole->pRoleData->byGroup));

    poUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(poUser);
    LOG_PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);

    if (pMsg->dwRoleID == 0 && strlen(pMsg->szRoleName) == 0)
    {
        //cheater
        LOG_PROCESS_ERROR(FALSE);
    }

    if (pMsg->dwRoleID == pstOnlineRole->pRoleData->dwRoleID || strcmp(pMsg->szRoleName, pstOnlineRole->pRoleData->szRoleName) == 0)
    {
        //cheater
        LOG_PROCESS_ERROR(FALSE);
    }

    if (pMsg->dwRoleID)
    {
        online_friend_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
        if (online_friend_role)
            friend_role = online_friend_role->pRoleData;
        else
            friend_role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    }
    else
    {
        online_friend_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByName(pMsg->szRoleName);
        if (online_friend_role)
            friend_role = online_friend_role->pRoleData;
        else
            friend_role = CRoleDataMgr::GetInstance().FindRoleByName(pMsg->szRoleName);
    }

    if (friend_role == NULL)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotFound, 0, 0);
        return;
    }

    if (pstOnlineRole->pRoleData->byGroup != friend_role->byGroup)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotFound, 0, 0);
        return;
    }

    if (pstOnlineRole->m_friends.find(friend_role->dwRoleID) != pstOnlineRole->m_friends.end())
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendAlreadyExists, 0, 0);
        return;
    }

    if (pstOnlineRole->m_friends.size() >= MAX_FRIEND_COUNT)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendFull, 0, 0);
        return;
    }

    // is personal enemy, if yes, delete relation
    for (MS_ONLINE_ROLE_DATA::PERSONAL_ENEMY_LIST::iterator iter = pstOnlineRole->m_personalEnemy.begin(); iter != pstOnlineRole->m_personalEnemy.end(); ++iter)
    {
        if (*iter == friend_role->dwRoleID)
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteFriend, NULL, NULL, "udud", poUser->dwCurRoleID, friend_role->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);
            pstOnlineRole->m_personalEnemy.erase(iter);
            nRetCode = DoGSDelFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, friend_role->dwRoleID, FRIEND_GROUP_ENEMY);
            LOG_CHECK_ERROR(nRetCode);
            nRetCode = DoClientPersonalEnemyDel(pstOnlineRole->dwCliSessionID, friend_role->dwRoleID);
            LOG_CHECK_ERROR(nRetCode);
            break;
        }
    }

    pstOnlineRole->m_friends[friend_role->dwRoleID] = 0;

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertFriend, NULL, NULL, "ucududuc", pstOnlineRole->pRoleData->byGroup, poUser->dwCurRoleID, friend_role->dwRoleID, 0);
    LOG_PROCESS_ERROR(nRetCode);

    // default group : 0  Allan Tao
    DoGSAddFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, friend_role->dwRoleID, 0);

    DoClientFriendAdd(pstOnlineRole->dwCliSessionID, friend_role->dwRoleID, friend_role->szRoleName, (pstOnlineRole->m_fans.find(friend_role->dwRoleID) == pstOnlineRole->m_fans.end() ? 0 : 1));
    if (online_friend_role)
    {
        online_friend_role->m_fans.insert(poUser->dwCurRoleID);
        DoClientFriendInfoChange(pstOnlineRole->dwCliSessionID, online_friend_role);
        DoClientFriendAddedBy(online_friend_role->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName);
    }
    else
    {
        DoClientFriendInfoChange(pstOnlineRole->dwCliSessionID, friend_role);
    }

Exit0:
    return;
}

void CMessageHandler::OnClientFriendDel(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nResult = errSuccess;
    C2M_FRIEND_DEL* pMsg = (C2M_FRIEND_DEL*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA* friend_role = NULL;
    MS_ONLINE_ROLE_DATA::FriendMap::iterator it;
    MS_USER* poUser = NULL;
    UINT8       byGroupID = 0;
    BOOL bRet = 0;

    LOG_PROCESS_ERROR(pstOnlineRole);
    poUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(poUser);
    LOG_PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);

    it = pstOnlineRole->m_friends.find(pMsg->dwRoleID);
    if (it == pstOnlineRole->m_friends.end())
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotFound, 0, 0);
        return;
    }
    if (it->second >= MAX_FRIEND_GROUP_COUNT)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotFound, 0, 0);
        return;
    }

    byGroupID = it->second;
    pstOnlineRole->m_friends.erase(it);

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteFriend, NULL, NULL, "udud", poUser->dwCurRoleID, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(bRet);

    DoGSDelFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwRoleID, byGroupID);

    DoClientFriendDel(pstOnlineRole->dwCliSessionID, pMsg->dwRoleID);

    friend_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    if (friend_role)
    {
        friend_role->m_fans.erase(pstOnlineRole->pRoleData->dwRoleID);
        DoClientFriendDeletedBy(friend_role->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName);
    }

Exit0:
    return;
}

void CMessageHandler::OnClientFriendChangeGroup(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nResult = errSuccess;
    C2M_FRIEND_CHANGE_GROUP* pMsg = (C2M_FRIEND_CHANGE_GROUP*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA::FriendMap::iterator it;
    MS_USER* poUser = NULL;
    BOOL bRet = 0;

    LOG_PROCESS_ERROR(pstOnlineRole);

    poUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(poUser);
    LOG_PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);
    LOG_PROCESS_ERROR(pMsg->byGroupID < MAX_FRIEND_GROUP_COUNT);

    it = pstOnlineRole->m_friends.find(pMsg->dwRoleID);
    if (it == pstOnlineRole->m_friends.end())
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotFound, 0, 0);
        return;
    }

    it->second = pMsg->byGroupID;

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateFriend, NULL, NULL, "ucudud", pMsg->byGroupID, poUser->dwCurRoleID, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return;
}

void CMessageHandler::OnClientChangeInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    C2M_CHANGE_INFO* pMsg = (C2M_CHANGE_INFO*)pPacketBuffer;
    bool changed = false;

    MS_USER* poUser = NULL;
    LOG_PROCESS_ERROR(pstOnlineRole);

    poUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(poUser);
    LOG_PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);
    LOG_PROCESS_ERROR(strlen(pMsg->szSignature) < SIGNATURE_LEN);
    LOG_PROCESS_ERROR(pMsg->byFriendState == FS_ONLINE ||
        pMsg->byFriendState == FS_AFK ||
        pMsg->byFriendState == FS_BUSY);

    if (strcmp(pstOnlineRole->m_szSignature, pMsg->szSignature))
    {
        strxcpy(pstOnlineRole->m_szSignature, pMsg->szSignature, SIGNATURE_LEN);
        DoGSChangeInfo(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->szSignature);
        changed = true;
    }
    if (pstOnlineRole->m_byFriendState != pMsg->byFriendState)
    {
        pstOnlineRole->m_byFriendState = pMsg->byFriendState;
        changed = true;
    }

    if (changed)
        pstOnlineRole->BroadCastInfoChange();

Exit0:
    return;
}

void CMessageHandler::OnClientFriendChat(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_FRIEND_CHAT* pMsg = (C2M_FRIEND_CHAT*)pPacketBuffer;
    INT32 message_len = (INT32)strnlen(pMsg->szMessage, MAX_CHAT_MESSAGE_LEN) + 1;
    M2C_FRIEND_CHAT* pReplyMsg = NULL;
    MS_USER* poUser = NULL;
    MS_ONLINE_ROLE_DATA* targetrole = NULL;
    const SCENE_DATA* pcSceneData = NULL;

    LOG_PROCESS_ERROR(pstOnlineRole);
    poUser = pstOnlineRole->pUser;
    LOG_PROCESS_ERROR(poUser);
    LOG_PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);

    pcSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pstOnlineRole->pRoleData->nSceneTempID);
    LOG_PROCESS_ERROR(pcSceneData);
    if (pcSceneData->bCannotFriendChat)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errChatSceneForbidden, 0, 0);
        return;
    }

    if (pstOnlineRole->m_friends.find(pMsg->dwTargetID) == pstOnlineRole->m_friends.end() ||
        pstOnlineRole->m_fans.find(pMsg->dwTargetID) == pstOnlineRole->m_fans.end())
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotMutual, 0, 0);
        return;
    }

    targetrole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTargetID);
    if (targetrole == NULL)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errFriendNotOnline, 0, 0);
        return;
    }

    pReplyMsg = (M2C_FRIEND_CHAT*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_CHAT) + message_len);
    pReplyMsg->wMsg = m2c_friend_chat;
    pReplyMsg->wSize = (UINT16)sizeof(M2C_FRIEND_CHAT) + message_len;
    pReplyMsg->dwFrom = pstOnlineRole->pRoleData->dwRoleID;
    pReplyMsg->byColor = pMsg->byColor;
    strxcpy(pReplyMsg->szMessage, pMsg->szMessage, message_len);

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pReplyMsg->szMessage);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = _SendClient(targetrole->dwCliSessionID, (PACKET_BUFFER*)pReplyMsg, pReplyMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return;

Exit0:
    if (pReplyMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pReplyMsg);
}

void CMessageHandler::OnClientPersonalEnemyAdd(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    C2M_PERSONAL_ENEMY_ADD* pMsg = (C2M_PERSONAL_ENEMY_ADD*)pPacketBuffer;
    MS_ROLE_DATA* pEnemyRole = NULL;
    MS_ONLINE_ROLE_DATA::PERSONAL_ENEMY_LIST::iterator end_iter;
    MS_ONLINE_ROLE_DATA::PERSONAL_ENEMY_LIST::iterator iter;
    UINT32      dwFirstRoleID = 0;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(IsValidGroup(pstOnlineRole->pRoleData->byGroup));

    // TODO is friend: do not count a followed player as an enemy. Allan Tao
    PROCESS_ERROR_RET_CODE(pstOnlineRole->m_friends.find(pMsg->dwRoleID) == pstOnlineRole->m_friends.end(), errFriendAlreadyExists);

    // check same
    end_iter = pstOnlineRole->m_personalEnemy.end();
    for (iter = pstOnlineRole->m_personalEnemy.begin(); iter != end_iter; ++iter)
        if (*iter == pMsg->dwRoleID)
            return;

    pEnemyRole = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    PROCESS_ERROR_RET_CODE(pEnemyRole && pEnemyRole->byGroup == pstOnlineRole->pRoleData->byGroup, errEnemyNotFind);


    if (pstOnlineRole->m_personalEnemy.size() >= MAX_PERSONAL_ENEMY_COUNT)
    {
        iter = pstOnlineRole->m_personalEnemy.begin();
        nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteFriend, NULL, NULL, "udud", pstOnlineRole->pRoleData->dwRoleID, *iter);
        LOG_PROCESS_ERROR(nRetCode);
        dwFirstRoleID = *iter;
        pstOnlineRole->m_personalEnemy.erase(iter);
        iter = pstOnlineRole->m_personalEnemy.end();

        nRetCode = DoGSDelFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, dwFirstRoleID, FRIEND_GROUP_ENEMY);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = DoClientPersonalEnemyDel(pstOnlineRole->dwCliSessionID, dwFirstRoleID);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlInsertFriend, NULL, NULL, "ucududuc", pstOnlineRole->pRoleData->byGroup, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwRoleID, FRIEND_GROUP_ENEMY);
    LOG_PROCESS_ERROR(nRetCode);
    pstOnlineRole->m_personalEnemy.push_back(pMsg->dwRoleID);
    nRetCode = DoGSAddFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwRoleID, FRIEND_GROUP_ENEMY);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = DoClientPersonalEnemyAdd(pstOnlineRole->dwCliSessionID, pMsg->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientPersonalEnemyDel(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    C2M_PERSONAL_ENEMY_DEL* pMsg = (C2M_PERSONAL_ENEMY_DEL*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA::PERSONAL_ENEMY_LIST::iterator iter;
    MS_ONLINE_ROLE_DATA::PERSONAL_ENEMY_LIST::iterator end_iter;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    end_iter = pstOnlineRole->m_personalEnemy.end();
    for (iter = pstOnlineRole->m_personalEnemy.begin(); iter != end_iter; ++iter)
    {
        if (*iter == pMsg->dwRoleID)
            break;
    }
    LOG_PROCESS_ERROR_RET_CODE(iter != pstOnlineRole->m_personalEnemy.end(), errEnemyNotFind);

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteFriend, NULL, NULL, "udud", pstOnlineRole->pRoleData->dwRoleID, *iter);
    LOG_PROCESS_ERROR(nRetCode);

    pstOnlineRole->m_personalEnemy.erase(iter);
    iter = pstOnlineRole->m_personalEnemy.end();

    nRetCode = DoGSDelFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwRoleID, FRIEND_GROUP_ENEMY);
    LOG_CHECK_ERROR(nRetCode);
    nRetCode = DoClientPersonalEnemyDel(pstOnlineRole->dwCliSessionID, pMsg->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

//BOOL CMessageHandler::DoRoleLogout(UINT32 dwRoleID)
//{
//    CTeam* pstTeam = NULL;
//    INT32 nRetCode;
//    MS_ONLINE_ROLE_DATA* pstRole = NULL;
//
//    pstRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
//    LOG_PROCESS_ERROR(pstRole);
//    LOG_PROCESS_ERROR(pstRole->stRoleData);
//
//    pstTeam = g_TeamMgr.Find(pstRole->stRoleData->dwTeamID);
//    if(pstTeam && pstTeam->GetLeader() == pstRole->stRoleData->dwRoleID)
//    {
//        nRetCode = pstTeam->SelectLeader(FALSE);
//        LOG_PROCESS_ERROR(nRetCode);
//
//        if(pstTeam->GetLeader() != pstRole->stRoleData->dwRoleID)
//        {
//            nRetCode = DoClientTeamChangeInfo(pstTeam);
//            LOG_CHECK_ERROR(nRetCode);
//
//            nRetCode = DoGSTeamChangeInfo(pstTeam);
//            LOG_PROCESS_ERROR(nRetCode);
//        }
//    }
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}

void CMessageHandler::OnClientChatMessage(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32	 nRetCode = 0;
    INT32	 nMessageSize = 0;

    INT32	 nTargetType = 0;
    INT32	 nResult = errUnknownError;
    UINT32	 dwCurrentTime = 0;
    C2M_CHAT_MESSAGE* pMsg = (C2M_CHAT_MESSAGE*)pPacketBuffer;
    const SCENE_DATA* pcSceneData = NULL;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR_RET_CODE(!pstOnlineRole->pUser->bFreeMode, errFreeMode);

    nMessageSize = pMsg->wSize - sizeof(C2M_CHAT_MESSAGE);
    LOG_PROCESS_ERROR((nMessageSize > 0) && (nMessageSize <= MAX_CHAT_MESSAGE_LEN));
    LOG_PROCESS_ERROR(pMsg->szMessage[nMessageSize - 1] == '\0');

    dwCurrentTime = CManageServer::GetInstance().GetGameTime();

    LOG_PROCESS_ERROR_RET_CODE(
        pstOnlineRole->dwLastSpeakTime + g_GWConfig.ChatConfig.nNormalMsgInterval / 1000 < dwCurrentTime, errChatTooFrequent);
    pstOnlineRole->dwLastSpeakTime = dwCurrentTime;

    pcSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pstOnlineRole->pRoleData->nSceneTempID);
    LOG_PROCESS_ERROR(pcSceneData);

    nTargetType = GetTypeFromID(pMsg->dwTargetID);
    switch (nTargetType)
    {
        case otRole:
        {
            INT32 nChatMsgSize = 0;
            MS_ONLINE_ROLE_DATA* poTargetRole = NULL;

            PROCESS_ERROR_RET_CODE(!pcSceneData->bCannotWhisperChat, errChatSceneForbidden);

            nChatMsgSize = (INT32)strnlen(pMsg->szMessage, MAX_CHAT_MESSAGE_LEN) + 1;
            LOG_PROCESS_ERROR(pMsg->szMessage[nChatMsgSize - 1] == '\0');

            if (GetIndexFromID(pMsg->dwTargetID) == 0)
            {
                INT32 nNameSize = (INT32)strnlen(pMsg->szMessage + nChatMsgSize, ROLE_NAME_LEN) + 1;
                LOG_PROCESS_ERROR((nNameSize + nChatMsgSize) == nMessageSize);

                poTargetRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByNameInGroup(pMsg->szMessage + nChatMsgSize,
                    GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID));
                PROCESS_ERROR_RET_CODE(poTargetRole, errChatTargetNotFound);
                LOG_PROCESS_ERROR_RET_CODE(poTargetRole->pRoleData, errChatTargetNotFound);
            }
            else
            {
                LOG_PROCESS_ERROR(nChatMsgSize == nMessageSize);

                poTargetRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(pMsg->dwTargetID,
                    GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID));

                PROCESS_ERROR_RET_CODE(poTargetRole, errChatTargetNotFound);
                LOG_PROCESS_ERROR_RET_CODE(poTargetRole->pRoleData, errChatTargetNotFound);
            }

            if (pstOnlineRole->pRoleData->dwForbidChatTime < dwCurrentTime)
            {
                nRetCode = DoClientChatMessage(pstOnlineRole->pRoleData->szRoleName, pstOnlineRole->pRoleData->dwRoleID,
                    poTargetRole->pRoleData->dwRoleID, cltNormal, pMsg->szMessage, 0, pstOnlineRole->nGMLevel > 0);
                LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
            }

            nRetCode = DoClientChatMessage(pstOnlineRole->pRoleData->szRoleName, poTargetRole->pRoleData->dwRoleID,
                pstOnlineRole->pRoleData->dwRoleID, cltNormal, pMsg->szMessage, 0, pstOnlineRole->nGMLevel > 0); //This message is also sent back to the sender
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);

            nRetCode = DoDLChatMessage(CManageServer::GetInstance().GetGameTime(),
                GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwGSSessionID), pstOnlineRole->pRoleData->dwRoleID,
                poTargetRole->pRoleData->dwRoleID, pMsg->szMessage, nMessageSize);
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
        case otTeam:
        {
            PROCESS_ERROR_RET_CODE(!pcSceneData->bCannotTeamChat, errChatSceneForbidden);
            PROCESS_ERROR(pMsg->dwTargetID == pstOnlineRole->pRoleData->dwTeamID);
            if (pstOnlineRole->pRoleData->dwForbidChatTime >= dwCurrentTime)
            {
                nRetCode = DoClientChatMessageToTarget(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->szRoleName,
                    pstOnlineRole->pRoleData->dwRoleID, pMsg->dwTargetID, cltNormal, pMsg->szMessage, 0);
                LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
            }
            else
            {
                nRetCode = DoClientChatMessage(pstOnlineRole->pRoleData->szRoleName, pstOnlineRole->pRoleData->dwRoleID,
                    pMsg->dwTargetID, cltNormal, pMsg->szMessage, 0, pstOnlineRole->nGMLevel > 0);
                LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
            }

            nRetCode = DoDLChatMessage(CManageServer::GetInstance().GetGameTime(),
                GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwGSSessionID), pstOnlineRole->pRoleData->dwRoleID,
                pMsg->dwTargetID, pMsg->szMessage, nMessageSize);
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
        case otGuild:
        {
            PROCESS_ERROR_RET_CODE(!pcSceneData->bCannotGuildChat, errChatSceneForbidden);
            PROCESS_ERROR(pMsg->dwTargetID == pstOnlineRole->pRoleData->dwGuildID);
            if (pstOnlineRole->pRoleData->dwForbidChatTime >= dwCurrentTime)
            {
                nRetCode = DoClientChatMessageToTarget(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->szRoleName,
                    pstOnlineRole->pRoleData->dwRoleID, pMsg->dwTargetID, cltNormal, pMsg->szMessage, 0);
                LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
            }
            else
            {
                nRetCode = DoClientChatMessage(pstOnlineRole->pRoleData->szRoleName, pstOnlineRole->pRoleData->dwRoleID,
                    pMsg->dwTargetID, cltNormal, pMsg->szMessage, 0, pstOnlineRole->nGMLevel > 0);
                LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatForbidden);
            }

            nRetCode = DoDLChatMessage(CManageServer::GetInstance().GetGameTime(),
                GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwGSSessionID), pstOnlineRole->pRoleData->dwRoleID,
                pMsg->dwTargetID, pMsg->szMessage, nMessageSize);
            LOG_CHECK_ERROR(nRetCode);
        }
        break;
        default:
            PROCESS_ERROR(FALSE);
            break;
    }

    return;

Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnClientQueryLineReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_QUERY_LINE_REQ* pMsg = (C2M_QUERY_LINE_REQ*)pPacketBuffer;
    const SCENE_DATA* poSceneData = NULL;
    INT32 nServerGroup = 0;
    UINT16 wLineInfo = 0;

    UINT16 wLineIDCount = 0;
    UINT32 dwlstLineID[32] = { 0 };

    poSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pstOnlineRole->pRoleData->nSceneTempID);
    LOG_PROCESS_ERROR(poSceneData);
    LOG_PROCESS_ERROR(poSceneData->nType == estLine);

    nServerGroup = GET_GROUP_INDEX_FROM_SESSION_ID(pstOnlineRole->dwCliSessionID);

    wLineIDCount = CSceneMgr::GetInstance().GetLineInfo(nServerGroup, pstOnlineRole->pRoleData->nSceneTempID, 32, dwlstLineID);
    LOG_PROCESS_ERROR(wLineIDCount > 0);

    nRetCode = DoClientQueryLineAck(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->nSceneTempID, wLineIDCount, dwlstLineID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    return;
}

BOOL CMessageHandler::DoErrorNotify(UINT32 dwClientSessionID, INT32 nErrorCode, INT32 nParam1, INT32 nParam2)
{
    INT32 nRetCode = 0;

    M2C_ERROR_NOTIFY* pMsg = (M2C_ERROR_NOTIFY*)m_pPacketAllocator->Alloc(sizeof(M2C_ERROR_NOTIFY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_error_notify;
    pMsg->nErrorCode = nErrorCode;
    pMsg->nParam1 = nParam1;
    pMsg->nParam2 = nParam2;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_ERROR_NOTIFY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientQueueInfo(UINT32 dwClientSessionID, INT32 nTotal, INT32 nSelfPos)
{
    INT32 nRetCode = 0;

    M2C_QUEUE_INFO* pMsg = (M2C_QUEUE_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_QUEUE_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_queue_info;
    pMsg->nTotal = nTotal;
    pMsg->nSelfPos = nSelfPos;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_QUEUE_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientSyncRoleInfo(UINT32 dwClientSessionID, MS_ROLE_DATA* pRoleData, const char* pGuildName, UINT32 dwCRC)
{
    INT32 nRetCode = 0;

    M2C_SYNC_ROLE_INFO* pMsg = (M2C_SYNC_ROLE_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_SYNC_ROLE_INFO));
    LOG_PROCESS_ERROR(pMsg);

    memset(pMsg, 0, sizeof(M2C_SYNC_ROLE_INFO));

    pMsg->wMsg = m2c_sync_role_info;
    pMsg->dwRoleID = pRoleData->dwRoleID;
    strxcpy(pMsg->szRoleName, pRoleData->szRoleName, ROLE_NAME_LEN);
    pMsg->byClass = pRoleData->byClass;
    pMsg->bySex = pRoleData->bySex;
    pMsg->byVIPLevel = pRoleData->byVIPLevel;

    if (pGuildName)
    {
        strxcpy(pMsg->szGuildName, pGuildName, GUILD_NAME_LEN);
    }

    if (crc32(0, (const Bytef*)pMsg, sizeof(M2C_SYNC_ROLE_INFO)) != dwCRC)
    {
        nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_SYNC_ROLE_INFO));
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

// ===== Team begin
BOOL CMessageHandler::DoClientTeamJoin(UINT32 dwClientSessionID, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    INT32 nSize;
    M2C_TEAM_JOIN* pMsg = NULL;
    LOG_PROCESS_ERROR(poTeam);

    nSize = sizeof(M2C_TEAM_JOIN) + sizeof(TEAM_MEMBER_BASEINFO) * poTeam->GetMemberCount();
    pMsg = (M2C_TEAM_JOIN*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_join;
    pMsg->wSize = nSize;
    strxcpy(pMsg->szTeamName, poTeam->GetTeamName(), TEAM_NAME_LEN);
    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szTeamName);
        LOG_CHECK_ERROR(nRetCode);
    }
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwLeadID = poTeam->GetLeader();
    pMsg->byAssignType = poTeam->GetAssignType();
    pMsg->nMemberSizeLimit = poTeam->GetMemberSizeLimit();
    pMsg->byActivityType = poTeam->GetActivityType();
    pMsg->dwActivityID = poTeam->GetActivityID();

    pMsg->byMemberCount = 0;

    for (INT32 nIndex = 0; nIndex < poTeam->GetMemberSizeLimit(); nIndex++)
    {
        MS_ROLE_DATA* poRoleData = poTeam->GetMember(nIndex);
        if (poRoleData)
        {
            pMsg->stTeamMemberList[pMsg->byMemberCount].dwRoleID = poRoleData->dwRoleID;
            pMsg->stTeamMemberList[pMsg->byMemberCount].bySex = poRoleData->bySex;
            pMsg->stTeamMemberList[pMsg->byMemberCount].byClass = poRoleData->byClass;
            pMsg->stTeamMemberList[pMsg->byMemberCount].byState = etsOffline;
            strxcpy(pMsg->stTeamMemberList[pMsg->byMemberCount].szRoleName, poRoleData->szRoleName, ROLE_NAME_LEN);

            MS_ONLINE_ROLE_DATA* pstOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(poRoleData->dwRoleID, poTeam->GetGroupID());
            if (pstOnlineRoleData)
                pMsg->stTeamMemberList[pMsg->byMemberCount].byState = etsOnline;

            pMsg->byMemberCount++;
        }

    }
    LOG_PROCESS_ERROR(pMsg->byMemberCount == poTeam->GetMemberCount());

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamAnswer(UINT32 dwClientSessionID, UINT8 byType, UINT32 dwRequestRoleID, const CHAR* pcszRoleName)
{
    INT32 nRetCode = 0;

    M2C_TEAM_ANSWER* pMsg = (M2C_TEAM_ANSWER*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_ANSWER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_answer;
    pMsg->dwAnswerRequestRoleID = dwRequestRoleID;
    pMsg->byAnswerType = byType;
    strxcpy(pMsg->szRequestRoleName, pcszRoleName, ROLE_NAME_LEN);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_ANSWER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamLeave(UINT32 dwClientSessionID, UINT32 dwTeamID)
{
    INT32 nRetCode = 0;

    M2C_TEAM_LEAVE* pMsg = (M2C_TEAM_LEAVE*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_LEAVE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_leave;
    pMsg->dwTeamID = dwTeamID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_LEAVE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamAddMember(MS_ONLINE_ROLE_DATA* poNewMember, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2C_TEAM_ADD_MEMBER* pMsg = NULL;

    LOG_PROCESS_ERROR(poNewMember);
    LOG_PROCESS_ERROR(poNewMember->pRoleData);
    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2C_TEAM_ADD_MEMBER*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_ADD_MEMBER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_add_member;
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->stAddMember.dwRoleID = poNewMember->pRoleData->dwRoleID;
    pMsg->stAddMember.bySex = poNewMember->pRoleData->bySex;
    pMsg->stAddMember.byClass = poNewMember->pRoleData->byClass;
    pMsg->stAddMember.byState = etsOnline;
    strxcpy(pMsg->stAddMember.szRoleName, poNewMember->pRoleData->szRoleName, ROLE_NAME_LEN);

    nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_ADD_MEMBER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamChangeInfo(CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2C_TEAM_CHANGEINFO* pMsg = NULL;
    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2C_TEAM_CHANGEINFO*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_CHANGEINFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_changeinfo;
    pMsg->dwRoomID = poTeam->GetActivityID();
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwLeadID = poTeam->GetLeader();
    pMsg->byAssignType = poTeam->GetAssignType();
    pMsg->byAuctionItemColorLimit = poTeam->GetAuctionItemColorLimit();
    pMsg->nTeamMemberLimit = poTeam->GetMemberSizeLimit();
    strxcpy(pMsg->szTeamName, poTeam->GetTeamName(), TEAM_NAME_LEN);
    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szTeamName);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_CHANGEINFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientRemoveMember(UINT32 dwDelMemberID, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2C_TEAM_REMOVE_MEMBER* pMsg = NULL;
    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2C_TEAM_REMOVE_MEMBER*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_REMOVE_MEMBER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_remove_member;
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwRemoveMemberID = dwDelMemberID;

    nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_REMOVE_MEMBER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamSyncMemberInfo(MS_ONLINE_ROLE_DATA* pstRoleData, INT32 nX, INT32 nY, INT32 nZ, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    M2C_TEAM_SYNC_MEMBERINFO* pMsg = NULL;
    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2C_TEAM_SYNC_MEMBERINFO*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_SYNC_MEMBERINFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_sync_memberinfo;
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->stMember.dwRoleID = pstRoleData->pRoleData->dwRoleID;
    pMsg->stMember.nLevel = pstRoleData->pRoleData->byLevel;
    pMsg->stMember.nMaxHp = pstRoleData->nMaxHp;
    pMsg->stMember.nCurHp = pstRoleData->nCurrentHp;
    pMsg->stMember.nMaxMp = pstRoleData->nMaxMp;
    pMsg->stMember.nCurMp = pstRoleData->nCurrentMp;
    pMsg->stMember.dwSceneTempID = pstRoleData->pRoleData->nSceneTempID;
    pMsg->stMember.nSceneID = pstRoleData->pRoleData->nSceneID;
    pMsg->stMember.nX = nX;
    pMsg->stMember.nY = nY;
    pMsg->stMember.nZ = nZ;

    nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_SYNC_MEMBERINFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamSyncMemberState(UINT32 dwRoleID, UINT8 byState, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    INT32 nSize = 0;
    M2C_TEAM_SYNC_MEMBERSTATE* pMsg = NULL;
    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2C_TEAM_SYNC_MEMBERSTATE*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_SYNC_MEMBERSTATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_sync_memberstate;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byState = byState;

    nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_SYNC_MEMBERSTATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientActivityChangeDungeon(CTeam* pTeam, UINT32 dwActivityType, INT32 nActivityID, BOOL bResult)
{
    INT32 nRetCode = 0;
    BOOL bRet = FALSE;

    M2C_ACTIVITY_CHANGE_DUNGEON* pMsg = NULL;

    pMsg = (M2C_ACTIVITY_CHANGE_DUNGEON*)m_pPacketAllocator->Alloc(sizeof(M2C_ACTIVITY_CHANGE_DUNGEON));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pTeam);

    pMsg->wMsg = m2c_activity_change_dungeon;
    pMsg->bResult = bResult;
    pMsg->dwActivityType = dwActivityType;
    pMsg->nActivityID = nActivityID;

    bRet = _BroadcastTeam(pTeam, (PACKET_BUFFER*)pMsg, sizeof(M2C_ACTIVITY_CHANGE_DUNGEON));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientActivityQueryTeam(UINT32 dwClientSessionID, BOOL bActivity, INT32 nActivityID, INT32 nPage)
{
    INT32 nRetCode = 0;
    INT32 nCount = 0;
    BOOL bRet = FALSE;
    INT32* pTeamHead = NULL;
    M2C_ACTIVITY_QUERY_TEAM* pMsg = NULL;

    pMsg = (M2C_ACTIVITY_QUERY_TEAM*)m_pPacketAllocator->Alloc(sizeof(M2C_ACTIVITY_QUERY_TEAM));
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(nPage >= 0 && nPage < MAX_TEAM_PAGE_COUNT);

    pMsg->wMsg = m2c_activity_query_team;
    pMsg->dwActivityType = bActivity;
    pMsg->nActivityID = nActivityID;
    pMsg->nPage = nPage;

    pTeamHead = CTeamMgr::GetInstance().GetActivityTeamHead(pMsg->dwActivityType, pMsg->nActivityID);
    LOG_PROCESS_ERROR(pTeamHead);

    for (INT32 nIndex = nPage * MAX_TEAM_COUNT_PER_PAGE; nIndex < (nPage + 1) * MAX_TEAM_COUNT_PER_PAGE; nIndex++)
    {
        if (*(pTeamHead + nIndex))
        {
            INT32 nTeamLevel = 0;
            CTeam* pTeam = CTeamMgr::GetInstance().Find(*(pTeamHead + nIndex));
            LOG_PROCESS_ERROR(pTeam);

            strxcpy(pMsg->stRoomTeam[nCount].szTeamName, pTeam->GetTeamName(), TEAM_NAME_LEN);
            if (g_GWConfig.bFilterCensoredWord)
            {
                nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->stRoomTeam[nCount].szTeamName);
                LOG_CHECK_ERROR(nRetCode);
            }
            pMsg->stRoomTeam[nCount].nTeamID = pTeam->GetTeamID();
            pMsg->stRoomTeam[nCount].dwLeaderID = pTeam->GetLeader();
            memcpy(pMsg->stRoomTeam[nCount].nMemberID, pTeam->GetMemberIDList(), sizeof(pMsg->stRoomTeam[nCount].nMemberID));
        }
        else
        {
            memset(&(pMsg->stRoomTeam[nCount]), 0, sizeof(pMsg->stRoomTeam[nCount]));
        }

        nCount++;
    }

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_ACTIVITY_QUERY_TEAM));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTeamSyncTeamInfo(UINT32 dwTeamID, const char* szTeamName, UINT8  byAuctionItemColorLimit, UINT32 dwLeaderID, MS_ONLINE_ROLE_DATA* pstRoleData)
{
    INT32 nRetCode = 0;

    M2C_TEAM_SYNC_TEAM_INFO* pMsg = (M2C_TEAM_SYNC_TEAM_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_TEAM_SYNC_TEAM_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_team_sync_team_info;
    pMsg->dwTeamID = dwTeamID;
    pMsg->byAuctionItemColorLimit = byAuctionItemColorLimit;
    pMsg->dwLeaderID = dwLeaderID;
    strxcpy(pMsg->szTeamName, szTeamName, TEAM_NAME_LEN);

    nRetCode = _SendClient(pstRoleData->dwCliSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_TEAM_SYNC_TEAM_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
// ===== Team end

BOOL CMessageHandler::DoClientSelfInfo(MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    M2C_SELF_INFO* pMsg = (M2C_SELF_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_SELF_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_self_info;
    strxcpy(pMsg->szSignature, pstOnlineRole->m_szSignature, SIGNATURE_LEN);
    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szSignature);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_SELF_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendList(MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nSize = (INT32)(sizeof(M2C_FRIEND_LIST) + sizeof(FRIEND_INFO_WITH_GROUP) * (pstOnlineRole->m_friends.size() + pstOnlineRole->m_personalEnemy.size()));
    M2C_FRIEND_LIST* pMsg = (M2C_FRIEND_LIST*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_friend_list;
    pMsg->wSize = nSize;
    pMsg->byFriendCount = 0;
    for (MS_ONLINE_ROLE_DATA::FriendMap::iterator it = pstOnlineRole->m_friends.begin(); it != pstOnlineRole->m_friends.end(); ++it)
    {
        UINT32 friend_id = it->first;
        MS_ROLE_DATA* offline_data = NULL;
        MS_ONLINE_ROLE_DATA* online_data = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(friend_id);
        if (online_data)
        {
            offline_data = online_data->pRoleData;

            strxcpy(pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szSignature, online_data->m_szSignature, SIGNATURE_LEN);
            if (g_GWConfig.bFilterCensoredWord)
            {
                nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(
                    pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szSignature);
                LOG_CHECK_ERROR(nRetCode);
            }
            pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.byFriendState = online_data->m_byFriendState;
        }
        else
        {
            offline_data = CRoleDataMgr::GetInstance().FindRole(friend_id);

            pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szSignature[0] = 0;
            pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.byFriendState = FS_OFFLINE;
        }

        LOG_CHECK_ERROR(offline_data);
        if (offline_data == NULL)
            continue;

        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.dwRoleID = friend_id;
        strxcpy(pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szRoleName, offline_data->szRoleName, ROLE_NAME_LEN);
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.byClass = offline_data->byClass;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.bySex = offline_data->bySex;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.nSceneTempID = offline_data->nSceneTempID;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].byGroup = it->second;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].bMutual = (pstOnlineRole->m_fans.find(friend_id) == pstOnlineRole->m_fans.end() ? 0 : 1);

        pMsg->byFriendCount++;
    }
    for (MS_ONLINE_ROLE_DATA::PERSONAL_ENEMY_LIST::iterator it = pstOnlineRole->m_personalEnemy.begin(); it != pstOnlineRole->m_personalEnemy.end(); ++it)
    {
        UINT32 person_enemy_id = *it;
        MS_ROLE_DATA* offline_data = NULL;
        MS_ONLINE_ROLE_DATA* online_data = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(person_enemy_id);
        if (online_data)
        {
            offline_data = online_data->pRoleData;

            strxcpy(pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szSignature, online_data->m_szSignature, SIGNATURE_LEN);
            pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.byFriendState = online_data->m_byFriendState;
        }
        else
        {
            offline_data = CRoleDataMgr::GetInstance().FindRole(person_enemy_id);

            pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szSignature[0] = 0;
            pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.byFriendState = FS_OFFLINE;
        }

        LOG_CHECK_ERROR(offline_data);
        if (offline_data == NULL)
            continue;

        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.dwRoleID = person_enemy_id;
        strxcpy(pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.szRoleName, offline_data->szRoleName, ROLE_NAME_LEN);
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.byClass = offline_data->byClass;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.bySex = offline_data->bySex;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].stFriendInfo.nSceneTempID = offline_data->nSceneTempID;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].byGroup = FRIEND_GROUP_ENEMY;
        pMsg->stFriendInfoWithGroup[pMsg->byFriendCount].bMutual = 0;

        pMsg->byFriendCount++;
    }
    pMsg->byFansCount = (UINT8)pstOnlineRole->m_fans.size();

    nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendInfoChange(UINT32 dwClientSessionID, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    BOOL bRet = FALSE;
    M2C_FRIEND_INFO_CHANGE* pMsg = (M2C_FRIEND_INFO_CHANGE*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_INFO_CHANGE));
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pstOnlineRole);

    pMsg->wMsg = m2c_friend_info_change;
    pMsg->stFriendInfo.dwRoleID = pstOnlineRole->pRoleData->dwRoleID;
    strxcpy(pMsg->stFriendInfo.szRoleName, pstOnlineRole->pRoleData->szRoleName, ROLE_NAME_LEN);
    strxcpy(pMsg->stFriendInfo.szSignature, pstOnlineRole->m_szSignature, SIGNATURE_LEN);
    pMsg->stFriendInfo.byClass = pstOnlineRole->pRoleData->byClass;
    pMsg->stFriendInfo.bySex = pstOnlineRole->pRoleData->bySex;
    pMsg->stFriendInfo.byFriendState = pstOnlineRole->m_byFriendState;
    pMsg->stFriendInfo.nSceneTempID = pstOnlineRole->pRoleData->nSceneTempID;

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_INFO_CHANGE));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendInfoChange(UINT32 dwClientSessionID, MS_ROLE_DATA* pstRole)
{
    BOOL bRet = FALSE;
    M2C_FRIEND_INFO_CHANGE* pMsg = (M2C_FRIEND_INFO_CHANGE*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_INFO_CHANGE));
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pstRole);

    pMsg->wMsg = m2c_friend_info_change;
    pMsg->stFriendInfo.dwRoleID = pstRole->dwRoleID;
    strxcpy(pMsg->stFriendInfo.szRoleName, pstRole->szRoleName, ROLE_NAME_LEN);
    pMsg->stFriendInfo.szSignature[0] = 0;
    pMsg->stFriendInfo.byClass = pstRole->byClass;
    pMsg->stFriendInfo.bySex = pstRole->bySex;
    pMsg->stFriendInfo.byFriendState = FS_OFFLINE;
    pMsg->stFriendInfo.nSceneTempID = 0;

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_INFO_CHANGE));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendOffline(UINT32 dwClientSessionID, MS_ROLE_DATA* pstRole)
{
    BOOL bRet = FALSE;
    M2C_FRIEND_INFO_CHANGE* pMsg = (M2C_FRIEND_INFO_CHANGE*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_INFO_CHANGE));
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pstRole);

    pMsg->wMsg = m2c_friend_info_change;
    pMsg->stFriendInfo.dwRoleID = pstRole->dwRoleID;
    strxcpy(pMsg->stFriendInfo.szRoleName, pstRole->szRoleName, ROLE_NAME_LEN);
    pMsg->stFriendInfo.byFriendState = FS_OFFLINE;

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_INFO_CHANGE));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendAdd(UINT32 dwClientSessionID, UINT32 dwRoleID, const char* szRoleName, UINT8 bMutual)
{
    BOOL bRet = FALSE;
    M2C_FRIEND_ADD* pMsg = (M2C_FRIEND_ADD*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_ADD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_friend_add;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szRoleName, szRoleName, ROLE_NAME_LEN);
    pMsg->bMutual = bMutual;

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_ADD));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendDel(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    BOOL bRet = FALSE;
    M2C_FRIEND_DEL* pMsg = (M2C_FRIEND_DEL*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_DEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_friend_del;
    pMsg->dwRoleID = dwRoleID;

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_DEL));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendAddedBy(UINT32 dwClientSessionID, UINT32 dwRoleID, const char* szRoleName)
{
    BOOL bRet = FALSE;
    M2C_FRIEND_ADDED_BY* pMsg = (M2C_FRIEND_ADDED_BY*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_ADDED_BY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_friend_added_by;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szRoleName, szRoleName, ROLE_NAME_LEN);

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_ADDED_BY));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFriendDeletedBy(UINT32 dwClientSessionID, UINT32 dwRoleID, const char* szRoleName)
{
    BOOL bRet = FALSE;

    M2C_FRIEND_DELETED_BY* pMsg = (M2C_FRIEND_DELETED_BY*)m_pPacketAllocator->Alloc(sizeof(M2C_FRIEND_DELETED_BY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_friend_deleted_by;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szRoleName, szRoleName, ROLE_NAME_LEN);

    bRet = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FRIEND_DELETED_BY));
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientPersonalEnemyAdd(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32   nRetCode = 0;
    M2C_PERSONAL_ENEMY_ADD* pMsg = (M2C_PERSONAL_ENEMY_ADD*)m_pPacketAllocator->Alloc(sizeof(M2C_PERSONAL_ENEMY_ADD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_personal_enemy_add;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_PERSONAL_ENEMY_ADD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientPersonalEnemyDel(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32       nRetCode = 0;
    M2C_PERSONAL_ENEMY_DEL* pMsg = (M2C_PERSONAL_ENEMY_DEL*)m_pPacketAllocator->Alloc(sizeof(M2C_PERSONAL_ENEMY_DEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_personal_enemy_del;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_PERSONAL_ENEMY_DEL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientChatMessage(const char* pSrcName, UINT32 dwSrcID, UINT32 dwTargetID,
    UINT32 dwLocation, const char* pcszMessage, INT32 nServerGroupID, BOOL bGM)
{
    INT32    nRetCode = 0;
    INT32	 nSize = 0;
    INT32	 nNameSize = 0;
    INT32	 nMessageSize = 0;
    UINT32	 dwTargetType = 0;
    M2C_CHAT_MESSAGE* pMsg = NULL;

    LOG_PROCESS_ERROR(pcszMessage);
    LOG_PROCESS_ERROR(pSrcName);

    nMessageSize = (INT32)strnlen(pcszMessage, MAX_CHAT_MESSAGE_LEN) + 1;
    LOG_PROCESS_ERROR(pcszMessage[nMessageSize - 1] == '\0');

    nNameSize = (INT32)strnlen(pSrcName, ROLE_NAME_LEN) + 1;
    LOG_PROCESS_ERROR(pSrcName[nNameSize - 1] == '\0');

    nSize = nMessageSize + nNameSize + sizeof(M2C_CHAT_MESSAGE);
    pMsg = (M2C_CHAT_MESSAGE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_chat_message;
    pMsg->wSize = nSize;
    pMsg->dwTargetID = dwTargetID;
    pMsg->dwSrcID = dwSrcID;
    pMsg->uLocation = dwLocation;
    pMsg->bGM = bGM;
    pMsg->uReserved = 0;
    strxcpy(pMsg->szMessage, pcszMessage, nMessageSize);
    strxcpy(pMsg->szMessage + nMessageSize, pSrcName, nNameSize);

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szMessage);
        LOG_CHECK_ERROR(nRetCode);
    }

    dwTargetType = GetTypeFromID(dwTargetID);

    switch (dwTargetType)
    {
        case otTeam:
        {
            CTeam* poTeam = CTeamMgr::GetInstance().Find(dwTargetID);
            LOG_PROCESS_ERROR(poTeam);

            nRetCode = _BroadcastTeam(poTeam, (PACKET_BUFFER*)pMsg, pMsg->wSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        case otChannel:
        {
            if (dwTargetID == cctServer)
            {
                nRetCode = _BroadcastAllClient((PACKET_BUFFER*)pMsg, pMsg->wSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            else if (dwTargetID == cctGroup)
            {
                nRetCode = _BroadcastGroupClient(nServerGroupID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                LOG_PROCESS_ERROR(0);
            }
        }
        break;
        case otRole:
        {
            MS_ONLINE_ROLE_DATA* pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwTargetID);
            LOG_PROCESS_ERROR(pOnlineRoleData);

            nRetCode = _SendClient(pOnlineRoleData->dwCliSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        case otGuild:
        {
            MS_ROLE_DATA* pRole = NULL;

            CMsGuild* poGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwTargetID);
            LOG_PROCESS_ERROR(poGuild);

            if (dwSrcID)
            {
                pRole = CRoleDataMgr::GetInstance().FindRole(dwSrcID);
                LOG_PROCESS_ERROR(pRole);

                nRetCode = poGuild->CanGuildChat(pRole->dwRoleID);
                LOG_PROCESS_ERROR(nRetCode);
            }

            nRetCode = _BroadcastGuild(poGuild, (PACKET_BUFFER*)pMsg, pMsg->wSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        default:
            LOG_PROCESS_ERROR(FALSE);
            break;
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientChatMessageToTarget(UINT32 dwTargetSessionID, const char* pSrcName, UINT32 dwSrcID, UINT32 dwTargetID,
    UINT32 dwLocation, const char* pcszMessage, INT32 nServerGroupID)
{
    INT32    nRetCode = 0;
    INT32	 nSize = 0;
    INT32	 nNameSize = 0;
    INT32	 nMessageSize = 0;
    UINT32	 dwTargetType = 0;
    M2C_CHAT_MESSAGE* pMsg = NULL;

    LOG_PROCESS_ERROR(pcszMessage);
    LOG_PROCESS_ERROR(pSrcName);

    nMessageSize = (INT32)strnlen(pcszMessage, MAX_CHAT_MESSAGE_LEN) + 1;
    LOG_PROCESS_ERROR(pcszMessage[nMessageSize - 1] == '\0');

    nNameSize = (INT32)strnlen(pSrcName, ROLE_NAME_LEN) + 1;
    LOG_PROCESS_ERROR(pSrcName[nNameSize - 1] == '\0');

    nSize = nMessageSize + nNameSize + sizeof(M2C_CHAT_MESSAGE);
    pMsg = (M2C_CHAT_MESSAGE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_chat_message;
    pMsg->wSize = nSize;
    pMsg->dwTargetID = dwTargetID;
    pMsg->dwSrcID = dwSrcID;
    pMsg->uLocation = dwLocation;
    pMsg->bGM = 0;
    pMsg->uReserved = 0;
    strxcpy(pMsg->szMessage, pcszMessage, nMessageSize);
    strxcpy(pMsg->szMessage + nMessageSize, pSrcName, nNameSize);

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szMessage);
        LOG_CHECK_ERROR(nRetCode);
    }

    dwTargetType = GetTypeFromID(dwTargetID);

    switch (dwTargetType)
    {
        case otTeam:
        {
            CTeam* poTeam = CTeamMgr::GetInstance().Find(dwTargetID);
            LOG_PROCESS_ERROR(poTeam);

            nRetCode = _SendClient(dwTargetSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        case otChannel:
        {
            if (dwTargetID == cctServer || dwTargetID == cctGroup)
            {
                nRetCode = _SendClient(dwTargetSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                LOG_PROCESS_ERROR(0);
            }
        }
        break;
        case otRole:
        {
            //MS_ONLINE_ROLE_DATA* pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwTargetID);
            //LOG_PROCESS_ERROR(pOnlineRoleData);

            nRetCode = _SendClient(dwTargetSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        case otGuild:
        {
            MS_ROLE_DATA* pRole = NULL;

            CMsGuild* poGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwTargetID);
            LOG_PROCESS_ERROR(poGuild);

            if (dwSrcID)
            {
                pRole = CRoleDataMgr::GetInstance().FindRole(dwSrcID);
                LOG_PROCESS_ERROR(pRole);

                nRetCode = poGuild->CanGuildChat(pRole->dwRoleID);
                LOG_PROCESS_ERROR(nRetCode);
            }

            nRetCode = _SendClient(dwTargetSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        default:
            LOG_PROCESS_ERROR(FALSE);
            break;
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientPublishQuest(UINT32 dwClientSessionID, UINT32 dwMasterRoleID, INT32 nQuestID)
{
    INT32 nRetCode = 0;
    M2C_PUBLISH_QUEST* pMsg = (M2C_PUBLISH_QUEST*)m_pPacketAllocator->Alloc(sizeof(M2C_PUBLISH_QUEST));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_publish_quest;
    pMsg->dwMasterRoleID = dwMasterRoleID;
    pMsg->nQuestID = nQuestID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_PUBLISH_QUEST));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientQueryLineAck(UINT32 dwClientSessionID, UINT32 dwSceneTempID, UINT32 dwLineCount, UINT32* plstLineID)
{
    M2C_QUERY_LINE_ACK* pMsg{};
    INT32 nRetCode = 0;
    INT32 nSize = sizeof(M2C_QUERY_LINE_ACK) + dwLineCount * sizeof(LINE_SCENE_DATA);
    const SCENE_DATA* pcSceneData = NULL;

    LOG_PROCESS_ERROR(dwLineCount < MAX_LINE_SCENE_NUMBER);

    pMsg = (M2C_QUERY_LINE_ACK*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pcSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(dwSceneTempID);
    LOG_PROCESS_ERROR(pcSceneData);

    pMsg->wMsg = m2c_query_line_ack;
    pMsg->wSize = nSize;
    pMsg->dwSceneTempID = dwSceneTempID;
    pMsg->wCount = 0;
    for (INT32 i = 0; i < dwLineCount; ++i)
    {
        pMsg->LineSceneData[pMsg->wCount].dwSceneID = plstLineID[i];

        SCENE_INFO* pSceneInfo = CSceneMgr::GetInstance().FindScene(dwSceneTempID, plstLineID[i]);
        if (pSceneInfo && (pSceneInfo->eState == ESCENESTATE_NORMAL || pSceneInfo->eState == ESCENESTATE_CREATING))
        {
            if ((pSceneInfo->nRoleCount + pSceneInfo->WaitUserList.size()) * 10000 < pcSceneData->nMaxRoleCount * g_GWConfig.nSceneBusyPercent)
                pMsg->LineSceneData[pMsg->wCount].byState = lssNormal;
            else if ((pSceneInfo->nRoleCount + pSceneInfo->WaitUserList.size()) * 10000 < pcSceneData->nMaxRoleCount * g_GWConfig.nSceneFullPercent)
                pMsg->LineSceneData[pMsg->wCount].byState = lssBusy;
            else
                pMsg->LineSceneData[pMsg->wCount].byState = lssFull;
        }
        else
        {
            pMsg->LineSceneData[pMsg->wCount].byState = lssInvalid;
        }

        pMsg->wCount++;
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMailWriteSuccess(UINT32 dwClientSessionID)
{
    INT32 nRetCode = 0;
    M2C_MAIL_WRITE_SUCCESS* pMsg = (M2C_MAIL_WRITE_SUCCESS*)m_pPacketAllocator->Alloc(sizeof(M2C_MAIL_WRITE_SUCCESS));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_mail_write_success;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MAIL_WRITE_SUCCESS));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;

}

BOOL CMessageHandler::DoClientMailListRecv(UINT32 dwClientSessionID, CBaseList& lstMails, UINT32 dwMaxMailCount)
{
    INT32 nRetCode = 0;
    M2C_MAIL_LIST_RECV* pMsg = (M2C_MAIL_LIST_RECV*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_mail_list_recv;
    pMsg->wMailCount = 0;

    for (BASE_LIST_NODE* node = lstMails.Head().pNext; node != &lstMails.Rear(); node = node->pNext)
    {
        MailInfo* mail = (MailInfo*)node;
        pMsg->stBriefMailInfo[pMsg->wMailCount].dwMailID = mail->dwMailID;
        pMsg->stBriefMailInfo[pMsg->wMailCount].dwMailTime = mail->dwMailTime;
        pMsg->stBriefMailInfo[pMsg->wMailCount].byMailType = mail->byMailType;
        pMsg->stBriefMailInfo[pMsg->wMailCount].byNew = mail->byNew;
        pMsg->stBriefMailInfo[pMsg->wMailCount].byAttachment = (mail->ma.byAttachType == ATTACHTYPE_NONE ? 0 : 1);
        strxcpy(pMsg->stBriefMailInfo[pMsg->wMailCount].szSenderRoleName, mail->szSenderRoleName, ROLE_NAME_LEN);
        strxcpy(pMsg->stBriefMailInfo[pMsg->wMailCount].szMailSubject, mail->szMailSubject, MAIL_SUBJECT_LEN);
        if (g_GWConfig.bFilterCensoredWord)
        {
            nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->stBriefMailInfo[pMsg->wMailCount].szMailSubject);
            LOG_CHECK_ERROR(nRetCode);
        }

        pMsg->wMailCount++;

        //Check bounds
        if (pMsg->wMailCount >= dwMaxMailCount)
            break;
    }

    pMsg->wSize = (UINT16)sizeof(M2C_MAIL_LIST_RECV) + (UINT16)sizeof(BriefMailInfo) * pMsg->wMailCount;
    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMailRead(UINT32 dwClientSessionID, const MailInfo* mail)
{
    INT32 nRetCode = 0;
    M2C_MAIL_READ* pMsg = (M2C_MAIL_READ*)m_pPacketAllocator->Alloc(sizeof(M2C_MAIL_READ));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_mail_read;
    pMsg->dwMailID = mail->dwMailID;
    pMsg->dwMailTime = mail->dwMailTime;
    pMsg->byMailType = mail->byMailType;

    if (mail->ma.byAttachType == ATTACHTYPE_SIMPLE)
    {
        pMsg->nAttachmentItemID = mail->ma.sa.nItemID;
        pMsg->nAttachmentItemCount = mail->ma.sa.nItemCount;
    }
    else if (mail->ma.byAttachType == ATTACHTYPE_BUFFER)
    {
        ItemInfo iteminfo;
        iteminfo.Load(mail->ma.buffer);
        pMsg->nAttachmentItemID = iteminfo.m_nTemplateID;
        pMsg->nAttachmentItemCount = iteminfo.m_nCurDur;
    }
    else
    {
        pMsg->nAttachmentItemID = MONEYTYPE_INVALID;
        pMsg->nAttachmentItemCount = 0;
    }

    strxcpy(pMsg->szSenderRoleName, mail->szSenderRoleName, ROLE_NAME_LEN);
    strxcpy(pMsg->szMailSubject, mail->szMailSubject, MAIL_SUBJECT_LEN);
    strxcpy(pMsg->szMailContent, mail->szMailContent, MAIL_CONTENT_LEN);

    if (g_GWConfig.bFilterCensoredWord)
    {
        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szMailSubject);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CManageServer::GetInstance().ReplaceCensoredWord(pMsg->szMailContent);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MAIL_READ));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMailDeleteSuccess(UINT32 dwClientSessionID, UINT32 dwMailID)
{
    INT32 nRetCode = 0;
    M2C_MAIL_DELETE_SUCCESS* pMsg = (M2C_MAIL_DELETE_SUCCESS*)m_pPacketAllocator->Alloc(sizeof(M2C_MAIL_DELETE_SUCCESS));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_mail_delete_success;
    pMsg->dwMailID = dwMailID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MAIL_DELETE_SUCCESS));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMailNewCount(UINT32 dwClientSessionID, CBaseList& lstMails)
{
    INT32 nRetCode = 0;
    M2C_MAIL_NEW_COUNT* pMsg = (M2C_MAIL_NEW_COUNT*)m_pPacketAllocator->Alloc(sizeof(M2C_MAIL_NEW_COUNT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_mail_new_count;
    pMsg->dwMailNewCount = 0;

    for (BASE_LIST_NODE* node = lstMails.Head().pNext; node != &lstMails.Rear(); node = node->pNext)
    {
        MailInfo* mail = (MailInfo*)node;
        if (mail->byNew)
            pMsg->dwMailNewCount++;
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MAIL_NEW_COUNT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientMailGetAttachAccess(UINT32 dwClientSessionID, UINT32 dwMailID)
{
    INT32 nRetCode = 0;
    M2C_MAIL_GET_ATTACH_SUCCESS* pMsg = (M2C_MAIL_GET_ATTACH_SUCCESS*)m_pPacketAllocator->Alloc(sizeof(M2C_MAIL_GET_ATTACH_SUCCESS));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_mail_get_attach_success;
    pMsg->dwMailID = dwMailID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_MAIL_GET_ATTACH_SUCCESS));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientJoinBattleQueue(UINT32 dwClientSessionID)
{
    INT32 nRetCode = 0;
    M2C_JOIN_BATTLE_QUEUE* pMsg = (M2C_JOIN_BATTLE_QUEUE*)m_pPacketAllocator->Alloc(sizeof(M2C_JOIN_BATTLE_QUEUE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_join_battle_queue;
    pMsg->dwAverageQueueTime = 0;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_JOIN_BATTLE_QUEUE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientCancelJoinBattleAck(UINT32 dwClientSessionID, UINT32 dwCancelJoinRoleID)
{
    INT32 nRetCode = 0;
    M2C_CANCEL_JOIN_BATTLE_ACK* pMsg = (M2C_CANCEL_JOIN_BATTLE_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_CANCEL_JOIN_BATTLE_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_cancel_join_battle_ack;
    pMsg->dwCancelJoinRoleID = dwCancelJoinRoleID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_CANCEL_JOIN_BATTLE_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientJoinBattleConfirmReq(UINT32 dwClientSessionID, UINT32 dwSceneTempID)
{
    INT32 nRetCode = 0;
    M2C_JOIN_BATTLE_CONFIRM_REQ* pMsg = (M2C_JOIN_BATTLE_CONFIRM_REQ*)m_pPacketAllocator->Alloc(sizeof(M2C_JOIN_BATTLE_CONFIRM_REQ));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_join_battle_confirm_req;
    pMsg->dwSceneTempID = dwSceneTempID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_JOIN_BATTLE_CONFIRM_REQ));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientArenaGetDataAck(UINT32 dwClientSessionID, UINT64 qwTotalFund, UINT8 byMaxVictoryTimes, UINT8 byRank, INT32 nRewardMoney, UINT32 dwRewardItemID)
{
    INT32 nRetCode = 0;
    M2C_ARENA_GET_DATA_ACK* pMsg = (M2C_ARENA_GET_DATA_ACK*)m_pPacketAllocator->Alloc(sizeof(M2C_ARENA_GET_DATA_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_arena_get_data_ack;
    pMsg->qwTotalFund = qwTotalFund;
    pMsg->byChampionMaxVictoryTimes = byMaxVictoryTimes;
    pMsg->byRank = byRank;
    pMsg->nRewardMoney = nRewardMoney;
    pMsg->dwRewardItemID = dwRewardItemID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_ARENA_GET_DATA_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientArenaEnd(UINT32 dwClientSessionID)
{
    INT32 nRetCode = 0;
    M2C_ARENA_END* pMsg = (M2C_ARENA_END*)m_pPacketAllocator->Alloc(sizeof(M2C_ARENA_END));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_arena_end;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_ARENA_END));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientArenaMatchBeginNtf()
{
    INT32 nRetCode = 0;
    M2C_ARENA_MATCH_BEGIN_NTF* pMsg = (M2C_ARENA_MATCH_BEGIN_NTF*)m_pPacketAllocator->Alloc(sizeof(M2C_ARENA_MATCH_BEGIN_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_arena_match_begin_ntf;

    ERR("DoClientArenaMatchBeginNtf  bug  discover");

    nRetCode = _BroadcastAllClient((PACKET_BUFFER*)pMsg, sizeof(M2C_ARENA_MATCH_BEGIN_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// ===== Premium auction begin
BOOL CMessageHandler::DoClientAuctionSearchResult(UINT32 dwClientSessionID, ExternalMessage::FLEX_MESSAGE_HEADER* pMsg)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pMsg);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionQuery(UINT32 dwClientSessionID, Auction* pAuction, MS_ROLE_DATA* pSellerRole)
{
    INT32 nRetCode = 0;
    CMsGuild* pGuild = NULL;
    M2C_AUCTION_QUERY* pMsg = (M2C_AUCTION_QUERY*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_auction_query;
    pMsg->dwAuctionID = pAuction->m_dwAuctionID;
    if (pAuction)
    {
        pMsg->byFound = 1;
        pMsg->stAuction.dwAuctionID = pAuction->m_dwAuctionID;
        pMsg->stAuction.dwSellerRoleID = pAuction->m_dwSellerRoleID;
        pMsg->stAuction.dwBuyerRoleID = pAuction->m_dwBuyerRoleID;
        pMsg->stAuction.llPrice = pAuction->m_llPrice;
        pMsg->stAuction.llBuyoutPrice = pAuction->m_llBuyoutPrice;
        pMsg->stAuction.llDepositPrice = pAuction->m_llDepositPrice;
        pMsg->stAuction.dwStartTime = pAuction->m_dwStartTime;
        pMsg->stAuction.dwFinishTime = pAuction->m_dwFinishTime;
        pMsg->stAuction.byAuctionStatus = pAuction->m_byAuctionStatus;

        pMsg->dwBuyerCount = CAuctionMgr::GetInstance().GetBuyerCount(pAuction->m_dwAuctionID);
        pMsg->bySellerLevel = pSellerRole->byLevel;
        pMsg->bySellerClass = pSellerRole->byClass;

        // Seller name
        strxcpy(pMsg->szSellerName, pSellerRole->szRoleName, ROLE_NAME_LEN);

        // Seller guild name
        pGuild = CMsGuildMgr::GetInstance().FindGuildByID(pSellerRole->dwGuildID);
        if (pGuild)
        {
            strxcpy(pMsg->szGuildName, pGuild->GetName(), GUILD_NAME_LEN);
        }
        else
        {
            strxcpy(pMsg->szGuildName, "None", GUILD_NAME_LEN);
        }

        pMsg->wSize = sizeof(M2C_AUCTION_QUERY) + sizeof(AUCTION_ITEM_INFO) + pAuction->m_iteminfo.Save(pMsg->stAuction.acItem);
    }
    else
    {
        pMsg->byFound = 0;
        pMsg->wSize = sizeof(M2C_AUCTION_QUERY);
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionSell(UINT32 dwClientSessionID, Auction* pAuction)
{
    INT32 nRetCode = 0;
    M2C_AUCTION_SELL* pMsg = (M2C_AUCTION_SELL*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pAuction);

    pMsg->wMsg = m2c_auction_sell;
    pMsg->stAuction.dwAuctionID = pAuction->m_dwAuctionID;
    pMsg->stAuction.dwSellerRoleID = pAuction->m_dwSellerRoleID;
    pMsg->stAuction.dwBuyerRoleID = pAuction->m_dwBuyerRoleID;
    pMsg->stAuction.llPrice = pAuction->m_llPrice;
    pMsg->stAuction.llBuyoutPrice = pAuction->m_llBuyoutPrice;
    pMsg->stAuction.llDepositPrice = pAuction->m_llDepositPrice;
    pMsg->stAuction.dwStartTime = pAuction->m_dwStartTime;
    pMsg->stAuction.dwFinishTime = pAuction->m_dwFinishTime;
    pMsg->stAuction.byAuctionStatus = pAuction->m_byAuctionStatus;
    pMsg->wSize = sizeof(M2C_AUCTION_SELL) + pAuction->m_iteminfo.Save(pMsg->stAuction.acItem);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionCancelSell(UINT32 dwClientSessionID, UINT32 dwAuctionID)
{
    INT32 nRetCode = 0;
    M2C_AUCTION_CANCEL_SELL* pMsg = (M2C_AUCTION_CANCEL_SELL*)m_pPacketAllocator->Alloc(sizeof(M2C_AUCTION_CANCEL_SELL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_auction_cancel_sell;
    pMsg->dwAuctionID = dwAuctionID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_AUCTION_CANCEL_SELL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionBuy(UINT32 dwClientSessionID, Auction* pAuction)
{
    INT32 nRetCode = 0;
    M2C_AUCTION_BUY* pMsg = (M2C_AUCTION_BUY*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pAuction);

    pMsg->wMsg = m2c_auction_buy;
    pMsg->stAuction.dwAuctionID = pAuction->m_dwAuctionID;
    pMsg->stAuction.dwSellerRoleID = pAuction->m_dwSellerRoleID;
    pMsg->stAuction.dwBuyerRoleID = pAuction->m_dwBuyerRoleID;
    pMsg->stAuction.llPrice = pAuction->m_llPrice;
    pMsg->stAuction.llBuyoutPrice = pAuction->m_llBuyoutPrice;
    pMsg->stAuction.llDepositPrice = pAuction->m_llDepositPrice;
    pMsg->stAuction.dwStartTime = pAuction->m_dwStartTime;
    pMsg->stAuction.dwFinishTime = pAuction->m_dwFinishTime;
    pMsg->stAuction.byAuctionStatus = pAuction->m_byAuctionStatus;
    pMsg->wSize = sizeof(M2C_AUCTION_SELL) + pAuction->m_iteminfo.Save(pMsg->stAuction.acItem);

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionFinished(UINT32 dwClientSessionID, UINT32 dwAuctionID, UINT8 bySuccess)
{
    INT32 nRetCode = 0;
    M2C_AUCTION_FINISHED* pMsg = (M2C_AUCTION_FINISHED*)m_pPacketAllocator->Alloc(sizeof(M2C_AUCTION_FINISHED));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_auction_finished;
    pMsg->dwAuctionID = dwAuctionID;
    pMsg->bySuccess = bySuccess;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_AUCTION_FINISHED));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionListMySelling(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    const CAuctionMgr::AUCTION_LIST& auction_list = CAuctionMgr::GetInstance().GetSellingAuctions(dwRoleID);
    UINT32 cur_data_len = 0;

    M2C_AUCTION_LIST_MY_SELLING* pMsg = (M2C_AUCTION_LIST_MY_SELLING*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_auction_list_my_selling;
    pMsg->byCount = 0;

    for (CAuctionMgr::AUCTION_LIST::const_iterator it = auction_list.begin(); it != auction_list.end(); ++it)
    {
        const Auction* auction = *it;
        AUCTION_ITEM_INFO* aii = (AUCTION_ITEM_INFO*)(pMsg->acAuctionItems + cur_data_len);
        UINT32 iteminfo_len = 0;

        aii->dwAuctionID = auction->m_dwAuctionID;
        aii->dwSellerRoleID = auction->m_dwSellerRoleID;
        aii->dwBuyerRoleID = auction->m_dwBuyerRoleID;
        aii->llPrice = auction->m_llPrice;
        aii->llBuyoutPrice = auction->m_llBuyoutPrice;
        aii->llDepositPrice = auction->m_llDepositPrice;
        aii->dwStartTime = auction->m_dwStartTime;
        aii->dwFinishTime = auction->m_dwFinishTime;
        aii->byAuctionStatus = auction->m_byAuctionStatus;
        iteminfo_len = auction->m_iteminfo.Save(aii->acItem);

        cur_data_len += (sizeof(AUCTION_ITEM_INFO) + iteminfo_len);
        pMsg->byCount++;
    }

    pMsg->wSize = sizeof(M2C_AUCTION_LIST_MY_SELLING) + cur_data_len;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionListMyBuying(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    const CAuctionMgr::AUCTION_LIST& auction_list = CAuctionMgr::GetInstance().GetBuyingAuctions(dwRoleID);
    UINT32 cur_data_len = 0;

    M2C_AUCTION_LIST_MY_BUYING* pMsg = (M2C_AUCTION_LIST_MY_BUYING*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_auction_list_my_buying;
    pMsg->byCount = 0;

    for (CAuctionMgr::AUCTION_LIST::const_iterator it = auction_list.begin(); it != auction_list.end(); ++it)
    {
        const Auction* auction = *it;
        AUCTION_ITEM_INFO* aii = (AUCTION_ITEM_INFO*)(pMsg->acAuctionItems + cur_data_len);
        UINT32 iteminfo_len = 0;

        aii->dwAuctionID = auction->m_dwAuctionID;
        aii->dwSellerRoleID = auction->m_dwSellerRoleID;
        aii->dwBuyerRoleID = auction->m_dwBuyerRoleID;
        aii->llPrice = auction->m_llPrice;
        aii->llBuyoutPrice = auction->m_llBuyoutPrice;
        aii->llDepositPrice = auction->m_llDepositPrice;
        aii->dwStartTime = auction->m_dwStartTime;
        aii->dwFinishTime = auction->m_dwFinishTime;
        aii->byAuctionStatus = auction->m_byAuctionStatus;
        iteminfo_len = auction->m_iteminfo.Save(aii->acItem);

        cur_data_len += (sizeof(AUCTION_ITEM_INFO) + iteminfo_len);
        pMsg->byCount++;
    }

    pMsg->wSize = sizeof(M2C_AUCTION_LIST_MY_BUYING) + cur_data_len;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientAuctionPayLeftMoney(UINT32 dwClientSessionID, UINT32 dwAuctionID)
{
    INT32 nRetCode = 0;
    M2C_AUCTION_PAY_LEFT_MONEY* pMsg = (M2C_AUCTION_PAY_LEFT_MONEY*)m_pPacketAllocator->Alloc(sizeof(M2C_AUCTION_PAY_LEFT_MONEY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_auction_pay_left_money;
    pMsg->dwAuctionID = dwAuctionID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_AUCTION_PAY_LEFT_MONEY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
// ===== Premium auction end

// ===== Gold exchange begin
BOOL CMessageHandler::DoClientExchangeAddOrder(UINT32 dwClientSessionID, Order* pOrder)
{
    INT32 nRetCode = 0;
    M2C_EXCHANGE_ADD_ORDER* pMsg = (M2C_EXCHANGE_ADD_ORDER*)m_pPacketAllocator->Alloc(sizeof(M2C_EXCHANGE_ADD_ORDER));
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pOrder);

    pMsg->wMsg = m2c_exchange_add_order;
    pMsg->stOrder.qwOrderID = pOrder->m_qwOrderID;
    pMsg->stOrder.dwOrderTime = pOrder->m_dwOrderTime;
    pMsg->stOrder.byOrderType = pOrder->m_byOrderType;
    pMsg->stOrder.byOrderStatus = pOrder->m_byOrderStatus;
    pMsg->stOrder.nPrice = pOrder->m_nPrice;
    pMsg->stOrder.nCount = pOrder->m_nCount;
    pMsg->stOrder.nCurCount = pOrder->m_nCurCount;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_EXCHANGE_ADD_ORDER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientExchangeCancelOrder(UINT32 dwClientSessionID, UINT64 qwOrderID)
{
    INT32 nRetCode = 0;
    M2C_EXCHANGE_CANCEL_ORDER* pMsg = (M2C_EXCHANGE_CANCEL_ORDER*)m_pPacketAllocator->Alloc(sizeof(M2C_EXCHANGE_CANCEL_ORDER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_exchange_cancel_order;
    pMsg->qwOrderID = qwOrderID;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_EXCHANGE_CANCEL_ORDER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientExchangeListPrices(UINT32 dwClientSessionID, UINT8 byGroupID)
{
    INT32 nRetCode = 0;
    INT32 sell_price[MAX_EXCHANGE_PRICE_COUNT];
    INT32 sell_handcount[MAX_EXCHANGE_PRICE_COUNT];
    UINT8 sell_count = 0;
    INT32 buy_price[MAX_EXCHANGE_PRICE_COUNT];
    INT32 buy_handcount[MAX_EXCHANGE_PRICE_COUNT];
    UINT8 buy_count = 0;
    UINT32 size = 0;

    M2C_EXCHANGE_LIST_PRICES* pMsg = NULL;

    CExchangeMgr::GetInstance().GetPrices(byGroupID, eotSell, MAX_EXCHANGE_PRICE_COUNT, sell_count, sell_price, sell_handcount);
    CExchangeMgr::GetInstance().GetPrices(byGroupID, eotBuy, MAX_EXCHANGE_PRICE_COUNT, buy_count, buy_price, buy_handcount);
    size = sizeof(M2C_EXCHANGE_LIST_PRICES) + sizeof(EXCHANGE_PRICE_INFO) * (sell_count + buy_count);

    pMsg = (M2C_EXCHANGE_LIST_PRICES*)m_pPacketAllocator->Alloc(size);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_exchange_list_prices;
    pMsg->wSize = size;
    pMsg->bySellCount = sell_count;
    pMsg->byBuyCount = buy_count;
    for (UINT8 i = 0; i < sell_count; i++)
    {
        pMsg->stPrices[i].nPrice = sell_price[i];
        pMsg->stPrices[i].nCount = sell_handcount[i];
    }
    for (UINT8 i = 0; i < buy_count; i++)
    {
        pMsg->stPrices[sell_count + i].nPrice = buy_price[i];
        pMsg->stPrices[sell_count + i].nCount = buy_handcount[i];
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, size);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientExchangeListMyOrders(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    const CExchangeMgr::ORDER_LIST& sell_order_list = CExchangeMgr::GetInstance().GetOrders(eotSell, dwRoleID);
    const CExchangeMgr::ORDER_LIST& buy_order_list = CExchangeMgr::GetInstance().GetOrders(eotBuy, dwRoleID);
    const CExchangeMgr::ORDER_LIST& finished_order_list = CExchangeMgr::GetInstance().GetFinishedOrders(dwRoleID);
    UINT32 size = sizeof(M2C_EXCHANGE_LIST_MY_ORDERS) + sizeof(EXCHANGE_ORDER_INFO) *
        (sell_order_list.size() + buy_order_list.size() + finished_order_list.size());
    M2C_EXCHANGE_LIST_MY_ORDERS* pMsg = (M2C_EXCHANGE_LIST_MY_ORDERS*)m_pPacketAllocator->Alloc(size);
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(sell_order_list.size() <= MAX_EXCHANGE_SELLING_ORDERS);
    LOG_PROCESS_ERROR(buy_order_list.size() <= MAX_EXCHANGE_BUYING_ORDERS);
    LOG_PROCESS_ERROR(finished_order_list.size() <= MAX_EXCHANGE_FINISHED_ORDERS);

    pMsg->wMsg = m2c_exchange_list_my_orders;
    pMsg->wSize = size;
    pMsg->byCount = 0;
    for (CExchangeMgr::ORDER_LIST::const_iterator it = sell_order_list.begin(); it != sell_order_list.end(); ++it)
    {
        const Order* order = (*it);
        pMsg->stOrders[pMsg->byCount].qwOrderID = order->m_qwOrderID;
        pMsg->stOrders[pMsg->byCount].dwOrderTime = order->m_dwOrderTime;
        pMsg->stOrders[pMsg->byCount].byOrderType = order->m_byOrderType;
        pMsg->stOrders[pMsg->byCount].byOrderStatus = order->m_byOrderStatus;
        pMsg->stOrders[pMsg->byCount].nPrice = order->m_nPrice;
        pMsg->stOrders[pMsg->byCount].nCount = order->m_nCount;
        pMsg->stOrders[pMsg->byCount].nCurCount = order->m_nCurCount;

        pMsg->byCount++;
    }
    for (CExchangeMgr::ORDER_LIST::const_iterator it = buy_order_list.begin(); it != buy_order_list.end(); ++it)
    {
        const Order* order = (*it);
        pMsg->stOrders[pMsg->byCount].qwOrderID = order->m_qwOrderID;
        pMsg->stOrders[pMsg->byCount].dwOrderTime = order->m_dwOrderTime;
        pMsg->stOrders[pMsg->byCount].byOrderType = order->m_byOrderType;
        pMsg->stOrders[pMsg->byCount].byOrderStatus = order->m_byOrderStatus;
        pMsg->stOrders[pMsg->byCount].nPrice = order->m_nPrice;
        pMsg->stOrders[pMsg->byCount].nCount = order->m_nCount;
        pMsg->stOrders[pMsg->byCount].nCurCount = order->m_nCurCount;

        pMsg->byCount++;
    }
    for (CExchangeMgr::ORDER_LIST::const_iterator it = finished_order_list.begin(); it != finished_order_list.end(); ++it)
    {
        const Order* order = (*it);
        pMsg->stOrders[pMsg->byCount].qwOrderID = order->m_qwOrderID;
        pMsg->stOrders[pMsg->byCount].dwOrderTime = order->m_dwOrderTime;
        pMsg->stOrders[pMsg->byCount].byOrderType = order->m_byOrderType;
        pMsg->stOrders[pMsg->byCount].byOrderStatus = order->m_byOrderStatus;
        pMsg->stOrders[pMsg->byCount].nPrice = order->m_nPrice;
        pMsg->stOrders[pMsg->byCount].nCount = order->m_nCount;
        pMsg->stOrders[pMsg->byCount].nCurCount = order->m_nCurCount;

        pMsg->byCount++;
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, size);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientExchangeListMyLogs(UINT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    const CExchangeMgr::LOG_LIST& log_list = CExchangeMgr::GetInstance().GetLogs(dwRoleID);
    UINT32 size = sizeof(M2C_EXCHANGE_LIST_MY_LOGS) + sizeof(EXCHANGE_LOG_INFO) * log_list.size();
    M2C_EXCHANGE_LIST_MY_LOGS* pMsg = (M2C_EXCHANGE_LIST_MY_LOGS*)m_pPacketAllocator->Alloc(size);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_exchange_list_my_logs;
    pMsg->wSize = size;
    pMsg->nCount = 0;
    for (CExchangeMgr::LOG_LIST::const_iterator it = log_list.begin(); it != log_list.end(); ++it)
    {
        const ExchangeLog* log = (*it);
        pMsg->stLogs[pMsg->nCount].dwTime = log->dwTime;
        pMsg->stLogs[pMsg->nCount].byOrderType = log->byType;
        pMsg->stLogs[pMsg->nCount].nPrice = log->nPrice;
        pMsg->stLogs[pMsg->nCount].nCount = log->nCount;

        pMsg->nCount++;
    }

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, size);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
// ===== Gold exchange end

BOOL CMessageHandler::DoClientSyncRealMoney(UINT32 dwClientSessionID, INT32 nBalance)
{
    INT32 nRetCode = 0;
    M2C_SYNC_REAL_MONEY* pMsg = (M2C_SYNC_REAL_MONEY*)m_pPacketAllocator->Alloc(sizeof(M2C_SYNC_REAL_MONEY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_sync_real_money;
    pMsg->nBalance = nBalance;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_SYNC_REAL_MONEY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientRealMoneyBuy(UINT32 dwClientSessionID, INT32 nItemID, INT32 nMoneyLeft)
{
    INT32 nRetCode = 0;
    M2C_REAL_MONEY_BUY* pMsg = (M2C_REAL_MONEY_BUY*)m_pPacketAllocator->Alloc(sizeof(M2C_REAL_MONEY_BUY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_real_money_buy;
    pMsg->nItemID = nItemID;
    pMsg->nMoneyLeft = nMoneyLeft;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_REAL_MONEY_BUY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientRealMoneyShopLog(MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    UINT16 wSize = sizeof(M2C_REAL_MONEY_SHOP_LOG) + sizeof(SHOP_LOG) * pstOnlineRole->m_lstShopLog.size();
    M2C_REAL_MONEY_SHOP_LOG* pMsg = (M2C_REAL_MONEY_SHOP_LOG*)m_pPacketAllocator->Alloc(wSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_real_money_shop_log;
    pMsg->wSize = wSize;
    pMsg->nShopLogCount = 0;
    for (std::list<MS_ONLINE_ROLE_DATA::ShopLog>::const_iterator it = pstOnlineRole->m_lstShopLog.begin(); it != pstOnlineRole->m_lstShopLog.end(); ++it)
    {
        const MS_ONLINE_ROLE_DATA::ShopLog& shoplog = *it;
        pMsg->stShopLog[pMsg->nShopLogCount].nItemID = shoplog.nItemID;
        pMsg->stShopLog[pMsg->nShopLogCount].nItemCount = shoplog.nItemCount;
        pMsg->stShopLog[pMsg->nShopLogCount].nCost = shoplog.nCost;
        pMsg->stShopLog[pMsg->nShopLogCount].dwShopTime = shoplog.dwShopTime;

        pMsg->nShopLogCount;
    }

    nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pMsg, wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientSaleInfo(UINT32 dwClientSessionID)
{
    const std::list<CRealMoneyShop::SaleInfo>& lstSaleInfo = CRealMoneyShop::GetInstance().GetSaleInfoList();
    INT32 nIndex = 0;
    INT32 nRetCode = 0;
    M2C_SALE_INFO* pMsg = (M2C_SALE_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_SALE_INFO) + sizeof(SALE_INFO) * lstSaleInfo.size());
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_sale_info;

    for (std::list<CRealMoneyShop::SaleInfo>::const_iterator it = lstSaleInfo.begin(); it != lstSaleInfo.end(); ++it)
    {
        const CRealMoneyShop::SaleInfo& si = *it;
        pMsg->stSaleInfo[nIndex].dwTime = si.dwTime;
        pMsg->stSaleInfo[nIndex].nItemID = si.nItemID;
        strxcpy(pMsg->stSaleInfo[nIndex].szRoleName, si.szRoleName, ROLE_NAME_LEN);
        nIndex++;
    }

    pMsg->nSaleInfoCount = lstSaleInfo.size();
    pMsg->wSize = sizeof(M2C_SALE_INFO) + sizeof(SALE_INFO) * lstSaleInfo.size();

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoQueryRanklist(MS_ONLINE_ROLE_DATA* pOnlineRole, INT32 nRankListType, INT32 nPageNum)
{
    INT32 nRetCode = 0;
    M2C_QUERY_RANKLIST* pMsg = NULL;
    INT64 llKey = 0;
    INT32 nRankNodeCount = 0;
    CBaseList* pRankList = NULL;
    RANK_NODE* pRankNode = NULL;
    INT32 nIndex = 0;

    LOG_PROCESS_ERROR(pOnlineRole && pOnlineRole->pRoleData);
    LOG_PROCESS_ERROR((nRankListType > rlInvalid) && (nRankListType < rlTotal));
    LOG_PROCESS_ERROR((nPageNum >= 0) && (nPageNum < MAX_RANK_LIST_NODE_COUNT / MAX_RANK_NODE_NUM_PER_PAGE));

    pMsg = (M2C_QUERY_RANKLIST*)m_pPacketAllocator->Alloc(sizeof(M2C_QUERY_RANKLIST));
    LOG_PROCESS_ERROR(pMsg);

    memset(pMsg, 0, sizeof(M2C_QUERY_RANKLIST));
    pMsg->wMsg = m2c_query_ranklist;
    pMsg->byRankListType = (UINT8)nRankListType;

    nRankNodeCount = CRankListMgr::GetInstance().GetRankNodeCount(pOnlineRole->pRoleData->byGroup, pMsg->byRankListType);
    if (nRankNodeCount % MAX_RANK_NODE_NUM_PER_PAGE == 0)
        pMsg->nTotalPage = nRankNodeCount / MAX_RANK_NODE_NUM_PER_PAGE;
    else
        pMsg->nTotalPage = nRankNodeCount / MAX_RANK_NODE_NUM_PER_PAGE + 1;

    if (pMsg->byRankListType >= rlRoleBegin && pMsg->byRankListType < rlRoleEnd)
        llKey = pOnlineRole->pRoleData->dwRoleID;
    else if (pMsg->byRankListType >= rlGuildBegin && pMsg->byRankListType < rlGuildEnd)
        llKey = pOnlineRole->pRoleData->dwGuildID;
    //else if (pMsg->nRankListType >= rlArenaBegin && pMsg->nRankListType < rlArenaEnd)
    //    llKey = pOnlineRole->pRoleData->dwArenaTeamID;
    else
        LOG_PROCESS_ERROR(FALSE);

    pRankNode = CRankListMgr::GetInstance().GetSelfRank(pOnlineRole->pRoleData->byGroup, pMsg->byRankListType, llKey);
    if (pRankNode)
    {
        pMsg->wSelfRank = pRankNode->nSelfRank;
        pMsg->wLastSelfRank = pRankNode->nLastRank;
    }
    else
    {
        pMsg->wSelfRank = 0;
        pMsg->wLastSelfRank = 0;
    }

    pRankList = CRankListMgr::GetInstance().GetRankList(pOnlineRole->pRoleData->byGroup, pMsg->byRankListType);
    LOG_PROCESS_ERROR(pRankList);

    pRankNode = (RANK_NODE*)pRankList->Head().pNext;
    while (pRankNode != &pRankList->Rear())
    {
        if (nIndex >= nPageNum * MAX_RANK_NODE_NUM_PER_PAGE)
            break;
        nIndex++;
        pRankNode = (RANK_NODE*)pRankNode->pNext;
    }

    nIndex = 0;
    while (pRankNode != &pRankList->Rear())
    {
        if (nIndex >= MAX_RANK_NODE_NUM_PER_PAGE)
            break;

        pMsg->llKey[nIndex] = pRankNode->llKey;
        pMsg->llValue[nIndex] = pRankNode->llValue;
        pMsg->wLastRank[nIndex] = (UINT8)pRankNode->nLastRank;

        nIndex++;
        pRankNode = (RANK_NODE*)pRankNode->pNext;
    }

    nRetCode = _SendClient(pOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_QUERY_RANKLIST));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFcmState(UINT32 dwClientSessionID, INT32 nOnlineMinutes, INT32 nOfflineMinutes)
{
    INT32 nRetCode = 0;
    M2C_FCM_STATE* pMsg = (M2C_FCM_STATE*)m_pPacketAllocator->Alloc(sizeof(M2C_FCM_STATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_fcm_state;
    pMsg->nOnlineMinutes = nOnlineMinutes;
    pMsg->nOfflineMinutes = nOfflineMinutes;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FCM_STATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientTimeInfo(UINT32 dwClientSessionID, INT32 nPlayTimeLeft)
{
    INT32 nRetCode = 0;
    M2C_TIME_INFO* pMsg = (M2C_TIME_INFO*)m_pPacketAllocator->Alloc(sizeof(M2C_TIME_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_time_info;
    pMsg->nPlayTimeLeft = nPlayTimeLeft;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_TIME_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientBuyTime(UINT32 dwClientSessionID, UINT8 bAuto, UINT8 byResult)
{
    INT32 nRetCode = 0;
    M2C_BUY_TIME* pMsg = (M2C_BUY_TIME*)m_pPacketAllocator->Alloc(sizeof(M2C_BUY_TIME));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_buy_time;
    pMsg->bAuto = bAuto;
    pMsg->byResult = byResult;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_BUY_TIME));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoClientFreeMode(UINT32 dwClientSessionID, UINT8 bFreeMode)
{
    INT32 nRetCode = 0;
    M2C_FREE_MODE* pMsg = (M2C_FREE_MODE*)m_pPacketAllocator->Alloc(sizeof(M2C_FREE_MODE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2c_free_mode;
    pMsg->byFreeMode = bFreeMode;

    nRetCode = _SendClient(dwClientSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2C_FREE_MODE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

void CMessageHandler::OnClientMailWrite(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    C2M_MAIL_WRITE* pMsg = (C2M_MAIL_WRITE*)pPacketBuffer;

    INT32 nRetCode = CMailMgr::GetInstance().WriteMail(pstOnlineRole, pMsg->szRoleName, pMsg->szMailSubject, pMsg->szMailContent);
    if (nRetCode != errSuccess)
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
}

void CMessageHandler::OnClientMailListRecv(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = CMailMgr::GetInstance().ListMailRecv(pstOnlineRole);
    if (nRetCode != errSuccess)
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
}

void CMessageHandler::OnClientMailRead(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    C2M_MAIL_READ* pMsg = (C2M_MAIL_READ*)pPacketBuffer;

    INT32 nRetCode = CMailMgr::GetInstance().ReadMail(pstOnlineRole, pMsg->dwMailID);
    if (nRetCode != errSuccess)
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
}

void CMessageHandler::OnClientMailDelete(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    C2M_MAIL_DELETE* pMsg = (C2M_MAIL_DELETE*)pPacketBuffer;

    INT32 nRetCode = CMailMgr::GetInstance().DeleteMail(pstOnlineRole, pMsg->dwMailID);
    if (nRetCode != errSuccess)
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
}

void CMessageHandler::OnClientMailGetAttach(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    C2M_MAIL_GET_ATTACH* pMsg = (C2M_MAIL_GET_ATTACH*)pPacketBuffer;

    INT32 nRetCode = CMailMgr::GetInstance().GetMailAttach(pstOnlineRole, pMsg->dwMailID);
    if (nRetCode != errSuccess)
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
}

void CMessageHandler::OnClientJoinBattle(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    CTeam* poTeam = NULL;
    SBattleQueueInfo* pstBattleQueue = NULL;
    UINT32 dwID = 0;
    UINT32 dwRoleCount = 0;
    C2M_JOIN_BATTLE* pMsg = (C2M_JOIN_BATTLE*)pPacketBuffer;

    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsNotInQueue, errBattleQueueNow);

    poTeam = CTeamMgr::GetInstance().Find(pstOnlineRole->pRoleData->dwTeamID);
    if (poTeam)
    {
        LOG_PROCESS_ERROR(poTeam->GetLeader() == pstOnlineRole->pRoleData->dwRoleID);

        dwID = poTeam->GetTeamID();
        dwRoleCount = poTeam->GetMemberCount();
    }
    else
    {
        dwID = pstOnlineRole->pRoleData->dwRoleID;
        dwRoleCount = 1;
    }

    nRetCode = CBattleQueueMgr::GetInstance().JoinBattle(dwID, dwRoleCount, pMsg->dwBattleMatch);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (pstOnlineRole && pstOnlineRole->stBattleQueue.byQueue == qsQueueConfirm)
    {
        CBattleQueueMgr::GetInstance().CheckBattleTempRole(pstOnlineRole->pRoleData->dwRoleID, 0, 0);
        pstOnlineRole->stBattleQueue.byQueue = qsNotInQueue;
    }

    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);

    return;
}

void CMessageHandler::OnClientCancelJoinBattleReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    C2M_CANCEL_JOIN_BATTLE_REQ* pMsg = (C2M_CANCEL_JOIN_BATTLE_REQ*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->stBattleQueue.byQueue == qsInQueue, errBattleNotQueue);

    nRetCode = CBattleQueueMgr::GetInstance().CancelJoinBattle(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);

    return;
}

void CMessageHandler::OnClientJoinBattleConfirm(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    CTeam* poTeam = NULL;
    C2M_JOIN_BATTLE_CONFIRM_ACK* pMsg = (C2M_JOIN_BATTLE_CONFIRM_ACK*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole->stBattleQueue.byQueue == qsQueueConfirm);

    nRetCode = CBattleQueueMgr::GetInstance().CheckBattleTempRole(pstOnlineRole->pRoleData->dwRoleID,
        pstOnlineRole->stBattleQueue.dwJoinSceneTempID, pstOnlineRole->stBattleQueue.dwJoinSceneID, pstOnlineRole->stBattleQueue.dwJoinSide);
    PROCESS_ERROR_RET_CODE(nRetCode, errBattleConfirmTimeout);

    if (pMsg->byResult == etarAgree)
    {
        nRetCode = DoGSJoinBattle(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID,
            pstOnlineRole->stBattleQueue.dwJoinSceneTempID, pstOnlineRole->stBattleQueue.dwJoinSceneID, pstOnlineRole->stBattleQueue.dwJoinSide);
        LOG_PROCESS_ERROR(nRetCode);
    }

    memset(&(pstOnlineRole->stBattleQueue), 0, sizeof(pstOnlineRole->stBattleQueue));
    pstOnlineRole->stBattleQueue.byQueue = qsNotInQueue;

    return;
Exit0:
    if (pstOnlineRole)
        pstOnlineRole->stBattleQueue.byQueue = qsNotInQueue;

    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);

    return;
}

void CMessageHandler::OnClientArenaGetData(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    UINT32  dwItemID = 0;
    INT32   nRank = 0;
    INT32   nMoney = 0;
    COMPETITOR_DATA* pData = NULL;
    C2M_ARENA_GET_DATA* pMsg = (C2M_ARENA_GET_DATA*)pPacketBuffer;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    dwItemID = CArenaMgr::GetInstance().GetReward(pstOnlineRole->pRoleData->byGroup);
    LOG_PROCESS_ERROR(dwItemID);

    nRank = CArenaMgr::GetInstance().GetRank(pstOnlineRole->pRoleData->byGroup, pstOnlineRole->pRoleData->dwRoleID);
    nMoney = CArenaMgr::GetInstance().GetRewardMoney(pstOnlineRole->pRoleData->byGroup, nRank);

    pData = CArenaMgr::GetInstance().GetStrongest(pstOnlineRole->pRoleData->byGroup);
    //LOG_PROCESS_ERROR(pData);
    if (pData)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientArenaGetDataAck(pstOnlineRole->dwCliSessionID,
            CArenaMgr::GetInstance().GetFund(pstOnlineRole->pRoleData->byGroup), pData->byPointSeasonVictoryTimes,
            CArenaMgr::GetInstance().GetRank(pstOnlineRole->pRoleData->byGroup, pstOnlineRole->pRoleData->dwRoleID), nMoney, dwItemID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = CMessageHandler::GetInstance().DoClientArenaGetDataAck(pstOnlineRole->dwCliSessionID,
            CArenaMgr::GetInstance().GetFund(pstOnlineRole->pRoleData->byGroup), 0,
            CArenaMgr::GetInstance().GetRank(pstOnlineRole->pRoleData->byGroup, pstOnlineRole->pRoleData->dwRoleID), nMoney, dwItemID);
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

void CMessageHandler::OnClientShield(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ROLE_DATA* pTargetRoleData = NULL;
    MS_ONLINE_ROLE_DATA* pTargetOnlineRole = NULL;
    MS_ONLINE_ROLE_DATA::FriendMap::iterator iter;
    CManageServer& ms = CManageServer::GetInstance();

    C2M_SHIELD* pMsg = (C2M_SHIELD*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    if (pMsg->dwRoleID)
    {
        pTargetRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    }
    else
    {
        pTargetRoleData = CRoleDataMgr::GetInstance().FindRoleByName(pMsg->szRoleName);
    }
    PROCESS_ERROR_RET_CODE(pTargetRoleData, errNotFindTargetRole);
    PROCESS_ERROR_RET_CODE(pTargetRoleData->byGroup == pstOnlineRole->pRoleData->byGroup, errNotFindTargetRole);

    iter = pstOnlineRole->m_friends.find(pTargetRoleData->dwRoleID);
    if (iter != pstOnlineRole->m_friends.end())
    {
        nRetCode = DoGSDelFriend(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pTargetRoleData->dwRoleID, iter->second);
        LOG_CHECK_ERROR(nRetCode);
        nRetCode = DoClientFriendDel(pstOnlineRole->dwCliSessionID, pMsg->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        pstOnlineRole->m_friends.erase(iter);
        nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlDeleteFriend, NULL, NULL, "udud", pstOnlineRole->pRoleData->dwRoleID, pTargetRoleData->dwRoleID);
        LOG_PROCESS_ERROR(nRetCode);
    }

    pTargetOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pTargetRoleData->dwRoleID);
    if (pTargetOnlineRole)
    {
        pTargetOnlineRole->m_fans.erase(pstOnlineRole->pRoleData->dwRoleID);
        DoClientFriendDeletedBy(pTargetOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->szRoleName);
    }

    nRetCode = DoGSShield(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pTargetRoleData->dwRoleID, pTargetRoleData->szRoleName);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

// ===== Premium auction begin
void CMessageHandler::OnClientAuctionSearchType(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_AUCTION_SEARCH_TYPE* pMsg = (C2M_AUCTION_SEARCH_TYPE*)pPacketBuffer;

    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionQuerying, 0, 0);
        return;
    }

    nRetCode = CAuctionMgr::GetInstance().SearchType(pstOnlineRole->dwCliSessionID,
        pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->byGroup, pMsg->bySortBy, pMsg->byAscending, pMsg->byAuctionType);

    if (nRetCode == errSuccess)
    {
        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsAuctionSearch;
    }
    else
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
    }
}

void CMessageHandler::OnClientAuctionSearch(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_AUCTION_SEARCH* pMsg = (C2M_AUCTION_SEARCH*)pPacketBuffer;

    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionQuerying, 0, 0);
        return;
    }

    nRetCode = CAuctionMgr::GetInstance().SearchItem(pstOnlineRole->dwCliSessionID,
        pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->byGroup, pMsg->bySortBy, pMsg->byAscending, pMsg->nItemTemplateID);

    if (nRetCode == errSuccess)
    {
        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsAuctionSearch;
    }
    else
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
    }
}

void CMessageHandler::OnClientAuctionQuery(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    MS_ROLE_DATA* pSellerRole = NULL;
    C2M_AUCTION_QUERY* pMsg = (C2M_AUCTION_QUERY*)pPacketBuffer;
    Auction* auction = CAuctionMgr::GetInstance().GetAuction(pMsg->dwAuctionID);
    LOG_PROCESS_ERROR(auction);

    pSellerRole = CRoleDataMgr::GetInstance().FindRole(auction->m_dwSellerRoleID);
    LOG_PROCESS_ERROR(pSellerRole);

    DoClientAuctionQuery(pstOnlineRole->dwCliSessionID, auction, pSellerRole);

Exit0:
    //WRN("function - [CMessageHandler::OnClientAuctionQuery], the ID is %d, not find Auction obj", pMsg->dwAuctionID);
    return;
}

void CMessageHandler::OnClientAuctionSell(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_AUCTION_SELL* pMsg = (C2M_AUCTION_SELL*)pPacketBuffer;

    if (CAuctionMgr::GetInstance().GetSellingAuctions(pstOnlineRole->pRoleData->dwRoleID).size() >= g_GWConfig.nMaxAuctionSellingItems)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionSellingFull, 0, 0);
        return;
    }

    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionQuerying, 0, 0);
        return;
    }

    LOG_PROCESS_ERROR(pMsg->llPrice >= 0);
    LOG_PROCESS_ERROR(pMsg->llBuyoutPrice == 0 || pMsg->llBuyoutPrice >= pMsg->llPrice);
    nRetCode = DoGSAuctionSell(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->byConSlot, pMsg->byItemSlot, pMsg->byAuctionTimeType, pMsg->byOpen, pMsg->llPrice, pMsg->llBuyoutPrice, pMsg->llDepositePrice);
    LOG_PROCESS_ERROR(nRetCode);

    pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsGsAuctionSell;

Exit0:
    return;
}

void CMessageHandler::OnClientAuctionCancelSell(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    C2M_AUCTION_CANCEL_SELL* pMsg = (C2M_AUCTION_CANCEL_SELL*)pPacketBuffer;
    INT32 nRetCode = CAuctionMgr::GetInstance().CancelItem(pstOnlineRole->pRoleData, pMsg->dwAuctionID);
    if (nRetCode == errSuccess)
    {
        DoClientAuctionCancelSell(pstOnlineRole->dwCliSessionID, pMsg->dwAuctionID);
    }
    else
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
    }
}

void CMessageHandler::OnClientAuctionBuy(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    UINT8 byBuyType = 0;
    INT32 nRetCode = 0;
    MS_ROLE_DATA* role = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    Auction* auction = NULL;
    BOOL buying_out = FALSE;
    C2M_AUCTION_BUY* pMsg = (C2M_AUCTION_BUY*)pPacketBuffer;

    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionQuerying, 0, 0);
        return;
    }

    // Get the Auction object
    auction = CAuctionMgr::GetInstance().GetAuction(pMsg->dwAuctionID);
    if (auction == NULL)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionItemNotFound, 0, 0);
        return;
    }

    // Check whether the bid exceeds the current highest price
    if (pMsg->llBidPrice < auction->GetNextPrice())
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionBuyingPriceLow, 0, 0);
        return;
    }

    // Determine whether this is a buyout: llBidPrice is a bid if below the buyout price
    if (auction->m_llBuyoutPrice && pMsg->llBidPrice >= auction->m_llBuyoutPrice)
    {
        buying_out = TRUE;
    }

    // For a non-buyout bid, check whether the player exceeds the auction order limit
    if (!buying_out && CAuctionMgr::GetInstance().GetBuyingAuctions(pstOnlineRole->pRoleData->dwRoleID).size() >= g_GWConfig.nMaxAuctionBuyingItems)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionBuyingFull, 0, 0);
        return;
    }

    // Check whether buyer and seller are the same player
    if (auction->m_dwSellerRoleID == pstOnlineRole->pRoleData->dwRoleID)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionSellerCannotBuy, 0, 0);
        return;
    }

    // For non-buyout bids, the current highest bidder cannot bid again
    if (!buying_out && auction->m_dwBuyerRoleID == pstOnlineRole->pRoleData->dwRoleID)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAuctionBuyerCannotBuy, 0, 0);
        return;
    }

    // Ask GS to deduct the deposit for either buyout or bidding; the balance is paid later
    // Distinguish buyout from bidding: the price updates the Auction object and affects client display

    // For a first bid without an existing deposit, deduct the deposit
    if (!buying_out && !CAuctionMgr::GetInstance().IsBidder(pMsg->dwAuctionID, pstOnlineRole->pRoleData->dwRoleID))
    {
        byBuyType = EAuctionBuyType::eabtBid;
        nRetCode = DoGSAuctionBuy(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwAuctionID, pMsg->llBidPrice, auction->m_llBuyoutPrice, auction->m_llDepositPrice, byBuyType);
        LOG_PROCESS_ERROR(nRetCode);

        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsGsAuctionBuy;
    }
    // For a first-action buyout without an existing deposit, deduct the deposit
    else if (buying_out == TRUE && !CAuctionMgr::GetInstance().IsBidder(pMsg->dwAuctionID, pstOnlineRole->pRoleData->dwRoleID))
    {
        byBuyType = EAuctionBuyType::eabtBuyout;
        nRetCode = DoGSAuctionBuy(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwAuctionID, pMsg->llBidPrice, auction->m_llBuyoutPrice, auction->m_llDepositPrice, byBuyType);
        LOG_PROCESS_ERROR(nRetCode);

        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsGsAuctionBuy;
    }
    else
    {
        // For a buyout after a previous deposit, the player has already acted on this order
        // Do not deduct another deposit; update and close the order, then charge the balance

        role = CRoleDataMgr::GetInstance().FindRole(pstOnlineRole->pRoleData->dwRoleID);
        pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pstOnlineRole->pRoleData->dwRoleID);

        INT32 nRetCode = CAuctionMgr::GetInstance().BidItem(role, pMsg->dwAuctionID, pMsg->llBidPrice);
        LOG_CHECK_ERROR(nRetCode == errSuccess);
        if (nRetCode != errSuccess)
        {
            CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(role, "Auction System Error", "", MONEYTYPE_02, pMsg->llBidPrice);

            if (pOnlineRole)
                DoErrorNotify(pOnlineRole->dwCliSessionID, nRetCode, 0, 0);
        }

        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;
    }


Exit0:
    return;
}

void CMessageHandler::OnClientAuctionListMySelling(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    DoClientAuctionListMySelling(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
}

void CMessageHandler::OnClientAuctionListMyBuying(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    DoClientAuctionListMyBuying(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
}

void CMessageHandler::OnClientAuctionPayLeftMoney(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    UINT32 dwCurTime{};
    // At balance payment, the current player is the final bidder and the deposit has already been deducted
    INT32 nRetCode = 0;
    Auction* auction = NULL;
    C2M_AUCTION_PAY_LEFT_MONEY* pMsg = (C2M_AUCTION_PAY_LEFT_MONEY*)pPacketBuffer;

    // Get the player’s purchased item list
    const CAuctionMgr::AUCTION_LIST& auction_list = CAuctionMgr::GetInstance().GetBuyingAuctions(pstOnlineRole->pRoleData->dwRoleID);

    // Check the list for the corresponding order ID
    for (const auto it : auction_list)
    {
        if (it->m_dwAuctionID == pMsg->dwAuctionID)
        {
            auction = it;
            break;
        }
    }
    LOG_PROCESS_ERROR(auction);

    dwCurTime = CManageServer::GetInstance().GetGameTime();

    // After expiry, the auction is complete: use the order price and charge the difference after the deposit
    if (auction->m_dwFinishTime <= dwCurTime)
    {
        INT64 diffPrice = auction->m_llPrice - auction->m_llDepositPrice;
        diffPrice = (diffPrice < 0) ? 0 : diffPrice;    // Check whether the calculated result is below zero
        nRetCode = DoGSAuctionPay(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwAuctionID, diffPrice);
        LOG_PROCESS_ERROR(nRetCode);
    }
    // Payment before expiry means the order reached the buyout price
    else
    {
        INT64 diffPrice = auction->m_llBuyoutPrice - auction->m_llDepositPrice;
        diffPrice = (diffPrice < 0) ? 0 : diffPrice;    // Check whether the calculated result is below zero
        nRetCode = DoGSAuctionPay(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwAuctionID, diffPrice);
        LOG_PROCESS_ERROR(nRetCode);
    }


Exit0:
    // Display a red warning if the order does not exist, etc.
    return;
}
// ===== Premium auction end

// ===== Gold exchange begin
void CMessageHandler::OnClientExchangeAddOrder(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_EXCHANGE_ADD_ORDER* pMsg = (C2M_EXCHANGE_ADD_ORDER*)pPacketBuffer;

    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeQuerying, 0, 0);
        return;
    }

    LOG_PROCESS_ERROR(!pstOnlineRole->pUser->bFreeMode);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= g_GWConfig.nRoleLowLevel);
    LOG_PROCESS_ERROR(pMsg->byOrderType == eotBuy || pMsg->byOrderType == eotSell);
    LOG_PROCESS_ERROR(pMsg->nItemID == MONEYTYPE_02);//Currently only gold can be bought or sold
    LOG_PROCESS_ERROR(pMsg->nCount > 0);
    LOG_PROCESS_ERROR(pMsg->nPrice > 1);//Must exceed 1: tax is at least 1 and the platform rejects zero net proceeds
    LOG_PROCESS_ERROR(pMsg->byOrderType == eotSell || (INT64)pMsg->nPrice * (INT64)pMsg->nCount * 10000LL <= (INT64)MAX_MONEY);

    if (pMsg->byOrderType == eotBuy && CExchangeMgr::GetInstance().GetOrders(pMsg->byOrderType, pstOnlineRole->pRoleData->dwRoleID).size() >= MAX_EXCHANGE_BUYING_ORDERS)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeOrderFull, 0, 0);
        return;
    }
    if (pMsg->byOrderType == eotSell && CExchangeMgr::GetInstance().GetOrders(pMsg->byOrderType, pstOnlineRole->pRoleData->dwRoleID).size() >= MAX_EXCHANGE_SELLING_ORDERS)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeOrderFull, 0, 0);
        return;
    }

    // TODO: check price and upper/lower trading limits

    if (pMsg->byOrderType == eotBuy)
    {
        // Spend vouchers to buy gold
        if (!g_ServerConfig.MS.bOABillingEnable)
        {
            DoErrorNotify(pstOnlineRole->dwCliSessionID, errRealMoneyShopNotOpen, 0, 0);
            return;
        }

        nRetCode = COABillMgr::GetInstance().DoFreezeCurrency(pstOnlineRole, pMsg->nPrice * pMsg->nCount, pMsg->nPrice, pMsg->nCount);
        LOG_PROCESS_ERROR(nRetCode);

        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsBillFreeze;
    }
    else
    {
        // Receive vouchers for selling gold
        nRetCode = DoGSExchangeAddOrder(pstOnlineRole->dwGSSessionID, pstOnlineRole->pRoleData->dwRoleID, MONEYTYPE_02, pMsg->nCount * 10000,
            pMsg->byOrderType, pMsg->nPrice, pMsg->nCount);
        LOG_PROCESS_ERROR(nRetCode);

        pstOnlineRole->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsGsExchange;
    }

Exit0:
    return;
}

void CMessageHandler::OnClientExchangeCancelOrder(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_EXCHANGE_CANCEL_ORDER* pMsg = (C2M_EXCHANGE_CANCEL_ORDER*)pPacketBuffer;

    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeQuerying, 0, 0);
        return;
    }

    nRetCode = CExchangeMgr::GetInstance().CancelOrder(pstOnlineRole, pMsg->qwOrderID);
    if (nRetCode != errSuccess)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
    }
}

void CMessageHandler::OnClientExchangeListPrices(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeQuerying, 0, 0);
        return;
    }

    LOG_PROCESS_ERROR(!pstOnlineRole->pUser->bFreeMode);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= g_GWConfig.nRoleLowLevel);

    DoClientExchangeListPrices(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->byGroup);

Exit0:
    return;
}

void CMessageHandler::OnClientExchangeListMyOrders(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeQuerying, 0, 0);
        return;
    }

    LOG_PROCESS_ERROR(!pstOnlineRole->pUser->bFreeMode);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= g_GWConfig.nRoleLowLevel);

    DoClientExchangeListMyOrders(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);

Exit0:
    return;
}

void CMessageHandler::OnClientExchangeListMyLogs(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    if (pstOnlineRole->pRoleData->eAuctionQueryStatus != MS_ROLE_DATA::eqsFree)
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errExchangeQuerying, 0, 0);
        return;
    }

    LOG_PROCESS_ERROR(!pstOnlineRole->pUser->bFreeMode);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= g_GWConfig.nRoleLowLevel);

    DoClientExchangeListMyLogs(pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);

Exit0:
    return;
}
// ===== Gold exchange end

// Query ingots: MS sends to GS
void CMessageHandler::OnClientCheckRealMoney(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    // Default code
    //if (g_ServerConfig.MS.bOABillingEnable)
    //{
    //    COABillMgr::GetInstance().DoQueryAccount(pstOnlineRole->pUser);
    //}
    //else
    //{
    //    DoErrorNotify(pstOnlineRole->dwCliSessionID, errRealMoneyShopNotOpen, 0, 0);
    //}

    // Custom code
    DoGSQueryYuanBao(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
}

void CMessageHandler::OnClientCheckRealMoneyShopData(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    // Default code
//    C2M_CHECK_REAL_MONEY_SHOP_DATA* pMsg = (C2M_CHECK_REAL_MONEY_SHOP_DATA*)pPacketBuffer;
//    M2C_SYNC_REAL_MONEY_SHOP_DATA* pResponse = NULL;
//    INT32 nRetCode = 0;
//
//    if (g_ServerConfig.MS.bOABillingEnable)
//    {
//        pResponse = (M2C_SYNC_REAL_MONEY_SHOP_DATA*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
//        LOG_PROCESS_ERROR(pResponse);
//        CRealMoneyShop::GetInstance().SyncRealMoneyShopData(pMsg->dwCrc, pResponse);
//
//        nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pResponse, pResponse->wSize);
//        LOG_PROCESS_ERROR(nRetCode);
//    }
//    else
//    {
//        DoErrorNotify(pstOnlineRole->dwCliSessionID, errRealMoneyShopNotOpen, 0, 0);
//    }
//
//    return;
//
//Exit0:
//    if (pResponse)
//        m_pPacketAllocator->Free((PACKET_BUFFER*)pResponse);

    // Custom code
    C2M_CHECK_REAL_MONEY_SHOP_DATA* pMsg = (C2M_CHECK_REAL_MONEY_SHOP_DATA*)pPacketBuffer;
    M2C_SYNC_REAL_MONEY_SHOP_DATA* pResponse = NULL;
    INT32 nRetCode = 0;

    pResponse = (M2C_SYNC_REAL_MONEY_SHOP_DATA*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pResponse);
    CRealMoneyShop::GetInstance().SyncRealMoneyShopData(pMsg->dwCrc, pResponse);

    nRetCode = _SendClient(pstOnlineRole->dwCliSessionID, (PACKET_BUFFER*)pResponse, pResponse->wSize);
    LOG_PROCESS_ERROR(nRetCode);

    return;

Exit0:
    if (pResponse)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pResponse);
}

void CMessageHandler::OnClientRealMoneyBuy(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    // Default code
    //if (g_ServerConfig.MS.bOABillingEnable)
    //{
    //    C2M_REAL_MONEY_BUY* pMsg = (C2M_REAL_MONEY_BUY*)pPacketBuffer;
    //    INT32 nRetCode = CRealMoneyShop::GetInstance().Buy(pstOnlineRole, pMsg->nItemID, pMsg->nCount, pMsg->szRoleName);
    //    if (nRetCode != errSuccess)
    //        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
    //}
    //else
    //{
    //    DoErrorNotify(pstOnlineRole->dwCliSessionID, errRealMoneyShopNotOpen, 0, 0);
    //}

    C2M_REAL_MONEY_BUY* pMsg = (C2M_REAL_MONEY_BUY*)pPacketBuffer;
    INT32 nRetCode = CRealMoneyShop::GetInstance().Buy(pstOnlineRole, pMsg->nItemID, pMsg->nCount, pMsg->szRoleName);
    if (nRetCode != errSuccess)
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nRetCode, 0, 0);
}

void CMessageHandler::OnClientRealMoneyShopLog(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    pstOnlineRole->SendShopLog();
}

void CMessageHandler::OnClientSaleInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    DoClientSaleInfo(pstOnlineRole->dwCliSessionID);
}

// GM command
void CMessageHandler::OnClientGMCommand(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    BOOL bRet = FALSE;
    INT32 nRetCode = 0;
    C2M_GM_COMMAND* pMsg = (C2M_GM_COMMAND*)pPacketBuffer;
    CLuaScript* poScript = NULL;
    INT32 nGMRet = 0;

    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    // Additional code: check whether GM commands are enabled and check GM character names (FALSE lets anyone issue commands)
    if (g_ServerConfig.CC.bGmComandEnable == TRUE)
    {
        bRet = g_GM_CheckRoleName(pstOnlineRole->pRoleData->szRoleName);
        LOG_PROCESS_ERROR(bRet);
    }

    // Test code
    //UINT32 size = CDataManager<EQUIP_DATA>::GetInstance().GetDataSize();
    //WRN("EQUIP_DATA size %d", size);
    //const EQUIP_DATA* pcNpcData = CDataManager<EQUIP_DATA>::GetInstance().GetFirstData();
    //while (pcNpcData)
    //{
    //    WRN("物品序号 %d, 名字 %s", pcNpcData->m_nTemplateID, pcNpcData->m_szName);
    //    pcNpcData = CDataManager<EQUIP_DATA>::GetInstance().GetNextData();
    //}


#ifdef _FINAL
    nRetCode = _CallFinalGMCommand(pstOnlineRole, pMsg->szGMCommand, stClient);
    LOG_PROCESS_ERROR(nRetCode);

    //nRetCode = DoGMCientCmd(pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pUser->dwClientIp, pstOnlineRole->pRoleData->szRoleName, pMsg->szGMCommand);
    //LOG_PROCESS_ERROR(nRetCode);
#else // _FINAL
    nRetCode = _CallDebugGMCommand(pstOnlineRole, pMsg->szGMCommand);
    LOG_PROCESS_ERROR(nRetCode);
#endif	// _FINAL

    return;
Exit0:
    WRN("Someone Is Using The GM Command - RoleName [%s]", pstOnlineRole->pRoleData->szRoleName);
    return;
}

void CMessageHandler::OnClientQueryRanklist(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nRetCode = 0;
    C2M_QUERY_RANKLIST* pMsg = (C2M_QUERY_RANKLIST*)pPacketBuffer;

    nRetCode = DoQueryRanklist(pstOnlineRole, pMsg->nRanklistID, pMsg->nPageSize);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnClientAwardUseNumber(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    if (g_ServerConfig.MS.bOAAwardEnable)
    {
        C2M_AWARD_USE_NUMBER* pMsg = (C2M_AWARD_USE_NUMBER*)pPacketBuffer;

        BOOL bRet = CAwardManager::GetInstance().SendAwardReq(pstOnlineRole, eratAwardReq_AwardNumber, pMsg->chAwardNumber);
        if (!bRet)
        {
            DoErrorNotify(pstOnlineRole->dwCliSessionID, errAwardReqSentFailed, 0, 0);
        }
    }
    else
    {
        DoErrorNotify(pstOnlineRole->dwCliSessionID, errAwardNotEnabled, 0, 0);
    }

Exit0:
    return;
}

// Purchase a time card
void CMessageHandler::OnClientBuyTime(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    // If the client UI enters 1, pMsg->dwBuyTimeCost is 100: the client multiplies by 100 before sending
    C2M_BUY_TIME* pMsg = (C2M_BUY_TIME*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg->dwBuyTimeCost > 0 && pMsg->dwBuyTimeCost <= 10000000);
    LOG_PROCESS_ERROR(pMsg->dwBuyTimeCost % 100 == 0);

    // Default code
    //COABillMgr::GetInstance().DoBuyTime(pstOnlineRole, pMsg->dwBuyTimeCost, pMsg->dwBuyTimeCost / 100 * 6000, FALSE);

    // Custom code: do not use the default time-card system
    DoGSCustomRoleBuyTime(pstOnlineRole->dwGSSessionID, pstOnlineRole->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID, pMsg->dwBuyTimeCost);

Exit0:
    return;
}

// Allow heaven-and-earth unity
void CMessageHandler::OnClientChangeRecallState(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    C2M_CHANGE_RECALL_STATE* pMsg = (C2M_CHANGE_RECALL_STATE*)pPacketBuffer;

    LOG_PROCESS_ERROR(pMsg->byEnabled <= 1);

    pstOnlineRole->bEnabledTeamRecall = pMsg->byEnabled;

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnDynamicAuth(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
}

void CMessageHandler::OnArenaFund(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, MS_ONLINE_ROLE_DATA* pstOnlineRole)
{
}