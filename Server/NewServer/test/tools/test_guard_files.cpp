#include "guard_files.h"
#include "native_handle.h"

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <fstream>
#include <latch>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <windows.h>
#include <winioctl.h>

namespace
{
namespace fs = std::filesystem;
using namespace std::chrono_literals;
int checks = 0;
int failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}

struct Fixture
{
    fs::path base;
    fs::path root;
    explicit Fixture(const fs::path& parent)
    {
        fs::create_directories(parent);
        base = fs::canonical(parent);
        root = base / ("guard-files-" + std::to_string(GetCurrentProcessId()) + "-" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!fs::create_directory(root)) throw std::runtime_error("fixture must be new");
    }
    ~Fixture()
    {
        std::error_code error;
        // The only recursive test teardown is an explicitly checked, unique
        // directory created by this fixture beneath its canonical workspace base.
        if (root.parent_path() == base && root.filename().string().starts_with("guard-files-") &&
            fs::canonical(root, error) == root && !error)
            fs::remove_all(root, error);
    }
};

void write(const fs::path& file, std::string_view value)
{
    fs::create_directories(file.parent_path());
    std::ofstream stream(file, std::ios::binary);
    stream.write(value.data(), static_cast<std::streamsize>(value.size()));
    if (!stream) throw std::runtime_error("fixture write failed");
}

// Build real directory junctions using the Windows filesystem API. No shell,
// elevated symlink privilege, service or deployment command is needed.
bool junction(const fs::path& link, const fs::path& destination)
{
    fs::create_directory(link);
    guard::NativeHandle handle{CreateFileW(link.c_str(), GENERIC_WRITE, 0, nullptr,
        OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr)};
    if (!handle || handle.get() == INVALID_HANDLE_VALUE) return false;
    struct MountPoint
    {
        DWORD tag;
        WORD length;
        WORD reserved;
        WORD substitute_offset;
        WORD substitute_length;
        WORD print_offset;
        WORD print_length;
        wchar_t names[4096];
    } data{};
    const auto target = fs::absolute(destination).native();
    const auto substitute = L"\\??\\" + target;
    const auto print_offset = substitute.size() + 1;
    if (print_offset + target.size() + 1 > std::size(data.names)) return false;
    data.tag = IO_REPARSE_TAG_MOUNT_POINT;
    data.substitute_length = static_cast<WORD>(substitute.size() * sizeof(wchar_t));
    data.print_offset = static_cast<WORD>(print_offset * sizeof(wchar_t));
    data.print_length = static_cast<WORD>(target.size() * sizeof(wchar_t));
    data.length = static_cast<WORD>(8 + (print_offset + target.size() + 1) * sizeof(wchar_t));
    std::ranges::copy(substitute, data.names);
    std::ranges::copy(target, data.names + print_offset);
    DWORD returned = 0;
    return DeviceIoControl(handle.get(), FSCTL_SET_REPARSE_POINT, &data, data.length + 8,
        nullptr, 0, &returned, nullptr) != 0;
}

void calendar()
{
    for (int year = 1899; year <= 2101; ++year)
        for (int month = 0; month <= 13; ++month)
            for (int day = 0; day <= 32; ++day)
            {
                const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
                constexpr std::array days{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                const int maximum = month >= 1 && month <= 12 ? days[month] + (month == 2 && leap) : 0;
                char date[16]{};
                std::snprintf(date, sizeof(date), "%04d_%02d_%02d", year, month, day);
                check(guard_files::is_log_date(date) == (day > 0 && day <= maximum),
                    "date selection matches Gregorian calendar, including century boundaries");
            }
    for (const auto* invalid : {"", "0000_01_01", "2024_2_01", "2024-02-01", "2024_02_01x",
            "../2024_02_01", "2024_02_01/", "202a_02_01", "2024_+2_01", "2024_02_ 1"})
        check(!guard_files::is_log_date(invalid), "non-date names cannot become deletion targets");
    check(guard_files::is_log_date("0001_01_01") && guard_files::is_log_date("9999_12_31"),
        "four-digit calendar boundaries supported");
}

void cleanup(const fs::path& root)
{
    const auto run = root / "runtime with spaces & 100% !";
    const auto log = run / "log";
    for (const auto* name : {"2024_02_29", "2025_01_01", "2025_02_29", "2024_2_28", "other"})
        fs::create_directories(log / name);
    write(log / "2020_01_01", "ordinary file");
    auto names = guard_files::log_folders(run);
    check(names && *names == std::vector<std::string>{"2024_02_29", "2025_01_01"},
        "enumerate only date directories in chronological order");
    check(!guard_files::log_folders(root / "missing") && !guard_files::log_folders({}),
        "missing or empty log roots are errors");
    write(log / "2024_02_29" / "first.log", "one");
    write(log / "2024_02_29" / "nested" / "deeper" / "second.log", "two");
    auto erased = guard_files::erase_log_folder(run, "2024_02_29");
    check(erased && *erased == 5 && !fs::exists(log / "2024_02_29"),
        "recursive ordinary log cleanup removes the complete date tree");
    check(fs::is_directory(log / "2024_02_29_cleard_by_guard") && fs::is_directory(log / "2025_01_01"),
        "successful cleanup marks only the removed date and leaves other dates alone");
    check(fs::is_directory(log / "other") && fs::is_directory(log / "2025_02_29"),
        "unrecognized directories survive cleanup");
    for (const auto* invalid : {"..", ".", "2025_02_29", "2025_01_01/..", "\\outside", "C:\\outside"})
        check(!guard_files::erase_log_folder(run, invalid), "invalid cleanup paths rejected before mutation");
    check(fs::exists(log / "2025_01_01"), "invalid targets never remove the selected date");
    check(!guard_files::erase_log_folder(run, "2020_01_01"), "a date-named file is not a log folder");

    const auto marker = log / "2025_01_01_cleard_by_guard";
    write(marker, "marker conflict");
    auto conflict = guard_files::erase_log_folder(run, "2025_01_01");
    check(!conflict && fs::exists(log / "2025_01_01"), "marker conflict fails before log deletion");
    fs::remove(marker);
    std::stop_source stop;
    stop.request_stop();
    auto canceled = guard_files::erase_log_folder(run, "2025_01_01", stop.get_token());
    check(!canceled && canceled.error() == std::errc::operation_canceled && !fs::exists(marker)
        && fs::exists(log / "2025_01_01"), "canceled cleanup has no completion marker");

    const auto locked = log / "2025_01_01" / "locked.log";
    write(locked, "retain while locked");
    {
        guard::NativeHandle handle{CreateFileW(locked.c_str(), GENERIC_READ, FILE_SHARE_READ,
            nullptr, OPEN_EXISTING, 0, nullptr)};
        check(handle && handle.get() != INVALID_HANDLE_VALUE, "open fixture file without delete sharing");
        auto failed = guard_files::erase_log_folder(run, "2025_01_01");
        check(!failed && fs::exists(locked) && !fs::exists(marker),
            "native deletion failure is visible and never marked successful");
    }
    fs::create_directory(marker);
    erased = guard_files::erase_log_folder(run, "2025_01_01");
    check(erased && *erased == 2 && fs::is_directory(marker), "cleanup can retry and reuse an existing ordinary marker");

    const auto outside = root / "outside";
    write(outside / "must-survive.log", "outside");
    auto link = log / "2026_01_01";
    const auto made = junction(link, outside);
    check(made, "create native directory junction fixture");
    if (made)
    {
        names = guard_files::log_folders(run);
        check(names && names->empty(), "date-named junctions are not offered as log directories");
        check(!guard_files::erase_log_folder(run, "2026_01_01") && fs::exists(outside / "must-survive.log"),
            "cleanup refuses a date directory junction");
        check(RemoveDirectoryW(link.c_str()) != 0, "remove only the fixture junction");
    }
    const auto date = log / "2026_01_02";
    fs::create_directory(date);
    link = date / "nested-junction";
    if (junction(link, outside))
    {
        write(date / "local.log", "local");
        check(!guard_files::erase_log_folder(run, "2026_01_02") && fs::exists(date / "local.log") &&
            fs::exists(outside / "must-survive.log") && !fs::exists(log / "2026_01_02_cleard_by_guard"),
            "preflight rejects nested junctions before deleting local files");
        check(RemoveDirectoryW(link.c_str()) != 0, "remove nested fixture junction");
    }
    else check(false, "create nested native junction fixture");
    const auto redirected = root / "redirected-runtime";
    if (junction(redirected, run))
    {
        check(!guard_files::log_folders(redirected) && !guard_files::erase_log_folder(redirected, "2026_01_02"),
            "ancestor junctions cannot redirect a configured cleanup root");
        check(RemoveDirectoryW(redirected.c_str()) != 0, "remove ancestor fixture junction");
    }
    else check(false, "create ancestor native junction fixture");
    // No junctions remain when Fixture performs its verified teardown.
}

void tags(const fs::path& root)
{
    const auto file = root / "tokens" / "version.txt";
    auto tag = guard_files::read_tag(file);
    check(tag && !*tag, "missing result files are pending");
    for (const auto& value : {std::string("server.zip"), std::string("SERVER.ZIP\r\n"),
            std::string(127, 'v') + " \r\n\t", std::string("v\x81\x40.zip")})
    {
        write(file, value);
        tag = guard_files::read_tag(file);
        check(tag && *tag && (**tag).size() <= 127 && !((**tag).ends_with('\n')),
            "read bounded version names and echo terminators without a writable file handle");
    }
    for (const auto& value : {std::string{}, std::string("\r\n"), std::string(" leading"), std::string("two names"),
            std::string("name\0suffix", 11), std::string(128, 'x'), std::string("x") + std::string(4096, ' ')})
    {
        write(file, value);
        check(!guard_files::read_tag(file), "empty, unterminated oversized and malformed tags rejected");
    }
    write(file, "x" + std::string(4095, ' '));
    tag = guard_files::read_tag(file);
    check(tag && *tag && **tag == "x", "total input bound includes trailing whitespace");
    check(!guard_files::read_tag(root), "directories cannot be result tags");
    write(file, "1234");
    check(!guard_files::read_tag(file, 3) && guard_files::read_tag(file, 4).has_value(),
        "caller-supplied tag capacity is enforced exactly");
    check(guard_files::same_tag("Server.ZIP", "sERVER.zip") && !guard_files::same_tag("a", "ab") &&
        !guard_files::same_tag("ab", "a") && !guard_files::same_tag("ab", "ac"),
        "case-insensitive tag equality never accepts prefixes");
    check(guard_files::same_tag("v\x81\x40", "V\x81\x40") && !guard_files::same_tag("\x81", "\x82"),
        "tag comparisons preserve non-ASCII bytes without locale-dependent folding");
    write(file, "complete.zip\r\n");
    tag = guard_files::consume_tag(file);
    check(tag && *tag && **tag == "complete.zip" && !fs::exists(file), "claim completed tags before consuming them");
    tag = guard_files::consume_tag(file);
    check(tag && !*tag, "consumed tags cannot be acknowledged twice");
    write(file, std::string(128, 'x'));
    check(!guard_files::consume_tag(file) && !fs::exists(file), "malformed completed tags are consumed once");
    check(std::ranges::distance(fs::directory_iterator(file.parent_path())) == 0,
        "claim directories and files are removed after both success and parse failure");
}

std::optional<guard_files::FileTask::Completion> wait(guard_files::FileTask& task)
{
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    while (std::chrono::steady_clock::now() < deadline)
    {
        if (auto result = task.poll()) return result;
        std::this_thread::sleep_for(1ms);
    }
    return {};
}

std::string read(const fs::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    return {std::istreambuf_iterator<char>{stream}, {}};
}

void uploads(const fs::path& root)
{
    const auto deep = root / std::string(160, 'd') / std::string(100, 'e');
    auto created = guard_files::directory(deep, true);
    check(created && *created == deep && guard_files::display_path(guard_files::io_path(deep)) == deep,
        "long IO paths preserve ordinary configuration/storage spelling");
    check(guard_files::directory(deep / "").value() == deep, "directory comparison normalizes trailing separators");
    {
        guard_files::IncomingFile receiver;
        const guard_files::UploadChunk longChunk{"long.bin", 0, 0, 1, 0, 1, std::span{"long data", 9}};
        auto result = receiver.receive(deep, longChunk, &receiver);
        check(result && *result && (**result).path == deep / "long.bin" &&
            guard_files::regular_file(deep / "long.bin").has_value(), "uploads can publish beyond Windows MAX_PATH");
    }
    check(guard_files::remove_entry(root, deep.lexically_relative(root)).has_value(), "bounded long-path cleanup succeeds");
    const fs::path unc{L"\\\\server\\share\\folder"};
    check(guard_files::display_path(guard_files::io_path(unc)) == unc, "UNC IO conversion round-trips without accessing a share");
    const auto folder = root / "uploads & 100% !";
    fs::create_directory(folder);
    int owner = 0, other = 0;
    guard_files::IncomingFile receiver;
    guard_files::UploadChunk chunk{"archive.zip", 0, 0, 1, 0, 2, std::span{"first", 5}};
    write(folder / "archive.zip", "old");
    auto result = receiver.receive(folder, chunk, &owner);
    check(result && !*result && read(folder / "archive.zip") == "old", "partial upload leaves the current file intact");
    auto foreign = receiver.receive(folder, chunk, &other);
    check(!foreign && foreign.error() == std::errc::device_or_resource_busy, "another connection cannot replace an active upload");
    receiver.abort(&other);
    chunk.buffer_index = 1; chunk.bytes = std::span{"second", 6};
    result = receiver.receive(folder, chunk, &owner);
    check(result && *result && (**result).path == folder / "archive.zip" &&
        read(folder / "archive.zip") == "firstsecond", "last chunk atomically replaces an existing destination");
    check(!receiver.receive(folder, chunk, &owner), "duplicate final chunks cannot reopen or append a completed file");
    check(std::ranges::distance(fs::directory_iterator(folder)) == 1, "completed uploads leave no temporary directories");

    for (const auto* name : {"../escape", "sub/name", "sub\\name", "C:drive", "CON", "nul.txt", "COM1.zip", "LPT9",
            "trailing.", "trailing ", "", "..", "bad:name", "bad*name"})
    {
        chunk = {name, 0, 0, 1, 0, 1, {}};
        check(!receiver.receive(folder, chunk, &owner), "invalid leaf names are rejected before creating files");
    }
    chunk = {"empty.bin", 0, 0, 1, 0, 1, {}};
    result = receiver.receive(folder, chunk, &owner);
    check(result && *result && fs::file_size(folder / "empty.bin") == 0, "empty files complete as a single empty chunk");
    check(guard_files::leaf_name("name & 100% !.zip"), "ordinary filename bytes need no shell escaping");
    const auto longName = std::string(128, 'x');
    chunk.name = longName;
    check(!receiver.receive(folder, chunk, &owner), "upload filename respects the wire field's terminator capacity");
    const std::string tooLarge(32769, 'x');
    chunk = {"large.bin", 0, 0, 1, 0, 1, std::span{tooLarge}};
    check(!receiver.receive(folder, chunk, &owner), "oversized payloads are rejected before stream writes");

    for (int changed = 0; changed < 7; ++changed)
    {
        chunk = {"sequence.bin", 0, 0, 2, 0, 2, std::span{"a", 1}};
        check(receiver.receive(folder, chunk, &owner).has_value(), "start metadata consistency case");
        chunk.buffer_index = 1;
        switch (changed)
        {
        case 0: chunk.name = "different.bin"; break;
        case 1: chunk.type = 1; break;
        case 2: chunk.file_index = 1; break;
        case 3: chunk.file_total = 3; break;
        case 4: chunk.buffer_total = 3; break;
        case 5: chunk.buffer_index = 2; break;
        }
        check(!receiver.receive(changed == 6 ? root : folder, chunk, &owner) && !fs::exists(folder / "sequence.bin"),
            "changed transfer metadata aborts without publishing an incomplete file");
    }
    for (const auto invalid : std::array{
            guard_files::UploadChunk{"bad.bin", 0, 0, 0, 0, 1, {}},
            guard_files::UploadChunk{"bad.bin", 0, 1, 1, 0, 1, {}},
            guard_files::UploadChunk{"bad.bin", 0, 0, 1, 0, 0, {}},
            guard_files::UploadChunk{"bad.bin", 0, 0, 1, 1, 1, {}}})
        check(!receiver.receive(folder, invalid, &owner), "invalid indexes and totals cannot start uploads");
    chunk = {"interrupted.bin", 0, 0, 1, 0, 2, std::span{"partial", 7}};
    {
        guard_files::IncomingFile interrupted;
        check(interrupted.receive(folder, chunk, &owner).has_value(), "start destructor cleanup case");
    }
    check(!fs::exists(folder / "interrupted.bin") && std::ranges::distance(fs::directory_iterator(folder)) == 2,
        "destroyed receivers close their streams and remove only their temporary files");
    std::string binary(32768, '\0');
    for (std::size_t index = 0; index < binary.size(); ++index) binary[index] = static_cast<char>(index % 256);
    chunk = {"binary.bin", 0, 0, 1, 0, 2, std::span{binary}};
    check(receiver.receive(folder, chunk, &owner).has_value(), "accept a full binary payload chunk");
    chunk.buffer_index = 1;
    result = receiver.receive(folder, chunk, &owner);
    check(result && *result && read(folder / "binary.bin") == binary + binary,
        "full final chunks preserve all byte values without an extra terminator chunk");
    write(folder / "locked.bin", "keep");
    chunk = {"locked.bin", 0, 0, 1, 0, 1, std::span{"replace", 7}};
    {
        guard::NativeHandle locked{CreateFileW((folder / "locked.bin").c_str(), GENERIC_READ,
            FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)};
        check(locked && locked.get() != INVALID_HANDLE_VALUE, "lock existing upload destination");
        check(!receiver.receive(folder, chunk, &owner) && read(folder / "locked.bin") == "keep",
            "failed publication preserves the existing destination");
    }
    result = receiver.receive(folder, chunk, &owner);
    check(result && *result && read(folder / "locked.bin") == "replace", "upload can restart after failed publication");
    const auto copy = folder / "config folder" / "copied.bin";
    check(guard_files::copy_file(folder / "archive.zip", copy).has_value() && read(copy) == "firstsecond",
        "configuration copies create directories and complete synchronously");
    check(guard_files::copy_file(folder / "locked.bin", copy).has_value() && read(copy) == "replace",
        "configuration replacement overwrites existing files atomically");
    check(!guard_files::copy_file(folder / "missing.bin", copy) && read(copy) == "replace",
        "copy failure cannot truncate the installed configuration");
    check(!guard_files::copy_file(folder / "archive.zip", copy, [] { return false; }) && read(copy) == "replace",
        "rejected configuration restores the previous installed file");
    check(!guard_files::copy_file(folder / "archive.zip", copy, []() -> bool { throw 42; }) && read(copy) == "replace",
        "throwing configuration validation also restores the previous file");
    const auto absent = copy.parent_path() / "absent.bin";
    check(!guard_files::copy_file(copy, absent, [] { return false; }) && !fs::exists(absent),
        "rejected first-time installation restores the previous absent state");
    const auto outside = root / "upload outside";
    fs::create_directory(outside);
    const auto link = folder / "redirect";
    if (junction(link, outside))
    {
        check(!receiver.receive(link, chunk, &owner) && !fs::exists(outside / "locked.bin"),
            "upload roots cannot follow directory junctions");
        check(!guard_files::copy_file(copy, link / "outside.bin") && !fs::exists(outside / "outside.bin"),
            "configuration copies cannot follow ancestor junctions");
        check(RemoveDirectoryW(link.c_str()) != 0, "remove upload fixture junction");
    }
    else check(false, "create upload junction fixture");
}

void workers()
{
    guard_files::FileTask task;
    check(!task.busy() && !task.poll() && !task.start("", {}), "empty worker owns no thread or result");
    std::latch entered{1};
    std::latch release{1};
    check(task.start("owned path", [data = std::make_unique<int>(42), &entered, &release](std::stop_token) -> guard_files::Result {
        entered.count_down(); release.wait(); return *data;
    }), "start accepts move-only owned captures");
    entered.wait();
    check(task.busy() && !task.poll() && !task.start("second", [](std::stop_token) -> guard_files::Result { return 7; }),
        "main thread cannot start a competing cleanup task");
    release.count_down();
    auto result = wait(task);
    check(result && result->path == "owned path" && result->result && *result->result == 42 && !task.busy(),
        "completion owns its path and joins before the next task");
    check(!task.poll(), "each completion is consumed once");
    for (int failure = 0; failure < 3; ++failure)
    {
        check(task.start("failure", [failure](std::stop_token) -> guard_files::Result {
            if (failure == 0) throw std::system_error(std::make_error_code(std::errc::permission_denied));
            if (failure == 1) throw std::bad_alloc{};
            throw 42;
        }), "reuse worker after a consumed completion");
        result = wait(task);
        const auto expected = failure == 0 ? std::errc::permission_denied :
            failure == 1 ? std::errc::not_enough_memory : std::errc::io_error;
        check(result && !result->result && result->result.error() == expected,
            "worker exceptions are delivered as explicit error results");
    }
    std::atomic<bool> stopped{false};
    std::latch waiting{1};
    {
        guard_files::FileTask owned;
        check(owned.start("cancel", [&stopped, &waiting](std::stop_token stop) -> guard_files::Result {
            std::mutex mutex;
            std::condition_variable_any condition;
            std::unique_lock lock(mutex);
            waiting.count_down();
            condition.wait(lock, stop, [] { return false; });
            stopped = true;
            return std::unexpected(std::make_error_code(std::errc::operation_canceled));
        }), "owned worker starts a cooperatively cancelable action");
        waiting.wait();
    }
    check(stopped, "destruction requests cancellation and joins before captures are destroyed");
    task.stop(); task.stop();
    check(!task.busy() && !task.poll(), "repeated shutdown clears stale results");
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2) throw std::runtime_error("workspace fixture parent required");
        Fixture fixture{argv[1]};
        calendar(); cleanup(fixture.root); tags(fixture.root); uploads(fixture.root); workers();
    }
    catch (const std::exception& error) { check(false, error.what()); }
    std::printf("guard files: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
