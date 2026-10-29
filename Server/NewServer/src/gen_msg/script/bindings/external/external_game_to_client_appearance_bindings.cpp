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

void register_external_game_to_client_appearance_bindings(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    sol::table external_message_namespace = globals["ExternalMessage"].get_or_create<sol::table>();
    auto external_message_char_appear_data_type = cqyh::gen_msg::usertype<ExternalMessage::CHAR_APPEAR_DATA>(L, external_message_namespace, "CHAR_APPEAR_DATA", "ExternalMessage::CHAR_APPEAR_DATA");
    external_message_char_appear_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::CHAR_APPEAR_DATA(); });
    external_message_char_appear_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::CHAR_APPEAR_DATA>(new ExternalMessage::CHAR_APPEAR_DATA()); });
    external_message_char_appear_data_type["dwCharID"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.dwCharID; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::dwCharID) value) { self.dwCharID = value; });
    external_message_char_appear_data_type["byMoveState"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.byMoveState; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::byMoveState) value) { self.byMoveState = value; });
    external_message_char_appear_data_type["wCurrentX"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.wCurrentX; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::wCurrentX) value) { self.wCurrentX = value; });
    external_message_char_appear_data_type["wCurrentY"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.wCurrentY; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::wCurrentY) value) { self.wCurrentY = value; });
    external_message_char_appear_data_type["wCurrentZ"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.wCurrentZ; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::wCurrentZ) value) { self.wCurrentZ = value; });
    external_message_char_appear_data_type["wFaceDirection"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.wFaceDirection; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_char_appear_data_type["byLifePercent"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.byLifePercent; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::byLifePercent) value) { self.byLifePercent = value; });
    external_message_char_appear_data_type["byMountID"] = sol::property([](const ExternalMessage::CHAR_APPEAR_DATA& self) { return self.byMountID; }, [](ExternalMessage::CHAR_APPEAR_DATA& self, decltype(ExternalMessage::CHAR_APPEAR_DATA::byMountID) value) { self.byMountID = value; });
    auto external_message_g2c_char_appear_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_APPEAR>(L, external_message_namespace, "G2C_CHAR_APPEAR", "ExternalMessage::G2C_CHAR_APPEAR", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_appear_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_APPEAR(); });
    external_message_g2c_char_appear_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_APPEAR>(new ExternalMessage::G2C_CHAR_APPEAR()); });
    external_message_g2c_char_appear_type["byAppearType"] = sol::property([](const ExternalMessage::G2C_CHAR_APPEAR& self) { return self.byAppearType; }, [](ExternalMessage::G2C_CHAR_APPEAR& self, decltype(ExternalMessage::G2C_CHAR_APPEAR::byAppearType) value) { self.byAppearType = value; });
    external_message_g2c_char_appear_type["stAppearData"] = &ExternalMessage::G2C_CHAR_APPEAR::stAppearData;
    auto external_message_g2c_sync_char_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_CHAR_LIST>(L, external_message_namespace, "G2C_SYNC_CHAR_LIST", "ExternalMessage::G2C_SYNC_CHAR_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_char_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_CHAR_LIST(); });
    external_message_g2c_sync_char_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_CHAR_LIST>(new ExternalMessage::G2C_SYNC_CHAR_LIST()); });
    external_message_g2c_sync_char_list_type["astAppearData"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_CHAR_LIST::astAppearData);
    auto external_message_missile_appear_data_type = cqyh::gen_msg::usertype<ExternalMessage::MISSILE_APPEAR_DATA>(L, external_message_namespace, "MISSILE_APPEAR_DATA", "ExternalMessage::MISSILE_APPEAR_DATA", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_missile_appear_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::MISSILE_APPEAR_DATA(); });
    external_message_missile_appear_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::MISSILE_APPEAR_DATA>(new ExternalMessage::MISSILE_APPEAR_DATA()); });
    external_message_missile_appear_data_type["dwMissileID"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.dwMissileID; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::dwMissileID) value) { self.dwMissileID = value; });
    external_message_missile_appear_data_type["dwOwnerID"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.dwOwnerID; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::dwOwnerID) value) { self.dwOwnerID = value; });
    external_message_missile_appear_data_type["wTemplateID"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wTemplateID; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wTemplateID) value) { self.wTemplateID = value; });
    external_message_missile_appear_data_type["wCurrentX"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wCurrentX; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wCurrentX) value) { self.wCurrentX = value; });
    external_message_missile_appear_data_type["wCurrentY"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wCurrentY; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wCurrentY) value) { self.wCurrentY = value; });
    external_message_missile_appear_data_type["wCurrentZ"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wCurrentZ; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wCurrentZ) value) { self.wCurrentZ = value; });
    external_message_missile_appear_data_type["wLifeFrm"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wLifeFrm; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wLifeFrm) value) { self.wLifeFrm = value; });
    external_message_missile_appear_data_type["wFaceDirection"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wFaceDirection; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_missile_appear_data_type["wDestX"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wDestX; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wDestX) value) { self.wDestX = value; });
    external_message_missile_appear_data_type["wDestY"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wDestY; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wDestY) value) { self.wDestY = value; });
    external_message_missile_appear_data_type["wDestZ"] = sol::property([](const ExternalMessage::MISSILE_APPEAR_DATA& self) { return self.wDestZ; }, [](ExternalMessage::MISSILE_APPEAR_DATA& self, decltype(ExternalMessage::MISSILE_APPEAR_DATA::wDestZ) value) { self.wDestZ = value; });
    auto external_message_g2c_activate_mount_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_ACTIVATE_MOUNT>(L, external_message_namespace, "G2C_ACTIVATE_MOUNT", "ExternalMessage::G2C_ACTIVATE_MOUNT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_activate_mount_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_ACTIVATE_MOUNT(); });
    external_message_g2c_activate_mount_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_ACTIVATE_MOUNT>(new ExternalMessage::G2C_ACTIVATE_MOUNT()); });
    external_message_g2c_activate_mount_type["nMountID"] = sol::property([](const ExternalMessage::G2C_ACTIVATE_MOUNT& self) { return self.nMountID; }, [](ExternalMessage::G2C_ACTIVATE_MOUNT& self, decltype(ExternalMessage::G2C_ACTIVATE_MOUNT::nMountID) value) { self.nMountID = value; });
    auto external_message_g2c_char_on_mount_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_ON_MOUNT>(L, external_message_namespace, "G2C_CHAR_ON_MOUNT", "ExternalMessage::G2C_CHAR_ON_MOUNT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_on_mount_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_ON_MOUNT(); });
    external_message_g2c_char_on_mount_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_ON_MOUNT>(new ExternalMessage::G2C_CHAR_ON_MOUNT()); });
    external_message_g2c_char_on_mount_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_ON_MOUNT& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_ON_MOUNT& self, decltype(ExternalMessage::G2C_CHAR_ON_MOUNT::dwCharID) value) { self.dwCharID = value; });
    external_message_g2c_char_on_mount_type["byMountID"] = sol::property([](const ExternalMessage::G2C_CHAR_ON_MOUNT& self) { return self.byMountID; }, [](ExternalMessage::G2C_CHAR_ON_MOUNT& self, decltype(ExternalMessage::G2C_CHAR_ON_MOUNT::byMountID) value) { self.byMountID = value; });
    auto external_message_g2c_char_off_mount_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_CHAR_OFF_MOUNT>(L, external_message_namespace, "G2C_CHAR_OFF_MOUNT", "ExternalMessage::G2C_CHAR_OFF_MOUNT", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_char_off_mount_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_CHAR_OFF_MOUNT(); });
    external_message_g2c_char_off_mount_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_CHAR_OFF_MOUNT>(new ExternalMessage::G2C_CHAR_OFF_MOUNT()); });
    external_message_g2c_char_off_mount_type["dwCharID"] = sol::property([](const ExternalMessage::G2C_CHAR_OFF_MOUNT& self) { return self.dwCharID; }, [](ExternalMessage::G2C_CHAR_OFF_MOUNT& self, decltype(ExternalMessage::G2C_CHAR_OFF_MOUNT::dwCharID) value) { self.dwCharID = value; });
    auto external_message_g2c_sync_role_avatar_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_ROLE_AVATAR>(L, external_message_namespace, "G2C_SYNC_ROLE_AVATAR", "ExternalMessage::G2C_SYNC_ROLE_AVATAR", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_role_avatar_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_ROLE_AVATAR(); });
    external_message_g2c_sync_role_avatar_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_ROLE_AVATAR>(new ExternalMessage::G2C_SYNC_ROLE_AVATAR()); });
    external_message_g2c_sync_role_avatar_type["dwRoleID"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_AVATAR& self) { return self.dwRoleID; }, [](ExternalMessage::G2C_SYNC_ROLE_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_ROLE_AVATAR::dwRoleID) value) { self.dwRoleID = value; });
    external_message_g2c_sync_role_avatar_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_AVATAR& self) { return self.byConSlot; }, [](ExternalMessage::G2C_SYNC_ROLE_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_ROLE_AVATAR::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_sync_role_avatar_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_AVATAR& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_SYNC_ROLE_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_ROLE_AVATAR::byItemSlot) value) { self.byItemSlot = value; });
    external_message_g2c_sync_role_avatar_type["nTemplateID"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_AVATAR& self) { return self.nTemplateID; }, [](ExternalMessage::G2C_SYNC_ROLE_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_ROLE_AVATAR::nTemplateID) value) { self.nTemplateID = value; });
    external_message_g2c_sync_role_avatar_type["byRefineFx"] = sol::property([](const ExternalMessage::G2C_SYNC_ROLE_AVATAR& self) { return self.byRefineFx; }, [](ExternalMessage::G2C_SYNC_ROLE_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_ROLE_AVATAR::byRefineFx) value) { self.byRefineFx = value; });
    auto external_message_g2c_sync_hero_avatar_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_HERO_AVATAR>(L, external_message_namespace, "G2C_SYNC_HERO_AVATAR", "ExternalMessage::G2C_SYNC_HERO_AVATAR", sol::base_classes, sol::bases<ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_hero_avatar_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_HERO_AVATAR(); });
    external_message_g2c_sync_hero_avatar_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_HERO_AVATAR>(new ExternalMessage::G2C_SYNC_HERO_AVATAR()); });
    external_message_g2c_sync_hero_avatar_type["dwHeroID"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_AVATAR& self) { return self.dwHeroID; }, [](ExternalMessage::G2C_SYNC_HERO_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_HERO_AVATAR::dwHeroID) value) { self.dwHeroID = value; });
    external_message_g2c_sync_hero_avatar_type["byConSlot"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_AVATAR& self) { return self.byConSlot; }, [](ExternalMessage::G2C_SYNC_HERO_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_HERO_AVATAR::byConSlot) value) { self.byConSlot = value; });
    external_message_g2c_sync_hero_avatar_type["byItemSlot"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_AVATAR& self) { return self.byItemSlot; }, [](ExternalMessage::G2C_SYNC_HERO_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_HERO_AVATAR::byItemSlot) value) { self.byItemSlot = value; });
    external_message_g2c_sync_hero_avatar_type["nTemplateID"] = sol::property([](const ExternalMessage::G2C_SYNC_HERO_AVATAR& self) { return self.nTemplateID; }, [](ExternalMessage::G2C_SYNC_HERO_AVATAR& self, decltype(ExternalMessage::G2C_SYNC_HERO_AVATAR::nTemplateID) value) { self.nTemplateID = value; });
    auto external_message_doodad_appear_data_type = cqyh::gen_msg::usertype<ExternalMessage::DOODAD_APPEAR_DATA>(L, external_message_namespace, "DOODAD_APPEAR_DATA", "ExternalMessage::DOODAD_APPEAR_DATA");
    external_message_doodad_appear_data_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::DOODAD_APPEAR_DATA(); });
    external_message_doodad_appear_data_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::DOODAD_APPEAR_DATA>(new ExternalMessage::DOODAD_APPEAR_DATA()); });
    external_message_doodad_appear_data_type["dwDoodadID"] = sol::property([](const ExternalMessage::DOODAD_APPEAR_DATA& self) { return self.dwDoodadID; }, [](ExternalMessage::DOODAD_APPEAR_DATA& self, decltype(ExternalMessage::DOODAD_APPEAR_DATA::dwDoodadID) value) { self.dwDoodadID = value; });
    external_message_doodad_appear_data_type["nTemplateID"] = sol::property([](const ExternalMessage::DOODAD_APPEAR_DATA& self) { return self.nTemplateID; }, [](ExternalMessage::DOODAD_APPEAR_DATA& self, decltype(ExternalMessage::DOODAD_APPEAR_DATA::nTemplateID) value) { self.nTemplateID = value; });
    external_message_doodad_appear_data_type["wCurrentX"] = sol::property([](const ExternalMessage::DOODAD_APPEAR_DATA& self) { return self.wCurrentX; }, [](ExternalMessage::DOODAD_APPEAR_DATA& self, decltype(ExternalMessage::DOODAD_APPEAR_DATA::wCurrentX) value) { self.wCurrentX = value; });
    external_message_doodad_appear_data_type["wCurrentY"] = sol::property([](const ExternalMessage::DOODAD_APPEAR_DATA& self) { return self.wCurrentY; }, [](ExternalMessage::DOODAD_APPEAR_DATA& self, decltype(ExternalMessage::DOODAD_APPEAR_DATA::wCurrentY) value) { self.wCurrentY = value; });
    external_message_doodad_appear_data_type["wCurrentZ"] = sol::property([](const ExternalMessage::DOODAD_APPEAR_DATA& self) { return self.wCurrentZ; }, [](ExternalMessage::DOODAD_APPEAR_DATA& self, decltype(ExternalMessage::DOODAD_APPEAR_DATA::wCurrentZ) value) { self.wCurrentZ = value; });
    external_message_doodad_appear_data_type["wFaceDirection"] = sol::property([](const ExternalMessage::DOODAD_APPEAR_DATA& self) { return self.wFaceDirection; }, [](ExternalMessage::DOODAD_APPEAR_DATA& self, decltype(ExternalMessage::DOODAD_APPEAR_DATA::wFaceDirection) value) { self.wFaceDirection = value; });
    external_message_doodad_appear_data_type["byRemainOpenCount"] = &ExternalMessage::DOODAD_APPEAR_DATA::byRemainOpenCount;
    auto external_message_g2c_sync_mount_list_type = cqyh::gen_msg::usertype<ExternalMessage::G2C_SYNC_MOUNT_LIST>(L, external_message_namespace, "G2C_SYNC_MOUNT_LIST", "ExternalMessage::G2C_SYNC_MOUNT_LIST", sol::base_classes, sol::bases<ExternalMessage::FLEX_MESSAGE_HEADER, ExternalMessage::MESSAGE_HEADER>());
    external_message_g2c_sync_mount_list_type.set_function("new", [](sol::table) -> decltype(auto) { return new ExternalMessage::G2C_SYNC_MOUNT_LIST(); });
    external_message_g2c_sync_mount_list_type.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<ExternalMessage::G2C_SYNC_MOUNT_LIST>(new ExternalMessage::G2C_SYNC_MOUNT_LIST()); });
    external_message_g2c_sync_mount_list_type["byMountID"] = cqyh::lua::array_property(&ExternalMessage::G2C_SYNC_MOUNT_LIST::byMountID);
}

}
