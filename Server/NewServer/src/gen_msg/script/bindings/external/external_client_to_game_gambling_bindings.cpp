#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_gambling_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_dice_bet_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_DICE_BET>(L, external_message_namespace, "C2G_DICE_BET", "ExternalMessage::C2G_DICE_BET", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_dice_bet_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_DICE_BET(); });
    external_message_c2g_dice_bet_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_DICE_BET>(new ExternalMessage::C2G_DICE_BET()); });
    external_message_c2g_dice_bet_type["bytype"] = sol::property([](const ExternalMessage::C2G_DICE_BET& self) { return self.bytype; }, [](ExternalMessage::C2G_DICE_BET& self, decltype(ExternalMessage::C2G_DICE_BET::bytype) value) { self.bytype = value; });
    external_message_c2g_dice_bet_type["bycount"] = sol::property([](const ExternalMessage::C2G_DICE_BET& self) { return self.bycount; }, [](ExternalMessage::C2G_DICE_BET& self, decltype(ExternalMessage::C2G_DICE_BET::bycount) value) { self.bycount = value; });
    auto external_message_c2g_gamble_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GAMBLE>(L, external_message_namespace, "C2G_GAMBLE", "ExternalMessage::C2G_GAMBLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_gamble_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GAMBLE(); });
    external_message_c2g_gamble_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GAMBLE>(new ExternalMessage::C2G_GAMBLE()); });
    external_message_c2g_gamble_type["byEquipType"] = sol::property([](const ExternalMessage::C2G_GAMBLE& self) { return self.byEquipType; }, [](ExternalMessage::C2G_GAMBLE& self, decltype(ExternalMessage::C2G_GAMBLE::byEquipType) value) { self.byEquipType = value; });
    auto external_message_c2g_jigsaw_answer_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_JIGSAW_ANSWER>(L, external_message_namespace, "C2G_JIGSAW_ANSWER", "ExternalMessage::C2G_JIGSAW_ANSWER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_jigsaw_answer_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_JIGSAW_ANSWER(); });
    external_message_c2g_jigsaw_answer_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_JIGSAW_ANSWER>(new ExternalMessage::C2G_JIGSAW_ANSWER()); });
    external_message_c2g_jigsaw_answer_type["byIndex"] = sol::property([](const ExternalMessage::C2G_JIGSAW_ANSWER& self) { return self.byIndex; }, [](ExternalMessage::C2G_JIGSAW_ANSWER& self, decltype(ExternalMessage::C2G_JIGSAW_ANSWER::byIndex) value) { self.byIndex = value; });
    auto external_message_c2g_spell_book_gamble_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_SPELL_BOOK_GAMBLE>(L, external_message_namespace, "C2G_SPELL_BOOK_GAMBLE", "ExternalMessage::C2G_SPELL_BOOK_GAMBLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_spell_book_gamble_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_SPELL_BOOK_GAMBLE(); });
    external_message_c2g_spell_book_gamble_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_SPELL_BOOK_GAMBLE>(new ExternalMessage::C2G_SPELL_BOOK_GAMBLE()); });
    auto external_message_c2g_potion_gamble_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_POTION_GAMBLE>(L, external_message_namespace, "C2G_POTION_GAMBLE", "ExternalMessage::C2G_POTION_GAMBLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_potion_gamble_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_POTION_GAMBLE(); });
    external_message_c2g_potion_gamble_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_POTION_GAMBLE>(new ExternalMessage::C2G_POTION_GAMBLE()); });
    external_message_c2g_potion_gamble_type["nGambleLevel"] = sol::property([](const ExternalMessage::C2G_POTION_GAMBLE& self) { return self.nGambleLevel; }, [](ExternalMessage::C2G_POTION_GAMBLE& self, decltype(ExternalMessage::C2G_POTION_GAMBLE::nGambleLevel) value) { self.nGambleLevel = value; });
    auto external_message_c2g_gamble_advance_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GAMBLE_ADVANCE>(L, external_message_namespace, "C2G_GAMBLE_ADVANCE", "ExternalMessage::C2G_GAMBLE_ADVANCE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_gamble_advance_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GAMBLE_ADVANCE(); });
    external_message_c2g_gamble_advance_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GAMBLE_ADVANCE>(new ExternalMessage::C2G_GAMBLE_ADVANCE()); });
    external_message_c2g_gamble_advance_type["nGambleType"] = sol::property([](const ExternalMessage::C2G_GAMBLE_ADVANCE& self) { return self.nGambleType; }, [](ExternalMessage::C2G_GAMBLE_ADVANCE& self, decltype(ExternalMessage::C2G_GAMBLE_ADVANCE::nGambleType) value) { self.nGambleType = value; });
    external_message_c2g_gamble_advance_type["nGambleLevel"] = sol::property([](const ExternalMessage::C2G_GAMBLE_ADVANCE& self) { return self.nGambleLevel; }, [](ExternalMessage::C2G_GAMBLE_ADVANCE& self, decltype(ExternalMessage::C2G_GAMBLE_ADVANCE::nGambleLevel) value) { self.nGambleLevel = value; });
}

}
