#include "stdafx.h"
#include "simple_net.h"
#include "internal_message_header.h"
#include "packet_allocator.h"

#define MAX_OPERATION_PER_LOOP          (1024)
#define MAX_MAIN_THREAD_PROCESS_TIME    (500)

namespace SimpleNet
{
#if defined(__linux__)

	bool _wfd_isset(SOCKET fd, wfd_set *set)
	{
		int i = 0;
		for (i = 0; i < set->fd_count; i++)
		{
			if (set->fd_array[i] == fd)
			{
				return true;
			}
		}
		return false;
	}

	void _wfd_clr(SOCKET fd, wfd_set *set)
	{
		int __i = 0;
		for (__i = 0; __i < (set)->fd_count; __i++)
		{
			if ((set)->fd_array[__i] == fd)
			{
				while (__i < (set)->fd_count - 1)
				{
					(set)->fd_array[__i] = (set)->fd_array[__i + 1];
					__i++;
				}
				(set)->fd_count--;
				break;
			}
		}
	}

#include <assert.h>    
	void _wfd_set(SOCKET fd, wfd_set *set)
	{
		int __i = 0;
		for (__i = 0; __i < (set)->fd_count; __i++)
		{
			if ((set)->fd_array[__i] == (fd))
			{
				break;
			}
		}
		if (__i == (set)->fd_count)
		{
			assert((set)->fd_count < FD_SETSIZE);
			(set)->fd_array[__i] = (fd);
			(set)->fd_count++;
		}
	}

	BOOL linux_select(wfd_set* _stReadSet, wfd_set* _stWriteSet, wfd_set* _stExceptSet, struct timeval * stTimeout)
	{
		INT32 nRetCode = 0;
		INT32 nMaxFD = 0;

		fd_set stReadSet;
		fd_set stWriteSet;
		fd_set stExceptSet;

		FD_ZERO(&stReadSet);
		FD_ZERO(&stWriteSet);
		FD_ZERO(&stExceptSet);

		if (_stReadSet)
			for (INT32 i = 0; i < _stReadSet->fd_count; ++i)
			{
			FD_SET(_stReadSet->fd_array[i], &stReadSet);
			if (_stReadSet->fd_array[i] > nMaxFD)
				nMaxFD = _stReadSet->fd_array[i];
			}

		if (_stWriteSet)
			for (INT32 i = 0; i < _stWriteSet->fd_count; ++i)
			{
			FD_SET(_stWriteSet->fd_array[i], &stWriteSet);
			if (_stWriteSet->fd_array[i] > nMaxFD)
				nMaxFD = _stWriteSet->fd_array[i];
			}

		if (_stExceptSet)
			for (INT32 i = 0; i < _stExceptSet->fd_count; ++i)
			{
			FD_SET(_stExceptSet->fd_array[i], &stExceptSet);
			if (_stExceptSet->fd_array[i] > nMaxFD)
				nMaxFD = _stExceptSet->fd_array[i];
			}

		nRetCode = select(nMaxFD + 1, &stReadSet, &stWriteSet, &stExceptSet, stTimeout);
		LOG_PROCESS_ERROR(nRetCode >= 0);

		if (_stReadSet)
			for (INT32 i = 0; i < _stReadSet->fd_count; ++i)
			{
			if (!FD_ISSET(_stReadSet->fd_array[i], &stReadSet))
			{
				int old_fd = _stReadSet->fd_count;
				WFD_CLR(_stReadSet->fd_array[i], _stReadSet);
				LOG_PROCESS_ERROR(old_fd == _stReadSet->fd_count + 1);
			}
			}

		if (_stWriteSet)
			for (INT32 i = 0; i < _stWriteSet->fd_count; ++i)
			{
			if (!FD_ISSET(_stWriteSet->fd_array[i], &stWriteSet))
			{
				int old_fd = _stWriteSet->fd_count;
				WFD_CLR(_stWriteSet->fd_array[i], _stWriteSet);
				LOG_PROCESS_ERROR(old_fd == _stWriteSet->fd_count + 1);
			}
			}

		if (_stExceptSet)
			for (INT32 i = 0; i < _stExceptSet->fd_count; ++i)
			{
			if (!FD_ISSET(_stExceptSet->fd_array[i], &stExceptSet))
			{
				int old_fd = _stExceptSet->fd_count;
				WFD_CLR(_stExceptSet->fd_array[i], _stExceptSet);
				LOG_PROCESS_ERROR(old_fd == _stExceptSet->fd_count + 1);
			}
			}
		return TRUE;

	Exit0:
		return FALSE;
	}
#endif
	INT32 CSocketSession::Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset)
	{
		INT32 nRetCode = 0;
		UINT16* pwPackLen = NULL;
		SIMPLE_NET_HEADER* pHeader = NULL;

		LOG_PROCESS_ERROR(pPacketBuffer);
		LOG_PROCESS_ERROR(nSize > 0);
		_mm_prefetch((char*)pPacketBuffer - sizeof(SIMPLE_NET_HEADER), _MM_HINT_T0);

		PROCESS_ERROR(!m_bClose);
		PROCESS_ERROR(m_eState == esccConnected);

		pwPackLen = (UINT16*)((char*)pPacketBuffer + nOffset - 2);
		*pwPackLen = nSize;

		pHeader = (SIMPLE_NET_HEADER*)((char*)pPacketBuffer - sizeof(SIMPLE_NET_HEADER));
		pHeader->nOffset = nOffset - 2;
		pHeader->stRecord.stSendRecord.wAlreadySend = 0;
		pHeader->stRecord.stSendRecord.wNeedSend = nSize + 2;

		nRetCode = m_oSendMsg.Push((VOID*)pPacketBuffer);
		LOG_PROCESS_ERROR(nRetCode);

		//static INT64 qwSend;
		//qwSend++;
		//if(qwSend % (10) == 0)
		//    INF("qwSend:%d", qwSend/(10));  
		return nSize;
	Exit0:
		//INF("fd %d Send error, nSize=%d, m_bClose=%d, Session State=%d, nRetCode=%d", m_nSockedFD, nSize, m_bClose, m_eState, nRetCode);
		if (!m_bClose)
			Close();

		return 0;
	}

	INT32 CSocketSession::SendNoPackageHead(PACKET_BUFFER* pPacketBuffer, INT32 nSize)
	{
		INT32 nRetCode = 0;
		INT16* nPackLen = NULL;
		SIMPLE_NET_HEADER* pHeader = NULL;
		PROCESS_ERROR(nSize > 0);
		PROCESS_ERROR(!m_bClose);

		PROCESS_ERROR(m_eState == esccConnected);

		pHeader = (SIMPLE_NET_HEADER*)((CHAR*)pPacketBuffer - sizeof(SIMPLE_NET_HEADER));
		pHeader->nOffset = 0;
		pHeader->stRecord.stSendRecord.wAlreadySend = 0;
		pHeader->stRecord.stSendRecord.wNeedSend = nSize;

		nRetCode = m_oSendMsg.Push((VOID*)pPacketBuffer);
		LOG_PROCESS_ERROR(nRetCode);

		return nSize;
	Exit0:
		if (!m_bClose)
			Close();

		return 0;
	}

	BOOL CSocketSession::IsSendMsg()
	{
		return (m_oSendMsg.Check() != NULL);
	}

	BOOL CSocketSession::ClearSendMsg()
	{
		while (m_oSendMsg.Check())
		{
			((CNet*)m_poNet)->FreeBuffer((PACKET_BUFFER*)m_oSendMsg.Pop());
		}

		return TRUE;
	}

	BOOL CSocketSession::IsRecvMsg()
	{
		return (m_oRecvMsg.Check() != NULL);
	}


	BOOL CSocketSession::GetRecvMsg(PACKET_BUFFER*& pPackBuffer, INT32& nSize)
	{
		SIMPLE_NET_HEADER* pHeader = NULL;
		pPackBuffer = NULL;
		//INT16* poPackLen = NULL;
		nSize = 0;

		pPackBuffer = (PACKET_BUFFER*)m_oRecvMsg.Pop();
		PROCESS_ERROR(pPackBuffer != NULL);

		pHeader = (SIMPLE_NET_HEADER*)((CHAR*)pPackBuffer - sizeof(SIMPLE_NET_HEADER));
		LOG_PROCESS_ERROR(pHeader != NULL);

		nSize = pHeader->stRecord.stRecvRecord.wPackageSize;
		return TRUE;
	Exit0:
		if (pPackBuffer && m_poNet)
			((CNet*)m_poNet)->FreeBuffer(pPackBuffer);

		if (!m_bClose)
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
		socklen_t nSockSize = sizeof(stSockAddr);

		LOG_PROCESS_ERROR(m_poNet != NULL);
		LOG_PROCESS_ERROR(m_nSockedFD != INVALID_SOCKET);
		LOG_PROCESS_ERROR(m_eState == esccConnected || m_eState == esccListening);

		nRetCode = getsockname(m_nSockedFD, ((SOCKADDR*)&stSockAddr), &nSockSize);
		LOG_PROCESS_ERROR(nRetCode == 0);

		dwAddr = (UINT32)stSockAddr.sin_addr.s_addr;
		wPort = ntohs(stSockAddr.sin_port);

		return TRUE;
	Exit0:
		return FALSE;
	}

	BOOL CSocketSession::GetRemoteAddress(UINT32& dwRemoteAddr, UINT16& wRemotePort)
	{
		INT32 nRetCode;
		SOCKADDR_IN stSockAddr;
		socklen_t nSockSize = sizeof(stSockAddr);

		LOG_PROCESS_ERROR(m_poNet != NULL);
		LOG_PROCESS_ERROR(m_nSockedFD != INVALID_SOCKET);
		LOG_PROCESS_ERROR(m_eState == esccConnected || m_eState == esccRemoteClosingWaitSend || m_eState == esccLocalClosingWaitRecv);

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
		m_nSockedFD = INVALID_SOCKET;
		m_Node.pPrev = m_Node.pNext = NULL;
		m_eState = esccUnConnect;
		m_poNet = NULL;
		m_poRecvBuff = NULL;
		m_poAcceptor = NULL;
		m_poUserData = NULL;
		m_bClose = FALSE;
		m_wRecvMsgSize = 0;

		return;
	}

	CSocketSession::~CSocketSession()
	{
		UnInit();
	}

	UINT16 CSocketSession::GetNeedRecvSize()
	{
		UINT16 dwNeedMsgSize = 2;
		if (m_wRecvMsgSize < 2)
			dwNeedMsgSize = 2 - m_wRecvMsgSize;
		else
			dwNeedMsgSize = (*((UINT16*)m_szMsgSize)) - m_wRecvMsgSize + 2;

		return dwNeedMsgSize;
	Exit0:
		return 0;
	}

	CHAR* CSocketSession::GetRecvBuff()
	{
		CHAR* poBuff = NULL;
		if (m_wRecvMsgSize < 2)
		{
			poBuff = &m_szMsgSize[m_wRecvMsgSize];
		}
		else
		{
			LOG_PROCESS_ERROR(m_poRecvBuff);
			poBuff = ((CHAR*)m_poRecvBuff + m_wRecvMsgSize - 2);
		}
		return poBuff;
	Exit0:
		return NULL;
	}

	BOOL CSocketSession::Init(INet* poNet)
	{
		m_eState = esccUnConnect;
		m_nSockedFD = INVALID_SOCKET;
		m_Node.pPrev = m_Node.pNext = NULL;
		m_poNet = poNet;
		m_poRecvBuff = NULL;
		m_poUserData = NULL;
		m_wRecvMsgSize = 0;

		UINT32 dwPacketBufferSize = ((CNet*)m_poNet)->GetPacketBufferSize();

		m_oSendMsg.Init(dwPacketBufferSize);
		m_oRecvMsg.Init(dwPacketBufferSize);

		m_bClose = FALSE;

		return TRUE;
	}

	BOOL CSocketSession::UnInit(void)
	{
		INT32 nRetCode = 0;
		m_Node.pPrev = m_Node.pNext = NULL;

		if (m_poRecvBuff != NULL)
		{
			((CNet*)m_poNet)->FreeBuffer(m_poRecvBuff);
			m_poRecvBuff = NULL;
		}

		if (m_nSockedFD != INVALID_SOCKET)
		{
			linger stLinger;
			stLinger.l_onoff = 1;
			stLinger.l_linger = 5;

			nRetCode = setsockopt(m_nSockedFD, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
			LOG_CHECK_ERROR(!nRetCode);

			nRetCode = close(m_nSockedFD);
			LOG_CHECK_ERROR(nRetCode == 0);

			m_nSockedFD = INVALID_SOCKET;
			m_eState = esccLocalClosed;
		}

		while (m_oRecvMsg.Check())
		{
			((CNet*)m_poNet)->FreeBuffer((PACKET_BUFFER*)m_oRecvMsg.Pop());
		}

		while (m_oSendMsg.Check())
		{
			((CNet*)m_poNet)->FreeBuffer((PACKET_BUFFER*)m_oSendMsg.Pop());
		}


		m_poNet = NULL;
		m_oRecvMsg.UnInit();
		m_oSendMsg.UnInit();

		return TRUE;
	}

	BOOL CSocketSession::Close(void)
	{
		//INT32 nRetCode = 0;
		SSessionMsg* poMsg = NULL;

		LOG_PROCESS_ERROR(m_poNet != NULL);
		LOG_PROCESS_ERROR(m_nSockedFD != INVALID_SOCKET);

		poMsg = new SSessionMsg;
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
	BOOL CSocketAcceptor::Open(UINT32 dwAddr, UINT16 wPort)
	{
		SOCKADDR_IN stSockAddr;
		INT32 nRetCode = 0;
		INT32 nOption = 1;
		UINT32 dwNonBlock = 1;
		SOCKET nSocketFD = INVALID_SOCKET;
		CSocketSession* poNewSession = NULL;
		SSessionMsg* poMsg = NULL;

		LOG_PROCESS_ERROR(m_poSession == NULL);
		LOG_PROCESS_ERROR(dwAddr != INADDR_NONE);
		LOG_PROCESS_ERROR(m_poNet);

		poNewSession = (CSocketSession*)((CNet*)m_poNet)->CreateSession();
		LOG_PROCESS_ERROR(poNewSession);

		nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);

		while (nSocketFD > MAX_SOCKET_FD)
		{
			INF("the socket value is %d", nSocketFD);
			close(nSocketFD);
			nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);
		}

		memset(&stSockAddr, 0, sizeof(stSockAddr));
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_addr.s_addr = dwAddr;
		stSockAddr.sin_port = htons(wPort);

#if defined(WIN32)
		nRetCode = ioctlsocket(nSocketFD, FIONBIO, (u_long*)&dwNonBlock);
#elif defined(__linux__)
		//UINT32 dwNonBlock = 1;
		//nRetCode = ioctl(nSocketFD, FIONBIO, (u_long*)&dwNonBlock);
		{
			int nReuseAddr = 1;
			nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_REUSEADDR, (char*)&(nReuseAddr), sizeof(nReuseAddr));
			LOG_PROCESS_ERROR(!nRetCode);

			int nFlags = fcntl(nSocketFD, F_GETFL, 0);
			nRetCode = fcntl(nSocketFD, F_SETFL, nFlags | O_NONBLOCK);
		}
#endif

		nRetCode = bind(nSocketFD, ((SOCKADDR*)&stSockAddr), sizeof(stSockAddr));
		LOG_PROCESS_ERROR(nRetCode == 0);

		nRetCode = setsockopt(nSocketFD, IPPROTO_TCP, TCP_NODELAY, (char*)&nOption, sizeof(nOption));
		LOG_PROCESS_ERROR(nRetCode != SOCKET_ERROR);

		nRetCode = listen(nSocketFD, SOMAXCONN);
		LOG_PROCESS_ERROR(nRetCode == 0);

		LOG_PROCESS_ERROR(g_SessionList[nSocketFD] == NULL);

		////for( int i = 0; i < 128; ++i )    // no sense
		//{
		//    if( g_SessionList[i] == poNewSession )
		//        CRI( "g_SessionList[i] duplicated, i=%d", i );
		//    LOG_PROCESS_ERROR(FALSE);
		//}

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
		if (nSocketFD != INVALID_SOCKET)
			closesocket(nSocketFD);

		if (poNewSession != NULL)
		{
			((CNet*)m_poNet)->DestroySession(poNewSession);
			poNewSession = NULL;
			m_poSession = NULL;
		}

		return FALSE;
	}

	BOOL CSocketAcceptor::Close(void)
	{
		//INT32 nRetCode = 0;

		if (m_poSession && m_poSession->GetSocketID() != INVALID_SOCKET)
		{
			SSessionMsg* poMsg = new SSessionMsg;
			LOG_PROCESS_ERROR(poMsg != NULL);
			poMsg->eOperate = esoDisConnect;
			poMsg->nSocketFD = m_poSession->GetSocketID();

			((CNet*)m_poNet)->PushMsgToWork(poMsg);
		}

		if (m_poSession)
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
		m_pUserData = NULL;

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
		m_pUserData = NULL;

		return TRUE;
	}

	BOOL CSocketAcceptor::UnInit(void)
	{
		//INT32 nRetCode = 0;

		if (m_poSession != NULL)
		{
			SOCKET nSocketFD = m_poSession->GetSocketID();
			if (nSocketFD < MAX_SOCKET_FD && nSocketFD > 0)
				g_SessionList[nSocketFD] = NULL;

			((CNet*)m_poNet)->DestroySession(m_poSession);
			m_poSession = NULL;
		}

		m_poNet = NULL;

		return TRUE;
	}

	void* CSocketAcceptor::GetUserData(void)
	{
		return m_pUserData;
	}

	void CSocketAcceptor::SetUserData(void* pUserData)
	{
		m_pUserData = pUserData;
	}

	/************************************************************************/
	CNet::CNet()
	{
		memset(&m_ConnetedSet, 0, sizeof(m_ConnetedSet));
		m_ThrdProc = NULL;
		m_bThreadRuning = FALSE;
		memset(&m_oInitParam, 0, sizeof(m_oInitParam));
		m_pNetHandler = NULL;
		m_pPacketAllocator = NULL;
	}

	CNet::~CNet()
	{
		UnInit();
	}

	ISocketAcceptor* CNet::CreateAcceptor(void)
	{
		CSocketAcceptor* poAcceptor = NULL;
		INT32 nRetCode = 0;

		poAcceptor = m_oAcceptorPool.New();
		LOG_PROCESS_ERROR(poAcceptor);

		nRetCode = poAcceptor->Init(this);
		LOG_PROCESS_ERROR(nRetCode);

		return poAcceptor;
	Exit0:
		if (poAcceptor)
		{
			nRetCode = DestroyAcceptor(poAcceptor);
			LOG_CHECK_ERROR(nRetCode);
		}
		return NULL;
	}

	BOOL CNet::DestroyAcceptor(ISocketAcceptor* pAcceptor)
	{
		//INT32 nRetCode = 0;
		CSocketAcceptor* poSocketAcceptor = NULL;
		PROCESS_SUCCESS(pAcceptor == NULL);

		poSocketAcceptor = (CSocketAcceptor*)pAcceptor;
		LOG_PROCESS_ERROR(poSocketAcceptor);

		poSocketAcceptor->UnInit();
		m_oAcceptorPool.Delete(poSocketAcceptor);
		return TRUE;
	Exit1:
		return TRUE;
	Exit0:
		return FALSE;
	}

	ISocketSession* CNet::Connect(UINT32 dwAddr, UINT16 wPort)
	{
		CSocketSession* poNewSession = NULL;
		INT32 nRetCode = 0;
		UINT32 dwNonBlock = 1;
		INT32 nOption = 1;
		SOCKADDR_IN stSockAddr;
		SOCKET nSocketFD = INVALID_SOCKET;
		//INT32 nZero = 0;
		SSessionMsg* poMsg = NULL;

		LINGER stLinger;
		stLinger.l_onoff = 1;
		stLinger.l_linger = 4;

		poNewSession = (CSocketSession*)CreateSession();
		LOG_PROCESS_ERROR(poNewSession);

		nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);

		while (nSocketFD > MAX_SOCKET_FD)
		{
			INF("the socket value is %d", nSocketFD);
			close(nSocketFD);
			nSocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);
		}

		memset(&stSockAddr, 0, sizeof(stSockAddr));
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_addr.s_addr = INADDR_ANY;
		stSockAddr.sin_port = 0;

		nRetCode = bind(nSocketFD, ((SOCKADDR*)&stSockAddr), sizeof(stSockAddr));
		LOG_PROCESS_ERROR(nRetCode == 0);

#if defined(WIN32)
		// Purpose: set the socket to nonblocking mode
		// FIONBIO: operation that sets nonblocking I/O
		// dwNonBlock = 1 enables nonblocking mode
		// In nonblocking mode, recv/send return immediately without blocking; incomplete operations report an error such as WSAEWOULDBLOCK
		// Typically used with asynchronous I/O or multiplexing (such as select or epoll) to avoid blocking the thread
		nRetCode = ioctlsocket(nSocketFD, FIONBIO, (u_long*)&dwNonBlock);
#elif defined(__linux__)
		//nRetCode = ioctl(nSocketFD, FIONBIO, (u_long*)&dwNonBlock);
		{
			int nFlags = 0;
			nFlags = fcntl(nSocketFD, F_GETFL, 0);
			nRetCode = fcntl(nSocketFD, F_SETFL, nFlags | O_NONBLOCK);
		}
#endif
		LOG_PROCESS_ERROR(nRetCode == 0);

		// Set receive buffer size
		nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_RCVBUF, (char*)&(m_oInitParam.nRecvBufSize), sizeof(m_oInitParam.nRecvBufSize));
		LOG_CHECK_ERROR(!nRetCode);

		// Set send buffer size
		nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_SNDBUF, (char*)&(m_oInitParam.nSendBufSize), sizeof(m_oInitParam.nSendBufSize));
		LOG_CHECK_ERROR(!nRetCode);

		// Control close behavior (nonzero l_onoff enables SO_LINGER; l_linger is the wait in seconds)
		// If l_onoff != 0 and l_linger > 0, close blocks until data is sent or the timeout expires.
		// If l_onoff = 0, close immediately and discard unsent data.
		nRetCode = setsockopt(nSocketFD, SOL_SOCKET, SO_LINGER, (char*)&(stLinger), sizeof(stLinger));
		LOG_CHECK_ERROR(!nRetCode);

		// Disable Nagle’s algorithm (nOption = 1 disables it)
		// Nagle’s algorithm combines small packets to reduce congestion at the cost of latency. TCP_NODELAY=1 sends data immediately for latency-sensitive uses such as games and real-time communication.
		nRetCode = setsockopt(nSocketFD, IPPROTO_TCP, TCP_NODELAY, (char*)&nOption, sizeof(nOption));
		LOG_PROCESS_ERROR(nRetCode != SOCKET_ERROR);

		memset(&stSockAddr, 0, sizeof(stSockAddr));
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(wPort);
		stSockAddr.sin_addr.s_addr = dwAddr;

		nRetCode = connect(nSocketFD, ((SOCKADDR*)&stSockAddr), sizeof(stSockAddr));
		LOG_PROCESS_ERROR(nRetCode == SOCKET_ERROR);
#if defined(WIN32)
		LOG_PROCESS_ERROR(errno == EWOULDBLOCK);
#elif defined(__linux__)
		LOG_PROCESS_ERROR(errno == EWOULDBLOCK || errno == EINPROGRESS);
#endif

		LOG_PROCESS_ERROR(g_SessionList[nSocketFD] == NULL);
		g_SessionList[nSocketFD] = poNewSession;
		poNewSession->SetSocketID(nSocketFD);

		poMsg = new SSessionMsg;
		LOG_PROCESS_ERROR(poMsg);
		poMsg->eOperate = esoConnect;
		poMsg->nSocketFD = nSocketFD;
		PushMsgToWork(poMsg);

		INF("try connecting to %s:%d via simple net", inet_ntoa(stSockAddr.sin_addr), wPort);

		return poNewSession;
	Exit0:
		if (nSocketFD != INVALID_SOCKET)
			closesocket(nSocketFD);

		if (poNewSession)
			DestroySession(poNewSession);

		return NULL;
	}

	BOOL CNet::Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEvent, void* pInitParam)
	{
		INT32 nRetCode = 0;
        if (m_bThreadRuning.load() || m_MsgListToMain.IsInitialized() || m_MsgListToWork.IsInitialized())
            return FALSE;
		m_pNetHandler = NULL;

		LOG_PROCESS_ERROR(pPacketAllocator != NULL);
		LOG_PROCESS_ERROR(pNetEvent != NULL);
		LOG_PROCESS_ERROR(pInitParam != NULL);

#if defined(WIN32)
		WSADATA  wsd;
		nRetCode = WSAStartup(MAKEWORD(2, 2), &wsd);
		LOG_PROCESS_ERROR(nRetCode == 0);
#endif

		LOG_PROCESS_ERROR(pInitParam != NULL);
		memcpy((void*)&m_oInitParam, pInitParam, sizeof(SIMPLE_INIT_PARAM));
		LOG_PROCESS_ERROR(m_oInitParam.nMaxSessionCount <= FD_SETSIZE);
//		LOG_PROCESS_ERROR(m_oInitParam.nMaxAcceptor <= 32); // TODO: magic number

		m_oSessionPool.Init(m_oInitParam.nMaxSessionCount);
		m_oAcceptorPool.Init(m_oInitParam.nMaxAcceptor);

        LOG_PROCESS_ERROR(m_MsgListToMain.Init());
        LOG_PROCESS_ERROR(m_MsgListToWork.Init());

		m_pNetHandler = pNetEvent;
		m_pPacketAllocator = pPacketAllocator;
		m_ThrdProc = CNet::_NetProc;

		m_bThreadRuning = TRUE; // Set before launch: the worker reads this flag immediately.
		nRetCode = oNetThread.Start(m_ThrdProc, this);
		LOG_PROCESS_ERROR(nRetCode);

		WFD_ZERO(&m_ConnetedSet);
		return TRUE;
	Exit0:
		UnInit();

		return FALSE;
	}

	BOOL CNet::UnInit(void)
	{
		INT32 nRetCode = 0;

		const bool wasRunning = m_bThreadRuning.exchange(false);
        m_MsgListToWork.Close();
		oNetThread.Wait();
        m_MsgListToMain.Close();
		if (wasRunning)
		{

			nRetCode = MainLoop();
			LOG_CHECK_ERROR(nRetCode);
		}

		for (INT32 i = 0; i < MAX_SOCKET_FD; ++i)
		{
			LOG_CHECK_ERROR(g_SessionList[i] == NULL);
			if (g_SessionList[i])
				DestroySession(g_SessionList[i]);

		}

		m_oSessionPool.UnInit();
		m_oAcceptorPool.UnInit();

		m_MsgListToWork.UnInit();
		m_MsgListToMain.UnInit();
		m_pPacketAllocator = NULL;
		m_pNetHandler = NULL;

#if defined(WIN32)
		WSACleanup();
#endif // WIN32

		return TRUE;
	}

	ISocketSession* CNet::CreateSession(void)
	{
		INT32 nRetCode = 0;
		CSocketSession* poNewSession = NULL;

		poNewSession = m_oSessionPool.New();
		LOG_PROCESS_ERROR(poNewSession);

		nRetCode = poNewSession->Init(this);
		LOG_PROCESS_ERROR(poNewSession);

		return poNewSession;
	Exit0:
		if (poNewSession)
		{
			DestroySession(poNewSession);
		}
		return NULL;
	}

	void CNet::DestroySession(ISocketSession* poSession)
	{
		INT32 nRetCode = 0;
		CSocketSession* poSockSession = NULL;
		SOCKET nSocketFD = INVALID_SOCKET;

		PROCESS_SUCCESS(poSession == NULL);
		poSockSession = (CSocketSession*)poSession;
		LOG_CHECK_ERROR(poSockSession);

		nSocketFD = poSockSession->GetSocketID();
		if (nSocketFD < MAX_SOCKET_FD && nSocketFD > 0)
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
        const bool accepted = m_MsgListToMain.Push(std::unique_ptr<SSessionMsg>(poMsg));
        LOG_CHECK_ERROR(accepted || !m_bThreadRuning.load());
        return accepted;
	}

	BOOL CNet::PushMsgToWork(SSessionMsg* poMsg)
	{
        const bool accepted = m_MsgListToWork.Push(std::unique_ptr<SSessionMsg>(poMsg));
        LOG_CHECK_ERROR(accepted || !m_bThreadRuning.load());
        return accepted;
	}

	SSessionMsg* CNet::PopWorkMsg()
	{
        return m_MsgListToWork.Pop().release();
	}

	BOOL CNet::IsUnProcessMsg()
	{
        return !m_MsgListToWork.IsEmpty();
	}

	BOOL CNet::UpdateWorkingSocket(wfd_set& stListenSet, wfd_set& stConnectingSet, wfd_set& stConnectedSet, wfd_set& stClosingSet)
	{
		SOCKET nSocketFD = INVALID_SOCKET;
        std::unique_ptr<SSessionMsg> message(PopWorkMsg());
        auto* poMsg = message.get();
		INT32 nRetCode = 0;

		while (poMsg)
		{
			CSocketSession* poSession;

			nSocketFD = poMsg->nSocketFD;
			LOG_PROCESS_ERROR(nSocketFD != INVALID_SOCKET);

			poSession = g_SessionList[poMsg->nSocketFD];

			if (poMsg->eOperate == esoDisConnect && WFD_ISSET(nSocketFD, &stListenSet))
			{
				//INF("%s Session failed!: clr listen port in listenSet, fd=%d", __FUNCTION__, poMsg->nSocketFD );
				WFD_CLR(nSocketFD, &stListenSet);
			}
			else if (poSession != NULL)
			{
				switch (poMsg->eOperate)
				{
				case esoListen:
				{
					nRetCode = WFD_ISSET(nSocketFD, &stListenSet);
					LOG_PROCESS_ERROR(nRetCode == 0);
					WFD_SET(nSocketFD, &stListenSet);
					poSession->SetState(esccListening);
				}
					break;
				case esoConnect:
				{
					nRetCode = WFD_ISSET(nSocketFD, &stConnectingSet);
					LOG_PROCESS_ERROR(nRetCode == 0);
					WFD_SET(nSocketFD, &stConnectingSet);
					poSession->SetState(esccConnecting);
				}
					break;
				case esoConnected:
				{
					nRetCode = WFD_ISSET(nSocketFD, &stConnectedSet);
					if (nRetCode != 0)
					{
						poSession->SetState(esccConnected);
						//WFD_CLR(nSocketFD, &stConnectingSet);
					}
				}
					break;
				case esoDisConnect:
				{
					WFD_CLR(nSocketFD, &stConnectingSet);
					WFD_CLR(nSocketFD, &stConnectedSet);
					WFD_SET(nSocketFD, &stClosingSet);

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
						//if(poSession->IsSendMsg())
						//{
						//    poSession->SetState(esccClosing);
						//}
						//else
						//{
						//    nRetCode = shutdown(nSocketFD, SD_SEND);                                
						//    poSession->SetState(esccClosed);
						//}
						if (poSession->IsSendMsg())
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
				default:
					LOG_PROCESS_ERROR(FALSE);
				}
			}

            message.reset(PopWorkMsg());
            poMsg = message.get();
		}

		return TRUE;
	Exit0:
		return FALSE;
	}

	BOOL CNet::_ProcessFailedSocket(SOCKET nSockErrorFD, wfd_set& stConnectingSet)
	{
		WFD_CLR(nSockErrorFD, &stConnectingSet);

		SSessionMsg* poMsg;
		CSocketSession* poConnectErrorSession = g_SessionList[nSockErrorFD];
		LOG_PROCESS_ERROR(poConnectErrorSession && poConnectErrorSession->GetSocketID() == nSockErrorFD);
		poConnectErrorSession->SetState(esccRemoteClosed);

		poMsg = new SSessionMsg;
		LOG_PROCESS_ERROR(poMsg);
		poMsg->eOperate = esoOnDisConnect;
		poMsg->nSocketFD = nSockErrorFD;
		PushMsgToMain(poMsg);

		return TRUE;
	Exit0:
		return FALSE;
	}

	BOOL CNet::UpdateExceptSocket(wfd_set& stExceptSet, wfd_set& stWriteSet, wfd_set& stReadSet, wfd_set& stConnectingSet)
	{
		INT32 nRetCode = 0;
		for (UINT32 i = 0; i < stExceptSet.fd_count; ++i)
		{
			SOCKET nSockErrorFD = stExceptSet.fd_array[i];
			INF("fd %d in ExceptSocket! treate it as connecting failed!", nSockErrorFD);
			nRetCode = _ProcessFailedSocket(nSockErrorFD, stConnectingSet);
			LOG_PROCESS_ERROR(nRetCode);
		}
#if defined(__linux__)
		for (INT32 i = 0; i < stConnectingSet.fd_count; ++i)
		{
			SOCKET nSockFD = stConnectingSet.fd_array[i];
			BOOL bFind = (WFD_ISSET(nSockFD, &stWriteSet) || WFD_ISSET(nSockFD, &stReadSet));
			if (bFind)
			{
				int error = 0;
				int len = sizeof(error);
				nRetCode = getsockopt(nSockFD, SOL_SOCKET, SO_ERROR, (void*)&error, (socklen_t*)&len);
				LOG_PROCESS_ERROR(nRetCode == 0);

				if (error != 0)
				{
					INF("fd %d must be connect failed!\n", nSockFD);
					WFD_CLR(nSockFD, &stWriteSet);
					WFD_CLR(nSockFD, &stReadSet);
					_ProcessFailedSocket(nSockFD, stConnectingSet);
				}
			}
		}
#endif
		return TRUE;
	Exit0:
		return FALSE;
	}

	BOOL CNet::UpdateWriteSocket(wfd_set& stWriteSet, wfd_set& stConnectingSet, wfd_set& stConnectedSet, wfd_set& stClosingSet)
	{
		INT32 nRetCode = 0;
		CSocketSession* poWriteSession = NULL;
		SSessionMsg* poMsg = NULL;
		SOCKET nSockWriteFD = 0;

		for (UINT32 i = 0; i < stWriteSet.fd_count; ++i)
		{
			nSockWriteFD = stWriteSet.fd_array[i];
			poWriteSession = g_SessionList[nSockWriteFD];
			LOG_PROCESS_ERROR(poWriteSession && poWriteSession->GetSocketID() == nSockWriteFD);

			BOOL bFind = FALSE;
			BOOL bClosingFind = FALSE;
			LOG_PROCESS_ERROR(poWriteSession);

			bFind = WFD_ISSET(nSockWriteFD, &stConnectingSet);
			if (bFind)
			{
				LOG_PROCESS_ERROR(poWriteSession->GetState() == esccConnecting);

				poWriteSession->SetState(esccConnected);

				WFD_CLR(nSockWriteFD, &stConnectingSet);
				WFD_SET(nSockWriteFD, &stConnectedSet);

				poMsg = new SSessionMsg;
				LOG_PROCESS_ERROR(poMsg);
				poMsg->eOperate = esoOnConnect;
				poMsg->nSocketFD = nSockWriteFD;
				PushMsgToMain(poMsg);
				continue;

			}

			bFind = WFD_ISSET(nSockWriteFD, &stConnectedSet);
			if (!bFind)
				bClosingFind = WFD_ISSET(nSockWriteFD, &stClosingSet);

			if (bFind || bClosingFind)
			{
				CCircleList& oSendMsg = poWriteSession->GetSendMsgList();
				PACKET_BUFFER* poBuff = (PACKET_BUFFER*)oSendMsg.Check();
				//UINT32 blockCount = 0;
				//UINT32 sendSuccessCount = 0;
				CHAR* poSendBuf;
				INT32 nSendLen;

				while (poBuff)
				{
					SIMPLE_NET_HEADER* pHeader = (SIMPLE_NET_HEADER*)((CHAR*)poBuff - sizeof(SIMPLE_NET_HEADER));
					INT32 nNeedSendLen = pHeader->stRecord.stSendRecord.wNeedSend;

					LOG_PROCESS_ERROR(nNeedSendLen > 0);

					poSendBuf = ((CHAR*)poBuff) + pHeader->nOffset + pHeader->stRecord.stSendRecord.wAlreadySend;
					nSendLen = send(nSockWriteFD, poSendBuf, nNeedSendLen, 0);
					LOG_PROCESS_ERROR(nSendLen != 0);

					//if (*((UINT8*)(poWriteSession->GetUserData()) + 16) == 2 &&
					//    ((InternalMessage::MESSAGE_HEADER*)(poSendBuf + 2))->wMsg == 20483)
					//    INT3();

					if (nSendLen > 0)
					{
						pHeader->stRecord.stSendRecord.wNeedSend -= nSendLen;
						pHeader->stRecord.stSendRecord.wAlreadySend += nSendLen;
					}
					else if (nSendLen == SOCKET_ERROR)
					{
						INT32 nErrCode = errno;
						if (nErrCode == EWOULDBLOCK)
						{
							break;
						}
						else
						{
							if (WFD_ISSET(nSockWriteFD, &stConnectedSet))
							{
								WFD_CLR(nSockWriteFD, &stConnectedSet);
								WFD_SET(nSockWriteFD, &stClosingSet);
								poWriteSession->SetState(esccRemoteClosed);
							}
							else if (WFD_ISSET(nSockWriteFD, &stClosingSet))
							{
								poWriteSession->SetState(esccRemoteClosed);
							}
							/*
							#if defined(__linux__) // connect error won't set in exception fds in linux
							else if(WFD_ISSET(nSockWriteFD, &stConnectingSet) )
							{
							WFD_CLR(nSockWriteFD, &stConnectingSet);
							WFD_CLR(nSockWriteFD, &stConnectedSet);
							INF("UpdateWrite send error, but in connectingSet, fd=%d", nSockWriteFD);
							poWriteSession->SetState(esccRemoteClosed);

							poMsg = new SSessionMsg;
							LOG_PROCESS_ERROR(poMsg);
							poMsg->eOperate = esoOnDisConnect;
							poMsg->nSocketFD = nSockWriteFD;
							PushMsgToMain(poMsg);
							}
							#endif
							*/
							else
							{
								LOG_CHECK_ERROR(FALSE);
							}

							poWriteSession->ClearSendMsg();
							INF("socket send has a error, error code is %d", nErrCode);

							break;
						}
					}

					if (pHeader->stRecord.stSendRecord.wNeedSend == 0)
					{
						poBuff = (PACKET_BUFFER*)oSendMsg.Pop();
						FreeBuffer(poBuff);
						poBuff = (PACKET_BUFFER*)oSendMsg.Check();

						//static INT64 qwRealSend;
						//qwRealSend++;
						//if(qwRealSend % (10) == 0)
						//    INF("qwRealSend:%d", qwRealSend/(10));
					}
				}

				if (poWriteSession->GetState() == esccLocalClosing && !poWriteSession->IsSendMsg())
				{
					nRetCode = shutdown(nSockWriteFD, SD_SEND);
					poWriteSession->SetState(esccLocalClosingWaitRecv);
				}
				else if (poWriteSession->GetState() == esccRemoteClosingWaitSend && !poWriteSession->IsSendMsg())
				{
					nRetCode = shutdown(nSockWriteFD, SD_SEND);
					poWriteSession->SetState(esccRemoteClosed);
				}
			}
		}

		return TRUE;
	Exit0:
		WFD_CLR(nSockWriteFD, &stConnectingSet);
		WFD_CLR(nSockWriteFD, &stConnectedSet);
		WFD_CLR(nSockWriteFD, &stClosingSet);
		if (poWriteSession)
		{
			poWriteSession->SetState(esccLocalClosed);
			poMsg = new SSessionMsg;
			LOG_PROCESS_ERROR(poMsg);
			poMsg->eOperate = esoOnDisConnect;
			poMsg->nSocketFD = nSockWriteFD;
			PushMsgToMain(poMsg);
		}
		return FALSE;

	}

	BOOL CNet::UpdateReadSocket(wfd_set& stReadSet, wfd_set& stListenSet, wfd_set& stConnectingSet, wfd_set& stConnectedSet, wfd_set& stClosingSet)
	{
		timeval stInterval;
		stInterval.tv_sec = 0;
		stInterval.tv_usec = 0;

		LINGER stLinger;
		stLinger.l_onoff = 1;
		stLinger.l_linger = 4;

		SOCKET nSockReadFD = INVALID_SOCKET;
		UINT32 dwNonBlock = 1;
		CSocketSession* poReadSession = NULL;
		SSessionMsg* poMsg = NULL;
		SOCKET nConnectedSocketFD = INVALID_SOCKET;

		for (UINT32 i = 0; i < stReadSet.fd_count; ++i)
		{
			nSockReadFD = stReadSet.fd_array[i];
			poReadSession = g_SessionList[nSockReadFD];
			if (!poReadSession)
			{
				INF("nSockReadFD %d in g_SessionList is null, continue;", nSockReadFD);
				continue;
			}

			BOOL bFind = FALSE;
			bFind = WFD_ISSET(nSockReadFD, &stListenSet);
			if (bFind)
			{
				INT32 nRetCode = 1;
				wfd_set stSignleSet;
				LOG_PROCESS_ERROR(poReadSession->GetState() == esccListening);

				while (nRetCode)
				{
					CSocketSession* poConnectedSession = NULL;
					nConnectedSocketFD = INVALID_SOCKET;

					nConnectedSocketFD = accept(nSockReadFD, NULL, NULL);
#if defined(__linux__)
					if (nConnectedSocketFD == INVALID_SOCKET && (errno == EWOULDBLOCK || errno == EAGAIN))
					{
						//listen socket if fine
						//INF("fd %d accept failed, errno = %d", nSockReadFD, errno);
						break;
					}
#endif         

					LOG_PROCESS_ERROR(nConnectedSocketFD != INVALID_SOCKET);
					LOG_PROCESS_ERROR(g_SessionList[nConnectedSocketFD] == NULL);

					poConnectedSession = (CSocketSession*)(CreateSession());
					LOG_PROCESS_ERROR(poConnectedSession != NULL);

#if defined(WIN32)
					nRetCode = ioctlsocket(nConnectedSocketFD, FIONBIO, (u_long*)&dwNonBlock);
#elif defined(__linux__)
					//nRetCode = ioctl(nConnectedSocketFD, FIONBIO, (u_long*)&dwNonBlock);
					{
						int nFlags = 0;
						nFlags = fcntl(nConnectedSocketFD, F_GETFL, 0);
						nRetCode = fcntl(nConnectedSocketFD, F_SETFL, nFlags | O_NONBLOCK);
					}
#endif


					nRetCode = setsockopt(nConnectedSocketFD, SOL_SOCKET, SO_RCVBUF, (char*)&(m_oInitParam.nRecvBufSize), sizeof((m_oInitParam.nRecvBufSize)));
					LOG_CHECK_ERROR(!nRetCode);

					nRetCode = setsockopt(nConnectedSocketFD, SOL_SOCKET, SO_SNDBUF, (char*)&(m_oInitParam.nSendBufSize), sizeof((m_oInitParam.nSendBufSize)));
					LOG_CHECK_ERROR(!nRetCode);

					nRetCode = setsockopt(nConnectedSocketFD, SOL_SOCKET, SO_LINGER, (char*)&(stLinger), sizeof(stLinger));
					LOG_CHECK_ERROR(!nRetCode);

					LOG_PROCESS_ERROR(g_SessionList[nConnectedSocketFD] == NULL);
					g_SessionList[nConnectedSocketFD] = poConnectedSession;
					poConnectedSession->SetSocketID(nConnectedSocketFD);

					poConnectedSession->SetAcceptor(poReadSession->GetAcceptor());
					poConnectedSession->SetState(esccConnected);

					WFD_SET(nConnectedSocketFD, &stConnectedSet);

					poMsg = new SSessionMsg;
					LOG_PROCESS_ERROR(poMsg);
					poMsg->eOperate = esoOnAccept;
					poMsg->nSocketFD = nConnectedSocketFD;
					PushMsgToMain(poMsg);

					WFD_ZERO(&stSignleSet);
					WFD_SET(nSockReadFD, &stSignleSet);
#if defined(WIN32)
					nRetCode = select(MAX_SOCKET_FD, &stSignleSet, NULL, NULL, &stInterval);
#elif defined(__linux__)
					nRetCode = linux_select(&stSignleSet, NULL, NULL, &stInterval);
#endif
				}
				continue;
			}

#if defined(__linux__)
			// since when call select() in linux
			// a failed connecting won't set in exception fd set but maybe set in read fd set to report error , 
			// soassume it works anyway, set it connected first to try recv data
			bFind = WFD_ISSET(nSockReadFD, &stConnectingSet);
			if (bFind)
			{
				if (poReadSession->GetState() == esccConnecting)
				{
					INF("fd %d in connecting set, and now readable, set it connected first");

					poReadSession->SetState(esccConnected);
					WFD_CLR(nSockReadFD, &stConnectingSet);
					WFD_SET(nSockReadFD, &stConnectedSet);

					poMsg = new SSessionMsg;
					LOG_PROCESS_ERROR(poMsg);
					poMsg->eOperate = esoOnConnect;
					poMsg->nSocketFD = nSockReadFD;
					PushMsgToMain(poMsg);

					continue;
				}
			}
#endif

			bFind = WFD_ISSET(nSockReadFD, &stConnectedSet) || WFD_ISSET(nSockReadFD, &stClosingSet);
			if (bFind)
			{
				CCircleList& oRecvMsg = poReadSession->GetRecvMsgList();
				INT32 nRecvLen;
				//INT32 nRetCode = 1;

				while (1)
				{
					nRecvLen = recv(nSockReadFD, poReadSession->GetRecvBuff(), poReadSession->GetNeedRecvSize(), 0);
					if (nRecvLen > 0)
					{
						poReadSession->SetCurRecvMsgSize(poReadSession->GetCurRecvMsgSize() + nRecvLen);
						if (poReadSession->GetCurRecvMsgSize() == 2)
						{
							INT32 nPackLen = 0;
							PACKET_BUFFER* poPacketBuff = NULL;
							SIMPLE_NET_HEADER* pHeader;
							nPackLen = poReadSession->GetNeedRecvSize();
							if (nPackLen > 0)
							{
								poPacketBuff = AllocBuffer(nPackLen);
								LOG_PROCESS_ERROR(poPacketBuff);

								pHeader = (SIMPLE_NET_HEADER*)((CHAR*)poPacketBuff - sizeof(SIMPLE_NET_HEADER));
								pHeader->stRecord.stRecvRecord.wPackageSize = nPackLen;

								poReadSession->SetRecvPacketBuffer(poPacketBuff);

								//static INT64 qwAllocRecv;
								//qwAllocRecv++;
								//if(qwAllocRecv % (1) == 0)
								//    INF("qwAllocRecv:%d", qwAllocRecv/(1));

							}
						}

						if (poReadSession->GetNeedRecvSize() == 0)
						{
							//static INT64 qwRecv;
							//qwRecv++;
							//if(qwRecv % (1) == 0)
							//    INF("qwRecv:%d", qwRecv/(1));
							if (poReadSession->GetRecvPacketBuffer())
							{
								oRecvMsg.Push(poReadSession->GetRecvPacketBuffer());
								poReadSession->SetRecvPacketBuffer(NULL);
							}
							poReadSession->SetCurRecvMsgSize(0);
						}
					}
					else if (nRecvLen == SOCKET_ERROR)
					{
						INT32 nError = errno;
						if (nError == EWOULDBLOCK)
						{
							break;
						}
						else
						{
							if (WFD_ISSET(nSockReadFD, &stConnectedSet))
							{
								WFD_CLR(nSockReadFD, &stConnectedSet);
								WFD_SET(nSockReadFD, &stClosingSet);
								poReadSession->SetState(esccRemoteClosed);
							}
							else if (WFD_ISSET(nSockReadFD, &stClosingSet))
							{
								poReadSession->SetState(esccRemoteClosed);
							}
							/*
							#if defined(__linux__) // connect error won't report in exception fds in linux
							else if(WFD_ISSET(nSockReadFD, &stConnectingSet) )
							{
							WFD_CLR(nSockReadFD, &stConnectingSet);
							WFD_CLR(nSockReadFD, &stConnectedSet);
							INF("UpdateWrite recv error, but in connectingSet, so set it remoteClosed, and esoOnDisConnect");
							poReadSession->SetState(esccRemoteClosed);

							poMsg = new SSessionMsg;
							LOG_PROCESS_ERROR(poMsg);
							poMsg->eOperate = esoOnDisConnect;
							poMsg->nSocketFD = nSockReadFD;
							PushMsgToMain(poMsg);
							}
							#endif
							*/
							else
							{
								LOG_PROCESS_ERROR(FALSE);
							}

							if (nError != ECONNRESET && nError != ECONNABORTED && nError != ESHUTDOWN)
								INF("some unprocess socket err[%d]", nError);

							//poMsg = new SSessionMsg;
							//LOG_PROCESS_ERROR(poMsg);
							//poMsg->eOperate = esoOnDisConnect;
							//poMsg->nSocketFD = nSockReadFD;
							//PushMsgToMain(poMsg);

							break;
						}
					}
					else //only nRecvLen == 0
					{
						if (WFD_ISSET(nSockReadFD, &stConnectedSet))
						{
							WFD_CLR(nSockReadFD, &stConnectedSet);
							WFD_SET(nSockReadFD, &stClosingSet);
							if (poReadSession->IsSendMsg())
							{
								poReadSession->SetState(esccRemoteClosingWaitSend);
							}
							else
							{
								shutdown(nSockReadFD, SD_SEND);
								poReadSession->SetState(esccRemoteClosed);
							}

						}
						else if (WFD_ISSET(nSockReadFD, &stClosingSet))
						{
							if (poReadSession->GetState() == esccLocalClosing)
								poReadSession->SetState(esccRemoteClosingWaitSend);
							else if (poReadSession->GetState() == esccLocalClosingWaitRecv)
								poReadSession->SetState(esccLocalClosed);
							else if (poReadSession->GetState() == esccRemoteClosed)
								;// don't process 
							else
							{
								INF("error: fd %d in ClosingSet but poSession State: %d", nSockReadFD, poReadSession->GetState());
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
		//WFD_CLR(nSockReadFD, &stConnectingSet);
		if (nConnectedSocketFD != INVALID_SOCKET)
			closesocket(nConnectedSocketFD);

		WFD_CLR(nSockReadFD, &stConnectedSet);
		WFD_CLR(nSockReadFD, &stListenSet);
		WFD_CLR(nSockReadFD, &stClosingSet);
		if (poReadSession)
		{
			poReadSession->SetState(esccLocalClosed);
			poMsg = new SSessionMsg;
			LOG_PROCESS_ERROR(poMsg);
			poMsg->eOperate = esoOnDisConnect;
			poMsg->nSocketFD = nSockReadFD;
			PushMsgToMain(poMsg);
		}

		return FALSE;
	}

	BOOL CNet::UpdateClosedSocket(wfd_set& stClosingSet)
	{
		SOCKET closeSockList[64];
		INT32 nCloseCount = 0;
		SSessionMsg* poMsg = NULL;
		CSocketSession* poSession = NULL;

		for (UINT32 i = 0; i < stClosingSet.fd_count; ++i)
		{
			SOCKET nSockID = stClosingSet.fd_array[i];
			ESessionState eState;

			poSession = g_SessionList[nSockID];
			if (!poSession)
			{
				//INF("%s Session failed!: poSession=NULL, fd=%d", __FUNCTION__, poMsg->nSocketFD );
				closeSockList[nCloseCount++] = nSockID;
				continue;
			}

			eState = poSession->GetState();
			if (eState == esccLocalClosed || eState == esccRemoteClosed)
			{
				poMsg = new SSessionMsg;
				LOG_PROCESS_ERROR(poMsg);
				poMsg->eOperate = esoOnDisConnect;
				poMsg->nSocketFD = nSockID;
				PushMsgToMain(poMsg);

				closeSockList[nCloseCount++] = nSockID;
			}
			else
			{
				//INF("fd %d in closing set,but eState=%d, so won't esoOnDisConnect\n", nSockID, eState );
			}
		}

		for (INT32 i = 0; i < nCloseCount; ++i)
			WFD_CLR(closeSockList[i], &stClosingSet);

		return TRUE;
	Exit0:
		return FALSE;
	}

#if defined(WIN32)
	UINT32 WINAPI CNet::_NetProc(VOID* pNet)
#elif defined(__linux__)
	VOID*  CNet::_NetProc(VOID* pNet)
#endif
	{
		CNet* poNet = (CNet*)pNet;
		BOOL bRun = TRUE;
		BOOL bMsg = FALSE;
		INT32 nRetCode = 0;
		timeval stInterval;

		wfd_set stReadSet;
		wfd_set stWriteSet;
		wfd_set stExceptSet;

		wfd_set stListenSet;
		wfd_set stConnectingSet;
		wfd_set stConnectedSet;
		wfd_set stClosingSet;

		WFD_ZERO(&stListenSet);
		WFD_ZERO(&stConnectingSet);
		WFD_ZERO(&stConnectedSet);
		WFD_ZERO(&stClosingSet);

		stInterval.tv_sec = 0;
		stInterval.tv_usec = 0;

		while (bRun || bMsg)
		{
			nRetCode = poNet->UpdateWorkingSocket(stListenSet, stConnectingSet, stConnectedSet, stClosingSet);
			LOG_PROCESS_ERROR(nRetCode);

			WFD_ZERO(&stExceptSet);
			WFD_ZERO(&stWriteSet);
			WFD_ZERO(&stReadSet);

#if defined(WIN32)            
			memcpy(&stExceptSet, &stConnectingSet, sizeof(wfd_set));
#endif
			memcpy(&stWriteSet, &stConnectingSet, sizeof(wfd_set));
			memcpy(&stReadSet, &stConnectedSet, sizeof(wfd_set));

#if defined(__linux__)
			// put fd from connecting set to read set
			for (INT32 i = 0; i < stConnectingSet.fd_count; ++i)
			{
				INT32 nSocketFD = stConnectingSet.fd_array[i];
				CSocketSession* poSession = g_SessionList[nSocketFD];
				if (poSession && poSession->GetSocketID() == nSocketFD)
					WFD_SET(stConnectingSet.fd_array[i], &stReadSet);
			}
#endif
			for (UINT32 i = 0; i < stListenSet.fd_count; ++i)
			{
				SOCKET nSocketFD = stListenSet.fd_array[i];
				CSocketSession* poSession = g_SessionList[nSocketFD];
				if (poSession && poSession->GetSocketID() == nSocketFD)
					WFD_SET(stListenSet.fd_array[i], &stReadSet);
			}

			for (UINT32 i = 0; i < stConnectedSet.fd_count; ++i)
			{
				SOCKET nSocketFD = stConnectedSet.fd_array[i];
				CSocketSession* poSession = g_SessionList[nSocketFD];
				if (poSession && poSession->GetSocketID() == nSocketFD
					&& poSession->GetSocketID() == stConnectedSet.fd_array[i]
					&& poSession->IsSendMsg())
					WFD_SET(stConnectedSet.fd_array[i], &stWriteSet);
			}

			for (UINT32 i = 0; i < stClosingSet.fd_count; ++i)
			{
				SOCKET nSocketFD = stClosingSet.fd_array[i];
				CSocketSession* poSession = g_SessionList[nSocketFD];
				if (poSession && poSession->GetSocketID() == nSocketFD)
				{
					if (poSession->GetState() != esccRemoteClosingWaitSend)
						WFD_SET(stClosingSet.fd_array[i], &stReadSet);
					if (poSession->GetState() == esccLocalClosing || poSession->GetState() == esccRemoteClosingWaitSend)
						WFD_SET(stClosingSet.fd_array[i], &stWriteSet);
				}
			}

#if defined(WIN32)
			nRetCode = select(MAX_SOCKET_FD, &stReadSet, &stWriteSet, &stExceptSet, &stInterval);
#elif defined(__linux__)
			nRetCode = linux_select(&stReadSet, &stWriteSet, &stExceptSet, &stInterval);
#endif
			if (nRetCode <= 0)
				Sleep(1);

			nRetCode = poNet->UpdateExceptSocket(stExceptSet, stWriteSet, stReadSet, stConnectingSet);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = poNet->UpdateWriteSocket(stWriteSet, stConnectingSet, stConnectedSet, stClosingSet);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = poNet->UpdateReadSocket(stReadSet, stListenSet, stConnectingSet, stConnectedSet, stClosingSet);
			LOG_CHECK_ERROR(nRetCode);

			nRetCode = poNet->UpdateClosedSocket(stClosingSet);
			LOG_CHECK_ERROR(nRetCode);

			if (!poNet->IsThreadRuning())
				bRun = FALSE;

			bMsg = poNet->IsUnProcessMsg();
		}

		return 0;
	Exit0:
		return 0;
	}

	BOOL CNet::MainLoop(void)
	{
		INT32 nRetCode = 0;
        std::unique_ptr<SSessionMsg> message;
		SSessionMsg* poMsg = NULL;
		CSocketSession* poSession = NULL;
		UINT64 qwStartTick = GetTickCountEx();

		LOG_PROCESS_ERROR(m_pNetHandler);

        message = m_MsgListToMain.Pop();
        poMsg = message.get();
		while (poMsg)
		{
			//INF("m_MsgListToMain.Pop() msg from poMsg->nSocketFD %d, poMsg->eOperate %d", poMsg->nSocketFD, poMsg->eOperate);
			poSession = g_SessionList[poMsg->nSocketFD];
			if (!poSession || poSession->GetSocketID() != poMsg->nSocketFD)
			{
                message.reset();
                message = m_MsgListToMain.Pop();
                poMsg = message.get();
				continue;
			}

			switch (poMsg->eOperate)
			{
			case esoOnAccept:
			{
				CSocketAcceptor* poAcceptor = poSession->GetAcceptor();
				LOG_PROCESS_ERROR(poAcceptor);

				m_pNetHandler->OnAccepted(poAcceptor, poSession);
				poMsg->eOperate = esoConnected;

				WFD_SET(poMsg->nSocketFD, &m_ConnetedSet);
                PushMsgToWork(message.release());
                poMsg = nullptr;

			}
				break;
			case esoOnConnect:
			{
				m_pNetHandler->OnConnected(poSession);
				poMsg->eOperate = esoConnected;

				WFD_SET(poMsg->nSocketFD, &m_ConnetedSet);
                PushMsgToWork(message.release());
                poMsg = nullptr;
			}
				break;
			case esoOnDisConnect:
			{
				PACKET_BUFFER* pPacketBuff = NULL;
				INT32 nSize = 0;

				LOG_PROCESS_ERROR(poSession->GetState() == esccLocalClosed || poSession->GetState() == esccRemoteClosed);
				while (poSession->IsRecvMsg())
				{
					nRetCode = poSession->GetRecvMsg(pPacketBuff, nSize);
					LOG_CHECK_ERROR(nRetCode);

					if (nRetCode)
					{
						m_pNetHandler->OnRecv(poSession, pPacketBuff, nSize);
						nSize = 0;
					}
				}
				m_pNetHandler->OnDisconnected(poSession, poSession->GetState() == esccRemoteClosed);
				WFD_CLR(poMsg->nSocketFD, &m_ConnetedSet);

				DestroySession(poSession);
				//INF("CNet::MainLoop() get main msg: esoOnDisConnect, SAFE_DELETE a msg from fd=%d, eOperate=%d", poMsg->nSocketFD, poMsg->eOperate );
                message.reset();
                poMsg = nullptr;
			}
				break;
			default:
				LOG_PROCESS_ERROR(FALSE);
			}

			UINT64 qwCurrentTick = GetTickCountEx();
			if (qwCurrentTick - qwStartTick > MAX_MAIN_THREAD_PROCESS_TIME)
			{
				INF("processing simple net event over time %d", (INT32)(qwCurrentTick - qwStartTick));
				break;
			}

            message = m_MsgListToMain.Pop();
            poMsg = message.get();
		}

		for (UINT32 i = 0; i < m_ConnetedSet.fd_count; ++i)
		{
			SOCKET nConnetedSocket = m_ConnetedSet.fd_array[i];
			CSocketSession* poSession = g_SessionList[nConnetedSocket];

			if (poSession && poSession->GetSocketID() == nConnetedSocket)
			{
				PACKET_BUFFER* pPacketBuff = NULL;
				INT32 nSize = 0;
				INT32 nPackageCount = MAX_OPERATION_PER_LOOP;

				while (poSession->IsRecvMsg() && nPackageCount > 0)
				{
					nRetCode = poSession->GetRecvMsg(pPacketBuff, nSize);
					LOG_CHECK_ERROR(nRetCode);

					if (nRetCode)
					{
						m_pNetHandler->OnRecv(poSession, pPacketBuff, nSize);
						nSize = 0;
						nPackageCount--;
					}
				}
			}

			UINT64 qwCurrentTick = GetTickCountEx();
			if (qwCurrentTick - qwStartTick > MAX_MAIN_THREAD_PROCESS_TIME)
			{
				INF("processing simple net message over time %d", (INT32)(qwCurrentTick - qwStartTick));
				break;
			}
		}

		return TRUE;
	Exit0:
		if (poSession)
			INF("%s  exit posession state:%d", __FUNCTION__, poSession->GetState());
		return FALSE;
	}

	INT64 CNet::PrintMemoryStat(void)
	{
		INT64 llSessionSize = m_oInitParam.nMaxSessionCount * sizeof(CSocketSession);
		INF("simple net session pool allocated %d sessions %.2fMB", m_oInitParam.nMaxSessionCount, 
			llSessionSize / (1024 * 1024.0f));

		INT64 llAcceptorSize = m_oInitParam.nMaxAcceptor * sizeof(CSocketAcceptor);
		INF("simple net acceptor pool allocated %d acceptors %.2fMB", m_oInitParam.nMaxAcceptor,
			llAcceptorSize / (1024 * 1024.0f));

        const auto pending = m_MsgListToMain.Size() + m_MsgListToWork.Size();
        const INT64 llMsgSize = pending * (sizeof(SSessionMsg) + sizeof(std::unique_ptr<SSessionMsg>));
		INF("simple net pending control events %zu, estimated payload %.2fMB", pending,
			llMsgSize / (1024 * 1024.0f));

		return llSessionSize + llAcceptorSize + llMsgSize;
	}

};
