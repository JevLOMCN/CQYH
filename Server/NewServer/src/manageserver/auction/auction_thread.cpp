#include "stdafx.h"
#include "auction_thread.h"

#include "auction_mgr.h"
#include "m2c_message.h"
#include "manageserver.h"
#include <algorithm>
#include <memory>
#include "worker_queue.h"

#include "server_config.h"
#include "dump.h"

enum
{
    AUC_REQUEST_ADD,
    AUC_REQUEST_DEL,
    AUC_REQUEST_UPDATE_PRICE,
    AUC_REQUEST_SEARCH,
    AUC_REQUEST_SEARCH_TYPE
};

struct AucRequest
{
    virtual ~AucRequest() = default;
    UINT8 m_eOp;
};
struct AucRequestAdd : public AucRequest
{
    Auction auction;
};
struct AucRequestDel : public AucRequest
{
    INT32 auctionid;
};
struct AucRequestUpdatePrice : public AucRequest
{
    UINT32 auctionid;
    INT64 price;
    UINT32 buyerid;
    UINT32 finishtime;
};
struct AucRequestSearch : public AucRequest
{
    UINT32 sessionid;
    UINT32 roleid;
    UINT8  groupid;
    UINT8  sortby;
    UINT8  asc;
    INT32  templateid;
};
struct AucRequestSearchType : public AucRequest
{
    UINT32 sessionid;
    UINT32 roleid;
    UINT8  groupid;
    UINT8  sortby;
    UINT8  asc;
    UINT8  auctiontype;
};

#define CMP_MEMBER(member_) if (a1->m_auction.member_ != a2->m_auction.member_) return a1->m_auction.member_ < a2->m_auction.member_
#define CMP_NODE_MEMBER(member_) if (a1->member_ != a2->member_) return a1->member_ < a2->member_
#define CMP_AUCTION_ID return a1->m_auction.m_dwAuctionID < a2->m_auction.m_dwAuctionID

bool CmpTotalPrice(const AuctionThreadNode* a1, const AuctionThreadNode* a2)
{
    CMP_MEMBER(m_llPrice);
    CMP_MEMBER(m_dwFinishTime);
    CMP_AUCTION_ID;
}
bool CmpSinglePrice(const AuctionThreadNode* a1, const AuctionThreadNode* a2)
{
    CMP_NODE_MEMBER(m_llSinglePrice);
    CMP_MEMBER(m_dwFinishTime);
    CMP_AUCTION_ID;
}
bool CmpBuyoutPrice(const AuctionThreadNode* a1, const AuctionThreadNode* a2)
{
    if (a1->m_auction.m_llBuyoutPrice != a2->m_auction.m_llBuyoutPrice)
    {
        if (a1->m_auction.m_llBuyoutPrice == 0)
            return false;
        else if (a2->m_auction.m_llBuyoutPrice == 0)
            return true;
        else
            return a1->m_auction.m_llBuyoutPrice < a2->m_auction.m_llBuyoutPrice;
    }
    CMP_MEMBER(m_dwFinishTime);
    CMP_AUCTION_ID;
}
bool CmpBuyoutSinglePrice(const AuctionThreadNode* a1, const AuctionThreadNode* a2)
{
    if (a1->m_llBuyoutSinglePrice != a2->m_llBuyoutSinglePrice)
    {
        if (a1->m_auction.m_llBuyoutPrice == 0)
            return false;
        else if (a2->m_auction.m_llBuyoutPrice == 0)
            return true;
        else
            return a1->m_llBuyoutSinglePrice < a2->m_llBuyoutSinglePrice;
    }
    CMP_MEMBER(m_dwFinishTime);
    CMP_AUCTION_ID;
}
bool CmpFinishTime(const AuctionThreadNode* a1, const AuctionThreadNode* a2)
{
    CMP_MEMBER(m_dwFinishTime);
    CMP_AUCTION_ID;
}

typedef bool (*CmpFunc)(const AuctionThreadNode* a1, const AuctionThreadNode* a2);
CmpFunc cmpfunc[esbTotal] = { CmpTotalPrice, CmpBuyoutPrice, CmpFinishTime };

BOOL CAuctionThread::Init(UINT32 dwAuctionPoolSize, UINT32 dwListSize)
{
    BOOL bRet = FALSE;

    bRet = m_requests.Init(dwListSize);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_results.Init(dwListSize);
    LOG_PROCESS_ERROR(bRet);

    bRet = m_AuctionPool.Init(dwAuctionPoolSize);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL CAuctionThread::UnInit()
{
    Wait();
    while (auto* request = static_cast<AucRequest*>(m_requests.Pop()))
        delete request;
    while (auto* result = static_cast<SearchResult*>(m_results.Pop()))
    {
        CManageServer::GetInstance().GetPacketAllocator()->Free((PACKET_BUFFER*)result->pMsg);
        delete result;
    }
    for (auto& lists : m_sorted_lists_map)
        lists.clear();
    for (auto& lists : m_sorted_type_lists_map)
        lists.clear();
    BOOL bRet = FALSE;

    bRet = m_requests.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_results.UnInit();
    LOG_CHECK_ERROR(bRet);

    bRet = m_AuctionPool.UnInit();
    LOG_CHECK_ERROR(bRet);

    return TRUE;
}

BOOL CAuctionThread::AddAuction(const Auction* auction)
{
    BOOL bRet = FALSE;

    AucRequestAdd* req = new AucRequestAdd;
    LOG_PROCESS_ERROR(req);

    req->m_eOp = AUC_REQUEST_ADD;
    req->auction = *auction;

    bRet = m_requests.Push(req);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (req)
        delete req;

    return FALSE;
}

BOOL CAuctionThread::DelAuction(UINT32 auctionid)
{
    BOOL bRet = FALSE;

    AucRequestDel* req = new AucRequestDel;
    LOG_PROCESS_ERROR(req);

    req->m_eOp = AUC_REQUEST_DEL;
    req->auctionid = auctionid;

    bRet = m_requests.Push(req);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (req)
        delete req;

    return FALSE;
}

BOOL CAuctionThread::UpdateAuctionPrice(UINT32 auctionid, INT64 price, UINT32 buyerid, UINT32 finishtime)
{
    BOOL bRet = FALSE;

    AucRequestUpdatePrice* req = new AucRequestUpdatePrice;
    LOG_PROCESS_ERROR(req);

    req->m_eOp = AUC_REQUEST_UPDATE_PRICE;
    req->auctionid = auctionid;
    req->price = price;
    req->buyerid = buyerid;
    req->finishtime = finishtime;

    bRet = m_requests.Push(req);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (req)
        delete req;

    return FALSE;
}

BOOL CAuctionThread::SearchAuction(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, INT32 templateid)
{
    BOOL bRet = FALSE;

    AucRequestSearch* req = new AucRequestSearch;
    LOG_PROCESS_ERROR(req);

    LOG_PROCESS_ERROR(sortby < esbTotal);

    req->m_eOp = AUC_REQUEST_SEARCH;
    req->sessionid = sessionid;
    req->roleid = roleid;
    req->groupid = groupid;
    req->sortby = sortby;
    req->asc = asc;
    req->templateid = templateid;

    bRet = m_requests.Push(req);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (req)
        delete req;

    return FALSE;
}

BOOL CAuctionThread::SearchAuctionType(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, UINT8 auctiontype)
{
    BOOL bRet = FALSE;

    AucRequestSearchType* req = new AucRequestSearchType;
    LOG_PROCESS_ERROR(req);

    req->m_eOp = AUC_REQUEST_SEARCH_TYPE;
    req->sessionid = sessionid;
    req->roleid = roleid;
    req->groupid = groupid;
    req->sortby = sortby;
    req->asc = asc;
    req->auctiontype = auctiontype;

    bRet = m_requests.Push(req);
    LOG_PROCESS_ERROR(bRet);

    return TRUE;

Exit0:
    if (req)
        delete req;

    return FALSE;
}

BOOL CAuctionThread::GetSearchResult(SearchResult& res)
{
    SearchResult* res_ = (SearchResult*)m_results.Pop();
    if (res_)
    {
        res = *res_;

        delete res_;
        return TRUE;
    }
    else
        return FALSE;
}

bool CAuctionThread::_PublishResult(SearchResult* result)
{
    const auto destroy = [](SearchResult* value)
    {
        CManageServer::GetInstance().GetPacketAllocator()->Free((PACKET_BUFFER*)value->pMsg);
        delete value;
    };
    std::unique_ptr<SearchResult, decltype(destroy)> owned(result, destroy);
    if (!cqyh::PublishUntilStopped(m_results, owned.get(), *this))
        return false;
    owned.release();
    return true;
}

void CAuctionThread::_ProcessRequest()
{
    std::unique_ptr<AucRequest> request(static_cast<AucRequest*>(m_requests.Pop()));
    auto* req = request.get();
    if (req)
    {
        switch (req->m_eOp)
        {
            case AUC_REQUEST_ADD:
            {
                AucRequestAdd* req_add = (AucRequestAdd*)req;
                AuctionThreadNode* auction = m_AuctionPool.New(req_add->auction.m_dwAuctionID);
                LOG_CHECK_ERROR(auction);
                LOG_CHECK_ERROR(req_add->auction.m_byGroupID < MAX_SERVER_GROUP_COUNT);

                if (auction && req_add->auction.m_byGroupID < MAX_SERVER_GROUP_COUNT)
                {
                    auction->m_auction = req_add->auction;

                    if (auction->m_auction.m_pItemData->m_byDurType == DURTYPE_STACK)
                    {
                        auction->m_nItemCount = auction->m_auction.m_iteminfo.m_nCurDur;
                        LOG_CHECK_ERROR(auction->m_nItemCount > 0);

                        if (auction->m_nItemCount <= 0)
                            auction->m_nItemCount = 1;
                    }
                    else
                        auction->m_nItemCount = 1;

                    // Different
                    auction->m_llSinglePrice = auction->m_auction.m_llPrice / auction->m_nItemCount;
                    auction->m_llBuyoutSinglePrice = auction->m_auction.m_llBuyoutPrice / auction->m_nItemCount;

                    _AddAuctionToList(auction);
                }
            }
            break;
            case AUC_REQUEST_DEL:
            {
                AucRequestDel* req_del = (AucRequestDel*)req;
                AuctionThreadNode* auction = m_AuctionPool.Find(req_del->auctionid);
                LOG_CHECK_ERROR(auction);
                LOG_CHECK_ERROR(auction && auction->m_auction.m_byGroupID < MAX_SERVER_GROUP_COUNT);

                if (auction && auction->m_auction.m_byGroupID < MAX_SERVER_GROUP_COUNT)
                {
                    _DelAuctionFromList(auction);
                    m_AuctionPool.Delete(auction);
                }
            }
            break;
            case AUC_REQUEST_UPDATE_PRICE:
            {
                AucRequestUpdatePrice* req_update = (AucRequestUpdatePrice*)req;
                AuctionThreadNode* auction = m_AuctionPool.Find(req_update->auctionid);
                LOG_CHECK_ERROR(auction);
                LOG_CHECK_ERROR(auction && auction->m_auction.m_byGroupID < MAX_SERVER_GROUP_COUNT);

                if (auction && auction->m_auction.m_byGroupID < MAX_SERVER_GROUP_COUNT)
                {
                    _DelAuctionFromList(auction);
                    auction->m_auction.m_llPrice = req_update->price;
                    auction->m_auction.m_dwBuyerRoleID = req_update->buyerid;
                    auction->m_auction.m_dwFinishTime = req_update->finishtime;
                    auction->m_llSinglePrice = auction->m_auction.m_llPrice / auction->m_nItemCount;
                    _AddAuctionToList(auction);
                }
            }
            break;
            case AUC_REQUEST_SEARCH:
            {
                AucRequestSearch* req_search = (AucRequestSearch*)req;
                SearchResult* sr = _SearchAuctionInList(req_search->sessionid, req_search->roleid,
                    req_search->groupid, req_search->sortby, req_search->asc, req_search->templateid);
                LOG_CHECK_ERROR(sr);
                if (sr)
                    _PublishResult(sr);
            }
            break;
            case AUC_REQUEST_SEARCH_TYPE:
            {
                AucRequestSearchType* req_search_type = (AucRequestSearchType*)req;
                SearchResult* sr = _SearchAuctionType(req_search_type->sessionid, req_search_type->roleid,
                    req_search_type->groupid, req_search_type->sortby, req_search_type->asc, req_search_type->auctiontype);
                LOG_CHECK_ERROR(sr);
                if (sr)
                    _PublishResult(sr);
            }
            break;
            default:
                LOG_CHECK_ERROR(FALSE);
        }
    }
}

void CAuctionThread::ThrdProc()
{
    for (; m_bstart; )
    {
        __try
        {
            _ProcessRequest();
            WaitForStop(std::chrono::milliseconds(16));
        }
        __except (DumpExceptionFilter(GetExceptionInformation()))
        {

        }

    }
}

INT32 CAuctionThread::SortedLists::GetAuctions(UINT8 sortby, UINT8 asc, AuctionThreadNode* auctions[], INT32 auctionsize)
{
    INT32 auc_count = 0;
    LOG_PROCESS_ERROR(sortby < esbTotal);

    if (asc)
    {
        for (std::list<AuctionThreadNode*>::const_iterator it = lists[sortby].begin(); it != lists[sortby].end(); ++it)
        {
            auctions[auc_count++] = *it;
            if (auc_count > AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE)
                break;
        }
    }
    else
    {
        for (std::list<AuctionThreadNode*>::const_reverse_iterator it = lists[sortby].rbegin(); it != lists[sortby].rend(); ++it)
        {
            auctions[auc_count++] = *it;
            if (auc_count > AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE)
                break;
        }
    }
Exit0:
    return auc_count;
}

void CAuctionThread::SortedLists::AddAuction(AuctionThreadNode* auction)
{
    for (INT32 i = 0; i < esbTotal; i++)
    {
        std::list<AuctionThreadNode*>::iterator it = lists[i].begin();
        for (; it != lists[i].end(); ++it)
        {
            if (cmpfunc[i](auction, *it))
                break;
        }
        lists[i].insert(it, auction);
    }
}

void CAuctionThread::_AddAuctionToList(AuctionThreadNode* auction)
{
    UINT8 auctiontype{};
    UINT8 group_id = auction->m_auction.m_byGroupID;
    LOG_PROCESS_ERROR(group_id < MAX_SERVER_GROUP_COUNT);

    auctiontype = auction->m_auction.m_pItemData->m_byAuctionType;
    LOG_PROCESS_ERROR(auctiontype);
    LOG_PROCESS_ERROR(auctiontype < MAX_AUCTION_TYPE);

    m_sorted_lists_map[group_id][auction->m_auction.m_pItemData->m_nTemplateID].AddAuction(auction);
    m_sorted_type_lists_map[group_id][auctiontype].AddAuction(auction);
    m_sorted_type_lists_map[group_id][0].AddAuction(auction);

Exit0:
    return;
}

void CAuctionThread::SortedLists::DelAuction(AuctionThreadNode* auction)
{
    for (INT32 i = 0; i < esbTotal; i++)
    {
        lists[i].remove(auction);
    }
}

void CAuctionThread::_DelAuctionFromList(AuctionThreadNode* auction)
{
    UINT8 auctiontype{};
    UINT8 group_id = auction->m_auction.m_byGroupID;
    LOG_PROCESS_ERROR(group_id < MAX_SERVER_GROUP_COUNT);

    auctiontype = auction->m_auction.m_pItemData->m_byAuctionType;
    LOG_PROCESS_ERROR(auctiontype);
    LOG_PROCESS_ERROR(auctiontype < MAX_AUCTION_TYPE);

    m_sorted_lists_map[group_id][auction->m_auction.m_pItemData->m_nTemplateID].DelAuction(auction);
    m_sorted_type_lists_map[group_id][auctiontype].DelAuction(auction);
    m_sorted_type_lists_map[group_id][0].DelAuction(auction);

Exit0:
    return;
}

SearchResult* CAuctionThread::_SearchAuctionInList(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, INT32 templateid)
{
    UINT32 auc_item_len = 0;
    SearchResult* sr = NULL;
    std::map<INT32, SortedLists>::iterator it;

    ExternalMessage::M2C_AUCTION_SEARCH* pMsg = (ExternalMessage::M2C_AUCTION_SEARCH*)CManageServer::GetInstance().GetPacketAllocator()->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(IsValidGroup(groupid));
    LOG_PROCESS_ERROR(sortby < esbTotal);

    pMsg->wMsg = ExternalMessage::m2c_auction_search;
    pMsg->btAuctionItemCount = 0;

    it = m_sorted_lists_map[groupid].find(templateid);
    if (it != m_sorted_lists_map[groupid].end())
    {
        AuctionThreadNode* atnode[AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE];
        INT32 auc_count = it->second.GetAuctions(sortby, asc, atnode, AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE);
        for (INT32 i = 0; i < auc_count; i++)
        {
            const AuctionThreadNode* au = atnode[i];

            ExternalMessage::AUCTION_ITEM_INFO* aii = (ExternalMessage::AUCTION_ITEM_INFO*)(pMsg->acAuctionItems + auc_item_len);
            UINT32 itemlen = au->m_auction.m_iteminfo.Save(aii->acItem);

            aii->dwAuctionID = au->m_auction.m_dwAuctionID;
            aii->dwSellerRoleID = au->m_auction.m_dwSellerRoleID;
            aii->dwBuyerRoleID = au->m_auction.m_dwBuyerRoleID;
            aii->llPrice = au->m_auction.m_llPrice;
            aii->llBuyoutPrice = au->m_auction.m_llBuyoutPrice;
            aii->llDepositPrice = au->m_auction.m_llDepositPrice;
            aii->dwStartTime = au->m_auction.m_dwStartTime;
            aii->dwFinishTime = au->m_auction.m_dwFinishTime;
            aii->byAuctionStatus = au->m_auction.m_byAuctionStatus;

            auc_item_len += (sizeof(ExternalMessage::AUCTION_ITEM_INFO) + itemlen);
            pMsg->btAuctionItemCount++;

            if (pMsg->btAuctionItemCount >= AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE)
            {
                break;
            }
        }
    }

    pMsg->wSize = (UINT16)(sizeof(ExternalMessage::M2C_AUCTION_SEARCH) + auc_item_len);

    sr = new SearchResult;
    LOG_PROCESS_ERROR(sr);

    sr->dwClientSessionID = sessionid;
    sr->dwRoleID = roleid;
    sr->pMsg = pMsg;

    return sr;

Exit0:
    if (pMsg)
        CManageServer::GetInstance().GetPacketAllocator()->Free((PACKET_BUFFER*)pMsg);
    if (sr)
        delete sr;

    return NULL;
}

SearchResult* CAuctionThread::_SearchAuctionType(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, UINT8 auctiontype)
{
    UINT32 auc_item_len = 0;
    SearchResult* sr = NULL;
    std::map<UINT16, SortedLists>::iterator it;

    ExternalMessage::M2C_AUCTION_SEARCH* pMsg = (ExternalMessage::M2C_AUCTION_SEARCH*)CManageServer::GetInstance().GetPacketAllocator()->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    LOG_PROCESS_ERROR(IsValidGroup(groupid));
    LOG_PROCESS_ERROR(auctiontype < MAX_AUCTION_TYPE);
    LOG_PROCESS_ERROR(sortby < esbTotal);

    pMsg->wMsg = ExternalMessage::m2c_auction_search;
    pMsg->btAuctionItemCount = 0;

    it = m_sorted_type_lists_map[groupid].find(auctiontype);
    if (it != m_sorted_type_lists_map[groupid].end())
    {
        AuctionThreadNode* atnode[AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE];
        INT32 auc_count = it->second.GetAuctions(sortby, asc, atnode, AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE);

        for (INT32 i = 0; i < auc_count; i++)
        {
            const AuctionThreadNode* au = atnode[i];

            ExternalMessage::AUCTION_ITEM_INFO* aii = (ExternalMessage::AUCTION_ITEM_INFO*)(pMsg->acAuctionItems + auc_item_len);
            UINT32 itemlen = au->m_auction.m_iteminfo.Save(aii->acItem);

            aii->dwAuctionID = au->m_auction.m_dwAuctionID;
            aii->dwSellerRoleID = au->m_auction.m_dwSellerRoleID;
            aii->dwBuyerRoleID = au->m_auction.m_dwBuyerRoleID;
            aii->llPrice = au->m_auction.m_llPrice;
            aii->llBuyoutPrice = au->m_auction.m_llBuyoutPrice;
            aii->llDepositPrice = au->m_auction.m_llDepositPrice;
            aii->dwStartTime = au->m_auction.m_dwStartTime;
            aii->dwFinishTime = au->m_auction.m_dwFinishTime;
            aii->byAuctionStatus = au->m_auction.m_byAuctionStatus;

            auc_item_len += (sizeof(ExternalMessage::AUCTION_ITEM_INFO) + itemlen);
            pMsg->btAuctionItemCount++;

            if (pMsg->btAuctionItemCount >= AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE)
            {
                break;
            }
        }
    }

    pMsg->wSize = (UINT16)(sizeof(ExternalMessage::M2C_AUCTION_SEARCH) + auc_item_len);

    sr = new SearchResult;
    LOG_PROCESS_ERROR(sr);

    sr->dwClientSessionID = sessionid;
    sr->dwRoleID = roleid;
    sr->pMsg = pMsg;

    return sr;

Exit0:
    if (pMsg)
        CManageServer::GetInstance().GetPacketAllocator()->Free((PACKET_BUFFER*)pMsg);
    if (sr)
        delete sr;
}
