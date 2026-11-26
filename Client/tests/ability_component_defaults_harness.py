"""Construct real ability components, including both trail diagnostic layouts."""
import re


def harness(game, source, function):
    text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / "AbilityProcessor.h"))
    names = ["EffectNode", "CastNode", "BaseComponent"] + re.findall(
        r"struct (\w+) : public BaseComponent", text)
    definitions = {name: function(text, re.search(r"struct " + name + r"\b[^;{]*\{", text).group()) + ";"
                   for name in names}
    parts = ["#include <list>\n#pragma push_macro(\"SHOW_TRAIL_LOG\")\n#undef SHOW_TRAIL_LOG"]
    for logging in [0, 1]:
        parts.extend(["#define SHOW_TRAIL_LOG " + str(logging),
                      "namespace AbilityComponentDefaults" + str(logging) + "Tests {", r'''
using FLOAT = float;
using FVector = ::FVector;
template<class T> using TArray = SkillQuestTests::TArray<T>;
struct AbilityNode;
struct EffectNode;
struct InitData;
struct UAbilityComponent { BYTE StopType = 0; };
enum EAbilityComponentStopType { StopDefault };
using STLEffectNodeVector = std::vector<EffectNode*>;
'''])
        pointer_types = sorted(set(re.findall(r"\b([AU]\w+)\s*\*", "\n".join(definitions.values()))) - {"UAbilityComponent"})
        parts.extend("struct " + name + ";" for name in pointer_types)
        parts.extend(definitions.values())
        # Supply inert engine methods so the real virtual component declarations
        # and constructors can be compiled unchanged in this standalone fixture.
        for name, definition in definitions.items():
            for match in re.finditer(r"virtual\s+([\w*]+)\s+(\w+)\(([^)]*)\)\s*;", definition):
                result, method, arguments = match.groups()
                arguments = re.sub(r"\s*=\s*[^,]+", "", arguments)
                body = "" if result == "void" else "return nullptr;" if "*" in result else "return FALSE;"
                parts.append(result + " " + name + "::" + method + "(" + arguments + ") {" + body + "}")
        parts.append(r'''
void CheckBase(const BaseComponent& value) {
    assert(!value.ComponentId && !value.ComponentType && !value.ComponentData && value.bStopped);
    assert(!value.CreatorId && !value.BaseActorId && !value.TargetId);
    assert(!value.TargetPoint.X && !value.TargetPoint.Y && !value.TargetPoint.Z);
    assert(value.Rate == 1 && value.MaxDistance == 100 && value.bClearWhenOwerRevive);
    assert(value.WeaponHitType == -1 && !value.bUseEngineObjectId && !value.bAddedToRoot);
    assert(!value.StopType && !value.CurrentTime && value.Duration == 1);
}
void Run() {
''')
        for name, definition in definitions.items():
            if name == "BaseComponent":
                parts.append("TipsStallTests::Poisoned<BaseComponent>([](auto& value) { CheckBase(value); });")
                continue
            pointers = re.findall(r"(?m)^\s*\w+\s*\*\s*(\w+)\s*;", definition)
            fields = re.findall(r"(?m)^\s*(?:INT|UINT|FLOAT|BOOL|UBOOL)\s+(\w+)\s*;", definition)
            checks = ["assert(!value." + pointer + ");" for pointer in pointers]
            for field in fields:
                if field == "bFirstTime":
                    checks.append("\n#if SHOW_TRAIL_LOG\nassert(value.bFirstTime);\n#endif\n")
                    continue
                expected = -1 if field in ["AnimIdx", "SoundCueComponentIdx"] or (field == "ActorIdx" and name != "WindSourceComponent") else 1 if field == "Visible" else 0
                checks.append("assert(value." + field + " == " + str(expected) + ");")
            if name in ["ThrowComponent", "TrailComponent"]:
                vectors = ["LastVelocity"] if name == "ThrowComponent" else ["SrcPoint1", "SrcPoint2", "SrcPoint3"]
                vector_checks = " ".join("assert(!value." + field + ".X && !value." + field + ".Y && !value." + field + ".Z);" for field in vectors)
                checks.append(vector_checks if name == "ThrowComponent" else "\n#if SHOW_TRAIL_LOG\n" + vector_checks + "\n#endif\n")
            for field in re.findall(r"(?m)^\s*(?:std::list<INT>|TArray<INT>|STLEffectNodeVector)\s+(\w+)\s*;", definition):
                checks.append("assert(value." + field + (".Num() == 0);" if field == "SoundCueComponentIdxs" else ".empty());"))
            if " : public BaseComponent" in definition:
                checks.append("CheckBase(value);")
            parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& value) {" + " ".join(checks) + "});")
        parts.append('puts("Ability component pointer/state defaults and trail diagnostic layout ' + str(logging) + ' passed");\n}\n}\n#undef SHOW_TRAIL_LOG')
    parts.append('#pragma pop_macro("SHOW_TRAIL_LOG")')
    return "\n".join(parts)
