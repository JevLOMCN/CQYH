#ifndef _DB_SAVE_ROLE_H_
#define _DB_SAVE_ROLE_H_

#include "mysql_owner.h"

class CDBSaveRole
{
public:
    CDBSaveRole();
    ~CDBSaveRole();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoSaveRole(char* buf, UINT32 bufsize);

    UINT32 m_dwRoleID;

    cqyh::MysqlStmtPtr m_stmt;

private:
    MYSQL_BIND m_bind_param[3];
};

#endif // _DB_SAVE_ROLE_H_
