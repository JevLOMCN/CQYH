#include "stdafx.h"
//#include "message_handler.h"
//#include "simple_net.h"
//#include "MMOGameEvent_Splash.h"
//#include "MMOFacade.h"
//#include "AbilityProcessor.h"
//#include "error_code_name.h"
//#include "MMOMsgBoxManager.h"
//using namespace ExternalMessage;
//
//extern HWND GGameWindow;
//
//TCHAR g_sdoLoginIP[MAX_PATH] = TEXT("");
//INT g_sdoLoginPort = 0;
//
//// Called from Launcher
//void EndSDOLogin()
//{
//	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
//	//pc->ConsoleCommand(TEXT("PAUSE"));
//			
//	pc->LoginGame(g_sdoLoginIP, g_sdoLoginPort, TEXT(""), TEXT(""));
//}
//
//void  CMessageHandler::LockSendRestrict( int msg )
//{ 
//    if( msg >= 0 && msg < m_vecMsgSendRestrictList.Num() )
//    {
//        m_vecMsgSendRestrictList(msg) = TRUE;
//        warnf( NAME_Log, TEXT("***** [CMessageHandler::LockSendRestrict] Func[%s]"), *MsgSendRestrictList(msg) );
//    }
//}
//
//void  CMessageHandler::UnLockSendRestrict( int msg, FString errMag )
//{ 
//    if( msg >= 0 && msg < m_vecMsgSendRestrictList.Num() )
//    {
//        m_vecMsgSendRestrictList(msg) = FALSE;
//        warnf( NAME_Log, TEXT("***** [CMessageHandler::UnLockSendRestrict] Func[%s] ErrMsg[%s]"), *MsgSendRestrictList(msg), *errMag );
//    }
//}
//
////------------------------------------------------------------------------
//VOID CMessageHandler::Login(const TCHAR* strIP, const INT nPort, const TCHAR* UserName, const TCHAR* Password, UBOOL bConnectBS)
//{		
//	//Must not send repeatedly before the message returns
//	if( m_vecMsgSendRestrictList(MsgRL_Login) )
//		return;
//    LockSendRestrict(MsgRL_Login);
//
//	_tcsncpy(_UserName, UserName, USER_NAME_LEN);
//	_UserName[USER_NAME_LEN - 1] = 0;
//	_tcsncpy(_PW, Password, USER_NAME_LEN);
//	_PW[USER_NAME_LEN - 1] = 0;
//    m_bConnectBS = bConnectBS;
//
//    if( m_pNet && NULL == m_pSocketSession )
//    {
//        m_pSocketSession = m_pNet->Connect(TCHAR_TO_UTF8(strIP), nPort);
//    }
//
//    if( !m_pNet || ! m_pSocketSession )
//    {
//        warnf(NAME_Error, TEXT("====== [CMessageHandler::Login] can't create connect ") );
//        UnLockSendRestrict( MsgRL_Login, TEXT("Can't create connect") );
//        return;
//    }
//    
//	warnf(NAME_Log, TEXT("====== [CMessageHandler::Login] %s:%d UserIDName[%s]"), strIP, nPort, UserName );
//    if(m_bConnectBS)
//        _connectState = NCS_ConnectingBS;
//    else
//        _connectState = NCS_Connecting;
//}
////------------------------------------------------------------------------
//void CMessageHandler::Send_CharactorSelect(int roleid)
//{
//	C2T_SELECT_ROLE msg;
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//	if( roleid < 0 )
//		roleid = 0;
//	if( ctrl->RoleList.Num() <= roleid || roleid < 0 )
//	{
//		warnf(NAME_Error, TEXT("====== [CMessageHandler::SendCharactorSelect] RoleID[%d] UserID[%s]"),roleid,_UserName);
//		return;
//	}
//	//Must not send repeatedly before the message returns
//	if( m_vecMsgSendRestrictList(MsgRL_Select) )
//		return;
//
//	msg.wMsg = c2t_select_role;
//	msg.dwRoleID = ctrl->RoleList(roleid).dwRoleID;
//	FString roleName = ctrl->RoleList(roleid).RoleName;
//	
//	if( _Send(&msg, sizeof(msg)) )
//    {
//        LockSendRestrict(MsgRL_Select);
//        warnf(NAME_Log, TEXT("====== [CMessageHandler::SendCharactorSelect] UserID[%d] RoleName[%s]"),_UserName, *roleName );
//    }
//    else
//    {
//        warnf(NAME_Error, TEXT("====== [CMessageHandler::SendCharactorSelect SendError] RoleID[%d] UserID[%s]"),roleid,_UserName);
//    }
//}
////------------------------------------------------------------------------
//void CMessageHandler::Send_DeleteRole( INT RoleListId)
//{
//	AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();
//
//	if( !pl || pl->RoleList.Num() <= 0 )
//		return;
//	//Must not send repeatedly before the message returns
//	if( m_vecMsgSendRestrictList(MsgRL_Delete) )
//		return;
//	if( RoleListId < 0 || RoleListId >= pl->RoleList.Num() )
//		return;
//	LockSendRestrict(MsgRL_Delete);
//
//	C2T_DELETE_ROLE msg;
//	msg.wMsg = c2t_delete_role;
//	msg.dwRoleID = pl->RoleList(RoleListId).dwRoleID;
//
//	//@Temp used for debugging
//	warnf(NAME_Log, TEXT("[CMessageHandler::Send_DeleteRole] [RoleListId]: %d"), RoleListId );
//	_Send(&msg, sizeof(msg));
//}
//
//void CMessageHandler::Send_UnDeleteRole(INT roleid)
//{
//	C2T_UNDELETE_ROLE msg;
//	msg.wMsg = c2t_undelete_role;
//	msg.dwRoleID = roleid;
//
//	//@Temp used for debugging
//	warnf(NAME_Log, TEXT("[CMessageHandler::Send_UnDeleteRole] [roleid]: %d"), roleid );
//	_Send(&msg, sizeof(msg));
//}
//
////------------------------------------------------------------------------------------------------------------------------------
//void CMessageHandler::Send_CreateRole(const TCHAR* Name,INT classid,INT sex)
//{
//	//Must not send repeatedly before the message returns
//	if( m_vecMsgSendRestrictList(MsgRL_Create) )
//		return;
//	LockSendRestrict(MsgRL_Create);
//
//	C2T_CREATE_ROLE msg;
//	msg.wMsg = c2t_create_role;
//	strxcpy(msg.szRoleName, Name, ROLE_NAME_LEN);
//	msg.byClass = classid;
//	msg.byFace = 1;
//	msg.byHair = 1;
//	msg.byHairColor = 1;
//	msg.bySex = sex;
//
//	//@Temp used for debugging
//	warnf(NAME_Log, TEXT("[CMessageHandler::Send_CreateRole] [Name]: %s [Class]:%d [Sex]:%d"), Name,classid,sex );
//	_Send(&msg, sizeof(msg));
//}
//
//void CMessageHandler::Parse_b2c_query_gate_ack(const char* buf, UINT32 size)
//{
//    B2C_QUERY_GATE_ACK *msg = (B2C_QUERY_GATE_ACK*)buf;
//
//    m_dwServerIP = msg->dwAddr;
//    m_dwPort = msg->wPort;
//
//	//CloseConnection();
//}
////------------------------------------------------------------------------
//void  CMessageHandler::Parse_t2c_error_notify(const char* buf, UINT32 size)
//{
//	/*
//	struct T2C_ERROR_NOTIFY : MESSAGE_HEADER
//	{
//	INT32            nErrCode;
//	INT32            nParam1;
//	INT32            nParam2;
//	};
//	*/
//	T2C_ERROR_NOTIFY* msg = (T2C_ERROR_NOTIFY*)buf;
//	
//    INT32 errCode = msg->nErrCode;
//
//	// If the server handled it and returned to the login screen, do not continue
//	if(_ProcessReturnLoginError( errCode ))
//	{
//		return;
//	}
//
//	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);
//
//	FString enumVal(g_szErrorCodeNameList[errCode]);//InEnum->GetEnum( errCode ).ToString();
//	
//	FString localVal = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal));
//
//	warnf(NAME_Error, TEXT("[CMessageHandler::Parse_t2c_error_notify] Error ID:[%d] %s"),
//		 errCode , *enumVal);
//	
//	if (  errCode  >= errSpellBegin &&  errCode  <= errSpellEnd)
//	{
//		MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvError( errCode , msg->nParam1, msg->nParam2);
//	}
//
//	FScriptDelegate func(EC_EventParm);
//	func.Object = NULL;
//	func.FunctionName = "";
//	MMOMsgBoxManager::getSingleton().AddModalMessageBox(localVal,0,func,func,FALSE);
//	//MMOUI::GetUIManager()->delegateAddScreenMessage(localVal);
//
//}
////------------------------------------------------------------------------
//void CMessageHandler::Parse_t2c_login_ack(const char* buf, UINT32 size)
//{
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//
//	T2C_LOGIN_ACK* msg = (T2C_LOGIN_ACK*)buf;
//	// OSCARLOGIN:    ctrl->eventLoginSuccess();
//	
//	MMOGameEvent_Splash loginEvt; 
//	loginEvt.EvtType = MMOGameEvent_Splash::ESET_LoginGameSuccess;
//	MMOUI::PostGameEvent(&loginEvt);
//
//    //Do not unlock until OnDisconnected
//    //UnLockSendRestrict(MsgRL_Login);
//
//	//Convenient for debugging with multiple clients
//	if ( appGetPlatformType() == UE3::PLATFORM_Windows && GGameWindow != NULL )
//	{
//		/*AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
//		if( player != NULL && ! player->actorName.IsEmpty() )
//		{
//		}*/
//		FString sName;
//		sName = _UserName;
//		sName += FString("--");
//		sName += MMOFacade::getSingleton().GetMMOAppName();
//		::SetWindowText(GGameWindow, *sName );	
//	}	
//
//    UMMOLocalAI::Instance()->OnLoginResult(TRUE);
//}
////------------------------------------------------------------------------
//void CMessageHandler::Parse_t2c_select_role_ack(const char* buf, UINT32 size)
//{
//	UnLockSendRestrict(MsgRL_Select);
//
//	T2C_SELECT_ROLE_ACK* msg = ( T2C_SELECT_ROLE_ACK*)buf;
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//    if( ctrl && ctrl->Pawn )
//	{
//		for (int i =0;i < ctrl->RoleList.Num(); i++)
//		{
//			if( ctrl->RoleList(i).dwRoleID == msg->dwRoleID)
//			{
//				ctrl->RoleName = ctrl->RoleList(i).RoleName;
//								
//				AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
//				if(!player)
//					return;
//				//INT lastLevel = player->GetActorLevel();
//				player->SetActorLevel( ctrl->RoleList(i).byLevel );
//				player->UserId = FString(_UserName);
//				player->ActorName = ctrl->RoleList(i).RoleName;
//				player->ActorAttributes->ClassType = ctrl->RoleList(i).byCareer;
//				player->ActorAttributes->GenderType = ctrl->RoleList(i).bySex;
//				player->ServerID = msg->dwRoleID;//added by york added later
//
//				MMOFacade::getSingleton().NotifyPlayerBaseInfoChange();
//				MMOFacade::getSingleton().NotifyPlayerLevelChange( 0 , ctrl->RoleList(i).byLevel );
//
//
//				UMMOSplashGFXPlayer *p = Cast<UMMOSplashGFXPlayer>(MMOUI::GetUIManager()->FindMoviePlayer(MMOUI::GetUIManager()->MoviePlayer_Login));
//				if(p)
//				{
//					MMOGameEvent_Splash loginEvt;
//					loginEvt.EvtType = MMOGameEvent_Splash::ESET_ShowSplashUI;
//					loginEvt.ShowSplashUI = FALSE;
//					p->OnNativeGameEvent(&loginEvt);
//					p->ShowAllUI(FALSE);
//					p->eventSetUIPlayerVisible(FALSE);
//				}
//
//				//Play animation after login is confirmed
//				MMOGameEvent_Splash loginEvt;
//				loginEvt.EvtType = MMOGameEvent_Splash::ESET_EnterGameSuccess;
//				MMOUI::PostGameEvent(&loginEvt);
//				
//				MMOGameEvent_Any enterGameEvt(GameEvent_EnterGameSuccess);
//				MMOUI::PostGameEvent(&enterGameEvt);
//
//				break;
//			}
//		}
//
//	}
// //   else if(msg->nErrCode == 2)
// //   {
// //       warnf( TEXT("====== [CMessageHandler::Parse_CLICOM_SELECTROLE_ACK] : the role data version is old, please reCreate new Role") );
// //       return;
// //   }
//	//else
//	//{
//	//	warnf( TEXT("====== [CMessageHandler::Parse_CLICOM_SELECTROLE_ACK] :failed !") );
//	//	return;
//	//}
//}
//void CMessageHandler::Parse_t2c_return_select_role(const char* buf, UINT32 size)
//{
//    T2C_RETURN_SELECT_ROLE* msg = (T2C_RETURN_SELECT_ROLE*)buf;
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//	if(!ctrl)
//	{
//		return;
//	}	
//
//	ctrl->bNeedCommitPosition = FALSE;
//	ctrl->ReturnSelect();
//	
//	////Does not necessarily enter Select state directly
//	//MMOGameEvent_Splash loginEvt;
//	//loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateListRoleCheck;
//	//MMOUI::PostGameEvent(&loginEvt);
//
//
//}
////------------------------------------------------------------------------
//void CMessageHandler::Parse_t2c_sync_role_list(const char* buf, UINT32 size)
//{
//	T2C_SYNC_ROLE_LIST* msg = (T2C_SYNC_ROLE_LIST*)buf;
//
//    //Sleep(1000);
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//	if(!ctrl)
//	{
//		return;
//	}
//
//	ctrl->ResetData();
//	
//	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
//	if( player != NULL )
//	{
//		//@todo add a player relogin method
//		
//		UStallManager::GetInstance().ChangeActorStallState( player,essNotOpen );
//	}
//	for (UINT8 i = 0; i < msg->byCount; ++i)
//	{
//        LOGIN_ROLE_DATA* pBaseData = &msg->RoleData[i];
//		//debugf(NAME_Debug, TEXT( "======= CLIDB_VALID_ROLELIST_ACK Role[%d][%s]" ),
//		//	pBaseData->qwRoleID, pBaseData->aszRoleName );
//
//        _tcscpy( _RoleName, UTF8_TO_TCHAR(pBaseData->szRoleName) );
//		//strxcpy( _RoleName,UTF8_TO_TCHAR(pBaseData->szRoleName),USER_NAME_LEN );
//        _RoleID = pBaseData->dwRoleID;
//			
//		FRoleInfo roleinfo(EC_EventParm);
//		roleinfo.RoleName= _RoleName;
//        roleinfo.dwRoleID = pBaseData->dwRoleID;
//		roleinfo.nSceneTempID = pBaseData->nSceneTempID;
//        roleinfo.nSceneID = pBaseData->nSceneTempID;
//		roleinfo.bySex = pBaseData->bySex;
//		roleinfo.byCareer = pBaseData->byClass;
//		roleinfo.byLevel = pBaseData->byLevel;
//		roleinfo.WeaponEffectIndex = ((pBaseData->byRefineFx<<6)>>6);
//		roleinfo.nOverTime = pBaseData->dwRemainingDeleteTime;
//		roleinfo.nBeginTime = (INT)GetTickCount();
//		//roleinfo.dwPosX = pBaseData->nX;
//		//roleinfo.dwPosY = pBaseData->nY;
//		//roleinfo.dwPosZ = pBaseData->nZ;
//		//roleinfo.wDir = pBaseData->wFaceDirection;
//		ctrl->RoleList.AddItem( roleinfo );
//	
//		FAvatarInfo tAvatarInfo(EC_EventParm);
//		//tAvatarInfo.MainHand = pBaseData->nMainHandID;
//		//tAvatarInfo.OffHand = pBaseData->nOffHandID;
//		for(int j=0;j<NES_VISIBLE_TOTAL;++j )
//		{
//			// temp fix: 0 is money!
//			if( pBaseData->nAvatar[j] > 100 )
//				tAvatarInfo.EquipList[j] = pBaseData->nAvatar[j];
//			else
//				tAvatarInfo.EquipList[j] = -1;
//		}
//		ctrl->AvatarInfoList.AddItem(tAvatarInfo);
//	}
//
//	if(EGameState_InGame == MMO::GetCurrentGameState() || EGameState_Loading == MMO::GetCurrentGameState())
//	{
//
//	}
//	else{
//		//Does not necessarily enter Select state directly
//		MMOGameEvent_Splash loginEvt;
//		loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateListRoleCheck;
//		MMOUI::PostGameEvent(&loginEvt);
//	}
//	
//}	
////------------------------------------------------------------------------
//void CMessageHandler::Parse_t2c_delete_role_ack(const char* buf, UINT32 size)
//{
//	UnLockSendRestrict(MsgRL_Delete);
//
//	T2C_DELETE_ROLE_ACK *msg = (T2C_DELETE_ROLE_ACK*)buf;
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//	if( ctrl)
//	{
//		for (INT I = 0;I<ctrl->RoleList.Num();I++)
//		{
//			if( ctrl->RoleList(I).dwRoleID == msg->dwRoleID )
//			{
//				ctrl->RoleList(I).nOverTime = msg->dwRemainingDeleteTime;
//				ctrl->RoleList(I).nBeginTime = (INT)GetTickCount();
//				// Do not delete, change to time display
//				//ctrl->RoleList.Remove(I,1);
//				//ctrl->AvatarInfoList.Remove(I,1);
//			}
//		}
//	}
//	MMOGameEvent_Splash loginEvt;
//	loginEvt.EvtType = MMOGameEvent_Splash::ESET_UpdateRoleListUI;
//	MMOUI::PostGameEvent(&loginEvt);
//
//	FScriptDelegate func(EC_EventParm);
//	func.Object = NULL;
//	func.FunctionName = "";
//	MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Login.DeleteRole")),0,func,func,FALSE);
//
//}
//void CMessageHandler::Parse_t2c_undelete_role_ack(const char* buf, UINT32 size)
//{
//	T2C_UNDELETE_ROLE_ACK *msg = (T2C_UNDELETE_ROLE_ACK*)buf;
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//	if( ctrl)
//	{
//		for (INT I = 0;I<ctrl->RoleList.Num();I++)
//		{
//			if( ctrl->RoleList(I).dwRoleID == msg->dwRoleID )
//			{
//				ctrl->RoleList(I).nOverTime = 0;
//				ctrl->RoleList(I).nBeginTime = 0;
//			}
//		}
//	}
//	MMOGameEvent_Splash loginEvt;
//	loginEvt.EvtType = MMOGameEvent_Splash::ESET_UpdateRoleListUI;
//	MMOUI::PostGameEvent(&loginEvt);
//}
////------------------------------------------------------------------------
//void CMessageHandler::Parse_t2c_create_role_ack(const char* buf, UINT32 size)
//{
//	UnLockSendRestrict(MsgRL_Create);
//
//	T2C_CREATE_ROLE_ACK* msg = (T2C_CREATE_ROLE_ACK*)buf;
//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
//
//
//	FRoleInfo roleinfo(EC_EventParm);
//    LOGIN_ROLE_DATA* pBaseData = (LOGIN_ROLE_DATA*)&msg->RoleData;
//	_tcscpy( _RoleName, UTF8_TO_TCHAR(pBaseData->szRoleName) );
//	roleinfo.RoleName=_RoleName;
//	roleinfo.dwRoleID = pBaseData->dwRoleID;
//	roleinfo.nSceneID = pBaseData->nSceneTempID;
//	roleinfo.nSceneTempID = pBaseData->nSceneTempID;
//	roleinfo.bySex = pBaseData->bySex;
//	roleinfo.byCareer = pBaseData->byClass;
//	roleinfo.byLevel = pBaseData->byLevel;
//	ctrl->RoleList.AddItem( roleinfo );		
//
//	//Avatar info
//	FAvatarInfo tAvatarInfo(EC_EventParm);
//	//tAvatarInfo.MainHand=pBaseData->nMainHandID;
//	//tAvatarInfo.OffHand=pBaseData->nOffHandID;
//	for(int i=0;i<NES_VISIBLE_TOTAL;++i)
//	{
//		// temp fix: 0 is money!
//		if( pBaseData->nAvatar[i] > 100 )
//			tAvatarInfo.EquipList[i] = pBaseData->nAvatar[i];
//		else
//			tAvatarInfo.EquipList[i] = -1;
//	}
//	ctrl->AvatarInfoList.AddItem(tAvatarInfo);
//
//	MMOGameEvent_Splash loginEvt;
//	loginEvt.EvtType = MMOGameEvent_Splash::ESET_CreateRoleSuccess;
//	loginEvt.SelectRoleIndex = -2; // TODO: remove this hack
//	MMOUI::PostGameEvent(&loginEvt);
//
//	FString msgStr = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.MsgBox.CreateRoleSucceeded"));
//	FScriptDelegate nullDelegate(EC_EventParm);
//	//MMOUI::GetUIManager()->delegateAddMessageBox(msgStr, 0, nullDelegate, nullDelegate);
//	MMOUI::GetUIManager()->delegateAddScreenMessage(msgStr,MsgInfoType_Table,TEXT("MMOGame.MsgBox.CreateRoleSucceeded"));
//
//	// Creation success sound effect
//	MMOFacade::getSingleton().PlayUISound(TEXT("menu_CreateCharacter"));
//}
