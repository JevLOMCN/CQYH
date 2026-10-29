#include "stdafx.h"
#include "oa_bill_mgr.h"
#include "server_config.h"
#include "role_data.h"
#include "client/user.h"
#include "client/usermgr.h"
#include <time.h>
#include <cstdio>
#include <memory>
#include "role_data_mgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "real_money_shop.h"
#include "message_handler.h"
#include "exchange/exchange_mgr.h"
#include "manageserver.h"

int SNDACALLBACK BillingCallback(EMsgDirection eMsgType, unsigned int dwRpcId, int nResult, const ISDOCommonMsg* pResponse);
int SNDACALLBACK NotifyCallback(EMsgDirection eMsgType, unsigned int dwRpcId, int nResult, const ISDOCommonMsg* pResponse);

COABillMgr::COABillMgr()
{
    m_poOABill = NULL;
}

BOOL COABillMgr::Init()
{
    if (m_poOABill || m_ResultList.IsInitialized())
    {
        LOG_CHECK_ERROR(!m_poOABill && !m_ResultList.IsInitialized());
        return FALSE;
    }
    INT32 nRetCode = 0;
    char szOAConfigFullPath[256];

    nRetCode = m_RequestPool.Init(g_ServerConfig.MS.nOABillingRequestCount);
    LOG_PROCESS_ERROR(nRetCode);

    LOG_PROCESS_ERROR(g_ServerConfig.MS.nOABillingResultCount > 0);
    nRetCode = m_ResultList.InitSlots(g_ServerConfig.MS.nOABillingResultCount);
    LOG_PROCESS_ERROR(nRetCode);

    LOG_PROCESS_ERROR(m_poOABill == NULL);
    m_poOABill.reset(sdoCreateCommonHandler());
    LOG_PROCESS_ERROR(m_poOABill);

    snprintf(szOAConfigFullPath, 256, "manageserver\\%s", g_ServerConfig.MS.szConfigFile);
    INF("Starting Billing System Using Config: %s", szOAConfigFullPath);
    nRetCode = m_poOABill->Initialize(szOAConfigFullPath);
    LOG_PROCESS_ERROR(nRetCode == 0);

    m_poOABill->RegisterCallback("BillingService", BillingCallback);
    m_poOABill->RegisterCallback("bl3-notify", NotifyCallback);

    return TRUE;
Exit0:
    nRetCode = UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return FALSE;
}

BOOL COABillMgr::UnInit()
{
    m_ResultList.Close();
    const auto stopped = cqyh::StopAndRelease(m_poOABill);
    LOG_CHECK_ERROR(stopped);
    m_ResultList.UnInit();
    m_RequestPool.UnInit();
    return TRUE;
}

BOOL COABillMgr::MainLoop()
{
    while (auto result = m_ResultList.Pop())
    {
        SOARequest* req = NULL;
        SOAResult* res = result.get();
        LOG_PROCESS_ERROR(res);

        req = m_RequestPool.Find(res->dwRpcId);
        LOG_PROCESS_ERROR(req);

        if (req->eType == SOARequest::ertQuery ||
            req->eType == SOARequest::ertCharge ||
            req->eType == SOARequest::ertFreeze ||
            req->eType == SOARequest::ertUnfreeze ||
            req->eType == SOARequest::ertConfirm ||
            req->eType == SOARequest::ertTransfer)
        {
            MS_USER* user = NULL;
            if (req->eType == SOARequest::ertQuery)
            {
                user = CUserMgr::GetInstance().FindUserByUserName(req->szUserName);
            }
            else
            {
                MS_ONLINE_ROLE_DATA* role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(req->dwRoleID);
                if (role)
                    user = role->pUser;
            }

            if (user)
            {
                if (res->nResult == 0)
                {
                    INT32 paytype = 0;
                    INT32 amount = 0;
                    char* temp_str = res->szBalanceInfo;

                    char* tok = strtok(temp_str, "#");
                    while (tok)
                    {
                        sscanf(tok, "%d|%d", &paytype, &amount);
                        if (paytype == 3)
                        {
                            user->nDianquan = amount;
                            break;
                        }

                        tok = strtok(NULL, "#");
                    }

                    CMessageHandler::GetInstance().DoClientSyncRealMoney(user->dwCliSessionID, user->nDianquan);
                }
            }
        }

        if (req->eType == SOARequest::ertQueryTime)
        {
            MS_USER* user = CUserMgr::GetInstance().FindUserByUserName(req->szUserName);
            if (user)
            {
                if (res->nResult == 0)
                {
                    INT32 timetype = 0;
                    INT32 timeamount = 0;
                    char* temp_str = res->szBalanceInfo;

                    user->nTimeLeft = 0;
                    char* tok = strtok(temp_str, "#");
                    while (tok)
                    {
                        sscanf(tok, "%d|%d", &timetype, &timeamount);
                        if (timetype == 2)
                        {
                            user->nTimeLeft = timeamount;
                            break;
                        }

                        tok = strtok(NULL, "#");
                    }

                    CMessageHandler::GetInstance().DoClientTimeInfo(user->dwCliSessionID, user->nTimeLeft);
                    INF("Query Time Result: UserID:%s, Amount:%d", req->szUserName, user->nTimeLeft);

                    if (user->eState == USER_STATE_INGAME && user->bFreeMode && user->nTimeLeft > 0)
                        user->SetFreeMode(FALSE);
                }
            }
        }

        if ((req->eType == SOARequest::ertQuery && res->nResult != 0) ||
            req->eType == SOARequest::ertCharge ||
            req->eType == SOARequest::ertFreeze ||
            req->eType == SOARequest::ertUnfreeze ||
            req->eType == SOARequest::ertConfirm ||
            req->eType == SOARequest::ertTransfer ||
            (req->eType == SOARequest::ertQueryTime && res->nResult != 0) ||
            req->eType == SOARequest::ertSubTime ||
            req->eType == SOARequest::ertBuyTime ||
            req->eType == SOARequest::ertBuyTimeAuto)
        {
            CMessageHandler::GetInstance().DoDLBillingResult(req->nGroupID, req->eType, res->dwRpcId, req->szUniqueID, res->nResult);
        }

        switch (req->eType)
        {
            case SOARequest::ertQuery:
                break;
            case SOARequest::ertCharge:
                CRealMoneyShop::GetInstance().OnCharge(res->nResult, req->dwRoleID, req->charge.nItemID, req->charge.nItemCount, req->charge.szRoleName);
                break;
            case SOARequest::ertFreeze:
                CExchangeMgr::GetInstance().OnBillingFreeze(res->nResult, req->dwRoleID, req->freeze.nPrice, req->freeze.nCount, req->szUniqueID);
                break;
            case SOARequest::ertUnfreeze:
                CExchangeMgr::GetInstance().OnBillingUnfreeze(res->nResult);
                break;
            case SOARequest::ertConfirm:
                CExchangeMgr::GetInstance().OnBillingConfirm(res->nResult, req->confirm.qwOrderID, req->dwRoleID, req->confirm.nCount, req->confirm.nMoney, req->szUniqueID);
                break;
            case SOARequest::ertTransfer:
                CExchangeMgr::GetInstance().OnBillingTransfer(res->nResult, req->transfer.qwBuyingOrderID, req->transfer.dwBuyerRoleID, req->dwRoleID, req->transfer.nCount, req->transfer.nMoney, req->szUniqueID);
                break;
            case SOARequest::ertQueryTime:
                break;
            case SOARequest::ertSubTime:
                OnSubTime(req, res);
                break;
            case SOARequest::ertBuyTime:
                OnBuyTime(req, res, FALSE);
                break;
            case SOARequest::ertBuyTimeAuto:
                OnBuyTime(req, res, TRUE);
                break;
            default:
                LOG_CHECK_ERROR(FALSE);
        }

        m_RequestPool.Delete(req);
    }

    if (CManageServer::GetInstance().GetGameFrame() % (GAME_FPS * 15) == 0)
    {
        struct CheckOverTime
        {
            UINT32 dwCurTime;
            UINT32 dwReqCount;
            BOOL operator()(UINT32 dwRpcID, SOARequest* req)
            {
                dwReqCount++;
                if (!req->bOverTime && req->dwReqTime + 30 < dwCurTime)
                {
                    req->bOverTime = 1;
                    CMessageHandler::GetInstance().DoDLBillingResult(req->nGroupID, req->eType, dwRpcID, req->szUniqueID, 99999999);
                }
                return TRUE;
            }
        };
        CheckOverTime cot;
        cot.dwCurTime = CManageServer::GetInstance().GetGameTime();
        cot.dwReqCount = 0;
        m_RequestPool.Traverse(cot);

        DBG("Unprocessed Requests: %u", cot.dwReqCount);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL COABillMgr::DoQueryAccount(MS_USER* user)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(user);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.accountQuery");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userId", user->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", GET_GROUP_INDEX_FROM_SESSION_ID(user->dwCliSessionID));
    pMsg->SetValue("sessionId", user->szOASessionID);
    pMsg->SetValue("payTypeId", 3);
    pMsg->SetValue("couplingType", 2);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", inet_ntoa(*(in_addr*)(&user->dwClientIp)));
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("endpointMAC", user->szMac);

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertQuery;
    req->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(user->dwCliSessionID);
    req->dwRoleID = 0;
    strxcpy(req->szUserName, user->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    DBG("Query Request Sent: RpcID:%u", dwRpcId);

    return TRUE;

Exit0:
    if (user)
        CMessageHandler::GetInstance().DoErrorNotify(user->dwCliSessionID, errCannotConnectSndaServer, nRetCode, 0);
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoCharge(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nMoney, INT32 nItemID, INT32 nItemCount, const char* szRoleName)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR(nMoney > 0);
    LOG_PROCESS_ERROR(nItemID >= 0);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.currencyFreeze");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userId", pstOnlineRole->pRoleData->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", pstOnlineRole->pRoleData->byGroup);
    pMsg->SetValue("sessionId", pstOnlineRole->pUser->szOASessionID);

    m_poOABill->GetGUID(szTempString);
    pMsg->SetValue("orderId", szTempString);

    pMsg->SetValue("appTradeType", 2);
    pMsg->SetValue("couplingType", 2);

    snprintf(szTempString, 256, "3|%d", nMoney);
    pMsg->SetValue("payInfo", szTempString);

    pMsg->SetValue("freezePeriod", 0);
    pMsg->SetValue("freezeType", 2);
    pMsg->SetValue("goodsNum", 1);

    snprintf(szTempString, 256, "%d,%d,%d,%d,%d,%d", nItemID, SNDA_APPID, g_ServerConfig.Common.nAreaID, 1, nItemCount, nMoney);
    pMsg->SetValue("goodsInfo", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", inet_ntoa(*(in_addr*)(&pstOnlineRole->pUser->dwClientIp)));
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", pstOnlineRole->pRoleData->byLevel);
    pMsg->SetValue("roleName", pstOnlineRole->pRoleData->szRoleName);
    pMsg->SetValue("endpointMAC", pstOnlineRole->pUser->szMac);

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertCharge;
    req->nGroupID = pstOnlineRole->pRoleData->byGroup;
    req->dwRoleID = pstOnlineRole->pRoleData->dwRoleID;
    strxcpy(req->szUserName, pstOnlineRole->pRoleData->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->charge.nItemID = nItemID;
    req->charge.nItemCount = nItemCount;
    req->charge.nTotalCost = nMoney;
    if (szRoleName && strlen(szRoleName) > 0)
        strxcpy(req->charge.szRoleName, szRoleName, ROLE_NAME_LEN);

    DBG("Charge Request Sent: RpcID:%u, Amount:%d", dwRpcId, nMoney);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(pstOnlineRole->pRoleData->byGroup, SOARequest::ertCharge, dwRpcId,
        pstOnlineRole->pRoleData->szUserName, "", szUniqueID, "", pstOnlineRole->pRoleData->dwRoleID, 0, 0, 0, nMoney, 0);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pstOnlineRole)
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, errCannotConnectSndaServer, nRetCode, 0);
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoFreezeCurrency(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nMoney, INT32 nPrice, INT32 nCount)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR(nMoney > 0);
    LOG_PROCESS_ERROR(nMoney == nPrice * nCount);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.currencyFreeze");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szTempString);
    pMsg->SetValue("uniqueId", szTempString);

    pMsg->SetValue("userId", pstOnlineRole->pRoleData->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", pstOnlineRole->pRoleData->byGroup);
    pMsg->SetValue("sessionId", pstOnlineRole->pUser->szOASessionID);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("orderId", szUniqueID);

    pMsg->SetValue("appTradeType", 8);
    pMsg->SetValue("couplingType", 2);

    snprintf(szTempString, 256, "3|%d", nMoney);
    pMsg->SetValue("payInfo", szTempString);

    pMsg->SetValue("freezePeriod", 7 * 86400);
    pMsg->SetValue("freezeType", 1);
    pMsg->SetValue("goodsNum", 1);

    snprintf(szTempString, 256, "%d,%d,%d,%d,%d,%d", MONEYTYPE_02, SNDA_APPID, g_ServerConfig.Common.nAreaID, 1, nCount, nMoney);
    pMsg->SetValue("goodsInfo", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", inet_ntoa(*(in_addr*)(&pstOnlineRole->pUser->dwClientIp)));
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", pstOnlineRole->pRoleData->byLevel);
    pMsg->SetValue("roleName", pstOnlineRole->pRoleData->szRoleName);
    pMsg->SetValue("endpointMAC", pstOnlineRole->pUser->szMac);

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertFreeze;
    req->nGroupID = pstOnlineRole->pRoleData->byGroup;
    req->dwRoleID = pstOnlineRole->pRoleData->dwRoleID;
    strxcpy(req->szUserName, pstOnlineRole->pRoleData->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->freeze.nPrice = nPrice;
    req->freeze.nCount = nCount;

    DBG("Freeze Request Sent: RpcID:%u, Amount:%d", dwRpcId, nMoney);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(pstOnlineRole->pRoleData->byGroup, SOARequest::ertFreeze, dwRpcId,
        pstOnlineRole->pRoleData->szUserName, "", szUniqueID, "", pstOnlineRole->pRoleData->dwRoleID, 0, 0, 0, nMoney, 0);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pstOnlineRole)
        CMessageHandler::GetInstance().DoErrorNotify(pstOnlineRole->dwCliSessionID, errCannotConnectSndaServer, nRetCode, 0);
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoUnfreezeCurrency(UINT32 dwRoleID, const char* szBillingOrderID, INT32 nMoney, UINT64 qwOrderID)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;
    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(dwRoleID);

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(nMoney > 0);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.currencyThaw");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userId", role->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", role->byGroup);

    m_poOABill->GetGUID(szTempString);
    pMsg->SetValue("sessionId", szTempString); //Cheng Qinhui said billing does not validate sessionID, so use a random 32-digit value

    pMsg->SetValue("orderId", szBillingOrderID);
    pMsg->SetValue("confirm", 2);
    pMsg->SetValue("thawType", 3);
    pMsg->SetValue("appTradeType", 8);
    pMsg->SetValue("couplingType", 2);

    snprintf(szTempString, 256, "3|%d", nMoney);
    pMsg->SetValue("payInfo", szTempString);

    pMsg->SetValue("goodsNum", 1);

    snprintf(szTempString, 256, "%d,%d,%d,%d,%d,%d", MONEYTYPE_02, SNDA_APPID, g_ServerConfig.Common.nAreaID, 1, 1, nMoney);
    pMsg->SetValue("goodsInfo", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", "0.0.0.0");
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", role->byLevel);
    pMsg->SetValue("roleName", role->szRoleName);
    pMsg->SetValue("endpointMAC", "11-11-11-11-11-11");

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertUnfreeze;
    req->nGroupID = role->byGroup;
    req->dwRoleID = dwRoleID;
    strxcpy(req->szUserName, role->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->unfreeze.nMoney = nMoney;

    DBG("Unfreeze Request Sent: RpcID:%u, Amount:%d", dwRpcId, nMoney);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(role->byGroup, SOARequest::ertUnfreeze, dwRpcId,
        role->szUserName, "", szUniqueID, szBillingOrderID, dwRoleID, 0, qwOrderID, 0, nMoney, 0);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoConfirmFrozenCurrency(UINT64 qwBuyingOrderID, UINT64 qwSellingOrderID, UINT32 dwRoleID, const char* szBillingOrderID, INT32 nCount, INT32 nMoney)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;
    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(dwRoleID);

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(nMoney > 0);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.currencyThaw");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userId", role->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", role->byGroup);

    m_poOABill->GetGUID(szTempString);
    pMsg->SetValue("sessionId", szTempString); //Cheng Qinhui said billing does not validate sessionID, so use a random 32-digit value

    pMsg->SetValue("orderId", szBillingOrderID);
    pMsg->SetValue("confirm", 1);
    pMsg->SetValue("thawType", 7);
    pMsg->SetValue("appTradeType", 8);
    pMsg->SetValue("couplingType", 2);

    snprintf(szTempString, 256, "3|%d", nMoney);
    pMsg->SetValue("payInfo", szTempString);

    pMsg->SetValue("goodsNum", 1);

    snprintf(szTempString, 256, "%d,%d,%d,%d,%d,%d", MONEYTYPE_02, SNDA_APPID, g_ServerConfig.Common.nAreaID, 1, nCount, nMoney);
    pMsg->SetValue("goodsInfo", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", "0.0.0.0");
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", role->byLevel);
    pMsg->SetValue("roleName", role->szRoleName);
    pMsg->SetValue("endpointMAC", "11-11-11-11-11-11");

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertConfirm;
    req->nGroupID = role->byGroup;
    req->dwRoleID = dwRoleID;
    strxcpy(req->szUserName, role->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->confirm.qwOrderID = qwBuyingOrderID;
    req->confirm.nCount = nCount;
    req->confirm.nMoney = nMoney;

    DBG("Confirm Request Sent: RpcID:%u, Amount:%d", dwRpcId, nMoney);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(role->byGroup, SOARequest::ertConfirm, dwRpcId,
        role->szUserName, "", szUniqueID, szBillingOrderID, dwRoleID, 0, qwBuyingOrderID, qwSellingOrderID, nMoney, 0);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoTransferCurrency(UINT64 qwBuyingOrderID, UINT64 qwSellingOrderID, MS_ROLE_DATA* buyer, MS_ROLE_DATA* seller,
    const char* szBillingOrderID, INT32 nCount, INT32 nMoney, INT32 nTax)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(buyer);
    LOG_PROCESS_ERROR(seller);
    LOG_PROCESS_ERROR(nCount > 0);
    LOG_PROCESS_ERROR(nMoney > 0);
    LOG_PROCESS_ERROR(nTax > 0);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.consignTransfer");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userIdDest", seller->szUserName);
    pMsg->SetValue("userIdTypeDest", 2);
    pMsg->SetValue("spIdDest", 7);
    pMsg->SetValue("appIdDest", SNDA_APPID);
    pMsg->SetValue("areaIdDest", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupIdDest", seller->byGroup);

    pMsg->SetValue("userIdSrc", buyer->szUserName);
    pMsg->SetValue("userIdTypeSrc", 2);
    pMsg->SetValue("spIdSrc", 7);
    pMsg->SetValue("appIdSrc", SNDA_APPID);
    pMsg->SetValue("areaIdSrc", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupIdSrc", buyer->byGroup);

    m_poOABill->GetGUID(szTempString);
    pMsg->SetValue("sessionId", szTempString); //Cheng Qinhui said billing does not validate sessionID, so use a random 32-digit value
    pMsg->SetValue("orderId", szBillingOrderID);
    pMsg->SetValue("appTradeType", 8);
    pMsg->SetValue("couplingType", 2);

    snprintf(szTempString, 256, "3|%d", nMoney - nTax);
    pMsg->SetValue("payInfo", szTempString);

    snprintf(szTempString, 256, "3|%d", nTax);
    pMsg->SetValue("feeInfoSrc", szTempString);

    pMsg->SetValue("transferType", 3);

    pMsg->SetValue("goodsNum", 1);

    snprintf(szTempString, 256, "%d,%d,%d,%d,%d,%d", MONEYTYPE_02, SNDA_APPID, g_ServerConfig.Common.nAreaID, 1, nCount, nMoney);
    pMsg->SetValue("goodsInfo", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", "0.0.0.0");
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", buyer->byLevel);
    pMsg->SetValue("roleName", buyer->szRoleName);
    pMsg->SetValue("endpointMAC", "11-11-11-11-11-11");

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertTransfer;
    req->nGroupID = seller->byGroup;
    req->dwRoleID = seller->dwRoleID;
    strxcpy(req->szUserName, seller->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->transfer.qwBuyingOrderID = qwBuyingOrderID;
    req->transfer.dwBuyerRoleID = buyer->dwRoleID;
    req->transfer.nCount = nCount;
    req->transfer.nMoney = nMoney;

    DBG("Transfer Request Sent: RpcID:%u, Amount:%d", dwRpcId, nMoney);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(seller->byGroup, SOARequest::ertTransfer, dwRpcId,
        seller->szUserName, buyer->szUserName, szUniqueID, szBillingOrderID, seller->dwRoleID, buyer->dwRoleID, qwBuyingOrderID, qwSellingOrderID, nMoney, nTax);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoQueryTime(MS_USER* pstUser)
{
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN] = { 0 };
    char szTempString[256] = { 0 };
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(pstUser);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.timeAuth");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userId", pstUser->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", GET_GROUP_INDEX_FROM_SESSION_ID(pstUser->dwCliSessionID));
    //pMsg->SetValue("pgt", pstUser->szPGT);

    strxcpy(szTempString, pstUser->szOASessionID, 32);//Billing requires truncation to 32 bytes
    pMsg->SetValue("sessionId", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", inet_ntoa(*(in_addr*)(&pstUser->dwClientIp)));
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("endpointMAC", pstUser->szMac);

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertQueryTime;
    req->nGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(pstUser->dwCliSessionID);
    req->dwRoleID = 0;
    strxcpy(req->szUserName, pstUser->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    DBG("Query Time Request Sent: RpcID:%u", dwRpcId);

    return TRUE;

Exit0:
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoSubTime(MS_ONLINE_ROLE_DATA* pstOnlineRole, UINT32 dwBeginTime, UINT32 dwEndTime, INT32 nTimeInterval, INT32 nStatus)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN] = { 0 };
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);
    LOG_PROCESS_ERROR(dwEndTime >= dwBeginTime);
    LOG_PROCESS_ERROR(nTimeInterval > 0);
    LOG_PROCESS_ERROR(nStatus > etbtInvalid && nStatus < etbtTotal);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.timeBilling");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);
    pMsg->SetValue("sessionId", szUniqueID);//Billing requires a numeric session ID

    pMsg->SetValue("userId", pstOnlineRole->pRoleData->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", pstOnlineRole->pRoleData->byGroup);
    //pMsg->SetValue("pgt", pstOnlineRole->pUser->szPGT);
    pMsg->SetValue("appTradeType", 1);
    pMsg->SetValue("payTypeId", 2);
    pMsg->SetValue("amount", nTimeInterval);

    sprintf(szTempString, "%d", nStatus);
    pMsg->SetValue("status", szTempString);

    cur_time = dwBeginTime;
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("beginTime", szTempString);

    cur_time = dwEndTime;
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("endTime", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", inet_ntoa(*(in_addr*)(&pstOnlineRole->pUser->dwClientIp)));
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", pstOnlineRole->pRoleData->byLevel);
    pMsg->SetValue("roleName", pstOnlineRole->pRoleData->szRoleName);
    pMsg->SetValue("endpointMAC", pstOnlineRole->pUser->szMac);

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    req->eType = SOARequest::ertSubTime;
    req->nGroupID = pstOnlineRole->pRoleData->byGroup;
    req->dwRoleID = pstOnlineRole->pRoleData->dwRoleID;
    strxcpy(req->szUserName, pstOnlineRole->pRoleData->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->subtime.nTimeAmount = nTimeInterval;
    req->subtime.nStatus = nStatus;

    DBG("Sub Time Request Sent: RpcID:%u", dwRpcId);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(pstOnlineRole->pRoleData->byGroup, SOARequest::ertSubTime, dwRpcId,
        pstOnlineRole->pRoleData->szUserName, "", szUniqueID, "", pstOnlineRole->pRoleData->dwRoleID, 0, 0, 0, nTimeInterval, nStatus);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

BOOL COABillMgr::DoBuyTime(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nPayAmount, INT32 nTimeAmount, BOOL bAuto)
{
    char szTempString[256] = { 0 };
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN] = { 0 };
    INT32 nRetCode = 0;
    UINT32 dwRpcId = 0;
    time_t cur_time;
    SOARequest* req = NULL;
    ISDOCommonMsg* pMsg = NULL;

    LOG_PROCESS_ERROR(m_poOABill);
    LOG_PROCESS_ERROR(pstOnlineRole);
    LOG_PROCESS_ERROR(pstOnlineRole->pUser);

    pMsg = sdoCreateMsg(E_REQUEST, "BillingService.CurrencyExchange");
    LOG_PROCESS_ERROR(pMsg);

    m_poOABill->GetGUID(szUniqueID);
    pMsg->SetValue("uniqueId", szUniqueID);

    pMsg->SetValue("userId", pstOnlineRole->pRoleData->szUserName);
    pMsg->SetValue("userIdType", 2);
    pMsg->SetValue("spId", 7);
    pMsg->SetValue("appId", SNDA_APPID);
    pMsg->SetValue("areaId", g_ServerConfig.Common.nAreaID);
    pMsg->SetValue("groupId", pstOnlineRole->pRoleData->byGroup);
    //pMsg->SetValue("pgt", pstOnlineRole->pUser->szPGT);
    pMsg->SetValue("sessionId", pstOnlineRole->pUser->szOASessionID);
    pMsg->SetValue("appTradeType", 7);
    pMsg->SetValue("couplingType", 2);

    sprintf(szTempString, "3|%d", nPayAmount);
    pMsg->SetValue("exchangeFromInfo", szTempString);

    //spId|appId|areaId|groupId|payTypeId|amount
    sprintf(szTempString, "7|%d|%d|%d|2|%d", SNDA_APPID, g_ServerConfig.Common.nAreaID, pstOnlineRole->pRoleData->byGroup, nTimeAmount);
    pMsg->SetValue("exchangeToInfo", szTempString);

    time(&cur_time);
    strftime(szTempString, 256, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    pMsg->SetValue("eventTimestamp", szTempString);

    pMsg->SetValue("endpointOS", 1);
    pMsg->SetValue("endpointType", 1);
    pMsg->SetValue("endpointIp", inet_ntoa(*(in_addr*)(&pstOnlineRole->pUser->dwClientIp)));
    pMsg->SetValue("frameType", 1);
    pMsg->SetValue("roleLevel", pstOnlineRole->pRoleData->byLevel);
    pMsg->SetValue("roleName", pstOnlineRole->pRoleData->szRoleName);
    pMsg->SetValue("endpointMAC", pstOnlineRole->pUser->szMac);

    dwRpcId = m_poOABill->GetRpcId();
    nRetCode = m_poOABill->ASyncSendMsg(pMsg, dwRpcId);
    LOG_PROCESS_ERROR(nRetCode == 0);

    pMsg->Release();

    req = m_RequestPool.New(dwRpcId);
    LOG_PROCESS_ERROR(req);

    memset(req, 0, sizeof(SOARequest));
    if (bAuto)
        req->eType = SOARequest::ertBuyTimeAuto;
    else
        req->eType = SOARequest::ertBuyTime;
    req->nGroupID = pstOnlineRole->pRoleData->byGroup;
    req->dwRoleID = pstOnlineRole->pRoleData->dwRoleID;
    strxcpy(req->szUserName, pstOnlineRole->pRoleData->szUserName, USER_NAME_LEN);
    strxcpy(req->szUniqueID, szUniqueID, OA_BILLING_UNIQUE_ID_LEN);
    req->dwReqTime = CManageServer::GetInstance().GetGameTime();

    req->buytime.nGroupID = pstOnlineRole->pRoleData->byGroup;
    req->buytime.nPayAmount = nPayAmount;
    req->buytime.nTimeAmount = nTimeAmount;

    DBG("Buy Time Request Sent: RpcID:%u", dwRpcId);

    nRetCode = CMessageHandler::GetInstance().DoDLBillingRequest(pstOnlineRole->pRoleData->byGroup, req->eType, dwRpcId,
        pstOnlineRole->pRoleData->szUserName, "", szUniqueID, "", pstOnlineRole->pRoleData->dwRoleID, 0, 0, 0, nTimeAmount, bAuto);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;

Exit0:
    if (pMsg)
        pMsg->Release();

    return FALSE;
}

void COABillMgr::OnSubTime(SOARequest* req, SOAResult* res)
{
    MS_USER* user = CUserMgr::GetInstance().FindUserByUserName(req->szUserName);
    if (user)
    {
        if (res->nResult == 0)
        {
            INF("Sub Time Succeeded: RpcID:%u, Balance:%s", res->dwRpcId, res->szBalanceInfo);

            sscanf(res->szBalanceInfo, "%d", &user->nTimeLeft);
            CMessageHandler::GetInstance().DoClientTimeInfo(user->dwCliSessionID, user->nTimeLeft);

            if (user->nTimeLeft <= 0 && user->eState == USER_STATE_INGAME && user->dwCurRoleID == req->dwRoleID)
            {
                MS_ONLINE_ROLE_DATA* role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(req->dwRoleID);
                if (role)
                {
                    BOOL bRet = DoBuyTime(role, 100, 6000, TRUE);
                    LOG_CHECK_ERROR(bRet);
                }
            }
        }
        else
        {
            INF("Sub Time Failed: RpcID:%u, Result:%d, UniqueID:%s, UserName:%s, Amount:%d",
                res->dwRpcId, res->nResult, req->szUniqueID, req->szUserName, req->subtime.nTimeAmount);
        }
    }

    if (req->subtime.nStatus == etbtSupplementary)
    {
        MS_ONLINE_ROLE_DATA* role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(req->dwRoleID);
        if (role)
        {
            if (res->nResult == 0)
            {
                BOOL bRet = CMessageHandler::GetInstance().DoGSAddExerciseTime(role->dwGSSessionID, req->dwRoleID, req->subtime.nTimeAmount / EXERCISE_COST_PER_HOUR, errSuccess);
                LOG_PROCESS_ERROR(bRet);
            }
            else
            {
                BOOL bRet = CMessageHandler::GetInstance().DoGSAddExerciseTime(role->dwGSSessionID, req->dwRoleID, req->subtime.nTimeAmount / EXERCISE_COST_PER_HOUR, errTimeBillingNotEnough);
                LOG_PROCESS_ERROR(bRet);
            }
        }
        else
        {
            if (res->nResult == 0)
            {
                INF("Failed To Add Exercise Time: RoleID:%u, Hour:%n", req->dwRoleID, req->subtime.nTimeAmount / EXERCISE_COST_PER_HOUR);
            }
        }
    }

Exit0:
    return;
}

void COABillMgr::OnBuyTime(SOARequest* req, SOAResult* res, BOOL bAuto)
{
    BOOL bSuccess = (res->nResult == 0);
    MS_USER* user = CUserMgr::GetInstance().FindUserByUserName(req->szUserName);
    if (user)
    {
        INT32 moneytype = 0;
        INT32 moneyamount = 0;
        char* temp_str = res->szBalanceInfo;

        char* tok = strtok(temp_str, "#");
        while (tok)
        {
            sscanf(tok, "%d|%d", &moneytype, &moneyamount);
            if (moneytype == 2)
            {
                user->nTimeLeft = moneyamount;
                CMessageHandler::GetInstance().DoClientTimeInfo(user->dwCliSessionID, user->nTimeLeft);
            }
            if (moneytype == 3)
            {
                user->nDianquan = moneyamount;
                CMessageHandler::GetInstance().DoClientSyncRealMoney(user->dwCliSessionID, user->nDianquan);
            }

            tok = strtok(NULL, "#");
        }

        if (!user->bUserWhiteList)
        {
            if (bAuto && !bSuccess)
            {
                user->SetFreeMode(TRUE);
            }
            if (!bAuto && bSuccess && user->nDianquan > 0 && user->bFreeMode)
            {
                user->SetFreeMode(FALSE);
            }
        }

        CMessageHandler::GetInstance().DoClientBuyTime(user->dwCliSessionID, bAuto, bSuccess);
    }
}

void COABillMgr::OnBuyTime_Test(CHAR* szRoleName, SOAResult res, BOOL bAuto)
{
    BOOL bSuccess = (res.nResult == 0);
    MS_USER* user = CUserMgr::GetInstance().FindUserByUserName(szRoleName);
    if (user)
    {
        INT32 moneytype = 0;
        INT32 moneyamount = 0;
        char* temp_str = res.szBalanceInfo;

        char* tok = strtok(temp_str, "#");
        while (tok)
        {
            // "2|86400"
            sscanf(tok, "%d|%d", &moneytype, &moneyamount);
            if (moneytype == 2)
            {
                user->nTimeLeft = moneyamount;
                CMessageHandler::GetInstance().DoClientTimeInfo(user->dwCliSessionID, user->nTimeLeft);
            }

            tok = strtok(NULL, "#");
        }

        if (!user->bUserWhiteList)
        {
            if (bAuto && !bSuccess)
            {
                user->SetFreeMode(TRUE);
            }
            if (!bAuto && bSuccess && user->nDianquan > 0 && user->bFreeMode)
            {
                user->SetFreeMode(FALSE);
            }
        }

        CMessageHandler::GetInstance().DoClientBuyTime(user->dwCliSessionID, bAuto, bSuccess);
    }
}

void COABillMgr::OnCallBack(SOAResult* res)
{
    const auto nRetCode = m_ResultList.Push(std::unique_ptr<SOAResult>(res));
    LOG_CHECK_ERROR(nRetCode);
}

int SNDACALLBACK BillingCallback(EMsgDirection eMsgType, unsigned int dwRpcId, int nResult, const ISDOCommonMsg* pResponse) try
{
    std::unique_ptr<SOAResult> owned;
    SOAResult* res = nullptr;
    ISDOCommonMsg* pMsg = NULL;
    char* msg_name = NULL;

    LOG_PROCESS_ERROR(pResponse);
    LOG_PROCESS_ERROR(eMsgType == E_RESPONSE);

    pMsg = (ISDOCommonMsg*)pResponse;
    msg_name = pMsg->GetMsgName();
    LOG_PROCESS_ERROR(msg_name);

    DBG("BillingCallback: %s", msg_name);

    if (strncmp(msg_name, "billingservice.accountquery", 100) == 0 ||
        strncmp(msg_name, "billingservice.currencyfreeze", 100) == 0 ||
        strncmp(msg_name, "billingservice.currencythaw", 100) == 0 ||
        strncmp(msg_name, "billingservice.timeauth", 100) == 0 ||
        strncmp(msg_name, "billingservice.timebilling", 100) == 0 ||
        strncmp(msg_name, "billingservice.consigntransfer", 100) == 0)
    {
        owned = std::make_unique<SOAResult>();
        res = owned.get();
        LOG_PROCESS_ERROR(res);


        res->dwRpcId = dwRpcId;
        res->nResult = nResult;
        if (nResult == 0)
        {
            char* szTempString = NULL;
            const char* balance_string = "balanceInfo";
            if (strncmp(msg_name, "billingservice.timebilling", 100) == 0)
                balance_string = "balance";
            if (strncmp(msg_name, "billingservice.consigntransfer", 100) == 0)
                balance_string = "balanceInfoDest";

            if (pMsg->GetValue(balance_string, &szTempString) == 0)
            {
                LOG_PROCESS_ERROR(szTempString);
                strxcpy(res->szBalanceInfo, szTempString, BALANCE_STR_LEN);
            }

            DBG("%s Success, RpcID:%u, BalanceInfo:%s", msg_name, dwRpcId, res->szBalanceInfo);
        }
        else
        {
            if (nResult != -106114 && nResult != -106908) //Do not log insufficient balance errors; otherwise there would be too many
            {
                char* szFailReason = NULL;
                pMsg->GetValue("failReason", &szFailReason);
                LOG_PROCESS_ERROR(szFailReason);
                WRN("%s Failed: RpcID:%u, nResult:%d, failReason:%s", msg_name, dwRpcId, nResult, szFailReason);
            }
        }

        COABillMgr::GetInstance().OnCallBack(owned.release());
    }
    else if (strncmp(msg_name, "billingservice.currencyexchange", 100) == 0)
    {
        owned = std::make_unique<SOAResult>();
        res = owned.get();
        LOG_PROCESS_ERROR(res);


        res->dwRpcId = dwRpcId;
        res->nResult = nResult;
        if (nResult == 0)
        {
            char* szTempString1 = NULL;
            char* szTempString2 = NULL;

            pMsg->GetValue("balanceFromInfo", &szTempString1);
            LOG_PROCESS_ERROR(szTempString1);
            pMsg->GetValue("balanceToInfo", &szTempString2);
            LOG_PROCESS_ERROR(szTempString2);
            snprintf(res->szBalanceInfo, sizeof(res->szBalanceInfo), "%s#%s", szTempString1, szTempString2);
            res->szBalanceInfo[sizeof(res->szBalanceInfo) - 1] = 0;

            DBG("%s Success, RpcID:%u, BalanceInfo:%s", msg_name, dwRpcId, res->szBalanceInfo);
        }
        else
        {
            if (nResult != -106114 && nResult != -106908) //Do not log insufficient balance errors; otherwise there would be too many
            {
                char* szFailReason = NULL;
                pMsg->GetValue("failReason", &szFailReason);
                LOG_PROCESS_ERROR(szFailReason);
                WRN("%s Failed: RpcID:%u, nResult:%d, failReason:%s", msg_name, dwRpcId, nResult, szFailReason);
            }
        }

        COABillMgr::GetInstance().OnCallBack(owned.release());
    }
    else
    {
        WRN("UnProcessed billingservice Msg: %s", msg_name);
    }


    return 0;

Exit0:
    return 0;
}
catch (...)
{
    std::fputs("Billing callback failed\n", stderr);
    return 0;
}

int SNDACALLBACK NotifyCallback(EMsgDirection eMsgType, unsigned int dwRpcId, int nResult, const ISDOCommonMsg* pResponse) try
{
    ISDOCommonMsg* pMsg = NULL;
    const auto releaseMessage = [](ISDOCommonMsg* value) { value->Release(); };
    std::unique_ptr<ISDOCommonMsg, decltype(releaseMessage)> reply;
    ISDOCommonMsg* pResMsg = nullptr;
    char* szUniqueID = NULL;

    LOG_PROCESS_ERROR(pResponse);
    LOG_PROCESS_ERROR(eMsgType == E_REQUEST);

    pMsg = (ISDOCommonMsg*)pResponse;

    if (strncmp(pMsg->GetMsgName(), "bl3-notify.chargenotify", 100) == 0)
    {
        INT32 nResultCode = -1;
        char szUniqueID[256] = "Default UniqueID";
        char szChargeDetail[256] = "Default ChargeDetail";
        char szBalanceInfo[256] = "Default BalanceInfo";
        char* szTemp = NULL;

        pMsg->GetValue("resultCode", &nResultCode);
        if (pMsg->GetValue("uniqueId", &szTemp) == 0)
            strxcpy(szUniqueID, szTemp, 256);
        if (pMsg->GetValue("chargeDetail", &szTemp) == 0)
            strxcpy(szChargeDetail, szTemp, 256);
        if (pMsg->GetValue("balanceInfo", &szTemp) == 0)
            strxcpy(szBalanceInfo, szTemp, 256);

        WRN("bl3-notify.chargenotify: resultCode:%d, chargeDetail:%s, balanceInfo:%s", nResultCode, szChargeDetail, szBalanceInfo);
    }
    else
    {
        WRN("UnProcessed bl3-notify Msg: %s", pMsg->GetMsgName());
    }

    // Acknowledge the notification
    pMsg->GetValue("uniqueId", &szUniqueID);
    reply.reset(sdoCreateMsg(E_RESPONSE, pMsg->GetMsgName()));
    pResMsg = reply.get();
    LOG_PROCESS_ERROR(pResMsg);
    pResMsg->SetValue("resultCode", 0);
    pResMsg->SetValue("uniqueId", szUniqueID);
    if (auto* handler = COABillMgr::GetInstance().GetHandler())
        handler->ASyncSendMsg(pResMsg, dwRpcId);

Exit0:
    return 0;
}
catch (...)
{
    std::fputs("Billing notification callback failed\n", stderr);
    return 0;
}
