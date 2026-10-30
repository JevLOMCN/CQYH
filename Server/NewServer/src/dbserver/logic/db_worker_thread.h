#ifndef _DB_WORKER_THREAD_H_
#define _DB_WORKER_THREAD_H_

#include "thread.h"
#include "circle_list.h"
#include "dbrole.h"
#include "dbuser.h"
#include "../db/db_create_role.h"
#include "../db/db_delete_role.h"
#include "../db/db_update_role_delete_time.h"
#include "../db/db_load_role.h"
#include "../db/db_save_role.h"
#include "../db/db_list_role.h"


class CDBWorkerThread : public CThread
{
public:
    CDBWorkerThread();
    virtual ~CDBWorkerThread();

    BOOL Init(UINT32 dwRoleCount);
    BOOL UnInit();
    BOOL ReInit();

    BOOL AddRoleDbTask(CDBRole* dbrole);
    CDBRole* GetFinishedRoleDbTask();

    BOOL AddRoleLockTask(CDBRole* dbrole);
    CDBRole* GetFinishedRoleLockTask();
    
    BOOL AddUserTask(CDBUser* dbuser);
    CDBUser* GetFinishedUserTask();

protected:
    virtual VOID ThrdProc();

private:
    BOOL m_bServerOK;

    INT32 CreateRole(CDBRole* dbrole);
    INT32 DeleteRole(CDBRole* dbrole);
    INT32 UpdateRoleDeleteTime(CDBRole* dbrole);
    INT32 SaveRole(UINT32 dwRoleID, DB_ROLE_DATA* data);
    INT32 LoadRole(UINT32 dwRoleID, DB_ROLE_DATA* data);
    INT32 ListRole(CDBUser* dbuser);

    BOOL SaveIfNeeded(CDBRole* dbrole);

    CCircleList m_RoleDbTaskList;
    CCircleList m_FinishedRoleDbTaskList;
    CCircleList m_RoleLockTaskList;
    CCircleList m_FinishedRoleLockTaskList;
    std::set<CDBRole*> m_roles;

    CCircleList m_UserTaskList;
    CCircleList m_FinishedUserTaskList;

    cqyh::MysqlPtr m_mysql;
    void CloseStatements();

    CDBCreateRole m_createrole;
    CDBDeleteRole m_deleterole;
    CDBUpdateRoleDeleteTime m_updateroledeletetime;
    CDBLoadRole m_loadrole;
    CDBSaveRole m_saverole;
    CDBListRole m_listrole;
};


#endif // _DB_WORKER_THREAD_H_
