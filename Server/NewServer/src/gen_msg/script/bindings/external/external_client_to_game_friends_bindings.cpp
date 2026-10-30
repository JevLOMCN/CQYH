#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_friends_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_friend_change_group_name_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME>(L, external_message_namespace, "C2G_FRIEND_CHANGE_GROUP_NAME", "ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_friend_change_group_name_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME(); });
    external_message_c2g_friend_change_group_name_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME>(new ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME()); });
    external_message_c2g_friend_change_group_name_type["byGroupID"] = sol::property([](const ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME& self) { return self.byGroupID; }, [](ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME& self, decltype(ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME::byGroupID) value) { self.byGroupID = value; });
    external_message_c2g_friend_change_group_name_type["szGroupName"] = cqyh::lua::string_property(&ExternalMessage::C2G_FRIEND_CHANGE_GROUP_NAME::szGroupName);
    auto external_message_c2g_unshield_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_UNSHIELD>(L, external_message_namespace, "C2G_UNSHIELD", "ExternalMessage::C2G_UNSHIELD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_unshield_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_UNSHIELD(); });
    external_message_c2g_unshield_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_UNSHIELD>(new ExternalMessage::C2G_UNSHIELD()); });
    external_message_c2g_unshield_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_UNSHIELD& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_UNSHIELD& self, decltype(ExternalMessage::C2G_UNSHIELD::dwRoleID) value) { self.dwRoleID = value; });
}

}
