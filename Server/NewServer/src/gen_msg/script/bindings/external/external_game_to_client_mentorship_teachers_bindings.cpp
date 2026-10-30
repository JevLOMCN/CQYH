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

void register_external_game_to_client_mentorship_teachers_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_master_reward_exp_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP>(L, external_message_namespace, "G2C_SYNC_MASTER_REWARD_EXP", "ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    auto external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP>(L, external_message_g2c_sync_master_reward_exp_type, "TEACHER_REWARD_EXP", "ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP");
    external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP(); });
    external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP>(new ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP()); });
    external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type["nExp"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self) { return self.nExp; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP::nExp) value) { self.nExp = value; });
    external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type["nMasterExp"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self) { return self.nMasterExp; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP::nMasterExp) value) { self.nMasterExp = value; });
    external_message_g2c_sync_master_reward_exp_teacher_reward_exp_type["nMoney"] = sol::property([](const ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self) { return self.nMoney; }, [](ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP& self, decltype(ExternalMessage::G2C_SYNC_MASTER_REWARD_EXP::TEACHER_REWARD_EXP::nMoney) value) { self.nMoney = value; });
}

}
