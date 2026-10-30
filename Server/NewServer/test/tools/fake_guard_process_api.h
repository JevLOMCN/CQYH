#pragma once

// Private forced include for the offline process-manager fixture. Include native
// declarations before aliasing calls; production targets never receive this file.
#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include "std_type.h"
#include "process_error.h"
#include "time_ex.h"
#include "statis_info.h"

BOOL WINAPI GuardTestEnumProcesses(DWORD*, DWORD, DWORD*);
HANDLE WINAPI GuardTestOpenProcess(DWORD, BOOL, DWORD);
BOOL WINAPI GuardTestEnumProcessModules(HANDLE, HMODULE*, DWORD, DWORD*);
DWORD WINAPI GuardTestGetModuleBaseNameA(HANDLE, HMODULE, char*, DWORD);
BOOL WINAPI GuardTestCloseHandle(HANDLE);
BOOL WINAPI GuardTestCreateProcessA(const char*, char*, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, void*, const char*, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
BOOL WINAPI GuardTestTerminateProcess(HANDLE, UINT);
BOOL WINAPI GuardTestGetSystemTimes(FILETIME*, FILETIME*, FILETIME*);
BOOL WINAPI GuardTestGetProcessTimes(HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME*);
BOOL WINAPI GuardTestGetProcessMemoryInfo(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
BOOL WINAPI GuardTestGetExitCodeProcess(HANDLE, DWORD*);
DWORD WINAPI GuardTestWaitForSingleObject(HANDLE, DWORD);
UINT64 GuardTestTick();
INT32 GuardTestStatisIndex(UINT32);
BOOL GuardTestSetStatis(INT32, INT32, INT64);
void GuardTestResetStatis();

#undef EnumProcesses
#undef EnumProcessModules
#undef GetModuleBaseNameA
#undef GetModuleBaseName
#undef CreateProcess
#undef GetProcessMemoryInfo
#define EnumProcesses GuardTestEnumProcesses
#define OpenProcess GuardTestOpenProcess
#define EnumProcessModules GuardTestEnumProcessModules
#define GetModuleBaseNameA GuardTestGetModuleBaseNameA
#define GetModuleBaseName GuardTestGetModuleBaseNameA
#define CreateProcess GuardTestCreateProcessA
#define CloseHandle GuardTestCloseHandle
#define CreateProcessA GuardTestCreateProcessA
#define TerminateProcess GuardTestTerminateProcess
#define GetSystemTimes GuardTestGetSystemTimes
#define GetProcessTimes GuardTestGetProcessTimes
#define GetProcessMemoryInfo GuardTestGetProcessMemoryInfo
#define GetExitCodeProcess GuardTestGetExitCodeProcess
#define WaitForSingleObject GuardTestWaitForSingleObject
#define GetTickCountEx GuardTestTick
#define g_GetProcessStatisIndex GuardTestStatisIndex
#define g_SetProcessStatisInfo GuardTestSetStatis
#define g_ResetStatisInfo GuardTestResetStatis
