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

void register_external_game_to_client_gambling_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_gamble_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GAMBLE>(L, external_message_namespace, "G2C_GAMBLE", "ExternalMessage::G2C_GAMBLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_gamble_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GAMBLE(); });
    external_message_g2c_gamble_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GAMBLE>(new ExternalMessage::G2C_GAMBLE()); });
    external_message_g2c_gamble_type["nResult"] = sol::property([](const ExternalMessage::G2C_GAMBLE& self) { return self.nResult; }, [](ExternalMessage::G2C_GAMBLE& self, decltype(ExternalMessage::G2C_GAMBLE::nResult) value) { self.nResult = value; });
    external_message_g2c_gamble_type["nItemID"] = sol::property([](const ExternalMessage::G2C_GAMBLE& self) { return self.nItemID; }, [](ExternalMessage::G2C_GAMBLE& self, decltype(ExternalMessage::G2C_GAMBLE::nItemID) value) { self.nItemID = value; });
    auto external_message_jigsaw_answer_data_type = cqyh::gen_msg::usertype<ExternalMessage::JIGSAW_ANSWER_DATA>(L, external_message_namespace, "JIGSAW_ANSWER_DATA", "ExternalMessage::JIGSAW_ANSWER_DATA");
    external_message_jigsaw_answer_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::JIGSAW_ANSWER_DATA(); });
    external_message_jigsaw_answer_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::JIGSAW_ANSWER_DATA>(new ExternalMessage::JIGSAW_ANSWER_DATA()); });
    external_message_jigsaw_answer_data_type["cAnswerData"] = cqyh::lua::string_property(&ExternalMessage::JIGSAW_ANSWER_DATA::cAnswerData);
    auto external_message_g2c_jigsaw_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_JIGSAW>(L, external_message_namespace, "G2C_JIGSAW", "ExternalMessage::G2C_JIGSAW", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_jigsaw_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_JIGSAW(); });
    external_message_g2c_jigsaw_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_JIGSAW>(new ExternalMessage::G2C_JIGSAW()); });
    external_message_g2c_jigsaw_type["dwTimeoutInterval"] = sol::property([](const ExternalMessage::G2C_JIGSAW& self) { return self.dwTimeoutInterval; }, [](ExternalMessage::G2C_JIGSAW& self, decltype(ExternalMessage::G2C_JIGSAW::dwTimeoutInterval) value) { self.dwTimeoutInterval = value; });
    external_message_g2c_jigsaw_type["byBlockX"] = sol::property([](const ExternalMessage::G2C_JIGSAW& self) { return self.byBlockX; }, [](ExternalMessage::G2C_JIGSAW& self, decltype(ExternalMessage::G2C_JIGSAW::byBlockX) value) { self.byBlockX = value; });
    external_message_g2c_jigsaw_type["byBlockY"] = sol::property([](const ExternalMessage::G2C_JIGSAW& self) { return self.byBlockY; }, [](ExternalMessage::G2C_JIGSAW& self, decltype(ExternalMessage::G2C_JIGSAW::byBlockY) value) { self.byBlockY = value; });
    external_message_g2c_jigsaw_type["cImageData"] = cqyh::lua::string_property(&ExternalMessage::G2C_JIGSAW::cImageData);
    external_message_g2c_jigsaw_type["AnswerData"] = cqyh::lua::array_property(&ExternalMessage::G2C_JIGSAW::AnswerData);
    auto external_message_g2c_spell_book_gamble_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SPELL_BOOK_GAMBLE>(L, external_message_namespace, "G2C_SPELL_BOOK_GAMBLE", "ExternalMessage::G2C_SPELL_BOOK_GAMBLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_spell_book_gamble_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SPELL_BOOK_GAMBLE(); });
    external_message_g2c_spell_book_gamble_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SPELL_BOOK_GAMBLE>(new ExternalMessage::G2C_SPELL_BOOK_GAMBLE()); });
    external_message_g2c_spell_book_gamble_type["nResult"] = sol::property([](const ExternalMessage::G2C_SPELL_BOOK_GAMBLE& self) { return self.nResult; }, [](ExternalMessage::G2C_SPELL_BOOK_GAMBLE& self, decltype(ExternalMessage::G2C_SPELL_BOOK_GAMBLE::nResult) value) { self.nResult = value; });
    external_message_g2c_spell_book_gamble_type["nItemID"] = sol::property([](const ExternalMessage::G2C_SPELL_BOOK_GAMBLE& self) { return self.nItemID; }, [](ExternalMessage::G2C_SPELL_BOOK_GAMBLE& self, decltype(ExternalMessage::G2C_SPELL_BOOK_GAMBLE::nItemID) value) { self.nItemID = value; });
    auto external_message_g2c_potion_gamble_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_POTION_GAMBLE>(L, external_message_namespace, "G2C_POTION_GAMBLE", "ExternalMessage::G2C_POTION_GAMBLE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_potion_gamble_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_POTION_GAMBLE(); });
    external_message_g2c_potion_gamble_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_POTION_GAMBLE>(new ExternalMessage::G2C_POTION_GAMBLE()); });
    external_message_g2c_potion_gamble_type["nResult"] = sol::property([](const ExternalMessage::G2C_POTION_GAMBLE& self) { return self.nResult; }, [](ExternalMessage::G2C_POTION_GAMBLE& self, decltype(ExternalMessage::G2C_POTION_GAMBLE::nResult) value) { self.nResult = value; });
    external_message_g2c_potion_gamble_type["nItemID"] = sol::property([](const ExternalMessage::G2C_POTION_GAMBLE& self) { return self.nItemID; }, [](ExternalMessage::G2C_POTION_GAMBLE& self, decltype(ExternalMessage::G2C_POTION_GAMBLE::nItemID) value) { self.nItemID = value; });
}

}
