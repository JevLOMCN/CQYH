#include "stdafx.h"
#include "user_manager.h"
#include "gm_svr_def.h"
#include "permission_mgr.h"
#include "gm_log.h"
#include "gm_role_mgr.h"
#include "ld_listener.h"

CUserManager CUserManager::m_Instance;

CUserManager  &CUserManager::GetInstance()
{
    return m_Instance;
}

CUserManager::CUserManager()
{

}

CUserManager::~CUserManager()
{

}

BOOL CUserManager::Init()
{
    INT32   nRetCode = 0;

    nRetCode = m_UserPool.Init(MAX_GM_TL_SESSION_COUNT);
    LOG_PROCESS_ERROR(nRetCode);

    m_nLDNum = 0;
    memset(m_LDUserList, 0, sizeof(m_LDUserList));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CUserManager::UnInit()
{
    INT32       nRetCode = 0;

    nRetCode = m_UserPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    m_nLDNum = 0;
    memset(m_LDUserList, 0, sizeof(m_LDUserList));

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CUserManager::LoginUser(UINT32 dwTLSessionID, UINT32 dwIP, const char *pcszName, const char *pcszPwd)
{
    INT32       nResult = errUnknownError;
    INT32       nRetCode = 0;
    INT32       nPermission = 0;
    UINT32      dwTLUserID = 0;
    CTLUser     *pTLUser = NULL;
    BOOL        bIsLD = FALSE;

    LOG_PROCESS_ERROR(pcszName);
    LOG_PROCESS_ERROR(pcszPwd);
    LOG_PROCESS_ERROR(dwTLSessionID < MAX_GM_TL_SESSION_COUNT);

    nPermission = CPermissionMgr::GetInstance().GetUserPermission(pcszName, pcszPwd);
    LOG_PROCESS_ERROR_RET_CODE(0 != nPermission, errGMGetPermissionError);

    dwTLUserID = CPermissionMgr::GetInstance().GetUserID(pcszName, pcszPwd);
    LOG_PROCESS_ERROR_RET_CODE(0 != dwTLUserID, errGMUserNotFind);

    bIsLD = CPermissionMgr::GetInstance().IsLD(pcszName, pcszPwd);

    pTLUser = m_UserPool.New(dwTLUserID);
    LOG_PROCESS_ERROR(pTLUser);

    pTLUser->Init();
    pTLUser->SetIP(dwIP);
    pTLUser->SetSessionID(dwTLSessionID);
    pTLUser->SetID(dwTLUserID);
    pTLUser->SetPermission((GM_PERMISSION)nPermission);
    pTLUser->SetUserName(pcszName);
    if (bIsLD == TRUE)
    {
        pTLUser->SetLD();
    }
        
    return errSuccess;
Exit0:
    return nResult;
}

BOOL CUserManager::LogoutUser(UINT32 dwTLUserID)
{
    INT32       nRetCode = 0;
    CTLUser     *pTLUser = NULL;

    //LOG_PROCESS_ERROR(dwTLUserID < MAX_GM_TL_SESSION_COUNT);

    nRetCode = CGMRoleMgr::GetInstance().DelRoleByUserID(dwTLUserID);
    LOG_CHECK_ERROR(nRetCode);

    pTLUser = FindUserByID(dwTLUserID);
    LOG_CHECK_ERROR(pTLUser);

    if (pTLUser)
    {
        INF("TL Logout.SessionID[%u], UserName[%s] UserID[%u]", pTLUser->GetSessionID(), pTLUser->GetUserName(), pTLUser->GetID());

        if (pTLUser->IsLD() == TRUE)
            CLDListener::GetInstance().UnRegisterGroupID(pTLUser->GetID());

        m_UserPool.Delete(pTLUser);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CUserManager::_IsExistLD(CTLUser *pTLUser)
{
    PROCESS_ERROR(pTLUser);

    for (INT32 i = 0; i < m_nLDNum; ++i)
    {
        if (m_LDUserList[i] == pTLUser)
            return TRUE;
    }

Exit0:
    return FALSE;
}

void CUserManager::_AddLDUser(CTLUser *pTLUser)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = _IsExistLD(pTLUser);
    PROCESS_ERROR(nRetCode == FALSE);

    m_LDUserList[m_nLDNum++] = pTLUser;
Exit0:
    return;
}

void CUserManager::_DelLDUser(CTLUser *pTLUser)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pTLUser);

    nRetCode = _IsExistLD(pTLUser);
    PROCESS_ERROR(nRetCode == TRUE);

    for (INT32 i = 0; i < m_nLDNum; ++i)
    {
        if (m_LDUserList[i] == pTLUser)
        {
            m_LDUserList[i] = m_LDUserList[m_nLDNum - 1];
            --m_nLDNum;
            break;
        }
    }

Exit0:
    return;
}
