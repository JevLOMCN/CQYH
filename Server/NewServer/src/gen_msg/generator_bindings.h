#pragma once

#include "sol_bindings.h"
#include "external_message_header.h"
#include <type_traits>
#include <utility>

namespace cqyh::gen_msg
{
inline constexpr char size_registry[] = "cqyh.generator.message_sizes";

// Generator metadata belongs to its Lua state, separate from the wire objects.
// sizeof(T) describes the packet, independent of allocator or userdata storage.
template<class T, class Parent, class... Args>
auto usertype(lua_State* L, Parent&& parent, const char* name,
              const char* cpp_name, Args&&... args)
{
    auto result = cqyh::lua::usertype<T>(L, std::forward<Parent>(parent), name,
                                       cpp_name, std::forward<Args>(args)...);
    sol::state_view lua(L);
    sol::table sizes = lua.registry()[size_registry].get_or_create<sol::table>();
    if constexpr (std::is_base_of_v<ExternalMessage::FLEX_MESSAGE_HEADER, T>)
        sizes[name] = ExternalMessage::flex_message_size;
    else
        sizes[name] = sizeof(T);
    return result;
}
}
