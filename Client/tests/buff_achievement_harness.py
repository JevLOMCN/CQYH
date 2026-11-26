"""Actual buff refresh with independent array capacities and achievement outputs."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / path))

    buff = clean("MMOBuffManager.cpp")
    refresh = function(buff, "void MMOBuffManager::RefreshSlotBuff(INT ObjId)")
    header = clean("MMOBuffManager.h")
    constants = sorted(set(re.findall(r"\b(?:EBuff_\w+|E_auet_\w+|EAT_FLAG_\w+|E_ac_\w+)\b", refresh)))
    parts = [r'''
namespace BuffAchievementTests {
using std::max;
using FString = std::string;
using STLIntVector = std::vector<INT>;
struct IdNode { STLIntVector Ids; };
using STLIdNodeVector = std::vector<IdNode>;
''', "enum { " + ", ".join(name + " = " + str(i + 1) for i, name in enumerate(constants)) + " };",
             "enum { " + ", ".join(name + " = " + re.search(r"#define " + name + r"\s+(\d+)", header).group(1)
                                        for name in ["MAXBUFFSLOT", "MAXBUFFSLOTEX"]) + " };", r'''
struct UBuff {
    UBOOL bShowIcon = TRUE, bShowStackCount = TRUE, bShowTimeUpTips = FALSE;
    INT bBuffType = EBuff_Normal, Type = 0, TemplateID = 0;
    BYTE Category = E_ac_None;
};
''', function(clean("AbilityProcessor.h"), "struct BuffData\n") + ";", r'''
using STLInt2BuffDataMap = std::map<INT, BuffData*>;
struct BuffDataMap { STLInt2BuffDataMap Buffs; };
using STLInt2BuffDataMapMap = std::map<INT, BuffDataMap*>;
struct Player {
    INT ServerID = 7;
    UBOOL stunned = FALSE;
    void eventSetStun(UBOOL value) { stunned = value; }
};
struct MMOFacade {
    Player player;
    bool present = true;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    Player* GetMainPlayer() { return present ? &player : nullptr; }
};
struct Mir2Attack {
    std::set<INT> flags;
    static Mir2Attack& getSingleton() { static Mir2Attack value; return value; }
    void ClearAbilityFlag() { flags.clear(); }
    void SetAbilityFlag(INT flag) { flags.insert(flag); }
};
UBOOL g_bGMBuff = FALSE;
namespace MMOUI {
struct Manager {
    INT normal = 0, functional = 0, deleted = 0, deleted_functional = 0, enabled = 0;
    void delegateBuffEnableAbility() { ++enabled; }
    template<class... Args> void delegateAddBuffPanel(INT index, Args...) { assert(index >= 0 && index < MAXBUFFSLOT); ++normal; }
    template<class... Args> void delegateAddFunctionBuffPanel(INT index, Args...) { assert(index >= 0 && index < MAXBUFFSLOTEX); ++functional; }
    void delegateDeleteBuffPanel(INT index) { assert(index >= 0 && index < MAXBUFFSLOT); ++deleted; }
    void delegateDeleteFunctionBuffPanel(INT index) { assert(index >= 0 && index < MAXBUFFSLOTEX); ++deleted_functional; }
};
Manager* GetUIManager() { static Manager value; return &value; }
}
struct MMOBuffManager {
    STLInt2BuffDataMapMap ActiveBuffsMap;
    BuffData* ShowBuffs[MAXBUFFSLOT] = {};
    UINT64 guard_normal = 0x123456789abcdef0ULL;
    BuffData* ShowBuffsEx[MAXBUFFSLOTEX] = {};
    UINT64 guard_function = 0x23456789abcdef01ULL;
    BuffData* ShowTargetBuffs[MAXBUFFSLOT] = {};
    void RefreshSlotBuff(INT);
};
''', refresh, r'''
void BuffTests() {
    for (INT normal : {0, 1, int(MAXBUFFSLOT), MAXBUFFSLOT + 1, 1000}) {
        for (INT functional : {0, 1, int(MAXBUFFSLOTEX), MAXBUFFSLOTEX + 1, 1000}) {
            for (bool functions_first : {false, true}) {
                MMOBuffManager manager;
                BuffDataMap map;
                manager.ActiveBuffsMap[7] = &map;
                std::vector<BuffData> data(normal + functional + 4);
                std::vector<UBuff> templates(data.size());
                INT key = 0;
                auto add = [&](INT count, INT kind) {
                    for (INT i = 0; i < count; ++i, ++key) {
                        data[key].ObjId = 7; data[key].Buff = &templates[key];
                        templates[key].bBuffType = kind; templates[key].Category = E_ac_Negative;
                        templates[key].bShowStackCount = (i % 2) == 0;
                        map.Buffs[key] = &data[key];
                    }
                };
                if (functions_first) { add(functional, EBuff_Function); add(normal, EBuff_Normal); }
                else { add(normal, EBuff_Normal); add(functional, EBuff_Function); }
                // Hidden entries beyond both display capacities must still
                // contribute stun, GM and every combat-ability flag.
                for (INT type : {E_auet_Type01, E_auet_Type09, E_auet_Type15, E_auet_Type18}) {
                    data[key].ObjId = 7; data[key].Buff = &templates[key];
                    templates[key].bShowIcon = FALSE; templates[key].Type = type;
                    templates[key].TemplateID = 2516; map.Buffs[key] = &data[key]; ++key;
                }
                *MMOUI::GetUIManager() = {};
                manager.RefreshSlotBuff(7);
                const auto& ui = *MMOUI::GetUIManager();
                assert(ui.normal == std::min(normal, int(MAXBUFFSLOT)));
                assert(ui.functional == std::min(functional, int(MAXBUFFSLOTEX)));
                assert(ui.deleted + ui.normal == MAXBUFFSLOT && ui.deleted_functional + ui.functional == MAXBUFFSLOTEX);
                assert(ui.enabled == 1 && MMOFacade::getSingleton().player.stunned && g_bGMBuff);
                assert(manager.guard_normal == 0x123456789abcdef0ULL && manager.guard_function == 0x23456789abcdef01ULL);
                assert(Mir2Attack::getSingleton().flags.count(EAT_FLAG_YINSHEN));
                assert(Mir2Attack::getSingleton().flags.count(EAT_FLAG_MOFADUN));
                assert(Mir2Attack::getSingleton().flags.count(EAT_FLAG_CISHA));
                for (auto* value : manager.ShowTargetBuffs) assert(!value);
                // Refreshing an empty set must clear previously filled slots.
                map.Buffs.clear(); manager.RefreshSlotBuff(7);
                for (auto* value : manager.ShowBuffs) assert(!value);
                for (auto* value : manager.ShowBuffsEx) assert(!value);
                assert(!MMOFacade::getSingleton().player.stunned && !g_bGMBuff && Mir2Attack::getSingleton().flags.empty());
                INT enabled = MMOUI::GetUIManager()->enabled;
                manager.RefreshSlotBuff(8); assert(MMOUI::GetUIManager()->enabled == enabled);
                MMOFacade::getSingleton().present = false;
                manager.RefreshSlotBuff(7); assert(MMOUI::GetUIManager()->enabled == enabled);
                MMOFacade::getSingleton().present = true;
            }
        }
    }
    puts("Normal/function buff arrays independently capped across 50 overflow/order cases; hidden combat flags and stale-slot cleanup preserved");
}
''']
    achievement = clean("MMOAchievementData.h")
    parts.append("enum { MAX_RECENT_COMPLETE_COUNT = " + re.search(r"MAX_RECENT_COMPLETE_COUNT\s*=\s*(\d+)", achievement).group(1) + " };")
    parts.append(function(achievement, "struct AchievementInfo\n") + ";")
    parts.append(r'''
using AchievementIdList = std::vector<UINT32>;
struct MMOAchievementData {
    AchievementIdList _AchievementCompleteList;
    std::map<UINT32, AchievementInfo> records;
    UBOOL GetAchievementInfo(UINT32 id, AchievementInfo& out) {
        auto found = records.find(id); if (found == records.end()) return FALSE;
        out = found->second; return TRUE;
    }
    void GetAchievementRecentComplete(INT (&)[MAX_RECENT_COMPLETE_COUNT], INT (&)[MAX_RECENT_COMPLETE_COUNT]);
};
''')
    parts.append(function(clean("MMOAchievementData.cpp"), "void MMOAchievementData::GetAchievementRecentComplete("))
    parts.append(r'''
void AchievementTests() {
    for (INT count : {0, 1, int(MAX_RECENT_COMPLETE_COUNT), MAX_RECENT_COMPLETE_COUNT + 3}) {
        MMOAchievementData manager;
        manager._AchievementCompleteList.push_back(9999); // Missing rows do not occupy an output slot.
        for (INT i = 0; i < count; ++i) {
            AchievementInfo info; info.Id = i + 1; info.Date = i + 101;
            manager.records[info.Id] = info; manager._AchievementCompleteList.push_back(info.Id);
        }
        struct Guarded { UINT32 before; INT data[MAX_RECENT_COMPLETE_COUNT]; UINT32 after; } ids, dates;
        memset(&ids, 0xa5, sizeof(ids)); memset(&dates, 0x5a, sizeof(dates));
        manager.GetAchievementRecentComplete(ids.data, dates.data);
        for (INT i = 0; i < MAX_RECENT_COMPLETE_COUNT; ++i) {
            assert(ids.data[i] == (i < count ? i + 1 : 0));
            assert(dates.data[i] == (i < count ? i + 101 : 0));
        }
        assert(ids.before == 0xa5a5a5a5 && ids.after == 0xa5a5a5a5);
        assert(dates.before == 0x5a5a5a5a && dates.after == 0x5a5a5a5a);
    }
    puts("Recent-achievement IDs and dates initialize all output slots for empty/missing/short/full/oversized lists");
}
void Run() { BuffTests(); AchievementTests(); }
}
''')
    return "\n".join(parts)
