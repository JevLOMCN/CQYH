#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/g2c_message.h"
#include "../../../../../../Common/define/dds_def.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/item_def.h"
#include "../../../../../../Common/define/master_def.h"
#include "../../../../../../Common/define/npc_def.h"
#include "../../../../../../Common/define/quest_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_game_to_client_session_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_gpk_dyn_code_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GPK_DYN_CODE>(L, external_message_namespace, "G2C_GPK_DYN_CODE", "ExternalMessage::G2C_GPK_DYN_CODE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_gpk_dyn_code_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GPK_DYN_CODE(); });
    external_message_g2c_gpk_dyn_code_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GPK_DYN_CODE>(new ExternalMessage::G2C_GPK_DYN_CODE()); });
    external_message_g2c_gpk_dyn_code_type["DynCode"] = cqyh::lua::string_property(&ExternalMessage::G2C_GPK_DYN_CODE::DynCode);
    auto external_message_g2c_gpk_auth_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GPK_AUTH_DATA>(L, external_message_namespace, "G2C_GPK_AUTH_DATA", "ExternalMessage::G2C_GPK_AUTH_DATA", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_gpk_auth_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GPK_AUTH_DATA(); });
    external_message_g2c_gpk_auth_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GPK_AUTH_DATA>(new ExternalMessage::G2C_GPK_AUTH_DATA()); });
    external_message_g2c_gpk_auth_data_type["AuthData"] = cqyh::lua::string_property(&ExternalMessage::G2C_GPK_AUTH_DATA::AuthData);
    auto external_message_g2c_return_select_role_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RETURN_SELECT_ROLE>(L, external_message_namespace, "G2C_RETURN_SELECT_ROLE", "ExternalMessage::G2C_RETURN_SELECT_ROLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_return_select_role_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RETURN_SELECT_ROLE(); });
    external_message_g2c_return_select_role_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RETURN_SELECT_ROLE>(new ExternalMessage::G2C_RETURN_SELECT_ROLE()); });
    external_message_g2c_return_select_role_type["nErrCode"] = sol::property([](const ExternalMessage::G2C_RETURN_SELECT_ROLE& self) { return self.nErrCode; }, [](ExternalMessage::G2C_RETURN_SELECT_ROLE& self, decltype(ExternalMessage::G2C_RETURN_SELECT_ROLE::nErrCode) value) { self.nErrCode = value; });
    external_message_g2c_return_select_role_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_RETURN_SELECT_ROLE& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_RETURN_SELECT_ROLE& self, decltype(ExternalMessage::G2C_RETURN_SELECT_ROLE::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_g2c_logout_timer_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_LOGOUT_TIMER>(L, external_message_namespace, "G2C_LOGOUT_TIMER", "ExternalMessage::G2C_LOGOUT_TIMER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_logout_timer_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_LOGOUT_TIMER(); });
    external_message_g2c_logout_timer_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_LOGOUT_TIMER>(new ExternalMessage::G2C_LOGOUT_TIMER()); });
    external_message_g2c_logout_timer_type["byStart"] = sol::property([](const ExternalMessage::G2C_LOGOUT_TIMER& self) { return self.byStart; }, [](ExternalMessage::G2C_LOGOUT_TIMER& self, decltype(ExternalMessage::G2C_LOGOUT_TIMER::byStart) value) { self.byStart = value; });
    external_message_g2c_logout_timer_type["byDelayTime"] = sol::property([](const ExternalMessage::G2C_LOGOUT_TIMER& self) { return self.byDelayTime; }, [](ExternalMessage::G2C_LOGOUT_TIMER& self, decltype(ExternalMessage::G2C_LOGOUT_TIMER::byDelayTime) value) { self.byDelayTime = value; });
    auto external_message_g2c_sync_vip_state_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_VIP_STATE>(L, external_message_namespace, "G2C_SYNC_VIP_STATE", "ExternalMessage::G2C_SYNC_VIP_STATE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_vip_state_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_VIP_STATE(); });
    external_message_g2c_sync_vip_state_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_VIP_STATE>(new ExternalMessage::G2C_SYNC_VIP_STATE()); });
    external_message_g2c_sync_vip_state_type["dwVIPTime"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_VIP_STATE::dwVIPTime);
    auto external_message_g2c_sync_vip_level_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_VIP_LEVEL>(L, external_message_namespace, "G2C_SYNC_VIP_LEVEL", "ExternalMessage::G2C_SYNC_VIP_LEVEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_vip_level_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_VIP_LEVEL(); });
    external_message_g2c_sync_vip_level_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_VIP_LEVEL>(new ExternalMessage::G2C_SYNC_VIP_LEVEL()); });
    external_message_g2c_sync_vip_level_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_VIP_LEVEL& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_VIP_LEVEL& self, decltype(ExternalMessage::G2C_SYNC_VIP_LEVEL::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_vip_level_type["byVIPLevel"] = sol::property([](const ExternalMessage::G2C_SYNC_VIP_LEVEL& self) { return self.byVIPLevel; }, [](ExternalMessage::G2C_SYNC_VIP_LEVEL& self, decltype(ExternalMessage::G2C_SYNC_VIP_LEVEL::byVIPLevel) value) { self.byVIPLevel = value; });
}

}
