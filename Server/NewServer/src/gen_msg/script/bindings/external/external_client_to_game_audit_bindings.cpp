#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_audit_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_spell_log_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SPELL_LOG>(L, external_message_namespace, "C2G_SPELL_LOG", "ExternalMessage::C2G_SPELL_LOG", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_spell_log_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SPELL_LOG(); });
    external_message_c2g_spell_log_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SPELL_LOG>(new ExternalMessage::C2G_SPELL_LOG()); });
    external_message_c2g_spell_log_type["dwTime"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.dwTime; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::dwTime) value) { self.dwTime = value; });
    external_message_c2g_spell_log_type["wSpellID"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.wSpellID; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::wSpellID) value) { self.wSpellID = value; });
    external_message_c2g_spell_log_type["nCurrentMp"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.nCurrentMp; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::nCurrentMp) value) { self.nCurrentMp = value; });
    external_message_c2g_spell_log_type["nCdRemain"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.nCdRemain; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::nCdRemain) value) { self.nCdRemain = value; });
    external_message_c2g_spell_log_type["dwTargetID"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.dwTargetID; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::dwTargetID) value) { self.dwTargetID = value; });
    external_message_c2g_spell_log_type["wTargetX"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.wTargetX; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::wTargetX) value) { self.wTargetX = value; });
    external_message_c2g_spell_log_type["wTargetY"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.wTargetY; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::wTargetY) value) { self.wTargetY = value; });
    external_message_c2g_spell_log_type["wTargetZ"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.wTargetZ; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::wTargetZ) value) { self.wTargetZ = value; });
    external_message_c2g_spell_log_type["byNotEnoughMp"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.byNotEnoughMp; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::byNotEnoughMp) value) { self.byNotEnoughMp = value; });
    external_message_c2g_spell_log_type["byInCd"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.byInCd; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::byInCd) value) { self.byInCd = value; });
    external_message_c2g_spell_log_type["byIsShiftPressed"] = sol::property([](const ExternalMessage::C2G_SPELL_LOG& self) { return self.byIsShiftPressed; }, [](ExternalMessage::C2G_SPELL_LOG& self, decltype(ExternalMessage::C2G_SPELL_LOG::byIsShiftPressed) value) { self.byIsShiftPressed = value; });
}

}
