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

void register_external_manage_to_client_guild_buildings_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g_b_data_type = cqyh::gen_msg::usertype<ExternalMessage::G_B_DATA>(L, external_message_namespace, "G_B_DATA", "ExternalMessage::G_B_DATA");
    external_message_g_b_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G_B_DATA(); });
    external_message_g_b_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G_B_DATA>(new ExternalMessage::G_B_DATA()); });
    external_message_g_b_data_type["qwNameID"] = sol::property([](const ExternalMessage::G_B_DATA& self) { return self.qwNameID; }, [](ExternalMessage::G_B_DATA& self, decltype(ExternalMessage::G_B_DATA::qwNameID) value) { self.qwNameID = value; });
    external_message_g_b_data_type["nType"] = sol::property([](const ExternalMessage::G_B_DATA& self) { return self.nType; }, [](ExternalMessage::G_B_DATA& self, decltype(ExternalMessage::G_B_DATA::nType) value) { self.nType = value; });
    external_message_g_b_data_type["nLevel"] = sol::property([](const ExternalMessage::G_B_DATA& self) { return self.nLevel; }, [](ExternalMessage::G_B_DATA& self, decltype(ExternalMessage::G_B_DATA::nLevel) value) { self.nLevel = value; });
    auto external_message_g_b_u_data_type = cqyh::gen_msg::usertype<ExternalMessage::G_B_U_DATA>(L, external_message_namespace, "G_B_U_DATA", "ExternalMessage::G_B_U_DATA");
    external_message_g_b_u_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G_B_U_DATA(); });
    external_message_g_b_u_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G_B_U_DATA>(new ExternalMessage::G_B_U_DATA()); });
    external_message_g_b_u_data_type["qwNameID"] = sol::property([](const ExternalMessage::G_B_U_DATA& self) { return self.qwNameID; }, [](ExternalMessage::G_B_U_DATA& self, decltype(ExternalMessage::G_B_U_DATA::qwNameID) value) { self.qwNameID = value; });
    external_message_g_b_u_data_type["nTime"] = sol::property([](const ExternalMessage::G_B_U_DATA& self) { return self.nTime; }, [](ExternalMessage::G_B_U_DATA& self, decltype(ExternalMessage::G_B_U_DATA::nTime) value) { self.nTime = value; });
    auto external_message_m2c_guild_upgrade_welfare_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF>(L, external_message_namespace, "M2C_GUILD_UPGRADE_WELFARE_NTF", "ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_upgrade_welfare_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF(); });
    external_message_m2c_guild_upgrade_welfare_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF>(new ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF()); });
    external_message_m2c_guild_upgrade_welfare_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF::byType) value) { self.byType = value; });
    external_message_m2c_guild_upgrade_welfare_ntf_type["nTime"] = sol::property([](const ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF& self) { return self.nTime; }, [](ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPGRADE_WELFARE_NTF::nTime) value) { self.nTime = value; });
    auto external_message_m2c_guild_update_welfare_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF>(L, external_message_namespace, "M2C_GUILD_UPDATE_WELFARE_NTF", "ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_update_welfare_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF(); });
    external_message_m2c_guild_update_welfare_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF>(new ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF()); });
    external_message_m2c_guild_update_welfare_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF::byType) value) { self.byType = value; });
    external_message_m2c_guild_update_welfare_ntf_type["byLevel"] = sol::property([](const ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF& self) { return self.byLevel; }, [](ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPDATE_WELFARE_NTF::byLevel) value) { self.byLevel = value; });
    auto external_message_m2c_guild_cancel_welfare_upgrade_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF>(L, external_message_namespace, "M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF", "ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_cancel_welfare_upgrade_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF(); });
    external_message_m2c_guild_cancel_welfare_upgrade_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF>(new ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF()); });
    external_message_m2c_guild_cancel_welfare_upgrade_ntf_type["dwID"] = sol::property([](const ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF& self) { return self.dwID; }, [](ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF& self, decltype(ExternalMessage::M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF::dwID) value) { self.dwID = value; });
    auto external_message_m2c_guild_upgrade_building_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF>(L, external_message_namespace, "M2C_GUILD_UPGRADE_BUILDING_NTF", "ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_upgrade_building_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF(); });
    external_message_m2c_guild_upgrade_building_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF>(new ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF()); });
    external_message_m2c_guild_upgrade_building_ntf_type["qwNpcNameID"] = sol::property([](const ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF& self) { return self.qwNpcNameID; }, [](ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF::qwNpcNameID) value) { self.qwNpcNameID = value; });
    external_message_m2c_guild_upgrade_building_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF::byType) value) { self.byType = value; });
    external_message_m2c_guild_upgrade_building_ntf_type["nTime"] = sol::property([](const ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF& self) { return self.nTime; }, [](ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPGRADE_BUILDING_NTF::nTime) value) { self.nTime = value; });
    auto external_message_m2c_guild_update_building_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF>(L, external_message_namespace, "M2C_GUILD_UPDATE_BUILDING_NTF", "ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_update_building_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF(); });
    external_message_m2c_guild_update_building_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF>(new ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF()); });
    external_message_m2c_guild_update_building_ntf_type["qwNpcNameID"] = sol::property([](const ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF& self) { return self.qwNpcNameID; }, [](ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF::qwNpcNameID) value) { self.qwNpcNameID = value; });
    external_message_m2c_guild_update_building_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_UPDATE_BUILDING_NTF::byType) value) { self.byType = value; });
    auto external_message_m2c_guild_cancel_building_upgrade_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF>(L, external_message_namespace, "M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF", "ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_cancel_building_upgrade_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF(); });
    external_message_m2c_guild_cancel_building_upgrade_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF>(new ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF()); });
    external_message_m2c_guild_cancel_building_upgrade_ntf_type["qwNpcNameID"] = sol::property([](const ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF& self) { return self.qwNpcNameID; }, [](ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF& self, decltype(ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF::qwNpcNameID) value) { self.qwNpcNameID = value; });
    external_message_m2c_guild_cancel_building_upgrade_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF& self, decltype(ExternalMessage::M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF::byType) value) { self.byType = value; });
    auto external_message_m2c_guild_del_building_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF>(L, external_message_namespace, "M2C_GUILD_DEL_BUILDING_NTF", "ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_del_building_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF(); });
    external_message_m2c_guild_del_building_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF>(new ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF()); });
    external_message_m2c_guild_del_building_ntf_type["qwNpcNameID"] = sol::property([](const ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF& self) { return self.qwNpcNameID; }, [](ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF::qwNpcNameID) value) { self.qwNpcNameID = value; });
    external_message_m2c_guild_del_building_ntf_type["byType"] = sol::property([](const ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF& self) { return self.byType; }, [](ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF& self, decltype(ExternalMessage::M2C_GUILD_DEL_BUILDING_NTF::byType) value) { self.byType = value; });
    auto external_message_m2c_guild_building_state_ntf_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF>(L, external_message_namespace, "M2C_GUILD_BUILDING_STATE_NTF", "ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_guild_building_state_ntf_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF(); });
    external_message_m2c_guild_building_state_ntf_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF>(new ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF()); });
    external_message_m2c_guild_building_state_ntf_type["qwNameID"] = sol::property([](const ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF& self) { return self.qwNameID; }, [](ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF& self, decltype(ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF::qwNameID) value) { self.qwNameID = value; });
    external_message_m2c_guild_building_state_ntf_type["dwReviveTime"] = sol::property([](const ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF& self) { return self.dwReviveTime; }, [](ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF& self, decltype(ExternalMessage::M2C_GUILD_BUILDING_STATE_NTF::dwReviveTime) value) { self.dwReviveTime = value; });
}

}
