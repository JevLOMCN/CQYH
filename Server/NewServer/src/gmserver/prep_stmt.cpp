#include "stdafx.h"
#include "prep_stmt.h"

#include "gmserver.h"

CPrepStmt::CPrepStmt()
{
}

BOOL CPrepStmt::Init(DBSetting *pDBSetting, const char* szQuery)
{
    INT32 nRet = 0;
    MYSQL* res_mysql = NULL;
    cqyh::MysqlPtr connection;
    cqyh::MysqlStmtPtr prepared;
    LOG_PROCESS_ERROR(pDBSetting);
    LOG_PROCESS_ERROR(szQuery);

    LOG_PROCESS_ERROR(mysql == NULL);
    LOG_PROCESS_ERROR(stmt == NULL);

    connection = cqyh::MakeMysql();
    LOG_PROCESS_ERROR(connection);

    res_mysql = mysql_real_connect(
        connection.get(),
        pDBSetting->szServerAddr,
        pDBSetting->szLoginName,
        pDBSetting->szPassword,
        pDBSetting->szDBName,
        pDBSetting->dwServerPort,
        NULL,
        0);
    LOG_PROCESS_ERROR(res_mysql);

    if (strlen(pDBSetting->szCharSet))
    {
        INT32 nRetCode = mysql_set_character_set(connection.get(), pDBSetting->szCharSet);
        LOG_PROCESS_ERROR(nRetCode == 0);
    }

    prepared.reset(mysql_stmt_init(connection.get()));
    LOG_PROCESS_ERROR(prepared);

    nRet = mysql_stmt_prepare(prepared.get(), szQuery, (unsigned long)strlen(szQuery));
    LOG_PROCESS_ERROR(nRet == 0);

    nRet = mysql_stmt_execute(prepared.get());
    LOG_PROCESS_ERROR(nRet == 0);

    mysql = std::move(connection);
    stmt = std::move(prepared);
    return TRUE;

Exit0:
    if (connection)
        INF("%d: %s", mysql_errno(connection.get()), mysql_error(connection.get()));

    return FALSE;
}

BOOL CPrepStmt::UnInit()
{
    const auto result = cqyh::CloseMysqlStatement(stmt);
    mysql = cqyh::MysqlPtr{};
    return result;
}

BOOL CPrepStmt::BindResult(MYSQL_BIND* pResults)
{
    LOG_PROCESS_ERROR(pResults);
    return mysql_stmt_bind_result(stmt.get(), pResults) == 0;

Exit0:
    return FALSE;
}

INT32 CPrepStmt::Fetch()
{
    return mysql_stmt_fetch(stmt.get());
}
