#include "stdafx.h"
#include "message_handler.h"
#include "MMOGuildManager.h"

using namespace ExternalMessage;

#define OffLineMode 0
// send
//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Send_QueryGuildName(UINT32 id, UINT32 crc)
{
	C2M_QUERY_GUILD_NAME* msg = (C2M_QUERY_GUILD_NAME*)ms_MsgBuffer;
	msg->dwGuildID = id;
	msg->dwCRC = crc;
	msg->wMsg = c2m_query_guild_name;
	_Send(msg, sizeof(C2M_QUERY_GUILD_NAME));
}

void CMessageHandler::Send_CreateGuild(const FString& name, const FString& announce)
{
	FTCHARToUTF8 szAnnounce(*announce);
	const INT32 textLength = szAnnounce.Length();
	if (textLength < 0 || textLength >= GUILD_ANNOUNCE_LEN ||
		(size_t)textLength + sizeof(C2G_GUILD_CREATE) + 1 > sizeof(ms_MsgBuffer) ||
		(size_t)textLength + sizeof(C2G_GUILD_CREATE) + 1 > 0xffff)
	{
		return;
	}

	const INT32 nAnnounceSize = textLength + 1;
	C2G_GUILD_CREATE* msg = (C2G_GUILD_CREATE*)ms_MsgBuffer;
	appMemzero(msg, sizeof(*msg));
	msg->wMsg = c2g_guild_create;
	strxcpy(msg->szGuildName, *name, GUILD_NAME_LEN);
	appMemcpy(msg->szAnnounce, (const ANSICHAR*)szAnnounce, nAnnounceSize);
	msg->wSize = sizeof(C2G_GUILD_CREATE) + nAnnounceSize;
	_Send(msg, msg->wSize);
}

void CMessageHandler::Send_ViewGuildList(UINT8 type, UINT32 id)
{
	C2M_VIEW_GUILD_LIST msg;
	msg.wMsg = c2m_view_guild_list;
	msg.byViewType = type;
	msg.dwLastGuildID = id;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_FindGuild(UINT32 id, const FString& name)
{
	C2M_FIND_GUILD msg;
	msg.wMsg = c2m_find_guild;
	msg.dwGuildID = id;
	strxcpy(msg.szGuildName, *name, GUILD_NAME_LEN);
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ApplyToJoinGuild(UINT32 id, const FString& name)
{
	C2M_APPLY_TO_JOIN_GUILD msg;
	msg.wMsg = c2m_apply_to_join_guild;
	msg.dwGuildID = id;
	strxcpy(msg.szGuildName, *name, GUILD_NAME_LEN);
	_Send(&msg, sizeof(msg));

}

void CMessageHandler::Send_GetApplyGuildList()
{
	C2M_GET_APPLY_GUILD_LIST msg;
	msg.wMsg = c2m_get_apply_guild_list;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildApplyResponse(UINT8 result, UINT32 roleId)
{
	C2M_REVERT_GUILD_APPLY msg;
	msg.wMsg = c2m_revert_guild_apply;
	msg.byRevert = result;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_InviteToJoinGuild(const FString& name)
{
	C2M_INVITE_TO_JOIN_GUILD msg;
	msg.wMsg = c2m_invite_to_join_guild;
	strxcpy(msg.szRoleName, *name, ROLE_NAME_LEN);
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildInviteResponse(UINT8 result, UINT32 roleId)
{
	C2M_REVERT_GUILD_INVITATION msg;
	msg.wMsg = c2m_revert_guild_invitation;
	msg.byResult = result;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_DisbandGuild()
{
	C2M_DISBAND_GUILD msg;
	msg.wMsg = c2m_disband_guild;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ViewMoreEvents()
{
	C2M_GUILD_MORE_HISTORY msg;
	msg.wMsg = c2m_guild_more_history;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildNoticeChange(const FString& notice)
{
	FTCHARToUTF8 szNotice(*notice);
	const INT32 textLength = szNotice.Length();
	if (textLength < 0 || textLength >= GUILD_NOTICE_LEN ||
		(size_t)textLength + sizeof(C2M_GUILD_NOTICE_CHANGE) + 1 > sizeof(ms_MsgBuffer) ||
		(size_t)textLength + sizeof(C2M_GUILD_NOTICE_CHANGE) + 1 > 0xffff)
	{
		return;
	}

	const INT32 nNoticeSize = textLength + 1;
	C2M_GUILD_NOTICE_CHANGE* msg = (C2M_GUILD_NOTICE_CHANGE*)ms_MsgBuffer;
	appMemzero(msg, sizeof(*msg));
	msg->wMsg = c2m_guild_notice_change;
    appMemcpy(msg->szNotice, (const ANSICHAR*)szNotice, nNoticeSize);
	msg->wSize = sizeof(C2M_GUILD_NOTICE_CHANGE) + nNoticeSize;
	_Send(msg, msg->wSize);
}

void CMessageHandler::Send_GuildForbidChat(UINT8 operate, UINT32 roleId)
{
	C2M_GUILD_FORBID_CHAT msg;
	msg.wMsg = c2m_guild_forbid_chat;
	msg.byOperation = operate;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ChangePosition(UINT32 roleId, UINT8 pos)
{
	C2M_GUILD_CHANGE_POSITION msg;
	msg.wMsg = c2m_guild_change_position;
	msg.dwRoleID = roleId;
	msg.byPosition = pos;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildRoleLeave()
{
//	C2M_GUILD_ROLE_LEAVE msg;
//	msg.wMsg = c2m_guild_role_leave;
//	_Send(&msg, sizeof(msg));

	C2G_LEAVE_GUILD msg;
	msg.wMsg = c2g_leave_guild;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildBanishRole(UINT32 roleId)
{
	C2M_GUILD_BANISH_ROLE msg;
	msg.wMsg = c2m_guild_banish_role;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildDemise(UINT32 roleId)
{
	C2M_GUILD_DEMISE msg;
	msg.wMsg = c2m_guild_demise;
	msg.dwRoleID = roleId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildContribute(UINT32 type, UINT32 count)
{
	C2G_GUILD_CONTRIBUTE_FUND msg;
	msg.wMsg = c2g_guild_contribute_fund;
	msg.nMoney = count;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildChangeSalaryPercent(UINT8 percent[7])
{
//	C2M_GUILD_CHANGE_SALARY_PERCENT msg;
//	msg.wMsg = c2m_guild_change_salary_percent;
//	appMemcpy(msg.bySalaryPercent, percent, sizeof(UINT8) * 7);
//	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_DrawSalary()
{
	C2G_GUILD_DRAW_SALARY msg;
	msg.wMsg = c2g_guild_draw_salary;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildDiplomatApp(UINT8 opType, UINT8 timeType, const FString& name)
{
	C2M_GUILD_DIPLOMAT_APP msg;
	msg.wMsg = c2m_guild_diplomat_app;
	msg.byOpType = opType;
	msg.byTimeType = timeType;
	strxcpy(msg.szGuildName, *name, GUILD_NAME_LEN);
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildGetApplyUnionList()
{
	C2M_GUILD_GET_APPLY_UNION_LIST msg;
	msg.wMsg = c2m_guild_get_apply_union_list;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildReplyUnionApp(UINT8 opType, UINT32 id)
{
	C2M_GUILD_REPLY_UNION_APP msg;
	msg.wMsg = c2m_guild_reply_union_app;
	msg.byOp = opType;
	msg.dwGuildID = id;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CancelAlliance(UINT32 id)
{
	C2M_GUILD_CANCEL_ALLIANCE msg;
	msg.wMsg = c2m_guild_cancel_alliance;
	msg.dwGuildID = id;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildChangeStorageRight(UINT8 pos, UINT16 right)
{
	C2M_GUILD_CHANGE_STORAGE_RIGHT msg;
	msg.wMsg = c2m_guild_change_storage_right;
	msg.byPosition = pos;
	msg.wRight = right;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RefreshWarehouse( INT tabID )
{
	C2G_GUILD_VIEW_STORAGE msg;
	msg.wMsg = c2g_guild_view_storage;
	msg.byStorageTag = tabID;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildPushItem(UINT8 srcConSlot, UINT8 srcSlot, UINT8 dstConSlot, UINT8 dstSlot)
{
	//Put into warehouse
	C2G_GUILD_PUSH_ITEM msg;
	msg.wMsg = c2g_guild_push_item;
	msg.wItemConSlot = srcConSlot;
	msg.wItemSlot = srcSlot;
	msg.byStorageConSlot = dstConSlot;
	msg.byStorageSlot = dstSlot;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildPopItem(UINT8 srcConSlot, UINT8 srcSlot, UINT8 dstConSlot, UINT8 dstSlot)
{
	C2G_GUILD_POP_ITEM msg;
	msg.wMsg = c2g_guild_pop_item;
	msg.wItemConSlot = dstConSlot;
	msg.wItemSlot = dstSlot;
	msg.byStorageConSlot = srcConSlot;
	msg.byStorageSlot = srcSlot;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildMoveItem(UINT8 srcConSlot, UINT8 srcSlot, UINT8 dstConSlot, UINT8 dstSlot)
{
	C2M_GUILD_MOVE_ITEM msg;
	msg.wMsg = c2m_guild_move_item;
	msg.bySourceConSlot = srcConSlot;
	msg.bySourceSlot = srcSlot;
	msg.byTargetConSlot = dstConSlot;
	msg.byTargetSlot = dstSlot;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_UpgradeGuildWelfare(INT8 type)
{
	C2M_GUILD_UPGRADE_WELFARE msg;
	msg.wMsg = c2m_guild_upgrade_welfare;
	msg.byType = type;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CancelGuildWelfareUpgrade(INT8 type)
{
	C2M_GUILD_CANCEL_WELFARE_UPGRADE msg;
	msg.wMsg = c2m_guild_cancel_welfare_upgrade;
	msg.byType = type;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CreateGuildBuilding(UINT32 npcId, UINT8 type)
{
	C2G_GUILD_CREATE_BUILDING msg;
	msg.wMsg = c2g_guild_create_building;
	msg.dwNpcID = npcId;
	msg.byBuildingType = type;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_UpgradeGuildBuilding(UINT8 upgradeType, UINT64 buildingId, INT8 type)
{
	C2G_GUILD_UPGRADE_BUILDING msg;
	msg.wMsg = c2g_guild_upgrade_building;
	msg.byUpgradeType = upgradeType;
	msg.qwNpcNameID = buildingId;
	msg.byBuildingType = type;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_DestroyGuildBuilding(UINT8 delType, UINT64 buildingId)
{
	C2G_GUILD_DEL_BUILDING msg;
	msg.wMsg = c2g_guild_del_building;
	msg.byDelType = delType;
	msg.qwNameID = buildingId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_CancelGuildBuildingUpgrade(INT8 type, UINT64 buildingId)
{
	C2M_GUILD_CANCEL_BUILDING_UPGRADE msg;
	msg.wMsg = c2m_guild_cancel_building_upgrade;
	msg.qwNameID = buildingId;
	msg.byType = type;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_PurchaseGuildWelfare(INT8 type)
{
	C2G_GUILD_BUY_AURA msg;
	msg.wMsg = c2g_guild_buy_aura;
	msg.byType = type;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_EnterGuildMap(UINT8 mapType, UINT32 guildId)
{
	C2G_GUILD_ENTER_MAP msg;
	msg.wMsg = c2g_guild_enter_map;
	msg.byMapType = mapType;
	msg.dwSceneID = guildId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ChangeWeather(UINT32 weatherType, UINT32 timeType)
{
	C2G_GUILD_CHANGE_WEATHER msg;
	msg.wMsg = c2g_guild_change_weather;
	msg.byWeatherType = weatherType;
	msg.byTimeType = timeType;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_OpenGuildDungeon(UINT32 dunId, UINT32 leaderId)
{
	C2G_GUILD_OPEN_DUNGEON msg;
	msg.wMsg = c2g_guild_open_dungeon;
	msg.dwTemplateID = dunId;
	msg.dwLeaderID = leaderId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_EnterGuildDungeon(UINT32 dunId)
{
	C2G_GUILD_ENTER_DUNGEON msg;
	msg.wMsg = c2g_guild_enter_dungeon;
	msg.dwTemplateID = dunId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RefreshGuildDungeonList()
{
	C2M_GUILD_GET_DUNGEON_LIST msg;
	msg.wMsg = c2m_guild_get_dungeon_list;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_Challenge(const FString& name)
{
	C2M_GUILD_DECLARE_WAR msg;
	msg.wMsg = c2m_guild_declare_war;
	strxcpy(msg.szGuildName, *name, GUILD_NAME_LEN);
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_Search()
{
    // todo_zhangrui war
	//C2M_GUILD_MATCH_GUILD msg;
	//msg.wMsg = c2m_guild_match_guild;
	//_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_MatchChallenge(UINT32 id)
{
	MMOGuildManager::getSingleton().QueryGuildInfo(id);

    // todo_zhangrui war
	//C2M_GUILD_BEGIN_MATCH_WAR msg;
	//msg.wMsg = c2m_guild_begin_match_war;
	//msg.dwGuildID = id;
	//_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_MoreGuildInfo()
{
	C2M_MORE_GUILD_INFO msg;
	msg.wMsg = c2m_more_guild_info;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_GuildAnnounceChange(const FString& announce)
{
	FTCHARToUTF8 szAnnounce(*announce);
	const INT32 textLength = szAnnounce.Length();
	if (textLength < 0 || textLength >= GUILD_ANNOUNCE_LEN ||
		(size_t)textLength + sizeof(C2M_GUILD_ANNOUNCE_CHANGE) + 1 > sizeof(ms_MsgBuffer) ||
		(size_t)textLength + sizeof(C2M_GUILD_ANNOUNCE_CHANGE) + 1 > 0xffff)
	{
		return;
	}

	const INT32 nAnnounceSize = textLength + 1;
	C2M_GUILD_ANNOUNCE_CHANGE* msg = (C2M_GUILD_ANNOUNCE_CHANGE*)ms_MsgBuffer;
	appMemzero(msg, sizeof(*msg));
	msg->wMsg = c2m_guild_announce_change;
	appMemcpy(msg->szAnnounce, (const ANSICHAR*)szAnnounce, nAnnounceSize);
	msg->wSize = sizeof(C2M_GUILD_ANNOUNCE_CHANGE) + nAnnounceSize;
	_Send(msg, msg->wSize);
}

void CMessageHandler::Send_OpenGuildActivity(UINT32 activityId)
{
	C2M_GUILD_ACTIVITY msg;
	msg.wMsg = c2m_guild_activity;
	msg.nActivityID = activityId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_QueryGuildWarRecord(UINT8 page)
{
	C2G_QUERY_GUILD_WAR_RECORD msg;
	msg.wMsg = c2g_query_guild_war_record;
	msg.byPage = page;
	_Send(&msg, sizeof(msg));
}

// receive
//////////////////////////////////////////////////////////////////////////
void CMessageHandler::Parse_m2c_query_guild_name_ack(const char* buf, UINT32 size)
{
	ASSERT_CHECK_ERROR_RETURN(buf && size == sizeof(M2C_QUERY_GUILD_NAME_ACK));
	M2C_QUERY_GUILD_NAME_ACK* msg = (M2C_QUERY_GUILD_NAME_ACK*)buf;
	if (!buf || size < sizeof(M2C_QUERY_GUILD_NAME_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyQueryGuildInfo(msg);
}

void CMessageHandler::Parse_m2c_create_guild_ack(const char* buf, UINT32 size)
{
	M2C_CREATE_GUILD_ACK* msg = (M2C_CREATE_GUILD_ACK*)buf;
	if (!buf || size < sizeof(M2C_CREATE_GUILD_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyCreateGuildSuccess(*ReadFixedUTF8(msg->szGuildName));
}

void CMessageHandler::Parse_m2c_guild_add_storage_event_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_ADD_STORAGE_EVENT_NTF* msg = (M2C_GUILD_ADD_STORAGE_EVENT_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ADD_STORAGE_EVENT_NTF))
		return;
	UBOOL fullPlayerName = TRUE;
	if(msg)
	{
		PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( msg->dwRoleID );
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		FString RoleName;
		if( msg->dwRoleID == player->ServerID )
		{
			RoleName = player->ActorName;
		}
		else if(!playerInfo)
		{
			//MMOFacade::getSingleton().RequestPlayerBaseInfo( msg->dwRoleID );
			fullPlayerName = FALSE;
		}
		else
		{
			RoleName = playerInfo->_RoleName;
		}

		ItemInfo iteminfo;
		if (!iteminfo.Load(msg->szItemBuffer, sizeof(msg->szItemBuffer)))
			return;

		UItem* item = MMOFacade::getSingleton().FindItemTemplate(iteminfo.m_nTemplateID);
		if (item)
		{
			BYTE durType = item->ItemDuration;
			int curCount = 0;
			if (durType == DURTYPE_NONE ||
				durType == DURTYPE_EQUIP ||
				durType == DURTYPE_USE ||
				durType == DURTYPE_PURITY ||
				durType == DURTYPE_DEPOSIT)
			{
				curCount = 1;
			}
			else
			{
				curCount = iteminfo.m_nCurDur;
			}
			MMOGuildData::getSingleton().AddAccessInfo(msg->dwRoleID, RoleName, iteminfo.m_nTemplateID, curCount, msg->dwOccurTime, msg->byType);
		}
	}
	if(fullPlayerName)
	{
		MMOUI::GetUIManager()->delegateUpdateAccessInfo();
	}
}

void CMessageHandler::Parse_m2c_guild_storage_event(const char* buf, UINT32 size)
{
	M2C_GUILD_STORAGE_EVENT* msg = (M2C_GUILD_STORAGE_EVENT*)buf;
	if (!buf || size < sizeof(M2C_GUILD_STORAGE_EVENT))
		return;

	UBOOL fullPlayerName = TRUE;

	MMOGuildData::getSingleton().ClearAccessInfo();

	if (msg->byEventCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	for ( INT i = 0 ; i < msg->byEventCount ; i++ )
	{
		PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( msg->info[i].dwRoleID );
		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		FString RoleName;
		if( msg->info[i].dwRoleID == player->ServerID )
		{
			RoleName = player->ActorName;
		}
		else if(!playerInfo)
		{
			//MMOFacade::getSingleton().RequestPlayerBaseInfo( msg->info[i].dwRoleID );
			fullPlayerName = FALSE;
		}
		else
		{
			RoleName = playerInfo->_RoleName;
		}

		ItemInfo iteminfo;
		if (!iteminfo.Load(msg->info[i].szItemBuffer, sizeof(msg->info[i].szItemBuffer)))
			return;

		UItem* item = MMOFacade::getSingleton().FindItemTemplate(iteminfo.m_nTemplateID);
		if (item)
		{
			BYTE durType = item->ItemDuration;
			int curCount = 0;
			if (durType == DURTYPE_NONE ||
				durType == DURTYPE_EQUIP ||
				durType == DURTYPE_USE ||
				durType == DURTYPE_PURITY ||
				durType == DURTYPE_DEPOSIT)
			{
				curCount = 1;
			}
			else
			{
				curCount = iteminfo.m_nCurDur;
			}
			MMOGuildData::getSingleton().AddAccessInfo(msg->info[i].dwRoleID, RoleName, iteminfo.m_nTemplateID, curCount, msg->info[i].dwTime, msg->info[i].byType);
		}	
	}
	//If names are complete update immediately, otherwise update once name info is complete
	if(fullPlayerName)
	{
		MMOUI::GetUIManager()->delegateUpdateAccessInfo();
	}
}

void CMessageHandler::Parse_m2c_view_guild_list_ack(const char* buf, UINT32 size)
{
	M2C_VIEW_GUILD_LIST_ACK* msg = (M2C_VIEW_GUILD_LIST_ACK*)buf;
	if (!buf || size < sizeof(M2C_VIEW_GUILD_LIST_ACK))
		return;
	if ((UINT32)msg->byCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildViewList(msg->byViewType, msg->info, msg->byCount);
}

void CMessageHandler::Parse_m2c_find_guild_ack(const char* buf, UINT32 size)
{
	M2C_FIND_GUILD_ACK* msg = (M2C_FIND_GUILD_ACK*)buf;
	if (!buf || size < sizeof(M2C_FIND_GUILD_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildFindList(msg->data);
}

void CMessageHandler::Parse_m2c_guild_info_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_INFO_NTF* msg = (M2C_GUILD_INFO_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_INFO_NTF))
		return;
	if (msg->byMemberCount > (size - sizeof(*msg)) / sizeof(msg->arrMemberData[0]) ||
		msg->stEventInfo.byEventCount > ARRAY_COUNT(msg->stEventInfo.stEventInfo) ||
		msg->stChestInfo.byCount > ARRAY_COUNT(msg->stChestInfo.stEvent) ||
		msg->byAllianceCount > ARRAY_COUNT(msg->stAlliance) ||
		msg->byEnemyCount > ARRAY_COUNT(msg->stEnemy))
		return;

	// Guild basic info
	GuildBaseInfo baseInfo;
	baseInfo.Id = msg->dwGuildID;
	baseInfo.Name = *ReadFixedUTF8(msg->szGuildName);
	baseInfo.level = msg->byLevel;
	baseInfo.MemberCount = msg->byMemberCount;
	baseInfo.Money = msg->dwGuildMoney;
	baseInfo.Salary = 0;
	baseInfo.LeaderName = *ReadFixedUTF8(msg->szLeaderName);
	baseInfo.CreatorName = *ReadFixedUTF8(msg->szCreaterName);
	baseInfo.CreateTime = msg->dwCreateTime;
	baseInfo.Herb = msg->nHerb;
	baseInfo.Wood = msg->nWood;
	baseInfo.Crystal = msg->nCrystal;
	baseInfo.Silver = msg->nSilver;
	baseInfo.Prosperity = msg->nProsperity;
	MMOGuildManager::getSingleton().NotifyUpdateGuildBaseInfo(baseInfo);

	// Actually the member list (some guild info is refreshed in the member list (member count, merit))
	MMOGuildManager::getSingleton().NotifyUpdateGuildFullMemberList(msg->arrMemberData, msg->byMemberCount);

	// Add own guild info to the query list
	MMOGuildManager::getSingleton().QueryGuildInfo(msg->dwGuildID);

	// 公会公告
	MMOGuildManager::getSingleton().NotifyUpdateGuildNotice(*ReadFixedUTF8(msg->szNotice));

	//Notify info change
	MMOFacade::getSingleton().NotifyPlayerBaseInfoChange();

	// guild event
	MMOGuildManager::getSingleton().NotifyUpdateGuildEvents(msg->stEventInfo.stEventInfo, msg->stEventInfo.byEventCount);
	MMOGuildManager::getSingleton().NotifyUpdateGuildMoneyEvents(msg->stChestInfo.stEvent, msg->stChestInfo.byCount);
	UMMOUIGuildMainPanel* panel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Main);
	if (panel)
	{
		panel->eventCallUpdateViewMoreHistoryState(TRUE);
	}

	// Warehouse permission
	for (INT i = 0; i < GUILD_POSITION_COUNT; ++ i)
	{
		MMOGuildManager::getSingleton().NotifyUpdateGuildStorageRights(i + 1, msg->wStorageRight[i]);
	}

	// Guild variable
	for (INT i = 0; i < MAX_GUILD_VARIABLE_COUNT; ++ i)
	{
		MMOGuildManager::getSingleton().NotifyUpdateGuildAttr(i, msg->arrVariables[i]);
	}

	// Guild salary
	//MMOGuildManager::getSingleton().NotifyUpdateGuildSalaryPercent(msg->bySalaryPercent);

	// guild diplomacy
	MMOGuildManager::getSingleton().NotifyUpdateGuildAllyList(msg->stAlliance, msg->byAllianceCount);
	MMOGuildManager::getSingleton().NotifyUpdateGuildEnemyList(msg->stEnemy, msg->byEnemyCount);

	// guild building
	MMOGuildManager::getSingleton().NotifyUpdateGuildBuildingList(msg->arrBData);
//	MMOGuildManager::getSingleton().NotifyUpdateGuildBuildingUpgradeList(msg->arrBUData);

	// guild welfare
	MMOGuildManager::getSingleton().NotifyUpdateGuildWelfareList(msg->arrWelfareAuraLvl, msg->arrWelfareUpgradeAuraID, msg->arrWelfareAuraUpgradeTime);

	// guild war
//	MMOGuildManager::getSingleton().NotifyGuildWarRelation(msg->dwAttackGuildID, msg->dwDefendGuildID); // "I declared war on" and "declared war on me"
//	MMOGuildManager::getSingleton().NotifyGuildWarCD(msg->nGuildWarAttackTime);

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void CMessageHandler::Parse_m2c_guild_add_history_event_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_ADD_HISTORY_EVENT_NTF* msg = (M2C_GUILD_ADD_HISTORY_EVENT_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ADD_HISTORY_EVENT_NTF))
		return;

	if (msg->byType > GUILD_HE_TYPE_END && msg->byType < GUILD_CHEST_END)
	{
		GUILD_EVENT_INFO eventInfo;
		eventInfo.byType = msg->byType;
		eventInfo.param1 = msg->dwParam1;
		eventInfo.param2 = msg->dwParam2;
		eventInfo.param3 = msg->dwParam3;
		eventInfo.param4 = msg->dwParam4;
		eventInfo.dwTime = msg->dwOccurTime;
		MMOGuildData::getSingleton().AddGuildMoneyEventInfo(eventInfo);
		MMOGuildManager::getSingleton().UpdateGuildMoneyEvents();
	}
	else
	{
		GUILD_EVENT_INFO eventInfo;
		eventInfo.byType = msg->byType;
		eventInfo.param1 = msg->dwParam1;
		eventInfo.param2 = msg->dwParam2;
		eventInfo.param3 = msg->dwParam3;
		eventInfo.param4 = msg->dwParam4;
		eventInfo.dwTime = msg->dwOccurTime;
		MMOGuildData::getSingleton().AddGuildEventInfo(eventInfo);
		MMOGuildManager::getSingleton().UpdateGuildEvents();
	}
}

void CMessageHandler::Parse_m2c_guild_more_history(const char* buf, UINT32 size)
{
	M2C_GUILD_MORE_HISTORY* msg = (M2C_GUILD_MORE_HISTORY*)buf;
	if (!buf || size < sizeof(M2C_GUILD_MORE_HISTORY))
		return;
	if ((UINT32)msg->wEventNum > (size - sizeof(*msg)) / sizeof(msg->stEventInfo[0]))
		return;
	MMOGuildManager::getSingleton().NotifyMoreGuildEvents(msg->stEventInfo, msg->wEventNum);
}

void CMessageHandler::Parse_m2c_apply_to_join_guild_ack(const char* buf, UINT32 size)
{
	M2C_APPLY_TO_JOIN_GUILD_ACK* msg = (M2C_APPLY_TO_JOIN_GUILD_ACK*)buf;
	if (!buf || size < sizeof(M2C_APPLY_TO_JOIN_GUILD_ACK))
		return;
	if (msg->dwGuildID != 0)
	{
		MMOGuildManager::getSingleton().NotifyApplyToJoinGuildSuccess();
	}
}

void CMessageHandler::Parse_m2c_get_apply_guild_list_ack(const char* buf, UINT32 size)
{
	M2C_GET_APPLY_GUILD_LIST_ACK* msg = (M2C_GET_APPLY_GUILD_LIST_ACK*)buf;
	if (!buf || size < sizeof(M2C_GET_APPLY_GUILD_LIST_ACK))
		return;
	if ((UINT32)msg->wAppCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildPendingMemberList(msg->info, msg->wAppCount);
}

void CMessageHandler::Parse_m2c_revert_guild_apply_ack(const char* buf, UINT32 size)
{
	M2C_REVERT_GUILD_APPLY_ACK* msg = (M2C_REVERT_GUILD_APPLY_ACK*)buf;
	if (!buf || size < sizeof(M2C_REVERT_GUILD_APPLY_ACK))
		return;
	// 同意和拒绝都会收到回复
	CMessageHandler::GetInstance().Send_GetApplyGuildList();
}

void CMessageHandler::Parse_m2c_guild_add_member_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_ADD_MEMBER_NTF* msg = (M2C_GUILD_ADD_MEMBER_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ADD_MEMBER_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyAddGuildFullMember(msg->info);
}

void CMessageHandler::Parse_m2c_guild_del_member_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_DEL_MEMBER_NTF* msg = (M2C_GUILD_DEL_MEMBER_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_DEL_MEMBER_NTF))
		return;
	// 服务器不用了
}

void CMessageHandler::Parse_m2c_be_invited_to_join_guild(const char* buf, UINT32 size)
{
	M2C_BE_INVITED_TO_JOIN_GUILD* msg = (M2C_BE_INVITED_TO_JOIN_GUILD*)buf;
	if (!buf || size < sizeof(M2C_BE_INVITED_TO_JOIN_GUILD))
		return;
	MMOGuildManager::getSingleton().NotifyBeInviteToJoinGuild(msg->dwRoleID, *ReadFixedUTF8(msg->szRoleName), *ReadFixedUTF8(msg->szGuildName));
}

void CMessageHandler::Parse_m2c_invite_to_join_guild_ack(const char* buf, UINT32 size)
{
	M2C_INVITE_TO_JOIN_GUILD_ACK* msg = (M2C_INVITE_TO_JOIN_GUILD_ACK*)buf;
	if (!buf || size < sizeof(M2C_INVITE_TO_JOIN_GUILD_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyInviteToJoinGuild(msg->byReplyType, *ReadFixedUTF8(msg->szRoleName));
}

void CMessageHandler::Parse_m2c_disband_guild_ntf(const char* buf, UINT32 size)
{
	M2C_DISBAND_GUILD_NTF* msg = (M2C_DISBAND_GUILD_NTF*)buf;
	if (!buf || size < sizeof(M2C_DISBAND_GUILD_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyDisbandGuild();
}

void CMessageHandler::Parse_m2c_guild_notice_change(const char* buf, UINT32 size)
{
	M2C_GUILD_NOTICE_CHANGE* msg = (M2C_GUILD_NOTICE_CHANGE*)buf;
	if (!buf || size < sizeof(M2C_GUILD_NOTICE_CHANGE))
		return;
	
	if (msg->wSize < sizeof(*msg) || msg->wSize > size ||
		msg->wSize - sizeof(*msg) > GUILD_NOTICE_LEN)
		return;
	const UINT32 noticeBytes = msg->wSize - sizeof(*msg);
	char szTempNotice[GUILD_NOTICE_LEN + 1] = {0};
	if (noticeBytes > 0)
		appMemcpy(szTempNotice, msg->szNotice, noticeBytes);
	MMOGuildManager::getSingleton().NotifyUpdateGuildNotice(UTF8_TO_TCHAR(szTempNotice));
}

void CMessageHandler::Parse_m2c_guild_forbid_chat_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_FORBID_CHAT_NTF* msg = (M2C_GUILD_FORBID_CHAT_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_FORBID_CHAT_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyGuildFullMemberForbitChat(msg->dwRoleID, msg->byOp == GUILD_CHAT_FORBID);
}

void CMessageHandler::Parse_m2c_guild_change_position_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_CHANGE_POSITION_NTF* msg = (M2C_GUILD_CHANGE_POSITION_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_CHANGE_POSITION_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyChangeGuildFullMemberPost(msg->dwRoleID, msg->byPosition);
}

void CMessageHandler::Parse_m2c_guild_role_leave_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_ROLE_LEAVE_NTF* msg = (M2C_GUILD_ROLE_LEAVE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ROLE_LEAVE_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyDelGuildFullMember(msg->dwRoleID);
}

void CMessageHandler::Parse_m2c_guild_role_leave_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_ROLE_LEAVE_ACK* msg = (M2C_GUILD_ROLE_LEAVE_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ROLE_LEAVE_ACK))
		return;
	
	if (msg->byLeaveType == GUILD_LEAVE_ACTIVE)
	{
		MMOGuildManager::getSingleton().NotifyExitGuild();
	}
	else if (msg->byLeaveType == GUILD_LEAVE_PASSIVE)
	{
		MMOGuildManager::getSingleton().NotifyBanishGuild();
	}
}

void CMessageHandler::Parse_m2c_guild_demise_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_DEMISE_NTF* msg = (M2C_GUILD_DEMISE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_DEMISE_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyGuildLeaderDemise(msg->dwOldLeaderID, msg->dwNewLeaderID);
}

void CMessageHandler::Parse_m2c_guild_attr_modify_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_ATTR_MODIFY_NTF* msg = (M2C_GUILD_ATTR_MODIFY_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ATTR_MODIFY_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildAttr(msg->nGuildAttrType, msg->dwGuildAttrValue);
}

void CMessageHandler::Parse_m2c_guild_apply_alliance_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_APPLY_ALLIANCE_ACK* msg = (M2C_GUILD_APPLY_ALLIANCE_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_APPLY_ALLIANCE_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyApplyAllianceSuccess();
}

void CMessageHandler::Parse_m2c_guild_get_apply_union_list_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_GET_APPLY_UNION_LIST_ACK* msg = (M2C_GUILD_GET_APPLY_UNION_LIST_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_GET_APPLY_UNION_LIST_ACK))
		return;
	if ((UINT32)msg->byCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildApplyAllyList(msg->info, msg->byCount);
}

void CMessageHandler::Parse_m2c_guild_reply_union_app_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_REPLY_UNION_APP_ACK* msg = (M2C_GUILD_REPLY_UNION_APP_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_REPLY_UNION_APP_ACK))
		return;
}

void CMessageHandler::Parse_m2c_guild_add_diplomat_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_ADD_DIPLOMAT_NTF* msg = (M2C_GUILD_ADD_DIPLOMAT_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_ADD_DIPLOMAT_NTF))
		return;

	if (msg->byType == GUILD_DIPLOMAT_ALLIANCE)
	{
		MMOGuildManager::getSingleton().NotifyAddGuildAlly(msg->dwGuildID, *ReadFixedUTF8(msg->szGuildName), msg->byLevel, msg->byMemberCount, msg->dwLastTime);
	}
	else
	{
		MMOGuildManager::getSingleton().NotifyAddGuildEnemy(msg->dwGuildID, *ReadFixedUTF8(msg->szGuildName), msg->byLevel, msg->byMemberCount, msg->dwLastTime);
	}

	Send_GuildGetApplyUnionList();
}

void CMessageHandler::Parse_m2c_guild_del_diplomat_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_DEL_DIPLOMAT_NTF* msg = (M2C_GUILD_DEL_DIPLOMAT_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_DEL_DIPLOMAT_NTF))
		return;
	
	if (msg->byType == GUILD_DIPLOMAT_ALLIANCE)
	{
		MMOGuildManager::getSingleton().NotifyDelGuildAlly(msg->dwGuildID);
	}
	else
	{
		MMOGuildManager::getSingleton().NotifyDelGuildEnemy(msg->dwGuildID);
	}

	Send_GuildGetApplyUnionList();
}

void CMessageHandler::Parse_m2c_guild_change_storage_right_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF* msg = (M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_CHANGE_STORAGE_RIGHT_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildStorageRights(msg->byPosition, msg->wRight);
}

void CMessageHandler::Parse_m2c_guild_view_storage_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_VIEW_STORAGE_ACK* msg = (M2C_GUILD_VIEW_STORAGE_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_VIEW_STORAGE_ACK))
		return;

    //TODO
	MMOGuildData::getSingleton().ClearWarehouseItemInfo();

	INT BuffSize = 0;
	if (msg->wItemCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	for ( int i = 0 ; i < msg->wItemCount ; i++ )
	{
		GUILD_STORAGE_ITEM_INFO* guildItemInfo = &msg->info[i];
		if (!guildItemInfo)
		{
			continue;
		}
		
		ItemInfo itemInfo;
		if (!itemInfo.Load(guildItemInfo->szItemBuffer, sizeof(guildItemInfo->szItemBuffer)))
			return;
		
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(itemInfo.m_nTemplateID);
		if (!item)
		{
			continue;
		}
		//BuffSize += Facade::GetItemSize(itemInfo);// sizeof( ITEM_INFO ) + tItemInfo->byAttrCount * sizeof( ITEM_ATTR );
		//ITEM_INFO* itemInfo = (ITEM_INFO*)msg->buff + i*sizeof(ITEM_INFO);
		MMOGuildData::getSingleton().SetWarehouseItemInfo( msg->byStorageTag , guildItemInfo->byItemSlot , &itemInfo  );
	}
	MMOUI::GetUIManager()->delegateFreshWarehouse();
}

void CMessageHandler::Parse_m2c_guild_push_item_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_PUSH_ITEM_ACK* msg = (M2C_GUILD_PUSH_ITEM_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_PUSH_ITEM_ACK))
		return;

	ItemInfo itemInfo;
	if (!itemInfo.Load(msg->buff, size - sizeof(*msg)))
		return;

	MMOGuildData::getSingleton().SetWarehouseItemInfo( msg->byStorageConSlot , msg->byStorageSlot , &itemInfo  );
	MMOUI::GetUIManager()->delegateFreshWarehouse();
}

void CMessageHandler::Parse_m2c_guild_pop_item_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_POP_ITEM_ACK* msg = (M2C_GUILD_POP_ITEM_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_POP_ITEM_ACK))
		return;

	MMOGuildData::getSingleton().SetWarehouseItem(msg->byStorageConSlot, msg->byStorageSlot, NULL);
	MMOUI::GetUIManager()->delegateFreshWarehouse();
}

void CMessageHandler::Parse_m2c_guild_move_item_ack(const char* buf, UINT32 size)
{
	M2C_GUILD_MOVE_ITEM_ACK* msg = (M2C_GUILD_MOVE_ITEM_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_MOVE_ITEM_ACK))
		return;

	MMOGuildData::getSingleton().SwapItem( msg->bySourceConSlot , msg->bySourceSlot , msg->byTargetConSlot , msg->byTargetSlot );
	MMOUI::GetUIManager()->delegateFreshWarehouse();
}

void CMessageHandler::Parse_m2c_sync_social_attr_ntf(const char* buf, UINT32 size)
{
	M2C_SYNC_SOCIAL_ATTR_NTF* msg = (M2C_SYNC_SOCIAL_ATTR_NTF*)buf;
	if (!buf || size < sizeof(M2C_SYNC_SOCIAL_ATTR_NTF))
		return;

    if (msg->byLevel == 0)  
    {// If 0, it is an offline message. Do not update level and map, only update offline time
		UINT32 lastLoginTime = msg->dwValue;
		MMOGuildManager::getSingleton().NotifyUpdateGuildFullMemberData(msg->dwRoleID, 0, 0, lastLoginTime);
    }
    else 
    {// If non-0, it is a level or map update message. No offline time
		UINT32 mapId = msg->dwValue;
		MMOGuildManager::getSingleton().NotifyUpdateGuildFullMemberData(msg->dwRoleID, msg->byLevel, mapId, 0);
    }
}

void CMessageHandler::Parse_g2c_sync_guild_name(const char* buf, UINT32 size)
{
	G2C_SYNC_GUILD_NAME* msg = (G2C_SYNC_GUILD_NAME*)buf;
	if (!buf || size < sizeof(G2C_SYNC_GUILD_NAME))
		return;

	// Add the guild info of the updated name to the query list
	if (msg->dwGuildID > 0)
	{
		MMOGuildManager::getSingleton().NotifyUpdateActorGuildTitle(msg->dwRoleID, TEXT(""), msg->dwGuildID);
	}
}

void CMessageHandler::Parse_m2c_guild_join_guild_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_JOIN_GUILD_NTF* msg = (M2C_GUILD_JOIN_GUILD_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_JOIN_GUILD_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyJoinGuildSuccess(*ReadFixedUTF8(msg->szGuildName));
}

void CMessageHandler::Parse_m2c_guild_upgrade_building_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_UPGRADE_BUILDING_NTF* msg = (M2C_GUILD_UPGRADE_BUILDING_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_UPGRADE_BUILDING_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyUpgradeGuildBuilding(msg->qwNpcNameID, msg->byType, msg->nTime);
}

void CMessageHandler::Parse_m2c_guild_update_building_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_UPDATE_BUILDING_NTF* msg = (M2C_GUILD_UPDATE_BUILDING_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_UPDATE_BUILDING_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildBuilding(msg->qwNpcNameID, msg->byType);
}

void CMessageHandler::Parse_m2c_guild_del_building_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_DEL_BUILDING_NTF* msg = (M2C_GUILD_DEL_BUILDING_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_DEL_BUILDING_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyDestroyGuildBuilding(msg->qwNpcNameID, msg->byType);
}

void CMessageHandler::Parse_m2c_guild_cancel_building_upgrade_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF* msg = (M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_CANCEL_BUILDING_UPGRADE_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyCancelGuildBuildingUpgrade(msg->qwNpcNameID, msg->byType);
}

void CMessageHandler::Parse_m2c_guild_upgrade_welfare_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_UPGRADE_WELFARE_NTF* msg = (M2C_GUILD_UPGRADE_WELFARE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_UPGRADE_WELFARE_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyUpgradeGuildWelfare(msg->byType, msg->nTime);
}

void CMessageHandler::Parse_m2c_guild_update_welfare_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_UPDATE_WELFARE_NTF* msg = (M2C_GUILD_UPDATE_WELFARE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_UPDATE_WELFARE_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildWelfare(msg->byType, msg->byLevel);
}

void CMessageHandler::Parse_m2c_guild_cancel_welfare_upgrade_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF* msg = (M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_CANCEL_WELFARE_UPGRADE_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyCancelGuildWelfareUpgrade(msg->dwID);
}

void CMessageHandler::Parse_g2c_guild_buy_aura_ack(const char* buf, UINT32 size)
{ 
	G2C_GUILD_BUY_AURA_ACK* msg = (G2C_GUILD_BUY_AURA_ACK*)buf;
	if (!buf || size < sizeof(G2C_GUILD_BUY_AURA_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyPurchaseGuildWelfare(msg->byType, msg->byCount);
}

void CMessageHandler::Parse_g2c_sync_purchase_aura_record(const char* buf, UINT32 size)
{ 
	G2C_SYNC_PURCHASE_AURA_RECORD* msg = (G2C_SYNC_PURCHASE_AURA_RECORD*)buf;
	if (!buf || size < sizeof(G2C_SYNC_PURCHASE_AURA_RECORD))
		return;
	if (msg->wCount < 0 ||
		(UINT32)msg->wCount > (size - sizeof(*msg)) / sizeof(msg->data[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildWelfareInfoList(msg->data, msg->wCount);
}

void CMessageHandler::Parse_m2c_guild_get_dungeon_list_ack(const char* buf, UINT32 size)
{ 
	M2C_GUILD_GET_DUNGEON_LIST_ACK* msg = (M2C_GUILD_GET_DUNGEON_LIST_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_GET_DUNGEON_LIST_ACK))
		return;
	if ((UINT32)msg->byDungeonCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildDungeonList(msg->info, msg->byDungeonCount);
}

void CMessageHandler::Parse_m2c_guild_open_dungeon_ack(const char* buf, UINT32 size)
{ 
	M2C_GUILD_OPEN_DUNGEON_ACK* msg = (M2C_GUILD_OPEN_DUNGEON_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_OPEN_DUNGEON_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyOpenDungeonSuccess();
}

void CMessageHandler::Parse_m2c_guild_dungeon_award(const char* buf, UINT32 size)
{ 
	M2C_GUILD_DUNGEON_AWARD* msg = (M2C_GUILD_DUNGEON_AWARD*)buf;
	if (!buf || size < sizeof(M2C_GUILD_DUNGEON_AWARD) ||
		msg->byItemCount > ARRAY_COUNT(msg->arrItem))
		return;
	MMOGuildManager::getSingleton().NotifyDungeonEnd(msg->dwOwnerID, msg->arrItem, msg->byItemCount);
}
/*
void CMessageHandler::Parse_m2c_guild_sync_war_stage_ntf(const char* buf, UINT32 size)
{
    // todo_zhangrui war
	//M2C_GUILD_SYNC_WAR_STAGE_NTF* msg = (M2C_GUILD_SYNC_WAR_STAGE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_SYNC_WAR_STAGE_NTF))
		return;
	//// 战斗双方的名字
	//MMOGuildManager::getSingleton().NotifyGuildWarState(msg->byStage,
	//	msg->dwIntruderID,
	//	UTF8_TO_TCHAR(msg->szIntruderName),
	//	msg->dwDefenderID, 
	//	UTF8_TO_TCHAR(msg->szDefenderName),
	//	msg->nLastTime);
}
*/
void CMessageHandler::Parse_m2c_guild_building_state_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_BUILDING_STATE_NTF* msg = (M2C_GUILD_BUILDING_STATE_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_BUILDING_STATE_NTF))
		return;
	//MMOGuildManager::getSingleton().NotifyGuildBuildingState(msg->qwNameID, msg->byState);    // todo_zhangrui war
}

void CMessageHandler::Parse_m2c_guild_battle_achievement(const char* buf, UINT32 size)
{ 
	M2C_GUILD_BATTLE_ACHIEVEMENT* msg = (M2C_GUILD_BATTLE_ACHIEVEMENT*)buf;
	if (!buf || size < sizeof(M2C_GUILD_BATTLE_ACHIEVEMENT))
		return;
	if ((UINT32)msg->byNum > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	MMOGuildManager::getSingleton().NotifyGuildWarResult(msg->info, msg->byNum);
}

void CMessageHandler::Parse_m2c_guild_member_salary_change(const char* buf, UINT32 size)
{ 
	M2C_GUILD_MEMBER_SALARY_CHANGE* msg = (M2C_GUILD_MEMBER_SALARY_CHANGE*)buf;
	if (!buf || size < sizeof(M2C_GUILD_MEMBER_SALARY_CHANGE))
		return;
	MMOGuildManager::getSingleton().NotifyGuildMemberSalaryChange(msg->dwSalary);
}

void CMessageHandler::Parse_m2c_guild_sync_war_cd_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_SYNC_WAR_CD_NTF* msg = (M2C_GUILD_SYNC_WAR_CD_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_SYNC_WAR_CD_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyGuildWarCD(msg->dwWarCD);
}

void CMessageHandler::Parse_g2c_guild_contribute_ack(const char* buf, UINT32 size)
{ 
	G2C_GUILD_CONTRIBUTE_ACK* msg = (G2C_GUILD_CONTRIBUTE_ACK*)buf;
	if (!buf || size < sizeof(G2C_GUILD_CONTRIBUTE_ACK))
		return;
	// 这里废掉了，统一走M2C_GUILD_CONTRIBUTE_ACK
	//MMOGuildManager::getSingleton().NotifyGuildContributeSuccess(GUILD_ATTR_MONEY, msg->nMoney);
}

void CMessageHandler::Parse_m2c_guild_contribute_ack(const char* buf, UINT32 size)
{ 
	M2C_GUILD_CONTRIBUTE_ACK* msg = (M2C_GUILD_CONTRIBUTE_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_CONTRIBUTE_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyGuildContributeSuccess(msg->nType, msg->nValue);
}

void CMessageHandler::Parse_g2c_guild_draw_salary_ack(const char* buf, UINT32 size)
{ 
	G2C_GUILD_DRAW_SALARY_ACK* msg = (G2C_GUILD_DRAW_SALARY_ACK*)buf;
	if (!buf || size < sizeof(G2C_GUILD_DRAW_SALARY_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyGuildDrawSalarySuccess(msg->nSalary);
}
/*
void CMessageHandler::Parse_m2c_guild_matching_guild_ack(const char* buf, UINT32 size)
{
    // todo_zhangrui war
	//M2C_GUILD_MATCHING_GUILD_ACK* msg = (M2C_GUILD_MATCHING_GUILD_ACK*)buf;
	if (!buf || size < sizeof(M2C_GUILD_MATCHING_GUILD_ACK))
		return;

	//MMOGuildManager::getSingleton().QueryGuildInfo(msg->dwGuildID);
	//// Query member name
	//MMOFacade::getSingleton().RequestPlayerBaseInfo(msg->dwRoleID);

	//GuildViewInfo info;
	//info.Id = msg->dwGuildID;
	//info.LeaderId = msg->dwRoleID; 
	//info.level = msg->byMainLevel;
	//info.Prosperity = msg->nProsperity;
	//info.Announce = UTF8_TO_TCHAR(msg->szAnnounce);
	//appMemCopy(info.BuildingLevel, msg->arrBuildingLevel);
	//MMOGuildManager::getSingleton().NotifyGuildWarSearchBegin(info);
}
*/
void CMessageHandler::Parse_m2c_guild_notify_ntf(const char* buf, UINT32 size)
{ 
	M2C_GUILD_NOTIFY_NTF* msg = (M2C_GUILD_NOTIFY_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_NOTIFY_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyGuildEventsHint(msg->byType);
}

void CMessageHandler::Parse_m2c_guild_more_chest(const char* buf, UINT32 size)
{
	M2C_GUILD_MORE_CHEST* msg = (M2C_GUILD_MORE_CHEST*)buf;
	if (!buf || size < sizeof(M2C_GUILD_MORE_CHEST))
		return;
	if ((UINT32)msg->wEventNum > (size - sizeof(*msg)) / sizeof(msg->stEvent[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildMoneyEvents(msg->stEvent, msg->wEventNum);

}

void CMessageHandler::Parse_m2c_more_guild_info_ack(const char* buf, UINT32 size)
{ 
	M2C_MORE_GUILD_INFO_ACK* msg = (M2C_MORE_GUILD_INFO_ACK*)buf;
	if (!buf || size < sizeof(M2C_MORE_GUILD_INFO_ACK))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateMoreGuildInfo(msg->nMatchWarConsumption, msg->byMatchCount);
}

// Bichon supremacy war
void CMessageHandler::Parse_m2c_enroll_sabac(const char* buf, UINT32 size)
{ 
	M2C_ENROLL_SABAC* msg = (M2C_ENROLL_SABAC*)buf;
	if (!buf || size < sizeof(M2C_ENROLL_SABAC))
		return;
	MMOGuildManager::getSingleton().NotifySabacEnrollGuildId(msg->dwGuildID);
}

void CMessageHandler::Parse_g2c_sync_sabac_enroll_list(const char* buf, UINT32 size)
{ 
	G2C_SYNC_SABAC_ENROLL_LIST* msg = (G2C_SYNC_SABAC_ENROLL_LIST*)buf;
	if (!buf || size < sizeof(G2C_SYNC_SABAC_ENROLL_LIST))
		return;
	if (msg->wSize < sizeof(*msg) || msg->wSize > size ||
		(msg->wSize - sizeof(*msg)) % sizeof(msg->EnrollList[0]) != 0)
		return;
	MMOGuildManager::getSingleton().NotifyUpdateSabacEnrollList(msg->EnrollList, (msg->wSize - sizeof(G2C_SYNC_SABAC_ENROLL_LIST)) / sizeof(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA));
}

void CMessageHandler::Parse_m2c_occupy_sabac(const char* buf, UINT32 size)
{ 
	M2C_OCCUPY_SABAC* msg = (M2C_OCCUPY_SABAC*)buf;
	if (!buf || size < sizeof(M2C_OCCUPY_SABAC))
		return;

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		// Server convention: when old == new is sent, it means Sabak ended
		if (msg->dwOldGuildID == msg->dwNewGuildID)
		{
			MMOGuildManager::getSingleton().NotifySabacEnd(msg->dwOldGuildID, msg->dwNewGuildID);
		}
		else
		{
			MMOGuildManager::getSingleton().NotifyUpdateSabacOccupyGuild(msg->dwNewGuildID, TRUE);
		}
	}
	else
	{
		MMOGuildManager::getSingleton().NotifyUpdateSabacOccupyGuild(msg->dwNewGuildID, FALSE);
	}
}

void CMessageHandler::Parse_g2c_sabac_state_change(const char* buf, UINT32 size)
{ 
	G2C_SABAC_STATE_CHANGE* msg = (G2C_SABAC_STATE_CHANGE*)buf;
	if (!buf || size < sizeof(G2C_SABAC_STATE_CHANGE))
		return;
	MMOGuildManager::getSingleton().NotifySabacState(msg->byStarted);
}

void CMessageHandler::Parse_m2c_sabac_winner_login(const char* buf, UINT32 size)
{ 
	M2C_SABAC_WINNER_LOGIN* msg = (M2C_SABAC_WINNER_LOGIN*)buf;
	if (!buf || size < sizeof(M2C_SABAC_WINNER_LOGIN))
		return;
	MMOGuildManager::getSingleton().NotifySabacWinnerLogin(*ReadFixedUTF8(msg->szRoleName));
}

void CMessageHandler::Parse_m2c_erect_statue(const char* buf, UINT32 size)
{ 
	M2C_ERECT_STATUE* msg = (M2C_ERECT_STATUE*)buf;
	if (!buf || size < sizeof(M2C_ERECT_STATUE))
		return;

	SabacStatue statue;
	statue.RoleId = msg->dwRoleID;
	MMOFacade::getSingleton().RequestPlayerBaseInfo(statue.RoleId, TRUE);
	statue.RoleName = *ReadFixedUTF8(msg->szRoleName);
	statue.RoleClass = msg->byClass;
	statue.RoleSex = msg->bySex;
	statue.HairType = msg->byHairType;
	statue.HairColor = msg->byHairColor;
	statue.FaceType = msg->byFaceType;
	statue.SkinColor = msg->bySkinColor;
	statue.TitleId = msg->byTitleID;
	statue.RefineFx = msg->byRefineFx;
	appMemcpy(statue.Avatar, msg->anAvatar, sizeof(INT32) * NES_VISIBLE_TOTAL);
	MMOGuildData::getSingleton().UpdateStatueData(statue);

	if (MMO::GetCurrentGameState() == EGameState_InGame)
	{
		MMOGuildManager::getSingleton().NotifyCreateStatue(statue);
	}
}

void CMessageHandler::Parse_g2c_sync_guild(const char* buf, UINT32 size)
{
	G2C_SYNC_GUILD* msg = (G2C_SYNC_GUILD*)buf;
	if (!buf || size < sizeof(G2C_SYNC_GUILD))
		return;
	// 服务器这里发的其实是CD的结束时间
	MMOGuildManager::getSingleton().NotifyGuildLeaveCD(msg->dwCD);
}

void CMessageHandler::Parse_m2c_guild_war_record(const char* buf, UINT32 size)
{
	M2C_GUILD_WAR_RECORD* msg = (M2C_GUILD_WAR_RECORD*)buf;
	if (!buf || size < sizeof(M2C_GUILD_WAR_RECORD))
		return;
	if ((UINT32)msg->wCount > (size - sizeof(*msg)) / sizeof(msg->info[0]))
		return;
	MMOGuildManager::getSingleton().NotifyUpdateGuildWarRecords(msg->info, msg->wCount);
}

void CMessageHandler::Parse_m2c_guild_join_war_ntf(const char* buf, UINT32 size)
{
	M2C_GUILD_JOIN_WAR_NTF* msg = (M2C_GUILD_JOIN_WAR_NTF*)buf;
	if (!buf || size < sizeof(M2C_GUILD_JOIN_WAR_NTF))
		return;
	MMOGuildManager::getSingleton().NotifyGuildJoinWar(msg->byWarCharacter, msg->dwTargetGuildID);
}