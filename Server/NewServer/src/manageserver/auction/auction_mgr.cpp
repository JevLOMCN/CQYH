#include "stdafx.h"
#include "auction_mgr.h"
#include "auction_thread.h"
#include "role_data_mgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "mail/mail_mgr.h"
#include "mail_template_data.h"
#include "manageserver.h"
#include "db/ms_sql_stmt_list.h"
#include "server_config.h"
#include "game_world_config.h"
#include "auction_def.h"

INT64 GetAuctionTax(INT64 price)
{
    INT64 tax = price * g_GWConfig.nAuctionTax / RATE_MAX;
    if (tax <= 0)
        return 1;
    else
        return tax;
}

INT64 Inc5Percent(INT64 price)
{
    LOG_PROCESS_ERROR(price > 0);

    if (price < 20)
        return price + 1;
    else
        return price + price / 20;

Exit0:
    return 1;
}

INT64 Auction::GetNextPrice() const
{
    if (m_dwBuyerRoleID == 0)
        return m_llPrice;
    else
    {
        INT64 nextprice = Inc5Percent(m_llPrice);
        if (m_llBuyoutPrice && nextprice > m_llBuyoutPrice)
            return m_llBuyoutPrice;
        else
            return nextprice;
    }
}

void Auction::Finish()
{
    BOOL is_expire{};
    // This function handles three cases: timeout, voluntary cancellation, and normal auction completion
    BOOL bRet = FALSE;

    // Seller
    MS_ROLE_DATA* seller_role = CRoleDataMgr::GetInstance().FindRole(m_dwSellerRoleID);
    LOG_PROCESS_ERROR(seller_role);

    // Do not resolve the buyer here: voluntary cancellation also uses this function and has no buyer
    //MS_ROLE_DATA* buyer_role = CRoleDataMgr::GetInstance().FindRole(m_dwBuyerRoleID);
    //LOG_PROCESS_ERROR(buyer_role);

    // Check order status
    LOG_PROCESS_ERROR(m_byAuctionStatus == easEffective);

    is_expire = (m_dwFinishTime <= CManageServer::GetInstance().GetGameTime());
    m_dwFinishTime = CManageServer::GetInstance().GetGameTime();

    // For a normal auction, BidItem adds an entry to m_deposit_map, so it must be nonempty here
    // Empty: no bids, due to timeout or voluntary cancellation
    if (auto& dmap = CAuctionMgr::GetInstance().m_deposit_map[m_dwAuctionID]; dmap.empty())
    {
        LOG_PROCESS_ERROR(m_dwBuyerRoleID == 0);

        const MAIL_TEMPLATE_DATA* seller_mail = NULL;
        char mail_subject[MAIL_SUBJECT_LEN];
        char attachbuffer[1024];

        UINT32 attachlen = m_iteminfo.Save(attachbuffer);
        LOG_PROCESS_ERROR(attachlen <= BUFFER_ATTACH_LEN);

        if (is_expire)
        {
            seller_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionFailSellerExpire);
            LOG_PROCESS_ERROR(seller_mail);
        }
        else
        {
            seller_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionFailSellerCancel);
            LOG_PROCESS_ERROR(seller_mail);
        }

        snprintf(mail_subject, MAIL_SUBJECT_LEN, seller_mail->m_szMailSubject, m_iteminfo.m_nTemplateID);
        bRet = CMailMgr::GetInstance().SendSystemMailWithBufferAttach(seller_role, mail_subject, seller_mail->m_szMailContent, attachbuffer, attachlen);
        LOG_PROCESS_ERROR(bRet);

        if (is_expire)
        {
            CAuctionMgr::GetInstance()._UpDateAuction(this, easExpired);

            // Copied code; logging is temporarily disabled
            //bRet = CMessageHandler::GetInstance().DoDLAuction(m_byGroupID, m_dwSellerRoleID, 0, m_dwAuctionID, "Expire",
            //    0, 0, 0, 0, m_iteminfo.m_nTemplateID, m_iteminfo.m_llGuid);
            //LOG_PROCESS_ERROR(bRet);
        }
        else
        {
            CAuctionMgr::GetInstance()._UpDateAuction(this, easCanceled);

            // Copied code; logging is temporarily disabled
            //bRet = CMessageHandler::GetInstance().DoDLAuction(m_byGroupID, m_dwSellerRoleID, 0, m_dwAuctionID, "Cancel",
            //    0, 0, 0, 0, m_iteminfo.m_nTemplateID, m_iteminfo.m_llGuid);
            //LOG_PROCESS_ERROR(bRet);
        }

        CAuctionMgr::GetInstance().m_role_selling_auction_list[m_dwSellerRoleID].remove(this);
    }
    // Nonempty: someone bid, so process a normal auction
    else
    {
        LOG_CHECK_ERROR(m_dwBuyerRoleID != 0);

        std::list<UINT32> allbidder;

        // Collect the RoleIDs of everyone who bid
        for (CAuctionMgr::AUCTION_DEPOSIT_MAP::const_iterator it = dmap.begin(); it != dmap.end(); ++it)
        {
            allbidder.push_back(it->first);
        }

        // Remove and refund deposits for all bidders except the buyer
        for (std::list<UINT32>::const_iterator it = allbidder.begin(); it != allbidder.end(); ++it)
        {
            CAuctionMgr::GetInstance().DelDeposit(this, *it);
            if (*it != m_dwBuyerRoleID)
            {
                CAuctionMgr::GetInstance().m_role_buying_auction_list[*it].remove(this);
                CAuctionMgr::GetInstance().ReturnDeposit(m_iteminfo.m_nTemplateID, *it, m_llDepositPrice);
            }
        }

        // Custom code
        //for (const auto it : allbidder)
        //{
        //    CAuctionMgr::GetInstance().DelDeposit(this, it);
        //
        //    if (it != m_dwBuyerRoleID)
        //    {
        //        CAuctionMgr::GetInstance().m_role_buying_auction_list[it].remove(this);
        //        CAuctionMgr::GetInstance().ReturnDeposit(m_iteminfo.m_nTemplateID, it, m_llDepositPrice);
        //    }
        //}

        // Send mail
        if (seller_role)
        {
            const MAIL_TEMPLATE_DATA* seller_mail = NULL;
            char mail_subject[MAIL_SUBJECT_LEN];

            seller_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionSellSuccessTemp);
            LOG_PROCESS_ERROR(seller_mail);

            snprintf(mail_subject, MAIL_SUBJECT_LEN, seller_mail->m_szMailSubject, m_iteminfo.m_nTemplateID);
            bRet = CMailMgr::GetInstance().SendSystemMail(seller_role, mail_subject, seller_mail->m_szMailContent);
            LOG_PROCESS_ERROR(bRet);
        }

        MS_ROLE_DATA* buyer_role = CRoleDataMgr::GetInstance().FindRole(m_dwBuyerRoleID);
        if (buyer_role)
        {
            const MAIL_TEMPLATE_DATA* buyer_mail = NULL;
            char mail_subject[MAIL_SUBJECT_LEN];

            buyer_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionBuySuccessTemp);
            LOG_PROCESS_ERROR(buyer_mail);

            snprintf(mail_subject, MAIL_SUBJECT_LEN, buyer_mail->m_szMailSubject, m_iteminfo.m_nTemplateID);
            bRet = CMailMgr::GetInstance().SendSystemMail(buyer_role, mail_subject, buyer_mail->m_szMailContent);
            LOG_PROCESS_ERROR(bRet);
        }

        LOG_PROCESS_ERROR(dmap.empty());    // This should now be empty

        // Delete order deposit data
        CAuctionMgr::GetInstance().m_deposit_map.erase(m_dwAuctionID);

        // Update order state to await balance payment
        CAuctionMgr::GetInstance()._UpDateAuction(this, easWaitForPayment);

        // Copied code; logging is temporarily disabled
        //bRet = CMessageHandler::GetInstance().DoDLAuction(m_byGroupID, m_dwSellerRoleID, m_dwBuyerRoleID, m_dwAuctionID, "Win",
        //    m_llPrice, 0, 0, 0, m_iteminfo.m_nTemplateID, m_iteminfo.m_llGuid);
        //LOG_PROCESS_ERROR(bRet);
    }

    return;

Exit0:
    CRI("Auction::Finish() Failed, AuctionID:%u", m_dwAuctionID);
    CAuctionMgr::GetInstance()._UpDateAuction(this, easError);
}

void Auction::FinishWaitForPayment()
{
    MS_ROLE_DATA* buyer_role{};
    const MAIL_TEMPLATE_DATA* seller_mail_item{};
    const MAIL_TEMPLATE_DATA* seller_mail_deposit{};
    const MAIL_TEMPLATE_DATA* buyer_mail{};
    UINT32 attachlen{};
    INT64 temp_price{};
    BOOL bRet = FALSE;
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    MS_ROLE_DATA* seller_role = CRoleDataMgr::GetInstance().FindRole(m_dwSellerRoleID);
    LOG_PROCESS_ERROR(seller_role);
    buyer_role = CRoleDataMgr::GetInstance().FindRole(m_dwBuyerRoleID);
    LOG_PROCESS_ERROR(buyer_role);
    LOG_PROCESS_ERROR(m_byAuctionStatus == easWaitForPayment);

    seller_mail_item = NULL;
    seller_mail_deposit = NULL;
    buyer_mail = NULL;
    char mail_subject[MAIL_SUBJECT_LEN];
    char attachbuffer[1024];
    attachlen = m_iteminfo.Save(attachbuffer);
    LOG_PROCESS_ERROR(attachlen <= BUFFER_ATTACH_LEN);

    seller_mail_item = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionLeftMoneyExpireReturnItem);
    LOG_PROCESS_ERROR(seller_mail_item);

    seller_mail_deposit = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionLeftMoneyExpireReturnDeposit);
    LOG_PROCESS_ERROR(seller_mail_deposit);

    buyer_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionLeftMoneyExpireBuyer);
    LOG_PROCESS_ERROR(buyer_mail);

    snprintf(mail_subject, MAIL_SUBJECT_LEN, seller_mail_item->m_szMailSubject, m_iteminfo.m_nTemplateID);
    bRet = CMailMgr::GetInstance().SendSystemMailWithBufferAttach(seller_role, mail_subject,
        seller_mail_item->m_szMailContent, attachbuffer, attachlen);
    LOG_PROCESS_ERROR(bRet);

    snprintf(mail_subject, MAIL_SUBJECT_LEN, seller_mail_deposit->m_szMailSubject, m_iteminfo.m_nTemplateID);
    temp_price = m_llDepositPrice - m_llDepositPrice * g_GWConfig.nAuctionDepositTax / RATE_MAX;
    while (temp_price > ONE_TIME_MAX_MONEY)
    {
        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(seller_role, mail_subject,
            seller_mail_deposit->m_szMailContent, MONEYTYPE_02, ONE_TIME_MAX_MONEY);
        LOG_PROCESS_ERROR(bRet);

        temp_price -= ONE_TIME_MAX_MONEY;
    }
    if (temp_price > 0)
    {
        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(seller_role, mail_subject,
            seller_mail_deposit->m_szMailContent, MONEYTYPE_02, (INT32)temp_price);
        LOG_PROCESS_ERROR(bRet);
    }

    snprintf(mail_subject, MAIL_SUBJECT_LEN, buyer_mail->m_szMailSubject, m_iteminfo.m_nTemplateID);
    bRet = CMailMgr::GetInstance().SendSystemMail(buyer_role, mail_subject, buyer_mail->m_szMailContent);
    LOG_PROCESS_ERROR(bRet);

    CAuctionMgr::GetInstance().m_role_buying_auction_list[m_dwBuyerRoleID].remove(this);
    CAuctionMgr::GetInstance()._AddBoughtAuction(m_dwBuyerRoleID, this);

    CAuctionMgr::GetInstance().m_role_selling_auction_list[m_dwSellerRoleID].remove(this);
    CAuctionMgr::GetInstance()._AddSoldAuction(m_dwSellerRoleID, this);

    CAuctionMgr::GetInstance()._UpDateAuction(this, easPaymentExpired);

    // Copied code; logging is temporarily disabled
    //bRet = CMessageHandler::GetInstance().DoDLAuction(m_byGroupID, m_dwSellerRoleID, m_dwBuyerRoleID, m_dwAuctionID, "PaymentExpire",
    //    0, 0, 0, 0, m_iteminfo.m_nTemplateID, m_iteminfo.m_llGuid);
    //LOG_PROCESS_ERROR(bRet);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(m_dwBuyerRoleID);
    if (pOnlineRole)
    {
        CMessageHandler::GetInstance().DoClientAuctionFinished(pOnlineRole->dwCliSessionID, m_dwAuctionID, easPaymentExpired);
    }

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(m_dwSellerRoleID);
    if (pOnlineRole)
    {
        CMessageHandler::GetInstance().DoClientAuctionFinished(pOnlineRole->dwCliSessionID, m_dwAuctionID, easPaymentExpired);
    }

    return;
Exit0:
    CRI("Auction::FinishWaitForPayment() Failed, AuctionID:%u", m_dwAuctionID);
    CAuctionMgr::GetInstance()._UpDateAuction(this, easError);
}

CAuctionMgr CAuctionMgr::ms_Instance;

UINT32 CAuctionMgr::_GetAuctionID(INT32 nGroupID)
{
    // Auction ID generation
    // First 8 bits: group number; remaining 24 bits: sequence starting at 1
    UINT32 auctionid = 0;
    LOG_PROCESS_ERROR(IsValidGroup(nGroupID));

    auctionid = m_dwSeeds[nGroupID] | (nGroupID << 24);

    m_dwSeeds[nGroupID]++;
    if (m_dwSeeds[nGroupID] > 0x00FFFFFF)
        m_dwSeeds[nGroupID] = 1;

Exit0:
    return auctionid;
}

void CAuctionMgr::_SetAuctionID(INT32 nGroupID, UINT32 AuctionID)
{
    LOG_PROCESS_ERROR(IsValidGroup(nGroupID));

    m_dwSeeds[nGroupID] = AuctionID;

Exit0:
    return;
}

CAuctionMgr::CAuctionMgr()
{
    m_threads = NULL;
    m_dwLastCheck = 0;
    m_dwThreadCount = 0;
    m_dwSearchThreadIndex = 0;
    m_nLoadedCount = 0;
}

BOOL CAuctionMgr::Init(UINT32 dwAuctionThreadCount, UINT32 dwAuctionPoolSize)
{
    BOOL bRet;
    UINT64 qwStartTick = GetTickCountEx();

    LOG_PROCESS_ERROR(dwAuctionThreadCount > 0 && dwAuctionThreadCount <= 8);

    bRet = m_AuctionPool.Init(dwAuctionPoolSize);
    LOG_PROCESS_ERROR(bRet);

    m_dwThreadCount = dwAuctionThreadCount;
    m_threads = new CAuctionThread[m_dwThreadCount];
    for (UINT32 i = 0; i < m_dwThreadCount; i++)
    {
        bRet = m_threads[i].Init(dwAuctionPoolSize, 20000);
        LOG_PROCESS_ERROR(bRet);

        bRet = m_threads[i].Start();
        LOG_PROCESS_ERROR(bRet);
    }
    m_dwSearchThreadIndex = 0;
    m_dwLastCheck = 0;

    m_nLoadedCount = 0;
    memset(seed_finder, 0, sizeof(seed_finder));

    INF("Loading Auction Data...");

    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(msqlSelectAuction, _Load, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    INF("Loaded %d Auction Data in (%d ms)", m_nLoadedCount, (INT32)(GetTickCountEx() - qwStartTick));

    for (UINT32 i = 0; i < MAX_SERVER_GROUP_COUNT; i++)
    {
        if (IsValidGroup(i))
        {
            m_dwSeeds[i] = seed_finder[i].FindSeed();
            LOG_PROCESS_ERROR(m_dwSeeds[i]);

            UINT32 tmpLoadedCount = m_nLoadedCount + 1;
            _SetAuctionID(i, tmpLoadedCount);
        }
        else
        {
            m_dwSeeds[i] = 0;
        }
    }

    qwStartTick = GetTickCountEx();
    m_nLoadedCount = 0;

    INF("Loading Auction Deposit Data...");

    bRet = CManageServer::GetInstance().GetSqlMgr().SyncExecSqlStmt(msqlSelectAuctionDeposit, _LoadAllDeposit, NULL, "");
    LOG_PROCESS_ERROR(bRet);

    INF("Loaded %d Auction Deposit Data in %d ms", m_nLoadedCount, (UINT32)(GetTickCountEx() - qwStartTick));

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CAuctionMgr::UnInit()
{
    BOOL bRet;

    if (m_threads)
    {
        for (UINT32 i = 0; i < m_dwThreadCount; ++i)
            m_threads[i].RequestStop();
        for (UINT32 i = 0; i < m_dwThreadCount; i++)
        {
            m_threads[i].Wait();
            bRet = m_threads[i].UnInit();
            LOG_CHECK_ERROR(bRet);
        }
        SAFE_DELETE_ARRAY(m_threads);
    }

    bRet = m_AuctionPool.UnInit();
    LOG_CHECK_ERROR(bRet);
    return TRUE;
}

Auction* CAuctionMgr::GetAuction(UINT32 auctionid)
{
    return m_AuctionPool.Find(auctionid);
}

struct UPDATE_AUCTION
{
    UINT32 m_dwCurTime;

    BOOL operator()(UINT32 auctionid, Auction* auction)
    {
        if (auction->m_byAuctionStatus == easEffective && auction->m_dwFinishTime <= m_dwCurTime)
        {
            auction->Finish();
        }
        if (auction->m_byAuctionStatus == easWaitForPayment && auction->m_dwFinishTime + g_GWConfig.nAuctionWaitForPaymentTime * 60 * 60 <= m_dwCurTime)
        {
            auction->FinishWaitForPayment();
        }

        return TRUE;
    }
};

BOOL CAuctionMgr::MainLoop(void)
{
    SearchResult sr;
    UINT32 dwCurTime = CManageServer::GetInstance().GetGameTime();

    for (UINT32 i = 0; i < m_dwThreadCount; i++)
    {
        while (m_threads[i].GetSearchResult(sr))
        {
            MS_ONLINE_ROLE_DATA* online_role = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(sr.dwRoleID);
            if (online_role)
            {
                LOG_CHECK_ERROR(online_role->dwCliSessionID == sr.dwClientSessionID);
                LOG_CHECK_ERROR(online_role->pRoleData->eAuctionQueryStatus == MS_ROLE_DATA::eqsAuctionSearch);

                online_role->pRoleData->eAuctionQueryStatus = MS_ROLE_DATA::eqsFree;

                CMessageHandler::GetInstance().DoClientAuctionSearchResult(sr.dwClientSessionID, sr.pMsg);
            }
            else
                CManageServer::GetInstance().GetPacketAllocator()->Free((PACKET_BUFFER*)sr.pMsg);
        }
    }

    if (dwCurTime - m_dwLastCheck >= 30)
    {
        UPDATE_AUCTION ua;
        ua.m_dwCurTime = dwCurTime;
        m_AuctionPool.Traverse(ua);

        m_dwLastCheck = dwCurTime;
    }

    return TRUE;
}

INT32 CAuctionMgr::AddItem(MS_ROLE_DATA* role, ItemInfo& iteminfo, UINT8 bytimetype, UINT8 byopen, INT64 llprice, INT64 llbuyoutprice, INT64 lldepositprice)
{
    UINT32 auctionid{};
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    UINT32 actual_buf_len = 0;
    char item_buf[256] = { 0 };
    char item_detail[1024] = { 0 };
    BOOL bRet;
    Auction* auction = NULL;

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(IsValidGroup(role->byGroup));
    LOG_PROCESS_ERROR(bytimetype < MAX_AUCTION_TIME_INTERVAL_TYPE);

    auctionid = _GetAuctionID(role->byGroup);
    LOG_PROCESS_ERROR(auctionid);

    auction = m_AuctionPool.New(auctionid);
    LOG_PROCESS_ERROR(auction);

    memset(auction, 0, sizeof(Auction));

    auction->m_pItemData = GetItemData(iteminfo.m_nTemplateID);
    INF("m_nTemplateID %d", iteminfo.m_nTemplateID);
    INF("auctionid %d", auctionid);
    LOG_PROCESS_ERROR(auction->m_pItemData);

    auction->m_dwAuctionID = auctionid;
    auction->m_iteminfo = iteminfo;
    auction->m_byGroupID = role->byGroup;
    auction->m_dwSellerRoleID = role->dwRoleID;
    auction->m_byOpen = byopen;
    auction->m_llPrice = llprice;
    auction->m_llBuyoutPrice = llbuyoutprice;
    auction->m_llDepositPrice = lldepositprice;
    auction->m_dwStartTime = CManageServer::GetInstance().GetGameTime();
    auction->m_dwFinishTime = auction->m_dwStartTime + 60 * 60 * g_GWConfig.nAuctionTimeInterval[bytimetype];
    auction->m_byAuctionStatus = easEffective;
    m_role_selling_auction_list[role->dwRoleID].push_back(auction);

    actual_buf_len = auction->m_iteminfo.Save(item_buf);

    // Combine item data into a string; temporarily disabled
    //auction->m_iteminfo.SaveString(item_detail, 1024);

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlInsertAuction, NULL, NULL, "ucudududlllududucucbs",
        role->byGroup, auction->m_dwAuctionID, auction->m_dwSellerRoleID, auction->m_dwBuyerRoleID,
        auction->m_llPrice, auction->m_llBuyoutPrice, auction->m_llDepositPrice,
        auction->m_dwStartTime, auction->m_dwFinishTime, auction->m_byOpen, auction->m_byAuctionStatus,
        item_buf, actual_buf_len, item_detail);
    LOG_PROCESS_ERROR(bRet);

    // Copied code; logging is temporarily disabled
    //bRet = CManageServer::GetInstance().DoDLAuction(role->byGroup, role->dwRoleID, 0, auction->m_dwAuctionID, "Sell",
    //    price, buyoutprice, depositprice, timetype, iteminfo.m_nTemplateID, iteminfo.m_llGuid);
    //LOG_PROCESS_ERROR(bRet);

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(role->dwRoleID);
    if (pOnlineRole)
    {
        CMessageHandler::GetInstance().DoClientAuctionSell(pOnlineRole->dwCliSessionID, auction);
    }

    for (UINT32 i = 0; i < m_dwThreadCount; i++)
    {
        bRet = m_threads[i].AddAuction(auction);
        LOG_PROCESS_ERROR(bRet);
    }

    return errSuccess;

Exit0:
    if (auction)
        m_AuctionPool.Delete(auction);
    return errItemUnknown;
}

INT32 CAuctionMgr::CancelItem(MS_ROLE_DATA* role, UINT32 auctionid)
{
    INT32 nResult = errAuctionUnknown;
    Auction* auction = m_AuctionPool.Find(auctionid);
    PROCESS_ERROR_RET_CODE(auction && auction->m_byAuctionStatus == easEffective, errAuctionItemNotFound);
    LOG_PROCESS_ERROR(auction->m_dwSellerRoleID == role->dwRoleID);
    PROCESS_ERROR_RET_CODE(auction->m_dwBuyerRoleID == 0, errAuctionOnBid);

    auction->Finish();
    return errSuccess;

Exit0:
    return nResult;
}

void CAuctionMgr::_UpDateAuction(Auction* auction, UINT8 status)
{
    LOG_PROCESS_ERROR(auction);

    if (auction->m_byAuctionStatus == easEffective)
    {
        for (UINT32 i = 0; i < m_dwThreadCount; i++)
        {
            BOOL bRet = m_threads[i].DelAuction(auction->m_dwAuctionID);
            LOG_PROCESS_ERROR(bRet);
        }
    }

    auction->m_byAuctionStatus = status;

    _UpDateAuctionSql(auction);

Exit0:
    return;
}

void CAuctionMgr::_UpDateAuctionSql(Auction* auction)
{
    BOOL bRet{};
    LOG_PROCESS_ERROR(auction);

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlUpdateAuction, NULL, NULL, "udluducud",
        auction->m_dwBuyerRoleID, auction->m_llPrice, auction->m_dwFinishTime, auction->m_byAuctionStatus, auction->m_dwAuctionID);
    LOG_PROCESS_ERROR(bRet);

Exit0:
    return;
}

INT32 CAuctionMgr::BidItem(MS_ROLE_DATA* role, UINT32 auctionid, INT64 price)
{
    MS_ONLINE_ROLE_DATA* pOnlineRole = NULL;
    Auction* auction = m_AuctionPool.Find(auctionid);
    BOOL bRet = FALSE;
    INT32 nResult = errAuctionUnknown;

    LOG_PROCESS_ERROR(role);
    LOG_PROCESS_ERROR(IsValidGroup(role->byGroup));
    LOG_PROCESS_ERROR_RET_CODE(auction, errAuctionItemNotFound);
    LOG_PROCESS_ERROR(auction->m_dwSellerRoleID != role->dwRoleID);
    LOG_PROCESS_ERROR(auction->m_byGroupID == role->byGroup);

    if (auction->m_byAuctionStatus != easEffective)
    {
        ReturnDeposit(auction->m_iteminfo.m_nTemplateID, role->dwRoleID, auction->m_llDepositPrice);
        return errSuccess;
    }

    // Add the bidder to the deposit list if absent, for both buyout and repeated bids
    if (!IsBidder(auctionid, role->dwRoleID))
    {
        AddDeposit(auction, role->dwRoleID);
        m_role_buying_auction_list[role->dwRoleID].push_back(auction);
    }

    // Copied code; logging is temporarily disabled
    //bRet = CMessageHandler::GetInstance().DoDLAuction(auction->m_byGroupID, auction->m_dwSellerRoleID, role->dwRoleID, auction->m_dwAuctionID,
    //    "Bid", price, 0, 0, 0, auction->m_iteminfo.m_nTemplateID, auction->m_iteminfo.m_llGuid);
    //LOG_PROCESS_ERROR(bRet);

    if (price >= auction->GetNextPrice())
    {
        if (auction->m_dwBuyerRoleID && auction->m_dwBuyerRoleID != role->dwRoleID)
        {
            const MAIL_TEMPLATE_DATA* original_buyer_mail = NULL;
            char mail_subject[MAIL_SUBJECT_LEN];
            MS_ROLE_DATA* original_buyer = NULL;

            original_buyer_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionOverBid);
            LOG_PROCESS_ERROR(original_buyer_mail);

            original_buyer = CRoleDataMgr::GetInstance().FindRole(auction->m_dwBuyerRoleID);
            LOG_PROCESS_ERROR(original_buyer);

            snprintf(mail_subject, MAIL_SUBJECT_LEN, original_buyer_mail->m_szMailSubject, auction->m_iteminfo.m_nTemplateID);
            bRet = CMailMgr::GetInstance().SendSystemMail(original_buyer, mail_subject, original_buyer_mail->m_szMailContent);
            LOG_PROCESS_ERROR(bRet);
        }

        if (auction->m_llBuyoutPrice == 0 || auction->m_llPrice < auction->m_llBuyoutPrice)
        {
            const MAIL_TEMPLATE_DATA* new_buyer_mail = NULL;
            char mail_subject[MAIL_SUBJECT_LEN];

            new_buyer_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionBidSuccess);
            LOG_PROCESS_ERROR(new_buyer_mail);

            snprintf(mail_subject, MAIL_SUBJECT_LEN, new_buyer_mail->m_szMailSubject, auction->m_iteminfo.m_nTemplateID);
            bRet = CMailMgr::GetInstance().SendSystemMail(role, mail_subject, new_buyer_mail->m_szMailContent);
            LOG_PROCESS_ERROR(bRet);
        }

        auction->m_dwBuyerRoleID = role->dwRoleID;
        auction->m_llPrice = price;

        // If the bid reaches the buyout price, end the auction immediately
        if (auction->m_llBuyoutPrice && auction->m_llPrice >= auction->m_llBuyoutPrice)
        {
            auction->Finish();
        }
        // If the bid is below the buyout price, update auction data
        else
        {
            if (auction->m_dwFinishTime < CManageServer::GetInstance().GetGameTime() + g_GWConfig.nAuctionFinishDelayTime)
            {
                auction->m_dwFinishTime = CManageServer::GetInstance().GetGameTime() + g_GWConfig.nAuctionFinishDelayTime;
            }

            _UpDateAuctionSql(auction);

            for (UINT32 i = 0; i < m_dwThreadCount; i++)
            {
                bRet = m_threads[i].UpdateAuctionPrice(auctionid, price, role->dwRoleID, auction->m_dwFinishTime);
                LOG_PROCESS_ERROR(bRet);
            }
        }
    }

    pOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(role->dwRoleID);
    if (pOnlineRole)
    {
        CMessageHandler::GetInstance().DoClientAuctionBuy(pOnlineRole->dwCliSessionID, auction);
    }

    return errSuccess;

Exit0:
    return errAuctionUnknown;
}

INT32 CAuctionMgr::PaidLeftMoney(Auction* auction)
{
    UINT32 attachlen{};
    INT64 temp_price{};
    BOOL bRet = FALSE;
    INT32 nResult = errAuctionUnknown;
    MS_ONLINE_ROLE_DATA* pOnlineRoleSeller = NULL;
    MS_ONLINE_ROLE_DATA* pOnlineRoleBuyer = NULL;

    MS_ROLE_DATA* seller_role = NULL;
    MS_ROLE_DATA* buyer_role = NULL;
    const MAIL_TEMPLATE_DATA* seller_mail = NULL;
    const MAIL_TEMPLATE_DATA* buyer_mail = NULL;
    char mail_subject_buyer[MAIL_SUBJECT_LEN] = { 0 };
    char mail_subject_seller[MAIL_SUBJECT_LEN] = { 0 };
    char mail_content_buyer[MAIL_CONTENT_LEN] = { 0 };
    char mail_content_seller[MAIL_CONTENT_LEN] = { 0 };
    INT64 tax = 0;
    char attachbuffer[256] = { 0 };
    INT32 mail_cd = 0;

    LOG_PROCESS_ERROR(auction);
    LOG_PROCESS_ERROR(auction->m_dwBuyerRoleID);
    LOG_PROCESS_ERROR(auction->m_byAuctionStatus == easWaitForPayment);

    attachlen = auction->m_iteminfo.Save(attachbuffer);
    LOG_PROCESS_ERROR(attachlen <= BUFFER_ATTACH_LEN);

    buyer_role = CRoleDataMgr::GetInstance().FindRole(auction->m_dwBuyerRoleID);
    LOG_PROCESS_ERROR(buyer_role);

    seller_role = CRoleDataMgr::GetInstance().FindRole(auction->m_dwSellerRoleID);
    LOG_PROCESS_ERROR(seller_role);

    seller_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionSuccessSeller);
    LOG_PROCESS_ERROR(seller_mail);

    buyer_mail = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionSuccessBuyer);
    LOG_PROCESS_ERROR(buyer_mail);

    // Send mail to the buyer
    snprintf(mail_subject_buyer, MAIL_SUBJECT_LEN, buyer_mail->m_szMailSubject, auction->m_iteminfo.m_nTemplateID);
    snprintf(mail_content_buyer, MAIL_CONTENT_LEN, buyer_mail->m_szMailContent, auction->m_llPrice);
    bRet = CMailMgr::GetInstance().SendSystemMailWithBufferAttach(buyer_role, mail_subject_buyer, mail_content_buyer, attachbuffer, attachlen);
    LOG_PROCESS_ERROR(bRet);

    // The mailed item can be claimed after its cooldown expires
    mail_cd = g_GWConfig.nAuctionMoneyCDTime - (CManageServer::GetInstance().GetGameTime() - auction->m_dwFinishTime);
    if (mail_cd < 0)
        mail_cd = 0;

    // Calculate tax and final amount, then mail the seller
    tax = GetAuctionTax(auction->m_llPrice);
    snprintf(mail_subject_seller, MAIL_SUBJECT_LEN, seller_mail->m_szMailSubject, auction->m_iteminfo.m_nTemplateID);
    snprintf(mail_content_seller, MAIL_CONTENT_LEN, seller_mail->m_szMailContent, auction->m_llPrice - tax, tax);

    temp_price = auction->m_llPrice - tax;
    while (temp_price > ONE_TIME_MAX_MONEY)
    {
        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(seller_role, mail_subject_seller, mail_content_seller, MONEYTYPE_02, ONE_TIME_MAX_MONEY);
        LOG_PROCESS_ERROR(bRet);

        temp_price -= ONE_TIME_MAX_MONEY;
    }
    if (temp_price > 0)
    {
        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(seller_role, mail_subject_seller, mail_content_seller, MONEYTYPE_02, (INT32)temp_price);
        LOG_PROCESS_ERROR(bRet);
    }

    // Remove the buyer from Selling and add to Sold
    m_role_selling_auction_list[auction->m_dwSellerRoleID].remove(auction);
    _AddSoldAuction(auction->m_dwSellerRoleID, auction);

    // Remove the buyer from Buying and add to Bought
    m_role_buying_auction_list[auction->m_dwBuyerRoleID].remove(auction);
    _AddBoughtAuction(auction->m_dwBuyerRoleID, auction);

    _UpDateAuction(auction, easFinished);

    // Copied code; logging is temporarily disabled
    //bRet = CMessageHandler::GetInstance().DoDLAuction(auction->m_byGroupID, auction->m_dwSellerRoleID, auction->m_dwBuyerRoleID, auction->m_dwAuctionID,
    //    "PaidLeftMoney", 0, 0, 0, 0, auction->m_iteminfo.m_nTemplateID, auction->m_iteminfo.m_llGuid);
    //LOG_PROCESS_ERROR(bRet);

    // Seller
    pOnlineRoleSeller = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(auction->m_dwSellerRoleID);
    if (pOnlineRoleSeller)
    {
        CMessageHandler::GetInstance().DoClientAuctionFinished(pOnlineRoleSeller->dwCliSessionID, auction->m_dwAuctionID, easFinished);
    }

    // Buyer
    pOnlineRoleBuyer = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(auction->m_dwBuyerRoleID);
    if (pOnlineRoleBuyer)
    {
        CMessageHandler::GetInstance().DoClientAuctionPayLeftMoney(pOnlineRoleBuyer->dwCliSessionID, auction->m_dwAuctionID);
        CMessageHandler::GetInstance().DoClientAuctionFinished(pOnlineRoleBuyer->dwCliSessionID, auction->m_dwAuctionID, easFinished);
    }

    return errSuccess;

Exit0:
    return nResult;
}

INT32 CAuctionMgr::SearchItem(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, INT32 templateid)
{
    const ITEM_DATA* item_data = GetItemData(templateid);
    BOOL bRet = FALSE;

    LOG_PROCESS_ERROR(item_data);
    LOG_PROCESS_ERROR(item_data->m_bAuction);
    LOG_PROCESS_ERROR(sortby < esbTotal);

    bRet = m_threads[(m_dwSearchThreadIndex++) % m_dwThreadCount].SearchAuction(sessionid, roleid, groupid, sortby, asc, templateid);
    LOG_PROCESS_ERROR(bRet);

    return errSuccess;

Exit0:
    return errItemUnknown;
}

INT32 CAuctionMgr::SearchType(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, UINT8 auctiontype)
{
    BOOL bRet{};
    LOG_PROCESS_ERROR(auctiontype < MAX_AUCTION_TYPE);
    LOG_PROCESS_ERROR(sortby < esbTotal);

    bRet = m_threads[(m_dwSearchThreadIndex++) % m_dwThreadCount].SearchAuctionType(sessionid, roleid, groupid, sortby, asc, auctiontype);
    LOG_PROCESS_ERROR(bRet);

    return errSuccess;

Exit0:
    return errItemUnknown;
}

static CAuctionMgr::AUCTION_LIST empty_auction_list;

const CAuctionMgr::AUCTION_LIST& CAuctionMgr::GetBuyingAuctions(UINT32 roleid) const
{
    ROLE_AUCTION_LIST::const_iterator it = m_role_buying_auction_list.find(roleid);
    if (it == m_role_buying_auction_list.end())
        return empty_auction_list;
    else
        return it->second;
}

const CAuctionMgr::AUCTION_LIST& CAuctionMgr::GetSellingAuctions(UINT32 roleid) const
{
    ROLE_AUCTION_LIST::const_iterator it = m_role_selling_auction_list.find(roleid);
    if (it == m_role_selling_auction_list.end())
        return empty_auction_list;
    else
        return it->second;
}

const CAuctionMgr::AUCTION_LIST& CAuctionMgr::GetBoughtAuctions(UINT32 roleid) const
{
    ROLE_AUCTION_LIST::const_iterator it = m_role_bought_auction_list.find(roleid);
    if (it == m_role_bought_auction_list.end())
        return empty_auction_list;
    else
        return it->second;
}

const CAuctionMgr::AUCTION_LIST& CAuctionMgr::GetSoldAuctions(UINT32 roleid) const
{
    ROLE_AUCTION_LIST::const_iterator it = m_role_sold_auction_list.find(roleid);
    if (it == m_role_sold_auction_list.end())
        return empty_auction_list;
    else
        return it->second;
}

BOOL CAuctionMgr::IsBidder(UINT32 auctionid, UINT32 roleid) const
{
    ALL_DEPOSIT_MAP::const_iterator it = m_deposit_map.find(auctionid);
    if (it == m_deposit_map.end())
        return FALSE;
    else
        return it->second.find(roleid) != it->second.end();
}

BOOL CAuctionMgr::AddDeposit(Auction* auction, UINT32 roleid)
{
    BOOL bRet{};
    LOG_PROCESS_ERROR(auction);
    LOG_PROCESS_ERROR(!IsBidder(auction->m_dwAuctionID, roleid));

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlInserAuctionDeposit, NULL, NULL,
        "ucududl", auction->m_byGroupID, auction->m_dwAuctionID, roleid, auction->m_llDepositPrice);
    LOG_PROCESS_ERROR(bRet);

    m_deposit_map[auction->m_dwAuctionID][roleid] = auction->m_llDepositPrice;

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CAuctionMgr::DelDeposit(Auction* auction, UINT32 roleid)
{
    BOOL bRet{};
    LOG_PROCESS_ERROR(auction);
    LOG_PROCESS_ERROR(IsBidder(auction->m_dwAuctionID, roleid));

    bRet = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(msqlDeleteAuctionDeposit, NULL, NULL,
        "udud", auction->m_dwAuctionID, roleid);
    LOG_PROCESS_ERROR(bRet);

    m_deposit_map[auction->m_dwAuctionID].erase(roleid);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CAuctionMgr::ReturnDeposit(INT32 itemid, UINT32 roleid, INT64 depositprice)
{
    INT64 temp_price{};
    const MAIL_TEMPLATE_DATA* mail_data = NULL;
    char mail_subjece[MAIL_SUBJECT_LEN] = { 0 };
    MS_ROLE_DATA* bidder = NULL;
    BOOL bRet = FALSE;

    mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAuctionFailBuyer);
    LOG_PROCESS_ERROR(mail_data);

    bidder = CRoleDataMgr::GetInstance().FindRole(roleid);
    LOG_PROCESS_ERROR(bidder);

    snprintf(mail_subjece, MAIL_SUBJECT_LEN, mail_data->m_szMailSubject, itemid);

    temp_price = depositprice;
    while (temp_price > ONE_TIME_MAX_MONEY)
    {
        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(bidder, mail_subjece,
            mail_data->m_szMailContent, MONEYTYPE_02, ONE_TIME_MAX_MONEY);
        LOG_PROCESS_ERROR(bRet);

        temp_price -= ONE_TIME_MAX_MONEY;
    }
    if (temp_price > 0)
    {
        bRet = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(bidder, mail_subjece,
            mail_data->m_szMailContent, MONEYTYPE_02, (INT32)temp_price);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;

Exit0:
    return FALSE;
}

UINT32 CAuctionMgr::GetBuyerCount(UINT32 auctionid) const
{
    ALL_DEPOSIT_MAP::const_iterator it = m_deposit_map.find(auctionid);
    if (it == m_deposit_map.end())
        return 0;
    else
        return (UINT32)it->second.size();
}

BOOL CAuctionMgr::_Load(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet = FALSE;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8 byGroupID;
        UINT32 dwAuctionID;
        UINT32 dwSellerRoleID;
        UINT32 dwBuyerRoleID;
        INT64  llPrice;
        INT64  llBuyoutPrice;
        INT64  llDepositPrice;
        UINT32 dwStartTime;
        UINT32 dwFinishTime;
        UINT8 byOpen;
        UINT8 byAuctionStatus;
        UINT32 dwItemBufferLen;
        char pItemBuffer[64];
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin(); it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->byGroupID));

        bRet = CAuctionMgr::ms_Instance._LoadAuction(pLoadResult->byGroupID, pLoadResult->dwAuctionID, pLoadResult->pItemBuffer, pLoadResult->dwItemBufferLen,
            pLoadResult->dwSellerRoleID, pLoadResult->dwBuyerRoleID, pLoadResult->llPrice, pLoadResult->llBuyoutPrice, pLoadResult->llDepositPrice,
            pLoadResult->dwStartTime, pLoadResult->dwFinishTime, pLoadResult->byOpen, pLoadResult->byAuctionStatus);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAuctionMgr::_LoadAuction(UINT8 groupid, UINT32 auctionid, const char* itembuf, UINT32 itemlen, UINT32 sellerid, UINT32 buyerid, INT64 price, INT64 buyoutprice, INT64 depositprice, UINT32 starttime, UINT32 finishtime, UINT8 open, UINT8 auctionstatus)
{
    INT32 group_id{};
    BOOL bRet{};
    UINT32 actual_item_len = 0;

    Auction* auction = m_AuctionPool.New(auctionid);
    LOG_PROCESS_ERROR(auction);

    memset(auction, 0, sizeof(Auction));

    actual_item_len = auction->m_iteminfo.Load(itembuf);
    LOG_PROCESS_ERROR(actual_item_len == itemlen);

    auction->m_pItemData = GetItemData(auction->m_iteminfo.m_nTemplateID);
    LOG_PROCESS_ERROR(auction->m_pItemData);

    auction->m_byGroupID = groupid;
    auction->m_dwAuctionID = auctionid;
    auction->m_dwSellerRoleID = sellerid;
    auction->m_dwBuyerRoleID = buyerid;
    auction->m_llPrice = price;
    auction->m_llBuyoutPrice = buyoutprice;
    auction->m_llDepositPrice = depositprice;
    auction->m_dwStartTime = starttime;
    auction->m_dwFinishTime = finishtime;
    auction->m_byOpen = open;
    auction->m_byAuctionStatus = auctionstatus;

    if (auctionstatus == easEffective)
    {
        for (UINT32 i = 0; i < m_dwThreadCount; i++)
        {
            BOOL bRet = m_threads[i].AddAuction(auction);
            LOG_PROCESS_ERROR(bRet);
        }
    }

    if (auctionstatus == easEffective || auctionstatus == easWaitForPayment)
    {
        m_role_selling_auction_list[sellerid].push_back(auction);

        if (buyerid)
        {
            m_role_buying_auction_list[buyerid].push_back(auction);
        }
    }
    else
    {
        _AddSoldAuction(sellerid, auction);
    }

    group_id = auctionid >> 24;
    LOG_PROCESS_ERROR(IsValidGroup(group_id));
    LOG_PROCESS_ERROR(group_id == groupid);

    bRet = seed_finder[groupid].CheckID(auctionid & 0x00FFFFFF);
    LOG_PROCESS_ERROR(bRet);

    m_nLoadedCount++;

    return TRUE;

Exit0:
    if (auction)
        m_AuctionPool.Delete(auction);
    return FALSE;
}

BOOL CAuctionMgr::_LoadAllDeposit(std::list<void*>& rResult, void* pUserData)
{
    BOOL bRet = FALSE;

#pragma pack(push, 1)
    struct LOAD_RESULT
    {
        UINT8 byGroupID;
        UINT32 dwAuctionID;
        UINT32 dwRoleID;
        INT64 llDepositPrice;
    };
#pragma pack(pop)

    for (std::list<void*>::iterator it = rResult.begin();
        it != rResult.end(); ++it)
    {
        LOAD_RESULT* pLoadResult = (LOAD_RESULT*)*it;

        LOG_PROCESS_ERROR(IsValidGroup(pLoadResult->byGroupID));

        bRet = CAuctionMgr::ms_Instance._LoadDeposit(pLoadResult->byGroupID, pLoadResult->dwAuctionID, pLoadResult->dwRoleID, pLoadResult->llDepositPrice);
        LOG_PROCESS_ERROR(bRet);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAuctionMgr::_LoadDeposit(UINT8 groupid, UINT32 auctionid, UINT32 roleid, INT64 depositprice)
{
    Auction* auction = m_AuctionPool.Find(auctionid);
    LOG_PROCESS_ERROR(auction);
    LOG_PROCESS_ERROR(auction->m_byGroupID == groupid);

    if (auction->m_dwBuyerRoleID != roleid)
    {
        m_role_buying_auction_list[roleid].push_back(auction);
    }

    m_deposit_map[auctionid][roleid] = depositprice;

    m_nLoadedCount++;

    return TRUE;
Exit0:
    return FALSE;
}

void CAuctionMgr::_AddSoldAuction(UINT32 roleid, Auction* auction)
{
    AUCTION_LIST& auction_list = m_role_sold_auction_list[roleid];
    auction_list.push_back(auction);

    if (auction_list.size() > g_GWConfig.nMaxAuctionSoldItems)
        auction_list.pop_front();
}

void CAuctionMgr::_AddBoughtAuction(UINT32 roleid, Auction* auction)
{
    AUCTION_LIST& auction_list = m_role_bought_auction_list[roleid];
    auction_list.push_back(auction);

    if (auction_list.size() > g_GWConfig.nMaxAuctionBoughtItems)
        auction_list.pop_front();
}

BOOL CAuctionMgr::SEED_FINDER::CheckID(UINT32 _id)
{
    LOG_PROCESS_ERROR(_id);
    LOG_PROCESS_ERROR(_id > last_id);

    if (first_id != 0)
    {
        first_id = _id;
    }

    if (last_id != 0)
    {
        UINT32 gap = _id - last_id;
        if (biggest_gap < gap)
        {
            biggest_gap = gap;
            biggest_gap_start_id = last_id;
        }
    }

    last_id = _id;

    return TRUE;
Exit0:
    return FALSE;
}

UINT32 CAuctionMgr::SEED_FINDER::FindSeed()
{
    if (first_id == 0)
    {
        return 1;
    }

    if (biggest_gap == 0)
    {
        return first_id + 1;
    }

    UINT32 cycle_gap = (0x00FFFFFF - last_id) + first_id;

    if (cycle_gap > biggest_gap)
    {
        LOG_PROCESS_ERROR(cycle_gap > 500000);
        return last_id + 1;
    }
    else
    {
        LOG_PROCESS_ERROR(biggest_gap > 500000);
        return biggest_gap_start_id + 1;
    }

Exit0:
    return 0;
}

