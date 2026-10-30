#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/b2c_message.h"

namespace cqyh::gen_msg::bindings {

void register_external_balance_to_client_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    external_message_namespace["b2c_message_begin"] = static_cast<lua_Number>(ExternalMessage::b2c_message_begin);
    external_message_namespace["b2c_query_gate_ack"] = static_cast<lua_Number>(ExternalMessage::b2c_query_gate_ack);
    external_message_namespace["b2c_message_end"] = static_cast<lua_Number>(ExternalMessage::b2c_message_end);
    auto external_message_b2c_query_gate_ack_type = cqyh::gen_msg::usertype<ExternalMessage::B2C_QUERY_GATE_ACK>(L, external_message_namespace, "B2C_QUERY_GATE_ACK", "ExternalMessage::B2C_QUERY_GATE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_b2c_query_gate_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::B2C_QUERY_GATE_ACK(); });
    external_message_b2c_query_gate_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::B2C_QUERY_GATE_ACK>(new ExternalMessage::B2C_QUERY_GATE_ACK()); });
    external_message_b2c_query_gate_ack_type["dwAddr"] = sol::property([](const ExternalMessage::B2C_QUERY_GATE_ACK& self) { return self.dwAddr; }, [](ExternalMessage::B2C_QUERY_GATE_ACK& self, decltype(ExternalMessage::B2C_QUERY_GATE_ACK::dwAddr) value) { self.dwAddr = value; });
    external_message_b2c_query_gate_ack_type["wPort"] = sol::property([](const ExternalMessage::B2C_QUERY_GATE_ACK& self) { return self.wPort; }, [](ExternalMessage::B2C_QUERY_GATE_ACK& self, decltype(ExternalMessage::B2C_QUERY_GATE_ACK::wPort) value) { self.wPort = value; });
}

}
