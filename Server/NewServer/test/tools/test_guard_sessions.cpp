#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <vector>
#include <winsock2.h>
#include "std_type.h"
#include "process_error.h"
#include "gd_session_mgr.h"
#include "gd_message_handler.h"
#include "guard_config.h"

namespace allocation
{
std::atomic<bool> fail = false;
}
void* operator new(std::size_t size)
{
    if (allocation::fail.exchange(false)) throw std::bad_alloc();
    if (auto* value = std::malloc(size ? size : 1)) return value;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* value) noexcept { std::free(value); }
void operator delete[](void* value) noexcept { std::free(value); }
void operator delete(void* value, std::size_t) noexcept { std::free(value); }
void operator delete[](void* value, std::size_t) noexcept { std::free(value); }

namespace
{
int checks = 0;
int failures = 0;
int pings = 0;
bool ping_success = true;
void check(bool value, const char* description)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}
class Net : public INet
{
public:
    int stops = 0;
    BOOL Init(IPacketAllocator*, INetEventHandler*, void*) override { return TRUE; }
    BOOL UnInit() override { ++stops; return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32, UINT16) override { return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};
class Session : public ISocketSession
{
public:
    int closes = 0;
    bool fail = false;
    bool throw_error = false;
    bool throw_other = false;
    void* data = nullptr;
    std::function<void()> on_close;
    INT32 Send(PACKET_BUFFER*, INT32, INT32) override { return TRUE; }
    INT32 SendNoPackageHead(PACKET_BUFFER*, INT32) override { return TRUE; }
    void* GetUserData() override { return data; }
    void SetUserData(void* value) override { data = value; }
    BOOL GetAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL Close() override
    {
        ++closes;
        if (on_close) on_close();
        if (throw_error) throw std::runtime_error("fixture close failure");
        if (throw_other) throw 42;
        return !fail;
    }
};
void configure()
{
    g_GuardConfig = {};
    g_GuardConfig.nRegisterGuardTimeout = 10;
    g_GuardConfig.nServerPingInterval = 100;
    g_GuardConfig.nServerPingTimeout = 200;
    g_GuardConfig.nPingGuardTimeout = 300;
    pings = 0;
    ping_success = true;
}
void initialization()
{
    configure();
    Net net;
    Session session;
    CGDSessionMgr manager;
    check(!manager.MainLoop() && !manager.AddServerSession(&session), "uninitialized session managers reject work");
    check(!manager.Init(nullptr) && !manager.CheckHasSocketSession(), "missing transport does not partially initialize state");
    check(manager.Init(&net), "initialize against a borrowed transport");
    check(!manager.Init(&net), "repeated initialization preserves active record addresses and state");
    check(!manager.GetGTSession(0) && !manager.GetGTSession(MAX_GT_SESSION_COUNT)
        && !manager.GetGTSession(MAX_GT_SESSION_COUNT + 1) && !manager.GetGTSession(MAX_GS_SESSION_COUNT - 1),
        "GT lookup uses its actual 32-slot bound rather than the larger GS bound");
    check(!manager.GetGSSession(0) && !manager.GetGSSession(MAX_GS_SESSION_COUNT)
        && !manager.GetMGSession(0) && !manager.GetMGSession(MAX_MG_SESSION_COUNT), "reserved/invalid GS and MG IDs are rejected");
    for (UINT32 id = 1; id < MAX_GT_SESSION_COUNT; ++id)
        check(manager.GetGTSession(id)->dwType == stGT && manager.GetGTSession(id)->dwSessionID == id
            && !manager.GetGTSession(id)->pSocketSession, "GT records have stable initialized type/ID fields");
    for (UINT32 id = 1; id < MAX_GS_SESSION_COUNT; ++id)
        check(manager.GetGSSession(id)->dwType == stGS && manager.GetGSSession(id)->dwSessionID == id, "GS slots preserve their native ID range");
    for (UINT32 id = 1; id < MAX_MG_SESSION_COUNT; ++id)
        check(manager.GetMGSession(id)->dwType == stMG && manager.GetMGSession(id)->dwSessionID == id, "MG slots preserve their independent native range");
    check(!manager.GetRemoteGDSessionByIndex(0) && !manager.GetRemoteGDSessionByIndex(MAX_GUARD_COUNT_PER_AREA)
        && !manager.GetRemoteGDSession(0), "empty remote slots cannot match the zero-IP sentinel");
    auto* remote = manager.GetRemoteGDSessionByIndex(1);
    remote->dwIp = 0x0100007f;
    remote->lstProcess.resize(5000);
    remote->lstProcess.back().dwProcessID = 999;
    check(manager.GetRemoteGDSession(0x0100007f) == remote && remote->lstProcess.back().dwProcessID == 999,
        "remote process records use owned standard storage without an obsolete fixed capacity");
    check(manager.UnInit() && manager.UnInit() && !manager.CheckHasSocketSession() && net.stops == 0,
        "shutdown is repeatable and never destroys the borrowed transport");
    check(manager.Init(&net) && !manager.GetRemoteGDSession(0x0100007f) && remote->lstProcess.empty(), "reuse clears remote record data");
    manager.UnInit();
    for (auto member : {&GUARD_CONFIG::nRegisterGuardTimeout, &GUARD_CONFIG::nServerPingInterval,
                       &GUARD_CONFIG::nServerPingTimeout, &GUARD_CONFIG::nPingGuardTimeout})
    {
        g_GuardConfig.*member = -1;
        check(!manager.Init(&net), "negative timing configuration cannot become an unsigned infinite deadline");
        configure();
    }
    CGDSessionMgr no_clock(nullptr);
    check(!no_clock.Init(&net), "missing injected clocks are rejected before session acceptance");
}
void connectivity()
{
    configure(); Net net; Session socket; CGDSessionMgr manager;
    check(manager.Init(&net), "initialize connectivity fixture");
    manager.GetGSSession(2)->pSocketSession = &socket;
    check(!manager.CheckSocketConnected(stMG, 2) && manager.CheckSocketConnected(stGS, 2), "MG connectivity does not accidentally query a GS slot");
    manager.GetGSSession(2)->pSocketSession = nullptr;
    manager.GetMGSession(2)->pSocketSession = &socket;
    check(manager.CheckSocketConnected(stMG, 2) && !manager.CheckSocketConnected(stGS, 2), "MG connectivity uses its own socket storage");
    manager.GetMGSession(2)->pSocketSession = nullptr;
    manager.GetGTSession(1)->pSocketSession = &socket;
    check(manager.CheckSocketConnected(stGT, 1) && !manager.CheckSocketConnected(stGT, MAX_GT_SESSION_COUNT), "GT connectivity bounds are checked before indexing");
    manager.GetGTSession(1)->pSocketSession = nullptr;
    for (auto* record : std::array<ServerSession*, 8>{&manager.GetMSSession(), &manager.GetDBSession(), &manager.GetLSSession(), &manager.GetBSSession(),
        &manager.GetDLSession(), &manager.GetMTSession(), &manager.GetGMSession(), &manager.GetMMSession()})
    {
        record->pSocketSession = &socket;
        check(manager.CheckSocketConnected(static_cast<UINT8>(record->dwType), 1) && manager.CheckHasSocketSession()
            && !manager.CheckSocketConnected(static_cast<UINT8>(record->dwType), 0)
            && !manager.CheckSocketConnected(static_cast<UINT8>(record->dwType), 2), "singleton connectivity includes DL/MT and rejects invalid numbers");
        record->pSocketSession = nullptr;
    }
    check(!manager.CheckSocketConnected(stInvalid, 1) && !manager.CheckHasSocketSession(), "unknown types and empty slots remain disconnected");
    manager.UnInit();
}
void registration_deadlines()
{
    configure(); Net net; Session first, second, third;
    UINT64 tick = 1000;
    CGDSessionMgr manager([&] { return tick; });
    check(manager.Init(&net) && !manager.AddServerSession(nullptr), "null pending sessions are rejected");
    check(manager.AddServerSession(&first) && !manager.AddServerSession(&first), "pending connections are unique by socket identity");
    check(manager.CheckHasSocketSession(), "unregistered sessions count toward shutdown draining");
    tick = 1010;
    check(manager.MainLoop() && first.closes == 0, "registration deadline equality keeps the connection alive");
    tick = 1011;
    check(manager.MainLoop() && first.closes == 1, "registration deadline closes only after its original strict boundary");
    check(manager.MainLoop() && first.closes == 1, "pending close is not requested repeatedly while disconnect is asynchronous");
    check(manager.RemoveServerSession(&first) && !manager.RemoveServerSession(&first), "disconnect removal releases the pending record exactly once");
    check(!manager.RemoveServerSession(nullptr) && !manager.CheckHasSocketSession(), "null/missing pending records do not corrupt emptiness");
    allocation::fail = true;
    check(!manager.AddServerSession(&second) && !allocation::fail && !manager.CheckHasSocketSession(), "allocation failure rolls back session acceptance without retaining the socket");
    check(manager.AddServerSession(&second), "acceptance recovers after allocation failure");
    second.on_close = [&] { manager.RemoveServerSession(&second); manager.RemoveServerSession(&third); };
    check(manager.AddServerSession(&third), "prepare adjacent pending records for synchronous removal");
    tick += 11;
    check(manager.MainLoop() && second.closes == 1 && third.closes == 0 && !manager.CheckHasSocketSession(),
        "timeout traversal remains valid when a close callback removes current and future nodes");
    second.on_close = {};
    tick = std::numeric_limits<UINT64>::max() - 5;
    check(manager.AddServerSession(&third), "accept a near-wrap timestamp");
    tick = std::numeric_limits<UINT64>::max();
    check(manager.MainLoop() && third.closes == 0, "deadline addition saturates instead of wrapping into an immediate timeout");
    manager.UnInit();
}
void keepalive_and_failures()
{
    configure(); Net net; Session socket; UINT64 tick = 100;
    CGDSessionMgr manager([&] { return tick; });
    check(manager.Init(&net) && manager.MainLoop() && pings == 0, "ping interval equality retains the old strict comparison");
    tick = 101;
    check(manager.MainLoop() && pings == 1, "ping sends once after the interval");
    tick = 201;
    check(manager.MainLoop() && pings == 1, "last successful scheduling time anchors the next strict interval");
    tick = 202; ping_success = false;
    check(!manager.MainLoop() && pings == 2, "ping failure reaches the loop caller");
    ping_success = true;
    auto& record = manager.GetDLSession();
    record.pSocketSession = &socket; record.qwTimer = tick; socket.data = &record;
    check(manager.RefreshServerSession(&socket) && record.qwTimer == tick + 300,
        "registered keepalive uses the configured timeout and owning session identity");
    Session old;
    old.data = &record;
    const auto refreshed = record.qwTimer;
    check(!manager.RefreshServerSession(&old) && record.qwTimer == refreshed,
        "a stale connection cannot extend the deadline of its replacement slot");
    socket.data = &socket;
    check(!manager.RefreshServerSession(&socket) && record.qwTimer == refreshed,
        "foreign transport user data is rejected without dereferencing it as a session record");
    socket.data = &record;
    g_GuardConfig.nPingGuardTimeout = -1;
    check(!manager.RefreshServerSession(&socket) && record.qwTimer == refreshed, "invalid reloaded keepalive durations cannot wrap a deadline");
    g_GuardConfig.nPingGuardTimeout = 300;
    record.qwTimer = tick;
    check(manager.MainLoop() && socket.closes == 0, "registered deadline equality remains alive");
    ++tick; socket.fail = true;
    check(!manager.MainLoop() && socket.closes == 1 && !record.bClosing && record.qwTimer == tick - 1,
        "failed close preserves a retryable deadline and returns failure");
    socket.fail = false;
    socket.on_close = [&] { record.pSocketSession = nullptr; socket.data = nullptr; };
    check(manager.MainLoop() && socket.closes == 2 && !manager.CheckHasSocketSession(), "retry can complete a synchronous disconnect safely");
    check(!manager.RefreshServerSession(nullptr) && !manager.RefreshServerSession(&socket), "disconnected and null sessions cannot refresh timers");
    socket.on_close = {};
    manager.UnInit();
    bool throw_clock = true;
    CGDSessionMgr clock_errors([&]() -> UINT64 { if (throw_clock) throw std::runtime_error("clock fixture"); return 1000; });
    check(clock_errors.Init(&net) && !clock_errors.AddServerSession(&socket) && !clock_errors.MainLoop(), "clock exceptions do not leave partially accepted sessions");
    throw_clock = false;
    check(clock_errors.AddServerSession(&socket), "clock recovery permits a fresh acceptance");
    clock_errors.UnInit();
}
void shutdown()
{
    configure(); Net net;
    std::array<Session, 8 + MAX_GT_SESSION_COUNT - 1 + MAX_GS_SESSION_COUNT - 1 + MAX_MG_SESSION_COUNT - 1> sockets;
    std::array<Session, 3> pending;
    Session remote;
    CGDSessionMgr manager;
    check(manager.Init(&net), "initialize complete shutdown fixture");
    std::vector<ServerSession*> slots{&manager.GetDBSession(), &manager.GetLSSession(), &manager.GetMSSession(), &manager.GetBSSession(),
        &manager.GetDLSession(), &manager.GetMTSession(), &manager.GetGMSession(), &manager.GetMMSession()};
    for (UINT32 id = 1; id < MAX_GT_SESSION_COUNT; ++id) slots.push_back(manager.GetGTSession(id));
    for (UINT32 id = 1; id < MAX_GS_SESSION_COUNT; ++id) slots.push_back(manager.GetGSSession(id));
    for (UINT32 id = 1; id < MAX_MG_SESSION_COUNT; ++id) slots.push_back(manager.GetMGSession(id));
    for (std::size_t index = 0; index < slots.size(); ++index)
    { slots[index]->pSocketSession = &sockets[index]; sockets[index].data = slots[index]; }
    for (auto& socket : pending) check(manager.AddServerSession(&socket), "add pending shutdown connections");
    for (auto& socket : pending) socket.on_close = [&manager, ptr = &socket] { manager.RemoveServerSession(ptr); };
    auto& remote_record = *manager.GetRemoteGDSessionByIndex(1);
    remote_record.pSocketSession = &remote; remote_record.dwIp = 123; remote.data = &remote_record;
    check(manager.CloseAllSocketSession() && !manager.AddServerSession(&pending[0]), "bulk shutdown stops new acceptance while disconnects drain");
    check(std::ranges::all_of(sockets, [](auto& socket) { return socket.closes == 1; })
        && std::ranges::all_of(pending, [](auto& socket) { return socket.closes == 1; })
        && remote.closes == 1 && remote.data == nullptr, "every local type, array slot, pending and remote socket receives one close request");
    check(manager.CloseAllSocketSession() && std::ranges::all_of(sockets, [](auto& socket) { return socket.closes == 1; }),
        "repeated bulk close does not duplicate asynchronous close requests");
    check(manager.CheckHasSocketSession(), "outstanding registered/remote disconnects remain visible until teardown");
    check(manager.UnInit() && !manager.CheckHasSocketSession()
        && std::ranges::all_of(sockets, [](auto& socket) { return socket.data == nullptr; }),
        "teardown detaches all borrowed registered user-data pointers before resetting records");
    check(manager.UnInit() && std::ranges::all_of(sockets, [](auto& socket) { return socket.closes == 1; }) && net.stops == 0,
        "destructor/repeated shutdown cannot revisit destroyed transport sessions");
    for (auto& socket : pending) socket.on_close = {};
    Session first, second, third;
    // These sessions outlive the explicit UnInit below, so failure cleanup is safe.
    check(manager.Init(&net) && manager.AddServerSession(&first) && manager.AddServerSession(&second) && manager.AddServerSession(&third), "initialize failure shutdown fixture");
    first.fail = true; second.throw_error = true; third.throw_other = true;
    check(!manager.CloseAllSocketSession() && first.closes == 1 && second.closes == 1 && third.closes == 1,
        "failed and throwing close operations do not prevent attempts on the remaining sessions");
    first.fail = false; second.throw_error = false; third.throw_other = false;
    check(manager.CloseAllSocketSession() && first.closes == 2 && second.closes == 2 && third.closes == 2,
        "a later bulk close retries prior failures once per connection");
    manager.UnInit();
}
}

// A spy for the session manager's one external operation. No guard process,
// command handler, deployment code or real socket backend is linked or executed.
CGDMessageHandler::CGDMessageHandler() : m_pNet(nullptr), m_pPacketAllocator(nullptr), m_GuardMessageFuncList{} {}
CGDMessageHandler::~CGDMessageHandler() = default;
CGDMessageHandler& CGDMessageHandler::GetInstance() { static CGDMessageHandler handler; return handler; }
BOOL CGDMessageHandler::DoAllPing() { ++pings; return ping_success; }
void CGDMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession*) {}
void CGDMessageHandler::OnConnected(ISocketSession*) {}
void CGDMessageHandler::OnRecv(ISocketSession*, PACKET_BUFFER*, UINT32) {}
void CGDMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}

int main()
{
    try
    {
        initialization(); connectivity(); registration_deadlines(); keepalive_and_failures(); shutdown();
        if (failures) return EXIT_FAILURE;
        std::printf("%d guard session checks passed\n", checks);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error) { std::fprintf(stderr, "FAIL: %s\n", error.what()); return EXIT_FAILURE; }
}
