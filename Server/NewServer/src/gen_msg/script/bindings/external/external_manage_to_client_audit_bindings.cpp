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

void register_external_manage_to_client_audit_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_guild_history_event_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_HISTORY_EVENT_INFO>(L, external_message_namespace, "GUILD_HISTORY_EVENT_INFO", "ExternalMessage::GUILD_HISTORY_EVENT_INFO");
    external_message_guild_history_event_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_HISTORY_EVENT_INFO(); });
    external_message_guild_history_event_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_HISTORY_EVENT_INFO>(new ExternalMessage::GUILD_HISTORY_EVENT_INFO()); });
    external_message_guild_history_event_info_type["byEventCount"] = sol::property([](const ExternalMessage::GUILD_HISTORY_EVENT_INFO& self) { return self.byEventCount; }, [](ExternalMessage::GUILD_HISTORY_EVENT_INFO& self, decltype(ExternalMessage::GUILD_HISTORY_EVENT_INFO::byEventCount) value) { self.byEventCount = value; });
    external_message_guild_history_event_info_type["stEventInfo"] = cqyh::lua::array_property(&ExternalMessage::GUILD_HISTORY_EVENT_INFO::stEventInfo);
    auto external_message_m2c_guild_add_history_event_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF>(L, external_message_namespace, "M2C_GUILD_ADD_HISTORY_EVENT_NTF", "ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_add_history_event_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF(); });
    external_message_m2c_guild_add_history_event_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF>(new ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF()); });
    external_message_m2c_guild_add_history_event_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF::byType) value) { self.byType = value; });
    external_message_m2c_guild_add_history_event_ntf_type["dwParam1"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self) { return self.dwParam1; }, [](ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF::dwParam1) value) { self.dwParam1 = value; });
    external_message_m2c_guild_add_history_event_ntf_type["dwParam2"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self) { return self.dwParam2; }, [](ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF::dwParam2) value) { self.dwParam2 = value; });
    external_message_m2c_guild_add_history_event_ntf_type["dwParam3"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self) { return self.dwParam3; }, [](ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF::dwParam3) value) { self.dwParam3 = value; });
    external_message_m2c_guild_add_history_event_ntf_type["dwParam4"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self) { return self.dwParam4; }, [](ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF::dwParam4) value) { self.dwParam4 = value; });
    external_message_m2c_guild_add_history_event_ntf_type["dwOccurTime"] = sol::property([](const ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self) { return self.dwOccurTime; }, [](ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF& self, decltype(ExternalMessage::M2C_GUILD_ADD_HISTORY_EVENT_NTF::dwOccurTime) value) { self.dwOccurTime = value; });
    auto external_message_m2c_guild_more_history_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_MORE_HISTORY>(L, external_message_namespace, "M2C_GUILD_MORE_HISTORY", "ExternalMessage::M2C_GUILD_MORE_HISTORY", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_more_history_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_MORE_HISTORY(); });
    external_message_m2c_guild_more_history_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_MORE_HISTORY>(new ExternalMessage::M2C_GUILD_MORE_HISTORY()); });
    external_message_m2c_guild_more_history_type["wEventNum"] = sol::property([](const ExternalMessage::M2C_GUILD_MORE_HISTORY& self) { return self.wEventNum; }, [](ExternalMessage::M2C_GUILD_MORE_HISTORY& self, decltype(ExternalMessage::M2C_GUILD_MORE_HISTORY::wEventNum) value) { self.wEventNum = value; });
    external_message_m2c_guild_more_history_type["stEventInfo"] = cqyh::lua::array_property(&ExternalMessage::M2C_GUILD_MORE_HISTORY::stEventInfo);
    auto external_message_shop_log_type = cqyh::gen_msg::usertype<ExternalMessage::SHOP_LOG>(L, external_message_namespace, "SHOP_LOG", "ExternalMessage::SHOP_LOG");
    external_message_shop_log_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::SHOP_LOG(); });
    external_message_shop_log_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::SHOP_LOG>(new ExternalMessage::SHOP_LOG()); });
    external_message_shop_log_type["nItemID"] = sol::property([](const ExternalMessage::SHOP_LOG& self) { return self.nItemID; }, [](ExternalMessage::SHOP_LOG& self, decltype(ExternalMessage::SHOP_LOG::nItemID) value) { self.nItemID = value; });
    external_message_shop_log_type["nItemCount"] = sol::property([](const ExternalMessage::SHOP_LOG& self) { return self.nItemCount; }, [](ExternalMessage::SHOP_LOG& self, decltype(ExternalMessage::SHOP_LOG::nItemCount) value) { self.nItemCount = value; });
    external_message_shop_log_type["nCost"] = sol::property([](const ExternalMessage::SHOP_LOG& self) { return self.nCost; }, [](ExternalMessage::SHOP_LOG& self, decltype(ExternalMessage::SHOP_LOG::nCost) value) { self.nCost = value; });
    external_message_shop_log_type["dwShopTime"] = sol::property([](const ExternalMessage::SHOP_LOG& self) { return self.dwShopTime; }, [](ExternalMessage::SHOP_LOG& self, decltype(ExternalMessage::SHOP_LOG::dwShopTime) value) { self.dwShopTime = value; });
}

}
