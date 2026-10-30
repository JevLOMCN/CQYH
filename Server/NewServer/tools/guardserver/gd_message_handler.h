#ifndef _GUARD_MESSAGE_HANDLER_H_
#define _GUARD_MESSAGE_HANDLER_H_
#include <array>
#include "guard_files.h"

#include "net.h"
#include "internal_message_header.h"
#include "common_message.h"
#include "server_def.h"
#include "error_def.h"

#define GD_ALERT_MT(__str_content__, ...)											\
    do																				\
	{																				\
        CGDMessageHandler::GetInstance().GDAlertMT(__str_content__, ##__VA_ARGS__);	\
	} while (0)

#define GD_ERROR_MT(__str_content__, ...)											\
    do																				\
	{																				\
        CGDMessageHandler::GetInstance().DoMTError(-1, __str_content__, ##__VA_ARGS__);	\
	} while (0)

class CGDMessageHandler : public INetEventHandler
{
public:
    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession);
    virtual void OnConnected(ISocketSession* pSocketSession);
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown);

public:
    CGDMessageHandler();
    ~CGDMessageHandler();

    BOOL Init(INet* pNet, IPacketAllocator* pPacketAllocator);
    BOOL UnInit(void);
    static CGDMessageHandler& GetInstance(void);    
    BOOL CheckAllClose(void);
    bool HasIncomingFile() const noexcept { return m_incomingFile.busy(); }
    void BeginShutdown() noexcept { m_receiving = false; m_incomingFile.abort(); }

public:
    /************************************************************************/
    // To All Server
    BOOL DoAllPing(void);

    BOOL DoQueryServerInfo(void);
    /************************************************************************/
    /************************************************************************/
    // To MS
    BOOL DoMSClose(void);
    BOOL DoMSCommand(const char* pszCommand);
    /************************************************************************/
    // To GT
    /************************************************************************/
    // To GS
    /************************************************************************/
    // To LS
    /************************************************************************/
    // To DB
    /************************************************************************/
	// To MT
	/************************************************************************/
	BOOL DoMTRegristerAck(INT32 nErrorCode, const char* szVersion, INT32 nMsgVersion, const char* szBuildVersion);
	BOOL DoMTUpdateCmdAck(INT32 nErrorCode, const char* szResult);
	BOOL DoMTSyncProcessSummary();
	BOOL DoMTUpdateProcess(INT32 nState, UINT32 dwProcessID, UINT8 byType, UINT32 dwSessionID, BOOL bAuto);
	BOOL DoMTDeployServerVersionAck(BOOL bSuccess, const char* pcszInfo);
	BOOL DoMTHotUpdatePatchAck(BOOL bSuccess, const char* pcszInfo);
	BOOL DoMTUpdateStatisInfo(void);
	BOOL DoMTNotify(UINT8 bySvrType, UINT8 bySvrID, UINT8 byLevel, INT32 nErrorCode, const char* szContent, ...);
	BOOL DoMTError(INT32 nErrorCode, const char* szContent, ...);
	void GDAlertMT(const char* szContent, ...);
private:
    /************************************************************************/
    // From All
    void OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    void OnProcessRegrister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    /************************************************************************/
    // From GT
    //void OnGTQuery(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

    /************************************************************************/
    // From GS
    //void OnGSQuery(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    /************************************************************************/
    // From LS
    //void OnLSQuery(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    /************************************************************************/
    // From DB
	//void OnDBQuery(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	/************************************************************************/
	// From MS
	void OnMSCommandAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnAllNotify(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	/************************************************************************/
	// From MT
	void OnMTRegrister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnMTExecCmd(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnMTDeployServerVersion(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnMTHotUpdatePatch(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

    void OnMTSendFiles(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    BOOL DoMTSendFilesAck(INT32 nErrorCode, const char* szFileName, 
        UINT16 wBufferIndex, UINT16 wBufferTotal,
        UINT16 wFileIndex, UINT16 wFileTotal);
	/************************************************************************/

private:
    BOOL _SendGT(UINT32 dwGTSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _SendGS(UINT32 dwGSSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _SendLS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _SendDB(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
	BOOL _SendBS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
	BOOL _SendDL(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _SendMT(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _SendGM(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
	BOOL _SendMM(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);

    BOOL _BroadcastGT(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _BroadcastGS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    BOOL _BroadcastAll(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);

private:
    INet*                               m_pNet;
    IPacketAllocator*                   m_pPacketAllocator;
    static CGDMessageHandler            ms_Instance;

    guard_files::IncomingFile m_incomingFile;
    bool m_receiving = false;

    typedef void (CGDMessageHandler::*GUARD_MESSAGE_FUNC)(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	std::array<GUARD_MESSAGE_FUNC, InternalMessage::m2a_end> m_GuardMessageFuncList{};
};

#endif  // _MESSAGE_HANDLER_H_
