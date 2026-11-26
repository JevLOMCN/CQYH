/*=============================================================================
	ItemManager.cpp
=============================================================================*/
#include "stdafx.h"
#include "FriendManager.h"

#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "MMOAchievementManager.h"
#include "net/message_handler.h"
#include "CommonGameEvents.h"
#include "MMOMsgButtonManager.h"
#include "MMOChatManager.h"
#include "LinkManager.h"

#define KillInfoFileName TEXT("KillInfo")

FriendManager::FriendManager()
{
	startTime = 0.0f;
	startTick = FALSE;
	nTradeNeedIntimacyValue = 0;
	Init();

}

void FriendManager::Init()
{
	FriendGroupList.AddZeroed( AttentionGroupId + 1 );
	
	MMOUI::RegisterEventHandler( this , GameEvent_AttentionByMsg );
	MMOUI::RegisterEventHandler( this , GameEvent_FriendAdd );
	MMOUI::RegisterEventHandler( this , GameEvent_CanAddEnemy );
	//LoadNearContact();
	MMOUI::RegisterEventHandler( this, GameEvent_PlayerBaseInfo);
	GConfig->GetString( TEXT("ChatColor"), TEXT("KillTime") , killTime , GUIColorIni);
	FString Intimacy = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TradeNeedIntimacyValue"));
	nTradeNeedIntimacyValue = appAtoi(*Intimacy);
}

void FriendManager::LoadNearContact()
{
	//account related, must be read after login once friend data is available, so the info can match
	FArchive* archive = NULL;
	if( MMOFacade::getSingleton().BeginLoadAccountBinary( &archive, TEXT("nearContact") ))
	{
		INT data = 0;
		archive->Serialize(&data, sizeof(data));
		const INT totalSize = archive->TotalSize();
		if (archive->IsError() || data < 0 || totalSize < sizeof(INT) ||
			(UINT)data > (totalSize - sizeof(INT)) / sizeof(INT))
			data = 0;
		for ( INT i = 0 ; i < data ; ++i )
		{
			INT roleId = -1;
			archive->Serialize(&roleId, sizeof(roleId));
			if (archive->IsError())
				break;
			if( roleId >= 0 )
			{
				MMOFriendMember mem = GetFriendInfoByRoleId(roleId);
				if( !mem.ActorName.IsEmpty() ) 
				{
					NearContactList.AddItem(mem);
				}
			}

		}

		if( NearContactList.Num() != 0 )
		{
			//MMOUI::GetUIManager()->delegateUpdateNearContactList();
			MMOGameEvent_UpdateNearContact evt;
			MMOUI::PostGameEvent(&evt);
		}
		
	}
	MMOFacade::getSingleton().EndLoadAccountBinary( archive );
}

void FriendManager::SaveNearContact()
{
	FArchive* pArchive = NULL;
	if( MMOFacade::getSingleton().BeginExportAccountData( &pArchive , TEXT("nearContact") ) )
	{
		INT size = NearContactList.Num();
		pArchive->Serialize( (void*)(&size) , sizeof(INT) );
		for ( INT i = 0 ; i < size ; i++ )
		{
			pArchive->Serialize( (void*)(& NearContactList(i).RoleId ) , sizeof(INT) );

		}
		
		//ar->Serialize( (void*) str.c_str() , str.length()*sizeof(char) );
		//pArchive->ByteOrderSerialize( pData , lDataLength );
	}
	MMOFacade::getSingleton().EndExportAccountData( pArchive );
}

void FriendManager::Shutdown()
{
	//SaveNearContact();
}

void FriendManager::ClearAll()
{
	//TArray<MMOFriendGroup> FriendGroupList;
	for ( INT i = 0 ; i < FriendGroupList.Num() ; i++ )
	{
		FriendGroupList(i).FriendList.Empty();
		if (FriendGroupList(i).IsDefault == TRUE)
			continue;
		FriendGroupList(i).GroupName.Empty();
		FriendGroupList(i).IsDefault = FALSE;
	}

	EnemyList.Empty();
	BlackList.Empty();
	NearContactList.Empty();

	
}

void FriendManager::ClearSearchList()
{
	for (INT i = 0; i < ARRAY_COUNT(SearchList); ++i)
		SearchList[i] = MMOFriendMember(EC_EventParm);
}

//----------------------------------------------------------------------------------------------------------------------
void FriendManager::AddNewFriend( INT RoleId , FString FriendName , INT GroupId )
{
	if( GroupId < 0 || GroupId >= FriendGroupList.Num() )
		return;
		
	MMOFriendMember item(EC_EventParm);
	item.RoleId = RoleId;
	item.ActorName = FriendName;
	item.AbbName = MMOFacade::CheckName( item.ActorName );
	FriendGroupList(GroupId).FriendList.AddItem( item );

	MMOGameEvent_OneInt evt( GameEvent_UpdateFriendGroup , GroupId );
	MMOUI::PostGameEvent( &evt );

	// add by zhangrui look up other characters' info here just in case (right-click menu operations may need this info)
	MMOFacade::getSingleton().RequestPlayerBaseInfo(RoleId);

	//MMOFacade::getSingleton().NotifyUpdateFriendGroup(GroupId);
}

void FriendManager::AddBlackList( MMOFriendMember member )
{
	BlackList.AddItem(member);
}

TArray<MMOFriendMember>& FriendManager::GetBlackList()
{
	return BlackList;
}

INT FriendManager::GetBlackNum()
{
	return BlackList.Num();
}

void FriendManager::PushToFriendGroup( INT groupId , MMOFriendMember member )
{
	if( groupId >= 0 && FriendGroupList.Num() > groupId )
	{
		FriendGroupList(groupId).FriendList.AddItem(member);
	}
}

//----------------------------------------------------------------------------------------------------------------------
INT FriendManager::GetEmptyFriendGroupId( )
{
	INT ID = -1;
	for ( INT i = 0 ; i < FriendGroupList.Num() ; i++ )
	{
		if( FriendGroupList(i).GroupName.IsEmpty() )
		{
			ID = i;
			break;
		}
	}
	return ID;
}

//----------------------------------------------------------------------------------------------------------------------
INT FriendManager::GetFriendNumInGroup( INT groupId )
{
	if( groupId >= 0 && FriendGroupList.Num() > groupId )
	{
		return FriendGroupList(groupId).FriendList.Num();
	}
	return 0;
}

INT FriendManager::GetTotalFriendNum()
{
	INT count = 0;
	for ( INT i = 0 ; i < FriendGroupNum ; i++ )
	{
		count += FriendGroupList(i).FriendList.Num();		
	}
	return count;
}

//note, this online actually means not offline----------------------------------------------------------------------------------------------------------------------
INT FriendManager::GetOnlineFriendNumInGroup( INT groupId )
{
	if( groupId >= 0 && FriendGroupList.Num() > groupId )
	{
		INT res = 0;
		for ( INT i = 0 ; i < FriendGroupList(groupId).FriendList.Num() ; i++ )
		{
			if( FriendGroupList(groupId).FriendList(i).Online != FS_OFFLINE )
			{
				res++;
			}
		}
		return res;
	}
	return 0;
}
//----------------------------------------------------------------------------------------------------------------------

UBOOL FriendManager::IsMyFriend( INT RoleId )
{
	for ( INT i = 0 ; i < FriendGroupNum ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == RoleId )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

MMOFriendMember* FriendManager::GetMyFriendInfo(INT roleId)
{
	for ( INT i = 0 ; i < FriendGroupNum ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == roleId )
			{
				return &FriendGroupList( i ).FriendList( j );
			}
		}
	}
	return NULL;
}

void FriendManager::NotifyNewContact( INT roleId )
{
	//UBOOL inList = FALSE;
	removeFromNerContactList(roleId);

	MMOFriendMember mem = GetFriendInfoByRoleId(roleId);

	NearContactList.InsertItem( mem , 0 );
	//there is an upper limit here
	//MMOUI::GetUIManager()->delegateUpdateNearContactList();
	MMOGameEvent_UpdateNearContact evt;
	MMOUI::PostGameEvent(&evt);

	SaveNearContact();
}

UBOOL FriendManager::InMyBlackList( INT roleId )
{
	for ( INT j = 0 ; j < BlackList.Num() ; j++ )
	{
		if( BlackList( j ).RoleId == roleId )
		{
			return TRUE;
		}
	}
	return FALSE;
}

UBOOL FriendManager::InMyEnemyList( INT roleId )
{
	for ( INT j = 0 ; j < EnemyList.Num() ; j++ )
	{
		if( EnemyList( j ).RoleId == roleId )
		{
			return TRUE;
		}
	}
	return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------

void FriendManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch( evt->GetEventType() )
	{
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* baseInfo = (MMOGameEvent_PlayerBaseInfo*)evt;

			if(baseInfo)
			{
				PlayerBaseInfo *playerInfo = baseInfo->_PlayerBaseInfo;
				if( InMyBlackList( baseInfo->_RoldID ) )
				{
					MMOFriendMember* member = GetBlackMemberByRoleId( baseInfo->_RoldID );

					if( member && playerInfo )
					{
						member->ActorName = playerInfo->_RoleName;
						member->ClassId = playerInfo->_RoleClass;
						member->Sex = playerInfo->_Sex;
						member->Lv = playerInfo->_Level;

						MMOGameEvent_Any evt(GameEvent_UpdateBlackList);
						MMOUI::PostGameEvent(&evt);
					}
				}
				else if( InMyEnemyList( baseInfo->_RoldID) )
				{
					MMOFriendMember* member = GetEnemyMemberByRoleId( baseInfo->_RoldID );

					if( member && playerInfo )
					{
						member->ActorName = playerInfo->_RoleName;
						member->ClassId = playerInfo->_RoleClass;
						member->Sex = playerInfo->_Sex;
						//member->Lv = playerInfo->_Level;

						MMOGameEvent_Any evt(GameEvent_UpdateEnemyContent);
						MMOUI::PostGameEvent(&evt);
						//MMOUI::GetUIManager()->delegateUpdateEnemyList();
					}


				}

			}
			break;
		}
	case GameEvent_AttentionByMsg:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if( cb->Agree )
				{
					AddFriendOk(cb->Box);
				}

			}
			break;
		}
	case GameEvent_FriendAdd:
		{
			MMOGameEvent_MsgboxCallBack* msgboxEvt = (MMOGameEvent_MsgboxCallBack*)(evt);
			if( msgboxEvt && msgboxEvt->Box )
			{
				if( msgboxEvt->Agree )
				{
					UGFxObject* inputClip = msgboxEvt->Box->GetObject( "textInput" );
					if( inputClip )
					{
						
						FString InputText = inputClip->GetString("text");
						if( !InputText.IsEmpty() )
						{
							FriendManager::getSingleton().RequestAddAttention( 0 , InputText );
						}
						else
						{
							MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.EmptyName") ) , MsgInfoType_Table ,TEXT("MMOGame.Friend.EmptyName"));
						}
						
					}

					
					//AddFriendOk(cb->Box);
				}

			}
			break;
		}
	case  GameEvent_CanAddEnemy:
		{
			MMOGameEvent_MsgboxCallBack* msgboxEvt = (MMOGameEvent_MsgboxCallBack*)(evt);
			if( msgboxEvt && msgboxEvt->Box )
			{
				if( msgboxEvt->Agree )
				{
					INT nRoleID = msgboxEvt->Box->GetInt(TEXT("iteminfoid"));

					AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
					if( pl && pl->ServerID != nRoleID)
					{
						if(nRoleID > 0)
							CMessageHandler::GetInstance().SendAddEnemy(nRoleID);
					}

				}
			}
			break;
		}
	}
	

}

//----------------------------------------------------------------------------------------------------------------------
void FriendManager::RequestDelAttention(INT RoleId)
{
	CMessageHandler::GetInstance().RequestDelFriend(RoleId);
}

//----------------------------------------------------------------------------------------------------------------------
void FriendManager::RequestAddAttention( INT RoleId , const FString& TargetName )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( RoleId == player->ServerID || appStrcmp( *TargetName , *player->ActorName ) == 0 )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.CantAddSelf") ),MsgInfoType_Table,TEXT("MMOGame.Friend.CantAddSelf") ); 
		return;
	}
	CMessageHandler::GetInstance().RequestAddFriend( TargetName , RoleId);
}

//----------------------------------------------------------------------------------------------------------------------
void FriendManager::ChangeFriendGroup(INT RoleId,INT GroupId)
{
	//this protocol only informs the server, so the client handles it directly
	CMessageHandler::GetInstance().ChangeFriendGroup(RoleId , GroupId);


	INT OriGroupId = GetFriendGroupId( RoleId );
	MovePlayerToGroup( RoleId , OriGroupId , GroupId );
}

void FriendManager::AddedAttentionBy(INT RoleId,const FString& RoleName)
{

	FString sz = RoleName; 


	if( FriendManager::getSingleton().IsMyAttention( RoleId ) || FriendManager::getSingleton().IsMyFriend( RoleId ) )
	{
		sz += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AddAttentionByNoAsk") );
		MMOMsgButtonManager::getSingleton().AddNewButton( sz , RoleId, RoleName , GameEvent_NoMsg , E_MsgBtnShow_Friend);
	}
	else
	{
		sz += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AddAttentionBy") );
		MMOMsgButtonManager::getSingleton().AddNewButton( sz , RoleId , RoleName , GameEvent_AttentionByMsg , E_MsgBtnShow_Friend );
	}
	
	
}
//------------------------------------------------------------------------

MMOFriendMember* FriendManager::GetAttentionInfo(INT roleId)
{
	for ( INT j = 0 ; j < FriendGroupList( AttentionGroupId ).FriendList.Num() ; j++ )
	{
		if( FriendGroupList( AttentionGroupId ).FriendList( j ).RoleId == roleId )
		{
			return &FriendGroupList( AttentionGroupId ).FriendList( j );
		}
	}		
	return NULL;
}

UBOOL FriendManager::IsMyAttention( int RoleId )
{
	for ( INT j = 0 ; j < FriendGroupList( AttentionGroupId ).FriendList.Num() ; j++ )
	{
		if( FriendGroupList( AttentionGroupId ).FriendList( j ).RoleId == RoleId )
		{
			return TRUE;
		}
	}		
	return FALSE;
}
//
INT FriendManager::GetFriendGroupId( INT RoleId )
{
	//friend list
	for ( INT i = 0 ; i < FriendGroupNum ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == RoleId )
			{
				return i;
			}
		}
	}

	//Follow
	if (IsMyAttention(RoleId))
	{
		return AttentionGroupId;
	}

	//Enemy
	MMOFriendMember* enemy = FriendManager::getSingleton().GetEnemyMemberByRoleId(RoleId);
	if (enemy)
	{
		return EnemyGroupId;
	}

	enemy = FriendManager::getSingleton().GetBlackMemberByRoleId(RoleId);
	if (enemy)
	{
		return BlackGroupId;
	}

	enemy = FriendManager::getSingleton().GetNearMemberByRoleId(RoleId);
	if (enemy)
	{
		return NearGroupId;
	}
	return -1;
}
//----------------------------------------------------------------------------------------------------------------------
void FriendManager::MovePlayerToGroup( INT roleId , INT fromGroupId , INT toGroupId )
{
	if (fromGroupId < 0 || toGroupId < 0 || fromGroupId >= FriendGroupList.Num()
		|| toGroupId >= FriendGroupList.Num() || fromGroupId == toGroupId)
		return;
	for ( INT j = 0 ; j < FriendGroupList( fromGroupId ).FriendList.Num() ; j++ )
	{
		if( FriendGroupList( fromGroupId ).FriendList( j ).RoleId == roleId )
		{
			MMOFriendMember FMember = FriendGroupList( fromGroupId ).FriendList(j);
			FriendGroupList( toGroupId ).FriendList.AddItem( FMember );
			//MMOFacade::getSingleton().NotifyUpdateFriendGroup( toGroupId );
			
			FriendGroupList( fromGroupId ).FriendList.Remove( j );
			//MMOFacade::getSingleton().NotifyUpdateFriendGroup( fromGroupId );

			MMOGameEvent_OneInt evt1( GameEvent_UpdateFriendGroup , toGroupId );
			MMOUI::PostGameEvent( &evt1 );
			MMOGameEvent_OneInt evt2( GameEvent_UpdateFriendGroup , fromGroupId );
			MMOUI::PostGameEvent( &evt2 );

			return;
		}
	}

}

//
void FriendManager::MoveGroupPlayerToGroup( INT fromGroupId , INT toGroupId )
{
	if( fromGroupId < 0 || toGroupId < 0
		|| FriendGroupList.Num() <= fromGroupId || FriendGroupList.Num() <= toGroupId )
	{
		return;
	}

	if( fromGroupId == toGroupId )
	{
		return;
	}

	if( FriendGroupList( fromGroupId ).FriendList.Num() == 0 )
	{
		//if empty, do not continue, otherwise it recurses infinitely
		return;
	}

	for ( INT j = 0 ; j < FriendGroupList( fromGroupId ).FriendList.Num() ; j++ )
	{
		MMOFriendMember FMember = FriendGroupList( fromGroupId ).FriendList(j);
		FriendGroupList( toGroupId ).FriendList.AddItem( FMember );
	}

	FriendGroupList( fromGroupId ).FriendList.Remove( 0 , FriendGroupList( fromGroupId ).FriendList.Num() );

	MMOGameEvent_OneInt evt1( GameEvent_UpdateFriendGroup , toGroupId );
	MMOUI::PostGameEvent( &evt1 );
	MMOGameEvent_OneInt evt2( GameEvent_UpdateFriendGroup , fromGroupId );
	MMOUI::PostGameEvent( &evt2 );
	//MMOFacade::getSingleton().NotifyUpdateFriendGroup( toGroupId );
	//MMOFacade::getSingleton().NotifyUpdateFriendGroup( fromGroupId );
}

//--------------------------------------------//unfollow---------------------------------------------------------
void FriendManager::DeleteAttention( INT roleId )
{
	for ( INT i = 0 ; i < FriendGroupList.Num() ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == roleId )
			{				
				FString text = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Friend.DeleteAttention")) , *FriendGroupList(i).FriendList(j).ActorName );
				MMOChatManager::getSingleton().AddSystemMessage( text );
				MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Table,TEXT("MMOGame.Friend.DeleteAttention"));

				FriendGroupList( i ).FriendList.Remove( j );
				//MMOFacade::getSingleton().NotifyUpdateFriendGroup( i );

				MMOGameEvent_OneInt evt( GameEvent_UpdateFriendGroup , i );
				MMOUI::PostGameEvent( &evt );

				break;
			}
		}
	}
	//delete from recent contacts
	removeFromNerContactList(roleId);
	MMOGameEvent_UpdateNearContact evt;
	MMOUI::PostGameEvent(&evt);
}
void FriendManager::AddNewEnemy(MMOFriendMember& member)
{
	EnemyList.AddItem(member);

	FLOAT HudNameDisplayRangeSquared2D = 0.0f;
	GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);

	UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
	if (sceneHint)
	{
		AMMOPlayer* pl = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(member.RoleId));
		ULocalPlayer* localPlayer = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
		if(pl && localPlayer)
		{
			sceneHint->UpdateHints_Player(pl, localPlayer, TRUE, HudNameDisplayRangeSquared2D);
		}
	}

}
//--------------------------------------------//unfollow---------------------------------------------------------
void FriendManager::RemoveEnemy( INT roleId )
{
	for ( INT i = 0 ; i < EnemyList.Num() ; i++ )
	{
		if( EnemyList(i).RoleId == roleId )
		{
			EnemyList.Remove(i);
			break;
		}
	}

	FLOAT HudNameDisplayRangeSquared2D = 0.0f;
	GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);
	UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
	if (sceneHint)
	{
		AMMOPlayer* pl = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(roleId));
		ULocalPlayer* localPlayer = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
		if(pl && localPlayer)
		{
			sceneHint->UpdateHints_Player(pl, localPlayer, TRUE, HudNameDisplayRangeSquared2D);
		}
	}
}

void FriendManager::DeleteFriendGroup( INT groupId )
{
	MoveGroupPlayerToGroup( groupId , DefaultFriendGroupId );

	MMOFriendGroup* fGroup = GetFriendGroup(groupId);
	if(!fGroup)
	{
		return;
	}
	fGroup->GroupName.Empty();

	MMOGameEvent_OneInt evt( GameEvent_FriendDelGroup , groupId );
	MMOUI::PostGameEvent(&evt);

	//Send message
	CMessageHandler::GetInstance().ChangeFriendGroupName(""  , groupId );
}

//-----------------------------------------------------------------------------------------------------
MMOFriendMember FriendManager::GetFriendInfoByRoleId( INT roleId )
{
	MMOFriendMember FriendInfo(EC_EventParm);
	for ( INT i = 0 ; i < FriendGroupNum ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == roleId )
			{
				return FriendGroupList( i ).FriendList( j );
			}
		}
	}
	return FriendInfo;
}

MMOFriendMember* FriendManager::GetBlackMemberByRoleId( INT roleId )
{
	for ( INT i = 0 ; i < BlackList.Num() ; i++ )
	{
		if( BlackList(i).RoleId == roleId )
		{
			return &BlackList(i);
		}		
	}
	return NULL;
}

MMOFriendMember* FriendManager::GetEnemyMemberByRoleId( INT roleId )
{
	for ( INT i = 0 ; i < EnemyList.Num() ; i++ )
	{
		if( EnemyList(i).RoleId == roleId )
		{
			return &EnemyList(i);
		}		
	}
	return NULL;
}

MMOFriendMember* FriendManager::GetNearMemberByRoleId( INT roleId )
{
	for ( INT i = 0 ; i < NearContactList.Num() ; i++ )
	{
		if( NearContactList(i).RoleId == roleId )
		{
			return &NearContactList(i);
		}		
	}
	return NULL;
}

//-----------------------------------------------------------------------------------------------------
MMOFriendMember* FriendManager::GetFriendGroupMember( INT groupId , INT pos )
{
	if( groupId >= 0 && pos >= 0 && FriendGroupList.Num() > groupId
		&& FriendGroupList(groupId).FriendList.Num() > pos )
	{
		return &FriendGroupList(groupId).FriendList(pos);
	}
	
	return NULL;
}

MMOFriendMember* FriendManager::GetFriendGroupMemberById( INT groupId , INT id )
{
	if (groupId >= 0 && groupId < FriendGroupList.Num())
	{
		for ( INT i = 0 ; i < FriendGroupList( groupId ).FriendList.Num() ; i++ )
		{
			if( FriendGroupList( groupId ).FriendList( i ).RoleId == id )
			{
				return &FriendGroupList( groupId ).FriendList( i );
			}
		}
	}
	

	return NULL;
}

MMOFriendMember* FriendManager::GetBlacklistMember( INT pos )
{
	if( pos >= 0 && BlackList.Num() > pos )
	{
		return &BlackList(pos);
	}

	return NULL;
}

MMOFriendMember* FriendManager::GetEnemylistMember( INT pos )
{
	if( pos >= 0 && EnemyList.Num() > pos )
	{
		return &EnemyList(pos);
	}

	return NULL;
}

MMOFriendMember* FriendManager::GetNearContact( INT pos )
{
	if( NearContactList.Num() > pos && pos >= 0 )
	{
		return &NearContactList(pos);
	}

	return NULL;
}

MMOFriendMember* FriendManager::GetSearchFriend( INT pos )
{
	if( UCONST_MaxSearchPanelItem > pos && pos >= 0 )
	{
		return &SearchList[pos];
	}

	return NULL;
}

static QSORT_RETURN CDECL SortFriend( const MMOFriendMember* fa, const MMOFriendMember* fb )
{
	if (fa && fb)
	{
		if (fa->Online < fb->Online)
			return -1;
		if (fa->Online > fb->Online)
			return 1;
		if (fa->AbbName < fb->AbbName)
			return -1;
		if (fb->AbbName < fa->AbbName)
			return 1;
	}
	return 0;
}

MMOFriendGroup* FriendManager::GetFriendGroup( INT groupId )
{
	if( groupId >= 0 && FriendGroupList.Num() > groupId )
	{
		return &FriendGroupList(groupId);
	}
	
	return NULL;
}

void FriendManager::SortFriendGroup( INT groupId )
{
	if( groupId >= 0 && FriendGroupList.Num() > groupId )
	{
		INT num = FriendGroupList(groupId).FriendList.Num();
		if( num > 0 )
		{
			appQsort( &FriendGroupList(groupId).FriendList(0) , num , sizeof(MMOFriendMember) , (QSORT_COMPARE)SortFriend );
		}
		
	}
}

void FriendManager::SortAllFriend()
{
	for ( INT i = 0 ; i < FriendGroupNum ; i++ )
	{
		SortFriendGroup( i );
	}
}

void FriendManager::AddFriendOk( UGFxObject* mc )
{
	if(mc)
	{
		INT MsgInfoId = mc->GetInt( TEXT("iteminfoid") );
		RequestAddAttention( MsgInfoId , TEXT("") );
	}

}
const TArray<MMOFriendGroup>& FriendManager::GetAllFriendList()const
{
	return FriendGroupList;
}

void FriendManager::AddKillInfoList( UBOOL iamKiller , INT killerRoleID , INT victimRoleID )
{

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player )
	{
		return;
	}

	FString time = _GetCurTimeString();
	FString times  = FString::Printf( TEXT("<FONT COLOR='%s'> ") ,*killTime ) + time  + TEXT("</FONT>");

	FString map,mapIcon;
	_GetCurMapString( map , mapIcon );

	MMOKillInfo killInfo;
	if( iamKiller )
	{
		AMMOActor* victimRole = MMOFacade::getSingleton().GetCharacter( victimRoleID );
		if(victimRole)
		{
			
			FString playerName = LinkManager::HtmlLink_KillInfoPlayer( victimRole->ActorName , victimRoleID );
			killInfo.ShowText = mapIcon + TEXT("<br>") + times + TEXT("<br>") + FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.IKill")) , *map , *playerName ); 
		}		
	}
	else
	{
		AMMOActor* killerRole = MMOFacade::getSingleton().GetCharacter( killerRoleID );
		FString playerName = "";

		if(killerRole && !killerRole->ActorName.IsEmpty())
		{
			playerName = LinkManager::HtmlLink_KillInfoPlayer( killerRole->ActorName , killerRoleID );
			
		}	
		else
		{
			PlayerBaseInfo* player = MMOFacade::getSingleton().RequestPlayerBaseInfo(killerRoleID);

			if (player)
			{
				playerName = LinkManager::HtmlLink_KillInfoPlayer( player->_RoleName , killerRoleID );
			}
		}
		killInfo.ShowText = mapIcon + TEXT("<br>") + times + TEXT("<br>") + FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.Killed")) , *map , *playerName ); 
	}
	

	KillInfoListBuff.AddItem(killInfo);
	startTime = 0.0f;
	startTick = TRUE;
}

void FriendManager::AddKilledDropItem( UItem* titem , FClientItemInfo* clientItem  )
{
	if( !titem || !clientItem )
	{
		return;
	}
	//FString time = _GetCurTimeString();
	FString map,mapIcon;
	_GetCurMapString( map , mapIcon );

	
	FString ShowText = /*mapIcon + TEXT("<br>") + time +*/ FString(TEXT("<br>")) + FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.KilledDropItem")) , *map , *LinkManager::HtmlLink_ColorItem( titem , clientItem ) );

	if( KillInfoListBuff.Num() > 0 )
	{
		KillInfoListBuff( KillInfoListBuff.Num()-1 ).ShowText += ShowText;
	}
	else if( KillInfoList.Num() > 0 )
	{
		KillInfoList( KillInfoList.Num()-1 ).ShowText += ShowText;

		MMOGameEvent_Any msg( GameEvent_UpdateKillInfo );
		MMOUI::PostGameEvent(&msg);

		SaveKillInfoData();
	}

}

void FriendManager::SaveKillInfoData()
{
	FArchive* pArchive = NULL;
	if( MMOFacade::getSingleton().BeginExportAccountData( &pArchive , KillInfoFileName ) )
	{
		INT size = KillInfoList.Num();
		pArchive->Serialize( (void*)(&size) , sizeof(INT) );
		for ( INT i = 0 ; i < size ; i++ )
		{
			INT textLen = KillInfoList(i).ShowText.Len()*sizeof(TCHAR);
			pArchive->Serialize( (void*)( &textLen ) , sizeof(INT) );
			pArchive->Serialize( (void*)( KillInfoList(i).ShowText.GetCharArray().GetData() ) , textLen );

		}

	}
	MMOFacade::getSingleton().EndExportAccountData( pArchive );

}

void FriendManager::LoadKillInfoData()
{
	FArchive* archive = NULL;
	KillInfoList.Empty();
	if (MMOFacade::getSingleton().BeginLoadAccountBinary(&archive, KillInfoFileName))
	{
		UINT32 count = 0;
		archive->Serialize(&count, sizeof(count));
		if (archive->IsError() || archive->Tell() > archive->TotalSize() ||
			count > (UINT32)((archive->TotalSize() - archive->Tell()) / sizeof(UINT32)))
			goto Finished;
		for (UINT32 i = 0; i < count; ++i)
		{
			UINT32 bytes = 0;
			archive->Serialize(&bytes, sizeof(bytes));
			TCHAR text[1024] = {0};
			if (archive->IsError() || bytes > 1024 || bytes % sizeof(TCHAR) != 0 ||
				archive->Tell() > archive->TotalSize() || bytes > (UINT32)(archive->TotalSize() - archive->Tell()))
				goto Finished;
			archive->Serialize(text, bytes);
			if (archive->IsError())
				goto Finished;
			MMOKillInfo info;
			info.ShowText = text;
			KillInfoList.AddItem(info);
		}
	}
Finished:
	MMOFacade::getSingleton().EndLoadAccountBinary(archive);
}


FString FriendManager::_GetCurTimeString()
{
	INT Year,Month,DayOfWeek,Day,Hour,Min,Sec,MSec;
	appSystemTime( Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec );
	return FString::Printf( TEXT( "%d-%d-%d %02d:%02d:%02d" ) , Year , Month , Day , Hour , Min , Sec ); 
}

void FriendManager::_GetCurMapString( FString& mapString , FString& mapIcon )
{
	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
	if(!playerCtrl)
	{
		return ;
	}

	FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
	INT mapType = E_est_Invalid;
	if( pMapInfo != NULL )
	{
		mapType = pMapInfo->ServerMapType;
	}

	
	switch(mapType)
	{
	case E_est_Guild:
		{
			mapString = pMapInfo->MapDisplayName;//MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.GuildMap"));
			mapIcon = TEXT("<IMG SRC='img://MMO_UIIcon.KillInfoMapIcon.Guild'>");
		}
		break;
	case E_est_Arena:
		{
			mapString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.ArenaMap"));
			mapIcon = TEXT("<IMG SRC='img://MMO_UIIcon.KillInfoMapIcon.arena'>");
		}
		break;
	case E_est_BattleField:
		{
			mapString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.BattleMap"));
			mapIcon = TEXT("<IMG SRC='img://MMO_UIIcon.KillInfoMapIcon.battle'>");
		}
		break;
	default:
		{
			if( pMapInfo != NULL )
			{
				mapString = pMapInfo->MapDisplayName;
				mapIcon = TEXT("<IMG SRC='img://MMO_UIIcon.KillInfoMapIcon.Normal'>");
			}
		}
		break;
	}
	
}

void FriendManager::TickTime( FLOAT deltaTime )
{
	CTRL_TICK("MMOFriendManager");

	if( startTick )
	{
		deltaTime = Clamp<FLOAT>( deltaTime , 0 , 1 );
		startTime += deltaTime;
		if( startTime > 1 )
		{
			startTick = FALSE;
			startTime = 0.0f;

			//when time is up, show the stored part

			for ( INT i = 0 ; i < KillInfoListBuff.Num() ; i++ )
			{
				if( KillInfoList.Num() >= 100 )
				{
					KillInfoList.Remove(0);
				}
				KillInfoList.AddItem( KillInfoListBuff(i) );
			}

			KillInfoListBuff.Empty();
					
			MMOGameEvent_Any msg( GameEvent_UpdateKillInfo );
			MMOUI::PostGameEvent(&msg);

			SaveKillInfoData();
		}
	}
	

	
}

void  FriendManager::UpdateFriendMemberInfo(MMOFriendMember &FriendInfo,INT nRoleID)
{
	if (FriendInfo.RoleId == 0)
	{
		PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(nRoleID);
		if (playerInfo)
		{
			FriendInfo.RoleId = playerInfo->_RoleID;
			FriendInfo.ActorName = playerInfo->_RoleName;
			FriendInfo.ClassId = playerInfo->_RoleClass;
		}
	}
}

void FriendManager::removeFromNerContactList(int roleId)
{
	for ( INT i = 0 ; i < NearContactList.Num() ; i++ )
	{
		if( NearContactList(i).RoleId == roleId )
		{
			NearContactList.Remove(i);
			break;
		}
	}
}
void FriendManager::UpdateNerContactListMember(MMOFriendMember* mem)
{
	for ( INT i = 0 ; i < NearContactList.Num() ; i++ )
	{
		if( NearContactList(i).RoleId == mem->RoleId )
		{
			NearContactList(i).Online = mem->Online;
			break;
		}
	}
}

void FriendManager::UpdateFriendIntimacy(UINT32 roleId,INT32 nIntimacy)
{
	for ( INT i = 0 ; i < FriendManager::getSingleton().GetFriendGroupNum() ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == roleId )
			{
				FriendGroupList( i ).FriendList( j ).nIntimacy = nIntimacy;
				MMOGameEvent_OneInt evt1( GameEvent_UpdateFriendGroup , i );
				MMOUI::PostGameEvent( &evt1 );
				break;
			}
		}
	}
}

INT FriendManager::GetPlayerIntimacy(UINT32 roldId)
{
	INT intimacy = 0;

	for ( INT i = 0 ; i < FriendManager::getSingleton().GetFriendGroupNum() ; i++ )
	{
		for ( INT j = 0 ; j < FriendGroupList( i ).FriendList.Num() ; j++ )
		{
			if( FriendGroupList( i ).FriendList( j ).RoleId == roldId )
			{
				intimacy = FriendGroupList( i ).FriendList( j ).nIntimacy;
				break;
			}
		}
	}
	return intimacy;
}

BOOL FriendManager::CheckIntimacy(UINT32 roleId)
{
	if (GetPlayerIntimacy(roleId) < nTradeNeedIntimacyValue)
	{
		return FALSE;
	}
	return TRUE;
}