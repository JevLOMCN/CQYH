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

void register_external_manage_to_client_friends_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_friend_info_type = cqyh::gen_msg::usertype<ExternalMessage::FRIEND_INFO>(L, external_message_namespace, "FRIEND_INFO", "ExternalMessage::FRIEND_INFO");
    external_message_friend_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::FRIEND_INFO(); });
    external_message_friend_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::FRIEND_INFO>(new ExternalMessage::FRIEND_INFO()); });
    external_message_friend_info_type["dwRoleID"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.dwRoleID; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::dwRoleID) value) { self.dwRoleID = value; });
    external_message_friend_info_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::FRIEND_INFO::szRoleName);
    external_message_friend_info_type["szSignature"] = cqyh::lua::string_property(&ExternalMessage::FRIEND_INFO::szSignature);
    external_message_friend_info_type["byClass"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.byClass; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::byClass) value) { self.byClass = value; });
    external_message_friend_info_type["bySex"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.bySex; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::bySex) value) { self.bySex = value; });
    external_message_friend_info_type["byFriendState"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.byFriendState; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::byFriendState) value) { self.byFriendState = value; });
    external_message_friend_info_type["nSceneTempID"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.nSceneTempID; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::nSceneTempID) value) { self.nSceneTempID = value; });
    external_message_friend_info_type["bySceneTempID"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.bySceneTempID; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::bySceneTempID) value) { self.bySceneTempID = value; });
    external_message_friend_info_type["wIntimacy"] = sol::property([](const ExternalMessage::FRIEND_INFO& self) { return self.wIntimacy; }, [](ExternalMessage::FRIEND_INFO& self, decltype(ExternalMessage::FRIEND_INFO::wIntimacy) value) { self.wIntimacy = value; });
    auto external_message_friend_info_with_group_type = cqyh::gen_msg::usertype<ExternalMessage::FRIEND_INFO_WITH_GROUP>(L, external_message_namespace, "FRIEND_INFO_WITH_GROUP", "ExternalMessage::FRIEND_INFO_WITH_GROUP");
    external_message_friend_info_with_group_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::FRIEND_INFO_WITH_GROUP(); });
    external_message_friend_info_with_group_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::FRIEND_INFO_WITH_GROUP>(new ExternalMessage::FRIEND_INFO_WITH_GROUP()); });
    external_message_friend_info_with_group_type["stFriendInfo"] = &ExternalMessage::FRIEND_INFO_WITH_GROUP::stFriendInfo;
    external_message_friend_info_with_group_type["byGroup"] = sol::property([](const ExternalMessage::FRIEND_INFO_WITH_GROUP& self) { return self.byGroup; }, [](ExternalMessage::FRIEND_INFO_WITH_GROUP& self, decltype(ExternalMessage::FRIEND_INFO_WITH_GROUP::byGroup) value) { self.byGroup = value; });
    external_message_friend_info_with_group_type["bMutual"] = sol::property([](const ExternalMessage::FRIEND_INFO_WITH_GROUP& self) { return self.bMutual; }, [](ExternalMessage::FRIEND_INFO_WITH_GROUP& self, decltype(ExternalMessage::FRIEND_INFO_WITH_GROUP::bMutual) value) { self.bMutual = value; });
    auto external_message_m2c_friend_list_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_LIST>(L, external_message_namespace, "M2C_FRIEND_LIST", "ExternalMessage::M2C_FRIEND_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_LIST(); });
    external_message_m2c_friend_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_LIST>(new ExternalMessage::M2C_FRIEND_LIST()); });
    external_message_m2c_friend_list_type["byFansCount"] = sol::property([](const ExternalMessage::M2C_FRIEND_LIST& self) { return self.byFansCount; }, [](ExternalMessage::M2C_FRIEND_LIST& self, decltype(ExternalMessage::M2C_FRIEND_LIST::byFansCount) value) { self.byFansCount = value; });
    external_message_m2c_friend_list_type["byFriendCount"] = sol::property([](const ExternalMessage::M2C_FRIEND_LIST& self) { return self.byFriendCount; }, [](ExternalMessage::M2C_FRIEND_LIST& self, decltype(ExternalMessage::M2C_FRIEND_LIST::byFriendCount) value) { self.byFriendCount = value; });
    external_message_m2c_friend_list_type["stFriendInfoWithGroup"] = cqyh::lua::array_property(&ExternalMessage::M2C_FRIEND_LIST::stFriendInfoWithGroup);
    auto external_message_m2c_friend_info_change_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_INFO_CHANGE>(L, external_message_namespace, "M2C_FRIEND_INFO_CHANGE", "ExternalMessage::M2C_FRIEND_INFO_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_info_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_INFO_CHANGE(); });
    external_message_m2c_friend_info_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_INFO_CHANGE>(new ExternalMessage::M2C_FRIEND_INFO_CHANGE()); });
    external_message_m2c_friend_info_change_type["stFriendInfo"] = &ExternalMessage::M2C_FRIEND_INFO_CHANGE::stFriendInfo;
    auto external_message_m2c_friend_add_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_ADD>(L, external_message_namespace, "M2C_FRIEND_ADD", "ExternalMessage::M2C_FRIEND_ADD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_add_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_ADD(); });
    external_message_m2c_friend_add_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_ADD>(new ExternalMessage::M2C_FRIEND_ADD()); });
    external_message_m2c_friend_add_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_FRIEND_ADD& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_FRIEND_ADD& self, decltype(ExternalMessage::M2C_FRIEND_ADD::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_friend_add_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_FRIEND_ADD::szRoleName);
    external_message_m2c_friend_add_type["bMutual"] = sol::property([](const ExternalMessage::M2C_FRIEND_ADD& self) { return self.bMutual; }, [](ExternalMessage::M2C_FRIEND_ADD& self, decltype(ExternalMessage::M2C_FRIEND_ADD::bMutual) value) { self.bMutual = value; });
    auto external_message_m2c_friend_del_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_DEL>(L, external_message_namespace, "M2C_FRIEND_DEL", "ExternalMessage::M2C_FRIEND_DEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_del_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_DEL(); });
    external_message_m2c_friend_del_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_DEL>(new ExternalMessage::M2C_FRIEND_DEL()); });
    external_message_m2c_friend_del_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_FRIEND_DEL& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_FRIEND_DEL& self, decltype(ExternalMessage::M2C_FRIEND_DEL::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_m2c_friend_added_by_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_ADDED_BY>(L, external_message_namespace, "M2C_FRIEND_ADDED_BY", "ExternalMessage::M2C_FRIEND_ADDED_BY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_added_by_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_ADDED_BY(); });
    external_message_m2c_friend_added_by_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_ADDED_BY>(new ExternalMessage::M2C_FRIEND_ADDED_BY()); });
    external_message_m2c_friend_added_by_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_FRIEND_ADDED_BY& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_FRIEND_ADDED_BY& self, decltype(ExternalMessage::M2C_FRIEND_ADDED_BY::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_friend_added_by_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_FRIEND_ADDED_BY::szRoleName);
    auto external_message_m2c_friend_deleted_by_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_DELETED_BY>(L, external_message_namespace, "M2C_FRIEND_DELETED_BY", "ExternalMessage::M2C_FRIEND_DELETED_BY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_deleted_by_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_DELETED_BY(); });
    external_message_m2c_friend_deleted_by_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_DELETED_BY>(new ExternalMessage::M2C_FRIEND_DELETED_BY()); });
    external_message_m2c_friend_deleted_by_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_FRIEND_DELETED_BY& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_FRIEND_DELETED_BY& self, decltype(ExternalMessage::M2C_FRIEND_DELETED_BY::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_friend_deleted_by_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_FRIEND_DELETED_BY::szRoleName);
    auto external_message_m2c_friend_chat_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_FRIEND_CHAT>(L, external_message_namespace, "M2C_FRIEND_CHAT", "ExternalMessage::M2C_FRIEND_CHAT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_friend_chat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_FRIEND_CHAT(); });
    external_message_m2c_friend_chat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_FRIEND_CHAT>(new ExternalMessage::M2C_FRIEND_CHAT()); });
    external_message_m2c_friend_chat_type["dwFrom"] = sol::property([](const ExternalMessage::M2C_FRIEND_CHAT& self) { return self.dwFrom; }, [](ExternalMessage::M2C_FRIEND_CHAT& self, decltype(ExternalMessage::M2C_FRIEND_CHAT::dwFrom) value) { self.dwFrom = value; });
    external_message_m2c_friend_chat_type["byColor"] = sol::property([](const ExternalMessage::M2C_FRIEND_CHAT& self) { return self.byColor; }, [](ExternalMessage::M2C_FRIEND_CHAT& self, decltype(ExternalMessage::M2C_FRIEND_CHAT::byColor) value) { self.byColor = value; });
    external_message_m2c_friend_chat_type["szMessage"] = cqyh::lua::string_property(&ExternalMessage::M2C_FRIEND_CHAT::szMessage);
    auto external_message_m2c_personal_enemy_add_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_PERSONAL_ENEMY_ADD>(L, external_message_namespace, "M2C_PERSONAL_ENEMY_ADD", "ExternalMessage::M2C_PERSONAL_ENEMY_ADD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_personal_enemy_add_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_PERSONAL_ENEMY_ADD(); });
    external_message_m2c_personal_enemy_add_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_PERSONAL_ENEMY_ADD>(new ExternalMessage::M2C_PERSONAL_ENEMY_ADD()); });
    external_message_m2c_personal_enemy_add_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_PERSONAL_ENEMY_ADD& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_PERSONAL_ENEMY_ADD& self, decltype(ExternalMessage::M2C_PERSONAL_ENEMY_ADD::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_m2c_personal_enemy_del_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_PERSONAL_ENEMY_DEL>(L, external_message_namespace, "M2C_PERSONAL_ENEMY_DEL", "ExternalMessage::M2C_PERSONAL_ENEMY_DEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_personal_enemy_del_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_PERSONAL_ENEMY_DEL(); });
    external_message_m2c_personal_enemy_del_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_PERSONAL_ENEMY_DEL>(new ExternalMessage::M2C_PERSONAL_ENEMY_DEL()); });
    external_message_m2c_personal_enemy_del_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_PERSONAL_ENEMY_DEL& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_PERSONAL_ENEMY_DEL& self, decltype(ExternalMessage::M2C_PERSONAL_ENEMY_DEL::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_m2c_sync_friend_intimacy_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_SYNC_FRIEND_INTIMACY>(L, external_message_namespace, "M2C_SYNC_FRIEND_INTIMACY", "ExternalMessage::M2C_SYNC_FRIEND_INTIMACY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_sync_friend_intimacy_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_SYNC_FRIEND_INTIMACY(); });
    external_message_m2c_sync_friend_intimacy_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_SYNC_FRIEND_INTIMACY>(new ExternalMessage::M2C_SYNC_FRIEND_INTIMACY()); });
    external_message_m2c_sync_friend_intimacy_type["dwFriendRoleID"] = sol::property([](const ExternalMessage::M2C_SYNC_FRIEND_INTIMACY& self) { return self.dwFriendRoleID; }, [](ExternalMessage::M2C_SYNC_FRIEND_INTIMACY& self, decltype(ExternalMessage::M2C_SYNC_FRIEND_INTIMACY::dwFriendRoleID) value) { self.dwFriendRoleID = value; });
    external_message_m2c_sync_friend_intimacy_type["nIntimacy"] = sol::property([](const ExternalMessage::M2C_SYNC_FRIEND_INTIMACY& self) { return self.nIntimacy; }, [](ExternalMessage::M2C_SYNC_FRIEND_INTIMACY& self, decltype(ExternalMessage::M2C_SYNC_FRIEND_INTIMACY::nIntimacy) value) { self.nIntimacy = value; });
}

}
