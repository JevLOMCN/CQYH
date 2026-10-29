#include "stdafx.h"

#include "ms_session_mgr.h"
#include "l2m_message.h"
#include "m2l_message.h"
#include "common_message.h"

#include "manageserver.h"
#include "db/ms_sql_stmt_list.h"

#include "time_ex.h"
#include "message_handler.h"

using namespace InternalMessage;

void CMessageHandler::OnLSRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SLSSession* pLSSession{};
    L2M_REGISTER* pMsg{};
    INT32 nRetCode = 0;
    //check
    SLSSession& rLSSession = CMSSessionMgr::GetInstance().GetLSSession();
    LOG_PROCESS_ERROR( rLSSession.pSocketSession == pSocketSession );
    pLSSession = (SLSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR( pLSSession );
    LOG_PROCESS_ERROR( pLSSession->pSocketSession == pSocketSession );
    LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

    pMsg = (L2M_REGISTER*)pPacketBuffer;

    m_ServerInfo.LS.dwAddr = pMsg->dwAddr;
    m_ServerInfo.LS.wPort = pMsg->wPort;

    nRetCode = DoLSRegisterAck();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = DoServerInfo();
    LOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void CMessageHandler::OnLSReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    m_ServerInfo.LS.byReady = true;

    INF("LS is ready");

    _CheckAllReady();
}

void CMessageHandler::OnLSCloseAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    INF("LS is closed");
}

// Callback function
BOOL _LoginQuery(std::list<void*>& ResultList, void* pUserData)
{
    if (!ResultList.empty())
    {
        return TRUE;
    }
    return FALSE;
}
void CMessageHandler::OnLSLoginQueryReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    BOOL bRet = FALSE;
    L2M_LOGIN_QUERY_REQ* pMsg = (L2M_LOGIN_QUERY_REQ*)pPacketBuffer;

    // MS queries the database for matching account credentials
    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(mysqlSelectLoginQuery, _LoginQuery, NULL, "ss", pMsg->szUserName, pMsg->szPassword);

    // Success = 1, failure = 0
    if (bRet == TRUE)
    {
        DoLSLoginQueryAck(pMsg->dwClientSessionID, 1, pMsg->szUserName);
    }
    else
    {
        DoLSLoginQueryAck(pMsg->dwClientSessionID, 0, pMsg->szUserName);
    }

Exit0:
    return;
}

BOOL CMessageHandler::DoLSRegisterAck(void)
{
    INT32 nRetCode = 0;
    M2L_REGISTER_ACK* pMsg = (M2L_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2L_REGISTER_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2l_register_ack;

    nRetCode = _SendLS((PACKET_BUFFER*)pMsg, sizeof(M2L_REGISTER_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CMessageHandler::DoLSLoginQueryAck(UINT32 dwClientSessionID, UINT32 dwError, CHAR* szUserName)
{
    INT32 nRetCode = 0;
    M2L_LOGIN_QUERY_ACK* pMsg = (M2L_LOGIN_QUERY_ACK*)m_pPacketAllocator->Alloc(sizeof(M2L_LOGIN_QUERY_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = m2l_login_query_ack;
    pMsg->dwClientSessionID = dwClientSessionID;
    pMsg->dwError = dwError;
    strxcpy(pMsg->szUserName, szUserName, USER_NAME_LEN);

    nRetCode = _SendLS((PACKET_BUFFER*)pMsg, sizeof(M2L_LOGIN_QUERY_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
