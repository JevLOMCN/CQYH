

#include "stdafx.h"
#include <dbgHelp.h>
#include <vector>
#include <map>
#include <limits.h>
#include "FMiniDump.h"


#pragma comment(lib, "dbghelp.lib")


FMiniDump* FMiniDump::LPTHIS = 0;


extern BOOL    CALLBACK ReadProcessMemoryProc64(HANDLE hProcess, DWORD64 lpBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);
extern PVOID   CALLBACK FunctionTableAccessProc64(HANDLE hProcess, DWORD64 AddrBase);
extern DWORD64 CALLBACK GetModuleBaseProc64(HANDLE hProcess, DWORD64 Address);


FMiniDump::FMiniDump()
	:lpLog(0)
{
    nDump.m_hProcess = 0;
    bIsReadSysinfo = FALSE;
    bIsReadException = FALSE;
    bIsReadModule = FALSE;
    bIsReadMemory = FALSE;
    bIsReadThread = FALSE;
	bIsReadExceptionModule = FALSE;
    hFileMiniDump = INVALID_HANDLE_VALUE;
    hFileMapping = NULL;
    lpMiniDumpAddress = NULL;
    MiniDumpSize = 0;
}

FMiniDump::~FMiniDump()
{
    Close();
}

bool FMiniDump::Open(const TCHAR* sFileName, const TCHAR* sSymSearchPath, DUMP_LOG* lg, bool full_analyze)
{
    if (nDump.m_hProcess)
        return false;

    szFilename = sFileName;
    szSymSearchPath = sSymSearchPath;
	lpLog = lg;

    hFileMiniDump = CreateFile(sFileName, FILE_ALL_ACCESS, 0, NULL, OPEN_EXISTING, NULL, NULL);
    if(hFileMiniDump==INVALID_HANDLE_VALUE)
    {
        DWORD id = GetLastError();
        Close();
        return false;
    }

    LARGE_INTEGER fileSize = {0};
    if (!GetFileSizeEx(hFileMiniDump, &fileSize) || fileSize.QuadPart < (LONGLONG)sizeof(MINIDUMP_HEADER) ||
        (ULONG64)fileSize.QuadPart > (ULONG64)(SIZE_T)-1)
    {
        Close();
        return false;
    }
    MiniDumpSize = (ULONG64)fileSize.QuadPart;

    hFileMapping = CreateFileMapping(hFileMiniDump, NULL, PAGE_READONLY, 0, 0, 0);
    if(hFileMapping==NULL)
    {
        Close();
        return false;
    }

    lpMiniDumpAddress = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if(lpMiniDumpAddress==NULL)
    {    
        Close();
        return false;
    }

    MINIDUMP_HEADER* header = (MINIDUMP_HEADER*)GetMappedRange(0, sizeof(MINIDUMP_HEADER));
    if (!header || header->Signature != MINIDUMP_SIGNATURE ||
        (header->Version & 0xffff) != MINIDUMP_VERSION ||
        !GetMappedRange(header->StreamDirectoryRva, (ULONG64)header->NumberOfStreams * sizeof(MINIDUMP_DIRECTORY)))
    {
        Close();
        return false;
    }

    static DWORD dwProcessID = 0;
    nDump.m_hProcess = (HANDLE)(++dwProcessID);  

    DWORD dwOptions = 0;
    dwOptions |= SYMOPT_EXACT_SYMBOLS; // Do not load an unmatched .pdb file. 
    dwOptions |= SYMOPT_FAIL_CRITICAL_ERRORS; // Do not display system dialog boxes when there is a media failure such as no media in a drive.
    dwOptions |= SYMOPT_UNDNAME; // All symbols are presented in undecorated form.   
    SymSetOptions(dwOptions);

    BOOL bSymInit = SymInitializeW(nDump.m_hProcess, CStringW(sSymSearchPath), FALSE);
    if(!bSymInit)
    {
        nDump.m_hProcess = NULL;
        Close();
        return false;
    }


	if (full_analyze)
	{
		bIsReadSysinfo = !ReadSysinfo();  
		bIsReadModule = !ReadModule();
		bIsReadThread = !ReadThread();
		bIsReadMemory = !ReadMemory();    
		bIsReadException = !ReadException();
		GetExceptionModuleInfo();
	}
    return true;
}

void FMiniDump::Close()
{
    UnmapViewOfFile(lpMiniDumpAddress);
	lpMiniDumpAddress = 0;
    MiniDumpSize = 0;
    lpLog = NULL;
    if(hFileMapping!=NULL)
        CloseHandle(hFileMapping);
	hFileMapping = 0;
    if(hFileMiniDump!=INVALID_HANDLE_VALUE)
        CloseHandle(hFileMiniDump);
	hFileMiniDump = INVALID_HANDLE_VALUE;
    if(nDump.m_hProcess!=NULL)
        SymCleanup(nDump.m_hProcess);
	nDump = DUMP_INFO();
    nDump.m_hProcess = 0;
	if (LPTHIS == this)
		LPTHIS = NULL;

	bIsReadSysinfo = FALSE;
    bIsReadException = FALSE;
    bIsReadModule = FALSE;
    bIsReadMemory = FALSE;
    bIsReadThread = FALSE;
	bIsReadExceptionModule = FALSE;
}

BOOL FMiniDump::CheckDbgHelpApiVersion()
{
    // Set valid dbghelp API version
    API_VERSION CompiledApiVer;
    CompiledApiVer.MajorVersion = 6;
    CompiledApiVer.MinorVersion = 1;
    CompiledApiVer.Revision = 11;    
    CompiledApiVer.Reserved = 0;
    LPAPI_VERSION pActualApiVer = ImagehlpApiVersionEx(&CompiledApiVer);    
    if(CompiledApiVer.MajorVersion!=pActualApiVer->MajorVersion ||
        CompiledApiVer.MinorVersion!=pActualApiVer->MinorVersion ||
        CompiledApiVer.Revision!=pActualApiVer->Revision)
    {     
        return FALSE;
    }

    return TRUE;
}

LPVOID FMiniDump::GetMappedRange(ULONG64 offset, ULONG64 size) const
{
    if (!lpMiniDumpAddress || offset > MiniDumpSize || size > MiniDumpSize - offset ||
        offset > (ULONG64)(ULONG_PTR)-1 - (ULONG_PTR)lpMiniDumpAddress)
        return NULL;
    return (LPBYTE)lpMiniDumpAddress + (SIZE_T)offset;
}

BOOL FMiniDump::ContainsMappedRange(const void* pointer, ULONG64 size) const
{
    if (!pointer || (ULONG_PTR)pointer < (ULONG_PTR)lpMiniDumpAddress)
        return FALSE;
    return GetMappedRange((ULONG_PTR)pointer - (ULONG_PTR)lpMiniDumpAddress, size) == pointer;
}

BOOL FMiniDump::ReadDumpStream(ULONG streamType, LPVOID& start, ULONG& size) const
{
    start = NULL;
    size = 0;
    const MINIDUMP_HEADER* header = (const MINIDUMP_HEADER*)GetMappedRange(0, sizeof(MINIDUMP_HEADER));
    if (!header || header->Signature != MINIDUMP_SIGNATURE || (header->Version & 0xffff) != MINIDUMP_VERSION)
        return FALSE;
    const MINIDUMP_DIRECTORY* directory = (const MINIDUMP_DIRECTORY*)GetMappedRange(
        header->StreamDirectoryRva, (ULONG64)header->NumberOfStreams * sizeof(MINIDUMP_DIRECTORY));
    if (!directory)
        return FALSE;
    for (ULONG i = 0; i < header->NumberOfStreams; ++i)
    {
        if (directory[i].StreamType != streamType)
            continue;
        LPVOID data = GetMappedRange(directory[i].Location.Rva, directory[i].Location.DataSize);
        if (!data)
            return FALSE;
        start = data;
        size = directory[i].Location.DataSize;
        return TRUE;
    }
    return FALSE;
}

BOOL FMiniDump::RVA2String(LPVOID start_addr, DWORD rva, CString& out)
{
    out.Empty();
    if (start_addr != lpMiniDumpAddress || rva % sizeof(WCHAR))
        return FALSE;
    const ULONG32* length = (const ULONG32*)GetMappedRange(rva, sizeof(ULONG32));
    if (!length || *length % sizeof(WCHAR) || *length / sizeof(WCHAR) > INT_MAX)
        return FALSE;
    const WCHAR* text = (const WCHAR*)GetMappedRange((ULONG64)rva + FIELD_OFFSET(MINIDUMP_STRING, Buffer), *length);
    if (!text)
        return FALSE;
    out = CString(text, (int)(*length / sizeof(WCHAR)));
    return TRUE;
}

const char* FMiniDump::GetVerString(CString& str)
{
	str.Format("%d.%d.%d.%d",
		(int)(nDump.m_verExe.dwFileVersionMS>>16),
		(int)(nDump.m_verExe.dwFileVersionMS&0xffff),
		(int)(nDump.m_verExe.dwFileVersionLS>>16),
		(int)(nDump.m_verExe.dwFileVersionLS&0xffff)
		);
	return str;
}

int FMiniDump::ReadSysinfo()
{
    LPVOID start = NULL;
    ULONG size = 0;
    if (!ReadDumpStream(SystemInfoStream, start, size) || size < sizeof(MINIDUMP_SYSTEM_INFO))
        return 1;
    const MINIDUMP_SYSTEM_INFO* info = (const MINIDUMP_SYSTEM_INFO*)start;
    CString servicePack;
    if (info->CSDVersionRva && !RVA2String(lpMiniDumpAddress, info->CSDVersionRva, servicePack))
        return 1;
    nDump.m_uProcessorArchitecture = info->ProcessorArchitecture;
    nDump.m_uchNumberOfProcessors = info->NumberOfProcessors;
    nDump.m_uchProductType = info->ProductType;
    nDump.m_ulVerMajor = info->MajorVersion;
    nDump.m_ulVerMinor = info->MinorVersion;
    nDump.m_ulVerBuild = info->BuildNumber;
    nDump.m_sCSDVer = servicePack;
    return 0;
}

int FMiniDump::ReadException()
{
    LPVOID pStreamStart = NULL;
    ULONG uStreamSize = 0;
    MINIDUMP_DIRECTORY* pmd = NULL;
    BOOL bRead = FALSE;

    bRead = ReadDumpStream(ExceptionStream, pStreamStart, uStreamSize);
    if (!bRead || uStreamSize < sizeof(MINIDUMP_EXCEPTION_STREAM))
        return 1;
    const MINIDUMP_EXCEPTION_STREAM* exception = (const MINIDUMP_EXCEPTION_STREAM*)pStreamStart;
    if (exception->ThreadContext.DataSize && (exception->ThreadContext.DataSize < sizeof(CONTEXT) ||
        !GetMappedRange(exception->ThreadContext.Rva, exception->ThreadContext.DataSize)))
        return 1;
    if(bRead)
    {
        MINIDUMP_EXCEPTION_STREAM* pExceptionStream = (MINIDUMP_EXCEPTION_STREAM*)pStreamStart;
        if(pExceptionStream!=NULL && 
            uStreamSize>=sizeof(MINIDUMP_EXCEPTION_STREAM))
        {
            nDump.m_uExceptionThreadId = pExceptionStream->ThreadId;
            nDump.m_uExceptionCode = pExceptionStream->ExceptionRecord.ExceptionCode;
            nDump.m_uExceptionAddress = pExceptionStream->ExceptionRecord.ExceptionAddress;          
            nDump.m_pExceptionThreadContext = 
                pExceptionStream->ThreadContext.DataSize ?
                (CONTEXT*)GetMappedRange(pExceptionStream->ThreadContext.Rva, pExceptionStream->ThreadContext.DataSize) : NULL;

			if (lpLog)
			{
				CString sMsg;
				int nExcModuleRowID = GetModuleRowIdByAddress(nDump.m_uExceptionAddress);
				if(nExcModuleRowID>=0)
				{
					sMsg.Format(_T("Unhandled exception at 0x%I64x in %s: 0x%x : %s"),
						nDump.m_uExceptionAddress,
						nDump.m_Modules[nExcModuleRowID].m_sModuleName,
						nDump.m_uExceptionCode,
						_T("Exception description.")
						);
				}
				else
				{
				}
				lpLog->OnLog(sMsg);
			}
        }    
    }
    else
    {
		if (lpLog)
			lpLog->OnLog(_T("No exception information found in minidump."));
        return 1;
    }

    return 0;
}

int FMiniDump::ReadModule(bool olny_exe)
{
    LPVOID pStreamStart = NULL;
    ULONG uStreamSize = 0;
    MINIDUMP_DIRECTORY* pmd = NULL;
    BOOL bRead = FALSE;

    bRead = ReadDumpStream(ModuleListStream, pStreamStart, uStreamSize);
    const ULONG prefix = FIELD_OFFSET(MINIDUMP_MODULE_LIST, Modules);
    if (!bRead || uStreamSize < prefix)
        return 1;
    const MINIDUMP_MODULE_LIST* modules = (const MINIDUMP_MODULE_LIST*)pStreamStart;
    if (modules->NumberOfModules > (uStreamSize - prefix) / sizeof(MINIDUMP_MODULE))
        return 1;
    nDump.m_verExe = VS_FIXEDFILEINFO();
    if (!olny_exe)
    {
        nDump.m_pExceptionModule = NULL;
        bIsReadExceptionModule = FALSE;
        nDump.m_Modules.clear();
        nDump.m_ModuleIndex.clear();
    }
    if(bRead)
    {
        MINIDUMP_MODULE_LIST* pModuleStream = (MINIDUMP_MODULE_LIST*)pStreamStart;
        if(pModuleStream!=NULL)
        {
            ULONG32 uNumberOfModules = pModuleStream->NumberOfModules;
            ULONG32 i;
            for(i=0; i<uNumberOfModules; i++)
            {
                MINIDUMP_MODULE* pModule = (MINIDUMP_MODULE*)((LPBYTE)pModuleStream->Modules+i*sizeof(MINIDUMP_MODULE));

                CString sModuleName;
                if (!RVA2String(lpMiniDumpAddress, pModule->ModuleNameRva, sModuleName))
                    return 1;
                if (sModuleName.Find(".exe") != -1)
                {
                    nDump.m_verExe = pModule->VersionInfo;
                    if (olny_exe)
                        return 0;
                }
                if (olny_exe)
                    continue;

				DWORD64 dwBaseAddr = pModule->BaseOfImage;
				DWORD64 dwImageSize = pModule->SizeOfImage;
                CString sShortModuleName = sModuleName;
                int pos = -1;
                pos = sModuleName.ReverseFind('\\');
                if(pos>=0)
                    sShortModuleName = sShortModuleName.Mid(pos+1);

                CStringW wcsModName(sModuleName);

                DWORD64 rrrtt = SymLoadModuleExW(nDump.m_hProcess, NULL, wcsModName, 0, dwBaseAddr, (DWORD)dwImageSize, NULL, 0);
                DWORD err_id = GetLastError();

                IMAGEHLP_MODULE64 modinfo;
                memset(&modinfo, 0, sizeof(IMAGEHLP_MODULE64));
                modinfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
                BOOL bModuleInfo = SymGetModuleInfo64(nDump.m_hProcess, dwBaseAddr, &modinfo);                DUMP_MODULE m;
                if(!bModuleInfo)
                {          
                    m.m_bImageUnmatched = TRUE;
                    m.m_bNoSymbolInfo = TRUE;
                    m.m_bPdbUnmatched = TRUE;
                    m.m_pVersionInfo = NULL;
                    m.m_sImageName = sModuleName;
					m.m_strModuleName = sShortModuleName.GetBuffer();
                    m.m_sModuleName = sShortModuleName;
                    m.m_uBaseAddr = dwBaseAddr;
                    m.m_uImageSize = dwImageSize;          
                }
                else
                {          
                    m.m_uBaseAddr = modinfo.BaseOfImage;
                    m.m_uImageSize = modinfo.ImageSize;        
                    m.m_sModuleName = sShortModuleName;
					m.m_strModuleName = sShortModuleName.GetBuffer();
                    m.m_sImageName = modinfo.ImageName;
                    m.m_sLoadedImageName = modinfo.LoadedImageName;
                    m.m_sLoadedPdbName = modinfo.LoadedPdbName;
                    m.m_pVersionInfo = &pModule->VersionInfo;
                    m.m_bPdbUnmatched = modinfo.PdbUnmatched;          
                    BOOL bTimeStampMatched = pModule->TimeDateStamp == modinfo.TimeDateStamp;
                    m.m_bImageUnmatched = !bTimeStampMatched;
                    m.m_bNoSymbolInfo = !modinfo.GlobalSymbols;
                }        

                nDump.m_Modules.push_back(m);
                nDump.m_ModuleIndex[m.m_uBaseAddr] = nDump.m_Modules.size()-1;          

				if (lpLog)
				{
					CString sMsg;
					if(m.m_bImageUnmatched)
						sMsg.Format(_T("Loaded '*%s'"), sModuleName);
					else
						sMsg.Format(_T("Loaded '%s'"), m.m_sLoadedImageName);

					if(m.m_bImageUnmatched)
						sMsg += _T(", No matching binary found.");          
					else if(m.m_bPdbUnmatched)
						sMsg += _T(", No matching PDB file found.");          
					else
					{
						if(m.m_bNoSymbolInfo)            
							sMsg += _T(", No symbols loaded.");          
						else
							sMsg += _T(", Symbols loaded.");          
					}
					lpLog->OnLog(sMsg);
				}
            }
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

int FMiniDump::GetModuleRowIdByBaseAddr(DWORD64 dwBaseAddr)
{
    std::map<DWORD64, size_t>::iterator it = nDump.m_ModuleIndex.find(dwBaseAddr);
    if(it!=nDump.m_ModuleIndex.end())
        return (int)it->second;
    return -1;
}

int FMiniDump::GetModuleRowIdByAddress(DWORD64 dwAddress)
{
    UINT i;
    for(i=0;i<nDump.m_Modules.size();i++)
    {
        if(nDump.m_Modules[i].m_uBaseAddr<=dwAddress && 
            dwAddress - nDump.m_Modules[i].m_uBaseAddr < nDump.m_Modules[i].m_uImageSize)
            return i;
    }

    return -1;
}

int FMiniDump::GetThreadRowIdByThreadId(DWORD dwThreadId)
{
    std::map<DWORD, size_t>::iterator it = nDump.m_ThreadIndex.find(dwThreadId);
    if(it!=nDump.m_ThreadIndex.end())
        return (int)it->second;
    return -1;
}

int FMiniDump::ReadMemory()
{
    LPVOID start = NULL;
    ULONG size = 0;
    const ULONG prefix = FIELD_OFFSET(MINIDUMP_MEMORY_LIST, MemoryRanges);
    if (!ReadDumpStream(MemoryListStream, start, size) || size < prefix)
        return 1;
    const MINIDUMP_MEMORY_LIST* list = (const MINIDUMP_MEMORY_LIST*)start;
    if (list->NumberOfMemoryRanges > (size - prefix) / sizeof(MINIDUMP_MEMORY_DESCRIPTOR))
        return 1;
    std::vector<DUMP_MEMORY> ranges;
    for (ULONG i = 0; i < list->NumberOfMemoryRanges; ++i)
    {
        const MINIDUMP_MEMORY_DESCRIPTOR& descriptor = list->MemoryRanges[i];
        LPVOID data = GetMappedRange(descriptor.Memory.Rva, descriptor.Memory.DataSize);
        if (!data)
            return 1;
        DUMP_MEMORY range;
        range.m_u64StartOfMemoryRange = descriptor.StartOfMemoryRange;
        range.m_uDataSize = descriptor.Memory.DataSize;
        range.m_pStartPtr = data;
        ranges.push_back(range);
    }
    nDump.m_MemRanges.swap(ranges);
    return 0;
}

void FMiniDump::GetExceptionModuleInfo()
{
    if (!bIsReadException || !bIsReadModule || bIsReadExceptionModule)
        return;
    nDump.m_pExceptionModule = NULL;
    nDump.m_uExceptionOffsetFromModuleBase = 0;
    const int index = GetModuleRowIdByAddress(nDump.m_uExceptionAddress);
    if (index >= 0)
    {
        nDump.m_pExceptionModule = &nDump.m_Modules[index];
        nDump.m_uExceptionOffsetFromModuleBase = nDump.m_uExceptionAddress - nDump.m_pExceptionModule->m_uBaseAddr;
    }
    bIsReadExceptionModule = TRUE;
}

int FMiniDump::ReadThread()
{
    LPVOID start = NULL;
    ULONG size = 0;
    const ULONG prefix = FIELD_OFFSET(MINIDUMP_THREAD_LIST, Threads);
    if (!ReadDumpStream(ThreadListStream, start, size) || size < prefix)
        return 1;
    const MINIDUMP_THREAD_LIST* list = (const MINIDUMP_THREAD_LIST*)start;
    if (list->NumberOfThreads > (size - prefix) / sizeof(MINIDUMP_THREAD))
        return 1;
    std::vector<DUMP_THREAD> threads;
    std::map<DWORD, size_t> index;
    for (ULONG i = 0; i < list->NumberOfThreads; ++i)
    {
        const MINIDUMP_THREAD& descriptor = list->Threads[i];
        DUMP_THREAD thread;
        thread.m_dwThreadId = descriptor.ThreadId;
        if (descriptor.ThreadContext.DataSize)
        {
            if (descriptor.ThreadContext.DataSize < sizeof(CONTEXT))
                return 1;
            thread.m_pThreadContext = (CONTEXT*)GetMappedRange(descriptor.ThreadContext.Rva, descriptor.ThreadContext.DataSize);
            if (!thread.m_pThreadContext)
                return 1;
        }
        threads.push_back(thread);
        index[thread.m_dwThreadId] = threads.size() - 1;
    }
    nDump.m_Threads.swap(threads);
    nDump.m_ThreadIndex.swap(index);
    return 0;
}

int FMiniDump::StackWalk(DWORD dwThreadId)
{ 
    int nThreadIndex = GetThreadRowIdByThreadId(dwThreadId);
    if (nThreadIndex < 0 || (size_t)nThreadIndex >= nDump.m_Threads.size())
        return 0;
    if(nDump.m_Threads[nThreadIndex].m_bStackWalk == TRUE)
        return 0; // Already done

    CONTEXT* pThreadContext = NULL;

    if(nDump.m_Threads[nThreadIndex].m_dwThreadId==nDump.m_uExceptionThreadId)
        pThreadContext = nDump.m_pExceptionThreadContext;
    else
        pThreadContext = nDump.m_Threads[nThreadIndex].m_pThreadContext;  

    if (!ContainsMappedRange(pThreadContext, sizeof(CONTEXT)))
        return 1;

    // Make modifiable context
    CONTEXT Context;
    memcpy(&Context, pThreadContext, sizeof(CONTEXT));

    LPTHIS = this;

    // Init stack frame with correct initial values
    // See this:
    // http://www.codeproject.com/KB/threads/StackWalker.aspx
    //
    // Given a current dbghelp, your code should:
    //  1. Always use StackWalk64
    //  2. Always set AddrPC to the current instruction pointer (Eip on x86, Rip on x64 and StIIP on IA64)
    //  3. Always set AddrStack to the current stack pointer (Esp on x86, Rsp on x64 and IntSp on IA64)
    //  4. Set AddrFrame to the current frame pointer when meaningful. On x86 this is Ebp, on x64 you 
    //     can use Rbp (but is not used by VC2005B2; instead it uses Rdi!) and on IA64 you can use RsBSP. 
    //     StackWalk64 will ignore the value when it isn't needed for unwinding.
    //  5. Set AddrBStore to RsBSP for IA64. 

    STACKFRAME64 sf;
    memset(&sf, 0, sizeof(STACKFRAME64));

    sf.AddrPC.Mode = AddrModeFlat;  
    sf.AddrFrame.Mode = AddrModeFlat;  
    sf.AddrStack.Mode = AddrModeFlat;  
    sf.AddrBStore.Mode = AddrModeFlat;  

    DWORD dwMachineType = 0;
    switch(nDump.m_uProcessorArchitecture)
    {
#ifdef _X86_
  case PROCESSOR_ARCHITECTURE_INTEL: 
      dwMachineType = IMAGE_FILE_MACHINE_I386;
      sf.AddrPC.Offset = Context.Eip;
      sf.AddrStack.Offset = Context.Esp;
      sf.AddrFrame.Offset = Context.Ebp;
      break;
#endif
#ifdef _AMD64_
  case PROCESSOR_ARCHITECTURE_AMD64:
      dwMachineType = IMAGE_FILE_MACHINE_AMD64;
      sf.AddrPC.Offset = Context.Rip;
      sf.AddrStack.Offset = Context.Rsp;
      sf.AddrFrame.Offset = Context.Rbp;
      break;
#endif
#ifdef _IA64_
  case PROCESSOR_ARCHITECTURE_AMD64:
      dwMachineType = IMAGE_FILE_MACHINE_IA64;
      sf.AddrPC.Offset = Context.StIIP;
      sf.AddrStack.Offset = Context.IntSp;
      sf.AddrFrame.Offset = Context.RsBSP;
      sf.AddrBStore.Offset = Context.RsBSP;
      break;
#endif 
  default:
      {
          return 1; // Unsupported architecture
      }
    }

    for(;;)
    {    
        BOOL bWalk = ::StackWalk64(
            dwMachineType,               // machine type
            nDump.m_hProcess,       // our process handle
            (HANDLE)dwThreadId,          // thread ID
            &sf,                         // stack frame
            dwMachineType==IMAGE_FILE_MACHINE_I386?NULL:(&Context), // used for non-I386 machines 
            ReadProcessMemoryProc64,     // our routine
            FunctionTableAccessProc64,   // our routine
            GetModuleBaseProc64,         // our routine
            NULL                         // safe to be NULL
            );

        if(!bWalk)
            break;      

        DUMP_STACK stack_frame;
        stack_frame.m_dwAddrPCOffset = sf.AddrPC.Offset;

        // Get module info
        IMAGEHLP_MODULE64 mi;
        memset(&mi, 0, sizeof(IMAGEHLP_MODULE64));
        mi.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
        BOOL bGetModuleInfo = SymGetModuleInfo64(nDump.m_hProcess, sf.AddrPC.Offset, &mi);     
        if(bGetModuleInfo)
        {
            stack_frame.m_nModuleRowID = GetModuleRowIdByBaseAddr(mi.BaseOfImage);      
        }

        // Get symbol info
        DWORD64 dwDisp64 = 0;
        __declspec(align(8)) BYTE buffer[4096] = {0};
        SYMBOL_INFO* sym_info = (SYMBOL_INFO*)buffer;
        sym_info->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym_info->MaxNameLen = 4096-sizeof(SYMBOL_INFO)-1;
        BOOL bGetSym = SymFromAddr(nDump.m_hProcess, sf.AddrPC.Offset, &dwDisp64, sym_info);

        if(bGetSym)
        {
            stack_frame.m_sSymbolName = CString(sym_info->Name, sym_info->NameLen);
            stack_frame.m_dw64OffsInSymbol = dwDisp64;
        }

        // Get source filename and line
        DWORD dwDisplacement = 0;
        IMAGEHLP_LINE64 line = {0};
        line.SizeOfStruct = sizeof(line);
        BOOL bGetLine = SymGetLineFromAddr64(nDump.m_hProcess, sf.AddrPC.Offset, &dwDisplacement, &line);

        if(bGetLine)
        {
            stack_frame.m_sSrcFileName = line.FileName;
            stack_frame.m_nSrcLineNumber = line.LineNumber;
        }

        nDump.m_Threads[nThreadIndex].m_StackTrace.push_back(stack_frame);
    }

    CString sStackTrace;
    UINT i;
    for(i=0; i<nDump.m_Threads[nThreadIndex].m_StackTrace.size(); i++)
    {
        DUMP_STACK& frame = nDump.m_Threads[nThreadIndex].m_StackTrace[i];

        if(frame.m_sSymbolName.IsEmpty())
            continue;

        CString sModuleName;
        CString sAddrPCOffset;
        CString sSymbolName;            
        CString sOffsInSymbol;
        CString sSourceFile;
        CString sSourceLine;

        if(frame.m_nModuleRowID>=0)
        {
            sModuleName = nDump.m_Modules[frame.m_nModuleRowID].m_sModuleName;
        }           

        sSymbolName = frame.m_sSymbolName;
        sAddrPCOffset.Format(_T("0x%I64x"), frame.m_dwAddrPCOffset);
        sSourceFile = frame.m_sSrcFileName;
        sSourceLine.Format(_T("%d"), frame.m_nSrcLineNumber);
        sOffsInSymbol.Format(_T("0x%I64x"), frame.m_dw64OffsInSymbol);

        CString str;
        str = sModuleName;
        if(!str.IsEmpty())
            str += _T("!");

        if(sSymbolName.IsEmpty())
            str += sAddrPCOffset;  
        else
        {
            str += sSymbolName;
            str += _T("+");
            str += sOffsInSymbol;
        }

        if(!sSourceFile.IsEmpty())
        {
            size_t pos = sSourceFile.ReverseFind('\\');
            if(pos>=0)
                sSourceFile = sSourceFile.Mid((int)pos+1);
            str += _T(" [ ");
            str += sSourceFile;
            str += _T(": ");
            str += sSourceLine;
            str += _T(" ] ");
        } 

        sStackTrace += str; 
        sStackTrace += _T("\n");
    }

    nDump.m_Threads[nThreadIndex].m_bStackWalk = TRUE;

    return 0;
}

BOOL CALLBACK ReadProcessMemoryProc64(HANDLE hProcess, DWORD64 lpBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead)
{
    if (!lpNumberOfBytesRead)
        return FALSE;
    *lpNumberOfBytesRead = 0;

    if (!FMiniDump::LPTHIS || hProcess != FMiniDump::LPTHIS->nDump.m_hProcess || !lpBaseAddress || !lpBuffer || !nSize)
        return FALSE;

    for (ULONG i = 0; i < FMiniDump::LPTHIS->nDump.m_MemRanges.size(); i++)
    {
        DUMP_MEMORY& mr = FMiniDump::LPTHIS->nDump.m_MemRanges[i];
        if (FMiniDump::LPTHIS->ContainsMappedRange(mr.m_pStartPtr, mr.m_uDataSize) &&
            lpBaseAddress >= mr.m_u64StartOfMemoryRange &&
            lpBaseAddress - mr.m_u64StartOfMemoryRange < mr.m_uDataSize)
        {
            DWORD64 dwOffs = lpBaseAddress-mr.m_u64StartOfMemoryRange;
            LONG64 lBytesRead = 0;

            if(mr.m_uDataSize-dwOffs>nSize)
                lBytesRead = nSize;
            else
                lBytesRead = mr.m_uDataSize-dwOffs;

            if(lBytesRead<=0 || nSize<lBytesRead)
                return FALSE;

            *lpNumberOfBytesRead = (DWORD)lBytesRead;
            memcpy(lpBuffer, (LPBYTE)mr.m_pStartPtr+dwOffs, (size_t)lBytesRead);

            return TRUE;
        }
    }

    return FALSE;
}

PVOID CALLBACK FunctionTableAccessProc64(HANDLE hProcess, DWORD64 AddrBase)
{   
    return SymFunctionTableAccess64(hProcess, AddrBase);
}

DWORD64 CALLBACK GetModuleBaseProc64(HANDLE hProcess, DWORD64 Address)
{  
    return SymGetModuleBase64(hProcess, Address);
}