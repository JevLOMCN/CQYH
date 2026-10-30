#ifndef _CERT_MESSAGE_HANDLER_H_
#define _CERT_MESSAGE_HANDLER_H_

#include "net.h"
#include <vector>

class CCertMessageHandler : public INetEventHandler
{
public:
    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession);
    virtual void OnConnected(ISocketSession* pSocketSession);
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown);

public:
    CCertMessageHandler();
    ~CCertMessageHandler();
    CCertMessageHandler(const CCertMessageHandler&) = delete;
    CCertMessageHandler& operator=(const CCertMessageHandler&) = delete;

    BOOL Init(INet* pNet, IPacketAllocator* pPacketAllocator);
    BOOL UnInit(void);
    void BeginShutdown() noexcept { m_accepting = false; }

    static CCertMessageHandler& GetInstance(void);
public:

private:
    INet*                           m_pNet;
    IPacketAllocator*               m_pPacketAllocator;
    std::vector<char> m_certificate;
    bool m_accepting = false;
};

extern CCertMessageHandler g_MessageHandler;

#endif  // _CERT_MESSAGE_HANDLER_H_
