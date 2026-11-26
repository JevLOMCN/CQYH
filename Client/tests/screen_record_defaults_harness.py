"""Exercise actual screen queue methods, arena bounds and record constructors."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    def record(path, name):
        text = clean(source(game / path))
        match = re.search(r"struct " + name + r"\b[^;{]*\{", text).group()
        return function(text, match) + ";"

    generated = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    screen_header = clean(source(game / "MMOScreenMsgMgr.h"))
    screen_cpp = clean(source(game / "MMOScreenMsgMgr.cpp"))
    arena_header = clean(source(game / "Src/MMOArenaManager.h"))
    parts = [r'''
namespace ScreenRecordDefaultsTests {
using FString = std::string;
struct FVector {
    float X, Y, Z;
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
};
enum EGameEvent { GameEvent_Splash };
struct MMOGameEvent { virtual EGameEvent GetEventType() const = 0; };
struct INativeGameEventHandler {};
template<class T> struct SingletonMeyers {};
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    T& operator()(INT i) { return values.at(i); }
    const T& operator()(INT i) const { return values.at(i); }
    void Push(const T& value) { values.push_back(value); }
    void Empty() { values.clear(); }
    void Remove(INT i, INT n) { values.erase(values.begin() + i, values.begin() + i + n); }
};
''', function(generated, "enum EScreenMsgType\n") + ";",
             record("MMOScreenMsgMgr.h", "ScreenMsg"),
             function(screen_header, "class MMOScreenMsgMgr:").replace("private:", "public:") + ";",
    ]
    parts.append(r'''
enum { EGameState_InGame = 1 };
namespace MMO {
    INT state = 0;
    INT GetCurrentGameState() { return state; }
}
MMOScreenMsgMgr* callback_manager = nullptr;
bool notify_on_hide = false;
bool queue_on_hide = false;
struct UMMOUIScreenPopUpBoxMsg {
    INT plays = 0, hides = 0, last_value = 0;
    void PlayScreenPopUpBoxMsg(INT value, EScreenMsgType) { ++plays; last_value = value; }
    void Show(BOOL visible) {
        assert(!visible); ++hides;
        if (notify_on_hide) callback_manager->NotifyEndPlayMsg();
        if (queue_on_hide) { queue_on_hide = false; callback_manager->PushMsg(EScnMsgType_Skill, 91); }
    }
} popup;
struct UMMOUIAchievementHintPanel {
    INT plays = 0, hides = 0, last_value = 0;
    void ShowAchievementHintPanel(INT value) { ++plays; last_value = value; }
    void Show(BOOL visible) {
        assert(!visible); ++hides;
        if (notify_on_hide) callback_manager->NotifyEndPlayMsg();
    }
} achievement;
namespace MMOUI {
    struct UI {
        INT Panel_FightingScreenHint_ScreenPopUpBoxMsg = 1, Panel_AchievementHint = 2;
    } ui;
    bool popup_available = true, achievement_available = true;
    UI* GetUIManager() { return &ui; }
    void* FindUIPanel(INT id) {
        if (id == 1) return popup_available ? &popup : nullptr;
        return achievement_available ? &achievement : nullptr;
    }
    void* FindUIPanelEx(INT id) { return FindUIPanel(id); }
}
''')
    for name in ["PushMsg", "NotifyEndPlayMsg", "TryPlayMsg", "ForceClearMsg"]:
        parts.append(function(screen_cpp, "void MMOScreenMsgMgr::" + name + "("))
    for path, name in [
        ("ActivityManager.h", "ActivityTeamMember"),
        ("Inc/MMONpcFactory.h", "NPCTemplateDataIndex"),
        ("Src/MMOArenaManager.h", "ArenaRankPrize"),
        ("Src/MMOArenaManager.h", "VSPKInfoPersonal"),
        ("Src/MMOArenaManager.h", "VSPKInfo"),
        ("Src/MMOGameEvent_Splash.h", "MMOGameEvent_Splash"),
        ("Src/MMOHeavenManager.h", "HeavenJustConvertData"),
        ("UITextParser.h", "ContextParameter"),
    ]:
        parts.append(record(path, name))
    parts.extend([
        function(clean(source(game / "MMOGuideData.h")), "enum GuideState\n") + ";",
        record("MMOGuideManager.h", "GuideRecord"),
        "struct NPCTemplateIndexDataProcessor {",
        function(clean(source(game / "Inc/MMONpcFactory.h")), "static NPCTemplateDataIndex* CreateDataObject("),
        "};",
        function(arena_header, "enum EVSSideIdx\n") + ";",
        "struct MMOArenaManager { VSPKInfo _VSOneOnOnePKInfo[EVSSide_Total]; INT GetVSSideUsedRecovery(INT);",
        function(arena_header, "VSPKInfo &GetVSSidePkInfo("),
        "};",
        function(clean(source(game / "Src/MMOArenaManager.cpp")), "INT MMOArenaManager::GetVSSideUsedRecovery("),
    ])
    # These obsolete protocol definitions are absent from the checked-in headers.
    # Model their POD bases only to exercise the actual derived constructors.
    parts.append(r'''
struct M2C_MASTER_ADD_APPLICANT { UINT32 id; char name[32]; };
struct MASTER_DATA { UINT32 id, level; char name[32]; };
''')
    parts.append(record("StudentManager.h", "ClientBriefStudentInfo"))
    parts.append(record("StudentDetailManager.h", "ClientBriefStudentDetailInfo"))
    parts.append(r'''
template<class T, class Check> void DefaultConstruct(Check check) {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(T) unsigned char bytes[sizeof(T)];
        memset(bytes, pattern, sizeof(bytes));
        T* value = new(bytes) T;
        check(*value);
        value->~T();
    }
}
void RecordTests() {
    DefaultConstruct<ScreenMsg>([](auto& v) { assert(v._Type == EScnMsgType_Invalid && !v._MsgValue); });
    DefaultConstruct<MMOScreenMsgMgr>([](auto& v) { assert(!v._IsPlaying && !v._Messages.Num()); });
    DefaultConstruct<ActivityTeamMember>([](auto& v) {
        assert(!v.PlayerId && !v.Vitality && !v.ActivityTimes && !v.MaxActivityTimes && !v.ClassId && !v.Level && v.Name.empty());
    });
    DefaultConstruct<NPCTemplateDataIndex>([](auto& v) { assert(!v.TemplateID && v.ObjectPath.empty() && v.DisplayName.empty()); });
    DefaultConstruct<ArenaRankPrize>([](auto& v) { assert(!v._RankMoneyNeed && !v._AwardPrizeID && v._Desc.empty()); });
    DefaultConstruct<VSPKInfoPersonal>([](auto& v) { assert(!v._ActorID && !v._UsedRecovery); });
    DefaultConstruct<VSPKInfo>([](auto& v) { assert(!v._SideIdx && !v._UsedRecovery && v._Name.empty() && !v._PerInfo._ActorID && !v._PerInfo._UsedRecovery); });
    DefaultConstruct<MMOGameEvent_Splash>([](auto& v) {
        assert(v.EvtType == MMOGameEvent_Splash::ESET_ShowSplashUI && v.SelectRoleIndex == -1 && !v.ShowSplashUI);
        assert(!v.PositionInfo.X && !v.PositionInfo.Y && !v.PositionInfo.Z && v.GetEventType() == GameEvent_Splash);
    });
    DefaultConstruct<HeavenJustConvertData>([](auto& v) { assert(!v.nID && !v.nTimesCount && !v.nCostDiscount); });
    DefaultConstruct<ContextParameter>([](auto& v) { assert(!v.IsInUse && v.ParameterString.empty()); });
    DefaultConstruct<GuideRecord>([](auto& v) { assert(!v.Id && v.State == GS_Unactivated); });
    DefaultConstruct<ClientBriefStudentInfo>([](auto& v) { assert(!v.IsSelected && !v.id); for (char c : v.name) assert(!c); });
    DefaultConstruct<ClientBriefStudentDetailInfo>([](auto& v) { assert(!v.IsSelected && !v.id && !v.level); for (char c : v.name) assert(!c); });
    for (bool flag : {false, true}) {
        auto* record = NPCTemplateIndexDataProcessor::CreateDataObject(123, flag);
        assert(flag && record->TemplateID == 123); delete record;
    }
    MMOArenaManager arena;
    arena._VSOneOnOnePKInfo[0]._UsedRecovery = 3;
    arena._VSOneOnOnePKInfo[1]._UsedRecovery = 9;
    assert(arena.GetVSSideUsedRecovery(0) == 3 && arena.GetVSSideUsedRecovery(1) == 9);
    for (INT index : {(-2147483647 - 1), -1, int(EVSSide_Total), 2147483647}) assert(arena.GetVSSideUsedRecovery(index) == -1);
}
void QueueTests() {
    MMOScreenMsgMgr queue;
    callback_manager = &queue;
    queue.ForceClearMsg();
    queue._IsPlaying = TRUE;
    queue.ForceClearMsg();
    assert(!queue._IsPlaying && !queue._Messages.Num());

    // Queue while loading, then skip a long run of messages with no UI panels.
    MMO::state = 0;
    for (INT i = 0; i < 10000; ++i) queue.PushMsg(i % 2 ? EScnMsgType_Skill : EScnMsgType_Achievement, i);
    MMOUI::popup_available = MMOUI::achievement_available = false;
    MMO::state = EGameState_InGame;
    queue.TryPlayMsg();
    assert(!queue._Messages.Num() && !queue._IsPlaying);

    // A missing popup must also be safe during explicit cleanup.
    MMO::state = 0;
    queue.PushMsg(EScnMsgType_Skill, 1);
    queue.PushMsg(EScnMsgType_Achievement, 2);
    queue.ForceClearMsg();
    assert(!queue._Messages.Num() && !queue._IsPlaying);

    MMOUI::popup_available = MMOUI::achievement_available = true;
    popup = {}; achievement = {};
    queue.PushMsg(EScnMsgType_MainSkillLevelUp, 3);
    queue.PushMsg(EScnMsgType_Achievement, 4);
    MMO::state = EGameState_InGame;
    queue.TryPlayMsg();
    assert(queue._IsPlaying && popup.plays == 1 && popup.last_value == 3);
    queue.NotifyEndPlayMsg();
    assert(queue._Messages.Num() == 1 && achievement.plays == 1 && achievement.last_value == 4);
    queue.NotifyEndPlayMsg();
    assert(!queue._Messages.Num() && !queue._IsPlaying);

    // Closing a panel may synchronously signal completion of its animation.
    MMO::state = 0;
    queue.PushMsg(EScnMsgType_MainSkillLevelUp, 5);
    queue.PushMsg(EScnMsgType_Achievement, 6);
    notify_on_hide = true;
    queue.ForceClearMsg();
    notify_on_hide = false;
    assert(popup.hides == 1 && achievement.hides == 1 && !queue._Messages.Num() && !queue._IsPlaying);

    // Preserve a fresh notification queued by a panel-close callback.
    queue.PushMsg(EScnMsgType_Skill, 7);
    queue_on_hide = true;
    queue.ForceClearMsg();
    assert(queue._Messages.Num() == 1 && queue._Messages(0)._MsgValue == 91);
    queue.ForceClearMsg();
    callback_manager = nullptr;
}
}
''')
    return "\n".join(parts)
