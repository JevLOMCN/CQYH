#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <stop_token>
#include <string>

namespace log_analysis
{
enum class Mode { errors = 1, network = 2 };
using ErrorCounts = std::map<std::string, std::uint64_t, std::less<>>;

struct Task
{
    std::filesystem::path path;
    Mode mode = Mode::errors;
    ErrorCounts errors;
    std::string output;
    std::string diagnostic;
    bool failed = false;
};

void Process(Task& task, std::stop_token stop);
}
