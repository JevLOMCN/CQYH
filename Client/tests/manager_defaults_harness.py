"""Production callback defaults, fixed student names and effect-index checks."""
import re


def harness(client, game, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#(?:include|pragma)[^\n]*", "", text)

    parts = [source(client / "Common/utility/fixed_string.h"), r'''
#include <memory>
namespace ManagerDefaultsTests {
using FString = ChatStringTests::FString;
enum EEventParm { EC_EventParm };
enum { NAME_None = 0 };
struct FName {
    int index;
    std::string text;
    FName() {}
    FName(int value) : index(value) {}
    FName(const char* value) : index(1), text(value) {}
    bool operator==(const FName& other) const { return index == other.index && text == other.text; }
};
struct UObject;
''']
    name_capacity = re.search(r"#define ROLE_NAME_LEN\s+([^\n]+)",
                              clean(source(client / "Common/define/string_def.h"))).group(1)
    parts.append("enum { StudentRoleNameCapacity = " + name_capacity + " };")
    delegate = source(game.parent / "Core/Inc/UnObjBas.h")
    start = delegate.index("struct FScriptDelegate\n")
    end = delegate.index("\n\tinline UBOOL IsCallable", start)
    parts.append(clean(delegate[start:end]) + "\n};")
    parts.append(r'''
struct StudentManager { FScriptDelegate _UpdateStudentListDelegate; StudentManager(); };
struct StudentDetailManager { FScriptDelegate _UpdateStudentDetailListDelegate; StudentDetailManager(); };
template<class T> struct SingletonMeyers {};
template<class T> struct TArray {
    std::vector<T> data;
    int Num() const { return (int)data.size(); }
    T& operator()(int i) { return data.at(i); }
};
struct UMaterialEffect { FName EffectName; void* Material = nullptr; UBOOL bShowInGame = FALSE; };
template<class T> T* Cast(UMaterialEffect* effect) { return effect; }
struct PostProcess { TArray<UMaterialEffect*> Effects; };
struct ULocalPlayer { void* Actor = nullptr; PostProcess* PlayerPostProcess = nullptr; };
struct Engine { TArray<ULocalPlayer*> GamePlayers; };
Engine* GEngine = nullptr;
struct Controller {
    int calls = 0;
    bool playing = false;
    void eventPlayLowHPWarningSound(UBOOL value) { ++calls; playing = value != 0; }
};
struct MMOFacade {
    Controller* controller = nullptr;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    Controller* GetMainController() { return controller; }
};
''')
    for name in ["StudentManager", "StudentDetailManager"]:
        parts.append(function(source(game / (name + ".cpp")), name + "::" + name + "()"))
    link = clean(source(game / "LinkManager.h"))
    parts.extend(re.findall(r"typedef UBOOL\s+\(\*CallBackLink(?:ButtonIdx)?Fun\)[^;]+;", link))
    parts.append(function(link, "struct FCallbackLinkFuns\n") + ";")
    parts.append(r'''
struct ClientBriefStudentInfo { char szRoleName[StudentRoleNameCapacity]; };
#define IMPLEMENT_COMPARE_POINTER(Type, Name, Body) int CompareStudentNames(Type* A, Type* B) Body
''')
    parts.append(re.search(r"(?m)^IMPLEMENT_COMPARE_POINTER\( ClientBriefStudentInfo, __StudentName, [^\n]+",
                           source(game / "StudentManager.cpp")).group())
    parts.append("#undef IMPLEMENT_COMPARE_POINTER")
    parts.append(clean(source(game / "MMOFullScreenEffectManager.h")))
    parts.append(clean(source(game / "MMOFullScreenEffectManager.cpp")))
    # Expose arrays to check guard behavior against a deliberately poisoned object.
    parts = [part.replace("protected:", "public:") for part in parts]
    parts.append(r'''
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(StudentManager) unsigned char student_bytes[sizeof(StudentManager)];
        memset(student_bytes, pattern, sizeof(student_bytes));
        auto* student = new(student_bytes) StudentManager;
        assert(!student->_UpdateStudentListDelegate.Object && !student->_UpdateStudentListDelegate.FunctionName.index);
        student->~StudentManager();
        alignas(StudentDetailManager) unsigned char detail_bytes[sizeof(StudentDetailManager)];
        memset(detail_bytes, pattern, sizeof(detail_bytes));
        auto* detail = new(detail_bytes) StudentDetailManager;
        assert(!detail->_UpdateStudentDetailListDelegate.Object && !detail->_UpdateStudentDetailListDelegate.FunctionName.index);
        detail->~StudentDetailManager();
        alignas(FCallbackLinkFuns) unsigned char callback_bytes[sizeof(FCallbackLinkFuns)];
        memset(callback_bytes, pattern, sizeof(callback_bytes));
        auto* callback = new(callback_bytes) FCallbackLinkFuns;
        assert(!callback->MouseClickProcess && !callback->MouseOverProcess && !callback->MouseOutProcess);
    }
    auto exact = std::make_unique<ClientBriefStudentInfo>();
    memset(exact->szRoleName, 'A', sizeof(exact->szRoleName));
    ClientBriefStudentInfo short_name = {}, same_length;
    short_name.szRoleName[0] = 'x';
    memset(same_length.szRoleName, 'B', sizeof(same_length.szRoleName));
    assert(CompareStudentNames(exact.get(), &short_name) == -1);
    assert(CompareStudentNames(&short_name, exact.get()) == 1);
    assert(CompareStudentNames(exact.get(), &same_length) == 0);
    assert(CompareStudentNames(exact.get(), exact.get()) == 0);
    MMOFullScreenEffectManager effects;
    Controller controller;
    MMOFacade::getSingleton().controller = &controller;
    for (int invalid : {-2147483647 - 1, -1, int(FSET_Total), 2147483647}) {
        effects.StartEffect(invalid);
        effects.StopEffect(invalid);
    }
    assert(!controller.calls && !effects._FullScreenEffectTypeOpen[0]);
    Engine engine; GEngine = &engine;
    UMaterialEffect material; material.EffectName = FName("HitEffect"); material.Material = &material;
    PostProcess post; post.Effects.data = {nullptr, &material};
    ULocalPlayer player; player.Actor = &player; player.PlayerPostProcess = &post;
    engine.GamePlayers.data = {nullptr, &player};
    effects.StartEffect(FSET_LowHP);
    assert(controller.calls == 1 && controller.playing && material.bShowInGame && effects._FullScreenEffectTypeOpen[0]);
    effects.StartEffect(FSET_LowHP); assert(controller.calls == 1);
    effects.StopAllEffect();
    assert(controller.calls == 2 && !controller.playing && !material.bShowInGame && !effects._FullScreenEffectTypeOpen[0]);
    MMOFacade::getSingleton().controller = nullptr;
    effects.StartEffect(FSET_LowHP); effects.StopEffect(FSET_LowHP);
    assert(!effects._FullScreenEffectTypeOpen[0]);
    GEngine = nullptr;
    effects.StartEffect(FSET_LowHP);
    effects._FullScreenEffectTypeOpen[0] = TRUE;
    effects.StopEffect(FSET_LowHP);
    puts("Callback initialization, bounded student-name sorting and effect indices passed");
}
}
''')
    return "\n".join(parts)
