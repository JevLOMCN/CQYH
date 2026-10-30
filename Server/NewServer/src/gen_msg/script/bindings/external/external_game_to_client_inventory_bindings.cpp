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

void register_external_game_to_client_inventory_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_role_learn_item_spell_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL>(L, external_message_namespace, "G2C_ROLE_LEARN_ITEM_SPELL", "ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_role_learn_item_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL(); });
    external_message_g2c_role_learn_item_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL>(new ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL()); });
    external_message_g2c_role_learn_item_spell_type["wSpellID"] = sol::property([](const ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL& self) { return self.wSpellID; }, [](ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL& self, decltype(ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL::wSpellID) value) { self.wSpellID = value; });
    external_message_g2c_role_learn_item_spell_type["bySpellLevel"] = sol::property([](const ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL& self) { return self.bySpellLevel; }, [](ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL& self, decltype(ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL::bySpellLevel) value) { self.bySpellLevel = value; });
    external_message_g2c_role_learn_item_spell_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL& self, decltype(ExternalMessage::G2C_ROLE_LEARN_ITEM_SPELL::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_g2c_role_remove_item_spell_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL>(L, external_message_namespace, "G2C_ROLE_REMOVE_ITEM_SPELL", "ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_role_remove_item_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL(); });
    external_message_g2c_role_remove_item_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL>(new ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL()); });
    external_message_g2c_role_remove_item_spell_type["wSpellID"] = sol::property([](const ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL& self) { return self.wSpellID; }, [](ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL& self, decltype(ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL::wSpellID) value) { self.wSpellID = value; });
    external_message_g2c_role_remove_item_spell_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL& self, decltype(ExternalMessage::G2C_ROLE_REMOVE_ITEM_SPELL::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_g2c_equip_spell_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_EQUIP_SPELL>(L, external_message_namespace, "G2C_EQUIP_SPELL", "ExternalMessage::G2C_EQUIP_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_equip_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_EQUIP_SPELL(); });
    external_message_g2c_equip_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_EQUIP_SPELL>(new ExternalMessage::G2C_EQUIP_SPELL()); });
    external_message_g2c_equip_spell_type["bySlot"] = sol::property([](const ExternalMessage::G2C_EQUIP_SPELL& self) { return self.bySlot; }, [](ExternalMessage::G2C_EQUIP_SPELL& self, decltype(ExternalMessage::G2C_EQUIP_SPELL::bySlot) value) { self.bySlot = value; });
    external_message_g2c_equip_spell_type["wSpellID"] = sol::property([](const ExternalMessage::G2C_EQUIP_SPELL& self) { return self.wSpellID; }, [](ExternalMessage::G2C_EQUIP_SPELL& self, decltype(ExternalMessage::G2C_EQUIP_SPELL::wSpellID) value) { self.wSpellID = value; });
    external_message_g2c_equip_spell_type["byRuneSlot"] = sol::property([](const ExternalMessage::G2C_EQUIP_SPELL& self) { return self.byRuneSlot; }, [](ExternalMessage::G2C_EQUIP_SPELL& self, decltype(ExternalMessage::G2C_EQUIP_SPELL::byRuneSlot) value) { self.byRuneSlot = value; });
    external_message_g2c_equip_spell_type["bySpellLevel"] = sol::property([](const ExternalMessage::G2C_EQUIP_SPELL& self) { return self.bySpellLevel; }, [](ExternalMessage::G2C_EQUIP_SPELL& self, decltype(ExternalMessage::G2C_EQUIP_SPELL::bySpellLevel) value) { self.bySpellLevel = value; });
    auto external_message_g2c_sync_container_sizes_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_CONTAINER_SIZES>(L, external_message_namespace, "G2C_SYNC_CONTAINER_SIZES", "ExternalMessage::G2C_SYNC_CONTAINER_SIZES", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_container_sizes_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_CONTAINER_SIZES(); });
    external_message_g2c_sync_container_sizes_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_CONTAINER_SIZES>(new ExternalMessage::G2C_SYNC_CONTAINER_SIZES()); });
    external_message_g2c_sync_container_sizes_type["abySizes"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_CONTAINER_SIZES::abySizes);
    auto external_message_g2c_sync_role_equip_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA>(L, external_message_namespace, "G2C_SYNC_ROLE_EQUIP_DATA", "ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_role_equip_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA(); });
    external_message_g2c_sync_role_equip_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA>(new ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA()); });
    external_message_g2c_sync_role_equip_data_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_role_equip_data_type["awRefineLevels"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA::awRefineLevels);
    external_message_g2c_sync_role_equip_data_type["byItemCount"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA& self) { return self.byItemCount; }, [](ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA::byItemCount) value) { self.byItemCount = value; });
    external_message_g2c_sync_role_equip_data_type["acItems"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_ROLE_EQUIP_DATA::acItems);
    auto external_message_g2c_sync_item_bag_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ITEM_BAG>(L, external_message_namespace, "G2C_SYNC_ITEM_BAG", "ExternalMessage::G2C_SYNC_ITEM_BAG", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_item_bag_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ITEM_BAG(); });
    external_message_g2c_sync_item_bag_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ITEM_BAG>(new ExternalMessage::G2C_SYNC_ITEM_BAG()); });
    external_message_g2c_sync_item_bag_type["byShowHat"] = sol::property([](const ExternalMessage::G2C_SYNC_ITEM_BAG& self) { return self.byShowHat; }, [](ExternalMessage::G2C_SYNC_ITEM_BAG& self, decltype(ExternalMessage::G2C_SYNC_ITEM_BAG::byShowHat) value) { self.byShowHat = value; });
    external_message_g2c_sync_item_bag_type["byItemCount"] = sol::property([](const ExternalMessage::G2C_SYNC_ITEM_BAG& self) { return self.byItemCount; }, [](ExternalMessage::G2C_SYNC_ITEM_BAG& self, decltype(ExternalMessage::G2C_SYNC_ITEM_BAG::byItemCount) value) { self.byItemCount = value; });
    external_message_g2c_sync_item_bag_type["acItems"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_ITEM_BAG::acItems);
    auto external_message_g2c_item_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ITEM_CHANGE>(L, external_message_namespace, "G2C_ITEM_CHANGE", "ExternalMessage::G2C_ITEM_CHANGE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_item_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ITEM_CHANGE(); });
    external_message_g2c_item_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ITEM_CHANGE>(new ExternalMessage::G2C_ITEM_CHANGE()); });
    external_message_g2c_item_change_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::G2C_ITEM_CHANGE::acItem);
    auto external_message_g2c_sign_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SIGN_ITEM>(L, external_message_namespace, "G2C_SIGN_ITEM", "ExternalMessage::G2C_SIGN_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sign_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SIGN_ITEM(); });
    external_message_g2c_sign_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SIGN_ITEM>(new ExternalMessage::G2C_SIGN_ITEM()); });
    external_message_g2c_sign_item_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_SIGN_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::G2C_SIGN_ITEM& self, decltype(ExternalMessage::G2C_SIGN_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_sign_item_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_SIGN_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_SIGN_ITEM& self, decltype(ExternalMessage::G2C_SIGN_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    external_message_g2c_sign_item_type["bySign"] = cqyh::lua::array_property(&ExternalMessage::G2C_SIGN_ITEM::bySign);
    auto external_message_g2c_item_swap_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ITEM_SWAP>(L, external_message_namespace, "G2C_ITEM_SWAP", "ExternalMessage::G2C_ITEM_SWAP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_item_swap_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ITEM_SWAP(); });
    external_message_g2c_item_swap_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ITEM_SWAP>(new ExternalMessage::G2C_ITEM_SWAP()); });
    external_message_g2c_item_swap_type["bySrcConSlot"] = sol::property([](const ExternalMessage::G2C_ITEM_SWAP& self) { return self.bySrcConSlot; }, [](ExternalMessage::G2C_ITEM_SWAP& self, decltype(ExternalMessage::G2C_ITEM_SWAP::bySrcConSlot) value) { self.bySrcConSlot = value; });
    external_message_g2c_item_swap_type["bySrcItemSlot"] = sol::property([](const ExternalMessage::G2C_ITEM_SWAP& self) { return self.bySrcItemSlot; }, [](ExternalMessage::G2C_ITEM_SWAP& self, decltype(ExternalMessage::G2C_ITEM_SWAP::bySrcItemSlot) value) { self.bySrcItemSlot = value; });
    external_message_g2c_item_swap_type["byDstConSlot"] = sol::property([](const ExternalMessage::G2C_ITEM_SWAP& self) { return self.byDstConSlot; }, [](ExternalMessage::G2C_ITEM_SWAP& self, decltype(ExternalMessage::G2C_ITEM_SWAP::byDstConSlot) value) { self.byDstConSlot = value; });
    external_message_g2c_item_swap_type["byDstItemSlot"] = sol::property([](const ExternalMessage::G2C_ITEM_SWAP& self) { return self.byDstItemSlot; }, [](ExternalMessage::G2C_ITEM_SWAP& self, decltype(ExternalMessage::G2C_ITEM_SWAP::byDstItemSlot) value) { self.byDstItemSlot = value; });
    auto external_message_g2c_item_delete_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ITEM_DELETE>(L, external_message_namespace, "G2C_ITEM_DELETE", "ExternalMessage::G2C_ITEM_DELETE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_item_delete_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ITEM_DELETE(); });
    external_message_g2c_item_delete_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ITEM_DELETE>(new ExternalMessage::G2C_ITEM_DELETE()); });
    external_message_g2c_item_delete_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_ITEM_DELETE& self) { return self.byConSlot; }, [](ExternalMessage::G2C_ITEM_DELETE& self, decltype(ExternalMessage::G2C_ITEM_DELETE::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_item_delete_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_ITEM_DELETE& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_ITEM_DELETE& self, decltype(ExternalMessage::G2C_ITEM_DELETE::byItemSlot) value) { self.byItemSlot = value; });
    auto external_message_g2c_destroy_death_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DESTROY_DEATH_ITEM>(L, external_message_namespace, "G2C_DESTROY_DEATH_ITEM", "ExternalMessage::G2C_DESTROY_DEATH_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_destroy_death_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DESTROY_DEATH_ITEM(); });
    external_message_g2c_destroy_death_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DESTROY_DEATH_ITEM>(new ExternalMessage::G2C_DESTROY_DEATH_ITEM()); });
    external_message_g2c_destroy_death_item_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::G2C_DESTROY_DEATH_ITEM::acItem);
    auto external_message_g2c_lose_pk_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_LOSE_PK_ITEM>(L, external_message_namespace, "G2C_LOSE_PK_ITEM", "ExternalMessage::G2C_LOSE_PK_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_lose_pk_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_LOSE_PK_ITEM(); });
    external_message_g2c_lose_pk_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_LOSE_PK_ITEM>(new ExternalMessage::G2C_LOSE_PK_ITEM()); });
    external_message_g2c_lose_pk_item_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::G2C_LOSE_PK_ITEM::acItem);
    auto external_message_g2c_pickup_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_PICKUP_ITEM>(L, external_message_namespace, "G2C_PICKUP_ITEM", "ExternalMessage::G2C_PICKUP_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_pickup_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_PICKUP_ITEM(); });
    external_message_g2c_pickup_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_PICKUP_ITEM>(new ExternalMessage::G2C_PICKUP_ITEM()); });
    external_message_g2c_pickup_item_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_PICKUP_ITEM& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_PICKUP_ITEM& self, decltype(ExternalMessage::G2C_PICKUP_ITEM::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_pickup_item_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::G2C_PICKUP_ITEM::acItem);
    auto external_message_g2c_container_size_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CONTAINER_SIZE_CHANGE>(L, external_message_namespace, "G2C_CONTAINER_SIZE_CHANGE", "ExternalMessage::G2C_CONTAINER_SIZE_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_container_size_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CONTAINER_SIZE_CHANGE(); });
    external_message_g2c_container_size_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CONTAINER_SIZE_CHANGE>(new ExternalMessage::G2C_CONTAINER_SIZE_CHANGE()); });
    external_message_g2c_container_size_change_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_CONTAINER_SIZE_CHANGE& self) { return self.byConSlot; }, [](ExternalMessage::G2C_CONTAINER_SIZE_CHANGE& self, decltype(ExternalMessage::G2C_CONTAINER_SIZE_CHANGE::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_container_size_change_type["byConSize"] = sol::property([](const ExternalMessage::G2C_CONTAINER_SIZE_CHANGE& self) { return self.byConSize; }, [](ExternalMessage::G2C_CONTAINER_SIZE_CHANGE& self, decltype(ExternalMessage::G2C_CONTAINER_SIZE_CHANGE::byConSize) value) { self.byConSize = value; });
    auto external_message_g2c_equip_dur_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_EQUIP_DUR_CHANGE>(L, external_message_namespace, "G2C_EQUIP_DUR_CHANGE", "ExternalMessage::G2C_EQUIP_DUR_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_equip_dur_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_EQUIP_DUR_CHANGE(); });
    external_message_g2c_equip_dur_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_EQUIP_DUR_CHANGE>(new ExternalMessage::G2C_EQUIP_DUR_CHANGE()); });
    external_message_g2c_equip_dur_change_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_EQUIP_DUR_CHANGE& self) { return self.byConSlot; }, [](ExternalMessage::G2C_EQUIP_DUR_CHANGE& self, decltype(ExternalMessage::G2C_EQUIP_DUR_CHANGE::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_equip_dur_change_type["byEquipSlot"] = sol::property([](const ExternalMessage::G2C_EQUIP_DUR_CHANGE& self) { return self.byEquipSlot; }, [](ExternalMessage::G2C_EQUIP_DUR_CHANGE& self, decltype(ExternalMessage::G2C_EQUIP_DUR_CHANGE::byEquipSlot) value) { self.byEquipSlot = value; });
    external_message_g2c_equip_dur_change_type["nDur"] = sol::property([](const ExternalMessage::G2C_EQUIP_DUR_CHANGE& self) { return self.nDur; }, [](ExternalMessage::G2C_EQUIP_DUR_CHANGE& self, decltype(ExternalMessage::G2C_EQUIP_DUR_CHANGE::nDur) value) { self.nDur = value; });
    auto external_message_g2c_sell_resource_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SELL_RESOURCE_ITEM>(L, external_message_namespace, "G2C_SELL_RESOURCE_ITEM", "ExternalMessage::G2C_SELL_RESOURCE_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sell_resource_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SELL_RESOURCE_ITEM(); });
    external_message_g2c_sell_resource_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SELL_RESOURCE_ITEM>(new ExternalMessage::G2C_SELL_RESOURCE_ITEM()); });
    external_message_g2c_sell_resource_item_type["nSoldCount"] = sol::property([](const ExternalMessage::G2C_SELL_RESOURCE_ITEM& self) { return self.nSoldCount; }, [](ExternalMessage::G2C_SELL_RESOURCE_ITEM& self, decltype(ExternalMessage::G2C_SELL_RESOURCE_ITEM::nSoldCount) value) { self.nSoldCount = value; });
    external_message_g2c_sell_resource_item_type["itemslots"] = cqyh::lua::array_property(&ExternalMessage::G2C_SELL_RESOURCE_ITEM::itemslots);
}

}
