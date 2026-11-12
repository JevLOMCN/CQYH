#ifndef _MS_SQL_STMT_LIST_H_
#define _MS_SQL_STMT_LIST_H_

#include "sql_mgr.h"

enum MS_SQL_STMT_ID
{
                                        // period, average, max
    msqlSelectServerVar,                // start
	msqlInsertServerVar,				// running, 1, 10
    msqlUpdateServerVar,                // running, 1, 10  

    msqlSelectRankList,                 // start
    msqlUpdateRankList,                 // running, 20, 10000
    msqlInsertRankList,                 // running, 20, 10000
    msqlDeleteRankList,                 // running, 20, 10000
    msqlUpdateLastRank,

    msqlSelectShopLog,
    msqlInsertShopLog,

    msqlSelectAuction,
    msqlUpdateAuction,
    msqlInsertAuction,
    msqlDeleteAuction,

    msqlSelectExchangeOrder,
    msqlInsertExchangeOrder,
    msqlUpdateExchangeOrderCount,
    msqlUpdateExchangeOrderStatus,

    msqlSelectExchangeHistory,
    msqlInsertExchangeHistory,
    msqlUpdateExchangeHistory,

    msqlSelectExchangeLog,
    msqlInsertExchangeLog,

    mysqlSelectMaster,                  // start
    mysqlInsertMaster,                  // running, 2, 10
    mysqlDeleteMaster,                  // running, 2, 10
    mysqlClearMaster,
    mysqlSelectMasterCZ,                // start
    mysqlInsertMasterCZ,                // running, 8, 40                
    mysqlDeleteMasterCZ,                // running, 8, 40
    mysqlUpdateMasterCZ,                // running, 2, 20
    mysqlClearMasterCZ,

    mysqlSelectGuild,                   // start 
    mysqlInsertGuild,                   // running, 1, 10
    mysqlUpdateGuild,                   // running, 10, 200(400)
    mysqlDeleteGuild,                   // running, 1, 10
    mysqlUpdateGuildNotice,             // running, 1, 10
    mysqlUpdateGuildAnnounce,           //
    mysqlUpdateGuildLeader,             // running, 1, 10
    mysqlClearGuild,
    mysqlSelectGuildWar,
    mysqlInsertGuildWar,

    mysqlSelectGMember,                 // start 
    mysqlInsertGMember,                 // running, 2,200
    mysqlDeleteGMember,                 // running, 2,200
    mysqlUpdateGMember,                 // running, 2,500
    mysqlDeleteGMemberByGuildID,
    mysqlClearGMember,

    mysqlInsertArena,                   // start 
    mysqlSelectArena,                   // start 
    mysqlUpdateArenaFund,               // running, 20, 10000
    mysqlUpdateArenaRank,               // running, 20, 5000

    mysqlDeleteExpiredMail,
    mysqlSelectMail,
    mysqlSelectRoleData,
    mysqlInsertMail,
    mysqlDeleteMail,
    mysqlUpdateMailRead,
    mysqlUpdateMailAttach,
    mysqlUpdateMailAttachLocked,

    mysqlSelectFriend,
    mysqlInsertFriend,
    mysqlDeleteFriend,
    mysqlUpdateFriend,

    mysqlSelectRole,
    mysqlSelectGM,
    mysqlReplaceGM,
    mysqlDeleteGM,

	mysqlSelectGroup,
	mysqlInsertGroup,
	mysqlUpdateGroup,
	mysqlDeleteGroup,

    mysqlSelectLoginQuery,    // Client login: query account credentials

    msqlSelectAuctionDeposit,
    msqlInserAuctionDeposit,
    msqlDeleteAuctionDeposit,


    msqlTotal
};

extern SQL_STMT_DATA g_MSSqlStmtList[msqlTotal];

#endif  // _MS_SQL_STMT_LIST_H_