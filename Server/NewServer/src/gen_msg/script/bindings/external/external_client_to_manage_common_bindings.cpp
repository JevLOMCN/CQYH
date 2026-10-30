#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_common_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_change_info_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_CHANGE_INFO>(L, external_message_namespace, "C2M_CHANGE_INFO", "ExternalMessage::C2M_CHANGE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_change_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_CHANGE_INFO(); });
    external_message_c2m_change_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_CHANGE_INFO>(new ExternalMessage::C2M_CHANGE_INFO()); });
    external_message_c2m_change_info_type["szSignature"] = cqyh::lua::string_property(&ExternalMessage::C2M_CHANGE_INFO::szSignature);
    external_message_c2m_change_info_type["byFriendState"] = sol::property([](const ExternalMessage::C2M_CHANGE_INFO& self) { return self.byFriendState; }, [](ExternalMessage::C2M_CHANGE_INFO& self, decltype(ExternalMessage::C2M_CHANGE_INFO::byFriendState) value) { self.byFriendState = value; });
    auto external_message_c2m_query_ranklist_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_QUERY_RANKLIST>(L, external_message_namespace, "C2M_QUERY_RANKLIST", "ExternalMessage::C2M_QUERY_RANKLIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_query_ranklist_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_QUERY_RANKLIST(); });
    external_message_c2m_query_ranklist_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_QUERY_RANKLIST>(new ExternalMessage::C2M_QUERY_RANKLIST()); });
    external_message_c2m_query_ranklist_type["nRanklistID"] = sol::property([](const ExternalMessage::C2M_QUERY_RANKLIST& self) { return self.nRanklistID; }, [](ExternalMessage::C2M_QUERY_RANKLIST& self, decltype(ExternalMessage::C2M_QUERY_RANKLIST::nRanklistID) value) { self.nRanklistID = value; });
    external_message_c2m_query_ranklist_type["nPageSize"] = sol::property([](const ExternalMessage::C2M_QUERY_RANKLIST& self) { return self.nPageSize; }, [](ExternalMessage::C2M_QUERY_RANKLIST& self, decltype(ExternalMessage::C2M_QUERY_RANKLIST::nPageSize) value) { self.nPageSize = value; });
    auto external_message_c2m_change_recall_state_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_CHANGE_RECALL_STATE>(L, external_message_namespace, "C2M_CHANGE_RECALL_STATE", "ExternalMessage::C2M_CHANGE_RECALL_STATE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_change_recall_state_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_CHANGE_RECALL_STATE(); });
    external_message_c2m_change_recall_state_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_CHANGE_RECALL_STATE>(new ExternalMessage::C2M_CHANGE_RECALL_STATE()); });
    external_message_c2m_change_recall_state_type["byEnabled"] = sol::property([](const ExternalMessage::C2M_CHANGE_RECALL_STATE& self) { return self.byEnabled; }, [](ExternalMessage::C2M_CHANGE_RECALL_STATE& self, decltype(ExternalMessage::C2M_CHANGE_RECALL_STATE::byEnabled) value) { self.byEnabled = value; });
}

}
