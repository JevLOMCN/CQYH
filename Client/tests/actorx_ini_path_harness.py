"""Actual ActorX INI setup with all Documents-path lengths and guarded arrays."""
import re


def harness(client, source, function):
    text = source(client / 'client/UE3/Development/Tools/ActorX_Plugin/ActorX_3DSMAX/MaxInterface.cpp')
    text = re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                  lambda m: '' if m.group().startswith(('//', '/*')) else m.group(), text)
    parts = [r'''
#pragma push_macro("_T")
#undef _T
#define _T(x) x
namespace ActorXIniPathTests {
enum { MAX_PATH = 260, CSIDL_PERSONAL = 1, CSIDL_FLAG_CREATE = 2, MB_ICONQUESTION = 1, MB_YESNO = 2 };
using HRESULT = long;
bool FAILED(HRESULT result) { return result < 0; }
struct Guarded { BYTE before[16]; char data[MAX_PATH]; BYTE after[32]; } guarded[6];
char (&IniFilePath)[MAX_PATH] = guarded[0].data;
char (&IniFileName)[MAX_PATH] = guarded[1].data;
char (&IniPathFileName)[MAX_PATH] = guarded[2].data;
char (&IniPSKFileName)[MAX_PATH] = guarded[3].data;
char (&IniPSAFileName)[MAX_PATH] = guarded[4].data;
char (&IniKeyFrameRange)[MAX_PATH] = guarded[5].data;
std::string documents;
bool shellSuccess = true, existing = false, openSuccess = true;
INT pathCalls = 0, opens = 0, closes = 0, directories = 0;
struct MaxInterface { INT GetMAXHWnd() { return 0; } } maxInterface;
MaxInterface* TheMaxInterface = &maxInterface;
HRESULT SHGetFolderPath(INT, INT, INT, INT, char* path) {
    if (!shellSuccess) return -1;
    assert(documents.size() < MAX_PATH); memcpy(path, documents.c_str(), documents.size() + 1); return 0;
}
bool PathFileExists(const char* path) { assert(path && *path && strlen(path) < MAX_PATH); ++pathCalls; return existing; }
INT _mkdir(const char* path) { assert(path && *path && strlen(path) < MAX_PATH); ++directories; return 0; }
template<class... Args> INT MessageBox(Args...) { return 0; }
FILE* TestOpen(const char* path, const char* mode) {
    assert(path && *path && strlen(path) < MAX_PATH && !strcmp(mode, "w"));
    ++opens; return openSuccess ? reinterpret_cast<FILE*>(1) : nullptr;
}
INT TestClose(FILE* file) { assert(file == reinterpret_cast<FILE*>(1)); ++closes; return 0; }
''', function(text, 'template<size_t Size>\nstatic bool SetActorXIniPath('),
             function(text, 'static void ClearActorXIniPaths('),
             function(text, 'void CreateIniFile(').replace('fopen(', 'TestOpen(').replace('fclose(', 'TestClose('), r'''
void Run() {
    const char* leaves[] = {"", "AnimationName.ini", "AnimationPath.ini", "PSKFileName.ini", "PSAFileName.ini", "KeyFrameRange.ini"};
    for (bool filesExist : {false, true}) for (INT length = 0; length < MAX_PATH; ++length) {
        memset(guarded, 0xa5, sizeof(guarded)); documents.assign(length, 'D');
        existing = filesExist; pathCalls = opens = closes = directories = 0;
        std::string directory = documents + "\\AgeOfDawn\\";
        bool fits = directory.size() + strlen("AnimationName.ini") < MAX_PATH;
        CreateIniFile();
        for (INT i = 0; i < 6; ++i) {
            for (BYTE byte : guarded[i].before) assert(byte == 0xa5);
            for (BYTE byte : guarded[i].after) assert(byte == 0xa5);
            assert(std::string(guarded[i].data) == (fits ? directory + leaves[i] : ""));
        }
        assert(opens == (fits && !existing ? 5 : 0) && closes == opens);
        assert(directories == (fits && !existing ? 1 : 0));
        if (!fits) assert(!pathCalls);
    }
    documents = "valid"; existing = false;
    CreateIniFile(); shellSuccess = false; pathCalls = opens = closes = 0;
    CreateIniFile();
    assert(!pathCalls && !opens && !closes);
    for (auto& path : guarded) assert(!*path.data);
    shellSuccess = true; openSuccess = false; pathCalls = opens = closes = 0;
    CreateIniFile(); assert(opens == 5 && closes == 0);
    struct Small { BYTE before[8]; char path[7]; BYTE after[8]; } small;
    for (size_t length : {size_t(0), size_t(1), size_t(6), size_t(7), size_t(1000)}) {
        memset(&small, 0xab, sizeof(small)); std::string text(length, 'X');
        assert(SetActorXIniPath(small.path, text) == (length < sizeof(small.path)));
        assert(std::string(small.path) == (length < sizeof(small.path) ? text : ""));
        for (BYTE byte : small.before) assert(byte == 0xab);
        for (BYTE byte : small.after) assert(byte == 0xab);
    }
    puts("ActorX settings paths bound every Documents-path length, clear failed setup and close newly created files");
}
}
#pragma pop_macro("_T")
''']
    return '\n'.join(parts)
