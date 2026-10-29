#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_guild_storage_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_guild_view_storage_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_VIEW_STORAGE>(L, external_message_namespace, "C2G_GUILD_VIEW_STORAGE", "ExternalMessage::C2G_GUILD_VIEW_STORAGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_view_storage_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_VIEW_STORAGE(); });
    external_message_c2g_guild_view_storage_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_VIEW_STORAGE>(new ExternalMessage::C2G_GUILD_VIEW_STORAGE()); });
    external_message_c2g_guild_view_storage_type["byStorageTag"] = sol::property([](const ExternalMessage::C2G_GUILD_VIEW_STORAGE& self) { return self.byStorageTag; }, [](ExternalMessage::C2G_GUILD_VIEW_STORAGE& self, decltype(ExternalMessage::C2G_GUILD_VIEW_STORAGE::byStorageTag) value) { self.byStorageTag = value; });
    auto external_message_c2g_guild_push_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_PUSH_ITEM>(L, external_message_namespace, "C2G_GUILD_PUSH_ITEM", "ExternalMessage::C2G_GUILD_PUSH_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_push_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_PUSH_ITEM(); });
    external_message_c2g_guild_push_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_PUSH_ITEM>(new ExternalMessage::C2G_GUILD_PUSH_ITEM()); });
    external_message_c2g_guild_push_item_type["wItemConSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_PUSH_ITEM& self) { return self.wItemConSlot; }, [](ExternalMessage::C2G_GUILD_PUSH_ITEM& self, decltype(ExternalMessage::C2G_GUILD_PUSH_ITEM::wItemConSlot) value) { self.wItemConSlot = value; });
    external_message_c2g_guild_push_item_type["wItemSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_PUSH_ITEM& self) { return self.wItemSlot; }, [](ExternalMessage::C2G_GUILD_PUSH_ITEM& self, decltype(ExternalMessage::C2G_GUILD_PUSH_ITEM::wItemSlot) value) { self.wItemSlot = value; });
    external_message_c2g_guild_push_item_type["byStorageConSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_PUSH_ITEM& self) { return self.byStorageConSlot; }, [](ExternalMessage::C2G_GUILD_PUSH_ITEM& self, decltype(ExternalMessage::C2G_GUILD_PUSH_ITEM::byStorageConSlot) value) { self.byStorageConSlot = value; });
    external_message_c2g_guild_push_item_type["byStorageSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_PUSH_ITEM& self) { return self.byStorageSlot; }, [](ExternalMessage::C2G_GUILD_PUSH_ITEM& self, decltype(ExternalMessage::C2G_GUILD_PUSH_ITEM::byStorageSlot) value) { self.byStorageSlot = value; });
    auto external_message_c2g_guild_pop_item_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_POP_ITEM>(L, external_message_namespace, "C2G_GUILD_POP_ITEM", "ExternalMessage::C2G_GUILD_POP_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_pop_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_POP_ITEM(); });
    external_message_c2g_guild_pop_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_POP_ITEM>(new ExternalMessage::C2G_GUILD_POP_ITEM()); });
    external_message_c2g_guild_pop_item_type["byStorageConSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_POP_ITEM& self) { return self.byStorageConSlot; }, [](ExternalMessage::C2G_GUILD_POP_ITEM& self, decltype(ExternalMessage::C2G_GUILD_POP_ITEM::byStorageConSlot) value) { self.byStorageConSlot = value; });
    external_message_c2g_guild_pop_item_type["byStorageSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_POP_ITEM& self) { return self.byStorageSlot; }, [](ExternalMessage::C2G_GUILD_POP_ITEM& self, decltype(ExternalMessage::C2G_GUILD_POP_ITEM::byStorageSlot) value) { self.byStorageSlot = value; });
    external_message_c2g_guild_pop_item_type["wItemConSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_POP_ITEM& self) { return self.wItemConSlot; }, [](ExternalMessage::C2G_GUILD_POP_ITEM& self, decltype(ExternalMessage::C2G_GUILD_POP_ITEM::wItemConSlot) value) { self.wItemConSlot = value; });
    external_message_c2g_guild_pop_item_type["wItemSlot"] = sol::property([](const ExternalMessage::C2G_GUILD_POP_ITEM& self) { return self.wItemSlot; }, [](ExternalMessage::C2G_GUILD_POP_ITEM& self, decltype(ExternalMessage::C2G_GUILD_POP_ITEM::wItemSlot) value) { self.wItemSlot = value; });
}

}
