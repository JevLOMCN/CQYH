#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_progression_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_spirit_exp_change_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SPIRIT_EXP_CHANGE>(L, external_message_namespace, "C2G_SPIRIT_EXP_CHANGE", "ExternalMessage::C2G_SPIRIT_EXP_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_spirit_exp_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SPIRIT_EXP_CHANGE(); });
    external_message_c2g_spirit_exp_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SPIRIT_EXP_CHANGE>(new ExternalMessage::C2G_SPIRIT_EXP_CHANGE()); });
    external_message_c2g_spirit_exp_change_type["bIsOnline"] = sol::property([](const ExternalMessage::C2G_SPIRIT_EXP_CHANGE& self) { return static_cast<bool>(self.bIsOnline); }, [](ExternalMessage::C2G_SPIRIT_EXP_CHANGE& self, bool value) { self.bIsOnline = value; });
    external_message_c2g_spirit_exp_change_type["bUseMoney"] = sol::property([](const ExternalMessage::C2G_SPIRIT_EXP_CHANGE& self) { return static_cast<bool>(self.bUseMoney); }, [](ExternalMessage::C2G_SPIRIT_EXP_CHANGE& self, bool value) { self.bUseMoney = value; });
    auto external_message_c2g_exercise_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_EXERCISE_INFO>(L, external_message_namespace, "C2G_EXERCISE_INFO", "ExternalMessage::C2G_EXERCISE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_exercise_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_EXERCISE_INFO(); });
    external_message_c2g_exercise_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_EXERCISE_INFO>(new ExternalMessage::C2G_EXERCISE_INFO()); });
    auto external_message_c2g_add_exercise_time_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ADD_EXERCISE_TIME>(L, external_message_namespace, "C2G_ADD_EXERCISE_TIME", "ExternalMessage::C2G_ADD_EXERCISE_TIME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_add_exercise_time_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ADD_EXERCISE_TIME(); });
    external_message_c2g_add_exercise_time_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ADD_EXERCISE_TIME>(new ExternalMessage::C2G_ADD_EXERCISE_TIME()); });
    external_message_c2g_add_exercise_time_type["nHour"] = sol::property([](const ExternalMessage::C2G_ADD_EXERCISE_TIME& self) { return self.nHour; }, [](ExternalMessage::C2G_ADD_EXERCISE_TIME& self, decltype(ExternalMessage::C2G_ADD_EXERCISE_TIME::nHour) value) { self.nHour = value; });
    auto external_message_c2g_get_exercise_exp_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GET_EXERCISE_EXP>(L, external_message_namespace, "C2G_GET_EXERCISE_EXP", "ExternalMessage::C2G_GET_EXERCISE_EXP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_get_exercise_exp_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GET_EXERCISE_EXP(); });
    external_message_c2g_get_exercise_exp_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GET_EXERCISE_EXP>(new ExternalMessage::C2G_GET_EXERCISE_EXP()); });
}

}
