#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_mail_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_mail_write_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MAIL_WRITE>(L, external_message_namespace, "C2M_MAIL_WRITE", "ExternalMessage::C2M_MAIL_WRITE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_mail_write_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MAIL_WRITE(); });
    external_message_c2m_mail_write_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MAIL_WRITE>(new ExternalMessage::C2M_MAIL_WRITE()); });
    external_message_c2m_mail_write_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2M_MAIL_WRITE::szRoleName);
    external_message_c2m_mail_write_type["szMailSubject"] = cqyh::lua::string_property(&ExternalMessage::C2M_MAIL_WRITE::szMailSubject);
    external_message_c2m_mail_write_type["dwMailContentLen"] = sol::property([](const ExternalMessage::C2M_MAIL_WRITE& self) { return self.dwMailContentLen; }, [](ExternalMessage::C2M_MAIL_WRITE& self, decltype(ExternalMessage::C2M_MAIL_WRITE::dwMailContentLen) value) { self.dwMailContentLen = value; });
    external_message_c2m_mail_write_type["szMailContent"] = cqyh::lua::string_property(&ExternalMessage::C2M_MAIL_WRITE::szMailContent);
    auto external_message_c2m_mail_list_recv_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MAIL_LIST_RECV>(L, external_message_namespace, "C2M_MAIL_LIST_RECV", "ExternalMessage::C2M_MAIL_LIST_RECV", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_mail_list_recv_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MAIL_LIST_RECV(); });
    external_message_c2m_mail_list_recv_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MAIL_LIST_RECV>(new ExternalMessage::C2M_MAIL_LIST_RECV()); });
    auto external_message_c2m_mail_read_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MAIL_READ>(L, external_message_namespace, "C2M_MAIL_READ", "ExternalMessage::C2M_MAIL_READ", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_mail_read_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MAIL_READ(); });
    external_message_c2m_mail_read_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MAIL_READ>(new ExternalMessage::C2M_MAIL_READ()); });
    external_message_c2m_mail_read_type["dwMailID"] = sol::property([](const ExternalMessage::C2M_MAIL_READ& self) { return self.dwMailID; }, [](ExternalMessage::C2M_MAIL_READ& self, decltype(ExternalMessage::C2M_MAIL_READ::dwMailID) value) { self.dwMailID = value; });
    auto external_message_c2m_mail_delete_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MAIL_DELETE>(L, external_message_namespace, "C2M_MAIL_DELETE", "ExternalMessage::C2M_MAIL_DELETE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_mail_delete_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MAIL_DELETE(); });
    external_message_c2m_mail_delete_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MAIL_DELETE>(new ExternalMessage::C2M_MAIL_DELETE()); });
    external_message_c2m_mail_delete_type["dwMailID"] = sol::property([](const ExternalMessage::C2M_MAIL_DELETE& self) { return self.dwMailID; }, [](ExternalMessage::C2M_MAIL_DELETE& self, decltype(ExternalMessage::C2M_MAIL_DELETE::dwMailID) value) { self.dwMailID = value; });
    auto external_message_c2m_mail_get_attach_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MAIL_GET_ATTACH>(L, external_message_namespace, "C2M_MAIL_GET_ATTACH", "ExternalMessage::C2M_MAIL_GET_ATTACH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_mail_get_attach_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MAIL_GET_ATTACH(); });
    external_message_c2m_mail_get_attach_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MAIL_GET_ATTACH>(new ExternalMessage::C2M_MAIL_GET_ATTACH()); });
    external_message_c2m_mail_get_attach_type["dwMailID"] = sol::property([](const ExternalMessage::C2M_MAIL_GET_ATTACH& self) { return self.dwMailID; }, [](ExternalMessage::C2M_MAIL_GET_ATTACH& self, decltype(ExternalMessage::C2M_MAIL_GET_ATTACH::dwMailID) value) { self.dwMailID = value; });
}

}
