#include "stdafx.h"
#include "iocp_mgr.h"

#include <condition_variable>
#include <exception>
#include <list>
#include <mutex>
#include <utility>

namespace log_analysis
{
struct Workers::State
{
    std::mutex mutex;
    std::condition_variable work_ready;
    std::condition_variable result_ready;
    std::list<Pointer> pending;
    std::list<Pointer> active;
    std::list<Pointer> completed;
    Processor processor;
    std::size_t outstanding = 0;
    bool accepting = true;
};

Workers::~Workers()
{
    Stop();
}

bool Workers::Start(std::size_t count, Processor processor) noexcept
{
    if (m_state.load() || count == 0 || count > 256 || !processor)
        return false;
    std::shared_ptr<State> state;
    try
    {
        state = std::make_shared<State>();
        state->processor = processor;
        // Reuse the member container: MSVC's checked vector constructor may allocate
        // inside a noexcept constructor, before a local rollback guard can exist.
        m_threads.reserve(count);
        for (std::size_t i = 0; i < count; ++i)
            m_threads.emplace_back([state](std::stop_token stop) { Run(state, stop); });
        m_state.store(std::move(state));
        return true;
    }
    catch (...)
    {
        if (state)
        {
            {
                std::scoped_lock lock(state->mutex);
                state->accepting = false;
            }
            state->work_ready.notify_all();
        }
        for (auto& thread : m_threads)
            thread.request_stop();
        m_threads.clear();
        return false;
    }
}

void Workers::Stop() noexcept
{
    const auto state = m_state.exchange({});
    if (!state)
        return;
    {
        std::scoped_lock lock(state->mutex);
        state->accepting = false;
        state->outstanding -= state->pending.size();
        state->pending.clear();
    }
    for (auto& thread : m_threads)
        thread.request_stop();
    state->work_ready.notify_all();
    state->result_ready.notify_all();
    m_threads.clear(); // Join before releasing active records and the shared state.
}

bool Workers::Submit(Pointer task) noexcept
{
    const auto state = m_state.load();
    if (!state || !task)
        return false;
    try
    {
        {
            std::scoped_lock lock(state->mutex);
            if (!state->accepting)
                return false;
            state->pending.push_back(std::move(task));
            ++state->outstanding;
        }
        state->work_ready.notify_one();
        return true;
    }
    catch (...) { return false; }
}

Workers::Pointer Workers::WaitResult()
{
    const auto state = m_state.load();
    if (!state)
        return {};
    std::unique_lock lock(state->mutex);
    state->result_ready.wait(lock, [&]
    { return !state->completed.empty() || !state->accepting || state->outstanding == 0; });
    if (state->completed.empty())
        return {};
    auto result = std::move(state->completed.front());
    state->completed.pop_front();
    --state->outstanding;
    if (state->outstanding == 0)
        state->result_ready.notify_all();
    return result;
}

std::size_t Workers::Pending() const
{
    const auto state = m_state.load();
    if (!state)
        return 0;
    std::scoped_lock lock(state->mutex);
    return state->outstanding;
}

void Workers::Run(const std::shared_ptr<State>& state, std::stop_token stop)
{
    for (;;)
    {
        std::unique_lock lock(state->mutex);
        state->work_ready.wait(lock, [&]
        { return !state->pending.empty() || !state->accepting || stop.stop_requested(); });
        if (!state->accepting || stop.stop_requested())
            return;
        const auto record = state->pending.begin();
        state->active.splice(state->active.end(), state->pending, record);
        Task& task = **record;
        lock.unlock();
        try { state->processor(task, stop); }
        catch (const std::exception& error)
        {
            task.failed = true;
            try { task.diagnostic = error.what(); } catch (...) { }
        }
        catch (...) { task.failed = true; }
        lock.lock();
        // Splice the existing node so publishing a result cannot fail an allocation.
        state->completed.splice(state->completed.end(), state->active, record);
        lock.unlock();
        state->result_ready.notify_one();
    }
}
}
