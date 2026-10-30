// Actual entry point; only service, socket, dump and process boundaries are fake.
#include "fake_guard_service_api.h"
#define main guardEntry
#include "../../tools/guardserver/main.cpp"
#undef main
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace fixture {
int checks=0,failures=0,loops=0,closes=0,deletes=0,drain=0;
bool early=false,badLoop=false,badCleanup=false,throwCleanup=false,registerStop=false;
bool badRegister=false,badDispatcher=false,badModule=false,badCreate=false,badScm=false,badOpen=false,badControl=false,inactive=false;
bool dumping=false;
DWORD failStatus=0;
std::string fail,command;
std::wstring module,binary;
std::vector<std::string> events;
std::vector<SERVICE_STATUS> statuses;
bool throwNet=false,throwDestroy=false,throwAcceptor=false,throwAcceptorDestroy=false;
LPHANDLER_FUNCTION handler=nullptr;
PHANDLER_ROUTINE console=nullptr;
void check(bool value,const char* message) { ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } }
BOOL stage(const char* name) { events.emplace_back(name); return fail!=name; }
void reset() {
    loops=closes=deletes=drain=0;
    failStatus=0;
    early=badLoop=badCleanup=throwCleanup=registerStop=badRegister=badDispatcher=badModule=badCreate=badScm=badOpen=badControl=inactive=dumping=false;
    fail.clear(); command.clear(); binary.clear(); events.clear(); statuses.clear(); handler=nullptr; console=nullptr;
    g_GuardConfig={}; std::strcpy(g_GuardConfig.szServerRunPath,".");
}
int entry(std::initializer_list<const char*> args) {
    std::vector<std::string> strings; for (auto* s:args) strings.emplace_back(s);
    std::vector<char*> argv; for (auto& s:strings) argv.push_back(s.data());
    return guardEntry(static_cast<int>(argv.size()),argv.data());
}
std::size_t event(const char* name) { return std::ranges::find(events,std::string{name})-events.begin(); }
class Net:public INet {
public:
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override { return TRUE; }
    BOOL UnInit() override { events.emplace_back("net.stop"); if (throwNet) throw 1; return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { events.emplace_back("acceptor.destroy"); if (throwAcceptorDestroy) throw 2; return TRUE; }
    ISocketSession* Connect(UINT32,UINT16) override { return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
} net;
BOOL destroyNet(INT32,INet*) { events.emplace_back("net.destroy"); if (throwDestroy) throw 3; return TRUE; }
class Acceptor:public ISocketAcceptor {
public:
    BOOL Open(UINT32,UINT16) override { return TRUE; }
    BOOL Close() override { events.emplace_back("acceptor.close"); if (throwAcceptor) throw 4; return TRUE; }
    void* GetUserData() override { return nullptr; }
    void SetUserData(void*) override {}
};
void ownership() {
    Acceptor acceptor;
    for (int failure=0;failure<5;++failure) {
        events.clear();
        throwNet=failure==1; throwDestroy=failure==2; throwAcceptor=failure==3; throwAcceptorDestroy=failure==4;
        {
            guard_runtime::NetOwner network{&net,{netSimple,destroyNet}};
            guard_runtime::AcceptorOwner listening{&acceptor,{network.get()}};
            listening.reset(); listening.reset(); network.reset(); network.reset();
        }
        check(events==std::vector<std::string>{"acceptor.close","acceptor.destroy","net.stop","net.destroy"},
            "transport owners release once in dependency order even when APIs throw");
    }
    throwNet=throwDestroy=throwAcceptor=throwAcceptorDestroy=false;
}
}
using namespace fixture;
BOOL WINAPI FixtureSetServiceStatus(SERVICE_STATUS_HANDLE h,LPSERVICE_STATUS s) { check(h!=nullptr,"registered status handle"); statuses.push_back(*s); return s->dwCurrentState!=failStatus; }
SERVICE_STATUS_HANDLE WINAPI FixtureRegisterHandler(LPCSTR,LPHANDLER_FUNCTION cb) { handler=cb; if (registerStop) cb(SERVICE_CONTROL_STOP); return badRegister ? nullptr : reinterpret_cast<SERVICE_STATUS_HANDLE>(1); }
BOOL WINAPI FixtureDispatcher(const SERVICE_TABLE_ENTRYA* table) { if (badDispatcher) return FALSE; check(!table[1].lpServiceProc,"terminated typed SCM table"); table[0].lpServiceProc(0,nullptr); return TRUE; }
BOOL WINAPI FixtureConsoleHandler(PHANDLER_ROUTINE cb,BOOL add) { if (add) console=cb; else { check(console==cb,"remove installed console handler"); console=nullptr; } return TRUE; }
BOOL WINAPI FixtureConsoleTitle(LPCSTR) { return TRUE; }
DWORD WINAPI FixtureModuleName(HMODULE,LPWSTR buffer,DWORD size) { if (badModule) { SetLastError(ERROR_FILE_NOT_FOUND); return 0; } const auto length=static_cast<DWORD>(module.size()); std::copy_n(module.data(),std::min(size,length),buffer); if (length<size) buffer[length]=L'\0'; return std::min(size,length); }
SC_HANDLE WINAPI FixtureOpenSCM(LPCWSTR,LPCWSTR,DWORD access) { check(access==SC_MANAGER_CREATE_SERVICE || access==SC_MANAGER_CONNECT,"minimal SCM access"); return badScm ? nullptr : reinterpret_cast<SC_HANDLE>(1); }
SC_HANDLE WINAPI FixtureCreateService(SC_HANDLE,LPCWSTR name,LPCWSTR,DWORD access,DWORD type,DWORD start,DWORD,LPCWSTR path,LPCWSTR,LPDWORD,LPCWSTR,LPCWSTR user,LPCWSTR) {
    check(std::wstring_view{name}==L"guard_cqyh" && access==SERVICE_QUERY_STATUS && type==SERVICE_WIN32_OWN_PROCESS && start==SERVICE_AUTO_START && !user,"preserved automatic LocalSystem service"); binary=path; return badCreate ? nullptr : reinterpret_cast<SC_HANDLE>(2);
}
SC_HANDLE WINAPI FixtureOpenService(SC_HANDLE,LPCWSTR,DWORD access) { check(access==(SERVICE_STOP|DELETE),"minimal service removal access"); return badOpen ? nullptr : reinterpret_cast<SC_HANDLE>(2); }
BOOL WINAPI FixtureControlService(SC_HANDLE,DWORD control,LPSERVICE_STATUS) { check(control==SERVICE_CONTROL_STOP,"stop before removal"); SetLastError(inactive ? ERROR_SERVICE_NOT_ACTIVE : ERROR_ACCESS_DENIED); return !badControl; }
BOOL WINAPI FixtureDeleteService(SC_HANDLE) { ++deletes; return TRUE; }
BOOL WINAPI FixtureCloseService(SC_HANDLE) { ++closes; return TRUE; }
BOOL FixtureStartDump(DUMP_CHECK_FUNC,BOOL,const char* path) { check(std::filesystem::path{path}.filename()=="log","dump path component"); dumping=true; return stage("dump.init"); }
BOOL FixtureStopDump() { dumping=false; return stage("dump.stop"); }
BOOL FixtureInitStats(UINT32) { return stage("stats.init"); }
void FixtureInitReport(const char*,UINT8,const char*) { stage("report.init"); }
BOOL FixtureProcessOption(INT32,TCHAR**) { return fail=="options" ? 0 : 1; }
CGuardServer::CGuardServer():m_qwLatestTickCheckDiskSpace(0) {}
CGuardServer::~CGuardServer()=default;
CGuardServer& CGuardServer::GetInstance() { static CGuardServer value; return value; }
BOOL CGuardServer::ReloadConfig() { return stage("config"); }
BOOL CGuardServer::Init(INetEventHandler*) {
    m_pNet=guard_runtime::NetOwner{&net,{netSimple,destroyNet}};
    if (early) { if (handler) handler(SERVICE_CONTROL_STOP); else if (console) console(CTRL_C_EVENT); }
    m_bRun=true; return stage("guard.init");
}
BOOL CGuardServer::Close() { m_bRun=false; return TRUE; }
void CGuardServer::StopBackgroundWork() { stage("workers.stop"); }
BOOL CGuardServer::UnInit() { stage("guard.stop"); m_bRun=false; m_pSocketAcceptor.reset(); m_pNet.reset(); if (throwCleanup) throw std::runtime_error("fixture teardown"); return !badCleanup; }
BOOL CGuardServer::MainLoop() { ++loops; if (fail=="loop.throw") throw std::runtime_error("fixture loop"); m_bRun=false; if (drain>0) --drain; return !badLoop; }
CGDSessionMgr::CGDSessionMgr(Clock clock):m_clock(std::move(clock)) {}
CGDSessionMgr::~CGDSessionMgr()=default;
CGDSessionMgr& CGDSessionMgr::GetInstance() { static CGDSessionMgr value; return value; }
BOOL CGDSessionMgr::Init(INet*) { return stage("sessions.init"); }
BOOL CGDSessionMgr::UnInit() { return stage("sessions.stop"); }
BOOL CGDSessionMgr::CloseAllSocketSession() { return stage("sessions.close"); }
BOOL CGDSessionMgr::CheckHasSocketSession() { return drain!=0; }
CGDMessageHandler::CGDMessageHandler()=default;
CGDMessageHandler::~CGDMessageHandler()=default;
CGDMessageHandler& CGDMessageHandler::GetInstance() { static CGDMessageHandler value; return value; }
BOOL CGDMessageHandler::Init(INet*,IPacketAllocator*) { return stage("messages.init"); }
BOOL CGDMessageHandler::UnInit() { return stage("messages.stop"); }
void CGDMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession*) {}
void CGDMessageHandler::OnConnected(ISocketSession*) {}
void CGDMessageHandler::OnRecv(ISocketSession*,PACKET_BUFFER*,UINT32) {}
void CGDMessageHandler::OnDisconnected(ISocketSession*,BOOL) {}
CProcessMgr::CProcessMgr():m_qwLastStatisTick(0) {}
CProcessMgr::~CProcessMgr()=default;
CProcessMgr& CProcessMgr::GetInstance() { static CProcessMgr value; return value; }
BOOL CProcessMgr::Init() { return stage("processes.init"); }
BOOL CProcessMgr::UnInit() { return stage("processes.stop"); }
CCmdMgr::CCmdMgr():m_bNeedPrintCmdDes(FALSE) {}
CCmdMgr::~CCmdMgr()=default;
CCmdMgr& CCmdMgr::GetInstance() { static CCmdMgr value; return value; }
BOOL CCmdMgr::Init() { return stage("commands.init"); }
BOOL CCmdMgr::UnInit() { return stage("commands.stop"); }
INT32 CCmdMgr::ExecOPCmd(const char* text) { command=text; return TRUE; }
int main(int argc,char** argv) {
    try {
        if (argc!=2) throw std::runtime_error("fixture directory required");
        const auto original=std::filesystem::current_path(),folder=std::filesystem::absolute(argv[1]);
        std::filesystem::create_directories(folder); std::filesystem::current_path(folder); module=(folder/L"guard with spaces.exe").wstring();
        ownership();
        reset(); check(entry({"guard","console"})==0 && loops==1 && !console && !dumping,"successful console and cleanup");
        check(command=="console ","legacy initial command joining retained");
        check(event("sessions.stop")<event("net.stop") && event("net.destroy")<event("messages.stop") && event("messages.stop")<event("dump.stop"),"cleanup preserves borrowed socket and callback dependencies");
        reset(); check(entry({"guard","start","-g"})==0 && command=="start -g ","initial command arguments remain space-separated");
        reset(); const auto longCommand=std::string(COMMON_NAME_LEN*2,'x');
        check(entry({"guard",longCommand.c_str(),"ignored"})==0 && command==longCommand.substr(0,COMMON_NAME_LEN-1),"initial command preserves legacy byte truncation");
        reset(); badLoop=true; check(entry({"guard","console"})==1,"cleanup cannot hide loop failure");
        for (bool throwing:{false,true}) { reset(); throwCleanup=throwing; badCleanup=!throwing; check(entry({"guard","console"})==1 && event("dump.stop")<events.size(),"failed cleanup continues"); }
        for (const auto* name:{"config","dump.init","stats.init","guard.init","sessions.init","messages.init","processes.init","commands.init","options","loop.throw"}) {
            reset(); fail=name; check(entry({"guard","console"})==1 && !console && !dumping,"startup/runtime failures unwind"); check(std::ranges::count(events,std::string{"guard.stop"})==1,"cleanup exactly once");
        }
        reset(); early=true; check(entry({"guard","console"})==0 && loops==0,"stop survives initialization run flag reset");
        reset(); drain=3; check(entry({"guard","console"})==0 && loops==3,"graceful callback draining");
        reset(); drain=-1; const auto before=std::chrono::steady_clock::now(); check(entry({"guard","console"})==1 && std::chrono::steady_clock::now()-before<std::chrono::seconds(8),"bounded drain timeout");
        reset(); entry({"guard"}); check(statuses.size()==3 && statuses[0].dwCurrentState==SERVICE_START_PENDING && statuses[1].dwCurrentState==SERVICE_RUNNING && statuses[2].dwCurrentState==SERVICE_STOPPED && statuses[2].dwWin32ExitCode==0,"SCM lifecycle success");
        check(statuses[0].dwControlsAccepted==0 && statuses[1].dwControlsAccepted==SERVICE_ACCEPT_STOP && statuses[2].dwControlsAccepted==0,"only running service accepts stop");
        for (bool registration:{false,true}) { reset(); early=!registration; registerStop=registration; entry({"guard"}); check(loops==0 && std::ranges::none_of(statuses,[](auto s){return s.dwCurrentState==SERVICE_RUNNING;}),"startup stop never reports later RUNNING"); }
        reset(); fail="guard.init"; entry({"guard"}); check(statuses.back().dwCurrentState==SERVICE_STOPPED && statuses.back().dwWin32ExitCode!=0,"SCM startup error");
        reset(); badRegister=true; entry({"guard"}); check(statuses.empty() && events.empty(),"failed registration never starts runtime");
        reset(); badModule=true; entry({"guard"}); check(events.empty() && statuses.back().dwWin32ExitCode!=0,"failed service directory lookup reports stopped error");
        reset(); badDispatcher=true; check(entry({"guard"})==1,"dispatcher failure");
        for (DWORD failed:{SERVICE_START_PENDING,SERVICE_RUNNING}) {
            reset(); failStatus=failed; entry({"guard"});
            check(loops==0 && statuses.back().dwWin32ExitCode!=0,"failed SCM status prevents runtime loop");
            if (failed==SERVICE_RUNNING) check(event("dump.stop")<events.size(),"failed RUNNING report unwinds initialized resources");
        }
        reset(); check(entry({"guard","/INSTALL"})==0 && closes==2 && binary==L"\""+module+L"\"","install quotes path and closes handles");
        reset(); badCreate=true; check(entry({"guard","/install"})==1 && closes==1,"failed create closes SCM");
        reset(); badScm=true; check(entry({"guard","/install"})==1 && closes==0,"failed SCM access");
        reset(); badModule=true; check(entry({"guard","/install"})==1 && closes==0,"module lookup failure");
        reset(); const auto saved=module; module=std::wstring(600,L'x')+L".exe"; check(entry({"guard","/install"})==0 && binary.size()==module.size()+2,"dynamic module path"); module=saved;
        reset(); check(entry({"guard","/uninstall"})==0 && closes==2 && deletes==1,"uninstall closes handles");
        reset(); badControl=true; check(entry({"guard","/uninstall"})==1 && deletes==0 && closes==2,"failed stop prevents delete");
        reset(); badControl=inactive=true; check(entry({"guard","/uninstall"})==0 && deletes==1,"already stopped service removal");
        reset(); badOpen=true; check(entry({"guard","/uninstall"})==1 && closes==1,"failed service open closes SCM");
        reset(); InitServiceState();
        g_hServiceStatus=reinterpret_cast<SERVICE_STATUS_HANDLE>(1);
        reportService(SERVICE_START_PENDING);
        {
            std::jthread running{[] { for (int i=0;i<100;++i) reportService(SERVICE_RUNNING); }};
            std::jthread stopping{[] { for (int i=0;i<100;++i) ServiceCtrlHandler(SERVICE_CONTROL_STOP); }};
        }
        bool pending=false;
        for (const auto& status:statuses) {
            if (status.dwCurrentState==SERVICE_STOP_PENDING) pending=true;
            check(!(pending && status.dwCurrentState==SERVICE_RUNNING),"concurrent stop cannot regress to running");
        }
        reportService(SERVICE_STOPPED);
        ServiceCtrlHandler(SERVICE_CONTROL_STOP);
        check(statuses.back().dwCurrentState==SERVICE_STOPPED,"late stop cannot regress final SCM status");
        g_hServiceStatus=nullptr;
        {
            const std::lock_guard lock{completionMutex}; runFinished=false;
        }
        std::atomic_bool controlEntered=false,controlReturned=false;
        {
            std::jthread closing{[&] { controlEntered=true; consoleControl(CTRL_CLOSE_EVENT); controlReturned=true; }};
            while (!controlEntered) std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            check(!controlReturned,"console close gives main-thread cleanup time to finish");
            { const std::lock_guard lock{completionMutex}; runFinished=true; }
            completion.notify_all();
        }
        check(controlReturned && !consoleControl(999),"completion releases close handler and unknown controls remain unhandled");
        std::filesystem::current_path(original);
    } catch (const std::exception& e) { check(false,e.what()); }
    std::printf("guard runtime: %d checks, %d failures\n",checks,failures); return failures!=0;
}
