#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>


IMPLEMENT_CLASS(UMMOUIGuildWarRecordPanel)

void UMMOUIGuildWarRecordPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildWarRecordPanel>(this, evts);
}

void UMMOUIGuildWarRecordPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		_CurPageIndex = 0;
		_CurPageType = 0;
		eventCallUpdateGuildWarRecordPage(0, _CurPageIndex + 1);
		CMessageHandler::GetInstance().Send_QueryGuildWarRecord(_CurPageIndex);
	}
}

void UMMOUIGuildWarRecordPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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

	switch(guildEvent->_EventType)
	{
	case MMOGameEvent_Guild::EGET_UpdateGuildWarRecord:
		UpdateGuildWarRecordList();
		break;

	case MMOGameEvent_Guild::EGET_NoMoreGuildWarRecord:
		NoMoreGuildWarRecord();
		break;
	}
}

void UMMOUIGuildWarRecordPanel::UpdateGuildWarRecordList()
{
	UGFxObject* dataArray = GFXPlayer->CreateArray();
	if (!dataArray)
	{
		return;
	}
	
	GuildWarRecordList guildWarRecordList = MMOGuildData::getSingleton().GetGuildWarRecordList();
	GuildWarRecordList::reverse_iterator iter = guildWarRecordList.rbegin();
	INT index = 0;
	for (; iter != guildWarRecordList.rend(); ++iter)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		object->SetString(TEXT("label"), TEXT(""));
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetBool(TEXT("win"), iter->byWarRes == GUILD_WAR_DEFENDER_LOSE);

		__time32_t localTime = iter->dwBeginDayTime;
		struct tm *time = _localtime32(&localTime);
		FString warTime = TEXT("");
		if (time)
		{
			warTime = iter->dwBeginDayTime != 0 ? FString::Printf(TEXT("%d-%d"),
				time->tm_mon + 1, time->tm_mday) : 
				MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UnKnowTime"));
		}

		GuildQueryInfo info;
		FString warFrom = TEXT("");
		if (MMOGuildData::getSingleton().GetGuildInfoById(iter->dwIntruderID, info))
		{
			warFrom = info.Name;
		}

		FString warTo = TEXT("");
		if (MMOGuildData::getSingleton().GetGuildInfoById(iter->dwDefenderID, info))
		{
			warTo = info.Name;
		}

		FString content = TEXT("");
		if (iter->byWarRes == GUILD_WAR_DEFENDER_LOSE)
		{// Attacker succeeded
			content = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildWarRecordSuccess"))),
				*warTime, *warFrom, *warTo, *warFrom);
		} 
		else if (iter->byWarRes == GUILD_WAR_DEFENDER_WIN)
		{// Defender succeeded
			content = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildWarRecordFailed"))),
				*warTime, *warFrom, *warTo, *warTo);
		}
		object->SetString(TEXT("content"), content);

		dataArray->SetElementObject(index, object);
		index ++;
	}
	eventCallUpdateGuildWarRecordList(dataArray);

	if (_CurPageType == 1)
	{
		_CurPageIndex = _CurPageIndex - 1;
	}
	else if (_CurPageType == 2)
	{
		_CurPageIndex = _CurPageIndex + 1;
	}
	eventCallUpdateGuildWarRecordPage(0, _CurPageIndex + 1);
}

void UMMOUIGuildWarRecordPanel::NoMoreGuildWarRecord()
{
}

void UMMOUIGuildWarRecordPanel::ASPrevRecordPageClick()
{
	_CurPageType = 1;
	if (_CurPageIndex > 0)
	{
		CMessageHandler::GetInstance().Send_QueryGuildWarRecord(_CurPageIndex - 1);
	}
}

void UMMOUIGuildWarRecordPanel::ASNextRecordPageClick()
{
	_CurPageType = 2;
	CMessageHandler::GetInstance().Send_QueryGuildWarRecord(_CurPageIndex + 1);
}