"""Production VIP member construction, invalid privilege indices and description outputs."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "Inc/MMOVipManager.h"))
    cpp = clean(source(game / "Src/MMOVipManager.cpp"))
    parts = [r'''
namespace VipDefaultTests {
using FString = AttributeRecordTests::FString;
template<class T> using TArray = SkillQuestTests::TArray<T>;
template<class T> using SingletonMeyers = SkillQuestTests::SingletonMeyers<T>;
using INativeGameEventHandler = SkillQuestTests::INativeGameEventHandler;
using MMOGameEvent = SkillQuestTests::MMOGameEvent;
struct SYSTEMTIME { UINT32 value; };
void FakeLocalTime(SYSTEMTIME* output) { output->value = 1000; }
FString appItoa(INT value) { return std::to_string(value); }
struct MMOFacade : SingletonMeyers<MMOFacade> {
    UINT32 ConvertSystemTimeToTmTSecond(SYSTEMTIME time) { return time.value; }
    INT32 GetClientToSceneSecondDiff() { return 0; }
};
namespace MMOUI { FString ParseLocalizedPropertyPath(const FString& path) { return path; } }
''']
    for name in ["eVipType", "eVipShopType", "eGameTimeHintType"]:
        parts.append(function(header, "enum " + name + "\n") + ";")
    parts.append(function(header, "struct SVIPTimeLimit\n") + ";")
    parts.append(function(header, "class MMOVipManager :").replace("private:", "public:") + ";")
    for signature in ["MMOVipManager::MMOVipManager()", "MMOVipManager::~MMOVipManager()",
                      "void MMOVipManager::SetMyLeftGameTime(", "UINT32 MMOVipManager::GetMyVipLeftSecond(",
                      "eVipType MMOVipManager::GetMyCurrentVipType()", "INT MMOVipManager::GetVipItemID(",
                      "const TArray<FString> &MMOVipManager::GetVipPrivilege(",
                      "void MMOVipManager::GetVipDescriptionString("]:
        parts.append(function(cpp, signature).replace("::GetLocalTime(", "FakeLocalTime("))
    parts.append(r'''
void Run() {
    TipsStallTests::Poisoned<SVIPTimeLimit>([](auto& value) { assert(!value._ExpireTime); });
    TipsStallTests::Poisoned<MMOVipManager>([](auto& manager) {
        assert(!manager._VipShopId && !manager._MySelectedRoleTotalTime && !manager._LastSelectedRoleTime);
        assert(!manager._MyCurGameMode && !manager._LastSelectedRole && !manager._MyInGameDayCnt);
        assert(!manager._MyLeftGameTime && manager._MyLeftGameTimeUpdateTime == 1000);
        assert(!manager._VipItemID[eVIPTyp_None]);
        assert(manager.GetMyCurrentVipType() == eVIPTyp_None);
        for (auto& time : manager._MyVipTime) assert(!time._ExpireTime);
        for (int invalid : {-2147483647 - 1, -1, int(eVIPTyp_LVMax), MAXINT}) {
            assert(!manager.GetVipPrivilege((eVipType)invalid).Num());
            assert(!manager.GetVipItemID(invalid) && !manager.GetMyVipLeftSecond(invalid));
        }
        for (int invalid : {-2147483647 - 1, -1, int(eVIPTyp_None), int(eVIPTyp_LVMax), MAXINT}) {
            FString name = "old"; UINT days = 123, hours = 456;
            manager.GetVipDescriptionString(invalid, name, days, hours);
            assert(name.empty() && !days && !hours);
        }
        for (int type = eVIPTyp_LV1; type < eVIPTyp_LVMax; ++type) {
            assert(manager.GetVipItemID(type) == 2100 + type);
            assert(!manager.GetMyVipLeftSecond(type));
            manager._VipPrivilege[type].AddItem("privilege");
            assert(manager.GetVipPrivilege((eVipType)type).Num() == 1);
        }
        manager._MyVipTime[eVIPTyp_LV2]._ExpireTime = 1000 + 90000;
        assert(manager.GetMyCurrentVipType() == eVIPTyp_LV2);
        FString name; UINT days = 123, hours = 456;
        manager.GetVipDescriptionString(eVIPTyp_LV2, name, days, hours);
        assert(!name.empty() && days == 1 && hours == 1);
    });
    puts("VIP expiry/member defaults, privilege index bounds and description output initialization passed");
}
}
''')
    return "\n".join(parts)
