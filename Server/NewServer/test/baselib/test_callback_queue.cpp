#include "callback_queue.h"
#include "release_owner.h"

#include <array>
#include <atomic>
#include <cstdio>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
int checks = 0, failures = 0;
std::atomic<int> live{0};
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
struct Record
{
    int producer, sequence;
    std::function<void()> cleanup;
    Record(int producer, int sequence) : producer(producer), sequence(sequence) { ++live; }
    ~Record() { if (cleanup) cleanup(); --live; }
};
using Queue = cqyh::CallbackQueue<Record>;

void bounded_ownership()
{
    Queue queue;
    check(!queue.Push(std::make_unique<Record>(0, 0)) && live == 0, "uninitialized queue rejects and releases ownership");
    check(!queue.InitSlots(0) && queue.InitSlots(3), "legacy slot count initializes a bounded queue");
    check(queue.Push(std::make_unique<Record>(0, 1)) && queue.Push(std::make_unique<Record>(0, 2)), "accept two usable slots");
    check(!queue.Push(std::make_unique<Record>(0, 3)) && live == 2, "full queue releases rejected record");
    check(!queue.InitSlots(9), "reinitialization preserves outstanding records");
    {
        auto first = queue.Pop();
        auto second = queue.Pop();
        check(first && second && first->sequence == 1 && second->sequence == 2 && !queue.Pop(), "owned records retain FIFO order");
    }
    check(live == 0, "consumer releases each record once");
    queue.Push(std::make_unique<Record>(0, 4));
    queue.Close();
    check(!queue.Push(std::make_unique<Record>(0, 5)) && live == 1, "close rejects new records while retaining accepted ownership");
    queue.UnInit();
    check(!queue.IsInitialized() && !queue.Pop() && live == 0, "shutdown releases all abandoned records");
    check(queue.InitSlots(1) && !queue.Push(std::make_unique<Record>(0, 0)), "one legacy slot has zero usable capacity");
    queue.UnInit();
    check(queue.InitSlots(3) && queue.Push(std::make_unique<Record>(0, 6)), "queue restarts after shutdown");
    bool reentered = false;
    auto record = std::make_unique<Record>(0, 7);
    record->cleanup = [&] { reentered = !queue.Pop() && !queue.IsInitialized(); };
    queue.Push(std::move(record));
    queue.UnInit();
    check(reentered && live == 0, "abandoned-record destructors can reenter without holding the queue lock");
}

void multiple_producers()
{
    constexpr int writers = 8, perWriter = 10000;
    Queue queue;
    check(queue.InitSlots(writers * perWriter + 1), "initialize concurrent callback fixture");
    std::atomic<bool> complete{false}, valid{true};
    int received = 0;
    std::array<int, writers> next{};
    std::jthread consumer([&]
    {
        for (;;)
        {
            auto record = queue.Pop();
            if (!record && complete.load())
            {
                // The first empty scan may precede a producer's final publish.
                record = queue.Pop();
                if (!record)
                    break;
            }
            if (record)
            {
                if (record->producer < 0 || record->producer >= writers ||
                    record->sequence != next[record->producer]++)
                    valid = false;
                ++received;
            }
            else
                std::this_thread::yield();
        }
    });
    std::vector<std::jthread> producers;
    for (int producer = 0; producer < writers; ++producer)
        producers.emplace_back([&, producer]
        {
            for (int sequence = 0; sequence < perWriter; ++sequence)
                if (!queue.Push(std::make_unique<Record>(producer, sequence)))
                    valid = false;
        });
    producers.clear();
    complete = true;
    consumer.join();
    check(valid && received == writers * perWriter, "multiple producers preserve every record and each producer's order");
    check(live == 0, "concurrent consumption releases all ownership");
}

void close_while_producing()
{
    Queue queue;
    queue.InitSlots(1025);
    std::atomic<bool> stop{false}, closed{false}, valid{true};
    std::atomic<int> attempted{0};
    std::vector<std::jthread> producers;
    for (int producer = 0; producer < 4; ++producer)
        producers.emplace_back([&, producer]
        {
            int sequence = 0;
            while (!stop.load())
            {
                const bool alreadyClosed = closed.load();
                const bool accepted = queue.Push(std::make_unique<Record>(producer, sequence++));
                if (alreadyClosed && accepted)
                    valid = false;
                ++attempted;
            }
        });
    while (attempted < 2000)
        std::this_thread::yield();
    queue.Close();
    closed = true;
    const int before = attempted;
    while (attempted < before + 2000)
        std::this_thread::yield();
    stop = true;
    producers.clear();
    queue.UnInit();
    check(valid && live == 0, "close and drain safely reject active producers without leaks");
}

struct FakeSdk
{
    Queue& queue;
    int& releases;
    bool& rejected;
    bool throwOnStop = false;
    bool Initialize() { return queue.Push(std::make_unique<Record>(0, 0)); }
    void Uninitialize()
    {
        rejected = !queue.Push(std::make_unique<Record>(0, 1));
        if (throwOnStop)
            throw std::runtime_error("expected SDK stop failure");
    }
    void Release() { ++releases; delete this; }
};
void sdk_lifecycle()
{
    for (const bool failStop : {false, true})
    {
        Queue queue;
        queue.InitSlots(3);
        int releases = 0;
        bool rejected = false;
        cqyh::ReleasePtr<FakeSdk> sdk(new FakeSdk{queue, releases, rejected, failStop});
        check(sdk->Initialize(), "queue is ready for a synchronous initialization callback");
        queue.Close();
        check(cqyh::StopAndRelease(sdk) == !failStop && !sdk && releases == 1 && rejected,
              "SDK shutdown rejects final callbacks and releases even when stop throws");
        queue.UnInit();
        check(live == 0, "SDK shutdown precedes pending callback destruction");
        check(cqyh::StopAndRelease(sdk), "repeated SDK shutdown is harmless");
    }
}

void control_events()
{
    Queue queue;
    check(queue.IsEmpty() && queue.Init(), "initialize an empty control-event queue");
    bool accepted = true;
    for (int sequence = 0; sequence < 100000; ++sequence)
        accepted = queue.Push(std::make_unique<Record>(0, sequence)) && accepted;
    check(accepted && !queue.IsEmpty(), "control events are retained without a fixed ring limit");
    bool ordered = true;
    queue.Close();
    for (int sequence = 0; sequence < 100000; ++sequence)
    {
        auto record = queue.Pop();
        ordered = record && record->sequence == sequence && ordered;
    }
    check(ordered && queue.IsEmpty() && live == 0, "closed control queue drains all events in order");
    queue.UnInit();
}
}

int main()
{
    bounded_ownership();
    multiple_producers();
    close_while_producing();
    sdk_lifecycle();
    control_events();
    std::printf("%d callback queue checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
