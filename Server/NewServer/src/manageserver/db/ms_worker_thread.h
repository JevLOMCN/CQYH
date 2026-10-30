#ifndef _MS_WORKER_THREAD_H_
#define _MS_WORKER_THREAD_H_

#include "thread.h"
#include "circle_list.h"
#include "mysql_owner.h"
#include "db/db_operation.h"

struct DBSetting;
class CMsGuild;
class CMsGuildMember;

class CMSWorkerThread : public CThread
{
public:
    CMSWorkerThread();
    virtual ~CMSWorkerThread();

    BOOL Init(UINT32 dwResultCount, const DBSetting* dbsetting);
    BOOL UnInit();
    BOOL ReInit();

    DB_OPERATION_RESULT* GetResult();

protected:
    virtual VOID ThrdProc();

private:
    INT32 _CheckSqlResult(BOOL bRet, MYSQL_STMT* stmt);

    BOOL m_bServerOK;
    cqyh::MysqlPtr m_mysql;
    const DBSetting* m_setting;

    CCircleList m_requests;
    CCircleList m_results;
};

#endif // _MS_WORKER_THREAD_H_
