#ifndef _CIRCLE_LIST_H_
#define _CIRCLE_LIST_H_

#include <atomic>
#include <vector>

// Process-local single-producer/single-consumer queue of borrowed pointers.
// One slot is reserved, retaining the legacy capacity of Init(size) - 1.
// Init/UnInit require both participants to have stopped. Check/Pop are consumer-only.
class CCircleList
{
public:
    CCircleList() = default;
    ~CCircleList() = default;
    CCircleList(const CCircleList&) = delete;
    CCircleList& operator=(const CCircleList&) = delete;

    BOOL Init(UINT32 size);
    BOOL UnInit();

    BOOL Push(void* data)
    {
        if (m_data.empty())
            return FALSE;
        const auto rear = m_rear.load(std::memory_order_relaxed);
        const auto next = (rear + 1) % m_data.size();
        if (next == m_head.load(std::memory_order_acquire))
            return FALSE;
        m_data[rear] = data;
        m_rear.store(next, std::memory_order_release);
        return TRUE;
    }

    void* Pop()
    {
        const auto head = m_head.load(std::memory_order_relaxed);
        if (head == m_rear.load(std::memory_order_acquire))
            return nullptr;
        auto* data = m_data[head];
        m_head.store((head + 1) % m_data.size(), std::memory_order_release);
        return data;
    }

    void* Check()
    {
        const auto head = m_head.load(std::memory_order_relaxed);
        if (head == m_rear.load(std::memory_order_acquire))
            return nullptr;
        return m_data[head];
    }

    BOOL IsEmpty() const
    {
        return m_head.load(std::memory_order_acquire) == m_rear.load(std::memory_order_acquire);
    }

private:
    std::vector<void*> m_data;
    std::atomic<std::size_t> m_head{0};
    std::atomic<std::size_t> m_rear{0};
};

#endif // _CIRCLE_LIST_H_
