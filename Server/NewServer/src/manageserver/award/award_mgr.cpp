#include "stdafx.h"

#include "award_mgr.h"
#include <time.h>
#include "time_ex.h"

#include "manageserver.h"
#include "role_data_mgr.h"
#include "client/user.h"
#include "onlinerole/online_role_data_mgr.h"
#include "mail/mail_mgr.h"
#include "mail_template_data.h"
#include <algorithm>
#include "client/usermgr.h"

#include "../3rd/award/Include/sotsapi.h"
#include "../3rd/award/Include/senditem.h"
#include "../3rd/award/Include/svccommoninfo.h"
#include "../3rd/award/Include/awardinfo.h"
#include "../3rd/award/Include/sotscommondefine.h"
#include "../3rd/award/Include/svcpd.h"

void CallBackFunction(ULONG handle, char* pServiceName, char *pData, INT32 length);

CAwardManager  CAwardManager::m_Instance;

CAwardManager & CAwardManager::GetInstance()
{
	return m_Instance;
}

CAwardManager::CAwardManager()
{
	m_pRmInitialize = NULL;
	m_pRmTerminate = NULL;
	m_pRmAsynCall = NULL;
	m_pRmGetSotsUniqueId = NULL;
}

CAwardManager::~CAwardManager()
{
	m_pRmInitialize = NULL;
	m_pRmTerminate = NULL;
	m_pRmAsynCall = NULL;
	m_pRmGetSotsUniqueId = NULL;
}

BOOL CAwardManager::Init()
{
	// Initialize UMP 
	char szCurrentDir[MAX_PATH];

	HINSTANCE hInst = LoadLibrary("sotsapi.dll");
	LOG_PROCESS_ERROR(hInst);

	m_pRmInitialize = (FunRmInitialize)GetProcAddress(hInst, "SotsInitialize");
	m_pRmTerminate = (FunRmTerminate)GetProcAddress(hInst, "SotsTerminate");
	m_pRmAsynCall = (FunRmAsynCall)GetProcAddress(hInst, "SotsAsynCall");
	m_pRmGetSotsUniqueId = (FunRmGetSotsUniqueId)GetProcAddress(hInst, "GetSotsUniqueId");

	LOG_PROCESS_ERROR(m_pRmInitialize);
	LOG_PROCESS_ERROR(m_pRmTerminate);
	LOG_PROCESS_ERROR(m_pRmAsynCall);
	LOG_PROCESS_ERROR(m_pRmGetSotsUniqueId);

	GetCurrentDirectory(MAX_PATH, szCurrentDir);

	char szFilePath[MAX_PATH];
	snprintf(szFilePath, MAX_PATH, "%s\\%s", szCurrentDir, "award\\ToolsGetConfig.ini");

	if (m_pRmInitialize(szFilePath, CallBackFunction) != 1)
	{
		LOG_PROCESS_ERROR(FALSE);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAwardManager::UnInit()
{	
	INT32 nRetCode = 0;

	if (m_pRmTerminate)
	{
		nRetCode = m_pRmTerminate();
		LOG_CHECK_ERROR(nRetCode == TRUE);
	}

	m_pRmInitialize = NULL;
	m_pRmTerminate = NULL;
	m_pRmAsynCall = NULL;
	m_pRmGetSotsUniqueId = NULL;

	return TRUE;
}

BOOL CAwardManager::SendAwardReq(MS_ONLINE_ROLE_DATA* pOnlineUserData, INT32 nAwardType, char* pAwardNumber)
{
	time_t llCurrentTime{};
	struct tm* tm{};
	INT32 nRetCode = 0;
	char szTime[COMMON_NAME_LEN];
    struct in_addr  tempAddr;

	GoldAwardAuthenInfoReqDef stAwardReqMsg;
	memset(&stAwardReqMsg, 0, sizeof(stAwardReqMsg));
	
	LOG_PROCESS_ERROR(pOnlineUserData);
	LOG_PROCESS_ERROR(pOnlineUserData->pRoleData);
	LOG_PROCESS_ERROR(pOnlineUserData->pUser);
	LOG_PROCESS_ERROR(pAwardNumber);

	/****************  msg head *****************/ 

	stAwardReqMsg.msgHead.msgType = htonl(GOLDAWARDAUTHENREQ);
	stAwardReqMsg.msgHead.msgLen = htonl(sizeof(stAwardReqMsg));
	stAwardReqMsg.msgHead.callTime = (INT32)time(NULL);
	stAwardReqMsg.msgHead.version = htonl(MSGVERSION);

	m_pRmGetSotsUniqueId(stAwardReqMsg.msgHead.msgId, 33);		// Get unique string

	/******************* base data *********************/

	stAwardReqMsg.msgBody.gameId = htonl(m_cltinfo.gameId);		
	stAwardReqMsg.msgBody.groupId = htonl(m_cltinfo.groupId);	
	stAwardReqMsg.msgBody.areaId = htonl(m_cltinfo.areaId);		
	stAwardReqMsg.msgBody.hostId = htonl(m_cltinfo.hostId);		

	m_pRmGetSotsUniqueId(stAwardReqMsg.msgBody.orderId, 33);		// Obtain a unique business transaction ID

	switch (nAwardType)
	{
	case eratAwardReq_Survey:
		{
			stAwardReqMsg.msgBody.awardType = htonl(1);													// The data center interface requires type 1

			strxcpy(stAwardReqMsg.msgBody.batchId, pAwardNumber, MAX_BATCHID_LEN);						// Batch ID
			strxcpy(stAwardReqMsg.msgBody.awardNum, "0", MAX_ORDERID_LEN);			
		}
		break;
	case eratAwardReq_AwardNumber:
		{
			stAwardReqMsg.msgBody.awardType = htonl(3);													// The data center interface requires type 3
			strxcpy(stAwardReqMsg.msgBody.batchId, "0", MAX_BATCHID_LEN);

			strxcpy(stAwardReqMsg.msgBody.awardNum, pAwardNumber, MAX_ORDERID_LEN);						// Promotion code
		}
		break;
	default:
		{
			LOG_PROCESS_ERROR(FALSE);
		}
		break;
	}


	/******************* user data *********************/

	stAwardReqMsg.msgBody.uidType = htonl(2);																		// Shanda Passport account type: 1 = string account, 2 = numeric account
	strxcpy(stAwardReqMsg.msgBody.userId, (LPCTSTR)pOnlineUserData->pUser->szUserName, MAX_ORDERID_LEN);			
	strxcpy(stAwardReqMsg.msgBody.roleName, (LPCTSTR)pOnlineUserData->pRoleData->szRoleName, MAX_ORDERID_LEN);			
	strxcpy(stAwardReqMsg.msgBody.roleProperty, "0", MAX_PROFESSION_LEN);			

	stAwardReqMsg.msgBody.userGrade = htonl(pOnlineUserData->pRoleData->byLevel);						
	stAwardReqMsg.msgBody.awardGrade = htonl(pOnlineUserData->pRoleData->byLevel);								

	llCurrentTime = (time_t)time(NULL);
	tm = localtime(&llCurrentTime);
	LOG_PROCESS_ERROR(tm);
	
	memset(&szTime, 0, sizeof(szTime));
	snprintf(szTime, COMMON_NAME_LEN, "%d-%d-%d %d:%d:%d", (tm->tm_year+1900), (tm->tm_mon+1), tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	strxcpy(stAwardReqMsg.msgBody.authenTime, (LPCSTR)szTime, MAX_TIME_LEN);

	tempAddr.s_addr = pOnlineUserData->pUser->dwClientIp;
	strxcpy(stAwardReqMsg.msgBody.endpointIp, inet_ntoa(tempAddr), IP_ADDRESS_LEN);

	nRetCode = m_pRmAsynCall("SVCAWARD", (char*)&stAwardReqMsg, sizeof(GoldAwardAuthenInfoReqDef), 0);
	LOG_PROCESS_ERROR(nRetCode > 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAwardManager::SendAwardAck(INT32 nResult, INT32 nUserType, char* pUserID, char* pOrderID, INT32 nReason)
{
	INT32 nRetCode = 0;
	INT32 nMessageLength = 0;

	GoldAwardAckInfoDef stAwardAckMsg;
	memset(&stAwardAckMsg, 0, sizeof(stAwardAckMsg));

	LOG_PROCESS_ERROR(pUserID);
	LOG_PROCESS_ERROR(pOrderID);

	// message header

	stAwardAckMsg.msgHead.msgType = htonl(GOLDAWARDACK);
	stAwardAckMsg.msgHead.callTime = (INT32)time(NULL);
	stAwardAckMsg.msgHead.version = htonl(MSGVERSION);
	stAwardAckMsg.msgHead.msgLen = htonl(sizeof(GoldAwardAckInfoDef));

	m_pRmGetSotsUniqueId(stAwardAckMsg.msgHead.msgId, 33);

	nMessageLength = sizeof(GoldAwardAckInfoDef) - 4 + 0;
	stAwardAckMsg.msgHead.msgLen = htonl(nMessageLength);

	// message body

	stAwardAckMsg.msgBody.uidType = nUserType;																	
	strxcpy(stAwardAckMsg.msgBody.userId, (LPCTSTR)pUserID, MAX_ORDERID_LEN);
	strxcpy(stAwardAckMsg.msgBody.orderId, pOrderID, MAX_ORDERID_LEN);

	if (nResult > 0)
	{
        stAwardAckMsg.msgBody.confirm = htonl(nReason);
		strxcpy(stAwardAckMsg.msgBody.description, "success", sizeof(stAwardAckMsg.msgBody.description));
	}
	else
	{
        stAwardAckMsg.msgBody.confirm = htonl(nReason);
		strxcpy(stAwardAckMsg.msgBody.description, "fail", sizeof(stAwardAckMsg.msgBody.description));
	}

	nRetCode = m_pRmAsynCall(SVCAWARDITEM, (char*)&stAwardAckMsg, nMessageLength, 4);
	LOG_PROCESS_ERROR(nRetCode > 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAwardManager::SendS2cAck(char* pOrderID, char* pMsgID, INT32 nVersion, INT32 nResult)
{
	INT32 nRetCode = 0;

	ItemS2CInfoResDef stS2cInfoResMsg;
	memset(&stS2cInfoResMsg, 0, sizeof(stS2cInfoResMsg));

	LOG_PROCESS_ERROR(pOrderID);
	LOG_PROCESS_ERROR(pMsgID);

	// message header
	stS2cInfoResMsg.msgHead.msgType = htonl(ITEMS2CRES);
	stS2cInfoResMsg.msgHead.callTime = (INT32)time(NULL);
	stS2cInfoResMsg.msgHead.version = nVersion;
	stS2cInfoResMsg.msgHead.msgLen = htonl(sizeof(ItemS2CInfoResDef));
	strxcpy(stS2cInfoResMsg.msgHead.msgId, pMsgID, MAX_ORDERID_LEN+1);

	// message body
	strxcpy(stS2cInfoResMsg.msgBody.orderId, pOrderID, MAX_ORDERID_LEN+1);
	stS2cInfoResMsg.msgBody.result = htonl(nResult);

	if (nResult < 0)
	{
		strxcpy(stS2cInfoResMsg.msgBody.desc, "fail", 64);
	}
	else
	{
		strxcpy(stS2cInfoResMsg.msgBody.desc, "succ", 64);
	}

	nRetCode = m_pRmAsynCall(SVCSENDITEM, (char*)&stS2cInfoResMsg, sizeof(ItemS2CInfoResDef), 4);
	LOG_PROCESS_ERROR(nRetCode > 0);

	return TRUE;
Exit0:
	return FALSE;
}

void _OnAwardItem(char *pData, INT32 length)
{
    GoldAwardAuthenInfoResDef* pResDefMessage = (GoldAwardAuthenInfoResDef*)pData;
    INT32 nRetCode = ntohl(pResDefMessage->msgBody.result);
    if (nRetCode == 0 || nRetCode == 1)
    {
        INT32 nItemCount = 0;
        INT32 nLength = 0;
        ItemInfoDef* pItemInfo = NULL;

        MS_ROLE_DATA* pRoleData = CRoleDataMgr::GetInstance().FindRoleByName(pResDefMessage->msgBody.roleName);
        if (pRoleData == NULL)
        {
            INF("_OnAwardItem: Find Role Failed: %s", pResDefMessage->msgBody.roleName);
            nRetCode = CAwardManager::GetInstance().SendAwardAck(FALSE, pResDefMessage->msgBody.uidType, pResDefMessage->msgBody.userId, pResDefMessage->msgBody.orderId, -1);			// fail - ack 
            LOG_CHECK_ERROR(nRetCode);
            return;
        }

        nItemCount = ntohl(pResDefMessage->msgBody.itemKindCount);
        LOG_CHECK_ERROR(nItemCount <= 7);

        for (INT32 nIndex = 0; nIndex < nItemCount; nIndex++)
        {
            INT32 nItemLength = 0;

            pItemInfo = (ItemInfoDef*)(pResDefMessage->msgBody.itemInfo + nLength);
            LOG_PROCESS_ERROR(pItemInfo);

            nRetCode = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(pRoleData, pResDefMessage->msgBody.pkgName, "",
                atoi(pItemInfo->itemId), ntohl(pItemInfo->itemNum));
            LOG_CHECK_ERROR(nRetCode);
            
            INF("Award : Send item %s(%d) to role %s with type SVCAWARDITEM", pItemInfo->itemId, ntohl(pItemInfo->itemNum), pRoleData->szRoleName);

            if ((ntohl(pItemInfo->itemAttrLen) % 4) == 0)
            {
                nItemLength = ntohl(pItemInfo->itemAttrLen);
            }
            else
            {
                nItemLength = (ntohl(pItemInfo->itemAttrLen) / 4 + 1) * 4;
            }

            nLength = nLength + sizeof(ItemInfoDef) - 4 + nItemLength;
        }

        // ack
        nRetCode = CAwardManager::GetInstance().SendAwardAck(1, pResDefMessage->msgBody.uidType, pResDefMessage->msgBody.userId, pResDefMessage->msgBody.orderId, 0);		// success - ack 
        LOG_CHECK_ERROR(nRetCode);
    }
    else
    {
        MS_ONLINE_ROLE_DATA* pOnlineRoleData = COnlineRoleDataMgr::GetInstance().FindOnlineRoleByName(pResDefMessage->msgBody.roleName);
        if (pOnlineRoleData)
            CMessageHandler::GetInstance().DoErrorNotify(pOnlineRoleData->dwCliSessionID, errAwardFailed, nRetCode, 0);
    }

Exit0:
    return;
}

void _OnSendItem(char *pData, INT32 length)
{
	const MAIL_TEMPLATE_DATA* mail_data{};
	INT32 nResult = -1;
	INT32 nRetCode = 0;
	MsgHeadDef* pHead = (MsgHeadDef*)pData;
	INT32 msgType = ntohl(pHead->msgType);
	INT32 nLength = 0;
	INT32 nItemCount = 0;
	ItemS2CInfoReqDef* pItemReqMsg = (ItemS2CInfoReqDef*)pData;
	ItemInfoDef* pItemInfo = NULL;
	MS_USER* pUser = NULL;
	MS_ROLE_DATA* pRoleData = NULL;
	VecSerialNumber::iterator Iter;

	LOG_PROCESS_ERROR(pData);

    pRoleData = CRoleDataMgr::GetInstance().FindRoleByName(pItemReqMsg->msgBody.roleName);
    LOG_PROCESS_ERROR_RET_CODE(pRoleData, -3);

	nItemCount = ntohl(pItemReqMsg->msgBody.itemKindCount);
	LOG_PROCESS_ERROR_RET_CODE(nItemCount < 7, -5);

	mail_data = CDataManager<MAIL_TEMPLATE_DATA>::GetInstance().GetData(mtiAward);
	LOG_PROCESS_ERROR(mail_data);

	for (INT32 nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		INT32 nItemLength = 0;
		pItemInfo = (ItemInfoDef*)(pItemReqMsg->msgBody.itemInfo + nLength);

		nRetCode = CMailMgr::GetInstance().SendSystemMailWithSimpleAttach(pRoleData, mail_data->m_szMailSubject, 
			mail_data->m_szMailContent, atoi(pItemInfo->itemId), ntohl(pItemInfo->itemNum));
		LOG_CHECK_ERROR(nRetCode);

		INF("Award : Send item %s(%d) to role %s with type SVCSENDITEM", pItemInfo->itemId, ntohl(pItemInfo->itemNum), pRoleData->szRoleName);

		if ((ntohl(pItemInfo->itemAttrLen) % 4) == 0)
		{
			nItemLength = ntohl(pItemInfo->itemAttrLen);
		}
		else
		{
			nItemLength = (ntohl(pItemInfo->itemAttrLen) / 4 + 1) * 4;
		}

		nLength = nLength + sizeof(ItemInfoDef) - 4 + nItemLength;
	}

	nResult = 0;

Exit0:
	if (pItemReqMsg)
	{
		nRetCode = CAwardManager::GetInstance().SendS2cAck(pItemReqMsg->msgBody.orderId,
			pItemReqMsg->msgHead.msgId, pItemReqMsg->msgHead.version, nResult);
		LOG_CHECK_ERROR(nRetCode);
	}

	return;
}

void CallBackFunction(ULONG handle, char* pServiceName, char *pData, INT32 length)
{
	INT32 nRetCode = 0;
	INT32 nRet = FALSE;

    LOG_PROCESS_ERROR(pServiceName);
    LOG_PROCESS_ERROR(pData);

	if (strcmp(pServiceName, SVCSOTSSYSTEM) == 0)
	{
        SotsClientInfoDef* p = (SotsClientInfoDef*)pData;

		if (p->msgHead.msgType == TYPESOTSCLIENTINFO)
		{
			memcpy(&(CAwardManager::GetInstance().GetClientInfo()), &p->msgBody,sizeof(SOTSCLIENTINFO));
		}
		return;
	}
	else if (strcmp(pServiceName, SVCAWARDITEM) == 0)   // Reward claim response
	{
        _OnAwardItem(pData, length);
	}
	else if (strcmp(pServiceName, SVCSENDITEM) == 0)    // Item delivery
	{
		_OnSendItem(pData, length);
	}
	else if (strcmp(pServiceName, SVCPD) == 0)          // Personalized reward claim interface
	{
		LOG_PROCESS_ERROR(FALSE);
	}
	else
	{
		LOG_PROCESS_ERROR(FALSE);
	}
		
Exit0:
	return;
}