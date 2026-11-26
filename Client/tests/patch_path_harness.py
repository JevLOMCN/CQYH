"""Real character conversions and mocked Win32 path/scope failure cases."""


def fixtures(client, source):
    parts = [r'''
namespace PathMock {
template<class Character> struct State {
    std::basic_string<Character> current;
    size_t module_length = 5;
    bool query_ok = true, read_ok = true, set_ok = true, grew = false, module_ok = true;
    int changes = 0;
};
template<class Character> State<Character>& Get() { static State<Character> state; return state; }
template<class Character> DWORD Read(DWORD capacity, Character* output) {
    auto& state = Get<Character>();
    if (!capacity) return state.query_ok ? (DWORD)state.current.size() + 1 : 0;
    if (!state.read_ok) return 0;
    if (state.grew) return capacity + 10;
    if (state.current.size() >= capacity) return (DWORD)state.current.size() + 1;
    std::copy(state.current.begin(), state.current.end(), output);
    output[state.current.size()] = 0;
    return (DWORD)state.current.size();
}
template<class Character> BOOL Change(const Character* path) {
    auto& state = Get<Character>();
    if (!state.set_ok || !path) return FALSE;
    state.current = path; ++state.changes; return TRUE;
}
template<class Character> DWORD Module(Character* output, DWORD capacity) {
    auto& state = Get<Character>();
    if (!state.module_ok) return 0;
    size_t length = std::min(state.module_length, (size_t)capacity);
    std::fill(output, output + length, (Character)'M');
    if (length < capacity) output[length] = 0;
    return (DWORD)length;
}
}
DWORD MockPathDirectory(DWORD n, char* output) { return PathMock::Read(n, output); }
DWORD MockPathDirectory(DWORD n, wchar_t* output) { return PathMock::Read(n, output); }
BOOL MockPathChange(const char* path) { return PathMock::Change(path); }
BOOL MockPathChange(const wchar_t* path) { return PathMock::Change(path); }
DWORD MockPathModule(HMODULE, char* output, DWORD n) { return PathMock::Module(output, n); }
DWORD MockPathModule(HMODULE, wchar_t* output, DWORD n) { return PathMock::Module(output, n); }
#define GetCurrentDirectoryA MockPathDirectory
#define GetCurrentDirectoryW MockPathDirectory
#define SetCurrentDirectoryA MockPathChange
#define SetCurrentDirectoryW MockPathChange
#define GetModuleFileNameA MockPathModule
#define GetModuleFileNameW MockPathModule
''', source(client / "client/PatchTools/PatchPath.h"), r'''
#undef GetCurrentDirectoryA
#undef GetCurrentDirectoryW
#undef SetCurrentDirectoryA
#undef SetCurrentDirectoryW
#undef GetModuleFileNameA
#undef GetModuleFileNameW
namespace PatchPathTests {
template<class Character> void Test() {
    using ClientPatch::ScopedCurrentDirectory;
    auto& state = PathMock::Get<Character>();
    const std::basic_string<Character> temporary(4, (Character)'T');
    for (unsigned char pattern : {0xa5, 0x5a}) {
        using Scope = ScopedCurrentDirectory<Character>;
        alignas(Scope) unsigned char storage[sizeof(Scope)];
        memset(storage, pattern, sizeof(storage));
        state.changes = 0;
        Scope* scope = new(storage) Scope;
        scope->~Scope();
        assert(!state.changes);
    }
    for (int length : {1, 259, 260, 1023, 1024, 2048}) {
        state = PathMock::State<Character>();
        state.current.assign(length, (Character)'C');
        auto original = state.current;
        {
            ScopedCurrentDirectory<Character> scope;
            assert(scope.Change(temporary.c_str()));
            assert(state.current == temporary && state.changes == 1);
            assert(!scope.Change(temporary.c_str()));
        }
        assert(state.current == original && state.changes == 2);
        {
            ScopedCurrentDirectory<Character> scope;
            assert(scope.Change(temporary.c_str()) && scope.Restore());
            assert(scope.Restore());
        }
        assert(state.current == original && state.changes == 4);
    }
    for (int failure = 0; failure < 5; ++failure) {
        state = PathMock::State<Character>();
        state.current.assign(5, (Character)'C');
        auto original = state.current;
        switch (failure) {
        case 0: state.query_ok = false; break;
        case 1: state.read_ok = false; break;
        case 2: state.grew = true; break;
        case 3: state.set_ok = false; break;
        case 4: break;
        }
        {
            ScopedCurrentDirectory<Character> scope;
            assert(!scope.Change(failure == 4 ? nullptr : temporary.c_str()));
        }
        assert(state.current == original && !state.changes);
    }
    for (int length : {1, 7, 8, 9, 2048}) {
        state = PathMock::State<Character>();
        state.current.assign(length, (Character)'C');
        state.module_length = length;
        struct Guarded { Character before, path[8], after; } buffer;
        buffer.before = 'A'; buffer.after = 'Z';
        std::fill(buffer.path, buffer.path + 8, (Character)'?');
        assert(ClientPatch::ReadCurrentDirectory(buffer.path) == (length < 8));
        if (length >= 8) assert(!buffer.path[0]);
        assert(buffer.before == 'A' && buffer.after == 'Z');
        assert(ClientPatch::ReadModulePath(buffer.path) == (length < 8));
        if (length >= 8) assert(!buffer.path[0]);
        assert(buffer.before == 'A' && buffer.after == 'Z');
        state.read_ok = state.module_ok = false;
        assert(!ClientPatch::ReadCurrentDirectory(buffer.path) && !buffer.path[0]);
        assert(!ClientPatch::ReadModulePath(buffer.path) && !buffer.path[0]);
    }
}
void Run() {
    Test<char>(); Test<wchar_t>();
    for (int length : {0, 1, 7, 8, 260, 2048}) {
        std::string input(length, 'P');
        std::wstring wide_input(length, L'P');
        struct Wide { wchar_t before, path[8], after; } wide;
        struct Narrow { char before, path[8], after; } narrow;
        wide.before = narrow.before = 'A'; wide.after = narrow.after = 'Z';
        assert(ClientPatch::ToWide(input.c_str(), wide.path) == (length < 8));
        assert(ClientPatch::ToAnsi(wide_input.c_str(), narrow.path) == (length < 8));
        if (length >= 8) assert(!wide.path[0] && !narrow.path[0]);
        assert(wide.before == 'A' && wide.after == 'Z');
        assert(narrow.before == 'A' && narrow.after == 'Z');
    }
    char narrow[8]; wchar_t wide[8];
    Utility2::PathString patch_string("path");
    ArchiveTaskTests::CString dialog_string(L"path");
    assert(ClientPatch::ToWide(patch_string, wide) && !wcscmp(wide, L"path"));
    assert(ClientPatch::ToAnsi(dialog_string, narrow) && !strcmp(narrow, "path"));
    assert(!ClientPatch::ToWide(nullptr, wide) && !wide[0]);
    assert(!ClientPatch::ToAnsi(nullptr, narrow) && !narrow[0]);
    puts("ANSI/wide path capacities, failures and directory-restoration regressions passed");
}
}
''']
    return "\n".join(parts)
