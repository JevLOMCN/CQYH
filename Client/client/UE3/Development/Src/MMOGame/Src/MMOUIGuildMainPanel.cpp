#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include <time.h>
#include "MMOMsgButtonManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuideManager.h"
#include "UITextParser.h"
#include "client_option_def.h"

IMPLEMENT_CLASS(UMMOUIGuildMainPanel)

void UMMOUIGuildMainPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildMainPanel>(this, evts);
}

void UMMOUIGuildMainPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if (v)
	{
		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ShowGuildPanel);
		MMOUI::PostGameEvent(&evtGuide);

		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		if (baseInfo.Id == 0)
		{
			// If there is no guild
			eventCallUpdateGuildIntroduce(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.NoGuild")));
			eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Guide")));
			eventCallInitGuild(FALSE);
			return;
		}

		eventCallSetWindowTitle(baseInfo.Name);
		UpdateGuildBaseInfo();
		
		UpdateGuildNotice(baseInfo.Notice);

		// When coming online only a query may have been sent, fetch the name again here
		MMOGuildManager::getSingleton().UpdateGuildEvents();
		MMOGuildManager::getSingleton().UpdateGuildMoneyEvents();

		MMOGuildData::getSingleton().SortGuildFullMember(FMST_TIME, TRUE);
		UpdateGuildFullMemberList();
		RequestGuildPendingMemberList();

		AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
		if (mainActor)
		{
			UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID);
			UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);
			if ((rights & G_POS_RIGHT_DIPLOMAT) != 0)
			{
				CMessageHandler::GetInstance().Send_GuildGetApplyUnionList();
			}

			UpdateGuildRights(post);
			UpdateGuildStorageRights(post);
		}
		UpdateGuildStorageState();

		UINT32 playerMoney = 0;
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (mainPlayer)
		{
			playerMoney = mainPlayer->GetMoney(MONEYTYPE_02);
		}
		UpdatePlayerMoney(playerMoney);
		UpdateGuildSalary();
		UpdateGuildWarInfo();

		CMessageHandler::GetInstance().Send_MoreGuildInfo();
		UpdateGuildWarSearchState(FALSE);

		UpdateGuildAllyList();
		UpdateGuildEnemyList();
		eventCallInitGuild(TRUE);

		_WarTargetName = TEXT("");

		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			uiManager->delegateUpdateMenuEffect(4, EMenuType_Child_Guild, FALSE, FALSE);
		}

		// guild activity
		UpdateGuildActivityList();

		eventCallShowGuildMainPage(_ShowPage, _ShowSubPage);

		UINT32 clientOption = CMessageHandler::GetInstance().GetClientOption();
		eventCallEnableGuildWar(clientOption & CLIENT_OPTION_BIT_GUILD_WAR);
	}
	else
	{
		MMOUI::GetUIManager()->delegateCloseRClickMenu(ERCMType_GuildAlliance);
		MMOUI::GetUIManager()->delegateCloseRClickMenu(ERCMType_GuildEnemy);
		MMOUI::GetUIManager()->delegateCloseRClickMenu(ERCMType_GuildMember);

	}
}

void UMMOUIGuildMainPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
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
	case MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo:
		{
			UpdateGuildBaseInfo();
			UpdateGuildStorageState();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildNotice:
		{
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			if (baseInfo.Id != 0)
			{
				UpdateGuildNotice(baseInfo.Notice);
			}
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildEvents:
		{
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			if (baseInfo.Id != 0)
			{
				UpdateGuildEvents(baseInfo.Events);
			}
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildMoneyEvents:
		{
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			if (baseInfo.Id != 0)
			{
				UpdateGuildMoneyEvents(baseInfo.MoneyEvents);
			}
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList:
		{
			UpdateGuildFullMemberList();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildPendingMemberList:
		{
			UpdateGuildPendingMemberList();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildStorageRights:
		{
			UpdateGuildStorageRights(guildEvent->_CurPost);
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildSalary:
		{
			UpdateGuildSalary();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildAllyList:
		{
			UpdateGuildAllyList();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildEnemyList:
		{
			UpdateGuildEnemyList();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildApplyAllyList:
		{
			UpdateGuildApplyAllyList();
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildWarCD:
		{
			UpdateGuildWarCD();
		}
		break;
	case MMOGameEvent_Guild::EGET_BeginSearch:
		{
			UpdateGuildWarSearchState(TRUE);
		}
		break;
	case MMOGameEvent_Guild::EGET_CompleteSearch:
		{
			UpdateGuildWarSearchState(FALSE);
		}
		break;
	case MMOGameEvent_Guild::EGET_UpdateGuildActivity:
		{
			UpdateGuildActivityList();
		}
		break;
	}
}

void UMMOUIGuildMainPanel::UpdateGuildFullMemberList()
{
	//SCOPE_CYCLE_COUNTER(STAT_GuildTime);
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildFullMemberInfoList guildFullMemberInfoList = MMOGuildData::getSingleton().GetGuildFullMemberInfoList();
	GuildFullMemberInfoList::iterator iter = guildFullMemberInfoList.begin();
	INT index = 0;
	for (; iter != guildFullMemberInfoList.end(); ++ iter)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		object->SetString(TEXT("label"), iter->Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);

		if (IsShowOnlineMember && iter->LastLoginTime != 0)
		{
			continue;
		}
		
		object->SetBool(TEXT("visible"), true);
		object->SetInt(TEXT("member_id"), iter->Id);
		FString name = iter->IsForbitChat ? (iter->Name + MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ForbidChat"))) : iter->Name;
		object->SetString(TEXT("member_name"), name);
		object->SetInt(TEXT("member_level"), iter->Level);

		FString jobName = TEXT("");
		MMOGuildData::getSingleton().GetJobName(iter->Job, jobName);
		object->SetString(TEXT("member_job"), jobName);

		FString postName = TEXT("");
		MMOGuildData::getSingleton().GetGuildPostName(iter->Post, postName);
		object->SetString(TEXT("member_post"), postName);

		FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo(iter->SceneID);
		// #23816
		if (mapInfo.MapDisplayName == TEXT("N/A"))
		{
			mapInfo.MapDisplayName = TEXT("-");
		}
		object->SetString(TEXT("member_map"), mapInfo.MapDisplayName);

		FString timeString = TEXT("");
		MMOFacade::getSingleton().GetDurationTimeString(iter->LastLoginTime, timeString);
		FString date = iter->LastLoginTime != 0 ? timeString : MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.FullMemberOnLine"));
		object->SetString(TEXT("member_time"), date);
		object->SetBool(TEXT("member_online"), iter->LastLoginTime == 0);

		AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
		object->SetBool(TEXT("member_self"), (mainActor && iter->Id == mainActor->ServerID));

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	eventCallUpdateGuildFullMemberList(dataProvider);
}

void UMMOUIGuildMainPanel::ShowOnlineMember(UBOOL isShow)
{
	IsShowOnlineMember = isShow;
	UpdateGuildFullMemberList();
}

void UMMOUIGuildMainPanel::FullMemberRightClick(INT posX, INT posY, INT roleId)
{
	MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_GuildMember, posX, posY, roleId, TEXT(""), 0);
}

void UMMOUIGuildMainPanel::AllianceRightClick(INT posX, INT posY, INT guildId)
{
	FString guildName = TEXT("");
	GuildQueryInfo info;
	if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
	{
		guildName = info.Name;
	}
	MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_GuildAlliance, posX, posY, guildId, guildName, 0);
}

void UMMOUIGuildMainPanel::EnemyRightClick(INT posX, INT posY, INT guildId)
{
	FString guildName = TEXT("");
	GuildQueryInfo info;
	if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
	{
		guildName = info.Name;
	}
	MMOUI::GetUIManager()->delegateOpenRClickMenu(ERCMType_GuildEnemy, posX, posY, guildId, guildName, 0);
}

void UMMOUIGuildMainPanel::SetForeignApplyDayType(INT foreignType, INT dayType)
{
	UINT32 money = 0;
	switch (dayType)
	{
	case 0 :
		money = (foreignType == GR_FRIEND) ? GFM_DAY_1 : GFM_DAY_1;
		break;
	case 1 :
		money = (foreignType == GR_FRIEND) ? GFM_DAY_3 : GFM_DAY_3;
		break;
	case 2 :
		money = (foreignType == GR_FRIEND) ? GFM_DAY_7 : GFM_DAY_7;
		break;
	case 3 :
		money = (foreignType == GR_FRIEND) ? GFM_DAY_30 : GFM_DAY_30;
		break;
	}

	eventCallUpdateForeignApplyConsume(FString::Printf(TEXT("Guild funds: %d"), money));
}

void UMMOUIGuildMainPanel::RequestGuildPendingMemberList()
{
	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if (mainActor)
	{
		UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID);
		UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);
		if ((rights & G_POS_RIGHT_RECRUIT) != 0)
		{
			CMessageHandler::GetInstance().Send_GetApplyGuildList();
		}
	}
}

void UMMOUIGuildMainPanel::UpdateGuildPendingMemberList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildPendingMemberInfoList guildPendingMemberInfoList = MMOGuildData::getSingleton().GetGuildPendingMemberInfoList();
	GuildPendingMemberInfoList::iterator iter = guildPendingMemberInfoList.begin();
	INT index = 0;
	for (; iter != guildPendingMemberInfoList.end(); ++ iter)
	{
		GuildPendingMemberInfo info = *iter;
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		object->SetString(TEXT("label"), info.Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetInt(TEXT("member_id"), iter->Id);
		object->SetString(TEXT("member_name"), iter->Name);
		object->SetInt(TEXT("member_level"), iter->Level);

		FString jobName = TEXT("");
		MMOGuildData::getSingleton().GetJobName(iter->Job, jobName);
		object->SetString(TEXT("member_job"), jobName);

		FString timeString = TEXT("");
		MMOFacade::getSingleton().GetDurationTimeString(iter->ApplyTime, timeString);
		FString date = iter->ApplyTime != 0 ? timeString : TEXT("");
		object->SetString(TEXT("member_time"), date);

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	eventCallUpdateGuildPendingMemberList(dataProvider);
}

void UMMOUIGuildMainPanel::AgreeToJoin(INT id)
{
	CMessageHandler::GetInstance().Send_GuildApplyResponse(GUILD_REVERT_AGREE, id);
}

void UMMOUIGuildMainPanel::RefuseToJoin(INT id)
{
	CMessageHandler::GetInstance().Send_GuildApplyResponse(GUILD_REVERT_REFUSE, id);
}


void UMMOUIGuildMainPanel::UpdateGuildBaseInfo()
{
	if(IsVisible==FALSE)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if (!mainActor)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject("Object");
	if (!object)
	{
		return;
	}

	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	object->SetInt(TEXT("guild_id"), /*baseInfo.Id*/GetIndexFromID(baseInfo.Id));
	object->SetString(TEXT("guild_name"), baseInfo.Name);
	object->SetInt(TEXT("guild_level"), baseInfo.level);
	object->SetInt(TEXT("guild_member_count"), baseInfo.MemberCount);
	object->SetInt(TEXT("guild_max_member_count"), MMOGuildData::getSingleton().GetCurGuildMemberMax());
	object->SetInt(TEXT("guild_money"), baseInfo.Money);

	UINT32 moneyMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_MONEY);
	FString moneyTips = FString::Printf(TEXT("%d/%d"), baseInfo.Money, moneyMax);
	moneyTips += TEXT("\n");
	moneyTips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildMoneyTips"));
	object->SetString(TEXT("guild_money_tips"), moneyTips);
	object->SetInt(TEXT("guild_salary_total"), 0);

	object->SetString(TEXT("guild_leader_name"), baseInfo.LeaderName);
	object->SetString(TEXT("guild_creator_name"), baseInfo.CreatorName);
	object->SetString(TEXT("guild_announce"), baseInfo.Announce);

	FString date = TEXT("");
	__time32_t localTime = baseInfo.CreateTime;
	struct tm *time = _localtime32(&localTime); 
	if (time)
	{
		date = baseInfo.CreateTime != 0 ? FString::Printf(TEXT("%d/%d/%d"), time->tm_year + 1900, time->tm_mon + 1, time->tm_mday) : TEXT("");
	}
	object->SetString(TEXT("guild_create_time"), date);

	UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID);
	FString postName = TEXT("");
	MMOGuildData::getSingleton().GetGuildPostName(post, postName);
	object->SetInt(TEXT("guild_post"), post);
	object->SetString(TEXT("guild_post_name"), postName);

	object->SetInt(TEXT("guild_contribute"), mainPlayer->GetMoney(MONEYTYPE_GUILD));

	UINT32 selfSalary = MMOGuildData::getSingleton().GetGuildFullMemberSalary(mainActor->ServerID);
	object->SetInt(TEXT("guild_achievement"), selfSalary);	// merit

	// Own permissions
	UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);
	object->SetBool(TEXT("guild_rights_leader"), MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID) == GUILD_POSITION_LEADER);
	object->SetBool(TEXT("guild_rights_invite"), (rights & G_POS_RIGHT_INVITE) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_recruit"), (rights & G_POS_RIGHT_RECRUIT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_forbid_chat"), (rights & G_POS_RIGHT_FORBID_CHAT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_notice"), (rights & G_POS_RIGHT_NOTICE) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_storage_right"), (rights & G_POS_RIGHT_SET_STORAGE_RIGHT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_dungeon"), (rights & G_POS_RIGHT_OPEN_DUNGEON) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_upgrade_building"), (rights & G_POS_RIGHT_UPGRADE_BUILDING) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_upgrade_skill"), (rights & G_POS_RIGHT_UPGRADE_WELFARE) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_change_pos"), (rights & G_POS_RIGHT_CHANGE_POS) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_banish"), (rights & G_POS_RIGHT_BANISH) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_foreign"), (rights & G_POS_RIGHT_DIPLOMAT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_activity"), (rights & G_POS_RIGHT_ACTIVITY) > 0 ? true : false);

	eventCallUpdateGuildBaseInfo(object);
}

void UMMOUIGuildMainPanel::ChangeGuildNotice(const FString& notice)
{
	if (MMOUI::HasCensoredWord(notice))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SpecialNotice"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.SpecialNotice"));
		return;
	}

	FTCHARToUTF8 szNotice(*notice);
	// Up to 60 Chinese characters (Length = 3 * 60 chars)
	if (szNotice.Length() > 180)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LongNotice"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.LongNotice"));
		return;
	}

	CMessageHandler::GetInstance().Send_GuildNoticeChange(notice);
}

void UMMOUIGuildMainPanel::UpdateGuildNotice(const FString& notice)
{
	eventCallUpdateGuildNotice(notice);
}

void UMMOUIGuildMainPanel::ViewMoreEvents()
{
	CMessageHandler::GetInstance().Send_ViewMoreEvents();
}

void UMMOUIGuildMainPanel::UpdateGuildEvents(const FString& events)
{
	eventCallUpdateGuildEvents(events);
}

void UMMOUIGuildMainPanel::UpdateGuildMoneyEvents(const FString& events)
{
	eventCallUpdateGuildMoneyEvents(events);
}

void UMMOUIGuildMainPanel::InviteMember(const FString& roleName)
{
	if (roleName.IsEmpty())
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.EnterRoleName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.EnterRoleName"));
		return;
	}

	FTCHARToUTF8 szRoleName(*roleName);
	// Up to 8 Chinese characters (Length = 3 * 8 chars)
	if (szRoleName.Length() > 24)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LongRoleName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.LongRoleName"));
		return;
	}

	if (MMOUI::HasCensoredWord(roleName))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SpecialRoleName"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.SpecialRoleName"));
		return;
	}

	CMessageHandler::GetInstance().Send_InviteToJoinGuild(roleName);
}

void UMMOUIGuildMainPanel::ExpelMember(INT roleId)
{
	ExpelConfirm(roleId);
}

void UMMOUIGuildMainPanel::SortMember(INT type, UBOOL isUp)
{
	MMOGuildManager::getSingleton().SortGuildFullMember(type, isUp);
}

void UMMOUIGuildMainPanel::ExpelConfirm(INT roleId)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && mainPlayer->ServerID == roleId)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ExpleSelf"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.ExpleSelf"));
		return;
	}

	// Cannot kick members during guild war
	if (MMOGuildManager::getSingleton().IsGuildWarStart())
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildWarCanNotBanish"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.GuildWarCanNotBanish"));
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ReplyAgreeExpel");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("ReplyRefuseExpel");	

	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.BanishAsk"));
	ExpelConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, roleId, funYes, funNo);
}

void UMMOUIGuildMainPanel::ReplyAgreeExpel(UGFxObject *mc)
{
	if(mc == NULL)
	{
		return;
	}

	int roleId = mc->GetInt(TEXT("iteminfoid"));
	CMessageHandler::GetInstance().Send_GuildBanishRole(roleId);

	//del confirm box
	ExpelConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::ReplyRefuseExpel(UGFxObject *mc)
{
	if(mc == NULL)
	{
		return;
	}

	//del confirm box
	ExpelConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::QuitConfirm()
{
	// Guild leader cannot leave
	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if (mainActor)
	{
		UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID);
		if (post == GUILD_POSITION_LEADER)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LeaderQuit"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.LeaderQuit"));
			return;
		}
	}

	// Cannot leave during guild war
	if (MMOGuildManager::getSingleton().IsGuildWarStart())
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildWarCanNotLeave"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.GuildWarCanNotLeave"));
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ReplyAgreeQuit");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("ReplyRefuseQuit");	

	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.QuitAsk"));
	QuitConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, 0, funYes, funNo);
}

void UMMOUIGuildMainPanel::ReplyAgreeQuit(UGFxObject *mc)
{
	if(mc == NULL)
	{
		return;
	}

	CMessageHandler::GetInstance().Send_GuildRoleLeave();

	//del confirm box
	QuitConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::ReplyRefuseQuit(UGFxObject *mc)
{
	if(mc == NULL)
	{
		return;
	}

	//del confirm box
	QuitConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::ChangePost(INT roleId, INT post)
{
	CMessageHandler::GetInstance().Send_ChangePosition(roleId, post);
}

void UMMOUIGuildMainPanel::LeaderDemise(const FString& roleName)
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id == 0)
	{
		return;
	}
		
	if (baseInfo.LeaderName == roleName)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.YouAreLeader")),MsgInfoType_Table,TEXT("MMOGame.Guild.YouAreLeader"));
		return;
	}

	if (!MMOGuildData::getSingleton().IsGuildFullMember(roleName))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.DemiseError")),MsgInfoType_Table,TEXT("MMOGame.Guild.DemiseError"));
		return;
	}

	UINT32 roleId = MMOGuildData::getSingleton().GetGuildFullMemberId(roleName);
	CMessageHandler::GetInstance().Send_GuildDemise(roleId);
}

void UMMOUIGuildMainPanel::DisbandGuild(const FString& text)
{
	if (text == MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Disband")))
	{
		CMessageHandler::GetInstance().Send_DisbandGuild();
	}
	else
	{
		FString text = TEXT("Invalid input");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.DisbandSuccess"));
	}
}

void UMMOUIGuildMainPanel::UpdateGuildRights(INT post)
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");

	UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);
	object->SetBool(TEXT("guild_rights_invite"), (rights & G_POS_RIGHT_INVITE) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_recruit"), (rights & G_POS_RIGHT_RECRUIT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_forbid_chat"), (rights & G_POS_RIGHT_FORBID_CHAT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_notice"), (rights & G_POS_RIGHT_NOTICE) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_storage_right"), (rights & G_POS_RIGHT_SET_STORAGE_RIGHT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_dungeon"), (rights & G_POS_RIGHT_OPEN_DUNGEON) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_upgrade_building"), (rights & G_POS_RIGHT_UPGRADE_BUILDING) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_upgrade_skill"), (rights & G_POS_RIGHT_UPGRADE_WELFARE) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_change_pos"), (rights & G_POS_RIGHT_CHANGE_POS) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_banish"), (rights & G_POS_RIGHT_BANISH) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_foreign"), (rights & G_POS_RIGHT_DIPLOMAT) > 0 ? true : false);
	object->SetBool(TEXT("guild_rights_activity"), (rights & G_POS_RIGHT_ACTIVITY) > 0 ? true : false);

	object->SetString(TEXT("guild_rights_tips"), *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ManageRights")));

	eventCallUpdateGuildRights(object);
}
void UMMOUIGuildMainPanel::ModifyGuildStorageRights(INT post, UGFxObject* object)
{
	if (!object)
	{
		return;
	}

	UINT16 rights = 0;
	for (INT i = 0; i < GUILD_MAX_STORAGE_RIGHT_COUNT; ++ i)
	{
		FString text = TEXT("");
		if (i < GUILD_MAX_STORAGE_RIGHT_COUNT / 2)
		{
			text = FString::Printf(TEXT("output%d"), i);
			if (object->GetBool(text))
			{
				rights |= 1 << i;
			}
		}
		else
		{
			text = FString::Printf(TEXT("input%d"), i - (GUILD_MAX_STORAGE_RIGHT_COUNT / 2));
			if (object->GetBool(text))
			{
				rights |= 1 << i;
			}
		}
	}

	CMessageHandler::GetInstance().Send_GuildChangeStorageRight(post, rights);
}

void UMMOUIGuildMainPanel::UpdateGuildStorageRights(INT post)
{
	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Guild.GuildStorageData"));
	if (!object)
	{
		return;
	}

	UINT16 rights = MMOGuildData::getSingleton().GetGuildStorageRights(post);
	object->SetBool(TEXT("output0"), (rights & G_STORAGE_1_POP) > 0 ? true : false);
	object->SetBool(TEXT("output1"), (rights & G_STORAGE_2_POP) > 0 ? true : false);
	object->SetBool(TEXT("output2"), (rights & G_STORAGE_3_POP) > 0 ? true : false);
	object->SetBool(TEXT("output3"), (rights & G_STORAGE_4_POP) > 0 ? true : false);
	object->SetBool(TEXT("output4"), (rights & G_STORAGE_5_POP) > 0 ? true : false);
	object->SetBool(TEXT("output5"), (rights & G_STORAGE_6_POP) > 0 ? true : false);
	object->SetBool(TEXT("input0"), (rights & G_STORAGE_1_PUSH) > 0 ? true : false);
	object->SetBool(TEXT("input1"), (rights & G_STORAGE_2_PUSH) > 0 ? true : false);
	object->SetBool(TEXT("input2"), (rights & G_STORAGE_3_PUSH) > 0 ? true : false);
	object->SetBool(TEXT("input3"), (rights & G_STORAGE_4_PUSH) > 0 ? true : false);
	object->SetBool(TEXT("input4"), (rights & G_STORAGE_5_PUSH) > 0 ? true : false);
	object->SetBool(TEXT("input5"), (rights & G_STORAGE_6_PUSH) > 0 ? true : false);

	eventCallUpdateGuildStorageRights(object, *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.BankRights")));
}

void UMMOUIGuildMainPanel::UpdateGuildStorageState()
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");

	INT level = MMOGuildData::getSingleton().GetGuildBaseInfo().level;
	object->SetString(TEXT("guild_storage_active_1"), level >= 1 ? TEXT("<font color='#00FF00'>Activated</font>") : TEXT("<font color='#FF0000'>Inactive</font>"));
	object->SetString(TEXT("guild_storage_active_2"), level >= 2 ? TEXT("<font color='#00FF00'>Activated</font>") : TEXT("<font color='#FF0000'>Inactive</font>"));
	object->SetString(TEXT("guild_storage_active_3"), level >= 3 ? TEXT("<font color='#00FF00'>Activated</font>") : TEXT("<font color='#FF0000'>Inactive</font>"));
	object->SetString(TEXT("guild_storage_active_4"), level >= 4 ? TEXT("<font color='#00FF00'>Activated</font>") : TEXT("<font color='#FF0000'>Inactive</font>"));
	object->SetString(TEXT("guild_storage_active_5"), level >= 5 ? TEXT("<font color='#00FF00'>Activated</font>") : TEXT("<font color='#FF0000'>Inactive</font>"));
	object->SetString(TEXT("guild_storage_active_6"), level >= 6 ? TEXT("<font color='#00FF00'>Activated</font>") : TEXT("<font color='#FF0000'>Inactive</font>"));

	eventCallUpdateGuildStorageState(object);
}

void UMMOUIGuildMainPanel::UpdateGuildSalary()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	for (INT i = 0; i < GUILD_POSITION_COUNT; ++ i)
	{
		UGFxObject* object = GFXPlayer->CreateObject("Object");

		FString postName = TEXT("");
		MMOGuildData::getSingleton().GetGuildPostName(i + 1, postName);
		object->SetString(TEXT("post"), postName);

		INT postCount = MMOGuildData::getSingleton().GetGuildPostCount(i + 1);
		object->SetInt(TEXT("post_count"), postCount);

		INT percent = MMOGuildData::getSingleton().GetGuildSalaryPercent(i + 1);
		object->SetInt(TEXT("percent"), percent);

		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		INT total = (INT)(FLOAT(percent) / 100.0f * FLOAT(baseInfo.Money) * FLOAT(GUILD_SALARY_PERCENT) / 100.0f);
		object->SetInt(TEXT("total"), total);
		object->SetInt(TEXT("capita"), postCount == 0 ? 0 : total / postCount);

		dataProvider->SetElementObject(i, object);
	}

	eventCallUpdateGuildSalary(dataProvider);
}

void UMMOUIGuildMainPanel::UpdatePlayerMoney(INT money)
{
	if(IsVisible==FALSE)
	{
		return;
	}
	FString intro = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeMoneyIntro"));
	eventCallUpdateContributeInfo(money, intro);
}

void UMMOUIGuildMainPanel::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
	switch (MoneyType)
	{
	case MONEYTYPE_01:
		break;
	case MONEYTYPE_02:
		UpdatePlayerMoney(  MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)MoneyType)  );
		UpdateGuildBaseInfo();
		break;
	case MONEYTYPE_03:
		break;
	case MONEYTYPE_04:
		break;
	}
}

void UMMOUIGuildMainPanel::Contribute(INT money)
{
}

void UMMOUIGuildMainPanel::DistributeSalary(UGFxObject* object)
{
	UINT8 salaryPercent[GUILD_POSITION_COUNT];
	for (INT i = 0; i < GUILD_POSITION_COUNT; ++ i)
	{
		FString text = FString::Printf(TEXT("percent%d"), i);
		salaryPercent[i] = object->GetInt(text);
	}

	CMessageHandler::GetInstance().Send_GuildChangeSalaryPercent(salaryPercent);
}

void UMMOUIGuildMainPanel::UpdateGuildAllyList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildAllyInfoList guildAllyInfoList = MMOGuildData::getSingleton().GetGuildAllyInfoList();
	GuildAllyInfoList::iterator iter = guildAllyInfoList.begin();
	INT index = 0;
	for (; iter != guildAllyInfoList.end(); ++ iter)
	{
		if (iter->LeftTime <= 0)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject("Object");
		object->SetString(TEXT("label"), iter->Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetInt(TEXT("guild_foreign_type"), GR_FRIEND);
		object->SetInt(TEXT("guild_id"), iter->Id);
		object->SetString(TEXT("guild_name"), iter->Name);
		object->SetInt(TEXT("guild_level"), iter->Level);
		object->SetInt(TEXT("guild_member_count"), iter->MemberCount);

		FString timeString = TEXT("");
		MMOFacade::getSingleton().GetTimeString(iter->LeftTime, timeString);
		object->SetString(TEXT("guild_left_time"), timeString);

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	eventCallUpdateGuildAllyList(dataProvider);
}

void UMMOUIGuildMainPanel::UpdateGuildEnemyList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildEnemyInfoList guildEnemyInfoList = MMOGuildData::getSingleton().GetGuildEnemyInfoList();
	GuildEnemyInfoList::iterator iter = guildEnemyInfoList.begin();
	INT index = 0;
	for (; iter != guildEnemyInfoList.end(); ++ iter)
	{
		if (iter->LeftTime <= 0)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject("Object");
		object->SetString(TEXT("label"), iter->Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetInt(TEXT("guild_foreign_type"), GR_ENEMY);
		object->SetInt(TEXT("guild_id"), iter->Id);
		object->SetString(TEXT("guild_name"), iter->Name);
		object->SetInt(TEXT("guild_level"), iter->Level);
		object->SetInt(TEXT("guild_member_count"), iter->MemberCount);

		FString timeString = TEXT("");
		MMOFacade::getSingleton().GetTimeString(iter->LeftTime, timeString);
		object->SetString(TEXT("guild_left_time"), timeString);

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	eventCallUpdateGuildEnemyList(dataProvider);
}

void UMMOUIGuildMainPanel::UpdateGuildApplyAllyList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();

	GuildApplyAllyInfoList guildApplyAllyInfoList = MMOGuildData::getSingleton().GetGuildApplyAllyInfoList();
	GuildApplyAllyInfoList::iterator iter = guildApplyAllyInfoList.begin();
	INT index = 0;
	for (; iter != guildApplyAllyInfoList.end(); ++ iter)
	{
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		object->SetString(TEXT("label"), iter->Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetInt(TEXT("guild_id"), iter->Id);
		object->SetString(TEXT("guild_name"), iter->Name);
		object->SetInt(TEXT("guild_level"), iter->Level);
		object->SetInt(TEXT("guild_member_count"), iter->MemberCount);
		object->SetString(TEXT("guild_leader_name"), iter->LeaderName);

		FString timeString = TEXT("");
		MMOFacade::getSingleton().GetTimeString(iter->LeftTime, timeString);
		object->SetString(TEXT("guild_left_time"), timeString);

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	eventCallUpdateGuildApplyAllyList(dataProvider);
}

void UMMOUIGuildMainPanel::UpdateGuildWarInfo()
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");
	if (!object)
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

	FString timeString = TEXT("");
	MMOFacade::getSingleton().GetTimeString(MMOGuildManager::getSingleton().GetGuildWarCD(), timeString);
	object->SetString(TEXT("guild_war_cd"), timeString);

	object->SetString(TEXT("guild_war_introduce"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WarIntroduce")));

	eventCallUpdateGuildWarInfo(object);
}

void UMMOUIGuildMainPanel::UpdateGuildWarCD()
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");

	FString timeString = TEXT("");
	MMOFacade::getSingleton().GetTimeString(MMOGuildManager::getSingleton().GetGuildWarCD(), timeString);
	object->SetString(TEXT("guild_war_cd"), timeString);

	eventCallUpdateGuildWarCD(timeString);
}

void UMMOUIGuildMainPanel::ApplyAlly(const FString& guildName, int timeType)
{
	if (guildName == TEXT(""))
	{
		FString text = TEXT("Please enter a guild name");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildNoItem"));
		return;
	}

	CMessageHandler::GetInstance().Send_GuildDiplomatApp(GUILD_DIPLOMAT_ALLIANCE, timeType + 1, guildName);
}

void UMMOUIGuildMainPanel::ApplyEnemy(const FString& guildName, int timeType)
{
	if (guildName == TEXT(""))
	{
		FString text = TEXT("Please enter a guild name");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errGuildNoItem"));
		return;
	}

	CMessageHandler::GetInstance().Send_GuildDiplomatApp(GUILD_DIPLOMAT_ENEMY, timeType + 1, guildName);
}

void UMMOUIGuildMainPanel::AgreeAlly(INT id)
{
	CMessageHandler::GetInstance().Send_GuildReplyUnionApp(GUILD_REVERT_AGREE, id);
	CMessageHandler::GetInstance().Send_GuildGetApplyUnionList();
}

void UMMOUIGuildMainPanel::RefuseAlly(INT id)
{
	CMessageHandler::GetInstance().Send_GuildReplyUnionApp(GUILD_REVERT_REFUSE, id);
	CMessageHandler::GetInstance().Send_GuildGetApplyUnionList();
}

void UMMOUIGuildMainPanel::InviteToJoinGuild(INT roleID)
{
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(roleID);
	if (actor)
	{
		CMessageHandler::GetInstance().Send_InviteToJoinGuild(actor->GetActorName());
	}
}

void UMMOUIGuildMainPanel::ForbidChat(INT roleID, UBOOL isForbid)
{
	CMessageHandler::GetInstance().Send_GuildForbidChat(isForbid ? GUILD_CHAT_FORBID : GUILD_CHAT_UNFORBID, roleID);
}

void UMMOUIGuildMainPanel::UpdateGuildWarSearchState(UBOOL isSearching)
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");

	if (object)
	{
		object->SetBool(TEXT("guild_war_searching"), isSearching);
	}

	eventCallUpdateGuildWarSearchState(object);
}

void UMMOUIGuildMainPanel::UpdateGuildWarSearchInfo(INT matchConsume, INT matchCount)
{
	UGFxObject* object = GFXPlayer->CreateObject("Object");

	if (object)
	{
		object->SetString(TEXT("guild_war_search_consume"), FString::Printf(TEXT("Guild funds: %d"), matchConsume));
		object->SetString(TEXT("guild_war_search_count"), FString::Printf(TEXT("%d/%d"), (INT)GUILD_MAX_MATCH_WAR_COUNT - matchCount, (INT)GUILD_MAX_MATCH_WAR_COUNT));
	}

	eventCallUpdateGuildWarSearchInfo(object);
}

void UMMOUIGuildMainPanel::ChallengeConfirm(const FString& guildName)
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeChallengeNow");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseChallengeNow");	

	_WarTargetName = guildName;
	FString text = FString::Printf(TEXT("Declare war on guild [%s]?"), *guildName);
	ChallengeConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, 0, funYes, funNo);
}

void UMMOUIGuildMainPanel::AgreeChallengeNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	CMessageHandler::GetInstance().Send_Challenge(_WarTargetName);
	_WarTargetName = TEXT("");
	ChallengeConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::RefuseChallengeNow(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_WarTargetName = TEXT("");
	ChallengeConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::SetShowPage(INT page, INT subPage)
{
	_ShowPage = page;
	_ShowSubPage = subPage;
}

void UMMOUIGuildMainPanel::AllyCancelConfirm(INT id)
{
	GuildQueryInfo info;
	if (!MMOGuildData::getSingleton().GetGuildInfoById(id, info))
	{
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("AgreeAllyCancel");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("RefuseAllyCancel");	

	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.AllyCancel")), *(info.Name));
	_AllyCancelConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox(text, id, funYes, funNo);
}

void UMMOUIGuildMainPanel::AgreeAllyCancel(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	INT guildId = mc->GetInt(TEXT("iteminfoid"));
	if (guildId > 0)
	{
		CMessageHandler::GetInstance().Send_CancelAlliance(guildId);
	}
	_AllyCancelConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::RefuseAllyCancel(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_AllyCancelConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::UpdateGuildActivityList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if (!dataProvider)
	{
		return;
	}
	
	MMOGuildData::getSingleton().UpdateGuildActivityState();

	GuildActivityInfoMap guildActivityInfoList = MMOGuildData::getSingleton().GetGuildActivityInfoMap();
	GuildActivityInfoMap::iterator iter = guildActivityInfoList.begin();
	INT index = 0;
	for (; iter != guildActivityInfoList.end(); ++ iter)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		UINT16 activityType = iter->first;
		UINT16 activityLevel = baseInfo.level;
		UINT32 activityId = MAKELONG(activityLevel, activityType);

		GuildActivityData data;
		if (!MMOGuildData::getSingleton().GetGuildActivityData(activityId, data))
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Guild.GuildActiveData"));
		if (!object)
		{
			continue;
		}

		object->SetString(TEXT("label"), data.Name);
		object->SetInt(TEXT("index"), index);
		object->SetInt(TEXT("type"), 0);
		object->SetInt(TEXT("deepth"), 0);
		object->SetBool(TEXT("visible"), true);

		object->SetInt(TEXT("ID"), activityId);
		object->SetString(TEXT("active_name"), data.Name);
		object->SetString(TEXT("active_content"), data.Desc);

		FString timeString = TEXT("");
		if (iter->second.State == GAS_NONE)
		{
			timeString = TEXT("TBD");
		} 
		else
		{
			if (iter->second.StartTime > 0)
			{
				__time32_t startTime = iter->second.StartTime;
				struct tm *startDate = _localtime32(&startTime);
				int startHour = startDate->tm_hour;
				int startMin = startDate->tm_min;

				__time32_t endTime = iter->second.StartTime + data.Duration;
				struct tm *endDate = _localtime32(&endTime);
				int endHour = endDate->tm_hour;
				int endMin = endDate->tm_min;

				if (startDate && endDate)
				{
					timeString = FString::Printf(TEXT("%d:%02d-%d:%02d"), startHour, startMin, endHour, endMin);
				}
			}
		}

		object->SetString(TEXT("active_timer"), timeString);

		FString npcName = TEXT("");
		const UNPCTemplate* npc = MMOFacade::getSingleton().FindNPCTemplate(data.NpcId);
		if (npc)
		{
			npcName = npc->GetDisplayName();
		}

		object->SetString(TEXT("active_map"), npcName);
		object->SetInt(TEXT("current"), MMOGuildData::getSingleton().GetGuildActivityComplete(activityType));
		object->SetInt(TEXT("max"), data.MaxCount);

		AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
		if (mainActor)
		{
			UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID);
			UINT16 rights = MMOGuildData::getSingleton().GetGuildRights(post);
			if ((rights & G_POS_RIGHT_ACTIVITY) == 0)
			{
				if (iter->second.State == GAS_NONE)
				{
					iter->second.State = GAS_NORIGHT;
				}
			}
		}
		object->SetInt(TEXT("active_state"), iter->second.State);

		dataProvider->SetElementObject(index, object);
		index++;
	}

	eventCallUpdateGuildActiveList(dataProvider);
}

//////////////////////////////////////////////////////////////////////////

// guild view
void UMMOUIGuildMainPanel::ASViewGuild()
{
	UMMOUIGuildViewPanel* panel = (UMMOUIGuildViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_View);
	if (panel)
	{
		panel->SetNpcCalled(FALSE);
		panel->ShowGuildViewPanel();
	}
	Show(FALSE);
}

// Update application list
void UMMOUIGuildMainPanel::ASUpdateGuildPendingMemberList()
{
	UpdateGuildPendingMemberList();
}

// Agree to join guild
void UMMOUIGuildMainPanel::ASAgreeToJoin(INT id)
{
	AgreeToJoin(id);
}

// Refuse to join guild
void UMMOUIGuildMainPanel::ASRefuseToJoin(INT id)
{
	RefuseToJoin(id);
}

// fund donation
void UMMOUIGuildMainPanel::ASContribute(INT type, INT count)
{
	if (count == 0)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Donation amount cannot be 0"),MsgInfoType_Table,TEXT("Donation amount cannot be 0"));
		return;
	}

	// determine whether funds have reached the limit
	UINT32 resMax = MMOGuildData::getSingleton().GetGuildTerritoryResMax(GUILD_ATTR_MONEY);
	UINT32 resCur = MMOGuildData::getSingleton().GetCurGuildTerritoryRes(GUILD_ATTR_MONEY);
//	if (resCur >= resMax)
//	{
//		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MoneyStoreLimit"));		
//		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MoneyStoreLimit"));
//		return;
//	}

	// #15422
	if (resCur + count > resMax)
	{	
		//MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MoneyStoreLimit"));

		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = this;
		funYes.FunctionName = TEXT("AgreeContribute");

		FScriptDelegate funNo(EC_EventParm);
		funNo.Object = this;
		funNo.FunctionName = TEXT("RefuseContribute");

		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeOver"));
		_ContributeConfirmBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(text, count, funYes, funNo);
	}
	else
	{
		CMessageHandler::GetInstance().Send_GuildContribute(GUILD_ATTR_MONEY, count);
	}
}

void UMMOUIGuildMainPanel::AgreeContribute(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	INT count = mc->GetInt(TEXT("iteminfoid"));
	if (count > 0)
	{
		CMessageHandler::GetInstance().Send_GuildContribute(GUILD_ATTR_MONEY, count);
	}
	_ContributeConfirmBox = NULL;
}

void UMMOUIGuildMainPanel::RefuseContribute(UGFxObject *mc)
{
	if (!mc)
	{
		return;
	}

	_ContributeConfirmBox = NULL;
}

// Change announcement
void UMMOUIGuildMainPanel::ASChangeNotice(const FString& notice)
{
	ChangeGuildNotice(notice);
}

// Change declaration
void UMMOUIGuildMainPanel::ASChangeAnnounce(const FString& announce)
{
	CMessageHandler::GetInstance().Send_GuildAnnounceChange(announce);
}

// View more guild events
void UMMOUIGuildMainPanel::ASViewMoreEvents()
{
	CMessageHandler::GetInstance().Send_ViewMoreEvents();
}

// Recruit members
void UMMOUIGuildMainPanel::ASInviteMember(const FString& roleName)
{
	InviteMember(roleName);
}

void UMMOUIGuildMainPanel::ASSortMember(INT type, UBOOL isUp)
{
	SortMember(type, isUp);
}

// Expel member
void UMMOUIGuildMainPanel::ASExpelMember(INT id)
{
	ExpelMember(id);
}

// Change position
void UMMOUIGuildMainPanel::ASChangePost(INT roleId, INT post)
{
	ChangePost(roleId, post);
}

// Whether to show online members
void UMMOUIGuildMainPanel::ASShowOnlineMember(UBOOL isShow)
{
	ShowOnlineMember(isShow);
}

// Whether to show online members
void UMMOUIGuildMainPanel::ASOnlineMemberNotify(UBOOL notify)
{
	MMOGuildManager::getSingleton().SetOnlineMemberNotify(notify);
}

// Guild leader abdication
void UMMOUIGuildMainPanel::ASLeaderDemise(const FString& roleName)
{
	LeaderDemise(roleName);
}

// Guild disband
void UMMOUIGuildMainPanel::ASDisbandGuild(const FString& text)
{
	DisbandGuild(text);
}

// Leave guild
void UMMOUIGuildMainPanel::ASQuitGuild()
{
	QuitConfirm();
}

// Update guild permission display
void UMMOUIGuildMainPanel::ASUpdateGuildRights(INT post)
{
	UpdateGuildRights(post + 1);
}

// Update warehouse permission display
void UMMOUIGuildMainPanel::ASModifyGuildStorageRights(INT post, UGFxObject* rights)
{
	ModifyGuildStorageRights(post + 1, rights);
}

void UMMOUIGuildMainPanel::ASUpdateGuildStorageRights(INT post)
{
	UpdateGuildStorageRights(post + 1);
}

// Salary distribution
void UMMOUIGuildMainPanel::ASDistributeSalary(UGFxObject* percent)
{
	DistributeSalary(percent);
}

void UMMOUIGuildMainPanel::ASApplyAlly(const FString& guildName, INT timeType)
{
	ApplyAlly(guildName, timeType);
}

void UMMOUIGuildMainPanel::ASApplyEnemy(const FString& guildName, INT timeType)
{
	ApplyEnemy(guildName, timeType);
}

void UMMOUIGuildMainPanel::ASAgreeAlly(INT id)
{
	AgreeAlly(id);
}

void UMMOUIGuildMainPanel::ASRefuseAlly(INT id)
{
	RefuseAlly(id);
}

void UMMOUIGuildMainPanel::ASFullMemberRightClick(INT posX, INT posY, INT roleId)
{
	FullMemberRightClick(posX, posY, roleId);
}

void UMMOUIGuildMainPanel::ASAllianceRightClick(int posX, int posY, int guildId)
{
	AllianceRightClick(posX, posY, guildId);
}

void UMMOUIGuildMainPanel::ASGuildEnemyRightClick(int posX, int posY, int guildId)
{
	EnemyRightClick(posX, posY, guildId);
}

void UMMOUIGuildMainPanel::ASForeignApplyDayType(int foreignType, int dayType)
{
	SetForeignApplyDayType(foreignType, dayType);
}

void UMMOUIGuildMainPanel::ASChallenge(const FString& guildName)
{
	if (!MMOGuildData::getSingleton().IsGuildEnemy(guildName))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ChallengeNotEnemy"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.ChallengeNotEnemy"));
		return;
	}

	ChallengeConfirm(guildName);
}

// Guild matchmaking war
void UMMOUIGuildMainPanel::ASSearchGuild()
{
	CMessageHandler::GetInstance().Send_Search();
}

void UMMOUIGuildMainPanel::ASSetShowPage(int page, int subPage)
{
	SetShowPage(page, subPage);
}

// guild activity
void UMMOUIGuildMainPanel::ASGuildActivityClickOpen(INT activityId)
{
	if (activityId <= 0)
	{
		return;
	}

	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	GuildActivityData data;
	if (!MMOGuildData::getSingleton().GetGuildActivityData(activityId, data))
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Guild.GuildActiveItemData"));
	if (!object)
	{
		return;
	}

	object->SetInt(TEXT("ID"), activityId);

	FString text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), data.MoneyReq);
	FString text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), data.MoneyReq);
	object->SetString(TEXT("money"), baseInfo.Money >= data.MoneyReq ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), data.HerbReq);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), data.HerbReq);
	object->SetString(TEXT("herb"), baseInfo.Herb >= data.HerbReq ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), data.WoodReq);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), data.WoodReq);
	object->SetString(TEXT("wood"), baseInfo.Wood >= data.WoodReq ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), data.CrystalReq);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), data.CrystalReq);
	object->SetString(TEXT("crystal"), baseInfo.Crystal >= data.CrystalReq ? text1 : text2);

	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%d</font>"), data.SilverReq);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%d</font>"), data.SilverReq);
	object->SetString(TEXT("silver"), baseInfo.Silver >= data.SilverReq ? text1 : text2);

	// prerequisite building
	UINT32 preBuildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(data.BuildingReq);
	FString preBuildingName = TEXT("None");
	GuildBuildingData buildingData;
	if (MMOGuildData::getSingleton().GetGuildBuildingData(data.BuildingReq, 0, buildingData))
	{
		preBuildingName = buildingData.Name;
	}
	text1 = FString::Printf(TEXT("<font color='#FFFFFF'>%s</font>"), *preBuildingName);
	text2 = FString::Printf(TEXT("<font color='#FF0000'>%s</font>"), *preBuildingName);
	object->SetString(TEXT("preBuilding"), preBuildingLv >= data.BuildingLvReq ? text1 : text2);

	object->SetBool(TEXT("enough"), 
		baseInfo.Money >= data.MoneyReq &&
		baseInfo.Herb >= data.HerbReq &&
		baseInfo.Wood >= data.WoodReq &&
		baseInfo.Crystal >= data.CrystalReq &&
		baseInfo.Silver >= data.SilverReq &&
		preBuildingLv >= data.BuildingLvReq);

	eventCallShowActiveOpenConfirm(object);
}

void UMMOUIGuildMainPanel::ASGuildActivitySumitOpen(int activityId)
{
	if (activityId > 0)
	{
		UINT16 activityType = HIWORD(activityId);
		CMessageHandler::GetInstance().Send_OpenGuildActivity(activityType);
		eventCallHideActiveOpenConfirm();
	}
}
