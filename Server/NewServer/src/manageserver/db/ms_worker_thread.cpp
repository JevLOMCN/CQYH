#include "stdafx.h"
#include "ms_worker_thread.h"
#include "errmsg.h"
#include "time_ex.h"
#include "error_def.h"
#include "manageserver.h"
#include <limits>

CMSWorkerThread::CMSWorkerThread()
{
    m_mysql = cqyh::MysqlPtr{};
    m_bServerOK = FALSE;
    m_setting = NULL;
}

CMSWorkerThread::~CMSWorkerThread()
{
    UnInit();
}

BOOL CMSWorkerThread::Init(UINT32 dwResultCount, const DBSetting* dbsetting)
{
    if (m_mysql || m_bstart)
    {
        LOG_CHECK_ERROR(!m_mysql && !m_bstart);
        return FALSE;
    }
    BOOL bRet = FALSE;
    MYSQL* res_mysql = NULL;

    LOG_PROCESS_ERROR(dwResultCount > 0 && dwResultCount <= (std::numeric_limits<UINT32>::max)() / 10);
    bRet = m_requests.Init(dwResultCount * 10);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_results.Init(dwResultCount);
    LOG_PROCESS_ERROR(bRet);

    m_mysql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(m_mysql.get());

    m_setting = dbsetting;
    LOG_PROCESS_ERROR(m_setting);
    res_mysql = mysql_real_connect(
        m_mysql.get(),
        m_setting->szServerAddr,
        m_setting->szLoginName,
        m_setting->szPassword,
        m_setting->szDBName,
        m_setting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(m_setting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(m_mysql.get(), m_setting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    m_bServerOK = TRUE;

    return TRUE;

Exit0:
    UnInit();
    return FALSE;
}

BOOL CMSWorkerThread::UnInit()
{
    Wait();
    while (auto* request = static_cast<DB_OPERATION_REQUEST*>(m_requests.Pop()))
        delete request;
    while (auto* result = static_cast<DB_OPERATION_RESULT*>(m_results.Pop()))
        delete result;
    BOOL bRet = FALSE;

    bRet = m_requests.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_results.UnInit();
    LOG_CHECK_ERROR(bRet);

    m_mysql = cqyh::MysqlPtr{};
    m_setting = nullptr;
    m_bServerOK = FALSE;
    return TRUE;

}

BOOL CMSWorkerThread::ReInit()
{
    BOOL bRet = FALSE;
    MYSQL* res_mysql = NULL;

    INF("DB Failure, Reconnecting...");

    auto new_mysql = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(m_setting);
    LOG_PROCESS_ERROR(new_mysql.get());

    res_mysql = mysql_real_connect(
        new_mysql.get(),
        m_setting->szServerAddr,
        m_setting->szLoginName,
        m_setting->szPassword,
        m_setting->szDBName,
        m_setting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(m_setting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(new_mysql.get(), m_setting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    m_mysql = std::move(new_mysql);

    INF("DB Reconnection Succeeded!!!");
    m_bServerOK = TRUE;
    return TRUE;

Exit0:
    INF("DB Reconnection Failed.");
    return FALSE;
}

DB_OPERATION_RESULT* CMSWorkerThread::GetResult()
{
    return (DB_OPERATION_RESULT*)m_results.Pop();
}

void CMSWorkerThread::ThrdProc()
{
    cqyh::MysqlThreadContext clientThread;
    if (!clientThread)
    {
        LOG_CHECK_ERROR(clientThread);
        return;
    }
    BOOL bRet = FALSE;

    for (; m_bstart; )
    {
        if (!m_bServerOK)
            ReInit();

        for (;;)
        {
            if (!m_bServerOK)
                break;
            
            DB_OPERATION_REQUEST* req = (DB_OPERATION_REQUEST*)m_requests.Pop();

            if (req == NULL)
                break;

            switch (req->m_eOp)
            {
            default:
                LOG_CHECK_ERROR(FALSE);
            }

            delete req;
        }

        WaitForStop(std::chrono::milliseconds(8));
    }
}

INT32 CMSWorkerThread::_CheckSqlResult(BOOL bRet, MYSQL_STMT* stmt)
{
    if (bRet)
        return errSuccess;
    else
    {
        if (mysql_stmt_errno(stmt) == CR_SERVER_LOST)
        {
            m_bServerOK = FALSE;
        }
        return errDatabase;
    }
}
