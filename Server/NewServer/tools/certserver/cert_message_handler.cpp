#include "stdafx.h"
#include "cert_message_handler.h"
#include "math_ex.h"
#include "zlib.h"
#include "cer_message.h"

#include <cstring>
#include <limits>
#include <memory>
#include <utility>

CCertMessageHandler g_MessageHandler;
using namespace InternalMessage;

namespace
{
struct CloseConnection
{
    ISocketSession* socket;
    ~CloseConnection()
    {
        if (!socket) return;
        try { LOG_CHECK_ERROR(socket->Close()); }
        catch (...) { ERR("cert connection close threw an exception"); }
    }
};
}

void CCertMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession* socket)
try
{
    const CloseConnection close{socket};
    if (!socket || !m_accepting || !m_pPacketAllocator) return;
    UINT32 address = 0;
    UINT16 port = 0;
    if (socket->GetRemoteAddress(address, port))
    {
        in_addr ip{};
        ip.s_addr = address;
        INF("connected from %s:%u", inet_ntoa(ip), port);
    }
    const auto free = [allocator = m_pPacketAllocator](PACKET_BUFFER* packet) { allocator->Free(packet); };
    std::unique_ptr<PACKET_BUFFER, decltype(free)> packet{
        m_pPacketAllocator->Alloc(static_cast<INT32>(m_certificate.size())), free};
    if (!packet) { ERR("cert packet allocation failed"); return; }
    std::memcpy(packet.get(), m_certificate.data(), m_certificate.size());
    const auto size = static_cast<INT32>(m_certificate.size());
    if (socket->Send(packet.get(), size, 0) != size)
    {
        ERR("cert packet send failed");
        return;
    }
    packet.release(); // Native Send consumes the packet on success.
}
catch (const std::exception& error) { ERR("cert connection failed: %s", error.what()); }
catch (...) { ERR("cert connection threw an exception"); }

void CCertMessageHandler::OnConnected(ISocketSession* socket)
{
    const CloseConnection close{socket};
}

void CCertMessageHandler::OnRecv(ISocketSession* socket, PACKET_BUFFER* buffer, UINT32)
{
    const CloseConnection close{socket};
    const auto free = [allocator = m_pPacketAllocator](PACKET_BUFFER* packet) {
        if (allocator) allocator->Free(packet);
    };
    const std::unique_ptr<PACKET_BUFFER, decltype(free)> packet{buffer, free};
}

void CCertMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}

CCertMessageHandler::CCertMessageHandler() : m_pNet(nullptr), m_pPacketAllocator(nullptr) {}
CCertMessageHandler::~CCertMessageHandler() { UnInit(); }

BOOL CCertMessageHandler::Init(INet* net, IPacketAllocator* allocator)
try
{
    if (!net || !allocator || m_pNet || m_pPacketAllocator) return FALSE;
    static_assert(sizeof(g_MathTable) <= (std::numeric_limits<uLong>::max)());
    // The protocol can only carry one packet; reject incompressible tables
    // instead of reserving a permanent 16 MiB global scratch buffer.
    std::vector<char> compressed(MAX_MESSAGE_SIZE - sizeof(CER_SYNC_CERTIFICATION));
    uLongf length = static_cast<uLongf>(compressed.size());
    const int result = compress(reinterpret_cast<Bytef*>(compressed.data()), &length,
        reinterpret_cast<const Bytef*>(&g_MathTable), static_cast<uLong>(sizeof(g_MathTable)));
    if (result != Z_OK || length == 0 || length > compressed.size())
    {
        ERR("cert table compression failed: %d", result);
        return FALSE;
    }
    std::vector<char> certificate(sizeof(CER_SYNC_CERTIFICATION) + length);
    CER_SYNC_CERTIFICATION header{};
    header.wMsg = cer_sync_certification;
    header.byKey = 206;
    std::memcpy(certificate.data(), &header, sizeof(header));
    std::memcpy(certificate.data() + sizeof(header), compressed.data(), length);
    m_certificate = std::move(certificate);
    m_pNet = net;
    m_pPacketAllocator = allocator;
    m_accepting = true;
    return TRUE;
}
catch (const std::exception& error)
{
    ERR("cert initialization failed: %s", error.what());
    return FALSE;
}

BOOL CCertMessageHandler::UnInit()
{
    BeginShutdown();
    m_pNet = nullptr;
    m_pPacketAllocator = nullptr;
    m_certificate.clear();
    return TRUE;
}

CCertMessageHandler& CCertMessageHandler::GetInstance() { return g_MessageHandler; }
