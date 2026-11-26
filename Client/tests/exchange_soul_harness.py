"""Production exchange list consumers, record defaults and soul input bounds."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    wire = clean(source(client / "Common/message/m2c_message.h"))
    gold = clean(source(game / "MMOGoldStoreManager.cpp"))
    gold_header = clean(source(game / "MMOGoldStoreManager.h"))
    heaven = clean(source(game / "Src/MMOHeavenManager.h"))
    network = clean(source(game / "net/message_handler_GoldStore.cpp"))
    soul = clean(source(game / "net/message_handler_Soul.cpp"))
    parsers = re.findall(r"void CMessageHandler::Parse_(\w+)\(const char\* buf, UINT32 size\)", network)
    if len(parsers) != 5:
        raise ValueError("Exchange parser coverage changed")
    parts = [r'''
namespace ExchangeSoulTests {
struct FString {
    unsigned magic = 0x1234abcd;
    std::string value;
};
using STLIntVector = std::vector<INT>;
namespace ExternalMessage {
#pragma pack(push, 1)
''']
    for name in ["EXCHANGE_ORDER_INFO", "M2C_EXCHANGE_ADD_ORDER", "M2C_EXCHANGE_CANCEL_ORDER",
                 "EXCHANGE_PRICE_INFO", "M2C_EXCHANGE_LIST_PRICES", "M2C_EXCHANGE_LIST_MY_ORDERS",
                 "EXCHANGE_LOG_INFO", "M2C_EXCHANGE_LIST_MY_LOGS"]:
        match = re.search(r"struct " + name + r"\b[^;{]*\{", wire).group()
        parts.append(function(wire, match) + ";")
    soul_wire = clean(source(client / "Common/message/g2c_message.h"))
    for name in ["UPGRADE", "ADD", "REMOVE"]:
        parts.append(function(soul_wire, "struct G2C_" + name + "_SOUL :") + ";")
    parts.append(function(clean(source(client / "Common/message/c2g_message.h")), "struct C2G_UPGRADE_SOUL :") + ";")
    parts.append("#pragma pack(pop)\n}\nusing namespace ExternalMessage;")
    declarations = {}
    for header, names in [(gold_header, ["Client_ExchangeItemInfo", "ExchangePriceInfo", "GoldStoreMyLogsData"]),
                          (heaven, ["BloodSlot", "HeavenGasItemConfig"])]:
        for name in names:
            match = re.search(r"struct " + name + r"\s*\{", header).group()
            declarations[name] = function(header, match)
            parts.append(declarations[name] + ";")
    exchange = clean(source(client / "Common/define/exchange_def.h"))
    parts.extend(function(exchange, "enum " + name) + ";" for name in ["EOrderType", "EOrderStatus"])
    parts.append(re.search(r"static const UINT32 EXCHANGE_TIME\s*=[^;]+;",
                           source(client / "Common/define/item_def.h")).group())
    parts.append(re.search(r"#define MAX_ORDER_COUNT\s+[^\n]+", gold).group())
    parts.append(r'''
using GoldStoreDataList = std::list<ExchangePriceInfo>;
using MyOrderDataList = std::list<Client_ExchangeItemInfo>;
using GoldStoreMyLogs = std::vector<GoldStoreMyLogsData*>;
struct MMOGoldStoreManager {
    int calls = 0;
    GoldStoreDataList _goldStoreBuyItemList, _goldStoreSellItemList;
    MyOrderDataList _goldMyOrders;
    GoldStoreMyLogs _MyLogsVec;
    static MMOGoldStoreManager& getSingleton() { static MMOGoldStoreManager value; return value; }
    void OnServer_AddOrder(EXCHANGE_ORDER_INFO* order) { assert(order->qwOrderID || !order->nCount); ++calls; }
    void OnServer_CancelSell(UINT64) { ++calls; }
    void OnServer_Listprices(UINT8, UINT8, EXCHANGE_PRICE_INFO*);
    void Onserver_ListMyOrders(UINT8, UINT8, EXCHANGE_ORDER_INFO*);
    void ConvertServerGoldStoreItemList_My(MyOrderDataList&, UINT8, EXCHANGE_ORDER_INFO*);
    void AddMyLogs(INT32, EXCHANGE_LOG_INFO*);
    void ClearMylogs();
    void NotifyUpdateMyLogs() { ++calls; }
    void NotifySellingList() { ++calls; }
    void NotifyBuyingList() { ++calls; }
    void NotifyMyList(UINT8) { ++calls; }
    static bool CompareMoney_Less(const ExchangePriceInfo&, const ExchangePriceInfo&);
    static bool CompareMoney_Large(const ExchangePriceInfo&, const ExchangePriceInfo&);
    static bool CompareOrderTime(const Client_ExchangeItemInfo&, const Client_ExchangeItemInfo&);
};
__time32_t _time32(__time32_t* output) { *output = 1000000; return *output; }
tm* _localtime32(const __time32_t*) { return nullptr; }
enum { c2g_upgrade_soul = 50, GameEvent_SoulLevelUp, GameEvent_SoulEmbed, GameEvent_SoulUnEmbed };
struct MMOGameEvent_OneInt { MMOGameEvent_OneInt(int, int) {} };
namespace MMOUI {
int events = 0;
void PostGameEvent(MMOGameEvent_OneInt*) { ++events; }
}
struct CMessageHandler {
    int sends = 0;
    std::vector<char> sent;
    void _Send(const void* bytes, size_t size) {
        ++sends; sent.assign((const char*)bytes, (const char*)bytes + size);
    }
    void Send_UpgradeSoul(UINT, UINT, UINT, const STLIntVector&, const STLIntVector&);
''')
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name in parsers)
    for name in ["upgrade", "add", "remove"]:
        parts.append("void Parse_g2c_" + name + "_soul(const char*, UINT32);\n"
                     "void Parse_g2c_" + name + "_soul(UINT8);")
    parts.append("};")
    for name in parsers:
        parts.append(function(network, "void CMessageHandler::Parse_" + name + "("))
    for name in ["upgrade", "add", "remove"]:
        parts.append(function(soul, "void CMessageHandler::Parse_g2c_" + name + "_soul(const char*"))
        parts.append(function(soul, "void CMessageHandler::Parse_g2c_" + name + "_soul(UINT8"))
    parts.append(function(soul, "void CMessageHandler::Send_UpgradeSoul("))
    for signature in [
        "UINT32 Client_ExchangeItemInfo::ConvertFromServer_Order(",
        "UINT32 ExchangePriceInfo::ConvertFromServer(",
        "bool MMOGoldStoreManager::CompareMoney_Less(", "bool MMOGoldStoreManager::CompareMoney_Large(",
        "bool MMOGoldStoreManager::CompareOrderTime(", "void MMOGoldStoreManager::OnServer_Listprices(",
        "void MMOGoldStoreManager::Onserver_ListMyOrders(",
        "void MMOGoldStoreManager::ConvertServerGoldStoreItemList_My(",
        "void MMOGoldStoreManager::AddMyLogs(", "void MMOGoldStoreManager::ClearMylogs()",
    ]:
        parts.append(function(gold, signature))
    parts.append(r'''
using Parser = void (CMessageHandler::*)(const char*, UINT32);
template<class T> void ShortHeaders(CMessageHandler& handler, Parser parser) {
    const int before = MMOGoldStoreManager::getSingleton().calls + MMOUI::events;
    (handler.*parser)(nullptr, 0); (handler.*parser)(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(T); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1);
        memset(bytes.get(), 0xff, size); (handler.*parser)(bytes.get(), size);
    }
    assert(before == MMOGoldStoreManager::getSingleton().calls + MMOUI::events);
    T packet = {}; (handler.*parser)((const char*)&packet, sizeof(packet));
    assert(before < MMOGoldStoreManager::getSingleton().calls + MMOUI::events);
}
template<class T, class Entry, class SetCount> void ListBounds(CMessageHandler& handler, Parser parser, SetCount set_count) {
    auto bytes = std::make_unique<char[]>(sizeof(T) + sizeof(Entry));
    memset(bytes.get(), 0, sizeof(T) + sizeof(Entry));
    T* packet = (T*)bytes.get(); set_count(packet, 1);
    const int calls = MMOGoldStoreManager::getSingleton().calls;
    for (UINT32 size = sizeof(T); size < sizeof(T) + sizeof(Entry); ++size) {
        (handler.*parser)(bytes.get(), size); assert(calls == MMOGoldStoreManager::getSingleton().calls);
    }
    for (int count : {2, 255}) {
        set_count(packet, count); (handler.*parser)(bytes.get(), sizeof(T) + sizeof(Entry));
        assert(calls == MMOGoldStoreManager::getSingleton().calls);
    }
    set_count(packet, 1); (handler.*parser)(bytes.get(), sizeof(T) + sizeof(Entry));
    assert(calls < MMOGoldStoreManager::getSingleton().calls);
}
void Run() {
''')
    for name, declaration in declarations.items():
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& value) {")
        for kind, field in re.findall(r"(?m)^\s*(INT|INT32|UINT32|UINT64|UINT8|FString)\s+(\w+)\s*;", declaration):
            if kind == "FString":
                parts.append("assert(value." + field + ".magic == 0x1234abcd && value." + field + ".value.empty());\n"
                             "value." + field + ".value.assign(1000, 'x');")
            else:
                parts.append("assert(!value." + field + ");")
        parts.append("});")
    parts.append("CMessageHandler handler;")
    for name in parsers:
        kind = re.search(r"\b(\w+)\* msg\s*=", function(network, "void CMessageHandler::Parse_" + name + "(")).group(1)
        parts.append("ShortHeaders<" + kind + ">(handler, &CMessageHandler::Parse_" + name + ");")
    for name in ["upgrade", "add", "remove"]:
        parts.append("ShortHeaders<G2C_" + name.upper() + "_SOUL>(handler, &CMessageHandler::Parse_g2c_" + name + "_soul);")
    parts.append(r'''
    ListBounds<M2C_EXCHANGE_LIST_PRICES, EXCHANGE_PRICE_INFO>(handler, &CMessageHandler::Parse_m2c_exchange_list_prices,
        [](auto* packet, int count) { packet->bySellCount = (UINT8)count; });
    ListBounds<M2C_EXCHANGE_LIST_PRICES, EXCHANGE_PRICE_INFO>(handler, &CMessageHandler::Parse_m2c_exchange_list_prices,
        [](auto* packet, int count) { packet->byBuyCount = (UINT8)count; });
    ListBounds<M2C_EXCHANGE_LIST_MY_ORDERS, EXCHANGE_ORDER_INFO>(handler, &CMessageHandler::Parse_m2c_exchange_list_my_orders,
        [](auto* packet, int count) { packet->byCount = (UINT8)count; });
    ListBounds<M2C_EXCHANGE_LIST_MY_LOGS, EXCHANGE_LOG_INFO>(handler, &CMessageHandler::Parse_m2c_exchange_list_my_logs,
        [](auto* packet, int count) { packet->nCount = count; });
    auto& manager = MMOGoldStoreManager::getSingleton();
    auto log_bytes = std::make_unique<char[]>(sizeof(M2C_EXCHANGE_LIST_MY_LOGS));
    auto* log = (M2C_EXCHANGE_LIST_MY_LOGS*)log_bytes.get();
    for (int count : {-2147483647 - 1, -1, 2147483647}) {
        log->nCount = count; const int calls = manager.calls;
        handler.Parse_m2c_exchange_list_my_logs(log_bytes.get(), sizeof(*log)); assert(manager.calls == calls);
    }
    const size_t price_size = sizeof(M2C_EXCHANGE_LIST_PRICES) + 510 * sizeof(EXCHANGE_PRICE_INFO);
    auto prices_bytes = std::make_unique<char[]>(price_size);
    memset(prices_bytes.get(), 0, price_size);
    auto* prices = (M2C_EXCHANGE_LIST_PRICES*)prices_bytes.get();
    prices->bySellCount = prices->byBuyCount = 255;
    for (int i = 0; i < 510; ++i) { prices->stPrices[i].nPrice = i; prices->stPrices[i].nCount = 1000 + i; }
    int calls = manager.calls;
    handler.Parse_m2c_exchange_list_prices(prices_bytes.get(), (UINT32)price_size - 1); assert(manager.calls == calls);
    handler.Parse_m2c_exchange_list_prices(prices_bytes.get(), (UINT32)price_size);
    assert(manager._goldStoreBuyItemList.size() == 255 && manager._goldStoreSellItemList.size() == 255);
    assert(manager._goldStoreBuyItemList.front().nPrice == 0 && manager._goldStoreSellItemList.front().nPrice == 509);
    assert(manager._goldStoreBuyItemList.front().nType == eotSell && manager._goldStoreSellItemList.front().nType == eotBuy);
    EXCHANGE_ORDER_INFO source_order = {}; source_order.qwOrderID = 123; source_order.nPrice = 456;
    Client_ExchangeItemInfo converted;
    assert(converted.ConvertFromServer_Order(&source_order) == sizeof(source_order));
    assert(converted.dwOrderID == 123 && converted.nPrice == 456 && !converted.dwFinishTime);
    for (UINT count : {0u, 1u, 4u, 5u, 6u, 0xffffffffu}) {
        for (size_t containers = 0; containers <= 5; ++containers) {
            for (size_t slots = 0; slots <= 5; ++slots) {
                STLIntVector con(containers, 7), items(slots, 8);
                const int sends = handler.sends;
                handler.Send_UpgradeSoul(3, 4, count, con, items);
                const bool valid = count <= 5 && count <= containers && count <= slots;
                assert(handler.sends == sends + (valid ? 1 : 0));
                if (valid) {
                    assert(handler.sent.size() == sizeof(C2G_UPGRADE_SOUL));
                    auto* packet = (C2G_UPGRADE_SOUL*)handler.sent.data();
                    assert(packet->bySrcCount == count && packet->byTargetConSlot == 3 && packet->byTargetItemSlot == 4);
                    for (UINT i = 0; i < 5; ++i) {
                        assert(packet->bySrcConSlot[i] == (i < count ? 7 : 0));
                        assert(packet->bySrcItemSlot[i] == (i < count ? 8 : 0));
                    }
                }
            }
        }
    }
    manager.ClearMylogs();
    puts("Exchange counts, initialized records/strings and soul-upgrade source-array bounds passed");
}
}
''')
    return "\n".join(parts)
