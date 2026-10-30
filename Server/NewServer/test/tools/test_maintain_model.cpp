// Real Qt window/message/session code with memory sockets and disposable files.
// Close queues disconnect; the fixture delivers that callback explicitly.
#include "stdafx.h"
#include "maintaintool.h"
#include "session_mgr.h"
#include "message_handler.h"
#include "packet_allocator.h"
#include "maintain_tool_version.h"
#include "maintain_files.h"
#include "math_ex.h"
#include "maintain_stats.h"
#include <QGraphicsItem>
#include <cmath>
#include <QDesktopServices>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFontDatabase>
#include "test_maintain_url_handler.h"
#include "transmit_file_mgr.h"
#include <QApplication>
#include <QList>
#include <QVector>
#include <QTreeWidget>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <atomic>
#include <thread>
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QTimerEvent>
#include <QListWidget>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

INet* g_pNet=nullptr;
namespace
{
using namespace InternalMessage;
namespace fs=std::filesystem;
int checks=0,failures=0;
void check(bool value,const char* description) { ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",description); } }
class Allocator:public IPacketAllocator
{
public:
    bool rejectAllocation=false,throwAllocation=false;
    std::map<char*,std::unique_ptr<char[]>> packets;
    char* Alloc(INT32 size) override { if (throwAllocation) throw std::bad_alloc{};if (rejectAllocation || size<=0) return nullptr; auto p=std::make_unique<char[]>(size); auto* data=p.get(); packets.emplace(data,std::move(p)); return data; }
    void Free(char* packet) override { check(packets.erase(packet)==1,"Qt handler releases packet once"); }
    BOOL IsAllocated(char* p) override { return packets.contains(p); }
    UINT32 PrintMemoryStat() override { return 0; }
    void CheckError() override {}
};
class Net:public INet
{
public:
    int polls=0;
    std::function<BOOL()> onPoll;
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override { return TRUE; }
    BOOL UnInit() override { return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32,UINT16) override { throw std::runtime_error("fixture must never initiate a native connection"); }
    BOOL MainLoop() override { ++polls;return onPoll ? onPoll() : TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};
class Session:public ISocketSession
{
public:
    Allocator& allocator; void* data=nullptr; int closes=0; std::vector<UINT16> sent;
    std::vector<std::unique_ptr<N2U_SEND_FILES>> chunks;
    std::function<void(const N2U_SEND_FILES&)> onChunk;
    bool rejectSend=false,throwSend=false;
    bool rejectClose=false;
    std::function<void()> onClose;
    explicit Session(Allocator& value):allocator(value) {}
    INT32 Send(char* packet,INT32 size,INT32 offset) override {
        if (throwSend) throw std::runtime_error("injected send failure");
        if (rejectSend) return 0;
        MESSAGE_HEADER h{}; std::memcpy(&h,packet+offset,sizeof(h)); sent.push_back(h.wMsg);
        N2U_SEND_FILES* chunk=nullptr;
        if (h.wMsg==n2u_send_files) {
            check(size==sizeof(N2U_SEND_FILES),"transfer retains the fixed packet ABI");
            auto copy=std::make_unique<N2U_SEND_FILES>();std::memcpy(copy.get(),packet+offset,sizeof(*copy));
            chunk=copy.get();chunks.push_back(std::move(copy));
        }
        allocator.Free(packet);
        if (chunk && onChunk) onChunk(*chunk);
        return size;
    }
    INT32 SendNoPackageHead(char*,INT32) override { return -1; }
    void* GetUserData() override { return data; }
    void SetUserData(void* value) override { data=value; }
    BOOL GetAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32& ip,UINT16& port) override { ip=0x0100007f;port=8810;return TRUE; }
    BOOL Close() override { ++closes;if (onClose) onClose();return !rejectClose; }
};
template<class Message> void deliver(Allocator& allocator,Session& socket,const Message& message)
{
    auto* packet=allocator.Alloc(sizeof(message)); std::memcpy(packet,&message,sizeof(message));
    CMTMessageHandler::GetInstance().OnRecv(&socket,packet,sizeof(message));
    check(allocator.packets.empty(),"message and response ownership balance through actual Qt callbacks");
}
struct Fixture
{
    fs::path original=fs::current_path(),base,root;
    explicit Fixture(const fs::path& parent)
    {
        fs::create_directories(parent); base=fs::canonical(parent);
        root=base/("maintain-model-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!fs::create_directory(root)) throw std::runtime_error("fixture must be new");
        fs::create_directories(root/"area_config/1_Test/127.0.0.1"); fs::create_directory(root/"version");fs::create_directory(root/"patch");
        fs::current_path(root);
    }
    ~Fixture()
    {
        std::error_code error;fs::current_path(original,error);
        if (root.parent_path()==base && root.filename().string().starts_with("maintain-model-") && fs::canonical(root,error)==root && !error)
            fs::remove_all(root,error);
    }
};
#include "test_maintain_transfers.inl"
#include "test_maintain_gui_lifecycle.inl"
#include "test_maintain_config.inl"
#include "test_maintain_exports.inl"
#include "test_maintain_statistics.inl"
#include "test_maintain_final.inl"
void run(const fs::path& parent)
{
    Fixture fixture{parent}; Allocator allocator; Net net; Session socket{allocator};
    g_pNet=&net;
    struct Cleanup
    {
        ~Cleanup()
        {
            CMTMessageHandler::GetInstance().BeginShutdown();
            CTransmitFileMgr::GetInstance().UnInit();
            CSessionMgr::GetInstance().UnInit();
            CMTMessageHandler::GetInstance().UnInit();
            g_pNet=nullptr;
        }
    } cleanup;
    std::memset(&g_MTConfig,0,sizeof(g_MTConfig));
    std::strcpy(g_MTConfig.szAreaConfigPath,"area_config");std::strcpy(g_MTConfig.szVersionPath,"version");std::strcpy(g_MTConfig.szPatchPath,"patch");
    g_MTConfig.nPingTimeout=1000;g_MTConfig.nReconnectTime=500;g_MTConfig.nServerPingInterval=100;
    auto& window=MaintainTool::GetInstance();
    check(window.Init(),"real window scans disposable area/version/patch directories");
    auto* areas=window.findChild<QTreeWidget*>("treeArea");auto* processes=window.findChild<QTreeWidget*>("treeGuardProcesses");
    check(areas && processes && areas->topLevelItemCount()==1 && areas->topLevelItem(0)->childCount()==1,
        "Qt tree uses the actual nAreaID and configured endpoint");
    if (!areas || !processes || areas->topLevelItemCount()!=1) throw std::runtime_error("missing fixture widgets");
    areas->topLevelItem(0)->child(0)->setSelected(true);
    check(CSessionMgr::GetInstance().Init(&net),"real singleton session bindings initialize without native transport");
    check(CMTMessageHandler::GetInstance().Init(&net,&allocator),"real message handler initialized with owned memory packets");
    auto* record=CSessionMgr::GetInstance().GetRemoteGDSession(1,0x0100007f);
    if (!record) throw std::runtime_error("missing configured endpoint");
    record->pSocketSession=&socket;socket.SetUserData(record);
    CMTMessageHandler::GetInstance().OnConnected(&socket);
    check(record->bConnected && !socket.sent.empty() && socket.sent.back()==n2u_regrister && allocator.packets.empty(),
        "connected callback registers through the in-memory peer");
    U2N_REGRISTER_ACK ack{};ack.wMsg=u2n_regrister_ack;ack.nMsgVersion=MAINTAIN_TOOLS_MSG_VERSION;
    std::strcpy(ack.szBuildVersion,MAINTAIN_TOOLS_BUILD_VERSION);std::strcpy(ack.szVersion,"release.zip\r\n");
    deliver(allocator,socket,ack);
    for (UINT32 index=0;index<96;++index)
    {
        N2U_UPDATE_SESSION process{};process.wMsg=n2u_update_process_info;process.dwProcessID=100+index;
        process.byType=stGS;process.dwSessionID=1+index%63;process.nState=epsNormal;deliver(allocator,socket,process);
    }
    check(record->lstProcess.size()==96 && processes->topLevelItemCount()==96,"actual GUI displays more process records than the removed fixed limit");
    auto stats=std::make_unique<U2N_UPDATE_STATIS_INFO>();stats->wMsg=u2n_update_statis_info;
    stats->oStatisInfo.ProcessInfo[0].bUsed=TRUE;stats->oStatisInfo.ProcessInfo[0].dwProcessID=100;
    stats->oStatisInfo.ProcessInfo[0].qwStatisEntry[statMemUsage]=8192;stats->oStatisInfo.ProcessInfo[0].qwStatisEntry[statCpuUsage]=50;
    deliver(allocator,socket,*stats);
    check(record->poStatisInfo->nAccumuIndex==1 && record->poStatisInfo->ProcessInfo[0].qwStatisEntry[statCpuUsage]==50,
        "statistics snapshot is retained in owned per-guard history");
    statistics_ui_tests(fixture.root,window,allocator,socket,*record,*processes);
    send_path_tests(fixture.root,window,allocator,socket,*record);
    export_tests(fixture.root,window,allocator,socket,*record,*processes);
    check(window.BeginHibernateGuardStatisInfoByIp(1,0x0100007f,100,stGS,1),"actual hibernation opens an owned standard stream");
    check(window.ProcessHibernateGuardStatisInfoByIp(&record->poStatisInfo->ProcessInfo[0],&record->lstProcess.front()),"actual hibernation writes and flushes a row");
    fs::path hibernate;
    for (const auto& entry:fs::directory_iterator(fixture.root/"log"))
        if (entry.path().filename().string().starts_with("hibernate_") && entry.path().extension()==".tsv") hibernate=entry.path();
    check(!hibernate.empty(),"hibernation file is published beneath the fixture log directory");
    {
        std::ifstream stream{hibernate,std::ios::binary};std::string text{std::istreambuf_iterator<char>{stream},{}};
        check(text.starts_with("time\tMemUsage\tCpuUsage\t") && text.find("\t8192\t50\t")!=text.npos,"TSV header and column ordering preserved");
    }
    N2U_UPDATE_SESSION removed{};removed.wMsg=n2u_update_process_info;removed.dwProcessID=100;removed.nState=epsDestroy;
    deliver(allocator,socket,removed);
    fs::rename(hibernate,hibernate.string()+".closed");
    check(record->lstProcess.size()==95 && processes->topLevelItemCount()==95,"process removal closes its stream and updates GUI records");
    transfer_tests(fixture.root,allocator,socket,*areas->topLevelItem(0)->child(0));
    const auto reject=[&](UINT16 id,std::size_t length) {
        auto* packet=allocator.Alloc(static_cast<INT32>(length));std::memset(packet,0x7f,length);
        MESSAGE_HEADER h{};h.wMsg=id;std::memcpy(packet,&h,sizeof(h));const auto closes=socket.closes;
        CMTMessageHandler::GetInstance().OnRecv(&socket,packet,static_cast<UINT32>(length));
        check(socket.closes==closes+1 && allocator.packets.empty(),"real receiver rejects malformed bodies before Qt access");
    };
    const auto logMask=CLog::GetInstance().GetLevelMask();CLog::GetInstance().SetLevelMask(0);
    for (const auto [id,length]:std::array<std::pair<UINT16,std::size_t>,5>{{{u2n_regrister_ack,sizeof(U2N_REGRISTER_ACK)},
        {n2u_update_process_info,sizeof(N2U_UPDATE_SESSION)},{u2n_sync_processes_summary,sizeof(U2N_SYNC_DEFAULT_INFO)},
        {u2n_send_files_ack,sizeof(U2N_SEND_FILES_ACK)},{u2n_update_statis_info,sizeof(U2N_UPDATE_STATIS_INFO)}}})
        for (std::size_t size=sizeof(MESSAGE_HEADER);size<length;++size) reject(id,size);
    reject(65535,sizeof(MESSAGE_HEADER));reject(u2n_regrister_ack,sizeof(U2N_REGRISTER_ACK));
    CLog::GetInstance().SetLevelMask(logMask);
    config_refresh_tests(fixture.root,window,socket,*record,*areas,*processes);
    QList<int> left{1,2,3},right{1,2,3};QVector<int> a{4,5},b{4,5};
    check(left==right && a==b,"Qt header algorithms work with standard span iterators in strict mode");
    connection_statistics_tests(window,allocator,socket,net);
    CMTMessageHandler::GetInstance().OnDisconnected(&socket,FALSE);
    check(!socket.data && !record->pSocketSession && record->lstProcess.empty(),"disconnect clears owned process resources");
    CMTMessageHandler::GetInstance().BeginShutdown();
    check(CSessionMgr::GetInstance().UnInit() && CMTMessageHandler::GetInstance().UnInit() && allocator.packets.empty(),"Qt fixture cleanup releases borrowed sessions before allocator death");
    login_tests(window);
    gui_lifecycle_tests(window,net);
    g_pNet=nullptr;
}
}
int main(int argc,char** argv)
{
    try {
        QApplication application{argc,argv};
        const auto font=QFontDatabase::addApplicationFont(qEnvironmentVariable("WINDIR")+"/Fonts/arial.ttf");
        const auto families=QFontDatabase::applicationFontFamilies(font);
        check(font>=0 && !families.empty(),"offscreen GUI fixture loads a real system font for visual validation");
        if (!families.empty()) QApplication::setFont(QFont{families.front(),9});
        bool missing=false;
        try { MaintainTool::GetInstance(); } catch (const std::logic_error&) { missing=true; }
        check(missing,"window is not globally constructed before the application");
        int destroyedScenes=0;
        std::jthread lateLogger;
        std::atomic<bool> lateStarted=false;
        {
            MaintainTool window;
            for (auto* scene:window.findChildren<QGraphicsScene*>())
                QObject::connect(scene,&QObject::destroyed,&application,[&] { ++destroyedScenes; });
            bool duplicate=false;
            try { MaintainTool other; } catch (const std::logic_error&) { duplicate=true; }
            check(duplicate && &MaintainTool::GetInstance()==&window,"one scoped window owns singleton access");
            if (argc!=2) throw std::runtime_error("fixture root required");run(argv[1]);
            lateLogger=std::jthread{[&](std::stop_token stop) {
                while (!stop.stop_requested()) {
                    MaintainTool::LogOutput(lltInfo,"racing destruction",18);
                    lateStarted=true;
                    std::this_thread::yield();
                }
            }};
            while (!lateStarted.load()) std::this_thread::yield();
        }
        lateLogger.request_stop();lateLogger.join();
        check(destroyedScenes==10,"window destruction releases every owned graphics scene");
        missing=false;
        try { MaintainTool::GetInstance(); } catch (const std::logic_error&) { missing=true; }
        check(missing,"window singleton is detached before QApplication destruction");
        MaintainTool::LogOutput(lltInfo,"late",4);
        QCoreApplication::sendPostedEvents(nullptr,QEvent::MetaCall);
    }
    catch (const std::exception& error) { check(false,error.what()); }
    std::printf("maintain model: %d checks, %d failures\n",checks,failures);return failures!=0;
}
