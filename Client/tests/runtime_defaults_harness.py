"""Actual performance entry behavior and remaining runtime constructor defaults."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(path))

    def definition(path, name, kind="class"):
        text = clean(game / path)
        match = re.search(r"\b" + kind + r"\s+" + name + r"\b[^;{]*\{", text).group()
        return function(text, match).replace("private:", "public:") + ";"

    performance = clean(game / "Inc/MMOPerformance.h")
    parts = [r'''
namespace RuntimeDefaultTests {
using ScreenRecordDefaultsTests::DefaultConstruct;
using FString = std::string;
using FName = INT;
using FVector = ::FVector;
struct FRotator {
    INT Pitch, Yaw, Roll;
    FRotator() {}
    FRotator(INT p, INT y, INT r) : Pitch(p), Yaw(y), Roll(r) {}
};
template<class T> struct SingletonMeyers {};
struct INativeGameEventHandler {};
enum EForceInit { E_ForceInit };
enum ENoInit { E_NoInit };
// Model the engine's intentional no-init base constructor, rather than silently
// replacing TArrayNoInit with an always-initialized standard container.
template<class T> struct TArray {
    T* data;
    INT count, capacity;
    TArray() : data(nullptr), count(0), capacity(0) {}
    TArray(ENoInit) {}
    INT Num() const { return count; }
    ~TArray() { assert(!data && !count && !capacity); }
};
''']
    array_header = clean(game.parent / "Core/Inc/Array.h")
    parts.append("template<class T> " + function(array_header, "class TArrayNoInit :") + ";")
    parts.append(function(performance, "enum eBottleNeckEntry\n") + ";")
    parts.append(re.search(r"typedef INT \(\*fnEntryAdjustInterface\)[^;]+;", performance).group())
    parts.append(function(performance, "class EntryData\n") + ";")
    parts.append(definition("Inc/MMOUIBackDecoratedMgr.h", "BackDecoratedUpgradeCfg", "struct"))
    parts.append(function(clean(game / "Src/MMOUIBackDecoratedMgr.cpp"), "BackDecoratedUpgradeCfg::BackDecoratedUpgradeCfg()"))
    parts.append(definition("Inc/LoginBase.h", "LoginBase"))
    for name in ["LoginBase::LoginBase()", "LoginBase::~LoginBase()"]:
        parts.append(function(clean(game / "Src/LoginBase.cpp"), name))
    parts.append(definition("Inc/MMOShopManager.h", "MMOShopManager"))
    for name in ["MMOShopManager::MMOShopManager()", "MMOShopManager::~MMOShopManager()"]:
        parts.append(function(clean(game / "Src/MMOShopManager.cpp"), name))
    parts.append("void MMOShopManager::OnNativeGameEvent(MMOGameEvent*) {}")
    parts.append(definition("MMOBossBloodManager.h", "MMOBossBloodManager"))
    parts.append(function(clean(game / "MMOBossBloodManager.cpp"), "INT MMOBossBloodManager::GetBossID()"))
    parts.append("void MMOBossBloodManager::OnNativeGameEvent(MMOGameEvent*) {}")
    parts.append("struct IFSMCtrl { virtual UBOOL Update(FLOAT) = 0; virtual void Action() = 0; };")
    for file, name in [("Src/FSMCtrl_AI.h", "FSMCtrl_ShowHintBtn"), ("Src/FSMCtrl_Behavior.h", "FSMCtrl_Test")]:
        parts.append(definition(file, name))
        parts.append("UBOOL " + name + "::Update(FLOAT) { return FALSE; } void " + name + "::Action() {}")
    parts.append(r'''
INT calls = 0, last_request = -1;
INT AdjustValue(INT value) { ++calls; last_request = value; return value; }
void PerformanceTests() {
    DefaultConstruct<EntryData>([](auto& entry) {
        assert(entry.eBottleNeckDetail == BN_COUNT && !entry.pfnAdjust);
        assert(entry.nCurValue == -1 && entry.nDefaultValue == -1 && entry.nDesireValue == -1);
        entry.Apply(); entry.Restore(); entry.FetchValue();
    });
    assert(!calls);
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(EntryData) unsigned char bytes[sizeof(EntryData)];
        memset(bytes, pattern, sizeof(bytes));
        auto* entry = new(bytes) EntryData(SKELETAL_UPDATE, AdjustValue);
        INT initial_calls = calls;
        // Unselected entries are still visited by the real balancer's Apply loop.
        entry->Apply();
        assert(calls == initial_calls && entry->nDesireValue == -1);
        entry->Adjust(3); entry->Apply();
        assert(calls == initial_calls + 1 && last_request == 3 && entry->nCurValue == 3);
        entry->Apply();
        assert(calls == initial_calls + 1);
        entry->Init(BN_ALL, AdjustValue);
        entry->Apply();
        assert(calls == initial_calls + 1 && entry->nDesireValue == -1);
        entry->Adjust(5); entry->Apply();
        assert(calls == initial_calls + 2 && last_request == 5);
        entry->~EntryData();
    }
    puts("Performance entries skip unset requests, apply explicit values and reset reused entries safely");
}
void RecordTests() {
    DefaultConstruct<BackDecoratedUpgradeCfg>([](auto& v) {
        assert(v._ToUpgradeItemID == -1 && !v._ToUpgradeItemLv && v._ConsumptionItemID == -1);
        assert(!v._ConsumptionItemCnt && !v._SuccessRate && !v._UpgradeMnyCosts && v._UpgradedItemID == -1);
    });
    DefaultConstruct<LoginBase>([](auto& v) {
        assert(!v.PlayerStartRotation.Pitch && !v.PlayerStartRotation.Yaw && !v.PlayerStartRotation.Roll);
        assert(!v.PlayerStartPostion.X && !v.PlayerStartPostion.Y && !v.PlayerStartPostion.Z);
        assert(!v.HiddenPostion.X && !v.HiddenPostion.Y && !v.HiddenPostion.Z);
        assert(!v.SelectCameraNormalPostion.X && !v.SelectCameraNormalPostion.Y && !v.SelectCameraNormalPostion.Z);
        assert(!v.CreateShowActors.Num() && !v.CreateShowActors.data && !v.CreateShowActors.capacity);
    });
    DefaultConstruct<MMOShopManager>([](auto& v) { assert(v.getShopId() == -1); });
    DefaultConstruct<MMOBossBloodManager>([](auto& v) { assert(!v.GetBossID()); });
    DefaultConstruct<FSMCtrl_ShowHintBtn>([](auto& v) { assert(!v._bShowHintBtn); });
    DefaultConstruct<FSMCtrl_Test>([](auto& v) { assert(!v.m_DelTime); });
    puts("Upgrade/shop/boss/controller defaults and login transforms/force-initialized actor arrays passed");
}
}
''')
    return "\n".join(parts)
