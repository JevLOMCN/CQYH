#include "stdafx.h"
#include "cash_accounting_stmt.list.h"

SQL_STMT_DATA g_CASqlStmtList[caSqlTotal] =
{
	{0, caSqlSelectLogoutRole, "SELECT UNIX_TIMESTAMP(LogTime{l}), RoleID{l}, Money1{d}, Money2{d}, Name{s21} FROM logdata_rolelogout_basic"},
	{0, caSqlSelectRoleMoney,  "SELECT SourceID{l}, TargetID{l}, MoneyType{c}, MoneyCount{d} FROM logdata_money WHERE (SourceID = [l] or TargetID = [l]) AND (LogTime >= FROM_UNIXTIME([l]) AND LogTime <= FROM_UNIXTIME([l]))"},
};
