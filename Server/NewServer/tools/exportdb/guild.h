#pragma once
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <span>
#include <string>

class CSqlMgr;

namespace cqyh::exportdb
{
struct GuildResources
{
    std::int32_t level = 0;
    std::int32_t money = 0;
    std::int32_t wood = 0;
    std::int32_t herb = 0;
    std::int32_t crystal = 0;
    std::int32_t silver = 0;
};
// The span is the uncompressed version-5 blob, excluding the SQL length prefix.
std::expected<GuildResources, std::string> ParseGuildBlob(std::span<const std::byte> bytes);
bool ExportGuilds(CSqlMgr& manager, const std::filesystem::path& output = "guild_stat.txt");
bool ExportMembers(CSqlMgr& manager, const std::filesystem::path& output = "guild_member.txt");
}
