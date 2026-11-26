"""Exercise production equipment-slot defaults, config loading and bounds."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "EquipSlotManager.h"))
    header = re.sub(r"(?m)^\s*#include[^\n]*", "", header)
    cpp = clean(source(game / "EquipSlotManager.cpp"))
    parts = [r'''
namespace EquipSlotTests {
using FString = std::string;
template<class T> struct SingletonMeyers {};
template<class K, class V> struct TMap {
    std::map<K, V> data;
    V& FindOrAdd(K key) { return data[key]; }
    V* Find(K key) { auto it = data.find(key); return it == data.end() ? nullptr : &it->second; }
};
template<class T> struct TArray {
    std::vector<T> data;
    int Num() const { return (int)data.size(); }
    void Push(T value) { data.push_back(value); }
    void InsertItem(T value, int index) { data.insert(data.begin() + index, value); }
    const T& operator()(int index) const { return data.at(index); }
    T& operator()(int index) { return data.at(index); }
};
enum EGameEvent { GameEvent_EquipSlotSyncLevel, GameEvent_EquipSlotEnhanceLvUp };
struct MMOGameEvent { virtual EGameEvent GetEventType() const = 0; };
struct MMOGameEvent_Any : MMOGameEvent {
    EGameEvent type;
    MMOGameEvent_Any(EGameEvent event) : type(event) {}
    EGameEvent GetEventType() const { return type; }
};
struct INativeGameEventHandler { virtual void OnNativeGameEvent(MMOGameEvent*) = 0; };
struct UEquip;
namespace MMOUI {
int notifications = 0;
void RegisterEventHandler(void*, EGameEvent) {}
void UnRegisterEventHandler(void*, EGameEvent) {}
void PostGameEvent(MMOGameEvent*) { ++notifications; }
}
#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif
struct CTabFile {
    using Row = std::map<std::string, std::string>;
    std::vector<Row> rows;
    int GetRowCount() { return (int)rows.size() + 3; }
    const char* GetData(const char* field, int row) { return rows.at(row - 3)[field].c_str(); }
    const char* GetData(int column, int row) {
        return rows.at(row - 3)["Attr" + std::to_string(column - 2)].c_str();
    }
};
int appAtoi(const char* value) { return atoi(value); }
struct MMOFacade {
    bool available = true;
    CTabFile table;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    bool ReadConfigString(CTabFile*& out, const char*, UBOOL) {
        if (!available) return false;
        out = new CTabFile(table); return true;
    }
    int FindClientEnumValue(const char* value, const char*, int = 2) { return atoi(value); }
};
''']
    parts.append(function(clean(source(client / "Common/define/item_def.h")),
                          "enum NORMAL_EQUIP_SLOT\n") + ";")
    enums = (game / "Inc/MMOGameClasses.h").read_text(encoding="utf-16")
    signature = re.search(r"enum EPlayerClass\s*\{", enums).group()
    parts.append(function(enums, signature) + ";")
    parts.append(header)
    parts.append("void EquipSlotManager::OnNativeGameEvent(MMOGameEvent*) {}")
    for signature in [
        "EquipSlotManager::EquipSlotManager()", "EquipSlotManager::~EquipSlotManager()",
        "void EquipSlotManager::ReadEnhancePropConfig()",
        "void EquipSlotManager::ReadEquipSlotEnhanceConfig()",
        "const EquipSlotEnhancePropCfgData *EquipSlotManager::FindEnhancePropCfg(",
        "const EquipSlotEnhanceCfgData *EquipSlotManager::FindEnhanceConsumeCfg(",
        "void EquipSlotManager::SyncEquipSlotLevel(",
        "EquipSlotProperties* EquipSlotManager::GetMyEquipSlotProperty(",
        "INT EquipSlotManager::GetMyEquipSlotLevel(",
        "INT EquipSlotManager::GetEnhanceActiveLevel() const",
        "const EquipSlotEnhanceActiveTemplate& EquipSlotManager::GetEquipSlotEnhanceActiveTemplate(",
    ]:
        parts.append(function(cpp, signature))
    parts.append(r'''
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(EquipSlotEnhancePropCfgData) unsigned char prop_bytes[sizeof(EquipSlotEnhancePropCfgData)];
        memset(prop_bytes, pattern, sizeof(prop_bytes));
        auto* prop = new(prop_bytes) EquipSlotEnhancePropCfgData;
        assert(!prop->RefineLv && !prop->ClassId && !prop->EquipSlotId);
        for (int value : prop->EffectProperties) assert(!value);
        alignas(EquipSlotEnhanceCfgData) unsigned char cfg_bytes[sizeof(EquipSlotEnhanceCfgData)];
        memset(cfg_bytes, pattern, sizeof(cfg_bytes));
        auto* cfg = new(cfg_bytes) EquipSlotEnhanceCfgData;
        assert(!cfg->RefineLv && !cfg->PlayerRequireLv && !cfg->MoneyCost);
        assert(!cfg->BaseSuccessRate && !cfg->SuccessRateFactor && !cfg->IsSysTip);
        alignas(EquipSlotEnhanceActiveTemplate::EffectData) unsigned char effect_bytes[sizeof(EquipSlotEnhanceActiveTemplate::EffectData)];
        memset(effect_bytes, pattern, sizeof(effect_bytes));
        auto* effect = new(effect_bytes) EquipSlotEnhanceActiveTemplate::EffectData;
        assert(!effect->EType && !effect->AttributeId && !effect->Value);
        alignas(EquipSlotManager) unsigned char manager_bytes[sizeof(EquipSlotManager)];
        memset(manager_bytes, pattern, sizeof(manager_bytes));
        auto* manager = new(manager_bytes) EquipSlotManager;
        assert(!manager->CheckFlag(EES_MAX) && !manager->GetEnhanceActiveLevel());
        for (int i = 0; i < MAX_ACTIVE_LEVEL; ++i) {
            const auto& active = manager->GetEquipSlotEnhanceActiveTemplate(i);
            assert(!active.level && !active.ActiveEffectList.Num());
        }
        for (int i = 0; i < EQUIP_SLOT_COUNT; ++i) {
            auto* slot = manager->GetCurMySlotEnhanceState(i);
            assert(slot && !slot->CurEnhanceLevel && !slot->MaxEnhanceLevel);
        }
        manager->~EquipSlotManager();
    }
    EquipSlotManager manager;
    auto& facade = MMOFacade::getSingleton();
    facade.available = false;
    manager.ReadEnhancePropConfig(); manager.ReadEquipSlotEnhanceConfig();
    assert(!manager.FindEnhancePropCfg(0, 0, 20) && !manager.FindEnhanceConsumeCfg(20));
    facade.available = true;
    CTabFile::Row valid = {{"RefineLevel", "20"}, {"Class", "0"}, {"NormalEquipSlot", "13"},
        {"Attr1", "11"}, {"Attr2", "22"}, {"Attr3", "33"}, {"Attr4", "44"}, {"Attr5", "55"},
        {"RequireLevel", "7"}, {"SuccessRate", "9000"}, {"BasePow", "100"}, {"Cost", "500"}, {"BroadCast", "1"}};
    facade.table.rows = {valid};
    for (auto field : {"Class", "NormalEquipSlot", "RefineLevel"}) {
        for (auto value : {"-2147483648", "-1", "2147483647"}) {
            auto bad = valid; bad[field] = value; facade.table.rows.push_back(bad);
        }
    }
    for (auto field : {"Class", "NormalEquipSlot"}) {
        auto bad = valid; bad[field] = field == std::string("Class") ? "7" : "14";
        facade.table.rows.push_back(bad);
    }
    manager.ReadEnhancePropConfig();
    const auto* prop = manager.FindEnhancePropCfg(0, NES_TOTAL, 20);
    assert(prop && prop->RefineLv == 20 && prop->ClassId == 0 && prop->EquipSlotId == NES_TOTAL);
    for (int i = 0; i < ENHANCE_EFFECT_COUNT; ++i) assert(prop->EffectProperties[i] == 11 * (i + 1));
    assert(manager.GetSortedEnhanceExtrPropLvIdx(0).Num() == 1);
    assert(manager.GetSortedEnhanceExtrPropLvIdx(0)(0) == 20);
    for (int invalid : {-2147483647 - 1, -1, 2147483647}) {
        assert(!manager.GetSortedEnhanceExtrPropLvIdx(invalid).Num());
        assert(!manager.GetEquipSlotEnhanceActiveTemplate(invalid).ActiveEffectList.Num());
        assert(!manager.GetCurMySlotEnhanceState(invalid) && !manager.GetMyEquipSlotProperty(invalid));
        assert(manager.GetMyEquipSlotLevel(invalid) == -1);
        assert(!manager.FindEnhancePropCfg(invalid, NES_TOTAL, 20));
        assert(!manager.FindEnhancePropCfg(0, invalid, 20));
        assert(!manager.FindEnhancePropCfg(0, NES_TOTAL, invalid));
    }
    assert(!manager.GetSortedEnhanceExtrPropLvIdx(EPCT_MAX).Num());
    assert(!manager.GetEquipSlotEnhanceActiveTemplate(MAX_ACTIVE_LEVEL).ActiveEffectList.Num());
    assert(!manager.GetCurMySlotEnhanceState(EQUIP_SLOT_COUNT));
    assert(!manager.FindEnhancePropCfg(256, NES_TOTAL, 20));
    assert(!manager.FindEnhancePropCfg(0, NES_TOTAL + 256, 20));
    assert(!manager.FindEnhancePropCfg(0, NES_TOTAL, 20 + 65536));
    facade.table.rows = {valid}; manager.ReadEquipSlotEnhanceConfig();
    const auto* cfg = manager.FindEnhanceConsumeCfg(20);
    assert(cfg && cfg->RefineLv == 20 && cfg->PlayerRequireLv == 7 && cfg->MoneyCost == 500);
    assert(cfg->BaseSuccessRate == 9000 && cfg->SuccessRateFactor == 100 && cfg->IsSysTip);
    manager.SyncEquipSlotLevel(nullptr); assert(!MMOUI::notifications);
    UINT16 levels[NES_TOTAL]; for (auto& level : levels) level = 65535;
    manager.SyncEquipSlotLevel(levels); assert(MMOUI::notifications == 1);
    assert(manager.GetEnhanceActiveLevel() == 6553);
    const auto& extreme = manager.GetEquipSlotEnhanceActiveTemplate(manager.GetEnhanceActiveLevel() - 1);
    assert(!extreme.level && !extreme.ActiveEffectList.Num());
    levels[0] = 20; manager.SyncEquipSlotLevel(levels);
    assert(manager.GetMyEquipSlotLevel(0) == 20 && manager.GetEnhanceActiveLevel() == 2);
    puts("Equipment-slot initialization, config records and array boundaries passed");
}
}
''')
    return "\n".join(parts)
