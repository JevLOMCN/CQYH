"""Real ActorX script methods, dialog capacities and early export-path checks."""
import re


def harness(client, source, function):
    root = client / 'client/UE3/Development/Tools'
    folders = ['ActorX_Plugin/ActorX_3DSMAX', 'ActorX/ActorX_3DSMAX',
               'ActorX/ActorX_MAYA', 'ActorX/ActorX_XSI']

    def clean(path):
        return re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                      lambda m: '' if m.group().startswith(('//', '/*')) else m.group(), source(path))

    def window_calls(text):
        for match in re.finditer(r'GetWindowText\(', text):
            begin = match.end()
            end, depth, commas = begin, 1, []
            while depth:
                c = text[end]
                if c == ',' and depth == 1:
                    commas.append(end)
                depth += (c == '(') - (c == ')')
                end += 1
            assert len(commas) == 2
            # Replace only the SDK window handle; keep the actual array and count.
            yield ('GetWindowText(0' + text[commas[0]:end] + ';',
                   text[commas[0] + 1:commas[1]].strip())

    parts = []
    for number, folder in enumerate(folders):
        directory = root / folder
        filename = ('MaxInterface.cpp' if folder.endswith('3DSMAX') else
                    'MayaInterface.cpp' if folder.endswith('MAYA') else 'XSIInterface.cpp')
        interface = clean(directory / filename)
        header = clean(directory / 'UnSkeletal.h')
        parts.extend(['namespace ActorXExportPath%d {' % number, r'''
using TCHAR = char;
enum { MAX_PATH = 260 };
''', function(header, 'template<size_t Size>\nUBOOL CopyActorXString('),
                      function(header, 'template<size_t Size>\nUBOOL MakeActorXFilePath('), r'''
struct Guarded { BYTE before[16]; char value[MAX_PATH]; BYTE after[16]; } guarded[12];
struct MaterialNames { BYTE before[16]; char value[8][MAX_PATH]; BYTE after[16]; } guardedMaterials;
char (&materialnames)[8][MAX_PATH] = guardedMaterials.value;
char framerangestring[600];
INT errors = 0, ioCalls = 0;
template<class... Args> void ErrorBox(Args...) { ++errors; }
void Reset() {
    memset(guarded, 0xab, sizeof(guarded)); memset(&guardedMaterials, 0xab, sizeof(guardedMaterials));
    for (auto& buffer : guarded) buffer.value[0] = 0;
    for (auto& buffer : materialnames) buffer[0] = 0;
    errors = ioCalls = 0;
}
void CheckGuards() {
    for (auto& buffer : guarded) for (INT i = 0; i < 16; ++i)
        assert(buffer.before[i] == 0xab && buffer.after[i] == 0xab);
    for (INT i = 0; i < 16; ++i) assert(guardedMaterials.before[i] == 0xab && guardedMaterials.after[i] == 0xab);
}
'''])
        names = ['to_path', 'to_animfile', 'to_skinfile', 'to_pathvtx', 'to_skinfilevtx',
                 'to_brushpath', 'to_brushfile', 'batchfoldername', 'KeyRange', 'newsequencename',
                 'LogPath', 'DestPath']
        for i, name in enumerate(names):
            parts.append('char (&%s)[MAX_PATH] = guarded[%d].value;' % (name, i))
        # The SDK headers declare these as complete arrays, allowing sizeof at callers.
        declarations = source(directory / 'SceneIFC.h')
        for name in ['to_path', 'to_animfile', 'to_skinfile', 'DestPath']:
            assert re.search(r'extern char ' + name + r'\[MAX_PATH\]', declarations), (folder, name)
        parts.append(r'''
std::string windowText;
bool windowFailure = false;
template<size_t Size> INT GetWindowText(INT, char (&output)[Size], size_t capacity) {
    assert(capacity > 0 && capacity <= Size);
    if (windowFailure) return 0;
    size_t length = windowText.size() < capacity - 1 ? windowText.size() : capacity - 1;
    memcpy(output, windowText.c_str(), length); output[length] = 0;
    return (INT)length;
}
void ReadWindows() {
    INT MatIdx = 7;
''')
        calls = [(call, name) for call, name in window_calls(interface) if 'sizeof(' in call]
        expected = [14, 5, 7, 8][number]
        assert len(calls) == expected, (folder, len(calls))
        parts.extend(call for call, name in calls)
        parts.append('}\nvoid FailedLocalReads() {\nwindowFailure = true;')
        for local in ['NewSName', 'KeyRange', 'RateString']:
            declaration = re.search(r'char ' + local + r'\[MAX_PATH\] = \{0\};', interface)
            if declaration:
                matching = [(call, name) for call, name in window_calls(interface) if name == local]
                assert len(matching) == 1
                parts.extend(['{', declaration[0], matching[0][0], 'assert(!' + local + '[0]);', '}'])
        parts.append('windowFailure = false;\n}')
        vertex = function(interface, 'int SceneIFC::WriteVertexAnims(')
        prefix = vertex[vertex.index('{') + 1:vertex.index('if (!MakeActorXFilePath')]
        assert prefix.strip() == 'char VertexMeshPath[MAX_PATH], VertexAnimPath[MAX_PATH];'
        parts.extend(['std::string meshPath, animPath;\nbool VertexPaths(char* DestFileName) {',
                      prefix, function(vertex, 'if (!MakeActorXFilePath'),
                      'meshPath = VertexMeshPath; animPath = VertexAnimPath; ++ioCalls; return true;\n}'])
        if filename != 'MayaInterface.cpp':
            dialogs = clean(directory / 'Dialogs.cpp')
            guards = [match.start() for match in re.finditer(r'if \(!MakeActorXFilePath\(', dialogs)]
            assert len(guards) == 2, folder
            for i, start in enumerate(guards):
                parts.extend(['bool Dialog%d() { do {' % i, function(dialogs[start:], 'if ('),
                              '++ioCalls; return true; } while (false); return false; }'])
        else:
            parts.append('bool Dialog0() { return false; } bool Dialog1() { return false; }')
        if filename == 'MaxInterface.cpp':
            parts.append(r'''
struct Scene { UBOOL DoForceRate = false; FLOAT PersistentRate = 30; } OurScene;
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    T& operator[](INT i) { return values.at(i); }
    INT AddZeroed(INT count) { INT first = Num(); values.resize(first + count); return first; }
    void AddItem(const T& value) { values.push_back(value); }
};
struct Animation { INT AnimInfo = 0; TArray<INT> KeyTrack; };
struct Actor { TArray<Animation> Animations, OutAnims; } TempActor;
INT skinSaves = 0, animSaves = 0, clears = 0, digests = 0;
std::string savedPath;
std::vector<Animation> savedAnimations;
void SaveAnimSet(char* path) { ++animSaves; savedPath = path; savedAnimations = TempActor.OutAnims.values; }
void ClearOutAnims() { ++clears; TempActor.Animations.values.clear(); TempActor.OutAnims.values.clear(); }
void DoDigestAnimation() { ++digests; }
template<class... Args> void PopupBox(Args...) { ++errors; }
void SaveCurrentSceneSkin() {
''')
            mesh = function(interface, 'void SaveCurrentSceneSkin()\n')
            assert mesh[mesh.index('{') + 1:].lstrip().startswith('if (!MakeActorXFilePath(')
            parts.extend([function(mesh, 'if (!MakeActorXFilePath'), '++skinSaves;\n}'])
            if number == 0:
                method = function(interface, 'void ExportAnimationData(')
                assert method[method.index('{') + 1:].lstrip().startswith('if (!MakeActorXFilePath(')
                parts.extend(['void GuiAnimation() {', function(method, 'if (!MakeActorXFilePath'), '++ioCalls;\n}'])
            parts.append(r'''
struct Script {
    INT active = 0;
    void InitOptions() { assert(!active); ++active; }
    void ClearOptions() { assert(active == 1); --active; }
''')
            for signature in ['INT AXSetOutputPath(', 'INT AXExportMesh(', 'INT AXDigestAnim(', 'INT AXExportAnimSet(']:
                parts.append(function(interface, signature))
            parts.append(r'''
};
void ScriptTests() {
    Script script;
    for (size_t length : {size_t(0), size_t(1), size_t(254), size_t(255), size_t(256), size_t(259), size_t(260), size_t(65536)}) {
        std::string input(length, 'N');
        for (bool nullInput : {false, true}) {
            Reset(); strcpy(to_path, "dir"); strcpy(LogPath, "old-log");
            char* name = nullInput ? nullptr : &input[0];
            bool fits = !nullInput && length > 0 && length < MAX_PATH;
            assert(script.AXSetOutputPath(name) == fits);
            assert(to_path == (fits ? input : "dir")); assert(LogPath == (fits ? input : "old-log"));
            strcpy(to_path, "dir"); skinSaves = animSaves = clears = digests = 0;
            bool pathFits = fits && 3 + 1 + length + 4 < MAX_PATH;
            assert(script.AXExportMesh(name) == pathFits && skinSaves == pathFits && !script.active);
            assert(script.AXDigestAnim(name, INT_MIN, INT_MAX, 60) == fits && !script.active && !OurScene.DoForceRate);
            if (fits) { assert(newsequencename == input); assert(!strcmp(framerangestring, "-2147483648-2147483647")); }
            TempActor.Animations.values.clear(); TempActor.OutAnims.values.clear();
            Animation a; a.AnimInfo = 7; a.KeyTrack.values = {1, 2, 3};
            TempActor.Animations.AddItem(a); TempActor.Animations.AddItem(a); TempActor.OutAnims.AddItem(a);
            assert(script.AXExportAnimSet(name) == pathFits && animSaves == pathFits && clears == pathFits && !script.active);
            if (pathFits) {
                assert(savedPath == "dir\\" + input + ".PSA" && savedAnimations.size() == 3);
                for (const auto& animation : savedAnimations) assert(animation.AnimInfo == 7 && animation.KeyTrack.values == std::vector<INT>({1, 2, 3}));
            } else assert(TempActor.Animations.Num() == 2 && TempActor.OutAnims.Num() == 1);
            CheckGuards();
        }
    }
    Reset(); memset(to_path, 'P', MAX_PATH - 1); to_path[MAX_PATH - 1] = 0;
    skinSaves = 0; SaveCurrentSceneSkin(); assert(!skinSaves && !DestPath[0]);
}
''')
            if number == 0:
                parts.append('void ExtraGuiTests() { ioCalls = 0; GuiAnimation(); assert(!ioCalls && !DestPath[0]); }')
            else:
                parts.append('void ExtraGuiTests() {}')
        else:
            parts.append('void ScriptTests() {} void ExtraGuiTests() {}')
        directory_name = 'to_path' if filename == 'MayaInterface.cpp' else 'to_pathvtx'
        parts.append('void SetVertexDirectory(const std::string& value) { assert(CopyActorXString(' + directory_name + ', value.c_str())); }')
        parts.append(r'''
void Run() {
    Reset();
    for (size_t length : {size_t(0), size_t(1), size_t(259), size_t(260), size_t(299), size_t(65536)}) {
        windowText.assign(length, 'W'); ReadWindows(); CheckGuards();
        for (auto& buffer : guarded) if (buffer.value[0] == 'W') assert(strlen(buffer.value) == (length < 259 ? length : 259));
    }
    FailedLocalReads();
    struct Small { BYTE before[8]; char value[7]; BYTE after[8]; } small;
    for (size_t length : {size_t(0), size_t(1), size_t(6), size_t(7), size_t(65536)}) {
        memset(&small, 0xab, sizeof(small)); strcpy(small.value, "old"); std::string input(length, 'C');
        assert(CopyActorXString(small.value, input.c_str()) == (length < sizeof(small.value)));
        assert(small.value == (length < sizeof(small.value) ? input : "old"));
        assert(!CopyActorXString(small.value, nullptr));
        assert(CopyActorXString(small.value, small.value));
        for (INT i = 0; i < 8; ++i) assert(small.before[i] == 0xab && small.after[i] == 0xab);
    }
    char* exact = new char[7]; memset(exact, 'X', 7);
    assert(!CopyActorXString(small.value, exact)); delete[] exact;
    for (INT directoryLength = 0; directoryLength < MAX_PATH; ++directoryLength) {
        std::string directory(directoryLength, 'D'); SetVertexDirectory(directory);
        for (INT nameLength : {0, 1, 248, 253, 254, 258, 259, 260, 65536}) {
            std::string name(nameLength, 'N');
            bool fits = directoryLength + 1 + nameLength + 5 < MAX_PATH;
            ioCalls = 0;
            assert(VertexPaths(&name[0]) == fits && ioCalls == fits);
            if (fits) assert(meshPath == directory + "\\" + name + "_d.3d" && animPath == directory + "\\" + name + "_a.3d");
            CheckGuards();
        }
    }
    ioCalls = 0; assert(!VertexPaths(nullptr) && !ioCalls);
    strcpy(DestPath, "dir"); assert(MakeActorXFilePath(DestPath, DestPath, "%n%s", ".PSA"));
    assert(!strcmp(DestPath, "dir\\%n%s.PSA"));
    assert(!MakeActorXFilePath(DestPath, nullptr, "name", ".PSA") && !DestPath[0]);
    strcpy(to_path, "dir"); strcpy(to_animfile, "walk");
    for (auto dialog : {Dialog0, Dialog1}) {
        ioCalls = 0; dialog();
        if (ioCalls) assert(!strcmp(DestPath, "dir\\walk.PSA"));
        memset(to_animfile, 'N', MAX_PATH - 1); to_animfile[MAX_PATH - 1] = 0;
        ioCalls = 0; assert(!dialog() && !ioCalls);
        strcpy(to_animfile, "walk");
    }
    ScriptTests(); ExtraGuiTests();
}
}
''')
    parts.append(r'''
void RunActorXExportPathTests() {
    ActorXExportPath0::Run(); ActorXExportPath1::Run(); ActorXExportPath2::Run(); ActorXExportPath3::Run();
    puts("ActorX script strings and 34 dialog capacities are bounded; invalid mesh/animation/vertex paths reject before file I/O or animation-buffer mutation");
}
''')
    return '\n'.join(parts)
