#pragma once
#include "std_type.h"
#include "common_message.h"
#include "gmtl_message.h"
#include "gm_def.h"
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <utility>

namespace gm
{
namespace receive_detail
{
using Bytes = std::span<const char>;
template<class T, class Check> bool inspect(Bytes bytes, Check check)
{
    if (bytes.size() < sizeof(T)) return false;
    T value;
    std::memcpy(&value, bytes.data(), sizeof(value));
    return check(value);
}
template<class T> bool fixed(Bytes bytes) { return bytes.size() >= sizeof(T); }
template<std::size_t N> bool text(const char (&value)[N]) { return std::memchr(value, 0, N) != nullptr; }
inline bool group(INT32 value) { return value >= 0 && value < MAX_SERVER_GROUP_COUNT; }
inline bool flag(INT32 value) { return value == FALSE || value == TRUE; }
template<class Record, class Check> bool records(Bytes bytes, INT32 count, Check check)
{
    if (count < 0 || static_cast<std::size_t>(count) > bytes.size() / sizeof(Record)) return false;
    for (INT32 index = 0; index < count; ++index)
    {
        if (!inspect<Record>(bytes, check)) return false;
        bytes = bytes.subspan(sizeof(Record));
    }
    return true;
}
template<class Message> bool role(Bytes bytes)
{
    constexpr auto prefix = offsetof(Message, szData);
    if (bytes.size() < prefix) return false;
    INT32 length;
    std::memcpy(&length, bytes.data() + offsetof(Message, nLength), sizeof(length));
    // The owned role decoder checks the individual records before publication.
    return length >= 0 && length <= 32000 && static_cast<std::size_t>(length) <= bytes.size() - prefix;
}
inline bool log_length(Bytes bytes, const InternalMessage::GM2TL_LOG_ACK& value)
{
    // The server declares the final NUL but older writers omit it from SendTL.
    return flag(value.bTruncated) && value.nLength > 0 &&
        static_cast<std::size_t>(value.nLength - 1) <= bytes.size() - sizeof(value);
}
}

struct LogReply
{
    INT32 type;
    bool truncated;
    std::string text;
};
inline std::optional<LogReply> decode_log_reply(std::span<const char> bytes)
{
    using namespace InternalMessage;
    if (bytes.size() > 0xFFFF) return {};
    std::optional<LogReply> result;
    receive_detail::inspect<GM2TL_LOG_ACK>(bytes, [&](const auto& value)
    {
        if (value.wMsg != gm2tl_log_ack || !receive_detail::log_length(bytes, value)) return false;
        std::string text(bytes.data() + sizeof(value), static_cast<std::size_t>(value.nLength - 1));
        if (const auto end = text.find('\0'); end != std::string::npos) text.resize(end);
        result.emplace(value.nLogType, value.bTruncated == TRUE, std::move(text));
        return true;
    });
    return result;
}

// Validate every field that determines a legacy receiver's read bounds before
// dispatch. Read wire values through memcpy; packets need not be aligned.
inline bool valid_receive_packet(std::span<const char> bytes)
{
    using namespace InternalMessage;
    using namespace receive_detail;
    if (bytes.size() < sizeof(MESSAGE_HEADER) || bytes.size() > 0xFFFF) return false;
    MESSAGE_HEADER header;
    std::memcpy(&header, bytes.data(), sizeof(header));
    switch (header.wMsg)
    {
    case a2a_ping: return fixed<A2A_PING>(bytes);
    case gm2tl_register_ack: return fixed<GM2TL_REGISTER_ACK>(bytes);
    case gm2tl_verify_ack:
        return inspect<GM2TL_VERIFY_ACK>(bytes, [](const auto& value) { return flag(value.bLD); });
    case gm2tl_login_ack:
        return inspect<GM2TL_LOGIN_ACK>(bytes, [](const auto& value)
        {
            if (value.nMSCount < 0 || value.nMSCount > MAX_MS_SESSION_COUNT) return false;
            for (INT32 index = 0; index < value.nMSCount; ++index)
                if (!text(value.stMSInfo[index].szMSName)) return false;
            return true;
        });
    case gm2tl_item_list_ntf:
        return inspect<GM2TL_ITEM_LIST_NTF>(bytes, [&](const auto& value)
        {
            return flag(value.bCLS) && records<InternalMessage::ITEM_TEMPLATE_DATA>(bytes.subspan(sizeof(value)), value.nCount,
                [](const auto& item) { return text(item.szName); });
        });
    case gm2tl_logout_ack: return fixed<GM2TL_LOGOUT_ACK>(bytes);
    case gm2tl_pre_query_ack:
        return inspect<GM2TL_PRE_QUERY_ACK>(bytes, [](const auto& value) { return text(value.szRoleName); });
    case gm2tl_query_role_id_ack:
        return inspect<GM2TL_QUERY_ROLE_ID_ACK>(bytes, [&](const auto& value)
        {
            return flag(value.byIsEnd) && records<GM_ROLE_KEY_INFO>(bytes.subspan(sizeof(value)), value.nCount,
                [](const auto& item) { return text(item.szRoleName) && text(item.szUserName); });
        });
    case gm2tl_query_role_data_ack: return role<GM2TL_QUERY_ROLE_DATA_ACK>(bytes);
    case gm2tl_forbid_chat_ack: return fixed<GM2TL_FORBID_CHAT_ACK>(bytes);
    case gm2tl_freeze_role_ack: return fixed<GM2TL_FREEZE_ROLE_ACK>(bytes);
    case gm2tl_kick_role_ack: return fixed<GM2TL_KICK_ROLE_ACK>(bytes);
    case gm2tl_send_mail_ack: return fixed<GM2TL_SEND_MAIL_ACK>(bytes);
    case gm2tl_announce_ack: return fixed<GM2TL_ANNOUNCE_ACK>(bytes);
    case gm2tl_get_announce_list_ack:
        return inspect<GM2TL_GET_ANNOUNCE_LIST_ACK>(bytes, [&](const auto& value)
        {
            return group(value.nGroupID) && records<GMTL_ANNOUNCE_DATA>(bytes.subspan(sizeof(value)), value.nCount,
                [](const auto& item) { return group(item.nGroupID) && text(item.szText); });
        });
    case gm2tl_del_announce_ack: return fixed<GM2TL_DEL_ANNOUNCE_ACK>(bytes);
    case gm2tl_log_ack:
        return inspect<GM2TL_LOG_ACK>(bytes, [&](const auto& value)
        {
            return log_length(bytes, value);
        });
    case gm2tl_exe_gm_str_ack: return fixed<GM2TL_EXE_GM_STR_ACK>(bytes);
    case gm2tl_load_role_data_ack: return role<GM2TL_LOAD_ROLE_DATA_ACK>(bytes);
    case gm2tl_modify_role_data_ack: return fixed<GM2TL_MODIFY_ROLE_DATA_ACK>(bytes);
    case gm2tl_query_name_to_id_ack: return fixed<GM2TL_QUERY_NAME_TO_ID_ACK>(bytes);
    case gm2tl_error_str:
        return bytes.size() > sizeof(GM2TL_ERROR_STR) &&
            std::memchr(bytes.data() + sizeof(GM2TL_ERROR_STR), 0, bytes.size() - sizeof(GM2TL_ERROR_STR));
    case gm2tl_role_exe_gm_cmd: return fixed<GM2TL_ROLE_EXE_GM_CMD>(bytes);
    case gm2tl_select_group_ack: return fixed<GM2TL_SELECT_GROUP_ACK>(bytes);
    case gm2tl_query_exchange_log_ack:
        return inspect<GM2TL_QUERY_EXCHANGE_LOG_ACK>(bytes, [&](const auto& value)
        {
            return flag(value.byIsClear) && value.nCount <= MAX_EXCHANGE_LOG_NUM &&
                records<GM_EXCHANGE_PRICE_LOG>(bytes.subspan(sizeof(value)), value.nCount,
                    [](const auto& item) { return text(item.szExchangeTime); });
        });
    case gm2tl_query_exchange_history_ack:
        return inspect<GM2TL_QUERY_EXCHANGE_HISTORY_ACK>(bytes, [&](const auto& value)
        {
            return flag(value.byIsClear) && value.nCount <= MAX_EXCHANGE_HISTORY_NUM &&
                records<GM_EXCHANGE_PRICE_HISTORY>(bytes.subspan(sizeof(value)), value.nCount,
                    [](const auto& item) { return text(item.szHistoryTime); });
        });
    case gm2tl_exchange_price:
        return inspect<GM2TL_EXCHANGE_PRICE>(bytes, [&](const auto& value)
        {
            const auto count = INT32(value.bySellPriceCount) + value.byBuyPriceCount;
            return count <= 40 && records<GMTL_EXCHANGE_PRICE_INFO>(bytes.subspan(sizeof(value)), count,
                [](const auto&) { return true; });
        });
    case gm2tl_query_order_detail_ack:
        return inspect<GM2TL_QUERY_ORDER_DETAIL_ACK>(bytes, [&](const auto& value)
        {
            return records<GM_EXCHANGE_COUNT_INFO>(bytes.subspan(sizeof(value)), value.byCountCount,
                [](const auto& item) { return text(item.szRoleName); });
        });
    case gm2tl_add_system_order_ack: return fixed<GM2TL_ADD_SYSTEM_ORDER_ACK>(bytes);
    case gm2tl_del_system_order_ack: return fixed<GM2TL_DEL_SYSTEM_ORDER_ACK>(bytes);
    case gm2tl_query_sys_exchange_info_ack: return fixed<GM2TL_QUERY_SYS_EXCHANGE_INFO_ACK>(bytes);
    case gm2tl_get_exchange_limit_ack: return fixed<GM2TL_GET_EXCHANGE_LIMIT_ACK>(bytes);
    case gm2tl_set_exchange_limit_ack: return fixed<GM2TL_SET_EXCHANGE_LIMIT_ACK>(bytes);
    default: return false;
    }
}
}
