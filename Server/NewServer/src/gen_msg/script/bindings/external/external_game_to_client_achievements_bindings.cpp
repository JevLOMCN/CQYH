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

void register_external_game_to_client_achievements_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_complete_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COMPLETE_ACHIEVEMENT>(L, external_message_namespace, "G2C_COMPLETE_ACHIEVEMENT", "ExternalMessage::G2C_COMPLETE_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_complete_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COMPLETE_ACHIEVEMENT(); });
    external_message_g2c_complete_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COMPLETE_ACHIEVEMENT>(new ExternalMessage::G2C_COMPLETE_ACHIEVEMENT()); });
    external_message_g2c_complete_achievement_type["nAchievementID"] = sol::property([](const ExternalMessage::G2C_COMPLETE_ACHIEVEMENT& self) { return self.nAchievementID; }, [](ExternalMessage::G2C_COMPLETE_ACHIEVEMENT& self, decltype(ExternalMessage::G2C_COMPLETE_ACHIEVEMENT::nAchievementID) value) { self.nAchievementID = value; });
    external_message_g2c_complete_achievement_type["dwCompleteTime"] = sol::property([](const ExternalMessage::G2C_COMPLETE_ACHIEVEMENT& self) { return self.dwCompleteTime; }, [](ExternalMessage::G2C_COMPLETE_ACHIEVEMENT& self, decltype(ExternalMessage::G2C_COMPLETE_ACHIEVEMENT::dwCompleteTime) value) { self.dwCompleteTime = value; });
    auto external_message_g2c_compare_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COMPARE_ACHIEVEMENT>(L, external_message_namespace, "G2C_COMPARE_ACHIEVEMENT", "ExternalMessage::G2C_COMPARE_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_compare_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COMPARE_ACHIEVEMENT(); });
    external_message_g2c_compare_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COMPARE_ACHIEVEMENT>(new ExternalMessage::G2C_COMPARE_ACHIEVEMENT()); });
    external_message_g2c_compare_achievement_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_COMPARE_ACHIEVEMENT& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_COMPARE_ACHIEVEMENT& self, decltype(ExternalMessage::G2C_COMPARE_ACHIEVEMENT::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_compare_achievement_type["nCount"] = sol::property([](const ExternalMessage::G2C_COMPARE_ACHIEVEMENT& self) { return self.nCount; }, [](ExternalMessage::G2C_COMPARE_ACHIEVEMENT& self, decltype(ExternalMessage::G2C_COMPARE_ACHIEVEMENT::nCount) value) { self.nCount = value; });
    external_message_g2c_compare_achievement_type["acAchievement"] = cqyh::lua::string_property(&ExternalMessage::G2C_COMPARE_ACHIEVEMENT::acAchievement);
    auto external_message_g2c_sync_achievement_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST>(L, external_message_namespace, "G2C_SYNC_ACHIEVEMENT_LIST", "ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_achievement_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST(); });
    external_message_g2c_sync_achievement_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST>(new ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST()); });
    external_message_g2c_sync_achievement_list_type["nCount"] = sol::property([](const ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST& self) { return self.nCount; }, [](ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST& self, decltype(ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST::nCount) value) { self.nCount = value; });
    external_message_g2c_sync_achievement_list_type["szData"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_ACHIEVEMENT_LIST::szData);
    auto external_message_g2c_reward_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_REWARD_ACHIEVEMENT>(L, external_message_namespace, "G2C_REWARD_ACHIEVEMENT", "ExternalMessage::G2C_REWARD_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_reward_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_REWARD_ACHIEVEMENT(); });
    external_message_g2c_reward_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_REWARD_ACHIEVEMENT>(new ExternalMessage::G2C_REWARD_ACHIEVEMENT()); });
    external_message_g2c_reward_achievement_type["nAchievementID"] = sol::property([](const ExternalMessage::G2C_REWARD_ACHIEVEMENT& self) { return self.nAchievementID; }, [](ExternalMessage::G2C_REWARD_ACHIEVEMENT& self, decltype(ExternalMessage::G2C_REWARD_ACHIEVEMENT::nAchievementID) value) { self.nAchievementID = value; });
}

}
