#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_characters_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_query_role_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_QUERY_ROLE_INFO>(L, external_message_namespace, "C2M_QUERY_ROLE_INFO", "ExternalMessage::C2M_QUERY_ROLE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_query_role_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_QUERY_ROLE_INFO(); });
    external_message_c2m_query_role_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_QUERY_ROLE_INFO>(new ExternalMessage::C2M_QUERY_ROLE_INFO()); });
    external_message_c2m_query_role_info_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_QUERY_ROLE_INFO& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_QUERY_ROLE_INFO& self, decltype(ExternalMessage::C2M_QUERY_ROLE_INFO::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2m_query_role_info_type["dwCRC"] = sol::property([](const ExternalMessage::C2M_QUERY_ROLE_INFO& self) { return self.dwCRC; }, [](ExternalMessage::C2M_QUERY_ROLE_INFO& self, decltype(ExternalMessage::C2M_QUERY_ROLE_INFO::dwCRC) value) { self.dwCRC = value; });
}

}
