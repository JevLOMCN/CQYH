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

void register_external_manage_to_client_siege_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_enroll_sabac_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ENROLL_SABAC>(L, external_message_namespace, "M2C_ENROLL_SABAC", "ExternalMessage::M2C_ENROLL_SABAC", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_enroll_sabac_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ENROLL_SABAC(); });
    external_message_m2c_enroll_sabac_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ENROLL_SABAC>(new ExternalMessage::M2C_ENROLL_SABAC()); });
    external_message_m2c_enroll_sabac_type["dwGuildID"] = sol::property([](const ExternalMessage::M2C_ENROLL_SABAC& self) { return self.dwGuildID; }, [](ExternalMessage::M2C_ENROLL_SABAC& self, decltype(ExternalMessage::M2C_ENROLL_SABAC::dwGuildID) value) { self.dwGuildID = value; });
    auto external_message_m2c_occupy_sabac_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_OCCUPY_SABAC>(L, external_message_namespace, "M2C_OCCUPY_SABAC", "ExternalMessage::M2C_OCCUPY_SABAC", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_occupy_sabac_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_OCCUPY_SABAC(); });
    external_message_m2c_occupy_sabac_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_OCCUPY_SABAC>(new ExternalMessage::M2C_OCCUPY_SABAC()); });
    external_message_m2c_occupy_sabac_type["dwOldGuildID"] = sol::property([](const ExternalMessage::M2C_OCCUPY_SABAC& self) { return self.dwOldGuildID; }, [](ExternalMessage::M2C_OCCUPY_SABAC& self, decltype(ExternalMessage::M2C_OCCUPY_SABAC::dwOldGuildID) value) { self.dwOldGuildID = value; });
    external_message_m2c_occupy_sabac_type["dwNewGuildID"] = sol::property([](const ExternalMessage::M2C_OCCUPY_SABAC& self) { return self.dwNewGuildID; }, [](ExternalMessage::M2C_OCCUPY_SABAC& self, decltype(ExternalMessage::M2C_OCCUPY_SABAC::dwNewGuildID) value) { self.dwNewGuildID = value; });
    auto external_message_m2c_erect_statue_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_ERECT_STATUE>(L, external_message_namespace, "M2C_ERECT_STATUE", "ExternalMessage::M2C_ERECT_STATUE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_erect_statue_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_ERECT_STATUE(); });
    external_message_m2c_erect_statue_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_ERECT_STATUE>(new ExternalMessage::M2C_ERECT_STATUE()); });
    external_message_m2c_erect_statue_type["dwRoleID"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.dwRoleID; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::dwRoleID) value) { self.dwRoleID = value; });
    external_message_m2c_erect_statue_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_ERECT_STATUE::szRoleName);
    external_message_m2c_erect_statue_type["byClass"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.byClass; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::byClass) value) { self.byClass = value; });
    external_message_m2c_erect_statue_type["bySex"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.bySex; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::bySex) value) { self.bySex = value; });
    external_message_m2c_erect_statue_type["byHairType"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.byHairType; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::byHairType) value) { self.byHairType = value; });
    external_message_m2c_erect_statue_type["byHairColor"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.byHairColor; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::byHairColor) value) { self.byHairColor = value; });
    external_message_m2c_erect_statue_type["byFaceType"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.byFaceType; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::byFaceType) value) { self.byFaceType = value; });
    external_message_m2c_erect_statue_type["bySkinColor"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.bySkinColor; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::bySkinColor) value) { self.bySkinColor = value; });
    external_message_m2c_erect_statue_type["byTitleID"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.byTitleID; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::byTitleID) value) { self.byTitleID = value; });
    external_message_m2c_erect_statue_type["byRefineFx"] = sol::property([](const ExternalMessage::M2C_ERECT_STATUE& self) { return self.byRefineFx; }, [](ExternalMessage::M2C_ERECT_STATUE& self, decltype(ExternalMessage::M2C_ERECT_STATUE::byRefineFx) value) { self.byRefineFx = value; });
    external_message_m2c_erect_statue_type["anAvatar"] = cqyh::lua::array_property(&ExternalMessage::M2C_ERECT_STATUE::anAvatar);
    auto external_message_m2c_sabac_winner_login_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_SABAC_WINNER_LOGIN>(L, external_message_namespace, "M2C_SABAC_WINNER_LOGIN", "ExternalMessage::M2C_SABAC_WINNER_LOGIN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_sabac_winner_login_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_SABAC_WINNER_LOGIN(); });
    external_message_m2c_sabac_winner_login_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_SABAC_WINNER_LOGIN>(new ExternalMessage::M2C_SABAC_WINNER_LOGIN()); });
    external_message_m2c_sabac_winner_login_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::M2C_SABAC_WINNER_LOGIN::szRoleName);
}

}
