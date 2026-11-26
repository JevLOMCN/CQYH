"""Actual crowd particle resize/copy helper and both renderer initialization paths."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / path))

    parts = [r'''
#pragma push_macro("CTRL_TICK")
#undef CTRL_TICK
#define CTRL_TICK(value) ((void)0)
namespace CrowdParticleTests {
using FVector = PositionDefaultTests::FVector;
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    bool IsValidIndex(INT index) const { return index >= 0 && index < Num(); }
    T& operator()(INT index) { return values.at(index); }
    const T& operator()(INT index) const { return values.at(index); }
    void SetNum(INT count) { assert(count >= 0); values.resize(count); }
};
struct UObject { virtual ~UObject() {} };
template<class T, class U> T* Cast(U* object) { return dynamic_cast<T*>(object); }
struct UMMOCrowdInstance { FVector Location{1, 2, 3}, Direction{4, 5, 6}; };
struct AMMOCrowdSystem : UObject { TArray<UMMOCrowdInstance*> CrowdInstanceData; };
struct UParticleModule : UObject {};
struct UParticleModuleLocationCrowdSystem : UParticleModule { TArray<FVector> ParticleLocations, ParticleRotations; };
struct LODLevel { TArray<UParticleModule*> Modules; };
struct UParticleSpriteEmitter { TArray<LODLevel*> LODLevels; };
struct FParticleEmitterInstance { UParticleSpriteEmitter* SpriteTemplate = nullptr; };
struct ParticleComponent {
    TArray<FParticleEmitterInstance*> EmitterInstances;
    UObject* owner = nullptr;
    void* Template = reinterpret_cast<void*>(1);
    INT attaches = 0, ticks = 0;
    void Attach() { ++attaches; }
    void Tick(FLOAT) { ++ticks; }
    UObject* GetOwner() { return owner; }
};
struct UMMOCrowdRenderComponentSM : ParticleComponent {
    using Super = ParticleComponent;
    UParticleModuleLocationCrowdSystem* PSModule = nullptr;
    AMMOCrowdSystem* OwnerSystem = nullptr;
    void Attach();
    void Tick(FLOAT);
    void Init(const AMMOCrowdSystem*);
};
struct AMMOCrowdRendererSM {
    ParticleComponent* ParticleSystemComponent = nullptr;
    AMMOCrowdSystem* CrowdSystem = nullptr;
    UParticleModuleLocationCrowdSystem* PSModule = nullptr;
    void Init();
};
''', function(clean("Src/CrowdParticleData.h"), "inline void UpdateCrowdParticleData(")]
    cpp = clean("Src/UnMMOCrowdRenderComponentSM.cpp")
    for signature in ["void UMMOCrowdRenderComponentSM::Attach()", "void UMMOCrowdRenderComponentSM::Tick(",
                      "void UMMOCrowdRenderComponentSM::Init("]:
        parts.append(function(cpp, signature))
    parts.append(function(clean("Src/AMMOCrowdRendererSM.cpp"), "void AMMOCrowdRendererSM::Init()"))
    parts.append(r'''
void Equal(FVector value, FLOAT x, FLOAT y, FLOAT z) { assert(value.X == x && value.Y == y && value.Z == z); }
void CheckData(const UParticleModuleLocationCrowdSystem& module, const AMMOCrowdSystem& system) {
    assert(module.ParticleLocations.Num() == system.CrowdInstanceData.Num());
    assert(module.ParticleRotations.Num() == system.CrowdInstanceData.Num());
    for (INT i = 0; i < system.CrowdInstanceData.Num(); ++i) {
        auto* instance = system.CrowdInstanceData(i);
        FVector pos = instance ? instance->Location : FVector(0, 0, 0);
        FVector dir = instance ? instance->Direction : FVector(0, 0, 0);
        Equal(module.ParticleLocations(i), pos.X, pos.Y, pos.Z);
        Equal(module.ParticleRotations(i), dir.X, dir.Y, dir.Z);
    }
}
void Run() {
    UMMOCrowdRenderComponentSM component;
    AMMOCrowdRendererSM renderer;
    AMMOCrowdSystem system;
    UMMOCrowdInstance instance;
    system.CrowdInstanceData.values = {&instance, nullptr, &instance};
    UParticleModuleLocationCrowdSystem stale, module;
    FParticleEmitterInstance emitter;
    UParticleSpriteEmitter sprite;
    LODLevel lod;
    UParticleModule unrelated;
    auto check_missing = [&] {
        component.PSModule = &stale; component.Init(&system); assert(!component.PSModule);
        renderer.PSModule = &stale; renderer.Init(); assert(!renderer.PSModule);
    };
    check_missing(); // No component in the actor, no emitter in the component.
    renderer.ParticleSystemComponent = &component; renderer.CrowdSystem = &system;
    component.EmitterInstances.values = {nullptr}; check_missing();
    component.EmitterInstances.values = {&emitter}; check_missing();
    emitter.SpriteTemplate = &sprite; check_missing();
    sprite.LODLevels.values = {nullptr}; check_missing();
    sprite.LODLevels.values = {&lod}; check_missing();
    lod.Modules.values = {nullptr, &unrelated}; check_missing();
    lod.Modules.values.push_back(&module);
    component.Init(nullptr); assert(!component.PSModule);
    component.Init(&system); assert(component.PSModule == &module); CheckData(module, system);
    renderer.Init(); assert(renderer.PSModule == &module); CheckData(module, system);
    // Every newly resized vector starts with NaNs in the mock. All positions
    // and rotations must be written before the particle system can consume them.
    for (INT count : {0, 1, 3, 1000, 2, 0}) {
        system.CrowdInstanceData.values.assign(count, &instance);
        if (count > 1) system.CrowdInstanceData(1) = nullptr;
        module.ParticleLocations.SetNum(count + 1); module.ParticleRotations.SetNum(count + 3);
        component.OwnerSystem = &system; component.Tick(0.1f); CheckData(module, system);
    }
    component.OwnerSystem = nullptr; component.Tick(0.1f);
    assert(!module.ParticleLocations.Num() && !module.ParticleRotations.Num());
    component.PSModule = &stale; component.owner = nullptr; component.Attach();
    assert(!component.PSModule && !component.OwnerSystem);
    UObject unrelated_owner; component.PSModule = &stale; component.owner = &unrelated_owner; component.Attach();
    assert(!component.PSModule && !component.OwnerSystem);
    component.PSModule = &stale; component.owner = &system; component.Template = nullptr; component.Attach();
    assert(!component.PSModule && component.OwnerSystem == &system);
    component.Template = reinterpret_cast<void*>(1); component.Attach();
    assert(component.PSModule == &module && component.OwnerSystem == &system); CheckData(module, system);
    assert(component.attaches == 4 && component.ticks == 7);
    system.CrowdInstanceData.values = {&instance};
    renderer.CrowdSystem = nullptr; renderer.Init();
    assert(renderer.PSModule == &module && !module.ParticleLocations.Num() && !module.ParticleRotations.Num());
    UpdateCrowdParticleData(nullptr, &system);
    puts("Crowd particle arrays resize and fully initialize across growth/shrink/null entries; both renderers reject missing emitter data and stale modules");
}
}
#pragma pop_macro("CTRL_TICK")
''')
    script = function(clean("Classes/MMOCrowdRendererSM.uc"), "event Tick(")
    assert "if (PSModule == none)" in script and "if (CrowdSystem == none)" in script
    for array in ["ParticleLocations", "ParticleRotations"]:
        resize = "PSModule." + array + ".Length = CrowdSystem.CrowdInstanceData.Length;"
        assert script.index(resize) < script.index("PSModule." + array + "[i]")
        assert "PSModule." + array + "[i] = vect(0, 0, 0);" in script
    return "\n".join(parts)
