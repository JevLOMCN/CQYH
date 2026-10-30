#include "stdafx.h"
#include "permission_mgr.h"
#include "script_mgr.h"
#include <time.h>
#include "server_config.h"
#include "lua.h"

CPermissionMgr CPermissionMgr::m_Instance;

CPermissionMgr& CPermissionMgr::GetInstance()
{
    return m_Instance;
}

CPermissionMgr::CPermissionMgr()
{
    m_qwReloadGlobalConfigTime = 0;
}

CPermissionMgr::~CPermissionMgr()
{

}

INT32 GM_LoadPackage(lua_State* L)
{
    const CHAR * pcszPackageName = NULL;
    lua_CFunction pLoadPackage = NULL;

    LOG_PROCESS_ERROR(lua_gettop(L) > 0);

    pcszPackageName = lua_tostring(L, 1);
    LOG_PROCESS_ERROR(pcszPackageName);

    pLoadPackage = CScriptMgr::GetInstance().FindPackage(pcszPackageName);
    LOG_PROCESS_ERROR(pLoadPackage);

    return pLoadPackage(L);

Exit0:
    if (pcszPackageName)
        CRI("failed to load package %s", pcszPackageName);

    return 0;
}

BOOL CPermissionMgr::Init()
{
    INT32       nRetCode = 0;

    m_qwReloadGlobalConfigTime = _time64(NULL);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CPermissionMgr::UnInit()
{
    INT32       nRetCode = 0;

    nRetCode = m_LuaScript.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CPermissionMgr::CheckIP(UINT32 dwIP)
{
    INT32       nRetCode = 0;
    BOOL        bInWhiteList = FALSE;

    if (_time64(NULL) - m_qwReloadGlobalConfigTime > 600)
    {
        nRetCode = LoadGlobalConfig(g_szGlobalConfigFileName);
        LOG_PROCESS_ERROR(nRetCode);
    }
    
	for (INT32 nIndex = 1; nIndex < sizeof(g_ServerConfig.Common.IpWhiteList) / sizeof(g_ServerConfig.Common.IpWhiteList[0]); ++nIndex)
    {
		if (g_ServerConfig.Common.IpWhiteList[nIndex].szIP[0] == 0)
            break;

		UINT32 dwWhiteAddr = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szIP);
        if (dwWhiteAddr == INADDR_NONE)
            break;
		UINT32 dwWhiteMask = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szMask);

        if ((dwWhiteAddr & dwWhiteMask) == (dwIP & dwWhiteMask))
        {
            bInWhiteList = TRUE;
            break;
        }
    }

    return bInWhiteList;
Exit0:
    return FALSE;
}

INT32 CPermissionMgr::GetUserPermission(const char *pcszName, const char *pcszPwd)
{
    LOG_PROCESS_ERROR(pcszName);
    LOG_PROCESS_ERROR(pcszPwd);

    for (INT32 i = 1; i < g_ServerConfig.GM_PWD.nGMUserNum + 1; ++i)
    {
        if ((0 == strncmp(g_ServerConfig.GM_PWD.gmUserList[i].szName, pcszName, USER_NAME_LEN))
            && (0 == strncmp(g_ServerConfig.GM_PWD.gmUserList[i].szPwd, pcszPwd, GM_TOOL_PASSWORD_LEN)))
        {
            return g_ServerConfig.GM_PWD.gmUserList[i].nPermission;
        }
    }

    for (INT32 i = 1; i < g_ServerConfig.GM_PWD.nLDUserNum + 1; ++i)
    {
        if ((0 == strncmp(g_ServerConfig.GM_PWD.LDUserList[i].szName, pcszName, USER_NAME_LEN))
            && (0 == strncmp(g_ServerConfig.GM_PWD.LDUserList[i].szPwd, pcszPwd, GM_TOOL_PASSWORD_LEN)))
        {
            return g_ServerConfig.GM_PWD.LDUserList[i].nPermission;
        }
    }

    return GM_PERMISSION_GUEST;
Exit0:
    return GM_PERMISSION_GUEST;
}

UINT32 CPermissionMgr::GetUserID(const char *pcszName, const char *pcszPwd)
{
    LOG_PROCESS_ERROR(pcszName);
    LOG_PROCESS_ERROR(pcszPwd);

    for (INT32 i = 1; i < g_ServerConfig.GM_PWD.nGMUserNum + 1; ++i)
    {
        if ((0 == strncmp(g_ServerConfig.GM_PWD.gmUserList[i].szName, pcszName, USER_NAME_LEN))
            && (0 == strncmp(g_ServerConfig.GM_PWD.gmUserList[i].szPwd, pcszPwd, GM_TOOL_PASSWORD_LEN)))
        {
            return g_ServerConfig.GM_PWD.gmUserList[i].dwUserID;
        }
    }

    for (INT32 i = 1; i < g_ServerConfig.GM_PWD.nLDUserNum + 1; ++i)
    {
        if ((0 == strncmp(g_ServerConfig.GM_PWD.LDUserList[i].szName, pcszName, USER_NAME_LEN))
            && (0 == strncmp(g_ServerConfig.GM_PWD.LDUserList[i].szPwd, pcszPwd, GM_TOOL_PASSWORD_LEN)))
        {
            return g_ServerConfig.GM_PWD.LDUserList[i].dwUserID;
        }
    }

    return 0;
Exit0:
    return 0;
}

BOOL CPermissionMgr::IsLD(const char *pcszName, const char *pcszPwd)
{
    LOG_PROCESS_ERROR(pcszName);
    LOG_PROCESS_ERROR(pcszPwd);

    for (INT32 i = 1; i < g_ServerConfig.GM_PWD.nLDUserNum + 1; ++i)
    {
        if ((0 == strncmp(g_ServerConfig.GM_PWD.LDUserList[i].szName, pcszName, USER_NAME_LEN))
            && (0 == strncmp(g_ServerConfig.GM_PWD.LDUserList[i].szPwd, pcszPwd, GM_TOOL_PASSWORD_LEN)))
        {
            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

BOOL CPermissionMgr::CheckExecutePermission(GM_COMMAND eCmd, INT32 nTLUserPermission)
{
    INT32   nCmdPermission = _GetCmdPermission(eCmd);
    return nTLUserPermission >= nCmdPermission;
}


INT32 CPermissionMgr::_GetCmdPermission(GM_COMMAND eCmd)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(eCmd > GM_CMD_NONE && eCmd < GM_CMD_NUM);

    return g_ServerConfig.GM_PWD.gmCmdList[eCmd].nPermission;
Exit0:
    return GM_PERMISSION_A;
}
