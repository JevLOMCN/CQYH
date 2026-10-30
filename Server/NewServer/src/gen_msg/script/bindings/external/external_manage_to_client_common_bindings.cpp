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

void register_external_manage_to_client_common_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_m2c_self_info_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_SELF_INFO>(L, external_message_namespace, "M2C_SELF_INFO", "ExternalMessage::M2C_SELF_INFO", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_self_info_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_SELF_INFO(); });
    external_message_m2c_self_info_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_SELF_INFO>(new ExternalMessage::M2C_SELF_INFO()); });
    external_message_m2c_self_info_type["szSignature"] = cqyh::lua::string_property(&ExternalMessage::M2C_SELF_INFO::szSignature);
    auto external_message_m2c_query_ranklist_type = cqyh::gen_msg::usertype<ExternalMessage::M2C_QUERY_RANKLIST>(L, external_message_namespace, "M2C_QUERY_RANKLIST", "ExternalMessage::M2C_QUERY_RANKLIST", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_m2c_query_ranklist_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::M2C_QUERY_RANKLIST(); });
    external_message_m2c_query_ranklist_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::M2C_QUERY_RANKLIST>(new ExternalMessage::M2C_QUERY_RANKLIST()); });
    external_message_m2c_query_ranklist_type["byRankListType"] = sol::property([](const ExternalMessage::M2C_QUERY_RANKLIST& self) { return self.byRankListType; }, [](ExternalMessage::M2C_QUERY_RANKLIST& self, decltype(ExternalMessage::M2C_QUERY_RANKLIST::byRankListType) value) { self.byRankListType = value; });
    external_message_m2c_query_ranklist_type["wSelfRank"] = sol::property([](const ExternalMessage::M2C_QUERY_RANKLIST& self) { return self.wSelfRank; }, [](ExternalMessage::M2C_QUERY_RANKLIST& self, decltype(ExternalMessage::M2C_QUERY_RANKLIST::wSelfRank) value) { self.wSelfRank = value; });
    external_message_m2c_query_ranklist_type["wLastSelfRank"] = sol::property([](const ExternalMessage::M2C_QUERY_RANKLIST& self) { return self.wLastSelfRank; }, [](ExternalMessage::M2C_QUERY_RANKLIST& self, decltype(ExternalMessage::M2C_QUERY_RANKLIST::wLastSelfRank) value) { self.wLastSelfRank = value; });
    external_message_m2c_query_ranklist_type["nTotalPage"] = sol::property([](const ExternalMessage::M2C_QUERY_RANKLIST& self) { return self.nTotalPage; }, [](ExternalMessage::M2C_QUERY_RANKLIST& self, decltype(ExternalMessage::M2C_QUERY_RANKLIST::nTotalPage) value) { self.nTotalPage = value; });
    external_message_m2c_query_ranklist_type["llKey"] = cqyh::lua::array_property(&ExternalMessage::M2C_QUERY_RANKLIST::llKey);
    external_message_m2c_query_ranklist_type["llValue"] = cqyh::lua::array_property(&ExternalMessage::M2C_QUERY_RANKLIST::llValue);
    external_message_m2c_query_ranklist_type["wLastRank"] = cqyh::lua::array_property(&ExternalMessage::M2C_QUERY_RANKLIST::wLastRank);
}

}
