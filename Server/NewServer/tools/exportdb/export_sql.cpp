#include "stdafx.h"
#include "sql_mgr.h"
#include "define.h"

SQL_STMT_DATA g_SqlStmtList[mysqlTotal] =
{
	{ 0, mysqlSelectAllRole, "SELECT RoleID{l}, RoleName{s24}, UserName{s64}, RoleData{b8192} from role", },
	{ 0, mysqlSelect1DayRole, "SELECT t.RoleID{l}, t.RoleName{s24}, t.UserName{s64}, t.RoleData{b8192} FROM \
							  (SELECT t1.RoleID, t1.RoleName, t1.UserName, t1.RoleData, t2.maxtime FROM role AS t1 \
							  INNER JOIN (SELECT username, MAX(logintime) AS maxtime, createtime FROM role GROUP BY username) t2 \
							  ON t1.username = t2.username AND t2.maxtime = logintime \
							  WHERE t2.maxtime < \"2014-09-26\" AND t2.createtime < \"2014-09-26\") AS t" },
	{ 0, mysqlSelectHighestRole, "SELECT t1.RoleID{l}, t1.RoleName{s24}, t1.UserName{s64}, t1.RoleData{b8192} FROM role AS t1 \
								 INNER JOIN (SELECT roleid, username, rolename, roledata, MAX(logintime) AS maxtime, createtime \
								 FROM role GROUP BY username) t2 ON t1.username = t2.username AND t2.maxtime = logintime" },

    { -1, mysqlInsertRole, "INSERT INTO role (GroupID, RoleID, RoleName, UserName, CreateTime, DeleteTime, LoginTime, RoleDeleted, BaseData, RoleData) VALUES (0, [l], [s24], [s63], now(), 0, now(), 0, [b8192], [b8192])" },

    { 0, mysqlSelectGuild, "SELECT CreateTime{s20}, DeleteTime{s20}, GuildID{l}, Version{d}, GuildName{s24}, GroupID{d}, LeaderID{l}, CreaterName{s24}, UNIX_TIMESTAMP(CreateTime){l}, Announce{s100}, Notice{s200}, MainData{b60000} FROM guild" },
    { 0, mysqlSelectGuildMemberCount , "SELECT count(*){l} from guild_member WHERE GuildID = [l]"},
    { 0, mysqlSelectGuildMemberInfo , "SELECT a1.RoleID{l}, a1.GuildID{l}, a2.GuildName{s24}, a3.RoleName{s24}, a3.UserName{s64} FROM \
                                        guild_member AS a1 INNER JOIN (SELECT GuildID, GuildName FROM guild) a2 INNER JOIN (SELECT \
                                        RoleID, RoleName, UserName FROM role) a3 ON a1.GuildID = a2.GuildID AND a1.RoleID = a3.RoleID" },
};
