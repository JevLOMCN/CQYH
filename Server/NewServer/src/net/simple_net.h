#ifndef _SIMPLE_NET_H_
#define _SIMPLE_NET_H_

#include "base_list.h"
#include "circle_list.h"
#include "callback_queue.h"
#include "free_object_pool.h"
#include "net.h"
#include "thread.h"
#include "time_ex.h"
#include "linux_compatible.h"

#if defined(WIN32)
    //#define FD_SETSIZE      64
	#include "winsock2.h"
	#define NOMINMAX
	#include <Windows.h>
#elif defined(__linux__)
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>

	#include <sys/time.h>
	#include <sys/select.h>
#endif 

namespace SimpleNet
{   
	
// for socket difference in win32 and linux
#if defined(WIN32)
    #undef errno
	#define errno WSAGetLastError()
	#define close(a) closesocket(a)
	typedef int socklen_t;

	#define EWOULDBLOCK 	WSAEWOULDBLOCK
	#define ECONNRESET 		WSAECONNRESET
	#define ECONNABORTED 	WSAECONNABORTED
	#define ESHUTDOWN 		WSAESHUTDOWN

#elif defined(__linux__)
	typedef int SOCKET;
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR 		(-1) 

	typedef sockaddr_in SOCKADDR_IN;
	typedef sockaddr SOCKADDR;
	typedef linger LINGER;
	
	// for shutdown
	#define SD_SEND SHUT_WR
	#define SD_RECV SHUT_RD
#endif

// for select difference in win32 and linux
#if defined(WIN32)
	
	typedef fd_set wfd_set;
	#define WFD_CLR(fd, set) FD_CLR(fd, set)
	#define WFD_SET(fd, set) FD_SET(fd, set)
	#define WFD_ZERO(set) FD_ZERO(set)
	#define WFD_ISSET(fd, set) FD_ISSET(fd, set)

#elif defined(__linux__)

	// almost completely copy from winsock2.h
	#ifndef FD_SETSIZE
		#define FD_SETSIZE      64
	#endif //FD_SETSIZE
	
	typedef struct wfd_set {
	        int fd_count;               // how many are SET? 
	        SOCKET  fd_array[FD_SETSIZE]; // an array of SOCKETs
	} wfd_set;
	
	#define WFD_ZERO(set) ((set)->fd_count=0)

	bool _wfd_isset( SOCKET fd, wfd_set *set ); 
	void _wfd_clr( SOCKET fd, wfd_set *set ); 
	void _wfd_set( SOCKET fd, wfd_set *set ); 

	#define WFD_ISSET(fd, set) _wfd_isset( (fd), (set) )
	#define WFD_CLR(fd, set) _wfd_clr( (fd), (set) )
	#define WFD_SET(fd, set) _wfd_set( (fd), (set) )

	BOOL linux_select( wfd_set* _stReadSet, wfd_set* _stWriteSet, wfd_set* _stExeptSet, struct timeval * stTimeout );

#endif	// WIN32
	
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
        esoOnReceive,   // no use
        esoDisConnect,
        esoOnDisConnect
    };

    struct SSessionMsg
    {
        ESessionOperate eOperate;
        SOCKET			nSocketFD;
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

	union SIMPLE_NET_HEADER
	{
		struct
		{
			UINT32  dwBufferSize;
			INT32   nOffset;
			Record  stRecord;
		};
		char    Reserved[PACKET_OVERHEAD_SIZE];
	};

    class CSocketAcceptor;
    class CSocketSession : public ISocketSession
    {
    public:
        virtual INT32 Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset);
		virtual INT32 SendNoPackageHead(PACKET_BUFFER* pPacketBuffer, INT32 nSize);


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
        inline VOID  SetRecvPacketBuffer(PACKET_BUFFER* poRecvBuff);
        inline PACKET_BUFFER* GetRecvPacketBuffer();

        CHAR* GetRecvBuff();
        UINT16 GetNeedRecvSize();

        BOOL GetRecvMsg(PACKET_BUFFER*& pPackBuffer, INT32& nSize);
        BOOL IsRecvMsg();
        BOOL IsSendMsg();
        BOOL ClearSendMsg();

        inline VOID SetSocketID(SOCKET nSocketFD);
        inline SOCKET GetSocketID();

        inline VOID SetState(ESessionState eState);
        inline ESessionState GetState();

        inline VOID SetAcceptor(CSocketAcceptor* poAcceptor);
        inline CSocketAcceptor* GetAcceptor();

    private:
        BASE_LIST_NODE      m_Node;

        CCircleList         m_oSendMsg;
        CCircleList         m_oRecvMsg;
        
        UINT16              m_wRecvMsgSize;
        CHAR                m_szMsgSize[2];
        PACKET_BUFFER*      m_poRecvBuff;


        INet*               m_poNet;
        SOCKET              m_nSockedFD;
        ESessionState       m_eState;

        BOOL                m_bClose;
        CSocketAcceptor*    m_poAcceptor;
        VOID*               m_poUserData;
    };

    class CSocketAcceptor : public ISocketAcceptor
    {
    public:
        virtual BOOL Open(UINT32 dwAddr, UINT16 wPort);
        virtual BOOL Close(void);
		virtual void* GetUserData(void);
		virtual void SetUserData(void* pUserData);

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
		void* m_pUserData;
    };

    class CNet : public INet
    {
    public:
        virtual BOOL Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEvent, void* pInitParam);
        virtual BOOL UnInit(void);
        
        virtual ISocketAcceptor* CreateAcceptor(void);
        virtual BOOL DestroyAcceptor(ISocketAcceptor* pAcceptor);

        virtual ISocketSession* Connect(UINT32 dwAddr, UINT16 wPort);
        virtual BOOL MainLoop(void);

		virtual INT64 PrintMemoryStat(void);
	
	public:
        CNet();
        ~CNet();
       
        ISocketSession* CreateSession(void);
        void DestroySession(ISocketSession* pSession);

        inline PACKET_BUFFER* AllocBuffer(INT32 nSize);
        inline VOID           FreeBuffer(PACKET_BUFFER* poBuffer);

        inline UINT32 GetPacketBufferSize();
        BOOL PushMsgToMain(SSessionMsg* poMsg);
        BOOL PushMsgToWork(SSessionMsg* poMsg);
        SSessionMsg* PopWorkMsg();

        BOOL IsThreadRuning(){return m_bThreadRuning;}
        BOOL IsUnProcessMsg();
    public:
#if defined(WIN32)
        static UINT32 WINAPI _NetProc(VOID* poNet);
#elif defined(__linux__)
        static VOID*  _NetProc(VOID* poNet);
#endif 
        BOOL UpdateWorkingSocket(wfd_set& stListenSet, wfd_set& stConnectingSet, wfd_set& stConnectedSet, wfd_set& stClosingSet);
        BOOL UpdateExceptSocket(wfd_set& stExceptSet, wfd_set& stWriteSet, wfd_set& stReadSet, wfd_set& stConnectingSet);
        BOOL UpdateWriteSocket(wfd_set& stWriteSet, wfd_set& stConnectingSet, wfd_set& stConnectedSet, wfd_set& stClosingSet);
        BOOL UpdateReadSocket(wfd_set& stReadSet, wfd_set& stListenSet, wfd_set& stConnectingSet, wfd_set& stConnectedSet, wfd_set& stClosingSet);
        BOOL UpdateClosedSocket(wfd_set& stClosingSet);

    private:
		BOOL _ProcessFailedSocket( SOCKET nSockErrorFD, wfd_set& stConnectingSet );

        CFreeObjectPool<CSocketSession> m_oSessionPool;
        CFreeObjectPool<CSocketAcceptor> m_oAcceptorPool;
        
        // Owned control events may be queued in both directions at once.
        cqyh::CallbackQueue<SSessionMsg> m_MsgListToWork;
        cqyh::CallbackQueue<SSessionMsg> m_MsgListToMain;

        wfd_set m_ConnetedSet;
        std::atomic<bool> m_bThreadRuning{false};
        CThread oNetThread;
        PFThrdProc m_ThrdProc;

        SIMPLE_INIT_PARAM m_oInitParam;
    private:
        IPacketAllocator*  m_pPacketAllocator;
        INetEventHandler*  m_pNetHandler;
    };

#include "simple_net.inl"
}

#endif  // _SIMPLE_NET_H_
