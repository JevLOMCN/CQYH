#include "stdafx.h"
#include "share_mem.h"
#include "cas.h"
#include <chrono>
#include <limits>
#include <memory>
#include <thread>

#ifdef __linux__

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

MemPtr CreateSharedMemory(const char* szName, UINT64 qwSize)
{
	void* mem_ptr = NULL;
	BOOL is_new = FALSE;
	INT32 ret = 0;
	MemPtr ptr;
	ptr.pFd = NULL;
	ptr.pSharedMem = NULL;

    INT32 fd = -1;
	//INT32 fd = shm_open(szName, O_RDWR, 0);
    shm_unlink( szName );

	if(fd < 0)
	{
		is_new = TRUE;
		fd = shm_open(szName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IXUSR);
        if ( fd > 0 )
        {
            ret = ftruncate( fd, qwSize );
            LOG_PROCESS_ERROR( ret == 0 );
        }
	}

	LOG_PROCESS_ERROR(fd);

	if(!is_new)
	{
		struct stat sb;
		ret = fstat(fd, &sb);
		LOG_PROCESS_ERROR(ret >= 0);

		if(qwSize < sb.st_size)
			qwSize = sb.st_size;
	}

	mem_ptr = mmap(NULL, qwSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	LOG_PROCESS_ERROR(mem_ptr != MAP_FAILED); 

    ptr.bIsNew = is_new;
	ptr.pSharedMem = (char *)mem_ptr;
	ptr.pFd = (void*)fd;
	ptr.qwSize = qwSize;
Exit0:
	return ptr;
}

void CloseSharedMemory(MemPtr& ptr)
{
	INT32 nRetCode = 0;

	nRetCode = munmap(ptr.pSharedMem, ptr.qwSize);
	LOG_PROCESS_ERROR(nRetCode >= 0);

	nRetCode = close( (unsigned long int)ptr.pFd );
	LOG_PROCESS_ERROR(nRetCode >= 0);

Exit0:
	return;
}

#else	// windows

#include <winternl.h>

namespace
{
struct MappingHandleCloser
{
    void operator()(void* value) const noexcept { CloseHandle(value); }
};
struct MappingViewCloser
{
    void operator()(void* value) const noexcept { UnmapViewOfFile(value); }
};
using MappingHandle = std::unique_ptr<void, MappingHandleCloser>;
using MappingView = std::unique_ptr<void, MappingViewCloser>;

bool WaitForMappingHeader(ShareMemHeader& header)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (cqyh::SharedLoad32(&header.nTag) != SHARED_MEM_HEAD_TAG)
    {
        if (std::chrono::steady_clock::now() >= deadline)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return true;
}

BOOL AttachMapping(MappingHandle handle, BOOL isNew, UINT64 requestedSize, MemPtr& result)
{
    MappingView view(MapViewOfFile(handle.get(), FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0));
    if (!view)
    {
        ERR("Cannot map shared memory (Windows error=%lu)", GetLastError());
        return FALSE;
    }
    auto& header = *static_cast<ShareMemHeader*>(view.get());
    if (isNew)
    {
        // qwSize is packed and immutable. Publish its initialized value through
        // the aligned 32-bit tag before allowing another process to consume it.
        header.qwSize = requestedSize;
        cqyh::SharedStore32(&header.nTag, SHARED_MEM_HEAD_TAG);
    }
    else if (!WaitForMappingHeader(header))
    {
        ERR("Shared-memory mapping metadata was not published");
        return FALSE;
    }
    const auto payloadSize = header.qwSize;
    MEMORY_BASIC_INFORMATION information{};
    if (!VirtualQuery(view.get(), &information, sizeof(information)) ||
        information.RegionSize < sizeof(ShareMemHeader) ||
        payloadSize == 0 || payloadSize > information.RegionSize - sizeof(ShareMemHeader))
    {
        ERR("Invalid shared-memory mapping size");
        return FALSE;
    }
    result.bIsNew = isNew;
    result.qwSize = payloadSize;
    result.pSharedMem = static_cast<char*>(view.release()) + sizeof(ShareMemHeader);
    result.pFd = handle.release();
    return TRUE;
}
}

BOOL CreateSharedMemory(const char* name, UINT64 size, MemPtr& result)
{
    result = {};
    if (!name || !*name || size == 0 || size > (std::numeric_limits<UINT64>::max)() - sizeof(ShareMemHeader))
    {
        LOG_CHECK_ERROR(name && *name && size > 0 &&
                        size <= (std::numeric_limits<UINT64>::max)() - sizeof(ShareMemHeader));
        return FALSE;
    }
    const auto total = size + sizeof(ShareMemHeader);
    HANDLE raw = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                                  static_cast<DWORD>(total >> 32), static_cast<DWORD>(total), name);
    const auto error = GetLastError();
    MappingHandle handle(raw);
    if (!handle)
    {
        ERR("Cannot create shared memory %s (Windows error=%lu)", name, error);
        return FALSE;
    }
    return AttachMapping(std::move(handle), error != ERROR_ALREADY_EXISTS, size, result);
}

BOOL OpenSharedMemory(const char* name, MemPtr& result)
{
    result = {};
    if (!name || !*name)
    {
        LOG_CHECK_ERROR(name && *name);
        return FALSE;
    }
    MappingHandle handle(OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, name));
    if (!handle)
        return FALSE; // An absent peer is a normal polling result.
    return AttachMapping(std::move(handle), FALSE, 0, result);
}

void CloseSharedMemory(MemPtr& value)
{
    if (value.pSharedMem)
        UnmapViewOfFile(value.pSharedMem - sizeof(ShareMemHeader));
    if (value.pFd)
        CloseHandle(value.pFd);
    value = {};
}

typedef NTSTATUS (__stdcall *NT_QUERYOBJECT)(_In_opt_ HANDLE, _In_ ULONG, _Out_opt_ PVOID, _In_ ULONG, _Out_opt_ PULONG);

void QuerySharedMemory(MemPtr& ptr)
{
	HMODULE hDll = GetModuleHandle("ntdll.dll");


	NT_QUERYOBJECT NtQueryObject = (NT_QUERYOBJECT)GetProcAddress(hDll, "NtQueryObject");

	typedef struct _HANDLE_STATE
	{
		DWORD r1;
		DWORD GrantedAccess;
		DWORD HandleCount;
		DWORD RefCount;
		DWORD r5;
		DWORD r6;
		DWORD r7;
		DWORD r8;
		DWORD r9;
		DWORD r10;
		DWORD r11;
		DWORD r12;
		DWORD r13;
		DWORD r14;
	} HANDLE_STATE;

	HANDLE_STATE st;

	ULONG ulOut;

	DWORD dwRetCode = NtQueryObject(ptr.pSharedMem, 0, &st, 38, &ulOut);

}

BOOL IsShareMemoryExists(const char* szName)
{
	HANDLE hMapFile = NULL;

	hMapFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, szName);
	PROCESS_ERROR(hMapFile);

	CloseHandle(hMapFile);
	return TRUE;
Exit0:
	CloseHandle(hMapFile);
	return FALSE;
}

#endif


