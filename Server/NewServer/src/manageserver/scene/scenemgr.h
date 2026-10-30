#ifndef _SCENE_MGR_H_
#define _SCENE_MGR_H_

#include "base_list.h"
#include "id_object_pool.h"

#include "string_def.h"
#include "scene_def.h"
#include "server_def.h"

#include <list>
#include <map>

#define MS_CLEAN_SCENE_FRE     5 

enum ESceneState
{
    ESCENESTATE_NOTEXIST = 1,
    ESCENESTATE_CREATING,
    ESCENESTATE_NORMAL,
	ESCENESTATE_CLOSING,
    ESCENESTATE_DESTROYING,
};

struct MS_ROLE_DATA;

struct SCENE_INFO : public BASE_LIST_NODE
{
    INT32                       nSceneTempID;
    INT32                       nSceneID;
	INT32						nServerGroupID;
    INT32                       nSceneType;
    UINT32                      dwGSSessionID;
    UINT32                      dwCreateTime;
    INT32                       nRoleCount;
	CBaseList					lstOnlineRole;			

	INT32						nSideCount;
	INT32						nSideCurRoleCount[MAX_SIDE_COUNT];

    INT32						eState;
    std::list<UINT32>           WaitUserList;

    BOOL                        bDelete;
};

extern BOOL SceneMgrTimerTrig(INT32 nParam1, INT32 nParam2, INT32 nParam3, const CHAR* szParam4);

class CSceneMgr
{
public:
    CSceneMgr();
    ~CSceneMgr();

    BOOL Init(void);
    BOOL UnInit(void);
    static CSceneMgr& GetInstance(void);
    
	SCENE_INFO* CreateScene(INT32 nSceneTempID, INT32 nServerGroupID, UINT32 dwSceneID);
	BOOL DestroyScene(INT32 nSceneTempID, INT32 nSceneID, INT32 nServerGroupID);
	SCENE_INFO* FindScene(INT32 nSceneTempID, INT32 nSceneID);

    BOOL CleanSceneByGSsessionID(UINT32 dwGSsessionID);
	UINT16 GetLineInfo(INT32 nServerGroup, UINT32 dwSceneTempID, UINT16 wMaxCount, UINT32* plstIDBuffer);

    UINT32 GetSceneCount(void);

    template <class TFunc>
    inline BOOL TraverseScene(TFunc& rFunc);

	template <class TFunc>
	inline BOOL TraverseSceneBySceneTempID(UINT32 dwSceneTempID, TFunc& rFunc);

	inline void AddSceneTempRoleCount(INT32 nServerGroup, INT32 nSceneTempID);
	inline void SubSceneTempRoleCount(INT32 nServerGroup, INT32 nSceneTempID);
	inline INT32 GetSceneTempRoleCount(INT32 nServerGroup, INT32 nSceneTempID);

private:
    UINT32 _GeneratorSceneID(INT32 nServerGroupID, INT32 nSceneTempID);
private:
    static CSceneMgr ms_Instance;

    CIDObjectPool<SCENE_INFO, UINT64>   m_SceneInfoPool;
    INT32                               m_nDungeonIDGenerator[MAX_SCENE_TEMPLATE_COUNT];
	
	typedef std::list<UINT32>			CLineSceneIDInfo;
	typedef CLineSceneIDInfo::iterator	CLineSceneIDInfoItr;
	CLineSceneIDInfo					m_lstLineSceneIDInfo[MAX_SERVER_GROUP_COUNT][MAX_SCENE_TEMPLATE_COUNT];
	
	CBaseList							m_lstSceneSameTempID[MAX_SCENE_TEMPLATE_COUNT];
	INT32								m_nSceneTempRoleCount[MAX_SERVER_GROUP_COUNT][MAX_SCENE_TEMPLATE_COUNT];

    typedef std::list<SCENE_INFO*> CCleanSceneList;
    typedef CCleanSceneList::iterator CCleanSceneListItr;

    struct CCleanSceneByGSSession
    {
        UINT32 dwGSSessionID;
        CCleanSceneList cleanSceneList; 
        BOOL operator()(UINT64 dwSceneKey, SCENE_INFO* poSceneInfo);
    };
};


template <class TFunc>
inline BOOL CSceneMgr::TraverseScene(TFunc& rFunc)
{
    return m_SceneInfoPool.Traverse(rFunc);
}

template <class TFunc>
inline BOOL CSceneMgr::TraverseSceneBySceneTempID(UINT32 dwSceneTempID, TFunc& rFunc)
{
	BASE_LIST_NODE* pstNode = NULL;
	LOG_PROCESS_ERROR(dwSceneTempID <= MAX_SCENE_TEMPLATE_COUNT);
	
	pstNode = m_lstSceneSameTempID[dwSceneTempID].Head().pNext;
	for(; pstNode != &(m_lstSceneSameTempID[dwSceneTempID].Rear()); pstNode = pstNode->pNext)
	{
		SCENE_INFO* pstSceneInfo = (SCENE_INFO*)pstNode;
		if(pstSceneInfo)
			rFunc(pstSceneInfo);
	}

	return TRUE;
Exit0:
	return FALSE;
}

inline void CSceneMgr::AddSceneTempRoleCount(INT32 nServerGroup, INT32 nSceneTempID)
{
	LOG_PROCESS_ERROR(nServerGroup > 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(nSceneTempID > 0 && nSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

	m_nSceneTempRoleCount[nServerGroup][nSceneTempID]++;
Exit0:
	return;
}

inline void CSceneMgr::SubSceneTempRoleCount(INT32 nServerGroup, INT32 nSceneTempID)
{
	LOG_PROCESS_ERROR(nServerGroup > 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(nSceneTempID > 0 && nSceneTempID < MAX_SCENE_TEMPLATE_COUNT);
	LOG_CHECK_ERROR(m_nSceneTempRoleCount[nServerGroup][nSceneTempID] > 0);

	m_nSceneTempRoleCount[nServerGroup][nSceneTempID]--;
Exit0:
	return;
}

inline INT32 CSceneMgr::GetSceneTempRoleCount(INT32 nServerGroup, INT32 nSceneTempID)
{
	LOG_PROCESS_ERROR(nServerGroup > 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(nSceneTempID > 0 && nSceneTempID < MAX_SCENE_TEMPLATE_COUNT);

	return m_nSceneTempRoleCount[nServerGroup][nSceneTempID];
Exit0:
	return 0;
}

#endif
