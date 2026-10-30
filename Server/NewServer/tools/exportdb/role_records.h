#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class CSqlMgr;
class CLuaScript;

namespace cqyh::exportdb
{
inline constexpr std::size_t role_client_variables = 128;
inline constexpr std::size_t role_spell_slots = 12;
inline constexpr std::size_t role_key_size = 4 + 25 + role_client_variables * 4;

struct RoleBlobInfo
{
    std::uint32_t id = 0;
    std::size_t base_offset = 0;
    std::size_t base_size = 0;
    std::uint32_t created = 0;
    std::uint32_t saved = 0;
    std::uint32_t deleted = 0;
    std::array<std::int32_t, 4> location{};
};
struct DecodedRole
{
    std::vector<std::byte> bytes;
    RoleBlobInfo info;
};
std::expected<RoleBlobInfo, std::string> InspectRoleBlob(std::span<const std::byte> bytes);
std::expected<DecodedRole, std::string> DecodeRoleBlob(std::span<const std::byte> compressed);
bool ClearRoleDeleteTime(DecodedRole& role);

struct RoleReport
{
    std::uint32_t id = 0;
    std::string name;
    std::string user;
    std::int32_t level = 0;
    std::int32_t experience = 0;
    std::int32_t class_id = 0;
    std::array<std::int64_t, 2> money{};
    bool deleted = false;
    std::array<std::int32_t, 4> location{};
    std::uint32_t created = 0;
    std::uint32_t saved = 0;
    std::int64_t item_count = 0;
    std::array<std::int32_t, role_spell_slots> spells{};
    std::vector<std::int32_t> statistics;
    std::array<std::int32_t, role_client_variables> variables{};
};
std::size_t RoleStatisticCount() noexcept;
std::string RoleReportHeader();
std::expected<std::string, std::string> FormatRoleReport(const RoleReport& report);
std::array<std::byte, role_key_size> MakeRoleKey(const RoleReport& report);
bool WriteRoleReports(std::span<const RoleReport> reports, const std::filesystem::path& directory);

enum class RoleSelection { all, one_day, highest };
using RoleReader = std::move_only_function<std::expected<RoleReport, std::string>(DecodedRole&, std::string_view)>;
bool ExportRoleRecords(CSqlMgr& manager, RoleReader reader,
    const std::filesystem::path& directory = ".", RoleSelection selection = RoleSelection::all);
// Requires the normal gameworld configuration/libraries initialized by the caller.
bool ExportRoles(CSqlMgr& manager, const std::filesystem::path& directory = ".");

struct NewRole
{
    std::string name;
    std::string user;
    std::vector<std::byte> bytes;
};
using RoleProducer = std::move_only_function<std::expected<NewRole, std::string>(std::uint32_t)>;
// Uses an initially drained manager. Work remains asynchronous, with at most
// 32 outstanding requests; owned completion state detects exhausted SQL retries.
bool InsertRoleRecords(CSqlMgr& manager, std::uint32_t start, std::uint64_t count, RoleProducer producer);
bool InsertRoles(CSqlMgr& manager, CLuaScript& script, std::uint32_t start, std::uint64_t count);
}
