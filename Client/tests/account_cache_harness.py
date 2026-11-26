"""Exercise real account-cache readers and writers with corrupted archives."""
import re


def harness(client, game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(path))

    assist_header = clean(game / "MMOAssistData.h")
    parts = [r'''
namespace AccountTests {
#undef TEXT
#define TEXT(s) L##s
#define HIWORD(value) ((UINT16)((value) >> 16))
using TCHAR = wchar_t;
enum { DEFAULT_PROTECT_TIME = 2, PROTECT_DATA_COUNT = 3 };
template<class T> struct Array : std::vector<T> {
    INT Num() const { return (INT)this->size(); }
    T& operator()(INT index) { return (*this)[index]; }
    void Remove(INT index, INT count) { this->erase(this->begin() + index, this->begin() + index + count); }
    void AddItem(const T& value) { this->push_back(value); }
    void Empty() { this->clear(); }
};
template<class T> using TArray = Array<T>;
struct FString {
    std::wstring value;
    FString() {}
    FString(const wchar_t* text) : value(text) {}
    const wchar_t* operator*() const { return value.c_str(); }
    FString operator+(const wchar_t* text) const { return FString((value + text).c_str()); }
    FString& operator+=(const FString& text) { value += text.value; return *this; }
    void ParseIntoArray(Array<FString>* output, const wchar_t*, BOOL omitEmpty) const {
        output->clear();
        size_t begin = 0;
        while (begin <= value.size()) {
            size_t end = value.find(L'|', begin);
            if (end == std::wstring::npos) end = value.size();
            if (end > begin || !omitEmpty) output->push_back(FString(value.substr(begin, end - begin).c_str()));
            if (end == value.size()) break;
            begin = end + 1;
        }
    }
};
void appStrncpy(wchar_t* output, const wchar_t* input, size_t capacity) {
    if (!capacity) return;
    size_t index = 0;
    while (index + 1 < capacity && input[index]) { output[index] = input[index]; ++index; }
    output[index] = 0;
}
struct FArchive {
    std::vector<char> bytes;
    INT position = 0, reads = 0, reported = -1, failAt = -1;
    bool error = false, saving = false;
    INT TotalSize() const { return reported == -1 ? (INT)bytes.size() : reported; }
    INT Tell() const { return position; }
    bool IsError() const { return error; }
    void Serialize(void* output, INT count) {
        ++reads;
        if (saving) {
            const char* data = static_cast<const char*>(output);
            bytes.insert(bytes.end(), data, data + count);
            position += count;
            return;
        }
        if (count < 0 || count > (INT)bytes.size() - position || (failAt >= 0 && position >= failAt)) {
            error = true;
            return;
        }
        if (count) memcpy(output, bytes.data() + position, count);
        position += count;
    }
    template<class T> void Append(const T& value) {
        const char* data = reinterpret_cast<const char*>(&value);
        bytes.insert(bytes.end(), data, data + sizeof(value));
    }
};
FArchive* active = nullptr;
FArchive* output = nullptr;
INT closes = 0;
std::vector<char> saved;
struct MMOFacade {
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    BOOL BeginLoadAccountBinary(FArchive** archive, FString) { *archive = active; return active != nullptr; }
    void EndLoadAccountBinary(FArchive*) { ++closes; }
    BOOL BeginExportAccountData(FArchive** archive, FString name) {
        assert(name.value == L"TeamInviteHistory");
        *archive = output;
        return output != nullptr;
    }
    void EndExportAccountData(FArchive*) { ++closes; }
    void ExportSimpleAccountData(void* buffer, size_t bytes, FString) {
        saved.assign((const char*)buffer, (const char*)buffer + bytes);
    }
};
struct MailManager {
    Array<FString> _RecentPlayerArray;
    void LoadData();
    void SaveData();
};
''']
    for file, name in [(game / "MailManager.h", "MAX_MAILRECENTPLAYER_NUM"),
                       (client / "Common/define/string_def.h", "ROLE_NAME_LEN")]:
        parts.append(re.search(r"#define " + name + r"\s+[^\n]+", clean(file)).group())
    mail = clean(game / "MailManager.cpp")
    parts.extend(function(mail, "void MailManager::" + name + "()") for name in ["LoadData", "SaveData"])
    team_header = clean(game / "Src/TeamManager.h")
    parts.append(re.search(r"#define MAX_ARCHIVE_INVITE_PLAYER\s+[^\n]+", team_header).group())
    parts.append(function(team_header, "struct MMOInvitedPlayer") + ";")
    parts.append(function(clean(game / "Src/FriendManager.h"), "struct MMOKillInfo") + ";")
    parts.append(function(clean(game / "Src/FriendChatManager.h"), "struct ContactLogStruct") + ";")
    parts.append(r'''
#define FriendChatLogFile TEXT("FriendChatLogFile")
#define KillInfoFileName TEXT("KillInfo")
struct TeamManager {
    TArray<MMOInvitedPlayer> InvitedPlayerCache;
    void LoadInvitePlayerBinary();
    void SaveInvitePlayerBinary();
};
struct FriendManager {
    TArray<MMOKillInfo> KillInfoList;
    static FriendManager& getSingleton() { static FriendManager value; return value; }
    UBOOL IsMyFriend(INT id) { return id == 42; }
    void LoadKillInfoData();
};
struct FriendChatManager {
    TArray<ContactLogStruct> ContactLogList;
    enum { MaxFriendChatLogNum = 100 };
    void LoadChatLogFromFile();
};
''')
    parts.extend(function(clean(game / "Src/TeamManager.cpp"), "void TeamManager::" + name + "()")
                 for name in ["LoadInvitePlayerBinary", "SaveInvitePlayerBinary"])
    parts.append(function(clean(game / "Src/FriendManager.cpp"), "void FriendManager::LoadKillInfoData()"))
    parts.append(function(clean(game / "Src/FriendChatManager.cpp"), "void FriendChatManager::LoadChatLogFromFile()"))
    parts.append(function(assist_header, "enum EProtectType") + ";")
    parts.extend(function(assist_header, "struct " + name) + ";" for name in [
        "AssistDropData", "AssistDropInfo", "AssistItemData", "AssistItemInfo"])
    parts.append(r'''
struct MMOAssistData {
    std::vector<AssistDropInfo> drops;
    std::vector<AssistItemInfo> items;
    static MMOAssistData& getSingleton() { static MMOAssistData value; return value; }
    void UpdateAssistDropInfo(UINT32 id, UBOOL valuable, UBOOL pick, UBOOL visible) {
        AssistDropInfo info; info.Id = id; info.Valuable = valuable; info.AutoPick = pick; info.Visible = visible;
        drops.push_back(info);
    }
    void UpdateAssistItemInfo(INT, const AssistItemInfo& info) { items.push_back(info); }
    void UpdateAssistItemInfoDefault() { items.clear(); }
};
namespace FSM {
struct Flags { BOOL ctrlFlag_DDCS, ctrlFlag_ZNBY, ctrlFlag_ZDLH, ctrlFlag_ZDZR, ctrlFlag_ZDKD, ctrlFlag_ZDYS; } Ctrl = {};
}
struct Mir2Attack {
    INT calls = 0; UINT16 skill = 0; UINT8 time = 0; UBOOL enabled = FALSE;
    static Mir2Attack& getSingleton() { static Mir2Attack value; return value; }
    void AutoPractice(UINT16 id, UINT8 duration, UBOOL state) { ++calls; skill = id; time = duration; enabled = state; }
};
struct MMOAssistManager {
    void _LoadAssistDrop();
    void _LoadAssistProtect(const UINT32 (&protectData)[PROTECT_DATA_COUNT]);
    void _LoadAssistSkill(UINT32 skillData);
};
''')
    assist = clean(game / "MMOAssistManager.cpp")
    parts.extend(function(assist, "void MMOAssistManager::" + name + "(")
                 for name in ["_LoadAssistDrop", "_LoadAssistProtect", "_LoadAssistSkill"])
    parts.append(r'''
void Run() {
    check_constructed<ContactLogStruct>([](ContactLogStruct& info) {
        assert(!info.RoleId && info.PlayerName.value.empty() && info.LogList.empty());
    });
    check_constructed<ContactLogStruct>([](ContactLogStruct& info) {
        assert(!info.RoleId && info.PlayerName.value.empty() && info.LogList.empty());
    }, EC_EventParm);
    check_constructed<AssistDropData>([](AssistDropData& info) {
        assert(!info.Id && !info.ItemClass && !info.Valuable && !info.AutoPick && !info.Visible && !info.Group);
    });
    check_constructed<AssistDropInfo>([](AssistDropInfo& info) {
        assert(!info.Id && !info.Valuable && !info.AutoPick && !info.Visible);
    });
    check_constructed<AssistItemData>([](AssistItemData& info) { assert(!info.ItemId && !info.ItemType && !info.Sort); });
    check_constructed<AssistItemInfo>([](AssistItemInfo& info) {
        assert(info.ProtectType == EPT_TOTAL && !info.ItemId && !info.Enable && !info.Value && info.Time == DEFAULT_PROTECT_TIME);
        const auto* bytes = reinterpret_cast<const unsigned char*>(&info);
        for (size_t index = sizeof(info.ProtectType); index < offsetof(AssistItemInfo, ItemId); ++index) assert(!bytes[index]);
        for (size_t index = offsetof(AssistItemInfo, Time) + sizeof(info.Time); index < sizeof(info); ++index) assert(!bytes[index]);
    });
    MailManager mail;
    for (const std::wstring text : {std::wstring(), std::wstring(L"A"), std::wstring(L"Alice|Bob|"), std::wstring(1000, L'A')}) {
        mail._RecentPlayerArray.clear();
        mail._RecentPlayerArray.push_back(FString(text.c_str()));
        saved.clear();
        mail.SaveData();
        assert(saved.size() == sizeof(TCHAR) * (ROLE_NAME_LEN * MAX_MAILRECENTPLAYER_NUM + 64));
        const size_t characters = saved.size() / sizeof(TCHAR);
        std::vector<TCHAR> bytes(characters);
        memcpy(bytes.data(), saved.data(), saved.size());
        assert(bytes.back() == 0);
        const std::wstring expected = (text + L"|").substr(0, characters - 1);
        assert(std::wstring(bytes.data()) == expected);
        for (size_t i = expected.size() + 1; i < bytes.size(); ++i) assert(!bytes[i]);
    }
    const size_t capacity = ROLE_NAME_LEN * MAX_MAILRECENTPLAYER_NUM + 64;
    for (INT byteCount : {1, 3, (INT)(capacity * sizeof(TCHAR) + 2), 100000}) {
        FArchive archive; archive.reported = byteCount;
        active = &archive;
        mail._RecentPlayerArray.clear();
        mail._RecentPlayerArray.push_back(FString(L"existing"));
        mail.LoadData();
        assert(!archive.reads && mail._RecentPlayerArray.size() == 1 && mail._RecentPlayerArray[0].value == L"existing");
    }
    for (bool fail : {false, true}) {
        FArchive archive;
        const std::wstring text = L"Alice|Bob|";
        archive.bytes.assign(reinterpret_cast<const char*>(text.data()), reinterpret_cast<const char*>(text.data() + text.size()));
        if (fail) archive.failAt = 0;
        active = &archive;
        mail._RecentPlayerArray.clear();
        mail.LoadData();
        if (fail) assert(mail._RecentPlayerArray.empty());
        else assert(mail._RecentPlayerArray.size() == 2 && mail._RecentPlayerArray[1].value == L"Bob");
    }
    FArchive full;
    const std::wstring noTerminator(capacity, L'X');
    full.bytes.assign(reinterpret_cast<const char*>(noTerminator.data()), reinterpret_cast<const char*>(noTerminator.data() + capacity));
    active = &full;
    mail.LoadData();
    assert(mail._RecentPlayerArray.size() == 1 && mail._RecentPlayerArray[0].value == noTerminator);
    FArchive many;
    const std::wstring delimiters(100, L'|');
    std::wstring names;
    for (INT i = 0; i < 50; ++i) names += L"A|";
    many.bytes.assign(reinterpret_cast<const char*>(names.data()), reinterpret_cast<const char*>(names.data() + names.size()));
    active = &many;
    mail.LoadData();
    assert(mail._RecentPlayerArray.size() == MAX_MAILRECENTPLAYER_NUM);

    MMOAssistManager manager;
    const UINT32 protect[PROTECT_DATA_COUNT] = {};
    for (bool drops : {true, false}) {
        auto load = [&] { if (drops) manager._LoadAssistDrop(); else manager._LoadAssistProtect(protect); };
        auto& received = MMOAssistData::getSingleton();
        const size_t recordSize = drops ? sizeof(AssistDropInfo) : sizeof(AssistItemInfo);
        for (INT count : {-1, 1, 0x7fffffff}) {
            FArchive archive; archive.Append(count); active = &archive;
            received.drops.clear(); received.items.clear();
            load();
            assert(received.drops.empty() && received.items.empty());
        }
        for (UINT32 length = 0; length < sizeof(INT) + recordSize; ++length) {
            FArchive archive;
            archive.Append<INT>(1);
            archive.bytes.resize(sizeof(INT) + recordSize, 0);
            archive.bytes.resize(length);
            active = &archive;
            load();
            assert(received.drops.empty() && received.items.empty());
        }
        FArchive valid;
        valid.Append<INT>(1);
        AssistDropInfo drop; drop.Id = 42; drop.Visible = TRUE;
        AssistItemInfo item; item.ProtectType = EPT_SLOW_HP; item.ItemId = 123;
        if (drops) valid.Append(drop); else valid.Append(item);
        active = &valid; load();
        if (drops) assert(received.drops.size() == 1 && received.drops[0].Id == 42 && received.drops[0].Visible);
        else assert(received.items.size() == 1 && received.items[0].ItemId == 123);
        received.drops.clear(); received.items.clear();
        valid.position = 0; valid.failAt = sizeof(INT);
        load();
        assert(received.drops.empty() && received.items.empty());
    }
    auto& practice = Mir2Attack::getSingleton();
    for (INT length = 0; length < sizeof(UINT32); ++length) {
        FArchive archive; archive.bytes.resize(length, 0); active = &archive;
        manager._LoadAssistSkill(0);
        assert(!practice.calls);
    }
    FArchive skill;
    skill.Append<UINT32>((123u << 16) | (7u << 7) | (1u << 6));
    active = &skill;
    manager._LoadAssistSkill(0);
    assert(practice.calls == 1 && practice.skill == 123 && practice.time == 7 && practice.enabled);
    TeamManager team;
    for (INT count : {0, 1, 10, 12}) {
        team.InvitedPlayerCache.clear();
        for (INT i = 0; i < count; ++i) {
            MMOInvitedPlayer player;
            player.ActorServerId = i + 1; player.Job = 2; player.Level = 3;
            player.ActorName = std::wstring(100, L'N').c_str();
            team.InvitedPlayerCache.AddItem(player);
        }
        FArchive writing; writing.saving = true; output = &writing;
        team.SaveInvitePlayerBinary();
        const INT kept = count > MAX_ARCHIVE_INVITE_PLAYER ? MAX_ARCHIVE_INVITE_PLAYER : count;
        assert(writing.bytes.size() == 2 * sizeof(INT) + kept * (3 * sizeof(INT) + ROLE_NAME_LEN * sizeof(TCHAR)));
        FArchive reading; reading.bytes = writing.bytes; active = &reading;
        team.InvitedPlayerCache.Empty();
        team.LoadInvitePlayerBinary();
        assert(team.InvitedPlayerCache.Num() == kept);
        for (INT i = 0; i < kept; ++i) {
            const auto& player = team.InvitedPlayerCache(i);
            assert(player.ActorServerId == count - kept + i + 1 && player.Job == 2 && player.Level == 3);
            assert(player.ActorName.value == std::wstring(ROLE_NAME_LEN - 1, L'N'));
        }
        if (count == 1) {
            for (UINT32 length = 0; length < writing.bytes.size(); ++length) {
                FArchive shortFile; shortFile.bytes.assign(writing.bytes.begin(), writing.bytes.begin() + length);
                active = &shortFile; team.InvitedPlayerCache.Empty();
                team.LoadInvitePlayerBinary();
                assert(team.InvitedPlayerCache.empty());
            }
            FArchive badMagic; badMagic.bytes = writing.bytes; badMagic.bytes[0] ^= 1;
            active = &badMagic; team.LoadInvitePlayerBinary();
            assert(team.InvitedPlayerCache.empty());
            FArchive badCount; badCount.bytes = writing.bytes;
            const INT excessiveCount = 0x7fffffff;
            memcpy(badCount.bytes.data() + sizeof(UINT32), &excessiveCount, sizeof(excessiveCount));
            active = &badCount; team.LoadInvitePlayerBinary();
            assert(team.InvitedPlayerCache.empty());
            FArchive failedRead; failedRead.bytes = writing.bytes; failedRead.failAt = 2 * sizeof(INT);
            active = &failedRead; team.LoadInvitePlayerBinary();
            assert(team.InvitedPlayerCache.empty());
        }
    }
    FArchive legacy; legacy.bytes.resize(sizeof(MMOInvitedPlayer), 0xA5);
    active = &legacy; team.InvitedPlayerCache.Empty();
    team.LoadInvitePlayerBinary();
    assert(team.InvitedPlayerCache.empty());

    FriendChatManager chat;
    auto name = [](FArchive& file, const std::wstring& text) {
        file.Append<INT>((INT)(text.size() * sizeof(TCHAR)));
        file.bytes.insert(file.bytes.end(), reinterpret_cast<const char*>(text.data()),
                          reinterpret_cast<const char*>(text.data() + text.size()));
    };
    for (INT nameBytes : {-1, 1, 2050, 0x7fffffff}) {
        FArchive file; file.Append<INT>(1); file.Append<INT>(42); file.Append(nameBytes); file.Append<INT>(0);
        active = &file; chat.LoadChatLogFromFile();
        assert(chat.ContactLogList.empty());
    }
    for (INT textBytes : {-1, 1, 4098, 0x7fffffff}) {
        FArchive file; file.Append<INT>(1); file.Append<INT>(42); name(file, L"A");
        file.Append<INT>(1); file.Append(textBytes);
        active = &file; chat.LoadChatLogFromFile();
        assert(chat.ContactLogList.empty());
    }
    for (INT count : {-1, 101, 0x7fffffff}) {
        FArchive file; file.Append<INT>(1); file.Append<INT>(42); name(file, L"A"); file.Append(count);
        active = &file; chat.LoadChatLogFromFile();
        assert(chat.ContactLogList.empty());
    }
    FArchive conversation;
    conversation.Append<INT>(1); conversation.Append<INT>(42); name(conversation, std::wstring(1024, L'N'));
    conversation.Append<INT>(1); name(conversation, std::wstring(2048, L'T'));
    active = &conversation; chat.LoadChatLogFromFile();
    assert(chat.ContactLogList.Num() == 1 && chat.ContactLogList(0).PlayerName.value.size() == 1024);
    assert(chat.ContactLogList(0).LogList(0).value.size() == 2048);
    for (UINT32 length = 0; length < conversation.bytes.size(); ++length) {
        FArchive truncated; truncated.bytes.assign(conversation.bytes.begin(), conversation.bytes.begin() + length);
        active = &truncated; chat.LoadChatLogFromFile();
        assert(chat.ContactLogList.empty());
    }
    auto& friendManager = FriendManager::getSingleton();
    for (UINT32 bytes : {1u, 1025u, 0xffffffffu}) {
        FArchive file; file.Append<UINT32>(1); file.Append(bytes);
        active = &file; friendManager.LoadKillInfoData();
        assert(friendManager.KillInfoList.empty());
    }
    FArchive kills;
    kills.Append<UINT32>(1); name(kills, L"Valid record");
    active = &kills; friendManager.LoadKillInfoData();
    assert(friendManager.KillInfoList.Num() == 1 && friendManager.KillInfoList(0).ShowText.value == L"Valid record");
    for (UINT32 length = 0; length < kills.bytes.size(); ++length) {
        FArchive truncated; truncated.bytes.assign(kills.bytes.begin(), kills.bytes.begin() + length);
        active = &truncated; friendManager.LoadKillInfoData();
        assert(friendManager.KillInfoList.empty());
    }
    active = nullptr;
    puts("Account cache size and short-read regressions passed");
}
#undef ROLE_NAME_LEN
#undef MAX_MAILRECENTPLAYER_NUM
#undef HIWORD
#undef MAX_ARCHIVE_INVITE_PLAYER
#undef FriendChatLogFile
#undef KillInfoFileName
#undef TEXT
#define TEXT(s) s
}
''')
    return "\n".join(parts)
