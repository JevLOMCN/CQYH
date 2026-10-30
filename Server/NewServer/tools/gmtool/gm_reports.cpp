#include "gm_reports.h"
#include "error_def.h"
#include <atomic>
#include <chrono>
#include <cstring>
#include <format>
#include <fstream>
#include <mutex>
#include <limits>
#include <string_view>

namespace
{
std::mutex publication_mutex;
template<std::size_t N> bool valid(const char (&text)[N]) { return std::memchr(text, 0, N) != nullptr; }
struct Staging
{
    std::filesystem::path path;
    ~Staging() { std::error_code ignored; if (!path.empty()) std::filesystem::remove(path, ignored); }
};
template<class Write> std::expected<void, std::string> publish(const std::filesystem::path& path, Write write)
{
    static std::atomic_uint64_t sequence{};
    Staging staging;
    std::ofstream output;
    for (INT32 attempt = 0; attempt < 16; ++attempt)
    {
        auto candidate = path;
        candidate += std::format(".tmp.{}.{}", std::chrono::steady_clock::now().time_since_epoch().count(), sequence++);
        output.open(candidate, std::ios::out | std::ios::binary | std::ios::noreplace);
        if (output) { staging.path=std::move(candidate); break; }
        std::error_code error;
        if (!std::filesystem::exists(candidate, error)) return std::unexpected("cannot create report file");
        output.clear();
    }
    if (!output) return std::unexpected("cannot create report staging file");
    write(output);
    output.flush();
    const bool complete = static_cast<bool>(output);
    output.close();
    if (!complete || !output) return std::unexpected("cannot finish writing report");
    std::error_code error;
    {
        std::lock_guard lock(publication_mutex);
        std::filesystem::rename(staging.path, path, error);
    }
    if (error) return std::unexpected("cannot publish report file: " + error.message());
    staging.path.clear();
    return {};
}
}

std::filesystem::path gm::utf8_path(std::string_view text)
{
    return std::filesystem::path(std::u8string(text.begin(), text.end()));
}
std::expected<void,std::string> gm::write_text_report(const std::filesystem::path& path,std::string_view bytes)
{
    if (bytes.size() > static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)()))
        return std::unexpected("report text is too large");
    return publish(path,[&](auto& output)
    {
        if (!bytes.empty()) output.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
    });
}

std::expected<void, std::string> gm::write_name_report(const std::filesystem::path& path,
    const std::map<INT32, ROLE_NAME_DATA>& names)
{
    for (const auto& [index, name] : names)
        if (!valid(name.szRoleName)) return std::unexpected("invalid role name in report");
    return publish(path, [&](auto& output)
    {
        for (const auto& [index, name] : names)
            output << std::format("{}  {}\r\n", name.szRoleName, name.dwRoleID);
    });
}

std::expected<void, std::string> gm::write_role_report(const std::filesystem::path& input,
    INT32 kind, const std::list<ROLE_RESULT>& roles)
{
    std::string_view suffix;
    switch (kind)
    {
    case CMD_FORBIDDEN_CHAT: suffix="_chat_result"; break;
    case CMD_FREEZE_ROLE: suffix="_freeze_result"; break;
    case CMD_KICK_ROLE: suffix="_kickrole_result"; break;
    case CMD_SEND_MAIL: suffix="_sendmail_result"; break;
    default: return std::unexpected("unsupported role report type");
    }
    for (const auto& role : roles)
        if (!valid(role.szRoleName)) return std::unexpected("invalid role name in report");
    auto path=input;
    path+=suffix;
    return publish(path, [&](auto& output)
    {
        output << "\xEF\xBB\xBFRoleName\tRoleID\tResult \r\n";
        for (const auto& role : roles)
            output << std::format("{}\t{}\t{} \r\n", role.szRoleName, role.dwRoleID, role.nResult == errSuccess ? "success" : "failed");
    });
}
