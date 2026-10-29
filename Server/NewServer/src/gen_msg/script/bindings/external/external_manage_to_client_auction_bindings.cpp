#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/m2c_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/ranklist_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/team_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_manage_to_client_auction_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_auction_item_info_type = cqyh::gen_msg::usertype<ExternalMessage::AUCTION_ITEM_INFO>(L, external_message_namespace, "AUCTION_ITEM_INFO", "ExternalMessage::AUCTION_ITEM_INFO");
    external_message_auction_item_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::AUCTION_ITEM_INFO(); });
    external_message_auction_item_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::AUCTION_ITEM_INFO>(new ExternalMessage::AUCTION_ITEM_INFO()); });
    external_message_auction_item_info_type["dwAuctionID"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.dwAuctionID; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::dwAuctionID) value) { self.dwAuctionID = value; });
    external_message_auction_item_info_type["dwSellerRoleID"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.dwSellerRoleID; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::dwSellerRoleID) value) { self.dwSellerRoleID = value; });
    external_message_auction_item_info_type["dwBuyerRoleID"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.dwBuyerRoleID; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::dwBuyerRoleID) value) { self.dwBuyerRoleID = value; });
    external_message_auction_item_info_type["llPrice"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.llPrice; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::llPrice) value) { self.llPrice = value; });
    external_message_auction_item_info_type["llBuyoutPrice"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.llBuyoutPrice; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::llBuyoutPrice) value) { self.llBuyoutPrice = value; });
    external_message_auction_item_info_type["llDepositPrice"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.llDepositPrice; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::llDepositPrice) value) { self.llDepositPrice = value; });
    external_message_auction_item_info_type["dwStartTime"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.dwStartTime; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::dwStartTime) value) { self.dwStartTime = value; });
    external_message_auction_item_info_type["dwFinishTime"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.dwFinishTime; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::dwFinishTime) value) { self.dwFinishTime = value; });
    external_message_auction_item_info_type["byAuctionStatus"] = sol::property([](const ExternalMessage::AUCTION_ITEM_INFO& self) { return self.byAuctionStatus; }, [](ExternalMessage::AUCTION_ITEM_INFO& self, decltype(ExternalMessage::AUCTION_ITEM_INFO::byAuctionStatus) value) { self.byAuctionStatus = value; });
    external_message_auction_item_info_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::AUCTION_ITEM_INFO::acItem);
    auto external_message_m2c_auction_search_type_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_SEARCH_TYPE>(L, external_message_namespace, "M2C_AUCTION_SEARCH_TYPE", "ExternalMessage::M2C_AUCTION_SEARCH_TYPE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_search_type_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_SEARCH_TYPE(); });
    external_message_m2c_auction_search_type_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_SEARCH_TYPE>(new ExternalMessage::M2C_AUCTION_SEARCH_TYPE()); });
    external_message_m2c_auction_search_type_type["byAuctionType"] = sol::property([](const ExternalMessage::M2C_AUCTION_SEARCH_TYPE& self) { return self.byAuctionType; }, [](ExternalMessage::M2C_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::M2C_AUCTION_SEARCH_TYPE::byAuctionType) value) { self.byAuctionType = value; });
    external_message_m2c_auction_search_type_type["byAuctionSubType"] = sol::property([](const ExternalMessage::M2C_AUCTION_SEARCH_TYPE& self) { return self.byAuctionSubType; }, [](ExternalMessage::M2C_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::M2C_AUCTION_SEARCH_TYPE::byAuctionSubType) value) { self.byAuctionSubType = value; });
    external_message_m2c_auction_search_type_type["nItemCount"] = sol::property([](const ExternalMessage::M2C_AUCTION_SEARCH_TYPE& self) { return self.nItemCount; }, [](ExternalMessage::M2C_AUCTION_SEARCH_TYPE& self, decltype(ExternalMessage::M2C_AUCTION_SEARCH_TYPE::nItemCount) value) { self.nItemCount = value; });
    external_message_m2c_auction_search_type_type["nItemID"] = cqyh::lua::array_property(&ExternalMessage::M2C_AUCTION_SEARCH_TYPE::nItemID);
    auto external_message_m2c_auction_search_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_SEARCH>(L, external_message_namespace, "M2C_AUCTION_SEARCH", "ExternalMessage::M2C_AUCTION_SEARCH", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_search_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_SEARCH(); });
    external_message_m2c_auction_search_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_SEARCH>(new ExternalMessage::M2C_AUCTION_SEARCH()); });
    external_message_m2c_auction_search_type["btAuctionItemCount"] = sol::property([](const ExternalMessage::M2C_AUCTION_SEARCH& self) { return self.btAuctionItemCount; }, [](ExternalMessage::M2C_AUCTION_SEARCH& self, decltype(ExternalMessage::M2C_AUCTION_SEARCH::btAuctionItemCount) value) { self.btAuctionItemCount = value; });
    external_message_m2c_auction_search_type["acAuctionItems"] = cqyh::lua::string_property(&ExternalMessage::M2C_AUCTION_SEARCH::acAuctionItems);
    auto external_message_m2c_auction_query_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_QUERY>(L, external_message_namespace, "M2C_AUCTION_QUERY", "ExternalMessage::M2C_AUCTION_QUERY", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_query_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_QUERY(); });
    external_message_m2c_auction_query_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_QUERY>(new ExternalMessage::M2C_AUCTION_QUERY()); });
    external_message_m2c_auction_query_type["dwAuctionID"] = sol::property([](const ExternalMessage::M2C_AUCTION_QUERY& self) { return self.dwAuctionID; }, [](ExternalMessage::M2C_AUCTION_QUERY& self, decltype(ExternalMessage::M2C_AUCTION_QUERY::dwAuctionID) value) { self.dwAuctionID = value; });
    external_message_m2c_auction_query_type["byFound"] = sol::property([](const ExternalMessage::M2C_AUCTION_QUERY& self) { return self.byFound; }, [](ExternalMessage::M2C_AUCTION_QUERY& self, decltype(ExternalMessage::M2C_AUCTION_QUERY::byFound) value) { self.byFound = value; });
    external_message_m2c_auction_query_type["dwBuyerCount"] = sol::property([](const ExternalMessage::M2C_AUCTION_QUERY& self) { return self.dwBuyerCount; }, [](ExternalMessage::M2C_AUCTION_QUERY& self, decltype(ExternalMessage::M2C_AUCTION_QUERY::dwBuyerCount) value) { self.dwBuyerCount = value; });
    external_message_m2c_auction_query_type["szSellerName"] = cqyh::lua::string_property(&ExternalMessage::M2C_AUCTION_QUERY::szSellerName);
    external_message_m2c_auction_query_type["bySellerLevel"] = sol::property([](const ExternalMessage::M2C_AUCTION_QUERY& self) { return self.bySellerLevel; }, [](ExternalMessage::M2C_AUCTION_QUERY& self, decltype(ExternalMessage::M2C_AUCTION_QUERY::bySellerLevel) value) { self.bySellerLevel = value; });
    external_message_m2c_auction_query_type["bySellerClass"] = sol::property([](const ExternalMessage::M2C_AUCTION_QUERY& self) { return self.bySellerClass; }, [](ExternalMessage::M2C_AUCTION_QUERY& self, decltype(ExternalMessage::M2C_AUCTION_QUERY::bySellerClass) value) { self.bySellerClass = value; });
    external_message_m2c_auction_query_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::M2C_AUCTION_QUERY::szGuildName);
    external_message_m2c_auction_query_type["stAuction"] = &ExternalMessage::M2C_AUCTION_QUERY::stAuction;
    auto external_message_m2c_auction_sell_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_SELL>(L, external_message_namespace, "M2C_AUCTION_SELL", "ExternalMessage::M2C_AUCTION_SELL", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_sell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_SELL(); });
    external_message_m2c_auction_sell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_SELL>(new ExternalMessage::M2C_AUCTION_SELL()); });
    external_message_m2c_auction_sell_type["stAuction"] = &ExternalMessage::M2C_AUCTION_SELL::stAuction;
    auto external_message_m2c_auction_cancel_sell_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_CANCEL_SELL>(L, external_message_namespace, "M2C_AUCTION_CANCEL_SELL", "ExternalMessage::M2C_AUCTION_CANCEL_SELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_cancel_sell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_CANCEL_SELL(); });
    external_message_m2c_auction_cancel_sell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_CANCEL_SELL>(new ExternalMessage::M2C_AUCTION_CANCEL_SELL()); });
    external_message_m2c_auction_cancel_sell_type["dwAuctionID"] = sol::property([](const ExternalMessage::M2C_AUCTION_CANCEL_SELL& self) { return self.dwAuctionID; }, [](ExternalMessage::M2C_AUCTION_CANCEL_SELL& self, decltype(ExternalMessage::M2C_AUCTION_CANCEL_SELL::dwAuctionID) value) { self.dwAuctionID = value; });
    auto external_message_m2c_auction_buy_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_BUY>(L, external_message_namespace, "M2C_AUCTION_BUY", "ExternalMessage::M2C_AUCTION_BUY", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_buy_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_BUY(); });
    external_message_m2c_auction_buy_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_BUY>(new ExternalMessage::M2C_AUCTION_BUY()); });
    external_message_m2c_auction_buy_type["stAuction"] = &ExternalMessage::M2C_AUCTION_BUY::stAuction;
    auto external_message_m2c_auction_finished_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_FINISHED>(L, external_message_namespace, "M2C_AUCTION_FINISHED", "ExternalMessage::M2C_AUCTION_FINISHED", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_finished_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_FINISHED(); });
    external_message_m2c_auction_finished_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_FINISHED>(new ExternalMessage::M2C_AUCTION_FINISHED()); });
    external_message_m2c_auction_finished_type["dwAuctionID"] = sol::property([](const ExternalMessage::M2C_AUCTION_FINISHED& self) { return self.dwAuctionID; }, [](ExternalMessage::M2C_AUCTION_FINISHED& self, decltype(ExternalMessage::M2C_AUCTION_FINISHED::dwAuctionID) value) { self.dwAuctionID = value; });
    external_message_m2c_auction_finished_type["bySuccess"] = sol::property([](const ExternalMessage::M2C_AUCTION_FINISHED& self) { return self.bySuccess; }, [](ExternalMessage::M2C_AUCTION_FINISHED& self, decltype(ExternalMessage::M2C_AUCTION_FINISHED::bySuccess) value) { self.bySuccess = value; });
    auto external_message_m2c_auction_list_my_selling_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_LIST_MY_SELLING>(L, external_message_namespace, "M2C_AUCTION_LIST_MY_SELLING", "ExternalMessage::M2C_AUCTION_LIST_MY_SELLING", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_list_my_selling_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_LIST_MY_SELLING(); });
    external_message_m2c_auction_list_my_selling_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_LIST_MY_SELLING>(new ExternalMessage::M2C_AUCTION_LIST_MY_SELLING()); });
    external_message_m2c_auction_list_my_selling_type["byCount"] = sol::property([](const ExternalMessage::M2C_AUCTION_LIST_MY_SELLING& self) { return self.byCount; }, [](ExternalMessage::M2C_AUCTION_LIST_MY_SELLING& self, decltype(ExternalMessage::M2C_AUCTION_LIST_MY_SELLING::byCount) value) { self.byCount = value; });
    external_message_m2c_auction_list_my_selling_type["acAuctionItems"] = cqyh::lua::string_property(&ExternalMessage::M2C_AUCTION_LIST_MY_SELLING::acAuctionItems);
    auto external_message_m2c_auction_list_my_buying_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_AUCTION_LIST_MY_BUYING>(L, external_message_namespace, "M2C_AUCTION_LIST_MY_BUYING", "ExternalMessage::M2C_AUCTION_LIST_MY_BUYING", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_auction_list_my_buying_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_AUCTION_LIST_MY_BUYING(); });
    external_message_m2c_auction_list_my_buying_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_AUCTION_LIST_MY_BUYING>(new ExternalMessage::M2C_AUCTION_LIST_MY_BUYING()); });
    external_message_m2c_auction_list_my_buying_type["byCount"] = sol::property([](const ExternalMessage::M2C_AUCTION_LIST_MY_BUYING& self) { return self.byCount; }, [](ExternalMessage::M2C_AUCTION_LIST_MY_BUYING& self, decltype(ExternalMessage::M2C_AUCTION_LIST_MY_BUYING::byCount) value) { self.byCount = value; });
    external_message_m2c_auction_list_my_buying_type["acAuctionItems"] = cqyh::lua::string_property(&ExternalMessage::M2C_AUCTION_LIST_MY_BUYING::acAuctionItems);
}

}
