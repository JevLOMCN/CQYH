#include "stdafx.h"
#include "thread.h"

#include <cstdio>
#include <exception>
#include <utility>

CThread::CThread() = default;

CThread::~CThread()
{
    Wait();
}

BOOL CThread::Start()
{
    return Start(Work([this](std::stop_token) { ThrdProc(); }));
}

BOOL CThread::Start(PFThrdProc function, void* parameter)
{
    if (!function)
        return FALSE;
    return Start(Work([function, parameter](std::stop_token) { function(parameter); }));
}

BOOL CThread::Start(Work work)
{
    if (!work)
        return FALSE;
    if (IsRunning())
        return TRUE;
    // A completed thread must still be joined, including after startup failure.
    if (m_thread.joinable())
        m_thread.join();

    try
    {
        // Publish the token and loop flag before the worker can execute.
        m_stopSource = std::stop_source{};
        m_failed.store(false);
        m_bstart.store(true);
        m_running.store(true);
        m_thread = std::jthread([this, work = std::move(work)](std::stop_token token) mutable
        {
            std::stop_callback stopped(token, [this] { RequestStop(); });
            try
            {
                work(m_stopSource.get_token());
            }
            catch (const std::exception& error)
            {
                std::fprintf(stderr, "Worker thread failed: %s\n", error.what());
                m_failed.store(true);
            }
            catch (...)
            {
                std::fputs("Worker thread failed with an unknown exception\n", stderr);
                m_failed.store(true);
            }
            m_bstart.store(false);
            m_running.store(false);
        });
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "Cannot start worker thread: %s\n", error.what());
        m_bstart.store(false);
        m_running.store(false);
        m_failed.store(true);
        return FALSE;
    }
    return TRUE;
}

void CThread::RequestStop() noexcept
{
    m_bstart.store(false);
    m_stopSource.request_stop();
}

bool CThread::IsRunning() const noexcept
{
    return m_running.load();
}

bool CThread::HasFailed() const noexcept
{
    return m_failed.load();
}

bool CThread::WaitForStop(std::chrono::milliseconds delay)
{
    const auto token = m_stopSource.get_token();
    std::unique_lock lock(m_waitMutex);
    m_stopWake.wait_for(lock, token, delay, [] { return false; });
    return token.stop_requested();
}

void CThread::Wait()
{
    RequestStop();
    if (m_thread.joinable())
        m_thread.join();
}

void CThread::Terminate()
{
    Wait();
}

THREAD_ID CThread::GetThreadID()
{
#if defined(WIN32) || defined(WIN64)
    return ::GetCurrentThreadId();
#else
    return pthread_self();
#endif
}
