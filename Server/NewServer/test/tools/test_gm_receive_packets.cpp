#include "gm_receive_packets.h"
#include "gm_receive_fixture.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace InternalMessage;
using gm_test::put;
namespace
{
std::size_t checks{};
void check(bool value, const char* message)
{
    ++checks;
    if (!value) throw std::runtime_error(message);
}
void rejection(const std::vector<char>& bytes)
{
    check(!gm::valid_receive_packet(bytes), "invalid packet accepted");
}
template<class T> void bad_field(UINT16 id, std::size_t offset, T value)
{
    auto bytes = gm_test::receive_packet(id);
    put(bytes, offset, value);
    rejection(bytes);
}
template<class Header, class Record> std::vector<char> batch(UINT16 id, INT32 count)
{
    auto bytes = gm_test::receive_packet(id);
    bytes.resize(sizeof(Header) + count * sizeof(Record));
    put(bytes, offsetof(Header, nCount), count);
    return bytes;
}
void fixed_and_prefixes()
{
    std::size_t known{};
    for (UINT32 id = 0; id <= 0xFFFF; ++id)
    {
        auto bytes = gm_test::receive_packet(static_cast<UINT16>(id));
        const bool expected = id == a2a_ping || (id >= gm2tl_register_ack && id <= gm2tl_set_exchange_limit_ack);
        check(gm::valid_receive_packet(bytes) == expected, "registered message coverage");
        if (!expected) continue;
        ++known;
        for (std::size_t size = 0; size < bytes.size(); ++size)
            check(!gm::valid_receive_packet({bytes.data(), size}), "truncated message prefix accepted");
        const auto original = bytes;
        std::vector<char> unaligned(bytes.size() + 1);
        std::ranges::copy(bytes, unaligned.begin() + 1);
        check(gm::valid_receive_packet({unaligned.data() + 1, bytes.size()}), "unaligned packet");
        check(bytes == original, "validation does not change wire bytes");
        bytes.push_back('x');
        check(gm::valid_receive_packet(bytes), "legacy trailing padding accepted");
    }
    check(known == 33, "all 33 message formats covered");
    auto large = gm_test::receive_packet(a2a_ping);
    large.resize(0x10000);
    rejection(large);
}
void counts_and_strings()
{
    for (INT32 value : {std::numeric_limits<INT32>::min(), -1, std::numeric_limits<INT32>::max()})
    {
        bad_field(gm2tl_login_ack, offsetof(GM2TL_LOGIN_ACK, nMSCount), value);
        bad_field(gm2tl_item_list_ntf, offsetof(GM2TL_ITEM_LIST_NTF, nCount), value);
        bad_field(gm2tl_query_role_id_ack, offsetof(GM2TL_QUERY_ROLE_ID_ACK, nCount), value);
        bad_field(gm2tl_get_announce_list_ack, offsetof(GM2TL_GET_ANNOUNCE_LIST_ACK, nCount), value);
        bad_field(gm2tl_query_exchange_log_ack, offsetof(GM2TL_QUERY_EXCHANGE_LOG_ACK, nCount), value);
        bad_field(gm2tl_query_exchange_history_ack, offsetof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK, nCount), value);
    }
    for (INT32 value : {-1, 2, std::numeric_limits<INT32>::max()})
    {
        bad_field(gm2tl_verify_ack, offsetof(GM2TL_VERIFY_ACK, bLD), value);
        bad_field(gm2tl_item_list_ntf, offsetof(GM2TL_ITEM_LIST_NTF, bCLS), value);
        bad_field(gm2tl_log_ack, offsetof(GM2TL_LOG_ACK, bTruncated), value);
    }
    for (UINT8 value : {UINT8{2}, UINT8{255}})
    {
        bad_field(gm2tl_query_role_id_ack, offsetof(GM2TL_QUERY_ROLE_ID_ACK, byIsEnd), value);
        bad_field(gm2tl_query_exchange_log_ack, offsetof(GM2TL_QUERY_EXCHANGE_LOG_ACK, byIsClear), value);
        bad_field(gm2tl_query_exchange_history_ack, offsetof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK, byIsClear), value);
    }
    for (INT32 count = 1; count <= 8; ++count)
    {
        auto items = batch<GM2TL_ITEM_LIST_NTF, InternalMessage::ITEM_TEMPLATE_DATA>(gm2tl_item_list_ntf, count);
        auto roles = batch<GM2TL_QUERY_ROLE_ID_ACK, GM_ROLE_KEY_INFO>(gm2tl_query_role_id_ack, count);
        auto announcements = batch<GM2TL_GET_ANNOUNCE_LIST_ACK, GMTL_ANNOUNCE_DATA>(gm2tl_get_announce_list_ack, count);
        auto logs = batch<GM2TL_QUERY_EXCHANGE_LOG_ACK, GM_EXCHANGE_PRICE_LOG>(gm2tl_query_exchange_log_ack, count);
        auto history = batch<GM2TL_QUERY_EXCHANGE_HISTORY_ACK, GM_EXCHANGE_PRICE_HISTORY>(gm2tl_query_exchange_history_ack, count);
        for (auto* bytes : {&items, &roles, &announcements, &logs, &history})
        {
            check(gm::valid_receive_packet(*bytes), "complete record batch");
            for (std::size_t size = 0; size < bytes->size(); ++size)
                check(!gm::valid_receive_packet({bytes->data(), size}), "truncated record batch");
        }
        const auto corrupt_text = [&](auto& bytes, std::size_t offset, std::size_t length)
        {
            const auto original = bytes;
            std::fill_n(bytes.data() + offset, length, 'x');
            rejection(bytes);
            bytes[offset + length - 1] = '\0';
            check(gm::valid_receive_packet(bytes), "last-byte string terminator accepted");
            bytes = original;
        };
        corrupt_text(items, sizeof(GM2TL_ITEM_LIST_NTF) + offsetof(InternalMessage::ITEM_TEMPLATE_DATA, szName), MAX_ITEM_NAME_LEN);
        corrupt_text(roles, sizeof(GM2TL_QUERY_ROLE_ID_ACK) + offsetof(GM_ROLE_KEY_INFO, szRoleName), ROLE_NAME_LEN);
        corrupt_text(roles, sizeof(GM2TL_QUERY_ROLE_ID_ACK) + offsetof(GM_ROLE_KEY_INFO, szUserName), USER_NAME_LEN);
        corrupt_text(announcements, sizeof(GM2TL_GET_ANNOUNCE_LIST_ACK) + offsetof(GMTL_ANNOUNCE_DATA, szText), GM_ANNOUNCE_LENGTH);
        corrupt_text(logs, sizeof(GM2TL_QUERY_EXCHANGE_LOG_ACK) + offsetof(GM_EXCHANGE_PRICE_LOG, szExchangeTime), 21);
        corrupt_text(history, sizeof(GM2TL_QUERY_EXCHANGE_HISTORY_ACK) + offsetof(GM_EXCHANGE_PRICE_HISTORY, szHistoryTime), 21);
    }
    auto history = batch<GM2TL_QUERY_EXCHANGE_HISTORY_ACK, GM_EXCHANGE_PRICE_HISTORY>(gm2tl_query_exchange_history_ack, MAX_EXCHANGE_HISTORY_NUM);
    check(gm::valid_receive_packet(history), "full history fits widget capacity");
    history = batch<GM2TL_QUERY_EXCHANGE_HISTORY_ACK, GM_EXCHANGE_PRICE_HISTORY>(gm2tl_query_exchange_history_ack, MAX_EXCHANGE_HISTORY_NUM + 1);
    rejection(history);
    auto login = gm_test::receive_packet(gm2tl_login_ack);
    put(login, offsetof(GM2TL_LOGIN_ACK, nMSCount), INT32{MAX_MS_SESSION_COUNT});
    check(gm::valid_receive_packet(login), "full server list");
    put(login, offsetof(GM2TL_LOGIN_ACK, nMSCount), INT32{MAX_MS_SESSION_COUNT + 1});
    rejection(login);
    put(login, offsetof(GM2TL_LOGIN_ACK, nMSCount), INT32{1});
    std::fill_n(login.data() + offsetof(GM2TL_LOGIN_ACK, stMSInfo) + offsetof(MS_INFO, szMSName), 33, 'x');
    rejection(login);
    auto prequery = gm_test::receive_packet(gm2tl_pre_query_ack);
    std::fill_n(prequery.data() + offsetof(GM2TL_PRE_QUERY_ACK, szRoleName), ROLE_NAME_LEN, 'x');
    rejection(prequery);
    auto error = gm_test::receive_packet(gm2tl_error_str);
    error.back() = 'x';
    rejection(error);
    for (INT32 group : {-1, MAX_SERVER_GROUP_COUNT})
        bad_field(gm2tl_get_announce_list_ack, offsetof(GM2TL_GET_ANNOUNCE_LIST_ACK, nGroupID), group);
}
void exchange_and_roles()
{
    for (UINT8 count : {UINT8{0}, UINT8{40}, UINT8{41}, UINT8{255}})
    {
        auto bytes = gm_test::receive_packet(gm2tl_exchange_price);
        bytes.resize(sizeof(GM2TL_EXCHANGE_PRICE) + count * sizeof(GMTL_EXCHANGE_PRICE_INFO));
        put(bytes, offsetof(GM2TL_EXCHANGE_PRICE, bySellPriceCount), count);
        check(gm::valid_receive_packet(bytes) == (count <= 40), "price row capacity");
        put(bytes, offsetof(GM2TL_EXCHANGE_PRICE, byBuyPriceCount), UINT8{255});
        rejection(bytes);
    }
    auto detail = gm_test::receive_packet(gm2tl_query_order_detail_ack);
    put(detail, offsetof(GM2TL_QUERY_ORDER_DETAIL_ACK, byCountCount), UINT8{1});
    rejection(detail);
    detail.resize(sizeof(GM2TL_QUERY_ORDER_DETAIL_ACK) + sizeof(GM_EXCHANGE_COUNT_INFO));
    check(gm::valid_receive_packet(detail), "complete order detail");
    std::fill_n(detail.data() + sizeof(GM2TL_QUERY_ORDER_DETAIL_ACK) + offsetof(GM_EXCHANGE_COUNT_INFO, szRoleName), ROLE_NAME_LEN, 'x');
    rejection(detail);
    for (UINT16 id : {gm2tl_query_role_data_ack, gm2tl_load_role_data_ack})
    {
        const auto offset = id == gm2tl_query_role_data_ack ? offsetof(GM2TL_QUERY_ROLE_DATA_ACK, nLength) : offsetof(GM2TL_LOAD_ROLE_DATA_ACK, nLength);
        for (INT32 length : {-1, 0, 1, 32000, 32001, std::numeric_limits<INT32>::max()})
        {
            auto bytes = gm_test::receive_packet(id);
            const auto prefix = bytes.size();
            bytes.resize(prefix + 32001);
            put(bytes, offset, length);
            check(gm::valid_receive_packet(bytes) == (length >= 0 && length <= 32000), "role buffer capacity");
            if (length > 0 && length <= 32000)
            {
                bytes.resize(prefix + length - 1);
                rejection(bytes);
            }
        }
    }
}
void log_compatibility()
{
    auto bytes = gm_test::receive_packet(gm2tl_log_ack);
    check(gm::decode_log_reply(bytes)->text.empty(), "legacy empty log omits NUL");
    const std::string text = "legacy UTF-8 log: \xE4\xB8\xAD";
    bytes.insert(bytes.end(), text.begin(), text.end());
    put(bytes, offsetof(GM2TL_LOG_ACK, nLength), static_cast<INT32>(text.size() + 1));
    const auto original = bytes;
    auto reply = gm::decode_log_reply(bytes);
    check(reply && reply->text == text && !reply->truncated, "legacy missing-NUL writer preserves all text");
    check(bytes == original, "log decoder leaves received bytes unchanged");
    bytes.push_back('\0');
    check(gm::decode_log_reply(bytes)->text == text, "corrected NUL-included writer also accepted");
    bytes.back() = 'x';
    check(gm::decode_log_reply(bytes)->text == text, "receiver supplies owned terminator");
    bytes[sizeof(GM2TL_LOG_ACK) + 3] = '\0';
    check(gm::decode_log_reply(bytes)->text == "leg", "embedded NUL retains visible legacy prefix");
    for (INT32 length : {std::numeric_limits<INT32>::min(), -1, 0, std::numeric_limits<INT32>::max()})
    {
        put(bytes, offsetof(GM2TL_LOG_ACK, nLength), length);
        rejection(bytes);
        check(!gm::decode_log_reply(bytes), "invalid log length rejected by owned decoder");
    }
    bytes = original;
    bytes.pop_back();
    rejection(bytes);
    check(!gm::decode_log_reply(bytes), "truncated declared text rejected");
}
}

int main()
{
    try
    {
        fixed_and_prefixes();
        counts_and_strings();
        exchange_and_roles();
        log_compatibility();
        std::cout << checks << " GM receive payload checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
