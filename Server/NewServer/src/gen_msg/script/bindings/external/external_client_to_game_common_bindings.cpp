#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_common_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_loadfinish_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LOADFINISH>(L, external_message_namespace, "C2G_LOADFINISH", "ExternalMessage::C2G_LOADFINISH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_loadfinish_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LOADFINISH(); });
    external_message_c2g_loadfinish_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LOADFINISH>(new ExternalMessage::C2G_LOADFINISH()); });
    external_message_c2g_loadfinish_type["nErrCode"] = sol::property([](const ExternalMessage::C2G_LOADFINISH& self) { return self.nErrCode; }, [](ExternalMessage::C2G_LOADFINISH& self, decltype(ExternalMessage::C2G_LOADFINISH::nErrCode) value) { self.nErrCode = value; });
    auto external_message_c2g_select_dialog_option_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SELECT_DIALOG_OPTION>(L, external_message_namespace, "C2G_SELECT_DIALOG_OPTION", "ExternalMessage::C2G_SELECT_DIALOG_OPTION", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_select_dialog_option_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SELECT_DIALOG_OPTION(); });
    external_message_c2g_select_dialog_option_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SELECT_DIALOG_OPTION>(new ExternalMessage::C2G_SELECT_DIALOG_OPTION()); });
    external_message_c2g_select_dialog_option_type["nParam"] = sol::property([](const ExternalMessage::C2G_SELECT_DIALOG_OPTION& self) { return self.nParam; }, [](ExternalMessage::C2G_SELECT_DIALOG_OPTION& self, decltype(ExternalMessage::C2G_SELECT_DIALOG_OPTION::nParam) value) { self.nParam = value; });
    auto external_message_c2g_revive_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_REVIVE>(L, external_message_namespace, "C2G_REVIVE", "ExternalMessage::C2G_REVIVE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_revive_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_REVIVE(); });
    external_message_c2g_revive_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_REVIVE>(new ExternalMessage::C2G_REVIVE()); });
    external_message_c2g_revive_type["byType"] = sol::property([](const ExternalMessage::C2G_REVIVE& self) { return self.byType; }, [](ExternalMessage::C2G_REVIVE& self, decltype(ExternalMessage::C2G_REVIVE::byType) value) { self.byType = value; });
    auto external_message_c2g_lovers_teleprot_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LOVERS_TELEPROT>(L, external_message_namespace, "C2G_LOVERS_TELEPROT", "ExternalMessage::C2G_LOVERS_TELEPROT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_lovers_teleprot_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LOVERS_TELEPROT(); });
    external_message_c2g_lovers_teleprot_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LOVERS_TELEPROT>(new ExternalMessage::C2G_LOVERS_TELEPROT()); });
}

}
