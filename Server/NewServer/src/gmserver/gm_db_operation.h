#ifndef _H_DB_OPERATION_H_
#define _H_DB_OPERATION_H_

#include "mysql_owner.h"
#include <list>

#include "gm_db_def.h"
#include "string_def.h"

struct DB_OP_LOG_ITEM_GUID : DB_OPERATION_REQUEST 
{
    UINT64      llItemID;
};

struct DB_OP_LOG_ITEM_TEMPLATE_ID : DB_OPERATION_REQUEST 
{
    UINT32      dwTemplateID;
};

struct DB_OP_LOG_ITEM_GUID_ROLE : DB_OPERATION_REQUEST 
{
    UINT32      dwRoleID;
    UINT64      llItemID;
};

struct DB_OP_LOG_ITEM_TEMP_ID_ROLE : DB_OPERATION_REQUEST
{
    UINT32      dwRoleID;
    UINT32      dwTemplateID;
};

struct DB_OP_LOG_ITEM_ROLE : DB_OPERATION_REQUEST 
{
    UINT32      dwRoleID;
};

struct DB_ITEM_LOG_DATA
{
    INT32       nLogTime;
    INT8        bySvrGroupID;
    INT8        byAreaID;
    char        szActionType[9];
    UINT32      dwTemplateID;
    UINT64      llGuid;
    INT8        byCount;
    UINT32      dwSrcRoleID;
    UINT32      dwTargetRoleID;
    char        szReason[31];
};

struct DB_OP_LOG_ITEM_RESULT : DB_OPERATION_RESULT
{
    std::list<DB_ITEM_LOG_DATA>        listItemLogData;
};



class CDBItemLogWithGuid
{
public:
    CDBItemLogWithGuid();
    ~CDBItemLogWithGuid();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoLoadItemLog(UINT64 llGuid, DB_OP_LOG_ITEM_RESULT *res);

    cqyh::MysqlStmtPtr m_stmt;

private:
    UINT64  m_llGuid;
    MYSQL_BIND  m_bind_param;
    MYSQL_BIND  m_bind_result[10];
};

class CDBItemLogWithTempID
{
public:
    CDBItemLogWithTempID();
    ~CDBItemLogWithTempID();

    BOOL Init(MYSQL* mysql);
    BOOL UnInit();

    BOOL DoLoadItemLog(UINT32 dwTemplateID, DB_OP_LOG_ITEM_RESULT *res);

    cqyh::MysqlStmtPtr m_stmt;
private:
    UINT32  m_dwTemplateID;
    MYSQL_BIND  m_bind_param;
    MYSQL_BIND  m_bind_result[10];
};

class CDBItemLogWithRoleGuid
{
public:
    CDBItemLogWithRoleGuid();
    ~CDBItemLogWithRoleGuid();

    BOOL Init(MYSQL *mysql);
    BOOL UnInit();
    BOOL DoLoadItemLog(UINT32 dwRoleID, UINT64 llGuid, DB_OP_LOG_ITEM_RESULT *res);

    cqyh::MysqlStmtPtr m_stmt;
private:
    UINT32  m_dwRoleID;
    UINT64  m_llGuid;
    MYSQL_BIND  m_bind_param[3];
    MYSQL_BIND  m_bind_result[10];
};

class CDBItemLogWithRoleTempID
{
public:
    CDBItemLogWithRoleTempID();
    ~CDBItemLogWithRoleTempID();

    BOOL Init(MYSQL *mysql);
    BOOL UnInit();

    BOOL DoLoadItemLog(UINT32 dwRoleID, UINT32 dwTemplateID, DB_OP_LOG_ITEM_RESULT *res);

    cqyh::MysqlStmtPtr m_stmt;

private:
    UINT32      m_dwRoleID;
    UINT32      m_dwTemplateID;
    MYSQL_BIND  m_bind_param[3];
    MYSQL_BIND  m_bind_result[10];
};

class CDBItemLogWithRole
{
public:
    CDBItemLogWithRole();
    ~CDBItemLogWithRole();

    BOOL Init(MYSQL *mysql);
    BOOL UnInit();

    BOOL DoLoadItemLog(UINT32 dwRoleID, DB_OP_LOG_ITEM_RESULT *res);

    cqyh::MysqlStmtPtr m_stmt;

private:
    UINT32  m_dwRoleID;
    MYSQL_BIND  m_bind_param[2];
    MYSQL_BIND  m_bind_result[10];
};




#endif //_H_DB_OPERATION_H_


