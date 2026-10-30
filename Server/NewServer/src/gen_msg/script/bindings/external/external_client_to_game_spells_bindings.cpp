#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_spells_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_learn_spell_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LEARN_SPELL>(L, external_message_namespace, "C2G_LEARN_SPELL", "ExternalMessage::C2G_LEARN_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_learn_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LEARN_SPELL(); });
    external_message_c2g_learn_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LEARN_SPELL>(new ExternalMessage::C2G_LEARN_SPELL()); });
    external_message_c2g_learn_spell_type["wSpellID"] = sol::property([](const ExternalMessage::C2G_LEARN_SPELL& self) { return self.wSpellID; }, [](ExternalMessage::C2G_LEARN_SPELL& self, decltype(ExternalMessage::C2G_LEARN_SPELL::wSpellID) value) { self.wSpellID = value; });
    external_message_c2g_learn_spell_type["bySpellLevel"] = sol::property([](const ExternalMessage::C2G_LEARN_SPELL& self) { return self.bySpellLevel; }, [](ExternalMessage::C2G_LEARN_SPELL& self, decltype(ExternalMessage::C2G_LEARN_SPELL::bySpellLevel) value) { self.bySpellLevel = value; });
    auto external_message_c2g_cast_spell_ex_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CAST_SPELL_EX>(L, external_message_namespace, "C2G_CAST_SPELL_EX", "ExternalMessage::C2G_CAST_SPELL_EX", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_cast_spell_ex_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CAST_SPELL_EX(); });
    external_message_c2g_cast_spell_ex_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CAST_SPELL_EX>(new ExternalMessage::C2G_CAST_SPELL_EX()); });
    external_message_c2g_cast_spell_ex_type["wSpellID"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.wSpellID; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::wSpellID) value) { self.wSpellID = value; });
    external_message_c2g_cast_spell_ex_type["bySerial"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.bySerial; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::bySerial) value) { self.bySerial = value; });
    external_message_c2g_cast_spell_ex_type["byRotateToTarget"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.byRotateToTarget; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::byRotateToTarget) value) { self.byRotateToTarget = value; });
    external_message_c2g_cast_spell_ex_type["dwTargetID"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.dwTargetID; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::dwTargetID) value) { self.dwTargetID = value; });
    external_message_c2g_cast_spell_ex_type["wTargetX"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.wTargetX; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::wTargetX) value) { self.wTargetX = value; });
    external_message_c2g_cast_spell_ex_type["wTargetY"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.wTargetY; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::wTargetY) value) { self.wTargetY = value; });
    external_message_c2g_cast_spell_ex_type["wTargetZ"] = sol::property([](const ExternalMessage::C2G_CAST_SPELL_EX& self) { return self.wTargetZ; }, [](ExternalMessage::C2G_CAST_SPELL_EX& self, decltype(ExternalMessage::C2G_CAST_SPELL_EX::wTargetZ) value) { self.wTargetZ = value; });
    auto external_message_c2g_directly_cast_spell_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DIRECTLY_CAST_SPELL>(L, external_message_namespace, "C2G_DIRECTLY_CAST_SPELL", "ExternalMessage::C2G_DIRECTLY_CAST_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_directly_cast_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DIRECTLY_CAST_SPELL(); });
    external_message_c2g_directly_cast_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DIRECTLY_CAST_SPELL>(new ExternalMessage::C2G_DIRECTLY_CAST_SPELL()); });
    external_message_c2g_directly_cast_spell_type["dwSpellID"] = sol::property([](const ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self) { return self.dwSpellID; }, [](ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self, decltype(ExternalMessage::C2G_DIRECTLY_CAST_SPELL::dwSpellID) value) { self.dwSpellID = value; });
    external_message_c2g_directly_cast_spell_type["bySpellLevel"] = sol::property([](const ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self) { return self.bySpellLevel; }, [](ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self, decltype(ExternalMessage::C2G_DIRECTLY_CAST_SPELL::bySpellLevel) value) { self.bySpellLevel = value; });
    external_message_c2g_directly_cast_spell_type["dwTargetID"] = sol::property([](const ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self) { return self.dwTargetID; }, [](ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self, decltype(ExternalMessage::C2G_DIRECTLY_CAST_SPELL::dwTargetID) value) { self.dwTargetID = value; });
    external_message_c2g_directly_cast_spell_type["wTargetX"] = sol::property([](const ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self) { return self.wTargetX; }, [](ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self, decltype(ExternalMessage::C2G_DIRECTLY_CAST_SPELL::wTargetX) value) { self.wTargetX = value; });
    external_message_c2g_directly_cast_spell_type["wTargetY"] = sol::property([](const ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self) { return self.wTargetY; }, [](ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self, decltype(ExternalMessage::C2G_DIRECTLY_CAST_SPELL::wTargetY) value) { self.wTargetY = value; });
    external_message_c2g_directly_cast_spell_type["wTargetZ"] = sol::property([](const ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self) { return self.wTargetZ; }, [](ExternalMessage::C2G_DIRECTLY_CAST_SPELL& self, decltype(ExternalMessage::C2G_DIRECTLY_CAST_SPELL::wTargetZ) value) { self.wTargetZ = value; });
    auto external_message_c2g_cast_switch_spell_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CAST_SWITCH_SPELL>(L, external_message_namespace, "C2G_CAST_SWITCH_SPELL", "ExternalMessage::C2G_CAST_SWITCH_SPELL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_cast_switch_spell_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CAST_SWITCH_SPELL(); });
    external_message_c2g_cast_switch_spell_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CAST_SWITCH_SPELL>(new ExternalMessage::C2G_CAST_SWITCH_SPELL()); });
    external_message_c2g_cast_switch_spell_type["wSpellID"] = sol::property([](const ExternalMessage::C2G_CAST_SWITCH_SPELL& self) { return self.wSpellID; }, [](ExternalMessage::C2G_CAST_SWITCH_SPELL& self, decltype(ExternalMessage::C2G_CAST_SWITCH_SPELL::wSpellID) value) { self.wSpellID = value; });
    auto external_message_c2g_role_cancel_cast_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_CANCEL_CAST>(L, external_message_namespace, "C2G_ROLE_CANCEL_CAST", "ExternalMessage::C2G_ROLE_CANCEL_CAST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_cancel_cast_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_CANCEL_CAST(); });
    external_message_c2g_role_cancel_cast_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_CANCEL_CAST>(new ExternalMessage::C2G_ROLE_CANCEL_CAST()); });
    external_message_c2g_role_cancel_cast_type["wSpellID"] = sol::property([](const ExternalMessage::C2G_ROLE_CANCEL_CAST& self) { return self.wSpellID; }, [](ExternalMessage::C2G_ROLE_CANCEL_CAST& self, decltype(ExternalMessage::C2G_ROLE_CANCEL_CAST::wSpellID) value) { self.wSpellID = value; });
    auto external_message_c2g_clear_spell_cache_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CLEAR_SPELL_CACHE>(L, external_message_namespace, "C2G_CLEAR_SPELL_CACHE", "ExternalMessage::C2G_CLEAR_SPELL_CACHE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_clear_spell_cache_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CLEAR_SPELL_CACHE(); });
    external_message_c2g_clear_spell_cache_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CLEAR_SPELL_CACHE>(new ExternalMessage::C2G_CLEAR_SPELL_CACHE()); });
    external_message_c2g_clear_spell_cache_type["nTag"] = sol::property([](const ExternalMessage::C2G_CLEAR_SPELL_CACHE& self) { return self.nTag; }, [](ExternalMessage::C2G_CLEAR_SPELL_CACHE& self, decltype(ExternalMessage::C2G_CLEAR_SPELL_CACHE::nTag) value) { self.nTag = value; });
}

}
