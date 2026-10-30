#include <charconv>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace
{
namespace fs = std::filesystem;

std::string_view trim(std::string_view text)
{
    constexpr std::string_view whitespace = " \t\r\n";
    const auto begin = text.find_first_not_of(whitespace);
    if (begin == text.npos)
        return {};
    return text.substr(begin, text.find_last_not_of(whitespace) - begin + 1);
}

std::expected<std::string, std::string> read_file(const fs::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return std::unexpected(std::format("Cannot open {}", path.string()));
    std::string contents{std::istreambuf_iterator<char>(file), {}};
    if (file.bad())
        return std::unexpected(std::format("Cannot read {}", path.string()));
    return contents;
}

std::expected<int, std::string> read_version(std::string_view source, std::string_view name)
{
    // Match definitions rather than mentions in comments or longer identifiers.
    for (std::size_t start = 0; start < source.size();)
    {
        const auto end = source.find('\n', start);
        auto line = trim(source.substr(start, end == source.npos ? end : end - start));
        start = end == source.npos ? source.size() : end + 1;
        if (!line.starts_with('#'))
            continue;
        line = trim(line.substr(1));
        if (!line.starts_with("define") || line.size() == 6 ||
            (line[6] != ' ' && line[6] != '\t'))
            continue;
        line = trim(line.substr(6));
        const auto token_end = line.find_first_of(" \t(");
        if (line.substr(0, token_end) != name)
            continue;
        line = token_end == line.npos ? std::string_view{} : trim(line.substr(token_end));
        if (line.empty())
            return std::unexpected(std::format("Missing integer for {}", name));
        if (line.starts_with('('))
        {
            const auto close = line.find(')');
            if (close == line.npos)
                return std::unexpected(std::format("Missing ')' in {}", name));
            const auto tail = trim(line.substr(close + 1));
            if (!tail.empty() && !tail.starts_with("//") && !tail.starts_with("/*"))
                return std::unexpected(std::format("Invalid definition of {}", name));
            line = trim(line.substr(1, close - 1));
        }
        else
        {
            line = trim(line.substr(0, line.find('/')));
        }
        if (line.empty())
            return std::unexpected(std::format("Missing integer for {}", name));
        int value = 0;
        const auto [ptr, error] = std::from_chars(line.data(), line.data() + line.size(), value);
        if (error != std::errc{} || ptr != line.data() + line.size() || value < 0)
            return std::unexpected(std::format("Invalid nonnegative integer for {}", name));
        return value;
    }
    return std::unexpected(std::format("Missing definition of {}", name));
}

bool same_file(const fs::path& left, const fs::path& right)
{
    std::error_code error;
    if (fs::equivalent(left, right, error))
        return true;
    return fs::weakly_canonical(left) == fs::weakly_canonical(right);
}

int generate(const fs::path& header, const fs::path& model, const fs::path& output)
{
    if (same_file(header, output) || same_file(model, output))
        throw std::runtime_error("The output must differ from both input files");
    const auto source = read_file(header);
    if (!source)
        throw std::runtime_error(source.error());
    const auto major = read_version(*source, "GM_MAJOR_VERSION");
    const auto minor = read_version(*source, "GM_MINOR_VERSION");
    if (!major || !minor)
        throw std::runtime_error(!major ? major.error() : minor.error());
    auto contents = read_file(model);
    if (!contents)
        throw std::runtime_error(contents.error());
    const auto marker = contents->find('_');
    if (marker == contents->npos)
        throw std::runtime_error("The template has no '_' version marker");
    contents->insert(marker + 1, std::format("{}.{}", *major, *minor));

    // Preserve template bytes; text reads combined with ftell previously added NUL padding.
    std::ofstream file(output, std::ios::binary | std::ios::trunc);
    if (!file)
        throw std::runtime_error(std::format("Cannot open {} for writing", output.string()));
    file.write(contents->data(), static_cast<std::streamsize>(contents->size()));
    file.close();
    if (!file)
        throw std::runtime_error(std::format("Cannot write {}", output.string()));
    std::cout << "OK!\n";
    return EXIT_SUCCESS;
}
}

int main(int argc, char* argv[])
{
    if (argc != 1 && argc != 4)
    {
        std::cerr << "Usage: gmtool_label_version [version-header template output]\n";
        return EXIT_FAILURE;
    }
    try
    {
        return argc == 1
            ? generate("../../../inc/gm/gm_version.h", "pak-version.model", "pak.bat")
            : generate(argv[1], argv[2], argv[3]);
    }
    catch (const std::exception& error)
    {
        std::cerr << "gmtool_label_version: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
