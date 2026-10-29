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

void register_external_manage_to_client_activities_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_activity_team_type = cqyh::gen_msg::usertype<ExternalMessage::ACTIVITY_TEAM>(L, external_message_namespace, "ACTIVITY_TEAM", "ExternalMessage::ACTIVITY_TEAM");
    external_message_activity_team_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::ACTIVITY_TEAM(); });
    external_message_activity_team_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::ACTIVITY_TEAM>(new ExternalMessage::ACTIVITY_TEAM()); });
    external_message_activity_team_type["szTeamName"] = cqyh::lua::string_property(&ExternalMessage::ACTIVITY_TEAM::szTeamName);
    external_message_activity_team_type["nTeamID"] = sol::property([](const ExternalMessage::ACTIVITY_TEAM& self) { return self.nTeamID; }, [](ExternalMessage::ACTIVITY_TEAM& self, decltype(ExternalMessage::ACTIVITY_TEAM::nTeamID) value) { self.nTeamID = value; });
    external_message_activity_team_type["dwLeaderID"] = sol::property([](const ExternalMessage::ACTIVITY_TEAM& self) { return self.dwLeaderID; }, [](ExternalMessage::ACTIVITY_TEAM& self, decltype(ExternalMessage::ACTIVITY_TEAM::dwLeaderID) value) { self.dwLeaderID = value; });
    external_message_activity_team_type["nMemberID"] = cqyh::lua::array_property(&ExternalMessage::ACTIVITY_TEAM::nMemberID);
    auto external_message_m2c_activity_query_team_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ACTIVITY_QUERY_TEAM>(L, external_message_namespace, "M2C_ACTIVITY_QUERY_TEAM", "ExternalMessage::M2C_ACTIVITY_QUERY_TEAM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_activity_query_team_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ACTIVITY_QUERY_TEAM(); });
    external_message_m2c_activity_query_team_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ACTIVITY_QUERY_TEAM>(new ExternalMessage::M2C_ACTIVITY_QUERY_TEAM()); });
    external_message_m2c_activity_query_team_type["dwActivityType"] = sol::property([](const ExternalMessage::M2C_ACTIVITY_QUERY_TEAM& self) { return self.dwActivityType; }, [](ExternalMessage::M2C_ACTIVITY_QUERY_TEAM& self, decltype(ExternalMessage::M2C_ACTIVITY_QUERY_TEAM::dwActivityType) value) { self.dwActivityType = value; });
    external_message_m2c_activity_query_team_type["nActivityID"] = sol::property([](const ExternalMessage::M2C_ACTIVITY_QUERY_TEAM& self) { return self.nActivityID; }, [](ExternalMessage::M2C_ACTIVITY_QUERY_TEAM& self, decltype(ExternalMessage::M2C_ACTIVITY_QUERY_TEAM::nActivityID) value) { self.nActivityID = value; });
    external_message_m2c_activity_query_team_type["nPage"] = sol::property([](const ExternalMessage::M2C_ACTIVITY_QUERY_TEAM& self) { return self.nPage; }, [](ExternalMessage::M2C_ACTIVITY_QUERY_TEAM& self, decltype(ExternalMessage::M2C_ACTIVITY_QUERY_TEAM::nPage) value) { self.nPage = value; });
    external_message_m2c_activity_query_team_type["stRoomTeam"] = cqyh::lua::array_property(&ExternalMessage::M2C_ACTIVITY_QUERY_TEAM::stRoomTeam);
    auto external_message_m2c_activity_change_dungeon_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON>(L, external_message_namespace, "M2C_ACTIVITY_CHANGE_DUNGEON", "ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_activity_change_dungeon_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON(); });
    external_message_m2c_activity_change_dungeon_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON>(new ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON()); });
    external_message_m2c_activity_change_dungeon_type["dwActivityType"] = sol::property([](const ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON& self) { return self.dwActivityType; }, [](ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON& self, decltype(ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON::dwActivityType) value) { self.dwActivityType = value; });
    external_message_m2c_activity_change_dungeon_type["nActivityID"] = sol::property([](const ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON& self) { return self.nActivityID; }, [](ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON& self, decltype(ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON::nActivityID) value) { self.nActivityID = value; });
    external_message_m2c_activity_change_dungeon_type["bResult"] = sol::property([](const ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON& self) { return static_cast<bool>(self.bResult); }, [](ExternalMessage::M2C_ACTIVITY_CHANGE_DUNGEON& self, bool value) { self.bResult = value; });
    auto external_message_guild_dungeon_award_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_DUNGEON_AWARD_INFO>(L, external_message_namespace, "GUILD_DUNGEON_AWARD_INFO", "ExternalMessage::GUILD_DUNGEON_AWARD_INFO");
    external_message_guild_dungeon_award_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_DUNGEON_AWARD_INFO(); });
    external_message_guild_dungeon_award_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_DUNGEON_AWARD_INFO>(new ExternalMessage::GUILD_DUNGEON_AWARD_INFO()); });
    external_message_guild_dungeon_award_info_type["dwItemID"] = sol::property([](const ExternalMessage::GUILD_DUNGEON_AWARD_INFO& self) { return self.dwItemID; }, [](ExternalMessage::GUILD_DUNGEON_AWARD_INFO& self, decltype(ExternalMessage::GUILD_DUNGEON_AWARD_INFO::dwItemID) value) { self.dwItemID = value; });
    external_message_guild_dungeon_award_info_type["byCount"] = sol::property([](const ExternalMessage::GUILD_DUNGEON_AWARD_INFO& self) { return self.byCount; }, [](ExternalMessage::GUILD_DUNGEON_AWARD_INFO& self, decltype(ExternalMessage::GUILD_DUNGEON_AWARD_INFO::byCount) value) { self.byCount = value; });
    auto external_message_m2c_guild_dungeon_award_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_DUNGEON_AWARD>(L, external_message_namespace, "M2C_GUILD_DUNGEON_AWARD", "ExternalMessage::M2C_GUILD_DUNGEON_AWARD", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_dungeon_award_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_DUNGEON_AWARD(); });
    external_message_m2c_guild_dungeon_award_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_DUNGEON_AWARD>(new ExternalMessage::M2C_GUILD_DUNGEON_AWARD()); });
    external_message_m2c_guild_dungeon_award_type["dwOwnerID"] = sol::property([](const ExternalMessage::M2C_GUILD_DUNGEON_AWARD& self) { return self.dwOwnerID; }, [](ExternalMessage::M2C_GUILD_DUNGEON_AWARD& self, decltype(ExternalMessage::M2C_GUILD_DUNGEON_AWARD::dwOwnerID) value) { self.dwOwnerID = value; });
    external_message_m2c_guild_dungeon_award_type["byItemCount"] = sol::property([](const ExternalMessage::M2C_GUILD_DUNGEON_AWARD& self) { return self.byItemCount; }, [](ExternalMessage::M2C_GUILD_DUNGEON_AWARD& self, decltype(ExternalMessage::M2C_GUILD_DUNGEON_AWARD::byItemCount) value) { self.byItemCount = value; });
    external_message_m2c_guild_dungeon_award_type["arrItem"] = cqyh::lua::array_property(&ExternalMessage::M2C_GUILD_DUNGEON_AWARD::arrItem);
}

}
