#include "message_handler.h"
#include "session_mgr.h"
#include "gm_session_events.h"
#include "tl_config.h"
#include "log.h"
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>

TL_CONFIG g_TLConfig{};
namespace
{
using Clock=CSessionMgr::Clock;
using namespace std::chrono;
std::size_t checks{},pings{};
Clock::time_point tick{};
std::vector<UINT32> registrations;
std::vector<GM_SESSION> notices;
std::function<BOOL()> ping_hook;
std::function<BOOL(UINT32)> register_hook;
std::function<void(const GM_SESSION&)> notice_hook;
void check(bool value,const char* text){++checks;if(!value)throw std::runtime_error(text);}
struct Socket final : ISocketSession
{
    void* data{};
    std::size_t closes{};
    bool disconnect=true,throw_close=false,throw_bind=false;
    BOOL close_result=TRUE;
    std::function<void()> close_hook,bind_hook;
    INT32 Send(PACKET_BUFFER*,INT32,INT32) override{return 0;}
    INT32 SendNoPackageHead(PACKET_BUFFER*,INT32) override{return 0;}
    void* GetUserData() override{return data;}
    void SetUserData(void* value) override
    {
        data=value;const auto callback=bind_hook;if(value && callback)callback();
        if(value && throw_bind)throw std::runtime_error("bind failed");
    }
    BOOL GetAddress(UINT32&,UINT16&) override{return FALSE;}
    BOOL GetRemoteAddress(UINT32&,UINT16&) override{return FALSE;}
    BOOL Close() override
    {
        ++closes;const auto callback=close_hook;if(callback)callback();
        if(disconnect)CMessageHandler::GetInstance().OnDisconnected(this,FALSE);
        if(throw_close)throw std::runtime_error("close failed");return close_result;
    }
};
struct Net final : INet
{
    std::array<Socket,16> sockets;
    std::size_t next{};
    std::vector<std::pair<UINT32,UINT16>> calls;
    std::function<ISocketSession*()> connect_hook;
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override{return TRUE;}
    BOOL UnInit() override{return TRUE;}
    ISocketAcceptor* CreateAcceptor() override{return nullptr;}
    BOOL DestroyAcceptor(ISocketAcceptor*) override{return TRUE;}
    ISocketSession* Connect(UINT32 address,UINT16 port) override
    {
        calls.emplace_back(address,port);const auto callback=connect_hook;
        return callback ? callback() : &sockets.at(next++);
    }
    BOOL MainLoop() override{return TRUE;}
    INT64 PrintMemoryStat() override{return 0;}
} net;
struct Probe : CMessageHandler { using CMessageHandler::OnAllPing; };
void reset(int count=1)
{
    ping_hook={};register_hook={};notice_hook={};
    for(auto& socket:net.sockets){socket.close_hook={};socket.bind_hook={};socket.throw_close=false;socket.throw_bind=false;socket.close_result=TRUE;socket.disconnect=true;}
    auto& manager=CSessionMgr::GetInstance();manager.UnInit();
    for(auto& socket:net.sockets)socket=Socket{};
    net.next=0;net.calls.clear();net.connect_hook={};pings=0;registrations.clear();notices.clear();
    g_TLConfig={};tick=Clock::time_point{}+hours(24*80);
    check(manager.SetClock([]{return tick;}),"inject inactive steady clock");
    for(int index=0;index<count;++index)
    {
        auto& config=g_TLConfig.ServerList.arrGMList[index];config.nSvrID=501+index;config.dwPort=3000+index;
        const std::string name(100,'a'+index);std::memcpy(config.szName,name.c_str(),name.size()+1);
        std::memcpy(config.szAddr,"127.0.0.1",10);
    }
}
void baseline_and_timers()
{
    auto& manager=CSessionMgr::GetInstance();auto& handler=CMessageHandler::GetInstance();Probe probe;
    reset(2);check(!manager.Init(nullptr) && !manager.MainLoop(),"inactive/null runtime cannot operate");
    check(manager.Init(&net) && !manager.CheckSession(),"staged session configuration initialized");
    check(manager.GetGMSession(0)->name==std::string(100,'a') && !manager.GetGMSession(2) && !manager.GetGMSession(UINT32_MAX),"full configured name and bounded sparse lookup");
    check(!manager.Init(&net) && !manager.SetClock({}),"active runtime cannot be overwritten");
    check(manager.MainLoop() && net.calls.size()==2 && pings==1,"initial poll after 32-bit tick wrap connects configured peers");
    auto* first=manager.GetGMSession(0);auto* second=manager.GetGMSession(1);
    const auto first_lifetime=first->connection_lifetime;
    check(first_lifetime && second->connection_lifetime && first_lifetime!=second->connection_lifetime,"every native binding has an owned distinct lifetime");
    check(first->pSocketSession==&net.sockets[0] && net.sockets[0].data==first && first->nConnectState==SESSION_CONNECTING,"stable record bound to deferred native connection");
    check(net.calls[0]==std::pair<UINT32,UINT16>{inet_addr("127.0.0.1"),3000},"owned endpoint fields retain native network address/port");
    handler.OnConnected(&net.sockets[0]);handler.OnConnected(&net.sockets[1]);handler.OnConnected(&net.sockets[0]);
    check(registrations==std::vector<UINT32>{0,1} && first->nConnectState==SESSION_CONNECTED,"native connected callbacks register once");
    check(first->deadline==tick+milliseconds(PING_SVR_TIMEOUT),"existing heartbeat timeout retained without narrow ticks");
    manager.MainLoop();check(pings==1 && net.calls.size()==2,"same tick does not repeat work");
    tick+=milliseconds(PING_SVR_INTERNAL);manager.MainLoop();check(pings==2,"monotonic ping cadence");
    tick+=seconds(2);const auto old=first->deadline;probe.OnAllPing(nullptr,0,&net.sockets[0]);check(first->deadline>old,"actual heartbeat callback refreshes current socket deadline");
    first->eState=GM_SVR_STATE_LOGIN;handler.OnDisconnected(&net.sockets[0],TRUE);
    check(!first->connection_lifetime,"disconnect detaches connection lifetime before callbacks");
    check(!first->pSocketSession && net.sockets[0].data==nullptr && first->eState==GM_SVR_STATE_DISCONNECT && notices.back().name==std::string(100,'a'),"disconnect detaches socket before owned UI notice");
    tick+=milliseconds(RECONNECT_TIME-1);manager.MainLoop();check(net.calls.size()==2,"reconnect backoff does not fire early");
    tick+=milliseconds(1);manager.MainLoop();handler.OnConnected(&net.sockets[2]);
    check(net.calls.size()==3 && first->pSocketSession==&net.sockets[2],"disconnected peer reconnects at deadline");
    check(first->connection_lifetime && first->connection_lifetime!=first_lifetime,"reconnect publishes a new connection lifetime");
    const auto deadline=first->deadline;net.sockets[0].data=first;
    handler.OnDisconnected(&net.sockets[0],TRUE);probe.OnAllPing(nullptr,0,&net.sockets[0]);
    check(first->pSocketSession==&net.sockets[2] && first->deadline==deadline,"stale disconnect/ping cannot clear or refresh replacement socket");
    first->deadline=tick;tick+=milliseconds(1);manager.MainLoop();
    check(net.sockets[2].closes==1 && !first->pSocketSession && first->deadline==tick+milliseconds(RECONNECT_TIME),"timeout closes once and preserves callback reconnect deadline");
    check(second->pSocketSession==&net.sockets[1],"unrelated session survives peer timeout");
    manager.UnInit();check(!manager.CheckSession() && net.sockets[1].closes==1 && !manager.GetGMSession(1),"shutdown detaches remaining native session");
    manager.UnInit();check(net.sockets[1].closes==1,"repeated teardown does not close again");
}
void failures_and_reentry()
{
    auto& manager=CSessionMgr::GetInstance();auto& handler=CMessageHandler::GetInstance();
    reset();net.connect_hook=[]{return nullptr;};check(manager.Init(&net) && !manager.MainLoop(),"failed native connect reported");
    check(manager.GetGMSession(0)->nConnectState==SESSION_DISCONNECTED,"failed connect does not stick in connecting state");
    tick+=milliseconds(RECONNECT_TIME-1);manager.MainLoop();check(net.calls.size()==1,"failed connect retries with backoff");
    net.connect_hook={};tick+=milliseconds(1);check(manager.MainLoop() && net.calls.size()==2,"failed connection can retry successfully");
    reset();net.connect_hook=[]()->ISocketSession*{throw std::runtime_error("connect failure");};manager.Init(&net);
    check(!manager.MainLoop() && manager.GetGMSession(0)->nConnectState==SESSION_DISCONNECTED,"throwing connect retains retry state");
    reset();manager.Init(&net);ping_hook=[&]{check(manager.MainLoop(),"reentrant poll is deferred");return TRUE;};
    check(manager.MainLoop() && net.calls.size()==1 && pings==1,"ping callback cannot recursively duplicate connections");
    reset();manager.Init(&net);ping_hook=[&]{manager.UnInit();check(!manager.Init(&net),"runtime cannot restart inside poll frame");return TRUE;};
    check(!manager.MainLoop() && net.calls.empty(),"shutdown in ping callback stops pending connects");
    reset();manager.Init(&net);net.connect_hook=[&]()->ISocketSession*{manager.UnInit();return &net.sockets[0];};
    check(!manager.MainLoop() && net.sockets[0].closes==1 && net.sockets[0].data==nullptr,"connection returned after shutdown is closed without binding");
    reset();net.sockets[0].bind_hook=[&]{manager.UnInit();};manager.Init(&net);
    check(!manager.MainLoop() && net.sockets[0].closes==1 && !manager.CheckSession(),"shutdown while binding leaves no native reference");
    reset();net.sockets[0].throw_bind=true;manager.Init(&net);
    check(!manager.MainLoop() && net.sockets[0].closes==1 && !manager.CheckSession(),"throwing binding cleans orphan and retains retry state");
    reset(2);manager.Init(&net);manager.MainLoop();handler.OnConnected(&net.sockets[0]);handler.OnConnected(&net.sockets[1]);
    net.sockets[0].close_hook=[&]{manager.UnInit();check(!manager.Init(&net),"runtime cannot restart inside close traversal");};
    manager.GetGMSession(0)->deadline=tick;tick+=milliseconds(1);
    check(!manager.MainLoop() && net.sockets[0].closes==1 && net.sockets[1].closes==1,"shutdown inside timeout closes each socket once");
    reset(2);manager.Init(&net);manager.MainLoop();net.sockets[0].throw_close=true;
    check(!manager.UnInit() && net.sockets[0].closes==1 && net.sockets[1].closes==1 && !manager.CheckSession(),"throwing close cannot prevent remaining teardown");
    reset();manager.Init(&net);manager.MainLoop();register_hook=[](UINT32){return FALSE;};handler.OnConnected(&net.sockets[0]);
    check(net.sockets[0].closes==1 && !manager.CheckSession(),"failed registration closes current native binding");
    reset();manager.Init(&net);manager.MainLoop();handler.OnConnected(&net.sockets[0]);
    notice_hook=[&](const GM_SESSION& before){manager.UnInit();check(before.name==std::string(100,'a'),"owned disconnect metadata survives runtime reset");};
    handler.OnDisconnected(&net.sockets[0],TRUE);check(!manager.GetGMSession(0),"UI notice may stop runtime without stale writes");
    reset();manager.Init(&net);manager.MainLoop();notice_hook=[](const GM_SESSION&){throw std::runtime_error("notice failure");};
    handler.OnDisconnected(&net.sockets[0],TRUE);check(!manager.CheckSession(),"throwing disconnect notice cannot retain native binding");
}
void configuration_and_limits()
{
    auto& manager=CSessionMgr::GetInstance();
    reset(2);g_TLConfig.ServerList.arrGMList[1].nSvrID=501;check(!manager.Init(&net) && !manager.GetGMSession(0),"duplicate server IDs cannot partially publish sessions");
    reset();g_TLConfig.ServerList.arrGMList[0].dwPort=65536;check(!manager.Init(&net),"native port narrowing rejected");
    reset();std::memset(g_TLConfig.ServerList.arrGMList[0].szName,'x',128);check(!manager.Init(&net),"unterminated configured name rejected");
    reset();std::memcpy(g_TLConfig.ServerList.arrGMList[0].szAddr,"invalid",8);check(!manager.Init(&net),"invalid native address rejected before connecting");
    reset();std::memcpy(g_TLConfig.ServerList.arrGMList[0].szAddr,"0.0.0.0",8);manager.Init(&net);manager.MainLoop();
    check(net.calls[0].first==inet_addr("127.0.0.1"),"legacy zero-address loopback fallback retained");
    reset();tick=Clock::time_point::max()-milliseconds(1);manager.Init(&net);manager.MainLoop();CMessageHandler::GetInstance().OnConnected(&net.sockets[0]);
    check(manager.GetGMSession(0)->deadline==Clock::time_point::max(),"heartbeat deadline saturates instead of overflowing");
    manager.UnInit();reset();tick=Clock::time_point::min();manager.Init(&net);
    check(manager.MainLoop() && net.calls.size()==1,"initial connect does not assume positive clock epoch");manager.UnInit();
}
}
CMessageHandler CMessageHandler::ms_Instance;
CMessageHandler::CMessageHandler()=default;
CMessageHandler::~CMessageHandler()=default;
CMessageHandler& CMessageHandler::GetInstance(){return ms_Instance;}
void CMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession*){}
void CMessageHandler::OnRecv(ISocketSession*,PACKET_BUFFER*,UINT32){}
BOOL CMessageHandler::DoAllPing(){++pings;const auto callback=ping_hook;return callback ? callback() : TRUE;}
BOOL CMessageHandler::DoRegisterToGM(UINT32 id){registrations.push_back(id);const auto callback=register_hook;return callback ? callback(id) : TRUE;}
void gm::session_disconnected(const GM_SESSION& before,UINT64)
{
    notices.push_back(before);const auto callback=notice_hook;if(callback)callback(before);
}
int main()
{
    CLog::GetInstance().SetLevelMask(0);WSADATA data{};if(WSAStartup(MAKEWORD(2,2),&data)!=0)return 1;
    try
    {
        baseline_and_timers();failures_and_reentry();configuration_and_limits();
        auto& manager=CSessionMgr::GetInstance();manager.UnInit();manager.SetClock({});WSACleanup();
        std::cout<<checks<<" GM session checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';CSessionMgr::GetInstance().UnInit();WSACleanup();return 1;}
}
