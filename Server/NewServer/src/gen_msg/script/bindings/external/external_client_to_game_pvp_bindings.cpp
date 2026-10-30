#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_pvp_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_change_pk_mode_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHANGE_PK_MODE>(L, external_message_namespace, "C2G_CHANGE_PK_MODE", "ExternalMessage::C2G_CHANGE_PK_MODE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_change_pk_mode_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHANGE_PK_MODE(); });
    external_message_c2g_change_pk_mode_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHANGE_PK_MODE>(new ExternalMessage::C2G_CHANGE_PK_MODE()); });
    external_message_c2g_change_pk_mode_type["byMode"] = sol::property([](const ExternalMessage::C2G_CHANGE_PK_MODE& self) { return self.byMode; }, [](ExternalMessage::C2G_CHANGE_PK_MODE& self, decltype(ExternalMessage::C2G_CHANGE_PK_MODE::byMode) value) { self.byMode = value; });
    auto external_message_c2g_arena_join_season_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ARENA_JOIN_SEASON>(L, external_message_namespace, "C2G_ARENA_JOIN_SEASON", "ExternalMessage::C2G_ARENA_JOIN_SEASON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_arena_join_season_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ARENA_JOIN_SEASON(); });
    external_message_c2g_arena_join_season_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ARENA_JOIN_SEASON>(new ExternalMessage::C2G_ARENA_JOIN_SEASON()); });
    auto external_message_c2g_arena_quit_season_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ARENA_QUIT_SEASON>(L, external_message_namespace, "C2G_ARENA_QUIT_SEASON", "ExternalMessage::C2G_ARENA_QUIT_SEASON", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_arena_quit_season_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ARENA_QUIT_SEASON(); });
    external_message_c2g_arena_quit_season_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ARENA_QUIT_SEASON>(new ExternalMessage::C2G_ARENA_QUIT_SEASON()); });
    auto external_message_c2g_arena_match_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ARENA_MATCH>(L, external_message_namespace, "C2G_ARENA_MATCH", "ExternalMessage::C2G_ARENA_MATCH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_arena_match_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ARENA_MATCH(); });
    external_message_c2g_arena_match_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ARENA_MATCH>(new ExternalMessage::C2G_ARENA_MATCH()); });
    auto external_message_c2g_arena_get_reward_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ARENA_GET_REWARD>(L, external_message_namespace, "C2G_ARENA_GET_REWARD", "ExternalMessage::C2G_ARENA_GET_REWARD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_arena_get_reward_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ARENA_GET_REWARD(); });
    external_message_c2g_arena_get_reward_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ARENA_GET_REWARD>(new ExternalMessage::C2G_ARENA_GET_REWARD()); });
}

}
