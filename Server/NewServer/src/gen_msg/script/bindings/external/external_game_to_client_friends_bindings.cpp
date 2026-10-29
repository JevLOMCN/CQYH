#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/g2c_message.h"
#include "../../../../../../Common/define/dds_def.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/item_def.h"
#include "../../../../../../Common/define/master_def.h"
#include "../../../../../../Common/define/npc_def.h"
#include "../../../../../../Common/define/quest_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_game_to_client_friends_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_shield_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SHIELD_LIST>(L, external_message_namespace, "G2C_SYNC_SHIELD_LIST", "ExternalMessage::G2C_SYNC_SHIELD_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_shield_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_SHIELD_LIST(); });
    external_message_g2c_sync_shield_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_SHIELD_LIST>(new ExternalMessage::G2C_SYNC_SHIELD_LIST()); });
    external_message_g2c_sync_shield_list_type["byRoleCount"] = sol::property([](const ExternalMessage::G2C_SYNC_SHIELD_LIST& self) { return self.byRoleCount; }, [](ExternalMessage::G2C_SYNC_SHIELD_LIST& self, decltype(ExternalMessage::G2C_SYNC_SHIELD_LIST::byRoleCount) value) { self.byRoleCount = value; });
    external_message_g2c_sync_shield_list_type["arrRoleID"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_SHIELD_LIST::arrRoleID);
    auto external_message_friend_group_info_type = cqyh::gen_msg::usertype<ExternalMessage::FRIEND_GROUP_INFO>(L, external_message_namespace, "FRIEND_GROUP_INFO", "ExternalMessage::FRIEND_GROUP_INFO");
    external_message_friend_group_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::FRIEND_GROUP_INFO(); });
    external_message_friend_group_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::FRIEND_GROUP_INFO>(new ExternalMessage::FRIEND_GROUP_INFO()); });
    external_message_friend_group_info_type["byGroupID"] = sol::property([](const ExternalMessage::FRIEND_GROUP_INFO& self) { return self.byGroupID; }, [](ExternalMessage::FRIEND_GROUP_INFO& self, decltype(ExternalMessage::FRIEND_GROUP_INFO::byGroupID) value) { self.byGroupID = value; });
    external_message_friend_group_info_type["szGroupName"] = cqyh::lua::string_property(&ExternalMessage::FRIEND_GROUP_INFO::szGroupName);
    auto external_message_g2c_friend_group_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_FRIEND_GROUP_LIST>(L, external_message_namespace, "G2C_FRIEND_GROUP_LIST", "ExternalMessage::G2C_FRIEND_GROUP_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_friend_group_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_FRIEND_GROUP_LIST(); });
    external_message_g2c_friend_group_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_FRIEND_GROUP_LIST>(new ExternalMessage::G2C_FRIEND_GROUP_LIST()); });
    external_message_g2c_friend_group_list_type["byGroupCount"] = sol::property([](const ExternalMessage::G2C_FRIEND_GROUP_LIST& self) { return self.byGroupCount; }, [](ExternalMessage::G2C_FRIEND_GROUP_LIST& self, decltype(ExternalMessage::G2C_FRIEND_GROUP_LIST::byGroupCount) value) { self.byGroupCount = value; });
    external_message_g2c_friend_group_list_type["stGroupInfo"] = cqyh::lua::array_property(&ExternalMessage::G2C_FRIEND_GROUP_LIST::stGroupInfo);
    auto external_message_g2c_shield_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SHIELD_ACK>(L, external_message_namespace, "G2C_SHIELD_ACK", "ExternalMessage::G2C_SHIELD_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_shield_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SHIELD_ACK(); });
    external_message_g2c_shield_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SHIELD_ACK>(new ExternalMessage::G2C_SHIELD_ACK()); });
    external_message_g2c_shield_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SHIELD_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SHIELD_ACK& self, decltype(ExternalMessage::G2C_SHIELD_ACK::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_shield_ack_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::G2C_SHIELD_ACK::szRoleName);
    auto external_message_g2c_unshield_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_UNSHIELD_ACK>(L, external_message_namespace, "G2C_UNSHIELD_ACK", "ExternalMessage::G2C_UNSHIELD_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_unshield_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_UNSHIELD_ACK(); });
    external_message_g2c_unshield_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_UNSHIELD_ACK>(new ExternalMessage::G2C_UNSHIELD_ACK()); });
    external_message_g2c_unshield_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_UNSHIELD_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_UNSHIELD_ACK& self, decltype(ExternalMessage::G2C_UNSHIELD_ACK::dwRoleID) value) { self.dwRoleID = value; });
}

}
