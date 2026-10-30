#pragma once
#include <algorithm>
#include <cstring>
#include <optional>
#include <span>
#include <string_view>
#include "std_type.h"
#include "net.h"
#include "common_message.h"

namespace maintain
{
using Bytes = std::span<const char>;
inline std::optional<std::string_view> text(Bytes bytes)
{
    const auto end = std::ranges::find(bytes, '\0');
    if (end == bytes.end()) return {};
    return std::string_view{bytes.data(), static_cast<std::size_t>(end - bytes.begin())};
}
template<class Message> std::optional<std::string_view> tail(Bytes bytes)
{
    return bytes.size() > sizeof(Message) ? text(bytes.subspan(sizeof(Message))) : std::nullopt;
}
inline bool valid(UINT16 id, Bytes bytes)
{
    using namespace InternalMessage;
    if (bytes.size() > MAX_MESSAGE_SIZE) return false;
    switch (id)
    {
    case a2a_ping: return bytes.size() == sizeof(A2A_PING);
    case u2n_regrister_ack:
        if (bytes.size() == sizeof(U2N_REGRISTER_ACK))
        {
            U2N_REGRISTER_ACK message{};
            std::memcpy(&message, bytes.data(), sizeof(message));
            return text(message.szVersion).has_value() && text(message.szBuildVersion).has_value();
        }
        return false;
    case u2n_sync_processes_summary: return bytes.size() == sizeof(U2N_SYNC_DEFAULT_INFO);
    case n2u_update_process_info: return bytes.size() == sizeof(N2U_UPDATE_SESSION);
    case u2n_exec_cmd_ack: return tail<U2N_UPDATE_CMD_ACK>(bytes).has_value();
    case u2n_send_files_ack:
        return bytes.size() == sizeof(U2N_SEND_FILES_ACK) && text(bytes.subspan(sizeof(MESSAGE_HEADER), 128)).has_value();
    case u2n_deploy_server_version_ack:
        return bytes.size() == sizeof(U2N_DEPLOY_SERVER_VERSION_ACK) && text(bytes.subspan(sizeof(MESSAGE_HEADER) + sizeof(INT32), 128)).has_value();
    case u2n_hot_update_patch_ack:
        return bytes.size() == sizeof(U2N_HOT_UPDATE_PATCH_ACK) && text(bytes.subspan(sizeof(MESSAGE_HEADER) + sizeof(INT32), 128)).has_value();
    case u2n_update_statis_info: return bytes.size() == sizeof(U2N_UPDATE_STATIS_INFO);
    case u2n_notify: return tail<U2N_NOTIFY>(bytes).has_value();
    case u2n_error:
        // Old guards overallocated this fixed-array message and could append
        // text past the array. Accept their terminated text within received bytes.
        return bytes.size() >= sizeof(U2N_ERROR) && text(bytes.subspan(sizeof(MESSAGE_HEADER) + sizeof(INT32))).has_value();
    default: return false;
    }
}
}
