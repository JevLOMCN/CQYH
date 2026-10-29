#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_economy_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_use_moneytype01_only_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_USE_MONEYTYPE01_ONLY>(L, external_message_namespace, "C2G_USE_MONEYTYPE01_ONLY", "ExternalMessage::C2G_USE_MONEYTYPE01_ONLY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_use_moneytype01_only_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_USE_MONEYTYPE01_ONLY(); });
    external_message_c2g_use_moneytype01_only_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_USE_MONEYTYPE01_ONLY>(new ExternalMessage::C2G_USE_MONEYTYPE01_ONLY()); });
    external_message_c2g_use_moneytype01_only_type["byUseMoneytype01Only"] = sol::property([](const ExternalMessage::C2G_USE_MONEYTYPE01_ONLY& self) { return self.byUseMoneytype01Only; }, [](ExternalMessage::C2G_USE_MONEYTYPE01_ONLY& self, decltype(ExternalMessage::C2G_USE_MONEYTYPE01_ONLY::byUseMoneytype01Only) value) { self.byUseMoneytype01Only = value; });
    auto external_message_c2g_trade_set_money_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_SET_MONEY>(L, external_message_namespace, "C2G_TRADE_SET_MONEY", "ExternalMessage::C2G_TRADE_SET_MONEY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_set_money_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_SET_MONEY(); });
    external_message_c2g_trade_set_money_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_SET_MONEY>(new ExternalMessage::C2G_TRADE_SET_MONEY()); });
    external_message_c2g_trade_set_money_type["nMoney"] = sol::property([](const ExternalMessage::C2G_TRADE_SET_MONEY& self) { return self.nMoney; }, [](ExternalMessage::C2G_TRADE_SET_MONEY& self, decltype(ExternalMessage::C2G_TRADE_SET_MONEY::nMoney) value) { self.nMoney = value; });
    auto external_message_c2g_spirit_exchange_air_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR>(L, external_message_namespace, "C2G_SPIRIT_EXCHANGE_AIR", "ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_spirit_exchange_air_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR(); });
    external_message_c2g_spirit_exchange_air_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR>(new ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR()); });
    external_message_c2g_spirit_exchange_air_type["wAirID"] = sol::property([](const ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR& self) { return self.wAirID; }, [](ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR& self, decltype(ExternalMessage::C2G_SPIRIT_EXCHANGE_AIR::wAirID) value) { self.wAirID = value; });
}

}
