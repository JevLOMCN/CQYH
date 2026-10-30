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

void register_external_game_to_client_loot_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_drop_info_type = cqyh::gen_msg::usertype<ExternalMessage::DROP_INFO>(L, external_message_namespace, "DROP_INFO", "ExternalMessage::DROP_INFO");
    external_message_drop_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::DROP_INFO(); });
    external_message_drop_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::DROP_INFO>(new ExternalMessage::DROP_INFO()); });
    external_message_drop_info_type["dwDropID"] = sol::property([](const ExternalMessage::DROP_INFO& self) { return self.dwDropID; }, [](ExternalMessage::DROP_INFO& self, decltype(ExternalMessage::DROP_INFO::dwDropID) value) { self.dwDropID = value; });
    external_message_drop_info_type["wDestX"] = sol::property([](const ExternalMessage::DROP_INFO& self) { return self.wDestX; }, [](ExternalMessage::DROP_INFO& self, decltype(ExternalMessage::DROP_INFO::wDestX) value) { self.wDestX = value; });
    external_message_drop_info_type["wDestY"] = sol::property([](const ExternalMessage::DROP_INFO& self) { return self.wDestY; }, [](ExternalMessage::DROP_INFO& self, decltype(ExternalMessage::DROP_INFO::wDestY) value) { self.wDestY = value; });
    external_message_drop_info_type["wDestZ"] = sol::property([](const ExternalMessage::DROP_INFO& self) { return self.wDestZ; }, [](ExternalMessage::DROP_INFO& self, decltype(ExternalMessage::DROP_INFO::wDestZ) value) { self.wDestZ = value; });
    external_message_drop_info_type["nTemplateID"] = sol::property([](const ExternalMessage::DROP_INFO& self) { return self.nTemplateID; }, [](ExternalMessage::DROP_INFO& self, decltype(ExternalMessage::DROP_INFO::nTemplateID) value) { self.nTemplateID = value; });
    external_message_drop_info_type["nCount"] = sol::property([](const ExternalMessage::DROP_INFO& self) { return self.nCount; }, [](ExternalMessage::DROP_INFO& self, decltype(ExternalMessage::DROP_INFO::nCount) value) { self.nCount = value; });
    auto external_message_g2c_generate_drop_from_object_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT>(L, external_message_namespace, "G2C_GENERATE_DROP_FROM_OBJECT", "ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_generate_drop_from_object_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT(); });
    external_message_g2c_generate_drop_from_object_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT>(new ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT()); });
    external_message_g2c_generate_drop_from_object_type["dwSrcID"] = sol::property([](const ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT& self) { return self.dwSrcID; }, [](ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT& self, decltype(ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT::dwSrcID) value) { self.dwSrcID = value; });
    external_message_g2c_generate_drop_from_object_type["drop"] = &ExternalMessage::G2C_GENERATE_DROP_FROM_OBJECT::drop;
    auto external_message_g2c_sync_drop_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_DROP_LIST>(L, external_message_namespace, "G2C_SYNC_DROP_LIST", "ExternalMessage::G2C_SYNC_DROP_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_drop_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_DROP_LIST(); });
    external_message_g2c_sync_drop_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_DROP_LIST>(new ExternalMessage::G2C_SYNC_DROP_LIST()); });
    external_message_g2c_sync_drop_list_type["byIsNew"] = sol::property([](const ExternalMessage::G2C_SYNC_DROP_LIST& self) { return self.byIsNew; }, [](ExternalMessage::G2C_SYNC_DROP_LIST& self, decltype(ExternalMessage::G2C_SYNC_DROP_LIST::byIsNew) value) { self.byIsNew = value; });
    external_message_g2c_sync_drop_list_type["nCount"] = sol::property([](const ExternalMessage::G2C_SYNC_DROP_LIST& self) { return self.nCount; }, [](ExternalMessage::G2C_SYNC_DROP_LIST& self, decltype(ExternalMessage::G2C_SYNC_DROP_LIST::nCount) value) { self.nCount = value; });
    external_message_g2c_sync_drop_list_type["astDrops"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_DROP_LIST::astDrops);
}

}
