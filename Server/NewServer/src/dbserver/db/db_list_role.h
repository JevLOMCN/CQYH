#ifndef _DB_LIST_ROLE_H_
#define _DB_LIST_ROLE_H_

#include "mysql_owner.h"

class CDBUser;

class CDBListRole
{
public:
    CDBListRole();
    ~CDBListRole();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoListRole(CDBUser* dbuser);

    cqyh::MysqlStmtPtr m_stmt;

private:
    MYSQL_BIND m_bind_param[2];
    MYSQL_BIND m_bind_result[3];
};

#endif // _DB_LIST_ROLE_H_
