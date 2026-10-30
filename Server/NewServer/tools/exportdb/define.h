#ifndef _H_DEFINE_H_
#define _H_DEFINE_H_


enum MS_SQL_STMT_ID
{
    mysqlSelectAllRole,
    mysqlSelect1DayRole,
    mysqlSelectHighestRole,

    //mysqlSelectGuildStorage,

    mysqlInsertRole,

    mysqlSelectGuild,
    mysqlSelectGuildMemberCount,
    mysqlSelectGuildMemberInfo,

    mysqlTotal
};





struct SQL_STMT_DATA;
extern SQL_STMT_DATA g_SqlStmtList[mysqlTotal];

#endif //_H_DEFINE_H_
