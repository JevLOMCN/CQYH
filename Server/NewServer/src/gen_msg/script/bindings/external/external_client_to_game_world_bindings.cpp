#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_world_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_select_scene_difficulty_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY>(L, external_message_namespace, "C2G_SELECT_SCENE_DIFFICULTY", "ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_select_scene_difficulty_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY(); });
    external_message_c2g_select_scene_difficulty_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY>(new ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY()); });
    external_message_c2g_select_scene_difficulty_type["bySelectedDifficulty"] = sol::property([](const ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY& self) { return self.bySelectedDifficulty; }, [](ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY& self, decltype(ExternalMessage::C2G_SELECT_SCENE_DIFFICULTY::bySelectedDifficulty) value) { self.bySelectedDifficulty = value; });
    auto external_message_c2g_change_line_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHANGE_LINE>(L, external_message_namespace, "C2G_CHANGE_LINE", "ExternalMessage::C2G_CHANGE_LINE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_change_line_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHANGE_LINE(); });
    external_message_c2g_change_line_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHANGE_LINE>(new ExternalMessage::C2G_CHANGE_LINE()); });
    external_message_c2g_change_line_type["byLineIndex"] = sol::property([](const ExternalMessage::C2G_CHANGE_LINE& self) { return self.byLineIndex; }, [](ExternalMessage::C2G_CHANGE_LINE& self, decltype(ExternalMessage::C2G_CHANGE_LINE::byLineIndex) value) { self.byLineIndex = value; });
    auto external_message_c2g_teleport_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TELEPORT>(L, external_message_namespace, "C2G_TELEPORT", "ExternalMessage::C2G_TELEPORT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_teleport_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TELEPORT(); });
    external_message_c2g_teleport_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TELEPORT>(new ExternalMessage::C2G_TELEPORT()); });
    external_message_c2g_teleport_type["dwTeleportID"] = sol::property([](const ExternalMessage::C2G_TELEPORT& self) { return self.dwTeleportID; }, [](ExternalMessage::C2G_TELEPORT& self, decltype(ExternalMessage::C2G_TELEPORT::dwTeleportID) value) { self.dwTeleportID = value; });
    auto external_message_c2g_explore_zone_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_EXPLORE_ZONE>(L, external_message_namespace, "C2G_EXPLORE_ZONE", "ExternalMessage::C2G_EXPLORE_ZONE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_explore_zone_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_EXPLORE_ZONE(); });
    external_message_c2g_explore_zone_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_EXPLORE_ZONE>(new ExternalMessage::C2G_EXPLORE_ZONE()); });
    auto external_message_c2g_teleport_by_platform_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TELEPORT_BY_PLATFORM>(L, external_message_namespace, "C2G_TELEPORT_BY_PLATFORM", "ExternalMessage::C2G_TELEPORT_BY_PLATFORM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_teleport_by_platform_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TELEPORT_BY_PLATFORM(); });
    external_message_c2g_teleport_by_platform_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TELEPORT_BY_PLATFORM>(new ExternalMessage::C2G_TELEPORT_BY_PLATFORM()); });
    external_message_c2g_teleport_by_platform_type["bySrcPlatform"] = sol::property([](const ExternalMessage::C2G_TELEPORT_BY_PLATFORM& self) { return self.bySrcPlatform; }, [](ExternalMessage::C2G_TELEPORT_BY_PLATFORM& self, decltype(ExternalMessage::C2G_TELEPORT_BY_PLATFORM::bySrcPlatform) value) { self.bySrcPlatform = value; });
    external_message_c2g_teleport_by_platform_type["byTgtPlatform"] = sol::property([](const ExternalMessage::C2G_TELEPORT_BY_PLATFORM& self) { return self.byTgtPlatform; }, [](ExternalMessage::C2G_TELEPORT_BY_PLATFORM& self, decltype(ExternalMessage::C2G_TELEPORT_BY_PLATFORM::byTgtPlatform) value) { self.byTgtPlatform = value; });
    auto external_message_c2g_leave_scene_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LEAVE_SCENE>(L, external_message_namespace, "C2G_LEAVE_SCENE", "ExternalMessage::C2G_LEAVE_SCENE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_leave_scene_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LEAVE_SCENE(); });
    external_message_c2g_leave_scene_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LEAVE_SCENE>(new ExternalMessage::C2G_LEAVE_SCENE()); });
    auto external_message_c2g_cancel_cutscene_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CANCEL_CUTSCENE>(L, external_message_namespace, "C2G_CANCEL_CUTSCENE", "ExternalMessage::C2G_CANCEL_CUTSCENE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_cancel_cutscene_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CANCEL_CUTSCENE(); });
    external_message_c2g_cancel_cutscene_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CANCEL_CUTSCENE>(new ExternalMessage::C2G_CANCEL_CUTSCENE()); });
    auto external_message_c2g_role_teleport_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_TELEPORT>(L, external_message_namespace, "C2G_ROLE_TELEPORT", "ExternalMessage::C2G_ROLE_TELEPORT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_teleport_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_TELEPORT(); });
    external_message_c2g_role_teleport_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_TELEPORT>(new ExternalMessage::C2G_ROLE_TELEPORT()); });
    external_message_c2g_role_teleport_type["wDestX"] = sol::property([](const ExternalMessage::C2G_ROLE_TELEPORT& self) { return self.wDestX; }, [](ExternalMessage::C2G_ROLE_TELEPORT& self, decltype(ExternalMessage::C2G_ROLE_TELEPORT::wDestX) value) { self.wDestX = value; });
    external_message_c2g_role_teleport_type["wDestY"] = sol::property([](const ExternalMessage::C2G_ROLE_TELEPORT& self) { return self.wDestY; }, [](ExternalMessage::C2G_ROLE_TELEPORT& self, decltype(ExternalMessage::C2G_ROLE_TELEPORT::wDestY) value) { self.wDestY = value; });
}

}
