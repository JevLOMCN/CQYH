#pragma once
#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "net.h"
#include <memory>
#include <cstring>
#include <span>
#include <utility>

namespace gm
{
// The allocator must outlive packets delivered by its transport. Capture it
// before callbacks, which may replace the handler's current runtime.
struct PacketDeleter
{
    IPacketAllocator* allocator{};
    std::shared_ptr<void> lifetime;
    void operator()(PACKET_BUFFER* packet) const
    {
        if (packet && allocator) allocator->Free(packet);
    }
};
using Packet = std::unique_ptr<PACKET_BUFFER, PacketDeleter>;
inline Packet copy_packet(IPacketAllocator* allocator, std::span<const char> bytes,std::shared_ptr<void> lifetime={})
{
    Packet packet(nullptr, PacketDeleter{allocator,std::move(lifetime)});
    if (!allocator || bytes.empty() || bytes.size() > MAX_MESSAGE_SIZE) return packet;
    packet.reset(allocator->Alloc(static_cast<INT32>(bytes.size())));
    if (packet) std::memcpy(packet.get(), bytes.data(), bytes.size());
    return packet;
}
}
