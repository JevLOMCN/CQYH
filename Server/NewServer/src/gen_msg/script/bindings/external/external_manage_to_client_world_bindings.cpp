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

void register_external_manage_to_client_world_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_line_scene_data_type = cqyh::gen_msg::usertype<ExternalMessage::LINE_SCENE_DATA>(L, external_message_namespace, "LINE_SCENE_DATA", "ExternalMessage::LINE_SCENE_DATA");
    external_message_line_scene_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::LINE_SCENE_DATA(); });
    external_message_line_scene_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::LINE_SCENE_DATA>(new ExternalMessage::LINE_SCENE_DATA()); });
    external_message_line_scene_data_type["dwSceneID"] = sol::property([](const ExternalMessage::LINE_SCENE_DATA& self) { return self.dwSceneID; }, [](ExternalMessage::LINE_SCENE_DATA& self, decltype(ExternalMessage::LINE_SCENE_DATA::dwSceneID) value) { self.dwSceneID = value; });
    external_message_line_scene_data_type["byState"] = sol::property([](const ExternalMessage::LINE_SCENE_DATA& self) { return self.byState; }, [](ExternalMessage::LINE_SCENE_DATA& self, decltype(ExternalMessage::LINE_SCENE_DATA::byState) value) { self.byState = value; });
    auto external_message_m2c_query_line_ack_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_QUERY_LINE_ACK>(L, external_message_namespace, "M2C_QUERY_LINE_ACK", "ExternalMessage::M2C_QUERY_LINE_ACK", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_query_line_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_QUERY_LINE_ACK(); });
    external_message_m2c_query_line_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_QUERY_LINE_ACK>(new ExternalMessage::M2C_QUERY_LINE_ACK()); });
    external_message_m2c_query_line_ack_type["wCount"] = sol::property([](const ExternalMessage::M2C_QUERY_LINE_ACK& self) { return self.wCount; }, [](ExternalMessage::M2C_QUERY_LINE_ACK& self, decltype(ExternalMessage::M2C_QUERY_LINE_ACK::wCount) value) { self.wCount = value; });
    external_message_m2c_query_line_ack_type["dwSceneTempID"] = sol::property([](const ExternalMessage::M2C_QUERY_LINE_ACK& self) { return self.dwSceneTempID; }, [](ExternalMessage::M2C_QUERY_LINE_ACK& self, decltype(ExternalMessage::M2C_QUERY_LINE_ACK::dwSceneTempID) value) { self.dwSceneTempID = value; });
    external_message_m2c_query_line_ack_type["LineSceneData"] = cqyh::lua::array_property(&ExternalMessage::M2C_QUERY_LINE_ACK::LineSceneData);
}

}
