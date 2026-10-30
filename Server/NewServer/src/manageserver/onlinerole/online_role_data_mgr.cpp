#include "stdafx.h"
#include "online_role_data_mgr.h"

#include <time.h>

#include "time_ex.h"
#include "server_config.h"

#include "role_data_mgr.h"
#include "onlinerole/online_role_data.h"
#include "client/usermgr.h"
#include "manageserver.h"
#include "team/team.h"
#include "team/room.h"
#include "./master/master_mgr.h"
#include "./guild/ms_guild_mgr.h"
#include "./queuematch/battlequeuemgr.h"
#include "scene/scenemgr.h"

COnlineRoleDataMgr COnlineRoleDataMgr::ms_Instance;

COnlineRoleDataMgr::COnlineRoleDataMgr()
{
}

COnlineRoleDataMgr::~COnlineRoleDataMgr()
{
	UnInit();
}

COnlineRoleDataMgr& COnlineRoleDataMgr::GetInstance(void)
{
    return ms_Instance;
}

BOOL COnlineRoleDataMgr::Init(UINT32 dwMaxOnlineRole)
{
    INT32 nRetCode = 0;

    nRetCode = m_oOnlineRolePool.Init(dwMaxOnlineRole);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;

Exit0:
    return FALSE;
}

BOOL COnlineRoleDataMgr::UnInit(void)
{
    INT32 nRetCode = 0;

    nRetCode = m_oOnlineRolePool.UnInit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
}

MS_ONLINE_ROLE_DATA* COnlineRoleDataMgr::NewOnlineRole(MS_USER* poUser)
{
    INT32 nRetCode = 0;
	MS_ONLINE_ROLE_DATA* pResult = NULL;
    std::pair<CID2RoleDataMapItr, bool>InsRet1;
    std::pair<CName2RoleDataMapItr, bool> InsRet2;
    MS_ROLE_DATA* poRoleData = NULL;
    UINT8 byGroupID = 0;

    LOG_PROCESS_ERROR(poUser);
    LOG_PROCESS_ERROR(poUser->dwCurRoleID);
    byGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(poUser->dwCliSessionID);
    LOG_PROCESS_ERROR(byGroupID >= 0 && byGroupID < MAX_SERVER_GROUP_COUNT);

    poRoleData = CRoleDataMgr::GetInstance().FindRole(poUser->dwCurRoleID);
    LOG_PROCESS_ERROR(poRoleData);

    //////////////////////////////////////////////////////////////////////////////
    // new
    ////////////////////////////////////////////////////////////////////////////// 
    pResult = m_oOnlineRolePool.New(poUser->dwCurRoleID);
    LOG_PROCESS_ERROR(pResult);

    InsRet1 = m_mapID2RoleData[byGroupID].insert(std::make_pair(poRoleData->dwRoleID, pResult));
    LOG_PROCESS_ERROR(InsRet1.second);

    InsRet2 = m_mapName2RoleData[byGroupID].insert(std::make_pair(poRoleData->szRoleName, pResult));
    LOG_PROCESS_ERROR(InsRet2.second);

    //////////////////////////////////////////////////////////////////////////////
    // init
    ////////////////////////////////////////////////////////////////////////////// 
    
    // essence info
    pResult->pUser = poUser;
    pResult->pRoleData = poRoleData;
    pResult->dwCliSessionID = poUser->dwCliSessionID;

    // basic info
    pResult->nCurrentHp = 0;
    pResult->nCurrentMp = 0;
    pResult->nMaxHp = 0;
    pResult->nMaxMp = 0;
    memset(pResult->dwJoinTeamRoleID, 0, sizeof(pResult->dwJoinTeamRoleID));
	pResult->dwGSSessionID = 0;
	pResult->dwTargetGSSessionID = 0;
	pResult->nGMLevel = 0;
	pResult->nPreEnterSceneTempID = 0;
	pResult->nPreEnterSceneID = 0;
	pResult->dwWaitReqRoleID = 0;
	pResult->m_byFriendState = 0;
	pResult->dwGuildInviterID = 0;
	pResult->bEnabledTeamRecall = FALSE;

    // team
    pResult->m_TeamInvite.clear();

    // frequency limits
    pResult->dwLastSpeakTime = 0;
    pResult->dwLastQueryRoleInfoTime = 0;
    pResult->byQueryRoleInfoCount = 0;

    // scene session info
    pResult->pPrev = pResult->pNext = NULL; // as a list node by SceneInfo->lstOnlineRole
    //pResult->dwSaveTime = 0;

    // joining
    //pResult->stArenaQueue.byQueue = qsNotInQueue;
    pResult->stBattleQueue.byQueue = qsNotInQueue;

    // friend
    pResult->m_friends.clear();
    pResult->m_fans.clear();
    pResult->m_personalEnemy.clear();

    // master
    pResult->bTwitter = FALSE;
    pResult->dwBeDeterminedRoleID = 0;
    pResult->dwMasterTwitterCD = 0;
	pResult->llMasterExp = 0;
    pResult->dwMasterCD = 0;
    ClearNode(&(pResult->stMasterTwitterNode));
    pResult->byMasterSendAppNum = 0;
    pResult->byMasterRecvAppNum = 0;
    memset(pResult->arrMasterSendApp, 0, sizeof(pResult->arrMasterSendApp));
    memset(pResult->arrMasterRecvApp, 0, sizeof(pResult->arrMasterRecvApp));

    // guild
    // dwGuildInviterID = 0; 
    
    // shop
    pResult->m_byShopLogLoaded = 0;
    pResult->m_lstShopLog.clear();

    return pResult;
Exit0:
    if(pResult)
        DelOnlineRole(pResult);

    return NULL;
}

BOOL COnlineRoleDataMgr::DelOnlineRole(MS_ONLINE_ROLE_DATA* poRoleData)
{
    INT32 nRetCode;
    CID2RoleDataMapItr itrID;
    CName2RoleDataMapItr itrName;
    CMsGuild    *poMsGuild = NULL;
    CMsGuildMember  *poMsGMember = NULL;

    BOOL bRet = FALSE;
    CTeam* poTeam = NULL;
    CMessageHandler& mh = CMessageHandler::GetInstance();
    UINT8 byGroupID = 0;

    MS_ONLINE_ROLE_DATA::PE_KILL_LIST::iterator   pe_end_iter;

    LOG_PROCESS_ERROR(poRoleData);
    LOG_PROCESS_ERROR(poRoleData->pRoleData);

    byGroupID = GET_GROUP_INDEX_FROM_SESSION_ID(poRoleData->dwCliSessionID);
    LOG_PROCESS_ERROR(byGroupID >= 0 && byGroupID < MAX_SERVER_GROUP_COUNT);

    //////////////////////////////////////////////////////////////////////////////
    // uninit
    ////////////////////////////////////////////////////////////////////////////// 
    
    ////// as a list node
	LOG_PROCESS_ERROR(poRoleData->pPrev == NULL);
	LOG_PROCESS_ERROR(poRoleData->pNext == NULL);
 
    ////// cancel joining
 //   // arena
	//nRetCode = CArenaQueueMgr::GetInstance().StopQueue(poRoleData->pRoleData->dwRoleID);
	//LOG_CHECK_ERROR(nRetCode);
    // battle
	if(poRoleData->stBattleQueue.byQueue == qsInQueue)
	{
		nRetCode = CBattleQueueMgr::GetInstance().CancelJoinBattle(poRoleData->pRoleData->dwRoleID);
		LOG_CHECK_ERROR(nRetCode);
	}
    // team
    poTeam = CTeamMgr::GetInstance().Find(poRoleData->pRoleData->dwTeamID);
    if(poTeam && poTeam->GetLeader() == poRoleData->pRoleData->dwRoleID)
    {
        nRetCode = poTeam->SelectLeader(FALSE);
        LOG_PROCESS_ERROR(nRetCode);

        if(poTeam->GetLeader() != poRoleData->pRoleData->dwRoleID)
        {
            nRetCode = CMessageHandler::GetInstance().DoClientTeamChangeInfo(poTeam);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CMessageHandler::GetInstance().DoGSTeamChangeInfo(poTeam);
            LOG_PROCESS_ERROR(nRetCode);
        }
		else if(poTeam->GetActivityID() > 0)
		{

		}
    }
	if(poTeam)
	{
		nRetCode = CMessageHandler::GetInstance().DoClientTeamSyncMemberState(poRoleData->pRoleData->dwRoleID, etsOffline, poTeam);
		LOG_CHECK_ERROR(nRetCode);
	}

	// Refresh ranking data
	//bRet = CRankListMgr::GetInstance().UpdateRank(poRoleData->dwCliSessionID, poRoleData->pRoleData->dwRoleID, poRoleData->pRoleData->byLevel);
	//LOG_CHECK_ERROR(bRet);

    ////// friend
    //Notify followers of logout
    for (MS_ONLINE_ROLE_DATA::FanMap::const_iterator it=poRoleData->m_fans.begin(); it!=poRoleData->m_fans.end(); ++it)
    {
        MS_ONLINE_ROLE_DATA* online_role = FindOnlineRoleByRoleID(*it);
        if (online_role)
            mh.DoClientFriendInfoChange(online_role->dwCliSessionID, poRoleData->pRoleData);
    }

    pe_end_iter = poRoleData->m_KillList.end();
    for (MS_ONLINE_ROLE_DATA::PE_KILL_LIST::iterator iter = poRoleData->m_KillList.begin(); iter != pe_end_iter; ++iter)
    {
        MS_ONLINE_ROLE_DATA* online_role = FindOnlineRoleByRoleID(*iter);
        if (online_role)
            mh.DoClientFriendInfoChange(online_role->pUser->dwCliSessionID, poRoleData->pRoleData);
    }

    // master
    nRetCode = MasterRoleLogout(*poRoleData);
    LOG_CHECK_ERROR(nRetCode);

    ////// guild
    poRoleData->pRoleData->nLastLoginTime = CManageServer::GetInstance().GetGameTime();        // Record logout time. Allan Tao [9.6.2013]
    if (0 != poRoleData->pRoleData->dwGuildID)
    {
        poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(poRoleData->pRoleData->dwGuildID);
        LOG_CHECK_ERROR(poMsGuild);
        if (poMsGuild)
        {
            nRetCode = CMessageHandler::GetInstance().DoClientSyncSocialAttrNtf(poMsGuild, 
                poRoleData->pRoleData->dwRoleID, 0, poRoleData->pRoleData->nLastLoginTime);
            LOG_CHECK_ERROR(nRetCode);

            poMsGMember = poMsGuild->GetMember(poRoleData->pRoleData->dwRoleID);
            LOG_CHECK_ERROR(poMsGMember);
            if (poMsGMember)
                poMsGMember->SavePow();
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    // delete
    ////////////////////////////////////////////////////////////////////////////// 
    itrID = m_mapID2RoleData[byGroupID].find(poRoleData->pRoleData->dwRoleID);
    LOG_CHECK_ERROR(itrID != m_mapID2RoleData[byGroupID].end());
    if (itrID != m_mapID2RoleData[byGroupID].end())
        m_mapID2RoleData[byGroupID].erase(itrID);

    itrName = m_mapName2RoleData[byGroupID].find(poRoleData->pRoleData->szRoleName);
    LOG_CHECK_ERROR(itrName != m_mapName2RoleData[byGroupID].end());
    if (itrName != m_mapName2RoleData[byGroupID].end())
        m_mapName2RoleData[byGroupID].erase(itrName);

    nRetCode = m_oOnlineRolePool.Delete(poRoleData);
    LOG_CHECK_ERROR(nRetCode);
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL COnlineRoleDataMgr::PreEnterScene(MS_ONLINE_ROLE_DATA* pstRoleData, SCENE_INFO* pstSceneInfo)
{
    LOG_PROCESS_ERROR(pstRoleData);
    LOG_PROCESS_ERROR(pstSceneInfo);

    pstRoleData->nPreEnterSceneTempID = pstSceneInfo->nSceneTempID;
    pstRoleData->nPreEnterSceneID = pstSceneInfo->nSceneID;

    pstSceneInfo->nRoleCount++; // move to EnterScene?

    //if(pstSceneInfo->bySideCount > 0)
    //{
    //    LOG_PROCESS_ERROR(bySide <= pstSceneInfo->bySideCount);
    //    pstRoleData->byPreEnterSide = bySide;
    //}
    //else
    //{
    //    pstRoleData->byPreEnterSide = 0;
    //}

    return TRUE;
Exit0:
    return FALSE;
}

BOOL COnlineRoleDataMgr::EnterScene(MS_ONLINE_ROLE_DATA* pstRoleData, SCENE_INFO* pstSceneInfo)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pstRoleData);
    LOG_PROCESS_ERROR(pstSceneInfo);

    LOG_PROCESS_ERROR(pstRoleData->nPreEnterSceneTempID == pstSceneInfo->nSceneTempID);
    LOG_PROCESS_ERROR(pstRoleData->nPreEnterSceneID == pstSceneInfo->nSceneID);

    pstRoleData->pRoleData->nSceneTempID = pstSceneInfo->nSceneTempID;
    pstRoleData->pRoleData->nSceneID = pstSceneInfo->nSceneID;
    //pstRoleData->pRoleData->bySide = pstRoleData->byPreEnterSide;

    pstRoleData->nPreEnterSceneTempID = pstRoleData->nPreEnterSceneID = 0;
    //pstRoleData->byPreEnterSide = 0;

    LOG_PROCESS_ERROR(pstRoleData->pPrev == NULL);
    LOG_PROCESS_ERROR(pstRoleData->pNext == NULL);
    pstSceneInfo->lstOnlineRole.PushRear(pstRoleData);

	CSceneMgr::GetInstance().AddSceneTempRoleCount(pstRoleData->pRoleData->byGroup, pstSceneInfo->nSceneTempID);

    nRetCode = CMessageHandler::GetInstance().DoDLEnterScene(CManageServer::GetInstance().GetGameTime(), pstRoleData->pRoleData->byGroup, 
        pstSceneInfo->nSceneID, pstSceneInfo->nSceneTempID, pstRoleData->pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    ////// guild
    if (0 != pstRoleData->pRoleData->dwGuildID)
    {
        CMsGuild *poMsGuild = CMsGuildMgr::GetInstance().FindGuildByID(pstRoleData->pRoleData->dwGuildID);
        if (poMsGuild)
        {
            nRetCode = CMessageHandler::GetInstance().DoClientSyncSocialAttrNtf(poMsGuild,
                pstRoleData->pRoleData->dwRoleID, pstRoleData->pRoleData->byLevel, pstRoleData->pRoleData->nSceneTempID);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL COnlineRoleDataMgr::LeaveScene(MS_ONLINE_ROLE_DATA* pstRoleData, INT32 nSceneTempID, INT32 nSceneID, BOOL bNormalLeave)
{
    INT32 nRetCode = 0;
    SCENE_INFO* pstSceneInfo = NULL;

    LOG_PROCESS_ERROR(pstRoleData);
    LOG_PROCESS_ERROR(pstRoleData->pRoleData);

    pstSceneInfo = CSceneMgr::GetInstance().FindScene( nSceneTempID, nSceneID );
    LOG_PROCESS_ERROR(pstSceneInfo);

    if(pstRoleData->pRoleData->nSceneTempID == pstSceneInfo->nSceneTempID && pstRoleData->pRoleData->nSceneID == pstSceneInfo->nSceneID)
    {
        if(pstSceneInfo->nSceneType == estArena)
        {
            //nRetCode = CArenaQueueMgr::GetInstance().LeaveArena(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, 
            //    pstRoleData->byPreEnterSide, bNormalLeave);  // pstRoleData->pRoleData->bySide ?
            //LOG_CHECK_ERROR(nRetCode);
        }
        else if(pstSceneInfo->nSceneType == estBattleField)
        {
            nRetCode = CBattleQueueMgr::GetInstance().LeaveBattle(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, bNormalLeave);
            LOG_CHECK_ERROR(nRetCode);
        }

        pstRoleData->pRoleData->nSceneTempID = pstRoleData->pRoleData->nSceneID = 0;	
    }
    else if(pstRoleData->nPreEnterSceneTempID == pstSceneInfo->nSceneTempID && pstRoleData->nPreEnterSceneID == pstSceneInfo->nSceneID)
    {
        if(pstSceneInfo->nSceneType == estArena)
        {
            //nRetCode = CArenaQueueMgr::GetInstance().LeaveArena(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, 
            //    pstRoleData->byPreEnterSide, bNormalLeave);
            //LOG_CHECK_ERROR(nRetCode);
        }
        else if(pstSceneInfo->nSceneType == estBattleField)
        {
            nRetCode = CBattleQueueMgr::GetInstance().LeaveBattle(pstRoleData->pRoleData->dwRoleID, pstSceneInfo->nSceneTempID, pstSceneInfo->nSceneID, bNormalLeave);
            LOG_CHECK_ERROR(nRetCode);
        }

        pstRoleData->nPreEnterSceneTempID = pstRoleData->nPreEnterSceneID = 0;
    }
    else
        LOG_PROCESS_ERROR(FALSE);

    pstSceneInfo->nRoleCount--;

    //LOG_CHECK_ERROR(pstRoleData->pPrev != NULL || pstRoleData->pNext != NULL);  // ?
    if(pstRoleData->pPrev != NULL || pstRoleData->pNext != NULL)
        pstSceneInfo->lstOnlineRole.Remove(pstRoleData);

	CSceneMgr::GetInstance().SubSceneTempRoleCount(pstRoleData->pRoleData->byGroup, pstSceneInfo->nSceneTempID);

    nRetCode = CMessageHandler::GetInstance().DoDLLeaveScene(CManageServer::GetInstance().GetGameTime(), 
		pstRoleData->pRoleData->byGroup, pstSceneInfo->nSceneID, pstSceneInfo->nSceneTempID, pstRoleData->pRoleData->dwRoleID);
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}  

MS_ONLINE_ROLE_DATA*  COnlineRoleDataMgr::FindOnlineRoleByName(const char* pcszRoleName)
{
    for(INT32 i = 0; i < MAX_SERVER_GROUP_COUNT; ++i)
    {
        CName2RoleDataMapItr itr = m_mapName2RoleData[i].find(pcszRoleName);
        if(itr != m_mapName2RoleData[i].end())
            return itr->second;
    }

    return NULL;
}

MS_ONLINE_ROLE_DATA*  COnlineRoleDataMgr::FindOnlineRoleByRoleID(UINT32 dwRoleID)
{
    MS_ONLINE_ROLE_DATA* pResult = NULL;

    pResult = m_oOnlineRolePool.Find(dwRoleID);
    
    return pResult;
}

MS_ONLINE_ROLE_DATA*    COnlineRoleDataMgr::FindOnlineRoleByNameInGroup(const char* pcszRoleName, UINT8 byGroupID)
{
    CName2RoleDataMapItr itr;

    LOG_PROCESS_ERROR(byGroupID >= 0 && byGroupID < MAX_SERVER_GROUP_COUNT);
    LOG_PROCESS_ERROR(pcszRoleName != NULL);

    itr = m_mapName2RoleData[byGroupID].find(pcszRoleName);
    if(itr != m_mapName2RoleData[byGroupID].end())
        return itr->second;

Exit0:
    return NULL;

}

MS_ONLINE_ROLE_DATA* COnlineRoleDataMgr::FindOnlineRoleByRoleIDInGroup(UINT32 dwRoleID, UINT8 byGroupID)
{
    CID2RoleDataMapItr itr;
    LOG_PROCESS_ERROR(byGroupID >= 0 && byGroupID < MAX_SERVER_GROUP_COUNT);

    itr = m_mapID2RoleData[byGroupID].find(dwRoleID);
    if(itr != m_mapID2RoleData[byGroupID].end())
        return itr->second;

Exit0:
    return FALSE;
}

UINT32	COnlineRoleDataMgr::GetRoleNumberCount(UINT8 byGroupIndex)
{
	UINT32 nResult = 0;

    LOG_PROCESS_ERROR(byGroupIndex < MAX_SERVER_GROUP_COUNT);
	nResult = (UINT32)m_mapID2RoleData[byGroupIndex].size();
Exit0:
	return nResult;
}

UINT32 COnlineRoleDataMgr::GetRoleNumberCount(void)
{
    return m_oOnlineRolePool.GetUsedSize();
}
