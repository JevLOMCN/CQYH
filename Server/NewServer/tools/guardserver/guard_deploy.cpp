#include "guard_deploy.h"
#include "native_handle.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <fstream>
#include <ranges>
#include <windows.h>

namespace guard_deploy
{
namespace
{
namespace fs = std::filesystem;
using guard_files::Result;
using guard_files::io_path;
using guard_files::display_path;
auto failure(std::errc error) { return std::unexpected(std::make_error_code(error)); }
auto native_failure() { return std::unexpected(std::error_code(GetLastError(), std::system_category())); }

bool same_path(const fs::path& left, const fs::path& right)
{
    const auto& a = left.native();
    const auto& b = right.native();
    return CompareStringOrdinal(a.data(), static_cast<int>(a.size()), b.data(), static_cast<int>(b.size()), TRUE) == CSTR_EQUAL;
}

bool beneath(const fs::path& parent, const fs::path& child)
{
    auto p = parent.begin(), c = child.begin();
    for (; p != parent.end(); ++p, ++c)
        if (c == child.end() || !same_path(*p, *c)) return false;
    return true;
}

bool valid_name(std::string_view name)
{
    // Existing current/result tag files use whitespace as a token delimiter.
    return name.size() <= 127 && guard_files::leaf_name(name) &&
        std::ranges::none_of(name, [](unsigned char value) { return value <= ' '; });
}

bool starts_with_name(std::string_view name, std::string_view prefix)
{
    return name.size() >= prefix.size() && guard_files::same_tag(name.substr(0, prefix.size()), prefix);
}

bool excluded(const fs::path& relative)
{
    if (starts_with_name(relative.begin()->string(), "_GUARD")) return true;
    const auto text = relative.string();
    for (std::size_t index = 0; index < text.size(); ++index)
        if (starts_with_name(std::string_view{text}.substr(index), "placeholder")) return true;
    return false;
}

void trace(const fs::path& path, std::string_view message) noexcept
{
    try
    {
        std::ofstream file(io_path(path), std::ios::app | std::ios::binary);
        file << message << "\r\n";
    }
    catch (...) {} // A diagnostic must not turn a completed copy into failure.
}

std::expected<void, std::error_code> write_tag(const fs::path& path, std::string_view name)
{
    guard_files::IncomingFile receiver;
    const auto value = std::string(name) + "\r\n";
    const auto filename = path.filename().string();
    const guard_files::UploadChunk chunk{filename, 0, 0, 1, 0, 1, std::span{value}};
    auto result = receiver.receive(path.parent_path(), chunk, &receiver);
    if (!result) return std::unexpected(result.error());
    return {};
}

std::expected<void, std::error_code> replace_file(const fs::path& source, const fs::path& destination)
{
    std::error_code error;
    if (fs::exists(io_path(destination), error))
    {
        if (auto checked = guard_files::regular_file(destination); !checked) return checked;
        fs::permissions(io_path(destination), fs::perms::owner_write, fs::perm_options::add, error);
    }
    if (error) return std::unexpected(error);
    return guard_files::copy_file(source, destination);
}

struct Stage
{
    fs::path root, path;
    bool owned = false;
    ~Stage()
    {
        if (!owned) return;
        try { (void)guard_files::remove_entry(root, path.filename()); }
        catch (...) {} // Retain an unremovable staging tree; never widen cleanup.
    }
};

std::expected<void, std::error_code> new_stage(Stage& stage, const fs::path& root)
{
    static std::atomic<unsigned long long> sequence{0};
    stage.root = root;
    for (int attempt = 0; attempt < 100; ++attempt)
    {
        stage.path = root / (".guard-extract-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()) + "-" + std::to_string(sequence++));
        std::error_code error;
        if (fs::create_directory(io_path(stage.path), error)) { stage.owned = true; return {}; }
        if (error && error != std::errc::file_exists) return std::unexpected(error);
    }
    return failure(std::errc::file_exists);
}

std::expected<std::vector<fs::path>, std::error_code> tree(const fs::path& directory, std::stop_token stop)
{
    auto checked = guard_files::directory(directory);
    if (!checked) return std::unexpected(checked.error());
    std::vector<fs::path> entries;
    std::error_code error;
    fs::recursive_directory_iterator iterator(io_path(*checked), error);
    if (error) return std::unexpected(error);
    for (const auto end = fs::recursive_directory_iterator{}; iterator != end; iterator.increment(error))
    {
        if (error) return std::unexpected(error);
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
        const auto path = iterator->path();
        if (iterator->is_directory(error))
        {
            if (auto plain = guard_files::directory(path); !plain) return std::unexpected(plain.error());
        }
        else if (auto plain = guard_files::regular_file(path); !plain) return std::unexpected(plain.error());
        if (error) return std::unexpected(error);
        entries.push_back(path.lexically_relative(io_path(*checked)));
    }
    if (error) return std::unexpected(error);
    return entries;
}

std::wstring quote(std::wstring_view value)
{
    std::wstring result{L"\""};
    std::size_t slashes = 0;
    for (const auto character : value)
    {
        if (character == L'\\') { ++slashes; continue; }
        result.append(character == L'"' ? slashes * 2 + 1 : slashes, L'\\');
        slashes = 0;
        result.push_back(character);
    }
    result.append(slashes * 2, L'\\');
    result.push_back(L'"');
    return result;
}

struct Child
{
    guard::NativeHandle process, thread;
    ~Child()
    {
        if (process && WaitForSingleObject(process.get(), 0) != WAIT_OBJECT_0)
        {
            TerminateProcess(process.get(), ERROR_CANCELLED);
            WaitForSingleObject(process.get(), 5000);
        }
    }
};
struct Attributes
{
    LPPROC_THREAD_ATTRIBUTE_LIST value;
    ~Attributes() { DeleteProcThreadAttributeList(value); }
};
}

Result run_process(const fs::path& program, const std::vector<std::wstring>& arguments,
    const fs::path& working_directory, const fs::path& log, std::stop_token stop)
{
    if (stop.stop_requested()) return failure(std::errc::operation_canceled);
    if (std::ranges::any_of(arguments, [](const auto& value) { return value.find(L'\0') != std::wstring::npos; }))
        return failure(std::errc::invalid_argument);
    if (auto checked = guard_files::regular_file(program); !checked) return std::unexpected(checked.error());
    auto working = guard_files::directory(working_directory);
    if (!working) return std::unexpected(working.error());
    auto command = quote(program.native());
    for (const auto& argument : arguments) { command.push_back(L' '); command += quote(argument); }
    // Only these two standard handles are inherited. Other guard files/sockets
    // must not be kept alive in the extractor process.
    SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    guard::NativeHandle output{CreateFileW(io_path(log).c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        &security, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (output.get() == INVALID_HANDLE_VALUE) return native_failure();
    guard::NativeHandle input{CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (input.get() == INVALID_HANDLE_VALUE) return native_failure();
    SIZE_T bytes = 0;
    InitializeProcThreadAttributeList(nullptr, 1, 0, &bytes);
    std::vector<std::byte> storage(bytes);
    auto* list = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(storage.data());
    if (!InitializeProcThreadAttributeList(list, 1, 0, &bytes)) return native_failure();
    Attributes owned{list};
    std::array<HANDLE, 2> inherited{output.get(), input.get()};
    if (!UpdateProcThreadAttribute(list, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, inherited.data(),
        sizeof(inherited), nullptr, nullptr)) return native_failure();
    STARTUPINFOEXW startup{};
    startup.StartupInfo.cb = sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdOutput = startup.StartupInfo.hStdError = output.get();
    startup.StartupInfo.hStdInput = input.get();
    startup.lpAttributeList = list;
    PROCESS_INFORMATION information{};
    if (!CreateProcessW(program.c_str(), command.data(), nullptr, nullptr, TRUE,
        CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, nullptr, working->c_str(), &startup.StartupInfo, &information))
        return native_failure();
    Child child{guard::NativeHandle{information.hProcess}, guard::NativeHandle{information.hThread}};
    for (;;)
    {
        const auto waited = WaitForSingleObject(child.process.get(), 50);
        if (waited == WAIT_OBJECT_0) break;
        if (waited == WAIT_FAILED) return native_failure();
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
    }
    DWORD exit = 0;
    if (!GetExitCodeProcess(child.process.get(), &exit)) return native_failure();
    return exit;
}

Result version(Request request, std::stop_token stop, Extractor extractor)
try
{
    namespace fs = std::filesystem;
    if (!valid_name(request.name) || !guard_files::same_tag(fs::path(request.name).extension().string(), ".zip"))
        return failure(std::errc::invalid_argument);
    const auto stem = fs::path(request.name).stem().string();
    if (!guard_files::leaf_name(stem)) return failure(std::errc::invalid_argument);
    auto guard = guard_files::directory(request.guard);
    auto versions = guard_files::directory(request.versions);
    auto runtime = guard_files::directory(request.runtime);
    if (!guard) return std::unexpected(guard.error());
    if (!versions) return std::unexpected(versions.error());
    if (!runtime) return std::unexpected(runtime.error());
    if (beneath(*versions, *runtime) || beneath(*versions, *guard)) return failure(std::errc::invalid_argument);
    const auto archive = *versions / request.name;
    if (auto checked = guard_files::regular_file(archive); !checked) return std::unexpected(checked.error());
    for (const auto suffix : {"-area_config.lua", "-gmsecurityconfig.lua"})
        if (auto checked = guard_files::regular_file(*versions / (request.name + suffix)); !checked)
            return std::unexpected(checked.error());
    const auto log = *guard / "deploy_version.log";
    trace(log, "Starting staged version deployment: " + request.name);
    Stage stage;
    if (auto created = new_stage(stage, *versions); !created) return std::unexpected(created.error());
    Result extracted = 0;
    if (extractor) extracted = extractor(archive, stage.path, log, stop);
    // 7za 9.20 has its own long-path handling but treats an extended-prefix
    // archive argument as an archive pattern it cannot find. Keep argv ordinary.
    else extracted = run_process(*guard / "7z.exe", {L"x", L"-y", L"-o" + stage.path.native(), archive.native()}, *guard, log, stop);
    if (!extracted) return std::unexpected(extracted.error());
    if (*extracted != 0) return failure(std::errc::io_error);
    trace(log, "Extractor completed; validating package");
    const auto package = stage.path / stem;
    auto entries = tree(package, stop);
    if (!entries) return std::unexpected(entries.error());
    trace(log, "Package validated; installing staged configuration");
    for (const auto file : {"area_config.lua", "gmsecurityconfig.lua"})
        if (auto copied = replace_file(*versions / (request.name + "-" + file), package / "globalconfig" / file); !copied)
            return std::unexpected(copied.error());
    entries = tree(package, stop);
    if (!entries) return std::unexpected(entries.error());
    trace(log, "Configuration staged; checking runtime cleanup paths");
    constexpr std::array retired{"dbserver", "manageserver", "gameserver", "gateserver", "datalogserver",
        "loginserver", "balanceserver", "gmserver", "cache", "data", "globalconfig", "commonconfig",
        "gpkitsvr64.dll", "SdoBase.dll", "sotsapi"};
    for (const auto& relative : *entries)
    {
        if (excluded(relative)) continue;
        const auto destination = *runtime / relative;
        if (beneath(destination, *guard) || (beneath(*runtime, *guard) && beneath(*guard, destination)) ||
            beneath(destination, *versions) || beneath(*versions, destination))
            return failure(std::errc::invalid_argument);
        if (std::ranges::any_of(retired, [&](const auto name) { return guard_files::same_tag(relative.begin()->string(), name); })) continue;
        auto parent = destination.parent_path();
        for (;;)
        {
            auto checked = guard_files::directory(parent);
            if (checked) break;
            if (checked.error() != std::errc::no_such_file_or_directory) return std::unexpected(checked.error());
            parent = parent.parent_path();
        }
        std::error_code error;
        const auto existing = fs::symlink_status(io_path(destination), error);
        if (error == std::errc::no_such_file_or_directory) continue;
        if (error) return std::unexpected(error);
        if (fs::exists(existing))
        {
            const bool source_directory = fs::is_directory(io_path(package / relative));
            if (source_directory)
            {
                if (auto checked = guard_files::directory(destination); !checked) return std::unexpected(checked.error());
            }
            else if (auto checked = guard_files::regular_file(destination); !checked) return std::unexpected(checked.error());
        }
    }
    for (const auto name : retired)
    {
        const auto target = *runtime / name;
        if (beneath(target, *guard) || beneath(target, *versions)) return failure(std::errc::invalid_argument);
        std::error_code error;
        const auto status = fs::symlink_status(io_path(target), error);
        if (error == std::errc::no_such_file_or_directory) { error.clear(); continue; }
        if (fs::exists(status))
        {
            if (fs::is_directory(io_path(target), error))
            {
                if (auto checked = tree(target, stop); !checked) return std::unexpected(checked.error());
            }
            else if (auto checked = guard_files::regular_file(target); !checked) return std::unexpected(checked.error());
        }
        if (error) return std::unexpected(error);
    }
    trace(log, "Runtime preflight complete");
    // The old deployment retired these trees before overlaying the package. Keep
    // that behavior, but perform complete package/config preflight first.
    for (const auto name : retired)
        if (auto removed = guard_files::remove_entry(*runtime, name, stop); !removed) return removed;
    std::uintmax_t files = 0;
    for (const auto& relative : *entries)
    {
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
        if (excluded(relative)) continue;
        const auto destination = *runtime / relative;
        if (fs::is_directory(io_path(package / relative)))
        {
            if (auto created = guard_files::directory(destination, true); !created) return std::unexpected(created.error());
        }
        else
        {
            if (auto copied = replace_file(package / relative, destination); !copied) return std::unexpected(copied.error());
            ++files;
        }
    }
    if (auto removed = guard_files::remove_entry(*versions, stem, stop); !removed) return removed;
    fs::rename(io_path(package),io_path(*versions / stem));
    std::vector<fs::path> stale;
    for (const auto& entry : fs::directory_iterator(io_path(*versions)))
        if (display_path(entry.path()) != stage.path && !starts_with_name(entry.path().filename().string(), stem)) stale.push_back(entry.path().filename());
    for (const auto& relative : stale)
    {
        if (stop.stop_requested()) break;
        if (auto removed = guard_files::remove_entry(*versions, relative, stop); !removed)
            trace(log, "Cache cleanup warning: " + relative.string() + ": " + removed.error().message());
    }
    const auto completed = "Version deployment completed: " + request.name;
    for (const auto& path : {*runtime / "current_server_version.txt", *guard / "current_server_version.txt"})
        if (auto written = write_tag(path, request.name); !written) return std::unexpected(written.error());
    if (auto written = write_tag(*guard / "version.txt", request.name); !written) return std::unexpected(written.error());
    trace(log, completed);
    return files;
}
catch (const std::system_error& error) { return std::unexpected(error.code()); }
catch (const std::bad_alloc&) { return failure(std::errc::not_enough_memory); }

Result patch(Request request, std::stop_token stop)
try
{
    if (!valid_name(request.name)) return failure(std::errc::invalid_argument);
    auto guard = guard_files::directory(request.guard);
    auto patches = guard_files::directory(request.patches);
    auto runtime = guard_files::directory(request.runtime);
    if (!guard) return std::unexpected(guard.error());
    if (!patches) return std::unexpected(patches.error());
    if (!runtime) return std::unexpected(runtime.error());
    const auto source = *patches / request.name;
    if (auto checked = guard_files::regular_file(source); !checked) return std::unexpected(checked.error());
    auto data = guard_files::directory(*runtime / "data", true);
    if (!data) return std::unexpected(data.error());
    if (beneath(*data, *guard) || beneath(*data, *patches)) return failure(std::errc::invalid_argument);
    std::vector<fs::path> stale;
    for (const auto& entry : fs::directory_iterator(io_path(*data)))
    {
        const auto name = entry.path().filename().string();
        if ((starts_with_name(name, "commonconfig_patch") || starts_with_name(name, "globalconfig_patch")) &&
            guard_files::same_tag(entry.path().extension().string(), ".pak") && !guard_files::same_tag(name, request.name))
        {
            if (auto checked = guard_files::regular_file(entry.path()); !checked) return std::unexpected(checked.error());
            stale.push_back(entry.path().filename());
        }
    }
    if (stop.stop_requested()) return failure(std::errc::operation_canceled);
    if (auto copied = guard_files::copy_file(source, *data / request.name); !copied) return std::unexpected(copied.error());
    for (const auto& relative : stale)
        if (auto removed = guard_files::remove_entry(*data, relative, stop); !removed) return removed;
    for (const auto& path : {*runtime / "current_server_patch.txt", *guard / "current_server_patch.txt"})
        if (auto written = write_tag(path, request.name); !written) return std::unexpected(written.error());
    if (auto written = write_tag(*guard / "patch.txt", request.name); !written) return std::unexpected(written.error());
    trace(*guard / "hot_update_patch.log", "Patch installation completed: " + request.name);
    return 1;
}
catch (const std::system_error& error) { return std::unexpected(error.code()); }
catch (const std::bad_alloc&) { return failure(std::errc::not_enough_memory); }
}
