"""Actual animation and PvP paths that formerly read raw TArray::Add storage."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(game / path))

    parts = [r'''
#pragma push_macro("TEXT")
#undef TEXT
#define TEXT(x) x
namespace ArrayConstructionTests {
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    bool IsValidIndex(INT i) const { return i >= 0 && i < Num(); }
    T& operator()(INT i) { assert(IsValidIndex(i)); return values[i]; }
    void Add(INT count) {
        assert(count >= 0); size_t old = values.size(); values.resize(old + count);
        if (count) memset(values.data() + old, 0xcd, count * sizeof(T));
    }
    void AddItem(const T& value) { values.push_back(value); }
    void Empty() { values.clear(); }
};
enum EName { NAME_None };
struct FString : std::string {
    using std::string::string;
    FString(const std::string& value) : std::string(value) {}
    INT Len() const { return (INT)size(); }
};
FString appItoa(INT value) { return std::to_string(value); }
struct FName {
    INT id;
    UINT magic;
    FName(EName value = NAME_None) : id(value), magic(0x1234abcd) {}
    FName(const char* value) : id(!strcmp(value, "Root") ? 1 : !strcmp(value, "Spine") ? 2 : !strcmp(value, "Neck") ? 3 : 4), magic(0x1234abcd) {}
    bool operator!=(const FName& other) const { assert(magic == 0x1234abcd && other.magic == magic); return id != other.id; }
    FString GetNameString() const { assert(magic == 0x1234abcd); return id ? "name" : ""; }
};
struct FMMOAnimGroup { FName RootBoneName{"Root"}, SpineBoneName{"Spine"}; };
enum { EABT_FullBody, EABT_UpperBody, EABT_Head, EABT_LeftFoot, EABT_RightFoot, EABT_Tail };
struct Actor {
    FMMOAnimGroup PawnAnimInfo;
    struct VelocityType { FLOAT speed = 2; FLOAT SizeSquared2D() const { return speed; } } Velocity;
};
struct MeshType {
    bool hasSequence = true;
    void* FindAnimSequence(FName) { return hasSequence ? this : nullptr; }
};
struct Blend {
    FLOAT length = 3, target = -1, time = -1;
    INT refreshes = 0;
    TArray<FName> BranchStartBoneName;
    template<class... Args> FLOAT PlayAnimation(Args...) { return length; }
    void RefreshWeightList() { ++refreshes; }
    void SetBlendTarget(FLOAT value, FLOAT seconds) { target = value; time = seconds; }
};
struct RateNode { FLOAT ScaleByValue = -1; };
struct UMMOActorAT {
    Actor* actor = nullptr;
    MeshType* Mesh = nullptr;
    Blend* ActionBlend = nullptr;
    Blend* ActionLoopBlend = nullptr;
    RateNode* ActionAnimRateNode = nullptr;
    FLOAT ActionPlayedLeftTime = -1, AnimBlendInTime = 0.3f;
    FLOAT PlayAnimation(FName, INT, FLOAT, UBOOL, BYTE, BYTE, BYTE, BYTE, FLOAT, FLOAT, FLOAT, BYTE);
};
''', function(clean('Src/AMMOActorAT.cpp'), 'FLOAT UMMOActorAT::PlayAnimation('), r'''
void AnimationTests() {
    UMMOActorAT animation;
    assert(animation.PlayAnimation("Idle", 1, 2, FALSE, 0, 0, 0, EABT_FullBody, 0, 0, 0, 0) == 0);
    MeshType mesh; Actor actor; Blend action, loop; RateNode rate;
    animation.Mesh = &mesh; animation.ActionBlend = &action; animation.ActionLoopBlend = &loop;
    animation.ActionAnimRateNode = &rate;
    for (INT mode : {INT(EABT_FullBody), INT(EABT_UpperBody), INT(EABT_Head), INT(EABT_Tail)}) {
        for (bool noRoot : {false, true}) {
            loop.BranchStartBoneName.Empty(); loop.refreshes = 0;
            actor.PawnAnimInfo.RootBoneName = noRoot ? FName(NAME_None) : FName("Root");
            actor.PawnAnimInfo.SpineBoneName = noRoot ? FName(NAME_None) : FName("Spine");
            animation.actor = &actor;
            assert(animation.PlayAnimation("Idle", 1, 2, FALSE, 0, 0, 0, BYTE(mode), 0, 0, 0, 0) == 3);
            INT expected = mode == EABT_Head ? 3 : noRoot ? 0 : mode == EABT_UpperBody ? 2 : 1;
            assert(loop.BranchStartBoneName.Num() == 1 && loop.BranchStartBoneName(0).id == expected);
            assert(loop.refreshes == (expected ? 1 : 0) && rate.ScaleByValue == 2);
            assert(loop.target == 1 && loop.time == animation.AnimBlendInTime);
            animation.PlayAnimation("Idle", 1, 2, FALSE, 0, 0, 0, BYTE(mode), 0, 0, 0, 0);
            assert(loop.refreshes == (expected ? 1 : 0));
        }
    }
    loop.BranchStartBoneName.Empty(); action.length = 0;
    animation.PlayAnimation("Idle", 1, 2, FALSE, 0, 0, 0, EABT_FullBody, 0, 0, 0, 0);
    assert(loop.BranchStartBoneName.Num() == 0);
}
struct UGFxObject {
    UINT magic = 0x98765432;
    bool selected = true, enabled = true;
    void SetBool(const char*, UBOOL value) { assert(magic == 0x98765432); enabled = value != 0; }
    void SetString(const char*, const FString&) { assert(magic == 0x98765432); }
    UBOOL GetBool(const char*) { assert(magic == 0x98765432); return selected; }
};
struct FScriptDelegate { template<class T> FScriptDelegate(T*, const char*) {} };
struct Movie {
    UGFxObject data;
    void AddPostAdvanceDelegate(FScriptDelegate) {}
    UGFxObject* CreateObject(const char*) { return &data; }
};
namespace MMOUI { FString ParseLocalizedPropertyPath(const char* key) { return key; } }
struct UMMOUIPVP {
    Movie* GFXPlayer = nullptr;
    TArray<UGFxObject*> ArenaMode, ArenaSize;
    INT ArenaModeSelCount = 2, ArenaSizeSelCount = 2;
    UGFxObject widgets[2][4];
    UINT missing = 0;
    INT shows = 0, texts = 0;
    UGFxObject* GetObject(const FString& name) {
        INT group = name.find("ArenaMode") == 0 ? 0 : 1;
        INT index = atoi(name.c_str() + 9);
        assert(index >= 0 && index < 4);
        return missing & (1u << (group * 4 + index)) ? nullptr : &widgets[group][index];
    }
    void eventCallSetPanelText(UGFxObject* value) { assert(value); ++texts; }
    void Show(UBOOL show) { assert(!show); ++shows; }
    void Init();
    void ArenaEnter();
};
''']
    pvp = clean('Src/MMOUIPVP.cpp')
    parts.extend(function(pvp, signature) for signature in ['void UMMOUIPVP::Init(', 'void UMMOUIPVP::ArenaEnter('])
    parts.append(r'''
void PvpTests() {
    UMMOUIPVP panel; Movie movie;
    for (bool hasMovie : {false, true}) for (UINT missing : {0u, 0xffu, 0x55u}) {
        panel.GFXPlayer = hasMovie ? &movie : nullptr; panel.missing = missing;
        for (INT count : {4, 2, 1, 0, -1, 2}) {
            panel.ArenaModeSelCount = panel.ArenaSizeSelCount = count;
            panel.Init(); panel.Init();
            INT expected = count > 0 ? count : 0;
            assert(panel.ArenaMode.Num() == expected && panel.ArenaSize.Num() == expected);
            for (INT i = 0; i < expected; ++i) {
                assert(panel.ArenaMode(i) == (missing & (1u << i) ? nullptr : &panel.widgets[0][i]));
                assert(panel.ArenaSize(i) == (missing & (1u << (4 + i)) ? nullptr : &panel.widgets[1][i]));
            }
            if (expected > 1 && panel.ArenaSize(1)) assert(!panel.ArenaSize(1)->enabled);
            INT previous = panel.shows; panel.ArenaEnter(); assert(panel.shows == previous + 1);
        }
    }
}
void Run() {
    AnimationTests(); PvpTests();
    puts("Animation bone names construct before comparison; PvP reinitialization and missing/short widget arrays remain safe");
}
}
#pragma pop_macro("TEXT")
''')
    return '\n'.join(parts)
