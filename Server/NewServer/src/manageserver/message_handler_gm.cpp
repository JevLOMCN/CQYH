#include "stdafx.h"
#include "string_def.h"
#include "message_handler.h"

#include "ms_session_mgr.h"

#include "./gm/gm_manager.h"
#include "script_mgr.h"
#include "role_data_mgr.h"

using namespace InternalMessage;

//BOOL CMessageHandler::DoGMLoginAck(UINT32 dwGMSessionID, INT32 nResult)
//{
//    INT32       nRetCode = 0;
//    M2GM_LOGIN_ACK  *pMsg = (M2GM_LOGIN_ACK *)m_pPacketAllocator->Alloc(sizeof(M2GM_LOGIN_ACK));
//    LOG_PROCESS_ERROR(pMsg);
//
//    pMsg->wMsg = m2gm_login_ack;
//    pMsg->nResult = nResult;
//
//    nRetCode = _SendGM(dwGMSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2GM_LOGIN_ACK));
//    LOG_PROCESS_ERROR(nRetCode);
//
//    return TRUE;
//Exit0:
//    if (pMsg)
//        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
//    return FALSE;
//}
//
//BOOL CMessageHandler::DoGMExecuteCommandAck(UINT32 dwGMSessionID, INT32 nResult)
//{
//    INT32   nRetCode = 0;
//    M2GM_GM_COMMAND_ACK *pMsg = (M2GM_GM_COMMAND_ACK*)m_pPacketAllocator->Alloc(sizeof(M2GM_GM_COMMAND_ACK));
//    LOG_PROCESS_ERROR(pMsg);
//
//    pMsg->wMsg = m2gm_gm_command_ack;
//    pMsg->nResult = nResult;
//
//    nRetCode = _SendGM(dwGMSessionID, (PACKET_BUFFER*)pMsg, sizeof(M2GM_GM_COMMAND_ACK));
//    LOG_PROCESS_ERROR(nRetCode);
//
//    return TRUE;
//Exit0:
//    if (pMsg)
//        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
//    return FALSE;
//}
//
//BOOL CMessageHandler::DoGMLogItemAck(UINT32 dwGMSessionID, INT32 nResult, DB_OP_LOG_ITEM_RESULT *res)
//{
//    INT32 nRetCode = 0;
//    INT32   length = sizeof(M2GM_GM_LOG_ITEM_ACK);
//    DB_ITEM_LOG_DATA   data;
//    //std::list<DB_ITEM_LOG_DATA>::iterator  iter;
//    
//    M2GM_GM_LOG_ITEM_ACK    *pMsg = (M2GM_GM_LOG_ITEM_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
//    LOG_PROCESS_ERROR(pMsg);
//
//    pMsg->wMsg = m2gm_gm_log_item_ack;
//    pMsg->nCount = 0;
//    pMsg->nResult = nResult;
//
//    if (res)
//    {
//        while (res->listItemLogData.size() != 0)
//        {
//            data = res->listItemLogData.front();
//
//            pMsg->info[pMsg->nCount].byAreaID = data.byAreaID;
//            pMsg->info[pMsg->nCount].byCount = data.byCount;
//            pMsg->info[pMsg->nCount].bySvrGroupID = data.bySvrGroupID;
//            pMsg->info[pMsg->nCount].dwSrcRoleID = data.dwSrcRoleID;
//            pMsg->info[pMsg->nCount].dwTargetRoleID = data.dwTargetRoleID;
//            pMsg->info[pMsg->nCount].dwTemplateID = data.dwTemplateID;
//            pMsg->info[pMsg->nCount].llGuid = data.llGuid;
//            pMsg->info[pMsg->nCount].nLogTime = data.nLogTime;
//            memcpy(pMsg->info[pMsg->nCount].szReason, data.szReason, sizeof(pMsg->info[pMsg->nCount].szReason));
//            memcpy(pMsg->info[pMsg->nCount].szActionType, data.szActionType, sizeof(pMsg->info[pMsg->nCount].szActionType));
//            ++pMsg->nCount;
//
//             res->listItemLogData.pop_front();
//             length += sizeof(LOG_ITEM_INFO);
//
//             if (pMsg->nCount == MSG_LOG_ITEM_COUNT)
//             {
//                 pMsg->nResult = 0xFFFFFFFF;
//
//                 nRetCode = _SendGM(dwGMSessionID, (PACKET_BUFFER*)pMsg, length);
//                 LOG_PROCESS_ERROR(nRetCode);
//
//                 pMsg = (M2GM_GM_LOG_ITEM_ACK*)m_pPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
//                 LOG_PROCESS_ERROR(pMsg);
//
//                 pMsg->wMsg = m2gm_gm_log_item_ack;
//                 pMsg->nCount = 0;
//                 pMsg->nResult = nResult;
//                 length = sizeof(M2GM_GM_LOG_ITEM_ACK);
//             }
//        }
//    }
//
//    nRetCode = _SendGM(dwGMSessionID, (PACKET_BUFFER*)pMsg, length);
//    LOG_PROCESS_ERROR(nRetCode);
//
//    return TRUE;
//Exit0:
//    if (pMsg)
//        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
//    return FALSE;
//
//}

// todo_tq 11 Allan Tao[25.11.2013] 
//void CMessageHandler::OnGMToolLogin(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
//{
//    INT32       nRetCode = 0;
//    INT32       nResult = errUnknownError;
//    GM_DATA     *pGMData = NULL;
//    SGMSession  *pSession = NULL;
//    UINT32      dwIP = 0;
//    UINT16      wPort = 0;
//    struct in_addr  tempAddr;
//    const char *    pcszIP = NULL;
//    GM2M_LOGIN  *pMsg = (GM2M_LOGIN*)pPacketBuffer;
//
//    pSession = (SGMSession*)pSocketSession->GetUserData();
//    LOG_PROCESS_ERROR(pSession);
//
//    pGMData = CGMManager::GetInstance().FindGMData(pSession->dwSessionID);
//    PROCESS_ERROR_RET_CODE(!pGMData, errGMReLogin);
//
//    nRetCode = CGMManager::GetInstance().CheckPassword(pMsg->szName, pMsg->szPwd);
//    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMPwdError);
//
//    nRetCode = pSocketSession->GetRemoteAddress(dwIP, wPort);
//    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMGetIPError);
//
//    tempAddr.s_addr = dwIP;
//    pcszIP = inet_ntoa(tempAddr);
//    
//    nRetCode = CGMManager::GetInstance().CheckIP(pcszIP);
//    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMError);
//
//    nRetCode = CGMManager::GetInstance().AddGMData(pSession->dwSessionID, pcszIP, pMsg->szName);
//    LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMError);
//
//    nRetCode = DoGMLoginAck(pSession->dwSessionID, errSuccess);
//    LOG_CHECK_ERROR(nRetCode);
//
//    return ;
//Exit0:
//    nRetCode = DoGMLoginAck(pSession->dwSessionID, nResult);
//    LOG_CHECK_ERROR(nResult);
//    return ;
//}
//
//void CMessageHandler::OnGMToolExecuteCommand(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
//{
//    INT32       nRetCode = 0;
//    INT32       nResult  = errUnknownError;
//    GM2M_GM_COMMAND* pMsg = (GM2M_GM_COMMAND*)pPacketBuffer;
//    CLuaScript* poScript = NULL;
//    GM_DATA     *pGMData = NULL;
//    SGMSession  *pSession = NULL;
//    INT32       nGMRet = 0;
//    
//    pSession = (SGMSession*)pSocketSession->GetUserData();
//    LOG_PROCESS_ERROR(pSession);
//
//    pGMData = CGMManager::GetInstance().FindGMData(pSession->dwSessionID);
//    LOG_PROCESS_ERROR_RET_CODE(pGMData, errGMError);
//
//
//    poScript = CScriptMgr::GetInstance().FindScript(MS_GM_SCRIPT_NAME);
//    LOG_PROCESS_ERROR(poScript);
//
//    nRetCode = poScript->CallFunction("OnMSGMCommand", "sdds:d", pMsg->szGMCommand, 0, 0, pGMData->szName, &nGMRet);
//    LOG_PROCESS_ERROR(nRetCode);
//    LOG_PROCESS_ERROR(nGMRet == errSuccess || errGSExecute == nGMRet);
//
//    if (errSuccess == nGMRet)
//    {
//        nRetCode = DoDLGmCommand(g_ServerConfig.LS.nAreaID, 0, pGMData->szName,
//            time(NULL), g_ServerConfig.LS.nAreaID, pMsg->szGMCommand, TRUE);
//        LOG_CHECK_ERROR(nRetCode);
//        DBG("role %s executed gm command %s", pGMData->szName, pMsg->szGMCommand);
//    }
//
//    return;
//Exit0:
//    if (pSession)
//    {
//        nRetCode = DoGMExecuteCommandAck(pSession->dwSessionID, nResult);
//        LOG_CHECK_ERROR(nRetCode);
//    }
//    
//    if (pGMData)
//    {
//        DBG("role %s failed to execute gm command %s", pGMData->szName, pMsg->szGMCommand);
//        nRetCode = DoDLGmCommand(g_ServerConfig.LS.nAreaID, 0, pGMData->szName, 
//            time(NULL), g_ServerConfig.LS.nAreaID, pMsg->szGMCommand, FALSE);
//        LOG_CHECK_ERROR(nRetCode);
//
//    }
//    return;
//}
//
//void CMessageHandler::OnGMLogItem(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession)
//{
//    INT32   nRetCode = 0;
//    INT32   nResult = errUnknownError;
//    GM2M_GM_LOG_ITEM    *pMsg = (GM2M_GM_LOG_ITEM*)pPacketBuffer;
//    MS_ROLE_DATA    *pstRoleData = NULL;
//    SGMSession  *pSession = NULL;
//    UINT32      dwRoleID = 0xFFFFFFFF;
//    UINT64      llItemGuid = 0xFFFFFFFFFFFFFFFF;
//    UINT32      dwItemTemplateID = 0xFFFFFFFF;
//
//    LOG_PROCESS_ERROR(pMsg);
//
//    pSession = (SGMSession*)pSocketSession->GetUserData();
//    LOG_PROCESS_ERROR(pSession);
//
//    if (GM_ROLE_ACCOUNT == pMsg->nRoleType)
//    {
//    }
//    else if (GM_ROLE_NAME == pMsg->nRoleType)
//    {
//        pstRoleData = CRoleDataMgr::GetInstance().FindRole(pMsg->szRole);
//        LOG_CHECK_ERROR(pstRoleData);
//        if (pstRoleData)
//        {
//            dwRoleID = pstRoleData->dwRoleID;
//        }
//    }
//    else if (GM_ROLE_ID == pMsg->nRoleType)
//    {
//        dwRoleID = pMsg->dwRoleID;
//    }
//
//    if (GM_ITEM_GUID == pMsg->nItemType)
//    {
//        llItemGuid = pMsg->llItem;
//    }
//    else if (GM_ITEM_TEMPLATE_ID == pMsg->nItemType)
//    {
//        dwItemTemplateID = pMsg->llItem;
//    }
//
//
//    if (dwRoleID != 0xFFFFFFFF)
//    {
//        if (llItemGuid != 0xFFFFFFFFFFFFFFFF)
//        {
//            nRetCode = CManageServer::GetInstance().m_GMToolThread.LoadItemLogWithRoleGuid(pSession->dwSessionID, dwRoleID, llItemGuid);
//            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMGetLogFailed);
//        }
//        else if (dwItemTemplateID != 0xFFFFFFFF)
//        {
//            nRetCode = CManageServer::GetInstance().m_GMToolThread.LoadItemLogWithRoleTempID(pSession->dwSessionID, dwRoleID, dwItemTemplateID);
//            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMGetLogFailed);
//        }
//        else
//        {
//            nRetCode = CManageServer::GetInstance().m_GMToolThread.LoadItemLogWithRole(pSession->dwSessionID, dwRoleID);
//            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMGetLogFailed);
//        }
//    }
//    else 
//    {
//        if (llItemGuid != 0xFFFFFFFFFFFFFFFF)
//        {
//            nRetCode = CManageServer::GetInstance().m_GMToolThread.LoadItemLogWithGuid(pSession->dwSessionID, llItemGuid);
//            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMGetLogFailed);
//        }
//        else if (dwItemTemplateID != 0xFFFFFFFF)
//        {
//            nRetCode = CManageServer::GetInstance().m_GMToolThread.LoadItemLogWithTempID(pSession->dwSessionID, dwItemTemplateID);
//            LOG_PROCESS_ERROR_RET_CODE(nRetCode, errGMGetLogFailed);
//        }
//        else
//        {
//            LOG_PROCESS_ERROR_RET_CODE(FALSE, errGMGetLogParamError);
//        }
//    }
//
//    return;
//Exit0:
//    if (pSession)
//    {
//        DoGMLogItemAck(pSession->dwSessionID, nResult, NULL);
//    }
//    
//    return;
//}
//
//void CMessageHandler::OnGMToolTest(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
//{
//    INT32       nRetCode = 0;
//    INT32       nResult  = errUnknownError;
//
//
//    INF("---        ---");
//    INF(" -          - ");
//    INF("      ||      ");
//    INF("      --      ");
//    INF("    -____-    ");
//
//
//
//}
