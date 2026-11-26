"""Production scene packet guards, variable lists and pending player/reward records."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    scene = clean(source(game / "net/message_handler_Scene.cpp"))
    parsers = [function(scene, match.group()) for match in re.finditer(
        r"void CMessageHandler::Parse_\w+\(const char\* buf, UINT32 size\)\s*\{", scene)]
    if len(parsers) != 25:
        raise ValueError("Scene parser coverage changed")
    packet_types = [re.search(r"\b(\w+)\* msg\s*=", body).group(1) for body in parsers]
    wire = clean(source(client / "Common/message/g2c_message.h")) + clean(source(client / "Common/message/m2c_message.h"))
    definitions = {}
    for match in re.finditer(r"\bstruct\s+(\w+)\b[^;{]*\{", wire):
        definitions[match[1]] = function(wire[match.start():], match.group()) + ";"
    declarations, emitted = [], set()

    def include(name):
        if name in emitted:
            return
        emitted.add(name)
        for word in re.findall(r"\b\w+\b", definitions[name]):
            if word != name and word in definitions:
                include(word)
        declarations.append(definitions[name])

    for name in packet_types:
        include(name)
    count = re.search(r"#define MAX_CLIENT_VARIABLE_COUNT\s+([^\n]+)",
                      clean(source(client / "Common/define/variable_def.h"))).group(1)
    parts = [r'''
#pragma push_macro("MAX_CLIENT_VARIABLE_COUNT")
#undef MAX_CLIENT_VARIABLE_COUNT
''', "#define MAX_CLIENT_VARIABLE_COUNT " + count, r'''
namespace SceneRecordTests {
using INT16 = int16_t;
using FString = AttributeRecordTests::FString;
template<class T> using TArray = SkillQuestTests::TArray<T>;
template<class K, class V> using TMap = SkillQuestTests::TMap<K, V>;
template<class T> using SingletonMeyers = SkillQuestTests::SingletonMeyers<T>;
using INativeGameEventHandler = SkillQuestTests::INativeGameEventHandler;
using MMOGameEvent = SkillQuestTests::MMOGameEvent;
using FScriptDelegate = ManagerDefaultsTests::FScriptDelegate;
using ManagerDefaultsTests::EC_EventParm;
''', function(clean(source(client / "Common/define/char_def.h")), "enum ENUM_COMBAT_STAT_ENTRY") + ";",
        "#pragma pack(push, 1)", *declarations, "#pragma pack(pop)"]
    names = []
    for body, packet in zip(parsers, packet_types):
        name = re.search(r"void CMessageHandler::Parse_(\w+)", body).group(1)
        # Compile the real entry guard, stopping before any engine side effects.
        prefix = re.search(r"\A[\s\S]+?if \(!buf \|\| size < sizeof\(\*msg\)(?:[^;]+?)return;", body)
        if not prefix:
            raise ValueError("Missing scene entry guard: " + name)
        code = prefix.group().split("{", 1)[1].replace("return;", "return false;")
        parts.append("bool Header_" + name + "(const char* buf, UINT32 size) {" + code + "return true; }")
        names.append((name, packet))
    parts.append(r'''
struct FVector { float X, Y, Z; FVector(float x, float y, float z) : X(x), Y(y), Z(z) {} };
void ConvertCoordN2C(FVector& pos) { pos.X *= 10; pos.Y *= -10; }
float g_preLoadX = 0, g_preLoadY = 0;
struct MapInfo { int ServerID = 123; } map;
struct MMOMapDataManager : SingletonMeyers<MMOMapDataManager> { MapInfo& GetCurMap() { return map; } };
struct USeqEvent_Server2Client_VarChange_Scene {};
enum { adKilledByNpcIDCount = 0, vtRole = 1, GameEvent_VariableInit = 2 };
struct MMOGameEvent_Achievement { MMOGameEvent_Achievement(int, int, int, int) {} };
struct MMOGameEvent_VariableChange { MMOGameEvent_VariableChange(int, int, int, int, int) {} };
namespace MMOUI { int events = 0; template<class T> void PostGameEvent(T*) { ++events; } }
int transducers = 0;
void UpdateTransducer(int) { ++transducers; }
namespace MMO {
    int changes = 0, stored = 0;
    template<class T> void TActiveKismetEvent_ServerVarChange(int, int, int, bool) { ++changes; }
    void SetServerSceneVars(const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA*, int count) { stored += count; }
}
struct GuildNotifications : SingletonMeyers<GuildNotifications> {
    int calls = 0; void NotifyUpdateSceneVariable(int, int, int) { ++calls; }
};
''')
    # The real consumer is retained below; this alias only stands in for unrelated guild UI.
    scene_variables = function(scene, "void CMessageHandler::Parse_g2c_sync_scene_variables(")
    scene_variables = scene_variables.replace("MMOGuildManager::", "GuildNotifications::")
    parts.append(function(clean(source(game / "MMOClientVariableManager.h")), "enum EClientVariableType\n") + ";")
    parts.append(r'''
struct MMOClientVariableManager : SingletonMeyers<MMOClientVariableManager> {
    int synced = 0; std::map<int, int> values;
    void SyncClientVariables(INT32* data) {
        for (int i = 0; i < MAX_CLIENT_VARIABLE_COUNT; ++i) assert(data[i] == i);
        ++synced;
    }
    void SetClientVariable(EClientVariableType index, INT32 value) { values[index] = value; }
};
struct MMOFacade : SingletonMeyers<MMOFacade> {
    int frames = 0, scores = 0;
    void SetStartServerFrameCount(INT32 value) { frames = value; }
    void SetFBOtherScore(const G2C_DUNGEON_FINISH_SCORE& data) {
        for (int i = 0; i < data.byCount; ++i) { assert(data.stScore[i].dwRoleID == 100U + i); ++scores; }
    }
};
struct CMessageHandler {
    bool _lockPlayer = true;
    void Parse_g2c_sync_frame(const char*, UINT32);
    void Parse_g2c_sync_next_jump_point(const char*, UINT32);
    void Parse_g2c_sync_scene_variables(const char*, UINT32);
    void Parse_g2c_sync_role_variables(const char*, UINT32);
    void Parse_g2c_sync_client_variables(const char*, UINT32);
    void Parse_g2c_dungeon_finish_score(const char*, UINT32);
    void Send_UpdateClientVarRange(UINT8, UINT8, BYTE*, size_t);
};
''')
    parts.append(scene_variables)
    for name in ["sync_frame", "sync_next_jump_point", "sync_role_variables", "sync_client_variables", "dungeon_finish_score"]:
        parts.append(function(scene, "void CMessageHandler::Parse_g2c_" + name + "("))
    parts.append(function(scene, "void CMessageHandler::Send_UpdateClientVarRange("))
    parts.append("void PacketTests() {")
    for name, packet in names:
        parts.append("{ auto read = Header_" + name + "; " + packet + " full = {};" +
                     "assert(!read(nullptr, sizeof(full)));" +
                     "for (UINT32 size = 0; size < sizeof(full); ++size) {" +
                     "std::vector<char> bytes(size ? size : 1); assert(!read(bytes.data(), size)); }" +
                     "assert(read((const char*)&full, sizeof(full))); }")
    parts.append(r'''
    CMessageHandler handler;
    G2C_SYNC_FRAME frame = {}; frame.nFrame = 456;
    handler.Parse_g2c_sync_frame((const char*)&frame, sizeof(frame));
    assert(!handler._lockPlayer && MMOFacade::getSingleton().frames == 456);
    G2C_SYNC_NEXT_JUMP_POINT point = {}; point.wJumpX = 12; point.wJumpY = 34;
    handler.Parse_g2c_sync_next_jump_point((const char*)&point, sizeof(point));
    assert(g_preLoadX == 120 && g_preLoadY == -340);
    for (bool role : {false, true}) {
        using Packet = G2C_SYNC_SCENE_VARIABLES;
        auto read = role ? &CMessageHandler::Parse_g2c_sync_role_variables : &CMessageHandler::Parse_g2c_sync_scene_variables;
        for (UINT32 actual = 0; actual <= sizeof(Packet) + 3 * sizeof(Packet::VARIABLE_DATA); ++actual) {
            for (UINT16 declared : {UINT16(0), UINT16(1), UINT16(3), UINT16(4), UINT16(actual), UINT16(actual + 1), UINT16(65535)}) {
                std::vector<char> bytes(std::max(actual, UINT32(sizeof(Packet))));
                auto* packet = (Packet*)bytes.data(); packet->wSize = declared;
                int oldChanges = MMO::changes, oldEvents = MMOUI::events;
                (handler.*read)(bytes.data(), actual);
                bool valid = actual >= sizeof(Packet) && declared >= sizeof(Packet) && declared <= actual &&
                    (declared - sizeof(Packet)) % sizeof(Packet::VARIABLE_DATA) == 0;
                int count = valid ? (declared - sizeof(Packet)) / sizeof(Packet::VARIABLE_DATA) : 0;
                assert(MMO::changes == oldChanges + (role ? 0 : count));
                assert(MMOUI::events == oldEvents + (role ? count * 2 : 0));
            }
        }
    }
    G2C_SYNC_CLIENT_VARIABLES variables = {};
    for (int i = 0; i < MAX_CLIENT_VARIABLE_COUNT; ++i) variables.nVariableList[i] = i;
    for (UINT32 length = 0; length < sizeof(variables); ++length)
        handler.Parse_g2c_sync_client_variables((const char*)&variables, length);
    assert(!MMOClientVariableManager::getSingleton().synced);
    handler.Parse_g2c_sync_client_variables((const char*)&variables, sizeof(variables));
    assert(MMOClientVariableManager::getSingleton().synced == 1);
    for (int capacity = 0; capacity < 4; ++capacity) {
        for (int count = 0; count < 5; ++count) {
            std::vector<char> bytes(sizeof(G2C_DUNGEON_FINISH_SCORE) + capacity * sizeof(SCENE_SCORE));
            auto* packet = (G2C_DUNGEON_FINISH_SCORE*)bytes.data(); packet->byCount = count;
            for (int i = 0; i < capacity; ++i) packet->stScore[i].dwRoleID = 100 + i;
            int old = MMOFacade::getSingleton().scores;
            handler.Parse_g2c_dungeon_finish_score(bytes.data(), (UINT32)bytes.size());
            assert(MMOFacade::getSingleton().scores == old + (count <= capacity ? count : 0));
        }
    }
    INT32 values[] = {10, 20, 30};
    handler.Send_UpdateClientVarRange(0, 2, nullptr, sizeof(values));
    assert(MMOClientVariableManager::getSingleton().values.empty());
    handler.Send_UpdateClientVarRange(0, 2, (BYTE*)values, sizeof(values));
    assert(MMOClientVariableManager::getSingleton().values[2] == 30);
    puts("Scene header guards (25 readers), variable byte lengths, score counts and null ranges passed");
}
''')
    records = []
    battle = clean(source(game / "MMOBattleManager.h"))
    parts.append(function(battle, "enum eBattlePlayerState\n") + ";")
    for path, names in [("MMOBattleManager.h", ["FBattlePlayerScore", "FBattlePlayerAwards", "FBattlePlayer"]),
                        ("MMODailySignInManager.h", ["SSignInReward", "SSignInRewardState"]),
                        ("MMOFacade.h", ["FBSummaryData", "FBAward", "FBMemberAwardData"])]:
        text = clean(source(game / path))
        if path == "MMOFacade.h":
            parts.append(re.search(r"static const UINT MAX_FB_AWARDS_CNT\s*=\s*[^;]+;", text).group())
        for name in names:
            definition = function(text, "struct " + name + "\n") + ";"
            parts.append(definition)
            fields = re.findall(r"(?m)^\s*(?:INT|UINT|UINT32|UBOOL|bool|BYTE)\s+(\w+)\s*;", definition)
            records.append((name, fields))
    daily = clean(source(game / "MMODailySignInManager.h"))
    parts.append(function(daily, "class MMODailySignInManager :").replace("private:", "public:") + ";")
    daily_cpp = clean(source(game / "MMODailySignInManager.cpp"))
    for name in ["MMODailySignInManager::MMODailySignInManager()", "MMODailySignInManager::~MMODailySignInManager()",
                 "void MMODailySignInManager::ClearData()"]:
        parts.append(function(daily_cpp, name))
    guild_cpp = clean(source(game / "MMOGuildManager.cpp"))
    guild_header = clean(source(game / "MMOGuildManager.h"))
    guild_ctor = function(guild_cpp, "MMOGuildManager::MMOGuildManager()")
    guild_fields = re.findall(r"FScriptDelegate\s+(_Open\w+Delegate)\s*;", guild_header)
    parts.append("struct MMOGuildManager { MMOGuildManager(); " +
                 " ".join("FScriptDelegate " + name + ";" for name in guild_fields) + " };")
    parts.append(guild_ctor[:guild_ctor.index("{")] + "{}")
    parts.append("void RecordTests() {")
    for name, fields in records:
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& v) {" +
                     " ".join("assert(!v." + field + ");" for field in fields) + "});")
    parts.append(r'''
    TipsStallTests::Poisoned<FBattlePlayer>([](auto& v) {
        assert(v._PlayerName.empty() && v._ServerName.empty() && v._State == EBPS_OFFLINE);
        assert(!v._Score._Sum && !v._Score._Kills && !v._Score._BeKilled && !v._Score._Damages && !v._Score._Heals);
        assert(!v._Awards._Points);
    });
    TipsStallTests::Poisoned<FBMemberAwardData>([](auto& v) {
        for (auto& award : v.m_Award) assert(!award.uiItemID && !award.uiItemCnt && !award.isValid);
    });
    TipsStallTests::Poisoned<MMODailySignInManager>([](auto& v) {
        assert(!v._MyCurSignInState._IsValid && !v._MyCurSignInState._CanSignIn);
        assert(!v._OpenDailySignInPanelDelegate.Object && !v._OpenDailySignInPanelDelegate.FunctionName.index);
        assert(v._HintSignInMsgButtonIdx == -1);
        v._MyCurSignInState._CurSignInDay = 28; v._MyCurSignInState._CanSignIn = TRUE;
        v.ClearData();
        assert(!v._MyCurSignInState._CurSignInDay && !v._MyCurSignInState._CanSignIn && v._MyCurSignInState._IsValid);
    });
''')
    parts.append("TipsStallTests::Poisoned<MMOGuildManager>([](auto& v) {" + " ".join(
        "assert(!v." + name + ".Object && !v." + name + ".FunctionName.index);" for name in guild_fields) + "});")
    parts.append(r'''
    puts("Battle/sign-in/dungeon defaults and sign-in/guild callback initialization passed");
}
}
#pragma pop_macro("MAX_CLIENT_VARIABLE_COUNT")
''')
    return "\n".join(parts)
