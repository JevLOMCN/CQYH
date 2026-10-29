#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_guild_buildings_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_guild_upgrade_welfare_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_UPGRADE_WELFARE>(L, external_message_namespace, "C2M_GUILD_UPGRADE_WELFARE", "ExternalMessage::C2M_GUILD_UPGRADE_WELFARE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_upgrade_welfare_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_UPGRADE_WELFARE(); });
    external_message_c2m_guild_upgrade_welfare_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_UPGRADE_WELFARE>(new ExternalMessage::C2M_GUILD_UPGRADE_WELFARE()); });
    external_message_c2m_guild_upgrade_welfare_type["byType"] = sol::property([](const ExternalMessage::C2M_GUILD_UPGRADE_WELFARE& self) { return self.byType; }, [](ExternalMessage::C2M_GUILD_UPGRADE_WELFARE& self, decltype(ExternalMessage::C2M_GUILD_UPGRADE_WELFARE::byType) value) { self.byType = value; });
    auto external_message_c2m_guild_cancel_welfare_upgrade_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE>(L, external_message_namespace, "C2M_GUILD_CANCEL_WELFARE_UPGRADE", "ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_cancel_welfare_upgrade_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE(); });
    external_message_c2m_guild_cancel_welfare_upgrade_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE>(new ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE()); });
    external_message_c2m_guild_cancel_welfare_upgrade_type["byType"] = sol::property([](const ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE& self) { return self.byType; }, [](ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE& self, decltype(ExternalMessage::C2M_GUILD_CANCEL_WELFARE_UPGRADE::byType) value) { self.byType = value; });
    auto external_message_c2m_guild_upgrade_building_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_UPGRADE_BUILDING>(L, external_message_namespace, "C2M_GUILD_UPGRADE_BUILDING", "ExternalMessage::C2M_GUILD_UPGRADE_BUILDING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_upgrade_building_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_UPGRADE_BUILDING(); });
    external_message_c2m_guild_upgrade_building_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_UPGRADE_BUILDING>(new ExternalMessage::C2M_GUILD_UPGRADE_BUILDING()); });
    external_message_c2m_guild_upgrade_building_type["byType"] = sol::property([](const ExternalMessage::C2M_GUILD_UPGRADE_BUILDING& self) { return self.byType; }, [](ExternalMessage::C2M_GUILD_UPGRADE_BUILDING& self, decltype(ExternalMessage::C2M_GUILD_UPGRADE_BUILDING::byType) value) { self.byType = value; });
    auto external_message_c2m_guild_cancel_building_upgrade_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE>(L, external_message_namespace, "C2M_GUILD_CANCEL_BUILDING_UPGRADE", "ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_cancel_building_upgrade_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE(); });
    external_message_c2m_guild_cancel_building_upgrade_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE>(new ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE()); });
    external_message_c2m_guild_cancel_building_upgrade_type["qwNameID"] = sol::property([](const ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE& self) { return self.qwNameID; }, [](ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE& self, decltype(ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE::qwNameID) value) { self.qwNameID = value; });
    external_message_c2m_guild_cancel_building_upgrade_type["byType"] = sol::property([](const ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE& self) { return self.byType; }, [](ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE& self, decltype(ExternalMessage::C2M_GUILD_CANCEL_BUILDING_UPGRADE::byType) value) { self.byType = value; });
}

}
