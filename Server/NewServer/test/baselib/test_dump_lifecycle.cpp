#include "std_type.h"
#include "process_error.h"
#include "dump.h"
#include "share_mem.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

// Link-time mapping fake: exercise the production worker without joining or
// replacing a dump host used by another running server.
namespace
{
struct LegacyProcess
{
    BOOL used;
    EXCEPTION_POINTERS* exception;
    UINT32 processId, threadId;
    BOOL dump;
    UINT64 lastDump, lastCheck;
};
struct LegacyMapping { UINT32 tag; LegacyProcess processes[64]; };
static_assert(sizeof(LegacyProcess) == 48 && sizeof(LegacyMapping) == 3080);
std::atomic<int> liveMappings{0}, opened{0}, closed{0}, remainingSlots{-1};
std::atomic<bool> failCreate{false}, failOpen{false}, badTag{false}, full{false};
int checks = 0, failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
BOOL allocate(MemPtr& out)
{
    auto* value = new LegacyMapping{};
    value->tag = badTag ? 0 : static_cast<UINT32>('dump');
    if (full)
        for (auto& slot : value->processes)
            slot.used = TRUE;
    out = {TRUE, reinterpret_cast<char*>(value), value, sizeof(LegacyMapping)};
    ++liveMappings;
    return TRUE;
}
bool waitOpened(int previous)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (opened.load() == previous && std::chrono::steady_clock::now() < deadline)
        std::this_thread::yield();
    return opened.load() > previous;
}
}

BOOL CreateSharedMemory(const char*, UINT64 size, MemPtr& out)
{
    if (failCreate || size != sizeof(LegacyMapping))
        return FALSE;
    return allocate(out);
}
BOOL OpenSharedMemory(const char*, MemPtr& out)
{
    ++opened;
    return failOpen ? FALSE : allocate(out);
}
void CloseSharedMemory(MemPtr& memory)
{
    auto* value = reinterpret_cast<LegacyMapping*>(memory.pSharedMem);
    int used = 0;
    for (const auto& slot : value->processes)
        used += slot.used != FALSE;
    remainingSlots = used;
    delete value;
    memory = {};
    --liveMappings;
    ++closed;
}
BOOL IsShareMemoryExists(const char*) { return FALSE; }
void QuerySharedMemory(MemPtr&) {}

int main()
{
    CLog::GetInstance().SetOutputMask(lotNone);
    check(StopDumpThread(), "stop before start is harmless");
    check(!StartDumpThread(nullptr, TRUE, nullptr), "reject a null dump folder");
    check(!StartDumpThread(nullptr, TRUE, ""), "reject an empty dump folder");
    failCreate = true;
    check(!StartDumpThread(nullptr, TRUE, "unused"), "host startup reports mapping failure");
    check(liveMappings == 0, "failed startup releases all mapping resources");
    failCreate = false;
    DWORD before = 0;
    GetProcessHandleCount(GetCurrentProcess(), &before);
    for (int iteration = 0; iteration < 20; ++iteration)
    {
        check(StartDumpThread(nullptr, TRUE, "unused"), "start a dump host against isolated mapping fake");
        check(liveMappings == 1, "successful host startup publishes a live mapping before returning");
        check(!StartDumpThread(nullptr, TRUE, "unused"), "reject duplicate startup without replacing a live worker");
        SetDumpFolder("unused_updated");
        SetDumpOption(FALSE);
        const auto started = std::chrono::steady_clock::now();
        StopDumpThread();
        check(liveMappings == 0 && std::chrono::steady_clock::now() - started < std::chrono::seconds(2),
              "stop wakes and joins the dump worker before releasing the mapping");
        const int count = closed;
        StopDumpThread();
        check(closed == count, "repeated stop never closes a mapping twice");
    }
    DWORD after = 0;
    GetProcessHandleCount(GetCurrentProcess(), &after);
    check(after <= before + 2, "dump worker restart cycles release native thread handles");
    for (int scenario = 0; scenario < 4; ++scenario)
    {
        failOpen = scenario == 1;
        badTag = scenario == 2;
        full = scenario == 3;
        const int count = opened;
        check(StartDumpThread(nullptr, FALSE, "unused"), "client startup allows an unavailable dump host");
        check(waitOpened(count), "client attempts to attach to the mapping");
        StopDumpThread();
        check(liveMappings == 0, "client stop releases accepted or rejected mapping views");
        if (scenario == 0)
            check(remainingSlots == 0, "client releases its claimed process slot before unmapping");
    }
    std::printf("%d dump lifecycle checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
