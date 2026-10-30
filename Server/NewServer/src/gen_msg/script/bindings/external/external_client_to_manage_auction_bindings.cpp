#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_auction_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_auction_search_type_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_SEARCH_TYPE>(L, external_message_namespace, "C2M_AUCTION_SEARCH_TYPE", "ExternalMessage::C2M_AUCTION_SEARCH_TYPE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_search_type_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_SEARCH_TYPE(); });
    external_message_c2m_auction_search_type_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_SEARCH_TYPE>(new ExternalMessage::C2M_AUCTION_SEARCH_TYPE()); });
    external_message_c2m_auction_search_type_type["bySortBy"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self) { return self.bySortBy; }, [](ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH_TYPE::bySortBy) value) { self.bySortBy = value; });
    external_message_c2m_auction_search_type_type["byAscending"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self) { return self.byAscending; }, [](ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH_TYPE::byAscending) value) { self.byAscending = value; });
    external_message_c2m_auction_search_type_type["byAuctionType"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self) { return self.byAuctionType; }, [](ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH_TYPE::byAuctionType) value) { self.byAuctionType = value; });
    external_message_c2m_auction_search_type_type["byAuctionSubType"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self) { return self.byAuctionSubType; }, [](ExternalMessage::C2M_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH_TYPE::byAuctionSubType) value) { self.byAuctionSubType = value; });
    auto external_message_c2m_auction_search_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_SEARCH>(L, external_message_namespace, "C2M_AUCTION_SEARCH", "ExternalMessage::C2M_AUCTION_SEARCH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_search_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_SEARCH(); });
    external_message_c2m_auction_search_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_SEARCH>(new ExternalMessage::C2M_AUCTION_SEARCH()); });
    external_message_c2m_auction_search_type["bySortBy"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH& self) { return self.bySortBy; }, [](ExternalMessage::C2M_AUCTION_SEARCH& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH::bySortBy) value) { self.bySortBy = value; });
    external_message_c2m_auction_search_type["byAscending"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH& self) { return self.byAscending; }, [](ExternalMessage::C2M_AUCTION_SEARCH& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH::byAscending) value) { self.byAscending = value; });
    external_message_c2m_auction_search_type["nItemTemplateID"] = sol::property([](const ExternalMessage::C2M_AUCTION_SEARCH& self) { return self.nItemTemplateID; }, [](ExternalMessage::C2M_AUCTION_SEARCH& self, decltype(ExternalMessage::C2M_AUCTION_SEARCH::nItemTemplateID) value) { self.nItemTemplateID = value; });
    auto external_message_c2m_auction_query_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_QUERY>(L, external_message_namespace, "C2M_AUCTION_QUERY", "ExternalMessage::C2M_AUCTION_QUERY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_query_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_QUERY(); });
    external_message_c2m_auction_query_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_QUERY>(new ExternalMessage::C2M_AUCTION_QUERY()); });
    external_message_c2m_auction_query_type["dwAuctionID"] = sol::property([](const ExternalMessage::C2M_AUCTION_QUERY& self) { return self.dwAuctionID; }, [](ExternalMessage::C2M_AUCTION_QUERY& self, decltype(ExternalMessage::C2M_AUCTION_QUERY::dwAuctionID) value) { self.dwAuctionID = value; });
    auto external_message_c2m_auction_sell_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_SELL>(L, external_message_namespace, "C2M_AUCTION_SELL", "ExternalMessage::C2M_AUCTION_SELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_sell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_SELL(); });
    external_message_c2m_auction_sell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_SELL>(new ExternalMessage::C2M_AUCTION_SELL()); });
    external_message_c2m_auction_sell_type["byConSlot"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.byConSlot; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::byConSlot) value) { self.byConSlot = value; });
    external_message_c2m_auction_sell_type["byItemSlot"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.byItemSlot; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::byItemSlot) value) { self.byItemSlot = value; });
    external_message_c2m_auction_sell_type["byAuctionTimeType"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.byAuctionTimeType; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::byAuctionTimeType) value) { self.byAuctionTimeType = value; });
    external_message_c2m_auction_sell_type["byOpen"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.byOpen; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::byOpen) value) { self.byOpen = value; });
    external_message_c2m_auction_sell_type["llPrice"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.llPrice; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::llPrice) value) { self.llPrice = value; });
    external_message_c2m_auction_sell_type["llBuyoutPrice"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.llBuyoutPrice; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::llBuyoutPrice) value) { self.llBuyoutPrice = value; });
    external_message_c2m_auction_sell_type["llDepositePrice"] = sol::property([](const ExternalMessage::C2M_AUCTION_SELL& self) { return self.llDepositePrice; }, [](ExternalMessage::C2M_AUCTION_SELL& self, decltype(ExternalMessage::C2M_AUCTION_SELL::llDepositePrice) value) { self.llDepositePrice = value; });
    auto external_message_c2m_auction_cancel_sell_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_CANCEL_SELL>(L, external_message_namespace, "C2M_AUCTION_CANCEL_SELL", "ExternalMessage::C2M_AUCTION_CANCEL_SELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_cancel_sell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_CANCEL_SELL(); });
    external_message_c2m_auction_cancel_sell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_CANCEL_SELL>(new ExternalMessage::C2M_AUCTION_CANCEL_SELL()); });
    external_message_c2m_auction_cancel_sell_type["dwAuctionID"] = sol::property([](const ExternalMessage::C2M_AUCTION_CANCEL_SELL& self) { return self.dwAuctionID; }, [](ExternalMessage::C2M_AUCTION_CANCEL_SELL& self, decltype(ExternalMessage::C2M_AUCTION_CANCEL_SELL::dwAuctionID) value) { self.dwAuctionID = value; });
    auto external_message_c2m_auction_buy_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_BUY>(L, external_message_namespace, "C2M_AUCTION_BUY", "ExternalMessage::C2M_AUCTION_BUY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_buy_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_BUY(); });
    external_message_c2m_auction_buy_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_BUY>(new ExternalMessage::C2M_AUCTION_BUY()); });
    external_message_c2m_auction_buy_type["dwAuctionID"] = sol::property([](const ExternalMessage::C2M_AUCTION_BUY& self) { return self.dwAuctionID; }, [](ExternalMessage::C2M_AUCTION_BUY& self, decltype(ExternalMessage::C2M_AUCTION_BUY::dwAuctionID) value) { self.dwAuctionID = value; });
    external_message_c2m_auction_buy_type["llBidPrice"] = sol::property([](const ExternalMessage::C2M_AUCTION_BUY& self) { return self.llBidPrice; }, [](ExternalMessage::C2M_AUCTION_BUY& self, decltype(ExternalMessage::C2M_AUCTION_BUY::llBidPrice) value) { self.llBidPrice = value; });
    auto external_message_c2m_auction_list_my_selling_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_LIST_MY_SELLING>(L, external_message_namespace, "C2M_AUCTION_LIST_MY_SELLING", "ExternalMessage::C2M_AUCTION_LIST_MY_SELLING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_list_my_selling_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_LIST_MY_SELLING(); });
    external_message_c2m_auction_list_my_selling_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_LIST_MY_SELLING>(new ExternalMessage::C2M_AUCTION_LIST_MY_SELLING()); });
    auto external_message_c2m_auction_list_my_buying_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_AUCTION_LIST_MY_BUYING>(L, external_message_namespace, "C2M_AUCTION_LIST_MY_BUYING", "ExternalMessage::C2M_AUCTION_LIST_MY_BUYING", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_auction_list_my_buying_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_AUCTION_LIST_MY_BUYING(); });
    external_message_c2m_auction_list_my_buying_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_AUCTION_LIST_MY_BUYING>(new ExternalMessage::C2M_AUCTION_LIST_MY_BUYING()); });
}

}
