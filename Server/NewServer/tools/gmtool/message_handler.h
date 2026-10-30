/********************************************************************
	created:	6:8:2013   11:25
	file:		message_handler.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#ifndef _H_MESSAGE_HANDLER_H_
#define _H_MESSAGE_HANDLER_H_


#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "net.h"
#include <array>
#include <span>
#include <memory>
//#include "message_notifier.h"
#include "internal_message_header.h"


class CMessageHandler : public INetEventHandler
{
public:
    virtual void OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession);
    virtual void OnConnected(ISocketSession* pSocketSession);
    virtual void OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize);
    virtual void OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown);

public:
    CMessageHandler();
    ~CMessageHandler();

    static CMessageHandler &GetInstance();

    BOOL Init(INet *pNet, IPacketAllocator *pPacketAllocator,std::shared_ptr<void> lifetime={});
    BOOL UnInit();
    void ReleaseAllocator(IPacketAllocator* allocator);

public:
    void OnAllPing(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    BOOL DoAllPing();

    // to gm
    BOOL DoRegisterToGM(UINT32 dwGMSessionID);

    BOOL DoVerify(UINT32 dwGMSessionID, const char *pcszName, const char *pcszPwd);
    BOOL DoLoginToGM(UINT32 dwGMSessionID, const char *pcszName, const char *pcszPwd);
    BOOL DoLogoutToGM(UINT32 dwGMSessionID);

    BOOL DoPreQuery(const char *pcszRoleName);
    BOOL DoQueryRoleID(INT32 nQueryType, const char *pcszName);
    BOOL DoQueryRoleData(UINT32 dwRoleID);
    BOOL DoForbidChat(UINT32 dwTime, UINT32 dwRoleID);
    BOOL DoFreezeRole(UINT32 dwTime, UINT32 dwRoleID);
    BOOL DoKickRole(UINT32 dwRoleID);
    BOOL DoSendMail(UINT32 dwRoleID, const char *pcszSubject, const char *pcszContent, const char *pcszReason, UINT32 dwItemID, INT32 nItemNum);
    BOOL DoAnnounce(INT32 nGroupID, UINT32 dwBeginTime, INT32 nRepeatNum, INT32 nIntervalTime, const char *pcszText);
    BOOL DoGetAnnounceList(INT32 nGroupID);
    BOOL DoDelAnnounce(INT32 nGroupID, UINT32 dwAnnounceSerial);
    BOOL DoLog(INT32 nLogType, UINT32 dwBeginTime, UINT32 dwEndTime, const char *pcszQueryStr);
    BOOL DoExecuteGMStrCmd(const char* pcszStrCmd);
    BOOL DoQueryNameToID(INT32 nIndex, const char *pcszRoleName);

    BOOL DoLoadRoleData(UINT32 dwRoleID);
    BOOL DoModifyRoleData(UINT32 dwRoleID, UINT32 dwTargetRoleID, INT32 nLength, const char *pcszData, const char *pcszReason);
    BOOL DoReleaseRole(UINT32 dwGMSessionID, UINT32 dwRoleID);

    BOOL DoExeRoleGMCmd(UINT32 dwRoleID, const char *pcszGM, const char *pcszReason);

    BOOL DoSelectGroup(INT32 nGroupID);
    BOOL DoQueryExchangeLog(INT32 nGroupID);
    BOOL DoQueryExchangeHistory(INT32 nGroupID);
    BOOL DoQueryOrderDetail(INT32 nGroupID, UINT8 byOrderType, INT32 nPrice);
    BOOL DoAddSystemOrder(INT32 nGroupID, INT32 nPrice, INT32 nCount);
    BOOL DoDelSystemOrder(INT32 nGroupID, UINT64 qwOrderID);
    BOOL DoQuerySysExchangeInfo(INT32 nGroupID, INT32 nFlag, UINT32 dwBeginToday, UINT32 dwEndToday, UINT32 dwBeginHour, UINT32 dwEndHour);
    BOOL DoQueryExchangeLimit(INT32 nGroupID);
    BOOL DoSetExchangeLimit(INT32 nGroupID, INT32 nPriceHigh, INT32 nPriceLow, INT32 nSingleOrderLimit, INT32 nDailyOrderLimit);

protected:
    void OnGMRegisterAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMVerifyAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMLoginAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMItemListNtf(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMLogoutAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMPreQueryAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQueryRoleID(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQueryRoleDataAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);

    void OnGMForbidChatAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMFreezeRoleAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMKickRoleAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMSendMailAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMAnnouceAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMGetAnnounceAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMDelAnnounce(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMLogAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMExecuteGMStrAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMLoadRoleDataAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMModifyRoleDataAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQueryNameToIDAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMErrorStr(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMRoleExeGMCmd(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);

    void OnGMSelectGroupAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQueryExchangeLogAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQueryExchangeHistoryAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMExchangePrice(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQueryOrderDetailAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMAddSystemOrderAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMDelSystemOrderAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMQuerySysExchangeInfoAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);

    void OnGMGetExchangeLimitAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);
    void OnGMSetExchangeLimitAck(PACKET_BUFFER *pPacketBuffer, UINT32 dwSize, ISocketSession *pSocketSession);

private:
    BOOL _SendGM(UINT32 session, std::span<const char> packet);
    BOOL _BroadcastGM(std::span<const char> packet);

private:
    INet*                               m_pNet = nullptr;
    IPacketAllocator*                   m_pPacketAllocator = nullptr;
    bool                                m_accepting = false;
    UINT64                              m_generation = 0;
    std::shared_ptr<void>                m_lifetime;
    static CMessageHandler              ms_Instance;

    typedef void (CMessageHandler::*MESSAGE_FUNC)(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, ISocketSession* pSocketSession);
    std::array<MESSAGE_FUNC, InternalMessage::max_message_count> m_MessageFuncList{};

};











#endif //_H_MESSAGE_HANDLER_H_



