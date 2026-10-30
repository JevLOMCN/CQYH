#include "gm_role_files.h"
#include "error_def.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <fstream>
#include <limits>

namespace
{
std::string_view without_bom(std::string_view text)
{
    if (text.starts_with("\xEF\xBB\xBF")) text.remove_prefix(3);
    return text;
}
std::expected<std::string, std::string> read(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) return std::unexpected("cannot open role input file");
    std::string text;
    std::array<char, 64 * 1024> block;
    while (input.read(block.data(), block.size()) || input.gcount() != 0)
    {
        const auto count = static_cast<std::size_t>(input.gcount());
        if (count > gm::max_role_file_size - text.size())
            return std::unexpected("role input file exceeds 10 MiB");
        text.append(block.data(), count);
    }
    if (!input.eof() || input.bad()) return std::unexpected("cannot read role input file");
    return text;
}
}

std::expected<std::vector<std::string>, std::string> gm::parse_role_names(std::string_view input)
{
    auto text = without_bom(input);
    std::vector<std::string> names;
    std::size_t line = 0;
    while (!text.empty())
    {
        ++line;
        const auto end = text.find_first_of("\r\n");
        const auto name = text.substr(0, end);
        if (name.find('\0') != std::string_view::npos)
            return std::unexpected(std::format("NUL in role name on line {}", line));
        if (name.size() >= ROLE_NAME_LEN)
            return std::unexpected(std::format("role name too long on line {}", line));
        if (!name.empty()) names.emplace_back(name);
        if (end == std::string_view::npos) break;
        const auto newline = text[end];
        text.remove_prefix(end + 1);
        if (newline == '\r' && text.starts_with('\n')) text.remove_prefix(1);
    }
    if (names.empty()) return std::unexpected("role input contains no names");
    return names;
}

std::expected<std::list<UINT32>, std::string> gm::parse_role_ids(std::string_view input)
{
    auto text = without_bom(input);
    constexpr std::string_view spaces = " \t\r\n\v\f";
    std::list<UINT32> ids;
    while (!text.empty())
    {
        const auto first = text.find_first_not_of(spaces);
        if (first == std::string_view::npos) break;
        text.remove_prefix(first);
        const auto end = text.find_first_of(spaces);
        const auto token = text.substr(0, end);
        UINT32 value{};
        const auto parsed = std::from_chars(token.data(), token.data() + token.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr != token.data() + token.size())
            return std::unexpected("invalid or out-of-range role ID");
        ids.push_back(value);
        if (end == std::string_view::npos) break;
        text.remove_prefix(end);
    }
    if (ids.empty()) return std::unexpected("role input contains no IDs");
    return ids;
}

std::expected<std::list<ROLE_RESULT>, std::string> gm::read_role_batch(const std::filesystem::path& path)
{
    auto text = read(path);
    if (!text) return std::unexpected(std::move(text.error()));
    auto names = parse_role_names(*text);
    if (!names) return std::unexpected(std::move(names.error()));
    std::list<ROLE_RESULT> result;
    for (const auto& name : *names)
    {
        ROLE_RESULT role{};
        std::ranges::copy(name, role.szRoleName);
        role.nResult = errUnknownError;
        result.push_back(role);
    }
    return result;
}

std::expected<std::map<INT32, ROLE_NAME_DATA>, std::string> gm::read_name_queries(const std::filesystem::path& path)
{
    auto text = read(path);
    if (!text) return std::unexpected(std::move(text.error()));
    auto names = parse_role_names(*text);
    if (!names) return std::unexpected(std::move(names.error()));
    if (names->size() > (std::numeric_limits<INT32>::max)()) return std::unexpected("too many role names");
    std::map<INT32, ROLE_NAME_DATA> result;
    for (const auto& name : *names)
    {
        ROLE_NAME_DATA role{};
        std::ranges::copy(name, role.szRoleName);
        result.emplace(static_cast<INT32>(result.size() + 1), role);
    }
    return result;
}

std::expected<std::list<UINT32>, std::string> gm::read_role_ids(const std::filesystem::path& path)
{
    auto text = read(path);
    if (!text) return std::unexpected(std::move(text.error()));
    return parse_role_ids(*text);
}
