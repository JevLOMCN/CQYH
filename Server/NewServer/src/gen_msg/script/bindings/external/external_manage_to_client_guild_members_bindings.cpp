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

void register_external_manage_to_client_guild_members_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_guild_member_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_MEMBER_INFO>(L, external_message_namespace, "GUILD_MEMBER_INFO", "ExternalMessage::GUILD_MEMBER_INFO");
    external_message_guild_member_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_MEMBER_INFO(); });
    external_message_guild_member_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_MEMBER_INFO>(new ExternalMessage::GUILD_MEMBER_INFO()); });
    external_message_guild_member_info_type["dwRoleID"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.dwRoleID; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::dwRoleID) value) { self.dwRoleID = value; });
    external_message_guild_member_info_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::GUILD_MEMBER_INFO::szRoleName);
    external_message_guild_member_info_type["byPosition"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.byPosition; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::byPosition) value) { self.byPosition = value; });
    external_message_guild_member_info_type["byLevel"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.byLevel; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::byLevel) value) { self.byLevel = value; });
    external_message_guild_member_info_type["byClass"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.byClass; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::byClass) value) { self.byClass = value; });
    external_message_guild_member_info_type["nSceneID"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.nSceneID; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::nSceneID) value) { self.nSceneID = value; });
    external_message_guild_member_info_type["nLastLoginTime"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.nLastLoginTime; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::nLastLoginTime) value) { self.nLastLoginTime = value; });
    external_message_guild_member_info_type["nNextSalary"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.nNextSalary; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::nNextSalary) value) { self.nNextSalary = value; });
    external_message_guild_member_info_type["byFlag"] = sol::property([](const ExternalMessage::GUILD_MEMBER_INFO& self) { return self.byFlag; }, [](ExternalMessage::GUILD_MEMBER_INFO& self, decltype(ExternalMessage::GUILD_MEMBER_INFO::byFlag) value) { self.byFlag = value; });
    auto external_message_m2c_apply_to_join_guild_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK>(L, external_message_namespace, "M2C_APPLY_TO_JOIN_GUILD_ACK", "ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_apply_to_join_guild_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK(); });
    external_message_m2c_apply_to_join_guild_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK>(new ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK()); });
    external_message_m2c_apply_to_join_guild_ack_type["dwGuildID"] = sol::property([](const ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK& self) { return self.dwGuildID; }, [](ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK& self, decltype(ExternalMessage::M2C_APPLY_TO_JOIN_GUILD_ACK::dwGuildID) value) { self.dwGuildID = value; });
    auto external_message_m2c_get_apply_guild_list_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK>(L, external_message_namespace, "M2C_GET_APPLY_GUILD_LIST_ACK", "ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_get_apply_guild_list_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK(); });
    external_message_m2c_get_apply_guild_list_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK>(new ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK()); });
    external_message_m2c_get_apply_guild_list_ack_type["wAppCount"] = sol::property([](const ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK& self) { return self.wAppCount; }, [](ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK& self, decltype(ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK::wAppCount) value) { self.wAppCount = value; });
    external_message_m2c_get_apply_guild_list_ack_type["info"] = cqyh::lua::array_property(&ExternalMessage::M2C_GET_APPLY_GUILD_LIST_ACK::info);
    auto external_message_m2c_revert_guild_apply_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK>(L, external_message_namespace, "M2C_REVERT_GUILD_APPLY_ACK", "ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_revert_guild_apply_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK(); });
    external_message_m2c_revert_guild_apply_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK>(new ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK()); });
    external_message_m2c_revert_guild_apply_ack_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK& self, decltype(ExternalMessage::M2C_REVERT_GUILD_APPLY_ACK::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_m2c_guild_add_member_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_ADD_MEMBER_NTF>(L, external_message_namespace, "M2C_GUILD_ADD_MEMBER_NTF", "ExternalMessage::M2C_GUILD_ADD_MEMBER_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_add_member_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_ADD_MEMBER_NTF(); });
    external_message_m2c_guild_add_member_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_ADD_MEMBER_NTF>(new ExternalMessage::M2C_GUILD_ADD_MEMBER_NTF()); });
    external_message_m2c_guild_add_member_ntf_type["info"] = &ExternalMessage::M2C_GUILD_ADD_MEMBER_NTF::info;
    auto external_message_m2c_guild_del_member_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF>(L, external_message_namespace, "M2C_GUILD_DEL_MEMBER_NTF", "ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_del_member_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF(); });
    external_message_m2c_guild_del_member_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF>(new ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF()); });
    external_message_m2c_guild_del_member_ntf_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF& self, decltype(ExternalMessage::M2C_GUILD_DEL_MEMBER_NTF::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_m2c_be_invited_to_join_guild_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD>(L, external_message_namespace, "M2C_BE_INVITED_TO_JOIN_GUILD", "ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_be_invited_to_join_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD(); });
    external_message_m2c_be_invited_to_join_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD>(new ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD()); });
    external_message_m2c_be_invited_to_join_guild_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD& self, decltype(ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_be_invited_to_join_guild_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD::szRoleName);
    external_message_m2c_be_invited_to_join_guild_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::M2C_BE_INVITED_TO_JOIN_GUILD::szGuildName);
    auto external_message_m2c_invite_to_join_guild_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK>(L, external_message_namespace, "M2C_INVITE_TO_JOIN_GUILD_ACK", "ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_invite_to_join_guild_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK(); });
    external_message_m2c_invite_to_join_guild_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK>(new ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK()); });
    external_message_m2c_invite_to_join_guild_ack_type["byReplyType"] = sol::property([](const ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK& self) { return self.byReplyType; }, [](ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK& self, decltype(ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK::byReplyType) value) { self.byReplyType = value; });
    external_message_m2c_invite_to_join_guild_ack_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_INVITE_TO_JOIN_GUILD_ACK::szRoleName);
    auto external_message_m2c_guild_change_position_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF>(L, external_message_namespace, "M2C_GUILD_CHANGE_POSITION_NTF", "ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_change_position_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF(); });
    external_message_m2c_guild_change_position_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF>(new ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF()); });
    external_message_m2c_guild_change_position_ntf_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF& self, decltype(ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_guild_change_position_ntf_type["byPosition"] = sol::property([](const ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF& self) { return self.byPosition; }, [](ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF& self, decltype(ExternalMessage::M2C_GUILD_CHANGE_POSITION_NTF::byPosition) value) { self.byPosition = value; });
    auto external_message_m2c_guild_member_salary_change_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE>(L, external_message_namespace, "M2C_GUILD_MEMBER_SALARY_CHANGE", "ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_member_salary_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE(); });
    external_message_m2c_guild_member_salary_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE>(new ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE()); });
    external_message_m2c_guild_member_salary_change_type["dwSalary"] = sol::property([](const ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE& self) { return self.dwSalary; }, [](ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE& self, decltype(ExternalMessage::M2C_GUILD_MEMBER_SALARY_CHANGE::dwSalary) value) { self.dwSalary = value; });
    auto external_message_m2c_guild_join_guild_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_JOIN_GUILD_NTF>(L, external_message_namespace, "M2C_GUILD_JOIN_GUILD_NTF", "ExternalMessage::M2C_GUILD_JOIN_GUILD_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_join_guild_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_JOIN_GUILD_NTF(); });
    external_message_m2c_guild_join_guild_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_JOIN_GUILD_NTF>(new ExternalMessage::M2C_GUILD_JOIN_GUILD_NTF()); });
    external_message_m2c_guild_join_guild_ntf_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::M2C_GUILD_JOIN_GUILD_NTF::szGuildName);
}

}
