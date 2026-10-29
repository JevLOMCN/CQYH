#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_activities_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_team_change_activity_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO>(L, external_message_namespace, "C2M_TEAM_CHANGE_ACTIVITY_INFO", "ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_team_change_activity_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO(); });
    external_message_c2m_team_change_activity_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO>(new ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO()); });
    external_message_c2m_team_change_activity_info_type["dwActivityType"] = sol::property([](const ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO& self) { return self.dwActivityType; }, [](ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO& self, decltype(ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO::dwActivityType) value) { self.dwActivityType = value; });
    external_message_c2m_team_change_activity_info_type["nActivityID"] = sol::property([](const ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO& self) { return self.nActivityID; }, [](ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO& self, decltype(ExternalMessage::C2M_TEAM_CHANGE_ACTIVITY_INFO::nActivityID) value) { self.nActivityID = value; });
    auto external_message_c2m_team_query_activity_team_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM>(L, external_message_namespace, "C2M_TEAM_QUERY_ACTIVITY_TEAM", "ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_team_query_activity_team_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM(); });
    external_message_c2m_team_query_activity_team_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM>(new ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM()); });
    external_message_c2m_team_query_activity_team_type["nPgae"] = sol::property([](const ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM& self) { return self.nPgae; }, [](ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM& self, decltype(ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM::nPgae) value) { self.nPgae = value; });
    external_message_c2m_team_query_activity_team_type["nActivityID"] = sol::property([](const ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM& self) { return self.nActivityID; }, [](ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM& self, decltype(ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM::nActivityID) value) { self.nActivityID = value; });
    external_message_c2m_team_query_activity_team_type["dwActivityType"] = sol::property([](const ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM& self) { return self.dwActivityType; }, [](ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM& self, decltype(ExternalMessage::C2M_TEAM_QUERY_ACTIVITY_TEAM::dwActivityType) value) { self.dwActivityType = value; });
    auto external_message_c2m_team_remove_activity_team_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_TEAM_REMOVE_ACTIVITY_TEAM>(L, external_message_namespace, "C2M_TEAM_REMOVE_ACTIVITY_TEAM", "ExternalMessage::C2M_TEAM_REMOVE_ACTIVITY_TEAM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_team_remove_activity_team_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_TEAM_REMOVE_ACTIVITY_TEAM(); });
    external_message_c2m_team_remove_activity_team_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_TEAM_REMOVE_ACTIVITY_TEAM>(new ExternalMessage::C2M_TEAM_REMOVE_ACTIVITY_TEAM()); });
    auto external_message_c2m_guild_activity_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_ACTIVITY>(L, external_message_namespace, "C2M_GUILD_ACTIVITY", "ExternalMessage::C2M_GUILD_ACTIVITY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_activity_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_ACTIVITY(); });
    external_message_c2m_guild_activity_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_ACTIVITY>(new ExternalMessage::C2M_GUILD_ACTIVITY()); });
    external_message_c2m_guild_activity_type["nActivityID"] = sol::property([](const ExternalMessage::C2M_GUILD_ACTIVITY& self) { return self.nActivityID; }, [](ExternalMessage::C2M_GUILD_ACTIVITY& self, decltype(ExternalMessage::C2M_GUILD_ACTIVITY::nActivityID) value) { self.nActivityID = value; });
    auto external_message_c2m_award_use_number_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AWARD_USE_NUMBER>(L, external_message_namespace, "C2M_AWARD_USE_NUMBER", "ExternalMessage::C2M_AWARD_USE_NUMBER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_award_use_number_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AWARD_USE_NUMBER(); });
    external_message_c2m_award_use_number_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AWARD_USE_NUMBER>(new ExternalMessage::C2M_AWARD_USE_NUMBER()); });
    external_message_c2m_award_use_number_type["chAwardNumber"] = cqyh::lua::string_property(&ExternalMessage::C2M_AWARD_USE_NUMBER::chAwardNumber);
}

}
