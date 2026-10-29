#include "stdafx.h"
#include "external_message_handler.h"

#include "time_ex.h"
#include "zlib.h"

#include "server_config.h"
#include "trie.h"
#include "censored_word_list.h"
#include "statis_info.h"
#include "game_world_config.h"
#include "role_db_def.h"

#include "gateserver.h"
#include "gt_session_mgr.h"
#include "internal_message_handler.h"
#include "c2g_message.h"
#include "g2c_message.h"
#include "c2m_message.h"
#include "m2c_message.h"
#include "c2t_message.h"
#include "t2c_message.h"
#include "external_message_size.h"

#include "utf8.h"
#include <time.h>

#define MAX_WHITE_LIST_SIZE				(1000)
#define MAX_MESSAGE_SIZE_BUFFER_SIZE	(1024)

using namespace ExternalMessage;

INT32 s_nDownMsgCount[max_message_count] = { 0 };
INT32 s_nDownMsgSize[max_message_count] = { 0 };
INT32 s_nUpMsgCount[max_message_count] = { 0 };
INT32 s_nUpMsgSize[max_message_count] = { 0 };

static unsigned char s_MessageSizeBuffer[MAX_MESSAGE_SIZE_BUFFER_SIZE];
static INT32 s_nMessageSizeBufferSize = MAX_MESSAGE_SIZE_BUFFER_SIZE;

CExternalMessageHandler CExternalMessageHandler::ms_Instance;

void CExternalMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
	INT32 nResult = errLoginUnknownError;
	INT32 nRetCode = 0;
	CLIENT_SESSION* pSession = NULL;
	UINT32 dwAddr;
	UINT16 wPort;
	struct in_addr addr;

	PROCESS_ERROR_RET_CODE(
		CGTSessionMgr::GetInstance().GetServerState() == sgsAllReady,
		errLoginServerNotReady);

	pSession = CGTSessionMgr::GetInstance().NewClientSession();
	LOG_PROCESS_ERROR(pSession);

	pSocketSession->SetUserData(pSession);

	pSession->pSocketSession = pSocketSession;
	pSession->byShutDownType = csdtClient;
	pSession->byServerGroup = 0;
	pSession->bAdult = TRUE;
	pSession->eState = cssUnAuthed;
	pSession->dwGSSessionID = 0xFFFF;
	pSession->szOASessionID[0] = 0;
	pSession->szPGT[0] = 0;
	pSession->szUserName[0] = 0;
	pSession->wClientPort = 0;
	pSession->bIpWhiteList = FALSE;
	pSession->bUserWhiteList = FALSE;
	pSession->byEncryptCode = 0;
	pSession->byRoleCount = 0;
	pSession->dwClientIp = 0;
	pSession->qwTimer = GetTickCountEx() + g_ServerConfig.GT.nLoginAuthTimeout;

	nRetCode = pSocketSession->GetRemoteAddress(dwAddr, wPort);
	LOG_CHECK_ERROR(nRetCode);
	addr.s_addr = dwAddr;

	for (INT32 nIndex = 1; nIndex < sizeof(g_ServerConfig.Common.IpWhiteList) / sizeof(g_ServerConfig.Common.IpWhiteList[0]); nIndex++)
	{
		if (g_ServerConfig.Common.IpWhiteList[nIndex].szIP[0] == 0)
			break;

		UINT32 dwWhiteAddr = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szIP);
		if (dwWhiteAddr == INADDR_NONE)
			break;
		UINT32 dwWhiteMask = inet_addr(g_ServerConfig.Common.IpWhiteList[nIndex].szMask);

		if ((dwWhiteAddr & dwWhiteMask) == (dwAddr & dwWhiteMask))
		{
			pSession->bIpWhiteList = TRUE;
			break;
		}
	}

	INF("session ptr<%08X>, session <%08X> connected from %s:%d, total %d", (INT32)pSession, pSession->dwSessionID, inet_ntoa(addr), wPort,
		CGTSessionMgr::GetInstance().GetClientSessionCount());

	GT_SYS_LOG_SUCCESS(pSession);

	return;
Exit0:

	if (pSession)
		GT_SYS_LOG_ERROR(pSession);

	if (pSocketSession)
	{
		T2C_ERROR_NOTIFY* pMsg = (T2C_ERROR_NOTIFY*)m_pPacketAllocator->Alloc(sizeof(T2C_ERROR_NOTIFY));
		if (pMsg)
		{
			pMsg->wMsg = t2c_error_notify;
			pMsg->nErrCode = nResult;
			pMsg->nParam1 = pMsg->nParam2 = 0;

			nRetCode = pSocketSession->Send((PACKET_BUFFER*)pMsg, sizeof(T2C_ERROR_NOTIFY), 0);
			LOG_CHECK_ERROR(nRetCode == sizeof(T2C_ERROR_NOTIFY));
			if (sizeof(T2C_ERROR_NOTIFY) != nRetCode)
			{
				m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
			}
		}
	}

	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CExternalMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
	LOG_PROCESS_ERROR(FALSE);

Exit0:
	if (pSocketSession)
		pSocketSession->Close();

	return;
}

void CExternalMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	CLIENT_SESSION* pSession = NULL;
	ExternalMessage::MESSAGE_HEADER* pHeader = (ExternalMessage::MESSAGE_HEADER*)pPacketBuffer;

	g_ChangeStatisInfo(statExternalNetflowIncome, (INT32)dwSize);
	g_ChangeStatisInfo(statExternalPacketIncome, 1);

	if (CGTSessionMgr::GetInstance().GetServerState() != sgsAllReady)
		return;

	LOG_PROCESS_ERROR(pSocketSession);
	LOG_PROCESS_ERROR(pPacketBuffer);
	LOG_PROCESS_ERROR(dwSize > 0);

	pSession = (CLIENT_SESSION*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pSession);

	// Decrypt
	if (pSession->byEncryptCode)
	{
		for (INT32 nIndex = sizeof(FLEX_MESSAGE_HEADER); nIndex < dwSize; nIndex++)
		{
			pPacketBuffer[nIndex] = pPacketBuffer[nIndex] ^ pSession->byEncryptCode;
		}
	}

	// Route data to GS, MS, or LS based on the packet ID
	if (pHeader->wMsg > c2g_message_begin && pHeader->wMsg < ExternalMessage::c2g_message_end)
	{
		LOG_PROCESS_ERROR(pSession->eState == cssPlaying);

		nRetCode = CInternalMessageHandler::GetInstance().DoGSClientMsg(pSession->dwGSSessionID, pSession->dwSessionID, pPacketBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if (pHeader->wMsg < ExternalMessage::c2m_message_end)
	{
		LOG_PROCESS_ERROR(pSession->eState == cssPlaying);

		nRetCode = CInternalMessageHandler::GetInstance().DoMSClientMsg(pSession->dwSessionID, pPacketBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if (pHeader->wMsg < ExternalMessage::c2t_message_end)
	{
		LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);

		nRetCode = (this->*m_MessageFuncList[pHeader->wMsg])(pSession, pPacketBuffer, dwSize);
		LOG_PROCESS_ERROR(nRetCode);

		m_pPacketAllocator->Free(pPacketBuffer);
	}

	if (g_ServerConfig.GT.bLogMessage)
	{
		if (pHeader->wMsg < sizeof(s_nUpMsgCount) / sizeof(s_nUpMsgCount[0]))
		{
			s_nUpMsgCount[pHeader->wMsg]++;
			s_nUpMsgSize[pHeader->wMsg] += dwSize;
		}
	}

	return;
Exit0:
	if (pSession && pHeader)
		CRI("unknown external message id %d size %d from <%08X> state %d", pHeader->wMsg, dwSize, pSession->dwSessionID, pSession->eState);
	else if (pHeader)
		CRI("unknown external message id %d size %d from unknown session", pHeader->wMsg, dwSize);

	if (pPacketBuffer)
		m_pPacketAllocator->Free(pPacketBuffer);

	if (pSocketSession)
		pSocketSession->Close();

	if (pSession)
		pSession->byShutDownType = csdtGT;

	return;
}

void CExternalMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bPassiveShutdown)
{
	INT32 nRetCode = 0;
	CLIENT_SESSION* pSession = NULL;

	LOG_PROCESS_ERROR(pSocketSession);

	pSession = (CLIENT_SESSION*)pSocketSession->GetUserData();
	PROCESS_ERROR(pSession);        // failed to accept

	pSession->pSocketSession = NULL;
	pSession->bConnected = FALSE;

	switch (pSession->eState)
	{
	case cssUnAuthed:
	case cssLSAuthing:
		break;
	case cssMSAuthing:
	case cssAuthed:
	case cssInQueue:
	case cssRoleOperating:
	case cssCreatingRole:
	case cssDeletingRole:
	case cssEnteringGame:
	case cssPlaying:
	case cssReturnSelectRole:
	{
		nRetCode = CInternalMessageHandler::GetInstance().DoMSClientShutdown(pSession->dwSessionID, pSession->byShutDownType);
		LOG_PROCESS_ERROR(nRetCode);
	}
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

Exit0:
	if (pSession)
	{
		UINT32 dwNoGroupSessionID = pSession->dwSessionID;
		SET_GROUP_INDEX_TO_SESSION_ID(dwNoGroupSessionID, 0);
		INF("<%08x> disconnected(no group id<%08X>),disconnect type(%d), total %d", pSession->dwSessionID, dwNoGroupSessionID, pSession->byShutDownType, CGTSessionMgr::GetInstance().GetClientSessionCount() - 1);
		//g_SetStatisInfo(statRoleCount, CGTSessionMgr::GetInstance().GetClientSessionCount() - 1);
		GT_SYS_LOG_SUCCESS(pSession);
		CGTSessionMgr::GetInstance().DelClientSession(pSession);
	}

	return;
}

CExternalMessageHandler::CExternalMessageHandler()
{
	m_pNet = NULL;
	m_pPacketAllocator = NULL;
	m_pClientSocketAcceptor = NULL;
	m_pRoot = NULL;
	memset(m_MessageFuncList, 0, sizeof(m_MessageFuncList));
}

CExternalMessageHandler::~CExternalMessageHandler()
{
	UnInit();
}

BOOL CExternalMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pNet);

	m_pNet = pNet;
	m_pPacketAllocator = pPacketAllocator;

	m_pRoot = trie_create();
	LOG_PROCESS_ERROR(m_pRoot);

	nRetCode = trie_insert(m_pRoot, g_szCensoredWordList);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = trie_insert(m_pRoot, g_GWConfig.szCensoredWordList);
	LOG_PROCESS_ERROR(nRetCode);

	m_MessageFuncList[c2t_login_req] = &CExternalMessageHandler::OnLoginReq;
	m_MessageFuncList[c2t_create_role] = &CExternalMessageHandler::OnCreateRole;
	m_MessageFuncList[c2t_delete_role] = &CExternalMessageHandler::OnDeleteRole;
	m_MessageFuncList[c2t_undelete_role] = &CExternalMessageHandler::OnUnDeleteRole;
	m_MessageFuncList[c2t_forever_delete_role] = &CExternalMessageHandler::OnForeverDeleteRole;
	m_MessageFuncList[c2t_select_role] = &CExternalMessageHandler::OnSelectRole;
	m_MessageFuncList[c2t_ping] = &CExternalMessageHandler::OnPing;

	LOG_PROCESS_ERROR(sizeof(g_ServerConfig.Common.IpWhiteList) / sizeof(g_ServerConfig.Common.IpWhiteList[0]) < MAX_WHITE_LIST_SIZE);
	LOG_PROCESS_ERROR(sizeof(g_ServerConfig.Common.UserWhiteList) / sizeof(g_ServerConfig.Common.UserWhiteList[0]) < MAX_WHITE_LIST_SIZE);

	INF("user white list");
	for (INT32 nIndex = 1; nIndex < sizeof(g_ServerConfig.Common.UserWhiteList) / sizeof(g_ServerConfig.Common.UserWhiteList[0]); nIndex++)
	{
		if (g_ServerConfig.Common.UserWhiteList[nIndex].szUserName[0] == 0)
			break;

		INF("user name : %s", g_ServerConfig.Common.UserWhiteList[nIndex].szUserName);
	}

	INF("ip white list");
	for (INT32 nIndex = 1; nIndex < sizeof(g_ServerConfig.Common.IpWhiteList) / sizeof(g_ServerConfig.Common.IpWhiteList[0]); nIndex++)
	{
		if (g_ServerConfig.Common.IpWhiteList[nIndex].szIP[0] == 0)
			break;

		INF("ip address : %s mask %s", g_ServerConfig.Common.IpWhiteList[nIndex].szIP, g_ServerConfig.Common.IpWhiteList[nIndex].szMask);
	}

	nRetCode = compress(s_MessageSizeBuffer, (uLongf*)&s_nMessageSizeBufferSize, 
		(const Bytef*)g_nS2CMessageSize, (max_message_count - 1) * sizeof(INT32));
	LOG_PROCESS_ERROR(nRetCode == Z_OK);
	INF("external message version %u.%u, crc %08X, message size buffer size %d", 
		EXTERNAL_MESSAGE_MAJOR_VERSION, EXTERNAL_MESSAGE_MINOR_VERSION, g_nMessageVersion, s_nMessageSizeBufferSize);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CExternalMessageHandler::UnInit(void)
{
	INT32 nRetCode = 0;

	m_pNet = NULL;
	m_pPacketAllocator = NULL;

	if (m_pRoot)
	{
		nRetCode = trie_destroy(m_pRoot);
		LOG_CHECK_ERROR(nRetCode);

		m_pRoot = NULL;
	}

	return TRUE;
}

CExternalMessageHandler& CExternalMessageHandler::GetInstance(void)
{
	return ms_Instance;
}

BOOL CExternalMessageHandler::Send(UINT32 dwSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
	UINT32 msgID = 0;
	INT32 nRetCode = 0;

	CLIENT_SESSION* pSession = CGTSessionMgr::GetInstance().FindClientSession(dwSessionID);
	PROCESS_ERROR(pSession);
	LOG_PROCESS_ERROR(pSession->pSocketSession);


	msgID = ((MESSAGE_HEADER*)(pPacketBuffer + nOffset))->wMsg;

	if (g_ServerConfig.GT.bLogMessage)
	{
		if (msgID < sizeof(s_nDownMsgCount) / sizeof(s_nDownMsgCount[0]))
		{
			s_nDownMsgCount[msgID]++;
			s_nDownMsgSize[msgID] += dwSize;
		}
	}

	//LOG_PROCESS_ERROR(msgID < ExternalMessage::max_message_count )
	if (msgID >= ExternalMessage::max_message_count)
	{
		CRI("MSG ID %d too big!", msgID);
		LOG_PROCESS_ERROR(FALSE);
	}

	// Encrypt
	if (pSession->byEncryptCode)
	{
		for (INT32 nIndex = nOffset + sizeof(FLEX_MESSAGE_HEADER); nIndex < dwSize + nOffset; nIndex++)
		{
			pPacketBuffer[nIndex] = pPacketBuffer[nIndex] ^ pSession->byEncryptCode;
		}
	}

	nRetCode = pSession->pSocketSession->Send(pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	g_ChangeStatisInfo(statExternalNetflowOutgoing, (INT32)dwSize);
	g_ChangeStatisInfo(statExternalPacketOutgoing, 1);

	return TRUE;
Exit0:
	//CRI("CExternalMessageHandler::Send failed, dwSessionID=%x", dwSessionID);
	return FALSE;
}

inline BOOL CExternalMessageHandler::_CheckCensoredWord(const char* pcszText)
{
	return trie_search(m_pRoot, pcszText);
}

void CExternalMessageHandler::DoErrorNotify(UINT32 dwSessionID, INT32 nErrorCode, INT32 nParam1, INT32 nParam2)
{
	INT32 nRetCode = 0;

	T2C_ERROR_NOTIFY* pMsg = (T2C_ERROR_NOTIFY*)m_pPacketAllocator->Alloc(sizeof(T2C_ERROR_NOTIFY));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_error_notify;
	pMsg->nErrCode = nErrorCode;
	pMsg->nParam1 = nParam1;
	pMsg->nParam2 = nParam2;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_ERROR_NOTIFY));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;
}

void CExternalMessageHandler::DoLoginAck(UINT32 dwSessionID)
{
	INT32 nRetCode = 0;
	CLIENT_SESSION* pSession = NULL;

	T2C_LOGIN_ACK* pMsg = (T2C_LOGIN_ACK*)m_pPacketAllocator->Alloc(sizeof(T2C_LOGIN_ACK) + s_nMessageSizeBufferSize);
	LOG_PROCESS_ERROR(pMsg);

	pSession = CGTSessionMgr::GetInstance().FindClientSession(dwSessionID);
	PROCESS_ERROR(pSession);

	pMsg->wMsg = t2c_login_ack;
	pMsg->wSize = sizeof(T2C_LOGIN_ACK) + s_nMessageSizeBufferSize;

	strxcpy(pMsg->szUserName, pSession->szUserName, USER_NAME_LEN);

	//memset(pMsg->szUserPassword, 0, USER_PASSWORD_LEN);    // Default code
	strxcpy(pMsg->szUserPassword, pSession->szPassword, USER_PASSWORD_LEN);    // Custom code

	pMsg->bAdult = pSession->bAdult;
	pMsg->byEncryptCode = g_ServerConfig.GT.nEncryptCode;
	pMsg->dwClientOption = g_GWConfig.dwClientOption;    // Previously gtconfig.lua; now gameworldconfig.lua
	memcpy(pMsg->szMessageSize, s_MessageSizeBuffer, s_nMessageSizeBufferSize);

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	pSession->byEncryptCode = (UINT8)g_ServerConfig.GT.nEncryptCode;

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;

}

void CExternalMessageHandler::DoSyncRoleList(UINT32 dwSessionID, INT32 nRoleCount, char BaseData[])
{
	INT32 nRetCode = 0;
	ROLE_DB_BASE_DATA* pBaseData = NULL;

	T2C_SYNC_ROLE_LIST* pMsg = (T2C_SYNC_ROLE_LIST*)m_pPacketAllocator->Alloc(sizeof(T2C_SYNC_ROLE_LIST));
	LOG_PROCESS_ERROR(pMsg);

	pBaseData = (ROLE_DB_BASE_DATA*)BaseData;

	pMsg->wMsg = t2c_sync_role_list;
	pMsg->byCount = nRoleCount;
	for (INT32 nIndex = 0; nIndex < nRoleCount; nIndex++)
	{
		pMsg->RoleData[nIndex].dwRoleID = pBaseData[nIndex].dwRoleID;
		strxcpy(pMsg->RoleData[nIndex].szRoleName, pBaseData[nIndex].szRoleName, ROLE_NAME_LEN);
        strxcpy(pMsg->RoleData[nIndex].szGuildName, pBaseData[nIndex].szGuildName, GUILD_NAME_LEN);
		pMsg->RoleData[nIndex].byClass = pBaseData[nIndex].byClass;
		pMsg->RoleData[nIndex].bySex = pBaseData[nIndex].bySex;
		pMsg->RoleData[nIndex].byHairType = pBaseData[nIndex].byHairType;
		pMsg->RoleData[nIndex].byHairColor = pBaseData[nIndex].byHairColor;
		pMsg->RoleData[nIndex].byFaceType = pBaseData[nIndex].byFaceType;
		pMsg->RoleData[nIndex].bySkinColor = pBaseData[nIndex].bySkinColor;
		pMsg->RoleData[nIndex].byLevel = pBaseData[nIndex].byLevel;
		pMsg->RoleData[nIndex].nSceneTempID = pBaseData[nIndex].nSceneTempID;
		pMsg->RoleData[nIndex].byRefineFx = pBaseData[nIndex].byRefineFx;
		pMsg->RoleData[nIndex].dwSaveTime = pBaseData[nIndex].dwSaveTime;
		pMsg->RoleData[nIndex].dwDeleteTime = pBaseData[nIndex].dwDeleteTime;

		memcpy(pMsg->RoleData[nIndex].nAvatar, pBaseData[nIndex].nAvatar,
			sizeof(pMsg->RoleData[nIndex].nAvatar));
	}

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_SYNC_ROLE_LIST));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;

}

void CExternalMessageHandler::DoSelectRoleAck(UINT32 dwSessionID, UINT32 dwRoleID)
{
	INT32 nRetCode = 0;

	T2C_SELECT_ROLE_ACK* pMsg = (T2C_SELECT_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(T2C_SELECT_ROLE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_select_role_ack;
	pMsg->dwRoleID = dwRoleID;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_SELECT_ROLE_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;

}

void CExternalMessageHandler::DoCreateRoleAck(UINT32 dwSessionID, CHAR BaseData[])
{
	INT32 nRetCode = 0;
	ROLE_DB_BASE_DATA* pBaseData = NULL;

	T2C_CREATE_ROLE_ACK* pMsg = (T2C_CREATE_ROLE_ACK*)m_pPacketAllocator->Alloc(sizeof(T2C_CREATE_ROLE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pBaseData = (ROLE_DB_BASE_DATA*)BaseData;

	pMsg->wMsg = t2c_create_role_ack;

	pMsg->RoleData.dwRoleID = pBaseData->dwRoleID;
	strxcpy(pMsg->RoleData.szRoleName, pBaseData->szRoleName, ROLE_NAME_LEN);
    pMsg->RoleData.szGuildName[0] = 0;
	pMsg->RoleData.byClass = pBaseData->byClass;
	pMsg->RoleData.bySex = pBaseData->bySex;
	pMsg->RoleData.byHairType = pBaseData->byHairType;
	pMsg->RoleData.byHairColor = pBaseData->byHairColor;
	pMsg->RoleData.byFaceType = pBaseData->byFaceType;
	pMsg->RoleData.bySkinColor = pBaseData->bySkinColor;
	pMsg->RoleData.byLevel = pBaseData->byLevel;

	pMsg->RoleData.nSceneTempID = pBaseData->nSceneTempID;

	memcpy(pMsg->RoleData.nAvatar, pBaseData->nAvatar, sizeof(pMsg->RoleData.nAvatar));

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_CREATE_ROLE_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);

	return;
}

void CExternalMessageHandler::DoDeleteRoleAck(UINT32 dwSessionID, UINT32 dwRoleID)
{
	INT32 nRetCode = 0;

	T2C_DELETE_ROLE_ACK* pMsg = (T2C_DELETE_ROLE_ACK*)m_pPacketAllocator->Alloc(
		sizeof(T2C_DELETE_ROLE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_delete_role_ack;
	pMsg->dwRoleID = dwRoleID;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_DELETE_ROLE_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;
}

void CExternalMessageHandler::DoUnDeleteRoleAck(UINT32 dwSessionID, UINT32 dwRoleID)
{
	INT32 nRetCode = 0;

	T2C_UNDELETE_ROLE_ACK* pMsg = (T2C_UNDELETE_ROLE_ACK*)m_pPacketAllocator->Alloc(
		sizeof(T2C_UNDELETE_ROLE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_undelete_role_ack;
	pMsg->dwRoleID = dwRoleID;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_UNDELETE_ROLE_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;
}

void CExternalMessageHandler::DoForeverDeleteRoleAck(UINT32 dwSessionID, UINT32 dwRoleID)
{
	INT32 nRetCode = 0;

	T2C_FOREVER_DELETE_ROLE_ACK* pMsg = (T2C_FOREVER_DELETE_ROLE_ACK*)m_pPacketAllocator->Alloc(
		sizeof(T2C_FOREVER_DELETE_ROLE_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_forever_delete_role_ack;
	pMsg->dwRoleID = dwRoleID;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_FOREVER_DELETE_ROLE_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;
}

void CExternalMessageHandler::DoReturnSelectRole(UINT32 dwSessionID)
{
	INT32 nRetCode = 0;

	T2C_RETURN_SELECT_ROLE* pMsg = (T2C_RETURN_SELECT_ROLE*)m_pPacketAllocator->Alloc(sizeof(T2C_RETURN_SELECT_ROLE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_return_select_role;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_RETURN_SELECT_ROLE));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;
}


void CExternalMessageHandler::DoPingAck(UINT32 dwSessionID, UINT32 dwPingTick)
{
	INT32 nRetCode = 0;

	T2C_PING_ACK* pMsg = (T2C_PING_ACK*)m_pPacketAllocator->Alloc(
		sizeof(T2C_PING_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = t2c_ping_ack;
	pMsg->dwPingTick = dwPingTick;

	nRetCode = Send(dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(T2C_PING_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return;
}


BOOL CExternalMessageHandler::OnLoginReq(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	SYST("OnLoginReq  1");

	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	UINT32 dwOldSessionID = 0;
	C2T_LOGIN_REQ* pMsg = (C2T_LOGIN_REQ*)pPacketBuffer;

	LOG_PROCESS_ERROR(pSession->eState == cssUnAuthed);

	// Write a null terminator
	pMsg->szUserName[USER_NAME_LEN - 1] = 0;
	pMsg->szPassword[USER_PASSWORD_LEN - 1] = 0;

	// Convert uppercase letters to lowercase
	_strlwr(pMsg->szUserName);
	_strlwr(pMsg->szPassword);

	// Copy
	strxcpy(pSession->szUserName, pMsg->szUserName, USER_NAME_LEN);
	strxcpy(pSession->szPassword, pMsg->szPassword, USER_PASSWORD_LEN);

	LOG_PROCESS_ERROR(pMsg->byGroup < MAX_SERVER_GROUP_COUNT);
	LOG_PROCESS_ERROR(g_ServerConfig.MS.nInitGroup & (1 << pMsg->byGroup));
	PROCESS_ERROR_RET_CODE(pMsg->wMajorVersion == EXTERNAL_MESSAGE_MAJOR_VERSION, errLoginErrorVersion);

	SYST("OnLoginReq  2");
	if (!CGTSessionMgr::GetInstance().IsOpened(pMsg->byGroup))
	{
		PROCESS_ERROR_RET_CODE(pSession->bIpWhiteList, errLoginServerNotReady);
	}
	pSession->byServerGroup = pMsg->byGroup;
	if (g_ServerConfig.GroupConfig[pMsg->byGroup].nMappedGroupID > 0 &&
		g_ServerConfig.GroupConfig[pMsg->byGroup].nMappedGroupID < MAX_SERVER_GROUP_COUNT)
	{
		pMsg->byGroup = g_ServerConfig.GroupConfig[pMsg->byGroup].nMappedGroupID;
	}

	dwOldSessionID = pSession->dwSessionID;
	SET_GROUP_INDEX_TO_SESSION_ID(pSession->dwSessionID, pMsg->byGroup);

	SYST("OnLoginReq  3");
	nRetCode = CInternalMessageHandler::GetInstance().DoLSLoginReq(
		pSession->dwSessionID, pMsg->szUserName, pMsg->szPassword, pMsg->szSessionID);
	LOG_PROCESS_ERROR(nRetCode);

	SYST("OnLoginReq  4");
	pSession->eState = cssLSAuthing;
	pSession->qwTimer = GetTickCountEx() + g_ServerConfig.GT.nLoginAuthTimeout;

	nRetCode = pSession->pSocketSession->GetRemoteAddress(pSession->dwClientIp, pSession->wClientPort);
	LOG_CHECK_ERROR(nRetCode);

	SYST("OnLoginReq  5");
	strxcpy(pSession->szMac, pMsg->szMac, MAC_ADDRESS_LEN);

	SYST("session<%08X>, state[%d], message %u.%u, crc %08X after CExternalMessageHandler::OnLoginReq success.",
		pSession->dwSessionID, pSession->eState, pMsg->wMajorVersion, pMsg->wMinorVersion, pMsg->dwCRC);
	SYST("OnLoginReq  6");

	return TRUE;
Exit0:
	if (nResult != errUnknownError)
		DoErrorNotify(pSession->dwSessionID, nResult, 0, 0);

	GT_SYS_LOG_ERROR(pSession);
	return FALSE;
}

BOOL CExternalMessageHandler::OnSelectRole(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	C2T_SELECT_ROLE* pMsg = (C2T_SELECT_ROLE*)pPacketBuffer;
	INT32 nIndex = 0;

	// An invalid state here may indicate a cheating attempt or a client bug
	LOG_PROCESS_ERROR(pSession->eState == cssRoleOperating);

	LOG_PROCESS_ERROR(pSession->byRoleCount > 0);

	for (; nIndex < pSession->byRoleCount; nIndex++)
	{
		if (pSession->RoleList[nIndex].dwRoleID == pMsg->dwRoleID)
			break;
	}
	// Not found: possible cheating attempt or client bug
	LOG_PROCESS_ERROR(nIndex < pSession->byRoleCount);
	LOG_PROCESS_ERROR(pSession->RoleList[nIndex].byDeleted == 0);

	nRetCode = CInternalMessageHandler::GetInstance().DoMSQueryScene(pSession->dwSessionID, pMsg->dwRoleID);
	LOG_PROCESS_ERROR(nRetCode);

	pSession->eState = cssEnteringGame;

	GT_SYS_LOG_SUCCESS(pSession);
	return TRUE;
Exit0:

	GT_SYS_LOG_ERROR(pSession);
	return FALSE;
}

BOOL CExternalMessageHandler::OnCreateRole(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	wchar_t wcName[ROLE_NAME_LEN];
	wchar_t* pwcName = wcName;
	INT32 nNameStringLen = 0;
	INT32 nUsableRoles = 0;
	CreateRoleParam p;
	C2T_CREATE_ROLE* pMsg = (C2T_CREATE_ROLE*)pPacketBuffer;

	// An invalid state here may indicate a cheating attempt or a client bug
	LOG_PROCESS_ERROR(pSession->eState == cssRoleOperating);

	LOG_PROCESS_ERROR(strlen(pMsg->szRoleName) > 0);
	LOG_PROCESS_ERROR(strlen(pMsg->szRoleName) < ROLE_NAME_LEN);

	LOG_PROCESS_ERROR(pMsg->byHairType < MAX_ROLE_HAIR_TYPE);
	LOG_PROCESS_ERROR(pMsg->byHairColor < MAX_ROLE_HAIR_COLOR);
	LOG_PROCESS_ERROR(pMsg->byFaceType < MAX_ROLE_FACE_TYPE);
	LOG_PROCESS_ERROR(pMsg->bySkinColor < MAX_ROLE_SKIN_COLOR);

	if (pSession->byRoleCount >= MAX_ROLE_PER_USER)
	{
		DoErrorNotify(pSession->dwSessionID, errLoginCreateRoleTooMany, 0, 0);
		PROCESS_SUCCESS(TRUE);
	}

	for (UINT8 i = 0; i < pSession->byRoleCount; i++)
	{
		if (pSession->RoleList[i].byDeleted == 0)
			nUsableRoles++;
	}
	if (nUsableRoles >= MAX_USABLE_ROLE_PER_USER)
	{
		DoErrorNotify(pSession->dwSessionID, errLoginCreateRoleTooMany, 0, 0);
		PROCESS_SUCCESS(TRUE);
	}

	if (!pSession->bUserWhiteList && _CheckCensoredWord(pMsg->szRoleName))
	{
		DoErrorNotify(pSession->dwSessionID, errLoginCreateRoleCensoredWord, 0, 0);
		PROCESS_SUCCESS(TRUE);
	}

	memset(wcName, 0, sizeof(wcName));
	nRetCode = utf8_to_wchar(pMsg->szRoleName, strlen(pMsg->szRoleName), wcName, ROLE_NAME_LEN, 0);
	LOG_PROCESS_ERROR(nRetCode > 0);
	LOG_PROCESS_ERROR(nRetCode < ROLE_NAME_LEN);

	while (*pwcName)
	{
		if (*pwcName >= '0' && *pwcName <= 'z')
			nNameStringLen += 2;
		else
			nNameStringLen += 3;
		++pwcName;
	}
	if (nNameStringLen > 24)
	{
		DoErrorNotify(pSession->dwSessionID, errLoginCreateRoleNameLength, 0, 0);
		PROCESS_SUCCESS(TRUE);
	}

	p.bySex = pMsg->bySex;
	p.byClass = pMsg->byClass;
	p.byHairType = pMsg->byHairType;
	p.byHairColor = pMsg->byHairColor;
	p.byFaceType = pMsg->byFaceType;
	p.bySkinColor = pMsg->bySkinColor;

	nRetCode = CInternalMessageHandler::GetInstance().DoMSCheckNewName(pSession->dwSessionID, pMsg->szRoleName, (const char*)&p, sizeof(p));
	if (!nRetCode)
	{
		DoErrorNotify(pSession->dwSessionID, errLoginCreateRoleNameServerNotConnected, 0, 0);
		PROCESS_SUCCESS(TRUE);
	}

	pSession->eState = cssCreatingRole;

Exit1:
	GT_SYS_LOG_SUCCESS(pSession);
	return TRUE;

Exit0:
	GT_SYS_LOG_ERROR(pSession);
	return FALSE;
}

BOOL CExternalMessageHandler::OnDeleteRole(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	C2T_DELETE_ROLE* pMsg = (C2T_DELETE_ROLE*)pPacketBuffer;
	INT32 nIndex = 0;
	INT32 nDeletedRoles = 0;

	// An invalid state here may indicate a cheating attempt or a client bug
	LOG_PROCESS_ERROR(pSession->eState == cssRoleOperating);

	LOG_PROCESS_ERROR(pSession->byRoleCount > 0);

	for (; nIndex < pSession->byRoleCount; nIndex++)
	{
		if (pSession->RoleList[nIndex].dwRoleID == pMsg->dwRoleID)
			break;
	}
	// Not found: possible cheating attempt or client bug
	LOG_PROCESS_ERROR(nIndex < pSession->byRoleCount);
	LOG_PROCESS_ERROR(pSession->RoleList[nIndex].byDeleted == 0);

	for (UINT8 i = 0; i < pSession->byRoleCount; i++)
	{
		if (pSession->RoleList[i].byDeleted == 1)
			nDeletedRoles++;
	}
	LOG_PROCESS_ERROR(nDeletedRoles < MAX_DELETE_ROLE_PER_USER);

	nRetCode = CInternalMessageHandler::GetInstance().DoDBDeleteRole(pSession->dwSessionID, pMsg->dwRoleID);
	LOG_PROCESS_ERROR(nRetCode);

	pSession->eState = cssDeletingRole;

	GT_SYS_LOG_SUCCESS(pSession);
	return TRUE;
Exit0:

	GT_SYS_LOG_ERROR(pSession);
	return FALSE;
}

BOOL CExternalMessageHandler::OnUnDeleteRole(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	C2T_UNDELETE_ROLE* pMsg = (C2T_UNDELETE_ROLE*)pPacketBuffer;
	INT32 nIndex = 0;
	INT32 nUsableRoles = 0;

	// An invalid state here may indicate a cheating attempt or a client bug
	LOG_PROCESS_ERROR(pSession->eState == cssRoleOperating);

	LOG_PROCESS_ERROR(pSession->byRoleCount > 0);

	for (; nIndex < pSession->byRoleCount; nIndex++)
	{
		if (pSession->RoleList[nIndex].dwRoleID == pMsg->dwRoleID)
			break;
	}
	// Not found: possible cheating attempt or client bug
	LOG_PROCESS_ERROR(nIndex < pSession->byRoleCount);
	LOG_PROCESS_ERROR(pSession->RoleList[nIndex].byDeleted == 1);

	for (UINT8 i = 0; i < pSession->byRoleCount; i++)
	{
		if (pSession->RoleList[i].byDeleted == 0)
			nUsableRoles++;
	}
	LOG_PROCESS_ERROR(nUsableRoles < MAX_USABLE_ROLE_PER_USER);

	nRetCode = CInternalMessageHandler::GetInstance().DoDBUnDeleteRole(pSession->dwSessionID, pMsg->dwRoleID);
	LOG_PROCESS_ERROR(nRetCode);

	pSession->eState = cssDeletingRole;

	GT_SYS_LOG_SUCCESS(pSession);
	return TRUE;
Exit0:

	GT_SYS_LOG_ERROR(pSession);
	return FALSE;
}

BOOL CExternalMessageHandler::OnForeverDeleteRole(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	C2T_FOREVER_DELETE_ROLE* pMsg = (C2T_FOREVER_DELETE_ROLE*)pPacketBuffer;
	INT32 nIndex = 0;

	// An invalid state here may indicate a cheating attempt or a client bug
	LOG_PROCESS_ERROR(pSession->eState == cssRoleOperating);

	LOG_PROCESS_ERROR(pSession->byRoleCount > 0);

	for (; nIndex < pSession->byRoleCount; nIndex++)
	{
		if (pSession->RoleList[nIndex].dwRoleID == pMsg->dwRoleID)
			break;
	}
	// Not found: possible cheating attempt or client bug
	LOG_PROCESS_ERROR(nIndex < pSession->byRoleCount);
	LOG_PROCESS_ERROR(pSession->RoleList[nIndex].byDeleted == 1);

	nRetCode = CInternalMessageHandler::GetInstance().DoDBForeverDeleteRole(pSession->dwSessionID, pMsg->dwRoleID);
	LOG_PROCESS_ERROR(nRetCode);

	pSession->eState = cssDeletingRole;

	GT_SYS_LOG_SUCCESS(pSession);
	return TRUE;
Exit0:

	GT_SYS_LOG_ERROR(pSession);
	return FALSE;
}

BOOL CExternalMessageHandler::OnPing(CLIENT_SESSION* pSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	C2T_PING* pMsg = (C2T_PING*)pPacketBuffer;

	if (pSession->eState == cssPlaying)
		pSession->qwTimer = GetTickCountEx() + g_ServerConfig.GT.nPlayingTimeout;

	CExternalMessageHandler::GetInstance().DoPingAck(pSession->dwSessionID, pMsg->dwPingTick);

	return TRUE;
}
