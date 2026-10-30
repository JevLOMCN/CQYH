#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_mentorship_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_master_twitter_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_MASTER_TWITTER>(L, external_message_namespace, "C2G_MASTER_TWITTER", "ExternalMessage::C2G_MASTER_TWITTER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_master_twitter_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_MASTER_TWITTER(); });
    external_message_c2g_master_twitter_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_MASTER_TWITTER>(new ExternalMessage::C2G_MASTER_TWITTER()); });
    external_message_c2g_master_twitter_type["byTwitter"] = sol::property([](const ExternalMessage::C2G_MASTER_TWITTER& self) { return self.byTwitter; }, [](ExternalMessage::C2G_MASTER_TWITTER& self, decltype(ExternalMessage::C2G_MASTER_TWITTER::byTwitter) value) { self.byTwitter = value; });
    auto external_message_c2g_receive_master_reward_exp_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP>(L, external_message_namespace, "C2G_RECEIVE_MASTER_REWARD_EXP", "ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_receive_master_reward_exp_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP(); });
    external_message_c2g_receive_master_reward_exp_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP>(new ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP()); });
    external_message_c2g_receive_master_reward_exp_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP& self, decltype(ExternalMessage::C2G_RECEIVE_MASTER_REWARD_EXP::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2g_query_master_reward_exp_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_MASTER_REWARD_EXP>(L, external_message_namespace, "C2G_QUERY_MASTER_REWARD_EXP", "ExternalMessage::C2G_QUERY_MASTER_REWARD_EXP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_master_reward_exp_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_MASTER_REWARD_EXP(); });
    external_message_c2g_query_master_reward_exp_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_MASTER_REWARD_EXP>(new ExternalMessage::C2G_QUERY_MASTER_REWARD_EXP()); });
}

}
