#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_guild_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_guild_create_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_CREATE>(L, external_message_namespace, "C2G_GUILD_CREATE", "ExternalMessage::C2G_GUILD_CREATE", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_create_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_CREATE(); });
    external_message_c2g_guild_create_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_CREATE>(new ExternalMessage::C2G_GUILD_CREATE()); });
    external_message_c2g_guild_create_type["szGuildName"] = cqyh::lua::string_property(&ExternalMessage::C2G_GUILD_CREATE::szGuildName);
    external_message_c2g_guild_create_type["szAnnounce"] = cqyh::lua::string_property(&ExternalMessage::C2G_GUILD_CREATE::szAnnounce);
    auto external_message_c2g_guild_contribute_fund_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND>(L, external_message_namespace, "C2G_GUILD_CONTRIBUTE_FUND", "ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_contribute_fund_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND(); });
    external_message_c2g_guild_contribute_fund_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND>(new ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND()); });
    external_message_c2g_guild_contribute_fund_type["nMoney"] = sol::property([](const ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND& self) { return self.nMoney; }, [](ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND& self, decltype(ExternalMessage::C2G_GUILD_CONTRIBUTE_FUND::nMoney) value) { self.nMoney = value; });
    auto external_message_c2g_guild_buy_aura_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_BUY_AURA>(L, external_message_namespace, "C2G_GUILD_BUY_AURA", "ExternalMessage::C2G_GUILD_BUY_AURA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_buy_aura_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_BUY_AURA(); });
    external_message_c2g_guild_buy_aura_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_BUY_AURA>(new ExternalMessage::C2G_GUILD_BUY_AURA()); });
    external_message_c2g_guild_buy_aura_type["byType"] = sol::property([](const ExternalMessage::C2G_GUILD_BUY_AURA& self) { return self.byType; }, [](ExternalMessage::C2G_GUILD_BUY_AURA& self, decltype(ExternalMessage::C2G_GUILD_BUY_AURA::byType) value) { self.byType = value; });
    auto external_message_c2g_guild_change_weather_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_CHANGE_WEATHER>(L, external_message_namespace, "C2G_GUILD_CHANGE_WEATHER", "ExternalMessage::C2G_GUILD_CHANGE_WEATHER", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_change_weather_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_CHANGE_WEATHER(); });
    external_message_c2g_guild_change_weather_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_CHANGE_WEATHER>(new ExternalMessage::C2G_GUILD_CHANGE_WEATHER()); });
    external_message_c2g_guild_change_weather_type["byWeatherType"] = sol::property([](const ExternalMessage::C2G_GUILD_CHANGE_WEATHER& self) { return self.byWeatherType; }, [](ExternalMessage::C2G_GUILD_CHANGE_WEATHER& self, decltype(ExternalMessage::C2G_GUILD_CHANGE_WEATHER::byWeatherType) value) { self.byWeatherType = value; });
    external_message_c2g_guild_change_weather_type["byTimeType"] = sol::property([](const ExternalMessage::C2G_GUILD_CHANGE_WEATHER& self) { return self.byTimeType; }, [](ExternalMessage::C2G_GUILD_CHANGE_WEATHER& self, decltype(ExternalMessage::C2G_GUILD_CHANGE_WEATHER::byTimeType) value) { self.byTimeType = value; });
    auto external_message_c2g_guild_enter_map_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_GUILD_ENTER_MAP>(L, external_message_namespace, "C2G_GUILD_ENTER_MAP", "ExternalMessage::C2G_GUILD_ENTER_MAP", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_guild_enter_map_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_GUILD_ENTER_MAP(); });
    external_message_c2g_guild_enter_map_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_GUILD_ENTER_MAP>(new ExternalMessage::C2G_GUILD_ENTER_MAP()); });
    external_message_c2g_guild_enter_map_type["byMapType"] = sol::property([](const ExternalMessage::C2G_GUILD_ENTER_MAP& self) { return self.byMapType; }, [](ExternalMessage::C2G_GUILD_ENTER_MAP& self, decltype(ExternalMessage::C2G_GUILD_ENTER_MAP::byMapType) value) { self.byMapType = value; });
    external_message_c2g_guild_enter_map_type["dwSceneID"] = sol::property([](const ExternalMessage::C2G_GUILD_ENTER_MAP& self) { return self.dwSceneID; }, [](ExternalMessage::C2G_GUILD_ENTER_MAP& self, decltype(ExternalMessage::C2G_GUILD_ENTER_MAP::dwSceneID) value) { self.dwSceneID = value; });
    auto external_message_c2g_leave_guild_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_LEAVE_GUILD>(L, external_message_namespace, "C2G_LEAVE_GUILD", "ExternalMessage::C2G_LEAVE_GUILD", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_leave_guild_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_LEAVE_GUILD(); });
    external_message_c2g_leave_guild_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_LEAVE_GUILD>(new ExternalMessage::C2G_LEAVE_GUILD()); });
}

}
