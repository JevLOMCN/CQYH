#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/t2c_message.h"
#include "../../../../../../Common/define/role_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_gate_to_client_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    external_message_namespace["t2c_message_begin"] = static_cast<lua_Number>(ExternalMessage::t2c_message_begin);
    external_message_namespace["t2c_error_notify"] = static_cast<lua_Number>(ExternalMessage::t2c_error_notify);
    external_message_namespace["t2c_login_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_login_ack);
    external_message_namespace["t2c_select_role_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_select_role_ack);
    external_message_namespace["t2c_sync_role_list"] = static_cast<lua_Number>(ExternalMessage::t2c_sync_role_list);
    external_message_namespace["t2c_create_role_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_create_role_ack);
    external_message_namespace["t2c_delete_role_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_delete_role_ack);
    external_message_namespace["t2c_undelete_role_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_undelete_role_ack);
    external_message_namespace["t2c_forever_delete_role_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_forever_delete_role_ack);
    external_message_namespace["t2c_return_select_role"] = static_cast<lua_Number>(ExternalMessage::t2c_return_select_role);
    external_message_namespace["t2c_ping_ack"] = static_cast<lua_Number>(ExternalMessage::t2c_ping_ack);
    external_message_namespace["t2c_message_end"] = static_cast<lua_Number>(ExternalMessage::t2c_message_end);
    auto external_message_t2c_error_notify_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_ERROR_NOTIFY>(L, external_message_namespace, "T2C_ERROR_NOTIFY", "ExternalMessage::T2C_ERROR_NOTIFY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_error_notify_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_ERROR_NOTIFY(); });
    external_message_t2c_error_notify_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_ERROR_NOTIFY>(new ExternalMessage::T2C_ERROR_NOTIFY()); });
    external_message_t2c_error_notify_type["nErrCode"] = sol::property([](const ExternalMessage::T2C_ERROR_NOTIFY& self) { return self.nErrCode; }, [](ExternalMessage::T2C_ERROR_NOTIFY& self, decltype(ExternalMessage::T2C_ERROR_NOTIFY::nErrCode) value) { self.nErrCode = value; });
    external_message_t2c_error_notify_type["nParam1"] = sol::property([](const ExternalMessage::T2C_ERROR_NOTIFY& self) { return self.nParam1; }, [](ExternalMessage::T2C_ERROR_NOTIFY& self, decltype(ExternalMessage::T2C_ERROR_NOTIFY::nParam1) value) { self.nParam1 = value; });
    external_message_t2c_error_notify_type["nParam2"] = sol::property([](const ExternalMessage::T2C_ERROR_NOTIFY& self) { return self.nParam2; }, [](ExternalMessage::T2C_ERROR_NOTIFY& self, decltype(ExternalMessage::T2C_ERROR_NOTIFY::nParam2) value) { self.nParam2 = value; });
    auto external_message_t2c_login_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_LOGIN_ACK>(L, external_message_namespace, "T2C_LOGIN_ACK", "ExternalMessage::T2C_LOGIN_ACK", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_login_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_LOGIN_ACK(); });
    external_message_t2c_login_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_LOGIN_ACK>(new ExternalMessage::T2C_LOGIN_ACK()); });
    external_message_t2c_login_ack_type["szUserName"] = cqyh::lua::string_property(&ExternalMessage::T2C_LOGIN_ACK::szUserName);
    external_message_t2c_login_ack_type["szUserPassword"] = cqyh::lua::string_property(&ExternalMessage::T2C_LOGIN_ACK::szUserPassword);
    external_message_t2c_login_ack_type["bAdult"] = sol::property([](const ExternalMessage::T2C_LOGIN_ACK& self) { return self.bAdult; }, [](ExternalMessage::T2C_LOGIN_ACK& self, decltype(ExternalMessage::T2C_LOGIN_ACK::bAdult) value) { self.bAdult = value; });
    external_message_t2c_login_ack_type["byEncryptCode"] = sol::property([](const ExternalMessage::T2C_LOGIN_ACK& self) { return self.byEncryptCode; }, [](ExternalMessage::T2C_LOGIN_ACK& self, decltype(ExternalMessage::T2C_LOGIN_ACK::byEncryptCode) value) { self.byEncryptCode = value; });
    external_message_t2c_login_ack_type["dwClientOption"] = sol::property([](const ExternalMessage::T2C_LOGIN_ACK& self) { return self.dwClientOption; }, [](ExternalMessage::T2C_LOGIN_ACK& self, decltype(ExternalMessage::T2C_LOGIN_ACK::dwClientOption) value) { self.dwClientOption = value; });
    external_message_t2c_login_ack_type["szMessageSize"] = cqyh::lua::string_property(&ExternalMessage::T2C_LOGIN_ACK::szMessageSize);
    auto external_message_login_role_data_type = cqyh::gen_msg::usertype<ExternalMessage::LOGIN_ROLE_DATA>(L, external_message_namespace, "LOGIN_ROLE_DATA", "ExternalMessage::LOGIN_ROLE_DATA");
    external_message_login_role_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::LOGIN_ROLE_DATA(); });
    external_message_login_role_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::LOGIN_ROLE_DATA>(new ExternalMessage::LOGIN_ROLE_DATA()); });
    external_message_login_role_data_type["dwRoleID"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.dwRoleID; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::dwRoleID) value) { self.dwRoleID = value; });
    external_message_login_role_data_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::LOGIN_ROLE_DATA::szRoleName);
    external_message_login_role_data_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::LOGIN_ROLE_DATA::szGuildName);
    external_message_login_role_data_type["byClass"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.byClass; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::byClass) value) { self.byClass = value; });
    external_message_login_role_data_type["bySex"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.bySex; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::bySex) value) { self.bySex = value; });
    external_message_login_role_data_type["byHairType"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.byHairType; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::byHairType) value) { self.byHairType = value; });
    external_message_login_role_data_type["byHairColor"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.byHairColor; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::byHairColor) value) { self.byHairColor = value; });
    external_message_login_role_data_type["byFaceType"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.byFaceType; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::byFaceType) value) { self.byFaceType = value; });
    external_message_login_role_data_type["bySkinColor"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.bySkinColor; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::bySkinColor) value) { self.bySkinColor = value; });
    external_message_login_role_data_type["byLevel"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.byLevel; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::byLevel) value) { self.byLevel = value; });
    external_message_login_role_data_type["nSceneTempID"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.nSceneTempID; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::nSceneTempID) value) { self.nSceneTempID = value; });
    external_message_login_role_data_type["byRefineFx"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.byRefineFx; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::byRefineFx) value) { self.byRefineFx = value; });
    external_message_login_role_data_type["nAvatar"] = cqyh::lua::array_property(&ExternalMessage::LOGIN_ROLE_DATA::nAvatar);
    external_message_login_role_data_type["dwSaveTime"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.dwSaveTime; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::dwSaveTime) value) { self.dwSaveTime = value; });
    external_message_login_role_data_type["dwDeleteTime"] = sol::property([](const ExternalMessage::LOGIN_ROLE_DATA& self) { return self.dwDeleteTime; }, [](ExternalMessage::LOGIN_ROLE_DATA& self, decltype(ExternalMessage::LOGIN_ROLE_DATA::dwDeleteTime) value) { self.dwDeleteTime = value; });
    auto external_message_t2c_sync_role_list_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_SYNC_ROLE_LIST>(L, external_message_namespace, "T2C_SYNC_ROLE_LIST", "ExternalMessage::T2C_SYNC_ROLE_LIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_sync_role_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_SYNC_ROLE_LIST(); });
    external_message_t2c_sync_role_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_SYNC_ROLE_LIST>(new ExternalMessage::T2C_SYNC_ROLE_LIST()); });
    external_message_t2c_sync_role_list_type["byCount"] = sol::property([](const ExternalMessage::T2C_SYNC_ROLE_LIST& self) { return self.byCount; }, [](ExternalMessage::T2C_SYNC_ROLE_LIST& self, decltype(ExternalMessage::T2C_SYNC_ROLE_LIST::byCount) value) { self.byCount = value; });
    external_message_t2c_sync_role_list_type["RoleData"] = cqyh::lua::array_property(&ExternalMessage::T2C_SYNC_ROLE_LIST::RoleData);
    auto external_message_t2c_select_role_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_SELECT_ROLE_ACK>(L, external_message_namespace, "T2C_SELECT_ROLE_ACK", "ExternalMessage::T2C_SELECT_ROLE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_select_role_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_SELECT_ROLE_ACK(); });
    external_message_t2c_select_role_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_SELECT_ROLE_ACK>(new ExternalMessage::T2C_SELECT_ROLE_ACK()); });
    external_message_t2c_select_role_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::T2C_SELECT_ROLE_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::T2C_SELECT_ROLE_ACK& self, decltype(ExternalMessage::T2C_SELECT_ROLE_ACK::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_t2c_create_role_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_CREATE_ROLE_ACK>(L, external_message_namespace, "T2C_CREATE_ROLE_ACK", "ExternalMessage::T2C_CREATE_ROLE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_create_role_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_CREATE_ROLE_ACK(); });
    external_message_t2c_create_role_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_CREATE_ROLE_ACK>(new ExternalMessage::T2C_CREATE_ROLE_ACK()); });
    external_message_t2c_create_role_ack_type["RoleData"] = &ExternalMessage::T2C_CREATE_ROLE_ACK::RoleData;
    auto external_message_t2c_delete_role_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_DELETE_ROLE_ACK>(L, external_message_namespace, "T2C_DELETE_ROLE_ACK", "ExternalMessage::T2C_DELETE_ROLE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_delete_role_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_DELETE_ROLE_ACK(); });
    external_message_t2c_delete_role_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_DELETE_ROLE_ACK>(new ExternalMessage::T2C_DELETE_ROLE_ACK()); });
    external_message_t2c_delete_role_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::T2C_DELETE_ROLE_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::T2C_DELETE_ROLE_ACK& self, decltype(ExternalMessage::T2C_DELETE_ROLE_ACK::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_t2c_undelete_role_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_UNDELETE_ROLE_ACK>(L, external_message_namespace, "T2C_UNDELETE_ROLE_ACK", "ExternalMessage::T2C_UNDELETE_ROLE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_undelete_role_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_UNDELETE_ROLE_ACK(); });
    external_message_t2c_undelete_role_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_UNDELETE_ROLE_ACK>(new ExternalMessage::T2C_UNDELETE_ROLE_ACK()); });
    external_message_t2c_undelete_role_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::T2C_UNDELETE_ROLE_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::T2C_UNDELETE_ROLE_ACK& self, decltype(ExternalMessage::T2C_UNDELETE_ROLE_ACK::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_t2c_forever_delete_role_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK>(L, external_message_namespace, "T2C_FOREVER_DELETE_ROLE_ACK", "ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_forever_delete_role_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK(); });
    external_message_t2c_forever_delete_role_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK>(new ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK()); });
    external_message_t2c_forever_delete_role_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK& self, decltype(ExternalMessage::T2C_FOREVER_DELETE_ROLE_ACK::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_t2c_return_select_role_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_RETURN_SELECT_ROLE>(L, external_message_namespace, "T2C_RETURN_SELECT_ROLE", "ExternalMessage::T2C_RETURN_SELECT_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_return_select_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_RETURN_SELECT_ROLE(); });
    external_message_t2c_return_select_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_RETURN_SELECT_ROLE>(new ExternalMessage::T2C_RETURN_SELECT_ROLE()); });
    auto external_message_t2c_ping_ack_type = cqyh::gen_msg::usertype<ExternalMessage::T2C_PING_ACK>(L, external_message_namespace, "T2C_PING_ACK", "ExternalMessage::T2C_PING_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_t2c_ping_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::T2C_PING_ACK(); });
    external_message_t2c_ping_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::T2C_PING_ACK>(new ExternalMessage::T2C_PING_ACK()); });
    external_message_t2c_ping_ack_type["dwPingTick"] = sol::property([](const ExternalMessage::T2C_PING_ACK& self) { return self.dwPingTick; }, [](ExternalMessage::T2C_PING_ACK& self, decltype(ExternalMessage::T2C_PING_ACK::dwPingTick) value) { self.dwPingTick = value; });
}

}
