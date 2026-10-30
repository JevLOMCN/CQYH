#include "stdafx.h"
#include "log.h"

#include "time_ex.h"
#include "utf8.h"
#include "dir.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "dump.h"

// string_ex.h retains this compatibility alias for unmigrated callers.
#ifdef snprintf
#undef snprintf
#endif

#define LOG_BUFFER_SIZE			(10 * 1024 * 1024)
#define MAX_MESSAGE_LEN			(4 * 1024)
#define MAX_LOG_MESSAGE_COUNT	(64 * 1024)

struct LOG_MESSAGE
{
	INT32 nLevelIndex;
	INT32 nMessageSize;
	char szMessage[MAX_MESSAGE_LEN];
};

CLog CLog::ms_Instance;

char g_szLevelString[32] = 
{
    'D',
    'I',
    'W',
    'E',
    'C',
    'S',
    'M',
    'L',
	'P',
	'Y',
};

#if defined(WIN32)
INT32 g_nLevelColor[32] =
{
    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
    FOREGROUND_GREEN,
    FOREGROUND_GREEN | FOREGROUND_RED,
    FOREGROUND_RED,
    FOREGROUND_RED,

    FOREGROUND_BLUE,
    FOREGROUND_BLUE | FOREGROUND_RED,
    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
	
	FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_BLUE | FOREGROUND_RED
};
#endif //WIN32

CLog::CLog()
{
    m_szModuleName[0] = 0;
	m_szCurrentFileFolder[0] = 0;
	m_szCurrentFilePath[0] = 0;
    m_dwProcessID = 0;
    m_eLogLevelMask = lltAll;
    m_eLogOutputMask = lotConsole;

    m_bPrintThreadID = FALSE;
    m_bFlushFile = FALSE;

    m_pFile = NULL;
    m_nCurrentSize = 0;
	m_dwLastUpdateCount = 0;
	m_dwLastFlushCount = 0;
#if defined(WIN32)
    m_hConsole = NULL;
#endif // WIN32
	m_pLogThread = NULL;
	m_dwThreadCount = 0;
	m_bStopThread = FALSE;
	m_pOutputFunc = NULL;
}

CLog::~CLog()
{
    UnInit();
}

CLog& CLog::GetInstance(void)
{
    return ms_Instance;
}

const char* CLog::GetModuleName(void)
{
	return m_szModuleName;
}

const char* CLog::GetLogFileFolder(void)
{
	return m_szCurrentFileFolder;
}

const char* CLog::GetLogFilePath(void)
{
	return m_szCurrentFilePath;
}

BOOL WINAPI HanlderRouting(DWORD CtrlType)
{
    return FALSE;
}

BOOL CLog::Init(const char* pcszModuleName, INT32 eLogLevelMask, INT32 eLogOutputMask, BOOL bLogThread, BOOL bPrintThreadID, BOOL bFlushFile)
{
    INT32 nRetCode = 0;

    PROCESS_ERROR(pcszModuleName);
    if (m_pLogThread)
        return FALSE;
    static std::atomic<std::uint64_t> nextGeneration{0};
    m_generation = nextGeneration.fetch_add(1) + 1;
    m_dwThreadCount.store(0);
    m_bStopThread.store(false);

#if defined(WIN32)
    m_hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
    LOG_PROCESS_ERROR(m_hConsole != INVALID_HANDLE_VALUE);

	{
		CONSOLE_FONT_INFOEX FontInfo;
		SetConsoleOutputCP(CP_UTF8);
		FontInfo.cbSize = sizeof(FontInfo);
		FontInfo.dwFontSize.X = 8;
		FontInfo.dwFontSize.Y = 16;
		wcsncpy(FontInfo.FaceName, L"Consolas", LF_FACESIZE);
		FontInfo.FontFamily = FF_DONTCARE;
		FontInfo.FontWeight = 500;
		FontInfo.nFont = 1;
		SetCurrentConsoleFontEx(m_hConsole, FALSE, &FontInfo);
        SetConsoleMode(m_hConsole, ENABLE_INSERT_MODE);
        SetConsoleCtrlHandler(HanlderRouting, TRUE);
        HMENU hMenu = GetSystemMenu(GetConsoleWindow(), FALSE);
        EnableMenuItem(hMenu, SC_CLOSE, MF_GRAYED);
	}
#endif //WIN32

    strxcpy(m_szModuleName, pcszModuleName, MAX_MODULE_NAME);
	memset(m_szCurrentFileFolder, 0, sizeof(m_szCurrentFileFolder));
	memset(m_szCurrentFilePath, 0, sizeof(m_szCurrentFilePath));
    m_dwProcessID = GetCurrentProcessId();
    m_eLogLevelMask = eLogLevelMask;
    m_eLogOutputMask = eLogOutputMask;
    m_bPrintThreadID = bPrintThreadID;
    m_bFlushFile = bFlushFile;
	m_dwLastUpdateCount = 0;
	m_dwLastFlushCount = 0;

	if (bLogThread)
	{
		for (INT32 nIndex = 0; nIndex < MAX_LOG_THREAD_COUNT; nIndex++)
		{
			nRetCode = m_LogList[nIndex].Init(MAX_LOG_MESSAGE_COUNT);
			PROCESS_ERROR(nRetCode);
		}

		m_pLogThread = new CThread;
		PROCESS_ERROR(m_pLogThread);

		nRetCode = m_pLogThread->Start(&CLog::_LogThreadProc, this);
        PROCESS_ERROR(nRetCode);
	}

    return TRUE;
Exit0:
    UnInit();

    return FALSE;
}

BOOL CLog::UnInit(void)
{
    INT32 nRetCode = 0;

	if (m_pLogThread)
	{
		m_bStopThread = TRUE;
		m_pLogThread->Wait();
		SAFE_DELETE(m_pLogThread);

    }
    for (auto& queue : m_LogList)
    {
        while (auto* message = static_cast<LOG_MESSAGE*>(queue.Pop()))
            delete message;
        queue.UnInit();
    }

	m_pOutputFunc = NULL;

    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }

    m_eLogLevelMask = m_eLogOutputMask = 0;

    return TRUE;
}

bool CLog::WriteLog(INT32 eLogLevel, const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(eLogLevel, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return true;
Exit0:
    return false;
}

void CLog::SetOutputMask(INT32 eLogOutputMask)
{
    m_eLogOutputMask = eLogOutputMask;
}

void CLog::SetLevelMask(INT32 eLogLevelMask)
{
	m_eLogLevelMask = eLogLevelMask;
}

INT32 CLog::GetLevelMask(void)
{
	return m_eLogLevelMask;
}

BOOL CLog::_WriteLog(INT32 eLogLevel, const char* pcszMessage, va_list valist)
{
    if ((eLogLevel & m_eLogLevelMask.load()) == 0)
        return TRUE;
    if (!pcszMessage)
        return FALSE;

    struct ThreadSlot
    {
        std::uint64_t generation = 0;
        UINT32 index = MAX_LOG_THREAD_COUNT;
    };
    static thread_local std::unordered_map<const CLog*, ThreadSlot> slots;
    LOG_MESSAGE local{};
    auto owned = m_pLogThread ? std::make_unique<LOG_MESSAGE>() : nullptr;
    auto* message = owned ? owned.get() : &local;
    for (INT32 index = 0; index < 32; ++index)
    {
        if ((UINT32{1} << index) == static_cast<UINT32>(eLogLevel))
        {
            if (g_szLevelString[index] != 0)
                message->nLevelIndex = index;
            break;
        }
    }
    const auto* now = localtime_ex();
    const auto level = g_szLevelString[message->nLevelIndex];
    // Reserve CR, LF and the final NUL even when vsnprintf truncates the body.
    constexpr auto capacity = MAX_MESSAGE_LEN - 2;
    int offset;
    if (m_bPrintThreadID)
        offset = std::snprintf(message->szMessage, capacity,
            "%d-%02d-%02d %02d:%02d:%02d.%03d\t[%c] [%08X] ",
            now->tm_year, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min,
            now->tm_sec, now->tm_msec, level, CThread::GetThreadID());
    else
        offset = std::snprintf(message->szMessage, capacity,
            "%d-%02d-%02d %02d:%02d:%02d.%03d\t[%c] ",
            now->tm_year, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min,
            now->tm_sec, now->tm_msec, level);
    if (offset < 0 || offset >= capacity)
        return FALSE;
    std::vsnprintf(message->szMessage + offset, capacity - offset, pcszMessage, valist);
    message->nMessageSize = static_cast<INT32>(strnlen(message->szMessage, capacity));
    message->szMessage[message->nMessageSize++] = '\r';
    message->szMessage[message->nMessageSize++] = '\n';
    message->szMessage[message->nMessageSize] = '\0';

    if (m_pLogThread)
    {
        auto& slot = slots[this];
        if (slot.generation != m_generation)
        {
            auto count = m_dwThreadCount.load();
            while (count < MAX_LOG_THREAD_COUNT &&
                   !m_dwThreadCount.compare_exchange_weak(count, count + 1)) {}
            slot = {m_generation, count};
        }
        if (slot.index >= MAX_LOG_THREAD_COUNT || !m_LogList[slot.index].Push(message))
        {
            std::fputs("Log write error: no queue capacity\n", stderr);
            return FALSE;
        }
        owned.release();
    }
    else
        _OutputMessage(message->nLevelIndex, message->szMessage, message->nMessageSize);
    return TRUE;
}

void CLog::_OutputMessage(INT32 level, const char* message, INT32 size)
{
    // Custom sinks can log recursively; scoped ownership also handles exceptions.
    std::scoped_lock lock(m_outputMutex);
    const auto outputs = m_eLogOutputMask.load();
    if (outputs & lotConsole)
        _Log2Console(level, message, size);
    if (outputs & lotFile)
        _Log2File(level, message, size);
    if (outputs & lotTrace)
        _Log2Trace(level, message, size);
    if (outputs & lotCustom)
        _Log2Custom(level, message, size);
}

bool CLog::_DrainMessages()
{
    bool found = false;
    const auto count = m_dwThreadCount.load();
    for (UINT32 index = 0; index < count; ++index)
    {
        std::unique_ptr<LOG_MESSAGE> message(static_cast<LOG_MESSAGE*>(m_LogList[index].Pop()));
        if (message)
        {
            found = true;
            _OutputMessage(message->nLevelIndex, message->szMessage, message->nMessageSize);
        }
    }
    if (!found)
    {
        std::scoped_lock lock(m_outputMutex);
        if (m_pFile && m_dwLastUpdateCount != m_dwLastFlushCount)
        {
            const auto result = std::fflush(m_pFile);
            LOG_CHECK_ERROR(result == 0);
            m_dwLastFlushCount = m_dwLastUpdateCount;
        }
    }
    return found;
}

BOOL CLog::_Log2Console(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize)
{
#if defined(WIN32)
    if(nLevelIndex < 32)
        ::SetConsoleTextAttribute(m_hConsole, g_nLevelColor[nLevelIndex]);
#endif //WIN32

    std::fwrite(pcszMessage, 1, nMessageSize, stdout);

    return TRUE;
}

BOOL CLog::_Log2File(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize)
{
    INT32 nRetCode = 0;
    size_t uWriteSize;

    if (m_nCurrentSize >= LOG_BUFFER_SIZE)
    {
		if (m_pFile)
	        fclose(m_pFile);
        m_pFile = NULL;
        m_nCurrentSize = 0;
    }

    if (m_pFile == NULL)
    {
        const tm_ex* pNow = localtime_ex();
		snprintf(m_szCurrentFileFolder, sizeof(m_szCurrentFileFolder), "log/%04d_%02d_%02d", pNow->tm_year, pNow->tm_mon, pNow->tm_mday);

        g_CreateDirectory("log", TRUE);
		g_CreateDirectory(m_szCurrentFileFolder, TRUE);

		snprintf(m_szCurrentFilePath, sizeof(m_szCurrentFilePath), "log/%04d_%02d_%02d/%s_%d_%04d_%02d_%02d_%02d_%02d_%02d_%03d.log",
            pNow->tm_year, pNow->tm_mon, pNow->tm_mday, m_szModuleName, m_dwProcessID, pNow->tm_year, pNow->tm_mon, 
            pNow->tm_mday, pNow->tm_hour, pNow->tm_min, pNow->tm_sec, pNow->tm_msec);
        m_pFile = fopen(m_szCurrentFilePath, "wb");
        if (m_pFile == NULL)
            printf("failed to create log file");
        PROCESS_ERROR(m_pFile);
    }

    uWriteSize = fwrite(pcszMessage, 1, nMessageSize, m_pFile);
    PROCESS_ERROR(uWriteSize == nMessageSize);

    if (m_bFlushFile)
        fflush(m_pFile);

    m_nCurrentSize += nMessageSize;
	m_dwLastUpdateCount++;

    return TRUE;
Exit0:

    return FALSE;
}

BOOL CLog::_Log2Trace(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize)
{
#if defined(WIN32) | defined(WIN64)
    OutputDebugString(pcszMessage);
#endif  // WIN32

    return TRUE;
}

BOOL CLog::_Log2Custom(INT32 nLevelIndex, const char* pcszMessage, INT32 nMessageSize)
{
    if (const auto output = m_pOutputFunc.load())
    {
#if defined(WIN32)
        if (nLevelIndex < 32)
            ::SetConsoleTextAttribute(m_hConsole, g_nLevelColor[nLevelIndex]);
#endif //WIN32
        output(nLevelIndex, pcszMessage, nMessageSize);
    }

    return TRUE;
}

void CLog::SetCustomOutput(CUSTOM_OUTPUT_FUNC pFunc)
{
    m_pOutputFunc = pFunc;
}

//#if defined(WIN32)
BOOL CLog::DbgLog(const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(lltDebug, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLog::InfLog(const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(lltInfo, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLog::WrnLog(const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(lltWarning, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLog::ErrLog(const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(lltError, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLog::CriLog(const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(lltCritical, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLog::SplLog(const char* pcszMessage, ...)
{
    INT32 nRetCode = 0;
    va_list valist;

    va_start(valist, pcszMessage);
    try { nRetCode = _WriteLog(lltSpell, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CLog::SYSLogT(const char* pcszMessage, ...)
{
	INT32 nRetCode = 0;
	va_list valist;

	va_start(valist, pcszMessage);
	try { nRetCode = _WriteLog(lltSys, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CLog::SYSLogF(const char* pcszMessage, ...)
{
	INT32 nRetCode = 0;
	va_list valist;

	va_start(valist, pcszMessage);
	try { nRetCode = _WriteLog(lltSys, pcszMessage, valist); }
    catch (...) { va_end(valist); throw; }
    va_end(valist);
    PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

THREAD_DECLARE(CLog::_LogThreadProc)(void* pParam)
{
    auto* logger = static_cast<CLog*>(pParam);
    for (;;)
    {
        __try
        {
            if (!logger->_DrainMessages())
            {
                // The first empty scan may precede the producer's final push.
                // Observe stop, then scan again before declaring the queue drained.
                if (logger->m_bStopThread.load() && !logger->_DrainMessages())
                    break;
                logger->m_pLogThread->WaitForStop(std::chrono::milliseconds(16));
            }
        }
        __except (DumpExceptionFilter(GetExceptionInformation()))
        {
            std::fputs("detected error in log thread\n", stderr);
        }
    }
    return 0;
}

//#endif // WIN32
