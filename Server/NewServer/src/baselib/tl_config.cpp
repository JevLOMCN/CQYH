#include "stdafx.h"
#include "sol_bindings.h"
#include "tl_config.h"
#include "script_mgr.h"
#include "string_ex.h"
#include "script/sol_tl_config.h"

TL_CONFIG   g_TLConfig;
static char szLoadFileName[256] = {0};
static char szSaveFilePath[256] = {0};

BOOL LoadTLConfig(const char *pcszFileName)
{
    INT32   nRetCode = 0;
    CLuaScript  oScript;
    INT32   nStackTop = 0;

    nRetCode = oScript.Init();
    LOG_PROCESS_ERROR(nRetCode);

    sol_tl_config_open(oScript.GetLuaState());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = oScript.LoadFromFile(pcszFileName);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = oScript.CallFunction("LoadFile", "");

    strxcpy(szLoadFileName, pcszFileName, 256);
    oScript.UnInit();
    return TRUE;
Exit0:
    oScript.UnInit();
    return FALSE;
}

BOOL LoadTLConfigFromBuffer(const char *pcScriptName, const char *pcBuffer, UINT32 dwSize)
{
    INT32   nRetCode = 0;
    CLuaScript  oScript;
    INT32   nStackTop = 0;

    nRetCode = oScript.Init();
    LOG_PROCESS_ERROR(nRetCode);

    sol_tl_config_open(oScript.GetLuaState());
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = oScript.LoadFromBuffer(pcScriptName, pcBuffer, dwSize);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = oScript.CallFunction("LoadFile", "");

    oScript.UnInit();
    return TRUE;
Exit0:
    oScript.UnInit();
    return FALSE;
}

GM_SVR *GetServer(INT32 nSvrID)
{
    for (INT32 i = 0; i < GM_MAX_SERVER_COUNT; ++i)
    {
        if (g_TLConfig.ServerList.arrGMList[i].nSvrID == nSvrID)
        {
            return &g_TLConfig.ServerList.arrGMList[i];
        }
    }

    return NULL;
}

