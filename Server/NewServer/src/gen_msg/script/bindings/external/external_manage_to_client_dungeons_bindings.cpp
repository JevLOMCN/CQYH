#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/m2c_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/ranklist_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/team_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_manage_to_client_dungeons_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_guild_dungeon_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_DUNGEON_INFO>(L, external_message_namespace, "GUILD_DUNGEON_INFO", "ExternalMessage::GUILD_DUNGEON_INFO");
    external_message_guild_dungeon_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_DUNGEON_INFO(); });
    external_message_guild_dungeon_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_DUNGEON_INFO>(new ExternalMessage::GUILD_DUNGEON_INFO()); });
    external_message_guild_dungeon_info_type["dwTemplateID"] = sol::property([](const ExternalMessage::GUILD_DUNGEON_INFO& self) { return self.dwTemplateID; }, [](ExternalMessage::GUILD_DUNGEON_INFO& self, decltype(ExternalMessage::GUILD_DUNGEON_INFO::dwTemplateID) value) { self.dwTemplateID = value; });
    external_message_guild_dungeon_info_type["dwOwnerID"] = sol::property([](const ExternalMessage::GUILD_DUNGEON_INFO& self) { return self.dwOwnerID; }, [](ExternalMessage::GUILD_DUNGEON_INFO& self, decltype(ExternalMessage::GUILD_DUNGEON_INFO::dwOwnerID) value) { self.dwOwnerID = value; });
    external_message_guild_dungeon_info_type["nCreateTime"] = sol::property([](const ExternalMessage::GUILD_DUNGEON_INFO& self) { return self.nCreateTime; }, [](ExternalMessage::GUILD_DUNGEON_INFO& self, decltype(ExternalMessage::GUILD_DUNGEON_INFO::nCreateTime) value) { self.nCreateTime = value; });
    auto external_message_m2c_guild_get_dungeon_list_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK>(L, external_message_namespace, "M2C_GUILD_GET_DUNGEON_LIST_ACK", "ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_get_dungeon_list_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK(); });
    external_message_m2c_guild_get_dungeon_list_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK>(new ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK()); });
    external_message_m2c_guild_get_dungeon_list_ack_type["byDungeonCount"] = sol::property([](const ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK& self) { return self.byDungeonCount; }, [](ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK& self, decltype(ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK::byDungeonCount) value) { self.byDungeonCount = value; });
    external_message_m2c_guild_get_dungeon_list_ack_type["info"] = cqyh::lua::array_property(&ExternalMessage::M2C_GUILD_GET_DUNGEON_LIST_ACK::info);
    auto external_message_m2c_guild_open_dungeon_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_OPEN_DUNGEON_ACK>(L, external_message_namespace, "M2C_GUILD_OPEN_DUNGEON_ACK", "ExternalMessage::M2C_GUILD_OPEN_DUNGEON_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_open_dungeon_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_OPEN_DUNGEON_ACK(); });
    external_message_m2c_guild_open_dungeon_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_OPEN_DUNGEON_ACK>(new ExternalMessage::M2C_GUILD_OPEN_DUNGEON_ACK()); });
}

}
