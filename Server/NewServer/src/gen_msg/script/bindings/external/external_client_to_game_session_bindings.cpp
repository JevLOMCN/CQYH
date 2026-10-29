#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_session_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_gpk_auth_data_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GPK_AUTH_DATA>(L, external_message_namespace, "C2G_GPK_AUTH_DATA", "ExternalMessage::C2G_GPK_AUTH_DATA", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_gpk_auth_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GPK_AUTH_DATA(); });
    external_message_c2g_gpk_auth_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GPK_AUTH_DATA>(new ExternalMessage::C2G_GPK_AUTH_DATA()); });
    external_message_c2g_gpk_auth_data_type["AuthData"] = cqyh::lua::string_property(&ExternalMessage::C2G_GPK_AUTH_DATA::AuthData);
    auto external_message_c2g_return_select_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_RETURN_SELECT_ROLE>(L, external_message_namespace, "C2G_RETURN_SELECT_ROLE", "ExternalMessage::C2G_RETURN_SELECT_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_return_select_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_RETURN_SELECT_ROLE(); });
    external_message_c2g_return_select_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_RETURN_SELECT_ROLE>(new ExternalMessage::C2G_RETURN_SELECT_ROLE()); });
    external_message_c2g_return_select_role_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_RETURN_SELECT_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_RETURN_SELECT_ROLE& self, decltype(ExternalMessage::C2G_RETURN_SELECT_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2g_logout_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LOGOUT>(L, external_message_namespace, "C2G_LOGOUT", "ExternalMessage::C2G_LOGOUT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_logout_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LOGOUT(); });
    external_message_c2g_logout_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LOGOUT>(new ExternalMessage::C2G_LOGOUT()); });
    auto external_message_c2g_logout_cancel_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LOGOUT_CANCEL>(L, external_message_namespace, "C2G_LOGOUT_CANCEL", "ExternalMessage::C2G_LOGOUT_CANCEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_logout_cancel_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LOGOUT_CANCEL(); });
    external_message_c2g_logout_cancel_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LOGOUT_CANCEL>(new ExternalMessage::C2G_LOGOUT_CANCEL()); });
    auto external_message_c2g_subscribe_vip_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SUBSCRIBE_VIP>(L, external_message_namespace, "C2G_SUBSCRIBE_VIP", "ExternalMessage::C2G_SUBSCRIBE_VIP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_subscribe_vip_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SUBSCRIBE_VIP(); });
    external_message_c2g_subscribe_vip_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SUBSCRIBE_VIP>(new ExternalMessage::C2G_SUBSCRIBE_VIP()); });
    external_message_c2g_subscribe_vip_type["byVIPLevel"] = sol::property([](const ExternalMessage::C2G_SUBSCRIBE_VIP& self) { return self.byVIPLevel; }, [](ExternalMessage::C2G_SUBSCRIBE_VIP& self, decltype(ExternalMessage::C2G_SUBSCRIBE_VIP::byVIPLevel) value) { self.byVIPLevel = value; });
    external_message_c2g_subscribe_vip_type["byMonth"] = sol::property([](const ExternalMessage::C2G_SUBSCRIBE_VIP& self) { return self.byMonth; }, [](ExternalMessage::C2G_SUBSCRIBE_VIP& self, decltype(ExternalMessage::C2G_SUBSCRIBE_VIP::byMonth) value) { self.byMonth = value; });
}

}
