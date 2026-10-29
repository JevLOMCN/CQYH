#ifndef nameserver_h__
#define nameserver_h__

#include <map>
#include <set>
#include <string>

#include "thread.h"
#include "mysql_owner.h"
#include "circle_list.h"
#include "object_pool.h"

struct NS_CONFIG
{
	struct NET_CONFIG
	{
		char ip[COMMON_NAME_LEN];
		INT32 port;

		INT32 max_acceptor;
		INT32 max_connector;
		INT32 max_session;
		INT32 send_buf_size;
		INT32 recv_buf_size;
		INT32 packet_buf_size;
		INT32 small_packet_count;
		INT32 large_packet_count;
		INT32 sharemem_disconnect_timeout;
		INT32 ping_interval;
		INT32 ping_timeout;
	};

	struct DB_CONFIG
	{
		char ip[COMMON_NAME_LEN];
		INT32 port;

		char account[COMMON_NAME_LEN];
		char password[COMMON_NAME_LEN];
		char db_name[COMMON_NAME_LEN];
		char charset[COMMON_NAME_LEN];
		INT32 pool_size;
	};

	NET_CONFIG net_config;
	DB_CONFIG db_config;
};

extern NS_CONFIG g_NSConfig;

struct DBQueryData
{
	INT32	eType;
	INT32	nSessionID;
	UINT32	dwCliSessionID;
	UINT32	dwNewID;
	INT32	nErrCode;
	char	szName[COMMON_NAME_LEN];
    char    UserData[USER_DATA_LEN];
};

//struct DBResultData
//{
//	INT32	nErrCode;
//	INT32	eType;
//	INT32	nSessionID;
//	UINT32	dwNewID;
//	char	szNewName[COMMON_NAME_LEN];
//    char    UserData[USER_DATA_LEN];
//};

class CNameServer : public CThread
{
public:
	CNameServer(void);
	virtual ~CNameServer(void);

	static CNameServer& GetInstance(void);

	BOOL Init(void);
	void UnInit(void);

	BOOL MainLoop(void);

    INT32 AddNewData(INT32 eType, INT32 nSessionID, UINT32 dwCliSessionID, const char* szName, const char* UserData);

protected:
	virtual void ThrdProc(void);

	void _ThrdWork(void);
	BOOL _LoadData(void);

	BOOL _ExistDB(MYSQL* mysql, const char* dbname);
	BOOL _TryCreateDB(void);

	BOOL _ThrdInitDB(void);

private:
	static CNameServer ms_Instance;

	cqyh::MysqlPtr m_sql;

	CCircleList m_DBTaskList;
	CCircleList m_ResutList;

	CObjectPool<DBQueryData> m_DBQueryPool;
//	CObjectPool<DBResultData> m_DBResultPool;

	typedef std::map<std::string, UINT32> TNameMap;
	TNameMap m_NameMap[ntTotal];

	typedef std::set<std::string> TLockNameSet;
	TLockNameSet m_LockNameSet[ntTotal];

	std::atomic<bool> m_bDBReady{false};
};


#endif // nameserver_h__

