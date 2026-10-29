#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_titles_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_title_equip_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TITLE_EQUIP>(L, external_message_namespace, "C2G_TITLE_EQUIP", "ExternalMessage::C2G_TITLE_EQUIP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_title_equip_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TITLE_EQUIP(); });
    external_message_c2g_title_equip_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TITLE_EQUIP>(new ExternalMessage::C2G_TITLE_EQUIP()); });
    external_message_c2g_title_equip_type["nTemplateTitleID"] = sol::property([](const ExternalMessage::C2G_TITLE_EQUIP& self) { return self.nTemplateTitleID; }, [](ExternalMessage::C2G_TITLE_EQUIP& self, decltype(ExternalMessage::C2G_TITLE_EQUIP::nTemplateTitleID) value) { self.nTemplateTitleID = value; });
    auto external_message_c2g_title_unequip_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TITLE_UNEQUIP>(L, external_message_namespace, "C2G_TITLE_UNEQUIP", "ExternalMessage::C2G_TITLE_UNEQUIP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_title_unequip_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TITLE_UNEQUIP(); });
    external_message_c2g_title_unequip_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TITLE_UNEQUIP>(new ExternalMessage::C2G_TITLE_UNEQUIP()); });
}

}
