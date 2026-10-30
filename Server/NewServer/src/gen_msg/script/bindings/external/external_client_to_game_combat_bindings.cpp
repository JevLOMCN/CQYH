#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_combat_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_combat_pose_switch_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_COMBAT_POSE_SWITCH>(L, external_message_namespace, "C2G_COMBAT_POSE_SWITCH", "ExternalMessage::C2G_COMBAT_POSE_SWITCH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_combat_pose_switch_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_COMBAT_POSE_SWITCH(); });
    external_message_c2g_combat_pose_switch_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_COMBAT_POSE_SWITCH>(new ExternalMessage::C2G_COMBAT_POSE_SWITCH()); });
    external_message_c2g_combat_pose_switch_type["byCombatPoseOn"] = sol::property([](const ExternalMessage::C2G_COMBAT_POSE_SWITCH& self) { return self.byCombatPoseOn; }, [](ExternalMessage::C2G_COMBAT_POSE_SWITCH& self, decltype(ExternalMessage::C2G_COMBAT_POSE_SWITCH::byCombatPoseOn) value) { self.byCombatPoseOn = value; });
    external_message_c2g_combat_pose_switch_type["byClientFlag"] = sol::property([](const ExternalMessage::C2G_COMBAT_POSE_SWITCH& self) { return self.byClientFlag; }, [](ExternalMessage::C2G_COMBAT_POSE_SWITCH& self, decltype(ExternalMessage::C2G_COMBAT_POSE_SWITCH::byClientFlag) value) { self.byClientFlag = value; });
    auto external_message_c2g_select_focus_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SELECT_FOCUS>(L, external_message_namespace, "C2G_SELECT_FOCUS", "ExternalMessage::C2G_SELECT_FOCUS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_select_focus_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SELECT_FOCUS(); });
    external_message_c2g_select_focus_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SELECT_FOCUS>(new ExternalMessage::C2G_SELECT_FOCUS()); });
    external_message_c2g_select_focus_type["dwFocusID"] = sol::property([](const ExternalMessage::C2G_SELECT_FOCUS& self) { return self.dwFocusID; }, [](ExternalMessage::C2G_SELECT_FOCUS& self, decltype(ExternalMessage::C2G_SELECT_FOCUS::dwFocusID) value) { self.dwFocusID = value; });
}

}
