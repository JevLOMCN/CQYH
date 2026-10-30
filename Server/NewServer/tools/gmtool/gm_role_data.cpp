#include "gm_role_data.h"
#include "gmtl_message.h"
#include <array>

namespace
{
static_assert(sizeof(GM_ROLE_SLOT) == 3 * sizeof(UINT16));
bool terminated(std::span<const char> text, std::size_t maximum)
{
    const auto end = std::ranges::find(text, '\0');
    return end != text.end() && static_cast<std::size_t>(end - text.begin()) <= maximum;
}

bool valid_record(const gm::RoleRecord& record)
{
    const auto bytes = std::span(record.payload);
    switch (record.type)
    {
    case GM_ROLE_BASE_DATA:
        switch (record.base)
        {
        case GM_RBD_NAME:
            // The maintained Server writer excludes the role-name terminator.
            return record.text().size() < ROLE_NAME_LEN;
        case GM_RBD_LAST_LOGOUT_TIME:
        case GM_RBD_FORBIDDEND_CHAT_TIME:
        case GM_RBD_FREEZE_ROLE_TIME:
        case GM_RBD_FREEZE_CHEAT_ROLE:
            return terminated(bytes, 20);
        case GM_RBD_X:
        case GM_RBD_Y:
        case GM_RBD_Z:
            return bytes.size() >= sizeof(INT64);
        default:
            if ((record.base >= GM_RBD_ID && record.base <= GM_RBD_MAP_SERIAL_ID) ||
                record.base == GM_RBD_GUILD_ID || (record.base >= GM_RBD_MONEY_1 && record.base <= GM_RBD_MONEY_14))
                return bytes.size() >= sizeof(UINT32);
            return true; // Preserve unknown fields for compatible future peers.
        }
    case GM_ROLE_TITLE:
        if (bytes.size() < sizeof(GM_ROLE_TITLE_SLOT)) return false;
        return terminated(record.value<GM_ROLE_TITLE_SLOT>().szName, MAX_TITLE_NAME_LEN - 1);
    case GM_ROLE_SPELL:
        if (bytes.size() < sizeof(GM_ROLE_SPELL_SLOT)) return false;
        return terminated(record.value<GM_ROLE_SPELL_SLOT>().szName, MAX_SPELL_NAME_LEN - 1);
    case GM_ROLE_ITEM:
        return bytes.size() >= sizeof(GM_ROLE_ITEM_SLOT) + 1 &&
            terminated(bytes.subspan(offsetof(GM_ROLE_ITEM_SLOT, szData)), MAX_ITEM_NAME_LEN - 1);
    case GM_ROLE_QUEST:
        return bytes.size() >= sizeof(UINT32) + 1 &&
            terminated(bytes.subspan(sizeof(UINT32)), COMMON_NAME_LEN - 1);
    case GM_ROLE_MAIL:
        if (bytes.size() < sizeof(GM_ROLE_MAIL_SLOT) + 1) return false;
        {
            const auto mail = record.value<GM_ROLE_MAIL_SLOT>();
            return mail.nMailType >= 0 && mail.nMailType < 2 && mail.nState >= 0 && mail.nState < 2 &&
                terminated(bytes.subspan(offsetof(GM_ROLE_MAIL_SLOT, szData)), MAIL_SUBJECT_LEN - 1);
        }
    default:
        return true;
    }
}

template <class Header>
std::expected<gm::RoleReply, std::string> decode_role(std::span<const std::byte> packet, UINT16 message)
{
    constexpr auto prefix = offsetof(Header, szData);
    if (packet.size() < prefix)
        return std::unexpected("truncated role reply header");
    Header header{};
    std::memcpy(&header, packet.data(), prefix);
    if (header.wMsg != message || !header.dwRoleID || header.nLength < 0 ||
        static_cast<std::size_t>(header.nLength) > sizeof(header.szData) ||
        static_cast<std::size_t>(header.nLength) > packet.size() - prefix)
        return std::unexpected("invalid role reply identity or length");
    bool online = false;
    if constexpr (requires { header.bIsOnline; })
    {
        if (header.bIsOnline != TRUE && header.bIsOnline != FALSE)
            return std::unexpected("invalid online flag in role reply");
        online = header.bIsOnline == TRUE;
    }
    auto document = gm::parse_role_data({reinterpret_cast<const char*>(packet.data() + prefix), static_cast<std::size_t>(header.nLength)});
    if (!document)
        return std::unexpected(std::move(document.error()));
    return gm::RoleReply{header.dwRoleID, header.nAreaID, online, std::move(*document)};
}
}

std::string gm::RoleRecord::text(std::size_t offset) const
{
    if (offset > payload.size())
        throw std::out_of_range("role text is outside its record");
    const auto begin = payload.begin() + offset;
    return {begin, std::find(begin, payload.end(), '\0')};
}

std::expected<gm::RoleDocument, std::string> gm::parse_role_data(std::span<const char> bytes)
{
    if (bytes.size() > MAX_GM_ROLE_DATA_STR_LENGTH)
        return std::unexpected("oversized role data");
    RoleDocument result;
    while (!bytes.empty())
    {
        if (bytes.size() < sizeof(GM_ROLE_SLOT))
            return std::unexpected("truncated role record header");
        GM_ROLE_SLOT header;
        std::memcpy(&header, bytes.data(), sizeof(header));
        if (header.wLength < sizeof(header) || header.wLength > bytes.size())
            return std::unexpected("invalid role record length");
        const auto payload = bytes.subspan(sizeof(header), header.wLength - sizeof(header));
        RoleRecord record{header.wRoleType, header.wBaseDataType, {payload.begin(), payload.end()}};
        if (!valid_record(record))
            return std::unexpected("invalid role record payload");
        result.push_back(std::move(record));
        bytes = bytes.subspan(header.wLength);
    }
    return result;
}

std::expected<gm::RoleReply, std::string> gm::decode_query_role(std::span<const std::byte> packet)
{
    return decode_role<InternalMessage::GM2TL_QUERY_ROLE_DATA_ACK>(packet, InternalMessage::gm2tl_query_role_data_ack);
}

std::expected<gm::RoleReply, std::string> gm::decode_loaded_role(std::span<const std::byte> packet)
{
    return decode_role<InternalMessage::GM2TL_LOAD_ROLE_DATA_ACK>(packet, InternalMessage::gm2tl_load_role_data_ack);
}

std::expected<void, std::string> gm::RoleEdits::append(UINT16 type, UINT16 base, std::span<const char> value)
{
    const auto size = sizeof(GM_ROLE_SLOT) + value.size();
    if (size > MAX_GM_ROLE_DATA_STR_LENGTH - bytes_.size())
        return std::unexpected("Too many pending changes. Apply or cancel them before adding more.");
    GM_ROLE_SLOT header;
    header.wLength = static_cast<UINT16>(size);
    header.wRoleType = type;
    header.wBaseDataType = base;
    const auto start = bytes_.size();
    bytes_.resize(start + size);
    std::memcpy(bytes_.data() + start, &header, sizeof(header));
    if (!value.empty())
        std::memcpy(bytes_.data() + start + sizeof(header), value.data(), value.size());
    return {};
}

std::expected<void, std::string> gm::RoleEdits::set_signed(UINT16 base, INT32 value)
{
    if (base != GM_RBD_LEVEL && base != GM_RBD_GMLEVEL && (base < GM_RBD_MONEY_2 || base > GM_RBD_MONEY_14))
        return std::unexpected("role field does not accept signed edits");
    return append(GM_ROLE_BASE_DATA, base, {reinterpret_cast<const char*>(&value), sizeof(value)});
}

std::expected<void, std::string> gm::RoleEdits::set_unsigned(UINT16 base, UINT32 value)
{
    if (base != GM_RBD_EXP && base != GM_RBD_PKVALUE)
        return std::unexpected("role field does not accept unsigned edits");
    return append(GM_ROLE_BASE_DATA, base, {reinterpret_cast<const char*>(&value), sizeof(value)});
}

std::expected<void, std::string> gm::RoleEdits::return_position()
{
    return append(GM_ROLE_BASE_DATA, GM_RBD_MAP_ID, {});
}

std::expected<void, std::string> gm::RoleEdits::remove(UINT16 type, UINT32 id)
{
    if (type != GM_ROLE_ITEM && type != GM_ROLE_QUEST && type != GM_ROLE_MAIL)
        return std::unexpected("role record cannot be removed by this command");
    return append(type, 0, {reinterpret_cast<const char*>(&id), sizeof(id)});
}

std::expected<void, std::string> gm::RoleEdits::take_item(UINT32 slot, UINT64 guid)
{
    std::array<char, sizeof(slot) + sizeof(guid)> value;
    std::memcpy(value.data(), &slot, sizeof(slot));
    std::memcpy(value.data() + sizeof(slot), &guid, sizeof(guid));
    return append(GM_ROLE_TAKE_ITEM, 0, value);
}
