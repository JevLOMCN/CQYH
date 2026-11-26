"""Actual inventory, equipment, crowd and input-state lookup boundaries."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    generated = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    parts = [r'''
namespace InventoryLookupTests {
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    bool IsValidIndex(INT index) const { return index >= 0 && index < Num(); }
    T& operator()(INT index) { return values.at(index); }
    const T& operator()(INT index) const { return values.at(index); }
    void AddZeroed(INT count) { values.resize(values.size() + count, T()); }
};
''', function(generated, "enum ECONTAINER_SLOT\n") + ";", r'''
struct UItem {};
struct FClientItemInfo {
    static INT live;
    UINT m_nTemplateID;
    INT m_nCurDur, m_nMaxDur;
    UINT8 m_byItemSlot;
    FClientItemInfo(UINT id, INT stack, INT maximum, UINT8 slot)
        : m_nTemplateID(id), m_nCurDur(stack), m_nMaxDur(maximum), m_byItemSlot(slot) { ++live; }
    ~FClientItemInfo() { --live; }
    INT GetCurStackCount(const UItem*) const { return m_nCurDur; }
};
INT FClientItemInfo::live = 0;
using ItemsMap = std::map<UINT32, FClientItemInfo*>;
struct MMOFacade {
    BYTE before = 0x79;
    UINT8 ContainerSize[E_CONSLOT_MAX] = {};
    BYTE after = 0x68;
    ItemsMap _Containers[E_CONSLOT_MAX];
    UItem item_template;
    void ClearItemsByContainerId(UINT16);
    INT GetUsedSlotCountInBag(UINT16) const;
    INT GetItemCountInBag(UINT, UINT16);
    INT GetEmptyItemSlot(ECONTAINER_SLOT) const;
    INT GetCanStackSlot(ECONTAINER_SLOT, INT, INT) const;
    void SetContainerSize(ECONTAINER_SLOT, UINT16);
    UINT16 GetContainerSize(ECONTAINER_SLOT) const;
    FClientItemInfo* FindFirstItemPos(UINT, INT*, INT*) const;
    UItem* FindItemTemplate(UINT id) { return id == 100 ? &item_template : nullptr; }
};
''']
    facade = clean(source(game / "MMOFacade.cpp"))
    for signature in ["void MMOFacade::ClearItemsByContainerId(", "INT MMOFacade::GetUsedSlotCountInBag(",
                      "INT MMOFacade::GetItemCountInBag( UINT itemID, UINT16", "INT MMOFacade::GetEmptyItemSlot(",
                      "INT MMOFacade::GetCanStackSlot(", "void MMOFacade::SetContainerSize(ECONTAINER_SLOT",
                      "UINT16 MMOFacade::GetContainerSize(", "FClientItemInfo* MMOFacade::FindFirstItemPos("]:
        parts.append(function(facade, signature))
    parts.append(r'''
void InventoryTests() {
    MMOFacade facade;
    auto* first = new FClientItemInfo(100, 5, 10, 0);
    facade._Containers[E_CONSLOT_DEFAULT][0] = first;
    facade._Containers[E_CONSLOT_DEFAULT][2] = new FClientItemInfo(100, 7, 12, 2);
    facade.SetContainerSize(E_CONSLOT_DEFAULT, 4);
    assert(facade.GetUsedSlotCountInBag(E_CONSLOT_DEFAULT) == 2);
    assert(facade.GetItemCountInBag(100, E_CONSLOT_DEFAULT) == 12);
    assert(facade.GetItemCountInBag(101, E_CONSLOT_DEFAULT) == 0);
    assert(facade.GetEmptyItemSlot(E_CONSLOT_DEFAULT) == 1);
    assert(facade.GetCanStackSlot(E_CONSLOT_DEFAULT, 100, 3) == 0);
    assert(facade.GetCanStackSlot(E_CONSLOT_DEFAULT, 100, 6) == -1);
    for (INT index : {(-2147483647 - 1), -1, int(E_CONSLOT_MAX), int(E_CONSLOT_MAX) + 1, 65535, 2147483647}) {
        ECONTAINER_SLOT slot = (ECONTAINER_SLOT)index;
        facade.SetContainerSize(slot, 255);
        assert(!facade.GetContainerSize(slot));
        assert(facade.GetEmptyItemSlot(slot) == -1);
        assert(facade.GetCanStackSlot(slot, 100, 1) == -1);
        assert(facade.before == 0x79 && facade.after == 0x68);
    }
    for (UINT16 index : {UINT16(E_CONSLOT_MAX), UINT16(E_CONSLOT_MAX + 1), UINT16(65535)}) {
        facade.ClearItemsByContainerId(index);
        assert(!facade.GetUsedSlotCountInBag(index) && !facade.GetItemCountInBag(100, index));
        assert(FClientItemInfo::live == 2);
    }
    INT container = 123, slot = 456;
    assert(!facade.FindFirstItemPos(999, &container, &slot) && container == -1 && slot == -1);
    assert(facade.FindFirstItemPos(100, &container, &slot) == first && container == E_CONSLOT_DEFAULT && slot == 0);
    assert(facade.FindFirstItemPos(100, nullptr, &slot) == first && slot == 0);
    assert(facade.FindFirstItemPos(100, &container, nullptr) == first && container == E_CONSLOT_DEFAULT);
    assert(facade.FindFirstItemPos(100, nullptr, nullptr) == first);
    assert(!facade.FindFirstItemPos(999, nullptr, nullptr));
    facade.ClearItemsByContainerId(E_CONSLOT_DEFAULT);
    assert(!FClientItemInfo::live && !facade.GetUsedSlotCountInBag(E_CONSLOT_DEFAULT));
    puts("Inventory bounds, protected capacity bytes, initialized/optional search outputs and valid item paths passed");
}
struct UEquip { INT marker; };
struct UItemManager {
    TArray<UEquip*> EquippedItemList_Normal, EquippedItemList_Avatar;
    UEquip* GetEquipBySlot(INT, UBOOL);
};
struct UMMOCrowdInstance { INT marker; };
struct AMMOCrowdSystem {
    TArray<UMMOCrowdInstance*> CrowdInstanceData;
    UMMOCrowdInstance* GetInstanceByID(INT);
};
using FString = std::string;
struct UObject {};
struct UClass {
    FString name;
    FString GetName() const { return name; }
};
struct UKeyEventProcess {
    UClass type;
    INT entered = 0, left = 0;
    UObject* context = nullptr;
    UClass* GetClass() { return &type; }
    void OnLeave() { ++left; }
    void OnEnter() { ++entered; }
    void SetContext(UObject* object) { context = object; }
};
struct UKeyEventProcessor {
    TArray<UKeyEventProcess*> ProcessStateList;
    INT ActiveIndex = -1;
    UBOOL GotoKeyProcessState(const FString&, UObject*);
    UKeyEventProcess* GetActiveState();
};
''')
    parts.append(function(clean(source(game / "Src/ItemManager.cpp")), "UEquip* UItemManager::GetEquipBySlot("))
    parts.append(function(clean(source(game / "Src/MMOCrowd.cpp")), "class UMMOCrowdInstance* AMMOCrowdSystem::GetInstanceByID("))
    keys = clean(source(game / "Src/KeyEventProcess.cpp"))
    for signature in ["UBOOL UKeyEventProcessor::GotoKeyProcessState(", "UKeyEventProcess* UKeyEventProcessor::GetActiveState()"]:
        parts.append(function(keys, signature))
    parts.append(r'''
void LookupTests() {
    UEquip normal{10}, avatar{20};
    UItemManager equipment;
    equipment.EquippedItemList_Normal.values = {&normal, nullptr};
    equipment.EquippedItemList_Avatar.values = {&avatar};
    for (INT index : {(-2147483647 - 1), -1, 2, 2147483647}) {
        assert(!equipment.GetEquipBySlot(index, TRUE) && !equipment.GetEquipBySlot(index, FALSE));
    }
    assert(equipment.GetEquipBySlot(0, TRUE) == &normal && equipment.GetEquipBySlot(0, FALSE) == &avatar);
    assert(!equipment.GetEquipBySlot(1, TRUE) && !equipment.GetEquipBySlot(1, FALSE));
    equipment.EquippedItemList_Normal.values.clear(); assert(!equipment.GetEquipBySlot(0, TRUE));
    AMMOCrowdSystem crowd;
    UMMOCrowdInstance instance{30}; crowd.CrowdInstanceData.values = {&instance};
    assert(crowd.GetInstanceByID(0) == &instance);
    for (INT index : {(-2147483647 - 1), -1, 1, 2147483647}) assert(!crowd.GetInstanceByID(index));
    crowd.CrowdInstanceData.values.clear(); assert(!crowd.GetInstanceByID(0));
    UKeyEventProcess first{{"first"}}, second{{"second"}};
    UKeyEventProcessor processor; processor.ProcessStateList.values = {&first, &second};
    UObject context;
    for (INT index : {(-2147483647 - 1), -2, -1, 2, 2147483647}) {
        processor.ActiveIndex = index;
        assert(processor.GotoKeyProcessState("first", &context));
        assert(processor.GetActiveState() == &first && !first.left && !second.left && first.context == &context);
    }
    assert(processor.GotoKeyProcessState("second", nullptr));
    assert(first.left == 1 && second.entered == 1 && processor.GetActiveState() == &second && !second.context);
    assert(!processor.GotoKeyProcessState("missing", &context) && processor.GetActiveState() == &second);
    puts("Equipment/crowd array limits and input-state transitions with stale indices passed");
}
}
''')
    return "\n".join(parts)
