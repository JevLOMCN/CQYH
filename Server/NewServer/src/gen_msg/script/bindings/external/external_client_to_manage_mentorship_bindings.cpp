#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_mentorship_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_master_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_INFO>(L, external_message_namespace, "C2M_MASTER_INFO", "ExternalMessage::C2M_MASTER_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_INFO(); });
    external_message_c2m_master_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_INFO>(new ExternalMessage::C2M_MASTER_INFO()); });
    auto external_message_c2m_master_cz_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_CZ>(L, external_message_namespace, "C2M_MASTER_CZ", "ExternalMessage::C2M_MASTER_CZ", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_cz_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_CZ(); });
    external_message_c2m_master_cz_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_CZ>(new ExternalMessage::C2M_MASTER_CZ()); });
    external_message_c2m_master_cz_type["dwRoleID"] = sol::property([](const ExternalMessage::C2M_MASTER_CZ& self) { return self.dwRoleID; }, [](ExternalMessage::C2M_MASTER_CZ& self, decltype(ExternalMessage::C2M_MASTER_CZ::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2m_master_cz_type["dwTargetID"] = sol::property([](const ExternalMessage::C2M_MASTER_CZ& self) { return self.dwTargetID; }, [](ExternalMessage::C2M_MASTER_CZ& self, decltype(ExternalMessage::C2M_MASTER_CZ::dwTargetID) value) { self.dwTargetID = value; });
    external_message_c2m_master_cz_type["byCZType"] = sol::property([](const ExternalMessage::C2M_MASTER_CZ& self) { return self.byCZType; }, [](ExternalMessage::C2M_MASTER_CZ& self, decltype(ExternalMessage::C2M_MASTER_CZ::byCZType) value) { self.byCZType = value; });
    external_message_c2m_master_cz_type["byCZLevel"] = sol::property([](const ExternalMessage::C2M_MASTER_CZ& self) { return self.byCZLevel; }, [](ExternalMessage::C2M_MASTER_CZ& self, decltype(ExternalMessage::C2M_MASTER_CZ::byCZLevel) value) { self.byCZLevel = value; });
    external_message_c2m_master_cz_type["szText"] = cqyh::lua::string_property(&ExternalMessage::C2M_MASTER_CZ::szText);
    auto external_message_c2m_master_grad_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_MASTER_GRAD>(L, external_message_namespace, "C2M_MASTER_GRAD", "ExternalMessage::C2M_MASTER_GRAD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_master_grad_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_MASTER_GRAD(); });
    external_message_c2m_master_grad_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_MASTER_GRAD>(new ExternalMessage::C2M_MASTER_GRAD()); });
}

}
