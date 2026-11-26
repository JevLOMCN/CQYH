#include "stdafx.h"
#include "message_handler.h"
#include "message_handler_Ranklist.h"
#include "MMOUtil.h"
#include "RankListManager.h"
#include "MMOFacade.h"

void CMessageHandler::Send_QueryRanklist(INT ranklistID, INT pageNum)
{
	C2M_QUERY_RANKLIST msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.wMsg = c2m_query_ranklist;
	msg.nRanklistID = ranklistID;
	msg.nPageSize = pageNum;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_m2c_query_ranklist(const char* buf, UINT32 size)
{
	if (!buf || size < sizeof(M2C_QUERY_RANKLIST))
		return;
	//ASSERT_CHECK_ERROR_RETURN(buf && size == sizeof(M2C_QUERY_RANKLIST));
	M2C_QUERY_RANKLIST* msg = (M2C_QUERY_RANKLIST*)buf;
	if(msg == NULL)
		return;
	RankListManager::getSingleton().Clear();
	RankListManager::getSingleton().SetType(RANK_LIST(msg->byRankListType));
	RankListManager::getSingleton().SetTotalType(RANK_LIST(msg->byRankListType));
	RankListManager::getSingleton().SetSelfRank(msg->wSelfRank);
	RankListManager::getSingleton().SetPageTotal( msg->nTotalPage);
	RankListManager::getSingleton().SetSelfLastRank(msg->wLastSelfRank);

	INT maxSize = RankListManager::getSingleton().getMaxRankListCount();
	if (RankListManager::getSingleton().GetPageTotal() >= maxSize)
	{
		RankListManager::getSingleton().SetPageTotal(maxSize);
	}
	
	for (INT index = 0; index < MAX_RANK_NODE_NUM_PER_PAGE; index++)
	{
		RankListManager::getSingleton().AddData(msg->llKey[index], msg->llValue[index],msg->wLastRank[index]);
	}
	RankListManager::getSingleton().NotifyUpdateRankList();
}
