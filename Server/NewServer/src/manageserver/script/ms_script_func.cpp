#include "stdafx.h"
#include "ms_script_func.h"

#include <time.h>

#include "string_def.h"
#include "server_config.h"
#include "time_ex.h"

#include "manageserver.h"
#include "role_data.h"
#include "onlinerole/online_role_data_mgr.h"
#include "client/usermgr.h"
#include "timer/time_trigger.h"
#include "message_handler.h"
#include "scene/scenemgr.h"
#include "ms_session_mgr.h"
#include "game_world_config.h"

extern char g_StringTable[STRING_TABLE_STR_COUNT][STRING_TABLE_STR_LEN];

struct LUA_TRIGGER_NODE : BASE_TRIGGER_NODE
{
	CLuaScript* pScript;
	char		szFuncName[SCRIPT_NAME_LEN];
	INT32		nInterval;
	INT32		nCount;
};

struct WAIT_TRIGGER_NODE : BASE_TRIGGER_NODE
{
	lua_State* L;
};

int lua_clear_string_table(lua_State* L)
{
    memset(g_StringTable, 0, sizeof(g_StringTable));
    return 0;
}

int lua_register_string(lua_State* L)
{
    UINT32 dwIndex = (INT32)lua_tonumber(L, 1);
    dwIndex = dwIndex - SERVER_STRING_START_ID;
    const char* szStr = lua_tostring(L, 2);
    LOG_CHECK_ERROR(szStr);

    LOG_PROCESS_ERROR( dwIndex < STRING_TABLE_STR_COUNT );
    LOG_PROCESS_ERROR( g_StringTable[dwIndex][0] == '\0' );

    strxcpy( g_StringTable[ dwIndex ], szStr, STRING_TABLE_STR_LEN);

    lua_pushboolean(L, TRUE);

    return 1;
Exit0:
    return 0;
}

int lua_call_gs_gm_func(lua_State* L)
{
    INT32   nRetCode = 0;
    UINT32  dwSelfID = lua_tonumber(L, 1);
    const char* pcszGmCommand = lua_tostring(L, 2);

    nRetCode = CMessageHandler::GetInstance().DoGSCallGMCommand(dwSelfID, pcszGmCommand);
    LOG_PROCESS_ERROR(nRetCode);

    lua_pushinteger(L, TRUE);
    return 1;
Exit0:
    lua_pushinteger(L, FALSE);
    return 1;
}

int lua_call_gs_func(lua_State* L)
{
	INT32   nRetCode = 0;
	UINT32  dwSelfID = lua_tonumber(L, 1);
	const char* pcszGmCommand = lua_tostring(L, 2);

	nRetCode = CMessageHandler::GetInstance().DoGSCallGMCommand(dwSelfID, pcszGmCommand);
	LOG_PROCESS_ERROR(nRetCode);

	lua_pushinteger(L, TRUE);
	return 1;
Exit0:
	lua_pushinteger(L, FALSE);
	return 1;
}

int lua_print_ms(lua_State* L)
{
    INT32   nRetCode = 0;
    const char* pcszLog = lua_tostring(L, 1);
    LOG_PROCESS_ERROR(pcszLog);

    DBG(pcszLog);

    lua_pushinteger(L, TRUE);
    return 1;
Exit0:
    lua_pushinteger(L, FALSE);
    return 1;
}

int lua_say(lua_State *L)
{
    INT32   nRetCode = 0;
    UINT32  dwTargetID = 0;
	INT32	nLocation = 0;
    const char *pcszText = NULL;
	INT32 nServerGroupID = 0;

    MS_ONLINE_ROLE_DATA *pstSourceRoleData = NULL;
    MS_ONLINE_ROLE_DATA *pstTargetRoleData = NULL;

    dwTargetID = lua_tonumber(L, 1);
	nLocation = lua_tonumber(L, 2);
    pcszText = lua_tostring(L, 3);
	LOG_PROCESS_ERROR(pcszText);
	nServerGroupID = lua_tonumber(L, 4);

    nRetCode = CMessageHandler::GetInstance().DoClientChatMessage("", 0, dwTargetID, 
		nLocation, pcszText, nServerGroupID, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

    return 0;
Exit0:
    return 0;
}

INT32 LuaTriggerCallback(BASE_TRIGGER_NODE *pTriggerNode)
{
	INT32 nRetCode = 0;
	LUA_TRIGGER_NODE* pNode = NULL;

	LOG_PROCESS_ERROR(pTriggerNode);

	pNode = (LUA_TRIGGER_NODE*)pTriggerNode;
	LOG_PROCESS_ERROR(pNode->pScript);

	nRetCode = pNode->pScript->CallFunction("lua_trigger_callback", "s", pNode->szFuncName);
	LOG_PROCESS_ERROR(nRetCode);

	pNode->nCount--;
	PROCESS_ERROR(pNode->nCount > 0);
	pNode->dwWashoutTime += pNode->nInterval;

	return TIME_TRIGGER_RET_CONTINUE;

Exit0:
	SAFE_DELETE(pTriggerNode);

	return TIME_TRIGGER_RET_END;
}

int lua_register_timer(lua_State* L)
{
	INT32 nRetCode = 0;
	struct tm StartTM;
	struct tm* pTriggerTM;
	time_t dwTriggerTime;
	time_t dwCurrentTime;
	INT32 nInterval = 0;
	INT32 nMaxCount = INT_MAX;
	const char* pcszCallbackFunc = NULL;
	LUA_TRIGGER_NODE* pNode = NULL;

	LOG_PROCESS_ERROR(lua_gettop(L) == 2);
	LOG_PROCESS_ERROR(lua_istable(L, 1));
	
	lua_pushstring(L, "year");
	lua_gettable(L, 1);
	StartTM.tm_year = lua_tonumber(L, 3) - 1900;
	lua_pop(L, 1);

	lua_pushstring(L, "month");
	lua_gettable(L, 1);
	StartTM.tm_mon = lua_tonumber(L, 3) - 1;
	lua_pop(L, 1);

	lua_pushstring(L, "day");
	lua_gettable(L, 1);
	StartTM.tm_mday = lua_tonumber(L, 3);
	lua_pop(L, 1);

	lua_pushstring(L, "hour");
	lua_gettable(L, 1);
	StartTM.tm_hour = lua_tonumber(L, 3);
	lua_pop(L, 1);

	lua_pushstring(L, "min");
	lua_gettable(L, 1);
	StartTM.tm_min = lua_tonumber(L, 3);
	lua_pop(L, 1);

	lua_pushstring(L, "sec");
	lua_gettable(L, 1);
	StartTM.tm_sec= lua_tonumber(L, 3);
	lua_pop(L, 1);

	lua_pushstring(L, "interval");
	lua_gettable(L, 1);
	nInterval = lua_tonumber(L, 3);
	lua_pop(L, 1);

	lua_pushstring(L, "maxcount");
	lua_gettable(L, 1);
	nMaxCount = lua_tonumber(L, 3);
	lua_pop(L, 1);

	pcszCallbackFunc = lua_tostring(L, 2);
	LOG_PROCESS_ERROR(pcszCallbackFunc);
	LOG_PROCESS_ERROR(strnlen(pcszCallbackFunc, SCRIPT_NAME_LEN) < SCRIPT_NAME_LEN);

	dwTriggerTime = mktime(&StartTM);
	dwCurrentTime = CManageServer::GetInstance().GetGameTime();

	while (dwTriggerTime < dwCurrentTime && nMaxCount--)
		dwTriggerTime += nInterval;

	pTriggerTM = localtime(&dwTriggerTime);
	LOG_PROCESS_ERROR(pTriggerTM);

	pNode = new LUA_TRIGGER_NODE;
	LOG_PROCESS_ERROR(pNode);

	pNode->pPrev = pNode->pNext = NULL;
	pNode->dwWashoutTime = dwTriggerTime;
	pNode->nTriggerType = tttLuaTimer;
	pNode->fp = LuaTriggerCallback;
	pNode->pScript = CLuaScript::GetScriptFromState(L);
	LOG_PROCESS_ERROR(pNode->pScript);

	strxcpy(pNode->szFuncName, pcszCallbackFunc, SCRIPT_NAME_LEN);
	pNode->nInterval = nInterval;
	pNode->nCount = nMaxCount;

	nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pNode);
	LOG_PROCESS_ERROR(nRetCode);

	lua_pushboolean(L, TRUE);

	pTriggerTM = localtime(&dwTriggerTime);
	LOG_PROCESS_ERROR(pTriggerTM);

	INF("registered lua time trigger %s at %d-%d-%d %02d:%02d:%02d interval %d count %d", pcszCallbackFunc,
		pTriggerTM->tm_year + 1900, pTriggerTM->tm_mon + 1, pTriggerTM->tm_mday, pTriggerTM->tm_hour, pTriggerTM->tm_min, pTriggerTM->tm_sec,
		nInterval, nMaxCount);

	return 1;
Exit0:
	if (pcszCallbackFunc)
		CRI("failed to register lua time trigger %s", pcszCallbackFunc);
	else
		CRI("failed to register lua time trigger");
	return 0;
}

int lua_clear_timer(lua_State* L)
{
	BASE_TRIGGER_NODE* pNode = CTimeTrigger::GetInstance().GetFirstTrigger(tttLuaTimer);
	while (pNode)
	{
		BASE_TRIGGER_NODE* pNextNode = CTimeTrigger::GetInstance().GetNextTrigger();

		CTimeTrigger::GetInstance().DeleteTrigger(pNode);
		SAFE_DELETE(pNode);
		
		pNode = pNextNode;
	}

	return 0;
}

INT32 WaitTriggerCallback(BASE_TRIGGER_NODE *pTriggerNode)
{
	INT32 nRetCode = 0;
	WAIT_TRIGGER_NODE* pNode = NULL;

	LOG_PROCESS_ERROR(pTriggerNode);

	pNode = (WAIT_TRIGGER_NODE*)pTriggerNode;
	LOG_PROCESS_ERROR(pNode->L);

	lua_resume(pNode->L, 0);

Exit0:
	SAFE_DELETE(pTriggerNode);

	return TIME_TRIGGER_RET_END;
}

int lua_wait(lua_State *L)
{
	INT32 nRetCode = 0;
	WAIT_TRIGGER_NODE* pNode = NULL;

	INT32 nWaitTime = (INT32)lua_tonumber(L, 1);
	
	pNode = new WAIT_TRIGGER_NODE;
	LOG_PROCESS_ERROR(pNode);

	pNode->pPrev = pNode->pNext = NULL;
	pNode->dwWashoutTime = CManageServer::GetInstance().GetGameTime() + nWaitTime;
	pNode->nTriggerType = tttLuaTimer;
	pNode->fp = WaitTriggerCallback;
	pNode->L = L;

	nRetCode = CTimeTrigger::GetInstance().InsertTrigger(pNode);
	LOG_PROCESS_ERROR(nRetCode);

	return lua_yield(L, 0);
Exit0:
	return 0;
}

int lua_kick_role(lua_State* L)
{
	MS_ONLINE_ROLE_DATA* pOnlineRoleData{};
	INT32 nRetCode = 0;
	const char* pcszRoleName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszRoleName);

	pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByName(pcszRoleName);
	LOG_PROCESS_ERROR(pOnlineRoleData);

	KICK_USER(pOnlineRoleData->dwCliSessionID, ktImmediately);

	lua_pushboolean(L, TRUE);
	return 1;
Exit0:
	lua_pushboolean(L, FALSE);
	return 1;
}

int lua_notify_guard(lua_State* L)
{
	INT32 nRetCode = 0;

	INT32 nLevel = lua_tonumber(L, 1);
	LOG_CHECK_ERROR(nLevel >= 0);

	const char* szStr = lua_tostring(L, 2);
	LOG_CHECK_ERROR(szStr);

	nRetCode = CMessageHandler::GetInstance().DoGDNotify(nLevel, szStr);
	LOG_PROCESS_ERROR( nRetCode );

	lua_pushboolean(L, TRUE);

	return 1;
Exit0:
	lua_pushboolean(L, FALSE);
	return 0;
}

int lua_kickall(lua_State* L)
{
	INT32 nRetCode = 0;
	INT32 nServerGroupID = lua_tonumber(L, 1);

	//struct TRAVERSE_KICK_ONLINE_ROLE
	//{
	//	BOOL operator()(UINT32 dwRoleID, MS_ONLINE_ROLE_DATA* pOnlineRole)
	//	{
	//		KICK_USER(pOnlineRole->dwCliSessionID, ktImmediately);
	//		return TRUE;
	//	}
	//} KickAllOnlineRole;

	struct TRAVERSE_KICK_USER_BY_GROUP
	{
		INT32 m_GroupID = -1;
		BOOL operator()(UINT32 dwRoleID, MS_USER* pUser)
		{
			INT32 nServerGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pUser->dwCliSessionID);
			if (m_GroupID == MAX_SERVER_GROUP_COUNT || m_GroupID == nServerGroupID)
			{
				KICK_USER(pUser->dwCliSessionID, ktImmediately);
			}
			return TRUE;
		}
	} KickAllUserByGroup;

	LOG_PROCESS_ERROR(nServerGroupID >= 0 && nServerGroupID < MAX_SERVER_GROUP_COUNT);

	//nRetCode = COnlineRoleDataMgr::GetInstance().TraverseOnlineRoleInGroup(KickAllOnlineRole, nServerGroupID);
	KickAllUserByGroup.m_GroupID = nServerGroupID;
	nRetCode = CUserMgr::GetInstance().TraverseUser(KickAllUserByGroup);
	LOG_PROCESS_ERROR(nRetCode);

	lua_pushboolean(L, TRUE);

	return 1;
Exit0:
	lua_pushboolean(L, FALSE);

	return 1;
}

INT32 lua_switch_server_state(lua_State* L)
{
	INT32 nRetCode = 0;
	INT32 nServerGroupID = lua_tonumber(L, 1);
	BOOL bOpened = lua_tonumber(L, 2);

	LOG_PROCESS_ERROR(nServerGroupID >= 0 && nServerGroupID <= MAX_SERVER_GROUP_COUNT);

	if (MAX_SERVER_GROUP_COUNT == nServerGroupID)
	{
		for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
		{
            if (IsValidGroup(i))
			{
				nRetCode = CMessageHandler::GetInstance().DoGTSwitchServerGroupState(i, bOpened);
				LOG_PROCESS_ERROR(nRetCode);
			}
			//else break;
		}
	} 
	else
	{
		nRetCode = CMessageHandler::GetInstance().DoGTSwitchServerGroupState(nServerGroupID, bOpened);
		LOG_PROCESS_ERROR(nRetCode);
	}

	lua_pushboolean(L, TRUE);

	return 1;
Exit0:
	lua_pushboolean(L, FALSE);

	return 1;
}

int lua_spawn_npc(lua_State* L)
{
	INT32 nRetCode = 0;
	INT32 nSceneTempID = lua_tonumber(L, 1);
	INT32 nX = lua_tonumber(L, 2);
	INT32 nY = lua_tonumber(L, 3);
	INT32 nZ = lua_tonumber(L, 4);
	INT32 nRange = lua_tonumber(L, 5);
	INT32 nNpcTemplateID = lua_tonumber(L, 6);
	INT32 nNpcCount = lua_tonumber(L, 7);
	INT32 nLiveTime = lua_tonumber(L, 8);

	INF("spawn npc %d count %d at %d (%d,%d,%d) range %d live %d", nNpcTemplateID, nNpcCount, nSceneTempID, nX, nY, nZ, nRange, nLiveTime);

	ConvertCoordO2N(nX, nY, nZ);

	nRetCode = CMessageHandler::GetInstance().DoGSSpawnNpc(nSceneTempID, 
		nX * POINT_PER_CM, nY * POINT_PER_CM, nZ * POINT_PER_CM, nRange * POINT_PER_CM, 
		nNpcTemplateID, nNpcCount, nLiveTime + CManageServer::GetInstance().GetGameTime());
	LOG_PROCESS_ERROR(nRetCode);

Exit0:

	return 0;
}

int lua_create_scene(lua_State* L)
{
	INT32 nRetCode = 0;
	UINT32 dwSuitableGS = 0;
	SCENE_INFO* poSceneInfo = NULL;

	INT32 nServerGroup = lua_tonumber(L, 1);
	INT32 nSceneTempID = lua_tonumber(L, 2);

	if (nServerGroup < MAX_SERVER_GROUP_COUNT)
	{
		dwSuitableGS = CMSSessionMgr::GetInstance().GetSuitableGS(0);
		LOG_PROCESS_ERROR(dwSuitableGS > 0);

		poSceneInfo = CSceneMgr::GetInstance().CreateScene(nSceneTempID, nServerGroup, 0);
		LOG_PROCESS_ERROR(poSceneInfo);

		poSceneInfo->dwGSSessionID = dwSuitableGS;
		poSceneInfo->eState = ESCENESTATE_CREATING;
		poSceneInfo->dwCreateTime = CManageServer::GetInstance().GetGameTime();
		poSceneInfo->WaitUserList.empty();

		nRetCode = CMessageHandler::GetInstance().DoGSCreateScene(dwSuitableGS, poSceneInfo->nServerGroupID, nSceneTempID, poSceneInfo->nSceneID);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		for (INT32 nServerGroup = 0; nServerGroup < MAX_SERVER_GROUP_COUNT; nServerGroup++)
		{
            if (IsValidGroup(nServerGroup))
			{
				dwSuitableGS = CMSSessionMgr::GetInstance().GetSuitableGS(0);
				LOG_PROCESS_ERROR(dwSuitableGS > 0);

				poSceneInfo = CSceneMgr::GetInstance().CreateScene(nSceneTempID, nServerGroup, 0);
				LOG_PROCESS_ERROR(poSceneInfo);

				poSceneInfo->dwGSSessionID = dwSuitableGS;
				poSceneInfo->eState = ESCENESTATE_CREATING;
				poSceneInfo->dwCreateTime = CManageServer::GetInstance().GetGameTime();
				poSceneInfo->WaitUserList.empty();

				nRetCode = CMessageHandler::GetInstance().DoGSCreateScene(dwSuitableGS, poSceneInfo->nServerGroupID, nSceneTempID, poSceneInfo->nSceneID);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
	}

	lua_pushnumber(L, poSceneInfo->nSceneID);

	return 1;
Exit0:
	CRI("failed to create scene %d on group %d", nSceneTempID, nServerGroup);

	return 0;
}

int lua_close_scene(lua_State* L)
{
	INT32 nRetCode = 0;
	UINT32 dwSuitableGS = 0;
	SCENE_INFO* poSceneInfo = NULL;

	INT32 nServerGroup = lua_tonumber(L, 1);
	INT32 nSceneTempID = lua_tonumber(L, 2);
	INT32 nSceneID = lua_tonumber(L, 3);

	if (nServerGroup < MAX_SERVER_GROUP_COUNT)
	{
		poSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, nSceneID);
		LOG_PROCESS_ERROR(poSceneInfo);

		poSceneInfo->eState = ESCENESTATE_CLOSING;

		nRetCode = CMessageHandler::GetInstance().DoGSCloseScene(poSceneInfo->dwGSSessionID,
			poSceneInfo->nServerGroupID, poSceneInfo->nSceneTempID, poSceneInfo->nSceneID);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		for (INT32 nServerGroup = 0; nServerGroup < MAX_SERVER_GROUP_COUNT; nServerGroup++)
		{
            if (IsValidGroup(nServerGroup))
			{
				poSceneInfo = CSceneMgr::GetInstance().FindScene(nSceneTempID, (nServerGroup << 24) | 1);
				LOG_PROCESS_ERROR(poSceneInfo);

				poSceneInfo->eState = ESCENESTATE_CLOSING;

				nRetCode = CMessageHandler::GetInstance().DoGSCloseScene(poSceneInfo->dwGSSessionID,
					poSceneInfo->nServerGroupID, poSceneInfo->nSceneTempID, poSceneInfo->nSceneID);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}
	}

	lua_pushboolean(L, TRUE);

	return 1;
Exit0:
	CRI("failed to destroy scene %d-%d on group %d", nSceneTempID, nSceneID, nServerGroup);

	return 0;
}

int lua_arrest_role(lua_State* L)
{
	INT32 nDuration{};
	INT32 nRetCode = 0;
	MS_ONLINE_ROLE_DATA* pOnlineRoleData = NULL;

	const char* pcszRoleName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszRoleName);

	nDuration = lua_tonumber(L, 2) * GAME_FPS;

	pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByName(pcszRoleName);
	LOG_PROCESS_ERROR(pOnlineRoleData && pOnlineRoleData->pRoleData);
	LOG_PROCESS_ERROR(pOnlineRoleData->pUser && pOnlineRoleData->pUser->eState == USER_STATE_INGAME);

	nRetCode = CMessageHandler::GetInstance().DoGSArrestRole(pOnlineRoleData->dwGSSessionID,
		pOnlineRoleData->pRoleData->dwRoleID, pOnlineRoleData->dwCliSessionID, nDuration);
	LOG_PROCESS_ERROR(nRetCode);

Exit0:

	return 0;
}

int lua_create_guild(lua_State* L)
{
    const char* pcszAnnounce{};
    INT32 nRetCode = 0;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;

    UINT32      dwRoleID = lua_tonumber(L, 1);

    const char* pcszGuildName = lua_tostring(L, 2);
    LOG_PROCESS_ERROR(pcszGuildName);

    pcszAnnounce = lua_tostring(L, 3);
    LOG_PROCESS_ERROR(pcszAnnounce);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
    LOG_PROCESS_ERROR(pOnlineRole);
    LOG_PROCESS_ERROR(pOnlineRole->pRoleData);

    nRetCode = CMessageHandler::GetInstance().DoNSCreateGuild(dwRoleID, pcszGuildName, pcszGuildName, FALSE);
    LOG_PROCESS_ERROR(nRetCode);

Exit0:

    return 0;
}

// Lua function table G
LUA_FUNC g_LuaScriptFuncTable[] =
{
    { "clear_string_table",         lua_clear_string_table },
	{ "register_string",			lua_register_string },
	{ "call_gs_gm_func",			lua_call_gs_gm_func },
	{ "call_gs_func",				lua_call_gs_func },
	{ "print_ms",					lua_print_ms },
	{ "say",						lua_say },
	{ "register_timer",				lua_register_timer },
	{ "clear_timer",				lua_clear_timer },
	{ "wait",						lua_wait },
	{ "kick_role",					lua_kick_role },
	{ "notify_guard", 				lua_notify_guard },
	{ "kickall",					lua_kickall },
	{ "switch_server_state",		lua_switch_server_state },
	{ "spawn_npc",					lua_spawn_npc },
	{ "create_scene",				lua_create_scene },
	{ "close_scene",				lua_close_scene },
	{ "arrest_role",				lua_arrest_role },
    { "create_guild",               lua_create_guild },
	{ 0, 0 },
};
