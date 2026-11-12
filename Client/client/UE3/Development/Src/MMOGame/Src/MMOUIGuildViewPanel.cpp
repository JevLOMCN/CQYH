#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include "MMOGuideManager.h"
#include "MMOMsgBoxManager.h"
#include "client_option_def.h"
#include <time.h>


IMPLEMENT_CLASS(UMMOUIGuildViewPanel)

void UMMOUIGuildViewPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildViewPanel>(this, evts);
}

void UMMOUIGuildViewPanel::SetNpcCalled(UBOOL npcCalled)
{
	_IsNpcCalled = npcCalled;
}

void UMMOUIGuildViewPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
		UpdateGuildLeaveCD();

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowGuildViewPanel);
		MMOUI::PostGameEvent(&evtGuide);

		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ViewPanel")));

		UINT32 clientOption = CMessageHandler::GetInstance().GetClientOption();
		eventCallEnableGuildWar(clientOption & CLIENT_OPTION_BIT_GUILD_WAR);

		// Open the panel, request the first page of data
		_LockPage = FALSE;
		_CurPageIndex = 0;
		CMessageHandler::GetInstance().Send_ViewGuildList(GUILD_VIEW_DOWN, 0); 
	}
	else
	{
		_IsNpcCalled = TRUE;
	}
}

void UMMOUIGuildViewPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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
	case MMOGameEvent_Guild::EGET_UpdateGuildViewList:
		{
			UpdateGuildViewList(guildEvent->_ViewType, guildEvent->_ViewSize);
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildFindList:
		{
			MMOGameEvent_Guild::GuildFindInfo info = guildEvent->_FindInfo;
			UpdateGuildFindList(info.Id, 
				info.Name, 
				info.Level, 
				info.MemberCount, 
				info.LeaderName, 
				info.CreatorName, 
				info.CreateTime,
				info.Announce,
				info.ProtectionTime);
		}
		break;
	case MMOGameEvent_Guild::EGET_ShowGuildView:
		{

			ShowGuildViewPanel();
		}
		break;
	}
}

void UMMOUIGuildViewPanel::ShowGuildViewPanel()
{
	if (!IsVisible)
	{
		Show(TRUE);
	}
}

void UMMOUIGuildViewPanel::PageUp()
{
	if (_CurPageIndex > 1 && !_LockPage)
	{
		UINT32 firstId = MMOGuildData::getSingleton().GetCurListFirstId();
		if (firstId > 0)
		{
			CMessageHandler::GetInstance().Send_ViewGuildList(GUILD_VIEW_UP, firstId);
			_LockPage = TRUE;
		}
	}
}

void UMMOUIGuildViewPanel::PageDown()
{
	GuildViewList guildViewInfoList = MMOGuildData::getSingleton().GetGuildViewInfoList();
	if (guildViewInfoList.size() >= MAX_VIEW_GUILD_LIST_COUNT && !_LockPage)
	{
		UINT32 lastId = MMOGuildData::getSingleton().GetCurListLastId();
		if (lastId > 0)
		{
			CMessageHandler::GetInstance().Send_ViewGuildList(GUILD_VIEW_DOWN, lastId);
			_LockPage = TRUE;
		}
	}
}

void UMMOUIGuildViewPanel::UpdateGuildViewList(INT type, INT size)
{
	if (size == 0)
	{
		_LockPage = FALSE;
		return;
	}

	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildViewList guildViewInfoList = MMOGuildData::getSingleton().GetGuildViewInfoList();
	GuildViewList::iterator iter = guildViewInfoList.begin();
	INT index = 0;
	for (; iter != guildViewInfoList.end(); ++ iter)
	{
		GuildViewInfo info = *iter;
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		object->SetString(TEXT("label"), info.Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		// #21838
		object->SetInt(TEXT("guild_id"), /*info.Id*/GetIndexFromID(info.Id));

		if (info.ProtectionTime > 0)
		{
			object->SetInt(TEXT("icon"), 1);
			FString timeString = TEXT("");
			MMOFacade::getSingleton().GetTimeString(info.ProtectionTime, timeString);
			FString tipsString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildIconTips1")) + timeString;
			object->SetString(TEXT("tips"), tipsString);
		}
		else
		{
			object->SetInt(TEXT("icon"), 0);;
			object->SetString(TEXT("tips"), TEXT(""));
		}

		object->SetInt(TEXT("guild_level"), info.level);
		object->SetString(TEXT("guild_name"), info.Name);
		object->SetInt(TEXT("guild_level"), info.level);
		object->SetInt(TEXT("guild_member_count"), info.MemberCount);
		object->SetString(TEXT("guild_leader_name"), info.LeaderName);
		object->SetString(TEXT("guild_creator_name"), info.CreatorName);

		FString date = TEXT("");
		__time32_t localTime = info.CreateTime;
		struct tm *time = _localtime32(&localTime); 
		if (time)
		{
			date = info.CreateTime != 0 ? FString::Printf(TEXT("%d/%d/%d"), time->tm_year + 1900, time->tm_mon + 1, time->tm_mday) : TEXT("");
		}

		object->SetString(TEXT("guild_create_time"), date);

		if (info.Announce.IsEmpty())
		{
			object->SetString(TEXT("guild_announce"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildAnnounceEmpty")));
		}
		else
		{
			object->SetString(TEXT("guild_announce"), info.Announce);
		}

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	if (type == GUILD_VIEW_UP)
	{
		_CurPageIndex -= (_CurPageIndex > 1) ? 1 : 0;
	}
	else
	{
		_CurPageIndex ++;
	}
	_LockPage = FALSE;
	eventCallUpdateGuildViewList(dataProvider, _CurPageIndex);
}

void UMMOUIGuildViewPanel::UpdateGuildViewInfo(INT id)
{
	GuildViewInfo info;
	if (!MMOGuildData::getSingleton().GetGuildViewInfo(id, info))
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (object)
	{
		object->SetInt(TEXT("guild_id"), /*info.Id*/GetIndexFromID(info.Id));
		object->SetString(TEXT("guild_name"), info.Name);
		object->SetInt(TEXT("guild_level"), info.level);
		object->SetInt(TEXT("guild_member_count"), info.MemberCount);
		object->SetString(TEXT("guild_leader_name"), info.LeaderName);
		object->SetString(TEXT("guild_creator_name"), info.CreatorName);

		FString date = TEXT("");
		__time32_t localTime = info.CreateTime;
		struct tm *time = _localtime32(&localTime); 
		if (time)
		{
			date = info.CreateTime != 0 ? FString::Printf(TEXT("%d/%d/%d"), time->tm_year + 1900, time->tm_mon + 1, time->tm_mday) : TEXT("");
		}

		object->SetString(TEXT("guild_create_time"), date);

		if (info.Announce.IsEmpty())
		{
			object->SetString(TEXT("guild_announce"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildAnnounceEmpty")));
		}
		else
		{
			object->SetString(TEXT("guild_announce"), info.Announce);
		}

		eventCallUpdateGuildViewInfo(object);
	}
}

void UMMOUIGuildViewPanel::UpdateGuildBuildingList(INT id)
{
	GuildViewInfo info;
	if (!MMOGuildData::getSingleton().GetGuildViewInfo(id, info))
	{
		return;
	}

	UGFxObject* objectArray = GFXPlayer->CreateArray();
	if (!objectArray)
	{
		return;
	}

	INT index = 0;
	for (INT i = GUILD_B_MAIN; i < GUILD_B_NUM; ++ i)
	{
		if (i == GUILD_B_GATE ||
			i == GUILD_B_WALL ||
			i == GUILD_B_BLOCK)
		{
			continue;
		}

		UINT32 buildingType = i;
		UINT32 buildingLv = info.BuildingLevel[i];
		GuildBuildingData buildingData;
		if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, buildingLv > 0 ? buildingLv : 1, buildingData))
		{
			UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
			if (object)
			{
				object->SetInt(TEXT("guild_building_type"), buildingType);
				object->SetString(TEXT("guild_building_name"), buildingData.Name);
				FString buildingIcon = FString::Printf(TEXT("img://UI_GuildTerritoryWidget.%d"), buildingType);
				object->SetString(TEXT("guild_building_icon"), buildingIcon);
				object->SetInt(TEXT("guild_building_level"), buildingLv);

				objectArray->SetElementObject(index, object);
				index ++;
			}
		}
	}

	eventCallUpdateGuildBuildingList(objectArray);
}

void UMMOUIGuildViewPanel::FindGuild(UBOOL isId, const FString& content)
{
	if (isId)
	{
		if (content.IsEmpty())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.EnterId"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.EnterId"));
			return;
		}

		UINT32 guildId = MakeObjectID(otGuild, appAtoi(*content));
		CMessageHandler::GetInstance().Send_FindGuild(guildId, TEXT(""));
	}
	else
	{
		if (content.IsEmpty())
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.EnterName"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.EnterName"));
			return;
		}

		FTCHARToUTF8 szGuildName(*content);
		// Up to 8 Chinese characters (Length = 3 * 8 chars)
		if (szGuildName.Length() > 24)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LongName"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.LongName"));
			return;
		}

		CMessageHandler::GetInstance().Send_FindGuild(0, content);
	}
}

void UMMOUIGuildViewPanel::UpdateGuildFindList(INT id, 
	const FString& guildName, 
	INT level, 
	INT memberCount,
	const FString& leaderName,
	const FString& creatorName,
	INT createTime, 
	const FString& announce,
	INT protectionTime)
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if (dataProvider)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetString(TEXT("label"), guildName);
			object->SetInt(TEXT("index"), 0);
			object->SetInt(TEXT("type"), 0);
			object->SetInt(TEXT("deepth"), 0);
			object->SetBool(TEXT("visible"), true);

			object->SetInt(TEXT("guild_id"), /*id*/GetIndexFromID(id));

			if (protectionTime > 0)
			{
				object->SetInt(TEXT("icon"), 1);
				FString timeString = TEXT("");
				MMOFacade::getSingleton().GetTimeString(protectionTime, timeString);
				FString tipsString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildIconTips1")) + timeString;
				object->SetString(TEXT("tips"), tipsString);
			}
			else
			{
				object->SetInt(TEXT("icon"), 0);
				object->SetString(TEXT("tips"), TEXT(""));
			}

			object->SetString(TEXT("guild_name"), guildName);
			object->SetInt(TEXT("guild_level"), level);
			object->SetInt(TEXT("guild_member_count"), memberCount);
			object->SetString(TEXT("guild_leader_name"), leaderName);
			object->SetString(TEXT("guild_creator_name"), creatorName);

			FString date = TEXT("");
			__time32_t localTime = createTime;
			struct tm *time = _localtime32(&localTime); 
			if (time)
			{
				date = createTime != 0 ? FString::Printf(TEXT("%d/%d/%d"), time->tm_year + 1900, time->tm_mon + 1, time->tm_mday) : TEXT("");
			}
			object->SetString(TEXT("guild_create_time"), date);

			if (announce.IsEmpty())
			{
				object->SetString(TEXT("guild_announce"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildAnnounceEmpty")));
			}
			else
			{
				object->SetString(TEXT("guild_announce"), announce);
			}

			dataProvider->SetElementObject(0, object);
		}
		eventCallUpdateGuildViewList(dataProvider, _CurPageIndex);
	}
}

void UMMOUIGuildViewPanel::ApplyForJoint(INT id)
{
	if (JoinConfirmBox)
	{
		return;
	}

	GuildViewInfo info;
	if (!MMOGuildData::getSingleton().GetGuildViewInfo(id, info))
	{
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeJoinNow");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseJoinNow");	

	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ApplyForJointConfirm")), *(info.Name));
	JoinConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, id, funYes, funNo);
}

void UMMOUIGuildViewPanel::AgreeJoinNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	INT id = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_ApplyToJoinGuild(id, TEXT(""));

	JoinConfirmBox = NULL;
}

void UMMOUIGuildViewPanel::RefuseJoinNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	JoinConfirmBox = NULL;
}
//////////////////////////////////////////////////////////////////////////

// Apply to join guild
void UMMOUIGuildViewPanel::ASApplyForJoint(INT id)
{
	UINT32 guildId = MakeObjectID(otGuild, id);
	ApplyForJoint(guildId);
}

void UMMOUIGuildViewPanel::ASUpdateGuildViewInfo(INT id)
{
	UINT32 guildId = MakeObjectID(otGuild, id);
	UpdateGuildViewInfo(guildId);
	UpdateGuildBuildingList(guildId);
}

// Find guild
void UMMOUIGuildViewPanel::ASFindGuild(UBOOL isId, const FString& content)
{
	FindGuild(isId, content);
}

// Turn page
void UMMOUIGuildViewPanel::ASPageUp()
{
	PageUp();
}

void UMMOUIGuildViewPanel::ASPageDown()
{
	PageDown();
}

void UMMOUIGuildViewPanel::ASChallengeConfirm(INT id)
{
	UINT32 guildId = MakeObjectID(otGuild, id);
	GuildViewInfo info;
	if (!MMOGuildData::getSingleton().GetGuildViewInfo(guildId, info))
	{
		return;
	}

	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	UINT32 buildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(GUILD_B_MAIN);
	GuildBuildingData buildingData;
	if (!MMOGuildData::getSingleton().GetGuildBuildingData(GUILD_B_MAIN, buildingLv, buildingData))
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Guild.GuildActiveItemData"));
	if (!object)
	{
		return;
	}

	object->SetInt(TEXT("ID"), info.Id);

	FString text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), buildingData.Param1);
	FString text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), buildingData.Param1);
	object->SetString(TEXT("money"), baseInfo.Money >= buildingData.Param1 ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), buildingData.Param2);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), buildingData.Param2);
	object->SetString(TEXT("herb"), baseInfo.Herb >= buildingData.Param2 ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), buildingData.Param3);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), buildingData.Param3);
	object->SetString(TEXT("wood"), baseInfo.Wood >= buildingData.Param3 ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), buildingData.Param4);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), buildingData.Param4);
	object->SetString(TEXT("crystal"), baseInfo.Crystal >= buildingData.Param4 ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), buildingData.Param5);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), buildingData.Param5);
	object->SetString(TEXT("silver"), baseInfo.Silver >= buildingData.Param5 ? text1 : text2);

	object->SetBool(TEXT("enough"),
		baseInfo.Money >= buildingData.Param1 &&
		baseInfo.Herb >= buildingData.Param2 &&
		baseInfo.Wood >= buildingData.Param3 &&
		baseInfo.Crystal >= buildingData.Param4 &&
		baseInfo.Silver >= buildingData.Param5);

	eventCallShowChallengeConfirm(object);
}

void UMMOUIGuildViewPanel::ASGuildWarSumitOpen(INT id)
{
	GuildViewInfo info;
	if (!MMOGuildData::getSingleton().GetGuildViewInfo(id, info))
	{
		return;
	}

	if (info.ProtectionTime > 0)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ChallengeProtection"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.ChallengeProtection"));
		return;
	}

	if (!MMOGuildData::getSingleton().IsGuildEnemy(info.Id))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ChallengeNotEnemy"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.ChallengeNotEnemy"));
		return;
	}

	CMessageHandler::GetInstance().Send_Challenge(info.Name);
}

void UMMOUIGuildViewPanel::UpdateGuildLeaveCD()
{
	FString cdStr = TEXT("");
	INT cd = MMOGuildManager::getSingleton().GetGuildLeaveCD();
	MMOFacade::getSingleton().GetTimeString(cd, cdStr);
	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.JointCD")), *cdStr);

	if (cd > 0)
	{
		eventCallUpdateGuildLeaveCD(text);
	} 
	else
	{
		eventCallUpdateGuildLeaveCD(TEXT(""));
	}
}

