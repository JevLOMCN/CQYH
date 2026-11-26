"""Production store packet lengths, fixed names, page bounds and UI records."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    wire = clean(source(client / "Common/message/m2c_message.h"))
    header = clean(source(game / "Src/MallManager.h"))
    header = re.sub(r"(?m)^\s*#include[^\n]*", "", header)
    cpp = clean(source(game / "Src/MallManager.cpp"))
    network = clean(source(game / "net/message_handler_Mall.cpp"))
    parts = [r'''
#include <cstdarg>
#include <ctime>
#include <list>
namespace MallSafetyTests {
using INT16 = int16_t;
using TCHAR = char;
const char* UTF8_TO_TCHAR(const char* value) { return value; }
struct FString {
    std::string value;
    FString(const char* text = "") : value(text) {}
    const char* operator*() const { return value.c_str(); }
    FString& operator+=(const FString& other) { value += other.value; return *this; }
    static FString Printf(const char* format, ...) {
        va_list args; va_start(args, format);
        char text[1024]; vsnprintf(text, sizeof(text), format, args); va_end(args);
        return FString(text);
    }
};
#ifndef M_UE3_STL_ALLOC
#define M_UE3_STL_ALLOC(type) std::allocator<type>
#endif
template<class T> struct SingletonMeyers {
    static T& getSingleton() { static T value; return value; }
};
struct MMOGameEvent {};
struct INativeGameEventHandler { virtual void OnNativeGameEvent(MMOGameEvent*) = 0; };
template<class T> struct TArray {
    std::vector<T> data;
    void Empty() { data.clear(); }
    void AddItem(const T& value) { data.push_back(value); }
};
using std::min;
template<class T> T Min(T a, T b) { return min(a, b); }
enum { GameEvent_RealMoneyChange, GameEvent_UpdateMoneyExchangeLog, c2m_real_money_buy = 42 };
struct MMOGameEvent_Any { explicit MMOGameEvent_Any(int) {} };
namespace MMOUI {
int events = 0;
void PostGameEvent(MMOGameEvent_Any*) { ++events; }
}
struct UItem {};
struct MMOFacade : SingletonMeyers<MMOFacade> {
    UItem item;
    UItem* FindItemTemplate(int id) { return id > 0 ? &item : nullptr; }
};
struct LinkManager { static FString HtmlLink_ColorItem(UItem*) { return FString("item"); } };
struct MallController : SingletonMeyers<MallController> {
    int calls = 0;
    void NotifyRealMoneyChange() { ++calls; }
    void NotifyGoodsListChange() { ++calls; }
    void NotifyBuyResult(int) { ++calls; }
    void NotifyBuyInfoChange() { ++calls; }
};
namespace ExternalMessage {
#pragma pack(push, 1)
''']
    role_capacity = re.search(r"#define ROLE_NAME_LEN\s+([^\n]+)",
                              clean(source(client / "Common/define/string_def.h"))).group(1)
    parts.insert(1, "enum { ROLE_NAME_LEN = " + role_capacity + " };")
    for name in ["M2C_SYNC_REAL_MONEY", "REAL_MONEY_ITEM_INFO", "M2C_SYNC_REAL_MONEY_SHOP_DATA",
                 "M2C_REAL_MONEY_BUY", "SHOP_LOG", "M2C_REAL_MONEY_SHOP_LOG", "SALE_INFO", "M2C_SALE_INFO"]:
        parts.append(function(wire, "struct " + name + ("\n" if name in ["SHOP_LOG", "SALE_INFO", "REAL_MONEY_ITEM_INFO"] else " :")) + ";")
    parts.append(function(clean(source(client / "Common/message/c2m_message.h")),
                          "struct C2M_REAL_MONEY_BUY :") + ";")
    parts.append("#pragma pack(pop)\n}\nusing namespace ExternalMessage;")
    parts.append(function(clean(source(game / "Inc/MMOUtil.h")), "template<size_t Size>"))
    parts.append(header.replace("private:", "public:"))
    parts.append(r'''
struct CMessageHandler {
    std::vector<char> sent;
    void _Send(const void* bytes, size_t size) { sent.assign((const char*)bytes, (const char*)bytes + size); }
    void Send_MallBuyGoods(INT, INT);
    void Send_MallGiveFriend(FString, INT, INT);
    void Parse_m2c_sync_real_money(const char*, UINT32);
    void Parse_m2c_real_money_buy(const char*, UINT32);
    void Parse_m2c_sync_real_money_shop_data(const char*, UINT32);
    void Parse_m2c_sale_info(const char*, UINT32);
    void Parse_m2c_real_money_shop_log(const char*, UINT32);
};
''')
    for signature in [
        "MallManager::MallManager()", "void MallManager::OnNativeGameEvent(",
        "void MallManager::ClearData()", "void MallManager::NotifyRealMoney(",
        "void MallManager::NotifyGoodsList(", "void MallManager::NotifyBuyInfo(",
        "void MallManager::GetGoodsByPage(", "void MallManager::UpdateMoneyLog(",
    ]:
        parts.append(function(cpp, signature))
    for name in ["m2c_sync_real_money", "m2c_real_money_buy", "m2c_sync_real_money_shop_data",
                 "m2c_sale_info", "m2c_real_money_shop_log"]:
        parts.append(function(network, "void CMessageHandler::Parse_" + name + "("))
    for name in ["Send_MallBuyGoods", "Send_MallGiveFriend"]:
        parts.append(function(network, "void CMessageHandler::" + name + "("))
    for path, name in [("Src/LoadMapTipsManager.h", "LoadMapTipsData"),
                       ("MMOHurtManager.h", "HurtData")]:
        parts.append(function(clean(source(game / path)), "struct " + name + "\n") + ";")
    parts.append(r'''
using DOUBLE = double;
float random_fraction = 0.0f;
float appFrand() { return random_fraction; }
struct LoadMapTipsManager {
    std::vector<LoadMapTipsData> vecMapTips;
    INT RandomNO(std::list<INT>&) const;
};
struct IMMOHintButtonCommand { virtual void Click() = 0; };
struct MMOHintButtonManager {
    using HintButtonCommandMap = std::map<int, IMMOHintButtonCommand*>;
    std::vector<int> _HintPosVec;
    HintButtonCommandMap _HintCommandMap;
    void ClickButton(int);
};
''')
    parts.append(function(clean(source(game / "Src/LoadMapTipsManager.cpp")),
                          "INT LoadMapTipsManager::RandomNO("))
    parts.append(function(clean(source(game / "Src/MMOHintButtonManager.cpp")),
                          "void MMOHintButtonManager::ClickButton("))
    parts.append(r'''
using Parser = void (CMessageHandler::*)(const char*, UINT32);
template<class T> void ShortPackets(CMessageHandler& handler, Parser parser) {
    const int calls = MallController::getSingleton().calls, events = MMOUI::events;
    (handler.*parser)(nullptr, 0);
    (handler.*parser)(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(T); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1);
        memset(bytes.get(), 0xff, size);
        (handler.*parser)(bytes.get(), size);
    }
    assert(calls == MallController::getSingleton().calls && events == MMOUI::events);
}
template<class T, class Entry> std::vector<char> Packet() {
    return std::vector<char>(sizeof(T) + sizeof(Entry));
}
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        TipsStallTests::Poisoned<MALLGOODSSHOWDATA>([](auto& value) {
            assert(!value.nItemTemplateID && !value.nHotIndex && !value.nShowIndex && value.iconShowPath.value.empty());
        });
        alignas(LoadMapTipsData) unsigned char tips_bytes[sizeof(LoadMapTipsData)];
        memset(tips_bytes, pattern, sizeof(tips_bytes));
        auto* tips = new(tips_bytes) LoadMapTipsData;
        assert(!tips->nID && !tips->nMapId && !tips->nClass && !tips->nMinLevel && !tips->nMaxLevel && !tips->nWeight);
        assert(tips->strTips.value.empty() && tips->Image.value.empty()); tips->~LoadMapTipsData();
        alignas(HurtData) unsigned char hurt_bytes[sizeof(HurtData)];
        memset(hurt_bytes, pattern, sizeof(hurt_bytes));
        auto* hurt = new(hurt_bytes) HurtData; assert(!hurt->AllHurts && !hurt->HurtTime);
    }
    CMessageHandler handler;
    ShortPackets<M2C_SYNC_REAL_MONEY>(handler, &CMessageHandler::Parse_m2c_sync_real_money);
    ShortPackets<M2C_REAL_MONEY_BUY>(handler, &CMessageHandler::Parse_m2c_real_money_buy);
    ShortPackets<M2C_SYNC_REAL_MONEY_SHOP_DATA>(handler, &CMessageHandler::Parse_m2c_sync_real_money_shop_data);
    ShortPackets<M2C_SALE_INFO>(handler, &CMessageHandler::Parse_m2c_sale_info);
    ShortPackets<M2C_REAL_MONEY_SHOP_LOG>(handler, &CMessageHandler::Parse_m2c_real_money_shop_log);
    auto& manager = MallManager::getSingleton();
    auto goods_bytes = Packet<M2C_SYNC_REAL_MONEY_SHOP_DATA, REAL_MONEY_ITEM_INFO>();
    auto* goods = (M2C_SYNC_REAL_MONEY_SHOP_DATA*)goods_bytes.data();
    goods->dwCrc = 77; goods->dwItemCount = 1; goods->Items[0].m_nItemTemplateID = 123;
    handler.Parse_m2c_sync_real_money_shop_data(goods_bytes.data(), (UINT32)goods_bytes.size());
    assert(manager.nCrc == 77 && manager.mallGoodsList.size() == 1);
    int calls = MallController::getSingleton().calls;
    goods->dwCrc = 88;
    for (UINT32 count : {2u, 0x40000001u, 0xffffffffu}) {
        goods->dwItemCount = count;
        handler.Parse_m2c_sync_real_money_shop_data(goods_bytes.data(), (UINT32)goods_bytes.size());
        assert(manager.nCrc == 77 && manager.mallGoodsList.size() == 1);
        assert(calls == MallController::getSingleton().calls);
    }
    auto sale_bytes = Packet<M2C_SALE_INFO, SALE_INFO>();
    auto* sale = (M2C_SALE_INFO*)sale_bytes.data();
    sale->nSaleInfoCount = 1; sale->stSaleInfo[0].nItemID = 123; sale->stSaleInfo[0].dwTime = 1000000;
    memset(sale->stSaleInfo[0].szRoleName, 'N', sizeof(sale->stSaleInfo[0].szRoleName));
    handler.Parse_m2c_sale_info(sale_bytes.data(), (UINT32)sale_bytes.size());
    assert(manager.mallBuyInfoList.size() == 1);
    assert(manager.mallBuyInfoList.front().value.find(std::string(ROLE_NAME_LEN, 'N')) != std::string::npos);
    calls = MallController::getSingleton().calls;
    for (int count : {-1, -2147483647 - 1, 2, 2147483647}) {
        sale->nSaleInfoCount = count;
        handler.Parse_m2c_sale_info(sale_bytes.data(), (UINT32)sale_bytes.size());
        assert(manager.mallBuyInfoList.size() == 1 && calls == MallController::getSingleton().calls);
    }
    auto log_bytes = Packet<M2C_REAL_MONEY_SHOP_LOG, SHOP_LOG>();
    auto* logs = (M2C_REAL_MONEY_SHOP_LOG*)log_bytes.data();
    logs->nShopLogCount = 1; logs->stShopLog[0].nItemID = 321;
    handler.Parse_m2c_real_money_shop_log(log_bytes.data(), (UINT32)log_bytes.size());
    assert(manager.shopLogArray.data.size() == 1 && manager.shopLogArray.data[0].nItemID == 321);
    int events = MMOUI::events;
    for (int count : {-1, -2147483647 - 1, 2, 2147483647}) {
        logs->nShopLogCount = count;
        handler.Parse_m2c_real_money_shop_log(log_bytes.data(), (UINT32)log_bytes.size());
        assert(events == MMOUI::events && manager.shopLogArray.data.size() == 1);
    }
    logs->nShopLogCount = 0;
    handler.Parse_m2c_real_money_shop_log(log_bytes.data(), sizeof(*logs));
    assert(manager.shopLogArray.data.empty());
    M2C_SYNC_REAL_MONEY money = {}; money.nBalance = 1234;
    handler.Parse_m2c_sync_real_money((char*)&money, sizeof(money)); assert(manager.nRealMoney == 1234);
    M2C_REAL_MONEY_BUY buy = {}; buy.nMoneyLeft = 456;
    handler.Parse_m2c_real_money_buy((char*)&buy, sizeof(buy)); assert(manager.nRealMoney == 456);
    for (const char* name : {"", "Bob", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"}) {
        handler.Send_MallGiveFriend(FString(name), 45, 2);
        assert(handler.sent.size() == sizeof(C2M_REAL_MONEY_BUY));
        auto* sent = (C2M_REAL_MONEY_BUY*)handler.sent.data();
        const size_t length = min(strlen(name), size_t(ROLE_NAME_LEN - 1));
        assert(sent->nItemID == 45 && sent->nCount == 2);
        assert(!memcmp(sent->szRoleName, name, length));
        for (size_t i = length; i < ROLE_NAME_LEN; ++i) assert(!sent->szRoleName[i]);
    }
    handler.Send_MallBuyGoods(45, 2);
    auto* sent = (C2M_REAL_MONEY_BUY*)handler.sent.data();
    for (char value : sent->szRoleName) assert(!value);
    manager.vecMallCurGoods.resize(32);
    for (int i = 0; i < 32; ++i) manager.vecMallCurGoods[i] = i;
    std::vector<int> page;
    for (int number : {-2147483647 - 1, -1, 3, 143165576, 2147483647}) {
        manager.nCurPage = number; page.push_back(-99); manager.GetGoodsByPage(page); assert(page.empty());
    }
    for (int number : {0, 1, 2}) {
        manager.nCurPage = number; manager.GetGoodsByPage(page);
        assert(page.size() == (number == 2 ? 2 : 15));
        for (size_t i = 0; i < page.size(); ++i) assert(page[i] == 15 * number + i);
    }
    struct Command : IMMOHintButtonCommand { int clicks = 0; void Click() { ++clicks; } } command;
    MMOHintButtonManager hints; hints._HintPosVec = {10, 11, 12};
    hints._HintCommandMap[10] = &command; hints._HintCommandMap[11] = nullptr;
    for (int pos : {-2147483647 - 1, -1, 1, 2, 3, 2147483647}) hints.ClickButton(pos);
    assert(!command.clicks); hints.ClickButton(0); assert(command.clicks == 1);
    LoadMapTipsManager tips;
    tips.vecMapTips.resize(4);
    std::list<int> candidates = {-2147483647 - 1, -1, 0, 4, 2147483647};
    assert(tips.RandomNO(candidates) == 0);
    candidates = {-1, 4}; assert(tips.RandomNO(candidates) == -1);
    candidates.clear(); assert(tips.RandomNO(candidates) == -1);
    candidates = {-1, 0, 1, 2, 3, 4};
    tips.vecMapTips[0].nWeight = -2147483647 - 1;
    tips.vecMapTips[1].nWeight = 0;
    tips.vecMapTips[2].nWeight = tips.vecMapTips[3].nWeight = 2147483647;
    for (float fraction : {0.0f, 0.25f, 0.49f, 0.5f, 0.75f, 1.0f}) {
        random_fraction = fraction;
        assert(tips.RandomNO(candidates) == (fraction < 0.5f ? 2 : 3));
    }
    tips.vecMapTips[2].nWeight = 1; tips.vecMapTips[3].nWeight = 3;
    random_fraction = 0.24f; assert(tips.RandomNO(candidates) == 2);
    random_fraction = 0.25f; assert(tips.RandomNO(candidates) == 3);
    puts("Store packet counts, fixed names, initialized records, page/hint bounds and loading-tip weights passed");
}
}
''')
    return "\n".join(parts)
