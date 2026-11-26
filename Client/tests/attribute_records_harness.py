"""Exercise actual attribute resets, array boundaries and drop record defaults."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "Src/MMOAttributeAccountManager.h"))
    cpp = clean(source(game / "Src/MMOAttributeAccountManager.cpp"))
    signatures = [
        "const FString MMOAttributeAccountManager::GetAttributeName(",
        "const FString MMOAttributeAccountManager::GetAttributeValue(",
        "const FString MMOAttributeAccountManager::GetAttributeDesc(",
        "UBOOL MMOAttributeAccountManager::IsRelationAttribute(",
        "INT MMOAttributeAccountManager::GetOwnerAttribute(",
        "void MMOAttributeAccountManager::SetAttrOwnerMode(",
    ]
    methods = "\n".join(function(cpp, signature) for signature in signatures)
    constants = sorted(set(re.findall(r"\b(?:cat\w+|SAIP_\w+)\b", methods)))
    parts = [r'''
namespace AttributeRecordTests {
using FLOAT = float;
enum { EPCT_PlayerClassA = 0, EPCT_PlayerClassG = 6, EPCT_PlayerClassH = 7, EPCT_MAX = 9 };
using AttrListenerType = int;
struct FString : std::string {
    unsigned magic = 0x9876abcd;
    using std::string::string;
    FString(const std::string& text) : std::string(text) {}
    void Empty() { assert(magic == 0x9876abcd); clear(); }
    const char* operator*() const { assert(magic == 0x9876abcd); return c_str(); }
    static FString FormatAsNumber(float value) { return std::to_string((int)value); }
    template<class... Args> static FString Printf(const char* format, Args... args) {
        char buffer[1024]; snprintf(buffer, sizeof(buffer), format, args...); return FString(buffer);
    }
};
FString appItoa(int value) { return std::to_string(value); }
template<class T> struct TArray {
    std::vector<T> data;
    int Num() const { return (int)data.size(); }
    bool IsValidIndex(int i) const { return i >= 0 && i < Num(); }
    T& operator()(int i) { return data.at(i); }
    const T& operator()(int i) const { return data.at(i); }
    void AddItem(const T& value) { data.push_back(value); }
};
struct MMOAttrListener {
    int classId = 0;
    int GetClassId() const { return classId; }
    int GetAttribute(int id) const { return id * 10; }
    int GetPkValue() const { return 2; }
    int GetFatigue() const { return 3; }
};
struct Attributes { int GetCurHp() { return 12; } int GetCurMp() { return 34; } } attributes;
struct AMMOPlayer : MMOAttrListener { Attributes* ActorAttributes = &attributes; } player;
struct MMOFacade {
    static MMOFacade& getSingleton() { static MMOFacade result; return result; }
    AMMOPlayer* GetMainPlayer() { return &player; }
    int GetCurrentBagWgt() { return 1; }
    int GetCurrentWeaponWgt() { return 2; }
    int GetCurrentArmorWgt() { return 3; }
};
struct UITextParser {
    static FString HTMLBeginFont(const char*) { return ""; }
    static FString HTMLEndFont() { return ""; }
};
''', "enum { " + ", ".join(name + " = " + str(100 + i) for i, name in enumerate(constants)) + " };",
        function(header, "enum EValueShowType\n") + ";",
        function(header, "struct StructAttributeItem\n") + ";",
        function(header, "struct StructAttributeGroup\n") + ";", r'''
struct MMOAttributeAccountManager {
    TArray<StructAttributeGroup> AttributeGroupList;
    TArray<MMOAttrListener*> AttrOwnerTypeList;
    MMOAttrListener* CurAttrListener = nullptr;
    int expEtra = 0;
    const FString GetAttributeName(INT, INT) const;
    const FString GetAttributeValue(INT, INT) const;
    const FString GetAttributeDesc(INT, INT) const;
    UBOOL IsRelationAttribute(INT, INT, INT);
    INT GetOwnerAttribute(INT, AttrListenerType);
    void SetAttrOwnerMode(AttrListenerType);
};
''', methods, r'''
void CheckDefault(const StructAttributeItem& value) {
    for (int i = 0; i < EPCT_MAX; ++i) {
        assert(value.AttributeName[i].magic == 0x9876abcd && value.AttributeName[i].empty());
        assert(value.TipsDesc[i].magic == 0x9876abcd && value.TipsDesc[i].empty());
        assert(!value.AttributeValueType[i] && !value.AttributeValueTypeB[i]);
        assert(!value.AttributeTypeA[i] && !value.AttributeTypeB[i]);
        assert(!value.AttributeMulParaA[i] && !value.AttributeMulParaB[i]);
    }
    assert(!value.DifferName && !value.DifferValueType && !value.DifferTips);
    assert(!value.DifferTipsValueType && !value.DifferTipsValuePara);
    assert(value.ValueShowType == EVST_Int && value.TipsValueShowType == EVST_Int);
}
void Run() {
    TipsStallTests::Poisoned<StructAttributeItem>([](auto& value) {
        CheckDefault(value);
        for (int pass = 0; pass < 5; ++pass) {
            for (int i = 0; i < EPCT_MAX; ++i) {
                value.AttributeName[i].assign(4000, 'n'); value.TipsDesc[i].assign(5000, 't');
                value.AttributeValueType[i] = value.AttributeValueTypeB[i] = 123;
                value.AttributeTypeA[i] = value.AttributeTypeB[i] = 456;
                value.AttributeMulParaA[i] = value.AttributeMulParaB[i] = 7.0f;
            }
            value.DifferName = value.DifferValueType = value.DifferTips = TRUE;
            value.DifferTipsValueType = value.DifferTipsValuePara = TRUE;
            value.ValueShowType = value.TipsValueShowType = EVST_MP;
            value.Reset(); CheckDefault(value);
        }
    });
    MMOAttributeAccountManager manager;
    manager.AttributeGroupList.data.resize(1);
    manager.AttributeGroupList(0).AttributeList.data.resize(1);
    manager.CurAttrListener = &player;
    auto& item = manager.AttributeGroupList(0).AttributeList(0);
    item.AttributeName[0] = "name"; item.TipsDesc[0] = "%s/%s";
    item.AttributeValueType[0] = 4;
    for (int invalid : {-2147483647 - 1, -1, 1, 2147483647}) {
        for (auto position : {std::pair<int, int>(invalid, 0), std::pair<int, int>(0, invalid)}) {
            assert(manager.GetAttributeName(position.first, position.second).empty());
            assert(manager.GetAttributeValue(position.first, position.second).empty());
            assert(manager.GetAttributeDesc(position.first, position.second).empty());
            assert(!manager.IsRelationAttribute(position.first, position.second, 4));
        }
    }
    assert(manager.GetAttributeName(0, 0) == "name" && manager.GetAttributeValue(0, 0) == "40");
    assert(manager.GetAttributeDesc(0, 0) == "40/0" && manager.IsRelationAttribute(0, 0, 4));
    item.DifferName = item.DifferValueType = item.DifferTips = TRUE;
    item.DifferTipsValueType = item.DifferTipsValuePara = TRUE;
    for (int invalid : {-2147483647 - 1, -1, int(EPCT_MAX), 2147483647}) {
        player.classId = invalid;
        assert(manager.GetAttributeName(0, 0).empty() && manager.GetAttributeValue(0, 0).empty());
        assert(manager.GetAttributeDesc(0, 0).empty() && !manager.IsRelationAttribute(0, 0, 4));
    }
    player.classId = EPCT_MAX - 1;
    item.AttributeName[player.classId] = "last"; item.AttributeValueType[player.classId] = 5;
    item.TipsDesc[player.classId] = "%s/%s";
    assert(manager.GetAttributeName(0, 0) == "last" && manager.GetAttributeValue(0, 0) == "50");
    assert(manager.GetAttributeDesc(0, 0) == "50/0" && manager.IsRelationAttribute(0, 0, 5));
    manager.AttrOwnerTypeList.data = {&player, nullptr};
    for (int invalid : {-2147483647 - 1, -1, 1, 2, 2147483647}) {
        assert(manager.GetOwnerAttribute(4, invalid) == 0);
        manager.SetAttrOwnerMode(invalid); assert(!manager.CurAttrListener);
    }
    manager.SetAttrOwnerMode(0); assert(manager.CurAttrListener == &player);
    assert(manager.GetOwnerAttribute(4, 0) == 40);
    puts("Attribute record string resets, row/class/owner bounds and valid displays passed");
}
struct FVector2D { float X, Y; FVector2D() {} FVector2D(float x, float y) : X(x), Y(y) {} };
struct UGFxObject;
''']
    drop = clean(source(game / "MMODropManager.h"))
    for name in ["FDropItemSet", "FDropGroup", "FDropActorInfo"]:
        parts.append(function(drop, "struct " + name + "\n") + ";")
    parts.append(r'''
void DropTests() {
    TipsStallTests::Poisoned<FDropItemSet>([](auto& v) {
        assert(!v.ItemTemplateID && !v.DropWeight && !v.CountMin && !v.CountMax);
    });
    TipsStallTests::Poisoned<FDropGroup>([](auto& v) {
        assert(!v.TemplateID && !v.Enabled && v.TemplateDesc.empty() && !v.DropItemList.Num());
    });
    TipsStallTests::Poisoned<FDropActorInfo>([](auto& v) {
        assert(!v.DropActor && !v.Hud && !v.ScreenPos.X && !v.ScreenPos.Y);
        assert(!v.Width && !v.Height && !v.IsVisible && !v.ShowTime && !v.CannotPickCD);
    });
}
struct UAI_Affix_Table {
    struct Entry { INT ID = 7; FString AffixName = "affix"; };
    TArray<Entry> Table;
    void ExportRowData(TArray<FString>&, INT);
    void ImportRowData(const TArray<FString>&);
};
INT appAtoi(const char* text) { return atoi(text); }
''')
    affix = clean(source(game / "Src/Ai_Affix_Table.cpp"))
    for signature in ["void UAI_Affix_Table::ExportRowData(", "void UAI_Affix_Table::ImportRowData("]:
        parts.append(function(affix, signature))
    parts.append(r'''
struct ReputationTemp { int id = 7; };
struct ItemInfo { int id = 8; };
struct FacadeAccess {
    TArray<ReputationTemp> _ReputationTempArray;
    TArray<ItemInfo*> _SoldItemList;
    ReputationTemp* GetReputationTemp(INT);
    ItemInfo* FindSoldItem(INT) const;
};
''')
    facade = clean(source(game / "MMOFacade.cpp"))
    for signature in ["ReputationTemp*  MMOFacade::GetReputationTemp(", "struct ItemInfo* MMOFacade::FindSoldItem("]:
        parts.append(function(facade, signature).replace("MMOFacade::", "FacadeAccess::"))
    parts.append(r'''
void ArrayTests() {
    UAI_Affix_Table table; table.Table.data.resize(1);
    for (int invalid : {-2147483647 - 1, -1, 1, 2147483647}) {
        TArray<FString> row; table.ExportRowData(row, invalid); assert(!row.Num());
    }
    TArray<FString> row; table.ExportRowData(row, 0);
    assert(row.Num() == 2 && row(0) == "7" && row(1) == "affix");
    TArray<FString> empty; table.ImportRowData(empty);
    TArray<FString> shortRow; shortRow.AddItem("7"); table.ImportRowData(shortRow);
    assert(table.Table(0).AffixName == "affix");
    shortRow.AddItem("updated"); table.ImportRowData(shortRow);
    assert(table.Table(0).AffixName == "updated");
    FacadeAccess facade; facade._ReputationTempArray.data.resize(1);
    ItemInfo info; facade._SoldItemList.AddItem(&info);
    for (int invalid : {-2147483647 - 1, -1, 1, 2147483647})
        assert(!facade.GetReputationTemp(invalid) && !facade.FindSoldItem(invalid));
    assert(facade.GetReputationTemp(0)->id == 7 && facade.FindSoldItem(0) == &info);
    puts("Drop record defaults, affix import/export boundaries and facade indices passed");
}
}
''')
    return "\n".join(parts)
