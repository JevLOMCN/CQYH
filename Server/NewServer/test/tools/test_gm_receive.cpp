#include "message_handler.h"
#include "gm_packet.h"
#include "gm_receive_fixture.h"
#include "log.h"
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

namespace
{
std::size_t checks{}, delivered{};
std::function<void()> on_receive;
void check(bool value, const char* message)
{
    ++checks;
    if (!value) throw std::runtime_error(message);
}
void receive()
{
    ++delivered;
    const auto callback = on_receive;
    if (callback) callback();
}
struct Allocator final : IPacketAllocator
{
    std::map<char*, std::unique_ptr<char[]>> packets;
    std::size_t allocated{}, freed{};
    PACKET_BUFFER* Alloc(INT32 size) override
    {
        auto bytes = std::make_unique<char[]>(size);
        auto* pointer = bytes.get();
        packets.emplace(pointer, std::move(bytes));
        ++allocated;
        return pointer;
    }
    void Free(PACKET_BUFFER* pointer) override
    {
        if (packets.erase(pointer) != 1) std::abort();
        ++freed;
    }
    BOOL IsAllocated(PACKET_BUFFER* pointer) override { return packets.contains(pointer); }
    UINT32 PrintMemoryStat() override { return static_cast<UINT32>(packets.size()); }
    void CheckError() override {}
    char* message(UINT16 id)
    {
        const auto source = gm_test::receive_packet(id);
        auto* bytes = Alloc(static_cast<INT32>(source.size()));
        std::memcpy(bytes, source.data(), source.size());
        return bytes;
    }
};
struct Socket final : ISocketSession
{
    std::size_t closes{};
    std::function<void()> on_close;
    INT32 Send(PACKET_BUFFER*, INT32, INT32) override { return 0; }
    INT32 SendNoPackageHead(PACKET_BUFFER*, INT32) override { return 0; }
    void* GetUserData() override { return nullptr; }
    void SetUserData(void*) override {}
    BOOL GetAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL Close() override { ++closes; const auto callback=on_close; if (callback) callback(); return TRUE; }
};
struct Net final : INet
{
    BOOL Init(IPacketAllocator*, INetEventHandler*, void*) override { return TRUE; }
    BOOL UnInit() override { return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32, UINT16) override { return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};
}

// Only the dispatch boundary is replaced: the fixture compiles the actual
// production Init/UnInit/OnRecv implementation without Qt or a live transport.
void CMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession*) {}
void CMessageHandler::OnConnected(ISocketSession*) {}
void CMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}

int main()
{
    CLog::GetInstance().SetLevelMask(0);
    try
    {
        Allocator first, second;
        Net net;
        Socket socket;
        CMessageHandler handler;
        check(!handler.Init(nullptr, &first), "null net rejected");
        check(!handler.Init(&net, nullptr), "null allocator rejected");
        handler.OnRecv(&socket, nullptr, 0);
        check(socket.closes == 1, "uninitialized receive closes socket");
        check(handler.Init(&net, &first), "initialize");
        check(!handler.Init(&net, &second), "duplicate initialization preserves allocator");
        for (UINT32 size : {0u, 1u, UINT32(MAX_MESSAGE_SIZE + 1)})
        {
            const auto before = socket.closes;
            handler.OnRecv(&socket, first.Alloc(1), size);
            check(socket.closes == before + 1 && first.packets.empty(), "invalid size rejected without reading bytes");
        }
        handler.OnRecv(nullptr, first.Alloc(1), 1);
        check(first.packets.empty(), "null socket still releases packet");
        handler.OnRecv(&socket, nullptr, 2);
        check(first.packets.empty(), "null packet rejected");

        std::vector<UINT16> registered;
        for (UINT32 id = 0; id <= 0xFFFF; ++id)
        {
            const auto previous = delivered;
            const auto closed = socket.closes;
            handler.OnRecv(&socket, first.message(static_cast<UINT16>(id)), static_cast<UINT32>(gm_test::receive_packet(static_cast<UINT16>(id)).size()));
            if (delivered != previous) registered.push_back(static_cast<UINT16>(id));
            check(delivered == previous + 1 || socket.closes == closed + 1, "all message IDs either dispatch or close");
            check(first.packets.empty(), "each dispatch releases exactly once");
        }
        check(registered.size() == 33, "all registered production receivers reachable");
        for (const auto registered_id : registered)
        {
            const auto source = gm_test::receive_packet(registered_id);
            if (source.size() <= sizeof(UINT16)) continue;
            const auto short_size = source.size() - 1;
            auto* short_packet = first.Alloc(static_cast<INT32>(short_size));
            std::memcpy(short_packet, source.data(), short_size);
            const auto previous = delivered;
            const auto closed = socket.closes;
            handler.OnRecv(&socket, short_packet, static_cast<UINT32>(short_size));
            check(delivered == previous && socket.closes == closed + 1 && first.packets.empty(),
                "short body rejected and released before receiver callback");
        }
        const auto id = registered.front();
        on_receive = [] { throw std::runtime_error("receiver failure"); };
        auto closed = socket.closes;
        handler.OnRecv(&socket, first.message(id), 2);
        check(socket.closes == closed + 1 && first.packets.empty(), "standard exception closes and releases");
        on_receive = [] { throw 42; };
        closed = socket.closes;
        handler.OnRecv(&socket, first.message(id), 2);
        check(socket.closes == closed + 1 && first.packets.empty(), "unknown exception closes and releases");
        on_receive = [&]
        {
            check(first.packets.size() == 1, "packet alive during receiver");
            handler.UnInit();
            handler.Init(&net, &second);
        };
        handler.OnRecv(&socket, first.message(id), 2);
        check(first.packets.empty() && second.freed == 0, "callback runtime replacement uses original allocator");
        on_receive = {};
        handler.OnRecv(&socket, second.message(id), 2);
        check(second.freed == 1, "next packet uses replacement allocator");

        socket.on_close = [&]
        {
            socket.on_close = {};
            handler.UnInit();
            handler.OnRecv(nullptr, second.Alloc(1), 0);
        };
        handler.OnRecv(&socket, second.Alloc(1), 0);
        check(second.packets.empty(), "close callback shutdown and nested receive release both packets");
        const auto before = delivered;
        handler.OnRecv(&socket, second.message(id), 2);
        check(delivered == before && second.packets.empty(), "late packet rejected and freed after shutdown");
        check(handler.Init(&net, &first), "restart after shutdown");
        handler.OnRecv(&socket, first.message(id), 2);
        check(delivered == before + 1 && first.packets.empty(), "restart restores dispatch");
        handler.UnInit();
        check(first.allocated == first.freed && second.allocated == second.freed, "balanced captured allocator ownership");
        std::cout << checks << " GM receive checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

void CMessageHandler::OnAllPing(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMVerifyAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMLoginAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMItemListNtf(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMLogoutAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMPreQueryAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQueryRoleID(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQueryRoleDataAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMForbidChatAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMFreezeRoleAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMKickRoleAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMSendMailAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMAnnouceAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMGetAnnounceAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMLogAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMExecuteGMStrAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMLoadRoleDataAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMModifyRoleDataAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQueryNameToIDAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMErrorStr(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMRoleExeGMCmd(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMSelectGroupAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQueryExchangeLogAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQueryExchangeHistoryAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMExchangePrice(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQueryOrderDetailAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMAddSystemOrderAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMDelSystemOrderAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMQuerySysExchangeInfoAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMGetExchangeLimitAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
void CMessageHandler::OnGMSetExchangeLimitAck(PACKET_BUFFER*, UINT32, ISocketSession*) { receive(); }
