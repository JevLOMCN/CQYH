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

void register_external_game_to_client_mentorship_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_master_reward_exp_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP>(L, external_message_namespace, "G2C_SYNC_MASTER_REWARD_EXP", "ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_master_reward_exp_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP(); });
    external_message_g2c_sync_master_reward_exp_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP>(new ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP()); });
    external_message_g2c_sync_master_reward_exp_type["dwMasterID"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP& self) { return self.dwMasterID; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::dwMasterID) value) { self.dwMasterID = value; });
    external_message_g2c_sync_master_reward_exp_type["nStudentRewardExp"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP& self) { return self.nStudentRewardExp; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::nStudentRewardExp) value) { self.nStudentRewardExp = value; });
    external_message_g2c_sync_master_reward_exp_type["nStudentRewardMoney"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP& self) { return self.nStudentRewardMoney; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::nStudentRewardMoney) value) { self.nStudentRewardMoney = value; });
    external_message_g2c_sync_master_reward_exp_type["TeacherRewardExp"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TeacherRewardExp);
    auto external_message_g2c_sync_master_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MASTER>(L, external_message_namespace, "G2C_SYNC_MASTER", "ExternalMessage::G2C_SYNC_MASTER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_master_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_MASTER(); });
    external_message_g2c_sync_master_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_MASTER>(new ExternalMessage::G2C_SYNC_MASTER()); });
    external_message_g2c_sync_master_type["dwCD"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER& self) { return self.dwCD; }, [](ExternalMessage::G2C_SYNC_MASTER& self, decltype(ExternalMessage::G2C_SYNC_MASTER::dwCD) value) { self.dwCD = value; });
    external_message_g2c_sync_master_type["byState"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER& self) { return self.byState; }, [](ExternalMessage::G2C_SYNC_MASTER& self, decltype(ExternalMessage::G2C_SYNC_MASTER::byState) value) { self.byState = value; });
    external_message_g2c_sync_master_type["byTwitter"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER& self) { return self.byTwitter; }, [](ExternalMessage::G2C_SYNC_MASTER& self, decltype(ExternalMessage::G2C_SYNC_MASTER::byTwitter) value) { self.byTwitter = value; });
    auto external_message_g2c_receive_master_reward_exp_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK>(L, external_message_namespace, "G2C_RECEIVE_MASTER_REWARD_EXP_ACK", "ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_receive_master_reward_exp_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK(); });
    external_message_g2c_receive_master_reward_exp_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK>(new ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK()); });
    external_message_g2c_receive_master_reward_exp_ack_type["bySuccess"] = sol::property([](const ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK& self) { return self.bySuccess; }, [](ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK& self, decltype(ExternalMessage::G2C_RECEIVE_MASTER_REWARD_EXP_ACK::bySuccess) value) { self.bySuccess = value; });
}

}
