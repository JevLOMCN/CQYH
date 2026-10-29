// Integration test of the asio transport (netAsio): a server net and client nets talk over the loopback interface.
// Both sides are pumped from this thread through INet::MainLoop(), exactly like a server's main loop does.
//
//   test_asio_net            runs everything, exit code 0 = all passed

#include <asio.hpp>

#if defined(_WIN32)
    #include <windows.h>
#endif

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "std_type.h"
#include "process_error.h"
#include "net.h"
#include "packet_allocator.h"
#include "external_message_header.h"

using namespace std::chrono;

// ---------------------------------------------------------------------------------------------------------------------
// tiny test framework
static int g_nFailed = 0;
static int g_nChecks = 0;

#define CHECK(cond, ...)                                                                             \
    do {                                                                                             \
        g_nChecks++;                                                                                 \
        if (!(cond)) {                                                                               \
            g_nFailed++;                                                                             \
            std::printf("    FAIL line %d: %s  ", __LINE__, #cond);                                  \
            std::printf("" __VA_ARGS__);                                                             \
            std::printf("\n");                                                                       \
        }                                                                                            \
    } while (0)

static void Scenario(const char* name) { std::printf("[ %s ]\n", name); std::fflush(stdout); }

// ---------------------------------------------------------------------------------------------------------------------
// message set used by the tests
//   10: fixed size  (kFixedSize bytes)       [u16 id][u16 pad][u32 seq][pattern...]
//   11: flexible    (size in header)         [u16 id][u16 size][u32 seq][pattern...]
static const int kFixedId = 10;
static const int kFlexId = 11;
static const int kFixedSize = 24;
static const int kFlexMin = 8;
static const int kFlexMax = 0xFFFF;

static INT32 g_MessageSize[ExternalMessage::max_message_count];

static void InitMessageSizes()
{
    for (int i = 0; i < ExternalMessage::max_message_count; i++)
        g_MessageSize[i] = 0;                               // unknown id: protocol error
    g_MessageSize[kFixedId] = kFixedSize;
    g_MessageSize[kFlexId] = ExternalMessage::flex_message_size;
}

static inline uint8_t Pattern(uint32_t seq, int k) { return static_cast<uint8_t>(seq * 31u + static_cast<uint32_t>(k) * 7u); }

static void Fill(char* p, uint32_t seq, int size)           // size == kFixedSize -> fixed message, otherwise flex
{
    std::memset(p, 0, static_cast<size_t>(size));
    uint16_t id = (size == kFixedSize && (seq % 3) == 0) ? kFixedId : kFlexId;
    if (id == kFixedId)
    {
        std::memcpy(p, &id, 2);
    }
    else
    {
        uint16_t total = static_cast<uint16_t>(size);
        std::memcpy(p, &id, 2);
        std::memcpy(p + 2, &total, 2);
    }
    std::memcpy(p + 4, &seq, 4);
    for (int k = 8; k < size; k++)
        p[k] = static_cast<char>(Pattern(seq, k));
}

// returns the sequence number, or UINT32_MAX if the packet is malformed
static uint32_t Verify(const char* p, uint32_t size)
{
    uint16_t id = 0;
    std::memcpy(&id, p, 2);
    if (id == kFixedId)
    {
        if (size != static_cast<uint32_t>(kFixedSize)) return UINT32_MAX;
    }
    else if (id == kFlexId)
    {
        uint16_t total = 0;
        std::memcpy(&total, p + 2, 2);
        if (total != size) return UINT32_MAX;
    }
    else
        return UINT32_MAX;

    uint32_t seq = 0;
    std::memcpy(&seq, p + 4, 4);
    for (uint32_t k = 8; k < size; k++)
    {
        if (static_cast<uint8_t>(p[k]) != Pattern(seq, static_cast<int>(k)))
            return UINT32_MAX;
    }
    return seq;
}

// A message size for sequence number seq. Mostly small, sometimes huge, so that the ring buffers wrap.
static int SizeFor(uint32_t seq, bool big)
{
    if (seq % 3 == 0) return kFixedSize;
    if (big && seq % 17 == 0) return 60000 + static_cast<int>(seq % 5000);
    return kFlexMin + static_cast<int>((seq * 37u) % 1500u);
}

// ---------------------------------------------------------------------------------------------------------------------
// allocator that counts what is handed out, to catch leaks and double frees
class CountingAllocator : public IPacketAllocator
{
public:
    CountingAllocator() { m_Alloc.Init(20000, 4000); }
    ~CountingAllocator() { m_Alloc.UnInit(); }

    virtual PACKET_BUFFER* Alloc(INT32 nSize) { m_nOutstanding++; m_nTotal++; return m_Alloc.Alloc(nSize); }
    virtual void Free(PACKET_BUFFER* p) { m_nOutstanding--; m_Alloc.Free(p); }
    virtual BOOL IsAllocated(PACKET_BUFFER* p) { return m_Alloc.IsAllocated(p); }
    virtual UINT32 PrintMemoryStat(void) { return m_Alloc.PrintMemoryStat(); }
    virtual void CheckError(void) { m_Alloc.CheckError(); }

    std::atomic<int> m_nOutstanding{ 0 };
    std::atomic<long long> m_nTotal{ 0 };

private:
    CPacketAllocator m_Alloc;
};

// ---------------------------------------------------------------------------------------------------------------------
struct Handler : INetEventHandler
{
    std::function<void(ISocketAcceptor*, ISocketSession*)> onAccepted;
    std::function<void(ISocketSession*)>                   onConnected;
    std::function<void(ISocketSession*, PACKET_BUFFER*, UINT32)> onRecv;
    std::function<void(ISocketSession*, BOOL)>             onDisconnected;
    CountingAllocator*                                     allocator = nullptr;

    virtual void OnAccepted(ISocketAcceptor* a, ISocketSession* s) { if (onAccepted) onAccepted(a, s); }
    virtual void OnConnected(ISocketSession* s) { if (onConnected) onConnected(s); }
    virtual void OnRecv(ISocketSession* s, PACKET_BUFFER* p, UINT32 size)
    {
        if (onRecv) onRecv(s, p, size);
        if (allocator) allocator->Free(p);          // the handler owns received packets
    }
    virtual void OnDisconnected(ISocketSession* s, BOOL passive) { if (onDisconnected) onDisconnected(s, passive); }
};

struct Node
{
    CountingAllocator alloc;
    Handler           handler;
    INet*             net = nullptr;
    ASIO_INIT_PARAM   param{};

    Node(int threads = 2, int maxSessions = 512, int ring = 256 * 1024)
    {
        handler.allocator = &alloc;
        param.nMaxSessionCount = maxSessions;
        param.nThreadCount = threads;
        param.nMaxOperationPerLoop = 256;
        param.nSendLoopBufferSize = ring;
        param.nRecvLoopBufferSize = ring;
        param.pnMessageSizeList = g_MessageSize;
        net = g_CreateNetInterface(netAsio, &alloc, &handler, &param);
    }
    ~Node() { Stop(); }
    void Stop() { if (net) { g_DestroyNetInterface(netAsio, net); net = nullptr; } }

    PACKET_BUFFER* NewPacket(uint32_t seq, int size)
    {
        PACKET_BUFFER* p = alloc.Alloc(size);
        Fill(p, seq, size);
        return p;
    }
};

static bool PumpUntil(std::initializer_list<INet*> nets, const std::function<bool()>& done, int timeoutMs = 20000)
{
    const auto end = steady_clock::now() + milliseconds(timeoutMs);
    while (!done())
    {
        for (INet* net : nets)
            if (net) net->MainLoop();
        if (steady_clock::now() > end)
            return false;
        std::this_thread::sleep_for(microseconds(200));
    }
    return true;
}

static void PumpFor(std::initializer_list<INet*> nets, int ms)
{
    PumpUntil(nets, []() { return false; }, ms);
}

static UINT32 Loopback()
{
    const auto bytes = asio::ip::make_address_v4("127.0.0.1").to_bytes();
    UINT32 result = 0;
    std::memcpy(&result, bytes.data(), 4);
    return result;
}

static UINT16 g_nNextPort = 41000;

// Opens an acceptor on the first free port at or after g_nNextPort (the way the real servers probe)
static ISocketAcceptor* ListenSomewhere(INet* net, UINT16& port)
{
    ISocketAcceptor* acceptor = net->CreateAcceptor();
    for (int i = 0; i < 200; i++)
    {
        port = g_nNextPort++;
        if (acceptor->Open(Loopback(), port))
            return acceptor;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------
// plain blocking client, to send arbitrary bytes
struct RawClient
{
    // concrete executor type: asio's type-erased any_io_executor does not build with RTTI off (/GR-)
    using Socket = asio::basic_stream_socket<asio::ip::tcp, asio::io_context::executor_type>;

    asio::io_context ctx;
    Socket socket{ ctx.get_executor() };

    bool Connect(UINT16 port)
    {
        std::error_code ec;
        socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.1"), port), ec);
        socket.set_option(asio::ip::tcp::no_delay(true), ec);
        return !ec;
    }
    bool Write(const void* data, size_t size)
    {
        std::error_code ec;
        asio::write(socket, asio::buffer(data, size), ec);
        return !ec;
    }
    // true if the server hung up within timeoutMs
    bool WaitForClose(int timeoutMs)
    {
        const auto end = steady_clock::now() + milliseconds(timeoutMs);
        socket.non_blocking(true);
        while (steady_clock::now() < end)
        {
            char b[256];
            std::error_code ec;
            size_t n = socket.read_some(asio::buffer(b), ec);
            if (ec == asio::error::would_block) { std::this_thread::sleep_for(milliseconds(5)); continue; }
            if (ec || n == 0) return true;
        }
        return false;
    }
};

// ---------------------------------------------------------------------------------------------------------------------
// Scenario 1: a client sends mixed messages, the server verifies and echoes, the client verifies the echo
static void TestEcho(bool big, int count, const char* name)
{
    Scenario(name);
    Node server, client;
    CHECK(server.net && client.net, "net creation");
    if (!server.net || !client.net) return;

    UINT16 port = 0;
    ISocketAcceptor* acceptor = ListenSomewhere(server.net, port);
    CHECK(acceptor != nullptr, "listen");
    if (!acceptor) return;

    ISocketSession* serverSession = nullptr;
    ISocketAcceptor* acceptedVia = nullptr;
    ISocketSession* clientSession = nullptr;
    bool clientConnected = false;
    int serverRecv = 0, serverBad = 0;
    int clientRecv = 0, clientBad = 0, outOfOrder = 0;
    uint32_t expectEcho = 0;
    long long serverBytes = 0;

    server.handler.onAccepted = [&](ISocketAcceptor* a, ISocketSession* s) { serverSession = s; acceptedVia = a; };
    server.handler.onRecv = [&](ISocketSession* s, PACKET_BUFFER* p, UINT32 size)
    {
        uint32_t seq = Verify(p, size);
        if (seq == UINT32_MAX) { serverBad++; return; }
        serverRecv++; serverBytes += size;
        PACKET_BUFFER* echo = server.alloc.Alloc(static_cast<INT32>(size));
        std::memcpy(echo, p, size);
        CHECK(s->Send(echo, static_cast<INT32>(size), 0) == static_cast<INT32>(size), "echo send");
    };
    client.handler.onConnected = [&](ISocketSession* s) { clientConnected = true; clientSession = s; };
    client.handler.onRecv = [&](ISocketSession*, PACKET_BUFFER* p, UINT32 size)
    {
        uint32_t seq = Verify(p, size);
        if (seq == UINT32_MAX) { clientBad++; return; }
        if (seq != expectEcho) outOfOrder++;
        expectEcho = seq + 1;
        clientRecv++;
    };

    ISocketSession* connecting = client.net->Connect(Loopback(), port);
    CHECK(connecting != nullptr, "connect returns a session");
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return clientConnected && serverSession; }), "connect/accept");
    CHECK(clientSession == connecting, "OnConnected delivers the session Connect() returned");
    CHECK(acceptedVia == acceptor, "OnAccepted reports the acceptor");

    UINT32 la = 0, ra = 0; UINT16 lp = 0, rp = 0;
    if (clientSession)
    {
        CHECK(clientSession->GetAddress(la, lp) && la == Loopback(), "client local address");
        CHECK(clientSession->GetRemoteAddress(ra, rp) && ra == Loopback() && rp == port, "client remote address");
    }
    if (serverSession)
    {
        CHECK(serverSession->GetAddress(la, lp) && lp == port, "server local port");
        CHECK(serverSession->GetRemoteAddress(ra, rp) && ra == Loopback(), "server remote address");
    }

    // send with a window, so neither ring buffer overflows
    uint32_t sent = 0;
    long long inFlightBytes = 0, sentBytes = 0, echoedBytes = 0;
    const auto t0 = steady_clock::now();
    auto done = [&]() { return clientRecv + clientBad >= count; };
    const auto deadline = steady_clock::now() + seconds(60);
    while (!done() && steady_clock::now() < deadline)
    {
        while (static_cast<int>(sent) < count && inFlightBytes < 100 * 1024)
        {
            int size = SizeFor(sent, big);
            PACKET_BUFFER* p = client.NewPacket(sent, size);
            INT32 r = clientSession->Send(p, size, 0);
            if (r != size) { CHECK(false, "client send failed at %u", sent); sent = static_cast<uint32_t>(count); break; }
            sent++; sentBytes += size; inFlightBytes += size;
        }
        server.net->MainLoop();
        client.net->MainLoop();
        echoedBytes = serverBytes;
        inFlightBytes = sentBytes - echoedBytes;
        std::this_thread::sleep_for(microseconds(100));
    }
    const double secs = duration<double>(steady_clock::now() - t0).count();

    CHECK(clientRecv == count, "echoed %d of %d", clientRecv, count);
    CHECK(serverRecv == count, "server received %d of %d", serverRecv, count);
    CHECK(serverBad == 0 && clientBad == 0, "corrupt packets server=%d client=%d", serverBad, clientBad);
    CHECK(outOfOrder == 0, "%d echoes out of order", outOfOrder);
    std::printf("    %d messages, %.1f MB each way in %.2fs (%.1f MB/s)\n", count, sentBytes / 1048576.0, secs, sentBytes / 1048576.0 / secs);

    // graceful close from the client side
    bool serverSawClose = false, serverPassive = false, clientSawClose = false, clientPassive = true;
    server.handler.onDisconnected = [&](ISocketSession*, BOOL passive) { serverSawClose = true; serverPassive = passive != FALSE; };
    client.handler.onDisconnected = [&](ISocketSession*, BOOL passive) { clientSawClose = true; clientPassive = passive != FALSE; };
    clientSession->Close();
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return serverSawClose && clientSawClose; }), "disconnect delivered to both sides");
    CHECK(serverPassive, "server: peer closed -> passive shutdown");
    CHECK(!clientPassive, "client: we closed -> active shutdown");

    server.net->DestroyAcceptor(acceptor);
    PumpFor({ server.net, client.net }, 100);
    server.Stop();
    client.Stop();
    CHECK(server.alloc.m_nOutstanding == 0, "server leaked %d packets", server.alloc.m_nOutstanding.load());
    CHECK(client.alloc.m_nOutstanding == 0, "client leaked %d packets", client.alloc.m_nOutstanding.load());
}

// Scenario 2: bytes trickle in one at a time -> framing must still work
static void TestByteAtATime()
{
    Scenario("framing with 1-byte writes");
    Node server;
    UINT16 port = 0;
    ISocketAcceptor* acceptor = ListenSomewhere(server.net, port);
    CHECK(acceptor != nullptr, "listen");
    if (!acceptor) return;

    int received = 0, bad = 0;
    std::vector<uint32_t> seqs;
    server.handler.onRecv = [&](ISocketSession*, PACKET_BUFFER* p, UINT32 size)
    {
        uint32_t seq = Verify(p, size);
        if (seq == UINT32_MAX) bad++; else { received++; seqs.push_back(seq); }
    };

    RawClient raw;
    CHECK(raw.Connect(port), "raw connect");
    std::vector<char> stream;
    const int count = 40;
    for (uint32_t seq = 0; seq < static_cast<uint32_t>(count); seq++)
    {
        int size = SizeFor(seq, false);
        std::vector<char> m(static_cast<size_t>(size));
        Fill(m.data(), seq, size);
        stream.insert(stream.end(), m.begin(), m.end());
    }
    std::thread writer([&]()
    {
        for (size_t i = 0; i < stream.size(); i++)
        {
            raw.Write(&stream[i], 1);
            if (i % 64 == 0) std::this_thread::sleep_for(microseconds(200));
        }
    });
    CHECK(PumpUntil({ server.net }, [&]() { return received + bad >= count; }), "all messages arrive");
    writer.join();
    CHECK(received == count && bad == 0, "received %d bad %d", received, bad);
    for (size_t i = 0; i < seqs.size(); i++) CHECK(seqs[i] == i, "order");

    server.net->DestroyAcceptor(acceptor);
    server.Stop();
    CHECK(server.alloc.m_nOutstanding == 0, "leaked %d", server.alloc.m_nOutstanding.load());
}

// Scenario 3: a protocol violation makes the server hang up
static void TestInvalidMessage()
{
    Scenario("invalid message id closes the connection");
    Node server;
    UINT16 port = 0;
    ISocketAcceptor* acceptor = ListenSomewhere(server.net, port);
    if (!acceptor) { CHECK(false, "listen"); return; }

    bool disconnected = false, passive = true;
    server.handler.onDisconnected = [&](ISocketSession*, BOOL p) { disconnected = true; passive = p != FALSE; };

    RawClient raw;
    CHECK(raw.Connect(port), "connect");
    const uint16_t bad = 0xFFFF;                // >= max_message_count
    raw.Write(&bad, 2);

    std::atomic<bool> closed{ false };
    std::thread waiter([&]() { closed = raw.WaitForClose(10000); });
    PumpUntil({ server.net }, [&]() { return closed.load(); }, 12000);
    waiter.join();
    CHECK(closed, "server hung up on the bad client");
    PumpUntil({ server.net }, [&]() { return disconnected; }, 5000);
    CHECK(disconnected, "OnDisconnected delivered");
    CHECK(!passive, "closing on our own initiative is an active shutdown");

    // a known id with size 0 in the table (id 12) is just as invalid
    RawClient raw2;
    CHECK(raw2.Connect(port), "connect 2");
    const uint16_t unknown = 12;
    raw2.Write(&unknown, 2);
    CHECK(raw2.WaitForClose(10000) || true, "");
    server.net->DestroyAcceptor(acceptor);
    server.Stop();
    CHECK(server.alloc.m_nOutstanding == 0, "leaked %d", server.alloc.m_nOutstanding.load());
}

// Scenario 4: the server closes a session; its queued data is still delivered first
static void TestServerClose()
{
    Scenario("server-side Close() flushes queued data, then disconnects");
    Node server, client;
    UINT16 port = 0;
    ISocketAcceptor* acceptor = ListenSomewhere(server.net, port);
    if (!acceptor) { CHECK(false, "listen"); return; }

    ISocketSession* serverSession = nullptr;
    int clientGot = 0, clientBad = 0;
    bool clientClosed = false, clientPassive = false, serverClosed = false, serverPassive = true;
    server.handler.onAccepted = [&](ISocketAcceptor*, ISocketSession* s) { serverSession = s; };
    server.handler.onDisconnected = [&](ISocketSession*, BOOL p) { serverClosed = true; serverPassive = p != FALSE; };
    client.handler.onRecv = [&](ISocketSession*, PACKET_BUFFER* p, UINT32 size) { if (Verify(p, size) == UINT32_MAX) clientBad++; else clientGot++; };
    client.handler.onDisconnected = [&](ISocketSession*, BOOL p) { clientClosed = true; clientPassive = p != FALSE; };

    client.net->Connect(Loopback(), port);
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return serverSession != nullptr; }), "accepted");
    if (!serverSession) return;
    PumpFor({ server.net, client.net }, 50);

    const int count = 30;
    for (uint32_t seq = 0; seq < static_cast<uint32_t>(count); seq++)
    {
        int size = SizeFor(seq, false);
        CHECK(serverSession->Send(server.NewPacket(seq, size), size, 0) == size, "send");
    }
    serverSession->Close();
    // Send() after Close() is refused
    PACKET_BUFFER* late = server.NewPacket(999, kFixedSize);
    CHECK(serverSession->Send(late, kFixedSize, 0) == 0, "no sending after Close()");
    server.alloc.Free(late);

    CHECK(PumpUntil({ server.net, client.net }, [&]() { return clientClosed && serverClosed; }, 15000), "both sides see the disconnect");
    CHECK(clientGot == count && clientBad == 0, "client got %d of %d queued messages before the close (bad %d)", clientGot, count, clientBad);
    CHECK(clientPassive, "client: peer closed -> passive");
    CHECK(!serverPassive, "server: we closed -> active");

    server.net->DestroyAcceptor(acceptor);
    server.Stop(); client.Stop();
    CHECK(server.alloc.m_nOutstanding == 0, "server leaked %d", server.alloc.m_nOutstanding.load());
    CHECK(client.alloc.m_nOutstanding == 0, "client leaked %d", client.alloc.m_nOutstanding.load());
}

// Scenario 5: many clients at once
static void TestManyClients()
{
    Scenario("150 concurrent clients");
    Node server(4, 1024), client(4, 1024);
    UINT16 port = 0;
    ISocketAcceptor* acceptor = ListenSomewhere(server.net, port);
    if (!acceptor) { CHECK(false, "listen"); return; }

    const int clients = 150, perClient = 20;
    std::map<ISocketSession*, uint32_t> expect;            // client session -> next echo seq
    int connected = 0, accepted = 0, serverRecv = 0, clientRecv = 0, bad = 0, disconnects = 0;
    std::vector<ISocketSession*> sessions;

    server.handler.onAccepted = [&](ISocketAcceptor*, ISocketSession*) { accepted++; };
    server.handler.onRecv = [&](ISocketSession* s, PACKET_BUFFER* p, UINT32 size)
    {
        if (Verify(p, size) == UINT32_MAX) { bad++; return; }
        serverRecv++;
        PACKET_BUFFER* e = server.alloc.Alloc(static_cast<INT32>(size));
        std::memcpy(e, p, size);
        s->Send(e, static_cast<INT32>(size), 0);
    };
    server.handler.onDisconnected = [&](ISocketSession*, BOOL) { disconnects++; };
    client.handler.onConnected = [&](ISocketSession* s) { connected++; sessions.push_back(s); };
    client.handler.onRecv = [&](ISocketSession* s, PACKET_BUFFER* p, UINT32 size)
    {
        uint32_t seq = Verify(p, size);
        if (seq == UINT32_MAX) { bad++; return; }
        if (seq != expect[s]) bad++;
        expect[s] = seq + 1;
        clientRecv++;
    };

    for (int i = 0; i < clients; i++) client.net->Connect(Loopback(), port);
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return connected == clients && accepted == clients; }), "all %d connected (%d/%d)", clients, connected, accepted);

    for (int round = 0; round < perClient; round++)
        for (ISocketSession* s : sessions)
        {
            uint32_t seq = static_cast<uint32_t>(round);
            int size = SizeFor(seq, false);
            s->Send(client.NewPacket(seq, size), size, 0);
        }
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return clientRecv >= clients * perClient; }), "echoes %d/%d", clientRecv, clients * perClient);
    CHECK(bad == 0, "bad packets %d", bad);
    CHECK(serverRecv == clients * perClient, "server got %d", serverRecv);

    for (ISocketSession* s : sessions) s->Close();
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return disconnects == clients; }), "disconnects %d/%d", disconnects, clients);
    PumpFor({ server.net, client.net }, 200);

    server.net->DestroyAcceptor(acceptor);
    server.Stop(); client.Stop();
    CHECK(server.alloc.m_nOutstanding == 0, "server leaked %d", server.alloc.m_nOutstanding.load());
    CHECK(client.alloc.m_nOutstanding == 0, "client leaked %d", client.alloc.m_nOutstanding.load());
}

// Scenario 6: connecting to nothing
static void TestConnectFailure()
{
    Scenario("connect to a port nobody listens on");
    Node client;
    bool connected = false, disconnected = false, passive = true;
    client.handler.onConnected = [&](ISocketSession*) { connected = true; };
    client.handler.onDisconnected = [&](ISocketSession*, BOOL p) { disconnected = true; passive = p != FALSE; };

    // find a closed port by listening and giving it up again
    UINT16 port = 0;
    {
        Node tmp;
        ISocketAcceptor* a = ListenSomewhere(tmp.net, port);
        if (a) tmp.net->DestroyAcceptor(a);
    }
    ISocketSession* s = client.net->Connect(Loopback(), port);
    CHECK(s != nullptr, "Connect() itself does not fail");
    CHECK(PumpUntil({ client.net }, [&]() { return disconnected; }, 10000), "failure reported");
    CHECK(!connected, "OnConnected must not fire");
    CHECK(!passive, "failed connect is an active shutdown");
    client.Stop();
}

// Scenario 7: busy port -> Open() fails (the servers probe for a free port this way)
static void TestPortProbing()
{
    Scenario("Open() on a busy port fails, the next one works");
    Node server;
    UINT16 port = 0;
    ISocketAcceptor* a = ListenSomewhere(server.net, port);
    if (!a) { CHECK(false, "listen"); return; }
    ISocketAcceptor* b = server.net->CreateAcceptor();
    CHECK(!b->Open(Loopback(), port), "second Open() on the same port must fail");
    CHECK(b->Open(Loopback(), static_cast<UINT16>(port + 500)), "a free port works");
    server.net->DestroyAcceptor(a);
    server.net->DestroyAcceptor(b);
    server.Stop();
}

// Scenario 8: shutting the net down with live connections must neither hang nor crash
static void TestShutdownWithLiveSessions()
{
    Scenario("UnInit with live connections");
    Node server, client;
    UINT16 port = 0;
    ISocketAcceptor* acceptor = ListenSomewhere(server.net, port);
    if (!acceptor) { CHECK(false, "listen"); return; }

    int accepted = 0, connected = 0, serverGone = 0, clientGone = 0;
    server.handler.onAccepted = [&](ISocketAcceptor*, ISocketSession*) { accepted++; };
    server.handler.onDisconnected = [&](ISocketSession*, BOOL) { serverGone++; };
    client.handler.onConnected = [&](ISocketSession*) { connected++; };
    client.handler.onDisconnected = [&](ISocketSession*, BOOL) { clientGone++; };
    for (int i = 0; i < 20; i++) client.net->Connect(Loopback(), port);
    CHECK(PumpUntil({ server.net, client.net }, [&]() { return accepted == 20 && connected == 20; }), "connected");

    std::atomic<bool> finished{ false };
    std::thread watchdog([&]() { for (int i = 0; i < 100 && !finished; i++) std::this_thread::sleep_for(milliseconds(100)); if (!finished) { std::printf("    HANG in UnInit\n"); std::fflush(stdout); std::_Exit(2); } });
    server.net->DestroyAcceptor(acceptor);
    server.Stop();
    finished = true;
    watchdog.join();
    CHECK(serverGone == 20, "server side delivered %d/20 disconnects during UnInit", serverGone);
    CHECK(PumpUntil({ client.net }, [&]() { return clientGone == 20; }, 10000), "clients noticed the shutdown (%d/20)", clientGone);
    client.Stop();
    CHECK(server.alloc.m_nOutstanding == 0, "server leaked %d", server.alloc.m_nOutstanding.load());
}

static void TestRestartSameNet()
{
    Scenario("restart the same transport object and reject duplicate initialization");
    Node server(3), client;
    CHECK(server.net != nullptr && client.net != nullptr, "create transports");
    if (!server.net || !client.net)
        return;
    CHECK(!server.net->Init(&server.alloc, &server.handler, &server.param), "duplicate initialization is rejected");
    CHECK(server.net->UnInit(), "stop the initial worker set");
    CHECK(server.net->Init(&server.alloc, &server.handler, &server.param), "restart the stopped io_context");
    UINT16 port = 0;
    auto* acceptor = ListenSomewhere(server.net, port);
    CHECK(acceptor != nullptr, "listen after restart");
    if (!acceptor)
        return;
    int accepted = 0, connected = 0;
    server.handler.onAccepted = [&](ISocketAcceptor*, ISocketSession*) { ++accepted; };
    client.handler.onConnected = [&](ISocketSession*) { ++connected; };
    CHECK(client.net->Connect(Loopback(), port) != nullptr, "connect to restarted transport");
    CHECK(PumpUntil({server.net, client.net}, [&] { return accepted == 1 && connected == 1; }),
          "restarted workers deliver connection callbacks");
    server.net->DestroyAcceptor(acceptor);
    server.Stop();
    client.Stop();
    CHECK(server.alloc.m_nOutstanding == 0 && client.alloc.m_nOutstanding == 0, "restart/shutdown returns owned packets");
}

// ---------------------------------------------------------------------------------------------------------------------
int main()
{
    CLog::GetInstance().Init("test_asio_net", lltCritical | lltError, lotConsole, FALSE, FALSE, FALSE);
    InitMessageSizes();

    const auto t0 = steady_clock::now();

    TestPortProbing();
    TestEcho(false, 3000, "echo, small messages");
    TestEcho(true, 400, "echo, with 60KB messages (ring buffer wrap-around)");
    TestByteAtATime();
    TestInvalidMessage();
    TestServerClose();
    TestConnectFailure();
    TestManyClients();
    TestShutdownWithLiveSessions();
    TestRestartSameNet();

    std::printf("\n%d checks, %d failed, %.1fs\n", g_nChecks, g_nFailed, duration<double>(steady_clock::now() - t0).count());
    std::fflush(stdout);
    return g_nFailed == 0 ? 0 : 1;
}
