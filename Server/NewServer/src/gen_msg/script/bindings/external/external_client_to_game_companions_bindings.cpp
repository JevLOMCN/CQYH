#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_companions_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_hire_hero_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_HIRE_HERO>(L, external_message_namespace, "C2G_HIRE_HERO", "ExternalMessage::C2G_HIRE_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_hire_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_HIRE_HERO(); });
    external_message_c2g_hire_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_HIRE_HERO>(new ExternalMessage::C2G_HIRE_HERO()); });
    external_message_c2g_hire_hero_type["byClass"] = sol::property([](const ExternalMessage::C2G_HIRE_HERO& self) { return self.byClass; }, [](ExternalMessage::C2G_HIRE_HERO& self, decltype(ExternalMessage::C2G_HIRE_HERO::byClass) value) { self.byClass = value; });
    external_message_c2g_hire_hero_type["bySex"] = sol::property([](const ExternalMessage::C2G_HIRE_HERO& self) { return self.bySex; }, [](ExternalMessage::C2G_HIRE_HERO& self, decltype(ExternalMessage::C2G_HIRE_HERO::bySex) value) { self.bySex = value; });
    auto external_message_c2g_fire_hero_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_FIRE_HERO>(L, external_message_namespace, "C2G_FIRE_HERO", "ExternalMessage::C2G_FIRE_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_fire_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_FIRE_HERO(); });
    external_message_c2g_fire_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_FIRE_HERO>(new ExternalMessage::C2G_FIRE_HERO()); });
    auto external_message_c2g_summon_hero_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SUMMON_HERO>(L, external_message_namespace, "C2G_SUMMON_HERO", "ExternalMessage::C2G_SUMMON_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_summon_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SUMMON_HERO(); });
    external_message_c2g_summon_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SUMMON_HERO>(new ExternalMessage::C2G_SUMMON_HERO()); });
    auto external_message_c2g_recall_hero_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_RECALL_HERO>(L, external_message_namespace, "C2G_RECALL_HERO", "ExternalMessage::C2G_RECALL_HERO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_recall_hero_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_RECALL_HERO(); });
    external_message_c2g_recall_hero_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_RECALL_HERO>(new ExternalMessage::C2G_RECALL_HERO()); });
    auto external_message_c2g_change_hero_ai_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHANGE_HERO_AI>(L, external_message_namespace, "C2G_CHANGE_HERO_AI", "ExternalMessage::C2G_CHANGE_HERO_AI", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_change_hero_ai_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHANGE_HERO_AI(); });
    external_message_c2g_change_hero_ai_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHANGE_HERO_AI>(new ExternalMessage::C2G_CHANGE_HERO_AI()); });
    external_message_c2g_change_hero_ai_type["byAiType"] = sol::property([](const ExternalMessage::C2G_CHANGE_HERO_AI& self) { return self.byAiType; }, [](ExternalMessage::C2G_CHANGE_HERO_AI& self, decltype(ExternalMessage::C2G_CHANGE_HERO_AI::byAiType) value) { self.byAiType = value; });
    auto external_message_c2g_change_hero_name_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHANGE_HERO_NAME>(L, external_message_namespace, "C2G_CHANGE_HERO_NAME", "ExternalMessage::C2G_CHANGE_HERO_NAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_change_hero_name_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHANGE_HERO_NAME(); });
    external_message_c2g_change_hero_name_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHANGE_HERO_NAME>(new ExternalMessage::C2G_CHANGE_HERO_NAME()); });
    external_message_c2g_change_hero_name_type["szNewName"] = cqyh::lua::string_property(&ExternalMessage::C2G_CHANGE_HERO_NAME::szNewName);
    auto external_message_c2g_change_pet_state_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CHANGE_PET_STATE>(L, external_message_namespace, "C2G_CHANGE_PET_STATE", "ExternalMessage::C2G_CHANGE_PET_STATE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_change_pet_state_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CHANGE_PET_STATE(); });
    external_message_c2g_change_pet_state_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CHANGE_PET_STATE>(new ExternalMessage::C2G_CHANGE_PET_STATE()); });
    external_message_c2g_change_pet_state_type["byState"] = sol::property([](const ExternalMessage::C2G_CHANGE_PET_STATE& self) { return self.byState; }, [](ExternalMessage::C2G_CHANGE_PET_STATE& self, decltype(ExternalMessage::C2G_CHANGE_PET_STATE::byState) value) { self.byState = value; });
}

}
