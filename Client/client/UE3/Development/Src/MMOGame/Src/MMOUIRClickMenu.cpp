#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"

#include "BattleTeamManager.h"
#include "FriendManager.h"
#include "MMOGuildManager.h"
#include "FSMIO.h"
#include "TeamManager.h"
#include "MasterStudentManager.h"
#include "CommonGameEvents.h"
#include "MMOMsgBoxManager.h"
#include "MailManager.h"
#include "MMOPlayerInfoManager.h"
#include "UITextParser.h"
#include "MMOMapDataManager.h"
#include "TeamManager.h"
#include "MMOChatManager.h"
#include "client_option_def.h"

IMPLEMENT_CLASS(UMMOUIRClickMenu)


AMMOActor* UMMOUIRClickMenu::GetActorByRoleId(INT RoleId)
{
	return MMOFacade::getSingleton().GetCharacter(RoleId);
}

void UMMOUIRClickMenu::CloseRClickMenu( BYTE type)
{
	if(curType==type)
	{
		ClearUI();
	}
}

void UMMOUIRClickMenu::OpenRClickMenu(BYTE type, INT x, INT y, INT roleId, const FString& roleName, INT index)
{
	//M_FORCE_TRY_BEGIN;

	//ChangeVisible();


	FuncList.Remove(0, FuncList.Num());

	for (INT i = 0; i < ButtonMaxNum; i ++)
	{
		UGFxObject* mc = GetObject(FString::Printf(TEXT("RClicklistItem%d"), i));
		if (mc)
		{
			mc->SetVisible(false);
		}
	}

	for (INT i = 0; i < 5; i ++)
	{
		UGFxObject* mc = GetObject(FString::Printf(TEXT("splitLine%d"), i));
		if (mc)
		{
			mc->SetVisible(false);
		}
	}

	UBOOL bRes = ChangeMenu(roleId, roleName, index, type);

	if (MenuOwnerTF)
	{
		MenuOwnerTF->SetText(OwnerName);
	}

	if (RCMenuBg)
	{
		RCMenuBg->SetInt(TEXT("height"), high + 10);
	}

	UGFxObject* MenuMask = GetObject( TEXT("Rclick_ScreenMask") );
	if( MenuMask )
	{
		MenuMask->SetVisible(true);
		MenuMask->SetInt( TEXT("x") , -x );
		MenuMask->SetInt( TEXT("y") , -y );

		FVector2D screenSize = MMOFacade::GetScreenSize();

		MenuMask->SetInt( TEXT("width") , screenSize.X );
		MenuMask->SetInt( TEXT("height") , screenSize.Y );
		MenuMask->SetVisible(true);
	}

	INT width = GetInt(TEXT("width"));
	ModifyPos(x, y, high, width);

	Show(bRes);


	//M_FORCE_TRY_END("Protected for UMMOUIRClickMenu::OpenRClickMenu");
}

//------------------------------------------------------------
void UMMOUIRClickMenu::ModifyPos( INT x , INT y , INT height , INT width )
{
	INT screenX = MMOFacade::getSingleton().GetScreenSize().X;
	INT screenY = MMOFacade::getSingleton().GetScreenSize().Y;
	INT offset = 10;
	if( x + 2*width > screenX )
	{
		x = screenX - 2*width - offset;
	}
	if( y + height > screenY )
	{
		y = screenY - height - offset;
	}

	//`log("y:"@y);
	SetInt( TEXT("x") , x );
	SetInt( TEXT("y") , y );
}

//-------------------------------------------------------------
void UMMOUIRClickMenu::AddMenuItem(FScriptDelegate func,const FString& StrName,UBOOL UseStringMap)
{
	FuncList.AddItem( func );
	FString str = UseStringMap?(MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.GamePlay.")) + StrName )):StrName;
	ShowClickItem( str );
}


//-----------------------
void UMMOUIRClickMenu::ShowClickItem(const FString& label)
{
	UGFxObject* ButtonMc;

	ButtonMc = GetObject( FString::Printf( TEXT("RClicklistItem%d") , step ) );
	if( !ButtonMc)
	{
		return;
	}
	ButtonMc->SetVisible(TRUE);
	ButtonMc->SetString( TEXT("label") , label );
	ButtonMc->SetInt( TEXT("y") , high );
	high = ButtonMc->GetInt( TEXT("height") ) + high;
	step++;
}

void UMMOUIRClickMenu::AddLine()
{
	UGFxObject* lineMc = GetObject( FString::Printf( TEXT("splitLine%d") , lineStep ) );
	if( !lineMc)
	{
		return;
	}
	lineMc->SetVisible(TRUE);
	lineMc->SetInt( TEXT("y") , high );
	high = lineMc->GetInt( TEXT("height") ) + high;
	lineStep++;
}

//------------------------------------------------------------------------------------------

void UMMOUIRClickMenu::ClearUI()
{
	Show(FALSE);
}

void UMMOUIRClickMenu::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt->GetEventType() == GameEvent_TargetChange)
	{
		MMOGameEvent_TargetEvent* targetEvt = (MMOGameEvent_TargetEvent*)evt;
		if(targetEvt == NULL)
		{
			return;
		}

		switch(targetEvt->EventType)
		{
			case MMOGameEvent_TargetEvent::ETCT_DesiredTarget:
			Show(FALSE);
			break;
		}
	}
}

//guild category
bool UMMOUIRClickMenu::AddMenuItem_Guild(FScriptDelegate &func)
{
	bool bAddLine = false;
	class AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return bAddLine;
	}

	if (MMOGuildData::getSingleton().GetGuildBaseInfo().Id > 0)
	{// I have a guild
		UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainPlayer->ServerID);
		INT ownerPost = MMOGuildData::getSingleton().GetGuildFullMemberPost(OwnerRoleId);
		UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);

		if (ownerPost != GUILD_POSITION_NONE)
		{// The other party is in the same guild as me
			if (post < ownerPost && (rights & G_POS_RIGHT_BANISH) > 0)
			{
				// Kick from guild
				func.FunctionName = "ClickKickOutGuild";
				AddMenuItem(func, TEXT("KickOutGuild"));
				bAddLine = true;
			}
			
			if (post <= ownerPost && (rights & G_POS_RIGHT_FORBID_CHAT) > 0)
			{
				// Guild mute
				if (MMOGuildData::getSingleton().IsGuildFullMemberForbitChat(OwnerRoleId))
				{
					func.FunctionName = "ClickForbidChatOff";
					AddMenuItem(func, TEXT("ForbidChatOff"));
					bAddLine = true;
				}
				else
				{
					func.FunctionName = "ClickForbidChatOn";
					AddMenuItem(func, TEXT("ForbidChatOn"));
					bAddLine = true;
				}
			}
		}
		else
		{// The other party has no guild or is in a different guild from me
			// If the target is not on the same map as me OwnerActor is empty, so use OwnerRoleId here
			PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(OwnerRoleId, TRUE);
			if (playerInfo)
			{		
				if (playerInfo->_GuildName.IsEmpty())
				{// The other party has no guild
					if ((rights & G_POS_RIGHT_INVITE) > 0)
					{
						// Invite to join guild
						func.FunctionName = "ClickInviteToJoinGuild";
						AddMenuItem(func, TEXT("InviteToJoinGuild"));
						bAddLine = true;
					}
					else if((rights & G_POS_RIGHT_RECRUIT) > 0)
					{
						// Recommend joining guild
						func.FunctionName = "ClickRecommendJoinGuild";
						AddMenuItem(func, TEXT("RecommendJoinGuild"));
						bAddLine = true;
					}
				}
				else
				{// The other party is in a different guild from me
					// copy guild name
					func.FunctionName = "ClickCopyGuildName";
					AddMenuItem(func, TEXT("CopyGuildName"));
					bAddLine = true;
				}
			}
		}
	}
	else
	{// I have no guild
		if(OwnerActor && !OwnerActor->GuildName.IsEmpty())
		{// The other party has a guild
			// Apply to join guild
			func.FunctionName = "ClickApplyToJoinGuild";
			AddMenuItem(func, TEXT("ApplyToJoinGuild"));

			// copy guild name
			func.FunctionName = "ClickCopyGuildName";
			AddMenuItem(func, TEXT("CopyGuildName"));

			bAddLine = true;
		}
	}

	return bAddLine;
}

//Guild right-click menu
void UMMOUIRClickMenu::_GuildMenuFormat()
{
	class AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (!Player)
	{
		return;
	}

	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	// Offline member, do not view info
	GuildFullMemberInfo memberInfo;
	if (MMOGuildData::getSingleton().GetGuildFullMemberInfo(OwnerRoleId, memberInfo))
	{
		if (memberInfo.LastLoginTime <= 0)
		{
			func.FunctionName = "ClickViewProfile";
			AddMenuItem(func, TEXT("ViewProfile"));
			AddLine();
		}
	}

	UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainPlayer->ServerID);
	INT ownerPost = MMOGuildData::getSingleton().GetGuildFullMemberPost(OwnerRoleId);
	UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);

	if (ownerPost != GUILD_POSITION_NONE)
	{// The other party is in the same guild as me
		if (post < ownerPost && (rights & G_POS_RIGHT_BANISH) > 0)
		{
			// Kick from guild
			func.FunctionName = "ClickKickOutGuild";
			AddMenuItem(func, TEXT("KickOutGuild"));
			AddLine();
		}

		if (post <= ownerPost && (rights & G_POS_RIGHT_FORBID_CHAT) > 0)
		{
			// Guild mute
			if (MMOGuildData::getSingleton().IsGuildFullMemberForbitChat(OwnerRoleId))
			{
				func.FunctionName = "ClickForbidChatOff";
				AddMenuItem(func, TEXT("ForbidChatOff"));
				AddLine();
			}
			else
			{
				func.FunctionName = "ClickForbidChatOn";
				AddMenuItem(func, TEXT("ForbidChatOn"));
				AddLine();
			}
		}
	}
}

//Common right-click menu
void UMMOUIRClickMenu::_MenuFormat(INT nType)
{
	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	if( !Player )
	{
		return;
	}

	/*
	Is the player themselves  only show
	1 View info
	2 Party operations
	*/
	//Self
	if( OwnerRoleId == Player->ServerID  )
	{
		if( TeamManager::getSingleton().HasTeam() )
		{
			func.FunctionName = "ClickLeaveTeam";
			AddMenuItem( func , TEXT("LeaveTeam") );

			/*if( TeamManager::getSingleton().IsTeamLeader() )
			{
			func.FunctionName = "ClickChangeTeamAuctionLimit";
			AddMenuItem( func , TEXT("ChangeTeamAuctionLimit") );
			}*/
		}
		//view own profile
		func.FunctionName = "ClickViewProfile";
		AddMenuItem( func , TEXT("ViewProfile") );

		return;
	}

	/*
	Order for others:
	Chat
	Party
	General
	Clan
	Master/apprentice
	General settings
	Copy name
	*/
	bool bAddLine = true;
	//request trade
	if( nType == ERCMType_TargetInfo )
	{
		func.FunctionName = "ClickApplyDoTrade";
		AddMenuItem( func, TEXT("ApplyDoTrade") );
	}
	//1 Chat
	{
		if( FriendManager::getSingleton().IsMyFriend( OwnerRoleId )  )
		{
			func.FunctionName = "ClickOpenContactDialog";
			AddMenuItem( func , TEXT("EnterChat") );
		}	
		//private chat
		func.FunctionName = "ClickEnterPrivateChat"; 
		AddMenuItem( func , TEXT("PrivateChat") );	
		//mail
		func.FunctionName = "ClickSendMail";
		AddMenuItem( func , TEXT("SendMail") );
		
		AddLine();
	}

	//2 Party
	{
		bAddLine = false;
		if( !TeamManager::getSingleton().HasTeam() ) 
		{
			//apply to join team
			func.FunctionName = "ClickInvitePlayerIntoTeam";
			AddMenuItem(  func , TEXT("joinTeam") );
			bAddLine = true;
		}
		else if( TeamManager::getSingleton().HasTeam() && !TeamManager::getSingleton().InMyTeam(OwnerRoleId) )
		{
			//I have a party, the other is not in my party, invite to party
			func.FunctionName = "ClickInvitePlayerIntoTeam";
			AddMenuItem( func , TEXT("InviteTeam") );
			bAddLine = true;
		}

		if( TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader() )
		{
			if(TeamManager::getSingleton().GetMemberById(OwnerRoleId))//if in the party
			{
				//kick from team
				func.FunctionName = "ClickLeaveTeam";
				AddMenuItem( func , TEXT("KickOutTeam") );
				//transfer leader
				func.FunctionName = "ClickMoveLeader";
				AddMenuItem( func , TEXT("MoveLeader") );
				bAddLine = true;
			}
		}
		if(bAddLine)
			AddLine();
	}

	//3 General
	{
		if( nType == ERCMType_TargetInfo )
		{
			func.FunctionName = "ClickViewProfile";
			AddMenuItem( func , TEXT("ViewProfile") );

			//click follow/unfollow
			if(1)
			{
				func.FunctionName = "ClickFollow";
				AddMenuItem( func , TEXT("Follow") );
			}
			else
			{
				func.FunctionName = "ClickNullFollow";
				AddMenuItem( func , TEXT("NullFollow") );
			}
		}
		else
		{
			func.FunctionName = "ClickSimpleProfile";
			AddMenuItem( func , TEXT("ViewProfile") );
		}
		if( nType == ERCMType_TargetInfo || nType == ERCMType_Friend || nType == ERCMType_ChatPlayer 
			|| nType == ERCMType_RankListPlayer || nType == ERCMType_Attention || nType == ERCMType_TeamMember || nType == ERCMType_Contact )
		{
			if( FriendManager::getSingleton().IsMyFriend(OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(OwnerRoleId) )
			{   //unfollow
				func.FunctionName = "ClickDeleteFriend";
				AddMenuItem( func , TEXT("DelAttention") );	
			}
			else
			{
				//add to follow
				func.FunctionName = "ClickAddFriend";
				AddMenuItem( func , TEXT("AddAttention") );	
			}

			if(FriendManager::getSingleton().InMyEnemyList(OwnerRoleId))
			{
				func.FunctionName = "ClickRemoveEnemy";
				AddMenuItem( func , TEXT("RemoveEnemy") );	
			}
			else
			{
				func.FunctionName = "ClickAddEnemy";
				AddMenuItem( func , TEXT("AddEnemy") );	
			}

			if( nType == ERCMType_Friend && FriendManager::getSingleton().IsMyFriend(OwnerRoleId) )
			{
				func.FunctionName = "ClickMoveFriend";
				AddMenuItem( func , TEXT("MoveFriend") );
			}
		}
		////Request trade
		//if( nType == ERCMType_TargetInfo )
		//{
		//	func.FunctionName = "ClickApplyDoTrade";
		//	AddMenuItem( func, TEXT("ApplyDoTrade") );
		//}
		// Achievement comparison
		func.FunctionName = "ClickAchievementCompare";
		AddMenuItem( func, TEXT("CompareAchievement") );
		AddLine();
	}

	//4 Guild
	{
		if( nType == ERCMType_TargetInfo || 
			nType == ERCMType_ChatPlayer ||
			nType == ERCMType_Friend ||
			nType == ERCMType_Contact ||
			nType == ERCMType_Attention ||
			nType == ERCMType_FriendGroup ||
			nType == ERCMType_RankListPlayer ||
			nType == ERCMType_GuildMember ||
			nType == ERCMType_TeamMember ||
			nType == ERCMType_TeamMate ||
			nType == ERCMType_TeamNearby ||
			nType == ERCMType_Dungeon ||
			nType == ERCMType_BattleEnter ||
			nType == ERCMType_BattleMember)
		{
			if (AddMenuItem_Guild(func))
			{
				AddLine();
			}
		}
	}

	//5 Master/apprentice
	{
		bAddLine = false;
		if (E_MASTER_NONE == MasterStudentManager::getSingleton().GetMasterStudentState())
		{
			int PlayerLv = Player->ActorAttributes->LevelAttributes.Level;
			if(GRADUATE_LEVEL > PlayerLv&& PlayerLv >= STUD_LEVEL_LOWER_LIMIT)
			{
				func.FunctionName = "ClickApplyMaster";
				AddMenuItem(func, TEXT("ApplyMaster"));
				bAddLine = true;
			}
		}
		else if (E_MASTER_STUDENT == MasterStudentManager::getSingleton().GetMasterStudentState())
		{
			const MasterDetailInfo&	pMasterInfo = MasterStudentManager::getSingleton().GetMasterDetailInfo();
			if(pMasterInfo.nRoleID > 0)
			{
				if(appStrcmp(*(pMasterInfo.masterName),*OwnerName) == 0)
				{
					func.FunctionName = "ClickBetryMaster";
					AddMenuItem(func, TEXT("BetryMaster"));
					bAddLine = true;
				}
			}
		}
		else if (E_MASTER_TEACHER == MasterStudentManager::getSingleton().GetMasterStudentState())
		{

			const StudentListData* pStudent = MasterStudentManager::getSingleton().GetStudentDataByID(OwnerRoleId);
			if (NULL != pStudent)
			{
				func.FunctionName = "ClickExpell";
				AddMenuItem(func, TEXT("Expell"));
				bAddLine = true;
			}
			else
			{
				func.FunctionName = "ClickApplyStudent";
				AddMenuItem(func, TEXT("Readmastermsg"));
				bAddLine = true;
			}
		}
		if (bAddLine)
			AddLine();
	}

	//6 General settings
	{
		bAddLine = false;
		if( nType == ERCMType_Friend || ERCMType_ChatPlayer )
		{
			if( FriendManager::getSingleton().InMyBlackList(OwnerRoleId) )
			{
				//unblock
				func.FunctionName = "ClickRemoveFromBlackList";
				AddMenuItem( func , TEXT("RemoveBlack") );
				bAddLine = true;
			}
			else
			{
				//block chat
				func.FunctionName = "ClickMoveToBlackList";
				AddMenuItem( func , TEXT("MoveToBlack") );
				bAddLine = true;
			}
		}
		//Report player
		//func.FunctionName = "ClickCommon3";
		//AddMenuItem( func , TEXT("Common3") );
		//Track position
		//func.FunctionName = "ClickCommon4";
		//AddMenuItem( func , TEXT("Common4") );
		if(bAddLine)
			AddLine();
	}

	func.FunctionName = "ClickCopyName";
	AddMenuItem( func , TEXT("CopyName") );
	AddLine();
}

//------------------------------------------------------------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByTargetInfo(INT RoleId, const FString& RoleName)
{
	OwnerName = RoleName;
	OwnerActor = GetActorByRoleId(RoleId);
	OwnerRoleId = RoleId;
	if( !OwnerActor || !Player )
	{
		return FALSE;
	}

	AMMOPlayer* OwnerPlayer = Cast<AMMOPlayer>( OwnerActor );
	if( !OwnerPlayer ) 
	{
		return FALSE;//not a player
	}

	if( OwnerPlayer == Player )
	{
		return TRUE;
	}

	if(OwnerPlayer->Owner)
	{
		return FALSE;//is a hero
	}

	_MenuFormat(ERCMType_TargetInfo);//common right-click menu 2
	return TRUE;
}

//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByFriendGroup(INT Index,const FString& GroupName)
{
	if( Index >= FriendManager::getSingleton().GetFriendGroupNum() )
	{
		//SetVisible(false);
		return FALSE;
	}

	if( Index >= AttentionGroupId )
	{
		//SetVisible(false);
		return FALSE;
	}
	
	OwnerName = GroupName;
	OwnerGroupId = Index;

	
	FScriptDelegate func(EC_EventParm);
	func.Object = this;
	func.FunctionName = "ClickNewGroup";
	AddMenuItem( func , TEXT("NewGroup") );	

	if( FriendManager::getSingleton().GetFriendGroup(Index) && !FriendManager::getSingleton().GetFriendGroup(Index)->IsDefault )
	{
		func.FunctionName = "ClickChangeGroupName";
		AddMenuItem( func , TEXT("ChangeGroupName") );

		func.FunctionName = "ClickDelGroup";
		AddMenuItem( func , TEXT("DelGroup") );
	}

	return TRUE;
}

//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByBattleMember( INT roleId )
{
	//const TArray<BattleTeamMember>& memberList = BattleTeamManager::getSingleton().GetMemberList();
	//BattleTeamMember* member = BattleTeamManager::getSingleton().GetMemberByRoleId(roleId);
	//if( member )
	//{
	//	if( !BattleTeamManager::getSingleton().HasTeam() )
	//	{
	//		return FALSE;
	//	}

	//	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

	//	OwnerName = member->PlayerName;
	//	OwnerRoleId = member->ServerID;

	//	UBOOL isSelf = ( member->ServerID == player->ServerID );
	//	UBOOL imLeader = BattleTeamManager::getSingleton().IsTeamLeader();

	//	
	//	FScriptDelegate func(EC_EventParm);
	//	func.Object = this;

	//	if( !isSelf )
	//	{
	//		func.FunctionName = "ClickEnterPrivateChat";
	//		AddMenuItem(  func , TEXT("PrivateChat") );	

	//		func.FunctionName = "ClickInvitePlayerIntoTeam";
	//		AddMenuItem( func , TEXT("InviteTeam") );
	//	}

	//	//Requirements cannot all be met, disband and change leader have no protocol support
	//	if( isSelf )
	//	{
	//		func.FunctionName = "ClickRemoveBattleTeam";
	//		AddMenuItem( func , TEXT("LeaveBattleTeam") );	
	//	}

	//	if( imLeader && !isSelf )
	//	{
	//		//Kick from team
	//		func.FunctionName = "ClickRemoveBattleTeam";
	//		AddMenuItem( func , TEXT("RemoveBattleMember") );	
	//	}

	//}
	//else
	//{
	//	return FALSE;
	//}

	return TRUE;
}

//
UBOOL UMMOUIRClickMenu::_MenuFormatByTeamMate(INT index)
{
	INT memberPos = TeamManager::getSingleton().GetMemberPosByUIPos(index);
	if( memberPos < 0 )
	{
		return FALSE;
	}

	_MenuFormatByMyTeam(memberPos);
	return TRUE;
}

//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByMyTeam(INT index)
{
	if( index >= TeamManager::getSingleton().GetTeamMemberCount() )
	{
		
		return FALSE;
	}

	MMOTeamMember* tMem = TeamManager::getSingleton().GetTeamMember(index);
	if(!tMem)
	{
		return FALSE;
	}
	OwnerName = tMem->ActorName;
	OwnerRoleId = tMem->ActorServerId;
	OwnerActor = GetActorByRoleId(OwnerRoleId);

	_MenuFormat(ERCMType_TeamMember);//common right-click menu 2
	return TRUE;
}
//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByAttention(INT Index,const FString& RoleName)
{
	//OwnerNearPlayer = player.TeamManager.NearbyPlayerList[index];
	OwnerName = RoleName;
	OwnerIndex = Index;
	OwnerGroupId = AttentionGroupId;
	MMOFriendMember* fMember = FriendManager::getSingleton().GetFriendGroupMember( AttentionGroupId , Index );
	if( !fMember )
	{
		return FALSE;
	}

	OwnerRoleId = fMember->RoleId;

	_MenuFormat(ERCMType_Attention);
	return TRUE;
}

//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByRankListPlayer( INT roleId , const FString& roleName )
{
	OwnerName = roleName;
	OwnerRoleId = roleId;
	OwnerActor = GetActorByRoleId(OwnerRoleId);

	FScriptDelegate func(EC_EventParm);
	func.Object = this;

	//Start private chat
	//func.FunctionName = 
	//View character info
	//func.FunctionName = "ClickViewProfileOnRank";
	//AddMenuItem( func , TEXT("Equip"));

	//View physique enhancement
	//func.FunctionName = "ClickOpenBuild";
	//AddMenuItem( func , TEXT("Build"));

	//View skills
	//func.FunctionName = "ClickOpenSkill";
	//AddMenuItem( func , TEXT("Skill"));

	//View realm
	//func.FunctionName = "ClickOpenJingjie";
	//AddMenuItem( func , TEXT("Jingjie"));

	_MenuFormat(ERCMType_RankListPlayer);//common right-click menu 2
	return TRUE;
}

void UMMOUIRClickMenu::ClickViewProfileOnRank( INT inMenuIdx )
{
	MMOPlayerInfoManager::getSingleton().setIsShowInfoByRankList(true);
	UMMOUIPlayerInfo* playerInfo = Cast<UMMOUIPlayerInfo>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo ) );
	if(playerInfo)
	{
		playerInfo->InitPlayerRoleId( OwnerRoleId );
	}
	ChangeVisible();
}

//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByRankListGuild( INT guildId , const FString& guildName )
{
	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	OwnerName = guildName;
	GuildId = guildId;
	//FScriptDelegate func(EC_EventParm);
	//func.Object = this;
	//func.FunctionName = "ClickOpenGuild";
	//AddMenuItem( func , TEXT("GuildInfo"));

	// modified by zhangrui05
	//MenuFormat(ERCMType_RankListGuild);//common right-click menu 2
	func.FunctionName = "ClickCopyName";
	AddMenuItem( func , TEXT("CopyName") );

	return TRUE;
}
void UMMOUIRClickMenu::ClickOpenGuild( INT inMenuIdx )
{
	//MMOUI::GetUIManager()->delegateShowGuildViewPanel();
	MMOGameEvent_ShowGuildView evt;
	MMOUI::PostGameEvent(&evt);
	CMessageHandler::GetInstance().Send_FindGuild(GuildId, TEXT("")); 
	Show(FALSE);
}

//-----------------------------------------
UBOOL UMMOUIRClickMenu::_MenuFormatByFriend( INT roleId , const FString& roleName )
{
	
	OwnerName = roleName;
	OwnerRoleId = roleId;
	OwnerActor = GetActorByRoleId(OwnerRoleId);

	_MenuFormat(ERCMType_Friend);
	return TRUE;
	
}

UBOOL UMMOUIRClickMenu::_MenuFormatByBlack(int Index )
{
	//This is actually no longer needed
	if( FriendManager::getSingleton().GetBlackNum() <= Index)
	{
		return FALSE;
	}
	MMOFriendMember* Member = FriendManager::getSingleton().GetBlacklistMember(Index);
	if(Member==NULL)
	{
		return FALSE;
	}
	OwnerName = Member->ActorName;
	OwnerIndex = Index;	
	OwnerRoleId = Member->RoleId;

	FScriptDelegate func(EC_EventParm);
	func.Object = this;
	func.FunctionName = "ClickRemoveFromBlackList";
	AddMenuItem( func , TEXT("RemoveBlack") );
	return TRUE;
}

UBOOL UMMOUIRClickMenu::_MenuFormatByChatPlayer( INT roleId , const FString& playerName )
{
	FScriptDelegate func( EC_EventParm);
	func.Object = this;
	
	OwnerName = playerName;
	OwnerRoleId = roleId;

	_MenuFormat(ERCMType_ChatPlayer);
	return TRUE;

}

UBOOL UMMOUIRClickMenu::_MenuFormatByMasterPlayer( INT roleId , const FString& playerName )
{
	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	OwnerName = playerName;
	OwnerRoleId = roleId;

	_MenuFormat(ERCMType_MasterStudent);//common right-click menu 2
	return TRUE;

}

UBOOL UMMOUIRClickMenu::_MenuFormatByGuildMember(INT RoleId)
{
	if (!Player)
	{
		return FALSE;
	}

	GuildFullMemberInfo info;
	if (!MMOGuildData::getSingleton().GetGuildFullMemberInfo(RoleId, info))
	{
		return FALSE;
	}
		
	OwnerRoleId = RoleId;
	OwnerActor = GetActorByRoleId(RoleId); // OwnerActor may be NULL, members are allowed to be offline here
	OwnerName = info.Name;

	// Quest #21837
	if (info.LastLoginTime > 0)
	{// Offline, only show view info and mute
		_GuildMenuFormat();
	}
	else
	{// Online, show the common right-click menu
		_MenuFormat(ERCMType_TargetInfo);
	}
	return TRUE;
}

UBOOL UMMOUIRClickMenu::_MenuFormatByGuildAlliance(INT guildId, const FString& guildName)
{
	FScriptDelegate func(EC_EventParm);
	func.Object = this;

	GuildId = guildId;
	OwnerName = guildName;
	
	func.FunctionName = "ClickCancelAlliance";
	AddMenuItem(func , TEXT("CancelAlliance"));
	
	// copy guild name
	func.FunctionName = "ClickCopyGuildNameByPanel";
	AddMenuItem(func, TEXT("CopyGuildName"));

	AddLine();

	return TRUE;
}

UBOOL UMMOUIRClickMenu::_MenuFormatByGuildEnemy(INT guildId, const FString& guildName)
{
	FScriptDelegate func(EC_EventParm);
	func.Object = this;

	GuildId = guildId;
	OwnerName = guildName;

	UINT32 clientOption = CMessageHandler::GetInstance().GetClientOption();
	if (clientOption & CLIENT_OPTION_BIT_GUILD_WAR)
	{
		func.FunctionName = "ClickGuildWar";
		AddMenuItem(func, TEXT("GuildWar"));
	}

	// copy guild name
	func.FunctionName = "ClickCopyGuildNameByPanel";
	AddMenuItem(func, TEXT("CopyGuildName"));

	AddLine();

	return TRUE;
}

UBOOL UMMOUIRClickMenu::_MenuFormatByAuctionPlayer( INT roleId , const FString& playerName )
{
	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	OwnerName = playerName;
	OwnerRoleId = roleId;

	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)
		return FALSE;

	//check self
	if (OwnerRoleId == Player->ServerID)
		if( OwnerRoleId == Player->ServerID  )
	{
		if( TeamManager::getSingleton().HasTeam() )
		{
			func.FunctionName = "ClickLeaveTeam";
			AddMenuItem( func , TEXT("LeaveTeam") );
		}
		//view own profile
		func.FunctionName = "ClickViewProfile";
		AddMenuItem( func , TEXT("ViewProfile") );

		return TRUE;
	}


	//private chat
	func.FunctionName = "ClickEnterPrivateChat"; 
	AddMenuItem( func , TEXT("PrivateChat") );	


	if( FriendManager::getSingleton().IsMyFriend(OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(OwnerRoleId) )
	{   //unfollow
		func.FunctionName = "ClickDeleteFriend";
		AddMenuItem( func , TEXT("DelAttention") );	
	}
	else
	{
		//add to follow
		func.FunctionName = "ClickAddFriend";
		AddMenuItem( func , TEXT("AddAttention") );	
	}

	//copy name
	func.FunctionName = "ClickCopyName";
	AddMenuItem( func , TEXT("CopyName") );

	// guild category
	// copy guild name
	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(OwnerRoleId, TRUE);
	if (playerInfo && !playerInfo->_GuildName.IsEmpty())
	{
		func.FunctionName = "ClickCopyGuildName";
		AddMenuItem(func, TEXT("CopyGuildName"));
	}
	

	AddLine();
	return TRUE;

}

//-------------------------------------------------------
void UMMOUIRClickMenu::ClickMoveToBlackList( INT inMenuIdx )
{
	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = "MoveBlackOk";
	funOk.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.FunctionName = "MsgBoxNo";
	funNo.Object = this;

	MMOMsgBoxManager::getSingleton().AddMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.AskMoveBlack") ) , OwnerRoleId , funOk , funNo );
	Show(FALSE);


}

//-------------------------------------------------------
void UMMOUIRClickMenu::ClickRemoveFromBlackList( INT inMenuIdx )
{
	CMessageHandler::GetInstance().SendRemoveFromBlackList( OwnerRoleId );
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickSendMail( INT inMenuIdx )
{
	MailManager::getSingleton().NotifySendMailToReceiver(OwnerName);
	Show(FALSE);
}

//-------------------------------------------------------
void UMMOUIRClickMenu::ClickMoveFriend( INT inMenuIdx )
{

	if( !CurClickButton /*|| !MenuChild*/ )
	{
		return;
	}
	
	INT x = GetInt( TEXT("x") ) + GetObject(TEXT("RCMenuBg"))->GetInt(TEXT("width"));
	INT y = GetInt( TEXT("y") ) + CurClickButton->GetInt(TEXT("y")) ;

	MMOUI::GetUIManager()->delegateOpenRClickChildMenu_Friend(x,y,OwnerRoleId);

}

//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickAddFriend( INT inMenuIdx )
{
	FriendManager::getSingleton().RequestAddAttention( OwnerRoleId , TEXT("") );
	
	Show(FALSE);
}
void UMMOUIRClickMenu::ClickAddEnemy( int inMenuIdx )
{
	MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_AddEnemy,OwnerRoleId);
	MMOUI::PostGameEvent(&et);
	Show(FALSE);
}
void UMMOUIRClickMenu::ClickRemoveEnemy( int inMenuIdx )
{
	MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_RemoveEnemy,OwnerRoleId);
	MMOUI::PostGameEvent(&et);
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickAchievementCompare( INT inMenuIdx )
{
	if (MMOUI::GetUIManager())
	{
		//MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_Achievement);
		CMessageHandler::GetInstance().Send_AchievementCompare(OwnerRoleId); 
		Show(FALSE);
	}
}

void UMMOUIRClickMenu::DelFriendOk( UGFxObject* mc )
{
	//FM
	INT roleId = mc->GetInt( TEXT("iteminfoid") );
	FriendManager::getSingleton().RequestDelAttention( roleId );
}
//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::AddNewGroupOk(class UGFxObject* mc)
{
	INT GroupId;
	FString textInput;
	GroupId = FriendManager::getSingleton().GetEmptyFriendGroupId();
	if( GroupId >= 0 )
	{
		textInput = mc->GetObject( "textInput" )->GetString( TEXT("text") );
		MMOUI::GetUIManager()->delegateChangeGroupName( textInput , GroupId );
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.TooMoreGroup") ),MsgInfoType_Table,TEXT("MMOGame.GamePlay.TooMoreGroup") );
	}
}

//
UBOOL UMMOUIRClickMenu::_MenuFormatByNearPlayer(INT Index)
{
	MMONearbyPlayer* nearPlayer = TeamManager::getSingleton().GetNearPlayer(Index);
	if(!nearPlayer)
	{
		return FALSE;
	}
	//;
	OwnerName = nearPlayer->ActorName;
	OwnerRoleId = nearPlayer->ActorServerId;

	if( FriendManager::getSingleton().IsMyAttention(OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(OwnerRoleId) )
	{
		FScriptDelegate fun;
		fun.FunctionName = "ClickDeleteFriend";
		fun.Object = this;

		AddMenuItem( fun , TEXT("DelAttention") );	
	}
	else
	{
		FScriptDelegate fun;
		fun.FunctionName = "ClickAddFriend";
		fun.Object = this;
		AddMenuItem( fun , TEXT("AddAttention") );	
	}
	if(FriendManager::getSingleton().InMyEnemyList(OwnerRoleId))
	{
		FScriptDelegate func;
		func.Object = this;
		func.FunctionName = "ClickRemoveEnemy";
		AddMenuItem( func , TEXT("RemoveEnemy") );	
	}
	else
	{
		FScriptDelegate func;
		func.Object = this;
		func.FunctionName = "ClickAddEnemy";
		AddMenuItem( func , TEXT("AddEnemy") );	
	}
	FScriptDelegate fun;
	fun.FunctionName = "ClickEnterPrivateChat";
	fun.Object = this;
	AddMenuItem( fun , TEXT("PrivateChat") );

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickInviteBattleTeam( INT inMenuIdx )
{
	CMessageHandler::GetInstance().Send_JoinBattleTeam( OwnerRoleId );
	Show(FALSE);
}

//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickRemoveBattleTeam( INT inMenuIdx )
{
	CMessageHandler::GetInstance().Send_LeaveBattleTeam( OwnerRoleId );
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickLeaveTeam( INT inMenuIdx )
{
	//TeamManager::getSingleton().LeaveTeam( OwnerRoleId );
	MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_LeaveTeam,OwnerRoleId);
	MMOUI::PostGameEvent(&et);

	Show(FALSE);
}

void UMMOUIRClickMenu::ClickMoveLeader( INT inMenuIdx )
{
	//TeamManager::getSingleton().MoveLeader( OwnerRoleId );
	MMOGameEvent_TeamCommon et(MMOGameEvent_TeamCommon::ETeamEventType_MoveLeader,OwnerRoleId);
	MMOUI::PostGameEvent(&et);

	Show(FALSE);
}

void UMMOUIRClickMenu::ClickEnterPrivateChat( INT inMenuIdx )
{
	PlayerBaseInfo* baseinfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( OwnerRoleId);
	if (baseinfo)
	{
		FString RoleName = baseinfo->_RoleName;
		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if(curMapInfo.HideRoleName)
		{
			RoleName = FString(" ");
		}
		MMOUI::GetUIManager()->delegateEnterPrivateChat( RoleName );
		Show(FALSE);
	}
}
//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickDelGroup( INT inMenuIdx )
{
	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = "DelGroupOk";
	funOk.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.FunctionName = "MsgBoxNo";
	funNo.Object = this;

	MMOMsgBoxManager::getSingleton().AddMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Friend.AskDelGroup") ) , OwnerGroupId , funOk , funNo );
	Show(FALSE);
}

//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::DelGroupOk( class UGFxObject* mc )
{
	if(!mc)
		return;

	INT groupId = mc->GetInt( TEXT("iteminfoid") );

	FriendManager::getSingleton().DeleteFriendGroup(groupId);
}
//-------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::MoveBlackOk( class UGFxObject* mc )
{
	INT roleId = mc->GetInt( TEXT("iteminfoid") );
	CMessageHandler::GetInstance().SendMoveToBlackList( roleId , TEXT("") );	
}

void UMMOUIRClickMenu::ClickNewGroup( INT inMenuIdx )
{
	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = "AddNewGroupOk";
	funOk.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.FunctionName = "MsgBoxNo";
	funNo.Object = this;

	UGFxObject* msgBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.NewGroup") ) , OwnerGroupId , 
		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.InputFriendGroupName") ) , funOk,  funNo, eInputFormatAll );

	if(msgBox)
	{
		UGFxObject* input = msgBox->GetObject( TEXT("textInput") );
		if(input)
			input->SetInt( TEXT("maxChars") , 8 );
	}
	
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickInvitePlayerIntoTeam( INT inMenuIdx )
{
	if(TeamManager::getSingleton().IsTeamLeader())
	{
		if( TeamManager::getSingleton().GetTeamMemberCount() >= TeamManager::getSingleton().GetMemberLimit())
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.TeamNumFull")));
			Show(FALSE);
			return;
		}
	}

	/*if (TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader())
	{
		CMessageHandler::GetInstance().Send_EnterTeam(OwnerRoleId);
	}
	else
	{
		TeamManager::getSingleton().SetTeamTargetRoldId(OwnerRoleId);
		CMessageHandler::GetInstance().Send_QueryTeam( OwnerRoleId );
	}*/

	TeamManager::getSingleton().QueryRoleHasTeam(OwnerRoleId);
	
	
	TeamManager::getSingleton().AddInvitedPlayer(OwnerRoleId);
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickChangeGroupName( INT inMenuIdx )
{
	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = "AddChangeNameOk";
	funOk.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.FunctionName = "MsgBoxNo";
	funNo.Object = this;

	UGFxObject* msgBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ChangeGroupName") ) , 
		OwnerGroupId , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.InputFriendGroupName") ) , funOk,  funNo , eInputFormatAll );

	if(msgBox)
	{
		UGFxObject* input = msgBox->GetObject( TEXT("textInput") );
		if(input)
			input->SetInt( TEXT("maxChars") , 8 );
	}
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickDeleteFriend( INT inMenuIdx )
{
	FScriptDelegate funOk(EC_EventParm);
	funOk.FunctionName = "DelFriendOk";
	funOk.Object = this;

	FScriptDelegate funNo(EC_EventParm);
	funNo.FunctionName = "MsgBoxNo";
	funNo.Object = this;

	MMOMsgBoxManager::getSingleton().AddMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AskDelFriend") ) + OwnerName , OwnerRoleId , funOk , funNo );
	Show(FALSE);
}


void UMMOUIRClickMenu::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_TargetChange);
	EventAdapter = new TGameEventAdapter<UMMOUIRClickMenu>(this, evts);

	//if( GFXPlayer )
	//{
	//	MenuChild = (UMMOUIRClickMenuChild*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_RClickChildMenu);//(UMMOUIRClickMenu*)GFXPlayer->GetVariableObject( TEXT("root.RClickMenuChild_mc") , UMMOUIRClickMenu::StaticClass() );
	//}
	
	/*if( MenuChild )
	{
	MenuChild->SetVisible( false );
	MenuChild->SetInt( TEXT("MenuLevel") , ERCMLevel_Child );
	MenuChild->eventCallResetChildMenuButton();
	}*/

	MMOUI::GetUIManager()->SetHideRClickMenuDelegate(FScriptDelegate(this, TEXT("ClearUI")));
}

void UMMOUIRClickMenu::PostOnShow(UBOOL v)
{
	if (v == FALSE)
	{
		MMOUI::GetUIManager()->delegateHideRClickChildMenu();
	}
	Super::PostOnShow(v);
}

void UMMOUIRClickMenu::OnFocusLost()
{
	UMMOUIBase::OnFocusLost();

	//UMMOUIBase* menu = MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_RClickMenu);
	//if(menu)
	//{
		SetVisible(FALSE);
		MMOUI::GetUIManager()->delegateHideRClickChildMenu();
	//}
	//MMOUI::GetUIManager()->ShowPanel( MMOUI::GetUIManager()->Panel_RClickMenu , FALSE );
}

void UMMOUIRClickMenu::ClickSimpleProfile( INT inMenuIdx )
{
	if( Player && Player->ServerID == OwnerRoleId )
	{
		return;
	}

	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_SimplePlayerInfo , TRUE, "" );
	UMMOUISimplePlayerInfo* playerInfo = Cast<UMMOUISimplePlayerInfo>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_SimplePlayerInfo ) );
	if(playerInfo)
	{
		playerInfo->InitPlayerRoleId( OwnerRoleId );
	}
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickViewProfile( INT inMenuIdx )
{
	
	if( Player && Player->ServerID == OwnerRoleId )
	{
		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_Character , TRUE, "" );
	}
	else
	{
		MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo , TRUE, "" );
		UMMOUIPlayerInfo* playerInfo = Cast<UMMOUIPlayerInfo>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo ) );
		if(playerInfo)
		{
			playerInfo->InitPlayerRoleId( OwnerRoleId );
		}
	}

	Show(FALSE);
}
//click follow/unfollow
void UMMOUIRClickMenu::ClickFollow( INT inMenuIdx ){
	//Follow
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	if(pc){
		OwnerActor = GetActorByRoleId(OwnerRoleId);
		if(OwnerActor){
			pc->eventAddCmdMoveToActor(OwnerActor,OwnerActor->Location, 500.0f,0,TRUE,TRUE);
			Show(FALSE);
			return;
		}
	}
}
void UMMOUIRClickMenu::ClickNullFollow( INT inMenuIdx ){
	//Unfollow
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	if(pc){
		pc->eventClearAllCmd();
		Show(FALSE);
	}
}

// Invite to join clan
void UMMOUIRClickMenu::ClickInviteToJoinGuild( INT inMenuIdx )
{
	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(OwnerRoleId);
	if (playerInfo)
	{	
		CMessageHandler::GetInstance().Send_InviteToJoinGuild(playerInfo->_RoleName);
	}

	Show(FALSE);
}

//Recommend joining clan
void UMMOUIRClickMenu::ClickRecommendJoinGuild( INT inMenuIdx )
{
	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(OwnerRoleId);
	if (playerInfo)
	{	
		CMessageHandler::GetInstance().Send_InviteToJoinGuild(playerInfo->_RoleName);
	}

	Show(FALSE);
}

void UMMOUIRClickMenu::ClickApplyDoTrade( INT inMenuIdx )
{
	//MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_TradeP2P );
	UMMOUITradeP2P *pTradeP2P = (UMMOUITradeP2P*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_TradeP2P );//for loading
	MMOUI::GetUIManager()->delegateApplyDoTrade(OwnerRoleId);	
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickOpenContactDialog( INT inMenuIdx )
{
	if(FriendManager::getSingleton().IsMyAttention(OwnerRoleId))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errFriendNotMutual"));
		MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Warning,text);
	}
	else
	{
		MMOUI::GetUIManager()->delegateSelectFriendChat( OwnerRoleId , OwnerName );
	}
	
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickChangeTeamAuctionLimit( INT inMenuIdx )
{
	if( !CurClickButton /*|| !MenuChild*/ )
	{
		return;
	}


	INT width = GetInt( TEXT("width") );
	INT x = GetInt( TEXT("x") ) + width;
	INT y = GetInt( TEXT("y") ) + CurClickButton->GetInt(TEXT("y")) ;
	
	//Reset submenu position
	MMOUI::GetUIManager()->delegateOpenRClickChildMenu_Team(x,y,width);
}


//Apply to join clan
void UMMOUIRClickMenu::ClickApplyToJoinGuild( INT inMenuIdx )
{
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(OwnerRoleId);
	if (actor)
	{
		CMessageHandler::GetInstance().Send_ApplyToJoinGuild(0, actor->GuildName);
	}
	Show(FALSE);
}

//Kick from clan
void UMMOUIRClickMenu::ClickKickOutGuild( INT inMenuIdx )
{
	UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Main);
	if (mainPanel)
	{
		mainPanel->ExpelMember(OwnerRoleId);
	}
	Show(FALSE);
}

//Guild mute
void UMMOUIRClickMenu::ClickForbidChatOn( INT inMenuIdx )
{
	CMessageHandler::GetInstance().Send_GuildForbidChat(GUILD_CHAT_FORBID, OwnerRoleId);
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickForbidChatOff( INT inMenuIdx )
{
	CMessageHandler::GetInstance().Send_GuildForbidChat(GUILD_CHAT_UNFORBID, OwnerRoleId);
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickCancelAlliance( INT inMenuIdx )
{
	if (GuildId > 0)
	{
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanel(uiManager->Panel_Guild_Main);
			if (mainPanel)
			{
				mainPanel->AllyCancelConfirm(GuildId);
			}
		}
	}
	Show(FALSE);
}

void UMMOUIRClickMenu::ClickGuildWar(INT inMenuIdx)
{
	GuildQueryInfo guildInfo;
	if (MMOGuildData::getSingleton().GetGuildInfoById(GuildId, guildInfo))
	{
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanel(uiManager->Panel_Guild_Main);
			if (mainPanel)
			{
				if (!MMOGuildData::getSingleton().IsGuildEnemy(guildInfo.Name))
				{
					FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ChallengeNotEnemy"));
					MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.ChallengeNotEnemy"));
				}
				else
				{
					mainPanel->ChallengeConfirm(guildInfo.Name);
				}
			}
		}
	}
	Show(FALSE);
}

//Master/apprentice
//become apprentice
void UMMOUIRClickMenu::ClickApplyMaster( INT inMenuIdx )
{
	CMessageHandler::GetInstance().Send_SubmitApplyToMaster(OwnerRoleId);
	Show(FALSE);
}
void UMMOUIRClickMenu::ClickApplyStudent( INT inMenuIdx )
{
	CMessageHandler::GetInstance().Send_SubmitApplyToStudent(OwnerRoleId);
	FString hintText = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Master.ApplyStudent"));
	MMOUI::GetUIManager()->delegateAddScreenMessage( hintText , MsgInfoType_Table , hintText );
	MMOChatManager::getSingleton().AddSystemMessage( hintText );
	Show(FALSE);
}
//Take apprentice
void UMMOUIRClickMenu::ClickReadmastermsg( INT inMenuIdx )
{
	Show(FALSE);
}
//Expel from master's school
void UMMOUIRClickMenu::ClickExpell( INT inMenuIdx )
{
	if (_ConfirmBox)
	{
		_ConfirmBox->SetVisible(FALSE);
		_ConfirmBox = NULL;
	}
	

	FScriptDelegate FireStudentDelegate(EC_EventParm);
	FireStudentDelegate.FunctionName = TEXT("FireStudent");
	FireStudentDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("OnFireStudentCancel");
	Nofunction.Object = this;

	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(OwnerRoleId);

	if (playerInfo)
	{
		FString szText = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.IsFireStudent")) , *playerInfo->_RoleName);
		_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , FireStudentDelegate , Nofunction );

		if(_ConfirmBox)
			_ConfirmBox->SetInt( TEXT("dataid") , OwnerRoleId);
	}
	Show(FALSE);
}


void UMMOUIRClickMenu::FireStudent(UGFxObject* mc)
{
	INT nID = mc->GetInt(TEXT("dataid"));
	CMessageHandler::GetInstance().Send_FireStudent(nID);
	_ConfirmBox = NULL;
}

void UMMOUIRClickMenu::OnFireStudentCancel()
{
	_ConfirmBox = NULL;
}

void UMMOUIRClickMenu::LeaveMaster(UGFxObject* m)
{
	if (_ConfirmBox)
	{
		_ConfirmBox->SetVisible(FALSE);
		_ConfirmBox = NULL;
	}

	CMessageHandler::GetInstance().Send_LeaveMaster();
	_ConfirmBox = NULL;
}
void UMMOUIRClickMenu::LeaveMasterCancel()
{
	_ConfirmBox = NULL;
}

//Betray master
void UMMOUIRClickMenu::ClickBetryMaster( INT inMenuIdx )
{
	FScriptDelegate LeaveMasterDelegate(EC_EventParm);
	LeaveMasterDelegate.FunctionName = TEXT("LeaveMaster");
	LeaveMasterDelegate.Object = this;

	FScriptDelegate Nofunction(EC_EventParm);
	Nofunction.FunctionName = TEXT("LeaveMasterCancel");
	Nofunction.Object = this;

	FString szText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MasterStudent.StudentLeaveMaster"));
	_ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( szText , 0 , LeaveMasterDelegate , Nofunction );

	Show(FALSE);
}

UBOOL UMMOUIRClickMenu::ChangeMenu(INT RoleId,const FString& RoleName,INT index,BYTE type)
{
	step = 0;
	lineStep = 0;
	high = 0;
	UGFxObject* lineBase = GetObject( TEXT("splitLine") );

	if( lineBase != NULL )
	{
		high = lineBase->GetInt( "height" ) + lineBase->GetInt("y");
	}

	curType = type;
	switch( type )
	{
	case ERCMType_TargetInfo:
		return _MenuFormatByTargetInfo(RoleId, RoleName);
		break;

	case ERCMType_TeamMember:
		return _MenuFormatByMyTeam(index);
		break;

	case ERCMType_TeamMate:
		return _MenuFormatByTeamMate(index);
		break;

	case ERCMType_TeamNearby:
		return _MenuFormatByNearPlayer(index);
		break;	

	case ERCMType_Friend:
	case ERCMType_Contact:
		return _MenuFormatByFriend(RoleId, RoleName);
		break;

	case ERCMType_RankListPlayer:
		return _MenuFormatByRankListPlayer(RoleId, RoleName);
		break;

	case ERCMType_RankListGuild:
		return _MenuFormatByRankListGuild(RoleId, RoleName);
		break;

	case ERCMType_Attention:
		return _MenuFormatByAttention(index, RoleName);
		break;

	case ERCMType_FriendGroup:
		return _MenuFormatByFriendGroup(index, RoleName);
		break;

	case ERCMType_BattleMember:
		return _MenuFormatByBattleMember(RoleId);
		break;

	//case ERCMType_Black:
	//	return _MenuFormatByBlack(index);
	//	break;

	case ERCMType_GuildMember:
		return _MenuFormatByGuildMember(RoleId);
		break;

	case ERCMType_GuildAlliance:
		return _MenuFormatByGuildAlliance(RoleId, RoleName);
		break;

	case ERCMType_GuildEnemy:
		return _MenuFormatByGuildEnemy(RoleId, RoleName);
		break;

	case ERCMType_ChatPlayer:
		return _MenuFormatByChatPlayer(RoleId, RoleName);
		break;

	case ERCMType_Dungeon:
		return _MenuFormatByDungeon(RoleName);
		break;

	case ERCMType_BattleEnter:
		return _MenuFormatByBattleEnter(RoleName);
		break;

	case ERCMType_ChangeMapLine:
		return _MenuFormatByMapChangeLine(RoleName);
		break;

	case ERCMType_MasterStudent:
		return _MenuFormatByMasterPlayer(RoleId, RoleName);
		break;
	case ERCMType_AuctionPlayer:
		return _MenuFormatByAuctionPlayer(RoleId, RoleName);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

//=======================================================================================================================
//Map line switch button right-click menu
UBOOL UMMOUIRClickMenu::_MenuFormatByMapChangeLine( const FString &title )
{
	OwnerName = title;

	//
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();

	FExportMap &curMapInfo = pMapDataManager->GetCurMap();
	UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
	if( canChangeLines == FALSE ){
		FString errorName = TEXT("MMOGame.Option.ErrorNoOtherLine");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return FALSE;
	}

	//get the lines of the current map
	TArray< MapLineInfo > &sceneLines = pMapDataManager->GetCurMapLines();
	INT curCombinatedSceneID = pMapDataManager->GetCurSceneLineIn();

	FString listItemStringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Option.MapLineListItemFormat") );
	INT linesCnt = sceneLines.Num();
	for( INT i = 0; i < linesCnt; ++i ){
		INT combinatedSceneID = sceneLines(i)._SceneLineCombineID;
		if( curCombinatedSceneID == combinatedSceneID ){
			continue;
		}
		//
		UINT sceneLineID = 0;
		UINT sceneServerID = 0;
		MMOMapDataManager::ParseServerMapLine( combinatedSceneID, sceneServerID, sceneLineID );
		//
		FScriptDelegate func( EC_EventParm);
		func.Object = this;

		func.FunctionName = "ClickChangeMapLine";
		AddMenuItem( func, FString::Printf( *listItemStringFormat, sceneLineID ), FALSE );
	}

	if( linesCnt <= 1 ){
		FString errorName = TEXT("MMOGame.Option.ErrorNoOtherLine");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickChangeMapLine( INT inMenuIdx )
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();

	//get the lines of the current map
	TArray< MapLineInfo > &sceneLines = pMapDataManager->GetCurMapLines();
	INT curCombinatedSceneID = pMapDataManager->GetCurSceneLineIn();

	//The list items do not include the line of the current map, so the index needs conversion
	INT linesCnt = sceneLines.Num();
	INT curLineInListIdx = linesCnt;
	for( INT i = 0; i < linesCnt; ++i ){
		INT combinatedSceneID = sceneLines(i)._SceneLineCombineID;
		if( curCombinatedSceneID == combinatedSceneID ){
			curLineInListIdx = i;
			break;
		}
	}

	INT lineInListIdx = inMenuIdx;
	if( lineInListIdx >= curLineInListIdx ){
		++lineInListIdx;
	}

	//
	if( lineInListIdx < linesCnt ){
		UMMOUIMinimap* pMinimap = Cast<UMMOUIMinimap>( MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap ) );
		if( pMinimap != NULL ){
			pMinimap->PreparingToChangeSceneLine( sceneLines(lineInListIdx)._SceneLineCombineID );
		}
	}

	Show(FALSE);
}

//=======================================================================================================================
//Leave dungeon
UBOOL UMMOUIRClickMenu::_MenuFormatByDungeon(const FString& title)
{
	OwnerName = title;

	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	func.FunctionName = "ClickLeaveFB";
	AddMenuItem( func , TEXT("LeaveFB") );
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickLeaveFB( INT inMenuIdx )
{
	UMMOUIMinimap *pMiniMapPanel = ( UMMOUIMinimap* )MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( pMiniMapPanel != NULL && pMiniMapPanel->ConfirmMsgBox != NULL ){
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = pMiniMapPanel;
	funYes.FunctionName = TEXT( "ReqExitDuplicate_Ok" );

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = pMiniMapPanel;
	funNo.FunctionName = TEXT( "ReqExitDuplicate_Cancel" );

	//popup message
	if( pMiniMapPanel != NULL ){
		FString msgInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral6") );
		pMiniMapPanel->ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgInfo, E_est_Dungeon, funYes, funNo );
	}

	Show(FALSE);
}

//=======================================================================================================================
UBOOL UMMOUIRClickMenu::_MenuFormatByBattleEnter( const FString &title )
{
	OwnerName = title;

	FScriptDelegate func( EC_EventParm);
	func.Object = this;

	func.FunctionName = "ClickLeaveBattleEnterQueue";
	AddMenuItem(func, TEXT("LeaveBattleEnterQueue"));
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIRClickMenu::ClickLeaveBattleEnterQueue( INT inMenuIdx )
{
	UMMOUIPVPBattle *pvpBattlePanel = (UMMOUIPVPBattle*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PVP_PVPBattle );
	if( pvpBattlePanel == NULL ){
		warnf( NAME_Error, TEXT("Panel_PVP_PVPBattle not found at UMMOGFXPlayer_Minimap::ASClickBattleInfo") );
		return;
	}
	//
	pvpBattlePanel->LeaveBattle();

	Show(FALSE);
}
//=======================================================================================================================

//Marry
//Propose
void UMMOUIRClickMenu::ClickMerry( INT inMenuIdx ){}
//Divorce
void UMMOUIRClickMenu::ClickDivorce( INT inMenuIdx ){}

//general settings
//block chat
void UMMOUIRClickMenu::ClickCommon1( INT inMenuIdx ){}
//unblock
void UMMOUIRClickMenu::ClickCommon2( INT inMenuIdx ){}
//Report player
void UMMOUIRClickMenu::ClickCommon3( INT inMenuIdx ){}
//Track position
void UMMOUIRClickMenu::ClickCommon4( INT inMenuIdx ){}

//copy name
void UMMOUIRClickMenu::ClickCopyName( INT inMenuIdx )
{
	appClipboardCopy( *OwnerName );
	Show(FALSE);
}

void UMMOUIRClickMenu::ASClickRightMenuItem(INT MenuLevel,INT ItemId)
{
	eventClickItem( MenuLevel , ItemId );
}

//Copy guild name (via player)
void UMMOUIRClickMenu::ClickCopyGuildName( INT inMenuIdx )
{
	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(OwnerRoleId, TRUE);
	if (playerInfo && !playerInfo->_GuildName.IsEmpty())
	{	
		appClipboardCopy(*(playerInfo->_GuildName));
	}
	Show(FALSE);
}

//Copy guild name (via UI)
void UMMOUIRClickMenu::ClickCopyGuildNameByPanel(INT inMenuIdx)
{
	GuildQueryInfo guildInfo;
	if (MMOGuildData::getSingleton().GetGuildInfoById(GuildId, guildInfo))
	{
		appClipboardCopy(*(guildInfo.Name));
	}
	Show(FALSE);
}

UBOOL UMMOUIRClickMenu::SkipCaptureInputKey( const FString& key, BYTE eIE, const FString& strHoverControllerName, FVector2D ClickPosInMovieSpace )
{
	if( strHoverControllerName == TEXT( "Rclick_ScreenMask" ))
	{
		if(key==TEXT("LeftMouseButton") || key==TEXT("RightMouseButton"))
		{
			ClearUI();
			MMOUI::GetUIManager()->delegateHideRClickChildMenu();

			return CheckSkipCapture();
		}
	}

	return Super::SkipCaptureInputKey( key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

BOOL UMMOUIRClickMenu::CheckSkipCapture()
{
	UMMOUIRankListPanel* rankPanel = Cast<UMMOUIRankListPanel>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_RankList));
	if (rankPanel != NULL && rankPanel->GetVisible())
	{
		return FALSE;
	}

	UMMOUIGuildMainPanel* mainPanel = Cast<UMMOUIGuildMainPanel>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Guild_Main));
	if (mainPanel != NULL && mainPanel->GetVisible())
	{
		return FALSE;
	}

	return TRUE;
}



