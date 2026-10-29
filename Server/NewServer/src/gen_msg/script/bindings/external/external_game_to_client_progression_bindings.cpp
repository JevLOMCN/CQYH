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

void register_external_game_to_client_progression_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_role_level_up_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ROLE_LEVEL_UP>(L, external_message_namespace, "G2C_ROLE_LEVEL_UP", "ExternalMessage::G2C_ROLE_LEVEL_UP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_role_level_up_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ROLE_LEVEL_UP(); });
    external_message_g2c_role_level_up_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ROLE_LEVEL_UP>(new ExternalMessage::G2C_ROLE_LEVEL_UP()); });
    external_message_g2c_role_level_up_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_ROLE_LEVEL_UP& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_ROLE_LEVEL_UP& self, decltype(ExternalMessage::G2C_ROLE_LEVEL_UP::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_role_level_up_type["byLevel"] = sol::property([](const ExternalMessage::G2C_ROLE_LEVEL_UP& self) { return self.byLevel; }, [](ExternalMessage::G2C_ROLE_LEVEL_UP& self, decltype(ExternalMessage::G2C_ROLE_LEVEL_UP::byLevel) value) { self.byLevel = value; });
    auto external_message_g2c_exp_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_EXP_CHANGE>(L, external_message_namespace, "G2C_EXP_CHANGE", "ExternalMessage::G2C_EXP_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_exp_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_EXP_CHANGE(); });
    external_message_g2c_exp_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_EXP_CHANGE>(new ExternalMessage::G2C_EXP_CHANGE()); });
    external_message_g2c_exp_change_type["nExp"] = sol::property([](const ExternalMessage::G2C_EXP_CHANGE& self) { return self.nExp; }, [](ExternalMessage::G2C_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXP_CHANGE::nExp) value) { self.nExp = value; });
    external_message_g2c_exp_change_type["nExpChange"] = sol::property([](const ExternalMessage::G2C_EXP_CHANGE& self) { return self.nExpChange; }, [](ExternalMessage::G2C_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXP_CHANGE::nExpChange) value) { self.nExpChange = value; });
    external_message_g2c_exp_change_type["nMaxExp"] = sol::property([](const ExternalMessage::G2C_EXP_CHANGE& self) { return self.nMaxExp; }, [](ExternalMessage::G2C_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXP_CHANGE::nMaxExp) value) { self.nMaxExp = value; });
    external_message_g2c_exp_change_type["nTodayExp"] = sol::property([](const ExternalMessage::G2C_EXP_CHANGE& self) { return self.nTodayExp; }, [](ExternalMessage::G2C_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXP_CHANGE::nTodayExp) value) { self.nTodayExp = value; });
    external_message_g2c_exp_change_type["nTodayMaxExp"] = sol::property([](const ExternalMessage::G2C_EXP_CHANGE& self) { return self.nTodayMaxExp; }, [](ExternalMessage::G2C_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXP_CHANGE::nTodayMaxExp) value) { self.nTodayMaxExp = value; });
    external_message_g2c_exp_change_type["nExtraExp"] = sol::property([](const ExternalMessage::G2C_EXP_CHANGE& self) { return self.nExtraExp; }, [](ExternalMessage::G2C_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXP_CHANGE::nExtraExp) value) { self.nExtraExp = value; });
    auto external_message_g2c_extra_exp_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_EXTRA_EXP_CHANGE>(L, external_message_namespace, "G2C_EXTRA_EXP_CHANGE", "ExternalMessage::G2C_EXTRA_EXP_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_extra_exp_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_EXTRA_EXP_CHANGE(); });
    external_message_g2c_extra_exp_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_EXTRA_EXP_CHANGE>(new ExternalMessage::G2C_EXTRA_EXP_CHANGE()); });
    external_message_g2c_extra_exp_change_type["nExtraExp"] = sol::property([](const ExternalMessage::G2C_EXTRA_EXP_CHANGE& self) { return self.nExtraExp; }, [](ExternalMessage::G2C_EXTRA_EXP_CHANGE& self, decltype(ExternalMessage::G2C_EXTRA_EXP_CHANGE::nExtraExp) value) { self.nExtraExp = value; });
    auto external_message_g2c_spell_level_up_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPELL_LEVEL_UP>(L, external_message_namespace, "G2C_SPELL_LEVEL_UP", "ExternalMessage::G2C_SPELL_LEVEL_UP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spell_level_up_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPELL_LEVEL_UP(); });
    external_message_g2c_spell_level_up_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPELL_LEVEL_UP>(new ExternalMessage::G2C_SPELL_LEVEL_UP()); });
    external_message_g2c_spell_level_up_type["wSpellID"] = sol::property([](const ExternalMessage::G2C_SPELL_LEVEL_UP& self) { return self.wSpellID; }, [](ExternalMessage::G2C_SPELL_LEVEL_UP& self, decltype(ExternalMessage::G2C_SPELL_LEVEL_UP::wSpellID) value) { self.wSpellID = value; });
    external_message_g2c_spell_level_up_type["byLevel"] = sol::property([](const ExternalMessage::G2C_SPELL_LEVEL_UP& self) { return self.byLevel; }, [](ExternalMessage::G2C_SPELL_LEVEL_UP& self, decltype(ExternalMessage::G2C_SPELL_LEVEL_UP::byLevel) value) { self.byLevel = value; });
    auto external_message_g2c_hero_exp_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_HERO_EXP_CHANGE>(L, external_message_namespace, "G2C_HERO_EXP_CHANGE", "ExternalMessage::G2C_HERO_EXP_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_hero_exp_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_HERO_EXP_CHANGE(); });
    external_message_g2c_hero_exp_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_HERO_EXP_CHANGE>(new ExternalMessage::G2C_HERO_EXP_CHANGE()); });
    external_message_g2c_hero_exp_change_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_HERO_EXP_CHANGE& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_HERO_EXP_CHANGE& self, decltype(ExternalMessage::G2C_HERO_EXP_CHANGE::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_hero_exp_change_type["dwExp"] = sol::property([](const ExternalMessage::G2C_HERO_EXP_CHANGE& self) { return self.dwExp; }, [](ExternalMessage::G2C_HERO_EXP_CHANGE& self, decltype(ExternalMessage::G2C_HERO_EXP_CHANGE::dwExp) value) { self.dwExp = value; });
    external_message_g2c_hero_exp_change_type["nExpChange"] = sol::property([](const ExternalMessage::G2C_HERO_EXP_CHANGE& self) { return self.nExpChange; }, [](ExternalMessage::G2C_HERO_EXP_CHANGE& self, decltype(ExternalMessage::G2C_HERO_EXP_CHANGE::nExpChange) value) { self.nExpChange = value; });
    external_message_g2c_hero_exp_change_type["dwMaxExp"] = sol::property([](const ExternalMessage::G2C_HERO_EXP_CHANGE& self) { return self.dwMaxExp; }, [](ExternalMessage::G2C_HERO_EXP_CHANGE& self, decltype(ExternalMessage::G2C_HERO_EXP_CHANGE::dwMaxExp) value) { self.dwMaxExp = value; });
    auto external_message_g2c_hero_level_up_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_HERO_LEVEL_UP>(L, external_message_namespace, "G2C_HERO_LEVEL_UP", "ExternalMessage::G2C_HERO_LEVEL_UP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_hero_level_up_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_HERO_LEVEL_UP(); });
    external_message_g2c_hero_level_up_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_HERO_LEVEL_UP>(new ExternalMessage::G2C_HERO_LEVEL_UP()); });
    external_message_g2c_hero_level_up_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_HERO_LEVEL_UP& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_HERO_LEVEL_UP& self, decltype(ExternalMessage::G2C_HERO_LEVEL_UP::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_hero_level_up_type["byLevel"] = sol::property([](const ExternalMessage::G2C_HERO_LEVEL_UP& self) { return self.byLevel; }, [](ExternalMessage::G2C_HERO_LEVEL_UP& self, decltype(ExternalMessage::G2C_HERO_LEVEL_UP::byLevel) value) { self.byLevel = value; });
    auto external_message_g2c_spirit_exp_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_EXP_CHANGE>(L, external_message_namespace, "G2C_SPIRIT_EXP_CHANGE", "ExternalMessage::G2C_SPIRIT_EXP_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_exp_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_EXP_CHANGE(); });
    external_message_g2c_spirit_exp_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_EXP_CHANGE>(new ExternalMessage::G2C_SPIRIT_EXP_CHANGE()); });
    external_message_g2c_spirit_exp_change_type["nExp"] = sol::property([](const ExternalMessage::G2C_SPIRIT_EXP_CHANGE& self) { return self.nExp; }, [](ExternalMessage::G2C_SPIRIT_EXP_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_EXP_CHANGE::nExp) value) { self.nExp = value; });
    auto external_message_g2c_spirit_notify_exp_change_time_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME>(L, external_message_namespace, "G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME", "ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_notify_exp_change_time_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME(); });
    external_message_g2c_spirit_notify_exp_change_time_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME>(new ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME()); });
    external_message_g2c_spirit_notify_exp_change_time_type["nRemainTime"] = sol::property([](const ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME& self) { return self.nRemainTime; }, [](ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME& self, decltype(ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME::nRemainTime) value) { self.nRemainTime = value; });
    external_message_g2c_spirit_notify_exp_change_time_type["nResultCode"] = sol::property([](const ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME& self) { return self.nResultCode; }, [](ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME& self, decltype(ExternalMessage::G2C_SPIRIT_NOTIFY_EXP_CHANGE_TIME::nResultCode) value) { self.nResultCode = value; });
    auto external_message_g2c_exercise_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_EXERCISE_INFO>(L, external_message_namespace, "G2C_EXERCISE_INFO", "ExternalMessage::G2C_EXERCISE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_exercise_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_EXERCISE_INFO(); });
    external_message_g2c_exercise_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_EXERCISE_INFO>(new ExternalMessage::G2C_EXERCISE_INFO()); });
    external_message_g2c_exercise_info_type["nTotalHour"] = sol::property([](const ExternalMessage::G2C_EXERCISE_INFO& self) { return self.nTotalHour; }, [](ExternalMessage::G2C_EXERCISE_INFO& self, decltype(ExternalMessage::G2C_EXERCISE_INFO::nTotalHour) value) { self.nTotalHour = value; });
    external_message_g2c_exercise_info_type["nCurHour"] = sol::property([](const ExternalMessage::G2C_EXERCISE_INFO& self) { return self.nCurHour; }, [](ExternalMessage::G2C_EXERCISE_INFO& self, decltype(ExternalMessage::G2C_EXERCISE_INFO::nCurHour) value) { self.nCurHour = value; });
    auto external_message_g2c_add_exercise_time_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ADD_EXERCISE_TIME>(L, external_message_namespace, "G2C_ADD_EXERCISE_TIME", "ExternalMessage::G2C_ADD_EXERCISE_TIME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_add_exercise_time_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ADD_EXERCISE_TIME(); });
    external_message_g2c_add_exercise_time_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ADD_EXERCISE_TIME>(new ExternalMessage::G2C_ADD_EXERCISE_TIME()); });
    external_message_g2c_add_exercise_time_type["nTotalHour"] = sol::property([](const ExternalMessage::G2C_ADD_EXERCISE_TIME& self) { return self.nTotalHour; }, [](ExternalMessage::G2C_ADD_EXERCISE_TIME& self, decltype(ExternalMessage::G2C_ADD_EXERCISE_TIME::nTotalHour) value) { self.nTotalHour = value; });
    auto external_message_g2c_get_exercise_exp_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GET_EXERCISE_EXP>(L, external_message_namespace, "G2C_GET_EXERCISE_EXP", "ExternalMessage::G2C_GET_EXERCISE_EXP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_get_exercise_exp_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GET_EXERCISE_EXP(); });
    external_message_g2c_get_exercise_exp_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GET_EXERCISE_EXP>(new ExternalMessage::G2C_GET_EXERCISE_EXP()); });
    external_message_g2c_get_exercise_exp_type["nHour"] = sol::property([](const ExternalMessage::G2C_GET_EXERCISE_EXP& self) { return self.nHour; }, [](ExternalMessage::G2C_GET_EXERCISE_EXP& self, decltype(ExternalMessage::G2C_GET_EXERCISE_EXP::nHour) value) { self.nHour = value; });
    external_message_g2c_get_exercise_exp_type["nExp"] = sol::property([](const ExternalMessage::G2C_GET_EXERCISE_EXP& self) { return self.nExp; }, [](ExternalMessage::G2C_GET_EXERCISE_EXP& self, decltype(ExternalMessage::G2C_GET_EXERCISE_EXP::nExp) value) { self.nExp = value; });
}

}
