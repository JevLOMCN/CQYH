"""Production world-object readers with exact wire buffers and missing actors."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    cpp = clean(source(game / "net/message_handler_Doodad.cpp"))
    wire = clean(source(client / "Common/message/g2c_message.h"))
    parsers = re.findall(r"void CMessageHandler::Parse_(\w+)\(const char\* buf, UINT32 size\)", cpp)
    if len(parsers) != 5:
        raise ValueError("World-object parser coverage changed")
    parts = [r'''
#pragma push_macro("SHIPPING_PC_GAME")
#undef SHIPPING_PC_GAME
#define SHIPPING_PC_GAME 0
namespace DoodadSafetyTests {
struct FRotator { int Pitch, Yaw, Roll; FRotator(int p, int y, int r) : Pitch(p), Yaw(y), Roll(r) {} };
struct FVector {
    float X, Y, Z;
    FVector(float x=0, float y=0, float z=0) : X(x), Y(y), Z(z) {}
    FVector operator-(const FVector& v) const { return FVector(X-v.X, Y-v.Y, Z-v.Z); }
    FRotator Rotation() { return FRotator(0, 1, 0); }
};
int ConvertToClientDegree(int value) { return value; }
void ConvertCoordN2C(FVector&) {}
enum { E_dt_Corpse = 1, g2c_sync_doodad_open_count = 2, c2g_operate_doodad = 3, sodtLeaveScene = 4 };
struct Actions { int Num() const { return 1; } };
struct Avatar { float CollisionHeight = 2, AvatarDrawScale = 3; };
struct UDoodad { Avatar* DoodadAvatar = nullptr; int DoodadType = 0; Actions OnSpawnEffectList; };
struct AMMODoodadActor {
    UDoodad* DoodadTemplate = nullptr;
    FVector Location;
    int effects = 0, remain = 1;
    void PlayActions(const Actions&) { ++effects; }
    int GetRemainOpenCount() { return remain; }
};
struct AMMOPlayer {
    UINT32 ServerID = 99;
    FVector Location;
    int rotations = 0;
    void SetDesiredRotation(FRotator, bool, bool, double, bool) { ++rotations; }
};
template<class T> T* Cast(AMMOPlayer* player) { return player; }
struct AMMOPlayerController {
    AMMOPlayer* Pawn = nullptr;
    int starts = 0, stops = 0;
    void OnOperateDoodadBarStart(const char*, int) { ++starts; }
    void OnOperateDoodadBarBarEnd(int) { ++stops; }
};
struct MMOFacade {
    AMMOPlayerController* controller = nullptr;
    AMMOPlayer* player = nullptr;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    AMMOPlayerController* GetMainController() { return controller; }
    AMMOPlayer* GetMainPlayer() { return player; }
};
namespace MMOUI { const char* ParseLocalizedPropertyPath(const char* value) { return value; } }
struct DoodadManager {
    UDoodad* definition = nullptr;
    AMMODoodadActor* actor = nullptr;
    int added = 0, refreshed = 0, started = 0, stopped = 0, updates = 0;
    UINT32 last_character = 0xffffffff;
    static DoodadManager& getSingleton() { static DoodadManager value; return value; }
    const UDoodad* FindDoodadTemplate(int) { return definition; }
    AMMODoodadActor* FindDoodadActor(int) { return actor; }
    AMMODoodadActor* AddDoodadActor(FVector, UDoodad*, FRotator, UINT32, BYTE) { ++added; return actor; }
    void RefreshDoodadEffects() { ++refreshed; }
    void OnDoodadBeginOperate(UINT32, UINT32, int) { ++started; }
    void OnDoodadEndOperate(UINT32, BYTE) { ++stopped; }
    void UpdateDoodadOperateCount(UINT32, BYTE, UINT32 character) { ++updates; last_character = character; }
    void DestroyDoodadActor(int, bool, int) {}
};
struct FSMIO {
    struct TargetType { void ClearActor(AMMODoodadActor*) {} } Target;
    static FSMIO& getSingleton() { static FSMIO value; return value; }
};
namespace ExternalMessage {
#pragma pack(push, 1)
''']
    parts.append(re.search(r"#define GAME_FPS\s+[^\n]+", clean(source(client / "Common/define/scene_def.h"))).group())
    for name in ["DOODAD_APPEAR_DATA", "G2C_SYNC_DOODAD_LIST", "G2C_DOODAD_APPEAR", "G2C_OPERATE_DOODAD_BEGIN",
                 "G2C_OPERATE_DOODAD_END", "G2C_SYNC_DOODAD_OPEN_COUNT"]:
        match = re.search(r"struct " + name + r"\b[^;{]*\{", wire).group()
        parts.append(function(wire, match) + ";")
    parts.append(function(clean(source(client / "Common/message/c2g_message.h")), "struct C2G_OPERATE_DOODAD :") + ";")
    parts.append("#pragma pack(pop)\n}\nusing namespace ExternalMessage;\nstruct CMessageHandler {")
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name in parsers)
    parts.append(r'''
    void Send_PickDoodadItem(INT);
    void _Send(const void*, size_t) {}
};
''')
    parts.extend(function(cpp, "void CMessageHandler::Parse_" + name + "(") for name in parsers)
    parts.append(function(cpp, "void CMessageHandler::Send_PickDoodadItem("))
    parts.append(r'''
int Effects() { auto& m = DoodadManager::getSingleton(); return m.added + m.refreshed + m.started + m.stopped + m.updates; }
using Parser = void (CMessageHandler::*)(const char*, UINT32);
template<class T> void ShortHeaders(CMessageHandler& handler, Parser parser) {
    int before = Effects(); (handler.*parser)(nullptr, 0); (handler.*parser)(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(T); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1); memset(bytes.get(), 0xff, size);
        (handler.*parser)(bytes.get(), size); assert(Effects() == before);
    }
}
void Run() {
    CMessageHandler handler;
''')
    for name in parsers:
        kind = re.search(r"\b(\w+)\* msg\s*=", function(cpp, "void CMessageHandler::Parse_" + name + "(")).group(1)
        parts.append("ShortHeaders<" + kind + ">(handler, &CMessageHandler::Parse_" + name + ");")
    parts.append(r'''
    auto& facade = MMOFacade::getSingleton(); auto& manager = DoodadManager::getSingleton();
    G2C_DOODAD_APPEAR appear = {}; appear.AppearData.nTemplateID = 123;
    G2C_OPERATE_DOODAD_BEGIN begin = {}; begin.dwCharID = 99; begin.wDuration = 32;
    G2C_OPERATE_DOODAD_END end = {}; end.dwCharID = 99;
    handler.Parse_g2c_doodad_appear((char*)&appear, sizeof(appear)); assert(!manager.added);
    handler.Parse_g2c_operate_doodad_begin((char*)&begin, sizeof(begin)); assert(manager.started == 1);
    handler.Parse_g2c_operate_doodad_end((char*)&end, sizeof(end)); assert(manager.stopped == 1);
    AMMOPlayer player; facade.player = &player;
    handler.Parse_g2c_operate_doodad_begin((char*)&begin, sizeof(begin)); assert(manager.started == 2);
    handler.Parse_g2c_operate_doodad_end((char*)&end, sizeof(end)); assert(manager.stopped == 2);
    AMMOPlayerController controller; facade.controller = &controller;
    handler.Parse_g2c_operate_doodad_begin((char*)&begin, sizeof(begin)); assert(!controller.starts);
    controller.Pawn = &player;
    UDoodad definition; Avatar avatar; definition.DoodadAvatar = &avatar; manager.definition = &definition;
    AMMODoodadActor actor; actor.DoodadTemplate = &definition; manager.actor = &actor;
    handler.Parse_g2c_doodad_appear((char*)&appear, sizeof(appear));
    assert(manager.added == 1 && manager.refreshed == 1 && actor.effects == 1);
    handler.Parse_g2c_operate_doodad_begin((char*)&begin, sizeof(begin));
    assert(controller.starts == 1 && player.rotations == 1);
    actor.DoodadTemplate = nullptr;
    handler.Parse_g2c_operate_doodad_begin((char*)&begin, sizeof(begin)); assert(controller.starts == 1);
    handler.Parse_g2c_operate_doodad_end((char*)&end, sizeof(end)); assert(controller.stops == 1);
    const size_t size = sizeof(G2C_SYNC_DOODAD_LIST) + 2 * sizeof(DOODAD_APPEAR_DATA);
    auto bytes = std::make_unique<char[]>(size); memset(bytes.get(), 0, size);
    auto* list = (G2C_SYNC_DOODAD_LIST*)bytes.get(); list->astAppearData[1].nTemplateID = 456;
    int before = Effects();
    for (UINT32 length = sizeof(*list) + 1; length < size; ++length) {
        if ((length - sizeof(*list)) % sizeof(DOODAD_APPEAR_DATA)) {
            handler.Parse_g2c_sync_doodad_list(bytes.get(), length); assert(Effects() == before);
        }
    }
    handler.Parse_g2c_sync_doodad_list(bytes.get(), (UINT32)size); assert(manager.added == 3);
    actor.remain = 1; handler.Send_PickDoodadItem(-123);
    assert(manager.updates == 1 && manager.last_character == 0);
    facade.player = nullptr; facade.controller = nullptr; manager.actor = nullptr; manager.definition = nullptr;
    puts("World-object packet lengths, partial records, missing players and local-message initialization passed");
}
}
#undef GAME_FPS
#pragma pop_macro("SHIPPING_PC_GAME")
''')
    return "\n".join(parts)
