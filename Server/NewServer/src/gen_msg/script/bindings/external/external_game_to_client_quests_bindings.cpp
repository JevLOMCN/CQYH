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

void register_external_game_to_client_quests_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_quest_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_QUEST_LIST>(L, external_message_namespace, "G2C_SYNC_QUEST_LIST", "ExternalMessage::G2C_SYNC_QUEST_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_quest_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_QUEST_LIST(); });
    external_message_g2c_sync_quest_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_QUEST_LIST>(new ExternalMessage::G2C_SYNC_QUEST_LIST()); });
    external_message_g2c_sync_quest_list_type["szData"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_QUEST_LIST::szData);
    auto external_message_g2c_sync_kill_npc_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_KILL_NPC_QUEST>(L, external_message_namespace, "G2C_SYNC_KILL_NPC_QUEST", "ExternalMessage::G2C_SYNC_KILL_NPC_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_kill_npc_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_KILL_NPC_QUEST(); });
    external_message_g2c_sync_kill_npc_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_KILL_NPC_QUEST>(new ExternalMessage::G2C_SYNC_KILL_NPC_QUEST()); });
    external_message_g2c_sync_kill_npc_quest_type["wKillCount"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_KILL_NPC_QUEST::wKillCount);
    external_message_g2c_sync_kill_npc_quest_type["byCompleteList"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_KILL_NPC_QUEST::byCompleteList);
    auto external_message_g2c_accept_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACCEPT_QUEST>(L, external_message_namespace, "G2C_ACCEPT_QUEST", "ExternalMessage::G2C_ACCEPT_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_accept_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACCEPT_QUEST(); });
    external_message_g2c_accept_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACCEPT_QUEST>(new ExternalMessage::G2C_ACCEPT_QUEST()); });
    external_message_g2c_accept_quest_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_ACCEPT_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::G2C_ACCEPT_QUEST& self, decltype(ExternalMessage::G2C_ACCEPT_QUEST::nQuestID) value) { self.nQuestID = value; });
    auto external_message_g2c_publish_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_PUBLISH_QUEST>(L, external_message_namespace, "G2C_PUBLISH_QUEST", "ExternalMessage::G2C_PUBLISH_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_publish_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_PUBLISH_QUEST(); });
    external_message_g2c_publish_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_PUBLISH_QUEST>(new ExternalMessage::G2C_PUBLISH_QUEST()); });
    external_message_g2c_publish_quest_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_PUBLISH_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::G2C_PUBLISH_QUEST& self, decltype(ExternalMessage::G2C_PUBLISH_QUEST::nQuestID) value) { self.nQuestID = value; });
    auto external_message_g2c_abandon_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ABANDON_QUEST>(L, external_message_namespace, "G2C_ABANDON_QUEST", "ExternalMessage::G2C_ABANDON_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_abandon_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ABANDON_QUEST(); });
    external_message_g2c_abandon_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ABANDON_QUEST>(new ExternalMessage::G2C_ABANDON_QUEST()); });
    external_message_g2c_abandon_quest_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_ABANDON_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::G2C_ABANDON_QUEST& self, decltype(ExternalMessage::G2C_ABANDON_QUEST::nQuestID) value) { self.nQuestID = value; });
    auto external_message_g2c_complete_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COMPLETE_QUEST>(L, external_message_namespace, "G2C_COMPLETE_QUEST", "ExternalMessage::G2C_COMPLETE_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_complete_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COMPLETE_QUEST(); });
    external_message_g2c_complete_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COMPLETE_QUEST>(new ExternalMessage::G2C_COMPLETE_QUEST()); });
    external_message_g2c_complete_quest_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_COMPLETE_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::G2C_COMPLETE_QUEST& self, decltype(ExternalMessage::G2C_COMPLETE_QUEST::nQuestID) value) { self.nQuestID = value; });
    auto external_message_g2c_share_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SHARE_QUEST>(L, external_message_namespace, "G2C_SHARE_QUEST", "ExternalMessage::G2C_SHARE_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_share_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SHARE_QUEST(); });
    external_message_g2c_share_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SHARE_QUEST>(new ExternalMessage::G2C_SHARE_QUEST()); });
    external_message_g2c_share_quest_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_SHARE_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::G2C_SHARE_QUEST& self, decltype(ExternalMessage::G2C_SHARE_QUEST::nQuestID) value) { self.nQuestID = value; });
    external_message_g2c_share_quest_type["nSrcID"] = sol::property([](const ExternalMessage::G2C_SHARE_QUEST& self) { return self.nSrcID; }, [](ExternalMessage::G2C_SHARE_QUEST& self, decltype(ExternalMessage::G2C_SHARE_QUEST::nSrcID) value) { self.nSrcID = value; });
    auto external_message_g2c_notify_next_circle_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST>(L, external_message_namespace, "G2C_NOTIFY_NEXT_CIRCLE_QUEST", "ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_notify_next_circle_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST(); });
    external_message_g2c_notify_next_circle_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST>(new ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST()); });
    external_message_g2c_notify_next_circle_quest_type["nCircleID"] = sol::property([](const ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST& self) { return self.nCircleID; }, [](ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST& self, decltype(ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST::nCircleID) value) { self.nCircleID = value; });
    external_message_g2c_notify_next_circle_quest_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST& self) { return self.nQuestID; }, [](ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST& self, decltype(ExternalMessage::G2C_NOTIFY_NEXT_CIRCLE_QUEST::nQuestID) value) { self.nQuestID = value; });
    auto external_message_g2c_buy_daily_quest_complete_time_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME>(L, external_message_namespace, "G2C_BUY_DAILY_QUEST_COMPLETE_TIME", "ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_buy_daily_quest_complete_time_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME(); });
    external_message_g2c_buy_daily_quest_complete_time_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME>(new ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME()); });
    external_message_g2c_buy_daily_quest_complete_time_type["nCurrentRemainTime"] = sol::property([](const ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME& self) { return self.nCurrentRemainTime; }, [](ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME& self, decltype(ExternalMessage::G2C_BUY_DAILY_QUEST_COMPLETE_TIME::nCurrentRemainTime) value) { self.nCurrentRemainTime = value; });
    auto external_message_g2c_brush_daily_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_BRUSH_DAILY_QUEST>(L, external_message_namespace, "G2C_BRUSH_DAILY_QUEST", "ExternalMessage::G2C_BRUSH_DAILY_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_brush_daily_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_BRUSH_DAILY_QUEST(); });
    external_message_g2c_brush_daily_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_BRUSH_DAILY_QUEST>(new ExternalMessage::G2C_BRUSH_DAILY_QUEST()); });
    external_message_g2c_brush_daily_quest_type["bClearCDTime"] = sol::property([](const ExternalMessage::G2C_BRUSH_DAILY_QUEST& self) { return static_cast<bool>(self.bClearCDTime); }, [](ExternalMessage::G2C_BRUSH_DAILY_QUEST& self, bool value) { self.bClearCDTime = value; });
    external_message_g2c_brush_daily_quest_type["nQuestID"] = cqyh::lua::array_property(&ExternalMessage::G2C_BRUSH_DAILY_QUEST::nQuestID);
    auto external_message_g2c_quest_recycle_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_QUEST_RECYCLE>(L, external_message_namespace, "G2C_QUEST_RECYCLE", "ExternalMessage::G2C_QUEST_RECYCLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_quest_recycle_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_QUEST_RECYCLE(); });
    external_message_g2c_quest_recycle_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_QUEST_RECYCLE>(new ExternalMessage::G2C_QUEST_RECYCLE()); });
    external_message_g2c_quest_recycle_type["nTemplateID"] = sol::property([](const ExternalMessage::G2C_QUEST_RECYCLE& self) { return self.nTemplateID; }, [](ExternalMessage::G2C_QUEST_RECYCLE& self, decltype(ExternalMessage::G2C_QUEST_RECYCLE::nTemplateID) value) { self.nTemplateID = value; });
    external_message_g2c_quest_recycle_type["nIndex"] = sol::property([](const ExternalMessage::G2C_QUEST_RECYCLE& self) { return self.nIndex; }, [](ExternalMessage::G2C_QUEST_RECYCLE& self, decltype(ExternalMessage::G2C_QUEST_RECYCLE::nIndex) value) { self.nIndex = value; });
    auto external_message_g2c_set_quest_failed_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SET_QUEST_FAILED>(L, external_message_namespace, "G2C_SET_QUEST_FAILED", "ExternalMessage::G2C_SET_QUEST_FAILED", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_set_quest_failed_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SET_QUEST_FAILED(); });
    external_message_g2c_set_quest_failed_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SET_QUEST_FAILED>(new ExternalMessage::G2C_SET_QUEST_FAILED()); });
    external_message_g2c_set_quest_failed_type["nQuestID"] = sol::property([](const ExternalMessage::G2C_SET_QUEST_FAILED& self) { return self.nQuestID; }, [](ExternalMessage::G2C_SET_QUEST_FAILED& self, decltype(ExternalMessage::G2C_SET_QUEST_FAILED::nQuestID) value) { self.nQuestID = value; });
    auto external_message_g2c_complete_kill_npc_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST>(L, external_message_namespace, "G2C_COMPLETE_KILL_NPC_QUEST", "ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_complete_kill_npc_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST(); });
    external_message_g2c_complete_kill_npc_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST>(new ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST()); });
    external_message_g2c_complete_kill_npc_quest_type["wQuestID"] = sol::property([](const ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST& self) { return self.wQuestID; }, [](ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST& self, decltype(ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST::wQuestID) value) { self.wQuestID = value; });
    external_message_g2c_complete_kill_npc_quest_type["byIndex"] = sol::property([](const ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST& self) { return self.byIndex; }, [](ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST& self, decltype(ExternalMessage::G2C_COMPLETE_KILL_NPC_QUEST::byIndex) value) { self.byIndex = value; });
    auto external_message_g2c_reset_kill_npc_quest_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_RESET_KILL_NPC_QUEST>(L, external_message_namespace, "G2C_RESET_KILL_NPC_QUEST", "ExternalMessage::G2C_RESET_KILL_NPC_QUEST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_reset_kill_npc_quest_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_RESET_KILL_NPC_QUEST(); });
    external_message_g2c_reset_kill_npc_quest_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_RESET_KILL_NPC_QUEST>(new ExternalMessage::G2C_RESET_KILL_NPC_QUEST()); });
    external_message_g2c_reset_kill_npc_quest_type["wQuestID"] = sol::property([](const ExternalMessage::G2C_RESET_KILL_NPC_QUEST& self) { return self.wQuestID; }, [](ExternalMessage::G2C_RESET_KILL_NPC_QUEST& self, decltype(ExternalMessage::G2C_RESET_KILL_NPC_QUEST::wQuestID) value) { self.wQuestID = value; });
}

}
