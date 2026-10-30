#pragma once
#include "../qt_cpp23.h"
#include "gm_log_data.h"
class QTextEdit;
namespace gm
{
std::expected<void,std::string> show_tool_log(QTextEdit& view,std::string_view text,std::chrono::system_clock::time_point when);
std::expected<void,std::string> show_game_log(QTextEdit& view,std::span<const char> bytes,const std::filesystem::path& path);
}
