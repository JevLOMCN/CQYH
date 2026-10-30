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

void register_external_game_to_client_reputation_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_reputation_info_type = cqyh::gen_msg::usertype<ExternalMessage::REPUTATION_INFO>(L, external_message_namespace, "REPUTATION_INFO", "ExternalMessage::REPUTATION_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_reputation_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::REPUTATION_INFO(); });
    external_message_reputation_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::REPUTATION_INFO>(new ExternalMessage::REPUTATION_INFO()); });
    external_message_reputation_info_type["byForceID"] = sol::property([](const ExternalMessage::REPUTATION_INFO& self) { return self.byForceID; }, [](ExternalMessage::REPUTATION_INFO& self, decltype(ExternalMessage::REPUTATION_INFO::byForceID) value) { self.byForceID = value; });
    external_message_reputation_info_type["shReputation"] = sol::property([](const ExternalMessage::REPUTATION_INFO& self) { return self.shReputation; }, [](ExternalMessage::REPUTATION_INFO& self, decltype(ExternalMessage::REPUTATION_INFO::shReputation) value) { self.shReputation = value; });
    auto external_message_g2c_sync_reputation_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_REPUTATION>(L, external_message_namespace, "G2C_SYNC_REPUTATION", "ExternalMessage::G2C_SYNC_REPUTATION", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_reputation_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_REPUTATION(); });
    external_message_g2c_sync_reputation_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_REPUTATION>(new ExternalMessage::G2C_SYNC_REPUTATION()); });
    external_message_g2c_sync_reputation_type["qwActiveState"] = sol::property([](const ExternalMessage::G2C_SYNC_REPUTATION& self) { return self.qwActiveState; }, [](ExternalMessage::G2C_SYNC_REPUTATION& self, decltype(ExternalMessage::G2C_SYNC_REPUTATION::qwActiveState) value) { self.qwActiveState = value; });
    external_message_g2c_sync_reputation_type["qwAwardState"] = sol::property([](const ExternalMessage::G2C_SYNC_REPUTATION& self) { return self.qwAwardState; }, [](ExternalMessage::G2C_SYNC_REPUTATION& self, decltype(ExternalMessage::G2C_SYNC_REPUTATION::qwAwardState) value) { self.qwAwardState = value; });
    external_message_g2c_sync_reputation_type["ReputationTable"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_REPUTATION::ReputationTable);
    auto external_message_g2c_modify_reputation_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_MODIFY_REPUTATION>(L, external_message_namespace, "G2C_MODIFY_REPUTATION", "ExternalMessage::G2C_MODIFY_REPUTATION", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_modify_reputation_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_MODIFY_REPUTATION(); });
    external_message_g2c_modify_reputation_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_MODIFY_REPUTATION>(new ExternalMessage::G2C_MODIFY_REPUTATION()); });
    external_message_g2c_modify_reputation_type["byForceID"] = sol::property([](const ExternalMessage::G2C_MODIFY_REPUTATION& self) { return self.byForceID; }, [](ExternalMessage::G2C_MODIFY_REPUTATION& self, decltype(ExternalMessage::G2C_MODIFY_REPUTATION::byForceID) value) { self.byForceID = value; });
    external_message_g2c_modify_reputation_type["nReputation"] = sol::property([](const ExternalMessage::G2C_MODIFY_REPUTATION& self) { return self.nReputation; }, [](ExternalMessage::G2C_MODIFY_REPUTATION& self, decltype(ExternalMessage::G2C_MODIFY_REPUTATION::nReputation) value) { self.nReputation = value; });
}

}
