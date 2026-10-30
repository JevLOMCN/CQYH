#include "stdafx.h"
#include "master.h"
#include "id_def.h"
#include "role.h"
#include "message_handler.h"
#include "game_world.h"

CMaster::CMaster() :
m_dwTeacherID(0),
m_nStudentNum(0),
m_nState(MASTER_NONE)
{
    m_bTwitter = TRUE;
    m_dwTwitterCD = 0;
    m_dwSelfID = 0;
    m_dwTeacherID = 0;
    m_nStudentNum = 0;
    m_nState = MASTER_NONE;
    m_dwCD = 0;

    memset(m_arrStudentID, 0, sizeof(m_arrStudentID));

	memset(m_TeacherReward, 0, sizeof(m_TeacherReward));
    m_dwStudentRewardTeacherID = 0;
	m_nStudentRewardExp = 0;

#ifdef _ROBOT
    m_nTeacherCZ = 0;
    m_nTeacherLevel = 0;
    memset(m_nStudentCZ, 0, sizeof(m_nStudentCZ));
    memset(m_nStudentLevel, 0, sizeof(m_nStudentLevel));
    m_nRecruitNum = 0;
    memset(m_dwRecruitList, 0, sizeof(m_dwRecruitList));
    m_nSoloNum = 0;
    memset(m_dwSoloList, 0, sizeof(m_dwSoloList));
    m_nSendAppNum = 0;
    memset(m_dwSendAppList, 0, sizeof(m_dwSendAppList));
    m_nRecvAppNum = 0;
    memset(m_dwRecvAppList, 0, sizeof(m_dwRecvAppList));

    m_dwGraduateRoleID = 0;
#endif
}

CMaster::~CMaster()
{
    m_bTwitter = TRUE;
    m_dwTwitterCD = 0;
    m_dwSelfID = 0;
    m_dwTeacherID = 0;
    m_nStudentNum = 0;
    m_nState = MASTER_NONE;
    m_dwCD = 0;
    memset(m_arrStudentID, 0, sizeof(m_arrStudentID));

#ifdef _ROBOT
    m_nTeacherCZ = 0;
    m_nTeacherLevel = 0;
    memset(m_nStudentCZ, 0, sizeof(m_nStudentCZ));
    memset(m_nStudentLevel, 0, sizeof(m_nStudentLevel));
    m_nRecruitNum = 0;
    memset(m_dwRecruitList, 0, sizeof(m_dwRecruitList));
    m_nSoloNum = 0;
    memset(m_dwSoloList, 0, sizeof(m_dwSoloList));
    m_nSendAppNum = 0;
    memset(m_dwSendAppList, 0, sizeof(m_dwSendAppList));
    m_nRecvAppNum = 0;
    memset(m_dwRecvAppList, 0, sizeof(m_dwRecvAppList));

    m_dwGraduateRoleID = 0;
#endif

}

BOOL CMaster::Init(UINT32 dwSelfID)
{
    m_bTwitter = TRUE;
    m_dwTwitterCD = 0;
    m_dwSelfID = dwSelfID;
    m_dwTeacherID = 0;
    m_nStudentNum = 0;
    m_nState = MASTER_NONE;
    m_dwCD = 0;

    memset(m_arrStudentID, 0, sizeof(m_arrStudentID));

	memset(m_TeacherReward, 0, sizeof(m_TeacherReward));
    m_dwStudentRewardTeacherID = 0;
	m_nStudentRewardExp = 0;

#ifdef _ROBOT
    m_nTeacherCZ = 0;
    m_nTeacherLevel = 0;
    memset(m_nStudentCZ, 0, sizeof(m_nStudentCZ));
    memset(m_nStudentLevel, 0, sizeof(m_nStudentLevel));
    m_nRecruitNum = 0;
    memset(m_dwRecruitList, 0, sizeof(m_dwRecruitList));
    m_nSoloNum = 0;
    memset(m_dwSoloList, 0, sizeof(m_dwSoloList));
    m_nSendAppNum = 0;
    memset(m_dwSendAppList, 0, sizeof(m_dwSendAppList));
    m_nRecvAppNum = 0;
    memset(m_dwRecvAppList, 0, sizeof(m_dwRecvAppList));

    m_dwGraduateRoleID = 0;
#endif

    return TRUE;
}

BOOL CMaster::Clear()
{
    m_dwTeacherID = 0;
    m_nStudentNum = 0;
    m_nState = MASTER_NONE;
    m_dwCD = 0;

    memset(m_arrStudentID, 0, sizeof(m_arrStudentID));

    return TRUE;
}

BOOL CMaster::Onload(UINT32 dwTeacherID, INT32 nCount, UINT32 *pStudentID)
{
    m_dwTeacherID = dwTeacherID;

    LOG_PROCESS_ERROR(pStudentID);
    LOG_PROCESS_ERROR(nCount <= MAX_STUDENT_NUM);

    m_nStudentNum = nCount;
    for (INT32 nIndex = 0; nIndex < nCount; ++nIndex)
    {
        m_arrStudentID[nIndex] = pStudentID[nIndex];
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::DelTeacher(UINT32 dwTeacherID)
{
    LOG_CHECK_ERROR(dwTeacherID == m_dwTeacherID);
    
    Clear();

    return TRUE;
}

BOOL CMaster::DelStudent(UINT32 dwStudentID)
{
#ifdef _ROBOT
    if (m_dwGraduateRoleID == dwStudentID)
        m_dwGraduateRoleID = 0;
#endif //_ROBOT
    for (int i = 0; i < m_nStudentNum; ++i)
    {
        if (m_arrStudentID[i] == dwStudentID)
        {
            m_arrStudentID[i] = m_arrStudentID[--m_nStudentNum];
            break;
        }
    }
    return TRUE;
}

void CMaster::AddTeacher(UINT32 dwTeacherID)
{
    m_dwTeacherID = dwTeacherID;
}

void CMaster::AddStudent(UINT32 dwStudentID)
{
    LOG_PROCESS_ERROR(m_nStudentNum < MAX_STUDENT_NUM);

    for (INT32 i = 0; i < m_nStudentNum; ++i)
        LOG_PROCESS_ERROR(m_arrStudentID[i] != dwStudentID);

    m_arrStudentID[m_nStudentNum] = dwStudentID;
    ++m_nStudentNum;

    return;
Exit0:
    if (5 == MAX_STUDENT_NUM)
        CRI("GS AddStudent Error! studentID[%d] teacherID[%d], StudentNum[%d], Array[%u][%u][%u][%u][%u]",
            dwStudentID, m_dwSelfID, m_nStudentNum, m_arrStudentID[0], m_arrStudentID[1], m_arrStudentID[2], m_arrStudentID[3], m_arrStudentID[4]);
    else
        CRI("GS AddStudent Error! studentID[%d] teacherID[%d], StudentNum[%d]", dwStudentID, m_dwSelfID, m_nStudentNum);

    return;
}

void CMaster::SetCD(UINT32 dwCD)
{ 
    INT32       nRetCode = 0;
    CRole       *poRole = NULL;
    m_dwCD = dwCD; 

    poRole = CGameWorld::GetInstance().FindRole(m_dwSelfID);
    LOG_PROCESS_ERROR(poRole);
#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncMaster(poRole);
    LOG_PROCESS_ERROR(nRetCode);
#endif //_ROBOT
    return ;
Exit0:
    return ;
}

void CMaster::SetState(INT32 nState)
{
    INT32       nRetCode = 0;
    CRole       *poRole = NULL;

    m_nState = nState; 

    poRole = CGameWorld::GetInstance().FindRole(m_dwSelfID);
    LOG_PROCESS_ERROR(poRole);
#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoSyncMaster(poRole);
    LOG_PROCESS_ERROR(nRetCode);
#endif //_ROBOT
    return ;
Exit0:
    return ;
}

UINT32 CMaster::GetStudentByIndex(INT32 nIndex)
{
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_STUDENT_NUM && nIndex < m_nStudentNum);
	
	return m_arrStudentID[nIndex];
Exit0:
	return INVALID_OBJECT_ID;
}

//INT32 CMaster::GetState()
//{
//    if (m_dwTeacherID == m_dwSelfID)
//    {
//        return MASTER_TEACHER;
//    }
//    else 
//    {
//        for (INT32 nIndex = 0; nIndex < m_nStudentNum; ++nIndex)
//        {
//            if (m_arrStudentID[nIndex] == m_dwSelfID)
//            {
//                return MASTER_STUDENT;
//            }
//        }
//    }
//
//    return MASTER_NONE;
//}


BOOL CMaster::IsStudentFull()
{
    CRole       *poRole = NULL;

    poRole = CGameWorld::GetInstance().FindRole(m_dwSelfID);
    LOG_PROCESS_ERROR(poRole);

    if (poRole->GetItemBag()->GetMoney(MONEYTYPE_MASTER_EXP) < g_GWConfig.nMasterLvlTwoExp)    //  < 300
    {
        PROCESS_SUCCESS(m_nStudentNum < g_GWConfig.nMasterLvlOneStudentNum);
    }
    else if ( poRole->GetItemBag()->GetMoney(MONEYTYPE_MASTER_EXP) < g_GWConfig.nMasterLvlThreeExp)   // 300 - 500
    {
        PROCESS_SUCCESS(m_nStudentNum < g_GWConfig.nMasterLevelTwoStudentNum);
    }
    else                                                                    // > 500
    {
        PROCESS_SUCCESS(m_nStudentNum < g_GWConfig.nMasterLevelThreeStudentNum);
    }

Exit1:
    return FALSE;
Exit0:
    return TRUE;
}


BOOL CMaster::LuaSyncMasterExpToMS(CRole *poRole)
{
    INT32   nRetCode = 0;

    LOG_PROCESS_ERROR(poRole);
    LOG_PROCESS_ERROR(m_dwSelfID == poRole->GetObjID());

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSSyncMasterExp(m_dwSelfID, poRole->GetItemBag()->GetMoney(MONEYTYPE_MASTER_EXP));
    LOG_PROCESS_ERROR(nRetCode);
#endif

    return TRUE;
Exit0:
    return FALSE;
}




#ifdef _ROBOT

BOOL CMaster::AsimovLoadTeacher(UINT32 dwTeacherID, UINT8 byCZ)
{
    m_dwTeacherID = dwTeacherID;
    m_nTeacherCZ = byCZ;
    m_nTeacherLevel = 0;
    return TRUE;
}

BOOL CMaster::AsimovLoadStudent(UINT32 dwStudentID, UINT8 byCZ)
{
    LOG_PROCESS_ERROR(m_nStudentNum < MAX_STUDENT_NUM);

    m_arrStudentID[m_nStudentNum] = dwStudentID;
    m_nStudentCZ[m_nStudentNum] = byCZ;
    m_nStudentLevel[m_nStudentNum] = 0;
    ++m_nStudentNum;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::AsimovClearRecruitList()
{
    m_nRecruitNum = 0;
    return TRUE;
}

BOOL CMaster::AsimovLoadRecuritList(UINT32 dwTeacherID)
{
    m_dwRecruitList[m_nRecruitNum++] = dwTeacherID;

    return TRUE;
}

BOOL CMaster::AsimovClearSoloList()
{
    m_nSoloNum = 0;
    return TRUE;
}

BOOL CMaster::AsimovLoadSoloList(UINT32 dwSoloID)
{
    m_dwSoloList[m_nSoloNum++] = dwSoloID;

    return TRUE;
}

INT32 CMaster::AsimovGetRecruitNum()
{
    return m_nRecruitNum;
}

UINT32 CMaster::AsimovGetRecruitRoleByIndex(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < SEND_MASTER_INFO_NUM && nIndex >= 0 && nIndex < m_nRecruitNum);

    return m_dwRecruitList[nIndex];
Exit0:
    return 0;
}

INT32 CMaster::AsimovGetSoloNum()
{
    return m_nSoloNum;
}

UINT32 CMaster::AsimovGetSoloRoleByIndex(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < SEND_SOLO_INFO_NUM && nIndex >= 0 && nIndex < m_nSoloNum);

    return m_dwSoloList[nIndex];
Exit0:
    return 0;
}

INT32 CMaster::AsimovGetSendListNum()
{
    return m_nSendAppNum;
}

UINT32 CMaster::AsimovGetSendRoleIDByIndex(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < MAX_MASTER_SEND_APP_NUM && nIndex >= 0 && nIndex < m_nSendAppNum);
    return m_dwSendAppList[nIndex];
Exit0:
    return 0;
}

INT32 CMaster::AsimovGetRecvNum()
{
    return m_nRecvAppNum;
}

UINT32 CMaster::AsimovGetRecvRoleIDByIndex(INT32 nIndex)
{
    LOG_PROCESS_ERROR(nIndex < MAX_MASTER_RECV_APP_NUM && nIndex >= 0 && nIndex < m_nRecvAppNum);
    return m_dwRecvAppList[nIndex];
Exit0:
    return 0;
}

BOOL CMaster::AsimovIsSendListFull()
{
    return !(m_nSendAppNum < MAX_MASTER_SEND_APP_NUM);
}

BOOL CMaster::AsimovIsInSendAppList(UINT32 dwRoleID)
{
    LOG_PROCESS_ERROR(m_nSendAppNum < MAX_MASTER_SEND_APP_NUM);

    for (INT32 i  = 0; i < m_nSendAppNum; ++i)
    {
        if (m_dwSendAppList[i] == dwRoleID)
            return TRUE;
    }

Exit0:
    return FALSE;
}

BOOL CMaster::AsimovAddSendAppList(UINT32 dwRoleID)
{
    INT32   nRetCode = 0;

    LOG_PROCESS_ERROR(m_nSendAppNum < MAX_MASTER_SEND_APP_NUM);

    nRetCode = AsimovIsInSendAppList(dwRoleID);
    LOG_PROCESS_ERROR(!nRetCode);

    m_dwSendAppList[m_nSendAppNum++] = dwRoleID;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::AsimovDelSendAppList(UINT32 dwRoleID)
{
    INT32   i = 0 ;
    BOOL    bDelFlag = FALSE;
    LOG_PROCESS_ERROR(m_nSendAppNum < MAX_MASTER_SEND_APP_NUM);

    for ( ; i < m_nSendAppNum; ++i)
    {
        if (m_dwSendAppList[i] == dwRoleID)
        {
            bDelFlag = TRUE;
            break;
        }
    }

    if (bDelFlag)
    {
        m_dwSendAppList[i] = m_dwSendAppList[--m_nSendAppNum];
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::AsimovClearSendAppList()
{
    m_nSendAppNum = 0;
    memset(m_dwSendAppList, 0, sizeof(m_dwSendAppList));

    return TRUE;
}

BOOL CMaster::AsimovIsRecvListFull()
{
    return !(m_nRecvAppNum == MAX_MASTER_RECV_APP_NUM);
}

BOOL CMaster::AsimovIsInRecvAppList(UINT32 dwRoleID)
{
    for (INT32 i  = 0; i < m_nRecvAppNum; ++i)
    {
        if (m_dwRecvAppList[i] == dwRoleID)
            return TRUE;
    }

    return FALSE; 
}

BOOL CMaster::AsimovAddRecvAppList(UINT32 dwRoleID)
{
    INT32   nRetCode = 0;

    LOG_PROCESS_ERROR(m_nRecvAppNum < MAX_MASTER_RECV_APP_NUM);

    nRetCode = AsimovIsInRecvAppList(dwRoleID);
    LOG_PROCESS_ERROR(!nRetCode);

    m_dwRecvAppList[m_nRecvAppNum++] = dwRoleID;

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::AsimovDelRecvAppList(UINT32 dwRoleID)
{
    INT32   i = 0 ;
    BOOL    bDelFlag = FALSE;
    LOG_PROCESS_ERROR(m_nRecvAppNum < MAX_MASTER_RECV_APP_NUM);

    for ( ; i < m_nRecvAppNum; ++i)
    {
        if (m_dwRecvAppList[i] == dwRoleID)
        {
            bDelFlag = TRUE;
            break;
        }
    }

    if (bDelFlag)
    {
        m_dwRecvAppList[i] = m_dwRecvAppList[--m_nRecvAppNum];
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::AsimovClearRecvAppList()
{
    m_nRecvAppNum = 0;
    memset(m_dwRecvAppList, 0, sizeof(m_dwRecvAppList));

    return TRUE;
}

BOOL CMaster::AsimovUpdateCZCount(UINT32 dwRoleID, INT32 nCZCount)
{
    if (m_dwTeacherID == dwRoleID)
    {
        m_nTeacherCZ = nCZCount;
    }
    else
    {
        for (INT32 i = 0 ; i < m_nStudentNum; ++i)
        {
            if (m_arrStudentID[i] == dwRoleID)
            {
                m_nStudentCZ[i] = nCZCount;
                break;
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CMaster::AsimovCanCZTeacher()
{
    return m_nTeacherCZ > 0;
}

BOOL CMaster::AsimovCanCZStudent(UINT32 dwRoleID)
{
    INT32   nIndex = 0;
    for (nIndex = 0; nIndex < m_nStudentNum; ++nIndex)
    {
        if (m_arrStudentID[nIndex] == dwRoleID)
            return m_nStudentCZ[nIndex] > 0;
    }

Exit0:
    return FALSE;
}

BOOL CMaster::AsimovUpdateLevel(UINT32 dwRoleID, INT32 nLevel)
{
    if (m_dwTeacherID == dwRoleID)
    {
        m_nTeacherCZ = m_nTeacherCZ + 1;
        if (m_nTeacherCZ < 0)
            m_nTeacherCZ = 0;
        m_nTeacherLevel = nLevel;
    }
    else
    {
        for (INT32 i = 0; i < m_nStudentNum; ++i)
        {
            if (m_arrStudentID[i] == dwRoleID)
            {
                m_nStudentCZ[i] = m_nStudentCZ[i] + 1;
                if (m_nStudentCZ[i] < 0)
                    m_nStudentCZ[i] = 0;
                m_nStudentLevel[i] = nLevel;
            }
        }
    }
    return TRUE;
}

BOOL CMaster::AsimovSetGraduateRoleID(UINT32 dwRoleID)
{
    m_dwGraduateRoleID = dwRoleID;
    return TRUE;
}

UINT32 CMaster::AsimovGetGraduateRoleID()
{
    return m_dwGraduateRoleID;
}

#endif // _ROBOT






