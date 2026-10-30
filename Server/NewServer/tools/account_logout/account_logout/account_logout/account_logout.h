#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>

namespace account_logout
{
struct Report
{
    std::size_t sessions = 0, users = 0, logouts = 0, logins = 0;
    std::string text;
};

// Input bytes stay in their existing encoding. Only the first whitespace-delimited
// token is significant, as in the legacy exported-file workflow.
std::expected<Report, std::string> analyze(const std::filesystem::path& root);
}
