#include "stdafx.h"
#include "string_ex.h" // Required by the existing gameworld data-manager headers.
#include "role_records.h"
#include "role_lua.h"
#include "role.h"
#include "game_world.h"
#include "role_db_def.h"
#include "sol_bindings.h"
#include "lua_script.h"
#include "data_manager.h"
#include "achievement_data.h"
#include "achievement_def.h"
#include "variable_def.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <ctime>
#include <format>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace cqyh::exportdb
{
namespace
{
static_assert(role_spell_slots == qssActiveEnd - qssActiveBegin);
std::expected<RoleReport, std::string> read_role(DecodedRole& decoded, std::string_view user)
{
    CRole role;
    if (!role.Init(decoded.info.id, 0)) return std::unexpected("cannot initialize exported role");
    bool repaired = false;
    if (!role.Load(reinterpret_cast<const char*>(decoded.bytes.data()), static_cast<UINT32>(decoded.bytes.size())))
    {
        if (decoded.info.deleted == 0 || !ClearRoleDeleteTime(decoded) ||
            !role.Load(reinterpret_cast<const char*>(decoded.bytes.data()), static_cast<UINT32>(decoded.bytes.size())))
            return std::unexpected("cannot load exported role");
        repaired = true;
    }
    RoleReport report;
    report.id = role.GetObjID();
    report.name = role.GetName();
    report.user = user;
    report.level = role.GetLevel();
    report.experience = role.GetExp();
    report.class_id = role.GetClass();
    report.money = {role.GetItemBag()->GetMoney(MONEYTYPE_01), role.GetItemBag()->GetMoney(MONEYTYPE_02)};
    report.deleted = repaired;
    report.location = decoded.info.location;
    report.created = role.GetCreateTime();
    report.saved = decoded.info.saved;
    auto& bag = *role.GetItemBag();
    bag.Begin();
    while (const auto* item = bag.Next())
    {
        if (item->m_iteminfo.m_nTemplateID != 93001) continue;
        const auto amount = static_cast<std::int64_t>(item->m_iteminfo.m_nCurDur);
        if ((amount > 0 && report.item_count > std::numeric_limits<std::int64_t>::max() - amount) ||
            (amount < 0 && report.item_count < std::numeric_limits<std::int64_t>::min() - amount))
            return std::unexpected("role item count overflow");
        report.item_count += amount;
    }
    for (std::size_t index = 0; index < report.spells.size(); ++index)
        report.spells[index] = role.GetSpellSlot(qssActiveBegin + static_cast<int>(index))->GetMainSpellID();
    report.statistics.reserve(RoleStatisticCount());
    for (int index = avdStatisticBegin; index < avdStatisticEnd; ++index)
        report.statistics.push_back(role.GetAchievementList().GetVariable(index));
    for (std::size_t index = 0; index < report.variables.size(); ++index)
        report.variables[index] = role.GetClientVariable(static_cast<int>(index));
    return report; // CRole's destructor always calls UnInit, including failures.
}

std::expected<NewRole, std::string> create_role(CLuaScript& script, std::uint32_t id)
{
    CRole role;
    if (!role.Init(id, 0)) return std::unexpected("cannot initialize inserted role");
    role.SetClass(g_Random(clsPlayerBegin, clsPlayerEnd - 1));
    role.SetSex(g_Random(cstMale, cstFemale));
    auto* state = script.GetLuaState();
    if (!state) return std::unexpected("missing role creation Lua state");
    const auto restore = detail::PreserveTargetRole(state);
    cqyh::lua::push_object(state, &role, "CRole");
    lua_setglobal(state, "TargetRole");
    BOOL accepted = FALSE;
    if (!script.CallFunction("OnNewGMCommand", "s:b", "InitRobot(false)", &accepted) || !accepted)
        return std::unexpected("role creation script failed");
    for (int achievement = 1; achievement < 480; ++achievement)
        if (g_Random(2) == 0 && CDataManager<ACHIEVEMENT_DATA>::GetInstance().GetData(achievement))
            role.GetAchievementList().Complete(achievement);
    for (int variable = 0; variable < MAX_ROLE_VARIABLE_COUNT; ++variable)
        role.SetVariable(variable, g_Random(2000000000));

    // CRole::Save retains the server's native fixed-buffer API. Use the same
    // 64 KiB scratch limit as the tool's reader, then validate the returned blob
    // and SQL capacities before any request is enqueued.
    std::array<char, 65536> bytes{};
    UINT32 size = 0;
    if (!role.Save(bytes.data(), size) || size > bytes.size()) return std::unexpected("cannot save inserted role");
    NewRole result;
    result.name = std::format("role{}", id);
    result.user = std::format("user{}", id);
    result.bytes.resize(size);
    std::memcpy(result.bytes.data(), bytes.data(), size);
    if (id % 1000 == 0)
    {
        INF("%u role records prepared", id);
        CGameWorld::GetInstance().SetGameTime(static_cast<UINT32>(std::time(nullptr)));
    }
    return result;
}
}
bool ExportRoles(CSqlMgr& manager, const std::filesystem::path& directory)
{
    return ExportRoleRecords(manager, read_role, directory);
}
bool InsertRoles(CSqlMgr& manager, CLuaScript& script, std::uint32_t start, std::uint64_t count)
{
    CGameWorld::GetInstance().SetGameTime(static_cast<UINT32>(std::time(nullptr)));
    return InsertRoleRecords(manager, start, count,
        [&script](std::uint32_t id) { return create_role(script, id); });
}
}
