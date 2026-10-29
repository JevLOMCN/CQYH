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

void register_external_game_to_client_guild_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_sync_guild_achievement_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_GUILD_ACHIEVEMENT>(L, external_message_namespace, "G2C_SYNC_GUILD_ACHIEVEMENT", "ExternalMessage::G2C_SYNC_GUILD_ACHIEVEMENT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_guild_achievement_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_GUILD_ACHIEVEMENT(); });
    external_message_g2c_sync_guild_achievement_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_GUILD_ACHIEVEMENT>(new ExternalMessage::G2C_SYNC_GUILD_ACHIEVEMENT()); });
    external_message_g2c_sync_guild_achievement_type["acAchievement"] = cqyh::lua::string_property(&ExternalMessage::G2C_SYNC_GUILD_ACHIEVEMENT::acAchievement);
    auto external_message_g2c_sync_guild_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_GUILD>(L, external_message_namespace, "G2C_SYNC_GUILD", "ExternalMessage::G2C_SYNC_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_GUILD(); });
    external_message_g2c_sync_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_GUILD>(new ExternalMessage::G2C_SYNC_GUILD()); });
    external_message_g2c_sync_guild_type["dwCD"] = sol::property([](const ExternalMessage::G2C_SYNC_GUILD& self) { return self.dwCD; }, [](ExternalMessage::G2C_SYNC_GUILD& self, decltype(ExternalMessage::G2C_SYNC_GUILD::dwCD) value) { self.dwCD = value; });
    auto external_message_g2c_sync_guild_name_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_GUILD_NAME>(L, external_message_namespace, "G2C_SYNC_GUILD_NAME", "ExternalMessage::G2C_SYNC_GUILD_NAME", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_guild_name_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_GUILD_NAME(); });
    external_message_g2c_sync_guild_name_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_GUILD_NAME>(new ExternalMessage::G2C_SYNC_GUILD_NAME()); });
    external_message_g2c_sync_guild_name_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_GUILD_NAME& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_GUILD_NAME& self, decltype(ExternalMessage::G2C_SYNC_GUILD_NAME::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_guild_name_type["dwGuildID"] = sol::property([](const ExternalMessage::G2C_SYNC_GUILD_NAME& self) { return self.dwGuildID; }, [](ExternalMessage::G2C_SYNC_GUILD_NAME& self, decltype(ExternalMessage::G2C_SYNC_GUILD_NAME::dwGuildID) value) { self.dwGuildID = value; });
    auto external_message_g2c_guild_buy_aura_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GUILD_BUY_AURA_ACK>(L, external_message_namespace, "G2C_GUILD_BUY_AURA_ACK", "ExternalMessage::G2C_GUILD_BUY_AURA_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_guild_buy_aura_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GUILD_BUY_AURA_ACK(); });
    external_message_g2c_guild_buy_aura_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GUILD_BUY_AURA_ACK>(new ExternalMessage::G2C_GUILD_BUY_AURA_ACK()); });
    external_message_g2c_guild_buy_aura_ack_type["byType"] = sol::property([](const ExternalMessage::G2C_GUILD_BUY_AURA_ACK& self) { return self.byType; }, [](ExternalMessage::G2C_GUILD_BUY_AURA_ACK& self, decltype(ExternalMessage::G2C_GUILD_BUY_AURA_ACK::byType) value) { self.byType = value; });
    external_message_g2c_guild_buy_aura_ack_type["byCount"] = sol::property([](const ExternalMessage::G2C_GUILD_BUY_AURA_ACK& self) { return self.byCount; }, [](ExternalMessage::G2C_GUILD_BUY_AURA_ACK& self, decltype(ExternalMessage::G2C_GUILD_BUY_AURA_ACK::byCount) value) { self.byCount = value; });
    auto external_message_guild_buy_aura_record_info_type = cqyh::gen_msg::usertype<ExternalMessage::GUILD_BUY_AURA_RECORD_INFO>(L, external_message_namespace, "GUILD_BUY_AURA_RECORD_INFO", "ExternalMessage::GUILD_BUY_AURA_RECORD_INFO");
    external_message_guild_buy_aura_record_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::GUILD_BUY_AURA_RECORD_INFO(); });
    external_message_guild_buy_aura_record_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::GUILD_BUY_AURA_RECORD_INFO>(new ExternalMessage::GUILD_BUY_AURA_RECORD_INFO()); });
    external_message_guild_buy_aura_record_info_type["nType"] = sol::property([](const ExternalMessage::GUILD_BUY_AURA_RECORD_INFO& self) { return self.nType; }, [](ExternalMessage::GUILD_BUY_AURA_RECORD_INFO& self, decltype(ExternalMessage::GUILD_BUY_AURA_RECORD_INFO::nType) value) { self.nType = value; });
    external_message_guild_buy_aura_record_info_type["byCount"] = sol::property([](const ExternalMessage::GUILD_BUY_AURA_RECORD_INFO& self) { return self.byCount; }, [](ExternalMessage::GUILD_BUY_AURA_RECORD_INFO& self, decltype(ExternalMessage::GUILD_BUY_AURA_RECORD_INFO::byCount) value) { self.byCount = value; });
    auto external_message_g2c_guild_contribute_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK>(L, external_message_namespace, "G2C_GUILD_CONTRIBUTE_ACK", "ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_guild_contribute_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK(); });
    external_message_g2c_guild_contribute_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK>(new ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK()); });
    external_message_g2c_guild_contribute_ack_type["nMoney"] = sol::property([](const ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK& self) { return self.nMoney; }, [](ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK& self, decltype(ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK::nMoney) value) { self.nMoney = value; });
    external_message_g2c_guild_contribute_ack_type["nContribution"] = sol::property([](const ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK& self) { return self.nContribution; }, [](ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK& self, decltype(ExternalMessage::G2C_GUILD_CONTRIBUTE_ACK::nContribution) value) { self.nContribution = value; });
}

}
