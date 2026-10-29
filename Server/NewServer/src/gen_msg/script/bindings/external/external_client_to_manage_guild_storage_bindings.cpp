#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_guild_storage_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_guild_change_storage_right_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT>(L, external_message_namespace, "C2M_GUILD_CHANGE_STORAGE_RIGHT", "ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_change_storage_right_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT(); });
    external_message_c2m_guild_change_storage_right_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT>(new ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT()); });
    external_message_c2m_guild_change_storage_right_type["byPosition"] = sol::property([](const ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT& self) { return self.byPosition; }, [](ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT& self, decltype(ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT::byPosition) value) { self.byPosition = value; });
    external_message_c2m_guild_change_storage_right_type["wRight"] = sol::property([](const ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT& self) { return self.wRight; }, [](ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT& self, decltype(ExternalMessage::C2M_GUILD_CHANGE_STORAGE_RIGHT::wRight) value) { self.wRight = value; });
    auto external_message_c2m_guild_move_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_MOVE_ITEM>(L, external_message_namespace, "C2M_GUILD_MOVE_ITEM", "ExternalMessage::C2M_GUILD_MOVE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_move_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_MOVE_ITEM(); });
    external_message_c2m_guild_move_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_MOVE_ITEM>(new ExternalMessage::C2M_GUILD_MOVE_ITEM()); });
    external_message_c2m_guild_move_item_type["bySourceConSlot"] = sol::property([](const ExternalMessage::C2M_GUILD_MOVE_ITEM& self) { return self.bySourceConSlot; }, [](ExternalMessage::C2M_GUILD_MOVE_ITEM& self, decltype(ExternalMessage::C2M_GUILD_MOVE_ITEM::bySourceConSlot) value) { self.bySourceConSlot = value; });
    external_message_c2m_guild_move_item_type["bySourceSlot"] = sol::property([](const ExternalMessage::C2M_GUILD_MOVE_ITEM& self) { return self.bySourceSlot; }, [](ExternalMessage::C2M_GUILD_MOVE_ITEM& self, decltype(ExternalMessage::C2M_GUILD_MOVE_ITEM::bySourceSlot) value) { self.bySourceSlot = value; });
    external_message_c2m_guild_move_item_type["byTargetConSlot"] = sol::property([](const ExternalMessage::C2M_GUILD_MOVE_ITEM& self) { return self.byTargetConSlot; }, [](ExternalMessage::C2M_GUILD_MOVE_ITEM& self, decltype(ExternalMessage::C2M_GUILD_MOVE_ITEM::byTargetConSlot) value) { self.byTargetConSlot = value; });
    external_message_c2m_guild_move_item_type["byTargetSlot"] = sol::property([](const ExternalMessage::C2M_GUILD_MOVE_ITEM& self) { return self.byTargetSlot; }, [](ExternalMessage::C2M_GUILD_MOVE_ITEM& self, decltype(ExternalMessage::C2M_GUILD_MOVE_ITEM::byTargetSlot) value) { self.byTargetSlot = value; });
    auto external_message_c2m_guild_more_chest_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_MORE_CHEST>(L, external_message_namespace, "C2M_GUILD_MORE_CHEST", "ExternalMessage::C2M_GUILD_MORE_CHEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_more_chest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_MORE_CHEST(); });
    external_message_c2m_guild_more_chest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_MORE_CHEST>(new ExternalMessage::C2M_GUILD_MORE_CHEST()); });
}

}
