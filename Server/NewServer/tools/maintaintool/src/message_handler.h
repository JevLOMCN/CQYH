#ifndef _MESSAGE_HANDLER_H_
#define _MESSAGE_HANDLER_H_
#include <array>

#include "net.h"
#include "internal_message_header.h"
#include "common_message.h"
#include "server_def.h"
#include "error_def.h"

struct SRemoteGuardSession;
class CMTMessageHandler : public INetEventHandler
{
public:
    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession);
    virtual void OnConnected(ISocketSession* pSocketSession);
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown);

public:
    CMTMessageHandler();
    ~CMTMessageHandler();

    BOOL Init(INet* pNet, IPacketAllocator* pPacketAllocator);
    BOOL UnInit(void);
    void BeginShutdown() noexcept { m_receiving = false; }
    static CMTMessageHandler& GetInstance(void);    

public:
    /************************************************************************/
    // To All Remote Guard Server
	BOOL DoPing(SRemoteGuardSession* pRemoteGDSession);
	BOOL DoGDRegristerGD(SRemoteGuardSession* pRemoteGDSession, const char* pcszUserName, const char* pcszPassword);
	BOOL DoGDExecCmd(INT32 nAreaID, UINT32 dwIP, const char* pcszCmd);

	BOOL DoGDSendFiles(INT32 nAreaID, UINT32 dwIP, BOOL bIsPatch, 
		const char* szFileName,
        UINT16 wBufferIndex, UINT16 wBufferTotal,  
        UINT16 wBufferSize, char szBuffer[],
        UINT16 wFileIndex, UINT16 wFileTotal);

	BOOL DoGDDeployServerVersion(INT32 nAreaID, UINT32 dwIP, const char* pcszVersion);
	BOOL DoGDHotUpdatePatch(INT32 nAreaID, UINT32 dwIP, const char* pcszPatch);
private:
    /************************************************************************/
    // From All
    void OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDRegristerAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDSyncProcessSummary(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDUpdateProcess(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDExecCmdAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    void OnGDSendFilesAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDDeployServerVersionAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDHotUpdatePatchAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDUpdateStatisInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDNotify(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnGDError(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

private:
    INet*                               m_pNet;
    IPacketAllocator*                   m_pPacketAllocator;
    static CMTMessageHandler				ms_Instance;

    typedef void (CMTMessageHandler::*GUARD_REMOTE_MESSAGE_FUNC)(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    std::array<GUARD_REMOTE_MESSAGE_FUNC, InternalMessage::m2a_end> m_RemoteGuardMessageFuncList{};
    bool m_receiving = false;
};

#endif  // _MESSAGE_HANDLER_H_
