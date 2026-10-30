#include "stdafx.h"
#include "ms_sql_stmt_list.h"

SQL_STMT_DATA g_MSSqlStmtList[msqlTotal] =
{
    { 0, msqlSelectServerVar,	"SELECT GroupID{uc}, VarIndex{ul}, VarValue{d} FROM variable"},
	{ 0, msqlInsertServerVar,	"INSERT INTO variable (GroupID, VarIndex, VarValue) VALUES ([uc], [ul], [d])"},
    { 0, msqlUpdateServerVar,	"UPDATE variable SET VarValue = [d] WHERE GroupID = [uc] AND VarIndex = [ul]"},

	{ 1, msqlSelectRankList,	"SELECT GroupID{uc}, RankListType{d}, RankKey{l}, RankValue{l}, LastRank{d} FROM ranklist ORDER BY UpdateTime"},
    { 1, msqlUpdateRankList,	"UPDATE ranklist SET RankValue = [l], UpdateTime=NOW() WHERE GroupID = [uc] AND RankListType = [d] AND RankKey = [l]"},
    { 1, msqlInsertRankList,	"INSERT INTO ranklist(GroupID, RankListType, RankKey, RankValue, UpdateTime) VALUES ([uc], [d], [l], [l], NOW())"},
    { 1, msqlDeleteRankList,	"DELETE FROM ranklist WHERE GroupID = [uc] AND RankListType = [d] AND RankKey = [l]"},
	{ 1, msqlUpdateLastRank,	"UPDATE ranklist SET LastRank = [d] WHERE GroupID = [uc] AND RankListType = [d] AND RankKey = [l]" },

    { 2, msqlSelectShopLog,		"SELECT ItemTemplateID{d}, ItemCount{d}, Cost{d}, UNIX_TIMESTAMP(ShopTime){d} FROM shop_log WHERE RoleID = [d] AND ShopTime >= SUBDATE(now(), 7) LIMIT 100"},
    { 2, msqlInsertShopLog,		"INSERT INTO shop_log (GroupID, RoleID, ItemTemplateID, ItemCount, Cost, ShopTime) VALUES ([uc], [d], [d], [d], [d], now())"},

    { 3, msqlSelectAuction,    "SELECT GroupID{uc}, AuctionID{ud}, Seller{ud}, Buyer{ud}, Price{l}, BuyoutPrice{l}, DepositPrice{l}, UNIX_TIMESTAMP(StartTime){ud}, UNIX_TIMESTAMP(FinishTime){ud}, Open{uc}, AuctionStatus{uc}, ItemBuffer{b64}, ItemDetail{b1024} FROM auction"},
    { 3, msqlUpdateAuction,    "UPDATE auction SET Buyer = [ud], Price = [l], FinishTime = FROM_UNIXTIME([ud]), AuctionStatus = [uc]  WHERE AuctionID = [ud]"},
    { 3, msqlInsertAuction,    "INSERT INTO auction (GroupID, AuctionID, Seller, Buyer, Price, BuyoutPrice, DepositPrice, StartTime, FinishTime, Open, AuctionStatus, ItemBuffer, ItemDetail) VALUES ([uc], [ud], [ud], [ud], [l], [l], [l], FROM_UNIXTIME([ud]), FROM_UNIXTIME([ud]), [uc], [uc], [b64], [s1024])"},
    { 3, msqlDeleteAuction,    "DELETE FROM auction WHERE AuctionID = [ud]"},

    { 4, msqlSelectExchangeOrder,       "SELECT GroupID{uc}, OrderID{ul}, UNIX_TIMESTAMP(OrderTime){ud}, OrderType{uc}, OrderStatus{uc}, RoleID{ud}, Price{d}, Count{d}, CurCount{d}, CurMoney{d}, BillingOrderID{s32} FROM exchange_order WHERE OrderStatus = [uc] OR OrderTime >= SUBDATE(now(), 7) ORDER BY OrderID ASC"},
    { 4, msqlInsertExchangeOrder,       "INSERT INTO exchange_order (GroupID, OrderID, OrderTime, OrderType, OrderStatus, RoleID, Price, Count, CurCount, CurMoney, BillingOrderID) VALUES ([uc], [ul], FROM_UNIXTIME([ud]), [uc], [uc], [ud], [d], [d], [d], [d], [s32])"},
    { 4, msqlUpdateExchangeOrderCount,  "UPDATE exchange_order SET CurCount = [d], CurMoney = [d] WHERE OrderID = [ul]" },
    { 4, msqlUpdateExchangeOrderStatus, "UPDATE exchange_order SET OrderStatus = [uc] WHERE OrderID = [ul]" },

    { 4, msqlSelectExchangeHistory, "SELECT GroupID{uc}, UNIX_TIMESTAMP(HistoryTime){ud}, Open{d}, Close{d}, High{d}, Low{d}, Volume{d}, TotalPrice{l} FROM exchange_history WHERE HistoryTime >= SUBDATE(now(), 10)" },
    { 4, msqlInsertExchangeHistory, "INSERT INTO exchange_history (GroupID, HistoryTime, Open, Close, High, Low, Volume, TotalPrice) VALUES ([uc], FROM_UNIXTIME([ud]), [d], [d], [d], [d], [d], [l])" },
    { 4, msqlUpdateExchangeHistory, "UPDATE exchange_history SET Open = [d], Close = [d], High = [d], Low = [d], Volume = [d], TotalPrice = [l] WHERE GroupID = [uc] AND HistoryTime = FROM_UNIXTIME([ud])" },

    { 4, msqlSelectExchangeLog, "SELECT GroupID{uc}, UNIX_TIMESTAMP(ExchangeTime){ud}, SellerRoleID{ud}, SellerOrderID{ul}, BuyerRoleID{ud}, BuyerOrderID{ul}, Price{d}, Count{d} FROM exchange_log WHERE ExchangeTime >= SUBDATE(now(), 7) ORDER BY ExchangeTime ASC" },
    { 4, msqlInsertExchangeLog, "INSERT INTO exchange_log (GroupID, ExchangeTime, SellerRoleID, SellerOrderID, BuyerRoleID, BuyerOrderID, Price, Count) VALUES ([uc], FROM_UNIXTIME([ud]), [ud], [ul], [ud], [ul], [d], [d])" },

    { 5, mysqlSelectMaster, "SELECT GroupID{uc}, TeacherID{ud}, StudentID{ud} FROM master WHERE DeleteTime IS NULL"},
    { 5, mysqlInsertMaster, "INSERT INTO master (GroupID, TeacherID, StudentID, CreateTime) VALUES ([uc], [ud], [ud], now())"},
    { 5, mysqlDeleteMaster, "DELETE FROM master WHERE TeacherID = [ud] AND StudentID = [ud]"},
    //{ 5, mysqlDeleteMaster, "UPDATE master SET DeleteTime = now() WHERE TeacherID = [ud] AND StudentID = [ud]" },
    { 5, mysqlClearMaster, "DELETE FROM master WHERE DeleteTime IS NOT NULL AND UNIX_TIMESTAMP(DeleteTime) < [ud]"},

    { 5, mysqlSelectMasterCZ, "SELECT GroupID{uc}, ID{ul}, cz{uc} FROM master_cz WHERE DeleteTime IS NULL"},
    { 5, mysqlInsertMasterCZ, "INSERT INTO master_cz (GroupID, ID, cz, CreateTime) VALUES ([uc], [ul], [uc], now())"},
    { 5, mysqlDeleteMasterCZ, "DELETE FROM master_cz WHERE ID = [ul]"},
    { 5, mysqlUpdateMasterCZ, "UPDATE master_cz SET cz = [uc] WHERE id = [ul]"},
    { 5, mysqlClearMasterCZ, "DELETE FROM master_cz WHERE DeleteTime IS NOT NULL AND UNIX_TIMESTAMP(DeleteTime) < [ud]" },

    { 6, mysqlSelectGuild, "SELECT GuildID{ud}, Version{uh}, GuildName{s24}, GroupID{uc}, LeaderID{ud}, CreaterName{s24}, UNIX_TIMESTAMP(CreateTime){d}, Announce{s100}, Notice{s200}, MainData{z60000} FROM guild WHERE DeleteTime IS NULL" },
    { 6, mysqlInsertGuild, "INSERT INTO guild (GuildID, Version, GuildName, GroupID, LeaderID, CreaterName, CreateTime, Announce, MainData) VALUES ([ud], [uh], [s24], [uc], [ud], [s24], now(), [s100], [z60000])"},
    { 6, mysqlUpdateGuild, "UPDATE guild SET MainData = [z60000] WHERE GuildID = [ud] LIMIT 1"},
    { 6, mysqlDeleteGuild, "UPDATE guild SET DeleteTime = now() WHERE GuildID = [ud] LIMIT 1"},
    { 6, mysqlUpdateGuildNotice, "UPDATE guild SET Notice = [s200] WHERE GuildID = [ud] LIMIT 1"},
    { 6, mysqlUpdateGuildAnnounce, "UPDATE guild SET Announce = [s100] WHERE GuildID = [ud] LIMIT 1"},
    { 6, mysqlUpdateGuildLeader, "UPDATE guild SET LeaderID = [ud] WHERE GuildID = [ud] LIMIT 1"},
    { 6, mysqlClearGuild , "DELETE FROM guild WHERE DeleteTime IS NOT NULL AND UNIX_TIMESTAMP(DeleteTime) < [ud]"},

    { 6, mysqlSelectGuildWar, "SELECT GroupID{uc}, IntruderID{ud}, DefenderID{ud}, UNIX_TIMESTAMP(BeginDayTime){ud} FROM guild_war WHERE UNIX_TIMESTAMP(BeginDayTime) > [ud] ORDER BY UNIX_TIMESTAMP(BeginDayTime) ASC"},
    { 6, mysqlInsertGuildWar, "INSERT INTO guild_war (GroupID, IntruderID, DefenderID, BeginDayTime) VALUES([uc], [ud], [ud], FROM_UNIXTIME([ud]))" },

    { 6, mysqlSelectGMember, "SELECT GroupID{uc}, RoleID{ud}, GuildID{ud}, Position{uc}, Flag{uc}, Salary{d}, UNIX_TIMESTAMP(DrawSalaryTime){ud}, Pow{d} FROM guild_member WHERE DeleteTime IS NULL"},
    { 6, mysqlInsertGMember, "INSERT INTO guild_member (RoleID, GroupID, GuildID, Position, Flag, Salary, DrawSalaryTime, CreateTime) VALUES ( [ud], [uc], [ud], [uc], [uc], [d], FROM_UNIXTIME([ud]), now())"},
    //{ 6, mysqlDeleteGMember, "UPDATE guild_member SET DeleteTime = now() WHERE RoleID = [ud] LIMIT 1" },
    { 6, mysqlDeleteGMember, "DELETE FROM guild_member WHERE RoleID = [ud] LIMIT 1" },
    { 6, mysqlUpdateGMember, "UPDATE guild_member SET Position = [uc], Flag = [uc], Salary = [d], DrawSalaryTime = FROM_UNIXTIME([ud]), Pow = [d] WHERE RoleID = [ud] LIMIT 1"},
    //{ 6, mysqlDeleteGMemberByGuildID, "UPDATE guild_member SET DeleteTime = now() WHERE GuildID = [ud]" },
    { 6, mysqlDeleteGMemberByGuildID, "DELETE FROM guild_member WHERE GuildID = [ud]" },
    { 6, mysqlClearGMember, "DELETE FROM guild_member WHERE DeleteTime IS NOT NULL AND UNIX_TIMESTAMP(DeleteTime) < [ud]" },

    { 7, mysqlInsertArena, "INSERT INTO arena (GroupID, Fund, `Rank`) VALUES ([uc], [ul], [b324])" },
    { 7, mysqlSelectArena, "SELECT GroupID{uc}, Fund{ul}, `Rank`{b324} FROM arena"},
    { 7, mysqlUpdateArenaFund, "UPDATE arena SET Fund = [ul] WHERE GroupID = [uc]" },
    { 7, mysqlUpdateArenaRank, "UPDATE arena SET `Rank` = [b324] WHERE GroupID = [uc]"},

    { 8, mysqlDeleteExpiredMail, "DELETE FROM mail WHERE MailTime < SUBDATE(now(), 30)"},
    { 8, mysqlSelectMail, "SELECT GroupID{uc}, MailID{ud}, UNIX_TIMESTAMP(MailTime){ud}, SenderRoleID{ud}, ReceiverRoleID{ud}, MailType{uc}, IsNew{uc}, AttachLocked{uc}, MailSubject{s60}, MailContent{s420}, AttachType{uc}, Attach{b64} FROM mail ORDER BY MailID ASC"},
    { 8, mysqlSelectRoleData, "SELECT RoleData{b32768} FROM role WHERE RoleID = [ud]"},
    { 8, mysqlInsertMail, "INSERT INTO mail (GroupID, MailID, MailTime, SenderRoleID, ReceiverRoleID, MailType, MailSubject, MailContent, AttachType, Attach) VALUES ([uc], [ud], now(), [ud], [ud], [uc], [s60], [s420], [uc], [b64])"},
    { 8, mysqlDeleteMail, "DELETE FROM mail WHERE MailID = [ud]"},
    { 8, mysqlUpdateMailRead, "UPDATE mail SET IsNew = 0 WHERE MailID = [ud]"},
    { 8, mysqlUpdateMailAttach, "UPDATE mail SET AttachLocked = 0, AttachType = 0, Attach = '' WHERE MailID = [ud]"},
    { 8, mysqlUpdateMailAttachLocked, "UPDATE mail SET AttachLocked = [c] WHERE MailID = [ud]"},

    { 9, mysqlSelectFriend, "SELECT RoleID{ud}, FriendID{ud}, FriendGroup{uc} FROM friend WHERE RoleID = [ud] or FriendID = [ud] ORDER BY CreateTime ASC", },
    { 9, mysqlInsertFriend, "INSERT INTO friend (GroupID, RoleID, FriendID, FriendGroup, CreateTime) VALUES ([uc], [ud], [ud], [uc], now())", },
    { 9, mysqlDeleteFriend, "DELETE FROM friend WHERE RoleID = [ud] AND FriendID = [ud]", },
    { 9, mysqlUpdateFriend, "UPDATE friend SET FriendGroup = [uc] WHERE RoleID = [ud] AND FriendID = [ud]", },

    // Default code used UserName [64]; changed to [32]
	//{ 10, mysqlSelectRole, "SELECT RoleID{ud}, RoleName{s24}, UserName{s64}, UNIX_TIMESTAMP(LoginTime){ud}, GroupID{uc}, BaseData{b512} FROM role WHERE RoleDeleted = 0"},
	{ 10, mysqlSelectRole, "SELECT RoleID{ud}, RoleName{s24}, UserName{s32}, UNIX_TIMESTAMP(LoginTime){ud}, GroupID{uc}, BaseData{b512} FROM role WHERE RoleDeleted = 0"},
    { 10, mysqlSelectGM , "SELECT GroupID{uc}, ID{ul}, UNIX_TIMESTAMP(Time){ud} from gm" },
    { 10, mysqlReplaceGM, "REPLACE INTO gm SET GroupID = [uc], ID = [ul], Time = FROM_UNIXTIME([ud])" },
    { 10, mysqlDeleteGM, "DELETE FROM gm WHERE ID = [ul]" },

	{ 0, mysqlSelectGroup, "SELECT GroupID{uc}, RegisterIp{s31} FROM group_ms" },
	{ 0, mysqlInsertGroup, "INSERT INTO group_ms (GroupID, RegisterIp, RegisterTime) VALUES ([uc], [s31], NOW())" },
	{ 0, mysqlUpdateGroup, "UPDATE group_ms SET RegisterTime = NOW() WHERE GroupID = [uc] AND RegisterIp = [s31]" },
	{ 0, mysqlDeleteGroup, "DELETE FROM group_ms WHERE RegisterIp = [s31]" },

    // "SELECT GroupID{uc}, ID{ul}, cz{uc} FROM master_cz WHERE DeleteTime IS NULL"
	//{ 0, mysqlSelectLoginQuery, "SELECT RealMoney{ul} FROM user WHERE UserName = [s33] AND Password = [s32]" },
	{ 0, mysqlSelectLoginQuery, "SELECT * FROM user WHERE UserName = [s33] AND Password = [s32]" },

    // Needs completion
    { 3, msqlSelectAuctionDeposit, "SELECT GroupID{uc}, AuctionID{ud}, RoleID{ud}, DepositPrice{l} FROM auction_deposit" },
    { 3, msqlInserAuctionDeposit,  "INSERT INTO auction_deposit (GroupID, AuctionID, RoleID, DepositPrice) VALUES ([uc], [ud], [ud], [l])" },
    { 3, msqlDeleteAuctionDeposit, "DELETE FROM auction_deposit WHERE AuctionID = [ud] AND RoleID = [ud]" },

};
