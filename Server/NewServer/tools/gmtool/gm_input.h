#pragma once
#include <charconv>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <system_error>

namespace gm
{
template<std::integral T> requires (!std::same_as<T,bool>)
std::expected<T,std::string> parse_decimal(std::string_view text,bool empty_zero=false)
{
    constexpr std::string_view spaces=" \t\r\n\f\v";
    const auto first=text.find_first_not_of(spaces);
    if (first==std::string_view::npos)
    {
        if (empty_zero) return T{};
        return std::unexpected("empty decimal field");
    }
    text=text.substr(first,text.find_last_not_of(spaces)-first+1);
    if (text.front()=='+')
    {
        text.remove_prefix(1);
        if (text.empty() || text.front()=='+' || text.front()=='-')
            return std::unexpected("invalid decimal sign");
    }
    T value{};
    const auto [end,error]=std::from_chars(text.data(),text.data()+text.size(),value);
    if (error!=std::errc{} || end!=text.data()+text.size()) return std::unexpected("invalid decimal value or range");
    return value;
}
inline std::expected<std::uint32_t,std::string> minute_duration(std::int64_t minutes,std::uint32_t unlimited_duration)
{
    if (minutes==-1) return unlimited_duration;
    if (minutes<=0) return std::unexpected("duration must be positive or -1");
    constexpr std::int64_t ceiling=1000000000;
    if (minutes>ceiling/60) return unlimited_duration;
    return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes{minutes}).count());
}
inline void normalize_line_breaks(std::span<char> text)
{
    for (std::size_t index=0;index<text.size();++index)
    {
        if (text[index]=='\r' && index+1<text.size() && text[index+1]=='\n')
        {text[index]=' ';text[++index]=' ';}
        else if (text[index]=='\n') text[index]=' ';
    }
}
}
