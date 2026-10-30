#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_achievements_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_complete_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_COMPLETE_ACHIEVEMENT>(L, external_message_namespace, "C2G_COMPLETE_ACHIEVEMENT", "ExternalMessage::C2G_COMPLETE_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_complete_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_COMPLETE_ACHIEVEMENT(); });
    external_message_c2g_complete_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_COMPLETE_ACHIEVEMENT>(new ExternalMessage::C2G_COMPLETE_ACHIEVEMENT()); });
    external_message_c2g_complete_achievement_type["nAchievementID"] = sol::property([](const ExternalMessage::C2G_COMPLETE_ACHIEVEMENT& self) { return self.nAchievementID; }, [](ExternalMessage::C2G_COMPLETE_ACHIEVEMENT& self, decltype(ExternalMessage::C2G_COMPLETE_ACHIEVEMENT::nAchievementID) value) { self.nAchievementID = value; });
    auto external_message_c2g_reward_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_REWARD_ACHIEVEMENT>(L, external_message_namespace, "C2G_REWARD_ACHIEVEMENT", "ExternalMessage::C2G_REWARD_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_reward_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_REWARD_ACHIEVEMENT(); });
    external_message_c2g_reward_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_REWARD_ACHIEVEMENT>(new ExternalMessage::C2G_REWARD_ACHIEVEMENT()); });
    external_message_c2g_reward_achievement_type["nAchievementID"] = sol::property([](const ExternalMessage::C2G_REWARD_ACHIEVEMENT& self) { return self.nAchievementID; }, [](ExternalMessage::C2G_REWARD_ACHIEVEMENT& self, decltype(ExternalMessage::C2G_REWARD_ACHIEVEMENT::nAchievementID) value) { self.nAchievementID = value; });
    auto external_message_c2g_compare_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_COMPARE_ACHIEVEMENT>(L, external_message_namespace, "C2G_COMPARE_ACHIEVEMENT", "ExternalMessage::C2G_COMPARE_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_compare_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_COMPARE_ACHIEVEMENT(); });
    external_message_c2g_compare_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_COMPARE_ACHIEVEMENT>(new ExternalMessage::C2G_COMPARE_ACHIEVEMENT()); });
    external_message_c2g_compare_achievement_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_COMPARE_ACHIEVEMENT& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_COMPARE_ACHIEVEMENT& self, decltype(ExternalMessage::C2G_COMPARE_ACHIEVEMENT::dwRoleID) value) { self.dwRoleID = value; });
}

}
