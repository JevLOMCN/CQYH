#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>


IMPLEMENT_CLASS(UMMOUIGuildWarStartPanel)

void UMMOUIGuildWarStartPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildWarStartPanel>(this, evts);
}

void UMMOUIGuildWarStartPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuideWar")));
	}
}

void UMMOUIGuildWarStartPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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
	if (!guildEvent)
	{
		return;
	}
}

void UMMOUIGuildWarStartPanel::ShowGuildWarStartPanel(INT targetGuildId)
{
	Show(TRUE);

	UINT32 attackId = MMOGuildData::getSingleton().GetMyGuildWarAttackId();
	UINT32 defendId = MMOGuildData::getSingleton().GetMyGuildWarDefendId();
	
	FString icon = TEXT("img://UI_GuildTerritoryWidget.building4");
	FString text = TEXT("");
	if (targetGuildId == attackId)
	{
		FString guildName = FString::Printf(TEXT("Guild ID:%d "), attackId);
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(attackId, info))
		{
			guildName = info.Name;
		}
		text = FString::Printf(TEXT("Our guild has declared war on guild <font color='#FF0000'>%s</font>"), *guildName);
	}
	if (targetGuildId == defendId)
	{
		FString guildName = FString::Printf(TEXT("Guild ID:%d "), defendId);
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(defendId, info))
		{
			guildName = info.Name;
		}
		text = FString::Printf(TEXT("Guild <font color='#FF0000'>%s</font> has declared war on our guild"), *guildName);
	}
	eventCallUpdateGuildWarStartInfo(icon, text);
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildWarStartPanel::ASJoinWar()
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id > 0)
	{
		CMessageHandler::GetInstance().Send_EnterGuildMap(GUILD_ENTER_OWNER, baseInfo.Id);
	}
}