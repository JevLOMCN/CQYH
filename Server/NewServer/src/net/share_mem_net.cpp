#include "stdafx.h"

#if defined(WIN32)

#include "share_mem_net.h"
#include "cas.h"
#include "string_ex.h"
#include "internal_message_header.h"

#define MAX_MAIN_THREAD_PROCESS_TIME    (500)

namespace ShareMem
{
	static const char* LISTENBUF_NAME_FMT = "%08X:%d listen";
	static const char* UPLOAD_BUF_NAME_FMT = "upload:%d:%d";
	static const char* DOWNLOAD_BUF_NAME_FMT = "download:%d:%d";

	CSocketSession::CSocketSession(void)
	{
		m_bDisconnected = FALSE;
		m_bReady = FALSE;
		m_poHandler = NULL;
		m_poPacketAllocator = NULL;
		m_nSerialNo = 0;
		m_nIndex = 0;
		m_nSendDataSize = 0;
		m_wThisMessageSize = 0;
		m_pUserData = NULL;
	}

	CSocketSession::~CSocketSession(void)
	{

	}

	INT32 CSocketSession::Send(PACKET_BUFFER* pPacketBuffer, INT32 nSize, INT32 nOffset)
	{
		INT32 nRetCode = 0;
		PACKET_BUFFER* pStart = NULL;
		SessionMemHeader* pHeader = nullptr;
		char* pWritePos = nullptr;

        LOG_PROCESS_ERROR(pPacketBuffer);
        LOG_PROCESS_ERROR(nSize >= 0);
		LOG_PROCESS_ERROR(m_poHandler);

		pHeader = (SessionMemHeader*)m_SendBuf.GetUserHeader();

		pWritePos = (char*)pPacketBuffer + nOffset - sizeof(UINT16);
		*(UINT16*)pWritePos = nSize;

		if(pHeader == NULL)
		{
			LOG_PROCESS_ERROR(m_nSendDataSize + nSize < MAX_MESSAGE_SIZE);

			memcpy(m_pMaxSendBuf + m_nSendDataSize, pWritePos, nSize + sizeof(UINT16));
			m_nSendDataSize += (nSize + sizeof(UINT16));

			//m_dwSendTick++;

			goto Exit1;
		}

		LOG_PROCESS_ERROR(pHeader->nConnState == csConnected);

		if(m_nSendDataSize > 0)
		{
			nRetCode = m_SendBuf.Write(m_pMaxSendBuf, m_nSendDataSize);
			LOG_PROCESS_ERROR(nRetCode);

			m_nSendDataSize = 0;
			//pHeader->dwSendTick += m_dwSendTick;
			//m_dwSendTick = 0;
		}

		nRetCode = m_SendBuf.Write((char*)pPacketBuffer + nOffset - sizeof(UINT16), nSize + sizeof(UINT16));
		LOG_PROCESS_ERROR(nRetCode);

Exit1:
		//DBG("Send: free");
		m_poPacketAllocator->Free(pPacketBuffer);
		return nSize;
Exit0:
		CRI("Send: close");
		Close();
		return 0;
	}

	BOOL CSocketSession::Recv(char* pPacketBuffer, INT32 nSize)
	{
		INT32 nRetCode = 0;
		SessionMemHeader* pLocalHeader = nullptr;

		LOG_PROCESS_ERROR(nSize >= 0);

		pLocalHeader = (SessionMemHeader*)m_RecvBuf.GetUserHeader();
		PROCESS_ERROR(pLocalHeader);

		nRetCode = m_RecvBuf.Read(pPacketBuffer, nSize);

//		DBG("Recv Data, SerialNo: %d, Size: %d", m_nSerialNo, nRetCode);

		PROCESS_ERROR(nRetCode);

		return TRUE;
Exit0:
		return FALSE;
	}

	void* CSocketSession::GetUserData(void)
	{
		return m_pUserData;
	}

	void CSocketSession::SetUserData(void* pUserData)
	{
		m_pUserData = pUserData;
	}

    BOOL CSocketSession::GetAddress(UINT32& dwAddr, UINT16& wPort)
    {
        LOG_PROCESS_ERROR(m_bReady);
        LOG_PROCESS_ERROR(!m_bDisconnected);

        dwAddr = m_dwAddr;
        wPort = m_nSerialNo;

        return TRUE;
Exit0:
        return FALSE;
    }

    BOOL CSocketSession::GetRemoteAddress(UINT32& dwRemoteAddr, UINT16& wRemotePort)
    {
        LOG_PROCESS_ERROR(m_bReady);
        LOG_PROCESS_ERROR(!m_bDisconnected);

        dwRemoteAddr = m_dwAddr;
        wRemotePort = m_nSerialNo;

        return TRUE;
Exit0:
        return FALSE;
    }

	BOOL CSocketSession::Init(INT32 nPingTimeout)
	{
		m_pUserData = NULL;
		m_bReady = FALSE;
		m_bDisconnected = FALSE;
		m_wThisMessageSize = 0;

		return TRUE;
	}

	BOOL CSocketSession::ReadyToUse(BOOL bIsClient, INT32 nPort, INT32 nSerialNo, INT32 nSendBufSize, INT32 nRecvBufSize)
	{
		INT32 nRetCode = 0;
		SessionMemHeader* pBufHeader = NULL;
		UINT64 qwSysTick = GetSysTick();

		char szName[64] = { 0 };
		const char* szSendFmt = NULL;
		const char* szRecvFmt = NULL;

		LOG_PROCESS_ERROR(m_poHandler);

		if(bIsClient)
		{
			szSendFmt = UPLOAD_BUF_NAME_FMT;
			szRecvFmt = DOWNLOAD_BUF_NAME_FMT;
		}
		else
		{
			szSendFmt = DOWNLOAD_BUF_NAME_FMT;
			szRecvFmt = UPLOAD_BUF_NAME_FMT;
		}
		snprintf(szName, 64, szSendFmt, nPort, nSerialNo);
		nRetCode = m_SendBuf.Create(szName, nSendBufSize, sizeof(SessionMemHeader));
		LOG_PROCESS_ERROR(nRetCode);

		snprintf(szName, 64, szRecvFmt, nPort, nSerialNo);
		nRetCode = m_RecvBuf.Create(szName, nRecvBufSize, sizeof(SessionMemHeader));
		LOG_PROCESS_ERROR(nRetCode);

		if(bIsClient)
		{
			m_SendBuf.Clear();
			m_RecvBuf.Clear();
		}

		pBufHeader = (SessionMemHeader*)m_SendBuf.GetUserHeader();
		pBufHeader->dwSerial = 0;
		pBufHeader->nConnState = csConnected;
		pBufHeader->qwTimeoutTick = qwSysTick;

		pBufHeader = (SessionMemHeader*)m_RecvBuf.GetUserHeader();
		pBufHeader->dwSerial = 0;
		pBufHeader->nConnState = csConnected;
		pBufHeader->qwTimeoutTick = qwSysTick;

		m_nSerialNo = nSerialNo;
		m_bReady = TRUE;

		return TRUE;
Exit0:
		m_SendBuf.Destroy();
		m_RecvBuf.Destroy();

		return FALSE;
	}


	BOOL CSocketSession::UnInit(void)
	{
		SessionMemHeader* pBufHeader = NULL;

		LOG_PROCESS_ERROR(m_poHandler);

		m_poHandler->OnDisconnected(this, FALSE);

		pBufHeader = (SessionMemHeader*)m_SendBuf.GetUserHeader();
		PROCESS_ERROR(pBufHeader);

		_EVA32(&pBufHeader->nConnState, csDisconnected);

		pBufHeader = (SessionMemHeader*)m_RecvBuf.GetUserHeader();
		PROCESS_ERROR(pBufHeader);

		_EVA32(&pBufHeader->nConnState, csDisconnected);

		m_bReady = FALSE;
		m_SendBuf.Destroy();
		m_RecvBuf.Destroy();

		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL CSocketSession::Mainloop(CNet* pNet)
	{
		INT32 nRetCode = 0;
        UINT64 qwStartTick = GetTickCountEx();

		if(!m_bReady)
			return TRUE;

		UINT64 qwTick = GetSysTick();

		INT64 llDiffTick = 0;

		SessionMemHeader* pLocalHeader = (SessionMemHeader*)m_RecvBuf.GetUserHeader();
        SessionMemHeader* pRemoteHeader = (SessionMemHeader*)m_SendBuf.GetUserHeader();

		INetEventHandler* poHandler = NULL; 

		poHandler = pNet->GetEventHandler();
		LOG_PROCESS_ERROR(poHandler);

		llDiffTick = qwTick - pLocalHeader->qwTimeoutTick;
		LOG_PROCESS_ERROR(llDiffTick < pNet->GetInitParam().nDisconnectTimeout);
        pRemoteHeader->qwTimeoutTick = qwTick;

		while(true)
		{
			if(m_wThisMessageSize == 0)
			{
				nRetCode = Recv((char*)&m_wThisMessageSize, sizeof(UINT16));
				if (!nRetCode)
					break;

				LOG_PROCESS_ERROR(m_wThisMessageSize <= pNet->GetInitParam().nRecvBufSize);
			}

			//--nOffset;

			m_pPackerBuffer = m_poPacketAllocator->Alloc(m_wThisMessageSize);

			LOG_PROCESS_ERROR(m_pPackerBuffer);

			nRetCode = Recv(m_pPackerBuffer, m_wThisMessageSize);

			if(!nRetCode)
			{
				m_poPacketAllocator->Free(m_pPackerBuffer);
				break;
			}

			poHandler->OnRecv(this, m_pPackerBuffer, m_wThisMessageSize);
			m_wThisMessageSize = 0;

            UINT64 qwCurrentTick = GetTickCountEx();
            if (qwCurrentTick - qwStartTick > MAX_MAIN_THREAD_PROCESS_TIME)
            {
                INF("processing share memory message over time %d", (INT32)(qwCurrentTick - qwStartTick));
                break;
            }
        }

		LOG_PROCESS_ERROR(pLocalHeader->nConnState != csDisconnected);

		return TRUE;
Exit0:
		//DBG("Diff Tick: %u", dwDiffTick);
		return FALSE;
	}

	BOOL CSocketSession::Close(void)
	{
//		DBG("Session Disconnected: %d", m_nIndex);
        struct in_addr addr;
        addr.s_addr = m_dwAddr;
        INF("ShareMem net close, Addr = %s %d", inet_ntoa(addr), m_nSerialNo );
		m_bDisconnected = TRUE;
		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////
	CSocketAcceptor::CSocketAcceptor(void)
	{
		memset(&m_AcceptBuffer, 0, sizeof(m_AcceptBuffer));
		m_nMaxConnCount = 0;
		m_nPort = 0;
		m_nSerialNo = 0;
		m_nIndex = 0;
		m_poNet = NULL;
		m_pUserData = NULL;

		m_SessionIndex = NULL;
	}

	CSocketAcceptor::~CSocketAcceptor(void)
	{

	}

	BOOL CSocketAcceptor::Init(CNet* pNet, INT32 nMaxConnCount, INT32 nIndex)
	{
		m_poNet = pNet;
		m_nMaxConnCount = nMaxConnCount;
		m_nIndex = nIndex;
		m_pUserData = NULL;

		m_SessionIndex = new INT32[nMaxConnCount];

		memset(m_SessionIndex, 0, nMaxConnCount * sizeof(INT32));

		return TRUE;
	}

	BOOL CSocketAcceptor::Open(UINT32 dwAddr, UINT16 wPort)
	{
		INT32 nRetCode = 0;
		AcceptBufHeader* pHeader = NULL;
		AcceptNode* pFirstNode = NULL;

		char szName[64] = { 0 };
		snprintf(szName, 64, LISTENBUF_NAME_FMT, dwAddr, wPort);

		// make sure the port has not been used.
		nRetCode = IsShareMemoryExists(szName);//OpenSharedMemory(szName, m_AcceptBuffer);
		LOG_PROCESS_ERROR(!nRetCode);

		nRetCode = CreateSharedMemory(szName, m_nMaxConnCount * sizeof(AcceptNode), m_AcceptBuffer);
		LOG_PROCESS_ERROR(nRetCode);
		LOG_PROCESS_ERROR(m_AcceptBuffer.pFd != NULL && m_AcceptBuffer.pSharedMem != NULL);

		pHeader = (AcceptBufHeader*)m_AcceptBuffer.pSharedMem;
		pHeader->nNodeCount = m_nMaxConnCount;
		pHeader->bClosed = FALSE;

		pFirstNode = (AcceptNode*)(m_AcceptBuffer.pSharedMem + sizeof(AcceptBufHeader));

		for(INT32 i = 0; i < m_nMaxConnCount; i++)
		{
			AcceptNode* pNode = pFirstNode + i;
			pNode->nAcceptState = asFree;
			pNode->nSerialNo = 0;
		}

		m_nPort = wPort;
        m_dwAddr = dwAddr;
		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL CSocketAcceptor::Close(void)
	{
		ISocketSession* poSession = NULL;
		INT32 nRetCode = 0;

		AcceptBufHeader* pHeader = (AcceptBufHeader*)m_AcceptBuffer.pSharedMem;
		PROCESS_ERROR(pHeader != NULL);

		for(INT32 i = 0; i < m_nMaxConnCount; i++)
			m_poNet->CloseSession(m_SessionIndex[i]);

		pHeader->bClosed = TRUE;

		if(m_AcceptBuffer.pSharedMem != NULL)
		{
			CloseSharedMemory(m_AcceptBuffer);
			m_AcceptBuffer.pSharedMem = NULL;
			m_AcceptBuffer.pFd = NULL;
			m_AcceptBuffer.qwSize = 0;
		}
		//QuerySharedMemory(m_AcceptBuffer);

Exit0:
		return TRUE;
	}

	BOOL CSocketAcceptor::Accept(void)
	{
		INT32 nRetCode = 0;
		CSocketSession* poSession = NULL;
		AcceptNode* pHeadNode = nullptr;

		INetEventHandler* poHandler = m_poNet->GetEventHandler();
		LOG_PROCESS_ERROR(poHandler);

		pHeadNode = (AcceptNode*)(m_AcceptBuffer.pSharedMem + sizeof(AcceptBufHeader));
		LOG_PROCESS_ERROR(pHeadNode != NULL);

		for(INT32 i = 0; i < m_nMaxConnCount; i++)
		{
			AcceptNode* pNode = pHeadNode + i;

			if(pNode->nAcceptState != asRequested)
				continue;

			INT32 nSerialNo = m_nSerialNo++;
			poSession = m_poNet->CreateSession();
			LOG_PROCESS_ERROR(poSession);
			poSession->Init(m_poNet->GetInitParam().nDisconnectTimeout);

			poSession->SetHandlerAndAllocator(m_poNet->GetEventHandler(), m_poNet->GetPacketAllocator());

			// Server
			nRetCode = poSession->ReadyToUse(FALSE, m_nPort, nSerialNo, m_poNet->GetInitParam().nSendBufSize, m_poNet->GetInitParam().nRecvBufSize);
			LOG_PROCESS_ERROR(nRetCode);

            poSession->SetAddr(m_dwAddr);

			pNode->nAcceptState = asAccepted;
			pNode->nSerialNo = nSerialNo;

			m_SessionIndex[i] = poSession->GetIndex();

			poHandler->OnAccepted(this, poSession);

//			DBG("Accepted : %d", i);
		}

		return TRUE;
Exit0:
		if(poSession != NULL)
			m_poNet->DestroySession(poSession);

		return FALSE;
	}

	void* CSocketAcceptor::GetUserData(void)
	{
		return m_pUserData;
	}

	void CSocketAcceptor::SetUserData(void* pUserData)
	{
		m_pUserData = pUserData;
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CNet::CNet(void)
	{
		m_poPackAllocator = NULL;
		m_poNetEventHandler = NULL;
		m_pAcceptorSlot = NULL;
		m_pSessionSlot = NULL;

	}

	CNet::~CNet(void)
	{
	}

	BOOL CNet::Init(IPacketAllocator* pPacketAllocator, INetEventHandler* pNetEvent, void* pInitParam)
	{
		INT32 nRetCode = 0;

		LOG_PROCESS_ERROR(pPacketAllocator);
		LOG_PROCESS_ERROR(pInitParam);
		LOG_PROCESS_ERROR(pNetEvent);

		m_poPackAllocator = pPacketAllocator;
		m_poNetEventHandler = pNetEvent;

		m_InitParam = *((SHMEM_INIT_PARAM*)pInitParam);

		nRetCode = m_AcceptorFreeNodePool.Init(m_InitParam.nMaxAcceptor);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_AcceptorPool.Init(m_InitParam.nMaxAcceptor);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_ConnectorPool.Init(m_InitParam.nMaxConnector);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_SessionFreeNodePool.Init(m_InitParam.nMaxSessionCount);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_SessionPool.Init(m_InitParam.nMaxSessionCount);
		LOG_PROCESS_ERROR(nRetCode);

		m_pAcceptorSlot = new CSocketAcceptor*[m_InitParam.nMaxAcceptor];
		LOG_PROCESS_ERROR(m_pAcceptorSlot);
		memset(m_pAcceptorSlot, 0, m_InitParam.nMaxAcceptor*sizeof(CSocketAcceptor*));

		m_ConnectorList.Clear();
		m_AcceptorFreeList.Clear();
		m_SessionFreeList.Clear();

		for(INT32 i = 0; i < m_InitParam.nMaxAcceptor; i++)
		{
			FREE_NODE* node = m_AcceptorFreeNodePool.New();
			LOG_PROCESS_ERROR(node);

			ClearNode(node);

			node->nIndex = i;

			nRetCode = m_AcceptorFreeList.PushRear(node);
			LOG_PROCESS_ERROR(nRetCode);
		}

		m_pSessionSlot = new CSocketSession*[m_InitParam.nMaxSessionCount];
		LOG_PROCESS_ERROR(m_pSessionSlot);
		memset(m_pSessionSlot, 0, m_InitParam.nMaxSessionCount*sizeof(CSocketSession*));

		for(INT32 i = 0; i < m_InitParam.nMaxSessionCount; i++)
		{
			FREE_NODE* node = m_SessionFreeNodePool.New();
			LOG_PROCESS_ERROR(node);

			ClearNode(node);

			node->nIndex = i;

			nRetCode = m_SessionFreeList.PushRear(node);
			LOG_PROCESS_ERROR(nRetCode);
		}


		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL CNet::UnInit(void)
	{
		m_poPackAllocator = NULL;

		SAFE_DELETE_ARRAY(m_pAcceptorSlot);
		SAFE_DELETE_ARRAY(m_pSessionSlot);

		FREE_NODE* node = NULL;

		while((node = (FREE_NODE*)m_AcceptorFreeList.PopHead()) != NULL)
			m_AcceptorFreeNodePool.Delete(node);

		while((node = (FREE_NODE*)m_SessionFreeList.PopHead()) != NULL)
			m_SessionFreeNodePool.Delete(node);


		m_AcceptorFreeNodePool.UnInit();
		m_SessionFreeNodePool.UnInit();

		return TRUE;
	}

	ISocketAcceptor* CNet::CreateAcceptor(void)
	{
		INT32 nRetCode = 0;
		CSocketAcceptor* poAcceptor = NULL;
		FREE_NODE* node = NULL;

		PROCESS_ERROR(!m_AcceptorFreeList.IsEmpty());

		node = (FREE_NODE*)m_AcceptorFreeList.PopHead();
		LOG_PROCESS_ERROR(node);

		LOG_PROCESS_ERROR(node->nIndex >= 0 && node->nIndex < m_InitParam.nMaxAcceptor);

		poAcceptor = m_AcceptorPool.New();

		m_AcceptorFreeNodePool.Delete(node);

		nRetCode = poAcceptor->Init(this, m_InitParam.nMaxSessionCount, node->nIndex);
		LOG_PROCESS_ERROR(nRetCode);

		m_pAcceptorSlot[node->nIndex] = poAcceptor;

		return poAcceptor;
Exit0:
		SAFE_DELETE(poAcceptor);
		return NULL;
	}

	BOOL CNet::DestroyAcceptor(ISocketAcceptor* pSocket)
	{
		INT32 nRetCode = 0;
		INT32 nIndex = 0;
		FREE_NODE* node = NULL;

		CSocketAcceptor* poAcc = (CSocketAcceptor*)pSocket;
		LOG_PROCESS_ERROR(poAcc);

		nRetCode = poAcc->Close();
		LOG_PROCESS_ERROR(nRetCode);

		nIndex = poAcc->GetIndex();
		LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_InitParam.nMaxAcceptor);

		m_pAcceptorSlot[nIndex] = NULL;

		m_AcceptorPool.Delete(poAcc);

		node = m_AcceptorFreeNodePool.New();
		LOG_PROCESS_ERROR(node);

		ClearNode(node);

		node->nIndex = nIndex;

		nRetCode = m_AcceptorFreeList.PushHead(node);
		LOG_PROCESS_ERROR(nRetCode);

		return TRUE;
Exit0:
		return FALSE;
	}

	ISocketSession* CNet::Connect(UINT32 dwAddr, UINT16 wPort)
	{
		INT32 nRetCode = 0;
		INT32 nSlot = -1;

		AcceptBufHeader* pHeader = NULL;
		AcceptNode* pHeadNode = NULL;
		CSocketSession* poSession = NULL;
		CONNECTOR_NODE* pConnectorNode = NULL;

		char szName[64] = { 0 };
		snprintf(szName, 64, LISTENBUF_NAME_FMT, dwAddr, wPort);

		MemPtr Ptr;
		nRetCode = OpenSharedMemory(szName, Ptr);
		PROCESS_ERROR(nRetCode);
		PROCESS_ERROR(Ptr.pFd != NULL && Ptr.pSharedMem != NULL);

		pHeader = (AcceptBufHeader*)Ptr.pSharedMem;

		pHeadNode = (AcceptNode*)(Ptr.pSharedMem + sizeof(AcceptBufHeader));

		for(INT32 i = 0; i < pHeader->nNodeCount; i++)
		{
			AcceptNode* pNode = pHeadNode + i;

			INT32 nInitValue = _CAS32(&pNode->nAcceptState, asRequested, asFree);
			if(nInitValue != asFree)
				continue;

			nSlot = i;
			break;
		}

		PROCESS_ERROR(nSlot >= 0);

		poSession = CreateSession();
		LOG_PROCESS_ERROR(poSession);

		poSession->Init(m_InitParam.nDisconnectTimeout);

		poSession->SetHandlerAndAllocator(m_poNetEventHandler, m_poPackAllocator);
        poSession->SetAddr(dwAddr);

		pConnectorNode = m_ConnectorPool.New();
		LOG_PROCESS_ERROR(pConnectorNode);

		ClearNode(pConnectorNode);

		pConnectorNode->Ptr = Ptr;
		pConnectorNode->nAcceptSlot = nSlot;
		pConnectorNode->nPort = wPort;
		pConnectorNode->nSessionSlot = poSession->GetIndex();

		nRetCode = m_ConnectorList.PushHead(pConnectorNode);
		LOG_PROCESS_ERROR(nRetCode);

        INF("connected to %s via share memory successfully", szName);

		return poSession;
Exit0:
		WRN("ShareMem net connect failed: %s", szName);
		if(poSession != NULL)
			DestroySession(poSession);

		return NULL;
	}

	BOOL CNet::CloseSession(INT32 nIndex)
	{
		CSocketSession* poSession = NULL;

		LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_InitParam.nMaxSessionCount);

		poSession = m_pSessionSlot[nIndex];

		if(poSession != NULL)
			return poSession->Close();

		return TRUE;
Exit0:
		return FALSE;
	}


	CSocketSession* CNet::CreateSession(void)
	{
		FREE_NODE* node = NULL;
		CSocketSession* poSession = NULL;

		node = (FREE_NODE*)m_SessionFreeList.PopHead();
		LOG_PROCESS_ERROR(node);

		LOG_PROCESS_ERROR(node->nIndex >= 0 && node->nIndex < m_InitParam.nMaxSessionCount);

		m_SessionFreeNodePool.Delete(node);

		poSession = m_SessionPool.New();
		LOG_PROCESS_ERROR(poSession);

		poSession->SetIndex(node->nIndex);

		m_pSessionSlot[node->nIndex] = poSession;

		return poSession;
Exit0:
		return NULL;
	}

	BOOL CNet::DestroySession(CSocketSession* poSession)
	{
		INT32 nIndex = -1;
		FREE_NODE* node = NULL;
		INT32 nRetCode = 0;

		nIndex = poSession->GetIndex();
		LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_InitParam.nMaxSessionCount);

		node = m_SessionFreeNodePool.New();
		LOG_PROCESS_ERROR(node);

		ClearNode(node);

		node->nIndex = nIndex;

		m_pSessionSlot[nIndex] = NULL;

		m_SessionPool.Delete(poSession);

		nRetCode = m_SessionFreeList.PushHead(node);
		LOG_PROCESS_ERROR(nRetCode);

		return TRUE;

Exit0:
		return FALSE;
	}

	BOOL CNet::MainLoop(void)
	{
		INT32 nRetCode = 0;

		for(INT32 i = 0; i < m_InitParam.nMaxSessionCount; i++)
		{
			CSocketSession* poSession = m_pSessionSlot[i];
			if(poSession == NULL)
				continue;

			nRetCode = poSession->Mainloop(this);
			if(!nRetCode)
            {
                LOG_CHECK_ERROR(FALSE);
				poSession->Close();
            }

			if(poSession->IsDisconnected())
			{
				poSession->UnInit();
				DestroySession(poSession);
			}
		}

		BASE_LIST_NODE* pNode = m_ConnectorList.Head().pNext;

		while(pNode != &m_ConnectorList.Rear())
		{
			BASE_LIST_NODE* pNext = pNode->pNext;

			CONNECTOR_NODE* pConnectorNode = (CONNECTOR_NODE*)pNode;

			AcceptBufHeader* pHeader = (AcceptBufHeader*)pConnectorNode->Ptr.pSharedMem;
			AcceptNode* pHeadNode = (AcceptNode*)(pConnectorNode->Ptr.pSharedMem + sizeof(AcceptBufHeader));
			AcceptNode* pAccNode = NULL;
			INT32 nInitValue = 0;
			CSocketSession* poSession = NULL;

			LOG_PROCESS_ERROR(!pHeader->bClosed);
			LOG_PROCESS_ERROR(pConnectorNode->nAcceptSlot >= 0 && pConnectorNode->nAcceptSlot < pHeader->nNodeCount);
			LOG_PROCESS_ERROR(pConnectorNode->nSessionSlot >= 0 && pConnectorNode->nSessionSlot < m_InitParam.nMaxSessionCount);

			pAccNode = pHeadNode + pConnectorNode->nAcceptSlot;

			poSession = m_pSessionSlot[pConnectorNode->nSessionSlot];

			LOG_PROCESS_ERROR(poSession != NULL && !poSession->IsDisconnected());

			nInitValue = _CAS32(&pAccNode->nAcceptState, asFree, asAccepted);

			if(nInitValue != asAccepted)
			{
				LOG_PROCESS_ERROR(nInitValue == asRequested);

				pNode = pNext;
				continue;
			}

			// Client
			nRetCode = poSession->ReadyToUse(TRUE, pConnectorNode->nPort, pAccNode->nSerialNo, m_InitParam.nSendBufSize, m_InitParam.nRecvBufSize);
			LOG_PROCESS_ERROR(nRetCode);

			m_poNetEventHandler->OnConnected(poSession);

			m_ConnectorList.Remove(pConnectorNode);
			m_ConnectorPool.Delete(pConnectorNode);
			CloseSharedMemory(pConnectorNode->Ptr);
			pNode = pNext;

			continue;
Exit0:
			if(pAccNode != NULL)
				pAccNode->nAcceptState = asFree;

			if(poSession != NULL)
				poSession->Close();

			CloseSharedMemory(pConnectorNode->Ptr);
			m_ConnectorList.Remove(pConnectorNode);
			m_ConnectorPool.Delete(pConnectorNode);
			pNode = pNext;
		}

		for(INT32 i = 0; i < m_InitParam.nMaxAcceptor; i++)
		{
			CSocketAcceptor* poAcc = m_pAcceptorSlot[i];
			if(poAcc == NULL)
				continue;

			nRetCode = poAcc->Accept();
			LOG_CHECK_ERROR(nRetCode);
		}


		return TRUE;
	}

	INT64 CNet::PrintMemoryStat(void)
	{
		INT64 llAcceptorSize = m_InitParam.nMaxAcceptor * (sizeof(FREE_NODE) + sizeof(CSocketAcceptor));
		INF("share mem acceptor pool allocated %d accpetors %.2fMB", m_InitParam.nMaxAcceptor,
			llAcceptorSize / (1024 * 1024.0f));

		INT64 llConnectorSize = m_InitParam.nMaxConnector * sizeof(CONNECTOR_NODE);
		INF("share mem connector pool allocated %d connectors %.2fMB", m_InitParam.nMaxConnector,
			llConnectorSize / (1024 * 1024.0f));

		INT64 llSessionSize = m_InitParam.nMaxSessionCount * (sizeof(FREE_NODE) + sizeof(CSocketSession));
		INF("share mem session pool allocated %d sessions %.2fMB", m_InitParam.nMaxSessionCount,
			llSessionSize / (1024 * 1024.0f));

		return llAcceptorSize + llConnectorSize + llSessionSize;
	}

};

#endif //WIN32
