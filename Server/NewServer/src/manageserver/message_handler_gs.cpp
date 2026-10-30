#include "stdafx.h"
#include "message_handler.h"

#include <time.h>

#include "g2m_message.h"
#include "m2g_message.h"
#include "common_message.h"
#include "m2t_message.h"
#include "chat_def.h"

#include "time_ex.h"
#include "guild_config.h"
#include "game_world_config.h"

#include "client/usermgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "scene/scenemgr.h"
#include "scene_data.h"
#include "team/team.h"

#include "manageserver.h"
#include "ms_session_mgr.h"
#include "gs_info_mgr.h"
#include "role_data.h"
#include "rsa_sign.h"
#include "gs_rsakey_mgr.h"
#include "master/master_mgr.h"
#include "mail/mail_mgr.h"
#include "script_mgr.h"
#include "queuematch/battlequeuemgr.h"
#include "./guild/ms_guild_mgr.h"
#include "../gameworld/item.h"
#include "math_ex.h"
#include "drop/ms_drop_mgr.h"
#include "auction/auction_mgr.h"
#include "exchange/exchange_mgr.h"
#include "guild_dungeon_data.h"
#include "ms_adventure.h"
#include "data/arena_data.h"
#include "award/award_mgr.h"
#include "chat/ms_chat.h"
#include "ranklist/ranklist.h"
#include "mail_template_data.h"
#include "guild_building_data.h"
#include "master_gift.h"
#include "arena/arena_mgr.h"
#include "gm/gm_def.h"
#include "gm/gm_manager.h"
#include "server_config.h"
#include "server_var.h"
#include "achievement_data.h"
#include "real_money_shop/real_money_shop.h"

#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"
#include "fcm/fcm.h"
#include "real_money_shop/oa_bill_mgr.h"
#include "blackmarket/ms_black_market_mgr.h"

using namespace InternalMessage;

void CMessageHandler::OnGSCreateSceneAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_CREATE_SCENE_ACK* pMsg = (G2M_CREATE_SCENE_ACK*)pPacketBuffer;
    UINT32 dwGSSessionID = 0;
    SCENE_INFO* pDestSceneInfo = NULL;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);
    dwGSSessionID = pGSSession->dwSessionID;

    pDestSceneInfo = CSceneMgr::GetInstance().FindScene(pMsg->nSceneTempID, pMsg->nSceneID);
    LOG_PROCESS_ERROR(pDestSceneInfo);
    LOG_PROCESS_ERROR(pDestSceneInfo->eState == ESCENESTATE_CREATING);

    if (pMsg->nErrCode != errSuccess)
    {
        for (std::list<UINT32>::iterator itr = pDestSceneInfo->WaitUserList.begin(); itr != pDestSceneInfo->WaitUserList.end(); ++itr)
        {
            MS_USER* poUser = CUserMgr::GetInstance().FindUserBySessionID(*itr);
            if (poUser == NULL) //  Offline
                continue;

            MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            if (pOnlineRole == NULL || pOnlineRole->pRoleData == NULL)
            {
                KICK_USER(poUser->dwCliSessionID, ktImmediately);
                continue;
            }

            if (poUser->eState == USER_STATE_JUMP_WAIT_CREATE_SCENE)
            {
                nRetCode = DoGSQuerySceneAck(pOnlineRole->dwGSSessionID, pOnlineRole->dwCliSessionID, errSceneCreateSceneFailed,
                    pOnlineRole->pRoleData->dwRoleID, pOnlineRole->PosList[0].nSceneTempID, pOnlineRole->PosList[0].nSceneID,
                    pOnlineRole->PosList[0].nX, pOnlineRole->PosList[0].nY, pOnlineRole->PosList[0].nZ, pOnlineRole->PosList[0].nFaceDirection);
                LOG_PROCESS_ERROR(nRetCode);

                poUser->eState = USER_STATE_INGAME;

                MS_SYS_LOG_SUCCESS(poUser);
            }
            else if (poUser->eState == USER_STATE_LOGIN_WAIT_CREATE_SCENE)
            {
                nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pOnlineRole);
                LOG_CHECK_ERROR(nRetCode);

                KICK_USER(poUser->dwCliSessionID, ktImmediately);
                continue;
            }
            else
            {
                KICK_USER(poUser->dwCliSessionID, ktImmediately);
                continue;
            }
        }

        nRetCode = CSceneMgr::GetInstance().DestroyScene(pMsg->nSceneTempID, pMsg->nSceneID, pMsg->byGroupID);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        pDestSceneInfo->eState = ESCENESTATE_NORMAL;
        pGSSession->nWorkload++;

        for (std::list<UINT32>::iterator itr = pDestSceneInfo->WaitUserList.begin();
            itr != pDestSceneInfo->WaitUserList.end(); ++itr)
        {
            MS_USER* poUser = CUserMgr::GetInstance().FindUserBySessionID(*itr);
            if (poUser == NULL) //  Offline
                continue;

            MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
            if (!pOnlineRole)
            {
                KICK_USER(poUser->dwCliSessionID, ktImmediately);
                continue;
            }
            LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

            if (poUser->eState == USER_STATE_LOGIN_WAIT_CREATE_SCENE)
            {
                nRetCode = _LoginQuerySceneProc(pOnlineRole, pOnlineRole->pRoleData->dwRoleID);
                LOG_CHECK_ERROR(nRetCode);
            }
            else if (poUser->eState == USER_STATE_JUMP_WAIT_CREATE_SCENE)
            {
                if (pOnlineRole->dwGSSessionID == dwGSSessionID)
                {
                    nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pOnlineRole, pDestSceneInfo);
                    LOG_PROCESS_ERROR(nRetCode);

                    nRetCode = DoGSQuerySceneAck(pOnlineRole->dwGSSessionID, pOnlineRole->dwCliSessionID, errSuccess,
                        pOnlineRole->pRoleData->dwRoleID, pOnlineRole->PosList[0].nSceneTempID, pOnlineRole->PosList[0].nSceneID,
                        pOnlineRole->PosList[0].nX, pOnlineRole->PosList[0].nY, pOnlineRole->PosList[0].nZ, pOnlineRole->PosList[0].nFaceDirection);
                    LOG_PROCESS_ERROR(nRetCode);

                    nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
                        pOnlineRole->pRoleData->nSceneTempID, pOnlineRole->pRoleData->nSceneID, TRUE);
                    LOG_PROCESS_ERROR(nRetCode);

                    nRetCode = COnlineRoleDataMgr::GetInstance().EnterScene(pOnlineRole, pDestSceneInfo);
                    LOG_PROCESS_ERROR(nRetCode);

                    poUser->eState = USER_STATE_INGAME;
                }
                else
                {
                    LOG_PROCESS_ERROR(pOnlineRole->pUser);

                    nRetCode = DoGSPreEnterGS(dwGSSessionID, pOnlineRole->pUser, pOnlineRole->pRoleData->dwRoleID,
                        pOnlineRole->PosList[0].nSceneTempID, pOnlineRole->PosList[0].nSceneID,
                        pOnlineRole->PosList[0].nX, pOnlineRole->PosList[0].nY, pOnlineRole->PosList[0].nZ,
                        pOnlineRole->PosList[0].nFaceDirection);
                    LOG_PROCESS_ERROR(nRetCode);

                    nRetCode = COnlineRoleDataMgr::GetInstance().PreEnterScene(pOnlineRole, pDestSceneInfo);
                    LOG_PROCESS_ERROR(nRetCode);

                    pOnlineRole->dwTargetGSSessionID = pDestSceneInfo->dwGSSessionID;
                    poUser->eState = USER_STATE_JUMP_PRE_ENTER_GS;
                }
            }
            else
            {
                KICK_USER(poUser->dwCliSessionID, ktImmediately);
                continue;
            }

            MS_SYS_LOG_SUCCESS(poUser);
        }
        pDestSceneInfo->WaitUserList.clear();
    }
    return;
Exit0:
    INF("create scene(%d, %d) error, gs session id %d", pMsg->nSceneTempID, pMsg->nSceneID, dwGSSessionID);
    return;
}

void CMessageHandler::OnGSDestroySceneAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    UINT32 dwClientSessionID = 0;
    INT32 nRetCode = 0;
    MS_USER* poUser = NULL;
    MS_ROLE_DATA* pstRoleData = NULL;
    SCENE_INFO* pstSceneInfo = NULL;
    G2M_DESTROY_SCENE_ACK* pMsg = NULL;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);

    pMsg = (G2M_DESTROY_SCENE_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pstSceneInfo = CSceneMgr::GetInstance().FindScene(pMsg->nSceneTempID, pMsg->nSceneID);
    LOG_PROCESS_ERROR(pstSceneInfo);

    LOG_CHECK_ERROR(pMsg->nErrCode == errSuccess);
    LOG_PROCESS_ERROR(pstSceneInfo->nRoleCount == 0);
    LOG_PROCESS_ERROR(pstSceneInfo->eState == ESCENESTATE_DESTROYING);

    nRetCode = CSceneMgr::GetInstance().DestroyScene(pMsg->nSceneTempID, pMsg->nSceneID, pMsg->byGroupID);
    LOG_PROCESS_ERROR(nRetCode);

    pGSSession->nWorkload--;

    INF("destroy scene [%d, %d]", pMsg->nSceneTempID, pMsg->nSceneID);

    return;
Exit0:
    if (pMsg)
        CRI("destroy scene [%d, %d] error", pMsg->nSceneTempID, pMsg->nSceneID);
    else
        CRI("destroy scene error");

    return;
}

void CMessageHandler::OnGSCloseScene(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    SCENE_INFO* pstSceneInfo = NULL;
    G2M_CLOSE_SCENE* pMsg = NULL;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);

    pMsg = (G2M_CLOSE_SCENE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pstSceneInfo = CSceneMgr::GetInstance().FindScene(pMsg->nSceneTempID, pMsg->nSceneID);
    LOG_PROCESS_ERROR(pstSceneInfo);

    pstSceneInfo->eState = ESCENESTATE_CLOSING;
    pstSceneInfo->bDelete = TRUE;

    INF("GS %d Closing Scene[%d, %d] Success", pGSSession->dwSessionID, pMsg->nSceneTempID, pMsg->nSceneID);
    return;
Exit0:
    INF("GS %d Closing Scene[%d, %d] Error", pGSSession->dwSessionID, pMsg->nSceneTempID, pMsg->nSceneID);
    return;
}

void CMessageHandler::OnGSQueryScene(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nResult = errUnknownError;
    INT32 nRetCode = 0;
    G2M_QUERY_SCENE* pMsg = (G2M_QUERY_SCENE*)pPacketBuffer;
    MS_USER* poUser = NULL;
    MS_ONLINE_ROLE_DATA* poOnlineRole = NULL;
    const SCENE_DATA* pstSceneData;
    UINT32 dwGSSessionID = 0;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);
    dwGSSessionID = pGSSession->dwSessionID;

    // The user must exist, but the state may be invalid
    poUser = CUserMgr::GetInstance().FindUserBySessionID(pMsg->dwClientSessionID);
    LOG_PROCESS_ERROR_RET_CODE(poUser, errCritical);
    PROCESS_ERROR(poUser->eState == USER_STATE_INGAME);

    poOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poOnlineRole, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(poOnlineRole->pRoleData, errCritical);
    LOG_PROCESS_ERROR_RET_CODE(poOnlineRole->pRoleData->dwRoleID == pMsg->dwRoleID, errCritical);

    pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pMsg->nSceneTempID);
    LOG_PROCESS_ERROR_RET_CODE(pstSceneData, errCritical);

    switch (pstSceneData->nType)
    {
        case estStatic:
        case estBirth:
            nRetCode = _HandleStaticScene(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estUnique:
            nRetCode = _ChangeScene(poOnlineRole, pMsg->nSceneTempID, 1, pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estLine:
            nRetCode = _HandleLineScene(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estBattleField:
            nRetCode = _HandleBattlefield(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estArena:
            nRetCode = _HandleArena(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estDungeon:
            nRetCode = _HandleDungeon(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estSoloDungeon:
            nRetCode = _HandleSoloDungeon(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estGuild:
            nRetCode = _HandleGuildScene(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        case estGuildDungeon:
            nRetCode = _HandleGuildDungeon(poOnlineRole, pMsg->nSceneTempID, pMsg->nSceneID,
                pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
            PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
    }

    MS_SYS_LOG_SUCCESS(poUser);
    return;
Exit0:
    MS_SYS_LOG_ERROR(poUser);

    if (nResult == errCritical)
    {
        KICK_USER(pMsg->dwClientSessionID, ktLogged);
    }
    else
    {
        nRetCode = CMessageHandler::GetInstance().DoGSQuerySceneAck(dwGSSessionID, pMsg->dwClientSessionID, nResult,
            pMsg->dwRoleID, pMsg->nSceneTempID, pMsg->nSceneID,
            pMsg->nDestX, pMsg->nDestY, pMsg->nDestZ, pMsg->nFaceDirection);
        LOG_CHECK_ERROR(nRetCode);
    }

    return;
}

void CMessageHandler::OnGSPreEnterGSAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_PRE_ENTER_GS_ACK* pMsg = (G2M_PRE_ENTER_GS_ACK*)pPacketBuffer;
    MS_USER* poUser = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    UINT32 dwGSSessionID = 0;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);
    dwGSSessionID = pGSSession->dwSessionID;

    poUser = CUserMgr::GetInstance().FindUserBySessionID(pMsg->dwClientSessionID);
    LOG_PROCESS_ERROR(poUser);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData->dwRoleID == pMsg->dwRoleID);

    if (pMsg->nErrCode == errSuccess)
    {
        if (poUser->eState == USER_STATE_LOGIN_PRE_ENTER_GS)
        {
            nRetCode = DoGTQuerySceneAck(pMsg->dwClientSessionID, pMsg->dwRoleID, dwGSSessionID, errSuccess, 0, 0);
            LOG_PROCESS_ERROR(nRetCode);
            poUser->eState = USER_STATE_LOGIN_GSREADY;
        }
        else if (poUser->eState == USER_STATE_JUMP_PRE_ENTER_GS)
        {
            nRetCode = DoGSLeaveGS(pOnlineRole->dwGSSessionID, pOnlineRole->dwCliSessionID, pOnlineRole->pRoleData->dwRoleID);
            LOG_PROCESS_ERROR(nRetCode);
            poUser->eState = USER_STATE_JUMP_EXIT_SRC;
        }
        else
        {
            LOG_PROCESS_ERROR(0);
        }

        // log again
        {
            INF("role session<%08X> pre entered GS %d, remains online role count %d",
                pOnlineRole->dwCliSessionID, dwGSSessionID, pGSSession->nOnlineRoleCount);
        }
    }
    else
    {
        LOG_CHECK_ERROR(pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nSceneTempID == pOnlineRole->nPreEnterSceneTempID);
        LOG_CHECK_ERROR(pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nSceneID == pOnlineRole->nPreEnterSceneID);

        nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(
            pOnlineRole, pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nSceneTempID, pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nSceneID, FALSE);
        LOG_PROCESS_ERROR(nRetCode);

        if (poUser->eState == USER_STATE_LOGIN_PRE_ENTER_GS)
        {
            nRetCode = _LoginQuerySceneProc(pOnlineRole, pMsg->dwRoleID);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else if (poUser->eState == USER_STATE_JUMP_PRE_ENTER_GS)
        {
            nRetCode = DoGSQuerySceneAck(pOnlineRole->dwGSSessionID, pOnlineRole->dwCliSessionID, errScenePreEnterFailed, pOnlineRole->pRoleData->dwRoleID,
                pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nSceneTempID,
                pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nSceneID,
                pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nX,
                pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nY,
                pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nZ,
                pOnlineRole->PosList[pOnlineRole->nPosIndex - 1].nFaceDirection);
            LOG_PROCESS_ERROR(nRetCode);

            pOnlineRole->dwTargetGSSessionID = 0;
            poUser->eState = USER_STATE_INGAME;
        }
        else
        {
            LOG_PROCESS_ERROR(FALSE);
        }

        // log again and rollback if need
        {
            LOG_PROCESS_ERROR(pGSSession->nOnlineRoleCount - 1 >= 0);
            --pGSSession->nOnlineRoleCount;
            INF("role session<%08X> failed to pre entere GS %d, remains online role count %d",
                pOnlineRole->dwCliSessionID, dwGSSessionID, pGSSession->nOnlineRoleCount);
        }
    }

    MS_SYS_LOG_SUCCESS(poUser);
    return;
Exit0:
    MS_SYS_LOG_ERROR(poUser);

    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnGSEnterGS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_ENTER_GS* pMsg = (G2M_ENTER_GS*)pPacketBuffer;
    MS_USER* poUser = NULL;
    SCENE_INFO* poSceneInfo = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    UINT32 dwGSSessionID = 0;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);
    dwGSSessionID = pGSSession->dwSessionID;

    // The user must exist; otherwise there is a bug
    poUser = CUserMgr::GetInstance().FindUserBySessionID(pMsg->dwClientSessionID);
    LOG_PROCESS_ERROR(poUser);
    PROCESS_ERROR(poUser->eState == USER_STATE_LOGIN_GSREADY || poUser->eState == USER_STATE_JUMP_GSREADY);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData->dwRoleID == pMsg->dwRoleID);

    poSceneInfo = CSceneMgr::GetInstance().FindScene(pOnlineRole->nPreEnterSceneTempID, pOnlineRole->nPreEnterSceneID);
    LOG_PROCESS_ERROR(poSceneInfo);

    LOG_CHECK_ERROR(pOnlineRole->dwGSSessionID == dwGSSessionID);// has changed when t2m_change_gs_ack
    nRetCode = COnlineRoleDataMgr::GetInstance().EnterScene(pOnlineRole, poSceneInfo);
    LOG_PROCESS_ERROR(nRetCode);

    strxcpy(pOnlineRole->m_szSignature, pMsg->szSignature, SIGNATURE_LEN);
    pOnlineRole->llMasterExp = pMsg->llMasterExp;
    pOnlineRole->dwMasterCD = pMsg->dwMasterCD;
    pOnlineRole->pRoleData->dwJoinGuildCD = pMsg->dwGuildCD;
    pOnlineRole->nGMLevel = pMsg->byGMLevel;
    pOnlineRole->pRoleData->nLastLoginTime = 0;     // Online state: set to 0. Allan Tao [9.6.2013]
    pOnlineRole->bTwitter = pMsg->byTwitter;
    pOnlineRole->dwBeDeterminedRoleID = 0;
    pOnlineRole->dwMasterTwitterCD = pMsg->dwMasterTwitterCD;

    if (poUser->eState == USER_STATE_LOGIN_GSREADY)
    {
        nRetCode = _DoLoginSyncInfo(pOnlineRole);  // Synchronize character team, guild, mail, friends, mentorship, Sabac, and statue information
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = _DoLoadOrSyncInfo(pOnlineRole);  // Synchronize special social information: guild mute time and mentorship
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = DoGSEnterGSAck(dwGSSessionID, pMsg->dwClientSessionID, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);
    poUser->eState = USER_STATE_INGAME;

    if (g_ServerConfig.MS.bFcmEnable && !poUser->bAdult)
        CFcmMgr::GetInstance().QueryFcmState(poUser->szUserName, poUser->dwClientIp);

    if (g_ServerConfig.MS.bTimeCardEnabled && !poUser->bUserWhiteList && pOnlineRole->pRoleData->byLevel > g_GWConfig.nFreeRoleLevel)
    {
        LOG_CHECK_ERROR(poUser->dwBeginTime == 0);
        if (pOnlineRole->pRoleData->byLevel > g_GWConfig.nFreeRoleLevel && poUser->nTimeLeft <= 0)
        {
            poUser->SetFreeMode(TRUE);
        }
        else
        {
            poUser->SetFreeMode(FALSE);
        }
    }

    //LOG_PROCESS_ERROR( pGSSession->nOnlineRoleCount + 1 > 0 );
    //++pGSSession->nOnlineRoleCount;// move to pre enter state when login and jump
    INF("OnGSEnterGS: role session<%08X> entered GS %d, remains online role count %d", pOnlineRole->dwCliSessionID, pGSSession->dwSessionID, pGSSession->nOnlineRoleCount);

    MS_SYS_LOG_SUCCESS(poUser);
    return;
Exit0:
    MS_SYS_LOG_ERROR(poUser);

    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnGSLeaveGSAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_LEAVE_GS_ACK* pMsg = (G2M_LEAVE_GS_ACK*)pPacketBuffer;
    MS_USER* poUser = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    SCENE_INFO* pSceneInfo = NULL;
    BOOL bNormalLeave = TRUE;

    SGSSession* poGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(poGSSession);

    // The user must exist; otherwise there is a bug
    poUser = CUserMgr::GetInstance().FindUserBySessionID(pMsg->dwClientSessionID);
    LOG_PROCESS_ERROR(poUser);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(poUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData->dwRoleID == pMsg->dwRoleID);

    if (poUser->nShutdownType == csdtTimeout || poUser->nShutdownType == csdtClient)
        bNormalLeave = FALSE;

    if (g_ServerConfig.MS.bTimeCardEnabled && !pOnlineRole->pUser->bUserWhiteList && pOnlineRole->pRoleData->byLevel > g_GWConfig.nFreeRoleLevel)
    {
        TimeBillingType tbt = etbtPlayerGoOffline;
        if (poUser->eState == USER_STATE_JUMP_EXIT_SRC)
            tbt = etbtChangeScene;

        LOG_CHECK_ERROR(poUser->dwBeginTime);
        if (poUser->dwBeginTime)
        {
            COABillMgr::GetInstance().DoSubTime(pOnlineRole, poUser->dwBeginTime, CManageServer::GetInstance().GetGameTime(),
                CManageServer::GetInstance().GetGameTime() - poUser->dwBeginTime, tbt);
            poUser->dwBeginTime = 0;
        }
    }

    switch (poUser->eState)
    {
        case USER_STATE_EXIT_GS_PREENTER:
        {
            nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole, pOnlineRole->nPreEnterSceneTempID, pOnlineRole->nPreEnterSceneID, bNormalLeave);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pOnlineRole);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = DoDBLeaveDB(pMsg->dwClientSessionID);
            LOG_PROCESS_ERROR(nRetCode);

            poUser->eState = USER_STATE_EXIT_DB;
        }
        break;
        case USER_STATE_EXIT_GS:
        {
            nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
                pOnlineRole->pRoleData->nSceneTempID, pOnlineRole->pRoleData->nSceneID, bNormalLeave);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pOnlineRole);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = DoDBLeaveDB(pMsg->dwClientSessionID);
            LOG_PROCESS_ERROR(nRetCode);

            poUser->eState = USER_STATE_EXIT_DB;
        }
        break;
        case USER_STATE_JUMP_EXIT_TGT:
        {
            LOG_CHECK_ERROR(poGSSession->dwSessionID == pOnlineRole->dwTargetGSSessionID);

            nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole, pOnlineRole->nPreEnterSceneTempID, pOnlineRole->nPreEnterSceneID, bNormalLeave);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pOnlineRole);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = DoDBLeaveDB(pMsg->dwClientSessionID);
            LOG_PROCESS_ERROR(nRetCode);

            poUser->eState = USER_STATE_EXIT_DB;
        }
        break;
        case USER_STATE_EXIT_GS_RETURN:
        {
            nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
                pOnlineRole->pRoleData->nSceneTempID, pOnlineRole->pRoleData->nSceneID, bNormalLeave);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = COnlineRoleDataMgr::GetInstance().DelOnlineRole(pOnlineRole);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = DoDBLeaveDB(pMsg->dwClientSessionID);
            LOG_PROCESS_ERROR(nRetCode);

            poUser->eState = USER_STATE_EXIT_DB_RETURN;
        }
        break;
        case USER_STATE_EXIT_2GS:
        {
            //exit src GS,change role scene
            if (poGSSession->dwSessionID == pOnlineRole->dwGSSessionID)
            {
                nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
                    pOnlineRole->pRoleData->nSceneTempID, pOnlineRole->pRoleData->nSceneID, bNormalLeave);
                LOG_PROCESS_ERROR(nRetCode);

                pOnlineRole->dwGSSessionID = 0;
                poUser->eState = USER_STATE_JUMP_EXIT_TGT;
            }
            else
            {
                LOG_CHECK_ERROR(poGSSession->dwSessionID == pOnlineRole->dwTargetGSSessionID);

                nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole, pOnlineRole->nPreEnterSceneTempID, pOnlineRole->nPreEnterSceneID, bNormalLeave);
                LOG_PROCESS_ERROR(nRetCode);

                pOnlineRole->nPosIndex = 0; // ???
                pOnlineRole->dwTargetGSSessionID = 0;
                poUser->eState = USER_STATE_EXIT_GS;
            }
        }
        break;
        case USER_STATE_JUMP_EXIT_SRC:
        {
            nRetCode = COnlineRoleDataMgr::GetInstance().LeaveScene(pOnlineRole,
                pOnlineRole->pRoleData->nSceneTempID, pOnlineRole->pRoleData->nSceneID, bNormalLeave);
            LOG_PROCESS_ERROR(nRetCode);

            pOnlineRole->dwGSSessionID = 0;
            poUser->eState = USER_STATE_JUMP_ENTER_TARGET;

            nRetCode = DoGTChangeGS(pOnlineRole->dwCliSessionID, pOnlineRole->dwTargetGSSessionID);
            LOG_PROCESS_ERROR(nRetCode);
        }
        break;
        case USER_STATE_AUTHED:
        case USER_STATE_QUEUEFINISHED:
        case USER_STATE_LOGIN_WAIT_CREATE_SCENE:
        case USER_STATE_LOGIN_PRE_ENTER_GS:
        case USER_STATE_LOGIN_GSREADY:
            //break;
        case USER_STATE_INGAME:

        case USER_STATE_WAIT_EXIT_DB:
        case USER_STATE_EXIT_DB:
        case USER_STATE_EXIT_DB_RETURN:

        case USER_STATE_JUMP_WAIT_CREATE_SCENE:
        case USER_STATE_JUMP_PRE_ENTER_GS:
        case USER_STATE_JUMP_GSREADY:
        case USER_STATE_JUMP_ENTER_TARGET:
            //{
            //    pSceneInfo = CSceneMgr::GetInstance().FindScene(pOnlineRole->stRoleData->nSceneTempID, pOnlineRole->stRoleData->nSceneID, 
            //        GET_GROUP_INDEX_FROM_SESSION_ID(pOnlineRole->dwCliSessionID));
            //    LOG_PROCESS_ERROR(pSceneInfo);

            //    nRetCode = _LeaveScene(pOnlineRole, pSceneInfo);
            //    LOG_PROCESS_ERROR(nRetCode);
            //}
            //break;
        default:
            INF("OnGSLeaveGSAck error user state: %d", poUser->eState);
            LOG_PROCESS_ERROR(FALSE);
            break;;
    }

    LOG_PROCESS_ERROR(poGSSession->nOnlineRoleCount - 1 >= 0);
    --poGSSession->nOnlineRoleCount;
    INF("session<%08X> leaved GS %d, remains online role count %d", pOnlineRole->dwCliSessionID, poGSSession->dwSessionID, poGSSession->nOnlineRoleCount);

    MS_SYS_LOG_SUCCESS(poUser);
    return;
Exit0:
    MS_SYS_LOG_ERROR(poUser);
    if (poUser && pOnlineRole)
    {
        WRN("user(%s, %d, %d) leave gs %d error. user current state",
            poUser->szUserName, pOnlineRole->dwCliSessionID, poUser->eState, pOnlineRole->dwGSSessionID);
    }
    else
    {
        WRN("user leave gs error. but poUser or pOnlineRole is null ");
    }

    KICK_USER(pMsg->dwClientSessionID, ktLogged);

    return;
}

void CMessageHandler::OnGSRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_REGISTER* pMsg = (G2M_REGISTER*)pPacketBuffer;
    UINT32 dwGSSessionID = 0;
    CTraverseTeamToGS stTraverseTeamToGS;
    INT32 nDataCount = (dwSize - sizeof(G2M_REGISTER)) / sizeof(UINT32);

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);
    LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    if (m_pCRCList == NULL)
    {
        m_pCRCList = new UINT32[nDataCount];
        LOG_PROCESS_ERROR(m_pCRCList);

        memcpy(m_pCRCList, pMsg->dwCRCList, nDataCount * sizeof(UINT32));
        m_nCRCDataCount = nDataCount;
    }
    else
    {
        LOG_PROCESS_ERROR(nDataCount == m_nCRCDataCount);
        for (INT32 nIndex = 0; nIndex < nDataCount; nIndex++)
        {
            if (m_pCRCList[nIndex] != pMsg->dwCRCList[nIndex])
                CRI("detected different crc value on GS %d", pGSSession->dwSessionID);
            LOG_PROCESS_ERROR(m_pCRCList[nIndex] == pMsg->dwCRCList[nIndex]);
        }
    }

    dwGSSessionID = pGSSession->dwSessionID;

    nRetCode = DoGSRegisterAck(dwGSSessionID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMsDropMgr::GetInstance().SendAllDropLimits(dwGSSessionID);
    LOG_PROCESS_ERROR(nRetCode);

    stTraverseTeamToGS.dwGSSessionID = dwGSSessionID;
    nRetCode = CTeamMgr::GetInstance().Traverse(stTraverseTeamToGS);
    LOG_PROCESS_ERROR(nRetCode);

    for (INT32 nServerGroup = 0; nServerGroup < MAX_SERVER_GROUP_COUNT; nServerGroup++)
    {
        nRetCode = DoGSSyncServerVariable(dwGSSessionID, nServerGroup);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = DoGSUpdateAverageLevel(dwGSSessionID, CRankListMgr::GetInstance().GetAverageLevel());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoServerInfo();
    LOG_PROCESS_ERROR(nRetCode);

    CRI("CMessageHandler::OnGSRegister %d successfully!", pGSSession->dwSessionID);
    return;
Exit0:
    if (pGSSession)
    {
        CRI("CMessageHandler::OnGSRegister failed! %d", pGSSession->dwSessionID);
    }
    else
    {
        CRI("CMessageHandler::OnGSRegister failed!");
    }
    return;
}

void CMessageHandler::OnGSReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    UINT32 dwGSSessionID = 0;
    INT32   nRetCode = 0;
    GS_INFO* pGSInfo = NULL;
    CGSInfoMgr& rGSInfoMgr = CGSInfoMgr::GetInstance();
    const SCENE_DATA* pSceneData = NULL;

    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);
    dwGSSessionID = pGSSession->dwSessionID;

    m_ServerInfo.GS[dwGSSessionID].byReady = true;

    pGSInfo = rGSInfoMgr.NewGSInfo(dwGSSessionID);
    LOG_CHECK_ERROR(pGSInfo);

    if (pGSInfo)
    {
        for (INT32 nIndex = 0; nIndex < pGSInfo->nInitSceneCount; nIndex++)
        {
            pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pGSInfo->nInitSceneList[nIndex].nSceneTempID);
            LOG_PROCESS_ERROR(pSceneData);

            DBG("OnGSReady GS %d(nInitMaxSceneRoleCount %d) init scene template %d MaxRoleCount %d",
                pGSInfo->dwInitGSSessionID, pGSInfo->nInitMaxSceneRoleCount, pSceneData->nSceneID, pSceneData->nMaxRoleCount);

            switch (pSceneData->nType)
            {
                case estStatic:
                case estLine:
                case estUnique:
                {
                    SCENE_INFO* poSceneInfo = CSceneMgr::GetInstance().CreateScene(
                        pGSInfo->nInitSceneList[nIndex].nSceneTempID, pGSInfo->nInitSceneList[nIndex].nServerGroupID, 0);
                    LOG_PROCESS_ERROR(poSceneInfo);

                    poSceneInfo->nServerGroupID = pGSInfo->nInitSceneList[nIndex].nServerGroupID;
                    poSceneInfo->dwGSSessionID = dwGSSessionID;
                    poSceneInfo->eState = ESCENESTATE_CREATING;
                    poSceneInfo->dwCreateTime = CManageServer::GetInstance().GetGameTime();
                    poSceneInfo->WaitUserList.empty();

                    nRetCode = DoGSCreateScene(dwGSSessionID, poSceneInfo->nServerGroupID, poSceneInfo->nSceneTempID, poSceneInfo->nSceneID);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
                case estDungeon:
                case estBattleField:
                case estArena:
                case estBirth:
                case estGuildDungeon:
                case estSoloDungeon:
                default:
                    LOG_PROCESS_ERROR(FALSE);
                    break;
            }
        }

    }
    // sync to gs
    nRetCode = CMsGuildMgr::GetInstance().SyncAllGuildDataToGS(dwGSSessionID);
    LOG_PROCESS_ERROR(nRetCode);

    INF("GS %d is ready", dwGSSessionID);

    _CheckAllReady();

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();
    if (pGSInfo)
        rGSInfoMgr.DelGSInfo(pGSInfo->dwInitGSSessionID);

    return;
}

void CMessageHandler::OnGSCloseAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR(pGSSession);

    INF("GS %d is closed", pGSSession->dwSessionID);

    return;
Exit0:
    pSocketSession->Close();
    return;
}

void CMessageHandler::OnGSBroadcastGroupClient(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pForwardMsg = NULL;
    G2M_BROADCAST_GROUP_CLIENT* pMsg = (G2M_BROADCAST_GROUP_CLIENT*)pPacketBuffer;

    pForwardMsg = m_pPacketAllocator->Alloc(dwSize - sizeof(G2M_BROADCAST_GROUP_CLIENT));
    LOG_PROCESS_ERROR(pForwardMsg);

    memcpy(pForwardMsg, pMsg->MessageData, dwSize - sizeof(G2M_BROADCAST_GROUP_CLIENT));

    nRetCode = _BroadcastGroupClient(pMsg->byServerGroup, pForwardMsg, dwSize - sizeof(G2M_BROADCAST_GROUP_CLIENT));
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (pForwardMsg)
        m_pPacketAllocator->Free(pForwardMsg);
    return;
}

void CMessageHandler::OnGSTeamSyncMember(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    UINT32 nRetCode = 0;
    UINT32 dwClientSessionID = 0;
    CTeam* poTeam = NULL;
    G2M_TEAM_SYNC_MEMBER* pMsg = (G2M_TEAM_SYNC_MEMBER*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* pstRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstRoleData);

    pstRoleData->pRoleData->byLevel = pMsg->nLevel;
    pstRoleData->nCurrentHp = pMsg->nCurHp;
    pstRoleData->nCurrentMp = pMsg->nCurMp;
    pstRoleData->nMaxHp = pMsg->nMaxHp;
    pstRoleData->nMaxMp = pMsg->nMaxMp;

    poTeam = CTeamMgr::GetInstance().Find(pstRoleData->pRoleData->dwTeamID);
    if (poTeam)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientTeamSyncMemberInfo(pstRoleData, pMsg->nX, pMsg->nY, pMsg->nZ, poTeam);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSChatMessage(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    INT32 nMesasgeSize = 0;
    INT32 nResult = errUnknownError;
    UINT32 dwSrcType = 0;
    UINT32 dwTarType = 0;
    MS_ONLINE_ROLE_DATA* poSrcRole = NULL;

    G2M_CHAT_MESSAGE* pMsg = (G2M_CHAT_MESSAGE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    nMesasgeSize = dwSize - sizeof(G2M_CHAT_MESSAGE);
    LOG_PROCESS_ERROR(nMesasgeSize <= MAX_CHAT_MESSAGE_LEN);
    LOG_PROCESS_ERROR(pMsg->byLocation > cltInvalid && pMsg->byLocation < cltTotal);

    dwSrcType = GetTypeFromID(pMsg->dwSourceID);
    dwTarType = GetTypeFromID(pMsg->dwTargetID);

    if (pMsg->dwSourceID && dwSrcType == otRole)
    {
        poSrcRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwSourceID);
        LOG_PROCESS_ERROR(poSrcRole);
        LOG_PROCESS_ERROR(poSrcRole->pRoleData);

        if (poSrcRole->pRoleData->dwForbidChatTime >= CManageServer::GetInstance().GetGameTime())
        {
            nRetCode = DoClientChatMessageToTarget(poSrcRole->dwCliSessionID, poSrcRole->pRoleData->szRoleName,
                poSrcRole->pRoleData->dwRoleID, pMsg->dwTargetID, pMsg->byLocation, pMsg->szMessage, poSrcRole->pRoleData->byGroup);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        else
        {
            nRetCode = DoClientChatMessage(poSrcRole->pRoleData->szRoleName, poSrcRole->pRoleData->dwRoleID,
                pMsg->dwTargetID, pMsg->byLocation, pMsg->szMessage, poSrcRole->pRoleData->byGroup, pMsg->bGM);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
    }
    else
    {
        if (dwTarType == otRole)
        {
            MS_ONLINE_ROLE_DATA* poTarget = NULL;
            poTarget = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTargetID);
            PROCESS_ERROR_RET_CODE(poTarget, errChatTargetNotFound);
            LOG_PROCESS_ERROR_RET_CODE(poTarget->pRoleData, errChatTargetNotFound);

            nRetCode = DoClientChatMessage(pMsg->szName, pMsg->dwSourceID, pMsg->dwTargetID, pMsg->byLocation,
                pMsg->szMessage, pMsg->byServerGroupID, pMsg->bGM);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
        // Announcement
        else
        {
            nRetCode = DoClientChatMessage(pMsg->szName, pMsg->dwSourceID, pMsg->dwTargetID, pMsg->byLocation,
                pMsg->szMessage, pMsg->byServerGroupID, pMsg->bGM);
            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errChatServiceUnavaible);
        }
    }

    return;
Exit0:
    if (dwSrcType != otRole) // npc call error
        return;

    return;
}

void CMessageHandler::OnGSLeaveTeam(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;

    MS_ONLINE_ROLE_DATA* pstLeaveRole = NULL;
    UINT32 dwLeaveClientSessionID = 0;
    UINT32 dwOldLeader;
    CTeam* poTeam = NULL;
    G2M_LEAVE_TEAM* pMsg = (G2M_LEAVE_TEAM*)pPacketBuffer;

    pstLeaveRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    PROCESS_ERROR(pstLeaveRole);

    // copy from OnClientTeamLeave, except notifying DL
    poTeam = CTeamMgr::GetInstance().Find(pstLeaveRole->pRoleData->dwTeamID);
    PROCESS_ERROR(poTeam);

    dwOldLeader = poTeam->GetLeader();
    if (dwOldLeader == pMsg->dwRoleID)
    {
        nRetCode = poTeam->SelectLeader(TRUE);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = poTeam->DelMember(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoClientTeamLeave(pstLeaveRole->dwCliSessionID, poTeam->GetTeamID());
    LOG_CHECK_ERROR(nRetCode);

    if (poTeam->GetMemberCount() == 0)
    {
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
        nRetCode = DoClientRemoveMember(pMsg->dwRoleID, poTeam);
        LOG_CHECK_ERROR(nRetCode);

        if (dwOldLeader != poTeam->GetLeader())
        {
            nRetCode = DoClientTeamChangeInfo(poTeam);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = DoGSTeamChangeInfo(poTeam);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    nRetCode = DoGSTeamRemoveMember(pMsg->dwRoleID, poTeam);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:

    return;
}

void CMessageHandler::OnGSClearTeamDungeon(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;

    MS_ONLINE_ROLE_DATA* pstRole = NULL;
    CTeam* poTeam = NULL;
    G2M_CLEAR_TEAM_DUNGEON* pMsg = (G2M_CLEAR_TEAM_DUNGEON*)pPacketBuffer;

    pstRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    PROCESS_ERROR(pstRole);

    poTeam = CTeamMgr::GetInstance().Find(pstRole->pRoleData->dwTeamID);
    PROCESS_ERROR(poTeam);
    PROCESS_ERROR(poTeam->GetDungeonTempID() == pstRole->pRoleData->nSceneTempID);
    PROCESS_ERROR(poTeam->GetDungeonID() == pstRole->pRoleData->nSceneID);

    poTeam->SetDungeonTempID(0);
    poTeam->SetDungeonID(0);
    return;
Exit0:

    return;
}

void CMessageHandler::OnGSPublishQuest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    MS_ONLINE_ROLE_DATA* pstOnlienRoleData = NULL;

    G2M_PUBLISH_QUEST* pMsg = (G2M_PUBLISH_QUEST*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pstOnlienRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstOnlienRoleData);

    for (INT32 i = 0; i < pMsg->nNum; i++)
    {
        MS_ONLINE_ROLE_DATA* pstRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentList[i]);
        if (pstRole)
        {
            nRetCode = DoClientPublishQuest(pstRole->dwCliSessionID, pstOnlienRoleData->pRoleData->dwRoleID, pMsg->nQuestID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSUpdateRoleInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;

    MS_ONLINE_ROLE_DATA* pstRole = NULL;
    G2M_UPDATE_ROLE_INFO* pMsg = (G2M_UPDATE_ROLE_INFO*)pPacketBuffer;

    pstRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstRole);

    pstRole->pRoleData->byLevel = pMsg->byLevel;
    pstRole->pRoleData->byVIPLevel = pMsg->byVIPLevel;

    if (g_ServerConfig.MS.bTimeCardEnabled && !pstRole->pUser->bUserWhiteList && pstRole->pRoleData->byLevel > g_GWConfig.nFreeRoleLevel && pstRole->pUser->dwBeginTime == 0)
    {
        if (pstRole->pUser->nTimeLeft > 0)
            pstRole->pUser->dwBeginTime = CManageServer::GetInstance().GetGameTime();
        else
            pstRole->pUser->SetFreeMode(TRUE);
    }

    ////// guild
    if (0 != pstRole->pRoleData->dwGuildID)
    {
        CMsGuild* poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstRole->pRoleData->dwGuildID);
        if (poMsGuild)
        {
            nRetCode = CMessageHandler::GetInstance().DoClientSyncSocialAttrNtf(poMsGuild,
                pstRole->pRoleData->dwRoleID, pstRole->pRoleData->byLevel, pstRole->pRoleData->nSceneTempID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    ////// master
    nRetCode = MasterLevelUp(*pstRole);

    return;
Exit0:

    return;
}

void CMessageHandler::OnGSGuildDrawSalary(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    INT32       nCurrentWeekTime = CManageServer::GetInstance().GetGameTime() / (g_GuildConfig.nDrawSalaryPeriod);
    G2M_GUILD_DRAW_SALARY* pMsg = (G2M_GUILD_DRAW_SALARY*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMsGuildMember = poMsGuild->GetMember(pOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    PROCESS_ERROR_RET_CODE(poMsGuildMember->GetSalary() != 0, errGuildNoSalary);
    PROCESS_ERROR_RET_CODE(poMsGuildMember->GetDrawSalaryTime() != nCurrentWeekTime, errGuildDrawSalaryAlready);

    nRetCode = poMsGuild->DrawSalary(*pOnlineRole);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildContribute(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    G2M_GUILD_CONTRIBUTE* pMsg = (G2M_GUILD_CONTRIBUTE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nRetCode = poMsGuild->Contribute(*pOnlineRole, pMsg->nType, pMsg->nMoney);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildContributeAck(pOnlineRole->dwCliSessionID, pMsg->nType, pMsg->nMoney);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildViewStorage(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_VIEW_STORAGE* pMsg = (G2M_GUILD_VIEW_STORAGE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMsGuildMember = poMsGuild->GetMember(pOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildNotMember);

    // check right
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    //LOG_PROCESS_ERROR_RET_CODE((poMsGuild->GetStorageRightByPosition(poMsGuildMember->GetPosition()) & (1 << pMsg->byStorageTag)) 
    //	|| (poMsGuild->GetStorageRightByPosition(poMsGuildMember->GetPosition()) & (1 << (pMsg->byStorageTag + 6))), errGuildNoRight);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildViewStorage(pOnlineRole->dwCliSessionID, poMsGuild, pMsg->byStorageTag);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildStoraveEvent(pOnlineRole->dwCliSessionID, poMsGuild);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildMsgError);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildPrePushItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_PRE_PUSH_ITEM* pMsg = (G2M_GUILD_PRE_PUSH_ITEM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    //nRetCode = poMsGuild->GetStorage().CheckCD();
    //LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildStorageCD);

    nRetCode = poMsGuild->PrePushItem(*pOnlineRole, pMsg->wSourceConSlot, pMsg->wSourceSlot, pMsg->byTargetConSlot, pMsg->byTargetSlot);
    PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildPushItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_PUSH_ITEM* pMsg = (G2M_GUILD_PUSH_ITEM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nRetCode = poMsGuild->PushItem(*pOnlineRole, pMsg->nResult, pMsg->byConSlot, pMsg->bySlot, pMsg->wItemSize, pMsg->buff);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildPrePopItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_PRE_POP_ITEM* pMsg = (G2M_GUILD_PRE_POP_ITEM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nRetCode = poMsGuild->PrePopItem(*pOnlineRole, pMsg->byConSlot, pMsg->bySlot, pMsg->wTargetConSlot, pMsg->wTargetSlot);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildPopItem(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_POP_ITEM* pMsg = (G2M_GUILD_POP_ITEM*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nRetCode = poMsGuild->PopItem(*pOnlineRole, pMsg->nResult, pMsg->byConSlot, pMsg->bySlot, pMsg->wConSlot, pMsg->wSlot);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildCreateBuilding(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    const GUILD_BUILDING_DATA* pTBData = NULL;

    G2M_GUILD_CREATE_BUILDING* pMsg = (G2M_GUILD_CREATE_BUILDING*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMsGuildMember = poMsGuild->GetMember(pOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMsGuildMember->GetPosition() && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_UPGRADE_BUILDING, errGuildNoRight);

    nRetCode = poMsGuild->GetWarCharacter();
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == GUILD_WAR_NONE, errGuildWarTime);
    //LOG_PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_MAIN), errGuildBuildingDead);

    nRetCode = poMsGuild->IsOneLevelBornBuilding(pMsg->nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildCannotCreate);

    nRetCode = poMsGuild->CreateBuilding(pMsg->dwRoleID, pMsg->qwNpcNameID, pMsg->nType);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildUpgradeBuilding(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    const GUILD_BUILDING_DATA* pTBData = NULL;

    G2M_GUILD_CREATE_BUILDING* pMsg = (G2M_GUILD_CREATE_BUILDING*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMsGuildMember = poMsGuild->GetMember(pOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMsGuildMember->GetPosition() && poMsGuildMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMsGuildMember->GetPosition()] & G_POS_RIGHT_UPGRADE_BUILDING, errGuildNoRight);

    nRetCode = poMsGuild->GetWarCharacter();
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == GUILD_WAR_NONE, errGuildWarTime);
    //LOG_PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_MAIN), errGuildBuildingDead);

    nRetCode = poMsGuild->BeginUpgradeBuilding(pMsg->dwRoleID, pMsg->qwNpcNameID, pMsg->nType);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildDelBuilding(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_GUILD_DEL_BUILDING* pMsg = (G2M_GUILD_DEL_BUILDING*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMsGuildMember = poMsGuild->GetMember(pOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuildMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_LEADER == poMsGuildMember->GetPosition() || GUILD_POSITION_SUBLEADER == poMsGuildMember->GetPosition(), errGuildNoRight);
    //LOG_PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_MAIN), errGuildBuildingDead);

    nRetCode = poMsGuild->IsOneLevelBornBuilding(pMsg->nType);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildCannotDel);

    nResult = poMsGuild->DelBuilding(pOnlineRole->pRoleData->dwRoleID, pMsg->qwNameID, pMsg->nType);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;
Exit0:
    if (pOnlineRole)
    {
        if (nResult == errUnknownError)
            KICK_USER(pOnlineRole->dwCliSessionID, ktLogged);
        else
            CMessageHandler::GetInstance().DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
    }

    return;
}

void CMessageHandler::OnGSGuildPreOpenDungeon(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    UINT32      dwLeaderID = 0;
    MS_G_B_DATA* pBaseBuilding = NULL;
    const GUILD_DUNGEON_DATA* pDungeonTemplateData = NULL;
    CMsGuildMember* poMember = NULL;

    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    G2M_GUILD_PRE_OPEN_DUNGEON* pMsg = (G2M_GUILD_PRE_OPEN_DUNGEON*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwOwnerID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    pDungeonTemplateData = GetGuildDungeonData(pMsg->dwTemplateID);
    LOG_PROCESS_ERROR_RET_CODE(pDungeonTemplateData, errGuildConfigError);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    poMember = poMsGuild->GetMember(pMsg->dwOwnerID);
    LOG_PROCESS_ERROR_RET_CODE(poMember, errGuildCannotFindMember);

    LOG_PROCESS_ERROR_RET_CODE(GUILD_POSITION_NONE < poMember->GetPosition() && poMember->GetPosition() < GUILD_POSITION_TOTAL, errGuildPositionError);
    LOG_PROCESS_ERROR_RET_CODE(g_GuildConfig.nInitGuildPositionRight[poMember->GetPosition()] & G_POS_RIGHT_OPEN_DUNGEON, errGuildNoRight);

    PROCESS_ERROR_RET_CODE(pMsg->llMoney >= pDungeonTemplateData->nConditionMoney, errNotEnoughMoney);
    if (pDungeonTemplateData->dwConditionBuildingID > GUILD_B_NONE)
    {
        LOG_PROCESS_ERROR_RET_CODE(pDungeonTemplateData->dwConditionBuildingID > GUILD_B_NONE
            && pDungeonTemplateData->dwConditionBuildingID < GUILD_B_FUNATIONAL_NUM, errGuildConfigError);
        pBaseBuilding = poMsGuild->GetFunBuildingDataByType(pDungeonTemplateData->dwConditionBuildingID);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding, errGuildBaseBuildingFailed);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding->nLevel >= pDungeonTemplateData->nConditionBuildingLevel, errGuildBaseBuildingFailed);
    }
    //PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_DUPLICATION), errGuildBuildingDead);

    nRetCode = poMsGuild->CheckSameDungeonType(pMsg->dwTemplateID);
    PROCESS_ERROR_RET_CODE(nRetCode, errGuildAlreadyOpenDungeon);

    nRetCode = DoGSGuildPreOpenDungeonAck(pOnlineRole->dwGSSessionID, pMsg->dwTemplateID, pMsg->dwOwnerID, pMsg->dwLeaderID, pDungeonTemplateData->nConditionMoney);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildOpenDungeonFailed);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildOpenDungeon(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    UINT32      dwLeaderID = 0;
    SCENE_INFO* pScene = 0;
    MS_G_B_DATA* pBaseBuilding = NULL;
    const GUILD_DUNGEON_DATA* pDungeonTemplateData = NULL;

    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    G2M_GUILD_OPEN_DUNGEON* pMsg = (G2M_GUILD_OPEN_DUNGEON*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwOwnerID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    pDungeonTemplateData = GetGuildDungeonData(pMsg->dwTemplateID);
    LOG_PROCESS_ERROR_RET_CODE(pDungeonTemplateData, errGuildConfigError);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    if (pDungeonTemplateData->dwConditionBuildingID > GUILD_B_NONE)
    {
        LOG_PROCESS_ERROR_RET_CODE(pDungeonTemplateData->dwConditionBuildingID > GUILD_B_NONE
            && pDungeonTemplateData->dwConditionBuildingID < GUILD_B_FUNATIONAL_NUM, errGuildConfigError);
        pBaseBuilding = poMsGuild->GetFunBuildingDataByType(pDungeonTemplateData->dwConditionBuildingID);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding, errGuildBaseBuildingFailed);
        LOG_PROCESS_ERROR_RET_CODE(pBaseBuilding->nLevel >= pDungeonTemplateData->nConditionBuildingLevel, errGuildBaseBuildingFailed);
    }
    //PROCESS_ERROR_RET_CODE(!poMsGuild->IsBuildingDisable(GUILD_B_DUPLICATION), errGuildBuildingDead);

    nRetCode = poMsGuild->CheckSameDungeonType(pMsg->dwTemplateID);
    PROCESS_ERROR_RET_CODE(nRetCode, errGuildAlreadyOpenDungeon);

    pScene = CSceneMgr::GetInstance().FindScene(pMsg->dwTemplateID, poMsGuild->GetID());
    LOG_PROCESS_ERROR_RET_CODE(!pScene, errGuildDungeonMapClosing);

    if (pMsg->dwLeaderID == 0)
        dwLeaderID = pMsg->dwOwnerID;
    else
        dwLeaderID = pMsg->dwLeaderID;

    nRetCode = poMsGuild->AddMoney(pDungeonTemplateData->nConditionMoney / 2, GUILD_CHEST_OPEN_DUNGEON);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAddMoneyFailed);

    nRetCode = poMsGuild->AddDungeon(pMsg->dwTemplateID, pMsg->dwOwnerID, dwLeaderID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildOpenDungeonFailed);

    nRetCode = CMessageHandler::GetInstance().DoClientGuildOpenDungeonAck(pOnlineRole->dwCliSessionID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAlreadyOpenDungeon);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildEnterDungeon(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;
    GUILD_DUNGEON* pGuildDungeon = NULL;

    G2M_GUILD_ENTER_DUNGEON* pMsg = (G2M_GUILD_ENTER_DUNGEON*)pPacketBuffer;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    pGuildDungeon = poMsGuild->FindDunGeon(pMsg->dwTemplateID);
    LOG_PROCESS_ERROR_RET_CODE(pGuildDungeon, errGuildCannotFindDungeon);

    //LOG_PROCESS_ERROR_RET_CODE(pGuildDungeon->dwLeaderID == pMsg->dwLeaderID, errGuildNotDungeonLeader);

    nRetCode = DoGSGuildEnterDungeon(pOnlineRole->dwGSSessionID, pMsg->dwTemplateID, pOnlineRole->pRoleData->dwRoleID, pGuildDungeon->dwLeaderID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildEnterDungeonFailed);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildCloseDungeonCommission(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    GUILD_DUNGEON* pGuildDungeon = NULL;
    UINT32      dwOwnerID = 0;
    const GUILD_DUNGEON_DATA* pDungeonTemplate = NULL;
    SCENE_INFO* poMsScene = NULL;
    MS_ROLE_DATA* pOwnerRole = NULL;
    BASE_LIST_NODE* pNode = NULL;
    INT32                   nSceneRoleNum = 0;
    INT32                   nAverageSalary = 0;
    MS_ONLINE_ROLE_DATA* pWorkerRole = NULL;
    CMsGuildMember* poMsGuildMember = NULL;
    INT32                   nGuildMoneyAddition = 0;
    INT32                   nCheckTotalMoney = 0;
    const MAIL_TEMPLATE_DATA* mail_data = NULL;

    G2M_GUILD_CLOSE_DUNGEON_COMMISSION* pMsg = (G2M_GUILD_CLOSE_DUNGEON_COMMISSION*)pPacketBuffer;

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwSceneID);
    LOG_PROCESS_ERROR(poMsGuild);

    pGuildDungeon = poMsGuild->FindDunGeon(pMsg->dwTemplateID);
    LOG_PROCESS_ERROR(pGuildDungeon);

    pDungeonTemplate = GetGuildDungeonData(pMsg->dwTemplateID);
    LOG_PROCESS_ERROR(pDungeonTemplate);

    dwOwnerID = pGuildDungeon->dwOwnerID;
    pOwnerRole = CRoleDataMgr::GetInstance().FindRole(dwOwnerID);
    LOG_PROCESS_ERROR(pOwnerRole);

    poMsScene = CSceneMgr::GetInstance().FindScene(pMsg->dwTemplateID, pMsg->dwSceneID);
    LOG_PROCESS_ERROR(poMsScene);

    nRetCode = poMsGuild->DelDungeon(pMsg->dwTemplateID);
    LOG_PROCESS_ERROR(nRetCode);

    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildDungeonAward);
    LOG_PROCESS_ERROR(mail_data);

    // send award mail to owner
    if (pMsg->nItemNum > 0)
    {
        for (INT32 i = 0; i < pMsg->nItemNum; ++i)
        {
            nRetCode = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(pOwnerRole, mail_data->m_szMailSubject, mail_data->m_szMailContent, pMsg->arrItemID[i], pMsg->arrItemCount[i]);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
    else
    {
        nRetCode = CMailMgr::GetInstance().SendSystemMail(pOwnerRole, mtiGuildDungeonNoAward);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientGuildDungeonAwardNtf(poMsScene, pOwnerRole->dwRoleID, pMsg->nItemNum, pMsg->arrItemID, pMsg->arrItemCount);
    LOG_PROCESS_ERROR(nRetCode);

    // send salary to everybody
    pNode = poMsScene->lstOnlineRole.Head().pNext;
    while (pNode && pNode != &poMsScene->lstOnlineRole.Rear())
    {
        pWorkerRole = (MS_ONLINE_ROLE_DATA*)pNode;
        if (!pWorkerRole->pRoleData)
            continue;
        if (pWorkerRole->pRoleData->dwGuildID != pOwnerRole->dwGuildID)
            continue;

        ++nSceneRoleNum;
        pNode = pNode->pNext;
    }

    nCheckTotalMoney = (double)pDungeonTemplate->nConditionMoney * g_GuildConfig.nGuildDungeonSalaryPercent / 100;
    if (nSceneRoleNum > 0)
        nAverageSalary = nCheckTotalMoney / nSceneRoleNum;

    if (nAverageSalary > 0)
    {
        pNode = poMsScene->lstOnlineRole.Head().pNext;
        while (pNode && pNode != &poMsScene->lstOnlineRole.Rear())
        {
            pWorkerRole = (MS_ONLINE_ROLE_DATA*)pNode;

            if (nCheckTotalMoney < nAverageSalary)
                break;
            if (!pWorkerRole->pRoleData)
                continue;
            if (pWorkerRole->pRoleData->dwGuildID != pOwnerRole->dwGuildID)
                continue;

            poMsGuildMember = poMsGuild->GetMember(pWorkerRole->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(poMsGuildMember);
            if (!poMsGuildMember)
                continue;
            nRetCode = poMsGuildMember->AddSalary(nAverageSalary);
            LOG_CHECK_ERROR(nRetCode);

            nCheckTotalMoney -= nAverageSalary;

            pNode = pNode->pNext;
        }
    }

    // add guild money
    nGuildMoneyAddition = pDungeonTemplate->nConditionMoney - pDungeonTemplate->nConditionMoney * g_GuildConfig.nGuildDungeonSalaryPercent / 100;
    LOG_CHECK_ERROR(nGuildMoneyAddition >= 0);
    if (nGuildMoneyAddition >= 0)
    {
        nRetCode = poMsGuild->AddMoney(nGuildMoneyAddition, GUILD_CHEST_COMPLETE_DUNGEON);
        LOG_CHECK_ERROR(nRetCode);
    }

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSGuildChangeRes(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nLuaRet = 0;
    CLuaScript* poScript = NULL;
    INT64       llTotalCount = 0;
    INT32       nChangeCount = 0;
    INT32       bAdd = FALSE;
    CMsGuild* poGuild = NULL;

    G2M_GUILD_CHANGE_RES* pMsg = (G2M_GUILD_CHANGE_RES*)pPacketBuffer;

    poScript = CScriptMgr::GetInstance().FindScript(GUILD_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("GuildChangeResource", "ddddddd:d",
        pMsg->dwNpcID, pMsg->dwGuildID, pMsg->nResourceType, pMsg->nChangeType, pMsg->nCount, pMsg->nReason, (INT32)pMsg->byCtxType, &nLuaRet);
    LOG_PROCESS_ERROR(nRetCode);
    LOG_PROCESS_ERROR(nLuaRet == errSuccess);

Exit0:
    return;
}

void CMessageHandler::OnGSGuildEndWar(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    G2M_GUILD_END_WAR* pMsg = (G2M_GUILD_END_WAR*)pPacketBuffer;

    nRetCode = CMsGuildMgr::GetInstance().WarEnd(pMsg->dwDefenderID, pMsg->nWarDefenderResult);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSGuildChangeBuildingState(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_CHANGE_BUILDING_STATE* pMsg = (G2M_GUILD_CHANGE_BUILDING_STATE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);

    nRetCode = poMsGuild->DisableBuilding(pMsg->qwNameID, pMsg->dwReviveTime);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoGSGuildSyncBuildingStateNtf(poMsGuild->GetID(), pMsg->qwNameID, pMsg->dwReviveTime);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoClientGuildSyncBuildingStateNtf(poMsGuild, pMsg->qwNameID, pMsg->dwReviveTime);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}


// TODO guild war end: consider moving this to Lua for designers during a future refactor. Allan Tao [11.11.2013]
//void CMessageHandler::OnGSGuildBattleScore(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
//{
//    // todo_tq: previous version of guild war settlement
//	INT32       nRetCode = 0;
//	INT32       nResult = errUnknownError;
//	MS_GUILD_WAR_DATA   *pWarData = NULL;
//	CMsGuild    *poIntruderGuild = NULL;
//	CMsGuild    *poDefenderGuild = NULL;
//	MS_ROLE_DATA        *pRoleData = NULL;
//	INT32       nAttackAchievementNum = 0;
//	GUILD_WAR_PERSONAL_ACHIEVEMENT  arrAttackAchievement[GUILD_MAX_MEMBER_COUNT] = {0};
//	INT32       nBeAttackedAchievementNum = 0;
//	GUILD_WAR_PERSONAL_ACHIEVEMENT  arrBeAttackedAchievement[GUILD_MAX_MEMBER_COUNT] = {0};
//	CMsGuildMember          *poMsGuildMember = NULL;
//
//	INT32       nWarResult = GUILD_WAR_RES_LOSE;
//	INT32       nTotalKill = 0;
//	INT32       nPlunderOfMoney = 0;
//	INT32       nPlunderOfWood = 0;
//	INT32       nPlunderOfHerb = 0;
//	INT32       nPlunderOfCrystal = 0;
//	INT32       nPlunderOfSilver = 0;
//    INT32       nExtraPlunderOfMoney = 0;
//	INT32       nTotalSalary = 0;
//    INT32       nTotalSalaryLimit = 0;
//    INT32       nTotalMoney = 0;
//
//    INT32       nLevelDifference = 0; 
//    INT32       nDecPercent = 0;
//
//	char        attack_mail_content[MAIL_CONTENT_LEN] = {0};
//	char        defence_mail_content[MAIL_CONTENT_LEN] = {0};
//    const MAIL_TEMPLATE_DATA* attack_mail_data = NULL;
//    const MAIL_TEMPLATE_DATA* defence_mail_data = NULL;
//
//	G2M_GUILD_BATTLE_SCORE   *pMsg = (G2M_GUILD_BATTLE_SCORE*)pPacketBuffer;
//
//	memset(arrAttackAchievement, 0, sizeof(arrAttackAchievement));
//	memset(arrBeAttackedAchievement, 0, sizeof(arrBeAttackedAchievement));
//
//	pWarData = CMsGuildMgr::GetInstance().FindBattleByDefend(pMsg->dwDefenderID);
//	LOG_PROCESS_ERROR(pWarData);
//
//	nWarResult = pWarData->nResult;
//
//	poIntruderGuild = CMsGuildMgr::GetInstance().FindGuildByID(pWarData->dwIntruderID);
//	LOG_PROCESS_ERROR(poIntruderGuild);
//
//	poDefenderGuild = CMsGuildMgr::GetInstance().FindGuildByID(pWarData->dwDefenderID);
//	LOG_PROCESS_ERROR(poDefenderGuild);
//
//	nRetCode = CMsGuildMgr::GetInstance().DelGuildBattle(pWarData->dwIntruderID, pWarData->dwDefenderID);
//	LOG_PROCESS_ERROR(nRetCode);
//	pWarData = NULL;
//
//	// Stat.
//	for (UINT32 i = 0; i < pMsg->dwRoleNum; ++i)
//	{
//		pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->arrKill[i].dwRoleID);
//		LOG_CHECK_ERROR(pRoleData);
//		if (!pRoleData)
//			continue;
//
//		if (pRoleData->dwGuildID == poIntruderGuild->GetID())
//		{
//			arrAttackAchievement[nAttackAchievementNum].dwRoleID = pMsg->arrKill[i].dwRoleID;
//			arrAttackAchievement[nAttackAchievementNum].nKillNum = pMsg->arrKill[i].nKillNum;
//			arrAttackAchievement[nAttackAchievementNum].nDeadCount = pMsg->arrKill[i].nDeadCount;
//			++nAttackAchievementNum;
//			nTotalKill += pMsg->arrKill[i].nKillNum;
//		}
//		else if (pRoleData->dwGuildID == poDefenderGuild->GetID())
//		{
//			arrBeAttackedAchievement[nBeAttackedAchievementNum].dwRoleID = pMsg->arrKill[i].dwRoleID;
//			arrBeAttackedAchievement[nBeAttackedAchievementNum].nKillNum = pMsg->arrKill[i].nKillNum;
//			arrBeAttackedAchievement[nBeAttackedAchievementNum].nDeadCount = pMsg->arrKill[i].nDeadCount;
//			++nBeAttackedAchievementNum;
//		}
//	}
//
//    // caculate 
//    nLevelDifference = poIntruderGuild->GetLevel() - poDefenderGuild->GetLevel();
//    if (nLevelDifference > 0)
//    {
//        nDecPercent = nLevelDifference * g_GuildConfig.nGuildWarLevelDecPercent;
//        nDecPercent = nDecPercent > g_GuildConfig.nGuildWarMaxDecPercent ? g_GuildConfig.nGuildWarMaxDecPercent : nDecPercent;
//    }
//
//	// deal result
//	if (GUILD_WAR_RES_WIN == nWarResult)
//	{
//		// deal resource
//		nPlunderOfMoney = (poDefenderGuild->GetMoney() * g_GuildConfig.nGuildWarMoneyPlunderPercent * pMsg->nMoneyPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfWood = (poDefenderGuild->GetWood() * g_GuildConfig.nGuildWarWoodPlunderPercent * pMsg->nWoodPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfHerb = (poDefenderGuild->GetHerb() * g_GuildConfig.nGuildWarHerbPlunderPercent * pMsg->nHerbPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfCrystal = (poDefenderGuild->GetCryStal() * g_GuildConfig.nGuildWarCrystalPlunderPercent * pMsg->nCrystalPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfSilver = (poDefenderGuild->GetSilver() * g_GuildConfig.nGuildWarSilverPlunderPercent * pMsg->nSilverPercent / 100) * (100 - nDecPercent) / 100;
//
//		LOG_PROCESS_ERROR(nPlunderOfMoney <= poDefenderGuild->GetMoney());
//		LOG_PROCESS_ERROR(nPlunderOfWood <= poDefenderGuild->GetWood());
//		LOG_PROCESS_ERROR(nPlunderOfHerb <= poDefenderGuild->GetHerb());
//		LOG_PROCESS_ERROR(nPlunderOfCrystal <= poDefenderGuild->GetCryStal());
//		LOG_PROCESS_ERROR(nPlunderOfSilver <= poDefenderGuild->GetSilver());
//
//        nRetCode = poDefenderGuild->SubMoney(nPlunderOfMoney, GUILD_CHEST_WAR_PLUNDER);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubWood(nPlunderOfWood);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubHerb(nPlunderOfHerb);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubCrystal(nPlunderOfCrystal);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubSilver(nPlunderOfSilver);
//		LOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = poIntruderGuild->AddMoney(nPlunderOfMoney, GUILD_CHEST_WAR_PLUNDER);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddWood(nPlunderOfWood);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddHerb(nPlunderOfHerb);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddCrystal(nPlunderOfCrystal);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddSilver(nPlunderOfSilver);
//        LOG_CHECK_ERROR(nRetCode);
//
//		// for win, extra plunder
//		nExtraPlunderOfMoney = poDefenderGuild->GetMoney() * g_GuildConfig.nGuildWarExtraPlunder / 100;
//        nRetCode = poIntruderGuild->AddMoney(nExtraPlunderOfMoney, GUILD_CHEST_WAR_PLUNDER_EXTRA);
//        LOG_CHECK_ERROR(nRetCode);
//        poDefenderGuild->SubMoney(nExtraPlunderOfMoney, GUILD_CHEST_WAR_PLUNDER_EXTRA);
//
//        nRetCode = poIntruderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_A_1, nPlunderOfMoney, 0, 0, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//        nRetCode = poIntruderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_A_2, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//        LOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = poDefenderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_D_1, nPlunderOfMoney, 0, 0, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//        nRetCode = poDefenderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_D_2, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//        LOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = poIntruderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_WIN, nExtraPlunderOfMoney, 0, 0, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//        nRetCode = poDefenderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_LOSE, nExtraPlunderOfMoney, 0, 0, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//
//        nTotalMoney = nExtraPlunderOfMoney + nPlunderOfMoney;
//
//
//		// salary
//		nTotalSalary = (UINT64)g_GuildConfig.nGuildDeclareWarMoney * g_GuildConfig.nGuildWarSalaryPercent / 100;
//        nTotalSalaryLimit = nTotalSalary;
//
//		for (INT32 i = 0; i < nAttackAchievementNum; ++i)
//		{
//			LOG_CHECK_ERROR(nTotalSalaryLimit > 0);
//			if (nTotalSalaryLimit <= 0)
//				break;
//            if (0 < nTotalKill)
//            {
//                arrAttackAchievement[i].nSalary = (long long)arrAttackAchievement[i].nKillNum * nTotalSalary / nTotalKill;
//            }
//            else
//            {
//                arrAttackAchievement[i].nSalary = 0;
//            }
//			
//			nTotalSalaryLimit -= arrAttackAchievement[i].nSalary;
//
//			poMsGuildMember = poIntruderGuild->GetMember(arrAttackAchievement[i].dwRoleID);
//			LOG_CHECK_ERROR(poMsGuildMember);
//			if (poMsGuildMember)
//			{
//				nRetCode = poMsGuildMember->AddSalary(arrAttackAchievement[i].nSalary);
//				LOG_CHECK_ERROR(nRetCode);
//			}
//		}
//
//		// send mail
//        attack_mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildWarAttackSucc);
//        defence_mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildWarDefenceFailed);
//        LOG_PROCESS_ERROR(attack_mail_data);
//        LOG_PROCESS_ERROR(defence_mail_data);
//
//		snprintf(attack_mail_content, MAIL_CONTENT_LEN, attack_mail_data->m_szMailContent, poIntruderGuild->GetName(), 
//			poDefenderGuild->GetName(), nTotalMoney, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//		snprintf(defence_mail_content, MAIL_CONTENT_LEN, defence_mail_data->m_szMailContent, poDefenderGuild->GetName(), 
//			poIntruderGuild->GetName(), nTotalMoney, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//
//		poMsGuildMember = poIntruderGuild->GetFirstGuildMember();
//		while (poMsGuildMember)
//		{
//			pRoleData = CRoleDataMgr::GetInstance().FindRole(poMsGuildMember->GetRoleID());
//			LOG_CHECK_ERROR(pRoleData);
//			if (pRoleData)
//			{
//				nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, attack_mail_data->m_szMailSubject, attack_mail_content);
//				LOG_CHECK_ERROR(nRetCode);
//			}
//
//			poMsGuildMember = poIntruderGuild->GetNextGuildMember();
//		}
//
//		poMsGuildMember = poDefenderGuild->GetFirstGuildMember();
//		while (poMsGuildMember)
//		{
//			pRoleData = CRoleDataMgr::GetInstance().FindRole(poMsGuildMember->GetRoleID());
//			LOG_CHECK_ERROR(pRoleData);
//			if (pRoleData)
//			{
//				nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, defence_mail_data->m_szMailSubject, defence_mail_content);
//				LOG_CHECK_ERROR(nRetCode);
//			}
//
//			poMsGuildMember = poDefenderGuild->GetNextGuildMember();
//		}
//
//	}
//	else if (GUILD_WAR_RES_LOSE == nWarResult)
//	{
//		// deal resource
//		nPlunderOfMoney = (poDefenderGuild->GetMoney() * g_GuildConfig.nGuildWarMoneyPlunderPercent * pMsg->nMoneyPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfWood = (poDefenderGuild->GetWood() * g_GuildConfig.nGuildWarWoodPlunderPercent * pMsg->nWoodPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfHerb = (poDefenderGuild->GetHerb() * g_GuildConfig.nGuildWarHerbPlunderPercent * pMsg->nHerbPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfCrystal = (poDefenderGuild->GetCryStal() * g_GuildConfig.nGuildWarCrystalPlunderPercent * pMsg->nCrystalPercent / 100) * (100 - nDecPercent) / 100;
//		nPlunderOfSilver = (poDefenderGuild->GetSilver() * g_GuildConfig.nGuildWarSilverPlunderPercent * pMsg->nSilverPercent / 100) * (100 - nDecPercent) / 100;
//
//		LOG_PROCESS_ERROR(nPlunderOfMoney <= poDefenderGuild->GetMoney());
//		LOG_PROCESS_ERROR(nPlunderOfWood <= poDefenderGuild->GetWood());
//		LOG_PROCESS_ERROR(nPlunderOfHerb <= poDefenderGuild->GetHerb());
//		LOG_PROCESS_ERROR(nPlunderOfCrystal <= poDefenderGuild->GetCryStal());
//		LOG_PROCESS_ERROR(nPlunderOfSilver <= poDefenderGuild->GetSilver());
//
//        nRetCode = poDefenderGuild->SubMoney(nPlunderOfMoney, GUILD_CHEST_WAR_PLUNDER);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubWood(nPlunderOfWood);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubHerb(nPlunderOfHerb);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubCrystal(nPlunderOfCrystal);
//		LOG_PROCESS_ERROR(nRetCode);
//		nRetCode = poDefenderGuild->SubSilver(nPlunderOfSilver);
//		LOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = poIntruderGuild->AddMoney(nPlunderOfMoney, GUILD_CHEST_WAR_PLUNDER);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddWood(nPlunderOfWood);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddHerb(nPlunderOfHerb);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddCrystal(nPlunderOfCrystal);
//        LOG_CHECK_ERROR(nRetCode);
//		nRetCode = poIntruderGuild->AddSilver(nPlunderOfSilver);
//        LOG_CHECK_ERROR(nRetCode);
//
//        nRetCode = poIntruderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_A_1, nPlunderOfMoney, 0, 0, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//        nRetCode = poIntruderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_A_2, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//        LOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = poDefenderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_D_1, nPlunderOfMoney, 0, 0, 0);
//        LOG_PROCESS_ERROR(nRetCode);
//        nRetCode = poDefenderGuild->AddGuildChestEvent(GUILD_CHEST_WAR_SACK_D_2, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//        LOG_PROCESS_ERROR(nRetCode);
//
//		// send mail
//        attack_mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildWarAttackFailed);
//        defence_mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiGuildWarDefenceSucc);
//        LOG_PROCESS_ERROR(attack_mail_data);
//        LOG_PROCESS_ERROR(defence_mail_data);
//
//		snprintf(attack_mail_content, MAIL_CONTENT_LEN, attack_mail_data->m_szMailContent, poIntruderGuild->GetName(), 
//			poDefenderGuild->GetName(), nPlunderOfMoney, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//		snprintf(defence_mail_content, MAIL_CONTENT_LEN, defence_mail_data->m_szMailContent, poDefenderGuild->GetName(), 
//			poIntruderGuild->GetName(), nPlunderOfMoney, nPlunderOfHerb, nPlunderOfWood, nPlunderOfCrystal, nPlunderOfSilver);
//
//		poMsGuildMember = poIntruderGuild->GetFirstGuildMember();
//		while (poMsGuildMember)
//		{
//			pRoleData = CRoleDataMgr::GetInstance().FindRole(poMsGuildMember->GetRoleID());
//			LOG_CHECK_ERROR(pRoleData);
//			if (pRoleData)
//			{
//				nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, attack_mail_data->m_szMailSubject, attack_mail_content); 
//				LOG_CHECK_ERROR(nRetCode);
//			}
//
//			poMsGuildMember = poIntruderGuild->GetNextGuildMember();
//		}
//
//		poMsGuildMember = poDefenderGuild->GetFirstGuildMember();
//		while (poMsGuildMember)
//		{
//			pRoleData = CRoleDataMgr::GetInstance().FindRole(poMsGuildMember->GetRoleID());
//			LOG_CHECK_ERROR(pRoleData);
//			if (pRoleData)
//			{
//				nRetCode = CMailMgr::GetInstance().SendSystemMail(pRoleData, defence_mail_data->m_szMailSubject, defence_mail_content);
//				LOG_CHECK_ERROR(nRetCode);
//			}
//
//			poMsGuildMember = poDefenderGuild->GetNextGuildMember();
//		}
//	}
//	else
//		LOG_PROCESS_ERROR(FALSE);
//
//	// broadcast guild
//	nRetCode = CMessageHandler::GetInstance().DoClientBattleAchievementNtf(poIntruderGuild, nAttackAchievementNum, arrAttackAchievement);
//	LOG_CHECK_ERROR(nRetCode);
//
//	nRetCode = CMessageHandler::GetInstance().DoClientBattleAchievementNtf(poDefenderGuild, nBeAttackedAchievementNum, arrBeAttackedAchievement);
//	LOG_CHECK_ERROR(nRetCode);
//
//	return;
//Exit0:
//	return ;
//}

void CMessageHandler::OnGSGuildAchievementReward(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;
    G2M_GUILD_ACHIEVEMENT_REWARD* pMsg = (G2M_GUILD_ACHIEVEMENT_REWARD*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* poRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(poRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);

    nRetCode = poMsGuild->CheckAchievement(pMsg->nID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGuildAchiNotComplete);

    nRetCode = DoGSGuildAchievementReward(poRole->dwGSSessionID, pMsg->dwRoleID, pMsg->nID);
    LOG_PROCESS_ERROR(nRetCode);

    return;

Exit0:
    nRetCode = DoErrorNotify(poRole->dwCliSessionID, nResult, 0, 0);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGSGuildLeaveGuild(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    INT32 nResult = errUnknownError;
    CMsGuild* poMsGuild = NULL;

    G2M_GUILD_LEAVE_GUILD* pMsg = (G2M_GUILD_LEAVE_GUILD*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* poRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(poRole->pRoleData);

    PROCESS_ERROR_RET_CODE(poRole->pRoleData->dwGuildID != 0, errGuildNoGuild);
    poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(poRole->pRoleData->dwGuildID);
    LOG_PROCESS_ERROR_RET_CODE(poMsGuild, errGuildNotFind);

    nResult = poMsGuild->LeaveGuild(*poRole->pRoleData);
    LOG_PROCESS_ERROR(errSuccess == nResult);

    return;

Exit0:
    nRetCode = DoErrorNotify(poRole->dwCliSessionID, nResult, 0, 0);
    LOG_CHECK_ERROR(nRetCode);
    return;
}

void CMessageHandler::OnGSEnrollSabac(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_ENROLL_SABAC* pMsg = (G2M_ENROLL_SABAC*)pPacketBuffer;

    CMsGuild* poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwGuildID);
    LOG_PROCESS_ERROR(poMsGuild);

    poMsGuild->EnrollSabac(pMsg->dwEnrollTime);

Exit0:
    return;
}

void CMessageHandler::OnGSOccupySabac(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_OCCUPY_SABAC* pMsg = (G2M_OCCUPY_SABAC*)pPacketBuffer;

    CMsGuild* poOldMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwOldGuildID);
    if (poOldMsGuild)
    {
        poOldMsGuild->OccupySabac(FALSE);
    }

    CMsGuild* poNewMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pMsg->dwNewGuildID);
    if (poNewMsGuild)
    {
        poNewMsGuild->OccupySabac(TRUE);
    }

    nRetCode = DoClientOccupySabac(0, pMsg->byServerGroup, pMsg->dwOldGuildID, pMsg->dwNewGuildID);
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = DoGSOccupySabac(pMsg->dwOldGuildID, pMsg->dwNewGuildID);
    LOG_CHECK_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnGSErectStatue(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_ERECT_STATUE* pMsg = (G2M_ERECT_STATUE*)pPacketBuffer;
    INT32 nVarValue = 0;
    MS_ROLE_DATA* pRoleData = NULL;

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pRoleData);

    CServerVar::GetInstance().SetVariable(pRoleData->byGroup, svtSabacWinner0, pMsg->dwRoleID);
    nVarValue = (pMsg->byFaceType << 24) | (pMsg->byHairType << 16) | (pMsg->byHairColor) | pMsg->bySkinColor;
    CServerVar::GetInstance().SetVariable(pRoleData->byGroup, svtSabacWinner1, nVarValue);
    nVarValue = (pMsg->byTitleID << 24) | (pMsg->byRefineFx << 16);
    CServerVar::GetInstance().SetVariable(pRoleData->byGroup, svtSabacWinner2, nVarValue);
    CServerVar::GetInstance().SetVariable(pRoleData->byGroup, svtSabacWinner3, pMsg->anAvatar[NES_MAIN_HAND]);
    CServerVar::GetInstance().SetVariable(pRoleData->byGroup, svtSabacWinner4, pMsg->anAvatar[NES_BODY]);
    CServerVar::GetInstance().SetVariable(pRoleData->byGroup, svtSabacWinner5, pMsg->anAvatar[NES_BACK]);

    nRetCode = DoClientErectStatue(pRoleData->byGroup, 0);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

// arena
void CMessageHandler::OnGSArenaSyncData(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_ARENA_SYNC_DATA* pMsg = (G2M_ARENA_SYNC_DATA*)pPacketBuffer;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CArenaMgr::GetInstance().UpdateSeasonInfo(pOnlineRole->pRoleData->byGroup, pMsg->dwRoleID, pMsg->byPointSeasonTimes, pMsg->nPointSeasonEndTime);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSArenaJoinSeason(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;
    G2M_ARENA_JOIN_SEASON* pMsg = (G2M_ARENA_JOIN_SEASON*)pPacketBuffer;

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = CArenaMgr::GetInstance().JoinArena(pstOnlineRole->pRoleData->byGroup, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSArenaMatch(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_ARENA_MATCH* pMsg = (G2M_ARENA_MATCH*)pPacketBuffer;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CArenaMgr::GetInstance().Match(pOnlineRole->pRoleData->byGroup, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSArenaWin(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSArenaDefeated(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_ARENA_DEFEATED* pMsg = (G2M_ARENA_DEFEATED*)pPacketBuffer;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CArenaMgr::GetInstance().UpdateSeasonInfo(pOnlineRole->pRoleData->byGroup, pMsg->dwRoleID, pMsg->byPointSeasonTimes, pMsg->nPointSeasonEndTime);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSArenaReward(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    UINT32      dwItemID = 0;
    INT32       nRank = 0;
    INT32       nMoney = 0;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_ARENA_REWARD* pMsg = (G2M_ARENA_REWARD*)pPacketBuffer;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    dwItemID = CArenaMgr::GetInstance().GetReward(pOnlineRole->pRoleData->byGroup);
    nRank = CArenaMgr::GetInstance().GetRank(pOnlineRole->pRoleData->byGroup, pOnlineRole->pRoleData->dwRoleID);
    nMoney = CArenaMgr::GetInstance().GetRewardMoney(pOnlineRole->pRoleData->byGroup, nRank);

    nRetCode = CMessageHandler::GetInstance().DoGSArenaReward(pOnlineRole->dwGSSessionID, pOnlineRole->pRoleData->dwRoleID, dwItemID, nMoney);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}


void CMessageHandler::OnGSAddKillList(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    MS_ROLE_DATA* pKillerRole = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineKiller = NULL;

    G2M_ADD_KILL_LIST* pMsg = (G2M_ADD_KILL_LIST*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    // 
    pOnlineKiller = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwKillerID);
    if (pOnlineKiller)
    {
        pOnlineKiller->m_KillList.insert(pMsg->dwRoleID);
        DoClientFriendInfoChange(pOnlineRole->dwCliSessionID, pOnlineKiller);
    }
    else
    {
        pKillerRole = CRoleDataMgr::GetInstance().FindRole(pMsg->dwKillerID);
        if (pKillerRole)
            DoClientFriendInfoChange(pOnlineRole->dwCliSessionID, pKillerRole);
    }

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGMLog(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    G2M_GM_LOG* pMsg = (G2M_GM_LOG*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    nRetCode = DoGMStrCmdLog(pMsg->dwTime, pstOnlineRole->pRoleData->byGroup,
        pstOnlineRole->pUser->dwClientIp, pMsg->dwRoleID,
        pstOnlineRole->pRoleData->szRoleName, pMsg->szGMCmd, pMsg->nResult);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSMailCostCheck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_MAIL_COST_CHECK* pMsg = (G2M_MAIL_COST_CHECK*)pPacketBuffer;

    CMailMgr::GetInstance().OnMailCostCheck(pMsg->nResult, pMsg->dwSenderRoleID, pMsg->dwReceiverRoleID, pMsg->szMailSubject, pMsg->szMailContent);
}

void CMessageHandler::OnGSSendSystemMail(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_SEND_SYSTEM_MAIL* pMsg = (G2M_SEND_SYSTEM_MAIL*)pPacketBuffer;

    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwReceiverRoleID);
    LOG_PROCESS_ERROR(role);

    CMailMgr::GetInstance().SendSystemMail(role, pMsg->szMailSubject, pMsg->szMailContent);

Exit0:
    return;
}

void CMessageHandler::OnGSSendSystemMailWithSimpleAttach(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_SEND_SYSTEM_MAIL_WITH_SIMPLE_ATTACH* pMsg = (G2M_SEND_SYSTEM_MAIL_WITH_SIMPLE_ATTACH*)pPacketBuffer;

    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwReceiverRoleID);
    LOG_PROCESS_ERROR(role);

    CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(role, pMsg->szMailSubject, pMsg->szMailContent, pMsg->nItemID, pMsg->nItemCount);

Exit0:
    return;
}

void CMessageHandler::OnGSSendSystemMailWithBufferAttach(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_SEND_SYSTEM_MAIL_WITH_BUFFER_ATTACH* pMsg = (G2M_SEND_SYSTEM_MAIL_WITH_BUFFER_ATTACH*)pPacketBuffer;

    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwReceiverRoleID);
    LOG_PROCESS_ERROR(role);

    CMailMgr::GetInstance().SendSystemMailWithBufferAttach(role, pMsg->szMailSubject, pMsg->szMailContent, pMsg->ItemBuffer, pMsg->byBufferLen);

Exit0:
    return;
}

void CMessageHandler::OnGSMailGetAttach(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_MAIL_GET_ATTACH* pMsg = (G2M_MAIL_GET_ATTACH*)pPacketBuffer;

    CMailMgr::GetInstance().OnMailGetAttach(pMsg->nResult, pMsg->dwMailID);
}

void CMessageHandler::OnGSCallLuaFunction(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    CLuaScript* poLuaScript = NULL;

    G2M_CALL_LUA_FUNCTION* pMsg = (G2M_CALL_LUA_FUNCTION*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    poLuaScript = CScriptMgr::GetInstance().FindScript(pMsg->chFileName);
    LOG_PROCESS_ERROR(poLuaScript);

    nRetCode = poLuaScript->CallFunction(pMsg->chFuncName, "ddd", pMsg->nValue1, pMsg->nValue2, pMsg->nValue3);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:

    return;
}

void CMessageHandler::OnGSSubmitTeacherAppAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nMessageSize = 0;
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineStudent = NULL;
    G2M_MST_SUBMIT_TEACHER_APP_ACK* pMsg = (G2M_MST_SUBMIT_TEACHER_APP_ACK*)pPacketBuffer;

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineStudent = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    PROCESS_ERROR_RET_CODE(pOnlineStudent, errMasterStudentNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->pUser->eState == USER_STATE_INGAME, errMasterStudentNotOnline);
    LOG_PROCESS_ERROR(pOnlineStudent->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    PROCESS_ERROR_RET_CODE(pOnlineStudent->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pOnlineStudent->pRoleData->byLevel < GRADUATE_LEVEL, errMasterStudHighLevel);
    PROCESS_ERROR_RET_CODE(pOnlineStudent->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pOnlineStudent->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pstOnlineRole);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->byMasterSendAppNum < MAX_MASTER_SEND_APP_NUM, errMasterStudAppFull);
    PROCESS_ERROR_RET_CODE(pOnlineStudent->byMasterRecvAppNum < MAX_MASTER_RECV_APP_NUM, errMasterTargetStudAppFull);

    for (INT32 nIndex = 0; nIndex < pOnlineStudent->byMasterRecvAppNum; ++nIndex)
        PROCESS_ERROR_RET_CODE(pOnlineStudent->arrMasterRecvApp[nIndex] != pstOnlineRole->pRoleData->dwRoleID, errMasterAlreadyApply);

    pstOnlineRole->arrMasterSendApp[pstOnlineRole->byMasterSendAppNum].dwRoleID = pMsg->dwStudentID;
    pstOnlineRole->arrMasterSendApp[pstOnlineRole->byMasterSendAppNum].nWashoutTime = CManageServer::GetInstance().GetGameTime();
    ++pstOnlineRole->byMasterSendAppNum;

    pOnlineStudent->arrMasterRecvApp[pOnlineStudent->byMasterRecvAppNum++] = pstOnlineRole->pRoleData->dwRoleID;

    // to own
    nRetCode = CMessageHandler::GetInstance().DoClientMstTeacherSubmitAppAck(pstOnlineRole->dwCliSessionID,
        pOnlineStudent->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    // to student
    nRetCode = CMessageHandler::GetInstance().DoClientMstTeacherSubmitAppNtf(pOnlineStudent->dwCliSessionID,
        pMsg->bySourceType, pstOnlineRole->pRoleData->dwRoleID, pstOnlineRole->pRoleData->byLevel, pstOnlineRole->llMasterExp);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnGSAgreeTeacherAppAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineTeacher = NULL;
    G2M_MST_AGREE_TEACHER_APP_ACK* pMsg = (G2M_MST_AGREE_TEACHER_APP_ACK*)pPacketBuffer;

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineTeacher = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    PROCESS_ERROR_RET_CODE(pOnlineTeacher, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineTeacher->pUser->eState == USER_STATE_INGAME, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR(pOnlineTeacher->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->byGroup == pOnlineTeacher->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    PROCESS_ERROR_RET_CODE(pstOnlineRole->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pstOnlineRole->pRoleData->byLevel < GRADUATE_LEVEL, errMasterStudHighLevel);
    LOG_PROCESS_ERROR_RET_CODE(pstOnlineRole->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pstOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineTeacher->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineTeacher);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);


    nRetCode = MasterCancelRecvApp(*pstOnlineRole, pOnlineTeacher->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(nRetCode, errMasterNoTeacherApp);            // todo_tq: logging was removed here because canceling the application does not notify the other party

    nRetCode = MasterCancelSendApp(*pOnlineTeacher, pstOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(nRetCode, errMasterNoTeacherApp);            // todo_tq: logging was removed here because canceling the application does not notify the other party

    nRetCode = CMasterMgr::GetInstance().CreateMasterRelation(*pOnlineTeacher, *pstOnlineRole);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineTeacher);
    if (nRetCode)
    {
        nRetCode = MasterTeacherCancelAllApp(*pOnlineTeacher);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientMstAgreeTeacherAppAck(pstOnlineRole->dwCliSessionID, pOnlineTeacher->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnGSSubmitStudAppAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nMessageSize = 0;
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineMaster = NULL;
    MS_ONLINE_ROLE_DATA* pstOnlineRole = NULL;
    G2M_MST_SUBMIT_STUD_APP_ACK* pMsg = (G2M_MST_SUBMIT_STUD_APP_ACK*)pPacketBuffer;

    pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    LOG_PROCESS_ERROR(pstOnlineRole);           // student
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData);

    pOnlineMaster = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    PROCESS_ERROR_RET_CODE(pOnlineMaster, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineMaster->pUser->eState == USER_STATE_INGAME, errMasterTeacherNotOnline);
    LOG_PROCESS_ERROR(pOnlineMaster->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineMaster->pRoleData->byGroup == pstOnlineRole->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    LOG_PROCESS_ERROR(pstOnlineRole->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pstOnlineRole->pRoleData->byLevel < GRADUATE_LEVEL);
    PROCESS_ERROR_RET_CODE(pstOnlineRole->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pstOnlineRole->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineMaster->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(nRetCode, errMasterNotMaster);                               // tq_log

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineMaster);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    PROCESS_ERROR_RET_CODE(pstOnlineRole->byMasterSendAppNum < MAX_MASTER_SEND_APP_NUM, errMasterStudAppFull);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineMaster->byMasterRecvAppNum < MAX_MASTER_RECV_APP_NUM, errMasterTargetStudAppFull);

    for (INT32 nIndex = 0; nIndex < pOnlineMaster->byMasterRecvAppNum; ++nIndex)
        PROCESS_ERROR_RET_CODE(pOnlineMaster->arrMasterRecvApp[nIndex] != pstOnlineRole->pRoleData->dwRoleID, errMasterAlreadyApply);

    pstOnlineRole->arrMasterSendApp[pstOnlineRole->byMasterSendAppNum].dwRoleID = pMsg->dwTeacherID;
    pstOnlineRole->arrMasterSendApp[pstOnlineRole->byMasterSendAppNum].nWashoutTime = CManageServer::GetInstance().GetGameTime();
    ++pstOnlineRole->byMasterSendAppNum;

    pOnlineMaster->arrMasterRecvApp[pOnlineMaster->byMasterRecvAppNum++] = pstOnlineRole->pRoleData->dwRoleID;

    // to own
    nRetCode = CMessageHandler::GetInstance().DoClientMstStudSubmitAppAck(pstOnlineRole->dwCliSessionID, pOnlineMaster->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    // to master
    nRetCode = CMessageHandler::GetInstance().DoClientMstStudSubmitAppNtf(pOnlineMaster->dwCliSessionID, pstOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pstOnlineRole->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnGSAgreeStudAppAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRoleData = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineStudent = NULL;

    M2G_MST_AGREE_STUD_APP* pMsg = (M2G_MST_AGREE_STUD_APP*)pPacketBuffer;

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwTeacherID);
    LOG_PROCESS_ERROR(pOnlineRoleData);
    LOG_PROCESS_ERROR(pOnlineRoleData->pRoleData);

    pOnlineStudent = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwStudentID);
    PROCESS_ERROR_RET_CODE(pOnlineStudent, errMasterStudNotOnline);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->pUser->eState == USER_STATE_INGAME, errMasterStudNotOnline);
    LOG_PROCESS_ERROR(pOnlineStudent->pRoleData);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineRoleData->pRoleData->byGroup == pOnlineStudent->pRoleData->byGroup, errMasterRoleNotExist);

    // pre condition
    PROCESS_ERROR_RET_CODE(pOnlineStudent->pRoleData->byLevel >= STUD_LEVEL_LOWER_LIMIT && pOnlineStudent->pRoleData->byLevel < GRADUATE_LEVEL, errMasterStudHighLevel);
    LOG_PROCESS_ERROR_RET_CODE(pOnlineStudent->dwMasterCD < CManageServer::GetInstance().GetGameTime(), errMasterApplayMstCD);
    nRetCode = CMasterMgr::GetInstance().IsStudent(pOnlineStudent->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineRoleData->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMasterStudhasMasterAlready);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineRoleData);
    PROCESS_ERROR_RET_CODE(!nRetCode, errMasterStudFull);

    nRetCode = MasterCancelRecvApp(*pOnlineRoleData, pOnlineStudent->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(nRetCode, errMasterNoStudApp);

    nRetCode = MasterCancelSendApp(*pOnlineStudent, pOnlineRoleData->pRoleData->dwRoleID);
    PROCESS_ERROR_RET_CODE(nRetCode, errMasterNoStudApp);

    nRetCode = CMasterMgr::GetInstance().CreateMasterRelation(*pOnlineRoleData, *pOnlineStudent);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode == errSuccess, nRetCode);

    nRetCode = CMasterMgr::GetInstance().IsStudentFull(*pOnlineRoleData);
    if (nRetCode)
    {
        nRetCode = MasterTeacherCancelAllApp(*pOnlineRoleData);
        LOG_CHECK_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoClientMstAgreeStudAppAck(pOnlineRoleData->dwCliSessionID, pMsg->dwStudentID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    if (nResult == errUnknownError)
        KICK_USER(pOnlineRoleData->dwCliSessionID, ktLogged);
    else
        CMessageHandler::GetInstance().DoErrorNotify(pOnlineRoleData->dwCliSessionID, nResult, 0, 0);
    return;
}

void CMessageHandler::OnGSSyncMasterExp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRoleData = NULL;

    G2M_SYNC_MASTER_EXP* pMsg = (G2M_SYNC_MASTER_EXP*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRoleData);

    pOnlineRoleData->llMasterExp = pMsg->llMasterExp;

    nRetCode = CMasterMgr::GetInstance().AjustRankingsOnTwitter(*pOnlineRoleData);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

BOOL SendMasterGift(UINT32 dwRoleID1, UINT32 dwRoleID2, UINT32 dwMailID1, UINT32 dwMailID2, const char* pcszText)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ROLE_DATA* pRole1 = NULL;
    MS_ROLE_DATA* pRole2 = NULL;
    const MAIL_TEMPLATE_DATA* mail_data2 = NULL;

    LOG_PROCESS_ERROR(pcszText);

    pRole1 = CRoleDataMgr::GetInstance().FindRole(dwRoleID1);
    LOG_PROCESS_ERROR(pRole1);

    pRole2 = CRoleDataMgr::GetInstance().FindRole(dwRoleID2);
    LOG_PROCESS_ERROR(pRole2);

    mail_data2 = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(dwMailID2);
    LOG_PROCESS_ERROR(mail_data2);

    if (dwMailID1 != 0)
    {
        nRetCode = CMailMgr::GetInstance().SendSystemMail(pRole1, dwMailID1);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(pRole2, mail_data2->m_szMailSubject, pcszText, mail_data2->m_nItemID, mail_data2->m_nItemCount);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

void CMessageHandler::OnGSMasterCZ(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRoleData = NULL;
    MS_ROLE_DATA* pTargetRole = NULL;
    MS_TEACHER_DATA* pTeacherData = NULL;
    MS_STUDENT_DATA* pSelfStudentData = NULL;
    MS_STUDENT_DATA* pTargetStudentData = NULL;
    INT32                   nLevel = 0;
    const MS_MASTER_GIFT_TEMPLATE* pTemplate = NULL;

    G2M_MASTER_CZ* pMsg = (G2M_MASTER_CZ*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);
    LOG_PROCESS_ERROR(pMsg->szText);
    LOG_PROCESS_ERROR(pMsg->byCZType > MASTER_CONG_BEGIN && pMsg->byCZType < MASTER_CONG_END);

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRoleData);
    LOG_PROCESS_ERROR(pOnlineRoleData->pRoleData);

    pTargetRole = CRoleDataMgr::GetInstance().FindRole(pMsg->dwTargetID);
    LOG_PROCESS_ERROR(pTargetRole);

    LOG_PROCESS_ERROR(pMsg->byCZLevel < GRADUATE_LEVEL);
    LOG_PROCESS_ERROR(pMsg->byCZLevel <= pTargetRole->byLevel);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineRoleData->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errMstCannotCZ);

    nLevel = CMasterMgr::GetInstance().GetCZ(pMsg->dwTargetID, pMsg->dwRoleID);
    LOG_PROCESS_ERROR(nLevel < pMsg->byCZLevel);

    pTemplate = GetMasterGiftTemplate(pMsg->byCZLevel);
    LOG_PROCESS_ERROR_RET_CODE(pTemplate, errMasterNotCZLevel);

    // teacher to student
    pTeacherData = CMasterMgr::GetInstance().GetTeacherData(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pTeacherData);
    pTargetStudentData = CMasterMgr::GetInstance().GetStudentData(pMsg->dwTargetID);
    LOG_PROCESS_ERROR(pTargetStudentData);

    INF("Master cz: role[%u] target[%u] Level[%d]", pMsg->dwRoleID, pMsg->dwTargetID, nLevel);

    nRetCode = CMasterMgr::GetInstance().UpdateCZ(pMsg->dwTargetID, *pOnlineRoleData, pMsg->byCZLevel);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = SendMasterGift(pMsg->dwRoleID, pMsg->dwTargetID,
        pTemplate->m_arrCZT2STeacherMailID[pMsg->byCZType - 1], pTemplate->m_arrCZT2SStudentMailID[pMsg->byCZType - 1], pMsg->szText);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoClientMstCZAck(pOnlineRoleData->dwCliSessionID, pMsg->dwTargetID);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSTryToFindTeacher(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    G2M_TRY_TO_FIND_TEACHER* pMsg = (G2M_TRY_TO_FIND_TEACHER*)pPacketBuffer;

    PushRoleInfoToTeacher(pMsg->dwRoleID);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSMasterTwitter(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32       nRetCode = 0;
    INT32       nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_MASTER_TWITTER* pMsg = (G2M_MASTER_TWITTER*)pPacketBuffer;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CMasterMgr::GetInstance().IsMaster(pOnlineRole->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(nRetCode);
    //LOG_PROCESS_ERROR_RET_CODE(pOnlineRole->dwMasterTwitterCD < CManageServer::GetInstance().GetGameTime(), errMasterOpFast);

    pOnlineRole->bTwitter = pMsg->bTwitter;

    if (pMsg->bTwitter == TRUE)
    {
        nRetCode = CMasterMgr::GetInstance().JoinTwitter(*pOnlineRole);
        LOG_CHECK_ERROR(nRetCode);
    }
    else
    {
        nRetCode = CMasterMgr::GetInstance().QuitTwitter(*pOnlineRole);
        //LOG_CHECK_ERROR(nRetCode);
        //pOnlineRole->dwMasterTwitterCD = CManageServer::GetInstance().GetGameTime() + g_GWConfig.nMasterTwitterCD;
    }

    nRetCode = CMessageHandler::GetInstance().DoClinetMstTwitterAck(pOnlineRole->dwCliSessionID, pOnlineRole->bTwitter);
    LOG_CHECK_ERROR(nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSGuildCreate(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    G2M_GUILD_CREATE* pMsg = (G2M_GUILD_CREATE*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CMessageHandler::GetInstance().DoNSCreateGuild(pMsg->dwRoleID, pMsg->szGuildName, pMsg->szAnnounce, TRUE);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSPreCreateGuildAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32   nRetCode = 0;
    INT32   nResult = errUnknownError;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    G2M_PRE_CREATE_GUILD_ACK* pMsg = (G2M_PRE_CREATE_GUILD_ACK*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    LOG_PROCESS_ERROR_RET_CODE(pMsg->nResult == errSuccess, pMsg->nResult);

    nRetCode = CMsGuildMgr::GetInstance().CreateGuild(*pOnlineRole, pMsg->szName);
    LOG_PROCESS_ERROR_RET_CODE(errSuccess == nRetCode, nRetCode);

    return;
Exit0:
    if (nResult != errUnknownError)
    {
        nRetCode = DoErrorNotify(pOnlineRole->dwCliSessionID, nResult, 0, 0);
        LOG_CHECK_ERROR(nResult);
    }
    return;
}

void CMessageHandler::OnGSDropLimit(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_DROP_LIMIT* pMsg = (G2M_DROP_LIMIT*)pPacketBuffer;
    LOG_PROCESS_ERROR(pMsg);

    CMsDropMgr::GetInstance().OnDrop(pMsg->nItemID, pMsg->nDropCount);

Exit0:
    return;
}

void CMessageHandler::OnGSAuctionSell(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_AUCTION_SELL* pMsg = (G2M_AUCTION_SELL*)pPacketBuffer;

    ItemInfo iteminfo;
    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(role->eAuctionQueryStatus == MS_ROLE_DATA::eqsGsAuctionSell);
    role->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;

    if (pOnlineRole)
        LOG_PROCESS_ERROR(pOnlineRole->pRoleData == role);

    if (pMsg->nResult == errSuccess)
    {
        UINT32 itemlen = iteminfo.Load(pMsg->acItem);

        INT32 nRetCode = CAuctionMgr::GetInstance().AddItem(role, iteminfo, pMsg->byTimeType, pMsg->byOpen, pMsg->llPrice, pMsg->llBuyoutPrice, pMsg->llDepositePrice);
        if (nRetCode != errSuccess)
        {
            CMailMgr::GetInstance().SendSystemMailWithBufferAttach(role, "Auction System Error", "", pMsg->acItem, itemlen);

            if (pOnlineRole)
                DoErrorNotify(pOnlineRole->dwCliSessionID, nRetCode, 0, 0);
        }
    }
    else
    {
        if (pOnlineRole)
            DoErrorNotify(pOnlineRole->dwCliSessionID, pMsg->nResult, 0, 0);
    }

Exit0:
    return;
}

void CMessageHandler::OnGSAuctionBuy(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    // Reaching this point means the debit succeeded
    G2M_AUCTION_BUY* pMsg = (G2M_AUCTION_BUY*)pPacketBuffer;

    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(role->eAuctionQueryStatus == MS_ROLE_DATA::eqsGsAuctionBuy);
    role->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;

    if (pOnlineRole)
        LOG_PROCESS_ERROR(pOnlineRole->pRoleData == role);

    if (pMsg->nResult == errSuccess)
    {
        INT32 nRetCode = CAuctionMgr::GetInstance().BidItem(role, pMsg->dwAuctionID, pMsg->llPrice);
        LOG_CHECK_ERROR(nRetCode == errSuccess);

        if (nRetCode != errSuccess)
        {
            CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(role, "Auction System Error", "", MONEYTYPE_02, pMsg->llPrice);

            if (pOnlineRole)
                DoErrorNotify(pOnlineRole->dwCliSessionID, nRetCode, 0, 0);
        }
    }
    else
    {
        if (pOnlineRole)
            DoErrorNotify(pOnlineRole->dwCliSessionID, pMsg->nResult, 0, 0);
    }

Exit0:
    return;
}

void CMessageHandler::OnGSAuctionPay(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    MS_ONLINE_ROLE_DATA* pOnlineRole{};
    // Reaching the balance-payment step means the debit succeeded
    Auction* auction = NULL;
    G2M_AUCTION_PAY* pMsg = (G2M_AUCTION_PAY*)pPacketBuffer;

    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(role);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    if (pOnlineRole)
        LOG_PROCESS_ERROR(pOnlineRole->pRoleData == role);

    auction = CAuctionMgr::GetInstance().GetAuction(pMsg->dwAuctionID);
    LOG_PROCESS_ERROR(auction);

    if (pMsg->nResult == errSuccess)
    {
        INT32 nRetCode = CAuctionMgr::GetInstance().PaidLeftMoney(auction);
        LOG_CHECK_ERROR(nRetCode == errSuccess);

        if (nRetCode != errSuccess)
        {
            CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(role, "Auction System Error Pay For Money", "", MONEYTYPE_02, pMsg->llPrice);

            if (pOnlineRole)
                DoErrorNotify(pOnlineRole->dwCliSessionID, nRetCode, 0, 0);
        }
    }
    else
    {
        if (pOnlineRole)
            DoErrorNotify(pOnlineRole->dwCliSessionID, pMsg->nResult, 0, 0);
    }

Exit0:
    return;
}

void CMessageHandler::OnGSExchangeAddOrder(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_EXCHANGE_ADD_ORDER* pMsg = (G2M_EXCHANGE_ADD_ORDER*)pPacketBuffer;

    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(role->eAuctionQueryStatus == MS_ROLE_DATA::eqsGsExchange);
    role->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;

    if (pOnlineRole)
        LOG_PROCESS_ERROR(pOnlineRole->pRoleData == role);

    if (pMsg->nResult == errSuccess)
    {
        INT32 nRetCode = CExchangeMgr::GetInstance().AddOrder(role, pMsg->byOrderType, pMsg->nCountOrder, pMsg->nPriceOrder, NULL);
        LOG_CHECK_ERROR(nRetCode == errSuccess);

        if (nRetCode != errSuccess)
        {
            CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(role, "Exchange System Error", "", pMsg->nItemIDRequire, pMsg->nItemCountRequire);

            if (pOnlineRole)
                DoErrorNotify(pOnlineRole->dwCliSessionID, nRetCode, 0, 0);
        }
    }
    else
    {
        if (pOnlineRole)
            DoErrorNotify(pOnlineRole->dwCliSessionID, pMsg->nResult, 0, 0);
    }

Exit0:
    return;
}

void CMessageHandler::OnGSUpdateRankListData(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_RANKLIST_ROLE_DATA* pMsg = (G2M_RANKLIST_ROLE_DATA*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    CMsGuild* poMsGuild = NULL;
    CMsGuildMember* poMsGMember = NULL;
    INT32 nAchID = 0;
    INT32 nRankListType = rlInvalid;
    RANK_NODE* pRankNode = NULL;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);

    PROCESS_ERROR(pOnlineRole->nGMLevel == 0);
    PROCESS_ERROR(pOnlineRole->pRoleData);
    PROCESS_ERROR(CManageServer::GetInstance().GetGameTime() >
        g_ServerConfig.GroupConfig[pOnlineRole->pRoleData->byGroup].dwStartDate + 86400);

    nRetCode = CRankListMgr::GetInstance().UpdateRoleRank(pOnlineRole->dwCliSessionID, pMsg->dwRoleID, pMsg->nLevel, pMsg->nArmerPower,
        pMsg->nSpiritPower, pMsg->nTotalPower, pMsg->nMoney, pMsg->nAchievement, pMsg->nMaster, pMsg->nPkValue, pMsg->nTotalKillCount,
        pMsg->nClass, pMsg->nExp, pMsg->nItemPower);
    LOG_PROCESS_ERROR(nRetCode);

    if (pOnlineRole->pRoleData->dwGuildID != 0)
    {
        poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pOnlineRole->pRoleData->dwGuildID);
        LOG_PROCESS_ERROR(poMsGuild);

        poMsGMember = poMsGuild->GetMember(pOnlineRole->pRoleData->dwRoleID);
        LOG_PROCESS_ERROR(poMsGMember);

        poMsGMember->SetPow(pMsg->nTotalPower);
    }

    CRankListMgr::GetInstance().BeginRankAchievementID();

    while ((nAchID = CRankListMgr::GetInstance().NextRankAchievementID()) > 0)
    {
        INT32 nRank = 0;

        const ACHIEVEMENT_DATA* pData = CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(nAchID);
        if (!pData || pData->vecAchievementType[0] != adRanklist)
        {
            LOG_CHECK_ERROR(false);
            continue;
        }

        switch (pData->vecAchievementValue1[0])
        {
            case avdRankPow:
                nRankListType = rlPower;
                break;
            case avdRankLevel:
                nRankListType = rlLevel;
                break;
            case avdRankMoney:
                nRankListType = rlMoney;
                break;
            case avdRankPkValue:
                nRankListType = rlPkValue;
                break;
            case avdRankMasterExp:
                nRankListType = rlMaster;
                break;
            case avdRankAchievementPts:
                nRankListType = rlAchievement;
                break;
            default:
                break;
        }

        pRankNode = CRankListMgr::GetInstance().GetSelfRank(
            pOnlineRole->pRoleData->byGroup, nRankListType, pOnlineRole->pRoleData->dwRoleID);
        if (pRankNode)
            nRank = pRankNode->nSelfRank;

        if (nRank > 0 && nRank <= pData->vecAchievementValue2[0])
        {
            nRetCode = DoGSCompleteAchievement(pOnlineRole->dwGSSessionID, pOnlineRole->pRoleData->dwRoleID, nAchID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSUpdateRankListNode(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_UPDATE_RANKLIST_NODE* pMsg = (G2M_UPDATE_RANKLIST_NODE*)pPacketBuffer;

    nRetCode = CRankListMgr::GetInstance().UpdateRankNode(pMsg->byServerGroup, pMsg->nRankListType, pMsg->llKey, pMsg->llValue);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}


void CMessageHandler::OnGSAdventureTrigger(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_ADVENTURE_TRIGGER* pMsg = (G2M_ADVENTURE_TRIGGER*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    CMSAdventure::GetInstance().CheckTrigger(pOnlineRole, pMsg->nAdventureID, pMsg->nVarID, pMsg->nMaxCount, pMsg->nChance);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSSendAwardToUmp(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_SEND_AWARD_TO_UMP* pMsg = (G2M_SEND_AWARD_TO_UMP*)pPacketBuffer;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    LOG_PROCESS_ERROR(pMsg);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CAwardManager::GetInstance().SendAwardReq(pOnlineRole, pMsg->nAwardType, pMsg->chAwardNumber);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    return;
}

void CMessageHandler::OnGSQueryRoleLocation(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    MS_ONLINE_ROLE_DATA* pTargetOnlineRole{};
    INT32 nRetCode = 0;
    G2M_QUERY_ROLE_LOCATION* pMsg = (G2M_QUERY_ROLE_LOCATION*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* pSrcOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    PROCESS_ERROR(pSrcOnlineRole && pSrcOnlineRole->pRoleData);

    pTargetOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByNameInGroup(
        pMsg->szRoleName, pSrcOnlineRole->pRoleData->byGroup);
    PROCESS_ERROR(pTargetOnlineRole && pTargetOnlineRole->pRoleData);
    PROCESS_ERROR(pTargetOnlineRole->pUser->eState == USER_STATE_INGAME);
    if (pMsg->bFromItem)
    {
        PROCESS_ERROR(pTargetOnlineRole->pRoleData->nSceneTempID == pSrcOnlineRole->pRoleData->nSceneTempID);

        const SCENE_DATA* pcSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pSrcOnlineRole->pRoleData->nSceneTempID);
        PROCESS_ERROR(pcSceneData);

        if (pcSceneData->nType != estLine)
            PROCESS_ERROR(pTargetOnlineRole->pRoleData->nSceneID == pSrcOnlineRole->pRoleData->nSceneID);
    }

    nRetCode = DoGSQueryRoleLocation(pTargetOnlineRole->dwGSSessionID, pMsg->dwRoleID, pMsg->dwClientSessionID,
        pTargetOnlineRole->pRoleData->dwRoleID, pMsg->bFromItem);
    LOG_PROCESS_ERROR(nRetCode);

    return;
Exit0:
    SGSSession* pGSSession = (SGSSession*)pSocketSession->GetUserData();
    if (pGSSession)
    {
        DoGSQueryRoleLocationAck(pGSSession->dwSessionID, pMsg->dwRoleID, pMsg->dwClientSessionID, 0, 0, 0, 0, 0, pMsg->bFromItem);
    }

    return;
}

void CMessageHandler::OnGSQueryRoleLocationAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_QUERY_ROLE_LOCATION_ACK* pMsg = (G2M_QUERY_ROLE_LOCATION_ACK*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRoleData && pOnlineRoleData->pRoleData);

    nRetCode = DoGSQueryRoleLocationAck(pOnlineRoleData->dwGSSessionID, pMsg->dwRoleID, pMsg->dwClientSessionID,
        pMsg->nSceneTempID, pMsg->nSceneID, pMsg->nX, pMsg->nY, pMsg->nZ, pMsg->bFromItem);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnGSTakeRole(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_TAKE_ROLE* pMsg = (G2M_TAKE_ROLE*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByName(pMsg->szRoleName);
    LOG_PROCESS_ERROR(pOnlineRoleData && pOnlineRoleData->pRoleData);

    nRetCode = DoGSTakeRole(pOnlineRoleData->dwGSSessionID, pOnlineRoleData->pRoleData->dwRoleID, pOnlineRoleData->dwCliSessionID,
        pMsg->nSceneTempID, pMsg->nSceneID, pMsg->nX, pMsg->nY, pMsg->nZ);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnGSRecallTeamMember(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    CTeam* poTeam{};
    INT32 nRetCode = 0;
    G2M_RECALL_TEAM_MEMBER* pMsg = (G2M_RECALL_TEAM_MEMBER*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwLeaderID);
    LOG_PROCESS_ERROR(pOnlineRoleData && pOnlineRoleData->pRoleData);

    poTeam = CTeamMgr::GetInstance().Find(pOnlineRoleData->pRoleData->dwTeamID);
    LOG_PROCESS_ERROR(poTeam);
    LOG_PROCESS_ERROR(poTeam->GetLeader() == pMsg->dwLeaderID);

    for (INT32 nIndex = 0; nIndex < poTeam->GetMemberSizeLimit(); nIndex++)
    {
        if (poTeam->GetMemberID(nIndex) == INVALID_OBJECT_ID || poTeam->GetMemberID(nIndex) == poTeam->GetLeader())
            continue;

        MS_ONLINE_ROLE_DATA* pMemberRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleIDInGroup(
            poTeam->GetMemberID(nIndex), poTeam->GetGroupID());
        if (pMemberRole == NULL || pMemberRole->pRoleData == NULL)
            continue;

        if (pMemberRole->bEnabledTeamRecall)
        {
            const SCENE_DATA* pcSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(pMemberRole->pRoleData->nSceneTempID);
            if (pcSceneData == NULL || pcSceneData->bCannotBeRecalled)
                continue;

            nRetCode = DoGSTakeRole(pMemberRole->dwGSSessionID, pMemberRole->pRoleData->dwRoleID, pMemberRole->dwCliSessionID,
                pMsg->nSceneTempID, pMsg->nSceneID, pMsg->nX, pMsg->nY, pMsg->nZ);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
        {
            char szMessage[COMMON_NAME_LEN];

            snprintf(szMessage, COMMON_NAME_LEN, g_GWConfig.szRecallFailMessage, pMemberRole->pRoleData->szRoleName);
            nRetCode = DoClientChatMessage("", 0, pMsg->dwLeaderID, cltNormal, szMessage, pMemberRole->pRoleData->byGroup, FALSE);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

Exit0:

    return;
}

void CMessageHandler::OnGSSetServerVariable(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    G2M_SET_SERVER_VARIABLE* pMsg = (G2M_SET_SERVER_VARIABLE*)pPacketBuffer;

    if (pMsg->bAbsoluteValue)
    {
        CServerVar::GetInstance().SetVariable(pMsg->byServerGroup, pMsg->qwKey, pMsg->nVarValue);
    }
    else
    {
        INT32 nValue = CServerVar::GetInstance().GetVariable(pMsg->byServerGroup, pMsg->qwKey) + pMsg->nVarValue;
        CServerVar::GetInstance().SetVariable(pMsg->byServerGroup, pMsg->qwKey, nValue);
    }

    return;
Exit0:

    return;
}

void CMessageHandler::OnGSGetBlackMarketLimitInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_GET_BLACK_MARKET_LIMIT_INFO* pMsg = (G2M_GET_BLACK_MARKET_LIMIT_INFO*)pPacketBuffer;
    SGSSession* s = (SGSSession*)pSocketSession->GetUserData();
    DoGSGetBlackMarketLimitInfo(s->dwSessionID, pMsg->dwRoleID);
}

void CMessageHandler::OnGSCheckBlackMarketProduct(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_CHECK_BLACK_MARKET_PRODUCT* pMsg = (G2M_CHECK_BLACK_MARKET_PRODUCT*)pPacketBuffer;
    INT32 nCount = 0;
    INT32 nResult = CMSBlackMarketMgr::GetInstance().CheckProduct(pMsg->nBlackMarketProductID, nCount);
    SGSSession* s = (SGSSession*)pSocketSession->GetUserData();
    DoGSCheckBlackMarketProduct(s->dwSessionID, pMsg->dwRoleID, pMsg->nBlackMarketProductID, nResult, nCount);
}

void CMessageHandler::OnGSAddExerciseTime(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_ADD_EXERCISE_TIME* pMsg = (G2M_ADD_EXERCISE_TIME*)pPacketBuffer;
    UINT32 cur_time = CManageServer::GetInstance().GetGameTime();
    INT32 nResult = errUnknownError;
    BOOL bRet = FALSE;

    MS_ONLINE_ROLE_DATA* role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR_RET_CODE(g_ServerConfig.MS.bOABillingEnable, errTimeBillingNotOpen);
    LOG_PROCESS_ERROR(pMsg->nHour > 0);
    PROCESS_ERROR_RET_CODE(role->pUser->nTimeLeft > pMsg->nHour * EXERCISE_COST_PER_HOUR + g_ServerConfig.MS.nTimeCardBillingPeriod, errTimeBillingNotEnough);

    bRet = COABillMgr::GetInstance().DoSubTime(role, cur_time, cur_time, pMsg->nHour * EXERCISE_COST_PER_HOUR, etbtSupplementary);
    LOG_PROCESS_ERROR(bRet);

    return;

Exit0:
    SGSSession* s = (SGSSession*)pSocketSession->GetUserData();
    bRet = DoGSAddExerciseTime(s->dwSessionID, pMsg->dwRoleID, pMsg->nHour, nResult);
    LOG_CHECK_ERROR(bRet);
    return;
}

void CMessageHandler::OnGSFreezeLogin(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;
    MS_ROLE_DATA* pRoleData = NULL;
    G2M_FREEZE_LOGIN* pMsg = (G2M_FREEZE_LOGIN*)pPacketBuffer;
    UINT32 dwFreezeEndTime = 0;

    LOG_PROCESS_ERROR(pMsg->dwFreezeTime > 0);

    dwFreezeEndTime = pMsg->dwFreezeTime + CManageServer::GetInstance().GetGameTime();

    DBG("freeze end time: %s", get_time_str(dwFreezeEndTime));

    pRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(pRoleData);

    nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlReplaceGM, NULL, NULL, "uculud", pRoleData->byGroup, (((UINT64)pMsg->dwRoleID) << 32) + GM_RECORD_FREEZE_CHEAT_ROLE, dwFreezeEndTime);
    LOG_PROCESS_ERROR(nRetCode);

    pRoleData->dwLoginFrozenTime = dwFreezeEndTime;

Exit0:
    return;
}

void CMessageHandler::OnGSTemporaryCharge(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    const REAL_MONEY_SHOP_DATA* data{};
    BOOL bRet{};
    G2M_TEMPORARY_CHARGE* pMsg = (G2M_TEMPORARY_CHARGE*)pPacketBuffer;

    MS_ONLINE_ROLE_DATA* role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(pMsg->dwRoleID);
    LOG_PROCESS_ERROR(role);

    data = CRealMoneyShop::GetInstance().GetDataMgr().GetFirstData();
    LOG_PROCESS_ERROR(data);

    bRet = COABillMgr::GetInstance().DoCharge(role, data->m_nCurrentPrice, data->m_nItemTemplateID, data->m_shItemCount, "");
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return;
}

// Player queries ingots
void CMessageHandler::OnGSQueryRealMoneyAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_QUERY_REAL_MONEY_ACK* pMsg = (G2M_QUERY_REAL_MONEY_ACK*)pPacketBuffer;
    CMessageHandler::GetInstance().DoClientSyncRealMoney(pMsg->dwCliSessionID, pMsg->llRealMoney);
}

// Player purchases cash-shop items
void CMessageHandler::OnGSRoleRealMoneyShopBuyItemAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    G2M_ROLE_REAL_MONEY_SHOP_BUY_ACK* pMsg = (G2M_ROLE_REAL_MONEY_SHOP_BUY_ACK*)pPacketBuffer;

    if (pMsg->nResult == 0)
    {
        // Purchase succeeded
        CRealMoneyShop::GetInstance().OnCharge(pMsg->nResult, pMsg->dwRoleID, pMsg->nItemID, pMsg->nItemCount, pMsg->szRoleName);
    }
}

// ========== DoGS functions follow ==========

BOOL CMessageHandler::DoGSRegisterAck(UINT32 dwGSSessionID)
{
    INT32 nRetCode = 0;
    M2G_REGISTER_ACK* pMsg = NULL;

    pMsg = (M2G_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_register_ack;
    pMsg->dwGSSessionID = dwGSSessionID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_REGISTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSPreEnterGS(INT32 dwGSSessionID, MS_USER* poUser, UINT32 dwRoleID, INT32 nSceneTempID,
    INT32 nSceneID, INT32 nX, INT32 nY, INT32 nZ, INT32 nFaceDirection)
{
    INT32 nRetCode = 0;
    M2G_PRE_ENTER_GS* pMsg = NULL;
    INT32 nSize = sizeof(M2G_PRE_ENTER_GS);

    LOG_PROCESS_ERROR(poUser);

    pMsg = (M2G_PRE_ENTER_GS*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_pre_enter_gs;
    pMsg->dwCliSessionID = poUser->dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;
    pMsg->nDestX = nX;
    pMsg->nDestY = nY;
    pMsg->nDestZ = nZ;
    pMsg->wFaceDirection = nFaceDirection;
    strxcpy(pMsg->szUserName, poUser->szUserName, USER_NAME_LEN);
    pMsg->byFcmState = poUser->byFcmState;
    pMsg->bIpWhiteList = poUser->bIpWhiteList;
    pMsg->bUserWhiteList = poUser->bUserWhiteList;
    pMsg->dwIpAddr = poUser->dwClientIp;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    // GS count increase
    {
        SGSSession* pGSSession = CMSSessionMgr::GetInstance().GetGSSession(dwGSSessionID);
        LOG_CHECK_ERROR(pGSSession);

        if (pGSSession)
        {
            ++pGSSession->nOnlineRoleCount;
            INF("role session<%08X> try to pre enter GS %d, remains online role count %d",
                poUser->dwCliSessionID, dwGSSessionID, pGSSession->nOnlineRoleCount);
        }
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSCreateScene(INT32 dwGSSessionID, INT32 nServerGroupID, INT32 nSceneTempID, INT32 nSceneID)
{
    INT32 nRetCode = 0;
    M2G_CREATE_SCENE* pMsg = NULL;
    INT32 nSize = sizeof(M2G_CREATE_SCENE);

    pMsg = (M2G_CREATE_SCENE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_create_scene;
    pMsg->byGroupID = (UINT8)nServerGroupID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSDestroyScene(INT32 dwGSSessionID, INT32 nServerGroupID, INT32 nSceneTempID, INT32 nSceneID)
{
    INT32 nRetCode = 0;
    M2G_DESTROY_SCENE* pMsg = NULL;
    INT32 nSize = sizeof(M2G_DESTROY_SCENE);

    pMsg = (M2G_DESTROY_SCENE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_destroy_scene;
    pMsg->byGroupID = (UINT8)nServerGroupID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSCloseScene(INT32 dwGSSessionID, INT32 nServerGroupID, INT32 nSceneTempID, INT32 nSceneID)
{
    INT32 nRetCode = 0;
    M2G_CLOSE_SCENE* pMsg = NULL;
    INT32 nSize = sizeof(M2G_CLOSE_SCENE);

    pMsg = (M2G_CLOSE_SCENE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_close_scene;
    pMsg->byServerGroup = (UINT8)nServerGroupID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSChangeScene(INT32 dwGSSessionID, INT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    M2G_CHANGE_SCENE* pMsg = NULL;
    INT32 nSize = sizeof(M2G_CHANGE_SCENE);

    pMsg = (M2G_CHANGE_SCENE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_change_scene;
    pMsg->dwCliSessionID = dwClientSessionID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSEnterGSAck(INT32 dwGSSessionID, INT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    M2G_ENTER_GS_ACK* pMsg = NULL;

    pMsg = (M2G_ENTER_GS_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_ENTER_GS_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_enter_gs_ack;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ENTER_GS_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSLeaveGS(INT32 dwGSSessionID, INT32 dwClientSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    M2G_LEAVE_GS* pMsg = NULL;

    pMsg = (M2G_LEAVE_GS*)m_pPacketAllocator->Alloc(sizeof(M2G_LEAVE_GS));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_leave_gs;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_LEAVE_GS));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSQuerySceneAck(INT32 dwGSSessionID, INT32 dwClientSessionID, INT32 nErrorCode, UINT32 dwRoleID,
    INT32 nSceneTempID, INT32 nSceneID, INT32 nX, INT32 nY, INT32 nZ, INT32 nFaceDirection)
{
    INT32 nRetCode = 0;
    M2G_QUERY_SCENE_ACK* pMsg = NULL;
    INT32 nSize = sizeof(M2G_QUERY_SCENE_ACK);

    pMsg = (M2G_QUERY_SCENE_ACK*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_query_scene_ack;
    pMsg->dwCliSessionID = dwClientSessionID;
    pMsg->nErrCode = nErrorCode;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;
    pMsg->nDestX = nX;
    pMsg->nDestY = nY;
    pMsg->nDestZ = nZ;
    pMsg->nFaceDirection = nFaceDirection;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSJumpArenaScene(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwSceneTempID, UINT32 dwSceneID, UINT8 bySide)
{
    INT32 nRetCode = 0;
    M2G_JUMP_ARENA_SCENE* pMsg = NULL;
    INT32 nSize = sizeof(M2G_JUMP_ARENA_SCENE);

    pMsg = (M2G_JUMP_ARENA_SCENE*)m_pPacketAllocator->Alloc(nSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_jump_arena_scene;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwSceneID = dwSceneID;
    pMsg->dwSceneTempID = dwSceneTempID;
    pMsg->bySide = bySide;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSTeamInfo(INT32 dwGSSessionID, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2G_TEAM_NEW* pMsg = NULL;
    UINT32 dwMessageSize = sizeof(M2G_TEAM_NEW) + poTeam->GetMemberCount() * sizeof(TEAM_MEMBER_CONTENT);

    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2G_TEAM_NEW*)m_pPacketAllocator->Alloc(dwMessageSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_team_new;
    pMsg->byGroupID = poTeam->GetGroupID();
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwLeadID = poTeam->GetLeader();
    pMsg->byAssignType = poTeam->GetAssignType();
    pMsg->nMemberSizeLimit = poTeam->GetMemberSizeLimit();
    pMsg->dwCreateTime = poTeam->GetCreateTime();
    strxcpy(pMsg->szTeamName, poTeam->GetTeamName(), TEAM_NAME_LEN);

    pMsg->byCount = 0;
    for (INT32 nIndex = 0; nIndex < poTeam->GetMemberSizeLimit(); nIndex++)
    {
        MS_ROLE_DATA* poRoleData = poTeam->GetMember(nIndex);
        if (poRoleData)
        {
            pMsg->stTeamMemberList[pMsg->byCount].dwMemberID = poRoleData->dwRoleID;
            pMsg->stTeamMemberList[pMsg->byCount].btLevel = poRoleData->byLevel;
            pMsg->stTeamMemberList[pMsg->byCount].btClass = poRoleData->byClass;
            pMsg->stTeamMemberList[pMsg->byCount].btSex = poRoleData->bySex;
            strxcpy(pMsg->stTeamMemberList[pMsg->byCount].szRoleName, poRoleData->szRoleName, ROLE_NAME_LEN);
            pMsg->byCount++;
        }
    }
    LOG_CHECK_ERROR(pMsg->byCount == poTeam->GetMemberCount());

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, dwMessageSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSTeamNew(CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2G_TEAM_NEW* pMsg = NULL;
    UINT32 dwMessageSize = sizeof(M2G_TEAM_NEW) + poTeam->GetMemberCount() * sizeof(TEAM_MEMBER_CONTENT);

    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2G_TEAM_NEW*)m_pPacketAllocator->Alloc(dwMessageSize);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_team_new;
    pMsg->byGroupID = poTeam->GetGroupID();
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwLeadID = poTeam->GetLeader();
    pMsg->byAssignType = poTeam->GetAssignType();
    pMsg->nMemberSizeLimit = poTeam->GetMemberSizeLimit();
    pMsg->dwCreateTime = poTeam->GetCreateTime();
    strxcpy(pMsg->szTeamName, poTeam->GetTeamName(), TEAM_NAME_LEN);

    pMsg->byCount = 0;
    for (INT32 nIndex = 0; nIndex < poTeam->GetMemberSizeLimit(); nIndex++)
    {
        MS_ROLE_DATA* poRoleData = poTeam->GetMember(nIndex);
        if (poRoleData)
        {
            pMsg->stTeamMemberList[pMsg->byCount].dwMemberID = poRoleData->dwRoleID;
            pMsg->stTeamMemberList[pMsg->byCount].btLevel = poRoleData->byLevel;
            pMsg->stTeamMemberList[pMsg->byCount].btClass = poRoleData->byClass;
            pMsg->stTeamMemberList[pMsg->byCount].btSex = poRoleData->bySex;
            strxcpy(pMsg->stTeamMemberList[pMsg->byCount].szRoleName, poRoleData->szRoleName, ROLE_NAME_LEN);
            pMsg->byCount++;
        }
    }
    LOG_PROCESS_ERROR(pMsg->byCount == poTeam->GetMemberCount());

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, dwMessageSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSTeamAddMember(MS_ROLE_DATA* pstNewMember, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2G_TEAM_ADD_MEMBER* pMsg = NULL;

    LOG_PROCESS_ERROR(poTeam);
    LOG_PROCESS_ERROR(pstNewMember);

    pMsg = (M2G_TEAM_ADD_MEMBER*)m_pPacketAllocator->Alloc(sizeof(M2G_TEAM_ADD_MEMBER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_team_add_member;
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->stMemberInfo.dwMemberID = pstNewMember->dwRoleID;
    strxcpy(pMsg->stMemberInfo.szRoleName, pstNewMember->szRoleName, ROLE_NAME_LEN);
    pMsg->stMemberInfo.btSex = pstNewMember->bySex;
    pMsg->stMemberInfo.btLevel = pstNewMember->byLevel;
    pMsg->stMemberInfo.btClass = pstNewMember->byClass;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_TEAM_ADD_MEMBER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSTeamChangeInfo(CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2G_TEAM_CHANGEINFO* pMsg = NULL;

    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2G_TEAM_CHANGEINFO*)m_pPacketAllocator->Alloc(sizeof(M2G_TEAM_CHANGEINFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_team_changeinfo;
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwLeadID = poTeam->GetLeader();
    pMsg->nTeamMemberLimit = poTeam->GetMemberSizeLimit();
    pMsg->byAssignType = poTeam->GetAssignType();
    pMsg->byAuctionItemColorLimit = poTeam->GetAuctionItemColorLimit();
    pMsg->dwActivityType = poTeam->GetActivityType();
    pMsg->dwActivityID = poTeam->GetActivityID();
    strxcpy(pMsg->szTeamName, poTeam->GetTeamName(), TEAM_NAME_LEN);


    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_TEAM_CHANGEINFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSTeamRemoveMember(UINT32 dwDelMemberID, CTeam* poTeam)
{
    INT32 nRetCode = 0;
    M2G_TEAM_REMOVE_MEMBER* pMsg = NULL;

    LOG_PROCESS_ERROR(poTeam);

    pMsg = (M2G_TEAM_REMOVE_MEMBER*)m_pPacketAllocator->Alloc(sizeof(M2G_TEAM_REMOVE_MEMBER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_team_remove_member;
    pMsg->dwTeamID = poTeam->GetTeamID();
    pMsg->dwRoleID = dwDelMemberID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_TEAM_REMOVE_MEMBER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSRoomEnterDungeon(INT32 dwGSSessionID, UINT32 dwRoleID, INT32 nSceneTempID, INT32 nSceneID)
{
    INT32 nRetCode = 0;
    M2G_ROOM_ENTER_DUNGEON* pMsg = NULL;

    pMsg = (M2G_ROOM_ENTER_DUNGEON*)m_pPacketAllocator->Alloc(sizeof(M2G_ROOM_ENTER_DUNGEON));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_room_enter_dungeon;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ROOM_ENTER_DUNGEON));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSpawnNpc(INT32 nSceneTempID,
    INT32 nX, INT32 nY, INT32 nZ, INT32 nRange, INT32 nNpcTemplateID, INT32 nNpcCount, UINT32 dwSuicideTime)
{
    INT32 nRetCode = 0;
    M2G_SPAWN_NPC* pMsg = NULL;

    pMsg = (M2G_SPAWN_NPC*)m_pPacketAllocator->Alloc(sizeof(M2G_SPAWN_NPC));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_spawn_npc;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nX = nX;
    pMsg->nY = nY;
    pMsg->nZ = nZ;
    pMsg->nRange = nRange;
    pMsg->nNpcTemplateID = nNpcTemplateID;
    pMsg->nNpcCount = nNpcCount;
    pMsg->dwSuicideTime = dwSuicideTime;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_SPAWN_NPC));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSChangeInfo(UINT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID, const char* szSignature)
{
    INT32 nRetCode = 0;
    M2G_CHANGE_INFO* pMsg = NULL;


    pMsg = (M2G_CHANGE_INFO*)m_pPacketAllocator->Alloc(sizeof(M2G_CHANGE_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_change_info;
    pMsg->dwCliSessionID = dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szSignature, szSignature, SIGNATURE_LEN);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_CHANGE_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAddFriend(UINT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID, UINT32 dwFriendID, UINT8 byGroupID)
{
    INT32 nRetCode = 0;
    M2G_FRIEND_ADD* pMsg = NULL;

    pMsg = (M2G_FRIEND_ADD*)m_pPacketAllocator->Alloc(sizeof(M2G_FRIEND_ADD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_friend_add;
    pMsg->dwCliSessionID = dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwFriendID = dwFriendID;
    pMsg->byGroupID = byGroupID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_FRIEND_ADD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSDelFriend(UINT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID, UINT32 dwFriendID, UINT8 byGroupID)
{
    INT32 nRetCode = 0;
    M2G_FRIEND_DEL* pMsg = NULL;

    pMsg = (M2G_FRIEND_DEL*)m_pPacketAllocator->Alloc(sizeof(M2G_FRIEND_DEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_friend_del;
    pMsg->dwCliSessionID = dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwFriendID = dwFriendID;
    pMsg->byGroupID = byGroupID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_FRIEND_DEL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstSyncState(UINT32 dwGSSessionID, UINT32 dwRoleID, INT32 nState)
{
    INT32   nRetCode = 0;
    M2G_MST_SYNC_STATE* pMsg = NULL;

    pMsg = (M2G_MST_SYNC_STATE*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_SYNC_STATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_sync_state;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nState = nState;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MST_SYNC_STATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstSyncData(UINT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID, MS_TEACHER_DATA& teacher)
{
    INT32   nRetCode = 0;
    M2G_MST_SYNC_DATA* pMsg = NULL;

    pMsg = (M2G_MST_SYNC_DATA*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_SYNC_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_sync_data;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTeacherID = teacher.dwRoleID;
    pMsg->byStudNum = teacher.byStudNum;
    for (INT32 nIndex = 0; nIndex < teacher.byStudNum; ++nIndex)
    {
        pMsg->arrStudentID[nIndex] = teacher.arrStudentID[nIndex];
    }

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MST_SYNC_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoGSSubmitTeacherApp(UINT32 dwGSSessionID, UINT8 bySourceType, UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2G_MST_SUBMIT_TEACHER_APP* pMsg = NULL;

    pMsg = (M2G_MST_SUBMIT_TEACHER_APP*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_SUBMIT_TEACHER_APP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_submit_teacher_app;
    pMsg->bySourceType = bySourceType;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MST_SUBMIT_TEACHER_APP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstAgreeTeacherApp(UINT32 dwGSSessionID, UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2G_MST_AGREE_TEACHER_APP* pMsg = NULL;

    pMsg = (M2G_MST_AGREE_TEACHER_APP*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_AGREE_TEACHER_APP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_agree_teacher_app;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MST_AGREE_TEACHER_APP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSubmitStudApp(UINT32 dwGSSessionID, UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2G_MST_SUBMIT_STUD_APP* pMsg = NULL;

    pMsg = (M2G_MST_SUBMIT_STUD_APP*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_SUBMIT_STUD_APP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_submit_stud_app;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MST_SUBMIT_STUD_APP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstAgreeStudApp(UINT32 dwGSSessionID, UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2G_MST_AGREE_STUD_APP* pMsg = NULL;

    pMsg = (M2G_MST_AGREE_STUD_APP*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_AGREE_STUD_APP));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_agree_stud_app;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MST_AGREE_STUD_APP));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstCZ(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwTargetID, INT32 nCZType, INT32 nCZLevel, INT32 nMoneyConsumption, const char* pcszText)
{
    INT32 nRetCode = 0;
    M2G_MST_CZ* pMsg = NULL;
    INT32       nLength = 0;

    LOG_PROCESS_ERROR(sizeof(M2G_MST_CZ) + MAX_CZ_TEXT_LENGTH < MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pcszText);

    pMsg = (M2G_MST_CZ*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_cz;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTargetID = dwTargetID;
    pMsg->nConsumption = nMoneyConsumption;
    pMsg->byCZType = nCZType;
    pMsg->byCZLevel = nCZLevel;
    strxcpy(pMsg->szText, pcszText, MAX_CZ_TEXT_LENGTH);

    nLength = sizeof(M2G_MST_CZ) + strlen(pMsg->szText) + 1;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstCreateRelationNtf(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2G_MST_CREATE_RELATION_NTF* pMsg = NULL;

    pMsg = (M2G_MST_CREATE_RELATION_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_CREATE_RELATION_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_create_relation_ntf;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_MST_CREATE_RELATION_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstDelRelationNtf(UINT32 dwTeacherID, UINT32 dwStudentID)
{
    INT32 nRetCode = 0;
    M2G_MST_DEL_RELATION_NTF* pMsg = NULL;

    pMsg = (M2G_MST_DEL_RELATION_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_DEL_RELATION_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_del_relation_ntf;
    pMsg->dwTeacherID = dwTeacherID;
    pMsg->dwStudentID = dwStudentID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_MST_DEL_RELATION_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMstUpdateData(UINT32 dwRoleID, INT32 nType, INT32 nValue)
{
    INT32 nRetCode = 0;
    M2G_MST_UPDATE_DATA* pMsg = NULL;

    pMsg = (M2G_MST_UPDATE_DATA*)m_pPacketAllocator->Alloc(sizeof(M2G_MST_UPDATE_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mst_update_data;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nType = nType;
    pMsg->nValue = nValue;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_MST_UPDATE_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSMailCostCheck(UINT32 dwGSSessionID, UINT32 dwSenderRoleID, UINT32 dwReceiverRoleID, const char* subject, const char* content)
{
    INT32 nRetCode = 0;
    M2G_MAIL_COST_CHECK* pMsg = NULL;

    pMsg = (M2G_MAIL_COST_CHECK*)m_pPacketAllocator->Alloc(sizeof(M2G_MAIL_COST_CHECK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mail_cost_check;
    pMsg->dwSenderRoleID = dwSenderRoleID;
    pMsg->dwReceiverRoleID = dwReceiverRoleID;
    strxcpy(pMsg->szMailSubject, subject, MAIL_SUBJECT_LEN);
    strxcpy(pMsg->szMailContent, content, MAIL_CONTENT_LEN);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MAIL_COST_CHECK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGetSimpleAttach(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwMailID, INT32 nItemID, INT32 nItemCount)
{
    INT32 nRetCode = 0;
    M2G_MAIL_GET_SIMPLE_ATTACH* pMsg = NULL;

    pMsg = (M2G_MAIL_GET_SIMPLE_ATTACH*)m_pPacketAllocator->Alloc(sizeof(M2G_MAIL_GET_SIMPLE_ATTACH));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mail_get_simple_attach;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwMailID = dwMailID;
    pMsg->nItemID = nItemID;
    pMsg->nItemCount = nItemCount;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MAIL_GET_SIMPLE_ATTACH));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGetBufferAttach(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwMailID, const char* buffer)
{
    INT32 nRetCode = 0;
    M2G_MAIL_GET_BUFFER_ATTACH* pMsg = NULL;

    pMsg = (M2G_MAIL_GET_BUFFER_ATTACH*)m_pPacketAllocator->Alloc(sizeof(M2G_MAIL_GET_BUFFER_ATTACH) + BUFFER_ATTACH_LEN);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_mail_get_buffer_attach;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwMailID = dwMailID;
    pMsg->byBufferLen = BUFFER_ATTACH_LEN;
    memcpy(pMsg->acBuffer, buffer, BUFFER_ATTACH_LEN);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_MAIL_GET_BUFFER_ATTACH) + BUFFER_ATTACH_LEN);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSCallLuaFunction(UINT32 dwGSSessionID, char* pcszScriptName, char* pcszFuncName, INT32 nValue1, INT32 nValue2, INT32 nValue3)
{
    INT32 nRetCode = 0;
    M2G_CALL_LUA_FUNCTION* pMsg = NULL;

    pMsg = (M2G_CALL_LUA_FUNCTION*)m_pPacketAllocator->Alloc(sizeof(M2G_CALL_LUA_FUNCTION));
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(pcszScriptName);
    LOG_PROCESS_ERROR(pcszFuncName);

    pMsg->wMsg = m2g_call_lua_function;
    strxcpy(pMsg->chFileName, pcszScriptName, PATH_LEN);
    strxcpy(pcszFuncName, pcszFuncName, fUNC_NAME_LEN);

    pMsg->nValue1 = nValue1;
    pMsg->nValue2 = nValue2;
    pMsg->nValue3 = nValue3;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_CALL_LUA_FUNCTION));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;

}

BOOL CMessageHandler::DoGSJoinBattle(INT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID, UINT32 dwSceneTempID, UINT32 dwSceneID, UINT8 bySide)
{
    INT32 nRetCode = 0;
    M2G_JOIN_BATTLE* pMsg = NULL;

    pMsg = (M2G_JOIN_BATTLE*)m_pPacketAllocator->Alloc(sizeof(M2G_JOIN_BATTLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_join_battle;
    pMsg->dwCliSessionID = dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwSceneTempID = dwSceneTempID;
    pMsg->dwSceneID = dwSceneID;
    pMsg->bySide = bySide;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_JOIN_BATTLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSPreCreateGuild(INT32 dwGSSessionID, UINT32 dwRoleID, const char* pcszGuildName, INT32 nIsNeedToConsume)
{
    INT32 nRetCode = 0;
    M2G_PRE_CREATE_GUILD* pMsg = NULL;

    pMsg = (M2G_PRE_CREATE_GUILD*)m_pPacketAllocator->Alloc(sizeof(M2G_PRE_CREATE_GUILD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_pre_create_guild;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szGuildName, pcszGuildName, GUILD_NAME_LEN);
    pMsg->byIsNeedToConsume = static_cast<UINT8>(nIsNeedToConsume);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_PRE_CREATE_GUILD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSRoleAddGuildNtf(CMsGuild* poMsGuild)
{
    INT32 nRetCode = 0;
    M2G_ROLE_ADD_GUILD_NTF* pMsg = NULL;
    MS_G_B_DATA* pData = NULL;
    INT32       nLength = 0;
    GUILD_UPGRADE_TRIGGER* pTrigger = NULL;

    LOG_PROCESS_ERROR(poMsGuild);

    LOG_PROCESS_ERROR(sizeof(M2G_ROLE_ADD_GUILD_NTF) + sizeof(G_B_DATA) * GUILD_MAX_BUILDING_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2G_ROLE_ADD_GUILD_NTF*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_role_add_guild;
    pMsg->dwGuildID = poMsGuild->GetID();
    pMsg->nState = poMsGuild->GetState();
    pMsg->dwLeaderID = poMsGuild->GetLeaderID();
    strxcpy(pMsg->szGuildName, poMsGuild->GetName(), GUILD_NAME_LEN);
    pMsg->byServerGroup = (UINT8)poMsGuild->GetSvrGroup();

    pMsg->byBuildingNum = 0;
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        pData = poMsGuild->GetFunBuildingDataByType(i);
        if (pData && pData->nType != 0)
        {
            pMsg->arrBData[pMsg->byBuildingNum].qwNameID = pData->qwNameID;
            pMsg->arrBData[pMsg->byBuildingNum].nType = pData->nType;
            pMsg->arrBData[pMsg->byBuildingNum].nLevel = pData->nLevel;
            //pMsg->arrBData[pMsg->byBuildingNum].nLevel = pBData->nBuildingLevel;
            ++pMsg->byBuildingNum;
        }
    }
    pData = poMsGuild->FirstUnFunBuilding();
    while (pData)
    {
        LOG_CHECK_ERROR(pMsg->byBuildingNum < GUILD_MAX_BUILDING_NUM);
        if (pMsg->byBuildingNum == GUILD_MAX_BUILDING_NUM)
            break;
        pMsg->arrBData[pMsg->byBuildingNum].qwNameID = pData->qwNameID;
        pMsg->arrBData[pMsg->byBuildingNum].nType = pData->nType;
        pMsg->arrBData[pMsg->byBuildingNum].nLevel = pData->nLevel;
        ++pMsg->byBuildingNum;

        pData = poMsGuild->NextUnFunBuilding();
    }

    pMsg->byUpgradeBuildingNum = pMsg->byBuildingNum;
    for (INT32 i = 0; i < poMsGuild->GetUpgradeBuildingNum(); ++i)
    {
        pTrigger = poMsGuild->GetUpgradeBuilding(i);
        LOG_PROCESS_ERROR(pTrigger);

        LOG_CHECK_ERROR((pMsg->byBuildingNum + pMsg->byUpgradeBuildingNum) < GUILD_MAX_BUILDING_NUM);
        if ((pMsg->byBuildingNum + pMsg->byUpgradeBuildingNum) == GUILD_MAX_BUILDING_NUM)
            break;
        pMsg->arrBData[pMsg->byUpgradeBuildingNum].qwNameID = pTrigger->qwNameID;
        pMsg->arrBData[pMsg->byUpgradeBuildingNum].nType = pTrigger->nType;
        pData = poMsGuild->GetBuildingDataByNpcID(pTrigger->qwNameID);
        if (pData)
            pMsg->arrBData[pMsg->byUpgradeBuildingNum].nLevel = pData->nLevel;
        else
            pMsg->arrBData[pMsg->byUpgradeBuildingNum].nLevel = 0;

        ++pMsg->byUpgradeBuildingNum;       // todo_tq_d
    }

    nLength = sizeof(M2G_ROLE_ADD_GUILD_NTF) + pMsg->byBuildingNum * sizeof(G_B_DATA);

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSDeleteGuildNtf(UINT32 dwGuildID)
{
    INT32 nRetCode = 0;
    M2G_GUILD_DELETE_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_DELETE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_DELETE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_disband;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_DELETE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoGSGuildAddMemberNtf(UINT32 dwRoleID, INT32 nPosition, UINT32 dwGuildID)
{
    INT32 nRetCode = 0;
    M2G_GUILD_ADD_MEMBER_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_ADD_MEMBER_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_ADD_MEMBER_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_add_member;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byPosition = (UINT8)nPosition;
    pMsg->dwGuildID = dwGuildID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_ADD_MEMBER_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildDelMemberNtf(UINT32 dwRoleID, UINT32 dwGuildID, UINT32 dwCD)
{
    INT32 nRetCode = 0;
    M2G_GUILD_DEL_MEMBER_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_DEL_MEMBER_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_DEL_MEMBER_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_del_member;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwCD = dwCD;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_DEL_MEMBER_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSyncGuildDataNtf(UINT32 dwGSSessionID, CMsGuild* poMsGuild)
{
    INT32 nRetCode = 0;
    CMsGuildMember* poMsGuildMember = NULL;
    M2G_SYNC_GUILD_DATA_NTF* pMsg = NULL;
    MS_G_B_DATA* pData = NULL;
    INT32       nLength = 0;
    GUILD_UPGRADE_TRIGGER* pTrigger = NULL;

    LOG_PROCESS_ERROR(poMsGuild);

    LOG_PROCESS_ERROR(sizeof(M2G_SYNC_GUILD_DATA_NTF) + sizeof(G_B_DATA) * GUILD_MAX_BUILDING_NUM < MAX_MESSAGE_SIZE);
    pMsg = (M2G_SYNC_GUILD_DATA_NTF*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_sync_guild_data_ntf;
    pMsg->dwGuildID = poMsGuild->GetID();
    pMsg->nState = poMsGuild->GetState();
    pMsg->dwLeaderID = poMsGuild->GetLeaderID();
    strxcpy(pMsg->szGuildName, poMsGuild->GetName(), GUILD_NAME_LEN);
    for (INT32 nIndex = 0; nIndex < MAX_SABAC_ENROLL_COUNT; nIndex++)
        pMsg->dwSabacEnrollTime[nIndex] = poMsGuild->GetSabacEnrollTime(nIndex);
    pMsg->byServerGroup = (UINT8)poMsGuild->GetSvrGroup();
    pMsg->bySabacOccupied = poMsGuild->IsOccupiedSabac();

    pMsg->byMemberNum = 0;
    poMsGuildMember = poMsGuild->GetFirstGuildMember();
    while (poMsGuildMember)
    {
        pMsg->memberList[pMsg->byMemberNum].dwRoleID = poMsGuildMember->GetRoleID();
        pMsg->memberList[pMsg->byMemberNum].byPosition = poMsGuildMember->GetPosition();
        pMsg->byMemberNum++;
        poMsGuildMember = poMsGuild->GetNextGuildMember();
    }
    pMsg->byAllianceNum = 0;
    for (INT32 i = 0; i < poMsGuild->GetAllianceNum(); ++i)
    {
        if (poMsGuild->GetAllianceDataByIndex(i))
        {
            pMsg->arrAllianceID[pMsg->byAllianceNum++] = poMsGuild->GetAllianceDataByIndex(i)->dwGuildID;
        }
    }
    pMsg->byEnemyNum = 0;
    for (INT32 i = 0; i < poMsGuild->GetEnemyNum(); ++i)
    {
        if (poMsGuild->GetEnemyDataByIndex(i))
        {
            pMsg->arrEnemyID[pMsg->byEnemyNum++] = poMsGuild->GetEnemyDataByIndex(i)->dwGuildID;
        }
    }

    pMsg->byBuildingNum = 0;
    for (INT32 i = GUILD_B_MAIN; i < GUILD_B_FUNATIONAL_NUM; ++i)
    {
        pData = poMsGuild->GetFunBuildingDataByType(i);
        if (pData && pData->nType != 0)
        {
            pMsg->arrBData[pMsg->byBuildingNum].qwNameID = pData->qwNameID;
            pMsg->arrBData[pMsg->byBuildingNum].nType = pData->nType;
            pMsg->arrBData[pMsg->byBuildingNum].nLevel = pData->nLevel;
            //pMsg->arrBData[pMsg->byBuildingNum].nLevel = pBData->nBuildingLevel;
            ++pMsg->byBuildingNum;
        }
    }
    pData = poMsGuild->FirstUnFunBuilding();
    while (pData)
    {
        LOG_CHECK_ERROR(pMsg->byBuildingNum < GUILD_MAX_BUILDING_NUM);
        if (pMsg->byBuildingNum == GUILD_MAX_BUILDING_NUM)
            break;
        pMsg->arrBData[pMsg->byBuildingNum].qwNameID = pData->qwNameID;
        pMsg->arrBData[pMsg->byBuildingNum].nType = pData->nType;
        pMsg->arrBData[pMsg->byBuildingNum].nLevel = pData->nLevel;
        ++pMsg->byBuildingNum;

        pData = poMsGuild->NextUnFunBuilding();
    }

    pMsg->byUpgradeBuildingNum = pMsg->byBuildingNum;
    for (INT32 i = 0; i < poMsGuild->GetUpgradeBuildingNum(); ++i)
    {
        pTrigger = poMsGuild->GetUpgradeBuilding(i);
        LOG_PROCESS_ERROR(pTrigger);
        LOG_CHECK_ERROR((pMsg->byBuildingNum + pMsg->byUpgradeBuildingNum) < GUILD_MAX_BUILDING_NUM);
        if ((pMsg->byBuildingNum + pMsg->byUpgradeBuildingNum) == GUILD_MAX_BUILDING_NUM)
            break;
        pMsg->arrBData[pMsg->byUpgradeBuildingNum].qwNameID = pTrigger->qwNameID;
        pMsg->arrBData[pMsg->byUpgradeBuildingNum].nType = pTrigger->nType;
        pData = poMsGuild->GetBuildingDataByNpcID(pTrigger->qwNameID);
        if (pData)
            pMsg->arrBData[pMsg->byUpgradeBuildingNum].nLevel = pData->nLevel;
        else
            pMsg->arrBData[pMsg->byUpgradeBuildingNum].nLevel = 0;
        ++pMsg->byUpgradeBuildingNum;           // todo_tq_d
    }

    for (INT32 i = 1; i < GUILD_MAX_WELFARE_AURA_COUNT; ++i)
    {
        pMsg->arrWelfareAuraLvl[i] = poMsGuild->GetWelfareAuraLevelByType(i);
    }

    for (INT32 i = 0; i < MAX_GUILD_VARIABLE_COUNT; ++i)
    {
        pMsg->arrVariables[i] = poMsGuild->GetVariable(i);
    }

    nLength = sizeof(M2G_SYNC_GUILD_DATA_NTF) + pMsg->byBuildingNum * sizeof(G_B_DATA);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, nLength);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSyncGuildAttrNtf(UINT32 dwGuildID, INT32 nAttrType, UINT32 dwAttrValue)
{
    INT32   nRetCode = 0;
    M2G_GUILD_SYNC_ATTR_NTF* pMsg = (M2G_GUILD_SYNC_ATTR_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_SYNC_ATTR_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_sync_attr_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nAttrType = nAttrType;
    pMsg->dwAttrValue = dwAttrValue;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_SYNC_ATTR_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildSyncVariableNtf(UINT32 dwGuildID, INT32 nIndex, INT32 nValue)
{
    INT32 nRetCode = 0;
    M2G_GUILD_SYNC_VARIABLE_NTF* pMsg = (M2G_GUILD_SYNC_VARIABLE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_SYNC_VARIABLE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_sync_variable_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nIndex = nIndex;
    pMsg->nValue = nValue;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_SYNC_VARIABLE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildDrawSalary(INT32 dwGSSessionID, UINT32 dwRoleID, INT32 nSalary)
{
    INT32 nRetCode = 0;
    M2G_GUILD_DRAW_SALARY* pMsg = NULL;

    pMsg = (M2G_GUILD_DRAW_SALARY*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_DRAW_SALARY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_draw_salary;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nSalary = nSalary;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_DRAW_SALARY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildAddDiplomat(UINT8 byType, UINT32 dwGuildID, UINT32 dwTargetGuildID)
{
    INT32 nRetCode = 0;
    M2G_GUILD_ADD_DIPLOMAT_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_ADD_DIPLOMAT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_ADD_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_add_diplomat_ntf;
    pMsg->byType = byType;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwTargetGuildID = dwTargetGuildID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_ADD_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildDelDiplomat(UINT8 byType, UINT32 dwGuildID, UINT32 dwTargetGuildID)
{
    INT32 nRetCode = 0;
    M2G_GUILD_DEL_DIPLOMAT_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_DEL_DIPLOMAT_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_DEL_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_del_diplomat_ntf;
    pMsg->byType = byType;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwTargetGuildID = dwTargetGuildID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_DEL_DIPLOMAT_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildPrePushItemAck(INT32 dwGSSessionID, UINT32 dwRoleID, UINT16 wSourceConSlot, UINT16 wSourceSlot, UINT8 byTargetConSlot, UINT8 byTargetSlot)
{
    INT32 nRetCode = 0;
    M2G_GUILD_PRE_PUSH_ITEM_ACK* pMsg = NULL;

    pMsg = (M2G_GUILD_PRE_PUSH_ITEM_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_PRE_PUSH_ITEM_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_pre_push_item_ack;
    pMsg->wConSlot = wSourceConSlot;
    pMsg->wSlot = wSourceSlot;
    pMsg->byConSlot = byTargetConSlot;
    pMsg->bySlot = byTargetSlot;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_PRE_PUSH_ITEM_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildPrePopItemAck(INT32 dwGSSessionID, UINT32 dwRoleID, UINT16 wConSlot, UINT16 wSlot, UINT8 byConSlot, UINT8 bySlot)
{
    INT32 nRetCode = 0;
    M2G_GUILD_PRE_POP_ITEM_ACK* pMsg = NULL;

    pMsg = (M2G_GUILD_PRE_POP_ITEM_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_PRE_POP_ITEM_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_pre_pop_item_ack;
    pMsg->wConSlot = wConSlot;
    pMsg->wSlot = wSlot;
    pMsg->byConSlot = byConSlot;
    pMsg->bySlot = bySlot;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_PRE_POP_ITEM_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildPopItemAck(INT32 dwGSSessionID, UINT32 dwRoleID, UINT16 wConSlot, UINT16 wSlot, INT32 nLength, const char* pcszBuffer)
{
    INT32 nRetCode = 0;
    M2G_GUILD_POP_ITEM_ACK* pMsg = NULL;

    pMsg = (M2G_GUILD_POP_ITEM_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_POP_ITEM_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_pop_item_ack;
    pMsg->wConSlot = wConSlot;
    pMsg->wSlot = wSlot;
    pMsg->dwRoleID = dwRoleID;

    pMsg->nLength = nLength;
    memset(pMsg->buff, 0, sizeof(pMsg->buff));
    memcpy(pMsg->buff, pcszBuffer, nLength);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_POP_ITEM_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildUpgradeBuildingNtf(UINT32 dwGuildID, UINT64 qwNameID, INT32 nType, INT32 nLevel)
{
    INT32   nRetCode = 0;
    M2G_GUILD_UPGRADE_BUILDING_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_UPGRADE_BUILDING_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_UPGRADE_BUILDING_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_upgrade_building_ntf;
    pMsg->nLevel = nLevel;
    pMsg->qwNameID = qwNameID;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nType = nType;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_UPGRADE_BUILDING_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildUpdateBuilding(UINT32 dwGuildID, UINT64 qwNameID, INT32 nType, INT32 nLevel)
{
    INT32   nRetCode = 0;
    M2G_GUILD_UPDATE_BUILDING_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_UPDATE_BUILDING_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_UPDATE_BUILDING_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_update_building_ntf;
    pMsg->nLevel = nLevel;
    pMsg->qwNameID = qwNameID;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nType = nType;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_UPDATE_BUILDING_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildDelBuilding(UINT32 dwGuildID, UINT64 qwNameID, INT32 nType)
{
    INT32   nRetCode = 0;
    M2G_GUILD_DEL_BUILDING_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_DEL_BUILDING_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_DEL_BUILDING_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_del_building_ntf;
    pMsg->qwNameID = qwNameID;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nType = nType;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_DEL_BUILDING_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildUpdateWelfareID(UINT32 dwGuildID, INT32 nType, INT32 nLevel)
{
    INT32   nRetCode = 0;
    M2G_GUILD_UPDATE_WELFARE_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_UPDATE_WELFARE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_UPDATE_WELFARE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_update_welfare_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nType = nType;
    pMsg->nLevel = nLevel;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_UPDATE_WELFARE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildUpdateState(UINT32 dwGuildID, INT32 nState)
{
    INT32       nRetCode = 0;
    M2G_GUILD_UPDATE_STATE_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_UPDATE_STATE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_UPDATE_STATE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_update_state_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->nState = nState;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_UPDATE_STATE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildChangePosition(UINT32 dwGuildID, UINT32 dwRoleID, INT32 nPosition)
{
    INT32   nRetCode = 0;
    M2G_GUILD_CHANGE_POSITION_NTF* pMsg = NULL;

    pMsg = (M2G_GUILD_CHANGE_POSITION_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_CHANGE_POSITION_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_change_position_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nPosition = nPosition;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_CHANGE_POSITION_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildPreOpenDungeonAck(INT32 dwGSSessionID, UINT32 dwTemplateID, UINT32 dwOwnerID, UINT32 dwLeaderID, INT32 nConsumeMoney)
{
    INT32   nRetCode = 0;
    M2G_GUILD_PRE_OPEN_DUNGEON_ACK* pMsg = NULL;

    pMsg = (M2G_GUILD_PRE_OPEN_DUNGEON_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_PRE_OPEN_DUNGEON_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_pre_open_dungeon_ack;
    pMsg->dwLeaderID = dwLeaderID;
    pMsg->dwTemplateID = dwTemplateID;
    pMsg->dwOwnerID = dwOwnerID;
    pMsg->nConsumeMoney = nConsumeMoney;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_PRE_OPEN_DUNGEON_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildEnterDungeon(INT32 dwGSSessionID, UINT32 dwTemplateID, UINT32 dwRoleID, UINT32 dwDungeonLeaderID)
{
    INT32   nRetCode = 0;
    M2G_GUILD_ENTER_DUNGEON* pMsg = NULL;

    pMsg = (M2G_GUILD_ENTER_DUNGEON*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_ENTER_DUNGEON));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_enter_dungeon;
    pMsg->dwTemplateID = dwTemplateID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwDungeonLeaderID = dwDungeonLeaderID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_ENTER_DUNGEON));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildSyncWarRelation(UINT32 dwIntruderID, UINT32 dwDefenderID)
{
    INT32       nRetCode = 0;
    M2G_GUILD_SYNC_WAR_RELATION* pMsg = (M2G_GUILD_SYNC_WAR_RELATION*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_SYNC_WAR_RELATION));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_sync_war_relation;
    pMsg->dwIntruderID = dwIntruderID;
    pMsg->dwDefenderID = dwDefenderID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_SYNC_WAR_RELATION));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildSyncWarCharacter(UINT32 dwGuildID, INT32 nCharacter)
{
    INT32       nRetCode = 0;
    M2G_GUILD_SYNC_WAR_CHAR* pMsg = (M2G_GUILD_SYNC_WAR_CHAR*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_SYNC_WAR_CHAR));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_sync_war_char;
    pMsg->dwGuildID = dwGuildID;
    pMsg->byCharacter = (UINT8)nCharacter;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_SYNC_WAR_CHAR));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildChangeRes(UINT32 dwNpcID, UINT32 dwGuildID, BOOL bAdd, INT32 nResType, INT32 nCount, INT32 nCtxType)
{
    INT32       nRetCode = 0;
    M2G_GUILD_CHANGE_RES_ACK* pMsg = (M2G_GUILD_CHANGE_RES_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_CHANGE_RES_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_change_res_ack;
    pMsg->dwNpcID = dwNpcID;
    pMsg->dwGuildID = dwGuildID;
    pMsg->bAdd = bAdd;
    pMsg->nResType = nResType;
    pMsg->nCount = nCount;
    pMsg->byCtxType = nCtxType;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_CHANGE_RES_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildWarEnd(UINT32 dwDefenderID)
{
    INT32       nRetCode = 0;
    M2G_GUILD_WAR_END* pMsg = (M2G_GUILD_WAR_END*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_WAR_END));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_war_end;
    pMsg->dwDefenderID = dwDefenderID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_WAR_END));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoGSGuildSyncBuildingStateNtf(UINT32 dwGuildID, UINT64 qwNameID, UINT32 dwReviveTime)
{
    INT32   nRetCode = 0;
    M2G_GUILD_SYNC_BUILDING_STATE_NTF* pMsg = (M2G_GUILD_SYNC_BUILDING_STATE_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_SYNC_BUILDING_STATE_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_sync_building_state_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->qwNameID = qwNameID;
    pMsg->dwReviveTime = dwReviveTime;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_SYNC_BUILDING_STATE_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildSyncMemberSalaryNtf(UINT32 dwGuildID, UINT32 dwRoleID, INT32 nSalary)
{
    INT32   nRetCode = 0;
    M2G_GUILD_SYNC_MEMBER_SALARY_NTF* pMsg = (M2G_GUILD_SYNC_MEMBER_SALARY_NTF*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_SYNC_MEMBER_SALARY_NTF));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_sync_member_salary_ntf;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nSalary = nSalary;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_SYNC_MEMBER_SALARY_NTF));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSyncRoleSocialData(MS_ONLINE_ROLE_DATA* pstOnlineRoleData)
{
    INT32 nRetCode = 0;
    M2G_SYNC_ROLE_SOCIAL_DATA* pMsg = NULL;

    LOG_PROCESS_ERROR(pstOnlineRoleData);
    LOG_PROCESS_ERROR(pstOnlineRoleData->pRoleData);

    pMsg = (M2G_SYNC_ROLE_SOCIAL_DATA*)m_pPacketAllocator->Alloc(sizeof(M2G_SYNC_ROLE_SOCIAL_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_sync_role_social_data;
    pMsg->dwRoleID = pstOnlineRoleData->pRoleData->dwRoleID;
    pMsg->dwGuildID = pstOnlineRoleData->pRoleData->dwGuildID;
    pMsg->dwForbidChatTime = pstOnlineRoleData->pRoleData->dwForbidChatTime;

    nRetCode = _SendGS(pstOnlineRoleData->dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_SYNC_ROLE_SOCIAL_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGuildAchievementReward(INT32 dwGSSessionID, UINT32 dwRoleID, INT32 nID)
{
    INT32 nRetCode = 0;
    M2G_GUILD_ACHIEVEMENT_REWARD* pMsg = (M2G_GUILD_ACHIEVEMENT_REWARD*)m_pPacketAllocator->Alloc(sizeof(M2G_GUILD_ACHIEVEMENT_REWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_guild_achievement_reward;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nID = nID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_GUILD_ACHIEVEMENT_REWARD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoGSEnrollSabac(UINT32 dwGuildID, UINT32 dwEnrollTime)
{
    INT32 nRetCode = 0;
    M2G_ENROLL_SABAC* pMsg = NULL;

    pMsg = (M2G_ENROLL_SABAC*)m_pPacketAllocator->Alloc(sizeof(M2G_ENROLL_SABAC));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_enroll_sabac;
    pMsg->dwGuildID = dwGuildID;
    pMsg->dwEnrollTime = dwEnrollTime;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_ENROLL_SABAC));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSOccupySabac(UINT32 dwOldGuildID, UINT32 dwNewGuildID)
{
    INT32 nRetCode = 0;
    M2G_OCCUPY_SABAC* pMsg = NULL;

    pMsg = (M2G_OCCUPY_SABAC*)m_pPacketAllocator->Alloc(sizeof(M2G_OCCUPY_SABAC));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_occupy_sabac;
    pMsg->dwOldGuildID = dwOldGuildID;
    pMsg->dwNewGuildID = dwNewGuildID;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_OCCUPY_SABAC));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoGSArenaReward(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwItemID, INT32 nMoney)
{
    INT32       nRetCode = 0;
    M2G_ARENA_REWARD* pMsg = NULL;

    pMsg = (M2G_ARENA_REWARD*)m_pPacketAllocator->Alloc(sizeof(M2G_ARENA_REWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_arena_reward;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwItemID = dwItemID;
    pMsg->nMoney = nMoney;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ARENA_REWARD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSCallGMCommand(UINT32 dwRoleID, const char* pcszGMCommand)
{
    INT32   nRetCode = 0;
    INT32   nGMCommandLength = 0;
    MS_ONLINE_ROLE_DATA* pstOnlineRoleData = NULL;
    M2G_CALL_GM_COMMAND* pMsg = NULL;

    LOG_PROCESS_ERROR(pcszGMCommand);

    nGMCommandLength = strlen(pcszGMCommand) + 1;

    pMsg = (M2G_CALL_GM_COMMAND*)m_pPacketAllocator->Alloc(sizeof(M2G_CALL_GM_COMMAND) + nGMCommandLength);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_call_gm_command;
    pMsg->dwRoleID = dwRoleID;
    strxcpy(pMsg->szGMCommand, pcszGMCommand, nGMCommandLength);

    if (0 == dwRoleID)
    {
        nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_CALL_GM_COMMAND) + nGMCommandLength);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        pstOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
        LOG_PROCESS_ERROR(pstOnlineRoleData);
        nRetCode = _SendGS(pstOnlineRoleData->dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_CALL_GM_COMMAND) + nGMCommandLength);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSShield(INT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwTargetRoleID, char* pTargetRoleName)
{
    INT32 nRetCode = 0;
    M2G_SHIELD* pMsg = NULL;

    LOG_PROCESS_ERROR(pTargetRoleName);

    pMsg = (M2G_SHIELD*)m_pPacketAllocator->Alloc(sizeof(M2G_SHIELD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_shield;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwTargetRoleID = dwTargetRoleID;
    strxcpy(pMsg->szTargetRoleName, pTargetRoleName, ROLE_NAME_LEN);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_SHIELD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSyncGMData(INT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwForbidChatTime)
{
    INT32       nRetCode = 0;
    M2G_SYNC_GM_DATA* pMsg = (M2G_SYNC_GM_DATA*)m_pPacketAllocator->Alloc(sizeof(M2G_SYNC_GM_DATA));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_sync_gm_data;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwForbidChatTime = dwForbidChatTime;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_SYNC_GM_DATA));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSUpdateStatisticVariable(INT32 dwGSSessionID, UINT32 dwID, UINT32 dwType, UINT32 dwIndex, INT32 nValue)
{
    INT32 nRetCode = 0;
    M2G_UPDATE_STATISTIC_VALUE* pMsg = NULL;

    pMsg = (M2G_UPDATE_STATISTIC_VALUE*)m_pPacketAllocator->Alloc(sizeof(M2G_UPDATE_STATISTIC_VALUE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_update_statistic_value;
    pMsg->dwIndex = dwIndex;
    pMsg->dwID = dwID;
    pMsg->nValue = nValue;
    pMsg->dwType = dwType;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_UPDATE_STATISTIC_VALUE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSDropLimit(UINT32 dwGSSessionID, INT32 nItemID, INT32 nItemCount)
{
    INT32 nRetCode = 0;
    M2G_DROP_LIMIT* pMsg = NULL;

    pMsg = (M2G_DROP_LIMIT*)m_pPacketAllocator->Alloc(sizeof(M2G_DROP_LIMIT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_drop_limit;
    pMsg->nItemID = nItemID;
    pMsg->nLimit = nItemCount;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_DROP_LIMIT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::BroadcastGSDropLimit(INT32 nItemID, INT32 nItemCount)
{
    INT32 nRetCode = 0;
    M2G_DROP_LIMIT* pMsg = NULL;

    pMsg = (M2G_DROP_LIMIT*)m_pPacketAllocator->Alloc(sizeof(M2G_DROP_LIMIT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_drop_limit;
    pMsg->nItemID = nItemID;
    pMsg->nLimit = nItemCount;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_DROP_LIMIT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAuctionSell(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT8 byConSlot, UINT8 byItemSlot, UINT8 byAuctionTimeType, UINT8 byOpen, INT64 llPrice, INT64 llBuyoutPrice, INT64 llDepositePrice)
{
    INT32 nRetCode = 0;
    M2G_AUCTION_SELL* pMsg = (M2G_AUCTION_SELL*)m_pPacketAllocator->Alloc(sizeof(M2G_AUCTION_SELL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_auction_sell;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byConSlot = byConSlot;
    pMsg->byItemSlot = byItemSlot;
    pMsg->byAuctionTimeType = byAuctionTimeType;
    pMsg->byOpen = byOpen;
    pMsg->llPrice = llPrice;
    pMsg->llBuyoutPrice = llBuyoutPrice;
    pMsg->llDepositePrice = llDepositePrice;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_AUCTION_SELL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAuctionBuy(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwAuctionID, INT64 llBidPrice, INT64 llAuctionPrice, INT64 llDepositPrice, UINT8 byBuyType)
{
    INT32 nRetCode = 0;
    M2G_AUCTION_BUY* pMsg = NULL;

    pMsg = (M2G_AUCTION_BUY*)m_pPacketAllocator->Alloc(sizeof(M2G_AUCTION_BUY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_auction_buy;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwAuctionID = dwAuctionID;
    pMsg->llBidPrice = llBidPrice;
    pMsg->llAuctionPrice = llAuctionPrice;
    pMsg->llDepositPrice = llDepositPrice;
    pMsg->byBuyType = byBuyType;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_AUCTION_BUY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAuctionPay(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwAuctionID, INT64 llPrice)
{
    INT32 nRetCode = 0;
    M2G_AUCTION_PAY* pMsg = NULL;

    pMsg = (M2G_AUCTION_PAY*)m_pPacketAllocator->Alloc(sizeof(M2G_AUCTION_PAY));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_auction_pay;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwAuctionID = dwAuctionID;
    pMsg->llPrice = llPrice;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_AUCTION_PAY));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSExchangeAddOrder(UINT32 dwGSSessionID, UINT32 dwRoleID, INT32 nItemIDRequire, INT32 nItemCountRequire,
    UINT8 byOrderType, INT32 nPriceOrder, INT32 nCountOrder)
{
    INT32 nRetCode = 0;
    M2G_EXCHANGE_ADD_ORDER* pMsg = NULL;

    pMsg = (M2G_EXCHANGE_ADD_ORDER*)m_pPacketAllocator->Alloc(sizeof(M2G_EXCHANGE_ADD_ORDER));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_exchange_add_order;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nItemIDRequire = nItemIDRequire;
    pMsg->nItemCountRequire = nItemCountRequire;
    pMsg->byOrderType = byOrderType;
    pMsg->nPriceOrder = nPriceOrder;
    pMsg->nCountOrder = nCountOrder;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_EXCHANGE_ADD_ORDER));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAdventureAward(UINT32 dwGSSessionID, INT32 nServerGroup, UINT32 dwRoleID, INT32 nAdventureID)
{
    INT32 nRetCode = 0;
    M2G_ADVENTURE_REWARD* pMsg = NULL;

    pMsg = (M2G_ADVENTURE_REWARD*)m_pPacketAllocator->Alloc(sizeof(M2G_ADVENTURE_REWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_adventure_reward;
    pMsg->nServerGroup = nServerGroup;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nAdventureID = nAdventureID;

    if (dwGSSessionID == 0)
    {
        nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_ADVENTURE_REWARD));
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ADVENTURE_REWARD));
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSCompleteAchievement(UINT32 dwGSSessionID, UINT32 dwRoleID, INT32 nAchID)
{
    INT32 nRetCode = 0;
    M2G_COMPLETE_ACHIEVEMENT* pMsg = NULL;

    pMsg = (M2G_COMPLETE_ACHIEVEMENT*)m_pPacketAllocator->Alloc(sizeof(M2G_COMPLETE_ACHIEVEMENT));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_complete_achievement;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nAchievementID = nAchID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_COMPLETE_ACHIEVEMENT));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}


BOOL CMessageHandler::DoGSQueryRoleLocation(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwClientSessionID,
    UINT32 dwTargetRoleID, BOOL bFromItem)
{
    INT32 nRetCode = 0;
    M2G_QUERY_ROLE_LOCATION* pMsg = NULL;

    pMsg = (M2G_QUERY_ROLE_LOCATION*)m_pPacketAllocator->Alloc(sizeof(M2G_QUERY_ROLE_LOCATION));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_query_role_location;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->dwTargetRoleID = dwTargetRoleID;
    pMsg->bFromItem = (UINT8)bFromItem;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_QUERY_ROLE_LOCATION));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSQueryRoleLocationAck(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwClientSessionID,
    INT32 nSceneTempID, INT32 nSceneID, INT32 nX, INT32 nY, INT32 nZ, BOOL bFromItem)
{
    INT32 nRetCode = 0;
    M2G_QUERY_ROLE_LOCATION_ACK* pMsg = NULL;

    pMsg = (M2G_QUERY_ROLE_LOCATION_ACK*)m_pPacketAllocator->Alloc(sizeof(M2G_QUERY_ROLE_LOCATION_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_query_role_location_ack;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;
    pMsg->nX = nX;
    pMsg->nY = nY;
    pMsg->nZ = nZ;
    pMsg->bFromItem = (UINT8)bFromItem;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_QUERY_ROLE_LOCATION_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSTakeRole(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwClientSessionID,
    INT32 nSceneTempID, INT32 nSceneID, INT32 nX, INT32 nY, INT32 nZ)
{
    INT32 nRetCode = 0;
    M2G_TAKE_ROLE* pMsg = NULL;

    pMsg = (M2G_TAKE_ROLE*)m_pPacketAllocator->Alloc(sizeof(M2G_TAKE_ROLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_take_role;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->nSceneTempID = nSceneTempID;
    pMsg->nSceneID = nSceneID;
    pMsg->nX = nX;
    pMsg->nY = nY;
    pMsg->nZ = nZ;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_TAKE_ROLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSArrestRole(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT32 dwClientSessionID, INT32 nDuration)
{
    INT32 nRetCode = 0;
    M2G_ARREST_ROLE* pMsg = NULL;

    pMsg = (M2G_ARREST_ROLE*)m_pPacketAllocator->Alloc(sizeof(M2G_ARREST_ROLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_arrest_role;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->nDuration = nDuration;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ARREST_ROLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSRealMoneyBuySuccess(UINT32 dwGSSessionID, UINT32 dwRoleID, INT32 nItemID, INT32 nStackCount, INT32 nItemCount, INT32 nOriginalPrice, const char* szGiver, UINT32 dwGiverID)
{
    INT32 nRetCode = 0;
    M2G_REAL_MONEY_BUY_SUCCESS* pMsg = (M2G_REAL_MONEY_BUY_SUCCESS*)m_pPacketAllocator->Alloc(sizeof(M2G_REAL_MONEY_BUY_SUCCESS));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_real_money_buy_success;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nItemID = nItemID;
    pMsg->nStackCount = nStackCount;
    pMsg->nItemCount = nItemCount;
    pMsg->nOriginalPrice = nOriginalPrice;
    pMsg->dwGiveRoleID = dwGiverID;
    strxcpy(pMsg->szGiverRoleName, szGiver, ROLE_NAME_LEN);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_REAL_MONEY_BUY_SUCCESS));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSFcmState(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT8 byFcmState)
{
    INT32 nRetCode = 0;
    M2G_FCM_STATE* pMsg = (M2G_FCM_STATE*)m_pPacketAllocator->Alloc(sizeof(M2G_FCM_STATE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_fcm_state;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byFcmState = byFcmState;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_FCM_STATE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSFreeMode(UINT32 dwGSSessionID, UINT32 dwRoleID, UINT8 byFreeMode)
{
    INT32 nRetCode = 0;
    M2G_FREE_MODE* pMsg = (M2G_FREE_MODE*)m_pPacketAllocator->Alloc(sizeof(M2G_FREE_MODE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_free_mode;
    pMsg->dwRoleID = dwRoleID;
    pMsg->byFreeMode = byFreeMode;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_FREE_MODE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAward(UINT32 dwGSSessionID, UINT32 dwRoleID, INT32 nItemID, INT32 nItemCount)
{
    INT32 nRetCode = 0;
    M2G_AWARD* pMsg = (M2G_AWARD*)m_pPacketAllocator->Alloc(sizeof(M2G_AWARD));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_award;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nItemID = nItemID;
    pMsg->nItemCount = nItemCount;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_AWARD));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSSyncServerVariable(UINT32 dwGSSessionID, INT32 nServerGroup)
{
    INT32 nRetCode = 0;
    UINT64 qwKey = 0;
    INT32 nValue = 0;
    INT32 nCount = 0;

    M2G_SYNC_SERVER_VARIABLE* pMsg = (M2G_SYNC_SERVER_VARIABLE*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_sync_server_variable;
    pMsg->byServerGroup = (UINT8)nServerGroup;

    nRetCode = CServerVar::GetInstance().GetFirstVariable(nServerGroup, qwKey, nValue);
    while (nRetCode)
    {
        pMsg->VarList[nCount].qwKey = qwKey;
        pMsg->VarList[nCount].nValue = nValue;
        nCount++;
        LOG_PROCESS_ERROR(nCount < ((MAX_MESSAGE_SIZE - sizeof(M2G_SYNC_SERVER_VARIABLE)) / (sizeof(pMsg->VarList[0]))));
        nRetCode = CServerVar::GetInstance().GetNextVariable(nServerGroup, qwKey, nValue);
    }

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_SYNC_SERVER_VARIABLE) + sizeof(pMsg->VarList[0]) * nCount);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSUpdateServerVariable(INT32 nServerGroup, UINT64 qwKey, INT32 nValue)
{
    INT32 nRetCode = 0;
    M2G_UPDATE_SERVER_VARIABLE* pMsg = (M2G_UPDATE_SERVER_VARIABLE*)m_pPacketAllocator->Alloc(sizeof(M2G_UPDATE_SERVER_VARIABLE));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_update_server_variable;
    pMsg->byServerGroup = (UINT8)nServerGroup;
    pMsg->qwKey = qwKey;
    pMsg->nValue = nValue;

    nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_UPDATE_SERVER_VARIABLE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSUpdateAverageLevel(UINT32 dwGSSessionID, INT32 nAverageLevel[])
{
    INT32 nRetCode = 0;
    M2G_UPDATE_AVERAGE_LEVEL* pMsg = (M2G_UPDATE_AVERAGE_LEVEL*)m_pPacketAllocator->Alloc(sizeof(M2G_UPDATE_AVERAGE_LEVEL));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_update_average_level;
    for (INT32 nIndex = 0; nIndex < MAX_SERVER_GROUP_COUNT; nIndex++)
        pMsg->nAverageLevel[nIndex] = nAverageLevel[nIndex];

    if (dwGSSessionID == 0)
    {
        nRetCode = _BroadcastGS((PACKET_BUFFER*)pMsg, sizeof(M2G_UPDATE_AVERAGE_LEVEL));
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_UPDATE_AVERAGE_LEVEL));
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSGetBlackMarketLimitInfo(INT32 dwGSSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    const std::map<INT32, CMSBlackMarketMgr::BMServerLimitInfo>& limitmap = CMSBlackMarketMgr::GetInstance().GetLimitMap();
    INT32 size = sizeof(M2G_GET_BLACK_MARKET_LIMIT_INFO) + sizeof(BLACK_MARKET_LIMIT_INFO) * limitmap.size();
    M2G_GET_BLACK_MARKET_LIMIT_INFO* pMsg = (M2G_GET_BLACK_MARKET_LIMIT_INFO*)m_pPacketAllocator->Alloc(size);

    pMsg->wMsg = m2g_get_black_market_limit_info;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nLimitInfoCount = 0;
    for (std::map<INT32, CMSBlackMarketMgr::BMServerLimitInfo>::const_iterator it = limitmap.begin(); it != limitmap.end(); ++it)
    {
        pMsg->LimitInfo[pMsg->nLimitInfoCount].nBlackMarketProductID = it->first;
        pMsg->LimitInfo[pMsg->nLimitInfoCount].dwCount = it->second.nCount;
        pMsg->nLimitInfoCount++;
    }

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, size);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSCheckBlackMarketProduct(INT32 dwGSSessionID, UINT32 dwRoleID, INT32 nProductID, INT32 nResult, INT32 nCount)
{
    INT32 nRetCode = 0;
    M2G_CHECK_BLACK_MARKET_PRODUCT* pMsg = (M2G_CHECK_BLACK_MARKET_PRODUCT*)m_pPacketAllocator->Alloc(sizeof(M2G_CHECK_BLACK_MARKET_PRODUCT));

    pMsg->wMsg = m2g_check_black_market_product;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nBlackMarketProductID = nProductID;
    pMsg->nResult = nResult;
    pMsg->nCount = nCount;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_UPDATE_AVERAGE_LEVEL));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoGSAddExerciseTime(INT32 dwGSSessionID, UINT32 dwRoleID, INT32 nHour, INT32 nResult)
{
    INT32 nRetCode = 0;
    M2G_ADD_EXERCISE_TIME* pMsg = (M2G_ADD_EXERCISE_TIME*)m_pPacketAllocator->Alloc(sizeof(M2G_ADD_EXERCISE_TIME));

    pMsg->wMsg = m2g_add_exercise_time;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nHour = nHour;
    pMsg->nResult = nResult;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ADD_EXERCISE_TIME));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// ===== Custom code follows =====

BOOL CMessageHandler::DoGSQueryYuanBao(INT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    M2G_QUERY_REAL_MONEY_REQ* pMsg = NULL;

    pMsg = (M2G_QUERY_REAL_MONEY_REQ*)m_pPacketAllocator->Alloc(sizeof(M2G_QUERY_REAL_MONEY_REQ));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_query_real_money_req;
    pMsg->dwCliSessionID = dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_QUERY_REAL_MONEY_REQ));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// Player purchases cash-shop items
BOOL CMessageHandler::DoGSRoleRealMoneyShopBuyItemReq(INT32 dwGSSessionID, UINT32 dwCliSessionID, CHAR* pcszRoleName, UINT32 dwRoleID, INT32 nTotalCost, INT32 nItemID, INT32 nItemCount)
{
    INT32 nRetCode = 0;
    M2G_ROLE_REAL_MONEY_SHOP_BUY_ITEM_REQ* pMsg = NULL;

    pMsg = (M2G_ROLE_REAL_MONEY_SHOP_BUY_ITEM_REQ*)m_pPacketAllocator->Alloc(sizeof(M2G_ROLE_REAL_MONEY_SHOP_BUY_ITEM_REQ));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_role_real_money_shop_buy_item_req;
    pMsg->dwCliSessionID = dwCliSessionID;
    pMsg->dwRoleID = dwRoleID;
    pMsg->nTotalCost = nTotalCost;
    pMsg->nItemID = nItemID;
    pMsg->nItemCount = nItemCount;
    strxcpy(pMsg->szRoleName, pcszRoleName, ROLE_NAME_LEN);

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_ROLE_REAL_MONEY_SHOP_BUY_ITEM_REQ));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

// Player purchases a time card
BOOL CMessageHandler::DoGSCustomRoleBuyTime(INT32 dwGSSessionID, UINT32 dwCliSessionID, UINT32 dwRoleID, UINT32 dwBuyTimeCost)
{
    INT32 nRetCode = 0;
    M2G_CUSTOM_ROLE_BUY_TIME* pMsg = NULL;

    pMsg = (M2G_CUSTOM_ROLE_BUY_TIME*)m_pPacketAllocator->Alloc(sizeof(M2G_CUSTOM_ROLE_BUY_TIME));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2g_custom_role_buy_time;
    pMsg->dwRoleID = dwRoleID;
    pMsg->dwBuyTimeCost = dwBuyTimeCost;

    nRetCode = _SendGS(dwGSSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2G_CUSTOM_ROLE_BUY_TIME));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
