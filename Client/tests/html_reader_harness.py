"""Test the production HTML file loader with failed and partial Win32 reads."""


def harness(game, source, function):
    parts = [r'''
namespace HtmlTests {
using FString = NetworkTests::FString;
using DWORD = uint32_t;
using HANDLE = void*;
struct LARGE_INTEGER { INT64 QuadPart; };
enum { GENERIC_READ = 1, FILE_SHARE_READ = 2, OPEN_EXISTING = 3, FILE_ATTRIBUTE_NORMAL = 4 };
#ifndef MAXINT
#define MAXINT 0x7fffffff
#endif
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#define UTF8_TO_TCHAR(s) (s)
#define CreateFile HtmlTests::Create
struct File {
    bool open = true, sizeWorks = true, readWorks = true;
    INT64 size = 0;
    INT reads = 0, closes = 0;
    std::string bytes;
} file;
HANDLE Create(const char*, DWORD, DWORD, void*, DWORD, DWORD, void*) {
    return file.open ? &file : INVALID_HANDLE_VALUE;
}
BOOL GetFileSizeEx(HANDLE handle, LARGE_INTEGER* size) {
    assert(handle == &file);
    if (!file.sizeWorks) return FALSE;
    size->QuadPart = file.size;
    return TRUE;
}
BOOL ReadFile(HANDLE handle, void* destination, DWORD requested, DWORD* read, void*) {
    assert(handle == &file);
    ++file.reads;
    const DWORD count = file.bytes.size() < requested ? (DWORD)file.bytes.size() : requested;
    if (count) memcpy(destination, file.bytes.data(), count);
    *read = count;
    return file.readWorks ? TRUE : FALSE;
}
BOOL CloseHandle(HANDLE handle) { assert(handle == &file); ++file.closes; return TRUE; }
struct WebHTMLContent { BOOL HtmlReadFile(const FString&, FString&) const; };
struct WebHTMLContentCopy { BOOL HtmlReadFile(const FString&, FString&) const; };
''']
    files = [game / "WebHtmlManager.cpp", *game.glob("WebHtmlManager - *.cpp")]
    if len(files) != 2:
        raise ValueError("Expected the active and legacy HTML reader copies")
    for index, path in enumerate(files):
        body = function(source(path), "BOOL WebHTMLContent::HtmlReadFile(")
        if index:
            body = body.replace("WebHTMLContent::", "WebHTMLContentCopy::", 1)
        parts.append(body)
    parts.append(r'''
template<class Loader> void Reader() {
    Loader loader;
    FString content = "original";
    file = File(); file.open = false;
    assert(!loader.HtmlReadFile("ignored", content) && !file.closes && !file.reads);
    assert(content.value == "original");
    for (INT64 size : {-1ll, (INT64)MAXINT, 0xffffffffll, 0x100000000ll}) {
        file = File(); file.size = size;
        assert(!loader.HtmlReadFile("ignored", content));
        assert(file.closes == 1 && !file.reads && content.value == "original");
    }
    file = File(); file.sizeWorks = false;
    assert(!loader.HtmlReadFile("ignored", content));
    assert(file.closes == 1 && !file.reads);
    for (bool success : {false, true}) {
        file = File(); file.size = 20; file.bytes = "partial"; file.readWorks = success;
        assert(!loader.HtmlReadFile("ignored", content));
        assert(file.closes == 1 && file.reads == 1 && content.value == "original");
    }
    file = File(); file.bytes = "complete"; file.size = file.bytes.size(); file.readWorks = false;
    assert(!loader.HtmlReadFile("ignored", content));
    assert(content.value == "original");
    for (const std::string bytes : {std::string(), std::string("complete"), std::string(4097, 'X')}) {
        file = File(); file.bytes = bytes; file.size = bytes.size();
        assert(loader.HtmlReadFile("ignored", content));
        assert(content.value == bytes && file.closes == 1 && file.reads == 1);
    }
}
void Run() {
    Reader<WebHTMLContent>();
    Reader<WebHTMLContentCopy>();
    puts("HTML short-read and size-overflow regressions passed");
}
#undef INVALID_HANDLE_VALUE
#undef UTF8_TO_TCHAR
#undef CreateFile
}
''')
    return "\n".join(parts)
