#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_crafting_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_weapon_level_up_put_in_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_WEAPON_LEVEL_UP_PUT_IN>(L, external_message_namespace, "C2G_WEAPON_LEVEL_UP_PUT_IN", "ExternalMessage::C2G_WEAPON_LEVEL_UP_PUT_IN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_weapon_level_up_put_in_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_WEAPON_LEVEL_UP_PUT_IN(); });
    external_message_c2g_weapon_level_up_put_in_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_WEAPON_LEVEL_UP_PUT_IN>(new ExternalMessage::C2G_WEAPON_LEVEL_UP_PUT_IN()); });
    auto external_message_c2g_weapon_level_up_take_out_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT>(L, external_message_namespace, "C2G_WEAPON_LEVEL_UP_TAKE_OUT", "ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_weapon_level_up_take_out_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT(); });
    external_message_c2g_weapon_level_up_take_out_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT>(new ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT()); });
    auto external_message_c2g_decompose_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DECOMPOSE>(L, external_message_namespace, "C2G_DECOMPOSE", "ExternalMessage::C2G_DECOMPOSE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_decompose_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DECOMPOSE(); });
    external_message_c2g_decompose_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DECOMPOSE>(new ExternalMessage::C2G_DECOMPOSE()); });
    external_message_c2g_decompose_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_DECOMPOSE& self) { return self.byConSlot; }, [](ExternalMessage::C2G_DECOMPOSE& self, decltype(ExternalMessage::C2G_DECOMPOSE::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_decompose_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_DECOMPOSE& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_DECOMPOSE& self, decltype(ExternalMessage::C2G_DECOMPOSE::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_c2g_compose_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_COMPOSE>(L, external_message_namespace, "C2G_COMPOSE", "ExternalMessage::C2G_COMPOSE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_compose_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_COMPOSE(); });
    external_message_c2g_compose_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_COMPOSE>(new ExternalMessage::C2G_COMPOSE()); });
    external_message_c2g_compose_type["nFormulaID"] = sol::property([](const ExternalMessage::C2G_COMPOSE& self) { return self.nFormulaID; }, [](ExternalMessage::C2G_COMPOSE& self, decltype(ExternalMessage::C2G_COMPOSE::nFormulaID) value) { self.nFormulaID = value; });
    auto external_message_c2g_weapon_level_up_take_out_immediately_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY>(L, external_message_namespace, "C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY", "ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_weapon_level_up_take_out_immediately_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY(); });
    external_message_c2g_weapon_level_up_take_out_immediately_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY>(new ExternalMessage::C2G_WEAPON_LEVEL_UP_TAKE_OUT_IMMEDIATELY()); });
}

}
