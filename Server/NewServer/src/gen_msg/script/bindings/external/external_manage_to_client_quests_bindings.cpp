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

void register_external_manage_to_client_quests_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_publish_quest_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_PUBLISH_QUEST>(L, external_message_namespace, "M2C_PUBLISH_QUEST", "ExternalMessage::M2C_PUBLISH_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_publish_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_PUBLISH_QUEST(); });
    external_message_m2c_publish_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_PUBLISH_QUEST>(new ExternalMessage::M2C_PUBLISH_QUEST()); });
    external_message_m2c_publish_quest_type["dwMasterRoleID"] = sol::property([](const ExternalMessage::M2C_PUBLISH_QUEST& self) { return self.dwMasterRoleID; }, [](ExternalMessage::M2C_PUBLISH_QUEST& self, decltype(ExternalMessage::M2C_PUBLISH_QUEST::dwMasterRoleID) value) { self.dwMasterRoleID = value; });
    external_message_m2c_publish_quest_type["nQuestID"] = sol::property([](const ExternalMessage::M2C_PUBLISH_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::M2C_PUBLISH_QUEST& self, decltype(ExternalMessage::M2C_PUBLISH_QUEST::nQuestID) value) { self.nQuestID = value; });
}

}
