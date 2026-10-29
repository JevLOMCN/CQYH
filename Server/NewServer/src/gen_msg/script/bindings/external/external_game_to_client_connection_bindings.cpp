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

void register_external_game_to_client_connection_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_error_notify_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ERROR_NOTIFY>(L, external_message_namespace, "G2C_ERROR_NOTIFY", "ExternalMessage::G2C_ERROR_NOTIFY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_error_notify_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ERROR_NOTIFY(); });
    external_message_g2c_error_notify_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ERROR_NOTIFY>(new ExternalMessage::G2C_ERROR_NOTIFY()); });
    external_message_g2c_error_notify_type["nErrCode"] = sol::property([](const ExternalMessage::G2C_ERROR_NOTIFY& self) { return self.nErrCode; }, [](ExternalMessage::G2C_ERROR_NOTIFY& self, decltype(ExternalMessage::G2C_ERROR_NOTIFY::nErrCode) value) { self.nErrCode = value; });
    external_message_g2c_error_notify_type["nParam1"] = sol::property([](const ExternalMessage::G2C_ERROR_NOTIFY& self) { return self.nParam1; }, [](ExternalMessage::G2C_ERROR_NOTIFY& self, decltype(ExternalMessage::G2C_ERROR_NOTIFY::nParam1) value) { self.nParam1 = value; });
    external_message_g2c_error_notify_type["nParam2"] = sol::property([](const ExternalMessage::G2C_ERROR_NOTIFY& self) { return self.nParam2; }, [](ExternalMessage::G2C_ERROR_NOTIFY& self, decltype(ExternalMessage::G2C_ERROR_NOTIFY::nParam2) value) { self.nParam2 = value; });
    auto external_message_g2c_sync_frame_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_FRAME>(L, external_message_namespace, "G2C_SYNC_FRAME", "ExternalMessage::G2C_SYNC_FRAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_frame_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_FRAME(); });
    external_message_g2c_sync_frame_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_FRAME>(new ExternalMessage::G2C_SYNC_FRAME()); });
    external_message_g2c_sync_frame_type["nFrame"] = sol::property([](const ExternalMessage::G2C_SYNC_FRAME& self) { return self.nFrame; }, [](ExternalMessage::G2C_SYNC_FRAME& self, decltype(ExternalMessage::G2C_SYNC_FRAME::nFrame) value) { self.nFrame = value; });
    auto external_message_g2c_frame_signal_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_FRAME_SIGNAL>(L, external_message_namespace, "G2C_FRAME_SIGNAL", "ExternalMessage::G2C_FRAME_SIGNAL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_frame_signal_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_FRAME_SIGNAL(); });
    external_message_g2c_frame_signal_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_FRAME_SIGNAL>(new ExternalMessage::G2C_FRAME_SIGNAL()); });
    auto external_message_g2c_ping_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_PING_ACK>(L, external_message_namespace, "G2C_PING_ACK", "ExternalMessage::G2C_PING_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_ping_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_PING_ACK(); });
    external_message_g2c_ping_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_PING_ACK>(new ExternalMessage::G2C_PING_ACK()); });
    external_message_g2c_ping_ack_type["dwPingTick"] = sol::property([](const ExternalMessage::G2C_PING_ACK& self) { return self.dwPingTick; }, [](ExternalMessage::G2C_PING_ACK& self, decltype(ExternalMessage::G2C_PING_ACK::dwPingTick) value) { self.dwPingTick = value; });
    auto external_message_g2c_countdown_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COUNTDOWN>(L, external_message_namespace, "G2C_COUNTDOWN", "ExternalMessage::G2C_COUNTDOWN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_countdown_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COUNTDOWN(); });
    external_message_g2c_countdown_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COUNTDOWN>(new ExternalMessage::G2C_COUNTDOWN()); });
    external_message_g2c_countdown_type["nType"] = sol::property([](const ExternalMessage::G2C_COUNTDOWN& self) { return self.nType; }, [](ExternalMessage::G2C_COUNTDOWN& self, decltype(ExternalMessage::G2C_COUNTDOWN::nType) value) { self.nType = value; });
    external_message_g2c_countdown_type["nTime"] = sol::property([](const ExternalMessage::G2C_COUNTDOWN& self) { return self.nTime; }, [](ExternalMessage::G2C_COUNTDOWN& self, decltype(ExternalMessage::G2C_COUNTDOWN::nTime) value) { self.nTime = value; });
}

}
