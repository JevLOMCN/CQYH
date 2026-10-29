#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_auras_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_cancel_aura_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CANCEL_AURA>(L, external_message_namespace, "C2G_CANCEL_AURA", "ExternalMessage::C2G_CANCEL_AURA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_cancel_aura_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CANCEL_AURA(); });
    external_message_c2g_cancel_aura_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CANCEL_AURA>(new ExternalMessage::C2G_CANCEL_AURA()); });
    external_message_c2g_cancel_aura_type["dwInstanceID"] = sol::property([](const ExternalMessage::C2G_CANCEL_AURA& self) { return self.dwInstanceID; }, [](ExternalMessage::C2G_CANCEL_AURA& self, decltype(ExternalMessage::C2G_CANCEL_AURA::dwInstanceID) value) { self.dwInstanceID = value; });
}

}
