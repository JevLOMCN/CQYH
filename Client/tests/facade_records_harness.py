"""Real enum conversion, fixed player names/checksum, kill defaults and NPC path."""
import re
import struct
import zlib


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    cpp = clean(source(game / "MMOFacade.cpp"))
    header = clean(source(game / "MMOFacade.h"))
    strings = clean(source(client / "Common/define/string_def.h"))
    role_size = int(re.search(r"#define ROLE_NAME_LEN\s+\(?([0-9]+)", strings).group(1))
    guild_size = int(re.search(r"#define GUILD_NAME_LEN\s+\(?([0-9]+)", strings).group(1))
    wire_bytes = struct.pack("<HI", 0, 123) + b"R" * role_size + bytes([2, 1, 3]) + b"G" * guild_size
    crc = zlib.crc32(wire_bytes)
    parts = [r'''
#pragma push_macro("MAX_PATH")
#undef MAX_PATH
#define MAX_PATH 260
namespace FacadeRecordTests {
using TCHAR = char;
struct FString {
    std::string value;
    FString(const char* text = "") : value(text) {}
    INT Len() const { return (INT)value.size(); }
    const char* operator*() const { return value.c_str(); }
    FString& operator+=(char value) { if (value) this->value += value; return *this; }
};
const char* UTF8_TO_TCHAR(const char* text) { return text; }
template<class K, class V> struct TMap {
    std::map<K, V> values;
    V& FindOrAdd(K key) { return values[key]; }
};
''']
    parts.append("enum { ROLE_NAME_LEN = " + str(role_size) + ", GUILD_NAME_LEN = " + str(guild_size) + " };")
    parts.append(function(header, "struct PlayerBaseInfo\n") + ";")
    parts.append("#pragma pack(push, 1)")
    parts.append(function(clean(source(client / "Common/message/m2c_message.h")), "struct M2C_SYNC_ROLE_INFO :") + ";")
    parts.append("#pragma pack(pop)")
    parts.append(function(clean(source(game / "Inc/MMOUtil.h")), "template<size_t Size>"))
    parts.append(r'''
struct MMOGameEvent_PlayerBaseInfo { MMOGameEvent_PlayerBaseInfo(UINT32, PlayerBaseInfo*) {} };
namespace MMOUI { int events = 0; void PostGameEvent(MMOGameEvent_PlayerBaseInfo*) { ++events; } }
struct MMOFacade {
    TMap<INT, PlayerBaseInfo> _PlayerBaseInfoMap;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    PlayerBaseInfo* AddPlayerBaseInfo(M2C_SYNC_ROLE_INFO&);
    FString ClientEnum2ServerEnum(const FString&, INT);
};
struct RankListManager {
    int refreshes = 0;
    static RankListManager& getSingleton() { static RankListManager value; return value; }
    void NotifyReFreshList() { ++refreshes; }
};
struct CMessageHandler { void Parse_m2c_sync_role_info(const char*, UINT32); };
''')
    parts.append(function(cpp, "PlayerBaseInfo *MMOFacade::AddPlayerBaseInfo("))
    parts.append(function(cpp, "FString MMOFacade::ClientEnum2ServerEnum("))
    parts.append(function(clean(source(game / "net/message_handler.cpp")), "void CMessageHandler::Parse_m2c_sync_role_info("))
    parts.append(r'''
std::string ExpectedEnum(const std::string& name, int escapes) {
    std::string result = name.empty() ? "" : name.substr(1);
    for (int n = 0; n < escapes; ++n) {
        size_t underscore = result.find('_');
        if (underscore == std::string::npos) break;
        result.erase(underscore, 1);
    }
    return result;
}
void CheckEnum(const std::string& name) {
    for (int escapes : {-2147483647 - 1, -1, 0, 1, 2, 3, 2147483647}) {
        FString input(name.c_str());
        assert(MMOFacade::getSingleton().ClientEnum2ServerEnum(input, escapes).value == ExpectedEnum(name, escapes));
        assert(input.value == name);
    }
}
void FacadeTests() {
    TipsStallTests::Poisoned<PlayerBaseInfo>([](auto& value) {
        assert(!value._RoleID && !value._RoleClass && !value._Sex && !value._Level && !value._VIPType && !value._Crc);
        assert(value._RoleName.value.empty() && value._GuildName.value.empty());
    });
    for (const char* text : {"", "E", "E_", "E_NES_MAIN_HAND", "E_cls_Warrior", "X____a_b__c"}) CheckEnum(text);
    for (int length : {2, 126, 127, 128, 129, 1024, 65536}) {
        CheckEnum(std::string(length, 'x'));
        CheckEnum("E_" + std::string(length, '_') + "long_value");
    }
    const char* repository_enums[] = {
''')
    enum_names = set()
    for path in [game / "Inc/MMOGameClasses.h", game / "Inc/MMOGameUIClasses.h"]:
        data = path.read_bytes()
        data = data.decode("utf-16" if data.startswith(b"\xff\xfe") else "latin1")
        for body in re.findall(r"enum\s+\w+\s*\{([^}]+)\}", data):
            enum_names.update(re.findall(r"(?m)^\s*(\w+)\s*(?:=|,)", body))
    if not enum_names:
        raise ValueError("No generated client enums found")
    parts.append(",\n".join('"' + name + '"' for name in sorted(enum_names)))
    parts.append(r'''
    };
    for (const char* text : repository_enums) CheckEnum(text);
    auto packet = std::make_unique<M2C_SYNC_ROLE_INFO>();
    packet->wMsg = 0; packet->dwRoleID = 123; packet->byClass = 2; packet->bySex = 1; packet->byVIPLevel = 3;
    memset(packet->szRoleName, 'R', sizeof(packet->szRoleName));
    memset(packet->szGuildName, 'G', sizeof(packet->szGuildName));
    CMessageHandler handler;
    handler.Parse_m2c_sync_role_info(nullptr, 0); handler.Parse_m2c_sync_role_info(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(*packet); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1); memset(bytes.get(), 0xff, size);
        handler.Parse_m2c_sync_role_info(bytes.get(), size); assert(!MMOUI::events);
    }
    handler.Parse_m2c_sync_role_info((char*)packet.get(), sizeof(*packet));
    auto& info = MMOFacade::getSingleton()._PlayerBaseInfoMap.values.at(123);
    assert(info._RoleName.value == std::string(ROLE_NAME_LEN, 'R') && info._GuildName.value == std::string(GUILD_NAME_LEN, 'G'));
    assert(info._RoleID == 123 && info._RoleClass == 2 && info._Sex == 1 && info._VIPType == 3 && !info._Level);
''')
    parts.append("assert((UINT32)info._Crc == " + str(crc) + "u);")
    parts.append(r'''
    packet->szGuildName[0] = 'H'; MMOFacade::getSingleton().AddPlayerBaseInfo(*packet);
    assert((UINT32)info._Crc == crc32(0, (Bytef*)packet.get(), sizeof(*packet)));
    assert(MMOUI::events == 2 && RankListManager::getSingleton().refreshes == 1);
}
}
namespace KillDefaultsTests {
using FString = FacadeRecordTests::FString;
template<class T> struct SingletonMeyers { static T& getSingleton() { static T value; return value; } };
enum EGameEvent { GameEvent_KillRecord };
struct MMOGameEvent {};
''')
    kill_header = clean(source(game / "MMOKillRecordManager.h"))
    parts.append(function(kill_header, "struct MMOGameEvent_KillRecord :") + ";")
    parts.append("namespace MMOUI {\nstd::vector<MMOGameEvent_KillRecord> events;\n"
                 "void PostGameEvent(MMOGameEvent_KillRecord* value) { events.push_back(*value); }\n}")
    parts.append(function(kill_header, "class MMOKillRecordManager:").replace("private:", "public:") + ";")
    kill_cpp = clean(source(game / "MMOKillRecordManager.cpp"))
    for signature in ["void MMOKillRecordManager::Init()", "void MMOKillRecordManager::NotifyNewKillMonster(",
                      "void MMOKillRecordManager::NotifyNewKillPlayer(", "void MMOKillRecordManager::NotifyKillMonsterReward(",
                      "void MMOKillRecordManager::NotifyKillMonsterMomentaryReward("]:
        parts.append(function(kill_cpp, signature))
    kill_network = clean(source(game / "net/message_handler_KillRecord.cpp"))
    parsers = re.findall(r"void CMessageHandler::Parse_(\w+)\(const char\* buf, UINT32 size\)", kill_network)
    parts.append("#pragma pack(push, 1)")
    for name in parsers:
        parts.append(function(clean(source(client / "Common/message/g2c_message.h")),
                              "struct " + name.upper() + " :") + ";")
    parts.append("#pragma pack(pop)\nstruct CMessageHandler {")
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name in parsers)
    parts.append("};")
    parts.extend(function(kill_network, "void CMessageHandler::Parse_" + name + "(") for name in parsers)
    parts.append(r'''
void Run() {
    TipsStallTests::Poisoned<MMOKillRecordManager>([](auto& value) {
        assert(!value.nKillPlayerCount && !value.nKillPlayerRecord && !value.nKillMonsterCount);
        assert(!value.nKillMonsterRecord && !value.nKillMonsterMomentaryCount && !value.nKillMonsterMomentaryRecord);
        value.nKillPlayerRecord = value.nKillMonsterMomentaryCount = 99; value.Init();
        assert(!value.nKillPlayerRecord && !value.nKillMonsterMomentaryCount);
    });
    CMessageHandler handler;
''')
    for name in parsers:
        kind = name.upper()
        parts.append("{\nhandler.Parse_" + name + "(nullptr, 0);\n"
                     "for (UINT32 size = 0; size < sizeof(" + kind + "); ++size) {\n"
                     "auto bytes = std::make_unique<char[]>(size ? size : 1); memset(bytes.get(), 0xff, size);\n"
                     "const auto events = MMOUI::events.size(); handler.Parse_" + name + "(bytes.get(), size);\n"
                     "assert(MMOUI::events.size() == events); }\n"
                     + kind + " packet = {}; packet.byCount = 10;\n"
                     "const auto events = MMOUI::events.size(); handler.Parse_" + name + "((char*)&packet, sizeof(packet));\n"
                     "assert(MMOUI::events.size() == events + 1); }\n")
    parts.append(r'''
    puts("Kill-record constructor/reset defaults and four short-message readers passed");
}
}
namespace NpcPathTests {
using TCHAR = char;
struct FVector { FVector(int, int, int) {} };
struct FRotator { FRotator(int, int, int) {} };
enum { NAME_None = 0 };
struct AActor {};
struct ANPC : AActor { static int StaticClass() { return 1; } };
template<class T> T* Cast(AActor* value) { return static_cast<T*>(value); }
int loads = 0, spawns = 0;
struct UObject {
    static AActor* StaticLoadObject(int, void*, void*, const char*, int, void*) { ++loads; return nullptr; }
};
struct World {
    AActor* SpawnActor(int, int, FVector, FRotator, AActor*, bool) { ++spawns; return nullptr; }
} world;
World* GWorld = &world;
using NpcTemplateMap = std::map<int, AActor*>;
struct MMONpcFactory { ANPC* CreateNpc(INT); };
''')
    parts.append(function(clean(source(game / "Src/MMONpcFactory.cpp")), "ANPC* MMONpcFactory::CreateNpc("))
    parts.append(r'''
void Run() {
    MMONpcFactory factory;
    for (int type : {-2147483647 - 1, -1, 0, 1, 2, 3, 4, 5, 2147483647}) assert(!factory.CreateNpc(type));
    assert(!loads && !spawns);
    puts("Unset NPC resource paths rejected before loading");
}
}
#pragma pop_macro("MAX_PATH")
''')
    return "\n".join(parts)
