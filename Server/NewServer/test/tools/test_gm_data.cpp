#include "data_mgr.h"
#include "gm_data_packets.h"
#include "gmtl_message.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
std::size_t checks;
void check(bool condition, const char* message)
{
    ++checks;
    if (!condition)
        throw std::runtime_error(message);
}
template <class Header, class Record>
std::vector<std::byte> packet(Header header, std::span<const Record> records)
{
    std::vector<std::byte> bytes(sizeof(header) + records.size_bytes());
    std::memcpy(bytes.data(), &header, sizeof(header));
    if (!records.empty())
        std::memcpy(bytes.data() + sizeof(header), records.data(), records.size_bytes());
    return bytes;
}
ITEM_TEMPLATE_DATA item(UINT32 id, const std::string& name)
{
    ITEM_TEMPLATE_DATA value{};
    value.dwTempID = id;
    std::copy(name.begin(), name.end(), value.szName);
    return value;
}
void roles(CDataMgr& data)
{
    check(!data.StoreRole(1, {}) && !data.FindRole(1) && !data.ClearRole(), "role access before initialization");
    check(data.Init(), "initialize manager");
    auto* first = data.StoreRole(0, {});
    check(first && first->dwRoleID == 0 && first->document.empty(), "zero-initialized role");
    first->document.push_back({999, 0, {'x', 'y'}});
    for (UINT32 id = 1; id <= 512; ++id)
    {
        auto* role = data.StoreRole(id, {});
        check(role && role->dwRoleID == id && role->document.empty(), "new role beyond old pool chunk");
        check(data.FindRole(0) == first && first->document.front().payload == std::vector<char>{'x', 'y'}, "stable role across container growth");
    }
    check(data.FindRole(512) && !data.FindRole(999), "role lookup");
    auto* replacement = data.StoreRole(0, {});
    check(replacement && replacement != first && replacement->dwRoleID == 0 && replacement->document.empty(), "owned replacement initializes all bytes");
    auto foreign = std::make_unique<GM_ROLE_DATA>();
    check(!data.DeleteRole(nullptr) && !data.DeleteRole(foreign.get()), "reject foreign role ownership");
    check(data.DeleteRole(replacement) && !data.FindRole(0), "delete owned role");
    check(!data.DeleteRole(replacement), "reject repeated deletion without dereferencing pointer");
    check(data.ClearRole() && !data.FindRole(512), "clear roles");
    check(data.StoreRole((std::numeric_limits<UINT32>::max)(), {}) != nullptr, "full-width role ID");
}

void items(CDataMgr& data)
{
    const std::array input{item(2, "second"), item(1, "first"), item(2, "ignored duplicate")};
    check(data.ApplyItems(0, true, input), "load item batch");
    auto owned = data.GetItems(0);
    check(owned.size() == 2 && owned[0].dwTempID == 1 && owned[1].dwTempID == 2 && std::string(owned[1].szName) == "second", "sorted item IDs and first duplicate wins");
    const auto extra = item(3, "third");
    check(data.ApplyItems(0, false, {&extra, 1}), "append item batch");
    check(data.ApplyItems(0, false, input), "append duplicate batch");
    check(data.GetItems(0).size() == 3 && owned.size() == 2, "snapshot survives append");
    auto bad = input;
    std::fill_n(bad.back().szName, sizeof(bad.back().szName), 'x');
    check(!data.ApplyItems(0, true, bad) && data.GetItems(0).size() == 3, "bad replace preserves entire map");
    check(!data.ApplyItems(0, false, bad) && data.GetItems(0).size() == 3, "bad append publishes no partial data");
    check(!data.ApplyItems(MAX_GM_SESSION_COUNT, true, input) && data.GetItems(MAX_GM_SESSION_COUNT).empty(), "invalid session bounds");
    data.ClearItemMap(MAX_GM_SESSION_COUNT);
    check(data.GetItems(0).size() == 3, "invalid clear leaves valid session intact");
    auto maximum = item((std::numeric_limits<UINT32>::max)(), std::string(MAX_ITEM_NAME_LEN - 1, 'm'));
    check(data.ApplyItems(MAX_GM_SESSION_COUNT - 1, true, {&maximum, 1}), "last session and maximum name length");
    data.ClearItemMap(0);
    check(data.GetItems(0).empty() && owned.size() == 2 && std::string(owned[0].szName) == "first", "snapshot survives clear");
    for (UINT32 session = 0; session < MAX_GM_SESSION_COUNT; ++session)
        check(data.ApplyItems(session, true, input), "populate every session map");
    check(data.Init(), "reinitialize populated manager");
    for (UINT32 session = 0; session < MAX_GM_SESSION_COUNT; ++session)
        check(data.GetItems(session).empty(), "reinitialization clears every map");
    check(!data.FindRole((std::numeric_limits<UINT32>::max)()), "reinitialize clears role owner");
}

void announcements(CDataMgr& data)
{
    std::vector<TL_ANNOUNCE_DATA> records(200);
    for (std::size_t index = 0; index < records.size(); ++index)
    {
        auto& record = records[index];
        record.nGroupID = index % 32;
        record.nSvrID = static_cast<INT32>(index + 1);
        record.dwSerial = static_cast<UINT32>(index + 1);
        record.dwIntervalTime = static_cast<UINT32>(100 + index);
        record.dwRepeatNum = static_cast<UINT32>(200 + index);
        std::copy_n("message", 7, record.szText);
    }
    check(data.ReplaceAnnounceList(records) && data.GetAnnounceList().size() == 200, "owned announcement list exceeds old fixed capacity");
    const auto snapshot = data.GetAnnounceList();
    check(data.DelAnnounceFromList(1), "delete first announcement");
    const auto& moved = data.GetAnnounceList().front();
    check(data.GetAnnounceList().size() == 199 && moved.nGroupID == records.back().nGroupID && moved.nSvrID == records.back().nSvrID &&
          moved.dwSerial == records.back().dwSerial && moved.dwIntervalTime == records.back().dwIntervalTime && moved.dwRepeatNum == records.back().dwRepeatNum &&
          std::string(moved.szText) == records.back().szText, "whole record preserves moved identity");
    check(snapshot.size() == 200 && snapshot.front().dwSerial == 1, "owned announcement snapshot");
    check(data.DelAnnounceFromList(200) && data.GetAnnounceList().size() == 198, "delete moved record");
    check(data.DelAnnounceFromList(999) && data.GetAnnounceList().size() == 198, "missing deletion is idempotent");
    auto invalid = records;
    for (const auto group : {-1, 32})
    {
        invalid.back().nGroupID = group;
        check(!data.ReplaceAnnounceList(invalid) && data.GetAnnounceList().size() == 198, "reject bad group without partial publication");
    }
    invalid = records;invalid.back().nSvrID = 0;
    check(!data.ReplaceAnnounceList(invalid), "reject absent server identity");
    invalid = records;std::fill_n(invalid.back().szText, sizeof(invalid.back().szText), 'x');
    check(!data.ReplaceAnnounceList(invalid), "reject unterminated announcement text");
    check(data.ReplaceAnnounceList(data.GetAnnounceList()) && data.GetAnnounceList().size() == 198, "self-sourced staged replacement");
    check(data.ReplaceAnnounceList({}) && data.GetAnnounceList().empty(), "empty reply clears stale announcements");
}

void packets(CDataMgr& data)
{
    InternalMessage::GM2TL_ITEM_LIST_NTF header{};
    header.wMsg = InternalMessage::gm2tl_item_list_ntf;header.bCLS = TRUE;header.nCount = 2;
    std::array<InternalMessage::ITEM_TEMPLATE_DATA, 2> input{};
    input[0].dwTempID = 29;input[1].dwTempID = 31;
    std::copy_n("first", 5, input[0].szName);std::copy_n("last", 4, input[1].szName);
    auto bytes = packet(header, std::span<const InternalMessage::ITEM_TEMPLATE_DATA>(input));
    for (std::size_t size = 0; size < bytes.size(); ++size)
        check(!gm::decode_item_batch(std::span(bytes).first(size)), "reject every truncated item packet");
    auto decoded = gm::decode_item_batch(bytes);
    check(decoded && decoded->clear && decoded->items.size() == 2 && decoded->items[1].dwTempID == 31 && std::string(decoded->items[1].szName) == "last", "decode item values");
    check(data.ApplyItems(9, decoded->clear, decoded->items) && data.GetItems(9).size() == 2, "publish decoded items");
    bytes.insert(bytes.begin(), std::byte{0});
    check(gm::decode_item_batch(std::span(bytes).subspan(1)).has_value(), "unaligned item packet");
    for (const auto count : {-1, (std::numeric_limits<INT32>::max)()})
    {
        header.nCount = count;
        check(!gm::decode_item_batch(packet(header, std::span<const InternalMessage::ITEM_TEMPLATE_DATA>(input))), "reject invalid item count");
    }
    header.nCount = 2;header.bCLS = 2;
    check(!gm::decode_item_batch(packet(header, std::span<const InternalMessage::ITEM_TEMPLATE_DATA>(input))), "reject invalid clear flag");
    header.bCLS = TRUE;header.wMsg = 0;
    check(!gm::decode_item_batch(packet(header, std::span<const InternalMessage::ITEM_TEMPLATE_DATA>(input))), "reject incorrect item message ID");
    header.wMsg = InternalMessage::gm2tl_item_list_ntf;
    std::fill_n(input.back().szName, sizeof(input.back().szName), 'x');
    check(!gm::decode_item_batch(packet(header, std::span<const InternalMessage::ITEM_TEMPLATE_DATA>(input))) && data.GetItems(9).size() == 2, "bad final record preserves previous item list");
    header.nCount = 0;
    auto empty = gm::decode_item_batch(packet(header, std::span<const InternalMessage::ITEM_TEMPLATE_DATA>{}));
    check(empty && empty->items.empty() && data.ApplyItems(9, empty->clear, empty->items) && data.GetItems(9).empty(), "empty clear item packet");

    InternalMessage::GM2TL_GET_ANNOUNCE_LIST_ACK announce{};
    announce.wMsg = InternalMessage::gm2tl_get_announce_list_ack;announce.nCount = 2;announce.nGroupID = 31;
    std::array<InternalMessage::GMTL_ANNOUNCE_DATA, 2> announcements{};
    announcements[0].nGroupID = 0;announcements[0].dwSerial = 1;
    announcements[1].nGroupID = 31;announcements[1].dwSerial = 2;
    announcements[1].dwIntervalTime = 600;announcements[1].dwRepeatNum = 7;
    std::copy_n("last group", 10, announcements[1].szText);
    auto message = packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>(announcements));
    for (std::size_t size = 0; size < message.size(); ++size)
        check(!gm::decode_announcements(std::span(message).first(size), 99), "reject every truncated announcement packet");
    auto list = gm::decode_announcements(message, 99);
    check(list && list->size() == 2 && list->back().nGroupID == 31 && list->back().nSvrID == 99 && list->back().dwSerial == 2 &&
          list->back().dwIntervalTime == 600 && list->back().dwRepeatNum == 7 && std::string(list->back().szText) == "last group", "decode announcement fields");
    check(data.ReplaceAnnounceList(*list), "publish decoded announcements");
    message.insert(message.begin(), std::byte{0});
    check(gm::decode_announcements(std::span(message).subspan(1), 99).has_value(), "unaligned announcement packet");
    check(!gm::decode_announcements(std::span(message).subspan(1), 0), "reject invalid announcement server");
    for (const auto count : {-1, (std::numeric_limits<INT32>::max)()})
    {
        announce.nCount = count;
        check(!gm::decode_announcements(packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>(announcements)), 99), "reject invalid announcement count");
    }
    announce.nCount = 2;announce.wMsg = 0;
    check(!gm::decode_announcements(packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>(announcements)), 99), "reject wrong announcement message ID");
    announce.wMsg = InternalMessage::gm2tl_get_announce_list_ack;announce.nGroupID = 32;
    check(!gm::decode_announcements(packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>(announcements)), 99), "reject wrong reply group");
    announce.nGroupID = 31;announcements.back().nGroupID = 32;
    check(!gm::decode_announcements(packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>(announcements)), 99), "reject wrong record group");
    announcements.back().nGroupID = 31;std::fill_n(announcements.back().szText, sizeof(announcements.back().szText), 'x');
    check(!gm::decode_announcements(packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>(announcements)), 99) && data.GetAnnounceList().size() == 2, "bad announcement preserves stored data");
    announce.nCount = 0;
    auto none = gm::decode_announcements(packet(announce, std::span<const InternalMessage::GMTL_ANNOUNCE_DATA>{}), 99);
    check(none && none->empty() && data.ReplaceAnnounceList(*none) && data.GetAnnounceList().empty(), "empty announcement packet");
}
}

int main()
try
{
    auto& data = CDataMgr::GetInstance();
    data.UnInit();
    roles(data);items(data);announcements(data);packets(data);
    check(data.UnInit() && data.UnInit(), "idempotent shutdown");
    check(!data.StoreRole(1, {}) && data.GetItems(9).empty() && data.GetAnnounceList().empty(), "shutdown releases owned state");
    std::cout << checks << " GM data checks passed\n";
    return 0;
}
catch (const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
