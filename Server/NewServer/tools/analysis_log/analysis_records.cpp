#include "stdafx.h"
#include "analysis_records.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace log_analysis
{
namespace
{
struct Line
{
    std::string_view time;
    std::string_view message;
    char level;
};

std::optional<Line> decode(std::string_view text)
{
    const auto tab = text.find('\t');
    if (tab == text.npos)
        return {};
    const auto time = text.substr(0, tab);
    text.remove_prefix(tab + 1);
    if (text.size() < 3 || text[0] != '[' || text[2] != ']')
        return {};
    const char level = text[1];
    text.remove_prefix(3);
    if (text.starts_with(' '))
        text.remove_prefix(1);
    // Thread IDs are optional in the server logger. Do not consume bracketed message text.
    if (text.size() >= 10 && text[0] == '[' && text[9] == ']' &&
        std::ranges::all_of(text.substr(1, 8), [](char c)
        { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); }))
    {
        text.remove_prefix(10);
        if (text.starts_with(' '))
            text.remove_prefix(1);
    }
    return Line{time, text, level};
}
}

void Process(Task& task, std::stop_token stop)
{
    if (task.mode != Mode::errors && task.mode != Mode::network)
        throw std::runtime_error("Invalid analysis mode");
    std::ifstream file(task.path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open log file");
    enum class Section { none, download, upload };
    Section section = Section::none;
    constexpr std::string_view download = "========================download================================";
    constexpr std::string_view upload = "========================upload================================";
    std::string text;
    while (std::getline(file, text))
    {
        if (stop.stop_requested())
            throw std::runtime_error("Analysis canceled");
        if (text.ends_with('\r'))
            text.pop_back();
        if (text.find('\0') != text.npos)
            throw std::runtime_error("Embedded NUL in log file");
        const auto line = decode(text);
        if (!line)
            continue;
        if (task.mode == Mode::errors)
        {
            if (line->level == 'E' || line->level == 'C')
                ++task.errors[std::string(line->message)];
            continue;
        }
        if (line->message == download)
            section = Section::download;
        else if (line->message == upload)
            section = Section::upload;
        else if (section == Section::download)
            task.output += std::format("{}\t{}\n", line->time, line->message);
        else if (section == Section::upload)
        {
            if (line->message.starts_with("c2"))
                task.output += std::format("\t\t\t\t\t\t{}\t{}\n", line->time, line->message);
            else
                section = Section::none;
        }
    }
    if (!file.eof() || file.bad())
        throw std::runtime_error("Cannot read log file");
    if (stop.stop_requested())
        throw std::runtime_error("Analysis canceled");
}
}
