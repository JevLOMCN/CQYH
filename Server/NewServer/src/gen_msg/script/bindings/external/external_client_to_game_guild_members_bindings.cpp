#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_guild_members_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_guild_draw_salary_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_DRAW_SALARY>(L, external_message_namespace, "C2G_GUILD_DRAW_SALARY", "ExternalMessage::C2G_GUILD_DRAW_SALARY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_draw_salary_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_DRAW_SALARY(); });
    external_message_c2g_guild_draw_salary_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_DRAW_SALARY>(new ExternalMessage::C2G_GUILD_DRAW_SALARY()); });
}

}
