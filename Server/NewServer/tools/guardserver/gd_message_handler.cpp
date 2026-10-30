#include "stdafx.h"
#include "time_ex.h"

#include "guard_config.h"
#include "maintain_tool_version.h"
#include "common_message.h"
#include "gd_session_mgr.h"
#include "gd_message_handler.h"
#include "guardserver.h"
#include "process_mgr.h"
#include "cmd_mgr.h"
#include "guard_packets.h"
#include "statis_info.h"
#include "process_option.h"
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <memory>
#include <format>
#include <limits>
#include <utility>

using namespace InternalMessage;
CGDMessageHandler CGDMessageHandler::ms_Instance;

void CGDMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pSocketSession);

    nRetCode = CGDSessionMgr::GetInstance().AddServerSession(pSocketSession);
    LOG_PROCESS_ERROR(nRetCode);

    INF("a connecting is accepted");

    return;
Exit0:
    if(pSocketSession)
        pSocketSession->Close();
    return;
}

void CGDMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
    LOG_PROCESS_ERROR(FALSE);

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();

    return;
}

void CGDMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
    // Keep the allocator used to accept this packet even if a handler changes
    // its own initialization state. Ownership lasts through the callback.
    const auto release = [allocator = m_pPacketAllocator](PACKET_BUFFER* packet) {
        if (allocator) allocator->Free(packet);
    };
    const std::unique_ptr<PACKET_BUFFER, decltype(release)> packet{pPacketBuffer, release};
    MESSAGE_HEADER header{};
    if (m_receiving && m_pPacketAllocator && pSocketSession && packet && dwSize >= sizeof(header))
    {
        std::memcpy(&header, packet.get(), sizeof(header));
        if (header.wMsg < m_GuardMessageFuncList.size())
        {
            if (const auto handler = m_GuardMessageFuncList[header.wMsg];
                handler && guard_packets::valid(header.wMsg, {packet.get(), dwSize}))
            {
                try
                {
                    (this->*handler)(packet.get(), dwSize, pSocketSession);
                    return;
                }
                catch (const std::exception& error) { ERR("guard message %u failed: %s", header.wMsg, error.what()); }
                catch (...) { ERR("guard message %u threw an exception", header.wMsg); }
            }
        }
    }
    CRI("invalid internal message id %u size %u", header.wMsg, dwSize);
    if (pSocketSession) pSocketSession->Close();
}

void CGDMessageHandler::OnDisconnected(ISocketSession* socket, BOOL)
{
    if (!socket) return;
    m_incomingFile.abort(socket);
    auto* record = static_cast<ServerSession*>(socket->GetUserData());
    socket->SetUserData(nullptr);
    if (!record)
    {
        CGDSessionMgr::GetInstance().RemoveServerSession(socket);
        return;
    }
    if (record->pSocketSession != socket) return;
    const auto previous = *record;
    // Sending the notification may synchronously register a replacement. Clear
    // the old record first and never write through it after that callback.
    record->pSocketSession = nullptr;
    record->dwProcessID = 0;
    record->qwTimer = (std::numeric_limits<UINT64>::max)();
    record->eState = epsUnonnect;
    record->bClosing = false;
    if (previous.dwProcessID)
        LOG_CHECK_ERROR(CProcessMgr::GetInstance().SyncProcessInfo(epsUnonnect, previous.dwProcessID));
    const auto* name = previous.dwType == stMT ? "MT" : GetServerShortName(previous.dwType);
    CRI("%s %u disconnected", name ? name : "unknown", previous.dwSessionID);
}

CGDMessageHandler::CGDMessageHandler()
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;
	m_GuardMessageFuncList.fill(nullptr);
}

CGDMessageHandler::~CGDMessageHandler()
{
    UnInit();
}

BOOL CGDMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator)
{
    if (m_pNet || m_pPacketAllocator) return FALSE;
    LOG_PROCESS_ERROR(pNet);
    LOG_PROCESS_ERROR(pPacketAllocator);

    try
    {
        const auto directory = std::filesystem::current_path().string();
        if (directory.size() >= sizeof(g_GuardConfig.szGuardPath)) return FALSE;
        std::ranges::copy(directory, g_GuardConfig.szGuardPath);
        g_GuardConfig.szGuardPath[directory.size()] = '\0';
    }
    catch (const std::exception& error)
    {
        CRI("cannot initialize guard directory: %s", error.what());
        return FALSE;
    }

    m_pNet = pNet;
    m_pPacketAllocator = pPacketAllocator;
    m_receiving = true;

    //All
    //////////////////////////////////////////////////////////////////////////////////////
    m_GuardMessageFuncList[a2a_ping] = &CGDMessageHandler::OnAllPing;
    m_GuardMessageFuncList[a2u_regrister] = &CGDMessageHandler::OnProcessRegrister;
	m_GuardMessageFuncList[n2u_regrister] = &CGDMessageHandler::OnMTRegrister;

	// from MS
	m_GuardMessageFuncList[m2u_command_ack] = &CGDMessageHandler::OnMSCommandAck;
	m_GuardMessageFuncList[a2u_notify] = &CGDMessageHandler::OnAllNotify;

	// from MT
	m_GuardMessageFuncList[n2u_exec_cmd] = &CGDMessageHandler::OnMTExecCmd;
    m_GuardMessageFuncList[n2u_send_files] = &CGDMessageHandler::OnMTSendFiles;
	m_GuardMessageFuncList[n2u_deploy_server_version] = &CGDMessageHandler::OnMTDeployServerVersion;
	m_GuardMessageFuncList[n2u_hot_update_patch] = &CGDMessageHandler::OnMTHotUpdatePatch;
    /////////////////////////////////////////////////////////////////////////////////////

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGDMessageHandler::UnInit(void)
{
    BeginShutdown();
    m_incomingFile.abort();
    m_pNet = nullptr;
    m_pPacketAllocator = nullptr;
    m_GuardMessageFuncList.fill(nullptr);
    return TRUE;
}

CGDMessageHandler& CGDMessageHandler::GetInstance(void)
{
    return ms_Instance;
}

BOOL CGDMessageHandler::_SendGT(UINT32 dwGTSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
    INT32 nRetCode = 0;

    SGTSession* pGTSession = CGDSessionMgr::GetInstance().GetGTSession(dwGTSessionID);
	LOG_PROCESS_ERROR(pGTSession && pGTSession->pSocketSession);

    nRetCode = pGTSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendGS(UINT32 dwGSSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
    INT32 nRetCode = 0;

    SGSSession* pGSSession = CGDSessionMgr::GetInstance().GetGSSession(dwGSSessionID);
    LOG_PROCESS_ERROR(pGSSession);
    LOG_PROCESS_ERROR(pGSSession->pSocketSession);

    nRetCode = pGSSession->pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendLS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
    INT32 nRetCode = 0;

    SLSSession& rLSSession = CGDSessionMgr::GetInstance().GetLSSession();
    LOG_PROCESS_ERROR(rLSSession.pSocketSession);

    nRetCode = rLSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendDB(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
    INT32 nRetCode = 0;

    SDBSession& rDBSession = CGDSessionMgr::GetInstance().GetDBSession();
    LOG_PROCESS_ERROR(rDBSession.pSocketSession);

    nRetCode = rDBSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
    INT32 nRetCode = 0;

    SMSSession& rMSSession = CGDSessionMgr::GetInstance().GetMSSession();
    LOG_PROCESS_ERROR(rMSSession.pSocketSession);

    nRetCode = rMSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendBS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
	INT32 nRetCode = 0;

	SBSSession& rBSSession = CGDSessionMgr::GetInstance().GetBSSession();
	LOG_PROCESS_ERROR(rBSSession.pSocketSession);

	nRetCode = rBSSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendDL(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{	
	INT32 nRetCode = 0;

	SDLSession& rDLSession = CGDSessionMgr::GetInstance().GetDLSession();
	LOG_PROCESS_ERROR(rDLSession.pSocketSession);

	nRetCode = rDLSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendMT(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
	INT32 nRetCode = 0;

	SMTSession& rMTSession = CGDSessionMgr::GetInstance().GetMTSession();
	LOG_PROCESS_ERROR(rMTSession.pSocketSession);

	nRetCode = rMTSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendGM(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
    INT32 nRetCode = 0;

    SGMSession& rGMSession = CGDSessionMgr::GetInstance().GetGMSession();
    LOG_PROCESS_ERROR(rGMSession.pSocketSession);

    nRetCode = rGMSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
    LOG_PROCESS_ERROR(nRetCode == dwSize);

    return TRUE;
Exit0:
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_SendMM(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
try
{
	INT32 nRetCode = 0;

	SMMSession& rMMSession = CGDSessionMgr::GetInstance().GetMMSession();
	LOG_PROCESS_ERROR(rMMSession.pSocketSession);

	nRetCode = rMMSession.pSocketSession->Send((PACKET_BUFFER*)pPacketBuffer, dwSize, nOffset);
	LOG_PROCESS_ERROR(nRetCode == dwSize);

	return TRUE;
Exit0:
	return FALSE;
}
catch (const std::exception& error)
{
    ERR("guard send failed: %s", error.what());
    return FALSE;
}
catch (...)
{
    ERR("guard send threw an exception");
    return FALSE;
}

BOOL CGDMessageHandler::_BroadcastGT(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pMsg = NULL;

    for(INT32 i = 1; i < MAX_GT_SESSION_COUNT; ++i)
    {
        SGTSession* pGTSession = CGDSessionMgr::GetInstance().GetGTSession(i);
        LOG_PROCESS_ERROR(pGTSession);
        LOG_PROCESS_ERROR(pGTSession->pSocketSession);

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGTSession->pSocketSession->Send(pMsg, dwSize, nOffset);
        LOG_PROCESS_ERROR(nRetCode == dwSize);

        pMsg = NULL;
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    if(pMsg)
        m_pPacketAllocator->Free(pMsg);
    if(pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);
    return FALSE;
}

BOOL CGDMessageHandler::_BroadcastGS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pMsg = NULL;

    for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
    {
        SGSSession* pGSSession = CGDSessionMgr::GetInstance().GetGSSession(i);
        LOG_PROCESS_ERROR(pGSSession);

        if(!pGSSession->pSocketSession)
            continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGSSession->pSocketSession->Send(pMsg, dwSize, nOffset);
        LOG_PROCESS_ERROR(nRetCode == dwSize);

        pMsg = NULL;
    }

    m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    if(pMsg)
        m_pPacketAllocator->Free(pMsg);
    if(pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);
    return FALSE;
}

BOOL CGDMessageHandler::_BroadcastAll(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset)
{
    INT32 nRetCode = 0;
    PACKET_BUFFER* pMsg = NULL;

    for(INT32 i = 1; i < MAX_GT_SESSION_COUNT; ++i)
    {
        SGTSession* pGTSession = CGDSessionMgr::GetInstance().GetGTSession(i);
        LOG_PROCESS_ERROR(pGTSession);

        if (pGTSession->pSocketSession == NULL)
            continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGTSession->pSocketSession->Send(pMsg, dwSize, nOffset);
        LOG_CHECK_ERROR(nRetCode == dwSize);

        pMsg = NULL;
    }

    for(INT32 i = 1; i < MAX_GS_SESSION_COUNT; ++i)
    {
        SGSSession* pGSSession = CGDSessionMgr::GetInstance().GetGSSession(i);
        LOG_PROCESS_ERROR(pGSSession);
        
        if (pGSSession->pSocketSession == NULL)
            continue;

        pMsg = m_pPacketAllocator->Alloc(dwSize);
        LOG_PROCESS_ERROR(pMsg);

        memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

        nRetCode = pGSSession->pSocketSession->Send(pMsg, dwSize, nOffset);
        LOG_PROCESS_ERROR(nRetCode == dwSize);

        pMsg = NULL;
    }
    
    {
        SMSSession& rMSSession = CGDSessionMgr::GetInstance().GetMSSession();
        if (rMSSession.pSocketSession)
        {
            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = rMSSession.pSocketSession->Send(pMsg, dwSize, nOffset);
            LOG_PROCESS_ERROR(nRetCode == dwSize);

            pMsg = NULL;
        }
    }

    {
        SDBSession& rDBSession = CGDSessionMgr::GetInstance().GetDBSession();
        if (rDBSession.pSocketSession)
        {
            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = rDBSession.pSocketSession->Send(pMsg, dwSize, nOffset);
            LOG_PROCESS_ERROR(nRetCode == dwSize);

            pMsg = NULL;
        }
    }

    {
        SLSSession& rLSSession = CGDSessionMgr::GetInstance().GetLSSession();
        if (rLSSession.pSocketSession)
        {
            pMsg = m_pPacketAllocator->Alloc(dwSize);
            LOG_PROCESS_ERROR(pMsg);

            memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

            nRetCode = rLSSession.pSocketSession->Send(pMsg, dwSize, nOffset);
            LOG_PROCESS_ERROR(nRetCode == dwSize);

            pMsg = NULL;
        }
    }

	{
		SBSSession& rBSSession = CGDSessionMgr::GetInstance().GetBSSession();
		if (rBSSession.pSocketSession)
		{
			pMsg = m_pPacketAllocator->Alloc(dwSize);
			LOG_PROCESS_ERROR(pMsg);

			memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

			nRetCode = rBSSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			LOG_PROCESS_ERROR(nRetCode == dwSize);

			pMsg = NULL;
		}
	}

	{
		SMMSession& rMMSession = CGDSessionMgr::GetInstance().GetMMSession();
		if (rMMSession.pSocketSession)
		{
			pMsg = m_pPacketAllocator->Alloc(dwSize);
			LOG_PROCESS_ERROR(pMsg);

			memcpy(pMsg + nOffset, pPacketBuffer + nOffset, dwSize);

			nRetCode = rMMSession.pSocketSession->Send(pMsg, dwSize, nOffset);
			LOG_PROCESS_ERROR(nRetCode == dwSize);

			pMsg = NULL;
		}
	}

    if(pPacketBuffer)
        m_pPacketAllocator->Free(pPacketBuffer);

    return TRUE;
Exit0:
    if(pMsg)
        m_pPacketAllocator->Free(pMsg);
    return FALSE;
}

BOOL CGDMessageHandler::DoMTUpdateStatisInfo(void)
{
    INT32 nRetCode = 0;
    U2N_UPDATE_STATIS_INFO* pMsg = NULL;
    stStatisInfoShm* pShm = nullptr;
    CGDSessionMgr &rSessionMgr = CGDSessionMgr::GetInstance();

    pMsg = (U2N_UPDATE_STATIS_INFO*)m_pPacketAllocator->Alloc(sizeof(U2N_UPDATE_STATIS_INFO));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = u2n_update_statis_info;       
    pMsg->bConnected = TRUE;

    pShm = g_GetStatisInfoShm();
    LOG_PROCESS_ERROR( pShm ); 
    memcpy( &pMsg->oStatisInfo, &pShm->oInfo, sizeof(pMsg->oStatisInfo) );

    nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(U2N_UPDATE_STATIS_INFO));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CGDMessageHandler::DoAllPing(void)
{
    INT32 nRetCode = 0;
    A2A_PING* pMsg = NULL;
    CGDSessionMgr &rSessionMgr = CGDSessionMgr::GetInstance();

    if (rSessionMgr.GetGMSession().pSocketSession)
    {
        pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = a2a_ping;

        nRetCode = _SendGM((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
    }

    if (rSessionMgr.GetMSSession().pSocketSession)
    {
        pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = a2a_ping;

        nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
    }

    if (rSessionMgr.GetLSSession().pSocketSession)
    {
        pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = a2a_ping;

        nRetCode = _SendLS((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
    }

	if (rSessionMgr.GetBSSession().pSocketSession)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendBS((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
	}

    if (rSessionMgr.GetDBSession().pSocketSession)
    {
        pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
        LOG_PROCESS_ERROR(pMsg);

        pMsg->wMsg = a2a_ping;

        nRetCode = _SendDB((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
    }

	if (rSessionMgr.GetDLSession().pSocketSession)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendDL((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
	}

	if (rSessionMgr.GetMTSession().pSocketSession)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
        LOG_CHECK_ERROR(nRetCode);

        if (!nRetCode)
            m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
        pMsg = NULL;
	}

	if (rSessionMgr.GetMMSession().pSocketSession)
	{
		pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
		LOG_PROCESS_ERROR(pMsg);

		pMsg->wMsg = a2a_ping;

		nRetCode = _SendMM((PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
		LOG_CHECK_ERROR(nRetCode);

		if (!nRetCode)
			m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
		pMsg = NULL;
	}

    for (INT32 nIndex = 1; nIndex < MAX_GS_SESSION_COUNT; nIndex++)
    {
        SGSSession* pGSSession = rSessionMgr.GetGSSession(nIndex);
        if (pGSSession && pGSSession->pSocketSession)
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);

            pMsg->wMsg = a2a_ping;

            nRetCode = _SendGS(pGSSession->dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
            pMsg = NULL;
        }
    }

    for (INT32 nIndex = 1; nIndex < MAX_GT_SESSION_COUNT; nIndex++)
    {
        SGTSession* pGTSession = rSessionMgr.GetGTSession(nIndex);
        if (pGTSession && pGTSession->pSocketSession)
        {
            pMsg = (A2A_PING*)m_pPacketAllocator->Alloc(sizeof(A2A_PING));
            LOG_PROCESS_ERROR(pMsg);

            pMsg->wMsg = a2a_ping;

            nRetCode = _SendGT(pGTSession->dwSessionID, (PACKET_BUFFER*)pMsg, sizeof(A2A_PING));
            LOG_CHECK_ERROR(nRetCode);

            if (!nRetCode)
                m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
            pMsg = NULL;
        }
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CGDMessageHandler::DoMSClose(void)
{
    INT32 nRetCode = 0;

    U2M_CLOSE* pMsg = (U2M_CLOSE*)(m_pPacketAllocator->Alloc(sizeof(U2M_CLOSE)));
    LOG_PROCESS_ERROR(pMsg);
    pMsg->wMsg = u2m_close;

    nRetCode = _SendMS((PACKET_BUFFER*)pMsg, sizeof(U2M_CLOSE));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CGDMessageHandler::DoMSCommand(const char* pszCommand)
{
    INT32 nRetCode = 0;
	INT32 nCommandLen = strlen(pszCommand);

	INT32 nSize = sizeof(U2M_COMMAND) + nCommandLen + 1;

	U2M_COMMAND* pMsg = (U2M_COMMAND*)(m_pPacketAllocator->Alloc(nSize));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = u2m_command;	// only be processed by final version
	strxcpy(pMsg->szCommand, pszCommand, nCommandLen+1);

	nRetCode = _SendMS((PACKET_BUFFER*)pMsg, nSize);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CGDMessageHandler::DoMTNotify(UINT8 type, UINT8 id, UINT8 level, INT32 error, const char* format, ...)
{
    if (!m_pPacketAllocator || !format) return FALSE;
    std::array<char, 4096> text{};
    va_list args;
    va_start(args, format);
    std::vsnprintf(text.data(), text.size(), format, args);
    va_end(args);
    const auto length = std::char_traits<char>::length(text.data());
    const auto size = sizeof(U2N_NOTIFY) + length + 1;
    const auto free = [allocator = m_pPacketAllocator](PACKET_BUFFER* p) { allocator->Free(p); };
    std::unique_ptr<PACKET_BUFFER, decltype(free)> packet{m_pPacketAllocator->Alloc(static_cast<INT32>(size)), free};
    if (!packet) return FALSE;
    U2N_NOTIFY header{};
    header.wMsg = u2n_notify;
    header.bySvrType = type;
    header.bySvrID = id;
    header.byLevel = level;
    header.nErrorCode = error;
    std::memcpy(packet.get(), &header, sizeof(header));
    std::memcpy(packet.get() + sizeof(header), text.data(), length + 1);
    CRI("DoMTNotify: %s", text.data());
    if (!_SendMT(packet.get(), static_cast<UINT32>(size))) return FALSE;
    packet.release();
    return TRUE;
}

BOOL CGDMessageHandler::DoMTError(INT32 error, const char* format, ...)
{
    if (!m_pPacketAllocator || !format) return FALSE;
    U2N_ERROR message{};
    message.wMsg = u2n_error;
    message.nErrorCode = error;
    va_list args;
    va_start(args, format);
    std::vsnprintf(message.szErrorInfo, sizeof(message.szErrorInfo), format, args);
    va_end(args);
    const auto free = [allocator = m_pPacketAllocator](PACKET_BUFFER* p) { allocator->Free(p); };
    std::unique_ptr<PACKET_BUFFER, decltype(free)> packet{m_pPacketAllocator->Alloc(sizeof(message)), free};
    if (!packet) return FALSE;
    std::memcpy(packet.get(), &message, sizeof(message));
    CRI("DoMTError: %s", message.szErrorInfo);
    if (!_SendMT(packet.get(), sizeof(message))) return FALSE;
    packet.release();
    return TRUE;
}

BOOL CGDMessageHandler::DoMTUpdateCmdAck(INT32 error, const char* result)
{
    if (!m_pPacketAllocator || !result) return FALSE;
    const auto text = std::string_view{result}.substr(0, MAX_MESSAGE_SIZE - sizeof(U2N_UPDATE_CMD_ACK) - 1);
    const auto size = sizeof(U2N_UPDATE_CMD_ACK) + text.size() + 1;
    const auto free = [allocator = m_pPacketAllocator](PACKET_BUFFER* p) { allocator->Free(p); };
    std::unique_ptr<PACKET_BUFFER, decltype(free)> packet{m_pPacketAllocator->Alloc(static_cast<INT32>(size)), free};
    if (!packet) return FALSE;
    U2N_UPDATE_CMD_ACK header{};
    header.wMsg = u2n_exec_cmd_ack;
    header.nErrorCode = error;
    std::memcpy(packet.get(), &header, sizeof(header));
    std::memcpy(packet.get() + sizeof(header), text.data(), text.size());
    packet.get()[size - 1] = '\0';
    if (!_SendMT(packet.get(), static_cast<UINT32>(size))) return FALSE;
    packet.release();
    return TRUE;
}

BOOL CGDMessageHandler::DoMTRegristerAck(INT32 nErrorCode, const char* szVersion, INT32 nMsgVersion, const char* szBuildVersion)
{
	INT32 nRetCode = 0;

	U2N_REGRISTER_ACK* pMsg = (U2N_REGRISTER_ACK*)(m_pPacketAllocator->Alloc(sizeof(U2N_REGRISTER_ACK)));
	LOG_PROCESS_ERROR(pMsg);
	
	memset(pMsg, 0, sizeof(U2N_REGRISTER_ACK));
	pMsg->wMsg = u2n_regrister_ack;
    pMsg->nErrorCode = nErrorCode;
    strxcpy(pMsg->szVersion, szVersion, sizeof(pMsg->szVersion) );
	pMsg->nMsgVersion = nMsgVersion;
	strxcpy(pMsg->szBuildVersion, szBuildVersion, sizeof(pMsg->szBuildVersion));

	nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(U2N_REGRISTER_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CGDMessageHandler::DoMTSyncProcessSummary()
{	
	INT32 nRetCode = 0;

	U2N_SYNC_DEFAULT_INFO* pMsg = (U2N_SYNC_DEFAULT_INFO*)(m_pPacketAllocator->Alloc(sizeof(U2N_SYNC_DEFAULT_INFO)));
	LOG_PROCESS_ERROR(pMsg);

	memset(pMsg, 0, sizeof(U2N_SYNC_DEFAULT_INFO));

	pMsg->wMsg = u2n_sync_processes_summary;
	pMsg->nDefaultCount[stMS] = g_GuardConfig.nLocalMSCount;
	pMsg->nDefaultCount[stDB] = g_GuardConfig.nLocalDBCount;
	pMsg->nDefaultCount[stBS] = g_GuardConfig.nLocalBSCount;
	pMsg->nDefaultCount[stLS] = g_GuardConfig.nLocalLSCount;
	pMsg->nDefaultCount[stGT] = g_GuardConfig.nLocalGTCount;
	pMsg->nDefaultCount[stGS] = g_GuardConfig.nLocalGSCount;
	pMsg->nDefaultCount[stDL] = g_GuardConfig.nLocalDLCount;
	pMsg->nDefaultCount[stMM] = g_GuardConfig.nLocalMMCount;
	pMsg->nDefaultCount[stMG] = g_GuardConfig.nLocalMGCount;

	nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(U2N_SYNC_DEFAULT_INFO));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CGDMessageHandler::DoMTUpdateProcess(INT32 nState, UINT32 dwProcessID, UINT8 byType, UINT32 dwSessionID, BOOL bAuto)
{
	INT32 nRetCode = 0;
	N2U_UPDATE_SESSION* pMsg = NULL;
	CGDSessionMgr &rSessionMgr = CGDSessionMgr::GetInstance();

	pMsg = (N2U_UPDATE_SESSION*)m_pPacketAllocator->Alloc(sizeof(N2U_UPDATE_SESSION));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = n2u_update_process_info;
	pMsg->dwProcessID = dwProcessID;
	pMsg->nState = nState;
	pMsg->bAuto = bAuto;
	pMsg->dwSessionID = dwSessionID;
	pMsg->byType = byType;

	nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(N2U_UPDATE_SESSION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CGDMessageHandler::DoMTDeployServerVersionAck(INT32 nErrorCode, const char* pcszInfo)
{
	INT32 nRetCode = 0;
	U2N_DEPLOY_SERVER_VERSION_ACK* pMsg = NULL;
	CGDSessionMgr &rSessionMgr = CGDSessionMgr::GetInstance();

	pMsg = (U2N_DEPLOY_SERVER_VERSION_ACK*)m_pPacketAllocator->Alloc(sizeof(U2N_DEPLOY_SERVER_VERSION_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = u2n_deploy_server_version_ack;
	pMsg->nErrorCode = nErrorCode;
	strxcpy(pMsg->szInfo, pcszInfo, sizeof(pMsg->szInfo));

	nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(U2N_DEPLOY_SERVER_VERSION_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CGDMessageHandler::DoMTHotUpdatePatchAck(INT32 nErrorCode, const char* pcszInfo)
{
	INT32 nRetCode = 0;
	U2N_HOT_UPDATE_PATCH_ACK* pMsg = NULL;
	CGDSessionMgr &rSessionMgr = CGDSessionMgr::GetInstance();

	pMsg = (U2N_HOT_UPDATE_PATCH_ACK*)m_pPacketAllocator->Alloc(sizeof(U2N_HOT_UPDATE_PATCH_ACK));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = u2n_hot_update_patch_ack;
	pMsg->nErrorCode = nErrorCode;
	strxcpy(pMsg->szInfo, pcszInfo, sizeof(pMsg->szInfo));

	nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(U2N_HOT_UPDATE_PATCH_ACK));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

void CGDMessageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    CGDSessionMgr::GetInstance().RefreshServerSession(pSocketSession);

    return;
}

namespace
{
ServerSession* registrationSlot(CGDSessionMgr& sessions, UINT8 type, UINT8 id)
{
    if (id == 0) return nullptr;
    switch (type)
    {
    case stGT: return sessions.GetGTSession(id);
    case stGS: return sessions.GetGSSession(id);
    case stMG: return sessions.GetMGSession(id);
    default: if (id != 1) return nullptr;
    }
    switch (type)
    {
    case stMS: return &sessions.GetMSSession();
    case stLS: return &sessions.GetLSSession();
    case stDB: return &sessions.GetDBSession();
    case stBS: return &sessions.GetBSSession();
    case stDL: return &sessions.GetDLSession();
    case stGM: return &sessions.GetGMSession();
    case stMM: return &sessions.GetMMSession();
    default: return nullptr;
    }
}

UINT64 registrationDeadline()
{
    const auto now = GetTickCountEx();
    const auto duration = static_cast<UINT64>(g_GuardConfig.nServerPingTimeout);
    const auto maximum = (std::numeric_limits<UINT64>::max)();
    return duration > maximum - now ? maximum : now + duration;
}

// Detach the old socket before Close: disconnect delivery may be synchronous or
// delayed, and must never clear a replacement record's address or timer.
bool retireConnection(ServerSession& slot)
{
    auto* previous = std::exchange(slot.pSocketSession, nullptr);
    if (!previous) return true;
    slot.qwTimer = (std::numeric_limits<UINT64>::max)();
    slot.eState = epsUnonnect;
    slot.dwProcessID = 0;
    slot.bClosing = false;
    previous->SetUserData(nullptr);
    bool closed = false;
    try { closed = previous->Close() != FALSE; }
    catch (...) { ERR("old guard connection close threw an exception"); }
    // Close may already have delivered destruction callbacks even on failure.
    // Never restore a borrowed pointer after calling it.
    // A reentrant registration has priority over the caller's stale snapshot.
    return closed && !slot.pSocketSession;
}

std::string addressOf(ISocketSession& socket)
{
    UINT32 address = 0;
    UINT16 port = 0;
    if (!socket.GetRemoteAddress(address, port)) return "unknown";
    in_addr ip{};
    ip.s_addr = address;
    const auto* text = inet_ntoa(ip);
    return text ? std::string{text} : std::string{"unknown"};
}
}

void CGDMessageHandler::OnProcessRegrister(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto message = *guard_packets::read<A2U_REGRISTER>({packet, size});
    auto& sessions = CGDSessionMgr::GetInstance();
    auto* slot = registrationSlot(sessions, message.byType, message.bySessionID);
    if (!slot || !message.dwProcessID || socket->GetUserData() || g_GuardConfig.nServerPingTimeout < 0)
    {
        ERR("invalid process registration type %u session %u", message.byType, message.bySessionID);
        socket->Close();
        return;
    }
    auto& processes = CProcessMgr::GetInstance();
    if ((!processes.IsProcessRuningByID(message.dwProcessID) &&
        !processes.AddProcessInfo(message.byType, message.dwProcessID, 0, FALSE)) ||
        !processes.SetProcessInfo(message.dwProcessID, message.byType, message.bySessionID) ||
        !sessions.RemoveServerSession(socket) || !retireConnection(*slot))
    {
        ERR("process registration failed for PID %u", message.dwProcessID);
        socket->Close();
        return;
    }
    slot->pSocketSession = socket;
    slot->qwTimer = registrationDeadline();
    slot->eState = epsNormal;
    slot->dwSessionID = message.bySessionID;
    slot->dwProcessID = message.dwProcessID;
    slot->bClosing = false;
    socket->SetUserData(slot);
    INF("%s %u registered successfully, pid = %u", GetServerShortName(message.byType),
        message.bySessionID, message.dwProcessID);
    LOG_CHECK_ERROR(processes.SyncProcessInfo(epsNormal, message.dwProcessID));
}

void CGDMessageHandler::GDAlertMT(const char* szContent, ...)
{
	INT32 nRetCode = 0;
	const tm_ex* pNow = NULL;
    char szInfo[4096] = { 0 };

	PROCESS_ERROR(CGDSessionMgr::GetInstance().GetMTSession().pSocketSession);

	va_list valist;
	va_start(valist, szContent);
	vsnprintf(szInfo, sizeof(szInfo), szContent, valist);
	va_end(valist);

	pNow = localtime_ex();
	nRetCode = DoMTNotify(stGD, 0, 0, errCritical,
		"%d-%02d-%02d %02d:%02d:%02d.%03d %s",
		pNow->tm_year, pNow->tm_mon, pNow->tm_mday,
		pNow->tm_hour, pNow->tm_min, pNow->tm_sec, pNow->tm_msec,
		szInfo);
	LOG_CHECK_ERROR(nRetCode);
Exit0:
	return;
}

void CGDMessageHandler::OnAllNotify(PACKET_BUFFER* packet, UINT32 size, ISocketSession*)
{
    const guard_packets::Bytes bytes{packet, size};
    A2U_NOTIFY message{};
    std::memcpy(&message, bytes.data(), sizeof(message));
    const auto content = *guard_packets::tail<A2U_NOTIFY>(bytes);
    const auto now = *localtime_ex();
    const auto* serverName = GetServerShortName(message.bySvrType);
    auto description = std::format("NOTIFY_{} [{}:{}] {}-{:02}-{:02} {:02}:{:02}:{:02}.{:03} {}",
        message.byLevel, serverName ? serverName : "(null)", message.bySvrID,
        now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, now.tm_msec, content);
    // Preserve the notification's legacy display/forwarding limit.
    if (description.size() > 4095) description.resize(4095);
    INF("%s", description.c_str());
    LOG_CHECK_ERROR(DoMTNotify(message.bySvrType, message.bySvrID, message.byLevel,
        message.nErrorCode, "%s", description.c_str()));
}

void CGDMessageHandler::OnMSCommandAck(PACKET_BUFFER* packet, UINT32 size, ISocketSession*)
{
    const std::string result{*guard_packets::tail<M2U_COMMAND_RESPONSE>({packet, size})};
    LOG_CHECK_ERROR(DoMTUpdateCmdAck(errSuccess, result.c_str()));
}

void CGDMessageHandler::OnMTRegrister(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    const auto message = *guard_packets::read<N2U_REGRISTER>({packet, size});
    const auto name = *guard_packets::text(message.szName);
    const auto password = *guard_packets::text(message.szPwd);
    const auto configuredName = guard_packets::text(g_GuardConfig.szMTUser);
    const auto configuredPassword = guard_packets::text(g_GuardConfig.szMTPwd);
    if (!configuredName || !configuredPassword || !guard_files::same_tag(name, *configuredName) ||
        !guard_files::same_tag(password, *configuredPassword) || socket->GetUserData() ||
        g_GuardConfig.nServerPingTimeout < 0)
    {
        GD_ERROR_MT("name or password don't match, or registration state is invalid");
        socket->Close();
        return;
    }
    auto& sessions = CGDSessionMgr::GetInstance();
    auto& slot = sessions.GetMTSession();
    if (!sessions.RemoveServerSession(socket)) { socket->Close(); return; }
    const auto newAddress = addressOf(*socket);
    if (slot.pSocketSession)
    {
        const auto info = std::format("kick off old MT from {}, because new MT from {} login",
            addressOf(*slot.pSocketSession), newAddress);
        CRI("%s", info.c_str());
        GD_ERROR_MT("%s", info.c_str());
        LOG_CHECK_ERROR(DoMTRegristerAck(errLoginRepeat, info.c_str(), MAINTAIN_TOOLS_MSG_VERSION, MAINTAIN_TOOLS_BUILD_VERSION));
        if (!retireConnection(slot)) { socket->Close(); return; }
    }
    slot.pSocketSession = socket;
    slot.qwTimer = registrationDeadline();
    slot.eState = epsNormal;
    slot.dwProcessID = 0;
    slot.bClosing = false;
    socket->SetUserData(&slot);
    INF("set new session: MT from %s", newAddress.c_str());
    std::array<char, PATH_LEN> version{};
    LOG_CHECK_ERROR(CGuardServer::GetInstance().FetchCurrentVersion(version.data(), static_cast<int>(version.size())));
    LOG_CHECK_ERROR(DoMTRegristerAck(errSuccess, version.data(), MAINTAIN_TOOLS_MSG_VERSION, MAINTAIN_TOOLS_BUILD_VERSION));
    LOG_CHECK_ERROR(DoMTSyncProcessSummary());
    LOG_CHECK_ERROR(CProcessMgr::GetInstance().SyncAllProcessInfo());
    INF("MT Register Success from %s", newAddress.c_str());
}

void CGDMessageHandler::OnMTExecCmd(PACKET_BUFFER* packet, UINT32 size, ISocketSession*)
{
    const std::string command{*guard_packets::tail<N2U_UPDATE_CMD>({packet, size})};
    BOOL result = FALSE;
    if (!command.empty())
    {
        result = command.front() == '.' ? CCmdMgr::GetInstance().ExecGMCmd(command.c_str())
            : CCmdMgr::GetInstance().ExecOPCmd(command.c_str());
        if (result && command.front() == '.') return; // MS provides the GM result.
    }
    auto info = std::format("ExecCmd [{}] issue {}", command, result ? "ok" : "failed");
    if (info.size() > 255) info.resize(255);
    LOG_CHECK_ERROR(DoMTUpdateCmdAck(result ? errSuccess : result, info.c_str()));
}

void CGDMessageHandler::OnMTDeployServerVersion(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    std::string name;
    if (packet && socket && size == sizeof(N2U_DEPLOY_SERVER_VERSION))
    {
        const auto& message = *reinterpret_cast<const N2U_DEPLOY_SERVER_VERSION*>(packet);
        const auto end = std::find(std::begin(message.szVersion), std::end(message.szVersion), '\0');
        if (end != std::end(message.szVersion)) name.assign(std::begin(message.szVersion), end);
    }
    if (!name.empty() && CGuardServer::GetInstance().StartDeployment(false, name.c_str())) return;
    CRI("could not start deployment: %s", name.c_str());
    GD_ERROR_MT("could not start deployment: %s", name.c_str());
    LOG_CHECK_ERROR(DoMTDeployServerVersionAck(errCritical, name.c_str()));
}

void CGDMessageHandler::OnMTHotUpdatePatch(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    std::string name;
    if (packet && socket && size == sizeof(N2U_HOT_UPDATE_PATCH))
    {
        const auto& message = *reinterpret_cast<const N2U_HOT_UPDATE_PATCH*>(packet);
        const auto end = std::find(std::begin(message.szPatch), std::end(message.szPatch), '\0');
        if (end != std::end(message.szPatch)) name.assign(std::begin(message.szPatch), end);
    }
    if (!name.empty() && CGuardServer::GetInstance().StartDeployment(true, name.c_str())) return;
    CRI("could not start patch installation: %s", name.c_str());
    GD_ERROR_MT("could not start patch installation: %s", name.c_str());
    LOG_CHECK_ERROR(DoMTHotUpdatePatchAck(errUnknownError, name.c_str()));
}

void CGDMessageHandler::OnMTSendFiles(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    std::string name;
    UINT16 bufferIndex = 0, bufferTotal = 0, fileIndex = 0, fileTotal = 0;
    const auto fail = [&](const char* reason) {
        if (socket) m_incomingFile.abort(socket);
        CRI("file transfer failed: %s", reason);
        GD_ERROR_MT("%s", reason);
        LOG_CHECK_ERROR(DoMTSendFilesAck(-1, name.c_str(), bufferIndex, bufferTotal, fileIndex, fileTotal));
    };
    // The maintenance tool sends the complete fixed-size N2U_SEND_FILES record.
    // Validate that contract before inspecting its fields or payload.
    if (!packet || !socket || size != sizeof(N2U_SEND_FILES))
    {
        fail("invalid file packet size");
        return;
    }
    const auto& message = *reinterpret_cast<const N2U_SEND_FILES*>(packet);
    bufferIndex = message.wBufferIndex; bufferTotal = message.wBufferTotal;
    fileIndex = message.wFileIndex; fileTotal = message.wFileTotal;
    const auto end = std::find(std::begin(message.szFileName), std::end(message.szFileName), '\0');
    if (end == std::end(message.szFileName) || message.wBufferSize > sizeof(message.szBuffer))
    {
        fail("invalid filename or chunk length");
        return;
    }
    try
    {
        name.assign(std::begin(message.szFileName), end);
        if (CGuardServer::GetInstance().DeploymentBusy())
        {
            fail("file transfer is unavailable during deployment"); return;
        }
        const char* root = nullptr;
        switch (message.nFileType)
        {
        case etftVersion: case etftConfig: root = g_GuardConfig.szServerVersionPath; break;
        case etftPatch: root = g_GuardConfig.szServerPatchPath; break;
        default: fail("invalid file type"); return;
        }
        const guard_files::UploadChunk chunk{name, message.nFileType, fileIndex, fileTotal,
            bufferIndex, bufferTotal, std::span{message.szBuffer, message.wBufferSize}};
        auto received = m_incomingFile.receive(root, chunk, socket);
        if (!received) { fail(received.error().message().c_str()); return; }
        if (!*received) return; // Preserve the final-chunk-only acknowledgement contract.
        const auto& uploaded = **received;
        if (uploaded.type == etftConfig)
        {
            char version[128]{};
            if (!CGuardServer::GetInstance().FetchCurrentVersion(version, sizeof(version)) ||
                !guard_files::leaf_name(version))
            {
                fail("cannot identify current version for configuration installation"); return;
            }
            const auto versionCopy = uploaded.path.parent_path() / (std::string(version) + "-" + name);
            const bool guardConfig = guard_files::same_tag(name, "guard_area_config.lua");
            const auto target = guardConfig
                ? std::filesystem::path(g_GuardConfig.szGuardPath) / "guardconfig" / name
                : std::filesystem::path(g_GuardConfig.szServerRunPath) / "globalconfig" / name;
            std::error_code installationError;
            auto copied = guard_files::copy_file(uploaded.path, versionCopy, [&] {
                auto installed = guardConfig
                    ? guard_files::copy_file(uploaded.path, target, [] {
                        return CGuardServer::GetInstance().ReloadConfig() != FALSE;
                    })
                    : guard_files::copy_file(uploaded.path, target);
                if (!installed) installationError = installed.error();
                return installed.has_value();
            });
            if (!copied)
            {
                fail((installationError ? installationError : copied.error()).message().c_str()); return;
            }
        }
        LOG_CHECK_ERROR(DoMTSendFilesAck(errSuccess, name.c_str(), bufferIndex, bufferTotal, fileIndex, fileTotal));
    }
    catch (const std::exception& error) { fail(error.what()); }
}

BOOL CGDMessageHandler::DoMTSendFilesAck(INT32 nErrorCode, const char* szFileName, 
                                      UINT16 wBufferIndex, UINT16 wBufferTotal,
                                      UINT16 wFileIndex, UINT16 wFileTotal)
{
    INT32 nRetCode = 0;
    U2N_SEND_FILES_ACK* pMsg = NULL;
    CGDSessionMgr &rSessionMgr = CGDSessionMgr::GetInstance();

    pMsg = (U2N_SEND_FILES_ACK*)m_pPacketAllocator->Alloc(sizeof(U2N_SEND_FILES_ACK));
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = u2n_send_files_ack;

    pMsg->wBufferIndex = wBufferIndex;
    pMsg->wBufferTotal = wBufferTotal;
    pMsg->nErrorCode = nErrorCode;

    pMsg->wFileIndex = wFileIndex;
    pMsg->wFileTotal = wFileTotal;
    strxcpy(pMsg->szFileName, szFileName, sizeof(pMsg->szFileName));

    nRetCode = _SendMT((PACKET_BUFFER*)pMsg, sizeof(U2N_SEND_FILES_ACK));
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    if (pMsg)
        m_pPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}
