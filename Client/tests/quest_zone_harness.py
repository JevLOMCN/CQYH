"""Actual quest-zone entry/exit, map insertion and trigger export row code."""
import re


def harness(client, game, source, function):
    def clean(path):
        return re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                      lambda m: "" if m.group().startswith(("//", "/*")) else m.group(), source(path))

    parts = [r'''
#include <sstream>
namespace QuestZoneSafetyTests {
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    T& operator()(INT index) { return values.at(index); }
    const T& operator()(INT index) const { return values.at(index); }
    void Push(const T& value) { values.push_back(value); }
};
enum { EGameState_InGame = 1, EVVT_QuestZone = 2 };
namespace MMO { INT state = EGameState_InGame; INT GetCurrentGameState() { return state; } }
struct AMMOPlayer { bool main = true; UBOOL IsMainPlayer() const { return main; } };
struct MMOGameEvent_Quest {
    enum Kind { EQET_EnterQuestZone, EQET_LeaveQuestZone };
    Kind type;
    INT id, QuestGridType = 0;
    MMOGameEvent_Quest(Kind kind, INT quest) : type(kind), id(quest) {}
};
namespace MMOUI {
std::vector<MMOGameEvent_Quest> events;
void PostGameEvent(const MMOGameEvent_Quest* event) { events.push_back(*event); }
std::string ParseLocalizedPropertyPath(const char*) { return "quest hint"; }
}
struct Text {
    std::wstring value;
    const wchar_t* operator*() const { return value.c_str(); }
};
struct Position { Text ToString() const { return {L"X=1,Y=2,Z=3"}; } };
struct AMMOVolume_Trigger {
    TArray<INT> Quests;
    INT QuestTriggerType = 3, VolumeType = EVVT_QuestZone;
    Position Location;
    Text GetName() const { return {L"zone"}; }
    void OnEnterQuestZone(AMMOPlayer*);
    void OnLeaveQuestZone(AMMOPlayer*);
};
''']
    volume = clean(game / "Src/AMMOVolume_Trigger.cpp")
    for signature in ["void AMMOVolume_Trigger::OnEnterQuestZone(", "void AMMOVolume_Trigger::OnLeaveQuestZone("]:
        parts.append(function(volume, signature))
    parts.append(r'''
void EventTests() {
    AMMOVolume_Trigger zone; AMMOPlayer player;
    for (auto values : {std::vector<INT>{}, std::vector<INT>{0}, std::vector<INT>{-1, 9}}) {
        zone.Quests.values = values;
        zone.OnEnterQuestZone(&player); zone.OnLeaveQuestZone(&player);
        assert(MMOUI::events.empty());
    }
    zone.Quests.values = {17};
    zone.OnEnterQuestZone(nullptr); zone.OnLeaveQuestZone(nullptr);
    player.main = false; zone.OnEnterQuestZone(&player); zone.OnLeaveQuestZone(&player); player.main = true;
    MMO::state = 0; zone.OnEnterQuestZone(&player); zone.OnLeaveQuestZone(&player); MMO::state = EGameState_InGame;
    assert(MMOUI::events.empty());
    zone.OnEnterQuestZone(&player); zone.OnLeaveQuestZone(&player);
    assert(MMOUI::events.size() == 2 && MMOUI::events[0].type == MMOGameEvent_Quest::EQET_EnterQuestZone);
    assert(MMOUI::events[1].type == MMOGameEvent_Quest::EQET_LeaveQuestZone);
    for (const auto& event : MMOUI::events) assert(event.id == 17 && event.QuestGridType == 3);
    MMOUI::events.clear();
}
using FString = std::string;
using FVector = PositionDefaultTests::FVector;
enum { E_qt_Emegency = 1, EACTUIF_None, EACTUIUF_UpdateAll, EACTMUI_QuestZone, EACTUIF_TaskNpc };
struct FQuestVolumeInfo { TArray<INT> Quests; INT MapID = 7; FVector Position{1, 2, 3}; };
struct FActorInMapUI {
    INT _ToggleFlag = 0, _NeedUpdateFlag = 0, _ID = 0, _ActorID = 0, _InMapID = 0, _Type = 0;
    UBOOL _IsValid = FALSE;
    FVector _OrigInMapPos{0, 0, 0};
    FString _DisplayName, _UiIconIdx;
};
struct UQuest { INT id, QuestType; INT GetTemplateID() const { return id; } };
struct MMOQuestData {
    std::map<INT, UQuest> records;
    bool available = true, accepted = false, complete = false;
    static MMOQuestData& getSingleton() { static MMOQuestData value; return value; }
    const UQuest* GetQuestData(INT id) { auto found = records.find(id); return found == records.end() ? nullptr : &found->second; }
    UBOOL IsQuestAvailable(INT) { return available; }
    UBOOL IsQuestAccepted(INT) { return accepted; }
    UBOOL IsEmegencyQuestAllCompleteToday() { return complete; }
};
struct MMOMapDataManager {
    TArray<FQuestVolumeInfo> zones;
    const TArray<FQuestVolumeInfo>* GetSpeciZoneInMap(INT scene) { return scene == 7 ? &zones : nullptr; }
    INT PutSpeciZoneToMapUI(INT, INT, TArray<FActorInMapUI>&);
};
''')
    parts.append(function(clean(game / "MMOMapDataManager.cpp"), "int MMOMapDataManager::PutSpeciZoneToMapUI("))
    parts.append(r'''
void MapTests() {
    MMOMapDataManager manager;
    TArray<FActorInMapUI> objects;
    assert(manager.PutSpeciZoneToMapUI(8, 0, objects) == 0 && !objects.Num());
    assert(manager.PutSpeciZoneToMapUI(7, 0, objects) == 0 && !objects.Num());
    FQuestVolumeInfo empty, missing, normal, emergency;
    missing.Quests.values = {99}; normal.Quests.values = {1}; emergency.Quests.values = {2};
    auto& quests = MMOQuestData::getSingleton();
    quests.records[1] = {1, 0}; quests.records[2] = {2, E_qt_Emegency};
    manager.zones.values = {empty, missing, normal, emergency};
    assert(manager.PutSpeciZoneToMapUI(7, 0, objects) == 1 && objects.Num() == 1);
    const auto& object = objects(0);
    assert(object._IsValid && object._ActorID == 3 && object._InMapID == 7 && object._Type == EACTMUI_QuestZone);
    assert(object._OrigInMapPos.X == 1 && object._OrigInMapPos.Y == 2 && object._OrigInMapPos.Z == 3);
    assert(object._DisplayName == "quest hint" && object._UiIconIdx == "questZone");
    objects.values.resize(2); objects(1)._ID = 99;
    quests.available = false; quests.accepted = true;
    assert(manager.PutSpeciZoneToMapUI(7, 0, objects) == 1 && objects(0)._IsValid && objects(1)._ID == 99);
    quests.complete = true;
    assert(manager.PutSpeciZoneToMapUI(7, 0, objects) == 1 && !objects(0)._IsValid);
    quests.available = true; quests.accepted = quests.complete = false;
}
std::vector<AMMOVolume_Trigger*> volumes;
template<class T> struct TObjectIterator {
    size_t index = 0;
    explicit operator bool() const { return index < volumes.size(); }
    void operator++() { ++index; }
    T* operator*() const { return volumes.at(index); }
};
''')
    maximum = re.search(r"#define MAX_QUEST_PER_ZONE\s+([^\n]+)", clean(client / "Common/define/grid_def.h")).group(1)
    parts.append("enum { MAX_QUEST_PER_ZONE = " + maximum + " };")
    exporter = function(clean(game / "Exporter_TriggerVolume.cpp"), "void Exporter_TriggerVolume::ProcessData(")
    parts.append('#pragma push_macro("TEXT")\n#undef TEXT\n#define TEXT(value) L##value')
    parts.append("std::wstring ExportRows() {\nstd::wstringstream ss;\n" +
                 re.search(r"const INT C_TMPBUF_SZ\s*=\s*\d+;", exporter).group() +
                 "\nwchar_t buffer[C_TMPBUF_SZ];\n" + function(exporter, "for (TObjectIterator<AMMOVolume_Trigger>") +
                 "\nreturn ss.str();\n}")
    parts.append('#pragma pop_macro("TEXT")')
    parts.append(r'''
void ExportTests() {
    for (INT count = 0; count <= MAX_QUEST_PER_ZONE + 1; ++count) {
        AMMOVolume_Trigger zone, other; other.VolumeType = -1;
        for (INT i = 0; i < count; ++i) zone.Quests.values.push_back(i + 10);
        volumes = {nullptr, &other, &zone};
        std::wstring rows = ExportRows();
        if (!count || count > MAX_QUEST_PER_ZONE) assert(rows.empty());
        else {
            for (INT i = 0; i < 4; ++i) {
                auto expected = L"QuestId" + std::to_wstring(i) + L"=\"" + std::to_wstring(i < count ? i + 10 : 0) + L"\"";
                assert(rows.find(expected) != rows.npos);
            }
        }
    }
    volumes.clear();
}
void Run() {
    EventTests(); MapTests(); ExportTests();
    puts("Quest-zone events/maps skip empty lists; trigger exports safely zero-fill missing quest entries and retain complete rows");
}
}
''')
    return "\n".join(parts)
