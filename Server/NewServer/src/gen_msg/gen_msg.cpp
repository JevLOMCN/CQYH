#include "stdafx.h"
#include "external_message_header.h"
#include "internal_message_header.h"

#include "generator_bindings.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "zlib.h"

#include "lua_script.h"

#include "gameworld/bt_action.h"
#include "bt_event.h"

extern int sol_external_message_open (lua_State* L);
extern int sol_enum_module_open (lua_State* L);

static UINT32 s_dwMsgCRC = 0;

static char s_szMsgScript[] = 
"MAX_MESSAGE_ID = 1023;\n"\
"print('#include \"stdafx.h\"\\n\\n')\n"\
"print('//**** gen result begin ****//\\n')\n"\
"s2c = {};c2s = {};\n"\
"for k,v in pairs(ExternalMessage) do\n"\
"   if string.sub(k,2,2) == '2' then\n"\
"       if type(v) == 'number' then\n"\
"           if string.sub(k,5,17) == 'message_begin' or string.sub(k,5,15) == 'message_end' then\n"\
"               \n"\
"           elseif ExternalMessage[string.upper(k)] == nil then\n"\
"               print('// unmatched message id : '..k..'('..v..')\\n')\n"\
"           else\n"\
"--               print('//matched message id : '..k..'('..v..')\\n');\n"\
"           end\n"\
"       elseif type(v) == 'table' then\n"\
"           local msg_id = ExternalMessage[string.lower(k)];\n"\
"           if msg_id == nil then\n"\
"               print('//unmatched message struct : '..k..'\\n')\n"\
"           else\n"\
"               local msg_prefix = string.sub(k,1,3);\n"\
"               if msg_prefix == 'C2G' or msg_prefix == 'C2M' or msg_prefix == 'C2T' or msg_prefix == 'C2B' then\n"\
"                   local size = get_c2s_size(msg_id, k)\n"\
"                   if not size then\n"\
"                       print('//cannot get message size : '..k..'\\n')\n"\
"                   else\n"\
"                       c2s[msg_id] = {};\n"\
"                       c2s[msg_id].size = size;\n"\
"                       c2s[msg_id].name = k;\n"\
"--                     print('//message '..k..' size is '..size..'\\n')\n"\
"                   end\n"\
"               elseif msg_prefix == 'G2C' or msg_prefix == 'M2C' or msg_prefix == 'T2C' or msg_prefix == 'B2C' then\n"\
"                   local size = get_s2c_size(msg_id, k)\n"\
"                   if not size then\n"\
"                       print('//cannot get message size : '..k..'\\n')\n"\
"                   else\n"\
"                       s2c[msg_id] = {};\n"\
"                       s2c[msg_id].size = size;\n"\
"                       s2c[msg_id].name = k;\n"\
"--                     print('//message '..k..' size is '..size..'\\n')\n"\
"                   end\n"\
"               else\n"\
"                   print('//error message struct : '..k..'\\n')\n"\
"               end\n"\
"           end\n"\
"       end\n"\
"   end\n"\
"end\n"\
"print('//**** gen result end ****//\\n\\n')\n"\
"print('INT32 g_nS2CMessageSize[] = {\\n')\n"\
"for i=0, MAX_MESSAGE_ID do\n"\
"   if s2c[i] then\n"\
"       local s = string.format('    %5d,    // %3d %s\\n', s2c[i].size, i, s2c[i].name);\n"\
"       print(s);\n"\
"   else\n"\
"       print('        0,\\n');\n"\
"   end\n"\
"end\n"\
"print('};\\n\\n')\n"\
"print('INT32 g_nC2SMessageSize[] = {\\n')\n"\
"for i=0, MAX_MESSAGE_ID do\n"\
"   if c2s[i] then\n"\
"       local s = string.format('    %5d,    // %3d %s\\n', c2s[i].size, i, c2s[i].name);\n"\
"       print(s);\n"\
"   else\n"\
"       print('        0,\\n');\n"\
"   end\n"\
"end\n"\
"print('};\\n\\n')\n"\
"print('#if !SHIPPING_PC_GAME\\n')\n"\
"print('const char* g_szS2CMessageName[] = {\\n')\n"\
"for i=0, MAX_MESSAGE_ID do\n"\
"   if s2c[i] then\n"\
"       local s = string.format('    \"%s\",\\n', string.lower(s2c[i].name));\n"\
"       print(s);\n"\
"   else\n"\
"       print('    \"undefined s2c message\",\\n');\n"\
"   end\n"\
"end\n"\
"print('};\\n\\n')\n"\
"print('const char* g_szC2SMessageName[] = {\\n')\n"\
"for i=0, MAX_MESSAGE_ID do\n"\
"   if c2s[i] then\n"\
"       local s = string.format('    \"%s\",\\n', string.lower(c2s[i].name));\n"\
"       print(s);\n"\
"   else\n"\
"       print('    \"undefined c2s message\",\\n');\n"\
"   end\n"\
"end\n"\
"print('};\\n\\n')\n"\
"print('#endif // !SHIPPING_PC_GAME\\n\\n')\n"\
"print('INT32 g_nMessageVersion = ('..crc()..');\\n\\n')\n";

#define FLEX_MESSAGE_SIZE   (-1)

static char s_szEnumScript[] = ""\
"print('class ServerEnum extends Object native;\\n\\n')\n"\
"for k,v in pairs(_G) do\n"\
"   if type(v) == 'table' and k ~= 'os' and k ~= 'table' and k ~= 'string' and k ~= 'math' and k ~= 'coroutine' and k ~= '_G' and k ~= 'debug' and k ~= 'package' and k ~= 'io'and k ~= 'ERROR_CODE_DEF' and not getmetatable(v) then\n"\
"       print('enum E'..k..'\\n{\\n')\n"\
"       local t = {};\n"\
"       local max = -1;\n"\
"       local prefix;\n"\
"       for kk, vv in pairs(v) do\n"\
"           if t[vv] then\n"\
"               if (not string.find(string.lower(kk), 'begin', 1, true)) and (not string.find(string.lower(kk), 'end', 1, true)) then\n"\
"                   t[vv] = kk\n"\
"                   if vv > max then\n"\
"                       max = vv;\n"\
"                   end\n"\
"               else\n"\
"                   print('// conflicted enum definition '..kk..'('..vv..')\\n');\n"\
"               end\n"\
"           else\n"\
"               t[vv] = kk\n"\
"               if vv > max then\n"\
"                   max = vv;\n"\
"               end\n"\
"           end\n"\
"           if prefix == nil then\n"\
"               prefix = kk;\n"\
"           else\n"\
"               while string.sub(kk, 1, #prefix) ~= prefix and #prefix > 0 do\n"\
"                   prefix = string.sub(prefix, 1, #prefix - 1);\n"\
"               end\n"\
"           end\n"\
"       end\n"\
"       \n"\
"       if prefix == nil then\n"\
"           print('// failed to found enum prefix\\n');\n"\
"       elseif #prefix == 0 then\n"\
"           print('// failed to found enum prefix\\n');\n"\
"       elseif string.sub(prefix, -1) == '_' then\n"\
"           prefix = string.sub(prefix, 1, #prefix - 1);\n"\
"       end\n"\
"       if max > 254 then\n"\
"           max = 254;\n"\
"           print('// max enum definition is over 254\\n');\n"\
"       end\n"\
"       for i=0, max do\n"\
"           local name = t[i];\n"\
"           if name == nil then\n"\
"               name = 'Reserved_'..i\n"\
"           else\n"\
"               name = string.sub(t[i], #prefix + 1, 1000);\n"\
"               if string.sub(name, 1, 1) == '_' then\n"\
"                   name = string.sub(name, 2, 1000);\n"\
"               end\n"\
"           end\n"\
"           name = 'E_'..prefix..'_'..name;\n"\
"           print('\t'..name)\n"\
"           for i=#name,32 do\n"\
"               print(' ')\n"\
"           end\n"\
"           print(' <DisplayName=\"'..name..'\">,\\n')\n"\
"       end\n"\
"       print('\tE_'..prefix..'_MAX\\n')\n"\
"       print('};\\n\\n')\n"\
"   end\n"\
"end\n"\
"\n"\
"";

static char s_szErrcodeScript[] = ""\
"print('#include \"stdafx.h\"\\n\\n')\n"\
"print('//**** gen error code name list begin ****//\\n')\n"\
"print('const char* g_szErrorCodeNameList[] = {\\n')\n"\
"local t = {};\n"\
"local max = -1;\n"\
"for k,v in pairs(ERROR_CODE_DEF) do\n"\
"    if t[v] then\n"\
"        if (not string.find(string.lower(kk), 'begin', 1, true)) and (not string.find(string.lower(kk), 'end', 1, true)) then\n"\
"            t[v] = k\n"\
"            if v > max then\n"\
"                max = v;\n"\
"            end\n"\
"        else\n"\
"            print('// conflicted enum definition '..k..'('..v..')\\n');\n"\
"        end\n"\
"    else\n"\
"        t[v] = k\n"\
"        if v > max then\n"\
"            max = v;\n"\
"        end\n"\
"    end\n"\
"end\n"\
"\n"\
"for i=0, max do\n"\
"    local name = t[i];\n"\
"    if name == nil then\n"\
"        name = 'Reserved_'..i\n"\
"    end\n"\
"    print('\t\"'..name..'\",')\n"\
"    for i=#name,64 do\n"\
"        print(' ')\n"\
"    end\n"\
"    print('// '..i..'\\n')\n"\
"end\n"\
"print('};\\n\\n')\n"\
"print('//**** gen error code name list end ****//\\n\\n')\n"\
"\n"\
"";

static char s_szBTNode[] = ""\
"bInBTNode = false;\n"\
"BTEdit = 'BET_Common';\n"\
"BTNodeList = {};\n"\
"BTNode = nil;\n"\
"for _, line in ipairs(BT_SOURCE_LINES) do\n"\
"   if bInBTNode and not string.find(line, '///< %@see') then\n"\
"       if string.find(line, '/%* Common') then \n"\
"           BTEdit = 'BET_Common' \n"\
"       end\n"\
"       if string.find(line, '/%* Role') then \n"\
"           BTEdit = 'BET_Common' \n"\
"       end\n"\
"       if string.find(line, '/%* AI') then \n"\
"           BTEdit = 'BET_AI' \n"\
"       end\n"\
"       if string.find(line, '/%* Char') then \n"\
"           BTEdit = 'BET_Common' \n"\
"       end\n"\
"       if string.find(line, '/%* Scene') then \n"\
"           BTEdit = 'BET_Scene' \n"\
"       end\n"\
"\n"\
"       local _, _, node_name = string.find(line, '%s*(bat%w*)%s*[=,].-///<.-');\n"\
"       if node_name then\n"\
"           BTNode = { name = node_name, type = BTEdit};\n"\
"           table.insert(BTNodeList, BTNode);\n"\
"       end\n"\
"\n"\
"       local _, _, param_name, param_desc = string.find(line, '.-@param%s*(%w+)%s*(.+)%s*[%(%[%c]');\n"\
"       if not param_desc then\n"\
"           _, _, param_name, param_desc = string.find(line, '.-@param%s*(%w+)%s*(.+)');\n"\
"       end\n"\
"\n"\
"       if param_name then\n"\
"           if not BTNode.param then BTNode.param = {}; end\n"\
"           local p = {name = param_name, desc = param_desc};\n"\
"           table.insert(BTNode.param, p);\n"\
"       end\n"\
"   elseif string.find(line, 'enum BT_ACTION_TYPE') then\n"\
"       bInBTNode = true;\n"\
"   end\n"\
"\n"\
"\n"\
"end\n"\
"\n"\
"print('[BTNodeTypeList]\\n')\n"\
"for k,v in ipairs(BTNodeList) do\n"\
"   print('BTNodeTypeList='..v.name..'\\n')\n"\
"end\n"\
"\n"\
"print('\\n')\n"\
"for k,v in ipairs(BTNodeList) do\n"\
"   print('\\n['..v.name..']\\n')\n"\
"   print('BTEdit='..v.type..'\\n')\n"\
"   if v.param then \n"\
"       for kk, vv in ipairs(v.param) do\n"\
"           print(vv.name..'='..vv.desc..'\\n')\n"\
"       end\n"\
"   end\n"\
"end\n"\
"\n"\
"";


static char s_szBTEvent[] = ""\
"bInBTEvent = false;\n"\
"BTEdit = 'BET_Both';\n"\
"BTEventList = {};\n"\
"BTEvent = nil;\n"\
"for _, line in ipairs(BT_SOURCE_LINES) do\n"\
"   if bInBTEvent and not string.find(line, '///< %@see') then\n"\
"       if string.find(line, '/%* AI') then \n"\
"           BTEdit = 'BET_AI' \n"\
"       end\n"\
"       if string.find(line, '/%* Scene') then \n"\
"           BTEdit = 'BET_Scene' \n"\
"       end\n"\
"\n"\
"       local _, _, node_name = string.find(line, '%s*(evt%w*)%s*[=,].-///<.-');\n"\
"       if node_name then\n"\
"           BTEvent = { name = node_name, type = BTEdit};\n"\
"           table.insert(BTEventList, BTEvent);\n"\
"       end\n"\
"\n"\
"       local _, _, param_name, param_desc = string.find(line, '.-@param%s*(%w+)%s*(.+)%s*[%(%[%c]');\n"\
"       if not param_desc then\n"\
"           _, _, param_name, param_desc = string.find(line, '.-@param%s*(%w+)%s*(.+)');\n"\
"       end\n"\
"\n"\
"       if param_name then\n"\
"           if not BTEvent.param then BTEvent.param = {}; end\n"\
"           local p = {name = param_name, desc = param_desc};\n"\
"           table.insert(BTEvent.param, p);\n"\
"       end\n"\
"   elseif string.find(line, 'enum BT_EVENT_TYPE') then\n"\
"       bInBTEvent = true;\n"\
"   end\n"\
"\n"\
"\n"\
"end\n"\
"\n"\
"print('[BTEventTypeList]\\n')\n"\
"for k,v in ipairs(BTEventList) do\n"\
"   print('BTEventTypeList='..v.name..'\\n')\n"\
"end\n"\
"\n"\
"print('\\n')\n"\
"for k,v in ipairs(BTEventList) do\n"\
"   print('\\n['..v.name..']\\n')\n"\
"   print('BTEdit='..v.type..'\\n')\n"\
"   if v.param then \n"\
"       for kk, vv in ipairs(v.param) do\n"\
"           print(vv.name..'='..vv.desc..'\\n')\n"\
"       end\n"\
"   end\n"\
"end\n"\
"\n"\
"";

INT32 LuaPrint(lua_State* L)
{
    std::size_t length = 0;
    const char* text = luaL_checklstring(L, 1, &length);
    if (std::fwrite(text, 1, length, stdout) != length)
        return luaL_error(L, "cannot write generated output");
    s_dwMsgCRC = static_cast<UINT32>(crc32_z(s_dwMsgCRC,
        reinterpret_cast<const Bytef*>(text), length));
    return 0;
}

INT32 LuaCRC(lua_State* L)
{
    lua_pushnumber(L, s_dwMsgCRC);
    return 1;
}

INT32 LuaGetMessageSize(lua_State* L)
{
    const char* name = luaL_checkstring(L, 2);
    lua_getfield(L, LUA_REGISTRYINDEX, cqyh::gen_msg::size_registry);
    if (!lua_istable(L, -1))
        return luaL_error(L, "packet size metadata is unavailable");
    lua_getfield(L, -1, name);
    if (!lua_isnumber(L, -1))
        return luaL_error(L, "packet size metadata is missing for %s", name);
    return 1;
}

BOOL GenerateMsgSize(void)
{
    INT32 nRetCode = 0;
    CLuaScript LuaScript;
    lua_State* L;
    LUA_FUNC LuaFunc;

    nRetCode = LuaScript.Init();
    LOG_PROCESS_ERROR(nRetCode);

    L = LuaScript.GetLuaState();
    LOG_PROCESS_ERROR(L);

    nRetCode = sol_external_message_open(L);
    LOG_PROCESS_ERROR(nRetCode);

    LuaFunc.pcszFuncName = "get_c2s_size";
    LuaFunc.pFunc = LuaGetMessageSize;
    nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
    LOG_PROCESS_ERROR(nRetCode);

    LuaFunc.pcszFuncName = "get_s2c_size";
    LuaFunc.pFunc = LuaGetMessageSize;
    nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
    LOG_PROCESS_ERROR(nRetCode);

    LuaFunc.pcszFuncName = "print";
    LuaFunc.pFunc = LuaPrint;
    nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
    LOG_PROCESS_ERROR(nRetCode);

    LuaFunc.pcszFuncName = "crc";
    LuaFunc.pFunc = LuaCRC;
    nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = LuaScript.LoadFromBuffer("", s_szMsgScript, sizeof(s_szMsgScript) - 1);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL GenerateEnum(void)
{
    INT32 nRetCode = 0;
    CLuaScript LuaScript;
    lua_State* L;
    LUA_FUNC LuaFunc;

    nRetCode = LuaScript.Init();
    LOG_PROCESS_ERROR(nRetCode);

    L = LuaScript.GetLuaState();
    LOG_PROCESS_ERROR(L);

    sol_enum_module_open(L);

    LuaFunc.pcszFuncName = "print";
    LuaFunc.pFunc = LuaPrint;
    nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
    LOG_PROCESS_ERROR(nRetCode);

    LuaFunc.pcszFuncName = "crc";
    LuaFunc.pFunc = LuaCRC;
    nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = LuaScript.LoadFromBuffer("", s_szEnumScript, sizeof(s_szEnumScript) - 1);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL GenerateErrcode(void)
{
	INT32 nRetCode = 0;
	CLuaScript LuaScript;
	lua_State* L;
	LUA_FUNC LuaFunc;

	nRetCode = LuaScript.Init();
	LOG_PROCESS_ERROR(nRetCode);

	L = LuaScript.GetLuaState();
	LOG_PROCESS_ERROR(L);

	sol_enum_module_open(L);

	LuaFunc.pcszFuncName = "print";
	LuaFunc.pFunc = LuaPrint;
	nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
	LOG_PROCESS_ERROR(nRetCode);

	LuaFunc.pcszFuncName = "crc";
	LuaFunc.pFunc = LuaCRC;
	nRetCode = LuaScript.RegisterFunc(&LuaFunc, 1);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = LuaScript.LoadFromBuffer("", s_szErrcodeScript, sizeof(s_szErrcodeScript) - 1);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

namespace
{
std::expected<std::vector<std::string>, std::string> ReadSourceLines(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
        return std::unexpected("cannot open source file: " + path.string());
    std::vector<std::string> lines;
    for (std::string line; std::getline(input, line); )
    {
        if (line.ends_with('\r'))
            line.pop_back();
        lines.push_back(std::move(line));
    }
    if (input.bad())
        return std::unexpected("cannot read source file: " + path.string());
    return lines;
}

BOOL GenerateBehaviorTree(std::string_view code, const std::filesystem::path& source)
{
    auto lines = ReadSourceLines(source);
    if (!lines)
    {
        std::fprintf(stderr, "%s\n", lines.error().c_str());
        return FALSE;
    }
    CLuaScript script;
    if (!script.Init())
        return FALSE;
    auto* L = script.GetLuaState();
    if (!L || !sol_enum_module_open(L))
        return FALSE;
    LUA_FUNC printFunction{"print", LuaPrint};
    if (!script.RegisterFunc(&printFunction, 1))
        return FALSE;
    lua_newtable(L);
    int index = 1;
    for (const auto& line : *lines)
    {
        lua_pushlstring(L, line.data(), line.size());
        lua_rawseti(L, -2, index++);
    }
    lua_setglobal(L, "BT_SOURCE_LINES");
    return script.LoadFromBuffer("behavior-generator", code.data(), static_cast<UINT32>(code.size()));
}

void DiagnosticOutput(INT32, const char* message, INT32 length)
{
    std::fwrite(message, 1, static_cast<std::size_t>(length), stderr);
}

struct Command
{
    std::string_view name;
    BOOL (*generate)();
    std::string_view behaviorScript;
    const char* sourceHeader;
};
}

int main(int argc, char* argv[]) try
{
    const std::array commands{
        Command{"enum", GenerateEnum, {}, nullptr},
        Command{"msg", GenerateMsgSize, {}, nullptr},
        Command{"errcode", GenerateErrcode, {}, nullptr},
        Command{"btnode", nullptr, s_szBTNode, "bt_action.h"},
        Command{"btevent", nullptr, s_szBTEvent, "bt_event.h"}};
    const std::string_view mode = argc > 1 ? argv[1] : "";
    const auto command = std::ranges::find(commands, mode, &Command::name);
    if (command == commands.end() || argc < 2 || argc > 3 || (argc == 3 && !command->sourceHeader))
    {
        std::fputs("Usage: gen_msg {enum|msg|errcode|btnode|btevent} [source-header]\n", stderr);
        return EXIT_FAILURE;
    }
    auto& logger = CLog::GetInstance();
    logger.SetCustomOutput(DiagnosticOutput);
    if (!logger.Init("gen_msg", lltAll, lotCustom, FALSE, FALSE, FALSE))
        return EXIT_FAILURE;
    const auto closeLog = [](CLog* value) { value->UnInit(); };
    std::unique_ptr<CLog, decltype(closeLog)> logSession(&logger, closeLog);
    bool success;
    if (command->sourceHeader)
    {
#ifndef CQYH_GENERATOR_SOURCE_DIR
#define CQYH_GENERATOR_SOURCE_DIR "../../inc/gameworld"
#endif
        const auto source = argc == 3 ? std::filesystem::path(argv[2]) :
            std::filesystem::path(CQYH_GENERATOR_SOURCE_DIR) / command->sourceHeader;
        success = GenerateBehaviorTree(command->behaviorScript, source) != FALSE;
    }
    else
        success = command->generate() != FALSE;
    if (std::fflush(stdout) != 0)
    {
        std::fputs("cannot flush generated output\n", stderr);
        success = false;
    }
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (const std::exception& error)
{
    std::fprintf(stderr, "Generator failed: %s\n", error.what());
    return EXIT_FAILURE;
}
catch (...)
{
    std::fputs("Generator failed with an unknown exception\n", stderr);
    return EXIT_FAILURE;
}
