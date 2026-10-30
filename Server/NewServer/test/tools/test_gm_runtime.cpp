#include "message_handler.h"
#include "gm_manager.h"
#include "gm_runtime.h"
#include "gm_packet.h"
#include "gm_receive_fixture.h"
#include "gm_session_events.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "cmd_manager.h"
#include "gm_log.h"
#include "tl_config.h"
#include "log.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

TL_CONFIG g_TLConfig{};
class CRoleInfo {};
CRoleInfo* g_pRoleInfo=nullptr;
namespace
{
std::size_t checks{};
void check(bool value,const char* text){++checks;if(!value)throw std::runtime_error(text);}
struct Allocator;struct Net;
struct Context
{
    std::vector<std::string> events;
    std::string fail,throw_at,cancel_at,cleanup_fault;
    std::function<void()> poll_hook,send_hook,address_hook,connect_hook,destroy_hook,receive_hook;
    std::weak_ptr<void> lifetime;
    Allocator* allocator{};Net* net{};
    std::vector<char*> queued;
    std::size_t depth{},polls{},receives{},sends{},allocations{},frees{},nets_destroyed{},allocators_destroyed{};
    bool logger{},data{},sessions{},handler{},commands{},send_accept=true,poll_ok=true,connect_null=false;
    CSessionMgr::Clock::time_point tick=CSessionMgr::Clock::time_point{}+std::chrono::hours(24*80);
    BOOL stage(const std::string& name)
    {
        events.push_back(name);
        if(name==cancel_at)
        {
            check(CGMManager::GetInstance().UnInit(),"startup cancellation accepted");
            check(!CGMManager::GetInstance().Init(),"reentrant startup rejected");
        }
        if(name==throw_at)throw std::runtime_error("fixture startup failure");return name!=fail;
    }
};
std::shared_ptr<Context> current;
struct Frame
{
    std::shared_ptr<Context> context;
    explicit Frame(std::shared_ptr<Context> value):context(std::move(value)){++context->depth;}
    ~Frame(){--context->depth;}
};
struct Allocator final : IPacketAllocator
{
    std::shared_ptr<Context> context;
    std::map<char*,std::unique_ptr<char[]>> live;
    explicit Allocator(std::shared_ptr<Context> value):context(std::move(value)){context->allocator=this;}
    ~Allocator()
    {
        if(!live.empty() || context->net || context->depth)std::abort();
        context->events.push_back("allocator.destroy");context->allocator=nullptr;++context->allocators_destroyed;
    }
    PACKET_BUFFER* Alloc(INT32 size) override
    {
        auto storage=std::make_unique<char[]>(size);auto* packet=storage.get();live.emplace(packet,std::move(storage));++context->allocations;return packet;
    }
    void Free(PACKET_BUFFER* packet) override{if(live.erase(packet)!=1)std::abort();++context->frees;}
    BOOL IsAllocated(PACKET_BUFFER* packet) override{return live.contains(packet);}
    UINT32 PrintMemoryStat() override{return static_cast<UINT32>(live.size());}
    void CheckError() override{}
};
struct Socket final : ISocketSession
{
    std::shared_ptr<Context> context;void* data{};
    explicit Socket(std::shared_ptr<Context> value):context(std::move(value)){}
    INT32 Send(PACKET_BUFFER* packet,INT32 size,INT32) override
    {
        Frame frame(context);++context->sends;
        const auto callback=context->send_hook;if(callback)callback();
        check(context->net && context->allocator && context->allocator->live.contains(packet),"native send retains transport/allocator/packet across callback");
        if(!context->send_accept)return 0;
        context->queued.push_back(packet);return size;
    }
    INT32 SendNoPackageHead(PACKET_BUFFER*,INT32) override{return 0;}
    void* GetUserData() override{return data;}
    void SetUserData(void* value) override{data=value;}
    BOOL GetAddress(UINT32& address,UINT16& port) override
    {
        Frame frame(context);const auto callback=context->address_hook;if(callback)callback();
        check(context->net && context->allocator,"address callback retains owning runtime");address=inet_addr("127.0.0.1");port=1234;return TRUE;
    }
    BOOL GetRemoteAddress(UINT32&,UINT16&) override{return FALSE;}
    BOOL Close() override
    {
        Frame frame(context);CMessageHandler::GetInstance().OnDisconnected(this,FALSE);return TRUE;
    }
};
struct Net final : INet
{
    std::shared_ptr<Context> context;Socket socket;
    explicit Net(std::shared_ptr<Context> value):context(value),socket(std::move(value)){context->net=this;}
    ~Net()
    {
        if(context->depth || socket.data)std::abort();context->events.push_back("net.destroy");context->net=nullptr;++context->nets_destroyed;
    }
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override{return TRUE;}
    BOOL UnInit() override{return TRUE;}
    ISocketAcceptor* CreateAcceptor() override{return nullptr;}
    BOOL DestroyAcceptor(ISocketAcceptor*) override{return TRUE;}
    ISocketSession* Connect(UINT32,UINT16) override
    {
        Frame frame(context);const auto callback=context->connect_hook;if(callback)callback();
        return context->connect_null ? nullptr : &socket;
    }
    BOOL MainLoop() override
    {
        Frame frame(context);++context->polls;const auto callback=context->poll_hook;if(callback)callback();return context->poll_ok;
    }
    INT64 PrintMemoryStat() override{return 0;}
};
void receive(const std::shared_ptr<Context>& context)
{
    const auto bytes=gm_test::receive_packet(InternalMessage::gm2tl_error_str);
    auto* packet=context->allocator->Alloc(static_cast<INT32>(bytes.size()));std::memcpy(packet,bytes.data(),bytes.size());
    CMessageHandler::GetInstance().OnRecv(&context->net->socket,packet,static_cast<UINT32>(bytes.size()));
}
std::shared_ptr<Context> fresh()
{
    CGMManager::GetInstance().UnInit();check(!CGMManager::GetInstance().IsRetiring(),"prior runtime fully retired");
    current=std::make_shared<Context>();return current;
}
void balanced(const std::shared_ptr<Context>& context)
{
    check(!context->allocator && !context->net && !context->logger && !context->sessions && !context->handler && !context->commands && !context->data,"all attempted runtime stages cleaned");
    check(context->lifetime.expired() && context->allocations==context->frees,"runtime owner and packet ownership released");
    check(!CGMManager::GetInstance().IsRunning() && !CGMManager::GetInstance().IsRetiring(),"stopped runtime can start again");
}
void bootstrap()
{
    auto& manager=CGMManager::GetInstance();check(manager.Init(),"runtime starts");
    check(manager.MainLoop(),"runtime establishes deferred connection");CMessageHandler::GetInstance().OnConnected(&current->net->socket);
    check(CSessionMgr::GetInstance().GetGMSession(0)->nConnectState==SESSION_CONNECTED,"actual session and sender registered through fake transport");
}
void startup()
{
    auto& manager=CGMManager::GetInstance();
    for(const auto* stage:{"log","config","allocator","net","sessions","handler","commands","data"})
    {
        auto context=fresh();context->fail=stage;check(!manager.Init(),"failed startup stage rejected");balanced(context);
        context=fresh();context->throw_at=stage;check(!manager.Init(),"throwing startup stage rejected");balanced(context);
        context=fresh();context->cancel_at=stage;check(!manager.Init(),"cancelled startup stage rejected");balanced(context);
    }
    auto context=fresh();bootstrap();const auto events=context->events;
    check(!manager.Init() && context->events==events,"active runtime cannot be overwritten");
    manager.UnInit();balanced(context);
    const auto net=std::ranges::find(context->events,"net.destroy"),allocator=std::ranges::find(context->events,"allocator.destroy"),log=std::ranges::find(context->events,"log.stop");
    check(net<allocator && allocator<log,"native transport retires before allocator and logger");
    manager.UnInit();check(context->nets_destroyed==1 && context->allocators_destroyed==1,"repeated shutdown is idempotent");
}
void callback_lifetimes()
{
    auto& manager=CGMManager::GetInstance();
    auto context=fresh();bootstrap();context->poll_hook=[context]
    {
        check(CGMManager::GetInstance().MainLoop(),"nested manager poll deferred");CGMManager::GetInstance().UnInit();
        check(context->net && context->allocator && CGMManager::GetInstance().IsRetiring(),"shutdown request retains active native loop");
        check(!CGMManager::GetInstance().Init(),"retiring runtime cannot restart in callback");
    };
    check(manager.MainLoop(),"intentional callback shutdown is not a fatal loop failure");context->poll_hook={};balanced(context);
    context=fresh();bootstrap();context->receive_hook=[context]
    {
        CGMManager::GetInstance().UnInit();check(context->net && context->allocator,"receive callback retains packet allocator");
    };context->poll_hook=[context]{receive(context);check(context->net,"outer native frame remains alive after receive returns");};
    manager.MainLoop();context->poll_hook={};context->receive_hook={};balanced(context);
    for(const bool accepted:{false,true})
    {
        context=fresh();bootstrap();context->send_accept=accepted;
        context->send_hook=[]{CGMManager::GetInstance().UnInit();};
        check((CMessageHandler::GetInstance().DoReleaseRole(0,42)!=FALSE)==accepted,"direct native send reports acceptance after callback shutdown");
        context->send_hook={};balanced(context);
    }
    context=fresh();bootstrap();context->address_hook=[]{CGMManager::GetInstance().UnInit();};
    check(!CMessageHandler::GetInstance().DoRegisterToGM(0),"address callback invalidates registration before send");context->address_hook={};balanced(context);
    context=fresh();bootstrap();std::shared_ptr<void> lease;
    context->receive_hook=[context,&lease]{lease=context->lifetime.lock();CGMManager::GetInstance().UnInit();};receive(context);context->receive_hook={};
    check(lease && manager.IsRetiring() && !manager.Init(),"explicit packet/runtime lease defers retirement");
    context->destroy_hook=[]{check(!CGMManager::GetInstance().Init(),"deferred native destruction blocks reinit after shutdown call returned");};
    lease.reset();context->destroy_hook={};balanced(context);
    context=fresh();bootstrap();context->destroy_hook=[context]
    {
        check(!CGMManager::GetInstance().Init(),"reinit blocked throughout native destruction");
        const auto before=context->receives;receive(context);check(context->receives==before,"late teardown packet freed without UI delivery");
    };manager.UnInit();context->destroy_hook={};balanced(context);
    context=fresh();bootstrap();context->poll_ok=false;check(!manager.MainLoop(),"fatal transport failure reported");balanced(context);
    context=fresh();bootstrap();context->poll_hook=[]{throw std::runtime_error("native poll failure");};
    check(!manager.MainLoop(),"throwing native poll shuts down safely");context->poll_hook={};balanced(context);
    context=fresh();check(manager.Init(),"transient retry runtime starts");context->connect_null=true;
    check(manager.MainLoop() && manager.IsRunning(),"transient session connection failure does not terminate application");context->connect_null=false;
    context->tick+=std::chrono::milliseconds(RECONNECT_TIME);check(manager.MainLoop() && manager.IsRunning(),"session retry remains operational");manager.UnInit();balanced(context);
    for(const auto* fault:{"handler","sessions","commands","data","net","allocator","log"})
    {
        context=fresh();bootstrap();context->cleanup_fault=fault;manager.UnInit();balanced(context);
        check(!manager.CleanupSucceeded(),"cleanup failure recorded while remaining resources retire");
    }
}
}
gm::RuntimeHooks gm::runtime_hooks()
{
    const auto context=current;RuntimeHooks hooks;
    hooks.log_start=[context]{context->logger=true;return context->stage("log");};
    hooks.log_stop=[context]{context->events.push_back("log.stop");context->logger=false;return context->cleanup_fault!="log";};
    hooks.load_config=[context]
    {
        g_TLConfig={};auto& config=g_TLConfig.ServerList.arrGMList[0];config.nSvrID=1;config.dwPort=3000;
        std::memcpy(config.szName,"fixture",8);std::memcpy(config.szAddr,"127.0.0.1",10);return context->stage("config");
    };
    hooks.allocator_create=[context]()->IPacketAllocator*{return context->stage("allocator") ? new Allocator(context) : nullptr;};
    hooks.allocator_destroy=[context](IPacketAllocator* value){delete static_cast<Allocator*>(value);return context->cleanup_fault!="allocator";};
    hooks.net_create=[context](IPacketAllocator*,INetEventHandler*)->INet*{return context->stage("net") ? new Net(context) : nullptr;};
    hooks.net_destroy=[context](INet* value)
    {
        if(context->depth)std::abort();
        for(auto* packet:context->queued)context->allocator->Free(packet);context->queued.clear();
        const auto callback=context->destroy_hook;if(callback)callback();delete static_cast<Net*>(value);return context->cleanup_fault!="net";
    };
    hooks.sessions_start=[context](INet* net)
    {
        context->sessions=true;auto& sessions=CSessionMgr::GetInstance();sessions.SetClock([context]{return context->tick;});
        return sessions.Init(net) && context->stage("sessions");
    };
    hooks.sessions_stop=[context]
    {context->events.push_back("sessions.stop");context->sessions=false;CSessionMgr::GetInstance().UnInit();CSessionMgr::GetInstance().SetClock({});return context->cleanup_fault!="sessions";};
    hooks.sessions_poll=[]{return CSessionMgr::GetInstance().MainLoop();};
    hooks.handler_start=[context](INet* net,IPacketAllocator* allocator,std::shared_ptr<void> lifetime)
    {context->handler=true;context->lifetime=lifetime;return CMessageHandler::GetInstance().Init(net,allocator,std::move(lifetime)) && context->stage("handler");};
    hooks.handler_stop=[context]{context->events.push_back("handler.stop");context->handler=false;CMessageHandler::GetInstance().UnInit();return context->cleanup_fault!="handler";};
    hooks.release_allocator=[context](IPacketAllocator* allocator)
    {check(!context->net && context->allocator,"allocator detached after native transport destruction");CMessageHandler::GetInstance().ReleaseAllocator(allocator);};
    hooks.commands_start=[context]{context->commands=true;return CCmdManager::GetInstance().Init() && context->stage("commands");};
    hooks.commands_stop=[context]{context->events.push_back("commands.stop");context->commands=false;CCmdManager::GetInstance().UnInit();return context->cleanup_fault!="commands";};
    hooks.commands_poll=[]{return CCmdManager::GetInstance().MainLoop();};
    hooks.data_start=[context]{context->data=true;return context->stage("data");};
    hooks.data_stop=[context]{context->events.push_back("data.stop");context->data=false;return context->cleanup_fault!="data";};return hooks;
}
CServerMgr CServerMgr::m_Instance;
CServerMgr& CServerMgr::GetInstance(){return m_Instance;}
BOOL CServerMgr::Init(){m_nSelectedSessionNum=1;m_SessionIDList[0]=0;return TRUE;}
UINT32 CServerMgr::GetSessionIDByIndex(INT32 index){return index==0 ? 0 : UINT32(-1);}
void CServerMgr::ResetSelectedServer(){m_nSelectedSessionNum=0;}
CGMLog CGMLog::m_Instance;
CGMLog::CGMLog()=default;CGMLog::~CGMLog()=default;
CGMLog& CGMLog::GetInstance(){return m_Instance;}
BOOL CGMLog::Log(std::string_view){return TRUE;}
void CMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession*){}
void gm::session_disconnected(const GM_SESSION&,UINT64){}
int main()
{
    CLog::GetInstance().SetLevelMask(0);WSADATA data{};if(WSAStartup(MAKEWORD(2,2),&data)!=0)return 1;
    try
    {
        CServerMgr::GetInstance().Init();startup();callback_lifetimes();
        CGMManager::GetInstance().UnInit();current.reset();WSACleanup();
        std::cout<<checks<<" GM runtime checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';CGMManager::GetInstance().UnInit();WSACleanup();return 1;}
}

void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMVerifyAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMLoginAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMItemListNtf(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMLogoutAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMPreQueryAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQueryRoleID(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQueryRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMForbidChatAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMFreezeRoleAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMKickRoleAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMSendMailAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMAnnouceAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMGetAnnounceAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMLogAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMExecuteGMStrAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMLoadRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMModifyRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQueryNameToIDAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMErrorStr(PACKET_BUFFER*,UINT32,ISocketSession* socket) {auto context=static_cast<Socket*>(socket)->context;++context->receives;const auto callback=context->receive_hook;if(callback)callback();}

void CMessageHandler::OnGMRoleExeGMCmd(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMSelectGroupAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQueryExchangeLogAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQueryExchangeHistoryAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMExchangePrice(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQueryOrderDetailAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMAddSystemOrderAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMDelSystemOrderAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMQuerySysExchangeInfoAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMGetExchangeLimitAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}

void CMessageHandler::OnGMSetExchangeLimitAck(PACKET_BUFFER*,UINT32,ISocketSession* socket) {}
