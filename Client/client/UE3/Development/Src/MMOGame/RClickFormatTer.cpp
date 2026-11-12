#include "stdafx.h"
#include "RClickFormatter.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "MMOBuffManager.h"
#include "MMOAttributeAccountManager.h"
#include "MMOEquipManager.h"
#include "MMOSkillManager.h"
#include "MMOHeavenManager.h"
#include "AbilityProcessor.h"
#include "FriendManager.h"
#include "BattleTeamManager.h"
#include "MMOQuestManager.h"
#include "TeamManager.h"
#include "MMOGuildManager.h"
#include "MasterStudentManager.h"
#include "client_option_def.h"

RClickFormatter::RClickFormatterMap RClickFormatter::_RClickFormatterMap;


bool InitRClickFormatterMap()
{
#define M_RGE_FMTTER(e, cls) { static cls g_##cls; RClickFormatter::RegRClickFormatter(e, &g_##cls); }

	M_RGE_FMTTER(ERCMType_TargetInfo    , RClickFormatter_TargetInfo    );
	M_RGE_FMTTER(ERCMType_TeamMember    , RClickFormatter_TeamMember    );
	M_RGE_FMTTER(ERCMType_TeamMate      , RClickFormatter_TeamMate      );
	M_RGE_FMTTER(ERCMType_ChatPlayer    , RClickFormatter_ChatPlayer    );
	M_RGE_FMTTER(ERCMType_TeamNearby    , RClickFormatter_TeamNearby    );
	M_RGE_FMTTER(ERCMType_Friend        , RClickFormatter_Friend        );
	M_RGE_FMTTER(ERCMType_BattleMember  , RClickFormatter_BattleMember  );
	M_RGE_FMTTER(ERCMType_FriendGroup   , RClickFormatter_FriendGroup   );
	M_RGE_FMTTER(ERCMType_Attention     , RClickFormatter_Attention     );
	M_RGE_FMTTER(ERCMType_Black         , RClickFormatter_Black         );
	M_RGE_FMTTER(ERCMType_GuildMember   , RClickFormatter_GuildMember   );
	M_RGE_FMTTER(ERCMType_Dungeon       , RClickFormatter_Dungeon       );
	M_RGE_FMTTER(ERCMType_RankListPlayer, RClickFormatter_RankListPlayer);
	M_RGE_FMTTER(ERCMType_RankListGuild , RClickFormatter_RankListGuild );
	M_RGE_FMTTER(ERCMType_GuildAlliance , RClickFormatter_GuildAlliance );
	M_RGE_FMTTER(ERCMType_AuctionPlayer , RClickFormatter_AuctionPlayer);
	M_RGE_FMTTER(ERCMType_GuildEnemy	, RClickFormatter_GuildEnemy);
    return true;
}

static bool g_InitRClickFormatter = InitRClickFormatterMap();

void RClickFormatter::RegRClickFormatter( ERClickMenuType RClickType, IRClickFormatter* fmt )
{
    _RClickFormatterMap[RClickType] = fmt;
}

IRClickFormatter* RClickFormatter::GetRClickFormatter( ERClickMenuType RClickType )
{
    RClickFormatterMap::iterator i = _RClickFormatterMap.find(RClickType);
    if(i != _RClickFormatterMap.end())
    {
        return i->second;
    }
    return 0;
}

void RClickFormatter::FormatRClick(ERClickMenuType RClickType, class UMMOUIRClickMenu* RClickMenu)
{
    IRClickFormatter* fmt = RClickFormatter::GetRClickFormatter(RClickType);
    if(fmt)
    {
        fmt->FormatRClick(RClickMenu);
    }
    else
    {
        warnf(NAME_Error, TEXT("[RClickFormatter::FormatRClick] formater not found:%d"), (INT)RClickType);
    }

}


//check self
UBOOL RClick_CheckSelf(class UMMOUIRClickMenu* RClickMenu , FScriptDelegate &func)
{
	if(RClickMenu==NULL)return TRUE;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return TRUE;


	//clicked on myself
	if( RClickMenu->OwnerRoleId == Player->ServerID  )
	{
		//self leaves team
		if( TeamManager::getSingleton().HasTeam() )
		{
			func.FunctionName = "ClickLeaveTeam";
			RClickMenu->AddMenuItem( func , TEXT("LeaveTeam") );
		}
		//view own profile
		func.FunctionName = "ClickViewProfile";
		RClickMenu->AddMenuItem( func , TEXT("ViewProfile") );
		return TRUE;
	}	
	return FALSE;
}

//contact category
void RClick_Contant(class UMMOUIRClickMenu* RClickMenu , FScriptDelegate &func)
{
	LOG_PROCESS_ERROR(RClickMenu);
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	LOG_PROCESS_ERROR(Player);

	//contact category
	//popup chat
	if( FriendManager::getSingleton().IsMyFriend( RClickMenu->OwnerRoleId )  )
	{
		func.FunctionName = "ClickOpenContactDialog";
		RClickMenu->AddMenuItem( func , TEXT("EnterChat") );
	}	
	//private chat
	func.FunctionName = "ClickEnterPrivateChat"; 
	RClickMenu->AddMenuItem( func , TEXT("PrivateChat") );	
	//mail
	func.FunctionName = "ClickSendMail";
	RClickMenu->AddMenuItem( func , TEXT("SendMail") );
	//master/apprentice
	if (E_MASTER_NONE == MasterStudentManager::getSingleton().GetMasterStudentState())
	{
		int PlayerLv = Player->ActorAttributes->LevelAttributes.Level;
		if(GRADUATE_LEVEL > PlayerLv&& PlayerLv >= STUD_LEVEL_LOWER_LIMIT)
		{
			func.FunctionName = "ClickApplyMaster";
			RClickMenu->AddMenuItem(func, TEXT("ApplyMaster"));
		}
	}
	else if (E_MASTER_STUDENT == MasterStudentManager::getSingleton().GetMasterStudentState())
	{
		const MasterDetailInfo& pMasterInfo = MasterStudentManager::getSingleton().GetMasterDetailInfo();
		if(pMasterInfo.nRoleID > 0)
		{
			if(appStrcmp(*pMasterInfo.masterName,*RClickMenu->OwnerName) == 0)
			{
				func.FunctionName = "ClickBetryMaster";
				RClickMenu->AddMenuItem(func, TEXT("BetryMaster"));
			}
		}
	}
	else if (E_MASTER_TEACHER == MasterStudentManager::getSingleton().GetMasterStudentState())
	{
		const StudentListData* pStudent = MasterStudentManager::getSingleton().GetStudentDataByID(RClickMenu->OwnerRoleId);
		if (NULL != pStudent)
		{
			func.FunctionName = "ClickExpell";
			RClickMenu->AddMenuItem(func, TEXT("Expell"));
		}
		else
		{
			func.FunctionName = "ClickApplyStudent";
			RClickMenu->AddMenuItem(func, TEXT("Readmastermsg"));
		}
	}

Exit0:
	return;
}


//contact category
UBOOL RClick_Cooperate(class UMMOUIRClickMenu* RClickMenu , FScriptDelegate &func)
{
	LOG_PROCESS_ERROR(RClickMenu);
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	LOG_PROCESS_ERROR(Player);
	//cooperation category
	RClickMenu->AddLine();
	//team category
	{
		//clicked on myself
		if( RClickMenu->OwnerRoleId == Player->ServerID && TeamManager::getSingleton().HasTeam() )
		{
			//self leaves team
			func.FunctionName = "ClickLeaveTeam";
			RClickMenu->AddMenuItem( func , TEXT("LeaveTeam") );
			return TRUE;
		}

		if( RClickMenu->OwnerActor /*&& OwnerActor->TeamID > 0*/ && !TeamManager::getSingleton().HasTeam() ) //I have no team
		{
			//apply to join team
			func.FunctionName = "ClickInvitePlayerIntoTeam";
			RClickMenu->AddMenuItem(  func , TEXT("ApplyTeam") );
		}
		else if( RClickMenu->OwnerActor /*&& OwnerActor->TeamID <= 0*/ && TeamManager::getSingleton().HasTeam() /*&& TeamManager::getSingleton().IsTeamLeader()*/ )
		{
			//invite to team
			func.FunctionName = "ClickInvitePlayerIntoTeam";
			RClickMenu->AddMenuItem( func , TEXT("InviteTeam") );
		}
		else if( RClickMenu->OwnerActor /*&& OwnerActor->TeamID <= 0*/ && !TeamManager::getSingleton().HasTeam() )
		{
			func.FunctionName = "ClickInvitePlayerIntoTeam";
			RClickMenu->AddMenuItem( func , TEXT("InviteTeam") );
		}

		//if( BattleTeamManager::getSingleton().HasTeam() && BattleTeamManager::getSingleton().IsTeamLeader() )
		//{
		//	func.FunctionName = "ClickInviteBattleTeam";
		//	RClickMenu->AddMenuItem( func , TEXT("InviteBattleTeam") );
		//}

		if( TeamManager::getSingleton().HasTeam() && TeamManager::getSingleton().IsTeamLeader() )
		{
			if(TeamManager::getSingleton().GetMemberById(RClickMenu->OwnerRoleId))//if in team
			{
				//kick from team
				func.FunctionName = "ClickLeaveTeam";
				RClickMenu->AddMenuItem( func , TEXT("KickOutTeam") );
				//transfer leader
				func.FunctionName = "ClickMoveLeader";
				RClickMenu->AddMenuItem( func , TEXT("MoveLeader") );
			}
		}
	}

Exit0:
	return FALSE;
}

//guild category
UBOOL RClick_Guild(class UMMOUIRClickMenu* RClickMenu, FScriptDelegate &func)
{
	if (RClickMenu)
	{
		RClickMenu->AddMenuItem_Guild(func);
	}
	return FALSE;
}

void RClickFormatter_TargetInfo::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	//view profile

	func.FunctionName = "ClickViewProfile";
	RClickMenu->AddMenuItem( func , TEXT("ViewProfile") );

	//click follow/unfollow
	if(1){
		func.FunctionName = "ClickFollow";
		RClickMenu->AddMenuItem( func , TEXT("Follow") );
	}
	else
	{
		func.FunctionName = "ClickNullFollow";
		RClickMenu->AddMenuItem( func , TEXT("NullFollow") );
	}
	

	//add as friend

		if( FriendManager::getSingleton().IsMyFriend(RClickMenu->OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(RClickMenu->OwnerRoleId) )
		{   //unfollow
			func.FunctionName = "ClickDeleteFriend";
			RClickMenu->AddMenuItem( func , TEXT("DelAttention") );	
		}
		else
		{
			//add to follow
			func.FunctionName = "ClickAddFriend";
			RClickMenu->AddMenuItem( func , TEXT("AddAttention") );	
		}
		if(FriendManager::getSingleton().InMyEnemyList(RClickMenu->OwnerRoleId))
		{
			func.FunctionName = "ClickRemoveEnemy";
			RClickMenu->AddMenuItem( func , TEXT("RemoveEnemy") );	
		}
		else
		{
			func.FunctionName = "ClickAddEnemy";
			RClickMenu->AddMenuItem( func , TEXT("AddEnemy") );	
		}

	
	//request trade

		func.FunctionName = "ClickApplyDoTrade";
		RClickMenu->AddMenuItem( func, TEXT("ApplyDoTrade") );
	
	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//guild category
	RClick_Guild(RClickMenu, func);

	//general settings

	RClickMenu->AddLine();
}


void RClickFormatter_TeamMember::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;

	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//general settings
	RClickMenu->AddLine();
}

void RClickFormatter_TeamMate::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{

	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	//general settings
	RClickMenu->AddLine();
}


void RClickFormatter_ChatPlayer::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;

	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	//view profile

	//add as friend

	if( FriendManager::getSingleton().IsMyFriend(RClickMenu->OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(RClickMenu->OwnerRoleId) )
	{   //unfollow
		func.FunctionName = "ClickDeleteFriend";
		RClickMenu->AddMenuItem( func , TEXT("DelAttention") );	
	}
	else
	{
		//add to follow
		func.FunctionName = "ClickAddFriend";
		RClickMenu->AddMenuItem( func , TEXT("AddAttention") );	
	}
	if(FriendManager::getSingleton().InMyEnemyList(RClickMenu->OwnerRoleId))
	{
		func.FunctionName = "ClickRemoveEnemy";
		RClickMenu->AddMenuItem( func , TEXT("RemoveEnemy") );	
	}
	else
	{
		func.FunctionName = "ClickAddEnemy";
		RClickMenu->AddMenuItem( func , TEXT("AddEnemy") );	
	}

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//guild category
	RClick_Guild(RClickMenu, func);
	
	//general settings
	RClickMenu->AddLine();


	if( FriendManager::getSingleton().InMyBlackList(RClickMenu->OwnerRoleId) )
	{
		//unblock
		func.FunctionName = "ClickRemoveFromBlackList";
		RClickMenu->AddMenuItem( func , TEXT("RemoveBlack") );
	}
	else
	{
		//block chat
		func.FunctionName = "ClickMoveToBlackList";
		RClickMenu->AddMenuItem( func , TEXT("MoveToBlack") );
	}


	RClickMenu->AddLine();
}


void RClickFormatter_TeamNearby::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	//view profile
	//add as friend
	//request trade

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//guild category

	//general settings
	RClickMenu->AddLine();
}


void RClickFormatter_Friend::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);


	//add as friend

	if( FriendManager::getSingleton().IsMyFriend(RClickMenu->OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(RClickMenu->OwnerRoleId) )
	{   //unfollow
		func.FunctionName = "ClickDeleteFriend";
		RClickMenu->AddMenuItem( func , TEXT("DelAttention") );	
	}
	else
	{
		//add to follow
		func.FunctionName = "ClickAddFriend";
		RClickMenu->AddMenuItem( func , TEXT("AddAttention") );	
	}
	if(FriendManager::getSingleton().InMyEnemyList(RClickMenu->OwnerRoleId))
	{
		func.FunctionName = "ClickRemoveEnemy";
		RClickMenu->AddMenuItem( func , TEXT("RemoveEnemy") );	
	}
	else
	{
		func.FunctionName = "ClickAddEnemy";
		RClickMenu->AddMenuItem( func , TEXT("AddEnemy") );	
	}

	func.FunctionName = "ClickMoveFriend";
	RClickMenu->AddMenuItem( func , TEXT("MoveFriend") );
		
	

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//guild category
	RClick_Guild(RClickMenu, func);

	//general settings
	RClickMenu->AddLine();
	if( FriendManager::getSingleton().InMyBlackList(RClickMenu->OwnerRoleId) )
	{
		//unblock
		func.FunctionName = "ClickRemoveFromBlackList";
		RClickMenu->AddMenuItem( func , TEXT("RemoveBlack") );
	}
	else
	{
		//block chat
		func.FunctionName = "ClickMoveToBlackList";
		RClickMenu->AddMenuItem( func , TEXT("MoveToBlack") );
	}
	RClickMenu->AddLine();
}


void RClickFormatter_BattleMember::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	//view profile

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//guild category

	//general settings
	RClickMenu->AddLine();

}


void RClickFormatter_FriendGroup::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;

	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );


	//general settings
	RClickMenu->AddLine();
}


void RClickFormatter_Attention::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;

	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	//add as friend

	if( FriendManager::getSingleton().IsMyFriend(RClickMenu->OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(RClickMenu->OwnerRoleId) )
	{   //unfollow
		func.FunctionName = "ClickDeleteFriend";
		RClickMenu->AddMenuItem( func , TEXT("DelAttention") );	
	}
	else
	{
		//add to follow
		func.FunctionName = "ClickAddFriend";
		RClickMenu->AddMenuItem( func , TEXT("AddAttention") );	
	}
	if(FriendManager::getSingleton().InMyEnemyList(RClickMenu->OwnerRoleId))
	{
		func.FunctionName = "ClickRemoveEnemy";
		RClickMenu->AddMenuItem( func , TEXT("RemoveEnemy") );	
	}
	else
	{
		func.FunctionName = "ClickAddEnemy";
		RClickMenu->AddMenuItem( func , TEXT("AddEnemy") );	
	}
	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );


	//general settings

	RClickMenu->AddLine();
}

void RClickFormatter_Black::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );


	//general settings
	RClickMenu->AddLine();
}

void RClickFormatter_GuildMember::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	//guild category
	RClick_Guild(RClickMenu, func);

	//general settings
	RClickMenu->AddLine();
}

void RClickFormatter_Dungeon::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;

	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);

	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );


	//general settings
	RClickMenu->AddLine();
}


void RClickFormatter_RankListPlayer::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);


	if( FriendManager::getSingleton().IsMyFriend(RClickMenu->OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(RClickMenu->OwnerRoleId) )
	{   //unfollow
		func.FunctionName = "ClickDeleteFriend";
		RClickMenu->AddMenuItem( func , TEXT("DelAttention") );	
	}
	else
	{
		//add to follow
		func.FunctionName = "ClickAddFriend";
		RClickMenu->AddMenuItem( func , TEXT("AddAttention") );	
	}
	if(FriendManager::getSingleton().InMyEnemyList(RClickMenu->OwnerRoleId))
	{
		func.FunctionName = "ClickRemoveEnemy";
		RClickMenu->AddMenuItem( func , TEXT("RemoveEnemy") );	
	}
	else
	{
		func.FunctionName = "ClickAddEnemy";
		RClickMenu->AddMenuItem( func , TEXT("AddEnemy") );	
	}
	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	// guild category
	RClick_Guild(RClickMenu, func);
	
	RClickMenu->AddLine();
}

void RClickFormatter_RankListGuild::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//contact category
	RClick_Contant(RClickMenu , func);

	//cooperation category
	RClick_Cooperate(RClickMenu , func);


	// Achievement comparison
	func.FunctionName = "ClickAchievementCompare";
	RClickMenu->AddMenuItem( func, TEXT("CompareAchievement") );

	// guild category
	RClick_Guild(RClickMenu, func);

	RClickMenu->AddLine();
}

void RClickFormatter_GuildAlliance::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//cancel alliance
	func.FunctionName = "ClickCancelAlliance";
	RClickMenu->AddMenuItem(func , TEXT("CancelAlliance"));

	//copy guild name
	func.FunctionName = "ClickCopyGuildNameByPanel";
	RClickMenu->AddMenuItem(func, TEXT("CopyGuildName"));

	RClickMenu->AddLine();
}

void RClickFormatter_GuildEnemy::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{
	FScriptDelegate func(EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if (Player == NULL)return;

	//declare war
	UINT32 clientOption = CMessageHandler::GetInstance().GetClientOption();
	if (clientOption & CLIENT_OPTION_BIT_GUILD_WAR)
	{
		func.FunctionName = "ClickGuildWar";
		RClickMenu->AddMenuItem(func, TEXT("GuildWar"));
	}

	//copy guild name
	func.FunctionName = "ClickCopyGuildNameByPanel";
	RClickMenu->AddMenuItem(func, TEXT("CopyGuildName"));

	RClickMenu->AddLine();
}

void RClickFormatter_AuctionPlayer::FormatRClick(class UMMOUIRClickMenu* RClickMenu)
{	
	FScriptDelegate func( EC_EventParm);
	func.Object = RClickMenu;
	class AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player==NULL)return;

	//check self
	UBOOL res = RClick_CheckSelf(RClickMenu , func);
	if(res)return;


	//private chat
	func.FunctionName = "ClickEnterPrivateChat"; 
	RClickMenu->AddMenuItem( func , TEXT("PrivateChat") );	


	if( FriendManager::getSingleton().IsMyFriend(RClickMenu->OwnerRoleId) || FriendManager::getSingleton().IsMyAttention(RClickMenu->OwnerRoleId) )
	{   //unfollow
		func.FunctionName = "ClickDeleteFriend";
		RClickMenu->AddMenuItem( func , TEXT("DelAttention") );	
	}
	else
	{
		//add to follow
		func.FunctionName = "ClickAddFriend";
		RClickMenu->AddMenuItem( func , TEXT("AddAttention") );	
	}

	//copy name
	func.FunctionName = "ClickCopyName";
	RClickMenu->AddMenuItem( func , TEXT("CopyName") );

	// guild category
	// copy guild name
	func.FunctionName = "ClickCopyGuildName";
	RClickMenu->AddMenuItem(func, TEXT("CopyGuildName"));

	RClickMenu->AddLine();
}

