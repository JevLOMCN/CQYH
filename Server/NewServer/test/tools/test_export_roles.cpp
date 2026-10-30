#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>
#include "std_type.h"
#include "process_error.h"
#include "sql_mgr.h"
#include "fake_mysql_client.h"
#include "role_records.h"
#include "role_lua.h"
#include "role_db_def.h"
#include "define.h"
#include "zlib.h"

namespace
{
namespace fs = std::filesystem;
using namespace cqyh::exportdb;
using Buffer = std::vector<std::byte>;
using fake_mysql::Cell;
using fake_mysql::Rows;
std::atomic<int> checks = 0;
std::atomic<int> failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}
template<class T> Buffer bytes(const T& value)
{
    Buffer result(sizeof(value)); std::memcpy(result.data(), &value, sizeof(value)); return result;
}
template<class T> Cell number(T value)
{
    Cell result(sizeof(value)); std::memcpy(result.data(), &value, sizeof(value)); return result;
}
void crc(Buffer& value)
{
    const auto checksum = crc32(0, reinterpret_cast<const Bytef*>(value.data()) + 8, static_cast<uInt>(value.size() - 8));
    std::memcpy(value.data() + 4, &checksum, 4);
}
Buffer blob(std::uint32_t id = 7, std::uint32_t deleted = 99, std::size_t extra = 0)
{
    ROLE_DB_DATA header;
    std::memset(&header, 0, sizeof(header));
    header.dwVersion = 3; header.dwRoleID = id;
    ROLE_DB_BASE_DATA base{};
    base.byVersion = 1; base.dwRoleID = id; base.byLevel = 2;
    base.dwCreateTime = 1577836800; base.dwSaveTime = 1577836860; base.dwDeleteTime = deleted;
    base.nSceneTempID = 1; base.nSceneID = 2; base.nX = -3; base.nY = 4;
    std::memcpy(base.szRoleName, "BlobName", 9);
    for (std::size_t index = 0; index < rbtTotal; ++index)
        header.BlockTable[index] = {static_cast<UINT8>(index), 0, sizeof(base)};
    header.BlockTable[rbtBase] = {rbtBase, sizeof(base), 0};
    if (extra) header.BlockTable[rbtMisc] = {rbtMisc, static_cast<UINT32>(extra), sizeof(base)};
    header.dwLen = static_cast<UINT32>(sizeof(header) + sizeof(base) + extra);
    auto result = bytes(header);
    const auto data = bytes(base);
    result.insert(result.end(), data.begin(), data.end());
    // Deterministic incompressible data for capacity validation.
    std::uint32_t random = 0x12345678;
    for (std::size_t index = 0; index < extra; ++index)
    {
        random ^= random << 13; random ^= random >> 17; random ^= random << 5;
        result.push_back(static_cast<std::byte>(random));
    }
    crc(result);
    return result;
}
Buffer modify(Buffer value, auto action)
{
    ROLE_DB_DATA header;
    std::memcpy(&header, value.data(), sizeof(header));
    action(header);
    std::memcpy(value.data(), &header, sizeof(header)); crc(value); return value;
}
Cell compress_bytes(const Buffer& value)
{
    Cell output(compressBound(static_cast<uLong>(value.size())));
    uLongf size = static_cast<uLongf>(output.size());
    if (compress(output.data(), &size, reinterpret_cast<const Bytef*>(value.data()), static_cast<uLong>(value.size())) != Z_OK)
        throw std::runtime_error("fixture compression failed");
    output.resize(size); return output;
}
Buffer as_bytes(const Cell& value)
{
    Buffer result(value.size()); std::memcpy(result.data(), value.data(), value.size()); return result;
}
std::string read(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary); return {std::istreambuf_iterator<char>(input), {}};
}
RoleReport sample()
{
    RoleReport report;
    report.id = 7; report.name = "Alice"; report.user = "user-a";
    report.level = 3; report.experience = 4; report.class_id = 2; report.money = {100, 200}; report.deleted = true;
    report.location = {1, 2, -3, 4}; report.created = 1577836800; report.saved = 1577836860; report.item_count = 9;
    for (std::size_t i = 0; i < report.spells.size(); ++i) report.spells[i] = 100 + static_cast<int>(i);
    for (std::size_t i = 0; i < RoleStatisticCount(); ++i) report.statistics.push_back(1000 + static_cast<int>(i));
    for (std::size_t i = 0; i < report.variables.size(); ++i) report.variables[i] = -500 + static_cast<int>(i);
    return report;
}
void parsing()
{
    const auto valid = blob();
    const auto info = InspectRoleBlob(valid);
    check(info && info->id == 7 && info->created == 1577836800 && info->saved == 1577836860 && info->deleted == 99
        && info->location == std::array<std::int32_t, 4>{1, 2, -3, 4}, "native role envelope/base metadata decodes independently of the gameworld");
    for (std::size_t length = 0; length < valid.size(); ++length)
        check(!InspectRoleBlob(std::span(valid).first(length)), "every truncated role prefix is rejected before metadata dereference");
    for (std::size_t offset = 0; offset < valid.size(); ++offset)
    {
        auto corrupt = valid; corrupt[offset] ^= std::byte{1};
        check(!InspectRoleBlob(corrupt), "per-byte role corruption fails length/version/CRC validation");
    }
    for (int kind = 0; kind < 9; ++kind)
    {
        const auto corrupt = modify(valid, [kind](auto& header)
        {
            if (kind == 0) header.dwVersion = 0;
            if (kind == 1) header.dwVersion = 4;
            if (kind == 2) header.BlockTable[0].dwBlockSize = std::numeric_limits<UINT32>::max();
            if (kind == 3) header.BlockTable[0].dwStartOffset = std::numeric_limits<UINT32>::max();
            if (kind == 4) header.BlockTable[0].btBlockType = 255;
            if (kind == 5) --header.BlockTable[0].dwBlockSize;
            if (kind == 6) header.BlockTable[0].btBlockType = rbtItem;
            if (kind == 7) ++header.dwRoleID;
            if (kind == 8) header.BlockTable[1].dwStartOffset = std::numeric_limits<UINT32>::max();
        });
        check(!InspectRoleBlob(corrupt), "CRC-valid invalid version/base/range/ID metadata is rejected");
    }
    const auto compressed = as_bytes(compress_bytes(valid));
    const auto decoded = DecodeRoleBlob(compressed);
    check(decoded && decoded->bytes == valid && decoded->info.id == 7, "bounded role decompression preserves the exact native blob");
    for (std::size_t length = 0; length < compressed.size(); ++length)
        check(!DecodeRoleBlob(std::span(compressed).first(length)), "every truncated compressed role is rejected");
    check(!DecodeRoleBlob(Buffer(8193)), "oversized compressed input is rejected");
    check(!DecodeRoleBlob(as_bytes(compress_bytes(Buffer(70000)))), "decompression cannot exceed the fixed 64 KiB scratch capacity");
    check(!InspectRoleBlob(blob(7, 0, 65536)), "oversized uncompressed role is rejected");
    auto invalid_base = valid;
    invalid_base[info->base_offset] = std::byte{0}; crc(invalid_base);
    check(!InspectRoleBlob(invalid_base), "unsupported base version is rejected before conversion");
    for (const int level : {0, 2})
    {
        auto value = valid;
        value[info->base_offset + offsetof(ROLE_DB_BASE_DATA, byLevel)] = static_cast<std::byte>(level); crc(value);
        DecodedRole role{value, *info};
        role.info.base_offset = std::numeric_limits<std::size_t>::max(); // Caller metadata is not trusted.
        check(ClearRoleDeleteTime(role) && InspectRoleBlob(role.bytes)->deleted == 0 && role.info.deleted == 0,
            "deleted-role repair revalidates offsets and recomputes CRC for all levels");
        const auto saved = role.bytes;
        check(ClearRoleDeleteTime(role) && role.bytes == saved, "deleted-role repair is repeatable");
    }
    DecodedRole invalid{{}, {}};
    check(!ClearRoleDeleteTime(invalid) && invalid.bytes.empty(), "invalid role repair does not write through unvalidated metadata");
}
void lua_lifetimes()
{
    const std::unique_ptr<lua_State, decltype(&lua_close)> state(luaL_newstate(), lua_close);
    check(state != nullptr, "create a real Lua VM for role-global lifetime checks");
    if (!state) return;
    auto* vm = state.get();
    int object = 1;
    for (const int kind : {0, 1, 2})
    {
        if (kind == 0) lua_pushnil(vm);
        if (kind == 1) lua_pushinteger(vm, 42);
        if (kind == 2) lua_pushlightuserdata(vm, &object);
        lua_setglobal(vm, "TargetRole");
        const auto top = lua_gettop(vm);
        try
        {
            const auto guard = detail::PreserveTargetRole(vm);
            lua_pushinteger(vm, 9); lua_setglobal(vm, "TargetRole");
            {
                const auto nested = detail::PreserveTargetRole(vm);
                lua_pushnil(vm); lua_setglobal(vm, "TargetRole");
            }
            lua_getglobal(vm, "TargetRole");
            check(lua_tointeger(vm, -1) == 9, "nested role scopes restore their immediate predecessor");
            lua_pop(vm, 1);
            throw std::runtime_error("role creation failure");
        }
        catch (const std::runtime_error&) {}
        lua_getglobal(vm, "TargetRole");
        check((kind == 0 && lua_isnil(vm, -1)) || (kind == 1 && lua_tointeger(vm, -1) == 42)
            || (kind == 2 && lua_touserdata(vm, -1) == &object), "exception cleanup restores the previous Lua role global without retaining a temporary C++ pointer");
        lua_pop(vm, 1);
        check(lua_gettop(vm) == top, "role-global guards preserve the Lua stack depth");
    }
    check(!detail::PreserveTargetRole(nullptr), "absent Lua state has an empty lifetime guard");
}
void reports(const fs::path& work, const fs::path& fixtures)
{
    auto report = sample();
    const auto formatted = FormatRoleReport(report);
    check(formatted && RoleReportHeader() + *formatted == read(fixtures / "export-original-role.txt"),
        "normal report header/columns/spacing/time formatting match the original format strings");
    check(role_key_size == 541, "packed key width remains the original 541-byte contract");
    const auto key = MakeRoleKey(report);
    std::array<std::byte, 541> expected{};
    expected[0] = std::byte{7};
    std::memcpy(expected.data() + 4, "Alice", 5);
    for (std::size_t i = 0; i < 128; ++i)
    {
        const auto value = static_cast<std::uint32_t>(-500 + static_cast<int>(i));
        for (std::size_t byte = 0; byte < 4; ++byte) expected[29 + i * 4 + byte] = static_cast<std::byte>(value >> (byte * 8));
    }
    check(key == expected, "key bytes deterministically initialize ID, terminated name, padding and all native variables");
    check(WriteRoleReports(std::span(&report, 1), work) && read(work / "exportdb.txt") == read(fixtures / "export-original-role.txt"),
        "owned binary-mode streams preserve original LF report bytes");
    check(read(work / "exportdb.dat") == std::string(reinterpret_cast<const char*>(expected.data()), expected.size()),
        "key report contains the frozen initialized record layout");
    report.id = std::numeric_limits<std::uint32_t>::max(); report.name = std::string(40, 'X');
    report.money = {std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::min()};
    report.item_count = 4294967296; report.created = 2147483648; report.saved = 4294967295;
    const auto wide = FormatRoleReport(report);
    check(wide && wide->starts_with("4294967295\t") && wide->find("9223372036854775807\t-9223372036854775808") != wide->npos
        && wide->find("2038-01-19 03:14:08\t2106-02-07 06:28:15\t4294967296") != wide->npos,
        "role IDs, money, item counts and timestamps retain their complete native domains");
    const auto long_key = MakeRoleKey(report);
    check(std::equal(long_key.begin() + 4, long_key.begin() + 28, std::array<std::byte, 24>{
        std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},
        std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},
        std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'},std::byte{'X'}}.begin())
        && long_key[28] == std::byte{}, "long key names are truncated to 24 bytes and terminated");
    report.name = "100%\r\n\t\xe5\x90\x8d"; report.user = "user\t\n";
    const auto escaped = FormatRoleReport(report);
    check(escaped && escaped->find("100%|||\xe5\x90\x8d\tuser||\t") != escaped->npos,
        "UTF-8/percent names remain literal while record delimiters cannot corrupt report columns");
    report.name = std::string("A\0hidden", 8);
    const auto nul_key = MakeRoleKey(report);
    check(FormatRoleReport(report)->find("A|hidden\t") != std::string::npos && nul_key[4] == std::byte{'A'}
        && std::all_of(nul_key.begin() + 5, nul_key.begin() + 29, [](auto value) { return value == std::byte{}; }),
        "embedded NUL is escaped in text and terminates a zero-padded native key name");
    const auto saved = read(work / "exportdb.txt"); report.statistics.clear();
    check(!FormatRoleReport(report) && !WriteRoleReports(std::span(&report, 1), work) && read(work / "exportdb.txt") == saved,
        "invalid statistic shapes fail before replacing reports");
    report = sample();
    check(!WriteRoleReports(std::span(&report, 1), work / "missing"), "missing output directory fails cleanly");
    report.statistics.assign(RoleStatisticCount(), std::numeric_limits<std::int32_t>::max());
    report.variables.fill(std::numeric_limits<std::int32_t>::min()); report.spells.fill(std::numeric_limits<std::int32_t>::max());
    const std::vector<RoleReport> many(10000, report);
    const auto line = FormatRoleReport(report);
    check(WriteRoleReports(many, work), "role reports stream beyond the old fixed message-buffer capacity");
    check(fs::file_size(work / "exportdb.txt") > 16 * 1024 * 1024
        && fs::file_size(work / "exportdb.txt") == RoleReportHeader().size() + line->size() * many.size()
        && fs::file_size(work / "exportdb.dat") == role_key_size * many.size(), "large text/key reports have exact complete lengths");
}
Rows role_rows;
std::vector<std::uint32_t> inserted;
int selection_calls = 0;
void binding(const MYSQL_BIND& value, enum_field_types type, unsigned long width)
{
    check(value.buffer && value.buffer_type == type && value.buffer_length == width && !value.is_unsigned,
        "role native binding type/width/signedness matches its production schema");
}
Rows query(std::string_view sql, std::span<const MYSQL_BIND> params, MYSQL_BIND* results)
{
    if (sql.starts_with("INSERT INTO role"))
    {
        check(params.size() == 5 && !results, "role insertion has exactly five parameters and no result columns");
        if (params.size() != 5) return {};
        binding(params[0], MYSQL_TYPE_LONGLONG, 8);
        binding(params[1], MYSQL_TYPE_STRING, 24); binding(params[2], MYSQL_TYPE_STRING, 63);
        binding(params[3], MYSQL_TYPE_BLOB, 8192); binding(params[4], MYSQL_TYPE_BLOB, 8192);
        std::int64_t id; std::memcpy(&id, params[0].buffer, 8);
        check(id >= 0 && id <= 4294967295, "insert binding preserves full unsigned-domain IDs");
        const auto decoded = DecodeRoleBlob({static_cast<const std::byte*>(params[4].buffer), *params[4].length});
        check(decoded && decoded->info.id == id && *params[3].length == decoded->info.base_size
            && std::memcmp(params[3].buffer, decoded->bytes.data() + decoded->info.base_offset, *params[3].length) == 0,
            "queued base/compressed buffers own consistent data after the producer's payload is destroyed");
        check(std::string_view(static_cast<const char*>(params[1].buffer), *params[1].length) == "role" + std::to_string(id)
            && std::string_view(static_cast<const char*>(params[2].buffer), *params[2].length) == "user" + std::to_string(id),
            "queued string buffers retain their original owned payload");
        inserted.push_back(static_cast<std::uint32_t>(id));
        return {};
    }
    check(params.empty() && sql.find("RoleData") != sql.npos, "role selection executes a production read-only query");
    binding(results[0], MYSQL_TYPE_LONGLONG, 8); binding(results[1], MYSQL_TYPE_STRING, 25);
    binding(results[2], MYSQL_TYPE_STRING, 65); binding(results[3], MYSQL_TYPE_BLOB, 8192);
    ++selection_calls;
    return role_rows;
}
fake_mysql::Row row(std::int64_t id, const Buffer& value, std::string_view user = "sql-user")
{
    const std::string name = "unused SQL name";
    return {number(id), Cell(name.begin(), name.end()), Cell(user.begin(), user.end()), compress_bytes(value)};
}
RoleReader reader(int& calls)
{
    return [token = std::make_unique<int>(1), &calls](DecodedRole& decoded, std::string_view user) -> std::expected<RoleReport, std::string>
    {
        ++calls;
        check(*token == 1, "move-only reader capture remains alive until SQL completion");
        auto result = sample(); result.id = decoded.info.id; result.name = "BlobName"; result.user = user; return result;
    };
}
NewRole new_role(std::uint32_t id) { return {"role" + std::to_string(id), "user" + std::to_string(id), blob(id, 0)}; }
void closed()
{
    check(!fake_mysql::ready && fake_mysql::connections == 0 && fake_mysql::statements == 0 && fake_mysql::threads == 0
        && fake_mysql::results == 0 && fake_mysql::scriptErrors == 0 && fake_mysql::orderErrors == 0,
        "role pipelines release SQL resources in dependency order");
}
void pipelines(const fs::path& work)
{
    fake_mysql::preparedHandler = query;
    CSqlMgr manager;
    check(manager.Init("fake", "", "", "fake", "utf8", 0, g_SqlStmtList, mysqlTotal, 1), "initialize production role SQL schemas against the no-socket client fake");
    int calls = 0;
    role_rows = {row(7, blob()), row(4294967295, blob(4294967295), std::string(65, 'U'))};
    for (const auto selection : {RoleSelection::all, RoleSelection::one_day, RoleSelection::highest})
        check(ExportRoleRecords(manager, reader(calls), work, selection), "all three selection schemas decode with consistent name/user/blob offsets");
    const auto report = read(work / "exportdb.txt");
    check(calls == 6 && report.find("7\tBlobName\tsql-user\t") != report.npos
        && report.find("4294967295\tBlobName\t" + std::string(64, 'U') + "\t") != report.npos,
        "SQL input order, full username width and high role IDs survive the full record pipeline");
    const auto old_calls = selection_calls;
    fake_mysql::failFetchAfterRow = true;
    check(ExportRoleRecords(manager, reader(calls), work) && selection_calls == old_calls + 2 && calls == 8,
        "role fetch retry discards partial rows before invoking readers or writing duplicate records");
    const auto saved = read(work / "exportdb.txt");
    for (const auto id : {std::int64_t{-1}, std::int64_t{4294967296}, std::int64_t{9}})
    {
        role_rows = {row(id, blob())};
        check(!ExportRoleRecords(manager, reader(calls), work) && read(work / "exportdb.txt") == saved,
            "invalid native role IDs or mismatched blob IDs cannot replace existing reports");
    }
    role_rows = {row(7, blob())}; role_rows.front().back() = Cell(8193);
    check(!ExportRoleRecords(manager, reader(calls), work), "native compressed-length truncation is rejected before decompression");
    role_rows = {row(7, blob())};
    check(!ExportRoleRecords(manager, [](DecodedRole&, std::string_view) -> std::expected<RoleReport, std::string> { return std::unexpected("reader failure"); }, work)
        && read(work / "exportdb.txt") == saved, "failed gameworld/model readers do not report partial success");
    check(!ExportRoleRecords(manager, [](DecodedRole&, std::string_view) -> std::expected<RoleReport, std::string> { throw std::runtime_error("reader exception"); }, work)
        && manager.IsCompleted(), "reader exceptions propagate as failure after SQL-owned result cleanup");
    check(!ExportRoleRecords(manager, nullptr, work), "missing readers are rejected");
    role_rows.clear();
    check(ExportRoleRecords(manager, reader(calls), work) && read(work / "exportdb.txt") == RoleReportHeader()
        && fs::file_size(work / "exportdb.dat") == 0, "empty role selections produce header-only/empty binary reports");
    check(InsertRoleRecords(manager, 0, 0, nullptr), "empty insertion range is a no-op");
    int produced = 0;
    inserted.clear();
    check(InsertRoleRecords(manager, 100, 70, [&produced](std::uint32_t id) -> std::expected<NewRole, std::string>
    {
        if (id == 132 || id == 164) check(inserted.size() == id - 100, "bounded asynchronous batches drain every 32 submitted records");
        ++produced; return new_role(id);
    }) && produced == 70 && inserted.size() == 70 && manager.IsCompleted(), "asynchronous insertion tracks every successful completion and drains before return");
    bool order = inserted.size() == 70;
    for (std::size_t i = 0; i < inserted.size(); ++i) order = order && inserted[i] == 100 + i;
    check(order, "single-worker insertion preserves producer order without borrowed payload lifetimes");
    inserted.clear();
    check(InsertRoleRecords(manager, 4294967295, 1, [](std::uint32_t id) -> std::expected<NewRole, std::string> { return new_role(id); })
        && inserted == std::vector<std::uint32_t>{4294967295}, "final unsigned-32-bit ID is inserted without a wrapping loop bound");
    check(!InsertRoleRecords(manager, 4294967295, 2, nullptr) && !InsertRoleRecords(manager, 1, std::numeric_limits<std::uint64_t>::max(), nullptr),
        "overflowing insertion ranges are rejected before production or queueing");
    inserted.clear();
    check(!InsertRoleRecords(manager, 10, 5, [](std::uint32_t id) -> std::expected<NewRole, std::string>
    { if (id == 13) return std::unexpected("stop producer"); return new_role(id); }) && inserted.size() == 3 && manager.IsCompleted(),
        "producer failure drains earlier asynchronous writes and returns failure");
    inserted.clear();
    check(!InsertRoleRecords(manager, 10, 5, [](std::uint32_t id) -> std::expected<NewRole, std::string>
    { if (id == 13) throw std::runtime_error("producer exception"); return new_role(id); }) && inserted.size() == 3 && manager.IsCompleted(),
        "producer exceptions also drain owned outstanding requests");
    for (int kind = 0; kind < 5; ++kind)
    {
        inserted.clear();
        check(!InsertRoleRecords(manager, 10, 1, [kind](std::uint32_t id) -> std::expected<NewRole, std::string>
        {
            auto value = new_role(id);
            if (kind == 0) value.name = std::string(25, 'N');
            if (kind == 1) value.user = std::string("nul\0user", 8);
            if (kind == 2) value.bytes.back() ^= std::byte{1};
            if (kind == 3) value.bytes = blob(id + 1, 0);
            if (kind == 4) value.bytes = blob(id, 0, 20000);
            return value;
        }) && inserted.empty() && manager.IsCompleted(), "invalid strings/blob identity/CRC or compressed capacity cannot enqueue an insert");
    }
    inserted.clear();
    fake_mysql::failExecutions = 2;
    check(!InsertRoleRecords(manager, 10, 1, [](std::uint32_t id) -> std::expected<NewRole, std::string> { return new_role(id); })
        && inserted.empty() && manager.IsCompleted(), "exhausted asynchronous SQL retries are detected even when success callbacks are not invoked");
    check(manager.UnInit(), "role test manager shuts down cleanly");
    closed();
    check(!InsertRoleRecords(manager, 10, 1, [](std::uint32_t id) -> std::expected<NewRole, std::string> { return new_role(id); }), "uninitialized managers reject insertion without retaining captures");
    fake_mysql::preparedHandler = nullptr;
}
}
int main(int argc, char** argv)
{
    if (argc != 3) return EXIT_FAILURE;
    try
    {
        _putenv_s("TZ", "GMT0"); _tzset();
        const auto work = fs::path(argv[1]) / std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        fs::create_directories(work);
        parsing(); lua_lifetimes(); reports(work, fs::path(argv[2])); pipelines(work); closed();
        if (failures) return EXIT_FAILURE;
        std::printf("%d export role checks passed\n", checks.load()); return EXIT_SUCCESS;
    }
    catch (const std::exception& error) { std::fprintf(stderr, "FAIL: %s\n", error.what()); return EXIT_FAILURE; }
}
