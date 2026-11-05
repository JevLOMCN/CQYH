#include "stdafx.h"
#include "message_handler.h"
#include "simple_net.h"
#include "MMOGameEvent_Splash.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "error_code_name.h"
#include "MMOMsgBoxManager.h"
#include "MMOLoginManager.h"
#include "UStallManager.h"
#include "external_message_size.h"
#include <string.h>

using namespace ExternalMessage;

extern HWND GGameWindow;

TCHAR g_sdoLoginIP[MAX_PATH] = TEXT("");
INT g_sdoLoginPort = 0;

TCHAR g_sdoLoginIP2[MAX_PATH] = TEXT("");
INT g_sdoLoginPort2 = 0;

// 从Launcher调用
void EndSDOLogin()
{
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	//pc->ConsoleCommand(TEXT("PAUSE"));
			
	pc->LoginGame(g_sdoLoginIP, g_sdoLoginPort, TEXT(""), TEXT(""));
}
void _EndSDOLogin(const FString username)
{
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	//pc->ConsoleCommand(TEXT("PAUSE"));
			
	pc->LoginGame(g_sdoLoginIP, g_sdoLoginPort, username, TEXT(""));
}


void  CMessageHandler::LockSendRestrict( int msg )
{ 
    if( msg >= 0 && msg < m_vecMsgSendRestrictList.Num() )
    {
        m_vecMsgSendRestrictList(msg) = TRUE;
        warnf( NAME_Log, TEXT("***** [CMessageHandler::LockSendRestrict] Func[%s]"), *MsgSendRestrictList(msg) );
    }
}

void  CMessageHandler::UnLockSendRestrict( int msg, FString errMag )
{ 
    if( msg >= 0 && msg < m_vecMsgSendRestrictList.Num() )
    {
        m_vecMsgSendRestrictList(msg) = FALSE;
        warnf( NAME_Log, TEXT("***** [CMessageHandler::UnLockSendRestrict] Func[%s] ErrMsg[%s]"), *MsgSendRestrictList(msg), *errMag );
    }
}

//------------------------------------------------------------------------
VOID CMessageHandler::Login(const TCHAR* strIP, const INT nPort, const TCHAR* UserName, const TCHAR* Password, UBOOL bConnectBS)
{		
	//消息返回之前 不得重复发送 
	if( m_vecMsgSendRestrictList(MsgRL_Login) )
		return;
    LockSendRestrict(MsgRL_Login);
	m_byEncryptCode = 0;

	_tcsncpy(_UserName, UserName, USER_NAME_LEN);
	_UserName[USER_NAME_LEN - 1] = 0;
	_tcsncpy(_PW, Password, USER_NAME_LEN);
	_PW[USER_NAME_LEN - 1] = 0;
    m_bConnectBS = bConnectBS;

    if( m_pNet && NULL == m_pSocketSession )
    {
		m_dwServerIP = INADDR_NONE; // 双线选择前要清掉，以便后面判定使用第一个返回的IP
        m_pSocketSession = m_pNet->Connect(TCHAR_TO_UTF8(strIP), nPort);
		m_bsIP1 = FString(TEXT("BS1 - ")) + strIP;
		if(m_pSocketSession)
		{
			m_pSocketSession->SetUserData((void*)&m_bsIP1);
		}

		m_pSocketSession2 = m_pNet->Connect(TCHAR_TO_UTF8(g_sdoLoginIP2), g_sdoLoginPort2);
		m_bsIP2 = FString(TEXT("BS2 - ")) + g_sdoLoginIP2;
		if(m_pSocketSession2)
		{
			m_pSocketSession2->SetUserData((void*)&m_bsIP2);
		}
    }

    if( !m_pNet || ! (m_pSocketSession || m_pSocketSession2))
    {
        warnf(NAME_Error, TEXT("====== [CMessageHandler::Login] can't create connect ") );
        UnLockSendRestrict( MsgRL_Login, TEXT("Can't create connect") );
        return;
    }
    
	warnf(NAME_Log, TEXT("====== [CMessageHandler::Login] %s:%d UserIDName[%s]"), strIP, nPort, UserName );
    if(m_bConnectBS)
        _connectState = NCS_ConnectingBS;
    else
        _connectState = NCS_Connecting;
}

extern bool g_isSDOLogin;

void CMessageHandler::Parse_b2c_query_gate_ack(const char* buf, UINT32 size)
{
	//USES_CONVERSION;
	B2C_QUERY_GATE_ACK *msg = (B2C_QUERY_GATE_ACK*)buf;


	// 20200930 is the seller's legacy handshake (gate IP then comes from -ip=, port fixed to 8702).
	// Any other reply is a normal balance-server answer: use the gate address and port it sends.
	const UBOOL bLegacyMagic = (msg->dwAddr == 20200930);

	// 不为空说明之前有连上过
	if(m_dwServerIP != INADDR_NONE)
	{
		SET_WARN_COLOR(COLOR_CYAN);
		warnf(TEXT("==============10086[b2c_query_gate_ack] abandon: ip:%d.%d.%d.%d:%d"),
			(BYTE)(m_dwServerIP & 0x000000ff),
			(BYTE)((m_dwServerIP & 0x0000ff00) >> 8),
			(BYTE)((m_dwServerIP & 0x00ff0000) >> 16),
			(BYTE)((m_dwServerIP & 0xff000000) >> 24),
			m_dwPort );
		CLEAR_WARN_COLOR();

		return;
	}
	/*const TCHAR* pCmd =  appCmdLine();
	int len = WideCharToMultiByte(CP_ACP, 0, pCmd, -1, NULL, 0, NULL, NULL); 
	char *pSrc = new char[len];
	WideCharToMultiByte(CP_ACP, 0, pCmd, -1, pSrc, len, NULL, NULL);

	//warnf(NAME_Log, TEXT("====== [CMessageHandler::Parse_b2c_query_gate_ack] pSrc = [%s]"), (const TCHAR*)pSrc);
	//strcpy_s( pSrc, len, pCmd );
	char *pIP = NULL;
	char *pStr = strtok(pSrc,",");
	//warnf(NAME_Log, TEXT("====== [CMessageHandler::Parse_b2c_query_gate_ack] pStr = [%s]"), (const TCHAR*)pStr);
	int iCount = 0;
	while(pStr != NULL)
	{
		++iCount;
		pStr = strtok(NULL,",");
		if(iCount== 2 && pStr != NULL)
		{
			pIP = new char[strlen(pStr)+1];
			strcpy_s( pIP, strlen(pStr)+1, pStr );
			break;
		}
	}
	//warnf(NAME_Log, TEXT("====== [CMessageHandler::Parse_b2c_query_gate_ack] pIP = [%s]"), (const TCHAR*)pIP);
	if(pIP != NULL)
	{
		m_dwServerIP = inet_addr(pIP);
	}*/
	UINT32 m_dwServerIP = INADDR_NONE;
    LOG_PROCESS_ERROR(g_sdoLoginIP != NULL);
    m_dwServerIP = inet_addr(TCHAR_TO_UTF8(g_sdoLoginIP));

	if(rand() % 2)
	{
		m_dwPort = 8702;
	}
	else
	{
		m_dwPort = 8702;
	}
	if(!bLegacyMagic)
	{
		m_dwServerIP = msg->dwAddr;
		m_dwPort = msg->wPort;
	}

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(TEXT("=============== [b2c_query_gate_ack] Use this: ip:%d.%d.%d.%d:%d"),
		(BYTE)(m_dwServerIP & 0x000000ff),
		(BYTE)((m_dwServerIP & 0x0000ff00) >> 8),
		(BYTE)((m_dwServerIP & 0x00ff0000) >> 16),
		(BYTE)((m_dwServerIP & 0xff000000) >> 24),
		m_dwPort );
	CLEAR_WARN_COLOR();

	//CloseConnection();

	if(_connectState == NCS_ConnectingBS)
	{
// 		if(pSocketSession == m_pSocketSession)
// 		{
// 			m_pSocketSession = NULL;
// 		}
// 		else if(pSocketSession == m_pSocketSession2)
// 		{
// 			m_pSocketSession2 = NULL;
// 		}

		warnf( NAME_Log, TEXT("=============== [b2c_query_gate_ack] NCS_ConnectingBS "));

		if(m_dwServerIP == INADDR_NONE || m_dwPort <= 0 )
		{
			warnf( NAME_Error, TEXT("=============== [b2c_query_gate_ack] Failed to get the server ip:%d.%d.%d.%d:%d"),
				(BYTE)(m_dwServerIP & 0x000000ff),
				(BYTE)((m_dwServerIP & 0x0000ff00) >> 8),
				(BYTE)((m_dwServerIP & 0x00ff0000) >> 16),
				(BYTE)((m_dwServerIP & 0xff000000) >> 24),
				m_dwPort );

			CleanMsgSendRestrictList();

			UBOOL bRet = FALSE;
			FString locStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Login.FailedGetServerIP"), bRet);
#if SHIPPING_PC_GAME
			if(bRet)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(locStr,MsgInfoType_Table,TEXT("MMOErrorInfo.Login.FailedGetServerIP"));
			}
#else
			MMOUI::GetUIManager()->delegateAddScreenMessage(locStr,MsgInfoType_Table,TEXT("MMOErrorInfo.Login.FailedGetServerIP"));
#endif
			//断线重登处理
			if(g_isSDOLogin)
			{
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");

				extern void ShowLoginDialog();
				ShowLoginDialog();

				AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
				if(ctrl)
				{
					ctrl->GotoState(TEXT("LoginState"));
				}
			}
			else
			{
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_User,	  TRUE, "");
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Select, FALSE, "");
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Create, FALSE, "");
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Login_Revert, FALSE, "");
			}

#if M_ENABLE_NET_MSG_BUFF
			//ClearNetMsgBuf();
#endif
		}

		LOG_PROCESS_ERROR(m_dwServerIP != INADDR_NONE && m_dwPort > 0);

		if(m_pNet)
		{
			_connectState = NCS_Connecting;
			m_pSocketSessionGate = m_pNet->Connect(m_dwServerIP, m_dwPort);
			m_gtSessionUserData = FString::Printf(TEXT("Gate Sesson - %d.%d.%d.%d"),
				(BYTE)(m_dwServerIP & 0x000000ff),
				(BYTE)((m_dwServerIP & 0x0000ff00) >> 8),
				(BYTE)((m_dwServerIP & 0x00ff0000) >> 16),
				(BYTE)((m_dwServerIP & 0xff000000) >> 24));
			m_pSocketSessionGate->SetUserData((void*)&m_gtSessionUserData);
		}
	}

Exit0:
	return;
}
//------------------------------------------------------------------------
void CMessageHandler::Send_CharactorSelect(int roleid)
{
	C2T_SELECT_ROLE msg;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( roleid < 0 )
		roleid = 0;
	if( ctrl->RoleList.Num() <= roleid || roleid < 0 )
	{
		warnf(NAME_Error, TEXT("====== [CMessageHandler::SendCharactorSelect] RoleID[%d] UserID[%s]"),roleid,_UserName);
		return;
	}
	//消息返回之前 不得重复发送 
	if( m_vecMsgSendRestrictList(MsgRL_Select) )
		return;

	msg.wMsg = c2t_select_role;
	msg.dwRoleID = ctrl->RoleList(roleid).dwRoleID;
	FString roleName = ctrl->RoleList(roleid).RoleName;
	
	if( _Send(&msg, sizeof(msg)) )
    {
        LockSendRestrict(MsgRL_Select);
        warnf(NAME_Log, TEXT("====== [CMessageHandler::SendCharactorSelect] UserID[%d] RoleName[%s]"),_UserName, *roleName );
    }
    else
    {
        warnf(NAME_Error, TEXT("====== [CMessageHandler::SendCharactorSelect SendError] RoleID[%d] UserID[%s]"),roleid,_UserName);
    }
}
//------------------------------------------------------------------------
void CMessageHandler::Send_DeleteRole( INT RoleListId)
{
	AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();

	if( !pl || pl->RoleList.Num() <= 0 )
		return;
	//消息返回之前 不得重复发送 
	if( m_vecMsgSendRestrictList(MsgRL_Delete) )
		return;
	if( RoleListId < 0 || RoleListId >= pl->RoleList.Num() )
		return;
	LockSendRestrict(MsgRL_Delete);

	C2T_DELETE_ROLE msg;
	msg.wMsg = c2t_delete_role;
	msg.dwRoleID = pl->RoleList(RoleListId).dwRoleID;

	//@Temp 调试使用
	warnf(NAME_Log, TEXT("[CMessageHandler::Send_DeleteRole] [RoleListId]: %d"), RoleListId );
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_UnDeleteRole(INT roleid)
{
	AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();
	if( !pl)
		return;

	C2T_UNDELETE_ROLE msg;
	msg.wMsg = c2t_undelete_role;
	msg.dwRoleID = roleid;

	//@Temp 调试使用
	warnf(NAME_Log, TEXT("[CMessageHandler::Send_UnDeleteRole] [roleid]: %d"), roleid );
	_Send(&msg, sizeof(msg));
}

//------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Send_CreateRole(const TCHAR* Name,INT classid,INT sex, INT hairType, INT hairColor, INT faceType, INT skinColor)
{
	//消息返回之前 不得重复发送 
	if( m_vecMsgSendRestrictList(MsgRL_Create) )
		return;
	LockSendRestrict(MsgRL_Create);

	C2T_CREATE_ROLE msg;
	msg.wMsg = c2t_create_role;
	strxcpy(msg.szRoleName, Name, ROLE_NAME_LEN);
	msg.byClass = classid;
	msg.byFaceType = faceType;
	msg.byHairType = hairType;
	msg.byHairColor = hairColor;
	msg.bySkinColor = skinColor;
	msg.bySex = sex;

	//@Temp 调试使用
	warnf(NAME_Log, TEXT("[CMessageHandler::Send_CreateRole] [Name]: %s [Class]:%d [Sex]:%d"), Name,classid,sex );
	_Send(&msg, sizeof(msg));
}
void CMessageHandler::Send_DeleteRoleForEver(INT nRoleID)
{
	C2T_FOREVER_DELETE_ROLE msg;
	msg.wMsg = c2t_forever_delete_role;
	msg.dwRoleID = nRoleID;

	//@Temp 调试使用
	warnf(NAME_Log, TEXT("[CMessageHandler::Send_DeleteRoleForEver] [roleid]: %d"), nRoleID );
	_Send(&msg, sizeof(msg));
}
//------------------------------------------------------------------------
void  CMessageHandler::Parse_t2c_error_notify(const char* buf, UINT32 size)
{
	/*
	struct T2C_ERROR_NOTIFY : MESSAGE_HEADER
	{
	INT32            nErrCode;
	INT32            nParam1;
	INT32            nParam2;
	};
	*/
	T2C_ERROR_NOTIFY* msg = (T2C_ERROR_NOTIFY*)buf;
	
    INT32 errCode = msg->nErrCode;

	FString enumVal = GetErrorCodeEnumString(errCode);
	FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal));

	warnf(NAME_Error, TEXT("[CMessageHandler::Parse_t2c_error_notify] Error ID:[%d] %s %s"),
		 errCode , *enumVal, *localVal);

	// 如果被服务器处理的返回到登录界面就不往下走了
	if(_ProcessReturnLoginError( errCode, msg->nParam1, msg->nParam2))
	{
		return;
	}

	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);
	//FString enumVal(g_szErrorCodeNameList[errCode]);//InEnum->GetEnum( errCode ).ToString();

	if (  errCode  >= errSpellBegin &&  errCode  <= errSpellEnd)
	{
		MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvError( errCode , msg->nParam1, msg->nParam2);
	}

	FScriptDelegate func(EC_EventParm);
	func.Object = NULL;
	func.FunctionName = "";
	MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal,0,func,func,FALSE);
	//MMOUI::GetUIManager()->delegateAddScreenMessage(localVal);

}
//------------------------------------------------------------------------
void CMessageHandler::Parse_t2c_login_ack(const char* buf, UINT32 size)
{
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();

	T2C_LOGIN_ACK* msg = (T2C_LOGIN_ACK*)buf;
	// OSCARLOGIN:    ctrl->eventLoginSuccess();

//------	MMO::ActiveGameState( EGameState_Selecting );
	_isAdult = msg->bAdult;
	m_byEncryptCode = msg->byEncryptCode;
	m_dwClientOption = msg->dwClientOption;

	INT32 nOutLen = (max_message_count - 1) * sizeof(INT32);
	INT32 nRetCode = uncompress((Bytef*)g_nS2CMessageSize, (uLongf*)&nOutLen, (const Bytef*)msg->szMessageSize, size - sizeof(T2C_LOGIN_ACK));
	if (nRetCode != Z_OK || nOutLen != (max_message_count - 1) * sizeof(INT32))
	{
		// todo : return to login frame, show message box and record error log

		FString msgStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Login.MessageSizeError"));
		UnLockSendRestrict(MsgRL_Login,msgStr);
		FScriptDelegate func(EC_EventParm);
		func.Object = NULL;
		func.FunctionName = "";
		MMOMsgBoxManager::getSingleton().AddModalMessageBox(msgStr,0,func,func,FALSE);
		return;
	}

	MMOGameEvent_Splash loginEvt; 
	loginEvt.EvtType = MMOGameEvent_Splash::ESET_LoginGameSuccess;
	MMOUI::PostGameEvent(&loginEvt);

    //不解锁 直到OnDisconnected
    //UnLockSendRestrict(MsgRL_Login);
	
	//方便多开时调试
	if ( appGetPlatformType() == UE3::PLATFORM_Windows && GGameWindow != NULL )
	{
		/*AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
		if( player != NULL && ! player->actorName.IsEmpty() )
		{
		}*/
		FString sName;
		sName = _UserName;
		sName += FString(" -- ");
		sName += MMOFacade::getSingleton().GetMMOAppName();
		::SetWindowText(GGameWindow, *sName );	
	}	
	//szUserName是转化后的数字帐号  在这里不做显示用
	_tcscpy( _UserName, UTF8_TO_TCHAR(msg->szUserName) );
    UMMOLocalAI::Instance()->OnLoginResult(TRUE);
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_t2c_select_role_ack(const char* buf, UINT32 size)
{
	UnLockSendRestrict(MsgRL_Select);
	warnf(TEXT("[-------------------------CMessageHandler::Parse_t2c_select_role_ack]"));
	T2C_SELECT_ROLE_ACK* msg = ( T2C_SELECT_ROLE_ACK*)buf;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
    if( ctrl && ctrl->Pawn )
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
		if(!player)
			return;

		for (int i =0;i < ctrl->RoleList.Num(); i++)
		{
			if( ctrl->RoleList(i).dwRoleID == msg->dwRoleID)
			{
				PlayerAppearanceData data;

				//INT lastLevel = player->GetActorLevel();
				player->SetActorLevel( ctrl->RoleList(i).byLevel );
				player->UserId = FString(_UserName);
				player->ActorName = ctrl->RoleList(i).RoleName;
				player->ActorAttributes->ClassType = data.Class = ctrl->RoleList(i).byCareer;
				player->ActorAttributes->GenderType = data.Sex = ctrl->RoleList(i).bySex;
				player->ServerID = msg->dwRoleID;//added by york 后来加的

				player->ActorAttributes->HairColor = data.HairColor = ctrl->RoleList(i).nHairColor;
				player->ActorAttributes->HairType = data.HairModel = ctrl->RoleList(i).nHairType;
				player->ActorAttributes->SkinColor = data.SkinColor = ctrl->RoleList(i).nSkinColor;
				player->ActorAttributes->FaceType = data.FaceModel = ctrl->RoleList(i).nFaceType;

				MMOFacade::getSingleton().NotifyPlayerBaseInfoChange();
				MMOFacade::getSingleton().NotifyPlayerLevelChange( 0 , ctrl->RoleList(i).byLevel );


// 				UMMOSplashGFXPlayer *p = Cast<UMMOSplashGFXPlayer>(MMOUI::GetUIManager()->FindMoviePlayer(MMOUI::GetUIManager()->MoviePlayer_Login));
// 				if(p)
// 				{
// 					MMOGameEvent_Splash loginEvt;
// 					loginEvt.EvtType = MMOGameEvent_Splash::ESET_ShowSplashUI;
// 					loginEvt.ShowSplashUI = FALSE;
// 					p->OnNativeGameEvent(&loginEvt);
// 					p->ShowAllUI(FALSE);
// 					p->eventSetUIPlayerVisible(FALSE);
// 				}

				//确认登录后再播放动画
				MMOGameEvent_Splash loginEvt;
				loginEvt.EvtType = MMOGameEvent_Splash::ESET_EnterGameSuccess;
				MMOUI::PostGameEvent(&loginEvt);
				
				MMOGameEvent_Any enterGameEvt(GameEvent_EnterGameSuccess);
				MMOUI::PostGameEvent(&enterGameEvt);

				//player->ItemManager->UpdateApperance();

				break;
			}
		}

	}
 //   else if(msg->nErrCode == 2)
 //   {
 //       warnf( TEXT("====== [CMessageHandler::Parse_CLICOM_SELECTROLE_ACK] : the role data version is old, please reCreate new Role") );
 //       return;
 //   }
	//else
	//{
	//	warnf( TEXT("====== [CMessageHandler::Parse_CLICOM_SELECTROLE_ACK] :failed !") );
	//	return;
	//}
}
void CMessageHandler::Parse_t2c_return_select_role(const char* buf, UINT32 size)
{
    T2C_RETURN_SELECT_ROLE* msg = (T2C_RETURN_SELECT_ROLE*)buf;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if(!ctrl)
	{
		return;
	}	

	ctrl->bNeedCommitPosition = FALSE;
	ctrl->ReturnSelect();
	
	////不一定直接进入Select状态
	//MMOGameEvent_Splash loginEvt;
	//loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateListRoleCheck;
	//MMOUI::PostGameEvent(&loginEvt);


}
//------------------------------------------------------------------------
void CMessageHandler::Parse_t2c_sync_role_list(const char* buf, UINT32 size)
{
	T2C_SYNC_ROLE_LIST* msg = (T2C_SYNC_ROLE_LIST*)buf;

    //Sleep(1000);
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if (!ctrl)
	{
		return;
	}

	ctrl->ResetData();
	
	for (UINT8 i = 0; i < msg->byCount; ++ i)
	{
        LOGIN_ROLE_DATA* pBaseData = &msg->RoleData[i];
		//debugf(NAME_Debug, TEXT( "======= CLIDB_VALID_ROLELIST_ACK Role[%d][%s]" ),
		//	pBaseData->qwRoleID, pBaseData->aszRoleName );

        _tcscpy( _RoleName, UTF8_TO_TCHAR(pBaseData->szRoleName) );
		//strxcpy( _RoleName,UTF8_TO_TCHAR(pBaseData->szRoleName),USER_NAME_LEN );
        _RoleID = pBaseData->dwRoleID;
			
		FRoleInfo roleinfo(EC_EventParm);
		roleinfo.RoleName= _RoleName;
        roleinfo.dwRoleID = pBaseData->dwRoleID;
		roleinfo.nSceneTempID = pBaseData->nSceneTempID;
        roleinfo.nSceneID = pBaseData->nSceneTempID;
		roleinfo.bySex = pBaseData->bySex;
		roleinfo.byCareer = pBaseData->byClass;
		roleinfo.byLevel = pBaseData->byLevel;
		//roleinfo.WeaponEffectIndex = ((pBaseData->byRefineFx<<6)>>6);
		// 原来的体格强化特效索引，现在用来记录武器升级点数
		roleinfo.WeaponEffectIndex = 0;
		roleinfo.WeaponUpgradePoint = pBaseData->byRefineFx;
		//nOverTime == 0是可用的角色 否则是在删除列表中
		roleinfo.nOverTime = pBaseData->dwDeleteTime;
		roleinfo.nBeginTime = (INT)GetTickCount();
		//roleinfo.dwPosX = pBaseData->nX;
		//roleinfo.dwPosY = pBaseData->nY;
		//roleinfo.dwPosZ = pBaseData->nZ;
		//roleinfo.wDir = pBaseData->wFaceDirection;
		roleinfo.nSkinColor = (INT)pBaseData->bySkinColor;
		roleinfo.nFaceType = (INT)pBaseData->byFaceType;
		roleinfo.nHairColor = (INT)pBaseData->byHairColor;
		roleinfo.nHairType = (INT)pBaseData->byHairType;
		roleinfo.nSkinColor = (INT)pBaseData->bySkinColor;
		roleinfo.nSaveTime = pBaseData->dwSaveTime;

		if(roleinfo.nOverTime == 0)
			ctrl->RoleList.AddItem( roleinfo );
		else
			ctrl->DeletingRoleList.AddItem( roleinfo );
	
		FAvatarInfo tAvatarInfo(EC_EventParm);
		
		tAvatarInfo.RoleId = roleinfo.dwRoleID;
		//tAvatarInfo.MainHand = pBaseData->nMainHandID;
		//tAvatarInfo.OffHand = pBaseData->nOffHandID;
		for (int j = 0; j < NES_VISIBLE_TOTAL; ++ j)
		{
			// temp fix: 0 is money!
			if( pBaseData->nAvatar[j] > 100 )
				tAvatarInfo.EquipList[j] = pBaseData->nAvatar[j];
			else
				tAvatarInfo.EquipList[j] = -1;
		}
		ctrl->AvatarInfoList.AddItem(tAvatarInfo);
	}

	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if (player != NULL)
	{
		UStallManager::getSingleton().ChangeActorStallState(player, essNotOpen);
	}

	//MMOFacade::getSingleton().SortRoleList();

	if (EGameState_InGame == MMO::GetCurrentGameState() || EGameState_Loading == MMO::GetCurrentGameState())
	{

	}
	else
	{
		//不一定直接进入Select状态
		MMOGameEvent_Splash loginEvt;
		loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateListRoleCheck;
		MMOUI::PostGameEvent(&loginEvt);
	}
	
}	
//------------------------------------------------------------------------
void CMessageHandler::Parse_t2c_delete_role_ack(const char* buf, UINT32 size)
{
	UnLockSendRestrict(MsgRL_Delete);

	T2C_DELETE_ROLE_ACK *msg = (T2C_DELETE_ROLE_ACK*)buf;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( ctrl)
	{
		for (INT I = 0;I<ctrl->RoleList.Num();I++)
		{
			if( ctrl->RoleList(I).dwRoleID == msg->dwRoleID )
			{
				FRoleInfo& roleInfo = ctrl->RoleList(I);
				roleInfo.nOverTime = -1;
				ctrl->DeletingRoleList.AddItem(roleInfo);
				ctrl->RoleList.Remove(I,1);
				break;
			}
		}
	}

	MMOGameEvent_Splash loginEvt;
	loginEvt.EvtType = MMOGameEvent_Splash::ESET_UpdateRoleListUI;
	MMOUI::PostGameEvent(&loginEvt);
/*
	FScriptDelegate func(EC_EventParm);
	func.Object = NULL;
	func.FunctionName = "";
	MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Login.DeleteRole")),0,func,func,FALSE);*/

}
void CMessageHandler::Parse_t2c_undelete_role_ack(const char* buf, UINT32 size)
{
	T2C_UNDELETE_ROLE_ACK *msg = (T2C_UNDELETE_ROLE_ACK*)buf;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( ctrl)
	{
		for (INT I = 0;I<ctrl->DeletingRoleList.Num();I++)
		{
			if( ctrl->DeletingRoleList(I).dwRoleID == msg->dwRoleID )
			{
				FRoleInfo& roleInfo = ctrl->DeletingRoleList(I);
				roleInfo.nOverTime = 0;
				ctrl->RoleList.AddItem(roleInfo);
				ctrl->DeletingRoleList.Remove(I,1);
				break;
			}
		}
	}
	MMOGameEvent_Splash loginEvt;
	loginEvt.EvtType = MMOGameEvent_Splash::ESET_UpdateRoleListUI;
	MMOUI::PostGameEvent(&loginEvt);
}
void CMessageHandler::Parse_t2c_forever_delete_role_ack(const char* buf, UINT32 size)
{
	T2C_FOREVER_DELETE_ROLE_ACK*msg = (T2C_FOREVER_DELETE_ROLE_ACK*)buf;
	if(msg)
	{
		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		if( ctrl)
		{
			for (INT I = 0;I<ctrl->DeletingRoleList.Num();I++)
			{
				if( ctrl->DeletingRoleList(I).dwRoleID == msg->dwRoleID )
				{
					ctrl->DeletingRoleList.Remove(I,1);
					break;
				}
			}
		}
		MMOGameEvent_Splash loginEvt;
		loginEvt.EvtType = MMOGameEvent_Splash::ESET_UpdateRoleListUI;
		MMOUI::PostGameEvent(&loginEvt);
	}
}
//------------------------------------------------------------------------
void CMessageHandler::Parse_t2c_create_role_ack(const char* buf, UINT32 size)
{
	UnLockSendRestrict(MsgRL_Create);

	T2C_CREATE_ROLE_ACK* msg = (T2C_CREATE_ROLE_ACK*)buf;
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();


	FRoleInfo roleinfo(EC_EventParm);
    LOGIN_ROLE_DATA* pBaseData = (LOGIN_ROLE_DATA*)&msg->RoleData;
	_tcscpy( _RoleName, UTF8_TO_TCHAR(pBaseData->szRoleName) );
	roleinfo.RoleName=_RoleName;
	roleinfo.dwRoleID = pBaseData->dwRoleID;
	roleinfo.nSceneID = pBaseData->nSceneTempID;
	roleinfo.nSceneTempID = pBaseData->nSceneTempID;
	roleinfo.bySex = pBaseData->bySex;
	roleinfo.byCareer = pBaseData->byClass;
	roleinfo.byLevel = pBaseData->byLevel;
	roleinfo.nHairType = (INT)pBaseData->byHairType;
	roleinfo.nHairColor = (INT)pBaseData->byHairColor;
	roleinfo.nFaceType = (INT)pBaseData->byFaceType;
	roleinfo.nSkinColor = (INT)pBaseData->bySkinColor;
	ctrl->RoleList.AddItem( roleinfo );		

	//Avatar信息
	FAvatarInfo tAvatarInfo(EC_EventParm);
	tAvatarInfo.RoleId = roleinfo.dwRoleID;
	//tAvatarInfo.MainHand=pBaseData->nMainHandID;
	//tAvatarInfo.OffHand=pBaseData->nOffHandID;
	for(int i=0;i<NES_VISIBLE_TOTAL;++i)
	{
		// temp fix: 0 is money!
		if( pBaseData->nAvatar[i] > 100 )
			tAvatarInfo.EquipList[i] = pBaseData->nAvatar[i];
		else
			tAvatarInfo.EquipList[i] = -1;
	}
	ctrl->AvatarInfoList.AddItem(tAvatarInfo);

	MMOGameEvent_Splash loginEvt;
	loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateRoleSuccess;
	loginEvt.SelectRoleIndex = -2; // TODO: remove this hack
	MMOUI::PostGameEvent(&loginEvt);

	FString msgStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MsgBox.CreateRoleSucceeded"));
	FScriptDelegate nullDelegate(EC_EventParm);
	//MMOUI::GetUIManager()->delegateAddMessageBox(msgStr, 0, nullDelegate, nullDelegate);
	MMOUI::GetUIManager()->delegateAddScreenMessage(msgStr,MsgInfoType_Table,TEXT("MMOGame.MsgBox.CreateRoleSucceeded"));

	// 创建成功音效
	MMOFacade::getSingleton().PlayUISound(TEXT("menu_CreateCharacter"));
}

void CMessageHandler::Parse_g2c_logout_timer(const char* buf, UINT32 size)
{
	// 暂时没什么用
}

