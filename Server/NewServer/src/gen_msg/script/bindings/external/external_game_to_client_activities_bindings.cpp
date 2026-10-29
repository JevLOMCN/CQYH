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

void register_external_game_to_client_activities_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_active_platform_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACTIVE_PLATFORM>(L, external_message_namespace, "G2C_ACTIVE_PLATFORM", "ExternalMessage::G2C_ACTIVE_PLATFORM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_active_platform_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACTIVE_PLATFORM(); });
    external_message_g2c_active_platform_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACTIVE_PLATFORM>(new ExternalMessage::G2C_ACTIVE_PLATFORM()); });
    external_message_g2c_active_platform_type["byActivePlatformID"] = sol::property([](const ExternalMessage::G2C_ACTIVE_PLATFORM& self) { return self.byActivePlatformID; }, [](ExternalMessage::G2C_ACTIVE_PLATFORM& self, decltype(ExternalMessage::G2C_ACTIVE_PLATFORM::byActivePlatformID) value) { self.byActivePlatformID = value; });
    auto external_message_g2c_activity_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACTIVITY_CHANGE>(L, external_message_namespace, "G2C_ACTIVITY_CHANGE", "ExternalMessage::G2C_ACTIVITY_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_activity_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACTIVITY_CHANGE(); });
    external_message_g2c_activity_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACTIVITY_CHANGE>(new ExternalMessage::G2C_ACTIVITY_CHANGE()); });
    external_message_g2c_activity_change_type["wActivity"] = sol::property([](const ExternalMessage::G2C_ACTIVITY_CHANGE& self) { return self.wActivity; }, [](ExternalMessage::G2C_ACTIVITY_CHANGE& self, decltype(ExternalMessage::G2C_ACTIVITY_CHANGE::wActivity) value) { self.wActivity = value; });
    auto external_message_g2c_activity_reward_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACTIVITY_REWARD>(L, external_message_namespace, "G2C_ACTIVITY_REWARD", "ExternalMessage::G2C_ACTIVITY_REWARD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_activity_reward_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACTIVITY_REWARD(); });
    external_message_g2c_activity_reward_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACTIVITY_REWARD>(new ExternalMessage::G2C_ACTIVITY_REWARD()); });
    external_message_g2c_activity_reward_type["wIndex"] = sol::property([](const ExternalMessage::G2C_ACTIVITY_REWARD& self) { return self.wIndex; }, [](ExternalMessage::G2C_ACTIVITY_REWARD& self, decltype(ExternalMessage::G2C_ACTIVITY_REWARD::wIndex) value) { self.wIndex = value; });
    auto external_message_g2c_sync_checkin_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_CHECKIN_INFO>(L, external_message_namespace, "G2C_SYNC_CHECKIN_INFO", "ExternalMessage::G2C_SYNC_CHECKIN_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_checkin_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_CHECKIN_INFO(); });
    external_message_g2c_sync_checkin_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_CHECKIN_INFO>(new ExternalMessage::G2C_SYNC_CHECKIN_INFO()); });
    external_message_g2c_sync_checkin_info_type["byLastCheckInIndex"] = sol::property([](const ExternalMessage::G2C_SYNC_CHECKIN_INFO& self) { return self.byLastCheckInIndex; }, [](ExternalMessage::G2C_SYNC_CHECKIN_INFO& self, decltype(ExternalMessage::G2C_SYNC_CHECKIN_INFO::byLastCheckInIndex) value) { self.byLastCheckInIndex = value; });
    external_message_g2c_sync_checkin_info_type["byCanCheckInToday"] = sol::property([](const ExternalMessage::G2C_SYNC_CHECKIN_INFO& self) { return self.byCanCheckInToday; }, [](ExternalMessage::G2C_SYNC_CHECKIN_INFO& self, decltype(ExternalMessage::G2C_SYNC_CHECKIN_INFO::byCanCheckInToday) value) { self.byCanCheckInToday = value; });
    external_message_g2c_sync_checkin_info_type["byCheckInValid"] = sol::property([](const ExternalMessage::G2C_SYNC_CHECKIN_INFO& self) { return self.byCheckInValid; }, [](ExternalMessage::G2C_SYNC_CHECKIN_INFO& self, decltype(ExternalMessage::G2C_SYNC_CHECKIN_INFO::byCheckInValid) value) { self.byCheckInValid = value; });
    auto external_message_g2c_checkin_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHECKIN>(L, external_message_namespace, "G2C_CHECKIN", "ExternalMessage::G2C_CHECKIN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_checkin_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHECKIN(); });
    external_message_g2c_checkin_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHECKIN>(new ExternalMessage::G2C_CHECKIN()); });
    external_message_g2c_checkin_type["byIndex"] = sol::property([](const ExternalMessage::G2C_CHECKIN& self) { return self.byIndex; }, [](ExternalMessage::G2C_CHECKIN& self, decltype(ExternalMessage::G2C_CHECKIN::byIndex) value) { self.byIndex = value; });
}

}
