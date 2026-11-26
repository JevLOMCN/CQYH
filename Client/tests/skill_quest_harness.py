"""Real skill records, twelve-slot mapping, short updates and quest sound pages."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    def generated(filename):
        data = (game / "Inc" / filename).read_bytes()
        return clean(data.decode("utf-16" if data.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))

    header = clean(source(game / "MMOSkillManager.h"))
    cpp = clean(source(game / "MMOSkillManager.cpp"))
    engine = generated("MMOGameClasses.h")
    ui = generated("MMOGameUIClasses.h")
    capacity = re.search(r"#define UCONST_SkillBarSlotMax\s+(\d+)", ui).group(1)
    parts = [r'''
#pragma push_macro("SCRIPT_ALIGN")
#pragma push_macro("CONST")
#pragma push_macro("SCOPE_LOG_TIME")
#pragma push_macro("MALLOC_PROFILER_GAME")
#pragma push_macro("M_SCOPE_MEM_PROF")
#undef SCRIPT_ALIGN
#undef CONST
#undef SCOPE_LOG_TIME
#undef MALLOC_PROFILER_GAME
#undef M_SCOPE_MEM_PROF
#define SCRIPT_ALIGN
#define CONST const
#define SCOPE_LOG_TIME(...)
#define MALLOC_PROFILER_GAME(...)
#define M_SCOPE_MEM_PROF(...)
namespace SkillQuestTests {
using BITFIELD = unsigned int;
enum EEventParm { EC_EventParm };
using FString = AttributeRecordTests::FString;
template<class T> struct TArray : AttributeRecordTests::TArray<T> {
    void Empty(INT = 0) { this->data.clear(); }
    void AddZeroed(INT count) { assert(count >= 0); this->data.resize(this->data.size() + count); }
    void Push(const T& value) { this->AddItem(value); }
};
template<class K, class V> struct TMap {
    std::map<K, V> data;
    V* Find(const K& key) { auto it = data.find(key); return it == data.end() ? nullptr : &it->second; }
    V& FindOrAdd(const K& key) { return data[key]; }
    void Empty() { data.clear(); }
    template<class Compare> void ValueSort() {}
};
template<class T> struct SingletonMeyers {
    static T& getSingleton() { static T value; return value; }
};
''', "enum { UCONST_SkillBarSlotMax = " + capacity + " };",
        re.search(r"static const INT SKILL_EQUIP_MAX\s*=\s*[^;]+;", header).group(),
        function(ui, "enum ECommandKey\n") + ";",
        function(engine, "enum EPlayerClass\n")
            .replace("EPCT_None               =6,", "EPCT_PlayerClassG = 6, EPCT_PlayerClassH = 7, EPCT_None = 8,")
            .replace("EPCT_MAX                =7,", "EPCT_MAX = 9,") + ";"]
    for kind, name in [("struct", "FSpell_ClassType"), ("enum", "ESPELL_CAST_TYPE"),
                       ("enum", "ESPELL_QTE_TYPE")]:
        definition = function(engine, kind + " " + name + "\n")
        if name == "FSpell_ClassType":
            definition = definition.replace("BITFIELD bClassType6:1;", "BITFIELD bClassType6:1; BITFIELD bClassType7:1; BITFIELD bClassType8:1;")
        parts.append(definition + ";")
    parts.append(function(header, "enum SpecailSkillType\n") + ";")
    for name in ["RuneSkillLevelUpConsum", "SpecialSkill", "TransformSkillGroup",
                 "AbilityTemplateDataIndex", "SpellVarData", "SpellProgessData", "MissileTemplateDataIndex"]:
        parts.append(function(header, "struct " + name + "\n") + ";")
    init = function(cpp, "void MMOSkillManager::Init()")
    events = sorted(set(re.findall(r"\bGameEvent_\w+", init)) |
                    {"GameEvent_ChangeSkillSlot", "GameEvent_SkillUiUpdate"})
    parts.extend(["enum { " + ", ".join(events) + " };", r'''
struct INativeGameEventHandler {};
struct MMOGameEvent_OneInt { MMOGameEvent_OneInt(int, int) {} };
struct MMOGameEvent {};
using EGameEvent = int;
struct UMMOAbility {
    int id = 123;
    UMMOAbility* MainSkill = nullptr;
    struct { FSpell_ClassType Class = FSpell_ClassType(EC_EventParm); } SpellCommonAttribute;
    int eventGetAbilityId() const { return id; }
};
namespace MMOUI {
    int posted = 0;
    template<class T> void PostGameEvent(T*) { ++posted; }
    void RegisterEventHandler(void*, int) {}
}
struct TableManager : SingletonMeyers<TableManager> { void InitFromFile(const char*, bool = false, bool = false) {} };
using SpellVarManager = TableManager;
using SpellProgessDataManager = TableManager;
using AbilityTemplateDataIndexManager = TableManager;
struct CompareInUi {};
#define COMPARE_POINTER_CLASS(...) CompareInUi
enum { MAX_SPELL_LEVEL = 3 };
''', function(header, "struct MMOGameEvent_SkillUiUpdate :") + ";", r'''
struct MMOSkillManager : SingletonMeyers<MMOSkillManager> {
    MMOSkillManager();
    void Init();
    void ReflashSkillOutlineData() {}
    void LoadSpecialSkills() {}
    void LoadTransformSkillGroup() {}
    TMap<INT, INT> _SkillSlotCmdDefault;
    TMap<INT, UBOOL> _SkillSlotActive, _SkillSlotFlag1;
    TMap<INT, FString> _SkillSlotIcon;
    TArray<TArray<TArray<INT>>> _AllMainAbilityIDList;
    UMMOAbility* CurRangeShowedSkill;
    UMMOAbility ability;
    int lookups = 0;
    UMMOAbility* FindAbilityTemplateNoWarning(UINT) { ++lookups; return &ability; }
    UMMOAbility* FindAbilityTemplate(UINT) { ++lookups; return &ability; }
    static UINT CombinateAbilityID(UINT, UINT, UINT = 0);
    INT GetSkillSlotDefaultBindEnum(INT);
    INT GetSkillbarSlotRealCmd(INT);
    INT GetSkillbarSlotIdxByKeyCmd(INT);
    UBOOL RedirectSkillSlot(INT, INT, UBOOL = TRUE);
    UBOOL ExchangeRedirectSkillSlot(INT, INT, UBOOL = TRUE);
    void ReflashSkillSlotRedirection(INT32*, INT);
    static void ConvertSpellType(TArray<UBOOL>&, const FSpell_ClassType&);
    static UBOOL IsSkillUsableForPlayer(UMMOAbility*, INT);
    TMap<INT, INT> GetAllMainAbilitiesIDs(INT, INT);
    TMap<INT, UMMOAbility*> GetAllMainAbilitiesSorted(INT, INT);
''', re.search(r"INT _SkillBarSlotRedirect\[[^\]]+\];", header).group(), "};", init])
    for signature in ["MMOSkillManager::MMOSkillManager()", "INT MMOSkillManager::GetSkillSlotDefaultBindEnum(",
                      "INT MMOSkillManager::GetSkillbarSlotRealCmd(", "INT MMOSkillManager::GetSkillbarSlotIdxByKeyCmd(",
                      "UBOOL MMOSkillManager::RedirectSkillSlot(", "UBOOL MMOSkillManager::ExchangeRedirectSkillSlot(",
                      "void MMOSkillManager::ReflashSkillSlotRedirection(", "void MMOSkillManager::ConvertSpellType(",
                      "UBOOL MMOSkillManager::IsSkillUsableForPlayer(", "UINT MMOSkillManager::CombinateAbilityID(",
                      "TMap< INT, INT > MMOSkillManager::GetAllMainAbilitiesIDs(",
                      "TMap< INT, class UMMOAbility* > MMOSkillManager::GetAllMainAbilitiesSorted("]:
        parts.append(function(cpp, signature))
    parts.append(r'''
void RecordTests() {
    TipsStallTests::Poisoned<RuneSkillLevelUpConsum>([](auto& v) { assert(!v._ItemID && !v._ItemCnt); });
    TipsStallTests::Poisoned<SpecialSkill>([](auto& v) {
        assert(!v._SkillID && v._SpecialSkillType == ESpcSkill_MAX && v._GetRequire.empty());
    });
    TipsStallTests::Poisoned<TransformSkillGroup>([](auto& v) { for (auto id : v._MainSkillID) assert(!id); });
    TipsStallTests::Poisoned<AbilityTemplateDataIndex>([](auto& v) {
        assert(v.ObjectPath.empty() && !v.TemplateID && !v.RuneSlotID && !v.ShowOnUI);
        assert(!v.Class.bClassType1 && !v.Class.bClassType2 && !v.Class.bClassType3);
        assert(!v.Class.bClassType4 && !v.Class.bClassType5 && !v.Class.bClassType6);
        assert(v.CastType == E_spct_Passive && v.QTEType == E_spqte_None);
    });
    TipsStallTests::Poisoned<SpellVarData>([](auto& v) {
        assert(!v.TemplateID && !v.RuneSlotID && !v.Level && !v.EffectRangeEnum);
        assert(v.Variable1.empty() && v.Value4.empty() && v.DetailDescription.empty());
    });
    TipsStallTests::Poisoned<SpellProgessData>([](auto& v) {
        assert(!v.TemplateID && !v.RuneSlot && !v.Level && !v.ConsumePts && !v.RequireLevel);
        assert(!v.RequireItem && !v.RequireItemCount && !v.Pow);
    });
    TipsStallTests::Poisoned<MissileTemplateDataIndex>([](auto& v) { assert(!v.TemplateID && v.ObjectPath.empty()); });
}
void SkillTests() {
    TipsStallTests::Poisoned<MMOSkillManager>([](auto& manager) {
        assert(!manager.CurRangeShowedSkill);
        static_assert(ARRAY_COUNT(manager._SkillBarSlotRedirect) == UCONST_SkillBarSlotMax);
        for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i) assert(manager.GetSkillbarSlotRealCmd(i) == -1);
        assert(manager.GetSkillbarSlotIdxByKeyCmd(-1) == -1);
        manager.Init();
        for (INT i = 0; i < UCONST_SkillBarSlotMax; ++i)
            assert(manager.GetSkillbarSlotRealCmd(i) == (i < 6 ? ECK_Slot1 + i : -1));
        for (INT invalid : {-2147483647 - 1, -1, int(UCONST_SkillBarSlotMax), 2147483647}) {
            assert(manager.GetSkillbarSlotRealCmd(invalid) == -1);
            assert(!manager.RedirectSkillSlot(invalid, ECK_Slot2));
            assert(!manager.ExchangeRedirectSkillSlot(invalid, 1) && !manager.ExchangeRedirectSkillSlot(1, invalid));
        }
        assert(manager.GetSkillbarSlotIdxByKeyCmd(ECK_Slot6) == 5);
        assert(manager.GetSkillbarSlotIdxByKeyCmd(ECK_Slot12) == -1);
        assert(manager.RedirectSkillSlot(UCONST_SkillBarSlotMax - 1, ECK_Slot3));
        assert(manager.GetSkillbarSlotRealCmd(UCONST_SkillBarSlotMax - 1) == ECK_Slot3);
        assert(manager.ExchangeRedirectSkillSlot(5, UCONST_SkillBarSlotMax - 1));
        assert(manager.GetSkillbarSlotRealCmd(5) == ECK_Slot3);
        assert(manager.GetSkillbarSlotRealCmd(UCONST_SkillBarSlotMax - 1) == ECK_Slot6);
        manager.Init();
        INT32 server[UCONST_SkillBarSlotMax]; for (auto& value : server) value = ECK_Slot2;
        for (INT length = -1; length < INT(sizeof(server)); ++length) {
            manager.ReflashSkillSlotRedirection(server, length);
            assert(manager.GetSkillbarSlotRealCmd(UCONST_SkillBarSlotMax - 1) == -1);
        }
        manager.ReflashSkillSlotRedirection(nullptr, sizeof(server));
        manager.ReflashSkillSlotRedirection(server, sizeof(server));
        assert(manager.GetSkillbarSlotRealCmd(UCONST_SkillBarSlotMax - 1) == ECK_Slot2);
        manager._AllMainAbilityIDList.data.resize(1);
        manager._AllMainAbilityIDList(0).data.resize(1);
        manager._AllMainAbilityIDList(0)(0).AddItem(123);
        for (INT invalid : {-2147483647 - 1, -1, 1, 2147483647}) {
            assert(manager.GetAllMainAbilitiesIDs(invalid, 0).data.empty());
            assert(manager.GetAllMainAbilitiesIDs(0, invalid).data.empty());
            assert(manager.GetAllMainAbilitiesSorted(invalid, 0).data.empty());
            assert(manager.GetAllMainAbilitiesSorted(0, invalid).data.empty());
        }
        assert(manager.lookups == 0);
        assert(*manager.GetAllMainAbilitiesIDs(0, 0).Find(123) == 0);
        assert(*manager.GetAllMainAbilitiesSorted(0, 0).Find(123) == &manager.ability);
    });
    FSpell_ClassType classes(EC_EventParm); classes.bClassType1 = classes.bClassType6 = TRUE;
    for (INT count : {0, 1, 6, int(EPCT_MAX), 8, 40}) {
        TArray<UBOOL> flags; flags.data.assign(count, 123);
        MMOSkillManager::ConvertSpellType(flags, classes);
        assert(flags.Num() == EPCT_MAX);
        for (INT i = 0; i < EPCT_MAX; ++i) assert(flags(i) == (i == 0 || i == 5));
    }
    UMMOAbility ability; ability.SpellCommonAttribute.Class = classes;
    assert(MMOSkillManager::IsSkillUsableForPlayer(&ability, 0));
    assert(MMOSkillManager::IsSkillUsableForPlayer(&ability, 5));
    for (INT i : {-1, 1, 6, 7, int(EPCT_MAX), 2147483647}) assert(!MMOSkillManager::IsSkillUsableForPlayer(&ability, i));
    puts("Skill defaults, twelve-slot mapping capacity, short syncs and class/category bounds passed");
}
''')
    bar = clean(source(game / "Src/MMOUISkillBar.cpp"))
    parts.extend([function(ui, "enum SkillBarSlotType\n") + ";", "struct UGFxObject;",
                  function(ui, "struct FSlotContainer\n") + ";", "#pragma pack(push, 1)",
                  function(bar, "struct FSKILL_BAR_SLOT\n") + ";", "#pragma pack(pop)", r'''
enum { SKILL_BAR_ITEM_SLOT_UNFIX_POS_ID = -1 };
struct UMMOUISkillBar {
    FSlotContainer SkillBarSlot[UCONST_SkillBarSlotMax];
    int sets = 0;
    UMMOUISkillBar() { for (auto& slot : SkillBarSlot) slot = FSlotContainer(EC_EventParm); }
    FSlotContainer& SkillBarSlotData(int i) { assert(i >= 0 && i < UCONST_SkillBarSlotMax); return SkillBarSlot[i]; }
    void SetBarSlot(int i, SkillBarSlotType type, int id) {
        SkillBarSlot[i].Type = type; SkillBarSlot[i].ID = id; ++sets;
    }
    void RefreshSkillBarByServerData(INT32*, INT);
};
''', function(bar, "void UMMOUISkillBar::RefreshSkillBarByServerData("), r'''
void BarTests() {
    UMMOUISkillBar bar;
    for (INT size = 1; size < sizeof(FSKILL_BAR_SLOT) * UCONST_SkillBarSlotMax; ++size) {
        std::vector<INT32> shortMessage((size + 3) / 4);
        bar.RefreshSkillBarByServerData(shortMessage.data(), size); assert(!bar.sets);
    }
    bar.RefreshSkillBarByServerData(nullptr, 100); assert(!bar.sets);
    FSKILL_BAR_SLOT full[UCONST_SkillBarSlotMax] = {};
    full[0].type = ESBarSlotTp_Skill;
    full[1].type = ESBarSlotTp_MAX; full[1].id = 999;
    full[UCONST_SkillBarSlotMax - 1].type = ESBarSlotTp_Item;
    full[UCONST_SkillBarSlotMax - 1].id = 123;
    bar.RefreshSkillBarByServerData((INT32*)full, sizeof(full));
    assert(bar.sets == UCONST_SkillBarSlotMax - 1);
    assert(bar.SkillBarSlot[1].ID == -1 && bar.SkillBarSlot[1].AidID == 999);
    assert(bar.SkillBarSlot[UCONST_SkillBarSlotMax - 1].ID == 123);
    puts("Short skill-bar updates rejected and complete twelve-slot updates passed");
}
}
#undef COMPARE_POINTER_CLASS
#pragma pop_macro("M_SCOPE_MEM_PROF")
#pragma pop_macro("MALLOC_PROFILER_GAME")
#pragma pop_macro("SCOPE_LOG_TIME")
#pragma pop_macro("SCRIPT_ALIGN")
#pragma pop_macro("CONST")
'''])
    return "\n".join(parts)
