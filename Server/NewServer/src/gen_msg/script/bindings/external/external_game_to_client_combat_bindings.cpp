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

void register_external_game_to_client_combat_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_play_action_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_PLAY_ACTION>(L, external_message_namespace, "G2C_PLAY_ACTION", "ExternalMessage::G2C_PLAY_ACTION", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_play_action_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_PLAY_ACTION(); });
    external_message_g2c_play_action_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_PLAY_ACTION>(new ExternalMessage::G2C_PLAY_ACTION()); });
    external_message_g2c_play_action_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_PLAY_ACTION& self) { return self.dwCharID; }, [](ExternalMessage::G2C_PLAY_ACTION& self, decltype(ExternalMessage::G2C_PLAY_ACTION::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_play_action_type["byActionID"] = sol::property([](const ExternalMessage::G2C_PLAY_ACTION& self) { return self.byActionID; }, [](ExternalMessage::G2C_PLAY_ACTION& self, decltype(ExternalMessage::G2C_PLAY_ACTION::byActionID) value) { self.byActionID = value; });
    auto external_message_target_data_type = cqyh::gen_msg::usertype<ExternalMessage::TARGET_DATA>(L, external_message_namespace, "TARGET_DATA", "ExternalMessage::TARGET_DATA");
    external_message_target_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::TARGET_DATA(); });
    external_message_target_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::TARGET_DATA>(new ExternalMessage::TARGET_DATA()); });
    external_message_target_data_type["dwTargetID"] = sol::property([](const ExternalMessage::TARGET_DATA& self) { return self.dwTargetID; }, [](ExternalMessage::TARGET_DATA& self, decltype(ExternalMessage::TARGET_DATA::dwTargetID) value) { self.dwTargetID = value; });
    external_message_target_data_type["wFlag"] = sol::property([](const ExternalMessage::TARGET_DATA& self) { return self.wFlag; }, [](ExternalMessage::TARGET_DATA& self, decltype(ExternalMessage::TARGET_DATA::wFlag) value) { self.wFlag = value; });
    external_message_target_data_type["wEffectDelayTime"] = sol::property([](const ExternalMessage::TARGET_DATA& self) { return self.wEffectDelayTime; }, [](ExternalMessage::TARGET_DATA& self, decltype(ExternalMessage::TARGET_DATA::wEffectDelayTime) value) { self.wEffectDelayTime = value; });
    auto external_message_g2c_directly_hp_change_effect_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT>(L, external_message_namespace, "G2C_DIRECTLY_HP_CHANGE_EFFECT", "ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_directly_hp_change_effect_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT(); });
    external_message_g2c_directly_hp_change_effect_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT>(new ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT()); });
    external_message_g2c_directly_hp_change_effect_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT& self) { return self.dwCharID; }, [](ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT& self, decltype(ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_directly_hp_change_effect_type["nHpChange"] = sol::property([](const ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT& self) { return self.nHpChange; }, [](ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT& self, decltype(ExternalMessage::G2C_DIRECTLY_HP_CHANGE_EFFECT::nHpChange) value) { self.nHpChange = value; });
    auto external_message_g2c_combat_pose_switch_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_COMBAT_POSE_SWITCH>(L, external_message_namespace, "G2C_COMBAT_POSE_SWITCH", "ExternalMessage::G2C_COMBAT_POSE_SWITCH", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_combat_pose_switch_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_COMBAT_POSE_SWITCH(); });
    external_message_g2c_combat_pose_switch_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_COMBAT_POSE_SWITCH>(new ExternalMessage::G2C_COMBAT_POSE_SWITCH()); });
    external_message_g2c_combat_pose_switch_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_COMBAT_POSE_SWITCH& self) { return self.dwCharID; }, [](ExternalMessage::G2C_COMBAT_POSE_SWITCH& self, decltype(ExternalMessage::G2C_COMBAT_POSE_SWITCH::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_combat_pose_switch_type["byCombatPoseOn"] = sol::property([](const ExternalMessage::G2C_COMBAT_POSE_SWITCH& self) { return self.byCombatPoseOn; }, [](ExternalMessage::G2C_COMBAT_POSE_SWITCH& self, decltype(ExternalMessage::G2C_COMBAT_POSE_SWITCH::byCombatPoseOn) value) { self.byCombatPoseOn = value; });
    external_message_g2c_combat_pose_switch_type["byClientFlag"] = sol::property([](const ExternalMessage::G2C_COMBAT_POSE_SWITCH& self) { return self.byClientFlag; }, [](ExternalMessage::G2C_COMBAT_POSE_SWITCH& self, decltype(ExternalMessage::G2C_COMBAT_POSE_SWITCH::byClientFlag) value) { self.byClientFlag = value; });
    auto external_message_g2c_char_select_focus_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_SELECT_FOCUS>(L, external_message_namespace, "G2C_CHAR_SELECT_FOCUS", "ExternalMessage::G2C_CHAR_SELECT_FOCUS", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_select_focus_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_SELECT_FOCUS(); });
    external_message_g2c_char_select_focus_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_SELECT_FOCUS>(new ExternalMessage::G2C_CHAR_SELECT_FOCUS()); });
    external_message_g2c_char_select_focus_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_SELECT_FOCUS& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_SELECT_FOCUS& self, decltype(ExternalMessage::G2C_CHAR_SELECT_FOCUS::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_select_focus_type["dwFocusID"] = sol::property([](const ExternalMessage::G2C_CHAR_SELECT_FOCUS& self) { return self.dwFocusID; }, [](ExternalMessage::G2C_CHAR_SELECT_FOCUS& self, decltype(ExternalMessage::G2C_CHAR_SELECT_FOCUS::dwFocusID) value) { self.dwFocusID = value; });
    auto external_message_g2c_focus_info_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_FOCUS_INFO>(L, external_message_namespace, "G2C_FOCUS_INFO", "ExternalMessage::G2C_FOCUS_INFO", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_focus_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_FOCUS_INFO(); });
    external_message_g2c_focus_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_FOCUS_INFO>(new ExternalMessage::G2C_FOCUS_INFO()); });
    external_message_g2c_focus_info_type["dwFocusID"] = sol::property([](const ExternalMessage::G2C_FOCUS_INFO& self) { return self.dwFocusID; }, [](ExternalMessage::G2C_FOCUS_INFO& self, decltype(ExternalMessage::G2C_FOCUS_INFO::dwFocusID) value) { self.dwFocusID = value; });
    external_message_g2c_focus_info_type["nCurrentHp"] = sol::property([](const ExternalMessage::G2C_FOCUS_INFO& self) { return self.nCurrentHp; }, [](ExternalMessage::G2C_FOCUS_INFO& self, decltype(ExternalMessage::G2C_FOCUS_INFO::nCurrentHp) value) { self.nCurrentHp = value; });
    external_message_g2c_focus_info_type["nCurrentMp"] = sol::property([](const ExternalMessage::G2C_FOCUS_INFO& self) { return self.nCurrentMp; }, [](ExternalMessage::G2C_FOCUS_INFO& self, decltype(ExternalMessage::G2C_FOCUS_INFO::nCurrentMp) value) { self.nCurrentMp = value; });
    external_message_g2c_focus_info_type["nMaxHp"] = sol::property([](const ExternalMessage::G2C_FOCUS_INFO& self) { return self.nMaxHp; }, [](ExternalMessage::G2C_FOCUS_INFO& self, decltype(ExternalMessage::G2C_FOCUS_INFO::nMaxHp) value) { self.nMaxHp = value; });
    external_message_g2c_focus_info_type["nMaxMp"] = sol::property([](const ExternalMessage::G2C_FOCUS_INFO& self) { return self.nMaxMp; }, [](ExternalMessage::G2C_FOCUS_INFO& self, decltype(ExternalMessage::G2C_FOCUS_INFO::nMaxMp) value) { self.nMaxMp = value; });
    external_message_g2c_focus_info_type["byAuraCount"] = sol::property([](const ExternalMessage::G2C_FOCUS_INFO& self) { return self.byAuraCount; }, [](ExternalMessage::G2C_FOCUS_INFO& self, decltype(ExternalMessage::G2C_FOCUS_INFO::byAuraCount) value) { self.byAuraCount = value; });
    external_message_g2c_focus_info_type["astAuraData"] = cqyh::lua::array_property(&ExternalMessage::G2C_FOCUS_INFO::astAuraData);
    auto external_message_g2c_enter_combat_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ENTER_COMBAT>(L, external_message_namespace, "G2C_ENTER_COMBAT", "ExternalMessage::G2C_ENTER_COMBAT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_enter_combat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ENTER_COMBAT(); });
    external_message_g2c_enter_combat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ENTER_COMBAT>(new ExternalMessage::G2C_ENTER_COMBAT()); });
    external_message_g2c_enter_combat_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_ENTER_COMBAT& self) { return self.dwCharID; }, [](ExternalMessage::G2C_ENTER_COMBAT& self, decltype(ExternalMessage::G2C_ENTER_COMBAT::dwCharID) value) { self.dwCharID = value; });
    auto external_message_g2c_leave_combat_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_LEAVE_COMBAT>(L, external_message_namespace, "G2C_LEAVE_COMBAT", "ExternalMessage::G2C_LEAVE_COMBAT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_leave_combat_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_LEAVE_COMBAT(); });
    external_message_g2c_leave_combat_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_LEAVE_COMBAT>(new ExternalMessage::G2C_LEAVE_COMBAT()); });
    external_message_g2c_leave_combat_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_LEAVE_COMBAT& self) { return self.dwCharID; }, [](ExternalMessage::G2C_LEAVE_COMBAT& self, decltype(ExternalMessage::G2C_LEAVE_COMBAT::dwCharID) value) { self.dwCharID = value; });
    auto external_message_g2c_addto_threatlist_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ADDTO_THREATLIST>(L, external_message_namespace, "G2C_ADDTO_THREATLIST", "ExternalMessage::G2C_ADDTO_THREATLIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_addto_threatlist_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ADDTO_THREATLIST(); });
    external_message_g2c_addto_threatlist_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ADDTO_THREATLIST>(new ExternalMessage::G2C_ADDTO_THREATLIST()); });
    external_message_g2c_addto_threatlist_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_ADDTO_THREATLIST& self) { return self.dwCharID; }, [](ExternalMessage::G2C_ADDTO_THREATLIST& self, decltype(ExternalMessage::G2C_ADDTO_THREATLIST::dwCharID) value) { self.dwCharID = value; });
    auto external_message_g2c_delfrom_threatlist_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_DELFROM_THREATLIST>(L, external_message_namespace, "G2C_DELFROM_THREATLIST", "ExternalMessage::G2C_DELFROM_THREATLIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_delfrom_threatlist_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_DELFROM_THREATLIST(); });
    external_message_g2c_delfrom_threatlist_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_DELFROM_THREATLIST>(new ExternalMessage::G2C_DELFROM_THREATLIST()); });
    external_message_g2c_delfrom_threatlist_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_DELFROM_THREATLIST& self) { return self.dwCharID; }, [](ExternalMessage::G2C_DELFROM_THREATLIST& self, decltype(ExternalMessage::G2C_DELFROM_THREATLIST::dwCharID) value) { self.dwCharID = value; });
    auto external_message_g2c_missile_appear_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_MISSILE_APPEAR>(L, external_message_namespace, "G2C_MISSILE_APPEAR", "ExternalMessage::G2C_MISSILE_APPEAR", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_missile_appear_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_MISSILE_APPEAR(); });
    external_message_g2c_missile_appear_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_MISSILE_APPEAR>(new ExternalMessage::G2C_MISSILE_APPEAR()); });
    external_message_g2c_missile_appear_type["stAppearData"] = &ExternalMessage::G2C_MISSILE_APPEAR::stAppearData;
    auto external_message_g2c_missile_move_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_MISSILE_MOVE>(L, external_message_namespace, "G2C_MISSILE_MOVE", "ExternalMessage::G2C_MISSILE_MOVE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_missile_move_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_MISSILE_MOVE(); });
    external_message_g2c_missile_move_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_MISSILE_MOVE>(new ExternalMessage::G2C_MISSILE_MOVE()); });
    external_message_g2c_missile_move_type["dwMissileID"] = sol::property([](const ExternalMessage::G2C_MISSILE_MOVE& self) { return self.dwMissileID; }, [](ExternalMessage::G2C_MISSILE_MOVE& self, decltype(ExternalMessage::G2C_MISSILE_MOVE::dwMissileID) value) { self.dwMissileID = value; });
    external_message_g2c_missile_move_type["wRemainFrm"] = sol::property([](const ExternalMessage::G2C_MISSILE_MOVE& self) { return self.wRemainFrm; }, [](ExternalMessage::G2C_MISSILE_MOVE& self, decltype(ExternalMessage::G2C_MISSILE_MOVE::wRemainFrm) value) { self.wRemainFrm = value; });
    external_message_g2c_missile_move_type["nFaceDirection"] = sol::property([](const ExternalMessage::G2C_MISSILE_MOVE& self) { return self.nFaceDirection; }, [](ExternalMessage::G2C_MISSILE_MOVE& self, decltype(ExternalMessage::G2C_MISSILE_MOVE::nFaceDirection) value) { self.nFaceDirection = value; });
    external_message_g2c_missile_move_type["wDestX"] = sol::property([](const ExternalMessage::G2C_MISSILE_MOVE& self) { return self.wDestX; }, [](ExternalMessage::G2C_MISSILE_MOVE& self, decltype(ExternalMessage::G2C_MISSILE_MOVE::wDestX) value) { self.wDestX = value; });
    external_message_g2c_missile_move_type["wDestY"] = sol::property([](const ExternalMessage::G2C_MISSILE_MOVE& self) { return self.wDestY; }, [](ExternalMessage::G2C_MISSILE_MOVE& self, decltype(ExternalMessage::G2C_MISSILE_MOVE::wDestY) value) { self.wDestY = value; });
    external_message_g2c_missile_move_type["wDestZ"] = sol::property([](const ExternalMessage::G2C_MISSILE_MOVE& self) { return self.wDestZ; }, [](ExternalMessage::G2C_MISSILE_MOVE& self, decltype(ExternalMessage::G2C_MISSILE_MOVE::wDestZ) value) { self.wDestZ = value; });
    auto external_message_g2c_sync_missile_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MISSILE_LIST>(L, external_message_namespace, "G2C_SYNC_MISSILE_LIST", "ExternalMessage::G2C_SYNC_MISSILE_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_missile_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_MISSILE_LIST(); });
    external_message_g2c_sync_missile_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_MISSILE_LIST>(new ExternalMessage::G2C_SYNC_MISSILE_LIST()); });
    external_message_g2c_sync_missile_list_type["astAppearData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MISSILE_LIST::astAppearData);
}

}
