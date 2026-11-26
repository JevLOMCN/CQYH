"""Production viewport queries, pick-ray generation and hit-point damage defaults."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', text)

    parts = [r'''
namespace ViewportQueryTests {
struct FVector {
    FLOAT X, Y, Z;
    FVector() : X(std::numeric_limits<FLOAT>::quiet_NaN()), Y(X), Z(X) {}
    FVector(FLOAT x, FLOAT y, FLOAT z) : X(x), Y(y), Z(z) {}
    FVector operator+(FVector v) const { return {X + v.X, Y + v.Y, Z + v.Z}; }
    FVector operator*(FLOAT s) const { return {X * s, Y * s, Z * s}; }
};
struct FVector2D {
    FLOAT X, Y;
    FVector2D() : X(std::numeric_limits<FLOAT>::quiet_NaN()), Y(X) {}
    FVector2D(FLOAT x, FLOAT y) : X(x), Y(y) {}
    FVector2D operator-(FVector2D v) const { return {X - v.X, Y - v.Y}; }
    bool IsNearlyZero() const { return std::abs(X) < 1e-6f && std::abs(Y) < 1e-6f; }
};
struct FRotator {};
struct UObject { virtual ~UObject() {} };
template<class T> T* Cast(UObject* p) { return dynamic_cast<T*>(p); }
struct FViewport {
    INT width = 800, height = 600;
    INT GetSizeX() const { return width; }
    INT GetSizeY() const { return height; }
};
struct UGameViewportClient {
    FViewport* Viewport = nullptr;
    INT ShowFlags = 0;
    FVector2D mouse{400, 300};
    void GetViewportSize(FVector2D&);
    FVector2D GetMousePosition() const { return mouse; }
};
struct WorldInfo { FLOAT TimeSeconds = 1, DeltaSeconds = 2, RealTimeSeconds = 3; };
struct AActor { struct WorldInfo* WorldInfo = nullptr; };
struct World { void* Scene = nullptr; };
World* GWorld = nullptr;
struct FSceneViewFamilyContext {
    FSceneViewFamilyContext(FViewport* vp, void*, INT, FLOAT, FLOAT, FLOAT,
                           UBOOL, UBOOL, UBOOL, UBOOL, UBOOL) { assert(vp); }
};
struct FSceneView {
    INT SizeX = 400, SizeY = 300, projections = 0;
    FVector2D last_screen{0, 0};
    void DeprojectFVector2D(FVector2D pos, FVector& origin, FVector& direction) {
        ++projections; last_screen = pos;
        origin = FVector(10, 20, 30); direction = FVector(pos.X, pos.Y, 1);
    }
};
struct ULocalPlayer : UObject {
    UGameViewportClient* ViewportClient = nullptr;
    AActor* Actor = nullptr;
    FSceneView* view = nullptr;
    INT calculations = 0;
    FSceneView* CalcSceneView(FSceneViewFamilyContext*, FVector&, FRotator&, FViewport*, void*) {
        ++calculations; return view;
    }
    void DeProject(FVector2D, FVector&, FVector&);
};
struct AMMOPlayerCamera : UObject {};
struct AMMOPlayerController {
    UObject* Player = nullptr;
    UObject* PlayerCamera = nullptr;
    FVector2D LastScreenMousePos{0, 0}, PlayerMouse{0, 0};
    UBOOL IsMouseMoving = FALSE;
    FVector MouseProjectionOrigin, MouseProjectionDirection, StartTrace, EndTrace;
    void NativeGeneratePickRay();
};
''']
    player = clean(source(game.parent / "Engine/Src/UnPlayer.cpp"))
    for signature in ["void UGameViewportClient::GetViewportSize(", "void ULocalPlayer::DeProject("]:
        parts.append(function(player, signature))
    parts.append(function(clean(source(game / "Src/MMOPlayerController.cpp")),
                          "void AMMOPlayerController::NativeGeneratePickRay()"))
    parts.append(r'''
void Equal(FVector v, FLOAT x, FLOAT y, FLOAT z) { assert(v.X == x && v.Y == y && v.Z == z); }
void Run() {
    FViewport viewport;
    UGameViewportClient client;
    FVector2D size;
    client.GetViewportSize(size);
    assert(size.X == 0 && size.Y == 0);
    client.Viewport = &viewport;
    client.GetViewportSize(size);
    assert(size.X == 800 && size.Y == 600);
    client.Viewport = nullptr;
    client.GetViewportSize(size);
    assert(size.X == 0 && size.Y == 0); // Also clears a previously valid result.

    World world;
    WorldInfo info;
    AActor actor; actor.WorldInfo = &info;
    FSceneView view;
    ULocalPlayer local;
    auto query_empty = [&] {
        FVector origin(7, 8, 9), direction(4, 5, 6);
        local.DeProject(FVector2D(.5f, .5f), origin, direction);
        Equal(origin, 0, 0, 0); Equal(direction, 0, 0, 0);
    };
    query_empty();
    local.ViewportClient = &client; query_empty();
    client.Viewport = &viewport; query_empty();
    local.Actor = &actor; query_empty(); // GWorld is absent.
    GWorld = &world; actor.WorldInfo = nullptr; query_empty();
    actor.WorldInfo = &info; query_empty(); // CalcSceneView returns null.
    assert(local.calculations == 1);
    local.view = &view;
    FVector origin, direction;
    local.DeProject(FVector2D(.5f, .5f), origin, direction);
    Equal(origin, 10, 20, 30); Equal(direction, 200, 150, 1);
    assert(view.last_screen.X == 200 && view.last_screen.Y == 150);

    AMMOPlayerCamera camera;
    UObject wrong_type;
    AMMOPlayerController controller;
    auto empty_ray = [&] {
        controller.MouseProjectionOrigin = FVector(1, 2, 3);
        controller.MouseProjectionDirection = FVector(4, 5, 6);
        controller.StartTrace = FVector(7, 8, 9);
        controller.EndTrace = FVector(10, 11, 12);
        controller.NativeGeneratePickRay();
        Equal(controller.MouseProjectionOrigin, 0, 0, 0);
        Equal(controller.MouseProjectionDirection, 0, 0, 0);
        Equal(controller.StartTrace, 0, 0, 0); Equal(controller.EndTrace, 0, 0, 0);
    };
    empty_ray();
    controller.Player = &wrong_type; empty_ray();
    controller.Player = &local; local.ViewportClient = nullptr; empty_ray();
    local.ViewportClient = &client; client.Viewport = nullptr; empty_ray();
    client.Viewport = &viewport;
    for (auto dims : {FVector2D(0, 600), FVector2D(800, 0), FVector2D(0, 0),
                      FVector2D(-1, 600), FVector2D(800, -1)}) {
        viewport.width = (INT)dims.X; viewport.height = (INT)dims.Y;
        INT calls = local.calculations;
        empty_ray(); assert(local.calculations == calls);
    }
    viewport.width = 800; viewport.height = 600;
    controller.PlayerCamera = &camera;
    local.view = nullptr; empty_ray();
    local.view = &view; local.Actor = nullptr; empty_ray();
    local.Actor = &actor;
    controller.NativeGeneratePickRay();
    Equal(controller.MouseProjectionOrigin, 10, 20, 30);
    Equal(controller.MouseProjectionDirection, 200, 150, 1);
    Equal(controller.StartTrace, 10, 20, 30);
    Equal(controller.EndTrace, 10 + 200 * 50000.f, 20 + 150 * 50000.f, 30 + 50000.f);
    controller.NativeGeneratePickRay();
    assert(!controller.IsMouseMoving);
    controller.PlayerCamera = nullptr;
    controller.NativeGeneratePickRay();
    Equal(controller.StartTrace, 0, 0, 0); Equal(controller.EndTrace, 0, 0, 0);
    GWorld = nullptr;
    puts("Viewport queries and pick rays define outputs for missing viewport/actor/world/view and zero dimensions");
}
}
''')

    # Exercise the exact generated record and the production initialization block
    # over poisoned storage, so the empty generated constructor cannot hide bugs.
    data = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(data.decode("utf-16" if data.startswith(b"\xff\xfe") else "latin1"))
    generated = generated.replace("\r\n", "\n")
    record = function(generated, "struct FDamageData\n") + ";"
    on_action = function(clean(source(game / "AbilityProcessor.cpp")), "void HitPointComponent::OnAction(")
    block = on_action[on_action.index("FDamageData DamageData"):on_action.index("if ( Link )")]
    match = re.match(r"FDamageData DamageData(\([^;]*\))?;", block)
    assert match is not None
    initializer = match.group(1) or ""
    assignments = block[match.end():]
    parts.extend([r'''
#pragma push_macro("SCRIPT_ALIGN")
#undef SCRIPT_ALIGN
#define SCRIPT_ALIGN
namespace DamageRecordTests {
using BITFIELD = unsigned int;
enum EEventParm { EC_EventParm };
struct AActor {};
struct UAbilityHitPointComponent { INT DamageValue = 123; };
''', record, r'''
void Run() {
    UAbilityHitPointComponent ability;
    AActor actor;
    UAbilityHitPointComponent* HitPointData = &ability;
    AActor* BaseActor = &actor;
    INT CreatorId = 42;
    alignas(FDamageData) unsigned char bytes[sizeof(FDamageData)];
    memset(bytes, 0xff, sizeof(bytes));
''', "    FDamageData& DamageData = *new(bytes) FDamageData" + initializer + ";\n",
                  assignments, r'''
    assert(DamageData.AbilityData == &ability && DamageData.BaseActor == &actor);
    assert(DamageData.CreatorId == 42 && DamageData.DamageValue == 123);
    assert(!DamageData.bServerDamageData && !DamageData.ElementaryType);
    assert(!DamageData.bCauseDead && !DamageData.DamageType);
    puts("Hit-point damage initializes cause-death and damage-type fields over poisoned record storage");
}
}
#pragma pop_macro("SCRIPT_ALIGN")
'''])
    return "\n".join(parts)
