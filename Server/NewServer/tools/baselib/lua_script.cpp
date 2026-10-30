#include "stdafx.h"
#include "sol_bindings.h"
#include "lua_script.h"

#include "file_owner.h"
#include "script_mgr.h"
#include "utf8.h"
#include "zlib.h"

#include <cstring>
#include <memory>


#define LUA_SCRIPT_THIS		"LuaScriptThis"
#define LUA_GLOBAL_BUFFER   "LuaGlobalBuffer"

#if defined(__linux__)
#define _snprintf snprintf
#endif

CLuaScript::CLuaScript(void)
{
	m_pLuaState = NULL;
	m_szScriptName[0] = 0;
    m_dwCRC = 0;
}

CLuaScript::~CLuaScript(void)
{
	UnInit();
}

BOOL CLuaScript::Init(void)
{
	LOG_PROCESS_ERROR(m_pLuaState == NULL);

	m_pLuaState = lua_open();
	LOG_PROCESS_ERROR(m_pLuaState);

    luaL_openlibs(m_pLuaState);
    cqyh::lua::initialize(m_pLuaState);
    cqyh::lua::initialize(m_pLuaState);

    lua_register(m_pLuaState, "print", _Print);
	lua_register(m_pLuaState, "Include", _Include);

	lua_pushlightuserdata(m_pLuaState, this);
	lua_setglobal(m_pLuaState, LUA_SCRIPT_THIS);

    m_dwCRC = 0;

	return TRUE;
Exit0:
	if (m_pLuaState)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	return FALSE;
}

BOOL CLuaScript::UnInit(void)
{
	if (m_pLuaState)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	//m_szScriptName[0] = 0;

	return TRUE;
}

BOOL CLuaScript::LoadFromFile(const char* pcszFileName)
{
	INT32 nRetCode = FALSE;
	std::unique_ptr<char[]> pszBuffer;
	UINT32 dwSize = 0;
	cqyh::FilePtr pFile;
    UINT32 dwOldCRC = m_dwCRC;

    LOG_PROCESS_ERROR(pcszFileName);

    m_dwCRC = 0;
    _GetCRC(pcszFileName, 0);

    if (dwOldCRC != m_dwCRC)
    {
        pFile.reset(g_OpenFile(pcszFileName, "rb"));
        LOG_PROCESS_ERROR(pFile);

        dwSize = pFile->Size();

        pszBuffer = std::make_unique<char[]>(dwSize);
        LOG_PROCESS_ERROR(pszBuffer);

        nRetCode = pFile->Read(pszBuffer.get(), dwSize);
        LOG_PROCESS_ERROR(nRetCode == dwSize);

        if (dwSize >= 3 && std::memcmp(pszBuffer.get(), "\xEF\xBB\xBF", 3) == 0)
        {
            nRetCode = LoadFromBuffer(pcszFileName, pszBuffer.get() + 3, dwSize - 3);
            LOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            nRetCode = LoadFromBuffer(pcszFileName, pszBuffer.get(), dwSize);
            LOG_PROCESS_ERROR(nRetCode);
        }

        pszBuffer.reset();
        pFile.reset();

		INF("loaded lua script %s successfully", pcszFileName);
    }
	else
	{
//		INF("skipped to load lua script %s", pcszFileName);
	}

	return TRUE;
Exit0:
	if (pcszFileName)
		WRN("failed to load lua script %s", pcszFileName);

	return FALSE;
}

BOOL CLuaScript::LoadFromBuffer(const char* pcszScriptName, const char* pcszBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pcszBuffer);

    if (pcszScriptName)
        strxcpy(m_szScriptName, pcszScriptName, SCRIPT_NAME_LEN);

	nRetCode = luaL_loadbuffer(m_pLuaState, pcszBuffer, dwSize, pcszScriptName);
	LOG_PROCESS_ERROR(nRetCode == 0);

    lua_setglobal(m_pLuaState, LUA_GLOBAL_BUFFER);
    lua_getglobal(m_pLuaState, LUA_GLOBAL_BUFFER);

	nRetCode = lua_pcall(m_pLuaState, 0, 0, 0);
    LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
	const char* pszErrorMsg = lua_tostring(m_pLuaState, -1);
	if (pszErrorMsg)
	{
		CRI("[LUA] LoadFromBuffer %s : %s", pcszScriptName, pszErrorMsg);
	}
    else
    {
        CRI("[LUA] LoadFromBuffer %s", pcszScriptName);
    }

	return FALSE;
}

BOOL CLuaScript::RegisterFunc(LUA_FUNC FuncList[], INT32 nFuncCount)
{
	LOG_PROCESS_ERROR(m_pLuaState);

	if (nFuncCount == 0)
	{
		for (int nIndex = 0;  FuncList[nIndex].pcszFuncName != NULL; nIndex ++)
			lua_register(m_pLuaState, FuncList[nIndex].pcszFuncName, FuncList[nIndex].pFunc);
	}
	else
	{
		for (int nIndex = 0; nIndex < nFuncCount; nIndex ++)
			lua_register(m_pLuaState, FuncList[nIndex].pcszFuncName, FuncList[nIndex].pFunc);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CLuaScript::_CallFunction(const char* pcszFuncName, const char* pcszFormat, va_list valist )
{
	INT32 nRetCode = 0;
    INT32 nStackTop = -1;
	INT32 nIndex = 0;
	INT32 nArgnum = 0;
	INT32 nParam = 0;
	INT32 nErrFunc = 0;
    INT32 nResult = 0;
    INT32 nStackIndex = 0;

	LOG_PROCESS_ERROR(m_pLuaState);
	nStackTop = nParam = lua_gettop(m_pLuaState);

	lua_pushcfunction(m_pLuaState, _LuaErrFunc);
	nErrFunc = lua_gettop(m_pLuaState);

	lua_getglobal(m_pLuaState, pcszFuncName);
	LOG_PROCESS_ERROR(lua_isfunction(m_pLuaState, -1));

    while (pcszFormat[nIndex] != '\0' && pcszFormat[nIndex] != ':')
	{
		switch(pcszFormat[nIndex])
		{
		case 'f':
			{ 
				double fNumber = va_arg(valist, double);
				lua_pushnumber(m_pLuaState, fNumber);
				nArgnum++;							
			}
			break;
		case 'd':
			{
				double fNumber = (double)(va_arg(valist, INT32));
				lua_pushnumber(m_pLuaState, fNumber);
				nArgnum++;
			}
			break;
		case 's':
			{
				char* pszString = va_arg(valist, char*);
				lua_pushstring(m_pLuaState, pszString);
				nArgnum++;							
			}
			break;
		case 'N':
			{
				lua_pushnil(m_pLuaState);
				nArgnum++;
			}
			break;
		case 'F':
			{
				lua_CFunction pLuaCFunc = va_arg(valist, lua_CFunction);
				lua_pushcfunction(m_pLuaState, pLuaCFunc) ;
				nArgnum++;
			}
			break;
		case 't':
			{
				lua_pushvalue(m_pLuaState, nParam);
				lua_remove(m_pLuaState, nParam);
				nArgnum++;
				nParam--;
			}
			break;
		case 'u':
			{
				lua_pushvalue(m_pLuaState, nParam);
				lua_remove(m_pLuaState, nParam);
				nArgnum++;
				nParam--;
			}
			break;
		case 'b':
			{
				BOOL bValue = va_arg(valist, BOOL);
				lua_pushboolean(m_pLuaState, bValue) ;
				nArgnum++;
			}
			break;
		case 'o':
			{
				void* pObj = va_arg(valist, void*);
				char* pszClassName = va_arg(valist, char*);
				cqyh::lua::push_object(m_pLuaState, pObj, pszClassName);
				nArgnum++;
			}
			break;
		default:
			LOG_PROCESS_ERROR(FALSE);
			break;
		}

		nIndex++;
	}

    if (pcszFormat[nIndex] == ':')
        nIndex++;

    nResult = (INT32)strlen(pcszFormat + nIndex);

	nRetCode = lua_pcall(m_pLuaState, nArgnum, nResult, nErrFunc);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nStackIndex = lua_gettop(m_pLuaState) - nResult + 1;
    LOG_PROCESS_ERROR(nStackIndex > 0);

    while (pcszFormat[nIndex] != '\0')
    {
        switch(pcszFormat[nIndex])
        {
        case 'f':
            {
                float* pfNumber = va_arg(valist, float*);
                LOG_PROCESS_ERROR(pfNumber);
                LOG_PROCESS_ERROR(lua_isnumber(m_pLuaState, nStackIndex));

                *pfNumber = (float)lua_tonumber(m_pLuaState, nStackIndex++);
            }
            break;
        case 'D':
            {
                double* pfNumber = va_arg(valist, double*);
                LOG_PROCESS_ERROR(pfNumber);
                LOG_PROCESS_ERROR(lua_isnumber(m_pLuaState, nStackIndex));

                *pfNumber = lua_tonumber(m_pLuaState, nStackIndex++);
            }
            break;
        case 'd':
            {
                INT32* pnInteger = va_arg(valist, INT32*);
                LOG_PROCESS_ERROR(pnInteger);
                LOG_PROCESS_ERROR(lua_isnumber(m_pLuaState, nStackIndex));

                *pnInteger = (INT32)lua_tonumber(m_pLuaState, nStackIndex++);
            }
            break;
        case 's':
            {
                char* pszString = va_arg(valist, char*);
                LOG_PROCESS_ERROR(pszString);
                LOG_PROCESS_ERROR(lua_isstring(m_pLuaState, nStackIndex));
                INT32 nStrLen = va_arg(valist, INT32);

                strxcpy(pszString, lua_tostring(m_pLuaState, nStackIndex++), nStrLen);
            }
            break;
        case 'b':
            {
                BOOL* pbBoolean = va_arg(valist, BOOL*);
                LOG_PROCESS_ERROR(pbBoolean);
                LOG_PROCESS_ERROR(lua_isboolean(m_pLuaState, nStackIndex));

                *pbBoolean = lua_toboolean(m_pLuaState, nStackIndex++);
            }
			break;
        case 'o':
            {
                void** ppObj = va_arg(valist, void**);
                LOG_PROCESS_ERROR(ppObj);
                LOG_PROCESS_ERROR(lua_isuserdata(m_pLuaState, nStackIndex) || lua_isnil(m_pLuaState, nStackIndex));

                *ppObj = cqyh::lua::object_pointer(m_pLuaState, nStackIndex++);
            }
			break;
        }

        nIndex++;	
    }

    lua_settop(m_pLuaState, nStackTop);

	return TRUE;

Exit0:
	const char* pszErrorMsg = lua_tostring(m_pLuaState, -1);
	if (pszErrorMsg)
	{
		CRI("failed to call lua function %s(%s), %s", pcszFuncName, pcszFormat, pszErrorMsg);
	}
	else
	{
		CRI("failed to call lua function %s(%s)", pcszFuncName, pcszFormat);
	}

    if (nStackTop != -1)
        lua_settop(m_pLuaState, nStackTop);

	return FALSE;
}

BOOL CLuaScript::CallFunction(const char* pcszFuncName, const char* pcszFormat, ...)
{
	BOOL nRet = FALSE;

	va_list valist;
	va_start(valist, pcszFormat);
	nRet = _CallFunction(pcszFuncName, pcszFormat, valist );
	va_end(valist);

	return nRet;
}

BOOL CLuaScript::Execute(void)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(m_pLuaState);

    lua_getglobal(m_pLuaState, LUA_GLOBAL_BUFFER);

    nRetCode = lua_pcall(m_pLuaState, 0, 0, 0);
    LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
    if (m_pLuaState)
    {
        const char* pszErrorMsg = lua_tostring(m_pLuaState, -1);
        if (pszErrorMsg)
        {
            CRI("[LUA] failed to execute script %s : %s", m_szScriptName, pszErrorMsg);
        }
        else
        {
            CRI("[LUA] failed to execute script %s", m_szScriptName);
        }
    }

    return FALSE;
}

BOOL CLuaScript::IsFuncExist(const char* pcszFuncName)
{
	BOOL bResult = FALSE;
	INT32 nStackTop = 0;

	LOG_PROCESS_ERROR(pcszFuncName);

	lua_getglobal(m_pLuaState, pcszFuncName);
	bResult = lua_isfunction(m_pLuaState, lua_gettop(m_pLuaState));
    lua_pop(m_pLuaState, 1);

Exit0:

	return bResult;
}

BOOL CLuaScript::IsTableExist(const char* pcszTableName)
{
    BOOL bResult = FALSE;
    INT32 nStackTop = 0;

    LOG_PROCESS_ERROR(pcszTableName);

    lua_getglobal(m_pLuaState, pcszTableName);
    bResult = lua_istable(m_pLuaState, lua_gettop(m_pLuaState));
    lua_pop(m_pLuaState, 1);

Exit0:

    return bResult;
}

const char* CLuaScript::GetScriptName(void)
{
	return m_szScriptName;
}

lua_State* CLuaScript::GetLuaState(void)
{
	return m_pLuaState;
}

CLuaScript* CLuaScript::GetScriptFromState(lua_State* L)
{
	CLuaScript* pResult = NULL;
	INT32 nTop = 0;

	LOG_PROCESS_ERROR(L);

	nTop = lua_gettop(L);
	lua_getglobal(L, LUA_SCRIPT_THIS);

	pResult = (CLuaScript*)lua_touserdata(L, -1);
	LOG_PROCESS_ERROR(pResult);

Exit0:
	if (L)
		lua_settop(L, nTop);

	return pResult;
}

BOOL CLuaScript::CollectGarbage(INT32 nGCSize)
{
    INT32 nMemoryAmount = lua_gc(m_pLuaState, LUA_GCCOUNT, 0);

    if (nMemoryAmount > nGCSize) 
    {
        lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);
        CRI("[LUA] collect garbage in script %s (%d KB)", m_szScriptName, nMemoryAmount);
    }
    return TRUE;
}

INT32 CLuaScript::_Include(lua_State* L)
{
	INT32 nRetCode = 0;
	cqyh::FilePtr pFile;
	std::unique_ptr<char[]> pszBuffer;
	UINT32 dwSize = 0;
	const char* pcszFileName = NULL;
    INT32 nErrFunc = 0;
    char szFileName[SCRIPT_NAME_LEN];

	pcszFileName = lua_tostring(L, 1);
	LOG_PROCESS_ERROR(pcszFileName);

    lua_pushcfunction(L, _LuaErrFunc);
    nErrFunc = lua_gettop(L);

	if (CScriptMgr::GetInstance().GetRootPath()[0])
	{
		snprintf(szFileName, SCRIPT_NAME_LEN, "%s/%s", CScriptMgr::GetInstance().GetRootPath(), pcszFileName);
		pFile.reset(g_OpenFile(szFileName, "rb"));
	}
	else
    {
        snprintf(szFileName, SCRIPT_NAME_LEN, "%s", pcszFileName);
        pFile.reset(g_OpenFile(szFileName, "rb"));
    }
    LOG_PROCESS_ERROR(pFile);

	dwSize = pFile->Size();
	LOG_PROCESS_ERROR(dwSize > 0);

	pszBuffer = std::make_unique<char[]>(dwSize);
	LOG_PROCESS_ERROR(pszBuffer);

	nRetCode = pFile->Read(pszBuffer.get(), dwSize);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

    if (dwSize >= 3 && std::memcmp(pszBuffer.get(), "\xEF\xBB\xBF", 3) == 0)
    {
	    nRetCode = luaL_loadbuffer(L, pszBuffer.get() + 3, dwSize - 3, pcszFileName);
	    LOG_PROCESS_ERROR(nRetCode == 0);
    }
    else
    {
        nRetCode = luaL_loadbuffer(L, pszBuffer.get(), dwSize, pcszFileName);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

	nRetCode = lua_pcall(L, 0, 0, nErrFunc);
	LOG_PROCESS_ERROR(nRetCode == 0);

	pszBuffer.reset();
	pFile.reset();

	return 0;

Exit0:
	if (pcszFileName)
    {
        if (lua_isstring(L, -1))
            CRI("[LUA] Include() : %s, %s", pcszFileName, lua_tostring(L, -1));
        else
            CRI("[LUA] Include() : %s", pcszFileName);
    }
	else 
		CRI("[LUA] Include() : nil file name");

	return 0;
}

void CLuaScript::_EnumTable(lua_State* L, int nTableIndex, int nTableDepth)
{
    int nTop = lua_gettop(L);     
    nTableIndex--;
    
    lua_pushnil(L);
    while (lua_next(L, nTableIndex) != 0) 
    {
        char szMessage[SCRIPT_NAME_LEN] = {0};
        char szTab[SCRIPT_NAME_LEN] = {0};
        const char* pszName = NULL;
		char szKeyName[256];

		switch (lua_type(L, -2))
		{
		case LUA_TNUMBER:
			itoa((INT32)lua_tonumber(L, -2), szKeyName, 10);
			pszName = szKeyName;
			break;
		case LUA_TSTRING:
			pszName = lua_tostring(L, -2);
			break;
		case LUA_TNIL:
		case LUA_TBOOLEAN:
		case LUA_TLIGHTUSERDATA:
		case LUA_TTABLE:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		default:
			break;
		}

        if  (pszName == NULL)
        {
            CLog::GetInstance().WriteLog(lltLua, "NULL key name!");
            continue;
        }

        for (int i = 0; i < nTableDepth; i++)
            strcat(szTab, "\t");

        switch (lua_type(L, -1))
        {
        case LUA_TNIL:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TNONE", szTab, pszName);
            break;
        case LUA_TBOOLEAN:
            if (lua_toboolean(L, -1))
                snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TBOOLEAN true", szTab, pszName);
            else
                snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TBOOLEAN false", szTab, pszName);
            break;
        case LUA_TLIGHTUSERDATA:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TLIGHTUSERDATA %P", szTab, pszName, lua_touserdata(L, -1));
            break;
        case LUA_TNUMBER:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TNUMBER %f", szTab, pszName, lua_tonumber(L, -1));
            break;
        case LUA_TSTRING:
            {
                const char* pszString = lua_tostring(L, -1);
                if (pszString)
                    snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TSTRING %s", szTab, pszName, pszString);
                else
                    snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TSTRING NULL", szTab, pszName);
            }
            break;
        case LUA_TTABLE:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TTABLE", szTab, pszName);
            break;
        case LUA_TFUNCTION:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TFUNCTION", szTab, pszName);
            break;
        case LUA_TUSERDATA:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TUSERDATA %P", szTab, pszName, lua_touserdata(L, -1));
            break;
        case LUA_TTHREAD:
            snprintf(szMessage, SCRIPT_NAME_LEN, "%s%s : LUA_TTHREAD %P", szTab, pszName, lua_tothread(L, -1));
            break;
        default:
            break;
        }

        CLog::GetInstance().WriteLog(lltLua, szMessage);

        if (lua_type(L, -1) == LUA_TTABLE && pszName[0] && nTableDepth < 2)
          _EnumTable(L, -1, nTableDepth + 1);

        lua_pop(L, 1);
    }

    lua_settop(L, nTop);

    return;
}

INT32 CLuaScript::_LuaErrFunc(lua_State* L)
{
    lua_Debug ar;
    INT32 nLevel = 1;
    static BOOL s_bInErrFunc = FALSE;

    if (s_bInErrFunc)
    {
        return 1;
    }

    s_bInErrFunc = TRUE;

    while (lua_getstack(L, nLevel, &ar))
    {
        if (lua_getinfo(L, "Snlu", &ar))
        {
            char szMessage[1024] = {0};
            if (ar.namewhat[0] != 0)
                _snprintf(szMessage, 1024, "[%d]%s:%d in function %s", nLevel, ar.short_src, ar.currentline, ar.name);
            else if (*ar.what == 'm')
                _snprintf(szMessage, 1024, "[%d]%s:%d in main chunk", nLevel, ar.short_src, ar.currentline);
            else if (*ar.what == 'C' || *ar.what == 't')
                _snprintf(szMessage, 1024, "[%d]%s:?", nLevel, ar.short_src);
            else
                _snprintf(szMessage, 1024, "[%d]%s:%d in function <%s:%d>", nLevel, ar.short_src, 
                ar.currentline, ar.short_src, ar.linedefined);
            CLog::GetInstance().WriteLog(lltLua, szMessage);
        }

        int nLocalIndex = 1;
        const char* pcszVarName = lua_getlocal(L, &ar, nLocalIndex);
        while (pcszVarName)
        {
            char szMessage[1024] = {0};
            switch (lua_type(L, -1))
            {
            case LUA_TNONE:
                {
                   _snprintf(szMessage, 1024, "\t%s : LUA_TNONE", pcszVarName);
                   break;
                }
            case LUA_TUSERDATA:
                {
                    _snprintf(szMessage, 1024, "\t%s : LUA_TUSERDATA", pcszVarName);
                    break;
                }
            case LUA_TNIL:
                {
                    _snprintf(szMessage, 1024, "\t%s : LUA_TNIL", pcszVarName);
                    break;
                }
            case LUA_TNUMBER:
                {
                    _snprintf(szMessage, 1024, "\t%s : LUA_TNUMBER %f", pcszVarName, lua_tonumber(L, -1));
                    break;
                }
            case LUA_TSTRING:
                {
                    const char* pszValue = lua_tostring(L, -1);
                    if (pszValue)
                        _snprintf(szMessage, 1024, "\t%s : LUA_TSTRING %s", pcszVarName, pszValue);
                    else
                        _snprintf(szMessage, 1024, "\t%s : LUA_TSTRING NULL", pcszVarName);
                    break;
                }
            case LUA_TTABLE:
                {
                    _snprintf(szMessage, 1024, "\t%s : LUA_TTABLE", pcszVarName);
                    break;
                }
            case LUA_TFUNCTION:
                {
                    _snprintf(szMessage, 1024, "\t%s : LUA_TFUNCTION", pcszVarName);
                    break;
                }
            }
            CLog::GetInstance().WriteLog(lltLua, szMessage);

            if (lua_type(L, -1) == LUA_TTABLE)
                _EnumTable(L, -1, 2);

            lua_pop(L, 1);
            nLocalIndex++;
            pcszVarName = lua_getlocal(L, &ar, nLocalIndex);
        }

        nLevel++;
    }

    s_bInErrFunc = FALSE;

    return 1;
}

INT32 CLuaScript::_Print(lua_State* L)
{
    const char* pcszMessage = NULL;
    LOG_PROCESS_ERROR(lua_gettop(L) > 0);

	if(lua_istable(L, 1))
		_EnumTable(L, -1, 0);
	else
	{
		pcszMessage = lua_tostring(L, 1);
		LOG_PROCESS_ERROR(pcszMessage);

		CLog::GetInstance().WriteLog(lltLua, pcszMessage);
	}

Exit0:
    return 0;
}

UINT32 CLuaScript::GetCRC(void)
{
    if (m_dwCRC == 0)
        _GetCRC(m_szScriptName, 0);

    return m_dwCRC;
}

void CLuaScript::_GetCRC(const char* pcszFileName, INT32 nLevel)
{
    INT32 nRetCode = 0;
    cqyh::FilePtr pFile;
    UINT32 dwSize = 0;
    std::unique_ptr<char[]> pszFileData;
    UINT32 dwPointer = 0;
    BOOL bInString = FALSE;
    char chStartChar = 0;
    char szIncludeFileName[MAX_PATH];
    char szTab[MAX_PATH] = {0};

    //memset(szTab, 0x09, nLevel);
    //DBG("%sinclude file %s", szTab, pcszFileName);

    pFile.reset(g_OpenFile(pcszFileName, "rb"));
    LOG_PROCESS_ERROR(pFile);

    dwSize = pFile->Size();
    PROCESS_ERROR(dwSize > 0);

    pszFileData = std::make_unique<char[]>(static_cast<std::size_t>(dwSize) + 1);
    LOG_PROCESS_ERROR(pszFileData);

    nRetCode = pFile->Read(pszFileData.get(), dwSize);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    pFile.reset();

    m_dwCRC = crc32(m_dwCRC, reinterpret_cast<const Bytef*>(pszFileData.get()), dwSize);

    while (dwPointer < dwSize)
    {
        switch (pszFileData[dwPointer])
        {
        case '\'':
            if (!bInString)
            {
                bInString = TRUE;
                chStartChar = '\'';
            }
            else if (bInString && chStartChar == '\'')
            {
                bInString = FALSE;
            }
            break;
        case '\"':
            if (!bInString)
            {
                bInString = TRUE;
                chStartChar = '\"';
            }
            else if (bInString && chStartChar == '\"')
            {
                bInString = FALSE;
            }
            break;
        case '[':
            if (!bInString && (dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == '['))
            {
                bInString = TRUE;
                chStartChar = '[';
                dwPointer++;
            }
            break;
        case ']':
            if (bInString && chStartChar == '[' && (dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == ']'))
            {
                bInString = FALSE;
                dwPointer++;
            }
            break;
        case '\\':
            if (bInString)
            {
                dwPointer++;
            }
            break;
        case '-':
            if (!bInString && (dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == '-'))
            {
                dwPointer += 2;
                if ((dwPointer + 1 < dwSize) && (pszFileData[dwPointer + 1] == '['))
                {
                    dwPointer += 3;
                    while (dwPointer < dwSize && pszFileData[dwPointer - 2] != ']' && pszFileData[dwPointer - 1] != '-' && pszFileData[dwPointer] != '-')
                    {
                        dwPointer++;
                    }
                }
                else
                {
                    while (dwPointer < dwSize && pszFileData[dwPointer] != '\n')
                        dwPointer++;
                }
            }
            break;
        default:
            if (!bInString && (dwPointer + 8 < dwSize) && 
                *(UINT32*)(pszFileData.get() + dwPointer) == 'lcnI' &&
                *(UINT32*)(pszFileData.get() + dwPointer + 4) == '(edu')
            {
                dwPointer += 8;
                while (dwPointer < dwSize && pszFileData[dwPointer] != '\"')
                    dwPointer++;
                dwPointer++;

                char* pszFileName = pszFileData.get() + dwPointer;

                while (dwPointer < dwSize && pszFileData[dwPointer] != '\"')
                    dwPointer++;
                pszFileData[dwPointer] = 0;

                if (CScriptMgr::GetInstance().GetRootPath()[0])
                {
                    snprintf(szIncludeFileName, MAX_PATH, "%s/%s", CScriptMgr::GetInstance().GetRootPath(), pszFileName);
                }
                else
                {
                    snprintf(szIncludeFileName, MAX_PATH, "%s", pszFileName);
                }

                _GetCRC(szIncludeFileName, nLevel + 1);
            }
            break;
        }

        dwPointer++;
    }

	return;
Exit0:

	if (pcszFileName)
		CRI("failed to get crc from %s", pcszFileName);
	else
		CRI("failed to get crc from null pcszFileName");

	return;
}
