#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/m2c_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/guild_def.h"
#include "../../../../../../Common/define/ranklist_def.h"
#include "../../../../../../Common/define/string_def.h"
#include "../../../../../../Common/define/team_def.h"
#include "../../../../../../Common/define/variable_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_manage_to_client_mentorship_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_master_twitter_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_TWITTER_ACK>(L, external_message_namespace, "M2C_MASTER_TWITTER_ACK", "ExternalMessage::M2C_MASTER_TWITTER_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_twitter_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_TWITTER_ACK(); });
    external_message_m2c_master_twitter_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_TWITTER_ACK>(new ExternalMessage::M2C_MASTER_TWITTER_ACK()); });
    external_message_m2c_master_twitter_ack_type["byTwitter"] = sol::property([](const ExternalMessage::M2C_MASTER_TWITTER_ACK& self) { return self.byTwitter; }, [](ExternalMessage::M2C_MASTER_TWITTER_ACK& self, decltype(ExternalMessage::M2C_MASTER_TWITTER_ACK::byTwitter) value) { self.byTwitter = value; });
    auto external_message_m2c_master_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_INFO>(L, external_message_namespace, "M2C_MASTER_INFO", "ExternalMessage::M2C_MASTER_INFO", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_INFO(); });
    external_message_m2c_master_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_INFO>(new ExternalMessage::M2C_MASTER_INFO()); });
    external_message_m2c_master_info_type["dwTeacherID"] = sol::property([](const ExternalMessage::M2C_MASTER_INFO& self) { return self.dwTeacherID; }, [](ExternalMessage::M2C_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_INFO::dwTeacherID) value) { self.dwTeacherID = value; });
    external_message_m2c_master_info_type["byTeacherLevel"] = sol::property([](const ExternalMessage::M2C_MASTER_INFO& self) { return self.byTeacherLevel; }, [](ExternalMessage::M2C_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_INFO::byTeacherLevel) value) { self.byTeacherLevel = value; });
    external_message_m2c_master_info_type["byStudentNum"] = sol::property([](const ExternalMessage::M2C_MASTER_INFO& self) { return self.byStudentNum; }, [](ExternalMessage::M2C_MASTER_INFO& self, decltype(ExternalMessage::M2C_MASTER_INFO::byStudentNum) value) { self.byStudentNum = value; });
    external_message_m2c_master_info_type["arrInfo"] = cqyh::lua::array_property(&ExternalMessage::M2C_MASTER_INFO::arrInfo);
    auto external_message_m2c_master_sync_lvl_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_SYNC_LVL>(L, external_message_namespace, "M2C_MASTER_SYNC_LVL", "ExternalMessage::M2C_MASTER_SYNC_LVL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_sync_lvl_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_SYNC_LVL(); });
    external_message_m2c_master_sync_lvl_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_SYNC_LVL>(new ExternalMessage::M2C_MASTER_SYNC_LVL()); });
    external_message_m2c_master_sync_lvl_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_MASTER_SYNC_LVL& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_MASTER_SYNC_LVL& self, decltype(ExternalMessage::M2C_MASTER_SYNC_LVL::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_master_sync_lvl_type["byLevel"] = sol::property([](const ExternalMessage::M2C_MASTER_SYNC_LVL& self) { return self.byLevel; }, [](ExternalMessage::M2C_MASTER_SYNC_LVL& self, decltype(ExternalMessage::M2C_MASTER_SYNC_LVL::byLevel) value) { self.byLevel = value; });
    auto external_message_m2c_master_clear_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_CLEAR_INFO>(L, external_message_namespace, "M2C_MASTER_CLEAR_INFO", "ExternalMessage::M2C_MASTER_CLEAR_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_clear_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_CLEAR_INFO(); });
    external_message_m2c_master_clear_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_CLEAR_INFO>(new ExternalMessage::M2C_MASTER_CLEAR_INFO()); });
    auto external_message_m2c_master_cz_level_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_CZ_LEVEL>(L, external_message_namespace, "M2C_MASTER_CZ_LEVEL", "ExternalMessage::M2C_MASTER_CZ_LEVEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_cz_level_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_CZ_LEVEL(); });
    external_message_m2c_master_cz_level_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_CZ_LEVEL>(new ExternalMessage::M2C_MASTER_CZ_LEVEL()); });
    external_message_m2c_master_cz_level_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_MASTER_CZ_LEVEL& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_MASTER_CZ_LEVEL& self, decltype(ExternalMessage::M2C_MASTER_CZ_LEVEL::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_master_cz_level_type["byCZLevel"] = sol::property([](const ExternalMessage::M2C_MASTER_CZ_LEVEL& self) { return self.byCZLevel; }, [](ExternalMessage::M2C_MASTER_CZ_LEVEL& self, decltype(ExternalMessage::M2C_MASTER_CZ_LEVEL::byCZLevel) value) { self.byCZLevel = value; });
    auto external_message_m2c_master_cz_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_CZ_ACK>(L, external_message_namespace, "M2C_MASTER_CZ_ACK", "ExternalMessage::M2C_MASTER_CZ_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_cz_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_CZ_ACK(); });
    external_message_m2c_master_cz_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_CZ_ACK>(new ExternalMessage::M2C_MASTER_CZ_ACK()); });
    external_message_m2c_master_cz_ack_type["dwTargetRoleID"] = sol::property([](const ExternalMessage::M2C_MASTER_CZ_ACK& self) { return self.dwTargetRoleID; }, [](ExternalMessage::M2C_MASTER_CZ_ACK& self, decltype(ExternalMessage::M2C_MASTER_CZ_ACK::dwTargetRoleID) value) { self.dwTargetRoleID = value; });
    auto external_message_m2c_master_grad_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_MASTER_GRAD>(L, external_message_namespace, "M2C_MASTER_GRAD", "ExternalMessage::M2C_MASTER_GRAD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_master_grad_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_MASTER_GRAD(); });
    external_message_m2c_master_grad_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_MASTER_GRAD>(new ExternalMessage::M2C_MASTER_GRAD()); });
    external_message_m2c_master_grad_type["dwStudentID"] = sol::property([](const ExternalMessage::M2C_MASTER_GRAD& self) { return self.dwStudentID; }, [](ExternalMessage::M2C_MASTER_GRAD& self, decltype(ExternalMessage::M2C_MASTER_GRAD::dwStudentID) value) { self.dwStudentID = value; });
}

}
