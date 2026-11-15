#include "stdafx.h"
#include "sol_bindings.h"
#include "stdlib.h"
#include "../../../Common/define/ranklist_def.h"

int sol_rank_list_def_open(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    globals["MAX_RANK_LIST_NODE_COUNT"] = static_cast<lua_Number>(MAX_RANK_LIST_NODE_COUNT);
    globals["MAX_RANK_NODE_NUM_PER_PAGE"] = static_cast<lua_Number>(MAX_RANK_NODE_NUM_PER_PAGE);
    globals["rlInvalid"] = static_cast<lua_Number>(rlInvalid);
    globals["rlBegin"] = static_cast<lua_Number>(rlBegin);
    globals["rlRoleBegin"] = static_cast<lua_Number>(rlRoleBegin);
    globals["rlLevel"] = static_cast<lua_Number>(rlLevel);
    globals["rlLevelWarrior"] = static_cast<lua_Number>(rlLevelWarrior);
    globals["rlLevelMage"] = static_cast<lua_Number>(rlLevelMage);
    globals["rlLevelTaoist"] = static_cast<lua_Number>(rlLevelTaoist);
    globals["rlLevelRogue"] = static_cast<lua_Number>(rlLevelRogue);
    globals["rlLevelArcher"] = static_cast<lua_Number>(rlLevelArcher);
    globals["rlPower"] = static_cast<lua_Number>(rlPower);
    globals["rlPowerWarrior"] = static_cast<lua_Number>(rlPowerWarrior);
    globals["rlPowerMage"] = static_cast<lua_Number>(rlPowerMage);
    globals["rlPowerTaoist"] = static_cast<lua_Number>(rlPowerTaoist);
    globals["rlPowerRogue"] = static_cast<lua_Number>(rlPowerRogue);
    globals["rlPowerArcher"] = static_cast<lua_Number>(rlPowerArcher);
    globals["rlMoney"] = static_cast<lua_Number>(rlMoney);
    globals["rlAchievement"] = static_cast<lua_Number>(rlAchievement);
    globals["rlMaster"] = static_cast<lua_Number>(rlMaster);
    globals["rlPkValue"] = static_cast<lua_Number>(rlPkValue);
    globals["rlTotalKillCount"] = static_cast<lua_Number>(rlTotalKillCount);
    globals["rlActivity01"] = static_cast<lua_Number>(rlActivity01);
    globals["rlActivity02"] = static_cast<lua_Number>(rlActivity02);
    globals["rlActivity03"] = static_cast<lua_Number>(rlActivity03);
    globals["rlActivity04"] = static_cast<lua_Number>(rlActivity04);
    globals["rlActivity05"] = static_cast<lua_Number>(rlActivity05);
    globals["rlActivity06"] = static_cast<lua_Number>(rlActivity06);
    globals["rlActivity07"] = static_cast<lua_Number>(rlActivity07);
    globals["rlActivity08"] = static_cast<lua_Number>(rlActivity08);
    globals["rlLevelSpear"] = static_cast<lua_Number>(rlLevelSpear);
    globals["rlLevelSorcerer"] = static_cast<lua_Number>(rlLevelSorcerer);
    globals["rlPowerSpear"] = static_cast<lua_Number>(rlPowerSpear);
    globals["rlPowerSorcerer"] = static_cast<lua_Number>(rlPowerSorcerer);
    globals["rlReserved05"] = static_cast<lua_Number>(rlReserved05);
    globals["rlReserved06"] = static_cast<lua_Number>(rlReserved06);
    globals["rlReserved07"] = static_cast<lua_Number>(rlReserved07);
    globals["rlReserved08"] = static_cast<lua_Number>(rlReserved08);
    globals["rlArena1v1"] = static_cast<lua_Number>(rlArena1v1);
    globals["rlArena1v1Warrior"] = static_cast<lua_Number>(rlArena1v1Warrior);
    globals["rlArena1v1Mage"] = static_cast<lua_Number>(rlArena1v1Mage);
    globals["rlArena1v1Taoist"] = static_cast<lua_Number>(rlArena1v1Taoist);
    globals["rlArena1v1Reserved01"] = static_cast<lua_Number>(rlArena1v1Reserved01);
    globals["rlArena1v1Reserved02"] = static_cast<lua_Number>(rlArena1v1Reserved02);
    globals["rlArena5v5"] = static_cast<lua_Number>(rlArena5v5);
    globals["rlArena5v5Reserved01"] = static_cast<lua_Number>(rlArena5v5Reserved01);
    globals["rlArena5v5Reserved02"] = static_cast<lua_Number>(rlArena5v5Reserved02);
    globals["rlArena5v5Reserved03"] = static_cast<lua_Number>(rlArena5v5Reserved03);
    globals["rlArena5v5Reserved04"] = static_cast<lua_Number>(rlArena5v5Reserved04);
    globals["rlArena5v5Reserved05"] = static_cast<lua_Number>(rlArena5v5Reserved05);
    globals["rlRoleEnd"] = static_cast<lua_Number>(rlRoleEnd);
    globals["rlGuildBegin"] = static_cast<lua_Number>(rlGuildBegin);
    globals["rlGuild_Level"] = static_cast<lua_Number>(rlGuild_Level);
    globals["rlGuild_Money"] = static_cast<lua_Number>(rlGuild_Money);
    globals["rlGuild_Resource_Herb"] = static_cast<lua_Number>(rlGuild_Resource_Herb);
    globals["rlGuild_Resource_Wood"] = static_cast<lua_Number>(rlGuild_Resource_Wood);
    globals["rlGuild_Resource_Crystal"] = static_cast<lua_Number>(rlGuild_Resource_Crystal);
    globals["rlGuild_Resource_Silver"] = static_cast<lua_Number>(rlGuild_Resource_Silver);
    globals["rlGuild_Pow"] = static_cast<lua_Number>(rlGuild_Pow);
    globals["rlGuild_War"] = static_cast<lua_Number>(rlGuild_War);
    globals["rlGuildEnd"] = static_cast<lua_Number>(rlGuildEnd);
    globals["rlTotal"] = static_cast<lua_Number>(rlTotal);
    return 1;
}
