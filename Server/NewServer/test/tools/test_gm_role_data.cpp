#include "gm_role_fixture.h"
#include "data_mgr.h"
#include "gmtl_message.h"
#include <iostream>

namespace
{
std::size_t checks;
void check(bool condition, const char* message)
{
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
template <class Header>
std::vector<std::byte> reply(UINT16 id, const std::vector<char>& data)
{
    Header header{};
    header.wMsg = id;header.nLength = static_cast<INT32>(data.size());header.nAreaID = 7;header.dwRoleID = 93;
    if constexpr (requires { header.bIsOnline; }) header.bIsOnline = TRUE;
    constexpr auto prefix = offsetof(Header, szData);
    std::vector<std::byte> bytes(prefix + data.size());
    std::memcpy(bytes.data(), &header, prefix);
    if (!data.empty()) std::memcpy(bytes.data() + prefix, data.data(), data.size());
    return bytes;
}

void parsing()
{
    const auto original = role_fixture::sample();
    auto parsed = gm::parse_role_data(original);
    check(parsed && parsed->size() == 17, "representative maintained role format");
    check((*parsed)[1].text() == std::string(ROLE_NAME_LEN - 1, 'n'), "name without terminator bounded by its slot");
    check((*parsed)[0].value<UINT32>() == 4294967295 && (*parsed)[9].value<INT64>() == (std::numeric_limits<INT64>::min)(), "full-width numeric fields");
    check((*parsed)[13].value<GM_ROLE_ITEM_SLOT>().qwGuid == (std::numeric_limits<UINT64>::max)() &&
          (*parsed)[13].text(offsetof(GM_ROLE_ITEM_SLOT, szData)) == "item", "item attributes and native tail layout");
    check(parsed->back().type == 999 && parsed->back().payload == std::vector<char>{'x','y','z'}, "unknown record preserved");
    auto unaligned = original;unaligned.insert(unaligned.begin(), '!');
    check(gm::parse_role_data(std::span(unaligned).subspan(1)).has_value(), "unaligned role buffer");
    auto independent = *parsed;
    parsed->clear();
    check(independent[1].text() == std::string(ROLE_NAME_LEN - 1, 'n'), "owned role snapshot");
    check(gm::parse_role_data({}).has_value(), "empty role document");
    check(!gm::parse_role_data(std::vector<char>(MAX_GM_ROLE_DATA_STR_LENGTH + 1)), "oversized document");
    for (const UINT16 length : {UINT16{0}, UINT16{1}, UINT16{5}, UINT16{7}, UINT16{65535}})
    {
        auto header = role_fixture::record(999, 0, {});
        std::memcpy(header.data(), &length, sizeof(length));
        check(!gm::parse_role_data(header), "invalid record size");
    }
    for (std::size_t size = 1; size < sizeof(GM_ROLE_SLOT); ++size)
        check(!gm::parse_role_data(std::span(original).first(size)), "partial record header");
    for (const auto base : {GM_RBD_ID, GM_RBD_LEVEL, GM_RBD_GUILD_ID, GM_RBD_MONEY_14})
        for (std::size_t size = 0; size < 4; ++size)
            check(!gm::parse_role_data(role_fixture::record(GM_ROLE_BASE_DATA, base, std::vector<char>(size))), "short numeric field");
    for (std::size_t size = 0; size < 8; ++size)
        check(!gm::parse_role_data(role_fixture::record(GM_ROLE_BASE_DATA, GM_RBD_X, std::vector<char>(size))), "short coordinate field");
    check(!gm::parse_role_data(role_fixture::record(GM_ROLE_BASE_DATA, GM_RBD_NAME, std::string(ROLE_NAME_LEN, 'n'))), "overlong role name");
    check(!gm::parse_role_data(role_fixture::record(GM_ROLE_BASE_DATA, GM_RBD_LAST_LOGOUT_TIME, std::string(21, 't'))), "unterminated timestamp");
    std::vector<char> long_quest(sizeof(UINT32) + COMMON_NAME_LEN, 'q');
    long_quest.back() = 0;
    check(gm::parse_role_data(role_fixture::record(GM_ROLE_QUEST, 0, long_quest)).has_value(), "maintained 255-byte quest-name capacity");
    for (const auto type : {GM_ROLE_SPELL, GM_ROLE_TITLE, GM_ROLE_ITEM, GM_ROLE_QUEST, GM_ROLE_MAIL})
    {
        const auto& valid = *std::ranges::find(independent, type, &gm::RoleRecord::type);
        check(!gm::parse_role_data(role_fixture::record(type, 0, {})), "missing typed payload");
        auto bad = valid.payload;
        std::size_t text_offset = 0;
        if (type == GM_ROLE_SPELL) text_offset = offsetof(GM_ROLE_SPELL_SLOT, szName);
        if (type == GM_ROLE_TITLE) text_offset = offsetof(GM_ROLE_TITLE_SLOT, szName);
        if (type == GM_ROLE_ITEM) text_offset = offsetof(GM_ROLE_ITEM_SLOT, szData);
        if (type == GM_ROLE_QUEST) text_offset = sizeof(UINT32);
        if (type == GM_ROLE_MAIL) text_offset = offsetof(GM_ROLE_MAIL_SLOT, szData);
        std::fill(bad.begin() + text_offset, bad.end(), 'x');
        check(!gm::parse_role_data(role_fixture::record(type, 0, bad)), "unterminated typed text");
    }
    const auto& mail = *std::ranges::find(independent, GM_ROLE_MAIL, &gm::RoleRecord::type);
    for (const auto offset : {offsetof(GM_ROLE_MAIL_SLOT, nMailType), offsetof(GM_ROLE_MAIL_SLOT, nState)})
    {
        for (const INT32 value : {-1, 2})
        {
            auto bad = mail.payload;std::memcpy(bad.data() + offset, &value, sizeof(value));
            check(!gm::parse_role_data(role_fixture::record(GM_ROLE_MAIL, 0, bad)), "mail display index bounds");
        }
    }
    bool bounded_value = false, bounded_text = false;
    try { independent[1].value<UINT64>(100); } catch (const std::out_of_range&) { bounded_value = true; }
    try { independent[1].text(100); } catch (const std::out_of_range&) { bounded_text = true; }
    check(bounded_value && bounded_text, "record access remains bounded");
}

void packets()
{
    const auto document = role_fixture::sample();
    auto query = reply<InternalMessage::GM2TL_QUERY_ROLE_DATA_ACK>(InternalMessage::gm2tl_query_role_data_ack, document);
    auto loaded = reply<InternalMessage::GM2TL_LOAD_ROLE_DATA_ACK>(InternalMessage::gm2tl_load_role_data_ack, document);
    for (std::size_t size = 0; size < query.size(); ++size)
        check(!gm::decode_query_role(std::span(query).first(size)), "every truncated query reply");
    for (std::size_t size = 0; size < loaded.size(); ++size)
        check(!gm::decode_loaded_role(std::span(loaded).first(size)), "every truncated loaded-role reply");
    auto first = gm::decode_query_role(query);auto second = gm::decode_loaded_role(loaded);
    check(first && first->role_id == 93 && first->area_id == 7 && first->online && first->document.size() == 17, "query reply metadata");
    check(second && !second->online && second->document.size() == 17, "loaded-role reply metadata");
    query.insert(query.begin(), std::byte{0});
    check(gm::decode_query_role(std::span(query).subspan(1)).has_value(), "unaligned query reply");
    query.erase(query.begin());
    for (const INT32 length : {-1, 32001, (std::numeric_limits<INT32>::max)()})
    {
        auto bad = query;std::memcpy(bad.data() + offsetof(InternalMessage::GM2TL_QUERY_ROLE_DATA_ACK, nLength), &length, sizeof(length));
        check(!gm::decode_query_role(bad), "reply length exceeds fixed wire capacity");
    }
    const UINT32 zero = 0;
    auto bad = query;std::memcpy(bad.data() + offsetof(InternalMessage::GM2TL_QUERY_ROLE_DATA_ACK, dwRoleID), &zero, sizeof(zero));
    check(!gm::decode_query_role(bad), "zero role reply ID");
    bad = query;const BOOL invalid_flag = 2;
    std::memcpy(bad.data() + offsetof(InternalMessage::GM2TL_QUERY_ROLE_DATA_ACK, bIsOnline), &invalid_flag, sizeof(invalid_flag));
    check(!gm::decode_query_role(bad), "online flag bounds");
    check(!gm::decode_loaded_role(query) && !gm::decode_query_role(loaded), "reply message identity");
    auto& manager = CDataMgr::GetInstance();manager.Init();
    auto* stored = manager.StoreRole(first->role_id, std::move(first->document));
    check(stored && stored->document.size() == 17, "store owned parsed records");
    auto corrupt = document;corrupt[0] = 0;corrupt[1] = 0;
    check(!gm::decode_query_role(reply<InternalMessage::GM2TL_QUERY_ROLE_DATA_ACK>(InternalMessage::gm2tl_query_role_data_ack, corrupt)) &&
          manager.FindRole(93) == stored && stored->document.size() == 17, "bad payload preserves stored role");
    manager.UnInit();
}

void edits()
{
    gm::RoleEdits edits;
    std::vector<char> expected;
    check(edits.set_signed(GM_RBD_LEVEL, -1).has_value(), "signed edit");
    role_fixture::append(expected, role_fixture::scalar(GM_RBD_LEVEL, INT32{-1}));
    check(edits.set_unsigned(GM_RBD_EXP, 4294967295).has_value(), "unsigned edit");
    role_fixture::append(expected, role_fixture::scalar(GM_RBD_EXP, UINT32{4294967295}));
    check(edits.return_position().has_value(), "return-position command");
    role_fixture::append(expected, role_fixture::record(GM_ROLE_BASE_DATA, GM_RBD_MAP_ID, {}));
    for (const auto type : {GM_ROLE_ITEM, GM_ROLE_QUEST, GM_ROLE_MAIL})
    {
        check(edits.remove(type, 4294967295).has_value(), "remove command");
        const UINT32 id = 4294967295;
        role_fixture::append(expected, role_fixture::record(type, 0, {reinterpret_cast<const char*>(&id), sizeof(id)}));
    }
    check(edits.take_item(4294967295, (std::numeric_limits<UINT64>::max)()).has_value(), "full-width transfer command");
    const std::array<char, 12> all_bits{char(-1),char(-1),char(-1),char(-1),char(-1),char(-1),char(-1),char(-1),char(-1),char(-1),char(-1),char(-1)};
    role_fixture::append(expected, role_fixture::record(GM_ROLE_TAKE_ITEM, 0, all_bits));
    check(std::ranges::equal(edits.bytes(), expected), "legacy edit bytes and zeroed unused header fields");
    check(!edits.set_signed(GM_RBD_NAME, 1) && !edits.set_signed(GM_RBD_MONEY_1, 1) && !edits.set_unsigned(GM_RBD_LEVEL, 1) && !edits.remove(GM_ROLE_SPELL, 1), "reject unsupported edits");
    check(std::ranges::equal(edits.bytes(), expected), "failed edits preserve pending bytes");
    edits.clear();check(edits.bytes().empty(), "clear edit storage");
    for (unsigned index = 0; index < 3275; ++index)
        check(edits.set_signed(GM_RBD_LEVEL, index).has_value(), "fill edit buffer");
    check(edits.take_item(0, 0).has_value() && edits.bytes().size() == MAX_GM_ROLE_DATA_STR_LENGTH, "exact full buffer");
    const std::vector<char> full(edits.bytes().begin(), edits.bytes().end());
    check(!edits.return_position() && !edits.set_unsigned(GM_RBD_EXP, 1) && std::ranges::equal(edits.bytes(), full), "full buffer rejects without partial records");
    edits.clear();check(edits.return_position().has_value() && edits.bytes().size() == 6, "reuse cleared builder");
}
}

int main()
try
{
    parsing();packets();edits();
    std::cout << checks << " GM role-data checks passed\n";
    return 0;
}
catch (const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
