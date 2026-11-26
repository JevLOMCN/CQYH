"""Real name validation, map conversion, and emitter branch regressions."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    parts = [r'''
#include <cmath>
#include <limits>
namespace NameBufferTests {
using TCHAR = wchar_t;
struct FString {
    std::wstring value;
    FString(const std::wstring& text) : value(text) {}
    const TCHAR* operator*() const { return value.c_str(); }
    INT Len() const { return (INT)value.size(); }
};
namespace MMOUI {
    INT calls = 0;
    bool HasSpecialSymbol(const FString& text) { ++calls; return text.value.find(L'!') != std::wstring::npos; }
}
struct UMMOUICreateRolePanel { INT CheckName(const FString&, INT&, INT&); };
''', function(clean(source(game / "Src/MMOUICreateRolePanel.cpp")), "INT UMMOUICreateRolePanel::CheckName("), r'''
void Run() {
    UMMOUICreateRolePanel panel;
    auto check = [&](const std::wstring& text, INT expected_size, INT expected_result = 1) {
        INT count = -123, size = -456, calls = MMOUI::calls;
        assert(panel.CheckName(FString(text), count, size) == expected_result);
        assert(count == (INT)text.size() && size == expected_size && MMOUI::calls == calls + 1);
    };
    check(L"", 0); check(L"abc123", 6); check(L"AZ", 6);
    check(L"\x4e00\x9fa5", 6); check(L"\x03b1\x03b2", 4);
    check(L"a!", 3, 0);
    for (INT length : {255, 256, 257, 1024, 65536}) {
        check(std::wstring(length, L'a'), length);
        check(std::wstring(length, L'Z'), length * 3);
        check(std::wstring(length, L'a') + L"\x4e00", length + 3);
        check(std::wstring(length, L'a') + L"!", length + 2, 0);
    }
    puts("Name validation handles long/Unicode text directly, initializes counts and scans symbols once");
}
}
namespace PositionDefaultTests {
struct FRotator;
struct FVector {
    FLOAT X, Y, Z;
    // Sentinels make an unset engine vector component observable consistently.
    FVector() : X(std::numeric_limits<FLOAT>::quiet_NaN()), Y(X), Z(X) {}
    FVector(FLOAT x, FLOAT y, FLOAT z) : X(x), Y(y), Z(z) {}
    FVector operator+(const FVector& v) const { return {X + v.X, Y + v.Y, Z + v.Z}; }
    FVector operator-(const FVector& v) const { return {X - v.X, Y - v.Y, Z - v.Z}; }
    FVector operator*(FLOAT scale) const { return {X * scale, Y * scale, Z * scale}; }
    FVector operator/(FLOAT scale) const { return {X / scale, Y / scale, Z / scale}; }
    friend FVector operator*(FLOAT scale, const FVector& v) { return v * scale; }
    FLOAT Size() const { return std::sqrt(X * X + Y * Y + Z * Z); }
    FVector SafeNormal() const { FLOAT length = Size(); return length < 1e-6f ? FVector(0, 0, 0) : *this / length; }
    FLOAT Dot2d(const FVector& v) const { return X * v.X + Y * v.Y; }
    FRotator Rotation() const;
};
struct FRotator {
    INT Pitch, Yaw, Roll;
    FRotator() : Pitch(-12345), Yaw(-12345), Roll(-12345) {}
    FRotator(INT p, INT y, INT r) : Pitch(p), Yaw(y), Roll(r) {}
    FRotator operator*(FLOAT scale) const { return {INT(Pitch * scale), INT(Yaw * scale), INT(Roll * scale)}; }
    FVector Vector() const { assert(!Pitch && !Yaw && !Roll); return {1, 0, 0}; }
};
FRotator FVector::Rotation() const {
    assert(std::isfinite(X) && std::isfinite(Y) && std::isfinite(Z));
    return FRotator(0, 0, 0);
}
using FVector2D = ::FVector2D;
const FLOAT SIN_MAP = 0, COS_MAP = 1;
struct MMOMapImgTiles {
    FVector2D _MapOffset2{4, 8}, _MapOffset{6, 10}, _Map3Dto2D{2, 4};
    FLOAT _ZoomFactor = 2;
    FVector World3DPosByScaledMap2DPos(FVector2D);
    FVector World3DPosByMap2DPos(FVector2D);
};
''']
    tiles = clean(source(game / "Src/MMOMapImgTiles.cpp"))
    for signature in ["FVector MMOMapImgTiles::World3DPosByScaledMap2DPos(", "FVector MMOMapImgTiles::World3DPosByMap2DPos("]:
        parts.append(function(tiles, signature))
    generated = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    for name in ["EProjectileTargetType", "ETrackLineType", "EEmitterType"]:
        parts.append(function(generated, "enum " + name + "\n") + ";")
    parts.append(r'''
// Translation matrices suffice for these branch tests; angular velocity is zero.
struct FMatrix {
    FVector origin;
    FMatrix() {}
    explicit FMatrix(FVector position) : origin(position) {}
    FVector GetOrigin() const { return origin; }
    FRotator Rotator() const { return FRotator(0, 0, 0); }
    FMatrix Inverse() const { return FMatrix(origin * -1); }
    FMatrix operator*(const FMatrix& other) const { return FMatrix(origin + other.origin); }
    FVector TransformFVector(FVector point) const { return point + origin; }
    FVector InverseTransformFVector(FVector point) const { return point - origin; }
};
struct FRotationMatrix : FMatrix { FRotationMatrix(FRotator r) : FMatrix(FVector(0, 0, 0)) { assert(!r.Pitch && !r.Yaw && !r.Roll); } };
template<class T, class U> T CubicInterp(T p0, T t0, T p1, T t1, U a) {
    return p0 * (2*a*a*a - 3*a*a + 1) + t0 * (a*a*a - 2*a*a + a) + p1 * (-2*a*a*a + 3*a*a) + t1 * (a*a*a - a*a);
}
enum { E_mmt_Static, E_mmt_AccordingToTerrain };
struct AActor {
    FVector Location{0, 0, 0};
    FRotator Rotation{0, 0, 0};
    bool pending = false;
    bool IsPendingKill() const { return pending; }
    FLOAT GetValidZValue(FVector point, INT, INT) { return point.Z; }
};
struct MMOFacade {
    AActor actor;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    AActor* GetMainActor() { return &actor; }
};
struct EmitterConfig {
    ETrackLineType TrackLineType = ETLT_Line;
    EProjectileTargetType ProjectileTargetType = EPTT_Actor;
    FLOAT Duration = 2, LoopCount = 1, MaxHeight = 4, TimeToFinishFixRotation = 1, RotateRadius = 4;
    FRotator RotateVelocity{0, 0, 0};
    bool bAttach = false;
    FMatrix GetSocketMatrix(AActor* actor, FVector, BOOL) { assert(actor); return FMatrix(actor->Location); }
};
struct MissileConfig { INT MoveType = E_mmt_Static; };
struct AMMOEmitter : AActor {
    INT OverwriteEmitterType = EMIT_FlyTo, OverwriteTargetType = EPTT_TargetPoint;
    EmitterConfig* EmitterData = nullptr;
    MissileConfig* MissileData = nullptr;
    AActor* Target = nullptr;
    AActor* Creator = nullptr;
    FLOAT CurrentTime = 1, Duration = 2, Speed = 1, DistanceToLand = 0, MaxDistance = 8;
    FVector TargetPoint{18, 28, 38};
    FMatrix SpawnMatrix{FVector(2, 4, 6)}, BaseActorMatrix{FVector(0, 0, 0)};
    struct { FVector v1{0, 0, 0}, v2{0, 0, 0}; } OverwriteBezierControl;
    bool killed = false;
    void SetLocation(FVector value) { Location = value; }
    void SetRotation(FRotator value) { Rotation = value; }
    FMatrix LocalToWorld() { return FMatrix(Location); }
    void Kill() { killed = true; }
    void Fly(FLOAT DeltaSeconds);
    void Rotate(FLOAT DeltaSeconds);
    FVector BeamNormal();
};
''')
    cpp = clean(source(game / "AbilityProcessor.cpp"))
    tick = function(cpp, "void AMMOEmitter::TickSpecial(")
    parts.append('#pragma push_macro("check")\n#undef check\n#define check(value) assert(value)')
    for kind in ["FlyTo", "Rotate"]:
        body = function(tick, "else if (OverwriteEmitterType == EMIT_" + kind + ")")
        parts.append("void AMMOEmitter::" + ("Fly" if kind == "FlyTo" else "Rotate") + "(FLOAT DeltaSeconds) {\n" + body.replace("else if", "if", 1) + "\n}")
    connect = function(tick, "else if (OverwriteEmitterType == EMIT_ConnectTo)")
    start = connect.index("FVector Dst(0, 0, 0);")
    end = connect.index("else if (EmitterData->ProjectileTargetType == EPTT_UseCasterDirection")
    parts.append("FVector AMMOEmitter::BeamNormal() {\n" + connect[start:end] + "\nreturn HitNormal;\n}")
    parts.append(r'''
#pragma pop_macro("check")
void Equal(FVector value, FVector expected) { assert(value.X == expected.X && value.Y == expected.Y && value.Z == expected.Z); }
void Run() {
    MMOMapImgTiles tiles;
    Equal(tiles.World3DPosByScaledMap2DPos(FVector2D(36, 60)), FVector(4, 5, 0));
    Equal(tiles.World3DPosByMap2DPos(FVector2D(36, 60)), FVector(10.5f, 13, 0));
    EmitterConfig config;
    AMMOEmitter emitter; emitter.EmitterData = &config;
    emitter.Location = FVector(10, 20, 30);
    emitter.OverwriteTargetType = 255; emitter.Fly(0.1f);
    Equal(emitter.Location, FVector(10, 16, 22));
    emitter.Location = FVector(10, 20, 30); emitter.OverwriteTargetType = EPTT_Actor;
    emitter.Fly(0.1f); Equal(emitter.Location, FVector(6, 12, 18));
    for (INT type : {int(EPTT_UseCasterDirection), int(EPTT_UseCasterDirectionAndCollision)}) {
        emitter.Location = FVector(10, 20, 30); emitter.OverwriteTargetType = type;
        emitter.Fly(0.1f); Equal(emitter.Location, FVector(6, 4, 6));
    }
    emitter.Location = FVector(10, 20, 30); emitter.Rotation = FRotator(1, 2, 3);
    config.TrackLineType = (ETrackLineType)255;
    emitter.Fly(0.1f); Equal(emitter.Location, FVector(10, 20, 30));
    assert(emitter.Rotation.Pitch == 1 && emitter.Rotation.Yaw == 2 && emitter.Rotation.Roll == 3);
    emitter.Rotation = FRotator(0, 0, 0); emitter.OverwriteTargetType = EPTT_TargetPoint;
    for (auto track : {ETLT_Parabola, ETLT_Bezier}) {
        config.TrackLineType = track; emitter.Fly(0.1f);
        assert(std::isfinite(emitter.Location.X) && std::isfinite(emitter.Location.Y) && std::isfinite(emitter.Location.Z));
    }
    config.ProjectileTargetType = EPTT_Actor;
    Equal(emitter.BeamNormal(), FVector(0, 0, 0));
    config.ProjectileTargetType = (EProjectileTargetType)255;
    Equal(emitter.BeamNormal(), FVector(0, 0, 0));
    emitter.OverwriteEmitterType = EMIT_Rotate;
    emitter.SpawnMatrix = FMatrix(FVector(10, 20, 30)); emitter.Location = FVector(14, 20, 30);
    AActor creator; creator.Location = FVector(10, 20, 30); emitter.Creator = &creator;
    emitter.Rotate(0.1f); Equal(emitter.Location, FVector(14, 20, 30));
    config.bAttach = true; emitter.Rotate(0.1f); Equal(emitter.Location, FVector(14, 20, 30));
    puts("Map heights and emitter destinations, missing-target normals, invalid tracks and unattached transforms are defined");
}
}
''')
    return "\n".join(parts)
