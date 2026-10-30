#include "stdafx.h"
#include "sql_stmt.h"

#include "mysql.h"
#include "zlib.h"
#include "sql_mgr.h"
#include "time.h"

CSqlStmt::CSqlStmt()
{
    m_nParamCount = 0;
    m_pSqlStmtData = NULL;
    m_nParamBufferSize = 0;
    m_pCallbackFunc = nullptr;
    m_pUserData = NULL;
    pPrev = pNext = NULL;
    m_nExecuteCount = 0;
    m_bSucceeded = false;
    m_pSqlMgr = NULL;
}

CSqlStmt::~CSqlStmt()
{
    UnInit();
}

BOOL CSqlStmt::Init(CSqlMgr *pSqlMgr, INT32 nSqlStmtID)
{
    LOG_PROCESS_ERROR(pSqlMgr);

    m_pSqlMgr = pSqlMgr;

    m_pSqlStmtData = m_pSqlMgr->GetSqlStmtData(nSqlStmtID);
    LOG_PROCESS_ERROR(m_pSqlStmtData);

    m_pbyParamBuffer = std::make_unique_for_overwrite<UINT8[]>(m_pSqlStmtData->nParamBufferSize);
    LOG_PROCESS_ERROR(m_pbyParamBuffer);

    m_nParamCount = 0;
    m_nParamBufferSize = 0;
    m_pCallbackFunc = nullptr;
    m_pUserData = NULL;
    m_nExecuteCount = 0;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSqlStmt::UnInit(void)
{
    m_pCallbackFunc = nullptr;
    m_pUserData = nullptr;
    m_Results.Clear();
    m_pbyParamBuffer.reset();
    m_pSqlStmtData = nullptr;
    m_bSucceeded = false;

    return TRUE;
}

#define DEF_PUSH_PARAM_FUNC(__ctype__, __mysql_type__, __is_unsigned__)                     \
    BOOL CSqlStmt::PushParam(__ctype__ Param)                                               \
    {                                                                                       \
        LOG_PROCESS_ERROR(m_nParamCount < m_pSqlStmtData->nParamCount);                     \
        LOG_PROCESS_ERROR(m_pSqlStmtData->nParamType[m_nParamCount] == (__mysql_type__));   \
        LOG_PROCESS_ERROR(m_pSqlStmtData->bIsUnsigned[m_nParamCount] == (__is_unsigned__)); \
                                                                                            \
        memcpy(m_pbyParamBuffer.get() + m_nParamBufferSize, &Param,                               \
            m_pSqlStmtData->nParamSize[m_nParamCount]);                                     \
        m_nParamBufferSize += m_pSqlStmtData->nParamSize[m_nParamCount];                    \
        m_nParamCount++;                                                                    \
                                                                                            \
        return TRUE;                                                                        \
    Exit0:                                                                                  \
        return FALSE;                                                                       \
    }

DEF_PUSH_PARAM_FUNC(INT8, MYSQL_TYPE_TINY, FALSE)
DEF_PUSH_PARAM_FUNC(INT16, MYSQL_TYPE_SHORT, FALSE)
DEF_PUSH_PARAM_FUNC(INT32, MYSQL_TYPE_LONG, FALSE)
DEF_PUSH_PARAM_FUNC(INT64, MYSQL_TYPE_LONGLONG, FALSE)

DEF_PUSH_PARAM_FUNC(UINT8, MYSQL_TYPE_TINY, TRUE)
DEF_PUSH_PARAM_FUNC(UINT16, MYSQL_TYPE_SHORT, TRUE)
DEF_PUSH_PARAM_FUNC(UINT32, MYSQL_TYPE_LONG, TRUE)
DEF_PUSH_PARAM_FUNC(UINT64, MYSQL_TYPE_LONGLONG, TRUE)

BOOL CSqlStmt::PushParam(const char* pcszValue)
{
    INT32 nStringLen = 0;

    LOG_PROCESS_ERROR(pcszValue);
    LOG_PROCESS_ERROR(m_nParamCount < m_pSqlStmtData->nParamCount);
    LOG_PROCESS_ERROR(m_pSqlStmtData->nParamType[m_nParamCount] == MYSQL_TYPE_STRING);

	nStringLen = (INT32)strnlen(pcszValue, m_pSqlStmtData->nParamSize[m_nParamCount]);
	LOG_PROCESS_ERROR(nStringLen < m_pSqlStmtData->nParamSize[m_nParamCount]);

	*(INT32*)(m_pbyParamBuffer.get() + m_nParamBufferSize) = nStringLen;
	m_nParamBufferSize += sizeof(INT32);

	memcpy(m_pbyParamBuffer.get() + m_nParamBufferSize, pcszValue, nStringLen + 1);
    m_nParamBufferSize += m_pSqlStmtData->nParamSize[m_nParamCount];

    m_nParamCount++;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSqlStmt::PushParam(UINT8* pbyValue, UINT32 dwValueSize)
{
    LOG_PROCESS_ERROR(pbyValue);
    LOG_PROCESS_ERROR(m_nParamCount < m_pSqlStmtData->nParamCount);
    LOG_PROCESS_ERROR(m_pSqlStmtData->nParamType[m_nParamCount] == MYSQL_TYPE_BLOB);
    LOG_PROCESS_ERROR(dwValueSize <= (UINT32)m_pSqlStmtData->nParamSize[m_nParamCount]);

    *(UINT32*)(m_pbyParamBuffer.get() + m_nParamBufferSize) = dwValueSize;
    m_nParamBufferSize += sizeof(UINT32);

    memcpy(m_pbyParamBuffer.get() + m_nParamBufferSize, pbyValue, dwValueSize);
    m_nParamBufferSize += m_pSqlStmtData->nParamSize[m_nParamCount];

    m_nParamCount++;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSqlStmt::PushParamZ(UINT8* pbyValue, UINT32 dwValueSize)
{
    INT32 nRet = 0;
    UINT32  *pSize = NULL;
    unsigned long   BufferLength = 0;
    LOG_PROCESS_ERROR(pbyValue);
    LOG_PROCESS_ERROR(m_nParamCount < m_pSqlStmtData->nParamCount);
    LOG_PROCESS_ERROR(m_pSqlStmtData->nParamType[m_nParamCount] == MYSQL_TYPE_LONG_BLOB);
    LOG_PROCESS_ERROR(dwValueSize <= (UINT32)m_pSqlStmtData->nParamSize[m_nParamCount]);

    pSize = (UINT32*)(m_pbyParamBuffer.get() + m_nParamBufferSize);
    m_nParamBufferSize += sizeof(UINT32);
    BufferLength = m_pSqlStmtData->nParamSize[m_nParamCount];

    nRet = compress((unsigned char *)(m_pbyParamBuffer.get() + m_nParamBufferSize), &BufferLength, (unsigned char*)pbyValue, dwValueSize);
    //debug
    //DBG("original data len:%u, zipped data len:%u, zipped data crc:%X", dwValueSize, BufferLength,
    //    crc32(0, (const Bytef*)(m_pbyParamBuffer.get() + m_nParamBufferSize), BufferLength));
    //end debug
    LOG_PROCESS_ERROR(nRet == Z_OK);

    *pSize = BufferLength;
    m_nParamBufferSize += m_pSqlStmtData->nParamSize[m_nParamCount];

    m_nParamCount++;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSqlStmt::PushTime(time_t tValue)
{
    MYSQL_TIME      mysqlTime = {};
    struct tm *pTm = localtime(&tValue);
    LOG_PROCESS_ERROR(pTm);

    mysqlTime.year      = pTm->tm_year + 1900;
    mysqlTime.month     = pTm->tm_mon + 1;
    mysqlTime.day       = pTm->tm_mday;
    mysqlTime.hour      = pTm->tm_hour;
    mysqlTime.minute    = pTm->tm_min;
    mysqlTime.second    = pTm->tm_sec;

    mysqlTime.time_type = MYSQL_TIMESTAMP_DATETIME;
    mysqlTime.second_part   = 0;

    LOG_PROCESS_ERROR(m_nParamCount < m_pSqlStmtData->nParamCount);
    LOG_PROCESS_ERROR(m_pSqlStmtData->nParamType[m_nParamCount] == MYSQL_TYPE_DATETIME);

    memcpy(m_pbyParamBuffer.get() + m_nParamBufferSize, &mysqlTime, m_pSqlStmtData->nParamSize[m_nParamCount]);
    m_nParamBufferSize += m_pSqlStmtData->nParamSize[m_nParamCount];

    m_nParamCount++;

    return TRUE;
Exit0:
    return FALSE;
}

