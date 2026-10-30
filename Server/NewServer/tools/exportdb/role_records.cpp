#include "stdafx.h"
#include "role_records.h"
#include "define.h"
#include "sql_mgr.h"
#include "role_db_def.h"
#include "achievement_def.h"
#include "variable_def.h"
#include "string_def.h"
#include "zlib.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <format>
#include <fstream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>

namespace cqyh::exportdb
{
namespace
{
using Bytes = std::span<const std::byte>;
template<class T> bool read(Bytes bytes, T& value, std::size_t offset = 0)
{
    static_assert(std::is_trivially_copyable_v<T>);
    if (offset > bytes.size() || sizeof(T) > bytes.size() - offset) return false;
    std::memcpy(&value, bytes.data() + offset, sizeof(T));
    return true;
}
std::string safe_field(std::string text)
{
    std::ranges::replace_if(text, [](char value) { return value == '\r' || value == '\n' || value == '\t' || value == '\0'; }, '|');
    return text;
}
std::string time_text(std::uint32_t stamp)
{
    const std::time_t value = stamp;
    std::tm local{};
    if (localtime_s(&local, &value) != 0) throw std::runtime_error("cannot format role timestamp");
    return std::format("{}-{:02}-{:02} {:02}:{:02}:{:02}", local.tm_year + 1900,
        local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
}
bool close(std::ofstream& output)
{
    output.flush();
    if (!output) return false;
    output.close();
    return !output.fail();
}
#pragma pack(push, 1)
struct RoleRowHeader
{
    std::int64_t id;
    char name[25];
    char user[65];
    std::uint32_t size;
};
struct LegacyKey
{
    std::uint32_t id;
    char name[25];
    std::int32_t variables[role_client_variables];
};
#pragma pack(pop)
static_assert(sizeof(RoleRowHeader) == 102);
static_assert(sizeof(LegacyKey) == role_key_size && offsetof(LegacyKey, name) == 4 && offsetof(LegacyKey, variables) == 29);
static_assert(MAX_CLIENT_VARIABLE_COUNT == role_client_variables && ROLE_NAME_LEN == 25);
static_assert(sizeof(ROLE_DB_BASE_DATA) == sizeof(ROLE_DB_DEF_V_1::ROLE_DB_BASE_DATA));
static_assert(offsetof(ROLE_DB_BASE_DATA, dwDeleteTime) == offsetof(ROLE_DB_DEF_V_1::ROLE_DB_BASE_DATA, dwDeleteTime));
}

std::expected<RoleBlobInfo, std::string> InspectRoleBlob(Bytes bytes)
{
    ROLE_DB_DATA header;
    if (!read(bytes, header) || bytes.size() > 65536 || header.dwLen != bytes.size())
        return std::unexpected("invalid role blob length");
    if (header.dwVersion == 0 || header.dwVersion > ROLE_DB_DATA_CURRENT_VERSION)
        return std::unexpected("unsupported role blob version");
    if (header.dwCRC != crc32(0, reinterpret_cast<const Bytef*>(bytes.data()) + 8, static_cast<uInt>(bytes.size() - 8)))
        return std::unexpected("role blob CRC mismatch");
    const auto payload = bytes.subspan(sizeof(header));
    for (const auto& block : header.BlockTable)
        if (block.dwStartOffset > payload.size() || block.dwBlockSize > payload.size() - block.dwStartOffset ||
            (block.dwBlockSize != 0 && block.btBlockType >= rbtTotal))
            return std::unexpected("invalid role block range/type");
    const auto& base_block = header.BlockTable[rbtBase];
    if (base_block.btBlockType != rbtBase || base_block.dwStartOffset != 0 || base_block.dwBlockSize != sizeof(ROLE_DB_BASE_DATA))
        return std::unexpected("invalid role base block");
    ROLE_DB_BASE_DATA base;
    const auto base_offset = sizeof(header) + base_block.dwStartOffset;
    if (!read(bytes, base, base_offset) || base.byVersion != ROLE_DB_BASE_DATA_CURRENT_VERSION || base.dwRoleID != header.dwRoleID)
        return std::unexpected("invalid role base data");
    return RoleBlobInfo{header.dwRoleID, base_offset, sizeof(base), base.dwCreateTime, base.dwSaveTime,
        base.dwDeleteTime, {base.nSceneTempID, base.nSceneID, base.nX, base.nY}};
}
std::expected<DecodedRole, std::string> DecodeRoleBlob(Bytes compressed)
{
    if (compressed.empty() || compressed.size() > 8192) return std::unexpected("invalid compressed role size");
    std::vector<std::byte> bytes(65536);
    uLongf size = static_cast<uLongf>(bytes.size());
    if (uncompress(reinterpret_cast<Bytef*>(bytes.data()), &size,
        reinterpret_cast<const Bytef*>(compressed.data()), static_cast<uLong>(compressed.size())) != Z_OK)
        return std::unexpected("cannot decompress role data");
    bytes.resize(size);
    auto info = InspectRoleBlob(bytes);
    if (!info) return std::unexpected(std::move(info.error()));
    return DecodedRole{std::move(bytes), *info};
}
bool ClearRoleDeleteTime(DecodedRole& role)
{
    const auto info = InspectRoleBlob(role.bytes);
    if (!info) return false;
    const std::uint32_t zero = 0;
    std::memcpy(role.bytes.data() + info->base_offset + offsetof(ROLE_DB_BASE_DATA, dwDeleteTime), &zero, sizeof(zero));
    const auto checksum = crc32(0, reinterpret_cast<const Bytef*>(role.bytes.data()) + 8, static_cast<uInt>(role.bytes.size() - 8));
    std::memcpy(role.bytes.data() + 4, &checksum, sizeof(checksum));
    role.info = *info;
    role.info.deleted = 0;
    return true;
}
std::size_t RoleStatisticCount() noexcept { return avdStatisticEnd - avdStatisticBegin; }
std::string RoleReportHeader()
{
    std::string text = "roleid\trolename\tusername\tlevel\texp\tclass\tmoney0\tmoney1\tdeleted\tscenetempid\tsceneid\tposx\tposy\tcreatetime\tsavetime\titemcount";
    for (std::size_t index = 1; index <= role_spell_slots; ++index)
        text += std::format("\t spell{}{}", index, index == role_spell_slots ? "" : " ");
    for (int index = avdStatisticBegin; index < avdStatisticEnd; ++index) text += std::format("\tStat{}", index);
    for (std::size_t index = 0; index < role_client_variables; ++index) text += std::format("\tClientVar{}", index);
    text += '\n';
    return text;
}
std::expected<std::string, std::string> FormatRoleReport(const RoleReport& report)
{
    if (report.statistics.size() != RoleStatisticCount()) return std::unexpected("incorrect role statistic count");
    std::string text = std::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
        report.id, safe_field(report.name), safe_field(report.user), report.level, report.experience, report.class_id,
        report.money[0], report.money[1], report.deleted ? 1 : 0,
        report.location[0], report.location[1], report.location[2], report.location[3],
        time_text(report.created), time_text(report.saved), report.item_count);
    for (const auto value : report.spells) text += std::format("\t{}", value);
    for (const auto value : report.statistics) text += std::format("\t{}", value);
    for (const auto value : report.variables) text += std::format("\t{}", value);
    text += '\n';
    return text;
}
std::array<std::byte, role_key_size> MakeRoleKey(const RoleReport& report)
{
    std::array<std::byte, role_key_size> bytes{};
    std::memcpy(bytes.data(), &report.id, sizeof(report.id));
    const auto length = std::min({report.name.size(), report.name.find('\0'), std::size_t{24}});
    std::memcpy(bytes.data() + 4, report.name.data(), length);
    std::memcpy(bytes.data() + 29, report.variables.data(), sizeof(report.variables));
    return bytes;
}
bool WriteRoleReports(std::span<const RoleReport> reports, const std::filesystem::path& directory)
{
    try
    {
        // Check record shapes before replacing either existing report.
        for (const auto& report : reports) if (report.statistics.size() != RoleStatisticCount()) return false;
        std::ofstream text(directory / "exportdb.txt", std::ios::binary | std::ios::trunc);
        std::ofstream binary(directory / "exportdb.dat", std::ios::binary | std::ios::trunc);
        if (!text || !binary) return false;
        text << RoleReportHeader();
        for (const auto& report : reports)
        {
            const auto line = FormatRoleReport(report);
            if (!line) return false;
            const auto key = MakeRoleKey(report);
            text.write(line->data(), static_cast<std::streamsize>(line->size()));
            binary.write(reinterpret_cast<const char*>(key.data()), static_cast<std::streamsize>(key.size()));
            if (!text || !binary) return false;
        }
        return close(text) && close(binary);
    }
    catch (const std::exception& error) { ERR("Role report failed: %s", error.what()); return false; }
}
bool ExportRoleRecords(CSqlMgr& manager, RoleReader reader, const std::filesystem::path& directory, RoleSelection selection)
{
    if (!reader) return false;
    try
    {
        int statement;
        switch (selection)
        {
        case RoleSelection::all: statement = mysqlSelectAllRole; break;
        case RoleSelection::one_day: statement = mysqlSelect1DayRole; break;
        case RoleSelection::highest: statement = mysqlSelectHighestRole; break;
        default: return false;
        }
        std::vector<RoleReport> reports;
        auto collect = [&reports, reader = std::move(reader)](std::list<void*>& rows, void*) mutable -> BOOL
        {
            reports.reserve(rows.size());
            for (const auto* row : rows)
            {
                if (!row) return FALSE;
                RoleRowHeader header;
                std::memcpy(&header, row, sizeof(header));
                if (header.id < 0 || static_cast<std::uint64_t>(header.id) > std::numeric_limits<std::uint32_t>::max() || header.size > 8192)
                    return FALSE;
                auto decoded = DecodeRoleBlob({static_cast<const std::byte*>(row) + sizeof(header), header.size});
                if (!decoded)
                { ERR("Cannot decode exported role: %s", decoded.error().c_str()); return FALSE; }
                if (decoded->info.id != header.id) return FALSE;
                const auto end = std::find(std::begin(header.user), std::end(header.user), '\0');
                const std::string_view user(header.user, std::min<std::size_t>(end - header.user, 64));
                auto report = reader(*decoded, user);
                if (!report)
                { ERR("Cannot read exported role: %s", report.error().c_str()); return FALSE; }
                if (report->id != decoded->info.id) return FALSE;
                reports.push_back(std::move(*report));
            }
            return TRUE;
        };
        if (!manager.SyncExecSqlStmtOwned(statement, std::move(collect), "")) return false;
        return WriteRoleReports(reports, directory);
    }
    catch (const std::exception& error) { ERR("Role export failed: %s", error.what()); return false; }
}
bool InsertRoleRecords(CSqlMgr& manager, std::uint32_t start, std::uint64_t count, RoleProducer producer)
{
    constexpr auto domain = std::uint64_t{1} << 32;
    if (count > domain - start || !manager.IsCompleted()) return false;
    if (count == 0) return true;
    if (!producer) return false;
    struct Completion { std::uint64_t queued = 0; std::uint64_t succeeded = 0; };
    std::shared_ptr<Completion> completed;
    try { completed = std::make_shared<Completion>(); }
    catch (const std::exception& error) { ERR("Role insertion failed: %s", error.what()); return false; }
    const auto drain = [&]
    {
        while (!manager.IsCompleted())
        {
            if (!manager.MainLoop()) return false;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return completed->succeeded == completed->queued;
    };
    bool success = true;
    try
    {
        for (std::uint64_t next = start; next < static_cast<std::uint64_t>(start) + count; ++next)
        {
            auto role = producer(static_cast<std::uint32_t>(next));
            if (!role) { ERR("Role creation failed: %s", role.error().c_str()); success = false; break; }
            const auto info = InspectRoleBlob(role->bytes);
            if (!info || info->id != next || role->name.size() > 24 || role->user.size() > 63 ||
                role->name.find('\0') != std::string::npos || role->user.find('\0') != std::string::npos)
            { success = false; break; }
            std::vector<std::byte> compressed(compressBound(static_cast<uLong>(role->bytes.size())));
            uLongf size = static_cast<uLongf>(compressed.size());
            if (compress(reinterpret_cast<Bytef*>(compressed.data()), &size,
                reinterpret_cast<const Bytef*>(role->bytes.data()), static_cast<uLong>(role->bytes.size())) != Z_OK || size > 8192 || info->base_size > 8192)
            { success = false; break; }
            if (!manager.AsyncExecSqlStmtOwned(mysqlInsertRole,
                [completed](std::list<void*>&, void*) -> BOOL { ++completed->succeeded; return TRUE; },
                "lssbb", static_cast<INT64>(next), role->name.c_str(), role->user.c_str(),
                reinterpret_cast<UINT8*>(role->bytes.data() + info->base_offset), static_cast<UINT32>(info->base_size),
                reinterpret_cast<UINT8*>(compressed.data()), static_cast<UINT32>(size)))
            { success = false; break; }
            ++completed->queued;
            if (completed->queued % 32 == 0 && !drain()) { success = false; break; }
        }
    }
    catch (const std::exception& error) { ERR("Role insertion failed: %s", error.what()); success = false; }
    // Earlier submissions may already have committed; drain and report their
    // outcome even if a later producer or enqueue failed. This is not a transaction.
    return drain() && success;
}
}
