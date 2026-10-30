#include "gm_data_packets.h"
#include "gmtl_message.h"
#include <cstring>

std::expected<gm::RoleKeys, std::string> gm::decode_role_keys(std::span<const std::byte> packet)
{
    InternalMessage::GM2TL_QUERY_ROLE_ID_ACK header;
    if (packet.size() < sizeof(header) || packet.size() > 65535)
        return std::unexpected("invalid role-key reply size");
    std::memcpy(&header, packet.data(), sizeof(header));
    if (header.wMsg != InternalMessage::gm2tl_query_role_id_ack || header.byIsEnd > 1 ||
        header.nCount < 0 || static_cast<std::size_t>(header.nCount) >
        (packet.size() - sizeof(header)) / sizeof(InternalMessage::GM_ROLE_KEY_INFO))
        return std::unexpected("invalid role-key count or flags");
    RoleKeys result{header.nAreaID, header.byIsEnd == 1, {}};
    result.rows.reserve(header.nCount);
    auto bytes = packet.subspan(sizeof(header));
    for (INT32 index = 0; index < header.nCount; ++index)
    {
        InternalMessage::GM_ROLE_KEY_INFO record;
        std::memcpy(&record, bytes.data(), sizeof(record));
        bytes = bytes.subspan(sizeof(record));
        const auto* name_end = static_cast<const char*>(std::memchr(record.szRoleName, 0, sizeof(record.szRoleName)));
        const auto* account_end = static_cast<const char*>(std::memchr(record.szUserName, 0, sizeof(record.szUserName)));
        if (!name_end || !account_end) return std::unexpected("unterminated role-key text");
        result.rows.push_back({record.dwRoleID,
            std::string(record.szRoleName, static_cast<std::size_t>(name_end - record.szRoleName)),
            std::string(record.szUserName, static_cast<std::size_t>(account_end - record.szUserName))});
    }
    return result;
}

std::expected<gm::ItemBatch, std::string> gm::decode_item_batch(std::span<const std::byte> packet)
{
    InternalMessage::GM2TL_ITEM_LIST_NTF header{};
    if (packet.size() < sizeof(header))
        return std::unexpected("truncated item-list header");
    std::memcpy(&header, packet.data(), sizeof(header));
    if (header.wMsg != InternalMessage::gm2tl_item_list_ntf ||
        (header.bCLS != FALSE && header.bCLS != TRUE) || header.nCount < 0 ||
        static_cast<std::size_t>(header.nCount) > (packet.size() - sizeof(header)) / sizeof(InternalMessage::ITEM_TEMPLATE_DATA))
        return std::unexpected("invalid item-list count or flags");

    ItemBatch result{header.bCLS == TRUE, {}};
    result.items.reserve(header.nCount);
    auto bytes = packet.subspan(sizeof(header));
    for (INT32 index = 0; index < header.nCount; ++index)
    {
        InternalMessage::ITEM_TEMPLATE_DATA wire{};
        std::memcpy(&wire, bytes.data(), sizeof(wire));
        bytes = bytes.subspan(sizeof(wire));
        if (!std::memchr(wire.szName, 0, sizeof(wire.szName)))
            return std::unexpected("unterminated item-template name");
        ITEM_TEMPLATE_DATA item{};
        item.dwTempID = wire.dwTempID;
        std::memcpy(item.szName, wire.szName, sizeof(item.szName));
        result.items.push_back(item);
    }
    return result;
}

std::expected<std::vector<TL_ANNOUNCE_DATA>, std::string> gm::decode_announcements(
    std::span<const std::byte> packet, INT32 server_id)
{
    InternalMessage::GM2TL_GET_ANNOUNCE_LIST_ACK header{};
    if (server_id <= 0 || packet.size() < sizeof(header))
        return std::unexpected("invalid announcement server or header");
    std::memcpy(&header, packet.data(), sizeof(header));
    if (header.wMsg != InternalMessage::gm2tl_get_announce_list_ack ||
        header.nGroupID < 0 || header.nGroupID >= MAX_SERVER_GROUP_COUNT || header.nCount < 0 ||
        static_cast<std::size_t>(header.nCount) > (packet.size() - sizeof(header)) / sizeof(InternalMessage::GMTL_ANNOUNCE_DATA))
        return std::unexpected("invalid announcement count");

    std::vector<TL_ANNOUNCE_DATA> result;
    result.reserve(header.nCount);
    auto bytes = packet.subspan(sizeof(header));
    for (INT32 index = 0; index < header.nCount; ++index)
    {
        InternalMessage::GMTL_ANNOUNCE_DATA wire{};
        std::memcpy(&wire, bytes.data(), sizeof(wire));
        bytes = bytes.subspan(sizeof(wire));
        if (wire.nGroupID < 0 || wire.nGroupID >= MAX_SERVER_GROUP_COUNT || !std::memchr(wire.szText, 0, sizeof(wire.szText)))
            return std::unexpected("invalid announcement group or text");
        TL_ANNOUNCE_DATA item{};
        item.nGroupID = wire.nGroupID;
        item.nSvrID = server_id;
        item.dwSerial = wire.dwSerial;
        item.dwIntervalTime = wire.dwIntervalTime;
        item.dwRepeatNum = wire.dwRepeatNum;
        std::memcpy(item.szText, wire.szText, sizeof(item.szText));
        result.push_back(item);
    }
    return result;
}
