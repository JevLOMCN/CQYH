"""Production friend-list indices/string resets and table enum narrowing."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "Src/FriendManager.h"))
    cpp = clean(source(game / "Src/FriendManager.cpp"))
    generated = (game / "Inc/MMOGameUIClasses.h").read_bytes()
    generated = generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1")
    capacity = re.search(r"#define UCONST_MaxSearchPanelItem\s+([^\r\n]+)", generated).group(1)
    parts = [r'''
namespace FriendEnumTests {
enum EEventParm { EC_EventParm };
using TCHAR = char;
struct FString {
    unsigned magic = 0x1234abcd;
    std::string value;
    FString(const char* text = "") : value(text) {}
    FString(const FString&) = default;
    FString& operator=(const FString& other) {
        assert(magic == 0x1234abcd && other.magic == 0x1234abcd); value = other.value; return *this;
    }
    const char* operator*() const { assert(magic == 0x1234abcd); return value.c_str(); }
    bool operator<(const FString& other) const { return value < other.value; }
    template<class... Args> static FString Printf(const char*, Args...) { return FString("error"); }
};
template<class T> struct TArray {
    std::vector<T> data;
    int Num() const { return (int)data.size(); }
    T& operator()(int index) { return data.at(index); }
    const T& operator()(int index) const { return data.at(index); }
    void AddItem(const T& value) { data.push_back(value); }
    void Remove(int start, int count = 1) { data.erase(data.begin() + start, data.begin() + start + count); }
};
''', "enum { UCONST_MaxSearchPanelItem = " + capacity + " };",
        function(header, "struct MMOFriendMember\n") + ";",
        function(header, "struct MMOFriendGroup\n") + ";", r'''
enum { FS_OFFLINE = 0, GameEvent_UpdateFriendGroup = 1 };
struct MMOGameEvent_OneInt { MMOGameEvent_OneInt(int, int) {} };
namespace MMOUI { int events = 0; void PostGameEvent(MMOGameEvent_OneInt*) { ++events; } }
struct MMOFacade {
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    static FString CheckName(const FString& text) { return text; }
    void RequestPlayerBaseInfo(int) {}
};
struct FriendManager {
    TArray<MMOFriendGroup> FriendGroupList;
    TArray<MMOFriendMember> BlackList, EnemyList, NearContactList;
    MMOFriendMember SearchList[UCONST_MaxSearchPanelItem];
    static FriendManager& getSingleton() { static FriendManager value; return value; }
    MMOFriendMember* GetSearchList() { return SearchList; }
    void ClearSearchList();
    void AddNewFriend(INT, FString, INT);
    void PushToFriendGroup(INT, MMOFriendMember);
    INT GetFriendNumInGroup(INT);
    INT GetOnlineFriendNumInGroup(INT);
    MMOFriendMember* GetFriendGroupMember(INT, INT);
    MMOFriendMember* GetFriendGroupMemberById(INT, INT);
    MMOFriendMember* GetBlacklistMember(INT);
    MMOFriendMember* GetEnemylistMember(INT);
    MMOFriendGroup* GetFriendGroup(INT);
    void MovePlayerToGroup(INT, INT, INT);
    void MoveGroupPlayerToGroup(INT, INT);
    void SortFriendGroup(INT);
};
using QSORT_COMPARE = int (*)(const void*, const void*);
void appQsort(void* items, int count, size_t size, QSORT_COMPARE compare) {
    assert(size == sizeof(MMOFriendMember)); auto* first = (MMOFriendMember*)items;
    std::sort(first, first + count, [&](const auto& a, const auto& b) { return compare(&a, &b) < 0; });
}
''']
    parts.append(function(cpp, "static QSORT_RETURN CDECL SortFriend(").replace("QSORT_RETURN CDECL", "int"))
    signatures = ["void FriendManager::ClearSearchList()", "void FriendManager::AddNewFriend(",
                  "void FriendManager::PushToFriendGroup(", "INT FriendManager::GetFriendNumInGroup(",
                  "INT FriendManager::GetOnlineFriendNumInGroup(", "MMOFriendMember* FriendManager::GetFriendGroupMember(",
                  "MMOFriendMember* FriendManager::GetFriendGroupMemberById(", "MMOFriendMember* FriendManager::GetBlacklistMember(",
                  "MMOFriendMember* FriendManager::GetEnemylistMember(", "MMOFriendGroup* FriendManager::GetFriendGroup(",
                  "void FriendManager::MovePlayerToGroup(", "void FriendManager::MoveGroupPlayerToGroup(",
                  "void FriendManager::SortFriendGroup("]
    parts.extend(function(cpp, signature) for signature in signatures)
    parts.append(r'''
struct UMMOUIContactWidget {
    int fills = 0;
    void UpdateSearchList(TArray<MMOFriendMember>&);
    void FillInSearchPanel() { ++fills; }
};
''')
    parts.append(function(clean(source(game / "Src/MMOUIContactWidget.cpp")), "void UMMOUIContactWidget::UpdateSearchList("))
    parts.append(r'''
void FriendTests() {
    auto& manager = FriendManager::getSingleton(); manager.FriendGroupList.data.resize(2);
    MMOFriendMember member; member.RoleId = 123; member.ActorName = FString("Name");
    manager.FriendGroupList(0).FriendList.AddItem(member);
    manager.BlackList.AddItem(member); manager.EnemyList.AddItem(member);
    for (int invalid : {-2147483647 - 1, -1, 2, 2147483647}) {
        assert(!manager.GetFriendGroup(invalid) && !manager.GetFriendGroupMember(invalid, 0));
        assert(!manager.GetFriendGroupMemberById(invalid, 123));
        assert(!manager.GetFriendGroupMember(0, invalid));
        assert(!manager.GetFriendNumInGroup(invalid) && !manager.GetOnlineFriendNumInGroup(invalid));
        int events = MMOUI::events;
        manager.AddNewFriend(999, FString("Invalid"), invalid); manager.PushToFriendGroup(invalid, member);
        manager.MovePlayerToGroup(123, invalid, 0); manager.MovePlayerToGroup(123, 0, invalid);
        manager.MoveGroupPlayerToGroup(invalid, 0); manager.MoveGroupPlayerToGroup(0, invalid);
        manager.SortFriendGroup(invalid); assert(MMOUI::events == events);
    }
    for (int invalid : {-2147483647 - 1, -1, 1, 2147483647}) {
        assert(!manager.GetBlacklistMember(invalid) && !manager.GetEnemylistMember(invalid));
    }
    assert(manager.GetBlacklistMember(0)->RoleId == 123 && manager.GetEnemylistMember(0)->RoleId == 123);
    assert(manager.GetFriendGroupMemberById(0, 123)->RoleId == 123);
    manager.MovePlayerToGroup(123, 0, 1);
    assert(!manager.GetFriendNumInGroup(0) && manager.GetFriendNumInGroup(1) == 1);
    manager.MoveGroupPlayerToGroup(1, 0);
    assert(manager.GetFriendNumInGroup(0) == 1 && !manager.GetFriendNumInGroup(1));
    manager.MovePlayerToGroup(123, 0, 0); assert(manager.GetFriendNumInGroup(0) == 1);
    assert(SortFriend(&member, &member) == 0);
    MMOFriendMember equal = member; assert(SortFriend(&member, &equal) == 0 && SortFriend(&equal, &member) == 0);
    equal.Online = 1; assert(SortFriend(&member, &equal) < 0 && SortFriend(&equal, &member) > 0);
    manager.PushToFriendGroup(0, equal); manager.SortFriendGroup(0);
    assert(manager.FriendGroupList(0).FriendList(0).Online == 0 && manager.FriendGroupList(0).FriendList(1).Online == 1);
    UMMOUIContactWidget widget;
    for (int count : {20, 0, 1, 6, 2, 0}) {
        for (auto& entry : manager.SearchList) {
            for (auto* text : {&entry.ActorName, &entry.Location, &entry.Sign, &entry.AbbName})
                text->value.assign(3000, 'x');
        }
        TArray<MMOFriendMember> results;
        for (int i = 0; i < count; ++i) { member.RoleId = 1000 + i + count; results.AddItem(member); }
        widget.UpdateSearchList(results);
        int limited = std::min(count, int(UCONST_MaxSearchPanelItem));
        for (int i = 0; i < UCONST_MaxSearchPanelItem; ++i) {
            assert(manager.SearchList[i].ActorName.magic == 0x1234abcd);
            assert(manager.SearchList[i].RoleId == (i < limited ? 1000 + i + count : 0));
        }
    }
    manager.ClearSearchList();
    for (auto& entry : manager.SearchList) {
        assert(!entry.RoleId && !entry.SceneTempID);
        for (auto* text : {&entry.ActorName, &entry.Location, &entry.Sign, &entry.AbbName})
            assert(text->magic == 0x1234abcd && text->value.empty());
    }
    puts("Friend-list group/row bounds, moves, equal sorting and constructed-string search resets passed");
}
}
#pragma push_macro("M_StatUsedTabHeader")
#pragma push_macro("M_AddDataError")
#undef M_StatUsedTabHeader
#undef M_AddDataError
namespace TableEnumTests {
using FString = FriendEnumTests::FString;
using TCHAR = char;
int errors = 0;
#define M_StatUsedTabHeader(...) ((void)0)
#define M_AddDataError(...) (++errors)
''')
    data = clean(source(game / "MMODataManager.h"))
    parts.append(function(data, "struct FindDataIndexResult\n") + ";")
    parts.append(r'''
struct FName { explicit FName(const char*) {} };
struct UEnum { int result = 0; int FindEnumIndex(FName) { return result; } } definition;
bool available = true;
struct UServerEnum { static int StaticClass() { return 1; } };
template<class T> T* FindObject(int, const char*, BOOL) { return available ? &definition : nullptr; }
namespace MMOUI { FString ConvertServerEnumToClientEnum(const FString& text) { return text; } }
struct CTabFile {
    bool use_default = false;
    const char* GetData(int, int) { return use_default ? nullptr : "enumValue"; }
};
struct Manager {
    bool header_found = true;
    static Manager& getSingleton() { static Manager value; return value; }
    static const char* GetTabfileName() { return "table.txt"; }
    FindDataIndexResult FindHeaderIndex(const FString&) { return FindDataIndexResult(header_found, 0); }
    const char* FindDefaultValue(int) { return "defaultEnum"; }
};
''')
    parts.append(function(data, "template<typename ManagerType>\nBYTE TReadDataValue_ServerEnum("))
    parts.append(r'''
void Run() {
    CTabFile row;
    for (bool use_default : {false, true}) {
        row.use_default = use_default;
        for (int invalid : {-2147483647 - 1, -2, -1, 256, 257, 2147483647}) {
            definition.result = invalid; int before = errors;
            assert(TReadDataValue_ServerEnum<Manager>(FString("header"), &row, 0, "enum") == 0);
            assert(errors == before + 1);
        }
        for (int valid : {0, 1, 127, 254, 255}) {
            definition.result = valid; int before = errors;
            assert(TReadDataValue_ServerEnum<Manager>(FString("header"), &row, 0, "enum") == valid && errors == before);
        }
    }
    available = false; int before = errors;
    assert(!TReadDataValue_ServerEnum<Manager>(FString("header"), &row, 0, "enum") && errors == before + 1);
    available = true; Manager::getSingleton().header_found = false; before = errors;
    assert(!TReadDataValue_ServerEnum<Manager>(FString("header"), &row, 0, "enum") && errors == before + 1);
    puts("Table enum validation before narrowing, missing enums and default-value paths passed");
}
}
#pragma pop_macro("M_AddDataError")
#pragma pop_macro("M_StatUsedTabHeader")
''')
    return "\n".join(parts)
