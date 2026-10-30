#pragma once

#include <bit>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace maintain
{
inline std::optional<std::uint32_t> parse_address(std::string_view remaining)
{
    std::uint32_t result = 0;
    for (int index = 0; index < 4; ++index)
    {
        const auto dot = remaining.find('.');
        const auto part = remaining.substr(0, dot);
        unsigned value = 0;
        if (part.empty() || (part.size() > 1 && part.front() == '0')) return {};
        const auto parsed = std::from_chars(part.data(), part.data() + part.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr != part.data() + part.size() || value > 255) return {};
        result = (result << 8) | value;
        if (index == 3) { if (dot != remaining.npos) return {}; }
        else
        {
            if (dot == remaining.npos) return {};
            remaining.remove_prefix(dot + 1);
        }
    }
    if (result == 0 || result == 0xffffffff) return {};
    if constexpr (std::endian::native == std::endian::little) return std::byteswap(result);
    else return result;
}
struct GuardFolder
{
    std::string address;
    std::uint32_t ip = 0;
};
struct AreaFolder
{
    int id = 0;
    std::string name, folder;
    std::vector<GuardFolder> guards;
};
struct ScanLimits
{
    int area_count;
    std::size_t guards_per_area, name_bytes;
};
std::expected<std::vector<AreaFolder>, std::string> read_areas(const std::filesystem::path& root, ScanLimits limits);
std::expected<std::vector<std::filesystem::path>, std::string> package_files(
    const std::filesystem::path& root, std::string_view extension);
std::expected<void, std::string> write_atomic(const std::filesystem::path& destination, std::string_view bytes);
std::expected<std::string, std::string> transform_security(std::string_view source);
std::expected<void, std::string> generate_security_file(
    const std::filesystem::path& source, const std::filesystem::path& destination);
}
