#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/m2c_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/ranklist_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/team_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_manage_to_client_mentorship_recruitment_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_master_recruit_list_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_RECRUIT_LIST>(L, external_message_namespace, "M2C_MASTER_RECRUIT_LIST", "ExternalMessage::M2C_MASTER_RECRUIT_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_recruit_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_RECRUIT_LIST(); });
    external_message_m2c_master_recruit_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_RECRUIT_LIST>(new ExternalMessage::M2C_MASTER_RECRUIT_LIST()); });
    auto external_message_m2c_master_recruit_list_recruit_master_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO>(L, external_message_m2c_master_recruit_list_type, "RECRUIT_MASTER_INFO", "ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO");
    external_message_m2c_master_recruit_list_recruit_master_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO(); });
    external_message_m2c_master_recruit_list_recruit_master_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO>(new ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO()); });
    external_message_m2c_master_recruit_list_recruit_master_info_type["byLevel"] = sol::property([](const ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self) { return self.byLevel; }, [](ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO::byLevel) value) { self.byLevel = value; });
    external_message_m2c_master_recruit_list_recruit_master_info_type["byClass"] = sol::property([](const ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self) { return self.byClass; }, [](ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO::byClass) value) { self.byClass = value; });
    external_message_m2c_master_recruit_list_recruit_master_info_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_master_recruit_list_recruit_master_info_type["llMasterExp"] = sol::property([](const ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self) { return self.llMasterExp; }, [](ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO::llMasterExp) value) { self.llMasterExp = value; });
    external_message_m2c_master_recruit_list_type["byCount"] = sol::property([](const ExternalMessage::M2C_MASTER_RECRUIT_LIST& self) { return self.byCount; }, [](ExternalMessage::M2C_MASTER_RECRUIT_LIST& self, decltype(ExternalMessage::M2C_MASTER_RECRUIT_LIST::byCount) value) { self.byCount = value; });
    external_message_m2c_master_recruit_list_type["szListData"] = cqyh::lua::array_property(&ExternalMessage::M2C_MASTER_RECRUIT_LIST::szListData);
    auto external_message_m2c_master_get_solo_list_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK>(L, external_message_namespace, "M2C_MASTER_GET_SOLO_LIST_ACK", "ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_get_solo_list_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK(); });
    external_message_m2c_master_get_solo_list_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK>(new ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK()); });
    auto external_message_m2c_master_get_solo_list_ack_master_solo_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO>(L, external_message_m2c_master_get_solo_list_ack_type, "MASTER_SOLO_INFO", "ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO");
    external_message_m2c_master_get_solo_list_ack_master_solo_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO(); });
    external_message_m2c_master_get_solo_list_ack_master_solo_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO>(new ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO()); });
    external_message_m2c_master_get_solo_list_ack_master_solo_info_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO& self, decltype(ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_master_get_solo_list_ack_master_solo_info_type["byLevel"] = sol::property([](const ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO& self) { return self.byLevel; }, [](ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO& self, decltype(ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO::byLevel) value) { self.byLevel = value; });
    external_message_m2c_master_get_solo_list_ack_master_solo_info_type["byClass"] = sol::property([](const ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO& self) { return self.byClass; }, [](ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO& self, decltype(ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO::byClass) value) { self.byClass = value; });
    external_message_m2c_master_get_solo_list_ack_type["byCount"] = sol::property([](const ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK& self) { return self.byCount; }, [](ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK& self, decltype(ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::byCount) value) { self.byCount = value; });
    external_message_m2c_master_get_solo_list_ack_type["szListData"] = cqyh::lua::array_property(&ExternalMessage::M2C_MASTER_GET_SOLO_LIST_ACK::szListData);
}

}
