#include "stdafx.h"
#include "message_handler.h"
#include "external_message_size.h"
#include "MMOAchievementManager.h"
#include "ScaleformEngine.h"
#include <bitset>

#include "packet_allocator.h"
#include "simple_net.h"

#include "role_db_def.h"

#include "UITextParser.h"
#include "ServerErrorEnumDescriptor.h"
#include "MMOGameEvent_Splash.h"
#include "CommonGameEvents.h"
#include "MMOFacade.h"
#include "MMOStats.h"
#include "AbilityProcessor.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"
#include "FSMIO.h"
#include "MMOGuildManager.h"
#include "MMOChatManager.h"
#include "..\..\Common\define\version.h" // oscar@snda
#include "error_code_name.h"

#include "ReplayManager.h"
#include "MMOWeatherManager.h"
#include "MMOHeavenManager.h"
#include "MMOSkillManager.h"
#include "TeamManager.h"
#include "RankListManager.h"
#include "MMOMsgBoxManager.h"
#include "FriendManager.h"
#include "MMOQuestManager.h"
#include "MMOAuctionManager.h"
#include "MMOVipManager.h"
#include "LoadMapTipsManager.h"
#include "ActivityManager.h"
#include "MMOAttributeAccountManager.h"
#include "MMOLoginManager.h"
#include "Mir2Attack.h"
#include "..\..\WinDrv\Inc\DeviceInfoHelper.h"
#include "MMODropManager.h"
#include "MMOJourneyManager.h"
#include "zlib.h"
#include "UStallManager.h"
#include "MMOGuideManager.h"
#include "MMOJigsawManager.h"
#include "MMOMsgBoxManager.h"
#include "MMODynamicPwdManager.h"
#include <IPTypes.h>


extern INT g_moveModGridSize;
extern INT g_netLagToleranceDisatnce;
extern bool g_disableAllInput;
bool g_disconnectByUser = false;
bool g_bOnReturnLoginConfirm = false;

#if USING_GPK
#include "../../External/GPK3.0.13/Include/GPKitClt.h"

extern SGPK::IGPKCltDynCode *GCltDynCode;
#endif

using namespace std;
using namespace ExternalMessage;

CMessageHandler CMessageHandler::ms_Instance;
char            CMessageHandler::ms_MsgBuffer[MAX_MESSAGE_SIZE];


extern FLOAT GAverageFPS, GUnit_RenderThreadTime, GUnit_GPUFrameTime, GUnit_GameThreadTime, GUnit_FrameTime;

extern FLOAT g_Run_AniRate[EPCT_MAX];
extern FLOAT g_Walk_AniRate[EPCT_MAX];
extern FLOAT g_Run_Oblique_AniRate[EPCT_MAX];
extern FLOAT g_Walk_Oblique_AniRate[EPCT_MAX];
extern FLOAT g_Run_Phase[EPCT_MAX];
extern FLOAT g_Walk_Phase[EPCT_MAX];
extern FLOAT g_Run_Oblique_Phase[EPCT_MAX];
extern FLOAT g_Walk_Oblique_Phase[EPCT_MAX];
extern UBOOL g_ResetPhaseFlag;
extern UBOOL g_UniformSpeed;

struct tm_ex
{
	INT32 tm_msec;	/* millisecond after the second - [0,999]*/
	INT32 tm_sec;     /* seconds after the minute - [0,59] */
	INT32 tm_min;     /* minutes after the hour - [0,59] */
	INT32 tm_hour;    /* hours since midnight - [0,23] */
	INT32 tm_mday;    /* day of the month - [1,31] */
	INT32 tm_mon;     /* months since January - [0,11] */
	INT32 tm_year;    /* years since 1900 */
	INT32 tm_wday;    /* days since Sunday - [0,6] */
};

static tm_ex t;

const tm_ex* localtime_ex(void)
{
	SYSTEMTIME SystemTime;

	GetLocalTime(&SystemTime);

	t.tm_msec = SystemTime.wMilliseconds;
	t.tm_sec = SystemTime.wSecond;
	t.tm_min = SystemTime.wMinute;
	t.tm_hour = SystemTime.wHour;
	t.tm_mday = SystemTime.wDay;
	t.tm_mon = SystemTime.wMonth;
	t.tm_year = SystemTime.wYear;
	t.tm_wday = SystemTime.wDayOfWeek;

	return &t;
}

FString g_sdoSessionID;
extern bool g_isSDOLogin;


INT32 g_SendClearAbilityCacheTimes=0;
INT32 g_RecieveClearAbilityCacheTimes=0;
FLOAT g_UpdateClearAbilityCacheTime=-1.0f;
UBOOL g_PreMovetoPoint=TRUE;

#pragma warning(push)
#pragma warning(disable:4731)

#if defined(WIN32)
INT32 _ASN32(volatile INT32* pDest, INT32 Value)
{
	return appInterlockedExchange(pDest, Value);
}

INT32 _CAS32(volatile INT32* pDestination, INT32 Exchange, INT32 Comparand)
{
	return appInterlockedCompareExchange((INT*)pDestination, Exchange, Comparand);
}

INT64 _CAS(volatile INT64* pDestination, INT64 Exchange, INT64 Comparand)
{
	*pDestination = Exchange;
	return Comparand;
	//__asm
	//{
	//    push        ebx
	//    push        ebp
	//    mov         ebp, dword ptr [esp + 14h] 
	//    mov         ebx, dword ptr [esp + 18h] 
	//    mov         ecx, dword ptr [esp + 1Ch] 
	//    mov         eax, dword ptr [esp + 20h] 
	//    mov         edx, dword ptr [esp + 24h] 
	//    lock cmpxchg8b   qword ptr [ebp] 
	//    pop         ebp  
	//    pop         ebx
	//    ret
	//}
	//        return InterlockedCompareExchange64(pDestination, Exchange, Comparand);
}
#elif defined(LINUX32) 
INT32 _CAS32(INT32* pDestination, INT32 Exchange, INT32 Comparand)
{
	return 0;
}

INT64 _CAS(INT64* pDestination, INT64 Exchange, INT64 Comparand)
{
	return __sync_val_compare_and_swap(pDestination, Exchange, Comparand);
}
#else
#error("unknown os")
#endif 

#pragma warning(pop)

size_t strxcpy(char *dst, const TCHAR *src, size_t maxSize)
{
	FTCHARToUTF8 conv(src);
	size_t newSz = conv.Length()+1;//last char '/0'
	return strxcpy(dst, conv.operator ANSICHAR *(), newSz > maxSize ? maxSize : newSz);
}

size_t strxcpy(char *dst, const char *src, size_t size)
{
	const char *d = dst;

	if (dst == NULL || src == NULL || size == 0)
		return 0;

	while (--size > 0)
	{
		if ((*dst++ = *src++) == 0)
			break;
	}

	if (size == 0)
	{
		*dst++ = 0;
	}

	return dst - d - 1;
}

size_t strxcat(char *dst, const char *src, size_t size)
{
	const char *d = dst;

	if (dst == NULL || src == NULL || size == 0)
		return 0;

	while (--size && *dst++);

	while (--size > 0)
	{
		if ((*dst++ = *src++) == 0)
			break;
	}

	if (size == 0)
	{
		*dst++ = 0;
	}

	return dst - d - 1;
}
//keep the identifier's complete utf8
size_t CopyUTF8(char* dst,const TCHAR* src, size_t maxSize)
{
	if (dst == NULL || src == NULL || maxSize == 0)
		return 0;

	FString strDes = TEXT("");
	INT nPos = (size_t)UITextParser::getSingleton().SplitStringTagWithUTF8MaxLength(FString(src),strDes,maxSize);
	if(nPos >= 0)
	{
		FTCHARToUTF8 conv(*strDes);
		size_t newSz = conv.Length()+1;//last char '/0'
		newSz = (newSz > maxSize ? maxSize : newSz);
		strxcpy(dst, conv.operator ANSICHAR *(), newSz);
		return newSz;
	}
	return 0;
}
	

void CMessageHandler::OnAccepted(ISocketAcceptor* pSocketAcceptor, ISocketSession* pSocketSession)
{
    return;
}

extern void CreateCustomDump(const TCHAR* fileName);

extern FString appGetMacAddress();


BYTE g_loginServerGroup = 1;

void CMessageHandler::OnConnected(ISocketSession* pSocketSession)
{
    INT32 nRetCode = 0;

	FString dbgStr = TEXT("N/A");
	if(pSocketSession)
	{
		if(pSocketSession->GetUserData())
		{
			dbgStr = *(FString*)pSocketSession->GetUserData();
		}

		warnf( NAME_Log, TEXT("=====####========== [CMessageHandler::OnConnected][CONNECTSTAT] %s m_nSockedFD[%x]"),
			*dbgStr, ((SimpleNet::CSocketSession*)pSocketSession)->m_nSockedFD );
	}


#if !SHIPPING_PC_GAME && M_ENABLE_RPMGR
	if(g_replayMode)
	{
		return;
	}
#endif
    
#if 0
	FString tmpDump = FString::Printf( TEXT("%sOnConnected-%d-%d-%d-%s.dmp"), *appGameLogDir(), MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER, *appSystemTimeString() );
	CreateCustomDump(*tmpDump);
#endif

    LOG_PROCESS_ERROR(pSocketSession);

    if(_connectState == NCS_ConnectingBS)
    {
		SET_WARN_COLOR(COLOR_CYAN);
		warnf( NAME_Log, TEXT("=====####========== [CMessageHandler::OnConnected]") );
		CLEAR_WARN_COLOR();
		//wait query_gate_ack package;
    }
    else if(_connectState == NCS_Connecting)
    {
		if(pSocketSession == m_pSocketSessionGate)
		{
			warnf( NAME_Log, TEXT("=====####========== [CMessageHandler::OnConnected] Send Login [%x]"), ((SimpleNet::CSocketSession*)pSocketSession)->m_nSockedFD );
			C2T_LOGIN_REQ msg; //= (C2T_LOGIN_REQ*)ms_MsgBuffer;

			memset(&msg, 0, sizeof(msg)); // szPassword etc. were left uninitialized
			msg.wMsg = c2t_login_req;
			//msg->wSize = sizeof(C2T_LOGIN_REQ);
			msg.byGroup = g_loginServerGroup;
			msg.wMajorVersion = EXTERNAL_MESSAGE_MAJOR_VERSION;
			msg.wMinorVersion = EXTERNAL_MESSAGE_MINOR_VERSION;
			msg.dwCRC = g_nMessageVersion;
			memset(msg.szMac, 0, MAC_ADDRESS_LEN);
			FString MacAddress = appGetMacAddress();
			char* AnsiMac = new char[MacAddress.Len()];
			memset(AnsiMac, 0, MacAddress.Len());
			for (INT i = 0; i < MacAddress.Len(); ++i)
			{
				AnsiMac[i] = MacAddress[i];
			}
			memcpy(msg.szMac, AnsiMac, MAC_ADDRESS_LEN);
			delete []AnsiMac;

			//msg->wInfoLen = 0;
			strxcpy(msg.szUserName, _UserName, USER_NAME_LEN);
			strxcpy(msg.szPassword, _PW, USER_PASSWORD_LEN);
			//strxcpy(msg.szUserType, L"TEST_TYPE_1", USER_TYPE_LEN);

			if(g_isSDOLogin)
			{
				msg.byGroup = g_loginServerGroup;
				strxcpy(msg.szSessionID, (const TCHAR*)*g_sdoSessionID, OA_SESSION_ID_LEN);
			}

			_connectState = NCS_Connected;

			nRetCode = _Send(&msg, sizeof(msg));
			LOG_PROCESS_ERROR(nRetCode);
		}
    }

    return;
Exit0:
    if (pSocketSession)
        pSocketSession->Close();

    return;
}

void __OnRecv(PACKET_BUFFER* pPacketBuffer, UINT32 dwSize, UINT16 wMsg, CMessageHandler::MESSAGE_FUNC* funs, CMessageHandler* hdl)
{
	STAT_AUTO_CYCLE_COUNTER(CMessageHandler::GetInstance().m_MessageDebugString[wMsg]);

	M_FORCE_TRY_BEGIN;
	(hdl->*funs[wMsg])(pPacketBuffer, dwSize);
	M_FORCE_TRY_END("Protected for __OnRecv");
}

//void CMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
//{
//	NET_PACKET NetPacket;
//	MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pPacketBuffer;
//
//	if (pHeader->wMsg == g2c_frame_signal)
//	{
//		MMOFacade::getSingleton().AddServerFrameCount();
//
//		m_pPacketAllocator->Free(pPacketBuffer);
//	}
//	else if (!_lockPlayer && pHeader->wMsg >= g2c_message_begin && pHeader->wMsg < g2c_message_end)
//	{
//		NetPacket.nServerFrame = MMOFacade::getSingleton().GetServerFrameCount();
//		NetPacket.pPacketBuffer = pPacketBuffer;
//		NetPacket.dwSize = dwSize;
//
//		m_PacketCache.push_back(NetPacket);
//	}
//	else
//	{
//		_OnRecv(pSocketSession, pPacketBuffer, dwSize);
//	}
//
//	return;
//}

void CMessageHandler::OnRecv(ISocketSession* pSocketSession, PACKET_BUFFER* pPacketBuffer, UINT32 dwSize)
{
	MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)pPacketBuffer;

	if (m_byEncryptCode)
	{
		for (UINT32 i = sizeof(FLEX_MESSAGE_HEADER); i< dwSize; i++)
			pPacketBuffer[i] = pPacketBuffer[i] ^ m_byEncryptCode;
	}

#if M_ENABLE_RPMGR
	if(!g_replayMode)
	{
		FReplayDataNetwork* newDat = new FReplayDataNetwork;
		newDat->DataType = ENDT_Recv;
		newDat->DataSize = dwSize;
		newDat->GameTime = appSeconds() - GStartTime;
		newDat->DataBuffer = new char[dwSize];
		appMemcpy(newDat->DataBuffer, pPacketBuffer, dwSize);

		ReplayManager::getSingleton().Recode(newDat);
	}
#endif


#if defined(_DEBUG) && 0
	if(pHeader->wMsg != g2c_frame_signal
		&& pHeader->wMsg != g2c_focus_hifreq_sync
		&& pHeader->wMsg != g2c_self_hifreq_sync
		&& pHeader->wMsg != g2c_region_hifreq_sync
		&& pHeader->wMsg != t2c_ping_ack
		&& pHeader->wMsg != g2c_ping_ack
		&& pHeader->wMsg != g2c_char_move)
	{
		SET_WARN_COLOR(COLOR_CYAN);
		warnf(NAME_Log, TEXT("##################[MessageHandler::OnRecv,msgid(%d)]:%s"), pHeader->wMsg, *m_MessageDebugString[pHeader->wMsg]);
		CLEAR_WARN_COLOR();
	}
#endif

	if(!m_MessageFuncList[pHeader->wMsg])
	{
		warnf(NAME_Error, TEXT("Protocol handler not found: %d"), pHeader->wMsg);
	}

	LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);
	
#if M_ENABLE_NET_MSG_BUFF
	if(pHeader->wMsg == t2c_ping_ack
		|| pHeader->wMsg == g2c_ping_ack
		|| pHeader->wMsg == b2c_query_gate_ack
		|| pHeader->wMsg == g2c_error_notify
		|| pHeader->wMsg == m2c_error_notify
		|| pHeader->wMsg == t2c_error_notify)
	{
		__OnRecv(pPacketBuffer, dwSize, pHeader->wMsg, &m_MessageFuncList[0], this);
	}
	else
	{
		AddToMsgBuf(pPacketBuffer, dwSize);
	}
#else
	__OnRecv(pPacketBuffer, dwSize, pHeader->wMsg, &m_MessageFuncList[0], this);
#endif
    //(this->*m_MessageFuncList[pHeader->wMsg])(pPacketBuffer, dwSize);

	INC_FLOAT_STAT_BY(MMOSTAT_Network_Download, (float)dwSize);
Exit0:
    m_pPacketAllocator->Free(pPacketBuffer);

    return;
}

#if M_ENABLE_NET_MSG_BUFF

void CMessageHandler::ClearNetMsgBuf()
{
	FScopeLock Scope(&_msgCacheLock);
	while(!_msgCache.empty())
	{
		NetMsgCacheBuff buf = _msgCache.front();
		delete[] buf.Buffer;
		_msgCache.pop_front();
	}
}

void CMessageHandler::AddToMsgBuf(PACKET_BUFFER* buf, UINT32 msgSize)
{
	FScopeLock Scope(&_msgCacheLock);
	NetMsgCacheBuff nmb;
	nmb.Buffer = new PACKET_BUFFER[msgSize];
	nmb.Size = msgSize;
	appMemcpy(nmb.Buffer, buf, msgSize);
	_msgCache.push_back(nmb);
}

// guarantee at least 35FPS
FLOAT g_maxProcTime = 1.0f / 35.0f;

void CMessageHandler::TickMsgBuff(FLOAT deltaTime)
{
	SCOPE_CYCLE_COUNTER(MMOSTAT_Network_ProcMsgBuf);

	FScopeLock Scope(&_msgCacheLock);
	
	//const DOUBLE maxProcTime = 1.0 / 50.0;
	DOUBLE initTime = appSeconds();
	//DWORD initCycle = appCycles();

#if !SHIPPING_PC_GAME
	TArray<UINT16> msgIds;
	TArray<FLOAT> msgProcTime;
#endif

	while (!_msgCache.empty())
	{
		NetMsgCacheBuff newBuf = _msgCache.front();
		MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)newBuf.Buffer;
#if !SHIPPING_PC_GAME
		msgIds.AddItem(pHeader->wMsg);
		DOUBLE startMsgProcTime = appSeconds();
#endif
		__OnRecv(newBuf.Buffer, newBuf.Size, pHeader->wMsg, &m_MessageFuncList[0], this);

#if !SHIPPING_PC_GAME
		DOUBLE endMsgProcTime = appSeconds();
		msgProcTime.AddItem((FLOAT)(endMsgProcTime - startMsgProcTime));
#endif

		delete[] newBuf.Buffer;
		_msgCache.pop_front();

		DOUBLE usedTime = appSeconds() - initTime;
		if(!_fullSpeedProcNetMsg && ( usedTime > (DOUBLE)g_maxProcTime))
		{
			break;
		}
	}

#if !SHIPPING_PC_GAME
	FLOAT dt = (FLOAT)(appSeconds() - initTime);
	if(dt > g_maxProcTime)
	{
		SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
		warnf(NAME_Log, TEXT("== Tick Msg Buf:%f"), dt);
		for (INT i = 0; i < msgIds.Num(); ++i)
		{
			debugf(TEXT("	Msg:%d %s UseTime:%f"), msgIds(i), *m_MessageDebugString[msgIds(i)], msgProcTime(i));
		}
		CLEAR_WARN_COLOR();
	}
#endif
}
#endif

void CMessageHandler::DebugParseMsg(const char* buf, UINT32 dwSize)
{
	MESSAGE_HEADER* pHeader = (MESSAGE_HEADER*)buf;

#ifdef _DEBUG
	warnf(NAME_Log, TEXT("------------DebugParseMsg:%d"), pHeader->wMsg);
#endif

	LOG_PROCESS_ERROR(m_MessageFuncList[pHeader->wMsg]);
	(this->*m_MessageFuncList[pHeader->wMsg])(buf, dwSize);

Exit0:
	return;
}


void CMessageHandler::OnDisconnected(ISocketSession* pSocketSession, BOOL bRemoteShutdown)
{

	FString dbgStr = TEXT("N/A");
	if(pSocketSession && pSocketSession->GetUserData())
	{
		dbgStr = *(FString*)pSocketSession->GetUserData();

		warnf( NAME_Log, TEXT("=====####========== [CMessageHandler::OnDisconnected][CONNECTSTAT] %s m_nSockedFD[%x]"),
			*dbgStr, ((SimpleNet::CSocketSession*)pSocketSession)->m_nSockedFD );
	}

#if 0
	FString tmpDump = FString::Printf( TEXT("%sOnDisconnected-%d-%d-%d-%s.dmp"), *appGameLogDir(), MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER, *appSystemTimeString() );
	CreateCustomDump(*tmpDump);
#endif

#if !SHIPPING_PC_GAME && M_ENABLE_RPMGR
	if(g_replayMode)
	{
		return;
	}
#endif

#if M_ENABLE_ASYNC_PROC_CHAR
	ClearAllCharAppearList();
#endif

    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pSocketSession);
	
	if(m_pSocketSession == pSocketSession)
    {
        m_pSocketSession = NULL;
			}

     if(_connectState == NCS_ConnectingBS)
			{
		 CleanMsgSendRestrictList();
// 		if(pSocketSession == m_pSocketSession)
//         {
// 			m_pSocketSession = NULL;
// 		}
// 		else if(pSocketSession == m_pSocketSession2)
// 		{
// 			m_pSocketSession2 = NULL;
// 		}
// 
//         warnf( NAME_Log, TEXT("=============== [Network::OnDisconnected] NCS_ConnectingBS bRemoteShutdown:%d"), bRemoteShutdown );
// 
#if M_ENABLE_NET_MSG_BUFF
 		TickMsgBuff(0.0f);
#endif
// 
//         if(m_dwServerIP == INADDR_NONE || m_dwPort <= 0 )
//         {
//             warnf( NAME_Error, TEXT("=============== [Network::OnDisconnected] Failed to get the server ip:%d.%d.%d.%d:%d"),
// 				(BYTE)(m_dwServerIP & 0x000000ff),
// 				(BYTE)((m_dwServerIP & 0x0000ff00) >> 8),
// 				(BYTE)((m_dwServerIP & 0x00ff0000) >> 16),
// 				(BYTE)((m_dwServerIP & 0xff000000) >> 24),
// 				m_dwPort );
// 
//             CleanMsgSendRestrictList();
// 
// 			UBOOL bRet = FALSE;
// 			FString locStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Login.FailedGetServerIP"), bRet);
// #if SHIPPING_PC_GAME
// 			if(bRet)
// 			{
// 				MMOUI::GetUIManager()->delegateAddScreenMessage(locStr,MsgInfoType_Table,TEXT("MMOErrorInfo.Login.FailedGetServerIP"));
// 			}
// #else
// 			MMOUI::GetUIManager()->delegateAddScreenMessage(locStr,MsgInfoType_Table,TEXT("MMOErrorInfo.Login.FailedGetServerIP"));
// #endif
// 			//reconnect/relogin handling
// 			if(g_isSDOLogin)
// 			{
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
// 
// 				extern void ShowLoginDialog();
// 				ShowLoginDialog();
// 
// 				AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
// 				if(ctrl)
// 				{
// 					ctrl->GotoState(TEXT("LoginState"));
// 				}
// 			}
// 			else
// 			{
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
// 				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
// 			}
// 
// #if M_ENABLE_NET_MSG_BUFF
// 			//ClearNetMsgBuf();
// #endif
// 		}
// 
//         LOG_PROCESS_ERROR(m_dwServerIP != INADDR_NONE && m_dwPort > 0);
// 
//         if(m_pNet)
//         {
//             m_pSocketSessionGate = m_pNet->Connect(m_dwServerIP, m_dwPort);
//             _connectState = NCS_Connecting;
//         }
		}
//     else

	// only handle Gate disconnect
	if(m_pSocketSessionGate == pSocketSession)
    {
        m_pSocketSessionGate = NULL;
        warnf(TEXT("=========================== [Network::OnDisconnected] bRemoteShutdown:%d"), bRemoteShutdown);
         
        //clear Flag
        CleanMsgSendRestrictList();
		//MMO::ActiveGameState(EGameState_Login);

        //reconnect/relogin handling
		AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
		if(!g_disconnectByUser)
		{
			if(EGameState_Loading == MMO::GetCurrentGameState())
			{
				if(pc)
				{
					if (MMOFacade::getSingleton().IsReceivedGameData == FALSE)
					{
						pc->OnLeaveScene();
						pc->ReturnLogin();
					}
					else
					{
						pc->OnLeaveScene();
						pc->OnReturnLoginConfirm();
					}
				}
			}
			else{
				g_disableAllInput = true;
				FGFxEngine::GetEngine()->PlayerControlRequireReleaseEvt = FALSE;

				FScriptDelegate funYes(EC_EventParm);
				funYes.Object = pc;
				funYes.FunctionName = TEXT("OnReturnLoginConfirm");	

				int nModalNum = MMOMsgBoxManager::getSingleton().GetMsgNumByType(EMsgboxType_Modal);
				if(g_bOnReturnLoginConfirm==false || nModalNum==0)
				{
					UGFxObject* MsgboxWindow = MMOMsgBoxManager::getSingleton().AddModalMessageBox(
						TEXT("Disconnected from the server. Please log in again."), 0, funYes, funYes, FALSE);
				}
				g_bOnReturnLoginConfirm = false;
				g_disconnectByUser = false;
			}
		}
		else
		{
			if( pc )
			{
				pc->OnLeaveScene();
				pc->ReturnLogin();
			}
		}

#if M_ENABLE_NET_MSG_BUFF
		ClearNetMsgBuf();
#endif

		// OSCARTODO: move these code to delegate login movieplayer
		//FScriptDelegate returnLoginOK(EC_EventParm);
		//returnLoginOK.FunctionName = TEXT("ReturnLoginOk");
		//returnLoginOK.Object = hud->HudMovie;
		//MMOUI::GetUIManager()->delegateAddMessageBox( Localize(TEXT("MsgBox"), TEXT("text14"), TEXT("MMOGame") ), 0, returnLoginOK, returnLoginOK );
    }

    return;
Exit0:
    return;
}

CMessageHandler::CMessageHandler()
    : _lockPlayer(true)
    , _bPreLogin(false)
    , _connectState(NCS_Closed)
	, _tickDropAnimTime(0.0f)
	, C_ClearDropAnimCounterInterval(20.0f)
	, _isAdult(0)
	, m_byEncryptCode(FALSE)
	, m_dwClientOption(0)
	, _disableSend(false)
#if M_ENABLE_NET_MSG_BUFF
	, _fullSpeedProcNetMsg(false)
#endif
{

}

CMessageHandler::~CMessageHandler()
{
    UnInit();
}

void CMessageHandler::CleanMsgSendRestrictList()
{
    warnf( NAME_Log, TEXT("***** [CMessageHandler::UnLock all SendRestrict]") );

	for( int i = 0;i< m_vecMsgSendRestrictList.Num() ;++i )
		m_vecMsgSendRestrictList(i)=FALSE;
}

void TryParseLoginIP();

BOOL CMessageHandler::Init(void)
{
	SCOPE_LOG_TIME(___INIT___, TEXT("CMessageHandler::Init"), 0);

	TryParseLoginIP();

    INT32 nRetCode = 0;
    SIMPLE_INIT_PARAM InitParam;
    INT16* nMessageSizeList = NULL;

	bPendingToExit = FALSE;

    m_pPacketAllocator = new CPacketAllocator;
    if (m_pPacketAllocator == NULL)
        goto Exit0;

    InitParam.nMaxAcceptor = 1;
    InitParam.nMaxConnector = 1;
    InitParam.nMaxSessionCount = 4;
    InitParam.nPacketBufferSize = 2048;
    InitParam.nSendBufSize = 1024 * 16;
    InitParam.nRecvBufSize = 1024 * 64;

// #if !SHIPPING_PC_GAME
// 	nRetCode = m_pPacketAllocator->Init(1024*64, 128);
// #else
//     nRetCode = m_pPacketAllocator->Init(1024, 128);
// #endif
//     LOG_PROCESS_ERROR(nRetCode);

    m_pNet = new SimpleNet::CNet;
    if (m_pNet == NULL)
        goto Exit0;

    nRetCode = m_pNet->Init(m_pPacketAllocator, this, &InitParam);
    if (!nRetCode)
        goto Exit0;

    m_bConnectBS = false;
    m_dwServerIP = INADDR_NONE;
    m_dwPort = 0;
	m_byEncryptCode = FALSE;
	m_dwClientOption = 0;

	m_vecMsgSendRestrictList.Init( MsgRL_Num);
	CleanMsgSendRestrictList();
    MsgSendRestrictList.AddZeroed( MsgRL_Num );
    MsgSendRestrictList(MsgRL_Login) = TEXT("MsgRL_Login");
    MsgSendRestrictList(MsgRL_Create) = TEXT("MsgRL_Create");
    MsgSendRestrictList(MsgRL_Delete) = TEXT("MsgRL_Delete");
    MsgSendRestrictList(MsgRL_Select) = TEXT("MsgRL_Select");

	m_RoleDataCache.InitSize(500);
	m_NPCDataCache.InitSize(500);

	M_INIT_MSGHANDLER_LOGIN;
	M_INIT_MSGHANDLER_STALL;
	M_INIT_MSGHANDLER_ITEM;
	M_INIT_MSGHANDLER_TEAM;
	M_INIT_MSGHANDLER_CHAT;
	M_INIT_MSGHANDLER_FRIEND;
	M_INIT_MSGHANDLER_COMBAT;
	M_INIT_MSGHANDLER_DROP;
	M_INIT_MSGHANDLER_DOODAD;
	M_INIT_MSGHANDLER_QUEST;
	M_INIT_MSGHANDLER_SCENE;
	M_INIT_MSGHANDLER_NPC;
	M_INIT_MSGHANDLER_TRADE;
	M_INIT_MSGHANDLER_SHOP;
	M_INIT_MSGHANDLER_MAIL;
	M_INIT_MSGHANDLER_MASTER;
	M_INIT_MSGHANDLER_ACHIEVEMENT;
	//M_INIT_MSGHANDLER_BATTLETEAM;
	M_INIT_MSGHANDLER_GUILD;
	M_INIT_MSGHANDLER_PLAYERINFO;
	M_INIT_MSGHANDLER_ARENA;
	M_INIT_MSGHANDLER_SOUL;
	M_INIT_MSGHANDLER_WEATHER;
	//M_INIT_MSGHANDLER_UNDERGROUNDTRADE;
	M_INIT_MSGHANDLER_RANKLIST;
	M_INIT_MSGHANDLER_HEAVEN;
	M_INIT_MSGHANDLER_MALL;
	M_INIT_MSGHANDLER_ACTIVITY;
	M_INIT_MSGHANDLER_AUCTION;
	M_INIT_MSGHANDLER_TEAM_AUCTION;
	M_INIT_MSGHANDLER_KILLRECORD;
	M_INIT_MSGHANDLER_WEBPAGE;
	M_INIT_MSGHANDLER_GOLDSTORE;
	M_INIT_MSGHANDLER_EQUIPREMAKE;
	M_INIT_MSGHANDLER_JIGSAW;
    // Game Server Protocol
    //
	
	MREG_PARSE_FUN(g2c_ping_ack);
	MREG_PARSE_FUN(t2c_ping_ack);
    MREG_PARSE_FUN(g2c_sync_game_data_begin);
	MREG_PARSE_FUN(g2c_sync_data_error);
    MREG_PARSE_FUN(g2c_exp_change);
	MREG_PARSE_FUN(g2c_exercise_info);
	MREG_PARSE_FUN(g2c_get_exercise_exp);
	MREG_PARSE_FUN(g2c_add_exercise_time);
	MREG_PARSE_FUN(g2c_extra_exp_change);
    MREG_PARSE_FUN(g2c_role_level_up);
    MREG_PARSE_FUN(g2c_sync_game_data_end);

    MREG_PARSE_FUN(g2c_return_select_role);

    MREG_PARSE_FUN(g2c_error_notify);
//    MREG_PARSE_FUN(g2c_frame_signal);
	MREG_PARSE_FUN(g2c_gpk_dyn_code);
	MREG_PARSE_FUN(g2c_gpk_auth_data);
  MREG_PARSE_FUN(g2c_char_run);
  MREG_PARSE_FUN(g2c_char_walk);
    MREG_PARSE_FUN(g2c_char_passive_move);
	MREG_PARSE_FUN(g2c_char_stop);
    MREG_PARSE_FUN(g2c_object_rotate);
    MREG_PARSE_FUN(g2c_sync_container_sizes);
	//MREG_PARSE_FUN(g2c_refine_item);
	//MREG_PARSE_FUN(g2c_transfer_refine_item);
	MREG_PARSE_FUN(g2c_refine);
	MREG_PARSE_FUN(g2c_sync_refine);
	MREG_PARSE_FUN(g2c_char_mine);
    MREG_PARSE_FUN(g2c_char_appear);
	MREG_PARSE_FUN(g2c_sync_npc_transform_state)
    MREG_PARSE_FUN(g2c_sync_char_list);
    MREG_PARSE_FUN(g2c_sync_role_data);
    MREG_PARSE_FUN(g2c_sync_attr_list);
    MREG_PARSE_FUN(g2c_sync_attr_change);
    MREG_PARSE_FUN(g2c_sync_role_avatar);
	MREG_PARSE_FUN(g2c_sync_hero_avatar);
	MREG_PARSE_FUN(g2c_object_disappear);
	MREG_PARSE_FUN(g2c_object_disappear_list);
    MREG_PARSE_FUN(g2c_sync_role_pos);

	MREG_PARSE_FUN(m2c_sync_role_info);
	MREG_PARSE_FUN(m2c_queue_info);

	MREG_PARSE_FUN(g2c_sync_vip_state);
	MREG_PARSE_FUN(g2c_sync_vip_level);

	MREG_PARSE_FUN(m2c_free_mode);

    MREG_PARSE_FUN(g2c_sync_item_bag);
    MREG_PARSE_FUN(g2c_item_delete);
    MREG_PARSE_FUN(g2c_item_change);
    MREG_PARSE_FUN(g2c_item_swap);

	MREG_PARSE_FUN(g2c_sync_reputation);
	MREG_PARSE_FUN(g2c_sync_title);
	MREG_PARSE_FUN(g2c_add_title);
	MREG_PARSE_FUN(g2c_del_title);
	MREG_PARSE_FUN(g2c_sync_pk_value);
	MREG_PARSE_FUN(g2c_sync_pk_result);
	MREG_PARSE_FUN(g2c_sync_grey_name);
	MREG_PARSE_FUN(g2c_sync_equip_title);
	MREG_PARSE_FUN(g2c_modify_reputation);


	MREG_PARSE_FUN(g2c_sync_spell_pts);
    MREG_PARSE_FUN(g2c_sync_spell_book);

    MREG_PARSE_FUN(g2c_role_learn_spell);
	MREG_PARSE_FUN(g2c_role_remove_spell);

	MREG_PARSE_FUN(g2c_role_learn_item_spell);
	MREG_PARSE_FUN(g2c_role_remove_item_spell);
//    MREG_PARSE_FUN(g2c_self_hifreq_sync);
//    MREG_PARSE_FUN(g2c_region_hifreq_sync);

	//new health sync mechanism
	MREG_PARSE_FUN(g2c_sync_hp);
	MREG_PARSE_FUN(g2c_sync_mp);

    //money sync
    MREG_PARSE_FUN(g2c_sync_money);
    MREG_PARSE_FUN(g2c_money_change);

	MREG_PARSE_FUN(t2c_ping_ack);
	MREG_PARSE_FUN(m2c_error_notify);


	MREG_PARSE_FUN(g2c_clear_spell_cache);
	//mount
	MREG_PARSE_FUN(g2c_sync_mount_list);
	MREG_PARSE_FUN(g2c_activate_mount);
	MREG_PARSE_FUN(g2c_char_on_mount);
	MREG_PARSE_FUN(g2c_char_off_mount);
	MREG_PARSE_FUN(g2c_char_wing);
	MREG_PARSE_FUN(g2c_wing_info);

	//hero
	MREG_PARSE_FUN(g2c_sync_hero_data);
	MREG_PARSE_FUN(g2c_hero_exp_change);
	MREG_PARSE_FUN(g2c_hero_level_up);
	MREG_PARSE_FUN(g2c_sync_hero_attr_list);
	MREG_PARSE_FUN(g2c_hero_attr_change);
	MREG_PARSE_FUN(g2c_hire_hero);
	MREG_PARSE_FUN(g2c_fire_hero);
	MREG_PARSE_FUN(g2c_summon_hero);
	MREG_PARSE_FUN(g2c_change_hero_ai);
	MREG_PARSE_FUN(g2c_recall_hero);
	MREG_PARSE_FUN(g2c_decompose);

	MREG_PARSE_FUN(g2c_compose);

	MREG_PARSE_FUN(g2c_draw_debug_line);

	MREG_PARSE_FUN(m2c_fcm_state);

	//MREG_PARSE_FUN(m2c_h5_url);

    return TRUE;
Exit0:
    UnInit();
    return FALSE;
}

BOOL CMessageHandler::UnInit(void)
{
	M_FORCE_TRY_BEGIN;
	ClearCacheData();
	M_FORCE_TRY_END("Protected for ClearCacheData");

	M_FORCE_TRY_BEGIN;
    SAFE_DELETE(m_pNet);
	M_FORCE_TRY_END("Protected for delete m_pNet");

	M_FORCE_TRY_BEGIN;
	SAFE_DELETE(m_pPacketAllocator);
	M_FORCE_TRY_END("Protected for delete m_pPacketAllocator");

	bPendingToExit = TRUE;

    return TRUE;
}

CMessageHandler& CMessageHandler::GetInstance(void)
{
    return ms_Instance;
}

static UINT32 s_dwNetDelay = 0;
static UINT32 s_dwGameDelay = 0;

void CMessageHandler::Tick(float fTime)
{
	M_FORCE_TRY_BEGIN;

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		static UINT32 s_dwLastPingTick = 0;
		UINT32 dwCurrentTick = GetTickCount();

#if STATS
		static UINT32 s_dwLastFlushTick = 0;
		static UINT32 s_dwUploadSpeed = 0;
		static UINT32 s_dwDownloadSpeed = 0;


		if (s_dwLastFlushTick / 10000 < dwCurrentTick / 10000)
		{
			s_dwUploadSpeed = dwUploadSpeed;
			s_dwDownloadSpeed = dwDownloadSpeed;
			dwUploadSpeed = 0;
			dwDownloadSpeed = 0;

			s_dwLastFlushTick = dwCurrentTick;
		}
#endif

		if (dwCurrentTick - s_dwLastPingTick > 4000)
		{
			Send_GatePing();
			Send_GamePing();

			s_dwLastPingTick = dwCurrentTick;
		}


		TickDropAnim(fTime);

		SET_FLOAT_STAT(MMOSTAT_Network_Upload, s_dwUploadSpeed / 10.0f);
		SET_FLOAT_STAT(MMOSTAT_Network_Download, s_dwDownloadSpeed / 10.0f);
		SET_DWORD_STAT(MMOSTAT_Network_NetDelay, s_dwNetDelay);
		SET_DWORD_STAT(MMOSTAT_Network_GameDelay, s_dwGameDelay);	
	}

	if (bPendingToExit)
	{
		return;
	}

	//static float s_fX, s_fY;
	//float fX = MMOFacade::getSingleton().GetMainActor()->Location.X;
	//float fY = MMOFacade::getSingleton().GetMainActor()->Location.Y;
	//const tm_ex* pNow = localtime_ex();
	//debugf(L"%02d:%02d:%02d.%03d (%.2f, %.2f), step %.2f", pNow->tm_hour, pNow->tm_min, pNow->tm_sec, pNow->tm_msec,
	//	fX, fY, sqrt((fX - s_fX) * (fX - s_fX) + (fY - s_fY) * (fY - s_fY)));
	//s_fX = fX;
	//s_fY = fY;

	//FLOAT timer = appSeconds();

#if M_ENABLE_NET_MSG_BUFF
	TickMsgBuff(fTime);
#endif

	m_pNet->MainLoop();

	//timer = appSeconds() - timer;
	//if( timer > 1.0 )
	//{
	//	timer = appSeconds();
	//}

	//while (m_PacketCache.size() > 0 && (m_PacketCache.front().nServerFrame < MMOFacade::getSingleton().GetClientFrameCount() || _lockPlayer))
	//{
	//	_OnRecv(m_pSocketSession, m_PacketCache.front().pPacketBuffer, m_PacketCache.front().dwSize);
	//	m_PacketCache.pop_front();
	//}

	M_FORCE_TRY_END("Protected for CMessageHandler::Tick");

}
//--------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_return_select_role(const char* buf, UINT32 size)
{
	G2C_RETURN_SELECT_ROLE* msg = (G2C_RETURN_SELECT_ROLE*)buf;

	//will definitely enter EGameState_Selecting; ESET_CreateListRoleCheck is used here to ensure RT is refreshed on relogin in all cases
	MMOGameEvent_Splash loginEvt;
	loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateListRoleCheck;
	MMOUI::PostGameEvent(&loginEvt);

	//MMO::ActiveGameState(EGameState_Selecting);
}

// update client the current frame count
//void CMessageHandler::Parse_g2c_frame_signal(const char* buf, UINT32 size)
//{
//	G2C_FRAME_SIGNAL* msg = (G2C_FRAME_SIGNAL*)buf;
//	MMOFacade::getSingleton().AddServerFrameCount();
//}
extern bool ShouldSkipGPK();

void CMessageHandler::Parse_g2c_gpk_auth_data(const char* buf, UINT32 size)
{
#if USING_GPK

#if !SHIPPING_PC_GAME
	if(ShouldSkipGPK())
	{
		return;
	}
#endif

	G2C_GPK_AUTH_DATA* msg = (G2C_GPK_AUTH_DATA*)buf;
	INT DataSize = msg->wSize - sizeof(FLEX_MESSAGE_HEADER);

	char* NewData = new char[msg->wSize];
	memcpy(NewData, buf, msg->wSize);
	C2G_GPK_AUTH_DATA* NewMsg = (C2G_GPK_AUTH_DATA*)NewData;
	NewMsg->wMsg = c2g_gpk_auth_data;
	GCltDynCode->ProcessAuth(NULL,NULL,(unsigned char *)(NewMsg->AuthData), DataSize);
	_Send(NewMsg, NewMsg->wSize);

	delete NewData;
#endif
}


void CMessageHandler::Parse_g2c_gpk_dyn_code(const char* buf, UINT32 size)
{
#if USING_GPK
#if !SHIPPING_PC_GAME
	if(ShouldSkipGPK())
	{
		return;
	}
#endif

	G2C_GPK_DYN_CODE* msg = (G2C_GPK_DYN_CODE*)buf;
	INT DataSize = msg->wSize - sizeof(FLEX_MESSAGE_HEADER);

	GCltDynCode->SetDynCode((const unsigned char*)(msg->DynCode), DataSize);
#endif
}

extern bool g_moveImm;


void CMessageHandler::Parse_g2c_char_walk(const char* buf, UINT32 size)
{
	G2C_CHAR_WALK* msg = (G2C_CHAR_WALK*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (!actor)
	{
		warnf(L"can not find actor whose id is %d", msg->dwCharID);
		return;
	}

	FVector pos(msg->wDestX,msg->wDestY,0);
	ConvertCoordN2C(pos);
	pos.Z = actor->Location.Z;
	pos.Z = actor->GetValidZValue(pos);
	pos.Z += actor->GetCollisionHeight()+64.0f;

	if (actor->Physics != PHYS_Walking)
	{
		actor->setPhysics(PHYS_Walking);
	}

	FVector OldPos = actor->Location;	// zhaojun@snda
	FVector DisDir = pos-actor->Location;
	DisDir.Z = 0.0;
	if(DisDir.SizeSquared2D()>0.000000001f)
	{
		FRotator NewRotation = DisDir.Rotation();
		actor->RotatoTo(NewRotation, 0);
	}
	else
	{
		return;
	}

	FVector dis = pos-actor->Location;
	FLOAT L = dis.Size2D();
	FLOAT P = abs(dis.X)+abs(dis.Y);
	UBOOL bAxial = (P/L>1.2);	//axial

	FLOAT MoveTimes = msg->wSpeed>0 ? ConvertToClientTime(msg->wSpeed) : 0.5f;
	FLOAT Speed		= bAxial ? (128.0f/MoveTimes) : (128.0f*1.414213562373095f/MoveTimes);
	FLOAT SpeedEx	= L/MoveTimes;
	FLOAT inMoveRateScale = 0.5/MoveTimes;
	
	if(actor->IsA(ANPC::StaticClass()))
	{
		//get the original animation duration
		FLOAT MoveTimesDefault = actor->GetAnimSeqLength(actor->PawnAnimInfo.WalkAnim);
		MoveTimesDefault = (MoveTimesDefault>0.01f&&MoveTimesDefault<100.0f)?MoveTimesDefault:0.5f;
		inMoveRateScale=  MoveTimesDefault/MoveTimes;		//compute animation play rate, default=1.0f
		if(inMoveRateScale<1.0f)//does not slow down speed, instead uses default speed plus pauses
		{
			MoveTimes = MoveTimesDefault;
			Speed		= bAxial ? (128.0f/MoveTimes) : (128.0f*1.414213562373095f/MoveTimes);
			SpeedEx		= L/MoveTimes;
			inMoveRateScale = 1.0f;
		}
		actor->SetMoveType(bAxial?EMT_Walk:EMT_Walk);

#if SHOW_MOVEMENT_LOG
		debugf(TEXT("%d Parse_g2c_char_walk %f, %f, %f"), msg->dwCharID, pos.X, pos.Y, pos.Z);
#endif
	}
	else
	{
		actor->SetMoveType(bAxial?EMT_Walk:EMT_Walk_Oblique);
	}

	//smart reset of animation phase: after standing for a while, when restarting the phase starts from the initial value
	if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f)
	{
		actor->SetMoveAnimPosition(bAxial?g_Walk_Phase[actor->GetClassId()]:g_Walk_Oblique_Phase[actor->GetClassId()]);	
	}
	actor->ResetMovePhaseTime = MoveTimes+0.5f;

	//decide whether to quickly compensate to the target point or walk there normally
	Speed = (L>128.0f*1.9f)? SpeedEx : Speed;
	actor->eventScriptMoveTo(pos, Speed,inMoveRateScale);
	actor->SetServerLocation(pos);	// zhaojun@snda


	if(actor->IsMainPlayer())
	{
		g_PreMovetoPoint = TRUE;
		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		if(ctrl)
		{
			ctrl->WaitingForTheMoveToRespond = -1.0f;
		}
		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
	}
}

void CMessageHandler::Parse_g2c_char_run(const char* buf, UINT32 size)
{
	G2C_CHAR_RUN* msg = (G2C_CHAR_RUN*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (!actor)
	{
		warnf(L"can not find actor whose id is %d", msg->dwCharID);
		return;
	}

	FVector pos(msg->wDestX,msg->wDestY,0);
	ConvertCoordN2C(pos);
	pos.Z = actor->Location.Z;
	pos.Z = actor->GetValidZValue(pos);
	pos.Z += actor->GetCollisionHeight()+64.0f;

	if (actor->Physics != PHYS_Walking)
	{
		actor->setPhysics(PHYS_Walking);
	}

	FVector OldPos = actor->Location;	// zhaojun@snda
	FVector DisDir = pos-actor->Location;
	DisDir.Z = 0.0;
	if(DisDir.SizeSquared2D()>0.000000001f)
	{
		FRotator NewRotation = DisDir.Rotation();
		actor->RotatoTo(NewRotation, 0);
	}
	else
	{
		return;
	}

	FVector dis = pos-actor->Location;
	FLOAT L = dis.Size2D();
	FLOAT P = abs(dis.X)+abs(dis.Y);
	UBOOL bAxial = (P/L>1.2);	//axial

	FLOAT MoveTimes = msg->wSpeed>0 ? ConvertToClientTime(msg->wSpeed) : 0.75f;
	FLOAT Speed		= bAxial ? (256.0f/MoveTimes) : (256.0f*1.414213562373095f/MoveTimes);
	FLOAT SpeedEx	= L/MoveTimes;
	FLOAT inMoveRateScale = 0.75/MoveTimes;

	if(actor->IsA(ANPC::StaticClass()))
	{
		//get the original animation duration
		FLOAT MoveTimesDefault = actor->GetAnimSeqLength(actor->PawnAnimInfo.RunAnim);
		MoveTimesDefault = (MoveTimesDefault>0.01f&&MoveTimesDefault<100.0f)?MoveTimesDefault:0.75f;
		inMoveRateScale=  MoveTimesDefault/MoveTimes;		//compute animation play rate, default=1.0f

		if(inMoveRateScale<1.0f)//does not slow down speed, instead uses default speed plus pauses
		{
			MoveTimes = MoveTimesDefault;
			Speed		= bAxial ? (128.0f/MoveTimes) : (128.0f*1.414213562373095f/MoveTimes);
			SpeedEx		= L/MoveTimes;
			inMoveRateScale = 1.0f;
		}

		actor->SetMoveType(bAxial?EMT_Run:EMT_Run);
	}
	else
	{
		actor->SetMoveType(bAxial?EMT_Run:EMT_Run_Oblique);
	}

	//smart reset of animation phase: after standing for a while, when restarting the phase starts from the initial value
	if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f)
	{
		actor->SetMoveAnimPosition(bAxial?g_Run_Phase[actor->GetClassId()]:g_Run_Oblique_Phase[actor->GetClassId()]);	
	}
	actor->ResetMovePhaseTime = MoveTimes+0.5f;
	//decide whether to quickly compensate to the target point or walk there normally
	Speed = (L>375.0f)? SpeedEx : Speed;
	actor->eventScriptMoveTo(pos, Speed,inMoveRateScale);
	actor->SetServerLocation(pos);	// zhaojun@snda


	if(actor->IsMainPlayer())
	{
		g_PreMovetoPoint = TRUE;
		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		if(ctrl)
		{
			ctrl->WaitingForTheMoveToRespond = -1.0f;
		}
		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
	}
}

void CMessageHandler::Parse_g2c_char_mine(const char* buf, UINT32 size)
{
	G2C_CHAR_MINE* msg = (G2C_CHAR_MINE*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (!actor)
	{
		warnf(L"can not find actor whose id is %d", msg->dwCharID);
		return;
	}

	FVector pos(msg->wDestX,msg->wDestY,0);
	ConvertCoordN2C(pos);
	pos.Z = actor->Location.Z;
	pos.Z = actor->GetValidZValue(pos);
	pos.Z += actor->GetCollisionHeight()+64.0f;
	if (actor->Physics != PHYS_Walking)
	{
		actor->setPhysics(PHYS_Walking);
	}

	if(actor->IsMainPlayer())
	{
		Mir2Attack::getSingleton().m_TryToMine = TRUE;
		Mir2Attack::getSingleton().m_TryToMinePos = pos;
	}

	FVector OldPos = actor->Location;	// zhaojun@snda
	FVector DisDir = pos-actor->Location;
	DisDir.Z = 0.0;
	FRotator NewRotation = DisDir.Rotation();
	actor->RotatoTo(NewRotation, 0);

	
	//ZONE_DATA* pBufZone = NULL;
	//GRID_DATA* pBufGrid = NULL;
	//UBOOL bRes= gServerGridWorld.GetZoneAndGrid(actor->ServerLocation, pBufZone, pBufGrid);
	INT MineLevel = 1;
	//if(bRes && pBufGrid)
	//{
	//	MineLevel = pBufGrid->uMineLevel;
	//}

	//on some servers a special hoe is used to mine gold ore
	UEquip* WeaponEquip = actor->GetWeaponEquip();
	if (WeaponEquip)
	{
		INT TempID = WeaponEquip->GetTemplateID();
		if (TempID == 99900042 || TempID == 99900043)
		{
			MineLevel = 2;
		}
	}

	//ItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem(E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND);
	//if (pItemInfo)
	//{
	//	if (pItemInfo->m_nTemplateID == 99900042 || pItemInfo->m_nTemplateID == 99900043)
	//	{
	//		MineLevel = 2;
	//	}
	//}


	UMMOAbility* pMineAbility = Mir2Attack::getSingleton().GetMineAbility(MineLevel);
	if(pMineAbility)
	{
		if(actor->IsMainPlayer())
		{
			//warnf( NAME_Log, TEXT("----------------------------self performs mining ability!"));
			FLOAT DelayTime = FLOAT(msg->wMineInterval)/1000.0f;
			FSMIO::getSingleton().CDTime.ResetCD_MoveDelay(DelayTime);
			pMineAbility->SpellCommonAttribute.HardTime = DelayTime;
			MMOFacade::getSingleton().GetAbilityProcessor()->PlayClientAbility(msg->dwCharID, pMineAbility);
			FSMIO::getSingleton().CDTime.ResetCD_WalkAndRunDelay(pMineAbility->SpellCommonAttribute.WalkDelayTime,pMineAbility->SpellCommonAttribute.RunDelayTime);
		}
		else
		{
			//warnf( NAME_Log, TEXT("----------------------------%s performs mining ability!"), *(actor->ActorName));
			FLOAT DelayTime = FLOAT(msg->wMineInterval)/1000.0f;
			pMineAbility->SpellCommonAttribute.HardTime = DelayTime;
			MMOFacade::getSingleton().GetAbilityProcessor()->PlayClientAbility(msg->dwCharID, pMineAbility);
		}
	}
}


//
//void CMessageHandler::Parse_g2c_char_walk(const char* buf, UINT32 size)
//{
//	G2C_CHAR_WALK* msg = (G2C_CHAR_WALK*)buf;
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
//	if (!actor)
//	{
//		warnf(L"can not find actor whose id is %d", msg->dwCharID);
//		return;
//	}
//
//	FVector pos(msg->wDestX,msg->wDestY,0);
//	ConvertCoordN2C(pos);
//	pos.Z = actor->Location.Z;
//	pos.Z = actor->GetValidZValue(pos);
//	pos.Z += actor->GetCollisionHeight()+64.0f;
//
//	if (actor->Physics != PHYS_Walking)
//	{
//		actor->setPhysics(PHYS_Walking);
//	}
//
//	//warnf(TEXT("-----------------------Parse_g2c_char_walk:%f,%f.%f"), pos.X,pos.Y,pos.Z);
//
//	FVector OldPos = actor->Location;	// zhaojun@snda
//	FVector Des = pos-actor->Location;
//
//	//check(OldPos == actor->ServerLocation);
//
//	Des.Z = 0.0;
//	FRotator NewRotation = Des.Rotation();
//	//NewRotation.Yaw = ConvertToClientDegree(msg->wFaceDirection);
//	actor->RotatoTo(NewRotation, 0);
//
//	FVector dis = pos-actor->Location;
//	FLOAT L = dis.Size2D();
//	FLOAT P = abs(dis.X)+abs(dis.Y); //GRID_WIDTH / POINT_PER_CM;
//	FLOAT speedConfig = 256.0f;
//	FLOAT speed = speedConfig;
//	FLOAT speedOptimize = speed;
//	FLOAT WalkTimes = 0.5f;
//	//if(actor->IsMainPlayer())
//	//{
//	//	INT WalkFrames = actor->ActorAttributes->GetAttribute(catWalkSpeed);
//	//	if(WalkFrames==0)WalkFrames=8;
//	//	WalkTimes = ConvertToClientTime(WalkFrames);
//	//	speedConfig = 128.0f/WalkTimes;
//	//}
//	
//	FLOAT inMoveRateScale = g_Walk_Oblique_AniRate[actor->GetClassId()];
//
//	if(actor->IsA(ANPC::StaticClass()))
//	{
//		//get the original animation duration
//		FLOAT time = actor->GetAnimSeqLength(actor->PawnAnimInfo.WalkAnim);
//		if(time<0.1f||time>10000.0f){
//			time=0.1f;//protection
//		}
//		//get the duration by a special formula; if there is no special formula, return the original animation duration
//		FLOAT optimizeTime = Mir2Attack::getSingleton().GetNpcMoveSpeedTime(actor,FALSE,time);
//		
//
//		if(P/L>1.2)//axial
//		{
//			speed = 128.0f/optimizeTime;				//compute speed
//			speedOptimize = L/optimizeTime;
//			inMoveRateScale=  time/optimizeTime;		//compute animation play rate, default=1.0f
//			actor->SetMoveType(EMT_Walk);
//
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f)	//animation reset check
//			{
//				actor->ResetMovePhaseTime = time+0.3f;
//				actor->SetMoveAnimPosition(0.0f);	
//			}
//		}
//		else	//diagonal
//		{
//			speed = 128.0f/optimizeTime*1.414213562373095f;	
//			speedOptimize = L/optimizeTime;
//			inMoveRateScale= time/optimizeTime*1.414213562373095f;
//			actor->SetMoveType(EMT_Walk);
//
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f)
//			{
//				actor->ResetMovePhaseTime = time+0.3f;
//				actor->SetMoveAnimPosition(0.0f);
///*				actor->ResetMovePhaseTime = time*1.414213562373095f+0.5f;
//				actor->SetMoveAnimPosition(0.0f);*/	
//			}
//		}
//	}
//	else{
//
//		INT WalkFrames = actor->ActorAttributes->GetAttribute(catWalkSpeed);
//		if(WalkFrames==0)WalkFrames=8;
//		WalkTimes = ConvertToClientTime(WalkFrames);
//		speedConfig = 128.0f/WalkTimes;
//
//		if(P/L>1.2 && g_UniformSpeed==FALSE)
//		{	
//			speedOptimize = speed = speedConfig;//256.0f;
//			actor->SetMoveType(EMT_Walk);
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f){
//				actor->ResetMovePhaseTime = WalkTimes+0.5f;
//				inMoveRateScale =  0.5f/WalkTimes;
//				actor->SetMoveAnimPosition(g_Walk_Phase[actor->GetClassId()]);	
//			}
//		}
//		else
//		{
//			speedOptimize = speed = speedConfig*1.414213562373095f;//362.0386719675f;
//			actor->SetMoveType(EMT_Walk_Oblique);
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f){
//				actor->ResetMovePhaseTime = WalkTimes+0.5f;
//				inMoveRateScale = 0.5f/WalkTimes;
//				actor->SetMoveAnimPosition(g_Walk_Oblique_Phase[actor->GetClassId()]);	
//			}
//		}
//	}
//
//	if(actor->IsMainPlayer())
//	{
//		g_PreMovetoPoint = TRUE;
//		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//		if(ctrl)
//		{
//			ctrl->WaitingForTheMoveToRespond = -1.0f;
//		}
//
//		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
//		actor->eventScriptMoveTo(pos, speed,inMoveRateScale);
//	}
//	else
//	{
//		if((pos-actor->Location).Size2D()>256.0)
//		{
//			actor->eventScriptMoveTo(pos, speedOptimize,inMoveRateScale);
//		}
//		else
//		{
//			actor->eventScriptMoveTo(pos, speed,inMoveRateScale);
//		}
//	}
//	actor->SetServerLocation(pos);	// zhaojun@snda
//}

//void CMessageHandler::Parse_g2c_char_run(const char* buf, UINT32 size)
//{
//	G2C_CHAR_RUN* msg = (G2C_CHAR_RUN*)buf;
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
//	if (!actor)
//	{
//		warnf(L"can not find actor whose id is %d", msg->dwCharID);
//		return;
//	}
//
//	FVector pos(msg->wDestX,msg->wDestY,0);
//	ConvertCoordN2C(pos);
//	pos.Z = actor->GetValidZValue(pos);
//	pos.Z += actor->GetCollisionHeight();
//
//	if (actor->Physics != PHYS_Walking)
//	{
//		actor->setPhysics(PHYS_Walking);
//	}
//
//
//	FVector Des = pos-actor->Location;
//	Des.Z = 0.0;
//	FRotator NewRotation = Des.Rotation();
//	//NewRotation.Yaw = ConvertToClientDegree(msg->wFaceDirection);
//	actor->RotatoTo(NewRotation, 0);
//
//	FVector OldPos = actor->Location;
//
//	//check(OldPos == actor->ServerLocation);
//
//	FVector dis = pos-actor->Location;
//	FLOAT L = dis.Size2D();
//	FLOAT P = abs(dis.X)+abs(dis.Y); //GRID_WIDTH / POINT_PER_CM*2;
//	FLOAT speedConfig = 341.333333333f;
//	FLOAT speed = speedConfig;//341.0*1.414;//482.0;//341.0;
//	FLOAT speedOptimize = speed;
//	FLOAT RunTimes = FLOAT(msg->wSpeed)/1000.0f;//  0.75f;
//	//if(actor->IsMainPlayer()){
//	//	INT RunFrames = actor->ActorAttributes->GetAttribute(catRunSpeed);
//	//	if(RunFrames==0)RunFrames=12;
//	//	RunTimes = ConvertToClientTime(RunFrames);
//	//	speedConfig = 256.0f/RunTimes;
//	//}
//
//	FLOAT inMoveRateScale = g_Run_Oblique_AniRate[actor->GetClassId()];
//	if(actor->IsA(ANPC::StaticClass()))
//	{
//		FLOAT time = actor->GetAnimSeqLength(actor->PawnAnimInfo.RunAnim);
//		if(time<0.1f || time>10000.0f){
//			time=0.1f;//protection
//		}
//		//get the duration by a special formula; if there is no special formula, return the original animation duration
//		FLOAT optimizeTime = Mir2Attack::getSingleton().GetNpcMoveSpeedTime(actor,TRUE,time);
//
//
//		if(P/L>1.2){
//			speed = 256.0f/optimizeTime;	//speedConfig;
//			speedOptimize = L/optimizeTime;
//			inMoveRateScale=  time/optimizeTime;		//compute animation play rate, default=1.0f
//			actor->SetMoveType(EMT_Run);
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f){
//				actor->ResetMovePhaseTime = time+0.3f;// 0.75+0.5f;
//				actor->SetMoveAnimPosition(0.0f);	
//			}
//		}
//		else
//		{
//			speed = 256.0f/optimizeTime*1.414213562373095f;
//			speedOptimize = L/optimizeTime;
//			inMoveRateScale=  time/optimizeTime*1.414213562373095f;		//compute animation play rate, default=1.0f
//			actor->SetMoveType(EMT_Run);
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f){
//				actor->ResetMovePhaseTime = time+0.3f;// 0.75+0.5f;
//				actor->SetMoveAnimPosition(0.0f);	
//			}
//		}
//	}
//	else{
//		//INT RunFrames = actor->ActorAttributes->GetAttribute(catRunSpeed);
//		//if(RunFrames==0)RunFrames=12;
//		//RunTimes = ConvertToClientTime(RunFrames);
//		speedConfig = 256.0f/wSpeed;// 256.0f/RunTimes;
//
//		if(P/L>1.2)
//		{	
//			speedOptimize = speed = speedConfig;
//			actor->SetMoveType(EMT_Run);
//
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f){
//				actor->ResetMovePhaseTime = 0.75+0.5f;
//				inMoveRateScale = 0.75f/RunTimes;
//				actor->SetMoveAnimPosition(g_Run_Phase[actor->GetClassId()]);	
//			}
//		}
//		else
//		{
//			speedOptimize = speed = speedConfig*1.414213562373095f;
//			inMoveRateScale = g_Run_Oblique_AniRate[actor->GetClassId()];
//			actor->SetMoveType(EMT_Run_Oblique);
//
//			if(actor->isMoving==FALSE && actor->ResetMovePhaseTime<0.0f){
//				actor->ResetMovePhaseTime = 0.75+0.5f;
//				inMoveRateScale = 0.75f/RunTimes;
//				actor->SetMoveAnimPosition(g_Run_Oblique_Phase[actor->GetClassId()]);	
//			}
//		}
//	}
//	
//	if(actor->IsMainPlayer())
//	{
//		g_PreMovetoPoint = TRUE;
//		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//		if(ctrl){
//			ctrl->WaitingForTheMoveToRespond = -1.0f;
//		}
//
//		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
//		actor->eventScriptMoveTo(pos, speed,inMoveRateScale);
//	}
//	else
//	{
//		if((pos-actor->Location).Size2D()>512.0)
//		{
//			actor->eventScriptMoveTo(pos, speedOptimize,inMoveRateScale);
//		}
//		else{
//			actor->eventScriptMoveTo(pos, speed,inMoveRateScale);
//		}
//	}
//
//	actor->SetServerLocation(pos);	//zhaojun@snda
//}


//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------





//////////////////////////////////////////////////////////////////////////
/*
typedef struct tagG2C_CHAR_DASH{
INT32            nFrame;
UINT32           dwDashSpeed;
INT32            nSrcX;
INT32            nSrcY;
INT32            nSrcZ;
INT32            nDestX;
INT32            nDestY;
INT32            nDestZ;
}G2C_CHAR_DASH;
*/

void CMessageHandler::Parse_g2c_char_passive_move(const char* buf, UINT32 size)
{
	G2C_CHAR_PASSIVE_MOVE* msg = (G2C_CHAR_PASSIVE_MOVE*)buf;
	FVector Des(msg->wDestX, msg->wDestY, 0);
	ConvertCoordN2C(Des);

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (!actor)
	{
		return;
	}

	if (actor->IsMainPlayer())
	{
		actor->GetMainController()->StopMove();
	}
	else
	{
		actor->StopMovement();
	}

	INT FrameCount = MMOFacade::getSingleton().GetServerFrameCount();
	if (actor->IsMainPlayer())
	{
		FrameCount = MMOFacade::getSingleton().GetClientFrameCount();
	}

	FLOAT speed = 0.f;
	FLOAT Duration = (msg->wDuration+1) * 1.0f / GAME_FPS;//add one so that there is no gap between consecutive server passive displacements
	//debugf(L"%%%%%%%%%%%%%%%%%%%%%%% msg->wDuration=%d", msg->wDuration);
	//Duration -= FLOAT(FrameCount - msg->nStartFrame) / FLOAT(GAME_FPS);

	//debugf(L"nDuration2=%d  ClientFrameCount=%d  nStartFrame=%d  Duration=%f", msg->llDuration2, FrameCount, msg->nStartFrame);

	if (Duration <= 0.f)
	{
		Duration = 0.001f;
	}

	// fix des
	Des.Z = actor->Location.Z;
	FVector Start = Des + FVector(0,0, 780);
	FVector End = Des + FVector(0, 0, -1500);

	AActor* HitActor = NULL;
	FVector HitLocation = actor->Location;
	// Get list of hit actors.
	FMemMark Mark(GMainThreadMemStack);
	FCheckResult* HitRet = GWorld->MultiLineCheck
		(
		GMainThreadMemStack,
		End,
		Start,
		FVector(0, 0, 0),
		TRACE_Blocking | TRACE_LevelGeometry | TRACE_Level | TRACE_Terrain,
		actor,
		NULL
		);

	while(HitRet)
	{
		if (HitRet->Actor)
		{
			AStaticMeshActor* sma = Cast<AStaticMeshActor>(HitRet->Actor);
			if (sma && (sma->bCanHidden || sma->bBlockActors == FALSE || sma->CollisionType != COLLIDE_BlockAll))
			{
				HitRet = HitRet->GetNext();
			}
			else
			{
				HitActor = HitRet->Actor;
				HitLocation = HitRet->Location;
				break;
			}
		}
		else
		{
			break;
		}
	}

	Mark.Pop();

	if (HitActor)
	{
		Des.Z = HitLocation.Z + actor->GetCollisionHeight();
	}

	FRotator Rot(0, 0, 0);
	Rot.Yaw = ConvertToClientDegree(msg->wFaceDirection);
	//debugf(L"PassiveMove");
	actor->SetDesiredRotation(Rot, FALSE, TRUE, -1.f, TRUE);

#if SHOW_MOVEMENT_LOG
	FRotator Dir = (Des - actor->Location).Rotation().GetDenormalized();
	debugf(TEXT("Passive move Des=%f, %f, %f Yaw=%d, Duration=%f"), Des.X, Des.Y, Des.Z, Dir.Yaw, Duration);
#endif
	
	//warnf(TEXT("------------------------Passive move Des:x=%f,y=%f"),Des.X, Des.Y);

	if (msg->byMoveState == CMS_THROWN)
	{
		actor->SetThrowTo(Des, Duration);
	}
	else if (msg->byMoveState == CMS_FEAR)
	{
		actor->FearTo(Des, Duration);
		//FRotator NewRotation = (Des - actor->Location).Rotation();
		//NewRotation.Pitch = 0;
		//NewRotation.Roll = 0;

		//actor->LockDesiredRotation(FALSE, TRUE);
		//actor->SetDesiredRotation(NewRotation, FALSE, TRUE, -1.f, TRUE);

		//actor->eventScriptMoveTo(Des, (Des - actor->Location).Size2D() / Duration);
	}
	else
	{
		//debugf(TEXT("==================================DashTo Dis = %f, Time = %f"), (Des-actor->Location).Size2D(), Duration );
		actor->DashTo(Des, Duration);
	}

	actor->SetServerLocation(Des);	// zhaojun@snda
	//actor->ServerLocation = Des;
}

void CMessageHandler::Parse_g2c_char_stop(const char* buf, UINT32 size)
{
	G2C_CHAR_STOP* msg = (G2C_CHAR_STOP*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if( actor == NULL )
		return;

	UBOOL IsMainPlayer = actor->IsMainPlayer();
	
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController* controller = MMOFacade::getSingleton().GetMainController();

	if( IsMainPlayer && ( player == NULL || controller == NULL ) )
	{
		warnf( TEXT("Main player & controller not properly set in Facade!" ) );
		return;
	}

	FVector pos(msg->wCurrentX,msg->wCurrentY,msg->wCurrentZ);
	ConvertCoordN2C(pos);

	// do not process if in a cutscene
	if( IsMainPlayer && player->IsInMatinee )
	{
		actor->SetServerLocation(pos);	//zhaojun@snda
		return;
	}

	if (IsMainPlayer)
	{
#ifdef DEBUG_SYNC_INFO
		FVector dis = pos-actor->Location;
		FLOAT DelDis = 64.0f;
		MMOFacade::getSingleton().GetMainController()->DrawDebugSphere(pos, 16, 16, 255, 0, 0, TRUE);
#endif

		if(msg->byForce)
		{
#ifdef DEBUG_SYNC_INFO
			if( dis.Size2D() > DelDis)
				warnf( TEXT("Forced stop, difference greater than %f----client: X%f Y%f Z%f  server: X%f Y%f Z%f ------------------------------------"),DelDis,actor->Location.X,actor->Location.Y,actor->Location.Z,pos.X,pos.Y,pos.Z );
#endif
			//MMOTargetManager::getSingleton().Clear();
			//FSMIO::getSingleton().State.ClearAllState();
			MMOFacade::getSingleton().GetMainController()->ResetMoving();
		}
		else
		{
#ifdef DEBUG_SYNC_INFO
			if( dis.Size2D() > DelDis)
				warnf( TEXT("Natural stop, difference greater than %f----client: X%f Y%f Z%f  server: X%f Y%f Z%f ------------------------------------"),DelDis,actor->Location.X,actor->Location.Y,actor->Location.Z,pos.X,pos.Y,pos.Z );
#endif
			MMOFacade::getSingleton().GetMainController()->ResetMoving();
		}
	}

	if( IsMainPlayer && controller->OnRevive )
	{	// should not set player position during revive
		controller->StopMove();
		controller->OnRevive = FALSE;
	}
	else
	{
		FVector Point(msg->wCurrentX, msg->wCurrentY, msg->wCurrentZ);
		ConvertCoordN2C(Point);

		//stopped at the same spot, no processing
		if((actor->ServerLocation-Point).Size2D()<64.0 && (actor->Location-Point).Size2D()<64.0)
		{
			return;
		}

		//warnf(TEXT("-----------------------Parse_g2c_char_stop:%f,%f.%f"), Point.X,Point.Y,Point.Z);

#if SHOW_MOVEMENT_LOG
		debugf(TEXT("%d StopMove %f, %f, %f"), msg->dwCharID, Point.X, Point.Y, Point.Z);
#endif
		//if ((Point - actor->Location).SizeSquared2D() >= FLOAT(g_moveModGridSize * g_moveModGridSize * g_netLagToleranceDisatnce * g_netLagToleranceDisatnce)) // pull only if over 2 grids of 128
//		if ((Point - actor->Location).SizeSquared2D() >= FLOAT(128.0f * 128.0f * g_netLagToleranceDisatnce * g_netLagToleranceDisatnce)) // pull back only when beyond 2 128-unit grid cells

		//if ((Point - actor->Location).SizeSquared2D() >= FLOAT(64.0f * 64.0f )) // pull back when beyond half a grid cell
		//FLOAT disSquare = (Point - actor->Location).SizeSquared2D();
		//if(disSquare<=8.0f*8.0f || disSquare>=256.0f*256.0f)
		//{
			Point.Z = actor->GetValidZValue(Point);
			Point.Z += actor->GetCollisionHeight();
			actor->SetLocation(Point);
			actor->UpdatePlaneShadowPosition();
			actor->UpdateLocationZ();
			if (IsMainPlayer)
			{
				actor->GetMainController()->StopMove();
			}
			else
			{
				actor->StopMovement();
			}
		//}
		//else
		//{
		//	Point.Z = actor->GetValidZValue(Point);
		//	Point.Z += actor->GetCollisionHeight();
		//	actor->eventScriptMoveTo(Point, 128.0f, 1.0f);
		//}


		//if ((Point - actor->Location).SizeSquared2D() >= FLOAT(64.0f * 64.0f )) // pull back when beyond half a grid cell
		//{
		//	Point.Z = actor->GetValidZValue(Point);
		//	Point.Z += actor->GetCollisionHeight();
		//	actor->SetLocation(Point);
		//	actor->UpdateLocationZ();
		//}

		//if (IsMainPlayer)
		//{
		//	actor->GetMainController()->StopMove();
		//}
		//else
		//{
		//	actor->StopMovement();
		//}
	}

	actor->SetServerLocation(pos);	//zhaojun@snda

	//float fX = MMOFacade::getSingleton().GetMainActor()->Location.X;
	//float fY = MMOFacade::getSingleton().GetMainActor()->Location.Y;
	//const tm_ex* pNow = localtime_ex();
	// 	debugf(L"%02d:%02d:%02d.%03d stop at (%.2f, %.2f) from (%.2f, %.2f)", pNow->tm_hour, pNow->tm_min, pNow->tm_sec, pNow->tm_msec,
	// 		msg->nCurrentX, msg->nCurrentY, fX, fY);
}

void CMessageHandler::Parse_g2c_object_rotate(const char* buf, UINT32 size)
{
	G2C_OBJECT_ROTATE* msg = (G2C_OBJECT_ROTATE*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if(actor==NULL)return;
	UBOOL bSelfCanRotote = actor->RotateToMousePointRefrence<=0 && (actor->isMoving==FALSE||actor->AttackAi_LockTime>0.0f);

	if ( actor->IsMainPlayer()==FALSE||bSelfCanRotote )
	//if (actor && actor->RotateToMousePointRefrence <= 0  && (actor->isMoving==FALSE||actor->AttackAi_LockTime>0.0f))
	{
		FRotator NewRotation = actor->Rotation;
		NewRotation.Yaw = ConvertToClientDegree(msg->wFaceDirection);
		FLOAT delta = FLOAT(msg->wDuration) / 1000.f;

#if SHOW_MOVEMENT_LOG

		debugf(TEXT("g2c_object_rotate Id=%d, Cur=%d To %d, time delta=%f"), actor->ServerID, actor->Rotation.Yaw, NewRotation.Yaw, delta);
		

		actor->FlushPersistentDebugLines();
		FVector DstPoint = actor->Location + NewRotation.Vector().SafeNormal() * 200.0f;
		actor->DrawDebugLine(actor->Location, DstPoint, 0, 255, 0, TRUE);

		DstPoint = actor->Location + actor->Rotation.Vector().SafeNormal() * 300.f;
		actor->DrawDebugLine(actor->Location, DstPoint, 255, 255, 0, TRUE);
#endif

		//debugf(TEXT("g2c_object_rotate Id=%d, Cur=%d To %d, time delta=%f   IsMoving=%d"), actor->ServerID, actor->Rotation.Yaw, NewRotation.Yaw, delta, actor->isMoving );

		if (msg->wDuration == 0)
		{
			actor->LockDesiredRotation(FALSE, TRUE);
			actor->SetDesiredRotation(NewRotation, FALSE, TRUE, -1.f, TRUE);
			actor->SetRotation(NewRotation);
		}
		else
		{
			actor->RotatoTo(NewRotation, delta);
		}
		
	}
	else
	{
		if(actor && actor->IsMainPlayer()==FALSE){
			debugf(TEXT("************ Parse_g2c_object_rotate ERROR!!!  (%s)"), *actor->ActorName);
		}
	}
}

void CMessageHandler::Parse_g2c_sync_role_pos(const char* buf, UINT32 size)
{
	// Do not sync position during a cutscene
	if(MMOFacade::getSingleton().GetMainPlayer()->IsInMatinee)
	{
		return;
	}

    G2C_SYNC_ROLE_POS* msg = (G2C_SYNC_ROLE_POS*)buf;
    FVector NewLocation(msg->wCurrentX, msg->wCurrentY, msg->wCurrentZ);

    ConvertCoordN2C(NewLocation);
    //debugf(NAME_Debug, TEXT("sync_role_pospos(%f, %f, %f)"), NewLocation.X, NewLocation.Y, NewLocation.Z );

	

	AMMOActor* mmoActor = MMOFacade::getSingleton().GetCharacter(msg->dwRoleID);	//    MMOFacade::getSingleton().GetMainPlayer();
	//check(mmoActor);

	//// not sync neither during revive
	//if( MMOFacade::getSingleton().GetMainController() &&
	//	MMOFacade::getSingleton().GetMainController()->OnRevive )
	//{
	//	return;
	//}
	if(mmoActor){
		NewLocation.Z += mmoActor->GetCollisionHeight();
		mmoActor->SetLocation(NewLocation);
		mmoActor->UpdateLocationZ();

		FRotator NewRotation(0, 0, 0);
		NewRotation.Yaw = ConvertToClientDegree(msg->wFaceDirection);
		mmoActor->LockDesiredRotation(FALSE, TRUE);
		mmoActor->SetDesiredRotation(NewRotation, FALSE, TRUE, -1.f, TRUE);
		mmoActor->SetRotation(NewRotation);

		if(mmoActor->IsMainPlayer())
		{
			mmoActor->MainController->StopMove(FALSE);
		}
		else{
			mmoActor->StopMovement();
		}
		mmoActor->UpdatePlaneShadowPosition();
		mmoActor->SetServerLocation(NewLocation);	// zhaojun@snda
	}
	return;

/*
	//MMOTargetManager::getSingleton().Clear();
	//FSMIO::getSingleton().State.ClearAllState();
	MMOFacade::getSingleton().GetMainController()->ResetMoving();
//	FLOAT speed = ConvertToClientSpeed(msg->wSpeed);
	FLOAT speed = 10000.0f;
	FVector Src(msg->wCurrentX, msg->wCurrentY, msg->wCurrentZ);
	ConvertCoordN2C(Src);
	FRotator Dir(0, 0, 0);
	Dir.Yaw = ConvertToClientDegree(msg->wFaceDirection);
	mmoActor->ResetMoveTo(msg->byMoveState, speed, Src, Src, Dir);
*/
}

// send msg to gameserver to get more char data
void CMessageHandler::SendQueryCharData(UINT CharId, UINT CRC)
{
	MMOFacade::getSingleton().UpdateTimeStamp();

	C2G_QUERY_CHAR_DATA msg;
    msg.wMsg = c2g_query_char_data;
	msg.dwCharID = CharId;
	msg.dwCRC = CRC;
	//msg.dwTimeStamp = MMOFacade::getSingleton().GetCurrentTimeStamp();
	_Send(&msg, sizeof(msg));

}

//------------------------------------------------------------------------
void CMessageHandler::Send_QueryPlayerInfo( UINT32 roleID , INT crc )
{
	C2M_QUERY_ROLE_INFO msg;
	msg.wMsg = c2m_query_role_info; 
	msg.dwRoleID = roleID;
	msg.dwCRC = crc;
	_Send(&msg, sizeof(msg));
}

//
void CMessageHandler::Parse_m2c_sync_role_info(const char* buf, UINT32 size)
{
	M2C_SYNC_ROLE_INFO* msg = (M2C_SYNC_ROLE_INFO*)buf;

	
	MMOFacade::getSingleton().AddPlayerBaseInfo( *msg );
	RankListManager::getSingleton().NotifyReFreshList();
}

void CMessageHandler::Parse_m2c_queue_info(const char* buf, UINT32 size)
{
	M2C_QUEUE_INFO* msg = (M2C_QUEUE_INFO*)buf;

	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Login_Queue, TRUE, TEXT("") );
	MMOGameEvent_Login_Queue evt( msg->nTotal, msg->nSelfPos );
	MMOUI::PostGameEvent(&evt);
}

//===============================================================================
void CMessageHandler::Parse_g2c_sync_vip_state(const char* buf, UINT32 size)
{
	G2C_SYNC_VIP_STATE* msg = (G2C_SYNC_VIP_STATE*)buf;

	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();
	for( INT i = vlLevel1; i < vlTotal; ++i ){
		mmoVipMgr.SetMyVipLeftSecondByServerTime( i, msg->dwVIPTime[i] );
	}

	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( pMainPlayer != NULL ){
		MMOGameEvent_OneInt vipStateEvt( GameEvent_VipStateChanged, pMainPlayer->ServerID );
		MMOUI::PostGameEvent( &vipStateEvt );
	}

	// Refresh quest status
	MMOQuestManager::getSingleton().UpdateQuestStatus();
}
//---------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_vip_level(const char* buf, UINT32 size)
{
	G2C_SYNC_VIP_LEVEL* msg = (G2C_SYNC_VIP_LEVEL*)buf;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( msg->dwRoleID );
	if (actor == NULL){
		warnf(TEXT("no such actor:%d, when recv g2c_sync_vip_level"), msg->dwRoleID);
		return;
	}

	//
	AMMOPlayer *pPlayer = Cast<AMMOPlayer>(actor);
	if( pPlayer != NULL ){
		pPlayer->SetVip( msg->byVIPLevel );
	}

	MMOGameEvent_OneInt vipStateEvt( GameEvent_VipStateChanged, msg->dwRoleID );
	MMOUI::PostGameEvent( &vipStateEvt );
}
//===============================================================================

// process char data
void CMessageHandler::ProcessCharData( G2C_CHAR_APPEAR* msg, CHAR_APPEAR_DATA* data, INT appearType )
{ 
	SCOPE_CYCLE_COUNTER(STAT_Load_ProcessCharData);

	//debugf( TEXT("[CMessageHandler::ProcessCharData] %d"), data->dwCharID);

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(data->dwCharID);
	if (actor == MMOFacade::getSingleton().GetMainActor())
	{
		AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
		if(ctrl != NULL)
		{
			ctrl->WaitingForTheCombatPose = -1.0f;
		}
		// return if this actor is  the main actor(controled by my self);
		return;
	}

	INT CharType = GetTypeFromID(data->dwCharID);

	if (actor == NULL)
	{

		FVector NewLocation;
		NewLocation.X = data->wCurrentX;
		NewLocation.Y = data->wCurrentY;
		NewLocation.Z = data->wCurrentZ;
		ConvertCoordN2C(NewLocation);
		FRotator Rot(0, 0, 0);
		Rot.Yaw = ConvertToClientDegree(data->wFaceDirection);

		actor = MMOFacade::getSingleton().CreateCharacter(data->dwCharID, CharType, NewLocation, Rot);
		if (actor == NULL)
		{
			warnf(TEXT("create npc/pc failed Id(%d)"), data->dwCharID);
			return;
		}

		if(msg)
		{
			actor->SceneObjectAppearType = msg->byAppearType;
		}
		else
		{
			actor->SceneObjectAppearType = ESOAT_InRange;
		}
		actor->UpdatePlaneShadowPosition();
		if (CharType == otNpc)
		{
		}
	}
	
	// create other player proxy
	switch(CharType)
	{
	case otRole:
		{
			if(data && data->byLifePercent==0)
			{
				actor->SetDead();
			}
		}
		break;

	case otNpc:
		{
			SCOPE_CYCLE_COUNTER(STAT_Load_ProcessCharData_NPC);
			//UBOOL bShow = MMOFacade::getSingleton().GetMainController()->bShowNpc;
			//actor->SetHidden(!bShow);

			AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(data->dwCharID);
			if(actor && msg && msg->stAppearData.bPet)
			{
				actor->SetActorType( EACT_Pet );
			}

			G2C_SYNC_NPC_EX_DATA* NpcCacheData = m_NPCDataCache.GetCacheData(data->dwCharID);
			if (NpcCacheData == NULL)
			{
				SendQueryCharData(data->dwCharID, 0);
			}
			else if (appearType == soatRevive)
			{
				m_NPCDataCache.Clear(data->dwCharID);
				SendQueryCharData(data->dwCharID, 0);
			}
			else
			{
				//SyncNpcData(pos->second);
				if(actor&&actor->ActorAttributes)
				{
					actor->ActorAttributes->HP = 0;
				}
				MMOFacade::getSingleton().AddOjbectToSync(data->dwCharID);
				INT size = (NpcCacheData->byType==E_npct_Pet)?sizeof(G2C_SYNC_NPC_EX_DATA):sizeof(G2C_SYNC_NPC_DATA);
				INT crc = crc32(0, (const Bytef*)NpcCacheData, size);
				SendQueryCharData(data->dwCharID, crc);

				if(actor)
				{
					if(NpcCacheData->byType==E_npct_Pet)
					{
						actor->ActorAttributes->PetLevel = NpcCacheData->byPetLevel;
						MMOFacade::getSingleton().UpdatePetLevelColor(actor);
					}
					MMOUI::GetUIManager()->delegateUpdateHintText(actor);
				}
			}
		}
		break;

	case otHero:
		{
			INT2SyncHeroDataMap::iterator pos = m_HeroDataCache.find(data->dwCharID);
			if (pos == m_HeroDataCache.end())
			{
				SendQueryCharData(data->dwCharID, 0);
			}
			else
			{
				SyncHeroData(pos->second, NULL);
				INT crc = crc32(0, (const Bytef*)pos->second, sizeof(G2C_SYNC_NPC_DATA));
				SendQueryCharData(data->dwCharID, crc);
			}
		}
		break;
	};

	actor->ActorAttributes->IsGreyName = data->bGreyName;
	//mount
	actor->ActorAttributes->MountID = data->byMountID;
	//wing
	{
		INT OldWingID = actor->ActorAttributes->WingID;
		actor->ActorAttributes->WingID = data->wWingID;
		if(OldWingID != (INT)data->wWingID && actor->Mesh && actor->Mesh->SkeletalMesh)
		{
			if(data->wWingID > 0)
				actor->EquipWing(data->wWingID);
			else
				actor->UnEquipWing();
		}
	}
	actor->ActorAttributes->byCombatPoseOn = data->byCombatPoseOn;
	actor->bHoldWeapon = !(actor->ActorAttributes->byCombatPoseOn);
	//hp percent
	actor->ActorAttributes->HpPercent = (INT)(data->byLifePercent);
	// init position
	FVector Location(data->wCurrentX, data->wCurrentY, data->wCurrentZ);
	ConvertCoordN2C(Location);
	Location.Z += actor->GetCollisionHeight();
	actor->SetLocation(Location);
	actor->UpdateLocationZ();
	FRotator Rot(0, 0, 0);
	Rot.Yaw = ConvertToClientDegree(data->wFaceDirection);
	actor->LockDesiredRotation(FALSE, TRUE);
	//debugf(L"ProcessCharDAta");
	actor->SetDesiredRotation(Rot, FALSE, FALSE, 0.05f, TRUE);


	if (data->byMoveState == CMS_DIE)
	{
		actor->bDead = TRUE;
		actor->ActorAttributes->IsGreyName = FALSE;
		MMOUI::GetUIManager()->delegateUpdateHintText(actor);
	}
	else
	{
		actor->bDead = FALSE;
	}

	if (CharType == otNpc)
	{
		ANPC* npc = (ANPC*)(actor);
		if (npc && data->bPet)
		{
			npc->OverrideActorType = EACT_Pet;
			npc->SetActorType( EACT_Pet );
		}
	}

	if(CharType==otRole)
	{
		AMMOPlayer* pPlayer = Cast<AMMOPlayer>(actor);
		if(pPlayer)
		{
			pPlayer->RefreshCombatPose();
		}
	}
	actor->RefreshAnim();

	////FVector Dst(data->wDestX, data->wDestY, actor->Location.Z);
	////ConvertCoordN2C(Dst);
	////Dst.Z = actor->GetValidZValue(Dst);
	//////Dst.Z += actor->GetCollisionHeight();

	////FLOAT speed = 10000.0f;//ConvertToClientSpeed(data->wSpeed);
	////actor->ResetMoveTo(data->byMoveState, speed, Dst, Location, Rot);

	actor->ReCalGridBlockRadius();	// zhaojun@snda

	{
		SCOPE_CYCLE_COUNTER(STAT_Load_ProcessCharData_EventAppear);
		MMOGameEvent_ActorAppear evt(actor);
		MMOUI::PostGameEvent(&evt);
	}
	/*
	else if( data->byMoveState == CMS_STALL )
	{
		//The appearing Actor's state may be EMT_PrepareStall or EMT_InStall
		//Request from server

		//@debug hook up this part
		actor->MoveType = EMT_InStall;
		
		INT2SyncRoleDataMap::iterator pos = m_RoleDataCache.find(data->dwCharID);
		if (pos == m_RoleDataCache.end())
		{
			SendQueryCharData(data->dwCharID, 0);
		}
		else
		{
			INT crc = crc32(0, (const Bytef*)pos->second, sizeof(G2C_SYNC_ROLE_DATA));
			SendQueryCharData(data->dwCharID, crc);
		}
	}
	*/
}

void CMessageHandler::NPCChangeState(INT InObjectId, BYTE InState)
{
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(InObjectId);
	ANPC* npc = Cast<ANPC>(actor);
	if (npc)
	{

		switch(InState)
		{
		case npctNpc:
			if(npc->Template)
			{
				npc->SetActorType( npc->Template->ActorType );
			}

			SendQueryCharData(InObjectId, 0);
			break;
		case npctPet:
			npc->SetActorType( EACT_Pet );
			npc->OverrideActorType = EACT_Pet;
			SendQueryCharData(InObjectId, 0);
			break;
		}
		//MMOUI::GetUIManager()->delegateUpdateHintText(npc);
	}
}


void CMessageHandler::SetActorAttribute(AMMOActor* Actor, INT AttributeId, INT Value)
{
	if (Actor == NULL)
	{
		return;
	}

	if (Actor->ActorAttributes && Actor->ActorAttributes->AttributeList.Num() > AttributeId )
	{
		Actor->ActorAttributes->AttributeList(AttributeId) =  Value;
	}
}

// a character appeared
void CMessageHandler::Parse_g2c_char_appear(const char* buf, UINT32 size)
{
	G2C_CHAR_APPEAR* msg = (G2C_CHAR_APPEAR*)buf;
		
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_appear] ID:%d"), msg->stAppearData.dwCharID);

#if M_ENABLE_ASYNC_PROC_CHAR
	check(size == sizeof(G2C_CHAR_APPEAR));

	FMsgBuf msgBuf;
	msgBuf.Size = size;
	msgBuf.MsgBuf = (MESSAGE_HEADER*)new BYTE[size];
	memcpy(msgBuf.MsgBuf, buf, size);
	_charAppearList.push_back(msgBuf);
#else
	ProcessCharData(msg, &(msg->stAppearData), msg->byAppearType);
#endif
}

void CMessageHandler::Parse_g2c_sync_npc_transform_state(const char* buf, UINT32 size)
{
	G2C_SYNC_NPC_TRANSFORM_STATE* msg = (G2C_SYNC_NPC_TRANSFORM_STATE*)buf;
	NPCChangeState(msg->dwNpcID, msg->byTransformState);
}

void CMessageHandler::Parse_g2c_sync_char_list(const char* buf, UINT32 size)
{
	G2C_SYNC_CHAR_LIST* msg = (G2C_SYNC_CHAR_LIST*)buf;

	INT byCharCount = (size - sizeof(G2C_SYNC_CHAR_LIST)) / (sizeof(msg->astAppearData[0]));
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_sync_char_list] Count:%d"), byCharCount);

	//FLOAT timer = appSeconds();
#if M_ENABLE_ASYNC_PROC_CHAR
	FMsgBuf msgBuf;
	msgBuf.Size = size;
	msgBuf.MsgBuf = (MESSAGE_HEADER*)new BYTE[size];
	memcpy(msgBuf.MsgBuf, buf, size);
	_charAppearList.push_back(msgBuf);
#else
	for (INT i = 0; i < byCharCount; ++i)
	{
		ProcessCharData(0, &(msg->astAppearData[i]), soatInRange);
	}
#endif
	//timer = appSeconds() - timer;
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_sync_char_list] Count:%d Time:%.4f"), byCharCount, timer);
}

#if M_HACK_AVATAR_COUNT
INT g_hackAvatarCount = 1;
bool g_enableHackAvatarCount = false;
TArray<G2C_SYNC_ROLE_DATA> g_hackAvatarList;
#endif

void CMessageHandler::Parse_g2c_sync_role_data(const char* buf, UINT32 size)
{
	G2C_SYNC_ROLE_DATA* pkSrc = (G2C_SYNC_ROLE_DATA*)buf;
	/*debugIDf( TEXT("Network"), TEXT("[Network::Parse_g2c_sync_role_data] ID:%d Class:%d Sex:%d"),
		pkSrc->dwRoleID, pkSrc->byClass, pkSrc->bySex);*/

	G2C_SYNC_ROLE_DATA* pk = pkSrc;

#if M_HACK_AVATAR_COUNT

	if(g_enableHackAvatarCount)
	{
		if(g_hackAvatarList.Num() < g_hackAvatarCount)
		{
			g_hackAvatarList.AddItem(*pkSrc);
		}
		else
		{
			INT idx = (INT)(g_hackAvatarCount * appFrand());
			pk = &g_hackAvatarList(idx);
			pk->dwRoleID = pkSrc->dwRoleID;
		}
	}

#endif

	G2C_SYNC_ROLE_DATA* OldData = m_RoleDataCache.GetCacheData(pkSrc->dwRoleID);
	if (OldData == NULL)
	{	
		G2C_SYNC_ROLE_DATA* newPk = new G2C_SYNC_ROLE_DATA;
		memcpy(newPk, pk, sizeof(G2C_SYNC_ROLE_DATA));
		m_RoleDataCache.AddNewData(pk->dwRoleID, newPk);
		MMOFacade::getSingleton().AddOjbectToSync(pk->dwRoleID);
	}
	else
	{
		memcpy(OldData, pkSrc, sizeof(G2C_SYNC_ROLE_DATA));
		MMOFacade::getSingleton().AddOjbectToSync(pk->dwRoleID);
	}
}

void CMessageHandler::Parse_g2c_sync_attr_list(const char* buf, UINT32 size)
{
	G2C_SYNC_ATTR_LIST* msg = (G2C_SYNC_ATTR_LIST*)buf;
    INT byAttrCount = (size - sizeof(G2C_SYNC_ATTR_LIST)) / sizeof(msg->astAttrData[0]);

	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if (!actor || !actor->ActorAttributes )
	{			
		return;
	}
	//Player attribute initialization
		actor->ActorAttributes->MountID = 0;
		actor->ActorAttributes->MountList.Empty();
		MMOFacade::getSingleton().RefreshMountData();

		actor->ActorAttributes->HeroID = 0;

		actor->ActorAttributes->AttributeList.Empty();
		actor->ActorAttributes->AttributeList.Add( catTotal );

	for (INT i = 0; i < byAttrCount; ++i)
	{
		MMOFacade::getSingleton().SetPlayerAttribute(msg->astAttrData[i].byAttrID, msg->astAttrData[i].nAttrValue);
		//SetActorAttribute(actor, msg->astAttrData[i].byAttrID, msg->astAttrData[i].nAttrValue);
	}

	actor->ReCalGridBlockRadius();	// zhaojun@snda
	
}

void CMessageHandler::Parse_g2c_sync_attr_change(const char* buf, UINT32 size)
{
	G2C_SYNC_ATTR_CHANGE* msg = (G2C_SYNC_ATTR_CHANGE*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if (!actor)
	{
		return;
	}

	MMOFacade::getSingleton().SetPlayerAttribute( msg->byAttrID, msg->nAttrValue);
	
	actor->ReCalGridBlockRadius();	// zhaojun@snda

}

void CMessageHandler::Parse_g2c_sync_role_avatar( const char* buf, UINT32 size )
{
	G2C_SYNC_ROLE_AVATAR* msg = (G2C_SYNC_ROLE_AVATAR*)buf;
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_sync_role_avatar] %d"), msg->dwRoleID );

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( msg->dwRoleID );
	if( actor )
	{
		if( actor->IsMainPlayer() ) ///Player does not handle this; item_swap etc. handle it
			return;
		if( msg->nTemplateID == 0 )
		{	// take off
			if( msg->byConSlot == CONSLOT_EQUIPPED )
			{
				actor->ItemManager->RequestTakeOff(GetClientEquipTypeByServer(msg->byItemSlot), true);
				actor->ItemManager->UpdateOwnerData();
			}
			//else if( msg->wConSlot == CONSLOT_HERO_EQUIPPED )
			//{
			//	AMMOActor* hero = MMOFacade::getSingleton().GetCharacter( actor->ActorAttributes->HeroID);
			//	if(hero && hero->ItemManager)
			//	{
			//		hero->ItemManager->RequestTakeOff( GetClientEquipTypeByServer( msg->wItemSlot ), true );
			//	}
			//}
			//else if( msg->wConSlot == CONSLOT_EQUIPPED_AVATAR )
			//{
			//	actor->ItemManager->RequestTakeOff( GetClientEquipTypeByServer( msg->wItemSlot, false ), false );
			//	actor->ItemManager->UpdateOwnerData();
			//}
		}
		else
		{
			const UItem* item = MMOFacade::getSingleton().FindItemTemplate(msg->nTemplateID);
			if( item )
			{
				if( msg->byConSlot == CONSLOT_EQUIPPED )
				{
					AMMOPlayer* player = Cast<AMMOPlayer>(actor);
					if (player)
					{
						player->WeaponUpgradePoint = msg->byRefineFx;
					}

					actor->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer( msg->byItemSlot ), true, item );
				}
				//else if( msg->wConSlot == CONSLOT_HERO_EQUIPPED )
				//{
				//	AMMOActor* hero = MMOFacade::getSingleton().GetCharacter( actor->ActorAttributes->HeroID);
				//	if(hero && hero->ItemManager)
				//	{
				//		hero->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer( msg->wItemSlot ), true, item );
				//	}
				//}
				//else if( msg->wConSlot == CONSLOT_EQUIPPED_AVATAR )
				//{
				//	actor->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer( msg->wItemSlot, false ), false, item );
				//}
				actor->ItemManager->UpdateOwnerData();
			}
		}
	}
}

void CMessageHandler::Parse_g2c_sync_hero_avatar( const char* buf, UINT32 size )
{
	G2C_SYNC_HERO_AVATAR* msg = (G2C_SYNC_HERO_AVATAR*)buf;
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_sync_hero_avatar] %d"), msg->dwHeroID);

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( msg->dwHeroID );
	if( actor )
	{
		if( actor->IsMainPlayer() ) ///Player does not handle this; item_swap etc. handle it
			return;
		if( msg->nTemplateID == 0 )
		{	// take off
			if( msg->byConSlot == CONSLOT_HERO_EQUIPPED )
			{
				actor->ItemManager->RequestTakeOff(GetClientEquipTypeByServer(msg->byItemSlot), true);
			}
		}
		else
		{
			const UItem* item = MMOFacade::getSingleton().FindItemTemplate(msg->nTemplateID);
			if( item )
			{
				if( msg->byConSlot == CONSLOT_HERO_EQUIPPED )
				{
					actor->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer( msg->byItemSlot ), true, item );
				}
				actor->ItemManager->UpdateOwnerData();
			}
		}
	}
}



void CMessageHandler::Parse_g2c_object_disappear_list(const char* buf, UINT32 size)
{
	UBOOL bNeedTrySpawnPlayer = FALSE;

	G2C_OBJECT_DISAPPEAR_LIST* msg = (G2C_OBJECT_DISAPPEAR_LIST*)buf;
	INT count = (msg->wSize - sizeof(FLEX_MESSAGE_HEADER) ) / sizeof(UINT32);
	for (INT i = 0; i < count; ++i)
	{
		//m_NPCDataCache.Clear(msg->dwObjIDList[i]);
		MMOFacade::getSingleton().RemoveOjbectToSync(msg->dwObjIDList[i]);

		MMOFacade::getSingleton().DestroyObject(msg->dwObjIDList[i], TRUE, sodtOutOfRange);

		if (GetTypeFromID(msg->dwObjIDList[i]) == otRole)
		{
			bNeedTrySpawnPlayer = TRUE;
		}
	}
}

void CMessageHandler::Parse_g2c_object_disappear(const char* buf, UINT32 size)
{
	G2C_OBJECT_DISAPPEAR* msg = (G2C_OBJECT_DISAPPEAR*)buf;

	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_object_disappear] ID:%d"), msg->dwObjID);

	MMOFacade::getSingleton().DestroyObject(msg->dwObjID, TRUE, msg->byDisappearType);
}

void CMessageHandler::OnMissileAppear(MISSILE_APPEAR_DATA& InData)
{
	FVector Src(InData.wCurrentX, InData.wCurrentY, InData.wCurrentZ);
	FVector Dst(InData.wDestX, InData.wDestY, InData.wDestZ);
	FVector Dis = Dst-Src;
	FRotator Rot(0, 0, 0);
	Rot.Yaw = ConvertToClientDegree(InData.wFaceDirection);
	ConvertCoordN2C(Src);
	ConvertCoordN2C(Dst);
	float LifeTime = FLOAT(InData.wLifeFrm) / FLOAT(GAME_FPS);
	float maxL = max(abs(Dis.X), abs(Dis.Y));
	float InSpeedX = 1.0f;
	if (maxL>0.0)InSpeedX = Dis.Size2D() / maxL;
	MMOFacade::getSingleton().GetAbilityProcessor()->OnMissileAppear(InData.dwMissileID, InData.wTemplateID, InData.dwOwnerID, InSpeedX, Src, Rot, LifeTime);
}

//void CMessageHandler::Parse_g2c_self_hifreq_sync(const char* buf, UINT32 size)
//{
//	G2C_SELF_HIFREQ_SYNC* msg = (G2C_SELF_HIFREQ_SYNC*)buf;
//
//	MMOFacade::getSingleton().SetMyCurrentHP(msg->nCurrentHp);
//	MMOFacade::getSingleton().SetMyCurrentMP(msg->nCurrentMp);
//
//}
//
//void CMessageHandler::Parse_g2c_region_hifreq_sync(const char* buf, UINT32 size)
//{
//	G2C_REGION_HIFREQ_SYNC* msg = (G2C_REGION_HIFREQ_SYNC*)buf ;
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID) ;
//	if(actor==NULL)return;
//	if(FSMIO::getSingleton().Target.GetSelectMMOActor() == actor)
//	{
//		return;
//	}
//
//	actor->ActorAttributes->HpPercent = msg->byHpPercent;
//	INT Hp = msg->byHpPercent * actor->ActorAttributes->GetMaxHp()/100;
//
//	AMMOActor* MainPlayer = MMOFacade::getSingleton().GetMainActor();
//	if(MainPlayer && msg->dwCharID == MainPlayer->ServerHeroID )
//	{
//		MainPlayer->HeroAttributes->SetHP(Hp);
//		MainPlayer->HeroAttributes->SetMaxHP(actor->ActorAttributes->GetMaxHp());
//	}
//
//	ANPC* npc = Cast<ANPC>(actor);
//	if(npc)
//	{
//		if( npc->ActorAttributes->GetCurHp() != Hp)
//		{
//			MMOFacade::getSingleton().SetNPCHP( npc , Hp );
//		}
//	}
//	AMMOPlayer* pl = Cast<AMMOPlayer>(actor);
//	if(pl && pl!= MMOFacade::getSingleton().GetMainPlayer() )
//	{
//		if( pl->ActorAttributes->GetCurHp() != Hp)
//		{
//			MMOFacade::getSingleton().SetPlayerHP(pl, Hp );
//		}
//	}
//
//	//if (actor && actor->ActorAttributes)
//	//{
//	//	//  Display like this for now
//	//	//actor->ActorAttributes->MaxHP = msg->byHpPercent;
//	//	actor->ActorAttributes->HP = msg->byHpPercent * actor->ActorAttributes->GetMaxHp()/100;			
//	//}
//}

void CMessageHandler::Parse_g2c_sync_pk_result(const char* buf, UINT32 size)
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	G2C_SYNC_PK_RESULT* msg = (G2C_SYNC_PK_RESULT*)buf;
	if( !player || !msg )
	{
		return;
	}
	
	UBOOL iamKiller = ( msg->dwKillerRoleID == player->ServerID );
	FriendManager::getSingleton().AddKillInfoList( iamKiller , msg->dwKillerRoleID , msg->dwVictimRoleID );

	if( iamKiller )
	{
		AMMOActor* VictimRole = MMOFacade::getSingleton().GetCharacter(msg->dwVictimRoleID);	

		FString VictimName = VictimRole!=NULL?VictimRole->ActorName:FString::Printf(TEXT("No such player"));
		FString str;
		switch(msg->byPKResult)
		{
		case rprNormal:
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.PkResult.rprNormal") ) , *VictimName, msg->shChangePkValue );
			break;
		case rprRedName:
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.PkResult.rprRedName") ) , *VictimName );
			break;
		case rprCounterStrike:
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.PkResult.rprCounterStrike") ) , *VictimName );
			break;
		case rprOpponentGuildMember:
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.PkResult.rprOpponentGuildMember") ) , *VictimName );
			break;
		case rprLuckyKill:
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.PkResult.rprLuckyKill") ) , *VictimName );
			break;
		default:
			return;
			break;
		}

		MMOUI::GetUIManager()->delegateAddScreenMessage( str ,MsgInfoType_Table,str);
	}
	


}


void CMessageHandler::Parse_g2c_sync_grey_name(const char* buf, UINT32 size)
{
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	G2C_SYNC_GREY_NAME* msg = (G2C_SYNC_GREY_NAME*)buf;
	if( !msg )
	{
		return;
	}

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwRoleID);
	if(actor && actor->ActorAttributes)
	{
		actor->ActorAttributes->IsGreyName = msg->bGreyName;
		MMOUI::GetUIManager()->delegateUpdateHintText(actor,ESHU_NAME);
	}

}


void CMessageHandler::Parse_g2c_sync_pk_value(const char* buf, UINT32 size)
{
	G2C_SYNC_PK_VALUE* msg = (G2C_SYNC_PK_VALUE*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwRoleID);
	if(actor && actor->ActorAttributes)
	{
		if ( actor == MMOFacade::getSingleton().GetMainActor() )
		{
			AMMOPlayer* player =Cast<AMMOPlayer>(actor);
			//UBOOL IsSameLevel = MMOFacade::getSingleton().IsSamePkLevel(MMOFacade::getSingleton().GetMyPkValue(),msg->dwPkValue);
			MMOFacade::getSingleton().SetMyPkValue(msg->dwPkValue);
			MMOFacade::getSingleton().NotifyPlayerPkValueChange();
		}
		else
		{
			AMMOPlayer* player =Cast<AMMOPlayer>(actor);
			UBOOL IsSameLevel = MMOFacade::getSingleton().IsSamePkLevel(actor->ActorAttributes->GetPkValue(),msg->dwPkValue);
			actor->ActorAttributes->SetPkValue(msg->dwPkValue); 
			if(IsSameLevel==FALSE)
			{
				MMOFacade::getSingleton().NotifyOtherPlayerPkValueChange(player,msg->dwPkValue);
			}
		}
	}
}

void CMessageHandler::Parse_g2c_modify_reputation(const char* buf, UINT32 size)
{
	G2C_MODIFY_REPUTATION* msg = (G2C_MODIFY_REPUTATION*)buf;	
	INT reputationLast = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), msg->byForceID);

	if(MMOFacade::getSingleton().GetPlayerActiveState(MMOFacade::getSingleton().GetMainPlayer(),msg->byForceID)==FALSE)
	{
		MMOFacade::getSingleton().SetPlayerActiveState(MMOFacade::getSingleton().GetMainPlayer(),msg->byForceID,msg->byForceID,TRUE);

		ReputationTemp* pTemp =	MMOFacade::getSingleton().GetReputationTempByForceID(msg->byForceID);
		if(pTemp){
			//#16373
			//MMOUI::GetUIManager()->ShowUnlockAOGString(pTemp->ForceName);
			////MMOUI::GetUIManager()->delegateShowUnlockAOGString( pTemp->ForceName );
		}
	}

	MMOFacade::getSingleton().SetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(),msg->byForceID,msg->byForceID,msg->nReputation,reputationLast);
	MMOUI::GetUIManager()->delegateUpdateReputationData();
	MMOUI::GetUIManager()->delegateRefreshReputationSelected();

	UBOOL bFlashMenu =  MMOFacade::getSingleton().GetPlayerAwardDisplay(MMOFacade::getSingleton().GetMainPlayer(),msg->byForceID);
	if(bFlashMenu)
	{
		// Temporarily disabled for CQ version
		//MMOUI::GetUIManager()->delegateUpdateMenuEffect(2,EMenuType_Child_Repute,TRUE,TRUE);

		ShowPanelAdvancedData T;
		T.PanelName = MMOUI::GetUIManager()->Panel_Reputation.GetNameString();
		T.Prarm1	= msg->byForceID;
		MMOFacade::getSingleton().SetShowPanelAdvancedData(T.PanelName, T);
	}

	MMOQuestManager::getSingleton().UpdateQuestReputation();

	// Achievement
	INT reputation = MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), msg->byForceID);	
	MMOGameEvent_Achievement evtAch1(adForceReputation, msg->byForceID, reputation, 0);
	MMOUI::PostGameEvent(&evtAch1);

	MMOGameEvent_Achievement evtAch2(adVariousForceCount, 0, 0, 0);
	MMOUI::PostGameEvent(&evtAch2);

	// Path of the Strong
	MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_ReputationChange, msg->byForceID, msg->nReputation, reputationLast);
	MMOUI::PostGameEvent(&evtJourney);
}

void CMessageHandler::Parse_g2c_sync_title(const char* buf, UINT32 size)
{
	G2C_SYNC_TITLE* msg = (G2C_SYNC_TITLE*)buf;

	MMOFacade::getSingleton().ResetTitleData();
	MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->TitleID = msg->nEquipedTitleID;
	INT TitleCount = msg->nTitleCount;
	for ( INT i = 0 ; i < TitleCount ; i++ ) //
	{
		TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(msg->stTitleList[i].m_nTemplateID);
		if(pTitle)
		{
			pTitle->ValidTime = msg->stTitleList[i].m_nExpireTime;
		}
	}
	MMOFacade::getSingleton().NotifyPlayerTitleChange();
}

void CMessageHandler::Parse_g2c_sync_equip_title(const char* buf, UINT32 size)
{
	G2C_SYNC_EQUIP_TITLE* msg = (G2C_SYNC_EQUIP_TITLE*)buf;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwRoleID);
	if(actor && actor->ActorAttributes)
	{
		if ( actor == MMOFacade::getSingleton().GetMainActor() )
		{
			AMMOPlayer* player =Cast<AMMOPlayer>(actor);
			player->ActorAttributes->TitleID = msg->byTitleID;

			//Equipment title tooltip
			TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(msg->byTitleID);
			if(pTitle){
				FString TitleName = pTitle->TitleName;// MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.TitleName"))  + appItoa(TitleId) );
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.EquipTitleMsg")) +  TitleName ,MsgInfoType_Table,TEXT("MMOGame.Title.EquipTitleMsg"));
			}

			MMOFacade::getSingleton().NotifyPlayerTitleChange();
			MMOFacade::getSingleton().NotifyPlayerPkValueChange();

			// Tutorial
			MMOGameEvent_Guide evtGuide(EGUET_EquipTitle);
			MMOUI::PostGameEvent(&evtGuide);
		}
		else
		{
			AMMOPlayer* player =Cast<AMMOPlayer>(actor);
			actor->ActorAttributes->TitleID = msg->byTitleID;
			MMOFacade::getSingleton().NotifyOtherPlayerTitleChange(player,msg->byTitleID);
			MMOFacade::getSingleton().NotifyOtherPlayerPkValueChange(player,actor->ActorAttributes->PkValue);
		}
	}
}

void CMessageHandler::Parse_g2c_add_title(const char* buf, UINT32 size)
{
	G2C_ADD_TITLE* msg = (G2C_ADD_TITLE*)buf;
	TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(msg->nTitleID);
	if(pTitle)
	{
		pTitle->ValidTime = msg->nExpireTime;
		pTitle->IsNew = TRUE;
		MMOUI::GetUIManager()->delegateUpdateTitleData(FALSE);

		MMOUI::GetUIManager()->delegateUpdateMenuEffect(0,EMenuType_Child_Charactor,FALSE,TRUE);

		ShowPanelAdvancedData T;
		T.PanelName = MMOUI::GetUIManager()->Panel_Character.GetNameString();
		T.TabName	= _T("CharactorClickTab2");
		MMOFacade::getSingleton().SetShowPanelAdvancedData(T.PanelName, T);


		FString TitleName = pTitle->TitleName;//  MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.TitleName"))  + appItoa(msg->nTitleID) );
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.AddTitleMsg")) +  TitleName ,MsgInfoType_Table,TEXT("MMOGame.Title.AddTitleMsg"));
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.AddTitleMsg")) +  TitleName ); 

		// Achievement
		MMOGameEvent_Achievement evtAch1(adTitle, msg->nTitleID, 0, 0);
		MMOUI::PostGameEvent(&evtAch1);

		INT titleCount = 0;
		MMOFacade::getSingleton().UpdateTitleArray();
		for(INT i = 0; i < MMOFacade::getSingleton().GetTitleDataArrayCount(); ++ i)
		{
			TitleData* title = MMOFacade::getSingleton().GetTitleDataArray(i);
			if(title && title->ValidTime != 0)
			{
				titleCount ++;
				//title->IsNew = TRUE;
			}
		}
		MMOGameEvent_Achievement evtAch2(adTitleCount, titleCount, 0, 0);
		MMOUI::PostGameEvent(&evtAch2);

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_GetTitle);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

void CMessageHandler::Parse_g2c_del_title(const char* buf, UINT32 size)
{
	G2C_DEL_TITLE* msg = (G2C_DEL_TITLE*)buf;
	TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(msg->nTitleID);
	if(pTitle)
	{
		pTitle->ValidTime = 0;
		MMOUI::GetUIManager()->delegateUpdateTitleData(FALSE);
		FString TitleName = pTitle->TitleName;// MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.TitleName"))  + appItoa(msg->nTitleID) );
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.RemoveTitleMsg")) +  TitleName,MsgInfoType_Table,TEXT("MMOGame.Title.RemoveTitleMsg") );
		MMOChatManager::getSingleton().AddSystemMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.RemoveTitleMsg")) +  TitleName ); 
	}
}


void CMessageHandler::Parse_g2c_sync_reputation(const char* buf, UINT32 size)
{
	G2C_SYNC_REPUTATION* msg = (G2C_SYNC_REPUTATION*)buf;

	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
	if(ctrl==NULL)
	{
		return;
	}
	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if(player==NULL)
	{
		warnf( TEXT("[CMessageHandler::Parse_g2c_sync_reputation] :GetAMMOPlayer failed !") );
		return;
	}

	std::bitset<64> ActiveState(msg->qwActiveState);
	std::bitset<64> AwardState(msg->qwAwardState);

	for(int i=0;i<MAX_NPC_FORCE_ID;i++)
	{
		MMOFacade::getSingleton().SetPlayerReputation(player, i,msg->ReputationTable[i].byForceID, msg->ReputationTable[i].shReputation,msg->ReputationTable[i].shReputation);
		MMOFacade::getSingleton().SetPlayerActiveState(player, i,msg->ReputationTable[i].byForceID,ActiveState.test(i));
		MMOFacade::getSingleton().SetPlayerAwardState(player, i,msg->ReputationTable[i].byForceID,AwardState.test(i));
	}

	MMOQuestManager::getSingleton().UpdateQuestReputation();

	MMOUI::GetUIManager()->delegateUpdateReputationData();
	//debugf( TEXT("[CMessageHandler::Parse_g2c_sync_reputation]" ));

}

void CMessageHandler::Send_CancelAura(INT BuffInstanceId)
{
	C2G_CANCEL_AURA msg;
	msg.wMsg=	c2g_cancel_aura;
	msg.dwInstanceID = BuffInstanceId;		
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CombatPoseOn(UBOOL bOn, UINT8 byType)
{
	C2G_COMBAT_POSE_SWITCH msg;
	msg.wMsg=	c2g_combat_pose_switch;
	msg.byCombatPoseOn =  bOn;
	msg.byClientFlag   =  byType;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_PkMode(UINT8 byPkMode)
{
	C2G_CHANGE_PK_MODE msg;
	msg.wMsg=	c2g_change_pk_mode;
	msg.byMode= byPkMode;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ChangeHeroAi(UINT8 byAiType)
{
	C2G_CHANGE_HERO_AI msg;
	msg.wMsg = c2g_change_hero_ai;
	msg.byAiType = byAiType;
	_Send(&msg, sizeof(msg) );
}

void CMessageHandler::Send_HeroSummon()
{
	C2G_SUMMON_HERO msg;
	msg.wMsg = c2g_summon_hero;
	_Send(&msg, sizeof(msg) );
}

void CMessageHandler::Send_HeroUncall()
{
	C2G_RECALL_HERO msg;
	msg.wMsg = c2g_recall_hero;
	_Send(&msg, sizeof(msg) );
}

void CMessageHandler::Send_HeroFire()
{
	C2G_FIRE_HERO msg;
	msg.wMsg = c2g_fire_hero;
	_Send(&msg, sizeof(msg) );
}

void CMessageHandler::Send_HeroHire(UINT8 byClass, UINT8 bySex)
{
	C2G_HIRE_HERO msg;
	msg.wMsg = c2g_hire_hero;
	msg.byClass = byClass;
	msg.bySex   = bySex;
	_Send(&msg, sizeof(msg) );
}

void CMessageHandler::Send_Query_Hero_Attr_List()
{
	C2G_QUERY_HERO_ATTR_LIST msg;
	msg.wMsg = c2g_query_hero_attr_list;
	_Send(&msg, sizeof(msg) );
}

extern INT	GetPlayerRoleID();

void CMessageHandler::Send_HardwareInfo()
{
	static INT lastRoleId = -1;
	if(lastRoleId == GetPlayerRoleID())
	{
		return;
	}

	lastRoleId = GetPlayerRoleID();

	C2G_CLIENT_HARDWARE_INFO msg;
	msg.wMsg = c2g_client_hardware_info;
	DeviceInfoHelper helper;
	UBOOL Ret = helper.GetHardwareInfo(msg);
	if (Ret)
	{
		_Send(&msg, sizeof(msg));
	}	
}

void CMessageHandler::Send_FPS()
{
	C2G_CLIENT_FPS_INFO msg;
	msg.wMsg = c2g_client_fps_info;
	msg.byGameFps = GAverageFPS;
	msg.byCpuFps = (GUnit_GameThreadTime > 0 ?  1.0 / GUnit_GameThreadTime : 0);
	msg.byGpuFps = (GUnit_RenderThreadTime > 0 ?  1.0 / GUnit_RenderThreadTime : 0);
	msg.wGateDelay = MMOFacade::getSingleton().GetGatePingDelayTime();
	msg.wGameDelay = MMOFacade::getSingleton().GetGameWorldPingDelayTime();
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_SpellRequestInfo(UINT16 InSpellId, INT32 InCurrentMp, INT32 InCdRemain, INT32 InTargetId, INT16 InTargetX, INT16 InTargetY, INT16 InTargetZ, UINT8 InNotEnoughMp, UINT8 InInCd, UINT32 InTime, UINT8 InShift)
{
	C2G_SPELL_LOG msg;
	msg.wMsg = c2g_spell_log;
	msg.wSpellID = InSpellId;
	msg.nCurrentMp = InCurrentMp;
	msg.nCdRemain = InCdRemain;
	msg.dwTargetID = InTargetId;
	msg.wTargetX = InTargetX;
	msg.wTargetY = InTargetY;
	msg.wTargetZ = InTargetZ;
	msg.byNotEnoughMp = InNotEnoughMp;
	msg.byInCd = InInCd;
	msg.dwTime = InTime;
	msg.byIsShiftPressed = InShift;
	_Send(&msg, sizeof(msg));
}


//------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_game_data_begin(const char* buf, UINT32 size)
{
    G2C_SYNC_GAME_DATA_BEGIN* msg = (G2C_SYNC_GAME_DATA_BEGIN*)buf;

	warnf(TEXT("[-------------------------CMessageHandler::Parse_g2c_sync_game_data_begin]"));
	MMOFacade::getSingleton().IsReceivedGameData = TRUE;
	//Time sync, must be placed first, otherwise the local operation delay makes the difference huge
	MMOFacade::getSingleton().SetEnterSceneServerTime( msg->dwServerTime );
	MMOWeatherManager::getSingleton().InitStartTime(msg->dwServerTime);

	//Start game time counting
	//Game time should already have been sent down; actively refresh remaining game time (character select screen does not deduct game time)
	MMOVipManager::getSingleton().UpdateMyLeftGameTimeUpdateTime();

	//MMO::ActiveGameState(EGameState_Loading);

	// clear ability andd effect processor first
	AbilityProcessor* ap = MMOFacade::getSingleton().GetAbilityProcessor();
	if (ap)
	{
		ap->ClearAllAbilityNode();
	}

	EffectProcessor::GetSingleton().DeleteAllComponent();


#if 0 // deferred loading
	// then clear ability template
	MMOSkillManager::getSingleton().ClearAbilityTemplates();
	MMOSkillManager::getSingleton().LoadAbilityTemplates((EPlayerClass)msg->byClass);
#endif

    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
    AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);

    player->ServerID = msg->dwRoleID;
	player->ServerHeroID = msg->dwHeroID;
    ctrl->PawnServerId = msg->dwRoleID;
    
    {
		//Originally initialized here, no longer here
        //player->TeamManager->TeamID = msg->dwTeamID;
        //if( msg->dwTeamID != 0 )
        //    player->TeamManager->CreateMyTeam( msg->dwTeamID );
    }
    //player->RunSpeed = ConvertToClientLength(msg->stBaseInfo.stAttackProp.wMoveSpeed);

	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();
	for( INT i = vlLevel1; i < vlTotal; ++i ){
		mmoVipMgr.SetMyVipLeftSecondByServerTime( i, msg->dwVIPTime[i] );
	}
	
	player->SetVip( mmoVipMgr.GetMyCurrentVipType() );

    player->ActorAttributes->SetHP(0);
    //player->ActorAttributes->SetMaxHP(0);
    player->ActorAttributes->SetMP(0);
    //player->ActorAttributes->SetMaxMP(0);

	MMOGameEvent_OneInt vipStateEvt( GameEvent_UpdateVipLevel, mmoVipMgr.GetMyCurrentVipType() );
	MMOUI::PostGameEvent( &vipStateEvt );

    FVector pos = FVector(
        msg->wPosX,
        msg->wPosY,
        msg->wPosZ);

	if (player == MMOFacade::getSingleton().GetMainPlayer() )
	{
		MMOFacade::getSingleton().SetMyLevel(msg->byLevel);
		MMOFacade::getSingleton().SetPlayerExp(msg->dwExp,msg->dwMaxExp,msg->dwTodayExp,msg->dwTodayMaxExp);
		MMOFacade::getSingleton().SetMyPkValue(msg->dwPkValue);
		MMOFacade::getSingleton().SetMyPkMode(msg->byPkMode);
	}
	else
	{
		player->ActorAttributes->LevelAttributes.CurrentExp = msg->dwExp;
		player->ActorAttributes->LevelAttributes.MaxExp = msg->dwMaxExp;
		player->ActorAttributes->LevelAttributes.Level	= msg->byLevel;
		player->HeroAttributes->LevelAttributes.Level	= msg->byHeroLevel;
		player->ActorAttributes->PkValue			= msg->dwPkValue;
		player->ActorAttributes->PkMode				= msg->byPkMode;
	}
	player->ActorAttributes->ClassType = msg->byClass;
	player->ActorAttributes->GenderType = msg->bySex;

	player->HeroAttributes->ClassType = msg->byHeroClass;
	player->HeroAttributes->GenderType = msg->byHeroSex;
	player->HeroAttributes->Growth	= msg->nHeroGrowth;
	player->HeroName =  FString(UTF8_TO_TCHAR(msg->szHeroName));
	player->HeroAi	 =  msg->byHeroAiType;

	//if(player->ServerHeroID>0)
	//{
	//	Send_Query_Hero_Attr_List();
	//}

    ConvertCoordN2C(pos);
	pos.Z += player->GetCollisionHeight();

    //debugf( TEXT("[CMessageHandler::Parse_g2c_attr_sync_begin] SceneTempID:%d"), msg->dwSceneTempID);

 //   FString strSceneName;
	//if( ! UGlobalMapInfo::GetSingleton()->FindMapName(msg->dwSceneTempID,strSceneName) )
	//	return;
	//
 //   ctrl->MapName = strSceneName;
	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	pMapDataMgr->SetCurMap( msg->dwSceneTempID );

	FExportMap &curMapInfo = pMapDataMgr->GetCurMap();
	if( curMapInfo.MapName == TEXT("N/A") ){
		return;
	}

	ctrl->MapName = curMapInfo.MapName;

	// OSCARLOGIN:    ctrl->eventSelectPlayerSuccess(pos);
	MMOGameEvent_Splash loginEvt;
	loginEvt.EvtType = MMOGameEvent_Splash::ESET_EnterGameEnd;
	loginEvt.PositionInfo = pos;
	MMOUI::PostGameEvent(&loginEvt);

    MMOFacade::getSingleton().AddMainCharacter(player);

   // player->eventSetMesh(msg->byClass, msg->bySex);
	player->ItemManager->UpdateApperance();
	

    if (msg->byMoveState == CMS_STAND)
    {
        player->bDead = FALSE;
    }
    else if (msg->byMoveState == CMS_DIE)
    {
        player->bDead = TRUE;
		player->ActorAttributes->IsGreyName = FALSE;
		MMOUI::GetUIManager()->delegateUpdateHintText(player);
		
    }

    player->RefreshAnim();

	LoadMapTipsManager::getSingleton().SetNextMapID( msg->dwSceneTempID);

	MMO::ActiveGameState(EGameState_Loading);

    MMOFacade::getSingleton().Teleport(curMapInfo.ServerID, pos, ConvertToClientDegree(msg->wFaceDirection));

	/*if( msg->dwOfflineExp != 0 )
	{
		MMOChatManager::getSingleton().AddSystemMessage( FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.TotalOfflineExp")) , msg->dwOfflineExp ) );
	}*/

	MMOGameEvent_OneInt evt( GameEvent_ExpExtraInit, msg->dwExtraExp );
	MMOUI::PostGameEvent(&evt);

	// Activity
	ActivityManager::getSingleton().NotifyActivityVariableErvtActivity(msg->nActivityLevel);
	ActivityManager::getSingleton().NotifyActivityVariableErvtActivityReward(msg->nActicityReward);

	//Double experience
	MMOAttributeAccountManager::getSingleton().setExpEtra(msg->dwExtraExp);

	player->ReCalGridBlockRadius();

	RankListManager::getSingleton().SetShowRank(msg->dwServerStartTime);
	RankListManager::getSingleton().SetMaxLevel(msg->wMaxLevel);
}

void CMessageHandler::Parse_g2c_sync_data_error(const char* buf, UINT32 size)
{
	G2C_SYNC_DATA_ERROR* msg = (G2C_SYNC_DATA_ERROR*)buf;

#if !SHIPPING_PC_GAME && M_ENABLE_RPMGR
	if(!g_replayMode)
	{
	MessageBox(NULL, ANSI_TO_TCHAR(msg->szErrorMsg), 0, 0);
	}
#endif
}

void CMessageHandler::Parse_g2c_exp_change(const char* buf, UINT32 size)
{
    G2C_EXP_CHANGE* msg = (G2C_EXP_CHANGE*)buf;
	if(!msg)
	{
		return;
	}
	UINT32 rowExp = msg->nExpChange - msg->nExtraExp;// - msg->nTeamExp - msg->nGuildExp - msg->nMasterExp;
	UINT32 teamExpPec = teamExpPec = 0;//rowExp > 0 ? ( msg->nTeamExp * 100 / rowExp ) : 0;
	UINT32 guildExpPec = 0;//rowExp > 0 ? ( msg->nGuildExp * 100 / rowExp ) : 0;

	FString str;
	if( msg->nExtraExp > 0 )
	{
//		if( msg->nExpChange > 0 && msg->nTeamExp == 0 )
//		{		
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.SysExpChangeWithOffline") ) , msg->nExpChange , msg->nExtraExp );
		//}
		//else if( msg->nExpChange > 0 && msg->nTeamExp > 0 )
		//{
		//	if (msg->nGuildExp > 0)
		//	{
		//		str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.TeamExpWithOfflineWithGuild") ) , msg->nExpChange , msg->nExtraExp , teamExpPec , guildExpPec );
		//	}
		//	else
		//	{
		//		str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.TeamExpWithOffline") ) , msg->nExpChange , msg->nExtraExp , teamExpPec );
		//	}
		//}
	}
	else
	{
		//if( msg->nExpChange > 0 && msg->nTeamExp == 0 )
		//{		
			str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.SysExpChange") ) , msg->nExpChange );
		//}
		//else if( msg->nExpChange > 0 && msg->nTeamExp > 0 )
		//{
		//	if (msg->nGuildExp > 0)
		//	{
		//		str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.TeamExpWithGuild") ) , msg->nExpChange , teamExpPec , guildExpPec );
		//	}
		//	else
		//	{
		//		str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.TeamExp") ) , msg->nExpChange , teamExpPec );
		//	}
		//}

	}
	//if((msg->nExpChange > 0) && (msg->nMasterExp > 0))
	//{
	//	str += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Chat.MasterExpAdd") ), msg->nMasterExp );
	//}
	
	if( !str.IsEmpty() )
	{
		MMOChatManager::getSingleton().AddSystemMessage( str );
	}

	MMOFacade::getSingleton().SetPlayerExp( msg->nExp,msg->nMaxExp ,msg->nTodayExp,msg->nTodayMaxExp,msg->nExpChange );

	//Update bonus experience
	MMOGameEvent_OneInt expExtrEvt( GameEvent_ExpExtraChanged, -msg->nExtraExp );
	MMOUI::PostGameEvent(&expExtrEvt);
}

void CMessageHandler::Parse_g2c_role_level_up(const char* buf, UINT32 size)
{
    G2C_ROLE_LEVEL_UP* msg = (G2C_ROLE_LEVEL_UP*)buf;
    AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwRoleID);
	INT nLastLevel = 0;
	INT nClassID = 0;
    if (actor && actor->ActorAttributes)
    {
		nClassID = actor->GetClassId();
		if( MMOFacade::getSingleton().GetMainActor() == actor ){
			nLastLevel = MMOFacade::getSingleton().GetMyLevel();
			MMOFacade::getSingleton().SetMyLevel(msg->byLevel );
		}
		else
		{
			nLastLevel = actor->ActorAttributes->LevelAttributes.Level;
			actor->ActorAttributes->LevelAttributes.Level = msg->byLevel;
			MMOGameEvent_OtherPlayerLevelUp evt;
			evt.lv = msg->byLevel;
			evt.roleID = actor->ServerID;
			MMOUI::PostGameEvent(&evt);
		}

		AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		player->PlayLevelUpEffect(actor->GetFootLocation(), actor->Rotation);

		// display the level up ui
		if(actor == player)
		{
// JO*(WJOSIJERg
			// TODO: show the MMOUILevelUp UI
			int deltaStrength = 0;/*MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catStrength  )
								-	MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catStrength  );*/

			int deltaAgility =	MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catAgility  )
							-	 MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catAgility  );

			int deltaIntellect = 0;/*MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catIntellect  )
							-	MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catIntellect  );*/
				
			int deltaStamina = 0;/*MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catStamina  )
							   -	MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catStamina  );*/

			int deltaSpirit = 0;/*MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catSpirit  )
							-	MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catSpirit  );*/

			int deltaValue = 0;/*MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catValue  )
				-	MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catValue  );*/

			int deltaCombatParam = 0;/*MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catCombatParam  )
				-	MMOFacade::getSingleton().GetAttributeConfigData(nLastLevel,player->GetClassId(),catCombatParam  );*/

			int deltaConstParam = 0;

			int deltaMaxHp = MMOFacade::getSingleton().GetDeltaMaxMP( nClassID , deltaConstParam, deltaStrength, deltaAgility, deltaIntellect, deltaStamina, deltaSpirit, deltaValue, deltaCombatParam );

			//int deltaMaxHp = MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel,player->GetClassId(),catMaxHp  )
			//				-MMOFacade::getSingleton().GetAttributeConfigData(msg->byLevel-1,player->GetClassId(),catMaxHp  );
			
			FString lp1 = TEXT("MMOGame.LevelUp.LevelUp1");
			FString lp2 = TEXT("MMOGame.LevelUp.LevelUp2");
			FString cs1 = MMOUI::ParseLocalizedPropertyPath(lp1);
			FString cs2 =	FString::Printf(TEXT("%d"),msg->byLevel);
			FString cs3 = MMOUI::ParseLocalizedPropertyPath(lp2);
			FString cs	= cs2;//cs1+cs2+cs3;
			
			MMOUI::GetUIManager()->ShowHintLevelUpWidget(cs,deltaStrength,deltaAgility,deltaIntellect,deltaStamina,deltaSpirit,deltaMaxHp);
			
			UIHudProcess( PlayerLevelUp(msg->byLevel) );
// JO*(WJOSIJERg end
		}
    }
}

void CMessageHandler::Parse_g2c_sync_game_data_end(const char* buf, UINT32 size)
{
    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
    AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);

	warnf(NAME_Warning, TEXT("[-------------------------CMessageHandler::Parse_g2c_sync_game_data_end]"));

}

void CMessageHandler::UpdateServerPosition(FVector& NewPosition)
{
	FVector servrPos = NewPosition;
	ConvertCoordC2N(servrPos);

	C2G_UPDATE_POS msg;
	msg.wMsg = c2g_update_pos;
	msg.wCurrentX = INT(servrPos.X);
	msg.wCurrentY = INT(servrPos.Y);
	msg.wCurrentZ = INT(servrPos.Z);
	_Send(&msg, sizeof(msg));   
}

void CMessageHandler::UpdateRotation(FRotator& NewRotation,INT DurationTime)
{
	C2G_ROLE_ROTATE msg;
    msg.wMsg = c2g_role_rotate;
	msg.wFaceDirection = ConvertToServerDegree(NewRotation.Yaw);
	msg.nDuration = DurationTime;
	//msg.dwTimeStamp = MMOFacade::getSingleton().GetCurrentTimeStamp();
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::UpdateTargetPoint(FVector& NewPoint)
{
	C2G_ROLE_ADJUST_TARGET_POINT msg;

	FVector NewLocation = NewPoint;
	ConvertCoordC2N(NewLocation);

    msg.wMsg = c2g_role_adjust_target_point;
    msg.wTargetX = NewLocation.X;
	msg.wTargetY = NewLocation.Y;
	msg.wTargetZ = NewLocation.Z;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::checkLog(FVector& checkPos, FVector& startPos, FVector& endPos)
{
    FLOAT fValueX = endPos.X - startPos.X;
    FLOAT fValueY = endPos.Y - startPos.Y;
       
    //Floating point calculation data.
    if (fabs(fValueX) > fabs(fValueY))
    {        
        FLOAT fCheckY = fValueY * (checkPos.X - startPos.X)/fValueX + startPos.Y;
        if(fabs(checkPos.Y - fCheckY) > 1)
        {
            debugf(L"----------------------------------------------------");
            debugf(L"the Y offset is %f", checkPos.Y - fCheckY);
            debugf(L"src[%f,%f,%f]=====>tgt[%f,%f,%f]", startPos.X, startPos.Y, startPos.Z, endPos.X, endPos.Y, endPos.Z);
            debugf(L"update location:%f, %f, %f", checkPos.X, checkPos.Y, checkPos.Z);
            debugf(L"----------------------------------------------------");
            return;
        }
    }
    else
    {
        FLOAT fCheckX = fValueX * (checkPos.Y - startPos.Y)/fValueY + startPos.X;
        if(fabs(checkPos.X - fCheckX) > 1)
        {
            debugf(L"----------------------------------------------------");
            debugf(L"the X offset is %f", checkPos.X - fCheckX);
            debugf(L"src[%f,%f,%f]=====>tgt[%f,%f,%f]", startPos.X, startPos.Y, startPos.Z, endPos.X, endPos.Y, endPos.Z);
            debugf(L"update location:%f, %f, %f", checkPos.X, checkPos.Y, checkPos.Z);
            debugf(L"----------------------------------------------------");
            return;
        }
    }

    //Calculation result log after truncating to integer
    INT32 dwValueX = (INT32)endPos.X - (INT32)startPos.X;
    INT32 dwValueY = (INT32)endPos.Y - (INT32)startPos.Y;
    if(abs(dwValueX) > abs(dwValueY))
    {
        if(dwValueX == 0)
            return;

        INT32 nCheckY = dwValueY * ((INT32)checkPos.X - (INT32)startPos.X)/dwValueX + (INT32)startPos.Y;
        if(abs((INT32)checkPos.Y - nCheckY) > 1)
        {
            debugf(L"----------------------------------------------------");
            debugf(L"the Y offset is %d", (INT32)checkPos.Y - nCheckY);
            debugf(L"src[%d,%d,%d]=====>tgt[%d,%d,%d]", (INT32)startPos.X, (INT32)startPos.Y, (INT32)startPos.Z, (INT32)endPos.X, (INT32)endPos.Y, (INT32)endPos.Z);
            debugf(L"update location:%d, %d, %d", (INT32)checkPos.X, (INT32)checkPos.Y, (INT32)checkPos.Z);

            FLOAT fCheckY = fValueY * (checkPos.X - startPos.X)/fValueX + startPos.Y;
            debugf(L"the Float Y offset is %f", checkPos.Y - fCheckY);
            debugf(L"src[%f,%f,%f]=====>tgt[%f,%f,%f]", startPos.X, startPos.Y, startPos.Z, endPos.X, endPos.Y, endPos.Z);
            debugf(L"update location:%f, %f, %f", checkPos.X, checkPos.Y, checkPos.Z);
            debugf(L"----------------------------------------------------");
        }
    }
    else
    {
        if(dwValueY == 0)
            return;

        INT32 nCheckX = dwValueX * ((INT32)checkPos.Y - (INT32)startPos.Y)/dwValueY + (INT32)startPos.X;
        if(abs((INT32)checkPos.X - nCheckX) > 1)
        {
            debugf(L"----------------------------------------------------");
                debugf(L"the X offset is %d", (INT32)checkPos.X - nCheckX);
            debugf(L"src[%d,%d,%d]=====>tgt[%d,%d,%d]", (INT32)startPos.X, (INT32)startPos.Y, (INT32)startPos.Z, (INT32)endPos.X, (INT32)endPos.Y, (INT32)endPos.Z);
            debugf(L"update location:%d, %d, %d", (INT32)checkPos.X, (INT32)checkPos.Y, (INT32)checkPos.Z);
                
            FLOAT fCheckX = fValueX * (checkPos.Y - startPos.Y)/fValueY + startPos.X;
            debugf(L"the float X offset is %f", checkPos.X - fCheckX);
            debugf(L"src[%f,%f,%f]=====>tgt[%f,%f,%f]", startPos.X, startPos.Y, startPos.Z, endPos.X, endPos.Y, endPos.Z);
            debugf(L"update location:%f, %f, %f", checkPos.X, checkPos.Y, checkPos.Z);
            debugf(L"----------------------------------------------------");
        }
    }
}
//------------------------------------------------------------------------
void CMessageHandler::Send_MoveTo_Mir2( INT objServerID, const FVector& currentPos, const FVector& targetPos, UBOOL bRunOrWalk )
{
	if(_lockPlayer)
	{
		return;
	}

#if SHOW_MOVEMENT_LOG
	debugf(TEXT("Send MoveTo Src=%f,%f  Dst=%f,%f"), currentPos.X, currentPos.Y, targetPos.X, targetPos.Y);
#endif

	FVector serverPos = targetPos;
	ConvertCoordC2N(serverPos);

	MMOFacade::getSingleton().UpdateTimeStamp();

	if(bRunOrWalk){
		C2G_ROLE_RUN msg;
		msg.wMsg = c2g_role_run;
		msg.wDestX = INT(serverPos.X);
		msg.wDestY = INT(serverPos.Y);
		_Send(&msg, sizeof(msg));
	}
	else
	{
		C2G_ROLE_WALK msg;
		msg.wMsg = c2g_role_walk;
		msg.wDestX = INT(serverPos.X);
		msg.wDestY = INT(serverPos.Y);
		_Send(&msg, sizeof(msg));
	}
}

void CMessageHandler::SendRoleMine(const FVector& targetPos)
{
	FVector serverPos = targetPos;
	ConvertCoordC2N(serverPos);

	C2G_ROLE_MINE msg;
	msg.wMsg = c2g_role_mine;
	msg.wDestX = INT(serverPos.X);
	msg.wDestY = INT(serverPos.Y);
	_Send(&msg, sizeof(msg));
}



void CMessageHandler::SendStopMove()
{
	C2G_ROLE_STOP msg;
	msg.wMsg = c2g_role_stop;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GatePing()
{
	C2T_PING msg;
	msg.wMsg = c2t_ping;
	msg.dwPingTick = (UINT32)(appSeconds() * 1000.0f);
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GamePing()
{
    if (MMO::GetCurrentGameState() >= EGameState_Loading)
    {
	    C2G_PING msg;
	    msg.wMsg = c2g_ping;
	    msg.dwPingTick = (UINT32)(appSeconds() * 1000.0f);
	    _Send(&msg, sizeof(msg));
    }
}

void CMessageHandler::Send_LoadFinish()
{
    if (MMO::GetCurrentGameState() >= EGameState_Loading)
    {
		warnf(NAME_Warning, TEXT("[CMessageHandler::Send_LoadFinish] OK"));

        C2G_LOADFINISH msg;
        msg.wMsg = c2g_loadfinish;
        msg.nErrCode = errSuccess;
        _Send(&msg, sizeof(msg));
    }
	else
	{
		warnf(NAME_Error, TEXT("[CMessageHandler::Send_LoadFinish] failed, CurrentGameState:%d"), (INT)MMO::GetCurrentGameState());
	}
}
//------------------------------------------------------------------------
void CMessageHandler::Send_ReturnLogin()
{
	/*
    //Save once on logout or switching account/character
    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
    if( ctrl )
        ctrl->eventSaveSlotBind();
        */
	Logout();
}

UBOOL CMessageHandler::IsConnection()
{
	if(m_pSocketSessionGate)
	{
		return TRUE;
	}
	return FALSE;
}

void CMessageHandler::CloseConnection(bool byUser)
{
	g_disconnectByUser = byUser;
	if (m_pSocketSessionGate)
	{		
		//m_pSocketSessionGate->Close();
	}
}

void CMessageHandler::Logout()
{
	if (m_pSocketSessionGate)
	{		
		C2G_LOGOUT msg;
		msg.wMsg = c2g_logout;
        warnf(NAME_Log, TEXT("====== [CMessageHandler::Logout()") );
		_Send(&msg, sizeof(C2G_LOGOUT));
	}
    else
    {
        warnf(NAME_Error, TEXT("====== [CMessageHandler::Can't Logout m_pSocketSessionGate is NULL") );
    }
}

//----------------------------------------------------
void CMessageHandler::Send_ReturnSelRole()
{
    C2G_RETURN_SELECT_ROLE msg;
    AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
    msg.wMsg = c2g_return_select_role;
    msg.dwRoleID = player->ServerID;
    _Send(&msg, sizeof(msg));

	SetDisableSend(true); // After this message is sent, no more messages may be sent, otherwise the connection drops
}

//==============================================================================
void CMessageHandler::Send_RechargeGameTime( INT setType )
{
	C2M_BUY_TIME msg;
	msg.wMsg = c2m_buy_time;
	msg.dwBuyTimeCost = setType;

	_Send( &msg , sizeof(C2M_BUY_TIME) );
}
//---------------------------------------------------------------------
void CMessageHandler::Parse_m2c_free_mode(const char* buf, UINT32 size)
{
	M2C_FREE_MODE* msg = (M2C_FREE_MODE*)buf;
	if( msg == NULL ){
		return;
	}

	warnf( NAME_Log, TEXT("Parse_m2c_free_mode current game FREE mode is %d"), msg->byFreeMode );

	MMOGameEvent_OneInt gamePlayStateChangeEvt( GameEvent_GamePlayStateChange, msg->byFreeMode );
	MMOUI::PostGameEvent( &gamePlayStateChangeEvt );
}

//==============================================================================
void CMessageHandler::Send_ApplySpecialExp( UINT64 applyCnt )
{
	C2G_ADD_EXERCISE_TIME msg;
	msg.wMsg = c2g_add_exercise_time;

	msg.nHour = (INT)applyCnt;
	_Send( &msg , sizeof(C2G_ADD_EXERCISE_TIME) );
}

//------------------------------------------------------------------------
void CMessageHandler::Send_GetCurAllSpecialExp()
{
	C2G_GET_EXERCISE_EXP msg;
	msg.wMsg = c2g_get_exercise_exp;

	_Send( &msg , sizeof(C2G_GET_EXERCISE_EXP) );
}

//------------------------------------------------------------------------
void CMessageHandler::Send_GetCurAllSpecialExpInfo()
{
	C2G_EXERCISE_INFO msg;
	msg.wMsg = c2g_exercise_info;

	_Send( &msg , sizeof(C2G_EXERCISE_INFO) );
}
//=======================================================================
void CMessageHandler::Parse_g2c_exercise_info(const char* buf, UINT32 size)
{
	G2C_EXERCISE_INFO* msg = (G2C_EXERCISE_INFO*)buf;
	if( !msg ){
		return;
	}


	UMMOUISpecialExpApply* panel = (UMMOUISpecialExpApply*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_SpecialExpApply );
	if( panel != NULL ){
		panel->TryToGetCurAllSpecialExp( msg->nCurHour, msg->nTotalHour );
	}
}
//=======================================================================
void CMessageHandler::Parse_g2c_get_exercise_exp(const char* buf, UINT32 size)
{
	G2C_GET_EXERCISE_EXP* msg = (G2C_GET_EXERCISE_EXP*)buf;
	if( !msg ){
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL){
		return;
	}
	
	if( msg->nExp > 0 ){
		mainPlayer->PlayReviveEffect( mainPlayer->GetFootLocation(), mainPlayer->Rotation );		
	}
}

//=======================================================================
void CMessageHandler::Parse_g2c_add_exercise_time(const char* buf, UINT32 size)
{
	G2C_ADD_EXERCISE_TIME* msg = (G2C_ADD_EXERCISE_TIME*)buf;
	if( !msg ){
		return;
	}

	MMOGameEvent_OneInt curSpecialExp( GameEvent_ApplySpecialExpRst, (INT32)msg->nTotalHour );
	MMOUI::PostGameEvent( &curSpecialExp );
}

////---------------------------------------------------------------------
void CMessageHandler::Send_BuyShopItem(INT ShopItemId , INT ItemCount , INT ItemTempId,INT ShopId )
{
    C2G_BUY_ITEM msg;
    msg.wMsg = c2g_buy_item; 
    msg.shItemCount = ItemCount;
    msg.nShopItemIndex = ShopItemId;
	msg.nShopID = ShopId;
    _Send(&msg, sizeof(msg));
}


////------------------------------------------------------------------------
void CMessageHandler::Send_SelMapDifficulty(int SelDifficulty)
{
    //C2G_SELECT_SCENE_DIFFICULTY msg;
    //msg.wMsg = c2g_select_scene_difficulty;
    //msg.bySelectedDifficulty = SelDifficulty;
    //_Send(&msg, sizeof(msg));
}
////------------------------------------------------------------------------
void CMessageHandler::Send_EnterDungeon(int SelDifficulty)
{
    C2G_ENTER_DUNGEON msg;
    msg.wMsg = c2g_enter_dungeon;
    msg.bySelectedDifficulty = SelDifficulty;
    _Send(&msg, sizeof(msg));
}
//Replay dungeon
void CMessageHandler::Send_PlayFBAgain()
{
	//C2G_DUNGEON_REPLAY msg;
	//msg.wMsg = c2g_dungeon_replay;

	//_Send( &msg, sizeof(msg) );
}
//Leave dungeon
void CMessageHandler::Send_LeaveScene()
{
	C2G_LEAVE_SCENE msg;
	msg.wMsg = c2g_leave_scene;
	_Send( &msg, sizeof(msg) );
}
//Lottery
void CMessageHandler::Send_GetAward(BYTE byIdx)
{
	//C2G_DUNGEON_SELECT_REWARD msg;
	//msg.wMsg = c2g_dungeon_select_reward;
	//msg.byRewardType = byIdx;
	//_Send( &msg, sizeof(msg) );

	////G2C_DUNGEON_REWARD testData;
	////testData.dwRoleID = MMOFacade::getSingleton().GetMainPlayer()->ServerID;
	////testData.byRewardType = 125;
	////testData.dwRewardID = 125;
	////testData.dwRewardCount = 1;
	////Parse_g2c_dungeon_reward( (const char*)&testData, sizeof(G2C_DUNGEON_REWARD) );
}
//void CMessageHandler::_OnRecv( const char* pBuf, UINT32 dwLen )
//{
//#if STATS
//    ++_ReceivedMsgCount;
//    _ReceivedBytes += dwLen;
//#endif
//
//    STNetMsgHeader* pNetMsgHeader = (STNetMsgHeader*)pBuf;
//    UINT16 wMsgID = SGDP::SDNtohs(pNetMsgHeader->wMsgID);
//    UINT32 dwDataLen = SGDP::SDNtohl(pNetMsgHeader->dwDataLen);
//    INT32 nPkgLen = dwDataLen + sizeof(STNetMsgHeader);
//
//    //debugIDf( TEXT("Network"), TEXT("=============== [CMessageHandler::OnRecv] len[%d] Msg[%d]"), dwLen, wMsgID );
//
//
//    if(nPkgLen < 0)
//    {
//        warnf(NAME_Error, TEXT("====== [CMessageHandler::OnRecv] Msg[%d] package length < 0!"), wMsgID );
//        return;
//    }
//
//    if(nPkgLen < (INT32)dwLen)
//    {
//        warnf( TEXT("====== [CMessageHandler::OnRecv] Msg[%d] package length < dwLen!"), wMsgID );
//        return;
//    }
//
//    CHAR DecodeData[PACKAGE_LENGTH] = {0};
//    static CSDProtocol *pProtocol = CProtoCli::Instance();// CProtocolMgr::Instance()->FindProtocol(wMsgID);
//    CHAR* cpMsgData = (CHAR*)(pBuf);
//    pProtocol->Decode(wMsgID, (CHAR*)(pBuf + sizeof(STNetMsgHeader)), dwDataLen, DecodeData, PACKAGE_LENGTH);
//
//    //++g_LogicID;
//
//    ParseFunctionMap::iterator it = _parseFunctionMap.find( wMsgID );
//    if(it != _parseFunctionMap.end())
//    {
//        //warnf(NAME_Warning, TEXT("=============== [CMessageHandler::OnRecv] Parse Msg:[%d]"), wMsgID );
//
//        (this->*(it->second))(DecodeData);
//    }
//    else
//    {
//        warnf(NAME_Error, TEXT("=============== [CMessageHandler::OnRecv] Msg parse function not found:[%d]"), 
//            wMsgID );
//    }
//}

//------------------------------------------------------------------------
void CMessageHandler::Send_GMCommand( const TCHAR* cmd )
{
    C2M_GM_COMMAND* msg = (C2M_GM_COMMAND*)ms_MsgBuffer;
    FTCHARToUTF8 szCmd(cmd);

    msg->wMsg = c2m_gm_command;

    AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if (actor)
	{
		strxcpy( msg->szRoleName, *(actor->ActorName), ROLE_NAME_LEN);
	}

	strxcpy( msg->szGMCommand , szCmd , szCmd.Length()+1 );
	msg->wSize = sizeof(C2M_GM_COMMAND) + szCmd.Length() + 1;

   // strxcpy(msg->szGMCommand, szCmd, MAX_GM_COMMAND_LEN);

    _Send(msg, msg->wSize);
}

void CMessageHandler::Send_Teleport( INT srcTeleportID )
{
    //_lockPlayer = true;
    //MMOFacade::getSingleton().GetMainController()->StopMove();
    //MMOFacade::getSingleton().GetMainController()->StopLatentExecution();
    //MMOFacade::getSingleton().GetMainController()->GotoState(FName(TEXT("Loading")));

	if(MMOFacade::getSingleton().GetMainController())
	{
		//MMOFacade::getSingleton().GetMainControllerAI()->EndCmdAbility();
		//MMOTargetManager::getSingleton().ClearDesireTarget();
		FSMIO::getSingleton().Target.ClearDesireTarget(NULL);
		MMOFacade::getSingleton().GetMainController()->StopMove();
	}
    //MMOFacade::getSingleton().GetMainController()->StopMove();

    C2G_TELEPORT msg;
    msg.wMsg = c2g_teleport;
	
    msg.dwTeleportID = srcTeleportID;
    _Send(&msg, sizeof(msg));
}

// Special command add by zhangrui05
//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Send_RoleTeleport(UINT16 posX, UINT16 posY)
{
	C2G_ROLE_TELEPORT msg;
	msg.wMsg =  c2g_role_teleport;
	msg.wDestX = posX;
	msg.wDestY = posY;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_QueryRoleLocation(const FString& roleName)
{
	INT32 nRoleNameSize = 0;
	FTCHARToUTF8 szRoleName(*roleName);
	nRoleNameSize = szRoleName.Length() + 1;
	if (nRoleNameSize > ROLE_NAME_LEN)
	{
		return;
	}

	C2G_QUERY_ROLE_LOCATION* msg = (C2G_QUERY_ROLE_LOCATION*)ms_MsgBuffer;
	msg->wMsg = c2g_query_role_location;
	strxcpy(msg->szRoleName, *roleName, ROLE_NAME_LEN);
	msg->wSize = sizeof(C2G_QUERY_ROLE_LOCATION) + nRoleNameSize;
	_Send(msg, msg->wSize);
}

void  CMessageHandler::Send_Dice_bet(UINT16 type, UINT16 count)
{
	C2G_DICE_BET msg;
	msg.wMsg =  c2g_dice_bet;
	msg.bytype = type;
	msg.bycount = count;
	warnf(TEXT("Protocol size %d"), sizeof(msg));
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RecallTeamMember()
{
	C2G_RECALL_TEAM_MEMBER msg;
	msg.wMsg =  c2g_recall_team_member;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ChangeRecallState(UBOOL enable)
{
	C2M_CHANGE_RECALL_STATE msg;
	msg.wMsg =  c2m_change_recall_state;
	msg.byEnabled = enable;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_Lovers_Teleport()
{
	C2G_LOVERS_TELEPROT msg;
	msg.wMsg =  c2g_lovers_teleprot;
	_Send(&msg, sizeof(msg));
}
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//Equip title
void CMessageHandler::Send_EquipTitle(BYTE byIdx)
{
	if(byIdx>0){
		C2G_TITLE_EQUIP msg;
		msg.wMsg =  c2g_title_equip;
		msg.nTemplateTitleID = byIdx;
		_Send( &msg, sizeof(msg) );
	}
	else
	{
		C2G_TITLE_UNEQUIP msg;
		msg.wMsg =  c2g_title_unequip;
		_Send( &msg, sizeof(msg) );
	}
}

//Unequip title
void CMessageHandler::Send_UnEquipTitle()
{
	C2G_TITLE_UNEQUIP msg;
	msg.wMsg =  c2g_title_unequip;
	_Send( &msg, sizeof(msg) );
}

void CMessageHandler::Send_ActiveMount(INT MountID)
{
//	C2G_ACTIVATE_MOUNT msg;
//	msg.wMsg =  c2g_activate_mount;
//	msg.nMountID = MountID;
//	_Send( &msg, sizeof(msg) );
}


//----------------------------------------------------------------------------------------------
void CMessageHandler::Send_Clear_Spell_Cache()
{
	C2G_CLEAR_SPELL_CACHE msg;
	msg.wMsg =  c2g_clear_spell_cache;

	g_SendClearAbilityCacheTimes += 1;
	g_UpdateClearAbilityCacheTime = 1.0f;
	msg.nTag =  g_SendClearAbilityCacheTimes;

	_Send( &msg, sizeof(msg) );
}

void CMessageHandler::Send_EquipMount(INT MountID)
{
	MMOFacade::getSingleton().GetMainController()->WaitingForEquipMount = 0.2;
	if(MountID>0)
	{
		C2G_ON_MOUNT msg;
		msg.wMsg =  c2g_on_mount;
		msg.nMountID = MountID;
		_Send( &msg, sizeof(msg) );
	}
	else
	{
		C2G_OFF_MOUNT msg;
		msg.wMsg =  c2g_off_mount;
		_Send( &msg, sizeof(msg) );
	}
}

void CMessageHandler::Send_WingOp(INT Op, INT Param)
{
	C2G_WING_OP msg;
	msg.wMsg = c2g_wing_op;
	msg.nOp = Op;
	msg.nParam = Param;
	_Send( &msg, sizeof(msg) );
}


void CMessageHandler::Send_ReceiveReputationAward(INT nForceID)
{
	C2G_RECEIVE_REPUTATION_AWARD  msg;
	msg.wMsg = c2g_receive_reputation_award;
	msg.byForceID = (UINT8)nForceID;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_Change_Pet_State(UINT8 byState)
{
	C2G_CHANGE_PET_STATE  msg;
	msg.wMsg = c2g_change_pet_state;
	msg.byState = byState;
	_Send(&msg, sizeof(msg));
}



//---------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_error_notify(const char* buf, UINT32 size)
{
	M2C_ERROR_NOTIFY* msg = (M2C_ERROR_NOTIFY*)buf;

	// If the server handled it and returned to the login screen, do not continue
	if(_ProcessReturnLoginError(msg->nErrorCode, msg->nParam1, msg->nParam2))
	{
		return;
	}

	MMOGameEvent_ServerError errorEvt( msg->nErrorCode, msg->nParam1, msg->nParam2 );
	MMOUI::PostGameEvent( &errorEvt );

	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);

	//FString enumVal = InEnum->GetEnum(msg->nErrorCode).ToString();
	FString enumVal = GetErrorCodeEnumString(msg->nErrorCode); // FString enumVal(g_szErrorCodeNameList[msg->nErrorCode]);
    FString error_ids = FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal);
	
	UBOOL parseRet = FALSE;
	FString localVal = MMOUI::ParseLocalizedPropertyPath(error_ids, parseRet);

	warnf(NAME_Error, TEXT("[CMessageHandler::Parse_m2c_error_notify] Error ID:[%d] %s"),
		msg->nErrorCode, *localVal);

	if (msg->nErrorCode >= errSpellBegin && msg->nErrorCode <= errSpellEnd)
	{
		MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvError(msg->nErrorCode, msg->nParam1, msg->nParam2);
	}

	if(msg->nErrorCode == errSpellCasting )//&& (msg->nParam1==4000 || msg->nParam1==4001))
	{
		return;
	}

	if (msg->nErrorCode == errMasterHighLevel || msg->nErrorCode == errItemNotFound  || msg->nErrorCode == errArenaMatchFailed )
		return;

	if (msg->nErrorCode >= errAchievementLevel && msg->nErrorCode <= errAchievementSpiritLevel)
	{
		// Achievement errors need no prompt
		return;
	}

	if (msg->nErrorCode ==errChatTooFrequent)
	{
		return;
	}

	/*if (msg->nErrorCode == errChatForbidden)
	{
		MMOChatManager::getSingleton().AddChatMessage(ECC_System,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatForbidden")),
			TEXT(""));
		return;
	}*/

	/*if (msg->nErrorCode == errChatSafeAreaForbidden)
	{
		MMOChatManager::getSingleton().AddChatMessage(ECC_System,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatSafeAreaForbidden")),
			TEXT(""));
		return;
	}*/

	if( msg->nErrorCode == errItemNotEnoughMoneytype01 )
	{
		//Insufficient bound gold
		UBOOL sel = MMOFacade::getSingleton().GetChainPay();
		if(!sel)
		{
			//MMOMsgBoxManager::getSingleton().AddOpenChainPayMessageBox();
			//MMOMsgBoxManager::getSingleton().AddModalMessageBoxEvent( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.AskOpenChainPay")) , 0 , GameEvent_OpenChainPay );
		}
	}

	if (msg->nErrorCode == errAuctionItemNotFound || msg->nErrorCode == errAuctionBuyingPriceLow || msg->nErrorCode ==errAuctionSellingPriceLow)
	{
		//Refresh UI
		MMOAuctionManager::getSingleton().NotifyUpdateCurrentPageInfo();
	}

	if (msg->nErrorCode == errRoleNotOnline)
	{
		UMMOUIBase* ui = MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_RankList );
		if(ui && (ui->IsVisible == FALSE))
		{
			return;
		}
	}

	if ( msg->nErrorCode == errChatTargetNotFound || msg->nErrorCode == errChatCountOut || 
		 msg->nErrorCode == errChatLevelTooLow ||  msg->nErrorCode == errChatMoneyNotEnough||
		 msg->nErrorCode == errChatForbidden ||  msg->nErrorCode ==  errChatSafeAreaForbidden ||
		 msg->nErrorCode == errGroupChatWithItem 
		 )
	{
		MMOChatManager::getSingleton().AddChatErrorMsg(error_ids);
		return;
	}

	if (msg->nErrorCode == errGuildQueryFailed)
	{
		MMOGuildManager::getSingleton().NotifyGuildQueryInvalidId(msg->nParam1);
		return;
	}

	if (msg->nErrorCode == errGuildDelAllianceSuccess)
	{
		MMOGuildManager::getSingleton().NotifyDelGuildAllySuccess(msg->nParam1);
		return;
	}

	if (msg->nErrorCode == errAwardFailed)
	{
		FString error = FString::Printf( TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAwardFailed_%d") , abs(msg->nParam1) );
		FString Msg = MMOUI::ParseLocalizedPropertyPath( error );
		MMOUI::GetUIManager()->delegateAddScreenMessage( Msg , MsgInfoType_Warning ,error);
		return;
	}

	if (msg->nErrorCode == errLoginNeedDynamicPassword)
	{
		MMODynamicPwdManager::getSingleton().NotifyShowDynamicPassword();
		return;
	}

	if (msg->nErrorCode == errGuildPageEnd)
	{
		MMOGuildManager::getSingleton().NotifyNoMoreGuildWarRecords();
		return;
	}

#if SHIPPING_PC_GAME
	if(!parseRet)
	{
		return;
	}
#endif

	//MMOUI::GetUIManager()->delegateAddScreenMessage(error_ids);
	//
	MMOUI::GetUIManager()->delegateAddScreenMessage( localVal , MMOUI::FindMsgTypeByLocalString(error_ids) ,error_ids);
}
//---------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_error_notify(const char* buf, UINT32 size)
{
    G2C_ERROR_NOTIFY* msg = (G2C_ERROR_NOTIFY*)buf;

	// If the server handled it and returned to the login screen, do not continue
	if(_ProcessReturnLoginError(msg->nErrCode, msg->nParam1, msg->nParam2))
	{
		return;
	}

	if (msg->nErrCode == errGpkAuthKick)
	{
		MMOMsgBoxManager::getSingleton().ClearAll();

		AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = pc;
		funYes.FunctionName = TEXT("ExitGameWindow");
		FString localVal = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGpkAuthKick"));

		MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal, 0, funYes, funYes, FALSE);
		g_bOnReturnLoginConfirm = true;
		return;
	}
	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);

	//FString enumVal = InEnum->GetEnum(msg->nErrCode).ToString();

	FString enumVal = GetErrorCodeEnumString(msg->nErrCode); // FString enumVal(g_szErrorCodeNameList[msg->nErrCode]);
    FString error_ids = FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal);

	UBOOL parseRet = FALSE;
	FString localVal = MMOUI::ParseLocalizedPropertyPath(error_ids, parseRet);

	if (msg->nErrCode == errMasterHighLevel || msg->nErrCode == errItemNotFound)
		return;
		 
    if (msg->nErrCode >= errSpellBegin && msg->nErrCode <= errSpellEnd)
    {
        MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvError(msg->nErrCode, msg->nParam1, msg->nParam2);
	}
	else if (msg->nErrCode >=errSpiritNotOpen && msg->nErrCode <= errSpiritAirEquipSameGroup)
	{
		MMOHeavenManager::getSingleton().ParseHeavenError(msg->nErrCode, msg->nParam1, msg->nParam2, localVal);
	}
	warnf(NAME_Error, TEXT("[CMessageHandler::Parse_g2c_error_notify] Error ID:[%d] %s"), msg->nErrCode, *localVal);
   

	if(msg->nErrCode == errSpellCasting )//&& (msg->nParam1==4000 || msg->nParam1==4001))
	{
		return;
	}
	 //Special error, do not display

	//Auto pickup 1, reply has a time gap; if scan speed slows items may be missed 2, allocation problem: Client has no drop item ownership
	if( ( msg->nErrCode == errDropNotFound ||  msg->nErrCode == errDropProtected || msg->nErrCode == errDropFaraway || msg->nErrCode == errDropOtherStand ) 
		&& msg->nParam1 >0 )
	{
		//Do not pick up other people's items from now on
		MMODropManager::getSingleton().SetDropActorCannotPick(msg->nParam1,2.0f);
		//return;
	}
	//if( ( msg->nErrCode == errDropNotFound ||  msg->nErrCode == errDropProtected || msg->nErrCode == errDropFaraway || msg->nErrCode == errDropOtherStand ) 
	//	&& msg->nParam2 == TRUE )
	//{
	//	//Do not pick up other people's items from now on
	//	if(msg->nErrCode == errDropProtected)
	//		MMODropManager::getSingleton().SetDropActorCannotPick(msg->nParam1);
	//	return;
	//}

	//if(msg->nErrCode==errItemBagFull && msg->nParam1>0)
	//{
	//	MMODropManager::getSingleton().SetDropActorCannotPick(msg->nParam1);
	//	return;
	//}
	

	if( msg->nErrCode == errTeamApplicationRefused || msg->nErrCode == errTeamInvitationRefused || msg->nErrCode == errTeamRefusedInvitation || msg->nErrCode == errTeamRefusedApplication )
	{
		//Declined your team request
		TeamManager::getSingleton().RefuseTeamAdd( msg->nParam1 , msg->nParam2 );
		return;
	}

	if( msg->nErrCode == errItemNotEnoughMoneytype01 )
	{
		//Insufficient bound gold
		UBOOL sel = MMOFacade::getSingleton().GetChainPay();
		if(!sel)
		{
			//MMOMsgBoxManager::getSingleton().AddOpenChainPayMessageBox();
			//MMOMsgBoxManager::getSingleton().AddModalMessageBoxEvent( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.AskOpenChainPay")) , 0 , GameEvent_OpenChainPay );
		}
	}

	if (msg->nErrCode >= errItemBagFull && msg->nErrCode <= errItemUnknown)
	{
		MMOUI::GetUIManager()->SetItemSlotWaitingState(FALSE);
	}

	if (msg->nErrCode >= errAchievementLevel && msg->nErrCode <= errAchievementSpiritLevel)
	{
		// Achievement errors need no prompt
		return;
	}
	if (msg->nErrCode == errAuctionItemNotFound || msg->nErrCode == errAuctionBuyingPriceLow || msg->nErrCode ==errAuctionSellingPriceLow)
	{
		//Refresh UI
		MMOAuctionManager::getSingleton().NotifyUpdateCurrentPageInfo();
	}

	if (msg->nErrCode == errRoleNotOnline)
	{
		UMMOUIBase* ui = MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_RankList );
		if(ui && (ui->IsVisible == FALSE))
		{
			return;
		}
	}

	if( msg->nErrCode == errRepReceiveAwardSuccess)
	{
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			MMOFacade::getSingleton().SetPlayerAwardState(mainPlayer, msg->nParam1, msg->nParam1, true);
			MMOUI::GetUIManager()->delegateUpdateReputationData();
			MMOUI::GetUIManager()->delegateRefreshReputationSelected();

			if (MMOFacade::getSingleton().GetPlayerAwardState(mainPlayer, msg->nParam1))
			{
				JourneyData data;
				if (MMOJourneyData::getSingleton().GetJourneyDataByReputation(msg->nParam1, data))
				{
					UMMOUIJourneyCompletePanel* panel = (UMMOUIJourneyCompletePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_JourneyComplete);
					if (panel)
					{
						panel->Show(TRUE);
						panel->eventCallJourneyComplete(data.Title);
					}
				}
			}
		}

		return;
	}
	
	if( msg->nErrCode == errMoveFail)
	{
		if(g_PreMovetoPoint){
			warnf(TEXT("---------------------------errMoveFail"));
		}

		MMOFacade::getSingleton().GetMainController()->WaitingForTheMoveToRespond = -1.0f;
		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
		return;
	}

	if( msg->nErrCode == errDropNotFound || msg->nErrCode == errDropFaraway )
	{
		return;
	}

	if( msg->nErrCode == errSpellCastNoMove || msg->nErrCode ==errSpellNotReady )
	{
		return;
	}

	if( msg->nErrCode == errItemWeekMoneyExceeded)
	{
		UBOOL   Res; 
		FString MoneyEnum		= MMOFacade::getSingleton().FindServerEnumString( msg->nParam1, TEXT("EMONEY_TYPE") );
		FString MoneyEnumName	= FString(TEXT("MMOGame.MoneyUI.")) + MoneyEnum;
		FString MoneyName		= MMOUI::ParseLocalizedPropertyPath( MoneyEnumName , Res);
		FString Msg				= FString::Printf(LocalizeSecure(localVal, *MoneyName));	//   FString::Printf(_TEXT("%f"), *MoneyName);  //MMOUI::ParseLocalizedPropertyPath( MoneyEnumName , Res);

		MMOUI::GetUIManager()->delegateAddScreenMessage( Msg , MsgInfoType_Warning ,error_ids);
		return;
	}

	if(msg->nErrCode==errSpellCasterCheckNodeFailed)
	{
		UBOOL   Res; 
		FString MsgKey	=  FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errSpellCasterCheckNodeFailed%d"),msg->nParam1);
		FString Msg		=  MMOUI::ParseLocalizedPropertyPath( MsgKey , Res);
		MMOUI::GetUIManager()->delegateAddScreenMessage( Msg , MsgInfoType_Warning ,error_ids);
		return;
	}

	if( msg->nErrCode==errPetChangeStateSuccess)
	{
		UBOOL   Res; 
		FString Tips;
		switch(msg->nParam1)
		{
		case E_ps_Invalid:
			//return;
			//Tips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EPET_STATE.E_ps_Invalid") , Res);
			break;
		case E_ps_Idle:
			Tips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EPET_STATE.E_ps_Idle") , Res);
			break;
		case E_ps_Fight:
			Tips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EPET_STATE.E_ps_Fight") , Res);
			break;
		}
		MMOUI::GetUIManager()->delegateUpdatePetState(msg->nParam1);
		if(Tips.Len()>0){
			MMOUI::GetUIManager()->delegateAddScreenMessage( Tips , MsgInfoType_Normal ,error_ids);
			MMOChatManager::getSingleton().AddSystemMessage(Tips);
		}
		return;
	}

	if(  msg->nErrCode==errAuraNotExist || msg->nErrCode==errAuraConfigError || msg->nErrCode==errAuraAppendFailed)
	{
		return;
	}
	if( msg->nErrCode==errAuraResisted )//|| msg->nErrCode==errAuraNotExist || msg->nErrCode==errAuraConfigError || msg->nErrCode==errAuraAppendFailed)
	{
		UBOOL   Res; 
		FString MsgKey	=  error_ids + FString::Printf(TEXT("%d"),msg->nParam1);
		FString Msg		=  MMOUI::ParseLocalizedPropertyPath( MsgKey , Res);
		MMOUI::GetUIManager()->delegateAddScreenMessage( Msg , MsgInfoType_Warning ,error_ids);
		return;
	}

	if (msg->nErrCode == errGuildAchiRewardSucc)
	{
		UINT32 id = ACHIEVEMENT_MAX + msg->nParam1;
		MMOAchievementManager::getSingleton().NotifyAchievementRewardObtain(id);
	}

	if ( msg->nErrCode == errChatTargetNotFound || msg->nErrCode == errChatCountOut || 
		msg->nErrCode == errChatLevelTooLow ||  msg->nErrCode == errChatMoneyNotEnough||
		msg->nErrCode == errChatForbidden ||  msg->nErrCode ==  errChatSafeAreaForbidden ||
		msg->nErrCode == errGroupChatWithItem 
		)
	{
		MMOChatManager::getSingleton().AddChatErrorMsg(error_ids);
		return;
	}

	if (msg->nErrCode == errGuildQueryFailed)
	{
		MMOGuildManager::getSingleton().NotifyGuildQueryInvalidId(msg->nParam1);
		return;
	}

	if (msg->nErrCode ==errChatTooFrequent)
	{
		return;
	}

	/*if (msg->nErrCode == errChatForbidden)
	{
		MMOChatManager::getSingleton().AddChatMessage(ECC_System,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatForbidden")),
			TEXT(""));
		return;
	}*/

	/*if (msg->nErrCode == errChatSafeAreaForbidden)
	{
		MMOChatManager::getSingleton().AddChatMessage(ECC_System,
			*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errChatSafeAreaForbidden")),
			TEXT(""));
		return;
	}*/

	if(msg->nErrCode == errLoginNeedDynamicPassword)
	{
		MMODynamicPwdManager::getSingleton().NotifyShowDynamicPassword();
		return;
	}

#if SHIPPING_PC_GAME
	if(!parseRet)
	{
		return;
	}
#endif
	
	MMOGameEvent_ServerError errorEvt( msg->nErrCode, msg->nParam1, msg->nParam2 );
	MMOUI::PostGameEvent( &errorEvt );

	if( msg->nErrCode == errTradeBagFull || msg->nErrCode == errArenaMatchFailed ){
		return;
	}
	//MMOUI::GetUIManager()->delegateAddScreenMessage(error_ids);
	
	if( msg->nErrCode == errItemLevelUpNotFinished ){
		//Weapon upgrade, retrieve error message, error message needs formatting
		UINT leftSec = msg->nParam1 + 59;
		UINT leftHour = leftSec / ( 60*60 );
		FString leftTime = TEXT("");
		if( leftHour > 0 ){
			leftTime = appItoa( leftHour );
			leftTime += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Time.CountDown4") );
		}

		UINT leftMin = ( leftSec - ( leftHour * 60*60 ) ) / 60;
		if( leftMin > 0 ){
			leftTime += appItoa( leftMin );
			leftTime += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Time.CountDown5") );
		}

		FString equipLevelUpError = FString::Printf( *localVal, *leftTime );
		MMOUI::GetUIManager()->delegateAddScreenMessage( equipLevelUpError , MMOUI::FindMsgTypeByLocalString(error_ids) ,error_ids);
		return;
	}

	if (msg->nErrCode == errAntiCheatSuccess || msg->nErrCode == errAntiCheatError || msg->nErrCode == errAntiCheatErrorKick || msg->nErrCode == errAntiCheatErrorTimeoutReturn)
	{
		MMOJigsawManager::getSingleton().OnRetCode(msg->nErrCode , msg->nParam1);
		return;
	}

	MMOUI::GetUIManager()->delegateAddScreenMessage( localVal , MMOUI::FindMsgTypeByLocalString(error_ids) ,error_ids);
}


void CMessageHandler::Parse_g2c_sync_door_state(const char* buf, UINT32 size)
{
	G2C_SYNC_DOOR_STATE* msg = (G2C_SYNC_DOOR_STATE*)buf;
	//Currently defined max 64
	std::bitset<64> doorstate(msg->qwDoorState);
	MMOFacade::getSingleton().SetDoorStates( doorstate );

}

void UpdateTransducer( int Progress )
{
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	AMMOHud* hud = Cast<AMMOHud>(ctrl->myHUD);
	const int DoorOfFateTemplateId = 3001;
	const int TransducerTemplateId = 3002;
	if(hud == NULL || hud->HudMovie == NULL )
		return;

	UMMOUITransducerInfo *_TransducerInfo;
	if( ! MMOUI::GetPanelByString( "Transducer",(UMMOUIBase**) &_TransducerInfo  ) )
		return;
	if( Progress <= 0 || Progress >= 1000 )
	{
		_TransducerInfo->eventUpdateInfo(false,0);
		if( Progress >= 1000 )
		{
			int num = MMOFacade::getSingleton().GetFriendlyNpcNum();
			if( num == 0 )
				return;
			MMOFacade::getSingleton().StartFriendlyNpcFlash();
			for ( int i = 0 ; i< num ; i++ )
			{
				ANPC* npc = MMOFacade::getSingleton().GetFrendlyNpc( i  );
				if( npc == NULL)
					continue;
				if( npc->NPCID == TransducerTemplateId )
				{
					FString tAnimSetName(L"Transducer_gameplay_ani.Transducer_gameplay_Charging");
					//Found the charger
					npc->eventChangeAnimSet( tAnimSetName );
				}
				else if( npc->NPCID == DoorOfFateTemplateId )
				{
					FString tType(L"MaterialInstanceTimeVarying");
					FString tMaterialName( L"Fx_LevelActor.M_PorTal_002_INST" );
					//Material after fully charged
					npc->eventSetToNewMaterialByName( tType, tMaterialName );
				}
			}
			//hud->HudMovie->eventShowMsgSceneHint( "Gate of Fate fully charged, use it to kill the powerful demon lord!!" );
		}
	}
	else
	{
		int num = MMOFacade::getSingleton().GetFriendlyNpcNum();
		if( num == 0 )
			return;
		MMOFacade::getSingleton().StartFriendlyNpcFlash();
		for ( int i = 0 ; i< num ; i++ )
		{
			ANPC* npc = MMOFacade::getSingleton().GetFrendlyNpc( i  );
			if( npc == NULL)
				continue;
			if( npc->NPCID == TransducerTemplateId )
			{
				FString tAnimSetName(L"Transducer_gameplay_ani.Transducer_gameplay_Charging");
				//Found the charger
				npc->eventChangeAnimSet( tAnimSetName );
			}
			else if( npc->NPCID == DoorOfFateTemplateId )
			{
				FString tType(L"MaterialInstanceConstant");
				FString tMaterialName( L"Fx_LevelActor.M_Portal_001_INST" );
				/*FString tType(L"MaterialInstanceTimeVarying");
				FString tMaterialName( L"Fx_LevelActor.M_PorTal_002_INST" );*/
				//Found the Gate of Fate, switch material when first visible
				npc->eventSetToNewMaterialByName(  tType, tMaterialName );
			}
		}	
		_TransducerInfo->eventUpdateInfo(true,Progress);
	}
}

//------------------------------------------------------------------------
//void _SetPlayerMoney( INT money ) 
//{
//	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
//	if(pl)
//	{
//		pl->_Money = money;
//		AMMOHud*  Hud = MMOFacade::getSingleton().GetMyHud();
//		if(Hud && Hud->HudMovie)
//		{
//			Hud->HudMovie->eventSetPlayerMoney( money );
//		}
//	}
//}
//------------------------------------------------------------------------

void CMessageHandler::Parse_g2c_clear_spell_cache(const char* buf, UINT32 size)
{
	G2C_CLEAR_SPELL_CACHE* msg =  (G2C_CLEAR_SPELL_CACHE*)buf;
	g_RecieveClearAbilityCacheTimes = msg->nTag;
	g_UpdateClearAbilityCacheTime=-1.0f;
	FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
}

void CMessageHandler::Parse_g2c_sync_mount_list(const char* buf, UINT32 size)
{
	G2C_SYNC_MOUNT_LIST* msg =  (G2C_SYNC_MOUNT_LIST*)buf;
	INT byCount = (size - sizeof(G2C_SYNC_MOUNT_LIST)) / sizeof(msg->byMountID[0]);
	
	MMOFacade::getSingleton().EmptyMountList();
	for(INT i=0;i<(INT)byCount;i++)
	{
		MMOFacade::getSingleton().AddMountList(msg->byMountID[i]);
	}
	MMOFacade::getSingleton().RefreshMountData();
}

void CMessageHandler::Parse_g2c_activate_mount(const char* buf, UINT32 size)
{
	G2C_ACTIVATE_MOUNT* msg =  (G2C_ACTIVATE_MOUNT*)buf;
	MMOFacade::getSingleton().AddMountList(msg->nMountID);
	MMOFacade::getSingleton().RefreshMountData();
	MountData* pData = MMOFacade::getSingleton().GetMountData(msg->nMountID);
	if(pData){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mount.ActivateMount")) +  pData->Name,MsgInfoType_Table,TEXT("MMOGame.Mount.ActivateMount") );
	}
}

void CMessageHandler::Parse_g2c_char_on_mount(const char* buf, UINT32 size)
{
	G2C_CHAR_ON_MOUNT* msg =  (G2C_CHAR_ON_MOUNT*)buf;
	MMOFacade::getSingleton().EquipMount(msg->dwCharID,msg->byMountID);
	MMOUI::GetUIManager()->delegateRefreshEquipedMount();
}

void CMessageHandler::Parse_g2c_char_off_mount(const char* buf, UINT32 size)
{
	G2C_CHAR_OFF_MOUNT* msg =  (G2C_CHAR_OFF_MOUNT*)buf;
	MMOFacade::getSingleton().EquipMount(msg->dwCharID,0);
	MMOUI::GetUIManager()->delegateRefreshEquipedMount();
}

void CMessageHandler::Parse_g2c_char_wing(const char* buf, UINT32 size)
{
	G2C_CHAR_WING* msg =  (G2C_CHAR_WING*)buf;
	MMOFacade::getSingleton().EquipWing(msg->dwCharID,msg->wWingID);
}

extern void MMOUIWingPanel_OnWingInfo(INT Level, UINT Mask, INT CurrentWing, UBOOL bUpgraded);	// MMOUIWingPanel.cpp
void CMessageHandler::Parse_g2c_wing_info(const char* buf, UINT32 size)
{
	G2C_WING_INFO* msg =  (G2C_WING_INFO*)buf;
	MMOUIWingPanel_OnWingInfo(msg->nWingLevel, msg->dwOwnedMask, msg->wCurrentWing, msg->byUpgraded ? TRUE : FALSE);
}


//hero-------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_hero_data(const char* buf, UINT32 size)
{
	G2C_SYNC_HERO_DATA* msg =  (G2C_SYNC_HERO_DATA*)buf;
	
	INT2SyncHeroDataMap::iterator pos = m_HeroDataCache.find((INT)msg->dwHeroID);
	if (pos == m_HeroDataCache.end())
	{
		G2C_SYNC_HERO_DATA* newPk = new G2C_SYNC_HERO_DATA;
		memcpy(newPk, msg, sizeof(G2C_SYNC_HERO_DATA));
		m_HeroDataCache.insert(make_pair(msg->dwHeroID, newPk));
		SyncHeroData(msg, NULL);
	}
	else
	{
		SyncHeroData(msg, pos->second);
		memcpy(pos->second, msg, sizeof(G2C_SYNC_HERO_DATA));
	}

	 //MMOFacade::getSingleton().SetMainHeroId((INT)msg->dwHeroID);
	
}

void CMessageHandler::Parse_g2c_hero_exp_change(const char* buf, UINT32 size)
{
	G2C_HERO_EXP_CHANGE* msg =  (G2C_HERO_EXP_CHANGE*)buf;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwHeroID);
	if (actor && actor->ActorAttributes)
	{
		actor->ActorAttributes->LevelAttributes.CurrentExp	= msg->dwExp;
		actor->ActorAttributes->LevelAttributes.MaxExp		= msg->dwMaxExp;
	}

	AMMOActor* MainPlayer =MMOFacade::getSingleton().GetMainActor();
	if( MainPlayer && MainPlayer->HeroAttributes )
	{
		MainPlayer->HeroAttributes->LevelAttributes.CurrentExp	= msg->dwExp;
		MainPlayer->HeroAttributes->LevelAttributes.MaxExp		= msg->dwMaxExp;
		MMOUI::GetUIManager()->delegateRefreshHero(FALSE);
		MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
	}

	if(msg->nExpChange<0)
	{
		FString messageString = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Hero.MessageExp"))), -msg->nExpChange);
		MMOUI::GetUIManager()->delegateAddScreenMessage( messageString ,MsgInfoType_Table,TEXT("MMOGame.Hero.MessageExp"));
	}
}

void CMessageHandler::Parse_g2c_hero_level_up(const char* buf, UINT32 size)
{
	G2C_HERO_LEVEL_UP* msg =  (G2C_HERO_LEVEL_UP*)buf;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwHeroID);
	if (actor && actor->ActorAttributes)
	{
		actor->ActorAttributes->LevelAttributes.Level = msg->byLevel;

		AMMOActor* MainPlayer =MMOFacade::getSingleton().GetMainActor();
		if( MainPlayer && MainPlayer->ServerHeroID == msg->dwHeroID )
		{
			MainPlayer->HeroAttributes->LevelAttributes.Level = msg->byLevel;
			MMOUI::GetUIManager()->delegateRefreshHero(FALSE);
			MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
		}
	}
}

void CMessageHandler::Parse_g2c_hero_attr_change(const char* buf, UINT32 size)
{
	G2C_HERO_ATTR_CHANGE* msg =  (G2C_HERO_ATTR_CHANGE*)buf;
	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if (!actor || !actor->HeroAttributes )
	{			
		return;
	}
	actor->SetHeroAttributeValue(msg->byAttrID, msg->nAttrValue);

	MMOUI::GetUIManager()->delegateRefreshHeroAttribute();
	//MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
}

void CMessageHandler::Parse_g2c_sync_hero_attr_list(const char* buf, UINT32 size)
{
	G2C_SYNC_HERO_ATTR_LIST* msg =  (G2C_SYNC_HERO_ATTR_LIST*)buf;
	INT byAttrCount = (size - sizeof(G2C_SYNC_HERO_ATTR_LIST)) / sizeof(msg->astAttrData[0]);

	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if (!actor || !actor->HeroAttributes )
	{			
		return;
	}
	//Player attribute initialization
	actor->HeroAttributes->AttributeList.Empty();
	actor->HeroAttributes->AttributeList.Add( catTotal );

	for (INT i = 0; i < byAttrCount; ++i)
	{
		actor->SetHeroAttributeValue(msg->astAttrData[i].byAttrID, msg->astAttrData[i].nAttrValue);
	}

	MMOUI::GetUIManager()->delegateRefreshHero(FALSE);
	MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
}

void CMessageHandler::Parse_g2c_hire_hero(const char* buf, UINT32 size)
{
	G2C_HIRE_HERO* msg =  (G2C_HIRE_HERO*)buf;
	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if (actor && actor->HeroAttributes)
	{
		actor->ServerHeroID = msg->dwHeroID;
		actor->HeroAttributes->LevelAttributes.Level	= msg->byHeroLevel;
		actor->HeroAttributes->ClassType				= msg->byHeroClass;
		actor->HeroAttributes->GenderType				= msg->byHeroSex;
		actor->HeroAttributes->Growth					= msg->nGrowth;
		actor->HeroName									= TEXT("");
		//Request detailed attributes
		Send_Query_Hero_Attr_List();

		if(EGameState_InGame == MMO::GetCurrentGameState()){
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Hero.MessageHireHero")) ,MsgInfoType_Table,TEXT("MMOGame.Hero.MessageHireHero"));
			//MMOTargetManager::getSingleton().SetDesireTarget(NULL,NULL);
		}
	}
}

void CMessageHandler::Parse_g2c_fire_hero(const char* buf, UINT32 size)
{
	G2C_FIRE_HERO* msg =  (G2C_FIRE_HERO*)buf;
	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if (actor)
	{
		actor->ServerHeroID = 0;
		MMOUI::GetUIManager()->delegateRefreshHero(TRUE);
		MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
	}
}

void CMessageHandler::Parse_g2c_change_hero_ai(const char* buf, UINT32 size)
{
	G2C_CHANGE_HERO_AI* msg =  (G2C_CHANGE_HERO_AI*)buf;
	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if (actor && actor->ServerHeroID == msg->dwHeroID )
	{
		actor->HeroAi = msg->nAiType;
		MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
	}	
}

void CMessageHandler::Parse_g2c_summon_hero(const char* buf, UINT32 size)
{
	G2C_SUMMON_HERO* msg =  (G2C_SUMMON_HERO*)buf;
	
	MMOFacade::getSingleton().SetHeroSummon(TRUE);
	MMOUI::GetUIManager()->delegateRefreshSummon();
	MMOUI::GetUIManager()->delegateHeroInfoUpdateData();
}

void CMessageHandler::Parse_g2c_recall_hero(const char* buf, UINT32 size)
{
	G2C_RECALL_HERO* msg =  (G2C_RECALL_HERO*)buf;

	MMOFacade::getSingleton().SetHeroSummon(FALSE);
	MMOUI::GetUIManager()->delegateRefreshSummon();
	MMOUI::GetUIManager()->delegateHeroInfoUpdateData();

}

void CMessageHandler::Parse_g2c_draw_debug_line(const char* buf, UINT32 size)
{
//#if !SHIPPING_PC_GAME
	G2C_DRAW_DEBUG_LINE* msg =  (G2C_DRAW_DEBUG_LINE*)buf;

	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if(actor)
	{
		FVector PosSrc = FVector(msg->wSrcX, msg->wSrcY, msg->wSrcZ);
		ConvertCoordN2C(PosSrc);

		FVector PosDst = FVector(msg->wDstX, msg->wDstY, msg->wDstZ);
		ConvertCoordN2C(PosDst);

		PosSrc.Z += 100;
		PosDst.Z += 100;

		debugf(_TEXT("Player: %f, %f, %f"), actor->Location.X, actor->Location.Y, actor->Location.Z);
		debugf(_TEXT("Src: %f, %f, %f, dst: %f, %f, %f"), PosSrc.X, PosSrc.Y, PosSrc.Z, PosDst.X, PosDst.Y, PosDst.Z);

		//actor->FlushPersistentDebugLines();
		actor->DrawDebugLine(PosSrc, PosDst, 255, 0, 0, TRUE);
	}

//#endif


}

//
void CMessageHandler::Send_ChainGameMoney( UBOOL openChain )
{
	//C2G_USE_MONEYTYPE01_ONLY msg;
	//msg.wMsg =  c2g_use_moneytype01_only;
	////1 means use bound gold only, 0 means gold can substitute bound gold by wubin
	//msg.byUseMoneytype01Only = !openChain;
	//_Send( &msg, sizeof(msg) );
}

//------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_money(const char* buf, UINT32 size)
{
	G2C_SYNC_MONEY* msg =  (G2C_SYNC_MONEY*)buf;
	if(  msg->byUseMoneytype01Only ^ !MMOFacade::getSingleton().GetChainPay()  )
	{
		//If the server differs from the current joint payment state, change it
		MMOFacade::getSingleton().SetChainPay( !msg->byUseMoneytype01Only );
		MMOGameEvent_OneInt ChainPayChange( GameEvent_ChainPayChange , !msg->byUseMoneytype01Only );
		MMOUI::PostGameEvent( &ChainPayChange );
	}
	
	//debugf( TEXT("[CMessageHandler::g2c_sync_money] %d"), msg->anMoneys );
	MMOFacade::getSingleton().SetPlayerMoney( *msg );
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_money_change(const char* buf, UINT32 size)
{
	G2C_MONEY_CHANGE* msg =  (G2C_MONEY_CHANGE*)buf;
	debugf( TEXT("[CMessageHandler::g2c_money_change] %d:%I64u"), msg->nMoneyType, msg->llMoney );
	
	MMOFacade::getSingleton().SetPlayerMoney( *msg );//(EMONEY_TYPE)msg->nMoneyType, msg->nMoney );
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_t2c_ping_ack(const char* buf, UINT32 size)
{
	T2C_PING_ACK* msg =  (T2C_PING_ACK*)buf;

	s_dwNetDelay = (UINT32)(appSeconds() * 1000.0f) - msg->dwPingTick;
	if (s_dwNetDelay > 100)
		warnf(TEXT("t2c delay is %d"), s_dwNetDelay);

	MMOFacade::getSingleton().SetGatePingDelayTime(s_dwGameDelay);
}

void CMessageHandler::Parse_g2c_ping_ack(const char* buf, UINT32 size)
{
	T2C_PING_ACK* msg =  (T2C_PING_ACK*)buf;
	s_dwGameDelay = (UINT32)(appSeconds() * 1000.0f) - msg->dwPingTick;
	if (s_dwGameDelay > 100)
		warnf(TEXT("g2c delay is %d"), s_dwGameDelay);
	MMOFacade::getSingleton().SetGameWorldPingDelayTime(s_dwGameDelay);
}

/************************************************************************/
INT CMessageHandler::GetClientEquipTypeByServer( INT ServerSlot )
{
	/*
	NES_MAIN_HAND,  //Main hand     Weapon          Weapon
    NES_OFF_HAND,   //Off hand      Weapon/Shield   Weapon/Shield
    NES_HEAD,       //Head          Hat             Hat
    NES_SHOULDER,   //Shoulder      Shoulder guard  Shoulder Armor
    NES_BODY,       //Body          Clothes or armor      Chest Armor
    NES_LEG,        //Leg           Pants or skirt  Pants
    NES_ARM,        //Wrist         Wristguard      Wrist Armor
    NES_WAIST,      //Waist         Belt            Belt
    NES_FOOT,       //Foot          Shoes           Shoes
    NES_BACK,       //Back          Cloak           Cloak
    NES_AVATAR,     //Fashion       Fashion         Avatar
    NES_NECK,       //Neck          Necklace        Necklace
    NES_FINGER_L,   //Left finger   Ring            Ring
    NES_FINGER_R,   //Right finger  Ring            Ring
    NES_WRIST_L,    //Left wrist    Bracelet        Bracelet
    NES_WRIST_R,    //Right wrist   Bracelet        Bracelet
	*/
	return ServerSlot;	// must ensure ids match
 	/*if( bNormalEquip )
 	{
 		switch(ServerSlot)
 		{
 		case NES_MAIN_HAND://Main hand
 			return E_NES_MAIN_HAND;
 		//case NES_OFF_HAND:
 		//	return E_NES_OFF_HAND;//
 		case NES_HEAD:
 			return E_NES_HEAD;//
 		case NES_SHOULDER:
 			return E_NES_SHOULDER;//
 		case NES_BODY:
 			return E_NES_BODY;//
		case NES_LEG:
 			return E_NES_LEG;
 		case NES_ARM:
 			return E_NES_ARM;
 		case  NES_WAIST:
 			return E_NES_WAIST;
 		case NES_FOOT:
 			return E_NES_FOOT;
 		case NES_FINGER_L:
 			return ENES_FingerL;//
 		case NES_FINGER_R:
 			return ENES_FingerR;//
 		case NES_WRIST_L:
 			return ENES_WristL;//
 		case NES_WRIST_R:
 			return ENES_WristR;
 		default:
 			break;
 		}
 	}*/
    //		else
    //		{
    //			switch( ServerSlot )
    //			{
    //			case( AES_HEAD ):
    //				return EAES_Head;
    //			case( AES_SHOULDER ):
    //				return EAES_Shoulder;
    //			case( AES_BODY ):
    //				return EAES_Body;
    //			case( AES_LEG ):
    //				return EAES_Leg;
    //			case( AES_FOOT ):
    //				return EAES_Foot;
    //// 			case( AES_HAND ):
    //// 				return EAES_Hand;
    //			case( AES_HAIR ):
    //				return EAES_Hair;
    //// 			case( AES_FACE ):
    //// 				return EAES_Face;
    //// 			case( AES_SKIN ):
    //// 				return EAES_Skin;
    //			default:
    //				break;
    //			}
    //		}

    return -1;
}

BOOL CMessageHandler::_Send(void* data, UINT32 size)
{
	MESSAGE_HEADER* msg = (MESSAGE_HEADER*)data;
	if(msg)
	{
#if !SHIPPING_PC_GAME
		extern char* g_szC2SMessageName[];
		UINT16 msgID = msg->wMsg;
		warnf(NAME_Log, TEXT("_Send:%d %s"), msg->wMsg, ANSI_TO_TCHAR(g_szC2SMessageName[msgID]));
#else
		//warnf(NAME_Log, TEXT("_Send:%d"), msg->wMsg);
#endif
	}

#if M_ENABLE_RPMGR
	#if !SHIPPING_PC_GAME
	extern char* g_szS2CMessageName[];
	MESSAGE_HEADER* msgHdr = (MESSAGE_HEADER*)data;
	UINT16 msgID = msgHdr->wMsg;
	verifyf(g_nC2SMessageSize[msgID] != 0, TEXT("[CMessageHandler::_Send] message is not valid, size < 0! Msg:%d %s"), msgID, ANSI_TO_TCHAR(g_szS2CMessageName[msgID]));

	if(g_replayMode)
	{
		return TRUE;
	}
	else
	{
// 		FReplayDataNetwork* newDat = new FReplayDataNetwork;
// 		newDat->DataType = ENDT_Send;
// 		newDat->DataSize = size;
// 		newDat->GameTime = appSeconds() - GStartTime;	
// 		newDat->DataBuffer = new char[size];
// 		appMemcpy(newDat->DataBuffer, data, size);
// 
// 		ReplayManager::getSingleton().Recode(newDat);
	}
	#endif

#endif
	if(_disableSend)
	{
#if !SHIPPING_PC_GAME
		extern char* g_szS2CMessageName[];
		MESSAGE_HEADER* msgHdr = (MESSAGE_HEADER*)data;
		UINT16 msgID = msgHdr->wMsg;
		verifyf(g_nC2SMessageSize[msgID] != 0, TEXT("[CMessageHandler::_Send] This message cannot be sent after returning to role select Msg:%d %s"), msgID, ANSI_TO_TCHAR(g_szS2CMessageName[msgID]));

#endif
		return FALSE;
	}
    if(!m_pSocketSessionGate || !m_pPacketAllocator)
        return FALSE;

    PACKET_BUFFER* pPacketBuffer = m_pPacketAllocator->Alloc(size);
    //debugf( TEXT("[_Send::alloc] %x"), (void*)pPacketBuffer);

    if(pPacketBuffer)
    {
		if (m_byEncryptCode)
		{	
			for (UINT32 i = 0; i< sizeof(FLEX_MESSAGE_HEADER) && i < size; i++)
				pPacketBuffer[i] = ((char*)data)[i];
			for (UINT32 i = sizeof(FLEX_MESSAGE_HEADER); i< size; i++)
				pPacketBuffer[i] = ((unsigned char*)data)[i] ^ m_byEncryptCode;
		}
		else
		{
			memcpy(pPacketBuffer, data, size);
		}
		//warnf(TEXT("_Send %d,5211"), size);
		INC_FLOAT_STAT_BY(MMOSTAT_Network_Upload, (float)size);
		//warnf(TEXT("_Send %d,5213"), size);
		return m_pSocketSessionGate->Send(pPacketBuffer, size, 0);
    }

    return FALSE;
}


void CMessageHandler::SyncHeroData(G2C_SYNC_HERO_DATA* NewData,  G2C_SYNC_HERO_DATA* OldData)
{
	if (NewData == NULL)
	{
		return;
	}

	AMMOActor* MainActor = MMOFacade::getSingleton().GetMainActor();
	if(MainActor && NewData->dwHeroID==MainActor->ServerHeroID)
	{
		MainActor->HeroName = FString(UTF8_TO_TCHAR(NewData->szName));

		MainActor->HeroAttributes->LevelAttributes.Level = NewData->byLevel;
		MainActor->HeroAttributes->ClassType	= NewData->byClass;
		MainActor->HeroAttributes->GenderType	= NewData->bySex;
		Send_Query_Hero_Attr_List();
	}

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(NewData->dwHeroID);

	if (actor == NULL)
	{
		warnf(TEXT("no such actor:%d, when recv sync_role_data"), NewData->dwHeroID);
		return;
	}

	actor->ActorName =  FString(UTF8_TO_TCHAR(NewData->szName));

	FString MasterName		= FString(UTF8_TO_TCHAR(NewData->szMasterName));
	FString MasterNameEx	= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Hero.Master"));
	actor->MasterName = FString::Printf(*MasterNameEx, *MasterName );

	actor->eventSetMesh(NewData->byClass, NewData->bySex);

	actor->ActorAttributes->SetMaxHP(NewData->nMaxHp);
	actor->ActorAttributes->SetMaxMP(NewData->nMaxMp);

	actor->ServerID = NewData->dwHeroID;
	actor->ActorAttributes->LevelAttributes.Level = NewData->byLevel;
	actor->ActorAttributes->ClassType	= NewData->byClass;
	actor->ActorAttributes->GenderType	= NewData->bySex;

	for( int i = 0; i < NES_VISIBLE_TOTAL; ++i )
	{
		const UItem* item = MMOFacade::getSingleton().FindItemTemplate(NewData->anAvatar[i]);
		if(!item)
		{
			warnf( TEXT("item not find [%id]"), NewData->anAvatar[i] );
			continue;
		}

		actor->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer( i ), true, item );
	}
	actor->ItemManager->UpdateOwnerData();

	MMOUI::GetUIManager()->delegateUpdateHintText(actor);

	actor->ReCalGridBlockRadius();
}


void CMessageHandler::SyncRoleData(G2C_SYNC_ROLE_DATA* NewData)
{
	if (NewData == NULL)
	{
		return;
	}

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(NewData->dwRoleID);

	if (actor == NULL)
	{
		warnf(TEXT("no such actor:%d, when recv sync_role_data"), NewData->dwRoleID);
		return;
	}
	actor->SetHidden(TRUE);
	actor->eventSetMesh(NewData->byClass, NewData->bySex);
	actor->ActorName =  FString(UTF8_TO_TCHAR(NewData->szName));
	actor->TeamID =  NewData->dwTeamID;

	actor->bResourceInitialized = TRUE;
	actor->bActorDataReset = TRUE;
	//actor->fActorDataResetDelay = 0.3f;

	//actor->GuildName = FString(UTF8_TO_TCHAR(NewData->szGuildName));
	// Add the guild info of the updated name to the query list
	if (NewData->dwGuildID > 0)
	{
		MMOGuildManager::getSingleton().NotifyUpdateActorGuildTitle(NewData->dwRoleID, TEXT(""), NewData->dwGuildID);
	}

	actor->MasterState = NewData->byMasterState;
	//actor->PersonalArenaScore = NewData->dwPersonalArenaScore;
	
	if(actor->ActorAttributes)
	{
		actor->ActorAttributes->ClassType = NewData->byClass;
		actor->ActorAttributes->GenderType = NewData->bySex;
		actor->ActorAttributes->Faction		= NewData->byFaction;
	
		AMMOPlayer *pPlayer = Cast<AMMOPlayer>(actor);
		if( pPlayer != NULL )
		{
			pPlayer->SetVip(NewData->byVIPLevel);

			//if(pPlayer->PetId>0)
			//{
			//	AMMOActor* PetActor = MMOFacade::getSingleton().GetCharacter(pPlayer->PetId);
			//	if(PetActor)
			//	{
			//		PetActor->MasterName = pPlayer->ActorName;
			//		MMOUI::GetUIManager()->delegateUpdateHintText(PetActor);
			//	}
			//}
		}

		if ( actor == MMOFacade::getSingleton().GetMainActor() )
		{
			MMOFacade::getSingleton().SetMyMaxMP(NewData->nMaxMp);
			MMOFacade::getSingleton().SetMyCurrentMaxHP(NewData->nMaxHp);
			MMOFacade::getSingleton().SetMyLevel(0);
			MMOFacade::getSingleton().SetMyPkValue(NewData->dwPkValue);
			if(actor->ActorAttributes->TitleID != NewData->byTitleID)
			{
				actor->ActorAttributes->TitleID = NewData->byTitleID;
				MMOFacade::getSingleton().NotifyPlayerTitleChange();
			}

			if(MMOFacade::getSingleton().GetMyPkMode() != NewData->byPkMode)
			{
				MMOFacade::getSingleton().SetMyPkMode(NewData->byPkMode);
			}

		}
		else
		{
			actor->ActorAttributes->SetMaxHP(NewData->nMaxHp);
			actor->ActorAttributes->SetMaxMP(NewData->nMaxMp);
			actor->ActorAttributes->LevelAttributes.Level = 0;
			if(actor->ActorAttributes->PkValue != NewData->dwPkValue)
			{
				actor->ActorAttributes->SetPkValue(NewData->dwPkValue);
				MMOFacade::getSingleton().NotifyOtherPlayerPkValueChange(Cast<AMMOPlayer>(actor),actor->ActorAttributes->PkValue);
			}
			if(actor->ActorAttributes->PkMode != NewData->byPkMode)
			{
				actor->ActorAttributes->PkMode = NewData->byPkMode;
				MMOFacade::getSingleton().NotifyOtherPlayerPkModeChange(Cast<AMMOPlayer>(actor),actor->ActorAttributes->PkMode);
			}
			if(actor->ActorAttributes->TitleID != NewData->byTitleID)
			{
				actor->ActorAttributes->TitleID = NewData->byTitleID;
				MMOFacade::getSingleton().NotifyOtherPlayerTitleChange(Cast<AMMOPlayer>(actor),actor->ActorAttributes->TitleID);
			}
		}

	}			

	// This data must be obtained before RequestTakeOn, RequestTakeOn needs it
	AMMOPlayer *player = Cast<AMMOPlayer>(actor);
	if (player)
	{
		// Weapon upgrade points
		player->WeaponUpgradePoint = NewData->byRefineFx;
	}

	for( int i = 0; i < NES_VISIBLE_TOTAL; ++i )
	{
		if(NewData->anAvatar[i] != 0)
		{
			const UItem* item = MMOFacade::getSingleton().FindItemTemplate(NewData->anAvatar[i]);
			if(!item)
			{
				warnf( TEXT("item not find [%id]"), NewData->anAvatar[i] );
				continue;
			}

			actor->ItemManager->RequestTakeOn( -1, GetClientEquipTypeByServer( i ), true, item );
		}
	}

	actor->NeedRefreshMaterial = FALSE;

	// apply hair/face/skin/etc.
	actor->ItemManager->ApplyHairModel( NewData->byClass, NewData->bySex, NewData->byHairType );
	actor->ItemManager->ApplyHairColor( NewData->byHairColor );
	actor->ItemManager->ApplyFaceModel( NewData->byClass, NewData->bySex, NewData->byFaceType );
	//actor->ItemManager->ApplySkinColor( NewData->bySkinColor );
	
	actor->ItemManager->UpdateOwnerData();

	actor->RefreshVisible();

	// weapon effect
	// Originally the body-strengthening effect index, now used to record weapon upgrade points
	//INT EffectIndex = (NewData->byRefineFx<<6)>>6;
	INT EffectIndex = 0;
	if (EffectIndex != 0)
	{
		actor->NotifyRefreshEquipLevelByIndex(EffectIndex);
	}

	//MMOUI::GetUIManager()->delegateUpdateHintText(actor);

	actor->ReCalGridBlockRadius();

	if (player)
	{
		// Data handling while in stall
		player->StallName = FString(UTF8_TO_TCHAR(NewData->szStallName));
		player->StallLevel = NewData->byStallLevel;
		UStallManager::getSingleton().ChangeActorStallState(player, NewData->byStallState);
	}

	MMOUI::GetUIManager()->delegateUpdateHintText( actor );
}

void CMessageHandler::TrySyncRoleData(INT InId)
{
	G2C_SYNC_ROLE_DATA* CacheData = m_RoleDataCache.GetCacheData(InId);
	if (CacheData == NULL)
	{
		CMessageHandler::GetInstance().SendQueryCharData(InId, 0);
	} 
	else
	{
		INT crc = crc32(0, (const Bytef*)CacheData, sizeof(G2C_SYNC_ROLE_DATA));
		SendQueryCharData(InId, crc);
		MMOFacade::getSingleton().AddOjbectToSync(InId);
	}
}

void CMessageHandler::ClearCacheData()
{
	CleanMsgSendRestrictList();

	m_RoleDataCache.Clear();

	m_NPCDataCache.Clear();

	for (INT2SyncHeroDataMap::iterator pos = m_HeroDataCache.begin(); pos != m_HeroDataCache.end(); ++pos)
	{
		delete pos->second;
		pos->second = NULL;
	}
	m_HeroDataCache.clear();

	
}

bool CMessageHandler::_ProcessReturnLoginError( INT errorCode, INT Param1, INT Param2 )
{
    //Login related unlock, Error may be triggered by T2C M2C

    EMsgSendRestrictList lockSlot = MsgRL_Num;
    //if(  errorCode  >= errLoginBegin &&  errorCode  <= errLoginEnd  )
    //    lockSlot = MsgRL_Login;
    //else 
	if(	errorCode == errDatabase)
	{
		lockSlot = MsgRL_Create;
	}
	else if(errorCode >= errLoginCreateRoleTooMany && errorCode <=errLoginCreateRoleNsDbSysErr)
	{
		lockSlot = MsgRL_Create;
	}
	else if(errorCode>=errLoginDeleteRoleNotFound && errorCode<=errLoginUnDeleteRoleNotFound)
	{
		lockSlot = MsgRL_Delete;
	}
	else if(errorCode==errLoginPrecreate||
		errorCode==errLoginRoleBusy||
		errorCode==errLoginRoleFrozen||
		errorCode==errLoginTimeUnknown||
		errorCode==errLoginLocked||
		errorCode==errLoginTooManyOnlineRole)
	{
		lockSlot = MsgRL_Select;
		MMO::ActiveGameState(EGameState_Selecting);
	}
	else if(errorCode>=errLoginBegin && errorCode<=errLoginEnd)
	{
		lockSlot = MsgRL_Login;
	}

    if( MsgRL_Num != lockSlot)
    {
        //static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);
        //FString enumVal = InEnum->GetEnum( errorCode ).ToString();
		FString enumVal = GetErrorCodeEnumString(errorCode); // FString enumVal(g_szErrorCodeNameList[errorCode]);
        FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal));

        UnLockSendRestrict( lockSlot , localVal  );
    }

	if(errorCode == errLoginRoleFrozen)
	{
		FString enumVal = GetErrorCodeEnumString(errorCode); // FString enumVal(g_szErrorCodeNameList[errorCode]);//InEnum->GetEnum( errCode ).ToString();

		if(Param1>0 && Param1<2000000000)
		{
			FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal));

			_SYSTEMTIME  ST = MMOFacade::getSingleton().ConvertUtcSecondToSystemTime( Param1 );

			FString Y = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Time.Time1"));
			FString M = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Time.Time2"));
			FString D = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Time.Time3"));
			FString H = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Time.Time4"));
			FString Mi = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Time.Time5"));
			FString S = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Time.Time6"));

			FString Msg = FString::Printf(TEXT("%s%d%s%d%s%d%s%d%s%d%s%d%s"), *localVal, ST.wYear, *Y, ST.wMonth, *M, ST.wDay, *D, ST.wHour, *H, ST.wMinute, *Mi, ST.wSecond,*S);
			FScriptDelegate nullFun(EC_EventParm);
			MMOMsgBoxManager::getSingleton().AddModalMessageBox(Msg,0, nullFun, nullFun,FALSE);
		}
		else
		{
			FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errLoginRoleFrozen_0"), *enumVal));
			FScriptDelegate nullFun(EC_EventParm);
			MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal,0, nullFun, nullFun,FALSE);
		}
		return true;
	}

	if (errorCode == errLoginRepeat)
	{
		AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
		
		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = pc;
		funYes.FunctionName = TEXT("OnReturnLoginConfirm");	

		FString localVal = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errLoginRepeat"));
		FScriptDelegate nullFun(EC_EventParm);
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal, 0, funYes, funYes,FALSE);
		g_bOnReturnLoginConfirm = true;
	}
	if (errorCode == errLoginRepeatKicked)
	{
		MMOMsgBoxManager::getSingleton().ClearAll();


		AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = pc;
		funYes.FunctionName = TEXT("OnReturnLoginConfirm");	
		FString localVal = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errLoginRepeatKicked"));

		MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal, 0, funYes, funYes,FALSE);
		g_bOnReturnLoginConfirm = true;
	}

	if( errorCode == errLoginRepeat ||
		errorCode == errLoginRepeatKicked||
		errorCode == errDisconnectTimeOut)
	{
		UMMOGameState_Login* loginState = Cast<UMMOGameState_Login>(g_gameStatesMap[EGameState_Login]);
		loginState->ErrorCodeCauseReturnLogin = errorCode;
		return true;
	}

	return false;
}

void CMessageHandler::Parse_m2c_fcm_state(const char* buf, UINT32 size)
{
	M2C_FCM_STATE* msg =  (M2C_FCM_STATE*)buf;

	if(!_isAdult)
	{
		FString msgID;
		if(msg->nOnlineMinutes < 1 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.FULL_Minutes");
		}
		else if(msg->nOnlineMinutes == 1 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.FULL_1HOUR");
		}
		else if(msg->nOnlineMinutes < 2 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.FULL_Minutes");
		}
		else if(msg->nOnlineMinutes == 2 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.FULL_2HOUR");
		}
		else if(msg->nOnlineMinutes < 3 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.FULL_Minutes");
		}
		else if(msg->nOnlineMinutes == 3 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.FULL_3HOUR");
		}
		else if(msg->nOnlineMinutes < 5 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.HALF");
		}
		else if(msg->nOnlineMinutes >= 5 * 60)
		{
			msgID = TEXT("MMOGame.FCM.MSG.None");
		}

		FString displayStr = MMOUI::ParseLocalizedPropertyPath(msgID);
		
		FString displayStrLoc = FString::Printf(LocalizeSecure(displayStr, msg->nOnlineMinutes));
		//FString::Printf(*displayStr, msg->nOnlineMinutes);

		//MMOUI::GetUIManager()->delegateAddScreenMessage(displayStrLoc, MsgInfoType_Warning);
		MMOChatManager::getSingleton().AddSystemMessage( displayStrLoc );

		FString marqueeClr;
		GConfig->GetString(TEXT("FCMColor"), TEXT("MarqueeColor"), marqueeClr, GUIColorIni);

		INT repetTime = 0, showTime = 0;
		GConfig->GetInt(TEXT("FCMColor"), TEXT("MarqueeRepeatTime"), repetTime, GUIColorIni);
		GConfig->GetInt(TEXT("FCMColor"), TEXT("MarqueeShowTime"), showTime, GUIColorIni);

		MMOGameEvent_MarqueeNoticeInfo evt( displayStrLoc, marqueeClr , repetTime , showTime );
		MMOUI::PostGameEvent(&evt);
	}
}

void CMessageHandler::Parse_g2c_extra_exp_change(const char* buf, UINT32 size)
{
	G2C_EXTRA_EXP_CHANGE* msg = (G2C_EXTRA_EXP_CHANGE*)buf;

	if(msg->nExtraExp > 0)
	{
		//Add scrolling message hint
		FString strScreen= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ExtraExp.Change"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Table,TEXT("MMOGame.ExtraExp.Change"));
		INT deltaExp = msg->nExtraExp - MMOAttributeAccountManager::getSingleton().getExpEtra();
		if (deltaExp > 0)
		{
			FString strChatMsg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ExtraExp.ChangeDetail"));
			MMOChatManager::getSingleton().AddSystemMessage( FString::Printf(*strChatMsg,deltaExp) );
		}
	}

	MMOGameEvent_OneInt evt( GameEvent_ExpExtraInit, msg->nExtraExp );
	MMOUI::PostGameEvent(&evt);

	MMOAttributeAccountManager::getSingleton().setExpEtra(msg->nExtraExp);
}

void CMessageHandler::Send_Activation(const FString& activation)
{
	C2M_AWARD_USE_NUMBER msg;
	msg.wMsg =  c2m_award_use_number;

	strxcpy(msg.chAwardNumber, *activation, ACTIVATION_LEN);

	_Send(&msg, sizeof(msg));
}

#if M_ENABLE_ASYNC_PROC_CHAR
void CMessageHandler::TickCharAppear( FLOAT deltaTime )
{
	debugf(TEXT("===== TickCharAppear"));

	DOUBLE StartTime = appSeconds();
	while(_charAppearList.size() > 0)
	{
		debugf(TEXT("\t ===== TickCharAppear in while"));
		FMsgBuf& msgBuf = _charAppearList.front();
		switch(msgBuf.MsgBuf->wMsg)
		{
		case g2c_char_appear:
			{
				G2C_CHAR_APPEAR* msg = (G2C_CHAR_APPEAR*)msgBuf.MsgBuf;
				ProcessCharData(msg, &(msg->stAppearData), msg->byAppearType);
			}
			break;

		case g2c_sync_char_list:
			{
				G2C_SYNC_CHAR_LIST* msg = (G2C_SYNC_CHAR_LIST*)msgBuf.MsgBuf;
				INT byCharCount = (msgBuf.Size - sizeof(G2C_SYNC_CHAR_LIST)) / (sizeof(msg->astAppearData[0]));
				for (INT i = 0; i < byCharCount; ++i)
				{
					ProcessCharData(0, &(msg->astAppearData[i]), soatInRange);
				}
			}
			break;
		}

		delete[] (BYTE*)msgBuf.MsgBuf;
		_charAppearList.pop_front();

		DOUBLE CurrentTime = appSeconds();
		if(CurrentTime - StartTime > 0.03f)
		{
			break;
		}
	}
}

bool CMessageHandler::CheckDuplicateChatContent( FString &Content )
{
	FString ContentTest = Content.Replace(TEXT(" "), TEXT(""));
	if(Content == g_lastSendContent)
	{
		FString strScreen = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Chat.CanNotSendDuplicate"));
		//MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Table,TEXT("MMOGame.Chat.NoPrivatePlayer"));
		MMOChatManager::getSingleton().AddSystemMessage(strScreen);
		return;
	}

	g_lastSendContent = Content;
	g_lastSendContent = g_lastSendContent.Replace(TEXT(" "), TEXT(""));
}

#endif
