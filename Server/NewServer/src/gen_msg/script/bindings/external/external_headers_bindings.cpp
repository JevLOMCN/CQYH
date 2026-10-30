#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"

namespace cqyh::gen_msg::bindings {

void register_external_headers_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    globals["EXTERNAL_MESSAGE_MAJOR_VERSION"] = static_cast<lua_Number>(EXTERNAL_MESSAGE_MAJOR_VERSION);
    globals["EXTERNAL_MESSAGE_MINOR_VERSION"] = static_cast<lua_Number>(EXTERNAL_MESSAGE_MINOR_VERSION);
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_message_header_type = cqyh::gen_msg::usertype<ExternalMessage::MESSAGE_HEADER>(L, external_message_namespace, "MESSAGE_HEADER", "ExternalMessage::MESSAGE_HEADER");
    external_message_message_header_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::MESSAGE_HEADER(); });
    external_message_message_header_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::MESSAGE_HEADER>(new ExternalMessage::MESSAGE_HEADER()); });
    external_message_message_header_type["wMsg"] = sol::property([](const ExternalMessage::MESSAGE_HEADER& self) { return self.wMsg; }, [](ExternalMessage::MESSAGE_HEADER& self, decltype(ExternalMessage::MESSAGE_HEADER::wMsg) value) { self.wMsg = value; });
    auto external_message_flex_message_header_type = cqyh::gen_msg::usertype<ExternalMessage::FLEX_MESSAGE_HEADER>(L, external_message_namespace, "FLEX_MESSAGE_HEADER", "ExternalMessage::FLEX_MESSAGE_HEADER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_flex_message_header_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::FLEX_MESSAGE_HEADER(); });
    external_message_flex_message_header_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::FLEX_MESSAGE_HEADER>(new ExternalMessage::FLEX_MESSAGE_HEADER()); });
    external_message_flex_message_header_type["wSize"] = sol::property([](const ExternalMessage::FLEX_MESSAGE_HEADER& self) { return self.wSize; }, [](ExternalMessage::FLEX_MESSAGE_HEADER& self, decltype(ExternalMessage::FLEX_MESSAGE_HEADER::wSize) value) { self.wSize = value; });
    external_message_namespace["max_message_count"] = static_cast<lua_Number>(ExternalMessage::max_message_count);
    external_message_namespace["flex_message_size"] = static_cast<lua_Number>(ExternalMessage::flex_message_size);
}

}
