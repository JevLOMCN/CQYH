#ifndef CQYH_TEST_GM_ROLE_FIXTURE_H
#define CQYH_TEST_GM_ROLE_FIXTURE_H
#include "gm_role_data.h"
#include <array>
#include <limits>

namespace role_fixture
{
inline std::vector<char> record(UINT16 type, UINT16 base, std::span<const char> payload)
{
    const std::array<UINT16, 3> header{static_cast<UINT16>(6 + payload.size()), type, base};
    std::vector<char> bytes(6 + payload.size());
    std::memcpy(bytes.data(), header.data(), 6);
    if (!payload.empty()) std::memcpy(bytes.data() + 6, payload.data(), payload.size());
    return bytes;
}
template <class T> std::vector<char> scalar(UINT16 base, T value)
{
    return record(GM_ROLE_BASE_DATA, base, {reinterpret_cast<const char*>(&value), sizeof(value)});
}
inline void append(std::vector<char>& all, const std::vector<char>& part) { all.insert(all.end(), part.begin(), part.end()); }
inline std::vector<char> sample()
{
    std::vector<char> bytes;
    append(bytes, scalar(GM_RBD_ID, UINT32{4294967295}));
    append(bytes, record(GM_ROLE_BASE_DATA, GM_RBD_NAME, std::string(ROLE_NAME_LEN - 1, 'n')));
    append(bytes, scalar(GM_RBD_GROUPID, INT32{1}));
    append(bytes, scalar(GM_RBD_CLASS, INT32{2}));
    append(bytes, scalar(GM_RBD_SEX, INT32{1}));
    append(bytes, scalar(GM_RBD_LEVEL, INT32{57}));
    append(bytes, scalar(GM_RBD_EXP, UINT32{4294967295}));
    append(bytes, scalar(GM_RBD_PKVALUE, UINT32{4000000000}));
    append(bytes, scalar(GM_RBD_MONEY_2, (std::numeric_limits<INT32>::min)()));
    append(bytes, scalar(GM_RBD_X, (std::numeric_limits<INT64>::min)()));
    std::array<char, 21> time{};
    std::copy_n("2026-10-29 12:34:56", 19, time.data());
    append(bytes, record(GM_ROLE_BASE_DATA, GM_RBD_LAST_LOGOUT_TIME, time));
    GM_ROLE_SPELL_SLOT spell{};spell.nSpellID = 17;spell.nRuneID = 23;spell.nEquipedIndex = 0;
    std::copy_n("spell", 5, spell.szName);
    append(bytes, record(GM_ROLE_SPELL, 0, {reinterpret_cast<const char*>(&spell), sizeof(spell)}));
    GM_ROLE_TITLE_SLOT title{};title.nTemplateID = 19;title.dwExpireTime = 4294967295;
    std::copy_n("title", 5, title.szName);
    append(bytes, record(GM_ROLE_TITLE, 0, {reinterpret_cast<const char*>(&title), sizeof(title)}));
    GM_ROLE_ITEM_SLOT item;
    std::memset(&item, 0, sizeof(item));
    item.dwItemID = 31;item.qwGuid = (std::numeric_limits<UINT64>::max)();
    item.wConSlot = 65535;item.wItemSlot = 65535;item.nNum = 2147483647;item.byLuck = -3;
    std::vector<char> item_data(sizeof(item) + 5, 0);
    std::memcpy(item_data.data(), &item, sizeof(item));
    std::copy_n("item", 4, item_data.data() + offsetof(GM_ROLE_ITEM_SLOT, szData));
    append(bytes, record(GM_ROLE_ITEM, 0, item_data));
    const UINT32 quest = 4294967295;
    std::vector<char> quest_data(sizeof(quest) + 6, 0);
    std::memcpy(quest_data.data(), &quest, sizeof(quest));
    std::copy_n("quest", 5, quest_data.data() + sizeof(quest));
    append(bytes, record(GM_ROLE_QUEST, 0, quest_data));
    GM_ROLE_MAIL_SLOT mail;
    std::memset(&mail, 0, sizeof(mail));mail.dwMailID = 41;mail.nMailType = 1;mail.nState = 1;
    std::vector<char> mail_data(sizeof(mail) + 5, 0);
    std::memcpy(mail_data.data(), &mail, sizeof(mail));
    std::copy_n("mail", 4, mail_data.data() + offsetof(GM_ROLE_MAIL_SLOT, szData));
    append(bytes, record(GM_ROLE_MAIL, 0, mail_data));
    append(bytes, record(999, 777, std::array<char, 3>{'x','y','z'}));
    return bytes;
}
}
#endif
