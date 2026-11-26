"""Exercise shared I/O bounds checks and the real game table loader."""
import csv
import io
import json
import random
import re
import subprocess


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    parts = [source(client / "Common/utility/tab_parser.h"),
             source(client / "client/PatchTools/DownloadBuffer.h"), r'''
namespace IoTests {
struct Reader {
    std::vector<unsigned char> input;
    size_t position;
    unsigned int batch;
    Reader(size_t length, unsigned int chunk) : input(length, 0x5A), position(0), batch(chunk) {}
    unsigned int Read(void* destination, unsigned int capacity) {
        size_t count = input.size() - position;
        if (count > capacity) count = capacity;
        if (count > batch) count = batch;
        if (count) memcpy(destination, input.data() + position, count);
        position += count;
        return (unsigned int)count;
    }
};
void Downloads() {
    for (unsigned int chunk : {1u, 17u, 4096u}) {
        for (size_t length : {0u, 1u, 4095u, 4096u, 4097u, 12000u}) {
            std::vector<unsigned char> output(3, 0xCC);
            Reader exact(length, chunk);
            assert(ClientPatch::ReadDownload(exact, output, length));
            assert(output == exact.input);
            if (length) {
                Reader tooLarge(length, chunk);
                assert(!ClientPatch::ReadDownload(tooLarge, output, length - 1));
                assert(output.empty());
            }
            Reader unknownLength(length, chunk);
            assert(ClientPatch::ReadDownload(unknownLength, output, (size_t)-1));
            assert(output == unknownLength.input);
        }
    }
    struct BrokenReader {
        unsigned int Read(void*, unsigned int capacity) { return capacity + 1; }
    } broken;
    std::vector<unsigned char> output;
    assert(!ClientPatch::ReadDownload(broken, output, 10000));
}
template<class Character>
void Table(const std::basic_string<Character>& input, int columns, int rows,
           const std::vector<std::basic_string<Character> >& expected) {
    std::vector<Character> bytes(input.begin(), input.end());
    bytes.push_back(0);
    std::vector<size_t> offsets;
    int actualColumns = -1, actualRows = -1;
    assert(ClientTable::Parse(bytes.data(), input.size(), offsets, actualColumns, actualRows));
    assert(actualColumns == columns && actualRows == rows && offsets.size() == expected.size());
    for (size_t i = 0; i < offsets.size(); ++i)
        assert(std::basic_string<Character>(bytes.data() + offsets[i]) == expected[i]);
}
void Tables() {
    Table<char>("a\tb\r\n1\t2", 2, 2, {"a", "b", "1", "2"});
    Table<char>("a\tb\r\n1\t2\r\n", 2, 2, {"a", "b", "1", "2"});
    Table<char>("a\tb\r1\t2\r", 2, 2, {"a", "b", "1", "2"});
    Table<char>("a\tb\n\t", 2, 2, {"a", "b", "", ""});
    Table<char>("\"a\"\"b\"\t\"c\nd\"", 2, 1, {"a\"b", "c\nd"});
    Table<wchar_t>(L"a\tb\n1\t\"2\"", 2, 2, {L"a", L"b", L"1", L"2"});
    for (const char* invalid : {"", "\"unfinished", "a\tb\n1", "a\n1\t2", "a\tb\n\n"}) {
        std::string input(invalid);
        std::vector<char> bytes(input.begin(), input.end());
        bytes.push_back(0);
        std::vector<size_t> offsets;
        int columns = 0, rows = 0;
        assert(!ClientTable::Parse(bytes.data(), input.size(), offsets, columns, rows));
    }
}
''']
    # Differential fixtures use Python's TSV writer, including quoted tabs,
    # embedded newlines, escaped quotes, empty cells and non-final newlines.
    randomizer = random.Random(19)
    parts.append("void GeneratedTables() {")
    for case in range(64):
        rows = [["".join(randomizer.choice(' ab\t\r\n"xyz') for _ in range(randomizer.randrange(14)))
                 for _ in range(3)] for _ in range(4)]
        output = io.StringIO(newline="")
        csv.writer(output, delimiter="\t", lineterminator="\r\n").writerows(rows)
        encoded = output.getvalue()
        if case % 2:
            encoded = encoded[:-2]
        expected = ", ".join(json.dumps(cell) for row in rows for cell in row)
        parts.append("Table<char>(" + json.dumps(encoded) + ", 3, 4, {" + expected + "});")
    parts.append("}\n}\n")

    table_source = clean(source(game / "tabfile.cpp"))
    table_header = clean(source(game / "tabfile.h"))
    begin = table_header.index("class CTabFile")
    declaration = function(table_header[begin:], "class CTabFile") + ";"
    declaration = re.sub(r"typedef std::map<FString,[\s\S]*?> MAP_NAME_2_COL;",
                         "typedef std::map<std::wstring, int> MAP_NAME_2_COL;", declaration)
    parts.append(r'''
#define UTF16_BOM 0xFEFF
#define SAFE_DELETE_ARRAY(pointer) do { delete[] (pointer); (pointer) = NULL; } while (0);
namespace GameTableTests {
struct FArchive {
    std::vector<char> bytes;
    size_t position;
    bool error;
    explicit FArchive(const std::vector<char>& data) : bytes(data), position(0), error(false) {}
    INT TotalSize() const { return (INT)bytes.size(); }
    bool IsError() const { return error; }
    bool Close() { return !error; }
    void Serialize(void* destination, INT count) {
        if (count < 0 || (size_t)count > bytes.size() - position) { error = true; return; }
        if (count) memcpy(destination, bytes.data() + position, count);
        position += count;
    }
};
struct FileManager {
    std::vector<char> fixture;
    FArchive* CreateFileReader(const TCHAR*) { return new FArchive(fixture); }
} manager;
FileManager* GFileManager = &manager;
''')
    parts.append(declaration)
    for signature in ["CTabFile::CTabFile()", "CTabFile::~CTabFile()", "bool CTabFile::Init(",
                      "bool CTabFile::UnInit(", "int CTabFile::GetRowCount(", "int CTabFile::GetColCount(",
                      "const TCHAR* CTabFile::GetData(int", "const TCHAR* CTabFile::GetData(const TCHAR*",
                      "int CTabFile::GetKeyColIdx("]:
        parts.append(function(table_source, signature))
    parts.append(r'''
void Fixture(const std::wstring& text) {
    manager.fixture.resize(sizeof(unsigned short) + text.size() * sizeof(wchar_t));
    unsigned short bom = UTF16_BOM;
    memcpy(manager.fixture.data(), &bom, sizeof(bom));
    if (!text.empty()) memcpy(manager.fixture.data() + sizeof(bom), text.data(), text.size() * sizeof(wchar_t));
}
void Test() {
    for (const wchar_t* value : {L"Name\tValue\nFirst\tSecond", L"Name\tValue\r\nFirst\tSecond\r\n"}) {
        Fixture(value);
        CTabFile table;
        assert(table.Init(L"fixture"));
        assert(table.GetColCount() == 2 && table.GetRowCount() == 2);
        assert(!wcscmp(table.GetData(L"Value", 1), L"Second"));
        assert(table.GetData(-1, 1) == nullptr && table.GetData(0, -1) == nullptr);
        table.UnInit();
        assert(table.GetColCount() == 0 && table.GetRowCount() == 0);
    }
    for (const wchar_t* invalid : {L"", L"A\tA\n1\t2", L"A\tB\n1", L"A\n\"unfinished"}) {
        Fixture(invalid);
        CTabFile table;
        assert(!table.Init(L"fixture"));
        assert(table.GetColCount() == 0 && table.GetRowCount() == 0);
    }
    Fixture(L"A\nB");
    manager.fixture.push_back(0);  // Reject half of a UTF-16 code unit.
    CTabFile table;
    assert(!table.Init(L"fixture"));
}
void ActualTable(const wchar_t* path, INT columns, INT rows, UINT64 expectedHash) {
    FILE* file = _wfopen(path, L"rb");
    assert(file);
    assert(fseek(file, 0, SEEK_END) == 0);
    const long length = ftell(file);
    assert(length > 0 && fseek(file, 0, SEEK_SET) == 0);
    manager.fixture.resize(length);
    assert(fread(manager.fixture.data(), 1, length, file) == (size_t)length);
    fclose(file);
    CTabFile table;
    assert(table.Init(path));
    if (table.GetColCount() != columns || table.GetRowCount() != rows)
        fwprintf(stderr, L"Table dimensions differ: %ls (actual %d x %d, expected %d x %d)\n", path, table.GetColCount(), table.GetRowCount(), columns, rows);
    assert(table.GetColCount() == columns && table.GetRowCount() == rows);
    UINT64 hash = 14695981039346656037ull;
    for (INT row = 0; row < rows; ++row) {
        for (INT column = 0; column < columns; ++column) {
            const wchar_t* cell = table.GetData(column, row);
            assert(cell);
            for (size_t i = 0; cell[i]; ++i) {
                hash = (hash ^ (cell[i] & 255)) * 1099511628211ull;
                hash = (hash ^ ((cell[i] >> 8) & 255)) * 1099511628211ull;
            }
            hash *= 1099511628211ull;
            hash *= 1099511628211ull;
        }
    }
    assert(hash == expectedHash);
}
}
''')
    parts.append("void test_repository_tables() {")
    actual_tables = 0
    # Generated exports can sit beside source tables and use a different format.
    # Validate the repository's tracked tables without consuming those outputs,
    # even when a generated export has been staged alongside the source files.
    tracked = subprocess.check_output(
        ["git", "ls-files", "-z", "--", "Client/client/UE3/MMOGame/Config"],
        cwd=client.parent, text=True, encoding="utf-8").split("\0")
    for path in sorted(client.parent / name for name in tracked if name.endswith(".txt")):
        if path.name == "StringMap.txt.txt":
            continue
        data = path.read_bytes()
        if not data.startswith(b"\xff\xfe"):
            continue
        rows = list(csv.reader(io.StringIO(data[2:].decode("utf-16-le").rstrip("\0"), newline=""),
                               delimiter="\t"))
        if rows and len(rows[0]) <= 1:
            continue
        if not rows or len({len(row) for row in rows}) != 1:
            raise AssertionError("Irregular repository table: " + str(path))
        expected_hash = 14695981039346656037
        for row in rows:
            for cell in row:
                for byte in cell.encode("utf-16-le") + b"\0\0":
                    expected_hash = ((expected_hash ^ byte) * 1099511628211) & ((1 << 64) - 1)
        parts.append("GameTableTests::ActualTable(L" + json.dumps(path.as_posix()) +
                     ", " + str(len(rows[0])) + ", " + str(len(rows)) + ", " + str(expected_hash) + "ull);")
        actual_tables += 1
    parts.append('puts("Repository tables verified: ' + str(actual_tables) + '");\n}')
    parts.append(r'''
void test_io_safety() {
    IoTests::Downloads();
    IoTests::Tables();
    IoTests::GeneratedTables();
    GameTableTests::Test();
    test_repository_tables();
    puts("Download and table I/O regressions passed");
}
''')
    return "\n".join(parts)
