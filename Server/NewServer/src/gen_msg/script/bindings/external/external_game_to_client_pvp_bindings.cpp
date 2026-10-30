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

void register_external_game_to_client_pvp_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_pk_value_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_PK_VALUE>(L, external_message_namespace, "G2C_SYNC_PK_VALUE", "ExternalMessage::G2C_SYNC_PK_VALUE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_pk_value_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_PK_VALUE(); });
    external_message_g2c_sync_pk_value_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_PK_VALUE>(new ExternalMessage::G2C_SYNC_PK_VALUE()); });
    external_message_g2c_sync_pk_value_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_VALUE& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_PK_VALUE& self, decltype(ExternalMessage::G2C_SYNC_PK_VALUE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_pk_value_type["dwPkValue"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_VALUE& self) { return self.dwPkValue; }, [](ExternalMessage::G2C_SYNC_PK_VALUE& self, decltype(ExternalMessage::G2C_SYNC_PK_VALUE::dwPkValue) value) { self.dwPkValue = value; });
    auto external_message_g2c_sync_pk_result_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_PK_RESULT>(L, external_message_namespace, "G2C_SYNC_PK_RESULT", "ExternalMessage::G2C_SYNC_PK_RESULT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_pk_result_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_PK_RESULT(); });
    external_message_g2c_sync_pk_result_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_PK_RESULT>(new ExternalMessage::G2C_SYNC_PK_RESULT()); });
    external_message_g2c_sync_pk_result_type["byPKResult"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_RESULT& self) { return self.byPKResult; }, [](ExternalMessage::G2C_SYNC_PK_RESULT& self, decltype(ExternalMessage::G2C_SYNC_PK_RESULT::byPKResult) value) { self.byPKResult = value; });
    external_message_g2c_sync_pk_result_type["dwKillerRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_RESULT& self) { return self.dwKillerRoleID; }, [](ExternalMessage::G2C_SYNC_PK_RESULT& self, decltype(ExternalMessage::G2C_SYNC_PK_RESULT::dwKillerRoleID) value) { self.dwKillerRoleID = value; });
    external_message_g2c_sync_pk_result_type["dwVictimRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_RESULT& self) { return self.dwVictimRoleID; }, [](ExternalMessage::G2C_SYNC_PK_RESULT& self, decltype(ExternalMessage::G2C_SYNC_PK_RESULT::dwVictimRoleID) value) { self.dwVictimRoleID = value; });
    external_message_g2c_sync_pk_result_type["shChangePkValue"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_RESULT& self) { return self.shChangePkValue; }, [](ExternalMessage::G2C_SYNC_PK_RESULT& self, decltype(ExternalMessage::G2C_SYNC_PK_RESULT::shChangePkValue) value) { self.shChangePkValue = value; });
    auto external_message_g2c_sync_pk_mode_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_PK_MODE>(L, external_message_namespace, "G2C_SYNC_PK_MODE", "ExternalMessage::G2C_SYNC_PK_MODE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_pk_mode_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_PK_MODE(); });
    external_message_g2c_sync_pk_mode_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_PK_MODE>(new ExternalMessage::G2C_SYNC_PK_MODE()); });
    external_message_g2c_sync_pk_mode_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_MODE& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_PK_MODE& self, decltype(ExternalMessage::G2C_SYNC_PK_MODE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_pk_mode_type["byPkMode"] = sol::property([](const ExternalMessage::G2C_SYNC_PK_MODE& self) { return self.byPkMode; }, [](ExternalMessage::G2C_SYNC_PK_MODE& self, decltype(ExternalMessage::G2C_SYNC_PK_MODE::byPkMode) value) { self.byPkMode = value; });
    auto external_message_g2c_sync_arena_simple_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO>(L, external_message_namespace, "G2C_SYNC_ARENA_SIMPLE_INFO", "ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_arena_simple_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO(); });
    external_message_g2c_sync_arena_simple_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO>(new ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO()); });
    external_message_g2c_sync_arena_simple_info_type["nTime"] = sol::property([](const ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO& self) { return self.nTime; }, [](ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO& self, decltype(ExternalMessage::G2C_SYNC_ARENA_SIMPLE_INFO::nTime) value) { self.nTime = value; });
    auto external_message_g2c_sync_arena_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ARENA>(L, external_message_namespace, "G2C_SYNC_ARENA", "ExternalMessage::G2C_SYNC_ARENA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_arena_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ARENA(); });
    external_message_g2c_sync_arena_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ARENA>(new ExternalMessage::G2C_SYNC_ARENA()); });
    external_message_g2c_sync_arena_type["nTime"] = sol::property([](const ExternalMessage::G2C_SYNC_ARENA& self) { return self.nTime; }, [](ExternalMessage::G2C_SYNC_ARENA& self, decltype(ExternalMessage::G2C_SYNC_ARENA::nTime) value) { self.nTime = value; });
    auto external_message_g2c_arena_status_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_STATUS>(L, external_message_namespace, "G2C_ARENA_STATUS", "ExternalMessage::G2C_ARENA_STATUS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_status_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_STATUS(); });
    external_message_g2c_arena_status_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_STATUS>(new ExternalMessage::G2C_ARENA_STATUS()); });
    external_message_g2c_arena_status_type["nStatus"] = sol::property([](const ExternalMessage::G2C_ARENA_STATUS& self) { return self.nStatus; }, [](ExternalMessage::G2C_ARENA_STATUS& self, decltype(ExternalMessage::G2C_ARENA_STATUS::nStatus) value) { self.nStatus = value; });
    auto external_message_g2c_arena_result_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_RESULT>(L, external_message_namespace, "G2C_ARENA_RESULT", "ExternalMessage::G2C_ARENA_RESULT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_result_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_RESULT(); });
    external_message_g2c_arena_result_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_RESULT>(new ExternalMessage::G2C_ARENA_RESULT()); });
    external_message_g2c_arena_result_type["byWin"] = sol::property([](const ExternalMessage::G2C_ARENA_RESULT& self) { return self.byWin; }, [](ExternalMessage::G2C_ARENA_RESULT& self, decltype(ExternalMessage::G2C_ARENA_RESULT::byWin) value) { self.byWin = value; });
    external_message_g2c_arena_result_type["nTime"] = sol::property([](const ExternalMessage::G2C_ARENA_RESULT& self) { return self.nTime; }, [](ExternalMessage::G2C_ARENA_RESULT& self, decltype(ExternalMessage::G2C_ARENA_RESULT::nTime) value) { self.nTime = value; });
    external_message_g2c_arena_result_type["nDmg"] = sol::property([](const ExternalMessage::G2C_ARENA_RESULT& self) { return self.nDmg; }, [](ExternalMessage::G2C_ARENA_RESULT& self, decltype(ExternalMessage::G2C_ARENA_RESULT::nDmg) value) { self.nDmg = value; });
    external_message_g2c_arena_result_type["nRewardMoney"] = sol::property([](const ExternalMessage::G2C_ARENA_RESULT& self) { return self.nRewardMoney; }, [](ExternalMessage::G2C_ARENA_RESULT& self, decltype(ExternalMessage::G2C_ARENA_RESULT::nRewardMoney) value) { self.nRewardMoney = value; });
    external_message_g2c_arena_result_type["nScore"] = sol::property([](const ExternalMessage::G2C_ARENA_RESULT& self) { return self.nScore; }, [](ExternalMessage::G2C_ARENA_RESULT& self, decltype(ExternalMessage::G2C_ARENA_RESULT::nScore) value) { self.nScore = value; });
    auto external_message_g2c_arena_sync_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_SYNC_DATA>(L, external_message_namespace, "G2C_ARENA_SYNC_DATA", "ExternalMessage::G2C_ARENA_SYNC_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_sync_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_SYNC_DATA(); });
    external_message_g2c_arena_sync_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_SYNC_DATA>(new ExternalMessage::G2C_ARENA_SYNC_DATA()); });
    external_message_g2c_arena_sync_data_type["bySeasonNum"] = sol::property([](const ExternalMessage::G2C_ARENA_SYNC_DATA& self) { return self.bySeasonNum; }, [](ExternalMessage::G2C_ARENA_SYNC_DATA& self, decltype(ExternalMessage::G2C_ARENA_SYNC_DATA::bySeasonNum) value) { self.bySeasonNum = value; });
    external_message_g2c_arena_sync_data_type["byIsInSeason"] = sol::property([](const ExternalMessage::G2C_ARENA_SYNC_DATA& self) { return self.byIsInSeason; }, [](ExternalMessage::G2C_ARENA_SYNC_DATA& self, decltype(ExternalMessage::G2C_ARENA_SYNC_DATA::byIsInSeason) value) { self.byIsInSeason = value; });
    external_message_g2c_arena_sync_data_type["byIsJJCMatchTime"] = sol::property([](const ExternalMessage::G2C_ARENA_SYNC_DATA& self) { return self.byIsJJCMatchTime; }, [](ExternalMessage::G2C_ARENA_SYNC_DATA& self, decltype(ExternalMessage::G2C_ARENA_SYNC_DATA::byIsJJCMatchTime) value) { self.byIsJJCMatchTime = value; });
    external_message_g2c_arena_sync_data_type["byCurSeasonVicTime"] = sol::property([](const ExternalMessage::G2C_ARENA_SYNC_DATA& self) { return self.byCurSeasonVicTime; }, [](ExternalMessage::G2C_ARENA_SYNC_DATA& self, decltype(ExternalMessage::G2C_ARENA_SYNC_DATA::byCurSeasonVicTime) value) { self.byCurSeasonVicTime = value; });
    external_message_g2c_arena_sync_data_type["byPointSeasonVicTime"] = sol::property([](const ExternalMessage::G2C_ARENA_SYNC_DATA& self) { return self.byPointSeasonVicTime; }, [](ExternalMessage::G2C_ARENA_SYNC_DATA& self, decltype(ExternalMessage::G2C_ARENA_SYNC_DATA::byPointSeasonVicTime) value) { self.byPointSeasonVicTime = value; });
    auto external_message_g2c_arena_join_new_season_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK>(L, external_message_namespace, "G2C_ARENA_JOIN_NEW_SEASON_ACK", "ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_join_new_season_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK(); });
    external_message_g2c_arena_join_new_season_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK>(new ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK()); });
    external_message_g2c_arena_join_new_season_ack_type["bySeasonNum"] = sol::property([](const ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK& self) { return self.bySeasonNum; }, [](ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK& self, decltype(ExternalMessage::G2C_ARENA_JOIN_NEW_SEASON_ACK::bySeasonNum) value) { self.bySeasonNum = value; });
    auto external_message_g2c_arena_quit_season_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK>(L, external_message_namespace, "G2C_ARENA_QUIT_SEASON_ACK", "ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_quit_season_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK(); });
    external_message_g2c_arena_quit_season_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK>(new ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK()); });
    external_message_g2c_arena_quit_season_ack_type["bySeasonNum"] = sol::property([](const ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK& self) { return self.bySeasonNum; }, [](ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK& self, decltype(ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK::bySeasonNum) value) { self.bySeasonNum = value; });
    external_message_g2c_arena_quit_season_ack_type["byPointVictoryTimes"] = sol::property([](const ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK& self) { return self.byPointVictoryTimes; }, [](ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK& self, decltype(ExternalMessage::G2C_ARENA_QUIT_SEASON_ACK::byPointVictoryTimes) value) { self.byPointVictoryTimes = value; });
    auto external_message_g2c_arena_win_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_WIN>(L, external_message_namespace, "G2C_ARENA_WIN", "ExternalMessage::G2C_ARENA_WIN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_win_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_WIN(); });
    external_message_g2c_arena_win_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_WIN>(new ExternalMessage::G2C_ARENA_WIN()); });
    auto external_message_g2c_arena_defeat_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ARENA_DEFEAT>(L, external_message_namespace, "G2C_ARENA_DEFEAT", "ExternalMessage::G2C_ARENA_DEFEAT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_arena_defeat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ARENA_DEFEAT(); });
    external_message_g2c_arena_defeat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ARENA_DEFEAT>(new ExternalMessage::G2C_ARENA_DEFEAT()); });
}

}
