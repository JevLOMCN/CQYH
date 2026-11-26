"""Actual reputation flags, queued item records and equipment-hint empty queue."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', text)

    def generated(path):
        raw = path.read_bytes()
        return clean(raw.decode("utf-16" if raw.startswith(b"\xff\xfe") else "latin1").replace("\r", ""))

    core = generated(game / "Inc/MMOGameClasses.h")
    ui = generated(game / "Inc/MMOGameUIClasses.h")
    parts = [r'''
#pragma push_macro("SCRIPT_ALIGN")
#pragma push_macro("MS_ALIGN")
#pragma push_macro("GCC_ALIGN")
#undef SCRIPT_ALIGN
#undef MS_ALIGN
#undef GCC_ALIGN
#define SCRIPT_ALIGN
#define MS_ALIGN(n) __declspec(align(n))
#define GCC_ALIGN(n)
namespace RecordFlagTests {
using BITFIELD = UINT;
enum EEventParm { EC_EventParm };
struct UGFxObject { INT marker; };
''']
    for name, text in [("FReputation", core), ("FOperationItemBagPos", core), ("FSlotContainer", ui), ("FHitGroup", core)]:
        parts.append(function(text, "struct " + name + "\n") + ";")
    parts.append(r'''
template<class T> struct TArray : ItemMeshBoundsTests::TArray<T> {
    INT removes = 0;
    void Push(const T& value) { this->AddItem(value); }
    void Remove(INT index) {
        assert(this->IsValidIndex(index)); ++removes; this->values.erase(this->values.begin() + index);
    }
};
struct UActorAttribute {
    TArray<FReputation> ReputationArray;
    void SetReputation(INT, INT, INT);
    INT GetReputation(INT);
    void SetActiveState(INT, INT, UBOOL);
    UBOOL GetActiveState(INT);
    void SetAwardState(INT, INT, UBOOL);
    UBOOL GetAwardState(INT);
};
struct UMMOUIEquipGainHintBox {
    TArray<FOperationItemBagPos> _NewGainedEquip;
    INT shows = 0, last_con = -1, last_slot = -1;
    void Show(UBOOL show) { assert(!show); ++shows; }
    void ShowContent(INT con, INT slot) { last_con = con; last_slot = slot; }
    void AddNewEquipGain(INT, INT);
    void TryShowNextOrHide();
    void ClearUI();
    void OnItemSwapped(INT, INT, INT, INT);
};
enum { E_CONSLOT_DEFAULT = 0, UCONST_SkillBarSlotMax = 12 };
struct UMMOUIWeaponSkillSetting {
    TArray<FOperationItemBagPos> _SkillLevelUpItems;
    void QueueMaterial(UINT j);
};
struct UMMOUISkillBar {
    FSlotContainer SkillBarSlot[UCONST_SkillBarSlotMax];
    UINT dirty = 0;
    FSlotContainer& SkillBarSlotData(INT index) { assert(index >= 0 && index < UCONST_SkillBarSlotMax); return SkillBarSlot[index]; }
    void MarkSlotDirty(INT index) { dirty |= UINT(1) << index; }
    void SwapBarSlotDataRaw(INT, INT);
};
''')
    attribute = clean(source(game / "Src/UActorAttribute.cpp"))
    for name, ctype in [("SetReputation", "void"), ("GetReputation", "INT"), ("SetActiveState", "void"),
                        ("GetActiveState", "UBOOL"), ("SetAwardState", "void"), ("GetAwardState", "UBOOL")]:
        parts.append(function(attribute, ctype + " UActorAttribute::" + name + "("))
    gain = clean(source(game / "Src/MMOUIEquipGainHintBox.cpp"))
    for name in ["AddNewEquipGain", "TryShowNextOrHide", "ClearUI", "OnItemSwapped"]:
        parts.append(function(gain, "void UMMOUIEquipGainHintBox::" + name + "("))
    material = clean(source(game / "Src/MMOUIWeaponSkillSetting.cpp"))
    start = material.index("FOperationItemBagPos newPos")
    end = material.index("_SkillLevelUpItems.Push( newPos );", start) + len("_SkillLevelUpItems.Push( newPos );")
    parts.extend(["void UMMOUIWeaponSkillSetting::QueueMaterial(UINT j) {", material[start:end], "}"])
    bar = clean(source(game / "Src/MMOUISkillBar.cpp"))
    parts.append(function(bar, "void UMMOUISkillBar::SwapBarSlotDataRaw("))
    parts.append("void PoisonTests() {")
    # Use the exact production initializer over poisoned storage, so checking
    # only a zero-filled stack frame cannot conceal a regression.
    for name, variable, text, assertion in [
        ("FReputation", "buf", attribute, "!value->ForceID && !value->ReputationData && !value->ActiveState && !value->AwardState"),
        ("FOperationItemBagPos", "newEquipPos", gain, "!value->_ConID && !value->_SlotID && !value->_IsValid"),
        ("FOperationItemBagPos", "newPos", material, "!value->_ConID && !value->_SlotID && !value->_IsValid"),
        ("FSlotContainer", "tempData", bar, "!value->ID && !value->AidID && !value->Type && !value->State && !value->SlotObject"),
        ("FHitGroup", "HitGroup", clean(source(game / "AbilityProcessor.cpp")), "!value->WeaponType && !value->WeaponHitType && !value->DirectionType && !value->NoUse"),
    ]:
        match = re.search(r'\b' + name + r'\s+' + variable + r'(\([^;]*\))?\s*;', text)
        assert match, (name, variable)
        parts.extend(["{", "alignas(" + name + ") BYTE bytes[sizeof(" + name + ")];",
                      "memset(bytes, 0xff, sizeof(bytes));",
                      "auto* value = new(bytes) " + name + (match.group(1) or "") + ";",
                      "assert(" + assertion + ");", "}"])
    assert "_IsValid=true" in source(game / "Classes/TypeDefine.uc")
    parts.append(r'''
}
void Run() {
    PoisonTests();
    UActorAttribute attributes;
    attributes.SetReputation(0, 7, 10);
    assert(attributes.GetReputation(7) == 10 && !attributes.GetActiveState(7) && !attributes.GetAwardState(7));
    attributes.SetActiveState(0, 7, TRUE); attributes.SetAwardState(0, 7, TRUE);
    attributes.SetReputation(0, 7, 20);
    assert(attributes.ReputationArray.Num() == 1 && attributes.GetReputation(7) == 20);
    assert(attributes.GetActiveState(7) && attributes.GetAwardState(7));
    attributes.SetReputation(1, 9, 30);
    assert(!attributes.GetActiveState(9) && !attributes.GetAwardState(9));
    assert(!attributes.GetReputation(99) && !attributes.GetActiveState(99) && !attributes.GetAwardState(99));
    UMMOUIEquipGainHintBox hint;
    hint.TryShowNextOrHide(); hint.TryShowNextOrHide();
    assert(hint.shows == 2 && hint._NewGainedEquip.removes == 0);
    hint.AddNewEquipGain(4, 5); hint.AddNewEquipGain(6, 7);
    for (auto& pos : hint._NewGainedEquip.values) assert(pos._IsValid);
    hint.OnItemSwapped(4, 5, 10, 11);
    assert(hint._NewGainedEquip(0)._ConID == 10 && hint._NewGainedEquip(0)._SlotID == 11 && hint._NewGainedEquip(0)._IsValid);
    hint.TryShowNextOrHide(); assert(hint.last_con == 6 && hint.last_slot == 7 && hint._NewGainedEquip.Num() == 1);
    hint.ClearUI(); hint.TryShowNextOrHide();
    assert(!hint._NewGainedEquip.Num() && hint._NewGainedEquip.removes == 1);
    UMMOUIWeaponSkillSetting setting;
    for (UINT slot : {UINT(0), UINT(1), UINT(255)}) setting.QueueMaterial(slot);
    assert(setting._SkillLevelUpItems.Num() == 3);
    for (INT i = 0; i < 3; ++i) assert(setting._SkillLevelUpItems(i)._IsValid && setting._SkillLevelUpItems(i)._ConID == E_CONSLOT_DEFAULT);
    UMMOUISkillBar skillbar;
    UGFxObject objects[UCONST_SkillBarSlotMax];
    for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i) {
        auto& slot = skillbar.SkillBarSlot[i];
        slot.ID = i; slot.AidID = i + 100; slot.Type = BYTE(i % 4);
        slot.State = i + 200; slot.SlotObject = &objects[i];
    }
    skillbar.SwapBarSlotDataRaw(0, UCONST_SkillBarSlotMax - 1);
    assert(skillbar.SkillBarSlot[0].ID == UCONST_SkillBarSlotMax - 1 && skillbar.SkillBarSlot[UCONST_SkillBarSlotMax - 1].ID == 0);
    for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i) assert(skillbar.SkillBarSlot[i].State == i + 200 && skillbar.SkillBarSlot[i].SlotObject == &objects[i]);
    assert(skillbar.dirty == (1u | (1u << (UCONST_SkillBarSlotMax - 1))));
    puts("Reputation/item flags initialize over poisoned storage; existing flags and slot widgets survive updates; empty hint queue is safe");
}
}
#pragma pop_macro("GCC_ALIGN")
#pragma pop_macro("MS_ALIGN")
#pragma pop_macro("SCRIPT_ALIGN")
''')
    return "\n".join(parts)
