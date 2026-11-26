"""Production default selection, merge preparation, equipment and mesh snapshots."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(path))

    raw = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = raw.decode("utf-16" if raw.startswith(b"\xff\xfe") else "latin1").replace("\r", "")
    item = clean(game / "Src/ItemManager.cpp")
    parts = [r'''
namespace AppearanceSetupTests {
template<class T> struct TArray : ItemMeshBoundsTests::TArray<T> {
    void Remove(INT index, INT count) { this->values.erase(this->values.begin() + index, this->values.begin() + index + count); }
    void Empty(INT = 0) { this->values.clear(); }
    void InsertItem(const T& value, INT index) { this->values.insert(this->values.begin() + index, value); }
};
template<class... Args> void debugf(Args...) {}
struct FString { const char* text; const char* operator*() const { return text; } };
enum EEventParm { EC_EventParm };
enum { NAME_None = 0, RF_Public = 1, RF_Transactional = 2 };
struct UObject {
    virtual ~UObject() {}
    static UObject* StaticConstructObject(INT, void* = nullptr, INT = 0, INT = 0, UObject* = nullptr);
};
template<class T> T* Cast(UObject* p) { return dynamic_cast<T*>(p); }
struct UTexture2D {};
struct USkeletalMesh {};
struct AMMOActor;
struct UEquipMesh : UObject {
    BYTE MeshType = 0;
    INT Priority = 0, UsedColorTextureID = 0;
    USkeletalMesh* SkeletalMesh = nullptr;
    UTexture2D* RegionTexture = nullptr;
    TArray<UTexture2D*> ColorTextures;
    bool bNeedSkinTexture = false, take_on = true, take_off = true;
    INT ons = 0, offs = 0;
    UEquipMesh* cloned_from = nullptr;
    INT GetClass() const { return 1; }
    FString GetName() const { return {"mesh"}; }
    FString GetFullName() const { return {"mesh"}; }
    bool OnTakeOn(AMMOActor*, UBOOL) { ++ons; return take_on; }
    bool OnTakeOff(AMMOActor*, UBOOL) { ++offs; return take_off; }
};
''', function(generated, "enum EBodyMeshType\n") + ";", r'''
struct FBodyPart {
    USkeletalMesh* Mesh = nullptr;
    TArray<UTexture2D*> Textures, Textures_Base;
    INT OrigX = 0, OrigY = 0, SizeX = 0, SizeY = 0;
    explicit FBodyPart(EEventParm) {}
};
struct UMMOSkeletalMeshMerge : UObject {
    static INT StaticClass() { return 2; }
    TArray<FBodyPart> Parts;
    INT TargetTexSizeX = 0, TargetTexSizeY = 0;
    bool bDoneMerge = false, prepare_ok = true;
    INT cancels = 0, prepares = 0;
    void CancelMerge() { ++cancels; }
    bool PrepareTextures() { ++prepares; return prepare_ok; }
};
bool fail_construct = false;
std::vector<UObject*> allocations;
UObject* UObject::StaticConstructObject(INT type, void*, INT, INT, UObject* source) {
    if (fail_construct) return nullptr;
    UObject* object;
    if (type == 1) {
        auto* mesh = new UEquipMesh(*static_cast<UEquipMesh*>(source));
        mesh->cloned_from = static_cast<UEquipMesh*>(source); object = mesh;
    } else object = new UMMOSkeletalMeshMerge;
    allocations.push_back(object); return object;
}
UObject* StaticConstructObject(INT type, void* owner, INT name, INT flags, UObject* source) {
    return UObject::StaticConstructObject(type, owner, name, flags, source);
}
struct RandomMeshList { BYTE MeshType = 0; TArray<UEquipMesh*> EquipMeshList; };
struct UAvatarData {
    TArray<UEquipMesh*> DefaultMeshListTemplate;
    TArray<RandomMeshList> RandomMeshListTemplate;
    bool MergeShoulder = false;
};
struct UItemManager;
struct Attribute { BYTE ClassType = 0, GenderType = 0; };
struct AMMOActor {
    UItemManager* ItemManager = nullptr;
    Attribute* ActorAttributes = nullptr;
    UBOOL bHidden = FALSE, bMainSkelChanged = FALSE;
    void SetVisibleByMergeMesh(UBOOL) {}
};
struct UItem {
    INT ons = 0, offs = 0;
    UBOOL OnTakeOn(AMMOActor*, UBOOL) { ++ons; return TRUE; }
    UBOOL OnTakeOff(AMMOActor*) { ++offs; return TRUE; }
};
struct UEquip : UItem {
    TArray<UEquipMesh*> MeshList;
    AMMOActor* BaseActor = nullptr;
    void Init(BYTE, BYTE, UBOOL) {}
    UBOOL OnTakeOn(AMMOActor*, UBOOL);
    UBOOL OnTakeOff(AMMOActor*);
};
struct UItemManager {
    AMMOActor* Owner = nullptr;
    bool UseMergedMesh = false, ReplaceFaceByShoulder = false;
    bool bIsPreparing = false, bIsMerging = false, bUseCachedTemplate = false;
    FLOAT MergePrepareTime = 0;
    INT CurrentSkinColorIdx = 0, range_updates = 0;
    TArray<UEquipMesh*> DefaultMeshList, CurrentMeshList, OldMeshList;
    TArray<UTexture2D*> SkinTextures, SkinTexturesBase;
    TArray<UEquip*> EquippedItemList_Normal;
    UMMOSkeletalMeshMerge* MergedMesh = nullptr;
    void SetDefaultMeshes(UAvatarData*, INT);
    void RefreshSkinTexture(INT, UBOOL);
    UBOOL PrepareMerge();
    void UpdateOwnerData(UBOOL);
    bool eventCheckNeedRemerge() { return false; }
    void ApplyEmissiveSilhouetteFactor(FLOAT) {}
    void eventUpdateCurrentAttackRange() { ++range_updates; }
};
using EquipMeshArray = TArray<UEquipMesh*>;
struct WorldInfo { FLOAT EmissiveSilhouetteFactor = 1; };
struct World { WorldInfo info; WorldInfo* GetWorldInfo() { return &info; } } world;
World* GWorld = &world;
UBOOL GIsGame = TRUE, GIsPlayInEditorWorld = FALSE;
struct FMergingHelper {
''', function(item, "UMMOSkeletalMeshMerge* CreateNewMesh()"), r'''
    UMMOSkeletalMeshMerge* FindSameMesh(UMMOSkeletalMeshMerge*) { return nullptr; }
} GMergingHelper;
''']
    for signature in ["void UItemManager::SetDefaultMeshes(", "void UItemManager::RefreshSkinTexture(",
                      "UBOOL UItemManager::PrepareMerge()", "void UItemManager::UpdateOwnerData("]:
        parts.append(function(item, signature))
    equip = clean(game / "Src/Equip.cpp")
    for signature in ["UBOOL UEquip::OnTakeOn(", "UBOOL UEquip::OnTakeOff("]:
        parts.append(function(equip, signature))
    parts.append(r'''
void SelectionTests() {
    UAvatarData avatar;
    UEquipMesh base, first, second, wrong;
    base.MeshType = first.MeshType = second.MeshType = EBMT_Chest; wrong.MeshType = EBMT_Hands;
    UTexture2D a, b; base.ColorTextures.values = first.ColorTextures.values = second.ColorTextures.values = {&a, &b};
    avatar.DefaultMeshListTemplate.values = {nullptr, &base};
    avatar.RandomMeshListTemplate.values.resize(1);
    auto& random = avatar.RandomMeshListTemplate(0);
    random.MeshType = EBMT_Chest;
    for (bool merged : {false, true}) for (INT alternatives : {0, 1, 2}) {
        random.EquipMeshList.values = {&first, &second}; random.EquipMeshList.values.resize(alternatives);
        for (INT key : {-2147483647 - 1, -1, 0, 1, 2, 3, 4, 2147483647}) {
            UItemManager manager; manager.UseMergedMesh = merged;
            manager.SetDefaultMeshes(&avatar, key);
            assert(manager.DefaultMeshList.Num() == 2 && !manager.DefaultMeshList(0));
            UINT selected = UINT(key) % (UINT(alternatives) + 1);
            auto* expected = selected ? random.EquipMeshList(selected - 1) : &base;
            assert(manager.DefaultMeshList(1) && manager.DefaultMeshList(1)->cloned_from == expected);
            if (!merged) assert(manager.SkinTextures.Num() == EBMT_Waist && manager.SkinTextures(EBMT_Chest) == &a);
        }
    }
    UItemManager manager;
    for (UEquipMesh* alternative : {static_cast<UEquipMesh*>(nullptr), &wrong}) {
        random.EquipMeshList.values = {alternative}; manager.SetDefaultMeshes(&avatar, 1);
        assert(manager.DefaultMeshList(1)->cloned_from == &base);
    }
    auto saved = manager.DefaultMeshList.values;
    manager.SetDefaultMeshes(nullptr, -1); assert(manager.DefaultMeshList.values == saved);
    fail_construct = true; manager.SetDefaultMeshes(&avatar, 0);
    assert(!manager.DefaultMeshList(0) && !manager.DefaultMeshList(1)); fail_construct = false;
}
void MergeTests() {
    UEquipMesh meshes[6]; USkeletalMesh skeletal[6]; UTexture2D regions[6], skin, base;
    auto configure = [&](UItemManager& manager) {
        for (INT i = 0; i < 6; ++i) {
            meshes[i].MeshType = BYTE(i); meshes[i].SkeletalMesh = &skeletal[i];
            meshes[i].RegionTexture = &regions[i]; meshes[i].bNeedSkinTexture = false;
        }
        manager.DefaultMeshList.values = {&meshes[0], &meshes[1], &meshes[2], &meshes[3], &meshes[4]};
        manager.SkinTextures.values.assign(5, &skin); manager.SkinTexturesBase.values.assign(5, &base);
    };
    UItemManager manager; configure(manager);
    assert(manager.PrepareMerge() && manager.MergedMesh->Parts.Num() == 6 && manager.MergedMesh->prepares == 1);
    for (INT i = 0; i < 5; ++i) {
        assert(manager.MergedMesh->Parts(i).Mesh == &skeletal[i]);
        assert(manager.MergedMesh->Parts(i).Textures(0) == &regions[i]);
    }
    assert(!manager.MergedMesh->Parts(5).Mesh && !manager.MergedMesh->Parts(5).Textures.Num());
    for (INT slot = 0; slot < 5; ++slot) for (INT count : {0, 1, 4, 5}) {
        configure(manager); meshes[slot].bNeedSkinTexture = true;
        manager.SkinTextures.values.resize(count);
        assert(bool(manager.PrepareMerge()) == (slot < count));
        configure(manager); meshes[slot].bNeedSkinTexture = true;
        manager.SkinTexturesBase.values.resize(count);
        assert(bool(manager.PrepareMerge()) == (slot < count));
    }
    for (INT slot = 0; slot < 6; ++slot) {
        configure(manager); meshes[slot].RegionTexture = nullptr;
        if (slot == 5) manager.DefaultMeshList.AddItem(&meshes[5]);
        assert(bool(manager.PrepareMerge()) == (slot < 5)); // Skirt has no sixth skin slot.
    }
    configure(manager); meshes[5].RegionTexture = nullptr; manager.DefaultMeshList.AddItem(&meshes[5]);
    manager.SkinTextures.AddItem(&skin); manager.SkinTexturesBase.AddItem(&base);
    assert(manager.PrepareMerge() && manager.MergedMesh->Parts(5).Textures(0) == &skin);
    configure(manager);
    UEquip equip; UEquipMesh invalid; invalid.MeshType = 255;
    equip.MeshList.values = {nullptr, &invalid, &meshes[1]};
    manager.EquippedItemList_Normal.values = {nullptr, &equip};
    assert(manager.PrepareMerge());
    fail_construct = true; assert(!manager.PrepareMerge()); fail_construct = false;
}
void EquipTests() {
    UEquip equip;
    UEquipMesh old, valid, invalid; valid.MeshType = EBMT_Chest; invalid.MeshType = 255;
    equip.MeshList.values = {nullptr, &invalid, &valid};
    UItemManager manager; AMMOActor actor; actor.ItemManager = &manager;
    assert(!equip.OnTakeOn(nullptr, FALSE) && !equip.OnTakeOff(nullptr));
    AMMOActor missing; assert(!equip.OnTakeOn(&missing, FALSE) && !equip.OnTakeOff(&missing));
    manager.CurrentMeshList.values.assign(1, &old);
    assert(equip.OnTakeOn(&actor, FALSE) && equip.OnTakeOff(&actor));
    assert(!valid.ons && !valid.offs && !invalid.ons && !invalid.offs && manager.CurrentMeshList(0) == &old);
    manager.CurrentMeshList.values.assign(EBMT_None, nullptr); manager.CurrentMeshList(EBMT_Chest) = &old;
    assert(equip.OnTakeOn(&actor, FALSE));
    assert(manager.CurrentMeshList(EBMT_Chest) == &valid && valid.ons == 1 && old.offs == 1);
    assert(equip.OnTakeOff(&actor) && !manager.CurrentMeshList(EBMT_Chest) && valid.offs == 1);
    valid.take_on = false; assert(equip.OnTakeOn(&actor, FALSE) && !manager.CurrentMeshList(EBMT_Chest));
}
void SnapshotTests() {
    UEquipMesh valid, invalid; valid.MeshType = 0; invalid.MeshType = 255;
    for (INT current : {0, 1, 5, 15}) for (INT old : {0, 1, 6, 20}) {
        UItemManager manager; AMMOActor actor; actor.ItemManager = &manager; manager.Owner = &actor;
        manager.CurrentMeshList.values.assign(current, nullptr); manager.OldMeshList.values.assign(old, &invalid);
        manager.DefaultMeshList.values = {nullptr, &invalid, &valid};
        manager.UpdateOwnerData(FALSE);
        assert(manager.OldMeshList.Num() == EBMT_NoMerge);
        for (INT i = 0; i < EBMT_NoMerge; ++i)
            assert(manager.OldMeshList(i) == (i < current ? manager.CurrentMeshList(i) : nullptr));
        assert(manager.range_updates == 1);
    }
}
void Run() {
    SelectionTests(); MergeTests(); EquipTests(); SnapshotTests();
    for (auto* object : allocations) delete object;
    allocations.clear();
    puts("Appearance setup bounds random/default selection, skin fallbacks, equipment changes and mesh snapshots");
}
}
''')
    return "\n".join(parts)
