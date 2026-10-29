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

void register_external_game_to_client_attributes_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_attr_data_type = cqyh::gen_msg::usertype<ExternalMessage::ATTR_DATA>(L, external_message_namespace, "ATTR_DATA", "ExternalMessage::ATTR_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_attr_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::ATTR_DATA(); });
    external_message_attr_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::ATTR_DATA>(new ExternalMessage::ATTR_DATA()); });
    external_message_attr_data_type["byAttrID"] = sol::property([](const ExternalMessage::ATTR_DATA& self) { return self.byAttrID; }, [](ExternalMessage::ATTR_DATA& self, decltype(ExternalMessage::ATTR_DATA::byAttrID) value) { self.byAttrID = value; });
    external_message_attr_data_type["nAttrValue"] = sol::property([](const ExternalMessage::ATTR_DATA& self) { return self.nAttrValue; }, [](ExternalMessage::ATTR_DATA& self, decltype(ExternalMessage::ATTR_DATA::nAttrValue) value) { self.nAttrValue = value; });
    auto external_message_g2c_sync_attr_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ATTR_LIST>(L, external_message_namespace, "G2C_SYNC_ATTR_LIST", "ExternalMessage::G2C_SYNC_ATTR_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_attr_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ATTR_LIST(); });
    external_message_g2c_sync_attr_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ATTR_LIST>(new ExternalMessage::G2C_SYNC_ATTR_LIST()); });
    external_message_g2c_sync_attr_list_type["astAttrData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_ATTR_LIST::astAttrData);
    auto external_message_g2c_sync_attr_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ATTR_CHANGE>(L, external_message_namespace, "G2C_SYNC_ATTR_CHANGE", "ExternalMessage::G2C_SYNC_ATTR_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_attr_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ATTR_CHANGE(); });
    external_message_g2c_sync_attr_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ATTR_CHANGE>(new ExternalMessage::G2C_SYNC_ATTR_CHANGE()); });
    external_message_g2c_sync_attr_change_type["byAttrID"] = sol::property([](const ExternalMessage::G2C_SYNC_ATTR_CHANGE& self) { return self.byAttrID; }, [](ExternalMessage::G2C_SYNC_ATTR_CHANGE& self, decltype(ExternalMessage::G2C_SYNC_ATTR_CHANGE::byAttrID) value) { self.byAttrID = value; });
    external_message_g2c_sync_attr_change_type["nAttrValue"] = sol::property([](const ExternalMessage::G2C_SYNC_ATTR_CHANGE& self) { return self.nAttrValue; }, [](ExternalMessage::G2C_SYNC_ATTR_CHANGE& self, decltype(ExternalMessage::G2C_SYNC_ATTR_CHANGE::nAttrValue) value) { self.nAttrValue = value; });
    auto external_message_g2c_sync_role_attr_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA>(L, external_message_namespace, "G2C_SYNC_ROLE_ATTR_DATA", "ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_role_attr_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA(); });
    external_message_g2c_sync_role_attr_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA>(new ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA()); });
    external_message_g2c_sync_role_attr_data_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_role_attr_data_type["nFatigue"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA& self) { return self.nFatigue; }, [](ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA::nFatigue) value) { self.nFatigue = value; });
    external_message_g2c_sync_role_attr_data_type["astAttrData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_ROLE_ATTR_DATA::astAttrData);
    auto external_message_g2c_sync_hero_attr_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_HERO_ATTR_LIST>(L, external_message_namespace, "G2C_SYNC_HERO_ATTR_LIST", "ExternalMessage::G2C_SYNC_HERO_ATTR_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_hero_attr_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_HERO_ATTR_LIST(); });
    external_message_g2c_sync_hero_attr_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_HERO_ATTR_LIST>(new ExternalMessage::G2C_SYNC_HERO_ATTR_LIST()); });
    external_message_g2c_sync_hero_attr_list_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_ATTR_LIST& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_SYNC_HERO_ATTR_LIST& self, decltype(ExternalMessage::G2C_SYNC_HERO_ATTR_LIST::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_sync_hero_attr_list_type["astAttrData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_HERO_ATTR_LIST::astAttrData);
    auto external_message_g2c_hero_attr_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_HERO_ATTR_CHANGE>(L, external_message_namespace, "G2C_HERO_ATTR_CHANGE", "ExternalMessage::G2C_HERO_ATTR_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_hero_attr_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_HERO_ATTR_CHANGE(); });
    external_message_g2c_hero_attr_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_HERO_ATTR_CHANGE>(new ExternalMessage::G2C_HERO_ATTR_CHANGE()); });
    external_message_g2c_hero_attr_change_type["byAttrID"] = sol::property([](const ExternalMessage::G2C_HERO_ATTR_CHANGE& self) { return self.byAttrID; }, [](ExternalMessage::G2C_HERO_ATTR_CHANGE& self, decltype(ExternalMessage::G2C_HERO_ATTR_CHANGE::byAttrID) value) { self.byAttrID = value; });
    external_message_g2c_hero_attr_change_type["nAttrValue"] = sol::property([](const ExternalMessage::G2C_HERO_ATTR_CHANGE& self) { return self.nAttrValue; }, [](ExternalMessage::G2C_HERO_ATTR_CHANGE& self, decltype(ExternalMessage::G2C_HERO_ATTR_CHANGE::nAttrValue) value) { self.nAttrValue = value; });
    auto external_message_g2c_sync_combat_stat_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_COMBAT_STAT>(L, external_message_namespace, "G2C_SYNC_COMBAT_STAT", "ExternalMessage::G2C_SYNC_COMBAT_STAT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_combat_stat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_COMBAT_STAT(); });
    external_message_g2c_sync_combat_stat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_COMBAT_STAT>(new ExternalMessage::G2C_SYNC_COMBAT_STAT()); });
    external_message_g2c_sync_combat_stat_type["DataList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_COMBAT_STAT::DataList);
}

}
