#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_shops_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_check_npc_shop_data_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHECK_NPC_SHOP_DATA>(L, external_message_namespace, "C2G_CHECK_NPC_SHOP_DATA", "ExternalMessage::C2G_CHECK_NPC_SHOP_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_check_npc_shop_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHECK_NPC_SHOP_DATA(); });
    external_message_c2g_check_npc_shop_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHECK_NPC_SHOP_DATA>(new ExternalMessage::C2G_CHECK_NPC_SHOP_DATA()); });
    external_message_c2g_check_npc_shop_data_type["dwCrc"] = sol::property([](const ExternalMessage::C2G_CHECK_NPC_SHOP_DATA& self) { return self.dwCrc; }, [](ExternalMessage::C2G_CHECK_NPC_SHOP_DATA& self, decltype(ExternalMessage::C2G_CHECK_NPC_SHOP_DATA::dwCrc) value) { self.dwCrc = value; });
    auto external_message_c2g_view_sold_items_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_VIEW_SOLD_ITEMS>(L, external_message_namespace, "C2G_VIEW_SOLD_ITEMS", "ExternalMessage::C2G_VIEW_SOLD_ITEMS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_view_sold_items_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_VIEW_SOLD_ITEMS(); });
    external_message_c2g_view_sold_items_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_VIEW_SOLD_ITEMS>(new ExternalMessage::C2G_VIEW_SOLD_ITEMS()); });
    auto external_message_c2g_buy_back_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BUY_BACK_ITEM>(L, external_message_namespace, "C2G_BUY_BACK_ITEM", "ExternalMessage::C2G_BUY_BACK_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_buy_back_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BUY_BACK_ITEM(); });
    external_message_c2g_buy_back_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BUY_BACK_ITEM>(new ExternalMessage::C2G_BUY_BACK_ITEM()); });
    external_message_c2g_buy_back_item_type["bySlot"] = sol::property([](const ExternalMessage::C2G_BUY_BACK_ITEM& self) { return self.bySlot; }, [](ExternalMessage::C2G_BUY_BACK_ITEM& self, decltype(ExternalMessage::C2G_BUY_BACK_ITEM::bySlot) value) { self.bySlot = value; });
    auto external_message_c2g_black_market_limit_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BLACK_MARKET_LIMIT_INFO>(L, external_message_namespace, "C2G_BLACK_MARKET_LIMIT_INFO", "ExternalMessage::C2G_BLACK_MARKET_LIMIT_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_black_market_limit_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BLACK_MARKET_LIMIT_INFO(); });
    external_message_c2g_black_market_limit_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BLACK_MARKET_LIMIT_INFO>(new ExternalMessage::C2G_BLACK_MARKET_LIMIT_INFO()); });
    auto external_message_c2g_black_market_cooldown_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BLACK_MARKET_COOLDOWN_INFO>(L, external_message_namespace, "C2G_BLACK_MARKET_COOLDOWN_INFO", "ExternalMessage::C2G_BLACK_MARKET_COOLDOWN_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_black_market_cooldown_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BLACK_MARKET_COOLDOWN_INFO(); });
    external_message_c2g_black_market_cooldown_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BLACK_MARKET_COOLDOWN_INFO>(new ExternalMessage::C2G_BLACK_MARKET_COOLDOWN_INFO()); });
    auto external_message_c2g_black_market_buy_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BLACK_MARKET_BUY>(L, external_message_namespace, "C2G_BLACK_MARKET_BUY", "ExternalMessage::C2G_BLACK_MARKET_BUY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_black_market_buy_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BLACK_MARKET_BUY(); });
    external_message_c2g_black_market_buy_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BLACK_MARKET_BUY>(new ExternalMessage::C2G_BLACK_MARKET_BUY()); });
    external_message_c2g_black_market_buy_type["nBlackMarketProductID"] = sol::property([](const ExternalMessage::C2G_BLACK_MARKET_BUY& self) { return self.nBlackMarketProductID; }, [](ExternalMessage::C2G_BLACK_MARKET_BUY& self, decltype(ExternalMessage::C2G_BLACK_MARKET_BUY::nBlackMarketProductID) value) { self.nBlackMarketProductID = value; });
}

}
