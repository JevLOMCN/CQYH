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

void register_external_game_to_client_crafting_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_weapon_level_up_put_in_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_WEAPON_LEVEL_UP_PUT_IN>(L, external_message_namespace, "G2C_WEAPON_LEVEL_UP_PUT_IN", "ExternalMessage::G2C_WEAPON_LEVEL_UP_PUT_IN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_weapon_level_up_put_in_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_WEAPON_LEVEL_UP_PUT_IN(); });
    external_message_g2c_weapon_level_up_put_in_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_WEAPON_LEVEL_UP_PUT_IN>(new ExternalMessage::G2C_WEAPON_LEVEL_UP_PUT_IN()); });
    auto external_message_g2c_weapon_level_up_take_out_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_WEAPON_LEVEL_UP_TAKE_OUT>(L, external_message_namespace, "G2C_WEAPON_LEVEL_UP_TAKE_OUT", "ExternalMessage::G2C_WEAPON_LEVEL_UP_TAKE_OUT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_weapon_level_up_take_out_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_WEAPON_LEVEL_UP_TAKE_OUT(); });
    external_message_g2c_weapon_level_up_take_out_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_WEAPON_LEVEL_UP_TAKE_OUT>(new ExternalMessage::G2C_WEAPON_LEVEL_UP_TAKE_OUT()); });
    auto external_message_g2c_weapon_level_up_result_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT>(L, external_message_namespace, "G2C_WEAPON_LEVEL_UP_RESULT", "ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_weapon_level_up_result_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT(); });
    external_message_g2c_weapon_level_up_result_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT>(new ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT()); });
    external_message_g2c_weapon_level_up_result_type["byResult"] = sol::property([](const ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT& self) { return self.byResult; }, [](ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT& self, decltype(ExternalMessage::G2C_WEAPON_LEVEL_UP_RESULT::byResult) value) { self.byResult = value; });
    auto external_message_g2c_decompose_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DECOMPOSE>(L, external_message_namespace, "G2C_DECOMPOSE", "ExternalMessage::G2C_DECOMPOSE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_decompose_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DECOMPOSE(); });
    external_message_g2c_decompose_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DECOMPOSE>(new ExternalMessage::G2C_DECOMPOSE()); });
    external_message_g2c_decompose_type["byResult"] = sol::property([](const ExternalMessage::G2C_DECOMPOSE& self) { return self.byResult; }, [](ExternalMessage::G2C_DECOMPOSE& self, decltype(ExternalMessage::G2C_DECOMPOSE::byResult) value) { self.byResult = value; });
    external_message_g2c_decompose_type["nOriginalItemID"] = sol::property([](const ExternalMessage::G2C_DECOMPOSE& self) { return self.nOriginalItemID; }, [](ExternalMessage::G2C_DECOMPOSE& self, decltype(ExternalMessage::G2C_DECOMPOSE::nOriginalItemID) value) { self.nOriginalItemID = value; });
    external_message_g2c_decompose_type["nItemID"] = cqyh::lua::array_property(&ExternalMessage::G2C_DECOMPOSE::nItemID);
    external_message_g2c_decompose_type["nItemCount"] = cqyh::lua::array_property(&ExternalMessage::G2C_DECOMPOSE::nItemCount);
    auto external_message_g2c_compose_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COMPOSE>(L, external_message_namespace, "G2C_COMPOSE", "ExternalMessage::G2C_COMPOSE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_compose_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COMPOSE(); });
    external_message_g2c_compose_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COMPOSE>(new ExternalMessage::G2C_COMPOSE()); });
    external_message_g2c_compose_type["byResult"] = sol::property([](const ExternalMessage::G2C_COMPOSE& self) { return self.byResult; }, [](ExternalMessage::G2C_COMPOSE& self, decltype(ExternalMessage::G2C_COMPOSE::byResult) value) { self.byResult = value; });
    external_message_g2c_compose_type["nResultItemID"] = sol::property([](const ExternalMessage::G2C_COMPOSE& self) { return self.nResultItemID; }, [](ExternalMessage::G2C_COMPOSE& self, decltype(ExternalMessage::G2C_COMPOSE::nResultItemID) value) { self.nResultItemID = value; });
}

}
