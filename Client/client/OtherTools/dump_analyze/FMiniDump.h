

#pragma once


struct DUMP_MODULE
{
    DUMP_MODULE()
        : m_uBaseAddr(0), m_uImageSize(0), m_bImageUnmatched(FALSE),
          m_bPdbUnmatched(FALSE), m_bNoSymbolInfo(FALSE), m_pVersionInfo(NULL)
    {
    }

    ULONG64 m_uBaseAddr;        // Base address
    ULONG64 m_uImageSize;       // Size of module
    CString m_sModuleName;      // Module name  
	std::string	m_strModuleName;
    CString m_sImageName;       // The image name. The name may or may not contain a full path. 
    CString m_sLoadedImageName; // The full path and file name of the file from which symbols were loaded. 
    CString m_sLoadedPdbName;   // The full path and file name of the .pdb file.     
    BOOL m_bImageUnmatched;     // If TRUE than there wasn't matching binary found.
    BOOL m_bPdbUnmatched;       // If TRUE than there wasn't matching PDB file found.
    BOOL m_bNoSymbolInfo;       // If TRUE than no symbols were generated for this module.
    VS_FIXEDFILEINFO* m_pVersionInfo; // Version info for module.
};

struct DUMP_STACK
{
    DUMP_STACK()
    {
        m_dwAddrPCOffset = 0;
        m_nModuleRowID = -1;
        m_dw64OffsInSymbol = 0;
        m_nSrcLineNumber = -1;
    }

    DWORD64 m_dwAddrPCOffset; 
    int m_nModuleRowID;         // ROWID of the record in CPR_MDMP_MODULES table.
    CString m_sSymbolName;      // Name of symbol
    DWORD64 m_dw64OffsInSymbol; // Offset in symbol
    CString m_sSrcFileName;     // Name of source file
    int m_nSrcLineNumber;       // Line number in the source file
};

struct DUMP_THREAD
{
    DUMP_THREAD()
    {
        m_dwThreadId = 0;
        m_pThreadContext = NULL;
        m_bStackWalk = FALSE;
    }

    DWORD m_dwThreadId;        // Thread ID.
    CONTEXT* m_pThreadContext; // Thread context
    BOOL m_bStackWalk;         // Was stack trace retrieved for this thread?
    std::vector< DUMP_STACK > m_StackTrace; // Stack trace for this thread.
};

struct DUMP_MEMORY
{
    DUMP_MEMORY() : m_u64StartOfMemoryRange(0), m_uDataSize(0), m_pStartPtr(NULL) {}

    ULONG64 m_u64StartOfMemoryRange; // Starting address
    ULONG32 m_uDataSize;             // Size of data
    LPVOID m_pStartPtr;              // Pointer to the memrange data stored in minidump
};

struct DUMP_INFO
{   
    DUMP_INFO()
    {
        m_hProcess = INVALID_HANDLE_VALUE;
        m_uProcessorArchitecture = 0;
        m_uchNumberOfProcessors = 0;
        m_verExe = VS_FIXEDFILEINFO();
        m_uchProductType = 0;
        m_ulVerMajor = 0;
        m_ulVerMinor = 0;
        m_ulVerBuild = 0;    
        m_uExceptionCode = 0;
        m_uExceptionAddress = 0;
        m_uExceptionThreadId = 0;
        m_pExceptionThreadContext = NULL;
		m_pExceptionModule = NULL;
		m_uExceptionOffsetFromModuleBase = 0;
    }

    HANDLE m_hProcess;                  // Process ID

    USHORT m_uProcessorArchitecture;    // CPU architecture（PROCESSOR_ARCHITECTURE_INTEL）
    UCHAR  m_uchNumberOfProcessors;     // Number of processors
    UCHAR  m_uchProductType;            // Type of machine (workstation, server, ...)
    ULONG  m_ulVerMajor;                // OS major version number
    ULONG  m_ulVerMinor;                // OS minor version number
    ULONG  m_ulVerBuild;                // OS build number
    CString m_sCSDVer;                  // The latest service pack installed
	VS_FIXEDFILEINFO m_verExe;			// exe file ver

    ULONG32 m_uExceptionCode;           // Structured exception's code
    ULONG64 m_uExceptionAddress;        // Exception address
    ULONG32 m_uExceptionThreadId;       // Exceptions thread ID 
    CONTEXT* m_pExceptionThreadContext; // Thread context
	DUMP_MODULE* m_pExceptionModule;	// Exception module;
	ULONG64 m_uExceptionOffsetFromModuleBase; // Exception offset address from module base


    std::vector< DUMP_THREAD > m_Threads;       // The list of threads.
    std::map< DWORD, size_t > m_ThreadIndex;    // <thread_id, thread_entry_index> pairs
    std::vector< DUMP_MODULE > m_Modules;       // The list of loaded modules.
    std::map< DWORD64, size_t > m_ModuleIndex;  // <base_addr, module_entry_index> pairs
    std::vector< DUMP_MEMORY > m_MemRanges;     // The list of memory ranges.  
};

class DUMP_LOG
{
public:
	DUMP_LOG(){}
	virtual~DUMP_LOG(){}

	virtual void OnLog(const TCHAR* str) = 0;
};

class FMiniDump
{
public:
    FMiniDump();
    ~FMiniDump();

    bool Open(const TCHAR* sFileName, const TCHAR* sSymSearchPath, DUMP_LOG* lg = 0, bool full_analyze = false);
    void Close();
	bool IsOpen(){ return nDump.m_hProcess != 0; }
    int StackWalk(DWORD dwThreadId);  
    BOOL CheckDbgHelpApiVersion();
    int GetModuleRowIdByBaseAddr(DWORD64 dwBaseAddr);
    int GetModuleRowIdByAddress(DWORD64 dwAddress);
    int GetThreadRowIdByThreadId(DWORD dwThreadId);
	BOOL RVA2String(LPVOID pStartAddr, DWORD rva, CString& out);
    LPVOID GetMappedRange(ULONG64 offset, ULONG64 size) const;
    BOOL ContainsMappedRange(const void* pointer, ULONG64 size) const;
    BOOL ReadDumpStream(ULONG streamType, LPVOID& start, ULONG& size) const;
	const char* GetVerString(CString& str);
	int ReadSysinfo();
	int ReadException();
	int ReadModule(bool olny_exe = false);
	int ReadMemory();
	int ReadThread();
	void GetExceptionModuleInfo();

    DUMP_INFO nDump;
    BOOL bIsReadSysinfo;
    BOOL bIsReadException;
    BOOL bIsReadModule;
    BOOL bIsReadMemory;
    BOOL bIsReadThread;
	BOOL bIsReadExceptionModule;

    CString szFilename;         // dmp文件路径
    CString szSymSearchPath;    // pdb和exe搜索路径
    HANDLE hFileMiniDump;       // dmp文件句柄
    HANDLE hFileMapping;        // dmp memory mapping object
    ULONG64 MiniDumpSize;
    LPVOID lpMiniDumpAddress;   // memory mapping
	DUMP_LOG* lpLog;

    friend BOOL CALLBACK ReadProcessMemoryProc64(HANDLE hProcess, DWORD64 lpBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);
    static FMiniDump* LPTHIS;
};

