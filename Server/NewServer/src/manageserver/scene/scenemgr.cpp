#include "stdafx.h"
#include "scenemgr.h"

#include "server_config.h"

#include "message_handler.h"
#include "scene_data.h"
#include "role_data_mgr.h"
#include "team/team.h"

CSceneMgr CSceneMgr::ms_Instance;

CSceneMgr::CSceneMgr()
{

}

CSceneMgr:: ~CSceneMgr()
{
    UnInit();
}

struct CTraverseScene
{
	UINT32 dwRecord;
	BOOL operator()(UINT64 dwSceneKey, SCENE_INFO* poSceneInfo)
	{
		INT32 nRetCode;
		PROCESS_SUCCESS(dwSceneKey % MS_CLEAN_SCENE_FRE != dwRecord);
		PROCESS_SUCCESS(poSceneInfo->eState != ESCENESTATE_NORMAL && poSceneInfo->eState != ESCENESTATE_CLOSING);
		
		if(poSceneInfo->nRoleCount > 0)
		{
//			poSceneInfo->bDelete = FALSE;
		}
		else if(poSceneInfo->bDelete)
		{
			poSceneInfo->eState = ESCENESTATE_DESTROYING;

			nRetCode = CMessageHandler::GetInstance().DoGSDestroyScene(poSceneInfo->dwGSSessionID, 
				poSceneInfo->nServerGroupID, poSceneInfo->nSceneTempID, poSceneInfo->nSceneID);
			LOG_PROCESS_ERROR(nRetCode);

			//INF("Do DoGSDestroyScene %d, %d, %d", poSceneInfo->dwGSSessionID, poSceneInfo->nSceneTempID, poSceneInfo->nSceneID);
		}
		else
		{
			switch(poSceneInfo->nSceneType)
			{
			case estDungeon:
			case estBattleField:
			case estArena:
			case estBirth:
			case estGuild:
			case estGuildDungeon:
			case estSoloDungeon:
				poSceneInfo->bDelete = TRUE;
				break;
			case estLine:
				{
					const SCENE_DATA* pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(poSceneInfo->nSceneTempID);
					LOG_PROCESS_ERROR(pstSceneData);

					if ((poSceneInfo->nSceneID & 0x00FFFFFF) > pstSceneData->nInitCount)
						poSceneInfo->bDelete = TRUE;
					else
						poSceneInfo->bDelete = FALSE;
				}
				break;
			case estUnique:
			case estStatic:
//				poSceneInfo->bDelete = FALSE;
				break;
			default:
				LOG_PROCESS_ERROR(FALSE);
			}
		}

Exit1:
		return TRUE;
Exit0:
		return FALSE;
	}
};

BOOL SceneMgrTimerTrig(INT32 nParam1, INT32 nParam2, INT32 nParam3, const CHAR* szParam4)
{
	INT32 nRetCode = 0;
	static UINT32 s_dwSceneCleanRecord = 0;

	CTraverseScene stTraverseScene;
	stTraverseScene.dwRecord = s_dwSceneCleanRecord++ % MS_CLEAN_SCENE_FRE;

	nRetCode = CSceneMgr::GetInstance().TraverseScene(stTraverseScene);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CSceneMgr::Init(void)
{
    INT32 nRetCode = 0;

    nRetCode = m_SceneInfoPool.Init(g_ServerConfig.MS.nInitSceneCount);
    LOG_PROCESS_ERROR(nRetCode);
    
    for (INT32 nIndex = 0; nIndex < MAX_SCENE_TEMPLATE_COUNT; nIndex++)
	{
        m_nDungeonIDGenerator[nIndex] = 1;
		m_lstSceneSameTempID[nIndex].Clear();
	}

	for (INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
	{
		for(INT32 j = 0; j < MAX_SCENE_TEMPLATE_COUNT; ++j)
			m_lstLineSceneIDInfo[i][j].clear();
	}

	memset(m_nSceneTempRoleCount, 0, sizeof(m_nSceneTempRoleCount));
	
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CSceneMgr::UnInit(void)
{
    INT32 nRetCode = 0;

    nRetCode = m_SceneInfoPool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

CSceneMgr& CSceneMgr::GetInstance(void)
{
    return ms_Instance;
}

UINT32 CSceneMgr::_GeneratorSceneID(INT32 nServerGroupID, INT32 nSceneTempID)
{
    UINT32 nSceneID = 0;
    const SCENE_DATA* pstSceneData;
    
    pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
    LOG_PROCESS_ERROR(pstSceneData);

    switch(pstSceneData->nType)
    {
    case estStatic:
        nSceneID = (nServerGroupID << 24) | 1;
        break;
    case estLine:
		{
			CLineSceneIDInfo::reverse_iterator itr = m_lstLineSceneIDInfo[nServerGroupID][nSceneTempID].rbegin();
			if (itr != m_lstLineSceneIDInfo[nServerGroupID][nSceneTempID].rend())
				nSceneID = (*itr) + 1;
			else
				nSceneID = 1;

			nSceneID = (nServerGroupID << 24) | nSceneID;
			m_lstLineSceneIDInfo[nServerGroupID][nSceneTempID].push_back(nSceneID);
		}
        break;
    case estDungeon:
	case estBattleField:
	case estArena:
	case estSoloDungeon:
        nSceneID = m_nDungeonIDGenerator[nSceneTempID]++;
        break;
	case estUnique:
		nSceneID = 1;
	case estGuild:
	case estBirth:
	case estGuildDungeon:
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }
    
Exit0:
    return nSceneID;    
}

SCENE_INFO* CSceneMgr::CreateScene(INT32 nSceneTempID, INT32 nServerGroupID, UINT32 dwSceneID)
{
    SCENE_INFO* pResult = NULL;
	INT32 nRetCode = 0;
    const SCENE_DATA* pstSceneData = NULL;
    UINT64 qwKey = 0;
    LOG_PROCESS_ERROR(nSceneTempID > 0 && nSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
    
	if(dwSceneID == 0)
		dwSceneID = _GeneratorSceneID(nServerGroupID, nSceneTempID);
	qwKey = ((UINT64)nSceneTempID << 32) | dwSceneID;

	pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
	LOG_PROCESS_ERROR(pstSceneData);
	
    pResult = m_SceneInfoPool.New(qwKey);
    LOG_PROCESS_ERROR(pResult);

	pResult->nServerGroupID = nServerGroupID;
    pResult->nSceneTempID = nSceneTempID;
    pResult->nSceneID = dwSceneID;
    pResult->bDelete = FALSE;
    pResult->nSceneType = pstSceneData->nType;
	pResult->nSideCount = pstSceneData->nSideCount;
	pResult->pPrev = pResult->pNext = NULL;
	pResult->nRoleCount = 0;
	pResult->lstOnlineRole.Clear();
	pResult->WaitUserList.clear();

	for(INT32 i = 0; i < MAX_SIDE_COUNT; ++i)
		pResult->nSideCurRoleCount[i] = 0;

	nRetCode = m_lstSceneSameTempID[nSceneTempID].PushRear(pResult);
	LOG_PROCESS_ERROR(nRetCode);
	
	INF("create scene %d-%X in group %d", nSceneTempID, dwSceneID, nServerGroupID);
    return pResult;
Exit0:
    if (pResult)
        m_SceneInfoPool.Delete(pResult);

    return NULL;
}

BOOL CSceneMgr::DestroyScene(INT32 nSceneTempID, INT32 nSceneID, INT32 nServerGroupID)
{
    INT32 nRetcode = 0;
	SCENE_INFO* pSceneInfo = NULL;
    const SCENE_DATA* pstSceneData = NULL;
	UINT64 qwKey = 0;
	
    pstSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
    LOG_PROCESS_ERROR(pstSceneData);

	qwKey = ((UINT64)nSceneTempID << 32) | nSceneID;
	pSceneInfo = m_SceneInfoPool.Find(qwKey);
	LOG_PROCESS_ERROR(pSceneInfo);
	LOG_PROCESS_ERROR (pSceneInfo->lstOnlineRole.Head().pNext == &pSceneInfo->lstOnlineRole.Rear());

	LOG_PROCESS_ERROR(pSceneInfo->pPrev != NULL && pSceneInfo->pNext != NULL );
	nRetcode = m_lstSceneSameTempID[nSceneTempID].Remove(pSceneInfo);
	LOG_CHECK_ERROR(nRetcode);

	m_SceneInfoPool.Delete(pSceneInfo);

    if(pstSceneData->nType == estLine)
		m_lstLineSceneIDInfo[nServerGroupID][nSceneTempID].remove(nSceneID);    

    INF("destroy scene %d-%X in group %d", nSceneTempID, nSceneID, nServerGroupID);

    return TRUE;
Exit0:
    return FALSE;
}

SCENE_INFO* CSceneMgr::FindScene(INT32 nSceneTempID, INT32 nSceneID)
{
	UINT64 qwKey = (UINT64)nSceneTempID << 32 | nSceneID ;

    return m_SceneInfoPool.Find(qwKey);
}

UINT16 CSceneMgr::GetLineInfo(INT32 nServerGroup, UINT32 dwSceneTempID, UINT16 wMaxCount, UINT32* plstIDBuffer)
{
	INT16 nLineSceneIDCount = 0;

	LOG_PROCESS_ERROR(nServerGroup >= 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(dwSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
	LOG_PROCESS_ERROR(plstIDBuffer);

	for (CLineSceneIDInfoItr itr = m_lstLineSceneIDInfo[nServerGroup][dwSceneTempID].begin();
		itr != m_lstLineSceneIDInfo[nServerGroup][dwSceneTempID].end(); ++itr)
	{
		if(nLineSceneIDCount >= wMaxCount)
			break;

		plstIDBuffer[nLineSceneIDCount++] = (nServerGroup << 24) | *itr;
	}
	return nLineSceneIDCount;

Exit0:
	return 0;
}

BOOL CSceneMgr::CCleanSceneByGSSession::operator()(UINT64 dwSceneKey, SCENE_INFO* poSceneInfo)
{
    if(poSceneInfo->dwGSSessionID == dwGSSessionID)
        cleanSceneList.push_back(poSceneInfo);

    return TRUE;
}

BOOL CSceneMgr::CleanSceneByGSsessionID(UINT32 dwGSsessionID)
{
    INT32 nRetCode = 0;
    
    CCleanSceneByGSSession stCleanScene;
    stCleanScene.dwGSSessionID = dwGSsessionID;
    stCleanScene.cleanSceneList.clear();

    nRetCode = m_SceneInfoPool.Traverse(stCleanScene);
    LOG_PROCESS_ERROR(nRetCode);

    for(CCleanSceneListItr itr = stCleanScene.cleanSceneList.begin(); itr != stCleanScene.cleanSceneList.end(); ++itr)
    {
        SCENE_INFO* poSceneInfo = *itr;
        if(poSceneInfo)
        {
            nRetCode = DestroyScene(poSceneInfo->nSceneTempID, poSceneInfo->nSceneID, poSceneInfo->nServerGroupID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

UINT32 CSceneMgr::GetSceneCount(void)
{
    return m_SceneInfoPool.GetUsedSize();
}
