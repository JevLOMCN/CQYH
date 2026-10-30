#include "stdafx.h"
#include "string_ex.h"

#include <algorithm>
#include <span>
#include <string_view>

std::size_t strxcpy(char* dst, const char* src, std::size_t size)
{
    if (dst == nullptr || src == nullptr || size == 0)
        return 0;

    const std::span destination(dst, size);
    // Preserve the legacy bounded read: packet fields need not contain a NUL
    // when the destination has room for only a prefix.
    std::size_t length = 0;
    while (length < destination.size() - 1 && src[length] != '\0')
        ++length;
    std::char_traits<char>::move(destination.data(), src, length);
    destination[length] = '\0';
    return length;
}

std::size_t strxcat(char* dst, const char* src, std::size_t size)
{
    if (dst == nullptr || src == nullptr || size == 0)
        return 0;

    const std::span destination(dst, size);
    const auto terminator = std::ranges::find(destination, '\0');
    if (terminator == destination.end())
    {
        destination.back() = '\0';
        return destination.size() - 1;
    }

    const auto length = static_cast<std::size_t>(terminator - destination.begin());
    return length + strxcpy(dst + length, src, size - length);
}

char* SplitStr(char*& tszString, char nSep)
{
    if (tszString == nullptr || nSep == '\0')
        return nullptr;

    const auto separator = std::string_view(tszString).find(nSep);
    if (separator == std::string_view::npos)
        return nullptr;

    char* token = tszString;
    tszString[separator] = '\0';
    tszString += separator + 1;
    return token;
}
