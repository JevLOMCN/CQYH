#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "std_type.h"
#include "process_error.h"
#include "sql_mgr.h"
#include "fake_mysql_client.h"
#include "guild.h"
#include "define.h"
#include "guild_def.h"
#include "../../src/manageserver/guild/guild_db_def_v_5.h"
#include "zlib.h"

namespace
{
using namespace GUILD_DEF_VER_5;
namespace fs = std::filesystem;
using Buffer = std::vector<std::byte>;
using fake_mysql::Cell;
using fake_mysql::Row;
using fake_mysql::Rows;
int checks = 0;
int failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
template<class T> Buffer bytes(const T& value)
{
    Buffer result(sizeof(value));
    std::memcpy(result.data(), &value, sizeof(value));
    return result;
}
template<class T> Cell number(T value)
{
    Cell result(sizeof(value));
    std::memcpy(result.data(), &value, sizeof(value));
    return result;
}
Cell text(std::string_view value) { return {value.begin(), value.end()}; }
void append(Buffer& target, const Buffer& value) { target.insert(target.end(), value.begin(), value.end()); }
void crc(Buffer& value)
{
    const auto checksum = crc32(0, reinterpret_cast<const Bytef*>(value.data()) + 8, static_cast<uInt>(value.size() - 8));
    std::memcpy(value.data() + 4, &checksum, sizeof(checksum));
}
using Blocks = std::array<Buffer, gbtTotal>;
Blocks valid_blocks(std::uint8_t level = 3)
{
    GUILD_DB_BASE base{};
    base.nMoney = 123; base.nWood = 456; base.nHerb = 789; base.nCrystal = 987; base.nSilver = 654;
    GUILD_DB_BUILDING building;
    std::memset(&building, 0, sizeof(building));
    building.byBuildingNum = 1;
    building.wBuildingBufferLength = sizeof(GUILD_DB_BUILDING_DATA);
    GUILD_DB_BUILDING_DATA main{};
    main.byType = GUILD_B_MAIN; main.byLevel = level;
    Blocks result;
    result[gbtBase] = bytes(base);
    result[gbtBuilding] = bytes(building);
    append(result[gbtBuilding], bytes(main));
    return result;
}
Buffer blob(Blocks blocks, std::uint32_t id = 17)
{
    GUILD_DB_DATA header;
    std::memset(&header, 0, sizeof(header));
    header.dwGuildID = id;
    header.dwVersion = 5;
    Buffer payload;
    for (std::size_t index = 0; index < blocks.size(); ++index)
    {
        header.BlockTable[index] = {static_cast<UINT8>(index), static_cast<UINT32>(blocks[index].size()), static_cast<UINT32>(payload.size())};
        append(payload, blocks[index]);
    }
    header.dwLen = static_cast<UINT32>(sizeof(header) + payload.size());
    Buffer result = bytes(header);
    append(result, payload);
    crc(result);
    return result;
}
Buffer modify(Buffer value, auto action)
{
    GUILD_DB_DATA header;
    std::memcpy(&header, value.data(), sizeof(header));
    action(header);
    std::memcpy(value.data(), &header, sizeof(header));
    crc(value);
    return value;
}
Cell compress_blob(const Buffer& value)
{
    Cell result(compressBound(static_cast<uLong>(value.size())));
    uLongf length = static_cast<uLongf>(result.size());
    if (compress(result.data(), &length, reinterpret_cast<const Bytef*>(value.data()), static_cast<uLong>(value.size())) != Z_OK)
        throw std::runtime_error("cannot compress fixture");
    result.resize(length);
    return result;
}
void parsing()
{
    using cqyh::exportdb::ParseGuildBlob;
    const auto valid = blob(valid_blocks());
    const auto parsed = ParseGuildBlob(valid);
    check(parsed && parsed->level == 3 && parsed->money == 123 && parsed->wood == 456
        && parsed->herb == 789 && parsed->crystal == 987 && parsed->silver == 654, "native version-5 base/building data decodes correctly");
    for (std::size_t length = 0; length < valid.size(); ++length)
        check(!ParseGuildBlob(std::span(valid).first(length)), "every truncated guild blob prefix is rejected");
    for (std::size_t offset = 0; offset < valid.size(); ++offset)
    {
        auto corrupt = valid;
        corrupt[offset] ^= std::byte{1};
        check(!ParseGuildBlob(corrupt), "single-byte corruption cannot pass the length/CRC checks");
    }
    const auto empty = ParseGuildBlob(blob({}));
    check(empty && empty->level == 0 && empty->money == 0, "missing resource/building blocks produce deterministic zero fields");
    check(ParseGuildBlob(modify(valid, [](auto& header) { header.dwVersion = 4; })).has_value(),
          "older-version records retain the original structurally compatible loading path");
    auto padded = valid;
    padded.resize(padded.size() + 10);
    check(ParseGuildBlob(padded).has_value(), "enclosing SQL blob padding does not alter its declared guild record");
    for (const auto mutation : {0, 1, 2, 3, 4})
    {
        const auto bad = modify(valid, [mutation](auto& header)
        {
            if (mutation == 0) header.dwLen = 7;
            if (mutation == 1) header.dwLen = 60000;
            if (mutation == 2) header.BlockTable[0].dwStartOffset = std::numeric_limits<UINT32>::max();
            if (mutation == 3) header.BlockTable[0].dwBlockSize = std::numeric_limits<UINT32>::max();
            if (mutation == 4) header.BlockTable[0].byBlockType = 255;
        });
        check(!ParseGuildBlob(bad), "invalid lengths, wrapped ranges and unknown block types fail after a valid CRC");
    }
    auto blocks = valid_blocks();
    blocks[gbtBase].pop_back();
    check(!ParseGuildBlob(blob(blocks)), "short fixed guild base block is rejected");
    for (const int kind : {gbtWelfare, gbtBuilding, gbtDiplomat})
    {
        blocks = valid_blocks();
        blocks[kind] = Buffer(1);
        check(!ParseGuildBlob(blob(blocks)), "short variable-block header is rejected before reading counters");
    }
    GUILD_DB_WELFARE welfare;
    std::memset(&welfare, 0, sizeof(welfare));
    blocks = valid_blocks();
    blocks[gbtWelfare] = bytes(welfare);
    check(ParseGuildBlob(blob(blocks)).has_value(), "empty welfare upgrade list is valid");
    welfare.byUpgradeWelfareNum = 1;
    blocks[gbtWelfare] = bytes(welfare);
    check(!ParseGuildBlob(blob(blocks)), "welfare list size must match its counter");
    GUILD_DB_WELFARE_UPGRADE upgrade{};
    append(blocks[gbtWelfare], bytes(upgrade));
    check(ParseGuildBlob(blob(blocks)).has_value(), "complete welfare upgrades are accepted");
    for (int mutation = 0; mutation < 4; ++mutation)
    {
        blocks = valid_blocks();
        GUILD_DB_BUILDING header;
        std::memcpy(&header, blocks[gbtBuilding].data(), sizeof(header));
        if (mutation == 0) header.byBuildingNum = 255;
        if (mutation == 1) header.wBuildingBufferLength = 65535;
        if (mutation == 2) header.byUpgradeBuildingNum = 1;
        if (mutation == 3) header.wUpgradeBufferLength = 65535;
        std::memcpy(blocks[gbtBuilding].data(), &header, sizeof(header));
        check(!ParseGuildBlob(blob(blocks)), "building and upgrade counters cannot escape their declared lists");
    }
    GUILD_DB_DIPLOMAT diplomat;
    std::memset(&diplomat, 0, sizeof(diplomat));
    blocks = valid_blocks();
    blocks[gbtDiplomat] = bytes(diplomat);
    check(ParseGuildBlob(blob(blocks)).has_value(), "empty diplomacy list is valid");
    diplomat.byAllianceNum = GUILD_MAX_ALLIANCE_NUM + 1;
    blocks[gbtDiplomat] = bytes(diplomat);
    check(!ParseGuildBlob(blob(blocks)), "out-of-range diplomacy count is rejected");
    diplomat.byAllianceNum = 1;
    blocks[gbtDiplomat] = bytes(diplomat);
    GUILD_DB_DIPLOMAT_DATA alliance{};
    append(blocks[gbtDiplomat], bytes(alliance));
    check(ParseGuildBlob(blob(blocks)).has_value(), "complete diplomacy entries are accepted");
    diplomat.byEnemyNum = GUILD_MAX_ENEMY_NUM + 1;
    std::memcpy(blocks[gbtDiplomat].data(), &diplomat, sizeof(diplomat));
    check(!ParseGuildBlob(blob(blocks)), "out-of-range enemy count is rejected");
    blocks = valid_blocks();
    for (int kind = gbtDungeon; kind < gbtTotal; ++kind)
        blocks[kind] = Buffer(7, std::byte{1});
    check(ParseGuildBlob(blob(blocks)).has_value(), "uninterpreted legacy blocks retain their enclosing-range contract");
}
struct Scenario
{
    Rows guilds;
    Rows members;
    std::vector<std::pair<std::int64_t, Rows>> counts;
    std::size_t next = 0;
    int loads = 0;
} scenario;
Row guild(std::int64_t id, std::int32_t group, std::int64_t leader, std::int64_t time,
          std::string_view name, std::string_view creator, const Buffer& data,
          std::string_view created = "2020-01-01 02:03:04", std::string_view deleted = "0000-00-00 00:00:00")
{
    return {text(created), text(deleted), number(id), number(std::int32_t{5}), text(name), number(group),
        number(leader), text(creator), number(time), text("notice"), text("announce"), compress_blob(data)};
}
Row member(std::int64_t role, std::int64_t guild_id, std::string_view guild_name,
           std::string_view role_name, std::string_view user)
{
    return {number(role), number(guild_id), text(guild_name), text(role_name), text(user)};
}
void binding(const MYSQL_BIND& value, enum_field_types type, unsigned long length)
{
    check(value.buffer && value.buffer_type == type && value.buffer_length == length && !value.is_unsigned,
          "production native binding type, width and signedness match the offline schema");
}
Rows query(std::string_view sql, std::span<const MYSQL_BIND> params, MYSQL_BIND* results)
{
    if (sql.find("count(*)") != sql.npos)
    {
        check(params.size() == 1, "membership query has one role-independent guild parameter");
        if (params.size() != 1 || scenario.next >= scenario.counts.size())
        {
            check(false, "membership query count/order matches the loaded guilds");
            return {};
        }
        binding(params[0], MYSQL_TYPE_LONGLONG, 8);
        binding(results[0], MYSQL_TYPE_LONGLONG, 8);
        std::int64_t id;
        std::memcpy(&id, params[0].buffer, sizeof(id));
        const auto& expected = scenario.counts[scenario.next++];
        check(id == expected.first, "membership parameter preserves the complete guild ID and input order");
        return expected.second;
    }
    check(params.empty(), "guild/member selection has no bound parameters");
    if (sql.find("a1.RoleID") != sql.npos)
    {
        constexpr std::array types{MYSQL_TYPE_LONGLONG, MYSQL_TYPE_LONGLONG, MYSQL_TYPE_STRING, MYSQL_TYPE_STRING, MYSQL_TYPE_STRING};
        constexpr std::array<unsigned long, 5> sizes{8, 8, 25, 25, 65};
        for (std::size_t i = 0; i < sizes.size(); ++i) binding(results[i], types[i], sizes[i]);
        return scenario.members;
    }
    check(sql.find("MainData") != sql.npos, "offline export executes only the two expected read-only selections");
    constexpr std::array types{MYSQL_TYPE_STRING, MYSQL_TYPE_STRING, MYSQL_TYPE_LONGLONG, MYSQL_TYPE_LONG,
        MYSQL_TYPE_STRING, MYSQL_TYPE_LONG, MYSQL_TYPE_LONGLONG, MYSQL_TYPE_STRING, MYSQL_TYPE_LONGLONG,
        MYSQL_TYPE_STRING, MYSQL_TYPE_STRING, MYSQL_TYPE_BLOB};
    constexpr std::array<unsigned long, 12> sizes{21, 21, 8, 4, 25, 4, 8, 25, 8, 101, 201, 60000};
    for (std::size_t i = 0; i < sizes.size(); ++i) binding(results[i], types[i], sizes[i]);
    ++scenario.loads;
    return scenario.guilds;
}
std::string read(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), {}};
}
void closed()
{
    check(fake_mysql::connections == 0 && fake_mysql::statements == 0 && fake_mysql::threads == 0
        && fake_mysql::results == 0 && !fake_mysql::ready && fake_mysql::orderErrors == 0 && fake_mysql::scriptErrors == 0,
        "all native SQL resources are released in dependency order");
}
void exports(const fs::path& work, const fs::path& fixtures)
{
    using namespace cqyh::exportdb;
    const std::string chinese = "\xe5\x90\x8d\xe5\xad\x97";
    scenario = {{guild(17, 2, 101, 1577844184, "Alpha%s", "Creator", blob(valid_blocks())),
                 guild(23, 0, 202, 1580702706, chinese, "100%", blob(valid_blocks(6), 23), "2020-02-03 04:05:06", "2021-03-04 05:06:07")},
        {member(101, 17, "Alpha%s", "Alice", "user-a"),
         member(102, 17, "Alpha%s", "100%name", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"),
         member(202, 23, chinese, "\xe8\xa7\x92\xe8\x89\xb2", "utf8-user")},
        {{17, {{number(std::int64_t{2})}}}, {23, {{number(std::int64_t{3})}}}}};
    fake_mysql::preparedHandler = query;
    CSqlMgr manager;
    check(manager.Init("fake", "", "", "fake", "utf8", 0, g_SqlStmtList, mysqlTotal, 1), "initialize export against the production SQL metadata and fake client");
    const auto guild_path = work / L"\u540d\u5b57-guild.txt";
    const auto members_path = work / "members.txt";
    check(ExportGuilds(manager, guild_path) && ExportMembers(manager, members_path), "guild/member exports complete successfully");
    check(read(guild_path) == read(fixtures / "export-original-guild.txt"), "guild report matches the original bytes and row order");
    check(read(members_path) == read(fixtures / "export-original-members.txt"), "member report matches original UTF-8/percent names and 32-byte username limit");
    scenario.next = 0;
    check(ExportGuilds(manager, guild_path) && read(guild_path) == read(fixtures / "export-original-guild.txt"), "repeated export replaces output without accumulating global records");
    constexpr std::int64_t high = 4294967295;
    scenario = {{guild(high, 255, high, high, std::string(25, 'X'), "creator", blob(valid_blocks(6)))},
        {member(high, high, std::string(25, 'G'), std::string(25, 'R'), std::string(65, 'U'))},
        {{high, {{number(std::int64_t{2147483648})}}}}};
    check(ExportGuilds(manager, guild_path) && read(guild_path).find("4294967295\t" + std::string(24, 'X') + "\t6\t255\t4294967295\t") != std::string::npos
        && read(guild_path).ends_with("2147483648\n"), "unsigned-domain IDs and wide membership counts are not signed/truncated during native conversion");
    check(ExportMembers(manager, members_path) && read(members_path).find("4294967295\t4294967295\t" + std::string(24, 'G') + "\t" + std::string(24, 'R') + "\t" + std::string(32, 'U') + "\n") != std::string::npos,
          "nonterminated native result fields are bounded before formatting");
    const auto saved = read(guild_path);
    scenario = {{guild(17, 2, 101, 1, "first", "creator", blob(valid_blocks())),
                 guild(23, 3, 202, 2, "second", "creator", blob(valid_blocks(6)))}, {},
                 {{17, {{number(std::int64_t{2})}}}, {23, {{number(std::int64_t{3})}}}}};
    fake_mysql::failFetchAfterRow = true;
    check(ExportGuilds(manager, guild_path) && scenario.loads == 2 && scenario.next == 2,
          "fetch retry discards partial guild rows before their records/count queries are published");
    const auto retry_report = read(guild_path);
    check(std::count(retry_report.begin(), retry_report.end(), '\n') == 3,
          "retried guild selection emits each complete guild once");
    { std::ofstream output(guild_path, std::ios::binary); output << saved; }
    scenario = {};
    fake_mysql::failExecutions = 2;
    check(!ExportGuilds(manager, guild_path) && read(guild_path) == saved,
          "exhausted SQL retries fail without replacing the previous report");
    for (const auto bad_id : {std::int64_t{-1}, std::int64_t{4294967296}})
    {
        scenario = {{guild(bad_id, 0, 1, 1, "bad", "bad", blob(valid_blocks()))}, {}, {}};
        check(!ExportGuilds(manager, guild_path) && read(guild_path) == saved, "invalid guild IDs fail without replacing existing output");
        scenario = {{}, {member(bad_id, 1, "bad", "bad", "bad")}, {}};
        check(!ExportMembers(manager, members_path), "invalid member IDs fail without silent wrapping");
    }
    auto invalid = blob(valid_blocks());
    invalid.back() ^= std::byte{1};
    scenario = {{guild(17, 0, 1, 1, "bad", "bad", invalid)}, {}, {}};
    check(!ExportGuilds(manager, guild_path) && read(guild_path) == saved, "invalid compressed guild contents cannot publish a partial report");
    scenario = {{guild(17, 0, 1, 1, "bad", "bad", blob(valid_blocks()))}, {}, {}};
    scenario.guilds[0].back() = Cell{0, 1, 2, 3};
    check(!ExportGuilds(manager, guild_path) && read(guild_path) == saved,
          "malformed compressed data fails before guild parsing/output");
    for (Rows counts : {Rows{}, Rows{{number(std::int64_t{-1})}}})
    {
        scenario = {{guild(17, 0, 1, 1, "good", "good", blob(valid_blocks()))}, {}, {{17, std::move(counts)}}};
        check(!ExportGuilds(manager, guild_path) && read(guild_path) == saved, "missing/negative membership count fails before output is replaced");
    }
    scenario = {};
    check(!ExportGuilds(manager, work) && !ExportMembers(manager, work), "output-open errors reach callers and release record storage");
    check(ExportGuilds(manager, guild_path) && read(guild_path) == "GuildID\tGuildName\tGuildLevel\tGroupID\tLeaderID\tCreaterName\tCreateTime\tDeleteTime\tMemberCount\n",
          "empty guild selection emits only its header");
    check(ExportMembers(manager, members_path) && read(members_path) == "RoleID\tGuildID\tGuildName\tRoleName\tUserName\n", "empty member selection emits only its header");
    const auto handle = CreateFileW(members_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    check(handle != INVALID_HANDLE_VALUE, "reserve an output sharing failure fixture");
    if (handle != INVALID_HANDLE_VALUE)
    {
        check(!ExportMembers(manager, members_path), "output sharing violations propagate failure");
        CloseHandle(handle);
    }
    check(manager.UnInit(), "export fixture manager shuts down");
    closed();
    check(!ExportGuilds(manager, guild_path) && !ExportMembers(manager, members_path), "uninitialized SQL managers cannot report successful exports");
    fake_mysql::preparedHandler = nullptr;
}
}
int main(int argc, char** argv)
{
    if (argc != 3) return EXIT_FAILURE;
    try
    {
        const fs::path work = fs::path(argv[1]) / std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        fs::create_directories(work);
        parsing();
        exports(work, fs::path(argv[2]));
        closed();
        if (failures) return EXIT_FAILURE;
        std::printf("%d exportdb checks passed\n", checks);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "FAIL: %s\n", error.what());
        return EXIT_FAILURE;
    }
}
