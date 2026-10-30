#include "stdafx.h"
#include "guild.h"
#include "sql_mgr.h"
#include "define.h"
#include "guild_def.h"
#include "string_def.h"
#include "../../src/manageserver/guild/guild_db_def_v_5.h"
#include "zlib.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <format>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace cqyh::exportdb
{
namespace
{
using Bytes = std::span<const std::byte>;
template<class T> bool read(Bytes bytes, T& value, std::size_t offset = 0)
{
    static_assert(std::is_trivially_copyable_v<T>);
    if (offset > bytes.size() || sizeof(T) > bytes.size() - offset)
        return false;
    std::memcpy(&value, bytes.data() + offset, sizeof(T));
    return true;
}
std::uint32_t identifier(std::int64_t value)
{
    if (value < 0 || static_cast<std::uint64_t>(value) > std::numeric_limits<std::uint32_t>::max())
        throw std::runtime_error("SQL identifier or timestamp is outside its unsigned 32-bit range");
    return static_cast<std::uint32_t>(value);
}
template<std::size_t Size> std::string text(const char (&buffer)[Size], std::size_t limit = Size - 1)
{
    const auto end = std::find(buffer, buffer + Size, '\0');
    return {buffer, std::min<std::size_t>(end - buffer, limit)};
}
#pragma pack(push, 1)
struct GuildRow
{
    char created[21];
    char deleted[21];
    std::int64_t id;
    std::int32_t version;
    char name[25];
    std::int32_t group;
    std::int64_t leader;
    char creator[25];
    std::int64_t timestamp;
    char announce[101];
    char notice[201];
};
struct MemberRow
{
    std::int64_t role;
    std::int64_t guild;
    char guild_name[25];
    char role_name[25];
    char user_name[65];
};
#pragma pack(pop)
static_assert(sizeof(GuildRow) == 426 && sizeof(MemberRow) == 131);
struct Guild
{
    std::uint32_t id = 0;
    std::string name;
    std::int32_t level = 0;
    std::int32_t group = 0;
    std::uint32_t leader = 0;
    std::string creator;
    std::string created;
    std::string deleted;
    std::int64_t members = 0;
};
bool finish(std::ofstream& output)
{
    output.flush();
    if (!output)
        return false;
    output.close();
    return !output.fail();
}
}

std::expected<GuildResources, std::string> ParseGuildBlob(Bytes bytes)
{
    using namespace GUILD_DEF_VER_5;
    GUILD_DB_DATA header;
    if (!read(bytes, header) || header.dwLen < sizeof(header) || header.dwLen >= 60000 || header.dwLen > bytes.size())
        return std::unexpected("invalid guild blob length");
    bytes = bytes.first(header.dwLen);
    const auto checksum = crc32(0, reinterpret_cast<const Bytef*>(bytes.data()) + 8, static_cast<uInt>(bytes.size() - 8));
    if (header.dwCRC != checksum)
        return std::unexpected("guild blob CRC mismatch");
    if (header.dwVersion < 5)
        INF("load old guild %u db data %u %u", header.dwGuildID, header.dwVersion, header.dwLen);
    const auto payload = bytes.subspan(sizeof(header));
    GuildResources resources;
    for (const auto& descriptor : header.BlockTable)
    {
        if (descriptor.dwStartOffset > payload.size() || descriptor.dwBlockSize > payload.size() - descriptor.dwStartOffset)
            return std::unexpected("guild block exceeds the blob");
        if (descriptor.dwBlockSize == 0)
            continue;
        const auto block = payload.subspan(descriptor.dwStartOffset, descriptor.dwBlockSize);
        switch (descriptor.byBlockType)
        {
        case gbtBase:
        {
            GUILD_DB_BASE base;
            if (block.size() != sizeof(base) || !read(block, base))
                return std::unexpected("invalid guild base block");
            resources.money = base.nMoney;
            resources.wood = base.nWood;
            resources.herb = base.nHerb;
            resources.crystal = base.nCrystal;
            resources.silver = base.nSilver;
            break;
        }
        case gbtWelfare:
        {
            GUILD_DB_WELFARE welfare;
            if (!read(block, welfare) || block.size() != sizeof(welfare) + welfare.byUpgradeWelfareNum * sizeof(GUILD_DB_WELFARE_UPGRADE))
                return std::unexpected("invalid guild welfare block");
            break;
        }
        case gbtBuilding:
        {
            GUILD_DB_BUILDING building;
            if (!read(block, building) || block.size() != sizeof(building) + building.wBuildingBufferLength + building.wUpgradeBufferLength ||
                building.wBuildingBufferLength != building.byBuildingNum * sizeof(GUILD_DB_BUILDING_DATA) ||
                building.wUpgradeBufferLength != building.byUpgradeBuildingNum * sizeof(GUILD_DB_BUILDING_UPGRADE))
                return std::unexpected("invalid guild building block");
            const auto buildings = block.subspan(sizeof(building), building.wBuildingBufferLength);
            for (std::size_t index = 0; index < building.byBuildingNum; ++index)
            {
                GUILD_DB_BUILDING_DATA item;
                if (!read(buildings, item, index * sizeof(item)))
                    return std::unexpected("truncated guild building");
                if (item.byType == GUILD_B_MAIN)
                    resources.level = item.byLevel;
            }
            break;
        }
        case gbtDiplomat:
        {
            GUILD_DB_DIPLOMAT diplomat;
            if (!read(block, diplomat) || diplomat.byAllianceNum > GUILD_MAX_ALLIANCE_NUM || diplomat.byEnemyNum > GUILD_MAX_ENEMY_NUM ||
                block.size() != sizeof(diplomat) + (diplomat.byAllianceNum + diplomat.byEnemyNum) * sizeof(GUILD_DB_DIPLOMAT_DATA))
                return std::unexpected("invalid guild diplomat block");
            break;
        }
        // The old exporter does not interpret these payloads. Their enclosing
        // ranges are still checked before accepting them.
        case gbtDungeon:
        case gbtStorage:
        case gbtHistory:
        case gbtStorageHistory:
            break;
        default:
            return std::unexpected("unknown guild block type");
        }
    }
    return resources;
}

bool ExportGuilds(CSqlMgr& manager, const std::filesystem::path& path)
{
    try
    {
        std::vector<Guild> guilds;
        auto load = [&guilds](std::list<void*>& rows, void*) -> BOOL
        {
            guilds.reserve(rows.size());
            for (const auto* row : rows)
            {
                if (!row)
                    return FALSE;
                // SQL result widths are fixed by export_sql.cpp, including the
                // blob's native length prefix. Copy packed fields into objects.
                const Bytes bytes(static_cast<const std::byte*>(row), sizeof(GuildRow) + sizeof(std::uint32_t) + 60000);
                GuildRow fields;
                std::uint32_t size;
                if (!read(bytes, fields) || !read(bytes, size, sizeof(fields)) || size > 60000)
                    return FALSE;
                // Decode this tool's blob here. The shared legacy {z} path does
                // not publish a reliable length prefix in its callback rows.
                std::array<std::byte, 60000> decoded;
                uLongf decoded_size = static_cast<uLongf>(decoded.size());
                if (uncompress(reinterpret_cast<Bytef*>(decoded.data()), &decoded_size,
                    reinterpret_cast<const Bytef*>(bytes.data()) + sizeof(fields) + sizeof(size), size) != Z_OK)
                {
                    ERR("Cannot decompress guild data");
                    return FALSE;
                }
                const auto parsed = ParseGuildBlob(std::span(decoded).first(decoded_size));
                if (!parsed)
                {
                    ERR("Cannot export guild: %s", parsed.error().c_str());
                    return FALSE;
                }
                if (fields.group < 0 || fields.group > 255)
                    return FALSE;
                // Validate the unused timestamp too; it is still part of the
                // native result schema and must not silently wrap.
                (void)identifier(fields.timestamp);
                guilds.push_back({identifier(fields.id), text(fields.name), parsed->level, fields.group,
                    identifier(fields.leader), text(fields.creator), text(fields.created), text(fields.deleted), 0});
            }
            return TRUE;
        };
        if (!manager.SyncExecSqlStmtOwned(mysqlSelectGuild, std::move(load), ""))
            return false;
        for (auto& guild : guilds)
        {
            auto count = [&guild](std::list<void*>& rows, void*) -> BOOL
            {
                if (rows.size() != 1 || !rows.front())
                    return FALSE;
                std::memcpy(&guild.members, rows.front(), sizeof(guild.members));
                return guild.members >= 0;
            };
            // Synchronous completion keeps the vector element alive throughout
            // the owned callback; the vector is not modified in this loop.
            if (!manager.SyncExecSqlStmtOwned(mysqlSelectGuildMemberCount, std::move(count), "l", static_cast<INT64>(guild.id)))
                return false;
        }
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        if (!output)
            return false;
        output << "GuildID\tGuildName\tGuildLevel\tGroupID\tLeaderID\tCreaterName\tCreateTime\tDeleteTime\tMemberCount\n";
        for (const auto& guild : guilds)
            output << std::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n", guild.id, guild.name,
                guild.level, guild.group, guild.leader, guild.creator, guild.created, guild.deleted, guild.members);
        return finish(output);
    }
    catch (const std::exception& error)
    {
        ERR("Guild export failed: %s", error.what());
        return false;
    }
}

bool ExportMembers(CSqlMgr& manager, const std::filesystem::path& path)
{
    try
    {
        std::vector<MemberRow> members;
        if (!manager.SyncExecSqlStmtOwned(mysqlSelectGuildMemberInfo,
            [&members](std::list<void*>& rows, void*) -> BOOL
            {
                members.reserve(rows.size());
                for (const auto* row : rows)
                {
                    if (!row)
                        return FALSE;
                    MemberRow member;
                    std::memcpy(&member, row, sizeof(member));
                    (void)identifier(member.role);
                    (void)identifier(member.guild);
                    members.push_back(member);
                }
                return TRUE;
            }, ""))
            return false;
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        if (!output)
            return false;
        output << "RoleID\tGuildID\tGuildName\tRoleName\tUserName\n";
        for (const auto& member : members)
            output << std::format("{}\t{}\t{}\t{}\t{}\n", identifier(member.role), identifier(member.guild),
                text(member.guild_name), text(member.role_name), text(member.user_name, USER_NAME_LEN - 1));
        return finish(output);
    }
    catch (const std::exception& error)
    {
        ERR("Guild member export failed: %s", error.what());
        return false;
    }
}
}
