#include "stdafx.h"
#include "db_update_role_delete_time.h"

#define DELETE_ROLE "UPDATE role SET DeleteTime = FROM_UNIXTIME(NULLIF(?, 0)) WHERE RoleID = ?"

CDBUpdateRoleDeleteTime::CDBUpdateRoleDeleteTime()
{
    m_stmt = nullptr;
    m_dwRoleID = 0;
    m_dwDeleteTime = 0;
    memset(m_bind_param, 0, sizeof(m_bind_param));
}

CDBUpdateRoleDeleteTime::~CDBUpdateRoleDeleteTime()
{

}

BOOL CDBUpdateRoleDeleteTime::Init(MYSQL* mysql)
{
    cqyh::MysqlStmtPtr prepared;
    INT32 nRet = 0;
    UINT32 param_count = 0;

    LOG_PROCESS_ERROR(mysql);
    LOG_PROCESS_ERROR(!m_stmt);

    prepared.reset(mysql_stmt_init(mysql));
    LOG_PROCESS_ERROR(prepared.get());

    nRet = mysql_stmt_prepare(prepared.get(), DELETE_ROLE, (unsigned long)strlen(DELETE_ROLE));
    LOG_PROCESS_ERROR(nRet == 0);

    param_count = mysql_stmt_param_count(prepared.get());
    LOG_PROCESS_ERROR(param_count == 2);

    memset(m_bind_param, 0, sizeof(m_bind_param));

    m_bind_param[0].buffer_type = MYSQL_TYPE_LONG;
    m_bind_param[0].buffer = &m_dwDeleteTime;

    m_bind_param[1].buffer_type = MYSQL_TYPE_LONG;
    m_bind_param[1].buffer = &m_dwRoleID;

    nRet = mysql_stmt_bind_param(prepared.get(), m_bind_param);
    LOG_PROCESS_ERROR(nRet == 0);

    m_stmt = std::move(prepared);
    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDBUpdateRoleDeleteTime::UnInit()
{
    return cqyh::CloseMysqlStatement(m_stmt);
}

BOOL CDBUpdateRoleDeleteTime::DoUpdate()
{
    INT32 nRet = mysql_stmt_execute(m_stmt.get());
    LOG_PROCESS_ERROR(nRet == 0);

    return TRUE;

Exit0:
    DBG("%d, %s", mysql_stmt_errno(m_stmt.get()), mysql_stmt_error(m_stmt.get()));
    return FALSE;
}
