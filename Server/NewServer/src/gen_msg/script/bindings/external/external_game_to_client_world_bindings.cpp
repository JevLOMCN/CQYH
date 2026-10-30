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

void register_external_game_to_client_world_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_enter_scene_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ENTER_SCENE>(L, external_message_namespace, "G2C_ENTER_SCENE", "ExternalMessage::G2C_ENTER_SCENE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_enter_scene_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ENTER_SCENE(); });
    external_message_g2c_enter_scene_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ENTER_SCENE>(new ExternalMessage::G2C_ENTER_SCENE()); });
    external_message_g2c_enter_scene_type["nSceneTempID"] = sol::property([](const ExternalMessage::G2C_ENTER_SCENE& self) { return self.nSceneTempID; }, [](ExternalMessage::G2C_ENTER_SCENE& self, decltype(ExternalMessage::G2C_ENTER_SCENE::nSceneTempID) value) { self.nSceneTempID = value; });
    external_message_g2c_enter_scene_type["nSceneID"] = sol::property([](const ExternalMessage::G2C_ENTER_SCENE& self) { return self.nSceneID; }, [](ExternalMessage::G2C_ENTER_SCENE& self, decltype(ExternalMessage::G2C_ENTER_SCENE::nSceneID) value) { self.nSceneID = value; });
    external_message_g2c_enter_scene_type["byWeather"] = sol::property([](const ExternalMessage::G2C_ENTER_SCENE& self) { return self.byWeather; }, [](ExternalMessage::G2C_ENTER_SCENE& self, decltype(ExternalMessage::G2C_ENTER_SCENE::byWeather) value) { self.byWeather = value; });
    external_message_g2c_enter_scene_type["wX"] = sol::property([](const ExternalMessage::G2C_ENTER_SCENE& self) { return self.wX; }, [](ExternalMessage::G2C_ENTER_SCENE& self, decltype(ExternalMessage::G2C_ENTER_SCENE::wX) value) { self.wX = value; });
    external_message_g2c_enter_scene_type["wY"] = sol::property([](const ExternalMessage::G2C_ENTER_SCENE& self) { return self.wY; }, [](ExternalMessage::G2C_ENTER_SCENE& self, decltype(ExternalMessage::G2C_ENTER_SCENE::wY) value) { self.wY = value; });
    external_message_g2c_enter_scene_type["wZ"] = sol::property([](const ExternalMessage::G2C_ENTER_SCENE& self) { return self.wZ; }, [](ExternalMessage::G2C_ENTER_SCENE& self, decltype(ExternalMessage::G2C_ENTER_SCENE::wZ) value) { self.wZ = value; });
    auto external_message_g2c_leave_scene_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_LEAVE_SCENE>(L, external_message_namespace, "G2C_LEAVE_SCENE", "ExternalMessage::G2C_LEAVE_SCENE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_leave_scene_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_LEAVE_SCENE(); });
    external_message_g2c_leave_scene_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_LEAVE_SCENE>(new ExternalMessage::G2C_LEAVE_SCENE()); });
    auto external_message_g2c_changescene_enter_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHANGESCENE_ENTER>(L, external_message_namespace, "G2C_CHANGESCENE_ENTER", "ExternalMessage::G2C_CHANGESCENE_ENTER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_changescene_enter_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHANGESCENE_ENTER(); });
    external_message_g2c_changescene_enter_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHANGESCENE_ENTER>(new ExternalMessage::G2C_CHANGESCENE_ENTER()); });
    external_message_g2c_changescene_enter_type["dwGameServerID"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.dwGameServerID; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::dwGameServerID) value) { self.dwGameServerID = value; });
    external_message_g2c_changescene_enter_type["nSceneTempID"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.nSceneTempID; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::nSceneTempID) value) { self.nSceneTempID = value; });
    external_message_g2c_changescene_enter_type["nSceneID"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.nSceneID; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::nSceneID) value) { self.nSceneID = value; });
    external_message_g2c_changescene_enter_type["wDestX"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.wDestX; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::wDestX) value) { self.wDestX = value; });
    external_message_g2c_changescene_enter_type["wDestY"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.wDestY; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::wDestY) value) { self.wDestY = value; });
    external_message_g2c_changescene_enter_type["wDestZ"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.wDestZ; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::wDestZ) value) { self.wDestZ = value; });
    external_message_g2c_changescene_enter_type["wFaceDirection"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.wFaceDirection; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_g2c_changescene_enter_type["byIsInJump"] = sol::property([](const ExternalMessage::G2C_CHANGESCENE_ENTER& self) { return self.byIsInJump; }, [](ExternalMessage::G2C_CHANGESCENE_ENTER& self, decltype(ExternalMessage::G2C_CHANGESCENE_ENTER::byIsInJump) value) { self.byIsInJump = value; });
    auto external_message_g2c_char_teleport_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_TELEPORT>(L, external_message_namespace, "G2C_CHAR_TELEPORT", "ExternalMessage::G2C_CHAR_TELEPORT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_teleport_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_TELEPORT(); });
    external_message_g2c_char_teleport_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_TELEPORT>(new ExternalMessage::G2C_CHAR_TELEPORT()); });
    external_message_g2c_char_teleport_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_TELEPORT& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_TELEPORT& self, decltype(ExternalMessage::G2C_CHAR_TELEPORT::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_teleport_type["nSceneID"] = sol::property([](const ExternalMessage::G2C_CHAR_TELEPORT& self) { return self.nSceneID; }, [](ExternalMessage::G2C_CHAR_TELEPORT& self, decltype(ExternalMessage::G2C_CHAR_TELEPORT::nSceneID) value) { self.nSceneID = value; });
    external_message_g2c_char_teleport_type["wDestX"] = sol::property([](const ExternalMessage::G2C_CHAR_TELEPORT& self) { return self.wDestX; }, [](ExternalMessage::G2C_CHAR_TELEPORT& self, decltype(ExternalMessage::G2C_CHAR_TELEPORT::wDestX) value) { self.wDestX = value; });
    external_message_g2c_char_teleport_type["wDestY"] = sol::property([](const ExternalMessage::G2C_CHAR_TELEPORT& self) { return self.wDestY; }, [](ExternalMessage::G2C_CHAR_TELEPORT& self, decltype(ExternalMessage::G2C_CHAR_TELEPORT::wDestY) value) { self.wDestY = value; });
    external_message_g2c_char_teleport_type["wDestZ"] = sol::property([](const ExternalMessage::G2C_CHAR_TELEPORT& self) { return self.wDestZ; }, [](ExternalMessage::G2C_CHAR_TELEPORT& self, decltype(ExternalMessage::G2C_CHAR_TELEPORT::wDestZ) value) { self.wDestZ = value; });
    auto external_message_g2c_query_scene_difficulty_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY>(L, external_message_namespace, "G2C_QUERY_SCENE_DIFFICULTY", "ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_query_scene_difficulty_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY(); });
    external_message_g2c_query_scene_difficulty_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY>(new ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY()); });
    external_message_g2c_query_scene_difficulty_type["dwSceneTemplateID"] = sol::property([](const ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY& self) { return self.dwSceneTemplateID; }, [](ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY& self, decltype(ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY::dwSceneTemplateID) value) { self.dwSceneTemplateID = value; });
    external_message_g2c_query_scene_difficulty_type["byDungeonDifficultyMask"] = sol::property([](const ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY& self) { return self.byDungeonDifficultyMask; }, [](ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY& self, decltype(ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY::byDungeonDifficultyMask) value) { self.byDungeonDifficultyMask = value; });
    external_message_g2c_query_scene_difficulty_type["byDungeonEnterCount"] = sol::property([](const ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY& self) { return self.byDungeonEnterCount; }, [](ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY& self, decltype(ExternalMessage::G2C_QUERY_SCENE_DIFFICULTY::byDungeonEnterCount) value) { self.byDungeonEnterCount = value; });
    auto external_message_g2c_select_scene_difficulty_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY>(L, external_message_namespace, "G2C_SELECT_SCENE_DIFFICULTY", "ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_select_scene_difficulty_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY(); });
    external_message_g2c_select_scene_difficulty_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY>(new ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY()); });
    external_message_g2c_select_scene_difficulty_type["bySelectedDifficulty"] = sol::property([](const ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY& self) { return self.bySelectedDifficulty; }, [](ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY& self, decltype(ExternalMessage::G2C_SELECT_SCENE_DIFFICULTY::bySelectedDifficulty) value) { self.bySelectedDifficulty = value; });
    auto external_message_g2c_scene_finish_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SCENE_FINISH>(L, external_message_namespace, "G2C_SCENE_FINISH", "ExternalMessage::G2C_SCENE_FINISH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_scene_finish_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SCENE_FINISH(); });
    external_message_g2c_scene_finish_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SCENE_FINISH>(new ExternalMessage::G2C_SCENE_FINISH()); });
    auto external_message_scene_score_type = cqyh::gen_msg::usertype<ExternalMessage::SCENE_SCORE>(L, external_message_namespace, "SCENE_SCORE", "ExternalMessage::SCENE_SCORE");
    external_message_scene_score_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::SCENE_SCORE(); });
    external_message_scene_score_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::SCENE_SCORE>(new ExternalMessage::SCENE_SCORE()); });
    external_message_scene_score_type["dwRoleID"] = sol::property([](const ExternalMessage::SCENE_SCORE& self) { return self.dwRoleID; }, [](ExternalMessage::SCENE_SCORE& self, decltype(ExternalMessage::SCENE_SCORE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_scene_score_type["dwTotalScore"] = sol::property([](const ExternalMessage::SCENE_SCORE& self) { return self.dwTotalScore; }, [](ExternalMessage::SCENE_SCORE& self, decltype(ExternalMessage::SCENE_SCORE::dwTotalScore) value) { self.dwTotalScore = value; });
    auto external_message_g2c_sync_teleport_platform_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_TELEPORT_PLATFORM>(L, external_message_namespace, "G2C_SYNC_TELEPORT_PLATFORM", "ExternalMessage::G2C_SYNC_TELEPORT_PLATFORM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_teleport_platform_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_TELEPORT_PLATFORM(); });
    external_message_g2c_sync_teleport_platform_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_TELEPORT_PLATFORM>(new ExternalMessage::G2C_SYNC_TELEPORT_PLATFORM()); });
    external_message_g2c_sync_teleport_platform_type["nTeleportPlatform"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_TELEPORT_PLATFORM::nTeleportPlatform);
    auto external_message_g2c_draw_debug_line_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DRAW_DEBUG_LINE>(L, external_message_namespace, "G2C_DRAW_DEBUG_LINE", "ExternalMessage::G2C_DRAW_DEBUG_LINE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_draw_debug_line_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DRAW_DEBUG_LINE(); });
    external_message_g2c_draw_debug_line_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DRAW_DEBUG_LINE>(new ExternalMessage::G2C_DRAW_DEBUG_LINE()); });
    external_message_g2c_draw_debug_line_type["byType"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.byType; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::byType) value) { self.byType = value; });
    external_message_g2c_draw_debug_line_type["wSrcX"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.wSrcX; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::wSrcX) value) { self.wSrcX = value; });
    external_message_g2c_draw_debug_line_type["wSrcY"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.wSrcY; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::wSrcY) value) { self.wSrcY = value; });
    external_message_g2c_draw_debug_line_type["wSrcZ"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.wSrcZ; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::wSrcZ) value) { self.wSrcZ = value; });
    external_message_g2c_draw_debug_line_type["wDstX"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.wDstX; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::wDstX) value) { self.wDstX = value; });
    external_message_g2c_draw_debug_line_type["wDstY"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.wDstY; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::wDstY) value) { self.wDstY = value; });
    external_message_g2c_draw_debug_line_type["wDstZ"] = sol::property([](const ExternalMessage::G2C_DRAW_DEBUG_LINE& self) { return self.wDstZ; }, [](ExternalMessage::G2C_DRAW_DEBUG_LINE& self, decltype(ExternalMessage::G2C_DRAW_DEBUG_LINE::wDstZ) value) { self.wDstZ = value; });
}

}
