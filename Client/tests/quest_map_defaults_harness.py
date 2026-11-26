"""Production quest reward capacities, map defaults and teleport region bounds."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    quest_header = clean(source(game / "MMOQuestData.h"))
    quest_cpp = clean(source(game / "MMOQuestData.cpp"))
    map_header = clean(source(game / "MMOMapDataManager.h"))
    map_cpp = clean(source(game / "MMOMapDataManager.cpp"))
    parts = [r'''
namespace QuestMapDefaultTests {
using FString = AttributeRecordTests::FString;
using TCHAR = wchar_t;
using FVector = ::FVector;
template<class T> using TArray = SkillQuestTests::TArray<T>;
struct UItem;
struct FSceneProgress;
struct FDungeonTarget;
struct UGFxObject;
struct FTeleportNodeObj { int ServerID = 123, InSceneID = 456; FString Name = "node"; };
''']
    for name in ["MAX_QUEST_PER_CIRCLE", "MAX_SELECTABLE_REWARD_COUNT"]:
        value = re.search(r"#define " + name + r"\s+([^\n]+)",
                          clean(source(client / "Common/define/quest_def.h"))).group(1)
        parts.append("enum { " + name + " = " + value + " };")
    parts.append("enum { INVALID_QUEST_VAR_ID = -1, MAX_CIRCLE_AWARD_ITEM_COUNT = 4 };")
    definitions = []
    for name in ["QuestAwardItemInfo", "CircleQuestData", "CircleQuestRewardData",
                 "QuestRecycleData", "EmergencyQuestCoolDownInfo"]:
        definition = function(quest_header, "struct " + name + "\n") + ";"
        parts.append(definition)
        definitions.append((name, definition))
    data = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(data.decode("utf-16" if data.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    parts.append(function(generated, "enum EWorldRegion\n") + ";")
    minor = re.search(r"#define UCONST_MaxSceneMinorTarget\s+(\d+)", generated).group(1)
    parts.append("enum { UCONST_MaxSceneMinorTarget = " + minor + " };")
    parts.append(function(map_header, "enum EActorInMapUIType\n") + ";")
    parts.append(function(map_header, "enum EActorInMapUpdateFlag\n") + ";")
    for name in ["FTeleportNetNodeData", "FObjInMapUI", "FMapMarkExport", "FPathPointInMapUI",
                 "MapLineInfo", "CurSceneProgress", "CurSceneTarget", "FBAchievementsState"]:
        match = re.search(r"\bstruct " + name + r"\b[^;{]*\{", map_header).group()
        definition = function(map_header, match) + ";"
        parts.append(definition)
        definitions.append((name, definition))
    parts.append(function(map_cpp, "FObjInMapUI::FObjInMapUI()"))
    parts.append(r'''
using CircleQuestRewardDataMap = std::map<INT, CircleQuestRewardData>;
struct MMOQuestData {
    bool found = true;
    CircleQuestData circle;
    CircleQuestRewardDataMap _CircleQuestRewardDataMap;
    bool GetCircleQuestData(INT, INT, CircleQuestData& data) {
        if (!found) return false; data = circle; return true;
    }
    UBOOL GetCircleQuestRewardData(INT, INT, INT, CircleQuestRewardData&);
};
struct MMOMapDataManager {
    bool _IsTeleportInited = true;
    TArray<FTeleportNetNodeData> _RegionTeleportNetList[EWR_MAX];
    TArray<FPathPointInMapUI> _MapCurPath;
    FTeleportNetNodeData* GetTeleportNetNodes(EWorldRegion, INT);
    FTeleportNetNodeData* GetTeleportNetNodes(EWorldRegion, INT, const FString&);
    FTeleportNetNodeData* GetTeleportNetNodes(INT);
    INT GetTeleportNetNodesIn(EWorldRegion, INT, UBOOL, TArray<FTeleportNetNodeData*>*);
    INT GetTeleportNetNodesIn(INT, TArray<FTeleportNetNodeData*>*);
    void SetMapPath(TArray<FVector>&);
    void InvalidCurPath() { for (auto& point : _MapCurPath.data) point._IsValid = FALSE; }
};
''')
    parts.append(function(quest_cpp, "UBOOL MMOQuestData::GetCircleQuestRewardData("))
    for signature in [
            "FTeleportNetNodeData *MMOMapDataManager::GetTeleportNetNodes( EWorldRegion regionID, INT nodeID )",
            "FTeleportNetNodeData *MMOMapDataManager::GetTeleportNetNodes( EWorldRegion regionID, INT inSceneID, const FString &nodeName )",
            "FTeleportNetNodeData *MMOMapDataManager::GetTeleportNetNodes( INT nodeID )",
            "INT MMOMapDataManager::GetTeleportNetNodesIn( EWorldRegion regionID,",
            "INT MMOMapDataManager::GetTeleportNetNodesIn( INT mapID,",
            "void MMOMapDataManager::SetMapPath("]:
        parts.append(function(map_cpp, signature))
    parts.append("void RecordTests() {")
    skip = {"VarIndex", "_ID"}
    for name, definition in definitions:
        fields = re.findall(r"(?m)^\s*(?:INT|UINT|INT32|UBOOL)\s+(\w+)\s*;", definition)
        fields = [field for field in fields if field not in skip]
        arrays = re.findall(r"(?m)^\s*(?:INT|UINT|TCHAR)\s+(\w+)\[[^\]]+\]\s*;", definition)
        pointers = re.findall(r"(?m)^\s*(?:const )?\w+\s*\*\s*(\w+)\s*;", definition)
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& v) {" +
                     " ".join("assert(!v." + field + ");" for field in fields + pointers) +
                     " ".join("for (auto value : v." + field + ") assert(!value);" for field in arrays) + "});")
    parts.append(r'''
    TipsStallTests::Poisoned<QuestRecycleData>([](auto& v) { assert(v.VarIndex == INVALID_QUEST_VAR_ID && v.ItemSubType.empty()); });
    TipsStallTests::Poisoned<FMapMarkExport>([](auto& v) { assert(!v._Pos.X && !v._Pos.Y && !v._Pos.Z); });
    TipsStallTests::Poisoned<FPathPointInMapUI>([](auto& v) {
        assert(!v._OrigInMapPos.X && !v._OrigInMapPos.Y && !v._OrigInMapPos.Z);
        assert(!v._MapID && !v._IsValid && !v._UiObjInWorldMap && !v._UiObjInMiniMap && !v._UiObjInPathMap);
        assert(v._DisplayName.empty() && v._UiIconIdx.empty());
    });
    TipsStallTests::Poisoned<CurSceneTarget>([](auto& v) {
        assert(!v._MainTarget._Template && !v._MainTarget._CurValue);
        for (auto& child : v._ChildTargets) assert(!child._Template && !child._CurValue);
    });
}
void Run() {
    MMOQuestData quest;
    CircleQuestRewardData output;
    quest.circle.CircleSize = MAXINT;
    quest.circle.RewardId[MAX_QUEST_PER_CIRCLE - 1] = 777;
    quest._CircleQuestRewardDataMap[777].RewardId = 777;
    assert(quest.GetCircleQuestRewardData(1, MAX_QUEST_PER_CIRCLE - 1, 1, output) && output.RewardId == 777);
    for (int invalid : {-2147483647 - 1, -1, int(MAX_QUEST_PER_CIRCLE), MAXINT}) {
        output.RewardId = 765;
        assert(!quest.GetCircleQuestRewardData(1, invalid, 1, output) && output.RewardId == 765);
    }
    quest.circle.CircleSize = 1;
    assert(!quest.GetCircleQuestRewardData(1, MAX_QUEST_PER_CIRCLE - 1, 1, output));
    quest.found = false; assert(!quest.GetCircleQuestRewardData(1, 0, 1, output));
    MMOMapDataManager map;
    FTeleportNodeObj first, last; last.ServerID = 999; last.InSceneID = 999;
    FTeleportNetNodeData valid; valid._NodeTemplate = &first;
    FTeleportNetNodeData other; other._NodeTemplate = &last;
    map._RegionTeleportNetList[0].AddItem(FTeleportNetNodeData());
    map._RegionTeleportNetList[0].AddItem(valid);
    map._RegionTeleportNetList[EWR_MAX - 1].AddItem(other);
    TArray<FTeleportNetNodeData*> outputNodes;
    for (int invalid : {-2147483647 - 1, -1, int(EWR_MAX), MAXINT}) {
        auto region = (EWorldRegion)invalid;
        assert(!map.GetTeleportNetNodes(region, 123) && !map.GetTeleportNetNodes(region, 456, FString("node")));
        assert(map.GetTeleportNetNodesIn(region, 456, TRUE, &outputNodes) == -1);
    }
    assert(map.GetTeleportNetNodes(EWR_EAST, 123)->_NodeTemplate == &first);
    assert(map.GetTeleportNetNodes(EWR_EAST, 456, FString("node"))->_NodeTemplate == &first);
    assert(map.GetTeleportNetNodes(999)->_NodeTemplate == &last);
    assert(!map.GetTeleportNetNodes(777));
    assert(map.GetTeleportNetNodesIn(EWR_EAST, 456, TRUE, &outputNodes) == 1 && outputNodes.Num() == 1);
    assert(map.GetTeleportNetNodesIn(EWR_EAST, 999, FALSE, &outputNodes) == 0 && !outputNodes.Num());
    assert(map.GetTeleportNetNodesIn(456, &outputNodes) == 1 && outputNodes(0)->_NodeTemplate == &first);
    assert(map.GetTeleportNetNodesIn(EWR_EAST, 456, FALSE, nullptr) == -1);
    assert(map.GetTeleportNetNodesIn(456, nullptr) == -1);
    map._IsTeleportInited = false;
    assert(map.GetTeleportNetNodesIn(EWR_EAST, 456, TRUE, &outputNodes) == -1);
    assert(map.GetTeleportNetNodesIn(456, &outputNodes) == -1);
    for (int count : {4, 1, 7, 0}) {
        TArray<FVector> path;
        for (int i = 0; i < count; ++i) path.AddItem(FVector(i, i + 1, i + 2));
        map.SetMapPath(path);
        for (int i = 0; i < map._MapCurPath.Num(); ++i) {
            const auto& point = map._MapCurPath(i);
            assert(point._IsValid == (i < count));
            assert(!point._MapID && !point._UiObjInWorldMap && !point._UiObjInMiniMap && !point._UiObjInPathMap);
            if (i < count) assert(point._OrigInMapPos.X == i && point._OrigInMapPos.Y == i + 1 && point._OrigInMapPos.Z == i + 2);
        }
    }
    puts("Quest/map defaults, fixed reward capacity, teleport regions/null entries and path reuse passed");
}
}
''')
    return "\n".join(parts)
