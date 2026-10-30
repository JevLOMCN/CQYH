#include "stdafx.h"
#include "db_delete_role.h"

#define DELETE_ROLE "UPDATE role SET RoleDeleted = 1 WHERE RoleID = ?"

CDBDeleteRole::CDBDeleteRole()
{
    m_stmt = nullptr;
    m_dwRoleID = 0;
    memset(&m_bind_param, 0, sizeof(m_bind_param));
}

CDBDeleteRole::~CDBDeleteRole()
{

}

BOOL CDBDeleteRole::Init(MYSQL* mysql)
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
    LOG_PROCESS_ERROR(param_count == 1);

    memset(&m_bind_param, 0, sizeof(m_bind_param));
    m_bind_param.buffer_type = MYSQL_TYPE_LONG;
    m_bind_param.buffer = &m_dwRoleID;
    nRet = mysql_stmt_bind_param(prepared.get(), &m_bind_param);
    LOG_PROCESS_ERROR(nRet == 0);

    m_stmt = std::move(prepared);
    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDBDeleteRole::UnInit()
{
    return cqyh::CloseMysqlStatement(m_stmt);
}

BOOL CDBDeleteRole::DoDeleteRole()
{
    INT32 nRet = mysql_stmt_execute(m_stmt.get());
    LOG_PROCESS_ERROR(nRet == 0);

    return TRUE;

Exit0:
    DBG("%d, %s", mysql_stmt_errno(m_stmt.get()), mysql_stmt_error(m_stmt.get()));
    return FALSE;
}
