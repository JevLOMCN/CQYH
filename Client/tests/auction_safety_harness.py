"""Exercise the actual auction constructors and variable-length decoders."""
import re


def harness(client, game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(path))

    header = clean(game / "MMOAuctionManager.h")
    implementation = clean(game / "MMOAuctionManager.cpp")
    wire = clean(client / "Common/message/m2c_message.h")
    strings = clean(client / "Common/define/string_def.h")
    facade = clean(game / "MMOFacade.h")
    base = facade[facade.index("struct FClientItemInfo"):facade.index("    void MinusSaleDur")]
    parts = [r'''
#include <cmath>
namespace AuctionTests {
using std::ceil;
struct UItem {};
enum { ItemSlotHint_Normal = 0 };
''', base + "\n};"]
    parts.extend(re.search(r"#define " + name + r"\s+[^\n]+", strings).group()
                 for name in ["ROLE_NAME_LEN", "GUILD_NAME_LEN"])
    parts.append("#pragma pack(push, 1)")
    parts.extend(function(wire, "struct " + name) + ";"
                 for name in ["AUCTION_ITEM_INFO", "M2C_AUCTION_QUERY"])
    parts.append("#pragma pack(pop)")
    # Only string formatting and template lookup need engine substitutes.
    parts.append(r'''
struct FString : ::FStringA {
    using ::FStringA::FStringA;
    static FString Printf(const char*, UINT8 value) { return FString(std::to_string(value).c_str()); }
    FString operator+(const FString& rhs) const { return FString((value + rhs.value).c_str()); }
};
namespace MMOUI { FString ParseLocalizedPropertyPath(FString value) { return value; } }
#define UTF8_TO_TCHAR(s) (s)
struct MMOFacade {
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    UItem* FindItemTemplate(INT) { static UItem item; return &item; }
};
''')
    parts.extend(function(header, "struct " + name) + ";"
                 for name in ["ClientAuctionItemInfo", "QueryAuctionItemInfo", "AuctionSellInfo"])
    parts.append(function(implementation, "UINT32 ClientAuctionItemInfo::ConvertFromServer("))
    parts.append(r'''
struct MMOAuctionManager {
    QueryAuctionItemInfo currentQueryAuctionInfo;
    INT notifications = 0;
    void NotifyUpdateQueryItem() { ++notifications; }
    void OnServer_QueryAuctionItem(M2C_AUCTION_QUERY*, UINT32);
};
''')
    parts.append(function(implementation, "void MMOAuctionManager::OnServer_QueryAuctionItem("))
    parts.append(r'''
void Run() {
    check_constructed<ClientAuctionItemInfo>([](ClientAuctionItemInfo& item) {
        assert(!item.dwAuctionID && !item.dwSellerRoleID && !item.dwBuyerRoleID);
        assert(!item.nPrice && !item.nBuyoutPrice && !item.nDepositPrice);
        assert(!item.dwStartTime && !item.dwFinishTime && !item.byAuctionStatus && !item.ItemTemplate);
    });
    check_constructed<QueryAuctionItemInfo>([](QueryAuctionItemInfo& query) {
        assert(!query.dwAuctionID && !query.byFound && !query.dwBuyerCount && !query.bySellerLevel);
    });
    check_constructed<AuctionSellInfo>([](AuctionSellInfo& item) {
        assert(!item.byConSlot && !item.byItemSlot && !item.byAuctionTimeType && !item.byOpen);
        assert(!item.llPrice && !item.llBuyoutPrice && !item.llDepositePrice);
    });
    std::vector<char> record(sizeof(AUCTION_ITEM_INFO) + 128, 0);
    record[sizeof(AUCTION_ITEM_INFO)] = 3;
    auto* auction = reinterpret_cast<AUCTION_ITEM_INFO*>(record.data());
    auction->dwAuctionID = 123;
    auction->llPrice = 456;
    ClientAuctionItemInfo item;
    const UINT32 used = item.ConvertFromServer(auction, (UINT32)record.size());
    assert(used > sizeof(AUCTION_ITEM_INFO));
    assert(item.dwAuctionID == 123 && item.nPrice == 456 && item.ItemTemplate);
    assert(!item.ConvertFromServer(nullptr, (UINT32)record.size()));
    for (UINT32 length = 0; length < used; ++length) {
        std::vector<char> truncated(record.begin(), record.begin() + length);
        assert(!item.ConvertFromServer(reinterpret_cast<AUCTION_ITEM_INFO*>(truncated.data()), length));
    }

    MMOAuctionManager manager;
    // A negative query contains the fixed packet only, with no encoded item.
    M2C_AUCTION_QUERY absent = {};
    absent.dwAuctionID = 999;
    manager.OnServer_QueryAuctionItem(&absent, sizeof(absent));
    assert(manager.notifications == 1 && manager.currentQueryAuctionInfo.dwAuctionID == 999);
    assert(!manager.currentQueryAuctionInfo.byFound && !manager.currentQueryAuctionInfo.info.ItemTemplate);
    assert(manager.currentQueryAuctionInfo.szSellerName.value.empty());
    const size_t payload = used - sizeof(AUCTION_ITEM_INFO);
    std::vector<char> response(sizeof(M2C_AUCTION_QUERY) + payload, 0);
    auto* query = reinterpret_cast<M2C_AUCTION_QUERY*>(response.data());
    query->dwAuctionID = 123;
    query->byFound = 1;
    memset(query->szGuildName, 'G', sizeof(query->szGuildName));
    memset(query->szSellerName, 'S', sizeof(query->szSellerName));
    memcpy(&query->stAuction, record.data(), used);
    for (UINT32 length = 0; length < response.size(); ++length) {
        std::vector<char> truncated(response.begin(), response.begin() + length);
        manager.OnServer_QueryAuctionItem(reinterpret_cast<M2C_AUCTION_QUERY*>(truncated.data()), length);
        assert(manager.notifications == 1 && manager.currentQueryAuctionInfo.dwAuctionID == 999);
    }
    manager.OnServer_QueryAuctionItem(query, (UINT32)response.size());
    assert(manager.notifications == 2 && manager.currentQueryAuctionInfo.byFound);
    assert(manager.currentQueryAuctionInfo.info.dwAuctionID == 123);
    assert(manager.currentQueryAuctionInfo.szGuildName.value == std::string(GUILD_NAME_LEN, 'G'));
    assert(manager.currentQueryAuctionInfo.szSellerName.value == std::string(ROLE_NAME_LEN, 'S'));
    manager.OnServer_QueryAuctionItem(&absent, sizeof(absent));
    assert(manager.notifications == 3 && !manager.currentQueryAuctionInfo.byFound);
    assert(manager.currentQueryAuctionInfo.szGuildName.value.empty());
    assert(!manager.currentQueryAuctionInfo.info.ItemTemplate);
    puts("Auction constructor and packet boundary regressions passed");
}
#undef UTF8_TO_TCHAR
#undef ROLE_NAME_LEN
#undef GUILD_NAME_LEN
}
''')
    return "\n".join(parts)
