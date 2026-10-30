#ifndef CQYH_GUARD_COMMAND_H
#define CQYH_GUARD_COMMAND_H

#include <algorithm>
#include <array>
#include <expected>
#include <string_view>

namespace guard_command
{
enum class Kind { quit, start, stop, close, check, reload };
enum class Error { empty, unknown };

struct Parsed
{
    Kind kind;
    std::string_view parameters;
};

inline constexpr std::array names{
    std::string_view{"quit"}, std::string_view{"start"}, std::string_view{"stop"},
    std::string_view{"close"}, std::string_view{"check"}, std::string_view{"reload"}};

constexpr char FoldAscii(char value) noexcept
{
    return value >= 'A' && value <= 'Z' ? static_cast<char>(value + ('a' - 'A')) : value;
}

// Views borrow the command text. Preserve the original space-only tokenization,
// case-insensitive command prefixes, and 255-byte parameter limit.
constexpr std::expected<Parsed, Error> Parse(std::string_view text) noexcept
{
    const auto start = text.find_first_not_of(' ');
    if (start == text.npos)
        return std::unexpected(Error::empty);

    text.remove_prefix(start);
    const auto separator = text.find(' ');
    const auto token = text.substr(0, separator);
    for (std::size_t index = 0; index < names.size(); ++index)
    {
        const auto name = names[index];
        if (token.size() >= name.size() &&
            std::ranges::equal(token.substr(0, name.size()), name, {}, FoldAscii, FoldAscii))
        {
            const auto parameters = separator == text.npos
                ? std::string_view{} : text.substr(separator + 1, 255);
            return Parsed{static_cast<Kind>(index), parameters};
        }
    }
    return std::unexpected(Error::unknown);
}
}

#endif
