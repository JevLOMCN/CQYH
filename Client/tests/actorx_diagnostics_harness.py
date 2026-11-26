"""Actual ActorX warning messages and bitmap diagnostics with long/missing data."""
import re


def harness(client, source, function):
    root = client / 'client/UE3/Development/Tools'

    def clean(path):
        return re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                      lambda m: '' if m.group().startswith(('//', '/*')) else m.group(), source(path))

    parts = [r'''
#include <cmath>
#pragma push_macro("_T")
#undef _T
#define _T(x) x
namespace ActorXDiagnosticTests {
struct TSTR {
    static INT live;
    std::string value;
    TSTR(const char* text = "") : value(text ? text : "") { ++live; }
    TSTR(const TSTR& other) : value(other.value) { ++live; }
    ~TSTR() { --live; }
    INT Length() const { return (INT)value.size(); }
    TSTR Substr(INT start, INT count) const { return value.substr(start, count).c_str(); }
    bool operator==(const TSTR& other) const { return value == other.value; }
    TSTR& operator+=(const TSTR& other) { value += other.value; return *this; }
    const char* data() const { return value.c_str(); }
};
INT TSTR::live = 0;
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    T& operator[](INT index) { return values.at(index); }
    void AddItem(const T& value) { values.push_back(value); }
};
enum { BONE_CLASS_ID = 2, HELPER_CLASS_ID = 3, DUMMY_CLASS_ID = 1, GEOM_CLASS_ID = 4, MB_ICONQUESTION = 1, MB_OK = 2 };
struct Object { INT type = BONE_CLASS_ID; INT SuperClassID() const { return type; } } bone, geometry{GEOM_CLASS_ID};
using Interval = INT;
using TimeValue = INT;
struct Matrix3 { FLOAT scale; FLOAT GetRow(INT) const { return scale; } };
FLOAT Length(FLOAT value) { return fabsf(value); }
struct INode {
    std::string name;
    std::vector<INode*> children;
    Object* object = nullptr;
    FLOAT scale = 1;
    INT NumberOfChildren() const { return (INT)children.size(); }
    INode* GetChildNode(INT i) { return children.at(i); }
    const char* GetName() const { return name.c_str(); }
    Object* GetObjectRef() { return object; }
    Matrix3 GetNodeTM(INT, Interval* interval) { *interval = 0; return {scale}; }
};
struct Interface {
    INode* root = nullptr;
    INode* left = nullptr;
    INode* right = nullptr;
    INode* GetRootNode() { return root; }
    INode* GetINodeByName(const char* name) { return !strcmp(name, "Weapon_L") ? left : right; }
    INT GetTime() { return 0; }
    INT GetMAXHWnd() { return 0; }
} core;
Interface* GetCOREInterface() { return &core; }
Interface* TheMaxInterface = &core;
std::string message;
INT boxes = 0;
INT MessageBox(INT, const char* text, const char*, INT) { message = text; ++boxes; return 0; }
''']
    text = clean(root / 'ActorX_Plugin/ActorX_3DSMAX/MaxInterface.cpp')
    for signature in ['void GetsAllNodeChildFn(', 'BOOL GetNodeNameCheckSpaceStr(',
                      'BOOL bCheckAnimNameSpace(', 'void GetXScaleValueOfBones(', 'void InvalidGeomObj(']:
        parts.append(function(text, signature))
    parts.append(r'''
void Run() {
    GetsAllNodeChildFn(nullptr, nullptr);
    INode root, node; root.children = {&node}; core.root = &root;
    GetsAllNodeChildFn(&root, nullptr);
    for (size_t length : {size_t(0), size_t(1), size_t(99), size_t(100), size_t(1000), size_t(65536)}) {
        std::string name = "%n%s" + std::string(length, 'A');
        node.name = name; boxes = 0;
        assert(!GetNodeNameCheckSpaceStr() && !boxes && !TSTR::live);
        assert(!bCheckAnimNameSpace(&name[0]) && !boxes && !TSTR::live);
        name += ' '; node.name = name;
        assert(GetNodeNameCheckSpaceStr() && boxes == 1 && message.find(name) != std::string::npos && !TSTR::live);
        assert(bCheckAnimNameSpace(&name[0]) && boxes == 2 && message.find(name) != std::string::npos && !TSTR::live);
        node.object = &bone; node.scale = 2;
        GetXScaleValueOfBones(); assert(boxes == 3 && message.find(name) != std::string::npos && !TSTR::live);
        node.object = nullptr;
    }
    assert(!bCheckAnimNameSpace(nullptr) && !TSTR::live);
    core.root = nullptr; boxes = 0;
    assert(!GetNodeNameCheckSpaceStr()); GetXScaleValueOfBones(); InvalidGeomObj(); assert(!boxes);
    INode left, right, lchild, rchild;
    left.children = {&lchild}; right.children = {&rchild};
    lchild.name.assign(65536, 'L'); rchild.name.assign(65536, 'R');
    lchild.object = rchild.object = &geometry;
    core.left = &left; core.right = &right;
    InvalidGeomObj(); assert(boxes == 1 && message.find(lchild.name) != std::string::npos && message.find(rchild.name) != std::string::npos);
    assert(!TSTR::live);
    puts("ActorX warnings preserve long literal names with no fixed buffers; temporary SDK strings and missing tree nodes remain safe");
}
}
''')
    folders = ['ActorX_Plugin/ActorX_3DSMAX', 'ActorX/ActorX_3DSMAX', 'ActorX/ActorX_MAYA', 'ActorX/ActorX_XSI']
    for number, folder in enumerate(folders):
        parts.extend(['namespace ActorXBitmap%d {' % number, r'''
enum { MAX_PATH = 260 };
void Memzero(void* buffer, size_t bytes) { memset(buffer, 0, bytes); }
char* _tcscpy(char* output, const char* input) { return strcpy(output, input); }
template<size_t Size> INT _tcscpy_s(char (&output)[Size], const char* input) { return strcpy_s(output, Size, input); }
''', function(clean(root / folder / 'Vertebrate.h'), 'struct VBitmapOrigin\n') + ';', r'''
template<class T> using TArray = ActorXDiagnosticTests::TArray<T>;
struct Actor {
    struct Skin { TArray<void*> RawMaterials; TArray<VBitmapOrigin> RawBMPaths; } SkinData;
};
std::pair<std::string, std::string> Report(Actor* Thing, INT i) {
'''])
        scene = clean(root / folder / 'SceneIFC.cpp')
        start = scene.index('char BitmapName[MAX_PATH]')
        end = scene.index('DebugBox("Retrieved bitmapname");', start)
        parts.extend([scene[start:end], 'return {BitmapName, BitmapPath};\n}'])
        guards = [match.start() for match in re.finditer(r'if \(Thing->SkinData.RawBMPaths.Num\(\) > i\)', scene)]
        assert len(guards) == 3, (folder, len(guards))
        for index, start in enumerate(guards):
            parts.extend(['std::pair<std::string, std::string> Pair%d(Actor* Thing, INT i) {' % index,
                          'char BitmapName[MAX_PATH] = {0}, BitmapPath[MAX_PATH] = {0};',
                          function(scene[start:], 'if ('), 'return {BitmapName, BitmapPath};\n}'])
        parts.append(r'''
void Run() {
    alignas(VBitmapOrigin) BYTE bytes[sizeof(VBitmapOrigin)]; memset(bytes, 0xff, sizeof(bytes));
    auto* record = new(bytes) VBitmapOrigin;
    for (BYTE value : bytes) assert(value == 0);
    Actor actor;
    auto missing = Report(&actor, 0); assert(missing.first == "[Internal material]");
    assert(missing.second == "" || missing.second == "[-]");
    actor.SkinData.RawMaterials.AddItem((void*)1);
    for (auto reader : {Pair0, Pair1, Pair2}) {
        auto absent = reader(&actor, 0); assert(absent.first.empty() && absent.second.empty());
    }
    VBitmapOrigin bitmap;
    strcpy(bitmap.RawBitmapName, "literal%n.bmp");
    memset(bitmap.RawBitmapPath, 'P', sizeof(bitmap.RawBitmapPath) - 1);
    bitmap.RawBitmapPath[sizeof(bitmap.RawBitmapPath) - 1] = 0;
    actor.SkinData.RawBMPaths.AddItem(bitmap);
    for (auto reader : {Report, Pair0, Pair1, Pair2}) {
        auto value = reader(&actor, 0);
        assert(value.first == bitmap.RawBitmapName && value.second == std::string(259, 'P'));
    }
    actor.SkinData.RawBMPaths.values.clear();
    auto absent = Report(&actor, 0); assert(absent.second.empty() || absent.second == "[-]");
}
}
''')
    parts.append(r'''
void RunActorXDiagnosticTests() {
    ActorXDiagnosticTests::Run();
    ActorXBitmap0::Run(); ActorXBitmap1::Run(); ActorXBitmap2::Run(); ActorXBitmap3::Run();
    puts("ActorX bitmap records and report paths initialize on missing data; all metadata array reads are bounded (four variants)");
}
#pragma pop_macro("_T")
''')
    return '\n'.join(parts)
