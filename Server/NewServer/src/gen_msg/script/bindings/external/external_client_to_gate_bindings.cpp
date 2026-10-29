#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2t_message.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_gate_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    external_message_namespace["c2t_message_begin"] = static_cast<lua_Number>(ExternalMessage::c2t_message_begin);
    external_message_namespace["c2t_login_req"] = static_cast<lua_Number>(ExternalMessage::c2t_login_req);
    external_message_namespace["c2t_create_role"] = static_cast<lua_Number>(ExternalMessage::c2t_create_role);
    external_message_namespace["c2t_delete_role"] = static_cast<lua_Number>(ExternalMessage::c2t_delete_role);
    external_message_namespace["c2t_forever_delete_role"] = static_cast<lua_Number>(ExternalMessage::c2t_forever_delete_role);
    external_message_namespace["c2t_undelete_role"] = static_cast<lua_Number>(ExternalMessage::c2t_undelete_role);
    external_message_namespace["c2t_select_role"] = static_cast<lua_Number>(ExternalMessage::c2t_select_role);
    external_message_namespace["c2t_ping"] = static_cast<lua_Number>(ExternalMessage::c2t_ping);
    external_message_namespace["c2t_message_end"] = static_cast<lua_Number>(ExternalMessage::c2t_message_end);
    auto external_message_c2t_login_req_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_LOGIN_REQ>(L, external_message_namespace, "C2T_LOGIN_REQ", "ExternalMessage::C2T_LOGIN_REQ", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_login_req_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_LOGIN_REQ(); });
    external_message_c2t_login_req_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_LOGIN_REQ>(new ExternalMessage::C2T_LOGIN_REQ()); });
    external_message_c2t_login_req_type["szUserName"] = cqyh::lua::string_property(&ExternalMessage::C2T_LOGIN_REQ::szUserName);
    external_message_c2t_login_req_type["szPassword"] = cqyh::lua::string_property(&ExternalMessage::C2T_LOGIN_REQ::szPassword);
    external_message_c2t_login_req_type["byGroup"] = sol::property([](const ExternalMessage::C2T_LOGIN_REQ& self) { return self.byGroup; }, [](ExternalMessage::C2T_LOGIN_REQ& self, decltype(ExternalMessage::C2T_LOGIN_REQ::byGroup) value) { self.byGroup = value; });
    external_message_c2t_login_req_type["wMajorVersion"] = sol::property([](const ExternalMessage::C2T_LOGIN_REQ& self) { return self.wMajorVersion; }, [](ExternalMessage::C2T_LOGIN_REQ& self, decltype(ExternalMessage::C2T_LOGIN_REQ::wMajorVersion) value) { self.wMajorVersion = value; });
    external_message_c2t_login_req_type["wMinorVersion"] = sol::property([](const ExternalMessage::C2T_LOGIN_REQ& self) { return self.wMinorVersion; }, [](ExternalMessage::C2T_LOGIN_REQ& self, decltype(ExternalMessage::C2T_LOGIN_REQ::wMinorVersion) value) { self.wMinorVersion = value; });
    external_message_c2t_login_req_type["szSessionID"] = cqyh::lua::string_property(&ExternalMessage::C2T_LOGIN_REQ::szSessionID);
    external_message_c2t_login_req_type["szMac"] = cqyh::lua::string_property(&ExternalMessage::C2T_LOGIN_REQ::szMac);
    external_message_c2t_login_req_type["dwCRC"] = sol::property([](const ExternalMessage::C2T_LOGIN_REQ& self) { return self.dwCRC; }, [](ExternalMessage::C2T_LOGIN_REQ& self, decltype(ExternalMessage::C2T_LOGIN_REQ::dwCRC) value) { self.dwCRC = value; });
    auto external_message_c2t_select_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_SELECT_ROLE>(L, external_message_namespace, "C2T_SELECT_ROLE", "ExternalMessage::C2T_SELECT_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_select_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_SELECT_ROLE(); });
    external_message_c2t_select_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_SELECT_ROLE>(new ExternalMessage::C2T_SELECT_ROLE()); });
    external_message_c2t_select_role_type["dwRoleID"] = sol::property([](const ExternalMessage::C2T_SELECT_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::C2T_SELECT_ROLE& self, decltype(ExternalMessage::C2T_SELECT_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2t_create_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_CREATE_ROLE>(L, external_message_namespace, "C2T_CREATE_ROLE", "ExternalMessage::C2T_CREATE_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_create_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_CREATE_ROLE(); });
    external_message_c2t_create_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_CREATE_ROLE>(new ExternalMessage::C2T_CREATE_ROLE()); });
    external_message_c2t_create_role_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2T_CREATE_ROLE::szRoleName);
    external_message_c2t_create_role_type["bySex"] = sol::property([](const ExternalMessage::C2T_CREATE_ROLE& self) { return self.bySex; }, [](ExternalMessage::C2T_CREATE_ROLE& self, decltype(ExternalMessage::C2T_CREATE_ROLE::bySex) value) { self.bySex = value; });
    external_message_c2t_create_role_type["byClass"] = sol::property([](const ExternalMessage::C2T_CREATE_ROLE& self) { return self.byClass; }, [](ExternalMessage::C2T_CREATE_ROLE& self, decltype(ExternalMessage::C2T_CREATE_ROLE::byClass) value) { self.byClass = value; });
    external_message_c2t_create_role_type["byHairType"] = sol::property([](const ExternalMessage::C2T_CREATE_ROLE& self) { return self.byHairType; }, [](ExternalMessage::C2T_CREATE_ROLE& self, decltype(ExternalMessage::C2T_CREATE_ROLE::byHairType) value) { self.byHairType = value; });
    external_message_c2t_create_role_type["byHairColor"] = sol::property([](const ExternalMessage::C2T_CREATE_ROLE& self) { return self.byHairColor; }, [](ExternalMessage::C2T_CREATE_ROLE& self, decltype(ExternalMessage::C2T_CREATE_ROLE::byHairColor) value) { self.byHairColor = value; });
    external_message_c2t_create_role_type["byFaceType"] = sol::property([](const ExternalMessage::C2T_CREATE_ROLE& self) { return self.byFaceType; }, [](ExternalMessage::C2T_CREATE_ROLE& self, decltype(ExternalMessage::C2T_CREATE_ROLE::byFaceType) value) { self.byFaceType = value; });
    external_message_c2t_create_role_type["bySkinColor"] = sol::property([](const ExternalMessage::C2T_CREATE_ROLE& self) { return self.bySkinColor; }, [](ExternalMessage::C2T_CREATE_ROLE& self, decltype(ExternalMessage::C2T_CREATE_ROLE::bySkinColor) value) { self.bySkinColor = value; });
    auto external_message_c2t_delete_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_DELETE_ROLE>(L, external_message_namespace, "C2T_DELETE_ROLE", "ExternalMessage::C2T_DELETE_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_delete_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_DELETE_ROLE(); });
    external_message_c2t_delete_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_DELETE_ROLE>(new ExternalMessage::C2T_DELETE_ROLE()); });
    external_message_c2t_delete_role_type["dwRoleID"] = sol::property([](const ExternalMessage::C2T_DELETE_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::C2T_DELETE_ROLE& self, decltype(ExternalMessage::C2T_DELETE_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2t_undelete_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_UNDELETE_ROLE>(L, external_message_namespace, "C2T_UNDELETE_ROLE", "ExternalMessage::C2T_UNDELETE_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_undelete_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_UNDELETE_ROLE(); });
    external_message_c2t_undelete_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_UNDELETE_ROLE>(new ExternalMessage::C2T_UNDELETE_ROLE()); });
    external_message_c2t_undelete_role_type["dwRoleID"] = sol::property([](const ExternalMessage::C2T_UNDELETE_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::C2T_UNDELETE_ROLE& self, decltype(ExternalMessage::C2T_UNDELETE_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2t_forever_delete_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_FOREVER_DELETE_ROLE>(L, external_message_namespace, "C2T_FOREVER_DELETE_ROLE", "ExternalMessage::C2T_FOREVER_DELETE_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_forever_delete_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_FOREVER_DELETE_ROLE(); });
    external_message_c2t_forever_delete_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_FOREVER_DELETE_ROLE>(new ExternalMessage::C2T_FOREVER_DELETE_ROLE()); });
    external_message_c2t_forever_delete_role_type["dwRoleID"] = sol::property([](const ExternalMessage::C2T_FOREVER_DELETE_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::C2T_FOREVER_DELETE_ROLE& self, decltype(ExternalMessage::C2T_FOREVER_DELETE_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2t_ping_type = cqyh::gen_msg::usertype<ExternalMessage::C2T_PING>(L, external_message_namespace, "C2T_PING", "ExternalMessage::C2T_PING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2t_ping_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2T_PING(); });
    external_message_c2t_ping_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2T_PING>(new ExternalMessage::C2T_PING()); });
    external_message_c2t_ping_type["dwPingTick"] = sol::property([](const ExternalMessage::C2T_PING& self) { return self.dwPingTick; }, [](ExternalMessage::C2T_PING& self, decltype(ExternalMessage::C2T_PING::dwPingTick) value) { self.dwPingTick = value; });
}

}
