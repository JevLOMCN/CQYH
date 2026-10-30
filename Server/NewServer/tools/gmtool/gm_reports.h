#pragma once
#include "cmd_data.h"
#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

namespace gm
{
std::filesystem::path utf8_path(std::string_view text);
std::expected<void,std::string> write_text_report(const std::filesystem::path& path, std::string_view bytes);
std::expected<void, std::string> write_name_report(const std::filesystem::path& path,
    const std::map<INT32, ROLE_NAME_DATA>& names);
std::expected<void, std::string> write_role_report(const std::filesystem::path& input,
    INT32 kind, const std::list<ROLE_RESULT>& roles);
}
