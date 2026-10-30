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

void register_external_manage_to_client_characters_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_sync_role_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_SYNC_ROLE_INFO>(L, external_message_namespace, "M2C_SYNC_ROLE_INFO", "ExternalMessage::M2C_SYNC_ROLE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_sync_role_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_SYNC_ROLE_INFO(); });
    external_message_m2c_sync_role_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_SYNC_ROLE_INFO>(new ExternalMessage::M2C_SYNC_ROLE_INFO()); });
    external_message_m2c_sync_role_info_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_SYNC_ROLE_INFO& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_SYNC_ROLE_INFO& self, decltype(ExternalMessage::M2C_SYNC_ROLE_INFO::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_sync_role_info_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_SYNC_ROLE_INFO::szRoleName);
    external_message_m2c_sync_role_info_type["byClass"] = sol::property([](const ExternalMessage::M2C_SYNC_ROLE_INFO& self) { return self.byClass; }, [](ExternalMessage::M2C_SYNC_ROLE_INFO& self, decltype(ExternalMessage::M2C_SYNC_ROLE_INFO::byClass) value) { self.byClass = value; });
    external_message_m2c_sync_role_info_type["bySex"] = sol::property([](const ExternalMessage::M2C_SYNC_ROLE_INFO& self) { return self.bySex; }, [](ExternalMessage::M2C_SYNC_ROLE_INFO& self, decltype(ExternalMessage::M2C_SYNC_ROLE_INFO::bySex) value) { self.bySex = value; });
    external_message_m2c_sync_role_info_type["byVIPLevel"] = sol::property([](const ExternalMessage::M2C_SYNC_ROLE_INFO& self) { return self.byVIPLevel; }, [](ExternalMessage::M2C_SYNC_ROLE_INFO& self, decltype(ExternalMessage::M2C_SYNC_ROLE_INFO::byVIPLevel) value) { self.byVIPLevel = value; });
    external_message_m2c_sync_role_info_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::M2C_SYNC_ROLE_INFO::szGuildName);
}

}
