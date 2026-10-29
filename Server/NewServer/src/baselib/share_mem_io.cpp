#include "stdafx.h"
#include "share_mem_io.h"
#include "cas.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <span>
#include <thread>

CShareMemBuffer::CShareMemBuffer() = default;
CShareMemBuffer::~CShareMemBuffer() { Destroy(); }

BOOL CShareMemBuffer::Create(const char* name, INT32 size, INT32 userHeaderSize)
{
    if (m_Ptr.pSharedMem || !name || !*name || size < 2 || userHeaderSize < 0 ||
        userHeaderSize % alignof(INT32) != 0)
    {
        LOG_CHECK_ERROR(!m_Ptr.pSharedMem && name && *name && size >= 2 &&
                        userHeaderSize >= 0 && userHeaderSize % alignof(INT32) == 0);
        return FALSE;
    }
    const auto total = static_cast<UINT64>(size) + sizeof(MemHeader) + userHeaderSize;
    if (!CreateSharedMemory(name, total, m_Ptr))
        return FALSE;
    if (m_Ptr.qwSize < static_cast<UINT64>(userHeaderSize) + sizeof(MemHeader))
    {
        Destroy();
        return FALSE;
    }
    auto* header = reinterpret_cast<MemHeader*>(m_Ptr.pSharedMem + userHeaderSize);
    if (m_Ptr.bIsNew)
    {
        header->nBlockSize = size;
        cqyh::SharedStore32(&header->nReadOffset, 0);
        cqyh::SharedStore32(&header->nWriteOffset, 0);
        // Publish immutable metadata and zero offsets only after initialization.
        cqyh::SharedStore32(&header->nMemTag, SHARED_MEM_TAG);
    }
    if (!Attach(userHeaderSize))
    {
        Destroy();
        return FALSE;
    }
    return TRUE;
}

BOOL CShareMemBuffer::Open(const char* name)
{
    return Open(name, 0);
}

BOOL CShareMemBuffer::Open(const char* name, INT32 userHeaderSize)
{
    if (m_Ptr.pSharedMem || !name || !*name || userHeaderSize < 0 ||
        userHeaderSize % alignof(INT32) != 0)
    {
        LOG_CHECK_ERROR(!m_Ptr.pSharedMem && name && *name && userHeaderSize >= 0 &&
                        userHeaderSize % alignof(INT32) == 0);
        return FALSE;
    }
    if (!OpenSharedMemory(name, m_Ptr))
        return FALSE;
    if (!Attach(userHeaderSize))
    {
        Destroy();
        return FALSE;
    }
    return TRUE;
}

BOOL CShareMemBuffer::Attach(INT32 userHeaderSize)
{
    const auto prefix = static_cast<UINT64>(userHeaderSize) + sizeof(MemHeader);
    if (!m_Ptr.pSharedMem || m_Ptr.qwSize < prefix)
        return FALSE;
    auto* header = reinterpret_cast<MemHeader*>(m_Ptr.pSharedMem + userHeaderSize);
    if (reinterpret_cast<std::uintptr_t>(header) % alignof(INT32) != 0)
        return FALSE;
    // Another process may have created the section but not yet published its ring.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (cqyh::SharedLoad32(&header->nMemTag) != SHARED_MEM_TAG)
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            ERR("Shared-memory ring metadata was not published");
            return FALSE;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    const auto size = header->nBlockSize;
    if (size < 2 || static_cast<UINT64>(size) > m_Ptr.qwSize - prefix)
    {
        LOG_CHECK_ERROR(size >= 2 && static_cast<UINT64>(size) <= m_Ptr.qwSize - prefix);
        return FALSE;
    }
    m_pHeader = header;
    m_pUserBlock = reinterpret_cast<char*>(header) + sizeof(MemHeader);
    m_nBlockSize = size;
    std::size_t read, write;
    return ReadOffsets(read, write);
}

bool CShareMemBuffer::ReadOffsets(std::size_t& read, std::size_t& write) const
{
    if (!m_pHeader || m_nBlockSize < 2)
        return false;
    const auto head = cqyh::SharedLoad32(&m_pHeader->nReadOffset);
    const auto tail = cqyh::SharedLoad32(&m_pHeader->nWriteOffset);
    if (head < 0 || tail < 0 || head > m_nBlockSize || tail > m_nBlockSize)
    {
        LOG_CHECK_ERROR(head >= 0 && tail >= 0 && head <= m_nBlockSize && tail <= m_nBlockSize);
        return false;
    }
    // Older peers could publish the end-of-block position instead of zero.
    read = static_cast<std::size_t>(head) % m_nBlockSize;
    write = static_cast<std::size_t>(tail) % m_nBlockSize;
    return true;
}

void CShareMemBuffer::Clear()
{
    if (m_pHeader)
    {
        cqyh::SharedStore32(&m_pHeader->nReadOffset, 0);
        cqyh::SharedStore32(&m_pHeader->nWriteOffset, 0);
    }
}

void CShareMemBuffer::Destroy()
{
    if (m_Ptr.pSharedMem)
        CloseSharedMemory(m_Ptr);
    m_Ptr = {};
    m_pHeader = nullptr;
    m_pUserBlock = nullptr;
    m_nBlockSize = 0;
}

BOOL CShareMemBuffer::Write(const char* data, INT32 length)
{
    if (length < 0 || (!data && length != 0))
        return FALSE;
    std::size_t read, write;
    if (!ReadOffsets(read, write))
        return FALSE;
    const auto size = static_cast<std::size_t>(m_nBlockSize);
    const auto used = (write + size - read) % size;
    const auto count = static_cast<std::size_t>(length);
    if (count > size - 1 - used)
        return FALSE;
    if (count == 0)
        return TRUE;
    const std::span<char> ring(m_pUserBlock, size);
    const auto first = (std::min)(count, size - write);
    std::memcpy(ring.subspan(write, first).data(), data, first);
    if (count > first)
        std::memcpy(ring.data(), data + first, count - first);
    // The peer must observe the payload before it observes the new write offset.
    cqyh::SharedStore32(&m_pHeader->nWriteOffset, static_cast<INT32>((write + count) % size));
    return TRUE;
}

BOOL CShareMemBuffer::Read(char* data, INT32 length)
{
    if (length < 0 || (!data && length != 0))
        return FALSE;
    std::size_t read, write;
    if (!ReadOffsets(read, write))
        return FALSE;
    const auto size = static_cast<std::size_t>(m_nBlockSize);
    const auto available = (write + size - read) % size;
    const auto count = static_cast<std::size_t>(length);
    if (count > available)
        return FALSE;
    if (count == 0)
        return TRUE;
    const std::span<const char> ring(m_pUserBlock, size);
    const auto first = (std::min)(count, size - read);
    std::memcpy(data, ring.subspan(read, first).data(), first);
    if (count > first)
        std::memcpy(data + first, ring.data(), count - first);
    // Release consumed storage only after its bytes have been copied out.
    cqyh::SharedStore32(&m_pHeader->nReadOffset, static_cast<INT32>((read + count) % size));
    return TRUE;
}
