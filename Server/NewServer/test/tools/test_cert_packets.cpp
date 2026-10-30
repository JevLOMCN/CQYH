#include <winsock2.h>
#include <windows.h>
#include "std_type.h"
#include "process_error.h"
#include "cert_message_handler.h"
#include "math_ex.h"
#include "cer_message.h"
#include "zlib.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

namespace {
int checks=0, failures=0;
void check(bool value,const char* message) { ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } }
class Allocator:public IPacketAllocator {
public:
    bool fail=false;
    int frees=0;
    std::map<char*,std::unique_ptr<char[]>> packets;
    PACKET_BUFFER* Alloc(INT32 size) override {
        if (fail || size<=0) return nullptr;
        auto value=std::make_unique<char[]>(size); auto* pointer=value.get(); packets.emplace(pointer,std::move(value)); return pointer;
    }
    void Free(char* packet) override { check(packets.erase(packet)==1,"packet released exactly once"); ++frees; }
    BOOL IsAllocated(char* packet) override { return packets.contains(packet); }
    UINT32 PrintMemoryStat() override { return 0; }
    void CheckError() override {}
};
class Net:public INet {
public:
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override { return TRUE; }
    BOOL UnInit() override { return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32,UINT16) override { return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};
class Session:public ISocketSession {
public:
    Allocator& allocator;
    int closes=0;
    bool fail=false,throws=false,closeThrows=false;
    std::vector<char> certificate;
    std::function<void()> onClose;
    explicit Session(Allocator& value):allocator(value) {}
    INT32 Send(char* packet,INT32 size,INT32 offset) override {
        if (throws) throw std::runtime_error("fixture send exception");
        if (fail) return -1;
        certificate.assign(packet+offset,packet+offset+size); allocator.Free(packet); return size;
    }
    INT32 SendNoPackageHead(char*,INT32) override { return -1; }
    void* GetUserData() override { return nullptr; }
    void SetUserData(void*) override {}
    BOOL GetAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32&,UINT16&) override { return FALSE; }
    BOOL Close() override { ++closes; if (onClose) onClose(); if (closeThrows) throw 1; return TRUE; }
};
}

int main()
{
    try {
        Allocator allocator; Net net; CCertMessageHandler handler;
        check(&CCertMessageHandler::GetInstance()==&g_MessageHandler,"one certificate handler singleton");
        check(!handler.Init(nullptr,&allocator) && !handler.Init(&net,nullptr),"missing initialization boundaries rejected");
        Session uninitialized{allocator}; handler.OnAccepted(nullptr,&uninitialized);
        check(uninitialized.closes==1 && uninitialized.certificate.empty(),"uninitialized handler closes without sending");
        g_MathTable.nSinTable[0]=123;
        g_MathTable.nCosTable[5]=-456;
        g_MathTable.wSqrtTable[25]=5;
        g_MathTable.nDirectionTable[8][9]=17;
        std::vector<char> original(sizeof(g_MathTable)); std::memcpy(original.data(),&g_MathTable,original.size());
        std::vector<char> legacy(16*1024*1024);
        uLongf legacySize=static_cast<uLongf>(legacy.size());
        check(compress(reinterpret_cast<Bytef*>(legacy.data()),&legacySize,reinterpret_cast<const Bytef*>(&g_MathTable),sizeof(g_MathTable))==Z_OK,"legacy compression reference succeeds");
        legacy.resize(legacySize);
        check(handler.Init(&net,&allocator) && !handler.Init(&net,&allocator),"initialize once without replacing borrowed allocator");
        Session success{allocator}; handler.OnAccepted(nullptr,&success);
        check(success.closes==1 && allocator.packets.empty() && success.certificate.size()<=MAX_MESSAGE_SIZE,"accepted peer receives one bounded owned packet then closes");
        using InternalMessage::CER_SYNC_CERTIFICATION;
        CER_SYNC_CERTIFICATION header{}; std::memcpy(&header,success.certificate.data(),sizeof(header));
        check(header.wMsg==InternalMessage::cer_sync_certification && header.byKey==206,"certificate wire ID and key preserved");
        check(std::equal(legacy.begin(),legacy.end(),success.certificate.begin()+sizeof(header),success.certificate.end()),"compressed bytes match legacy zlib output exactly");
        std::vector<char> restored(original.size()); uLongf restoredSize=static_cast<uLongf>(restored.size());
        check(uncompress(reinterpret_cast<Bytef*>(restored.data()),&restoredSize,reinterpret_cast<const Bytef*>(success.certificate.data()+sizeof(header)),static_cast<uLong>(success.certificate.size()-sizeof(header)))==Z_OK && restored==original,"wire payload restores complete math table");
        for (int mode=0;mode<3;++mode) {
            Session failure{allocator}; allocator.fail=mode==0; failure.fail=mode==1; failure.throws=mode==2;
            handler.OnAccepted(nullptr,&failure);
            check(failure.closes==1 && allocator.packets.empty(),"failed allocation or send closes and releases ownership");
            allocator.fail=false;
        }
        Session receiving{allocator}; auto* packet=allocator.Alloc(3);
        const auto before=allocator.frees; handler.OnRecv(&receiving,packet,3);
        check(receiving.closes==1 && allocator.frees==before+1 && allocator.packets.empty(),"unexpected receive frees packet before closing");
        packet=allocator.Alloc(1); handler.OnRecv(nullptr,packet,1);
        check(allocator.packets.empty(),"missing receive socket cannot leak its packet");
        handler.OnAccepted(nullptr,nullptr); handler.OnRecv(nullptr,nullptr,0);
        handler.BeginShutdown(); Session stopping{allocator}; handler.OnAccepted(nullptr,&stopping);
        check(stopping.closes==1 && stopping.certificate.empty(),"shutdown rejects new replies");
        packet=allocator.Alloc(2); handler.OnRecv(&stopping,packet,2);
        check(allocator.packets.empty(),"shutdown keeps receive allocator alive through transport callbacks");
        check(handler.UnInit() && handler.UnInit() && handler.Init(&net,&allocator),"repeatable cleanup and initialization");
        Session reentrant{allocator}; reentrant.onClose=[&] { handler.UnInit(); };
        handler.OnAccepted(nullptr,&reentrant);
        check(reentrant.closes==1 && allocator.packets.empty(),"close callback can clear handler after send completion");
        check(handler.Init(&net,&allocator),"reinitialize after callback cleanup");
        Session closeFailure{allocator}; closeFailure.closeThrows=true; handler.OnConnected(&closeFailure);
        check(closeFailure.closes==1,"close exception stays inside callback boundary");
        handler.UnInit();
        auto* bytes=reinterpret_cast<unsigned char*>(&g_MathTable);
        unsigned state=123456789;
        for (std::size_t i=0;i<sizeof(g_MathTable);++i) { state^=state<<13; state^=state>>17; state^=state<<5; bytes[i]=static_cast<unsigned char>(state); }
        check(!handler.Init(&net,&allocator),"table exceeding one transport packet is rejected transactionally");
        Session rejected{allocator}; handler.OnAccepted(nullptr,&rejected);
        check(rejected.closes==1 && rejected.certificate.empty(),"failed compression does not publish a partial certificate");
        std::memcpy(&g_MathTable,original.data(),original.size());
        check(handler.Init(&net,&allocator) && handler.UnInit(),"valid compression can retry after failure");
    } catch (const std::exception& error) { check(false,error.what()); }
    std::printf("cert packets: %d checks, %d failures\n",checks,failures); return failures!=0;
}
