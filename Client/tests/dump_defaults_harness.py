"""Actual dump records, lifecycle and stack queries with poisoned storage."""
import re


def harness(client, source, function):
    root = client / 'client/OtherTools/dump_analyze'

    def clean(path):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(root / path))

    header, cpp = clean('FMiniDump.h'), clean('FMiniDump.cpp')
    parts = [r'''
#include <dbghelp.h>
#include <cstdarg>
#pragma push_macro("_T")
#undef _T
#define _T(x) x
namespace DumpDefaultsTests {
using TCHAR = char;
struct CString {
    std::string value;
    mutable std::wstring wide;
    CString(const char* text = "") : value(text ? text : "") {}
    CString(const char* text, INT length) : value(text, length) {}
    CString(const WCHAR* text, INT length) {
        INT count = WideCharToMultiByte(CP_UTF8, 0, text, length, nullptr, 0, nullptr, nullptr);
        value.resize(count);
        if (count) assert(WideCharToMultiByte(CP_UTF8, 0, text, length, &value[0], count, nullptr, nullptr) == count);
    }
    CString(const WCHAR* text) : CString(text, text ? (INT)wcslen(text) : 0) {}
    operator const char*() const { return value.c_str(); }
    operator const WCHAR*() const {
        INT count = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
        wide.resize(count);
        assert(MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &wide[0], count) == count);
        return wide.c_str();
    }
    bool IsEmpty() const { return value.empty(); }
    void Empty() { value.clear(); }
    INT Find(const char* needle) const { auto index = value.find(needle); return index == std::string::npos ? -1 : (INT)index; }
    char* GetBuffer() { return const_cast<char*>(value.c_str()); }
    CString& operator+=(const CString& text) { value += text.value; return *this; }
    INT ReverseFind(char c) { auto index = value.rfind(c); return index == std::string::npos ? -1 : (INT)index; }
    CString Mid(INT index) { return value.substr(index).c_str(); }
    static const char* FormatArg(const CString& text) { return text.value.c_str(); }
    template<class T> static T FormatArg(T value) { return value; }
    template<class... Args> void Format(const char* format, Args... args) {
        char buffer[1024]; snprintf(buffer, sizeof(buffer), format, FormatArg(args)...); value = buffer;
    }
};
''']
    for name in ['DUMP_MODULE', 'DUMP_STACK', 'DUMP_THREAD', 'DUMP_MEMORY', 'DUMP_INFO']:
        parts.append(function(header, 'struct ' + name + '\n') + ';')
    parts.extend([function(header, 'class DUMP_LOG\n') + ';',
                  function(header, 'class FMiniDump\n') + ';', r'''
FMiniDump* FMiniDump::LPTHIS = nullptr;
INT unmaps = 0, closes = 0, cleanups = 0, walks = 0, lines = 0;
bool successfulLine = false;
BOOL UnmapViewOfFile(void*) { ++unmaps; return TRUE; }
BOOL CloseHandle(HANDLE) { ++closes; return TRUE; }
BOOL SymCleanup(HANDLE) { ++cleanups; return TRUE; }
BOOL CALLBACK ReadProcessMemoryProc64(HANDLE, DWORD64, PVOID, DWORD, LPDWORD);
PVOID CALLBACK FunctionTableAccessProc64(HANDLE, DWORD64) { return nullptr; }
DWORD64 CALLBACK GetModuleBaseProc64(HANDLE, DWORD64) { return 0; }
BOOL TestStackWalk64(DWORD, HANDLE, HANDLE, STACKFRAME64* frame, CONTEXT*,
    PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64,
    PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64) {
    if (walks++) return FALSE;
    frame->AddrPC.Offset = 0x1234; return TRUE;
}
BOOL TestSymGetModuleInfo64(HANDLE, DWORD64, IMAGEHLP_MODULE64* info) {
    assert(info->SizeOfStruct == sizeof(*info) && !info->BaseOfImage && !info->ImageSize);
    return FALSE;
}
BOOL TestSymFromAddr(HANDLE, DWORD64, DWORD64* displacement, SYMBOL_INFO* info) {
    assert(!*displacement && info->SizeOfStruct == sizeof(*info));
    assert(!info->Flags && !info->NameLen && !info->TypeIndex && !info->Address);
    return FALSE;
}
BOOL TestSymGetLineFromAddr64(HANDLE, DWORD64, DWORD* displacement, IMAGEHLP_LINE64* line) {
    assert(!*displacement && line->SizeOfStruct == sizeof(*line));
    assert(!line->Key && !line->FileName && !line->LineNumber && !line->Address); ++lines;
    if (!successfulLine) return FALSE;
    line->FileName = const_cast<char*>("source.cpp"); line->LineNumber = 23; return TRUE;
}
'''])
    for signature in ['FMiniDump::FMiniDump()', 'FMiniDump::~FMiniDump()', 'void FMiniDump::Close()',
                      'int FMiniDump::GetThreadRowIdByThreadId(', 'int FMiniDump::GetModuleRowIdByBaseAddr(',
                      'LPVOID FMiniDump::GetMappedRange(', 'BOOL FMiniDump::ContainsMappedRange(',
                      'BOOL FMiniDump::ReadDumpStream(', 'BOOL FMiniDump::RVA2String(']:
        parts.append(function(cpp, signature))
    parts.append(function(cpp, 'BOOL CALLBACK ReadProcessMemoryProc64('))
    # Redirect external SDK calls while retaining their real argument structs.
    stack = function(cpp, 'int FMiniDump::StackWalk(')
    stack = stack.replace('::StackWalk64(', 'DumpDefaultsTests::TestStackWalk64(')
    for name in ['SymGetModuleInfo64', 'SymFromAddr', 'SymGetLineFromAddr64']:
        stack = stack.replace(name + '(', 'Test' + name + '(')
    parts.append(stack)
    parts.append(r'''
void CheckInfo(const DUMP_INFO& info) {
    assert(!info.m_uProcessorArchitecture && !info.m_uchNumberOfProcessors && !info.m_uchProductType);
    assert(!info.m_ulVerMajor && !info.m_ulVerMinor && !info.m_ulVerBuild && info.m_sCSDVer.IsEmpty());
    VS_FIXEDFILEINFO zero = {}; assert(!memcmp(&info.m_verExe, &zero, sizeof(zero)));
    assert(!info.m_uExceptionCode && !info.m_uExceptionAddress && !info.m_uExceptionThreadId);
    assert(!info.m_pExceptionThreadContext && !info.m_pExceptionModule && !info.m_uExceptionOffsetFromModuleBase);
    assert(info.m_Threads.empty() && info.m_Modules.empty() && info.m_MemRanges.empty());
}
void Run() {
    {
        alignas(DUMP_MODULE) BYTE bytes[sizeof(DUMP_MODULE)]; memset(bytes, 0xff, sizeof(bytes));
        auto* value = new(bytes) DUMP_MODULE;
        assert(!value->m_uBaseAddr && !value->m_uImageSize && !value->m_bImageUnmatched);
        assert(!value->m_bPdbUnmatched && !value->m_bNoSymbolInfo && !value->m_pVersionInfo);
        value->~DUMP_MODULE();
    }
    {
        alignas(DUMP_STACK) BYTE bytes[sizeof(DUMP_STACK)]; memset(bytes, 0xff, sizeof(bytes));
        auto* value = new(bytes) DUMP_STACK;
        assert(!value->m_dwAddrPCOffset && !value->m_dw64OffsInSymbol);
        assert(value->m_nModuleRowID == -1 && value->m_nSrcLineNumber == -1); value->~DUMP_STACK();
    }
    {
        alignas(DUMP_MEMORY) BYTE bytes[sizeof(DUMP_MEMORY)]; memset(bytes, 0xff, sizeof(bytes));
        auto* value = new(bytes) DUMP_MEMORY;
        assert(!value->m_u64StartOfMemoryRange && !value->m_uDataSize && !value->m_pStartPtr);
    }
    alignas(FMiniDump) BYTE bytes[sizeof(FMiniDump)]; memset(bytes, 0xff, sizeof(bytes));
    auto* dump = new(bytes) FMiniDump;
    CheckInfo(dump->nDump); assert(!dump->IsOpen());
    CONTEXT context = {};
    dump->lpMiniDumpAddress = &context; dump->MiniDumpSize = sizeof(context);
    for (bool success : {false, true}) {
        successfulLine = success; walks = 0;
        DUMP_THREAD thread; thread.m_dwThreadId = 1; thread.m_pThreadContext = &context;
        dump->nDump.m_Threads = {thread}; dump->nDump.m_ThreadIndex[1] = 0;
#if defined(_M_IX86)
        dump->nDump.m_uProcessorArchitecture = PROCESSOR_ARCHITECTURE_INTEL;
#else
        dump->nDump.m_uProcessorArchitecture = PROCESSOR_ARCHITECTURE_AMD64;
#endif
        assert(dump->StackWalk(1) == 0);
        const auto& stack = dump->nDump.m_Threads[0].m_StackTrace;
        assert(stack.size() == 1 && stack[0].m_dwAddrPCOffset == 0x1234);
        assert(stack[0].m_nSrcLineNumber == (success ? 23 : -1));
    }
    assert(lines == 2);
    BYTE input[8] = {1, 2, 3, 4, 5, 6, 7, 8}, output[8] = {};
    DWORD read = 99;
    FMiniDump::LPTHIS = nullptr;
    assert(!ReadProcessMemoryProc64((HANDLE)1, 100, output, 8, &read) && read == 0);
    assert(!ReadProcessMemoryProc64((HANDLE)1, 100, output, 8, nullptr));
    dump->nDump.m_hProcess = (HANDLE)1; FMiniDump::LPTHIS = dump;
    dump->lpMiniDumpAddress = input; dump->MiniDumpSize = sizeof(input);
    DUMP_MEMORY memory; memory.m_u64StartOfMemoryRange = 100;
    memory.m_uDataSize = sizeof(input); memory.m_pStartPtr = input;
    dump->nDump.m_MemRanges = {memory};
    assert(ReadProcessMemoryProc64((HANDLE)1, 102, output, 8, &read));
    assert(read == 6 && memcmp(output, input + 2, read) == 0 && output[6] == 0);
    read = 99; assert(!ReadProcessMemoryProc64((HANDLE)2, 100, output, 8, &read) && read == 0);
    read = 99; assert(!ReadProcessMemoryProc64((HANDLE)1, 108, output, 8, &read) && read == 0);
    dump->nDump.m_MemRanges[0].m_pStartPtr = nullptr;
    assert(!ReadProcessMemoryProc64((HANDLE)1, 100, output, 8, &read) && read == 0);
    dump->nDump.m_hProcess = (HANDLE)1; dump->nDump.m_uchNumberOfProcessors = 8;
    dump->nDump.m_verExe.dwFileVersionMS = 123;
    dump->nDump.m_pExceptionThreadContext = &context; dump->nDump.m_sCSDVer = "old";
    dump->nDump.m_Modules.emplace_back(); dump->nDump.m_MemRanges.emplace_back();
    dump->hFileMiniDump = (HANDLE)2; dump->hFileMapping = (HANDLE)3;
    FMiniDump::LPTHIS = dump;
    dump->Close(); CheckInfo(dump->nDump);
    assert(!dump->MiniDumpSize);
    assert(!dump->IsOpen() && !FMiniDump::LPTHIS && closes == 2 && cleanups == 1);
    read = 99; assert(!ReadProcessMemoryProc64((HANDLE)1, 100, output, 8, &read) && read == 0);
    dump->~FMiniDump();
    puts("Dump records initialize over poisoned storage; close clears prior metadata; stack symbol/line queries receive initialized SDK structs");
}
}
#pragma pop_macro("_T")
''')
    return '\n'.join(parts)
