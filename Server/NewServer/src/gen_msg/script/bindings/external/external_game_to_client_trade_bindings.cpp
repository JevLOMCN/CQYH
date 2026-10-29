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

void register_external_game_to_client_trade_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_trade_state_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TRADE_STATE_CHANGE>(L, external_message_namespace, "G2C_TRADE_STATE_CHANGE", "ExternalMessage::G2C_TRADE_STATE_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_trade_state_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TRADE_STATE_CHANGE(); });
    external_message_g2c_trade_state_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TRADE_STATE_CHANGE>(new ExternalMessage::G2C_TRADE_STATE_CHANGE()); });
    external_message_g2c_trade_state_change_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_TRADE_STATE_CHANGE& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_TRADE_STATE_CHANGE& self, decltype(ExternalMessage::G2C_TRADE_STATE_CHANGE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_trade_state_change_type["byTradeState"] = sol::property([](const ExternalMessage::G2C_TRADE_STATE_CHANGE& self) { return self.byTradeState; }, [](ExternalMessage::G2C_TRADE_STATE_CHANGE& self, decltype(ExternalMessage::G2C_TRADE_STATE_CHANGE::byTradeState) value) { self.byTradeState = value; });
    external_message_g2c_trade_state_change_type["nRoleLevel"] = sol::property([](const ExternalMessage::G2C_TRADE_STATE_CHANGE& self) { return self.nRoleLevel; }, [](ExternalMessage::G2C_TRADE_STATE_CHANGE& self, decltype(ExternalMessage::G2C_TRADE_STATE_CHANGE::nRoleLevel) value) { self.nRoleLevel = value; });
    auto external_message_g2c_trade_set_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TRADE_SET_ITEM>(L, external_message_namespace, "G2C_TRADE_SET_ITEM", "ExternalMessage::G2C_TRADE_SET_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_trade_set_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TRADE_SET_ITEM(); });
    external_message_g2c_trade_set_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TRADE_SET_ITEM>(new ExternalMessage::G2C_TRADE_SET_ITEM()); });
    external_message_g2c_trade_set_item_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_TRADE_SET_ITEM& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_TRADE_SET_ITEM& self, decltype(ExternalMessage::G2C_TRADE_SET_ITEM::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_trade_set_item_type["byTradeSlot"] = sol::property([](const ExternalMessage::G2C_TRADE_SET_ITEM& self) { return self.byTradeSlot; }, [](ExternalMessage::G2C_TRADE_SET_ITEM& self, decltype(ExternalMessage::G2C_TRADE_SET_ITEM::byTradeSlot) value) { self.byTradeSlot = value; });
    external_message_g2c_trade_set_item_type["bAdd"] = sol::property([](const ExternalMessage::G2C_TRADE_SET_ITEM& self) { return self.bAdd; }, [](ExternalMessage::G2C_TRADE_SET_ITEM& self, decltype(ExternalMessage::G2C_TRADE_SET_ITEM::bAdd) value) { self.bAdd = value; });
    external_message_g2c_trade_set_item_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::G2C_TRADE_SET_ITEM::acItem);
}

}
