#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_guild_members_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_apply_to_join_guild_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_APPLY_TO_JOIN_GUILD>(L, external_message_namespace, "C2M_APPLY_TO_JOIN_GUILD", "ExternalMessage::C2M_APPLY_TO_JOIN_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_apply_to_join_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_APPLY_TO_JOIN_GUILD(); });
    external_message_c2m_apply_to_join_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_APPLY_TO_JOIN_GUILD>(new ExternalMessage::C2M_APPLY_TO_JOIN_GUILD()); });
    external_message_c2m_apply_to_join_guild_type["dwGuildID"] = sol::property([](const ExternalMessage::C2M_APPLY_TO_JOIN_GUILD& self) { return self.dwGuildID; }, [](ExternalMessage::C2M_APPLY_TO_JOIN_GUILD& self, decltype(ExternalMessage::C2M_APPLY_TO_JOIN_GUILD::dwGuildID) value) { self.dwGuildID = value; });
    external_message_c2m_apply_to_join_guild_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::C2M_APPLY_TO_JOIN_GUILD::szGuildName);
    auto external_message_c2m_get_apply_guild_list_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GET_APPLY_GUILD_LIST>(L, external_message_namespace, "C2M_GET_APPLY_GUILD_LIST", "ExternalMessage::C2M_GET_APPLY_GUILD_LIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_get_apply_guild_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GET_APPLY_GUILD_LIST(); });
    external_message_c2m_get_apply_guild_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GET_APPLY_GUILD_LIST>(new ExternalMessage::C2M_GET_APPLY_GUILD_LIST()); });
    auto external_message_c2m_revert_guild_apply_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_REVERT_GUILD_APPLY>(L, external_message_namespace, "C2M_REVERT_GUILD_APPLY", "ExternalMessage::C2M_REVERT_GUILD_APPLY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_revert_guild_apply_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_REVERT_GUILD_APPLY(); });
    external_message_c2m_revert_guild_apply_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_REVERT_GUILD_APPLY>(new ExternalMessage::C2M_REVERT_GUILD_APPLY()); });
    external_message_c2m_revert_guild_apply_type["byRevert"] = sol::property([](const ExternalMessage::C2M_REVERT_GUILD_APPLY& self) { return self.byRevert; }, [](ExternalMessage::C2M_REVERT_GUILD_APPLY& self, decltype(ExternalMessage::C2M_REVERT_GUILD_APPLY::byRevert) value) { self.byRevert = value; });
    external_message_c2m_revert_guild_apply_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_REVERT_GUILD_APPLY& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_REVERT_GUILD_APPLY& self, decltype(ExternalMessage::C2M_REVERT_GUILD_APPLY::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_invite_to_join_guild_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_INVITE_TO_JOIN_GUILD>(L, external_message_namespace, "C2M_INVITE_TO_JOIN_GUILD", "ExternalMessage::C2M_INVITE_TO_JOIN_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_invite_to_join_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_INVITE_TO_JOIN_GUILD(); });
    external_message_c2m_invite_to_join_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_INVITE_TO_JOIN_GUILD>(new ExternalMessage::C2M_INVITE_TO_JOIN_GUILD()); });
    external_message_c2m_invite_to_join_guild_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2M_INVITE_TO_JOIN_GUILD::szRoleName);
    auto external_message_c2m_guild_change_position_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_CHANGE_POSITION>(L, external_message_namespace, "C2M_GUILD_CHANGE_POSITION", "ExternalMessage::C2M_GUILD_CHANGE_POSITION", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_change_position_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_CHANGE_POSITION(); });
    external_message_c2m_guild_change_position_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_CHANGE_POSITION>(new ExternalMessage::C2M_GUILD_CHANGE_POSITION()); });
    external_message_c2m_guild_change_position_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_GUILD_CHANGE_POSITION& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_GUILD_CHANGE_POSITION& self, decltype(ExternalMessage::C2M_GUILD_CHANGE_POSITION::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2m_guild_change_position_type["byPosition"] = sol::property([](const ExternalMessage::C2M_GUILD_CHANGE_POSITION& self) { return self.byPosition; }, [](ExternalMessage::C2M_GUILD_CHANGE_POSITION& self, decltype(ExternalMessage::C2M_GUILD_CHANGE_POSITION::byPosition) value) { self.byPosition = value; });
}

}
