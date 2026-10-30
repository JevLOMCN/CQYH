#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_trade_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_trade_ask_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_ASK>(L, external_message_namespace, "C2G_TRADE_ASK", "ExternalMessage::C2G_TRADE_ASK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_ask_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_ASK(); });
    external_message_c2g_trade_ask_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_ASK>(new ExternalMessage::C2G_TRADE_ASK()); });
    external_message_c2g_trade_ask_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_TRADE_ASK& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_TRADE_ASK& self, decltype(ExternalMessage::C2G_TRADE_ASK::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2g_trade_accept_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_ACCEPT>(L, external_message_namespace, "C2G_TRADE_ACCEPT", "ExternalMessage::C2G_TRADE_ACCEPT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_accept_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_ACCEPT(); });
    external_message_c2g_trade_accept_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_ACCEPT>(new ExternalMessage::C2G_TRADE_ACCEPT()); });
    external_message_c2g_trade_accept_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_TRADE_ACCEPT& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_TRADE_ACCEPT& self, decltype(ExternalMessage::C2G_TRADE_ACCEPT::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2g_trade_set_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_SET_ITEM>(L, external_message_namespace, "C2G_TRADE_SET_ITEM", "ExternalMessage::C2G_TRADE_SET_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_set_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_SET_ITEM(); });
    external_message_c2g_trade_set_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_SET_ITEM>(new ExternalMessage::C2G_TRADE_SET_ITEM()); });
    external_message_c2g_trade_set_item_type["byTradeSlot"] = sol::property([](const ExternalMessage::C2G_TRADE_SET_ITEM& self) { return self.byTradeSlot; }, [](ExternalMessage::C2G_TRADE_SET_ITEM& self, decltype(ExternalMessage::C2G_TRADE_SET_ITEM::byTradeSlot) value) { self.byTradeSlot = value; });
    external_message_c2g_trade_set_item_type["bAdd"] = sol::property([](const ExternalMessage::C2G_TRADE_SET_ITEM& self) { return self.bAdd; }, [](ExternalMessage::C2G_TRADE_SET_ITEM& self, decltype(ExternalMessage::C2G_TRADE_SET_ITEM::bAdd) value) { self.bAdd = value; });
    external_message_c2g_trade_set_item_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_TRADE_SET_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::C2G_TRADE_SET_ITEM& self, decltype(ExternalMessage::C2G_TRADE_SET_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_trade_set_item_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_TRADE_SET_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_TRADE_SET_ITEM& self, decltype(ExternalMessage::C2G_TRADE_SET_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_c2g_trade_lock_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_LOCK>(L, external_message_namespace, "C2G_TRADE_LOCK", "ExternalMessage::C2G_TRADE_LOCK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_lock_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_LOCK(); });
    external_message_c2g_trade_lock_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_LOCK>(new ExternalMessage::C2G_TRADE_LOCK()); });
    auto external_message_c2g_trade_confirm_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_CONFIRM>(L, external_message_namespace, "C2G_TRADE_CONFIRM", "ExternalMessage::C2G_TRADE_CONFIRM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_confirm_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_CONFIRM(); });
    external_message_c2g_trade_confirm_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_CONFIRM>(new ExternalMessage::C2G_TRADE_CONFIRM()); });
    auto external_message_c2g_trade_unlock_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_UNLOCK>(L, external_message_namespace, "C2G_TRADE_UNLOCK", "ExternalMessage::C2G_TRADE_UNLOCK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_unlock_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_UNLOCK(); });
    external_message_c2g_trade_unlock_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_UNLOCK>(new ExternalMessage::C2G_TRADE_UNLOCK()); });
    auto external_message_c2g_trade_cancel_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TRADE_CANCEL>(L, external_message_namespace, "C2G_TRADE_CANCEL", "ExternalMessage::C2G_TRADE_CANCEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_trade_cancel_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TRADE_CANCEL(); });
    external_message_c2g_trade_cancel_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TRADE_CANCEL>(new ExternalMessage::C2G_TRADE_CANCEL()); });
}

}
