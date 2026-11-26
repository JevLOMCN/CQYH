"""Actual inventory writes, skin/clone indexes and reputation record/widget defaults."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(path))

    item = clean(game / "Src/ItemManager.cpp")
    parts = [r'''
namespace ItemMeshBoundsTests {
template<class T> struct TArray : TextureMergeSafetyTests::TArray<T> {
    void AddZeroed(INT count) { this->values.resize(this->values.size() + count, T()); }
};
enum {
    E_CONSLOT_DEFAULT = InventoryLookupTests::E_CONSLOT_DEFAULT,
    E_CONSLOT_BANK = InventoryLookupTests::E_CONSLOT_BANK,
    E_CONSLOT_HERO_EQUIPMENT = InventoryLookupTests::E_CONSLOT_HERO_EQUIPMENT,
    E_CONSLOT_HERO_EQUIPPED = InventoryLookupTests::E_CONSLOT_HERO_EQUIPPED,
    E_CONSLOT_DEPOSIT = InventoryLookupTests::E_CONSLOT_DEPOSIT,
    E_CONSLOT_RESOURCE = InventoryLookupTests::E_CONSLOT_RESOURCE,
    EBMT_Waist = 5
};
template<class... Args> void debugf(Args...) {}
struct UItem { INT id; };
struct UTexture { INT id; };
struct UEquipMesh {
    BYTE MeshType = 0;
    INT UsedColorTextureID = 0;
    TArray<UTexture*> ColorTextures;
};
struct UItemManager {
    TArray<UItem*> BagItemList, BagBankList, BagHeroList, BagDepositList, BagResourceList;
    TArray<UEquipMesh*> DefaultMeshList;
    TArray<UTexture*> SkinTextures, SkinTexturesBase;
    INT CurrentSkinColorIdx = 0;
    int RequestExchangeItem(INT, INT, INT, INT);
    void RefreshSkinTexture(INT, UBOOL);
};
''']
    for signature in ["int UItemManager::RequestExchangeItem(", "void UItemManager::RefreshSkinTexture("]:
        parts.append(function(item, signature))
    parts.append(r'''
void Run() {
    UItem a{1}, b{2};
    UItemManager manager;
    TArray<UItem*>* bags[] = {&manager.BagItemList, &manager.BagBankList, &manager.BagHeroList,
                              &manager.BagDepositList, &manager.BagResourceList};
    const INT ids[] = {E_CONSLOT_DEFAULT, E_CONSLOT_BANK, E_CONSLOT_HERO_EQUIPMENT, E_CONSLOT_DEPOSIT, E_CONSLOT_RESOURCE};
    for (INT src = 0; src < 5; ++src) for (INT dst = 0; dst < 5; ++dst) {
        for (auto* bag : bags) bag->values = {&a, nullptr, &b};
        for (INT index : {-2147483647 - 1, -1, 3, 2147483647}) {
            assert(!manager.RequestExchangeItem(ids[src], index, ids[dst], 0));
            assert(!manager.RequestExchangeItem(ids[src], 0, ids[dst], index));
            for (auto* bag : bags) assert(bag->values == std::vector<UItem*>({&a, nullptr, &b}));
        }
        assert(manager.RequestExchangeItem(ids[src], 0, ids[dst], 2));
        assert((*bags[src])(0) == &b && (*bags[dst])(2) == &a);
        for (auto* bag : bags) bag->values = {&a, nullptr, &b};
        assert(manager.RequestExchangeItem(ids[src], 0, ids[dst], 1));
        assert((*bags[src])(0) == nullptr && (*bags[dst])(1) == &a);
    }
    assert(!manager.RequestExchangeItem(-1, 0, E_CONSLOT_DEFAULT, 0));
    assert(!manager.RequestExchangeItem(E_CONSLOT_DEFAULT, 0, E_CONSLOT_HERO_EQUIPPED, 0));
    for (auto* bag : bags) bag->values.clear();
    assert(!manager.RequestExchangeItem(E_CONSLOT_DEFAULT, 0, E_CONSLOT_BANK, 0));

    UTexture skin{1}, base{2}, previous{3};
    UEquipMesh mesh;
    mesh.ColorTextures.values = {&skin, &base}; mesh.UsedColorTextureID = 1;
    manager.DefaultMeshList.values = {nullptr, &mesh};
    for (INT skins : {0, 1, 5, 8}) for (INT bases : {0, 1, 5, 8})
    for (INT type : {0, 4, 5, 255}) for (bool color_only : {false, true}) {
        manager.SkinTextures.values.assign(skins, &previous);
        manager.SkinTexturesBase.values.assign(bases, &previous);
        mesh.MeshType = BYTE(type);
        manager.RefreshSkinTexture(1, color_only);
        bool valid = type < EBMT_Waist && type < skins && (color_only || type < bases);
        for (INT i = 0; i < skins; ++i)
            assert(manager.SkinTextures(i) == (valid && i == type ? &skin : &previous));
        for (INT i = 0; i < bases; ++i)
            assert(manager.SkinTexturesBase(i) == (valid && !color_only && i == type ? &base : &previous));
    }
    manager.SkinTextures.values = {&previous}; manager.SkinTexturesBase.values = {&previous}; mesh.MeshType = 0;
    for (INT index : {-2147483647 - 1, -1, 0, 2, 2147483647}) {
        manager.RefreshSkinTexture(index, FALSE);
        assert(manager.SkinTextures(0) == &previous && manager.SkinTexturesBase(0) == &previous);
    }
    manager.CurrentSkinColorIdx = 123; mesh.UsedColorTextureID = -1;
    manager.RefreshSkinTexture(1, FALSE);
    assert(manager.SkinTextures(0) == &skin && !manager.SkinTexturesBase(0));
    mesh.ColorTextures.values.clear(); manager.RefreshSkinTexture(1, FALSE);
    assert(!manager.SkinTextures(0) && !manager.SkinTexturesBase(0));
    puts("Inventory exchanges validate both indexes before mutation; skin refresh uses actual array bounds");
}
}
namespace RenderCloneBoundsTests {
template<class T> using TArray = ItemMeshBoundsTests::TArray<T>;
enum { EBMT_NoMerge = 6, EBMT_None = 10, NAME_None = 0, RF_Transient = 1 };
struct UObject { virtual ~UObject() {} };
template<class T> T* Cast(UObject* object) { return dynamic_cast<T*>(object); }
struct USkeletalMesh {};
struct UEquipMesh : UObject { static INT StaticClass() { return 2; } };
struct USkeletalMeshComponent : UObject {
    USkeletalMesh* SkeletalMesh = nullptr;
    static INT StaticClass() { return 1; }
    void SetSkeletalMesh(USkeletalMesh* mesh) { SkeletalMesh = mesh; }
};
void* GetTransientPackage() { return nullptr; }
std::vector<UObject*> allocations;
bool fail_allocation = false;
UObject* StaticConstructObject(INT type, void*, INT, INT, UObject* = nullptr) {
    if (fail_allocation) return nullptr;
    UObject* object = type == 1 ? static_cast<UObject*>(new USkeletalMeshComponent) : static_cast<UObject*>(new UEquipMesh);
    allocations.push_back(object); return object;
}
struct UItemManager { TArray<UEquipMesh*> CurrentMeshList; };
struct AMMOActor { USkeletalMeshComponent* Mesh = nullptr; UItemManager* ItemManager = nullptr; };
struct AMMORenderOnlyActor {
    USkeletalMeshComponent* MainMesh = nullptr;
    TArray<UEquipMesh*> EquipMeshList;
    INT merges = 0;
    std::vector<UEquipMesh*> taken;
    void MergeEquipMeshes() { ++merges; }
    void TakeOn(UEquipMesh* mesh) { taken.push_back(mesh); }
    void Clone(AMMOActor*);
    void AddEquipMeshByTemplate(UEquipMesh*, INT);
};
''')
    renderer = clean(game / "Src/MMORenderOnlyActor.cpp")
    for signature in ["void AMMORenderOnlyActor::Clone(", "void AMMORenderOnlyActor::AddEquipMeshByTemplate("]:
        parts.append(function(renderer, signature))
    parts.append(r'''
void Run() {
    USkeletalMesh skeletal;
    USkeletalMeshComponent component; component.SkeletalMesh = &skeletal;
    UEquipMesh equip;
    UItemManager items;
    AMMOActor source{&component, &items};
    for (INT before : {0, 1, 3, 9, 10, 14}) for (INT count : {0, 1, 3, 9, 10, 15}) {
        items.CurrentMeshList.values.assign(count, &equip);
        AMMORenderOnlyActor target;
        target.EquipMeshList.values.assign(before, &equip);
        target.Clone(&source);
        assert(target.MainMesh && target.MainMesh->SkeletalMesh == &skeletal);
        assert(target.EquipMeshList.Num() == EBMT_None && target.merges == (count > 0));
        for (INT i = 0; i < EBMT_None; ++i) assert(target.EquipMeshList(i) == (i < count ? &equip : nullptr));
        assert(target.taken.size() == size_t(EBMT_None - EBMT_NoMerge - 1));
        for (INT i = EBMT_NoMerge + 1; i < EBMT_None; ++i)
            assert(target.taken[i - EBMT_NoMerge - 1] == target.EquipMeshList(i));
    }
    AMMORenderOnlyActor target;
    target.EquipMeshList.values.assign(1, &equip);
    auto initial = allocations.size();
    for (INT index : {-2147483647 - 1, -1, INT(EBMT_None), 2147483647}) target.AddEquipMeshByTemplate(&equip, index);
    assert(allocations.size() == initial && target.EquipMeshList(0) == &equip);
    target.AddEquipMeshByTemplate(&equip, 0);
    assert(allocations.size() == initial + 1 && target.EquipMeshList(0) != &equip && target.EquipMeshList.Num() == EBMT_None);
    for (INT i = 1; i < EBMT_None; ++i) assert(!target.EquipMeshList(i));
    for (INT before : {0, 1, 5}) {
        AMMORenderOnlyActor short_target;
        short_target.EquipMeshList.values.assign(before, &equip);
        short_target.AddEquipMeshByTemplate(&equip, EBMT_None - 1);
        assert(short_target.EquipMeshList.Num() == EBMT_None && short_target.EquipMeshList(EBMT_None - 1));
        for (INT i = 0; i < EBMT_None - 1; ++i) assert(short_target.EquipMeshList(i) == (i < before ? &equip : nullptr));
    }
    source.Mesh = nullptr; target.Clone(&source); target.Clone(nullptr);
    assert(!target.MainMesh && target.EquipMeshList.Num() == EBMT_None);
    source.Mesh = &component; fail_allocation = true; target.Clone(&source);
    assert(!target.MainMesh && target.EquipMeshList.Num() == EBMT_None);
    fail_allocation = false;
    for (auto* object : allocations) delete object;
    allocations.clear();
    puts("Rendering clones initialize all destination slots and handle short lists and invalid template indexes");
}
}
#pragma push_macro("SCRIPT_ALIGN")
#undef SCRIPT_ALIGN
#define SCRIPT_ALIGN
namespace ReputationRecordTests {
using BITFIELD = UINT;
enum EEventParm { EC_EventParm };
struct FString {
    const char* text;
    FString() : text(nullptr) {}
    FString(const char* value) : text(value) {}
    static FString Printf(const char* format, INT index) {
        static char buffer[64]; snprintf(buffer, sizeof(buffer), format, index); return FString(buffer);
    }
};
template<class T> struct TArray : ItemMeshBoundsTests::TArray<T> {
    void Push(const T& value) { this->AddItem(value); }
};
''')
    generated = source(game / "Inc/MMOGameUIClasses.h")
    for name in ["FReputationData", "FReputationData2"]:
        parts.append(function(generated, "struct " + name + "\n") + ";")
    parts.append(r'''
enum { MAX_REPUTATION_COUNT = 4, ESPType_Invalid = 0, EPCT_PlayerClassG = 6, EPCT_PlayerClassH = 7 };
struct UItem { INT id = 7; };
struct UGfxObject {
    std::map<std::string, INT> fields;
    INT GetInt(const char* name) { return fields[name]; }
    void SetInt(const char* name, INT value) { fields[name] = value; }
};
using UGFxObject = UGfxObject;
struct ReputationTemp {
    INT ListLevel = 0, ForceID = 9, ForceVisible = 1, InitValue = 10, MaxValue = 20;
    FString ForceName{"name"}, ForceTips{"tips"}, ForceInfo{"info"};
};
struct AMMOPlayer { INT id = 0; INT GetClassId() const { return id; } };
struct MMOFacade {
    ReputationTemp* templates[MAX_REPUTATION_COUNT] = {};
    AMMOPlayer player;
    UItem item;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    ReputationTemp* GetReputationTemp(INT i) { return templates[i]; }
    AMMOPlayer* GetMainPlayer() { return &player; }
    UItem* FindItemTemplate(INT id) { return id > 0 ? &item : nullptr; }
};
struct UMMOUICharacter {
    TArray<FReputationData> ReputationList;
    void InitReputationData();
};
struct UMMOUIReputationPanel {
    TArray<FReputationData2> ReputationList;
    TArray<UGFxObject*> ItemSlot;
    UGFxObject* widgets[10] = {};
    struct Assignment { UGFxObject* object; INT slot, count; UItem* item; };
    std::vector<Assignment> assignments;
    UGFxObject* GetObject(const FString& name) { INT index = -1; sscanf(name.text, "mc_slot%d", &index); return widgets[index]; }
    void SetItemBarData(UGFxObject* object, UItem* item, INT slot, INT count) {
        assert(object); assignments.push_back({object, slot, count, item});
    }
    void InitReputationData();
    void UpdateRewordSlot(UGFxObject*);
};
''')
    character = clean(game / "Src/MMOUICharactor.cpp")
    panel = clean(game / "Src/MMOUIReputationPanel.cpp")
    parts.append(function(character, "void UMMOUICharacter::InitReputationData()"))
    parts.append(function(panel, "void UMMOUIReputationPanel::InitReputationData()"))
    parts.append(function(panel, "void UMMOUIReputationPanel::UpdateRewordSlot("))
    parts.append("void PoisonTests() {")
    for name, text in [("FReputationData", character), ("FReputationData2", panel)]:
        match = re.search(r"\b" + name + r" data(\([^;]*\))?\s*;", text)
        assert match
        parts.extend(["{", "alignas(" + name + ") BYTE bytes[sizeof(" + name + ")];",
                      "memset(bytes, 0xff, sizeof(bytes));",
                      "auto* value = new(bytes) " + name + (match.group(1) or "") + ";",
                      "assert(!value->TempIndex && !value->type && !value->ForceID && !value->ForceLevel);",
                      "assert(!value->InitValue && !value->MaxValue && !value->Visible && !value->awardState);",
                      "assert(!value->ForceName.text && !value->ForceTips.text && !value->ForceInfo.text);", "}"])
    parts.append(r'''
}
void Run() {
    PoisonTests();
    auto& facade = MMOFacade::getSingleton();
    ReputationTemp one, two; two.ForceID = 11; two.ListLevel = 1;
    facade.templates[0] = &one; facade.templates[2] = &two;
    UMMOUICharacter character;
    character.InitReputationData();
    assert(character.ReputationList.Num() == 2);
    assert(character.ReputationList(0).ForceID == 9 && character.ReputationList(1).ForceID == 11);
    for (auto& value : character.ReputationList.values) assert(!value.awardState && value.Visible);
    UMMOUIReputationPanel panel;
    UGFxObject widgets[10], rewards;
    const char* professions[] = {"Warrior", "Mage", "Rogue", "Archer"};
    for (INT class_id = 0; class_id < 4; ++class_id) {
        char key[64];
        snprintf(key, sizeof(key), "_%sAwardItemID1", professions[class_id]); rewards.fields[key] = 7;
        snprintf(key, sizeof(key), "_%sAwardItemCount1", professions[class_id]); rewards.fields[key] = 100 + class_id;
    }
    for (INT missing : {-1, 0, 5, 9, 10}) {
        for (INT i = 0; i < 10; ++i) panel.widgets[i] = missing == 10 || missing == i ? nullptr : &widgets[i];
        panel.InitReputationData();
        assert(panel.ItemSlot.Num() == 10 && panel.ReputationList.Num() == 2);
        for (INT i = 0; i < 10; ++i) assert(panel.ItemSlot(i) == panel.widgets[i]);
        for (auto& value : panel.ReputationList.values) assert(!value.awardState && value.Visible);
        for (INT class_id = 0; class_id < 5; ++class_id) {
            facade.player.id = class_id; panel.assignments.clear(); panel.UpdateRewordSlot(&rewards);
            INT expected = 0;
            for (INT i = 0; i < 10; ++i) if (panel.widgets[i]) {
                auto& assignment = panel.assignments.at(expected++);
                assert(assignment.object == panel.widgets[i] && assignment.slot == i);
                assert(assignment.count == (i == 0 && class_id < 4 ? 100 + class_id : 0));
                assert(assignment.item == (i == 0 && class_id < 4 ? &facade.item : nullptr));
            }
            assert(panel.assignments.size() == size_t(expected));
        }
    }
    for (INT count : {0, 1, 5, 9, 10, 20}) {
        panel.ItemSlot.values.assign(count, &widgets[0]); panel.assignments.clear();
        panel.UpdateRewordSlot(&rewards);
        assert(panel.assignments.size() == size_t(std::min(count, 10)));
    }
    puts("Reputation records initialize award state; reward widgets preserve slot indexes and bound missing/short lists");
}
}
#pragma pop_macro("SCRIPT_ALIGN")
''')
    return "\n".join(parts)
