#include "stdafx.h"
#include "gs_info_mgr.h"

#include "server_config.h"
#include "scene_data.h"

CGSInfoMgr CGSInfoMgr::ms_Instance;

CGSInfoMgr::CGSInfoMgr()
{
    memset(&m_GSInfoList, 0, sizeof(m_GSInfoList));
}

CGSInfoMgr::~CGSInfoMgr()
{
    UnInit();
}

BOOL CGSInfoMgr::Init(void)
{
    INT32 nRetCode = 0;

    memset(m_GSInfoList, 0, sizeof(m_GSInfoList));

    CTraverseSceneData TraverseSceneData;
    TraverseSceneData.m_nGSIndex = 0;
    TraverseSceneData.m_pGSInfoList = m_GSInfoList;

	LOG_PROCESS_ERROR(g_ServerConfig.MS.nInitGSCount + 1 < MAX_GS_SESSION_COUNT);
    nRetCode = CDataManager<SCENE_DATA>::GetInstance().Traverse(TraverseSceneData);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSInfoMgr::UnInit(void)
{
    return TRUE;
}

CGSInfoMgr& CGSInfoMgr::GetInstance(void)
{
    return ms_Instance;
}

GS_INFO* CGSInfoMgr::NewGSInfo( UINT32 dwGSSessionID )
{
    LOG_PROCESS_ERROR( dwGSSessionID > 0 );

    for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
    {
        if(m_GSInfoList[i].bConnected == TRUE && m_GSInfoList[i].dwInitGSSessionID == dwGSSessionID)
        {
            CRI("CGSInfoMgr::NewGSInfo index=%d's dwGSSessionID %d match, but has bConnected", i, dwGSSessionID);
            continue;
        }

        if(m_GSInfoList[i].bConnected == FALSE && m_GSInfoList[i].dwInitGSSessionID == 0)
        {
            m_GSInfoList[i].bConnected = TRUE;
            m_GSInfoList[i].dwInitGSSessionID = dwGSSessionID;

            return &m_GSInfoList[i];
        }
    }

Exit0:
    return NULL;
}

BOOL CGSInfoMgr::DelGSInfo(UINT32 dwGSSessionID)
{
    LOG_PROCESS_ERROR( dwGSSessionID > 0 );

    for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
    {
        if(m_GSInfoList[i].bConnected == FALSE && m_GSInfoList[i].dwInitGSSessionID == dwGSSessionID)
        {
            CRI("CGSInfoMgr::DelGSInfo index=%d's dwGSSessionID %d match, but not bConnected", i, dwGSSessionID);
            continue;
        }

        if(m_GSInfoList[i].bConnected == TRUE && m_GSInfoList[i].dwInitGSSessionID == dwGSSessionID)
        {
            m_GSInfoList[i].bConnected = FALSE;
            m_GSInfoList[i].dwInitGSSessionID = 0;
            break;
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGSInfoMgr::CTraverseSceneData::_AssignGS(INT32 nTargetGSIndex, const SCENE_DATA* pSceneData, INT32 nGroupID)
{
	LOG_PROCESS_ERROR(pSceneData);
	LOG_PROCESS_ERROR(nTargetGSIndex > 0 && nTargetGSIndex <= g_ServerConfig.MS.nInitGSCount);

	LOG_PROCESS_ERROR(nTargetGSIndex < MAX_GS_SESSION_COUNT);
	LOG_PROCESS_ERROR(m_pGSInfoList[nTargetGSIndex].nInitSceneCount < MAX_INIT_SCENE_PER_GS &&
		m_pGSInfoList[nTargetGSIndex].nInitSceneCount >= 0);

	m_pGSInfoList[nTargetGSIndex].nInitSceneList[m_pGSInfoList[nTargetGSIndex].nInitSceneCount].nServerGroupID = nGroupID;
	m_pGSInfoList[nTargetGSIndex].nInitSceneList[m_pGSInfoList[nTargetGSIndex].nInitSceneCount].nSceneTempID = pSceneData->nSceneID;
	m_pGSInfoList[nTargetGSIndex].nInitSceneCount++;
	m_pGSInfoList[nTargetGSIndex].nInitMaxSceneRoleCount += pSceneData->nMaxRoleCount;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGSInfoMgr::CTraverseSceneData::AssignGSInTurn(const SCENE_DATA* pSceneData, INT32 nGroupID)
{
	BOOL bRetCode = FALSE;
	INT32 nTargetGSIndex = m_nGSIndex + 1;

	bRetCode = _AssignGS(nTargetGSIndex, pSceneData, nGroupID);
	LOG_PROCESS_ERROR(bRetCode);

	m_nGSIndex++;
	m_nGSIndex %= g_ServerConfig.MS.nInitGSCount;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGSInfoMgr::CTraverseSceneData::AssignGSByInitMaxRoleCount(const SCENE_DATA* pSceneData, INT32 nGroupID)
{
	BOOL bRetCode = FALSE;
	INT32 nTargetGSIndex = -1;
	INT32 nMininumInitMaxRoleCount = INT_MAX;

	for (INT32 i = 1; i <= g_ServerConfig.MS.nInitGSCount; ++i)
	{
		if (nMininumInitMaxRoleCount > m_pGSInfoList[i].nInitMaxSceneRoleCount)
		{
			nMininumInitMaxRoleCount = m_pGSInfoList[i].nInitMaxSceneRoleCount;
			nTargetGSIndex = i;
		}
	}

	bRetCode = _AssignGS(nTargetGSIndex, pSceneData, nGroupID);
	LOG_PROCESS_ERROR(bRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CGSInfoMgr::CTraverseSceneData::operator()(const SCENE_DATA* pSceneData)
{
    if (pSceneData->nSceneID == 0)
        return TRUE;

    switch(pSceneData->nType)
    {
    case estStatic:
		LOG_CHECK_ERROR(pSceneData->nInitCount <= 1);
		if (pSceneData->nInitCount == 1)
		{
			for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
			{
				if (IsValidGroup(i))
				{
					AssignGSByInitMaxRoleCount(pSceneData, i);
				}
			}
        }
        break;
	case estUnique:
		AssignGSByInitMaxRoleCount(pSceneData, MAX_SERVER_GROUP_COUNT);
		break;
	case estLine:
        {
            for(INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
            {
                if(IsValidGroup(i))
                {
                    for(INT32 j = 0; j < pSceneData->nInitCount; ++j)
                    {
						AssignGSByInitMaxRoleCount(pSceneData, i);
                    }
                }
            }
        }
        break;
    case estDungeon:
    case estBattleField:
    case estArena:
	case estBirth:
    case estGuild:
	case estGuildDungeon:
	case estSoloDungeon:
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return TRUE;
Exit0:
    return FALSE;
}