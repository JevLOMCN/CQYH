#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include "thread.h"
#include "circle_list.h"

#define MAX_MODULE_NAME			(256)
#define MAX_LOG_THREAD_COUNT	(64)
enum LOG_OUTPUT_TYPE
{
	lotNone		    = 0x00000000,

	lotConsole	    = 0x00000001,
	lotFile		    = 0x00000002,
	lotTrace	    = 0x00000004,
	lotCustom       = 0x00000008,

	lotAll		    = 0xFFFFFFFF
};

enum LOG_LEVEL_TYPE
{
	lltNone,

	lltDebug        = 0x00000001,
	lltInfo         = 0x00000002,
	lltWarning      = 0x00000004,
    lltError        = 0x00000008,
	lltCritical     = 0x00000010,

	lltSDU			= 0x00000020,
	lltMove			= 0x00000040,
	lltLua			= 0x00000080,
	lltSpell		= 0x00000100,

	lltSys			= 0x00000200,

	lltAll			= 0xFFFFFFFF
};

#if defined(__linux__)
#define DBG(__message__, ...)    CLog::GetInstance().WriteLog(lltDebug, __message__, ##__VA_ARGS__)
#define INF(__message__, ...)    CLog::GetInstance().WriteLog(lltInfo, __message__, ##__VA_ARGS__)
#define WRN(__message__, ...)    CLog::GetInstance().WriteLog(lltWarning, __message__, ##__VA_ARGS__)
#define ERR(__message__, ...)    CLog::GetInstance().WriteLog(lltError, __message__, ##__VA_ARGS__)
#define CRI(__message__, ...)    CLog::GetInstance().WriteLog(lltCritical, __message__, ##__VA_ARGS__)
//#define SPL(__message__, ...)    CLog::GetInstance().WriteLog(lltSpell, __message__, ##__VA_ARGS__)
#define SYST(__message__, ...)   CLog::GetInstance().WriteLog(lltSysSuccess, __message__, ##__VA_ARGS__)
#define SYSF(__message__, ...)   CLog::GetInstance().WriteLog(lltSysError, __message__, ##__VA_ARGS__)
#else   // __linux__
#define DBG    CLog::GetInstance().DbgLog
#define INF    CLog::GetInstance().InfLog
#define WRN    CLog::GetInstance().WrnLog
#define ERR    CLog::GetInstance().ErrLog
#define CRI    CLog::GetInstance().CriLog
#define SYST   CLog::GetInstance().SYSLogT
#define SYSF   CLog::GetInstance().SYSLogF
//#define SPL    CLog::GetInstance().SplLog
#endif  // __linux__

typedef void (*CUSTOM_OUTPUT_FUNC)(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize);

class CLog
{
public:
	CLog();
	virtual ~CLog();

	BOOL Init(const TCHAR* pcszModuleName, INT32 eLogLevelMask, INT32 eLogOutputMask, BOOL bLogThread, BOOL bPrintThreadID, BOOL bFlushFile);
	BOOL UnInit(void);
	static CLog& GetInstance(void);
	const TCHAR* GetModuleName(void);
	const TCHAR* GetLogFileFolder(void);
	const TCHAR* GetLogFilePath(void);

	bool WriteLog(INT32 eLogLevel, const TCHAR* pcszMessage, ...);
	void SetOutputMask(INT32 eLogOutputMask);

	void SetLevelMask(INT32 eLogLevelMask);
	INT32 GetLevelMask(void);

	void SetCustomOutput(CUSTOM_OUTPUT_FUNC pFunc);

	//#if defined(WIN32)
	BOOL DbgLog(const char* pcszMessage, ...);
	BOOL InfLog(const char* pcszMessage, ...);
	BOOL WrnLog(const char* pcszMessage, ...);
    BOOL ErrLog(const char* pcszMessage, ...);
	BOOL CriLog(const char* pcszMessage, ...);
	BOOL SplLog(const char* pcszMessage, ...);
	BOOL SYSLogT(const char* pcszMessage, ...);
	BOOL SYSLogF(const char* pcszMessage, ...);
	//#endif // WIN32

private:
	BOOL _WriteLog(INT32 nLevelIndex, const char* pcszMessage, va_list valist);
    void _OutputMessage(INT32 level, const char* message, INT32 size);
    bool _DrainMessages();

	BOOL _Log2Console(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize);
	BOOL _Log2File(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize);
	BOOL _Log2Trace(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize);
	BOOL _Log2Custom(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize);

	static THREAD_DECLARE(_LogThreadProc)(void* pParam);

private:
	static CLog         ms_Instance;
    std::recursive_mutex m_outputMutex;

	char		        m_szModuleName[MAX_MODULE_NAME];
	UINT32              m_dwProcessID;
	std::atomic<INT32> m_eLogLevelMask;
	std::atomic<INT32> m_eLogOutputMask;
	BOOL                m_bPrintThreadID;
	BOOL                m_bFlushFile;

	char				m_szCurrentFileFolder[MAX_MODULE_NAME];
	char				m_szCurrentFilePath[MAX_MODULE_NAME];
	FILE*		        m_pFile;
	INT32		        m_nCurrentSize;
	UINT32				m_dwLastUpdateCount;
	UINT32				m_dwLastFlushCount;
#if defined(WIN32)
	HANDLE				m_hConsole;
#endif
	CThread*			m_pLogThread;
	CCircleList			m_LogList[MAX_LOG_THREAD_COUNT];
	std::atomic<UINT32> m_dwThreadCount;
    std::uint64_t m_generation = 0;
	std::atomic<bool> m_bStopThread{false};

	std::atomic<CUSTOM_OUTPUT_FUNC> m_pOutputFunc;
};

#endif	// _LOG_H_
