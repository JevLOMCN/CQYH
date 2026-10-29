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

void register_external_manage_to_client_chat_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_chat_message_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_CHAT_MESSAGE>(L, external_message_namespace, "M2C_CHAT_MESSAGE", "ExternalMessage::M2C_CHAT_MESSAGE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_chat_message_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_CHAT_MESSAGE(); });
    external_message_m2c_chat_message_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_CHAT_MESSAGE>(new ExternalMessage::M2C_CHAT_MESSAGE()); });
    external_message_m2c_chat_message_type["dwSrcID"] = sol::property([](const ExternalMessage::M2C_CHAT_MESSAGE& self) { return self.dwSrcID; }, [](ExternalMessage::M2C_CHAT_MESSAGE& self, decltype(ExternalMessage::M2C_CHAT_MESSAGE::dwSrcID) value) { self.dwSrcID = value; });
    external_message_m2c_chat_message_type["dwTargetID"] = sol::property([](const ExternalMessage::M2C_CHAT_MESSAGE& self) { return self.dwTargetID; }, [](ExternalMessage::M2C_CHAT_MESSAGE& self, decltype(ExternalMessage::M2C_CHAT_MESSAGE::dwTargetID) value) { self.dwTargetID = value; });
    external_message_m2c_chat_message_type["uLocation"] = sol::property([](const ExternalMessage::M2C_CHAT_MESSAGE& self) { return self.uLocation; }, [](ExternalMessage::M2C_CHAT_MESSAGE& self, decltype(ExternalMessage::M2C_CHAT_MESSAGE::uLocation) value) { self.uLocation = value; });
    external_message_m2c_chat_message_type["bGM"] = sol::property([](const ExternalMessage::M2C_CHAT_MESSAGE& self) { return self.bGM; }, [](ExternalMessage::M2C_CHAT_MESSAGE& self, decltype(ExternalMessage::M2C_CHAT_MESSAGE::bGM) value) { self.bGM = value; });
    external_message_m2c_chat_message_type["uReserved"] = sol::property([](const ExternalMessage::M2C_CHAT_MESSAGE& self) { return self.uReserved; }, [](ExternalMessage::M2C_CHAT_MESSAGE& self, decltype(ExternalMessage::M2C_CHAT_MESSAGE::uReserved) value) { self.uReserved = value; });
    external_message_m2c_chat_message_type["szMessage"] = cqyh::lua::string_property(&ExternalMessage::M2C_CHAT_MESSAGE::szMessage);
}

}
