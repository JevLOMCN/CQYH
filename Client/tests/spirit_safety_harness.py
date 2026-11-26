"""Actual spirit wire layouts, readers and slot storage under forced relocation."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    cpp = clean(source(game / "Src/MMOHeavenManager.cpp"))
    header = clean(source(game / "Src/MMOHeavenManager.h"))
    network = clean(source(game / "net/message_handler_Heaven.cpp"))
    wire = clean(source(client / "Common/message/g2c_message.h"))
    names = re.findall(r"void CMessageHandler::Parse_(\w+)\(const char\* buf, UINT32 size\)", network)
    if len(names) != 9:
        raise ValueError("Spirit packet coverage changed")
    parsers = [function(network, "void CMessageHandler::Parse_" + name + "(") for name in names]
    types = [re.search(r"\b(\w+)\* msg\s*=", body).group(1) for body in parsers]
    parts = [r'''
#include <functional>
namespace SpiritSafetyTests {
struct FString : std::string {
    using std::string::string;
    using std::string::operator=;
    FString(std::string value) : std::string(value) {}
};
FString appItoa(int number) { return FString(std::to_string(number)); }
template<class T> struct TArray {
    std::vector<T> data;
    int Num() const { return (int)data.size(); }
    void Empty() { std::vector<T>().swap(data); }
    void AddItem(const T& value) {
        auto relocated = data; relocated.push_back(value); data.swap(relocated);
    }
    T& operator()(int index) { return data.at(index); }
    const T& operator()(int index) const { return data.at(index); }
};
''']
    spirit = clean(source(client / "Common/define/spirit_def.h"))
    for name in ["SPIRIT_OPEN_LEVEL", "MAX_AIRSEA_BAG_CONTAINER_SIZE", "MAX_AIRSEA_STALL_CONTAINER_SIZE",
                 "MAX_AIRSEA_EQUIP_CONTAINER_SIZE", "MAX_BLOODSEA_PAGE", "MAX_BLOODSEA_SLOT_PER_PAGE", "MAX_AIR_PROCEDURE"]:
        value = re.search(r"#define\s+" + name + r"\s+([^\n]+)", spirit).group(1)
        parts.append("enum { " + name + " = " + value + " };")
    generated = (game / "Inc/MMOGameUIClasses.h").read_bytes()
    generated = generated.decode("utf-16" if generated.startswith(b"\xff\xfe") else "latin1")
    for name in ["UCONST_GasBagSlotMaxNum", "UCONST_GasBankSlotMaxNum", "UCONST_GasEquipSlotMaxNum"]:
        value = re.search(r"#define " + name + r"\s+([^\r\n]+)", generated).group(1)
        parts.append("enum { " + name + " = " + value + " };")
    parts.append(function(spirit, "enum SPIRIT_AIR_SEA_BAG_TYPE") + ";")
    for name in ["HeavenConvertType", "HeavenGasOperateType"]:
        parts.append(function(header, "enum " + name) + ";")
    parts.append("#pragma pack(push, 1)")
    version = re.search(r"using namespace ROLE_DB_DEF_V_(\d+)", source(client / "Common/define/role_db_def.h")).group(1)
    db = clean(source(client / ("Common/define/role_db_def_v_" + version + ".h")))
    parts.append(function(db, "struct ROLE_DB_SPIRIT_DATA\n") + ";")
    parts.extend(function(wire, "struct " + name + " :") + ";" for name in types)
    parts.append("#pragma pack(pop)")
    for name in ["GasItem", "GasSlot", "BloodSlot"]:
        parts.append(function(header, "struct " + name + "\n") + ";")
    parts.append(r'''
enum { adSpiritLevel, adSpiritBuyAirEquipSlotCount, adSpiritBuyAirBagSlotCount, adSpiritBuyAirStallSlotCount,
       adSpiritBloodPageCompleteCount, adSpiritBloodPageLevel, adSpiritBloodPageCount,
       EGUET_OpenGas, EGUET_EquipGas, PowerPart_HeavenPow, ECVT_Quest, GQT_Soul, GQT_Heaven,
       MsgInfoType_Table };
struct MMOGameEvent_Achievement { MMOGameEvent_Achievement(int, int, int, int) {} };
struct MMOGameEvent_Guide { explicit MMOGameEvent_Guide(int) {} };
struct MMOGameEvent_Heaven {
    enum Kind { EHeavenEvent_SynData, EHeavenEvent_UpdateGasSlot, EHeavenEvent_BloodStateUpdate,
                EHeavenEvent_BloodStateOpen, EHeavenEvent_PractisePhrase };
    INT SlotContainerID = 0, SlotID = 0, nValue = 0;
    explicit MMOGameEvent_Heaven(Kind) {}
};
std::function<void()> on_event;
namespace MMOUI {
int events = 0;
template<class T> void PostGameEvent(T*) { ++events; if (on_event) on_event(); }
FString ParseLocalizedPropertyPath(const FString& text) { return text; }
struct UI { void delegateAddScreenMessage(const FString&, int, const char*) { ++events; } };
UI* GetUIManager() { static UI value; return &value; }
}
struct Attributes {
    struct { int heavenLevel = 0, heavenExp = 0; } heavenAttributes;
    struct { int Level = 10; } LevelAttributes;
};
struct AMMOPlayer { Attributes* ActorAttributes = nullptr; };
struct MMOFacade {
    AMMOPlayer* player = nullptr;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    AMMOPlayer* GetMainPlayer() { return player; }
};
struct MMOPowerManager {
    int calls = 0;
    static MMOPowerManager& getSingleton() { static MMOPowerManager value; return value; }
    void UpdatePartPower(int) { ++calls; }
};
struct MMOQuestManager {
    int calls = 0;
    static MMOQuestManager& getSingleton() { static MMOQuestManager value; return value; }
    void UpdateClientVariable(int, int) { ++calls; }
};
using GasItemList = std::list<GasItem>;
struct MMOHeavenManager {
    int calls = 0;
    int nCurHeavenExp = 0, nHeavenLevel = 0, nCostMoneyAll = 0, nDailyExp2PtsTime = 0;
    int gasPractiseState = 0, curMaxID = 0, bLevelLimited = 0, bServerWaiting = 0;
    UINT32 dwLastExp2PtsTime = 0;
    int nBloodState[MAX_BLOODSEA_PAGE] = {};
    TArray<GasSlot> arrSlotGasBag, arrSlotGasEquip, arrSlotGasBank;
    TArray<BloodSlot> arrSlotBlood;
    TArray<FString> converterLogLookUp;
    GasItemList myGasItemList;
    std::vector<int> gasItemTemplateMap, heavenConvertList, heavenLevelDataMap, heavenAttList;
    GasSlot* pkTargetGasSlot = nullptr;
    static MMOHeavenManager& getSingleton() { static MMOHeavenManager value; return value; }
    void Clear();
    void NotifyUpdateHeavenData(const ROLE_DB_SPIRIT_DATA*);
    void NotifyOpenGasSlot(INT, INT);
    void NotifyUpdateBloodState(INT, INT, INT);
    void NotifyBloodOpen(INT);
    void NotifyUpdatePractiseBtnIndex(INT);
    GasSlot* GetSlotByIndex(INT, INT);
    const GasSlot* GetTargetSlot() const;
    BloodSlot* GetBloodSlotByIndex(INT, INT);
    INT GetBtnPractiseState(INT);
    INT GenerateUniqueID();
    void AddNewGas(GasItem&);
    void CheckSetNeedTime(BOOL) { ++calls; }
    void NotifyUpdateHeavenExp(INT) { ++calls; }
    void NotifyUpdateHeavenLevel(INT) { ++calls; }
    void NotifyUpdateGasChange(INT, INT, INT, INT) { ++calls; }
    void ParseHeavenError(INT, INT, INT, FString&) { ++calls; }
    INT GetConvertType() { return HeavenConvertType_JustConvert; }
    INT GetGasOperateType() { return HeavenGasOperateType_Practise; }
};
struct CMessageHandler {
''')
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name in names)
    parts.append("};")
    parts.extend(parsers)
    for signature in [
        "void MMOHeavenManager::Clear()", "void MMOHeavenManager::NotifyUpdateHeavenData(",
        "void MMOHeavenManager::NotifyOpenGasSlot(", "void MMOHeavenManager::NotifyUpdateBloodState(",
        "void MMOHeavenManager::NotifyBloodOpen(", "void MMOHeavenManager::NotifyUpdatePractiseBtnIndex(",
        "GasSlot* MMOHeavenManager::GetSlotByIndex(", "BloodSlot* MMOHeavenManager::GetBloodSlotByIndex(",
        "const GasSlot* MMOHeavenManager::GetTargetSlot()const", "INT\tMMOHeavenManager::GetBtnPractiseState(",
        "INT MMOHeavenManager::GenerateUniqueID()", "void MMOHeavenManager::AddNewGas(",
    ]:
        parts.append(function(cpp, signature))
    parts.append(r'''
int Effects() {
    return MMOUI::events + MMOHeavenManager::getSingleton().calls
        + MMOPowerManager::getSingleton().calls + MMOQuestManager::getSingleton().calls;
}
using Parser = void (CMessageHandler::*)(const char*, UINT32);
void ShortPackets(CMessageHandler& handler, Parser parser, size_t minimum, void (*initialize)(char*) = nullptr) {
    int before = Effects();
    (handler.*parser)(nullptr, 0); (handler.*parser)(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < minimum; ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1);
        memset(bytes.get(), 0xff, size); (handler.*parser)(bytes.get(), size);
        assert(Effects() == before);
    }
    auto bytes = std::make_unique<char[]>(minimum); memset(bytes.get(), 0, minimum);
    if (initialize) initialize(bytes.get());
    (handler.*parser)(bytes.get(), (UINT32)minimum); assert(Effects() > before);
}
void Run() {
    CMessageHandler handler;
''')
    for name, kind in zip(names, types):
        minimum = "sizeof(" + kind + ")" + (" + sizeof(ROLE_DB_SPIRIT_DATA)" if name == "g2c_sync_spirit" else "")
        initialize = ""
        if name == "g2c_spirit_brush_slot":
            initialize = ", [](char* bytes) { auto* msg = (G2C_SPIRIT_BRUSH_SLOT*)bytes; msg->nPage = 1; msg->nIndex = 1; }"
        elif name == "g2c_spirit_slot_size_change":
            initialize = ", [](char* bytes) { auto* msg = (G2C_SPIRIT_SLOT_SIZE_CHANGE*)bytes; msg->nType = sasbtBag; msg->nSize = 1; }"
        parts.append("ShortPackets(handler, &CMessageHandler::Parse_" + name + ", " + minimum + initialize + ");")
    parts.append(r'''
    auto& manager = MMOHeavenManager::getSingleton();
    manager.arrSlotGasBag.Empty(); manager.arrSlotGasBank.Empty(); manager.arrSlotGasEquip.Empty();
    manager.pkTargetGasSlot = nullptr;
    for (int container : {sasbtBag, sasbtStall, sasbtEquip}) {
        for (int count : {-2147483647 - 1, -1, 0, 2147483647}) {
            int before = Effects(); manager.NotifyOpenGasSlot(container, count); assert(Effects() == before);
        }
        manager.NotifyOpenGasSlot(container, 1);
        assert(manager.GetSlotByIndex(container, 0));
        for (int index : {-2147483647 - 1, -1, 1, 2147483647}) assert(!manager.GetSlotByIndex(container, index));
    }
    for (int container : {-2147483647 - 1, -1, int(sasbtTotal), 2147483647}) {
        int before = Effects(); manager.NotifyOpenGasSlot(container, 1); assert(Effects() == before);
        assert(!manager.GetSlotByIndex(container, 0));
    }
    manager.pkTargetGasSlot = manager.GetSlotByIndex(sasbtBag, 0);
    manager.pkTargetGasSlot->nTagID = 0; manager.pkTargetGasSlot->nUniqueID = 123;
    on_event = [&] {
        assert(manager.GetTargetSlot() == manager.GetSlotByIndex(sasbtBag, 0));
        assert(manager.GetTargetSlot()->nTagID == 0 && manager.GetTargetSlot()->nUniqueID == 123);
    };
    for (int count = 2; count <= UCONST_GasBagSlotMaxNum; ++count) manager.NotifyOpenGasSlot(sasbtBag, count);
    assert(manager.arrSlotGasBag.Num() == UCONST_GasBagSlotMaxNum);
    on_event = nullptr;
    manager.Clear(); assert(!manager.GetTargetSlot());
    manager.pkTargetGasSlot = manager.GetSlotByIndex(sasbtBag, 0); manager.pkTargetGasSlot->nTagID = 0;
    const size_t full_size = sizeof(G2C_SYNC_SPIRIT) + sizeof(ROLE_DB_SPIRIT_DATA);
    auto data_bytes = std::make_unique<char[]>(full_size); memset(data_bytes.get(), 0, full_size);
    auto* message = (G2C_SYNC_SPIRIT*)data_bytes.get();
    auto* saved = (ROLE_DB_SPIRIT_DATA*)message->szData;
    saved->byLevel = 30; saved->nExp = 12345;
    saved->nBagContainerSize = UCONST_GasBagSlotMaxNum;
    saved->nStallContainerSize = UCONST_GasBankSlotMaxNum;
    saved->nEquipContainerSize = UCONST_GasEquipSlotMaxNum;
    saved->nBagContainerTemplate[MAX_AIRSEA_BAG_CONTAINER_SIZE - 1] = 101;
    saved->nStallContainerTemplate[MAX_AIRSEA_STALL_CONTAINER_SIZE - 1] = 102;
    saved->nEquipContainerTemplate[MAX_AIRSEA_EQUIP_CONTAINER_SIZE - 1] = 103;
    saved->nBloodSlot[MAX_BLOODSEA_PAGE - 1][MAX_BLOODSEA_SLOT_PER_PAGE - 1] = 99;
    Attributes attributes; AMMOPlayer player; player.ActorAttributes = &attributes;
    MMOFacade::getSingleton().player = &player;
    on_event = [&] { assert(!manager.GetTargetSlot()); };
    handler.Parse_g2c_sync_spirit(data_bytes.get(), (UINT32)full_size);
    assert(attributes.heavenAttributes.heavenLevel == 30 && attributes.heavenAttributes.heavenExp == 12345);
    assert(manager.myGasItemList.size() == 3 && manager.myGasItemList.back().nTempID == 103);
    assert(manager.arrSlotBlood.Num() == MAX_BLOODSEA_PAGE * MAX_BLOODSEA_SLOT_PER_PAGE);
    assert(manager.arrSlotBlood(manager.arrSlotBlood.Num() - 1).nValue == 99);
    player.ActorAttributes = nullptr;
    handler.Parse_g2c_sync_spirit(data_bytes.get(), (UINT32)full_size);
    MMOFacade::getSingleton().player = nullptr; on_event = nullptr;
    manager.gasPractiseState = -1;
    for (int index : {-2147483647 - 1, -1, int(MAX_AIR_PROCEDURE), 31, 32, 2147483647})
        assert(!manager.GetBtnPractiseState(index));
    for (int index = 0; index < MAX_AIR_PROCEDURE; ++index)
        assert(manager.GetBtnPractiseState(index) == (1 << index));
    puts("Spirit payload lengths, slot indices, growth/reload pointer lifetimes and shift bounds passed");
}
}
''')
    return "\n".join(parts)
