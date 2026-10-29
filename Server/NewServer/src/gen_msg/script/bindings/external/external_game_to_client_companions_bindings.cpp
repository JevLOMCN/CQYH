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

void register_external_game_to_client_companions_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_pet_level_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_PET_LEVEL>(L, external_message_namespace, "G2C_SYNC_PET_LEVEL", "ExternalMessage::G2C_SYNC_PET_LEVEL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_pet_level_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_PET_LEVEL(); });
    external_message_g2c_sync_pet_level_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_PET_LEVEL>(new ExternalMessage::G2C_SYNC_PET_LEVEL()); });
    external_message_g2c_sync_pet_level_type["dwNpcID"] = sol::property([](const ExternalMessage::G2C_SYNC_PET_LEVEL& self) { return self.dwNpcID; }, [](ExternalMessage::G2C_SYNC_PET_LEVEL& self, decltype(ExternalMessage::G2C_SYNC_PET_LEVEL::dwNpcID) value) { self.dwNpcID = value; });
    external_message_g2c_sync_pet_level_type["byLevel"] = sol::property([](const ExternalMessage::G2C_SYNC_PET_LEVEL& self) { return self.byLevel; }, [](ExternalMessage::G2C_SYNC_PET_LEVEL& self, decltype(ExternalMessage::G2C_SYNC_PET_LEVEL::byLevel) value) { self.byLevel = value; });
    auto external_message_g2c_sync_hero_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_HERO_DATA>(L, external_message_namespace, "G2C_SYNC_HERO_DATA", "ExternalMessage::G2C_SYNC_HERO_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_hero_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_HERO_DATA(); });
    external_message_g2c_sync_hero_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_HERO_DATA>(new ExternalMessage::G2C_SYNC_HERO_DATA()); });
    external_message_g2c_sync_hero_data_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_DATA& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_SYNC_HERO_DATA& self, decltype(ExternalMessage::G2C_SYNC_HERO_DATA::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_sync_hero_data_type["szName"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_HERO_DATA::szName);
    external_message_g2c_sync_hero_data_type["szMasterName"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_HERO_DATA::szMasterName);
    external_message_g2c_sync_hero_data_type["byClass"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_DATA& self) { return self.byClass; }, [](ExternalMessage::G2C_SYNC_HERO_DATA& self, decltype(ExternalMessage::G2C_SYNC_HERO_DATA::byClass) value) { self.byClass = value; });
    external_message_g2c_sync_hero_data_type["bySex"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_DATA& self) { return self.bySex; }, [](ExternalMessage::G2C_SYNC_HERO_DATA& self, decltype(ExternalMessage::G2C_SYNC_HERO_DATA::bySex) value) { self.bySex = value; });
    external_message_g2c_sync_hero_data_type["byLevel"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_DATA& self) { return self.byLevel; }, [](ExternalMessage::G2C_SYNC_HERO_DATA& self, decltype(ExternalMessage::G2C_SYNC_HERO_DATA::byLevel) value) { self.byLevel = value; });
    external_message_g2c_sync_hero_data_type["anAvatar"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_HERO_DATA::anAvatar);
    external_message_g2c_sync_hero_data_type["nMaxHp"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_DATA& self) { return self.nMaxHp; }, [](ExternalMessage::G2C_SYNC_HERO_DATA& self, decltype(ExternalMessage::G2C_SYNC_HERO_DATA::nMaxHp) value) { self.nMaxHp = value; });
    external_message_g2c_sync_hero_data_type["nMaxMp"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_DATA& self) { return self.nMaxMp; }, [](ExternalMessage::G2C_SYNC_HERO_DATA& self, decltype(ExternalMessage::G2C_SYNC_HERO_DATA::nMaxMp) value) { self.nMaxMp = value; });
    auto external_message_g2c_hire_hero_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_HIRE_HERO>(L, external_message_namespace, "G2C_HIRE_HERO", "ExternalMessage::G2C_HIRE_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_hire_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_HIRE_HERO(); });
    external_message_g2c_hire_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_HIRE_HERO>(new ExternalMessage::G2C_HIRE_HERO()); });
    external_message_g2c_hire_hero_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_HIRE_HERO& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_HIRE_HERO& self, decltype(ExternalMessage::G2C_HIRE_HERO::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_hire_hero_type["byHeroClass"] = sol::property([](const ExternalMessage::G2C_HIRE_HERO& self) { return self.byHeroClass; }, [](ExternalMessage::G2C_HIRE_HERO& self, decltype(ExternalMessage::G2C_HIRE_HERO::byHeroClass) value) { self.byHeroClass = value; });
    external_message_g2c_hire_hero_type["byHeroSex"] = sol::property([](const ExternalMessage::G2C_HIRE_HERO& self) { return self.byHeroSex; }, [](ExternalMessage::G2C_HIRE_HERO& self, decltype(ExternalMessage::G2C_HIRE_HERO::byHeroSex) value) { self.byHeroSex = value; });
    external_message_g2c_hire_hero_type["byHeroLevel"] = sol::property([](const ExternalMessage::G2C_HIRE_HERO& self) { return self.byHeroLevel; }, [](ExternalMessage::G2C_HIRE_HERO& self, decltype(ExternalMessage::G2C_HIRE_HERO::byHeroLevel) value) { self.byHeroLevel = value; });
    external_message_g2c_hire_hero_type["byHeroAiType"] = sol::property([](const ExternalMessage::G2C_HIRE_HERO& self) { return self.byHeroAiType; }, [](ExternalMessage::G2C_HIRE_HERO& self, decltype(ExternalMessage::G2C_HIRE_HERO::byHeroAiType) value) { self.byHeroAiType = value; });
    external_message_g2c_hire_hero_type["nGrowth"] = sol::property([](const ExternalMessage::G2C_HIRE_HERO& self) { return self.nGrowth; }, [](ExternalMessage::G2C_HIRE_HERO& self, decltype(ExternalMessage::G2C_HIRE_HERO::nGrowth) value) { self.nGrowth = value; });
    auto external_message_g2c_fire_hero_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_FIRE_HERO>(L, external_message_namespace, "G2C_FIRE_HERO", "ExternalMessage::G2C_FIRE_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_fire_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_FIRE_HERO(); });
    external_message_g2c_fire_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_FIRE_HERO>(new ExternalMessage::G2C_FIRE_HERO()); });
    external_message_g2c_fire_hero_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_FIRE_HERO& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_FIRE_HERO& self, decltype(ExternalMessage::G2C_FIRE_HERO::dwHeroID) value) { self.dwHeroID = value; });
    auto external_message_g2c_change_hero_ai_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHANGE_HERO_AI>(L, external_message_namespace, "G2C_CHANGE_HERO_AI", "ExternalMessage::G2C_CHANGE_HERO_AI", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_change_hero_ai_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHANGE_HERO_AI(); });
    external_message_g2c_change_hero_ai_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHANGE_HERO_AI>(new ExternalMessage::G2C_CHANGE_HERO_AI()); });
    external_message_g2c_change_hero_ai_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_CHANGE_HERO_AI& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_CHANGE_HERO_AI& self, decltype(ExternalMessage::G2C_CHANGE_HERO_AI::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_change_hero_ai_type["nAiType"] = sol::property([](const ExternalMessage::G2C_CHANGE_HERO_AI& self) { return self.nAiType; }, [](ExternalMessage::G2C_CHANGE_HERO_AI& self, decltype(ExternalMessage::G2C_CHANGE_HERO_AI::nAiType) value) { self.nAiType = value; });
    auto external_message_g2c_summon_hero_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SUMMON_HERO>(L, external_message_namespace, "G2C_SUMMON_HERO", "ExternalMessage::G2C_SUMMON_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_summon_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SUMMON_HERO(); });
    external_message_g2c_summon_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SUMMON_HERO>(new ExternalMessage::G2C_SUMMON_HERO()); });
    auto external_message_g2c_recall_hero_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RECALL_HERO>(L, external_message_namespace, "G2C_RECALL_HERO", "ExternalMessage::G2C_RECALL_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_recall_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RECALL_HERO(); });
    external_message_g2c_recall_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RECALL_HERO>(new ExternalMessage::G2C_RECALL_HERO()); });
}

}
