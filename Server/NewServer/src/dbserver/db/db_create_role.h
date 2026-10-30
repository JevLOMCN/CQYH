#ifndef _DB_CREATE_ROLE_H_
#define _DB_CREATE_ROLE_H_

#include "mysql_owner.h"

class CDBRole;

class CDBCreateRole
{
public:
    CDBCreateRole();
    ~CDBCreateRole();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoCreateRole(UINT8 byGroupID, UINT32 roleid, char* rolename, char* username,
        char* base_data_buf, UINT32 base_data_buf_len, char* role_data_buf, UINT32 role_data_buf_len);

    cqyh::MysqlStmtPtr m_stmt;

private:
    MYSQL_BIND m_bind_param[6];
    MYSQL_BIND m_bind_result[2];
};

#endif // _DB_LIST_ROLE_H_
