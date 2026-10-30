#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_attributes_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_query_hero_attr_list_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_HERO_ATTR_LIST>(L, external_message_namespace, "C2G_QUERY_HERO_ATTR_LIST", "ExternalMessage::C2G_QUERY_HERO_ATTR_LIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_hero_attr_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_HERO_ATTR_LIST(); });
    external_message_c2g_query_hero_attr_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_HERO_ATTR_LIST>(new ExternalMessage::C2G_QUERY_HERO_ATTR_LIST()); });
    auto external_message_c2g_query_combat_stat_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_COMBAT_STAT>(L, external_message_namespace, "C2G_QUERY_COMBAT_STAT", "ExternalMessage::C2G_QUERY_COMBAT_STAT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_combat_stat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_COMBAT_STAT(); });
    external_message_c2g_query_combat_stat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_COMBAT_STAT>(new ExternalMessage::C2G_QUERY_COMBAT_STAT()); });
}

}
