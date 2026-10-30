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

void register_external_game_to_client_economy_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_money_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MONEY>(L, external_message_namespace, "G2C_SYNC_MONEY", "ExternalMessage::G2C_SYNC_MONEY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_money_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_MONEY(); });
    external_message_g2c_sync_money_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_MONEY>(new ExternalMessage::G2C_SYNC_MONEY()); });
    external_message_g2c_sync_money_type["allMoneys"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::allMoneys);
    external_message_g2c_sync_money_type["allMoneysLimit"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::allMoneysLimit);
    external_message_g2c_sync_money_type["anDailyEarned"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anDailyEarned);
    external_message_g2c_sync_money_type["anDailyEarnedLimit"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anDailyEarnedLimit);
    external_message_g2c_sync_money_type["anDailyUsed"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anDailyUsed);
    external_message_g2c_sync_money_type["anDailyUsedLimit"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anDailyUsedLimit);
    external_message_g2c_sync_money_type["anWeeklyEarned"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anWeeklyEarned);
    external_message_g2c_sync_money_type["anWeeklyEarnedLimit"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anWeeklyEarnedLimit);
    external_message_g2c_sync_money_type["anWeeklyUsed"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anWeeklyUsed);
    external_message_g2c_sync_money_type["anWeeklyUsedLimit"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MONEY::anWeeklyUsedLimit);
    external_message_g2c_sync_money_type["byUseMoneytype01Only"] = sol::property([](const ExternalMessage::G2C_SYNC_MONEY& self) { return self.byUseMoneytype01Only; }, [](ExternalMessage::G2C_SYNC_MONEY& self, decltype(ExternalMessage::G2C_SYNC_MONEY::byUseMoneytype01Only) value) { self.byUseMoneytype01Only = value; });
    auto external_message_g2c_pickup_money_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_PICKUP_MONEY>(L, external_message_namespace, "G2C_PICKUP_MONEY", "ExternalMessage::G2C_PICKUP_MONEY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_pickup_money_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_PICKUP_MONEY(); });
    external_message_g2c_pickup_money_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_PICKUP_MONEY>(new ExternalMessage::G2C_PICKUP_MONEY()); });
    external_message_g2c_pickup_money_type["nMoneyType"] = sol::property([](const ExternalMessage::G2C_PICKUP_MONEY& self) { return self.nMoneyType; }, [](ExternalMessage::G2C_PICKUP_MONEY& self, decltype(ExternalMessage::G2C_PICKUP_MONEY::nMoneyType) value) { self.nMoneyType = value; });
    external_message_g2c_pickup_money_type["nCount"] = sol::property([](const ExternalMessage::G2C_PICKUP_MONEY& self) { return self.nCount; }, [](ExternalMessage::G2C_PICKUP_MONEY& self, decltype(ExternalMessage::G2C_PICKUP_MONEY::nCount) value) { self.nCount = value; });
    auto external_message_g2c_money_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_MONEY_CHANGE>(L, external_message_namespace, "G2C_MONEY_CHANGE", "ExternalMessage::G2C_MONEY_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_money_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_MONEY_CHANGE(); });
    external_message_g2c_money_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_MONEY_CHANGE>(new ExternalMessage::G2C_MONEY_CHANGE()); });
    external_message_g2c_money_change_type["nMoneyType"] = sol::property([](const ExternalMessage::G2C_MONEY_CHANGE& self) { return self.nMoneyType; }, [](ExternalMessage::G2C_MONEY_CHANGE& self, decltype(ExternalMessage::G2C_MONEY_CHANGE::nMoneyType) value) { self.nMoneyType = value; });
    external_message_g2c_money_change_type["llMoney"] = sol::property([](const ExternalMessage::G2C_MONEY_CHANGE& self) { return self.llMoney; }, [](ExternalMessage::G2C_MONEY_CHANGE& self, decltype(ExternalMessage::G2C_MONEY_CHANGE::llMoney) value) { self.llMoney = value; });
    external_message_g2c_money_change_type["nDailyEarned"] = sol::property([](const ExternalMessage::G2C_MONEY_CHANGE& self) { return self.nDailyEarned; }, [](ExternalMessage::G2C_MONEY_CHANGE& self, decltype(ExternalMessage::G2C_MONEY_CHANGE::nDailyEarned) value) { self.nDailyEarned = value; });
    external_message_g2c_money_change_type["nDailyUsed"] = sol::property([](const ExternalMessage::G2C_MONEY_CHANGE& self) { return self.nDailyUsed; }, [](ExternalMessage::G2C_MONEY_CHANGE& self, decltype(ExternalMessage::G2C_MONEY_CHANGE::nDailyUsed) value) { self.nDailyUsed = value; });
    external_message_g2c_money_change_type["nWeeklyEarned"] = sol::property([](const ExternalMessage::G2C_MONEY_CHANGE& self) { return self.nWeeklyEarned; }, [](ExternalMessage::G2C_MONEY_CHANGE& self, decltype(ExternalMessage::G2C_MONEY_CHANGE::nWeeklyEarned) value) { self.nWeeklyEarned = value; });
    external_message_g2c_money_change_type["nWeeklyUsed"] = sol::property([](const ExternalMessage::G2C_MONEY_CHANGE& self) { return self.nWeeklyUsed; }, [](ExternalMessage::G2C_MONEY_CHANGE& self, decltype(ExternalMessage::G2C_MONEY_CHANGE::nWeeklyUsed) value) { self.nWeeklyUsed = value; });
    auto external_message_g2c_trade_set_money_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TRADE_SET_MONEY>(L, external_message_namespace, "G2C_TRADE_SET_MONEY", "ExternalMessage::G2C_TRADE_SET_MONEY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_trade_set_money_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TRADE_SET_MONEY(); });
    external_message_g2c_trade_set_money_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TRADE_SET_MONEY>(new ExternalMessage::G2C_TRADE_SET_MONEY()); });
    external_message_g2c_trade_set_money_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_TRADE_SET_MONEY& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_TRADE_SET_MONEY& self, decltype(ExternalMessage::G2C_TRADE_SET_MONEY::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_trade_set_money_type["nMoney"] = sol::property([](const ExternalMessage::G2C_TRADE_SET_MONEY& self) { return self.nMoney; }, [](ExternalMessage::G2C_TRADE_SET_MONEY& self, decltype(ExternalMessage::G2C_TRADE_SET_MONEY::nMoney) value) { self.nMoney = value; });
}

}
