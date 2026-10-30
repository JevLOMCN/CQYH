#include <Windows.h>
#include "std_type.h"
#include "file_owner.h"
#include "dir.h"
#include "process_error.h"
#include "lua_script.h"
#include "script_mgr.h"
#include "tabfile.h"
#include "pak_def.h"
#include "hash.h"
#include "zlib.h"

#include <array>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

namespace fs = std::filesystem;
namespace
{
int checks = 0;
int failures = 0;
std::vector<std::string>* captured_logs = nullptr;
void capture_log(INT32, const char* message, INT32 size)
{
    if (captured_logs != nullptr)
        captured_logs->emplace_back(message, static_cast<std::size_t>(size));
}
void check(bool condition, const char* description)
{
    ++checks;
    if (!condition)
    {
        std::fprintf(stderr, "FAIL: %s\n", description);
        ++failures;
    }
}

struct TestDirectory
{
    fs::path path = "baselib-resources-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    TestDirectory()
    {
        if (!fs::create_directory(path))
            throw std::runtime_error("cannot create resource test directory");
    }
    ~TestDirectory()
    {
        CScriptMgr::GetInstance().SetRootPath("");
        g_ClosePakFolder();
        std::error_code error;
        fs::remove_all(path, error);
    }
};

void write_file(const fs::path& path, const std::string& bytes)
{
    std::ofstream file(path, std::ios::binary);
    file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!file)
        throw std::runtime_error("cannot write resource fixture");
}

bool exclusively_openable(const fs::path& path)
{
    const auto handle = CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    CloseHandle(handle);
    return true;
}

std::string read_file(IFile& file)
{
    std::string bytes(file.Size(), '\0');
    const auto read = file.Read(bytes.data(), static_cast<UINT32>(bytes.size()));
    bytes.resize(read);
    return bytes;
}

struct CountedFile final : IFile
{
    int& closed;
    int& destroyed;
    CountedFile(int& close_count, int& destroy_count) : closed(close_count), destroyed(destroy_count) {}
    ~CountedFile() { ++destroyed; }
    BOOL Close() override { ++closed; delete this; return FALSE; }
    UINT32 Read(void*, UINT32) override { return 0; }
    BOOL Seek(INT32, INT32) override { return FALSE; }
    BOOL Eof() override { return TRUE; }
    UINT32 Size() override { return 0; }
};

void test_owners(const fs::path& folder)
{
    int closed = 0;
    int destroyed = 0;
    {
        cqyh::FilePtr owner(new CountedFile(closed, destroyed));
        auto moved = std::move(owner);
        check(!owner && moved, "move transfers IFile ownership");
        moved.reset(new CountedFile(closed, destroyed));
        check(closed == 1 && destroyed == 1, "reset calls Close and the concrete destructor once");
        auto* raw = moved.release();
        check(!moved && closed == 1, "release transfers ownership without closing");
        raw->Close();
    }
    check(closed == 2 && destroyed == 2, "IFile destruction uses Close even when Close returns false");
    {
        cqyh::FilePtr owner(new CountedFile(closed, destroyed));
    }
    check(closed == 3 && destroyed == 3, "scope exit closes IFile exactly once");
    {
        cqyh::FilePtr empty;
    }
    check(closed == 3, "empty ownership needs no cleanup");

    const auto path = folder / "stdio.txt";
    {
        cqyh::CFilePtr file(std::fopen(path.string().c_str(), "wb"));
        check(file != nullptr, "own a C stream");
        if (file)
            std::fputs("stdio", file.get());
        auto moved = std::move(file);
        check(!file && moved, "move transfers C stream ownership");
    }
    check(exclusively_openable(path), "C stream is closed on scope exit");
}

void test_loose_files(const fs::path& folder)
{
    const auto path = folder / "loose.txt";
    write_file(path, "abcdef");
    check(!cqyh::OpenFile(nullptr, "rb") && !cqyh::OpenFile(path.string().c_str(), nullptr), "null factory arguments fail safely");
    check(!cqyh::OpenFile((folder / "missing.txt").string().c_str(), "rb"), "missing file returns an empty owner");
    {
        auto file = cqyh::OpenFile(path.string().c_str(), "rb");
        check(file && file->Size() == 6, "open an ordinary file");
        if (!file)
            return;
        std::array<char, 8> buffer{};
        check(file->Read(buffer.data(), 2) == 2 && std::string(buffer.data(), 2) == "ab", "read a prefix");
        check(file->Size() == 6, "size query succeeds mid-read");
        check(file->Read(buffer.data(), 2) == 2 && std::string(buffer.data(), 2) == "cd", "size preserves the cursor");
        check(file->Seek(-1, SEEK_END) && file->Read(buffer.data(), 1) == 1 && buffer[0] == 'f', "normal file end-relative seek is unchanged");
        check(file->Read(buffer.data(), 1) == 0 && file->Eof(), "normal file reports EOF after reading past the end");
        check(file->Read(nullptr, 1) == 0, "null read destination fails safely");
    }
    check(exclusively_openable(path), "IFile owner closes the underlying ordinary file");
    check(g_GetFileModifyTime(path.string().c_str()) != 0 && g_GetFileCreateTime(path.string().c_str()) != 0, "file timestamps remain available");
    check(g_GetFileModifyTime(nullptr) == 0 && !g_IsFileExist(nullptr), "null metadata queries fail safely");
    check(exclusively_openable(path), "metadata queries release their streams");
}

void write_pack(const fs::path& output, const std::string& name, const std::string& bytes, bool compressed, bool truncated = false)
{
    PAK_FILE_HEADER header{};
    PAK_FILE_INFO info{};
    PAK_CHUNK_INFO chunk{};
    std::string payload = bytes;
    if (compressed)
    {
        uLongf length = compressBound(static_cast<uLong>(bytes.size()));
        payload.resize(length);
        if (compress2(reinterpret_cast<Bytef*>(payload.data()), &length,
                      reinterpret_cast<const Bytef*>(bytes.data()), static_cast<uLong>(bytes.size()), Z_DEFAULT_COMPRESSION) != Z_OK)
            throw std::runtime_error("cannot compress pak fixture");
        payload.resize(length);
        info.nDataChunkCount = 1;
        chunk.dwChunkSize = static_cast<UINT32>(payload.size());
    }
    else
        info.dwFileFlag = FileFlag_NoCompressed;
    hash_file_name(name.c_str(), &info.dwNameHashA, &info.dwNameHashB);
    info.dwOriginalSize = static_cast<UINT32>(bytes.size());
    info.dwCompressedSize = static_cast<UINT32>(payload.size());
    info.dwChunkSize = static_cast<UINT32>(payload.size());
    info.dwCreateTime = 100;
    info.dwModifyTime = 200;
    header.dwTag = PAK_FILE_TAG;
    header.dwVersion = PAK_FILE_VERSION;
    header.dwPackageFlag = PackageFlag_NoHashTable | PackageFlag_NoNameBuffer;
    header.nFileCount = 1;
    header.dwFileInfoOffset = sizeof(header);
    header.dwFileInfoSize = sizeof(info) + (compressed ? sizeof(chunk) : 0);
    header.dwDataOffset = header.dwFileInfoOffset + header.dwFileInfoSize;
    header.dwDataSize = static_cast<UINT32>(payload.size());
    std::ofstream file(output, std::ios::binary);
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    file.write(reinterpret_cast<const char*>(&info), sizeof(info));
    if (compressed)
        file.write(reinterpret_cast<const char*>(&chunk), sizeof(chunk));
    file.write(payload.data(), static_cast<std::streamsize>(truncated ? payload.size() / 2 : payload.size()));
    if (!file)
        throw std::runtime_error("cannot write pak fixture");
}

void test_packed_files(const fs::path& folder)
{
    const auto packs = folder / "paks";
    fs::create_directory(packs);
    const auto shadow = (folder / "shadow.txt").string();
    const auto packed = (folder / "packed.txt").string();
    const auto empty = (folder / "empty.txt").string();
    const auto broken = (folder / "broken.txt").string();
    write_pack(packs / "a.pak", shadow, "packed shadow", false);
    write_pack(packs / "b.pak", packed, "compressed payload", true);
    write_pack(packs / "c.pak", empty, "", false);
    write_pack(packs / "d.pak", broken, "truncated payload", false, true);
    write_file(shadow, "loose shadow");
    check(g_OpenPakFolder(packs.string().c_str()), "open synthetic legacy pak files");
    {
        auto file = cqyh::OpenFile(shadow.c_str(), "rb");
        check(file && read_file(*file) == "loose shadow", "loose files take priority over packages");
    }
    fs::remove(shadow);
    {
        auto file = cqyh::OpenFile(shadow.c_str(), "rb");
        check(file && read_file(*file) == "packed shadow", "missing loose files fall back to packages");
    }
    {
        auto file = cqyh::OpenFile(packed.c_str(), "rb");
        check(file && file->Size() == 18, "open a compressed packed file");
        if (file)
        {
            std::array<char, 32> buffer{};
            check(file->Read(buffer.data(), (std::numeric_limits<UINT32>::max)()) == 18 &&
                  std::string(buffer.data(), 18) == "compressed payload", "oversized packed read is clamped without overflow");
            check(file->Eof() && file->Read(buffer.data(), 1) == 0, "packed reads stop at EOF");
            check(file->Seek(0, SEEK_END) && file->Read(buffer.data(), 1) == 1 && buffer[0] == 'd', "preserve legacy packed END,0 last-byte convention");
            check(file->Seek(-1, SEEK_END) && file->Eof(), "legacy packed END,-1 reaches EOF");
            check(!file->Seek(-1, SEEK_SET) && !file->Seek(1, SEEK_CUR), "invalid seeks do not wrap the packed cursor");
            check(file->Seek(0, SEEK_SET) && read_file(*file) == "compressed payload", "packed reader remains usable after rejected seeks");
        }
    }
    {
        auto file = cqyh::OpenFile(empty.c_str(), "rb");
        check(file && file->Size() == 0 && file->Eof(), "empty packed files retain their behavior");
    }
    check(g_GetFileCreateTime(packed.c_str()) == 100 && g_GetFileModifyTime(packed.c_str()) == 200, "packed timestamps remain available");
    check(g_IsFileExist(packed.c_str()), "packed existence lookup remains available");
    for (int i = 0; i < 32; ++i)
        check(!cqyh::OpenFile(broken.c_str(), "rb"), "failed packed initialization returns no partially initialized file");
    check(g_ClosePakFolder(), "close package streams");
    check(exclusively_openable(packs / "a.pak"), "package streams are released on close");
}

void test_logging(const fs::path& folder)
{
    std::vector<std::string> messages;
    captured_logs = &messages;
    CLog::GetInstance().SetCustomOutput(capture_log);
    CLog::GetInstance().SetOutputMask(lotCustom);
    g_GetFileModifyTime(nullptr);
    check(!messages.empty(), "invalid metadata requests still produce diagnostics");
    messages.clear();
    check(!cqyh::OpenFile((folder / "missing.txt").string().c_str(), "rb") && messages.empty(),
          "ordinary loose-file misses remain quiet for package fallback");
    g_OpenPakFolder((folder / "paks").string().c_str());
    messages.clear();
    const auto broken = (folder / "broken.txt").string();
    check(!cqyh::OpenFile(broken.c_str(), "rb"), "failed packed open still fails with logging enabled");
    bool named_failure = false;
    for (const auto& message : messages)
        named_failure = named_failure || message.find(broken) != std::string::npos;
    check(named_failure, "packed-file failure diagnostics retain the file name");
    g_ClosePakFolder();
    messages.clear();
    struct RejectEntry
    {
        bool RequireDirectory() const { return false; }
        bool operator()(const char*) { return false; }
    } reject;
    check(!g_DirectoryTraverse(folder.string().c_str(), reject) && !messages.empty(),
          "directory callback failures retain their diagnostic");
    messages.clear();
    const auto deleted = folder / "deleted";
    fs::create_directory(deleted);
    check(g_DeleteDirectory(deleted.string().c_str()), "directory removal succeeds with logging enabled");
    bool deletion_logged = false;
    for (const auto& message : messages)
        deletion_logged = deletion_logged || message.find("Deleting directory") != std::string::npos;
    check(deletion_logged, "synchronous directory removal retains an operation log");
    CLog::GetInstance().SetOutputMask(lotNone);
    CLog::GetInstance().SetCustomOutput(nullptr);
    captured_logs = nullptr;
}

void test_tables(const fs::path& folder)
{
    static_assert(!std::is_copy_constructible_v<CTabFile> && !std::is_copy_assignable_v<CTabFile>);
    const std::string text = "ID\tName\n1\tcaf\xC3\xA9\n";
    const auto utf8 = folder / "utf8.txt";
    const auto utf16 = folder / "utf16.txt";
    const auto invalid = folder / "invalid.txt";
    write_file(utf8, "\xEF\xBB\xBF" + text);
    const std::u16string wide = u"ID\tName\n1\tcaf\u00E9\n";
    std::string encoded = "\xFF\xFE";
    encoded.append(reinterpret_cast<const char*>(wide.data()), wide.size() * sizeof(char16_t));
    write_file(utf16, encoded);
    write_file(invalid, std::string("\xFF\xFE\x00\xD8", 4));
    for (const auto& path : {utf8, utf16})
    {
        CTabFile table;
        check(table.Init(path.string().c_str(), 0), "load UTF-8 or UTF-16 table");
        check(table.GetRowCount() == 1 && table.GetColCount() == 2, "preserve table dimensions");
        char name[32]{};
        check(table.ReadData("Name", 0, name, sizeof(name)) && std::string(name) == "caf\xC3\xA9", "table values remain UTF-8");
        check(table.GetCRC() == crc32(0, reinterpret_cast<const Bytef*>(text.data()), static_cast<uInt>(text.size())), "CRC covers converted bytes without a sentinel");
        check(exclusively_openable(path), "table releases its input stream after loading");
    }
    CTabFile reusable;
    for (int i = 0; i < 16; ++i)
    {
        check(!reusable.Init(invalid.string().c_str(), 0), "failed UTF-16 conversion is reported");
        check(exclusively_openable(invalid), "failed conversion closes the input stream");
    }
    check(reusable.Init(utf8.string().c_str(), 0), "table can be reused after failed conversion");
    check(!reusable.Init(utf16.string().c_str(), 0) && reusable.GetRowCount() == 1,
          "reinitializing a live table preserves its existing owned data");
    check(reusable.UnInit() && reusable.GetRowCount() == 0 && reusable.GetColCount() == 0 && reusable.GetCRC() == 0,
          "explicit table cleanup releases data and resets metadata");
    check(reusable.Init(utf16.string().c_str(), 0), "table can be reused after explicit cleanup");
    const auto quote = folder / "quote.txt";
    write_file(quote, "A\n\"x\"");
    CTabFile trailing_quote;
    check(trailing_quote.Init(quote.string().c_str(), 0), "parser lookahead stays within its allocated buffer");
    const auto malformed = folder / "malformed.txt";
    for (const auto* data : {"A\tB\n1\n", "A\tB\n1\t2\t3\n", "A\tB\n1\n2\t3\t4\n"})
    {
        write_file(malformed, data);
        CTabFile table;
        check(!table.Init(malformed.string().c_str(), 0) && table.GetRowCount() == 0,
              "malformed row widths cannot shift later columns or leave partial offsets");
        check(exclusively_openable(malformed), "malformed table cleanup releases the input file");
    }
    std::string sparse = "A";
    for (int column = 0; column < 1024; ++column)
        sparse += "\t" + std::to_string(column);
    sparse += "\n";
    sparse.append(1024, '\n');
    write_file(malformed, sparse);
    CTabFile sparseTable;
    check(!sparseTable.Init(malformed.string().c_str(), 0), "sparse dimensions are rejected before allocating their row-column product");
}

void test_table_numbers(const fs::path& folder)
{
    const auto path = folder / "numbers.txt";
    write_file(path, "Number\tText\nServer\tServer\n17\tfallback\n"
                     "  +42tail\tvalue\n-2147483648\tlow\n2147483647\thigh\n"
                     "\t\nword\tplain\n2147483648\tover\n-2147483649\tunder\n+-3\tbad\n"
                     "2620000000\tlevel-limit\n");
    CTabFile table;
    check(table.Init(path.string().c_str(), 1), "load numeric-prefix and default-row fixture");
    const auto* numberColumn = table.GetColName(0);
    const auto* textColumn = table.GetColName(1);
    check(numberColumn && textColumn && std::string_view{numberColumn} == "Number" &&
          std::string_view{textColumn} == "Text", "column names use the physical header despite skipped data rows");
    INT32 number = 0;
    check(table.ReadData("Number", 1, number) && number == 42, "table numeric fields retain whitespace, plus signs and trailing text");
    check(table.ReadData(0, 2, number) && number == (std::numeric_limits<INT32>::min)(), "table accepts the minimum signed value");
    check(table.ReadData(0, 3, number) && number == (std::numeric_limits<INT32>::max)(), "table accepts the maximum signed value");
    check(table.ReadData(0, 4, number) && number == 17, "empty numeric field uses the default row");
    check(table.ReadData(0, 5, number) && number == 0, "legacy nonnumeric field remains zero");
    check(table.ReadData(0, 6, number) && number == (std::numeric_limits<INT32>::max)(), "legacy positive overflow retains the Windows CRT limit");
    check(table.ReadData(0, 7, number) && number == (std::numeric_limits<INT32>::min)(), "legacy negative overflow retains the Windows CRT limit");
    check(table.ReadData(0, 8, number) && number == 0, "malformed repeated signs retain the legacy zero result");
    check(table.ReadData(0, 9, number) && number == (std::numeric_limits<INT32>::max)(), "shipped level-60 MaxExp sentinel remains compatible");

    const std::array boundedName{'N', 'u', 'm', 'b', 'e', 'r', 'x', 'x'};
    check(table.ReadData(std::string_view{boundedName.data(), 6}, 1, number) && number == 42,
          "column lookup accepts a bounded nonterminated string view");
    check(table.ReadData(std::string{"Number"}, 1, number) && number == 42,
          "owning standard strings use the borrowed-view overload");
    char value[16]{};
    check(table.ReadData(std::string_view{"TextSuffix", 4}, 4, value, sizeof(value)) && std::string(value) == "fallback",
          "string views retain default-string lookup");
    std::strcpy(value, "keep");
    check(table.ReadData("Text", 1, value, 0) && std::string(value) == "keep", "zero-capacity table copy retains its no-op contract");
    check(!table.ReadData("Text", 1, value, -1) && std::string(value) == "keep", "negative capacity is rejected without writing");
    check(!table.ReadData("Text", 1, nullptr, 0), "null output buffer is rejected");
    number = 99;
    check(!table.ReadData(nullptr, 0, number) && number == 99, "null column name is rejected without changing output");
    check(!table.ReadData(std::string_view{"Missing"}, 0, number) && number == 99, "missing view column preserves output");
    check(!table.ReadData(-1, 0, number) && number == 99, "invalid column index preserves output");
    check(!table.ReadData(0, (std::numeric_limits<INT32>::max)(), number) && number == 99,
          "row adjustment cannot overflow a signed index");

    const auto emptyPath = folder / "empty-numbers.txt";
    write_file(emptyPath, "Number\tText\n\t\n");
    CTabFile empty;
    check(empty.Init(emptyPath.string().c_str(), 0), "load a short table without a default row");
    check(empty.ReadData("Number", 0, number) && number == 0, "empty short-table field avoids a missing default row");
    check(empty.ReadData("Text", 0, value, sizeof(value)) && std::string(value).empty(),
          "short-table string fallback stays within its offset table");
}

void test_scripts(const fs::path& folder)
{
    const auto main = folder / "main.lua";
    const auto included = folder / "included.lua";
    const auto invalid = folder / "invalid.lua";
    const auto tiny = folder / "tiny.lua";
    write_file(main, "Include(\"included.lua\")\nresult = included_value + 1\n");
    write_file(included, "\xEF\xBB\xBF" "included_value = 41\n");
    write_file(invalid, "this is not valid lua syntax !\n");
    write_file(tiny, "\n");
    CScriptMgr::GetInstance().SetRootPath(folder.string().c_str());
    auto exercise = [&]()
    {
        CLuaScript script;
        bool success = script.Init() && script.LoadFromFile(main.string().c_str());
        if (success)
        {
            lua_getglobal(script.GetLuaState(), "result");
            success = lua_tointeger(script.GetLuaState(), -1) == 42;
            lua_pop(script.GetLuaState(), 1);
        }
        return success;
    };
    check(exercise(), "Lua loads ordinary files, included files and UTF-8 BOMs");
    {
        CLuaScript script;
        check(script.Init() && script.LoadFromFile(tiny.string().c_str()), "one-byte Lua files do not require a four-byte BOM read");
    }
    {
        CLuaScript script;
        check(script.Init() && !script.LoadFromFile(invalid.string().c_str()), "Lua syntax failure is reported");
        check(exclusively_openable(invalid), "Lua syntax failure releases its input stream");
    }
    DWORD before_handles = 0;
    DWORD after_handles = 0;
    GetProcessHandleCount(GetCurrentProcess(), &before_handles);
#ifdef _DEBUG
    _CrtMemState before{};
    _CrtMemState after{};
    _CrtMemCheckpoint(&before);
#endif
    for (int i = 0; i < 32; ++i)
        check(exercise(), "repeated Lua loads retain include behavior");
#ifdef _DEBUG
    _CrtMemCheckpoint(&after);
    check(after.lCounts[_NORMAL_BLOCK] == before.lCounts[_NORMAL_BLOCK] &&
          after.lSizes[_NORMAL_BLOCK] == before.lSizes[_NORMAL_BLOCK], "successful CRC scans retain no allocated file buffers");
#endif
    GetProcessHandleCount(GetCurrentProcess(), &after_handles);
    check(after_handles == before_handles, "repeated Lua loads retain no file handles");
    check(exclusively_openable(main) && exclusively_openable(included), "Lua releases main and included streams");
}
}

int main()
{
    CLog::GetInstance().Init("test_baselib_resources", lltAll, lotNone, FALSE, FALSE, FALSE);
    TestDirectory directory;
    test_owners(directory.path);
    test_loose_files(directory.path);
    test_packed_files(directory.path);
    test_logging(directory.path);
    test_tables(directory.path);
    test_table_numbers(directory.path);
    test_scripts(directory.path);
    std::printf("%d resource checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
