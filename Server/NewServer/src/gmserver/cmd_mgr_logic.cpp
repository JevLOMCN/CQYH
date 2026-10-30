#include "stdafx.h"
#include "cmd_mgr.h"
#include "gm_data.h"
#include "message_handler.h"
#include "tl_user.h"
#include "user_manager.h"
#include "permission_mgr.h"
#include "db_mgr.h"
#include "kernel.h"
#include "time.h"
#include "role.h"
#include "gm_role_mgr.h"
#include "gm_session_mgr.h"
#include "gm_log.h"
#include "gmserver.h"
#include "game_world.h"

BOOL CB_PreQueryRoleID(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;

    CMD_PRE_QUERY   *pQueryCmd = (CMD_PRE_QUERY*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = CDBMgr::GetInstance().PreQueryRoleID(pQueryCmd);
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Database query failed.");

    LOG_ECHO_ERROR(pQueryCmd->dwRoleID != 0, pTLUser->GetID(), "Role does not exist");

    nRetCode = CMessageHandler::GetInstance().DoTLPreQueryAck(pTLUser->GetSessionID(), pQueryCmd->szRoleName, pQueryCmd->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    pCmd = NULL;

    return TRUE;
Exit0:
    nRetCode = CMessageHandler::GetInstance().DoTLPreQueryAck(pTLUser->GetSessionID(), pQueryCmd->szRoleName, 0);
    LOG_CHECK_ERROR(nRetCode);

    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_QueryRoleID(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;
   
    CMD_QUERY_ROLE_ID   *pQueryCmd = (CMD_QUERY_ROLE_ID*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_QUERY_ROLE_NAME:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        nRetCode = CDBMgr::GetInstance().QueryRoleID(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Database query failed");
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Query RoleID timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out.");
        }
    }
        break;
    case GM_LS_QUERY_ROLE_NAME_ACK:
    {
        DBG("TLUserID[%u] Query RoleID Reslut[%d]", pTLUser->GetID(), pQueryCmd->nResult);

        LOG_ECHO_ERROR(pQueryCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Database query failed");

        nRetCode = CMessageHandler::GetInstance().DoTLQueryRoleIDAck(pTLUser->GetSessionID(), pQueryCmd->nCount, pQueryCmd->RoleKey);
        LOG_PROCESS_ERROR(nRetCode);
        
        EchoTLError(pTLUser->GetSessionID(), "Query role ID successful");

        //pTLUser->ClearCmd();
        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}


BOOL CB_QueryRoleData(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;

    CMD_QUERY_ROLE_DATA   *pQueryCmd = (CMD_QUERY_ROLE_DATA*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_MS_QUERY_ROLE_INFO:
    {
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            pCmd->eCmdState = GM_LS_TIMEOUT;
            DBG("TLUserID[%u] Query Role Data send msg to ms", pTLUser->GetID());
            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSQueryRoleData(pTLUser->GetID(), pCmd->dwSerialID, pQueryCmd->dwRoleID);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            DBG("TLUserID[%u] Query Role Data, ms not online", pTLUser->GetID());
            pQueryCmd->bIsOnline = FALSE;
            pQueryCmd->dwLogoutTime = 0;
            pQueryCmd->nResult = errSuccess;
            pCmd->eCmdState = GM_LS_MS_QUERY_ROLE_INFO_ACK;
        }
    }
        break;
    case GM_LS_MS_QUERY_ROLE_INFO_ACK:
    {
        LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), 
            "Failed to query role from ms[%s]", CGmServer::GetInstance().GetErrString(pCmd->nResult));

        pCmd->eCmdState = GM_LS_LOAD_ROLE;
    }
        break;
    case GM_LS_LOAD_ROLE:
    {
        nRetCode = CDBMgr::GetInstance().QueryRoleData(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Database query failed");
    }
        break;
    case GM_LS_LOAD_ROLE_ACK:
    {
        LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load role");

        if (pQueryCmd->bIsOnline == TRUE)
        {
            memset(pQueryCmd->szSaveTime, 0, sizeof(pQueryCmd->szSaveTime));
             
            time_t  tTime = pQueryCmd->dwLogoutTime;
            struct tm* pTM = localtime(&tTime);
            if (pTM && pQueryCmd->dwLogoutTime != 0)
                snprintf(pQueryCmd->szSaveTime, 21, "%d-%d-%d %d:%d:%d", pTM->tm_year, pTM->tm_mon, pTM->tm_yday, pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
            else
                snprintf(pQueryCmd->szSaveTime, 21, "%d", pQueryCmd->dwLogoutTime);
        }

        nRetCode = LoadRole(pQueryCmd, pQueryCmd->nLength, pQueryCmd->szData);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to parse character data.");

        pQueryCmd->eCmdState = GM_LS_QUERY_GUILD_ID;
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Query Role Data timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_QUERY_GUILD_ID:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        DBG("TLUserID[%u] Query Role Data, begin query guild id", pTLUser->GetID());

        nRetCode = CDBMgr::GetInstance().QueryGuildID(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Database query failed");
    }
        break;
    case GM_LS_QUERY_GUILD_ID_ACK:
    {
        LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load guild");

        nRetCode = LoadGuild(pQueryCmd, pQueryCmd->nLength, pQueryCmd->szData);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to parse guild data.");

        pCmd->eCmdState = GM_LS_QUERY_GM_FORBID;
    }
        break;
    case GM_LS_QUERY_GM_FORBID:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        DBG("TLUserID[%u] Query Role Data, begin query forbid time", pTLUser->GetID());

        nRetCode = CDBMgr::GetInstance().QueryGMForbid(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query GM forbid data");
    }
        break;
    case GM_LS_QUERY_GM_FORBID_ACK:
    {
        LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load GM forbid data");

        nRetCode = LoadGMForbid(pQueryCmd, pQueryCmd->nLength, pQueryCmd->szData);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to parse GM forbid data");

        pCmd->eCmdState = GM_LS_LOAD_MAIL;
    }
        break;
    case GM_LS_LOAD_MAIL:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        DBG("TLUserID[%u] query role data, begin query mail", pTLUser->GetID());

        nRetCode = CDBMgr::GetInstance().QueryMail(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query mail");
    }
        break;
    case GM_LS_LOAD_MAIL_ACK:
    {
        LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load mail data");

        nRetCode = LoadMail(pQueryCmd, pQueryCmd->nLength, pQueryCmd->szData);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to parse mail data");

        DBG("TLUserID[%u] Query Role Data, result[%d]", pTLUser->GetID(), pQueryCmd->nResult);

        nRetCode = CMessageHandler::GetInstance().DoTLQueryRoleDataAck(pTLUser->GetSessionID(), pQueryCmd->dwRoleID, 
            pQueryCmd->bIsOnline, pQueryCmd->nLength, pQueryCmd->szData);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Character data queried successfully.");

        if (pQueryCmd->poRole)
        {
            CGameWorld::GetInstance().DelRole(pQueryCmd->poRole);
            pQueryCmd->poRole = NULL;
        }

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    if (pQueryCmd && pQueryCmd->poRole)
    {
        CGameWorld::GetInstance().DelRole(pQueryCmd->poRole);
        pQueryCmd->poRole = NULL;
    }
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_ForbiddenChat(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_FORBIDDEN_CHAT   *pForbidChat = (CMD_FORBIDDEN_CHAT*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_FORBID_CHAT:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] Forbid Role Chat send msg to MS", pTLUser->GetID());
            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSForbidChat(pTLUser->GetID(), pForbidChat->dwSerialID, pForbidChat->dwTime, pForbidChat->dwRoleID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] Forbid Role Chat modify db directly, ms not online", pTLUser->GetID());

            nRetCode = CDBMgr::GetInstance().ForbiddenChat(pForbidChat);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database.");

            pForbidChat->nResult = errSuccess;
            pForbidChat->eCmdState = GM_LS_FORBID_CHAT_ACK;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Forbid Role Chat timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out.");
        }
    }
        break;
    case GM_LS_FORBID_CHAT_ACK:
    {
        INF("TLUserID[%u] Forbid Role Chat , result[%d]", pTLUser->GetID(), pForbidChat->nResult);

        LOG_ECHO_ERROR(pForbidChat->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to mute player.");

        nRetCode = CMessageHandler::GetInstance().DoTLForbidChatAck(pTLUser->GetSessionID(), pForbidChat->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        if (pForbidChat->dwTime == 0)
        {
            nRetCode = CGMLog::GetInstance().WriteLog(pForbidChat->szUserName, "Forbid chat", "RoleID[%u],EndTime[%u]", pForbidChat->dwRoleID, pForbidChat->dwTime);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
        {
            time_t  t = pForbidChat->dwTime;
            struct tm  *pt = localtime(&t);

            nRetCode = CGMLog::GetInstance().WriteLog(pForbidChat->szUserName, "Forbid chat", "RoleID[%u],EndTime[%d-%d-%d %d:%d:%d]", 
                pForbidChat->dwRoleID, pt->tm_year + 1900, pt->tm_mon, pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
            LOG_CHECK_ERROR(nRetCode);
        }

        if (pForbidChat->dwTime == 0)
            EchoTLError(pTLUser->GetSessionID(), "Failed to unmute player.");
        else
            EchoTLError(pTLUser->GetSessionID(), "Failed to mute player.");

        //pTLUser->ClearCmd();
        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_FreezeRole(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_FREEZE_ROLE   *pFreezeRole = (CMD_FREEZE_ROLE*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_FREEZE_ROLE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] Freeze Role send msg to ms", pTLUser->GetID());

            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSFreezeRole(pTLUser->GetID(), pFreezeRole->dwSerialID, pFreezeRole->dwTime, pFreezeRole->dwRoleID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] Freeze Role modify db directly, ms not online", pTLUser->GetID());

            nRetCode = CDBMgr::GetInstance().FreezeRole(pFreezeRole);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database.");

            pFreezeRole->nResult = errSuccess;
            pFreezeRole->eCmdState = GM_LS_FREEZE_ROLE_ACK;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Freeze Role timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out.");
        }
    }
        break;
    case GM_LS_FREEZE_ROLE_ACK:
    {
        INF("TLUserID[%u] Freeze Role Result[%d]", pTLUser->GetID(), pFreezeRole->nResult);

        LOG_ECHO_ERROR(pFreezeRole->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to freeze player.");

        nRetCode = CMessageHandler::GetInstance().DoTLFreezeRoleAck(pTLUser->GetSessionID(), pFreezeRole->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        if (pFreezeRole->dwTime == 0)
        {
            nRetCode = CGMLog::GetInstance().WriteLog(pFreezeRole->szUserName, "Freeze role", "RoleID[%u],EndTime[%u]", pFreezeRole->dwRoleID, pFreezeRole->dwTime);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
        {
            time_t  t = pFreezeRole->dwTime;
            struct tm  *pt = localtime(&t);
            
            nRetCode = CGMLog::GetInstance().WriteLog(pFreezeRole->szUserName, "Freeze role", "RoleID[%u],EndTime[%d-%d-%d %d:%d:%d]", 
                pFreezeRole->dwRoleID, pt->tm_year + 1900, pt->tm_mon, pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
            LOG_CHECK_ERROR(nRetCode);
        }

        if (pFreezeRole->dwTime == 0)
            EchoTLError(pTLUser->GetSessionID(), "Failed to unfreeze player.");
        else
            EchoTLError(pTLUser->GetSessionID(), "Failed to freeze player.");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_KickRole(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_KICK_ROLE   *pKickRole = (CMD_KICK_ROLE*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_KICK_ROLE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] Kick Role send msg to ms", pTLUser->GetID());

            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSKickRole(pTLUser->GetID(), pKickRole->dwSerialID, pKickRole->dwRoleID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] Kick Role ms not online", pTLUser->GetID());

            pKickRole->eCmdState = GM_LS_KICK_ROLE_ACK;
            pKickRole->nResult = errGMMSNotOnline;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Kick Role timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_KICK_ROLE_ACK:
    {
        INF("TLUserID[%u] Kick Role Result[%d]", pTLUser->GetID(), pKickRole->nResult);

        LOG_ECHO_ERROR(pKickRole->nResult == errSuccess || pKickRole->nResult == errGMMSNotOnline, pTLUser->GetSessionID(), "Failed to kick player");

        nRetCode = CMessageHandler::GetInstance().DoTLKickRoleAck(pTLUser->GetSessionID(), pKickRole->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGMLog::GetInstance().WriteLog(pKickRole->szUserName, "Kick role", "RoleID[%u]", pKickRole->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Failed to kick player");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_SendMail(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_SEND_MAIL   *pSendMail = (CMD_SEND_MAIL*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_SEND_MAIL:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT
;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] Send Mail msg to ms", pTLUser->GetID());

            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSSendMail(pTLUser->GetID(),
                pSendMail->dwSerialID, pSendMail->dwRoleID, pSendMail->szTitle, pSendMail->szContent, pSendMail->dwItemID, pSendMail->nItemNum);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] Send Mail modify db directly, ms not online", pTLUser->GetID());

            nRetCode = CDBMgr::GetInstance().SendMail(pCmd);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database");

            pSendMail->eCmdState = GM_LS_SEND_MAIL_ACK;
            pSendMail->nResult = errSuccess;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Send Mail timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_SEND_MAIL_ACK:
    {
        DBG("TLUserID[%u] Send Mail Result[%d]", pTLUser->GetID(), pSendMail->nResult);

        LOG_ECHO_ERROR(pSendMail->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to send mail");

        nRetCode = CMessageHandler::GetInstance().DoTLSendMailAck(pTLUser->GetSessionID(), pSendMail->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGMLog::GetInstance().WriteLog(pSendMail->szUserName, "send mail", "RoleID[%u],Subject[%s],Content[%s],ItemID[%u],ItemNum[%d],Reason[%s]",
            pSendMail->dwRoleID, pSendMail->szTitle, pSendMail->szContent, pSendMail->dwItemID, pSendMail->nItemNum, pSendMail->szReason);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Failed to send mail");

        //pTLUser->ClearCmd();
        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_Announce(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_ANNOUNCE   *pAnnounce = (CMD_ANNOUNCE*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_ANNOUNCE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] send announce msg to ms", pTLUser->GetID());

            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSAnnounce(pAnnounce->nGroupID, pTLUser->GetID(), 
                pAnnounce->dwSerialID, pAnnounce->dwBeginTime, pAnnounce->nRepeatNum, pAnnounce->dwIntervalTime, pAnnounce->szText);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS");
        }
        else
        {
            DBG("TLUserID[%u] send announce ms not online", pTLUser->GetID());

            pAnnounce->eCmdState = GM_LS_ANNOUNCE_ACK;
            pAnnounce->nResult = errGMMSNotOnline;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] send announce timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_ANNOUNCE_ACK:
    {
        DBG("TLUserID[%u] groupid[%d] send announce Result[%d]", pTLUser->GetID(), pAnnounce->nGroupID, pAnnounce->nResult);

        LOG_ECHO_ERROR(pAnnounce->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to send announcement");

        nRetCode = CMessageHandler::GetInstance().DoTLAnnounceAck(pAnnounce->nGroupID, pTLUser->GetSessionID(), errSuccess);
        LOG_CHECK_ERROR(nRetCode);

        if (pAnnounce->dwBeginTime == 0)
        {
            nRetCode = CGMLog::GetInstance().WriteLog(pAnnounce->szUserName, "Announce", "GroupID[%d]BeginTime[%u],RepeatNum[%d],IntervalTime[%d],Text[%s]",
                pAnnounce->nGroupID, pAnnounce->dwBeginTime, pAnnounce->nRepeatNum, pAnnounce->dwIntervalTime, pAnnounce->szText);
            LOG_CHECK_ERROR(nRetCode);
        }
        else
        {
            time_t  t = pAnnounce->dwBeginTime;
            struct tm  *pt = localtime(&t);

            nRetCode = CGMLog::GetInstance().WriteLog(pAnnounce->szUserName, "Announce", "GroupID[%d]BeginTime[%d-%d-%d %d:%d:%d],RepeatNum[%d],IntervalTime[%d],Text[%s]",
                pAnnounce->nGroupID, pt->tm_year + 1900, pt->tm_mon, pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec, pAnnounce->nRepeatNum, pAnnounce->dwIntervalTime, pAnnounce->szText);
            LOG_CHECK_ERROR(nRetCode);
        }

        EchoTLError(pTLUser->GetSessionID(), "Failed to send announcement");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    if (pTLUser && pAnnounce)
    {
        nRetCode = CMessageHandler::GetInstance().DoTLAnnounceAck(pAnnounce->nGroupID, pTLUser->GetSessionID(), nResult);
        LOG_CHECK_ERROR(nRetCode);
    }

    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_GetAnnounceList(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_GET_ANNOUNCE_LIST   *pAnnounce = (CMD_GET_ANNOUNCE_LIST*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_MS_QUERY_ANNOUNCE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] query announce list send msg to ms", pTLUser->GetID());
            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSGetAnnounceList(pAnnounce->nGroupID, pTLUser->GetID(), pAnnounce->dwSerialID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS");
        }
        else
        {
            DBG("TLUserID[%u] query announce list ms not online", pTLUser->GetID());
            pAnnounce->eCmdState = GM_LS_MS_QUERY_ANNOUNCE_ACK;
            pAnnounce->nResult = errGMMSNotOnline;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] query announce list timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_MS_QUERY_ANNOUNCE_ACK:
    {
        DBG("TLUserID[%u] groupid[%d] query announce list Result[%d]", pTLUser->GetID(), pAnnounce->nGroupID, pAnnounce->nResult);

        LOG_ECHO_ERROR(pAnnounce->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to retrieve announcement");

        nRetCode = CMessageHandler::GetInstance().DoTLGetAnnounceListAck(pAnnounce->nGroupID, pTLUser->GetSessionID(), pCmd);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Failed to retrieve announcement");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_DelAnnounce(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_DEL_ANNOUNCE   *pAnnounce = (CMD_DEL_ANNOUNCE*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_DEL_ANNOUNCE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] Del Announce send msg to ms", pTLUser->GetID());
            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSDelAnnounce(pAnnounce->nGroupID, pTLUser->GetID(), pAnnounce->dwSerialID, pAnnounce->dwAnnounceSerial);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] Del Announce ms not online", pTLUser->GetID());
            pAnnounce->eCmdState = GM_LS_DEL_ANNOUNCE_ACK;
            pAnnounce->nResult = errGMMSNotOnline;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Del Announce timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_DEL_ANNOUNCE_ACK:
    {
        DBG("TLUserID[%u] groupid[%d] Del Announce Result[%d]", pTLUser->GetID(), pAnnounce->nGroupID, pAnnounce->nResult);
        LOG_ECHO_ERROR(pAnnounce->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to delete announcement");

        nRetCode = CMessageHandler::GetInstance().DoTLDelAnnounceAck(pAnnounce->nGroupID, pTLUser->GetSessionID(), pAnnounce->dwAnnounceSerial);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGMLog::GetInstance().WriteLog(pAnnounce->szUserName, "Del Announce", "GroupID[%d]AnnounceSerial[%u]", pAnnounce->nGroupID, pAnnounce->dwAnnounceSerial);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Announcement deleted successfully.");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_Log(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_LOG   *pCmdLog = (CMD_LOG*)pCmd;
    LOG_PROCESS_ERROR(pCmdLog);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmdLog->eCmdState)
    {
    case GM_LS_QUERY_LOG:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        nRetCode = CDBMgr::GetInstance().QueryLog(pCmdLog);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database");
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] query log timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_QUERY_LOG_ACK:
    {
        ITEM_LOG_DATA       *pItemLog = NULL;
        INT32               nLength = 0;

        pItemLog = (ITEM_LOG_DATA*)pCmdLog->szBuffer;

        DBG("TLUserID[%u] query log Result[%d]", pTLUser->GetID(), pCmdLog->nResult);

        LOG_ECHO_ERROR(pCmdLog->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to query log");

        nRetCode = CMessageHandler::GetInstance().DoTLLogAck(pTLUser->GetSessionID(), pCmdLog->nLogType, pCmdLog->bTruncated, pCmdLog->nLength, pCmdLog->szBuffer);
        LOG_CHECK_ERROR(nRetCode);

        //nRetCode = CGMLog::GetInstance().WriteLog(pCmdLog->szUserName, "query game log", "Type[%d],QueryStr[%s]",
            //pCmdLog->nLogType, pCmdLog->szQueryStr);
        //LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Log query successful");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}


BOOL CB_ExeGMStr(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_EXE_GM_STR   *pExeGMStr = (CMD_EXE_GM_STR*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_EXE_SYS_CMD:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] exe gm str send to ms", pTLUser->GetID());
            // online 
            nRetCode = CMessageHandler::GetInstance().DoMSExeGMStrCmd(pTLUser->GetID(), pCmd->dwSerialID, pExeGMStr->szGMStr);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] exe gm str ms not online", pTLUser->GetID());
            pExeGMStr->eCmdState = GM_LS_EXE_SYS_CMD_ACK;
            pExeGMStr->nResult = errGMMSNotOnline;
        }
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] exe gm str timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timed out");
        }
    }
        break;
    case GM_LS_EXE_SYS_CMD_ACK:
    {
        DBG("TLUserID[%u] exe gm str Result[%d]", pTLUser->GetID(), pExeGMStr->nResult);

        LOG_ECHO_ERROR(pExeGMStr->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to execute gm command");

        nRetCode = CMessageHandler::GetInstance().DoTLExeGMStrAck(pTLUser->GetSessionID());
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = CGMLog::GetInstance().WriteLog(pExeGMStr->szUserName, "execute gm str cmd", "Str[%s]", pExeGMStr->szGMStr);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "GM command executed successfully.");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_LoadRoleData(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;

    CMD_LOAD_ROLE   *pLoadRoleCmd = (CMD_LOAD_ROLE*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pLoadRoleCmd->dwRoleID, pLoadRoleCmd->dwTLUserID);
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Role has been loaded by another user");

    switch (pCmd->eCmdState)
    {
    case GM_LS_MS_CHECK_ROLE_FREEZE:
    {
        pLoadRoleCmd->eCmdState = GM_LS_TIMEOUT;

        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] load role send msg to ms to check role is frozen", pTLUser->GetID());
            // online 
            nRetCode = CMessageHandler::GetInstance().DoCheckRoleFreeze(pTLUser->GetID(), pCmd->dwSerialID, pLoadRoleCmd->dwRoleID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS.");
        }
        else
        {
            DBG("TLUserID[%u] load role ask db to check role is frozen", pTLUser->GetID());
            nRetCode = CDBMgr::GetInstance().CheckRoleFreeze(pLoadRoleCmd);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Role is not frozen.");
        }
    }
        break;
    case GM_LS_MS_CHECK_ROLE_FREEZE_ACK:
    {
        LOG_ECHO_ERROR(pLoadRoleCmd->nResult != errGMRoleNotFreeze, pTLUser->GetSessionID(), "Role is not frozen.");
        LOG_ECHO_ERROR(pLoadRoleCmd->nResult != errGMRoleFreezeLessOneHour, pTLUser->GetSessionID(), "Role freeze time is less than 1 hour.");
        LOG_ECHO_ERROR(pLoadRoleCmd->nResult != errGMRoleOnline, pTLUser->GetSessionID(), "Role is online.");
        LOG_ECHO_ERROR(pLoadRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Other error[%s]", CGmServer::GetInstance().GetErrString(pLoadRoleCmd->nResult));

        pCmd->eCmdState = GM_LS_LOAD_ROLE;
    }
        break;
    case GM_LS_LOAD_ROLE:
    {
        DBG("TLUserID[%u] load role", pTLUser->GetID());
        nRetCode = CDBMgr::GetInstance().LoadRole(pLoadRoleCmd->dwSerialID, pLoadRoleCmd->dwRoleID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Database query failed");

        pCmd->eCmdState = GM_LS_TIMEOUT;
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] load role timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Execution timeout");
        }
    }
        break;
    case GM_LS_LOAD_ROLE_ACK:
    {
        LOG_ECHO_ERROR(pLoadRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load role");

        nRetCode = LoadRole(pLoadRoleCmd, pLoadRoleCmd->nLength, pLoadRoleCmd->szData);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to parse role data");

        pCmd->eCmdState = GM_LS_LOAD_MAIL;
    }
        break;
    case GM_LS_LOAD_MAIL:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        nRetCode = CDBMgr::GetInstance().LoadMail(pLoadRoleCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Database query failed");
    }
        break;
    case GM_LS_LOAD_MAIL_ACK:
    {
        LOG_ECHO_ERROR(pLoadRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load mail");

        nRetCode = LoadMail(pLoadRoleCmd, pLoadRoleCmd->nLength, pLoadRoleCmd->szData);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to parse mail data");

        nRetCode = CMessageHandler::GetInstance().DoTLLoadRoleDataAck(pTLUser->GetSessionID(), pLoadRoleCmd->dwRoleID, pLoadRoleCmd->nLength, pLoadRoleCmd->szData);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Failed to load role");

        if (pLoadRoleCmd->poRole)
        {
            CGameWorld::GetInstance().DelRole(pLoadRoleCmd->poRole);
            pLoadRoleCmd->poRole = NULL;
        }

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    if (pLoadRoleCmd)
    {
        if (pLoadRoleCmd->poRole)
        {
            CGameWorld::GetInstance().DelRole(pLoadRoleCmd->poRole);
            pLoadRoleCmd->poRole = NULL;
        }

        nRetCode = CGMRoleMgr::GetInstance().DelRole(pLoadRoleCmd->dwRoleID, pLoadRoleCmd->dwTLUserID);
        LOG_CHECK_ERROR(nRetCode);
    }
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_ModifyRoleData(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;

    GM_ROLE_SLOT    *pSlot = NULL;
    INT32           nLength = 0;

    CMD_MODIFY_ROLE   *pModifyRoleCmd = (CMD_MODIFY_ROLE*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pModifyRoleCmd->dwRoleID, pModifyRoleCmd->dwTLUserID);
    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Role has been loaded by another user");

    if (pModifyRoleCmd->dwTargetRoleID != 0)
    {
        nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pModifyRoleCmd->dwTargetRoleID, pModifyRoleCmd->dwTLUserID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Target role has been loaded by another user");
    }

    switch (pCmd->eCmdState)
    {
    case GM_LS_MS_DEL_ROLE_CACHE:
    {
        if (CGMSessionMgr::GetInstance().IsMSOnline())            
        {
            DBG("TLUserID[%u] modify role send msg to ms to clear role cache", pTLUser->GetID());

            nRetCode = CMessageHandler::GetInstance().DoMSClearRoleCache(pModifyRoleCmd->dwTLUserID, pModifyRoleCmd->dwRoleID, pModifyRoleCmd->dwSerialID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to ms");

            pCmd->eCmdState = GM_LS_TIMEOUT;
        }
        else
        {
            pCmd->eCmdState = GM_LS_MS_DEL_MAIL;
            pModifyRoleCmd->bClearTargetRoleCache = TRUE;
            pModifyRoleCmd->nResult = errSuccess;
            DBG("TLUserID[%u] modify role ms not online", pTLUser->GetID());
        }
    }
        break;
    case GM_LS_MS_DEL_ROLE_CACHE_ACK:
    {
        DBG("TLUserID[%u] modify role step 90, Result[%d]", pTLUser->GetID(), pModifyRoleCmd->nResult);
        LOG_ECHO_ERROR(pModifyRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to clear role cache data");

        if (pModifyRoleCmd->bClearTargetRoleCache == FALSE)
        {
            DBG("TLUserID[%u] modify role send msg to ms to clear target cache", pTLUser->GetID());

            nRetCode = CMessageHandler::GetInstance().DoMSClearRoleCache(pModifyRoleCmd->dwTLUserID, pModifyRoleCmd->dwTargetRoleID, pModifyRoleCmd->dwSerialID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to ms");

            pModifyRoleCmd->bClearTargetRoleCache = TRUE;
            pModifyRoleCmd->eCmdState = GM_LS_TIMEOUT;

            break;
        }

        pCmd->eCmdState = GM_LS_MS_DEL_MAIL;
        pCmd->nResult = errSuccess;
    }
        break;
    case GM_LS_MS_DEL_MAIL:
    {
        while (nLength < pModifyRoleCmd->nLength)
        {
            pSlot = (GM_ROLE_SLOT *)(pModifyRoleCmd->szData + nLength);

            switch (pSlot->wRoleType)
            {
            case GM_ROLE_BASE_DATA:
            case GM_ROLE_QUEST:
            case GM_ROLE_ITEM:
            case GM_ROLE_TAKE_ITEM:
                break;
            case GM_ROLE_MAIL:
            {
                if (CGMSessionMgr::GetInstance().IsMSOnline())
                {
                    DBG("TLUserID[%u] modify role send msg to ms to del mail", pTLUser->GetID());

                    nRetCode = CMessageHandler::GetInstance().DoMSDelMail(pTLUser->GetID(), pCmd->dwSerialID, pModifyRoleCmd->dwRoleID, *((UINT32*)pSlot->szData));
                    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to ms");

                    ++pModifyRoleCmd->nMailCount;
                    //LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "ms在线，不能删除邮件");
                }
                else
                {
                    DBG("TLUserID[%u] modify role db del mail", pTLUser->GetID());

                    nRetCode = CDBMgr::GetInstance().DelMail(*((UINT32*)pSlot->szData));
                    LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to execute timeout");
                    --pModifyRoleCmd->nSlotCount;

                    GM_LOG_BUFFER   buff;
                    snprintf(buff.szLog, GM_DATA_LOG_LENGTH, "[%d] Del Mail[%u]", GM_ROLE_MAIL, *((UINT32*)pSlot->szData));
                    pModifyRoleCmd->logList.push_back(buff);
                }
            }
                break;
            default:
                LOG_PROCESS_ERROR(FALSE);
                break;
            }

            ++pModifyRoleCmd->nSlotCount;
            nLength += pSlot->wLength;
        }

        if (pModifyRoleCmd->nMailCount == 0)
        {
            pModifyRoleCmd->nResult = errSuccess;
            pModifyRoleCmd->eCmdState = GM_LS_LOAD_ROLE;
        }
        else
            pModifyRoleCmd->eCmdState = GM_LS_TIMEOUT;

    }
        break;
    case GM_LS_MS_DEL_MAIL_ACK:
    {
        DBG("Receive MAIL %d", pModifyRoleCmd->nMailCount);
        if (pModifyRoleCmd->nMailCount == 0)
        {
            pCmd->eCmdState = GM_LS_LOAD_ROLE;
            pCmd->nResult = errSuccess;
        }
        else
        {
            pCmd->eCmdState = GM_LS_TIMEOUT;
        }
    }
        break;
    case GM_LS_LOAD_ROLE:
    {
        pModifyRoleCmd->poRole = pModifyRoleCmd->poSelfRole;
        pModifyRoleCmd->poSelfRole = NULL;

        nRetCode = CDBMgr::GetInstance().LoadRole(pModifyRoleCmd->dwSerialID, pModifyRoleCmd->dwRoleID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query role from database");

        pModifyRoleCmd->eCmdState = GM_LS_TIMEOUT;
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] modify role timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Failed to execute timeout");
        }
    }
        break;
    case GM_LS_LOAD_ROLE_ACK:
    {
        DBG("TLUserID[%u] modify role Result[%d]", pTLUser->GetID(), pModifyRoleCmd->nResult);
        LOG_ECHO_ERROR(pModifyRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load role data");

        if (pModifyRoleCmd->dwTargetRoleID == 0)
        {
            pModifyRoleCmd->poSelfRole = pModifyRoleCmd->poRole;
            pModifyRoleCmd->eCmdState = GM_LS_MODIFY_ROLE;
            pModifyRoleCmd->nResult = errSuccess;
        }
        else
        {
            if (pModifyRoleCmd->poSelfRole == NULL)
            {
                pModifyRoleCmd->poSelfRole = pModifyRoleCmd->poRole;
                pModifyRoleCmd->poRole = pModifyRoleCmd->poTargetRole;

                nRetCode = CDBMgr::GetInstance().LoadRole(pModifyRoleCmd->dwSerialID, pModifyRoleCmd->dwTargetRoleID);
                LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query role from database");
                pModifyRoleCmd->eCmdState = GM_LS_TIMEOUT;
            }
            else
            {
                pModifyRoleCmd->eCmdState = GM_LS_MODIFY_ROLE;
                pModifyRoleCmd->nResult = errSuccess;
            }
        }
    }
        break;
    case GM_LS_MODIFY_ROLE:
    {
        LOG_ECHO_ERROR(pModifyRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load target role");

        // modify role
        nRetCode = ModifyRoleData(pModifyRoleCmd);
        LOG_ECHO_ERROR(nRetCode == errSuccess, pTLUser->GetSessionID(), "Failed to modify role data");

        pModifyRoleCmd->eCmdState = GM_LS_MODIFY_ROLE_ACK;
        pModifyRoleCmd->nResult = errSuccess;
        DBG("modify role data success.");
    }
        break;
    case GM_LS_MODIFY_ROLE_ACK:
    {
        DBG("TLUserID[%u] modify role Result[%d]", pTLUser->GetID(), pModifyRoleCmd->nResult);

        LOG_ECHO_ERROR(pModifyRoleCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to modify role data");

        for (CMD_MODIFY_ROLE::LOG_LIST::iterator iter = pModifyRoleCmd->logList.begin(); iter != pModifyRoleCmd->logList.end(); ++iter)
        {
            nRetCode = CGMLog::GetInstance().WriteLog(pModifyRoleCmd->szUserName, "modify role", "Role[%u] %s Reason[%s]",
                pModifyRoleCmd->dwRoleID, iter->szLog, pModifyRoleCmd->szReason);
            LOG_CHECK_ERROR(nRetCode);
        }

        nRetCode = CGMRoleMgr::GetInstance().DelRole(pModifyRoleCmd->dwRoleID, pModifyRoleCmd->dwTLUserID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to unlock role");

        if (pModifyRoleCmd->dwTargetRoleID)
        {
            nRetCode = CGMRoleMgr::GetInstance().DelRole(pModifyRoleCmd->dwTargetRoleID, pModifyRoleCmd->dwTLUserID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to unlock target role");
        }

        nRetCode = CMessageHandler::GetInstance().DoTLModifyRoleDataAck(pTLUser->GetSessionID(), pModifyRoleCmd->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Failed to modify role data");

        if (pModifyRoleCmd->poSelfRole)
        {
            pModifyRoleCmd->poSelfRole->UnInit();
            CGameWorld::GetInstance().DelRole(pModifyRoleCmd->poSelfRole);
            pModifyRoleCmd->poSelfRole = NULL;
        }
        if (pModifyRoleCmd->poTargetRole)
        {
            pModifyRoleCmd->poTargetRole->UnInit();
            CGameWorld::GetInstance().DelRole(pModifyRoleCmd->poTargetRole);
            pModifyRoleCmd->poTargetRole = NULL;
        }
        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    if (pModifyRoleCmd)
    {
        if (pModifyRoleCmd->poSelfRole)
        {
            pModifyRoleCmd->poSelfRole->UnInit();
            CGameWorld::GetInstance().DelRole(pModifyRoleCmd->poSelfRole);
            pModifyRoleCmd->poSelfRole = NULL;
        }
        if (pModifyRoleCmd->poTargetRole)
        {
            pModifyRoleCmd->poTargetRole->UnInit();
            CGameWorld::GetInstance().DelRole(pModifyRoleCmd->poTargetRole);
            pModifyRoleCmd->poTargetRole = NULL;
        }

        nRetCode = CGMRoleMgr::GetInstance().DelRole(pModifyRoleCmd->dwRoleID, pModifyRoleCmd->dwTLUserID);
        LOG_CHECK_ERROR(nRetCode);

        if (pModifyRoleCmd->dwTargetRoleID)
        {
            nRetCode = CGMRoleMgr::GetInstance().DelRole(pModifyRoleCmd->dwTargetRoleID, pModifyRoleCmd->dwTLUserID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}


BOOL CB_QueryNameToID(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;

    CMD_QUERY_NAME_TO_ID   *pQueryCmd = (CMD_QUERY_NAME_TO_ID*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_QUERY_NAME_LIST:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        nRetCode = CDBMgr::GetInstance().QueryNameToID(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query name to ID from database");
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] query name id list timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Failed to query name to ID from database");
        }
    }
        break;
    case GM_LS_QUERY_NAME_LIST_ACK:
    {
        DBG("TLUserID[%u] query name id list Result[%d]", pTLUser->GetID(), pQueryCmd->nResult);

        LOG_ECHO_ERROR(pQueryCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to query name to ID from database");

        nRetCode = CMessageHandler::GetInstance().DoTLQueryNameToIDAck(pTLUser->GetSessionID(), pQueryCmd->nIndex, pQueryCmd->dwRoleID);
        LOG_CHECK_ERROR(nRetCode);

        //nRetCode = CGMLog::GetInstance().WriteLog(pQueryCmd->szUserName, "query name to id", "Name[%s]", pQueryCmd->szRoleName);
        //LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Bulk character ID query successful.");

        //pTLUser->ClearCmd();
        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}


BOOL CB_RoleExeCmd(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;

    CMD_ROLE_EXE_GM_CMD   *pRoleExeCmd = (CMD_ROLE_EXE_GM_CMD*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_MS_CHECK_ROLE_FREEZE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] role exe gm cmd send msg to ms to check role online", pTLUser->GetID());

            nRetCode = CMessageHandler::GetInstance().DoCheckRoleFreeze(pTLUser->GetID(), pCmd->dwSerialID, pRoleExeCmd->dwRoleID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS");
        }
        else
        {
            DBG("TLUserID[%u] role exe gm cmd ms not online, db check role online", pTLUser->GetID());

            nRetCode = CDBMgr::GetInstance().CheckRoleFreeze(pRoleExeCmd);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to check role freeze");

            pCmd->eCmdState = GM_LS_MS_CHECK_ROLE_FREEZE_ACK;
            pRoleExeCmd->nResult = errSuccess;
        }
    }
        break;
    case GM_LS_MS_CHECK_ROLE_FREEZE_ACK:
    {
        LOG_ECHO_ERROR(pRoleExeCmd->nResult != errGMRoleNotFreeze, pTLUser->GetSessionID(), "Failed to check role freeze");
        LOG_ECHO_ERROR(pRoleExeCmd->nResult != errGMRoleFreezeLessOneHour, pTLUser->GetSessionID(), "Failed to check role freeze");

        LOG_ECHO_ERROR(pRoleExeCmd->nResult != errGMRoleOnline, pTLUser->GetSessionID(), "Character is online.");
        LOG_ECHO_ERROR(pRoleExeCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Other errors");

        pCmd->eCmdState = GM_LS_MS_DEL_ROLE_CACHE;
    }
        break;
    case GM_LS_MS_DEL_ROLE_CACHE:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        if (CGMSessionMgr::GetInstance().IsMSOnline())
        {
            DBG("TLUserID[%u] role exe gm cmd send msg to ms, clear role cache", pTLUser->GetID());

            nRetCode = CMessageHandler::GetInstance().DoMSClearRoleCache(pRoleExeCmd->dwTLUserID, pRoleExeCmd->dwRoleID, pRoleExeCmd->dwSerialID);
            LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to send message to MS");
        }
        else
        {
            DBG("TLUserID[%u] role exe gm cmd ms not online, clear role cache", pTLUser->GetID());

            pCmd->eCmdState = GM_LS_MS_DEL_ROLE_CACHE_ACK;
            pRoleExeCmd->nResult = errSuccess;
        }
    }
        break;
    case GM_LS_MS_DEL_ROLE_CACHE_ACK:
    {
        DBG("TLUserID[%u] role exe gm cmd clear role cache Result[%d]", pTLUser->GetID(), pRoleExeCmd->nResult);

        LOG_ECHO_ERROR(pRoleExeCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to clear role cache data");

        nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pRoleExeCmd->dwRoleID, pRoleExeCmd->dwTLUserID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Character has been loaded by another user");

        pCmd->eCmdState = GM_LS_LOAD_ROLE;
    }
        break;
    case GM_LS_LOAD_ROLE:
    {
        nRetCode = CDBMgr::GetInstance().LoadRole(pRoleExeCmd->dwSerialID, pRoleExeCmd->dwRoleID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database");

        DBG("TLUserID[%u] role exe gm cmd ExeSuccess", pTLUser->GetID());
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] role exe gm cmd timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Failed to execute command within timeout");
        }
    }
        break;
    case GM_LS_LOAD_ROLE_ACK:
    {
        DBG("TLUserID[%u] role exe gm cmd Result[%d]", pTLUser->GetID(), pRoleExeCmd->nResult);

        LOG_ECHO_ERROR(pRoleExeCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to load role data");

        nRetCode = ExeLuaGMCmd(pRoleExeCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to execute GM command");

        nRetCode = CGMRoleMgr::GetInstance().CheckRoleLock(pRoleExeCmd->dwRoleID, pRoleExeCmd->dwTLUserID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Character has been loaded by another user");

        nRetCode = CGMRoleMgr::GetInstance().DelRole(pRoleExeCmd->dwRoleID, pRoleExeCmd->dwTLUserID);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to unlock character");

        LOG_CHECK_ERROR(pRoleExeCmd->poRole);
        if (pRoleExeCmd->poRole)
        {
            CGameWorld::GetInstance().DelRole(pRoleExeCmd->poRole);
            pRoleExeCmd->poRole = NULL;
        }

        DBG("TLUserID[%u] role exe gm cmd Del Role Cache OKay", pTLUser->GetID());

        nRetCode = CGMLog::GetInstance().WriteLog(pRoleExeCmd->szUserName, "role exe lua script", "RoleID[%u] GM[%s] Reason[%s]",
            pRoleExeCmd->dwRoleID, pRoleExeCmd->szGM, pRoleExeCmd->szReason);
        LOG_CHECK_ERROR(nRetCode);

        EchoTLError(pTLUser->GetSessionID(), "Failed to execute GM command");

        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;

    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    if (pRoleExeCmd && pRoleExeCmd->poRole)
    {
        CGameWorld::GetInstance().DelRole(pRoleExeCmd->poRole);
        pRoleExeCmd->poRole = NULL;
    }
    if (pRoleExeCmd)
    {
        nRetCode = CGMRoleMgr::GetInstance().DelRole(pRoleExeCmd->dwRoleID, pRoleExeCmd->dwTLUserID);
        LOG_CHECK_ERROR(nRetCode);
    }
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}

BOOL CB_QuerySysExchangeInfo(CTLUser *pTLUser, CMD_DATA_BASE *pCmd)
{
    INT32       nRetCode = 0;
    INT32       nResult = errGMUnknownError;
    UINT32      dwRoleID = 0;

    QUERY_SYSTEM_EXCHANGE_INFO   *pQueryCmd = (QUERY_SYSTEM_EXCHANGE_INFO*)pCmd;
    LOG_PROCESS_ERROR(pCmd);
    LOG_PROCESS_ERROR(pTLUser);

    switch (pCmd->eCmdState)
    {
    case GM_LS_SYS_EXCHANGE_ORDER:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;

        nRetCode = CDBMgr::GetInstance().QuerySysExchangeOrder(pQueryCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database");
    }
        break;
    case GM_LS_TIMEOUT:
    {
        if (time(NULL) > pCmd->nTimeout)
        {
            INF("TLUserID[%u] Query system exchange info timeout", pTLUser->GetID());
            LOG_ECHO_ERROR(FALSE, pTLUser->GetSessionID(), "Failed to execute command within timeout");
        }
    }
        break;
    case GM_LS_SYS_EXCHANGE_ORDER_ACK:
    {
        DBG("TLUserID[%u] query sys exchange order result[%d]", pTLUser->GetID(), pCmd->nResult);
        LOG_ECHO_ERROR(pCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to query database");
        pCmd->eCmdState = GM_LS_SYS_EXCHANGE_LOG;
    }
        break;
    case GM_LS_SYS_EXCHANGE_LOG:
    {
        pCmd->eCmdState = GM_LS_TIMEOUT;
        nRetCode = CDBMgr::GetInstance().QuerySysExchangeLog(pCmd);
        LOG_ECHO_ERROR(nRetCode, pTLUser->GetSessionID(), "Failed to query database");
    }
        break;
    case GM_LS_SYS_EXCHANGE_LOG_ACK:
    {
        DBG("TLUserID[%u] query sys exchange log result[%d]", pTLUser->GetID(), pCmd->nResult);
        LOG_ECHO_ERROR(pQueryCmd->nResult == errSuccess, pTLUser->GetSessionID(), "Failed to query database");

        nRetCode = CMessageHandler::GetInstance().DoTLQuerySystemExchangeInfo(pTLUser->GetSessionID(), 
            pQueryCmd->dwBeginDayTime, pQueryCmd->dwEndDayTime, 
            pQueryCmd->byGroupID, pQueryCmd->byFlag,
            pQueryCmd->llSysTotalMoney, pQueryCmd->llTotalMoney, 
            pQueryCmd->llSysTotalExchangeMoney, pQueryCmd->llTotalExchangeMoney, 
            pQueryCmd->llSysExchangeRMB, pQueryCmd->llExchangeRMB, 
            pQueryCmd->nAveragePriceToday, pQueryCmd->nAveragePriceLastHour);
        LOG_PROCESS_ERROR(nRetCode);

        //pTLUser->ClearCmd();
        CKernel::GetInstance().PopCmd(pCmd);
        CCmdMgr::GetInstance().DeleteCmd(pCmd);
        pCmd = NULL;
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    CKernel::GetInstance().PopCmd(pCmd);
    CCmdMgr::GetInstance().DeleteCmd(pCmd);
    return FALSE;
}
