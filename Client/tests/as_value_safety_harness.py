"""Actual Scaleform value conversions, returns and temporary array ownership."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', text)

    root = game.parent / "GFxUI"
    active, stub = source(root / "Src/ScaleformMovie.cpp").split("#else // WITH_GFx = 0", 1)
    active, stub = clean(active), clean(stub)
    header = clean(source(root / "Inc/GFxUIClasses.h"))
    engine = clean(source(root / "Inc/ScaleformEngine.h"))
    array = function(engine, "struct FAutoGFxValueArray\n") + ";"
    macro = re.search(r'^#define AutoGFxValueArray[^\n]+', engine, re.M).group()
    parts = [r'''
#pragma push_macro("SCRIPT_ALIGN")
#pragma push_macro("TCHAR_IS_1_BYTE")
#pragma push_macro("TEXT")
#pragma push_macro("MAXINT")
#pragma push_macro("AutoGFxValueArray")
#undef SCRIPT_ALIGN
#undef MAXINT
#undef AutoGFxValueArray
#define SCRIPT_ALIGN
#define MAXINT 2147483647
''', macro]
    signatures = ["FASValue UGFxMoviePlayer::Invoke (", "FASValue UGFxMoviePlayer::GetVariable (",
                  "struct FASValue UGFxObject::Invoke (", "struct FASValue UGFxObject::Get (",
                  "struct FASValue UGFxObject::GetElement (", "struct FASValue UGFxObject::GetElementMember ("]
    for ctype, movie_name, object_name in [("UBOOL", "Bool", "Bool"), ("FLOAT", "Number", "Float"),
                                         ("INT", "Int", "Int"), ("FString", "String", "String")]:
        signatures.append(ctype + " UGFxMoviePlayer::GetVariable" + movie_name + " (")
        for prefix in ["Get", "GetElement", "GetElementMember"]:
            signatures.append(ctype + " UGFxObject::" + prefix + object_name + " (")
    signatures.append("FString UGFxObject::GetText()")
    for ansi in [0, 1]:
        for enabled, text in [(0, stub), (1, active)]:
            name = "ASValue%d_%dTests" % (ansi, enabled)
            parts.extend(["#undef TCHAR_IS_1_BYTE\n#define TCHAR_IS_1_BYTE " + str(ansi),
                          "#undef TEXT\n#define TEXT(s) " + ("s" if ansi else "L##s"), "namespace " + name + " {",
                          "using TCHAR = " + ("char" if ansi else "wchar_t") + ";"])
            parts.append(r'''
using BITFIELD = UINT;
using SIZE_T = size_t;
using String = std::string;
enum EEventParm { EC_EventParm };
std::set<void*> strings;
struct FString {
    TCHAR* data;
    FString() : data(nullptr) {}
    FString(const TCHAR* text) : FString() { *this = text; }
    FString(const FString& other) : FString() { *this = *other; }
    ~FString() { Clear(); }
    void Clear() { if (data) { assert(strings.erase(data) == 1); free(data); data = nullptr; } }
    FString& operator=(const TCHAR* text) {
        std::basic_string<TCHAR> copy(text); Clear();
        if (!copy.empty()) { data = static_cast<TCHAR*>(malloc((copy.size() + 1) * sizeof(TCHAR)));
            memcpy(data, copy.c_str(), (copy.size() + 1) * sizeof(TCHAR)); assert(strings.insert(data).second); }
        return *this;
    }
    FString& operator=(const FString& other) { if (this != &other) *this = *other; return *this; }
    const TCHAR* operator*() const { static const TCHAR empty[] = {0}; return data ? data : empty; }
};
struct FTCHARToUTF8 {
    std::string text;
    explicit FTCHARToUTF8(const TCHAR* value) { while (*value) text += char(*value++); }
    operator const char*() const { return text.c_str(); }
};
struct FUTF8ToTCHAR {
    std::basic_string<TCHAR> text;
    explicit FUTF8ToTCHAR(const char* value) { while (*value) text += TCHAR(*value++); }
    operator const TCHAR*() const { return text.c_str(); }
};
template<class T> struct TArray {
    std::vector<T> values;
    INT override_count = -1;
    INT Num() const { return override_count < 0 ? INT(values.size()) : override_count; }
    T& operator()(INT n) { return values.at(n); }
    const T& operator()(INT n) const { return values.at(n); }
    void AddItem(const T& value) { values.push_back(value); }
    void AddZeroed(INT count) { assert(count >= 0); values.resize(values.size() + count, T()); }
    T* GetData() { return values.data(); }
    const T* GetData() const { return values.data(); }
    void Empty() { values.clear(); }
};
namespace GFx {
struct Value {
    enum Type { VT_Undefined, VT_Null, VT_Number, VT_Int, VT_UInt, VT_Boolean, VT_String, VT_StringW, VT_Object, VT_Array, VT_DisplayObject,
                VT_ConvertBoolean = 64 | VT_Boolean, VT_ConvertNumber = 64 | VT_Number, VT_ConvertInt = 64 | VT_Int,
                VT_ConvertString = 64 | VT_String, VT_ConvertStringW = 64 | VT_StringW };
    Type type = VT_Undefined;
    double number = 0;
    INT integer = 0;
    UINT uinteger = 0;
    bool boolean = false, success = false, null_string = false;
    std::string text;
    std::wstring wide;
    Value* result = nullptr;
    static INT live;
    Value() { ++live; }
    explicit Value(Type requested) : Value() {
        type = requested;
        // Conversion tags report their scalar type even before a successful
        // write. Poison their payloads to detect reads after backend failure.
        number = std::numeric_limits<double>::quiet_NaN(); integer = 123; boolean = true; null_string = true;
    }
    Value(const Value& value) { ++live; *this = value; }
    ~Value() { --live; }
    Type GetType() const { return Type(UINT(type) & 63); }
    double GetNumber() const { return number; }
    INT GetInt() const { return integer; }
    UINT GetUInt() const { return uinteger; }
    bool GetBool() const { return boolean; }
    const char* GetString() const { return null_string ? nullptr : text.c_str(); }
    const wchar_t* GetStringW() const { return null_string ? nullptr : wide.c_str(); }
    void SetNull() { type = VT_Null; }
    void SetUndefined() { type = VT_Undefined; }
    void SetNumber(double n) { type = VT_Number; number = n; }
    void SetInt(INT n) { type = VT_Int; integer = n; }
    void SetBoolean(bool b) { type = VT_Boolean; boolean = b; }
    void SetString(const char* s) { type = VT_String; null_string = !s; text = s ? s : ""; }
    void SetStringW(const wchar_t* s) { type = VT_StringW; null_string = !s; wide = s ? s : L""; }
    bool IsObject() const { return type == VT_Object || type == VT_Array || type == VT_DisplayObject; }
    bool IsArray() const { return type == VT_Array; }
    bool IsDisplayObject() const { return type == VT_DisplayObject; }
    bool Invoke(const char*, Value* out, const Value*, UINT) { if (!success || !result) return false; *out = *result; return true; }
    bool GetMember(const char*, Value* out) { if (!success || !result) return false; *out = *result; return true; }
    bool GetElement(UINT index, Value* out) { if (!success || !result || index != 0) return false; *out = *result; return true; }
    bool GetText(Value* out) { return GetMember("text", out); }
};
INT Value::live = 0;
}
bool fail_malloc = false;
std::set<void*> buffers;
void* appMalloc(SIZE_T size) {
    if (fail_malloc) return nullptr;
    void* p = malloc(size); assert(p && buffers.insert(p).second); return p;
}
void appFree(void* p) { assert(buffers.erase(p) == 1); free(p); }
''')
            parts.extend([function(header, "enum ASType\n") + ";",
                          function(header, "struct FASValue\n") + ";", array])
            parts.append(r'''
struct Movie {
    enum SetArrayType { SA_Int, SA_Double, SA_Float, SA_String, SA_StringW, SA_Value };
    GFx::Value response;
    std::vector<GFx::Value> array;
    bool success = false, array_success = true, partial = false, null_string = false;
    bool report_total = false;
    UINT reported_total = 0, calls = 0, last_index = 0, last_count = 0;
    UINT GetVariableArraySize(const String&) { return report_total ? reported_total : UINT(array.size()); }
    bool Invoke(const char*, GFx::Value* out, const GFx::Value*, UINT) { if (!success) return false; *out = response; return true; }
    bool GetVariable(GFx::Value* out, const char*) { if (!success) return false; *out = response; return true; }
    void CreateObject(GFx::Value* out, const char*, const GFx::Value*, UINT) { out->type = GFx::Value::VT_Object; }
    bool GetVariableArray(SetArrayType type, const String&, UINT index, void* data, UINT count) {
        ++calls; last_index = index; last_count = count;
        assert(type != SA_StringW); // The SDK's wide-string preparation path is unsafe for suffix reads.
        assert(index <= array.size() && count <= array.size() - index);
        if (!array_success && !partial) return false;
        for (UINT i = 0; i < count; ++i) {
            const auto& v = array[index + i];
            if (type == SA_Value) static_cast<GFx::Value*>(data)[i] = v;
            else if (type == SA_Int) static_cast<INT*>(data)[i] = v.integer;
            else if (type == SA_Float) static_cast<FLOAT*>(data)[i] = FLOAT(v.number);
            else if (type == SA_String) static_cast<const char**>(data)[i] = null_string ? nullptr : v.text.c_str();
            else if (type == SA_StringW) static_cast<const wchar_t**>(data)[i] = null_string ? nullptr : v.wide.c_str();
            if (!array_success) return false;
        }
        return true;
    }
    bool SetVariableArray(SetArrayType, const char*, UINT index, const void*, UINT count) {
        ++calls; last_index = index; last_count = count; return true;
    }
};
struct MovieInstance { Movie* pView = nullptr; };
void* GGFxEngine = reinterpret_cast<void*>(1);
struct UClass {};
struct UGFxObject {
    void* Value = nullptr;
    static UClass* StaticClass() { static UClass value; return &value; }
    FASValue Invoke(const FString&, const TArray<FASValue>&);
    FASValue Get(const FString&);
    FASValue GetElement(INT);
    FASValue GetElementMember(INT, const FString&);
    UBOOL GetBool(const FString&); FLOAT GetFloat(const FString&); INT GetInt(const FString&); FString GetString(const FString&);
    UBOOL GetElementBool(INT); FLOAT GetElementFloat(INT); INT GetElementInt(INT); FString GetElementString(INT);
    UBOOL GetElementMemberBool(INT, const FString&); FLOAT GetElementMemberFloat(INT, const FString&);
    INT GetElementMemberInt(INT, const FString&); FString GetElementMemberString(INT, const FString&);
    FString GetText();
};
struct UGFxMoviePlayer {
    MovieInstance* pMovie = nullptr;
    FASValue Invoke(const FString&, const TArray<FASValue>&);
    FASValue GetVariable(const FString&);
    UBOOL GetVariableBool(const FString&); FLOAT GetVariableNumber(const FString&); INT GetVariableInt(const FString&); FString GetVariableString(const FString&);
    UGFxObject* CreateObject(const FString&, UClass*, const TArray<FASValue>&);
    UGFxObject* CreateValueAddRef(GFx::Value*, UClass*) { static UGFxObject result; return &result; }
    UBOOL GetVariableArray(const FString&, INT, TArray<FASValue>&);
    UBOOL GetVariableIntArray(const FString&, INT, TArray<INT>&);
    UBOOL GetVariableFloatArray(const FString&, INT, TArray<FLOAT>&);
    UBOOL GetVariableStringArray(const FString&, INT, TArray<FString>&);
    UBOOL SetVariableArray(const FString&, INT, const TArray<FASValue>&);
    UBOOL SetVariableIntArray(const FString&, INT, const TArray<INT>&);
    UBOOL SetVariableFloatArray(const FString&, INT, const TArray<FLOAT>&);
    UBOOL SetVariableStringArray(const FString&, INT, const TArray<FString>&);
};
''')
            if enabled:
                for sig in ["static inline void ConvertFASValueToGFxValue(", "static inline void ConvertGFxValueToFASValue("]:
                    parts.append(function(active, sig))
            for sig in signatures:
                parts.append(function(text, sig))
            if enabled:
                parts.append(function(active, "UGFxObject* UGFxMoviePlayer::CreateObject ("))
                for kind in ["", "Int", "Float", "String"]:
                    for action in ["Get", "Set"]:
                        parts.append(function(active, "UBOOL UGFxMoviePlayer::" + action + "Variable" + kind + "Array ("))
                parts.append(r'''
enum { CPF_Parm = 1, CPF_ReturnParm = 2 };
struct UProperty { UINT PropertyFlags = CPF_Parm; INT Offset = 0; };
struct Node { virtual ~Node() {} FString GetName() { return FString(TEXT("test")); } };
struct UFunction : Node {
    INT count = 0;
    UProperty result;
    UProperty* GetReturnProperty() { return &result; }
};
template<class T> T* Cast(Node* node) { return dynamic_cast<T*>(node); }
template<class T> struct TFieldIterator {
    UFunction* function;
    INT index = 0;
    T property;
    explicit TFieldIterator(UFunction* f) : function(f) {}
    operator bool() const { return index < function->count; }
    T* operator->() { property.Offset = index * sizeof(INT); return &property; }
    T* operator*() { return operator->(); }
    void operator++() { ++index; }
};
struct FFrame { struct Node* Node; BYTE* Locals; };
struct FGFxEngine {
    static void ConvertUPropToGFx(UProperty*, BYTE* input, GFx::Value& value, Movie*) {
        INT n; memcpy(&n, input, sizeof(n)); value.SetInt(n);
    }
    static void ConvertGFxToUProp(UProperty*, BYTE* result, const GFx::Value& value, UGFxMoviePlayer*) {
        INT n = value.GetInt(); memcpy(result, &n, sizeof(n));
    }
};
enum ActionScriptReturnSignature { FunctionIsVoid, FunctionReturnsValue };
#pragma push_macro("P_GET_STR")
#pragma push_macro("P_FINISH")
#pragma push_macro("RESULT_DECL")
#undef P_GET_STR
#undef P_FINISH
#undef RESULT_DECL
#define P_GET_STR(name) FString name(TEXT("method"))
#define P_FINISH ((void)0)
#define RESULT_DECL void* Result
''')
                parts.append("template<typename InvokerType>\n" + function(active, "void ExecuteActionScript ("))
                parts.append(r'''
#pragma pop_macro("RESULT_DECL")
#pragma pop_macro("P_FINISH")
#pragma pop_macro("P_GET_STR")
void ExecuteTests() {
    UFunction function; function.count = 20000;
    std::vector<INT> input(function.count, 8);
    FFrame stack{&function, reinterpret_cast<BYTE*>(input.data())};
    Movie view; MovieInstance movie{&view}; UGFxMoviePlayer player; player.pMovie = &movie;
    view.success = true; view.response.SetInt(77);
    GFx::Value result; result.SetInt(0);
    INT output = -1;
    ExecuteActionScript(&view, &player, result, stack, &output);
    assert(output == 77);
    result.SetInt(5); fail_malloc = true; output = -1;
    ExecuteActionScript(&view, &player, result, stack, &output);
    assert(output == 5); fail_malloc = false;
    result.SetInt(6); player.pMovie = nullptr; output = -1;
    ExecuteActionScript(&view, &player, result, stack, &output);
    assert(output == 6);
    player.pMovie = &movie; movie.pView = nullptr; output = -1;
    ExecuteActionScript(&view, &player, result, stack, &output);
    assert(output == 6);
    output = -1;
    ExecuteActionScript(&view, &player, result, stack, &output, FunctionIsVoid);
    assert(output == -1);
}
''')
            # Tests are appended below with the matching namespace still open.
            parts.append(_tests(enabled))
            parts.append("}")
    parts.append(r'''
#pragma pop_macro("AutoGFxValueArray")
#pragma pop_macro("MAXINT")
#pragma pop_macro("TEXT")
#pragma pop_macro("TCHAR_IS_1_BYTE")
#pragma pop_macro("SCRIPT_ALIGN")
''')
    return "\n".join(parts)


def _tests(enabled):
    tests = r'''
void ScalarDefaults(UGFxMoviePlayer& player, UGFxObject& object, const FString& path) {
    assert(!player.GetVariableBool(path) && player.GetVariableNumber(path) == 0 && !player.GetVariableInt(path));
    assert(!player.GetVariableString(path).data);
    assert(!object.GetBool(path) && object.GetFloat(path) == 0 && !object.GetInt(path));
    assert(!object.GetString(path).data && !object.GetText().data);
    assert(!object.GetElementBool(0) && object.GetElementFloat(0) == 0 && !object.GetElementInt(0));
    assert(!object.GetElementString(0).data);
    assert(!object.GetElementMemberBool(0, path) && object.GetElementMemberFloat(0, path) == 0 && !object.GetElementMemberInt(0, path));
    assert(!object.GetElementMemberString(0, path).data);
}
void ScalarTests() {
    FString path(TEXT("missing"));
    Movie view; MovieInstance instance{&view}; UGFxMoviePlayer player; UGFxObject object;
    ScalarDefaults(player, object, path);
    player.pMovie = &instance; instance.pView = nullptr;
    ScalarDefaults(player, object, path); instance.pView = &view;
    GFx::Value root; object.Value = &root;
    ScalarDefaults(player, object, path);
    root.type = GFx::Value::VT_Object;
    ScalarDefaults(player, object, path); // Failed lookup retains a valid conversion tag and poisoned payload.
    GFx::Value child; child.type = GFx::Value::VT_Object;
    root.type = GFx::Value::VT_Array; root.success = true; root.result = &child;
    ScalarDefaults(player, object, path); // Failed nested member lookup.
    root.type = GFx::Value::VT_DisplayObject; root.success = false;
    ScalarDefaults(player, object, path); // Failed text lookup.
'''
    if enabled:
        tests += r'''
    view.success = true; root.success = true; root.type = GFx::Value::VT_Object; root.result = &view.response;
    view.response.SetNumber(3.25);
    assert(player.GetVariableNumber(path) == 3.25f && object.GetFloat(path) == 3.25f);
    view.response.SetInt(-7);
    assert(player.GetVariableInt(path) == -7 && object.GetInt(path) == -7);
    view.response.SetBoolean(true);
    assert(player.GetVariableBool(path) && object.GetBool(path));
    view.response.SetString("hello");
    assert(std::basic_string<TCHAR>(*player.GetVariableString(path)) == TEXT("hello"));
    assert(std::basic_string<TCHAR>(*object.GetString(path)) == TEXT("hello"));
    root.type = GFx::Value::VT_Array;
    assert(std::basic_string<TCHAR>(*object.GetElementString(0)) == TEXT("hello"));
    assert(!object.GetElementString(-1).data && !object.GetElementString(1).data);
    view.response.SetNumber(1.5); assert(object.GetElementFloat(0) == 1.5f);
    view.response.SetInt(123); assert(object.GetElementInt(0) == 123);
    view.response.SetBoolean(true); assert(object.GetElementBool(0));
    root.result = &child; child.success = true; child.result = &view.response;
    assert(object.GetElementMemberBool(0, path));
    view.response.SetInt(-8); assert(object.GetElementMemberInt(0, path) == -8);
    view.response.SetNumber(6.5); assert(object.GetElementMemberFloat(0, path) == 6.5f);
    view.response.SetString("member");
    assert(std::basic_string<TCHAR>(*object.GetElementMemberString(0, path)) == TEXT("member"));
    view.response.SetString(nullptr);
    assert(!object.GetElementMemberString(0, path).data && !player.GetVariableString(path).data);
    root.result = &view.response;
    assert(!object.GetElementString(0).data);
    root.type = GFx::Value::VT_Object; assert(!object.GetString(path).data);
    root.type = GFx::Value::VT_DisplayObject; assert(!object.GetText().data);
#if !TCHAR_IS_1_BYTE
    view.response.SetStringW(L"wide");
    assert(std::basic_string<TCHAR>(*player.GetVariableString(path)) == TEXT("wide"));
    assert(std::basic_string<TCHAR>(*object.GetText()) == TEXT("wide"));
    view.response.SetStringW(nullptr);
    assert(!player.GetVariableString(path).data && !object.GetText().data);
#endif
'''
    tests += r'''
}
void Default(const FASValue& value, BYTE expected) {
    assert(value.Type == expected && !value.B && value.N == 0 && value.I == 0 && !value.S.data);
}
void ArrayLifetimeTests() {
    INT live = GFx::Value::live;
    { FAutoGFxValueArray empty(0); assert(empty.IsValid() && !empty.Array); }
    { FAutoGFxValueArray large(20000); assert(large.IsValid() && large.Count == 20000);
        large[19999].SetInt(42); assert(large[19999].GetInt() == 42); }
    for (UINT invalid : {UINT(-1), UINT(MAXINT) / UINT(sizeof(GFx::Value)) + 1}) {
        FAutoGFxValueArray values(invalid); assert(!values.IsValid() && !values.Array);
    }
    fail_malloc = true;
    { FAutoGFxValueArray values(3); assert(!values.IsValid()); }
    fail_malloc = false;
    assert(GFx::Value::live == live && buffers.empty());
}
void Run() {
    ScalarTests();
    ArrayLifetimeTests();
    {
        Movie view; MovieInstance instance{&view}; UGFxMoviePlayer player;
        UGFxObject object; TArray<FASValue> args;
        FString path(TEXT("value"));
'''
    expected = "AS_Undefined" if enabled else "AS_Null"
    tests += "        const BYTE absent = " + expected + ";\n"
    tests += r'''
        Default(player.Invoke(path, args), absent); Default(player.GetVariable(path), absent);
        Default(object.Invoke(path, args), absent); Default(object.Get(path), absent);
        Default(object.GetElement(0), absent); Default(object.GetElementMember(0, path), absent);
        player.pMovie = &instance;
        Default(player.Invoke(path, args), absent); Default(player.GetVariable(path), absent);
        GFx::Value value; object.Value = &value;
        Default(object.Invoke(path, args), absent); Default(object.Get(path), absent);
        Default(object.GetElement(0), absent); Default(object.GetElementMember(0, path), absent);
'''
    if enabled:
        tests += r'''
        ExecuteTests();
        view.success = true; view.response.SetInt(123);
        auto good = player.Invoke(path, args); assert(good.Type == AS_Int && good.I == 123 && !good.B && good.N == 0);
        good = player.GetVariable(path); assert(good.Type == AS_Int && good.I == 123);
        instance.pView = nullptr;
        Default(player.GetVariable(path), absent); Default(player.Invoke(path, args), absent);
        instance.pView = &view;
        value.type = GFx::Value::VT_Object; value.success = true; value.result = &view.response;
        good = object.Get(path); assert(good.Type == AS_Int && good.I == 123);
        good = object.Invoke(path, args); assert(good.Type == AS_Int && good.I == 123);
        value.type = GFx::Value::VT_Array;
        good = object.GetElement(0); assert(good.Type == AS_Int && good.I == 123);
        Default(object.GetElement(-1), absent); Default(object.GetElement(1), absent);
        GFx::Value member; member.type = GFx::Value::VT_Object; member.success = true; member.result = &view.response;
        value.result = &member;
        good = object.GetElementMember(0, path); assert(good.Type == AS_Int && good.I == 123);
        member.success = false; Default(object.GetElementMember(0, path), absent);
        for (INT type = GFx::Value::VT_Undefined; type <= GFx::Value::VT_Array; ++type) {
            GFx::Value input; input.type = GFx::Value::Type(type);
            input.number = 1.25; input.integer = -7; input.uinteger = UINT(-1); input.boolean = true;
            input.text = "narrow"; input.wide = L"wide";
            FASValue output(EC_EventParm); output.Type = AS_String; output.S = TEXT("old");
            ConvertGFxValueToFASValue(output, input);
            assert(output.N == (type == GFx::Value::VT_Number ? 1.25f : 0));
            assert(output.I == (type == GFx::Value::VT_Int ? -7 : type == GFx::Value::VT_UInt ? -1 : 0));
            assert(output.B == (type == GFx::Value::VT_Boolean));
            if (type == GFx::Value::VT_String) assert(std::basic_string<TCHAR>(*output.S) == TEXT("narrow"));
#if !TCHAR_IS_1_BYTE
            else if (type == GFx::Value::VT_StringW) assert(std::basic_string<TCHAR>(*output.S) == TEXT("wide"));
#endif
            else assert(!output.S.data);
        }
        // The checked temporary rejects an impossible count before caller loops.
        args.override_count = MAXINT;
        Default(player.Invoke(path, args), absent); Default(object.Invoke(path, args), absent);
        assert(!player.CreateObject(path, nullptr, args)); args.override_count = -1;
        fail_malloc = true;
        FASValue arg(EC_EventParm); args.AddItem(arg);
        Default(player.Invoke(path, args), absent); Default(object.Invoke(path, args), absent);
        assert(!player.CreateObject(path, nullptr, args)); fail_malloc = false;
        args.Empty(); assert(player.CreateObject(path, nullptr, args));

        view.array.resize(3);
        for (INT i = 0; i < 3; ++i) {
            view.array[i].SetInt(11 * (i + 1)); view.array[i].number = i + 1.5;
            view.array[i].text = "text"; view.array[i].wide = L"text";
        }
        TArray<FASValue> values; TArray<INT> ints; TArray<FLOAT> floats; TArray<FString> text;
        assert(player.GetVariableArray(path, 1, values) && values.Num() == 2 && values(0).I == 22 && values(1).I == 33);
        assert(player.GetVariableIntArray(path, 1, ints) && ints.values == std::vector<INT>({22, 33}));
        assert(player.GetVariableFloatArray(path, 1, floats) && floats.values == std::vector<FLOAT>({2.5f, 3.5f}));
        assert(player.GetVariableStringArray(path, 1, text) && text.Num() == 2 && std::basic_string<TCHAR>(*text(0)) == TEXT("text"));
        for (INT bad : {-2147483647 - 1, -1, 4, MAXINT}) {
            UINT calls = view.calls;
            assert(!player.GetVariableArray(path, bad, values)); assert(!player.GetVariableIntArray(path, bad, ints));
            assert(!player.GetVariableFloatArray(path, bad, floats)); assert(!player.GetVariableStringArray(path, bad, text));
            assert(view.calls == calls && values.Num() == 2 && ints(0) == 22 && floats(0) == 2.5f && text.Num() == 2);
        }
        view.array_success = false; view.partial = true;
        assert(!player.GetVariableArray(path, 0, values) && values.Num() == 2 && values(0).I == 22);
        assert(!player.GetVariableIntArray(path, 0, ints) && ints(0) == 22);
        assert(!player.GetVariableFloatArray(path, 0, floats) && floats(0) == 2.5f);
        assert(!player.GetVariableStringArray(path, 0, text) && text.Num() == 2);
        view.array_success = true; view.partial = false;
        for (UINT total : {UINT(MAXINT), UINT(MAXINT) + 1, UINT(-1)}) {
            view.report_total = true; view.reported_total = total; UINT calls = view.calls;
            assert(!player.GetVariableArray(path, 0, values)); assert(!player.GetVariableIntArray(path, 0, ints));
            assert(!player.GetVariableFloatArray(path, 0, floats)); assert(!player.GetVariableStringArray(path, 0, text));
            assert(view.calls == calls);
        }
        view.report_total = false;
        view.null_string = true; assert(player.GetVariableStringArray(path, 0, text) && text.Num() == 3 && !text(0).data);
        view.null_string = false;
        assert(player.GetVariableArray(path, 3, values) && !values.Num());
        assert(player.GetVariableIntArray(path, 3, ints) && !ints.Num());
        assert(player.GetVariableFloatArray(path, 3, floats) && !floats.Num());
        assert(player.GetVariableStringArray(path, 3, text) && !text.Num());
        view.array.clear();
        assert(player.GetVariableArray(path, 0, values) && !values.Num());
        assert(player.GetVariableIntArray(path, 0, ints) && !ints.Num());
        assert(player.GetVariableFloatArray(path, 0, floats) && !floats.Num());
        assert(player.GetVariableStringArray(path, 0, text) && !text.Num());
        view.array_success = false;
        assert(!player.GetVariableArray(path, 0, values)); assert(!player.GetVariableIntArray(path, 0, ints));
        assert(!player.GetVariableFloatArray(path, 0, floats)); assert(!player.GetVariableStringArray(path, 0, text));
        view.array_success = true;
        assert(player.SetVariableArray(path, 0, values) && view.last_count == 0);
        assert(player.SetVariableIntArray(path, 0, ints) && view.last_count == 0);
        assert(player.SetVariableFloatArray(path, 0, floats) && view.last_count == 0);
        assert(player.SetVariableStringArray(path, 0, text) && view.last_count == 0);
        values.AddItem(arg); ints.AddItem(1); floats.AddItem(2); text.AddItem(FString(TEXT("string")));
        for (INT bad : {-1, MAXINT}) {
            UINT calls = view.calls;
            assert(!player.SetVariableArray(path, bad, values)); assert(!player.SetVariableIntArray(path, bad, ints));
            assert(!player.SetVariableFloatArray(path, bad, floats)); assert(!player.SetVariableStringArray(path, bad, text));
            assert(view.calls == calls);
        }
        // Large UI arrays must not consume the thread's stack.
        values.values.resize(20000, arg); text.values.resize(20000, FString(TEXT("entry")));
        assert(player.SetVariableArray(path, 0, values) && view.last_count == 20000);
        assert(player.SetVariableStringArray(path, 0, text) && view.last_count == 20000);
        fail_malloc = true; assert(!player.SetVariableArray(path, 0, values)); fail_malloc = false;
'''
    tests += r'''
    }
    assert(strings.empty() && buffers.empty() && !GFx::Value::live);
    puts("Scaleform values/scalars handle failed lookups with poisoned conversion payloads; array and return regressions passed");
}
'''
    return tests
