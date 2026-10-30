#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_economy_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_auction_pay_left_money_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY>(L, external_message_namespace, "C2M_AUCTION_PAY_LEFT_MONEY", "ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_pay_left_money_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY(); });
    external_message_c2m_auction_pay_left_money_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY>(new ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY()); });
    external_message_c2m_auction_pay_left_money_type["dwAuctionID"] = sol::property([](const ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY& self) { return self.dwAuctionID; }, [](ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY& self, decltype(ExternalMessage::C2M_AUCTION_PAY_LEFT_MONEY::dwAuctionID) value) { self.dwAuctionID = value; });
    auto external_message_c2m_exchange_add_order_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_EXCHANGE_ADD_ORDER>(L, external_message_namespace, "C2M_EXCHANGE_ADD_ORDER", "ExternalMessage::C2M_EXCHANGE_ADD_ORDER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_exchange_add_order_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_EXCHANGE_ADD_ORDER(); });
    external_message_c2m_exchange_add_order_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_EXCHANGE_ADD_ORDER>(new ExternalMessage::C2M_EXCHANGE_ADD_ORDER()); });
    external_message_c2m_exchange_add_order_type["byOrderType"] = sol::property([](const ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self) { return self.byOrderType; }, [](ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self, decltype(ExternalMessage::C2M_EXCHANGE_ADD_ORDER::byOrderType) value) { self.byOrderType = value; });
    external_message_c2m_exchange_add_order_type["nItemID"] = sol::property([](const ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self) { return self.nItemID; }, [](ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self, decltype(ExternalMessage::C2M_EXCHANGE_ADD_ORDER::nItemID) value) { self.nItemID = value; });
    external_message_c2m_exchange_add_order_type["nPrice"] = sol::property([](const ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self) { return self.nPrice; }, [](ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self, decltype(ExternalMessage::C2M_EXCHANGE_ADD_ORDER::nPrice) value) { self.nPrice = value; });
    external_message_c2m_exchange_add_order_type["nCount"] = sol::property([](const ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self) { return self.nCount; }, [](ExternalMessage::C2M_EXCHANGE_ADD_ORDER& self, decltype(ExternalMessage::C2M_EXCHANGE_ADD_ORDER::nCount) value) { self.nCount = value; });
    auto external_message_c2m_exchange_cancel_order_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER>(L, external_message_namespace, "C2M_EXCHANGE_CANCEL_ORDER", "ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_exchange_cancel_order_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER(); });
    external_message_c2m_exchange_cancel_order_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER>(new ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER()); });
    external_message_c2m_exchange_cancel_order_type["qwOrderID"] = sol::property([](const ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER& self) { return self.qwOrderID; }, [](ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER& self, decltype(ExternalMessage::C2M_EXCHANGE_CANCEL_ORDER::qwOrderID) value) { self.qwOrderID = value; });
    auto external_message_c2m_exchange_list_prices_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_EXCHANGE_LIST_PRICES>(L, external_message_namespace, "C2M_EXCHANGE_LIST_PRICES", "ExternalMessage::C2M_EXCHANGE_LIST_PRICES", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_exchange_list_prices_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_EXCHANGE_LIST_PRICES(); });
    external_message_c2m_exchange_list_prices_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_EXCHANGE_LIST_PRICES>(new ExternalMessage::C2M_EXCHANGE_LIST_PRICES()); });
    auto external_message_c2m_exchange_list_my_orders_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_EXCHANGE_LIST_MY_ORDERS>(L, external_message_namespace, "C2M_EXCHANGE_LIST_MY_ORDERS", "ExternalMessage::C2M_EXCHANGE_LIST_MY_ORDERS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_exchange_list_my_orders_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_EXCHANGE_LIST_MY_ORDERS(); });
    external_message_c2m_exchange_list_my_orders_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_EXCHANGE_LIST_MY_ORDERS>(new ExternalMessage::C2M_EXCHANGE_LIST_MY_ORDERS()); });
    auto external_message_c2m_exchange_list_my_logs_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_EXCHANGE_LIST_MY_LOGS>(L, external_message_namespace, "C2M_EXCHANGE_LIST_MY_LOGS", "ExternalMessage::C2M_EXCHANGE_LIST_MY_LOGS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_exchange_list_my_logs_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_EXCHANGE_LIST_MY_LOGS(); });
    external_message_c2m_exchange_list_my_logs_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_EXCHANGE_LIST_MY_LOGS>(new ExternalMessage::C2M_EXCHANGE_LIST_MY_LOGS()); });
    auto external_message_c2m_check_real_money_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_CHECK_REAL_MONEY>(L, external_message_namespace, "C2M_CHECK_REAL_MONEY", "ExternalMessage::C2M_CHECK_REAL_MONEY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_check_real_money_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_CHECK_REAL_MONEY(); });
    external_message_c2m_check_real_money_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_CHECK_REAL_MONEY>(new ExternalMessage::C2M_CHECK_REAL_MONEY()); });
    auto external_message_c2m_check_real_money_shop_data_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA>(L, external_message_namespace, "C2M_CHECK_REAL_MONEY_SHOP_DATA", "ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_check_real_money_shop_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA(); });
    external_message_c2m_check_real_money_shop_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA>(new ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA()); });
    external_message_c2m_check_real_money_shop_data_type["dwCrc"] = sol::property([](const ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA& self) { return self.dwCrc; }, [](ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA& self, decltype(ExternalMessage::C2M_CHECK_REAL_MONEY_SHOP_DATA::dwCrc) value) { self.dwCrc = value; });
    auto external_message_c2m_real_money_buy_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_REAL_MONEY_BUY>(L, external_message_namespace, "C2M_REAL_MONEY_BUY", "ExternalMessage::C2M_REAL_MONEY_BUY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_real_money_buy_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_REAL_MONEY_BUY(); });
    external_message_c2m_real_money_buy_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_REAL_MONEY_BUY>(new ExternalMessage::C2M_REAL_MONEY_BUY()); });
    external_message_c2m_real_money_buy_type["nItemID"] = sol::property([](const ExternalMessage::C2M_REAL_MONEY_BUY& self) { return self.nItemID; }, [](ExternalMessage::C2M_REAL_MONEY_BUY& self, decltype(ExternalMessage::C2M_REAL_MONEY_BUY::nItemID) value) { self.nItemID = value; });
    external_message_c2m_real_money_buy_type["nCount"] = sol::property([](const ExternalMessage::C2M_REAL_MONEY_BUY& self) { return self.nCount; }, [](ExternalMessage::C2M_REAL_MONEY_BUY& self, decltype(ExternalMessage::C2M_REAL_MONEY_BUY::nCount) value) { self.nCount = value; });
    external_message_c2m_real_money_buy_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2M_REAL_MONEY_BUY::szRoleName);
    auto external_message_c2m_real_money_shop_log_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_REAL_MONEY_SHOP_LOG>(L, external_message_namespace, "C2M_REAL_MONEY_SHOP_LOG", "ExternalMessage::C2M_REAL_MONEY_SHOP_LOG", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_real_money_shop_log_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_REAL_MONEY_SHOP_LOG(); });
    external_message_c2m_real_money_shop_log_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_REAL_MONEY_SHOP_LOG>(new ExternalMessage::C2M_REAL_MONEY_SHOP_LOG()); });
    auto external_message_c2m_sale_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_SALE_INFO>(L, external_message_namespace, "C2M_SALE_INFO", "ExternalMessage::C2M_SALE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_sale_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_SALE_INFO(); });
    external_message_c2m_sale_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_SALE_INFO>(new ExternalMessage::C2M_SALE_INFO()); });
    auto external_message_c2m_buy_time_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_BUY_TIME>(L, external_message_namespace, "C2M_BUY_TIME", "ExternalMessage::C2M_BUY_TIME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_buy_time_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_BUY_TIME(); });
    external_message_c2m_buy_time_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_BUY_TIME>(new ExternalMessage::C2M_BUY_TIME()); });
    external_message_c2m_buy_time_type["dwBuyTimeCost"] = sol::property([](const ExternalMessage::C2M_BUY_TIME& self) { return self.dwBuyTimeCost; }, [](ExternalMessage::C2M_BUY_TIME& self, decltype(ExternalMessage::C2M_BUY_TIME::dwBuyTimeCost) value) { self.dwBuyTimeCost = value; });
}

}
