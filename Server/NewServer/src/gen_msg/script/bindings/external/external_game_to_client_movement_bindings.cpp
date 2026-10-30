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

void register_external_game_to_client_movement_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_next_jump_point_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT>(L, external_message_namespace, "G2C_SYNC_NEXT_JUMP_POINT", "ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_next_jump_point_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT(); });
    external_message_g2c_sync_next_jump_point_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT>(new ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT()); });
    external_message_g2c_sync_next_jump_point_type["wJumpX"] = sol::property([](const ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT& self) { return self.wJumpX; }, [](ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT& self, decltype(ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT::wJumpX) value) { self.wJumpX = value; });
    external_message_g2c_sync_next_jump_point_type["wJumpY"] = sol::property([](const ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT& self) { return self.wJumpY; }, [](ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT& self, decltype(ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT::wJumpY) value) { self.wJumpY = value; });
    external_message_g2c_sync_next_jump_point_type["wJumpZ"] = sol::property([](const ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT& self) { return self.wJumpZ; }, [](ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT& self, decltype(ExternalMessage::G2C_SYNC_NEXT_JUMP_POINT::wJumpZ) value) { self.wJumpZ = value; });
    auto external_message_g2c_sync_role_pos_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_POS>(L, external_message_namespace, "G2C_SYNC_ROLE_POS", "ExternalMessage::G2C_SYNC_ROLE_POS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_role_pos_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_POS(); });
    external_message_g2c_sync_role_pos_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_POS>(new ExternalMessage::G2C_SYNC_ROLE_POS()); });
    external_message_g2c_sync_role_pos_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_POS& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_ROLE_POS& self, decltype(ExternalMessage::G2C_SYNC_ROLE_POS::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_role_pos_type["wCurrentX"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_POS& self) { return self.wCurrentX; }, [](ExternalMessage::G2C_SYNC_ROLE_POS& self, decltype(ExternalMessage::G2C_SYNC_ROLE_POS::wCurrentX) value) { self.wCurrentX = value; });
    external_message_g2c_sync_role_pos_type["wCurrentY"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_POS& self) { return self.wCurrentY; }, [](ExternalMessage::G2C_SYNC_ROLE_POS& self, decltype(ExternalMessage::G2C_SYNC_ROLE_POS::wCurrentY) value) { self.wCurrentY = value; });
    external_message_g2c_sync_role_pos_type["wCurrentZ"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_POS& self) { return self.wCurrentZ; }, [](ExternalMessage::G2C_SYNC_ROLE_POS& self, decltype(ExternalMessage::G2C_SYNC_ROLE_POS::wCurrentZ) value) { self.wCurrentZ = value; });
    external_message_g2c_sync_role_pos_type["wFaceDirection"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_POS& self) { return self.wFaceDirection; }, [](ExternalMessage::G2C_SYNC_ROLE_POS& self, decltype(ExternalMessage::G2C_SYNC_ROLE_POS::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_g2c_sync_role_pos_type["byMoveState"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_POS& self) { return self.byMoveState; }, [](ExternalMessage::G2C_SYNC_ROLE_POS& self, decltype(ExternalMessage::G2C_SYNC_ROLE_POS::byMoveState) value) { self.byMoveState = value; });
    auto external_message_g2c_char_walk_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_WALK>(L, external_message_namespace, "G2C_CHAR_WALK", "ExternalMessage::G2C_CHAR_WALK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_walk_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_WALK(); });
    external_message_g2c_char_walk_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_WALK>(new ExternalMessage::G2C_CHAR_WALK()); });
    external_message_g2c_char_walk_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_WALK& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_WALK& self, decltype(ExternalMessage::G2C_CHAR_WALK::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_walk_type["wSpeed"] = sol::property([](const ExternalMessage::G2C_CHAR_WALK& self) { return self.wSpeed; }, [](ExternalMessage::G2C_CHAR_WALK& self, decltype(ExternalMessage::G2C_CHAR_WALK::wSpeed) value) { self.wSpeed = value; });
    external_message_g2c_char_walk_type["wDestX"] = sol::property([](const ExternalMessage::G2C_CHAR_WALK& self) { return self.wDestX; }, [](ExternalMessage::G2C_CHAR_WALK& self, decltype(ExternalMessage::G2C_CHAR_WALK::wDestX) value) { self.wDestX = value; });
    external_message_g2c_char_walk_type["wDestY"] = sol::property([](const ExternalMessage::G2C_CHAR_WALK& self) { return self.wDestY; }, [](ExternalMessage::G2C_CHAR_WALK& self, decltype(ExternalMessage::G2C_CHAR_WALK::wDestY) value) { self.wDestY = value; });
    auto external_message_g2c_char_run_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_RUN>(L, external_message_namespace, "G2C_CHAR_RUN", "ExternalMessage::G2C_CHAR_RUN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_run_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_RUN(); });
    external_message_g2c_char_run_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_RUN>(new ExternalMessage::G2C_CHAR_RUN()); });
    external_message_g2c_char_run_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_RUN& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_RUN& self, decltype(ExternalMessage::G2C_CHAR_RUN::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_run_type["wSpeed"] = sol::property([](const ExternalMessage::G2C_CHAR_RUN& self) { return self.wSpeed; }, [](ExternalMessage::G2C_CHAR_RUN& self, decltype(ExternalMessage::G2C_CHAR_RUN::wSpeed) value) { self.wSpeed = value; });
    external_message_g2c_char_run_type["wDestX"] = sol::property([](const ExternalMessage::G2C_CHAR_RUN& self) { return self.wDestX; }, [](ExternalMessage::G2C_CHAR_RUN& self, decltype(ExternalMessage::G2C_CHAR_RUN::wDestX) value) { self.wDestX = value; });
    external_message_g2c_char_run_type["wDestY"] = sol::property([](const ExternalMessage::G2C_CHAR_RUN& self) { return self.wDestY; }, [](ExternalMessage::G2C_CHAR_RUN& self, decltype(ExternalMessage::G2C_CHAR_RUN::wDestY) value) { self.wDestY = value; });
    auto external_message_g2c_char_stop_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_STOP>(L, external_message_namespace, "G2C_CHAR_STOP", "ExternalMessage::G2C_CHAR_STOP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_stop_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_STOP(); });
    external_message_g2c_char_stop_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_STOP>(new ExternalMessage::G2C_CHAR_STOP()); });
    external_message_g2c_char_stop_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_STOP& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_STOP& self, decltype(ExternalMessage::G2C_CHAR_STOP::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_stop_type["byForce"] = sol::property([](const ExternalMessage::G2C_CHAR_STOP& self) { return self.byForce; }, [](ExternalMessage::G2C_CHAR_STOP& self, decltype(ExternalMessage::G2C_CHAR_STOP::byForce) value) { self.byForce = value; });
    external_message_g2c_char_stop_type["wCurrentX"] = sol::property([](const ExternalMessage::G2C_CHAR_STOP& self) { return self.wCurrentX; }, [](ExternalMessage::G2C_CHAR_STOP& self, decltype(ExternalMessage::G2C_CHAR_STOP::wCurrentX) value) { self.wCurrentX = value; });
    external_message_g2c_char_stop_type["wCurrentY"] = sol::property([](const ExternalMessage::G2C_CHAR_STOP& self) { return self.wCurrentY; }, [](ExternalMessage::G2C_CHAR_STOP& self, decltype(ExternalMessage::G2C_CHAR_STOP::wCurrentY) value) { self.wCurrentY = value; });
    external_message_g2c_char_stop_type["wCurrentZ"] = sol::property([](const ExternalMessage::G2C_CHAR_STOP& self) { return self.wCurrentZ; }, [](ExternalMessage::G2C_CHAR_STOP& self, decltype(ExternalMessage::G2C_CHAR_STOP::wCurrentZ) value) { self.wCurrentZ = value; });
    auto external_message_g2c_char_passive_move_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_PASSIVE_MOVE>(L, external_message_namespace, "G2C_CHAR_PASSIVE_MOVE", "ExternalMessage::G2C_CHAR_PASSIVE_MOVE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_passive_move_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_PASSIVE_MOVE(); });
    external_message_g2c_char_passive_move_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_PASSIVE_MOVE>(new ExternalMessage::G2C_CHAR_PASSIVE_MOVE()); });
    external_message_g2c_char_passive_move_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_passive_move_type["byMoveState"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.byMoveState; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::byMoveState) value) { self.byMoveState = value; });
    external_message_g2c_char_passive_move_type["wDestX"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.wDestX; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::wDestX) value) { self.wDestX = value; });
    external_message_g2c_char_passive_move_type["wDestY"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.wDestY; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::wDestY) value) { self.wDestY = value; });
    external_message_g2c_char_passive_move_type["wDestZ"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.wDestZ; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::wDestZ) value) { self.wDestZ = value; });
    external_message_g2c_char_passive_move_type["wFaceDirection"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.wFaceDirection; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_g2c_char_passive_move_type["wDuration"] = sol::property([](const ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self) { return self.wDuration; }, [](ExternalMessage::G2C_CHAR_PASSIVE_MOVE& self, decltype(ExternalMessage::G2C_CHAR_PASSIVE_MOVE::wDuration) value) { self.wDuration = value; });
    auto external_message_g2c_char_freeze_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_FREEZE>(L, external_message_namespace, "G2C_CHAR_FREEZE", "ExternalMessage::G2C_CHAR_FREEZE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_freeze_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_FREEZE(); });
    external_message_g2c_char_freeze_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_FREEZE>(new ExternalMessage::G2C_CHAR_FREEZE()); });
    external_message_g2c_char_freeze_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_FREEZE& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_FREEZE& self, decltype(ExternalMessage::G2C_CHAR_FREEZE::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_freeze_type["byFreezed"] = sol::property([](const ExternalMessage::G2C_CHAR_FREEZE& self) { return self.byFreezed; }, [](ExternalMessage::G2C_CHAR_FREEZE& self, decltype(ExternalMessage::G2C_CHAR_FREEZE::byFreezed) value) { self.byFreezed = value; });
    external_message_g2c_char_freeze_type["wCurrentX"] = sol::property([](const ExternalMessage::G2C_CHAR_FREEZE& self) { return self.wCurrentX; }, [](ExternalMessage::G2C_CHAR_FREEZE& self, decltype(ExternalMessage::G2C_CHAR_FREEZE::wCurrentX) value) { self.wCurrentX = value; });
    external_message_g2c_char_freeze_type["wCurrentY"] = sol::property([](const ExternalMessage::G2C_CHAR_FREEZE& self) { return self.wCurrentY; }, [](ExternalMessage::G2C_CHAR_FREEZE& self, decltype(ExternalMessage::G2C_CHAR_FREEZE::wCurrentY) value) { self.wCurrentY = value; });
    external_message_g2c_char_freeze_type["wCurrentZ"] = sol::property([](const ExternalMessage::G2C_CHAR_FREEZE& self) { return self.wCurrentZ; }, [](ExternalMessage::G2C_CHAR_FREEZE& self, decltype(ExternalMessage::G2C_CHAR_FREEZE::wCurrentZ) value) { self.wCurrentZ = value; });
    auto external_message_g2c_object_rotate_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_OBJECT_ROTATE>(L, external_message_namespace, "G2C_OBJECT_ROTATE", "ExternalMessage::G2C_OBJECT_ROTATE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_object_rotate_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_OBJECT_ROTATE(); });
    external_message_g2c_object_rotate_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_OBJECT_ROTATE>(new ExternalMessage::G2C_OBJECT_ROTATE()); });
    external_message_g2c_object_rotate_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_OBJECT_ROTATE& self) { return self.dwCharID; }, [](ExternalMessage::G2C_OBJECT_ROTATE& self, decltype(ExternalMessage::G2C_OBJECT_ROTATE::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_object_rotate_type["wDuration"] = sol::property([](const ExternalMessage::G2C_OBJECT_ROTATE& self) { return self.wDuration; }, [](ExternalMessage::G2C_OBJECT_ROTATE& self, decltype(ExternalMessage::G2C_OBJECT_ROTATE::wDuration) value) { self.wDuration = value; });
    external_message_g2c_object_rotate_type["wFaceDirection"] = sol::property([](const ExternalMessage::G2C_OBJECT_ROTATE& self) { return self.wFaceDirection; }, [](ExternalMessage::G2C_OBJECT_ROTATE& self, decltype(ExternalMessage::G2C_OBJECT_ROTATE::wFaceDirection) value) { self.wFaceDirection = value; });
}

}
