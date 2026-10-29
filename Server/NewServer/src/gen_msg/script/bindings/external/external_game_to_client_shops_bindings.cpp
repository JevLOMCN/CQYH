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

void register_external_game_to_client_shops_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_npc_shop_data_type = cqyh::gen_msg::usertype<ExternalMessage::NPC_SHOP_DATA>(L, external_message_namespace, "NPC_SHOP_DATA", "ExternalMessage::NPC_SHOP_DATA");
    external_message_npc_shop_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::NPC_SHOP_DATA(); });
    external_message_npc_shop_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::NPC_SHOP_DATA>(new ExternalMessage::NPC_SHOP_DATA()); });
    external_message_npc_shop_data_type["m_nShopID"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nShopID; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nShopID) value) { self.m_nShopID = value; });
    external_message_npc_shop_data_type["m_szShopName"] = cqyh::lua::string_property(&ExternalMessage::NPC_SHOP_DATA::m_szShopName);
    external_message_npc_shop_data_type["m_nItemID"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nItemID; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nItemID) value) { self.m_nItemID = value; });
    external_message_npc_shop_data_type["m_nItemCount"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nItemCount; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nItemCount) value) { self.m_nItemCount = value; });
    external_message_npc_shop_data_type["m_nCostItemID1"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nCostItemID1; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nCostItemID1) value) { self.m_nCostItemID1 = value; });
    external_message_npc_shop_data_type["m_nCostValue1"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nCostValue1; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nCostValue1) value) { self.m_nCostValue1 = value; });
    external_message_npc_shop_data_type["m_nCostItemID2"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nCostItemID2; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nCostItemID2) value) { self.m_nCostItemID2 = value; });
    external_message_npc_shop_data_type["m_nCostValue2"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nCostValue2; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nCostValue2) value) { self.m_nCostValue2 = value; });
    external_message_npc_shop_data_type["m_nRequireForceID"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nRequireForceID; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nRequireForceID) value) { self.m_nRequireForceID = value; });
    external_message_npc_shop_data_type["m_nReputation"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nReputation; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nReputation) value) { self.m_nReputation = value; });
    external_message_npc_shop_data_type["m_nRequireGuildBuildingID"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nRequireGuildBuildingID; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nRequireGuildBuildingID) value) { self.m_nRequireGuildBuildingID = value; });
    external_message_npc_shop_data_type["m_nBuildingLevel"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nBuildingLevel; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nBuildingLevel) value) { self.m_nBuildingLevel = value; });
    external_message_npc_shop_data_type["m_nShopTypeSell"] = sol::property([](const ExternalMessage::NPC_SHOP_DATA& self) { return self.m_nShopTypeSell; }, [](ExternalMessage::NPC_SHOP_DATA& self, decltype(ExternalMessage::NPC_SHOP_DATA::m_nShopTypeSell) value) { self.m_nShopTypeSell = value; });
    auto external_message_g2c_sync_npc_shop_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_NPC_SHOP_DATA>(L, external_message_namespace, "G2C_SYNC_NPC_SHOP_DATA", "ExternalMessage::G2C_SYNC_NPC_SHOP_DATA", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_npc_shop_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_NPC_SHOP_DATA(); });
    external_message_g2c_sync_npc_shop_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_NPC_SHOP_DATA>(new ExternalMessage::G2C_SYNC_NPC_SHOP_DATA()); });
    external_message_g2c_sync_npc_shop_data_type["dwCrc"] = sol::property([](const ExternalMessage::G2C_SYNC_NPC_SHOP_DATA& self) { return self.dwCrc; }, [](ExternalMessage::G2C_SYNC_NPC_SHOP_DATA& self, decltype(ExternalMessage::G2C_SYNC_NPC_SHOP_DATA::dwCrc) value) { self.dwCrc = value; });
    external_message_g2c_sync_npc_shop_data_type["dwItemCount"] = sol::property([](const ExternalMessage::G2C_SYNC_NPC_SHOP_DATA& self) { return self.dwItemCount; }, [](ExternalMessage::G2C_SYNC_NPC_SHOP_DATA& self, decltype(ExternalMessage::G2C_SYNC_NPC_SHOP_DATA::dwItemCount) value) { self.dwItemCount = value; });
    external_message_g2c_sync_npc_shop_data_type["ShopData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_NPC_SHOP_DATA::ShopData);
    auto external_message_g2c_view_sold_items_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_VIEW_SOLD_ITEMS>(L, external_message_namespace, "G2C_VIEW_SOLD_ITEMS", "ExternalMessage::G2C_VIEW_SOLD_ITEMS", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_view_sold_items_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_VIEW_SOLD_ITEMS(); });
    external_message_g2c_view_sold_items_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_VIEW_SOLD_ITEMS>(new ExternalMessage::G2C_VIEW_SOLD_ITEMS()); });
    external_message_g2c_view_sold_items_type["byCount"] = sol::property([](const ExternalMessage::G2C_VIEW_SOLD_ITEMS& self) { return self.byCount; }, [](ExternalMessage::G2C_VIEW_SOLD_ITEMS& self, decltype(ExternalMessage::G2C_VIEW_SOLD_ITEMS::byCount) value) { self.byCount = value; });
    external_message_g2c_view_sold_items_type["acItems"] = cqyh::lua::string_property(&ExternalMessage::G2C_VIEW_SOLD_ITEMS::acItems);
    auto external_message_black_market_limit_info_type = cqyh::gen_msg::usertype<ExternalMessage::BLACK_MARKET_LIMIT_INFO>(L, external_message_namespace, "BLACK_MARKET_LIMIT_INFO", "ExternalMessage::BLACK_MARKET_LIMIT_INFO");
    external_message_black_market_limit_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::BLACK_MARKET_LIMIT_INFO(); });
    external_message_black_market_limit_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::BLACK_MARKET_LIMIT_INFO>(new ExternalMessage::BLACK_MARKET_LIMIT_INFO()); });
    external_message_black_market_limit_info_type["nBlackMarketProductID"] = sol::property([](const ExternalMessage::BLACK_MARKET_LIMIT_INFO& self) { return self.nBlackMarketProductID; }, [](ExternalMessage::BLACK_MARKET_LIMIT_INFO& self, decltype(ExternalMessage::BLACK_MARKET_LIMIT_INFO::nBlackMarketProductID) value) { self.nBlackMarketProductID = value; });
    external_message_black_market_limit_info_type["nCount"] = sol::property([](const ExternalMessage::BLACK_MARKET_LIMIT_INFO& self) { return self.nCount; }, [](ExternalMessage::BLACK_MARKET_LIMIT_INFO& self, decltype(ExternalMessage::BLACK_MARKET_LIMIT_INFO::nCount) value) { self.nCount = value; });
    auto external_message_g2c_black_market_limit_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO>(L, external_message_namespace, "G2C_BLACK_MARKET_LIMIT_INFO", "ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_black_market_limit_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO(); });
    external_message_g2c_black_market_limit_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO>(new ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO()); });
    external_message_g2c_black_market_limit_info_type["nLimitInfoCount"] = sol::property([](const ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO& self) { return self.nLimitInfoCount; }, [](ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO& self, decltype(ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO::nLimitInfoCount) value) { self.nLimitInfoCount = value; });
    external_message_g2c_black_market_limit_info_type["LimitInfo"] = cqyh::lua::array_property(&ExternalMessage::G2C_BLACK_MARKET_LIMIT_INFO::LimitInfo);
    auto external_message_black_market_cooldown_info_type = cqyh::gen_msg::usertype<ExternalMessage::BLACK_MARKET_COOLDOWN_INFO>(L, external_message_namespace, "BLACK_MARKET_COOLDOWN_INFO", "ExternalMessage::BLACK_MARKET_COOLDOWN_INFO");
    external_message_black_market_cooldown_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::BLACK_MARKET_COOLDOWN_INFO(); });
    external_message_black_market_cooldown_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::BLACK_MARKET_COOLDOWN_INFO>(new ExternalMessage::BLACK_MARKET_COOLDOWN_INFO()); });
    external_message_black_market_cooldown_info_type["nBlackMarketProductID"] = sol::property([](const ExternalMessage::BLACK_MARKET_COOLDOWN_INFO& self) { return self.nBlackMarketProductID; }, [](ExternalMessage::BLACK_MARKET_COOLDOWN_INFO& self, decltype(ExternalMessage::BLACK_MARKET_COOLDOWN_INFO::nBlackMarketProductID) value) { self.nBlackMarketProductID = value; });
    external_message_black_market_cooldown_info_type["dwCooldown"] = sol::property([](const ExternalMessage::BLACK_MARKET_COOLDOWN_INFO& self) { return self.dwCooldown; }, [](ExternalMessage::BLACK_MARKET_COOLDOWN_INFO& self, decltype(ExternalMessage::BLACK_MARKET_COOLDOWN_INFO::dwCooldown) value) { self.dwCooldown = value; });
    auto external_message_g2c_black_market_cooldown_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO>(L, external_message_namespace, "G2C_BLACK_MARKET_COOLDOWN_INFO", "ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_black_market_cooldown_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO(); });
    external_message_g2c_black_market_cooldown_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO>(new ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO()); });
    external_message_g2c_black_market_cooldown_info_type["nCooldownInfoCount"] = sol::property([](const ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO& self) { return self.nCooldownInfoCount; }, [](ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO& self, decltype(ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO::nCooldownInfoCount) value) { self.nCooldownInfoCount = value; });
    external_message_g2c_black_market_cooldown_info_type["CooldownInfo"] = cqyh::lua::array_property(&ExternalMessage::G2C_BLACK_MARKET_COOLDOWN_INFO::CooldownInfo);
    auto external_message_g2c_black_market_buy_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_BLACK_MARKET_BUY>(L, external_message_namespace, "G2C_BLACK_MARKET_BUY", "ExternalMessage::G2C_BLACK_MARKET_BUY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_black_market_buy_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_BLACK_MARKET_BUY(); });
    external_message_g2c_black_market_buy_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_BLACK_MARKET_BUY>(new ExternalMessage::G2C_BLACK_MARKET_BUY()); });
    external_message_g2c_black_market_buy_type["nBlackMarketProductID"] = sol::property([](const ExternalMessage::G2C_BLACK_MARKET_BUY& self) { return self.nBlackMarketProductID; }, [](ExternalMessage::G2C_BLACK_MARKET_BUY& self, decltype(ExternalMessage::G2C_BLACK_MARKET_BUY::nBlackMarketProductID) value) { self.nBlackMarketProductID = value; });
    external_message_g2c_black_market_buy_type["nCount"] = sol::property([](const ExternalMessage::G2C_BLACK_MARKET_BUY& self) { return self.nCount; }, [](ExternalMessage::G2C_BLACK_MARKET_BUY& self, decltype(ExternalMessage::G2C_BLACK_MARKET_BUY::nCount) value) { self.nCount = value; });
}

}
