#include "stdafx.h"
#include "simple_net.h"
#include "packet_allocator.h"
#include "base/process_error.h"

#include "external_message_size.h"
#include "..\..\Common\define\version.h" // oscar@snda

using namespace ExternalMessage;

extern void CreateCustomDump(const TCHAR* fileName);

#define MAX_SESSION_COUNT   (64)
#define MAX_ACCEPTER_COUNT  (16)

extern INT32 g_nS2CMessageSize[];
extern char* g_szC2SMessageName[];
extern char* g_szS2CMessageName[];

TCHAR* g_ESessionState[] =
{
	TEXT("esccUnConnect"),
	TEXT("esccConnecting"),
	TEXT("esccListening"),

	TEXT("esccConnected"),
	TEXT("esccLocalClosing"),
	TEXT("esccLocalClosingWaitRecv"),
	TEXT("esccLocalClosed"),

	TEXT("esccRemoteClosingWaitSend"),
	TEXT("esccRemoteClosed")
};

#if 0

void _DumpNetworkCallStack()
{
	// Initialize stack walking... loads up symbol information.
	appInitStackWalking();

	// Capture C++ callstack
	const UINT MaxDepth = 200;
	QWORD FullBackTrace[MaxDepth];
	memset( FullBackTrace, 0, sizeof(FullBackTrace) );
	appCaptureStackBackTrace( FullBackTrace, MaxDepth );

	TArray<FString> CallStackList;

	FString tmpStr;

	// Iterate over all addresses in the callstack to look up symbol name. Skipping 2 first entries.
	for( INT AddressIndex=1; AddressIndex<ARRAY_COUNT(FullBackTrace) && FullBackTrace[AddressIndex]; AddressIndex++ )
	{
		ANSICHAR AddressInformation[512];
		AddressInformation[0] = 0;
		appProgramCounterToHumanReadableString( FullBackTrace[AddressIndex], AddressInformation, ARRAY_COUNT(AddressInformation)-1, VF_DISPLAY_BASIC );
		FString CallStackString = FString(AddressInformation);
		if(!CallStackString.IsEmpty())
		{
			CallStackList.AddItem(CallStackString);
			tmpStr += CallStackString + TEXT("\r\n");
		}
	}

	warnf(NAME_Log, TEXT("%s-------------\n"), *tmpStr);
}

#	define M_DUMP_NETWORK_STACK _DumpNetworkCallStack()
#else
#	define M_DUMP_NETWORK_STACK __noop
#endif

namespace SimpleNet
{

	VOID CSocketSession::SetState(ESessionState eState)
	{
		warnf(NAME_Log, TEXT("==Network[%x]== CSocketSession::SetState %s -> %s"), (INT)m_nSockedFD, g_ESessionState[m_eState], g_ESessionState[eState]);
		M_DUMP_NETWORK_STACK;

		m_eState = eState;
	}

    INT32 CSocketSession::Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset)
    {
		//debugf(NAME_Log, TEXT("==Network[%x]== CSocketSession::Send m_eState:%s"), (INT)m_nSockedFD, g_ESessionState[m_eState]);
		M_DUMP_NETWORK_STACK;
        INT32 nRetCode;
        LOG_PROCESS_ERROR(nSize > 0);
        LOG_PROCESS_ERROR(!m_bClose);

        LOG_PROCESS_ERROR(m_eState == esccConnected);

        SIMPLE_NET_HEADER* pHeader = (SIMPLE_NET_HEADER*)((CHAR*)pPacketBuffer - sizeof(SIMPLE_NET_HEADER));
        pHeader->stRecord.stSendRecord.wAlreadySend = 0;
        pHeader->stRecord.stSendRecord.wNeedSend = nSize;

        nRetCode = m_oSendMsg.Push((VOID*)pPacketBuffer);
        LOG_PROCESS_ERROR(nRetCode);
        
        //warnf( TEXT("[m_oSendMsg::Push] %x"), (void*)pPacketBuffer);

        //static INT64 qwSend;
        //qwSend++;
        //if(qwSend % (1024*32) == 0)
        //    INF("qwSend:%d", qwSend/(1024*32));  
        return nSize;
Exit0:  
        if(pPacketBuffer && m_poNet)
            ((CNet*)m_poNet)->FreeBufer(pPacketBuffer);
        if(!m_bClose)
            Close();

        //log
        {
            INT32 nMsgCount[1024]={0};
            PACKET_BUFFER* pBuffer = (PACKET_BUFFER*)m_oSendMsg.Pop();
            while(pBuffer)
            {   
                MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pBuffer;
                nMsgCount[pHeader->wMsg]++;

                pBuffer = (PACKET_BUFFER*)m_oSendMsg.Pop();
            }

            for(INT32 i = 0; i < 1024; ++i)
            {
				if(nMsgCount[i] > 0)
				{
					warnf(NAME_Error, TEXT("current send buf msg[%d, %d]"), i, nMsgCount[i]);
				}
            }
        }
        return 0;
    }

    BOOL CSocketSession::IsSendMsg()
    {
        return (m_oSendMsg.Check() != NULL);
    }

    BOOL CSocketSession::ClearSendMsg()
    {
        while(m_oSendMsg.Check())
        {
            ((CNet*)m_poNet)->FreeBufer((PACKET_BUFFER*)m_oSendMsg.Pop());
        }

        return TRUE;
    }

    BOOL CSocketSession::IsRecvMsg()
    {
        return (m_oRecvMsg.Check() != NULL);
    }


    BOOL CSocketSession::GetRecvMsg(PACKET_BUFFER*& pPackBuffer, INT32& nSize)
    {
		//M_DUMP_NETWORK_STACK;

        SIMPLE_NET_HEADER* pHeader = NULL;
        pPackBuffer = NULL;
        INT16* poPackLen = NULL;
        nSize = 0;

        pPackBuffer = (PACKET_BUFFER*)m_oRecvMsg.Pop();
        LOG_PROCESS_ERROR(pPackBuffer != NULL);
        
        //warnf( TEXT("[m_oRecvMsg::Pop] %x"), (void*)pPackBuffer);


        pHeader = (SIMPLE_NET_HEADER*)((CHAR*)pPackBuffer - sizeof(SIMPLE_NET_HEADER));
        LOG_PROCESS_ERROR(pHeader != NULL);

        nSize = pHeader->stRecord.stRecvRecord.wPackageSize;
        return TRUE;
Exit0:
        if(pPackBuffer && m_poNet)
            ((CNet*)m_poNet)->FreeBufer(pPackBuffer);
        
        if(!m_bClose)
            Close();

        return FALSE;
    }

    void* CSocketSession::GetUserData(void)
    {
        return m_poUserData;
    }

    void CSocketSession::SetUserData(void* pUserData)
    {
        m_poUserData = pUserData;
    }

    BOOL CSocketSession::GetAddress(UINT32& dwAddr, UINT16& wPort)
    {
        INT32 nRetCode;
        SOCKADDR_IN stSockAddr;
        INT32 nSockSize = sizeof(stSockAddr);

        LOG_PROCESS_ERROR(m_poNet != NULL);
        LOG_PROCESS_ERROR(m_nSockedFD != INVALID_SOCKET);
        LOG_PROCESS_ERROR(m_eState == esccConnected || m_eState == esccListening);

        nRetCode = getsockname(m_nSockedFD, ((SOCKADDR*)&stSockAddr), &nSockSize);
        LOG_PROCESS_ERROR(nRetCode == 0);
        
        dwAddr = stSockAddr.sin_addr.s_addr;
        wPort = ntohs(stSockAddr.sin_port);

        return TRUE;
Exit0:
        return FALSE;
    }

    BOOL CSocketSession::GetRemoteAddress(UINT32& dwRemoteAddr, UINT16& wRemotePort)
    {
        INT32 nRetCode;
        SOCKADDR_IN stSockAddr;
        INT32 nSockSize = sizeof(stSockAddr);

        LOG_PROCESS_ERROR(m_poNet != NULL);
        LOG_PROCESS_ERROR(m_nSockedFD != INVALID_SOCKET);
        LOG_PROCESS_ERROR(m_eState == esccConnected);

        nRetCode = getpeername(m_nSockedFD, ((SOCKADDR*)&stSockAddr), &nSockSize);
        LOG_PROCESS_ERROR(nRetCode == 0);

        dwRemoteAddr = stSockAddr.sin_addr.s_addr;
        wRemotePort = ntohs(stSockAddr.sin_port);

        return TRUE;
Exit0:
        return FALSE;
    }

    CSocketSession::CSocketSession()
    {
        m_wRecvMsgSize = 0;
        m_nSockedFD = INVALID_SOCKET;
        m_Node.pPrev = m_Node.pNext = NULL;
        m_eState = esccUnConnect;
        m_poNet = NULL;
        m_poAcceptor = NULL;
        m_poUserData = NULL;
        m_bClose = FALSE;

		warnf(NAME_Log, TEXT("==Network[%x]== CSocketSession::CSocketSession m_eState:%s"), (INT)m_nSockedFD, g_ESessionState[m_eState]);
        return;
    }

    CSocketSession::~CSocketSession()
    {
        UnInit();
    }

    BOOL CSocketSession::Init(INet* poNet)
    {
		M_DUMP_NETWORK_STACK;

        m_eState = esccUnConnect;
        m_nSockedFD = INVALID_SOCKET;
        m_Node.pPrev = m_Node.pNext = NULL;
        m_poNet = poNet;
        m_poUserData = NULL;
        m_wRecvMsgSize = 0;

        m_oSendMsg.Init(((CNet*)m_poNet)->GetPacketBufferSize());
        m_oRecvMsg.Init(((CNet*)m_poNet)->GetPacketBufferSize());

        m_bClose = FALSE;

		warnf(NAME_Log, TEXT("==Network[%x]== CSocketSession::Init m_eState:%s"), (INT)m_nSockedFD, g_ESessionState[m_eState]);

        return TRUE;
    }

    BOOL CSocketSession::UnInit(void)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode = 0;
        m_Node.pPrev = m_Node.pNext = NULL;

        if(m_nSockedFD != INVALID_SOCKET)
        {
			warnf(NAME_Log, TEXT("==Network[%x]== CSocketSession::UnInit m_eState:%s -> esccLocalClosed"), (INT)m_nSockedFD, g_ESessionState[m_eState]);
            nRetCode = closesocket(m_nSockedFD);
            LOG_CHECK_ERROR(nRetCode == 0);
            
            m_nSockedFD = INVALID_SOCKET;
            m_eState = esccLocalClosed;
        }

        while(m_oRecvMsg.Check())
        {
            ((CNet*)m_poNet)->FreeBufer((PACKET_BUFFER*)m_oRecvMsg.Pop());
        }

        while(m_oSendMsg.Check())
        {
            ((CNet*)m_poNet)->FreeBufer((PACKET_BUFFER*)m_oSendMsg.Pop());
        }

        m_poNet = NULL;    
		m_oRecvMsg.UnInit();
        m_oSendMsg.UnInit();
        
		SET_WARN_COLOR(COLOR_CYAN);
		warnf( NAME_Log, TEXT("=====####========== [CSocketSession::UnInit][%x]"), m_nSockedFD );
		CLEAR_WARN_COLOR();
        return TRUE;
    }

    INT32 CSocketSession::GetNeedRecvSize()
    {
        INT32 nNeedRecvSize = sizeof(MESSAGE_HEADER);
        INT32 nMsgSize = 0;
        UINT16 messageID = 0;

        if(m_wRecvMsgSize < sizeof(MESSAGE_HEADER))
        {
            nNeedRecvSize = sizeof(MESSAGE_HEADER) - m_wRecvMsgSize;
        }
        else if(m_wRecvMsgSize >= sizeof(MESSAGE_HEADER))
        {
            memcpy(&messageID, m_szTempMsg, sizeof(messageID));
            LOG_PROCESS_ERROR(messageID < ExternalMessage::max_message_count);
            nMsgSize = g_nS2CMessageSize[messageID];
            if(nMsgSize == 0)
            {
                //warnf(NAME_Error, TEXT("the message[%d] UnRegrister"), (UINT8)m_szTempMsg[0]);
                LOG_PROCESS_ERROR(FALSE);
            }
            else if(nMsgSize == flex_message_size)
            {
                if(m_wRecvMsgSize < sizeof(FLEX_MESSAGE_HEADER))
                    nNeedRecvSize = sizeof(FLEX_MESSAGE_HEADER) - m_wRecvMsgSize;
                else
                {
                    UINT16 packetSize = 0;
                    memcpy(&packetSize, &m_szTempMsg[2], sizeof(packetSize));
                    LOG_PROCESS_ERROR(packetSize >= sizeof(FLEX_MESSAGE_HEADER));
                    nNeedRecvSize = (INT32)packetSize - m_wRecvMsgSize;
                }
            }
            else if(nMsgSize > 0)
            {
                nNeedRecvSize = nMsgSize - m_wRecvMsgSize;
            }
            else
            {
                LOG_PROCESS_ERROR(FALSE);
            }
        }
        LOG_PROCESS_ERROR(nNeedRecvSize >= 0);
        LOG_PROCESS_ERROR((UINT32)m_wRecvMsgSize + nNeedRecvSize <= 0xFFFFu);
        LOG_PROCESS_ERROR((UINT32)m_wRecvMsgSize + nNeedRecvSize <= sizeof(m_szTempMsg));
        return nNeedRecvSize;
Exit0:
        return flex_message_size;
    }

    CHAR* CSocketSession::GetRecvBuff()
    {
        return &m_szTempMsg[m_wRecvMsgSize];
    }

    BOOL CSocketSession::Close(void)
    {
		M_DUMP_NETWORK_STACK;
		
        INT32 nRetCode = 0;
        
        LOG_PROCESS_ERROR(m_poNet != NULL);
        LOG_PROCESS_ERROR(m_nSockedFD != INVALID_SOCKET);

        SSessionMsg* poMsg = new SSessionMsg;
        LOG_PROCESS_ERROR(poMsg);

        poMsg->nSocketFD = m_nSockedFD;
        poMsg->eOperate = esoDisConnect;
        ((CNet*)m_poNet)->PushMsgToWork(poMsg); 

        m_bClose = TRUE;
        return TRUE;   
Exit0:
        return FALSE;
    }

    /************************************************************************/
    BOOL CSocketAcceptor::Open(const char* pcszIPAddress, UINT16 wPort)
    {
		M_DUMP_NETWORK_STACK;

        SOCKADDR_IN stSockAddr;
        INT32 nRetCode;
        UINT32 dwNonBlock = 1;
        UINT32 dwAddr = INADDR_NONE;
        SOCKET nSocketFD = INVALID_SOCKET;
        CSocketSession* poNewSession = NULL;
        SSessionMsg* poMsg = NULL;

        LOG_PROCESS_ERROR(m_poSession == NULL);
        LOG_PROCESS_ERROR(pcszIPAddress);
        LOG_PROCESS_ERROR(m_poNet);

        poNewSession = (CSocketSession*)((CNet*)m_poNet)->CreateSession();
        LOG_PROCESS_ERROR(poNewSession);

        nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);

        while(nSocketFD > MAX_SOCKET_FD)
        {
            closesocket(nSocketFD);
            nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);
        }
        dwAddr = inet_addr(pcszIPAddress);
        LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

        memset(&stSockAddr, 0, sizeof(stSockAddr));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_addr.s_addr = dwAddr;
        stSockAddr.sin_port = htons(wPort);

        nRetCode = bind(nSocketFD, ((SOCKADDR*)&stSockAddr), sizeof(stSockAddr));
        LOG_PROCESS_ERROR(nRetCode == 0);

        nRetCode = ioctlsocket(nSocketFD, FIONBIO, (u_long*)&dwNonBlock);
        LOG_PROCESS_ERROR(nRetCode == 0);

        nRetCode = listen(nSocketFD, 32);
        LOG_PROCESS_ERROR(nRetCode == 0);

        g_SessionList[nSocketFD] = poNewSession;
        poNewSession->SetAcceptor(this);
        poNewSession->SetSocketID(nSocketFD);

        m_poSession = poNewSession;

        poMsg = new SSessionMsg;
        LOG_PROCESS_ERROR(poMsg);
        poMsg->eOperate = esoListen;
        poMsg->nSocketFD = nSocketFD;
        
        ((CNet*)m_poNet)->PushMsgToWork(poMsg);

        return TRUE;
Exit0:
        SAFE_DELETE(poMsg);

        if(nSocketFD != INVALID_SOCKET)
            closesocket(nSocketFD);
        
        if(poNewSession != NULL)
        {
            ((CNet*)m_poNet)->DestroySession(poNewSession);
            poNewSession = NULL;
            m_poSession = NULL;
        }

        return FALSE;
    }

    BOOL CSocketAcceptor::Close(void)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode = 0;
        
        if(m_poSession && m_poSession->GetSocketID() != INVALID_SOCKET)
        {
            SSessionMsg* poMsg = new SSessionMsg;
            LOG_PROCESS_ERROR(poMsg != NULL);
            poMsg->eOperate = esoDisConnect;
            poMsg->nSocketFD = m_poSession->GetSocketID();

            ((CNet*)m_poNet)->PushMsgToWork(poMsg);
        }

        if(m_poSession)
        {
            ((CNet*)m_poNet)->DestroySession(m_poSession);
            m_poSession = NULL;
        }

        return TRUE;
Exit0:
        return FALSE;
    }

    CSocketAcceptor::CSocketAcceptor()
    {
        m_poSession = NULL;
        m_poNet = NULL;

        return;
    }

    CSocketAcceptor::~CSocketAcceptor()
    {
        UnInit();
    }

    BOOL CSocketAcceptor::Init(INet* pNet)
    {        
        m_poSession = NULL;
        m_poNet = pNet;

        return TRUE;
    }

    BOOL CSocketAcceptor::UnInit(void)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode = 0;

        if(m_poSession != NULL)
        {
            INT32 nSocketFD = m_poSession->GetSocketID();
            if( nSocketFD != INVALID_SOCKET)
                g_SessionList[nSocketFD] = NULL;
            
            ((CNet*)m_poNet)->DestroySession(m_poSession);
            m_poSession = NULL;
        }

        m_poNet = NULL;

        return TRUE;
    }

    /************************************************************************/
    CNet::CNet()
		: m_ThreadHandle(NULL)
    {
        m_bThreadRuning = FALSE;
        m_pPacketAllocator = NULL;
        m_pNetHandler = NULL;
        FD_ZERO(&m_ConnetedSet);
        memset(&m_oInitParam, 0, sizeof(m_oInitParam));
        memset(m_nMessageSizeList, 0, sizeof(m_nMessageSizeList));
    }

    CNet::~CNet()
    {
        UnInit();
    }

    ISocketAcceptor* CNet::CreateAcceptor(void)
    {
		M_DUMP_NETWORK_STACK;

        CSocketAcceptor* poAcceptor = NULL;
        INT32 nRetCode = 0;

        poAcceptor = m_oAcceptorPool.New();
        LOG_PROCESS_ERROR(poAcceptor);

        nRetCode = poAcceptor->Init(this);
        LOG_PROCESS_ERROR(nRetCode);

        return poAcceptor;
Exit0:
        if(poAcceptor)
        {
            nRetCode = DestroyAcceptor(poAcceptor);
            LOG_CHECK_ERROR(nRetCode);
        }
        return NULL;
    }

    BOOL CNet::DestroyAcceptor(ISocketAcceptor* pAcceptor)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode = 0;
        PROCESS_SUCCESS(pAcceptor == NULL);

        CSocketAcceptor* poSocketAcceptor = (CSocketAcceptor*)pAcceptor;
        LOG_PROCESS_ERROR(poSocketAcceptor);
            
        poSocketAcceptor->UnInit();
        m_oAcceptorPool.Delete(poSocketAcceptor);
        return TRUE;
Exit1:
        return TRUE;
Exit0:
        return FALSE;
    }

    ISocketSession* CNet::Connect(UINT32 dwIPAddress, UINT16 wPort)
    {
		M_DUMP_NETWORK_STACK;

        CSocketSession* poNewSession = NULL;
        INT32 nRetCode = 0;
        SOCKADDR_IN stSockAddr;
        UINT32 dwNonBlock = 1;
        SOCKET nSocketFD = INVALID_SOCKET;
        INT32 nZero = 0;

        LINGER stLinger;
        stLinger.l_onoff = 1;
        stLinger.l_linger = 4;

        LOG_PROCESS_ERROR(dwIPAddress != INADDR_NONE);

        poNewSession = (CSocketSession*)CreateSession();
        LOG_PROCESS_ERROR(poNewSession);

        nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);

        while(nSocketFD > MAX_SOCKET_FD)
        {
            closesocket(nSocketFD);
            nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);
        }

        memset(&stSockAddr, 0, sizeof(stSockAddr));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_addr.s_addr = INADDR_ANY;
        stSockAddr.sin_port = 0;

        nRetCode = bind(nSocketFD, ((SOCKADDR*)&stSockAddr), sizeof(stSockAddr));
        LOG_PROCESS_ERROR(nRetCode == 0);

        nRetCode = ioctlsocket(nSocketFD, FIONBIO, (u_long*)&dwNonBlock);
        LOG_PROCESS_ERROR(nRetCode == 0);

        nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_RCVBUF, (char*)&(m_oInitParam.nRecvBufSize), sizeof(m_oInitParam.nRecvBufSize));
        LOG_CHECK_ERROR(!nRetCode);

        nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_SNDBUF, (char*)&(m_oInitParam.nSendBufSize), sizeof(m_oInitParam.nSendBufSize));
        LOG_CHECK_ERROR(!nRetCode);

        nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_LINGER, (char*)&(stLinger), sizeof(stLinger));
        LOG_CHECK_ERROR(!nRetCode);

//		setsockopt(nSocketFD, IPPROTO_TCP, TCP_NODELAY, (char*)&(dwNonBlock), sizeof(dwNonBlock));

        memset(&stSockAddr, 0, sizeof(stSockAddr));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(wPort);
        stSockAddr.sin_addr.s_addr = dwIPAddress;

        nRetCode = connect(nSocketFD, ((SOCKADDR*)&stSockAddr), sizeof(stSockAddr));
        LOG_PROCESS_ERROR(nRetCode == SOCKET_ERROR);
        LOG_PROCESS_ERROR(WSAGetLastError() == WSAEWOULDBLOCK);
        LOG_PROCESS_ERROR(g_SessionList[nSocketFD] == NULL);

        g_SessionList[nSocketFD] = poNewSession;
        poNewSession->SetSocketID(nSocketFD);

        SSessionMsg* poMsg = new SSessionMsg;
        LOG_PROCESS_ERROR(poMsg);
        poMsg->eOperate = esoConnect;
        poMsg->nSocketFD = nSocketFD;
        PushMsgToWork(poMsg);

        return poNewSession;        
Exit0:
        if(nSocketFD != INVALID_SOCKET)
            closesocket(nSocketFD);

        if(poNewSession)
            DestroySession(poNewSession);
        
        return NULL;
    }

    ISocketSession* CNet::Connect(const char* pcszIPAddress, UINT16 wPort)
    {
		M_DUMP_NETWORK_STACK;

        CSocketSession* poNewSession = NULL;
        UINT32 dwAddr = INADDR_NONE;

        LOG_PROCESS_ERROR(pcszIPAddress != NULL);

        dwAddr = inet_addr(pcszIPAddress);
        LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);

        poNewSession = (CSocketSession*)Connect(dwAddr, wPort);
        LOG_PROCESS_ERROR(poNewSession);

        return poNewSession;
Exit0:
        return NULL;
    }

    BOOL CNet::Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEvent, void* pInitParam)
    {
		M_DUMP_NETWORK_STACK;

        WSADATA  wsd;
        INT32 nRetCode = 0;
        m_bThreadRuning = FALSE;
        m_pNetHandler = NULL;

        LOG_PROCESS_ERROR(pPacketAllocator != NULL);
        LOG_PROCESS_ERROR(pNetEvent != NULL);
        LOG_PROCESS_ERROR(pInitParam != NULL);

        nRetCode = WSAStartup(MAKEWORD(2, 2), &wsd);
        LOG_PROCESS_ERROR(nRetCode == 0);
        LOG_PROCESS_ERROR(pInitParam != NULL);
        memcpy((void*)&m_oInitParam, pInitParam, sizeof(SIMPLE_INIT_PARAM));
        LOG_PROCESS_ERROR(m_oInitParam.nMaxSessionCount >= 0 && m_oInitParam.nMaxSessionCount <= 64);
        LOG_PROCESS_ERROR(m_oInitParam.nMaxAcceptor >= 0 && m_oInitParam.nMaxAcceptor <= 32);

        nRetCode = m_oSessionPool.Init(m_oInitParam.nMaxSessionCount);
        LOG_PROCESS_ERROR(nRetCode);
        nRetCode = m_oAcceptorPool.Init(m_oInitParam.nMaxAcceptor);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_MsgListToMain.Init(1024);
        LOG_PROCESS_ERROR(nRetCode);
        nRetCode = m_MsgListToWork.Init(1024);
        LOG_PROCESS_ERROR(nRetCode);
        
        m_pNetHandler = pNetEvent;
        m_pPacketAllocator = pPacketAllocator;
        //m_ThrdProc = CNet::_NetProc;
        //m_bThreadRuning = oNetThread.Start(m_ThrdProc, this);
		m_bThreadRuning = TRUE;
		DWORD ThreadID;
		m_ThreadHandle = CreateThread(NULL,0,CNet::_NetProc, this, 0, &ThreadID);

        LOG_PROCESS_ERROR(m_bThreadRuning);

        FD_ZERO(&m_ConnetedSet);
        return TRUE;
Exit0:
        UnInit();

        return FALSE;
    }

    BOOL CNet::UnInit(void)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode = 0;
        if(m_bThreadRuning)
        {
            m_bThreadRuning = FALSE;
            //oNetThread.Wait();
			WaitForSingleObject(m_ThreadHandle, INFINITE);
			CloseHandle(m_ThreadHandle);
			m_ThreadHandle = NULL;

            nRetCode = MainLoop();
            LOG_CHECK_ERROR(nRetCode);
        }
        for(INT32 i = 0; i < MAX_SOCKET_FD; ++i)
        {
            LOG_CHECK_ERROR(g_SessionList[i] == NULL);
            if(g_SessionList[i])
                DestroySession(g_SessionList[i]);
            
        }

        m_oSessionPool.UnInit();
        m_oAcceptorPool.UnInit();

        m_MsgListToWork.UnInit();
        m_MsgListToMain.UnInit();
        m_pPacketAllocator = NULL;
        m_pNetHandler = NULL;

        WSACleanup();

        return TRUE;
    }

    ISocketSession* CNet::CreateSession(void)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode = 0;
        CSocketSession* poNewSession = NULL;

        poNewSession = m_oSessionPool.New();
        LOG_PROCESS_ERROR(poNewSession);

        nRetCode = poNewSession->Init(this);
        LOG_PROCESS_ERROR(nRetCode);

        return poNewSession;
Exit0:  
        if(poNewSession)
            DestroySession(poNewSession);
        
        return NULL;
    }

    void CNet::DestroySession(ISocketSession* pSession)
    {
		M_DUMP_NETWORK_STACK;

        INT32 nRetCode;
        INT32 nSocketFD;

        PROCESS_SUCCESS(pSession == NULL);

        CSocketSession* poSockSession = (CSocketSession*)pSession;
        LOG_CHECK_ERROR(poSockSession);

        nSocketFD = poSockSession->GetSocketID();
        if(nSocketFD != INVALID_SOCKET)
            g_SessionList[nSocketFD] = NULL;
        

        nRetCode = poSockSession->UnInit();
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = m_oSessionPool.Delete(poSockSession);
        LOG_CHECK_ERROR(nRetCode);

        return;
Exit1:
        return;
    }

    BOOL CNet::PushMsgToMain(SSessionMsg* poMsg)
    {
        return m_MsgListToMain.Push((VOID*)poMsg);
    }

    BOOL CNet::PushMsgToWork(SSessionMsg* poMsg)
    {
        return m_MsgListToWork.Push((VOID*)poMsg);
    }
    
    SSessionMsg* CNet::PopWorkMsg()
    {
        SSessionMsg* poMsg = NULL;
        PROCESS_ERROR(m_MsgListToWork.Check());

        poMsg = (SSessionMsg*)m_MsgListToWork.Pop();

        return poMsg;
Exit0:
        return NULL;
    }

    BOOL CNet::IsUnProcessMsg()
    {
        if(m_MsgListToWork.Check())
            return TRUE;

        return FALSE;
    }
    BOOL CNet::UpdateWorkingSocket(fd_set& stListenSet, fd_set& stConnectingSet, fd_set& stConnectedSet, fd_set& stClosingSet)
    {		
        SOCKET nSocketFD = INVALID_SOCKET;
        SSessionMsg* poMsg = PopWorkMsg();
        INT32 nRetCode = 0;

        while(poMsg)
        {
            CSocketSession* poSession;

            nSocketFD = poMsg->nSocketFD;
            LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);

            poSession = g_SessionList[poMsg->nSocketFD];
            if(poMsg->eOperate == esoDisConnect && FD_ISSET(nSocketFD, &stListenSet))
            {
                FD_CLR(nSocketFD, &stListenSet);
            }
            else if(poSession != NULL)
            {
                switch(poMsg->eOperate)
                {
                case esoListen:
                    {
                        nRetCode = FD_ISSET(nSocketFD, &stListenSet); 
                        LOG_PROCESS_ERROR(nRetCode == 0);
                        FD_SET(nSocketFD, &stListenSet);
                        poSession->SetState(esccListening);
                    }
                    break;
                case esoConnect:
                    {
                        nRetCode = FD_ISSET(nSocketFD, &stConnectingSet); 
                        LOG_PROCESS_ERROR(nRetCode == 0);
                        FD_SET(nSocketFD, &stConnectingSet);
                        poSession->SetState(esccConnecting);
                    }
                    break;
                case esoConnected:
                    {
                        nRetCode = FD_ISSET(nSocketFD, &stConnectedSet);
                        if(nRetCode != 0)
                            poSession->SetState(esccConnected);
                        
                    }
                    break;
                case esoDisConnect:
                    {
                        FD_CLR(nSocketFD, &stConnectingSet);
                        FD_CLR(nSocketFD, &stConnectedSet);
                        FD_SET(nSocketFD, &stClosingSet);

                        switch (poSession->GetState())
                        {
                        case esccUnConnect:
                            PROCESS_ERROR(FALSE);
                            break;
                        case esccConnecting:
                            poSession->SetState(esccLocalClosed);
                            break;
                        case esccConnected:
                            {
                                if(poSession->IsSendMsg())
                                {
                                    poSession->SetState(esccLocalClosing);
                                }
                                else
                                {
                                    nRetCode = shutdown(nSocketFD, SD_SEND);                                
                                    poSession->SetState(esccLocalClosingWaitRecv);
                                }
                            }
                            break;
                        case esccLocalClosing:
                        case esccLocalClosingWaitRecv:
                        case esccLocalClosed:
                        case esccRemoteClosingWaitSend:
                        case esccRemoteClosed:
                            break;
                        default:
                            LOG_PROCESS_ERROR(FALSE);
                        }
                    }
                    break;
                }
            }
            SAFE_DELETE(poMsg);
            poMsg = PopWorkMsg();
        }

        return TRUE;
Exit0:
        return FALSE;
    }

    BOOL CNet::UpdateExceptSocket(fd_set& stExeptSet, fd_set& stConnectingSet)
    {
        for(UINT32 i = 0; i < stExeptSet.fd_count; ++i)
        {
            INT32 nSockErrorFD = stExeptSet.fd_array[i];
            CSocketSession* poConnectErrorSession = g_SessionList[nSockErrorFD];
            SSessionMsg* poMsg;
            LOG_PROCESS_ERROR(poConnectErrorSession);

            poConnectErrorSession->SetState(esccRemoteClosed);
            FD_CLR(nSockErrorFD, &stConnectingSet);

            poMsg = new SSessionMsg;
            LOG_PROCESS_ERROR(poMsg);
            poMsg->eOperate = esoOnDisConnect;
            poMsg->nSocketFD = nSockErrorFD;
            PushMsgToMain(poMsg);
        }

        return TRUE;
Exit0:
        return FALSE;
    }

    BOOL CNet::UpdateWriteSocket(fd_set& stWriteSet, fd_set& stConnectingSet, fd_set& stConnectedSet, fd_set& stClosingSet)
    {
        INT32 nRetCode = 0;
        CSocketSession* poWriteSession = NULL;
        SSessionMsg* poMsg = NULL;
        INT32 nSockWriteFD = 0;
        for(UINT32 i = 0; i < stWriteSet.fd_count; ++i)
        {
            nSockWriteFD = stWriteSet.fd_array[i];
            poWriteSession = g_SessionList[nSockWriteFD];
           
            BOOL bFind = FALSE;
            BOOL bClosingFind = FALSE;
            LOG_PROCESS_ERROR(poWriteSession);

            bFind = FD_ISSET(nSockWriteFD, &stConnectingSet);
            if(bFind)
            {
                LOG_PROCESS_ERROR(poWriteSession->GetState() == esccConnecting);
                poWriteSession->SetState(esccConnected);

                FD_CLR(nSockWriteFD, &stConnectingSet);
                FD_SET(nSockWriteFD, &stConnectedSet);

                poMsg = new SSessionMsg;
                LOG_PROCESS_ERROR(poMsg);
                poMsg->eOperate = esoOnConnect;
                poMsg->nSocketFD = nSockWriteFD;
                PushMsgToMain(poMsg);
                continue;
            }


            bFind = FD_ISSET(nSockWriteFD, &stConnectedSet);
            if(!bFind)
                bClosingFind = FD_ISSET(nSockWriteFD, &stClosingSet);
            if(bFind || bClosingFind)
            {
                CCircleList& oSendMsg = poWriteSession->GetSendMsgList();
                PACKET_BUFFER* poBuff = (PACKET_BUFFER*)oSendMsg.Check();
                UINT32 blockCount = 0;
                UINT32 sendSuccessCount = 0;
                CHAR* poSendBuf;
                INT32 nSendLen;

                while(poBuff)
                {
                    SIMPLE_NET_HEADER* pHeader = (SIMPLE_NET_HEADER*)((CHAR*)poBuff - sizeof(SIMPLE_NET_HEADER));
                    INT32 nNeedSendLen = pHeader->stRecord.stSendRecord.wNeedSend;
      //              if(nNeedSendLen <= 0)
      //              {						
						//FString tmpDump = FString::Printf( TEXT("%sSimpleNet-SendZero-%d-%d-%d-%s.dmp"),
						//	*appGameLogDir(), MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER, *appSystemTimeString() );
						//CreateCustomDump(*tmpDump);

						//appMsgf(AMT_OK, TEXT("Error send message size: nNeedSendLen <= 0, check dump file %s"), *tmpDump);

      //                  poBuff = (PACKET_BUFFER*)oSendMsg.Pop();
      //                  FreeBufer(poBuff);
      //                  poBuff = (PACKET_BUFFER*)oSendMsg.Check();
						//continue;
      //              }
                    LOG_PROCESS_ERROR(nNeedSendLen > 0);
                    
                    poSendBuf = ((CHAR*)poBuff) + pHeader->nOffset + pHeader->stRecord.stSendRecord.wAlreadySend;
                    nSendLen = send(nSockWriteFD, poSendBuf, nNeedSendLen, 0);
                    LOG_PROCESS_ERROR(nSendLen != 0);
                    if(nSendLen > 0)
                    {
                        pHeader->stRecord.stSendRecord.wNeedSend -= nSendLen;
                        pHeader->stRecord.stSendRecord.wAlreadySend += nSendLen;
                    }
                    else if(nSendLen == SOCKET_ERROR)
                    {
                        INT32 nErrCode = WSAGetLastError();
                        if(nErrCode == WSAEWOULDBLOCK)
                        {
                            break;
                        }
                        else
                        {
                            if(FD_ISSET(nSockWriteFD, &stConnectedSet))
                            {
                                FD_CLR(nSockWriteFD, &stConnectedSet);
                                FD_SET(nSockWriteFD, &stClosingSet);
                                poWriteSession->SetState(esccRemoteClosed);
                            }
                            else if(FD_ISSET(nSockWriteFD, &stClosingSet))
                            {
                                poWriteSession->SetState(esccRemoteClosed);
                            }

							warnf(NAME_Error, TEXT("some socket unprocess send error:%d, nError"), nErrCode); 

							poWriteSession->ClearSendMsg();
                            break;
                        }
                    }

                    if(pHeader->stRecord.stSendRecord.wNeedSend == 0)
                    {
                        poBuff = (PACKET_BUFFER*)oSendMsg.Pop();
                        //warnf( TEXT("[Send Success oSendMsg::Pop] %x"), (void*)poBuff);

                        FreeBufer(poBuff);
                        poBuff = (PACKET_BUFFER*)oSendMsg.Check();
                        
                        //static INT64 qwRealSend;
                        //qwRealSend++;
                        //if(qwRealSend % (10) == 0)
                        //    INF("qwRealSend:%d", qwRealSend/(10));
                    }
                }
                if(poWriteSession->GetState() == esccLocalClosing && !poWriteSession->IsSendMsg())
                {
                    nRetCode = shutdown(nSockWriteFD, SD_SEND);
                    poWriteSession->SetState(esccLocalClosingWaitRecv);
                }
                else if(poWriteSession->GetState() == esccRemoteClosingWaitSend && !poWriteSession->IsSendMsg())
                {
                    nRetCode = shutdown(nSockWriteFD, SD_SEND);
                    poWriteSession->SetState(esccRemoteClosed);
                }
            }
        }
        
        return TRUE;
Exit0:
        FD_CLR(nSockWriteFD, &stConnectingSet);
        FD_CLR(nSockWriteFD, &stConnectedSet);
        FD_CLR(nSockWriteFD, &stClosingSet);
        if(poWriteSession)
            poWriteSession->SetState(esccLocalClosed);

        poMsg = new SSessionMsg;
        LOG_PROCESS_ERROR(poMsg);
        poMsg->eOperate = esoOnDisConnect;
        poMsg->nSocketFD = nSockWriteFD;
        PushMsgToMain(poMsg);
        return FALSE;
    }

    BOOL CNet::UpdateReadSocket(fd_set& stReadSet, fd_set& stListenSet, fd_set& stConnectedSet, fd_set& stClosingSet)
    {
        INT32 nSockReadFD;
        SSessionMsg* poMsg;
        CSocketSession* poReadSession = NULL;
        SOCKET nConnectedSocketFD = INVALID_SOCKET;

        timeval stInterval;
        stInterval.tv_sec = 0;
        stInterval.tv_usec = 0;

        LINGER stLinger;
        stLinger.l_onoff = 1;
        stLinger.l_linger = 4;

        for(UINT32 i = 0; i < stReadSet.fd_count; ++i)
        {
            nSockReadFD = stReadSet.fd_array[i];
            poReadSession = g_SessionList[nSockReadFD];
            BOOL bFind = FALSE;
            if(!poReadSession)
                continue;

            bFind = FD_ISSET(nSockReadFD, &stListenSet);
            if(bFind)
            {
                INT32 nRetCode = 1;
                fd_set stSignleSet;
                LOG_PROCESS_ERROR(poReadSession->GetState() == esccListening);

                while(nRetCode)
                {
                    CSocketSession* poConnectedSession = NULL;
                    nConnectedSocketFD = INVALID_SOCKET;
                    UINT32 dwNonBlock = 1;

                    nConnectedSocketFD = accept(nSockReadFD, NULL, NULL);
                    LOG_PROCESS_ERROR(nConnectedSocketFD != INVALID_SOCKET);
                    LOG_PROCESS_ERROR(g_SessionList[nConnectedSocketFD] == NULL);

                    poConnectedSession = (CSocketSession*)(CreateSession());
                    LOG_PROCESS_ERROR(poConnectedSession != NULL);

                    nRetCode = setsockopt(nConnectedSocketFD, SOL_SOCKET, SO_RCVBUF, (char*)&(m_oInitParam.nRecvBufSize), sizeof(m_oInitParam.nRecvBufSize));
                    LOG_CHECK_ERROR(!nRetCode);

                    nRetCode = setsockopt(nConnectedSocketFD, SOL_SOCKET, SO_SNDBUF, (char*)&(m_oInitParam.nSendBufSize), sizeof(m_oInitParam.nSendBufSize));
                    LOG_CHECK_ERROR(!nRetCode);

                    nRetCode = setsockopt(nConnectedSocketFD, SOL_SOCKET, SO_LINGER, (char*)&(stLinger), sizeof(stLinger));
                    LOG_CHECK_ERROR(!nRetCode);

                    poConnectedSession->SetSocketID(nConnectedSocketFD);
                    g_SessionList[nConnectedSocketFD] = poConnectedSession;

                    poConnectedSession->SetAcceptor(poReadSession->GetAcceptor());
                    poConnectedSession->SetState(esccConnected);

                    FD_SET(nConnectedSocketFD, &stConnectedSet);

                    poMsg = new SSessionMsg;
                    LOG_PROCESS_ERROR(poMsg);
                    poMsg->eOperate = esoOnAccept;
                    poMsg->nSocketFD = nConnectedSocketFD;
                    PushMsgToMain(poMsg);

                    FD_ZERO(&stSignleSet);
                    FD_SET(nSockReadFD, &stSignleSet);
                    nRetCode = select(MAX_SOCKET_FD, &stSignleSet, NULL, NULL, &stInterval);
                }
                continue;
            }


            bFind = FD_ISSET(nSockReadFD, &stConnectedSet) || FD_ISSET(nSockReadFD, &stClosingSet);
            if(bFind)
            {
                CCircleList& oRecvMsg = poReadSession->GetRecvMsgList();                        
                INT32 nRecvLen;
                INT32 nRetCode = 1;
                
                while(1)
                {
                    static INT32 s_nLastMsgID = 0;

                    INT32 nNeedRecvSize = poReadSession->GetNeedRecvSize();
                    if (nNeedRecvSize <= 0)
                    {
						#if !SHIPPING_PC_GAME
                        wchar_t szText[256];
                        UINT16 invalidMessageID = 0;
                        const char* invalidMessageName = "<unknown>";
                        const char* lastMessageName = "<unknown>";
                        if (poReadSession->GetCurRecvMsgSize() >= sizeof(MESSAGE_HEADER))
                        {
                            memcpy(&invalidMessageID, poReadSession->GetRecvBuff() - poReadSession->GetCurRecvMsgSize(), sizeof(invalidMessageID));
                            if (invalidMessageID < ExternalMessage::max_message_count && g_szS2CMessageName[invalidMessageID])
                                invalidMessageName = g_szS2CMessageName[invalidMessageID];
                        }
                        if (s_nLastMsgID >= 0 && s_nLastMsgID < ExternalMessage::max_message_count && g_szS2CMessageName[s_nLastMsgID])
                            lastMessageName = g_szS2CMessageName[s_nLastMsgID];
						
                        appStrncpy(szText, *FString::Printf(LocalizeSecure(Localize(TEXT("Error"), TEXT("UnregisterProtocol"), TEXT("ProgError")),
                            s_nLastMsgID, UTF8_TO_TCHAR(lastMessageName),
                            (int)invalidMessageID,
							UTF8_TO_TCHAR(invalidMessageName),
                            g_nMessageVersion)), ARRAY_COUNT(szText));
						warnf(NAME_Error, TEXT("%s"), szText);
                        appMsgf(AMT_OK, TEXT("%s"), szText);
						#endif
#ifdef _DEBUG
                        appDebugBreak();
#endif  // _DEBUG
                    }
                    LOG_PROCESS_ERROR(nNeedRecvSize > 0);

                    nRecvLen = recv(nSockReadFD, poReadSession->GetRecvBuff(), nNeedRecvSize, 0);
                    if(nRecvLen > 0)
                    {
                        poReadSession->SetCurRecvMsgSize(poReadSession->GetCurRecvMsgSize() + nRecvLen);
                        if(poReadSession->GetNeedRecvSize() == 0)
                        {
                            SIMPLE_NET_HEADER* pHeader;
                            PACKET_BUFFER* poPacketBuff = NULL;
                            INT32 nSize = poReadSession->GetCurRecvMsgSize();                            
                            LOG_PROCESS_ERROR(nSize > 0);

                            poPacketBuff = AllocBuffer(nSize);
                            LOG_PROCESS_ERROR(poPacketBuff);
                            //warnf( TEXT("[_recv::alloc] %x"), (void*)poPacketBuff);

                            pHeader = (SIMPLE_NET_HEADER*)((CHAR*)poPacketBuff - sizeof(SIMPLE_NET_HEADER));
                            memcpy(poPacketBuff, poReadSession->GetRecvBuff() - nSize, nSize);
                            pHeader->stRecord.stRecvRecord.wPackageSize = nSize;

                            //static INT64 qwRecv;
                            //qwRecv++;
                            //if(qwRecv % (1024) == 0)
                            //    INF("qwRecv:%d", qwRecv/(1024));

                            oRecvMsg.Push(poPacketBuff);

                            //warnf( TEXT("[oRecvMsg::Push] %x"), (void*)poPacketBuff);

                            poReadSession->SetCurRecvMsgSize(0);

                            s_nLastMsgID = ((MESSAGE_HEADER*)poPacketBuff)->wMsg;
                        }
                    }                    
                    else if(nRecvLen == SOCKET_ERROR)
                    {
                        INT32 nError = WSAGetLastError();
                        if(nError == WSAEWOULDBLOCK)
                        {
                            break;
                        }
                        else
                        {
                            if(FD_ISSET(nSockReadFD, &stConnectedSet))
                            {
                                FD_CLR(nSockReadFD, &stConnectedSet);
                                FD_SET(nSockReadFD, &stClosingSet);
                                poReadSession->SetState(esccRemoteClosed);
                            }
                            else if(FD_ISSET(nSockReadFD, &stClosingSet))
                            {
                                poReadSession->SetState(esccRemoteClosed);
                            }
                            else
                            {
                                LOG_PROCESS_ERROR(FALSE);
                            }

                            warnf(NAME_Error, TEXT("some socket unprocess recv error:%d, nError"),nError); 

                            //poMsg = new SSessionMsg;
                            //LOG_PROCESS_ERROR(poMsg);
                            //poMsg->eOperate = esoOnDisConnect;
                            //poMsg->nSocketFD = nSockReadFD;
                            //PushMsgToMain(poMsg);

                            break;
                        }
                    }
                    else    //only (nRecvLen == 0)
                    {
                        if(FD_ISSET(nSockReadFD, &stConnectedSet))
                        {
                            FD_CLR(nSockReadFD, &stConnectedSet);
                            FD_SET(nSockReadFD, &stClosingSet);
                            if(poReadSession->IsSendMsg())
                            {
                                poReadSession->SetState(esccRemoteClosingWaitSend);
                            }
                            else
                            {
                                shutdown(nSockReadFD, SD_SEND);
                                poReadSession->SetState(esccRemoteClosed);
                            }
                        }
                        else if(FD_ISSET(nSockReadFD, &stClosingSet))
                        {
                            if(poReadSession->GetState() == esccLocalClosing)
                                poReadSession->SetState(esccRemoteClosingWaitSend);
                            else if(poReadSession->GetState() == esccLocalClosingWaitRecv)
                                poReadSession->SetState(esccLocalClosed);
                            else if(poReadSession->GetState() != esccRemoteClosed)
                            {
                                LOG_PROCESS_ERROR(FALSE);
                            }
                        }
                        else
                        {
                            LOG_PROCESS_ERROR(FALSE);
                        }
                        //poMsg = new SSessionMsg;
                        //LOG_PROCESS_ERROR(poMsg);
                        //poMsg->eOperate = esoOnDisConnect;
                        //poMsg->nSocketFD = nSockReadFD;
                        //PushMsgToMain(poMsg);
                        break;
                    }
                }
            }
        }
        return TRUE;
Exit0:
        if(nConnectedSocketFD != INVALID_SOCKET)
            closesocket(nConnectedSocketFD);

        FD_CLR(nSockReadFD, &stConnectedSet);
        FD_CLR(nSockReadFD, &stListenSet);
        FD_CLR(nSockReadFD, &stClosingSet);
        if(poReadSession)
            poReadSession->SetState(esccLocalClosed);

        poMsg = new SSessionMsg;
        LOG_PROCESS_ERROR(poMsg);
        poMsg->eOperate = esoOnDisConnect;
        poMsg->nSocketFD = nSockReadFD;
        PushMsgToMain(poMsg);

        return FALSE;
    }

    BOOL CNet::UpdateClosedSocket(fd_set& stClosingSet)
    {
        INT32 closeSockList[64];
        INT32 nCloseCount = 0;
        SSessionMsg* poMsg;

        for(UINT32 i = 0; i < stClosingSet.fd_count; ++i)
        {
            INT32 nSockID = stClosingSet.fd_array[i];
            ESessionState eState;

            if(!g_SessionList[nSockID])
            {
                closeSockList[nCloseCount++] = nSockID;
                continue;
            }

            eState = g_SessionList[nSockID]->GetState();
            if(eState == esccLocalClosed || eState == esccRemoteClosed)
            {
                poMsg = new SSessionMsg;
                LOG_PROCESS_ERROR(poMsg);
                poMsg->eOperate = esoOnDisConnect;
                poMsg->nSocketFD = nSockID;
                PushMsgToMain(poMsg);

                closeSockList[nCloseCount++] = nSockID;
            }
        }

        for(INT32 i = 0; i < nCloseCount; ++i)
            FD_CLR(closeSockList[i], &stClosingSet);

        return TRUE;
Exit0:
        return FALSE;
    }

    DWORD STDCALL CNet::_NetProc(VOID* pNet)
    {
        CNet* poNet = (CNet*)pNet;
        BOOL bRun = TRUE;
        BOOL bMsg = FALSE;
        INT32 nRetCode = 0;
        timeval stInterval;

        fd_set stReadSet;
        fd_set stWriteSet;
        fd_set stExeptSet;

        fd_set stListenSet;
        fd_set stConnectingSet;
        fd_set stConnectedSet;
        fd_set stClosingSet;

        FD_ZERO(&stListenSet);
        FD_ZERO(&stConnectingSet);
        FD_ZERO(&stConnectedSet);
        FD_ZERO(&stClosingSet);

        stInterval.tv_sec = 0;
        stInterval.tv_usec = 0;

        while(bRun || bMsg)
        {
            nRetCode = poNet->UpdateWorkingSocket(stListenSet, stConnectingSet, stConnectedSet, stClosingSet);
            LOG_PROCESS_ERROR(nRetCode);
            
            FD_ZERO(&stExeptSet);
            FD_ZERO(&stWriteSet);
            FD_ZERO(&stReadSet);

            memcpy(&stExeptSet, &stConnectingSet, sizeof(fd_set));
            memcpy(&stWriteSet, &stConnectingSet, sizeof(fd_set));
            memcpy(&stReadSet, &stConnectedSet, sizeof(fd_set));

            for(UINT32 i = 0; i < stListenSet.fd_count; ++i)
                FD_SET(stListenSet.fd_array[i], &stReadSet);


            for(UINT32 i = 0; i < stConnectedSet.fd_count; ++i)
            {
                CSocketSession* pTempSession = g_SessionList[stConnectedSet.fd_array[i]];
                LOG_CHECK_ERROR(pTempSession);
                if(pTempSession && pTempSession->IsSendMsg())
                    FD_SET(stConnectedSet.fd_array[i], &stWriteSet);
            }

            for(UINT32 i = 0; i < stClosingSet.fd_count; ++i)
            {
                CSocketSession* pTempSession = g_SessionList[stClosingSet.fd_array[i]];
                if(pTempSession && pTempSession->GetState() != esccRemoteClosingWaitSend)
                    FD_SET(stClosingSet.fd_array[i], &stReadSet);
            }

            for(UINT32 i = 0; i < stClosingSet.fd_count; ++i)
            {
                CSocketSession* pTempSession = g_SessionList[stClosingSet.fd_array[i]];
                if(pTempSession && (pTempSession->GetState() == esccLocalClosing || pTempSession->GetState() == esccRemoteClosingWaitSend))
                    FD_SET(stClosingSet.fd_array[i], &stWriteSet);
            }
            nRetCode = select(MAX_SOCKET_FD, &stReadSet, &stWriteSet, &stExeptSet, &stInterval);
            if(nRetCode <= 0)
                Sleep(1);
            
            nRetCode = poNet->UpdateExceptSocket(stExeptSet, stConnectingSet);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = poNet->UpdateWriteSocket(stWriteSet, stConnectingSet, stConnectedSet, stClosingSet);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = poNet->UpdateReadSocket(stReadSet, stListenSet, stConnectedSet, stClosingSet);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = poNet->UpdateClosedSocket(stClosingSet);
            LOG_CHECK_ERROR(nRetCode);
            
            if(!poNet->IsThreadRuning())
                bRun = FALSE;
            bMsg = poNet->IsUnProcessMsg();
        }

        return 0;
Exit0:
        return 0;
    }

	void _OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, INetEventHandler* evtHdl)
	{
		M_FORCE_TRY_BEGIN;
		 evtHdl->OnRecv(pSocketSession, pPacketBuffer, dwSize);
		M_FORCE_TRY_END("Protected for _OnRecv");		
	}


    BOOL CNet::MainLoop(void)
    {
        SSessionMsg* poMsg = NULL;
        CSocketSession* poSession = NULL;
        INT32 nRetCode = 0;
        
        LOG_PROCESS_ERROR(m_pNetHandler);

        poMsg = (SSessionMsg*)m_MsgListToMain.Pop();
        while (poMsg)
        {
            poSession = g_SessionList[poMsg->nSocketFD];
            LOG_CHECK_ERROR(poSession);
            
            if(!poSession)
            {  
                SAFE_DELETE(poMsg);
                continue;
            }

            switch(poMsg->eOperate)
            {
            case esoOnAccept:
                {
                    CSocketAcceptor* poAcceptor = poSession->GetAcceptor();
                    LOG_PROCESS_ERROR(poAcceptor);

                    m_pNetHandler->OnAccepted(poAcceptor, poSession);
                    poMsg->eOperate = esoConnected;

                    FD_SET(poMsg->nSocketFD, &m_ConnetedSet);
                    m_MsgListToWork.Push(poMsg);

                }
                break;
            case esoOnConnect:
                {
                    m_pNetHandler->OnConnected(poSession);
                    poMsg->eOperate = esoConnected;

                    FD_SET(poMsg->nSocketFD, &m_ConnetedSet);
                    m_MsgListToWork.Push(poMsg);
                }
                break;
            case esoOnDisConnect:
                {
                    PACKET_BUFFER* pPacketBuff = NULL;
                    INT32 nSize = 0;
                    LOG_PROCESS_ERROR(poSession->GetState() == esccLocalClosed || poSession->GetState() == esccRemoteClosed);
                    while(poSession->IsRecvMsg())
                    {   
                        nRetCode = poSession->GetRecvMsg(pPacketBuff, nSize);
                        LOG_CHECK_ERROR(nRetCode);

                        if(nRetCode)
                        {
							_OnRecv(poSession, pPacketBuff, nSize, m_pNetHandler);
                            //m_pNetHandler->OnRecv(poSession, pPacketBuff, nSize);

                            nSize = 0;  
                        }
                    }
                    m_pNetHandler->OnDisconnected(poSession, poSession->GetState() == esccRemoteClosed);
                    FD_CLR(poMsg->nSocketFD, &m_ConnetedSet);

                    DestroySession(poSession);
                    SAFE_DELETE(poMsg);
                }
                break;
            }
            poMsg = (SSessionMsg*)m_MsgListToMain.Pop();
        }

        for(UINT32 i = 0; i < m_ConnetedSet.fd_count; ++i)
        {
            INT32 nConnetedSocket = m_ConnetedSet.fd_array[i];
            CSocketSession* poSession = g_SessionList[nConnetedSocket];
            if(poSession)
            {
                PACKET_BUFFER* pPacketBuff = NULL;
                INT32 nSize = 0;
                while(poSession->IsRecvMsg())
                {   
                    nRetCode = poSession->GetRecvMsg(pPacketBuff, nSize);
                    LOG_CHECK_ERROR(nRetCode);
                    
                    if(nRetCode)
                    {
                        //static INT64 qwRealRecv;
                        //qwRealRecv++;
                        //if(qwRealRecv % (1024) == 0)
                        //    INF("qwRealRecv:%d", qwRealRecv/(1024));
						_OnRecv(poSession, pPacketBuff, nSize, m_pNetHandler);
						//m_pNetHandler->OnRecv(poSession, pPacketBuff, nSize);

                        nSize = 0;  
                    }
                }
            }
        }
                
        return TRUE;
Exit0:
        return FALSE;
    }
};
