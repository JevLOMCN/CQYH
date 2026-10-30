#include "account_logout.h"

#include <charconv>
#include <cstdint>
#include <format>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace account_logout
{
namespace
{
std::string display(const std::filesystem::path& path)
{
    const auto bytes = path.u8string();
    return {reinterpret_cast<const char*>(bytes.data()), bytes.size()};
}

std::string_view token(std::string_view line)
{
    constexpr std::string_view whitespace = " \t\r\n\v\f";
    const auto begin = line.find_first_not_of(whitespace);
    if (begin == line.npos) return {};
    line.remove_prefix(begin);
    return line.substr(0, line.find_first_of(whitespace));
}

std::expected<std::vector<std::string>, std::string> read(const std::filesystem::path& path)
{
    std::ifstream stream{path, std::ios::binary};
    if (!stream) return std::unexpected(std::format("cannot open {}", display(path)));
    std::vector<std::string> lines;
    for (std::string line; std::getline(stream, line);)
    {
        if (lines.empty() && line.starts_with("\xEF\xBB\xBF")) line.erase(0, 3);
        if (line.find('\0') != line.npos)
            return std::unexpected(std::format("{}:{}: embedded zero byte", display(path), lines.size() + 1));
        lines.push_back(std::move(line));
    }
    if (stream.bad()) return std::unexpected(std::format("cannot read {}", display(path)));
    return lines;
}

std::expected<std::vector<std::uint32_t>, std::string> sessions(const std::filesystem::path& path)
{
    const auto lines = read(path);
    if (!lines) return std::unexpected(lines.error());
    std::vector<std::uint32_t> values;
    values.reserve(lines->size());
    for (const auto& line : *lines)
    {
        auto value = token(line);
        if (value.starts_with('+')) value.remove_prefix(1);
        if (value.starts_with("0x") || value.starts_with("0X")) value.remove_prefix(2);
        if (value.empty())
            return std::unexpected(std::format("{}:{}: invalid 32-bit hexadecimal session ID", display(path), values.size() + 1));
        std::uint32_t id = 0;
        const auto result = std::from_chars(value.data(), value.data() + value.size(), id, 16);
        if (result.ec != std::errc{} || result.ptr != value.data() + value.size())
            return std::unexpected(std::format("{}:{}: invalid 32-bit hexadecimal session ID", display(path), values.size() + 1));
        values.push_back(id);
    }
    return values;
}

std::expected<std::vector<std::string>, std::string> users(const std::filesystem::path& path)
{
    auto lines = read(path);
    if (!lines) return std::unexpected(lines.error());
    std::vector<std::string> values;
    values.reserve(lines->size());
    for (const auto& line : *lines)
    {
        const auto name = token(line);
        if (name.empty() || name.size() > 63)
            return std::unexpected(std::format("{}:{}: username must contain 1 to 63 bytes", display(path), values.size() + 1));
        values.emplace_back(name);
    }
    return values;
}
}

std::expected<Report, std::string> analyze(const std::filesystem::path& root)
{
    const auto ids = sessions(root / "db_file" / "session.txt");
    if (!ids) return std::unexpected(ids.error());
    const auto names = users(root / "db_file" / "user.txt");
    if (!names) return std::unexpected(names.error());
    if (ids->size() != names->size())
        return std::unexpected(std::format("session.txt/user.txt row count mismatch: {} versus {}", ids->size(), names->size()));
    const auto logoutIds = sessions(root / "db_file" / "logout.txt");
    if (!logoutIds) return std::unexpected(logoutIds.error());
    const auto loginNames = users(root / "dl_file" / "AccountLogin.txt");
    if (!loginNames) return std::unexpected(loginNames.error());

    std::unordered_map<std::string, std::uint32_t> byName;
    byName.reserve(names->size());
    for (std::size_t index = 0; index < names->size(); ++index)
        byName.try_emplace((*names)[index], (*ids)[index]); // Preserve first duplicate-name match.
    const std::unordered_set<std::uint32_t> loggedOut(logoutIds->begin(), logoutIds->end());
    Report report{ids->size(), names->size(), logoutIds->size(), loginNames->size(), {}};
    for (const auto& name : *loginNames)
    {
        const auto found = byName.find(name);
        const auto id = found == byName.end() ? 0 : found->second;
        if (id == 0)
            report.text += std::format("Can not find UserName : {} in db login file?\n", name);
        if (loggedOut.contains(id))
            report.text += std::format("Session : {:x}, UserName : {} Found!\n", id, name);
        else
            report.text += std::format("SessionID : {:x} in , UserName : {} in dl file, not found in db file?\n", id, name);
    }
    return report;
}
}
