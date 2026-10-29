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

void register_external_manage_to_client_guild_diplomacy_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_guild_diplomat_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_DIPLOMAT_INFO>(L, external_message_namespace, "GUILD_DIPLOMAT_INFO", "ExternalMessage::GUILD_DIPLOMAT_INFO");
    external_message_guild_diplomat_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_DIPLOMAT_INFO(); });
    external_message_guild_diplomat_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_DIPLOMAT_INFO>(new ExternalMessage::GUILD_DIPLOMAT_INFO()); });
    external_message_guild_diplomat_info_type["byType"] = sol::property([](const ExternalMessage::GUILD_DIPLOMAT_INFO& self) { return self.byType; }, [](ExternalMessage::GUILD_DIPLOMAT_INFO& self, decltype(ExternalMessage::GUILD_DIPLOMAT_INFO::byType) value) { self.byType = value; });
    external_message_guild_diplomat_info_type["dwGuildID"] = sol::property([](const ExternalMessage::GUILD_DIPLOMAT_INFO& self) { return self.dwGuildID; }, [](ExternalMessage::GUILD_DIPLOMAT_INFO& self, decltype(ExternalMessage::GUILD_DIPLOMAT_INFO::dwGuildID) value) { self.dwGuildID = value; });
    external_message_guild_diplomat_info_type["dwLastTime"] = sol::property([](const ExternalMessage::GUILD_DIPLOMAT_INFO& self) { return self.dwLastTime; }, [](ExternalMessage::GUILD_DIPLOMAT_INFO& self, decltype(ExternalMessage::GUILD_DIPLOMAT_INFO::dwLastTime) value) { self.dwLastTime = value; });
    external_message_guild_diplomat_info_type["byLevel"] = sol::property([](const ExternalMessage::GUILD_DIPLOMAT_INFO& self) { return self.byLevel; }, [](ExternalMessage::GUILD_DIPLOMAT_INFO& self, decltype(ExternalMessage::GUILD_DIPLOMAT_INFO::byLevel) value) { self.byLevel = value; });
    external_message_guild_diplomat_info_type["byMemberCount"] = sol::property([](const ExternalMessage::GUILD_DIPLOMAT_INFO& self) { return self.byMemberCount; }, [](ExternalMessage::GUILD_DIPLOMAT_INFO& self, decltype(ExternalMessage::GUILD_DIPLOMAT_INFO::byMemberCount) value) { self.byMemberCount = value; });
    external_message_guild_diplomat_info_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::GUILD_DIPLOMAT_INFO::szGuildName);
    auto external_message_m2c_guild_apply_alliance_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK>(L, external_message_namespace, "M2C_GUILD_APPLY_ALLIANCE_ACK", "ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_apply_alliance_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK(); });
    external_message_m2c_guild_apply_alliance_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK>(new ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK()); });
    external_message_m2c_guild_apply_alliance_ack_type["dwGuildID"] = sol::property([](const ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK& self) { return self.dwGuildID; }, [](ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK& self, decltype(ExternalMessage::M2C_GUILD_APPLY_ALLIANCE_ACK::dwGuildID) value) { self.dwGuildID = value; });
    auto external_message_apply_alliance_info_type = cqyh::gen_msg::usertype<ExternalMessage::APPLY_ALLIANCE_INFO>(L, external_message_namespace, "APPLY_ALLIANCE_INFO", "ExternalMessage::APPLY_ALLIANCE_INFO");
    external_message_apply_alliance_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::APPLY_ALLIANCE_INFO(); });
    external_message_apply_alliance_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::APPLY_ALLIANCE_INFO>(new ExternalMessage::APPLY_ALLIANCE_INFO()); });
    external_message_apply_alliance_info_type["dwGuildID"] = sol::property([](const ExternalMessage::APPLY_ALLIANCE_INFO& self) { return self.dwGuildID; }, [](ExternalMessage::APPLY_ALLIANCE_INFO& self, decltype(ExternalMessage::APPLY_ALLIANCE_INFO::dwGuildID) value) { self.dwGuildID = value; });
    external_message_apply_alliance_info_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::APPLY_ALLIANCE_INFO::szGuildName);
    external_message_apply_alliance_info_type["byLevel"] = sol::property([](const ExternalMessage::APPLY_ALLIANCE_INFO& self) { return self.byLevel; }, [](ExternalMessage::APPLY_ALLIANCE_INFO& self, decltype(ExternalMessage::APPLY_ALLIANCE_INFO::byLevel) value) { self.byLevel = value; });
    external_message_apply_alliance_info_type["byMemberCount"] = sol::property([](const ExternalMessage::APPLY_ALLIANCE_INFO& self) { return self.byMemberCount; }, [](ExternalMessage::APPLY_ALLIANCE_INFO& self, decltype(ExternalMessage::APPLY_ALLIANCE_INFO::byMemberCount) value) { self.byMemberCount = value; });
    external_message_apply_alliance_info_type["szLeaderName"] = cqyh::lua::string_property(&ExternalMessage::APPLY_ALLIANCE_INFO::szLeaderName);
    external_message_apply_alliance_info_type["dwWashoutTime"] = sol::property([](const ExternalMessage::APPLY_ALLIANCE_INFO& self) { return self.dwWashoutTime; }, [](ExternalMessage::APPLY_ALLIANCE_INFO& self, decltype(ExternalMessage::APPLY_ALLIANCE_INFO::dwWashoutTime) value) { self.dwWashoutTime = value; });
    auto external_message_m2c_guild_get_apply_union_list_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK>(L, external_message_namespace, "M2C_GUILD_GET_APPLY_UNION_LIST_ACK", "ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_get_apply_union_list_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK(); });
    external_message_m2c_guild_get_apply_union_list_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK>(new ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK()); });
    external_message_m2c_guild_get_apply_union_list_ack_type["byCount"] = sol::property([](const ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK& self) { return self.byCount; }, [](ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK& self, decltype(ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK::byCount) value) { self.byCount = value; });
    external_message_m2c_guild_get_apply_union_list_ack_type["info"] = cqyh::lua::array_property(&ExternalMessage::M2C_GUILD_GET_APPLY_UNION_LIST_ACK::info);
    auto external_message_m2c_guild_reply_union_app_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK>(L, external_message_namespace, "M2C_GUILD_REPLY_UNION_APP_ACK", "ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_reply_union_app_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK(); });
    external_message_m2c_guild_reply_union_app_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK>(new ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK()); });
    external_message_m2c_guild_reply_union_app_ack_type["dwGuildID"] = sol::property([](const ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK& self) { return self.dwGuildID; }, [](ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK& self, decltype(ExternalMessage::M2C_GUILD_REPLY_UNION_APP_ACK::dwGuildID) value) { self.dwGuildID = value; });
    auto external_message_m2c_guild_add_diplomat_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF>(L, external_message_namespace, "M2C_GUILD_ADD_DIPLOMAT_NTF", "ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_add_diplomat_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF(); });
    external_message_m2c_guild_add_diplomat_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF>(new ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF()); });
    external_message_m2c_guild_add_diplomat_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF::byType) value) { self.byType = value; });
    external_message_m2c_guild_add_diplomat_ntf_type["dwGuildID"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self) { return self.dwGuildID; }, [](ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF::dwGuildID) value) { self.dwGuildID = value; });
    external_message_m2c_guild_add_diplomat_ntf_type["dwLastTime"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self) { return self.dwLastTime; }, [](ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF::dwLastTime) value) { self.dwLastTime = value; });
    external_message_m2c_guild_add_diplomat_ntf_type["byLevel"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self) { return self.byLevel; }, [](ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF::byLevel) value) { self.byLevel = value; });
    external_message_m2c_guild_add_diplomat_ntf_type["byMemberCount"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self) { return self.byMemberCount; }, [](ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF::byMemberCount) value) { self.byMemberCount = value; });
    external_message_m2c_guild_add_diplomat_ntf_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::M2C_GUILD_ADD_DIPLOMAT_NTF::szGuildName);
    auto external_message_m2c_guild_del_diplomat_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF>(L, external_message_namespace, "M2C_GUILD_DEL_DIPLOMAT_NTF", "ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_del_diplomat_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF(); });
    external_message_m2c_guild_del_diplomat_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF>(new ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF()); });
    external_message_m2c_guild_del_diplomat_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF::byType) value) { self.byType = value; });
    external_message_m2c_guild_del_diplomat_ntf_type["dwGuildID"] = sol::property([](const ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF& self) { return self.dwGuildID; }, [](ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF& self, decltype(ExternalMessage::M2C_GUILD_DEL_DIPLOMAT_NTF::dwGuildID) value) { self.dwGuildID = value; });
}

}
