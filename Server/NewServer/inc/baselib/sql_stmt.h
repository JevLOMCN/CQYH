#ifndef _SQL_STMT_H_
#define _SQL_STMT_H_

#include "base_list.h"
#include "sql_results.h"
#include <functional>
#include <memory>
#include <utility>

struct SQL_STMT_DATA;
class CSqlStmt;
class CSqlMgr;

typedef BOOL (*SQL_STMT_CALLBACK_FUNC)(std::list<void*>& rResult, void* pUserData);
using SQL_STMT_CALLBACK = std::move_only_function<BOOL(std::list<void*>&, void*)>;

// The statement owns captured data until completion, rejection or shutdown.
// The function pointer receives a borrow and must not delete it.
template<class T, class Deleter>
SQL_STMT_CALLBACK MakeOwnedSqlCallback(SQL_STMT_CALLBACK_FUNC callback,
                                     std::unique_ptr<T, Deleter> data)
{
    if (!callback)
        return nullptr;
    return [callback, data = std::move(data)](std::list<void*>& rows, void*)
    {
        return callback(rows, data.get());
    };
}

class CSqlStmt : public BASE_LIST_NODE
{
public:
    BOOL PushParam(INT8 chValue);
    BOOL PushParam(INT16 shValue);
    BOOL PushParam(INT32 nValue);
    BOOL PushParam(INT64 llValue);

    BOOL PushParam(UINT8 byValue);
    BOOL PushParam(UINT16 wValue);
    BOOL PushParam(UINT32 dwValue);
    BOOL PushParam(UINT64 qwValue);

    BOOL PushTime(time_t tValue);
    BOOL PushParam(const char* pcszValue);
    BOOL PushParam(UINT8* pbyValue, UINT32 dwValueSize);
    BOOL PushParamZ(UINT8* pbyValue, UINT32 dwValueSize);

public:
    CSqlStmt();
    ~CSqlStmt();

    BOOL Init(CSqlMgr *pSqlMgr, INT32 nSqlStmtID);
    BOOL UnInit(void);

private:
    const SQL_STMT_DATA*    m_pSqlStmtData;
    INT32                   m_nParamCount;
    std::unique_ptr<UINT8[]> m_pbyParamBuffer;
    INT32                   m_nParamBufferSize;
    cqyh::SqlResults       m_Results;
    SQL_STMT_CALLBACK      m_pCallbackFunc;
    void*                   m_pUserData;
    INT32                   m_nExecuteCount;
    bool                    m_bSucceeded = false;

    friend class CSqlMgr;
    CSqlMgr*                 m_pSqlMgr;
};

#endif  // _SQL_STMT_H_
