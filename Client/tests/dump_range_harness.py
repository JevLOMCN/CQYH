"""Production minidump readers with real SDK records and adversarial RVAs/counts."""
import re


def harness(client, source, function):
    text = source(client / 'client/OtherTools/dump_analyze/FMiniDump.cpp')
    text = re.sub(r'''"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*[\s\S]*?\*/|//[^\n]*''',
                  lambda m: '' if m.group().startswith(('//', '/*')) else m.group(), text)
    parts = [r'''
#pragma push_macro("_T")
#undef _T
#define _T(x) x
namespace DumpDefaultsTests {
struct CStringW {
    std::wstring value;
    CStringW(const CString& text) : value(static_cast<const WCHAR*>(text)) {}
    CStringW(const WCHAR* text) : value(text ? text : L"") {}
    operator const WCHAR*() const { return value.c_str(); }
};
DWORD64 TestSymLoadModuleExW(HANDLE, HANDLE, const WCHAR*, const WCHAR*, DWORD64 base, DWORD, void*, DWORD) { return base; }
LPVOID openData = nullptr;
LONGLONG openSize = 0;
bool sizeSuccess = true, mapSuccess = true;
template<class... Args> HANDLE TestCreateFile(Args...) { return (HANDLE)1; }
BOOL TestGetFileSizeEx(HANDLE, LARGE_INTEGER* size) { size->QuadPart = openSize; return sizeSuccess; }
template<class... Args> HANDLE TestCreateFileMapping(Args...) { return (HANDLE)2; }
template<class... Args> LPVOID TestMapViewOfFile(Args...) { return mapSuccess ? openData : nullptr; }
DWORD TestSymSetOptions(DWORD options) { return options; }
BOOL TestSymInitializeW(HANDLE, const WCHAR*, BOOL) { return TRUE; }
''']
    for signature in ['bool FMiniDump::Open(', 'int FMiniDump::ReadSysinfo(',
                      'int FMiniDump::ReadException(', 'int FMiniDump::ReadModule(',
                      'int FMiniDump::ReadMemory(', 'int FMiniDump::ReadThread(',
                      'int FMiniDump::GetModuleRowIdByAddress(', 'void FMiniDump::GetExceptionModuleInfo(']:
        method = function(text, signature)
        for name in ['CreateFile', 'GetFileSizeEx', 'CreateFileMapping', 'MapViewOfFile',
                     'SymSetOptions', 'SymInitializeW', 'SymLoadModuleExW', 'SymGetModuleInfo64']:
            method = method.replace(name + '(', 'Test' + name + '(')
        parts.append(method)
    parts.append(r'''
struct DumpFixture {
    std::vector<BYTE> bytes;
    FMiniDump dump;
    DumpFixture(ULONG type, ULONG size) : bytes(4096, 0) {
        auto& header = At<MINIDUMP_HEADER>(0);
        header.Signature = MINIDUMP_SIGNATURE; header.Version = MINIDUMP_VERSION;
        header.NumberOfStreams = 1; header.StreamDirectoryRva = sizeof(header);
        auto& directory = Directory(); directory.StreamType = type;
        directory.Location.Rva = 64; directory.Location.DataSize = size;
        Bind();
    }
    template<class T> T& At(size_t offset) { assert(offset + sizeof(T) <= bytes.size()); return *reinterpret_cast<T*>(bytes.data() + offset); }
    MINIDUMP_DIRECTORY& Directory() { return At<MINIDUMP_DIRECTORY>(sizeof(MINIDUMP_HEADER)); }
    void Bind() { dump.lpMiniDumpAddress = bytes.data(); dump.MiniDumpSize = bytes.size(); }
    void String(ULONG rva, const std::wstring& value) {
        At<ULONG32>(rva) = ULONG(value.size() * sizeof(WCHAR));
        memcpy(bytes.data() + rva + sizeof(ULONG32), value.data(), value.size() * sizeof(WCHAR));
    }
};
void DumpRangeTests() {
    DumpFixture fixture(SystemInfoStream, sizeof(MINIDUMP_SYSTEM_INFO));
    auto& dump = fixture.dump;
    auto& info = fixture.At<MINIDUMP_SYSTEM_INFO>(64);
    info.MajorVersion = 10; info.NumberOfProcessors = 8; info.CSDVersionRva = 256;
    fixture.String(256, L"service");
    assert(dump.ReadSysinfo() == 0 && dump.nDump.m_ulVerMajor == 10 && dump.nDump.m_uchNumberOfProcessors == 8);
    assert(dump.nDump.m_sCSDVer.value == "service");
    for (size_t length = 0; length < 280; ++length) {
        std::vector<BYTE> truncated(fixture.bytes.begin(), fixture.bytes.begin() + length);
        dump.lpMiniDumpAddress = truncated.data(); dump.MiniDumpSize = length;
        assert((dump.ReadSysinfo() == 0) == (length >= 256 + 4 + 14));
    }
    fixture.Bind();
    LPVOID stream = (LPVOID)1; ULONG size = 99;
    fixture.Directory().Location.Rva = 4096; fixture.Directory().Location.DataSize = 1;
    assert(!dump.ReadDumpStream(SystemInfoStream, stream, size) && !stream && !size);
    fixture.Directory().Location.Rva = 64; fixture.Directory().Location.DataSize = sizeof(info);
    fixture.At<MINIDUMP_HEADER>(0).NumberOfStreams = 0xffffffff;
    assert(!dump.ReadDumpStream(SystemInfoStream, stream, size));
    fixture.At<MINIDUMP_HEADER>(0).NumberOfStreams = 1;
    fixture.At<MINIDUMP_HEADER>(0).StreamDirectoryRva = 4095;
    assert(!dump.ReadDumpStream(SystemInfoStream, stream, size));
    fixture.At<MINIDUMP_HEADER>(0).StreamDirectoryRva = sizeof(MINIDUMP_HEADER);
    assert(!dump.GetMappedRange(~ULONG64(0), 1));
    assert(!dump.GetMappedRange(1, ~ULONG64(0)));
    assert(dump.GetMappedRange(4096, 0) == fixture.bytes.data() + 4096);
    assert(!dump.GetMappedRange(4096, 1));
    CString string("old");
    for (ULONG rva : {ULONG(4093), ULONG(4096), ULONG(0xffffffff)})
        assert(!dump.RVA2String(dump.lpMiniDumpAddress, rva, string) && string.IsEmpty());
    fixture.At<ULONG32>(256) = 0xffffffff;
    assert(!dump.RVA2String(dump.lpMiniDumpAddress, 256, string));
    fixture.At<ULONG32>(256) = 3;
    assert(!dump.RVA2String(dump.lpMiniDumpAddress, 256, string));
    fixture.String(4090, L"X");
    assert(dump.RVA2String(dump.lpMiniDumpAddress, 4090, string) && string.value == "X");
    assert(!dump.RVA2String(nullptr, 4090, string) && string.IsEmpty());
    DumpFixture memory(MemoryListStream, FIELD_OFFSET(MINIDUMP_MEMORY_LIST, MemoryRanges) + sizeof(MINIDUMP_MEMORY_DESCRIPTOR));
    auto& ranges = memory.At<MINIDUMP_MEMORY_LIST>(64);
    ranges.NumberOfMemoryRanges = 1; ranges.MemoryRanges[0].StartOfMemoryRange = 100;
    ranges.MemoryRanges[0].Memory.Rva = 4088; ranges.MemoryRanges[0].Memory.DataSize = 8;
    memset(memory.bytes.data() + 4088, 7, 8);
    assert(memory.dump.ReadMemory() == 0 && memory.dump.nDump.m_MemRanges.size() == 1);
    memory.dump.nDump.m_hProcess = (HANDLE)1; FMiniDump::LPTHIS = &memory.dump;
    BYTE output[8] = {}; DWORD read = 99;
    assert(ReadProcessMemoryProc64((HANDLE)1, 104, output, 8, &read) && read == 4 && output[3] == 7 && output[4] == 0);
    memory.dump.MiniDumpSize = 4095;
    assert(!ReadProcessMemoryProc64((HANDLE)1, 104, output, 8, &read) && read == 0);
    memory.Bind();
    for (ULONG count : {ULONG(2), ULONG(0xffffffff)}) {
        ranges.NumberOfMemoryRanges = count; assert(memory.dump.ReadMemory() == 1);
    }
    ranges.NumberOfMemoryRanges = 1; ranges.MemoryRanges[0].Memory.Rva = 4090;
    assert(memory.dump.ReadMemory() == 1);
    DumpFixture threads(ThreadListStream, FIELD_OFFSET(MINIDUMP_THREAD_LIST, Threads) + sizeof(MINIDUMP_THREAD));
    auto& list = threads.At<MINIDUMP_THREAD_LIST>(64);
    list.NumberOfThreads = 1; list.Threads[0].ThreadId = 17;
    list.Threads[0].ThreadContext.Rva = 512; list.Threads[0].ThreadContext.DataSize = sizeof(CONTEXT);
    assert(threads.dump.ReadThread() == 0 && threads.dump.nDump.m_Threads.size() == 1);
    assert(threads.dump.nDump.m_Threads[0].m_pThreadContext == (CONTEXT*)(threads.bytes.data() + 512));
    list.NumberOfThreads = 0xffffffff; assert(threads.dump.ReadThread() == 1); list.NumberOfThreads = 1;
    for (ULONG bytes : {ULONG(1), ULONG(sizeof(CONTEXT) - 1), ULONG(0xffffffff)}) {
        list.Threads[0].ThreadContext.DataSize = bytes; assert(threads.dump.ReadThread() == 1);
    }
    list.Threads[0].ThreadContext.DataSize = sizeof(CONTEXT); list.Threads[0].ThreadContext.Rva = 4090;
    assert(threads.dump.ReadThread() == 1);
    list.Threads[0].ThreadContext.DataSize = 0;
    assert(threads.dump.ReadThread() == 0 && !threads.dump.nDump.m_Threads[0].m_pThreadContext);
    DumpFixture exception(ExceptionStream, sizeof(MINIDUMP_EXCEPTION_STREAM));
    auto& record = exception.At<MINIDUMP_EXCEPTION_STREAM>(64);
    record.ThreadId = 17; record.ExceptionRecord.ExceptionAddress = 0x1001;
    record.ThreadContext.Rva = 512; record.ThreadContext.DataSize = sizeof(CONTEXT);
    assert(exception.dump.ReadException() == 0 && exception.dump.nDump.m_uExceptionAddress == 0x1001);
    record.ThreadContext.Rva = 4090; assert(exception.dump.ReadException() == 1);
    record.ThreadContext.DataSize = 0;
    assert(exception.dump.ReadException() == 0 && !exception.dump.nDump.m_pExceptionThreadContext);
    DumpFixture modules(ModuleListStream, FIELD_OFFSET(MINIDUMP_MODULE_LIST, Modules) + sizeof(MINIDUMP_MODULE));
    auto& module = modules.At<MINIDUMP_MODULE_LIST>(64);
    module.NumberOfModules = 1; module.Modules[0].BaseOfImage = 0x1000;
    module.Modules[0].SizeOfImage = 0x100; module.Modules[0].ModuleNameRva = 256;
    module.Modules[0].VersionInfo.dwFileVersionMS = 123;
    modules.String(256, L"client.exe");
    assert(modules.dump.ReadModule() == 0 && modules.dump.nDump.m_Modules.size() == 1);
    assert(modules.dump.nDump.m_verExe.dwFileVersionMS == 123);
    assert(modules.dump.ReadModule() == 0 && modules.dump.nDump.m_Modules.size() == 1);
    assert(modules.dump.ReadModule(true) == 0 && modules.dump.nDump.m_Modules.size() == 1);
    modules.dump.bIsReadModule = modules.dump.bIsReadException = TRUE;
    modules.dump.nDump.m_uExceptionAddress = 0x1001;
    modules.dump.GetExceptionModuleInfo(); assert(modules.dump.nDump.m_uExceptionOffsetFromModuleBase == 1);
    assert(modules.dump.GetModuleRowIdByAddress(0x1100) == -1);
    module.NumberOfModules = 0xffffffff; assert(modules.dump.ReadModule() == 1);
    module.NumberOfModules = 1; module.Modules[0].ModuleNameRva = 4095;
    assert(modules.dump.ReadModule() == 1);
    DumpFixture opener(SystemInfoStream, sizeof(MINIDUMP_SYSTEM_INFO));
    openData = opener.bytes.data(); sizeSuccess = mapSuccess = true;
    for (LONGLONG length : {LONGLONG(-1), LONGLONG(0), LONGLONG(sizeof(MINIDUMP_HEADER) - 1)}) {
        openSize = length; assert(!opener.dump.Open("dump", nullptr)); assert(!opener.dump.MiniDumpSize);
    }
    openSize = opener.bytes.size(); sizeSuccess = false;
    assert(!opener.dump.Open("dump", nullptr)); sizeSuccess = true; mapSuccess = false;
    assert(!opener.dump.Open("dump", nullptr)); mapSuccess = true;
    opener.At<MINIDUMP_HEADER>(0).Signature = 0;
    assert(!opener.dump.Open("dump", nullptr)); opener.At<MINIDUMP_HEADER>(0).Signature = MINIDUMP_SIGNATURE;
    assert(opener.dump.Open("dump", nullptr)); assert(opener.dump.MiniDumpSize == opener.bytes.size()); opener.dump.Close();
    FMiniDump::LPTHIS = nullptr;
    puts("Minidump readers bound mapped streams, strings, array counts and contexts; truncated files and stale callback ranges reject safely");
}
}
#pragma pop_macro("_T")
''')
    return '\n'.join(parts)
