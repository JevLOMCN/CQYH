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

void register_external_manage_to_client_pvp_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_cancel_join_battle_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK>(L, external_message_namespace, "M2C_CANCEL_JOIN_BATTLE_ACK", "ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_cancel_join_battle_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK(); });
    external_message_m2c_cancel_join_battle_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK>(new ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK()); });
    external_message_m2c_cancel_join_battle_ack_type["dwCancelJoinRoleID"] = sol::property([](const ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK& self) { return self.dwCancelJoinRoleID; }, [](ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK& self, decltype(ExternalMessage::M2C_CANCEL_JOIN_BATTLE_ACK::dwCancelJoinRoleID) value) { self.dwCancelJoinRoleID = value; });
    auto external_message_m2c_join_battle_confirm_req_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ>(L, external_message_namespace, "M2C_JOIN_BATTLE_CONFIRM_REQ", "ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_join_battle_confirm_req_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ(); });
    external_message_m2c_join_battle_confirm_req_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ>(new ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ()); });
    external_message_m2c_join_battle_confirm_req_type["dwSceneTempID"] = sol::property([](const ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ& self) { return self.dwSceneTempID; }, [](ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ& self, decltype(ExternalMessage::M2C_JOIN_BATTLE_CONFIRM_REQ::dwSceneTempID) value) { self.dwSceneTempID = value; });
    auto external_message_m2c_arena_fund_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ARENA_FUND>(L, external_message_namespace, "M2C_ARENA_FUND", "ExternalMessage::M2C_ARENA_FUND", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_arena_fund_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ARENA_FUND(); });
    external_message_m2c_arena_fund_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ARENA_FUND>(new ExternalMessage::M2C_ARENA_FUND()); });
    external_message_m2c_arena_fund_type["llTotalFund"] = sol::property([](const ExternalMessage::M2C_ARENA_FUND& self) { return self.llTotalFund; }, [](ExternalMessage::M2C_ARENA_FUND& self, decltype(ExternalMessage::M2C_ARENA_FUND::llTotalFund) value) { self.llTotalFund = value; });
    external_message_m2c_arena_fund_type["llFundPerTicket"] = sol::property([](const ExternalMessage::M2C_ARENA_FUND& self) { return self.llFundPerTicket; }, [](ExternalMessage::M2C_ARENA_FUND& self, decltype(ExternalMessage::M2C_ARENA_FUND::llFundPerTicket) value) { self.llFundPerTicket = value; });
    auto external_message_m2c_arena_get_data_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ARENA_GET_DATA_ACK>(L, external_message_namespace, "M2C_ARENA_GET_DATA_ACK", "ExternalMessage::M2C_ARENA_GET_DATA_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_arena_get_data_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ARENA_GET_DATA_ACK(); });
    external_message_m2c_arena_get_data_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ARENA_GET_DATA_ACK>(new ExternalMessage::M2C_ARENA_GET_DATA_ACK()); });
    external_message_m2c_arena_get_data_ack_type["qwTotalFund"] = sol::property([](const ExternalMessage::M2C_ARENA_GET_DATA_ACK& self) { return self.qwTotalFund; }, [](ExternalMessage::M2C_ARENA_GET_DATA_ACK& self, decltype(ExternalMessage::M2C_ARENA_GET_DATA_ACK::qwTotalFund) value) { self.qwTotalFund = value; });
    external_message_m2c_arena_get_data_ack_type["byChampionMaxVictoryTimes"] = sol::property([](const ExternalMessage::M2C_ARENA_GET_DATA_ACK& self) { return self.byChampionMaxVictoryTimes; }, [](ExternalMessage::M2C_ARENA_GET_DATA_ACK& self, decltype(ExternalMessage::M2C_ARENA_GET_DATA_ACK::byChampionMaxVictoryTimes) value) { self.byChampionMaxVictoryTimes = value; });
    external_message_m2c_arena_get_data_ack_type["byRank"] = sol::property([](const ExternalMessage::M2C_ARENA_GET_DATA_ACK& self) { return self.byRank; }, [](ExternalMessage::M2C_ARENA_GET_DATA_ACK& self, decltype(ExternalMessage::M2C_ARENA_GET_DATA_ACK::byRank) value) { self.byRank = value; });
    external_message_m2c_arena_get_data_ack_type["nRewardMoney"] = sol::property([](const ExternalMessage::M2C_ARENA_GET_DATA_ACK& self) { return self.nRewardMoney; }, [](ExternalMessage::M2C_ARENA_GET_DATA_ACK& self, decltype(ExternalMessage::M2C_ARENA_GET_DATA_ACK::nRewardMoney) value) { self.nRewardMoney = value; });
    external_message_m2c_arena_get_data_ack_type["dwRewardItemID"] = sol::property([](const ExternalMessage::M2C_ARENA_GET_DATA_ACK& self) { return self.dwRewardItemID; }, [](ExternalMessage::M2C_ARENA_GET_DATA_ACK& self, decltype(ExternalMessage::M2C_ARENA_GET_DATA_ACK::dwRewardItemID) value) { self.dwRewardItemID = value; });
    auto external_message_m2c_arena_end_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ARENA_END>(L, external_message_namespace, "M2C_ARENA_END", "ExternalMessage::M2C_ARENA_END", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_arena_end_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ARENA_END(); });
    external_message_m2c_arena_end_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ARENA_END>(new ExternalMessage::M2C_ARENA_END()); });
    auto external_message_m2c_arena_match_begin_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ARENA_MATCH_BEGIN_NTF>(L, external_message_namespace, "M2C_ARENA_MATCH_BEGIN_NTF", "ExternalMessage::M2C_ARENA_MATCH_BEGIN_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_arena_match_begin_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ARENA_MATCH_BEGIN_NTF(); });
    external_message_m2c_arena_match_begin_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ARENA_MATCH_BEGIN_NTF>(new ExternalMessage::M2C_ARENA_MATCH_BEGIN_NTF()); });
}

}
