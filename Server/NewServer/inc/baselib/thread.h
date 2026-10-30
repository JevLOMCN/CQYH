#ifndef _THREAD_H_
#define _THREAD_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>

#if (defined(WIN32) || defined(WIN64))
	typedef UINT32 THREAD_ID;
	typedef HANDLE  THREAD_HANDLE;
	typedef unsigned int (WINAPI *PFThrdProc)(void *);
	#define THREAD_DECLARE(x)  unsigned int WINAPI  x
	#define THREAD_RETURN(x)   return x;
#else
	typedef pthread_t THREAD_ID;
	typedef long    THREAD_HANDLE;
	typedef void *  (*PFThrdProc)(void *);
	#define THREAD_DECLARE(x)  void *  x
	#define THREAD_RETURN(x)   return x;
#endif // WIN32 | WIN64

class CThread
{
public:
	CThread(void);
	virtual ~CThread(void);

	BOOL Start(void);
	BOOL Start(PFThrdProc pFunc, void* pParam);
	using Work = std::move_only_function<void(std::stop_token)>;
	BOOL Start(Work work);
	void RequestStop() noexcept;
	bool IsRunning() const noexcept;
	bool HasFailed() const noexcept;
	bool WaitForStop(std::chrono::milliseconds delay);
	void Wait(void);
	// Compatibility entry point: termination now requests stop and joins.
	void Terminate(void);

	static THREAD_ID GetThreadID(void);

protected:
	virtual void ThrdProc(void) { }

    // Derived owners must Wait() before destroying members used by ThrdProc().
    std::atomic<bool> m_bstart{false};

private:
    CThread(const CThread&) = delete;
    CThread& operator=(const CThread&) = delete;

    // Start/Wait and resource lifetime belong to one controlling thread.
    // RequestStop, IsRunning and HasFailed may be used concurrently.
    std::stop_source m_stopSource;
    std::mutex m_waitMutex;
    std::condition_variable_any m_stopWake;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_failed{false};
    std::jthread m_thread;
};

#endif  // _THREAD_H_
