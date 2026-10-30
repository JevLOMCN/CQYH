#include "stdafx.h"
#include "exchange_mgr.h"
#include "role_data.h"
#include "manageserver.h"
#include "onlinerole/online_role_data_mgr.h"
#include "role_data_mgr.h"
#include "db/ms_sql_stmt_list.h"
#include "time_ex.h"
#include "mail_template_data.h"
#include "mail/mail_mgr.h"
#include "real_money_shop/oa_bill_mgr.h"
#include "server_config.h"
#include "server_var.h"

CExchangeMgr CExchangeMgr::ms_Instance;

#define ODD_BIT_IS_ONE (0x55555555)
#define EVEN_BIT_IS_ONE (0xAAAAAAAA)
UINT64 CreateOrderGuid()
{
    static UINT32 exchange_random_seed = CManageServer::GetInstance().GetGameTime();

    exchange_random_seed = (UINT32)(exchange_random_seed * 0x08088405) + 1;

    UINT64 random_number = (UINT32)((exchange_random_seed * (UINT64)0xFFFFFFFF) >> 32);

    UINT64 cur_time = CManageServer::GetInstance().GetGameTime();

    return
        ((random_number & ODD_BIT_IS_ONE) << 32) |
        ((cur_time & EVEN_BIT_IS_ONE) << 32) |
        (random_number & ODD_BIT_IS_ONE) |
        (cur_time & EVEN_BIT_IS_ONE);
}

BOOL CExchangeMgr::Init(UINT32 dwOrderPoolSize)
{
    UINT64 qwStartTick = 0;

    BOOL bRet = m_order_pool.Init(dwOrderPoolSize);
    LOG_PROCESS_ERROR(bRet);

    for (UINT8 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        m_orders[i].m_byGroupID = i;
    }

    bRet = m_log_pool.Init(1024);
    LOG_PROCESS_ERROR(bRet);

    m_nLoadedCount = 0;
    
    memset(limit_params, 0, sizeof(limit_params));
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        if (IsValidGroup(i))
        {
            bRet = m_orders[i].Init();
            LOG_PROCESS_ERROR(bRet);

            limit_params[i].nPriceHigh = CServerVar::GetInstance().GetVariable(i, 1);
            limit_params[i].nPriceLow = CServerVar::GetInstance().GetVariable(i, 2);
            limit_params[i].nSingleOrderLimit = CServerVar::GetInstance().GetVariable(i, 3);
            limit_params[i].nDailyOrderLimit = CServerVar::GetInstance().GetVariable(i, 4);
            limit_params[i].nDay = (CManageServer::GetInstance().GetGameTime() + CManageServer::GetInstance().GetTimeZone()) / 86400;
            limit_params[i].nDailyOrderCurCount = 0;
        }
    }

    qwStartTick = GetTickCountEx();

    INF("Loading Exchange Order Data...");

    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(msqlSelectExchangeOrder, _Load, NULL, "uc", eosEffective);
    LOG_PROCESS_ERROR(bRet);

    INF("Loaded %d Exchange Order Data in (%d ms)", m_nLoadedCount, (INT32)(GetTickCountEx() - qwStartTick));

    m_nLoadedCount = 0;

    qwStartTick = GetTickCountEx();

    INF("Loading Exchange Log Data...");

    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(msqlSelectExchangeLog, _LoadLog, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    INF("Loaded %d Exchange Order Log in (%d ms)", m_nLoadedCount, (INT32)(GetTickCountEx() - qwStartTick));

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CExchangeMgr::UnInit()
{
    BOOL bRet = FALSE;
    
    for (ROLE_ORDER_LIST::iterator it = m_role_buying_order_list.begin(); it != m_role_buying_order_list.end(); ++it)
    {
        ORDER_LIST& orderlist = it->second;
        for (ORDER_LIST::iterator it2 = orderlist.begin(); it2 != orderlist.end(); ++it2)
        {
            bRet = m_order_pool.Delete(*it2);
            LOG_CHECK_ERROR(bRet);
        }
    }
    for (ROLE_ORDER_LIST::iterator it = m_role_selling_order_list.begin(); it != m_role_selling_order_list.end(); ++it)
    {
        ORDER_LIST& orderlist = it->second;
        for (ORDER_LIST::iterator it2 = orderlist.begin(); it2 != orderlist.end(); ++it2)
        {
            bRet = m_order_pool.Delete(*it2);
            LOG_CHECK_ERROR(bRet);
        }
    }
    for (ROLE_ORDER_LIST::iterator it = m_role_finished_order_list.begin(); it != m_role_finished_order_list.end(); ++it)
    {
        ORDER_LIST& orderlist = it->second;
        for (ORDER_LIST::iterator it2 = orderlist.begin(); it2 != orderlist.end(); ++it2)
        {
            bRet = m_order_pool.Delete(*it2);
            LOG_CHECK_ERROR(bRet);
        }
    }
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        if (IsValidGroup(i))
        {
            bRet = m_orders[i].UnInit();
            LOG_CHECK_ERROR(bRet);
        }
    }

    bRet = m_order_pool.UnInit();
    LOG_CHECK_ERROR(bRet);

    for (ROLE_LOG_LIST::iterator it = m_role_log_list.begin(); it != m_role_log_list.end(); ++it)
    {
        LOG_LIST& loglist = it->second;
        for (LOG_LIST::iterator it2 = loglist.begin(); it2 != loglist.end(); ++it2)
        {
            bRet = m_log_pool.Delete(*it2);
            LOG_CHECK_ERROR(bRet);
        }
    }
    m_role_log_list.clear();

    bRet = m_log_pool.UnInit();
    LOG_CHECK_ERROR(bRet);

    return TRUE;
}

BOOL CExchangeMgr::MainLoop()
{
    for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        if (IsValidGroup(i))
        {
            BOOL bRet = m_orders[i].MainLoop();
            LOG_CHECK_ERROR(bRet);

            if (limit_params[i].nDay != (CManageServer::GetInstance().GetGameTime() + CManageServer::GetInstance().GetTimeZone()) / 86400)
            {
                limit_params[i].nDay = (CManageServer::GetInstance().GetGameTime() + CManageServer::GetInstance().GetTimeZone()) / 86400;
                limit_params[i].nDailyOrderCurCount = 0;
            }
        }
    }

    return TRUE;
}

INT32 CExchangeMgr::AddOrder(MS_ROLE_DATA* role, UINT8 ordertype, INT32 count, INT32 price, const char* billing_order_id)
{
    UINT64 orderid{};
    Order* order{};
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(IsValidGroup(role->byGroup));

    orderid = CreateOrderGuid();
    order = m_order_pool.New(orderid);
    LOG_PROCESS_ERROR(order);

    memset(order, 0, sizeof(Order));

    order->m_qwOrderID = orderid;
    order->m_byOrderType = ordertype;
    order->m_byOrderStatus = eosEffective;
    order->m_dwRoleID = role->dwRoleID;
    order->m_nPrice = price;
    order->m_nCount = count;
    order->m_nCurCount = count;
    order->m_dwOrderTime = CManageServer::GetInstance().GetGameTime();
    if (ordertype == eotBuy)
    {
        order->m_nCurMoney = price * count;
        order->m_nBillingCount = count;

        LOG_PROCESS_ERROR(billing_order_id);
        LOG_PROCESS_ERROR(strlen(billing_order_id) == OA_BILLING_ORDER_ID_LEN - 1);
        strxcpy(order->m_szBillingOrderID, billing_order_id, OA_BILLING_ORDER_ID_LEN);
    }
    else
    {
        order->m_nCurMoney = count * 10000;
    }

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlInsertExchangeOrder, NULL, NULL, "uculuducucuddddds",
        role->byGroup, order->m_qwOrderID, order->m_dwOrderTime, order->m_byOrderType, eosEffective, order->m_dwRoleID, order->m_nPrice, order->m_nCurCount, order->m_nCurCount, order->m_nCurMoney, order->m_szBillingOrderID);
    LOG_CHECK_ERROR(bRet);

    m_orders[role->byGroup].AddOrder(order);

    if (ordertype == eotBuy)
        m_role_buying_order_list[role->dwRoleID].push_back(order);
    else
        m_role_selling_order_list[role->dwRoleID].push_back(order);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(role->dwRoleID);
    if(pOnlineRole)
    {
        CMessageHandler::GetInstance().DoClientExchangeAddOrder(pOnlineRole->dwCliSessionID, order);
    }

    return errSuccess;

Exit0:
    if (order)
        m_order_pool.Delete(order);

    return errItemUnknown;
}

INT32 CExchangeMgr::CancelOrder(MS_ONLINE_ROLE_DATA* online_role, UINT64 orderid)
{
    BOOL bRet = FALSE;
    Order* order = m_order_pool.Find(orderid);
    if (order == NULL)
        return errExchangeOrderNotFound;

    LOG_PROCESS_ERROR(order->m_dwRoleID == online_role->pRoleData->dwRoleID);
    LOG_PROCESS_ERROR(order->m_nCurCount > 0);

    bRet = m_orders[online_role->pRoleData->byGroup].CancelOrder(order);
    LOG_PROCESS_ERROR(bRet);

    if (order->m_byOrderType == eotBuy)
        m_role_buying_order_list[online_role->pRoleData->dwRoleID].remove(order);
    else
        m_role_selling_order_list[online_role->pRoleData->dwRoleID].remove(order);

    CMessageHandler::GetInstance().DoClientExchangeCancelOrder(online_role->dwCliSessionID, orderid);

    _DeleteOrder(order, eosCanceled);

    return errSuccess;

Exit0:
    return errItemUnknown;
}

void CExchangeMgr::AddSystemOrder(UINT32 dwTLUserID, UINT8 groupid, INT32 price, INT32 count)
{
    UINT64 orderid{};
    INT32 nResult = errUnknownError;
    BOOL bRet = FALSE;
    Order* order = NULL;

    LOG_PROCESS_ERROR(IsValidGroup(groupid));
    LOG_PROCESS_ERROR(price <= limit_params[groupid].nPriceHigh);
    LOG_PROCESS_ERROR(price >= limit_params[groupid].nPriceLow);
    LOG_PROCESS_ERROR(count <= limit_params[groupid].nSingleOrderLimit);
    LOG_PROCESS_ERROR(count + limit_params[groupid].nDailyOrderCurCount <= limit_params[groupid].nDailyOrderLimit);

    orderid = CreateOrderGuid();
    order = m_order_pool.New(orderid);
    LOG_PROCESS_ERROR(order);

    memset(order, 0, sizeof(Order));

    order->m_qwOrderID = orderid;
    order->m_byOrderType = eotSell;
    order->m_byOrderStatus = eosEffective;
    order->m_dwRoleID = 0;
    order->m_nPrice = price;
    order->m_nCount = count;
    order->m_nCurCount = count;
    order->m_nCurMoney = count * 10000;
    order->m_dwOrderTime = CManageServer::GetInstance().GetGameTime();

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlInsertExchangeOrder, NULL, NULL, "uculuducucuddddds",
        groupid, order->m_qwOrderID, order->m_dwOrderTime, order->m_byOrderType, eosEffective, order->m_dwRoleID, order->m_nPrice, order->m_nCurCount, order->m_nCurCount, order->m_nCurMoney, order->m_szBillingOrderID);
    LOG_CHECK_ERROR(bRet);

    m_orders[groupid].AddOrder(order);
    m_role_selling_order_list[0].push_back(order);

    limit_params[groupid].nDailyOrderCurCount += count;

    bRet = CMessageHandler::GetInstance().DoGMAddSystemOrderAck(dwTLUserID, groupid, order->m_qwOrderID, errSuccess, price, count);
    LOG_CHECK_ERROR(bRet);

    return;

Exit0:
    bRet = CMessageHandler::GetInstance().DoGMAddSystemOrderAck(dwTLUserID, groupid, 0, nResult, price, count);
    LOG_CHECK_ERROR(bRet);
}

void CExchangeMgr::CancelSystemOrder(UINT32 dwTLUserID, UINT8 groupid, UINT64 orderid)
{
    Order* order{};
    BOOL bRet = FALSE;
    INT32 nResult = errUnknownError;

    INT32   nPrice = 0;
    INT32   nCount = 0;

    LOG_PROCESS_ERROR(IsValidGroup(groupid));

    order = m_order_pool.Find(orderid);
    LOG_PROCESS_ERROR_RET_CODE(order, errExchangeOrderNotFound);
    LOG_PROCESS_ERROR(order->m_byOrderType == eotSell);
    LOG_PROCESS_ERROR(order->m_dwRoleID == 0);
    LOG_PROCESS_ERROR(order->m_nCurCount > 0);

    nPrice = order->m_nPrice;
    nCount = order->m_nCurCount;

    bRet = m_orders[groupid].CancelOrder(order);
    LOG_PROCESS_ERROR(bRet);

    m_role_selling_order_list[0].remove(order);

    _DeleteOrder(order, eosCanceled);

    bRet = CMessageHandler::GetInstance().DoGMCancelSystemOrderAck(dwTLUserID, groupid, order->m_qwOrderID, errSuccess, nPrice, nCount);
    LOG_CHECK_ERROR(bRet);

    return;

Exit0:
    bRet = CMessageHandler::GetInstance().DoGMCancelSystemOrderAck(dwTLUserID, groupid, 0, nResult, nPrice, nCount);
    LOG_CHECK_ERROR(bRet);
}

void CExchangeMgr::GetPrices(UINT8 groupid, UINT8 ordertype, UINT8 requirecount, UINT8& count, INT32 price[], INT32 handcount[])
{
    LOG_PROCESS_ERROR(IsValidGroup(groupid));

    if (ordertype == eotBuy)
    {
        m_orders[groupid].GetBuyingPrices(requirecount, count, price, handcount);
    }

    if (ordertype == eotSell)
    {
        m_orders[groupid].GetSellingPrices(requirecount, count, price, handcount);
    }

Exit0:
    return;
}

void CExchangeMgr::GetCounts(UINT8 groupid, UINT8 ordertype, INT32 price, UINT8 requirecount, UINT8& count, UINT32 roleid[], INT32 handcount[], UINT64 orderid[])
{
    LOG_PROCESS_ERROR(IsValidGroup(groupid));
    LOG_PROCESS_ERROR(ordertype == eotSell || ordertype == eotBuy);

    m_orders[groupid].GetCounts(ordertype, price, requirecount, count, roleid, handcount, orderid);

Exit0:
    return;
}

const CExchangeMgr::ORDER_LIST& CExchangeMgr::GetOrders(UINT8 ordertype, UINT32 roleid) const
{
    static ORDER_LIST empty_order_list;

    if (ordertype == eotBuy)
    {
        ROLE_ORDER_LIST::const_iterator it = m_role_buying_order_list.find(roleid);
        if (it == m_role_buying_order_list.end())
            return empty_order_list;
        else
            return it->second;
    }
    else if (ordertype == eotSell)
    {
        ROLE_ORDER_LIST::const_iterator it = m_role_selling_order_list.find(roleid);
        if (it == m_role_selling_order_list.end())
            return empty_order_list;
        else
            return it->second;
    }
    else
    {
        LOG_CHECK_ERROR(FALSE);
        return empty_order_list;
    }
}

const CExchangeMgr::ORDER_LIST& CExchangeMgr::GetFinishedOrders(UINT32 roleid) const
{
    static ORDER_LIST empty_order_list;

    ROLE_ORDER_LIST::const_iterator it = m_role_finished_order_list.find(roleid);
    if (it == m_role_finished_order_list.end())
        return empty_order_list;
    else
        return it->second;
}

const CExchangeMgr::LOG_LIST& CExchangeMgr::GetLogs(UINT32 roleid) const
{
    static LOG_LIST empty_log_list;

    ROLE_LOG_LIST::const_iterator it = m_role_log_list.find(roleid);
    if (it == m_role_log_list.end())
        return empty_log_list;
    else
        return it->second;
}

void CExchangeMgr::AddLog(UINT32 roleid, UINT64 orderid, UINT32 curtime, UINT8 ordertype, INT32 price, INT32 count)
{
    LOG_LIST& loglist = m_role_log_list[roleid];

    ExchangeLog* log = m_log_pool.New();
    LOG_PROCESS_ERROR(log);

    log->qwOrderID = orderid;
    log->dwTime = curtime;
    log->byType = ordertype;
    log->nPrice = price;
    log->nCount = count;

    loglist.push_back(log);

    if (loglist.size() >= MAX_EXCHANGE_LOGS)
    {
        BOOL bRet = m_log_pool.Delete(loglist.front());
        LOG_PROCESS_ERROR(bRet);

        loglist.pop_front();
    }

Exit0:
    return;
}

void CExchangeMgr::GetSystemLimit(UINT32 dwTLUserID, UINT8 groupid)
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(IsValidGroup(groupid));

    bRet = CMessageHandler::GetInstance().DoGMGetExchangeLimitAck(dwTLUserID, groupid,
        limit_params[groupid].nPriceHigh, limit_params[groupid].nPriceLow, limit_params[groupid].nSingleOrderLimit, limit_params[groupid].nDailyOrderLimit);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return;
}

void CExchangeMgr::SetSystemLimit(UINT32 dwTLUserID, UINT8 groupid, INT32 high, INT32 low, INT32 single, INT32 daily)
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(IsValidGroup(groupid));

    limit_params[groupid].nPriceHigh = high;
    limit_params[groupid].nPriceLow = low;
    limit_params[groupid].nSingleOrderLimit = single;
    limit_params[groupid].nDailyOrderLimit = daily;

    CServerVar::GetInstance().SetVariable(groupid, 1, high);
    CServerVar::GetInstance().SetVariable(groupid, 2, low);
    CServerVar::GetInstance().SetVariable(groupid, 3, single);
    CServerVar::GetInstance().SetVariable(groupid, 4, daily);

    bRet = CMessageHandler::GetInstance().DoGMSetExchangeLimitAck(dwTLUserID, groupid,
        limit_params[groupid].nPriceHigh, limit_params[groupid].nPriceLow, limit_params[groupid].nSingleOrderLimit, limit_params[groupid].nDailyOrderLimit);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return;
}

void CExchangeMgr::OnBillingFreeze(INT32 result, UINT32 roleid, INT32 price, INT32 count, const char* billing_order_id)
{
    INT32 nRetCode = errItemUnknown;
    MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(roleid);
    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(role->eAuctionQueryStatus == MS_ROLE_DATA::eqsBillFreeze);
    role->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;

    if (result == 0)
    {
        nRetCode = AddOrder(role, eotBuy, count, price, billing_order_id);
        LOG_PROCESS_ERROR(nRetCode == errSuccess);
    }
    else
    {
        MS_ONLINE_ROLE_DATA* pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(role->dwRoleID);
        if(pOnlineRole)
        {
            if (result == -106114 || result == -106908)
            {
                CMessageHandler::GetInstance().DoErrorNotify(pOnlineRole->dwCliSessionID, errRealMoneyBuyNotEnoughMoney, 0, 0);
            }
            else
            {
                CMessageHandler::GetInstance().DoErrorNotify(pOnlineRole->dwCliSessionID, errBillingSystemError, result, 0);
            }
        }
    }

Exit0:
    return;
}

void CExchangeMgr::OnBillingUnfreeze(INT32 result)
{
    LOG_CHECK_ERROR(result == 0);
}

void CExchangeMgr::OnBillingConfirm(INT32 result, UINT64 buying_order_id, UINT32 buyer_roleid, INT32 count, INT32 money, const char* billing_order_id)
{
    BOOL bRet = FALSE;
    Order* order = m_order_pool.Find(buying_order_id);
    MS_ROLE_DATA* buyer = CRoleDataMgr::GetInstance().FindRole(buyer_roleid);

    LOG_PROCESS_ERROR(order);
    LOG_PROCESS_ERROR(buyer);

    if (result == 0)
    {
        const MAIL_TEMPLATE_DATA* mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiExchangeBuySuccess);
        char mail_subject[MAIL_SUBJECT_LEN] = { 0 };
        char mail_content[MAIL_CONTENT_LEN] = { 0 };

        LOG_CHECK_ERROR(mail);
        if (mail)
        {
            strxcpy(mail_subject, mail->m_szMailSubject, MAIL_SUBJECT_LEN);
            snprintf(mail_content, MAIL_CONTENT_LEN, mail->m_szMailContent, (float)money / count / 100, count, (float)money / 100);
        }
        else
        {
            strxcpy(mail_subject, "exchange success", MAIL_SUBJECT_LEN);
            strxcpy(mail_content, "exchange success", MAIL_CONTENT_LEN);
        }

        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(buyer, mail_subject, mail_content, MONEYTYPE_02, count * 10000);
        LOG_CHECK_ERROR(bRet);

    }
    else
    {
        char mail_content[MAIL_CONTENT_LEN] = { 0 };
        snprintf(mail_content, MAIL_CONTENT_LEN, "please report to gm with following information: %d, %s, %d, %d",
            result, billing_order_id, money, count);

        bRet = CMailMgr::GetInstance().SendSystemMail(buyer, "exchange buy failed", mail_content);
        LOG_CHECK_ERROR(bRet);
    }

    order->m_nBillingCount -= count;
    LOG_CHECK_ERROR(order->m_nBillingCount >= 0);
    if (order->m_nBillingCount <= 0)
    {
        _DeleteOrder(order, eosFinished);
    }

Exit0:
    return;
}

void CExchangeMgr::OnBillingTransfer(INT32 result, UINT64 buying_order_id, UINT32 buyer_roleid, UINT32 seller_roleid, INT32 count, INT32 money, const char* billing_order_id)
{
    BOOL bRet = FALSE;
    Order* order = m_order_pool.Find(buying_order_id);
    MS_ROLE_DATA* buyer = CRoleDataMgr::GetInstance().FindRole(buyer_roleid);
    MS_ROLE_DATA* seller = CRoleDataMgr::GetInstance().FindRole(seller_roleid);

    LOG_PROCESS_ERROR(order);
    LOG_PROCESS_ERROR(buyer);
    LOG_PROCESS_ERROR(seller);

    if (result == 0)
    {
        const MAIL_TEMPLATE_DATA* mail = NULL;
        char mail_subject[MAIL_SUBJECT_LEN] = {0};
        char mail_content[MAIL_CONTENT_LEN] = {0};

        mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiExchangeBuySuccess);
        LOG_CHECK_ERROR(mail);
        if (mail)
        {
            strxcpy(mail_subject, mail->m_szMailSubject, MAIL_SUBJECT_LEN);
            snprintf(mail_content, MAIL_CONTENT_LEN, mail->m_szMailContent, (float)money / count / 100, count, (float)money / 100);
        }
        else
        {
            strxcpy(mail_subject, "exchange buy success", MAIL_SUBJECT_LEN);
            strxcpy(mail_content, "exchange buy success", MAIL_CONTENT_LEN);
        }

        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(buyer, mail_subject, mail_content, MONEYTYPE_02, count * 10000);
        LOG_CHECK_ERROR(bRet);


        mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiExchangeSellSuccess);
        LOG_CHECK_ERROR(mail);
        if (mail)
        {
            INT32 price = money / count;
            INT32 tax = (price * EXCHANGE_TAX + 99) / 100;
            if (tax < 1)
                tax = 1;
            strxcpy(mail_subject, mail->m_szMailSubject, MAIL_SUBJECT_LEN);
            snprintf(mail_content, MAIL_CONTENT_LEN, mail->m_szMailContent, (float)money / count / 100, count, (float)money / 100,
                float(tax) * count / 100, float(money - tax*count) / 100);
        }
        else
        {
            strxcpy(mail_subject, "exchange sell success", MAIL_SUBJECT_LEN);
            strxcpy(mail_content, "exchange sell success", MAIL_CONTENT_LEN);
        }

        bRet = CMailMgr::GetInstance().SendSystemMail(seller, mail_subject, mail_content);
        LOG_CHECK_ERROR(bRet);
    }
    else
    {
        char mail_content[MAIL_CONTENT_LEN] = { 0 };
        snprintf(mail_content, MAIL_CONTENT_LEN, "please report to gm with following information: %d, %s, %d, %d",
            result, billing_order_id, money, count);

        bRet = CMailMgr::GetInstance().SendSystemMail(buyer, "exchange buy failed", mail_content);
        LOG_CHECK_ERROR(bRet);

        bRet = CMailMgr::GetInstance().SendSystemMail(seller, "exchange sell failed", mail_content);
        LOG_CHECK_ERROR(bRet);
    }

    order->m_nBillingCount -= count;
    LOG_CHECK_ERROR(order->m_nBillingCount >= 0);
    if (order->m_nBillingCount <= 0)
    {
        _DeleteOrder(order, eosFinished);
    }

Exit0:
    return;
}

BOOL CExchangeMgr::_Load(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet = FALSE;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8 byGroupID;
        UINT64 qwOrderID;
        UINT32 dwOrderTime;
        UINT8 byOrderType;
        UINT8 byOrderStatus;
        UINT32 dwRoleID;
        INT32 nPrice;
        INT32 nCount;
        INT32 nCurCount;
        INT32 nCurMoney;
        char szBillingOrderID[OA_BILLING_ORDER_ID_LEN];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

		LOG_PROCESS_ERROR(pLoadResult->byGroupID >= 0 && pLoadResult->byGroupID < MAX_SERVER_GROUP_COUNT);
		if (g_ServerConfig.GroupConfig[pLoadResult->byGroupID].nMappedGroupID > 0 &&
			g_ServerConfig.GroupConfig[pLoadResult->byGroupID].nMappedGroupID < MAX_SERVER_GROUP_COUNT)
		{
			pLoadResult->byGroupID = g_ServerConfig.GroupConfig[pLoadResult->byGroupID].nMappedGroupID;
		}

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->byGroupID));

        bRet = CExchangeMgr::ms_Instance._LoadOrder(pLoadResult->byGroupID, pLoadResult->qwOrderID, pLoadResult->dwOrderTime, pLoadResult->byOrderType,
            pLoadResult->byOrderStatus, pLoadResult->dwRoleID, pLoadResult->nPrice, pLoadResult->nCount, pLoadResult->nCurCount, pLoadResult->nCurMoney, pLoadResult->szBillingOrderID);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CExchangeMgr::_LoadOrder(UINT8 groupid, UINT64 orderid, UINT32 ordertime, UINT8 ordertype, UINT8 orderstatus, UINT32 roleid, INT32 price, INT32 count, INT32 curcount, INT32 curmoney, const char* billing_order_id)
{
    Order* order = m_order_pool.New(orderid);
    LOG_PROCESS_ERROR(order);

    memset(order, 0, sizeof(Order));

    LOG_PROCESS_ERROR(IsValidGroup(groupid));
    LOG_PROCESS_ERROR(ordertype == eotBuy || ordertype == eotSell);
    LOG_PROCESS_ERROR(orderstatus > eosInvalid && orderstatus < eosTotal);

    order->m_qwOrderID = orderid;
    order->m_dwOrderTime = ordertime;
    order->m_byOrderType = ordertype;
    order->m_byOrderStatus = orderstatus;
    order->m_dwRoleID = roleid;
    order->m_nPrice = price;
    order->m_nCount = count;
    order->m_nCurCount = curcount;
    order->m_nCurMoney = curmoney;

    if (ordertype == eotBuy)
    {
        order->m_nBillingCount = count;

        LOG_PROCESS_ERROR(billing_order_id);
        LOG_PROCESS_ERROR(strlen(billing_order_id) == OA_BILLING_ORDER_ID_LEN - 1);
        strxcpy(order->m_szBillingOrderID, billing_order_id, OA_BILLING_ORDER_ID_LEN);
    }

    if (ordertype == eotSell && roleid == 0 &&
        (CManageServer::GetInstance().GetGameTime() + CManageServer::GetInstance().GetTimeZone()) / 86400 == limit_params[groupid].nDay)
    {
        limit_params[groupid].nDailyOrderCurCount += count;
    }

    if (orderstatus == eosEffective)
    {
        if (order->m_nCurCount == 0)
        {
            INF("Count 0 Order Found, Deleting Now: OrderID: %u, ", order->m_qwOrderID);
            _DeleteOrder(order, eosFinished);
        }
        else
        {
            m_orders[groupid].AddOrder(order);
        }
    }
    else
    {
        ORDER_LIST& order_list = m_role_finished_order_list[roleid];
        order_list.push_back(order);
        if (order_list.size() >= MAX_EXCHANGE_FINISHED_ORDERS)
        {
            BOOL bRet = m_order_pool.Delete(order_list.front());
            LOG_CHECK_ERROR(bRet);

            order_list.pop_front();
        }
    }

    m_nLoadedCount++;

    return TRUE;

Exit0:
    return FALSE;
}

void CExchangeMgr::_DeleteOrder(Order* order, UINT8 status)
{
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(order);
    LOG_PROCESS_ERROR(order->m_byOrderType == eotBuy || order->m_byOrderType == eotSell);
    LOG_PROCESS_ERROR(order->m_byOrderStatus == eosEffective);
    LOG_PROCESS_ERROR(status > eosInvalid && status < eosTotal && status != eosEffective);

    order->m_byOrderStatus = status;

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlUpdateExchangeOrderStatus, NULL, NULL, "ucul", status, order->m_qwOrderID);
    LOG_CHECK_ERROR(bRet);

    if (order->m_byOrderType == eotBuy)
    {
        if (order->m_dwRoleID && order->m_nCurCount > 0 && order->m_nCurMoney > 0)
        {
            MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(order->m_dwRoleID);
            LOG_PROCESS_ERROR(role);

            const MAIL_TEMPLATE_DATA* mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiExchangeBuyFailed);
            char mail_subject[MAIL_SUBJECT_LEN] = { 0 };
            char mail_content[MAIL_CONTENT_LEN] = { 0 };
            BOOL bRet = FALSE;

            LOG_CHECK_ERROR(mail);
            if (mail)
            {
                strxcpy(mail_subject, mail->m_szMailSubject, MAIL_SUBJECT_LEN);
                snprintf(mail_content, MAIL_CONTENT_LEN, mail->m_szMailContent, (float)order->m_nCurMoney / 100);
            }
            else
            {
                strxcpy(mail_subject, "exchange failed", MAIL_SUBJECT_LEN);
                strxcpy(mail_content, "exchange failed", MAIL_CONTENT_LEN);
            }

            bRet = CMailMgr::GetInstance().SendSystemMail(role, mail_subject, mail_content);
            LOG_CHECK_ERROR(bRet);
        }

        if (order->m_nCurMoney > 0)
        {
            bRet = COABillMgr::GetInstance().DoUnfreezeCurrency(order->m_dwRoleID, order->m_szBillingOrderID, order->m_nCurMoney, order->m_qwOrderID);
            LOG_CHECK_ERROR(bRet);
        }
    }
    else
    {
        if (order->m_dwRoleID && order->m_nCurCount > 0)
        {
            MS_ROLE_DATA* role = CRoleDataMgr::GetInstance().FindRole(order->m_dwRoleID);
            LOG_PROCESS_ERROR(role);

            const MAIL_TEMPLATE_DATA* mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiExchangeSellFailed);
            char mail_subject[MAIL_SUBJECT_LEN] = {0};
            char mail_content[MAIL_CONTENT_LEN] = {0};
            BOOL bRet = FALSE;

            LOG_CHECK_ERROR(mail);
            if (mail)
            {
                strxcpy(mail_subject, mail->m_szMailSubject, MAIL_SUBJECT_LEN);
                snprintf(mail_content, MAIL_CONTENT_LEN, mail->m_szMailContent, order->m_nCurCount);
            }
            else
            {
                strxcpy(mail_subject, "exchange failed", MAIL_SUBJECT_LEN);
                strxcpy(mail_content, "exchange failed", MAIL_CONTENT_LEN);
            }

            bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(role, mail_subject, mail_content, MONEYTYPE_02, order->m_nCurCount * 10000);
            LOG_CHECK_ERROR(bRet);
        }
    }

    {
        ORDER_LIST& order_list = m_role_finished_order_list[order->m_dwRoleID];
        order_list.push_back(order);
        if (order_list.size() >= MAX_EXCHANGE_FINISHED_ORDERS)
        {
            bRet = m_order_pool.Delete(order_list.front());
            LOG_CHECK_ERROR(bRet);

            order_list.pop_front();
        }
    }

Exit0:
    return;
}

BOOL CExchangeMgr::_LoadLog(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet = FALSE;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8 byGroupID;
        UINT32 dwTime;
        UINT32 dwSellerRoleID;
        UINT64 qwSellerOrderID;
        UINT32 dwBuyerRoleID;
        UINT64 qwBuyerOrderID;
        INT32 nPrice;
        INT32 nCount;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->byGroupID));

        CExchangeMgr::ms_Instance.AddLog(pLoadResult->dwSellerRoleID, pLoadResult->qwSellerOrderID, pLoadResult->dwTime, eotSell, pLoadResult->nPrice, pLoadResult->nCount);
        CExchangeMgr::ms_Instance.AddLog(pLoadResult->dwBuyerRoleID, pLoadResult->qwBuyerOrderID, pLoadResult->dwTime, eotBuy, pLoadResult->nPrice, pLoadResult->nCount);
        CExchangeMgr::ms_Instance.m_nLoadedCount++;
    }

    return TRUE;

Exit0:
    return FALSE;
}
