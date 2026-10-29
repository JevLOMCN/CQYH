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

void register_external_game_to_client_enhancement_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_equip_rune_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_EQUIP_RUNE>(L, external_message_namespace, "G2C_EQUIP_RUNE", "ExternalMessage::G2C_EQUIP_RUNE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_equip_rune_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_EQUIP_RUNE(); });
    external_message_g2c_equip_rune_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_EQUIP_RUNE>(new ExternalMessage::G2C_EQUIP_RUNE()); });
    external_message_g2c_equip_rune_type["wSpellID"] = sol::property([](const ExternalMessage::G2C_EQUIP_RUNE& self) { return self.wSpellID; }, [](ExternalMessage::G2C_EQUIP_RUNE& self, decltype(ExternalMessage::G2C_EQUIP_RUNE::wSpellID) value) { self.wSpellID = value; });
    external_message_g2c_equip_rune_type["byRuneSlot"] = sol::property([](const ExternalMessage::G2C_EQUIP_RUNE& self) { return self.byRuneSlot; }, [](ExternalMessage::G2C_EQUIP_RUNE& self, decltype(ExternalMessage::G2C_EQUIP_RUNE::byRuneSlot) value) { self.byRuneSlot = value; });
    external_message_g2c_equip_rune_type["bySpellLevel"] = sol::property([](const ExternalMessage::G2C_EQUIP_RUNE& self) { return self.bySpellLevel; }, [](ExternalMessage::G2C_EQUIP_RUNE& self, decltype(ExternalMessage::G2C_EQUIP_RUNE::bySpellLevel) value) { self.bySpellLevel = value; });
    auto external_message_g2c_leech_soul_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_LEECH_SOUL>(L, external_message_namespace, "G2C_LEECH_SOUL", "ExternalMessage::G2C_LEECH_SOUL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_leech_soul_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_LEECH_SOUL(); });
    external_message_g2c_leech_soul_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_LEECH_SOUL>(new ExternalMessage::G2C_LEECH_SOUL()); });
    external_message_g2c_leech_soul_type["dwFromObjID"] = sol::property([](const ExternalMessage::G2C_LEECH_SOUL& self) { return self.dwFromObjID; }, [](ExternalMessage::G2C_LEECH_SOUL& self, decltype(ExternalMessage::G2C_LEECH_SOUL::dwFromObjID) value) { self.dwFromObjID = value; });
    external_message_g2c_leech_soul_type["dwToObjID"] = sol::property([](const ExternalMessage::G2C_LEECH_SOUL& self) { return self.dwToObjID; }, [](ExternalMessage::G2C_LEECH_SOUL& self, decltype(ExternalMessage::G2C_LEECH_SOUL::dwToObjID) value) { self.dwToObjID = value; });
    external_message_g2c_leech_soul_type["nSoulPts"] = sol::property([](const ExternalMessage::G2C_LEECH_SOUL& self) { return self.nSoulPts; }, [](ExternalMessage::G2C_LEECH_SOUL& self, decltype(ExternalMessage::G2C_LEECH_SOUL::nSoulPts) value) { self.nSoulPts = value; });
    auto external_message_g2c_sync_role_spirit_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA>(L, external_message_namespace, "G2C_SYNC_ROLE_SPIRIT_DATA", "ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_role_spirit_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA(); });
    external_message_g2c_sync_role_spirit_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA>(new ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA()); });
    external_message_g2c_sync_role_spirit_data_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_role_spirit_data_type["szData"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_ROLE_SPIRIT_DATA::szData);
    auto external_message_g2c_sync_refine_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_REFINE>(L, external_message_namespace, "G2C_SYNC_REFINE", "ExternalMessage::G2C_SYNC_REFINE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_refine_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_REFINE(); });
    external_message_g2c_sync_refine_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_REFINE>(new ExternalMessage::G2C_SYNC_REFINE()); });
    external_message_g2c_sync_refine_type["awRefineLevel"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_REFINE::awRefineLevel);
    auto external_message_g2c_luck_oil_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_LUCK_OIL>(L, external_message_namespace, "G2C_LUCK_OIL", "ExternalMessage::G2C_LUCK_OIL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_luck_oil_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_LUCK_OIL(); });
    external_message_g2c_luck_oil_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_LUCK_OIL>(new ExternalMessage::G2C_LUCK_OIL()); });
    external_message_g2c_luck_oil_type["chResult"] = sol::property([](const ExternalMessage::G2C_LUCK_OIL& self) { return self.chResult; }, [](ExternalMessage::G2C_LUCK_OIL& self, decltype(ExternalMessage::G2C_LUCK_OIL::chResult) value) { self.chResult = value; });
    auto external_message_g2c_repair_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_REPAIR_ITEM>(L, external_message_namespace, "G2C_REPAIR_ITEM", "ExternalMessage::G2C_REPAIR_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_repair_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_REPAIR_ITEM(); });
    external_message_g2c_repair_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_REPAIR_ITEM>(new ExternalMessage::G2C_REPAIR_ITEM()); });
    auto external_message_g2c_sync_spirit_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SPIRIT>(L, external_message_namespace, "G2C_SYNC_SPIRIT", "ExternalMessage::G2C_SYNC_SPIRIT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_spirit_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_SPIRIT(); });
    external_message_g2c_sync_spirit_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_SPIRIT>(new ExternalMessage::G2C_SYNC_SPIRIT()); });
    external_message_g2c_sync_spirit_type["szData"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_SPIRIT::szData);
    auto external_message_g2c_spirit_up_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_UP>(L, external_message_namespace, "G2C_SPIRIT_UP", "ExternalMessage::G2C_SPIRIT_UP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_up_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_UP(); });
    external_message_g2c_spirit_up_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_UP>(new ExternalMessage::G2C_SPIRIT_UP()); });
    external_message_g2c_spirit_up_type["nLevel"] = sol::property([](const ExternalMessage::G2C_SPIRIT_UP& self) { return self.nLevel; }, [](ExternalMessage::G2C_SPIRIT_UP& self, decltype(ExternalMessage::G2C_SPIRIT_UP::nLevel) value) { self.nLevel = value; });
    auto external_message_g2c_spirit_air_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_AIR_CHANGE>(L, external_message_namespace, "G2C_SPIRIT_AIR_CHANGE", "ExternalMessage::G2C_SPIRIT_AIR_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_air_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_AIR_CHANGE(); });
    external_message_g2c_spirit_air_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_AIR_CHANGE>(new ExternalMessage::G2C_SPIRIT_AIR_CHANGE()); });
    external_message_g2c_spirit_air_change_type["nBagType"] = sol::property([](const ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self) { return self.nBagType; }, [](ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_AIR_CHANGE::nBagType) value) { self.nBagType = value; });
    external_message_g2c_spirit_air_change_type["nAirID"] = sol::property([](const ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self) { return self.nAirID; }, [](ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_AIR_CHANGE::nAirID) value) { self.nAirID = value; });
    external_message_g2c_spirit_air_change_type["nIndex"] = sol::property([](const ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self) { return self.nIndex; }, [](ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_AIR_CHANGE::nIndex) value) { self.nIndex = value; });
    external_message_g2c_spirit_air_change_type["nExp"] = sol::property([](const ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self) { return self.nExp; }, [](ExternalMessage::G2C_SPIRIT_AIR_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_AIR_CHANGE::nExp) value) { self.nExp = value; });
    auto external_message_g2c_spirit_blood_open_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_BLOOD_OPEN>(L, external_message_namespace, "G2C_SPIRIT_BLOOD_OPEN", "ExternalMessage::G2C_SPIRIT_BLOOD_OPEN", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_blood_open_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_BLOOD_OPEN(); });
    external_message_g2c_spirit_blood_open_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_BLOOD_OPEN>(new ExternalMessage::G2C_SPIRIT_BLOOD_OPEN()); });
    external_message_g2c_spirit_blood_open_type["nPage"] = sol::property([](const ExternalMessage::G2C_SPIRIT_BLOOD_OPEN& self) { return self.nPage; }, [](ExternalMessage::G2C_SPIRIT_BLOOD_OPEN& self, decltype(ExternalMessage::G2C_SPIRIT_BLOOD_OPEN::nPage) value) { self.nPage = value; });
    auto external_message_g2c_spirit_air_levelup_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_AIR_LEVELUP>(L, external_message_namespace, "G2C_SPIRIT_AIR_LEVELUP", "ExternalMessage::G2C_SPIRIT_AIR_LEVELUP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_air_levelup_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_AIR_LEVELUP(); });
    external_message_g2c_spirit_air_levelup_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_AIR_LEVELUP>(new ExternalMessage::G2C_SPIRIT_AIR_LEVELUP()); });
    external_message_g2c_spirit_air_levelup_type["byLevel"] = sol::property([](const ExternalMessage::G2C_SPIRIT_AIR_LEVELUP& self) { return self.byLevel; }, [](ExternalMessage::G2C_SPIRIT_AIR_LEVELUP& self, decltype(ExternalMessage::G2C_SPIRIT_AIR_LEVELUP::byLevel) value) { self.byLevel = value; });
    auto external_message_g2c_spirit_slot_size_change_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE>(L, external_message_namespace, "G2C_SPIRIT_SLOT_SIZE_CHANGE", "ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_slot_size_change_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE(); });
    external_message_g2c_spirit_slot_size_change_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE>(new ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE()); });
    external_message_g2c_spirit_slot_size_change_type["nType"] = sol::property([](const ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE& self) { return self.nType; }, [](ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE::nType) value) { self.nType = value; });
    external_message_g2c_spirit_slot_size_change_type["nSize"] = sol::property([](const ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE& self) { return self.nSize; }, [](ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE& self, decltype(ExternalMessage::G2C_SPIRIT_SLOT_SIZE_CHANGE::nSize) value) { self.nSize = value; });
    auto external_message_g2c_spirit_brush_slot_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPIRIT_BRUSH_SLOT>(L, external_message_namespace, "G2C_SPIRIT_BRUSH_SLOT", "ExternalMessage::G2C_SPIRIT_BRUSH_SLOT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spirit_brush_slot_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPIRIT_BRUSH_SLOT(); });
    external_message_g2c_spirit_brush_slot_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPIRIT_BRUSH_SLOT>(new ExternalMessage::G2C_SPIRIT_BRUSH_SLOT()); });
    external_message_g2c_spirit_brush_slot_type["nPage"] = sol::property([](const ExternalMessage::G2C_SPIRIT_BRUSH_SLOT& self) { return self.nPage; }, [](ExternalMessage::G2C_SPIRIT_BRUSH_SLOT& self, decltype(ExternalMessage::G2C_SPIRIT_BRUSH_SLOT::nPage) value) { self.nPage = value; });
    external_message_g2c_spirit_brush_slot_type["nIndex"] = sol::property([](const ExternalMessage::G2C_SPIRIT_BRUSH_SLOT& self) { return self.nIndex; }, [](ExternalMessage::G2C_SPIRIT_BRUSH_SLOT& self, decltype(ExternalMessage::G2C_SPIRIT_BRUSH_SLOT::nIndex) value) { self.nIndex = value; });
    external_message_g2c_spirit_brush_slot_type["nNumber"] = sol::property([](const ExternalMessage::G2C_SPIRIT_BRUSH_SLOT& self) { return self.nNumber; }, [](ExternalMessage::G2C_SPIRIT_BRUSH_SLOT& self, decltype(ExternalMessage::G2C_SPIRIT_BRUSH_SLOT::nNumber) value) { self.nNumber = value; });
    auto external_message_g2c_upgrade_soul_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_UPGRADE_SOUL>(L, external_message_namespace, "G2C_UPGRADE_SOUL", "ExternalMessage::G2C_UPGRADE_SOUL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_upgrade_soul_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_UPGRADE_SOUL(); });
    external_message_g2c_upgrade_soul_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_UPGRADE_SOUL>(new ExternalMessage::G2C_UPGRADE_SOUL()); });
    external_message_g2c_upgrade_soul_type["byResult"] = sol::property([](const ExternalMessage::G2C_UPGRADE_SOUL& self) { return self.byResult; }, [](ExternalMessage::G2C_UPGRADE_SOUL& self, decltype(ExternalMessage::G2C_UPGRADE_SOUL::byResult) value) { self.byResult = value; });
    auto external_message_g2c_add_soul_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ADD_SOUL>(L, external_message_namespace, "G2C_ADD_SOUL", "ExternalMessage::G2C_ADD_SOUL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_add_soul_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ADD_SOUL(); });
    external_message_g2c_add_soul_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ADD_SOUL>(new ExternalMessage::G2C_ADD_SOUL()); });
    external_message_g2c_add_soul_type["byResult"] = sol::property([](const ExternalMessage::G2C_ADD_SOUL& self) { return self.byResult; }, [](ExternalMessage::G2C_ADD_SOUL& self, decltype(ExternalMessage::G2C_ADD_SOUL::byResult) value) { self.byResult = value; });
    auto external_message_g2c_remove_soul_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_REMOVE_SOUL>(L, external_message_namespace, "G2C_REMOVE_SOUL", "ExternalMessage::G2C_REMOVE_SOUL", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_remove_soul_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_REMOVE_SOUL(); });
    external_message_g2c_remove_soul_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_REMOVE_SOUL>(new ExternalMessage::G2C_REMOVE_SOUL()); });
    external_message_g2c_remove_soul_type["byResult"] = sol::property([](const ExternalMessage::G2C_REMOVE_SOUL& self) { return self.byResult; }, [](ExternalMessage::G2C_REMOVE_SOUL& self, decltype(ExternalMessage::G2C_REMOVE_SOUL::byResult) value) { self.byResult = value; });
    auto external_message_g2c_rune_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RUNE_ITEM>(L, external_message_namespace, "G2C_RUNE_ITEM", "ExternalMessage::G2C_RUNE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_rune_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RUNE_ITEM(); });
    external_message_g2c_rune_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RUNE_ITEM>(new ExternalMessage::G2C_RUNE_ITEM()); });
    external_message_g2c_rune_item_type["byResult"] = sol::property([](const ExternalMessage::G2C_RUNE_ITEM& self) { return self.byResult; }, [](ExternalMessage::G2C_RUNE_ITEM& self, decltype(ExternalMessage::G2C_RUNE_ITEM::byResult) value) { self.byResult = value; });
    auto external_message_g2c_refine_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_REFINE>(L, external_message_namespace, "G2C_REFINE", "ExternalMessage::G2C_REFINE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_refine_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_REFINE(); });
    external_message_g2c_refine_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_REFINE>(new ExternalMessage::G2C_REFINE()); });
    external_message_g2c_refine_type["bySlot"] = sol::property([](const ExternalMessage::G2C_REFINE& self) { return self.bySlot; }, [](ExternalMessage::G2C_REFINE& self, decltype(ExternalMessage::G2C_REFINE::bySlot) value) { self.bySlot = value; });
    external_message_g2c_refine_type["byResult"] = sol::property([](const ExternalMessage::G2C_REFINE& self) { return self.byResult; }, [](ExternalMessage::G2C_REFINE& self, decltype(ExternalMessage::G2C_REFINE::byResult) value) { self.byResult = value; });
    external_message_g2c_refine_type["wRefineLevel"] = sol::property([](const ExternalMessage::G2C_REFINE& self) { return self.wRefineLevel; }, [](ExternalMessage::G2C_REFINE& self, decltype(ExternalMessage::G2C_REFINE::wRefineLevel) value) { self.wRefineLevel = value; });
    auto external_message_g2c_randomize_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RANDOMIZE_ITEM>(L, external_message_namespace, "G2C_RANDOMIZE_ITEM", "ExternalMessage::G2C_RANDOMIZE_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_randomize_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RANDOMIZE_ITEM(); });
    external_message_g2c_randomize_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RANDOMIZE_ITEM>(new ExternalMessage::G2C_RANDOMIZE_ITEM()); });
    external_message_g2c_randomize_item_type["wRandomAttrID"] = cqyh::lua::array_property(&ExternalMessage::G2C_RANDOMIZE_ITEM::wRandomAttrID);
    auto external_message_g2c_identify_item_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_IDENTIFY_ITEM>(L, external_message_namespace, "G2C_IDENTIFY_ITEM", "ExternalMessage::G2C_IDENTIFY_ITEM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_identify_item_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_IDENTIFY_ITEM(); });
    external_message_g2c_identify_item_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_IDENTIFY_ITEM>(new ExternalMessage::G2C_IDENTIFY_ITEM()); });
    external_message_g2c_identify_item_type["byResult"] = sol::property([](const ExternalMessage::G2C_IDENTIFY_ITEM& self) { return self.byResult; }, [](ExternalMessage::G2C_IDENTIFY_ITEM& self, decltype(ExternalMessage::G2C_IDENTIFY_ITEM::byResult) value) { self.byResult = value; });
    auto external_message_g2c_rune_item_advance_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RUNE_ITEM_ADVANCE>(L, external_message_namespace, "G2C_RUNE_ITEM_ADVANCE", "ExternalMessage::G2C_RUNE_ITEM_ADVANCE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_rune_item_advance_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RUNE_ITEM_ADVANCE(); });
    external_message_g2c_rune_item_advance_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RUNE_ITEM_ADVANCE>(new ExternalMessage::G2C_RUNE_ITEM_ADVANCE()); });
    external_message_g2c_rune_item_advance_type["byResult"] = sol::property([](const ExternalMessage::G2C_RUNE_ITEM_ADVANCE& self) { return self.byResult; }, [](ExternalMessage::G2C_RUNE_ITEM_ADVANCE& self, decltype(ExternalMessage::G2C_RUNE_ITEM_ADVANCE::byResult) value) { self.byResult = value; });
    external_message_g2c_rune_item_advance_type["wRune"] = cqyh::lua::array_property(&ExternalMessage::G2C_RUNE_ITEM_ADVANCE::wRune);
    auto external_message_g2c_rune_item_advance_confirm_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM>(L, external_message_namespace, "G2C_RUNE_ITEM_ADVANCE_CONFIRM", "ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_rune_item_advance_confirm_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM(); });
    external_message_g2c_rune_item_advance_confirm_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM>(new ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM()); });
    external_message_g2c_rune_item_advance_confirm_type["byResult"] = sol::property([](const ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM& self) { return self.byResult; }, [](ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM& self, decltype(ExternalMessage::G2C_RUNE_ITEM_ADVANCE_CONFIRM::byResult) value) { self.byResult = value; });
}

}
