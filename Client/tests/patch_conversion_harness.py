"""Exercise production patch helpers with real conversions and fake filesystem calls."""
import re


def harness(client, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    parts = [r'''
#define UNICODE
#define _UNICODE
#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <new>
int filesystem_calls = 0;
DWORD TestGetFileAttributes(LPCWSTR) { ++filesystem_calls; return INVALID_FILE_ATTRIBUTES; }
BOOL TestCreateDirectory(LPCWSTR, void*) { ++filesystem_calls; return TRUE; }
HANDLE TestCreateFile(LPCWSTR, DWORD, DWORD, void*, DWORD, DWORD, HANDLE) {
    ++filesystem_calls; return INVALID_HANDLE_VALUE;
}
HANDLE TestFindFirstFile(LPCWSTR, WIN32_FIND_DATA*) {
    ++filesystem_calls; return INVALID_HANDLE_VALUE;
}
BOOL TestDeleteFile(LPCWSTR) { ++filesystem_calls; return TRUE; }
BOOL TestRemoveDirectory(LPCWSTR) { ++filesystem_calls; return TRUE; }
BOOL TestSetFileAttributes(LPCWSTR, DWORD) { ++filesystem_calls; return TRUE; }
#undef GetFileAttributes
#undef CreateDirectory
#undef CreateFile
#undef FindFirstFile
#undef DeleteFile
#undef RemoveDirectory
#undef SetFileAttributes
#define GetFileAttributes TestGetFileAttributes
#define CreateDirectory TestCreateDirectory
#define CreateFile TestCreateFile
#define FindFirstFile TestFindFirstFile
#define DeleteFile TestDeleteFile
#define RemoveDirectory TestRemoveDirectory
#define SetFileAttributes TestSetFileAttributes
#define check(value) assert(value)
#define STR_DIRECT_FILE_SUB_ITEM_KEY_ATTRIB "Key"
#define STR_DIRECT_FILE_SUB_ITEM_VALUE_ATTRIB "Value"
#define PATCH_DEL_TABLE_FILE_MAGIC "MLED"
#define PATCH_DEL_TABLE_FILE_VER "1.0.0.0"
#define PATCH_RENAME_TABLE_FILE_MAGIC "MNER"
#define PATCH_RENAME_TABLE_FILE_VER "1.0.0.0"
''']
    for index, path in enumerate(["FixClient/FixClientUtil.h", "PatchMPQ/PatchMPQUtil.h",
                                  "pakpatcher/PakPatcher.inl"]):
        text = clean(source(client / "client/PatchTools" / path))
        parts.append("namespace Utility%d {\nusing ::GetFileSize;\n" % index)
        parts.append(function(text, "class PathString") + ";")
        parts.append(text[text.index("bool ExtractString("):text.index("void GetConfigDirs(")])
        parts.append(function(text, "class DeleteFileMap") + ";")
        parts.append(function(text, "class RenameFileMap") + ";")
        parts.append(function(text, "void GetConfigDirs("))
        if index < 2:
            parts.append(r'''
std::wstring attribute_key, attribute_value;
struct CMarkup {
    int finds = 0;
    bool Load(LPCTSTR) { return true; }
    bool FindElem() { return ++finds <= 2; }
    bool FindElem(LPCTSTR) { return true; }
    bool IntoElem() { return true; }
    void OutOfElem() {}
    const wchar_t* GetAttrib(const wchar_t* key) {
        return key[0] == L'K' ? attribute_key.c_str() : attribute_value.c_str();
    }
};
''')
            parts.append(function(text, "bool ParseDirectFileSection("))
            for signature in ["bool IsDirExist(", "bool IsFileExist(", "bool mkDir(",
                              "INT CreateDirectoryHierarchy(", "DWORD GetFileSize(",
                              "void EnumFilesInDirectory(", "void DeleteFilesInDirectory("]:
                parts.append(function(text, signature))
            parts.append(r'''
void CheckPaths() {
    for (int length : {0, 1, 259, 260, 300, 2000}) {
        attribute_key.assign(length, L'K');
        attribute_value.assign(1, L'V');
        std::map<std::string, std::string> attributes;
        assert(ParseDirectFileSection(L"ignored", L"ignored", attributes) == (length < MAX_PATH));
        assert(attributes.size() == (length < MAX_PATH));
        if (length < MAX_PATH) assert(attributes[std::string(length, 'K')] == "V");
        attribute_key.assign(1, L'K');
        attribute_value.assign(length, L'V');
        attributes.clear();
        assert(ParseDirectFileSection(L"ignored", L"ignored", attributes) == (length < MAX_PATH));
        assert(attributes.size() == (length < MAX_PATH));
        if (length < MAX_PATH) assert(attributes["K"] == std::string(length, 'V'));
    }
    for (int length : {1, 1019, 1020, 1023, 1024, 1040, 2000}) {
        std::string path(length, 'A');
        filesystem_calls = 0;
        assert(!IsDirExist(path.c_str()));
        assert(filesystem_calls == (length < 1024));
        filesystem_calls = 0;
        assert(!IsFileExist(path.c_str()));
        assert(filesystem_calls == (length < 1024));
        filesystem_calls = 0;
        assert(GetFileSize(path.c_str()) == (DWORD)-1);
        assert(filesystem_calls == (length < 1024));
        std::vector<PathString> files;
        filesystem_calls = 0;
        EnumFilesInDirectory(files, path.c_str());
        assert(files.empty() && filesystem_calls == (length < 1020));
        filesystem_calls = 0;
        DeleteFilesInDirectory(path.c_str(), true);
        assert(filesystem_calls == (length < 1020));
        if (length >= 1024) {
            filesystem_calls = 0;
            assert(!mkDir(path, false) && !mkDir(path, true, true));
            assert(!filesystem_calls);
        }
    }
    std::vector<PathString> files;
    filesystem_calls = 0;
    EnumFilesInDirectory(files, nullptr);
    EnumFilesInDirectory(files, "");
    DeleteFilesInDirectory(nullptr, true);
    DeleteFilesInDirectory("", true);
    assert(!IsFileExist(nullptr) && !IsDirExist(nullptr));
    assert(!CreateDirectoryHierarchy(nullptr));
    assert(!filesystem_calls);
}
''')
        parts.append(r'''
void Run() {
    for (int length : {0, 1, 1023, 1024, 1040, 2000}) {
        std::wstring path(length, L'A');
        PathString converted(path.c_str());
        assert(converted == std::string(length < 1024 ? length : 0, 'A'));
    }
    assert(PathString((const WCHAR*)nullptr).empty());
    FILE* config = nullptr;
    assert(!fopen_s(&config, "patch-config.txt", "w"));
    fputs("first second\nthird", config);
    fclose(config);
    std::vector<PathString> dirs;
    GetConfigDirs(dirs, "patch-config.txt");
    assert(dirs.size() == 3 && dirs[0] == "first" && dirs[2] == "third");
    assert(!fopen_s(&config, "patch-config.txt", "w"));
    fputs(std::string(2000, 'A').c_str(), config);
    fclose(config);
    dirs.clear();
    GetConfigDirs(dirs, "patch-config.txt");
    assert(dirs.empty());
}
}
''')
    from patch_table_harness import fixtures
    parts.append(fixtures(client, source, function))
    from md5_initialization_harness import fixtures as md5_fixtures
    parts.append(md5_fixtures(client, source, function))
    from archive_member_harness import fixtures as member_fixtures
    parts.append(member_fixtures(client, source, function))
    from patch_path_harness import fixtures as path_fixtures
    parts.append(path_fixtures(client, source))
    from patch_file_read_harness import fixtures as file_fixtures
    parts.append(file_fixtures(client, source, function))
    from patch_logging_harness import fixtures as log_fixtures
    parts.append(log_fixtures(client, source, function))
    from patch_worker_harness import fixtures as worker_fixtures
    parts.append(worker_fixtures(client, source, function))
    from patch_ui_harness import fixtures as ui_fixtures
    parts.append(ui_fixtures(client, source, function))
    from patch_rectangle_harness import fixtures as rectangle_fixtures
    parts.append(rectangle_fixtures(client, source, function))
    from web_drag_harness import fixtures as web_drag_fixtures
    parts.append(web_drag_fixtures(client, source, function))
    from web_manager_harness import fixtures as web_fixtures
    parts.append(web_fixtures(client, source, function))
    from device_info_harness import fixtures as device_fixtures
    parts.append(device_fixtures(client, source, function))
    from wininet_safety_harness import fixtures as wininet_fixtures
    parts.append(wininet_fixtures(client, source, function))
    parts.append(r'''
namespace ArchiveTests {
int calls = 0;
int GetFileCountInternal(const char*) { ++calls; return 7; }
bool GetFileListInternal(const char*, char[][MAX_PATH], unsigned int, unsigned int) { ++calls; return true; }
DWORD GetFileSizeInternal(const char*, const char*) { ++calls; return 7; }
bool GetFileBufferInternal(const char*, const char*, unsigned char*, DWORD size, DWORD* read) {
    ++calls; if (read) *read = size; return true;
}
bool DeleteFileInternal(const char*, const char*) { ++calls; return true; }
bool ExtractFileInternal(const char*, const char*, const char*, int& index) { ++calls; index = 7; return true; }
bool AddFileInternal(const char*, const char*, bool, DWORD) { ++calls; return true; }
''')
    text = clean(source(client / "client/PatchTools/pakpatcher/PakPatcher.cpp"))
    for name in ["int GetPackFileCount(", "bool GetPackFileList(", "DWORD GetPackFileSize(",
                 "bool GetPackFileBuffer(", "bool GetPackFileBufferSized(", "bool DeletePackFile(", "bool ExtractPackFile(",
                 "bool AddPackFile("]:
        parts.append(function(text, name))
    parts.append(r'''
void Run() {
    char files[1][MAX_PATH];
    unsigned char buffer[1];
    for (int length : {0, 1, 1023, 1024, 1040, 2000}) {
        std::wstring path(length, L'A');
        const wchar_t* p = path.c_str();
        bool valid = length < 1024;
        calls = 0;
        assert(GetPackFileCount(p) == (valid ? 7 : 0));
        assert(GetPackFileList(p, files, 1, 1) == valid);
        assert(GetPackFileSize(p, L"x") == (valid ? 7 : 0));
        assert(GetPackFileSize(L"x", p) == (valid ? 7 : 0));
        assert(GetPackFileBuffer(p, L"x", buffer) == valid);
        assert(GetPackFileBuffer(L"x", p, buffer) == valid);
        assert(GetPackFileBufferSized(p, L"x", buffer, 1) == valid);
        assert(GetPackFileBufferSized(L"x", p, buffer, 1) == valid);
        assert(DeletePackFile(p, L"x") == valid);
        assert(DeletePackFile(L"x", p) == valid);
        assert(AddPackFile(p, L"x", false, 0) == valid);
        assert(AddPackFile(L"x", p, false, 0) == valid);
        int index = -1;
        assert(ExtractPackFile(p, L"x", L"x", index) == valid);
        assert(ExtractPackFile(L"x", p, L"x", index) == valid);
        assert(ExtractPackFile(L"x", L"x", p, index) == valid);
        assert(index == (valid ? 7 : -1));
        assert(calls == (valid ? 15 : 0));
    }
}
}
''')
    from extract_table_cell_harness import harness as extract_table_cell_harness
    parts.append(extract_table_cell_harness(client, source, function))
    from dump_defaults_harness import harness as dump_defaults_harness
    parts.append(dump_defaults_harness(client, source, function))
    from dump_range_harness import harness as dump_range_harness
    parts.append(dump_range_harness(client, source, function))
    parts.append(r'''
int main() {
    DevMessageTests::Run();
    WebManagerTests::Run();
    DeviceTests::Run();
    WinInetTests::Run();
    TestPatchUi();
    TestPatchRectangles();
    WebDragTests::Run();
    TestWorkers();
    PatchLogTests::Run();
    PatchReadTests::Run(); Compression0::Run(); Compression1::Run(); CopyVersionTests::Run();
    PatchPathTests::Run();
    ArchiveMemberTests::Run(); ArchiveTaskTests::Run();
    TestMd5();
    PatchTableTests::Run();
    Utility0::Run(); Utility0::CheckPaths();
    Utility1::Run(); Utility1::CheckPaths();
    Utility2::Run(); ArchiveTests::Run();
    ExtractTableCellTests::Run();
    DumpDefaultsTests::Run();
    DumpDefaultsTests::DumpRangeTests();
    puts("Patch-tool conversion and config-reader regressions passed");
}
''')
    return "\n".join(parts)
