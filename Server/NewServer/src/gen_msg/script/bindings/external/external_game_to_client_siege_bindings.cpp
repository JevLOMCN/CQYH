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

void register_external_game_to_client_siege_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sabac_state_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SABAC_STATE_CHANGE>(L, external_message_namespace, "G2C_SABAC_STATE_CHANGE", "ExternalMessage::G2C_SABAC_STATE_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sabac_state_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SABAC_STATE_CHANGE(); });
    external_message_g2c_sabac_state_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SABAC_STATE_CHANGE>(new ExternalMessage::G2C_SABAC_STATE_CHANGE()); });
    external_message_g2c_sabac_state_change_type["byStarted"] = sol::property([](const ExternalMessage::G2C_SABAC_STATE_CHANGE& self) { return self.byStarted; }, [](ExternalMessage::G2C_SABAC_STATE_CHANGE& self, decltype(ExternalMessage::G2C_SABAC_STATE_CHANGE::byStarted) value) { self.byStarted = value; });
    auto external_message_g2c_occupy_sabac_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_OCCUPY_SABAC>(L, external_message_namespace, "G2C_OCCUPY_SABAC", "ExternalMessage::G2C_OCCUPY_SABAC", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_occupy_sabac_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_OCCUPY_SABAC(); });
    external_message_g2c_occupy_sabac_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_OCCUPY_SABAC>(new ExternalMessage::G2C_OCCUPY_SABAC()); });
    external_message_g2c_occupy_sabac_type["dwOldGuildID"] = sol::property([](const ExternalMessage::G2C_OCCUPY_SABAC& self) { return self.dwOldGuildID; }, [](ExternalMessage::G2C_OCCUPY_SABAC& self, decltype(ExternalMessage::G2C_OCCUPY_SABAC::dwOldGuildID) value) { self.dwOldGuildID = value; });
    external_message_g2c_occupy_sabac_type["dwNewGuildID"] = sol::property([](const ExternalMessage::G2C_OCCUPY_SABAC& self) { return self.dwNewGuildID; }, [](ExternalMessage::G2C_OCCUPY_SABAC& self, decltype(ExternalMessage::G2C_OCCUPY_SABAC::dwNewGuildID) value) { self.dwNewGuildID = value; });
    auto external_message_g2c_sync_sabac_enroll_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST>(L, external_message_namespace, "G2C_SYNC_SABAC_ENROLL_LIST", "ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_sabac_enroll_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST(); });
    external_message_g2c_sync_sabac_enroll_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST>(new ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST()); });
    external_message_g2c_sync_sabac_enroll_list_type["EnrollList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::EnrollList);
}

}
