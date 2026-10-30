#pragma once
#include "std_type.h"
#include <chrono>
#include <array>
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>

namespace gm
{
std::expected<std::string,std::string> timed_log(std::string_view text,std::chrono::system_clock::time_point when);
std::expected<std::string_view,std::string> game_log_body(std::span<const char> terminated);
std::expected<void,std::string> clear_game_log(const std::filesystem::path& path);
std::expected<void,std::string> append_game_log(const std::filesystem::path& path,std::string_view text);
BOOL output_tool_log(std::string_view text);
BOOL output_game_log(INT32 type,BOOL truncated,std::string_view text);
using LogQueryFields=std::array<std::string_view,7>;
std::expected<std::string,std::string> serialize_log_query(INT32 type,const LogQueryFields& fields);
}
