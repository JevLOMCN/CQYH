#include "stdafx.h"
#include "sol_bindings.h"
#include "net_event_msg.h"
#include "role_db_def.h"
extern int sol_external_message_open(lua_State* L);
extern int sol_error_def_open(lua_State* L);
int sol_msg_open(lua_State* L) {
    sol_external_message_open(L);
    sol_error_def_open(L);
    sol::state_view lua(L);
    auto role = cqyh::lua::usertype<ROLE_DB_BASE_DATA>(L, lua.globals(), "ROLE_DB_BASE_DATA", "ROLE_DB_BASE_DATA");
    role["byVersion"] = &ROLE_DB_BASE_DATA::byVersion;
    role["byRequireChangeName"] = &ROLE_DB_BASE_DATA::byRequireChangeName;
    role["wWingID"] = &ROLE_DB_BASE_DATA::wWingID;
    role["dwRoleID"] = &ROLE_DB_BASE_DATA::dwRoleID;
    role["dwCreateTime"] = &ROLE_DB_BASE_DATA::dwCreateTime;
    role["dwSaveTime"] = &ROLE_DB_BASE_DATA::dwSaveTime;
    role["dwDeleteTime"] = &ROLE_DB_BASE_DATA::dwDeleteTime;
    role["szRoleName"] = cqyh::lua::string_property(&ROLE_DB_BASE_DATA::szRoleName);
    role["szUserName"] = cqyh::lua::string_property(&ROLE_DB_BASE_DATA::szUserName);
    role["byClass"] = &ROLE_DB_BASE_DATA::byClass;
    role["bySex"] = &ROLE_DB_BASE_DATA::bySex;
    role["byHairType"] = &ROLE_DB_BASE_DATA::byHairType;
    role["byHairColor"] = &ROLE_DB_BASE_DATA::byHairColor;
    role["byFaceType"] = &ROLE_DB_BASE_DATA::byFaceType;
    role["bySkinColor"] = &ROLE_DB_BASE_DATA::bySkinColor;
    role["byLevel"] = &ROLE_DB_BASE_DATA::byLevel;
    role["byReserved3"] = &ROLE_DB_BASE_DATA::byReserved3;
    role["nSceneTempID"] = &ROLE_DB_BASE_DATA::nSceneTempID;
    role["nSceneID"] = &ROLE_DB_BASE_DATA::nSceneID;
    role["nX"] = &ROLE_DB_BASE_DATA::nX;
    role["nY"] = &ROLE_DB_BASE_DATA::nY;
    role["nZ"] = &ROLE_DB_BASE_DATA::nZ;
    role["wFaceDirection"] = &ROLE_DB_BASE_DATA::wFaceDirection;
    role["wMountID"] = &ROLE_DB_BASE_DATA::wMountID;
    role["nStaticReturnSceneID"] = &ROLE_DB_BASE_DATA::nStaticReturnSceneID;
    role["nStaticReturnX"] = &ROLE_DB_BASE_DATA::nStaticReturnX;
    role["nStaticReturnY"] = &ROLE_DB_BASE_DATA::nStaticReturnY;
    role["nStaticReturnZ"] = &ROLE_DB_BASE_DATA::nStaticReturnZ;
    role["wStaticReturnFaceDirection"] = &ROLE_DB_BASE_DATA::wStaticReturnFaceDirection;
    role["byReserved4"] = &ROLE_DB_BASE_DATA::byReserved4;
    role["byStaticReturnSceneTempID"] = &ROLE_DB_BASE_DATA::byStaticReturnSceneTempID;
    role["nReviveSceneID"] = &ROLE_DB_BASE_DATA::nReviveSceneID;
    role["nRevivePtX"] = &ROLE_DB_BASE_DATA::nRevivePtX;
    role["nRevivePtY"] = &ROLE_DB_BASE_DATA::nRevivePtY;
    role["nRevivePtZ"] = &ROLE_DB_BASE_DATA::nRevivePtZ;
    role["wReviveFaceDirection"] = &ROLE_DB_BASE_DATA::wReviveFaceDirection;
    role["byReserved5"] = &ROLE_DB_BASE_DATA::byReserved5;
    role["byReviveSceneTempID"] = &ROLE_DB_BASE_DATA::byReviveSceneTempID;
    role["nAvatar"] = cqyh::lua::array_property(&ROLE_DB_BASE_DATA::nAvatar);
    role["byRefineFx"] = &ROLE_DB_BASE_DATA::byRefineFx;
    role["szGuildName"] = cqyh::lua::string_property(&ROLE_DB_BASE_DATA::szGuildName);
    sol::table messages = lua["ExternalMessage"];
    messages["b2c_robot_connected"] = ExternalMessage::b2c_robot_connected;
    messages["t2c_robot_connected"] = ExternalMessage::t2c_robot_connected;
    messages["t2c_robot_disconnected"] = ExternalMessage::t2c_robot_disconnected;
    cqyh::lua::usertype<ExternalMessage::T2C_ROBOT_CONNECTED>(L, messages, "T2C_ROBOT_CONNECTED", "ExternalMessage::T2C_ROBOT_CONNECTED");
    cqyh::lua::usertype<ExternalMessage::T2C_ROBOT_DISCONNECTED>(L, messages, "T2C_ROBOT_DISCONNECTED", "ExternalMessage::T2C_ROBOT_DISCONNECTED");
    cqyh::lua::usertype<ExternalMessage::B2C_ROBOT_CONNECTED>(L, messages, "B2C_ROBOT_CONNECTED", "ExternalMessage::B2C_ROBOT_CONNECTED");
    sol::table errors = lua.create_named_table("error_code");
    for (const auto& entry : lua.globals()) {
        if (entry.second.is<lua_Number>()) errors[entry.first] = entry.second;
    }
    return 1;
}
