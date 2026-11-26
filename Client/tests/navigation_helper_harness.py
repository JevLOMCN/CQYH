"""Actual navigation helper reuse, failed-query outputs and missing handles."""
import re


def harness(game, source, function):
    cpp = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / "Src/MMONavigationHelper.cpp"))
    parts = ['#pragma push_macro("SHIPPING_PC_GAME")\n#undef SHIPPING_PC_GAME']
    for shipping in [0, 1]:
        parts.append("#define SHIPPING_PC_GAME " + str(shipping) + "\nnamespace NavigationHelper" + str(shipping) + "Tests {")
        parts.append(r'''
enum EEventParm { EC_EventParm };
struct FVector {
    FLOAT X, Y, Z;
    FVector() : X(std::numeric_limits<FLOAT>::quiet_NaN()), Y(X), Z(X) {}
    FVector(FLOAT x, FLOAT y, FLOAT z) : X(x), Y(y), Z(z) {}
    explicit FVector(EEventParm) : X(0), Y(0), Z(0) {}
};
void Equal(FVector v, FLOAT x, FLOAT y, FLOAT z) { assert(v.X == x && v.Y == y && v.Z == z); }
bool g_drawPathDebug = false;
struct NavigationHandle {
    void* PathConstraintList = reinterpret_cast<void*>(1);
    void* PathGoalList = reinterpret_cast<void*>(2);
    UBOOL bUltraVerbosePathDebugging = FALSE, bVisualPathDebugging = FALSE;
    bool find_ok = true, next_ok = true, partial_failure = false, prepared = false;
    INT clears = 0, finds = 0, draws = 0, destinations = 0, moves = 0;
    FLOAT arrival = -1;
    FVector target{0, 0, 0};
    void ClearConstraints() { assert(!PathConstraintList && !PathGoalList); ++clears; }
    UBOOL FindPath() { assert(prepared); ++finds; return find_ok; }
    void DrawPathCache(FVector v, UBOOL draw) { Equal(v, 0, 0, 0); assert(draw); ++draws; }
    void SetFinalDestination(FVector v) { target = v; ++destinations; }
    UBOOL GetNextMoveLocation(FVector& aim, FLOAT distance) {
        Equal(aim, 0, 0, 0); ++moves; arrival = distance;
        if (next_ok) aim = FVector(12, 23, 34);
        else if (partial_failure) aim.X = 99;
        return next_ok;
    }
};
struct AController { NavigationHandle* NavigationHandle = nullptr; };
struct UMMONavigationHelper {
    AController* _Owner = nullptr;
    NavigationHandle* _NavigationHandle = nullptr;
    FVector _Destination{0, 0, 0};
    UBOOL ShowDebugPath = FALSE;
    bool parameters_ok = true;
    INT parameter_calls = 0;
    UBOOL eventPram_NavMeshPath_Toward(NavigationHandle* handle, FVector) {
        ++parameter_calls; handle->prepared = parameters_ok; return parameters_ok;
    }
    void Setup(AController*);
    UBOOL FindPathToDest(FVector);
    void BeginPathing(FVector);
    UBOOL GetNextMoveLocation(FVector&, FLOAT);
};
''')
        for signature in ["void UMMONavigationHelper::Setup(", "UBOOL UMMONavigationHelper::FindPathToDest(",
                          "void UMMONavigationHelper::BeginPathing(", "UBOOL UMMONavigationHelper::GetNextMoveLocation("]:
            parts.append(function(cpp, signature))
        parts.append(r'''
void Run() {
    UMMONavigationHelper helper;
    FVector target(4, 5, 6), aim;
    assert(!helper.FindPathToDest(target) && !helper.GetNextMoveLocation(aim, 1)); Equal(aim, 0, 0, 0);
    helper.BeginPathing(target); Equal(helper._Destination, 4, 5, 6);
    AController owner;
    auto* first = new NavigationHandle;
    owner.NavigationHandle = first; helper.ShowDebugPath = TRUE; helper.Setup(&owner);
    assert(helper._Owner == &owner && helper._NavigationHandle == first);
    assert(first->bUltraVerbosePathDebugging && first->bVisualPathDebugging);
    helper.BeginPathing(target); Equal(first->target, 4, 5, 6); assert(first->destinations == 1);
    g_drawPathDebug = true;
    assert(helper.FindPathToDest(target) && first->clears == 1 && first->finds == 1);
#if SHIPPING_PC_GAME
    assert(!first->draws);
#else
    assert(first->draws == 1);
#endif
    first->find_ok = false;
    assert(!helper.FindPathToDest(target) && first->finds == 2);
    helper.parameters_ok = false;
    assert(!helper.FindPathToDest(target) && first->finds == 2);
    helper.parameters_ok = true;
    assert(helper.GetNextMoveLocation(aim, 17)); Equal(aim, 12, 23, 34); assert(first->arrival == 17);
    first->next_ok = false;
    for (bool partial : {false, true}) {
        first->partial_failure = partial; aim = FVector(11, 22, 33);
        assert(!helper.GetNextMoveLocation(aim, 17)); Equal(aim, 0, 0, 0);
    }
    helper.Setup(nullptr);
    assert(!helper._Owner && !helper._NavigationHandle);
    delete first; // The helper must not touch its old owner's freed handle.
    assert(!helper.FindPathToDest(target) && !helper.GetNextMoveLocation(aim, 2)); Equal(aim, 0, 0, 0);
    helper.BeginPathing(target);
    NavigationHandle second;
    owner.NavigationHandle = &second; helper.ShowDebugPath = FALSE; helper.Setup(&owner);
    assert(!second.bUltraVerbosePathDebugging && !second.bVisualPathDebugging);
    assert(helper.FindPathToDest(target) && second.finds == 1);
    AController without_handle;
    helper.Setup(&without_handle);
    assert(helper._Owner == &without_handle && !helper._NavigationHandle);
    assert(!helper.FindPathToDest(target) && !helper.GetNextMoveLocation(aim, 2)); Equal(aim, 0, 0, 0);
    assert(second.finds == 1 && !second.moves);
    puts("Navigation helper clears detached handles, rejects incomplete path setup and defines failed move outputs");
}
}
#undef SHIPPING_PC_GAME
''')
    parts.append('#pragma pop_macro("SHIPPING_PC_GAME")')
    return "\n".join(parts)
