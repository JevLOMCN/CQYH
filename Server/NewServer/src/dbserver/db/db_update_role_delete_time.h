#ifndef _DB_UPDATE_ROLE_DELETE_TIME_H_
#define _DB_UPDATE_ROLE_DELETE_TIME_H_

#include "mysql_owner.h"

class CDBRole;

class CDBUpdateRoleDeleteTime
{
public:
    CDBUpdateRoleDeleteTime();
    ~CDBUpdateRoleDeleteTime();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoUpdate();

    UINT32 m_dwRoleID;
    UINT32 m_dwDeleteTime;

    cqyh::MysqlStmtPtr m_stmt;

private:
    MYSQL_BIND m_bind_param[2];
};

#endif // _DB_UPDATE_ROLE_DELETE_TIME_H_
