#include "stdafx.h"
#include "generator_bindings.h"
#include "../registration.h"
#include "../../../../../../Common/message/external_message_header.h"
#include "../../../../../../Common/message/c2g_message.h"
#include "../../../../../../Common/define/friend_def.h"
#include "../../../../../../Common/define/string_def.h"

namespace cqyh::gen_msg::bindings {

void register_external_client_to_game_characters_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_c2g_role_adjust_target_point_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT>(L, external_message_namespace, "C2G_ROLE_ADJUST_TARGET_POINT", "ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_adjust_target_point_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT(); });
    external_message_c2g_role_adjust_target_point_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT>(new ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT()); });
    external_message_c2g_role_adjust_target_point_type["wTargetX"] = sol::property([](const ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT& self) { return self.wTargetX; }, [](ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT& self, decltype(ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT::wTargetX) value) { self.wTargetX = value; });
    external_message_c2g_role_adjust_target_point_type["wTargetY"] = sol::property([](const ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT& self) { return self.wTargetY; }, [](ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT& self, decltype(ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT::wTargetY) value) { self.wTargetY = value; });
    external_message_c2g_role_adjust_target_point_type["wTargetZ"] = sol::property([](const ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT& self) { return self.wTargetZ; }, [](ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT& self, decltype(ExternalMessage::C2G_ROLE_ADJUST_TARGET_POINT::wTargetZ) value) { self.wTargetZ = value; });
    auto external_message_c2g_query_char_data_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_CHAR_DATA>(L, external_message_namespace, "C2G_QUERY_CHAR_DATA", "ExternalMessage::C2G_QUERY_CHAR_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_char_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_CHAR_DATA(); });
    external_message_c2g_query_char_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_CHAR_DATA>(new ExternalMessage::C2G_QUERY_CHAR_DATA()); });
    external_message_c2g_query_char_data_type["dwCharID"] = sol::property([](const ExternalMessage::C2G_QUERY_CHAR_DATA& self) { return self.dwCharID; }, [](ExternalMessage::C2G_QUERY_CHAR_DATA& self, decltype(ExternalMessage::C2G_QUERY_CHAR_DATA::dwCharID) value) { self.dwCharID = value; });
    external_message_c2g_query_char_data_type["dwCRC"] = sol::property([](const ExternalMessage::C2G_QUERY_CHAR_DATA& self) { return self.dwCRC; }, [](ExternalMessage::C2G_QUERY_CHAR_DATA& self, decltype(ExternalMessage::C2G_QUERY_CHAR_DATA::dwCRC) value) { self.dwCRC = value; });
    auto external_message_c2g_query_role_detail_data_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA>(L, external_message_namespace, "C2G_QUERY_ROLE_DETAIL_DATA", "ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_role_detail_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA(); });
    external_message_c2g_query_role_detail_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA>(new ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA()); });
    external_message_c2g_query_role_detail_data_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self, decltype(ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA::dwRoleID) value) { self.dwRoleID = value; });
    external_message_c2g_query_role_detail_data_type["dwAttrCRC"] = sol::property([](const ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self) { return self.dwAttrCRC; }, [](ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self, decltype(ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA::dwAttrCRC) value) { self.dwAttrCRC = value; });
    external_message_c2g_query_role_detail_data_type["dwEquipCRC"] = sol::property([](const ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self) { return self.dwEquipCRC; }, [](ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self, decltype(ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA::dwEquipCRC) value) { self.dwEquipCRC = value; });
    external_message_c2g_query_role_detail_data_type["dwSpiritCRC"] = sol::property([](const ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self) { return self.dwSpiritCRC; }, [](ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self, decltype(ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA::dwSpiritCRC) value) { self.dwSpiritCRC = value; });
    external_message_c2g_query_role_detail_data_type["dwSpellCRC"] = sol::property([](const ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self) { return self.dwSpellCRC; }, [](ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA& self, decltype(ExternalMessage::C2G_QUERY_ROLE_DETAIL_DATA::dwSpellCRC) value) { self.dwSpellCRC = value; });
    auto external_message_c2g_query_role_pow_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_ROLE_POW>(L, external_message_namespace, "C2G_QUERY_ROLE_POW", "ExternalMessage::C2G_QUERY_ROLE_POW", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_role_pow_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_ROLE_POW(); });
    external_message_c2g_query_role_pow_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_ROLE_POW>(new ExternalMessage::C2G_QUERY_ROLE_POW()); });
    external_message_c2g_query_role_pow_type["dwRoleID"] = sol::property([](const ExternalMessage::C2G_QUERY_ROLE_POW& self) { return self.dwRoleID; }, [](ExternalMessage::C2G_QUERY_ROLE_POW& self, decltype(ExternalMessage::C2G_QUERY_ROLE_POW::dwRoleID) value) { self.dwRoleID = value; });
    auto external_message_c2g_query_role_location_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_QUERY_ROLE_LOCATION>(L, external_message_namespace, "C2G_QUERY_ROLE_LOCATION", "ExternalMessage::C2G_QUERY_ROLE_LOCATION", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_query_role_location_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_QUERY_ROLE_LOCATION(); });
    external_message_c2g_query_role_location_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_QUERY_ROLE_LOCATION>(new ExternalMessage::C2G_QUERY_ROLE_LOCATION()); });
    external_message_c2g_query_role_location_type["szRoleName"] = cqyh::lua::string_property(&ExternalMessage::C2G_QUERY_ROLE_LOCATION::szRoleName);
    auto external_message_c2g_role_mine_type = cqyh::gen_msg::usertype<ExternalMessage::C2G_ROLE_MINE>(L, external_message_namespace, "C2G_ROLE_MINE", "ExternalMessage::C2G_ROLE_MINE", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_c2g_role_mine_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::C2G_ROLE_MINE(); });
    external_message_c2g_role_mine_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::C2G_ROLE_MINE>(new ExternalMessage::C2G_ROLE_MINE()); });
    external_message_c2g_role_mine_type["wDestX"] = sol::property([](const ExternalMessage::C2G_ROLE_MINE& self) { return self.wDestX; }, [](ExternalMessage::C2G_ROLE_MINE& self, decltype(ExternalMessage::C2G_ROLE_MINE::wDestX) value) { self.wDestX = value; });
    external_message_c2g_role_mine_type["wDestY"] = sol::property([](const ExternalMessage::C2G_ROLE_MINE& self) { return self.wDestY; }, [](ExternalMessage::C2G_ROLE_MINE& self, decltype(ExternalMessage::C2G_ROLE_MINE::wDestY) value) { self.wDestY = value; });
}

}
