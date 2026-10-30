#pragma once
#include "cmd_data.h"
#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

namespace gm
{
inline constexpr std::size_t max_role_file_size = 10 * 1024 * 1024;
std::expected<std::vector<std::string>, std::string> parse_role_names(std::string_view text);
std::expected<std::list<UINT32>, std::string> parse_role_ids(std::string_view text);
std::expected<std::list<ROLE_RESULT>, std::string> read_role_batch(const std::filesystem::path& path);
std::expected<std::map<INT32, ROLE_NAME_DATA>, std::string> read_name_queries(const std::filesystem::path& path);
std::expected<std::list<UINT32>, std::string> read_role_ids(const std::filesystem::path& path);
}
