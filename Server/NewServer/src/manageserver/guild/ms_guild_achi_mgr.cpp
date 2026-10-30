#include "stdafx.h"
#include "ms_guild_achi_mgr.h"
#include "ms_guild.h"
#include "guild_achievement_cdtn.h"

CMSGuildAchiMgr  CMSGuildAchiMgr::m_Instance;

CMSGuildAchiMgr & CMSGuildAchiMgr::GetInstance()
{
    return m_Instance;
}

CMSGuildAchiMgr::CMSGuildAchiMgr()
{

}

CMSGuildAchiMgr::~CMSGuildAchiMgr()
{
}

BOOL CMSGuildAchiMgr::Init()
{
    for (INT32 i = GUILD_ACHI_BEGIN; i < GUILD_ACHI_TYPE_NUM; ++i)
    {
        m_con[i].Clear();
    }

    return TRUE;
}

BOOL CMSGuildAchiMgr::UnInit()
{
    INT32   nRetCode = 0;

    nRetCode = Clear();
    LOG_CHECK_ERROR(nRetCode);

    for (INT32 i = GUILD_ACHI_BEGIN; i < GUILD_ACHI_TYPE_NUM; ++i)
    {
        m_con[i].Clear();
    }

    return nRetCode;
}

BOOL CMSGuildAchiMgr::AddAchiType(INT32 nType, INT32 nParam1, INT32 nAchiID)
{
    INT32           nCheckCount = 0;
    BASE_LIST_NODE    *pNext = NULL;
    ACHI_CONDITION    *pData = NULL;
    LOG_PROCESS_ERROR(nType < GUILD_ACHI_TYPE_NUM);

    pNext = &m_con[nType].Head();
    while (pNext->pNext != &m_con[nType].Rear())
    {
        pData = (ACHI_CONDITION *)pNext->pNext;
        PROCESS_SUCCESS(pData->nParam1 == nParam1 && pData->nAchievementID == nAchiID);

        pNext = pNext->pNext;
        ++nCheckCount;
    }

    LOG_PROCESS_ERROR(nCheckCount < 8);     // char,

    pData = new ACHI_CONDITION;
    LOG_PROCESS_ERROR(pData);

    ClearNode(pData);
    m_con[nType].PushRear(pData);

    pData->nConditionType = nType;
    pData->nParam1 = nParam1;
    pData->nAchievementID = nAchiID;

Exit1:
    return TRUE;
Exit0:
    return FALSE;
}

//BOOL CMSGuildAchiMgr::DelAchiType(INT32 nType, INT32 nParam1, INT32 nAchiID)
//{
//    BASE_LIST_NODE    *pNext = NULL;
//    ACHI_CONDITION    *pData = NULL;
//    LOG_PROCESS_ERROR(nType < GUILD_ACHI_TYPE_NUM);
//
//    pNext = &m_con[nType].Head();
//    while (pNext->pNext != &m_con[nType].Rear())
//    {
//        pData = (ACHI_CONDITION *)pNext->pNext;
//        if (pData->nParam1 == nParam1 && pData->nAchievementID == nAchiID)
//        {
//            m_con[nType].Remove(pData);
//            break;
//        }
//
//        pNext = pNext->pNext;
//    }
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}

BOOL CMSGuildAchiMgr::Clear()
{
    BASE_LIST_NODE  *pNode = NULL;
    BASE_LIST_NODE  *pNext = NULL;
    for (INT32 i = GUILD_ACHI_BEGIN; i < GUILD_ACHI_TYPE_NUM; ++i)
    {
        pNext = m_con[i].Head().pNext;
        while (pNext != &m_con[i].Rear())
        {
            pNode = pNext;
            pNext = pNode->pNext;

            delete ((ACHI_CONDITION*)pNode);
        }
    }

    return TRUE;
}

void CMSGuildAchiMgr::RecordAchiCondition(INT32 nType, CMsGuild *poMsGuild)
{
    INT32           nRetCode = 0;
    BASE_LIST_NODE  *pNode = NULL;
    ACHI_CONDITION    *pData = NULL;
    INT32           nParam1 = 0;
    const GUILD_ACHI_CDTN * pTemplate = NULL;

    LOG_PROCESS_ERROR(poMsGuild);

    switch (nType)
    {
    case GUILD_ACHI_MEMBER:
    {
        nParam1 = poMsGuild->GetMemberNum();
    }
        break;
    case GUILD_ACHI_ENEMY:
    {
        nParam1 = poMsGuild->GetEnemyNum();
    }
        break;
    case GUILD_ACHI_ALLIANCE:
    {
        nParam1 = poMsGuild->GetAllianceNum();
    }
        break;
    default:
        break;
    }

    pNode = m_con[nType].Head().pNext;
    while (pNode != &m_con[nType].Rear())
    {
        pData = (ACHI_CONDITION*)pNode;

        if (pData->nParam1 == nParam1)
        {
            pTemplate = GetGuildAchiCdtn(nType, pData->nParam1);
            LOG_PROCESS_ERROR(pTemplate);

            poMsGuild->SetAchievementCondition(nType, pTemplate->nPos);
        }

        pNode = pNode->pNext;
    }

Exit0:
    return;
}
