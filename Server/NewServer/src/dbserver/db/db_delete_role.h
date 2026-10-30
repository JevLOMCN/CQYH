#ifndef _DB_DELETE_ROLE_H_
#define _DB_DELETE_ROLE_H_

#include "mysql_owner.h"

class CDBRole;

class CDBDeleteRole
{
public:
    CDBDeleteRole();
    ~CDBDeleteRole();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoDeleteRole();

    UINT32 m_dwRoleID;

    cqyh::MysqlStmtPtr m_stmt;

private:
    MYSQL_BIND m_bind_param;
};

#endif // _DB_DELETE_ROLE_H_
