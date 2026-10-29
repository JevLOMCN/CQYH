#ifndef _PREP_STMT_H_
#define _PREP_STMT_H_

#include "mysql_owner.h"




class CPrepStmt
{
public:
    CPrepStmt();
    BOOL Init(const char* szQuery);
    BOOL UnInit();
    BOOL BindResult(MYSQL_BIND* pResults);
    INT32 Fetch();
    MYSQL_STMT* GetStmt() const { return stmt.get(); }

private:
    cqyh::MysqlPtr mysql;
    cqyh::MysqlStmtPtr stmt;
};

#endif // _PREP_STMT_H_
