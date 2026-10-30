#include "message_handler.h"
#include "process_error.h"
#include "common_message.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "cmd_manager.h"
#include "gm_log.h"
#include "gm_log_data.h"
#include "gm_def.h"
#include "gm_version.h"
#include "error_def.h"
#include "gmtl_message.h"
#include "log.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <vector>

using namespace InternalMessage;
class CRoleInfo {};
CRoleInfo* g_pRoleInfo = nullptr;
namespace
{
std::size_t checks{};
void check(bool value, const char* message)
{
    ++checks;
    if (!value) throw std::runtime_error(message);
}
struct Request { UINT32 session; std::vector<char> bytes; };
std::vector<Request> requests;
std::vector<UINT32> selected{0, 1, 2};
std::function<BOOL(UINT32)> on_send;
std::function<void()> on_alloc;
std::vector<std::string> game_logs;
std::function<void()> on_game_log;
BOOL game_write=TRUE;
struct Allocator final : IPacketAllocator
{
    std::map<char*, std::unique_ptr<char[]>> live;
    std::vector<INT32> sizes;
    std::size_t fail_at{}, throw_at{}, allocations{}, freed{};
    PACKET_BUFFER* Alloc(INT32 size) override
    {
        sizes.push_back(size);
        if (sizes.size() == throw_at) throw std::bad_alloc();
        if (sizes.size() == fail_at) return nullptr;
        const auto callback = on_alloc;
        if (callback) callback();
        auto buffer = std::make_unique<char[]>(size);
        auto* pointer = buffer.get();
        live.emplace(pointer, std::move(buffer));
        ++allocations;
        return pointer;
    }
    void Free(PACKET_BUFFER* packet) override
    {
        if (live.erase(packet) != 1) std::abort();
        ++freed;
    }
    BOOL IsAllocated(PACKET_BUFFER* packet) override { return live.contains(packet); }
    UINT32 PrintMemoryStat() override { return static_cast<UINT32>(live.size()); }
    void CheckError() override {}
    void reset()
    {
        check(live.empty() && allocations == freed, "native packet ownership balanced");
        sizes.clear(); fail_at=0; throw_at=0;
    }
} allocator, replacement_allocator;
struct Socket final : ISocketSession
{
    UINT32 id{};
    BOOL address_ok = TRUE;
    INT32 Send(PACKET_BUFFER* packet, INT32 size, INT32 offset) override
    {
        check(offset == 0, "native sends use exact packet start");
        auto* owner = allocator.live.contains(packet) ? &allocator : &replacement_allocator;
        check(owner->live.contains(packet), "transport receives an owned native packet");
        requests.push_back({id, {packet, packet + size}});
        const auto callback = on_send;
        if (callback && !callback(id)) return 0;
        owner->Free(packet); // Accept consumes; rejection and throws do not.
        return size;
    }
    INT32 SendNoPackageHead(PACKET_BUFFER*, INT32) override { return 0; }
    void* GetUserData() override { return CSessionMgr::GetInstance().GetGMSession(id); }
    void SetUserData(void*) override {}
    BOOL GetAddress(UINT32& address, UINT16& port) override { address=0x0100007F; port=12345; return address_ok; }
    BOOL GetRemoteAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL Close() override { return TRUE; }
};
std::array<Socket, 3> sockets;
struct Net final : INet
{
    BOOL Init(IPacketAllocator*, INetEventHandler*, void*) override { return TRUE; }
    BOOL UnInit() override { return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32, UINT16) override { return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
} net;
void reset()
{
    on_game_log={};game_logs.clear();game_write=TRUE;
    on_send={}; on_alloc={}; requests.clear();
    allocator.reset(); replacement_allocator.reset();
    auto& handler=CMessageHandler::GetInstance(); handler.UnInit();
    check(handler.Init(&net, &allocator), "initialize actual handler");
    CCmdManager::GetInstance().Init();
    selected={0,1,2}; CServerMgr::GetInstance().Init(); CSessionMgr::GetInstance().Init(&net);
}
template<class T> T sent(std::size_t index = 0)
{
    check(index < requests.size() && requests[index].bytes.size() >= sizeof(T), "sent packet has expected fixed fields");
    T value;
    std::memcpy(&value, requests[index].bytes.data(), sizeof(value));
    return value;
}
void packet_check(UINT16 id, std::size_t size, std::size_t count = 3)
{
    check(requests.size() == count, "expected send count");
    for (const auto& request : requests)
    {
        UINT16 actual;
        std::memcpy(&actual, request.bytes.data(), sizeof(actual));
        check(actual == id && request.bytes.size() == size, "wire message ID and exact length");
    }
    check(allocator.live.empty() && replacement_allocator.live.empty(), "no native packet retained after send");
    for (INT32 allocated : allocator.sizes) check(allocated == size, "allocator receives exact published size");
}
void families()
{
    auto& h=CMessageHandler::GetInstance();
    reset(); check(h.DoAllPing(), "ping"); packet_check(a2a_ping, sizeof(MESSAGE_HEADER));
    reset(); check(h.DoRegisterToGM(0), "register"); packet_check(tl2gm_register, sizeof(TL2GM_REGISTER), 1);
    const auto registration=sent<TL2GM_REGISTER>();
    check(registration.dwIP==0x0100007F && registration.wPort==12345 && registration.szComputerName[0], "registration native address and host name");
    reset(); check(h.DoVerify(1,"user","password"), "verify"); packet_check(tl2gm_verify,sizeof(TL2GM_VERIFY),1);
    check(std::string(sent<TL2GM_VERIFY>().szName)=="user" && std::string(sent<TL2GM_VERIFY>().szPwd)=="password", "credentials preserved");
    reset(); check(h.DoLoginToGM(2,"user","password"), "login"); packet_check(tl2gm_login,sizeof(TL2GM_LOGIN),1);
    check(sent<TL2GM_LOGIN>().nVersion==GM_MAJOR_VERSION && sent<TL2GM_LOGIN>().nDependVersion==GM_DEPEND_VERSION, "protocol version fields");
    reset(); check(h.DoLogoutToGM(0), "logout"); packet_check(tl2gm_logout,sizeof(TL2GM_LOGOUT),1);
    reset(); check(h.DoPreQuery("role"), "pre-query"); packet_check(tl2gm_pre_query,sizeof(TL2GM_PRE_QUERY));
    check(std::string(sent<TL2GM_PRE_QUERY>().szRoleName)=="role", "role-name bytes");
    reset(); check(h.DoQueryRoleID(7,"account"), "role ID"); packet_check(tl2gm_query_role_id,sizeof(TL2GM_QUERY_ROLE_ID));
    check(sent<TL2GM_QUERY_ROLE_ID>().nQueryType==7 && std::string(sent<TL2GM_QUERY_ROLE_ID>().szName)=="account", "query fields");
    reset(); check(h.DoQueryRoleData(0xFEDCBA98), "role data"); packet_check(tl2gm_query_role_data,sizeof(TL2GM_QUERY_ROLE_DATA));
    const auto query=sent<TL2GM_QUERY_ROLE_DATA>();
    check(query.dwRoleID==0xFEDCBA98 && std::ranges::all_of(query.szRoleName,[](char c){return c==0;}), "unused role-name bytes initialized");
    reset(); check(h.DoForbidChat(17,42), "chat"); packet_check(tl2gm_forbid_chat,sizeof(TL2GM_FORBID_CHAT));
    check(sent<TL2GM_FORBID_CHAT>().dwTime==17 && sent<TL2GM_FORBID_CHAT>().dwRoleID==42, "chat payload");
    reset(); check(h.DoFreezeRole(19,43), "freeze"); packet_check(tl2gm_freeze_role,sizeof(TL2GM_FREEZE_ROLE));
    check(sent<TL2GM_FREEZE_ROLE>().dwTime==19 && sent<TL2GM_FREEZE_ROLE>().dwRoleID==43, "freeze payload");
    reset(); check(h.DoKickRole(44), "kick"); packet_check(tl2gm_kick_role,sizeof(TL2GM_KICK_ROLE));
    reset(); check(h.DoSendMail(45,"subject","content","reason",123,8), "mail"); packet_check(tl2gm_send_mail,sizeof(TL2GM_SEND_MAIL));
    const auto mail=sent<TL2GM_SEND_MAIL>();
    check(mail.dwRoleID==45 && mail.dwItemID==123 && mail.nItemNum==8 && std::string(mail.szSubject)=="subject" && std::string(mail.szContent)=="content" && std::string(mail.szReason)=="reason", "mail payload");
    reset(); check(h.DoAnnounce(3,100,4,5,"text"), "announcement"); packet_check(tl2gm_announce,sizeof(TL2GM_ANNOUNCE)+5);
    const auto announce=sent<TL2GM_ANNOUNCE>();
    check(announce.nGroupID==3 && announce.dwBeginTime==100 && announce.nRepeatNum==4 && announce.dwIntervalTime==5, "announcement fields");
    check(std::string(requests[0].bytes.data()+sizeof(announce))=="text", "terminated announcement tail");
    reset(); check(h.DoGetAnnounceList(4), "announcement list"); packet_check(tl2gm_get_announce_list,sizeof(TL2GM_GET_ANNOUNCE_LIST));
    reset(); check(h.DoDelAnnounce(5,1234), "announcement delete"); packet_check(tl2gm_del_announce,sizeof(TL2GM_DEL_ANNOUNCE));
    check(sent<TL2GM_DEL_ANNOUNCE>().dwAnnounceSerial==1234, "announcement serial");
    reset(); check(h.DoLog(2,3,4,"query"), "log"); packet_check(tl2gm_log,sizeof(TL2GM_LOG)+6);
    check(sent<TL2GM_LOG>().nLogType==2 && sent<TL2GM_LOG>().dwBeginTime==3 && sent<TL2GM_LOG>().dwEndTime==4, "log fields");
    reset(); check(h.DoExecuteGMStrCmd("gm string"), "GM string"); packet_check(tl2gm_exe_gm_str,sizeof(TL2GM_EXE_GM_STR)+10);
    reset(); check(h.DoQueryNameToID(321,"role"), "name to ID"); packet_check(tl2gm_query_name_to_id,sizeof(TL2GM_QUERY_NAME_TO_ID));
    check(sent<TL2GM_QUERY_NAME_TO_ID>().nIndex==321, "query index");
    reset(); check(h.DoLoadRoleData(46), "load role"); packet_check(tl2gm_load_role_data,sizeof(TL2GM_LOAD_ROLE_DATA));
    reset(); check(h.DoReleaseRole(1,47), "release role"); packet_check(tl2gm_release_role,sizeof(TL2GM_RELEASE_ROLE),1);
    reset(); check(h.DoExeRoleGMCmd(48,"role cmd","reason"), "role command"); packet_check(tl2gm_role_exe_cmd,sizeof(TL2GM_ROLE_EXE_CMD));
    check(std::string(sent<TL2GM_ROLE_EXE_CMD>().szGM)=="role cmd", "role command text");
    reset(); check(h.DoSelectGroup(6), "select group"); packet_check(tl2gm_select_group,sizeof(TL2GM_SELECT_GROUP));
    reset(); check(h.DoQueryExchangeLog(7), "exchange prices"); packet_check(tl2gm_exchange_price,sizeof(TL2GM_EXCHANGE_PRICE));
    reset(); check(h.DoQueryExchangeHistory(8), "exchange history"); packet_check(tl2gm_exchange_history,sizeof(TL2GM_EXCHANGE_HISTORY));
    reset(); check(h.DoQueryOrderDetail(9,1,123), "order detail"); packet_check(tl2gm_order_detail,sizeof(TL2GM_ORDER_DETAIL));
    check(sent<TL2GM_ORDER_DETAIL>().nPrice==123 && sent<TL2GM_ORDER_DETAIL>().byOrderType==1, "order detail fields");
    reset(); check(h.DoAddSystemOrder(10,234,345), "add order"); packet_check(tl2gm_add_system_order,sizeof(TL2GM_ADD_SYSTEM_ORDER));
    check(sent<TL2GM_ADD_SYSTEM_ORDER>().nPrice==234 && sent<TL2GM_ADD_SYSTEM_ORDER>().nCount==345, "order fields");
    reset(); check(h.DoDelSystemOrder(11,0xFEDCBA9876543210ULL), "delete order"); packet_check(tl2gm_del_system_order,sizeof(TL2GM_DEL_SYSTEM_ORDER));
    check(sent<TL2GM_DEL_SYSTEM_ORDER>().qwOrderID==0xFEDCBA9876543210ULL, "full-width order ID");
    reset(); check(h.DoQuerySysExchangeInfo(12,3,1,2,3,4), "exchange statistics"); packet_check(tl2gm_sys_exchange_info,sizeof(TL2GM_SYS_EXCHANGE_INFO));
    const auto statistics=sent<TL2GM_SYS_EXCHANGE_INFO>();
    check(statistics.byFlag==3 && statistics.dwBeginDayTime==1 && statistics.dwEndDayTime==2 && statistics.dwBeginHourTime==3 && statistics.dwEndHourTime==4, "statistics fields");
    reset(); check(h.DoQueryExchangeLimit(13), "exchange limits"); packet_check(tl2gm_get_exchange_limit,sizeof(TL2GM_GET_EXCHANGE_LIMIT));
    reset(); check(h.DoSetExchangeLimit(14,100,10,20,30), "set limits"); packet_check(tl2gm_set_exchange_limit,sizeof(TL2GM_SET_EXCHANGE_LIMIT));
    const auto limits=sent<TL2GM_SET_EXCHANGE_LIMIT>();
    check(limits.nPriceHigh==100 && limits.nPriceLow==10 && limits.nSingleOrderLimit==20 && limits.nDailyOrderLimit==30, "limit fields");
}
void role_and_text_bounds()
{
    auto& h=CMessageHandler::GetInstance();
    reset();
    std::vector<char> data(MAX_GM_ROLE_DATA_STR_LENGTH);
    for (std::size_t i=0;i<data.size();++i) data[i]=static_cast<char>(i);
    check(h.DoModifyRoleData(42,43,static_cast<INT32>(data.size()),data.data(),"reason"), "full 32-KiB role data");
    packet_check(tl2gm_modify_role_data,sizeof(TL2GM_MODIFY_ROLE_DATA)+data.size());
    const auto role=sent<TL2GM_MODIFY_ROLE_DATA>();
    check(role.dwRoleID==42 && role.dwTargetRoleID==43 && role.nLength==data.size(), "role header");
    check(std::ranges::equal(data,std::span(requests[0].bytes).subspan(sizeof(role))), "all binary role bytes preserved");
    reset();
    for (INT32 size : {std::numeric_limits<INT32>::min(),-1,0,MAX_GM_ROLE_DATA_STR_LENGTH+1,std::numeric_limits<INT32>::max()})
        check(!h.DoModifyRoleData(42,43,size,data.data(),"reason"), "invalid role lengths rejected before copying");
    check(!h.DoModifyRoleData(42,43,1,nullptr,"reason") && !h.DoModifyRoleData(0,43,1,data.data(),"reason"), "invalid role identity/input");
    const std::string long_reason(GM_REASON_LEN,'x');
    check(!h.DoModifyRoleData(42,43,1,data.data(),long_reason.c_str()), "oversized reason rejected");
    check(requests.empty() && allocator.sizes.empty(), "invalid role input never reaches allocator");
    check(!h.DoVerify(0,nullptr,"password") && !h.DoLoginToGM(0,"name",nullptr), "null credentials rejected");
    const std::string long_name(USER_NAME_LEN,'x');
    check(!h.DoVerify(0,long_name.c_str(),"password"), "fixed name cannot truncate");
    check(!h.DoPreQuery(nullptr) && !h.DoSendMail(1,"s",nullptr,"r",0,0), "null fixed texts rejected");
    check(!h.DoExecuteGMStrCmd(nullptr) && !h.DoLog(1,2,3,nullptr) && !h.DoAnnounce(0,1,2,3,nullptr), "null variable texts rejected");
    check(!h.DoSelectGroup(-1) && !h.DoSelectGroup(MAX_SERVER_GROUP_COUNT) && !h.DoQuerySysExchangeInfo(0,256,0,0,0,0), "narrow fields checked before conversion");
    const std::string too_long(MAX_MESSAGE_SIZE,'x');
    check(!h.DoExecuteGMStrCmd(too_long.c_str()) && !h.DoLog(1,2,3,too_long.c_str()) && !h.DoAnnounce(0,1,2,3,too_long.c_str()), "variable tails respect wire capacity");
    check(requests.empty() && allocator.sizes.empty(), "invalid text never reaches allocator");
    reset();
    const std::string maximum_name(USER_NAME_LEN-1,'n');
    const std::string maximum_password(GM_TOOL_PASSWORD_LEN-1,'p');
    check(h.DoVerify(0,maximum_name.c_str(),maximum_password.c_str()), "maximum fixed strings fit exactly");
    const auto credentials=sent<TL2GM_VERIFY>();
    check(std::string(credentials.szName)==maximum_name && std::string(credentials.szPwd)==maximum_password,
        "fixed strings retain their final in-bounds terminators");
    reset();
    const std::string maximum(MAX_MESSAGE_SIZE-sizeof(TL2GM_EXE_GM_STR)-1,'x');
    check(h.DoExecuteGMStrCmd(maximum.c_str()), "largest terminated variable packet");
    packet_check(tl2gm_exe_gm_str,MAX_MESSAGE_SIZE);
}
void failures_and_callbacks()
{
    auto& h=CMessageHandler::GetInstance(); auto& manager=CCmdManager::GetInstance();
    for (std::size_t fail=1;fail<=3;++fail)
    {
        reset(); allocator.fail_at=fail;
        check(!manager.SendGMStrCmd("command"), "preflight allocation failure reported");
        check(requests.empty() && allocator.live.empty() && !manager.GetCurrentCmd(), "allocation failure sends nothing and retires command");
    }
    reset(); allocator.throw_at=2;
    bool threw=false; try { manager.SendGMStrCmd("command"); } catch (const std::bad_alloc&) { threw=true; }
    check(threw && requests.empty() && allocator.live.empty() && !manager.GetCurrentCmd(), "throwing allocation rolls back preflight ownership");
    reset(); selected={0,0}; CServerMgr::GetInstance().Init();
    check(!manager.SendGMStrCmd("command") && requests.empty() && allocator.sizes.empty(), "duplicate selection rejected before allocation");
    reset(); selected={0,MAX_GM_SESSION_COUNT}; CServerMgr::GetInstance().Init();
    check(!manager.SendGMStrCmd("command") && requests.empty(), "invalid session rejected before sends");
    reset(); selected.clear(); CServerMgr::GetInstance().Init();
    check(!manager.SendGMStrCmd("command") && requests.empty(), "empty selection fails");
    reset(); CSessionMgr::GetInstance().GetGMSession(1)->nConnectState=SESSION_DISCONNECTED;
    check(!manager.SendGMStrCmd("command") && requests.empty(), "disconnected selected peer fails preflight");
    reset(); on_send=[](UINT32 id){return id!=1;};
    check(manager.SendGMStrCmd("command"), "partial broadcast retains accepted sends");
    check(manager.GetCurrentCmd()->nSvrNum==2 && requests.size()==3 && allocator.live.empty(), "rejection excluded from pending reply count");
    reset(); on_send=[](UINT32){return FALSE;};
    check(!manager.SendGMStrCmd("command") && !manager.GetCurrentCmd() && allocator.live.empty(), "all rejected sends retire command and packets");
    reset(); CommandPtr held;
    on_send=[&](UINT32)
    {
        held=manager.GetCurrentCmd();
        check(held && held->in_flight>=2 && held->nSvrNum>0, "reply count and scoped ownership precede native send");
        --held->nSvrNum; manager.MainLoop();
        check(manager.GetCurrentCmd()==held, "inline ACK polling cannot retire issuing frame");
        return TRUE;
    };
    check(manager.SendGMStrCmd("command") && held->nSvrNum==0 && held->in_flight==0, "inline replies counted once");
    manager.MainLoop(); check(!manager.GetCurrentCmd(), "inline completion retires after sends return");
    reset(); on_send=[&](UINT32)
    {
        CServerMgr::GetInstance().ResetSelectedServer(); return TRUE;
    };
    check(manager.SendGMStrCmd("command") && requests.size()==3 && manager.GetCurrentCmd()->nSvrNum==3, "selection changes cannot alter owned broadcast snapshot");
    reset(); on_send=[&](UINT32 id)
    {
        if (id==0) CSessionMgr::GetInstance().GetGMSession(1)->pSocketSession=nullptr;
        return TRUE;
    };
    check(manager.SendGMStrCmd("command") && requests.size()==2 && manager.GetCurrentCmd()->nSvrNum==2, "peer removed during callback skipped and uncounted");
    reset(); CommandPtr replacement;
    on_send=[&](UINT32)
    {
        on_send={}; manager.CmdEnd();
        check(manager.SendGMStrCmd("replacement"), "callback can start a replacement command");
        replacement=manager.GetCurrentCmd(); return TRUE;
    };
    check(manager.SendGMStrCmd("original") && manager.GetCurrentCmd()==replacement && replacement->nSvrNum==3, "old broadcast cannot overwrite replacement count");
    check(requests.size()==4 && allocator.live.empty(), "replacement stops unsent original packets");
    reset(); on_send=[&](UINT32)
    {
        on_send={}; h.UnInit(); check(h.Init(&net,&allocator), "same-address runtime replacement"); return TRUE;
    };
    check(manager.SendGMStrCmd("command") && requests.size()==1 && allocator.live.empty(), "generation gate stops stale broadcast after restart");
    reset(); on_send=[&](UINT32)
    {
        h.UnInit(); manager.UnInit(); return TRUE;
    };
    check(!manager.SendGMStrCmd("command") && requests.size()==1 && allocator.live.empty(), "shutdown callback cancels unsent packets");
    reset(); on_send=[](UINT32)->BOOL { throw std::runtime_error("native send exception"); };
    threw=false; try { manager.SendGMStrCmd("command"); } catch (const std::runtime_error&) { threw=true; }
    check(threw && allocator.live.empty() && !manager.GetCurrentCmd(), "throwing send releases current and unsent copies");
    reset(); on_alloc=[&]
    {
        on_alloc={}; h.UnInit(); check(h.Init(&net,&replacement_allocator), "allocator replacement during allocation");
    };
    check(!manager.SendGMStrCmd("command") && requests.empty() && allocator.live.empty() && replacement_allocator.freed==replacement_allocator.allocations, "preflight callback uses captured allocator and cannot send across runtime replacement");
    reset(); on_send=[](UINT32){return FALSE;};
    check(!h.DoVerify(0,"name","password") && allocator.live.empty(), "single-send failure releases buffer");
    reset(); on_send=[](UINT32 id){return id!=1;};
    check(!h.DoAllPing() && requests.size()==3 && allocator.live.empty(), "failed ping does not skip healthy peers");
    reset(); on_send=[&](UINT32)
    {
        h.UnInit(); return TRUE;
    };
    check(!h.DoAllPing() && requests.size()==1 && allocator.live.empty(), "ping loop stops across runtime shutdown");
    reset(); on_send=[](UINT32)->BOOL {throw std::runtime_error("single send");};
    threw=false; try {h.DoVerify(0,"name","password");} catch(const std::runtime_error&) {threw=true;}
    check(threw && allocator.live.empty(), "single-send exception releases buffer");
    reset(); h.UnInit();
    check(!h.DoVerify(0,"name","password") && !h.DoExecuteGMStrCmd("command") && allocator.sizes.empty(), "closed handler rejects sends before native allocation");
    reset(); sockets[0].address_ok=FALSE;
    check(!h.DoRegisterToGM(0) && allocator.sizes.empty(), "failed registration address lookup does not allocate");
    sockets[0].address_ok=TRUE;
    reset();
}
void name_query_workflow(const std::filesystem::path& root)
{
    std::filesystem::create_directories(root);
    const auto report=root/"nameTOid.txt";
    const auto read=[&]
    {
        std::ifstream input(report,std::ios::binary);
        return std::string(std::istreambuf_iterator<char>(input),{});
    };
    const auto prepare=[&]
    {
        reset(); selected={0}; CServerMgr::GetInstance().Init();
        std::ofstream(report,std::ios::binary)<<"previous report";
    };
    std::map<INT32,ROLE_NAME_DATA> names;
    for (const auto [index,name] : {std::pair{1,"first"},{7,"second"},{2147483647,"last"}})
    {
        auto& value=names[index];std::memcpy(value.szRoleName,name,std::strlen(name)+1);
    }
    auto& manager=CCmdManager::GetInstance();
    prepare(); check(manager.QueryNameToID(names),"start sparse name batch");
    check(sent<TL2GM_QUERY_NAME_TO_ID>().nIndex==1,"first name index issued");
    check(!manager.ReceiveNameToID(1,1,42,report) && !manager.ReceiveNameToID(0,7,42,report),"wrong session and future index ignored");
    check(manager.ReceiveNameToID(0,1,42,report).has_value(),"accept first row");
    check(requests.size()==1 && read()=="previous report","next row deferred and old report preserved");
    check(!manager.ReceiveNameToID(0,1,99,report),"duplicate reply cannot advance pending batch");
    selected={1}; CServerMgr::GetInstance().Init();
    check(manager.MainLoop() && sent<TL2GM_QUERY_NAME_TO_ID>(1).nIndex==7,"map iteration handles sparse next index");
    check(requests.back().session==0,"selection changes cannot redirect remaining name queries");
    check(manager.ReceiveNameToID(0,7,4294967295,report).has_value(),"accept full-width role ID");
    check(manager.MainLoop() && sent<TL2GM_QUERY_NAME_TO_ID>(2).nIndex==2147483647,"last INT32 index does not overflow");
    check(manager.ReceiveNameToID(0,2147483647,0,report).has_value(),"complete final row");
    check(!manager.GetCurrentCmd() && read()=="first  42\r\nsecond  4294967295\r\nlast  0\r\n","final reply retires command and publishes all distinct rows");
    check(!manager.ReceiveNameToID(0,2147483647,0,report),"late reply cannot publish again");
    prepare(); check(manager.QueryNameToID(names),"start interrupted batch");
    check(manager.ReceiveNameToID(0,1,42,report).has_value(),"first interrupted row");
    on_send=[](UINT32){return FALSE;};
    check(!manager.MainLoop() && !manager.GetCurrentCmd() && read()=="previous report","failed continuation retires command without partial report");
    prepare(); check(manager.QueryNameToID(names),"start timeout batch");
    check(manager.ReceiveNameToID(0,1,42,report).has_value(),"first timed row");
    manager.GetCurrentCmd()->deadline=CCmdManager::Clock::time_point::min();
    check(manager.MainLoop() && !manager.GetCurrentCmd() && requests.size()==1 && read()=="previous report","expired batch cannot issue deferred row");
    prepare(); std::map<INT32,ROLE_NAME_DATA> one{{1,names.at(1)}};
    check(manager.QueryNameToID(one),"start failing report batch");
    check(!manager.ReceiveNameToID(0,1,42,root/"missing"/"report.txt") && !manager.GetCurrentCmd(),"report failure retires completed network command");
    prepare(); INT32 depth=0;
    on_send=[&](UINT32 id)
    {
        ++depth; check(depth==1,"inline replies never recursively issue next name query");
        const auto index=sent<TL2GM_QUERY_NAME_TO_ID>(requests.size()-1).nIndex;
        check(manager.ReceiveNameToID(id,index,static_cast<UINT32>(index),report).has_value(),"inline name reply accepted");
        manager.MainLoop(); --depth; return TRUE;
    };
    check(manager.QueryNameToID(names),"start inline reply batch");
    for (INT32 poll=0;poll<3 && manager.GetCurrentCmd();++poll) check(manager.MainLoop(),"drain deferred inline row");
    check(!manager.GetCurrentCmd() && requests.size()==3 && read()=="first  1\r\nsecond  7\r\nlast  2147483647\r\n","inline batch completes exactly once");
    reset(); check(!manager.QueryNameToID(names) && requests.empty(),"name query requires one selected peer");
    reset();
}
template<class Message, class Fill> void deliver(UINT16 id, UINT32 peer, Fill fill)
{
    Message message;
    std::memset(&message,0,sizeof(message)); message.wMsg=id; fill(message);
    auto* packet=allocator.Alloc(sizeof(message)); std::memcpy(packet,&message,sizeof(message));
    CMessageHandler::GetInstance().OnRecv(&sockets.at(peer),packet,sizeof(message));
}
void log_ack(UINT32 peer,INT32 type=GM_LOG_QUERY_ITEM,std::string_view text="row",BOOL truncated=FALSE,bool omit_nul=false)
{
    GM2TL_LOG_ACK reply;
    std::memset(&reply,0,sizeof(reply));reply.wMsg=gm2tl_log_ack;reply.nLogType=type;reply.bTruncated=truncated;
    reply.nLength=static_cast<INT32>(text.size()+1);
    const auto size=sizeof(reply)+text.size()+(omit_nul ? 0 : 1);
    auto* packet=allocator.Alloc(static_cast<INT32>(size));std::memcpy(packet,&reply,sizeof(reply));
    std::memcpy(packet+sizeof(reply),text.data(),text.size());if(!omit_nul)packet[size-1]=0;
    CMessageHandler::GetInstance().OnRecv(&sockets.at(peer),packet,static_cast<UINT32>(size));
}
void log_workflow()
{
    auto& manager=CCmdManager::GetInstance();
    const auto start=[&]
    {
        reset();selected={1};CServerMgr::GetInstance().Init();
        check(manager.QueryLog(GM_LOG_QUERY_ITEM,2200000000U,2200003600U,"1 2 3"),"managed log request starts on one peer");
        const auto command=std::static_pointer_cast<LOG_DATA>(manager.GetCurrentCmd());
        check(command && command->reply_peer==1 && command->reply_socket==&sockets[1] && command->nSvrNum==1,"log peer/socket/count installed before send callback");
        return command;
    };
    auto command=start();log_ack(0);log_ack(1,GM_LOG_QUERY_MONEY);
    check(game_logs.empty() && command->nSvrNum==1,"wrong peer or log type cannot consume request");
    selected={2};CServerMgr::GetInstance().Init();log_ack(1,GM_LOG_QUERY_ITEM,"legacy row",TRUE,true);
    check(game_logs==std::vector<std::string>{"legacy row"} && command->reply_done && !command->reply_busy && command->nSvrNum==0,"bound peer reply accepted after UI selection changes and missing legacy NUL handled");
    log_ack(1);check(game_logs.size()==1 && command->nSvrNum==0,"duplicate log reply cannot append or decrement twice");manager.MainLoop();
    check(!manager.GetCurrentCmd(),"completed log retires outside receive/issuing frames");
    command=start();game_write=FALSE;log_ack(1);
    check(command->nSvrNum==1 && !command->reply_done && !command->reply_busy,"failed output leaves log command retryable");
    game_write=TRUE;log_ack(1);check(command->nSvrNum==0 && command->reply_done,"successful output retry completes log command");
    command=start();on_game_log=[&]{log_ack(1);};log_ack(1);on_game_log={};
    check(game_logs.size()==1 && command->nSvrNum==0,"reentrant duplicate reply blocked while output callback runs");
    command=start();on_game_log=[&]{manager.DelCmd(command);};log_ack(1);on_game_log={};
    check(!manager.GetCurrentCmd() && command->nSvrNum==0,"cancellation inside output retains old owner safely");
    command=start();CommandPtr replacement;
    on_game_log=[&]
    {
        manager.DelCmd(command);
        check(manager.SendAnnounce(1,0,1,1,"new"),"replace current command in log output callback");
        replacement=manager.GetCurrentCmd();replacement->nSvrNum=7;
    };log_ack(1);on_game_log={};
    check(manager.GetCurrentCmd()==replacement && replacement->nSvrNum==7 && command->nSvrNum==0,"reply cannot decrement replacement command");
    command=start();auto* session=CSessionMgr::GetInstance().GetGMSession(1);auto* original=session->pSocketSession;session->pSocketSession=&sockets[2];
    log_ack(1);check(game_logs.empty() && command->nSvrNum==1,"replaced socket cannot deliver log reply");session->pSocketSession=original;
    on_game_log=[]{throw std::runtime_error("fixture output failure");};log_ack(1);on_game_log={};
    check(command->nSvrNum==1 && !command->reply_busy && !command->reply_done,"throwing output releases busy guard without completion");
    command=start();on_game_log=[&]{CMessageHandler::GetInstance().UnInit();manager.UnInit();};log_ack(1);on_game_log={};
    check(!manager.GetCurrentCmd() && command->nSvrNum==0,"shutdown during output releases old command safely");
    reset();selected={1};CServerMgr::GetInstance().Init();
    on_send=[&](UINT32 peer)
    {
        log_ack(peer);manager.MainLoop();
        check(manager.GetCurrentCmd() && manager.GetCurrentCmd()->in_flight>0,"inline reply remains owned until native send returns");return TRUE;
    };
    check(manager.QueryLog(GM_LOG_QUERY_ITEM,1,2,"1 2 3") && manager.GetCurrentCmd()->nSvrNum==0 && game_logs.size()==1,"inline native log reply completes once");
    on_send={};manager.MainLoop();check(!manager.GetCurrentCmd(),"inline completed log retires after issuing frame");
    reset();check(!manager.QueryLog(GM_LOG_QUERY_ITEM,1,2,"1 2 3") && requests.empty(),"managed log query rejects multiple selected peers");
    selected={1};CServerMgr::GetInstance().Init();
    check(!manager.QueryLog(-1,1,2,"query") && !manager.QueryLog(0,2,1,"query") && requests.empty(),"invalid log type/date rejected before native send");reset();
}
void resolve(UINT32 peer, const char* name, UINT32 role)
{
    deliver<GM2TL_PRE_QUERY_ACK>(gm2tl_pre_query_ack,peer,[&](auto& reply)
    { reply.dwRoleID=role;std::memcpy(reply.szRoleName,name,std::strlen(name)+1); });
}
void bulk_ack(UINT16 id,UINT32 peer,UINT32 role,INT32 result)
{
    // These four packed acknowledgements have the same fields and layout.
    static_assert(sizeof(GM2TL_FORBID_CHAT_ACK)==sizeof(GM2TL_FREEZE_ROLE_ACK));
    static_assert(sizeof(GM2TL_KICK_ROLE_ACK)==sizeof(GM2TL_SEND_MAIL_ACK));
    deliver<GM2TL_FORBID_CHAT_ACK>(id,peer,[&](auto& reply){reply.dwRoleID=role;reply.nResult=result;});
}
template<class T> std::shared_ptr<T> batch(INT32 kind,const std::filesystem::path& input)
{
    auto command=std::static_pointer_cast<T>(CCmdManager::GetInstance().NewCmd(kind));
    command->report_path=input;
    for (const auto name : {"first","second"})
    {
        ROLE_RESULT row{};std::memcpy(row.szRoleName,name,std::strlen(name)+1);command->roleList.push_back(row);
    }
    return command;
}
std::string read_file(const std::filesystem::path& path)
{
    std::ifstream input(path,std::ios::binary);
    return {std::istreambuf_iterator<char>(input),{}};
}
template<class T> void bulk_family(const std::filesystem::path& root,INT32 kind,UINT16 request,UINT16 ack,const char* suffix)
{
    auto& manager=CCmdManager::GetInstance();reset();selected={0};CServerMgr::GetInstance().Init();
    const auto input=root/("roles-"+std::to_string(kind));auto report=input;report+=suffix;
    std::ofstream(report,std::ios::binary)<<"previous report";
    auto command=batch<T>(kind,input);
    check(manager.PreQuery(kind,"first",command),"start bulk resolver");
    resolve(1,"first",101); resolve(0,"other",101);
    check(manager.GetCurrentCmd()->Kind()==CMD_PRE_QUERY && manager.GetCurrentCmd()->nSvrNum==1,"wrong peer/name cannot consume role resolution");
    resolve(0,"first",101);resolve(0,"first",101);
    check(requests.size()==1 && manager.GetCurrentCmd()->nSvrNum==0,"duplicate resolution cannot issue or cancel continuation");
    selected={1};CServerMgr::GetInstance().Init();
    check(manager.MainLoop() && manager.GetCurrentCmd()==command && command->reply_peer==0,"deferred typed handoff retains original peer");
    check(requests.size()==2 && sent<MESSAGE_HEADER>(1).wMsg==request && requests.back().session==0,"actual bulk request issued to captured peer");
    bulk_ack(ack,1,101,errSuccess);bulk_ack(ack,0,999,errSuccess);
    check(command->roleList.size()==2 && command->roleResList.empty() && command->nSvrNum==1,"wrong peer/role cannot advance batch");
    bulk_ack(ack,0,101,errSuccess);bulk_ack(ack,0,101,errSuccess);
    check(command->roleList.size()==1 && command->roleResList.size()==1 && requests.size()==2,"first ACK committed exactly once before deferred next query");
    check(read_file(report)=="previous report","partial batch does not publish report");
    check(manager.MainLoop() && manager.GetCurrentCmd()->Kind()==CMD_PRE_QUERY,"next role resolver deferred to MainLoop");
    bulk_ack(ack,0,101,errSuccess);resolve(0,"first",101);
    check(manager.GetCurrentCmd()->nSvrNum==1,"old phase replies cannot consume next role query");
    resolve(0,"second",202);
    check(manager.MainLoop() && manager.GetCurrentCmd()==command && command->roleList.front().dwRoleID==202,"second role handoff");
    bulk_ack(ack,0,202,errUnknownError);
    const std::string expected="\xEF\xBB\xBFRoleName\tRoleID\tResult \r\nfirst\t101\tsuccess \r\nsecond\t202\tfailed \r\n";
    check(!manager.GetCurrentCmd() && read_file(report)==expected,"final ACK retires command and publishes distinct complete result rows");
    bulk_ack(ack,0,202,errUnknownError);
    check(read_file(report)==expected && allocator.live.empty(),"late ACK cannot republish and packets release exactly once");
}
void bulk_workflows(const std::filesystem::path& root)
{
    std::filesystem::create_directories(root);
    bulk_family<FORBIDDEN_CHAT>(root,CMD_FORBIDDEN_CHAT,tl2gm_forbid_chat,gm2tl_forbid_chat_ack,"_chat_result");
    bulk_family<FREEZE_ROLE>(root,CMD_FREEZE_ROLE,tl2gm_freeze_role,gm2tl_freeze_role_ack,"_freeze_result");
    bulk_family<KICK_ROLE>(root,CMD_KICK_ROLE,tl2gm_kick_role,gm2tl_kick_role_ack,"_kickrole_result");
    bulk_family<SEND_MAIL>(root,CMD_SEND_MAIL,tl2gm_send_mail,gm2tl_send_mail_ack,"_sendmail_result");
    auto& manager=CCmdManager::GetInstance();
    const auto prepare=[&]{reset();selected={0};CServerMgr::GetInstance().Init();};
    for (const auto [kind,id] : {std::pair{CMD_QUERY_ROLE_DATA,tl2gm_query_role_data},
        {CMD_LOAD_ROLE_DATA,tl2gm_load_role_data},{CMD_EXE_ROLE_GM_CMD,tl2gm_role_exe_cmd}})
    {
        prepare();auto target=manager.NewCmd(kind);
        if (kind==CMD_QUERY_ROLE_DATA) std::memcpy(std::static_pointer_cast<QUERY_ROLE_DATA>(target)->szRoleName,"first",6);
        if (kind==CMD_LOAD_ROLE_DATA) std::memcpy(std::static_pointer_cast<LOAD_ROLE_DATA>(target)->szRoleName,"first",6);
        if (kind==CMD_EXE_ROLE_GM_CMD) std::memcpy(std::static_pointer_cast<EXE_ROLE_CMD>(target)->szRoleName,"first",6);
        check(manager.PreQuery(kind,"first",target),"stage non-bulk typed role handoff");resolve(0,"first",101);
        check(manager.MainLoop() && manager.GetCurrentCmd()==target && requests.size()==2 && sent<MESSAGE_HEADER>(1).wMsg==id,
            "each supported role continuation issues its matching wire operation");
        manager.CmdEnd();
    }
    prepare();auto empty=std::static_pointer_cast<KICK_ROLE>(manager.NewCmd(CMD_KICK_ROLE));
    check(manager.PreQuery(CMD_KICK_ROLE,"first",empty),"stage empty continuation");resolve(0,"first",101);
    check(manager.GetCurrentCmd()->nSvrNum==1 && empty->roleList.empty(),"empty role list rejected without dereferencing front");manager.CmdEnd();
    prepare();auto command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"not-found");std::weak_ptr<CMD_BASE> life=command;
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage missing role");command.reset();resolve(0,"first",0);
    check(!manager.GetCurrentCmd() && life.expired(),"not-found retires wrapper and child ownership");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"cancel");life=command;
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage cancellation");command.reset();resolve(0,"first",101);manager.CmdEnd();
    check(!manager.GetCurrentCmd() && life.expired(),"cancel releases queued continuation without cycles");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"timeout");life=command;
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage timeout");command.reset();resolve(0,"first",101);
    manager.GetCurrentCmd()->deadline=CCmdManager::Clock::time_point::min();manager.MainLoop();
    check(!manager.GetCurrentCmd() && life.expired() && requests.size()==1,"timeout cannot issue deferred role operation");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"shutdown");life=command;
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage shutdown");command.reset();resolve(0,"first",101);manager.UnInit();
    check(!manager.GetCurrentCmd() && life.expired(),"shutdown clears pending wrapper/child continuation");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"send-failure");
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage failed handoff");resolve(0,"first",101);on_send=[](UINT32){return FALSE;};
    check(!manager.MainLoop() && !manager.GetCurrentCmd() && allocator.live.empty(),"failed handoff retires exact owners and packet");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"throwing-send");
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage throwing handoff");resolve(0,"first",101);
    on_send=[](UINT32)->BOOL{throw std::runtime_error("bulk send exception");};
    bool threw=false;try{manager.MainLoop();}catch(const std::runtime_error&){threw=true;}
    check(threw && !manager.GetCurrentCmd() && allocator.live.empty(),"throwing continuation retires command and packets");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"missing"/"result");command->roleList.pop_back();
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"stage failing bulk report");resolve(0,"first",101);manager.MainLoop();
    bulk_ack(gm2tl_kick_role_ack,0,101,errSuccess);
    check(!manager.GetCurrentCmd() && allocator.live.empty(),"bulk report failure does not leave completed command waiting");
    prepare();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"inline");INT32 depth=0;
    on_send=[&](UINT32 peer)
    {
        ++depth;check(depth==1,"inline bulk replies cannot recursively issue native operations");
        const auto id=sent<MESSAGE_HEADER>(requests.size()-1).wMsg;
        if (id==tl2gm_pre_query)
        {
            const auto query=sent<TL2GM_PRE_QUERY>(requests.size()-1);
            resolve(peer,query.szRoleName,std::string(query.szRoleName)=="first" ? 101 : 202);
        }
        else
        {
            const auto request=sent<TL2GM_KICK_ROLE>(requests.size()-1);bulk_ack(gm2tl_kick_role_ack,peer,request.dwRoleID,errSuccess);
        }
        manager.MainLoop();--depth;return TRUE;
    };
    check(manager.PreQuery(CMD_KICK_ROLE,"first",command),"start inline bulk workflow");
    for (INT32 poll=0;poll<8 && manager.GetCurrentCmd();++poll) check(manager.MainLoop(),"drain inline bulk continuations");
    check(!manager.GetCurrentCmd() && requests.size()==4 && allocator.live.empty(),"inline two-role batch completes without recursion or retained packets");
    reset();command=batch<KICK_ROLE>(CMD_KICK_ROLE,root/"multi");
    check(!manager.PreQuery(CMD_KICK_ROLE,"first",command) && requests.empty(),"role workflow requires a single initial peer");reset();
}
void reused_connections()
{
    auto& handler=CMessageHandler::GetInstance();auto& manager=CCmdManager::GetInstance();
    const auto replace=[](UINT32 id){CSessionMgr::GetInstance().GetGMSession(id)->connection_lifetime=std::make_shared<const char>(0);};
    reset();on_alloc=[&]{replace(0);};
    check(!handler.DoVerify(0,"user","password") && requests.empty(),"allocator reconnect cannot redirect credentials to reused socket");
    reset();selected={0};CServerMgr::GetInstance().Init();on_alloc=[&]{replace(0);};
    check(!manager.SendGMStrCmd("fixture") && requests.empty(),"broadcast allocator reconnect cannot adopt new binding");
    reset();on_send=[&](UINT32 id){if(id==0)replace(1);return TRUE;};
    check(manager.SendGMStrCmd("fixture") && requests.size()==2 && requests[0].session==0 && requests[1].session==2,
        "partial broadcast skips later peer whose socket address was reused");
    check(manager.GetCurrentCmd()->nSvrNum==2,"only unchanged accepted peers remain in reply count");
    reset();selected={0};CServerMgr::GetInstance().Init();
    check(manager.QueryLog(GM_LOG_QUERY_ITEM,1,2,"query"),"log request on original binding");
    const auto command=manager.GetCurrentCmd();replace(0);log_ack(0);
    check(game_logs.empty() && command->nSvrNum==1,"old log request rejects ACK from reused socket binding");
    reset();selected={0};CServerMgr::GetInstance().Init();
    const auto target=manager.NewCmd(CMD_QUERY_ROLE_DATA);
    std::memcpy(std::static_pointer_cast<QUERY_ROLE_DATA>(target)->szRoleName,"first",6);
    check(manager.PreQuery(CMD_QUERY_ROLE_DATA,"first",target),"pre-query starts on original binding");resolve(0,"first",42);
    replace(0);check(!manager.MainLoop() && requests.size()==1,"deferred role continuation cannot adopt replacement binding");
    reset();
}

}

CSessionMgr CSessionMgr::m_Instance;
CSessionMgr::CSessionMgr() = default;
CSessionMgr::~CSessionMgr() = default;
CSessionMgr& CSessionMgr::GetInstance() { return m_Instance; }
BOOL CSessionMgr::Init(INet* value)
{
    m_pNet=value;
    for (auto& session : m_GMSession) session={};
    for (UINT32 id=0;id<sockets.size();++id)
    {
        sockets[id].id=id; sockets[id].address_ok=TRUE;
        auto& session=m_GMSession[id];session.dwSessionID=id;session.connection_lifetime=std::make_shared<const char>(0);session.pSocketSession=&sockets[id];session.nConnectState=SESSION_CONNECTED;
    }
    return TRUE;
}
GM_SESSION* CSessionMgr::Find(ISocketSession* socket)
{
    if (!socket) return nullptr;
    const auto found=std::ranges::find_if(m_GMSession,[&](const auto& session){return session.pSocketSession==socket;});
    return found==m_GMSession.end() ? nullptr : &*found;
}
GM_SESSION* CSessionMgr::GetGMSession(UINT32 id) { return id<MAX_GM_SESSION_COUNT ? &m_GMSession[id] : nullptr; }
CServerMgr CServerMgr::m_Instance;
CServerMgr& CServerMgr::GetInstance() {return m_Instance;}
BOOL CServerMgr::Init()
{
    m_nSelectedSessionNum=static_cast<INT32>(selected.size());
    for (std::size_t i=0;i<selected.size();++i) m_SessionIDList[i]=selected[i];
    return TRUE;
}
UINT32 CServerMgr::GetSessionIDByIndex(INT32 index) {return index>=0 && index<m_nSelectedSessionNum ? m_SessionIDList[index] : UINT32(-1);}
void CServerMgr::ResetSelectedServer() {m_nSelectedSessionNum=0;}
CGMLog CGMLog::m_Instance;
CGMLog::CGMLog() = default;
CGMLog::~CGMLog() = default;
CGMLog& CGMLog::GetInstance() {return m_Instance;}
BOOL CGMLog::Log(std::string_view) {return TRUE;}
BOOL gm::output_game_log(INT32,BOOL,std::string_view text)
{
    game_logs.emplace_back(text);const auto callback=on_game_log;const auto result=game_write;
    if(callback)callback();return result;
}
void CMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession*) {}
void CMessageHandler::OnConnected(ISocketSession*) {}
void CMessageHandler::OnDisconnected(ISocketSession*,BOOL) {}

int main(int argc, char** argv)
{
    CLog::GetInstance().SetLevelMask(0);
    WSADATA winsock{};
    if (WSAStartup(MAKEWORD(2,2),&winsock)!=0) return 1;
    try
    {
        check(argc==2,"sender fixture output directory argument");
        reused_connections(); families(); role_and_text_bounds(); failures_and_callbacks(); name_query_workflow(argv[1]);bulk_workflows(std::filesystem::path(argv[1])/"bulk");log_workflow();
        CMessageHandler::GetInstance().UnInit(); CCmdManager::GetInstance().UnInit();
        std::cout << checks << " GM native sender checks passed\n";
        WSACleanup(); return 0;
    }
    catch(const std::exception& error)
    {
        std::cerr << error.what() << '\n'; WSACleanup(); return 1;
    }
}

void CMessageHandler::OnAllPing(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMVerifyAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLoginAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMItemListNtf(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLogoutAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryRoleID(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMAnnouceAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMGetAnnounceAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMExecuteGMStrAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMLoadRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMModifyRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryNameToIDAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMErrorStr(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMRoleExeGMCmd(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMSelectGroupAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryExchangeLogAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryExchangeHistoryAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMExchangePrice(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQueryOrderDetailAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMAddSystemOrderAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMDelSystemOrderAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMQuerySysExchangeInfoAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMGetExchangeLimitAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
void CMessageHandler::OnGMSetExchangeLimitAck(PACKET_BUFFER*,UINT32,ISocketSession*) {}
