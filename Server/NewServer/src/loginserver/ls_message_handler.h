#ifndef _LS_MESSAGE_HANDLER_H_
#define _LS_MESSAGE_HANDLER_H_

#include "net.h"
#include "internal_message_header.h"

struct GATE_SESSION;

class CLSMessageHandler : public INetEventHandler
{
public:
    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession);
    virtual void OnConnected(ISocketSession* pSocketSession);
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown);

public:
    CLSMessageHandler();
    ~CLSMessageHandler();

    BOOL Init(INet* pNet, IPacketAllocator* pPacketAllocator);
    BOOL UnInit(void);
    static CLSMessageHandler& GetInstance(void);

public:
    /************************************************************************/
    // To All
    BOOL DoAllPing(void);
    
    /************************************************************************/
    // To Gd
    BOOL DoGDRegister();
    /************************************************************************/
    // To MS
    BOOL DoMSRegister(UINT32 dwGTAddr, UINT16 wGTPort);
    BOOL DoMSReady(void);
    BOOL DoMSCloseAck(void);
    BOOL DoMsLoginQueryReq(UINT32 dwClientSessionID, CHAR* szUserName, CHAR* szPassword);

    /************************************************************************/
    // To GT
    BOOL DoGTRegisterAck(UINT32 dwGTSession);
    BOOL DoGTLoginAck(UINT32 dwClientSessionID, INT32 nErrorCode, const CHAR* szOASessionID, const CHAR* szUserName, const CHAR* szPGT, UINT8 bAdult, UINT16 wInfoLen, const BYTE* abyInfo);

	/************************************************************************/
	// To MG
	BOOL DoMGRegisterAck(UINT32 dwMGSession);
	BOOL DoMGLoginAck(UINT32 dwClientSessionID, INT32 nErrorCode, const CHAR* szOASessionID, const CHAR* szUserName, const CHAR* szPGT, UINT8 bAdult, UINT16 wInfoLen, const BYTE* abyInfo);

private:
    /************************************************************************/
    // From All
    void OnAllPing(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

    /************************************************************************/
    // From MS
    void OnMSRegisterAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    void OnMSAllReady(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    void OnMSServerInfo(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    void OnMSClose(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

    void OnMSLoginQueryAck(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

    /************************************************************************/
    // From GT
    void OnGTRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    void OnGTLoginReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

	/************************************************************************/
	// From MG
	void OnMGRegister(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
	void OnMGLoginReq(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);

private:
    inline BOOL _SendGD(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    inline BOOL _SendMS(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
    inline BOOL _SendGT(UINT32 dwGTSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);
	inline BOOL _SendMG(UINT32 dwMGSessionID, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INT32 nOffset = 0);

private:
    INet*                               m_pNet;
    IPacketAllocator*                   m_pPacketAllocator;
    static CLSMessageHandler            ms_Instance;

    typedef void (CLSMessageHandler::*MESSAGE_FUNC)(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    MESSAGE_FUNC                        m_MessageFuncList[InternalMessage::max_message_count];
};

#endif  // _LS_MESSAGE_HANDLER_H_
