#include "std_type.h"
#include "process_error.h"
#include "circle_list.h"
#include "mutex.h"
#include "thread.h"
#include "free_stack.h"
#include "worker_queue.h"

#include <array>
#include <algorithm>
#include <atomic>
#include <barrier>
#include <chrono>
#include <cstdio>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

namespace
{
int checks = 0;
int failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}

class LoopThread final : public CThread
{
public:
    ~LoopThread() override { Wait(); }
    std::atomic<unsigned> entered{0};
private:
    void ThrdProc() override
    {
        if (m_bstart)
            ++entered;
        while (m_bstart)
            WaitForStop(30s);
    }
};

void test_threads()
{
    DWORD handlesBefore = 0;
    GetProcessHandleCount(GetCurrentProcess(), &handlesBefore);
    {
        LoopThread thread;
        for (unsigned iteration = 0; iteration < 200; ++iteration)
        {
            check(thread.Start(), "start legacy virtual worker");
            const auto deadline = std::chrono::steady_clock::now() + 2s;
            while (thread.entered.load() <= iteration && std::chrono::steady_clock::now() < deadline)
                std::this_thread::yield();
            check(thread.entered.load() == iteration + 1, "worker observes running flag on entry");
            check(thread.Start(), "start of active worker is idempotent");
            const auto before = std::chrono::steady_clock::now();
            thread.Wait();
            check(!thread.IsRunning() && std::chrono::steady_clock::now() - before < 2s,
                  "stop interrupts a long idle wait and joins");
            thread.Wait();
        }
    }
    DWORD handlesAfter = 0;
    GetProcessHandleCount(GetCurrentProcess(), &handlesAfter);
    check(handlesAfter <= handlesBefore + 2, "restart/join cycles release native thread handles");

    CThread worker;
    std::atomic<bool> destroyed{false};
    struct Owned
    {
        std::atomic<bool>& destroyed;
        ~Owned() { destroyed.store(true); }
    };
    auto owned = std::make_unique<Owned>(destroyed);
    check(worker.Start(CThread::Work([value = std::move(owned), &worker](std::stop_token token)
    {
        while (!token.stop_requested())
            worker.WaitForStop(30s);
    })), "start move-only worker with stop token");
    worker.Wait();
    check(destroyed.load(), "join releases move-only captured resources");
    check(worker.Start(CThread::Work([](std::stop_token) { throw std::runtime_error("expected worker failure"); })),
          "launch throwing worker");
    worker.Wait();
    check(worker.HasFailed() && !worker.IsRunning(), "worker failure is observable after join");
    check(worker.Start(CThread::Work([](std::stop_token) {})), "restart after worker failure");
    worker.Wait();
    check(!worker.HasFailed(), "restart clears previous failure");
    check(!worker.Start(static_cast<PFThrdProc>(nullptr), nullptr), "reject null callback");

    std::atomic<bool> stopped{false};
    {
        CThread scoped;
        check(scoped.Start(CThread::Work([&](std::stop_token token)
        {
            while (!token.stop_requested())
                std::this_thread::yield();
            stopped.store(true);
        })), "start worker owned by scope");
    }
    check(stopped.load(), "thread destructor cooperatively stops and joins");
}

void test_completion_backpressure()
{
    CCircleList output;
    check(output.Init(2), "initialize a one-record completion queue");
    int first = 1, second = 2;
    for (const bool cancel : {false, true})
    {
        check(output.Push(&first), "fill the completion queue");
        CThread worker;
        std::atomic<bool> entered{false}, finished{false}, published{false};
        check(worker.Start(CThread::Work([&](std::stop_token)
        {
            entered = true;
            published = cqyh::PublishUntilStopped(output, &second, worker);
            finished = true;
        })), "start a worker with a completed record");
        const auto deadline = std::chrono::steady_clock::now() + 2s;
        while (!entered && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        check(entered && !finished, "full queue retains the pending completion");
        if (cancel)
        {
            worker.Wait();
            check(finished && !published && output.Pop() == &first && output.IsEmpty(),
                  "stop interrupts backpressure and retains caller ownership");
        }
        else
        {
            check(output.Pop() == &first, "consumer makes room for the next completion");
            while (!finished && std::chrono::steady_clock::now() < deadline)
                std::this_thread::yield();
            worker.Wait();
            check(finished && published && output.Pop() == &second && output.IsEmpty(),
                  "completion is published exactly once when space becomes available");
        }
    }
    output.UnInit();
}

void test_queue()
{
    CCircleList queue;
    int first = 1, second = 2, third = 3;
    check(queue.IsEmpty() && !queue.Push(&first) && !queue.Pop(), "uninitialized queue is empty");
    check(!queue.Init(0), "reject zero slots");
    check(queue.Init(3), "initialize bounded queue");
    check(queue.Push(&first) && queue.Push(&second) && !queue.Push(&third), "reserve one slot at capacity");
    check(!queue.Init(5) && queue.Check() == &first, "reinitialization preserves outstanding pointers");
    check(queue.Pop() == &first && queue.Push(&third), "reuse a consumed slot");
    check(queue.Pop() == &second && queue.Pop() == &third && queue.IsEmpty(), "wrapped queue retains FIFO order");
    queue.UnInit();
    check(queue.IsEmpty() && !queue.Check() && !queue.Pop(), "shutdown resets indices and storage");

    // Tiny rings force many full/empty transitions while checking publication
    // of non-atomic payloads and FIFO delivery through reused queue slots.
    for (const UINT32 slots : {2u, 3u, 17u, 1024u})
    {
        check(queue.Init(slots), "initialize queue for concurrent transfer");
        constexpr std::size_t count = 200000;
        struct Payload { std::size_t sequence; std::array<std::size_t, 8> values; };
        std::vector<Payload> payloads(count);
        std::barrier start(2);
        std::atomic<bool> valid{true};
        std::jthread consumer([&]
        {
            start.arrive_and_wait();
            for (std::size_t expected = 0; expected < count; ++expected)
            {
                Payload* value;
                while (!(value = static_cast<Payload*>(queue.Check())))
                    std::this_thread::yield();
                if (value->sequence != expected)
                    valid.store(false);
                for (auto number : value->values)
                    if (number != expected * 7 + 3)
                        valid.store(false);
                if (queue.Pop() != value)
                    valid.store(false);
            }
        });
        start.arrive_and_wait();
        for (std::size_t sequence = 0; sequence < count; ++sequence)
        {
            payloads[sequence].sequence = sequence;
            payloads[sequence].values.fill(sequence * 7 + 3);
            while (!queue.Push(&payloads[sequence]))
                std::this_thread::yield();
        }
        consumer.join();
        check(valid.load() && queue.IsEmpty(), "cross-thread payload visibility and FIFO order across wraps");
        queue.UnInit();
    }
}

void test_mutex()
{
    CMutex mutex;
    try
    {
        std::scoped_lock first(mutex);
        std::scoped_lock second(mutex);
        throw std::runtime_error("expected scope unwind");
    }
    catch (const std::runtime_error&) {}
    bool acquired = false;
    std::jthread next([&] { std::scoped_lock lock(mutex); acquired = true; });
    next.join();
    check(acquired, "recursive scoped locks release on exception");
}

void test_free_stack()
{
    static_assert(sizeof(STACK_NODE) == 2 * sizeof(void*));
    static_assert(offsetof(STACK_NODE, pData) == sizeof(void*));
    CFreeStack stack;
    STACK_NODE first{}, second{};
    check(stack.Init() && !stack.Pop(), "free stack initializes empty");
    stack.Push(&first);
    stack.Push(&second);
    check(!stack.Init(), "reinitialization preserves outstanding free nodes");
    check(stack.Pop() == &second && stack.Pop() == &first && !stack.Pop(),
          "free stack returns exact full-width node pointers in LIFO order");
    stack.UnInit();

    struct Payload
    {
        STACK_NODE node{};
        std::atomic<bool> inUse{false};
        std::size_t visits = 0;
    };
    std::array<Payload, 128> storage;
    for (auto& value : storage)
    {
        value.node.pData = &value;
        stack.Push(&value.node);
    }
    std::atomic<bool> valid{true};
    constexpr int workers = 8, rounds = 50000;
    std::barrier start(workers);
    std::vector<std::jthread> threads;
    for (int index = 0; index < workers; ++index)
        threads.emplace_back([&]
        {
            start.arrive_and_wait();
            for (int round = 0; round < rounds; ++round)
            {
                STACK_NODE* node;
                while (!(node = stack.Pop()))
                    std::this_thread::yield();
                auto& value = *static_cast<Payload*>(node->pData);
                if (value.inUse.exchange(true))
                {
                    valid.store(false);
                    return;
                }
                ++value.visits; // Only the current node owner may access this.
                value.inUse.store(false);
                stack.Push(node);
            }
        });
    threads.clear();
    std::size_t visits = 0;
    for (auto& value : storage)
        visits += value.visits;
    check(valid.load() && visits == workers * rounds, "multiple workers exclusively reuse stable free nodes");
    std::vector<STACK_NODE*> returned;
    while (auto* node = stack.Pop())
        returned.push_back(node);
    std::sort(returned.begin(), returned.end(), std::less<STACK_NODE*>{});
    check(returned.size() == storage.size() && std::adjacent_find(returned.begin(), returned.end()) == returned.end(),
          "concurrent free stack neither loses nor duplicates nodes");
    check(stack.UnInit(), "free stack closes after all nodes are drained");
}

std::mutex sinkMutex;
std::vector<std::string> messages;
CLog* recursiveLogger = nullptr;
void sink(INT32, const char* message, INT32 size)
{
    {
        std::scoped_lock lock(sinkMutex);
        messages.emplace_back(message, size);
    }
    static thread_local bool nested = false;
    if (recursiveLogger && !nested)
    {
        nested = true;
        recursiveLogger->InfLog("nested log");
        nested = false;
    }
}

void throwing_sink(INT32, const char*, INT32)
{
    throw std::runtime_error("expected log sink failure");
}

BOOL modulo_guard(int& evaluations)
{
    LOG_PROCESS_ERROR(++evaluations % 2 == 0);
    return TRUE;
Exit0:
    return FALSE;
}

int modulo_result_guard(int& evaluations)
{
    int nResult = 0;
    LOG_PROCESS_ERROR_RET_CODE(++evaluations % 2 == 0, 17);
Exit0:
    return nResult;
}

void test_diagnostic_conditions()
{
    auto& logger = CLog::GetInstance();
    logger.SetOutputMask(lotCustom);
    logger.SetCustomOutput(sink);
    int evaluations = 0;
    check(!modulo_guard(evaluations) && evaluations == 1, "logged modulo guard evaluates once and exits");
    LOG_CHECK_ERROR(3 % 2 == 0);
    evaluations = 0;
    check(modulo_result_guard(evaluations) == 17 && evaluations == 1, "logged modulo guard retains its result code");
    check(messages.size() == 3 && messages[0].find("++evaluations % 2 == 0") != std::string::npos &&
          messages[1].find("3 % 2 == 0") != std::string::npos &&
          messages[2].find("++evaluations % 2 == 0") != std::string::npos,
          "diagnostic condition text is data, including percent characters");
    messages.clear();
    logger.SetOutputMask(lotNone);
    logger.SetCustomOutput(nullptr);
}

void test_logging()
{
    CLog logger;
    logger.SetOutputMask(lotCustom);
    logger.SetCustomOutput(sink);
    recursiveLogger = &logger;
    check(logger.InfLog("outer log"), "synchronous sink may log recursively");
    recursiveLogger = nullptr;
    check(messages.size() == 2 && messages[1].find("nested log") != std::string::npos,
          "recursive sink delivers both messages");
    messages.clear();
    check(logger.InfLog("%s", std::string(10000, 'x').c_str()), "long message is safely truncated");
    check(messages.size() == 1 && messages[0].size() <= 4095 && messages[0].ends_with("\r\n"),
          "truncation retains CRLF within the buffer");
    messages.clear();
    logger.SetCustomOutput(throwing_sink);
    bool caught = false;
    try { logger.InfLog("throwing sink"); }
    catch (const std::runtime_error&) { caught = true; }
    logger.SetCustomOutput(sink);
    std::jthread next([&] { logger.InfLog("after sink failure"); });
    next.join();
    check(caught && messages.size() == 1, "sink exception releases the output lock for other threads");
    messages.clear();
    for (int generation = 0; generation < 3; ++generation)
    {
        check(logger.Init("concurrency_test", lltAll, lotCustom, TRUE, FALSE, FALSE), "initialize asynchronous logger");
        logger.SetCustomOutput(sink);
        constexpr int writers = 8, perWriter = 1000;
        std::vector<std::jthread> producers;
        for (int writer = 0; writer < writers; ++writer)
            producers.emplace_back([&, writer]
            {
                for (int sequence = 0; sequence < perWriter; ++sequence)
                    logger.InfLog("writer %d sequence %d", writer, sequence);
            });
        // The controlling thread is reused across Init/UnInit, exercising its TLS slot.
        check(logger.InfLog("generation %d", generation), "controlling thread submits a message");
        producers.clear();
        logger.UnInit();
        check(messages.size() == writers * perWriter + 1, "shutdown drains every accepted message");
        messages.clear();
    }
    CLog other;
    check(logger.Init("first_logger", lltAll, lotCustom, TRUE, FALSE, FALSE) &&
          other.Init("second_logger", lltAll, lotCustom, TRUE, FALSE, FALSE),
          "initialize two independent asynchronous loggers");
    logger.SetCustomOutput(sink);
    other.SetCustomOutput(sink);
    bool accepted = true;
    for (int index = 0; index < 100; ++index)
        accepted = logger.InfLog("first %d", index) && other.InfLog("second %d", index) && accepted;
    logger.UnInit();
    other.UnInit();
    check(accepted && messages.size() == 200, "alternating loggers reuse their own producer slots");
    messages.clear();
}
}

int main()
{
    test_threads();
    test_completion_backpressure();
    test_queue();
    test_mutex();
    test_free_stack();
    test_logging();
    test_diagnostic_conditions();
    std::printf("%d concurrency checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
