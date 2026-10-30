#pragma once
#include <compare>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <iosfwd>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace poslog
{
inline constexpr std::int64_t points_per_cm=1024;
inline constexpr std::int64_t max_scene_width=points_per_cm*128*2048;
inline constexpr std::int32_t projection_extent=153600;
struct Position { std::int32_t x{},y{};bool operator==(const Position&) const=default; };
struct GroupKey
{
    unsigned hour{};
    std::uint32_t scene{};
    auto operator<=>(const GroupKey&) const=default;
};
using Groups=std::map<GroupKey,std::vector<Position>>;
struct LogData { Groups groups;std::size_t rows{},outside_scene{}; };
struct Pixel { std::uint32_t column{},row{};bool operator==(const Pixel&) const=default; };
std::optional<Position> project(std::int64_t x,std::int64_t y);
std::optional<Pixel> pixel(Position point,std::uint32_t width,std::uint32_t height);
std::expected<LogData,std::string> parse_log(std::istream& input);
std::expected<LogData,std::string> load_log(const std::filesystem::path& filename);
}
