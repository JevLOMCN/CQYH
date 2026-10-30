#include "stdafx.h"
#include "anticheat_log.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <map>
#include <string>
#include <string_view>

namespace
{
struct anticheat_line_data : line_data
{
    std::string role_name;
    std::chrono::sys_seconds action_time{};
    anticheat_action_type action_type = aca_invalid;
    bool has_time = false;
    bool has_action = false;
    bool has_name = false;
};

struct anticheat_log_context : log_context
{
    std::size_t start_count = 0;
    std::size_t generate_count = 0;
    std::size_t success_count = 0;
    std::size_t failed_count = 0;
    std::size_t timeout_count = 0;
    std::map<std::string, std::chrono::sys_seconds, std::less<>> pending;
    anticheat_line_data current;
};

bool read_time(std::string_view text, anticheat_line_data& line)
{
    std::array<int, tfCount> values{};
    constexpr std::array separators{'-', '-', ' ', ':', ':'};
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        if (text.empty())
            return false;
        const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), values[i]);
        if (error != std::errc{})
            return false;
        text.remove_prefix(end - text.data());
        if (i < separators.size())
        {
            if (text.empty() || text.front() != separators[i])
                return false;
            text.remove_prefix(1);
        }
    }
    using namespace std::chrono;
    if (!text.empty() || values[tfYear] < 1 || values[tfYear] > 32767 ||
        values[tfMonth] < 1 || values[tfMonth] > 12 || values[tfDay] < 1 || values[tfDay] > 31 ||
        values[tfHour] < 0 || values[tfHour] > 23 || values[tfMin] < 0 || values[tfMin] > 59 ||
        values[tfSec] < 0 || values[tfSec] > 59)
        return false;
    const year_month_day date{year{values[tfYear]}, month{static_cast<unsigned>(values[tfMonth])},
        day{static_cast<unsigned>(values[tfDay])}};
    if (!date.ok())
        return false;
    std::ranges::copy(values, line.time_value);
    line.action_time = sys_days{date} + hours{values[tfHour]} + minutes{values[tfMin]} + seconds{values[tfSec]};
    return true;
}

bool anticheat_end_func(log_context* context)
{
    const auto& state = *static_cast<anticheat_log_context*>(context);
    std::printf("process end, start: %zu, generate: %zu, success: %zu, failed: %zu include timeout: %zu",
        state.start_count, state.generate_count, state.success_count, state.failed_count, state.timeout_count);
    return true;
}
}

bool anticheat_process_field(int field_index, int, const char* value, log_context* context)
{
    if (!context || !value)
        return false;
    auto& state = *static_cast<anticheat_log_context*>(context);
    auto& line = state.current;
    if (field_index == ac_time)
    {
        line = {};
        line.has_time = read_time(value, line);
        return line.has_time;
    }
    if (field_index == ac_name)
    {
        line.role_name = value;
        line.has_name = !line.role_name.empty();
        return line.has_name;
    }
    if (field_index == ac_action)
    {
        const std::string_view action(value);
        line.has_action = true;
        if (action == "generate")
            line.action_type = aca_generate;
        else if (action == "success")
            line.action_type = aca_succsss;
        else if (action == "failed" || action == "kick")
            line.action_type = aca_failed;
        else if (action == "start")
            line.action_type = aca_started;
    }
    return true;
}

bool anticheat_process_line(int, log_context* context)
{
    auto& state = *static_cast<anticheat_log_context*>(context);
    const auto& line = state.current;
    if (!line.has_time || !line.has_action || !line.has_name)
        return false;
    switch (line.action_type)
    {
    case aca_generate: ++state.generate_count; break;
    case aca_succsss: ++state.success_count; break;
    case aca_failed: ++state.failed_count; break;
    case aca_started: ++state.start_count; break;
    default: break;
    }
    const auto found = state.pending.find(line.role_name);
    if (found == state.pending.end())
    {
        if (line.action_type == aca_generate)
            state.pending.emplace(line.role_name, line.action_time);
    }
    else
    {
        if (line.action_type == aca_failed && line.action_time - found->second >= std::chrono::minutes{10})
            ++state.timeout_count;
        state.pending.erase(found);
    }
    return true;
}

log_context* anticheat_init()
{
    static anticheat_log_context state;
    state = {};
    state.ld = &state.current;
    state.field_func = anticheat_process_field;
    state.line_func = anticheat_process_line;
    state.end_func = anticheat_end_func;
    return &state;
}
