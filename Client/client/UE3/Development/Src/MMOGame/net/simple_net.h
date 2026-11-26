#ifndef _SIMPLE_NET_H_
#define _SIMPLE_NET_H_

#include <winsock2.h>

#include "base/base_list.h"
#include "base/circle_list.h"
#include "base/free_object_pool.h"
#include "external_message_header.h"

#include "net.h"

namespace SimpleNet
{   
    class CSocketSession;
    static CSocketSession* g_SessionList[65536] = {NULL};
    static const INT32 MAX_SOCKET_FD = 65535;

    enum ESessionState
    {
        esccUnConnect = 0,
        esccConnecting,
        esccListening,

        esccConnected,
        esccLocalClosing,
        esccLocalClosingWaitRecv,
        esccLocalClosed,

        esccRemoteClosingWaitSend,
        esccRemoteClosed,
    };

    enum ESessionOperate
    {
        esoListen = 0,
        esoConnect,
        esoOnConnect,
        esoOnAccept,
        esoConnected,
        esoOnReceive,
        esoDisConnect,
        esoOnDisConnect
    };

    struct SSessionMsg
    {
        ESessionOperate eOperate;
        INT32   nSocketFD;
    };

    union Record
    {
        struct SendRecord
        {
            UINT16 wAlreadySend;
            UINT16 wNeedSend;
        }stSendRecord;

        struct RecvRecord
        {
            UINT16 wPackageSize;
            UINT16 wNeedRecv;
        }stRecvRecord;
    };

    struct SIMPLE_NET_HEADER
    {
        UINT32  dwBufferSize;
        UINT32  dwState;
        INT32   nOffset;
        Record  stRecord;
        INT32   nReserved[2];
    };

    class CSocketAcceptor;
    class CSocketSession : public ISocketSession
    {
    public:
        virtual INT32 Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset);

        virtual void* GetUserData(void);
        virtual void SetUserData(void* pUserData);
        virtual BOOL GetAddress(UINT32& dwAddr, UINT16& wPort);
        virtual BOOL GetRemoteAddress(UINT32& dwRemoteAddr, UINT16& wRemotePort);

        virtual BOOL Close(void);
    public:
        CSocketSession();
        ~CSocketSession();
        
    public:
        BOOL Init(INet* poNet);
        BOOL UnInit(void);

        inline CCircleList& GetSendMsgList();
        inline CCircleList& GetRecvMsgList();
        
        inline VOID   SetCurRecvMsgSize(UINT16 wMsgSize);
        inline UINT16 GetCurRecvMsgSize();

        CHAR* GetRecvBuff();
        INT32 GetNeedRecvSize();

        BOOL GetRecvMsg(PACKET_BUFFER*& pPackBuffer, INT32& nSize);
        BOOL IsRecvMsg();
        BOOL IsSendMsg();
        BOOL ClearSendMsg();

        inline VOID SetSocketID(INT32 nSocketFD);
        inline UINT32 GetSocketID();

        inline VOID SetState(ESessionState eState);
        inline ESessionState GetState();

        inline VOID SetAcceptor(CSocketAcceptor* poAcceptor);
        inline CSocketAcceptor* GetAcceptor();
    public:
        BASE_LIST_NODE m_Node;

        CCircleList m_oSendMsg;
        CCircleList m_oRecvMsg;
        
        UINT16         m_wRecvMsgSize;
        CHAR           m_szTempMsg[64 * 1024 + sizeof(FLEX_MESSAGE_HEADER)];

        INet* m_poNet;
        SOCKET m_nSockedFD;
        ESessionState m_eState;

        BOOL m_bClose;
        CSocketAcceptor* m_poAcceptor;
        VOID* m_poUserData;
    };

    class CSocketAcceptor : public ISocketAcceptor
    {
    public:
        virtual BOOL Open(const char* pcszIPAddress, UINT16 wPort);
        virtual BOOL Close(void);

    public:
        CSocketAcceptor();
        ~CSocketAcceptor();

        BOOL Init(INet* pNet);
        BOOL UnInit(void);

        inline VOID SetSession(CSocketSession* poSession);
        inline CSocketSession* GetSession();

    private:
        CSocketSession* m_poSession;
        INet* m_poNet;
    };

    class CNet : public INet
    {
    public:
        virtual BOOL Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEvent, void* pInitParam);
        virtual BOOL UnInit(void);
        
        virtual ISocketAcceptor* CreateAcceptor(void);
        virtual BOOL DestroyAcceptor(ISocketAcceptor* pAcceptor);

        virtual ISocketSession* Connect(const char* pcszIPAddress, UINT16 wPort);
        virtual ISocketSession* Connect(UINT32 dwIPAddress, UINT16 wPort);
        virtual BOOL MainLoop(void);
    public:
        CNet();
        ~CNet();
       
        ISocketSession* CreateSession(void);
        void DestroySession(ISocketSession* pSession);

        inline PACKET_BUFFER* AllocBuffer(INT32 nSize);
        inline VOID           FreeBufer(PACKET_BUFFER* poBuffer);

        inline UINT32 GetPacketBufferSize();
        BOOL PushMsgToMain(SSessionMsg* poMsg);
        BOOL PushMsgToWork(SSessionMsg* poMsg);
        SSessionMsg* PopWorkMsg();

        BOOL IsThreadRuning(){return m_bThreadRuning;}
        BOOL IsUnProcessMsg();
    public:
        static DWORD STDCALL _NetProc(VOID* poNet);

        BOOL UpdateWorkingSocket(fd_set& stListenSet, fd_set& stConnectingSet, fd_set& stConnectedSet, fd_set& stClosingSet);
        BOOL UpdateExceptSocket(fd_set& stExeptSet, fd_set& stConnectingSet);
        BOOL UpdateWriteSocket(fd_set& stWriteSet, fd_set& stConnectingSet, fd_set& stConnectedSet, fd_set& stClosingSet);
        BOOL UpdateReadSocket(fd_set& stReadSet, fd_set& stListenSet, fd_set& stConnectedSet, fd_set& stClosingSet);
        BOOL UpdateClosedSocket(fd_set& stClosingSet);

    private:
        CFreeObjectPool<CSocketSession> m_oSessionPool;
        CFreeObjectPool<CSocketAcceptor> m_oAcceptorPool;
        
        CCircleList m_MsgListToWork;
        CCircleList m_MsgListToMain;

        fd_set m_ConnetedSet;
        BOOL m_bThreadRuning;
        //SGDP::CSDThread oNetThread;
		//SGDP::PFThrdProc m_ThrdProc;
		HANDLE m_ThreadHandle;

        SIMPLE_INIT_PARAM m_oInitParam;
    private:
        IPacketAllocator*  m_pPacketAllocator;
        INetEventHandler*  m_pNetHandler;
    private:
        INT16 m_nMessageSizeList[ExternalMessage::max_message_count];
    };

#include "simple_net.inl"
}

#endif  // _SIMPLE_NET_H_