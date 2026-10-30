#include "analysis_records.h"
#include "iocp_mgr.h"

#include <windows.h>
#include <crtdbg.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <future>
#include <iostream>
#include <latch>
#include <new>
#include <set>
#include <stdexcept>
#include <string>

// Fail allocations on the selected calling thread, including after workers have started.
thread_local int allocation_countdown = -1;
void* operator new(std::size_t size)
{
    if (allocation_countdown == 0)
    {
        allocation_countdown = -1;
        throw std::bad_alloc();
    }
    if (allocation_countdown > 0)
        --allocation_countdown;
    if (auto* memory = std::malloc(size == 0 ? 1 : size))
        return memory;
    throw std::bad_alloc();
}
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }

namespace
{
using namespace log_analysis;
using namespace std::chrono_literals;
int checks = 0;
void trace(const char* stage)
{
    if (std::getenv("CQYH_TRACE_WORKERS"))
        std::fprintf(stderr, "%s\n", stage);
}
void require(bool condition, const char* message)
{
    ++checks;
    if (!condition)
        throw std::runtime_error(message);
}

void write_file(const char* name, std::string_view text)
{
    std::ofstream file(name, std::ios::binary);
    file.write(text.data(), static_cast<std::streamsize>(text.size()));
    file.close();
    require(bool(file), "write log fixture");
}

void records()
{
    const std::string prefix = "2020-01-01 12:00:00.000\t";
    const std::string long_message(10000, 'x');
    for (const std::string ending : {"\r\n", "\n", ""})
    {
        write_file("errors.log", prefix + "[E] [00000001] message\r\n" + prefix + "[C] message\n" +
            prefix + "[I] ignored\n" + prefix + "[E] " + long_message + ending);
        Task task;
        task.path = "errors.log";
        Process(task, {});
        require(task.errors == ErrorCounts{{"message", 2}, {long_message, 1}}, "error grouping and line endings");
    }
    write_file("short.log", "[\r\nshort\n2020\t[E\n");
    Task task;
    task.path = "short.log";
    Process(task, {});
    require(task.errors.empty(), "skip incomplete prefixes without reading beyond the line");
    write_file("empty.log", "");
    task.path = "empty.log";
    Process(task, {});
    require(task.errors.empty(), "empty file");
    std::stop_source canceled;
    canceled.request_stop();
    bool caught = false;
    try { Process(task, canceled.get_token()); }
    catch (const std::runtime_error&) { caught = true; }
    require(caught, "empty-file cancellation");
    write_file("nul.log", std::string("bad\0text", 8));
    task.path = "nul.log";
    caught = false;
    try { Process(task, {}); }
    catch (const std::runtime_error&) { caught = true; }
    require(caught, "reject embedded NUL");
}

void fake_process(Task& task, std::stop_token)
{
    if (task.path == "throw")
        throw std::runtime_error("processor failed");
    task.output = "done";
    if (task.path == "no-alloc-publication")
        allocation_countdown = 0;
}

void queues()
{
    Workers workers;
    require(!workers.Start(0) && !workers.Start(257) && !workers.Start(1, nullptr), "invalid worker options");
    require(workers.Start(4, fake_process) && !workers.Start(), "start only once");
    require(!workers.Submit(nullptr), "reject null task");
    std::vector<std::jthread> producers;
    std::atomic<bool> accepted{true};
    for (int producer = 0; producer < 4; ++producer)
    {
        producers.emplace_back([&, producer]
        {
            for (int i = 0; i < 200; ++i)
            {
                auto task = std::make_unique<Task>();
                task->path = std::to_string(producer) + "-" + std::to_string(i);
                if (!workers.Submit(std::move(task)))
                    accepted = false;
            }
        });
    }
    producers.clear();
    require(accepted && workers.Pending() == 800, "owned multi-producer submission");
    std::set<std::string> seen;
    for (int i = 0; i < 800; ++i)
    {
        auto task = workers.WaitResult();
        require(task && !task->failed && task->output == "done" && seen.insert(task->path.string()).second,
            "exactly one completed result per submitted task");
    }
    require(workers.Pending() == 0 && !workers.WaitResult(), "drain completed results");
    for (const char* name : {"throw", "no-alloc-publication"})
    {
        trace(name);
        auto task = std::make_unique<Task>();
        task->path = name;
        require(workers.Submit(std::move(task)), "submit failure/publication fixture");
        task = workers.WaitResult();
        require(task && (task->failed == (std::string_view(name) == "throw")), "worker error is observable, publication needs no allocation");
        if (task->failed)
            require(task->diagnostic == "processor failed", "retain processor error diagnostic");
    }
    workers.Stop();
    trace("publication stopped");
    require(!workers.Submit(std::make_unique<Task>()) && !workers.WaitResult(), "reject work after stop");
    require(workers.Start(2) && workers.Submit(std::make_unique<Task>(Task{.path = "absent.log"})), "restart workers");
    const auto missing = workers.WaitResult();
    require(missing && missing->failed && !missing->diagnostic.empty(), "missing file cannot strand a task");
    workers.Stop();
}

std::latch* entered = nullptr;
void wait_for_stop(Task&, std::stop_token stop)
{
    entered->count_down();
    std::mutex mutex;
    std::condition_variable_any ready;
    std::unique_lock lock(mutex);
    ready.wait(lock, stop, [] { return false; });
}

void shutdown()
{
    Workers workers;
    std::latch running(1);
    entered = &running;
    require(workers.Start(1, wait_for_stop), "start blocking processor");
    for (int i = 0; i < 200; ++i)
        require(workers.Submit(std::make_unique<Task>()), "queue pending work");
    running.wait();
    std::promise<bool> done;
    auto result = done.get_future();
    std::jthread consumer([&] { done.set_value(bool(workers.WaitResult())); });
    require(result.wait_for(20ms) == std::future_status::timeout, "result consumer waits for active work");
    const auto start = std::chrono::steady_clock::now();
    workers.Stop();
    require(std::chrono::steady_clock::now() - start < 2s, "stop cancels pending work and joins active worker");
    require(result.wait_for(1s) == std::future_status::ready && workers.Pending() == 0, "stop wakes result waiters");
    consumer.join();
    workers.Stop();
}

void startup_failures()
{
    DWORD before = 0, after = 0;
    require(GetProcessHandleCount(GetCurrentProcess(), &before), "initial handle count");
    int failures = 0, successes = 0;
    for (int failure = 0; failure < 48; ++failure)
    {
        if (std::getenv("CQYH_TRACE_WORKERS"))
            std::fprintf(stderr, "startup failure index %d\n", failure);
        Workers workers;
        allocation_countdown = failure;
        const bool started = workers.Start(4, fake_process);
        allocation_countdown = -1;
        if (started)
            ++successes;
        else
            ++failures;
        workers.Stop();
        require(workers.Pending() == 0, "startup rollback resets state");
    }
    require(failures > 8 && successes > 0, "exercise allocation failures through partial worker startup");
    require(GetProcessHandleCount(GetCurrentProcess(), &after) && before == after, "startup failures release thread handles");
    std::cout << failures << " startup failure points, " << successes << " successful starts\n";
}
}

int main(int argc, char* argv[])
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    try
    {
        require(argc == 2, "expected work directory");
        std::filesystem::create_directories(argv[1]);
        std::filesystem::current_path(argv[1]);
        trace("records"); records();
        trace("queues"); queues();
        trace("shutdown"); shutdown();
        trace("startup failures"); startup_failures();
        std::cout << checks << " analysis parser/worker checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        allocation_countdown = -1;
        std::cerr << error.what() << '\n';
        return 1;
    }
}
