#include "stdafx.h"
//#include "sdtime.h"

#include "common_message.h"

#include <QtWidgets/QMessageBox>

#include "session_mgr.h"
#include "message_handler.h"
#include "maintaintool.h"
#include "transmit_file_mgr.h"
#include "maintain_tool_version.h"
#include <algorithm>
#include <cstring>
#include <memory>
#include <format>
#include "receive_packets.h"
#include "maintain_stats.h"

using namespace InternalMessage;
CMTMessageHandler CMTMessageHandler::ms_Instance;

void CMTMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession* socket)
{
    try { if (socket) socket->Close(); }
    catch (...) { ERR("cannot close unexpected inbound maintaintool connection"); }
}

void CMTMessageHandler::OnConnected(ISocketSession* socket)
{
    try
    {
        if (!socket) return;
        if (!m_receiving) { socket->Close(); return; }
        auto* session=static_cast<SRemoteGuardSession*>(socket->GetUserData());
        if (!session || session->pSocketSession!=socket) { socket->Close(); return; }
        auto fresh=std::make_unique<stProcessStatisInfo>();
        session->poStatisInfo=std::move(fresh);
        session->lstProcess.clear();
        session->dwTimer=CSessionMgr::GetInstance().Deadline(g_MTConfig.nPingTimeout);
        session->bConnected=TRUE;
        if (!MaintainTool::GetInstance().UpdateGuardConnectInfo(session,emctConnected,nullptr))
        { socket->Close(); return; }
        if (!m_receiving || socket->GetUserData()!=session || session->pSocketSession!=socket) return;
        if (!DoGDRegristerGD(session,MaintainTool::GetInstance().GetUserName(),MaintainTool::GetInstance().GetPassword()))
            socket->Close();
    }
    catch (const std::exception& error)
    {
        ERR("maintaintool connected callback failed: %s",error.what());
        if (socket) socket->Close();
    }
}

void CMTMessageHandler::OnRecv(ISocketSession* socket, PACKET_BUFFER* buffer, UINT32 size)
{
    const auto free = [allocator = m_pPacketAllocator](PACKET_BUFFER* packet) { if (allocator) allocator->Free(packet); };
    const std::unique_ptr<PACKET_BUFFER, decltype(free)> packet{buffer, free};
    MESSAGE_HEADER header{};
    if (m_receiving && socket && packet && m_pPacketAllocator && size >= sizeof(header))
    {
        std::memcpy(&header, packet.get(), sizeof(header));
        auto* session = static_cast<SRemoteGuardSession*>(socket->GetUserData());
        if (session && session->pSocketSession == socket && header.wMsg < m_RemoteGuardMessageFuncList.size() &&
            maintain::valid(header.wMsg, {packet.get(), size}))
        {
            if (const auto handler = m_RemoteGuardMessageFuncList[header.wMsg])
            {
                try { (this->*handler)(packet.get(), size, socket); return; }
                catch (const std::exception& error) { ERR("maintain message failed: %s", error.what()); }
                catch (...) { ERR("maintain message callback threw an exception"); }
            }
        }
    }
    ERR("invalid maintain message %u size %u", header.wMsg, size);
    if (socket) socket->Close();
}

void CMTMessageHandler::OnDisconnected(ISocketSession* socket, BOOL)
{
    if (!socket) return;
    auto* session = static_cast<SRemoteGuardSession*>(socket->GetUserData());
    socket->SetUserData(nullptr);
    if (!session || session->pSocketSession != socket) return;
    const auto area = session->dwArea;
    const auto ip = session->dwIp;
    session->pSocketSession = nullptr;
    session->bConnected = FALSE;
    session->closing = false;
    session->dwTimer = CSessionMgr::GetInstance().Deadline(g_MTConfig.nReconnectTime);
    session->lstProcess.clear();
    CTransmitFileMgr::GetInstance().StopSending(area, ip);
    LOG_CHECK_ERROR(MaintainTool::GetInstance().UpdateGuardConnectInfo(session, emctUnConnect, nullptr));
    MaintainTool::GetInstance().UpdateGuardProcessesInfo();
    MaintainTool::GetInstance().UpdateGuardStatisInfo(area,ip);
}

CMTMessageHandler::CMTMessageHandler()
{
    m_pNet = NULL;
    m_pPacketAllocator = NULL;
}

CMTMessageHandler::~CMTMessageHandler()
{
    UnInit();
}

BOOL CMTMessageHandler::Init(INet* pNet, IPacketAllocator* pPacketAllocator)
{

    {
    if (m_pNet || m_pPacketAllocator) return FALSE;
    LOG_PROCESS_ERROR(pNet);
    LOG_PROCESS_ERROR(pPacketAllocator);

    m_pNet = pNet;
    m_pPacketAllocator = pPacketAllocator;
    m_RemoteGuardMessageFuncList.fill(nullptr);
    m_receiving = true;

    //All
    //////////////////////////////////////////////////////////////////////////////////////
    m_RemoteGuardMessageFuncList[a2a_ping] = &CMTMessageHandler::OnAllPing;
	m_RemoteGuardMessageFuncList[u2n_regrister_ack] = &CMTMessageHandler::OnGDRegristerAck;
	m_RemoteGuardMessageFuncList[u2n_sync_processes_summary] = &CMTMessageHandler::OnGDSyncProcessSummary;
	m_RemoteGuardMessageFuncList[n2u_update_process_info] = &CMTMessageHandler::OnGDUpdateProcess;
	m_RemoteGuardMessageFuncList[u2n_exec_cmd_ack] = &CMTMessageHandler::OnGDExecCmdAck;
    m_RemoteGuardMessageFuncList[u2n_send_files_ack] = &CMTMessageHandler::OnGDSendFilesAck;
	m_RemoteGuardMessageFuncList[u2n_deploy_server_version_ack] = &CMTMessageHandler::OnGDDeployServerVersionAck;
	m_RemoteGuardMessageFuncList[u2n_hot_update_patch_ack] = &CMTMessageHandler::OnGDHotUpdatePatchAck;
	m_RemoteGuardMessageFuncList[u2n_update_statis_info] = &CMTMessageHandler::OnGDUpdateStatisInfo;
	m_RemoteGuardMessageFuncList[u2n_notify] = &CMTMessageHandler::OnGDNotify;
	m_RemoteGuardMessageFuncList[u2n_error] = &CMTMessageHandler::OnGDError;
    /////////////////////////////////////////////////////////////////////////////////////
    return TRUE;
    }
Exit0:
    return FALSE;
}

BOOL CMTMessageHandler::UnInit(void)
{
    BeginShutdown();
    m_RemoteGuardMessageFuncList.fill(nullptr);
    m_pNet = nullptr;
    m_pPacketAllocator = nullptr;
    return TRUE;
}

CMTMessageHandler& CMTMessageHandler::GetInstance(void)
{
    return ms_Instance;
}

namespace
{
template<class Initialize>
BOOL send_packet(IPacketAllocator* allocator,bool accepting,SRemoteGuardSession* session,std::size_t size,Initialize initialize)
try
{
    if (!accepting || !allocator || !session || !session->bConnected || !session->pSocketSession ||
        !size || size>MAX_MESSAGE_SIZE) return FALSE;
    auto* socket=session->pSocketSession;
    const auto free=[allocator](PACKET_BUFFER* packet) { allocator->Free(packet); };
    std::unique_ptr<PACKET_BUFFER,decltype(free)> packet{allocator->Alloc(static_cast<INT32>(size)),free};
    if (!packet) { ERR("maintaintool packet allocation failed (%zu bytes)",size); return FALSE; }
    std::memset(packet.get(),0,size);
    initialize(packet.get());
    if (socket->Send(packet.get(),static_cast<INT32>(size),0)!=static_cast<INT32>(size))
    { ERR("maintaintool transport rejected an outgoing packet"); return FALSE; }
    packet.release(); // Native Send takes ownership only on success.
    return TRUE;
}
catch (const std::exception& error) { ERR("maintaintool send failed: %s",error.what()); return FALSE; }
catch (...) { ERR("maintaintool send callback threw an exception"); return FALSE; }
}
BOOL CMTMessageHandler::DoPing(SRemoteGuardSession* session)
{
    return send_packet(m_pPacketAllocator,m_receiving,session,sizeof(A2A_PING),[](auto* packet) {
        reinterpret_cast<A2A_PING*>(packet)->wMsg=a2a_ping;
    });
}
BOOL CMTMessageHandler::DoGDRegristerGD(SRemoteGuardSession* session,const char* user,const char* password)
{
    if (!user || !password) return FALSE;
    const std::string_view name{user},secret{password};
    if (name.size()>=sizeof(N2U_REGRISTER::szName) || secret.size()>=sizeof(N2U_REGRISTER::szPwd)) return FALSE;
    return send_packet(m_pPacketAllocator,m_receiving,session,sizeof(N2U_REGRISTER),[&](auto* packet) {
        auto* message=reinterpret_cast<N2U_REGRISTER*>(packet);
        message->wMsg=n2u_regrister;
        std::ranges::copy(name,message->szName);std::ranges::copy(secret,message->szPwd);
    });
}
BOOL CMTMessageHandler::DoGDExecCmd(INT32 area,UINT32 ip,const char* text)
{
    if (!text) return FALSE;
    const std::string_view command{text};
    if (command.size()>MAX_MESSAGE_SIZE-sizeof(N2U_UPDATE_CMD)-1) return FALSE;
    return send_packet(m_pPacketAllocator,m_receiving,CSessionMgr::GetInstance().GetRemoteGDSession(area,ip),
        sizeof(N2U_UPDATE_CMD)+command.size()+1,[&](auto* packet) {
            auto* message=reinterpret_cast<N2U_UPDATE_CMD*>(packet);message->wMsg=n2u_exec_cmd;
            std::ranges::copy(command,message->szCmd);
        });
}
BOOL CMTMessageHandler::DoGDSendFiles(INT32 area,UINT32 ip,BOOL type,const char* name,
    UINT16 block,UINT16 blocks,UINT16 length,char bytes[],UINT16 file,UINT16 files)
{
    if (!name || std::strlen(name)>=sizeof(N2U_SEND_FILES::szFileName) || !blocks || block>=blocks ||
        !files || file>=files || length>G_FILE_SEND_BLOCK || (!bytes && length)) return FALSE;
    return send_packet(m_pPacketAllocator,m_receiving,CSessionMgr::GetInstance().GetRemoteGDSession(area,ip),
        sizeof(N2U_SEND_FILES),[&](auto* packet) {
            auto* message=reinterpret_cast<N2U_SEND_FILES*>(packet);
            message->wMsg=n2u_send_files;message->nFileType=type;
            std::memcpy(message->szFileName,name,std::strlen(name));
            message->wBufferIndex=block;message->wBufferTotal=blocks;message->wBufferSize=length;
            message->wFileIndex=file;message->wFileTotal=files;
            if (length) std::memcpy(message->szBuffer,bytes,length);
        });
}
BOOL CMTMessageHandler::DoGDDeployServerVersion(INT32 area,UINT32 ip,const char* version)
{
    if (!version || std::strlen(version)>=sizeof(N2U_DEPLOY_SERVER_VERSION::szVersion)) return FALSE;
    return send_packet(m_pPacketAllocator,m_receiving,CSessionMgr::GetInstance().GetRemoteGDSession(area,ip),
        sizeof(N2U_DEPLOY_SERVER_VERSION),[&](auto* packet) {
            auto* message=reinterpret_cast<N2U_DEPLOY_SERVER_VERSION*>(packet);message->wMsg=n2u_deploy_server_version;
            std::memcpy(message->szVersion,version,std::strlen(version));
        });
}
BOOL CMTMessageHandler::DoGDHotUpdatePatch(INT32 area,UINT32 ip,const char* patch)
{
    if (!patch || std::strlen(patch)>=sizeof(N2U_HOT_UPDATE_PATCH::szPatch)) return FALSE;
    return send_packet(m_pPacketAllocator,m_receiving,CSessionMgr::GetInstance().GetRemoteGDSession(area,ip),
        sizeof(N2U_HOT_UPDATE_PATCH),[&](auto* packet) {
            auto* message=reinterpret_cast<N2U_HOT_UPDATE_PATCH*>(packet);message->wMsg=n2u_hot_update_patch;
            std::memcpy(message->szPatch,patch,std::strlen(patch));
        });
}

void CMTMessageHandler::OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
    SRemoteGuardSession* pSession = (SRemoteGuardSession*)pSocketSession->GetUserData();
    if(pSession)
        pSession->dwTimer = CSessionMgr::GetInstance().Deadline(g_MTConfig.nPingTimeout);

    return;
}

void CMTMessageHandler::OnGDRegristerAck(PACKET_BUFFER* packet, UINT32, ISocketSession* socket)
{
    U2N_REGRISTER_ACK message{};
    std::memcpy(&message, packet, sizeof(message));
    UINT32 ip = 0; UINT16 port = 0;
    socket->GetRemoteAddress(ip, port);
    in_addr address{}; address.s_addr = ip;
    if (message.nMsgVersion != MAINTAIN_TOOLS_MSG_VERSION)
    {
        const auto info = std::format("guard on {} msg version don't match, expecting {}, but got {}",
            inet_ntoa(address), MAINTAIN_TOOLS_MSG_VERSION, message.nMsgVersion);
        INF("%s", info.c_str());
        QMessageBox box;
        box.setText(QString::fromLocal8Bit(info.c_str()));
        box.exec();
        CRI("%s", info.c_str());
        MaintainTool::GetInstance().UpdateNotifyInfo(QString::fromLocal8Bit(info.c_str()));
        socket->Close();
        return;
    }
    if (std::string_view{message.szBuildVersion} != MAINTAIN_TOOLS_BUILD_VERSION)
        CRI("guard on %s build version don't match, expecting %s, but got %s",
            inet_ntoa(address), MAINTAIN_TOOLS_BUILD_VERSION, message.szBuildVersion);
    auto* session = static_cast<SRemoteGuardSession*>(socket->GetUserData());
    if (!session || session->pSocketSession != socket) return;
    if (message.nErrorCode == errSuccess)
    {
        const auto area = session->dwArea;
        std::string version{message.szVersion};
        if (const auto end = version.find_first_of(" \r\n"); end != version.npos) version.resize(end);
        LOG_CHECK_ERROR(MaintainTool::GetInstance().SetAreaVersion(area, version.c_str()));
        if (socket->GetUserData() == session && session->pSocketSession == socket)
            LOG_CHECK_ERROR(MaintainTool::GetInstance().UpdateGuardConnectInfo(session, emctNormal, nullptr));
    }
    else if (message.nErrorCode == errLoginRepeat)
    {
        INF("kicked off! %s", message.szVersion);
        QMessageBox box; box.setText(QString::fromLocal8Bit(message.szVersion)); box.exec();
    }
    else INF("register %s failed!: reason %s", inet_ntoa(address), message.szVersion);
}

void CMTMessageHandler::OnGDSyncProcessSummary(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	U2N_SYNC_DEFAULT_INFO* pMsg = (U2N_SYNC_DEFAULT_INFO*)pPacketBuffer;

	SRemoteGuardSession* pSession = (SRemoteGuardSession*)pSocketSession->GetUserData();

    {
	if(pSession)
	{
		nRetCode = MaintainTool::GetInstance().UpdateGuardConnectInfo(pSession, emctNormal, pMsg->nDefaultCount );
		LOG_PROCESS_ERROR(nRetCode);
	}
    }
Exit0:
	return;
}

void CMTMessageHandler::OnGDUpdateProcess(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    if (size != sizeof(N2U_UPDATE_SESSION)) { socket->Close(); return; }
    N2U_UPDATE_SESSION message{};
    std::memcpy(&message, packet, sizeof(message));
    auto* session = static_cast<SRemoteGuardSession*>(socket->GetUserData());
    if (!session || session->pSocketSession != socket) return;
    if (!message.dwProcessID || message.nState < epsUnonnect || message.nState > epsDestroy ||
        (message.nState != epsDestroy && (message.byType < stMS || message.byType >= stClient)))
    { socket->Close(); return; }
    auto& processes = session->lstProcess;
    auto process = std::ranges::find(processes, message.dwProcessID, &SProcessInfo::dwProcessID);
    if (message.nState == epsDestroy)
    {
        if (process != processes.end()) processes.erase(process);
        if (session->poStatisInfo) maintain::forget_statistics(*session->poStatisInfo,message.dwProcessID);
    }
    else
    {
        if (process == processes.end())
        {
            processes.emplace_back();
            process = std::prev(processes.end());
        }
        process->dwProcessID = message.dwProcessID;
        process->dwSessionID = message.dwSessionID;
        process->byType = message.byType;
        process->bAuto = message.bAuto != FALSE;
        process->nState = message.nState;
    }
    const auto area=session->dwArea,ip=session->dwIp;
    LOG_CHECK_ERROR(MaintainTool::GetInstance().UpdateGuardProcessesInfo());
    MaintainTool::GetInstance().UpdateGuardStatisInfo(area,ip);
}

void CMTMessageHandler::OnGDNotify(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	UINT32	dwIP = 0;
	UINT16  wPort = 0;

	SRemoteGuardSession* pSession = (SRemoteGuardSession*)pSocketSession->GetUserData();

    {
	LOG_PROCESS_ERROR(pSession);
	LOG_PROCESS_ERROR(pSession->poStatisInfo);

	U2N_NOTIFY* pMsg = (U2N_NOTIFY*)pPacketBuffer;
	LOG_PROCESS_ERROR(pMsg);

	nRetCode = pSocketSession->GetRemoteAddress(dwIP, wPort);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = MaintainTool::GetInstance().UpdateNotifyInfo(dwIP, pMsg->bySvrType, pMsg->bySvrID, pMsg->byLevel, pMsg->nErrorCode, pMsg->szContent);
	LOG_PROCESS_ERROR(nRetCode);

	return;
    }
Exit0:
	return;
}

void CMTMessageHandler::OnGDError(PACKET_BUFFER* packet, UINT32 size, ISocketSession* socket)
{
    auto* session = static_cast<SRemoteGuardSession*>(socket->GetUserData());
    if (!session || !session->poStatisInfo) return;
    UINT32 ip = 0; UINT16 port = 0;
    socket->GetRemoteAddress(ip, port);
    in_addr address{}; address.s_addr = ip;
    const std::string info{*maintain::text(maintain::Bytes{packet, size}.subspan(sizeof(MESSAGE_HEADER) + sizeof(INT32)))};
    CRI("error from guard on %s: %s", inet_ntoa(address), info.c_str());
}
void CMTMessageHandler::OnGDUpdateStatisInfo(PACKET_BUFFER* packet,UINT32,ISocketSession* socket)
{
    auto* session=static_cast<SRemoteGuardSession*>(socket->GetUserData());
    if (!session || session->pSocketSession!=socket || !session->poStatisInfo) return;
    const auto& message=*reinterpret_cast<const U2N_UPDATE_STATIS_INFO*>(packet);
    const auto result=maintain::ingest_statistics(*session->poStatisInfo,message.oStatisInfo,session->lstProcess);
    if (!result) throw std::runtime_error(result.error());
    const auto area=session->dwArea,ip=session->dwIp;
    for (auto& process:session->lstProcess)
    {
        if (!process.hibernate) continue;
        const auto found=std::ranges::find_if(session->poStatisInfo->ProcessInfo,[&](const auto& entry) {
            return entry.bUsed && entry.dwProcessID==process.dwProcessID;
        });
        if (found==std::end(session->poStatisInfo->ProcessInfo)) continue;
        MaintainTool::GetInstance().ProcessHibernateGuardStatisInfoByIp(&*found,&process);
        if (socket->GetUserData()!=session || session->pSocketSession!=socket) return;
    }
    // GUI callbacks may reload or shut down the session. No borrowed state follows.
    MaintainTool::GetInstance().UpdateGuardStatisInfo(area,ip);
}

void CMTMessageHandler::OnGDExecCmdAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	UINT32	dwIP = 0;
	UINT16  wPort = 0;

	U2N_UPDATE_CMD_ACK* pMsg = (U2N_UPDATE_CMD_ACK*)pPacketBuffer;

	SRemoteGuardSession* pSession = (SRemoteGuardSession*)pSocketSession->GetUserData();

    {
	LOG_PROCESS_ERROR(pSession);

	nRetCode = pSocketSession->GetRemoteAddress(dwIP, wPort);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = MaintainTool::GetInstance().UpdateCmdResult(dwIP, pMsg->nErrorCode, pMsg->szResult);
	LOG_PROCESS_ERROR(nRetCode);
    }
Exit0:
	return;
}

void CMTMessageHandler::OnGDDeployServerVersionAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	SRemoteGuardSession* pSession = (SRemoteGuardSession*)pSocketSession->GetUserData();
	U2N_DEPLOY_SERVER_VERSION_ACK* pMsg = (U2N_DEPLOY_SERVER_VERSION_ACK*)pPacketBuffer;
	

    {
	LOG_PROCESS_ERROR(pSession);
	
	nRetCode = MaintainTool::GetInstance().DeployVersionResultByIp(pSession->dwArea, 
		pSession->dwIp, pMsg->szInfo, pMsg->nErrorCode);
	PROCESS_ERROR(nRetCode);
	
	return;

    }
Exit0:
	return;
}

void CMTMessageHandler::OnGDHotUpdatePatchAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession)
{
	INT32 nRetCode = 0;
	SRemoteGuardSession* pSession = (SRemoteGuardSession*)pSocketSession->GetUserData();
	U2N_HOT_UPDATE_PATCH_ACK* pMsg = (U2N_HOT_UPDATE_PATCH_ACK*)pPacketBuffer;


    {
	LOG_PROCESS_ERROR(pSession);

	nRetCode = MaintainTool::GetInstance().HotUpdatePatchResultByIp(pSession->dwArea,
		pSession->dwIp, pMsg->szInfo, pMsg->nErrorCode);
	PROCESS_ERROR(nRetCode);

	return;

    }
Exit0:
	return;
}

void CMTMessageHandler::OnGDSendFilesAck(PACKET_BUFFER* packet, UINT32, ISocketSession* socket)
{
    auto* session = static_cast<SRemoteGuardSession*>(socket->GetUserData());
    if (!session || session->pSocketSession != socket) return;
    U2N_SEND_FILES_ACK message{};
    std::memcpy(&message, packet, sizeof(message));
    if (!CTransmitFileMgr::GetInstance().Acknowledge(session->dwArea, session->dwIp, message))
        ERR("ignoring acknowledgement that does not match the current file transfer");
}


