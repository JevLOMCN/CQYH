#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_teams_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_adjacent_role_noteam_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ADJACENT_ROLE_NOTEAM>(L, external_message_namespace, "C2G_ADJACENT_ROLE_NOTEAM", "ExternalMessage::C2G_ADJACENT_ROLE_NOTEAM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_adjacent_role_noteam_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ADJACENT_ROLE_NOTEAM(); });
    external_message_c2g_adjacent_role_noteam_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ADJACENT_ROLE_NOTEAM>(new ExternalMessage::C2G_ADJACENT_ROLE_NOTEAM()); });
    auto external_message_c2g_adjacent_team_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ADJACENT_TEAM>(L, external_message_namespace, "C2G_ADJACENT_TEAM", "ExternalMessage::C2G_ADJACENT_TEAM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_adjacent_team_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ADJACENT_TEAM(); });
    external_message_c2g_adjacent_team_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ADJACENT_TEAM>(new ExternalMessage::C2G_ADJACENT_TEAM()); });
    auto external_message_c2g_recall_team_member_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_RECALL_TEAM_MEMBER>(L, external_message_namespace, "C2G_RECALL_TEAM_MEMBER", "ExternalMessage::C2G_RECALL_TEAM_MEMBER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_recall_team_member_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_RECALL_TEAM_MEMBER(); });
    external_message_c2g_recall_team_member_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_RECALL_TEAM_MEMBER>(new ExternalMessage::C2G_RECALL_TEAM_MEMBER()); });
}

}
