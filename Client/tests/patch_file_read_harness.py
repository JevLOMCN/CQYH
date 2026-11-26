"""Exercise file-copy, BOM and both compression input branches without MFC."""
import re


def fixtures(client, source, function):
    parts = [source(client / "client/PatchTools/ReadFileBuffer.h"), r'''
namespace PatchReadTests {
struct Reader {
    unsigned long long length;
    unsigned int supplied;
    int reads = 0;
    unsigned long long GetLength() { return length; }
    unsigned int Read(void* out, unsigned int size) {
        ++reads; assert(supplied <= size); memset(out, 'R', supplied); return supplied;
    }
};
void Run() {
    for (unsigned int size : {0u, 1u, 17u, 4096u, 4097u}) {
        for (unsigned int supplied : {0u, size / 2, size}) {
            Reader reader{size, supplied};
            std::vector<unsigned char> bytes(3, '?');
            assert(ClientPatch::ReadFileBuffer(reader, bytes) == (size == supplied));
            assert(bytes.size() == (size == supplied ? size : 0));
            for (auto byte : bytes) assert(byte == 'R');
        }
    }
    for (auto size : {0x100000000ULL, 0xffffffffffffffffULL}) {
        Reader reader{size, 0};
        std::vector<unsigned char> bytes(3, '?');
        assert(!ClientPatch::ReadFileBuffer(reader, bytes) && bytes.empty() && !reader.reads);
    }
    Reader too_big{10, 0}; std::vector<unsigned char> bytes;
    assert(!ClientPatch::ReadFileBuffer(too_big, bytes, 9) && !too_big.reads);
    struct Bom {
        unsigned char data[2]; unsigned int count;
        unsigned int Read(void* output, unsigned int size) {
            assert(size == 2); memcpy(output, data, count); return count;
        }
    } bom;
    for (unsigned char first : {0xfe, 0xff, 0, 0x61}) {
        for (unsigned char second : {0xfe, 0xff, 0, 0x62}) {
            for (unsigned int count = 0; count <= 2; ++count) {
                bom = {{first, second}, count};
                assert(ClientPatch::HasUnicodeBom(bom) == (count == 2 &&
                    ((first == 0xfe && second == 0xff) || (first == 0xff && second == 0xfe))));
            }
        }
    }
    puts("File read length and BOM short-read regressions passed");
}
}
namespace CompressionMock {
unsigned long long length = 3;
unsigned int supplied = 3;
bool mapping_ok = true, view_ok = true, compression_ok = true;
int compressed = 0, outputs = 0, writes = 0, mapped = 0, unmapped = 0, maps_closed = 0;
std::vector<BYTE> input{'a', 'b', 'c'};
void Reset() {
    length = supplied = 3; mapping_ok = view_ok = compression_ok = true;
    compressed = outputs = writes = mapped = unmapped = maps_closed = 0;
    input.assign({'a', 'b', 'c'});
}
struct CString {
    std::wstring value;
    CString(const wchar_t* p = L"") : value(p) {}
    CString(const char* p) { while (*p) value += *p++; }
    operator const wchar_t*() const { return value.c_str(); }
    CString operator+(const CString& rhs) const { return CString((value + rhs.value).c_str()); }
    friend CString operator+(const wchar_t* left, const CString& rhs) { return CString(left) + rhs; }
    CString& operator+=(const CString& rhs) { value += rhs.value; return *this; }
    int ReverseFind(wchar_t c) { auto pos = value.rfind(c); return pos == value.npos ? -1 : (int)pos; }
    CString Left(int count) { return CString(value.substr(0, std::max(count, 0)).c_str()); }
};
struct CFile {
    enum { modeRead = 1, modeReadWrite = 2, typeBinary = 4, modeCreate = 8 };
    bool Open(const CString&, int mode, int) { if (mode & modeCreate) ++outputs; return true; }
    void Close() {}
    unsigned long long GetLength() { return length; }
    unsigned int Read(void* buffer, unsigned int size) {
        assert(size <= input.size()); assert(supplied <= size);
        memcpy(buffer, input.data(), supplied); return supplied;
    }
    void Write(const void*, unsigned int count) { ++writes; assert(count <= 64); }
    operator HANDLE() const { return reinterpret_cast<HANDLE>(1); }
};
struct SVersionInfo { unsigned int _originLen = 3; CString _fullPathName, _relatedPathName, _codeMD5; };
struct CSingleLock { CSingleLock(int*, int) {} };
int g_logLock = 0;
CString g_targPath, g_versionName;
bool g_useVerMD5FileNameFormat = false;
struct File { static CString GetVerMD5FileName(const CString& p, const CString&, const CString&) { return p; } };
using Md53::MD5;
enum { Z_OK = 0 };
using uLong = unsigned long;
''']
    # Exercise the bound from the repository's real zlib implementation.
    zlib_bound = function(source(client / "client/UE3/Development/External/zlib/Src/compress.c"),
                          "uLong ZEXPORT compressBound (")
    parts.append("uLong compressBound(uLong sourceLen) " + zlib_bound[zlib_bound.index("{"):])
    parts.append(r'''
int compress(BYTE* output, unsigned long* size, const BYTE* source, unsigned int count) {
    ++compressed;
    assert(*size == compressBound(count));
    assert(count <= input.size() && !memcmp(source, input.data(), count));
    if (!compression_ok) return -1;
    *size = 2; output[0] = 'Z'; output[1] = 'P'; return Z_OK;
}
}
HANDLE MockCreateMapping(HANDLE, void*, DWORD, DWORD, DWORD, const wchar_t*) {
    return CompressionMock::mapping_ok ? reinterpret_cast<HANDLE>(2) : nullptr;
}
void* MockMapView(HANDLE, DWORD, DWORD, DWORD, size_t length) {
    if (!CompressionMock::view_ok) return nullptr;
    assert(length <= CompressionMock::input.size());
    ++CompressionMock::mapped; return CompressionMock::input.data();
}
BOOL MockUnmapView(const void*) { ++CompressionMock::unmapped; return TRUE; }
BOOL MockCloseMapping(HANDLE) { ++CompressionMock::maps_closed; return TRUE; }
#undef CreateFileMapping
#define CreateFileMapping MockCreateMapping
#define MapViewOfFile MockMapView
#define UnmapViewOfFile MockUnmapView
#define CloseHandle MockCloseMapping
''')
    task = source(client / "client/PatchTools/vigenerator/Task.cpp")
    method = function(task, "void AbstractCompressTask::Compress(")
    for mapped in [0, 1]:
        parts.append("namespace Compression%d {\nusing namespace CompressionMock;\n" % mapped)
        parts.append("struct AbstractCompressTask { int _idx = 0; void Compress(SVersionInfo*); };\n")
        parts.append("#define M_USE_FILE_MAP %d\n" % mapped)
        parts.append(method)
        parts.append("#undef M_USE_FILE_MAP\n")
        parts.append(r'''
void Run() {
    for (int failure = 0; failure < 8; ++failure) {
        Reset(); SVersionInfo info;
        switch (failure) {
        case 0: break;
        case 1: length = 2; break;
        case 2: length = 4; break;
        case 3: supplied = 2; break;
        case 4: mapping_ok = false; break;
        case 5: view_ok = false; break;
        case 6: length = info._originLen = (unsigned int)-1; break;
        case 7: compression_ok = false; break;
        }
        AbstractCompressTask task;
        task.Compress(&info);
''')
        parts.append("bool expected = " + ("failure == 0 || failure == 4 || failure == 5" if not mapped
                                            else "failure == 0 || failure == 3") + ";\n")
        parts.append(r'''
        assert((info._originLen != 0) == expected);
        assert(outputs == expected && writes == expected);
        assert(unmapped == mapped);
''')
        if mapped:
            parts.append("assert(maps_closed == (failure != 1 && failure != 2 && failure != 4 && failure != 6));\n")
        parts.append("}\nReset(); length = supplied = 0; SVersionInfo empty; empty._originLen = 0;\n"
                     "AbstractCompressTask task; task.Compress(&empty); assert(outputs == 1 && writes == 1);\n}\n}\n")
    parts.append(r'''
#undef CreateFileMapping
#undef MapViewOfFile
#undef UnmapViewOfFile
#undef CloseHandle
namespace CopyVersionTests {
using CompressionMock::CString;
using CompressionMock::CFile;
struct Patch { bool _bShouldUpdateViFile = true; CString _strViFileName; };
struct CFilePatchDlg {
    std::vector<Patch> _vectorFilePatches{Patch()};
    CString _strTmpDir, _strLog;
    bool _bPatchSuccess = true;
    void DoPatchVi();
};
''')
    parts.append(function(source(client / "client/PatchTools/FilePatch/FilePatchDlg.cpp"), "void CFilePatchDlg::DoPatchVi()"))
    parts.append(r'''
void Run() {
    for (int test = 0; test < 4; ++test) {
        CompressionMock::Reset();
        if (test == 1) CompressionMock::supplied = 2;
        if (test == 2) CompressionMock::length = 0x100000000ULL;
        if (test == 3) CompressionMock::length = CompressionMock::supplied = 0;
        CFilePatchDlg dlg;
        dlg.DoPatchVi();
        bool complete = test == 0 || test == 3;
        assert(dlg._bPatchSuccess == complete);
        assert(CompressionMock::outputs == complete);
        assert(CompressionMock::writes == (test == 0));
    }
    puts("Compression input, mapping failure and version-copy regressions passed");
}
}
''')
    return "\n".join(parts)
