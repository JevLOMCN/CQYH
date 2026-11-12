#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "MMOAchievementManager.h"
#include "FriendManager.h"
#include "FriendChatManager.h"
#include "MMOQuestManager.h"
#include "MMOChatManager.h"
#include "MMOMsgButtonManager.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;

//----------------------------------------------------------request friendlist
void CMessageHandler::Request_FriendList()
{
	//C2M_FRIEND_LIST msg;
	//msg.byMsg = c2m_friend_list;
	//_Send( &msg, sizeof(msg));
}

//---------------------------------------------------------------------------------------------------------
void CMessageHandler::SendFriendChat( INT RoleId , FString ChatContent )
{
	if( ChatContent.IsEmpty())
	{
		return;
	}
	
	//MMOFacade::getSingleton().NotifyFriendChatMsg( RoleId , ChatContent );

	C2M_FRIEND_CHAT* msg =  (C2M_FRIEND_CHAT*)ms_MsgBuffer;
	
	msg->wMsg = c2m_friend_chat;
	msg->dwTargetID = RoleId;

	FTCHARToUTF8 szContent( *ChatContent);
	msg->wSize = sizeof(C2M_FRIEND_CHAT) + appStrlen( szContent ) + 1;
	strxcpy( msg->szMessage , szContent , MAX_CHAT_MESSAGE_LEN );

	_Send(msg, msg->wSize);
}


//---------------------------------------------------------------------------------------------------------
void CMessageHandler::RequestAddFriend( FString RoleName , INT RoleId  )
{
	//Fill in ID if there is one, otherwise fill 0 and look up by name
	C2M_FRIEND_ADD msg;
	msg.wMsg = c2m_friend_add;
	msg.dwRoleID  = RoleId;
	strxcpy(msg.szRoleName , *RoleName, ROLE_NAME_LEN );

	_Send( &msg , sizeof(C2M_FRIEND_ADD) );
}
//
void CMessageHandler::RequestFriendChangeInfo( FString szSignature , INT byFriendState  )
{
	C2M_CHANGE_INFO msg;
	msg.wMsg = c2m_change_info;
	strxcpy( msg.szSignature , *szSignature ,SIGNATURE_LEN );
	msg.byFriendState = byFriendState;
	_Send( &msg , sizeof(C2M_CHANGE_INFO) );
}
//----------------------------------------------------------                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
void CMessageHandler::RequestDelFriend( INT RoleId  )
{	
	C2M_FRIEND_DEL msg;
	msg.wMsg = c2m_friend_del;
	msg.dwRoleID  = RoleId;
	
	_Send(&msg, sizeof(C2M_FRIEND_DEL));
}
//----------------------------------------------------------
void CMessageHandler::ChangeFriendGroupName( FString GroupName, INT GroupId  )
{
	C2G_FRIEND_CHANGE_GROUP_NAME msg;
	msg.wMsg = c2g_friend_change_group_name;
	msg.byGroupID = GroupId;
	strxcpy(msg.szGroupName , TCHAR_TO_UTF8(*GroupName) ,FRIEND_GROUP_NAME_LEN);
	 
	_Send( & msg, sizeof(C2G_FRIEND_CHANGE_GROUP_NAME));

}
//----------------------------------------------------------
void CMessageHandler::ChangeFriendGroup( INT RoleId, INT GroupId   )
{
	
	C2M_FRIEND_CHANGE_GROUP msg;
	msg.wMsg = c2m_friend_change_group;
	msg.dwRoleID  = RoleId;
	msg.byGroupID = GroupId;

	_Send( &msg, sizeof( C2M_FRIEND_CHANGE_GROUP ));
}
//----------------------------------------------------------

//----------------------------------------------------enter scene initialization sync------
void CMessageHandler::Parse_g2c_friend_group_list(const char* buf, UINT32 size)
{
	G2C_FRIEND_GROUP_LIST* msg = (G2C_FRIEND_GROUP_LIST*)buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player )
	{
		return;
	}
	

	INT GroupNum = msg->byGroupCount;
	for ( INT i = 0 ; i < GroupNum ; i++ ) //
	{
		INT GroupId = msg->stGroupInfo[i].byGroupID;
		if( FriendManager::getSingleton().GetFriendGroup(GroupId) )
		{
			FriendManager::getSingleton().GetFriendGroup(GroupId)->GroupName = UTF8_TO_TCHAR( msg->stGroupInfo[i].szGroupName);
		}		

		MMOGameEvent_OneInt evt( GameEvent_UpdateFriendGroup , GroupId );
		MMOUI::PostGameEvent( &evt );
	}
	
}
//-----------------------------------------------------sync signature on entering scene-----
void CMessageHandler::Parse_m2c_self_info(const char* buf, UINT32 size)
{
	M2C_SELF_INFO* msg = (M2C_SELF_INFO*)buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	
}
//-----------------------------------------------------enter scene initialization sync-----
void CMessageHandler::Parse_m2c_friend_list(const char* buf, UINT32 size)
{
	M2C_FRIEND_LIST* msg = (M2C_FRIEND_LIST*)buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( ctrl && player )
	{
				
		for ( int i = 0 ; i< msg->byFriendCount ; i++ )
		{
			MMOFriendMember Friend;
			Friend.ActorName = UTF8_TO_TCHAR( msg->stFriendInfoWithGroup[i].stFriendInfo.szRoleName );
			Friend.ClassId = msg->stFriendInfoWithGroup[i].stFriendInfo.byClass;
			//Friend.Lv = msg->stFriendInfoWithGroup[i].stFriendInfo.byLevel;
			Friend.RoleId = msg->stFriendInfoWithGroup[i].stFriendInfo.dwRoleID;
			Friend.Sex = msg->stFriendInfoWithGroup[i].stFriendInfo.bySex;
			Friend.Online = msg->stFriendInfoWithGroup[i].stFriendInfo.byFriendState;
			Friend.AbbName = MMOFacade::CheckName( Friend.ActorName );
			Friend.Sign = UTF8_TO_TCHAR( msg->stFriendInfoWithGroup[i].stFriendInfo.szSignature );
			Friend.nIntimacy = msg->stFriendInfoWithGroup[i].stFriendInfo.wIntimacy;
			Friend.SceneTempID = msg->stFriendInfoWithGroup[i].stFriendInfo.bySceneTempID;

			if(Friend.RoleId <= 0)
				continue;

			if( Friend.Online == FS_OFFLINE )
			{
				Friend.Location = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Offline") );
			}
			else
			{
				Friend.Location = UGlobalMapInfo::GetSingleton()->FindMapInfo( msg->stFriendInfoWithGroup[i].stFriendInfo.bySceneTempID ).MapDisplayName;
			}
			
			if( FRIEND_GROUP_ENEMY == msg->stFriendInfoWithGroup[i].byGroup )
			{
				//Enemy
				if(!FriendManager::getSingleton().InMyEnemyList(Friend.RoleId))
					FriendManager::getSingleton().EnemyList.AddItem(Friend);
			}
			else if( msg->stFriendInfoWithGroup[i].bMutual )
			{			
				//Two-way, is friend
				FriendManager::getSingleton().PushToFriendGroup( msg->stFriendInfoWithGroup[i].byGroup , Friend );
									
			}
			else
			{
				//Follow
				if(!FriendManager::getSingleton().IsMyAttention(Friend.RoleId))
				{
					Friend.Location = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.Attention") );
					FriendManager::getSingleton().PushToFriendGroup( AttentionGroupId , Friend );
				}
			}
		}
	}

	//After syncing friends, read friend related saved data
	FriendManager::getSingleton().LoadKillInfoData();
	FriendManager::getSingleton().LoadNearContact();
	FriendChatManager::getSingleton().LoadChatLogFromFile();
	//Sort names
	FriendManager::getSingleton().SortAllFriend();
}

//----------------------------------------------------------two-way friends
void CMessageHandler::Parse_m2c_friend_info_change(const char* buf, UINT32 size)
{
	M2C_FRIEND_INFO_CHANGE* msg = (M2C_FRIEND_INFO_CHANGE*)buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

	if( player )
	{
		for (int  i = 0 ; i < FriendManager::getSingleton().GetFriendGroupNum() ; i++)
		{
			for (int  j = 0 ; j < FriendManager::getSingleton().GetFriendNumInGroup(i) ; j++ )
			{
				MMOFriendMember* fMember = FriendManager::getSingleton().GetFriendGroupMember( i , j );
				if( fMember && fMember->RoleId == msg->stFriendInfo.dwRoleID )
				{
					//fMember->Lv = msg->stFriendInfo.byLevel;
					//fMember->Online = msg->stFriendInfo.byFriendState;
					fMember->Sign = UTF8_TO_TCHAR(msg->stFriendInfo.szSignature);
					fMember->ClassId = msg->stFriendInfo.byClass;
					fMember->Sex = msg->stFriendInfo.bySex;
					fMember->SceneTempID = msg->stFriendInfo.bySceneTempID;

					if(FriendManager::getSingleton().IsMyAttention(fMember->RoleId))
					{
						fMember->Location = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.Attention") );
					}
					else
					{
						fMember->Location =  UGlobalMapInfo::GetSingleton()->FindMapInfo( msg->stFriendInfo.bySceneTempID ).MapDisplayName;
					}
					
					
					if(MMOUI::GetUIManager()->HintFriendOnline && i != AttentionGroupId)
					{
						if(fMember->Online != msg->stFriendInfo.byFriendState)
						{
							if((FS_OFFLINE == fMember->Online)&& (msg->stFriendInfo.byFriendState == FS_ONLINE))
							{
								//Your friend came online
								FString text = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.FriendOnline")) , *(fMember->ActorName) );
								MMOChatManager::getSingleton().AddSystemMessage( text );
								MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Normal,text);
							}
							else if((FS_ONLINE == fMember->Online)&& (msg->stFriendInfo.byFriendState == FS_OFFLINE))
							{
								//Your friend went offline
								FString text = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.FriendOffline")) , *(fMember->ActorName) );
								MMOChatManager::getSingleton().AddSystemMessage( text);
								MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Normal,text);
							}
						}
					}
					fMember->Online = msg->stFriendInfo.byFriendState;

					FriendManager::getSingleton().UpdateNerContactListMember(fMember);

					//No need to update others, needs sorting, update again
					
					MMOGameEvent_OneInt evt1( GameEvent_UpdateFriendGroup , i );
					MMOUI::PostGameEvent( &evt1 );

					MMOGameEvent_OneInt evt( GameEvent_FriendInfoChange , fMember->RoleId );
					MMOUI::PostGameEvent( &evt );

					MMOGameEvent_UpdateNearContact evt2;
					MMOUI::PostGameEvent(&evt2);

					return;
				}
				
			}
		}
		
		//Not found, so not a friend... try enemies
		MMOFriendMember* enemy = FriendManager::getSingleton().GetEnemyMemberByRoleId(msg->stFriendInfo.dwRoleID);
		if( enemy )
		{
			//enemy->Lv = msg->stFriendInfo.byLevel;
			enemy->Online = msg->stFriendInfo.byFriendState;
			enemy->Sign = UTF8_TO_TCHAR(msg->stFriendInfo.szSignature);
			enemy->ClassId = msg->stFriendInfo.byClass;
			enemy->Sex = msg->stFriendInfo.bySex;
			enemy->Location =  UGlobalMapInfo::GetSingleton()->FindMapInfo( msg->stFriendInfo.bySceneTempID ).MapDisplayName;

			MMOGameEvent_Any evt( GameEvent_UpdateEnemyContent );
			MMOUI::PostGameEvent( &evt );

			return;
		}

		//Means not an enemy, what remains is the blacklist; designer does not require updating blacklist info
		MMOFriendMember* blackP = FriendManager::getSingleton().GetBlackMemberByRoleId(msg->stFriendInfo.dwRoleID);
		if( blackP )
		{
			//blackP->Lv = msg->stFriendInfo.byLevel;
			blackP->Online = msg->stFriendInfo.byFriendState;
			blackP->Sign = UTF8_TO_TCHAR(msg->stFriendInfo.szSignature);
			blackP->ClassId = msg->stFriendInfo.byClass;
			blackP->Sex = msg->stFriendInfo.bySex;
			blackP->Location =  UGlobalMapInfo::GetSingleton()->FindMapInfo( msg->stFriendInfo.bySceneTempID ).MapDisplayName;

			MMOGameEvent_Any evt( GameEvent_UpdateBlackList );
			MMOUI::PostGameEvent( &evt );

			return;
		}
	}
	
	
}

void CMessageHandler::Parse_m2c_personal_enemy_add(const char* buf, UINT32 size)
{
	M2C_PERSONAL_ENEMY_ADD* msg = (M2C_PERSONAL_ENEMY_ADD*)buf;
	
	MMOFriendMember Friend(EC_EventParm);
	//Friend.ActorName = UTF8_TO_TCHAR( msg->szRoleName );
	Friend.RoleId = msg->dwRoleID;
	if(Friend.RoleId <= 0)
		return;

	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( msg->dwRoleID );

	if(playerInfo)
	{		
		Friend.ActorName = playerInfo->_RoleName;
		Friend.ClassId = playerInfo->_RoleClass;
		Friend.Sex = playerInfo->_Sex;		
	}
	else
	{
		//Update when the protocol is ready, use default values for now
	}
	if(!FriendManager::getSingleton().InMyEnemyList(msg->dwRoleID))
	{
		FriendManager::getSingleton().AddNewEnemy(Friend);
		//MMOUI::GetUIManager()->delegateUpdateEnemyList();
		MMOGameEvent_Any evt(GameEvent_UpdateEnemyContent);
		MMOUI::PostGameEvent(&evt);
	}
}

void CMessageHandler::Parse_m2c_personal_enemy_del(const char* buf, UINT32 size)
{
	M2C_PERSONAL_ENEMY_DEL* msg = (M2C_PERSONAL_ENEMY_DEL*)buf;
	FriendManager::getSingleton().RemoveEnemy(msg->dwRoleID);
	//MMOUI::GetUIManager()->delegateUpdateEnemyList();
	MMOGameEvent_Any evt(GameEvent_UpdateEnemyContent);
	MMOUI::PostGameEvent(&evt);
}

//----------------------------------------------------------
void CMessageHandler::Parse_m2c_friend_add(const char* buf, UINT32 size)
{
	M2C_FRIEND_ADD* msg = (M2C_FRIEND_ADD*)buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	FString roleName = FString( UTF8_TO_TCHAR(msg->szRoleName) );
	if( player )
	{
		if( msg->bMutual )
		{
			FString text = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.BecomeFriendWith")) , *roleName );
			MMOChatManager::getSingleton().AddSystemMessage( text );
			MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Table,TEXT("MMOGame.Friend.BecomeFriendWith"));
			FriendManager::getSingleton().AddNewFriend( msg->dwRoleID , roleName , 0  );

			// Achievement event: friend count changed
			INT total = 0;
			for (INT i = 0; i < FriendManager::getSingleton().GetFriendGroupNum() ; ++ i )
			{
				if (i !=  AttentionGroupId)
				{
					total += FriendManager::getSingleton().GetFriendNumInGroup(i);
				}
			}
			MMOGameEvent_Achievement evtAch(adFriendCount, total, 0, 0);
			MMOUI::PostGameEvent(&evtAch);
		}
		else
		{
			FString text = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.BecomeAttentionWith")) , *roleName );
			MMOChatManager::getSingleton().AddSystemMessage( text );
			MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Table,TEXT("MMOGame.Friend.BecomeAttentionWith"));
		
			FriendManager::getSingleton().AddNewFriend( msg->dwRoleID , roleName , AttentionGroupId );
		}

		// Guide quest
		MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Friend);
	}

}

//--------------------------------------------unfollow the other party--------------
void CMessageHandler::Parse_m2c_friend_del(const char* buf, UINT32 size)
{
	M2C_FRIEND_DEL* msg = ( M2C_FRIEND_DEL* )buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player )
	{
		FriendManager::getSingleton().DeleteAttention( msg->dwRoleID );
	}
}

//--------------------------------------------unfollowed by the other party--------------
void CMessageHandler::Parse_m2c_friend_deleted_by(const char* buf, UINT32 size)
{
	M2C_FRIEND_DELETED_BY* msg = ( M2C_FRIEND_DELETED_BY* )buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player )
	{
		
		if( FriendManager::getSingleton().IsMyFriend( msg->dwRoleID ))
		{
			MMOFriendMember* member = FriendManager::getSingleton().GetMyFriendInfo(msg->dwRoleID);
			if (member)
			{
				member->Location = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.Attention") );
			}

			INT GroupId = FriendManager::getSingleton().GetFriendGroupId( msg->dwRoleID );
			if( GroupId >= 0 )
			{
				FriendManager::getSingleton().MovePlayerToGroup( msg->dwRoleID , GroupId , AttentionGroupId );
			}

			
		}
	}
}

//----------------------------------------------------------
void CMessageHandler::Parse_m2c_friend_added_by(const char* buf, UINT32 size)
{
	M2C_FRIEND_ADDED_BY* msg = (M2C_FRIEND_ADDED_BY*)buf;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( player )
	{
		FString roleName(UTF8_TO_TCHAR(msg->szRoleName));
		if( FriendManager::getSingleton().IsMyAttention( msg->dwRoleID) ) //I also follow him, so move from follow to friend
		{
			MMOFriendMember* member = FriendManager::getSingleton().GetAttentionInfo(msg->dwRoleID);
			if (member)
			{
				member->Location = UGlobalMapInfo::GetSingleton()->FindMapInfo( member->SceneTempID).MapDisplayName;
			}
			
			FString text = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.BecomeFriendWith")) , *roleName );
			MMOChatManager::getSingleton().AddSystemMessage( text );
			MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Table,TEXT("MMOGame.Friend.BecomeFriendWith"));

			FriendManager::getSingleton().MovePlayerToGroup( msg->dwRoleID , AttentionGroupId , DefaultFriendGroupId );

			// Achievement event: friend count changed
			INT total = 0;
			for (INT i = 0; i < FriendManager::getSingleton().GetFriendGroupNum(); ++ i)
			{
				if (i !=  AttentionGroupId)
				{
					total += FriendManager::getSingleton().GetFriendGroupNum();
				}
			}
			MMOGameEvent_Achievement evtAch(adFriendCount, total, 0, 0);
			MMOUI::PostGameEvent(&evtAch);

			FriendManager::getSingleton().AddedAttentionBy( msg->dwRoleID , roleName );
		}
		else if( FriendManager::getSingleton().IsMyFriend( msg->dwRoleID )) //This case is illegal
		{
			
		}
		else  // Remind me whether to follow him
		{
			FriendManager::getSingleton().AddedAttentionBy( msg->dwRoleID , UTF8_TO_TCHAR(msg->szRoleName) );
		}
	}
	
}

//---------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_friend_chat(const char* buf, UINT32 size)
{
	M2C_FRIEND_CHAT* msg = (M2C_FRIEND_CHAT*)buf;

	MMOFacade::getSingleton().NotifyFriendChatMsg( msg->dwFrom , UTF8_TO_TCHAR(msg->szMessage) );
}

//---------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_shield_list(const char* buf, UINT32 size)
{
	G2C_SYNC_SHIELD_LIST* msg = (G2C_SYNC_SHIELD_LIST*)buf;
	//UBOOL fullPlayerName = TRUE;

	for ( INT i = 0 ; i < msg->byRoleCount ; i++ )
	{
		if(msg->arrRoleID[i] <= 0)
			continue;

		MMOFriendMember role(EC_EventParm);
		
		role.RoleId = msg->arrRoleID[i];
		role.ActorName = TEXT("Unknown player");
 
		PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( role.RoleId );

		if(playerInfo)
		{
			role.ActorName = playerInfo->_RoleName;
			role.ClassId = playerInfo->_RoleClass;
			role.Sex = playerInfo->_Sex;			
		}
		else
		{
			//MMOFacade::getSingleton().RequestPlayerBaseInfo( role.RoleId );			
		}

		FriendManager::getSingleton().AddBlackList(role);		
	}
	//MMOUI::GetUIManager()->delegateUpdateBlackList();
	MMOGameEvent_Any evt(GameEvent_UpdateBlackList);
	MMOUI::PostGameEvent(&evt);
}

//---------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_shield_ack(const char* buf, UINT32 size)
{
	G2C_SHIELD_ACK* msg = (G2C_SHIELD_ACK*)buf;
	MMOFriendMember role;
	appMemzero( &role , sizeof(MMOFriendMember) );
	
	role.RoleId = msg->dwRoleID;
	role.ActorName = msg->szRoleName;

	PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( role.RoleId );

	if(playerInfo)
	{
		role.ActorName = playerInfo->_RoleName;
		role.ClassId = playerInfo->_RoleClass;
		role.Sex = playerInfo->_Sex;		
	}
	else
	{
		//MMOFacade::getSingleton().RequestPlayerBaseInfo( role.RoleId );			
	}

	FriendManager::getSingleton().AddBlackList(role);

	MMOGameEvent_Any evt(GameEvent_UpdateBlackList);
	MMOUI::PostGameEvent(&evt);
}

//---------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_unshield_ack(const char* buf, UINT32 size)
{
	G2C_UNSHIELD_ACK* msg = (G2C_UNSHIELD_ACK*)buf;
	TArray<MMOFriendMember>& blacklist = FriendManager::getSingleton().GetBlackList();

	for ( INT i = 0 ; i < FriendManager::getSingleton().GetBlackNum() ; i++ )
	{
		if( blacklist(i).RoleId == msg->dwRoleID )
		{
			blacklist.Remove(i);
			break;
		}
	}
	//MMOUI::GetUIManager()->delegateUpdateBlackList();
	MMOGameEvent_Any evt(GameEvent_UpdateBlackList);
	MMOUI::PostGameEvent(&evt);
}

void CMessageHandler::SendMoveToBlackList( INT roleId , FString roleName )
{
	C2M_SHIELD msg;
	msg.wMsg = c2m_shield;
	//Using either roleid or rolename is enough
	//If roleid != 0 use name
	msg.dwRoleID = roleId;
	strxcpy( msg.szRoleName , *roleName, ROLE_NAME_LEN );
	_Send( &msg, sizeof(msg));
}

void CMessageHandler::SendRemoveFromBlackList( INT roleId )
{
	C2G_UNSHIELD msg;
	msg.wMsg = c2g_unshield;
	msg.dwRoleID = roleId;
	_Send( &msg, sizeof(msg));
}
void CMessageHandler::SendAddEnemy(INT nRoleID)
{
	C2M_PERSONAL_ENEMY_ADD msg;
	msg.wMsg = c2m_personal_enemy_add;
	msg.dwRoleID = nRoleID;
	_Send( &msg, sizeof(msg));
}
void CMessageHandler::SendDeleteEnemy(INT nRoleID)
{
	C2M_PERSONAL_ENEMY_DEL msg;
	msg.wMsg = c2m_personal_enemy_del;
	msg.dwRoleID = nRoleID;
	_Send( &msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_sync_msg_paopao(const char* buf, UINT32 size)
{
	G2C_SYNC_MSG_PAOPAO* msg = (G2C_SYNC_MSG_PAOPAO*)buf;
	if(msg)
	{
		if(msg->dwParam1 > 0&&(!FriendManager::getSingleton().InMyEnemyList(msg->dwParam1)))
		{
			AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(playerCtrl == NULL || mainPlayer == NULL || (mainPlayer->ServerID == msg->dwParam1))
				return;

			FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
			if( pMapInfo == NULL )
				return;
		
			INT mapType = pMapInfo->ServerMapType;

			if( mapType == E_est_Static || mapType == E_est_Line || mapType == E_est_Guild )
			{
				//Field map
				AMMOActor* pkActor = MMOFacade::getSingleton().GetCharacter(msg->dwParam1);
				if(pkActor)
				{			
					 if (MMOUI::GetUIManager()->DeclineShowEnemy)
					 {
						
					 }
					 else
					 {
						FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.AddNewEnemy"))), *(pkActor->ActorName));
						MMOMsgButtonManager::getSingleton().AddNewButton(text, msg->dwParam1, pkActor->ActorName, GameEvent_CanAddEnemy, E_MsgBtnShow_Enemy);
					 }
				}
			}
		}
	}
}

void CMessageHandler::Parse_m2c_sync_friend_intimacy(const char* buf, UINT32 size)
{
	M2C_SYNC_FRIEND_INTIMACY* msg = (M2C_SYNC_FRIEND_INTIMACY*)buf;
	if (msg)
	{
		FriendManager::getSingleton().UpdateFriendIntimacy(msg->dwFriendRoleID,msg->nIntimacy);
	}
}

void CMessageHandler::Parse_m2c_h5_url(const char* buf, UINT32 size)
{
	M2C_H5_URL* msg = (M2C_H5_URL*)buf;
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player )
	{
		return;
	}
	//ShellExecute(NULL, L"open", L"http://www.cqyh.info", NULL, NULL, SW_SHOWNORMAL);
	ShellExecuteA(NULL,"open", "..\\..\\chrome32_60.0.3112.78\\chrome.exe", msg->url, NULL, SW_SHOWNORMAL);
	/*INT GroupNum = msg->byGroupCount;
	for ( INT i = 0 ; i < GroupNum ; i++ ) //
	{
		INT GroupId = msg->stGroupInfo[i].byGroupID;
		if( FriendManager::getSingleton().GetFriendGroup(GroupId) )
		{
			FriendManager::getSingleton().GetFriendGroup(GroupId)->GroupName = UTF8_TO_TCHAR( msg->stGroupInfo[i].szGroupName);
		}		

		MMOGameEvent_OneInt evt( GameEvent_UpdateFriendGroup , GroupId );
		MMOUI::PostGameEvent( &evt );
	}*/
	
}
