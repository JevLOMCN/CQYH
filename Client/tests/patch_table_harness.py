"""Malformed patch-table regressions run against all three production parsers."""


def fixtures(client, source, function):
    parts = [r'''
namespace PatchTableTests {
using Utility2::PathString;
using Utility2::DeleteFileMap;
using Utility2::RenameFileMap;
template<class Table, class PathString>
void CheckCorruption(Table& table, const PathString& encoded) {
    Table saved = table;
    for (size_t size = 0; size < encoded.size(); ++size) {
        PathString truncated(encoded.c_str(), (UINT)size);
        assert(!table.FromString(truncated) && table == saved);
    }
    for (size_t offset = 0; offset < 11; ++offset) {
        PathString corrupt = encoded;
        corrupt[offset] ^= 0x5a;
        assert(!table.FromString(corrupt) && table == saved);
    }
    PathString trailing = encoded;
    trailing += 'x';
    assert(!table.FromString(trailing) && table == saved);
    // Corrupt every possible four-byte length/count field, including unaligned ones.
    for (size_t offset = 11; offset + sizeof(int) <= encoded.size(); ++offset) {
        for (int length : {-1, 0x7fffffff, (int)encoded.size() + 1}) {
            PathString corrupt = encoded;
            memcpy(&corrupt[offset], &length, sizeof(length));
            Table candidate = saved;
            bool valid = candidate.FromString(corrupt);
            if (!valid) assert(candidate == saved);
        }
    }
}
template<class D, class R>
void CheckTables() {
    using PathString = typename D::key_type;
    D deletions;
    deletions["00000.pak"].push_back("first");
    deletions["00000.pak"].push_back("second");
    deletions["00001.pak"].push_back("last");
    PathString encoded;
    assert(deletions.ToString(encoded));
    D decoded;
    assert(decoded.FromString(encoded) && decoded == deletions);
    CheckCorruption(decoded, encoded);

    // Explicitly target all length/count fields in the deletion table.
    for (size_t offset : {11u, 15u, 28u, 32u, 41u, 51u, 64u, 68u}) {
        for (int value : {-1, 0x7fffffff}) {
            PathString corrupt = encoded;
            memcpy(&corrupt[offset], &value, sizeof(value));
            assert(!decoded.FromString(corrupt) && decoded == deletions);
        }
    }
    R renames;
    renames["00000.pak"].push_back(std::make_pair("old", "new"));
    renames["00001.pak"].push_back(std::make_pair("last-old", "last-new"));
    encoded.clear();
    assert(renames.ToString(encoded));
    R renamed;
    assert(renamed.FromString(encoded) && renamed == renames);
    CheckCorruption(renamed, encoded);
    for (size_t offset : {11u, 15u, 28u, 32u, 39u, 46u, 59u, 63u, 75u}) {
        for (int value : {-1, 0x7fffffff}) {
            PathString corrupt = encoded;
            memcpy(&corrupt[offset], &value, sizeof(value));
            assert(!renamed.FromString(corrupt) && renamed == renames);
        }
    }
    // Valid zero-entry tables replace the previous contents safely.
    PathString empty("MLED1.0.0.0");
    empty.AppendInt(0);
    assert(decoded.FromString(empty) && decoded.empty());
    empty.assign("MNER1.0.0.0");
    empty.AppendInt(0);
    assert(renamed.FromString(empty) && renamed.empty());
}
PathString archive;
DWORD reported_high = 0, reported_size = 0, bytes_read = 0;
bool read_ok = true;
int reads = 0;
DWORD SFileGetFileSize(HANDLE, DWORD* high) {
    *high = reported_high; return reported_size;
}
BOOL SFileReadFile(HANDLE, void* buffer, DWORD size, DWORD* read, void*) {
    ++reads;
    assert(size <= archive.size());
    assert(bytes_read <= size);
    memcpy(buffer, archive.c_str(), bytes_read);
    *read = bytes_read;
    return read_ok;
}
enum { SFileMpqNumberOfFiles, SFileMpqMaxFileCount };
DWORD file_count = 0, maximum_count = 0, requested_capacity = 0;
bool count_ok = true, maximum_ok = true, resize_ok = true;
int resizes = 0;
BOOL SFileGetFileInfo(HANDLE, int type, DWORD* result, size_t, void*) {
    if (type == SFileMpqNumberOfFiles) {
        if (!count_ok) return FALSE;
        *result = file_count;
    } else {
        if (!maximum_ok) return FALSE;
        *result = maximum_count;
    }
    return TRUE;
}
BOOL SFileSetMaxFileCount(HANDLE, DWORD size) {
    ++resizes; requested_capacity = size; return resize_ok;
}
''']
    text = source(client / "client/PatchTools/pakpatcher/PakPatcher.cpp")
    parts.append(function(text, "bool EnsureArchiveCapacity("))
    parts.append(function(text, "template<class Table>\nbool LoadPatchTable("))
    # Exercise the actual output guards without opening an archive or touching game files.
    parts.append(r'''
int created = 0, written = 0;
bool create_ok = true;
bool metadata_mode = false;
std::vector<PathString> written_tables;
enum { PAK_CREATE_FLAG = 0, PAK_COMPRESSION = 0 };
bool SFileCreateFile(HANDLE, const char*, int, DWORD, DWORD, int, HANDLE* file) {
    ++created;
    if (metadata_mode) {
        *file = reinterpret_cast<HANDLE>(written_tables.size());
        written_tables.push_back(PathString());
    }
    return create_ok;
}
void SFileWriteFile(HANDLE file, const char* data, DWORD count, int) {
    ++written;
    if (metadata_mode) written_tables[(size_t)file].assign(data, count);
    else assert(count == bytes_read);
}
void SFileFinishFile(HANDLE) {}
bool SFileCreateArchive(const char*, int, int, HANDLE*) { return true; }
void SFileCloseArchive(HANDLE) {}
const char* PATCH_DEL_TABLE_FILE = "_DEL_.dat";
const char* PATCH_RENAME_TABLE_FILE = "_REN_.dat";
void CheckMetadataWriter() {
    DeleteFileMap DeleteFiles;
    RenameFileMap RenameFiles;
    DeleteFiles["00000.pak"].push_back("delete");
    RenameFiles["00000.pak"].push_back(std::make_pair("old", "new"));
    HANDLE hDelRenArchive = nullptr;
    const char* DestDelRenPak = "unused";
    metadata_mode = true;
''')
    parts.append(function(text, "if (SFileCreateArchive(DestDelRenPak, 0, 8, &hDelRenArchive))"))
    parts.append(r'''
    metadata_mode = false;
    assert(written_tables.size() == 2);
    DeleteFileMap deleted;
    RenameFileMap renamed;
    assert(deleted.FromString(written_tables[0]) && deleted == DeleteFiles);
    assert(renamed.FromString(written_tables[1]) && renamed == RenameFiles);
}
bool CopyPatchPayload() {
    HANDLE hPatchFile = nullptr, hDestArchive = nullptr, hDestFile = nullptr;
    struct { const char* cFileName = "target"; DWORD lcLocale = 0; } fd;
    DWORD FileSize = reported_size, dwRead = 0;
    std::vector<char> bytes(FileSize, '?');
    char* Buffer = bytes.data();
    bool bNeedCompact = false;
''')
    parts.append(function(text, "if (SFileReadFile(hPatchFile, Buffer, FileSize, &dwRead, NULL)"))
    parts.append(r'''
    return bNeedCompact;
}
bool ExtractPayload() {
    HANDLE hFile = nullptr;
    DWORD FileSize = reported_size, dwRead = 0;
    std::vector<char> bytes(FileSize, '?');
    char* Buffer = bytes.data();
    const char* strLocalPath = "extract-result.bin";
    bool bSucc = false;
''')
    parts.append(function(text, "if (SFileReadFile(hFile, Buffer, FileSize, &dwRead, NULL)"))
    parts.append(r'''
    return bSucc;
}
''')
    parts.append(r'''
void Run() {
    CheckMetadataWriter();
    CheckTables<Utility0::DeleteFileMap, Utility0::RenameFileMap>();
    CheckTables<Utility1::DeleteFileMap, Utility1::RenameFileMap>();
    CheckTables<DeleteFileMap, RenameFileMap>();
    file_count = 10; maximum_count = 20;
    for (bool first : {false, true}) for (bool second : {false, true}) {
        count_ok = first; maximum_ok = second; resizes = 0;
        assert(EnsureArchiveCapacity(nullptr, 1) == (first && second));
        assert(!resizes);
    }
    count_ok = maximum_ok = true;
    for (bool resize : {false, true}) {
        resize_ok = resize; resizes = 0;
        assert(EnsureArchiveCapacity(nullptr, 10) == resize);
        assert(resizes == 1 && requested_capacity == 20);
    }
    resizes = 0;
    assert(!EnsureArchiveCapacity(nullptr, (DWORD)-1) && !resizes);
    file_count = (DWORD)-2; maximum_count = (DWORD)-1;
    assert(EnsureArchiveCapacity(nullptr, 0) && !resizes);
    assert(!EnsureArchiveCapacity(nullptr, 2) && !resizes);

    archive.assign("payload");
    reported_size = (DWORD)archive.size();
    for (DWORD actual = 0; actual <= reported_size; ++actual) {
        bytes_read = actual;
        for (bool success : {false, true}) {
            read_ok = success; created = written = 0;
            bool valid = success && actual == reported_size;
            assert(CopyPatchPayload() == valid);
            assert(created == valid && written == valid);
            assert(ExtractPayload() == valid);
        }
    }
    archive.clear(); reported_size = bytes_read = 0; read_ok = true;
    assert(ExtractPayload());
    DeleteFileMap table;
    table["00000.pak"].push_back("target");
    assert(table.ToString(archive));
    const DeleteFileMap original = table;
    char local[64];
    reported_size = (DWORD)archive.size();
    for (DWORD actual = 0; actual <= reported_size; ++actual) {
        bytes_read = actual;
        for (bool success : {false, true}) {
            read_ok = success;
            assert(LoadPatchTable(nullptr, local, sizeof(local), table) ==
                   (success && actual == reported_size));
            assert(table == original);
        }
    }
    reads = 0;
    reported_high = 1;
    assert(!LoadPatchTable(nullptr, local, sizeof(local), table) && !reads);
    reported_high = 0;
    reported_size = (DWORD)-1;
    assert(!LoadPatchTable(nullptr, local, sizeof(local), table) && !reads);
    // A valid table larger than the old fixed 16 MB buffer must load intact.
    table.clear();
    table["00000.pak"].push_back(std::string(16 * 1024 * 1024 + 1, 'x'));
    archive.clear();
    assert(table.ToString(archive));
    reported_size = bytes_read = (DWORD)archive.size();
    read_ok = true;
    DeleteFileMap large;
    assert(LoadPatchTable(nullptr, local, sizeof(local), large) && large == table);
    puts("Patch-table length, count, truncation and archive-size regressions passed");
}
}
''')
    return "\n".join(parts)
