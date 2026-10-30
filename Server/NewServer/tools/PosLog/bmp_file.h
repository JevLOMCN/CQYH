#pragma once
#include "common_log.h"
#include <span>

namespace poslog
{
std::expected<std::vector<char>,std::string> make_bitmap(std::span<const Position> positions,std::uint32_t side);
std::expected<void,std::string> write_bitmap(const std::filesystem::path& filename,
    std::span<const Position> positions,std::uint32_t side);
}
