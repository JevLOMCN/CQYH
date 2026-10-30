// Regression tests for the public baselib utilities, including the standalone tools' copy.
#include "dir.h"
#include "string_ex.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

namespace
{
int failures = 0;
int checks = 0;

void check(bool condition, const char* description)
{
    ++checks;
    if (!condition)
    {
        std::fprintf(stderr, "FAIL: %s\n", description);
        ++failures;
    }
}

void test_strings()
{
    // Check every capacity, including zero and exact fits, with guards on both sides.
    for (const std::string_view source : {"", "a", "12345678"})
    {
        for (std::size_t capacity = 0; capacity <= 10; ++capacity)
        {
            std::array<char, 12> storage;
            storage.fill('#');
            char* destination = storage.data() + 1;
            const auto copied = strxcpy(destination, source.data(), capacity);
            const auto expected = capacity == 0 ? 0 : (std::min)(source.size(), capacity - 1);
            check(copied == expected, "copy returns the stored length");
            check(storage.front() == '#' && storage[capacity + 1] == '#', "copy respects buffer bounds");
            if (capacity == 0)
                check(destination[0] == '#', "zero capacity copy leaves the destination alone");
            else
                check(std::string_view(destination) == source.substr(0, expected), "copy truncates and terminates");
        }
    }

    char destination[8] = "abc";
    check(strxcpy(nullptr, "a", 1) == 0, "copy accepts a null destination");
    check(strxcpy(destination, nullptr, sizeof(destination)) == 0 && std::strcmp(destination, "abc") == 0,
          "copy with null source leaves the destination alone");
    const char bounded_source[2] = {'a', 'b'};
    char bounded_copy[3] = {};
    check(strxcpy(bounded_copy, bounded_source, sizeof(bounded_copy)) == 2 && std::strcmp(bounded_copy, "ab") == 0,
          "truncated copy does not scan past the needed source prefix");
    char bounded_append[4] = "x";
    check(strxcat(bounded_append, bounded_source, sizeof(bounded_append)) == 3 && std::strcmp(bounded_append, "xab") == 0,
          "truncated append does not scan past the needed source prefix");
    char alias[8] = "abcd";
    check(strxcpy(alias, alias, sizeof(alias)) == 4 && std::strcmp(alias, "abcd") == 0, "self-copy preserves the string");
    check(strxcpy(alias, alias + 1, sizeof(alias)) == 3 && std::strcmp(alias, "bcd") == 0, "copy can shift a suffix left");
    check(strxcpy(alias, alias, 3) == 2 && std::strcmp(alias, "bc") == 0, "self-copy truncates within capacity");

    for (const std::string_view initial : {"", "abc", "1234567"})
    {
        for (const std::string_view suffix : {"", "z", "12345678"})
        {
            for (std::size_t capacity = initial.size() + 1; capacity <= 10; ++capacity)
            {
                std::array<char, 12> storage;
                storage.fill('#');
                char* buffer = storage.data() + 1;
                std::memcpy(buffer, initial.data(), initial.size());
                buffer[initial.size()] = '\0';
                const auto length = strxcat(buffer, suffix.data(), capacity);
                const auto expected = (std::string(initial) + std::string(suffix)).substr(0, capacity - 1);
                check(length == expected.size() && buffer == expected, "append returns the final truncated length");
                check(storage.front() == '#' && storage[capacity + 1] == '#', "append respects buffer bounds");
            }
        }
    }

    std::array<char, 5> unterminated{'a', 'b', 'c', 'd', '#'};
    check(strxcat(unterminated.data(), "x", 4) == 3, "unterminated append reports the repaired length");
    check(unterminated == std::array<char, 5>{'a', 'b', 'c', '\0', '#'}, "unterminated append stays in bounds");
    check(strxcat(nullptr, "x", 1) == 0, "append accepts a null destination");
    check(strxcat(destination, nullptr, sizeof(destination)) == 0 && std::strcmp(destination, "abc") == 0,
          "append with null source leaves the destination alone");
    check(strxcat(destination, "x", 0) == 0 && std::strcmp(destination, "abc") == 0,
          "zero capacity append leaves the destination alone");
    char single = 'x';
    check(strxcat(&single, "abc", 1) == 0 && single == '\0', "one byte append terminates in bounds");

    char text[] = ",one,,two,";
    char* cursor = text;
    for (const std::string_view expected : {"", "one", "", "two"})
    {
        const char* token = SplitStr(cursor, ',');
        check(token != nullptr && token == expected, "split preserves empty tokens");
    }
    char* remaining = cursor;
    check(SplitStr(cursor, ',') == nullptr && cursor == remaining && *cursor == '\0', "split leaves the last token alone");
    char final_token[] = "last";
    cursor = final_token;
    check(SplitStr(cursor, ',') == nullptr && cursor == final_token && std::strcmp(cursor, "last") == 0,
          "split without a separator leaves the input alone");
    check(SplitStr(cursor, '\0') == nullptr && cursor == final_token, "split does not consume the terminator");
    cursor = nullptr;
    check(SplitStr(cursor, ',') == nullptr, "split accepts a null cursor");

}

class TestDirectory
{
public:
    fs::path path;

    TestDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        path = fs::current_path() / ("baselib-utilities-" + std::to_string(suffix));
        if (!fs::create_directory(path))
            throw std::runtime_error("cannot create test directory");
    }

    ~TestDirectory()
    {
        // The fixture owns only this newly created child of the test working directory.
        std::error_code error;
        fs::remove_all(path, error);
    }
};

struct Collector
{
    bool directories;
    std::set<std::string> names;
    bool RequireDirectory() const { return directories; }
    bool operator()(const char* name) { return names.emplace(name).second; }
};

void test_directories()
{
    for (const char* invalid : {static_cast<const char*>(nullptr), ""})
    {
        check(!g_CreateDirectory(invalid, TRUE), "invalid directory creation fails");
        check(!g_DirectoryExists(invalid), "invalid directory does not exist");
        check(!g_DeleteDirectory(invalid), "invalid directory deletion fails");
        Collector collector{false, {}};
        check(!g_DirectoryTraverse(invalid, collector), "invalid directory traversal fails");
    }

    TestDirectory fixture;
    const auto child = (fixture.path / "space & directory").string();
    check(g_CreateDirectory(child.c_str(), FALSE), "create a directory containing spaces and shell characters");
    check(g_DirectoryExists(child.c_str()), "created directory exists");
    check(!g_CreateDirectory(child.c_str(), FALSE), "nonrecursive creation reports an existing directory");
    check(g_CreateDirectory(child.c_str(), TRUE), "recursive creation accepts an existing directory");
    const auto nested = (fs::path(child) / "parent" / "child").string();
    check(!g_CreateDirectory(nested.c_str(), FALSE), "nonrecursive creation requires the parent");
    check(g_CreateDirectory(nested.c_str(), TRUE), "recursive creation creates missing parents");

    const auto file = (fixture.path / "ordinary.txt").string();
    std::ofstream(file) << "data";
    check(!g_DirectoryExists(file.c_str()), "a regular file is not a directory");
    check(!g_CreateDirectory(file.c_str(), TRUE), "creation does not accept an existing regular file");
    check(!g_DeleteDirectory(file.c_str()) && fs::is_regular_file(file), "directory deletion leaves regular files alone");
    const auto blocked = (fs::path(file) / "child").string();
    check(!g_CreateDirectory(blocked.c_str(), TRUE), "a file cannot be used as a parent directory");

    std::ofstream(fixture.path / ".dotfile") << "hidden";
    fs::create_directory(fixture.path / ".dotdirectory");
    const auto hidden = fixture.path / "hidden.txt";
    std::ofstream(hidden) << "hidden";
#if defined(WIN32) || defined(WIN64)
    check(SetFileAttributesW(hidden.c_str(), FILE_ATTRIBUTE_HIDDEN) != 0, "set the Windows hidden attribute");
#endif
    const auto root = fixture.path.string();
    Collector files{false, {}};
    check(g_DirectoryTraverse(root.c_str(), files), "traverse files");
    std::set<std::string> expected_files{"ordinary.txt"};
#if !defined(WIN32) && !defined(WIN64)
    expected_files.insert("hidden.txt");
#endif
    check(files.names == expected_files, "file traversal excludes directories, dot names, and hidden entries");
    Collector directories{true, {}};
    check(g_DirectoryTraverse(root.c_str(), directories), "traverse directories");
    check(directories.names == std::set<std::string>{"space & directory"}, "directory traversal excludes files and dot names");

    struct RejectEntry
    {
        int calls = 0;
        bool RequireDirectory() const { return false; }
        bool operator()(const char*) { ++calls; return false; }
    } reject;
    check(!g_DirectoryTraverse(root.c_str(), reject) && reject.calls == 1, "traversal reports callback failure");
    Collector invalid{false, {}};
    check(!g_DirectoryTraverse(file.c_str(), invalid), "cannot traverse a regular file");
    const auto missing = (fixture.path / "missing").string();
    check(!g_DirectoryTraverse(missing.c_str(), invalid), "cannot traverse a missing directory");
    check(!g_DeleteDirectory(missing.c_str()), "deletion reports a missing directory");

    const auto empty = (fixture.path / "empty").string();
    check(g_CreateDirectory(empty.c_str(), FALSE), "create an empty directory");
    Collector empty_entries{false, {}};
    check(g_DirectoryTraverse(empty.c_str(), empty_entries) && empty_entries.names.empty(), "empty traversal succeeds");

    std::ofstream(fs::path(nested) / "contents.txt") << "nested";
    check(g_DeleteDirectory(child.c_str()), "recursively delete a directory containing spaces and shell characters");
    check(!fs::exists(child), "deletion finishes before returning");
    check(fs::is_regular_file(file), "deletion preserves siblings");

#if defined(WIN32) || defined(WIN64)
    // Match the legacy A APIs' active Windows code page rather than assuming UTF-8.
    const auto native_path = fixture.path / L"caf\u00e9";
    const auto& native = native_path.native();
    BOOL substituted = FALSE;
    BOOL* used_default = GetACP() == CP_UTF8 ? nullptr : &substituted;
    const int bytes = WideCharToMultiByte(CP_ACP, 0, native.data(), static_cast<int>(native.size()),
                                         nullptr, 0, nullptr, used_default);
    check(bytes > 0 && !substituted, "test path is representable in the active code page");
    if (bytes > 0 && !substituted)
    {
        std::string encoded(static_cast<std::size_t>(bytes), '\0');
        WideCharToMultiByte(CP_ACP, 0, native.data(), static_cast<int>(native.size()),
                            encoded.data(), bytes, nullptr, used_default);
        check(g_CreateDirectory(encoded.c_str(), FALSE) && fs::is_directory(native_path),
              "creation preserves the active code page for non-ASCII paths");
        check(g_DirectoryExists(encoded.c_str()), "existence checks preserve non-ASCII paths");
        check(g_DeleteDirectory(encoded.c_str()) && !fs::exists(native_path), "deletion preserves non-ASCII paths");
    }
#endif
}
}

int main()
{
    test_strings();
    test_directories();
    std::printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
