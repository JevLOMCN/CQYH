#include <winsock2.h>
#include "std_type.h"
#include "process_error.h"
#include "share_mem_io.h"
#include "cas.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>

using namespace std::chrono_literals;
namespace
{
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
std::string name()
{
    static unsigned sequence = 0;
    return "CQYH_shared_test_" + std::to_string(GetCurrentProcessId()) + "_" +
           std::to_string(GetTickCount64()) + "_" + std::to_string(++sequence);
}
struct RawMapping
{
    MemPtr value{};
    ~RawMapping() { if (value.pSharedMem) CloseSharedMemory(value); }
};
struct CloseHandleDeleter { void operator()(void* value) const { CloseHandle(value); } };
struct UnmapDeleter { void operator()(void* value) const { UnmapViewOfFile(value); } };
using Handle = std::unique_ptr<void, CloseHandleDeleter>;
using View = std::unique_ptr<void, UnmapDeleter>;

void boundaries()
{
    static_assert(sizeof(MemHeader) == 16 && offsetof(MemHeader, nWriteOffset) == 12);
    static_assert(sizeof(ShareMemHeader) == 12 && offsetof(ShareMemHeader, qwSize) == 4);
    static_assert(!std::is_copy_constructible_v<CShareMemBuffer>);
    CShareMemBuffer writer, reader;
    std::array<char, 8> out{};
    check(!writer.Write("x", 1) && !reader.Read(out.data(), 1), "uninitialized views reject I/O");
    writer.Clear();
    writer.Destroy();
    const auto id = name();
    check(!writer.Create(id.c_str(), 1, 0) && !writer.Create(id.c_str(), 8, -1) &&
          !writer.Create(id.c_str(), 8, 1), "reject undersized rings and invalid native word alignment");
    check(writer.Create(id.c_str(), 8, 0) && reader.Open(id.c_str()), "Open attaches an existing ring");
    check(writer.Write(nullptr, 0) && reader.Read(nullptr, 0), "zero-byte operations need no buffer");
    check(!writer.Write(nullptr, 1) && !reader.Read(nullptr, 1) &&
          !writer.Write("x", -1) && !reader.Read(out.data(), -1), "reject null and negative byte ranges");
    check(writer.Write("abcdef", 6) && reader.Read(out.data(), 4), "prepare wrapped ring");
    check(writer.Write("ghijk", 5), "fill all seven usable bytes across the wrap");
    check(!writer.Write("x", 1), "full wrapped ring rejects an overwrite");
    check(reader.Read(out.data(), 7) && std::memcmp(out.data(), "efghijk", 7) == 0,
          "read consumes the final complete wrapped payload");
    out.fill('#');
    check(!reader.Read(out.data(), 1) && out[0] == '#', "failed read leaves the destination untouched");
    check(writer.Write("ok", 2), "queue data before rejected reinitialization");
    check(!writer.Create(name().c_str(), 16, 0) && !reader.Open(name().c_str()), "reject replacing active mappings");
    check(reader.Read(out.data(), 2) && out[0] == 'o' && out[1] == 'k', "rejected initialization preserves old data");
    RawMapping raw;
    check(OpenSharedMemory(id.c_str(), raw.value), "open raw view for legacy counter compatibility");
    auto* header = reinterpret_cast<MemHeader*>(raw.value.pSharedMem);
    cqyh::SharedStore32(&header->nReadOffset, 8);
    cqyh::SharedStore32(&header->nWriteOffset, 0);
    check(writer.Write("end", 3) && reader.Read(out.data(), 3) && std::memcmp(out.data(), "end", 3) == 0,
          "normalize a legacy end-of-block offset");
    cqyh::SharedStore32(&header->nReadOffset, -1);
    check(!writer.Write("x", 1) && !reader.Read(out.data(), 1), "reject corrupted offsets before touching payload storage");
    writer.Clear();
    reader.Destroy();
    reader.Destroy();
    check(!reader.GetData() && !reader.GetUserHeader(), "destroy clears all borrowed pointers");
    check(reader.Open(id.c_str()), "destroyed view can reopen");
    check(!reader.Read(out.data(), 1), "clear resets the shared counters");
}

void model()
{
    std::mt19937 random(0xC00123);
    for (const int capacity : {2, 3, 8, 17, 64, 257})
    {
        const auto id = name();
        CShareMemBuffer writer, reader;
        check(writer.Create(id.c_str(), capacity, 16) && reader.Open(id.c_str(), 16), "open model-test views with user header");
        if (!writer.GetUserHeader() || !reader.GetData())
            return;
        std::memset(writer.GetUserHeader(), 0x5a, 16);
        std::deque<char> expected;
        std::array<char, 260> input{}, output{};
        bool valid = true;
        for (int operation = 0; operation < 20000; ++operation)
        {
            const auto count = static_cast<int>(random() % (capacity + 2));
            if (random() % 2)
            {
                for (int index = 0; index < count; ++index)
                    input[index] = static_cast<char>(random());
                const bool accepted = static_cast<std::size_t>(count) <= capacity - 1 - expected.size();
                valid = (writer.Write(input.data(), count) != FALSE) == accepted && valid;
                if (accepted)
                    expected.insert(expected.end(), input.begin(), input.begin() + count);
            }
            else
            {
                output.fill('#');
                const bool accepted = static_cast<std::size_t>(count) <= expected.size();
                valid = (reader.Read(output.data(), count) != FALSE) == accepted && valid;
                if (accepted)
                    for (int index = 0; index < count; ++index)
                    {
                        valid = output[index] == expected.front() && valid;
                        expected.pop_front();
                    }
                else
                    valid = std::ranges::all_of(output, [](char c) { return c == '#'; }) && valid;
            }
        }
        check(valid, "byte ring matches a FIFO model through full, empty and wrapped states");
        check(std::memcmp(writer.GetUserHeader(), std::string(16, 'Z').data(), 16) == 0,
              "ring operations preserve the independent user header");
    }
}

void invalid_metadata_and_cleanup()
{
    const auto missing = name();
    CShareMemBuffer reader;
    check(!reader.Open(missing.c_str()), "absent peer is an ordinary open failure");
    RawMapping raw;
    const auto invalid = name();
    check(CreateSharedMemory(invalid.c_str(), sizeof(MemHeader) + 8, raw.value), "create isolated invalid-metadata fixture");
    auto* header = reinterpret_cast<MemHeader*>(raw.value.pSharedMem);
    *header = {SHARED_MEM_TAG, 4096, 0, 0};
    check(!reader.Open(invalid.c_str()) && !reader.GetData(), "failed attach rejects size exceeding the mapping and releases its view");
    *header = {SHARED_MEM_TAG, 8, 0, 9};
    check(!reader.Open(invalid.c_str()), "failed attach rejects invalid initial counters");
    *header = {SHARED_MEM_TAG, 8, 0, 0};
    check(reader.Open(invalid.c_str()), "view recovers after failed attach");
    reader.Destroy();

    DWORD before = 0, after = 0;
    GetProcessHandleCount(GetCurrentProcess(), &before);
    for (int iteration = 0; iteration < 100; ++iteration)
    {
        const auto id = name();
        {
            CShareMemBuffer scope;
            check(scope.Create(id.c_str(), 17, 0), "create scope-owned mapping");
        }
        check(!IsShareMemoryExists(id.c_str()), "scope exit releases the final named mapping handle");
    }
    GetProcessHandleCount(GetCurrentProcess(), &after);
    check(after <= before + 2, "mapping lifecycle does not leak native handles");
    MemPtr empty{};
    CloseSharedMemory(empty);
    check(!empty.pFd && !empty.pSharedMem && empty.qwSize == 0, "closing an empty mapping is idempotent");

    const auto conflict = name();
    Handle event(CreateEventA(nullptr, FALSE, FALSE, conflict.c_str()));
    check(event && !CreateSharedMemory(conflict.c_str(), 32, empty) && !empty.pFd && !empty.pSharedMem,
          "native object-type conflict fails without leaking an output handle");

    const auto invalidOuter = name();
    Handle outerHandle(CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 4096, invalidOuter.c_str()));
    View outerView(outerHandle ? MapViewOfFile(outerHandle.get(), FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0) : nullptr);
    check(outerView != nullptr, "create isolated malformed outer header");
    if (outerView)
    {
        auto* outer = static_cast<ShareMemHeader*>(outerView.get());
        outer->qwSize = (std::numeric_limits<UINT64>::max)();
        cqyh::SharedStore32(&outer->nTag, SHARED_MEM_HEAD_TAG);
        GetProcessHandleCount(GetCurrentProcess(), &before);
        check(!OpenSharedMemory(invalidOuter.c_str(), empty) && !empty.pFd && !empty.pSharedMem,
              "outer metadata cannot advertise memory beyond its mapped view");
        GetProcessHandleCount(GetCurrentProcess(), &after);
        check(after <= before, "failed outer-header validation releases its temporary handle and view");
        outer->qwSize = 32;
        cqyh::SharedStore32(&outer->nTag, 0);
        check(!OpenSharedMemory(invalidOuter.c_str(), empty), "unpublished outer metadata times out without being overwritten");
    }
}

struct Control { UINT32 magic; INT32 status; INT32 received; UINT32 reserved; };
static_assert(sizeof(Control) == 16);
constexpr UINT32 magic = 0x1234abcd;
constexpr unsigned frames = 100000;
using Frame = std::array<UINT32, 8>;
Frame make_frame(unsigned sequence)
{
    Frame frame{};
    for (unsigned index = 0; index < frame.size(); ++index)
        frame[index] = sequence ^ (index * UINT32{0x9e3779b9});
    return frame;
}
int child(const char* id)
{
    const auto eventName = std::string(id) + "_reader_started";
    Handle started(OpenEventA(EVENT_MODIFY_STATE, FALSE, eventName.c_str()));
    if (!started || !SetEvent(started.get()))
        return 1;
    CShareMemBuffer reader;
    if (!reader.Open(id, sizeof(Control)))
        return 2;
    auto* control = static_cast<Control*>(reader.GetUserHeader());
    if (cqyh::SharedLoad32(&control->status) != 3 || control->magic != magic)
        return 3;
    cqyh::SharedStore32(&control->status, 2);
    const auto deadline = std::chrono::steady_clock::now() + 15s;
    for (unsigned sequence = 0; sequence < frames; ++sequence)
    {
        Frame frame{};
        while (!reader.Read(reinterpret_cast<char*>(frame.data()), sizeof(frame)))
        {
            if (std::chrono::steady_clock::now() > deadline)
                return 4;
            std::this_thread::yield();
        }
        if (frame != make_frame(sequence))
            return 5;
    }
    cqyh::SharedStore32(&control->received, frames);
    cqyh::SharedStore32(&control->status, 1);
    return 0;
}

void cross_process(bool delayed)
{
    const auto id = name();
    const auto eventName = id + "_reader_started";
    Handle started(CreateEventA(nullptr, TRUE, FALSE, eventName.c_str()));
    check(started != nullptr, "create peer-start synchronization event");
    if (!started)
        return;
    CShareMemBuffer writer;
    Handle raw;
    View rawView;
    constexpr auto payloadSize = sizeof(Control) + sizeof(MemHeader) + 257;
    if (delayed)
    {
        raw.reset(CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                                    payloadSize + sizeof(ShareMemHeader), id.c_str()));
        rawView.reset(raw ? MapViewOfFile(raw.get(), FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0) : nullptr);
        check(rawView != nullptr, "create unpublished mapping for startup-publication test");
        if (!rawView)
            return;
    }
    else
    {
        check(writer.Create(id.c_str(), 257, sizeof(Control)), "create cross-process writer");
        if (!writer.GetUserHeader())
            return;
        *static_cast<Control*>(writer.GetUserHeader()) = {magic, 0, 0, 0};
        cqyh::SharedStore32(&static_cast<Control*>(writer.GetUserHeader())->status, 3);
    }
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    check(length > 0 && length < executable.size(), "locate current executable for isolated peer process");
    if (!length || length >= executable.size())
        return;
    const std::wstring wideId(id.begin(), id.end());
    auto command = L"\"" + std::wstring(executable.data()) + L"\" --reader " + wideId;
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    const bool launched = CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                                         nullptr, nullptr, &startup, &process) != FALSE;
    check(launched, "launch isolated consumer process");
    if (!launched)
        return;
    Handle childProcess(process.hProcess), childThread(process.hThread);
    check(WaitForSingleObject(started.get(), 5000) == WAIT_OBJECT_0, "consumer starts before metadata publication");
    if (delayed)
    {
        // Force the peer through both ready-tag waits, keeping the existing layouts.
        std::this_thread::sleep_for(100ms);
        auto* outer = static_cast<ShareMemHeader*>(rawView.get());
        auto* control = reinterpret_cast<Control*>(static_cast<char*>(rawView.get()) + sizeof(ShareMemHeader));
        *control = {magic, 0, 0, 0};
        cqyh::SharedStore32(&control->status, 3);
        outer->qwSize = payloadSize;
        cqyh::SharedStore32(&outer->nTag, SHARED_MEM_HEAD_TAG);
        std::this_thread::sleep_for(100ms);
        auto* ring = reinterpret_cast<MemHeader*>(reinterpret_cast<char*>(control) + sizeof(Control));
        *ring = {0, 257, 0, 0};
        cqyh::SharedStore32(&ring->nMemTag, SHARED_MEM_TAG);
        check(writer.Create(id.c_str(), 257, sizeof(Control)), "attach writer after delayed metadata publication");
    }
    bool sent = writer.GetUserHeader() != nullptr;
    const auto deadline = std::chrono::steady_clock::now() + 15s;
    for (unsigned sequence = 0; sent && sequence < frames; ++sequence)
    {
        const auto frame = make_frame(sequence);
        while (!writer.Write(reinterpret_cast<const char*>(frame.data()), sizeof(frame)))
        {
            if (std::chrono::steady_clock::now() > deadline || WaitForSingleObject(childProcess.get(), 0) == WAIT_OBJECT_0)
            {
                sent = false;
                break;
            }
            std::this_thread::yield();
        }
    }
    const auto waited = WaitForSingleObject(childProcess.get(), 20000);
    DWORD exitCode = STILL_ACTIVE;
    GetExitCodeProcess(childProcess.get(), &exitCode);
    auto* control = static_cast<Control*>(writer.GetUserHeader());
    check(sent && waited == WAIT_OBJECT_0 && exitCode == 0, "separate process receives every wrapped frame in FIFO order");
    check(control && cqyh::SharedLoad32(&control->status) == 1 &&
          cqyh::SharedLoad32(&control->received) == frames, "peer publishes successful payload validation");
}
}

int main(int argc, char** argv)
{
    CLog::GetInstance().SetOutputMask(lotNone);
    if (argc == 3 && std::string_view(argv[1]) == "--reader")
        return child(argv[2]);
    boundaries();
    model();
    invalid_metadata_and_cleanup();
    cross_process(false);
    cross_process(true);
    std::printf("%d shared-memory checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
