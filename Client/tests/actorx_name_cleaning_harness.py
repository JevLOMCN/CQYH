"""ActorX name sanitation uses the actual destination capacity in four tools."""
import re


def harness(client, source, function):
    root = client / 'client/UE3/Development/Tools'
    folders = ['ActorX_Plugin/ActorX_3DSMAX', 'ActorX/ActorX_3DSMAX',
               'ActorX/ActorX_MAYA', 'ActorX/ActorX_XSI']

    def clean(path):
        return re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                      lambda m: '' if m.group().startswith(('//', '/*')) else m.group(), source(path))

    parts = []
    for index, folder in enumerate(folders):
        directory = root / folder
        parts.extend(['namespace ActorXNameCleaning%d {' % index, r'''
using DWORD = uint32_t;
using TCHAR = char;
enum { CTL_CHARS = 31, SINGLE_QUOTE = 39 };
void Memzero(void* output, size_t size) { memset(output, 0, size); }
''', function(clean(directory / 'UnSkeletal.cpp'), 'void CleanString('),
                      function(clean(directory / 'Vertebrate.h'), 'struct VMaterial\n') + ';'])
        filename = ('MaxInterface.cpp' if folder.endswith('3DSMAX') else
                    'MayaInterface.cpp' if folder.endswith('MAYA') else 'XSIInterface.cpp')
        interface = clean(directory / filename)
        name = 'SequenceName' if folder.endswith('MAYA') else 'AnimName'
        # Compile each actual call so a later change cannot silently lose sizeof.
        call = re.search(r'CleanString\(' + name + r', Thing->RawAnimName, sizeof\(Thing->RawAnimName\)\);', interface)
        assert call, folder
        parts.extend(['struct Actor { char RawAnimName[64]; };',
                      'void AnimationName(Actor* Thing, const char* ' + name + ') {', call[0], '}'])
        scene = clean(directory / 'SceneIFC.cpp')
        start = scene.index('char MaterialName[sizeof(')
        end = scene.index(';', scene.index('CleanString(', start)) + 1
        parts.extend([r'''
struct Skin { std::vector<VMaterial> Materials; };
struct Model { Skin SkinData; };
std::string ReportName(Model* Thing, INT i) {
''', scene[start:end], 'return MaterialName;\n}'])
        if filename == 'MaxInterface.cpp':
            parts.extend([r'''
struct TSTR {
    std::string text;
    TSTR(const char* input = "") : text(input) {}
    operator const char*() const { return text.c_str(); }
};
enum { BMTEX_CLASS_ID = 3, ID_DI = 1 };
struct Class_ID {
    INT first, second;
    Class_ID(INT a, INT b) : first(a), second(b) {}
    bool operator==(const Class_ID& other) const { return first == other.first && second == other.second; }
};
struct Texmap { INT type = BMTEX_CLASS_ID; Class_ID ClassID() const { return {type, 0}; } };
struct BitmapTex : Texmap {
    std::string filename;
    const char* GetMapName() const { return filename.c_str(); }
};
struct Mtl { Texmap* map = nullptr; Texmap* GetSubTexmap(INT) { return map; } };
void SplitPathFile(const TSTR& input, TSTR* path, TSTR* file) {
    size_t slash = input.text.rfind('\\');
    *path = slash == std::string::npos ? "" : input.text.substr(0, slash).c_str();
    *file = slash == std::string::npos ? input : input.text.substr(slash + 1).c_str();
}
''', function(interface, 'void GetBitmapName('), r'''
void BitmapTests() {
    BitmapTex texture;
    Mtl material; material.map = &texture;
    struct Guarded { BYTE before[16]; char value[260]; BYTE after[16]; } name, path;
    for (size_t length : {size_t(0), size_t(63), size_t(255), size_t(259), size_t(260), size_t(65536)}) {
        texture.filename = std::string(length, 'P') + "\\" + std::string(length, 'F');
        memset(&name, 0xab, sizeof(name)); memset(&path, 0xab, sizeof(path));
        GetBitmapName(name.value, sizeof(name.value), path.value, sizeof(path.value), &material);
        assert(name.value == std::string(length < 259 ? length : 259, 'F'));
        assert(path.value == std::string(length < 259 ? length : 259, 'P'));
        for (INT i = 0; i < 16; ++i) assert(name.before[i] == 0xab && name.after[i] == 0xab && path.before[i] == 0xab && path.after[i] == 0xab);
    }
    for (INT missing = 0; missing < 3; ++missing) {
        memset(name.value, 0xab, sizeof(name.value)); memset(path.value, 0xab, sizeof(path.value));
        texture.type = missing == 2 ? 999 : BMTEX_CLASS_ID;
        material.map = missing == 1 ? nullptr : &texture;
        GetBitmapName(name.value, sizeof(name.value), path.value, sizeof(path.value), missing == 0 ? nullptr : &material);
        for (char c : name.value) assert(!c);
        for (char c : path.value) assert(!c);
    }
    texture.type = BMTEX_CLASS_ID; material.map = &texture; texture.filename = "path\\file";
    GetBitmapName(nullptr, 0, nullptr, 0, &material);
    char smallName[2] = {'x', 'x'}, smallPath[1] = {'x'};
    GetBitmapName(smallName, sizeof(smallName), smallPath, sizeof(smallPath), &material);
    assert(!strcmp(smallName, "f") && !smallPath[0]);
}
'''])
        else:
            parts.append('void BitmapTests() {}')
        parts.append(r'''
void Run() {
    for (size_t length : {size_t(0), size_t(1), size_t(62), size_t(63), size_t(64), size_t(255), size_t(256), size_t(259), size_t(65536)}) {
        std::string input(length, 'A');
        for (size_t capacity : {size_t(0), size_t(1), size_t(2), size_t(64), size_t(256), size_t(260)}) {
            std::vector<BYTE> bytes(capacity + 32, 0xab);
            char* output = reinterpret_cast<char*>(bytes.data() + 16);
            CleanString(input.c_str(), output, capacity);
            if (capacity) {
                size_t copied = length < capacity - 1 ? length : capacity - 1;
                assert(output == std::string(copied, 'A'));
                for (size_t i = copied; i < capacity; ++i) assert(!output[i]);
            }
            for (size_t i = 0; i < 16; ++i) assert(bytes[i] == 0xab && bytes[capacity + 16 + i] == 0xab);
        }
        Actor actor; memset(&actor, 0xab, sizeof(actor));
        AnimationName(&actor, input.c_str());
        assert(actor.RawAnimName == std::string(length < 63 ? length : 63, 'A'));
        alignas(VMaterial) BYTE bytes[sizeof(VMaterial)]; memset(bytes, 0xab, sizeof(bytes));
        VMaterial* material = new(bytes) VMaterial;
        for (BYTE b : bytes) assert(!b);
        material->TextureIndex = 0x12345678;
        material->SetName(input.c_str());
        assert(material->TextureIndex == 0x12345678 && material->MaterialName == std::string(length < 63 ? length : 63, 'A'));
        material->SetName(material->MaterialName);
        assert(material->MaterialName == std::string(length < 63 ? length : 63, 'A'));
        material->SetName(nullptr);
        for (char c : material->MaterialName) assert(!c);
        material->~VMaterial();
    }
    CleanString(nullptr, nullptr, 0); CleanString("test", nullptr, 10);
    char overlap[64] = "a\"\tb\n";
    CleanString(overlap, overlap, sizeof(overlap)); assert(!strcmp(overlap, "a'_b_"));
    strcpy(overlap, "a\"\tb\n");
    CleanString(overlap, overlap + 1, sizeof(overlap) - 1); assert(!strcmp(overlap + 1, "a'_b_"));
    CleanString(nullptr, overlap, sizeof(overlap));
    for (char c : overlap) assert(!c);
    char high[] = {char(0x80), char(0xff), 0};
    CleanString(high, overlap, sizeof(overlap)); assert(!strcmp(high, overlap));
    // A fixed binary material field may contain no terminator at all.
    Model model; model.SkinData.Materials.resize(1);
    memset(&model.SkinData.Materials[0], 'M', sizeof(VMaterial));
    assert(ReportName(&model, 0) == std::string(63, 'M'));
    char* exact = new char[64]; memset(exact, 'R', 64);
    CleanString(exact, overlap, sizeof(overlap)); assert(overlap == std::string(63, 'R'));
    VMaterial material; material.SetName(exact); assert(material.MaterialName == std::string(63, 'R'));
    delete[] exact;
    BitmapTests();
}
}
''')
    parts.append(r'''
void RunActorXNameCleaningTests() {
    ActorXNameCleaning0::Run(); ActorXNameCleaning1::Run(); ActorXNameCleaning2::Run(); ActorXNameCleaning3::Run();
    puts("ActorX cleaners bound animation/material/bitmap names, terminate full material fields and initialize all XSI material members (four variants)");
}
''')
    return '\n'.join(parts)
