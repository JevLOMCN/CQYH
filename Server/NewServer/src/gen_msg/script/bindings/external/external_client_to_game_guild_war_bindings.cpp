#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_guild_war_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_guild_reward_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT>(L, external_message_namespace, "C2G_GUILD_REWARD_ACHIEVEMENT", "ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_reward_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT(); });
    external_message_c2g_guild_reward_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT>(new ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT()); });
    external_message_c2g_guild_reward_achievement_type["nID"] = sol::property([](const ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT& self) { return self.nID; }, [](ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT& self, decltype(ExternalMessage::C2G_GUILD_REWARD_ACHIEVEMENT::nID) value) { self.nID = value; });
    auto external_message_c2g_query_guild_war_record_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD>(L, external_message_namespace, "C2G_QUERY_GUILD_WAR_RECORD", "ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_guild_war_record_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD(); });
    external_message_c2g_query_guild_war_record_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD>(new ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD()); });
    external_message_c2g_query_guild_war_record_type["byPage"] = sol::property([](const ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD& self) { return self.byPage; }, [](ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD& self, decltype(ExternalMessage::C2G_QUERY_GUILD_WAR_RECORD::byPage) value) { self.byPage = value; });
}

}
