#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_world_objects_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_operate_doodad_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_OPERATE_DOODAD>(L, external_message_namespace, "C2G_OPERATE_DOODAD", "ExternalMessage::C2G_OPERATE_DOODAD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_operate_doodad_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_OPERATE_DOODAD(); });
    external_message_c2g_operate_doodad_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_OPERATE_DOODAD>(new ExternalMessage::C2G_OPERATE_DOODAD()); });
    external_message_c2g_operate_doodad_type["dwDoodadID"] = sol::property([](const ExternalMessage::C2G_OPERATE_DOODAD& self) { return self.dwDoodadID; }, [](ExternalMessage::C2G_OPERATE_DOODAD& self, decltype(ExternalMessage::C2G_OPERATE_DOODAD::dwDoodadID) value) { self.dwDoodadID = value; });
    auto external_message_c2g_interact_with_npc_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_INTERACT_WITH_NPC>(L, external_message_namespace, "C2G_INTERACT_WITH_NPC", "ExternalMessage::C2G_INTERACT_WITH_NPC", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_interact_with_npc_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_INTERACT_WITH_NPC(); });
    external_message_c2g_interact_with_npc_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_INTERACT_WITH_NPC>(new ExternalMessage::C2G_INTERACT_WITH_NPC()); });
    external_message_c2g_interact_with_npc_type["dwNpcID"] = sol::property([](const ExternalMessage::C2G_INTERACT_WITH_NPC& self) { return self.dwNpcID; }, [](ExternalMessage::C2G_INTERACT_WITH_NPC& self, decltype(ExternalMessage::C2G_INTERACT_WITH_NPC::dwNpcID) value) { self.dwNpcID = value; });
    auto external_message_c2g_stop_interact_with_npc_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_STOP_INTERACT_WITH_NPC>(L, external_message_namespace, "C2G_STOP_INTERACT_WITH_NPC", "ExternalMessage::C2G_STOP_INTERACT_WITH_NPC", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_stop_interact_with_npc_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_STOP_INTERACT_WITH_NPC(); });
    external_message_c2g_stop_interact_with_npc_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_STOP_INTERACT_WITH_NPC>(new ExternalMessage::C2G_STOP_INTERACT_WITH_NPC()); });
    auto external_message_c2g_talk_to_npc_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_TALK_TO_NPC>(L, external_message_namespace, "C2G_TALK_TO_NPC", "ExternalMessage::C2G_TALK_TO_NPC", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_talk_to_npc_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_TALK_TO_NPC(); });
    external_message_c2g_talk_to_npc_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_TALK_TO_NPC>(new ExternalMessage::C2G_TALK_TO_NPC()); });
    external_message_c2g_talk_to_npc_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_TALK_TO_NPC& self) { return self.nQuestID; }, [](ExternalMessage::C2G_TALK_TO_NPC& self, decltype(ExternalMessage::C2G_TALK_TO_NPC::nQuestID) value) { self.nQuestID = value; });
    external_message_c2g_talk_to_npc_type["dwTalkNpcID"] = sol::property([](const ExternalMessage::C2G_TALK_TO_NPC& self) { return self.dwTalkNpcID; }, [](ExternalMessage::C2G_TALK_TO_NPC& self, decltype(ExternalMessage::C2G_TALK_TO_NPC::dwTalkNpcID) value) { self.dwTalkNpcID = value; });
}

}
