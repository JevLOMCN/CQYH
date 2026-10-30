// Actual service entry point and handler; all SCM/network/allocator boundaries
// are in memory. No Windows service or network listener is started.
#include "fake_guard_service_api.h"
#define WinMain certEntry
#include "../../tools/certserver/certserver.cpp"
#undef WinMain
#include "cer_message.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>

namespace fixture
{
int checks=0, failures=0, loops=0, handleCloses=0, deletes=0, liveNets=0;
std::string fail;
bool earlyStop=false, registerStop=false, inactive=false;
DWORD failedStatus=0;
std::wstring module, binary;
std::vector<std::string> events;
std::vector<SERVICE_STATUS> states;
LPHANDLER_FUNCTION controlHandler=nullptr;
using Packets=std::map<char*,std::unique_ptr<char[]>>;
std::map<CPacketAllocator*,Packets> memory;
void check(bool value,const char* name) { ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",name); } }
bool stage(const char* name) { events.emplace_back(name); return fail!=name; }
void reset()
{
    check(memory.empty() && liveNets==0,"previous runtime released every owned allocator/network");
    loops=handleCloses=deletes=0; fail.clear(); earlyStop=registerStop=inactive=false;
    failedStatus=0; binary.clear(); events.clear(); states.clear(); controlHandler=nullptr;
}
int entry(const char* text="") { auto copy=std::string{text}; return certEntry(nullptr,nullptr,copy.data(),0); }
std::size_t event(const char* name) { return std::ranges::find(events,std::string{name})-events.begin(); }
class Session:public ISocketSession
{
public:
    IPacketAllocator* allocator;
    int sends=0, closes=0;
    explicit Session(IPacketAllocator* value):allocator(value) {}
    INT32 Send(char* packet,INT32 size,INT32) override {
        ++sends;
        InternalMessage::CER_SYNC_CERTIFICATION header{}; std::memcpy(&header,packet,sizeof(header));
        check(header.wMsg==InternalMessage::cer_sync_certification && header.byKey==206,"actual runtime sends the legacy certificate");
        allocator->Free(packet); return size;
    }
    INT32 SendNoPackageHead(char*,INT32) override { return -1; }
    void* GetUserData() override { return nullptr; }
    void SetUserData(void*) override {}
    BOOL GetAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL Close() override { ++closes; return TRUE; }
};
class Acceptor:public ISocketAcceptor
{
public:
    IPacketAllocator* allocator;
    INetEventHandler* handler;
    Acceptor(IPacketAllocator* a,INetEventHandler* h):allocator(a),handler(h) {}
    BOOL Open(UINT32 address,UINT16 port) override {
        check(address==0 && port==8111,"listener address and port preserved");
        if (!stage("acceptor.open")) return FALSE;
        if (fail=="acceptor.open.throw") throw std::runtime_error("open fixture");
        Session peer{allocator}; handler->OnAccepted(this,&peer);
        check(peer.sends==1 && peer.closes==1,"initialized handler sends and closes the memory peer");
        return TRUE;
    }
    BOOL Close() override {
        stage("acceptor.close");
        Session peer{allocator}; handler->OnAccepted(this,&peer);
        check(peer.sends==0 && peer.closes==1,"shutdown gates accepted callbacks before listener close");
        if (fail=="acceptor.close.throw") throw 1;
        return fail!="acceptor.close";
    }
    void* GetUserData() override { return nullptr; }
    void SetUserData(void*) override {}
};
class Net:public INet
{
public:
    IPacketAllocator* allocator;
    INetEventHandler* handler;
    Acceptor acceptor;
    bool callbacks=false;
    Net(IPacketAllocator* a,INetEventHandler* h):allocator(a),handler(h),acceptor(a,h) { ++liveNets; }
    ~Net() { --liveNets; events.emplace_back("network.deleted"); }
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override { return TRUE; }
    void receive()
    {
        if (!callbacks) return;
        Session peer{allocator}; auto* packet=allocator->Alloc(8);
        check(packet!=nullptr,"allocator outlives transport completion callbacks");
        handler->OnRecv(&peer,packet,8);
        check(peer.closes==1 && !allocator->IsAllocated(packet),"shutdown handler still frees native receive packets");
    }
    BOOL UnInit() override {
        stage("network.stop"); receive();
        if (fail=="network.stop.throw") throw 2;
        return fail!="network.stop";
    }
    ISocketAcceptor* CreateAcceptor() override {
        callbacks=true;
        if (!stage("acceptor.create")) return nullptr;
        if (fail=="acceptor.create.throw") throw std::runtime_error("create fixture");
        return &acceptor;
    }
    BOOL DestroyAcceptor(ISocketAcceptor* value) override {
        check(value==&acceptor,"destroy the created listener exactly once"); stage("acceptor.destroy");
        if (fail=="acceptor.destroy.throw") throw 3;
        return fail!="acceptor.destroy";
    }
    ISocketSession* Connect(UINT32,UINT16) override { return nullptr; }
    BOOL MainLoop() override {
        ++loops;
        if (fail=="loop.throw") throw std::runtime_error("loop fixture");
        if (fail=="loop") return FALSE;
        controlHandler(SERVICE_CONTROL_STOP); return TRUE;
    }
    INT64 PrintMemoryStat() override { return 0; }
};
}
using namespace fixture;

BOOL WINAPI FixtureSetServiceStatus(SERVICE_STATUS_HANDLE handle,LPSERVICE_STATUS value)
{
    check(handle!=nullptr,"only registered SCM handles report status");
    states.push_back(*value); return value->dwCurrentState!=failedStatus;
}
SERVICE_STATUS_HANDLE WINAPI FixtureRegisterHandler(LPCSTR name,LPHANDLER_FUNCTION callback)
{
    check(std::string_view{name}=="certification server","service identity preserved");
    controlHandler=callback;
    if (registerStop) callback(SERVICE_CONTROL_STOP);
    return fail=="register" ? nullptr : reinterpret_cast<SERVICE_STATUS_HANDLE>(1);
}
BOOL WINAPI FixtureDispatcher(const SERVICE_TABLE_ENTRYA* table)
{
    if (fail=="dispatch") return FALSE;
    check(table[0].lpServiceProc && !table[1].lpServiceProc,"typed SCM callback table is terminated");
    table[0].lpServiceProc(0,nullptr); return TRUE;
}
DWORD WINAPI FixtureModuleName(HMODULE,LPWSTR buffer,DWORD size)
{
    if (fail=="module") { SetLastError(ERROR_FILE_NOT_FOUND); return 0; }
    const auto length=static_cast<DWORD>(module.size());
    std::copy_n(module.data(),std::min(length,size),buffer);
    if (length<size) buffer[length]=L'\0'; return std::min(length,size);
}
SC_HANDLE WINAPI FixtureOpenSCM(LPCWSTR,LPCWSTR,DWORD access)
{
    check(access==SC_MANAGER_CREATE_SERVICE || access==SC_MANAGER_CONNECT,"operation-specific SCM access");
    return fail=="scm" ? nullptr : reinterpret_cast<SC_HANDLE>(1);
}
SC_HANDLE WINAPI FixtureCreateService(SC_HANDLE,LPCWSTR name,LPCWSTR,DWORD access,DWORD type,DWORD start,DWORD,LPCWSTR path,LPCWSTR,LPDWORD,LPCWSTR,LPCWSTR user,LPCWSTR)
{
    check(std::wstring_view{name}==L"certification server" && access==SERVICE_QUERY_STATUS && type==SERVICE_WIN32_OWN_PROCESS &&
        start==SERVICE_AUTO_START && !user,"automatic LocalSystem service semantics preserved");
    binary=path; return fail=="service.create" ? nullptr : reinterpret_cast<SC_HANDLE>(2);
}
SC_HANDLE WINAPI FixtureOpenService(SC_HANDLE,LPCWSTR,DWORD access)
{
    check(access==(SERVICE_STOP|DELETE),"removal uses stop/delete rights");
    return fail=="service.open" ? nullptr : reinterpret_cast<SC_HANDLE>(2);
}
BOOL WINAPI FixtureControlService(SC_HANDLE,DWORD code,LPSERVICE_STATUS)
{
    check(code==SERVICE_CONTROL_STOP,"stop requested before delete");
    SetLastError(inactive ? ERROR_SERVICE_NOT_ACTIVE : ERROR_ACCESS_DENIED);
    return fail!="service.stop";
}
BOOL WINAPI FixtureDeleteService(SC_HANDLE) { ++deletes; return fail!="service.delete"; }
BOOL WINAPI FixtureCloseService(SC_HANDLE) { ++handleCloses; return TRUE; }

INet* g_CreateNetInterface(INT32 type,IPacketAllocator* allocator,INetEventHandler* handler,void* parameters)
{
    check(type==netSimple,"existing simple transport retained");
    const auto& p=*static_cast<SIMPLE_INIT_PARAM*>(parameters);
    check(p.nMaxAcceptor==1 && p.nMaxConnector==1 && p.nMaxSessionCount==32 && p.nSendBufSize==1024*1024 &&
        p.nRecvBufSize==1024*1024 && p.nPacketBufferSize==4,"transport capacities preserved");
    if (!stage("network.create")) return nullptr;
    if (fail=="network.create.throw") throw std::runtime_error("network factory fixture");
    if (earlyStop) controlHandler(SERVICE_CONTROL_STOP);
    if (fail=="handler.init") {
        auto* bytes=reinterpret_cast<unsigned char*>(&g_MathTable); unsigned state=123456789;
        for (int i=0;i<128*1024;++i) { state^=state<<13; state^=state>>17; state^=state<<5; bytes[i]=static_cast<unsigned char>(state); }
    }
    return new fixture::Net(allocator,handler);
}
BOOL g_DestroyNetInterface(INT32,INet* network)
{
    auto* value=static_cast<fixture::Net*>(network);
    stage("network.destroy"); value->receive(); delete value;
    if (fail=="network.destroy.throw") throw 4;
    return fail!="network.destroy";
}

CPacketAllocator::CPacketAllocator() { m_nSmallCount=m_nLargeCount=0; }
CPacketAllocator::~CPacketAllocator() { UnInit(); }
BOOL CPacketAllocator::Init(INT32 smallCount,INT32 largeCount)
{
    check(smallCount==1024 && largeCount==256,"packet pool capacities preserved"); memory.try_emplace(this);
    return stage("allocator.init");
}
BOOL CPacketAllocator::UnInit()
{
    if (auto found=memory.find(this);found!=memory.end()) {
        check(found->second.empty(),"pool cleanup finds no outstanding packets"); memory.erase(found); stage("allocator.stop");
        return fail!="allocator.stop";
    }
    return TRUE;
}
char* CPacketAllocator::Alloc(INT32 size)
{
    auto found=memory.find(this); if (found==memory.end() || size<=0) return nullptr;
    auto value=std::make_unique<char[]>(size); auto* pointer=value.get(); found->second.emplace(pointer,std::move(value)); return pointer;
}
void CPacketAllocator::Free(char* packet) { check(memory.at(this).erase(packet)==1,"runtime receive/send packet released once"); }
BOOL CPacketAllocator::IsAllocated(char* packet) { return memory.at(this).contains(packet); }
UINT32 CPacketAllocator::PrintMemoryStat() { return 0; }
void CPacketAllocator::CheckError() {}

int main(int argc,char** argv)
{
    try {
        if (argc!=2) throw std::runtime_error("fixture directory required");
        const auto original=std::filesystem::current_path(); const auto folder=std::filesystem::absolute(argv[1]);
        std::filesystem::create_directories(folder); std::filesystem::current_path(folder);
        module=(folder/L"certificate 测试 server.exe").wstring();
        reset(); check(entry()==0 && loops==1,"successful runtime returns zero");
        check(event("acceptor.close")<event("acceptor.destroy") && event("acceptor.destroy")<event("network.stop") &&
            event("network.stop")<event("network.destroy") && event("network.deleted")<event("allocator.stop"),"resource dependency order preserved");
        check(states.front().dwCurrentState==SERVICE_START_PENDING && states.front().dwControlsAccepted==0 &&
            std::ranges::any_of(states,[](auto s){return s.dwCurrentState==SERVICE_RUNNING && s.dwControlsAccepted==SERVICE_ACCEPT_STOP;}) &&
            states.back().dwCurrentState==SERVICE_STOPPED && states.back().dwWin32ExitCode==NO_ERROR,"SCM sees readiness, stop and success");
        for (const char* failure:{"allocator.init","network.create","network.create.throw","handler.init","acceptor.create",
            "acceptor.create.throw","acceptor.open","acceptor.open.throw","loop","loop.throw","acceptor.close","acceptor.close.throw",
            "acceptor.destroy","acceptor.destroy.throw","network.stop","network.stop.throw","network.destroy","network.destroy.throw","allocator.stop"}) {
            reset(); fail=failure; check(entry()==1 && memory.empty() && liveNets==0,"partial startup/loop/cleanup failure returns nonzero and releases resources");
            check(states.back().dwCurrentState==SERVICE_STOPPED && states.back().dwWin32ExitCode!=NO_ERROR,"SCM receives failure through final cleanup");
            check(std::ranges::count(events,std::string{"network.destroy"})<=1,"network ownership is released once");
        }
        for (bool registration:{false,true}) {
            reset(); earlyStop=!registration; registerStop=registration;
            check(entry()==0 && loops==0 && std::ranges::none_of(states,[](auto s){return s.dwCurrentState==SERVICE_RUNNING;}),"startup stop survives initialization without RUNNING regression");
        }
        for (DWORD failed:{SERVICE_START_PENDING,SERVICE_RUNNING,SERVICE_STOPPED}) {
            reset(); failedStatus=failed; check(entry()==1 && memory.empty() && liveNets==0,"failed status update propagates without leaking runtime");
        }
        reset(); fail="register"; check(entry()==1 && states.empty() && events.empty(),"registration failure never reports on null handle");
        reset(); fail="dispatch"; check(entry()==1,"dispatcher failure propagates");
        reset(); check(entry("/INSTALL")==0 && binary==L"\""+module+L"\"" && handleCloses==2,"installation quotes executable path and closes handles");
        for (const char* failure:{"module","scm","service.create"}) {
            reset(); fail=failure; check(entry("/install")==1 && handleCloses==(fail=="service.create" ? 1 : 0),"failed install releases any SCM handle");
        }
        reset(); const auto saved=module; module=std::wstring(700,L'x')+L".exe";
        check(entry("/install")==0 && binary.size()==module.size()+2,"executable path grows beyond MAX_PATH"); module=saved;
        reset(); check(entry("/uninstall")==0 && deletes==1 && handleCloses==2,"successful removal closes handles");
        reset(); fail="service.stop"; check(entry("/uninstall")==1 && deletes==0 && handleCloses==2,"failed stop prevents deletion");
        reset(); fail="service.stop"; inactive=true; check(entry("/uninstall")==0 && deletes==1,"already stopped service can be removed");
        reset(); fail="service.open"; check(entry("/uninstall")==1 && handleCloses==1,"failed service open releases SCM");
        reset(); fail="service.delete"; check(entry("/uninstall")==1 && handleCloses==2,"failed delete releases handles");
        reset(); statusHandle=reinterpret_cast<SERVICE_STATUS_HANDLE>(1); status.dwCurrentState=SERVICE_START_PENDING;
        reportStatus(SERVICE_START_PENDING);
        {
            std::jthread running{[] { for (int i=0;i<100;++i) reportStatus(SERVICE_RUNNING); }};
            std::jthread stopping{[] { for (int i=0;i<100;++i) ServiceCtrlHandler(SERVICE_CONTROL_STOP); }};
        }
        bool pending=false;
        for (const auto& value:states) {
            if (value.dwCurrentState==SERVICE_STOP_PENDING) pending=true;
            check(!(pending && value.dwCurrentState==SERVICE_RUNNING),"concurrent controls cannot regress from stopping to running");
        }
        reportStatus(SERVICE_STOPPED); ServiceCtrlHandler(SERVICE_CONTROL_STOP);
        check(states.back().dwCurrentState==SERVICE_STOPPED,"late controls cannot regress final status");
        const auto count=states.size(); ServiceCtrlHandler(SERVICE_CONTROL_INTERROGATE);
        check(states.size()==count+1 && states.back().dwCurrentState==SERVICE_STOPPED,"interrogate repeats current status");
        statusHandle=nullptr;
        std::filesystem::current_path(original);
    } catch (const std::exception& error) { check(false,error.what()); }
    std::printf("cert runtime: %d checks, %d failures\n",checks,failures); return failures!=0;
}
