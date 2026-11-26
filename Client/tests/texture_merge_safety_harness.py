"""Actual equipment material replacement and texture merge initialization/bounds."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(path))

    parts = [r'''
namespace TextureMergeSafetyTests {
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    bool IsValidIndex(INT n) const { return n >= 0 && n < Num(); }
    T& operator()(INT n) { assert(IsValidIndex(n)); return values[n]; }
    const T& operator()(INT n) const { assert(IsValidIndex(n)); return values[n]; }
    INT AddItem(const T& value) { values.push_back(value); return Num() - 1; }
    void Empty() { values.clear(); }
};
using DWORD = unsigned long;
using FTexture2DRHIParamRef = void*;
struct UTexture2D { bool loaded = true; bool IsFullyStreamedIn() const { return loaded; } };
struct UTexture2DDynamic : UTexture2D {};
struct FTexture2DResource {};
struct FEventWin {
    bool created = false;
    INT triggers = 0, waits = 0;
    void Create() { created = true; }
    void Trigger() { ++triggers; }
    bool Wait(INT) { ++waits; return false; }
};
''']
    header = clean(game.parent / "Engine/Inc/UnTextureMerge.h")
    parts.append(function(header, "struct FTextureMergeRegion\n") + ";")
    parts.append("using TextureMergeList = TArray<FTextureMergeRegion>;")
    parts.append(function(header, "struct FTextureLockData\n") + ";")
    parts.append("using TextureLockDataList = TArray<FTextureLockData*>;")
    definition = function(header, "class FTextureMergeUtil\n")
    parts.append(definition.replace("private:", "public:") + ";")
    parts.append(r'''
TextureLockDataList FTextureMergeUtil::LockData_D, FTextureMergeUtil::LockData_N, FTextureMergeUtil::LockData_S;
UINT FTextureMergeUtil::CurrentRunningThreadNum = 0;
INT starts = 0, merge_calls = 0;
void FTextureMergeUtil::StartMergingThread() { ++starts; }
void FTextureMergeUtil::CheckLockDataRefNum(FTextureLockData*&, INT&, TextureLockDataList&) { assert(false); }
UBOOL FTextureMergeUtil::DoMerge() { ++merge_calls; return TRUE; }
#pragma push_macro("ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER")
#undef ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
#define ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(Name, Type, Var, Value, Code) do { assert(false); } while (0)
''')
    engine = clean(game.parent / "Engine/Src/UnTextureMerge.cpp")
    for signature in ["FTextureMergeUtil::FTextureMergeUtil()", "FTextureMergeUtil::~FTextureMergeUtil()",
                      "void FTextureMergeUtil::CancelCurrentMerge()", "void FTextureMergeUtil::UpdateState()"]:
        parts.append(function(engine, signature))
    parts.append(r'''
#pragma pop_macro("ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER")
struct Part {
    INT SizeX = 32, SizeY = 64, OrigX = 0, OrigY = 0;
    TArray<UTexture2D*> Textures, Textures_Base, Textures_N, Textures_S;
};
struct UMMOSkeletalMeshMerge {
    TArray<Part> Parts;
    FTextureMergeUtil* TexMergeUtil = nullptr;
    UTexture2DDynamic* MergedTex_D = nullptr;
    UTexture2DDynamic* MergedTex_N = nullptr;
    UTexture2DDynamic* MergedTex_S = nullptr;
    UBOOL MergeTextures();
    UBOOL IsSourceTexturesFullyLoaded();
    UBOOL PreparationBounds();
};
''')
    mesh = clean(game / "Src/MMOSkeletalMeshMerge.cpp")
    for signature in ["UBOOL UMMOSkeletalMeshMerge::MergeTextures()",
                      "UBOOL UMMOSkeletalMeshMerge::IsSourceTexturesFullyLoaded()"]:
        parts.append(function(mesh, signature))
    prepare = function(mesh, "UBOOL UMMOSkeletalMeshMerge::PrepareTextures()")
    # Compile the actual preflight before texture loading/streaming, whose engine
    # resource calls are outside this regression's scope.
    preflight = prepare[prepare.index("{") + 1:prepare.index("UTexture2D* tex;")]
    parts.extend(["UBOOL UMMOSkeletalMeshMerge::PreparationBounds() {", preflight, "return TRUE; }"])
    parts.append(r'''
void Run() {
    for (BYTE pattern : {BYTE(0xff), BYTE(0xa5)}) {
        alignas(FTextureMergeUtil) BYTE bytes[sizeof(FTextureMergeUtil)];
        memset(bytes, pattern, sizeof(bytes));
        auto* utility = new(bytes) FTextureMergeUtil;
''')
    fields = re.findall(r'^\s*(UBOOL|INT|UINT|DWORD|BYTE\*|UTexture2DDynamic\*|FTextureMergeRegion\*|FTexture2DRHIParamRef|FTexture2DResource\*|FTextureLockData\*)\s+(\w+)\s*;',
                        definition, re.M)
    assert len(fields) == 51
    parts.extend("        assert(!utility->" + name + ");" for _, name in fields)
    parts.append(r'''
        assert(utility->MergeFinishEvent.created);
        assert(!utility->MergeFinished() && !utility->IsDoingMerge());
        utility->CancelCurrentMerge();
        utility->UpdateState();
        assert(!starts && !utility->MergeFinishEvent.triggers && utility->MergeFinishEvent.waits == 1);
        utility->~FTextureMergeUtil();
    }
    UTexture2D diffuse, normal, specular;
    UTexture2DDynamic target_d, target_n, target_s;
    for (INT count : {0, 1, 2, 32}) {
        UMMOSkeletalMeshMerge mesh;
        mesh.Parts.values.resize(2);
        for (auto& part : mesh.Parts.values) {
            part.Textures.values.assign(count, &diffuse);
            part.Textures_Base.values.assign(count, &diffuse);
            part.Textures_N.values.assign(count, &normal);
            part.Textures_S.values.assign(count, &specular);
        }
        assert(mesh.PreparationBounds());
        assert(mesh.IsSourceTexturesFullyLoaded());
        mesh.MergedTex_D = &target_d; mesh.MergedTex_N = &target_n; mesh.MergedTex_S = &target_s;
        INT calls = merge_calls;
        assert(mesh.MergeTextures() && merge_calls == calls + 1 && mesh.TexMergeUtil);
        auto* utility = mesh.TexMergeUtil;
        assert(utility->TargetTexture_D == &target_d && utility->TargetTexture_N == &target_n && utility->TargetTexture_S == &target_s);
        assert(utility->SourceRegions_D.Num() == 2 && utility->SourceRegions_N.Num() == 2 && utility->SourceRegions_S.Num() == 2);
        assert(utility->SourceRegions_D(1).Textures.Num() == count);
        assert(utility->SourceRegions_D(1).SizeX == 32 && utility->SourceRegions_D(1).SizeY == 64);
        auto& part = mesh.Parts(1);
        part.Textures_Base.AddItem(&diffuse);
        assert(!mesh.PreparationBounds());
        part.Textures_Base.values.pop_back();
        for (auto* list : {&part.Textures_N, &part.Textures_S}) {
            auto saved = list->values;
            list->AddItem(&normal);
            calls = merge_calls;
            assert(!mesh.MergeTextures() && merge_calls == calls && mesh.TexMergeUtil == utility);
            list->values = saved;
            if (count) {
                list->values.pop_back();
                assert(!mesh.MergeTextures() && merge_calls == calls);
                list->values = saved;
            }
        }
        if (count) {
            for (auto* list : {&part.Textures, &part.Textures_N, &part.Textures_S}) {
                UTexture2D* saved = (*list)(0);
                (*list)(0) = nullptr;
                assert(!mesh.IsSourceTexturesFullyLoaded() && !mesh.MergeTextures());
                (*list)(0) = saved;
                saved->loaded = false;
                assert(!mesh.IsSourceTexturesFullyLoaded());
                saved->loaded = true;
            }
            part.Textures_Base.values.pop_back();
            assert(!mesh.PreparationBounds());
        }
        assert(mesh.MergeTextures());
        delete utility;
    }
    puts("Texture merge initializes all 51 native fields and rejects mismatched/null texture arrays before indexing");
}
}
namespace MaterialTextureSafetyTests {
template<class T> using TArray = TextureMergeSafetyTests::TArray<T>;
struct FString {
    const char* text;
    const char* operator*() const { return text; }
};
enum { LOAD_NoWarn, ClassMaterial, ClassMesh, ClassArmor };
struct UObject {
    virtual ~UObject() {}
    virtual bool IsA(INT) const { return false; }
};
template<class T> T* Cast(UObject* p) { return dynamic_cast<T*>(p); }
struct UTexture : UObject {};
struct UMaterialInterface : UObject {};
struct UMaterial : UMaterialInterface { static INT StaticClass() { return ClassMaterial; } };
struct UMaterialInstance : UMaterialInterface {
    std::map<std::string, UTexture*> available, assigned;
    UMaterialInterface* parent = nullptr;
    INT parents = 0;
    bool GetTextureParameterValue(const char* name, UTexture*& out) {
        auto found = available.find(name);
        if (found == available.end()) return false; // Leave out untouched, as the engine can.
        out = found->second; return true;
    }
    void SetParent(UMaterialInterface* p) { parent = p; ++parents; }
    void SetTextureParameterValue(const char* name, UTexture* texture) { assigned[name] = texture; }
};
struct UMeshComponent : UObject {
    TArray<UMaterialInterface*> Materials;
    static INT StaticClass() { return ClassMesh; }
    bool IsA(INT c) const override { return c == ClassMesh; }
};
struct AArmorActor : UObject {
    UMeshComponent* MainMesh = nullptr;
    static INT StaticClass() { return ClassArmor; }
    bool IsA(INT c) const override { return c == ClassArmor; }
};
struct UEquipMesh : UObject { BYTE MeshType = 0; };
struct OwnerActor { UObject* equip = nullptr; UObject* GetEquip(BYTE) { return equip; } };
UObject* loaded_material = nullptr;
UObject* StaticLoadObject(INT, void*, const char*, void*, INT, void*) { return loaded_material; }
struct UItemManager {
    OwnerActor* Owner = nullptr;
    TArray<UObject*> CurrentMeshList;
    void ChangeEquipMeshMaterial(BYTE, const FString&);
};
''')
    parts.append(function(clean(game / "Src/ItemManager.cpp"), "void UItemManager::ChangeEquipMeshMaterial("))
    parts.append(r'''
void Run() {
    UItemManager manager;
    OwnerActor owner;
    UEquipMesh equip;
    UMeshComponent mesh;
    AArmorActor armor; armor.MainMesh = &mesh;
    UTexture a, b, c;
    UMaterial material;
    UMaterialInstance full, partial, missing;
    full.available = {{"Diffuse_Map", &a}, {"Normal_Map", &b}, {"Specular_Map", &c}};
    partial.available = {{"Normal_Map", &b}};
    mesh.Materials.values = {nullptr, &material, &full, &partial, &missing};
    owner.equip = &mesh;
    manager.CurrentMeshList.values = {&equip};
    manager.Owner = &owner;
    FString path{"test"};
    manager.ChangeEquipMeshMaterial(0, path); // Missing replacement material.
    assert(!full.parents);
    loaded_material = &material;
    manager.Owner = nullptr;
    manager.ChangeEquipMeshMaterial(0, path);
    manager.Owner = &owner;
    manager.ChangeEquipMeshMaterial(1, path); manager.ChangeEquipMeshMaterial(255, path);
    assert(!full.parents);
    for (UObject* object : {static_cast<UObject*>(&mesh), static_cast<UObject*>(&armor)}) {
        owner.equip = object;
        full.assigned.clear(); partial.assigned.clear(); missing.assigned.clear();
        manager.ChangeEquipMeshMaterial(0, path);
        assert(full.parent == &material && partial.parent == &material && missing.parent == &material);
        assert(full.assigned == full.available);
        assert(partial.assigned == partial.available);
        assert(missing.assigned.empty()); // No uninitialized or previous-slot pointers copied.
    }
    full.available["Diffuse_Map"] = nullptr; // A successful null result remains valid.
    owner.equip = &mesh;
    manager.ChangeEquipMeshMaterial(0, path);
    assert(full.assigned.count("Diffuse_Map") && full.assigned["Diffuse_Map"] == nullptr);
    puts("Material replacement preserves only successful per-slot texture queries and handles invalid slots/material types");
}
}
''')
    return "\n".join(parts)
