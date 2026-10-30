#pragma once
#include <memory>
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

namespace cqyh::exportdb::detail
{
struct RestoreTargetRole
{
    int reference;
    void operator()(lua_State* state) const noexcept
    {
        if (reference == LUA_REFNIL) lua_pushnil(state);
        else lua_rawgeti(state, LUA_REGISTRYINDEX, reference);
        lua_setglobal(state, "TargetRole");
        luaL_unref(state, LUA_REGISTRYINDEX, reference);
    }
};
using TargetRoleGuard = std::unique_ptr<lua_State, RestoreTargetRole>;
// Borrows the state; restores its previous global and releases the registry
// reference before the temporary C++ role object can be destroyed.
inline TargetRoleGuard PreserveTargetRole(lua_State* state)
{
    if (!state) return {nullptr, {LUA_REFNIL}};
    lua_getglobal(state, "TargetRole");
    return {state, {luaL_ref(state, LUA_REGISTRYINDEX)}};
}
}
