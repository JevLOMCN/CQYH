#include "stdafx.h"
#include "pre_save_list.h"
#include "./guild/ms_guild_mgr.h"
#include "ms_sql_stmt_list.h"
#include "manageserver.h"

CPreSaveList CPreSaveList::m_Instance;

CPreSaveList& CPreSaveList::GetInstance()
{
    return m_Instance;
}

CPreSaveList::CPreSaveList()
{

}

CPreSaveList::~CPreSaveList()
{

}

BOOL CPreSaveList::Init()
{
    INT32       nRetCode = 0;

    m_set.clear();

    nRetCode = m_queue.Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_IDPool.Init(10000);
    LOG_PROCESS_ERROR(nRetCode);

    m_pBuffer = new char[SAVE_BUFFER_SIZE];
    LOG_PROCESS_ERROR(m_pBuffer);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CPreSaveList::UnInit()
{
    INT32       nRetCode = 0;
    INT32       nIndex = 0;

    // save all
    while (1)
    {
        ID_NODE *pNode = m_queue.PopFront();
        if (pNode == NULL)
            break;
        if (nIndex == MAX_MS_SAVE_COMMIT_NUM)       // 
            Sleep(8);

        ++nIndex;
        // do node logic
        nRetCode = _Save(pNode->nType, pNode->dwID);
        LOG_CHECK_ERROR(nRetCode);
        if (FALSE == nRetCode)
        {
            _SaveToList(pNode);
            continue;
        }

        nRetCode = m_IDPool.Delete(pNode);
        LOG_CHECK_ERROR(nRetCode);
        m_set.erase((((UINT64)pNode->nType) << 32) + pNode->dwID);
    }


    delete[] m_pBuffer;
    m_pBuffer = NULL;

    m_set.clear();
    nRetCode = m_queue.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    nRetCode = m_IDPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

BOOL CPreSaveList::MainLoop()
{
    INT32       nRetCode = 0;

    for (INT32 i = 0; i < MAX_MS_SAVE_COMMIT_NUM; ++i)
    {
        ID_NODE *pNode = m_queue.PopFront();
        if (pNode == NULL)
            break;

        // do node logic
        nRetCode = _Save(pNode->nType, pNode->dwID);
        LOG_CHECK_ERROR(nRetCode);
        if (FALSE == nRetCode)
        {
            _SaveToList(pNode);
            continue;                
        }

        nRetCode = m_IDPool.Delete(pNode);
        LOG_CHECK_ERROR(nRetCode);
        m_set.erase((((UINT64)pNode->nType) << 32) + pNode->dwID);
    }

    return TRUE;
} 

BOOL CPreSaveList::SaveToList(INT32 nType, UINT32 dwObjID)
{
    ID_NODE     *pNode = NULL;
    ID_SET::iterator iter = m_set.find((((UINT64)nType) << 32) + dwObjID);
    PROCESS_SUCCESS(iter != m_set.end());

    pNode = m_IDPool.New();
    LOG_PROCESS_ERROR(pNode);

    pNode->dwID = dwObjID;
    pNode->nType = nType;

    m_queue.PushBack(pNode);
    m_set.insert((((UINT64)nType) << 32) + dwObjID);

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CPreSaveList::_SaveToList(ID_NODE *pNode)
{
    LOG_PROCESS_ERROR(pNode);

    m_queue.PushBack(pNode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CPreSaveList::_Save(INT32 nType, UINT32 dwID)
{
    INT32       nRetCode = 0;
    UINT32      dwSize = 0;
    LOG_PROCESS_ERROR(m_pBuffer);

    switch (nType)
    {
    case PRE_SAVE_GUILD_MAIN:
    {
        CMsGuild    *poMSGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwID);
        if (poMSGuild)
        {
            memset(m_pBuffer, 0, SAVE_BUFFER_SIZE);
            nRetCode = poMSGuild->SaveGuild(m_pBuffer, dwSize);
            LOG_PROCESS_ERROR(nRetCode);

            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateGuild, 
                NULL, NULL, "zud", m_pBuffer, dwSize, dwID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
        break;
    case PRE_SAVE_GUILD_NOTICE:
    {
        CMsGuild    *poMSGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwID);
        if (poMSGuild)
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateGuildNotice, 
                NULL, NULL, "sud", poMSGuild->GetNotice(), dwID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
        break;
    case PRE_SAVE_GUILD_LEADER:
    {
        CMsGuild    *poMSGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwID);
        if (poMSGuild)
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateGuildLeader, 
                NULL, NULL, "udud", poMSGuild->GetLeaderID(), dwID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
        break;
    case PRE_SAVE_GUILD_ANNOUNCE:
    {
        CMsGuild    *poMSGuild = CMsGuildMgr::GetInstance().FindGuildByID(dwID);
        if (poMSGuild)
        {
            nRetCode = CManageServer::GetInstance().GetSqlMgr().AsyncExecSqlStmt(mysqlUpdateGuildAnnounce,
                NULL, NULL, "sud", poMSGuild->GetAnnounce(), dwID);
            LOG_PROCESS_ERROR(nRetCode);
        }
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
    }

    return TRUE;
Exit0:
    return FALSE;
}
