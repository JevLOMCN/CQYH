// asio based transport (netAsio). See asio_net.h for the overview.
//
// Threads
//   worker threads  : run the asio io_context; only socket I/O and moving bytes between the sockets and the
//                     per-session ring buffers happens there. Every session has its own strand, so all operations on
//                     one socket are serialized no matter how many workers there are.
//   "main" thread   : whoever calls CNet::MainLoop(). It parses packets, drives the session state machine and calls
//                     INetEventHandler. Worker threads talk to it through a mutex protected event queue.
//
// This file includes socket headers before <windows.h>, independently of the legacy precompiled stdafx.h.

#include <asio.hpp>

#if defined(_WIN32)
    #include <windows.h>
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <exception>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include "std_type.h"
#if defined(__linux__)
    #include "linux_compatible.h"
#endif
#include "process_error.h"
#include "time_ex.h"

#include "asio_net.h"
#include "external_message_header.h"

namespace AsioNet
{
    using asio::ip::tcp;

    // Concrete (not type erased) executor types: every socket, acceptor and timer lives on its own strand.
    using Strand   = asio::strand<asio::io_context::executor_type>;
    using Socket   = asio::basic_stream_socket<tcp, Strand>;
    using AcceptorSocket = asio::basic_socket_acceptor<tcp, Strand>;
    using Timer    = asio::basic_waitable_timer<std::chrono::steady_clock, asio::wait_traits<std::chrono::steady_clock>, Strand>;

    namespace
    {
        constexpr UINT64 kMaxMainLoopTimeMs   = 500;         // MainLoop() gives up after this long and continues next call
        constexpr UINT64 kShutdownTimeoutMs   = 4 * 1000;    // a closing session waits this long for the peer to hang up
        constexpr UINT64 kSendDelayMs         = 16;          // minimum time between two send kicks from MainLoop()
        constexpr size_t kMaxReadChunk        = 2048;        // bytes requested per read, as the IOCP version did
        constexpr int    kListenBacklog       = 256;
        constexpr int    kSocketBufferSize    = 16 * 1024;   // SO_SNDBUF / SO_RCVBUF of every session socket

        enum class SessionState
        {
            Accepted,       // socket accepted/connected, waiting for the main thread to pick it up
            Connecting,     // outgoing connection in progress
            Normal,         // established, send/recv allowed
            Disconnecting,  // Close() was called (or the connect failed): flush, drain what is left, then go away
            Cleanup,        // OnDisconnected was delivered; waiting for outstanding I/O to finish
        };

        enum class EventType
        {
            Accepted,       // a new incoming connection
            Connected,      // an outgoing connection was established
            ConnectFailed,  // an outgoing connection could not be established
            Closed,         // the receive side ended (peer closed, reset, error)
        };

        void SetSocketOptions(Socket& socket)
        {
            std::error_code ec;
            socket.set_option(tcp::no_delay(true), ec);
            socket.set_option(asio::socket_base::send_buffer_size(kSocketBufferSize), ec);
            socket.set_option(asio::socket_base::receive_buffer_size(kSocketBufferSize), ec);
        }

        UINT32 ToNetworkOrder(const asio::ip::address_v4& address)
        {
            const auto bytes = address.to_bytes();     // network byte order, i.e. the memory layout of in_addr
            UINT32 result = 0;
            std::memcpy(&result, bytes.data(), sizeof(result));
            return result;
        }

        asio::ip::address_v4 FromNetworkOrder(UINT32 address)
        {
            asio::ip::address_v4::bytes_type bytes;
            std::memcpy(bytes.data(), &address, sizeof(address));
            return asio::ip::address_v4(bytes);
        }
    }

    class Session;
    class Acceptor;

    struct Event
    {
        EventType                type;
        std::shared_ptr<Session> session;
        Acceptor*                acceptor;
    };

    class NetImpl
    {
    public:
        NetImpl() = default;
        ~NetImpl() { UnInit(); }

        bool Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEventHandler, void* pInitParam);
        void UnInit(void);

        ISocketAcceptor* CreateAcceptor(void);
        bool DestroyAcceptor(ISocketAcceptor* pSocketAcceptor);
        ISocketSession* Connect(UINT32 dwAddr, UINT16 wPort);
        bool MainLoop(void);
        INT64 PrintMemoryStat(void);

        // worker thread -> main thread
        void PushEvent(EventType type, std::shared_ptr<Session> session, Acceptor* acceptor = nullptr);

    public:
        ASIO_INIT_PARAM         param{};
        IPacketAllocator*       allocator = nullptr;
        INetEventHandler*       handler = nullptr;

        asio::io_context        ctx;
        std::optional<asio::executor_work_guard<asio::io_context::executor_type>> workGuard;
        std::vector<std::jthread> threads;

        std::atomic<int>        sessionCount{ 0 };

    private:
        void HandleEvent(const Event& event);
        bool IsAcceptorAlive(const Acceptor* acceptor) const;

        std::mutex              m_EventMutex;
        std::vector<Event>      m_Events;

        // main thread only
        std::list<std::shared_ptr<Session>>     m_Sessions;
        std::vector<std::unique_ptr<Acceptor>>  m_Acceptors;
        bool                    m_bInitialized = false;
    };

    // ------------------------------------------------------------------------------------------------------------
    class Session final : public ISocketSession, public std::enable_shared_from_this<Session>
    {
    public:
        Session(NetImpl& net, Acceptor* acceptor)
            : m_Net(net)
            , m_pAcceptor(acceptor)
            , m_Strand(asio::make_strand(net.ctx))
            , m_Socket(m_Strand)
            , m_SendBuf(static_cast<size_t>(net.param.nSendLoopBufferSize))
            , m_RecvBuf(static_cast<size_t>(net.param.nRecvLoopBufferSize))
        {
            m_Net.sessionCount.fetch_add(1);
        }

        ~Session() { m_Net.sessionCount.fetch_sub(1); }

        // ---- ISocketSession (main thread) ----
        virtual INT32 Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset);
        virtual INT32 SendNoPackageHead(PACKET_BUFFER*, INT32) { return 0; }     // not supported (neither was it by IOCP)

        virtual void* GetUserData(void) { return m_pUserData; }
        virtual void SetUserData(void* pUserData) { m_pUserData = pUserData; }

        virtual BOOL GetAddress(UINT32& dwAddr, UINT16& wPort);
        virtual BOOL GetRemoteAddress(UINT32& dwRemoteAddr, UINT16& wRemotePort);

        virtual BOOL Close(void);

        // ---- main thread ----
        SessionState GetState(void) const { return m_eState; }
        void SetState(SessionState state) { m_eState = state; }
        Acceptor* GetAcceptor(void) const { return m_pAcceptor; }

        INT32 Recv(void);                       // delivers at most one packet to the event handler
        void TrySend(UINT64 qwCurrentTick);
        void StartRecv(void);
        void Shutdown(void);                    // close the socket
        bool CheckCleanup(void) const { return !m_bSending.load() && !m_bRecving.load(); }
        UINT64 GetShutdownTimer(void) const { return m_qwShutdownTimer; }

        // ---- worker threads ----
        Socket& GetSocket(void) { return m_Socket; }
        void OnSocketEstablished(void);         // options + addresses; called before any I/O is started
        void BeginConnect(const tcp::endpoint& endpoint);

    private:
        void StartSend(void);
        void DoWrite(void);
        void OnWritten(const std::error_code& ec, size_t size);
        void OnSendIdle(void);
        void DoRead(void);
        void OnRead(const std::error_code& ec, size_t size);
        void FailSend(void);

        int SendUsed(void) const
        {
            const int size = static_cast<int>(m_SendBuf.size());
            const int used = m_SendRear.load(std::memory_order_acquire) - m_SendHead.load(std::memory_order_acquire);
            return used < 0 ? used + size : used;
        }

    private:
        NetImpl&        m_Net;
        Acceptor*       m_pAcceptor;

        Strand          m_Strand;
        Socket          m_Socket;

        // main thread state
        SessionState    m_eState = SessionState::Accepted;
        void*           m_pUserData = nullptr;
        UINT64          m_qwShutdownTimer = 0;
        UINT64          m_qwSendTimer = 0;
        bool            m_bShutdownPosted = false;
        bool            m_bProtocolError = false;   // the receive stream is corrupt: stop parsing it

        // addresses, filled in before the session is handed to the main thread
        UINT32          m_dwLocalAddr = 0;
        UINT16          m_wLocalPort = 0;
        UINT32          m_dwRemoteAddr = 0;
        UINT16          m_wRemotePort = 0;

        // ring buffers. send: main thread produces (Rear), the strand consumes (Head).
        //               recv: the strand produces (Rear), the main thread consumes (Head).
        std::vector<char>   m_SendBuf;
        std::vector<char>   m_RecvBuf;
        std::atomic<int>    m_SendHead{ 0 };
        std::atomic<int>    m_SendRear{ 0 };
        std::atomic<int>    m_RecvHead{ 0 };
        std::atomic<int>    m_RecvRear{ 0 };

        std::atomic<bool>   m_bSending{ false };
        std::atomic<bool>   m_bRecving{ false };
        std::atomic<bool>   m_bClosing{ false };    // Close() was requested: shut the send side down once flushed
    };

    // ------------------------------------------------------------------------------------------------------------
    class Acceptor final : public ISocketAcceptor
    {
    public:
        explicit Acceptor(NetImpl& net)
            : m_Net(net), m_Strand(asio::make_strand(net.ctx)), m_Acceptor(m_Strand), m_RetryTimer(m_Strand) {}

        virtual BOOL Open(UINT32 dwAddr, UINT16 wPort);
        virtual BOOL Close(void);
        virtual void* GetUserData(void) { return m_pUserData; }
        virtual void SetUserData(void* pUserData) { m_pUserData = pUserData; }

    private:
        void DoAccept(void);
        void OnAccept(const std::error_code& ec, const std::shared_ptr<Session>& session);
        void RetryLater(void);

        NetImpl&        m_Net;
        Strand          m_Strand;
        AcceptorSocket  m_Acceptor;
        Timer           m_RetryTimer;
        void*           m_pUserData = nullptr;
        std::atomic<bool> m_bOpen{ false };
    };

    // ============================================================================================================
    // Session
    // ============================================================================================================
    void Session::OnSocketEstablished(void)
    {
        std::error_code ec;

        SetSocketOptions(m_Socket);

        const tcp::endpoint local = m_Socket.local_endpoint(ec);
        if (!ec && local.address().is_v4())
        {
            m_dwLocalAddr = ToNetworkOrder(local.address().to_v4());
            m_wLocalPort = local.port();
        }

        const tcp::endpoint remote = m_Socket.remote_endpoint(ec);
        if (!ec && remote.address().is_v4())
        {
            m_dwRemoteAddr = ToNetworkOrder(remote.address().to_v4());
            m_wRemotePort = remote.port();
        }
    }

    void Session::BeginConnect(const tcp::endpoint& endpoint)
    {
        asio::post(m_Strand, [self = shared_from_this(), endpoint]()
        {
            self->m_Socket.async_connect(endpoint, [self](const std::error_code& ec)
            {
                if (ec)
                {
                    self->m_Net.PushEvent(EventType::ConnectFailed, self);
                    return;
                }

                self->OnSocketEstablished();
                self->m_Net.PushEvent(EventType::Connected, self);
            });
        });
    }

    BOOL Session::GetAddress(UINT32& dwAddr, UINT16& wPort)
    {
        if (m_eState != SessionState::Normal)
            return FALSE;

        dwAddr = m_dwLocalAddr;
        wPort = m_wLocalPort;
        return TRUE;
    }

    BOOL Session::GetRemoteAddress(UINT32& dwRemoteAddr, UINT16& wRemotePort)
    {
        if (m_eState != SessionState::Normal && m_eState != SessionState::Disconnecting)
            return FALSE;

        dwRemoteAddr = m_dwRemoteAddr;
        wRemotePort = m_wRemotePort;
        return TRUE;
    }

    // Copies the packet into the send ring buffer and gives the packet buffer back to the allocator.
    INT32 Session::Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset)
    {
        const int bufSize = static_cast<int>(m_SendBuf.size());

        if (pPacketBuffer == nullptr || nSize <= 0)
        {
            ERR("asio session %p: Send() with invalid packet (ptr %p, size %d)", (void*)this, (void*)pPacketBuffer, nSize);
            return 0;
        }

        if (m_eState != SessionState::Normal)
            return 0;

        // the ring buffer keeps one slot free to tell "full" from "empty"
        if (SendUsed() + nSize >= bufSize)
        {
            CRI("asio session %p: send buffer is full (%d bytes used, %d to send), closing", (void*)this, SendUsed(), nSize);
            Close();
            return 0;
        }

        const char* source = static_cast<const char*>(pPacketBuffer) + nOffset;
        const int rear = m_SendRear.load(std::memory_order_relaxed);   // only this (main) thread moves Rear

        const int firstPart = std::min(nSize, bufSize - rear);
        std::memcpy(m_SendBuf.data() + rear, source, static_cast<size_t>(firstPart));
        if (firstPart < nSize)
            std::memcpy(m_SendBuf.data(), source + firstPart, static_cast<size_t>(nSize - firstPart));

        m_SendRear.store((rear + nSize) % bufSize, std::memory_order_release);

        m_Net.allocator->Free(pPacketBuffer);

        if (!m_bSending.load())
        {
            StartSend();
            m_qwSendTimer = GetTickCountEx() + kSendDelayMs;
        }

        return nSize;
    }

    void Session::StartSend(void)
    {
        bool expected = false;
        if (!m_bSending.compare_exchange_strong(expected, true))
            return;     // a write is already running; it picks up whatever was appended

        asio::post(m_Strand, [self = shared_from_this()]() { self->DoWrite(); });
    }

    // strand
    void Session::DoWrite(void)
    {
        const int bufSize = static_cast<int>(m_SendBuf.size());
        const int head = m_SendHead.load(std::memory_order_acquire);
        const int rear = m_SendRear.load(std::memory_order_acquire);

        if (head == rear)
        {
            OnSendIdle();
            return;
        }

        const int length = (head < rear) ? (rear - head) : (bufSize - head);

        asio::async_write(m_Socket, asio::buffer(m_SendBuf.data() + head, static_cast<size_t>(length)),
            [self = shared_from_this()](const std::error_code& ec, size_t size) { self->OnWritten(ec, size); });
    }

    // strand
    void Session::OnWritten(const std::error_code& ec, size_t size)
    {
        if (ec)
        {
            FailSend();
            return;
        }

        const int bufSize = static_cast<int>(m_SendBuf.size());
        const int head = m_SendHead.load(std::memory_order_relaxed);   // only the strand moves Head
        m_SendHead.store((head + static_cast<int>(size)) % bufSize, std::memory_order_release);

        DoWrite();      // keeps going until the ring buffer is empty
    }

    // strand: nothing left to write
    void Session::OnSendIdle(void)
    {
        m_bSending.store(false);

        // Close() asked for a graceful end: tell the peer we are done sending once everything is flushed.
        if (m_bClosing.load())
        {
            std::error_code ec;
            m_Socket.shutdown(Socket::shutdown_send, ec);
        }

        // Send() may have appended data between the emptiness check and clearing the flag.
        if (m_SendHead.load(std::memory_order_acquire) != m_SendRear.load(std::memory_order_acquire))
        {
            bool expected = false;
            if (m_bSending.compare_exchange_strong(expected, true))
                DoWrite();
        }
    }

    // strand: a failed write means the connection is gone; closing the socket also ends the pending read
    void Session::FailSend(void)
    {
        std::error_code ec;
        m_Socket.close(ec);
        m_bSending.store(false);
    }

    void Session::TrySend(UINT64 qwCurrentTick)
    {
        if (m_bSending.load() || m_qwSendTimer >= qwCurrentTick)
            return;

        if (SendUsed() > 0)
        {
            StartSend();
            m_qwSendTimer = GetTickCountEx() + kSendDelayMs;
        }
    }

    void Session::StartRecv(void)
    {
        m_bRecving.store(true);
        asio::post(m_Strand, [self = shared_from_this()]() { self->DoRead(); });
    }

    // strand
    void Session::DoRead(void)
    {
        const int bufSize = static_cast<int>(m_RecvBuf.size());
        const int head = m_RecvHead.load(std::memory_order_acquire);
        const int rear = m_RecvRear.load(std::memory_order_relaxed);   // only the strand moves Rear

        // contiguous free space behind Rear; one slot stays free so that Head == Rear means "empty"
        int space = (head <= rear) ? (bufSize - rear - (head == 0 ? 1 : 0)) : (head - rear - 1);

        if (space <= 0)
        {
            CRI("asio session %p: receive buffer is full, closing", (void*)this);
            std::error_code ec;
            m_Socket.close(ec);
            m_bRecving.store(false);
            m_Net.PushEvent(EventType::Closed, shared_from_this());
            return;
        }

        space = static_cast<int>(std::min<size_t>(static_cast<size_t>(space), kMaxReadChunk));

        m_Socket.async_read_some(asio::buffer(m_RecvBuf.data() + rear, static_cast<size_t>(space)),
            [self = shared_from_this()](const std::error_code& ec, size_t size) { self->OnRead(ec, size); });
    }

    // strand
    void Session::OnRead(const std::error_code& ec, size_t size)
    {
        if (ec || size == 0)
        {
            m_bRecving.store(false);
            m_Net.PushEvent(EventType::Closed, shared_from_this());
            return;
        }

        const int bufSize = static_cast<int>(m_RecvBuf.size());
        const int rear = m_RecvRear.load(std::memory_order_relaxed);
        m_RecvRear.store((rear + static_cast<int>(size)) % bufSize, std::memory_order_release);

        DoRead();
    }

    // Close() from the application: stop accepting sends, flush what is queued, then let the peer hang up.
    BOOL Session::Close(void)
    {
        switch (m_eState)
        {
            case SessionState::Connecting:
            {
                m_eState = SessionState::Disconnecting;
                Shutdown();     // aborts the pending connect, which reports ConnectFailed
            }
            break;
            case SessionState::Normal:
            {
                m_eState = SessionState::Disconnecting;
                m_bClosing.store(true);

                if (SendUsed() == 0 && !m_bSending.load())
                {
                    asio::post(m_Strand, [self = shared_from_this()]()
                    {
                        std::error_code ec;
                        self->m_Socket.shutdown(Socket::shutdown_send, ec);
                    });
                }
                else
                {
                    StartSend();    // OnSendIdle() shuts the send side down after the flush
                }

                m_qwShutdownTimer = GetTickCountEx() + kShutdownTimeoutMs;
            }
            break;
            case SessionState::Disconnecting:
                break;
            default:
                CRI("asio session %p: Close() in state %d", (void*)this, static_cast<int>(m_eState));
                return FALSE;
        }

        return TRUE;
    }

    void Session::Shutdown(void)
    {
        if (m_bShutdownPosted)
            return;
        m_bShutdownPosted = true;

        asio::post(m_Strand, [self = shared_from_this()]()
        {
            std::error_code ec;
            self->m_Socket.close(ec);
        });
    }

    // Takes one complete packet out of the receive ring buffer and hands it to the event handler.
    // Returns the size of the delivered packet, or 0 if there is none (yet).
    INT32 Session::Recv(void)
    {
        using namespace ExternalMessage;

        const int bufSize = static_cast<int>(m_RecvBuf.size());
        PACKET_BUFFER* pPacketBuffer = nullptr;

        if (m_bProtocolError || (m_eState != SessionState::Normal && m_eState != SessionState::Disconnecting))
            return 0;

        const int head = m_RecvHead.load(std::memory_order_relaxed);   // only this (main) thread moves Head
        const int rear = m_RecvRear.load(std::memory_order_acquire);
        if (head == rear)
            return 0;

        int available = rear - head;
        if (available < 0)
            available += bufSize;
        if (available < static_cast<int>(sizeof(MESSAGE_HEADER)))
            return 0;

        auto byteAt = [&](int index) -> int { return static_cast<unsigned char>(m_RecvBuf[static_cast<size_t>(index % bufSize)]); };

        const int messageId = byteAt(head) | (byteAt(head + 1) << 8);
        if (messageId >= max_message_count)
        {
            CRI("asio session %p: invalid message id %d from client", (void*)this, messageId);
            m_bProtocolError = true;
            Close();
            return 0;
        }

        int messageSize = m_Net.param.pnMessageSizeList[messageId];
        if (messageSize == flex_message_size)
        {
            if (available < static_cast<int>(sizeof(FLEX_MESSAGE_HEADER)))
                return 0;
            messageSize = byteAt(head + static_cast<int>(sizeof(MESSAGE_HEADER)))
                        | (byteAt(head + static_cast<int>(sizeof(MESSAGE_HEADER)) + 1) << 8);
        }

        if (messageSize <= 0 || messageSize >= bufSize)
        {
            CRI("asio session %p: invalid size %d of message %d", (void*)this, messageSize, messageId);
            m_bProtocolError = true;
            Close();
            return 0;
        }

        if (available < messageSize)
            return 0;       // not complete yet

        pPacketBuffer = m_Net.allocator->Alloc(messageSize);
        if (pPacketBuffer == nullptr)
        {
            CRI("asio session %p: no packet buffer for %d bytes", (void*)this, messageSize);
            m_bProtocolError = true;
            Close();
            return 0;
        }

        const int firstPart = std::min(messageSize, bufSize - head);
        std::memcpy(pPacketBuffer, m_RecvBuf.data() + head, static_cast<size_t>(firstPart));
        if (firstPart < messageSize)
            std::memcpy(pPacketBuffer + firstPart, m_RecvBuf.data(), static_cast<size_t>(messageSize - firstPart));

        m_RecvHead.store((head + messageSize) % bufSize, std::memory_order_release);

        m_Net.handler->OnRecv(this, pPacketBuffer, static_cast<UINT32>(messageSize));
        return messageSize;
    }

    // ============================================================================================================
    // Acceptor
    // ============================================================================================================
    BOOL Acceptor::Open(UINT32 dwAddr, UINT16 wPort)
    {
        if (dwAddr == INADDR_NONE)
            return FALSE;

        std::error_code ec;
        const tcp::endpoint endpoint(FromNetworkOrder(dwAddr), wPort);

        m_Acceptor.open(endpoint.protocol(), ec);
        if (ec)
        {
            CRI("asio acceptor: open failed: %s", ec.message().c_str());
            return FALSE;
        }

        // Callers probe for a free port by calling Open() until it succeeds, so a port that is in use must make
        // bind() fail. On Windows SO_REUSEADDR would let a second socket bind an already listening port.
#if !defined(_WIN32)
        m_Acceptor.set_option(asio::socket_base::reuse_address(true), ec);
#endif

        m_Acceptor.bind(endpoint, ec);
        if (!ec)
            m_Acceptor.listen(kListenBacklog, ec);
        if (ec)
        {
            std::error_code ignored;
            m_Acceptor.close(ignored);
            return FALSE;       // port busy etc.; the caller tries the next one
        }

        m_bOpen.store(true);
        asio::post(m_Strand, [this]() { DoAccept(); });
        return TRUE;
    }

    BOOL Acceptor::Close(void)
    {
        if (!m_bOpen.exchange(false))
            return TRUE;

        // the acceptor is only touched on its strand
        auto done = std::make_shared<std::promise<void>>();
        std::future<void> finished = done->get_future();
        asio::post(m_Strand, [this, done]()
        {
            std::error_code ec;
            m_Acceptor.close(ec);
            m_RetryTimer.cancel();
            done->set_value();
        });

        // wait for it, unless there is nobody left to run the strand
        if (!m_Net.threads.empty())
            finished.wait();

        return TRUE;
    }

    // strand
    void Acceptor::DoAccept(void)
    {
        if (!m_bOpen.load())
            return;

        if (m_Net.sessionCount.load() >= m_Net.param.nMaxSessionCount)
        {
            RetryLater();       // no free session: leave the connection in the backlog for a moment
            return;
        }

        auto session = std::make_shared<Session>(m_Net, this);
        m_Acceptor.async_accept(session->GetSocket(), [this, session](const std::error_code& ec) { OnAccept(ec, session); });
    }

    // strand
    void Acceptor::OnAccept(const std::error_code& ec, const std::shared_ptr<Session>& session)
    {
        if (ec)
        {
            if (ec == asio::error::operation_aborted || !m_bOpen.load())
                return;

            ERR("asio acceptor: accept failed: %s", ec.message().c_str());
            RetryLater();
            return;
        }

        session->OnSocketEstablished();
        m_Net.PushEvent(EventType::Accepted, session, this);

        DoAccept();
    }

    // strand
    void Acceptor::RetryLater(void)
    {
        m_RetryTimer.expires_after(std::chrono::milliseconds(100));
        m_RetryTimer.async_wait([this](const std::error_code& ec)
        {
            if (!ec)
                DoAccept();
        });
    }

    // ============================================================================================================
    // NetImpl
    // ============================================================================================================
    bool NetImpl::Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEventHandler, void* pInitParam)
    {
        if (m_bInitialized)
            return false;
        if (!pPacketAllocator || !pNetEventHandler || !pInitParam)
        {
            ERR("asio net: Init() with null argument");
            return false;
        }

        std::memcpy(&param, pInitParam, sizeof(param));

        if (param.pnMessageSizeList == nullptr || param.nMaxSessionCount <= 0 || param.nThreadCount <= 0 ||
            param.nSendLoopBufferSize <= 1 || param.nRecvLoopBufferSize <= 1)
        {
            ERR("asio net: invalid init parameter (sessions %d, threads %d, send buffer %d, recv buffer %d)",
                param.nMaxSessionCount, param.nThreadCount, param.nSendLoopBufferSize, param.nRecvLoopBufferSize);
            return false;
        }

        if (param.nMaxOperationPerLoop <= 0)
            param.nMaxOperationPerLoop = 1;

        allocator = pPacketAllocator;
        handler = pNetEventHandler;

        try
        {
            ctx.restart();
            workGuard.emplace(asio::make_work_guard(ctx));
            threads.reserve(param.nThreadCount);
            for (int i = 0; i < param.nThreadCount; i++)
            {
                threads.emplace_back([this](std::stop_token token)
                {
                    std::stop_callback stopped(token, [this] { ctx.stop(); });
                    for (;;)
                    {
                        try
                        {
                            ctx.run();
                            break;
                        }
                        catch (const std::exception& e)
                        {
                            CRI("asio work thread exception: %s", e.what());
                        }
                    }
                    INF("asio work thread quit");
                });
            }
        }
        catch (const std::exception& error)
        {
            workGuard.reset();
            ctx.stop();
            threads.clear(); // jthread joins every worker already launched.
            allocator = nullptr;
            handler = nullptr;
            ERR("asio net: cannot start workers: %s", error.what());
            return false;
        }

        m_bInitialized = true;
        return true;
    }

    void NetImpl::UnInit(void)
    {
        if (!m_bInitialized)
            return;
        m_bInitialized = false;

        // stop listening, then close every socket so that the outstanding operations complete
        for (auto& acceptor : m_Acceptors)
            acceptor->Close();

        for (auto& session : m_Sessions)
            session->Shutdown();

        workGuard.reset();
        for (auto& thread : threads)
        {
            if (thread.joinable())
                thread.join();
        }
        threads.clear();

        // deliver the disconnects that the shutdown caused
        MainLoop();

        if (!m_Sessions.empty())
            INF("asio net: %d sessions left after shutdown", static_cast<int>(m_Sessions.size()));
        m_Sessions.clear();
        m_Acceptors.clear();
        m_Events.clear();

        allocator = nullptr;
        handler = nullptr;
    }

    ISocketAcceptor* NetImpl::CreateAcceptor(void)
    {
        m_Acceptors.push_back(std::make_unique<Acceptor>(*this));
        return m_Acceptors.back().get();
    }

    bool NetImpl::DestroyAcceptor(ISocketAcceptor* pSocketAcceptor)
    {
        for (auto it = m_Acceptors.begin(); it != m_Acceptors.end(); ++it)
        {
            if (it->get() == pSocketAcceptor)
            {
                (*it)->Close();
                m_Acceptors.erase(it);
                return true;
            }
        }
        return false;
    }

    bool NetImpl::IsAcceptorAlive(const Acceptor* acceptor) const
    {
        for (const auto& item : m_Acceptors)
        {
            if (item.get() == acceptor)
                return true;
        }
        return false;
    }

    ISocketSession* NetImpl::Connect(UINT32 dwAddr, UINT16 wPort)
    {
        if (sessionCount.load() >= param.nMaxSessionCount)
        {
            CRI("asio net: no free session for connect");
            return nullptr;
        }

        auto session = std::make_shared<Session>(*this, nullptr);
        session->SetState(SessionState::Connecting);
        m_Sessions.push_back(session);

        session->BeginConnect(tcp::endpoint(FromNetworkOrder(dwAddr), wPort));
        return session.get();
    }

    void NetImpl::PushEvent(EventType type, std::shared_ptr<Session> session, Acceptor* acceptor)
    {
        std::lock_guard<std::mutex> lock(m_EventMutex);
        m_Events.push_back(Event{ type, std::move(session), acceptor });
    }

    void NetImpl::HandleEvent(const Event& event)
    {
        const std::shared_ptr<Session>& session = event.session;

        switch (event.type)
        {
            case EventType::Accepted:
            {
                if (!IsAcceptorAlive(event.acceptor) || !m_bInitialized)
                {
                    // the acceptor was destroyed while the connection was on its way
                    session->SetState(SessionState::Cleanup);
                    session->Shutdown();
                    m_Sessions.push_back(session);
                    break;
                }

                session->SetState(SessionState::Normal);
                m_Sessions.push_back(session);

                handler->OnAccepted(event.acceptor, session.get());
                session->StartRecv();
            }
            break;
            case EventType::Connected:
            {
                if (session->GetState() == SessionState::Connecting)
                {
                    session->SetState(SessionState::Normal);

                    handler->OnConnected(session.get());
                    session->StartRecv();
                    break;
                }
            }
            // Close() was called while connecting: the connection is no use any more
            [[fallthrough]];
            case EventType::ConnectFailed:
            {
                if (session->GetState() != SessionState::Cleanup)
                {
                    session->SetState(SessionState::Cleanup);
                    session->Shutdown();
                    handler->OnDisconnected(session.get(), FALSE);
                }
            }
            break;
            case EventType::Closed:
            {
                const SessionState state = session->GetState();
                if (state == SessionState::Normal || state == SessionState::Disconnecting)
                {
                    while (session->Recv() > 0) {}      // hand over what arrived before the connection ended

                    session->SetState(SessionState::Cleanup);
                    session->Shutdown();
                    handler->OnDisconnected(session.get(), state == SessionState::Normal ? TRUE : FALSE);
                }
            }
            break;
        }
    }

    bool NetImpl::MainLoop(void)
    {
        if (handler == nullptr)
            return false;

        const UINT64 qwStartTick = GetTickCountEx();
        UINT64 qwCurrentTick = qwStartTick;

        // 1. sessions: deliver packets, flush sends, expire closing sessions, release finished ones
        for (auto it = m_Sessions.begin(); it != m_Sessions.end();)
        {
            const std::shared_ptr<Session> session = *it;     // keep it alive while a callback runs
            auto next = std::next(it);

            switch (session->GetState())
            {
                case SessionState::Disconnecting:
                case SessionState::Normal:
                {
                    if (session->GetState() == SessionState::Disconnecting && session->GetShutdownTimer() < qwCurrentTick)
                        session->Shutdown();

                    int budget = param.nMaxOperationPerLoop;
                    while (budget > 0 && session->Recv() > 0)
                        budget--;
                    if (budget <= 0)
                        CRI("asio session %p: received package count is over max package per loop", (void*)session.get());

                    if (session->GetState() == SessionState::Normal)
                        session->TrySend(qwCurrentTick);
                }
                break;
                case SessionState::Cleanup:
                {
                    if (session->CheckCleanup())
                        next = m_Sessions.erase(it);
                }
                break;
                default:
                    break;
            }

            it = next;

            qwCurrentTick = GetTickCountEx();
            if (qwCurrentTick - qwStartTick > kMaxMainLoopTimeMs)
            {
                INF("processing asio session message over time %d", static_cast<int>(qwCurrentTick - qwStartTick));
                break;
            }
        }

        // 2. connection events produced by the worker threads
        std::vector<Event> events;
        {
            std::lock_guard<std::mutex> lock(m_EventMutex);
            events.swap(m_Events);
        }

        for (size_t index = 0; index < events.size(); index++)
        {
            HandleEvent(events[index]);

            qwCurrentTick = GetTickCountEx();
            if (qwCurrentTick - qwStartTick > kMaxMainLoopTimeMs)
            {
                INF("processing asio session event over time %d", static_cast<int>(qwCurrentTick - qwStartTick));

                // put the rest back in front of whatever arrived in the meantime
                std::lock_guard<std::mutex> lock(m_EventMutex);
                m_Events.insert(m_Events.begin(), events.begin() + static_cast<std::ptrdiff_t>(index) + 1, events.end());
                break;
            }
        }

        return true;
    }

    INT64 NetImpl::PrintMemoryStat(void)
    {
        const INT64 sessions = sessionCount.load();
        const INT64 bytes = sessions * (static_cast<INT64>(param.nSendLoopBufferSize) + param.nRecvLoopBufferSize);

        INF("asio net: %d sessions, ring buffers %.2fMB", static_cast<int>(sessions), bytes / (1024.0 * 1024.0));
        return bytes / (1024 * 1024);
    }

    // ============================================================================================================
    // CNet
    // ============================================================================================================
    CNet::CNet() : m_pImpl(std::make_unique<NetImpl>()) {}
    CNet::~CNet() { m_pImpl->UnInit(); }

    BOOL CNet::Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEventHandler, void* pInitParam)
    {
        return m_pImpl->Init(pPacketAllocator, pNetEventHandler, pInitParam) ? TRUE : FALSE;
    }

    BOOL CNet::UnInit(void)
    {
        m_pImpl->UnInit();
        return TRUE;
    }

    ISocketAcceptor* CNet::CreateAcceptor(void) { return m_pImpl->CreateAcceptor(); }
    BOOL CNet::DestroyAcceptor(ISocketAcceptor* pSocketAcceptor) { return m_pImpl->DestroyAcceptor(pSocketAcceptor) ? TRUE : FALSE; }
    ISocketSession* CNet::Connect(UINT32 dwAddr, UINT16 wPort) { return m_pImpl->Connect(dwAddr, wPort); }
    BOOL CNet::MainLoop(void) { return m_pImpl->MainLoop() ? TRUE : FALSE; }
    INT64 CNet::PrintMemoryStat(void) { return m_pImpl->PrintMemoryStat(); }
}
