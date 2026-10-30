// Compile the actual entry point with in-memory application/service boundaries.
// The separate model fixture exercises the real QApplication and window.
#include "stdafx.h"
#include <QtWidgets/QApplication>
#include "mt_config.h"
#include "packet_allocator.h"
#include "message_handler.h"
#include "session_mgr.h"
#include "transmit_file_mgr.h"
#include "global_config.h"
#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

MAINTAIN_CONFIG g_MTConfig{};
namespace runtime_fixture
{
int checks=0,failures=0,eventResult=0;
bool throwing=false,appAlive=false,windowAlive=false,logAlive=false,allocatorAlive=false,netAlive=false;
bool handlerAlive=false,sessionAlive=false,transferAlive=false,receiving=false,polling=false;
std::string failure;
std::vector<std::string> events;
void check(bool value,const char* name) { ++checks;if (!value) { ++failures;std::fprintf(stderr,"FAIL: %s\n",name); } }
bool stage(const char* name)
{
    events.emplace_back(name);
    if (failure!=name) return true;
    if (throwing) throw std::runtime_error(name);
    return false;
}
std::size_t event(const char* name) { return std::ranges::find(events,std::string{name})-events.begin(); }
void before(const char* first,const char* second)
{
    if (event(first)<events.size() && event(second)<events.size())
        check(event(first)<event(second),"actual entry point preserves dependency lifetime during cleanup");
}
struct Application
{
    Application(int&,char**) { if (!stage("app")) throw std::runtime_error("app");appAlive=true; }
    ~Application() { check(!windowAlive && !netAlive,"application outlives window and native transport");appAlive=false;events.emplace_back("app_close"); }
    int exec() { check(polling && receiving && netAlive && sessionAlive,"event loop starts only after all runtime stages are ready");if (!stage("event_loop")) return 1;return eventResult; }
};
struct Window
{
    Window() { check(appAlive,"application exists before window construction");if (!stage("window")) throw std::runtime_error("window");windowAlive=true; }
    ~Window() { check(!polling && !netAlive,"window outlives polling and transport shutdown");windowAlive=false;events.emplace_back("window_close"); }
    BOOL Init() { return stage("window_init"); }
    BOOL UnInit() { polling=false;return stage("window_stop"); }
    BOOL StartPolling() { check(netAlive && receiving && sessionAlive && transferAlive,"timer starts after handler/session/transfer initialization");polling=true;return stage("poll"); }
    void show() { if (!stage("show")) throw std::runtime_error("show"); }
    void Login() { if (!stage("login")) throw std::runtime_error("login"); }
    static void LogOutput(INT32,const char*,INT32) {}
};
struct Log
{
    static Log& GetInstance() { static Log value;return value; }
    template<class... Args> BOOL Init(Args...) { logAlive=true;return stage("logger"); }
    void SetCustomOutput(void (*)(INT32,const char*,INT32)) {}
    BOOL UnInit() { check(!netAlive && !allocatorAlive,"logger outlives transport and allocator cleanup");logAlive=false;return stage("logger_close"); }
    template<class... Args> BOOL ErrLog(Args...) { return TRUE; }
    template<class... Args> BOOL InfLog(Args...) { return TRUE; }
};
struct Allocator
{
    BOOL Init(INT32,INT32) { check(logAlive,"logger is ready before allocator startup");allocatorAlive=true;return stage("allocator"); }
    BOOL UnInit() { check(!netAlive && !handlerAlive,"allocator outlives all transport callbacks");allocatorAlive=false;return stage("allocator_close"); }
    ~Allocator() { check(!allocatorAlive,"scoped allocator has been explicitly drained"); }
};
struct Handler
{
    static Handler& GetInstance() { static Handler value;return value; }
    BOOL Init(INet*,Allocator*) { handlerAlive=receiving=true;return stage("handler"); }
    void BeginShutdown() noexcept { receiving=false;events.emplace_back("receive_stop"); }
    BOOL UnInit() { check(!netAlive && allocatorAlive,"handler retains its allocator through transport destruction");handlerAlive=false;return stage("handler_close"); }
};
struct Sessions
{
    static Sessions& GetInstance() { static Sessions value;return value; }
    BOOL Init(INet*) { sessionAlive=true;return stage("sessions"); }
    BOOL UnInit() { check(netAlive && !receiving && !polling,"sessions detach while transport exists and polling is stopped");sessionAlive=false;return stage("sessions_close"); }
};
struct Transfers
{
    static Transfers& GetInstance() { static Transfers value;return value; }
    BOOL Reload() { transferAlive=true;return stage("transfers"); }
    BOOL UnInit() { check(!polling,"transfer shutdown follows timer stop");transferAlive=false;return stage("transfers_close"); }
};
class Net final:public INet
{
public:
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override { return TRUE; }
    BOOL UnInit() override { return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32,UINT16) override { check(false,"no real connection may be initiated");return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};
INet* create(INT32 type,Allocator*,Handler*,SIMPLE_INIT_PARAM* options)
{
    check(type==netSimple && allocatorAlive && options->nMaxSessionCount==37,"actual entry point forwards configured transport options");
    if (!stage("network")) return nullptr;
    netAlive=true;return new Net;
}
BOOL destroy(INT32,INet* net)
{
    check(!receiving && !polling && !sessionAlive && allocatorAlive,"transport drains after sessions but before allocator destruction");
    delete static_cast<Net*>(net);netAlive=false;return stage("network_close");
}
BOOL load(const char* path)
{
    check(std::string_view{path}=="maintaintool_config.lua","configuration path remains unchanged");
    g_MTConfig.nMaxSessionCount=37;return stage("config");
}
}

#define QApplication runtime_fixture::Application
#define MAINTAINTOOL_H
#define MaintainTool runtime_fixture::Window
#define CLog runtime_fixture::Log
#define CPacketAllocator runtime_fixture::Allocator
#define CMTMessageHandler runtime_fixture::Handler
#define CSessionMgr runtime_fixture::Sessions
#define CTransmitFileMgr runtime_fixture::Transfers
#define g_CreateNetInterface runtime_fixture::create
#define g_DestroyNetInterface runtime_fixture::destroy
#define LoadMTConfig runtime_fixture::load
#define WinMain maintainEntry
#include "../../tools/maintaintool/src/main.cpp"
#undef WinMain
#undef QApplication
#undef MaintainTool
#undef CLog
#undef CPacketAllocator
#undef CMTMessageHandler
#undef CSessionMgr
#undef CTransmitFileMgr
#undef g_CreateNetInterface
#undef g_DestroyNetInterface
#undef LoadMTConfig

int main()
{
    using namespace runtime_fixture;
    const auto run=[](const char* fail,bool throws,int result=0) {
        check(!appAlive && !windowAlive && !logAlive && !allocatorAlive && !netAlive &&
            !handlerAlive && !sessionAlive && !transferAlive && !receiving && !polling && !g_pNet,
            "previous entry point call released every owned dependency");
        events.clear();failure=fail;throwing=throws;eventResult=result;
        const auto code=maintainEntry(nullptr,nullptr,nullptr,0);
        check(code==((failure.empty()) ? result : EXIT_FAILURE),"startup/event-loop/cleanup failure reaches the process exit code");
        check(!appAlive && !windowAlive && !logAlive && !allocatorAlive && !netAlive &&
            !handlerAlive && !sessionAlive && !transferAlive && !receiving && !polling && !g_pNet,
            "success and injected failures leave no live runtime resources");
        before("receive_stop","window_stop");before("window_stop","transfers_close");
        before("window_stop","sessions_close");before("sessions_close","network_close");
        before("network_close","handler_close");before("handler_close","allocator_close");
        before("allocator_close","logger_close");before("logger_close","window_close");
        before("window_close","app_close");
        for (const auto name: {"network_close","handler_close","sessions_close","transfers_close","allocator_close","logger_close"})
            check(std::ranges::count(events,std::string{name})<=1,"runtime destructor does not repeat explicit shutdown");
    };
    run("",false);run("",false,7);
    for (const auto failure: {"app","window","config","logger","allocator","network","window_init","handler","sessions","transfers","poll","show","login","event_loop",
        "window_stop","transfers_close","sessions_close","network_close","handler_close","allocator_close","logger_close"})
        for (bool throws: {false,true}) run(failure,throws);
    std::printf("maintain runtime: %d checks, %d failures\n",checks,failures);
    return failures!=0;
}
