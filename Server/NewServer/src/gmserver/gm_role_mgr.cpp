#include "stdafx.h"
#include "gm_role_mgr.h"
#include "gm_session_mgr.h"
#include "message_handler.h"
#include "role.h"
#include "game_world.h"
#include "server_config.h"
#include <time.h>

CGMRoleMgr   CGMRoleMgr::m_Instance;

CGMRoleMgr::CGMRoleMgr()
{

}

CGMRoleMgr::~CGMRoleMgr()
{

}

CGMRoleMgr& CGMRoleMgr::GetInstance()
{
    return m_Instance;
}

BOOL CGMRoleMgr::Init()
{
    INT32   nRetCode = 0;

    m_RoleMap.clear();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMRoleMgr::UnInit()
{
    INT32       nRetCode = 0;

    m_RoleMap.clear();

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMRoleMgr::CheckRoleExist(UINT32 dwRoleID)
{
    ROLE_MAP::iterator  iter = m_RoleMap.find(dwRoleID);
    PROCESS_ERROR(iter != m_RoleMap.end());

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMRoleMgr::CheckRoleLock(UINT32 dwRoleID, UINT32 dwTLUserID)
{
    ROLE_MAP::iterator  iter = m_RoleMap.find(dwRoleID);
    PROCESS_ERROR(iter != m_RoleMap.end());
    PROCESS_ERROR(iter->second.dwTLUserID == dwTLUserID);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMRoleMgr::AddRole(UINT32 dwRoleID, UINT32 dwTLUserID)
{
    INT32       nRetCode = 0;
    GM_LOCK_DATA        data;
    ROLE_MAP::iterator  iter = m_RoleMap.find(dwRoleID);
    PROCESS_ERROR(iter == m_RoleMap.end());

    data.dwRoleID = dwRoleID;
    data.dwTLUserID = dwTLUserID;

    m_RoleMap.insert(std::make_pair(dwRoleID, data));

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMRoleMgr::DelRole(UINT32 dwRoleID, UINT32 dwTLUserID)
{
    ROLE_MAP::iterator  iter = m_RoleMap.find(dwRoleID);
    PROCESS_SUCCESS(iter == m_RoleMap.end());
    PROCESS_ERROR(iter->second.dwTLUserID == dwTLUserID);

    m_RoleMap.erase(dwRoleID);

Exit1:
Exit0 :
    return TRUE;
}

BOOL CGMRoleMgr::DelRoleByUserID(UINT32 dwTLUserID)
{
    ROLE_MAP::iterator iter = m_RoleMap.begin();
    ROLE_MAP::iterator end_iter = m_RoleMap.end();

    for (; iter != end_iter; ++iter)
    {
        if (iter->second.dwTLUserID == dwTLUserID)
        {
            m_RoleMap.erase(iter);
            return TRUE;
        }
            
    }
    
    return TRUE;
}
