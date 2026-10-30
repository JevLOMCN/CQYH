#include "stdafx.h"
#include "dump.h"
#include "time_ex.h"
#include "share_mem.h"
#include "cas.h"
#include "dir.h"
#include "report.h"
#include <eh.h>
#include <algorithm>
#include <atomic>
#include <array>
#include <future>
#include <memory>
#include <mutex>
#include <utility>

#define DUMP_INTERVAL           (3000 * 1000)
#define MAX_WAIT_DUMP_TIME      (100)
#define DUMP_MEMORY_NAME        "DumpShareMemory"
#define MAX_DUMP_PROCESS_COUNT  (64)
#define DUMP_SHARE_MEMORY_TAG   'dump'
#define CHECK_PROCESS_INTERVAL  (8 * 1000)

#ifdef __linux__

#endif  // __linux__

#ifdef WIN32

#include <psapi.h>
#include <DbgHelp.h>

struct DUMP_SHARE_MEMORY
{
    UINT32          dwTag;
    struct PROCESS_DATA
    {
        volatile BOOL bUsed;
        struct _EXCEPTION_POINTERS* pExceptionPointers;
        UINT32  dwProcessID;
        UINT32  dwThreadID;
        BOOL    bDump;
        UINT64  qwLastDumpTick;
		UINT64	qwLastCheckAliveTick;
    }       ProcessList[MAX_DUMP_PROCESS_COUNT];
};

// These owners and flags are local to this process. The shared record above
// retains its native representation for interoperability with existing hosts.
struct DumpMapping
{
    MemPtr memory{};
    INT32 processIndex = 0;
    bool claimedSlot = false;
    DUMP_SHARE_MEMORY* View() const
    {
        return reinterpret_cast<DUMP_SHARE_MEMORY*>(memory.pSharedMem);
    }
    ~DumpMapping()
    {
        if (claimedSlot)
            InterlockedExchange(reinterpret_cast<volatile LONG*>(&View()->ProcessList[processIndex].bUsed), FALSE);
        if (memory.pSharedMem)
            CloseSharedMemory(memory);
    }
};
static std::atomic<std::shared_ptr<DumpMapping>> s_dumpMapping;
static DUMP_CHECK_FUNC s_pDumpCheckFunc = nullptr;
static BOOL s_bDumpHost = FALSE;
static char s_szDumpFolder[256] = {};
static std::mutex s_dumpFolderMutex;
static std::atomic<bool> s_bShowExceptionDialog{false};
static std::atomic<bool> s_bLocalDump{false};
static std::atomic<_EXCEPTION_POINTERS*> s_pExceptionPointers{nullptr};
static bool s_timerPeriodActive = false;
// Destroy the worker before the state it accesses during cooperative shutdown.
static CThread s_dumpThread;

void CreateDumpFile(struct _EXCEPTION_POINTERS* pExceptionPointers, UINT32 dwProcessID, UINT32 dwThreadID)
{
    std::array<char, sizeof(s_szDumpFolder)> folder;
    {
        std::scoped_lock lock(s_dumpFolderMutex);
        std::copy_n(s_szDumpFolder, folder.size(), folder.data());
    }
    INT32 nRetCode = 0;
    char szFileName[MAX_PATH] = { 0 };
    char szModuleName[MAX_PATH] = {0};
	char szWorkingDir[MAX_PATH] = { 0 };
    char* pszModuleName = NULL;

    DWORD dwBufferSize = MAX_PATH;
    HANDLE hDumpFile = INVALID_HANDLE_VALUE;
    HANDLE hProcess = NULL;
	BOOL bSmallDump = FALSE;

    SYSTEMTIME stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION ExParam;
    GetLocalTime(&stLocalTime);

	GetCurrentDirectory(MAX_PATH, szWorkingDir);

#if defined (_FINAL)
	GetDiskFreeSpaceEx(szWorkingDir, &FreeBytes, NULL, NULL);
	PROCESS_ERROR(FreeBytes.QuadPart > 1024LL * 1024 * 1024 * 16);
	if (FreeBytes.QuadPart < 1024LL * 1024 * 1024 * 32)
		bSmallDump = TRUE;
#endif // _FINAL

	LOG_PROCESS_ERROR(folder[0] != '\0');
    g_CreateDirectory(folder.data(), TRUE);

    snprintf(szFileName, MAX_PATH, "%s/%04d_%02d_%02d", 
		folder.data(),
        stLocalTime.wYear, 
        stLocalTime.wMonth, 
        stLocalTime.wDay);
    g_CreateDirectory(szFileName, TRUE);

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessID);
    if (hProcess == NULL)
        goto Exit0;

    GetProcessImageFileName(hProcess, szModuleName, MAX_PATH);
    pszModuleName = szModuleName + strlen(szModuleName);
    while (*pszModuleName != '.' && pszModuleName > szModuleName)
        pszModuleName--;
    if (*pszModuleName == '.')
    {
        *pszModuleName = 0;
        while (*pszModuleName != '\\' && pszModuleName > szModuleName)
            pszModuleName--;
        pszModuleName++;
    }

    snprintf(szFileName, MAX_PATH, "%s/%04d_%02d_%02d/%s_%d_%04d_%02d_%02d_%02d_%02d_%02d_%03d.dmp", 
		folder.data(),
        stLocalTime.wYear, 
        stLocalTime.wMonth, 
        stLocalTime.wDay, 
        pszModuleName, 
		dwProcessID,
        stLocalTime.wYear, 
        stLocalTime.wMonth, 
        stLocalTime.wDay, 
        stLocalTime.wHour, 
        stLocalTime.wMinute, 
        stLocalTime.wSecond, 
        stLocalTime.wMilliseconds);

    hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    if (hDumpFile == INVALID_HANDLE_VALUE)
        goto Exit0;

    ExParam.ThreadId = dwThreadID;
    ExParam.ExceptionPointers = pExceptionPointers;
    ExParam.ClientPointers = TRUE;

    if (pExceptionPointers == NULL || bSmallDump)
    {
        nRetCode = MiniDumpWriteDump(hProcess, dwProcessID, hDumpFile, 
            (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithThreadInfo), NULL, NULL, NULL);
        if (!nRetCode)
            goto Exit0;
    }
    else
    {
        nRetCode = MiniDumpWriteDump(hProcess, dwProcessID, hDumpFile, 
            (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithFullMemory | MiniDumpWithThreadInfo), &ExParam, NULL, NULL);
        if (!nRetCode)
            goto Exit0;

		REPORT(1, "dump file %s for process %d thread %d", szFileName, dwProcessID, dwThreadID);
    }

Exit0:
    if (hDumpFile != INVALID_HANDLE_VALUE)
        CloseHandle(hDumpFile);

    if (hProcess != NULL)
        CloseHandle(hProcess);

    return;
}

void DumpThreadProc(std::stop_token token, std::promise<BOOL> ready)
{
	INT32 nRetCode = 0;
	UINT64 qwLastCheckTick = 0;
	auto mapping = std::make_shared<DumpMapping>();
    DUMP_SHARE_MEMORY* memory = nullptr;

    if (s_bDumpHost)
    {
        nRetCode = CreateSharedMemory(DUMP_MEMORY_NAME, sizeof(DUMP_SHARE_MEMORY), mapping->memory);
        if (!nRetCode)
        {
            LOG_CHECK_ERROR(nRetCode);
            ready.set_value(FALSE);
            return;
        }

        memory = mapping->View();
        memset(memory, 0, sizeof(DUMP_SHARE_MEMORY));

        cqyh::SharedStore32(&memory->ProcessList[0].bUsed, TRUE);
        memory->ProcessList[0].dwProcessID = GetCurrentProcessId();

        cqyh::SharedStore32(reinterpret_cast<INT32*>(&memory->dwTag), DUMP_SHARE_MEMORY_TAG);
        s_dumpMapping.store(mapping);
    }

    ready.set_value(TRUE);
    while (!token.stop_requested())
    {
        if (s_bDumpHost)
        {
            for (INT32 nIndex = 0; nIndex < MAX_DUMP_PROCESS_COUNT; nIndex++)
            {
                if (!cqyh::SharedLoad32(&memory->ProcessList[nIndex].bUsed))
                    continue;

                UINT64 qwCurrentTick = GetTickCountEx();

                if (cqyh::SharedLoad32(&memory->ProcessList[nIndex].bDump))
                {
                    if (qwCurrentTick - memory->ProcessList[nIndex].qwLastDumpTick > DUMP_INTERVAL ||
                        memory->ProcessList[nIndex].pExceptionPointers == NULL)
                    {
                        CreateDumpFile(memory->ProcessList[nIndex].pExceptionPointers,
                            memory->ProcessList[nIndex].dwProcessID,
                            memory->ProcessList[nIndex].dwThreadID);
                        memory->ProcessList[nIndex].qwLastDumpTick = qwCurrentTick;
						INF("created dump for process %d", memory->ProcessList[nIndex].dwProcessID);
                    }
                    cqyh::SharedStore32(&memory->ProcessList[nIndex].bDump, FALSE);
                }

				if (qwCurrentTick - memory->ProcessList[nIndex].qwLastCheckAliveTick > CHECK_PROCESS_INTERVAL)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, memory->ProcessList[nIndex].dwProcessID);
                    if (hProcess == NULL)
                    {
                        cqyh::SharedStore32(&memory->ProcessList[nIndex].bUsed, FALSE);
                    }
                    else
                    {
                        CloseHandle(hProcess);
                    }
					memory->ProcessList[nIndex].qwLastCheckAliveTick = qwCurrentTick;
                }
            }
        }
        else
        {
            UINT64 qwCurrentTick = GetTickCountEx();

            if ((memory == NULL || !cqyh::SharedLoad32(&memory->ProcessList[mapping->processIndex].bUsed)) && qwCurrentTick - qwLastCheckTick > CHECK_PROCESS_INTERVAL)
            {
				s_dumpMapping.store(nullptr);
                mapping = std::make_shared<DumpMapping>();
                memory = nullptr;
                nRetCode = OpenSharedMemory(DUMP_MEMORY_NAME, mapping->memory);
                if (nRetCode && cqyh::SharedLoad32(reinterpret_cast<const INT32*>(&mapping->View()->dwTag)) == DUMP_SHARE_MEMORY_TAG)
                {
                    memory = mapping->View();
                    mapping->processIndex = MAX_DUMP_PROCESS_COUNT;

                    for (INT32 nIndex = 0; nIndex < MAX_DUMP_PROCESS_COUNT; nIndex++)
                    {
                        if (!cqyh::SharedLoad32(&memory->ProcessList[nIndex].bUsed))
                        {
                            nRetCode = InterlockedCompareExchange((volatile long*)&memory->ProcessList[nIndex].bUsed, TRUE, FALSE);
                            if (nRetCode == FALSE)
                            {
                                mapping->processIndex = nIndex;
                                mapping->claimedSlot = true;
                                memory->ProcessList[nIndex].dwProcessID = GetCurrentProcessId();
                                break;
                            }
                        }
                    }
                    LOG_CHECK_ERROR(mapping->processIndex < MAX_DUMP_PROCESS_COUNT);
                    if (mapping->processIndex < MAX_DUMP_PROCESS_COUNT)
                        s_dumpMapping.store(mapping);
                    else
                        memory = nullptr;
                }
                qwLastCheckTick = qwCurrentTick;
            }

            if (s_bLocalDump)
            {
                CreateDumpFile(s_pExceptionPointers.load(), GetCurrentProcessId(), GetCurrentThreadId());
                s_bLocalDump = FALSE;
            }

            if (s_pDumpCheckFunc && s_pDumpCheckFunc())
            {
                INT32 nWaitTimer = 0;

                if (memory && cqyh::SharedLoad32(reinterpret_cast<const INT32*>(&memory->dwTag)))
                {
                    memory->ProcessList[mapping->processIndex].pExceptionPointers = NULL;
                    memory->ProcessList[mapping->processIndex].dwProcessID = GetCurrentProcessId();
                    memory->ProcessList[mapping->processIndex].dwThreadID = GetCurrentThreadId();
                    cqyh::SharedStore32(&memory->ProcessList[mapping->processIndex].bDump, TRUE);

                    while (!token.stop_requested() && cqyh::SharedLoad32(&memory->ProcessList[mapping->processIndex].bDump) && nWaitTimer < MAX_WAIT_DUMP_TIME)
                    {
                        nWaitTimer++;
                        s_dumpThread.WaitForStop(std::chrono::milliseconds(50));
                    }
                }
				else
				{
					CreateDumpFile(NULL, GetCurrentProcessId(), GetCurrentThreadId());
				}
            }
        }

        s_dumpThread.WaitForStop(std::chrono::milliseconds(16));
    }

    s_dumpMapping.store(nullptr);
}

LONG WINAPI DumpExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionPointers)
{
    // Keep the view alive if the worker reconnects or stops during this filter.
    const auto mapping = s_dumpMapping.load();
    auto* memory = mapping ? mapping->View() : nullptr;
    const auto processIndex = mapping ? mapping->processIndex : 0;
    INT32 nWaitTimer = 0;

    if (memory && cqyh::SharedLoad32(reinterpret_cast<const INT32*>(&memory->dwTag)))
    {
        memory->ProcessList[processIndex].pExceptionPointers = pExceptionPointers;
        memory->ProcessList[processIndex].dwProcessID = GetCurrentProcessId();
        memory->ProcessList[processIndex].dwThreadID = GetCurrentThreadId();
        cqyh::SharedStore32(&memory->ProcessList[processIndex].bDump, TRUE);

        while (cqyh::SharedLoad32(&memory->ProcessList[processIndex].bDump) && nWaitTimer < MAX_WAIT_DUMP_TIME)
        {
            nWaitTimer++;
            Sleep(50);
        }
    }
    else
    {
        s_pExceptionPointers = pExceptionPointers;
        s_bLocalDump = TRUE;
        while (s_bLocalDump && nWaitTimer < MAX_WAIT_DUMP_TIME)
        {
            nWaitTimer++;
            Sleep(50);
        }
    }

    CRI("exception code %08X, address %p", 
        pExceptionPointers->ExceptionRecord->ExceptionCode, pExceptionPointers->ExceptionRecord->ExceptionAddress);

    if (!IsDebuggerPresent() && s_bShowExceptionDialog)
    {
        MessageBox(NULL, "exception detected", NULL, 0);
    }

    if (IsDebuggerPresent())
    {
        INT3();
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

#if defined(WIN32) | defined (WIN64)

void DisableSetUnhandledExceptionFilter(void)
{
    HMODULE hModule = NULL;
    void* addr = nullptr;
    int size = 0;

    hModule = LoadLibrary("kernel32.dll");
    LOG_PROCESS_ERROR(hModule);

    addr = (void*)GetProcAddress(hModule, "SetUnhandledExceptionFilter");
    LOG_PROCESS_ERROR(addr);

    unsigned char code[16];
	code[size++] = 0x33;
	code[size++] = 0xC0;
	code[size++] = 0xC2;
#if defined(WIN64)
    code[size++] = 0x00;
#else
	code[size++] = 0x04;
#endif	// WIN64
    code[size++] = 0x00;

    DWORD dwOldFlag, dwTempFlag;
    VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
    WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
    VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);

Exit0:
    if (hModule)
        FreeLibrary(hModule);

    return;
}

#pragma optimize("", off)

typedef DWORD(*THREAD_PROC)(void* pUserData);
static THREAD_PROC s_pThreadProcList[1024] = { 0 };
static void* s_pThreadUserDataList[1024] = { 0 };
static INT32 s_nThreadCount = 0;

DWORD WINAPI SafeThreadProc(void* pUserData)
{
	DWORD dwResult = 0;
	INT32 nThreadIndex = (INT32)pUserData;

	LOG_PROCESS_ERROR(nThreadIndex >= 0 && nThreadIndex < s_nThreadCount);
	LOG_PROCESS_ERROR(s_pThreadProcList[nThreadIndex] != NULL);

	INF("start safe thread %d", nThreadIndex);

	__try
	{
		dwResult = s_pThreadProcList[nThreadIndex](s_pThreadUserDataList[nThreadIndex]);
	}
	__except (DumpExceptionFilter(GetExceptionInformation()))
	{
		CRI("exception in thread %d", nThreadIndex);
	}

Exit0:

	return dwResult;
}

extern "C" void _ReplaceThreadProc(int a, int b, void* pThreadFunc, void* pUserData);

INT32 ReplaceThreadProc(int a, int b, void* pThreadFunc, void* pUserData)
{
	s_pThreadProcList[s_nThreadCount] = (THREAD_PROC)pThreadFunc;
	s_pThreadUserDataList[s_nThreadCount] = pUserData;

	_ReplaceThreadProc(0, 0, SafeThreadProc, (void*)s_nThreadCount);

	s_nThreadCount++;
	return s_nThreadCount - 1;
}

#pragma optimize("", on)

void HookCreateThread(void)
{
	HMODULE hModule = NULL;
	unsigned char* addr = nullptr;
	DWORD dwOldFlag, dwTempFlag;
	INT32 nEntryOffset = 0x152;
	INT32 nOffset1, nOffset2, nOffset3;

	hModule = LoadLibrary("kernel32.dll");
	LOG_PROCESS_ERROR(hModule);

	addr = (unsigned char*)GetProcAddress(hModule, "CreateThread") - nEntryOffset;
	LOG_PROCESS_ERROR(addr);

	VirtualProtect(addr, nEntryOffset + 4, PAGE_READWRITE, &dwOldFlag);

	if (*(UINT32*)(addr + nEntryOffset) == 0x48EC8348)
	{
		nOffset1 = 0x62;
		nOffset2 = 0xE0;
		nOffset3 = 0x148;

		LOG_PROCESS_ERROR(*(UINT64*)(addr + nOffset1) == 0x9090909090909090);
		LOG_PROCESS_ERROR(*(UINT64*)(addr + nOffset2) == 0x9090909090909090);
		LOG_PROCESS_ERROR(*(UINT64*)(addr + nOffset2 + 8) == 0x9090909090909090);

		*(UINT64*)(addr + nOffset1) = (UINT64)&ReplaceThreadProc;

		addr[nOffset2++] = 0x48;
		addr[nOffset2++] = 0x83;
		addr[nOffset2++] = 0xEC;
		addr[nOffset2++] = 0x48;
		addr[nOffset2++] = 0xE8;
		addr[nOffset2++] = 0x5F;
		addr[nOffset2++] = 0x00;
		addr[nOffset2++] = 0x00;
		addr[nOffset2++] = 0x00;
		addr[nOffset2++] = 0xEB;
		addr[nOffset2++] = (nEntryOffset + 4) - (nOffset2 + 1);

		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0x25;
		addr[nOffset3++] = 0x14;
		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0xEB;
		addr[nOffset3++] = 0x8C;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
	}
	else if (*(UINT32*)(addr + nEntryOffset) == 0x48DC8B4C)
	{
		nOffset1 = 0x62;
		nOffset2 = 0xE0;
		nOffset3 = 0x148;

		LOG_PROCESS_ERROR(*(UINT64*)(addr + nOffset1) == 0x9090909090909090);
		LOG_PROCESS_ERROR(*(UINT64*)(addr + nOffset2) == 0x9090909090909090);
		LOG_PROCESS_ERROR(*(UINT64*)(addr + nOffset2 + 8) == 0x9090909090909090);

		*(UINT64*)(addr + nOffset1) = (UINT64)&ReplaceThreadProc;

		addr[nOffset2++] = 0x8B;
		addr[nOffset2++] = 0xDC;
		addr[nOffset2++] = 0x48;
		addr[nOffset2++] = 0x90;
		addr[nOffset2++] = 0xE8;
		addr[nOffset2++] = 0x5F;
		addr[nOffset2++] = 0x00;
		addr[nOffset2++] = 0x00;
		addr[nOffset2++] = 0x00;
		addr[nOffset2++] = 0xEB;
		addr[nOffset2++] = 0x6B;

		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0x25;
		addr[nOffset3++] = 0x14;
		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0xFF;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0x90;
		addr[nOffset3++] = 0xEB;
		addr[nOffset3++] = 0x8C;
		addr[nOffset3++] = 0x90;
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	VirtualProtect(addr, nEntryOffset + 4, dwOldFlag, &dwTempFlag);

Exit0:
	if (hModule)
		FreeLibrary(hModule);

	return;
}
#endif	// WIN32 | WIN64

void _crt_handler(void)
{
	INT3();
}

void _invalid_param_handler(const wchar_t* pcszExpression, const wchar_t* pcszFunction,
	const wchar_t* pcszFile, unsigned int dwLine, uintptr_t reserved)
{
	INT3();
}

int __cdecl MyReportHook(int, char* pszMessage, int *)
{
	INT3();
	return 0;
}

BOOL StartDumpThread(DUMP_CHECK_FUNC pDumpCheckFunc, BOOL bDumpHost, const char* pDumpFolder)
{
    // Startup/shutdown are controlled by the owning server thread.
    if (!pDumpFolder || !*pDumpFolder || s_timerPeriodActive || s_dumpThread.IsRunning())
        return FALSE;
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    SetUnhandledExceptionFilter(DumpExceptionFilter);
    _set_purecall_handler(_crt_handler);
    set_terminate(_crt_handler);
    _set_invalid_parameter_handler(_invalid_param_handler);
    s_timerPeriodActive = timeBeginPeriod(1) == TIMERR_NOERROR;
#if defined (_FINAL)
    char workingDirectory[MAX_PATH] = {};
    ULARGE_INTEGER freeBytes{};
    GetCurrentDirectory(MAX_PATH, workingDirectory);
    if (!GetDiskFreeSpaceEx(workingDirectory, &freeBytes, nullptr, nullptr) ||
        freeBytes.QuadPart <= 1024LL * 1024 * 1024 * 36)
    {
        LOG_CHECK_ERROR(false);
        StopDumpThread();
        return FALSE;
    }
#endif
    s_bDumpHost = bDumpHost;
    SetDumpFolder(pDumpFolder);
    s_pDumpCheckFunc = pDumpCheckFunc;
    s_bLocalDump.store(false);
    s_pExceptionPointers.store(nullptr);
    try
    {
        std::promise<BOOL> ready;
        auto started = ready.get_future();
        if (!s_dumpThread.Start(CThread::Work([ready = std::move(ready)](std::stop_token token) mutable
            { DumpThreadProc(token, std::move(ready)); })) || !started.get())
        {
            StopDumpThread();
            return FALSE;
        }
    }
    catch (const std::exception& error)
    {
        ERR("Cannot initialize dump worker: %s", error.what());
        StopDumpThread();
        return FALSE;
    }
    return TRUE;
}

void SetDumpOption(BOOL bShowDumpDialog)
{
    s_bShowExceptionDialog.store(bShowDumpDialog != FALSE);
}

void SetDumpFolder(const char* pDumpFolder)
{
    std::scoped_lock lock(s_dumpFolderMutex);
    strxcpy(s_szDumpFolder, pDumpFolder, sizeof(s_szDumpFolder));
}

BOOL StopDumpThread()
{
    s_dumpThread.Wait();
    s_dumpMapping.store(nullptr);
    if (s_timerPeriodActive)
    {
        timeEndPeriod(1);
        s_timerPeriodActive = false;
    }
    return TRUE;
}

BOOL CheckDiskRemainCapacity(void)
{
#ifdef _FINAL
	char szWorkingDir[MAX_PATH] = { 0 };
	ULARGE_INTEGER FreeBytes;
	GetCurrentDirectory(MAX_PATH, szWorkingDir);
	GetDiskFreeSpaceEx(szWorkingDir, &FreeBytes, NULL, NULL);
	LOG_PROCESS_ERROR(FreeBytes.QuadPart < 32LL * 1024 * 1024 * 1024);
#endif	// _FINAL

	return TRUE;
Exit0:
	return FALSE;
}

#endif  // WIN32

void TestDump(void)
{
	{
		int i = 0;
		int j = j / i;
	}

	{
		class _CBase
		{
		public:
			_CBase(_CBase* pDerived) { m_pDerived = pDerived; }
			~_CBase() { m_pDerived->func(); }
			virtual void func(void) = 0;

		protected:
			_CBase* m_pDerived;
		};

		class _CDerived : public _CBase
		{
		public:
			_CDerived() : _CBase(this) {}
			virtual void func(void) {}
		};

		_CDerived();
	}

}
