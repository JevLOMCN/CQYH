#include <chrono>
#include <cstdio>
#include <memory>
#if defined(_WIN32)
#include <winsock2.h>
#endif
#include "std_type.h"
#include "process_error.h"
#include "simple_net.h"

namespace
{
int checks = 0, failures = 0;
void check(bool condition, const char* description)
{
    ++checks;
    if (!condition)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
struct Allocator final : IPacketAllocator
{
    int outstanding = 0;
    PACKET_BUFFER* Alloc(INT32 size) override { ++outstanding; return new char[size]; }
    void Free(PACKET_BUFFER* buffer) override { --outstanding; delete[] buffer; }
    BOOL IsAllocated(PACKET_BUFFER*) override { return TRUE; }
    UINT32 PrintMemoryStat() override { return 0; }
    void CheckError() override {}
};
struct Handler final : INetEventHandler
{
    int calls = 0;
    void OnAccepted(ISocketAcceptor*, ISocketSession*) override { ++calls; }
    void OnConnected(ISocketSession*) override { ++calls; }
    void OnRecv(ISocketSession*, PACKET_BUFFER*, UINT32) override { ++calls; }
    void OnDisconnected(ISocketSession*, BOOL) override { ++calls; }
};
SimpleNet::SSessionMsg* event()
{
    auto record = std::make_unique<SimpleNet::SSessionMsg>();
    record->nSocketFD = 0; // An absent session: a delayed event must be discarded safely.
    record->eOperate = SimpleNet::esoConnected;
    return record.release();
}
}

int main()
{
    CLog::GetInstance().SetOutputMask(lotNone);
    Allocator allocator;
    Handler handler;
    SIMPLE_INIT_PARAM parameters{2, 2, 8, 4096, 4096, 8};
    DWORD handlesBefore = 0, handlesAfter = 0;
    GetProcessHandleCount(GetCurrentProcess(), &handlesBefore);
    {
        SimpleNet::CNet net;
        check(!net.PushMsgToMain(event()) && !net.PushMsgToWork(event()),
              "uninitialized transport consumes rejected events");
        for (int generation = 0; generation < 12; ++generation)
        {
            check(net.Init(&allocator, &handler, &parameters), "initialize the real SimpleNet worker");
            check(!net.Init(&allocator, &handler, &parameters) && net.IsThreadRuning(),
                  "duplicate initialization preserves the active worker");
            bool accepted = true;
            for (int index = 0; index < 4096; ++index)
            {
                accepted = net.PushMsgToMain(event()) && accepted;
                accepted = net.PushMsgToWork(event()) && accepted;
            }
            check(accepted, "retain more control events than the previous ring capacity in both directions");
            if (generation % 2 == 0)
                check(net.MainLoop(), "main loop discards delayed events for absent sessions");
            const auto started = std::chrono::steady_clock::now();
            check(net.UnInit() && !net.IsThreadRuning() &&
                  std::chrono::steady_clock::now() - started < std::chrono::seconds(2),
                  "shutdown joins and drains control events without waiting for the main loop");
            check(!net.PushMsgToMain(event()) && !net.PushMsgToWork(event()),
                  "closed transport consumes rejected events");
            check(net.UnInit(), "repeated shutdown is harmless");
        }
    }
    GetProcessHandleCount(GetCurrentProcess(), &handlesAfter);
    check(handlesAfter <= handlesBefore + 2, "restart cycles release worker and socket runtime handles");
    check(handler.calls == 0 && allocator.outstanding == 0,
          "delayed control events do not invoke handlers or retain packet buffers");
    std::printf("%d SimpleNet lifecycle checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
