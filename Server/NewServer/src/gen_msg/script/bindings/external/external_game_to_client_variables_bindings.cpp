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

void register_external_game_to_client_variables_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_scene_variables_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SCENE_VARIABLES>(L, external_message_namespace, "G2C_SYNC_SCENE_VARIABLES", "ExternalMessage::G2C_SYNC_SCENE_VARIABLES", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_scene_variables_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_SCENE_VARIABLES(); });
    external_message_g2c_sync_scene_variables_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_SCENE_VARIABLES>(new ExternalMessage::G2C_SYNC_SCENE_VARIABLES()); });
    auto external_message_g2c_sync_scene_variables_variable_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA>(L, external_message_g2c_sync_scene_variables_type, "VARIABLE_DATA", "ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA");
    external_message_g2c_sync_scene_variables_variable_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA(); });
    external_message_g2c_sync_scene_variables_variable_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA>(new ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA()); });
    external_message_g2c_sync_scene_variables_variable_data_type["wVarIndex"] = sol::property([](const ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA& self) { return self.wVarIndex; }, [](ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA& self, decltype(ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA::wVarIndex) value) { self.wVarIndex = value; });
    external_message_g2c_sync_scene_variables_variable_data_type["nValue"] = sol::property([](const ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA& self) { return self.nValue; }, [](ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA& self, decltype(ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA::nValue) value) { self.nValue = value; });
    external_message_g2c_sync_scene_variables_type["VarList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_SCENE_VARIABLES::VarList);
    auto external_message_g2c_update_variable_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_UPDATE_VARIABLE>(L, external_message_namespace, "G2C_UPDATE_VARIABLE", "ExternalMessage::G2C_UPDATE_VARIABLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_update_variable_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_UPDATE_VARIABLE(); });
    external_message_g2c_update_variable_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_UPDATE_VARIABLE>(new ExternalMessage::G2C_UPDATE_VARIABLE()); });
    external_message_g2c_update_variable_type["byType"] = sol::property([](const ExternalMessage::G2C_UPDATE_VARIABLE& self) { return self.byType; }, [](ExternalMessage::G2C_UPDATE_VARIABLE& self, decltype(ExternalMessage::G2C_UPDATE_VARIABLE::byType) value) { self.byType = value; });
    external_message_g2c_update_variable_type["wIndex"] = sol::property([](const ExternalMessage::G2C_UPDATE_VARIABLE& self) { return self.wIndex; }, [](ExternalMessage::G2C_UPDATE_VARIABLE& self, decltype(ExternalMessage::G2C_UPDATE_VARIABLE::wIndex) value) { self.wIndex = value; });
    external_message_g2c_update_variable_type["nReferID"] = sol::property([](const ExternalMessage::G2C_UPDATE_VARIABLE& self) { return self.nReferID; }, [](ExternalMessage::G2C_UPDATE_VARIABLE& self, decltype(ExternalMessage::G2C_UPDATE_VARIABLE::nReferID) value) { self.nReferID = value; });
    external_message_g2c_update_variable_type["nValue"] = sol::property([](const ExternalMessage::G2C_UPDATE_VARIABLE& self) { return self.nValue; }, [](ExternalMessage::G2C_UPDATE_VARIABLE& self, decltype(ExternalMessage::G2C_UPDATE_VARIABLE::nValue) value) { self.nValue = value; });
    auto external_message_g2c_sync_client_variables_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_CLIENT_VARIABLES>(L, external_message_namespace, "G2C_SYNC_CLIENT_VARIABLES", "ExternalMessage::G2C_SYNC_CLIENT_VARIABLES", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_client_variables_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_CLIENT_VARIABLES(); });
    external_message_g2c_sync_client_variables_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_CLIENT_VARIABLES>(new ExternalMessage::G2C_SYNC_CLIENT_VARIABLES()); });
    external_message_g2c_sync_client_variables_type["nVariableList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_CLIENT_VARIABLES::nVariableList);
    auto external_message_g2c_sync_role_variables_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_VARIABLES>(L, external_message_namespace, "G2C_SYNC_ROLE_VARIABLES", "ExternalMessage::G2C_SYNC_ROLE_VARIABLES", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_role_variables_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_VARIABLES(); });
    external_message_g2c_sync_role_variables_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_VARIABLES>(new ExternalMessage::G2C_SYNC_ROLE_VARIABLES()); });
    auto external_message_g2c_sync_role_variables_variable_data_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA>(L, external_message_g2c_sync_role_variables_type, "VARIABLE_DATA", "ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA");
    external_message_g2c_sync_role_variables_variable_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA(); });
    external_message_g2c_sync_role_variables_variable_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA>(new ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA()); });
    external_message_g2c_sync_role_variables_variable_data_type["wVarIndex"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA& self) { return self.wVarIndex; }, [](ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA::wVarIndex) value) { self.wVarIndex = value; });
    external_message_g2c_sync_role_variables_variable_data_type["nValue"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA& self) { return self.nValue; }, [](ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA& self, decltype(ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA::nValue) value) { self.nValue = value; });
    external_message_g2c_sync_role_variables_type["VarList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_ROLE_VARIABLES::VarList);
    auto external_message_g2c_update_achievement_variable_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE>(L, external_message_namespace, "G2C_UPDATE_ACHIEVEMENT_VARIABLE", "ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_update_achievement_variable_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE(); });
    external_message_g2c_update_achievement_variable_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE>(new ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE()); });
    external_message_g2c_update_achievement_variable_type["nIndex"] = sol::property([](const ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE& self) { return self.nIndex; }, [](ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE& self, decltype(ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE::nIndex) value) { self.nIndex = value; });
    external_message_g2c_update_achievement_variable_type["nValue"] = sol::property([](const ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE& self) { return self.nValue; }, [](ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE& self, decltype(ExternalMessage::G2C_UPDATE_ACHIEVEMENT_VARIABLE::nValue) value) { self.nValue = value; });
}

}
