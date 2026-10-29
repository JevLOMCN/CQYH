#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_guild_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_query_guild_name_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_QUERY_GUILD_NAME>(L, external_message_namespace, "C2M_QUERY_GUILD_NAME", "ExternalMessage::C2M_QUERY_GUILD_NAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_query_guild_name_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_QUERY_GUILD_NAME(); });
    external_message_c2m_query_guild_name_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_QUERY_GUILD_NAME>(new ExternalMessage::C2M_QUERY_GUILD_NAME()); });
    external_message_c2m_query_guild_name_type["dwGuildID"] = sol::property([](const ExternalMessage::C2M_QUERY_GUILD_NAME& self) { return self.dwGuildID; }, [](ExternalMessage::C2M_QUERY_GUILD_NAME& self, decltype(ExternalMessage::C2M_QUERY_GUILD_NAME::dwGuildID) value) { self.dwGuildID = value; });
    external_message_c2m_query_guild_name_type["dwCRC"] = sol::property([](const ExternalMessage::C2M_QUERY_GUILD_NAME& self) { return self.dwCRC; }, [](ExternalMessage::C2M_QUERY_GUILD_NAME& self, decltype(ExternalMessage::C2M_QUERY_GUILD_NAME::dwCRC) value) { self.dwCRC = value; });
    auto external_message_c2m_view_guild_list_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_VIEW_GUILD_LIST>(L, external_message_namespace, "C2M_VIEW_GUILD_LIST", "ExternalMessage::C2M_VIEW_GUILD_LIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_view_guild_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_VIEW_GUILD_LIST(); });
    external_message_c2m_view_guild_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_VIEW_GUILD_LIST>(new ExternalMessage::C2M_VIEW_GUILD_LIST()); });
    external_message_c2m_view_guild_list_type["byViewType"] = sol::property([](const ExternalMessage::C2M_VIEW_GUILD_LIST& self) { return self.byViewType; }, [](ExternalMessage::C2M_VIEW_GUILD_LIST& self, decltype(ExternalMessage::C2M_VIEW_GUILD_LIST::byViewType) value) { self.byViewType = value; });
    external_message_c2m_view_guild_list_type["dwLastGuildID"] = sol::property([](const ExternalMessage::C2M_VIEW_GUILD_LIST& self) { return self.dwLastGuildID; }, [](ExternalMessage::C2M_VIEW_GUILD_LIST& self, decltype(ExternalMessage::C2M_VIEW_GUILD_LIST::dwLastGuildID) value) { self.dwLastGuildID = value; });
    auto external_message_c2m_more_guild_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MORE_GUILD_INFO>(L, external_message_namespace, "C2M_MORE_GUILD_INFO", "ExternalMessage::C2M_MORE_GUILD_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_more_guild_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MORE_GUILD_INFO(); });
    external_message_c2m_more_guild_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MORE_GUILD_INFO>(new ExternalMessage::C2M_MORE_GUILD_INFO()); });
    auto external_message_c2m_find_guild_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_FIND_GUILD>(L, external_message_namespace, "C2M_FIND_GUILD", "ExternalMessage::C2M_FIND_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_find_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_FIND_GUILD(); });
    external_message_c2m_find_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_FIND_GUILD>(new ExternalMessage::C2M_FIND_GUILD()); });
    external_message_c2m_find_guild_type["dwGuildID"] = sol::property([](const ExternalMessage::C2M_FIND_GUILD& self) { return self.dwGuildID; }, [](ExternalMessage::C2M_FIND_GUILD& self, decltype(ExternalMessage::C2M_FIND_GUILD::dwGuildID) value) { self.dwGuildID = value; });
    external_message_c2m_find_guild_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::C2M_FIND_GUILD::szGuildName);
    auto external_message_c2m_revert_guild_invitation_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_REVERT_GUILD_INVITATION>(L, external_message_namespace, "C2M_REVERT_GUILD_INVITATION", "ExternalMessage::C2M_REVERT_GUILD_INVITATION", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_revert_guild_invitation_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_REVERT_GUILD_INVITATION(); });
    external_message_c2m_revert_guild_invitation_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_REVERT_GUILD_INVITATION>(new ExternalMessage::C2M_REVERT_GUILD_INVITATION()); });
    external_message_c2m_revert_guild_invitation_type["byResult"] = sol::property([](const ExternalMessage::C2M_REVERT_GUILD_INVITATION& self) { return self.byResult; }, [](ExternalMessage::C2M_REVERT_GUILD_INVITATION& self, decltype(ExternalMessage::C2M_REVERT_GUILD_INVITATION::byResult) value) { self.byResult = value; });
    external_message_c2m_revert_guild_invitation_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_REVERT_GUILD_INVITATION& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_REVERT_GUILD_INVITATION& self, decltype(ExternalMessage::C2M_REVERT_GUILD_INVITATION::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_disband_guild_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_DISBAND_GUILD>(L, external_message_namespace, "C2M_DISBAND_GUILD", "ExternalMessage::C2M_DISBAND_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_disband_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_DISBAND_GUILD(); });
    external_message_c2m_disband_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_DISBAND_GUILD>(new ExternalMessage::C2M_DISBAND_GUILD()); });
    auto external_message_c2m_guild_notice_change_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_NOTICE_CHANGE>(L, external_message_namespace, "C2M_GUILD_NOTICE_CHANGE", "ExternalMessage::C2M_GUILD_NOTICE_CHANGE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_notice_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_NOTICE_CHANGE(); });
    external_message_c2m_guild_notice_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_NOTICE_CHANGE>(new ExternalMessage::C2M_GUILD_NOTICE_CHANGE()); });
    external_message_c2m_guild_notice_change_type["szNotice"] = cqyh::lua::string_property(&ExternalMessage::C2M_GUILD_NOTICE_CHANGE::szNotice);
    auto external_message_c2m_guild_forbid_chat_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_FORBID_CHAT>(L, external_message_namespace, "C2M_GUILD_FORBID_CHAT", "ExternalMessage::C2M_GUILD_FORBID_CHAT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_forbid_chat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_FORBID_CHAT(); });
    external_message_c2m_guild_forbid_chat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_FORBID_CHAT>(new ExternalMessage::C2M_GUILD_FORBID_CHAT()); });
    external_message_c2m_guild_forbid_chat_type["byOperation"] = sol::property([](const ExternalMessage::C2M_GUILD_FORBID_CHAT& self) { return self.byOperation; }, [](ExternalMessage::C2M_GUILD_FORBID_CHAT& self, decltype(ExternalMessage::C2M_GUILD_FORBID_CHAT::byOperation) value) { self.byOperation = value; });
    external_message_c2m_guild_forbid_chat_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_GUILD_FORBID_CHAT& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_GUILD_FORBID_CHAT& self, decltype(ExternalMessage::C2M_GUILD_FORBID_CHAT::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_guild_role_leave_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_ROLE_LEAVE>(L, external_message_namespace, "C2M_GUILD_ROLE_LEAVE", "ExternalMessage::C2M_GUILD_ROLE_LEAVE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_role_leave_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_ROLE_LEAVE(); });
    external_message_c2m_guild_role_leave_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_ROLE_LEAVE>(new ExternalMessage::C2M_GUILD_ROLE_LEAVE()); });
    auto external_message_c2m_guild_banish_role_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_BANISH_ROLE>(L, external_message_namespace, "C2M_GUILD_BANISH_ROLE", "ExternalMessage::C2M_GUILD_BANISH_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_banish_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_BANISH_ROLE(); });
    external_message_c2m_guild_banish_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_BANISH_ROLE>(new ExternalMessage::C2M_GUILD_BANISH_ROLE()); });
    external_message_c2m_guild_banish_role_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_GUILD_BANISH_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_GUILD_BANISH_ROLE& self, decltype(ExternalMessage::C2M_GUILD_BANISH_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_guild_demise_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_DEMISE>(L, external_message_namespace, "C2M_GUILD_DEMISE", "ExternalMessage::C2M_GUILD_DEMISE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_demise_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_DEMISE(); });
    external_message_c2m_guild_demise_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_DEMISE>(new ExternalMessage::C2M_GUILD_DEMISE()); });
    external_message_c2m_guild_demise_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_GUILD_DEMISE& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_GUILD_DEMISE& self, decltype(ExternalMessage::C2M_GUILD_DEMISE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2m_guild_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_ACHIEVEMENT>(L, external_message_namespace, "C2M_GUILD_ACHIEVEMENT", "ExternalMessage::C2M_GUILD_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_ACHIEVEMENT(); });
    external_message_c2m_guild_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_ACHIEVEMENT>(new ExternalMessage::C2M_GUILD_ACHIEVEMENT()); });
}

}
