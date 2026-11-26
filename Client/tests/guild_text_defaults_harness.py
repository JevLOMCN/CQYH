"""Production guild text packets, short quest packets and guild record defaults."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    guild = clean(source(game / "net/message_handler_Guild.cpp"))
    quest = clean(source(game / "net/message_handler_Quest.cpp"))
    constants = "\n".join(clean(source(client / "Common/define" / name))
                          for name in ["string_def.h", "guild_def.h", "quest_def.h"])
    macros = ["GUILD_NAME_LEN", "GUILD_ANNOUNCE_LEN", "GUILD_NOTICE_LEN", "GUILD_DUNGEON_AWARD_NUM",
              "MAX_REWARD_QUEST_COUNT_PER_ROLE"]
    parts = []
    for name in macros:
        parts.extend(['#pragma push_macro("' + name + '")', "#undef " + name,
                      re.search(r"#define " + name + r"\s+[^\n]+", constants).group()])
    parts.append(r'''
#pragma push_macro("UTF8_TO_TCHAR")
#undef UTF8_TO_TCHAR
#define UTF8_TO_TCHAR(text) (text)
namespace GuildTextDefaultTests {
using FString = AttributeRecordTests::FString;
using ANSICHAR = char;
template<class T> using SingletonMeyers = SkillQuestTests::SingletonMeyers<T>;
INT forced_length = -2;
struct FTCHARToUTF8 {
    const char* text;
    explicit FTCHARToUTF8(const char* value) : text(value) {}
    INT Length() const { return forced_length == -2 ? (INT)strlen(text) : forced_length; }
    operator const char*() const { return text; }
};
enum { c2g_guild_create = 1, c2m_guild_notice_change = 2, c2m_guild_announce_change = 3 };
#pragma pack(push, 1)
''')
    wire = "\n".join(clean(source(client / "Common/message" / filename)) for filename in [
        "c2g_message.h", "c2m_message.h", "m2c_message.h", "g2c_message.h"])
    for name in ["C2G_GUILD_CREATE", "C2M_GUILD_NOTICE_CHANGE", "C2M_GUILD_ANNOUNCE_CHANGE",
                 "M2C_GUILD_NOTICE_CHANGE", "G2C_SYNC_SABAC_ENROLL_LIST"]:
        parts.append(function(wire, "struct " + name + " :") + ";")
    quest_names = ["g2c_accept_quest", "g2c_abandon_quest", "g2c_complete_quest", "g2c_notify_next_circle_quest",
                   "g2c_share_quest", "g2c_buy_daily_quest_complete_time", "g2c_brush_daily_quest", "g2c_quest_recycle",
                   "g2c_set_quest_failed"]
    quest_bodies = [function(quest, "void CMessageHandler::Parse_" + name + "(") for name in quest_names]
    quest_types = [re.search(r"\b(\w+)\* msg\s*=", body).group(1) for body in quest_bodies]
    for name in quest_types:
        parts.append(function(wire, "struct " + name + " :") + ";")
    parts.append(r'''
#pragma pack(pop)
struct MMOGuildManager : SingletonMeyers<MMOGuildManager> {
    int notices = 0, enrollments = 0, records = 0;
    std::string notice;
    void NotifyUpdateGuildNotice(const char* text) { notice = text; ++notices; }
    void NotifyUpdateSabacEnrollList(const G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA* data, size_t count) {
        for (size_t i = 0; i < count; ++i) assert(data[i].dwGuildID == 100 + i);
        records += (int)count; ++enrollments;
    }
};
int quest_notifications = 0;
struct MMOQuestManager : SingletonMeyers<MMOQuestManager> {
    template<class... Args> void NotifyAcceptQuest(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyAbandonQuest(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyCompleteQuest(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyNextCircleQuest(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyShareQuest(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyUpgradeRewardQuestLimit(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyRefreshRewardQuestList(Args...) { ++quest_notifications; }
    template<class... Args> void NotifyQuestFailed(Args...) { ++quest_notifications; }
};
struct UMMOUIQuestItemCollectionPanel { void Show(BOOL) { ++quest_notifications; } } panel;
namespace MMOUI {
    struct UI { int Panel_QuestItemCollection = 1; } ui;
    UI* GetUIManager() { return &ui; }
    UMMOUIQuestItemCollectionPanel* FindUIPanel(int) { return &panel; }
}
template<size_t Capacity> struct Handler {
    char ms_MsgBuffer[Capacity];
    std::vector<char> sent;
    int sends = 0;
    void _Send(const void* data, size_t bytes) {
        assert(bytes <= Capacity); sent.assign((const char*)data, (const char*)data + bytes); ++sends;
    }
    void Send_CreateGuild(const FString&, const FString&);
    void Send_GuildNoticeChange(const FString&);
    void Send_GuildAnnounceChange(const FString&);
    void Parse_m2c_guild_notice_change(const char*, UINT32);
    void Parse_g2c_sync_sabac_enroll_list(const char*, UINT32);
''')
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name in quest_names)
    parts.append("};")
    for signature in ["void CMessageHandler::Send_CreateGuild(", "void CMessageHandler::Send_GuildNoticeChange(",
                      "void CMessageHandler::Send_GuildAnnounceChange(", "void CMessageHandler::Parse_m2c_guild_notice_change(",
                      "void CMessageHandler::Parse_g2c_sync_sabac_enroll_list("]:
        parts.append("template<size_t Capacity>\n" + function(guild, signature).replace("CMessageHandler::", "Handler<Capacity>::"))
    parts.extend("template<size_t Capacity>\n" + body.replace("CMessageHandler::", "Handler<Capacity>::") for body in quest_bodies)
    parts.append(r'''
template<size_t Capacity> void SendTests() {
    Handler<Capacity> handler;
    for (int operation = 0; operation < 3; ++operation) {
        int limit = operation == 1 ? GUILD_NOTICE_LEN : GUILD_ANNOUNCE_LEN;
        size_t header = operation == 0 ? sizeof(C2G_GUILD_CREATE) : sizeof(FLEX_MESSAGE_HEADER);
        auto send = [&](const FString& text) {
            if (operation == 0) handler.Send_CreateGuild(FString("name"), text);
            else if (operation == 1) handler.Send_GuildNoticeChange(text);
            else handler.Send_GuildAnnounceChange(text);
        };
        for (int count : {0, 1, 2, limit - 1, limit, limit + 1}) {
            FString text(count, 'x');
            int old = handler.sends; memset(handler.ms_MsgBuffer, 0xa5, sizeof(handler.ms_MsgBuffer));
            send(text);
            bool fits = count < limit && count + header + 1 <= Capacity;
            assert(handler.sends == old + fits);
            if (!fits) continue;
            auto* packet = (FLEX_MESSAGE_HEADER*)handler.sent.data();
            assert(packet->wSize == header + count + 1 && packet->wMsg == operation + 1);
            assert(!memcmp(handler.sent.data() + header, text.c_str(), count + 1));
            if (operation == 0) {
                auto* create = (C2G_GUILD_CREATE*)handler.sent.data();
                assert(!strcmp(create->szGuildName, "name"));
                for (size_t i = 5; i < sizeof(create->szGuildName); ++i) assert(!create->szGuildName[i]);
            }
        }
        FString utf8(u8"é测试"); int old = handler.sends; send(utf8);
        bool fits = utf8.size() + header + 1 <= Capacity;
        assert(handler.sends == old + fits);
        if (fits) assert(!memcmp(handler.sent.data() + header, utf8.c_str(), utf8.size() + 1));
        for (int length : {-2147483647 - 1, -1, MAXINT}) {
            forced_length = length; old = handler.sends; send(FString("short")); assert(handler.sends == old);
        }
        forced_length = -2;
    }
}
void PacketTests() {
    SendTests<512>(); SendTests<8>();
    Handler<512> handler; auto& guild = MMOGuildManager::getSingleton();
    handler.Parse_m2c_guild_notice_change(nullptr, 100);
    for (UINT32 count = 0; count <= GUILD_NOTICE_LEN + 1; ++count) {
        UINT32 total = sizeof(M2C_GUILD_NOTICE_CHANGE) + count;
        std::vector<char> bytes(total, 'x'); auto* packet = (M2C_GUILD_NOTICE_CHANGE*)bytes.data();
        packet->wSize = (UINT16)total; int old = guild.notices;
        handler.Parse_m2c_guild_notice_change(bytes.data(), total);
        assert(guild.notices == old + (count <= GUILD_NOTICE_LEN));
        if (count <= GUILD_NOTICE_LEN) assert(guild.notice == std::string(count, 'x'));
        packet->wSize = total + 1; old = guild.notices;
        handler.Parse_m2c_guild_notice_change(bytes.data(), total); assert(guild.notices == old);
        packet->wSize = sizeof(M2C_GUILD_NOTICE_CHANGE) - 1;
        handler.Parse_m2c_guild_notice_change(bytes.data(), total); assert(guild.notices == old);
    }
    for (UINT32 actual = sizeof(G2C_SYNC_SABAC_ENROLL_LIST); actual < 32; ++actual) {
        for (UINT16 declared = 0; declared <= actual + 1; ++declared) {
            std::vector<char> bytes(actual);
            auto* packet = (G2C_SYNC_SABAC_ENROLL_LIST*)bytes.data(); packet->wSize = declared;
            size_t capacity = (actual - sizeof(*packet)) / sizeof(packet->EnrollList[0]);
            for (size_t i = 0; i < capacity; ++i) packet->EnrollList[i].dwGuildID = 100 + (UINT)i;
            int old = guild.enrollments, oldRecords = guild.records;
            handler.Parse_g2c_sync_sabac_enroll_list(bytes.data(), actual);
            bool valid = declared >= sizeof(*packet) && declared <= actual &&
                (declared - sizeof(*packet)) % sizeof(packet->EnrollList[0]) == 0;
            assert(guild.enrollments == old + valid);
            if (valid) assert(guild.records == oldRecords + (declared - sizeof(*packet)) / sizeof(packet->EnrollList[0]));
        }
    }
''')
    for name, packet in zip(quest_names, quest_types):
        parts.append("{ auto read = &Handler<512>::Parse_" + name + "; " + packet + " full = {}; int old = quest_notifications;" +
                     "(handler.*read)(nullptr, sizeof(full));" +
                     "for (UINT32 size = 0; size < sizeof(full); ++size) {" +
                     "std::vector<char> bytes(size ? size : 1); (handler.*read)(bytes.data(), size); }" +
                     "assert(quest_notifications == old); (handler.*read)((const char*)&full, sizeof(full));" +
                     "assert(quest_notifications == old + 1); }")
    parts.append('puts("Guild text byte/buffer limits, initialized name tails, short notices/enrollment records and nine quest readers passed");\n}')
    data = clean(source(game / "MMOGuildData.h"))
    definitions = {}
    for name in ["GuildBuildingData", "GuildBuildingInfo", "GuildWelfareData", "GuildWelfareUpgradeInfo", "GuildWelfareInfo", "GuildDungeonData"]:
        definitions[name] = function(data, "struct " + name + "\n") + ";"
        parts.append(definitions[name])
    parts.append("void RecordTests() {")
    for name, definition in definitions.items():
        fields = re.findall(r"(?m)^\s*UINT(?:8|32|64)\s+(\w+)\s*;", definition)
        strings = re.findall(r"(?m)^\s*FString\s+(\w+)\s*;", definition)
        arrays = re.findall(r"(?m)^\s*UINT32\s+(\w+)\[[^\]]+\]\s*;", definition)
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& value) {" +
                     " ".join("assert(!value." + field + ");" for field in fields) +
                     " ".join("assert(value." + field + ".empty()); value." + field + ".assign(2000, 'x');" for field in strings) +
                     " ".join("for (auto item : value." + field + ") assert(!item);" for field in arrays) + "});")
    parts.append('puts("Guild building/welfare/dungeon record defaults and constructed strings passed");\n}\n}')
    parts.append('#pragma pop_macro("UTF8_TO_TCHAR")')
    parts.extend('#pragma pop_macro("' + name + '")' for name in reversed(macros))
    return "\n".join(parts)
