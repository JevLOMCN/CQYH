"""Actual worker records, live workers, startup failures and shutdown ownership."""
import re


def fixtures(client, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#include[^\n]*", "", text)

    parts = [r'''
#include <list>
#include <map>
#include <atomic>
#include <thread>
#include <memory>
#include <process.h>
#define ASSERT(value) assert(value)
#define TRACE(...) ((void)0)
namespace WorkerFault {
std::atomic<int> handles{0}, closed{0}, sections{0}, sections_closed{0};
int event_calls = 0, thread_calls = 0, fail_event = 0, fail_thread = 0;
bool fail_map = false;
void Reset() {
    handles = closed = sections = sections_closed = 0;
    event_calls = thread_calls = fail_event = fail_thread = 0;
    fail_map = false;
}
template<class T> struct MapAllocator : std::allocator<T> {
    template<class U> struct rebind { using other = MapAllocator<U>; };
    MapAllocator() = default;
    template<class U> MapAllocator(const MapAllocator<U>&) {}
    T* allocate(size_t count) {
        if (fail_map) throw std::bad_alloc();
        return std::allocator<T>::allocate(count);
    }
};
}
HANDLE WorkerCreateEvent(void* attributes, BOOL manual, BOOL initial, LPCTSTR name) {
    if (++WorkerFault::event_calls == WorkerFault::fail_event) return nullptr;
    HANDLE handle = ::CreateEventW(static_cast<SECURITY_ATTRIBUTES*>(attributes), manual, initial, name);
    if (handle) ++WorkerFault::handles;
    return handle;
}
uintptr_t WorkerBeginThread(void* attributes, unsigned stack, unsigned (__stdcall* entry)(void*),
                            void* data, unsigned flags, unsigned* id) {
    if (++WorkerFault::thread_calls == WorkerFault::fail_thread) return 0;
    auto handle = ::_beginthreadex(attributes, stack, entry, data, flags, id);
    if (handle) ++WorkerFault::handles;
    return handle;
}
HANDLE WorkerCreateThread(SECURITY_ATTRIBUTES* attributes, SIZE_T stack, LPTHREAD_START_ROUTINE entry,
                           void* data, DWORD flags, DWORD* id) {
    if (++WorkerFault::thread_calls == WorkerFault::fail_thread) return nullptr;
    HANDLE handle = ::CreateThread(attributes, stack, entry, data, flags, id);
    if (handle) ++WorkerFault::handles;
    return handle;
}
BOOL WorkerCloseHandle(HANDLE handle) {
    assert(handle && handle != INVALID_HANDLE_VALUE);
    ++WorkerFault::closed; return ::CloseHandle(handle);
}
void WorkerInitializeSection(CRITICAL_SECTION* section) {
    ++WorkerFault::sections; ::InitializeCriticalSection(section);
}
void WorkerDeleteSection(CRITICAL_SECTION* section) {
    ++WorkerFault::sections_closed; ::DeleteCriticalSection(section);
}
''']
    for index, folder in enumerate(["FixClient", "PatchMPQ", "FilePatch", "vigenerator"]):
        path = client / "client/PatchTools" / folder
        parts.append("namespace Worker%d {\nusing CString = std::wstring;\n" % index)
        parts.append("#undef IRUN_OBJECT_DEFINED\n#undef THREAD_POOL_CLASS\n")
        if index % 2:
            parts.append("#define USE_WIN32API_THREAD\n")
        parts.append(clean(source(path / "RunObject.h")))
        header = clean(source(path / "ThreadPool.h")).replace("private:", "public:")
        header = header.replace("typedef map<DWORD, _ThreadData> ThreadMap;",
                                "typedef map<DWORD, _ThreadData, std::less<DWORD>, "
                                "WorkerFault::MapAllocator<std::pair<const DWORD, _ThreadData>>> ThreadMap;")
        parts.append(header)
        cpp = clean(source(path / "ThreadPool.cpp"))
        parts.append(r'''
#undef CreateEvent
#define CreateEvent WorkerCreateEvent
#define CreateThread WorkerCreateThread
#define _beginthreadex WorkerBeginThread
#define CloseHandle WorkerCloseHandle
#define InitializeCriticalSection WorkerInitializeSection
#define DeleteCriticalSection WorkerDeleteSection
''')
        parts.append(cpp)
        parts.append(r'''
#undef CreateEvent
#define CreateEvent CreateEventW
#undef CreateThread
#undef _beginthreadex
#undef CloseHandle
#undef InitializeCriticalSection
#undef DeleteCriticalSection
#undef USE_WIN32API_THREAD
''')
        parts.append(r'''
int deleted = 0;
struct Task : IRunObject {
    bool automatic;
    explicit Task(bool own) : automatic(own) {}
    bool AutoDelete() override { assert(pThreadPool); return automatic; }
    void Run() override {}
    ~Task() override { if (automatic) assert(pThreadPool); ++deleted; }
};
DWORD WINAPI Callback(void*) { return 0; }
void TestRecordsAndQueues() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(CThreadPool) unsigned char memory[sizeof(CThreadPool)];
        memset(memory, pattern, sizeof(memory));
        CThreadPool* pool = new(memory) CThreadPool(3, false, 100);
        assert(!pool->GetShutdownHandle() && pool->m_state == Destroyed);
        assert(pool->m_nPoolSize == 3 && pool->m_nWaitForThreadsToDieMS == 100);
        for (auto c : pool->m_szPoolName) assert(!c);
        pool->~CThreadPool();
        alignas(_FunctionData) unsigned char function_memory[sizeof(_FunctionData)];
        memset(function_memory, pattern, sizeof(function_memory));
        _FunctionData* f = new(function_memory) _FunctionData;
        assert(!f->lpStartAddress && !f->pData);
        alignas(_ThreadData) unsigned char thread_memory[sizeof(_ThreadData)];
        memset(thread_memory, pattern, sizeof(thread_memory));
        _ThreadData* t = new(thread_memory) _ThreadData;
        assert(!t->bFree && !t->hThread && !t->WaitHandle && !t->dwThreadId);
        alignas(UserPoolData) unsigned char user_memory[sizeof(UserPoolData)];
        memset(user_memory, pattern, sizeof(user_memory));
        UserPoolData* u = new(user_memory) UserPoolData;
        assert(!u->pData && !u->pThreadPool);
        alignas(Task) unsigned char task_memory[sizeof(Task)];
        memset(task_memory, pattern, sizeof(task_memory));
        Task* task = new(task_memory) Task(false);
        assert(!task->pThreadPool);
        task->~Task();
    }
    CThreadPool pool(1, false);
    pool.m_state = Ready;
    assert(!pool.Run((IRunObject*)nullptr));
    assert(!pool.Run((LPTHREAD_START_ROUTINE)nullptr, nullptr));
    assert(pool.m_functionList.empty());
    LPTHREAD_START_ROUTINE proc = Callback;
    void* data = reinterpret_cast<void*>(1);
    IRunObject* object = reinterpret_cast<IRunObject*>(1);
    assert(!pool.GetThreadProc(0, proc, &data, &object) && !proc && !data && !object);
    int caller_data = 7;
    assert(pool.Run(Callback, &caller_data));
    assert(!pool.GetThreadProc(0, proc, nullptr, &object));
    assert(pool.m_functionList.size() == 1);
    assert(pool.GetThreadProc(0, proc, &data, &object));
    assert(proc == Callback && data && !object);
    UserPoolData* wrapper = static_cast<UserPoolData*>(data);
    assert(wrapper->pData == &caller_data && wrapper->pThreadPool == &pool);
    delete wrapper;
    Task* owned = new Task(true);
    Task retained(false);
    deleted = 0;
    assert(pool.Run(owned) && pool.Run(&retained));
    assert(owned->pThreadPool == &pool && retained.pThreadPool == &pool);
    assert(pool.Run(Callback, &caller_data));
    pool.ReleaseMemory();
    assert(deleted == 1 && !retained.pThreadPool && caller_data == 7);
    assert(pool.m_functionList.empty());
    pool.ReleaseMemory();
    assert(deleted == 1);
    assert(pool.Run(&retained));
    assert(pool.GetThreadProc(0, proc, &data, &object));
    assert(!proc && !data && object == &retained);
    retained.pThreadPool = nullptr;
}
struct BlockingTask : IRunObject {
    HANDLE started, release;
    BlockingTask(HANDLE s, HANDLE r) : started(s), release(r) {}
    bool AutoDelete() override { return false; }
    void Run() override {
        SetEvent(started);
        assert(WaitForSingleObject(release, 10000) == WAIT_OBJECT_0);
        assert(pThreadPool->GetPoolSize() == 1);
        assert(!pThreadPool->Run(Callback, nullptr));
    }
};
struct OwnedTask : IRunObject {
    std::atomic<int>& destroyed;
    explicit OwnedTask(std::atomic<int>& count) : destroyed(count) {}
    bool AutoDelete() override { return true; }
    void Run() override {}
    ~OwnedTask() override { ++destroyed; }
};
struct CountData {
    std::atomic<int> completed{0};
    HANDLE finished;
    int expected;
};
DWORD WINAPI CountCallback(void* parameter) {
    auto* wrapper = static_cast<UserPoolData*>(parameter);
    auto* count = static_cast<CountData*>(wrapper->pData);
    if (++count->completed == count->expected) SetEvent(count->finished);
    return 0;
}
void TestRuntime() {
    for (int failure = 0; failure < 7; ++failure) {
        WorkerFault::Reset();
        {
            CThreadPool pool(3, false, 0);
            if (failure < 3) WorkerFault::fail_event = failure + 1;
            else if (failure < 6) WorkerFault::fail_thread = failure - 2;
            else WorkerFault::fail_map = true;
            assert(!pool.Create());
            assert(pool.GetState() == Destroyed && pool.m_threads.empty() && !pool.GetShutdownHandle());
            assert(WorkerFault::handles == WorkerFault::closed);
            WorkerFault::fail_event = WorkerFault::fail_thread = 0;
            WorkerFault::fail_map = false;
            assert(pool.Create());
            assert(pool.GetState() == Ready && pool.m_threads.size() == 3);
            pool.Destroy();
            assert(pool.GetState() == Destroyed && pool.m_threads.empty());
            assert(WorkerFault::handles == WorkerFault::closed);
        }
        assert(WorkerFault::sections == WorkerFault::sections_closed);
    }
    WorkerFault::Reset(); WorkerFault::fail_thread = 2;
    bool threw = false;
    try { CThreadPool failure(3, true, 0); } catch (...) { threw = true; }
    assert(threw && WorkerFault::handles == WorkerFault::closed);
    assert(WorkerFault::sections == WorkerFault::sections_closed);

    WorkerFault::Reset();
    {
        CThreadPool pool(1, true, 0);
        HANDLE started = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        HANDLE release = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        HANDLE first_done = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        HANDLE second_done = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        assert(started && release && first_done && second_done);
        BlockingTask running(started, release);
        Task retained(false);
        std::atomic<int> destroyed{0};
        assert(pool.Run(&running));
        assert(WaitForSingleObject(started, 5000) == WAIT_OBJECT_0);
        assert(pool.Run(new OwnedTask(destroyed)) && pool.Run(&retained));
        assert(!pool.IsAllTaskCompleted());
        std::thread first([&] { pool.Destroy(); SetEvent(first_done); });
        for (int wait = 0; pool.GetState() != Destroying && wait < 5000; ++wait) Sleep(1);
        assert(pool.GetState() == Destroying);
        assert(!pool.Run(&retained));
        std::thread second([&] { pool.Destroy(); SetEvent(second_done); });
        assert(WaitForSingleObject(first_done, 200) == WAIT_TIMEOUT);
        assert(WaitForSingleObject(second_done, 50) == WAIT_TIMEOUT);
        assert(!destroyed);
        SetEvent(release);
        assert(WaitForSingleObject(first_done, 5000) == WAIT_OBJECT_0);
        assert(WaitForSingleObject(second_done, 5000) == WAIT_OBJECT_0);
        first.join(); second.join();
        assert(destroyed == 1 && !running.pThreadPool && !retained.pThreadPool);
        assert(pool.GetState() == Destroyed && pool.IsAllTaskCompleted());
        ::CloseHandle(started); ::CloseHandle(release);
        ::CloseHandle(first_done); ::CloseHandle(second_done);
    }
    assert(WorkerFault::handles == WorkerFault::closed);
    assert(WorkerFault::sections == WorkerFault::sections_closed);

    WorkerFault::Reset();
    {
        CThreadPool pool(3);
        CountData data;
        data.finished = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        data.expected = 400;
        std::vector<std::thread> producers;
        for (int n = 0; n < 4; ++n) producers.emplace_back([&] {
            for (int i = 0; i < 100; ++i) assert(pool.Run(CountCallback, &data));
        });
        for (auto& producer : producers) producer.join();
        assert(WaitForSingleObject(data.finished, 5000) == WAIT_OBJECT_0);
        for (int wait = 0; !pool.IsAllTaskCompleted() && wait < 5000; ++wait) Sleep(1);
        assert(pool.IsAllTaskCompleted() && data.completed == data.expected);
        pool.Destroy();
        ::CloseHandle(data.finished);
    }
    assert(WorkerFault::handles == WorkerFault::closed);
    assert(WorkerFault::sections == WorkerFault::sections_closed);

    WorkerFault::Reset();
    {
        CThreadPool first(1), second(1);
        std::wstring second_name = second.m_szPoolName;
        first.Destroy();
        CThreadPool third(1);
        assert(second_name != third.m_szPoolName);
    }
    assert(WorkerFault::handles == WorkerFault::closed);
    assert(WorkerFault::sections == WorkerFault::sections_closed);
}
void Run() { TestRecordsAndQueues(); TestRuntime(); }
}
''')
    parts.append("void TestWorkers() { " + " ".join("Worker%d::Run();" % i for i in range(4)) +
                 ' puts("Worker initialization, startup failures, live-thread shutdown and queue regressions passed (four copies, CRT/Win32)"); }')
    return "\n".join(parts)
