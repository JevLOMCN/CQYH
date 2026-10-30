#include "stdafx.h"
#include "kernel.h"
#include "tl_user.h"
#include "gm_session_mgr.h"
#include "user_manager.h"
#include "cmd_mgr.h"
#include "message_handler.h"
#include "gm_role_mgr.h"
#include "gmserver.h"
#include "server_config.h"
#include "gm_data.h"
#include "permission_mgr.h"

CKernel  CKernel::m_Instance;

CKernel::CKernel()
{
    m_CmdList.clear();
    m_nTicket = 1;
}

CKernel::~CKernel()
{

}

CKernel& CKernel::GetInstance()
{
    return m_Instance;
}

BOOL CKernel::PushCmd(CMD_DATA_BASE *pCmd)
{
    LOG_PROCESS_ERROR(pCmd);

    m_CmdList.push_back(pCmd);

Exit0:
    return TRUE;
}

BOOL CKernel::PopCmd(CMD_DATA_BASE *pCmd)
{
    CMD_LIST::iterator iter = m_CmdList.begin();
    LOG_PROCESS_ERROR(pCmd);

    while (iter != m_CmdList.end())
    {
        if (pCmd == (*iter))
        {
            m_CmdList.erase(iter);
            return TRUE;
        }
        else
            ++iter;
    }

    return TRUE;
Exit0:
    return FALSE;
}

CMD_DATA_BASE * CKernel::FindCmd(UINT32 dwSerialID)
{
    CMD_LIST::iterator iter = m_CmdList.begin();
    while (iter != m_CmdList.end())
    {
        if (dwSerialID == (*iter)->dwSerialID)
        {
            return *iter;
        }
        ++iter;
    }

Exit0:
    return NULL;
}

BOOL CKernel::MainLoop()
{
    BOOL        bRet = FALSE;
    INT32       nRetCode = 0;
    CMD_DATA_BASE   *pCmd = NULL;
    UINT64      qwTickCount = GetTickCount64();

    if (qwTickCount / 250 != m_nTicket)
    {
        m_nTicket = qwTickCount / 250;

        CMD_LIST::iterator iter = m_CmdList.begin();
        while (iter != m_CmdList.end())
        {
            CTLUser     *pTLUser = NULL;
            CMD_LIST::iterator  old_iter = iter;
            ++iter;

            pTLUser = CUserManager::GetInstance().FindUserByID((*old_iter)->dwTLUserID);
            if (pTLUser == NULL)
            {
                pCmd = *old_iter;
                nRetCode = CKernel::GetInstance().PopCmd(pCmd);
                LOG_CHECK_ERROR(nRetCode);
                CCmdMgr::GetInstance().DeleteCmd(pCmd);
                continue;
            }

            nRetCode = CPermissionMgr::GetInstance().CheckExecutePermission((*old_iter)->eCmdType, pTLUser->GetPermission());
            if (nRetCode == FALSE)  // Default code used "="; it appears to have been a mistake and was changed to "=="
            {
                CMessageHandler::GetInstance().DoTLErrorStr(pTLUser->GetSessionID(), "Insufficient permissions.");
                pCmd = *old_iter;
                nRetCode = CKernel::GetInstance().PopCmd(pCmd);
                LOG_CHECK_ERROR(nRetCode);
                CCmdMgr::GetInstance().DeleteCmd(pCmd);
                continue;
            }

            bRet = (*old_iter)->pfunc(pTLUser, *old_iter);
            LOG_CHECK_ERROR(bRet);
        }
    }

    return TRUE;
}
