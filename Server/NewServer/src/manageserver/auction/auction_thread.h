#ifndef _AUCTION_THREAD_H_
#define _AUCTION_THREAD_H_

#include "thread.h"
#include "circle_list.h"
#include "external_message_header.h"
#include "auction_mgr.h"
#include "server_def.h"
#include "auction_def.h"

struct M2C_AUCTION_SEARCH_RESULT;

struct SearchResult
{
    UINT32 dwClientSessionID;
    UINT32 dwRoleID;
    ExternalMessage::FLEX_MESSAGE_HEADER* pMsg;
};

struct AuctionThreadNode
{
    Auction m_auction;
    INT32 m_nItemCount;
    INT64 m_llSinglePrice;
    INT64 m_llBuyoutSinglePrice;
};

class CAuctionThread : public CThread
{
public:
    ~CAuctionThread() override { UnInit(); }
    BOOL Init(UINT32 dwAuctionPoolSize, UINT32 dwListSize);
    BOOL UnInit();

    BOOL AddAuction(const Auction* auction);
    BOOL DelAuction(UINT32 auctionid);
    BOOL UpdateAuctionPrice(UINT32 auctionid, INT64 price, UINT32 buyerid, UINT32 finishtime);
    BOOL SearchAuction(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, INT32 templateid);
    BOOL SearchAuctionType(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, UINT8 auctiontype);
    BOOL GetSearchResult(SearchResult& res);

protected:
    virtual VOID ThrdProc();

private:
    void _ProcessRequest();
    bool _PublishResult(SearchResult* result);
    void _AddAuctionToList(AuctionThreadNode* auction);
    void _DelAuctionFromList(AuctionThreadNode* auction);
    SearchResult* _SearchAuctionInList(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, INT32 templateid);
    SearchResult* _SearchAuctionType(UINT32 sessionid, UINT32 roleid, UINT8 groupid, UINT8 sortby, UINT8 asc, UINT8 auctiontype);
    CCircleList m_requests;
    CCircleList m_results;

    CIDObjectPool<AuctionThreadNode, UINT32> m_AuctionPool;

    struct SortedLists
    {
        std::list<AuctionThreadNode*> lists[esbTotal];
        INT32 GetAuctions(UINT8 sortby, UINT8 asc, AuctionThreadNode* auctions[], INT32 auctionsize);
        void AddAuction(AuctionThreadNode* auction);
        void DelAuction(AuctionThreadNode* auction);
    };

    std::map<INT32, SortedLists> m_sorted_lists_map[MAX_SERVER_GROUP_COUNT];
    std::map<UINT16, SortedLists> m_sorted_type_lists_map[MAX_SERVER_GROUP_COUNT];

    //void _AddAuctionToSortedLists(AuctionThreadNode* auction, SortedLists& sorted_lists);
    //void _DelAuctionFromSortedLists(AuctionThreadNode* auction, SortedLists& sorted_lists);
};

#endif // _AUCTION_THREAD_H_
