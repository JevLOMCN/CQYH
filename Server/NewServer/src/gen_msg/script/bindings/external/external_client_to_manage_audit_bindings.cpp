#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_audit_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_guild_more_history_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_MORE_HISTORY>(L, external_message_namespace, "C2M_GUILD_MORE_HISTORY", "ExternalMessage::C2M_GUILD_MORE_HISTORY", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_more_history_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_MORE_HISTORY(); });
    external_message_c2m_guild_more_history_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_MORE_HISTORY>(new ExternalMessage::C2M_GUILD_MORE_HISTORY()); });
}

}
