"""Actual handbook table record, reader and shared READ_* macros."""
import re


def harness(client, source, function):
    root = client / "Common/fusion"

    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(root / path))

    needed = {"READ_START", "READ_INT", "READ_STR", "READ_INT_EXPR", "READ_END"}
    macros = {}
    lines = clean("inc/read_data_helper.h").splitlines()
    for index, line in enumerate(lines):
        match = re.match(r"#define (\w+)\b", line)
        if not match or match.group(1) not in needed:
            continue
        definition = [line]
        while definition[-1].rstrip().endswith("\\"):
            index += 1
            definition.append(lines[index])
        macros[match.group(1)] = "\n".join(definition)
    assert macros.keys() == needed
    parts = []
    for name, definition in macros.items():
        parts.append('#pragma push_macro("' + name + '")\n#undef ' + name + "\n" + definition)
    header = clean("inc/achievement_data.h")
    parts.append("namespace HandbookTableTests {\n")
    parts.append("enum { MAX_HANDBOOKITEM_COUNT = " + re.search(r"#define MAX_HANDBOOKITEM_COUNT\s+([^\n]+)", header).group(1) + " };\n")
    parts.append(r'''
struct CTabFile {
    std::map<std::string, std::string> fields;
    bool unterminated = false;
    INT ReadData(const char* key, INT, INT32& value) {
        auto found = fields.find(key); if (found == fields.end()) return FALSE;
        value = atoi(found->second.c_str()); return TRUE;
    }
    INT ReadData(const char* key, INT, char* out, size_t capacity) {
        auto found = fields.find(key); if (found == fields.end()) return FALSE;
        size_t count = std::min(found->second.size(), capacity - 1);
        memcpy(out, found->second.data(), count); out[count] = 0;
        if (unterminated && std::string(key) == "ID") { memset(out, ' ', capacity); out[0] = '1'; }
        return TRUE;
    }
};
struct Script {
    INT CallFunction(const char*, const char*, const char* value, INT32* out) { *out = atoi(value); return TRUE; }
} script;
Script* g_pReadDataScript = &script;
''')
    parts.append(function(header, "struct HANDBOOK_DATA\n") + ";")
    parts.append(function(clean("src/data/achievement_data.cpp"), "BOOL HANDBOOK_DATA::ReadLine("))
    parts.append(r'''
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        for (INT count = 0; count <= MAX_HANDBOOKITEM_COUNT + 1; ++count) {
            for (bool trailing_comma : {false, true}) {
                CTabFile table;
                table.fields = {{"HandbookID", "123"}, {"HandbookIndex", "4"}, {"HandbookType", "5"}, {"ID", ""}};
                for (INT i = 0; i < count; ++i) {
                    if (i) table.fields["ID"] += ',';
                    table.fields["ID"] += std::to_string(i + 1);
                }
                if (count && trailing_comma) table.fields["ID"] += ',';
                struct Guarded { UINT32 before; HANDBOOK_DATA data; UINT32 after; } value;
                memset(&value, pattern, sizeof(value));
                INT32 id = -1; const char* name = nullptr;
                assert(!!HANDBOOK_DATA::ReadLine(id, name, &value.data, &table, 1) == (count <= MAX_HANDBOOKITEM_COUNT));
                assert(id == 123 && value.data.nHandbookID == 123 && value.data.nIndex == 4 && value.data.nHandbookType == 5);
                for (INT i = 0; i < MAX_HANDBOOKITEM_COUNT; ++i)
                    assert(value.data.nID[i] == (i < count ? i + 1 : 0));
                UINT32 guard = pattern == 0xa5 ? 0xa5a5a5a5 : 0x5a5a5a5a;
                assert(value.before == guard && value.after == guard);
            }
        }
    }
    for (size_t length : {size_t(1), size_t(1022), size_t(1023), size_t(1024), size_t(65536)}) {
        CTabFile table;
        table.fields = {{"HandbookID", "123"}, {"HandbookIndex", "4"}, {"HandbookType", "5"}, {"ID", "1" + std::string(length - 1, ' ')}};
        // Use an allocation ending at szValue's last byte: ASan catches any
        // read beyond its terminator rather than hiding it in adjacent storage.
        HANDBOOK_DATA* data = new HANDBOOK_DATA;
        memset(data, 0xa5, sizeof(*data));
        INT32 id; const char* name = nullptr;
        assert(HANDBOOK_DATA::ReadLine(id, name, data, &table, 1) && data->nID[0] == 1);
        assert(data->szValue[sizeof(data->szValue) - 1] == 0);
        for (INT i = 1; i < MAX_HANDBOOKITEM_COUNT; ++i) assert(!data->nID[i]);
        table.unterminated = true;
        assert(HANDBOOK_DATA::ReadLine(id, name, data, &table, 1) && data->nID[0] == 1);
        delete data;
    }
    for (const char* missing : {"HandbookID", "HandbookIndex", "HandbookType", "ID"}) {
        CTabFile table;
        table.fields = {{"HandbookID", "123"}, {"HandbookIndex", "4"}, {"HandbookType", "5"}, {"ID", "1"}};
        table.fields.erase(missing);
        HANDBOOK_DATA data; memset(&data, 0x5a, sizeof(data));
        INT32 id = -1; const char* name = nullptr;
        assert(!HANDBOOK_DATA::ReadLine(id, name, &data, &table, 1));
        assert(id == (std::string(missing) == "HandbookID" ? 0 : 123));
        for (INT value : data.nID) assert(!value);
    }
    puts("Handbook table reader initializes unused IDs, handles full/unterminated text, accepts 32 entries and rejects excess before writing");
}
}
''')
    for name in reversed(macros):
        parts.append('#pragma pop_macro("' + name + '")')
    return "\n".join(parts)
