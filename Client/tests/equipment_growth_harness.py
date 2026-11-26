"""Actual equipment sparse-array growth, output defaults and configuration records."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    equip = clean(source(game / "MMOEquipManager.h"))
    cpp = clean(source(game / "MMOEquipManager.cpp"))
    parts = [r'''
#include <type_traits>
#pragma push_macro("SCOPE_LOG_TIME")
#undef SCOPE_LOG_TIME
#define SCOPE_LOG_TIME(...)
namespace EquipmentGrowthTests {
using FString = AttributeRecordTests::FString;
using FVector2D = AttributeRecordTests::FVector2D;
template<class T> struct TArray : SkillQuestTests::TArray<T> {
    void Add(INT count) {
        assert(count >= 0 && count < 10000);
        size_t start = this->data.size(); this->data.resize(start + count);
        // UE3 Add deliberately does not construct new elements.
        if constexpr (std::is_trivially_copyable_v<T>)
            for (size_t i = start; i < this->data.size(); ++i) memset(&this->data[i], 0xa5, sizeof(T));
    }
    void AddZeroed(INT count) {
        size_t start = this->data.size(); Add(count);
        if constexpr (std::is_trivially_copyable_v<T>)
            for (size_t i = start; i < this->data.size(); ++i) memset(&this->data[i], 0, sizeof(T));
    }
};
template<class K, class V> using TMap = SkillQuestTests::TMap<K, V>;
template<class T> using SingletonMeyers = SkillQuestTests::SingletonMeyers<T>;
''']
    for name in ["ENHANCE_EQUIP_PROPERTY"]:
        parts.append(re.search(r"#define " + name + r"\s+[^\n]+", equip).group())
    parts.append(re.search(r"static const INT EQUIP_RUNE_SKILL_MAX\s*=\s*[^;]+;", equip).group())
    records = []
    for filename, names in [("MMOEquipManager.h", ["EnhanceResult", "WeaponSkillCfg", "EquipLvUpCfg"]),
                            ("MMOLoginManager.h", ["ChangeMeshSubItem", "ChangeMeshItem", "PlayerAppearanceData"]),
                            ("MMOHelperData.h", ["HelperData"]),
                            ("MMOBuffManager.h", ["BuffTemplateDataIndex"]),
                            ("MMOJourneyData.h", ["MonsterMapData", "JourneyData"])]:
        text = clean(source(game / filename))
        for name in names:
            definition = function(text, "struct " + name + "\n") + ";"
            parts.append(definition)
            fields = re.findall(r"(?m)^\s*(?:INT|UINT|UINT8|INT32|UINT32|UBOOL)\s+(\w+)\s*;", definition)
            strings = re.findall(r"(?m)^\s*FString\s+(\w+)\s*;", definition)
            records.append((name, fields, strings))
    parts.append(r'''
struct CTabFile {
    struct Row { int level, cost, power; };
    std::vector<Row> rows;
    UINT forced_count = 0;
    UINT GetRowCount() const { return forced_count ? forced_count : (UINT)rows.size() + 3; }
    const char* GetData(const char* field, UINT index) {
        const Row& row = rows.at(index - 3); static std::string result;
        result = std::to_string(!strcmp(field, "EquipLevel") ? row.level : !strcmp(field, "LevelupCost") ? row.cost : row.power);
        return result.c_str();
    }
};
int appAtoi(const char* text) { return atoi(text); }
struct UMMOAbility {
    int id = 0;
    struct { int Level = 0; } SpellCommonAttribute;
    int eventGetAbilityId() const { return id; }
};
struct MMOSkillManager : SingletonMeyers<MMOSkillManager> {
    std::map<UINT, UMMOAbility*> abilities, learned;
    UMMOAbility* FindAbilityTemplate(UINT id) { auto it = abilities.find(id); return it == abilities.end() ? nullptr : it->second; }
    UMMOAbility* FindMyLearnedMainSkill(UINT id) { auto it = learned.find(id); return it == learned.end() ? nullptr : it->second; }
    static UINT CombinateAbilityID(UINT id, UINT level, UINT rune = 0) { return SkillQuestTests::MMOSkillManager::CombinateAbilityID(id, level, rune); }
};
using ItemInfo = ::ItemInfo;
enum { E_CONSLOT_EQUIPPED = 0, E_NES_MAIN_HAND = 0, M_SKIP_ROWS = 3 };
struct MMOFacade : SingletonMeyers<MMOFacade> {
    ItemInfo* weapon = nullptr;
    bool available = true;
    CTabFile table;
    ItemInfo* FindItem(INT, INT) { return weapon; }
    bool ReadConfigString(CTabFile*& result, const char*, BOOL) {
        if (!available) return false; result = new CTabFile(table); return true;
    }
};
struct MMOEquipManager {
    TArray<EquipLvUpCfg> _EquipLevelUpCfg;
    TMap<UINT, WeaponSkillCfg> _EquipRuneSkills;
    const EquipLvUpCfg* FindEquipLevelUpData(INT);
    EquipLvUpCfg* AddNewEquipLevelUpData(INT);
    UBOOL ReadEquipLevelUpData();
    const WeaponSkillCfg* GetWeaponSkillCfg(UINT);
    UBOOL GetSkillUniqueIDOnWeapon(UINT&, UINT);
    INT GetWeaponRuneSkillIDBySlotIdx(INT, const ItemInfo*);
    UMMOAbility* GetWeaponRuneSkillBySlotIdx(INT, const ItemInfo*, UBOOL&);
    UMMOAbility* GetCurWeaponRuneSkillOfMainSkillID(INT, UBOOL&);
};
''')
    for signature in ["const EquipLvUpCfg* MMOEquipManager::FindEquipLevelUpData(",
                      "EquipLvUpCfg* MMOEquipManager::AddNewEquipLevelUpData(",
                      "UBOOL MMOEquipManager::ReadEquipLevelUpData()",
                      "const WeaponSkillCfg *MMOEquipManager::GetWeaponSkillCfg(",
                      "UBOOL MMOEquipManager::GetSkillUniqueIDOnWeapon(",
                      "INT MMOEquipManager::GetWeaponRuneSkillIDBySlotIdx(",
                      "UMMOAbility* MMOEquipManager::GetWeaponRuneSkillBySlotIdx(",
                      "UMMOAbility* MMOEquipManager::GetCurWeaponRuneSkillOfMainSkillID("]:
        parts.append(function(cpp, signature))
    parts.append("void RecordTests() {")
    for name, fields, strings in records:
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& v) {" +
                     " ".join("assert(!v." + field + ");" for field in fields) +
                     " ".join("assert(v." + field + ".empty());" for field in strings) + "});")
    parts.append(r'''
    TipsStallTests::Poisoned<EnhanceResult>([](auto& v) { for (int value : v._Prop) assert(!value); });
    TipsStallTests::Poisoned<ChangeMeshItem>([](auto& v) {
        assert(!v.SubItem.Class && !v.SubItem.SubTypeValue && !v.SubItem.Sex && v.SubItem.MainType.empty());
    });
    TipsStallTests::Poisoned<MonsterMapData>([](auto& v) { assert(!v.MonsterPoint.X && !v.MonsterPoint.Y); });
}
void Run() {
    MMOEquipManager manager;
    for (int level : {-2147483647 - 1, -1, MAXINT / int(sizeof(EquipLvUpCfg)), MAXINT}) {
        assert(!manager.AddNewEquipLevelUpData(level));
        assert(!manager.FindEquipLevelUpData(level));
        assert(!manager._EquipLevelUpCfg.Num());
    }
    for (int level : {0, 1, 10, 10, 11, 100}) {
        auto* data = manager.AddNewEquipLevelUpData(level);
        assert(data == manager.FindEquipLevelUpData(level));
        assert(manager._EquipLevelUpCfg.Num() == level + 1);
        for (auto& entry : manager._EquipLevelUpCfg.data) assert(!entry._Level && !entry._MnyCost && !entry._PowerAdd);
    }
    manager.AddNewEquipLevelUpData(10)->_MnyCost = 123;
    manager.AddNewEquipLevelUpData(200); assert(manager.FindEquipLevelUpData(10)->_MnyCost == 123);
    auto& facade = MMOFacade::getSingleton();
    facade.table.rows = {{1, 12, 34}, {9, 56, 78}, {-1, 90, 90}, {MAXINT, 99, 99}};
    assert(manager.ReadEquipLevelUpData());
    assert(manager._EquipLevelUpCfg.Num() == 10);
    assert(manager.FindEquipLevelUpData(1)->_MnyCost == 12 && manager.FindEquipLevelUpData(9)->_PowerAdd == 78);
    for (int hole : {0, 2, 3, 4, 5, 6, 7, 8}) {
        auto* data = manager.FindEquipLevelUpData(hole); assert(!data->_Level && !data->_MnyCost && !data->_PowerAdd);
    }
    facade.table.forced_count = UINT32(-1);
    assert(!manager.ReadEquipLevelUpData()); assert(!manager._EquipLevelUpCfg.Num());
    facade.table.forced_count = 0;
    UBOOL active = 123;
    assert(!manager.GetCurWeaponRuneSkillOfMainSkillID(123, active) && !active);
    ItemInfo weapon; facade.weapon = &weapon;
    active = 123; assert(!manager.GetCurWeaponRuneSkillOfMainSkillID(123, active) && !active);
    auto& skills = MMOSkillManager::getSingleton();
    UMMOAbility first, second; first.id = 456; second.id = 123;
    auto& firstCfg = manager._EquipRuneSkills.FindOrAdd(1); firstCfg._SkillID = first.id;
    auto& secondCfg = manager._EquipRuneSkills.FindOrAdd(2); secondCfg._SkillID = second.id;
    skills.abilities[MMOSkillManager::CombinateAbilityID(first.id, 0)] = &first;
    skills.abilities[MMOSkillManager::CombinateAbilityID(second.id, 0)] = &second;
    weapon.m_wRune[0] = 1; active = 123;
    assert(!manager.GetCurWeaponRuneSkillOfMainSkillID(123, active) && !active);
    weapon.m_wRune[1] = 2;
    assert(manager.GetCurWeaponRuneSkillOfMainSkillID(123, active) == &second && active);
    for (int invalid : {-2147483647 - 1, -1, int(EQUIP_RUNE_SKILL_MAX), MAXINT}) {
        active = 123; assert(!manager.GetWeaponRuneSkillBySlotIdx(invalid, &weapon, active) && !active);
    }
    facade.weapon = nullptr;
    puts("Equipment level-up exact-end/sparse growth, initialized gaps, invalid levels and skill outputs passed");
}
}
#undef ENHANCE_EQUIP_PROPERTY
#pragma pop_macro("SCOPE_LOG_TIME")
''')
    return "\n".join(parts)
