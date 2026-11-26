"""Production WinInet response logic with deterministic asynchronous reads."""
import re


def fixtures(client, source, function):
    root = client / "client/UE3/Development/Src/WinDrv"

    def clean(text):
        return re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                      lambda match: "" if match[0].startswith(("//", "/*")) else match[0], text)

    parts = [r'''
#include <wininet.h>
#include <algorithm>
#include <cstdarg>
#ifndef MAXINT
#define MAXINT 0x7fffffff
#endif
namespace WinInetTests {
using INT = int;
using UBOOL = int;
struct FStringAllocator {};
template<class T, class Allocator = FStringAllocator> struct TArray {
    std::vector<T> values;
    INT virtual_count = -1, largest = 0;
    INT Num() const { return virtual_count < 0 ? (INT)values.size() : virtual_count; }
    void SetNum(INT count) {
        assert(count >= 0);
        if (virtual_count >= 0) { virtual_count = count; return; }
        assert(count < 4 * 1024 * 1024); // An untrusted length must never allocate gigabytes.
        largest = std::max(largest, count); values.resize(count);
    }
    void AddZeroed(INT count) { INT old = Num(); SetNum(old + count); std::fill(values.begin() + old, values.end(), T()); }
    void AddItem(const T& value) { assert(virtual_count < 0 && Num() < MAXINT); values.push_back(value); }
    void Empty() { values.clear(); virtual_count = -1; }
    T* GetTypedData() { return values.data(); }
    const T* GetTypedData() const { return values.data(); }
    T* GetData() { return values.data(); }
    const T* GetData() const { return values.data(); }
    T& operator()(INT i) { assert(virtual_count < 0 && i >= 0 && i < Num()); return values.at(i); }
};
struct FString {
    std::wstring text;
    FString(const wchar_t* value = L"") : text(value) {}
    FString(std::wstring value) : text(value) {}
    FString(INT count, const wchar_t* value) : text(value, count) { assert(count >= 0); }
    INT Len() const { return (INT)text.size(); }
    const wchar_t* operator*() const { return text.c_str(); }
    FString& operator+=(const FString& value) { text += value.text; return *this; }
    static FString Printf(const wchar_t* format, ...) {
        wchar_t buffer[2048]; va_list args; va_start(args, format);
        _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, format, args); va_end(args);
        return std::wstring(buffer);
    }
};
struct Utf8Text {
    std::wstring text;
    explicit Utf8Text(const BYTE* bytes) {
        int count = ::MultiByteToWideChar(CP_UTF8, 0, (const char*)bytes, -1, nullptr, 0);
        assert(count > 0); text.resize(count);
        assert(::MultiByteToWideChar(CP_UTF8, 0, (const char*)bytes, -1, &text[0], count) == count);
    }
};
template<class T> T Min(T a, T b) { return a < b ? a : b; }
template<class T> T Max(T a, T b) { return a > b ? a : b; }
template<class T> void Exchange(T& a, T& b) { std::swap(a, b); }
void appMemcpy(void* output, const void* input, size_t bytes) { memcpy(output, input, bytes); }
INT appAtoi(const wchar_t* value) { return _wtoi(value); }
const wchar_t* appGetGameName() { return L"Test"; }
INT GEngineVersion = 1;
struct State {
    DWORD error = 0;
    std::vector<BYTE> body;
    size_t position = 0;
    DWORD chunk = 317, advertised = 0;
    int reads = 0, pending_at = 0, fail_at = 0, invalid_count_at = 0;
    bool alternating_pending = false;
    void* pending_buffer = nullptr;
    DWORD pending_capacity = 0;
    DWORD* pending_count = nullptr;
    std::wstring header, expected_name, extended = L"Extended details";
    DWORD header_error = 0, header_required = 0, extended_required = 0;
    bool odd_header = false, oversized_header = false, omit_terminator = false;
    bool status_fail = false, short_status = false, extended_fail = false, extended_read_fail = false;
    bool connected_fail = false;
    int header_calls = 0, opens = 0;
} state;
DWORD MockLastError() { return state.error; }
BOOL MockRead(HINTERNET, void* output, DWORD capacity, DWORD* count) {
    ++state.reads; assert(capacity > 0 && *count == 0 && !state.pending_count);
    if (state.reads == state.fail_at) { state.error = ERROR_ACCESS_DENIED; *count = 0xffffffff; return FALSE; }
    if (state.reads == state.invalid_count_at) { *count = capacity + 1; return TRUE; }
    if (state.reads == state.pending_at || (state.alternating_pending && state.reads % 2)) {
        state.pending_buffer = output; state.pending_capacity = capacity; state.pending_count = count;
        state.error = ERROR_IO_PENDING; return FALSE;
    }
    DWORD bytes = (DWORD)std::min(state.body.size() - state.position, (size_t)std::min(capacity, state.chunk));
    if (bytes) memcpy(output, state.body.data() + state.position, bytes);
    state.position += bytes; *count = bytes; return TRUE;
}
void CompletePending() {
    assert(state.pending_count);
    DWORD bytes = (DWORD)std::min(state.body.size() - state.position, (size_t)std::min(state.pending_capacity, state.chunk));
    if (bytes) memcpy(state.pending_buffer, state.body.data() + state.position, bytes);
    *state.pending_count = bytes; state.position += bytes; state.pending_count = nullptr;
}
BOOL MockQuery(HINTERNET, DWORD level, void* output, DWORD* bytes, DWORD*) {
    if (level == (HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER)) {
        if (state.status_fail) { state.error = ERROR_ACCESS_DENIED; return FALSE; }
        assert(*bytes == sizeof(INT)); *(INT*)output = 200;
        if (state.short_status) --*bytes;
        return TRUE;
    }
    ++state.header_calls;
    if (level == HTTP_QUERY_CUSTOM && !state.expected_name.empty())
        assert(std::wstring((wchar_t*)output) == state.expected_name);
    if (state.header_error) { state.error = state.header_error; return FALSE; }
    std::wstring text = level == HTTP_QUERY_CONTENT_LENGTH ? std::to_wstring(state.advertised) : state.header;
    DWORD required = state.header_required ? state.header_required : (DWORD)(text.size() + 1) * sizeof(wchar_t);
    if (*bytes < required) { *bytes = required; state.error = ERROR_INSUFFICIENT_BUFFER; return FALSE; }
    const DWORD capacity = *bytes;
    assert((text.size() + 1) * sizeof(wchar_t) <= capacity);
    memcpy(output, text.data(), text.size() * sizeof(wchar_t));
    if (!state.omit_terminator) ((wchar_t*)output)[text.size()] = 0;
    *bytes = (DWORD)text.size() * sizeof(wchar_t);
    if (state.odd_header) ++*bytes;
    if (state.oversized_header) *bytes = capacity + sizeof(wchar_t);
    return TRUE;
}
BOOL MockExtended(DWORD* error, wchar_t* output, DWORD* chars) {
    *error = ERROR_INTERNET_EXTENDED_ERROR;
    if (!output) {
        if (state.extended_fail) { state.error = ERROR_ACCESS_DENIED; return FALSE; }
        *chars = state.extended_required ? state.extended_required : (DWORD)state.extended.size();
        state.error = ERROR_INSUFFICIENT_BUFFER; return FALSE;
    }
    if (state.extended_read_fail || *chars <= state.extended.size()) { state.error = ERROR_INSUFFICIENT_BUFFER; return FALSE; }
    memcpy(output, state.extended.c_str(), (state.extended.size() + 1) * sizeof(wchar_t));
    *chars = (DWORD)state.extended.size(); return TRUE;
}
DWORD MockFormat(DWORD, const void*, DWORD, DWORD, wchar_t* output, DWORD capacity, va_list*) {
    const wchar_t* text = L"Failure"; assert(capacity > wcslen(text));
    wcscpy_s(output, capacity, text); return (DWORD)wcslen(text);
}
BOOL MockConnected(DWORD* flags, DWORD) { assert(*flags == 0); if (state.connected_fail) return FALSE; *flags = INTERNET_CONNECTION_LAN; return TRUE; }
DWORD MockAttempt(DWORD) { return ERROR_SUCCESS; }
BOOL MockCheck(const wchar_t*, DWORD, DWORD) { return TRUE; }
HINTERNET MockOpen(const wchar_t*, DWORD, const wchar_t*, const wchar_t*, DWORD) { ++state.opens; return (HINTERNET)&state; }
INTERNET_STATUS_CALLBACK MockCallback(HINTERNET, INTERNET_STATUS_CALLBACK) { return nullptr; }
BOOL MockSetOption(HINTERNET, DWORD, void*, DWORD) { return TRUE; }
BOOL MockClose(HINTERNET) { return TRUE; }
static HINTERNET GRootInternetHandle = nullptr;
class FHttpRequestWinInet;
class FURLWinInet {};
void CALLBACK InternetStatusCallbackWinInet(HINTERNET, DWORD_PTR, DWORD, LPVOID, DWORD);
#define debugf(...) ((void)0)
#define UTF8_TO_TCHAR(s) Utf8Text(s).text.c_str()
#undef HttpQueryInfo
#define HttpQueryInfo MockQuery
#define InternetReadFile MockRead
#define GetLastError MockLastError
#undef InternetGetLastResponseInfo
#define InternetGetLastResponseInfo MockExtended
#define FormatMessageW MockFormat
#define InternetGetConnectedState MockConnected
#define InternetAttemptConnect MockAttempt
#undef InternetCheckConnection
#define InternetCheckConnection MockCheck
#undef InternetOpen
#define InternetOpen MockOpen
#undef InternetSetStatusCallback
#define InternetSetStatusCallback MockCallback
#undef InternetSetOption
#define InternetSetOption MockSetOption
#define InternetCloseHandle MockClose
''']
    header = clean(source(root / "Inc/HttpImplementationWinInet.h"))
    declaration = re.search(r"class FHttpResponseWinInet\s*\{", header)
    parts.append(function(header[declaration.start():], "class FHttpResponseWinInet")
                 .replace("private:", "public:") + ";")
    cpp = clean(source(root / "Src/HttpImplementationWinInet.cpp"))
    signatures = ["FString InternetTranslateError(", "bool InitWinInet()",
                  "void CALLBACK InternetStatusCallbackWinInet(\n", "FHttpResponseWinInet::FHttpResponseWinInet(",
                  "FHttpResponseWinInet::~FHttpResponseWinInet()", "bool FHttpResponseWinInet::IsReady() const",
                  "bool FHttpResponseWinInet::Succeeded() const", "const TArray<BYTE>* FHttpResponseWinInet::GetPayload() const",
                  "const FString FHttpResponseWinInet::GetPayloadAsString() const", "void FHttpResponseWinInet::FailResponse()",
                  "void FHttpResponseWinInet::ProcessResponse(", "FString FHttpResponseWinInet::PrivateGetHeaderString(",
                  "TArray<FString> FHttpResponseWinInet::GetAllHeaders() const", "INT FHttpResponseWinInet::GetResponseCode() const"]
    parts.extend(function(cpp, signature) for signature in signatures)
    parts.append(r'''
void Run() {
    INTERNET_ASYNC_RESULT good = {}; good.dwResult = 1;
    for (int async : {0, 1, 2}) for (size_t length : {0u, 1u, 16383u, 16384u, 16385u, 70000u}) {
        state = State(); state.advertised = 0x7fffffff;
        state.chunk = async == 2 ? 17000 : 317;
        if (async == 1) state.pending_at = 2;
        if (async == 2) state.alternating_pending = true;
        state.body.resize(length);
        for (size_t i = 0; i < length; ++i) state.body[i] = 'a' + i % 26;
        TArray<BYTE> request; request.AddItem(1);
        FHttpResponseWinInet response(nullptr, nullptr, FURLWinInet(), request);
        assert(request.Num() == 0 && !response.GetPayload());
        response.ProcessResponse(&good);
        int completions = 0;
        while (!response.IsReady()) {
            assert(++completions < 1000);
            assert(state.pending_buffer == &response.ResponsePayload(response.TotalBytesRead));
            CompletePending(); response.ProcessResponse(&good);
        }
        assert(response.Succeeded() && response.GetPayload()->values == state.body);
        assert(response.GetPayloadAsString().Len() == length);
        assert(response.ResponsePayload.largest <= int(length) + 16384 && state.header_calls == 0);
        response.ProcessResponse(&good); assert(response.GetPayload()->values == state.body);
    }
    for (int failure = 0; failure < 6; ++failure) {
        state = State(); state.body.assign(1000, 'x');
        if (failure == 0) state.fail_at = 1;
        if (failure == 1) state.fail_at = 2;
        if (failure == 2) state.invalid_count_at = 2;
        if (failure == 3) state.pending_at = 2;
        TArray<BYTE> request;
        FHttpResponseWinInet response(nullptr, nullptr, FURLWinInet(), request);
        if (failure == 4) response.ProcessResponse(nullptr);
        else if (failure == 5) { INTERNET_ASYNC_RESULT bad = {}; response.ProcessResponse(&bad); }
        else response.ProcessResponse(&good);
        if (failure == 3) { CompletePending(); INTERNET_ASYNC_RESULT bad = {}; response.ProcessResponse(&bad); }
        assert(response.IsReady() && !response.Succeeded());
        assert(response.GetPayload()->Num() == (failure >= 1 && failure <= 3 ? 317 : 0));
        for (BYTE byte : response.GetPayload()->values) assert(byte == 'x');
    }
    state = State(); TArray<BYTE> request;
    FHttpResponseWinInet response(nullptr, nullptr, FURLWinInet(), request);
    response.ResponsePayload.virtual_count = MAXINT;
    response.TotalBytesRead = MAXINT - 1; response.AsyncBytesRead = 1;
    response.ProcessResponse(&good);
    assert(response.IsReady() && !response.Succeeded() && response.GetPayloadAsString().Len() == 0 && !state.reads);
    response.bIsReady = false; response.TotalBytesRead = -1;
    response.ProcessResponse(&good); assert(response.GetPayload()->Num() == 0);
    for (DWORD size = 0; size < sizeof(INTERNET_ASYNC_RESULT); ++size) {
        response.bIsReady = false;
        std::vector<BYTE> partial(size, 0);
        InternetStatusCallbackWinInet(nullptr, (DWORD_PTR)&response, INTERNET_STATUS_REQUEST_COMPLETE, partial.data(), size);
        assert(response.IsReady() && !response.Succeeded());
    }
    InternetStatusCallbackWinInet(nullptr, 0, INTERNET_STATUS_REQUEST_COMPLETE, nullptr, 0);
    for (const std::wstring text : {std::wstring(), std::wstring(L"value"), std::wstring(2000, L'V')}) {
        state = State(); state.header = text; state.omit_terminator = true;
        assert(response.PrivateGetHeaderString(HTTP_QUERY_CONTENT_TYPE, FString()).text == text);
    }
    state = State(); state.header.assign(3000, L'V'); state.expected_name.assign(500, L'N');
    assert(response.PrivateGetHeaderString(HTTP_QUERY_CUSTOM, FString(state.expected_name)).text == state.header);
    assert(state.header_calls == 2);
    for (int invalid = 0; invalid < 5; ++invalid) {
        state = State(); state.header = L"value";
        if (invalid == 0) state.header_error = ERROR_ACCESS_DENIED;
        if (invalid == 1) state.header_error = ERROR_HTTP_HEADER_NOT_FOUND;
        if (invalid == 2) state.header_required = 0xffffffff;
        if (invalid == 3) state.odd_header = true;
        if (invalid == 4) state.oversized_header = true;
        assert(response.PrivateGetHeaderString(HTTP_QUERY_CONTENT_TYPE, FString()).Len() == 0);
    }
    state = State(); state.header = L"HTTP/1.1 200 OK\r\nA: B\r\n\r\n";
    auto headers = response.GetAllHeaders();
    assert(headers.Num() == 3 && headers(0).text == L"HTTP/1.1 200 OK" && headers(1).text == L"A: B" && headers(2).Len() == 0);
    state.header = L"X: Y"; assert(response.GetAllHeaders()(0).text == L"X: Y");
    state = State(); assert(response.GetResponseCode() == 200);
    state.status_fail = true; assert(response.GetResponseCode() == 0);
    state.status_fail = false; state.short_status = true; assert(response.GetResponseCode() == 0);
    for (int invalid = 0; invalid < 4; ++invalid) {
        state = State();
        if (invalid == 1) state.extended_fail = true;
        if (invalid == 2) state.extended_read_fail = true;
        if (invalid == 3) state.extended_required = 0xffffffff;
        auto error = InternetTranslateError(ERROR_INTERNET_EXTENDED_ERROR);
        assert((error.text.find(L"Extended details") != std::wstring::npos) == (invalid == 0));
    }
    state = State(); state.connected_fail = true; assert(InitWinInet() && state.opens == 1);
    puts("WinInet checked growth, async counts, header lengths and initialized status regressions passed");
}
}
#undef debugf
#undef UTF8_TO_TCHAR
#undef HttpQueryInfo
#undef InternetReadFile
#undef GetLastError
#undef InternetGetLastResponseInfo
#undef FormatMessageW
#undef InternetGetConnectedState
#undef InternetAttemptConnect
#undef InternetCheckConnection
#undef InternetOpen
#undef InternetSetStatusCallback
#undef InternetSetOption
#undef InternetCloseHandle
''')
    return "\n".join(parts)
