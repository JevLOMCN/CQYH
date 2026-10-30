#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/g2c_message.h"
#include "../../../../../../Common/define/dds_def.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/item_def.h"
#include "../../../../../../Common/define/master_def.h"
#include "../../../../../../Common/define/npc_def.h"
#include "../../../../../../Common/define/quest_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_game_to_client_dungeons_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_dungeon_finish_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DUNGEON_FINISH_DATA>(L, external_message_namespace, "G2C_DUNGEON_FINISH_DATA", "ExternalMessage::G2C_DUNGEON_FINISH_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_dungeon_finish_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DUNGEON_FINISH_DATA(); });
    external_message_g2c_dungeon_finish_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DUNGEON_FINISH_DATA>(new ExternalMessage::G2C_DUNGEON_FINISH_DATA()); });
    external_message_g2c_dungeon_finish_data_type["nTotalTime"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nTotalTime; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nTotalTime) value) { self.nTotalTime = value; });
    external_message_g2c_dungeon_finish_data_type["byTimeScore"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.byTimeScore; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::byTimeScore) value) { self.byTimeScore = value; });
    external_message_g2c_dungeon_finish_data_type["nKillNpcCount"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nKillNpcCount; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nKillNpcCount) value) { self.nKillNpcCount = value; });
    external_message_g2c_dungeon_finish_data_type["nDestroyDoodadCount"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nDestroyDoodadCount; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nDestroyDoodadCount) value) { self.nDestroyDoodadCount = value; });
    external_message_g2c_dungeon_finish_data_type["byAttackScore"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.byAttackScore; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::byAttackScore) value) { self.byAttackScore = value; });
    external_message_g2c_dungeon_finish_data_type["byGoalResult"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.byGoalResult; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::byGoalResult) value) { self.byGoalResult = value; });
    external_message_g2c_dungeon_finish_data_type["byGoalScore"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.byGoalScore; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::byGoalScore) value) { self.byGoalScore = value; });
    external_message_g2c_dungeon_finish_data_type["byTotalScore"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.byTotalScore; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::byTotalScore) value) { self.byTotalScore = value; });
    external_message_g2c_dungeon_finish_data_type["nNormalExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nNormalExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nNormalExp) value) { self.nNormalExp = value; });
    external_message_g2c_dungeon_finish_data_type["nTeamExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nTeamExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nTeamExp) value) { self.nTeamExp = value; });
    external_message_g2c_dungeon_finish_data_type["nGuildExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nGuildExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nGuildExp) value) { self.nGuildExp = value; });
    external_message_g2c_dungeon_finish_data_type["nMasterExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nMasterExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nMasterExp) value) { self.nMasterExp = value; });
    external_message_g2c_dungeon_finish_data_type["nVipExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nVipExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nVipExp) value) { self.nVipExp = value; });
    external_message_g2c_dungeon_finish_data_type["nFirstPassExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nFirstPassExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nFirstPassExp) value) { self.nFirstPassExp = value; });
    external_message_g2c_dungeon_finish_data_type["nActivityExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nActivityExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nActivityExp) value) { self.nActivityExp = value; });
    external_message_g2c_dungeon_finish_data_type["nTotalExp"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_DATA& self) { return self.nTotalExp; }, [](ExternalMessage::G2C_DUNGEON_FINISH_DATA& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_DATA::nTotalExp) value) { self.nTotalExp = value; });
    auto external_message_g2c_sync_dungeon_goal_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_DUNGEON_GOAL>(L, external_message_namespace, "G2C_SYNC_DUNGEON_GOAL", "ExternalMessage::G2C_SYNC_DUNGEON_GOAL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_dungeon_goal_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_DUNGEON_GOAL(); });
    external_message_g2c_sync_dungeon_goal_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_DUNGEON_GOAL>(new ExternalMessage::G2C_SYNC_DUNGEON_GOAL()); });
    external_message_g2c_sync_dungeon_goal_type["nBossKillCount"] = sol::property([](const ExternalMessage::G2C_SYNC_DUNGEON_GOAL& self) { return self.nBossKillCount; }, [](ExternalMessage::G2C_SYNC_DUNGEON_GOAL& self, decltype(ExternalMessage::G2C_SYNC_DUNGEON_GOAL::nBossKillCount) value) { self.nBossKillCount = value; });
    external_message_g2c_sync_dungeon_goal_type["nEliteNpcKillCount"] = sol::property([](const ExternalMessage::G2C_SYNC_DUNGEON_GOAL& self) { return self.nEliteNpcKillCount; }, [](ExternalMessage::G2C_SYNC_DUNGEON_GOAL& self, decltype(ExternalMessage::G2C_SYNC_DUNGEON_GOAL::nEliteNpcKillCount) value) { self.nEliteNpcKillCount = value; });
    auto external_message_g2c_dungeon_finish_score_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DUNGEON_FINISH_SCORE>(L, external_message_namespace, "G2C_DUNGEON_FINISH_SCORE", "ExternalMessage::G2C_DUNGEON_FINISH_SCORE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_dungeon_finish_score_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DUNGEON_FINISH_SCORE(); });
    external_message_g2c_dungeon_finish_score_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DUNGEON_FINISH_SCORE>(new ExternalMessage::G2C_DUNGEON_FINISH_SCORE()); });
    external_message_g2c_dungeon_finish_score_type["byCount"] = sol::property([](const ExternalMessage::G2C_DUNGEON_FINISH_SCORE& self) { return self.byCount; }, [](ExternalMessage::G2C_DUNGEON_FINISH_SCORE& self, decltype(ExternalMessage::G2C_DUNGEON_FINISH_SCORE::byCount) value) { self.byCount = value; });
    external_message_g2c_dungeon_finish_score_type["stScore"] = cqyh::lua::array_property(&ExternalMessage::G2C_DUNGEON_FINISH_SCORE::stScore);
    auto external_message_g2c_dungeon_reward_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DUNGEON_REWARD>(L, external_message_namespace, "G2C_DUNGEON_REWARD", "ExternalMessage::G2C_DUNGEON_REWARD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_dungeon_reward_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DUNGEON_REWARD(); });
    external_message_g2c_dungeon_reward_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DUNGEON_REWARD>(new ExternalMessage::G2C_DUNGEON_REWARD()); });
    external_message_g2c_dungeon_reward_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_DUNGEON_REWARD& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_DUNGEON_REWARD& self, decltype(ExternalMessage::G2C_DUNGEON_REWARD::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_dungeon_reward_type["byRewardType"] = sol::property([](const ExternalMessage::G2C_DUNGEON_REWARD& self) { return self.byRewardType; }, [](ExternalMessage::G2C_DUNGEON_REWARD& self, decltype(ExternalMessage::G2C_DUNGEON_REWARD::byRewardType) value) { self.byRewardType = value; });
    external_message_g2c_dungeon_reward_type["dwRewardID"] = sol::property([](const ExternalMessage::G2C_DUNGEON_REWARD& self) { return self.dwRewardID; }, [](ExternalMessage::G2C_DUNGEON_REWARD& self, decltype(ExternalMessage::G2C_DUNGEON_REWARD::dwRewardID) value) { self.dwRewardID = value; });
    external_message_g2c_dungeon_reward_type["dwRewardCount"] = sol::property([](const ExternalMessage::G2C_DUNGEON_REWARD& self) { return self.dwRewardCount; }, [](ExternalMessage::G2C_DUNGEON_REWARD& self, decltype(ExternalMessage::G2C_DUNGEON_REWARD::dwRewardCount) value) { self.dwRewardCount = value; });
    auto external_message_g2c_query_survey_path_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_QUERY_SURVEY_PATH>(L, external_message_namespace, "G2C_QUERY_SURVEY_PATH", "ExternalMessage::G2C_QUERY_SURVEY_PATH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_query_survey_path_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_QUERY_SURVEY_PATH(); });
    external_message_g2c_query_survey_path_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_QUERY_SURVEY_PATH>(new ExternalMessage::G2C_QUERY_SURVEY_PATH()); });
    external_message_g2c_query_survey_path_type["szQuerySurveyPath"] = cqyh::lua::string_property(&ExternalMessage::G2C_QUERY_SURVEY_PATH::szQuerySurveyPath);
}

}
