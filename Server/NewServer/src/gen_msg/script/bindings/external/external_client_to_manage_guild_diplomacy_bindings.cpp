#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2m_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_manage_guild_diplomacy_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2m_guild_diplomat_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_DIPLOMAT_APP>(L, external_message_namespace, "C2M_GUILD_DIPLOMAT_APP", "ExternalMessage::C2M_GUILD_DIPLOMAT_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_diplomat_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_DIPLOMAT_APP(); });
    external_message_c2m_guild_diplomat_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_DIPLOMAT_APP>(new ExternalMessage::C2M_GUILD_DIPLOMAT_APP()); });
    external_message_c2m_guild_diplomat_app_type["byOpType"] = sol::property([](const ExternalMessage::C2M_GUILD_DIPLOMAT_APP& self) { return self.byOpType; }, [](ExternalMessage::C2M_GUILD_DIPLOMAT_APP& self, decltype(ExternalMessage::C2M_GUILD_DIPLOMAT_APP::byOpType) value) { self.byOpType = value; });
    external_message_c2m_guild_diplomat_app_type["byTimeType"] = sol::property([](const ExternalMessage::C2M_GUILD_DIPLOMAT_APP& self) { return self.byTimeType; }, [](ExternalMessage::C2M_GUILD_DIPLOMAT_APP& self, decltype(ExternalMessage::C2M_GUILD_DIPLOMAT_APP::byTimeType) value) { self.byTimeType = value; });
    external_message_c2m_guild_diplomat_app_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::C2M_GUILD_DIPLOMAT_APP::szGuildName);
    auto external_message_c2m_guild_get_apply_union_list_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_GET_APPLY_UNION_LIST>(L, external_message_namespace, "C2M_GUILD_GET_APPLY_UNION_LIST", "ExternalMessage::C2M_GUILD_GET_APPLY_UNION_LIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_get_apply_union_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_GET_APPLY_UNION_LIST(); });
    external_message_c2m_guild_get_apply_union_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_GET_APPLY_UNION_LIST>(new ExternalMessage::C2M_GUILD_GET_APPLY_UNION_LIST()); });
    auto external_message_c2m_guild_reply_union_app_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_REPLY_UNION_APP>(L, external_message_namespace, "C2M_GUILD_REPLY_UNION_APP", "ExternalMessage::C2M_GUILD_REPLY_UNION_APP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_reply_union_app_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_REPLY_UNION_APP(); });
    external_message_c2m_guild_reply_union_app_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_REPLY_UNION_APP>(new ExternalMessage::C2M_GUILD_REPLY_UNION_APP()); });
    external_message_c2m_guild_reply_union_app_type["byOp"] = sol::property([](const ExternalMessage::C2M_GUILD_REPLY_UNION_APP& self) { return self.byOp; }, [](ExternalMessage::C2M_GUILD_REPLY_UNION_APP& self, decltype(ExternalMessage::C2M_GUILD_REPLY_UNION_APP::byOp) value) { self.byOp = value; });
    external_message_c2m_guild_reply_union_app_type["dwGuildID"] = sol::property([](const ExternalMessage::C2M_GUILD_REPLY_UNION_APP& self) { return self.dwGuildID; }, [](ExternalMessage::C2M_GUILD_REPLY_UNION_APP& self, decltype(ExternalMessage::C2M_GUILD_REPLY_UNION_APP::dwGuildID) value) { self.dwGuildID = value; });
    auto external_message_c2m_guild_cancel_alliance_type = cqyh::gen_msg::usertype<ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE>(L, external_message_namespace, "C2M_GUILD_CANCEL_ALLIANCE", "ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2m_guild_cancel_alliance_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE(); });
    external_message_c2m_guild_cancel_alliance_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE>(new ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE()); });
    external_message_c2m_guild_cancel_alliance_type["dwGuildID"] = sol::property([](const ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE& self) { return self.dwGuildID; }, [](ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE& self, decltype(ExternalMessage::C2M_GUILD_CANCEL_ALLIANCE::dwGuildID) value) { self.dwGuildID = value; });
}

}
