#include "guard_files.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <fstream>
#include <new>
#include <ranges>

#ifdef _WIN32
#include <windows.h>
#endif

namespace guard_files
{
namespace
{
namespace fs = std::filesystem;
std::unexpected<std::error_code> failure(std::errc value)
{
    return std::unexpected(std::make_error_code(value));
}

// std::filesystem's symlink classification alone does not cover every Windows
// reparse point. Inspect every component, not just the final directory entry.
std::expected<void, std::error_code> plain_path(const fs::path& absolute)
{
    const auto normalized = display_path(absolute);
    fs::path component;
    for (const auto& part : normalized)
    {
        component /= part;
        if (component == normalized.root_name() || (normalized.root_name().native().starts_with(fs::path{"\\\\"}.native()) && component == normalized.root_path())) continue;
#ifdef _WIN32
        auto native = component.native();
        if (!native.starts_with(L"\\\\?\\"))
            native = native.starts_with(L"\\\\") ? L"\\\\?\\UNC\\" + native.substr(2) : L"\\\\?\\" + native;
        const auto attributes = GetFileAttributesW(native.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES)
            return std::unexpected(std::error_code(GetLastError(), std::system_category()));
        if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
            return failure(std::errc::too_many_symbolic_link_levels);
#else
        std::error_code error;
        const auto status = fs::symlink_status(io_path(component), error);
        if (error) return std::unexpected(error);
        if (fs::is_symlink(status)) return failure(std::errc::too_many_symbolic_link_levels);
        if (!fs::exists(status)) return failure(std::errc::no_such_file_or_directory);
#endif
    }
    return {};
}

std::expected<fs::path, std::error_code> log_root(const fs::path& run_directory)
{
    if (run_directory.empty()) return failure(std::errc::invalid_argument);
    std::error_code error;
    auto root = display_path(fs::absolute(run_directory / "log", error).lexically_normal());
    if (error) return std::unexpected(error);
    if (auto checked = plain_path(root); !checked) return std::unexpected(checked.error());
    if (!fs::is_directory(io_path(root), error))
        return std::unexpected(error ? error : std::make_error_code(std::errc::not_a_directory));
    return root;
}

bool within(const fs::path& root, const fs::path& entry)
{
    const auto relative = entry.lexically_relative(root);
    return !relative.empty() && relative != "." && !relative.is_absolute()
        && *relative.begin() != "..";
}

bool whitespace(char value) noexcept
{
    return value == ' ' || value == '\t' || value == '\r' || value == '\n';
}
}

std::filesystem::path display_path(const std::filesystem::path& path)
{
#ifdef _WIN32
    const auto& value = path.native();
    if (value.starts_with(L"\\\\?\\UNC\\")) return std::filesystem::path{L"\\\\" + value.substr(8)};
    if (value.starts_with(L"\\\\?\\")) return std::filesystem::path{value.substr(4)};
#endif
    return path;
}

std::filesystem::path io_path(const std::filesystem::path& path)
{
#ifdef _WIN32
    if (path.empty()) return path;
    const auto value = std::filesystem::absolute(path).lexically_normal().native();
    if (value.starts_with(L"\\\\?\\")) return path;
    return std::filesystem::path{value.starts_with(L"\\\\")
        ? L"\\\\?\\UNC\\" + value.substr(2) : L"\\\\?\\" + value};
#else
    return path;
#endif
}

bool is_log_date(std::string_view name) noexcept
{
    if (name.size() != 10 || name[4] != '_' || name[7] != '_') return false;
    const auto number = [](std::string_view text) -> int {
        if (!std::ranges::all_of(text, [](char value) { return value >= '0' && value <= '9'; })) return -1;
        int result = 0;
        const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
        return parsed.ec == std::errc{} ? result : -1;
    };
    const auto year = number(name.substr(0, 4));
    const auto month = number(name.substr(5, 2));
    const auto day = number(name.substr(8, 2));
    return year > 0 && month > 0 && day > 0 && std::chrono::year_month_day{
        std::chrono::year{year}, std::chrono::month{static_cast<unsigned>(month)},
        std::chrono::day{static_cast<unsigned>(day)}}.ok();
}

std::expected<std::vector<std::string>, std::error_code> log_folders(const fs::path& run_directory)
{
    auto root = log_root(run_directory);
    if (!root) return std::unexpected(root.error());
    std::error_code error;
    fs::directory_iterator iterator(io_path(*root), error);
    if (error) return std::unexpected(error);
    std::vector<std::string> names;
    for (const auto end = fs::directory_iterator{}; iterator != end; iterator.increment(error))
    {
        if (error) return std::unexpected(error);
        const auto name = iterator->path().filename().string();
        if (!is_log_date(name)) continue;
        if (auto checked = plain_path(iterator->path()); !checked)
        {
            if (checked.error() == std::errc::too_many_symbolic_link_levels) continue;
            return std::unexpected(checked.error());
        }
        if (iterator->is_directory(error)) names.push_back(name);
        if (error) return std::unexpected(error);
    }
    if (error) return std::unexpected(error);
    std::ranges::sort(names);
    return names;
}

Result erase_log_folder(const fs::path& run_directory, std::string_view date, std::stop_token stop)
{
    if (!is_log_date(date)) return failure(std::errc::invalid_argument);
    auto root = log_root(run_directory);
    if (!root) return std::unexpected(root.error());
    const auto target = *root / date;
    const auto marker = *root / (std::string(date) + "_cleard_by_guard");
    // These are direct children derived from a strict date, and the resolved
    // absolute boundaries are checked before any traversal or deletion.
    if (!within(*root, target) || target.parent_path() != *root)
        return failure(std::errc::permission_denied);
    if (auto checked = plain_path(target); !checked) return std::unexpected(checked.error());
    std::error_code error;
    if (!fs::is_directory(io_path(target), error))
        return std::unexpected(error ? error : std::make_error_code(std::errc::not_a_directory));
    const auto marker_status = fs::symlink_status(io_path(marker), error);
    if (error && error != std::errc::no_such_file_or_directory) return std::unexpected(error);
    error.clear();
    if (fs::exists(marker_status))
    {
        if (auto checked = plain_path(marker); !checked) return std::unexpected(checked.error());
        if (!fs::is_directory(marker_status)) return failure(std::errc::file_exists);
    }

    // Preflight the complete tree. Never follow links or delete a reparse point,
    // even if another entry was already enumerated. Reverse order removes leaves
    // before their parents without recursive calls or remove_all following a
    // directory that has been replaced since enumeration.
    std::vector<fs::path> entries{target};
    for (std::size_t index = 0; index < entries.size(); ++index)
    {
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
        if (auto checked = plain_path(entries[index]); !checked) return std::unexpected(checked.error());
        if (!fs::is_directory(io_path(entries[index]), error))
        {
            if (error) return std::unexpected(error);
            continue;
        }
        fs::directory_iterator iterator(io_path(entries[index]), error);
        if (error) return std::unexpected(error);
        for (const auto end = fs::directory_iterator{}; iterator != end; iterator.increment(error))
        {
            if (stop.stop_requested()) return failure(std::errc::operation_canceled);
            if (error) return std::unexpected(error);
            const auto entry = display_path(iterator->path());
            if (!within(target, entry)) return failure(std::errc::permission_denied);
            if (auto checked = plain_path(entry); !checked) return std::unexpected(checked.error());
            entries.push_back(entry);
        }
        if (error) return std::unexpected(error);
    }
    std::uintmax_t removed = 0;
    for (const auto& entry : entries | std::views::reverse)
    {
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
        if (!within(*root, entry)) return failure(std::errc::permission_denied);
        if (auto checked = plain_path(entry); !checked) return std::unexpected(checked.error());
        if (!fs::remove(io_path(entry), error))
            return std::unexpected(error ? error : std::make_error_code(std::errc::io_error));
        ++removed;
    }
    if (auto checked = plain_path(*root); !checked) return std::unexpected(checked.error());
    // An existing marker must also remain an ordinary directory.
    if (!fs::create_directory(io_path(marker), error))
    {
        if (error) return std::unexpected(error);
        if (auto checked = plain_path(marker); !checked) return std::unexpected(checked.error());
        if (!fs::is_directory(io_path(marker), error))
            return std::unexpected(error ? error : std::make_error_code(std::errc::not_a_directory));
    }
    return removed;
}

std::expected<std::optional<std::string>, std::error_code> read_tag(const fs::path& path, std::size_t limit)
{
    std::error_code error;
    const auto status = fs::status(io_path(path), error);
    if (error == std::errc::no_such_file_or_directory || (!error && !fs::exists(status)))
        return std::optional<std::string>{};
    if (error) return std::unexpected(error);
    if (!fs::is_regular_file(status)) return failure(std::errc::invalid_argument);
    errno = 0;
    std::ifstream file(io_path(path), std::ios::binary);
    if (!file) return std::unexpected(errno ? std::error_code(errno, std::generic_category())
        : std::make_error_code(std::errc::io_error));
    std::string tag;
    bool trailing = false;
    char value = 0;
    std::size_t bytes = 0;
    while (file.get(value))
    {
        if (++bytes > 4096) return failure(std::errc::value_too_large);
        if (whitespace(value)) { trailing = true; continue; }
        if (trailing || value == '\0') return failure(std::errc::invalid_argument);
        if (tag.size() >= limit) return failure(std::errc::value_too_large);
        tag.push_back(value);
    }
    if (file.bad() || !file.eof()) return failure(std::errc::io_error);
    if (tag.empty()) return failure(std::errc::invalid_argument);
    return std::optional{std::move(tag)};
}

bool same_tag(std::string_view left, std::string_view right) noexcept
{
    const auto lower = [](unsigned char value) {
        return value >= 'A' && value <= 'Z' ? value + ('a' - 'A') : value;
    };
    return std::ranges::equal(left, right, {}, lower, lower);
}

std::expected<std::optional<std::string>, std::error_code> consume_tag(const fs::path& path, std::size_t limit)
{
    std::error_code error;
    const auto status = fs::symlink_status(io_path(path), error);
    if (error == std::errc::no_such_file_or_directory || (!error && !fs::exists(status)))
        return std::optional<std::string>{};
    if (error) return std::unexpected(error);
    if (!fs::is_regular_file(status)) return failure(std::errc::invalid_argument);
    const auto absolute = display_path(fs::absolute(path, error).lexically_normal());
    if (error) return std::unexpected(error);
    if (auto checked = plain_path(absolute); !checked) return std::unexpected(checked.error());

    struct Claim
    {
        fs::path directory;
        fs::path file;
        ~Claim()
        {
            // Only the two exact paths in the exclusively created directory.
            // Never recursively remove an arbitrary result-file parent.
            try
            {
                std::error_code ignored;
                fs::remove(io_path(file), ignored);
                fs::remove(io_path(directory), ignored);
            }
            catch (...) {}
        }
    };
    static std::atomic<std::uint64_t> sequence{0};
    fs::path directory;
    bool created = false;
    for (int attempt = 0; attempt < 100 && !created; ++attempt)
    {
        directory = absolute.parent_path() / (".guard-result-" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
            std::to_string(sequence.fetch_add(1, std::memory_order_relaxed)));
        created = fs::create_directory(io_path(directory), error);
        if (error && error != std::errc::file_exists) return std::unexpected(error);
        error.clear();
    }
    if (!created) return failure(std::errc::file_exists);
    Claim claim{std::move(directory), {}};
    claim.file = claim.directory / "result";
    fs::rename(io_path(absolute),io_path(claim.file), error);
    if (error)
    {
#ifdef _WIN32
        // cmd.exe's redirected echo keeps a non-delete-sharing writer handle
        // until the complete line has been written. Do not consume a partial
        // line or turn this normal, brief sharing conflict into an operation failure.
        if (error == std::error_code(ERROR_SHARING_VIOLATION, std::system_category()) ||
            error == std::error_code(ERROR_LOCK_VIOLATION, std::system_category()))
            return std::optional<std::string>{};
#endif
        if (error == std::errc::no_such_file_or_directory) return std::optional<std::string>{};
        return std::unexpected(error);
    }
    auto tag = read_tag(claim.file, limit);
    if (!fs::remove(io_path(claim.file), error) || error)
        return std::unexpected(error ? error : std::make_error_code(std::errc::io_error));
    return tag;
}

bool leaf_name(std::string_view name) noexcept
{
    if (name.empty() || name.size() > 255 || name == "." || name == ".." ||
        name.back() == '.' || name.back() == ' ') return false;
    for (const unsigned char value : name)
        if (value < 32 || value == 127 || std::string_view{"<>:\"/\\|?*"}.find(value) != std::string_view::npos)
            return false;
    const auto stem = name.substr(0, name.find('.'));
    for (const auto reserved : {"CON", "PRN", "AUX", "NUL", "CONIN$", "CONOUT$"})
        if (same_tag(stem, reserved)) return false;
    if (stem.size() == 4 && (same_tag(stem.substr(0, 3), "COM") || same_tag(stem.substr(0, 3), "LPT")) &&
        stem[3] >= '1' && stem[3] <= '9') return false;
    return true;
}

std::expected<fs::path, std::error_code> directory(const fs::path& path, bool create)
{
    if (path.empty()) return failure(std::errc::invalid_argument);
    std::error_code error;
    auto absolute = display_path(fs::absolute(path, error).lexically_normal());
    if (error) return std::unexpected(error);
    if (absolute.filename().empty() && absolute != absolute.root_path()) absolute = absolute.parent_path();
    fs::path current;
    for (const auto& part : absolute)
    {
        current /= part;
        if (current == absolute.root_name() || (absolute.root_name().native().starts_with(fs::path{"\\\\"}.native()) && current == absolute.root_path())) continue;
        auto status = fs::symlink_status(io_path(current), error);
        if (create && (error == std::errc::no_such_file_or_directory || (!error && !fs::exists(status))))
        {
            error.clear();
            fs::create_directory(io_path(current), error);
            if (error) return std::unexpected(error);
        }
        else if (error) return std::unexpected(error);
        if (auto checked = plain_path(current); !checked) return std::unexpected(checked.error());
        if (!fs::is_directory(io_path(current), error))
            return std::unexpected(error ? error : std::make_error_code(std::errc::not_a_directory));
    }
    return absolute;
}

namespace
{
std::expected<void, std::error_code> ordinary_destination(const fs::path& destination)
{
    std::error_code error;
    const auto status = fs::symlink_status(io_path(destination), error);
    if (error == std::errc::no_such_file_or_directory || (!error && !fs::exists(status))) return {};
    if (error) return std::unexpected(error);
    if (auto checked = plain_path(destination); !checked) return std::unexpected(checked.error());
    if (!fs::is_regular_file(status)) return failure(std::errc::invalid_argument);
    return {};
}

std::expected<fs::path, std::error_code> temporary_directory(const fs::path& root)
{
    static std::atomic<std::uint64_t> sequence{0};
    for (int attempt = 0; attempt < 100; ++attempt)
    {
        auto path = root / (".guard-upload-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
            std::to_string(sequence.fetch_add(1, std::memory_order_relaxed)));
        std::error_code error;
        if (fs::create_directory(io_path(path), error)) return path;
        if (error && error != std::errc::file_exists) return std::unexpected(error);
    }
    return failure(std::errc::file_exists);
}

struct TemporaryDirectory
{
    fs::path path;
    bool retain_backup = false;
    ~TemporaryDirectory()
    {
        try
        {
            std::error_code error;
            fs::remove(io_path(path / "content"), error);
            if (!retain_backup)
            {
                fs::remove(io_path(path / "previous"), error);
                fs::remove(io_path(path), error);
            }
        }
        catch (...) {}
    }
};
}

std::expected<void, std::error_code> copy_file(const fs::path& source, const fs::path& destination,
    std::move_only_function<bool()> validate)
{
    if (!leaf_name(destination.filename().string())) return failure(std::errc::invalid_argument);
    auto parent = directory(destination.parent_path(), true);
    if (!parent) return std::unexpected(parent.error());
    const auto target = *parent / destination.filename();
    if (auto checked = ordinary_destination(target); !checked) return checked;
    auto temporary = temporary_directory(*parent);
    if (!temporary) return std::unexpected(temporary.error());
    TemporaryDirectory owned{std::move(*temporary)};
    std::error_code error;
    fs::copy_file(io_path(source),io_path(owned.path / "content"), fs::copy_options::none, error);
    if (error) return std::unexpected(error);
    if (auto checked = ordinary_destination(target); !checked) return checked;
    const bool previous = fs::exists(io_path(target), error);
    if (error) return std::unexpected(error);
    if (validate && previous)
    {
        fs::copy_file(io_path(target),io_path(owned.path / "previous"), fs::copy_options::none, error);
        if (error) return std::unexpected(error);
    }
    fs::rename(io_path(owned.path / "content"),io_path(target), error);
    if (error) return std::unexpected(error);
    if (validate)
    {
        bool accepted = false;
        try { accepted = validate(); }
        catch (...) {} // A failed/throwing validation must restore the installed file.
        if (!accepted)
        {
            if (previous) fs::rename(io_path(owned.path / "previous"),io_path(target), error);
            else fs::remove(io_path(target), error);
            // Preserve a recoverable backup if an external lock prevents rollback.
            if (error) { owned.retain_backup = previous; return std::unexpected(error); }
            return failure(std::errc::invalid_argument);
        }
    }
    return {};
}

std::expected<void, std::error_code> regular_file(const fs::path& path)
{
    std::error_code error;
    const auto absolute = display_path(fs::absolute(path, error).lexically_normal());
    if (error) return std::unexpected(error);
    if (auto checked = plain_path(absolute); !checked) return checked;
    if (!fs::is_regular_file(io_path(absolute), error))
        return std::unexpected(error ? error : std::make_error_code(std::errc::invalid_argument));
    return {};
}

Result remove_entry(const fs::path& root, const fs::path& relative, std::stop_token stop)
{
    if (relative.empty() || relative.is_absolute() || relative.has_root_name()) return failure(std::errc::invalid_argument);
    for (const auto& part : relative)
        if (part == "." || part == "..") return failure(std::errc::invalid_argument);
    auto parent = directory(root);
    if (!parent) return std::unexpected(parent.error());
    const auto target = *parent / relative;
    if (!within(*parent, target)) return failure(std::errc::permission_denied);
    std::error_code error;
    const auto status = fs::symlink_status(io_path(target), error);
    if (error == std::errc::no_such_file_or_directory || (!error && !fs::exists(status))) return 0;
    if (error) return std::unexpected(error);
    std::vector<fs::path> entries{target};
    for (std::size_t index = 0; index < entries.size(); ++index)
    {
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
        if (auto checked = plain_path(entries[index]); !checked) return std::unexpected(checked.error());
        if (!fs::is_directory(io_path(entries[index]), error))
        {
            if (error) return std::unexpected(error);
            continue;
        }
        fs::directory_iterator iterator(io_path(entries[index]), error);
        if (error) return std::unexpected(error);
        for (const auto end = fs::directory_iterator{}; iterator != end; iterator.increment(error))
        {
            if (error) return std::unexpected(error);
            if (stop.stop_requested()) return failure(std::errc::operation_canceled);
            entries.push_back(display_path(iterator->path()));
        }
        if (error) return std::unexpected(error);
    }
    std::uintmax_t removed = 0;
    for (const auto& entry : entries | std::views::reverse)
    {
        if (stop.stop_requested()) return failure(std::errc::operation_canceled);
        // Check the resolved absolute boundary and every component again before
        // removing each leaf; never recursively delete through a reparse point.
        if (!within(*parent, entry)) return failure(std::errc::permission_denied);
        if (auto checked = plain_path(entry); !checked) return std::unexpected(checked.error());
        fs::permissions(io_path(entry), fs::perms::owner_write, fs::perm_options::add, error);
        if (error) return std::unexpected(error);
        if (!fs::remove(io_path(entry), error)) return std::unexpected(error ? error : std::make_error_code(std::errc::io_error));
        ++removed;
    }
    return removed;
}

IncomingFile::~IncomingFile() { abort(); }

void IncomingFile::abort(const void* owner) noexcept
{
    if (!m_transfer || (owner && owner != m_transfer->owner)) return;
    try
    {
        auto& transfer = *m_transfer;
        if (transfer.stream.is_open()) transfer.stream.close();
        std::error_code error;
        if (!transfer.temporary_file.empty()) fs::remove(io_path(transfer.temporary_file), error);
        if (!transfer.temporary_directory.empty()) fs::remove(io_path(transfer.temporary_directory), error);
    }
    catch (...) {}
    m_transfer.reset();
}

std::expected<std::optional<UploadedFile>, std::error_code> IncomingFile::receive(
    const fs::path& root, const UploadChunk& chunk, const void* owner)
{
    if (m_transfer && owner != m_transfer->owner) return failure(std::errc::device_or_resource_busy);
    const auto failed = [this, owner](std::error_code error)
        -> std::expected<std::optional<UploadedFile>, std::error_code> {
        abort(owner); return std::unexpected(error);
    };
    try
    {
        if (!owner || !leaf_name(chunk.name) || chunk.name.size() > 127 || chunk.bytes.size() > 32768 ||
            chunk.file_total == 0 || chunk.file_index >= chunk.file_total ||
            chunk.buffer_total == 0 || chunk.buffer_index >= chunk.buffer_total)
            return failed(std::make_error_code(std::errc::invalid_argument));
        auto checked_root = directory(root, true);
        if (!checked_root) return failed(checked_root.error());
        if (chunk.buffer_index == 0)
        {
            abort(owner);
            m_transfer.emplace();
            auto& transfer = *m_transfer;
            transfer.owner = owner;
            transfer.root = *checked_root;
            transfer.name = chunk.name;
            transfer.destination = transfer.root / transfer.name;
            transfer.type = chunk.type;
            transfer.file_index = chunk.file_index;
            transfer.file_total = chunk.file_total;
            transfer.buffer_total = chunk.buffer_total;
            if (auto checked = ordinary_destination(transfer.destination); !checked) return failed(checked.error());
            auto temporary = temporary_directory(transfer.root);
            if (!temporary) return failed(temporary.error());
            transfer.temporary_directory = std::move(*temporary);
            transfer.temporary_file = transfer.temporary_directory / "content";
            transfer.stream.open(io_path(transfer.temporary_file), std::ios::binary | std::ios::out | std::ios::noreplace);
            if (!transfer.stream) return failed(std::make_error_code(std::errc::io_error));
        }
        if (!m_transfer) return failed(std::make_error_code(std::errc::invalid_argument));
        auto& transfer = *m_transfer;
        if (transfer.root != *checked_root || transfer.name != chunk.name || transfer.type != chunk.type ||
            transfer.file_index != chunk.file_index || transfer.file_total != chunk.file_total ||
            transfer.buffer_total != chunk.buffer_total || transfer.next_buffer != chunk.buffer_index)
            return failed(std::make_error_code(std::errc::invalid_argument));
        if (!chunk.bytes.empty())
            transfer.stream.write(chunk.bytes.data(), static_cast<std::streamsize>(chunk.bytes.size()));
        if (!transfer.stream) return failed(std::make_error_code(std::errc::io_error));
        ++transfer.next_buffer;
        if (transfer.next_buffer != transfer.buffer_total) return std::optional<UploadedFile>{};
        transfer.stream.close();
        if (transfer.stream.fail()) return failed(std::make_error_code(std::errc::io_error));
        if (auto checked = directory(transfer.root); !checked) return failed(checked.error());
        if (auto checked = ordinary_destination(transfer.destination); !checked) return failed(checked.error());
        // Construct the completion before publishing the new file, so allocation
        // failure leaves the existing destination intact.
        UploadedFile uploaded{transfer.destination, transfer.type};
        std::error_code error;
        fs::rename(io_path(transfer.temporary_file),io_path(transfer.destination), error);
        if (error) return failed(error);
        abort(owner);
        return std::optional{std::move(uploaded)};
    }
    catch (const std::system_error& error) { return failed(error.code()); }
    catch (const std::bad_alloc&) { return failed(std::make_error_code(std::errc::not_enough_memory)); }
}

FileTask::~FileTask() { stop(); }

bool FileTask::start(fs::path path, Action action)
{
    if (busy() || !action) return false;
    m_worker = std::jthread([this, path = std::move(path), action = std::move(action)](std::stop_token stop) mutable {
        Result result = failure(std::errc::io_error);
        try { result = action(stop); }
        catch (const std::system_error& error) { result = std::unexpected(error.code()); }
        catch (const std::bad_alloc&) { result = failure(std::errc::not_enough_memory); }
        catch (...) { result = failure(std::errc::io_error); }
        std::lock_guard lock(m_mutex);
        m_completion.emplace(Completion{std::move(path), result});
    });
    return true;
}

std::optional<FileTask::Completion> FileTask::poll()
{
    std::optional<Completion> completion;
    {
        std::lock_guard lock(m_mutex);
        completion.swap(m_completion);
    }
    if (completion) m_worker.join();
    return completion;
}

void FileTask::stop()
{
    if (m_worker.joinable()) { m_worker.request_stop(); m_worker.join(); }
    std::lock_guard lock(m_mutex);
    m_completion.reset();
}
}
