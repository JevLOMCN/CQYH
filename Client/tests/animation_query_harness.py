"""Actual two-sequence playback/query functions and actor query wrappers."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / path))

    parts = [r'''
namespace AnimationQueryTests {
using FName = INT;
const FName NAME_None = 0;
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    bool IsValidIndex(INT index) const { return index >= 0 && index < Num(); }
    T& operator()(INT index) { return values.at(index); }
    const T& operator()(INT index) const { return values.at(index); }
};
struct UAnimNode { virtual ~UAnimNode() {} };
template<class T> T* Cast(UAnimNode* node) { return dynamic_cast<T*>(node); }
struct AnimSequence {} sequence;
struct UAnimNodeSequence : UAnimNode {
    UBOOL bPlaying = TRUE, bCauseActorAnimEnd = TRUE, loop = FALSE;
    FName AnimSeqName = 123;
    FLOAT Rate = 1, CurrentTime = 1, playback_length = 4, time_left = 2;
    AnimSequence* AnimSeq = &sequence;
    BYTE axes[3] = {};
    INT stops = 0, replays = 0, plays = 0;
    void StopAnim() { ++stops; bPlaying = FALSE; }
    void ReplayAnim() { ++replays; }
    void SetAnim(FName name) { AnimSeqName = name; }
    void SetRootBoneAxisOption(BYTE x, BYTE y, BYTE z) { axes[0] = x; axes[1] = y; axes[2] = z; }
    void PlayAnim(UBOOL looping, FLOAT rate, FLOAT position) { ++plays; bPlaying = TRUE; loop = looping; Rate = rate; CurrentTime = position; }
    FLOAT GetTimeLeft() const { return time_left; }
    FLOAT GetAnimPlaybackLength() const { return playback_length; }
};
struct Child { UAnimNode* Anim = nullptr; FLOAT Weight = 0; };
struct UAnimNodeBlendTwoSequences {
    TArray<Child> Children;
    INT ActiveChildIdx = -1, ActiveAnimKey = 0;
    BYTE AnimLevel = 0;
    FLOAT target = -1, blend_time = -1;
    void SetBlendTarget(FLOAT value, FLOAT time) { target = value; blend_time = time; }
    FLOAT PlayAnimation(FName, INT, FLOAT, FLOAT, FLOAT, UBOOL, BYTE, BYTE, BYTE, UBOOL, float, BYTE);
    UBOOL StopAnimationByName(FName);
    UBOOL StopAnimationByKey(INT);
    void ReplayAnim();
    UBOOL IsPlayingAnimationByName(FName);
    UBOOL IsPlayingAnimationByKey(INT);
    UBOOL IsPlayAnyAnimation();
    void SetAnimRate(INT, FLOAT);
    UBOOL GetCurrentAnimInfo(INT&, BYTE&, FLOAT&, FName&);
};
''']
    node = clean("Src/AnimNodeBlendTwoSequences.cpp")
    for signature in ["FLOAT UAnimNodeBlendTwoSequences::PlayAnimation(",
                      "UBOOL UAnimNodeBlendTwoSequences::StopAnimationByName(",
                      "UBOOL UAnimNodeBlendTwoSequences::StopAnimationByKey(",
                      "void UAnimNodeBlendTwoSequences::ReplayAnim()",
                      "UBOOL UAnimNodeBlendTwoSequences::IsPlayingAnimationByName(",
                      "UBOOL UAnimNodeBlendTwoSequences::IsPlayingAnimationByKey(",
                      "UBOOL UAnimNodeBlendTwoSequences::IsPlayAnyAnimation()",
                      "void UAnimNodeBlendTwoSequences::SetAnimRate(",
                      "UBOOL UAnimNodeBlendTwoSequences::GetCurrentAnimInfo("]:
        parts.append(function(node, signature))
    parts.append(r'''
struct LoopBlend { TArray<Child> Children; FLOAT Child2WeightTarget = 1; };
struct UMMOActorAT {
    LoopBlend* ActionLoopBlend = nullptr;
    UAnimNodeBlendTwoSequences *ActionBlend = nullptr, *IdleLoopBlend = nullptr;
    UBOOL GetCurrentAnimInfo(INT&, BYTE&, FLOAT&, FName&);
};
struct AMMOActor {
    UMMOActorAT* ActorAT = nullptr;
    UBOOL GetCurrentAnimName(FName&);
};
''')
    parts.append(function(clean("Src/AMMOActorAT.cpp"), "UBOOL UMMOActorAT::GetCurrentAnimInfo("))
    parts.append(function(clean("Src/MMOActor.cpp"), "UBOOL AMMOActor::GetCurrentAnimName("))
    parts.append(r'''
template<class Query> void CheckQuery(Query&& query, bool expected, FLOAT percent = 0.25f) {
    INT key = -123; BYTE level = 255; FLOAT progress = -456; FName name = -789;
    assert(!!query(key, level, progress, name) == expected);
    if (expected) assert(key == 17 && level == 4 && progress == percent && name == 123);
    else assert(!key && !level && !progress && name == NAME_None);
}
FLOAT Play(UAnimNodeBlendTwoSequences& node) {
    return node.PlayAnimation(123, 17, 1.25f, 0.2f, 0.3f, TRUE, 1, 2, 3, FALSE, 0.75f, 4);
}
void NodeTests() {
    for (INT count : {0, 1, 2, 3}) {
        UAnimNodeBlendTwoSequences node;
        UAnimNodeSequence sequences[3];
        for (INT i = 0; i < count; ++i) node.Children.values.push_back({&sequences[i], 0});
        node.ActiveAnimKey = 17; node.AnimLevel = 4;
        for (INT index : {(-2147483647 - 1), -2, -1, 0, 1, 2, count, 2147483647}) {
            node.ActiveChildIdx = index;
            bool valid = index >= 0 && index < count;
            assert(!!node.IsPlayingAnimationByName(123) == valid);
            assert(!!node.IsPlayingAnimationByKey(17) == valid);
            assert(!!node.IsPlayAnyAnimation() == valid);
            CheckQuery([&](auto&... out) { return node.GetCurrentAnimInfo(out...); }, valid);
        }
        node.ActiveChildIdx = 0;
        if (count < 2) assert(!Play(node));
        else { node.ActiveChildIdx = -2; assert(!Play(node)); node.ActiveChildIdx = 2; assert(!Play(node)); }
        node.ReplayAnim();
        for (INT i = 0; i < count; ++i) assert(sequences[i].replays == (i < 2 ? 1 : 0));
        assert(!!node.StopAnimationByName(123) == (count > 0));
        assert(!!node.StopAnimationByKey(17) == (count > 1));
        for (INT i = 0; i < count; ++i) assert(sequences[i].stops == (i < 2 ? 1 : 0));
    }
    UAnimNode wrong_type;
    UAnimNodeBlendTwoSequences node;
    node.Children.values = {{nullptr, 0}, {&wrong_type, 0}};
    node.ActiveAnimKey = 17; node.AnimLevel = 4;
    for (INT index : {0, 1}) {
        node.ActiveChildIdx = index;
        assert(!node.IsPlayAnyAnimation() && !node.IsPlayingAnimationByName(123) && !node.IsPlayingAnimationByKey(17));
        CheckQuery([&](auto&... out) { return node.GetCurrentAnimInfo(out...); }, false);
        assert(!Play(node));
    }
    node.ReplayAnim(); assert(!node.StopAnimationByName(123) && !node.StopAnimationByKey(17));
    UAnimNodeSequence first, second;
    node.Children.values = {{&first, 0}, {&second, 0}};
    node.ActiveChildIdx = 0;
    assert(Play(node) == 2 && node.ActiveChildIdx == 1 && node.target == 1 && node.blend_time == 0.2f);
    assert(first.bCauseActorAnimEnd == FALSE && second.bCauseActorAnimEnd && second.plays == 1);
    assert(second.loop && second.Rate == 1.25f && second.CurrentTime == 0.75f);
    assert(second.axes[0] == 1 && second.axes[1] == 2 && second.axes[2] == 3);
    CheckQuery([&](auto&... out) { return node.GetCurrentAnimInfo(out...); }, true, 0.1875f);
    first.AnimSeq = nullptr;
    assert(Play(node) == 2 && node.ActiveChildIdx == 0 && node.target == 0 && first.CurrentTime == 0);
    first.playback_length = 0;
    CheckQuery([&](auto&... out) { return node.GetCurrentAnimInfo(out...); }, true, 1);
    first.bPlaying = FALSE;
    CheckQuery([&](auto&... out) { return node.GetCurrentAnimInfo(out...); }, false);
    node.ActiveChildIdx = -1;
    assert(Play(node) == 2 && node.ActiveChildIdx == 0);
    node.SetAnimRate(17, 2); assert(first.Rate == 2 && second.Rate == 2);
    node.SetAnimRate(99, 3); assert(first.Rate == 2 && second.Rate == 2);
    puts("Two-sequence animation playback/queries reject missing, invalid and wrong-type children and initialize failure outputs");
}
void WrapperTests() {
    UMMOActorAT controller; AMMOActor actor;
    FName name = 999;
    assert(!actor.GetCurrentAnimName(name) && name == NAME_None);
    actor.ActorAT = &controller;
    LoopBlend loop;
    UAnimNodeBlendTwoSequences blend;
    UAnimNodeSequence sequence;
    blend.Children.values = {{&sequence, 0}}; blend.ActiveChildIdx = 0; blend.ActiveAnimKey = 17; blend.AnimLevel = 4;
    auto query = [&](auto&... out) { return controller.GetCurrentAnimInfo(out...); };
    CheckQuery(query, false);
    controller.ActionLoopBlend = &loop; CheckQuery(query, false);
    controller.ActionBlend = &blend;
    for (INT count : {0, 1, 2, 3}) {
        loop.Children.values.assign(count, Child{nullptr, 0});
        CheckQuery(query, count == 2);
        name = 999;
        assert(!actor.GetCurrentAnimName(name) && name == NAME_None); // No idle node.
    }
    loop.Children.values.assign(2, Child{nullptr, 0});
    loop.Children(1).Weight = 1;
    name = 999; assert(actor.GetCurrentAnimName(name) && name == 123);
    loop.Child2WeightTarget = 0;
    CheckQuery(query, false);
    controller.ActionBlend = nullptr;
    name = 999; assert(!actor.GetCurrentAnimName(name) && name == NAME_None);
    controller.IdleLoopBlend = &blend; loop.Children(1).Weight = 0;
    name = 999; assert(actor.GetCurrentAnimName(name) && name == 123);
    sequence.bPlaying = FALSE;
    name = 999; assert(!actor.GetCurrentAnimName(name) && name == NAME_None);
    puts("Actor animation wrappers define failure outputs and retain active/idle successful selection");
}
void Run() { NodeTests(); WrapperTests(); }
}
''')
    script = clean("Classes/AnimNodeBlendTwoSequences.uc")
    for signature in ["function StopAllAnimation()", "function ShowPlayStatus()"]:
        assert "i < 2 && i < Children.Length" in function(script, signature)
    return "\n".join(parts)
