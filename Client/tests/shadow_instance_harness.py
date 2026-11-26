"""Real shadow instance allocation, release and refresh with invalid/free slots."""
import re


def harness(game, source, function):
    text = re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(game / 'Src/InstancedShadowPlaneActor.cpp'))
    parts = [r'''
template<class T> struct ShadowArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    bool IsValidIndex(INT index) const { return index >= 0 && index < Num(); }
    T* GetData() { return values.data(); }
    T& operator()(INT index) { assert(IsValidIndex(index)); return values[index]; }
    void Empty(INT count) { values.clear(); values.reserve(count); }
    void Add(INT count) { values.resize(values.size() + count); }
};
template<class T> void* operator new(size_t size, ShadowArray<T>& array) {
    assert(size == sizeof(T)); array.Add(1); return &array.values.back();
}
template<class T> void operator delete(void*, ShadowArray<T>&) {}
namespace ShadowInstanceTests {
enum { MAX_SHADOW_NUM = 100 };
struct FVector {
    FLOAT X, Y, Z;
    FVector(FLOAT x = 0, FLOAT y = 0, FLOAT z = 0) : X(x), Y(y), Z(z) {}
};
struct FVector2D {
    FLOAT X, Y;
    FVector2D(FLOAT x = 0, FLOAT y = 0) : X(x), Y(y) {}
};
struct FMatrix {
    FVector origin;
    FLOAT scale = 1;
    static const FMatrix Identity;
    void SetOrigin(FVector point) { origin = point; }
    FMatrix ApplyScale(FLOAT value) const { FMatrix copy = *this; copy.scale *= value; return copy; }
};
const FMatrix FMatrix::Identity;
struct FInstancedStaticMeshInstanceData {
    FMatrix Transform;
    FVector2D LightmapUVBias, ShadowmapUVBias;
};
struct UInstancedStaticMeshComponent {
    ShadowArray<FInstancedStaticMeshInstanceData> PerInstanceSMData;
    UBOOL bNeedsReattach = FALSE;
};
struct Settings {
    UBOOL dynamic = FALSE;
    UBOOL GetAllowDynamicShadows() const { return dynamic; }
} GSystemSettings;
struct AInstancedShadowPlaneActor {
    UInstancedStaticMeshComponent* instancedShadowPlane = nullptr;
    ShadowArray<INT> InstanceList;
    INT currentIndex = 0;
    FMatrix initMatrix;
    FLOAT zOffset = 10, fixedScale = 1.2f;
    void InitSlots();
    INT AddInstance(FVector, float);
    void ReleaseInstance(INT);
    void Refresh(INT, FVector);
};
''']
    for signature in ['INT AInstancedShadowPlaneActor::AddInstance(',
                      'void AInstancedShadowPlaneActor::ReleaseInstance(',
                      'void AInstancedShadowPlaneActor::Refresh(']:
        parts.append(function(text, signature))
    start = text.index('currentIndex = 0;')
    end = text.index('instancedShadowPlane->Bounds.Origin', start)
    parts.extend(['void AInstancedShadowPlaneActor::InitSlots() {', text[start:end], '}', r'''
void Run() {
    AInstancedShadowPlaneActor actor;
    assert(actor.AddInstance(FVector(), 1) == -1);
    actor.ReleaseInstance(-1); actor.Refresh(-1, FVector());
    UInstancedStaticMeshComponent mesh;
    actor.instancedShadowPlane = &mesh;
    assert(actor.AddInstance(FVector(), 1) == -1);
    actor.InitSlots(); actor.InitSlots();
    assert(actor.InstanceList.Num() == MAX_SHADOW_NUM);
    for (INT index : {-2147483647 - 1, -1, 0, 100, 2147483647}) {
        actor.ReleaseInstance(index); actor.Refresh(index, FVector());
    }
    for (INT i = 0; i < MAX_SHADOW_NUM; ++i) {
        assert(actor.AddInstance(FVector(i, 2, 3), 2) == i);
        auto& data = mesh.PerInstanceSMData(i);
        assert(data.Transform.origin.X == i && data.Transform.origin.Z == 13);
        assert(data.LightmapUVBias.X == -1 && data.ShadowmapUVBias.Y == -1);
    }
    assert(actor.AddInstance(FVector(), 1) == -1);
    for (INT index : {-2147483647 - 1, -1, 100, 2147483647}) {
        actor.ReleaseInstance(index); actor.Refresh(index, FVector());
        assert(actor.currentIndex == MAX_SHADOW_NUM);
    }
    for (INT i = 0; i < MAX_SHADOW_NUM; i += 2) {
        actor.ReleaseInstance(i); INT active = actor.currentIndex;
        actor.ReleaseInstance(i); assert(actor.currentIndex == active);
        actor.Refresh(i, FVector(999, 999, 999));
        assert(mesh.PerInstanceSMData(i).Transform.origin.X == -100000);
    }
    INT count = mesh.PerInstanceSMData.Num();
    for (INT i = MAX_SHADOW_NUM - 2; i >= 0; i -= 2) {
        assert(actor.AddInstance(FVector(i + 1000, 2, 3), 1) == i);
        assert(mesh.PerInstanceSMData.Num() == count);
        assert(mesh.PerInstanceSMData(i).Transform.origin.X == i + 1000);
    }
    for (INT i = 1; i < MAX_SHADOW_NUM; i += 2)
        assert(mesh.PerInstanceSMData(i).Transform.origin.X == i);
    actor.Refresh(7, FVector(1, 2, 3));
    assert(mesh.PerInstanceSMData(7).Transform.origin.Z == 13);
    for (INT i = 0; i < MAX_SHADOW_NUM; ++i) actor.ReleaseInstance(i);
    assert(actor.currentIndex == 0);
    GSystemSettings.dynamic = TRUE;
    assert(actor.AddInstance(FVector(), 1) == -1 && actor.currentIndex == 0);
    GSystemSettings.dynamic = FALSE;
    for (INT i = MAX_SHADOW_NUM - 1; i >= 0; --i) assert(actor.AddInstance(FVector(i, 2, 3), 1) == i);
    assert(mesh.PerInstanceSMData.Num() == MAX_SHADOW_NUM);
    puts("Shadow slots reject invalid indexes and duplicate releases, reuse selected slots and initialize new instance data");
}
}
'''])
    return '\n'.join(parts)
