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

void register_external_manage_to_client_attributes_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_sync_social_attr_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF>(L, external_message_namespace, "M2C_SYNC_SOCIAL_ATTR_NTF", "ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_sync_social_attr_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF(); });
    external_message_m2c_sync_social_attr_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF>(new ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF()); });
    external_message_m2c_sync_social_attr_ntf_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF& self, decltype(ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_sync_social_attr_ntf_type["dwValue"] = sol::property([](const ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF& self) { return self.dwValue; }, [](ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF& self, decltype(ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF::dwValue) value) { self.dwValue = value; });
    external_message_m2c_sync_social_attr_ntf_type["byLevel"] = sol::property([](const ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF& self) { return self.byLevel; }, [](ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF& self, decltype(ExternalMessage::M2C_SYNC_SOCIAL_ATTR_NTF::byLevel) value) { self.byLevel = value; });
}

}
