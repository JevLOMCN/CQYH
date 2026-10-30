#ifndef CQYH_BASELIB_CALLBACK_QUEUE_H
#define CQYH_BASELIB_CALLBACK_QUEUE_H

#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <limits>
#include <utility>

namespace cqyh
{
// Process-local owned records from callback or control-event threads.
// No producer-thread guarantee is required. Legacy slot counts reserve one slot.
template<class T>
class CallbackQueue
{
public:
    using Pointer = std::unique_ptr<T>;
    CallbackQueue() = default;
    ~CallbackQueue() { UnInit(); }
    CallbackQueue(const CallbackQueue&) = delete;
    CallbackQueue& operator=(const CallbackQueue&) = delete;

    // Control events must not be dropped when both directions are busy. Their
    // count is governed by the sessions, rather than a fixed ring capacity.
    bool Init() noexcept
    {
        return InitSlots((std::numeric_limits<std::size_t>::max)());
    }

    bool InitSlots(std::size_t slots) noexcept
    {
        if (slots == 0)
            return false;
        try
        {
            auto pending = std::make_unique<Queue>();
            std::scoped_lock lock(m_mutex);
            if (m_pending)
                return false;
            m_pending = std::move(pending);
            m_capacity = slots - 1;
            m_accepting = true;
            return true;
        }
        catch (...) { return false; }
    }

    // Ownership is consumed on acceptance, rejection, or allocation failure.
    bool Push(Pointer record) noexcept
    {
        if (!record)
            return false;
        try
        {
            std::scoped_lock lock(m_mutex);
            if (!m_accepting || !m_pending || m_pending->size() >= m_capacity)
                return false;
            m_pending->push_back(std::move(record));
            return true;
        }
        catch (...) { return false; }
    }

    Pointer Pop()
    {
        std::scoped_lock lock(m_mutex);
        if (!m_pending || m_pending->empty())
            return {};
        auto record = std::move(m_pending->front());
        m_pending->pop_front();
        return record;
    }

    bool IsEmpty() const
    {
        std::scoped_lock lock(m_mutex);
        return !m_pending || m_pending->empty();
    }

    std::size_t Size() const
    {
        std::scoped_lock lock(m_mutex);
        return m_pending ? m_pending->size() : 0;
    }

    bool IsInitialized() const
    {
        std::scoped_lock lock(m_mutex);
        return m_pending != nullptr;
    }

    void Close()
    {
        std::scoped_lock lock(m_mutex);
        m_accepting = false;
    }

    void UnInit()
    {
        std::unique_ptr<Queue> abandoned;
        {
            std::scoped_lock lock(m_mutex);
            m_accepting = false;
            m_capacity = 0;
            abandoned = std::move(m_pending);
        }
        // Destroy records outside the lock, including reentrant cleanup.
    }

private:
    using Queue = std::deque<Pointer>;
    mutable std::mutex m_mutex;
    std::unique_ptr<Queue> m_pending;
    std::size_t m_capacity = 0;
    bool m_accepting = false;
};
}
#endif // CQYH_BASELIB_CALLBACK_QUEUE_H
