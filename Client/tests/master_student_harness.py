"""Production mentor/student readers, list consumers and record defaults."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    network = clean(source(game / "net/message_handler_MasterStudent.cpp"))
    cpp = clean(source(game / "MasterStudentManager.cpp"))
    header = clean(source(game / "MasterStudentManager.h"))
    signatures = re.findall(r"void CMessageHandler::Parse_(\w+)\(const char\* buf, UINT32 size\)\s*\{", network)
    if len(signatures) != 33:
        raise ValueError("Mentor/student parser coverage changed")
    parsers = [function(network, "void CMessageHandler::Parse_" + name + "(") for name in signatures]
    types = [re.search(r"\b(\w+)\* msg\s*=", body).group(1) for body in parsers]
    parts = [r'''
namespace MasterStudentTests {
struct FString : std::string {
    using std::string::string;
    using std::string::operator=;
    const char* operator*() const { return c_str(); }
};
using INT16 = int16_t;
enum EMASTER_STATE { E_MASTER_NONE };
enum { EPCT_PlayerClassA = 0, GRADUATE_LEVEL = 50 };
struct MMOGameEvent {};
''']
    constants = clean(source(client / "Common/define/master_def.h"))
    for name in ["MAX_STUDENT_NUM", "MAX_CZ_TEXT_LENGTH"]:
        value = re.search(r"#define " + name + r"\s+([^\n]+)", constants).group(1)
        parts.append("enum { " + name + " = " + value + " };")
    parts.append(function(header, "enum EMasterStudentType\n") + ";")
    parts.append(r'''
struct MMOGameEvent_MasterStudent { explicit MMOGameEvent_MasterStudent(EMasterStudentType) {} };
namespace MMOUI {
int events = 0;
void PostGameEvent(MMOGameEvent_MasterStudent*) { ++events; }
FString ParseLocalizedPropertyPath(const char*) { return FString("Unknown"); }
}
struct PlayerBaseInfo {
    FString _RoleName = "Player";
    int _Level = 42, _RoleClass = 3;
};
struct MMOFacade {
    bool available = false;
    PlayerBaseInfo player;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    PlayerBaseInfo* RequestPlayerBaseInfo(UINT32, UBOOL) { return available ? &player : nullptr; }
};
namespace ExternalMessage {
#pragma pack(push, 1)
''')
    wires = clean(source(client / "Common/message/m2c_message.h")) + "\n" + clean(source(client / "Common/message/g2c_message.h"))
    parts.append(function(wires, "struct MASTER_STUDENT_INFO\n") + ";")
    for name in dict.fromkeys(types):
        parts.append(function(wires, "struct " + name + " :") + ";")
    parts.append(function(clean(source(client / "Common/message/c2m_message.h")), "struct C2M_MASTER_CZ :") + ";")
    parts.append("#pragma pack(pop)\n}\nusing namespace ExternalMessage;")
    records = ["Teacher_RewardExp", "MasterDetailInfo", "StudentListData", "ApplicantsListData",
               "SystemPosterListData", "CongratulationGiftData", "TwitterInfo", "GiftDetailData", "MailTemplateData"]
    declarations = {}
    for name in records:
        declaration = re.search(r"struct " + name + r"\s*\{", header).group()
        declarations[name] = function(header, declaration)
        parts.append(declarations[name] + ";")
    parts.append(r'''
using StudentList = std::list<StudentListData>;
using SystemPosterList = std::list<SystemPosterListData>;
struct MasterStudentManager {
    int calls = 0;
    StudentList listStudent;
    SystemPosterList listSystemPoster;
    MasterDetailInfo masterInfo;
    TwitterInfo studentTwitterInfo;
    static MasterStudentManager& getSingleton() { static MasterStudentManager value; return value; }
    SystemPosterListData* _GetSystemPosterDataByID(INT);
    StudentListData* _GetStudentDataByID(INT);
    void AddorUpdateSystemPoster(const M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO*);
    void AddorUpdateSystemPoster(const M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO*);
    StudentListData* AddorUpdateStudent(const MASTER_STUDENT_INFO*);
    void MarkSystemPosterList();
    void RemoveInvalidSystemPoster();
    void NotifyUpdateMasterList(const M2C_MASTER_RECRUIT_LIST*);
    void NotifyUpdateMasterSoloListAck(const M2C_MASTER_GET_SOLO_LIST_ACK*);
    void NotifyMasterInfo(const M2C_MASTER_INFO*);
''')
    real_methods = {"NotifyUpdateMasterList", "NotifyUpdateMasterSoloListAck", "NotifyMasterInfo"}
    methods = set(re.findall(r"MasterStudentManager::getSingleton\(\)\.(\w+)\(", "\n".join(parsers)))
    for name in sorted(methods - real_methods):
        parts.append("template<class... Args> void " + name + "(Args&&...) { ++calls; }")
    parts.append(r'''
};
int conversion_length = -2;
struct FTCHARToUTF8 {
    const char* text;
    FTCHARToUTF8(const char* value) : text(value) {}
    int Length() const { return conversion_length == -2 ? (int)strlen(text) : conversion_length; }
    operator const char*() const { return text; }
};
enum { c2m_master_cz = 40 };
struct CMessageHandler {
    int requests = 0;
    char ms_MsgBuffer[512] = {};
    std::vector<char> sent;
    static CMessageHandler& GetInstance() { static CMessageHandler value; return value; }
    void Send_QueryMasterExp() { ++requests; }
    void Send_Congratulation(INT, INT, INT, FString, INT);
    void _Send(const void* bytes, size_t length) {
        assert(length <= sizeof(ms_MsgBuffer));
        sent.assign((const char*)bytes, (const char*)bytes + length);
    }
''')
    parts.extend("void Parse_" + name + "(const char*, UINT32);" for name in signatures)
    parts.append("};")
    parts.extend(parsers)
    parts.append(function(network, "void CMessageHandler::Send_Congratulation("))
    for signature in [
        "SystemPosterListData* MasterStudentManager::_GetSystemPosterDataByID(",
        "StudentListData* MasterStudentManager::_GetStudentDataByID(",
        "void MasterStudentManager::AddorUpdateSystemPoster(const M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO*",
        "void MasterStudentManager::AddorUpdateSystemPoster(const M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO*",
        "StudentListData* MasterStudentManager::AddorUpdateStudent(const MASTER_STUDENT_INFO*",
        "void MasterStudentManager::MarkSystemPosterList()",
        "void MasterStudentManager::RemoveInvalidSystemPoster()",
        "void MasterStudentManager::NotifyUpdateMasterList(",
        "void MasterStudentManager::NotifyUpdateMasterSoloListAck(",
        "void  MasterStudentManager::NotifyMasterInfo(",
    ]:
        parts.append(function(cpp, signature))
    parts.append(r'''
using Parser = void (CMessageHandler::*)(const char*, UINT32);
int Effects() {
    return MasterStudentManager::getSingleton().calls + MMOUI::events + CMessageHandler::GetInstance().requests;
}
template<class T> void TestHeader(CMessageHandler& handler, Parser parser) {
    const int before = Effects();
    (handler.*parser)(nullptr, 0);
    (handler.*parser)(nullptr, 0xffffffffu);
    for (UINT32 size = 0; size < sizeof(T); ++size) {
        auto bytes = std::make_unique<char[]>(size ? size : 1);
        memset(bytes.get(), 0xff, size);
        (handler.*parser)(bytes.get(), size);
        assert(Effects() == before);
    }
    T message = {};
    (handler.*parser)((const char*)&message, sizeof(message));
    assert(Effects() > before);
}
template<class T, class Entry, class SetCount> void TestList(CMessageHandler& handler, Parser parser, SetCount set_count) {
    auto bytes = std::make_unique<char[]>(sizeof(T) + sizeof(Entry));
    memset(bytes.get(), 0, sizeof(T) + sizeof(Entry));
    T* message = (T*)bytes.get();
    const int before = Effects();
    set_count(message, 1);
    for (UINT32 size = sizeof(T); size < sizeof(T) + sizeof(Entry); ++size) {
        (handler.*parser)(bytes.get(), size); assert(Effects() == before);
    }
    for (int count : {2, 255}) {
        set_count(message, count);
        (handler.*parser)(bytes.get(), sizeof(T) + sizeof(Entry)); assert(Effects() == before);
    }
    set_count(message, 1);
    (handler.*parser)(bytes.get(), sizeof(T) + sizeof(Entry)); assert(Effects() > before);
}
void Run() {
''')
    for name in records:
        parts.append("TipsStallTests::Poisoned<" + name + ">([](auto& value) {")
        for kind, field, count in re.findall(r"(?m)^\s*(INT|INT32|UINT32|UINT8|BOOL|bool|FString)\s+(\w+)\s*(?:\[(\d+)\])?\s*;", declarations[name]):
            if count:
                parts.append("for (auto item : value." + field + ") assert(!item);")
            elif kind == "FString":
                parts.append("assert(value." + field + ".empty());")
            else:
                parts.append("assert(!value." + field + ");")
        parts.append("});")
    parts.append("CMessageHandler handler;")
    parts.extend("TestHeader<" + kind + ">(handler, &CMessageHandler::Parse_" + name + ");"
                 for name, kind in zip(signatures, types))
    for kind, entry, name, field in [
        ("M2C_MASTER_RECRUIT_LIST", "M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO", "m2c_master_recruit_list", "byCount"),
        ("M2C_MASTER_GET_SOLO_LIST_ACK", "M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO", "m2c_master_get_solo_list_ack", "byCount"),
        ("M2C_MASTER_INFO", "MASTER_STUDENT_INFO", "m2c_master_info", "byStudentNum"),
    ]:
        parts.append("TestList<" + kind + ", " + entry + ">(handler, &CMessageHandler::Parse_" + name + ",\n"
                     "[](auto* message, int count) { message->" + field + " = (UINT8)count; });")
    parts.append(r'''
    auto& manager = MasterStudentManager::getSingleton();
    manager.listSystemPoster.clear();
    M2C_MASTER_RECRUIT_LIST::RECRUIT_MASTER_INFO recruit = {};
    recruit.dwRoleID = 100; recruit.llMasterExp = 12345;
    manager.AddorUpdateSystemPoster(&recruit);
    assert(manager.listSystemPoster.size() == 1);
    assert(!manager.listSystemPoster.front().bSelect && manager.listSystemPoster.front().bValid);
    assert(manager.listSystemPoster.front().bCanApply && manager.listSystemPoster.front().nMasterExp == 12345);
    M2C_MASTER_GET_SOLO_LIST_ACK::MASTER_SOLO_INFO solo = {}; solo.dwRoleID = 101;
    manager.AddorUpdateSystemPoster(&solo);
    assert(manager.listSystemPoster.size() == 2);
    auto& result = manager.listSystemPoster.back();
    assert(!result.bSelect && result.bCanApply && result.bValid && !result.nMasterExp);
    assert(result.RoleName == "Unknown" && !result.nLevel && !result.nClass);
    manager.studentTwitterInfo.roleID = 102; manager.studentTwitterInfo.bApply = FALSE;
    solo.dwRoleID = 102; MMOFacade::getSingleton().available = true;
    manager.AddorUpdateSystemPoster(&solo);
    assert(!manager.listSystemPoster.back().bCanApply && !manager.listSystemPoster.back().bSelect);
    assert(!manager.listSystemPoster.back().nMasterExp && manager.listSystemPoster.back().RoleName == "Player");
    for (size_t length : {size_t(0), size_t(1), size_t(MAX_CZ_TEXT_LENGTH - 2), size_t(MAX_CZ_TEXT_LENGTH - 1)}) {
        FString text(length, 'x');
        handler.Send_Congratulation(1, 2, 3, text, 4);
        auto* packet = (C2M_MASTER_CZ*)handler.sent.data();
        assert(handler.sent.size() == sizeof(C2M_MASTER_CZ) + length + 1 && packet->wSize == handler.sent.size());
        assert(packet->dwRoleID == 1 && packet->dwTargetID == 2 && packet->byCZType == 3 && packet->byCZLevel == 4);
        assert(!memcmp(packet->szText, text.c_str(), length + 1));
    }
    const auto last = handler.sent;
    handler.Send_Congratulation(1, 2, 3, FString(MAX_CZ_TEXT_LENGTH, 'x'), 4);
    assert(handler.sent == last);
    for (int invalid : {-2147483647 - 1, -1, 2147483647}) {
        conversion_length = invalid;
        handler.Send_Congratulation(1, 2, 3, FString("short"), 4); assert(handler.sent == last);
    }
    conversion_length = -2;
    puts("Mentor/student defaults, 33 packet readers, three variable lists and outgoing text limits passed");
}
}
''')
    return "\n".join(parts)
