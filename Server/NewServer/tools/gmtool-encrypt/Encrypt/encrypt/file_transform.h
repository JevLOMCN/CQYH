#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace gm_file
{
enum class Operation { encrypt, decrypt };

// Encryption keeps the legacy CRC32-of-basename output name in output_directory.
// Decryption replaces the selected file only after reading/transforming/staging succeeds.
// The optional directory applies only to encryption; an empty one means current_path().
std::expected<std::filesystem::path,std::string> transform_file(
    const std::filesystem::path& input,Operation operation,
    const std::filesystem::path& output_directory={});
}
