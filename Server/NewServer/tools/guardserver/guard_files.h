#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <functional>
#include <fstream>
#include <span>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>

namespace guard_files
{
using Result = std::expected<std::uintmax_t, std::error_code>;
// Extended-length Windows IO paths keep staging depth independent of MAX_PATH.
// Stored/configuration paths retain their ordinary spelling.
std::filesystem::path io_path(const std::filesystem::path& path);
std::filesystem::path display_path(const std::filesystem::path& path);

bool is_log_date(std::string_view name) noexcept;
std::expected<std::vector<std::string>, std::error_code>
    log_folders(const std::filesystem::path& run_directory);
// Deletes only a validated date directory beneath run_directory/log. Reparse
// points are refused, including Windows junctions. Cancellation can leave a
// partially removed directory; the completion marker is written only on success.
Result erase_log_folder(const std::filesystem::path& run_directory,
    std::string_view date, std::stop_token stop = {});

// Missing files are pending. Malformed, empty and oversized tags are errors.
// Reads require no write permission and accept the whitespace emitted by echo.
std::expected<std::optional<std::string>, std::error_code>
    read_tag(const std::filesystem::path& path, std::size_t limit = 127);
// Claim an operation result before reading it. A Windows batch writer still
// holding the file open leaves it pending; malformed completed results are
// consumed once. Temporary claim directories are exclusively created siblings.
std::expected<std::optional<std::string>, std::error_code>
    consume_tag(const std::filesystem::path& path, std::size_t limit = 127);
bool same_tag(std::string_view left, std::string_view right) noexcept;

bool leaf_name(std::string_view name) noexcept;
std::expected<std::filesystem::path, std::error_code>
    directory(const std::filesystem::path& path, bool create = false);
std::expected<void, std::error_code> copy_file(const std::filesystem::path& source,
    const std::filesystem::path& destination, std::move_only_function<bool()> validate = {});
std::expected<void, std::error_code> regular_file(const std::filesystem::path& path);
Result remove_entry(const std::filesystem::path& root, const std::filesystem::path& relative,
    std::stop_token stop = {});

struct UploadChunk
{
    std::string_view name;
    int type;
    std::uint16_t file_index, file_total, buffer_index, buffer_total;
    std::span<const char> bytes;
};
struct UploadedFile
{
    std::filesystem::path path;
    int type;
};

class IncomingFile
{
public:
    IncomingFile() = default;
    ~IncomingFile();
    IncomingFile(const IncomingFile&) = delete;
    IncomingFile& operator=(const IncomingFile&) = delete;
    std::expected<std::optional<UploadedFile>, std::error_code> receive(
        const std::filesystem::path& root, const UploadChunk& chunk, const void* owner);
    void abort(const void* owner = nullptr) noexcept;
    bool busy() const noexcept { return m_transfer.has_value(); }

private:
    struct Transfer
    {
        const void* owner = nullptr;
        std::filesystem::path root, temporary_directory, temporary_file, destination;
        std::string name;
        int type = 0;
        std::uint16_t file_index = 0, file_total = 0, buffer_total = 0;
        std::uint32_t next_buffer = 0;
        std::ofstream stream;
    };
    std::optional<Transfer> m_transfer;
};

// One main-thread-owned task. Worker captures must own their paths/data and must
// not access server globals, network sessions or logging. Completion is consumed
// on the main thread; destruction requests cancellation and joins the worker.
class FileTask
{
public:
    using Action = std::move_only_function<Result(std::stop_token)>;
    struct Completion
    {
        std::filesystem::path path;
        Result result;
    };

    FileTask() = default;
    ~FileTask();
    FileTask(const FileTask&) = delete;
    FileTask& operator=(const FileTask&) = delete;
    bool start(std::filesystem::path path, Action action);
    bool busy() const noexcept { return m_worker.joinable(); }
    std::optional<Completion> poll();
    void stop();

private:
    std::mutex m_mutex;
    std::optional<Completion> m_completion;
    std::jthread m_worker;
};
}
