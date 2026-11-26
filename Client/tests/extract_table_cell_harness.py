"""Actual ExtractTable UTF-8 conversion and row loop, using Windows conversion."""
import re


def harness(client, source, function):
    text = source(client / 'client/DevTools/ExtractTable/ExtractTable.cpp')
    text = re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', text)
    parts = ['namespace ExtractTableCellTests {',
             function(text, 'static bool AppendUtf8TableCell('), r'''
struct CTabFile {
    std::vector<std::vector<const char*>> rows;
    size_t GetRowCount() const { return rows.size(); }
    const char* GetData(INT column, size_t row) { return rows.at(row).at(column); }
};
int ConvertRows(CTabFile& tabFile, const std::vector<INT>& requiredCols, std::wstring& writeBuf) {
''', function(text, 'for (size_t j = 0; j < tabFile.GetRowCount(); ++j)'),
             'return 0;\n}', r'''
void Run() {
    const std::string utf8[] = {"A", "\xe4\xb8\xad", "\xf0\x9f\x98\x80"};
    const std::wstring utf16[] = {L"A", L"\x4e2d", L"\xd83d\xde00"};
    for (INT kind = 0; kind < 3; ++kind) {
        for (size_t count : {size_t(0), size_t(1), size_t(2047), size_t(2048), size_t(2049), size_t(65536)}) {
            std::string input; std::wstring expected = L"prefix:";
            for (size_t i = 0; i < count; ++i) { input += utf8[kind]; expected += utf16[kind]; }
            std::wstring actual = L"prefix:";
            assert(AppendUtf8TableCell(actual, input.c_str()) && actual == expected);
            CTabFile table; table.rows = {{input.c_str(), "", nullptr}, {"last", "x", "y"}};
            actual.clear();
            assert(ConvertRows(table, {0, 1, 2}, actual) == 0);
            assert(actual == expected.substr(7) + L"\t\t \r\nlast\tx\ty\r\n");
        }
    }
    for (const char* invalid : {"\x80", "\xc0\xaf", "\xc2", "\xe4\xb8", "\xed\xa0\x80",
                                "\xf0\x9f\x98", "\xf4\x90\x80\x80", "\xff"}) {
        std::wstring output = L"keep";
        assert(!AppendUtf8TableCell(output, invalid) && output == L"keep");
        CTabFile table; table.rows = {{invalid}};
        assert(ConvertRows(table, {0}, output) == -4 && output == L"keep");
    }
    std::wstring output = L"keep";
    assert(!AppendUtf8TableCell(output, nullptr) && output == L"keep");
    assert(AppendUtf8TableCell(output, "\xef\xbb\xbf") && output == L"keep\xfeff");
    puts("ExtractTable cells preserve full 2048+ character text, convert Unicode pairs and reject malformed input without unterminated reads");
}
}
''']
    assert 'tmpBuf[2048]' not in text
    return '\n'.join(parts)
