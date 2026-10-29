#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_friends_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_friend_add_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_FRIEND_ADD>(L, external_message_namespace, "C2M_FRIEND_ADD", "ExternalMessage::C2M_FRIEND_ADD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_friend_add_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_FRIEND_ADD(); });
    external_message_c2m_friend_add_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_FRIEND_ADD>(new ExternalMessage::C2M_FRIEND_ADD()); });
    external_message_c2m_friend_add_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_FRIEND_ADD& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_FRIEND_ADD& self, decltype(ExternalMessage::C2M_FRIEND_ADD::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2m_friend_add_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2M_FRIEND_ADD::szRoleName);
    auto external_message_c2m_friend_del_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_FRIEND_DEL>(L, external_message_namespace, "C2M_FRIEND_DEL", "ExternalMessage::C2M_FRIEND_DEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_friend_del_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_FRIEND_DEL(); });
    external_message_c2m_friend_del_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_FRIEND_DEL>(new ExternalMessage::C2M_FRIEND_DEL()); });
    external_message_c2m_friend_del_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_FRIEND_DEL& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_FRIEND_DEL& self, decltype(ExternalMessage::C2M_FRIEND_DEL::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_friend_change_group_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_FRIEND_CHANGE_GROUP>(L, external_message_namespace, "C2M_FRIEND_CHANGE_GROUP", "ExternalMessage::C2M_FRIEND_CHANGE_GROUP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_friend_change_group_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_FRIEND_CHANGE_GROUP(); });
    external_message_c2m_friend_change_group_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_FRIEND_CHANGE_GROUP>(new ExternalMessage::C2M_FRIEND_CHANGE_GROUP()); });
    external_message_c2m_friend_change_group_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_FRIEND_CHANGE_GROUP& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_FRIEND_CHANGE_GROUP& self, decltype(ExternalMessage::C2M_FRIEND_CHANGE_GROUP::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2m_friend_change_group_type["byGroupID"] = sol::property([](const ExternalMessage::C2M_FRIEND_CHANGE_GROUP& self) { return self.byGroupID; }, [](ExternalMessage::C2M_FRIEND_CHANGE_GROUP& self, decltype(ExternalMessage::C2M_FRIEND_CHANGE_GROUP::byGroupID) value) { self.byGroupID = value; });
    auto external_message_c2m_friend_chat_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_FRIEND_CHAT>(L, external_message_namespace, "C2M_FRIEND_CHAT", "ExternalMessage::C2M_FRIEND_CHAT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_friend_chat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_FRIEND_CHAT(); });
    external_message_c2m_friend_chat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_FRIEND_CHAT>(new ExternalMessage::C2M_FRIEND_CHAT()); });
    external_message_c2m_friend_chat_type["dwTargetID"] = sol::property([](const ExternalMessage::C2M_FRIEND_CHAT& self) { return self.dwTargetID; }, [](ExternalMessage::C2M_FRIEND_CHAT& self, decltype(ExternalMessage::C2M_FRIEND_CHAT::dwTargetID) value) { self.dwTargetID = value; });
    external_message_c2m_friend_chat_type["byColor"] = sol::property([](const ExternalMessage::C2M_FRIEND_CHAT& self) { return self.byColor; }, [](ExternalMessage::C2M_FRIEND_CHAT& self, decltype(ExternalMessage::C2M_FRIEND_CHAT::byColor) value) { self.byColor = value; });
    external_message_c2m_friend_chat_type["szMessage"] = cqyh::lua::string_property(&ExternalMessage::C2M_FRIEND_CHAT::szMessage);
    auto external_message_c2m_personal_enemy_add_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_PERSONAL_ENEMY_ADD>(L, external_message_namespace, "C2M_PERSONAL_ENEMY_ADD", "ExternalMessage::C2M_PERSONAL_ENEMY_ADD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_personal_enemy_add_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_PERSONAL_ENEMY_ADD(); });
    external_message_c2m_personal_enemy_add_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_PERSONAL_ENEMY_ADD>(new ExternalMessage::C2M_PERSONAL_ENEMY_ADD()); });
    external_message_c2m_personal_enemy_add_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_PERSONAL_ENEMY_ADD& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_PERSONAL_ENEMY_ADD& self, decltype(ExternalMessage::C2M_PERSONAL_ENEMY_ADD::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_personal_enemy_del_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_PERSONAL_ENEMY_DEL>(L, external_message_namespace, "C2M_PERSONAL_ENEMY_DEL", "ExternalMessage::C2M_PERSONAL_ENEMY_DEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_personal_enemy_del_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_PERSONAL_ENEMY_DEL(); });
    external_message_c2m_personal_enemy_del_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_PERSONAL_ENEMY_DEL>(new ExternalMessage::C2M_PERSONAL_ENEMY_DEL()); });
    external_message_c2m_personal_enemy_del_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_PERSONAL_ENEMY_DEL& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_PERSONAL_ENEMY_DEL& self, decltype(ExternalMessage::C2M_PERSONAL_ENEMY_DEL::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_shield_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_SHIELD>(L, external_message_namespace, "C2M_SHIELD", "ExternalMessage::C2M_SHIELD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_shield_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_SHIELD(); });
    external_message_c2m_shield_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_SHIELD>(new ExternalMessage::C2M_SHIELD()); });
    external_message_c2m_shield_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_SHIELD& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_SHIELD& self, decltype(ExternalMessage::C2M_SHIELD::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2m_shield_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2M_SHIELD::szRoleName);
}

}
