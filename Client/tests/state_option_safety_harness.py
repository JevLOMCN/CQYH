"""Exercise actual state constructors/resets and display-level bounds."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / path))

    header = clean("Src/FSMState.h")
    cpp = clean("Src/FSMState.cpp")
    parts = [r'''
namespace StateOptionSafetyTests {
using ScreenRecordDefaultsTests::DefaultConstruct;
using FVector = ::FVector;
using FRotator = RuntimeDefaultTests::FRotator;
struct AActor; struct AMMOActor; struct AMMOPlayer; struct AMMOPlayerController;
struct ANPC; struct UMMOAbility; struct IIInteractableActor;
template<class T> struct TScriptInterface {
    void* object = nullptr;
    T* interface_pointer = nullptr;
};
template<class T> TScriptInterface<T> ScriptInterfaceCast(void*) { return {}; }
template<class T> struct SingletonMeyers {
    static T& getSingleton() { static T value; return value; }
};
''']
    parts.append(function(header, "enum KEY_TYPE\n") + ";")
    parts.append(function(header, "class IFSMState\n") + ";")
    parts.append(function(header, "class FSMStateManager :").replace("private:", "public:") + ";")
    for signature in ["IFSMState::IFSMState(void)", "void FSMStateManager::AddGameState(", "void FSMStateManager::Init()"]:
        parts.append(function(cpp, signature))
    states = ["Input", "Charactor", "CDTime", "Ability", "Target", "Trigger", "Ctrl"]
    definitions = {}
    for suffix in states:
        name = "FSMState_" + suffix
        definitions[name] = function(header, "class " + name + ":")
        parts.append(definitions[name] + ";")
        parts.append(function(cpp, "void " + name + "::Init()"))
        parts.append("void " + name + "::Update(FLOAT) {}")
    # Verify real reset behavior through the manager as well as construction.
    special = {"CheckWeaponBindTime": "3.0f", "PickTarget": "TRUE", "state_Standby": "TRUE",
               "ctrlAttackAi_LastDelay": "0.25f", "ctrlAttackAi_LockTime": "0.05f",
               "DelayTimeWaitingSendMove": "-1.0f", "UpdateSceneHintTime": "-1.0f"}
    preserved = {"ctrlFlag_IgnoreShiftBtn", "ctrlFlag_DDCS", "ctrlFlag_ZNBY", "ctrlFlag_ZDZR",
                 "ctrlFlag_ZDLH", "ctrlFlag_ZDYS", "ctrlFlag_ZDKD"}
    fields = {}
    for name, declaration in definitions.items():
        fields[name] = re.findall(
            r"(?m)^\s*(UBOOL|INT|FLOAT|FVector|KEY_TYPE|(?:AActor|AMMOActor|AMMOPlayer|AMMOPlayerController|ANPC|UMMOAbility)\*)\s+(\w+)\s*;",
            declaration)
        if not fields[name]:
            raise ValueError("Missing state fields: " + name)
        parts.append("void Check(const " + name + "& v, bool reset = false) {")
        for kind, field in fields[name]:
            if kind == "FVector":
                parts.append("assert(!v." + field + ".X && !v." + field + ".Y && !v." + field + ".Z);")
            else:
                expected = "(reset ? TRUE : FALSE)" if field in preserved else special.get(field, "0")
                parts.append("assert(v." + field + " == " + expected + ");")
        if name == "FSMState_Target":
            parts.append("assert(!v.DesireInteractableActor.object && !v.DesireInteractableActor.interface_pointer);")
        parts.append("}")
    parts.append("void StateTests() {")
    for name in definitions:
        parts.append("DefaultConstruct<" + name + ">([](auto& v) {")
        parts.append("auto& manager = FSMStateManager::getSingleton(); assert(manager.IFSMStateList.size() == 1 && manager.IFSMStateList[0] == &v); Check(v);")
        for kind, field in fields[name]:
            if kind.endswith("*"):
                value = "reinterpret_cast<" + kind + ">(uintptr_t(1))"
            elif kind == "FVector":
                value = "FVector(7, 8, 9)"
            elif kind == "KEY_TYPE":
                value = "KT_PRIMARY"
            else:
                value = "TRUE" if field in preserved else "123"
            parts.append("v." + field + " = " + value + ";")
        if name == "FSMState_Target":
            parts.append("v.DesireInteractableActor.object = &v; v.DesireInteractableActor.interface_pointer = reinterpret_cast<IIInteractableActor*>(&v);")
        parts.append("manager.Init(); Check(v, true); manager.IFSMStateList.clear(); });")
    parts.append('puts("Seven state constructors and complete resets passed; configured control preferences preserved");\n}')
    options = clean("Src/MMOUIOptionConfig.cpp")
    parts.append("template<class T> T Clamp(T value, T low, T high) { return value < low ? low : (value > high ? high : value); }")
    for name in ["VLevelData", "INTLevelData", "BOOLLevelData", "FLOATLevelData"]:
        match = re.search(r"class " + name + r"\b[^;{]*\{", options).group()
        parts.append(function(options, match) + ";")
    parts.append(r'''
void OptionTests() {
    INTLevelData integers(11, 22, 33, 44, 55);
    BOOLLevelData booleans(FALSE, TRUE, FALSE, TRUE, TRUE);
    FLOATLevelData floats(0.25f, 0.5f, 0.75f, 1.0f, 1.25f);
    VLevelData* values[] = {&integers, &booleans, &floats};
    for (INT index : {(-2147483647 - 1), -1, 0, 1, 2, 3, 4, 5, 2147483647}) {
        const INT bounded = index < 0 ? 0 : (index > 4 ? 4 : index);
        assert(values[0]->GetIntData(index) == integers.arrLevel[bounded]);
        assert(values[1]->GetBoolData(index) == booleans.arrLevel[bounded]);
        assert(values[2]->GetFloatData(index) == floats.arrLevel[bounded]);
    }
    assert(integers.GetMinValue() == 11 && integers.GetMaxValue() == 55);
    puts("Integer, boolean and float display levels handle negative/oversized indices and retain valid values");
}
}
''')
    return "\n".join(parts)
