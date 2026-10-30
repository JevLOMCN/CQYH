#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_inventory_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_equip_spell_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_EQUIP_SPELL>(L, external_message_namespace, "C2G_EQUIP_SPELL", "ExternalMessage::C2G_EQUIP_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_equip_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_EQUIP_SPELL(); });
    external_message_c2g_equip_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_EQUIP_SPELL>(new ExternalMessage::C2G_EQUIP_SPELL()); });
    external_message_c2g_equip_spell_type["bySlot"] = sol::property([](const ExternalMessage::C2G_EQUIP_SPELL& self) { return self.bySlot; }, [](ExternalMessage::C2G_EQUIP_SPELL& self, decltype(ExternalMessage::C2G_EQUIP_SPELL::bySlot) value) { self.bySlot = value; });
    external_message_c2g_equip_spell_type["wSpellID"] = sol::property([](const ExternalMessage::C2G_EQUIP_SPELL& self) { return self.wSpellID; }, [](ExternalMessage::C2G_EQUIP_SPELL& self, decltype(ExternalMessage::C2G_EQUIP_SPELL::wSpellID) value) { self.wSpellID = value; });
    auto external_message_c2g_use_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_USE_ITEM>(L, external_message_namespace, "C2G_USE_ITEM", "ExternalMessage::C2G_USE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_use_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_USE_ITEM(); });
    external_message_c2g_use_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_USE_ITEM>(new ExternalMessage::C2G_USE_ITEM()); });
    external_message_c2g_use_item_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_USE_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::C2G_USE_ITEM& self, decltype(ExternalMessage::C2G_USE_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_use_item_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_USE_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_USE_ITEM& self, decltype(ExternalMessage::C2G_USE_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_c2g_move_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_MOVE_ITEM>(L, external_message_namespace, "C2G_MOVE_ITEM", "ExternalMessage::C2G_MOVE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_move_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_MOVE_ITEM(); });
    external_message_c2g_move_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_MOVE_ITEM>(new ExternalMessage::C2G_MOVE_ITEM()); });
    external_message_c2g_move_item_type["bySrcConSlot"] = sol::property([](const ExternalMessage::C2G_MOVE_ITEM& self) { return self.bySrcConSlot; }, [](ExternalMessage::C2G_MOVE_ITEM& self, decltype(ExternalMessage::C2G_MOVE_ITEM::bySrcConSlot) value) { self.bySrcConSlot = value; });
    external_message_c2g_move_item_type["bySrcItemSlot"] = sol::property([](const ExternalMessage::C2G_MOVE_ITEM& self) { return self.bySrcItemSlot; }, [](ExternalMessage::C2G_MOVE_ITEM& self, decltype(ExternalMessage::C2G_MOVE_ITEM::bySrcItemSlot) value) { self.bySrcItemSlot = value; });
    external_message_c2g_move_item_type["byDstConSlot"] = sol::property([](const ExternalMessage::C2G_MOVE_ITEM& self) { return self.byDstConSlot; }, [](ExternalMessage::C2G_MOVE_ITEM& self, decltype(ExternalMessage::C2G_MOVE_ITEM::byDstConSlot) value) { self.byDstConSlot = value; });
    external_message_c2g_move_item_type["byDstItemSlot"] = sol::property([](const ExternalMessage::C2G_MOVE_ITEM& self) { return self.byDstItemSlot; }, [](ExternalMessage::C2G_MOVE_ITEM& self, decltype(ExternalMessage::C2G_MOVE_ITEM::byDstItemSlot) value) { self.byDstItemSlot = value; });
    auto external_message_c2g_split_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SPLIT_ITEM>(L, external_message_namespace, "C2G_SPLIT_ITEM", "ExternalMessage::C2G_SPLIT_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_split_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SPLIT_ITEM(); });
    external_message_c2g_split_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SPLIT_ITEM>(new ExternalMessage::C2G_SPLIT_ITEM()); });
    external_message_c2g_split_item_type["bySrcConSlot"] = sol::property([](const ExternalMessage::C2G_SPLIT_ITEM& self) { return self.bySrcConSlot; }, [](ExternalMessage::C2G_SPLIT_ITEM& self, decltype(ExternalMessage::C2G_SPLIT_ITEM::bySrcConSlot) value) { self.bySrcConSlot = value; });
    external_message_c2g_split_item_type["bySrcItemSlot"] = sol::property([](const ExternalMessage::C2G_SPLIT_ITEM& self) { return self.bySrcItemSlot; }, [](ExternalMessage::C2G_SPLIT_ITEM& self, decltype(ExternalMessage::C2G_SPLIT_ITEM::bySrcItemSlot) value) { self.bySrcItemSlot = value; });
    external_message_c2g_split_item_type["byDstConSlot"] = sol::property([](const ExternalMessage::C2G_SPLIT_ITEM& self) { return self.byDstConSlot; }, [](ExternalMessage::C2G_SPLIT_ITEM& self, decltype(ExternalMessage::C2G_SPLIT_ITEM::byDstConSlot) value) { self.byDstConSlot = value; });
    external_message_c2g_split_item_type["byDstItemSlot"] = sol::property([](const ExternalMessage::C2G_SPLIT_ITEM& self) { return self.byDstItemSlot; }, [](ExternalMessage::C2G_SPLIT_ITEM& self, decltype(ExternalMessage::C2G_SPLIT_ITEM::byDstItemSlot) value) { self.byDstItemSlot = value; });
    external_message_c2g_split_item_type["shItemCount"] = sol::property([](const ExternalMessage::C2G_SPLIT_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::C2G_SPLIT_ITEM& self, decltype(ExternalMessage::C2G_SPLIT_ITEM::shItemCount) value) { self.shItemCount = value; });
    auto external_message_c2g_delete_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DELETE_ITEM>(L, external_message_namespace, "C2G_DELETE_ITEM", "ExternalMessage::C2G_DELETE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_delete_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DELETE_ITEM(); });
    external_message_c2g_delete_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DELETE_ITEM>(new ExternalMessage::C2G_DELETE_ITEM()); });
    external_message_c2g_delete_item_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_DELETE_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::C2G_DELETE_ITEM& self, decltype(ExternalMessage::C2G_DELETE_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_delete_item_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_DELETE_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_DELETE_ITEM& self, decltype(ExternalMessage::C2G_DELETE_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    external_message_c2g_delete_item_type["shItemCount"] = sol::property([](const ExternalMessage::C2G_DELETE_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::C2G_DELETE_ITEM& self, decltype(ExternalMessage::C2G_DELETE_ITEM::shItemCount) value) { self.shItemCount = value; });
    auto external_message_c2g_pickup_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_PICKUP_ITEM>(L, external_message_namespace, "C2G_PICKUP_ITEM", "ExternalMessage::C2G_PICKUP_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_pickup_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_PICKUP_ITEM(); });
    external_message_c2g_pickup_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_PICKUP_ITEM>(new ExternalMessage::C2G_PICKUP_ITEM()); });
    external_message_c2g_pickup_item_type["dwDropID"] = sol::property([](const ExternalMessage::C2G_PICKUP_ITEM& self) { return self.dwDropID; }, [](ExternalMessage::C2G_PICKUP_ITEM& self, decltype(ExternalMessage::C2G_PICKUP_ITEM::dwDropID) value) { self.dwDropID = value; });
    external_message_c2g_pickup_item_type["byPickMode"] = sol::property([](const ExternalMessage::C2G_PICKUP_ITEM& self) { return self.byPickMode; }, [](ExternalMessage::C2G_PICKUP_ITEM& self, decltype(ExternalMessage::C2G_PICKUP_ITEM::byPickMode) value) { self.byPickMode = value; });
    auto external_message_c2g_sign_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SIGN_ITEM>(L, external_message_namespace, "C2G_SIGN_ITEM", "ExternalMessage::C2G_SIGN_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_sign_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SIGN_ITEM(); });
    external_message_c2g_sign_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SIGN_ITEM>(new ExternalMessage::C2G_SIGN_ITEM()); });
    external_message_c2g_sign_item_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_SIGN_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::C2G_SIGN_ITEM& self, decltype(ExternalMessage::C2G_SIGN_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_sign_item_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_SIGN_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_SIGN_ITEM& self, decltype(ExternalMessage::C2G_SIGN_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_c2g_buy_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BUY_ITEM>(L, external_message_namespace, "C2G_BUY_ITEM", "ExternalMessage::C2G_BUY_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_buy_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BUY_ITEM(); });
    external_message_c2g_buy_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BUY_ITEM>(new ExternalMessage::C2G_BUY_ITEM()); });
    external_message_c2g_buy_item_type["nShopID"] = sol::property([](const ExternalMessage::C2G_BUY_ITEM& self) { return self.nShopID; }, [](ExternalMessage::C2G_BUY_ITEM& self, decltype(ExternalMessage::C2G_BUY_ITEM::nShopID) value) { self.nShopID = value; });
    external_message_c2g_buy_item_type["nShopItemIndex"] = sol::property([](const ExternalMessage::C2G_BUY_ITEM& self) { return self.nShopItemIndex; }, [](ExternalMessage::C2G_BUY_ITEM& self, decltype(ExternalMessage::C2G_BUY_ITEM::nShopItemIndex) value) { self.nShopItemIndex = value; });
    external_message_c2g_buy_item_type["shItemCount"] = sol::property([](const ExternalMessage::C2G_BUY_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::C2G_BUY_ITEM& self, decltype(ExternalMessage::C2G_BUY_ITEM::shItemCount) value) { self.shItemCount = value; });
    auto external_message_c2g_sell_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SELL_ITEM>(L, external_message_namespace, "C2G_SELL_ITEM", "ExternalMessage::C2G_SELL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_sell_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SELL_ITEM(); });
    external_message_c2g_sell_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SELL_ITEM>(new ExternalMessage::C2G_SELL_ITEM()); });
    external_message_c2g_sell_item_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_SELL_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::C2G_SELL_ITEM& self, decltype(ExternalMessage::C2G_SELL_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_sell_item_type["byItemSlot"] = sol::property([](const ExternalMessage::C2G_SELL_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::C2G_SELL_ITEM& self, decltype(ExternalMessage::C2G_SELL_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    external_message_c2g_sell_item_type["shItemCount"] = sol::property([](const ExternalMessage::C2G_SELL_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::C2G_SELL_ITEM& self, decltype(ExternalMessage::C2G_SELL_ITEM::shItemCount) value) { self.shItemCount = value; });
    auto external_message_c2g_buy_container_slot_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BUY_CONTAINER_SLOT>(L, external_message_namespace, "C2G_BUY_CONTAINER_SLOT", "ExternalMessage::C2G_BUY_CONTAINER_SLOT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_buy_container_slot_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BUY_CONTAINER_SLOT(); });
    external_message_c2g_buy_container_slot_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BUY_CONTAINER_SLOT>(new ExternalMessage::C2G_BUY_CONTAINER_SLOT()); });
    external_message_c2g_buy_container_slot_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_BUY_CONTAINER_SLOT& self) { return self.byConSlot; }, [](ExternalMessage::C2G_BUY_CONTAINER_SLOT& self, decltype(ExternalMessage::C2G_BUY_CONTAINER_SLOT::byConSlot) value) { self.byConSlot = value; });
    external_message_c2g_buy_container_slot_type["byCount"] = sol::property([](const ExternalMessage::C2G_BUY_CONTAINER_SLOT& self) { return self.byCount; }, [](ExternalMessage::C2G_BUY_CONTAINER_SLOT& self, decltype(ExternalMessage::C2G_BUY_CONTAINER_SLOT::byCount) value) { self.byCount = value; });
    auto external_message_c2g_organize_bag_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ORGANIZE_BAG>(L, external_message_namespace, "C2G_ORGANIZE_BAG", "ExternalMessage::C2G_ORGANIZE_BAG", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_organize_bag_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ORGANIZE_BAG(); });
    external_message_c2g_organize_bag_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ORGANIZE_BAG>(new ExternalMessage::C2G_ORGANIZE_BAG()); });
    external_message_c2g_organize_bag_type["byConSlot"] = sol::property([](const ExternalMessage::C2G_ORGANIZE_BAG& self) { return self.byConSlot; }, [](ExternalMessage::C2G_ORGANIZE_BAG& self, decltype(ExternalMessage::C2G_ORGANIZE_BAG::byConSlot) value) { self.byConSlot = value; });
    auto external_message_c2g_sell_resource_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SELL_RESOURCE_ITEM>(L, external_message_namespace, "C2G_SELL_RESOURCE_ITEM", "ExternalMessage::C2G_SELL_RESOURCE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_sell_resource_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SELL_RESOURCE_ITEM(); });
    external_message_c2g_sell_resource_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SELL_RESOURCE_ITEM>(new ExternalMessage::C2G_SELL_RESOURCE_ITEM()); });
}

}
