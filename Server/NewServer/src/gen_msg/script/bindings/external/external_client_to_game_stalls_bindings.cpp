#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_stalls_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_open_stall_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_OPEN_STALL>(L, external_message_namespace, "C2G_OPEN_STALL", "ExternalMessage::C2G_OPEN_STALL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_open_stall_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_OPEN_STALL(); });
    external_message_c2g_open_stall_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_OPEN_STALL>(new ExternalMessage::C2G_OPEN_STALL()); });
    auto external_message_c2g_close_stall_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CLOSE_STALL>(L, external_message_namespace, "C2G_CLOSE_STALL", "ExternalMessage::C2G_CLOSE_STALL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_close_stall_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CLOSE_STALL(); });
    external_message_c2g_close_stall_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CLOSE_STALL>(new ExternalMessage::C2G_CLOSE_STALL()); });
    auto external_message_c2g_real_open_stall_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_REAL_OPEN_STALL>(L, external_message_namespace, "C2G_REAL_OPEN_STALL", "ExternalMessage::C2G_REAL_OPEN_STALL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_real_open_stall_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_REAL_OPEN_STALL(); });
    external_message_c2g_real_open_stall_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_REAL_OPEN_STALL>(new ExternalMessage::C2G_REAL_OPEN_STALL()); });
    auto external_message_c2g_real_close_stall_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_REAL_CLOSE_STALL>(L, external_message_namespace, "C2G_REAL_CLOSE_STALL", "ExternalMessage::C2G_REAL_CLOSE_STALL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_real_close_stall_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_REAL_CLOSE_STALL(); });
    external_message_c2g_real_close_stall_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_REAL_CLOSE_STALL>(new ExternalMessage::C2G_REAL_CLOSE_STALL()); });
    auto external_message_c2g_add_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ADD_STALL_ITEM>(L, external_message_namespace, "C2G_ADD_STALL_ITEM", "ExternalMessage::C2G_ADD_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_add_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ADD_STALL_ITEM(); });
    external_message_c2g_add_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ADD_STALL_ITEM>(new ExternalMessage::C2G_ADD_STALL_ITEM()); });
    external_message_c2g_add_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::C2G_ADD_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::C2G_ADD_STALL_ITEM& self, decltype(ExternalMessage::C2G_ADD_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    external_message_c2g_add_stall_item_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_ADD_STALL_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::C2G_ADD_STALL_ITEM& self, decltype(ExternalMessage::C2G_ADD_STALL_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_add_stall_item_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_ADD_STALL_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_ADD_STALL_ITEM& self, decltype(ExternalMessage::C2G_ADD_STALL_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    external_message_c2g_add_stall_item_type["shItemCount"] = sol::property([](const ExternalMessage::C2G_ADD_STALL_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::C2G_ADD_STALL_ITEM& self, decltype(ExternalMessage::C2G_ADD_STALL_ITEM::shItemCount) value) { self.shItemCount = value; });
    external_message_c2g_add_stall_item_type["nPrice"] = sol::property([](const ExternalMessage::C2G_ADD_STALL_ITEM& self) { return self.nPrice; }, [](ExternalMessage::C2G_ADD_STALL_ITEM& self, decltype(ExternalMessage::C2G_ADD_STALL_ITEM::nPrice) value) { self.nPrice = value; });
    auto external_message_c2g_remove_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_REMOVE_STALL_ITEM>(L, external_message_namespace, "C2G_REMOVE_STALL_ITEM", "ExternalMessage::C2G_REMOVE_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_remove_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_REMOVE_STALL_ITEM(); });
    external_message_c2g_remove_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_REMOVE_STALL_ITEM>(new ExternalMessage::C2G_REMOVE_STALL_ITEM()); });
    external_message_c2g_remove_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::C2G_REMOVE_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::C2G_REMOVE_STALL_ITEM& self, decltype(ExternalMessage::C2G_REMOVE_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    auto external_message_c2g_change_stall_name_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHANGE_STALL_NAME>(L, external_message_namespace, "C2G_CHANGE_STALL_NAME", "ExternalMessage::C2G_CHANGE_STALL_NAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_change_stall_name_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHANGE_STALL_NAME(); });
    external_message_c2g_change_stall_name_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHANGE_STALL_NAME>(new ExternalMessage::C2G_CHANGE_STALL_NAME()); });
    external_message_c2g_change_stall_name_type["szStallName"] = cqyh::lua::string_property(&ExternalMessage::C2G_CHANGE_STALL_NAME::szStallName);
    auto external_message_c2g_upgrade_stall_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_UPGRADE_STALL>(L, external_message_namespace, "C2G_UPGRADE_STALL", "ExternalMessage::C2G_UPGRADE_STALL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_upgrade_stall_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_UPGRADE_STALL(); });
    external_message_c2g_upgrade_stall_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_UPGRADE_STALL>(new ExternalMessage::C2G_UPGRADE_STALL()); });
    external_message_c2g_upgrade_stall_type["btUpgradeCostType"] = sol::property([](const ExternalMessage::C2G_UPGRADE_STALL& self) { return self.btUpgradeCostType; }, [](ExternalMessage::C2G_UPGRADE_STALL& self, decltype(ExternalMessage::C2G_UPGRADE_STALL::btUpgradeCostType) value) { self.btUpgradeCostType = value; });
    auto external_message_c2g_view_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_VIEW_STALL_ITEM>(L, external_message_namespace, "C2G_VIEW_STALL_ITEM", "ExternalMessage::C2G_VIEW_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_view_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_VIEW_STALL_ITEM(); });
    external_message_c2g_view_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_VIEW_STALL_ITEM>(new ExternalMessage::C2G_VIEW_STALL_ITEM()); });
    external_message_c2g_view_stall_item_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_VIEW_STALL_ITEM& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_VIEW_STALL_ITEM& self, decltype(ExternalMessage::C2G_VIEW_STALL_ITEM::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2g_buy_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BUY_STALL_ITEM>(L, external_message_namespace, "C2G_BUY_STALL_ITEM", "ExternalMessage::C2G_BUY_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_buy_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BUY_STALL_ITEM(); });
    external_message_c2g_buy_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BUY_STALL_ITEM>(new ExternalMessage::C2G_BUY_STALL_ITEM()); });
    external_message_c2g_buy_stall_item_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_BUY_STALL_ITEM& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_BUY_STALL_ITEM& self, decltype(ExternalMessage::C2G_BUY_STALL_ITEM::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2g_buy_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::C2G_BUY_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::C2G_BUY_STALL_ITEM& self, decltype(ExternalMessage::C2G_BUY_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    external_message_c2g_buy_stall_item_type["shItemCount"] = sol::property([](const ExternalMessage::C2G_BUY_STALL_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::C2G_BUY_STALL_ITEM& self, decltype(ExternalMessage::C2G_BUY_STALL_ITEM::shItemCount) value) { self.shItemCount = value; });
    external_message_c2g_buy_stall_item_type["llModfiyFrame"] = sol::property([](const ExternalMessage::C2G_BUY_STALL_ITEM& self) { return self.llModfiyFrame; }, [](ExternalMessage::C2G_BUY_STALL_ITEM& self, decltype(ExternalMessage::C2G_BUY_STALL_ITEM::llModfiyFrame) value) { self.llModfiyFrame = value; });
}

}
