#include "stdafx.h"
#include "team.h"

#include <time.h>

#include "time_ex.h"
#include "game_world_config.h"

#include "role_data_mgr.h"
#include "client/usermgr.h"
#include "onlinerole/online_role_data_mgr.h"
#include "id_def.h"
#include "message_handler.h"
#include "room.h"
#include "manageserver.h"
#include "dungeon_data.h"
#include "data/activity_data.h"

UINT32                       g_TeamIndex = 1;
UINT32                       g_dwRecord = 0;

static CTeamMgr m_Instance;

UINT32 GenerateTeamID()
{
    return (otTeam << 28) + g_TeamIndex++;
}

BOOL CTraverseTeamToGS::operator ()(UINT32 dwTeamID, CTeam* poTeam)
{
    INT32 nRetCode;
    LOG_PROCESS_ERROR(dwGSSessionID > 0);

    nRetCode = CMessageHandler::GetInstance().DoGSTeamInfo(dwGSSessionID, poTeam);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}   

CTeam::CTeam()
{
    memset(m_szTeamName, 0, TEAM_NAME_LEN);
    m_dwTeamID = 0;

    m_nMemberSizeLimit = 0;
    m_byAssignType = etaFreeForAll;
    m_dwLeaderID = 0;
    memset(m_dwMemberIDList, 0, sizeof(m_dwMemberIDList));
    m_byGroupID = 0;
    m_nMemberCount = 0;
    m_dwCreateTime = 0;
    m_dwActivityID = 0;
	m_byAssignType = 0;

    m_nDungeonTempID = m_nDungeonID = 0;
}

CTeam::~CTeam()
{
    UnInit();
}

BOOL CTeam::Init(UINT8 byGroupID, UINT32 dwTeamID, const CHAR* pszTeamName, UINT32 dwMaxMemberCount, UINT8 byAssignType, UINT8 byAuctionItemColorLimit)
{
    LOG_PROCESS_ERROR(dwTeamID != 0);
    LOG_PROCESS_ERROR(pszTeamName != NULL);
	LOG_PROCESS_ERROR(dwMaxMemberCount <= MAX_TEAM_MEMBER_COUNT);

    m_dwActivityID = 0;
    m_byGroupID = byGroupID;
    m_dwTeamID = dwTeamID;
    m_nMemberSizeLimit = dwMaxMemberCount;
    m_nMemberCount = 0;
    m_byAssignType = byAssignType;
	m_byAuctionItemColorLimit = byAuctionItemColorLimit;
    strxcpy(m_szTeamName, pszTeamName, TEAM_NAME_LEN);
	strxcat(m_szTeamName, g_GWConfig.TeamDefaultName, TEAM_NAME_LEN);
    m_dwLeaderID = 0;
    memset(m_dwMemberIDList, 0, sizeof(m_dwMemberIDList));
    m_nDungeonTempID = m_nDungeonID = 0;
    m_dwCreateTime = CManageServer::GetInstance().GetGameTime();

	m_dwActivityID = 0;
	m_dwActivityType = etaatInvalid;

    return TRUE;
Exit0:
    return FALSE;

}

BOOL CTeam::UnInit()
{
    return TRUE;
}   

BOOL CTeam::AddMember(UINT32 dwRoleID)
{
    INT32 nRetCode = 0;
    MS_ROLE_DATA* poRoleData = NULL;

    LOG_PROCESS_ERROR(m_nMemberCount < m_nMemberSizeLimit);

    poRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
    LOG_PROCESS_ERROR(poRoleData);

    for (INT32 nIndex = 0; nIndex < m_nMemberSizeLimit; nIndex++)
    {
        if (m_dwMemberIDList[nIndex] == 0)
        {
            poRoleData->dwTeamID = m_dwTeamID;
            m_dwMemberIDList[nIndex] = dwRoleID;
            m_nMemberCount++;

            return TRUE;
        }
    }

Exit0:
    return FALSE;
}

MS_ROLE_DATA*  CTeam::FindMember(UINT32 dwRoleID)
{
    MS_ROLE_DATA* poRoleData = NULL;

    for (INT32 nIndex = 0; nIndex < m_nMemberSizeLimit; nIndex++)
    {
        if (m_dwMemberIDList[nIndex] == dwRoleID)
        {
            poRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
            LOG_PROCESS_ERROR(poRoleData);

            return poRoleData;
        }
    }

Exit0:
    return NULL;
}

MS_ROLE_DATA* CTeam::GetMember(INT32 nMemberIndex)
{
    MS_ROLE_DATA* poRoleData = NULL;
    LOG_PROCESS_ERROR(nMemberIndex >= 0 && nMemberIndex < m_nMemberSizeLimit);

    poRoleData = CRoleDataMgr::GetInstance().FindRole(m_dwMemberIDList[nMemberIndex]);
    return poRoleData;
Exit0:
    return NULL;
}

UINT32 CTeam::GetMemberID(INT32 nMemberIndex)
{
    LOG_PROCESS_ERROR(nMemberIndex >= 0 && nMemberIndex < m_nMemberSizeLimit);

    return m_dwMemberIDList[nMemberIndex];
Exit0:
    return 0;
}

BOOL  CTeam::DelMember(UINT32 dwRoleID)
{
    MS_ROLE_DATA* poRoleData = NULL;

    LOG_PROCESS_ERROR(m_dwLeaderID != dwRoleID || m_nMemberCount == 1);

    for (INT32 nIndex = 0; nIndex < m_nMemberSizeLimit; nIndex++)
    {
        if (m_dwMemberIDList[nIndex] == dwRoleID)
        {
            poRoleData = CRoleDataMgr::GetInstance().FindRole(dwRoleID);
            LOG_PROCESS_ERROR(poRoleData);

            poRoleData->dwTeamID = INVALID_OBJECT_ID;
            m_dwMemberIDList[nIndex] = 0;

            break;
        }
    }

    m_nMemberCount--;
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CTeam::SelectLeader(BOOL bLeaveTeam)
{
    PROCESS_SUCCESS(m_nMemberCount <= 1);

    for (INT32 nIndex = 0; nIndex < m_nMemberSizeLimit; nIndex++)
    {
        if (m_dwMemberIDList[nIndex] == 0)
            continue;

        UINT32 dwRoleID = m_dwMemberIDList[nIndex];
        if (dwRoleID == m_dwLeaderID)
            continue;

        MS_ONLINE_ROLE_DATA* pstOnlineRole = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByRoleID(dwRoleID);
        if(pstOnlineRole && pstOnlineRole->pUser->eState == USER_STATE_INGAME)
        {
            m_dwLeaderID = dwRoleID;
            return TRUE;
        }
    }
    
    PROCESS_SUCCESS(bLeaveTeam == FALSE);
    //Transfer leadership before leaving the team
    for (INT32 nIndex = 0; nIndex < m_nMemberSizeLimit; nIndex++)
    {
        if (m_dwMemberIDList[nIndex] == 0)
            continue;

        UINT32 dwRoleID = m_dwMemberIDList[nIndex];
        if (dwRoleID != m_dwLeaderID)
        {
            m_dwLeaderID = dwRoleID;
            return TRUE;
        }
    }

    return FALSE;
Exit1:
    return TRUE;
}

/************************************************************************/
/*                         CTeamMgr                                     */
/************************************************************************/

CTeamMgr CTeamMgr::m_Instance;

CTeamMgr::CTeamMgr()
{
	memset(&nActivityTeam, 0, sizeof(nActivityTeam));
	memset(&nDungeonTeam, 0, sizeof(nDungeonTeam));

	MapActivityTeam.clear();
	MapDungeonTeam.clear();
}

CTeamMgr::~CTeamMgr()
{

}

INT32 CTeamMgr::Init(INT32 nInitSize)
{
	INT32 nRetCode = 0;
	INT32 nTeamCount = 0;
	const DUNGEON_DATA* pDungeon = NULL;
	const ACTIVITY_DATA* pActivity = NULL;

	memset(&nActivityTeam, 0, sizeof(nActivityTeam));
	memset(&nDungeonTeam, 0, sizeof(nDungeonTeam));

	MapActivityTeam.clear();
	MapDungeonTeam.clear();

	nRetCode = m_TeamMgr.Init(nInitSize);
	LOG_PROCESS_ERROR(nRetCode);

	pDungeon = CDataManager<DUNGEON_DATA>::GetInstance().GetFirstData();
	while (pDungeon && nTeamCount < MAX_TEAM_DUNGEON)
	{
		MapDungeonTeam.insert(std::make_pair(pDungeon->nSceneID, &(nDungeonTeam[nTeamCount++][0])));
		pDungeon = CDataManager<DUNGEON_DATA>::GetInstance().GetNextData();
	}

	nTeamCount = 0;
	pActivity = CDataManager<ACTIVITY_DATA>::GetInstance().GetFirstData();
	while (pActivity && nTeamCount < MAX_TEAM_DUNGEON)
	{
		MapActivityTeam.insert(std::make_pair(pActivity->dwActivityID, &(nActivityTeam[nTeamCount++][0])));
		pActivity = CDataManager<ACTIVITY_DATA>::GetInstance().GetNextData();
	}

	return TRUE;
Exit0:
	return FALSE;
}

CTeamMgr& CTeamMgr::GetInstance()
{
	return m_Instance;
}

INT32 CTeamMgr::UnInit()
{
	BOOL nRetCode = FALSE;

	nRetCode = m_TeamMgr.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	MapActivityTeam.clear();
	MapDungeonTeam.clear();

	return TRUE;	
}

CTeam* CTeamMgr::Find(UINT32 dwTeamID)
{
	return m_TeamMgr.Find(dwTeamID);
}

CTeam* CTeamMgr::New(UINT32 dwTeamID)
{
	return m_TeamMgr.New(dwTeamID);
}

BOOL CTeamMgr::Delete(CTeam* pTeam)
{
	return m_TeamMgr.Delete(pTeam);
}

INT32 CTeamMgr::AddActivityTeam(UINT32 dwActivityType, INT32 nActivityID, INT32 nTeamID)
{
	INT32 nRetCode = 0;
	INT32* pTeamHead = NULL;
	Iter_MapId2Team Iter;

	switch (dwActivityType)
	{
	case etaatActivity:
		{	
			Iter = MapActivityTeam.find(nActivityID);
			LOG_PROCESS_ERROR(Iter != MapActivityTeam.end());
		}
		break;
	case etaatDungeon:
		{
			Iter = MapDungeonTeam.find(nActivityID);
			LOG_PROCESS_ERROR(Iter != MapDungeonTeam.end());
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	pTeamHead = Iter->second;

	for (INT32 nIndex = 0; nIndex < MAX_TEAM_COUNT_PER_ACTIVITY; nIndex++)
	{
		if (*pTeamHead == 0)
		{
			*pTeamHead = nTeamID;

			return TRUE;
		}

		pTeamHead++;
	}

Exit0:
	return FALSE;
}

INT32 CTeamMgr::DeleteActivityTeam(UINT32 dwActivityType, INT32 nActivityID, INT32 nTeamID)
{
	INT32 nRetCode = 0;
	INT32* pTeamHead = NULL;
	Iter_MapId2Team Iter;

	switch (dwActivityType)
	{
	case etaatActivity:
		{
			Iter = MapActivityTeam.find(nActivityID);
			LOG_PROCESS_ERROR(Iter != MapActivityTeam.end());
		}
		break;
	case etaatDungeon:
		{
			Iter = MapDungeonTeam.find(nActivityID);
			LOG_PROCESS_ERROR(Iter != MapDungeonTeam.end());
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	pTeamHead = Iter->second;
	LOG_PROCESS_ERROR(pTeamHead);

	for (INT32 nIndex = 0; nIndex < MAX_TEAM_COUNT_PER_ACTIVITY; nIndex++)
	{
		if (*pTeamHead == nTeamID)
		{
			*pTeamHead = 0;
		}

		pTeamHead++;
	}

	return TRUE;
Exit0:
	return FALSE;
}

INT32* CTeamMgr::GetActivityTeamHead(UINT32 dwActivityType, INT32 nActivityID)
{
	INT32 nRetCode = 0;
	INT32* pTeamHead = NULL;
	Iter_MapId2Team Iter;

	switch (dwActivityType)
	{
	case etaatActivity:
		{
			Iter = MapActivityTeam.find(nActivityID);
			LOG_PROCESS_ERROR(Iter != MapActivityTeam.end());
		}
		break;
	case etaatDungeon:
		{
			Iter = MapDungeonTeam.find(nActivityID);
			LOG_PROCESS_ERROR(Iter != MapDungeonTeam.end());
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}

	LOG_PROCESS_ERROR(Iter->second);
	return Iter->second;

Exit0:
	return NULL;
}