#include "stdafx.h"
#include "sql_mgr.h"

#include "mysql.h"
#include "mysql_time.h"
#include "time_ex.h"
#include "dump.h"
#include "zlib.h"
#include "mysql_owner.h"
#include <exception>

#define PREALLOCATE_SQL_STMT_COUNT  (8)
#define MAX_DB_QUERY_COUNT          (65536)
#define MAX_SQL_RETRY_COUNT         (1)
#define MAX_PARAM_BUFFER_SIZE       (64 * 1024)

struct SqlWorkerResources
{
    struct Statement
    {
        std::unique_ptr<MYSQL_BIND[]> pParamBind;
        std::unique_ptr<MYSQL_BIND[]> pResultBind;
        std::unique_ptr<UINT8[]> pbyBuffer;
        // Close the statement before destroying its bound buffers.
        cqyh::MysqlStmtPtr pStmt;
    };

    MYSQL* OpenConnection()
    {
        connection = cqyh::MakeMysql();
        return connection.get();
    }
    Statement* AllocateStatements(std::size_t count)
    {
        statements = std::make_unique<Statement[]>(count);
        return statements.get();
    }
    void Reset() noexcept
    {
        statements.reset();
        connection = cqyh::MysqlPtr{};
    }

    cqyh::MysqlPtr connection;
    std::unique_ptr<Statement[]> statements;
};

//CSqlMgr CSqlMgr::ms_Instance;

CSqlMgr::CSqlMgr()
{
    m_nSqlStmtCount = 0;
    m_nThreadCount = 0;
    m_bStopThread = FALSE;
    m_nThreadIndex = 0;
    m_dwServerPort = 0;
    m_nNewSqlStmtCount = 0;
    m_nDelSqlStmtCount = 0;
}

CSqlMgr::~CSqlMgr()
{
    UnInit();
}

BOOL CSqlMgr::Init(const char* psczServerAddr, const char* pcszUserName, const char* pcszPassword, 
                   const char* pcszDBName, const char* pcszCharset, UINT32 dwServerPort,
                   SQL_STMT_DATA* pSqlStmtList, INT32 nSqlStmtCount, INT32 nThreadCount)
{
    INT32 nRetCode = 0;
    std::unique_ptr<CSqlStmt> pSqlStmt;

    if (m_pSqlStmtList || m_pSqlStmtPool || m_nThreadCount != 0)
    {
        LOG_CHECK_ERROR(!m_pSqlStmtList && !m_pSqlStmtPool && m_nThreadCount == 0);
        return FALSE;
    }

    LOG_PROCESS_ERROR(psczServerAddr);
    LOG_PROCESS_ERROR(pcszUserName);
    LOG_PROCESS_ERROR(pcszPassword);
    LOG_PROCESS_ERROR(pcszDBName);
    LOG_PROCESS_ERROR(pcszCharset);
    LOG_PROCESS_ERROR(pSqlStmtList);
    LOG_PROCESS_ERROR(nThreadCount > 0 && nThreadCount <= MAX_DB_WORKER_THREAD_COUNT);
    LOG_PROCESS_ERROR(nSqlStmtCount > 0);

    LOG_PROCESS_ERROR(m_library.Acquire());

    m_pSqlStmtList = std::make_unique_for_overwrite<SQL_STMT_DATA[]>(nSqlStmtCount);
    LOG_PROCESS_ERROR(m_pSqlStmtList);

    memcpy(m_pSqlStmtList.get(), pSqlStmtList, sizeof(SQL_STMT_DATA) * nSqlStmtCount);
    m_nSqlStmtCount = nSqlStmtCount;
    m_nThreadIndex = 0;

    m_pSqlStmtPool = std::make_unique<CBaseList[]>(m_nSqlStmtCount);
    LOG_PROCESS_ERROR(m_pSqlStmtPool);

    for (INT32 nIndex = 0; nIndex < m_nSqlStmtCount; nIndex++)
    {
        LOG_PROCESS_ERROR(nIndex == m_pSqlStmtList[nIndex].nSqlStmtID);

        nRetCode = _ParseSqlStmt(m_pSqlStmtList.get() + nIndex);
        LOG_PROCESS_ERROR(nRetCode);
    }

    for (INT32 nSqlStmtID = 0; nSqlStmtID < m_nSqlStmtCount; nSqlStmtID++)
    {
        m_pSqlStmtPool[nSqlStmtID].Clear();
        for (INT32 nIndex = 0; nIndex < PREALLOCATE_SQL_STMT_COUNT; nIndex++)
        {
            pSqlStmt = std::make_unique<CSqlStmt>();
            LOG_PROCESS_ERROR(pSqlStmt);

            nRetCode = pSqlStmt->Init(this, nSqlStmtID);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = m_pSqlStmtPool[nSqlStmtID].PushRear(pSqlStmt.get());
            LOG_PROCESS_ERROR(nRetCode);      

            pSqlStmt.release();
        }
    }

    strxcpy(m_szServerAddr, psczServerAddr, MAX_PATH);
    strxcpy(m_szUserName, pcszUserName, MAX_PATH);
    strxcpy(m_szPassword, pcszPassword, MAX_PATH);
    strxcpy(m_szDBName, pcszDBName, MAX_PATH);
    strxcpy(m_szCharset, pcszCharset, MAX_PATH);
    m_dwServerPort = dwServerPort;
    m_bStopThread = FALSE;
    m_nThreadCount = nThreadCount;
    for (INT32 nIndex = 0; nIndex < m_nThreadCount; nIndex++)
    {
        nRetCode = m_PushList[nIndex].Init(MAX_DB_QUERY_COUNT);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_PopList[nIndex].Init(MAX_DB_QUERY_COUNT);
        LOG_PROCESS_ERROR(nRetCode);

        auto pSqlParam = std::make_unique<SQL_THREAD_PARAM>();
        pSqlParam->pSqlMgr = this;
        pSqlParam->nIndex = nIndex;

        nRetCode = m_Thread[nIndex].Start(_WorkerThread, pSqlParam.get());
        LOG_PROCESS_ERROR(nRetCode);
        pSqlParam.release();
    }

    return TRUE;
Exit0:
    pSqlStmt.reset();

    UnInit();

    return FALSE;
}

BOOL CSqlMgr::UnInit(void)
{
    INT32 nRetCode = 0;

    m_bStopThread = TRUE;
    for (INT32 nIndex = 0; nIndex < m_nThreadCount; nIndex++)
    {
        m_Thread[nIndex].Wait();
    }
    // A worker leaves an unfinished statement in its input queue. Release both
    // queues only after joining, including any captured callback data.
    for (INT32 nIndex = 0; nIndex < m_nThreadCount; ++nIndex)
    {
        while (auto* statement = static_cast<CSqlStmt*>(m_PushList[nIndex].Pop()))
            delete statement;
        while (auto* statement = static_cast<CSqlStmt*>(m_PopList[nIndex].Pop()))
            delete statement;
        m_PushList[nIndex].UnInit();
        m_PopList[nIndex].UnInit();
    }
    m_nThreadCount = 0;

    for (INT32 nIndex = 0; m_pSqlStmtPool && nIndex < m_nSqlStmtCount; nIndex++)
    {
        for (BASE_LIST_NODE* pNode = m_pSqlStmtPool[nIndex].Head().pNext;
            pNode != &m_pSqlStmtPool[nIndex].Rear(); )  //pNode = pNode->pNext)
        {
            BASE_LIST_NODE* pNextNode = pNode->pNext;

            CSqlStmt* pSqlStmt = (CSqlStmt*)pNode;

            nRetCode = pSqlStmt->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            SAFE_DELETE(pSqlStmt);

            pNode = pNextNode;
        }

        m_pSqlStmtPool[nIndex].Clear();
    }
    m_nSqlStmtCount = 0;

    m_pSqlStmtPool.reset();
    m_pSqlStmtList.reset();
    m_nNewSqlStmtCount = 0;
    m_nDelSqlStmtCount = 0;
    m_nThreadIndex = 0;

    m_library.Reset();

    return TRUE;
}

//CSqlMgr& CSqlMgr::GetInstance(void)
//{
//    return ms_Instance;
//}

BOOL CSqlMgr::MainLoop(void)
{
    INT32 nRetCode = 0;

    for (INT32 nIndex = 0; nIndex < m_nThreadCount; nIndex++)
    {
        while (auto* pSqlStmt = _TakeCompleted(nIndex))
        {
            nRetCode = _CompleteSqlStmt(pSqlStmt);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

Exit0:
    return TRUE;
}

CSqlStmt* CSqlMgr::_NewSqlStmt(INT32 nSqlStmtID)
{
    CSqlStmt* pResult = NULL;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(nSqlStmtID >= 0 && nSqlStmtID < m_nSqlStmtCount);

    if (m_pSqlStmtPool[nSqlStmtID].IsEmpty())
    {
        auto created = std::make_unique<CSqlStmt>();
        nRetCode = created->Init(this, nSqlStmtID);
        LOG_PROCESS_ERROR(nRetCode);
        pResult = created.release();
    }
    else
    {
        pResult = (CSqlStmt*)m_pSqlStmtPool[nSqlStmtID].PopHead();
    }
    LOG_PROCESS_ERROR(pResult);

    pResult->m_nExecuteCount = 0;
    pResult->m_bSucceeded = false;
    pResult->m_nParamCount = 0;
    pResult->m_nParamBufferSize = 0;

	m_nNewSqlStmtCount++;

    return pResult;
Exit0:
    if (pResult)
        _DelSqlStmt(pResult);
    return NULL;
}

BOOL CSqlMgr::_DelSqlStmt(CSqlStmt* pSqlStmt)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pSqlStmt);
    LOG_PROCESS_ERROR(pSqlStmt->m_pSqlStmtData);
    LOG_PROCESS_ERROR(pSqlStmt->m_pSqlStmtData->nSqlStmtID >= 0 && pSqlStmt->m_pSqlStmtData->nSqlStmtID < m_nSqlStmtCount);

    pSqlStmt->m_pCallbackFunc = nullptr;
    pSqlStmt->m_pUserData = nullptr;
    pSqlStmt->m_Results.Clear();

    nRetCode = m_pSqlStmtPool[pSqlStmt->m_pSqlStmtData->nSqlStmtID].PushRear(pSqlStmt);
    LOG_PROCESS_ERROR(nRetCode);

	m_nDelSqlStmtCount++;

    return TRUE;
Exit0:
    return FALSE;
}

CSqlStmt* CSqlMgr::_ExecSqlStmt(INT32 nSqlStmtID, SQL_STMT_CALLBACK callback, void* pUserData, const char* pcszFormat, va_list valist)
{
    INT32 nRetCode = 0;
    CSqlStmt* pSqlStmt = NULL;
    BOOL bUnsigned = FALSE;

    LOG_PROCESS_ERROR(pcszFormat);
    LOG_PROCESS_ERROR(m_nThreadCount > 0);

    pSqlStmt = _NewSqlStmt(nSqlStmtID);
    LOG_PROCESS_ERROR(pSqlStmt);

    pSqlStmt->m_pCallbackFunc = std::move(callback);
    pSqlStmt->m_pUserData = pUserData;

    while (*pcszFormat)
    {
        switch (*pcszFormat)
        {
        case 'c':
            {
                if (bUnsigned)
                {
                    UINT8 byValue = va_arg(valist, UINT8);
                    nRetCode = pSqlStmt->PushParam(byValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                else
                {
                    INT8 chValue = va_arg(valist, INT8);
                    nRetCode = pSqlStmt->PushParam(chValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                bUnsigned = FALSE;
            }
            break;
		case 't' :
			{
                LOG_PROCESS_ERROR(bUnsigned == FALSE);
				time_t tValue = va_arg(valist, time_t);
 				nRetCode = pSqlStmt->PushTime(tValue);
 				LOG_PROCESS_ERROR(nRetCode);		
			}
			break;
        case 'h':
            {
                if (bUnsigned)
                {
                    UINT16 wValue = va_arg(valist, UINT16);
                    nRetCode = pSqlStmt->PushParam(wValue);
                    LOG_PROCESS_ERROR(nRetCode);
                    bUnsigned = FALSE;
                }
                else
                {
                    INT16 shValue = va_arg(valist, INT16);
                    nRetCode = pSqlStmt->PushParam(shValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                bUnsigned = FALSE;
            }
            break;
        case 'd':
            {
                if (bUnsigned)
                {
                    UINT32 dwValue = va_arg(valist, UINT32);
                    nRetCode = pSqlStmt->PushParam(dwValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                else
                {
                    INT32 nValue = va_arg(valist, INT32);
                    nRetCode = pSqlStmt->PushParam(nValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                bUnsigned = FALSE;
            }
            break;
        case 'l':
            {
                if (bUnsigned)
                {
                    UINT64 qwValue = va_arg(valist, UINT64);
                    nRetCode = pSqlStmt->PushParam(qwValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                else
                {
                    INT64 llValue = va_arg(valist, INT64);
                    nRetCode = pSqlStmt->PushParam(llValue);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                bUnsigned = FALSE;
            }
            break;
        case 's':
            {
                LOG_PROCESS_ERROR(bUnsigned == FALSE);
                const char* pcszValue = va_arg(valist, const char*);
                nRetCode = pSqlStmt->PushParam(pcszValue);
                LOG_PROCESS_ERROR(nRetCode);
            }
            break;
        case 'b':
            {
                UINT8* pbyValue = va_arg(valist, UINT8*);
                UINT32 dwValueSize = va_arg(valist, UINT32);
                nRetCode = pSqlStmt->PushParam(pbyValue, dwValueSize);
                LOG_PROCESS_ERROR(nRetCode);
            }
            break;
        case 'z':
        {
            UINT8* pbyValue = va_arg(valist, UINT8*);
            UINT32 dwValueSize = va_arg(valist, UINT32);
            nRetCode = pSqlStmt->PushParamZ(pbyValue, dwValueSize);
            LOG_PROCESS_ERROR(nRetCode);
        }
            break;
        case 'u':
            {
                bUnsigned = TRUE;
            }
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
            break;
        }
        pcszFormat++;
    }

    if (pSqlStmt->m_pSqlStmtData && pSqlStmt->m_pSqlStmtData->nThreadIndex >= 0)
    {
        const auto index = pSqlStmt->m_pSqlStmtData->nThreadIndex % m_nThreadCount;
        LOG_PROCESS_ERROR(m_Thread[index].IsRunning());
        nRetCode = m_PushList[index].Push(pSqlStmt);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        LOG_PROCESS_ERROR(m_Thread[m_nThreadIndex].IsRunning());
        nRetCode = m_PushList[m_nThreadIndex].Push(pSqlStmt);
        LOG_PROCESS_ERROR(nRetCode);

        m_nThreadIndex = (m_nThreadIndex + 1) % m_nThreadCount;
    }

    return pSqlStmt;
Exit0:
    CRI("failed to execute sql stmt %d", nSqlStmtID);
    if (pSqlStmt)
        _DelSqlStmt(pSqlStmt);

    return NULL;
}

BOOL CSqlMgr::AsyncExecSqlStmt(INT32 nSqlStmtID, SQL_STMT_CALLBACK_FUNC pCallbackFunc, void* pUserData, const char* pcszFormat, ...)
{
    CSqlStmt* pSqlStmt = NULL;
    va_list valist;

    va_start(valist, pcszFormat);

    pSqlStmt = _ExecSqlStmt(nSqlStmtID, pCallbackFunc, pUserData, pcszFormat, valist);
    va_end(valist);
    LOG_PROCESS_ERROR(pSqlStmt);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSqlMgr::SyncExecSqlStmt(INT32 nSqlStmtID, SQL_STMT_CALLBACK_FUNC pCallbackFunc, void* pUserData, const char* pcszFormat, ...)
{
    va_list valist;
    va_start(valist, pcszFormat);
    auto* statement = _ExecSqlStmt(nSqlStmtID, pCallbackFunc, pUserData, pcszFormat, valist);
    va_end(valist);
    return _WaitSqlStmt(statement);
}

BOOL CSqlMgr::AsyncExecSqlStmtOwned(INT32 nSqlStmtID, SQL_STMT_CALLBACK callback, const char* pcszFormat, ...)
{
    va_list valist;
    va_start(valist, pcszFormat);
    auto* statement = _ExecSqlStmt(nSqlStmtID, std::move(callback), nullptr, pcszFormat, valist);
    va_end(valist);
    return statement != nullptr;
}

BOOL CSqlMgr::SyncExecSqlStmtOwned(INT32 nSqlStmtID, SQL_STMT_CALLBACK callback, const char* pcszFormat, ...)
{
    va_list valist;
    va_start(valist, pcszFormat);
    auto* statement = _ExecSqlStmt(nSqlStmtID, std::move(callback), nullptr, pcszFormat, valist);
    va_end(valist);
    return _WaitSqlStmt(statement);
}

BOOL CSqlMgr::_CompleteSqlStmt(CSqlStmt* statement)
{
    const auto recycle = [this](CSqlStmt* value)
    {
        const auto result = _DelSqlStmt(value);
        LOG_CHECK_ERROR(result);
    };
    const std::unique_ptr<CSqlStmt, decltype(recycle)> completed(statement, recycle);
    if (!statement->m_bSucceeded)
        return FALSE;
    if (statement->m_pCallbackFunc)
        return statement->m_pCallbackFunc(statement->m_Results.Rows(), statement->m_pUserData);
    return TRUE;
}

BOOL CSqlMgr::_WaitSqlStmt(CSqlStmt* statement)
{
    if (!statement)
        return FALSE;
    BOOL result = TRUE;
    BOOL completed = FALSE;
    std::exception_ptr failure;
    while (!completed)
    {
        for (INT32 nIndex = 0; nIndex < m_nThreadCount; nIndex++)
        {
            while (auto* pTempSqlStmt = _TakeCompleted(nIndex))
            {
                if (statement == pTempSqlStmt)
                    completed = TRUE;
                try
                {
                    const auto nRetCode = _CompleteSqlStmt(pTempSqlStmt);
                    LOG_CHECK_ERROR(nRetCode);
                    if (!nRetCode)
                        result = FALSE;
                }
                catch (...)
                {
                    // Finish the synchronous caller's borrow before unwinding it.
                    if (!failure)
                        failure = std::current_exception();
                    result = FALSE;
                }
            }
        }
        if (!completed)
            std::this_thread::yield();
    }

    if (failure)
        std::rethrow_exception(failure);
    return result;
}

CSqlStmt* CSqlMgr::_TakeCompleted(INT32 threadIndex)
{
    if (auto* statement = static_cast<CSqlStmt*>(m_PopList[threadIndex].Pop()))
        return statement;
    if (m_Thread[threadIndex].IsRunning())
        return nullptr;
    // The worker can fail before entering its processing loop (e.g. TLS init).
    // Join before taking over its consumer side and fail any abandoned requests.
    m_Thread[threadIndex].Wait();
    if (auto* statement = static_cast<CSqlStmt*>(m_PopList[threadIndex].Pop()))
        return statement;
    auto* statement = static_cast<CSqlStmt*>(m_PushList[threadIndex].Pop());
    if (statement)
    {
        statement->m_Results.Clear();
        statement->m_bSucceeded = false;
    }
    return statement;
}

bool CSqlMgr::_PublishCompleted(INT32 threadIndex, CSqlStmt* statement)
{
    while (!m_PopList[threadIndex].Push(statement))
    {
        if (m_bStopThread.load() ||
            m_Thread[threadIndex].WaitForStop(std::chrono::milliseconds(1)))
            return false;
    }
    // No further statement access after publication: the main thread can now
    // recycle it. On interrupted backpressure the input queue retains ownership
    // so UnInit disposes its captured data on the controlling thread after join.
    m_PushList[threadIndex].Pop();
    return true;
}

const SQL_STMT_DATA* CSqlMgr::GetSqlStmtData(INT32 nSqlStmtID)
{
    LOG_PROCESS_ERROR(nSqlStmtID >= 0 && nSqlStmtID < m_nSqlStmtCount);

    return m_pSqlStmtList.get() + nSqlStmtID;
Exit0:
    return NULL;
}

BOOL CSqlMgr::_ParseSqlStmt(SQL_STMT_DATA* pSqlStmtData)
{
    INT32 nRetCode = 0;
    const char* pcszSql = pSqlStmtData->pcszSql;
    BOOL bParamEscaped = FALSE;
    BOOL bResultEscaped = FALSE;

    pSqlStmtData->nParamCount = 0;
    pSqlStmtData->nParamBufferSize = 0;

    while (*pcszSql)
    {
        switch (*pcszSql)
        {
        case '[':
            {
                LOG_PROCESS_ERROR(!bParamEscaped && !bResultEscaped);
                bParamEscaped = TRUE;
                pSqlStmtData->szQuery[pSqlStmtData->nQuerySize++] = '?';
            }
            break;
        case ']':
            {
                LOG_PROCESS_ERROR(bParamEscaped);
                bParamEscaped = FALSE;
            }
            break;
        case '{':
            {
                LOG_PROCESS_ERROR(!bParamEscaped && !bResultEscaped);
                bResultEscaped = TRUE;
            }
            break;
        case '}':
            {
                LOG_PROCESS_ERROR(bResultEscaped);
                bResultEscaped = FALSE;
            }
            break;
        case 'c':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_TINY;
                    pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = sizeof(INT8);
                    pSqlStmtData->nParamBufferSize += pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                    pSqlStmtData->nParamCount++;
                }
                else if (bResultEscaped)
                {
                    pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_TINY;
                    pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = sizeof(INT8);
                    pSqlStmtData->nResultBufferSize += pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                    pSqlStmtData->nResultCount++;
                }
            }
            break;
        case 'h':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_SHORT;
                    pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = sizeof(INT16);
                    pSqlStmtData->nParamBufferSize += pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                    pSqlStmtData->nParamCount++;
                }
                else if (bResultEscaped)
                {
                    pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_SHORT;
                    pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = sizeof(INT16);
                    pSqlStmtData->nResultBufferSize += pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                    pSqlStmtData->nResultCount++;
                }
            }
            break;
        case 'd':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_LONG;
                    pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = sizeof(INT32);
                    pSqlStmtData->nParamBufferSize += pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                    pSqlStmtData->nParamCount++;
                }
                else if (bResultEscaped)
                {
                    pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_LONG;
                    pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = sizeof(INT32);
                    pSqlStmtData->nResultBufferSize += pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                    pSqlStmtData->nResultCount++;
                }
            }
            break;
        case 'l':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_LONGLONG;
                    pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = sizeof(INT64);
                    pSqlStmtData->nParamBufferSize += pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                    pSqlStmtData->nParamCount++;
                }
                else if (bResultEscaped)
                {
                    pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_LONGLONG;
                    pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = sizeof(INT64);
                    pSqlStmtData->nResultBufferSize += pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                    pSqlStmtData->nResultCount++;
                }
            }
            break;
        case 's':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_STRING;
                    pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = atoi(pcszSql + 1) + 1;
                    LOG_PROCESS_ERROR(pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] > 0);
                    pSqlStmtData->nParamBufferSize += sizeof(UINT32) + pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                    pSqlStmtData->nParamCount++;
                }
                else if (bResultEscaped)
                {
                    pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_STRING;
                    pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = atoi(pcszSql + 1) + 1;
                    LOG_PROCESS_ERROR(pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] > 0);
                    pSqlStmtData->nResultBufferSize += pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                    pSqlStmtData->nResultCount++;
                }
            }
            break;
        case 'b':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_BLOB;
                    pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = atoi(pcszSql + 1);
                    LOG_PROCESS_ERROR(pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] > 0);
                    pSqlStmtData->nParamBufferSize += sizeof(UINT32) + pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                    pSqlStmtData->nParamCount++;
                }
                else if (bResultEscaped)
                {
                    pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_BLOB;
                    pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = atoi(pcszSql + 1);
                    LOG_PROCESS_ERROR(pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] > 0);
                    pSqlStmtData->nResultBufferSize += sizeof(UINT32) + pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                    pSqlStmtData->nResultCount++;
                }
            }
            break;
        case 'z':
        {
            if (bParamEscaped)
            {
                pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_LONG_BLOB;
                pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = atoi(pcszSql + 1);            // Keep the original buffer maximum size; exceeding it is unlikely
                LOG_PROCESS_ERROR(pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] > 0);
                pSqlStmtData->nParamBufferSize += sizeof(UINT32) + pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
                pSqlStmtData->nParamCount++;
            }
            else if (bResultEscaped)
            {
                pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_LONG_BLOB;
                pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = atoi(pcszSql + 1);
                LOG_PROCESS_ERROR(pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] > 0);
                pSqlStmtData->nResultBufferSize += sizeof(UINT32) + pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
                pSqlStmtData->nResultCount++;
            }
        }
            break;
		case 't':
			{
 				if (bParamEscaped)
 				{
 					pSqlStmtData->nParamType[pSqlStmtData->nParamCount] = MYSQL_TYPE_DATETIME;
 					pSqlStmtData->nParamSize[pSqlStmtData->nParamCount] = sizeof(MYSQL_TIME);
 					pSqlStmtData->nParamBufferSize += pSqlStmtData->nParamSize[pSqlStmtData->nParamCount];
 					pSqlStmtData->nParamCount++;
 				}
 				else if (bResultEscaped)
 				{
 					pSqlStmtData->nResultType[pSqlStmtData->nResultCount] = MYSQL_TYPE_DATETIME;
 					pSqlStmtData->nResultSize[pSqlStmtData->nResultCount] = sizeof(MYSQL_TIME);
 					pSqlStmtData->nResultBufferSize += pSqlStmtData->nResultSize[pSqlStmtData->nResultCount];
 					pSqlStmtData->nResultCount++;
 				}
			}
			break;
        case 'u':
            {
                if (bParamEscaped)
                {
                    pSqlStmtData->bIsUnsigned[pSqlStmtData->nParamCount] = TRUE;
                }
                else if (bResultEscaped)
                {
                    //pSqlStmtData->bIsUnsigned[pSqlStmtData->nParamCount] = TRUE;
                }
            }
            break;
        default:
            LOG_PROCESS_ERROR(isdigit(*pcszSql) || (!bParamEscaped && !bResultEscaped));
            break;
        }
        if (!bParamEscaped && !bResultEscaped && *pcszSql != ']' && *pcszSql != '}')
            pSqlStmtData->szQuery[pSqlStmtData->nQuerySize++] = *pcszSql;

        pcszSql++;
    }

    LOG_PROCESS_ERROR(pSqlStmtData->nParamBufferSize < MAX_PARAM_BUFFER_SIZE);
    LOG_PROCESS_ERROR(pSqlStmtData->nResultBufferSize < MAX_PARAM_BUFFER_SIZE);
    pSqlStmtData->nBufferSize = pSqlStmtData->nParamBufferSize > pSqlStmtData->nResultBufferSize ? 
        pSqlStmtData->nParamBufferSize : pSqlStmtData->nResultBufferSize;

    return TRUE;
Exit0:
    CRI("failed to parse sql stmt : %s", pSqlStmtData->pcszSql);

    return FALSE;
}

THREAD_DECLARE(CSqlMgr::_WorkerThread)(void* pParam)
{
    const std::unique_ptr<SQL_THREAD_PARAM> parameters(static_cast<SQL_THREAD_PARAM*>(pParam));
    if (!parameters || !parameters->pSqlMgr)
    {
        LOG_CHECK_ERROR(parameters && parameters->pSqlMgr);
        return 0;
    }
    cqyh::MysqlThreadLease clientThread;
    if (!clientThread)
    {
        LOG_CHECK_ERROR(clientThread);
        return 0;
    }
    SqlWorkerResources resources;
    return _WorkerLoop(parameters->pSqlMgr, parameters->nIndex, resources);
}

THREAD_DECLARE(CSqlMgr::_WorkerLoop)(CSqlMgr* pSqlMgr, INT32 nThreadIndex, SqlWorkerResources& resources)
{
    INT32       nRetCode = 0;
    SqlWorkerResources::Statement* pThreadSqlData = NULL;
    MYSQL*      pMysql = NULL;
    BOOL        bInited = FALSE;
    char        szCharList[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    while (!pSqlMgr->m_bStopThread)
	{
		__try
		{
			if (!bInited)
			{
				pMysql = resources.OpenConnection();
				LOG_PROCESS_ERROR(pMysql);

                LOG_PROCESS_ERROR(mysql_real_connect(pMysql, pSqlMgr->m_szServerAddr, pSqlMgr->m_szUserName,
					pSqlMgr->m_szPassword, pSqlMgr->m_szDBName, pSqlMgr->m_dwServerPort, NULL, 0));

				if (pSqlMgr->m_szCharset[0])
				{
					nRetCode = mysql_set_character_set(pMysql, pSqlMgr->m_szCharset);
					LOG_PROCESS_ERROR(nRetCode == 0);
				}

				pThreadSqlData = resources.AllocateStatements(pSqlMgr->m_nSqlStmtCount);
				LOG_PROCESS_ERROR(pThreadSqlData);

				for (INT32 nStmtIndex = 0; nStmtIndex < pSqlMgr->m_nSqlStmtCount; nStmtIndex++)
				{
					INT32 nBufferSize = 0;
					SQL_STMT_DATA* pSqlStmtData = pSqlMgr->m_pSqlStmtList.get() + nStmtIndex;

					pThreadSqlData[nStmtIndex].pStmt.reset(mysql_stmt_init(pMysql));
					LOG_PROCESS_ERROR(pThreadSqlData[nStmtIndex].pStmt.get());

                    //DBG(pSqlStmtData->szQuery);
					nRetCode = mysql_stmt_prepare(pThreadSqlData[nStmtIndex].pStmt.get(), pSqlStmtData->szQuery, pSqlStmtData->nQuerySize);
					LOG_PROCESS_ERROR(nRetCode == 0);

					pThreadSqlData[nStmtIndex].pbyBuffer.reset(new UINT8[pSqlStmtData->nBufferSize]);
					LOG_PROCESS_ERROR(pThreadSqlData[nStmtIndex].pbyBuffer.get());

					nRetCode = mysql_stmt_param_count(pThreadSqlData[nStmtIndex].pStmt.get());
					LOG_PROCESS_ERROR(nRetCode == pSqlStmtData->nParamCount);

                    if (pSqlStmtData->nParamCount > 0)
                    {
                        pThreadSqlData[nStmtIndex].pParamBind.reset(new MYSQL_BIND[pSqlStmtData->nParamCount]{});
                        LOG_PROCESS_ERROR(pThreadSqlData[nStmtIndex].pParamBind.get());

                        for (INT32 nParamIndex = 0; nParamIndex < pSqlStmtData->nParamCount; nParamIndex++)
                        {
                            pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_type = (enum_field_types)pSqlStmtData->nParamType[nParamIndex];
                            if (MYSQL_TYPE_LONG_BLOB == pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_type)
                                pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_type = MYSQL_TYPE_BLOB;
                            if (pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_type == MYSQL_TYPE_STRING)
                                pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_length = pSqlStmtData->nParamSize[nParamIndex] - 1;
                            else
                                pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_length = pSqlStmtData->nParamSize[nParamIndex];
                            if (pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_type == MYSQL_TYPE_STRING ||
                                pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer_type == MYSQL_TYPE_BLOB)
                            {
                                pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].length = (unsigned long*)(pThreadSqlData[nStmtIndex].pbyBuffer.get() + nBufferSize);
                                nBufferSize += sizeof(unsigned long);
                            }
                            pThreadSqlData[nStmtIndex].pParamBind.get()[nParamIndex].buffer = pThreadSqlData[nStmtIndex].pbyBuffer.get() + nBufferSize;
                            nBufferSize += pSqlStmtData->nParamSize[nParamIndex];
                        }

                        nRetCode = mysql_stmt_bind_param(pThreadSqlData[nStmtIndex].pStmt.get(), pThreadSqlData[nStmtIndex].pParamBind.get());
                        LOG_PROCESS_ERROR(nRetCode == 0);
                    }

                    if (pSqlStmtData->nResultCount > 0)
                    {
						pThreadSqlData[nStmtIndex].pResultBind.reset(new MYSQL_BIND[pSqlStmtData->nResultCount]{});
					    LOG_PROCESS_ERROR(pThreadSqlData[nStmtIndex].pResultBind.get());

					    nBufferSize = 0;
					    for (INT32 nResultIndex = 0; nResultIndex < pSqlStmtData->nResultCount; nResultIndex++)
					    {
						    pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].buffer_type = (enum_field_types)pSqlStmtData->nResultType[nResultIndex];
                            if (MYSQL_TYPE_LONG_BLOB == pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].buffer_type)
                                pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].buffer_type = MYSQL_TYPE_BLOB;
						    pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].buffer_length = pSqlStmtData->nResultSize[nResultIndex];
						    if (pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].buffer_type == MYSQL_TYPE_BLOB)
						    {
							    pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].length = (unsigned long*)(pThreadSqlData[nStmtIndex].pbyBuffer.get() + nBufferSize);
							    nBufferSize += sizeof(unsigned long);
						    }
						    pThreadSqlData[nStmtIndex].pResultBind.get()[nResultIndex].buffer = pThreadSqlData[nStmtIndex].pbyBuffer.get() + nBufferSize;
						    nBufferSize += pSqlStmtData->nResultSize[nResultIndex];
					    }

						nRetCode = mysql_stmt_bind_result(pThreadSqlData[nStmtIndex].pStmt.get(), pThreadSqlData[nStmtIndex].pResultBind.get());
						LOG_PROCESS_ERROR(nRetCode == 0);
					}
				}

				INF("db worker thread %d initialized successfully", nThreadIndex);
				bInited = TRUE;
			}

			if (pSqlMgr->m_PushList[nThreadIndex].Check() && bInited)
			{
				INT32 nSqlStmtID = 0;
				CSqlStmt* pSqlStmt = (CSqlStmt*)pSqlMgr->m_PushList[nThreadIndex].Check();
				LOG_PROCESS_ERROR(pSqlStmt);
				LOG_PROCESS_ERROR(pSqlStmt->m_pSqlStmtData);

				nSqlStmtID = pSqlStmt->m_pSqlStmtData->nSqlStmtID;

				if (pSqlStmt->m_nExecuteCount > MAX_SQL_RETRY_COUNT)
				{
					char szDumpData[MAX_PARAM_BUFFER_SIZE * 2];

					for (INT32 nIndex = 0; nIndex < pSqlStmt->m_nParamBufferSize; nIndex++)
					{
						if (nIndex % 2 == 0)
							szDumpData[nIndex] = szCharList[(pSqlStmt->m_pbyParamBuffer[nIndex / 2] >> 4) & 0xF];
						else
							szDumpData[nIndex] = szCharList[pSqlStmt->m_pbyParamBuffer[nIndex / 2] & 0xF];
					}
					szDumpData[pSqlStmt->m_nParamBufferSize] = 0;
					CRI("retry count of sql stmt %d exceeds limit. data dump %s", nSqlStmtID, szDumpData);
                    if (!pSqlMgr->_PublishCompleted(nThreadIndex, pSqlStmt))
                        return 0;
					continue;
				}
				pSqlStmt->m_nExecuteCount++;

				// A retried statement must not retain rows from a partial fetch.
                pSqlStmt->m_Results.Clear();
				memcpy(pThreadSqlData[nSqlStmtID].pbyBuffer.get(),
					pSqlStmt->m_pbyParamBuffer.get(), pSqlStmt->m_pSqlStmtData->nParamBufferSize);

				nRetCode = mysql_stmt_execute(pThreadSqlData[nSqlStmtID].pStmt.get());
				if (nRetCode != 0)
				{
					CRI("failed to execute mysql stmt %d (%s)", nSqlStmtID, pSqlStmt->m_pSqlStmtData->pcszSql);
					goto Exit0;
				}

				nRetCode = mysql_stmt_fetch(pThreadSqlData[nSqlStmtID].pStmt.get());
				while (nRetCode == 0 || nRetCode == MYSQL_DATA_TRUNCATED)
				{
					LOG_CHECK_ERROR(nRetCode != MYSQL_DATA_TRUNCATED);

					void* pData = pSqlStmt->m_Results.Append(pSqlStmt->m_pSqlStmtData->nResultBufferSize);
					LOG_PROCESS_ERROR(pData);

                    bool bHasZBlob = false;
                    for (INT32 nResultIndex = 0; nResultIndex < pSqlStmt->m_pSqlStmtData->nResultCount; nResultIndex++)
                    {
                        if ((enum_field_types)pSqlStmt->m_pSqlStmtData->nResultType[nResultIndex] == MYSQL_TYPE_LONG_BLOB)
                        {
                            bHasZBlob = true;
                            break;
                        }
                    }

                    if (bHasZBlob == true)
                    {
                        INT32 nBufferSize = 0;
                        unsigned long   ActualZipLength = 0;
                        unsigned long   *pZBlobLength = 0;
                       
                        for (INT32 nResultIndex = 0; nResultIndex < pSqlStmt->m_pSqlStmtData->nResultCount; nResultIndex++)
                        {
                            if (pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].buffer_type == MYSQL_TYPE_BLOB)
                            {
                                pZBlobLength = pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].length;
                                ActualZipLength = *pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].length;
                                nBufferSize += sizeof(unsigned long);
                            }

                            if ((enum_field_types)pSqlStmt->m_pSqlStmtData->nResultType[nResultIndex] == MYSQL_TYPE_LONG_BLOB)
                            {
                                INT32   nRet = 0;
                                unsigned long   length = pSqlStmt->m_pSqlStmtData->nResultSize[nResultIndex];
                                nRet = uncompress((unsigned char*)pData + nBufferSize, &length,
                                    (unsigned char*)pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].buffer, ActualZipLength);
                                if (nRet != Z_OK)
                                {
                                    CRI("unzip failed, zipped data len:%u, zipped data crc:%X", pSqlStmt->m_pSqlStmtData->nResultSize[nResultIndex],
                                        crc32(0, (const Bytef*)pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].buffer, ActualZipLength));
                                }
                                //else
                                //{
                                //    DBG("unzip. zipped data len:%u, zipped data crc:%X", length,
                                //        crc32(0, (const Bytef*)pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].buffer, ActualZipLength));
                                //}
                                *pZBlobLength = length;     // update the unzipped blob length
                            }
                            else
                            {
                                memcpy((char*)pData + nBufferSize, pThreadSqlData[nSqlStmtID].pResultBind.get()[nResultIndex].buffer, pSqlStmt->m_pSqlStmtData->nResultSize[nResultIndex]);
                            }
                            nBufferSize += pSqlStmt->m_pSqlStmtData->nResultSize[nResultIndex];
                        }
                    }
                    else
                    {
                        memcpy(pData, pThreadSqlData[nSqlStmtID].pbyBuffer.get(), pSqlStmt->m_pSqlStmtData->nResultBufferSize);
                    }

					nRetCode = mysql_stmt_fetch(pThreadSqlData[nSqlStmtID].pStmt.get());
				}

                LOG_PROCESS_ERROR(nRetCode == MYSQL_NO_DATA);
				nRetCode = mysql_stmt_free_result(pThreadSqlData[nSqlStmtID].pStmt.get());
				LOG_PROCESS_ERROR(nRetCode == 0);
                pSqlStmt->m_bSucceeded = true;

                if (!pSqlMgr->_PublishCompleted(nThreadIndex, pSqlStmt))
                    return 0;
			}
			else
			{
				pSqlMgr->m_Thread[nThreadIndex].WaitForStop(std::chrono::milliseconds(1));
			}
		}
		__except (DumpExceptionFilter(GetExceptionInformation()))
		{
			goto Exit0;
		}

        continue;

Exit0:
		if (pMysql && mysql_errno(pMysql) != 0)
			CRI("mysql error %d %s", mysql_errno(pMysql), mysql_error(pMysql));
		else
			CRI("unknown mysql error");
        resources.Reset();
        pThreadSqlData = nullptr;
        pMysql = nullptr;

        bInited = FALSE;
        pSqlMgr->m_Thread[nThreadIndex].WaitForStop(std::chrono::seconds(8));
    }


    return 0;
}
