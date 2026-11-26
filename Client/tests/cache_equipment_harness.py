"""Real character-cache lifecycle and equipment packet reader regressions."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    cache = clean(source(game / "net/message_handler.h"))
    parts = [r'''
#include <type_traits>
#pragma push_macro("check")
#undef check
#define check(value) ((void)0)
namespace CacheEquipmentTests {
using std::make_pair;
''', function(cache, "template<typename T>\nclass CharCacheData") + ";", r'''
struct Record {
    static int live, destroyed;
    int value;
    explicit Record(int v) : value(v) { ++live; }
    ~Record() { --live; ++destroyed; }
};
int Record::live = 0, Record::destroyed = 0;
static_assert(!std::is_copy_constructible<CharCacheData<Record>>::value, "Owning cache must not be shallow-copied");
static_assert(!std::is_copy_assignable<CharCacheData<Record>>::value, "Owning cache must not be shallow-assigned");
void CacheTests() {
    {
        CharCacheData<Record> cache;
        assert(!cache.GetCacheData(1));
        for (int size : {-2147483647 - 1, -1, 0}) cache.InitSize(size);
        auto* rejected = new Record(9); cache.AddNewData(9, rejected);
        assert(!cache.GetCacheData(9)); delete rejected;
        cache.InitSize(3);
        cache.AddNewData(10, new Record(10)); cache.AddNewData(0, new Record(0));
        cache.AddNewData(20, new Record(20));
        assert(cache.GetCacheData(0) && cache.GetCacheData(0)->value == 0);
        cache.Clear(10); cache.AddNewData(30, new Record(30));
        assert(cache.GetCacheData(0) && cache.GetCacheData(20) && cache.GetCacheData(30));
        auto* replacement = new Record(99); int destroyed = Record::destroyed;
        cache.AddNewData(0, replacement);
        assert(Record::destroyed == destroyed + 1 && cache.GetCacheData(0) == replacement);
        destroyed = Record::destroyed;
        cache.AddNewData(0, replacement); cache.AddNewData(0, nullptr);
        assert(Record::destroyed == destroyed && cache.GetCacheData(0)->value == 99);
        cache.Clear(0); cache.Clear(0); assert(!cache.GetCacheData(0));
        cache.Clear(); assert(!Record::live);
        cache.InitSize(5);
        for (int i = 0; i < 5; ++i) cache.AddNewData(i, new Record(i));
        cache.Clear(); cache.InitSize(1); cache.AddNewData(101, new Record(101));
        assert(cache.GetCacheData(101)->value == 101 && Record::live == 1);
        cache.AddNewData(102, new Record(102));
        assert(!cache.GetCacheData(101) && cache.GetCacheData(102)->value == 102);
        for (int capacity : {7, 1, 5, 2, 1, 3}) {
            cache.Clear(); assert(!Record::live); cache.InitSize(capacity);
            for (int i = 0; i < 50; ++i) {
                cache.AddNewData(i, new Record(i)); assert(cache.GetCacheData(i)->value == i);
                assert(Record::live <= capacity);
            }
            for (int i = 50 - capacity; i < 50; ++i) assert(cache.GetCacheData(i)->value == i);
        }
    }
    assert(!Record::live);
}
''']
    item = clean(source(client / "Common/define/item_def.h"))
    parts.append(function(item, "enum NORMAL_EQUIP_SLOT") + ";")
    parts.append(re.search(r"static const UINT8 MAX_DECOMPOSE_ITEM_COUNT\s*=[^;]+;", item).group())
    wire = clean(source(client / "Common/message/g2c_message.h"))
    parsers = []
    parts.append("#pragma pack(push, 1)")
    for file in ["EquipSlot", "EquipRemake", "EquipDecompose"]:
        cpp = clean(source(game / ("net/message_handler_" + file + ".cpp")))
        for name in re.findall(r"void CMessageHandler::Parse_(\w+)\(const char\* buf, UINT32 size\)", cpp):
            body = function(cpp, "void CMessageHandler::Parse_" + name + "(")
            kind = re.search(r"\b(\w+)\* msg\s*=", body).group(1)
            parts.append(function(wire, "struct " + kind + " :") + ";")
            parsers.append((name, kind, body))
    if len(parsers) != 7:
        raise ValueError("Equipment parser coverage changed")
    parts.append("#pragma pack(pop)")
    parts.append(function(clean(source(game / "EquipDecomposeManager.h")), "struct DecomposeResult\n") + ";")
    parts.append(r'''
enum { GameEvent_BookRemakeResult, GameEvent_MedicineRemakeResult, ECVT_Quest,
       GQT_Decompose, GQT_Compose, EGUET_DecompEquip, EGUET_CompEquip };
int events = 0;
struct MMOGameEvent_RemakeResult { MMOGameEvent_RemakeResult(int, int, int) {} };
struct MMOGameEvent_Guide { explicit MMOGameEvent_Guide(int) {} };
struct MMOGameEvent_EquipMaker {
    enum Kind { EEDT_DecomposeSuccess, EEDT_ComposeSuccess };
    int IntRst = 0;
    explicit MMOGameEvent_EquipMaker(Kind) {}
};
struct MMOGameEvent_EquipSlotEnhanceResult { MMOGameEvent_EquipSlotEnhanceResult(int, int, int) {} };
namespace MMOUI { template<class T> void PostGameEvent(T*) { ++events; } }
struct MMOQuestManager {
    static MMOQuestManager& getSingleton() { static MMOQuestManager value; return value; }
    void UpdateClientVariable(int, int, int) { ++events; }
};
template<class T> struct TArray {
    std::vector<T> data;
    void Empty() { data.clear(); }
    void AddItem(const T& value) { data.push_back(value); }
};
struct EquipSlotManager {
    UINT16 levels[NES_TOTAL] = {};
    static EquipSlotManager& getSingleton() { static EquipSlotManager value; return value; }
    void SyncEquipSlotLevel(UINT16* incoming) { memcpy(levels, incoming, sizeof(levels)); ++events; }
};
struct EquipDecomposeManager {
    TArray<DecomposeResult> results;
    static EquipDecomposeManager& getSingleton() { static EquipDecomposeManager value; return value; }
    TArray<DecomposeResult>& GetDecomposeResult() { return results; }
};
struct MMOEquipRemakeManager {
    static MMOEquipRemakeManager& getSingleton() { static MMOEquipRemakeManager value; return value; }
    void OnRemakeResultSuccess(int) { ++events; }
    void OnRemakeResultFailed() { ++events; }
};
struct CMessageHandler {
''')
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name, _, _ in parsers)
    parts.append("};")
    parts.extend(body for _, _, body in parsers)
    parts.append(r'''
using Parser = void (CMessageHandler::*)(const char*, UINT32);
template<class T> void HeaderBounds(CMessageHandler& handler, Parser parser) {
    int before = events; (handler.*parser)(nullptr, 0); (handler.*parser)(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(T); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1); memset(bytes.get(), 0xff, size);
        (handler.*parser)(bytes.get(), size); assert(events == before);
    }
    T message = {}; (handler.*parser)((char*)&message, sizeof(message)); assert(events > before);
}
void Run() {
    CacheTests();
    CMessageHandler handler;
''')
    parts.extend("HeaderBounds<" + kind + ">(handler, &CMessageHandler::Parse_" + name + ");" for name, kind, _ in parsers)
    parts.append(r'''
    G2C_DECOMPOSE decompose = {}; decompose.byResult = 1;
    decompose.nItemID[MAX_DECOMPOSE_ITEM_COUNT - 1] = 123;
    decompose.nItemCount[MAX_DECOMPOSE_ITEM_COUNT - 1] = 456;
    handler.Parse_g2c_decompose((char*)&decompose, sizeof(decompose));
    const auto& results = EquipDecomposeManager::getSingleton().results.data;
    assert(results.size() == 1 && results[0].itemId == 123 && results[0].cnt == 456);
    G2C_SYNC_REFINE refine = {}; refine.awRefineLevel[NES_TOTAL - 1] = 65535;
    handler.Parse_g2c_sync_refine((char*)&refine, sizeof(refine));
    assert(EquipSlotManager::getSingleton().levels[NES_TOTAL - 1] == 65535);
    puts("Character-cache reset, ownership, ring bounds and seven equipment packet readers passed");
}
}
#pragma pop_macro("check")
''')
    return "\n".join(parts)
