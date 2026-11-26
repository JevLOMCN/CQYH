"""Construct real tip/stall/shop records in poisoned storage."""
import re


def harness(game, source, function):
    def definition(path, name):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / path))
        return function(text, "struct " + name + "\n") + ";"

    parts = [r'''
namespace TipsStallTests {
enum EEventParm { EC_EventParm };
using BITFIELD = unsigned int;
struct UItem;
template<class T> using TArray = std::vector<T>;
struct FString : std::string {
    using std::string::string;
    static FString Printf(const char* format, INT a, INT b) {
        char result[128];
        snprintf(result, sizeof(result), format, a, b);
        return FString(result);
    }
};
''']
    for path, name in [("MMOFreeTipsDataManager.h", "FFreeTipsData"),
                       ("MMOFreeTipsUIManager.h", "FreeTipsData"),
                       ("UStallManager.h", "StallItemInfo"),
                       ("Src/Shop.h", "ServerShopItemData"),
                       ("Src/Shop.h", "ServerShopData")]:
        parts.append(definition(path, name))
    parts.append(function(source(game / "Src/Shop.h"),
                          "static FString FormatAsString(ServerShopItemData* obj)"))
    parts.append(r'''
template<class T, class Check, class... Args> void Poisoned(Check check, Args... args) {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(T) unsigned char bytes[sizeof(T)];
        memset(bytes, pattern, sizeof(bytes));
        T* value = new(bytes) T(args...);
        check(*value);
        value->~T();
    }
}
void Run() {
    auto data = [](FFreeTipsData& value) {
        assert(!value.ID);
        for (auto text : {&value.MovieName, &value.PanelName, &value.ValueName, &value.ScriptTxt}) {
            assert(text->empty()); text->assign(120, 'x');
        }
    };
    Poisoned<FFreeTipsData>(data); Poisoned<FFreeTipsData>(data, EC_EventParm);
    auto tips = [](FreeTipsData& value) {
        assert(!value.ID);
        for (auto text : {&value.MovieName, &value.PanelName, &value.Element1Name, &value.Element2Name,
                          &value.Element3Name, &value.Element4Name, &value.TipsScript,
                          &value.TipsType, &value.ClassesName, &value.KeyName}) {
            assert(text->empty()); text->assign(120, 'x');
        }
    };
    Poisoned<FreeTipsData>(tips); Poisoned<FreeTipsData>(tips, EC_EventParm);
    auto item = [](StallItemInfo& value) {
        assert(!value.IsLocked && !value.ItemId && !value.Item && !value.DurType && !value.MaxStack);
        assert(!value.CurDur && !value.Price && !value.ModifyTime);
    };
    Poisoned<StallItemInfo>([&](StallItemInfo& value) {
        item(value); assert(value.SrcBag == -1 && value.SrcSlot == -1);
    });
    Poisoned<StallItemInfo>([&](StallItemInfo& value) {
        item(value); assert(!value.SrcBag && !value.SrcSlot);
    }, EC_EventParm);
    Poisoned<ServerShopData>([](ServerShopData& value) {
        assert(!value._ShopId && value._ShopName.empty() && value._ShowItemList.empty());
    });
    ServerShopItemData product;
    product.ShopSlot = 23;
    assert(FormatAsString(&product) == "ServerShopItemData[23]23");
    puts("Tip/stall/shop initialization and numeric shop log formatting passed");
}
}
''')
    return "\n".join(parts)
