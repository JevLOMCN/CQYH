#ifndef _OA_AUTH_MGR_H_
#define _OA_AUTH_MGR_H_

#include "SdoBase.h"
#include "callback_queue.h"
#include "release_owner.h"
#include "id_object_pool.h"
#include "string_def.h"

struct SOAAuthingUser
{
    UINT32 dwSessionID;
    char szOASessionID[OA_SESSION_ID_LEN];
};

struct SOAAuthorizedUser
{
	UINT32 dwRpcID;
	INT32 nError;
	char szSNDAID[USER_NAME_LEN];
	char szAdult[OA_ADULT_FIELD_LEN];
	char szApplingAdult[OA_ADULT_FIELD_LEN];
    char szPGT[OA_PGT_FIELD_LEN];
};


class COAAuthMgr
{
public:
    COAAuthMgr();
    ~COAAuthMgr();

    BOOL Init(VOID);
    BOOL UnInit(VOID);
    static COAAuthMgr& GetInstance(VOID);
	BOOL MainLoop();

	BOOL DoAuthorize(INT32 nSessionID, const CHAR* pcszAuthingStr);
	VOID OnAuthorized(SOAAuthorizedUser* pstAuthorizedUser);
private:
	static COAAuthMgr ms_Instance;

	CIDObjectPool<SOAAuthingUser, UINT32>		m_AuthingUserPool;
	cqyh::CallbackQueue<SOAAuthorizedUser> m_lstAuthorizedUser;
	cqyh::ReleasePtr<ISDOCommonHandler> m_poOAAuth;
};

#endif