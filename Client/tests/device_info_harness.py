"""Real hardware helper with mocked WMI and actual Windows VARIANT operations."""
import re


def fixtures(client, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    parts = [r'''
#include <objbase.h>
#include <oleauto.h>
#include <cstdarg>
#pragma comment(lib, "oleaut32.lib")
''', source(client / "Common/utility/fixed_string.h"), r'''
namespace DeviceTests {
using UBOOL = int;
using INT = int;
using UINT8 = unsigned char;
using UINT16 = unsigned short;
using UINT32 = unsigned int;
using UINT64 = unsigned long long;
enum { c2g_client_hardware_info = 123, BUILD_NUMBER = 456,
       WBEM_FLAG_FORWARD_ONLY = 1, WBEM_FLAG_RETURN_IMMEDIATELY = 2, WBEM_INFINITE = -1,
       CLSID_WbemLocator = 1, IID_IWbemLocator = 2 };
struct MESSAGE_HEADER { UINT16 wMsg; };
struct Property {
    VARTYPE type = VT_EMPTY;
    ULONG number = 0;
    std::wstring text;
    bool null_string = false;
    static Property String(std::wstring value) { Property p; p.type = VT_BSTR; p.text = value; return p; }
    static Property Number(ULONG value) { Property p; p.type = VT_I4; p.number = value; return p; }
    static Property Boolean(bool value) { Property p; p.type = VT_BOOL; p.number = value; return p; }
};
using Row = std::map<std::wstring, Property>;
struct State {
    int fail_stage = 0;
    bool partial_failure = false, null_locator = false, null_service = false;
    bool query_fail = false, null_enum = false, next_fail = false, null_row = false;
    bool fail_get = false, memory_ok = true, os_ok = true;
    int live_objects = 0, created = 0, released = 0, com_inits = 0, com_uninits = 0;
    int active_strings = 0, initialized = 0, cleared = 0;
    std::map<std::wstring, std::vector<Row>> rows;
    std::set<VARIANT*> variants;
} state;
void TrackedVariantInit(VARIANT* value) { ::VariantInit(value); ++state.initialized; state.variants.insert(value); }
HRESULT TrackedVariantClear(VARIANT* value) {
    assert(state.variants.count(value));
    if (value->vt == VT_BSTR && value->bstrVal) --state.active_strings;
    ++state.cleared;
    return ::VariantClear(value);
}
HRESULT TrackedVariantChangeType(VARIANT* output, VARIANT* input, USHORT flags, VARTYPE type) {
    assert(output == input && state.variants.count(output));
    bool string_before = input->vt == VT_BSTR && input->bstrVal;
    HRESULT result = ::VariantChangeType(output, input, flags, type);
    if (string_before && output->vt != VT_BSTR) --state.active_strings;
    return result;
}
struct bstr_t {
    std::wstring text;
    bstr_t(const char* value) { while (*value) text += (unsigned char)*value++; }
    bstr_t(const wchar_t* value) : text(value) {}
};
using _bstr_t = bstr_t;
struct RefObject {
    RefObject() { ++state.live_objects; ++state.created; }
    virtual ~RefObject() { --state.live_objects; ++state.released; }
    ULONG Release() { delete this; return 0; }
};
struct IWbemClassObject : RefObject {
    Row row;
    explicit IWbemClassObject(Row value) : row(value) {}
    HRESULT Get(const wchar_t* name, LONG, VARIANT* value, void*, void*) {
        assert(state.variants.count(value) && value->vt == VT_EMPTY);
        auto it = row.find(name);
        if (state.fail_get || it == row.end()) return E_FAIL;
        const auto& property = it->second;
        value->vt = property.type;
        if (property.type == VT_BSTR) {
            value->bstrVal = property.null_string ? nullptr : ::SysAllocStringLen(property.text.data(), (UINT)property.text.size());
            if (value->bstrVal) ++state.active_strings;
        } else if (property.type == VT_BOOL) value->boolVal = property.number ? VARIANT_TRUE : VARIANT_FALSE;
        else value->ulVal = property.number;
        return S_OK;
    }
};
struct IEnumWbemClassObject : RefObject {
    std::vector<Row> rows;
    size_t index = 0;
    explicit IEnumWbemClassObject(std::vector<Row> values) : rows(values) {}
    HRESULT Next(LONG, ULONG, IWbemClassObject** output, ULONG* returned) {
        assert(*output == nullptr && *returned == 0);
        if (state.next_fail) {
            if (state.partial_failure) *output = new IWbemClassObject(Row());
            return E_FAIL;
        }
        if (index == rows.size()) return S_FALSE;
        *returned = 1;
        if (!state.null_row) *output = new IWbemClassObject(rows[index]);
        ++index; return S_OK;
    }
};
struct IWbemServices : RefObject {
    HRESULT ExecQuery(bstr_t, bstr_t query, LONG, void*, IEnumWbemClassObject** output) {
        assert(*output == nullptr);
        if (!state.null_enum && (!state.query_fail || state.partial_failure))
            *output = new IEnumWbemClassObject(state.rows[query.text]);
        return state.query_fail ? E_FAIL : S_OK;
    }
};
struct IWbemLocator : RefObject {
    HRESULT ConnectServer(bstr_t, void*, void*, LONG, void*, LONG, void*, IWbemServices** output) {
        if (!state.null_service && (state.fail_stage != 4 || state.partial_failure)) *output = new IWbemServices;
        return state.fail_stage == 4 ? E_FAIL : S_OK;
    }
};
HRESULT CoInitializeEx(void*, DWORD) {
    if (state.fail_stage == 1) return E_FAIL;
    ++state.com_inits; return S_OK;
}
void CoUninitialize() {
    assert(!state.live_objects);
    ++state.com_uninits;
}
HRESULT CoInitializeSecurity(void*, LONG, void*, void*, DWORD, DWORD, void*, DWORD, void*) {
    return state.fail_stage == 2 ? E_FAIL : S_OK;
}
HRESULT CoCreateInstance(int, void*, DWORD, int, void** output) {
    if (!state.null_locator && (state.fail_stage != 3 || state.partial_failure)) *output = new IWbemLocator;
    return state.fail_stage == 3 ? E_FAIL : S_OK;
}
HRESULT CoSetProxyBlanket(IWbemServices*, DWORD, DWORD, void*, DWORD, DWORD, void*, DWORD) {
    return state.fail_stage == 5 ? E_FAIL : S_OK;
}
struct FString {
    std::wstring text;
    FString(std::wstring value = L"") : text(value) {}
    const wchar_t* operator*() const { return text.c_str(); }
    static FString Printf(const wchar_t* format, ...) {
        wchar_t buffer[1024]; va_list args; va_start(args, format);
        _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, format, args); va_end(args);
        return std::wstring(buffer);
    }
};
FString GetPlayerRoleName() { return std::wstring(400, L'R'); }
INT GetPlayerRoleID() { return 42; }
FString appItoa(INT value) { return std::to_wstring(value); }
struct Utf8 {
    std::string bytes;
    explicit Utf8(const wchar_t* text) {
        int length = ::WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
        assert(length > 0); bytes.resize(length);
        assert(::WideCharToMultiByte(CP_UTF8, 0, text, -1, &bytes[0], length, nullptr, nullptr) == length);
    }
    operator const char*() const { return bytes.c_str(); }
};
struct Log { void Logf(const wchar_t*, ...) {} } log;
Log* GLog = &log;
UINT g_videoRam = 0;
BOOL MockMemoryStatus(MEMORYSTATUSEX* memory) {
    assert(memory->dwLength == sizeof(*memory) && !memory->ullTotalVirtual);
    if (!state.memory_ok) return FALSE;
    memory->ullTotalVirtual = 0x123456789ull; return TRUE;
}
BOOL MockVersion(LPOSVERSIONINFO version) {
    assert(version->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX));
    if (!state.os_ok) return FALSE;
    version->dwMajorVersion = 10; version->dwMinorVersion = 0; version->dwBuildNumber = 123;
    return TRUE;
}
#define VariantInit TrackedVariantInit
#define VariantClear TrackedVariantClear
#define VariantChangeType TrackedVariantChangeType
#define CoInitializeEx DeviceTests::CoInitializeEx
#define CoInitializeSecurity DeviceTests::CoInitializeSecurity
#define CoCreateInstance DeviceTests::CoCreateInstance
#define CoSetProxyBlanket DeviceTests::CoSetProxyBlanket
#define CoUninitialize DeviceTests::CoUninitialize
#define TCHAR_TO_UTF8(s) Utf8(s)
#undef GetVersionEx
#define GetVersionEx MockVersion
#define GlobalMemoryStatusEx MockMemoryStatus
#define warnf(...) ((void)0)
''']
    strings = clean(source(client / "Common/define/string_def.h"))
    for name in ["MAX_DEVICE_STRING_LEN", "MAX_NETWORK_ADAPTERINFO_STRING_LEN"]:
        parts.append(re.search(r"#define " + name + r"\s+[^\n]+", strings).group())
    wire = clean(source(client / "Common/message/c2g_message.h"))
    declaration = re.search(r"struct\s+C2G_CLIENT_HARDWARE_INFO\b", wire).group()
    parts.append("#pragma pack(push, 1)\n" + function(wire, declaration) + ";\n#pragma pack(pop)")
    helper = clean(source(client / "client/UE3/Development/Src/WinDrv/Inc/DeviceInfoHelper.h"))
    helper = function(helper, "class DeviceInfoHelper\n") + ";"
    parts.append(helper.replace("class DeviceInfoHelper\n{", "class DeviceInfoHelper\n{\npublic:"))
    parts.append(r'''
void Reset() { assert(!state.live_objects && !state.active_strings); state = State(); }
void Healthy() {
    Row cpu = {{L"Name", Property::String(std::wstring(500, L'C'))}, {L"Family", Property::Number(6)},
               {L"Level", Property::Number(4)}, {L"NumberOfCores", Property::Number(8)}};
    Row memory = {{L"Capacity", Property::String(L"1024")}, {L"TypeDetail", Property::Number(3)},
                  {L"Speed", Property::Number(3200)}};
    Row video = {{L"Name", Property::String(std::wstring(500, L'V'))}, {L"AdapterRAM", Property::Number(1024)},
                 {L"VideoProcessor", Property::String(std::wstring(500, L'P'))}};
    Row disk = {{L"Model", Property::String(std::wstring(500, L'D'))}};
    Row skipped = {{L"PhysicalAdapter", Property::Boolean(false)}};
    Row network = {{L"PhysicalAdapter", Property::Boolean(true)}, {L"NetEnabled", Property::Boolean(true)},
                   {L"Name", Property::String(std::wstring(500, L'N'))}, {L"MACAddress", Property::String(std::wstring(100, L'M'))},
                   {L"AdapterType", Property::String(L"Ethernet")}};
    state.rows[L"SELECT * FROM Win32_Processor"] = {cpu};
    state.rows[L"SELECT * FROM Win32_PhysicalMemory"] = {memory, memory};
    state.rows[L"SELECT * FROM Win32_VideoController"] = {video};
    state.rows[L"SELECT * FROM Win32_DiskDrive"] = {disk};
    state.rows[L"SELECT * FROM Win32_NetworkAdapter"] = {skipped, network};
}
void Balanced() {
    assert(!state.live_objects && state.created == state.released && !state.active_strings);
    assert(state.com_inits == state.com_uninits);
}
void Run() {
    DeviceInfoHelper helper;
    for (int stage = 1; stage <= 5; ++stage) {
        for (bool partial : {false, true}) {
            Reset(); state.fail_stage = stage; state.partial_failure = partial;
            C2G_CLIENT_HARDWARE_INFO info; memset(&info, 0xa5, sizeof(info));
            assert(!helper.GetHardwareInfo(info, TRUE));
            C2G_CLIENT_HARDWARE_INFO empty = {}; empty.wMsg = c2g_client_hardware_info;
            assert(!memcmp(&info, &empty, sizeof(info))); Balanced();
        }
    }
    for (int failure = 0; failure < 9; ++failure) {
        Reset(); Healthy();
        if (failure == 0) state.null_locator = true;
        if (failure == 1) state.null_service = true;
        if (failure == 2) state.query_fail = true;
        if (failure == 3) { state.query_fail = true; state.partial_failure = true; }
        if (failure == 4) state.null_enum = true;
        if (failure == 5) state.next_fail = true;
        if (failure == 6) { state.next_fail = true; state.partial_failure = true; }
        if (failure == 7) state.null_row = true;
        if (failure == 8) state.fail_get = true;
        C2G_CLIENT_HARDWARE_INFO info; memset(&info, 0x5a, sizeof(info));
        assert(helper.GetHardwareInfo(info, TRUE) == (failure >= 2));
        assert(info.wMsg == c2g_client_hardware_info); Balanced();
    }
    Reset(); Healthy(); C2G_CLIENT_HARDWARE_INFO info;
    assert(helper.GetHardwareInfo(info, TRUE)); Balanced();
    assert(info.stcpuinfo.Family == 6 && info.stcpuinfo.Level == 4 && info.stcpuinfo.NumberOfCores == 8);
    assert(strlen(info.stcpuinfo.Name) == MAX_DEVICE_STRING_LEN - 1);
    assert(strlen(info.stcpuinfo.Stepping) == MAX_DEVICE_STRING_LEN - 1);
    assert(info.stmemoryinfo.Capacity == 2048 && info.stmemoryinfo.Speed == 42);
    assert(info.stvideoinfo.AdapterRAM == 1024 && strlen(info.stvideoinfo.Name) == MAX_DEVICE_STRING_LEN - 1);
    assert(strlen(info.stdiskinfo.Model) == MAX_DEVICE_STRING_LEN - 1 && info.stdiskinfo.Size == 0x123456789ull);
    assert(strlen(info.stnetworkadapterinfo.Name) == MAX_DEVICE_STRING_LEN - 1);
    assert(strlen(info.stnetworkadapterinfo.NetworkAdapterInfo) == MAX_NETWORK_ADAPTERINFO_STRING_LEN - 1);
    for (int type = 0; type < 4; ++type) {
        Reset(); Healthy();
        Property wrong = Property::Number(123);
        if (type == 1) { wrong.type = VT_BSTR; wrong.null_string = true; }
        if (type == 2) wrong.type = VT_EMPTY;
        if (type == 3) wrong.type = VT_NULL;
        state.rows[L"SELECT * FROM Win32_Processor"][0][L"Name"] = wrong;
        state.memory_ok = false; state.os_ok = false;
        assert(helper.GetHardwareInfo(info, TRUE));
        assert(!info.stcpuinfo.Name[0] && !info.stdiskinfo.Size); Balanced();
    }
    Reset(); assert(helper.GetHardwareInfo(info)); Balanced();
    assert(!info.stcpuinfo.Name[0] && !info.stmemoryinfo.Capacity);
    puts("WMI initialized variants, typed properties, bounded strings and COM cleanup regressions passed");
}
}
#undef VariantInit
#undef VariantClear
#undef VariantChangeType
#undef CoInitializeEx
#undef CoInitializeSecurity
#undef CoCreateInstance
#undef CoSetProxyBlanket
#undef CoUninitialize
#undef TCHAR_TO_UTF8
#undef GetVersionEx
#define GetVersionEx GetVersionExW
#undef GlobalMemoryStatusEx
#undef warnf
#undef MAX_DEVICE_STRING_LEN
#undef MAX_NETWORK_ADAPTERINFO_STRING_LEN
''')
    return "\n".join(parts)
