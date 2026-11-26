"""Real string search/extension helpers from all four ActorX tool variants."""
import re


def harness(client, source, function):
    root = client / 'client/UE3/Development/Tools'
    folders = ['ActorX_Plugin/ActorX_3DSMAX', 'ActorX/ActorX_3DSMAX',
               'ActorX/ActorX_MAYA', 'ActorX/ActorX_XSI']
    parts = ['#include <cctype>\n#include <climits>']
    for number, folder in enumerate(folders):
        text = source(root / folder / 'UnSkeletal.cpp')
        parts.append('namespace ActorXString%d {' % number)
        parts.append(r'''
char* _tcscpy(char* output, const char* input) { return strcpy(output, input); }
INT _tcscpy_s(char* output, size_t capacity, const char* input) { return strcpy_s(output, capacity, input); }
char* _strlwr(char* text) {
    for (char* p = text; *p; ++p) *p = char(std::tolower((unsigned char)*p));
    return text;
}
''')
        for signature in ['UBOOL FindSubString(', 'UBOOL RemoveExtString(']:
            parts.append(function(text, signature))
        if folder.endswith('ActorX_3DSMAX'):
            max_text = source(root / folder / 'MaxInterface.cpp')
            branch = function(max_text, 'if (!RemoveExtString(data.cFileName,')
            parts.extend([r'''
void BatchName(const std::string& name, char (&newsequencename)[260], INT& Errors) {
    struct { char cFileName[260]; } data;
    assert(name.size() < sizeof(data.cFileName));
    memcpy(data.cFileName, name.c_str(), name.size() + 1);
    do {
''', branch, '} while (false);\n}', r'''
void BatchTests() {
    char output[260];
    for (const std::string& base : {std::string("%n%s%12345s"), std::string(255, 'A'), std::string("walk")}) {
        INT errors = 0;
        BatchName(base + ".max", output, errors);
        assert(!errors && output == base);
    }
    strcpy(output, "old"); INT errors = 0;
    BatchName("no-extension", output, errors); assert(errors == 1 && !output[0]);
}
'''])
        else:
            parts.append('void BatchTests() {}')
        parts.append(r'''
void Run() {
    INT position = 999;
    assert(!FindSubString(nullptr, "a", position) && !position);
    position = 999; assert(!FindSubString("a", nullptr, position) && !position);
    assert(FindSubString("Walk.MAX", ".max", position) && position == 4);
    assert(FindSubString("Walk", "WALK", position) && position == 0);
    assert(!FindSubString("Walk", "jump", position) && position == 0);
    for (size_t prefix : {size_t(0), size_t(1), size_t(31), size_t(63), size_t(255), size_t(256), size_t(300)}) {
        std::string input(prefix, 'A'); input += ".MAX";
        for (size_t capacity : {size_t(0), size_t(1), size_t(32), size_t(64), size_t(260)}) {
            std::vector<BYTE> bytes(capacity + 32, 0xab);
            char* output = reinterpret_cast<char*>(bytes.data() + 16);
            bool fits = prefix > 0 && prefix < capacity;
            assert(RemoveExtString(input.c_str(), ".max", output, capacity) == fits);
            if (capacity) assert(std::string(output) == (fits ? std::string(prefix, 'A') : ""));
            for (size_t i = 0; i < 16; ++i) assert(bytes[i] == 0xab && bytes[16 + capacity + i] == 0xab);
        }
    }
    char overlap[64] = "walk.MAX";
    assert(RemoveExtString(overlap, ".max", overlap, sizeof(overlap)) && !strcmp(overlap, "walk"));
    strcpy(overlap, "old");
    assert(!RemoveExtString(nullptr, ".max", overlap, sizeof(overlap)) && !*overlap);
    assert(!RemoveExtString("walk.max", ".max", nullptr, 10));
    BatchTests();
}
}
''')
    parts.append(r'''
void RunActorXStringTests() {
    ActorXString0::Run(); ActorXString1::Run(); ActorXString2::Run(); ActorXString3::Run();
    puts("ActorX string helpers use matching array deletion, bound extension outputs and treat percent-sign filenames as literal data (four copies)");
}
''')
    return '\n'.join(parts)
