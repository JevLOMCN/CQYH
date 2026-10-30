#pragma once

#include "analysis_records.h"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace log_analysis
{
// Lifecycle calls are serialized by the owner. Submission/result waits may overlap Stop().
class Workers
{
public:
    using Pointer = std::unique_ptr<Task>;
    using Processor = void (*)(Task&, std::stop_token);
    Workers() = default;
    ~Workers();
    Workers(const Workers&) = delete;
    Workers& operator=(const Workers&) = delete;

    bool Start(std::size_t count = 32, Processor processor = Process) noexcept;
    void Stop() noexcept;
    bool Submit(Pointer task) noexcept;
    Pointer WaitResult();
    std::size_t Pending() const;

private:
    struct State;
    static void Run(const std::shared_ptr<State>& state, std::stop_token stop);
    std::atomic<std::shared_ptr<State>> m_state;
    std::vector<std::jthread> m_threads;
};
}
