#pragma once

#include <algorithm>
#include <cstring>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include "std_type.h"
#include "net.h"
#include "common_message.h"

namespace guard_packets
{
using Bytes = std::span<const char>;

template<class T> std::optional<T> read(Bytes bytes) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    if (bytes.size() < sizeof(T)) return {};
    T value{};
    std::memcpy(&value, bytes.data(), sizeof(T));
    return value;
}

inline std::optional<std::string_view> text(Bytes bytes) noexcept
{
    const auto end = std::ranges::find(bytes, '\0');
    if (end == bytes.end()) return {};
    return std::string_view{bytes.data(), static_cast<std::size_t>(end - bytes.begin())};
}

template<class T> std::optional<std::string_view> tail(Bytes bytes) noexcept
{
    if (bytes.size() <= sizeof(T)) return {};
    return text(bytes.subspan(sizeof(T)));
}

// Keep the packed wire structures, including legacy flexible char tails.
// Only the received span supplies a valid bound for strings in those tails.
inline bool valid(UINT16 id, Bytes bytes) noexcept
{
    using namespace InternalMessage;
    if (bytes.size() > MAX_MESSAGE_SIZE) return false;
    switch (id)
    {
    case a2a_ping: return bytes.size() == sizeof(A2A_PING);
    case a2u_regrister: return bytes.size() == sizeof(A2U_REGRISTER);
    case n2u_regrister:
        if (bytes.size() != sizeof(N2U_REGRISTER)) return false;
        if (const auto value = read<N2U_REGRISTER>(bytes))
            return text(value->szName).has_value() && text(value->szPwd).has_value();
        return false;
    case a2u_notify: return tail<A2U_NOTIFY>(bytes).has_value();
    case m2u_command_ack: return tail<M2U_COMMAND_RESPONSE>(bytes).has_value();
    case n2u_exec_cmd: return tail<N2U_UPDATE_CMD>(bytes).has_value();
    default: return true; // Upload/deployment handlers have their own body checks.
    }
}
}
