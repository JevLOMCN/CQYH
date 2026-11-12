#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include "MMOGuideManager.h"
#include "MMOMsgBoxManager.h"

#include <time.h>

IMPLEMENT_CLASS(UMMOUISabacEnrollViewPanel)

void UMMOUISabacEnrollViewPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUISabacEnrollViewPanel>(this, evts);
}

void UMMOUISabacEnrollViewPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacEnrollViewPanel")));

		UINT32 curSabacOccupyGuildId = MMOGuildData::getSingleton().GetSabacOccupyGuildId();
		if (curSabacOccupyGuildId > 0)
		{
			FString guildName = TEXT("Unknown guild");
			FString leaderName = TEXT("Unknown guild member");

			GuildQueryInfo info;
			if (MMOGuildData::getSingleton().GetGuildInfoById(curSabacOccupyGuildId, info))
			{
				guildName = info.Name;
				PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(info.LeaderId);
				if (playerInfo)
				{
					leaderName = playerInfo->_RoleName;
				}
			}
			FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacEnrollViewDescHasGuild")), *leaderName, *guildName);
			eventCallUpdateDesc(text);
		}
		else
		{
			eventCallUpdateDesc(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacEnrollViewDescNoGuild")));
		}

		_CurPageIndex = 1;
		UpdateSabacEnrollList();
	}
}

void UMMOUISabacEnrollViewPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

	if (gameEvent->GetEventType() != GameEvent_Guild)
	{
		return;
	}

	MMOGameEvent_Guild* guildEvent = (MMOGameEvent_Guild*)gameEvent;
	if (guildEvent == NULL)
	{
		return;
	}
	switch(guildEvent->_EventType)
	{
	case MMOGameEvent_Guild::EGET_UpdateSabacEnrollList:
		{
			_CurPageIndex = 1;
			UpdateSabacEnrollList();
		}
		break;
	}
}

void UMMOUISabacEnrollViewPanel::PageUp()
{
	if (_CurPageIndex > 1)
	{
		_CurPageIndex --;
		UpdateSabacEnrollList();
	}
}

void UMMOUISabacEnrollViewPanel::PageDown()
{
	GuildSabacEnrollList sabacEnrollList = MMOGuildData::getSingleton().GetSabacEnrollList();
	if (sabacEnrollList.size() >= MAX_VIEW_GUILD_LIST_COUNT)
	{
		_CurPageIndex ++;
		UpdateSabacEnrollList();
	}
}

void UMMOUISabacEnrollViewPanel::UpdateSabacEnrollList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildSabacEnrollList sabacEnrollList = MMOGuildData::getSingleton().GetSabacEnrollList();
	GuildSabacEnrollList::iterator iter = sabacEnrollList.begin();
	INT index = 0;
	for (; iter != sabacEnrollList.end(); ++ iter)
	{
		GuildQueryInfo info;
		if (!MMOGuildData::getSingleton().GetGuildInfoById(iter->dwGuildID, info))
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		object->SetString(TEXT("label"), info.Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetInt(TEXT("guild_id"), info.Id);
		object->SetString(TEXT("guild_name"), info.Name);
		object->SetInt(TEXT("guild_level"), info.Level);
		object->SetInt(TEXT("guild_member_count"), info.MemberCount);

		PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(info.LeaderId);
		if (playerInfo)
		{
			object->SetString(TEXT("guild_leader_name"), playerInfo->_RoleName);
		}
		else
		{
			object->SetString(TEXT("guild_leader_name"), *FString::Printf(TEXT("ID:%d"), info.LeaderId));
		}
		
		FString date = TEXT("");
		__time32_t localTime = info.CreateTime;
		struct tm *time = _localtime32(&localTime); 
		if (time)
		{
			date = info.CreateTime != 0 ? FString::Printf(TEXT("%d.%d.%d"), time->tm_year + 1900, time->tm_mon + 1, time->tm_mday) : TEXT("");
		}
		object->SetString(TEXT("guild_create_time"), date);

		// Siege time = registration time + 3 days
		localTime = iter->dwEnrollTime + (3 * 24 * 3600);
		time = _localtime32(&localTime); 
		date = info.CreateTime != 0 ? FString::Printf(TEXT("%d.%d.%d"), time->tm_year + 1900, time->tm_mon + 1, time->tm_mday) : TEXT("");
		object->SetString(TEXT("guild_start_time"), date);

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	eventCallUpdateSabacEnrollList(dataProvider, _CurPageIndex);
}

//////////////////////////////////////////////////////////////////////////

// Turn page
void UMMOUISabacEnrollViewPanel::ASEnrollPageUp()
{
//	PageUp();
}

void UMMOUISabacEnrollViewPanel::ASEnrollPageDown()
{
//	PageDown();
}