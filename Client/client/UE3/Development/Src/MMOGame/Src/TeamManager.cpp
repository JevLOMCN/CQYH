/*=============================================================================
	ItemManager.cpp
=============================================================================*/
#include "stdafx.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "net/message_handler.h"
#include "TeamManager.h"
#include "MMOMsgButtonManager.h"
#include "CommonGameEvents.h"
#include "MMOChatManager.h"
#include "FSMIO.h"
#include "MMOArenaManager.h"

using namespace FSM;

//IMPLEMENT_CLASS(UTeamManager)
TeamManager::TeamManager()
{
	hasTeam = false;
	ImTeamLeader = false;
	TeamLeaderServeId = -1;
	TeamID = 0;
	_team = NULL;
	_CurAuctionItem = NULL;
	_EndAuctionItemllGuid = 0;
	_AuctionItemColorLimit = ITEMCOLOR_WHITE;

	_teamTargetType = 0;
	_teamTargetId = -1;
	_CurBidMoney = 0;
	_CurBidRoleId = -1;
	InvitePlayerList.Empty();
	InvitedPlayerCache.Empty(MAX_ARCHIVE_INVITE_PLAYER);

	//Options
	TeamAuctionItemColorLimitMap item;
	item.Color = ITEMCOLOR_WHITE;
	item.ColorStringText = TEXT("MMOGame.TeamAuction.TeamAuctionColorLimit0");
	_ItemAuctionLimitList.AddItem( item );
	item.Color = ITEMCOLOR_GREEN;
	item.ColorStringText = TEXT("MMOGame.TeamAuction.TeamAuctionColorLimit1");
	_ItemAuctionLimitList.AddItem( item );
	item.Color = ITEMCOLOR_BLUE;
	item.ColorStringText = TEXT("MMOGame.TeamAuction.TeamAuctionColorLimit2");
	_ItemAuctionLimitList.AddItem( item );
	item.Color = ITEMCOLOR_PURPLE;
	item.ColorStringText = TEXT("MMOGame.TeamAuction.TeamAuctionColorLimit3");
	_ItemAuctionLimitList.AddItem( item );
	item.Color = ITEMCOLOR_ORANGE;
	item.ColorStringText = TEXT("MMOGame.TeamAuction.TeamAuctionColorLimit4");
	_ItemAuctionLimitList.AddItem( item );

	LoadInvitePlayerBinary();

	isExpand = FALSE;
}

TeamManager::~TeamManager()
{
	SaveInvitePlayerBinary();
}

void TeamManager::CreateMyTeam( UINT32 nTeamID)
{
	hasTeam = true;
	TeamMemberList.Empty();
	//TeamLeaderServeId = TeamleaderId;
	TeamID = nTeamID;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer != NULL ){
		mainPlayer->TeamID = TeamID;
	}

	MMOGameEvent_Any evt(GameEvent_TeamStateChaged); 
	MMOUI::PostGameEvent(&evt);
}

void TeamManager::LeaveMyTeam( )
{
	hasTeam = false;
	TeamMemberList.Empty();
	TeamLeaderServeId = -1;
	TeamID = 0;
	ImTeamLeader = FALSE;
	MemberLimit = 0;
	SetTeamName( TEXT("") );
	//_myTeamName.Empty();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer != NULL ){
		mainPlayer->TeamID = TeamID;
	}

	MMOGameEvent_Any evt(GameEvent_TeamStateChaged); 
	MMOUI::PostGameEvent(&evt);

	ClearTeamAuctionItemList();

	MMOFacade::getSingleton().RefleshAllHints();
}

void TeamManager::ClearTeamInfo()
{
	TeamManager::getSingleton().LeaveMyTeam();
	TeamManager::getSingleton().GetTeamMemberList().Empty();
}
//------------------------------------------------------------
void TeamManager::AddTeamMember(struct MMOTeamMember tMember)
{
	TeamMemberList.AddItem( tMember );
}
//------------------------------------------------------------
void TeamManager::SetTeamLeaderRoleId(INT RoleId)
{
	UBOOL leaderChanged = ( TeamLeaderServeId != RoleId );
	TeamLeaderServeId = RoleId;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	
	ImTeamLeader = ( hasTeam && RoleId == pl->ServerID );
	for ( int i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		TeamMemberList(i).isLeader = TeamMemberList(i).ActorServerId == RoleId;
	}

	if(leaderChanged)
	{
		if(MMO::GetCurrentGameState() == EGameState_InGame)
		{
			if( ImTeamLeader )
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.IamTeamLeader") ) ,MsgInfoType_Table,TEXT("MMOGame.Team.IamTeamLeader"));
			}
			else
			{
				MMOTeamMember *member = GetMemberById(RoleId);
				if(member)
				{
					MMOUI::GetUIManager()->delegateAddScreenMessage( member->ActorName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.PlayerTeamLeader") ) ,MsgInfoType_Table,TEXT("MMOGame.Team.PlayerTeamLeader"));
				}
			}
		}
	}
		
	
	MMOGameEvent_Any evt(GameEvent_TeamStateChaged); 
	MMOUI::PostGameEvent(&evt);
}

void TeamManager::LoadInvitePlayerBinary()
{
	FArchive* pArchive      = NULL;
	size_t  lDataLength		= 0; 
	FString DataTypeName	= TEXT("TeamInviteHistory");

	if( MMOFacade::getSingleton().BeginLoadAccountBinary( &pArchive,DataTypeName ) )
	{
		INT TotalSize = pArchive->TotalSize();
		INT Size = TotalSize *sizeof(BYTE)/sizeof(MMOInvitedPlayer);
		InvitedPlayerCache.Add(Size);
		pArchive->Serialize( InvitedPlayerCache.GetData(), pArchive->TotalSize() );
	}
	MMOFacade::getSingleton().EndLoadAccountBinary( pArchive );
}

void TeamManager::SaveInvitePlayerBinary()
{
	void    *pData			= InvitedPlayerCache.GetData();
	size_t  lDataLength		= sizeof(MMOInvitedPlayer)*InvitedPlayerCache.Num();
	FString DataTypeName	= TEXT("-TeamInviteHistory-");

	MMOFacade::getSingleton().ExportSimpleAccountData( pData,lDataLength,DataTypeName );
}


//------------------------------------------------------------
void TeamManager::Init( )
{

	GConfig->GetInt( TEXT("TeamAuction") , TEXT("AddBidRate") , _AddBidRate , GDefaultGameIni );

	//_team = team;
	MMOUI::RegisterEventHandler( this , GameEvent_InviteTeamMsg );
	MMOUI::RegisterEventHandler( this , GameEvent_AddTeamMsg );
	MMOUI::RegisterEventHandler( this , GameEvent_PlayerBaseInfo );
	//This is placed here, though it doesn't really belong here
	//MMOUI::RegisterEventHandler( this , GameEvent_ExitMsg );
	bTargetRoleTeam = FALSE;
	bTeamAnswerHasTeam = FALSE;
}

void TeamManager::AddInvitedPlayer(INT roleId)
{
	if(roleId > 0)
	{
		InvitePlayerList.AddUniqueItem(roleId);
		if(!CheckRepeatInvitePlayer(roleId))
		{
			PlayerBaseInfo* pBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(roleId);
			if(pBaseInfo != NULL)
			{
				MMOInvitedPlayer player;
				player.ActorName = pBaseInfo->_RoleName;
				player.ActorServerId = pBaseInfo->_RoleID;
				player.Job = pBaseInfo->_RoleClass;
				player.Level = pBaseInfo->_Level;
				if(InvitedPlayerCache.Num() < MAX_ARCHIVE_INVITE_PLAYER)
				{
					InvitedPlayerCache.Push(player);
				}
				else
				{
					InvitedPlayerCache.Remove(0);
					InvitedPlayerCache.Push(player);
				}
			}
			else
			{
				WaitingPlayerInfoQueue.push(roleId);
			}
		}
	}
}

UBOOL TeamManager::CheckRepeatInvitePlayer(INT serverId)
{
	for(INT i = 0; i < InvitedPlayerCache.Num(); ++i)
	{
		if(InvitedPlayerCache(i).ActorServerId == serverId)
			return TRUE;
	}
	return FALSE;
}

void TeamManager::RemoveInvitedPlayer(INT roleId)
{
	InvitePlayerList.RemoveSingleItem(roleId);
}

void TeamManager::InvitePlayers()
{
	for(INT i = 0; i < InvitePlayerList.Num(); ++i)
	{
		check (InvitePlayerList(i) > 0);
		CMessageHandler::GetInstance().Send_EnterTeam( InvitePlayerList(i) );
	}
}

void TeamManager::ClearInvitePlayers()
{
	InvitePlayerList.Empty();
}

void TeamManager::SetTeamName(const FString& teamName )
{
	_myTeamName = teamName;
	MMOGameEvent_Any evt(GameEvent_TeamNameChange);	
	MMOUI::PostGameEvent( &evt );
}

void TeamManager::InviteTeamOk( UGFxObject* mc )
{
	if (HasTeam()==FALSE && bTeamAnswerHasTeam ==FALSE)
	{
		if(mc)
		{
			INT msgInfoId = mc->GetInt( TEXT("iteminfoid") );
			SendTeamInviteAnswer( msgInfoId , true );
		}
	}
	else
	{
		bTeamAnswerHasTeam = FALSE;
	}
		
}

void TeamManager::InviteTeamNo( UGFxObject* mc )
{
	if(mc)
	{
		INT msgInfoId = mc->GetInt( TEXT("iteminfoid") );
		SendTeamInviteAnswer( msgInfoId , FALSE );
	}
}

void TeamManager::AddTeamOk( UGFxObject* mc )
{
	if(mc)
	{
		INT msgInfoId = mc->GetInt( TEXT("iteminfoid") );
		SendTeamAddAnswer( msgInfoId , true );
	}
}

void TeamManager::AddTeamNo( UGFxObject* mc )
{
	if(mc)
	{
		INT msgInfoId = mc->GetInt( TEXT("iteminfoid") );
		SendTeamAddAnswer( msgInfoId , FALSE );
	}
}

void TeamManager::LeaveTeam( INT roleId )
{
	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
	FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
	if( pMapInfo == NULL ){
		return;
	}
	INT mapType = pMapInfo->ServerMapType;

// 	if( mapType == E_est_Static || mapType == E_est_Line || mapType == E_est_Guild || mapType == E_est_GuildDungeon 
// 		|| mapType == E_est_Dungeon || mapType == E_est_Birth )
	if( mapType == E_est_Static || mapType == E_est_Line || mapType == E_est_Guild )
	{
		CMessageHandler::GetInstance().Send_TeamLeave(roleId);

		SetExpand(FALSE);
	}
	else
	{
		//Not on a field map, cannot leave team
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.SceneCannotLeaveTeam") ) ,MsgInfoType_Table,TEXT("MMOGame.Team.SceneCannotLeaveTeam"));
	}
}

void TeamManager::MoveLeader(INT roleId)
{	
	if( HasTeam() && IsTeamLeader() )
	{		
		CMessageHandler::GetInstance().Send_OperateTeam( roleId , GetAssignType(), GetMemberLimit()  , _myTeamName , _AuctionItemColorLimit );
		
	}
}

void TeamManager::SendOperateTeam( INT AssignType,INT TeamMemberLimit,const FString& TeamName , INT auctionItemColorLimit )
{
	CMessageHandler::GetInstance().Send_OperateTeam( GetTeamLeaderServeId() , AssignType, TeamMemberLimit , TeamName , auctionItemColorLimit );
}

//------------------------------------------------------------
void TeamManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEvent_InviteTeamMsg:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if( cb->Agree )
				{
					InviteTeamOk( cb->Box );
				}
				else
				{
					InviteTeamNo( cb->Box );
				}
			}

		}
		break;
	case GameEvent_AddTeamMsg:
		{
			MMOGameEvent_MsgboxCallBack* cb = (MMOGameEvent_MsgboxCallBack*)(evt);
			if(cb)
			{
				if( cb->Agree )
				{
					AddTeamOk( cb->Box );
				}
				else
				{
					AddTeamNo( cb->Box );
				}
			}
		}
		break;
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* baseInfo = (MMOGameEvent_PlayerBaseInfo*)evt;
			if(baseInfo)
			{
				INT idx = 0;
				if( UnknownRefuseList.FindItem( baseInfo->_RoldID , idx ) )
				{
					UnknownRefuseList.RemoveItem( baseInfo->_RoldID );

					PlayerBaseInfo *refuse = baseInfo->_PlayerBaseInfo;
					if(refuse)
					{
						MMOUI::GetUIManager()->delegateAddScreenMessage( refuse->_RoleName + MMOUI::ParseLocalizedPropertyPath("MMOGame.Team.RefuseTeamAdd") ,MsgInfoType_Table,TEXT("MMOGame.Team.RefuseTeamAdd"));
					}
					
				}
				
				if(WaitingPlayerInfoQueue.size() > 0 && WaitingPlayerInfoQueue.front() == baseInfo->_RoldID)
				{
					WaitingPlayerInfoQueue.pop();
					if(!CheckRepeatInvitePlayer(baseInfo->_RoldID))
					{
						MMOInvitedPlayer player;
						player.ActorName = baseInfo->_PlayerBaseInfo->_RoleName;
						player.ActorServerId = baseInfo->_PlayerBaseInfo->_RoleID;
						player.Job = baseInfo->_PlayerBaseInfo->_RoleClass;
						player.Level = baseInfo->_PlayerBaseInfo->_Level;
						if(InvitedPlayerCache.Num() < MAX_ARCHIVE_INVITE_PLAYER)
						{
							InvitedPlayerCache.Push(player);
						}
						else
						{
							InvitedPlayerCache.Remove(0);
							InvitedPlayerCache.Push(player);
						}
					}
				}
			}
		}
	}
}

//------------------------------------------------------------
void TeamManager::RemoveTeamMember( int TeamID )
{
	for ( int i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		if( TeamMemberList(i).ActorServerId == TeamID )
		{
			TeamMemberList.Remove(i);
			break;
		}
	}
}

//------------------------------------------------------------
void TeamManager::InviteTeamRequest(const FString& RoleName,INT RoleId)
{
	//FString errName;
	//UBOOL isOK = CheckBeTeamQualification(RoleId, errName);
	//if( !isOK ){
	//	FString hintText = MMOUI::ParseLocalizedPropertyPath( errName );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( hintText, MsgInfoType_Table, errName );
	//	MMOChatManager::getSingleton().AddSystemMessage( hintText );
	//	return;
	//}

	if( _autoTeam )
	{
		SendTeamInviteAnswer( RoleId , TRUE );
	}
	else
	{
        if (MMOUI::GetUIManager()->DeclineInviteTeam)
        {
            SendTeamInviteAnswer( RoleId , FALSE );
        }
        else
        {
		    FString title = RoleName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Inviteadd") );
		
		    MMOMsgButtonManager::getSingleton().AddNewButton( title , RoleId , RoleName , GameEvent_InviteTeamMsg , E_MsgBtnShow_Team ,TEXT("MMOGame.Team.OKMsg") ,TEXT("MMOGame.Team.CancelMsg")  );
			
			if (HasTeam())
			{
				bTeamAnswerHasTeam = TRUE;
				
			}
			else
			{
				bTeamAnswerHasTeam = FALSE;
			}
			
		}
	}
}

void TeamManager::AddTeamRequest(const FString& RoleName,INT RoleId)
{
	if(_autoTeam)
	{
		SendTeamAddAnswer(RoleId,true);
	}
	else
	{
		FString title = RoleName + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.Hopeadd") );
		//FScriptDelegate OnClickOK(EC_EventParm);
		//OnClickOK.Object = this;
		//OnClickOK.FunctionName = "AddTeamOk" ;

		//FScriptDelegate OnClickCancel(EC_EventParm);
		//OnClickCancel.Object = this;
		//OnClickCancel.FunctionName = "AddTeamCancel" ;
		//FString okMsg =  MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.OKMsg") );
		//FString cancelMsg =  MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Team.CancelMsg") );

		MMOMsgButtonManager::getSingleton().AddNewButton( title , RoleId , RoleName , GameEvent_AddTeamMsg , E_MsgBtnShow_Team ,TEXT("MMOGame.Team.OKMsg"), TEXT("MMOGame.Team.CancelMsg") );

		
	}
}

void TeamManager::SendTeamInviteAnswer(INT RoleId,UBOOL agree)
{
	if( agree )
	{
		FString errName;
		UBOOL isOK = CheckBeTeamQualification(RoleId, errName);
		if( !isOK )
		{
			FString hintText = MMOUI::ParseLocalizedPropertyPath( errName );
			MMOUI::GetUIManager()->delegateAddScreenMessage( hintText, MsgInfoType_Table, errName );
			MMOChatManager::getSingleton().AddSystemMessage( hintText );
			agree = FALSE;
		}
	}

	CMessageHandler::GetInstance().Send_TeamAnswer(RoleId , agree , etatInvite );
}

void TeamManager::SendTeamAddAnswer(INT RoleId,UBOOL agree)
{
	CMessageHandler::GetInstance().Send_TeamAnswer(RoleId , agree , etatEnter );
}
//------------------------------------------------------------
INT TeamManager::GetMemberPosByMemberId(INT MemberId)
{
	for (int i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		if( TeamMemberList(i).ActorServerId == MemberId )
		{				
			return i;
			
		}
	}
	return -1;
}

//------------------------------------------------------------
INT TeamManager::GetMemberPosByUIPos( INT uiPos )
{
	for (int i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		if( TeamMemberList(i).UIPos == uiPos )
		{				
			return i;

		}
	}
	return -1;
}

//------------------------------------------------------------
MMOTeamMember *TeamManager::GetMemberById( INT roleID )
{
	for( int i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		if( TeamMemberList(i).ActorServerId == roleID )
		{				
			return &TeamMemberList(i);
		}
	}
	return NULL;
}

//------------------------------------------------------------
MMOTeamMember* TeamManager::GetTeamMember( INT index )
{
	if(index >=0 && index <= TeamMemberList.Num())
	{
		return &TeamMemberList(index);
	}
	return NULL;
}

//------------------------------------------------------------------------------------------------------------------------
MMONearbyPlayer* TeamManager::GetNearPlayer( INT index )
{
	if( NearbyPlayerList.Num() > index )
	{
		return &NearbyPlayerList(index);
	}
	return NULL;
}

MMOTeamInfo* TeamManager::GetNearTeam( INT index )
{
	if( TeamInfoList.Num() > index )
	{
		return &TeamInfoList(index);
	}
	return NULL;
}

//------------------------------------------------------------------------------------------------------------------------
void TeamManager::GetNearbyPlayerList()
{
	NearbyPlayerList.Empty();
	STLInt2IntMap* PlayerMap = MMOFacade::getSingleton().GetCharacterMap();
	STLInt2IntMap::iterator PlayerMapItor ;
	//warnf( TEXT("4"));
	AMMOPlayer* my = MMOFacade::getSingleton().GetMainPlayer();

	for ( PlayerMapItor= PlayerMap->begin();  PlayerMapItor !=PlayerMap->end(); PlayerMapItor++ )
	{
		AMMOPlayer* pl = Cast<AMMOPlayer>(UObject::GetIndexedObject( PlayerMapItor->second ));
		if( pl && pl->ActorAttributes && pl->MasterName.IsEmpty() )
		{
			if(InMyTeam(pl->ServerID) || pl->ActorName.IsEmpty() || pl->ServerID == my->ServerID/*pl->ActorAttributes->LevelAttributes.Level <= 0*/)
				continue;

			MMONearbyPlayer NearPlayer;
			NearPlayer.ActorServerId = pl->ServerID;
			NearPlayer.ClassId = pl->ActorAttributes->ClassType;
			NearPlayer.Lv = pl->ActorAttributes->LevelAttributes.Level;
			NearPlayer.Sex = pl->ActorAttributes->GenderType;
			NearPlayer.InTeam = pl->TeamID > 0; //no team <=0
			NearPlayer.ActorName = pl->ActorName;
			NearbyPlayerList.AddItem(NearPlayer);

			
		}

		

	}
}

void TeamManager::RefuseTeamAdd( INT roleId0 , INT roldId1 )
{
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	INT refuseId = 0;
	if( player->ServerID == roleId0 )
	{
		refuseId = roldId1;
	}
	else
	{
		refuseId = roleId0;
	}

	PlayerBaseInfo *refuse = MMOFacade::getSingleton().RequestPlayerBaseInfo( refuseId );
	if(refuse)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( refuse->_RoleName + MMOUI::ParseLocalizedPropertyPath("MMOGame.Team.RefuseTeamAdd") ,MsgInfoType_Table,TEXT("MMOGame.Team.RefuseTeamAdd"));
	}
	else
	{
		UnknownRefuseList.AddItem(refuseId);
	}

}

UBOOL TeamManager::InMyTeam( INT roleId )
{
	for( INT i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		if( TeamMemberList(i).ActorServerId == roleId )
		{
			return TRUE;
		}
		
	}
	return FALSE;
}

void TeamManager::ClearTeamAuctionItemList()
{
	TeamAuctionItemList.Empty();
	_CurAuctionItem = NULL;
}

void TeamManager::AddTeamAuctionItem( ItemInfo item , ETeamAuctionItemState bidState , INT index ,INT rewardMemberCount)
{
	MMOTeamAuctionItem newItem;
	newItem.Item = item;
	newItem.BidState = bidState;
	newItem.RewardMemberCount = rewardMemberCount;
	newItem.Index = index;
	TeamAuctionItemList.AddItem(newItem);

	//post game event
	MMOGameEvent_Any msg( GameEvent_TeamAuctionNewItem );
	MMOUI::PostGameEvent( &msg );
	MMOGameEvent_Any msgListChange( GameEvent_TeamAuctionListChange );
	MMOUI::PostGameEvent( &msgListChange );
}

void TeamManager::RemoveCurrentAuctionItem(INT index)
{
	/*if(!_CurAuctionItem)
	{
		return;
	}*/

	//if (_EndAuctionItemllGuid != 0)
	//{
		for ( INT idx = 0 ; idx < TeamAuctionItemList.Num() ; idx++ )
		{
			if( TeamAuctionItemList(idx).Index == index )
			{
				TeamAuctionItemList.Remove(idx);
				_EndAuctionItemllGuid = 0;
				break;
			}
		}
	//}
}

//Start auctioning an item
void TeamManager::StartTeamAuctionItem( UINT32 itemIndex ,  UINT32 bidMoney,UINT32 bidCount,UBOOL canBid )
{
	if( EGameState_Loading == MMO::GetCurrentGameState() ){
		/*	SET_WARN_COLOR(COLOR_CYAN);
		warnf( NAME_Log, TEXT("[StartTeamAuctionItem :ID %d, BUT is Loading, Returned.]"), itemIndex );
		CLEAR_WARN_COLOR();*/
		return;
	}

	//Record a current one
	for ( INT idx = 0 ; idx < TeamAuctionItemList.Num() ; idx++ )
	{
		if( TeamAuctionItemList(idx).Index == itemIndex)
		{
			if (TeamAuctionItemList(idx).BidState == ETAIS_GiveUp)
				return;

			_CurAuctionItem = &TeamAuctionItemList(idx);
			
			
			if(canBid)
			{
				_CurBidRoleId = -1;
				_CurBidMoney = bidMoney;//_CurAuctionItem->RewardMemberCount;
				_CurAuctionItem->RewardMemberCount = bidCount;

				//Show auction
				MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_TeamAuction , TRUE, "" );

				MMOGameEvent_Any msg( GameEvent_TeamAuctionNewAuction );
				MMOUI::PostGameEvent( &msg );
				MMOGameEvent_Any msgListChange( GameEvent_TeamAuctionListChange );
				MMOUI::PostGameEvent( &msgListChange );
			}
			
			break;
		}
	}

	_EndAuctionItemllGuid = itemIndex;
	
}

INT TeamManager::GetMinBidMoney()
{ 
	if( _CurBidRoleId < 0 )
	{
		return _CurBidMoney;
	}

	return _CurBidMoney + GetAddBidMoney();
}

void TeamManager::OtherBid( INT roleId , INT itemIndex , INT bidMoney )
{
	if( _CurAuctionItem && _CurAuctionItem->Index == itemIndex )
	{
		_CurBidMoney = bidMoney;
		_CurBidRoleId = roleId;
		MMOGameEvent_TeamAuctionOtherBid msg( roleId , bidMoney );
		MMOUI::PostGameEvent( &msg );
	}
}

INT TeamManager::GetCanBidAuctionItemNum()
{
	INT count = 0;
	for ( INT i = 0 ; i < TeamAuctionItemList.Num() ; i++ )
	{
		if( TeamAuctionItemList(i).BidState == ETAIS_CanBid )
		{
			count++;
		}
	}
	return count;
}

void TeamManager::EndTeamAuctionItem( INT itemIndex , INT roleId )
{
	RemoveCurrentAuctionItem(itemIndex);

	_CurBidMoney = 0;
	_CurAuctionItem = NULL;
	MMOTeamMember* member = GetMemberById( roleId );

	for ( INT i = 0 ; i < TeamAuctionItemList.Num() ; i++ )
	{
		if( TeamAuctionItemList(i).Index == itemIndex )
		{
			UItem* item = MMOFacade::getSingleton().FindItemTemplate(TeamAuctionItemList(i).Item.m_nTemplateID);
			if( member && item )
			{
				FString str = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamAuction.BidSuccess")) , *member->ActorName , *(item->GetDisplayName()) );
				MMOUI::GetUIManager()->delegateAddScreenMessage( str , MsgInfoType_Table , TEXT("MMOGame.TeamAuction.BidSuccess") );
				MMOChatManager::getSingleton().AddSystemMessage(str);
			}
			break;
		}
	}
	

	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_TeamAuction , FALSE , TEXT("") );

	//
	MMOGameEvent_Any msg( GameEvent_TeamAuctionEnd );
	MMOUI::PostGameEvent( &msg );
	MMOGameEvent_Any msgListChange( GameEvent_TeamAuctionListChange );
	MMOUI::PostGameEvent( &msgListChange );
}

void TeamManager::NotifyGiveUpBid( INT itemIndex /*, INT itemTempId */)
{
	for ( INT i = 0 ; i < TeamAuctionItemList.Num() ; i++ )
	{
		if( TeamAuctionItemList(i).Index == itemIndex /*&& TeamAuctionItemList(i).Item.m_nTemplateID == itemTempId*/ )
		{
			TeamAuctionItemList(i).BidState = ETAIS_GiveUp;
			break;
		}
	}

	if( _CurAuctionItem && _CurAuctionItem->Index == itemIndex /*&& _CurAuctionItem->Item.m_nTemplateID == itemTempId*/ )
	{
		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_TeamAuction , FALSE , TEXT("") );
	}

	MMOGameEvent_Any msg( GameEvent_TeamAuctionListChange );
	MMOUI::PostGameEvent( &msg );
}

void TeamManager::RequestGiveUp( INT itemTempId , INT itemIndex )
{
	CMessageHandler::GetInstance().Send_TeamAuctionAbandonBid( itemTempId , itemIndex );
}

void TeamManager::RequestGiveUp( INT itemIndex )
{
	for ( INT i = 0 ; i < TeamAuctionItemList.Num() ; i++ )
	{
		if( TeamAuctionItemList(i).Index == itemIndex )
		{
			RequestGiveUp( TeamAuctionItemList(i).Item.m_nTemplateID , itemIndex );
			break;
		}
	}
}

void TeamManager::RequestGiveUpCurrentItem()
{
	if(!_CurAuctionItem)
	{
		return;
	}
	CMessageHandler::GetInstance().Send_TeamAuctionAbandonBid( _CurAuctionItem->Item.m_nTemplateID , _CurAuctionItem->Index );
}

void TeamManager::RequestGiveUpAll()
{
	for ( INT i = 0 ; i < TeamAuctionItemList.Num() ; i++ )
	{
		if( TeamAuctionItemList(i).BidState == ETAIS_CanBid )
		{
			RequestGiveUp( TeamAuctionItemList(i).Item.m_nTemplateID , TeamAuctionItemList(i).Index );
			//break;
		}
	}
}

INT TeamManager::GetTeamAuctionItemColorIndex( INT colorLimit )
{
	for( INT i = 0 ; i < _ItemAuctionLimitList.Num() ; i++ )
	{
		if( _ItemAuctionLimitList(i).Color == colorLimit )
		{
			return i;
		}
	}
	return 0;
}

INT TeamManager::GetCurTeamAuctionItemColorIndex()
{
	for( INT i = 0 ; i < _ItemAuctionLimitList.Num() ; i++ )
	{
		if( _ItemAuctionLimitList(i).Color == _AuctionItemColorLimit )
		{
			return i;
		}
	}
	return 0;
}

INT TeamManager::GetTeamAuctionItemColorByIdx( INT idx )
{
	if( idx < 0 || _ItemAuctionLimitList.Num() <= idx )
	{
		return 0;
	}
	return _ItemAuctionLimitList(idx).Color;
}

void TeamManager::NotifyQuickTeam(BOOL join)
{
	// UMMOUITargetInfo* target =  Cast<UMMOUITargetInfo>(MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_HudInfo_TargetInfo));
	 //if (target)
	 //{
		 //AMMOActor* actor = FSM::Target.CurTargetMMOActor;//target->CurrentMMOActor;
		 AMMOActor* actor = FSMIO::getSingleton().Target.GetTargetMMOActor();

		 if ( actor)
		 {
			 //Form team
				 if (join)
				 {
					 /*if (TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader())
					 {
					 CMessageHandler::GetInstance().Send_EnterTeam(actor->ServerID);
					 }
					 else
					 {
					 targetRoleId = actor->ServerID;
					 CMessageHandler::GetInstance().Send_QueryTeam( actor->ServerID );
					 }*/
					 QueryRoleHasTeam(actor->ServerID);

				 }
				 //Kick player
				 else
				 {
					 if( TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader() )
					 {
						 if ( actor->ServerID != MMOFacade::getSingleton().GetMainPlayer()->ServerID)
						 {
							 if(TeamManager::getSingleton().GetMemberById(actor->ServerID))//if in the team
							 {
								 //TeamManager::getSingleton().LeaveTeam( actor->ServerID );
								 MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam,actor->ServerID);
								 MMOUI::PostGameEvent(&et);
							 }
							 else
							 {
								 FString strScreen= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.Error.NotInTeam"));
								 MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Table,TEXT("MMOGame.Team.Error.NotInTeam"));
							 }
						 }
						 /*else
						 {
							 FString strScreen= MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Team.Error.NotLeaveMine"));
							 MMOUI::GetUIManager()->delegateAddScreenMessage(strScreen,MsgInfoType_Table,TEXT("MMOGame.Team.Error.NotLeaveMine"));
						 }*/
					 }
				 }
		}

		 else
		 {
			 // Designer requires that one can also leave the team without a target
			 if(!join&&TeamManager::getSingleton().HasTeam())
			 {
				 MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam,MMOFacade::getSingleton().GetMainPlayer()->ServerID);
				 MMOUI::PostGameEvent(&et);
				 //TeamManager::getSingleton().LeaveTeam( MMOFacade::getSingleton().GetMainPlayer()->ServerID);
			 }
		 }
			
	// } 
	 
}

void TeamManager::TeamExpand()
{
	if( HasTeam() && IsTeamLeader() )
	{
		//SendOperateTeam( GetAssignType() , LargeTeamNum , GetTeamName() , GetAuctionItemColorLimit() );
		isExpand = TRUE;
	}
}

void TeamManager::SelectedNearTeamMate(INT nIndex,BOOL teamMate)
{
	AMMOPlayer* my = MMOFacade::getSingleton().GetMainPlayer();

	INT teammateIdx = nIndex;

	if (teamMate)
		teammateIdx = TeamManager::getSingleton().GetMemberPosByUIPos( nIndex );
	

	MMOTeamMember* teammate = TeamManager::getSingleton().GetTeamMember( teammateIdx );
	if( teammate == NULL || teammate->Online == FALSE)
		return;
	if (teammate->ActorServerId == my->ServerID)
		return;

	STLInt2IntMap* PlayerMap = MMOFacade::getSingleton().GetCharacterMap();
	STLInt2IntMap::iterator PlayerMapItor ;

	for ( PlayerMapItor= PlayerMap->begin();  PlayerMapItor !=PlayerMap->end(); PlayerMapItor++ )
	{
		AMMOPlayer* pl = Cast<AMMOPlayer>(UObject::GetIndexedObject( PlayerMapItor->second ));
		if( pl && pl->ServerID == teammate->ActorServerId)
		{
			FSMIO::getSingleton().Target.SetDesireActor(pl);
			break;
		}

	}

}

INT TeamManager::GetTeamAuctionItemIdx(INT idx)
{
	INT index = 0;

	for ( INT i = 0 ; i < TeamAuctionItemList.Num() ; i++ )
	{
		if (TeamAuctionItemList(i).Index == idx)
		{
			index = i + 1;
			break;
		}
	}
	return index;
}


MMOTeamMember* TeamManager::GetTeamLeader()
{
	for( int i = 0 ; i < TeamMemberList.Num() ; i++ )
	{
		if( TeamMemberList(i).isLeader )
		{				
			return &TeamMemberList(i);
		}
	}
	return NULL;

}

UGFxObject* TeamManager::CreateTeamMate(MMOTeamMember* member,UMMOGFXPlayer* gfxPlayer)
{
	UGFxObject* data = gfxPlayer->CreateObject(TEXT("ActionScript3.Team.MyTeamMateData"));
	if (!data)
		return NULL;

	member->HPPecent = Clamp<FLOAT>( member->HPPecent , 0.01 , 1 );

	data->SetString(TEXT("Name"),member->ActorName);
	data->SetBool(TEXT("OnLine"),member->Online);
	data->SetBool(TEXT("IsLeader"),member->isLeader);
	data->SetInt(TEXT("HP"),member->HP);
	data->SetFloat(TEXT("HPPecent"),member->HPPecent);
	data->SetInt(TEXT("ClassId"),member->ClassId);
	data->SetInt(TEXT("Sex"),member->Sex);
	return data;

}

void TeamManager::NotifyCreateTeam()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamSet, TRUE, "");
	bTargetRoleTeam = TRUE;
}
void TeamManager::NotifyEnterTeam()
{
	CMessageHandler::GetInstance().Send_EnterTeam(targetRoleId);
	ResetTeamTarget();
}

void TeamManager::ResetTeamTarget()
{
	targetRoleId = 0;
	bTargetRoleTeam = FALSE;
}

void TeamManager::QueryRoleHasTeam(INT roleId)
{
	//FString errName;
	//UBOOL isOK = CheckBeTeamQualification(roleId, errName);
	//if( !isOK ){
	//	FString hintText = MMOUI::ParseLocalizedPropertyPath( errName );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( hintText, MsgInfoType_Table, errName );
	//	MMOChatManager::getSingleton().AddSystemMessage( hintText );
	//	return;
	//}

	if (TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader())
	{
		CMessageHandler::GetInstance().Send_EnterTeam(roleId);
	}
	else
	{
		targetRoleId = roleId;
		CMessageHandler::GetInstance().Send_QueryTeam( roleId );
	}
}

UBOOL TeamManager::CheckBeTeamQualification(INT roleId, FString &errMsgName)
{
	errMsgName.Empty();

	INT curArenaState = MMOArenaManager::getSingleton().GetCurVSState();
	if( curArenaState == ARENA_STAT_READY || curArenaState == ARENA_STAT_WAIT || curArenaState == ARENA_STAT_FIGHT )
	{
		errMsgName = TEXT("MMOGame.Arena.NoTeam");
		return FALSE;
	}

	return TRUE;
}