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

void register_external_game_to_client_chat_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_chat_message_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAT_MESSAGE>(L, external_message_namespace, "G2C_CHAT_MESSAGE", "ExternalMessage::G2C_CHAT_MESSAGE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_chat_message_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAT_MESSAGE(); });
    external_message_g2c_chat_message_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAT_MESSAGE>(new ExternalMessage::G2C_CHAT_MESSAGE()); });
    external_message_g2c_chat_message_type["dwTargetID"] = sol::property([](const ExternalMessage::G2C_CHAT_MESSAGE& self) { return self.dwTargetID; }, [](ExternalMessage::G2C_CHAT_MESSAGE& self, decltype(ExternalMessage::G2C_CHAT_MESSAGE::dwTargetID) value) { self.dwTargetID = value; });
    external_message_g2c_chat_message_type["dwSrcID"] = sol::property([](const ExternalMessage::G2C_CHAT_MESSAGE& self) { return self.dwSrcID; }, [](ExternalMessage::G2C_CHAT_MESSAGE& self, decltype(ExternalMessage::G2C_CHAT_MESSAGE::dwSrcID) value) { self.dwSrcID = value; });
    external_message_g2c_chat_message_type["uLocation"] = sol::property([](const ExternalMessage::G2C_CHAT_MESSAGE& self) { return self.uLocation; }, [](ExternalMessage::G2C_CHAT_MESSAGE& self, decltype(ExternalMessage::G2C_CHAT_MESSAGE::uLocation) value) { self.uLocation = value; });
    external_message_g2c_chat_message_type["uReserved"] = sol::property([](const ExternalMessage::G2C_CHAT_MESSAGE& self) { return self.uReserved; }, [](ExternalMessage::G2C_CHAT_MESSAGE& self, decltype(ExternalMessage::G2C_CHAT_MESSAGE::uReserved) value) { self.uReserved = value; });
    external_message_g2c_chat_message_type["szMessage"] = cqyh::lua::string_property(&ExternalMessage::G2C_CHAT_MESSAGE::szMessage);
}

}
