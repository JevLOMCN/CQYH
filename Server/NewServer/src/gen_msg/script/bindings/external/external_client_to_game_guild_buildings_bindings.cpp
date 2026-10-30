#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_guild_buildings_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_guild_create_building_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_CREATE_BUILDING>(L, external_message_namespace, "C2G_GUILD_CREATE_BUILDING", "ExternalMessage::C2G_GUILD_CREATE_BUILDING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_create_building_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_CREATE_BUILDING(); });
    external_message_c2g_guild_create_building_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_CREATE_BUILDING>(new ExternalMessage::C2G_GUILD_CREATE_BUILDING()); });
    external_message_c2g_guild_create_building_type["dwNpcID"] = sol::property([](const ExternalMessage::C2G_GUILD_CREATE_BUILDING& self) { return self.dwNpcID; }, [](ExternalMessage::C2G_GUILD_CREATE_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_CREATE_BUILDING::dwNpcID) value) { self.dwNpcID = value; });
    external_message_c2g_guild_create_building_type["byBuildingType"] = sol::property([](const ExternalMessage::C2G_GUILD_CREATE_BUILDING& self) { return self.byBuildingType; }, [](ExternalMessage::C2G_GUILD_CREATE_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_CREATE_BUILDING::byBuildingType) value) { self.byBuildingType = value; });
    auto external_message_c2g_guild_upgrade_building_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_UPGRADE_BUILDING>(L, external_message_namespace, "C2G_GUILD_UPGRADE_BUILDING", "ExternalMessage::C2G_GUILD_UPGRADE_BUILDING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_upgrade_building_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_UPGRADE_BUILDING(); });
    external_message_c2g_guild_upgrade_building_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_UPGRADE_BUILDING>(new ExternalMessage::C2G_GUILD_UPGRADE_BUILDING()); });
    external_message_c2g_guild_upgrade_building_type["byUpgradeType"] = sol::property([](const ExternalMessage::C2G_GUILD_UPGRADE_BUILDING& self) { return self.byUpgradeType; }, [](ExternalMessage::C2G_GUILD_UPGRADE_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_UPGRADE_BUILDING::byUpgradeType) value) { self.byUpgradeType = value; });
    external_message_c2g_guild_upgrade_building_type["qwNpcNameID"] = sol::property([](const ExternalMessage::C2G_GUILD_UPGRADE_BUILDING& self) { return self.qwNpcNameID; }, [](ExternalMessage::C2G_GUILD_UPGRADE_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_UPGRADE_BUILDING::qwNpcNameID) value) { self.qwNpcNameID = value; });
    external_message_c2g_guild_upgrade_building_type["byBuildingType"] = sol::property([](const ExternalMessage::C2G_GUILD_UPGRADE_BUILDING& self) { return self.byBuildingType; }, [](ExternalMessage::C2G_GUILD_UPGRADE_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_UPGRADE_BUILDING::byBuildingType) value) { self.byBuildingType = value; });
    auto external_message_c2g_guild_del_building_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_DEL_BUILDING>(L, external_message_namespace, "C2G_GUILD_DEL_BUILDING", "ExternalMessage::C2G_GUILD_DEL_BUILDING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_del_building_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_DEL_BUILDING(); });
    external_message_c2g_guild_del_building_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_DEL_BUILDING>(new ExternalMessage::C2G_GUILD_DEL_BUILDING()); });
    external_message_c2g_guild_del_building_type["byDelType"] = sol::property([](const ExternalMessage::C2G_GUILD_DEL_BUILDING& self) { return self.byDelType; }, [](ExternalMessage::C2G_GUILD_DEL_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_DEL_BUILDING::byDelType) value) { self.byDelType = value; });
    external_message_c2g_guild_del_building_type["qwNameID"] = sol::property([](const ExternalMessage::C2G_GUILD_DEL_BUILDING& self) { return self.qwNameID; }, [](ExternalMessage::C2G_GUILD_DEL_BUILDING& self, decltype(ExternalMessage::C2G_GUILD_DEL_BUILDING::qwNameID) value) { self.qwNameID = value; });
}

}
