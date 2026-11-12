#include "stdafx.h"
#include "message_handler.h"
#include "TeamManager.h"

void CMessageHandler::Send_TeamAuctionBid( INT itemTempId , INT bidMoney , INT index )
{
	//This sends itemTempid only for verification, cannot be fully trusted
	C2G_TEAM_AUCTION_BID msg;
	msg.wMsg = c2g_team_auction_bid;
	msg.dwAuctionIndex = index;
//	msg.nItemID = itemTempId;
	msg.dwOfferPrice = bidMoney;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_TeamAuctionAbandonBid( INT itemTempId , INT itemIndex )
{
	//This sends itemTempid only for verification, cannot be fully trusted
	C2G_TEAM_AUCTION_ABANDON msg;
	msg.wMsg = c2g_team_auction_abandon;
//	msg.nItemID = itemTempId;
	msg.dwAuctionIndex = itemIndex;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_team_auction_abandon(const char* buf, UINT32 size)
{
	G2C_TEAM_AUCTION_ABANDON* msg = (G2C_TEAM_AUCTION_ABANDON*)buf;
	if(msg)
	{
		if (msg->dwRoleID == MMOFacade::getSingleton().GetMainPlayer()->ServerID)
		{
			TeamManager::getSingleton().NotifyGiveUpBid( msg->dwAuctionIndex/* , msg->nItemID */);
		}
		
	}
}

void CMessageHandler::Parse_g2c_team_auction_begin(const char* buf, UINT32 size)
{
	//Start auctioning this item
	G2C_TEAM_AUCTION_BEGIN* msg = (G2C_TEAM_AUCTION_BEGIN*)buf;
	INT BuffSize = 0;

//	ItemInfo iteminfo;
//	INT buflen = iteminfo.Load(msg->acItemInfo + BuffSize);

	TeamManager::getSingleton().StartTeamAuctionItem( msg->dwAuctionIndex , msg->dwBottomPrice, msg->byBidderCount);
	
}

void CMessageHandler::Parse_g2c_team_auction_add_item(const char* buf, UINT32 size)
{
	G2C_TEAM_AUCTION_ADD_ITEM* msg = (G2C_TEAM_AUCTION_ADD_ITEM*)buf;
	INT BuffSize = 0;

//	if( TeamManager::getSingleton().GetMyTeamID() != msg->dwTeamID )
//	{
//		return;
//	}	

	ItemInfo iteminfo;
	INT buflen = iteminfo.Load( msg->acItemInfo + BuffSize );

	ETeamAuctionItemState state = ETAIS_CanBid;
//	if( !msg->bCanBid )
//	{
//		state = ETAIS_NoPermit;
//	}
	TeamManager::getSingleton().AddTeamAuctionItem( iteminfo , state ,  msg->dwAuctionIndex );
	
}

void CMessageHandler::Parse_g2c_team_auction_notify(const char* buf, UINT32 size)
{
	//Someone bid
	G2C_TEAM_AUCTION_NOTIFY* msg = (G2C_TEAM_AUCTION_NOTIFY*)buf;
	if(msg)
	{
		TeamManager::getSingleton().OtherBid( msg->dwBidderRoleID , msg->dwAuctionIndex , msg->dwOfferPrice );
	}
}

void CMessageHandler::Parse_g2c_team_auction_end(const char* buf, UINT32 size)
{
	G2C_TEAM_AUCTION_END* msg = (G2C_TEAM_AUCTION_END*)buf;
	if(msg)
	{
		TeamManager::getSingleton().EndTeamAuctionItem( msg->dwAuctionIndex , msg->dwWinnerRoleID );
	}
}