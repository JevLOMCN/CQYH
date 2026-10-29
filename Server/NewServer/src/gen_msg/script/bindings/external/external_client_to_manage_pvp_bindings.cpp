#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_pvp_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_join_battle_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_JOIN_BATTLE>(L, external_message_namespace, "C2M_JOIN_BATTLE", "ExternalMessage::C2M_JOIN_BATTLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_join_battle_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_JOIN_BATTLE(); });
    external_message_c2m_join_battle_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_JOIN_BATTLE>(new ExternalMessage::C2M_JOIN_BATTLE()); });
    external_message_c2m_join_battle_type["dwBattleMatch"] = sol::property([](const ExternalMessage::C2M_JOIN_BATTLE& self) { return self.dwBattleMatch; }, [](ExternalMessage::C2M_JOIN_BATTLE& self, decltype(ExternalMessage::C2M_JOIN_BATTLE::dwBattleMatch) value) { self.dwBattleMatch = value; });
    auto external_message_c2m_cancel_join_battle_req_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_CANCEL_JOIN_BATTLE_REQ>(L, external_message_namespace, "C2M_CANCEL_JOIN_BATTLE_REQ", "ExternalMessage::C2M_CANCEL_JOIN_BATTLE_REQ", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_cancel_join_battle_req_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_CANCEL_JOIN_BATTLE_REQ(); });
    external_message_c2m_cancel_join_battle_req_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_CANCEL_JOIN_BATTLE_REQ>(new ExternalMessage::C2M_CANCEL_JOIN_BATTLE_REQ()); });
    auto external_message_c2m_join_battle_confirm_ack_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK>(L, external_message_namespace, "C2M_JOIN_BATTLE_CONFIRM_ACK", "ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_join_battle_confirm_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK(); });
    external_message_c2m_join_battle_confirm_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK>(new ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK()); });
    external_message_c2m_join_battle_confirm_ack_type["byResult"] = sol::property([](const ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK& self) { return self.byResult; }, [](ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK& self, decltype(ExternalMessage::C2M_JOIN_BATTLE_CONFIRM_ACK::byResult) value) { self.byResult = value; });
    auto external_message_c2m_arena_get_data_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_ARENA_GET_DATA>(L, external_message_namespace, "C2M_ARENA_GET_DATA", "ExternalMessage::C2M_ARENA_GET_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_arena_get_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_ARENA_GET_DATA(); });
    external_message_c2m_arena_get_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_ARENA_GET_DATA>(new ExternalMessage::C2M_ARENA_GET_DATA()); });
}

}
