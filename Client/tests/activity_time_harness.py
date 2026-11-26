"""Production activity record constructors and date-conversion failure paths."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    activity = clean(source(game / "ActivityManager.h"))
    cpp = clean(source(game / "ActivityManager.cpp"))
    parts = [r'''
#pragma push_macro("MAKELONG")
#undef MAKELONG
namespace ActivityTimeTests {
struct FString {
    unsigned magic = 0x1234abcd;
    std::string value;
    FString(const char* text = "") : value(text) {}
    FString& operator=(const char* text) { value = text; return *this; }
    static FString Printf(const char* format, int a, int b, int c, int d) {
        char text[128]; snprintf(text, sizeof(text), format, a, b, c, d); return FString(text);
    }
};
template<class T> struct TArray { unsigned magic = 0x2468abcd; std::vector<T> data; };
bool fail_time = false, fail_local = false, fail_safe = false;
int make_calls = 0, fail_make = 0, local32_calls = 0, fail_local32 = 0;
time_t clock_value = 0;
tm shared_date = {};
time_t time(time_t* out) { if (out) *out = clock_value; return fail_time ? (time_t)-1 : clock_value; }
tm* localtime(const time_t* stamp) {
    if (fail_local) return nullptr;
    tm* date = ::localtime(stamp); if (!date) return nullptr;
    shared_date = *date; return &shared_date;
}
time_t FakeMktime(tm* date) {
    ++make_calls;
    if (make_calls == fail_make) return -1;
    time_t result = ::mktime(date);
    shared_date.tm_year = -999; return result;
}
int FakeLocalTimeSafe(tm* out, const time_t* stamp) {
    if (fail_safe) return 1;
    return ::localtime_s(out, stamp);
}
tm* _localtime32(const __time32_t*) {
    ++local32_calls;
    if (local32_calls == fail_local32) return nullptr;
    shared_date = {}; shared_date.tm_hour = local32_calls == 1 ? 10 : 12;
    shared_date.tm_min = local32_calls == 1 ? 20 : 30; return &shared_date;
}
''']
    parts.append(function(activity, "enum EActivityState") + ";")
    records = {}
    for header, names in [(activity, ["ActivityTime", "RewardBarItem", "ActivityReward", "ActivityData", "FBActivityData", "ActivityTarget"]),
                          (clean(source(game / "MMOGuildData.h")), ["GuildActivityData", "GuildActivityInfo"])]:
        for name in names:
            records[name] = function(header, "struct " + name + "\n")
            parts.append(records[name] + ";")
    parts.append(function(cpp, "static BOOL MakeActivityTime(").replace("mktime(&date)", "FakeMktime(&date)"))
    parts.append(function(cpp, "EActivityState ActivityTime::GetState()"))
    parts.append(r'''
struct GuildBaseInfo { UINT32 Id = 1; UINT16 level = 1; };
using GuildActivityInfoMap = std::map<UINT16, GuildActivityInfo>;
enum { GAS_NONE = 0 };
UINT32 MAKELONG(UINT16 low, UINT16 high) { return low | ((UINT32)high << 16); }
struct MMOGuildData {
    GuildBaseInfo base;
    GuildActivityInfoMap activities;
    GuildActivityData settings;
    bool available = true;
    static MMOGuildData& getSingleton() { static MMOGuildData value; return value; }
    GuildBaseInfo GetGuildBaseInfo() { return base; }
    GuildActivityInfoMap GetGuildActivityInfoMap() { return activities; }
    bool GetGuildActivityData(UINT32, GuildActivityData& out) { out = settings; return available; }
};
struct ActivityManager { void _GetGuildActivityTimeString(UINT16, FString&); };
''')
    parts.append(function(cpp, "void ActivityManager::_GetGuildActivityTimeString("))
    parts.append(function(clean(source(game.parent / "Core/Src/UnVcWin32.cpp")), "void appSecondsToLocalTime(")
                 .replace("localtime_s(&TimeStruct, &Time)", "FakeLocalTimeSafe(&TimeStruct, &Time)"))
    parts.append(re.search(r"#define MAX_NORMAL_QUEST_COUNT\s+[^\n]+",
                           clean(source(client / "Common/define/quest_def.h"))).group())
    parts.append(r'''
struct MMOQuestData {
    UINT32 _RepeatQuestScanTime = 12345;
    UINT32 GetQuestCompletedDate(INT32, INT32&, INT32&, INT32&, INT32&, INT32&, INT32&, INT32&) const;
};
''')
    parts.append(function(clean(source(game / "MMOQuestData.cpp")), "UINT32 MMOQuestData::GetQuestCompletedDate("))
    parts.append("void Run() {")
    for name, declaration in records.items():
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& value) {")
        fields = re.findall(r"(?m)^\s*(INT|UINT8|UINT16|UINT32|time_t|EActivityState|FString|TArray<[^>]+>|ActivityTime|ActivityReward)\s+(\w+)\s*(?:\[(\d+)\])?\s*;", declaration)
        for kind, field, count in fields:
            if count:
                parts.append("for (auto item : value." + field + ") assert(!item);")
            elif kind == "FString":
                parts.append("assert(value." + field + ".magic == 0x1234abcd && value." + field + ".value.empty());\n"
                             "value." + field + ".value.assign(2000, 'x');")
            elif kind.startswith("TArray"):
                parts.append("assert(value." + field + ".magic == 0x2468abcd && value." + field + ".data.empty());\n"
                             "value." + field + ".data.resize(3);")
            elif kind == "ActivityTime":
                parts.append("assert(!value." + field + ".startT && value." + field + ".sTimeStr.magic == 0x1234abcd);")
            elif kind == "ActivityReward":
                parts.append("assert(!value." + field + ".nRewardType && !value." + field + ".nRewardVitality);\n"
                             "for (int item : value." + field + ".nArrItemID) assert(!item);")
            else:
                parts.append("assert(!value." + field + ");")
        parts.append("});")
    parts.append(r'''
    tm now = {}; now.tm_year = 126; now.tm_mon = 10; now.tm_mday = 2; now.tm_hour = 12; now.tm_isdst = -1;
    clock_value = ::mktime(&now); assert(clock_value != (time_t)-1);
    ActivityTime unlimited; unlimited.startT = 123;
    fail_time = true; assert(unlimited.GetState() == EStarting && !unlimited.startT); fail_time = false;
    ActivityTime range; range.nStartTime_year = range.nEndTime_year = 2026;
    range.nStartTime_hour = 11; range.nEndTime_hour = 13;
    make_calls = 0; assert(range.GetState() == EStarting && range.startT > 0);
    range.nStartTime_hour = 13; range.nEndTime_hour = 14;
    make_calls = 0; assert(range.GetState() == ENOStart);
    range.nStartTime_hour = 9; range.nEndTime_hour = 10;
    make_calls = 0; assert(range.GetState() == EActivityOver);
    fail_time = true; assert(range.GetState() == EActivityOver && !range.startT); fail_time = false;
    fail_local = true; assert(range.GetState() == EActivityOver && !range.startT); fail_local = false;
    range.nStartTime_hour = 11; range.nEndTime_hour = 13;
    for (int failure : {1, 2}) {
        fail_make = failure; make_calls = 0; assert(range.GetState() == EActivityOver);
    }
    fail_make = 0;
    for (int invalid : {-2147483647 - 1, -1, 13, 2147483647}) {
        range.nStartTime_month = invalid; make_calls = 0;
        assert(range.GetState() == EActivityOver && !range.startT && !make_calls);
    }
    range.nStartTime_month = 0; range.nStartTime_year = -2147483647 - 1;
    assert(range.GetState() == EActivityOver && !range.startT);
    auto& guild = MMOGuildData::getSingleton();
    guild.activities[1].StartTime = 100; guild.activities[1].State = 1; guild.settings.Duration = 300;
    ActivityManager manager; FString text;
    local32_calls = 0; manager._GetGuildActivityTimeString(1, text); assert(text.value == "10:20-12:30");
    for (int failure : {1, 2}) {
        text.value = "old"; local32_calls = 0; fail_local32 = failure;
        manager._GetGuildActivityTimeString(1, text); assert(text.value.empty());
    }
    fail_local32 = 0;
    for (UINT32 stamp : {0x7fffffffu, 0x80000000u, 0xffffffffu}) {
        guild.activities[1].StartTime = stamp; text.value = "old"; local32_calls = 0;
        manager._GetGuildActivityTimeString(1, text); assert(text.value.empty() && !local32_calls);
    }
    INT year = -99, month = -99, week = -99, day = -99, hour = -99, minute = -99, second = -99;
    fail_safe = true;
    appSecondsToLocalTime(clock_value, year, month, week, day, hour, minute, second);
    assert(!year && !month && !week && !day && !hour && !minute && !second); fail_safe = false;
    appSecondsToLocalTime(clock_value, year, month, week, day, hour, minute, second);
    assert(year == 2026 && month == 11 && day == 2 && hour == 12);
    MMOQuestData quest;
    year = month = week = day = hour = minute = second = -99;
    assert(!quest.GetQuestCompletedDate(1, year, month, day, week, hour, minute, second));
    assert(!year && !month && !week && !day && !hour && !minute && !second);
    fail_local = true; year = month = week = day = hour = minute = second = -99;
    assert(!quest.GetQuestCompletedDate(MAX_NORMAL_QUEST_COUNT + 1, year, month, day, week, hour, minute, second));
    assert(!year && !month && !week && !day && !hour && !minute && !second); fail_local = false;
    quest._RepeatQuestScanTime = (UINT32)clock_value;
    assert(quest.GetQuestCompletedDate(MAX_NORMAL_QUEST_COUNT + 1, year, month, day, week, hour, minute, second) == quest._RepeatQuestScanTime);
    assert(year == 126 && month == 10 && day == 2 && hour == 12);
    puts("Activity/guild record defaults, failed date conversions, timestamp ranges and quest outputs passed");
}
}
#undef MAX_NORMAL_QUEST_COUNT
#pragma pop_macro("MAKELONG")
''')
    return "\n".join(parts)
