#include "gm_configuration.h"
#include "lua_script.h"
#include "sol_bindings.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <limits>
#include <optional>
#include <set>

int sol_tl_config_open(lua_State* state);

namespace
{
constexpr std::size_t max_script_bytes = 16 * 1024 * 1024;

bool ipv4(std::string_view address)
{
    if (address == "255.255.255.255")
        return false;
    for (unsigned index = 0; index < 4; ++index)
    {
        const auto separator = address.find('.');
        const auto part = address.substr(0, separator);
        unsigned octet = 0;
        const auto parsed = std::from_chars(part.data(), part.data() + part.size(), octet);
        if (part.empty() || (part.size() > 1 && part.front() == '0') ||
            parsed.ec != std::errc{} || parsed.ptr != part.data() + part.size() || octet > 255)
            return false;
        if (index == 3)
            return separator == std::string_view::npos;
        if (separator == std::string_view::npos)
            return false;
        address.remove_prefix(separator + 1);
    }
    return false;
}

template <class Object, std::size_t Size>
auto checked_string(char (Object::*member)[Size], std::string& error)
{
    return sol::property(
        [member](const Object& object) { return std::string(object.*member); },
        [member, &error](Object& object, sol::object supplied)
        {
            if (supplied.get_type() != sol::type::string)
            {
                error = "configuration string has the wrong type";
                return;
            }
            const auto value = supplied.as<std::string_view>();
            if (value.size() >= Size || value.find('\0') != std::string::npos)
            {
                error = "configuration string exceeds its field or contains NUL";
                return;
            }
            std::fill_n(object.*member, Size, '\0');
            std::copy(value.begin(), value.end(), object.*member);
        });
}

template <class Object, class Number>
auto checked_number(Number Object::*member, Number maximum, std::string& error)
{
    return sol::property(
        [member](const Object& object) { return object.*member; },
        [member, maximum, &error](Object& object, sol::object supplied)
        {
            if (supplied.get_type() != sol::type::number)
            {
                error = "configuration number has the wrong type";
                return;
            }
            const auto value = supplied.as<lua_Number>();
            if (!std::isfinite(value) || value < 0 || value > maximum || std::trunc(value) != value)
            {
                error = "configuration number is outside its integral field range";
                return;
            }
            object.*member = static_cast<Number>(value);
        });
}

template <class Object, class Member>
auto checked_member(Member Object::*member, std::string& error)
{
    return sol::property(
        [member](Object& object) -> Member& { return object.*member; },
        [member, &error](Object& object, sol::object value)
        {
            if (!value.is<Member>())
                error = "configuration object has the wrong type";
            else
                object.*member = value.as<const Member&>();
        });
}

template <class Object, class Element, std::size_t Size>
auto checked_array(Element (Object::*member)[Size], std::string& error)
{
    return sol::property([member, &error](Object& object, sol::this_state state)
    {
        sol::state_view lua(state);
        auto* data = object.*member;
        auto table = lua.create_table();
        auto meta = lua.create_table();
        auto index = [&error](const sol::object& key) -> std::optional<std::size_t>
        {
            if (key.get_type() == sol::type::number)
            {
                const auto value = key.as<lua_Number>();
                if (std::isfinite(value) && value >= 0 && value < Size && std::trunc(value) == value)
                    return static_cast<std::size_t>(value);
            }
            error = "configuration array index is outside its integral range";
            return std::nullopt;
        };
        meta.set_function(sol::meta_function::index, [data, index](sol::table, sol::object key, sol::this_state state) -> sol::object
        {
            if (const auto slot = index(key))
                return sol::make_object(state, std::ref(data[*slot]));
            return sol::nil;
        });
        meta.set_function(sol::meta_function::new_index, [data, index, &error](sol::table, sol::object key, sol::object value)
        {
            const auto slot = index(key);
            if (!slot)
                return;
            if (!value.is<Element>())
                error = "configuration array element has the wrong type";
            else
                data[*slot] = value.as<const Element&>();
        });
        table[sol::metatable_key] = meta;
        return table;
    });
}
}

gm::Configuration gm::parse_configuration(std::string_view script)
{
    if (script.starts_with("\xEF\xBB\xBF"))
        script.remove_prefix(3);
    if (script.empty() || script.size() > max_script_bytes)
        return std::unexpected("empty or oversized configuration script");
    auto candidate = std::make_unique<TL_CONFIG>();
    std::string assignment_error;
    CLuaScript interpreter;
    if (!interpreter.Init())
        return std::unexpected("cannot initialize configuration interpreter");

    auto* state = interpreter.GetLuaState();
    sol_tl_config_open(state);
    {
        sol::state_view lua(state);
        lua.set_function("GetTLConfig", [staged = candidate.get()]() -> TL_CONFIG& { return *staged; });
        // This private interpreter gets bounded setters. Shared Server bindings
        // retain their existing behavior, including zero-based array properties.
        auto globals = lua.globals();
        auto server_type = cqyh::lua::usertype<GM_SVR>(state, globals, "GM_SVR", "GM_SVR");
        server_type["szName"] = checked_string(&GM_SVR::szName, assignment_error);
        server_type["szAddr"] = checked_string(&GM_SVR::szAddr, assignment_error);
        server_type["nSvrID"] = checked_number(&GM_SVR::nSvrID, (std::numeric_limits<INT32>::max)(), assignment_error);
        server_type["dwPort"] = checked_number(&GM_SVR::dwPort, UINT32{65535}, assignment_error);
        server_type["groupList"] = checked_array(&GM_SVR::groupList, assignment_error);
        auto group_type = cqyh::lua::usertype<GM_GROUP_DATA>(state, globals, "GM_GROUP_DATA", "GM_GROUP_DATA");
        group_type["szGroupName"] = checked_string(&GM_GROUP_DATA::szGroupName, assignment_error);
        sol::table config_type = lua["TL_CONFIG"];
        auto list_type = cqyh::lua::usertype<TL_CONFIG::GM_SVR_LIST>(state, config_type, "GM_SVR_LIST", "TL_CONFIG::GM_SVR_LIST");
        list_type["nSvrNum"] = checked_number(&TL_CONFIG::GM_SVR_LIST::nSvrNum, INT32{GM_MAX_SERVER_COUNT}, assignment_error);
        list_type["arrGMList"] = checked_array(&TL_CONFIG::GM_SVR_LIST::arrGMList, assignment_error);
        auto config_usertype = cqyh::lua::usertype<TL_CONFIG>(state, globals, "TL_CONFIG", "TL_CONFIG");
        config_usertype["ServerList"] = checked_member(&TL_CONFIG::ServerList, assignment_error);
    }

    const bool loaded = interpreter.LoadFromBuffer("gmtool_config.lua", script.data(), static_cast<UINT32>(script.size())) &&
        interpreter.CallFunction("LoadFile", "");
    // Finalizers may still write the candidate. Close while it and the checked
    // setter state are alive, then validate the final result before publication.
    interpreter.UnInit();
    if (!loaded)
        return std::unexpected("configuration script or LoadFile failed");
    if (!assignment_error.empty())
        return std::unexpected(std::move(assignment_error));

    std::set<INT32> server_ids;
    for (const auto& server : candidate->ServerList.arrGMList)
    {
        if (!server.nSvrID)
            continue;
        if (!server_ids.insert(server.nSvrID).second)
            return std::unexpected("duplicate server ID in configuration");
        if (!server.szName[0] || !ipv4(server.szAddr) || !server.dwPort)
            return std::unexpected("active server requires a name, IPv4 address and port");
    }
    if (server_ids.empty())
        return std::unexpected("configuration contains no active servers");
    return candidate;
}

gm::Configuration gm::load_configuration(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
        return std::unexpected("cannot open configuration file");
    std::string script;
    std::array<char, 64 * 1024> chunk;
    while (input)
    {
        input.read(chunk.data(), chunk.size());
        const auto size = static_cast<std::size_t>(input.gcount());
        if (size > max_script_bytes - script.size())
            return std::unexpected("oversized configuration file");
        script.append(chunk.data(), size);
    }
    if (input.bad() || !input.eof())
        return std::unexpected("cannot read configuration file");
    return parse_configuration(script);
}

std::expected<void, std::string> gm::reload_configuration(const std::filesystem::path& path, TL_CONFIG& live)
{
    auto candidate = load_configuration(path);
    if (!candidate)
        return std::unexpected(std::move(candidate.error()));
    live = **candidate;
    return {};
}
