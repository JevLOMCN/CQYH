#include <winsock2.h>
#include "std_type.h"
#include "process_error.h"
#include "net.h"
#include "session_mgr.h"
#include "mt_config.h"
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <functional>
#include <limits>
#include <stdexcept>

MAINTAIN_CONFIG g_MTConfig{};
maintain::SessionHooks maintain::make_session_hooks()
{
    return {[](INT32) { return false; }, [](SRemoteGuardSession&, E_MT_CONNECT_STATE) { return true; },
        [](SRemoteGuardSession&) { return true; }};
}
namespace
{
int checks=0, failures=0;
void check(bool value,const char* message) { ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } }
void configure()
{
    std::memset(&g_MTConfig,0,sizeof(g_MTConfig));
    g_MTConfig.nPingTimeout=100; g_MTConfig.nReconnectTime=50; g_MTConfig.nServerPingInterval=20;
    auto& area=g_MTConfig.AreaConfig[1]; area.nAreaID=1;
    std::strcpy(area.guardList[1].szAddress,"127.0.0.1"); area.guardList[1].nPort=8810;
}
class Session:public ISocketSession
{
public:
    int closes=0; void* data=nullptr; bool fail=false; bool throwing=false;
    std::function<void()> onClose;
    INT32 Send(char*,INT32,INT32) override { return -1; }
    INT32 SendNoPackageHead(char*,INT32) override { return -1; }
    void* GetUserData() override { return data; }
    void SetUserData(void* value) override { data=value; }
    BOOL GetAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL Close() override { ++closes; if (onClose) onClose(); if (throwing) throw 7; return !fail; }
};
class Net:public INet
{
public:
    int connects=0,stops=0; std::function<ISocketSession*(UINT32,UINT16)> connect;
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override { return TRUE; }
    BOOL UnInit() override { ++stops; return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32 ip,UINT16 port) override { ++connects; return connect ? connect(ip,port) : nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};
struct Views
{
    bool expanded=true; int changes=0,pings=0; E_MT_CONNECT_STATE last=emctTotal;
    std::function<void()> onPing,onChange;
    std::function<bool(INT32)> onExpanded;
    maintain::SessionHooks hooks()
    {
        return {
            [this](INT32 area) { return onExpanded ? onExpanded(area) : expanded; },
            [this](SRemoteGuardSession&,E_MT_CONNECT_STATE state) { ++changes; last=state; if (onChange) onChange(); return true; },
            [this](SRemoteGuardSession&) { ++pings; if (onPing) onPing(); return true; }};
    }
};
constexpr UINT32 loopback=0x0100007f;
void baseline()
{
    configure(); Net net; Session socket; Views views; UINT64 tick=1;
    CSessionMgr manager{views.hooks(),[&] { return tick; }};
    check(!manager.MainLoop() && !manager.Init(nullptr),"uninitialized managers reject work");
    check(manager.Init(&net) && !manager.Init(&net),"one initialization preserves record addresses");
    auto* record=manager.GetRemoteGDSession(1,loopback);
    check(record && record->dwArea==1 && record->wPort==8810 && record->poStatisInfo &&
        record->poStatisInfo->nAccumuIndex==0 && !record->poStatisInfo->ProcessInfo[0].bUsed,"configured record owns initialized statistics");
    check(!manager.GetRemoteGDSession(0,loopback) && !manager.GetRemoteGDSession(-1,loopback) &&
        !manager.GetRemoteGDSession(MAX_AREA_COUNT,loopback) && !manager.GetRemoteGDSession(1,0),"reserved/invalid identities cannot match empty slots");
    net.connect=[&](UINT32 ip,UINT16 port) { check(ip==loopback && port==8810,"connect receives network-order address and port"); return &socket; };
    views.expanded=false;
    check(manager.MainLoop() && net.connects==0,"collapsed areas do not connect");
    views.expanded=true;
    check(manager.MainLoop() && record->pSocketSession==&socket && socket.data==record && views.last==emctConnecting,"connection stores a stable borrowed record");
    for (const char* invalid:{"0.0.0.0","255.255.255.255","127.1","256.0.0.1","127.000.0.1","1.2.3.4.5","1.2.3.-1","hostname","1.2.3.4x"})
    {
        std::strcpy(g_MTConfig.AreaConfig[1].guardList[1].szAddress,invalid);
        check(!manager.Reload() && socket.closes==0 && manager.GetRemoteGDSession(1,loopback)==record && record->pSocketSession==&socket,
            "invalid endpoint cannot disturb the live configuration");
    }
    std::memset(g_MTConfig.AreaConfig[1].guardList[1].szAddress,'x',sizeof(g_MTConfig.AreaConfig[1].guardList[1].szAddress));
    check(!manager.Reload() && socket.closes==0,"unterminated endpoint is bounded");
    std::strcpy(g_MTConfig.AreaConfig[1].guardList[1].szAddress,"127.0.0.1");
    for (int port:{0,-1,65536}) { g_MTConfig.AreaConfig[1].guardList[1].nPort=port; check(!manager.Reload() && socket.closes==0,"invalid port does not wrap into a valid port"); }
    g_MTConfig.AreaConfig[1].guardList[1].nPort=8810;
    g_MTConfig.AreaConfig[1].nAreaID=2; check(!manager.Reload() && socket.closes==0,"real area ID must match its configured index");
    g_MTConfig.AreaConfig[1].nAreaID=1;
    g_MTConfig.AreaConfig[1].guardList[2]=g_MTConfig.AreaConfig[1].guardList[1];
    check(!manager.Reload() && socket.closes==0,"duplicate area/IP identity is rejected before closing");
    g_MTConfig.AreaConfig[1].guardList[2]={};
    for (auto member:{&MAINTAIN_CONFIG::nPingTimeout,&MAINTAIN_CONFIG::nReconnectTime,&MAINTAIN_CONFIG::nServerPingInterval})
    {
        const auto saved=g_MTConfig.*member; g_MTConfig.*member=-1;
        check(!manager.Reload() && socket.closes==0,"negative timing cannot become an unsigned infinite timeout"); g_MTConfig.*member=saved;
    }
    check(manager.Reload() && socket.closes==1 && !socket.data && manager.GetRemoteGDSession(1,loopback)==record,"successful reload detaches old socket but preserves record addresses");
    check(manager.UnInit() && manager.UnInit() && net.stops==0,"cleanup repeats without destroying borrowed transport");
    CSessionMgr noClock{views.hooks(),nullptr}; check(!noClock.Init(&net),"missing clocks are rejected");
    CSessionMgr noHooks{maintain::SessionHooks{}}; check(!noHooks.Init(&net),"missing UI/network bindings are rejected");
}
void timeouts()
{
    configure(); Net net; Session first,second; Views views; UINT64 tick=(UINT64{1}<<32)+1;
    CSessionMgr manager{views.hooks(),[&] { return tick; }};
    net.connect=[&](UINT32,UINT16) -> ISocketSession* { return net.connects==1 ? &first : &second; };
    check(manager.Init(&net) && manager.MainLoop(),"monotonic timing works beyond the old 32-bit wrap");
    auto* record=manager.GetRemoteGDSession(1,loopback);
    check(record->dwTimer==tick+100,"pending connection has a bounded deadline");
    tick+=101;
    check(manager.MainLoop() && first.closes==1 && !first.data && record->dwTimer==tick+50,"even uncompleted connection attempts time out and schedule retry");
    tick+=49; manager.MainLoop(); check(net.connects==1,"reconnect delay is respected");
    tick+=2; manager.MainLoop(); check(net.connects==2 && record->pSocketSession==&second,"expired retry connects again");
    record->bConnected=TRUE; record->dwTimer=tick+1000;
    tick+=21; check(manager.MainLoop() && views.pings==1,"connected peers receive periodic pings");
    check(manager.CloseAllSession() && !second.data,"manual close detaches socket userdata");
    tick+=10000; manager.MainLoop(); check(net.connects==2,"manual close remains stopped until reload");
    tick=(std::numeric_limits<UINT64>::max)()-1;
    check(manager.Deadline(100)==(std::numeric_limits<UINT64>::max)(),"deadline addition saturates rather than wrapping");
    manager.UnInit();
    configure(); tick=1; net.connect=[](UINT32,UINT16)->ISocketSession* { throw 99; }; net.connects=0;
    check(manager.Init(&net) && !manager.MainLoop() && net.connects==1,"native connect exception becomes a failed poll");
    manager.MainLoop(); check(net.connects==1,"failed attempts retain a reconnect cooldown");
}
void reentrancy(const std::filesystem::path& directory)
{
    configure(); Net net; Session first,second; Views views; UINT64 tick=1;
    CSessionMgr manager{views.hooks(),[&] { return tick; }};
    net.connect=[&](UINT32,UINT16)->ISocketSession* { return net.connects==1 ? &first : &second; };
    manager.Init(&net); manager.MainLoop(); auto* record=manager.GetRemoteGDSession(1,loopback);
    record->lstProcess.resize(2000); record->lstProcess.back().dwProcessID=5000;
    std::filesystem::create_directories(directory);
    const auto file=std::filesystem::absolute(directory)/("owned-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+".tsv");
    record->lstProcess.back().hibernate=std::make_unique<std::ofstream>(file,std::ios::binary|std::ios::noreplace);
    check(*record->lstProcess.back().hibernate && record->lstProcess.size()==2000,"process metadata no longer has a fixed 64-entry limit");
    first.onClose=[&] {
        check(!first.data && record->lstProcess.empty(),"owned streams and userdata retire before native close");
        std::filesystem::rename(file,file.string()+".closed");
        check(!manager.Reload(),"close callbacks cannot reenter configuration commit");
    };
    check(manager.Reload() && std::filesystem::exists(file.string()+".closed"),"reload closes the hibernation stream before callbacks");
    std::filesystem::remove(file.string()+".closed");
    first.onClose={}; tick+=1; manager.MainLoop(); record->bConnected=TRUE; record->dwTimer=tick+1000;
    views.onPing=[&] { check(manager.Reload(),"ping callback may request a fresh configuration"); };
    const auto connections=net.connects; tick+=21; manager.MainLoop();
    check(net.connects==connections && !record->pSocketSession,"a reentrant reload stops the current poll from connecting replacement state");
    views.onPing={};
    views.onExpanded=[&](INT32) { manager.UnInit(); return false; };
    tick+=1; manager.MainLoop(); check(!manager.GetRemoteGDSession(1,loopback),"expanded-area callback can shut down without further table work");
    views.onExpanded={}; net.connect=[&](UINT32,UINT16)->ISocketSession* { manager.UnInit(); return &first; };
    manager.Init(&net); manager.MainLoop();
    check(!first.data && !manager.GetRemoteGDSession(1,loopback),"connection returned after shutdown closes without publishing a stale pointer");
    configure(); std::strcpy(g_MTConfig.AreaConfig[1].guardList[2].szAddress,"127.0.0.2");g_MTConfig.AreaConfig[1].guardList[2].nPort=8810;
    net.connects=0; net.connect=[&](UINT32,UINT16)->ISocketSession* { return net.connects==1 ? &first : &second; };
    manager.Init(&net); manager.MainLoop();
    const auto firstCloses=first.closes,secondCloses=second.closes;
    first.onClose=[&] { manager.UnInit(); };
    check(manager.CloseAllSession() && first.closes==firstCloses+1 && second.closes==secondCloses+1 && !first.data && !second.data,
        "nested shutdown defers record clearing until every borrowed connection closes");
    first.onClose={};
    check(manager.Init(&net),"manager can initialize after reentrant shutdown");
    net.connects=0;
    manager.MainLoop(); second.throwing=true;
    check(!manager.CloseAllSession() && !second.data,"throwing close reports failure after detachment"); second.throwing=false;
}
}
int main(int argc,char** argv)
{
    try { if (argc!=2) throw std::runtime_error("fixture directory required"); baseline(); timeouts(); reentrancy(argv[1]); }
    catch (const std::exception& error) { check(false,error.what()); }
    std::printf("maintain sessions: %d checks, %d failures\n",checks,failures); return failures!=0;
}
