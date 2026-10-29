#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_variables_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_update_client_variable_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE>(L, external_message_namespace, "C2G_UPDATE_CLIENT_VARIABLE", "ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_update_client_variable_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE(); });
    external_message_c2g_update_client_variable_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE>(new ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE()); });
    external_message_c2g_update_client_variable_type["byVarIndex"] = sol::property([](const ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE& self) { return self.byVarIndex; }, [](ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE& self, decltype(ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE::byVarIndex) value) { self.byVarIndex = value; });
    external_message_c2g_update_client_variable_type["nVarValue"] = sol::property([](const ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE& self) { return self.nVarValue; }, [](ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE& self, decltype(ExternalMessage::C2G_UPDATE_CLIENT_VARIABLE::nVarValue) value) { self.nVarValue = value; });
}

}
