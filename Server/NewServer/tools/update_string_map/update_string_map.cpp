#include "stdafx.h"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace
{
namespace fs = std::filesystem;
constexpr std::string_view prefix = "MMOErrorInfo.ERROR_CODE_DEF.";

std::string_view trim(std::string_view text)
{
    constexpr std::string_view whitespace = " \t\r\n";
    const auto first = text.find_first_not_of(whitespace);
    return first == text.npos ? std::string_view{} :
        text.substr(first, text.find_last_not_of(whitespace) - first + 1);
}

std::string read_file(const fs::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error(std::format("Cannot open {}", path.string()));
    std::string data{std::istreambuf_iterator<char>(file), {}};
    if (file.bad())
        throw std::runtime_error(std::format("Cannot read {}", path.string()));
    return data;
}

int conversion_size(std::size_t size)
{
    if (size > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::runtime_error("Text is too large for the native encoding API");
    return static_cast<int>(size);
}

std::wstring from_utf8(std::string_view text)
{
    if (text.empty())
        return {};
    const int length = conversion_size(text.size());
    const int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), length, nullptr, 0);
    if (count == 0)
        throw std::runtime_error("Invalid UTF-8 text");
    std::wstring wide(count, L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), length, wide.data(), count) != count)
        throw std::runtime_error("Cannot decode UTF-8 text");
    return wide;
}

std::string to_utf8(std::wstring_view text)
{
    if (text.empty())
        return {};
    const int length = conversion_size(text.size());
    const int count = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.data(), length, nullptr, 0, nullptr, nullptr);
    if (count == 0)
        throw std::runtime_error("Invalid UTF-16 text");
    std::string utf8(count, '\0');
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.data(), length, utf8.data(), count, nullptr, nullptr) != count)
        throw std::runtime_error("Cannot encode UTF-16 text");
    return utf8;
}

std::string decode_map(std::string_view bytes)
{
    if (bytes.starts_with("\xfe\xff"))
        throw std::runtime_error("UTF-16BE string maps are unsupported; use UTF-16LE or UTF-8");
    const bool has_bom = bytes.starts_with("\xff\xfe");
    if (has_bom)
        bytes.remove_prefix(2);
    if (has_bom || bytes.find('\0') != bytes.npos)
    {
        if (bytes.size() % 2 != 0)
            throw std::runtime_error("Truncated UTF-16LE string map");
        std::wstring wide;
        wide.reserve(bytes.size() / 2);
        for (std::size_t i = 0; i < bytes.size(); i += 2)
        {
            const auto low = static_cast<unsigned char>(bytes[i]);
            const auto high = static_cast<unsigned char>(bytes[i + 1]);
            wide.push_back(static_cast<wchar_t>(low | (high << 8)));
        }
        return to_utf8(wide);
    }
    if (bytes.starts_with("\xef\xbb\xbf"))
        bytes.remove_prefix(3);
    from_utf8(bytes); // Validate even entries that do not refer to error codes.
    return std::string(bytes);
}

std::string encode_map(std::string_view utf8)
{
    const auto wide = from_utf8(utf8);
    std::string bytes = "\xff\xfe";
    bytes.reserve(2 + wide.size() * 2);
    for (const wchar_t unit : wide)
    {
        bytes.push_back(static_cast<char>(unit & 0xff));
        bytes.push_back(static_cast<char>((unit >> 8) & 0xff));
    }
    return bytes;
}

struct ErrorEntry
{
    std::string name;
    std::string comment;
};

std::vector<ErrorEntry> read_errors(std::string_view source)
{
    std::vector<ErrorEntry> errors;
    std::set<std::string, std::less<>> names;
    bool inside = false;
    bool complete = false;
    for (std::size_t start = 0; start < source.size();)
    {
        const auto end = source.find('\n', start);
        auto line = trim(source.substr(start, end == source.npos ? end : end - start));
        start = end == source.npos ? source.size() : end + 1;
        if (!inside)
        {
            if (line.starts_with("enum ERROR_CODE_DEF") &&
                (line.size() == 19 || line[19] == ' ' || line[19] == '\t' || line[19] == '{'))
                inside = true;
            continue;
        }
        if (line.starts_with("};"))
        {
            complete = true;
            break;
        }
        if (!line.starts_with("err"))
            continue;
        const auto name = line.substr(0, line.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
        if (name.find("Begin") != name.npos || name.find("End") != name.npos || name.starts_with("errGM"))
            continue;
        if (!names.emplace(name).second)
            throw std::runtime_error(std::format("Duplicated error name {}", name));
        auto comment = std::string_view("no comment");
        const auto comment_start = line.find("///<");
        if (comment_start != line.npos)
        {
            comment = trim(line.substr(comment_start + 4));
            // Keep the legacy first-token comment convention used by translation imports.
            comment = comment.substr(0, comment.find_first_of(" ,;\t\r\n"));
        }
        errors.push_back({std::string(name), std::string(comment)});
    }
    if (!complete || errors.empty())
        throw std::runtime_error("Missing, empty or unterminated ERROR_CODE_DEF enum");
    return errors;
}

bool same_file(const fs::path& left, const fs::path& right)
{
    std::error_code error;
    return fs::equivalent(left, right, error) || fs::weakly_canonical(left) == fs::weakly_canonical(right);
}

int generate(const fs::path& header, const fs::path& input, const fs::path& output)
{
    if (same_file(header, output) || same_file(input, output))
        throw std::runtime_error("The output must differ from both input files");
    const auto errors = read_errors(read_file(header));
    std::set<std::string, std::less<>> remaining;
    for (const auto& error : errors)
        remaining.insert(error.name);
    std::set<std::string, std::less<>> seen;
    const auto text = decode_map(read_file(input));
    std::string report;
    std::size_t removed = 0;
    std::size_t added = 0;
    for (std::size_t start = 0; start < text.size();)
    {
        const auto end = text.find('\n', start);
        auto line = trim(std::string_view(text).substr(start, end == text.npos ? end : end - start));
        start = end == text.npos ? text.size() : end + 1;
        if (!line.starts_with(prefix))
            continue;
        line.remove_prefix(prefix.size());
        const auto name = line.substr(0, line.find_first_of("\t\r\n"));
        if (name.empty())
            throw std::runtime_error("Empty error name in string map");
        if (!seen.emplace(name).second)
            continue;
        if (!remaining.erase(std::string(name)))
        {
            report += std::format("{}{}\tinvalid enum value, please erase it\tinvalid enum value, please erase it\t0\t0\n", prefix, name);
            ++removed;
        }
    }
    // Declaration order is the established output order; enum numeric values are not serialized.
    for (const auto& error : errors)
    {
        if (remaining.contains(error.name))
        {
            report += std::format("{}{}\t{}\t{}\t0\t0\n", prefix, error.name, error.comment, error.comment);
            ++added;
        }
    }
    const auto bytes = encode_map(report);
    std::ofstream file(output, std::ios::binary | std::ios::trunc);
    if (!file)
        throw std::runtime_error(std::format("Cannot open {} for writing", output.string()));
    file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    file.close();
    if (!file)
        throw std::runtime_error(std::format("Cannot write {}", output.string()));
    std::cout << std::format("{} old error code(s), {} new error code(s)\n", removed, added);
    std::cout << "generated " << output.string() << '\n';
    return EXIT_SUCCESS;
}
}

int main(int argc, char* argv[])
{
    if (argc != 1 && argc != 4)
    {
        std::cerr << "Usage: update_string_map [error-header string-map output]\n";
        return EXIT_FAILURE;
    }
    try
    {
        return argc == 1
            ? generate("../../common/define/error_def.h", "../../client/ue3/mmogame/config/stringmap.txt",
                "../../client/ue3/mmogame/config/stringmap_new.txt")
            : generate(argv[1], argv[2], argv[3]);
    }
    catch (const std::exception& error)
    {
        std::cerr << "update_string_map: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
