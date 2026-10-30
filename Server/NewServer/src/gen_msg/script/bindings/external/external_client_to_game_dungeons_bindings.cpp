#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_dungeons_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_enter_dungeon_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ENTER_DUNGEON>(L, external_message_namespace, "C2G_ENTER_DUNGEON", "ExternalMessage::C2G_ENTER_DUNGEON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_enter_dungeon_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ENTER_DUNGEON(); });
    external_message_c2g_enter_dungeon_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ENTER_DUNGEON>(new ExternalMessage::C2G_ENTER_DUNGEON()); });
    external_message_c2g_enter_dungeon_type["bySelectedDifficulty"] = sol::property([](const ExternalMessage::C2G_ENTER_DUNGEON& self) { return self.bySelectedDifficulty; }, [](ExternalMessage::C2G_ENTER_DUNGEON& self, decltype(ExternalMessage::C2G_ENTER_DUNGEON::bySelectedDifficulty) value) { self.bySelectedDifficulty = value; });
    auto external_message_c2g_dungeon_select_reward_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DUNGEON_SELECT_REWARD>(L, external_message_namespace, "C2G_DUNGEON_SELECT_REWARD", "ExternalMessage::C2G_DUNGEON_SELECT_REWARD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_dungeon_select_reward_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DUNGEON_SELECT_REWARD(); });
    external_message_c2g_dungeon_select_reward_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DUNGEON_SELECT_REWARD>(new ExternalMessage::C2G_DUNGEON_SELECT_REWARD()); });
    external_message_c2g_dungeon_select_reward_type["byRewardType"] = sol::property([](const ExternalMessage::C2G_DUNGEON_SELECT_REWARD& self) { return self.byRewardType; }, [](ExternalMessage::C2G_DUNGEON_SELECT_REWARD& self, decltype(ExternalMessage::C2G_DUNGEON_SELECT_REWARD::byRewardType) value) { self.byRewardType = value; });
    auto external_message_c2g_dungeon_replay_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DUNGEON_REPLAY>(L, external_message_namespace, "C2G_DUNGEON_REPLAY", "ExternalMessage::C2G_DUNGEON_REPLAY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_dungeon_replay_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DUNGEON_REPLAY(); });
    external_message_c2g_dungeon_replay_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DUNGEON_REPLAY>(new ExternalMessage::C2G_DUNGEON_REPLAY()); });
    auto external_message_c2g_guild_open_dungeon_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_OPEN_DUNGEON>(L, external_message_namespace, "C2G_GUILD_OPEN_DUNGEON", "ExternalMessage::C2G_GUILD_OPEN_DUNGEON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_open_dungeon_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_OPEN_DUNGEON(); });
    external_message_c2g_guild_open_dungeon_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_OPEN_DUNGEON>(new ExternalMessage::C2G_GUILD_OPEN_DUNGEON()); });
    external_message_c2g_guild_open_dungeon_type["dwTemplateID"] = sol::property([](const ExternalMessage::C2G_GUILD_OPEN_DUNGEON& self) { return self.dwTemplateID; }, [](ExternalMessage::C2G_GUILD_OPEN_DUNGEON& self, decltype(ExternalMessage::C2G_GUILD_OPEN_DUNGEON::dwTemplateID) value) { self.dwTemplateID = value; });
    external_message_c2g_guild_open_dungeon_type["dwLeaderID"] = sol::property([](const ExternalMessage::C2G_GUILD_OPEN_DUNGEON& self) { return self.dwLeaderID; }, [](ExternalMessage::C2G_GUILD_OPEN_DUNGEON& self, decltype(ExternalMessage::C2G_GUILD_OPEN_DUNGEON::dwLeaderID) value) { self.dwLeaderID = value; });
    auto external_message_c2g_guild_enter_dungeon_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_ENTER_DUNGEON>(L, external_message_namespace, "C2G_GUILD_ENTER_DUNGEON", "ExternalMessage::C2G_GUILD_ENTER_DUNGEON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_enter_dungeon_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_ENTER_DUNGEON(); });
    external_message_c2g_guild_enter_dungeon_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_ENTER_DUNGEON>(new ExternalMessage::C2G_GUILD_ENTER_DUNGEON()); });
    external_message_c2g_guild_enter_dungeon_type["dwTemplateID"] = sol::property([](const ExternalMessage::C2G_GUILD_ENTER_DUNGEON& self) { return self.dwTemplateID; }, [](ExternalMessage::C2G_GUILD_ENTER_DUNGEON& self, decltype(ExternalMessage::C2G_GUILD_ENTER_DUNGEON::dwTemplateID) value) { self.dwTemplateID = value; });
    auto external_message_c2g_query_survey_path_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_SURVEY_PATH>(L, external_message_namespace, "C2G_QUERY_SURVEY_PATH", "ExternalMessage::C2G_QUERY_SURVEY_PATH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_survey_path_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_SURVEY_PATH(); });
    external_message_c2g_query_survey_path_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_SURVEY_PATH>(new ExternalMessage::C2G_QUERY_SURVEY_PATH()); });
}

}
