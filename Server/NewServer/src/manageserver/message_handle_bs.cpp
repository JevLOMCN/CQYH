#include "stdafx.h"
#include "message_handler.h"
#include "ms_session_mgr.h"
#include "b2m_message.h"
#include "m2b_message.h"

using namespace InternalMessage;

void CMessageHandler::OnBSRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SBSSession* pBSSession{};
	B2M_REGISTER* pMsg{};
	INT32 nRetCode = 0;

    //check
    SBSSession& rBSSession = CMSSessionMgr::GetInstance().GetBSSession();
    LOG_PROCESS_ERROR( rBSSession.pSocketSession == pSocketSession );

    pBSSession = (SBSSession*)pSocketSession->GetUserData();
    LOG_PROCESS_ERROR( pBSSession );
    LOG_PROCESS_ERROR( pBSSession->pSocketSession == pSocketSession );
    LOG_PROCESS_ERROR(CMSSessionMgr::GetInstance().GetServerState() != sgsClosing);

	pMsg = (B2M_REGISTER*)pPacketBuffer;

	m_ServerInfo.BS.dwAddr = pMsg->dwAddr;
	m_ServerInfo.BS.wPort = pMsg->wPort;

	nRetCode = DoBSRegisterAck();
	LOG_PROCESS_ERROR(nRetCode);

Exit0:
	return;
}

void CMessageHandler::OnBSCloseAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	SBSSession* pBSSession = (SBSSession*)pSocketSession->GetUserData();
	LOG_PROCESS_ERROR(pBSSession);

	return;
Exit0:
	if (pSocketSession)
		pSocketSession->Close();
	return;
}

void CMessageHandler::OnBSReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;

	m_ServerInfo.BS.byReady = true;
	INF("BS is ready");

	nRetCode = DoServerInfo();
	LOG_PROCESS_ERROR(nRetCode);

	_CheckAllReady();
Exit0:
	return;
}

BOOL CMessageHandler::DoBSRegisterAck()
{
	INT32 nRetCode = 0;
	
	M2B_REGISTER_ACK* pMsg = (M2B_REGISTER_ACK*)m_pPacketAllocator->Alloc(sizeof(M2B_REGISTER_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = m2b_register_ack;
	nRetCode = _SendBS((PACKET_BUFFER*)pMsg, sizeof(M2B_REGISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}