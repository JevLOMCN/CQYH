#include "stdafx.h"
#include "ms_guild_member.h"
#include "ms_guild_mgr.h"
#include "manageserver.h"
#include "onlinerole/online_role_data_mgr.h"
#include "message_handler.h"
#include "db/ms_sql_stmt_list.h"
#include "sql_mgr.h"

CMsGuildMember::CMsGuildMember() : 
m_dwGuildID(INVALID_OBJECT_ID),
m_dwRoleID(INVALID_OBJECT_ID),
m_byPosition(0),
m_byGuildFlag(0),
m_nSalary(0),
m_dwDrawSalaryTime(0)
{

}

CMsGuildMember::~CMsGuildMember()
{

}

BOOL CMsGuildMember::Init(UINT32 dwGuildID, UINT32 dwRoleID, UINT8 byPosition, UINT8 byFlag, INT32 nSalary, INT32 dwDrawSalaryTime, INT32 nPow)
{
    m_dwGuildID = dwGuildID;
    m_dwRoleID = dwRoleID;
    m_byPosition = byPosition;
    m_byGuildFlag = byFlag;
    m_nSalary = nSalary;
    m_dwDrawSalaryTime = dwDrawSalaryTime;
    m_nPow = nPow;

    return TRUE;
}

BOOL CMsGuildMember::UnInit()
{
    m_dwGuildID = 0;
    m_dwRoleID = 0;
    m_byPosition = 0;
    m_byGuildFlag = 0;
    m_nSalary = 0;
    m_dwDrawSalaryTime = 0;
    m_nPow = 0;

    return TRUE;
}

BOOL CMsGuildMember::SetPosition(UINT8 byPosition, BOOL isNtf /*= TRUE*/)
{
    INT32   nRetCode = 0;

    LOG_PROCESS_ERROR(byPosition <= GUILD_POSITION_TOTAL);
    m_byPosition = byPosition;

    nRetCode = _Save();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = CMessageHandler::GetInstance().DoGSGuildChangePosition(m_dwGuildID, m_dwRoleID, byPosition);
    LOG_CHECK_ERROR(nRetCode);

    if (isNtf)
    {
        CMsGuild    *poGuild = CMsGuildMgr::GetInstance().FindGuildByID(m_dwGuildID);
        if (poGuild)
        {
            nRetCode = CMessageHandler::GetInstance().DoClientGuildChangePositionNtf(poGuild, m_dwRoleID, byPosition);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMember::SetFlag(UINT8 byFlag)
{
    INT32   nRetCode = 0;

    m_byGuildFlag = byFlag;

    nRetCode = _Save();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMember::AddSalary(INT32 nSalary, BOOL isSave)
{
    INT32   nRetCode = 0;
    MS_ONLINE_ROLE_DATA     *pOnlineRoleData = NULL;

    LOG_PROCESS_ERROR(nSalary > 0);

    m_nSalary += nSalary;

    if (m_nSalary > MAX_MONEY)
    {
        m_nSalary = MAX_MONEY;
    }

    if (isSave)
    {
        nRetCode = _Save();
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoGSGuildSyncMemberSalaryNtf(m_dwGuildID, m_dwRoleID, m_nSalary);
    LOG_PROCESS_ERROR(nRetCode);

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(m_dwRoleID);
    if (pOnlineRoleData)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildMemberSalaryChange(pOnlineRoleData->dwCliSessionID, GetSalary());
        LOG_CHECK_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMember::SubSalary(INT32 nSalary, BOOL isSave)
{
    INT32   nRetCode = 0;
    MS_ONLINE_ROLE_DATA     *pOnlineRoleData = NULL;

    LOG_PROCESS_ERROR(nSalary > 0);
    LOG_PROCESS_ERROR(m_nSalary >= nSalary);

    m_nSalary -= nSalary;

    if (isSave)
    {
        nRetCode = _Save();
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CMessageHandler::GetInstance().DoGSGuildSyncMemberSalaryNtf(m_dwGuildID, m_dwRoleID, m_nSalary);
    LOG_PROCESS_ERROR(nRetCode);

    pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(m_dwRoleID);
    if (pOnlineRoleData)
    {
        nRetCode = CMessageHandler::GetInstance().DoClientGuildMemberSalaryChange(pOnlineRoleData->dwCliSessionID, GetSalary());
        LOG_CHECK_ERROR(nRetCode);
    }


    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMsGuildMember::SetDrawSalaryTime(UINT32 dwTime)
{
    m_dwDrawSalaryTime = dwTime;
    return TRUE;
}

BOOL CMsGuildMember::_Save()
{
    return CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateGMember, NULL, NULL, "ucucduddud", m_byPosition, 
        m_byGuildFlag, m_nSalary, m_dwDrawSalaryTime, m_nPow, m_dwRoleID);
}

// pow
BOOL CMsGuildMember::SavePow()
{
    INT32   nRetCode = 0;

    nRetCode = _Save();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}
