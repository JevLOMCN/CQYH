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

void register_external_game_to_client_guild_members_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_g2c_guild_draw_salary_ack_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK>(L, external_message_namespace, "G2C_GUILD_DRAW_SALARY_ACK", "ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_guild_draw_salary_ack_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK(); });
    external_message_g2c_guild_draw_salary_ack_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK>(new ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK()); });
    external_message_g2c_guild_draw_salary_ack_type["nSalary"] = sol::property([](const ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK& self) { return self.nSalary; }, [](ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK& self, decltype(ExternalMessage::G2C_GUILD_DRAW_SALARY_ACK::nSalary) value) { self.nSalary = value; });
}

}
