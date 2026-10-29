#ifndef CQYH_BASELIB_NUMBER_PARSE_H
#define CQYH_BASELIB_NUMBER_PARSE_H

#include <charconv>
#include <cctype>
#include <cstdint>
#include <expected>
#include <string_view>
#include <system_error>

namespace cqyh
{
// Match legacy decimal-prefix fields: whitespace/sign are accepted, and trailing
// text is left unread. Unlike atoi, overflow is reported explicitly.
[[nodiscard]] inline std::expected<std::int32_t, std::errc> ParseIntegerPrefix(std::string_view value)
{
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())))
        value.remove_prefix(1);
    if (value.empty())
        return std::unexpected(std::errc::invalid_argument);
    if (value.front() == '+')
    {
        value.remove_prefix(1);
        if (value.empty() || value.front() < '0' || value.front() > '9')
            return std::unexpected(std::errc::invalid_argument);
    }
    std::int32_t result{};
    const auto parsed = std::from_chars(value.data(), value.data() + value.size(), result);
    if (parsed.ec != std::errc{})
        return std::unexpected(parsed.ec);
    return result;
}
}

#endif // CQYH_BASELIB_NUMBER_PARSE_H
