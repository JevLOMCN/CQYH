/********************************************************************
	created:	20:12:2013   13:59
	file:		db_mgr.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/


#ifndef _H_MYSQL_MGR_H_
#define _H_MYSQL_MGR_H_

#include "server_def.h"
#include "gm_data.h"
#include "mysql.h"
#include "sql_mgr.h"

//struct  SVR_DB_DATA
//{
//    UINT32      dwSvrID;
//    DBSetting   DBData;
//};


class CRole;


class CDBMgr
{
protected:
    CDBMgr();
    ~CDBMgr();

    static CDBMgr       m_Instance;

public:
    static  CDBMgr&  GetInstance();

    BOOL    Init();
    BOOL    UnInit();

public:
    DBSetting   *GetDBSetting();
    DBSetting   *GetSlaverDBSetting();

    cqyh::MysqlPtr GetMysql( );
    cqyh::MysqlPtr GetSlaverMysql();

    cqyh::MysqlPtr GetLogMysql( );
    cqyh::MysqlPtr GetSlaverLogMysql();


    BOOL        PreQueryRoleID(CMD_DATA_BASE *pCmd);
    BOOL        QueryRoleID(CMD_DATA_BASE *pCmd);
    BOOL        QueryRoleData(CMD_DATA_BASE *pCmd);
    BOOL        QueryGuildID(CMD_DATA_BASE *pCmd);
    BOOL        QueryGMForbid(CMD_DATA_BASE *pCmd);
    BOOL        QueryMail(CMD_DATA_BASE *pCmd);
    BOOL        ForbiddenChat(CMD_DATA_BASE *pCmd);
    BOOL        FreezeRole(CMD_DATA_BASE *pCmd);
    BOOL        SendMail(CMD_DATA_BASE *pCmd);
    BOOL        QueryLog(CMD_DATA_BASE *pCmd);
    BOOL        LoadRole(UINT32 dwSerialID, UINT32 dwRoleID);
    BOOL        LoadMail(CMD_DATA_BASE *pCmd);
    BOOL        RoleExeGMCmd(CMD_DATA_BASE *pCmd);
    BOOL        DelMail(UINT32 dwMailID);
    BOOL        CheckRoleFreeze(CMD_DATA_BASE *pCmd);

    BOOL        SaveRoleData(CRole *poRole);

    BOOL        QueryNameToID(CMD_DATA_BASE *pCmd);


    BOOL        QueryExchangeLog(UINT32 dwSessionID, INT32 nGroupID);
    BOOL        QueryExchangeHistory(UINT32 dwSessionID, INT32 nGroupID);

    BOOL        QuerySysExchangeOrder(CMD_DATA_BASE *pCmd);
    BOOL        QuerySysExchangeLog(CMD_DATA_BASE* pCmd);

    BOOL    MainLoop();

protected:
    inline CSqlMgr &   GetMasterSqlMgr()       { return m_MasterSqlMgr; }
    inline CSqlMgr &   GetSlaverSqlMgr()       { return m_SlaverSqlMgr; }
    inline CSqlMgr &   GetLogDataSqlMgr()      { return m_LogDataSqlMgr; }

private:
    //typedef     std::map<UINT32, SVR_DB_DATA>   DB_MAP;
    //DB_MAP         m_DBMap;

    DBSetting   m_DBData;
    DBSetting   m_DBLogData;
    DBSetting   m_SlaverDBData;


    CSqlMgr     m_MasterSqlMgr;
    CSqlMgr     m_SlaverSqlMgr;
    CSqlMgr     m_LogDataSqlMgr;


};

BOOL LoadRole(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData);
BOOL LoadGuild(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData);
BOOL LoadGMForbid(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData);
BOOL LoadMail(CMD_ROLE_DATA_BASE *pRoleCmd, INT32 &nLength, char *pData);
INT32 ModifyRoleData(CMD_MODIFY_ROLE *pModifyCmd);

BOOL ExeLuaGMCmd(CMD_ROLE_EXE_GM_CMD *pCmd);



// Query callbacks borrow both rows and user data from their owning SQL statement.
BOOL CB_DBPreQueryRoleID(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryRoleID(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryGuildID(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryGMForbid(std::list<void*>& rResult, void* pUserData);

//BOOL CB_DBForbiddenChat(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBSelectMailID(std::list<void*>& rResult, void* pUserData);


BOOL CB_DBLoadRole(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBLoadMail(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBRoleExeCmd(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBCheckRoleFreeze(std::list<void*>& rResult, void* pUserData);


BOOL CB_DBSaveRole(std::list<void*>& rResult, void* pUserData);

BOOL CB_DBQueryNameToID(std::list<void*>& rResult, void* pUserData);



BOOL CB_DBQueryExchangeLog(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryExchangeHistory(std::list<void*>& rResult, void* pUserData);




BOOL CB_DBQuerySysExchangeOrderInfo(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQuerySysExchangeLogInfo(std::list<void*>& rResult, void* pUserData);



// log
BOOL CB_DBQueryLogRoleLogin(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryLogRoleLogout(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryLogItem(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryLogMoney(std::list<void*>& rResult, void* pUserData);
BOOL CB_DBQueryLogQuest(std::list<void*>& rResult, void* pUserData);










#endif //_H_MYSQL_MGR_H_
















