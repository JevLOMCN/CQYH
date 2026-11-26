"""Real web-manager conversions and mocked browser registry state (both copies)."""
import re


def fixtures(client, source, function):
    game = client / "client/UE3/Development/Src/MMOGame"

    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#(?:include|pragma)[^\n]*", "", text)

    parts = [r'''
#include <climits>
#include <cstring>
namespace WebManagerTests {
using INT = int;
using BYTE = unsigned char;
enum EEventParm { EC_EventParm };
enum { NAME_None = 0 };
struct FName { int value; FName() {} FName(int v) : value(v) {} };
struct UObject;
template<class T> struct SingletonMeyers {};
template<class T> using TArray = std::vector<T>;
struct FString {
    std::wstring value;
    FString(const wchar_t* text = L"") : value(text) {}
    void Empty() { value.clear(); }
    const wchar_t* operator*() const { return value.c_str(); }
    FString operator+(const wchar_t* suffix) const { return (value + suffix).c_str(); }
};
FString appGameConfigDir() { return L"C:\\Game\\Config\\"; }
struct State {
    bool config_ok = true, open_ok = true;
    LONG query_result = ERROR_SUCCESS;
    DWORD type = REG_BINARY;
    std::vector<BYTE> bytes;
    int reads = 0, writes = 0, closes = 0, launches = 0;
    WINDOWPLACEMENT written = {};
    int conversion_calls = 0, fail_conversion_at = 0;
} state;
RECT GPrimaryMonitorWorkRect = {0, 0, 1920, 1080};
struct Config {
    bool GetString(const wchar_t*, const wchar_t*, FString& text, const wchar_t*) {
        text = L"https://example.invalid/quit";
        return state.config_ok;
    }
} config;
Config* GConfig = &config;
LONG MockRegOpen(HKEY, const wchar_t* key, DWORD, DWORD, HKEY* handle) {
    assert(!wcscmp(key, L"Software\\Microsoft\\Internet Explorer\\Main"));
    if (!state.open_ok) return ERROR_ACCESS_DENIED;
    *handle = (HKEY)&state; return ERROR_SUCCESS;
}
LONG MockRegQuery(HKEY handle, const wchar_t*, DWORD*, DWORD* type, LPBYTE data, DWORD* size) {
    assert(handle == (HKEY)&state && data && *size == sizeof(WINDOWPLACEMENT));
    ++state.reads;
    if (state.query_result != ERROR_SUCCESS) return state.query_result;
    *type = state.type;
    if (state.bytes.size() > *size) { *size = (DWORD)state.bytes.size(); return ERROR_MORE_DATA; }
    if (!state.bytes.empty()) memcpy(data, state.bytes.data(), state.bytes.size());
    *size = (DWORD)state.bytes.size(); return ERROR_SUCCESS;
}
LONG MockRegSet(HKEY handle, const wchar_t*, DWORD, DWORD type, const BYTE* bytes, DWORD size) {
    assert(handle == (HKEY)&state && type == REG_BINARY && size == sizeof(WINDOWPLACEMENT));
    ++state.writes; memcpy(&state.written, bytes, size); return ERROR_SUCCESS;
}
LONG MockRegClose(HKEY handle) { assert(handle == (HKEY)&state); ++state.closes; return ERROR_SUCCESS; }
int ConvertMulti(UINT codepage, DWORD flags, LPCCH input, int count, LPWSTR output, int capacity) {
    if (++state.conversion_calls == state.fail_conversion_at) return 0;
    return ::MultiByteToWideChar(codepage, flags, input, count, output, capacity);
}
int ConvertWide(UINT codepage, DWORD flags, LPCWCH input, int count, LPSTR output, int capacity,
                LPCCH replacement, LPBOOL used) {
    if (++state.conversion_calls == state.fail_conversion_at) return 0;
    return ::WideCharToMultiByte(codepage, flags, input, count, output, capacity, replacement, used);
}
''']
    delegate = source(game.parent / "Core/Inc/UnObjBas.h")
    start = delegate.index("struct FScriptDelegate\n")
    end = delegate.index("\n\tinline UBOOL IsCallable", start)
    parts.append(clean(delegate[start:end]) + "\n};")
    header = clean(source(game / "WebHtmlManager.h")).replace("private:", "public:")
    for index, path in enumerate([game / "WebHtmlManager.cpp", *game.glob("WebHtmlManager - *.cpp")]):
        parts.append("namespace Copy" + str(index) + " {")
        # The two source copies have the same class include guard in isolation.
        parts.append(re.sub(r"(?m)^\s*#(?:ifndef|define|endif)[^\n]*", "", header))
        text = clean(source(path))
        for signature in ["WebHtmlManager::WebHtmlManager(void)", "WebHtmlManager::~WebHtmlManager(void)",
                          "void WebHtmlManager::OpenGameQuitWeb()", "BOOL WebHtmlManager::UrlEncode(",
                          "BOOL WebHtmlManager::UrlDecode("]:
            body = function(text, signature)
            for api, mock in [("RegOpenKeyEx", "MockRegOpen"), ("RegQueryValueEx", "MockRegQuery"),
                              ("RegSetValueEx", "MockRegSet"), ("RegCloseKey", "MockRegClose"),
                              ("MultiByteToWideChar", "ConvertMulti"), ("WideCharToMultiByte", "ConvertWide")]:
                body = re.sub(r"(?:::)?\b" + api + r"(?=\s*\()", mock, body)
            parts.append(body)
        parts.append(r'''
void WebHtmlManager::OpenUrlByIE(FString) { ++state.launches; }
''')
        parts.append("}")
    parts.append(r'''
template<class Manager> void Check() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(Manager) unsigned char bytes[sizeof(Manager)];
        memset(bytes, pattern, sizeof(bytes));
        Manager* manager = new(bytes) Manager;
        assert(!manager->pFunInitWebWindowDll && !manager->pFunInitWebWindowTitle);
        assert(!manager->pFunRedrawWindow && !manager->pFunOpenUrl && !manager->pFunShutDownDll);
        assert(!manager->_webDll && !manager->_webHwnd && !manager->_GameServiceEncryptDll);
        assert(!manager->pFunSDRS_Encrypt && !manager->pFunSDRS_Decrypt && !manager->pFunSDRS_Release);
        assert(!manager->_OpenGmBallDelegate.Object && !manager->_OpenGmBallDelegate.FunctionName.value);
        manager->~Manager();
    }
    Manager manager;
    for (int size : {0, 1, 2, 4, int(sizeof(WINDOWPLACEMENT)) - 1,
                     int(sizeof(WINDOWPLACEMENT)), int(sizeof(WINDOWPLACEMENT)) + 1, 1000}) {
        state = State(); state.bytes.assign(size, 0x5a);
        manager.OpenGameQuitWeb();
        assert(state.launches == 1 && state.closes == 1 && state.reads == 1);
        assert(state.writes == (size == sizeof(WINDOWPLACEMENT)));
        if (state.writes) {
            const auto& placement = state.written;
            assert(placement.length == sizeof(WINDOWPLACEMENT) && placement.showCmd == SW_SHOWNORMAL);
            assert(placement.flags == 0x5a5a5a5a);
            assert(placement.rcNormalPosition.left == 448 && placement.rcNormalPosition.top == 156);
            assert(placement.rcNormalPosition.right == 1472 && placement.rcNormalPosition.bottom == 924);
        }
    }
    for (int invalid = 0; invalid < 5; ++invalid) {
        state = State(); state.bytes.resize(sizeof(WINDOWPLACEMENT));
        if (invalid == 0) state.config_ok = false;
        if (invalid == 1) state.open_ok = false;
        if (invalid == 2) state.query_result = ERROR_ACCESS_DENIED;
        if (invalid == 3) state.query_result = ERROR_MORE_DATA;
        if (invalid == 4) state.type = REG_SZ;
        manager.OpenGameQuitWeb();
        assert(!state.writes && state.closes == (invalid > 1 ? 1 : 0));
        assert(state.launches == (invalid == 0 ? 0 : 1));
    }
    const std::pair<std::string, std::string> cases[] = {
        {"", ""}, {"a Z0-.~", "a+Z0-.~"}, {"!+%_", "%21%2b%25%5f"},
        {std::string(1000, '&'), std::string()}
    };
    for (const auto& test : cases) {
        std::string encoded = test.second;
        if (test.first.size() == 1000) for (int i = 0; i < 1000; ++i) encoded += "%26";
        for (int capacity = 0; capacity <= int(encoded.size()) + 2; ++capacity) {
            state = State(); std::vector<char> buffer(capacity + 2, '#');
            bool ok = Manager::UrlEncode(test.first.c_str(), buffer.data() + 1, capacity, FALSE);
            assert(ok == (capacity > encoded.size()));
            assert(buffer.front() == '#' && buffer.back() == '#');
            if (ok) assert(std::string(buffer.data() + 1) == encoded);
            else if (capacity) assert(buffer[1] == 0);
        }
        for (int capacity = 0; capacity <= int(test.first.size()) + 2; ++capacity) {
            state = State(); std::vector<char> buffer(capacity + 2, '#');
            bool ok = Manager::UrlDecode(encoded.c_str(), buffer.data() + 1, capacity);
            assert(ok == (capacity > test.first.size()));
            assert(buffer.front() == '#' && buffer.back() == '#');
            if (ok) assert(std::string(buffer.data() + 1) == test.first);
            else if (capacity) assert(buffer[1] == 0);
        }
    }
    for (const char* bad : {"%", "%a", "x%", "%g0", "%0g", "%/0", "%ff", "%c0%af"}) {
        state = State(); const size_t size = strlen(bad) + 1;
        auto exact = std::make_unique<char[]>(size); memcpy(exact.get(), bad, size);
        char output[64]; memset(output, '#', sizeof(output));
        assert(!Manager::UrlDecode(exact.get(), output, sizeof(output)) && !output[0]);
    }
    for (int fail = 1; fail <= 4; ++fail) {
        char output[64]; state = State(); state.fail_conversion_at = fail;
        assert(!Manager::UrlEncode("hello &", output, sizeof(output), TRUE) && !output[0]);
        state = State(); state.fail_conversion_at = fail;
        assert(!Manager::UrlDecode("hello+%26", output, sizeof(output)) && !output[0]);
    }
    char alias[64] = "Hello world!";
    state = State(); assert(Manager::UrlEncode(alias, alias, sizeof(alias), TRUE));
    assert(!strcmp(alias, "Hello+world%21"));
    assert(Manager::UrlDecode(alias, alias, sizeof(alias)) && !strcmp(alias, "Hello world!"));
    char output[64]; state = State();
    assert(Manager::UrlEncode("?", output, sizeof(output), TRUE) && !strcmp(output, "%3F"));
    for (const wchar_t* wide : {L"\u00e9\u00df", L"\u4e2d\u6587 \u03bb"}) {
        int size = ::WideCharToMultiByte(CP_ACP, 0, wide, -1, nullptr, 0, nullptr, nullptr);
        assert(size > 0);
        std::vector<char> ansi(size);
        assert(::WideCharToMultiByte(CP_ACP, 0, wide, -1, ansi.data(), size, nullptr, nullptr) == size);
        char encoded[256], decoded[256];
        assert(Manager::UrlEncode(ansi.data(), encoded, sizeof(encoded), TRUE));
        assert(Manager::UrlDecode(encoded, decoded, sizeof(decoded)) && !strcmp(decoded, ansi.data()));
        std::vector<char> exact(size);
        assert(Manager::UrlDecode(encoded, exact.data(), size) && !strcmp(exact.data(), ansi.data()));
        assert(!Manager::UrlDecode(encoded, exact.data(), size - 1) && !exact[0]);
    }
    assert(!Manager::UrlDecode(nullptr, output, sizeof(output)));
    assert(!Manager::UrlEncode("x", nullptr, 1, FALSE));
    assert(!Manager::UrlDecode("x", output, -1));
}
void Run() {
    Check<Copy0::WebHtmlManager>(); Check<Copy1::WebHtmlManager>();
    puts("Web manager initialization, registry size and URL conversion regressions passed (two copies)");
}
}
''')
    return "\n".join(parts)
