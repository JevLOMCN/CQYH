#include "guard_deploy.h"
#include "native_handle.h"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <ranges>
#include <string>
#include <thread>
#include <windows.h>

namespace
{
namespace fs = std::filesystem;
using namespace std::chrono_literals;
int checks = 0, failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}
void write(const fs::path& path, std::string_view data)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    if (!file) throw std::runtime_error("fixture write failed");
}
std::string read(const fs::path& path)
{
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>{file}, {}};
}
struct Fixture
{
    fs::path parent, root;
    explicit Fixture(const fs::path& base)
    {
        fs::create_directories(base);
        parent = fs::canonical(base);
        root = parent / ("guard-deploy-" + std::to_string(GetCurrentProcessId()) + "-" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!fs::create_directory(root)) throw std::runtime_error("new fixture required");
    }
    ~Fixture()
    {
        std::error_code error;
        if (root.parent_path() == parent && root.filename().string().starts_with("guard-deploy-") &&
            fs::canonical(root, error) == root && !error) fs::remove_all(root, error);
    }
};
guard_deploy::Request seed(const fs::path& root)
{
    guard_deploy::Request request{root / "runtime" / "_GUARD", root / "runtime" / "_GUARD" / "version",
        root / "runtime" / "_GUARD" / "patch", root / "runtime", "release.zip"};
    fs::create_directories(request.versions); fs::create_directories(request.patches);
    write(request.versions / request.name, "fake archive");
    write(request.versions / (request.name + "-area_config.lua"), "area override");
    write(request.versions / (request.name + "-gmsecurityconfig.lua"), "gm override");
    write(request.runtime / "gameserver" / "old.bin", "old game");
    write(request.runtime / "cache" / "old", "old cache");
    write(request.runtime / "data" / "old.pak", "old data");
    write(request.runtime / "custom" / "keep", "keep custom");
    write(request.guard / "guardserver.exe", "running guard");
    write(request.guard / "current_server_version.txt", "previous.zip\r\n");
    return request;
}
guard_deploy::Extractor extract(const fs::path& source)
{
    return [source](const fs::path&, const fs::path& destination, const fs::path&, std::stop_token) -> guard_files::Result {
        fs::copy(source, destination / "release", fs::copy_options::recursive);
        return 0;
    };
}
void deployment(const fs::path& root, const fs::path& source)
{
    auto request = seed(root / "success & 100% !");
    // ReloadConfig commonly receives '..\\', which retains a trailing separator.
    request.guard /= ""; request.versions /= ""; request.runtime /= "";
    write(request.versions / "old.zip", "old archive");
    write(request.versions / "release-old.zip", "matching prefix");
    auto result = guard_deploy::version(request, {}, extract(source));
    if (!result) std::fprintf(stderr, "deployment diagnostic: %d %s; guard path chars %zu; log: %s\n", result.error().value(), result.error().message().c_str(), request.guard.native().size(), read(request.guard / "deploy_version.log").c_str());
    check(result && *result >= 4, "complete staged deployment returns copied-file count");
    check(read(request.runtime / "gameserver" / "new.bin") == "new game" &&
        !fs::exists(request.runtime / "gameserver" / "old.bin") && !fs::exists(request.runtime / "cache"),
        "obsolete server trees/cache are retired before package overlay");
    check(read(request.runtime / "custom" / "keep") == "keep custom" && read(request.runtime / "custom" / "new") == "new custom",
        "package overlay preserves unrelated existing files");
    check(read(request.guard / "guardserver.exe") == "running guard" && !fs::exists(request.guard / "bad.exe"),
        "_GUARD package contents never replace the running guard");
    check(!fs::exists(request.runtime / "_GUARD-extra") && !fs::exists(request.runtime / "placeholder.txt"),
        "legacy _GUARD prefix and placeholder exclusion entries are preserved");
    check(read(request.runtime / "globalconfig" / "area_config.lua") == "area override" &&
        read(request.runtime / "globalconfig" / "gmsecurityconfig.lua") == "gm override", "per-version configurations override archive defaults");
    check(!fs::exists(request.versions / "old.zip") && fs::exists(request.versions / "release-old.zip") &&
        fs::is_directory(request.versions / "release"), "cache pruning preserves the current version prefix and extracted tree");
    for (const auto& tag : {request.runtime / "current_server_version.txt", request.guard / "current_server_version.txt", request.guard / "version.txt"})
        check(read(tag) == "release.zip\r\n", "successful deployment publishes established version tags");
    check(std::ranges::none_of(fs::directory_iterator(request.versions), [](const auto& entry) {
        return entry.path().filename().string().starts_with(".guard-extract-"); }), "successful staging directories are cleaned");

    for (int mode = 0; mode < 4; ++mode)
    {
        auto failed = seed(root / ("failure" + std::to_string(mode)));
        guard_deploy::Extractor backend;
        if (mode == 0) backend = [](const auto&, const auto&, const auto&, auto) -> guard_files::Result { return 7; };
        if (mode == 1) backend = [](const auto&, const auto&, const auto&, auto) -> guard_files::Result { return 0; };
        if (mode == 2) fs::remove(failed.versions / (failed.name + "-area_config.lua"));
        if (mode == 3) failed.name = "../escape.zip";
        result = guard_deploy::version(failed, {}, std::move(backend));
        check(!result && read(failed.runtime / "gameserver" / "old.bin") == "old game" &&
            read(failed.guard / "current_server_version.txt") == "previous.zip\r\n",
            "extractor/package/config/name failures occur before retiring runtime files");
    }
    auto canceled = seed(root / "canceled");
    std::stop_source stop; stop.request_stop();
    result = guard_deploy::version(canceled, stop.get_token(), extract(source));
    check(!result && result.error() == std::errc::operation_canceled &&
        fs::exists(canceled.runtime / "gameserver" / "old.bin"), "canceled deployment leaves existing runtime files alone");
    auto overlap = seed(root / "overlap");
    overlap.versions = overlap.runtime;
    result = guard_deploy::version(overlap, {}, extract(source));
    check(!result && fs::exists(overlap.runtime / "gameserver" / "old.bin"), "overlapping cache/runtime roots are rejected before mutation");
    auto readonly = seed(root / "readonly");
    write(readonly.runtime / "custom" / "new", "old custom");
    check(SetFileAttributesW((source / "globalconfig" / "area_config.lua").c_str(), FILE_ATTRIBUTE_READONLY) &&
        SetFileAttributesW((readonly.runtime / "custom" / "new").c_str(), FILE_ATTRIBUTE_READONLY), "mark fixture configuration/overlay files read-only");
    result = guard_deploy::version(readonly, {}, extract(source));
    check(result && read(readonly.runtime / "globalconfig" / "area_config.lua") == "area override" &&
        read(readonly.runtime / "custom" / "new") == "new custom", "legacy writable configuration and /R overlay behavior is retained");
    check(SetFileAttributesW((source / "globalconfig" / "area_config.lua").c_str(), FILE_ATTRIBUTE_NORMAL) != 0,
        "restore source fixture attributes");
    auto locked = seed(root / "locked overlay");
    write(locked.runtime / "custom" / "new", "locked old custom");
    {
        guard::NativeHandle handle{CreateFileW((locked.runtime / "custom" / "new").c_str(), GENERIC_READ,
            FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)};
        check(handle && handle.get() != INVALID_HANDLE_VALUE, "lock an existing overlay destination");
        result = guard_deploy::version(locked, {}, extract(source));
        check(!result && !fs::exists(locked.guard / "version.txt") &&
            read(locked.guard / "current_server_version.txt") == "previous.zip\r\n" &&
            read(locked.runtime / "custom" / "new") == "locked old custom",
            "copy failure never publishes successful result/current-version tags");
        check(!fs::exists(locked.runtime / "gameserver" / "old.bin"),
            "runtime replacement retains its existing nontransactional failure boundary");
    }
}
void patches(const fs::path& root)
{
    auto request = seed(root / "patch tests");
    request.name = "commonconfig_patch2.pak";
    write(request.patches / request.name, "new patch");
    write(request.runtime / "data" / "commonconfig_patch1.pak", "old patch");
    write(request.runtime / "data" / "globalconfig_patch1.pak", "old global patch");
    write(request.runtime / "data" / "unrelated.pak", "keep");
    auto result = guard_deploy::patch(request);
    check(result && read(request.runtime / "data" / request.name) == "new patch", "install patch using ordinary filesystem copy");
    check(!fs::exists(request.runtime / "data" / "commonconfig_patch1.pak") &&
        !fs::exists(request.runtime / "data" / "globalconfig_patch1.pak") &&
        read(request.runtime / "data" / "unrelated.pak") == "keep", "retire only the established patch filename families");
    check(read(request.guard / "patch.txt") == request.name + "\r\n" &&
        read(request.runtime / "current_server_patch.txt") == request.name + "\r\n", "patch tags use the existing paths and format");
    request.name = "missing.pak";
    check(!guard_deploy::patch(request) && read(request.runtime / "data" / "commonconfig_patch2.pak") == "new patch",
        "missing patch input never deletes the installed patch");
    for (const auto relative : {fs::path{}, fs::path{"."}, fs::path{".."}, fs::path{"../runtime"}, request.runtime})
        check(!guard_files::remove_entry(request.runtime, relative), "bounded cleanup refuses root/parent/absolute targets");
}
void native(const fs::path& root, const fs::path& program, const fs::path& extractor, const fs::path& source)
{
    fs::create_directories(root);
    const auto log = root / "child output.log";
    auto result = guard_deploy::run_process(program, {L"exit"}, root, log);
    check(result && *result == 17, "native child exit code is observed without a shell");
    const std::vector<std::wstring> expected{L"", L"spaces & % !", L"a\"b", L"trailing\\", L"slashes\\\\\" value", L"\u4E2D\u6587"};
    std::vector<std::wstring> arguments{L"args", (root / "args.bin").native()};
    arguments.insert(arguments.end(), expected.begin(), expected.end());
    result = guard_deploy::run_process(program, arguments, root, log);
    check(result && *result == 0, "run child with quoted literal arguments");
    std::ifstream file(root / "args.bin", std::ios::binary);
    for (const auto& argument : expected)
    {
        std::uint32_t size = 0; file.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::wstring value(size, L'\0'); file.read(value.data() ? reinterpret_cast<char*>(value.data()) : nullptr, size * sizeof(wchar_t));
        check(file.good() && value == argument, "spaces, empty values, quotes and trailing slashes survive argv parsing");
    }
    file.close();
    const auto ready = root / "ready.txt";
    guard_files::Result canceled = 0;
    std::jthread child([&](std::stop_token stop) {
        canceled = guard_deploy::run_process(program, {L"hold", ready.native()}, root, log, stop);
    });
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    DWORD pid = 0;
    while (!pid && std::chrono::steady_clock::now() < deadline)
    {
        std::ifstream(ready) >> pid;
        if (!pid) std::this_thread::sleep_for(5ms);
    }
    check(pid != 0, "owned cancellation fixture child published its process ID");
    guard::NativeHandle ended{OpenProcess(SYNCHRONIZE, FALSE, pid)};
    check(ended != nullptr, "retain identity of the running test child before cancellation");
    child.request_stop(); child.join();
    check(!canceled && canceled.error() == std::errc::operation_canceled, "stop token cancels the owned native child");
    check(ended && WaitForSingleObject(ended.get(), 0) == WAIT_OBJECT_0, "canceled child has exited before returning");
    DWORD before = 0, after = 0;
    check(GetProcessHandleCount(GetCurrentProcess(), &before) != 0, "read native handle baseline");
    for (int index = 0; index < 5; ++index)
        check(guard_deploy::run_process(program, {L"exit"}, root, log).has_value(), "repeat owned child launch and wait");
    check(GetProcessHandleCount(GetCurrentProcess(), &after) && before == after, "process/thread/log/attribute resources do not leak handles");
    auto request = seed(root / "real archive");
    fs::copy_file(extractor, request.guard / "7z.exe");
    const auto input = root / "zip input";
    fs::create_directory(input);
    fs::copy(source, input / "release", fs::copy_options::recursive);
    fs::remove(request.versions / request.name);
    result = guard_deploy::run_process(extractor, {L"a", L"-tzip", (request.versions / request.name).native(), L"release"}, input, log);
    check(result && *result == 0, "bundled extractor creates the offline ZIP fixture");
    result = guard_deploy::version(request);
    if (!result) std::fprintf(stderr, "native deployment diagnostic: %d %s; log: %s\n", result.error().value(), result.error().message().c_str(), read(request.guard / "deploy_version.log").c_str());
    check(result && read(request.runtime / "gameserver" / "new.bin") == "new game" &&
        read(request.guard / "guardserver.exe") == "running guard", "real bundled ZIP extraction and staged deployment agree with filesystem fixtures");
}
}
int main(int argc, char** argv)
{
    try
    {
        if (argc != 4) throw std::runtime_error("workspace parent, child executable and extractor required");
        Fixture fixture{argv[1]};
        const auto source = fixture.root / "package source";
        write(source / "gameserver" / "new.bin", "new game");
        write(source / "globalconfig" / "area_config.lua", "archive area");
        write(source / "custom" / "new", "new custom");
        write(source / "_GUARD" / "bad.exe", "must not install");
        write(source / "_GUARD-extra" / "bad.exe", "excluded prefix");
        write(source / "placeholder.txt", "excluded placeholder");
        deployment(fixture.root, source); patches(fixture.root);
        native(fixture.root / "native & 100% !", fs::absolute(argv[2]), fs::absolute(argv[3]), source);
    }
    catch (const std::exception& error) { check(false, error.what()); }
    std::printf("guard deployment: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
