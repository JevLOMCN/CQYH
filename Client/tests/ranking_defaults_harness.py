"""Production ranking clock failures, record defaults and password packet tail."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "RankListManager.h"))
    cpp = clean(source(game / "RankListManager.cpp"))
    ranks = clean(source(client / "Common/define/ranklist_def.h"))
    parts = [r'''
#pragma push_macro("SAFE_DELETE")
#undef SAFE_DELETE
#define SAFE_DELETE(p) do { delete (p); (p) = nullptr; } while (0)
namespace RankingDefaultsTests {
template<class T> struct SingletonMeyers {
    static T& getSingleton() { static T value; return value; }
};
struct Config { bool GetInt(const char*, const char*, int&, const char*) { return false; } } config;
Config* GConfig = &config;
const char* GDefaultGameIni = "";
int notifications = 0;
int clock_calls = 0, fail_conversion = 0;
bool fail_clock = false;
tm server_time = {}, today_time = {}, shared_time = {};
__time32_t _time32(__time32_t* output) { if (fail_clock) return -1; *output = 1000000; return *output; }
tm* _localtime32(const __time32_t*) {
    ++clock_calls;
    if (clock_calls == fail_conversion) return nullptr;
    shared_time = clock_calls == 1 ? server_time : today_time; return &shared_time;
}
''']
    value = re.search(r"#define MAX_ROLE_LEVEL\s+([^\n]+)", clean(source(client / "Common/define/role_def.h"))).group(1)
    parts.append("enum { MAX_ROLE_LEVEL = " + value + " };")
    parts.extend(re.findall(r"#define MAXRANKLISTCOUNT_\w+\s+[^\n]+", header))
    parts.append(function(ranks, "enum RANK_LIST") + ";")
    value = re.search(r"#define\s+MAX_RANK_NODE_NUM_PER_PAGE\s+([^\n]+)", ranks).group(1)
    parts.append("enum { MAX_RANK_NODE_NUM_PER_PAGE = " + value + " };")
    for name, alias in [("tagRankListData", "RankListData"), ("tagPageInfo", "PageInfo")]:
        match = re.search(r"typedef struct " + name + r"\s*\{", header).group()
        parts.append(function(header, match) + alias + ";")
    parts.append(r'''
using RankVec = std::vector<RankListData*>;
using PageMap = std::map<RANK_LIST, PageInfo*>;
''')
    parts.append(function(header, "class RankListManager\n").replace("private:", "public:") + ";")
    parts.append("void RankListManager::NotifyUpdateRankList() { ++notifications; }")
    for signature in ["RankListManager::RankListManager()", "RankListManager::~RankListManager()",
                      "void RankListManager::Clear()", "void RankListManager::AddData(",
                      "void RankListManager::SetType(", "void RankListManager::SetPageTotal(",
                      "INT RankListManager::GetPageTotal() const", "INT RankListManager::getMaxRankListCount()",
                      "void RankListManager::SetShowRank("]:
        parts.append(function(cpp, signature))
    parts.append("#pragma pack(push, 1)")
    parts.append(function(clean(source(client / "Common/message/m2c_message.h")), "struct M2C_QUERY_RANKLIST :") + ";")
    strings = clean(source(client / "Common/define/string_def.h"))
    value = re.search(r"#define OA_DYNAMIC_PASSWORD_LEN\s+([^\n]+)", strings).group(1)
    parts.append("enum { OA_DYNAMIC_PASSWORD_LEN = " + value + " };")
    parts.append(function(clean(source(client / "Common/message/c2m_message.h")), "struct C2M_DYNAMIC_AUTH :") + ";")
    parts.append("#pragma pack(pop)")
    parts.append(r'''
using FString = NetworkTests::FString;
enum { c2m_dynamic_auth = 60 };
struct CMessageHandler {
    std::vector<char> sent;
    void Parse_m2c_query_ranklist(const char*, UINT32);
    void Send_DynamicPassword(const FString&);
    void _Send(const void* bytes, size_t size) { sent.assign((const char*)bytes, (const char*)bytes + size); }
};
''')
    parts.append(function(clean(source(game / "net/message_handler_Ranklist.cpp")), "void CMessageHandler::Parse_m2c_query_ranklist("))
    parts.append(function(clean(source(game / "net/message_handler_DynamicPwd.cpp")), "void CMessageHandler::Send_DynamicPassword("))
    remake = clean(source(game / "MMOEquipRemakeMgr.h"))
    for name in ["GambleTemplateDataKey", "GambleTemplateDataValue"]:
        parts.append(function(remake, "struct " + name + "\n") + ";")
    parts.append(r'''
using ITEM_SUBTYPE = int;
struct GambleTemplateIndexDataProcessor { static int ItemSubTypeToGambleType(ITEM_SUBTYPE value) { return value; } };
struct GambleTemplateDataIndexManager {
    GambleTemplateDataValue* found = nullptr;
    static GambleTemplateDataIndexManager& getSingleton() { static GambleTemplateDataIndexManager value; return value; }
    const GambleTemplateDataValue* FindData(UINT) { return found; }
};
struct MMOEquipRemakeManager { UBOOL GetMaterialIDAndCost(UINT8, INT, INT&, INT&); };
''')
    parts.append(function(clean(source(game / "MMOEquipRemakeMgr.cpp")), "UBOOL MMOEquipRemakeManager::GetMaterialIDAndCost("))
    parts.append(r'''
void AssertZero(const tm& value) {
    assert(!value.tm_sec && !value.tm_min && !value.tm_hour && !value.tm_mday && !value.tm_mon);
    assert(!value.tm_year && !value.tm_wday && !value.tm_yday && !value.tm_isdst);
}
void Run() {
    TipsStallTests::Poisoned<RankListData>([](auto& v) { assert(!v.Id && !v.Value && !v.LastRank); });
    TipsStallTests::Poisoned<PageInfo>([](auto& v) { assert(!v.curPage && !v.totalPage); });
    TipsStallTests::Poisoned<GambleTemplateDataValue>([](auto& v) {
        assert(!v.nGambleType && !v.nGambleLevel && !v.nCostItemID && !v.nCostItemCount);
    });
    TipsStallTests::Poisoned<RankListManager>([](auto& manager) { AssertZero(manager.GetShowRankTime()); });
    auto& manager = RankListManager::getSingleton();
    server_time.tm_year = today_time.tm_year = 126;
    server_time.tm_mon = today_time.tm_mon = 10;
    server_time.tm_mday = today_time.tm_mday = 2;
    today_time.tm_hour = 12;
    clock_calls = 0; manager.SetShowRank(1000000);
    assert(!manager.IsShowRank() && manager.GetShowRankTime().tm_hour == 0);
    server_time.tm_hour = 1; clock_calls = 0; manager.SetShowRank(1000000); assert(manager.IsShowRank());
    fail_conversion = 1; clock_calls = 0; manager.SetShowRank(1000000);
    assert(manager.IsShowRank()); AssertZero(manager.GetShowRankTime());
    fail_conversion = 2; clock_calls = 0; manager.SetShowRank(1000000);
    assert(manager.IsShowRank() && manager.GetShowRankTime().tm_hour == 1);
    fail_conversion = 0; fail_clock = true; clock_calls = 0; manager.SetShowRank(1000000); assert(manager.IsShowRank());
    fail_clock = false;
    for (UINT32 stamp : {0x80000000u, 0xffffffffu}) {
        clock_calls = 0; manager.SetShowRank(stamp); assert(manager.IsShowRank() && !clock_calls);
        AssertZero(manager.GetShowRankTime());
    }
    CMessageHandler handler;
    handler.Parse_m2c_query_ranklist(nullptr, 0); handler.Parse_m2c_query_ranklist(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(M2C_QUERY_RANKLIST); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1); memset(bytes.get(), 0xff, size);
        handler.Parse_m2c_query_ranklist(bytes.get(), size); assert(!notifications);
    }
    M2C_QUERY_RANKLIST rank = {}; rank.byRankListType = rlLevel; rank.nTotalPage = 100;
    rank.llKey[MAX_RANK_NODE_NUM_PER_PAGE - 1] = 123;
    rank.llValue[MAX_RANK_NODE_NUM_PER_PAGE - 1] = 456;
    handler.Parse_m2c_query_ranklist((char*)&rank, sizeof(rank));
    assert(notifications == 1 && manager._RankVec.size() == 1 && manager._RankVec[0]->Id == 123);
    assert(manager.GetPageTotal() == MAXRANKLISTCOUNT_PERSON);
    for (int length : {0, 1, 5, 6, 7, 1000}) {
        std::string password(length, 'P'); handler.Send_DynamicPassword(FString(password.c_str()));
        auto* packet = (C2M_DYNAMIC_AUTH*)handler.sent.data();
        size_t copied = std::min(password.size(), size_t(OA_DYNAMIC_PASSWORD_LEN - 1));
        assert(handler.sent.size() == sizeof(*packet) && !memcmp(packet->szDynamicPassword, password.c_str(), copied));
        for (size_t i = copied; i < OA_DYNAMIC_PASSWORD_LEN; ++i) assert(!packet->szDynamicPassword[i]);
    }
    MMOEquipRemakeManager remake;
    INT item = 123, cost = 456;
    assert(!remake.GetMaterialIDAndCost(1, 0, item, cost) && !item && !cost);
    GambleTemplateDataValue material; material.nCostItemID = 789; material.nCostItemCount = 4;
    GambleTemplateDataIndexManager::getSingleton().found = &material;
    assert(remake.GetMaterialIDAndCost(1, 0, item, cost) && item == 789 && cost == 4);
    GambleTemplateDataIndexManager::getSingleton().found = nullptr;
    puts("Ranking initialized time/records, conversion failures, short packets and password tail bytes passed");
}
}
#pragma pop_macro("SAFE_DELETE")
''')
    return "\n".join(parts)
