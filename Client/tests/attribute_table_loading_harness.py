"""Actual attribute and faction loaders, including UE3's unconstructed Add slots."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', text)

    header = clean(source(game / 'MMOFacade.h'))
    cpp = clean(source(game / 'MMOFacade.cpp'))
    force_count = re.search(r'#define MAX_NPC_FORCE_ID\s+\((\d+)\)',
                            source(client / 'Common/define/npc_def.h'))[1]
    parts = [r'''
#pragma push_macro("TEXT")
#pragma push_macro("SCOPE_LOG_TIME")
#pragma push_macro("M_AddDataError")
#undef TEXT
#undef SCOPE_LOG_TIME
#undef M_AddDataError
#define TEXT(x) x
#define SCOPE_LOG_TIME(...)
#define M_AddDataError(...) do { ++errors; } while (0)
namespace AttributeTableLoadingTests {
using FString = AttributeRecordTests::FString;
INT errors = 0, destroyed = 0;
INT appAtoi(const char* text) { return atoi(text); }
''', 'enum { MAX_NPC_FORCE_ID = ' + force_count + ' };',
             function(header, 'struct Attribute2\n') + ';', r'''
template<class T> struct TArray {
    std::vector<T> values;
    INT Num() const { return (INT)values.size(); }
    T& operator()(INT index) { assert(index >= 0 && index < Num()); return values[index]; }
    void Add(INT count) {
        assert(count >= 0); size_t old = values.size(); values.resize(old + count);
        if (count) memset(values.data() + old, 0xcd, count * sizeof(T));
    }
    void AddItem(const T& value) { values.push_back(value); }
};
struct CTabFile {
    std::vector<std::vector<std::string>> rows;
    INT columns = 5;
    ~CTabFile() { ++destroyed; }
    INT GetRowCount() const { return (INT)rows.size(); }
    INT GetColCount() const { return columns; }
    const char* GetData(INT column, INT row) {
        assert(row >= 0 && row < GetRowCount() && column >= 0 && column < columns);
        return rows[row][column].c_str();
    }
};
struct MMOFacade {
    TArray<Attribute2> vecAttribute2;
    INT ForceRelative[MAX_NPC_FORCE_ID][MAX_NPC_FORCE_ID];
    CTabFile* input = nullptr;
    bool failure = false;
    static MMOFacade& getSingleton() { static MMOFacade singleton; return singleton; }
    UBOOL ReadConfigString(CTabFile*& table, const char*, UBOOL) {
        if (failure) return FALSE;
        table = input; input = nullptr; return TRUE;
    }
    INT FindClientEnumValue(const char* text, const char*) { return atoi(text); }
    UBOOL ReadAttribute2Config();
    UBOOL ReadForceRelative();
};
''']
    for signature in ['UBOOL MMOFacade::ReadAttribute2Config(', 'UBOOL MMOFacade::ReadForceRelative(']:
        parts.append(function(cpp, signature))
    parts.append(r'''
CTabFile* Table(INT rows, INT columns = 5) {
    auto* table = new CTabFile; table->columns = columns;
    table->rows.resize(rows, std::vector<std::string>(columns));
    for (INT row = 0; row < rows; ++row) for (INT col = 0; col < columns; ++col)
        table->rows[row][col] = std::to_string(row * 10 + col);
    return table;
}
void Run() {
    alignas(Attribute2) BYTE storage[sizeof(Attribute2)]; memset(storage, 0xff, sizeof(storage));
    Attribute2* zero = new(storage) Attribute2;
    for (BYTE value : storage) assert(value == 0);
    auto& facade = MMOFacade::getSingleton();
    for (INT rows : {0, 1, 2, 3, 10, 2}) {
        facade.input = Table(rows);
        INT previous = destroyed;
        assert(facade.ReadAttribute2Config() && destroyed == previous + 1);
        assert(facade.vecAttribute2.Num() == (rows > 2 ? rows - 2 : 0));
        for (INT i = 0; i < facade.vecAttribute2.Num(); ++i) {
            const auto& data = facade.vecAttribute2(i); INT row = i + 2;
            assert(data.ClassID == row * 10 && data.AttrName == row * 10 + 1);
            assert(data.ConstParam == row * 10 + 2 && data.catValue == row * 10 + 3 && data.catCombatParam == row * 10 + 4);
            assert(!data.catStrength && !data.catAgility && !data.catIntellect && !data.catStamina && !data.catSpirit);
        }
    }
    facade.input = Table(4); assert(facade.ReadAttribute2Config());
    auto saved = facade.vecAttribute2.values;
    facade.failure = true; assert(!facade.ReadAttribute2Config()); facade.failure = false;
    assert(facade.vecAttribute2.Num() == (INT)saved.size());
    assert(memcmp(saved.data(), facade.vecAttribute2.values.data(), saved.size() * sizeof(Attribute2)) == 0);
    for (INT rows : {0, 1, 2, 3, MAX_NPC_FORCE_ID + 3}) {
        for (INT columns : {1, 5, MAX_NPC_FORCE_ID + 2}) {
            facade.input = Table(rows, columns);
            assert(facade.ReadForceRelative());
            assert(facade.vecAttribute2.Num() == (INT)saved.size());
            assert(memcmp(saved.data(), facade.vecAttribute2.values.data(), saved.size() * sizeof(Attribute2)) == 0);
            for (INT row = 0; row < MAX_NPC_FORCE_ID; ++row) for (INT col = 0; col < MAX_NPC_FORCE_ID; ++col) {
                INT expected = row + 2 < rows && col + 1 < columns ? (row + 2) * 10 + col + 1 : 0;
                assert(facade.ForceRelative[row][col] == expected);
            }
        }
    }
    puts("Attribute loading constructs every record and replaces reloads; faction tables never append uninitialized attribute entries");
}
}
#pragma pop_macro("M_AddDataError")
#pragma pop_macro("SCOPE_LOG_TIME")
#pragma pop_macro("TEXT")
''')
    return '\n'.join(parts)
