"""Compile production archive readers and their FixClient hashing caller with failure mocks."""


def fixtures(client, source, function):
    patch = source(client / "client/PatchTools/pakpatcher/PakPatcher.cpp")
    parts = [r'''
#include <tchar.h>
namespace ArchiveMemberTests {
enum { STREAM_PROVIDER_FLAT = 0, STREAM_FLAG_READ_ONLY = 1 };
bool archive_ok = true, file_ok = true, read_ok = true;
DWORD file_size = 3, size_high = 0, actual_read = 3;
std::vector<unsigned char> payload{'a', 'b', 'c', 'd'};
int archives_opened = 0, archives_closed = 0, files_opened = 0, files_closed = 0, reads = 0;
bool change_size = false;
DWORD next_size = 0;
void Reset() {
    archive_ok = file_ok = read_ok = true;
    file_size = actual_read = 3; size_high = 0;
    archives_opened = archives_closed = files_opened = files_closed = reads = 0;
    change_size = false;
    payload.assign({'a', 'b', 'c', 'd'});
}
bool SFileOpenArchive(const char*, int, int flags, HANDLE* archive) {
    ++archives_opened;
    assert(flags & STREAM_FLAG_READ_ONLY);
    if (!archive_ok) return false;
    *archive = reinterpret_cast<HANDLE>(1); return true;
}
bool SFileOpenFileEx(HANDLE, const char*, int, HANDLE* file) {
    ++files_opened;
    if (!file_ok) return false;
    *file = reinterpret_cast<HANDLE>(2); return true;
}
DWORD SFileGetFileSize(HANDLE, DWORD* high) {
    *high = size_high;
    DWORD result = file_size;
    if (change_size) file_size = next_size;
    return result;
}
bool SFileReadFile(HANDLE, void* buffer, DWORD size, DWORD* read, void*) {
    ++reads;
    assert(size <= payload.size());
    memcpy(buffer, payload.data(), std::min(size, actual_read));
    *read = actual_read;
    return read_ok;
}
void SFileCloseFile(HANDLE) { ++files_closed; }
void SFileCloseArchive(HANDLE) { ++archives_closed; }
using Utility2::PathString;
using Md52::MD5;
using HashComparisonTests::GetFileSize;
''']
    for signature in ["DWORD GetFileSizeInternal(", "bool GetFileBufferInternal(",
                      "DWORD GetPackFileSize(", "bool GetPackFileBuffer(", "bool GetPackFileBufferSized("]:
        parts.append(function(patch, signature))
    parts.append(function(source(client / "client/PatchTools/pakpatcher/PakPatcher.inl"),
                          "bool CompareFile(const PathString& FileName,"))
    parts.append(r'''
void Run() {
    FILE* local = nullptr;
    assert(!fopen_s(&local, "archive-source.txt", "wb"));
    fputs("abc", local); fclose(local);
    for (int failure = 0; failure < 7; ++failure) {
        Reset();
        switch (failure) {
        case 0: break;
        case 1: archive_ok = false; break;
        case 2: file_ok = false; break;
        case 3: size_high = 1; break;
        case 4: file_size = (DWORD)-1; break;
        case 5: read_ok = false; break;
        case 6: actual_read = 2; break;
        }
        assert(CompareFile("archive-source.txt", "archive", "member") == (failure != 0));
        assert(archives_closed == (failure != 1));
        assert(files_closed == (failure != 1 && failure != 2));
    }
    Reset(); payload.assign({'d', 'e', 'f'});
    assert(CompareFile("archive-source.txt", "archive", "member"));
    for (DWORD size = 0; size <= 17; ++size) {
        for (DWORD capacity = 0; capacity <= 19; ++capacity) {
            Reset(); file_size = actual_read = size; payload.assign(size, 'P');
            unsigned char buffer[32]; memset(buffer, 0xa5, sizeof(buffer));
            DWORD returned = 0xffffffff;
            assert(GetFileBufferInternal("archive", "member", buffer + 1, capacity, &returned) == (size <= capacity));
            assert(returned == (size <= capacity ? size : 0));
            assert(reads == (size <= capacity));
            assert(archives_closed == 1 && files_closed == 1);
            assert(buffer[0] == 0xa5);
            for (size_t i = capacity + 1; i < sizeof(buffer); ++i) assert(buffer[i] == 0xa5);
            Reset(); file_size = actual_read = size; payload.assign(size, 'P');
            assert(GetPackFileBufferSized(L"archive", L"member", buffer + 1, capacity) == (size == capacity));
        }
    }
    for (int failure = 0; failure < 6; ++failure) {
        Reset();
        switch (failure) {
        case 0: archive_ok = false; break;
        case 1: file_ok = false; break;
        case 2: size_high = 1; break;
        case 3: file_size = (DWORD)-1; break;
        case 4: read_ok = false; break;
        case 5: actual_read = 2; break;
        }
        unsigned char buffer[3];
        assert(!GetPackFileBufferSized(L"archive", L"member", buffer, sizeof(buffer)));
        assert(archives_closed == (failure != 0));
        assert(files_closed == (failure > 1));
        // The legacy export must also report failed or incomplete reads correctly.
        assert(!GetPackFileBuffer(L"archive", L"member", buffer));
    }
    for (DWORD size : {DWORD(0), DWORD(1), DWORD(17), DWORD(-1)}) {
        Reset(); file_size = size;
        assert(GetPackFileSize(L"archive", L"member") == (size == (DWORD)-1 ? 0 : size));
        assert(archives_closed == 1 && files_closed == 1);
    }
    Reset(); size_high = 1;
    assert(!GetPackFileSize(L"archive", L"member"));
    Reset();
    DWORD returned = 99;
    unsigned char buffer[3];
    assert(!GetFileBufferInternal(nullptr, "member", buffer, 3, &returned) && !returned);
    assert(!GetFileBufferInternal("archive", nullptr, buffer, 3, &returned));
    assert(!GetFileBufferInternal("archive", "member", nullptr, 3, &returned));
    assert(!archives_opened);
    puts("Archive member capacity, short-read and resource-cleanup regressions passed");
}
}
namespace ArchiveTaskTests {
struct CString {
    std::wstring value;
    CString(const wchar_t* text = L"") : value(text) {}
    operator const wchar_t*() const { return value.c_str(); }
    int GetLength() { return (int)value.size(); }
    wchar_t* GetBuffer(int) { return value.data(); }
    void ReleaseBuffer() {}
    CString& operator=(const wchar_t* text) { value = text; return *this; }
    CString& operator=(const char* text) { value.clear(); while (*text) value += *text++; return *this; }
};
using Md50::MD5;
''')
    header = source(client / "client/PatchTools/FixClient/Task.h")
    parts.extend(line for line in header.splitlines() if line.startswith(
        ("typedef bool(*pfGetPackFileBufferSized)", "typedef DWORD(*pfGetPackFileSize)")))
    parts.append(r'''
pfGetPackFileBufferSized g_pfGetPackFileBufferSized = ArchiveMemberTests::GetPackFileBufferSized;
pfGetPackFileSize g_pfGetPackFileSize = ArchiveMemberTests::GetPackFileSize;
LONG g_genMD5TaskCompletedCount = 0;
int g_csGenMD5 = 0;
void EnterCriticalSection(int*) { ++g_csGenMD5; }
void LeaveCriticalSection(int*) { assert(g_csGenMD5 == 1); --g_csGenMD5; }
void LogMsgToUI(const CString&) {}
bool fail_allocation = false;
void* Allocate(size_t size) { return fail_allocation ? nullptr : std::malloc(size); }
struct GenerateMD5Task {
    bool _isMPQ = true;
    CString _srcFile = L"archive", _srcSubFile = L"member", _targetMD5 = L"old result";
    void Run();
};
#define M_LOG_TO_CONSOLE 0
#define malloc Allocate
''')
    parts.append(function(source(client / "client/PatchTools/FixClient/Task.cpp"), "void GenerateMD5Task::Run()"))
    parts.append(r'''
#undef malloc
#undef M_LOG_TO_CONSOLE
void Run() {
    using namespace ArchiveMemberTests;
    for (int failure = 0; failure < 6; ++failure) {
        Reset();
        switch (failure) {
        case 0: break;
        case 1: change_size = true; next_size = actual_read = 4; break;
        case 2: change_size = true; next_size = actual_read = 2; break;
        case 3: read_ok = false; break;
        case 4: actual_read = 2; break;
        case 5: fail_allocation = true; break;
        }
        GenerateMD5Task task;
        g_genMD5TaskCompletedCount = 0;
        task.Run();
        assert(task._targetMD5.value == (failure ? L"Fail" : L"900150983cd24fb0d6963f7d28e17f72"));
        assert(g_genMD5TaskCompletedCount == 1 && !g_csGenMD5);
        fail_allocation = false;
    }
    FILE* file = nullptr;
    assert(!_wfopen_s(&file, L"task-hash.txt", L"wb"));
    fputs("abc", file); fclose(file);
    GenerateMD5Task task;
    task._isMPQ = false; task._srcFile = L"task-hash.txt";
    task.Run();
    assert(task._targetMD5.value == L"900150983cd24fb0d6963f7d28e17f72");
    puts("FixClient sized-read, size-change and hashing regressions passed");
}
}
''')
    return "\n".join(parts)
