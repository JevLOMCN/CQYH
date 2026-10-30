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

void register_external_game_to_client_common_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_sabac_enroll_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST>(L, external_message_namespace, "G2C_SYNC_SABAC_ENROLL_LIST", "ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    auto external_message_revive_info_type = cqyh::gen_msg::usertype<ExternalMessage::REVIVE_INFO>(L, external_message_namespace, "REVIVE_INFO", "ExternalMessage::REVIVE_INFO");
    external_message_revive_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::REVIVE_INFO(); });
    external_message_revive_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::REVIVE_INFO>(new ExternalMessage::REVIVE_INFO()); });
    external_message_revive_info_type["byReviveType"] = sol::property([](const ExternalMessage::REVIVE_INFO& self) { return self.byReviveType; }, [](ExternalMessage::REVIVE_INFO& self, decltype(ExternalMessage::REVIVE_INFO::byReviveType) value) { self.byReviveType = value; });
    external_message_revive_info_type["nRequireItemID"] = sol::property([](const ExternalMessage::REVIVE_INFO& self) { return self.nRequireItemID; }, [](ExternalMessage::REVIVE_INFO& self, decltype(ExternalMessage::REVIVE_INFO::nRequireItemID) value) { self.nRequireItemID = value; });
    external_message_revive_info_type["nRequireItemCount"] = sol::property([](const ExternalMessage::REVIVE_INFO& self) { return self.nRequireItemCount; }, [](ExternalMessage::REVIVE_INFO& self, decltype(ExternalMessage::REVIVE_INFO::nRequireItemCount) value) { self.nRequireItemCount = value; });
    auto external_message_g2c_revive_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_REVIVE_INFO>(L, external_message_namespace, "G2C_REVIVE_INFO", "ExternalMessage::G2C_REVIVE_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_revive_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_REVIVE_INFO(); });
    external_message_g2c_revive_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_REVIVE_INFO>(new ExternalMessage::G2C_REVIVE_INFO()); });
    external_message_g2c_revive_info_type["ReviveInfo"] = cqyh::lua::array_property(&ExternalMessage::G2C_REVIVE_INFO::ReviveInfo);
    external_message_g2c_revive_info_type["nWaitTime"] = sol::property([](const ExternalMessage::G2C_REVIVE_INFO& self) { return self.nWaitTime; }, [](ExternalMessage::G2C_REVIVE_INFO& self, decltype(ExternalMessage::G2C_REVIVE_INFO::nWaitTime) value) { self.nWaitTime = value; });
    external_message_g2c_revive_info_type["dwKillerID"] = sol::property([](const ExternalMessage::G2C_REVIVE_INFO& self) { return self.dwKillerID; }, [](ExternalMessage::G2C_REVIVE_INFO& self, decltype(ExternalMessage::G2C_REVIVE_INFO::dwKillerID) value) { self.dwKillerID = value; });
    auto external_message_g2c_activate_revive_pt_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACTIVATE_REVIVE_PT>(L, external_message_namespace, "G2C_ACTIVATE_REVIVE_PT", "ExternalMessage::G2C_ACTIVATE_REVIVE_PT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_activate_revive_pt_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACTIVATE_REVIVE_PT(); });
    external_message_g2c_activate_revive_pt_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACTIVATE_REVIVE_PT>(new ExternalMessage::G2C_ACTIVATE_REVIVE_PT()); });
    external_message_g2c_activate_revive_pt_type["nX"] = sol::property([](const ExternalMessage::G2C_ACTIVATE_REVIVE_PT& self) { return self.nX; }, [](ExternalMessage::G2C_ACTIVATE_REVIVE_PT& self, decltype(ExternalMessage::G2C_ACTIVATE_REVIVE_PT::nX) value) { self.nX = value; });
    external_message_g2c_activate_revive_pt_type["nY"] = sol::property([](const ExternalMessage::G2C_ACTIVATE_REVIVE_PT& self) { return self.nY; }, [](ExternalMessage::G2C_ACTIVATE_REVIVE_PT& self, decltype(ExternalMessage::G2C_ACTIVATE_REVIVE_PT::nY) value) { self.nY = value; });
    external_message_g2c_activate_revive_pt_type["nZ"] = sol::property([](const ExternalMessage::G2C_ACTIVATE_REVIVE_PT& self) { return self.nZ; }, [](ExternalMessage::G2C_ACTIVATE_REVIVE_PT& self, decltype(ExternalMessage::G2C_ACTIVATE_REVIVE_PT::nZ) value) { self.nZ = value; });
    auto external_message_g2c_object_disappear_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_OBJECT_DISAPPEAR>(L, external_message_namespace, "G2C_OBJECT_DISAPPEAR", "ExternalMessage::G2C_OBJECT_DISAPPEAR", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_object_disappear_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_OBJECT_DISAPPEAR(); });
    external_message_g2c_object_disappear_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_OBJECT_DISAPPEAR>(new ExternalMessage::G2C_OBJECT_DISAPPEAR()); });
    external_message_g2c_object_disappear_type["dwObjID"] = sol::property([](const ExternalMessage::G2C_OBJECT_DISAPPEAR& self) { return self.dwObjID; }, [](ExternalMessage::G2C_OBJECT_DISAPPEAR& self, decltype(ExternalMessage::G2C_OBJECT_DISAPPEAR::dwObjID) value) { self.dwObjID = value; });
    external_message_g2c_object_disappear_type["byDisappearType"] = sol::property([](const ExternalMessage::G2C_OBJECT_DISAPPEAR& self) { return self.byDisappearType; }, [](ExternalMessage::G2C_OBJECT_DISAPPEAR& self, decltype(ExternalMessage::G2C_OBJECT_DISAPPEAR::byDisappearType) value) { self.byDisappearType = value; });
    auto external_message_g2c_object_disappear_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST>(L, external_message_namespace, "G2C_OBJECT_DISAPPEAR_LIST", "ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_object_disappear_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST(); });
    external_message_g2c_object_disappear_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST>(new ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST()); });
    external_message_g2c_object_disappear_list_type["dwObjIDList"] = cqyh::lua::array_property(&ExternalMessage::G2C_OBJECT_DISAPPEAR_LIST::dwObjIDList);
    auto external_message_g2c_update_weather_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_UPDATE_WEATHER>(L, external_message_namespace, "G2C_UPDATE_WEATHER", "ExternalMessage::G2C_UPDATE_WEATHER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_update_weather_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_UPDATE_WEATHER(); });
    external_message_g2c_update_weather_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_UPDATE_WEATHER>(new ExternalMessage::G2C_UPDATE_WEATHER()); });
    external_message_g2c_update_weather_type["byWeather"] = sol::property([](const ExternalMessage::G2C_UPDATE_WEATHER& self) { return self.byWeather; }, [](ExternalMessage::G2C_UPDATE_WEATHER& self, decltype(ExternalMessage::G2C_UPDATE_WEATHER::byWeather) value) { self.byWeather = value; });
    auto external_message_g2c_interact_result_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_INTERACT_RESULT>(L, external_message_namespace, "G2C_INTERACT_RESULT", "ExternalMessage::G2C_INTERACT_RESULT", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_interact_result_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_INTERACT_RESULT(); });
    external_message_g2c_interact_result_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_INTERACT_RESULT>(new ExternalMessage::G2C_INTERACT_RESULT()); });
    external_message_g2c_interact_result_type["dwNpcID"] = sol::property([](const ExternalMessage::G2C_INTERACT_RESULT& self) { return self.dwNpcID; }, [](ExternalMessage::G2C_INTERACT_RESULT& self, decltype(ExternalMessage::G2C_INTERACT_RESULT::dwNpcID) value) { self.dwNpcID = value; });
    external_message_g2c_interact_result_type["szDialogContent"] = cqyh::lua::string_property(&ExternalMessage::G2C_INTERACT_RESULT::szDialogContent);
    auto external_message_g2c_activate_hankbook_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACTIVATE_HANKBOOK>(L, external_message_namespace, "G2C_ACTIVATE_HANKBOOK", "ExternalMessage::G2C_ACTIVATE_HANKBOOK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_activate_hankbook_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACTIVATE_HANKBOOK(); });
    external_message_g2c_activate_hankbook_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACTIVATE_HANKBOOK>(new ExternalMessage::G2C_ACTIVATE_HANKBOOK()); });
    external_message_g2c_activate_hankbook_type["nType"] = sol::property([](const ExternalMessage::G2C_ACTIVATE_HANKBOOK& self) { return self.nType; }, [](ExternalMessage::G2C_ACTIVATE_HANKBOOK& self, decltype(ExternalMessage::G2C_ACTIVATE_HANKBOOK::nType) value) { self.nType = value; });
    external_message_g2c_activate_hankbook_type["nIndex"] = sol::property([](const ExternalMessage::G2C_ACTIVATE_HANKBOOK& self) { return self.nIndex; }, [](ExternalMessage::G2C_ACTIVATE_HANKBOOK& self, decltype(ExternalMessage::G2C_ACTIVATE_HANKBOOK::nIndex) value) { self.nIndex = value; });
    auto external_message_g2c_sync_sabac_enroll_list_enroll_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA>(L, external_message_g2c_sync_sabac_enroll_list_type, "ENROLL_DATA", "ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA");
    external_message_g2c_sync_sabac_enroll_list_enroll_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA(); });
    external_message_g2c_sync_sabac_enroll_list_enroll_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA>(new ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA()); });
    external_message_g2c_sync_sabac_enroll_list_enroll_data_type["dwGuildID"] = sol::property([](const ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA& self) { return self.dwGuildID; }, [](ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA& self, decltype(ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA::dwGuildID) value) { self.dwGuildID = value; });
    external_message_g2c_sync_sabac_enroll_list_enroll_data_type["dwEnrollTime"] = sol::property([](const ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA& self) { return self.dwEnrollTime; }, [](ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA& self, decltype(ExternalMessage::G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA::dwEnrollTime) value) { self.dwEnrollTime = value; });
}

}
