#include "stdafx.h"
#include "gm_query_thread.h"
#include "time_ex.h"
#include "errmsg.h"
#include "error_def.h"
#include "db_mgr.h"
#include "gm_role_mgr.h"
#include <limits>

CGMQueryThread::CGMQueryThread()
{
    m_bServerOK = FALSE;
}

CGMQueryThread::~CGMQueryThread()
{
    UnInit();
}

BOOL CGMQueryThread::Init(UINT32 dwResultCount)
{
    if (m_bServerOK || m_bstart)
    {
        LOG_CHECK_ERROR(!m_bServerOK && !m_bstart);
        return FALSE;
    }
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(dwResultCount > 0 && dwResultCount <= (std::numeric_limits<UINT32>::max)() / 10);

    nRetCode = m_requests.Init(dwResultCount * 10);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_results.Init(dwResultCount);
    LOG_PROCESS_ERROR(nRetCode);

    m_bServerOK = TRUE;
    return TRUE;
Exit0:
    UnInit();
    return FALSE;
}

BOOL CGMQueryThread::UnInit()
{
    Wait();
    while (auto* request = static_cast<DB_OPERATION_REQUEST*>(m_requests.Pop()))
        delete request;
    while (auto* result = static_cast<DB_OPERATION_RESULT*>(m_results.Pop()))
        delete result;
    m_requests.UnInit();
    m_results.UnInit();
    m_bServerOK = FALSE;
    return TRUE;
}

BOOL CGMQueryThread::ReInit()
{
    return TRUE;
}

DB_OPERATION_RESULT* CGMQueryThread::GetResult()
{
    return (DB_OPERATION_RESULT*)m_results.Pop();
}

// log
//BOOL CGMQueryThread::LoadItemLogWithGuid(CMD_DATA_TEMP_BASE *pCmd, UINT64 llGuid)
//{
//    DB_OP_LOG_ITEM_GUID *req = new DB_OP_LOG_ITEM_GUID;
//    req->m_eOp = DB_OP_ITEM_LOG_GUID;
//    req->llItemID = llGuid;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//}
//
//BOOL CGMQueryThread::LoadItemLogWithTempID(CMD_DATA_TEMP_BASE *pCmd, UINT32 dwTemplateID)
//{
//    DB_OP_LOG_ITEM_TEMPLATE_ID  *req = new DB_OP_LOG_ITEM_TEMPLATE_ID;
//    req->m_eOp = DB_OP_ITEM_LOG_TEMPID;
//    req->dwTemplateID = dwTemplateID;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//
//}
//
//BOOL CGMQueryThread::LoadItemLogWithRoleGuid(CMD_DATA_TEMP_BASE *pCmd, UINT32 dwRoleID, UINT64 llGuid)
//{
//    DB_OP_LOG_ITEM_GUID_ROLE *req = new DB_OP_LOG_ITEM_GUID_ROLE;
//    req->m_eOp = DB_OP_ITEM_LOG_ROLE_GUID;
//    req->dwRoleID = dwRoleID;
//    req->llItemID = llGuid;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//}
//
//BOOL CGMQueryThread::LoadItemLogWithRoleTempID(CMD_DATA_TEMP_BASE *pCmd, UINT32 dwRoleID, UINT32 dwTemplateID)
//{
//    DB_OP_LOG_ITEM_TEMP_ID_ROLE *req = new DB_OP_LOG_ITEM_TEMP_ID_ROLE;
//    req->m_eOp = DB_OP_ITEM_LOG_ROLE_TEMPID;
//    req->dwRoleID = dwRoleID;
//    req->dwTemplateID = dwTemplateID;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//}
//
//BOOL CGMQueryThread::LoadItemLogWithRole(CMD_DATA_TEMP_BASE *pCmd, UINT32 dwRoleID)
//{
//    DB_OP_LOG_ITEM_ROLE *req = new DB_OP_LOG_ITEM_ROLE;
//    req->m_eOp = DB_OP_ITEM_LOG_ROLE;
//    req->dwRoleID = dwRoleID;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//}
//
//BOOL CGMQueryThread::ViewRoleData(CMD_DATA_TEMP_BASE *pCmd)
//{
//    DB_OP_GM_VIEW_ROLE_REQUEST  *req = NULL;
//    LOG_PROCESS_ERROR(pCmd);
//
//    CMD_DATA_VIEW_ROLE_INFO *pRoleCmd = (CMD_DATA_VIEW_ROLE_INFO*)pCmd;
//
//    req = new DB_OP_GM_VIEW_ROLE_REQUEST;
//    req->m_eOp = DB_OP_GM_VIEW_ROLE_REQ;
//    req->dwRoleID = pRoleCmd->dwRoleID;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//Exit0:
//    return FALSE;
//}
//
//BOOL CGMQueryThread::LoadRoleData(CMD_DATA_TEMP_BASE *pCmd)
//{
//    DB_OP_GM_LOAD_ROLE_REQUEST  *req = NULL;
//    LOG_PROCESS_ERROR(pCmd);
//
//    req = new DB_OP_GM_LOAD_ROLE_REQUEST;
//    req->m_eOp = DB_OP_GM_LOAD_ROLE_REQ;
//    req->dwRoleID = pCmd->dwRoleID;
//    req->pCmd = (CMD_DATA_TEMP_BASE *)pCmd;
//
//    return m_requests.Push(req);
//Exit0:
//    return FALSE;
//}
//
//BOOL CGMQueryThread::SaveRoleData(CMD_DATA_TEMP_BASE* pCmd)
//{
//    DB_OP_GM_SAVE_ROLE_REQUEST  *req = NULL;
//    LOG_PROCESS_ERROR(pCmd);
//
//    req = new DB_OP_GM_SAVE_ROLE_REQUEST;
//    LOG_PROCESS_ERROR(req);
//
//    req->m_eOp = DB_OP_GM_SAVE_ROLE_REQ;
//    req->pCmd = pCmd;
//
//    return m_requests.Push(req);
//Exit0:
//    return FALSE;
//}

void CGMQueryThread::ThrdProc()
{
    cqyh::MysqlThreadContext clientThread;
    if (!clientThread)
    {
        LOG_CHECK_ERROR(clientThread);
        return;
    }
    BOOL nRetCode = FALSE;

    for (; m_bstart; )
    {
        if (!m_bServerOK)
            ReInit();

        for (;;)
        {
            if (!m_bServerOK)
                break;

            const std::unique_ptr<DB_OPERATION_REQUEST> req(static_cast<DB_OPERATION_REQUEST*>(m_requests.Pop()));
            if (req == NULL)
                break;

            //if (NULL == req->pCmd)
                //break;

            auto pMysql = CDBMgr::GetInstance().GetMysql();
            LOG_CHECK_ERROR(pMysql);
            auto pLogMysql = CDBMgr::GetInstance().GetLogMysql();
            LOG_CHECK_ERROR(pLogMysql);
            if (NULL == pMysql || NULL == pLogMysql)
                continue;

            //switch (req->m_eOp)
            //{
            //case DB_OP_ITEM_LOG_GUID:
            //    {
            //        DB_OP_LOG_ITEM_RESULT   *pItemRes = new DB_OP_LOG_ITEM_RESULT;
            //        DB_OP_LOG_ITEM_GUID *pstGuidReq = (DB_OP_LOG_ITEM_GUID*)req;

            //        nRetCode = m_DBItemLogWithGuid.Init(pLogMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pItemRes->m_eOp = DB_OP_ITEM_LOG_RES;
            //            pItemRes->pCmd = pstGuidReq->pCmd;

            //            nRetCode = m_DBItemLogWithGuid.DoLoadItemLog(pstGuidReq->llItemID, pItemRes);
            //            pItemRes->m_nResult = _CheckSqlResult(nRetCode, m_DBItemLogWithGuid.m_stmt);

            //            m_results.Push(pItemRes);

            //            nRetCode = m_DBItemLogWithGuid.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_ITEM_LOG_TEMPID:
            //    {
            //        DB_OP_LOG_ITEM_RESULT   *pItemRes = new DB_OP_LOG_ITEM_RESULT;
            //        DB_OP_LOG_ITEM_TEMPLATE_ID *pTempIDReq = (DB_OP_LOG_ITEM_TEMPLATE_ID*)req;

            //        nRetCode = m_DBItemLogWithTempID.Init(pLogMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pItemRes->m_eOp = DB_OP_ITEM_LOG_RES;
            //            pItemRes->pCmd = pTempIDReq->pCmd;
            //            nRetCode = m_DBItemLogWithTempID.DoLoadItemLog(pTempIDReq->dwTemplateID, pItemRes);
            //            pItemRes->m_nResult = _CheckSqlResult(nRetCode, m_DBItemLogWithTempID.m_stmt);

            //            m_results.Push(pItemRes);

            //            nRetCode = m_DBItemLogWithGuid.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_ITEM_LOG_ROLE_GUID:
            //    {
            //        DB_OP_LOG_ITEM_RESULT   *pItemRes = new DB_OP_LOG_ITEM_RESULT;
            //        DB_OP_LOG_ITEM_GUID_ROLE *pItemGuidRoleReq = (DB_OP_LOG_ITEM_GUID_ROLE*)req;

            //        nRetCode = m_DBItemLogWithRoleGuid.Init(pLogMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pItemRes->m_eOp = DB_OP_ITEM_LOG_RES;
            //            pItemRes->pCmd = pItemGuidRoleReq->pCmd;
            //            nRetCode = m_DBItemLogWithRoleGuid.DoLoadItemLog(pItemGuidRoleReq->dwRoleID, pItemGuidRoleReq->llItemID, pItemRes);
            //            pItemRes->m_nResult = _CheckSqlResult(nRetCode, m_DBItemLogWithRoleGuid.m_stmt);

            //            m_results.Push(pItemRes);

            //            nRetCode = m_DBItemLogWithRoleGuid.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_ITEM_LOG_ROLE_TEMPID:
            //    {
            //        DB_OP_LOG_ITEM_RESULT   *pItemRes = new DB_OP_LOG_ITEM_RESULT;
            //        DB_OP_LOG_ITEM_TEMP_ID_ROLE *pItemRoleTempIDReq = (DB_OP_LOG_ITEM_TEMP_ID_ROLE*)req;

            //        nRetCode = m_DBItemLogWithRoleTempID.Init(pLogMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pItemRes->m_eOp = DB_OP_ITEM_LOG_RES;
            //            pItemRes->pCmd = pItemRoleTempIDReq->pCmd;
            //            nRetCode = m_DBItemLogWithRoleTempID.DoLoadItemLog(pItemRoleTempIDReq->dwRoleID, pItemRoleTempIDReq->dwTemplateID, pItemRes);
            //            pItemRes->m_nResult = _CheckSqlResult(nRetCode, m_DBItemLogWithRoleTempID.m_stmt);

            //            m_results.Push(pItemRes);

            //            nRetCode = m_DBItemLogWithRoleTempID.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_ITEM_LOG_ROLE:
            //    {
            //        DB_OP_LOG_ITEM_ROLE *pItemRoleReq = (DB_OP_LOG_ITEM_ROLE*)req;
            //        DB_OP_LOG_ITEM_RESULT   *pItemRes = new DB_OP_LOG_ITEM_RESULT;

            //        nRetCode = m_DBItemLogWithRole.Init(pLogMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pItemRes->m_eOp = DB_OP_ITEM_LOG_RES;
            //            pItemRes->pCmd = pItemRoleReq->pCmd;
            //            nRetCode = m_DBItemLogWithRole.DoLoadItemLog(pItemRoleReq->dwRoleID, pItemRes);
            //            pItemRes->m_nResult = _CheckSqlResult(nRetCode, m_DBItemLogWithRole.m_stmt);

            //            m_results.Push(pItemRes);

            //            nRetCode = m_DBItemLogWithRole.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_GM_VIEW_ROLE_REQ:
            //    {
            //        DB_OP_GM_VIEW_ROLE_REQUEST  *pRoleReq = (DB_OP_GM_VIEW_ROLE_REQUEST*)req;
            //        DB_OP_GM_VIEW_ROLE_RESULT   *pRoleRsp = new DB_OP_GM_VIEW_ROLE_RESULT;

            //        nRetCode = m_DBLoadRole.Init(pMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pRoleRsp->m_eOp = DB_OP_GM_VIEW_ROLE_RES;
            //            pRoleRsp->pCmd = pRoleReq->pCmd;

            //            m_DBLoadRole.m_dwRoleID = pRoleReq->dwRoleID;
            //            nRetCode = m_DBLoadRole.DoLoadRole(pRoleRsp->szRoleData, sizeof(pRoleRsp->szRoleData), pRoleRsp->dwLength);
            //            pRoleRsp->m_nResult = _CheckSqlResult(nRetCode, m_DBLoadRole.m_stmt);

            //            m_results.Push(pRoleRsp);

            //            nRetCode = m_DBLoadRole.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_GM_LOAD_ROLE_REQ:
            //    {
            //        DB_OP_GM_LOAD_ROLE_REQUEST  *pRoleReq = (DB_OP_GM_LOAD_ROLE_REQUEST*)req;
            //        DB_OP_GM_LOAD_ROLE_RESULT   *pRoleRsp = new DB_OP_GM_LOAD_ROLE_RESULT;

            //        nRetCode = m_DBLoadRole.Init(pMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pRoleRsp->m_eOp = DB_OP_GM_LOAD_ROLE_RES;
            //            pRoleRsp->pCmd = pRoleReq->pCmd;
            //            
            //            m_DBLoadRole.m_dwRoleID = pRoleReq->dwRoleID;
            //            nRetCode = m_DBLoadRole.DoLoadRole(pRoleRsp->szRoleData, sizeof(pRoleRsp->szRoleData), pRoleRsp->dwLength);
            //            pRoleRsp->m_nResult = _CheckSqlResult(nRetCode, m_DBLoadRole.m_stmt);

            //            m_results.Push(pRoleRsp);

            //            nRetCode = m_DBLoadRole.UnInit();
            //            LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //case DB_OP_GM_SAVE_ROLE_REQ:
            //    {
            //        DB_OP_GM_SAVE_ROLE_REQUEST  *pRoleReq = (DB_OP_GM_SAVE_ROLE_REQUEST*)req;
            //        DB_OP_GM_SAVE_ROLE_RESULT   *pRoleRsp = new DB_OP_GM_SAVE_ROLE_RESULT;

            //        nRetCode = m_DBSaveRole.Init(pMysql);
            //        LOG_CHECK_ERROR(nRetCode);
            //        if (nRetCode)
            //        {
            //            pRoleRsp->m_eOp = DB_OP_GM_SAVE_ROLE_RES;
            //            pRoleRsp->pCmd = pRoleReq->pCmd;

            //            CGMRoleData *pGMRole = CGMRoleMgr::GetInstance().FindRole(pRoleReq->pCmd->dwSvrID, pRoleReq->pCmd->dwRoleID);
            //            LOG_CHECK_ERROR(pGMRole);
            //            if (pGMRole && pGMRole->GetRoleData())
            //            {
            //                UINT32      dwLength = 0;
            //                char        szBuffer[MAX_ROLE_BUFFER_LENGTH] = {0};

            //                m_DBSaveRole.m_dwRoleID = pGMRole->GetRoleData()->GetObjID();

            //                nRetCode = pGMRole->GetRoleData()->Save(szBuffer, dwLength);
            //                LOG_CHECK_ERROR(nRetCode);
            //                if (nRetCode)
            //                {
            //                    nRetCode = m_DBSaveRole.DoSaveRole(szBuffer, dwLength);
            //                    pRoleRsp->m_nResult = _CheckSqlResult(nRetCode, m_DBSaveRole.m_stmt);

            //                    m_results.Push(pRoleRsp);
            //                }
            //            }

            //           nRetCode = m_DBSaveRole.UnInit();
            //           LOG_CHECK_ERROR(nRetCode);
            //        }
            //    }
            //    break;
            //default:
            //    LOG_CHECK_ERROR(FALSE);
            //}
        }

        WaitForStop(std::chrono::milliseconds(16));
    }
}

INT32 CGMQueryThread::_CheckSqlResult(BOOL nRetCode, MYSQL_STMT* stmt)
{
    if (nRetCode)
        return errSuccess;
    else
    {
        if (mysql_stmt_errno(stmt) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }
}
