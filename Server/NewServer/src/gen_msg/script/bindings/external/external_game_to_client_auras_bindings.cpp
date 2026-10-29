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

void register_external_game_to_client_auras_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_aura_data_type = cqyh::gen_msg::usertype<ExternalMessage::AURA_DATA>(L, external_message_namespace, "AURA_DATA", "ExternalMessage::AURA_DATA");
    external_message_aura_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::AURA_DATA(); });
    external_message_aura_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::AURA_DATA>(new ExternalMessage::AURA_DATA()); });
    external_message_aura_data_type["wID"] = sol::property([](const ExternalMessage::AURA_DATA& self) { return self.wID; }, [](ExternalMessage::AURA_DATA& self, decltype(ExternalMessage::AURA_DATA::wID) value) { self.wID = value; });
    external_message_aura_data_type["dwInstanceID"] = sol::property([](const ExternalMessage::AURA_DATA& self) { return self.dwInstanceID; }, [](ExternalMessage::AURA_DATA& self, decltype(ExternalMessage::AURA_DATA::dwInstanceID) value) { self.dwInstanceID = value; });
    external_message_aura_data_type["byStackCount"] = sol::property([](const ExternalMessage::AURA_DATA& self) { return self.byStackCount; }, [](ExternalMessage::AURA_DATA& self, decltype(ExternalMessage::AURA_DATA::byStackCount) value) { self.byStackCount = value; });
    external_message_aura_data_type["dwRemainDuration"] = sol::property([](const ExternalMessage::AURA_DATA& self) { return self.dwRemainDuration; }, [](ExternalMessage::AURA_DATA& self, decltype(ExternalMessage::AURA_DATA::dwRemainDuration) value) { self.dwRemainDuration = value; });
    external_message_aura_data_type["dwMaxDuration"] = sol::property([](const ExternalMessage::AURA_DATA& self) { return self.dwMaxDuration; }, [](ExternalMessage::AURA_DATA& self, decltype(ExternalMessage::AURA_DATA::dwMaxDuration) value) { self.dwMaxDuration = value; });
    auto external_message_g2c_sync_aura_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_AURA_LIST>(L, external_message_namespace, "G2C_SYNC_AURA_LIST", "ExternalMessage::G2C_SYNC_AURA_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_aura_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_AURA_LIST(); });
    external_message_g2c_sync_aura_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_AURA_LIST>(new ExternalMessage::G2C_SYNC_AURA_LIST()); });
    external_message_g2c_sync_aura_list_type["astAuraData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_AURA_LIST::astAuraData);
    auto external_message_g2c_new_aura_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_NEW_AURA>(L, external_message_namespace, "G2C_NEW_AURA", "ExternalMessage::G2C_NEW_AURA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_new_aura_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_NEW_AURA(); });
    external_message_g2c_new_aura_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_NEW_AURA>(new ExternalMessage::G2C_NEW_AURA()); });
    external_message_g2c_new_aura_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_NEW_AURA& self) { return self.dwCharID; }, [](ExternalMessage::G2C_NEW_AURA& self, decltype(ExternalMessage::G2C_NEW_AURA::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_new_aura_type["wAuraID"] = sol::property([](const ExternalMessage::G2C_NEW_AURA& self) { return self.wAuraID; }, [](ExternalMessage::G2C_NEW_AURA& self, decltype(ExternalMessage::G2C_NEW_AURA::wAuraID) value) { self.wAuraID = value; });
    external_message_g2c_new_aura_type["dwInstanceID"] = sol::property([](const ExternalMessage::G2C_NEW_AURA& self) { return self.dwInstanceID; }, [](ExternalMessage::G2C_NEW_AURA& self, decltype(ExternalMessage::G2C_NEW_AURA::dwInstanceID) value) { self.dwInstanceID = value; });
    external_message_g2c_new_aura_type["dwCasterID"] = sol::property([](const ExternalMessage::G2C_NEW_AURA& self) { return self.dwCasterID; }, [](ExternalMessage::G2C_NEW_AURA& self, decltype(ExternalMessage::G2C_NEW_AURA::dwCasterID) value) { self.dwCasterID = value; });
    external_message_g2c_new_aura_type["dwDuration"] = sol::property([](const ExternalMessage::G2C_NEW_AURA& self) { return self.dwDuration; }, [](ExternalMessage::G2C_NEW_AURA& self, decltype(ExternalMessage::G2C_NEW_AURA::dwDuration) value) { self.dwDuration = value; });
    external_message_g2c_new_aura_type["byStackCount"] = sol::property([](const ExternalMessage::G2C_NEW_AURA& self) { return self.byStackCount; }, [](ExternalMessage::G2C_NEW_AURA& self, decltype(ExternalMessage::G2C_NEW_AURA::byStackCount) value) { self.byStackCount = value; });
    auto external_message_g2c_del_aura_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DEL_AURA>(L, external_message_namespace, "G2C_DEL_AURA", "ExternalMessage::G2C_DEL_AURA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_del_aura_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DEL_AURA(); });
    external_message_g2c_del_aura_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DEL_AURA>(new ExternalMessage::G2C_DEL_AURA()); });
    external_message_g2c_del_aura_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_DEL_AURA& self) { return self.dwCharID; }, [](ExternalMessage::G2C_DEL_AURA& self, decltype(ExternalMessage::G2C_DEL_AURA::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_del_aura_type["dwInstanceID"] = sol::property([](const ExternalMessage::G2C_DEL_AURA& self) { return self.dwInstanceID; }, [](ExternalMessage::G2C_DEL_AURA& self, decltype(ExternalMessage::G2C_DEL_AURA::dwInstanceID) value) { self.dwInstanceID = value; });
    auto external_message_g2c_aura_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_AURA_CHANGE>(L, external_message_namespace, "G2C_AURA_CHANGE", "ExternalMessage::G2C_AURA_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_aura_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_AURA_CHANGE(); });
    external_message_g2c_aura_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_AURA_CHANGE>(new ExternalMessage::G2C_AURA_CHANGE()); });
    external_message_g2c_aura_change_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_AURA_CHANGE& self) { return self.dwCharID; }, [](ExternalMessage::G2C_AURA_CHANGE& self, decltype(ExternalMessage::G2C_AURA_CHANGE::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_aura_change_type["AuraData"] = &ExternalMessage::G2C_AURA_CHANGE::AuraData;
    auto external_message_g2c_aura_effect_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_AURA_EFFECT>(L, external_message_namespace, "G2C_AURA_EFFECT", "ExternalMessage::G2C_AURA_EFFECT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_aura_effect_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_AURA_EFFECT(); });
    external_message_g2c_aura_effect_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_AURA_EFFECT>(new ExternalMessage::G2C_AURA_EFFECT()); });
    external_message_g2c_aura_effect_type["dwInstanceID"] = sol::property([](const ExternalMessage::G2C_AURA_EFFECT& self) { return self.dwInstanceID; }, [](ExternalMessage::G2C_AURA_EFFECT& self, decltype(ExternalMessage::G2C_AURA_EFFECT::dwInstanceID) value) { self.dwInstanceID = value; });
    external_message_g2c_aura_effect_type["wAuraID"] = sol::property([](const ExternalMessage::G2C_AURA_EFFECT& self) { return self.wAuraID; }, [](ExternalMessage::G2C_AURA_EFFECT& self, decltype(ExternalMessage::G2C_AURA_EFFECT::wAuraID) value) { self.wAuraID = value; });
    external_message_g2c_aura_effect_type["dwCasterID"] = sol::property([](const ExternalMessage::G2C_AURA_EFFECT& self) { return self.dwCasterID; }, [](ExternalMessage::G2C_AURA_EFFECT& self, decltype(ExternalMessage::G2C_AURA_EFFECT::dwCasterID) value) { self.dwCasterID = value; });
    external_message_g2c_aura_effect_type["dwTargetID"] = sol::property([](const ExternalMessage::G2C_AURA_EFFECT& self) { return self.dwTargetID; }, [](ExternalMessage::G2C_AURA_EFFECT& self, decltype(ExternalMessage::G2C_AURA_EFFECT::dwTargetID) value) { self.dwTargetID = value; });
    external_message_g2c_aura_effect_type["nEffectValue"] = sol::property([](const ExternalMessage::G2C_AURA_EFFECT& self) { return self.nEffectValue; }, [](ExternalMessage::G2C_AURA_EFFECT& self, decltype(ExternalMessage::G2C_AURA_EFFECT::nEffectValue) value) { self.nEffectValue = value; });
    auto external_message_g2c_add_cooldown_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ADD_COOLDOWN>(L, external_message_namespace, "G2C_ADD_COOLDOWN", "ExternalMessage::G2C_ADD_COOLDOWN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_add_cooldown_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ADD_COOLDOWN(); });
    external_message_g2c_add_cooldown_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ADD_COOLDOWN>(new ExternalMessage::G2C_ADD_COOLDOWN()); });
    external_message_g2c_add_cooldown_type["dwCooldownID"] = sol::property([](const ExternalMessage::G2C_ADD_COOLDOWN& self) { return self.dwCooldownID; }, [](ExternalMessage::G2C_ADD_COOLDOWN& self, decltype(ExternalMessage::G2C_ADD_COOLDOWN::dwCooldownID) value) { self.dwCooldownID = value; });
    external_message_g2c_add_cooldown_type["dwCooldownValue"] = sol::property([](const ExternalMessage::G2C_ADD_COOLDOWN& self) { return self.dwCooldownValue; }, [](ExternalMessage::G2C_ADD_COOLDOWN& self, decltype(ExternalMessage::G2C_ADD_COOLDOWN::dwCooldownValue) value) { self.dwCooldownValue = value; });
    auto external_message_aura_brief_data_type = cqyh::gen_msg::usertype<ExternalMessage::AURA_BRIEF_DATA>(L, external_message_namespace, "AURA_BRIEF_DATA", "ExternalMessage::AURA_BRIEF_DATA");
    external_message_aura_brief_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::AURA_BRIEF_DATA(); });
    external_message_aura_brief_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::AURA_BRIEF_DATA>(new ExternalMessage::AURA_BRIEF_DATA()); });
    external_message_aura_brief_data_type["wTemplateID"] = sol::property([](const ExternalMessage::AURA_BRIEF_DATA& self) { return self.wTemplateID; }, [](ExternalMessage::AURA_BRIEF_DATA& self, decltype(ExternalMessage::AURA_BRIEF_DATA::wTemplateID) value) { self.wTemplateID = value; });
    external_message_aura_brief_data_type["dwInstanceID"] = sol::property([](const ExternalMessage::AURA_BRIEF_DATA& self) { return self.dwInstanceID; }, [](ExternalMessage::AURA_BRIEF_DATA& self, decltype(ExternalMessage::AURA_BRIEF_DATA::dwInstanceID) value) { self.dwInstanceID = value; });
    auto external_message_g2c_sync_aura_brief_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_AURA_BRIEF>(L, external_message_namespace, "G2C_SYNC_AURA_BRIEF", "ExternalMessage::G2C_SYNC_AURA_BRIEF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_aura_brief_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_AURA_BRIEF(); });
    external_message_g2c_sync_aura_brief_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_AURA_BRIEF>(new ExternalMessage::G2C_SYNC_AURA_BRIEF()); });
    external_message_g2c_sync_aura_brief_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_SYNC_AURA_BRIEF& self) { return self.dwCharID; }, [](ExternalMessage::G2C_SYNC_AURA_BRIEF& self, decltype(ExternalMessage::G2C_SYNC_AURA_BRIEF::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_sync_aura_brief_type["BriefData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_AURA_BRIEF::BriefData);
    auto external_message_g2c_sync_purchase_aura_record_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD>(L, external_message_namespace, "G2C_SYNC_PURCHASE_AURA_RECORD", "ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_purchase_aura_record_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD(); });
    external_message_g2c_sync_purchase_aura_record_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD>(new ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD()); });
    external_message_g2c_sync_purchase_aura_record_type["wCount"] = sol::property([](const ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD& self) { return self.wCount; }, [](ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD& self, decltype(ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD::wCount) value) { self.wCount = value; });
    external_message_g2c_sync_purchase_aura_record_type["data"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_PURCHASE_AURA_RECORD::data);
}

}
