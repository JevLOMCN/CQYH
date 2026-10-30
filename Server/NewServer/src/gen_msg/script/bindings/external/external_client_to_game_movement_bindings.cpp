#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_movement_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_role_stop_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_STOP>(L, external_message_namespace, "C2G_ROLE_STOP", "ExternalMessage::C2G_ROLE_STOP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_stop_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_STOP(); });
    external_message_c2g_role_stop_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_STOP>(new ExternalMessage::C2G_ROLE_STOP()); });
    auto external_message_c2g_update_pos_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_UPDATE_POS>(L, external_message_namespace, "C2G_UPDATE_POS", "ExternalMessage::C2G_UPDATE_POS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_update_pos_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_UPDATE_POS(); });
    external_message_c2g_update_pos_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_UPDATE_POS>(new ExternalMessage::C2G_UPDATE_POS()); });
    external_message_c2g_update_pos_type["wDelay"] = sol::property([](const ExternalMessage::C2G_UPDATE_POS& self) { return self.wDelay; }, [](ExternalMessage::C2G_UPDATE_POS& self, decltype(ExternalMessage::C2G_UPDATE_POS::wDelay) value) { self.wDelay = value; });
    external_message_c2g_update_pos_type["wCurrentX"] = sol::property([](const ExternalMessage::C2G_UPDATE_POS& self) { return self.wCurrentX; }, [](ExternalMessage::C2G_UPDATE_POS& self, decltype(ExternalMessage::C2G_UPDATE_POS::wCurrentX) value) { self.wCurrentX = value; });
    external_message_c2g_update_pos_type["wCurrentY"] = sol::property([](const ExternalMessage::C2G_UPDATE_POS& self) { return self.wCurrentY; }, [](ExternalMessage::C2G_UPDATE_POS& self, decltype(ExternalMessage::C2G_UPDATE_POS::wCurrentY) value) { self.wCurrentY = value; });
    external_message_c2g_update_pos_type["wCurrentZ"] = sol::property([](const ExternalMessage::C2G_UPDATE_POS& self) { return self.wCurrentZ; }, [](ExternalMessage::C2G_UPDATE_POS& self, decltype(ExternalMessage::C2G_UPDATE_POS::wCurrentZ) value) { self.wCurrentZ = value; });
    auto external_message_c2g_role_rotate_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_ROTATE>(L, external_message_namespace, "C2G_ROLE_ROTATE", "ExternalMessage::C2G_ROLE_ROTATE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_rotate_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_ROTATE(); });
    external_message_c2g_role_rotate_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_ROTATE>(new ExternalMessage::C2G_ROLE_ROTATE()); });
    external_message_c2g_role_rotate_type["wFaceDirection"] = sol::property([](const ExternalMessage::C2G_ROLE_ROTATE& self) { return self.wFaceDirection; }, [](ExternalMessage::C2G_ROLE_ROTATE& self, decltype(ExternalMessage::C2G_ROLE_ROTATE::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_c2g_role_rotate_type["nDuration"] = sol::property([](const ExternalMessage::C2G_ROLE_ROTATE& self) { return self.nDuration; }, [](ExternalMessage::C2G_ROLE_ROTATE& self, decltype(ExternalMessage::C2G_ROLE_ROTATE::nDuration) value) { self.nDuration = value; });
    auto external_message_c2g_role_walk_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_WALK>(L, external_message_namespace, "C2G_ROLE_WALK", "ExternalMessage::C2G_ROLE_WALK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_walk_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_WALK(); });
    external_message_c2g_role_walk_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_WALK>(new ExternalMessage::C2G_ROLE_WALK()); });
    external_message_c2g_role_walk_type["wDestX"] = sol::property([](const ExternalMessage::C2G_ROLE_WALK& self) { return self.wDestX; }, [](ExternalMessage::C2G_ROLE_WALK& self, decltype(ExternalMessage::C2G_ROLE_WALK::wDestX) value) { self.wDestX = value; });
    external_message_c2g_role_walk_type["wDestY"] = sol::property([](const ExternalMessage::C2G_ROLE_WALK& self) { return self.wDestY; }, [](ExternalMessage::C2G_ROLE_WALK& self, decltype(ExternalMessage::C2G_ROLE_WALK::wDestY) value) { self.wDestY = value; });
    auto external_message_c2g_role_run_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_RUN>(L, external_message_namespace, "C2G_ROLE_RUN", "ExternalMessage::C2G_ROLE_RUN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_run_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_RUN(); });
    external_message_c2g_role_run_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_RUN>(new ExternalMessage::C2G_ROLE_RUN()); });
    external_message_c2g_role_run_type["wDestY"] = sol::property([](const ExternalMessage::C2G_ROLE_RUN& self) { return self.wDestY; }, [](ExternalMessage::C2G_ROLE_RUN& self, decltype(ExternalMessage::C2G_ROLE_RUN::wDestY) value) { self.wDestY = value; });
    external_message_c2g_role_run_type["wDestX"] = sol::property([](const ExternalMessage::C2G_ROLE_RUN& self) { return self.wDestX; }, [](ExternalMessage::C2G_ROLE_RUN& self, decltype(ExternalMessage::C2G_ROLE_RUN::wDestX) value) { self.wDestX = value; });
}

}
