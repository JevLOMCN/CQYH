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

void register_external_game_to_client_stalls_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_stall_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_STALL>(L, external_message_namespace, "G2C_SYNC_STALL", "ExternalMessage::G2C_SYNC_STALL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_stall_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_STALL(); });
    external_message_g2c_sync_stall_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_STALL>(new ExternalMessage::G2C_SYNC_STALL()); });
    external_message_g2c_sync_stall_type["dwUpgradeExpireTime"] = sol::property([](const ExternalMessage::G2C_SYNC_STALL& self) { return self.dwUpgradeExpireTime; }, [](ExternalMessage::G2C_SYNC_STALL& self, decltype(ExternalMessage::G2C_SYNC_STALL::dwUpgradeExpireTime) value) { self.dwUpgradeExpireTime = value; });
    auto external_message_g2c_stall_state_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_STALL_STATE_CHANGE>(L, external_message_namespace, "G2C_STALL_STATE_CHANGE", "ExternalMessage::G2C_STALL_STATE_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_stall_state_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_STALL_STATE_CHANGE(); });
    external_message_g2c_stall_state_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_STALL_STATE_CHANGE>(new ExternalMessage::G2C_STALL_STATE_CHANGE()); });
    external_message_g2c_stall_state_change_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_STALL_STATE_CHANGE& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_STALL_STATE_CHANGE& self, decltype(ExternalMessage::G2C_STALL_STATE_CHANGE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_stall_state_change_type["btStallState"] = sol::property([](const ExternalMessage::G2C_STALL_STATE_CHANGE& self) { return self.btStallState; }, [](ExternalMessage::G2C_STALL_STATE_CHANGE& self, decltype(ExternalMessage::G2C_STALL_STATE_CHANGE::btStallState) value) { self.btStallState = value; });
    auto external_message_g2c_add_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ADD_STALL_ITEM>(L, external_message_namespace, "G2C_ADD_STALL_ITEM", "ExternalMessage::G2C_ADD_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_add_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ADD_STALL_ITEM(); });
    external_message_g2c_add_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ADD_STALL_ITEM>(new ExternalMessage::G2C_ADD_STALL_ITEM()); });
    external_message_g2c_add_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::G2C_ADD_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::G2C_ADD_STALL_ITEM& self, decltype(ExternalMessage::G2C_ADD_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    external_message_g2c_add_stall_item_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_ADD_STALL_ITEM& self) { return self.byConSlot; }, [](ExternalMessage::G2C_ADD_STALL_ITEM& self, decltype(ExternalMessage::G2C_ADD_STALL_ITEM::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_add_stall_item_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_ADD_STALL_ITEM& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_ADD_STALL_ITEM& self, decltype(ExternalMessage::G2C_ADD_STALL_ITEM::byItemSlot) value) { self.byItemSlot = value; });
    external_message_g2c_add_stall_item_type["shItemCount"] = sol::property([](const ExternalMessage::G2C_ADD_STALL_ITEM& self) { return self.shItemCount; }, [](ExternalMessage::G2C_ADD_STALL_ITEM& self, decltype(ExternalMessage::G2C_ADD_STALL_ITEM::shItemCount) value) { self.shItemCount = value; });
    external_message_g2c_add_stall_item_type["nPrice"] = sol::property([](const ExternalMessage::G2C_ADD_STALL_ITEM& self) { return self.nPrice; }, [](ExternalMessage::G2C_ADD_STALL_ITEM& self, decltype(ExternalMessage::G2C_ADD_STALL_ITEM::nPrice) value) { self.nPrice = value; });
    auto external_message_g2c_remove_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_REMOVE_STALL_ITEM>(L, external_message_namespace, "G2C_REMOVE_STALL_ITEM", "ExternalMessage::G2C_REMOVE_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_remove_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_REMOVE_STALL_ITEM(); });
    external_message_g2c_remove_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_REMOVE_STALL_ITEM>(new ExternalMessage::G2C_REMOVE_STALL_ITEM()); });
    external_message_g2c_remove_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::G2C_REMOVE_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::G2C_REMOVE_STALL_ITEM& self, decltype(ExternalMessage::G2C_REMOVE_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    auto external_message_g2c_change_stall_name_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHANGE_STALL_NAME>(L, external_message_namespace, "G2C_CHANGE_STALL_NAME", "ExternalMessage::G2C_CHANGE_STALL_NAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_change_stall_name_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHANGE_STALL_NAME(); });
    external_message_g2c_change_stall_name_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHANGE_STALL_NAME>(new ExternalMessage::G2C_CHANGE_STALL_NAME()); });
    external_message_g2c_change_stall_name_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_CHANGE_STALL_NAME& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_CHANGE_STALL_NAME& self, decltype(ExternalMessage::G2C_CHANGE_STALL_NAME::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_change_stall_name_type["szStallName"] = cqyh::lua::string_property(&ExternalMessage::G2C_CHANGE_STALL_NAME::szStallName);
    auto external_message_g2c_change_stall_level_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHANGE_STALL_LEVEL>(L, external_message_namespace, "G2C_CHANGE_STALL_LEVEL", "ExternalMessage::G2C_CHANGE_STALL_LEVEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_change_stall_level_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHANGE_STALL_LEVEL(); });
    external_message_g2c_change_stall_level_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHANGE_STALL_LEVEL>(new ExternalMessage::G2C_CHANGE_STALL_LEVEL()); });
    external_message_g2c_change_stall_level_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_CHANGE_STALL_LEVEL& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_CHANGE_STALL_LEVEL& self, decltype(ExternalMessage::G2C_CHANGE_STALL_LEVEL::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_change_stall_level_type["byStallLevel"] = sol::property([](const ExternalMessage::G2C_CHANGE_STALL_LEVEL& self) { return self.byStallLevel; }, [](ExternalMessage::G2C_CHANGE_STALL_LEVEL& self, decltype(ExternalMessage::G2C_CHANGE_STALL_LEVEL::byStallLevel) value) { self.byStallLevel = value; });
    auto external_message_stall_item_info_type = cqyh::gen_msg::usertype<ExternalMessage::STALL_ITEM_INFO>(L, external_message_namespace, "STALL_ITEM_INFO", "ExternalMessage::STALL_ITEM_INFO");
    external_message_stall_item_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::STALL_ITEM_INFO(); });
    external_message_stall_item_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::STALL_ITEM_INFO>(new ExternalMessage::STALL_ITEM_INFO()); });
    external_message_stall_item_info_type["btStallSlot"] = sol::property([](const ExternalMessage::STALL_ITEM_INFO& self) { return self.btStallSlot; }, [](ExternalMessage::STALL_ITEM_INFO& self, decltype(ExternalMessage::STALL_ITEM_INFO::btStallSlot) value) { self.btStallSlot = value; });
    external_message_stall_item_info_type["nPrice"] = sol::property([](const ExternalMessage::STALL_ITEM_INFO& self) { return self.nPrice; }, [](ExternalMessage::STALL_ITEM_INFO& self, decltype(ExternalMessage::STALL_ITEM_INFO::nPrice) value) { self.nPrice = value; });
    external_message_stall_item_info_type["llModifyFrame"] = sol::property([](const ExternalMessage::STALL_ITEM_INFO& self) { return self.llModifyFrame; }, [](ExternalMessage::STALL_ITEM_INFO& self, decltype(ExternalMessage::STALL_ITEM_INFO::llModifyFrame) value) { self.llModifyFrame = value; });
    external_message_stall_item_info_type["acItem"] = cqyh::lua::string_property(&ExternalMessage::STALL_ITEM_INFO::acItem);
    auto external_message_g2c_view_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_VIEW_STALL_ITEM>(L, external_message_namespace, "G2C_VIEW_STALL_ITEM", "ExternalMessage::G2C_VIEW_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_view_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_VIEW_STALL_ITEM(); });
    external_message_g2c_view_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_VIEW_STALL_ITEM>(new ExternalMessage::G2C_VIEW_STALL_ITEM()); });
    external_message_g2c_view_stall_item_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_VIEW_STALL_ITEM& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_VIEW_STALL_ITEM& self, decltype(ExternalMessage::G2C_VIEW_STALL_ITEM::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_view_stall_item_type["btStallItemCount"] = sol::property([](const ExternalMessage::G2C_VIEW_STALL_ITEM& self) { return self.btStallItemCount; }, [](ExternalMessage::G2C_VIEW_STALL_ITEM& self, decltype(ExternalMessage::G2C_VIEW_STALL_ITEM::btStallItemCount) value) { self.btStallItemCount = value; });
    external_message_g2c_view_stall_item_type["acStallItems"] = cqyh::lua::string_property(&ExternalMessage::G2C_VIEW_STALL_ITEM::acStallItems);
    auto external_message_g2c_buy_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_BUY_STALL_ITEM>(L, external_message_namespace, "G2C_BUY_STALL_ITEM", "ExternalMessage::G2C_BUY_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_buy_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_BUY_STALL_ITEM(); });
    external_message_g2c_buy_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_BUY_STALL_ITEM>(new ExternalMessage::G2C_BUY_STALL_ITEM()); });
    external_message_g2c_buy_stall_item_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_BUY_STALL_ITEM& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_BUY_STALL_ITEM& self, decltype(ExternalMessage::G2C_BUY_STALL_ITEM::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_buy_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::G2C_BUY_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::G2C_BUY_STALL_ITEM& self, decltype(ExternalMessage::G2C_BUY_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    external_message_g2c_buy_stall_item_type["nLeftItemCount"] = sol::property([](const ExternalMessage::G2C_BUY_STALL_ITEM& self) { return self.nLeftItemCount; }, [](ExternalMessage::G2C_BUY_STALL_ITEM& self, decltype(ExternalMessage::G2C_BUY_STALL_ITEM::nLeftItemCount) value) { self.nLeftItemCount = value; });
    auto external_message_g2c_sold_stall_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SOLD_STALL_ITEM>(L, external_message_namespace, "G2C_SOLD_STALL_ITEM", "ExternalMessage::G2C_SOLD_STALL_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sold_stall_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SOLD_STALL_ITEM(); });
    external_message_g2c_sold_stall_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SOLD_STALL_ITEM>(new ExternalMessage::G2C_SOLD_STALL_ITEM()); });
    external_message_g2c_sold_stall_item_type["btStallSlot"] = sol::property([](const ExternalMessage::G2C_SOLD_STALL_ITEM& self) { return self.btStallSlot; }, [](ExternalMessage::G2C_SOLD_STALL_ITEM& self, decltype(ExternalMessage::G2C_SOLD_STALL_ITEM::btStallSlot) value) { self.btStallSlot = value; });
    external_message_g2c_sold_stall_item_type["nItemCount"] = sol::property([](const ExternalMessage::G2C_SOLD_STALL_ITEM& self) { return self.nItemCount; }, [](ExternalMessage::G2C_SOLD_STALL_ITEM& self, decltype(ExternalMessage::G2C_SOLD_STALL_ITEM::nItemCount) value) { self.nItemCount = value; });
    external_message_g2c_sold_stall_item_type["nTotalIncome"] = sol::property([](const ExternalMessage::G2C_SOLD_STALL_ITEM& self) { return self.nTotalIncome; }, [](ExternalMessage::G2C_SOLD_STALL_ITEM& self, decltype(ExternalMessage::G2C_SOLD_STALL_ITEM::nTotalIncome) value) { self.nTotalIncome = value; });
}

}
