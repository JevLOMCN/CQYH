"""Run real chat and mail parsers with exact-size hostile wire buffers."""
import re


def harness(client, game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(path))

    mail = clean(game / "net/message_handler_Mail.cpp")
    chat = clean(game / "net/message_handler_Chat.cpp")
    wire = clean(client / "Common/message/m2c_message.h")
    parts = [r'''
namespace NetworkTests {
using CHAR = char;
struct FString : ::FStringA {
    using ::FStringA::FStringA;
    const char* operator*() const { return value.c_str(); }
    bool IsEmpty() const { return value.empty(); }
};
#define UTF8_TO_TCHAR(s) (s)
#ifndef ARRAY_COUNT
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#endif
#define ZeroMemory(p, n) memset(p, 0, n)
''', function(clean(game / "Inc/MMOUtil.h"), "template<size_t Size>")]
    strings = clean(client / "Common/define/string_def.h")
    parts.extend(re.search(r"#define " + name + r"\s+[^\n]+", strings).group()
                 for name in ["ROLE_NAME_LEN", "MAIL_SUBJECT_LEN", "MAIL_CONTENT_LEN",
                              "USER_NAME_LEN", "USER_PASSWORD_LEN", "GUILD_NAME_LEN"])
    friend = clean(client / "Common/define/friend_def.h")
    role = clean(client / "Common/define/role_def.h")
    parts.extend(re.search(r"#define " + name + r"\s+[^\n]+", friend).group()
                 for name in ["FRIEND_GROUP_NAME_LEN", "SIGNATURE_LEN"])
    parts.extend(re.search(r"#define " + name + r"\s+[^\n]+", role).group()
                 for name in ["MAX_USABLE_ROLE_PER_USER", "MAX_DELETE_ROLE_PER_USER", "MAX_ROLE_PER_USER"])
    parts.append(re.search(r"#define MAX_CHAT_MESSAGE_LEN\s+[^\n]+",
                           clean(client / "Common/define/chat_def.h")).group())
    parts.append(re.search(r"#define TEAM_NAME_LEN\s+[^\n]+", strings).group())
    parts.append(function(clean(client / "Common/define/item_def.h"), "enum NORMAL_EQUIP_SLOT") + ";")
    parts.append("namespace ExternalMessage {\n#pragma pack(push, 1)")
    parts.extend(function(wire, "struct " + name) + ";" for name in [
        "M2C_CHAT_MESSAGE", "BriefMailInfo", "M2C_MAIL_LIST_RECV", "M2C_MAIL_READ",
        "FRIEND_INFO", "FRIEND_INFO_WITH_GROUP", "M2C_FRIEND_LIST", "TEAM_MEMBER_BASEINFO", "M2C_TEAM_JOIN"])
    g2c = clean(client / "Common/message/g2c_message.h")
    parts.extend(function(g2c, "struct " + name) + ";" for name in [
        "G2C_CHAT_MESSAGE", "FRIEND_GROUP_INFO", "G2C_FRIEND_GROUP_LIST", "TITLE_DATA", "G2C_SYNC_TITLE",
        "G2C_SYNC_SHIELD_LIST", "ADJACENT_TEAM_ITEM", "G2C_SYNC_ADJACENT_TEAM"])
    t2c = clean(client / "Common/message/t2c_message.h")
    parts.extend(function(t2c, "struct " + name) + ";" for name in [
        "T2C_LOGIN_ACK", "LOGIN_ROLE_DATA", "T2C_SYNC_ROLE_LIST"])
    parts.append(function(clean(client / "Common/message/c2m_message.h"), "struct C2M_FRIEND_CHAT") + ";")
    parts.append("#pragma pack(pop)\n}\nusing namespace ExternalMessage;")
    header = clean(game / "MailManager.h")
    parts.extend(function(header, "struct " + name) + ";"
                 for name in ["ClientBriefMailInfo", "ClientMailDetail"])
    parts.append(r'''
enum { MAX_MAIL_LIST_COUNT = 10 };
struct DialogContext { FString TempText; void Clear() { TempText.Empty(); } };
struct ParseContext {};
struct UITextParser {
    static UITextParser& getSingleton() { static UITextParser value; return value; }
    void Parse(ParseContext&, DialogContext& context, const char* text) { context.TempText = text; }
};
struct MailManager {
    std::vector<ClientBriefMailInfo> brief;
    std::vector<ClientMailDetail> detail;
    INT notifications = 0;
    bool check = false;
    static MailManager& getSingleton() { static MailManager manager; return manager; }
    void AddOrUpdateMail(BriefMailInfo* info, FString subject) {
        ClientBriefMailInfo entry;
        *static_cast<BriefMailInfo*>(&entry) = *info;
        entry.mailSubject = subject;
        brief.push_back(entry);
    }
    void NotifyUpdateMailList() { ++notifications; }
    void SetCheckMail(BOOL value) { check = value != 0; }
    void ReadMail(ClientMailDetail value) { detail.push_back(value); }
};
struct MMOChatManager {
    INT calls = 0;
    std::string text, sender;
    UINT32 target = 0, source = 0, gm = 0;
    static MMOChatManager& getSingleton() { static MMOChatManager manager; return manager; }
    void ReceiveMessage(UINT32, INT, UINT32, FString content, FString name,
                        UINT32 from, UINT32 to = 0, UINT32 isGM = 0) {
        ++calls; text = content.value; sender = name.value; source = from; target = to; gm = isGM;
    }
};
INT GetTypeFromID(UINT32 value) { return (INT)(value >> 24); }
enum { c2m_friend_chat = 1 };
struct FTCHARToUTF8 {
    const char* bytes;
    FTCHARToUTF8(const char* text) : bytes(text) {}
    operator const char*() const { return bytes; }
};
struct CMessageHandler {
    char ms_MsgBuffer[4096];
    std::vector<char> sent;
    BOOL _Send(const void* buffer, UINT32 size) {
        assert(size <= sizeof(ms_MsgBuffer));
        sent.assign((const char*)buffer, (const char*)buffer + size);
        return TRUE;
    }
    void SendFriendChat(INT, FString);
    void Parse_m2c_chat_message(const char*, UINT32);
    void Parse_g2c_chat_message(const char*, UINT32);
    void Parse_m2c_mail_list_recv(const char*, UINT32);
    void Parse_m2c_mail_read(const char*, UINT32);
};
''')
    parts.append(function(chat, "static BOOL ReadChatSender("))
    parts.extend(function(chat, "void CMessageHandler::Parse_" + name + "(")
                 for name in ["m2c_chat_message", "g2c_chat_message"])
    parts.extend(function(mail, "void CMessageHandler::Parse_" + name + "(")
                 for name in ["m2c_mail_list_recv", "m2c_mail_read"])
    parts.append(function(clean(game / "net/message_handler_Friend.cpp"), "void CMessageHandler::SendFriendChat("))
    # Compile each real entry guard, stopping before its engine/UI dependency.
    for file, parser, name, end in [
        ("message_handler_Login.cpp", "t2c_sync_role_list", "Roles", "AMMOPlayerController* ctrl"),
        ("message_handler_Friend.cpp", "g2c_friend_group_list", "Groups", "AMMOPlayer* player"),
        ("message_handler_Friend.cpp", "m2c_friend_list", "Friends", "AMMOPlayer* player"),
        ("message_handler.cpp", "g2c_sync_title", "Titles", "AMMOPlayer* player"),
        ("message_handler_Team.cpp", "m2c_team_join", "TeamJoin", "AMMOPlayer* pl"),
        ("message_handler_Team.cpp", "g2c_sync_adjacent_team", "NearbyTeams", "AMMOPlayer* pl"),
    ]:
        body = function(clean(game / "net" / file), "void CMessageHandler::Parse_" + parser + "(")
        prefix = body[body.index("{") + 1:body.index(end)].replace("return;", "return false;")
        parts.append("bool Validate" + name + "(const char* buf, UINT32 size) {" + prefix + "return true; }")
    login = clean(game / "net/message_handler_Login.cpp")
    parts.append(r'''
void appStrncpy(char* destination, const char* text, size_t size) { ::strxcpy(destination, text, size); }
struct LoginNames {
    char before = 'A';
    char _UserName[USER_NAME_LEN];
    char middle = 'B';
    char _RoleName[ROLE_NAME_LEN];
    char after = 'C';
    void User(T2C_LOGIN_ACK* msg) {
''')
    parts.append(re.search(r"appStrncpy\(_UserName[^;]+;", login).group())
    parts.append("}\nvoid Role(LOGIN_ROLE_DATA* pBaseData) {")
    parts.append(re.search(r"appStrncpy\(_RoleName[^;]+;", login).group())
    parts.append("}\n};")
    for parser, name, marker in [("g2c_sync_shield_list", "Shields", "for ( INT i")]:
        body = function(clean(game / "net/message_handler_Friend.cpp"), "void CMessageHandler::Parse_" + parser + "(")
        prefix = body[body.index("{") + 1:body.index(marker)].replace("return;", "return false;")
        parts.append("bool Validate" + name + "(const char* buf, UINT32 size) {" + prefix + "return true; }")
    parts.append(r'''
template<class Packet, class Parse>
void Chat(Parse parse) {
    CMessageHandler handler;
    auto& received = MMOChatManager::getSingleton();
    for (const std::string payload : {
             std::string(), std::string("abc"), std::string("abc\0", 4),
             std::string("abc\0sender", 10)}) {
        std::vector<char> packet(sizeof(Packet) + payload.size(), 0);
        memcpy(packet.data() + sizeof(Packet), payload.data(), payload.size());
        const INT calls = received.calls;
        (handler.*parse)(packet.data(), (UINT32)packet.size());
        assert(received.calls == calls);
    }
    for (const std::string payload : {std::string("hello\0sender\0", 13), std::string("\0\0", 2)}) {
        std::vector<char> packet(sizeof(Packet) + payload.size(), 0);
        memcpy(packet.data() + sizeof(Packet), payload.data(), payload.size());
        auto* message = reinterpret_cast<Packet*>(packet.data());
        message->dwSrcID = 123;
        message->dwTargetID = 456;
        const INT calls = received.calls;
        for (UINT32 length = 0; length < packet.size(); ++length) {
            std::vector<char> truncated(packet.begin(), packet.begin() + length);
            (handler.*parse)(truncated.data(), length);
            assert(received.calls == calls);
        }
        (handler.*parse)(packet.data(), (UINT32)packet.size());
        assert(received.calls == calls + 1 && received.source == 123);
        assert(received.text == payload.c_str());
        assert(received.sender == payload.c_str() + strlen(payload.c_str()) + 1);
    }
}
void Run() {
    char unterminated[4] = {'A', 'B', 'C', 'D'};
    assert(ReadFixedUTF8(unterminated).value == "ABCD");
    char terminated[4] = {'A', 0, 'C', 'D'};
    assert(ReadFixedUTF8(terminated).value == "A");
    LoginNames names;
    T2C_LOGIN_ACK login = {};
    LOGIN_ROLE_DATA role = {};
    memset(login.szUserName, 'U', USER_NAME_LEN);
    memset(role.szRoleName, 'R', ROLE_NAME_LEN);
    names.User(&login);
    names.Role(&role);
    assert(names.before == 'A' && names.middle == 'B' && names.after == 'C');
    assert(std::string(names._UserName) == std::string(USER_NAME_LEN - 1, 'U'));
    assert(std::string(names._RoleName) == std::string(ROLE_NAME_LEN - 1, 'R'));
    T2C_SYNC_ROLE_LIST roles = {};
    for (UINT32 length = 0; length < sizeof(roles); ++length) {
        const char* begin = reinterpret_cast<const char*>(&roles);
        std::vector<char> truncated(begin, begin + length);
        assert(!ValidateRoles(truncated.data(), length));
    }
    for (UINT32 count = 0; count <= 255; ++count) {
        roles.byCount = (UINT8)count;
        assert(ValidateRoles(reinterpret_cast<const char*>(&roles), sizeof(roles)) ==
               (count <= ARRAY_COUNT(roles.RoleData)));
    }
    for (UINT32 count = 0; count <= 3; ++count) {
        std::vector<char> groups(sizeof(G2C_FRIEND_GROUP_LIST) + count * sizeof(FRIEND_GROUP_INFO), 0);
        auto* packet = reinterpret_cast<G2C_FRIEND_GROUP_LIST*>(groups.data());
        packet->byGroupCount = (UINT8)count;
        assert(ValidateGroups(groups.data(), (UINT32)groups.size()));
        if (count) assert(!ValidateGroups(groups.data(), (UINT32)groups.size() - 1));
        packet->byGroupCount = 255;
        assert(!ValidateGroups(groups.data(), (UINT32)groups.size()));
        std::vector<char> friends(sizeof(M2C_FRIEND_LIST) + count * sizeof(FRIEND_INFO_WITH_GROUP), 0);
        auto* friendPacket = reinterpret_cast<M2C_FRIEND_LIST*>(friends.data());
        friendPacket->byFriendCount = (UINT8)count;
        assert(ValidateFriends(friends.data(), (UINT32)friends.size()));
        if (count) assert(!ValidateFriends(friends.data(), (UINT32)friends.size() - 1));
        friendPacket->byFriendCount = 255;
        assert(!ValidateFriends(friends.data(), (UINT32)friends.size()));
        std::vector<char> titles(sizeof(G2C_SYNC_TITLE) + count * sizeof(TITLE_DATA), 0);
        auto* titlePacket = reinterpret_cast<G2C_SYNC_TITLE*>(titles.data());
        titlePacket->nTitleCount = (UINT8)count;
        assert(ValidateTitles(titles.data(), (UINT32)titles.size()));
        if (count) assert(!ValidateTitles(titles.data(), (UINT32)titles.size() - 1));
        titlePacket->nTitleCount = 255;
        assert(!ValidateTitles(titles.data(), (UINT32)titles.size()));
        std::vector<char> members(sizeof(M2C_TEAM_JOIN) + count * sizeof(TEAM_MEMBER_BASEINFO), 0);
        auto* joined = reinterpret_cast<M2C_TEAM_JOIN*>(members.data());
        joined->byMemberCount = (UINT8)count;
        assert(ValidateTeamJoin(members.data(), (UINT32)members.size()));
        if (count) assert(!ValidateTeamJoin(members.data(), (UINT32)members.size() - 1));
        joined->byMemberCount = 255;
        assert(!ValidateTeamJoin(members.data(), (UINT32)members.size()));
        std::vector<char> nearby(sizeof(G2C_SYNC_ADJACENT_TEAM) + count * sizeof(ADJACENT_TEAM_ITEM), 0);
        auto* teams = reinterpret_cast<G2C_SYNC_ADJACENT_TEAM*>(nearby.data());
        teams->byCount = (UINT8)count;
        assert(ValidateNearbyTeams(nearby.data(), (UINT32)nearby.size()));
        if (count) assert(!ValidateNearbyTeams(nearby.data(), (UINT32)nearby.size() - 1));
        teams->byCount = 255;
        assert(!ValidateNearbyTeams(nearby.data(), (UINT32)nearby.size()));
        std::vector<char> shielded(sizeof(G2C_SYNC_SHIELD_LIST) + count * sizeof(UINT32), 0);
        auto* shield = reinterpret_cast<G2C_SYNC_SHIELD_LIST*>(shielded.data());
        shield->byRoleCount = (UINT8)count;
        assert(ValidateShields(shielded.data(), (UINT32)shielded.size()));
        if (count) assert(!ValidateShields(shielded.data(), (UINT32)shielded.size() - 1));
        shield->byRoleCount = 255;
        assert(!ValidateShields(shielded.data(), (UINT32)shielded.size()));
    }
    check_constructed<ClientBriefMailInfo>([](ClientBriefMailInfo& mail) {
        assert(!mail.IsSelected);
        const auto* bytes = reinterpret_cast<const unsigned char*>(static_cast<const BriefMailInfo*>(&mail));
        for (size_t i = 0; i < sizeof(BriefMailInfo); ++i) assert(bytes[i] == 0);
        assert(mail.mailSubject.value.empty());
    });
    check_constructed<ClientMailDetail>([](ClientMailDetail& mail) {
        const auto* bytes = reinterpret_cast<const unsigned char*>(static_cast<const M2C_MAIL_READ*>(&mail));
        for (size_t i = 0; i < sizeof(M2C_MAIL_READ); ++i) assert(bytes[i] == 0);
        assert(mail.mailSubject.value.empty());
    });
    Chat<M2C_CHAT_MESSAGE>(&CMessageHandler::Parse_m2c_chat_message);
    Chat<G2C_CHAT_MESSAGE>(&CMessageHandler::Parse_g2c_chat_message);
    CMessageHandler handler;
    for (size_t length : {1u, 255u, 256u, 1024u, 100000u}) {
        memset(handler.ms_MsgBuffer, 0xA5, sizeof(handler.ms_MsgBuffer));
        const std::string text(length, 'A');
        handler.SendFriendChat(123, FString(text.c_str()));
        const auto* sent = reinterpret_cast<const C2M_FRIEND_CHAT*>(handler.sent.data());
        const size_t copied = length < MAX_CHAT_MESSAGE_LEN ? length : MAX_CHAT_MESSAGE_LEN - 1;
        assert(handler.sent.size() == sizeof(C2M_FRIEND_CHAT) + copied + 1);
        assert(sent->wSize == handler.sent.size() && sent->dwTargetID == 123 && !sent->byColor);
        assert(std::string(sent->szMessage) == text.substr(0, copied));
    }
    auto& received = MailManager::getSingleton();
    std::vector<char> packet(sizeof(M2C_MAIL_LIST_RECV) + sizeof(BriefMailInfo), 0);
    auto* list = reinterpret_cast<M2C_MAIL_LIST_RECV*>(packet.data());
    list->wMailCount = 1;
    memset(list->stBriefMailInfo[0].szSenderRoleName, 'N', ROLE_NAME_LEN);
    memset(list->stBriefMailInfo[0].szMailSubject, 'S', MAIL_SUBJECT_LEN);
    const auto original = packet;
    for (UINT32 length = 0; length < packet.size(); ++length) {
        std::vector<char> truncated(packet.begin(), packet.begin() + length);
        handler.Parse_m2c_mail_list_recv(truncated.data(), length);
        assert(received.brief.empty() && !received.notifications);
    }
    handler.Parse_m2c_mail_list_recv(packet.data(), (UINT32)packet.size());
    assert(packet == original && received.brief.size() == 1 && received.notifications == 1);
    assert(received.brief[0].mailSubject.value == std::string(MAIL_SUBJECT_LEN, 'S'));
    assert(strlen(received.brief[0].szSenderRoleName) == ROLE_NAME_LEN - 1);
    list->wMailCount = 65535;
    handler.Parse_m2c_mail_list_recv(packet.data(), (UINT32)packet.size());
    assert(received.brief.size() == 1 && received.notifications == 1);
    M2C_MAIL_READ detail = {};
    memset(detail.szSenderRoleName, 'N', ROLE_NAME_LEN);
    memset(detail.szMailSubject, 'S', MAIL_SUBJECT_LEN);
    memset(detail.szMailContent, 'C', MAIL_CONTENT_LEN);
    for (UINT32 length = 0; length < sizeof(detail); ++length) {
        const char* begin = reinterpret_cast<const char*>(&detail);
        std::vector<char> truncated(begin, begin + length);
        handler.Parse_m2c_mail_read(truncated.data(), length);
        assert(received.detail.empty());
    }
    handler.Parse_m2c_mail_read(reinterpret_cast<const char*>(&detail), sizeof(detail));
    assert(received.detail.size() == 1);
    assert(received.detail[0].mailSubject.value == std::string(MAIL_SUBJECT_LEN, 'S'));
    assert(strlen(received.detail[0].szMailContent) == MAIL_CONTENT_LEN - 1);
    assert(detail.szMailContent[MAIL_CONTENT_LEN - 1] == 'C');
    puts("Chat and mail packet boundary regressions passed");
}
#undef UTF8_TO_TCHAR
#undef ROLE_NAME_LEN
#undef MAIL_SUBJECT_LEN
#undef MAIL_CONTENT_LEN
#undef USER_NAME_LEN
#undef USER_PASSWORD_LEN
#undef GUILD_NAME_LEN
#undef FRIEND_GROUP_NAME_LEN
#undef SIGNATURE_LEN
#undef MAX_USABLE_ROLE_PER_USER
#undef MAX_DELETE_ROLE_PER_USER
#undef MAX_ROLE_PER_USER
#undef MAX_CHAT_MESSAGE_LEN
#undef TEAM_NAME_LEN
}
''')
    return "\n".join(parts)
