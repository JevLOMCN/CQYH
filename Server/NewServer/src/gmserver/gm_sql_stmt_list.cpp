#include "stdafx.h"
#include "gm_sql_stmt_list.h"

SQL_STMT_DATA g_GMMasterSqlStmtList[msqlTotal] =
{
    { 0, GMMasterMysqlSelectRoleData, "SELECT GroupID{uc}, RoleID{ud}, RoleName{s24}, UserName{s64}, LoginTime{s20}, COALESCE(UNIX_TIMESTAMP(DeleteTime), 0){ud}, RoleData{b8192} from role WHERE RoleID = [ud]" },
    { 0, GMMasterMysqlSelectMail, "SELECT MailID{ud}, UNIX_TIMESTAMP(MailTime){ud}, MailType{uc}, IsNew{uc}, AttachLocked{uc}, MailSubject{s60}, AttachType{uc}, Attach{b64} from mail WHERE ReceiverRoleID = [ud]" },
    { 0, GMMasterMysqlUpdateRoleData, "UPDATE role SET BaseData = [b8000], RoleData = [b32768] WHERE RoleID = [ud]" },

    { 1, GMMasterMysqlReplaceGM, "REPLACE INTO gm SET GroupID = [uc], ID = [ul], Time = FROM_UNIXTIME([ud])" },
    { 1, GMMasterMysqlDeleteGM, "DELETE FROM gm WHERE ID = [ul]" },

    { 2, GMMasterMysqlSelectMaxMailID, "SELECT max(MailID){ud} from mail" },
    { 2, GMMasterMysqlInsertMail, "INSERT INTO mail (MailID, MailTime, SenderRoleID, ReceiverRoleID, MailType, MailSubject, MailContent, AttachType, Attach) VALUES ([ud], now(), [ud], [ud], [uc], [s60], [s420], [uc], [b64])" },
       
    { 3, GMMasterMysqlDeleteMail , "DELETE FROM mail WHERE MailID = [ud]"},
};

SQL_STMT_DATA g_GMSlaverStmtList[sSqlTotal] =
{
    { 0, GMMysqlSelectRoleID , "SELECT RoleID{ud} FROM role WHERE RoleName = [s24]"},
    { 0, GMMysqlSelectRoleIDByRoleName, "SELECT RoleID{ud}, RoleName{s24}, UserName{s64} FROM role WHERE RoleName like concat('%', [s24], '%') ORDER BY LENGTH(RoleName)" },
    { 0, GMMysqlSelectRoleIDByUserName, "SELECT RoleID{ud}, RoleName{s24}, UserName{s64} FROM role WHERE UserName = [s64]" },
    { 0, GMMysqlSelectRoleData, "SELECT GroupID{uc}, RoleID{ud}, RoleName{s24}, UserName{s64}, LoginTime{s20}, COALESCE(UNIX_TIMESTAMP(DeleteTime), 0){ud}, RoleData{b8192} from role WHERE RoleID = [ud]" },
    { 0, GMMysqlSelectGuildID , "SELECT GuildID{ud} from guild_member WHERE RoleID = [ud]" },
    { 0, GMMysqlSelectGMForbid, "SELECT ID{ul}, Time{s20} from gm WHERE ID = [ul] or ID = [ul]" },
    { 0, GMMysqlSelectMail, "SELECT MailID{ud}, UNIX_TIMESTAMP(MailTime){ud}, MailType{uc}, IsNew{uc}, AttachLocked{uc}, MailSubject{s60}, AttachType{uc}, Attach{b64} from mail WHERE ReceiverRoleID = [ud]" },
    { 0, GMMysqlSelectFreezeTime, "SELECT UNIX_TIMESTAMP(Time){ud} from gm WHERE ID = [ul]" },
    { 0, GMMysqlSelectRoleIDByName, "SELECT RoleID{ud}, RoleName{s24} FROM role WHERE RoleName = [s24]" },
    { 0, GMMysqlSelectGroup, "SELECT GroupID{uc} FROM role WHERE RoleID = [ud]" },

    // UNIX_TIMESTAMP(ExchangeTime) > [ud] AND
    { 1, GMMysqlSelectExchangeLog, "SELECT GroupID{uc}, UNIX_TIMESTAMP(ExchangeTime){ud}, ExchangeTime{s20}, SellerRoleID{ud}, SellerOrderID{ud}, " \
    "BuyerRoleID{ud}, BuyerOrderID{ud}, Price{d}, Count{d} FROM exchange_log WHERE " \
    " GroupID = [uc] ORDER BY UNIX_TIMESTAMP(ExchangeTime) DESC LIMIT 200 " },

    //UNIX_TIMESTAMP(HistoryTime) > [ud] AND
    { 1, GMMysqlSelectExchangeHistory, "SELECT GroupID{uc}, UNIX_TIMESTAMP(HistoryTime){ud}, HistoryTime{s20}, Open{d}, Close{d}, " \
    "High{d}, Low{d}, Volume{d}, TotalPrice{lld} FROM exchange_history WHERE  " \
    " GroupID = [uc] ORDER BY UNIX_TIMESTAMP(HistoryTime) LIMIT 768" },

    { 1, GMMysqlSelectSysExchangeOrder, "SELECT RoleID{ud}, Count{d} FROM exchange_order WHERE GroupID = [uc] AND UNIX_TIMESTAMP(OrderTime) > [ud] AND UNIX_TIMESTAMP(OrderTime) < [ud]" },
    { 1, GMMysqlSelectSysExchangeLog , "SELECT UNIX_TIMESTAMP(ExchangeTime){ud}, SellerRoleID{ud}, Price{d}, Count{d} FROM exchange_log WHERE GroupID = [uc] AND UNIX_TIMESTAMP(ExchangeTime) > [ud] AND UNIX_TIMESTAMP(ExchangeTime) < [ud]" },
};


SQL_STMT_DATA g_GMLogDataStmtList[lSqlTotal] =
{
    //{ 1, GMLogSelectRoleLogin, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, SndaID{s64}, Action{s10}, IP{s18} FROM logdata_accountlogin WHERE SndaID = [s64] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000"},
    //
    //{ 1, GMLogSelectRoleLogout, "SELECT LogTime{s20}, AreaID{h}, GroupID{uc}, RoleID{ud}, SndaID{s64}, SceneTemplateID{d}, "\
    //"SceneID{d}, Name{s21}, Class{s16}, Sex{s16}, Level{uc}, Experience{d}, Money1{d}, Money2{d}, X{d}, Y{d}, Z{d} "\
    //"FROM logdata_rolelogout_basic WHERE RoleID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },

    //{ 1, GMLogSelectItemByGuid, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, RoleID{ud}, ActionType{s20}, Guid{ul}, "\
    //"TemplateID{ud}, ItemCount{d}, Reason{s30}, UserDefinedID{ul} FROM logdata_item WHERE Guid = [ul] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000"},
    //{ 1, GMLogSelectItemByItemID, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, RoleID{ud}, ActionType{s20}, Guid{ul}, "\
    //"TemplateID{ud}, ItemCount{d}, Reason{s30}, UserDefinedID{ul} FROM logdata_item WHERE TemplateID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },
    //{ 1, GMLogSelectItemByRoleID, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, RoleID{ud}, ActionType{s20}, Guid{ul}, "\
    //"TemplateID{ud}, ItemCount{d}, Reason{s30}, UserDefinedID{ul} FROM logdata_item WHERE RoleID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },
    //
    //{ 1, GMLogSelectMoneyBySourceRole, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, SourceID{ud}, TargetID{ud}, Reason{s32}, "\
    //"Param{c}, Action{s4}, MoneyType{c}, MoneyCount{d} FROM logdata_money WHERE SourceID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },
    //{ 1, GMLogSelectMoneyByTargetRole, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, SourceID{ud}, TargetID{ud}, Reason{s32}, "\
    //"Param{c}, Action{s4}, MoneyType{c}, MoneyCount{d} FROM logdata_money WHERE TargetID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },

    //{ 1, GMLogSelectQuestByRole, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, RoleID{ud}, QuestID{ud}, Action{s8}, Type{s6}, QuestName{s20}"\
    //" FROM logdata_quest WHERE RoleID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },
    //{ 1, GMLogSelectQuestByQuest, "SELECT LogTime{s20}, GroupID{uc}, AreaID{h}, RoleID{ud}, QuestID{ud}, Action{s8}, Type{s6}, QuestName{s20}"\
    //" FROM logdata_quest WHERE QuestID = [ud] AND UNIX_TIMESTAMP(LogTime) > [ud] AND UNIX_TIMESTAMP(LogTime) < [ud] LIMIT 2000" },
};
