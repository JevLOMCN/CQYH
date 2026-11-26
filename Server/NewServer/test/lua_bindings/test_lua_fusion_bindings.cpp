#include "std_type.h"
#include "sol_bindings.h"
#include "game_world_config.h"
#include "guild_config.h"
#include "new_role_config.h"
#include "obj_pool_config.h"
#include "server_config.h"
#include "grid_def.h"
#include "script/sol_enum.h"
#include "script/sol_global_config.h"
#include "script/sol_guild_config.h"
#include <cstdio>

// The production bindings operate on these process-wide configuration objects.
GAME_WORLD_CONFIG g_GWConfig = {};
GUILD_CONFIG g_GuildConfig = {};
NEW_ROLE_CONFIG g_NewRoleConfig = {};
OBJ_POOL_CONFIG g_ObjPoolConfig = {};
SERVER_CONFIG g_ServerConfig = {};
char g_GmRoleName[10][ROLE_NAME_LEN] = {};

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    cqyh::lua::initialize(lua);
    sol_enum_open(lua);
    sol_global_config_open(lua);
    sol_guild_config_open(lua);
    GRID_DATA grid = {};
    lua["grid"] = &grid;
    const char* script = R"(
        assert(errSuccess == 1)
        assert(type(MAX_SPELL_EFFECT_DISTANCE) == 'number')
        assert(catStabAtk == 12 and catMaxStabAtk == 13)
        assert(catArcheryAtk == 14 and catMaxArcheryAtk == 15)
        assert(catMaxHp == 16)
        assert(spatStab == 4 and spatArchery == 5)
        g_GWConfig.nForceSyncDistance = 4321
        g_GWConfig.szConfigPath = 'test/config'
        assert(g_GWConfig.nForceSyncDistance == 4321)
        assert(g_GWConfig.szConfigPath == 'test/config')
        g_ServerConfig.MS.nInitGroup = 8
        assert(IsValidGroup(3) == true)
        assert(IsValidGroup(2) == false)
        grid.uNextGrid = 2
        assert(grid.uNextGrid == 2)
    )";
    if (luaL_dostring(lua, script) != 0) {
        std::fprintf(stderr, "%s\n", lua_tostring(lua, -1));
        return 1;
    }
    // Repeated package loading must preserve the shared type registrations.
    sol_enum_open(lua);
    sol_global_config_open(lua);
    if (luaL_dostring(lua, "assert(grid.uNextGrid == 2 and g_GWConfig.nForceSyncDistance == 4321)") != 0) return 2;
    if (grid.uNextGrid != 2 || g_GWConfig.nForceSyncDistance != 4321) return 3;
    std::puts("Fusion binding entry points, shared types and configuration writes passed");
    return 0;
}
