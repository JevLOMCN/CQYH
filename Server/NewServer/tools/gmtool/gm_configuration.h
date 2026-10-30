#ifndef CQYH_GM_CONFIGURATION_H
#define CQYH_GM_CONFIGURATION_H

#include "std_type.h"
#include "tl_config.h"
#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace gm
{
using Configuration = std::expected<std::unique_ptr<TL_CONFIG>, std::string>;
// Scripts retain GetTLConfig(), LoadFile(), constants and zero-based arrays.
// The candidate is isolated from g_TLConfig until the caller publishes it.
Configuration parse_configuration(std::string_view script);
Configuration load_configuration(const std::filesystem::path& path);
std::expected<void, std::string> reload_configuration(const std::filesystem::path& path, TL_CONFIG& live);
}
#endif
