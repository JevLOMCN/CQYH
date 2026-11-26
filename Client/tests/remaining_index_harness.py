"""Execute real team/chat/loot/table accessors and quest counter consumers."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    parts = [r'''
#pragma push_macro("M_AddDataError")
#undef M_AddDataError
namespace RemainingIndexTests {
using FString = AttributeRecordTests::FString;
using TCHAR = char;
template<class T> using TArray = SkillQuestTests::TArray<T>;
template<class T> using SingletonMeyers = SkillQuestTests::SingletonMeyers<T>;
struct MMOTeamMember { int id = 1; };
struct MMONearbyPlayer { int id = 2; };
struct MMOTeamInfo { int id = 3; };
struct TeamManager {
    TArray<MMOTeamMember> TeamMemberList;
    TArray<MMONearbyPlayer> NearbyPlayerList;
    TArray<MMOTeamInfo> TeamInfoList;
    MMOTeamMember* GetTeamMember(INT);
    MMONearbyPlayer* GetNearPlayer(INT);
    MMOTeamInfo* GetNearTeam(INT);
};
struct MMOChatManager {
    struct Channel { TArray<INT> ChatMessageList; };
    TArray<Channel> ChatFilterChannelList;
    void CleanChannelChat(INT);
};
struct SlotOpenTaxData { int value = 1; };
struct MMOHeavenManager {
    TArray<SlotOpenTaxData> arrSlotTax;
    const SlotOpenTaxData* GetSlotEquipTax(INT) const;
};
''']
    team = clean(source(game / "Src/TeamManager.cpp"))
    for signature in ["MMOTeamMember* TeamManager::GetTeamMember(", "MMONearbyPlayer* TeamManager::GetNearPlayer(",
                      "MMOTeamInfo* TeamManager::GetNearTeam("]:
        parts.append(function(team, signature))
    parts.append(function(clean(source(game / "Src/MMOChatManager.cpp")), "void MMOChatManager::CleanChannelChat("))
    parts.append(function(clean(source(game / "Src/MMOHeavenManager.cpp")), "const SlotOpenTaxData* MMOHeavenManager::GetSlotEquipTax("))
    doodad = clean(source(game / "DoodadManager.h"))
    for name in ["FDoodadDropSet", "FDoodadDropData"]:
        parts.append(function(doodad, "struct " + name + "\n") + ";")
    parts.append(r'''
bool GIsEditor = false;
int errors = 0;
#define M_AddDataError(...) (++errors)
template<class DataType, class DataProcessor, class KeyType> struct TDataManger {
    TArray<FString> _defaultValues;
    const TCHAR* FindDefaultValue(int) const;
    const TCHAR* GetTabfileName() const { return "table"; }
};
''')
    parts.append(function(clean(source(game / "MMODataManager.h")),
                          "template<typename DataType, typename DataProcessor, typename KeyType >\nconst TCHAR* TDataManger<DataType, DataProcessor, KeyType>::FindDefaultValue(int idx) const"))
    parts.append(r'''
void IndexTests() {
    TeamManager team; MMOChatManager chat; MMOHeavenManager heaven;
    for (int count : {0, 1, 3}) {
        team.TeamMemberList.data.resize(count); team.NearbyPlayerList.data.resize(count); team.TeamInfoList.data.resize(count);
        heaven.arrSlotTax.data.resize(count); chat.ChatFilterChannelList.data.resize(count);
        for (auto& channel : chat.ChatFilterChannelList.data) channel.ChatMessageList.data.assign(3, 123);
        for (int invalid : {-2147483647 - 1, -1, count, MAXINT}) {
            assert(!team.GetTeamMember(invalid) && !team.GetNearPlayer(invalid) && !team.GetNearTeam(invalid));
            assert(!heaven.GetSlotEquipTax(invalid)); chat.CleanChannelChat(invalid);
        }
        for (auto& channel : chat.ChatFilterChannelList.data) assert(channel.ChatMessageList.Num() == 3);
        for (int i = 0; i < count; ++i) {
            assert(team.GetTeamMember(i)->id == 1 && team.GetNearPlayer(i)->id == 2 && team.GetNearTeam(i)->id == 3);
            assert(heaven.GetSlotEquipTax(i)->value == 1);
            chat.CleanChannelChat(i); assert(!chat.ChatFilterChannelList(i).ChatMessageList.Num());
        }
    }
    TipsStallTests::Poisoned<FDoodadDropData>([](auto& data) {
        assert(!data.GetDoodadTemplateID() && !data.GetDoodadLevel() && !data.GetSceneDifficulty());
        for (int i = 0; i < data.GetDropSetCount(); ++i) assert(!data.GetDropSet(i).DropChance && !data.GetDropSet(i).DropGroupID);
        FDoodadDropSet entry; entry.DropChance = 5; entry.DropGroupID = 7;
        for (int i = 0; i < data.GetDropSetCount(); ++i) data.SetDropSet(i, entry);
        for (int invalid : {-2147483647 - 1, -1, data.GetDropSetCount(), MAXINT}) {
            data.SetDropSet(invalid, FDoodadDropSet());
            assert(data.GetDropSet(invalid).DropChance == 5);
        }
        for (int i = 0; i < data.GetDropSetCount(); ++i) assert(data.GetDropSet(i).DropGroupID == 7);
    });
    TDataManger<int, int, int> defaults; defaults._defaultValues.AddItem(FString("value"));
    for (int invalid : {-2147483647 - 1, -1, 1, MAXINT}) assert(!*defaults.FindDefaultValue(invalid));
    assert(!strcmp(defaults.FindDefaultValue(0), "value"));
    GIsEditor = true; assert(!*defaults.FindDefaultValue(0)); GIsEditor = false;
    puts("Team/chat/heaven/table index bounds and loot record initialization/bounds passed");
}
''')
    capacity = re.search(r"#define MAX_CONDITION_PER_QUEST\s+([^\n]+)",
                         clean(source(client / "Common/define/quest_def.h"))).group(1)
    parts.extend(["enum { MAX_CONDITION_PER_QUEST = " + capacity + " };", r'''
enum { MsgInfoType_Table = 1 };
struct FQuestCheckKillNpc {
    int CheckKillNpcNewId = 5, CheckKillNpcGroupId = 0, CheckKillNpcCnt = 2;
    FString Description = "group";
};
struct UNPCTemplate { FString GetDisplayName() { return "npc"; } } npc;
struct UQuest {
    TArray<FQuestCheckKillNpc> CheckKillNpc;
    UNPCTemplate* GetCheckKillNpc(int index) { assert(CheckKillNpc.IsValidIndex(index)); return &npc; }
};
struct AcceptedQuestData { UINT8 KillNpcCount[MAX_CONDITION_PER_QUEST] = {}; };
using AcceptedQuestDataMap = std::map<INT, AcceptedQuestData>;
using MonsterList = std::list<INT>;
struct MMOQuestData : SingletonMeyers<MMOQuestData> {
    AcceptedQuestDataMap _AcceptedQuestDataMap;
    UQuest quest;
    bool available = true;
    int removed = 0;
    const UQuest* GetQuestData(INT) { return available ? &quest : nullptr; }
    const AcceptedQuestData* GetAcceptedQuestData(INT id) { auto it = _AcceptedQuestDataMap.find(id); return it == _AcceptedQuestDataMap.end() ? nullptr : &it->second; }
    void _RemoveMonsterAcceptedQuest(int, int) { ++removed; }
    bool GetGroupMonsterList(int, MonsterList& list) { list.push_back(1); return true; }
    void UpdateQuestNpcKillCount(INT, UINT16, INT);
};
struct MMOQuestManager : SingletonMeyers<MMOQuestManager> {
    int statuses = 0, targets = 0;
    void UpdateQuestStatus() { ++statuses; }
    void UpdateQuestTargets(int) { ++targets; }
    void UpdateQuestNpcKillCount(INT, UINT16, INT);
};
namespace MMOUI {
    struct UI {
        int reports = 0;
        void delegateAddScreenMessage(const FString&, int, const char*) { ++reports; }
    } ui;
    UI* GetUIManager() { return &ui; }
    FString ParseLocalizedPropertyPath(const char*) { return "%s: %d/%d"; }
}
'''])
    parts.append(function(clean(source(game / "MMOQuestData.cpp")), "void MMOQuestData::UpdateQuestNpcKillCount("))
    parts.append(function(clean(source(game / "MMOQuestManager.cpp")), "void MMOQuestManager::UpdateQuestNpcKillCount("))
    parts.append(r'''
void QuestTests() {
    auto& data = MMOQuestData::getSingleton(); auto& manager = MMOQuestManager::getSingleton();
    for (int templateCount : {0, 1, MAX_CONDITION_PER_QUEST - 1, int(MAX_CONDITION_PER_QUEST), MAX_CONDITION_PER_QUEST + 3}) {
        data.quest.CheckKillNpc.data.resize(templateCount);
        for (UINT16 index = 0; index <= MAX_CONDITION_PER_QUEST + 3; ++index) {
            data._AcceptedQuestDataMap[1] = AcceptedQuestData();
            int oldReports = MMOUI::ui.reports, oldRemoved = data.removed;
            manager.UpdateQuestNpcKillCount(1, index, 2);
            bool valid = index < MAX_CONDITION_PER_QUEST && index < templateCount;
            assert(MMOUI::ui.reports == oldReports + valid && data.removed == oldRemoved + valid);
            for (int i = 0; i < MAX_CONDITION_PER_QUEST; ++i)
                assert(data._AcceptedQuestDataMap[1].KillNpcCount[i] == (i == index ? 2 : 0));
        }
    }
    manager.UpdateQuestNpcKillCount(1, UINT16(-1), 2);
    puts("Quest NPC condition arrays independently bounded against fixed counter capacity and template count passed");
}
}
#pragma pop_macro("M_AddDataError")
''')
    return "\n".join(parts)
