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

void register_external_manage_to_client_connection_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_error_notify_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ERROR_NOTIFY>(L, external_message_namespace, "M2C_ERROR_NOTIFY", "ExternalMessage::M2C_ERROR_NOTIFY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_error_notify_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ERROR_NOTIFY(); });
    external_message_m2c_error_notify_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ERROR_NOTIFY>(new ExternalMessage::M2C_ERROR_NOTIFY()); });
    external_message_m2c_error_notify_type["nErrorCode"] = sol::property([](const ExternalMessage::M2C_ERROR_NOTIFY& self) { return self.nErrorCode; }, [](ExternalMessage::M2C_ERROR_NOTIFY& self, decltype(ExternalMessage::M2C_ERROR_NOTIFY::nErrorCode) value) { self.nErrorCode = value; });
    external_message_m2c_error_notify_type["nParam1"] = sol::property([](const ExternalMessage::M2C_ERROR_NOTIFY& self) { return self.nParam1; }, [](ExternalMessage::M2C_ERROR_NOTIFY& self, decltype(ExternalMessage::M2C_ERROR_NOTIFY::nParam1) value) { self.nParam1 = value; });
    external_message_m2c_error_notify_type["nParam2"] = sol::property([](const ExternalMessage::M2C_ERROR_NOTIFY& self) { return self.nParam2; }, [](ExternalMessage::M2C_ERROR_NOTIFY& self, decltype(ExternalMessage::M2C_ERROR_NOTIFY::nParam2) value) { self.nParam2 = value; });
    auto external_message_m2c_queue_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_QUEUE_INFO>(L, external_message_namespace, "M2C_QUEUE_INFO", "ExternalMessage::M2C_QUEUE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_queue_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_QUEUE_INFO(); });
    external_message_m2c_queue_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_QUEUE_INFO>(new ExternalMessage::M2C_QUEUE_INFO()); });
    external_message_m2c_queue_info_type["nTotal"] = sol::property([](const ExternalMessage::M2C_QUEUE_INFO& self) { return self.nTotal; }, [](ExternalMessage::M2C_QUEUE_INFO& self, decltype(ExternalMessage::M2C_QUEUE_INFO::nTotal) value) { self.nTotal = value; });
    external_message_m2c_queue_info_type["nSelfPos"] = sol::property([](const ExternalMessage::M2C_QUEUE_INFO& self) { return self.nSelfPos; }, [](ExternalMessage::M2C_QUEUE_INFO& self, decltype(ExternalMessage::M2C_QUEUE_INFO::nSelfPos) value) { self.nSelfPos = value; });
    auto external_message_m2c_join_battle_queue_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_JOIN_BATTLE_QUEUE>(L, external_message_namespace, "M2C_JOIN_BATTLE_QUEUE", "ExternalMessage::M2C_JOIN_BATTLE_QUEUE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_join_battle_queue_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_JOIN_BATTLE_QUEUE(); });
    external_message_m2c_join_battle_queue_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_JOIN_BATTLE_QUEUE>(new ExternalMessage::M2C_JOIN_BATTLE_QUEUE()); });
    external_message_m2c_join_battle_queue_type["dwAverageQueueTime"] = sol::property([](const ExternalMessage::M2C_JOIN_BATTLE_QUEUE& self) { return self.dwAverageQueueTime; }, [](ExternalMessage::M2C_JOIN_BATTLE_QUEUE& self, decltype(ExternalMessage::M2C_JOIN_BATTLE_QUEUE::dwAverageQueueTime) value) { self.dwAverageQueueTime = value; });
}

}
