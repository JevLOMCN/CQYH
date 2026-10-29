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

void register_external_manage_to_client_guild_war_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_guild_battle_achievement_data_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA>(L, external_message_namespace, "GUILD_BATTLE_ACHIEVEMENT_DATA", "ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA");
    external_message_guild_battle_achievement_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA(); });
    external_message_guild_battle_achievement_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA>(new ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA()); });
    external_message_guild_battle_achievement_data_type["dwRoleID"] = sol::property([](const ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self) { return self.dwRoleID; }, [](ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self, decltype(ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA::dwRoleID) value) { self.dwRoleID = value; });
    external_message_guild_battle_achievement_data_type["nKillNum"] = sol::property([](const ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self) { return self.nKillNum; }, [](ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self, decltype(ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA::nKillNum) value) { self.nKillNum = value; });
    external_message_guild_battle_achievement_data_type["nDealCount"] = sol::property([](const ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self) { return self.nDealCount; }, [](ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self, decltype(ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA::nDealCount) value) { self.nDealCount = value; });
    external_message_guild_battle_achievement_data_type["nSalary"] = sol::property([](const ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self) { return self.nSalary; }, [](ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA& self, decltype(ExternalMessage::GUILD_BATTLE_ACHIEVEMENT_DATA::nSalary) value) { self.nSalary = value; });
    auto external_message_m2c_guild_battle_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT>(L, external_message_namespace, "M2C_GUILD_BATTLE_ACHIEVEMENT", "ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_battle_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT(); });
    external_message_m2c_guild_battle_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT>(new ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT()); });
    external_message_m2c_guild_battle_achievement_type["byNum"] = sol::property([](const ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT& self) { return self.byNum; }, [](ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT& self, decltype(ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT::byNum) value) { self.byNum = value; });
    external_message_m2c_guild_battle_achievement_type["info"] = cqyh::lua::array_property(&ExternalMessage::M2C_GUILD_BATTLE_ACHIEVEMENT::info);
    auto external_message_m2c_guild_sync_war_cd_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF>(L, external_message_namespace, "M2C_GUILD_SYNC_WAR_CD_NTF", "ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_sync_war_cd_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF(); });
    external_message_m2c_guild_sync_war_cd_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF>(new ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF()); });
    external_message_m2c_guild_sync_war_cd_ntf_type["dwWarCD"] = sol::property([](const ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF& self) { return self.dwWarCD; }, [](ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF& self, decltype(ExternalMessage::M2C_GUILD_SYNC_WAR_CD_NTF::dwWarCD) value) { self.dwWarCD = value; });
    auto external_message_m2c_guild_join_war_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_JOIN_WAR_NTF>(L, external_message_namespace, "M2C_GUILD_JOIN_WAR_NTF", "ExternalMessage::M2C_GUILD_JOIN_WAR_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_join_war_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_JOIN_WAR_NTF(); });
    external_message_m2c_guild_join_war_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_JOIN_WAR_NTF>(new ExternalMessage::M2C_GUILD_JOIN_WAR_NTF()); });
    external_message_m2c_guild_join_war_ntf_type["byWarCharacter"] = sol::property([](const ExternalMessage::M2C_GUILD_JOIN_WAR_NTF& self) { return self.byWarCharacter; }, [](ExternalMessage::M2C_GUILD_JOIN_WAR_NTF& self, decltype(ExternalMessage::M2C_GUILD_JOIN_WAR_NTF::byWarCharacter) value) { self.byWarCharacter = value; });
    external_message_m2c_guild_join_war_ntf_type["dwTargetGuildID"] = sol::property([](const ExternalMessage::M2C_GUILD_JOIN_WAR_NTF& self) { return self.dwTargetGuildID; }, [](ExternalMessage::M2C_GUILD_JOIN_WAR_NTF& self, decltype(ExternalMessage::M2C_GUILD_JOIN_WAR_NTF::dwTargetGuildID) value) { self.dwTargetGuildID = value; });
    auto external_message_guild_war_record_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_WAR_RECORD_INFO>(L, external_message_namespace, "GUILD_WAR_RECORD_INFO", "ExternalMessage::GUILD_WAR_RECORD_INFO");
    external_message_guild_war_record_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_WAR_RECORD_INFO(); });
    external_message_guild_war_record_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_WAR_RECORD_INFO>(new ExternalMessage::GUILD_WAR_RECORD_INFO()); });
    external_message_guild_war_record_info_type["dwBeginDayTime"] = sol::property([](const ExternalMessage::GUILD_WAR_RECORD_INFO& self) { return self.dwBeginDayTime; }, [](ExternalMessage::GUILD_WAR_RECORD_INFO& self, decltype(ExternalMessage::GUILD_WAR_RECORD_INFO::dwBeginDayTime) value) { self.dwBeginDayTime = value; });
    external_message_guild_war_record_info_type["dwIntruderID"] = sol::property([](const ExternalMessage::GUILD_WAR_RECORD_INFO& self) { return self.dwIntruderID; }, [](ExternalMessage::GUILD_WAR_RECORD_INFO& self, decltype(ExternalMessage::GUILD_WAR_RECORD_INFO::dwIntruderID) value) { self.dwIntruderID = value; });
    external_message_guild_war_record_info_type["dwDefenderID"] = sol::property([](const ExternalMessage::GUILD_WAR_RECORD_INFO& self) { return self.dwDefenderID; }, [](ExternalMessage::GUILD_WAR_RECORD_INFO& self, decltype(ExternalMessage::GUILD_WAR_RECORD_INFO::dwDefenderID) value) { self.dwDefenderID = value; });
    external_message_guild_war_record_info_type["byWarRes"] = sol::property([](const ExternalMessage::GUILD_WAR_RECORD_INFO& self) { return self.byWarRes; }, [](ExternalMessage::GUILD_WAR_RECORD_INFO& self, decltype(ExternalMessage::GUILD_WAR_RECORD_INFO::byWarRes) value) { self.byWarRes = value; });
    auto external_message_m2c_guild_war_record_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_WAR_RECORD>(L, external_message_namespace, "M2C_GUILD_WAR_RECORD", "ExternalMessage::M2C_GUILD_WAR_RECORD", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_war_record_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_WAR_RECORD(); });
    external_message_m2c_guild_war_record_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_WAR_RECORD>(new ExternalMessage::M2C_GUILD_WAR_RECORD()); });
    external_message_m2c_guild_war_record_type["wCount"] = sol::property([](const ExternalMessage::M2C_GUILD_WAR_RECORD& self) { return self.wCount; }, [](ExternalMessage::M2C_GUILD_WAR_RECORD& self, decltype(ExternalMessage::M2C_GUILD_WAR_RECORD::wCount) value) { self.wCount = value; });
    external_message_m2c_guild_war_record_type["info"] = cqyh::lua::array_property(&ExternalMessage::M2C_GUILD_WAR_RECORD::info);
}

}
