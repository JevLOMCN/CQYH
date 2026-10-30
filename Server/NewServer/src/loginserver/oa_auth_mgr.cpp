#include "stdafx.h"
#include "oa_auth_mgr.h"
#include "server_config.h"
#include "ls_message_handler.h"
#include <cstdio>
#include <memory>

COAAuthMgr COAAuthMgr::ms_Instance;

int SNDACALLBACK AuthorizeCallback(EMsgDirection eMsgType, unsigned int dwRpcID, int nResult, const ISDOCommonMsg* pResponse);

COAAuthMgr::COAAuthMgr()
{
	m_poOAAuth = NULL;
}

COAAuthMgr::~COAAuthMgr()
{
    UnInit();
}

BOOL COAAuthMgr::Init(VOID)
{
    if (m_poOAAuth || m_lstAuthorizedUser.IsInitialized())
    {
        LOG_CHECK_ERROR(!m_poOAAuth && !m_lstAuthorizedUser.IsInitialized());
        return FALSE;
    }
    INT32 nRetCode = 0;
	char szOAConfigFullPath[256];

	nRetCode = m_AuthingUserPool.Init(g_ServerConfig.LS.nInitAuthCount);
	LOG_PROCESS_ERROR(nRetCode);

	LOG_PROCESS_ERROR(g_ServerConfig.LS.nTotalAuthingCount > 0);
	nRetCode = m_lstAuthorizedUser.InitSlots(g_ServerConfig.LS.nTotalAuthingCount);
	LOG_PROCESS_ERROR(nRetCode);

	m_poOAAuth.reset(sdoCreateCommonHandler());
	LOG_PROCESS_ERROR(m_poOAAuth);
	
	snprintf(szOAConfigFullPath, 256, "loginserver\\%s", g_ServerConfig.LS.szConfigFile);
    INF("Starting Auth System Using Config: %s", szOAConfigFullPath);
	nRetCode = m_poOAAuth->Initialize(szOAConfigFullPath);
	LOG_PROCESS_ERROR(nRetCode == 0);
	
	m_poOAAuth->RegisterCallback("VerifyServer", AuthorizeCallback);
	
	return TRUE;
Exit0:
	nRetCode = UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return FALSE;
}

BOOL COAAuthMgr::UnInit(VOID)
{
    m_lstAuthorizedUser.Close();
    const auto stopped = cqyh::StopAndRelease(m_poOAAuth);
    LOG_CHECK_ERROR(stopped);
    m_lstAuthorizedUser.UnInit();
    const auto result = m_AuthingUserPool.UnInit();
    LOG_CHECK_ERROR(result);
    return TRUE;
}

COAAuthMgr& COAAuthMgr::GetInstance(VOID)
{
    return ms_Instance;
}

BOOL COAAuthMgr::MainLoop()
{	
	INT32 nRetCode = 0;
	SOAAuthingUser* pstAuthingUser = NULL;
	while (auto authorized = m_lstAuthorizedUser.Pop())
	{
		INT32 nError = errLoginAuthFailed;

		auto* pstAuthorizedUser = authorized.get();
		LOG_PROCESS_ERROR(pstAuthorizedUser);
		
		pstAuthingUser = m_AuthingUserPool.Find(pstAuthorizedUser->dwRpcID);
		LOG_PROCESS_ERROR(pstAuthingUser);

		if (pstAuthorizedUser->nError == 0)
			nError = errSuccess;
		else
			INF("session <%08X> oa authorized failed, error code %d", pstAuthingUser->dwSessionID, pstAuthorizedUser->nError);

		// Mobile client
		if (IS_MOBILE_CLIENT(pstAuthingUser->dwSessionID))
		{
			nRetCode = CLSMessageHandler::GetInstance().DoMGLoginAck(pstAuthingUser->dwSessionID, nError,
				pstAuthingUser->szOASessionID, pstAuthorizedUser->szSNDAID, pstAuthorizedUser->szPGT,
				strcmp(pstAuthorizedUser->szAdult, "Y") == 0 ? 1 : 0, 0, NULL);
			LOG_CHECK_ERROR(nRetCode);

			INF("User from session <%08X> get SNDA ID %s, and DoMGLoginAck", pstAuthingUser->dwSessionID, pstAuthorizedUser->szSNDAID);
		}
		// PC client
		else
		{
			nRetCode = CLSMessageHandler::GetInstance().DoGTLoginAck(pstAuthingUser->dwSessionID, nError,
				pstAuthingUser->szOASessionID, pstAuthorizedUser->szSNDAID, pstAuthorizedUser->szPGT,
				strcmp(pstAuthorizedUser->szAdult, "Y") == 0 ? 1 : 0, 0, NULL);
			LOG_CHECK_ERROR(nRetCode);

			INF("User from session <%08X> get SNDA ID %s, and DoGTLoginAck", pstAuthingUser->dwSessionID, pstAuthorizedUser->szSNDAID);
		}
	
		nRetCode = m_AuthingUserPool.Delete(pstAuthingUser);
		LOG_CHECK_ERROR(nRetCode);
	}
	
	return TRUE;
Exit0:
	return FALSE;
}

BOOL COAAuthMgr::DoAuthorize(INT32 nSessionID, const CHAR* pcszAuthingStr)
{
	INT32 nRetCode = 0;
	ISDOCommonMsg* pMsg = NULL;
	UINT32 dwRpcID = 0;
	SOAAuthingUser* pAuthUser = NULL;

	LOG_PROCESS_ERROR(pcszAuthingStr);
	LOG_PROCESS_ERROR(m_poOAAuth);

	dwRpcID = m_poOAAuth->GetRpcId();
	pAuthUser = m_AuthingUserPool.New(dwRpcID);
	LOG_PROCESS_ERROR(pAuthUser);
	pAuthUser->dwSessionID = nSessionID;
    strxcpy(pAuthUser->szOASessionID, pcszAuthingStr, OA_SESSION_ID_LEN);

	pMsg = sdoCreateMsg(E_REQUEST, "VerifyServer.ReserveNotify");
	LOG_PROCESS_ERROR(pMsg);
	
	pMsg->SetValue("sessionId", pcszAuthingStr);
	pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
	pMsg->SetValue("appId", SNDA_APPID);
	
	nRetCode = m_poOAAuth->ASyncSendMsg(pMsg, dwRpcID);
	LOG_PROCESS_ERROR(nRetCode == 0);

	pMsg->Release();

	return TRUE;
Exit0:
	
	if(pMsg)
		pMsg->Release();

	return FALSE;
}

VOID COAAuthMgr::OnAuthorized(SOAAuthorizedUser* record)
{
    const auto accepted = m_lstAuthorizedUser.Push(std::unique_ptr<SOAAuthorizedUser>(record));
    LOG_CHECK_ERROR(accepted);
}

#define dbg_auth_str(a) pAuthUser->GetValue(a, &pszMsg); DBG("%s: %s", a, pszMsg);
#define dbg_auth_int(a) pAuthUser->GetValue(a, &debug_int); DBG("%s: %d", a, debug_int);
int SNDACALLBACK AuthorizeCallback(EMsgDirection eMsgType, unsigned int dwRpcID, int nResult, const ISDOCommonMsg* pResponse) try
{
	std::unique_ptr<SOAAuthorizedUser> owned;
    SOAAuthorizedUser* pstAuthorizedUser = nullptr;
	char* pszMsg = NULL;
	ISDOCommonMsg* pAuthUser = NULL;
	
	LOG_PROCESS_ERROR(pResponse);

	pAuthUser = (ISDOCommonMsg*)pResponse;
	LOG_PROCESS_ERROR(pAuthUser);

	owned = std::make_unique<SOAAuthorizedUser>();
    pstAuthorizedUser = owned.get();
	LOG_PROCESS_ERROR(pstAuthorizedUser);

	pstAuthorizedUser->dwRpcID = dwRpcID;
	pstAuthorizedUser->nError = nResult;

	pAuthUser->GetValue("sndaId", &pszMsg);
	if(pszMsg != NULL)
	{
		strxcpy(pstAuthorizedUser->szSNDAID, pszMsg, USER_NAME_LEN);
		pszMsg = NULL;
	}

	pAuthUser->GetValue("adult", &pszMsg);
	if(pszMsg != NULL)
	{
		strxcpy(pstAuthorizedUser->szAdult, pszMsg, OA_ADULT_FIELD_LEN);
		pszMsg = NULL;
	}

	pAuthUser->GetValue("applingAdult", &pszMsg);
	if(pszMsg != NULL)
    {
		strxcpy(pstAuthorizedUser->szApplingAdult, pszMsg, OA_ADULT_FIELD_LEN);
        pszMsg = NULL;
    }

    pAuthUser->GetValue("proxyTicket", &pszMsg);
    if(pszMsg != NULL)
    {
        strxcpy(pstAuthorizedUser->szPGT, pszMsg, OA_PGT_FIELD_LEN);
        pszMsg = NULL;
    }

	COAAuthMgr::GetInstance().OnAuthorized(owned.release());

Exit0:
	return 0;
}
catch (...)
{
    std::fputs("Authentication callback failed\n", stderr);
    return 0;
}
