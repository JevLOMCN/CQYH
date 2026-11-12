#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "TeamManager.h"
#include "FriendManager.h"
#include "MMOGuildManager.h"
#include "MMOGuildData.h"

IMPLEMENT_CLASS(UMMOUITeamInvitePanel)

void UMMOUITeamInvitePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		if(CurTab <= 0)
		{
			UpdateFriendsList();
		}
		else if(CurTab == 1)
		{
			UpdateRecentsList();
		}
		else if(CurTab == 2)
		{
			UpdateGuildsList();
		}
	}
	else
	{
		TeamManager::getSingleton().ClearInvitePlayers();
	}
}

void UMMOUITeamInvitePanel::UpdateFriendsList()
{
	CurTab = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(!dataProvider)
		return;
	INT idx = 0;
	const TArray<MMOFriendGroup>& friendGroups = FriendManager::getSingleton().GetAllFriendList();
	for(INT i = 0; i < friendGroups.Num(); ++i)
	{
		const MMOFriendGroup& group = friendGroups(i);
		for(INT j = 0; j < group.FriendList.Num(); ++j)
		{
			//Do not show offline ones
			if(group.FriendList(j).Online == FS_OFFLINE)
				continue;

			UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
			data->SetBool(TEXT("PlayerSelected"), FALSE);
			data->SetString(TEXT("Name"), group.FriendList(j).ActorName);
			data->SetInt(TEXT("Type"),0);
			data->SetString(TEXT("ClassName"), MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(group.FriendList(j).ClassId)));
			//data->SetInt(TEXT("Level"),group.FriendList(j).Lv);
			data->SetString(TEXT("Position"),TEXT(""));
			data->SetInt(TEXT("playerId"), group.FriendList(j).RoleId);
			dataProvider->SetElementObject(idx++, data);
		}
	}

	eventUpdateInvitePlayerList(dataProvider);
}

void UMMOUITeamInvitePanel::UpdateRecentsList()
{
	CurTab = 1;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(!dataProvider)
		return;

	//TODO:
	INT idx = 0;
	TArray<MMOInvitedPlayer>& InviteHistoryList = TeamManager::getSingleton().GetInvitePlayerListHistory();
	for(INT i = 0; i < InviteHistoryList.Num(); ++i)
	{
		UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
		data->SetBool(TEXT("PlayerSelected"), FALSE);
		data->SetString(TEXT("Name"), InviteHistoryList(i).ActorName);
		data->SetInt(TEXT("Type"),-1);
		data->SetString(TEXT("ClassName"), MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(InviteHistoryList(i).Job)));
		//data->SetInt(TEXT("Level"),InviteHistoryList(i).Level);
		data->SetString(TEXT("Position"),TEXT(""));
		data->SetInt(TEXT("playerId"), InviteHistoryList(i).ActorServerId);
		dataProvider->SetElementObject(idx++, data);
	}
	eventUpdateInvitePlayerList(dataProvider);
}

void UMMOUITeamInvitePanel::UpdateGuildsList()
{
	CurTab = 2;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(!dataProvider)
		return;
	GuildFullMemberInfoList guidMemberList = MMOGuildData::getSingleton().GetGuildFullMemberInfoList();
	INT idx = 0;
	GuildFullMemberInfoList::iterator iter = guidMemberList.begin();
	for (; iter != guidMemberList.end(); ++ iter )
	{
		if (iter->LastLoginTime == 0)
		{
			UGFxObject* data = GFXPlayer->CreateObject(TEXT("Object"));
			data->SetBool(TEXT("PlayerSelected"), FALSE);
			data->SetString(TEXT("Name"), iter->Name);
			data->SetInt(TEXT("Type"),0);
			data->SetString(TEXT("ClassName"), MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(iter->Job)));
			//data->SetInt(TEXT("Level"),iter->Level);
			data->SetString(TEXT("Position"),TEXT(""));
			data->SetInt(TEXT("playerId"), iter->Id);
			dataProvider->SetElementObject(idx++, data);
		}
	}

	eventUpdateInvitePlayerList(dataProvider);
}

void UMMOUITeamInvitePanel::ASShowInviteFriendList()
{
	UpdateFriendsList();

}

void UMMOUITeamInvitePanel::ASShowInviteRecentList()
{
	UpdateRecentsList();
}

void UMMOUITeamInvitePanel::ASShowInviteGuiderList()
{
	UpdateGuildsList();
}

void UMMOUITeamInvitePanel::ASClickInviteOk()
{
	TeamManager::getSingleton().InvitePlayers();
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamInvite, FALSE, "");
}

void UMMOUITeamInvitePanel::ASClickInviteCancel()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_TeamInvite, FALSE, "");
}