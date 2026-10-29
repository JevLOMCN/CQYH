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

void register_external_game_to_client_titles_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_equip_title_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_EQUIP_TITLE>(L, external_message_namespace, "G2C_SYNC_EQUIP_TITLE", "ExternalMessage::G2C_SYNC_EQUIP_TITLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_equip_title_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_EQUIP_TITLE(); });
    external_message_g2c_sync_equip_title_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_EQUIP_TITLE>(new ExternalMessage::G2C_SYNC_EQUIP_TITLE()); });
    external_message_g2c_sync_equip_title_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_EQUIP_TITLE& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_EQUIP_TITLE& self, decltype(ExternalMessage::G2C_SYNC_EQUIP_TITLE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_equip_title_type["byTitleID"] = sol::property([](const ExternalMessage::G2C_SYNC_EQUIP_TITLE& self) { return self.byTitleID; }, [](ExternalMessage::G2C_SYNC_EQUIP_TITLE& self, decltype(ExternalMessage::G2C_SYNC_EQUIP_TITLE::byTitleID) value) { self.byTitleID = value; });
    auto external_message_title_data_type = cqyh::gen_msg::usertype<ExternalMessage::TITLE_DATA>(L, external_message_namespace, "TITLE_DATA", "ExternalMessage::TITLE_DATA");
    external_message_title_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::TITLE_DATA(); });
    external_message_title_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::TITLE_DATA>(new ExternalMessage::TITLE_DATA()); });
    external_message_title_data_type["m_nTemplateID"] = sol::property([](const ExternalMessage::TITLE_DATA& self) { return self.m_nTemplateID; }, [](ExternalMessage::TITLE_DATA& self, decltype(ExternalMessage::TITLE_DATA::m_nTemplateID) value) { self.m_nTemplateID = value; });
    external_message_title_data_type["m_nExpireTime"] = sol::property([](const ExternalMessage::TITLE_DATA& self) { return self.m_nExpireTime; }, [](ExternalMessage::TITLE_DATA& self, decltype(ExternalMessage::TITLE_DATA::m_nExpireTime) value) { self.m_nExpireTime = value; });
    auto external_message_g2c_sync_title_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_TITLE>(L, external_message_namespace, "G2C_SYNC_TITLE", "ExternalMessage::G2C_SYNC_TITLE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_title_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_TITLE(); });
    external_message_g2c_sync_title_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_TITLE>(new ExternalMessage::G2C_SYNC_TITLE()); });
    external_message_g2c_sync_title_type["nEquipedTitleID"] = sol::property([](const ExternalMessage::G2C_SYNC_TITLE& self) { return self.nEquipedTitleID; }, [](ExternalMessage::G2C_SYNC_TITLE& self, decltype(ExternalMessage::G2C_SYNC_TITLE::nEquipedTitleID) value) { self.nEquipedTitleID = value; });
    external_message_g2c_sync_title_type["nTitleCount"] = sol::property([](const ExternalMessage::G2C_SYNC_TITLE& self) { return self.nTitleCount; }, [](ExternalMessage::G2C_SYNC_TITLE& self, decltype(ExternalMessage::G2C_SYNC_TITLE::nTitleCount) value) { self.nTitleCount = value; });
    external_message_g2c_sync_title_type["stTitleList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_TITLE::stTitleList);
    auto external_message_g2c_add_title_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ADD_TITLE>(L, external_message_namespace, "G2C_ADD_TITLE", "ExternalMessage::G2C_ADD_TITLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_add_title_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ADD_TITLE(); });
    external_message_g2c_add_title_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ADD_TITLE>(new ExternalMessage::G2C_ADD_TITLE()); });
    external_message_g2c_add_title_type["nTitleID"] = sol::property([](const ExternalMessage::G2C_ADD_TITLE& self) { return self.nTitleID; }, [](ExternalMessage::G2C_ADD_TITLE& self, decltype(ExternalMessage::G2C_ADD_TITLE::nTitleID) value) { self.nTitleID = value; });
    external_message_g2c_add_title_type["nExpireTime"] = sol::property([](const ExternalMessage::G2C_ADD_TITLE& self) { return self.nExpireTime; }, [](ExternalMessage::G2C_ADD_TITLE& self, decltype(ExternalMessage::G2C_ADD_TITLE::nExpireTime) value) { self.nExpireTime = value; });
    auto external_message_g2c_del_title_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DEL_TITLE>(L, external_message_namespace, "G2C_DEL_TITLE", "ExternalMessage::G2C_DEL_TITLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_del_title_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DEL_TITLE(); });
    external_message_g2c_del_title_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DEL_TITLE>(new ExternalMessage::G2C_DEL_TITLE()); });
    external_message_g2c_del_title_type["nTitleID"] = sol::property([](const ExternalMessage::G2C_DEL_TITLE& self) { return self.nTitleID; }, [](ExternalMessage::G2C_DEL_TITLE& self, decltype(ExternalMessage::G2C_DEL_TITLE::nTitleID) value) { self.nTitleID = value; });
    external_message_g2c_del_title_type["nDelType"] = sol::property([](const ExternalMessage::G2C_DEL_TITLE& self) { return self.nDelType; }, [](ExternalMessage::G2C_DEL_TITLE& self, decltype(ExternalMessage::G2C_DEL_TITLE::nDelType) value) { self.nDelType = value; });
}

}
