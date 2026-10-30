#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_quests_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_quest_recycle_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUEST_RECYCLE>(L, external_message_namespace, "C2G_QUEST_RECYCLE", "ExternalMessage::C2G_QUEST_RECYCLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_quest_recycle_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUEST_RECYCLE(); });
    external_message_c2g_quest_recycle_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUEST_RECYCLE>(new ExternalMessage::C2G_QUEST_RECYCLE()); });
    external_message_c2g_quest_recycle_type["nTemplateID"] = sol::property([](const ExternalMessage::C2G_QUEST_RECYCLE& self) { return self.nTemplateID; }, [](ExternalMessage::C2G_QUEST_RECYCLE& self, decltype(ExternalMessage::C2G_QUEST_RECYCLE::nTemplateID) value) { self.nTemplateID = value; });
    external_message_c2g_quest_recycle_type["nIndex"] = sol::property([](const ExternalMessage::C2G_QUEST_RECYCLE& self) { return self.nIndex; }, [](ExternalMessage::C2G_QUEST_RECYCLE& self, decltype(ExternalMessage::C2G_QUEST_RECYCLE::nIndex) value) { self.nIndex = value; });
    auto external_message_c2g_complete_kill_npc_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST>(L, external_message_namespace, "C2G_COMPLETE_KILL_NPC_QUEST", "ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_complete_kill_npc_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST(); });
    external_message_c2g_complete_kill_npc_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST>(new ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST()); });
    external_message_c2g_complete_kill_npc_quest_type["wQuestID"] = sol::property([](const ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST& self) { return self.wQuestID; }, [](ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST& self, decltype(ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST::wQuestID) value) { self.wQuestID = value; });
    external_message_c2g_complete_kill_npc_quest_type["byIndex"] = sol::property([](const ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST& self) { return self.byIndex; }, [](ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST& self, decltype(ExternalMessage::C2G_COMPLETE_KILL_NPC_QUEST::byIndex) value) { self.byIndex = value; });
    auto external_message_c2g_accept_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ACCEPT_QUEST>(L, external_message_namespace, "C2G_ACCEPT_QUEST", "ExternalMessage::C2G_ACCEPT_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_accept_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ACCEPT_QUEST(); });
    external_message_c2g_accept_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ACCEPT_QUEST>(new ExternalMessage::C2G_ACCEPT_QUEST()); });
    external_message_c2g_accept_quest_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_ACCEPT_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::C2G_ACCEPT_QUEST& self, decltype(ExternalMessage::C2G_ACCEPT_QUEST::nQuestID) value) { self.nQuestID = value; });
    external_message_c2g_accept_quest_type["nAcceptType"] = sol::property([](const ExternalMessage::C2G_ACCEPT_QUEST& self) { return self.nAcceptType; }, [](ExternalMessage::C2G_ACCEPT_QUEST& self, decltype(ExternalMessage::C2G_ACCEPT_QUEST::nAcceptType) value) { self.nAcceptType = value; });
    external_message_c2g_accept_quest_type["nAcceptSrcID"] = sol::property([](const ExternalMessage::C2G_ACCEPT_QUEST& self) { return self.nAcceptSrcID; }, [](ExternalMessage::C2G_ACCEPT_QUEST& self, decltype(ExternalMessage::C2G_ACCEPT_QUEST::nAcceptSrcID) value) { self.nAcceptSrcID = value; });
    auto external_message_c2g_abandon_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ABANDON_QUEST>(L, external_message_namespace, "C2G_ABANDON_QUEST", "ExternalMessage::C2G_ABANDON_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_abandon_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ABANDON_QUEST(); });
    external_message_c2g_abandon_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ABANDON_QUEST>(new ExternalMessage::C2G_ABANDON_QUEST()); });
    external_message_c2g_abandon_quest_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_ABANDON_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::C2G_ABANDON_QUEST& self, decltype(ExternalMessage::C2G_ABANDON_QUEST::nQuestID) value) { self.nQuestID = value; });
    auto external_message_c2g_complete_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_COMPLETE_QUEST>(L, external_message_namespace, "C2G_COMPLETE_QUEST", "ExternalMessage::C2G_COMPLETE_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_complete_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_COMPLETE_QUEST(); });
    external_message_c2g_complete_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_COMPLETE_QUEST>(new ExternalMessage::C2G_COMPLETE_QUEST()); });
    external_message_c2g_complete_quest_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_COMPLETE_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::C2G_COMPLETE_QUEST& self, decltype(ExternalMessage::C2G_COMPLETE_QUEST::nQuestID) value) { self.nQuestID = value; });
    external_message_c2g_complete_quest_type["nSelectedAwardIdx"] = sol::property([](const ExternalMessage::C2G_COMPLETE_QUEST& self) { return self.nSelectedAwardIdx; }, [](ExternalMessage::C2G_COMPLETE_QUEST& self, decltype(ExternalMessage::C2G_COMPLETE_QUEST::nSelectedAwardIdx) value) { self.nSelectedAwardIdx = value; });
    auto external_message_c2g_share_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SHARE_QUEST>(L, external_message_namespace, "C2G_SHARE_QUEST", "ExternalMessage::C2G_SHARE_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_share_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SHARE_QUEST(); });
    external_message_c2g_share_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SHARE_QUEST>(new ExternalMessage::C2G_SHARE_QUEST()); });
    external_message_c2g_share_quest_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_SHARE_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::C2G_SHARE_QUEST& self, decltype(ExternalMessage::C2G_SHARE_QUEST::nQuestID) value) { self.nQuestID = value; });
    external_message_c2g_share_quest_type["nShareType"] = sol::property([](const ExternalMessage::C2G_SHARE_QUEST& self) { return self.nShareType; }, [](ExternalMessage::C2G_SHARE_QUEST& self, decltype(ExternalMessage::C2G_SHARE_QUEST::nShareType) value) { self.nShareType = value; });
    auto external_message_c2g_confirm_publish_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST>(L, external_message_namespace, "C2G_CONFIRM_PUBLISH_QUEST", "ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_confirm_publish_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST(); });
    external_message_c2g_confirm_publish_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST>(new ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST()); });
    external_message_c2g_confirm_publish_quest_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST& self, decltype(ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST::nQuestID) value) { self.nQuestID = value; });
    external_message_c2g_confirm_publish_quest_type["dwMasterID"] = sol::property([](const ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST& self) { return self.dwMasterID; }, [](ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST& self, decltype(ExternalMessage::C2G_CONFIRM_PUBLISH_QUEST::dwMasterID) value) { self.dwMasterID = value; });
    auto external_message_c2g_fly_to_quest_zone_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_FLY_TO_QUEST_ZONE>(L, external_message_namespace, "C2G_FLY_TO_QUEST_ZONE", "ExternalMessage::C2G_FLY_TO_QUEST_ZONE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_fly_to_quest_zone_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_FLY_TO_QUEST_ZONE(); });
    external_message_c2g_fly_to_quest_zone_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_FLY_TO_QUEST_ZONE>(new ExternalMessage::C2G_FLY_TO_QUEST_ZONE()); });
    external_message_c2g_fly_to_quest_zone_type["nQuestID"] = sol::property([](const ExternalMessage::C2G_FLY_TO_QUEST_ZONE& self) { return self.nQuestID; }, [](ExternalMessage::C2G_FLY_TO_QUEST_ZONE& self, decltype(ExternalMessage::C2G_FLY_TO_QUEST_ZONE::nQuestID) value) { self.nQuestID = value; });
    external_message_c2g_fly_to_quest_zone_type["nZoneIndex"] = sol::property([](const ExternalMessage::C2G_FLY_TO_QUEST_ZONE& self) { return self.nZoneIndex; }, [](ExternalMessage::C2G_FLY_TO_QUEST_ZONE& self, decltype(ExternalMessage::C2G_FLY_TO_QUEST_ZONE::nZoneIndex) value) { self.nZoneIndex = value; });
    auto external_message_c2g_generate_circle_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GENERATE_CIRCLE_QUEST>(L, external_message_namespace, "C2G_GENERATE_CIRCLE_QUEST", "ExternalMessage::C2G_GENERATE_CIRCLE_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_generate_circle_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GENERATE_CIRCLE_QUEST(); });
    external_message_c2g_generate_circle_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GENERATE_CIRCLE_QUEST>(new ExternalMessage::C2G_GENERATE_CIRCLE_QUEST()); });
    auto external_message_c2g_buy_daily_quest_complete_time_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME>(L, external_message_namespace, "C2G_BUY_DAILY_QUEST_COMPLETE_TIME", "ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_buy_daily_quest_complete_time_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME(); });
    external_message_c2g_buy_daily_quest_complete_time_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME>(new ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME()); });
    external_message_c2g_buy_daily_quest_complete_time_type["nTime"] = sol::property([](const ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME& self) { return self.nTime; }, [](ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME& self, decltype(ExternalMessage::C2G_BUY_DAILY_QUEST_COMPLETE_TIME::nTime) value) { self.nTime = value; });
    auto external_message_c2g_daily_quest_complete_shot_cut_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DAILY_QUEST_COMPLETE_SHOT_CUT>(L, external_message_namespace, "C2G_DAILY_QUEST_COMPLETE_SHOT_CUT", "ExternalMessage::C2G_DAILY_QUEST_COMPLETE_SHOT_CUT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_daily_quest_complete_shot_cut_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DAILY_QUEST_COMPLETE_SHOT_CUT(); });
    external_message_c2g_daily_quest_complete_shot_cut_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DAILY_QUEST_COMPLETE_SHOT_CUT>(new ExternalMessage::C2G_DAILY_QUEST_COMPLETE_SHOT_CUT()); });
    auto external_message_c2g_brush_daily_quest_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_BRUSH_DAILY_QUEST>(L, external_message_namespace, "C2G_BRUSH_DAILY_QUEST", "ExternalMessage::C2G_BRUSH_DAILY_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_brush_daily_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_BRUSH_DAILY_QUEST(); });
    external_message_c2g_brush_daily_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_BRUSH_DAILY_QUEST>(new ExternalMessage::C2G_BRUSH_DAILY_QUEST()); });
    external_message_c2g_brush_daily_quest_type["bUseMoney"] = sol::property([](const ExternalMessage::C2G_BRUSH_DAILY_QUEST& self) { return static_cast<bool>(self.bUseMoney); }, [](ExternalMessage::C2G_BRUSH_DAILY_QUEST& self, bool value) { self.bUseMoney = value; });
}

}
