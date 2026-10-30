#pragma once

#include "guard_files.h"

namespace guard_deploy
{
struct Request
{
    std::filesystem::path guard, versions, patches, runtime;
    std::string name;
};

// The Windows process boundary owns only the child it creates. Arguments are
// passed directly to CreateProcess, with no command interpreter involved.
guard_files::Result run_process(const std::filesystem::path& program,
    const std::vector<std::wstring>& arguments, const std::filesystem::path& working_directory,
    const std::filesystem::path& log, std::stop_token stop = {});

using Extractor = std::move_only_function<guard_files::Result(
    const std::filesystem::path& archive, const std::filesystem::path& destination,
    const std::filesystem::path& log, std::stop_token stop)>;

guard_files::Result version(Request request, std::stop_token stop = {}, Extractor extractor = {});
guard_files::Result patch(Request request, std::stop_token stop = {});
}
