"""Exercise production font registration against malformed registry/config data."""
import re


def harness(game, source, function):
    parts = [r'''
#undef TEXT
#define TEXT(s) L##s
#define _WINDOWS_
#ifndef ARRAY_COUNT
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#endif
namespace FontTests {
using TCHAR = wchar_t;
using DWORD = unsigned long;
using LONG = long;
using HKEY = void*;
using LPBYTE = unsigned char*;
enum { MAX_PATH = 260, ERROR_SUCCESS = 0, ERROR_ACCESS_DENIED = 5, ERROR_MORE_DATA = 234,
       ERROR_NO_MORE_ITEMS = 259, REG_SZ = 1, REG_EXPAND_SZ = 2, REG_BINARY = 3,
       KEY_READ = 1, HWND_BROADCAST = 1, WM_FONTCHANGE = 2 };
const HKEY HKEY_LOCAL_MACHINE = (HKEY)1;
struct FString {
    std::wstring value;
    FString(const wchar_t* text = L"") : value(text) {}
    FString(std::wstring text) : value(text) {}
    const wchar_t* operator*() const { return value.c_str(); }
    INT Len() const { return (INT)value.size(); }
    FString& operator+=(const FString& other) { value += other.value; return *this; }
    bool operator==(const wchar_t* text) const { return value == text; }
};
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    void Empty() { values.clear(); }
    void AddItem(const T& value) { values.push_back(value); }
    T& operator()(INT i) { assert(i >= 0 && i < Num()); return values[i]; }
};
struct Entry {
    std::wstring name = L"Demo (TrueType)";
    std::wstring data = L"C:\\Windows\\Fonts\\demo.ttf";
    DWORD type = REG_SZ;
    LONG result = ERROR_SUCCESS;
    bool odd_bytes = false, oversized_result = false;
};
struct State {
    bool open_ok = true, query_ok = true, directory_ok = true;
    std::wstring directory = L"C:\\Game";
    INT font_count = 1, names = 1, files = 1;
    int enum_calls = 0, closes = 0, notifications = 0;
    std::vector<Entry> entries;
    std::vector<std::wstring> added;
} state;
struct FFilename : FString {
    FFilename(const FString& text) : FString(text) {}
    FFilename(const wchar_t* text) : FString(text) {}
    FString GetPath() const {
        auto slash = value.find_last_of(L"/\\");
        return slash == std::wstring::npos ? FString() : FString(value.substr(0, slash));
    }
    bool FileExists() const { return true; }
};
struct Config {
    void GetInt(const wchar_t*, const wchar_t*, INT& count, const wchar_t*) { count = state.font_count; }
    void GetString(const wchar_t*, const wchar_t*, FString& text, const wchar_t*) { text = L"\\Fonts"; }
    void GetArray(const wchar_t*, const wchar_t* name, TArray<FString>& out, const wchar_t*) {
        bool names = !wcscmp(name, L"FontName");
        for (int i = 0; i < (names ? state.names : state.files); ++i)
            out.AddItem(names ? L"Demo (TrueType)" : L"demo.ttf");
    }
} config;
Config* GConfig = &config;
const wchar_t* GUIIni = L"UI.ini";
TArray<FString> g_ArrayTempFont;
void debugf(const wchar_t*) {}
DWORD GetCurrentDirectory(DWORD capacity, wchar_t* output) {
    if (!state.directory_ok) return 0;
    if (state.directory.size() >= capacity) return (DWORD)state.directory.size() + 1;
    memcpy(output, state.directory.c_str(), (state.directory.size() + 1) * sizeof(wchar_t));
    return (DWORD)state.directory.size();
}
LONG RegOpenKeyEx(HKEY, const wchar_t*, DWORD, DWORD, HKEY* output) {
    if (!state.open_ok) return ERROR_ACCESS_DENIED;
    *output = &state; return ERROR_SUCCESS;
}
LONG RegQueryInfoKey(HKEY key, wchar_t*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*,
                     DWORD* values, DWORD*, DWORD*, DWORD*, void*) {
    assert(key == &state);
    if (!state.query_ok) return ERROR_ACCESS_DENIED;
    *values = (DWORD)state.entries.size() + 2; // Registry may shrink after this snapshot.
    return ERROR_SUCCESS;
}
LONG RegEnumValue(HKEY key, DWORD index, wchar_t* name, DWORD* name_count,
                  DWORD*, DWORD* type, LPBYTE data, DWORD* data_bytes) {
    assert(key == &state && name_count != data_bytes);
    assert(*name_count == 16383 && *data_bytes == 16383 * sizeof(wchar_t));
    ++state.enum_calls;
    if (index >= state.entries.size()) return ERROR_NO_MORE_ITEMS;
    const auto& entry = state.entries[index];
    if (entry.result != ERROR_SUCCESS) return entry.result;
    if (entry.name.size() > *name_count || entry.data.size() * sizeof(wchar_t) > *data_bytes)
        return ERROR_MORE_DATA;
    memcpy(name, entry.name.data(), entry.name.size() * sizeof(wchar_t));
    memcpy(data, entry.data.data(), entry.data.size() * sizeof(wchar_t));
    *name_count = (DWORD)entry.name.size();
    *data_bytes = (DWORD)entry.data.size() * sizeof(wchar_t);
    if (entry.odd_bytes) ++*data_bytes;
    if (entry.oversized_result) *data_bytes = 0xffffffff;
    *type = entry.type;
    return ERROR_SUCCESS;
}
LONG RegCloseKey(HKEY key) { assert(key == &state); ++state.closes; return ERROR_SUCCESS; }
INT AddFontResource(const wchar_t* path) { state.added.push_back(path); return 1; }
void SendNotifyMessage(INT, INT, INT, INT) { ++state.notifications; }
''']
    text = source(game / "MMOFacade.cpp")
    text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
    body = function(text, "void RegisterMMOFontFile()")
    body = re.sub(r"::(Reg\w+|AddFontResource|SendNotifyMessage)", r"\1", body)
    parts.append(body)
    parts.append(r'''
void Run() {
    auto call = [] {
        RegisterMMOFontFile();
        assert(state.closes == (state.open_ok ? 1 : 0));
        assert(state.notifications == (state.added.empty() ? 0 : 1));
    };
    state = State(); state.open_ok = false; call();
    assert(!state.enum_calls && state.added.empty());
    state = State(); state.query_ok = false; call();
    assert(!state.enum_calls && state.added.size() == 1);
    for (DWORD type : {DWORD(REG_SZ), DWORD(REG_EXPAND_SZ)}) {
        state = State(); Entry entry; entry.type = type;
        state.entries.push_back(entry); call();
        assert(state.added.empty() && state.enum_calls == 2);
    }
    for (int invalid = 0; invalid < 5; ++invalid) {
        state = State(); Entry entry;
        if (invalid == 0) entry.type = REG_BINARY;
        if (invalid == 1) entry.odd_bytes = true;
        if (invalid == 2) entry.oversized_result = true;
        if (invalid == 3) entry.result = ERROR_MORE_DATA;
        if (invalid == 4) entry.result = ERROR_ACCESS_DENIED;
        state.entries.push_back(entry); call();
        assert(state.added.size() == 1);
        assert(state.added[0] == L"C:\\Game\\Fonts\\demo.ttf");
        assert(g_ArrayTempFont.Num() == 1);
    }
    state = State(); Entry oversized; oversized.data.assign(16384, L'x');
    state.entries = {oversized, Entry()}; call(); assert(state.added.empty());
    state = State(); Entry limit; limit.data.assign(16383, L'x');
    state.entries = {limit}; call(); assert(state.added.empty());
    for (int invalid = 0; invalid < 5; ++invalid) {
        state = State();
        if (invalid == 0) state.names = 0;
        if (invalid == 1) state.files = 0;
        if (invalid == 2) state.font_count = 2147483647;
        if (invalid == 3) state.directory_ok = false;
        if (invalid == 4) state.directory.assign(500, L'x');
        call(); assert(state.added.empty());
    }
    puts("Font registry lengths, unterminated data and failed-query regressions passed");
}
}
#undef _WINDOWS_
#undef TEXT
#define TEXT(s) s
''')
    return "\n".join(parts)
