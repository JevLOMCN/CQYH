// Read-only smoke for the real Windows API adapter. No other process is opened,
// and no creation, termination, deployment or service-management API is called.
#include <windows.h>
#include <psapi.h>
#include <cstdio>
#include "native_handle.h"

int main()
{
    int checks = 0;
    int failures = 0;
    const auto check = [&](bool value, const char* description) {
        ++checks;
        if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
    };
    HANDLE borrowed = nullptr;
    {
        guard::NativeHandle process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
            FALSE, GetCurrentProcessId())};
        check(process != nullptr, "open own process using poll access rights");
        if (!process) return 1;
        borrowed = process.get();
        DWORD flags = 0;
        check(GetHandleInformation(process.get(), &flags) != FALSE && !(flags & HANDLE_FLAG_INHERIT),
            "query handle is valid and not inherited");
        FILETIME creation{}, exit{}, kernel{}, user{};
        check(GetProcessTimes(process.get(), &creation, &exit, &kernel, &user) != FALSE,
            "query creation identity using limited information rights");
        check(creation.dwLowDateTime || creation.dwHighDateTime, "real creation identity is nonzero");
        check(WaitForSingleObject(process.get(), 0) == WAIT_TIMEOUT, "live process poll is nonblocking");
        DWORD code = 0;
        check(GetExitCodeProcess(process.get(), &code) != FALSE && code == STILL_ACTIVE,
            "query exit status using poll access rights");
    }
    DWORD flags = 0;
    check(!GetHandleInformation(borrowed, &flags) && GetLastError() == ERROR_INVALID_HANDLE,
        "scoped owner closes the real query handle");
    {
        guard::NativeHandle process{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, GetCurrentProcessId())};
        check(process != nullptr, "open own process using statistics access rights");
        if (!process) return 1;
        PROCESS_MEMORY_COUNTERS memory{};
        memory.cb = sizeof(memory);
        check(GetProcessMemoryInfo(process.get(), &memory, sizeof(memory)) != FALSE,
            "query memory using statistics access rights");
        FILETIME idle{}, kernel{}, user{};
        check(GetSystemTimes(&idle, &kernel, &user) != FALSE, "query system time counters");
    }
    std::printf("guard native handles: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
