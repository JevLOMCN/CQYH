"""Actual facade record defaults, money cache bounds and constructed-string reloads."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "MMOFacade.h"))
    cpp = clean(source(game / "MMOFacade.cpp"))
    data = (game / "Inc/MMOGameClasses.h").read_bytes()
    generated = clean(data.decode("utf-16" if data.startswith(b"\xff\xfe") else "latin1").replace("\r\n", "\n"))
    role = clean(source(client / "Common/define/role_def.h"))
    macro = re.search(r"#define MAX_REPUTATION_AWARD_COUNT\s+[^\n]+", role).group()
    parts = [r'''
#pragma push_macro("MAX_REPUTATION_AWARD_COUNT")
#pragma push_macro("SCOPE_LOG_TIME")
#pragma push_macro("MoneyRatio")
#undef MAX_REPUTATION_AWARD_COUNT
#undef SCOPE_LOG_TIME
#undef MoneyRatio
#define SCOPE_LOG_TIME(...)
''', macro, re.search(r"#define MoneyRatio\s+[^\n]+", cpp).group(), r'''
namespace FacadeMoneyDefaultTests {
struct FString : AttributeRecordTests::FString {
    using AttributeRecordTests::FString::FString;
    FString& operator=(const char* text) { assert(magic == 0x9876abcd); assign(text); return *this; }
    INT Len() const { return (INT)size(); }
};
using FLOAT = float;
''']
    for name in ["EMONEY_TYPE", "ECHAR_MODIFIER_INDEX", "ESCENE_WEATHER_TYPE", "ESCENE_DAYTIME_TYPE",
                 "EActorClassType", "ENPC_QUALITY"]:
        parts.append(function(generated, "enum " + name + "\n") + ";")
    money_defs = clean(source(client / "Common/define/money_def.h"))
    parts.append(function(money_defs, "enum MONEY_TYPE\n") + ";")
    parts.append(re.search(r"static const INT64 MAX_MONEY\s*=\s*[^;]+;", money_defs).group())
    definitions = {}
    for name in ["StringMapVal", "ItemRandomAttr", "Player_Money", "AffixClientInfo", "HintConfig",
                 "TitleData", "TweenData", "MountData", "ReputationTemp"]:
        match = re.search(r"struct " + name + r"\s*\{", header).group()
        definitions[name] = function(header, match) + ";"
        parts.append(definitions[name])
    parts.append("#pragma pack(push, 1)")
    wire = clean(source(client / "Common/message/g2c_message.h"))
    for name in ["G2C_SYNC_MONEY", "G2C_MONEY_CHANGE"]:
        parts.append(function(wire, "struct " + name + " :") + ";")
    parts.append(r'''
#pragma pack(pop)
bool g_bSyncPlayerMoney = false;
void debugf(int, const char*, ...) {}
enum { NAME_Error = 1, MsgInfoType_Table = 2, M_SKIP_ROWS = 3 };
namespace MMOUI {
    struct UI { void delegateAddScreenMessage(const FString&, int, const char*) {} } ui;
    UI* GetUIManager() { return &ui; }
    FString ParseLocalizedPropertyPath(const char* path) { return path; }
}
struct CTabFile {
    bool hints = false;
    std::vector<std::vector<std::string>> hint_rows;
    std::vector<std::map<std::string, std::string>> named_rows;
    INT GetRowCount() const { return hints ? (INT)hint_rows.size() : (INT)named_rows.size() + M_SKIP_ROWS; }
    INT GetColCount() const { return 8; }
    const char* GetData(INT column, INT row) { return hint_rows.at(row).at(column).c_str(); }
    const char* GetData(const char* key, INT row) { return named_rows.at(row - M_SKIP_ROWS)[key].c_str(); }
};
INT appAtoi(const char* value) { return atoi(value); }
struct MMOFacade {
    Player_Money _PlayerMoney;
    HintConfig NpcHintInfo[EACT_MAX], NpcQualityHintInfo[E_nq_MAX];
    std::map<UINT, ItemRandomAttr> RandomAttributesMap;
    std::vector<std::vector<std::string>> hint_rows;
    std::vector<std::map<std::string, std::string>> attribute_rows;
    bool available = true;
    INT notifications = 0;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    UBOOL ReadConfigString(CTabFile*& result, const char* filename, BOOL) {
        if (!available) return FALSE;
        result = new CTabFile;
        result->hints = !strcmp(filename, "hint.txt");
        result->hint_rows = hint_rows; result->named_rows = attribute_rows;
        return TRUE;
    }
    static FString ClientEnum2ServerEnum(const FString& text) { return text; }
    INT FindClientEnumValue(const FString& text, const char*) {
        if (text == "E_cmi_A") return E_cmi_A;
        if (text == "E_cmi_AllNpcSpecies") return E_cmi_AllNpcSpecies;
        if (text == "rain") return E_swt_Rain;
        if (text == "night") return E_sdt_Night;
        return 0;
    }
    FString FindClientEnumString(const char* text, const char*, INT) { return text; }
    void NotifyPlayerMoneyChange(EMONEY_TYPE, INT64, INT64) { ++notifications; }
    INT32 GetInt32(INT64);
    void SetPlayerMoney(G2C_SYNC_MONEY);
    void SetPlayerMoney(G2C_MONEY_CHANGE);
    void SetPlayerMoney(EMONEY_TYPE, INT64);
    INT32 GetPlayerMoney_Weekly(EMONEY_TYPE);
    INT32 GetPlayerMoney_WeeklyLimit(EMONEY_TYPE);
    INT32 GetPlayerMoney(EMONEY_TYPE);
    INT64 GetPlayerMoney64(EMONEY_TYPE);
    HintConfig* GetHintInfo(EActorClassType, ENPC_QUALITY);
    void ReadHintNode(HintConfig&, FString, INT, INT, INT, INT);
    UBOOL ReadHint();
    UBOOL ReadRandomAttribute();
    void ResetCachedMoney();
};
''')
    for signature in ["INT32 MMOFacade::GetInt32(", "void MMOFacade::SetPlayerMoney( G2C_SYNC_MONEY",
                      "void MMOFacade::SetPlayerMoney( G2C_MONEY_CHANGE", "void MMOFacade::SetPlayerMoney( EMONEY_TYPE",
                      "INT32 MMOFacade::GetPlayerMoney_Weekly(", "INT32 MMOFacade::GetPlayerMoney_WeeklyLimit(",
                      "INT32 MMOFacade::GetPlayerMoney(", "INT64 MMOFacade::GetPlayerMoney64(",
                      "HintConfig*  MMOFacade::GetHintInfo(",
                      "void  MMOFacade::ReadHintNode(HintConfig& NpcHint,  FString",
                      "UBOOL MMOFacade::ReadHint()", "UBOOL MMOFacade::ReadRandomAttribute()"]:
        parts.append(function(cpp, signature))
    reset = re.search(r"appMemzero\(&_PlayerMoney,\s*[^;]+;", cpp).group()
    parts.append("void MMOFacade::ResetCachedMoney() {" + reset + "}")
    parts.append("void CheckMoneyZero(const Player_Money& value) {")
    money_arrays = re.findall(r"(?m)^\s*INT(?:32|64)\s+(\w+)\[[^\]]+\]\s*;", definitions["Player_Money"])
    parts.extend("for (auto item : value." + field + ") assert(!item);" for field in money_arrays)
    parts.append("assert(!value.byUseMoneytype01Only);\n}")
    parts.append("void RecordTests() {")
    for name, definition in definitions.items():
        fields = re.findall(r"(?m)^\s*(?:INT|INT32|BOOL|FLOAT)\s+(\w+)\s*;", definition)
        arrays = re.findall(r"(?m)^\s*INT\s+(\w+)\[[^\]]+\]\s*;", definition)
        strings = re.findall(r"(?m)^\s*FString\s+(\w+)\s*;", definition)
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& value) {" +
                     " ".join("assert(!value." + field + ");" for field in fields) +
                     " ".join("for (auto item : value." + field + ") assert(!item);" for field in arrays) +
                     " ".join("assert(value." + field + ".empty()); value." + field + ".assign(2000, 'x');" for field in strings) + "});")
    parts.append(r'''
    TipsStallTests::Poisoned<Player_Money>([](auto& value) { CheckMoneyZero(value); });
    TipsStallTests::Poisoned<ItemRandomAttr>([](auto& value) {
        assert(value.formatType == E_cmi_Invalid && value.weatherType == E_swt_Invalid && value.dayType == E_sdt_Total);
    });
}
void MoneyTests() {
    MMOFacade facade;
    memset(&facade._PlayerMoney, 0xa5, sizeof(facade._PlayerMoney));
    facade.ResetCachedMoney(); CheckMoneyZero(facade._PlayerMoney);
    for (int invalid : {-2147483647 - 1, -1, int(E_MONEYTYPE_COUNT), MAXINT}) {
        auto type = (EMONEY_TYPE)invalid; int old = facade.notifications;
        facade.SetPlayerMoney(type, 123);
        G2C_MONEY_CHANGE update = {}; update.nMoneyType = invalid; update.llMoney = 123;
        facade.SetPlayerMoney(update);
        assert(facade.notifications == old);
        assert(!facade.GetPlayerMoney(type) && !facade.GetPlayerMoney64(type));
        assert(!facade.GetPlayerMoney_Weekly(type) && !facade.GetPlayerMoney_WeeklyLimit(type));
        CheckMoneyZero(facade._PlayerMoney);
    }
    G2C_SYNC_MONEY full = {};
    for (int i = 0; i < MONEYTYPE_COUNT; ++i) {
        full.allMoneys[i] = 10000 + i; full.allMoneysLimit[i] = 20000 + i;
        full.anDailyEarned[i] = 100 + i; full.anDailyUsed[i] = 200 + i;
        full.anDailyEarnedLimit[i] = 300 + i; full.anDailyUsedLimit[i] = 400 + i;
        full.anWeeklyEarned[i] = 5000 + i; full.anWeeklyUsed[i] = 6000 + i;
        full.anWeeklyEarnedLimit[i] = 7000 + i; full.anWeeklyUsedLimit[i] = 8000 + i;
    }
    facade.SetPlayerMoney(full);
    for (int i = 0; i < MONEYTYPE_COUNT; ++i) {
        assert(facade.GetPlayerMoney64((EMONEY_TYPE)i) == 10000 + i);
        assert(facade._PlayerMoney.anDailyUsed[i] == 200 + i && facade._PlayerMoney.anWeeklyUsed[i] == 6000 + i);
        assert(facade._PlayerMoney.allMoneysLimit[i] == 20000 + i && facade._PlayerMoney.anWeeklyUsedLimit[i] == 8000 + i);
    }
    G2C_MONEY_CHANGE update = {}; update.nMoneyType = E_MONEYTYPE_COUNT - 1;
    update.llMoney = 123456; update.nDailyUsed = 321; update.nWeeklyUsed = 654;
    facade.SetPlayerMoney(update);
    assert(facade._PlayerMoney.anDailyUsed[update.nMoneyType] == 321);
    assert(facade._PlayerMoney.anWeeklyUsed[update.nMoneyType] == 654);
    assert(facade.GetPlayerMoney64((EMONEY_TYPE)update.nMoneyType) == 123456);
    facade.ResetCachedMoney(); CheckMoneyZero(facade._PlayerMoney);
    puts("Money cache full-size reset, negative/type limits, sync/change counters and record defaults passed");
}
void ReloadTests() {
    auto& facade = MMOFacade::getSingleton();
    for (int invalid : {-2147483647 - 1, -1, int(EACT_MAX), MAXINT})
        assert(!facade.GetHintInfo((EActorClassType)invalid, E_nq_Normal1));
    for (int invalid : {-2147483647 - 1, -1, int(E_nq_MAX), MAXINT})
        assert(!facade.GetHintInfo(EACT_Monster, (ENPC_QUALITY)invalid));
    for (int pass = 0; pass < 4; ++pass) {
        for (auto& value : facade.NpcHintInfo) { value.NpcTypeName.assign(4000, 'x'); value.NameVisible = 123; }
        for (auto& value : facade.NpcQualityHintInfo) { value.NpcTypeName.assign(4000, 'y'); value.NameVisible = 456; }
        facade.hint_rows.assign(2, std::vector<std::string>(5, "0"));
        assert(facade.ReadHint());
        for (auto& value : facade.NpcHintInfo) assert(value.NpcTypeName.magic == 0x9876abcd && value.NpcTypeName.empty() && !value.NameVisible);
        for (auto& value : facade.NpcQualityHintInfo) assert(value.NpcTypeName.magic == 0x9876abcd && value.NpcTypeName.empty() && !value.NameVisible);
    }
    facade.hint_rows.assign(14, std::vector<std::string>(5, "1"));
    for (int i = 0; i < 14; ++i) facade.hint_rows[i][0] = "name" + std::to_string(i);
    assert(facade.ReadHint());
    assert(facade.GetHintInfo(EACT_Monster, E_nq_Normal1)->NpcTypeName == "name1");
    assert(facade.GetHintInfo(EACT_NPC, E_nq_Normal1)->NameVisible == 1);
    facade.attribute_rows = {
        {{"Key", "cmi_A"}, {"Param1", "attr"}, {"Param2", "123"}, {"Pow", "456"}, {"Tip", "first"}, {"AttributeID", "1"}},
        {{"Key", "cmi_AllNpcSpecies"}, {"Param2", "234"}, {"Pow", "567"}, {"Tip", "second"},
         {"WeatherCond", "rain"}, {"DaytimeCond", "night"}, {"AttributeID", "2"}}
    };
    for (int pass = 0; pass < 4; ++pass) {
        assert(facade.ReadRandomAttribute() && facade.RandomAttributesMap.size() == 2);
        const auto& first = facade.RandomAttributesMap.at(1); const auto& second = facade.RandomAttributesMap.at(2);
        assert(first.formatType == E_cmi_A && first.attrType == "attr" && first.strText == "first");
        assert(first.attrType.magic == 0x9876abcd && first.value == 123 && first.power == 456);
        assert(first.weatherType == E_swt_Invalid && first.dayType == E_sdt_Total);
        assert(second.attrType.empty() && second.strText == "second" && second.value == 234);
        assert(second.weatherType == E_swt_Rain && second.dayType == E_sdt_Night);
    }
    puts("Hint index bounds, constructed-string hint/attribute reloads and real table rows passed");
}
}
#pragma pop_macro("MoneyRatio")
#pragma pop_macro("SCOPE_LOG_TIME")
#pragma pop_macro("MAX_REPUTATION_AWARD_COUNT")
''')
    return "\n".join(parts)
