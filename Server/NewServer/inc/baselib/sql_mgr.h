#ifndef _SQL_MGR_H_
#define _SQL_MGR_H_

#include "thread.h"
#include "circle_list.h"
#include "base_list.h"
#include "sql_stmt.h"
#include "mysql_owner.h"

#define MAX_SQL_STMT_PARAM_COUNT    (256)
#define MAX_SQL_STMT_RESULT_COUNT   (32)
#define MAX_DB_WORKER_THREAD_COUNT  (8)
#define MAX_SQL_QUERY_LEN           (1024 * 8)

#include "mysql.h"

/************************************************************************/
/* INT8      c                                                          */
/* UINT8     uc                                                         */
/* INT16     h                                                          */
/* UINT16    uh                                                         */
/* INT32     d                                                          */
/* UINT32    ud                                                         */
/* INT64     l                                                          */
/* UINT64    ul                                                         */
/* string    s       length excludes the null terminator                                           */
/* blob	     b                                                          */
/* time_t    t       explicitly convert to time_t before passing                           */
/* zip blob  z                                                          */
/************************************************************************/

struct SQL_STMT_DATA
{
    INT32       nThreadIndex;
    INT32       nSqlStmtID;

    const char* pcszSql;
    char        szQuery[MAX_SQL_QUERY_LEN];
    INT32       nQuerySize;

    INT32       nParamCount;
    INT32       nParamType[MAX_SQL_STMT_PARAM_COUNT];
    INT32       nParamSize[MAX_SQL_STMT_PARAM_COUNT];
    bool        bIsUnsigned[MAX_SQL_STMT_PARAM_COUNT];
    INT32       nParamBufferSize;

    INT32       nResultCount;
    INT32       nResultType[MAX_SQL_STMT_RESULT_COUNT];
    INT32       nResultSize[MAX_SQL_STMT_RESULT_COUNT];
    INT32       nResultBufferSize;

    INT32       nBufferSize;
};

struct SQL_THREAD_PARAM 
{
    CSqlMgr     *pSqlMgr;
    INT32       nIndex;
};

struct SqlWorkerResources;

class CSqlMgr
{
public:
    CSqlMgr();
    ~CSqlMgr();

    BOOL Init(const char* psczServerAddr, const char* pcszUserName, const char* pcszPassword, 
        const char* pcszDBName, const char* pcszCharset, UINT32 dwServerPort,
        SQL_STMT_DATA* pSqlStmtList, INT32 nSqlStmtCount, INT32 nThreadCount);
    BOOL UnInit(void);
    //static CSqlMgr& GetInstance(void);
    BOOL MainLoop(void);

    BOOL AsyncExecSqlStmt(INT32 nSqlStmtID, SQL_STMT_CALLBACK_FUNC pCallbackFunc, void* pUserData, const char* pcszFormat, ...);
    BOOL SyncExecSqlStmt(INT32 nSqlStmtID, SQL_STMT_CALLBACK_FUNC pCallbackFunc, void* pUserData, const char* pcszFormat, ...);
    BOOL AsyncExecSqlStmtOwned(INT32 nSqlStmtID, SQL_STMT_CALLBACK callback, const char* pcszFormat, ...);
    BOOL SyncExecSqlStmtOwned(INT32 nSqlStmtID, SQL_STMT_CALLBACK callback, const char* pcszFormat, ...);

    const SQL_STMT_DATA* GetSqlStmtData(INT32 nSqlStmtID);
	inline BOOL IsCompleted(void);

private:
    BOOL _ParseSqlStmt(SQL_STMT_DATA* pSqlStmtData);
    CSqlStmt* _NewSqlStmt(INT32 nSqlStmtID);
    BOOL _DelSqlStmt(CSqlStmt* pSqlStmt);
    CSqlStmt* _ExecSqlStmt(INT32 nSqlStmtID, SQL_STMT_CALLBACK callback, void* pUserData, const char* pcszFormat, va_list valist);
    BOOL _WaitSqlStmt(CSqlStmt* statement);
    BOOL _CompleteSqlStmt(CSqlStmt* statement);
    CSqlStmt* _TakeCompleted(INT32 threadIndex);
    bool _PublishCompleted(INT32 threadIndex, CSqlStmt* statement);
    static THREAD_DECLARE(_WorkerThread)(void * pParam);
    static THREAD_DECLARE(_WorkerLoop)(CSqlMgr* manager, INT32 threadIndex, SqlWorkerResources& resources);

private:
    //static CSqlMgr  ms_Instance;

    std::unique_ptr<SQL_STMT_DATA[]> m_pSqlStmtList;
    INT32           m_nSqlStmtCount;
    cqyh::MysqlLibraryLease m_library;

    char            m_szServerAddr[MAX_PATH];
    char            m_szUserName[MAX_PATH];
    char            m_szPassword[MAX_PATH];
    char            m_szDBName[MAX_PATH];
    char            m_szCharset[MAX_PATH];
    UINT32          m_dwServerPort;

    INT32           m_nThreadCount;
    CThread         m_Thread[MAX_DB_WORKER_THREAD_COUNT];
    std::atomic<bool> m_bStopThread{false};
    CCircleList     m_PushList[MAX_DB_WORKER_THREAD_COUNT];
    CCircleList     m_PopList[MAX_DB_WORKER_THREAD_COUNT];
    INT32           m_nThreadIndex;

    std::unique_ptr<CBaseList[]> m_pSqlStmtPool;
	INT32			m_nNewSqlStmtCount;
	INT32			m_nDelSqlStmtCount;
};

inline BOOL CSqlMgr::IsCompleted(void)
{
	return m_nNewSqlStmtCount == m_nDelSqlStmtCount;
}

#endif  // _SQL_MGR_H_
