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

void register_external_game_to_client_team_auction_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_team_auction_add_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM>(L, external_message_namespace, "G2C_TEAM_AUCTION_ADD_ITEM", "ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_team_auction_add_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM(); });
    external_message_g2c_team_auction_add_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM>(new ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM()); });
    external_message_g2c_team_auction_add_item_type["dwAuctionIndex"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM& self) { return self.dwAuctionIndex; }, [](ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM::dwAuctionIndex) value) { self.dwAuctionIndex = value; });
    external_message_g2c_team_auction_add_item_type["acItemInfo"] = cqyh::lua::string_property(&ExternalMessage::G2C_TEAM_AUCTION_ADD_ITEM::acItemInfo);
    auto external_message_g2c_team_auction_begin_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TEAM_AUCTION_BEGIN>(L, external_message_namespace, "G2C_TEAM_AUCTION_BEGIN", "ExternalMessage::G2C_TEAM_AUCTION_BEGIN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_team_auction_begin_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TEAM_AUCTION_BEGIN(); });
    external_message_g2c_team_auction_begin_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TEAM_AUCTION_BEGIN>(new ExternalMessage::G2C_TEAM_AUCTION_BEGIN()); });
    external_message_g2c_team_auction_begin_type["dwAuctionIndex"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self) { return self.dwAuctionIndex; }, [](ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_BEGIN::dwAuctionIndex) value) { self.dwAuctionIndex = value; });
    external_message_g2c_team_auction_begin_type["dwBottomPrice"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self) { return self.dwBottomPrice; }, [](ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_BEGIN::dwBottomPrice) value) { self.dwBottomPrice = value; });
    external_message_g2c_team_auction_begin_type["byBidderCount"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self) { return self.byBidderCount; }, [](ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_BEGIN::byBidderCount) value) { self.byBidderCount = value; });
    external_message_g2c_team_auction_begin_type["dwRemainTime"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self) { return self.dwRemainTime; }, [](ExternalMessage::G2C_TEAM_AUCTION_BEGIN& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_BEGIN::dwRemainTime) value) { self.dwRemainTime = value; });
    auto external_message_g2c_team_auction_notify_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TEAM_AUCTION_NOTIFY>(L, external_message_namespace, "G2C_TEAM_AUCTION_NOTIFY", "ExternalMessage::G2C_TEAM_AUCTION_NOTIFY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_team_auction_notify_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TEAM_AUCTION_NOTIFY(); });
    external_message_g2c_team_auction_notify_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TEAM_AUCTION_NOTIFY>(new ExternalMessage::G2C_TEAM_AUCTION_NOTIFY()); });
    external_message_g2c_team_auction_notify_type["dwAuctionIndex"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self) { return self.dwAuctionIndex; }, [](ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_NOTIFY::dwAuctionIndex) value) { self.dwAuctionIndex = value; });
    external_message_g2c_team_auction_notify_type["dwBidderRoleID"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self) { return self.dwBidderRoleID; }, [](ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_NOTIFY::dwBidderRoleID) value) { self.dwBidderRoleID = value; });
    external_message_g2c_team_auction_notify_type["dwOfferPrice"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self) { return self.dwOfferPrice; }, [](ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_NOTIFY::dwOfferPrice) value) { self.dwOfferPrice = value; });
    external_message_g2c_team_auction_notify_type["dwRemainTime"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self) { return self.dwRemainTime; }, [](ExternalMessage::G2C_TEAM_AUCTION_NOTIFY& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_NOTIFY::dwRemainTime) value) { self.dwRemainTime = value; });
    auto external_message_g2c_team_auction_abandon_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TEAM_AUCTION_ABANDON>(L, external_message_namespace, "G2C_TEAM_AUCTION_ABANDON", "ExternalMessage::G2C_TEAM_AUCTION_ABANDON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_team_auction_abandon_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TEAM_AUCTION_ABANDON(); });
    external_message_g2c_team_auction_abandon_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TEAM_AUCTION_ABANDON>(new ExternalMessage::G2C_TEAM_AUCTION_ABANDON()); });
    external_message_g2c_team_auction_abandon_type["dwAuctionIndex"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_ABANDON& self) { return self.dwAuctionIndex; }, [](ExternalMessage::G2C_TEAM_AUCTION_ABANDON& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_ABANDON::dwAuctionIndex) value) { self.dwAuctionIndex = value; });
    external_message_g2c_team_auction_abandon_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_ABANDON& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_TEAM_AUCTION_ABANDON& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_ABANDON::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_g2c_team_auction_end_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_TEAM_AUCTION_END>(L, external_message_namespace, "G2C_TEAM_AUCTION_END", "ExternalMessage::G2C_TEAM_AUCTION_END", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_team_auction_end_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_TEAM_AUCTION_END(); });
    external_message_g2c_team_auction_end_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_TEAM_AUCTION_END>(new ExternalMessage::G2C_TEAM_AUCTION_END()); });
    external_message_g2c_team_auction_end_type["dwAuctionIndex"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_END& self) { return self.dwAuctionIndex; }, [](ExternalMessage::G2C_TEAM_AUCTION_END& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_END::dwAuctionIndex) value) { self.dwAuctionIndex = value; });
    external_message_g2c_team_auction_end_type["dwWinnerRoleID"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_END& self) { return self.dwWinnerRoleID; }, [](ExternalMessage::G2C_TEAM_AUCTION_END& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_END::dwWinnerRoleID) value) { self.dwWinnerRoleID = value; });
    external_message_g2c_team_auction_end_type["dwTax"] = sol::property([](const ExternalMessage::G2C_TEAM_AUCTION_END& self) { return self.dwTax; }, [](ExternalMessage::G2C_TEAM_AUCTION_END& self, decltype(ExternalMessage::G2C_TEAM_AUCTION_END::dwTax) value) { self.dwTax = value; });
}

}
