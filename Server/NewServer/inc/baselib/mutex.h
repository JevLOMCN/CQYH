#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <mutex>

// Preserve CRITICAL_SECTION's recursive behavior: a custom log sink may log again.
class CMutex
{
public:
    CMutex() = default;
    ~CMutex() = default;
    CMutex(const CMutex&) = delete;
    CMutex& operator=(const CMutex&) = delete;

    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }
    void Lock() { lock(); }
    void Unlock() { unlock(); }

private:
    std::recursive_mutex m_mutex;
};

#endif // _MUTEX_H_
