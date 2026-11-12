#ifndef __MESSAGE_HANDLER_GUILD_H__
#define __MESSAGE_HANDLER_GUILD_H__

public:
	// guild management
	void Send_QueryGuildName(UINT32 id, UINT32 crc);
	void Send_CreateGuild(const FString& name, const FString& announce);
	void Send_ViewGuildList(UINT8 type, UINT32 id);
	void Send_FindGuild(UINT32 id, const FString& name);
	void Send_ApplyToJoinGuild(UINT32 id, const FString& name);
	void Send_GetApplyGuildList();
	void Send_GuildApplyResponse(UINT8 result, UINT32 roleId);
	void Send_InviteToJoinGuild(const FString& name);
	void Send_GuildInviteResponse(UINT8 result, UINT32 roleId);
	void Send_DisbandGuild();
	void Send_ViewMoreEvents();
	void Send_GuildNoticeChange(const FString& notice);
	void Send_GuildAnnounceChange(const FString& announce);
	void Send_GuildForbidChat(UINT8 operate, UINT32 roleId);
	void Send_ChangePosition(UINT32 roleId, UINT8 pos);
	void Send_GuildRoleLeave();
	void Send_GuildBanishRole(UINT32 roleId);
	void Send_GuildDemise(UINT32 roleId);
	void Send_GuildContribute(UINT32 type, UINT32 count);
	void Send_GuildChangeSalaryPercent(UINT8 percent[7]);
	void Send_DrawSalary();
	void Send_GuildDiplomatApp(UINT8 opType, UINT8 timeType, const FString& name);
	void Send_GuildGetApplyUnionList();
	void Send_GuildReplyUnionApp(UINT8 opType, UINT32 id);
	void Send_GuildChangeStorageRight(UINT8 pos, UINT16 right);
	void Send_CancelAlliance(UINT32 id);

	// guild warehouse
	void Send_GuildMoveItem(UINT8 srcConSlot, UINT8 srcSlot, UINT8 dstConSlot, UINT8 dstSlot);
	void Send_GuildPushItem(UINT8 srcConSlot, UINT8 srcSlot, UINT8 dstConSlot, UINT8 dstSlot);
	void Send_GuildPopItem(UINT8 srcConSlot, UINT8 srcSlot, UINT8 dstConSlot, UINT8 dstSlot);
	void Send_RefreshWarehouse(INT tabID);

	// guild territory
	void Send_CreateGuildBuilding(UINT32 npcId, UINT8 type);
	void Send_UpgradeGuildBuilding(UINT8 upgradeType, UINT64 buildingId, INT8 type);
	void Send_DestroyGuildBuilding(UINT8 delType, UINT64 buildingId);
	void Send_CancelGuildBuildingUpgrade(INT8 type, UINT64 buildingId);
	void Send_UpgradeGuildWelfare(INT8 type);
	void Send_CancelGuildWelfareUpgrade(INT8 type);
	void Send_PurchaseGuildWelfare(INT8 type);
	void Send_EnterGuildMap(UINT8 mapType, UINT32 guildId);
	void Send_ChangeWeather(UINT32 weatherType, UINT32 timeType);

	// guild dungeon
	void Send_OpenGuildDungeon(UINT32 dunId, UINT32 leaderId);
	void Send_EnterGuildDungeon(UINT32 dunId);
	void Send_RefreshGuildDungeonList();

	// guild war
	void Send_Challenge(const FString& name);
	void Send_Search();
	void Send_MatchChallenge(UINT32 id);
	void Send_MoreGuildInfo();
	void Send_QueryGuildWarRecord(UINT8 page);

	// guild activity
	void Send_OpenGuildActivity(UINT32 activityId);

M_DECL_PARSE_FUN(m2c_query_guild_name_ack);
M_DECL_PARSE_FUN(m2c_create_guild_ack);
M_DECL_PARSE_FUN(m2c_view_guild_list_ack);
M_DECL_PARSE_FUN(m2c_find_guild_ack);
M_DECL_PARSE_FUN(m2c_guild_info_ntf);
M_DECL_PARSE_FUN(m2c_apply_to_join_guild_ack);
M_DECL_PARSE_FUN(m2c_get_apply_guild_list_ack);
M_DECL_PARSE_FUN(m2c_revert_guild_apply_ack);
M_DECL_PARSE_FUN(m2c_guild_add_member_ntf);
M_DECL_PARSE_FUN(m2c_guild_del_member_ntf);
M_DECL_PARSE_FUN(m2c_be_invited_to_join_guild);
M_DECL_PARSE_FUN(m2c_invite_to_join_guild_ack);
M_DECL_PARSE_FUN(m2c_disband_guild_ntf);
M_DECL_PARSE_FUN(m2c_guild_notice_change);
M_DECL_PARSE_FUN(m2c_guild_forbid_chat_ntf);
M_DECL_PARSE_FUN(m2c_guild_change_position_ntf);
M_DECL_PARSE_FUN(m2c_guild_role_leave_ntf);
M_DECL_PARSE_FUN(m2c_guild_role_leave_ack);
M_DECL_PARSE_FUN(m2c_guild_demise_ntf);
M_DECL_PARSE_FUN(m2c_guild_attr_modify_ntf);
M_DECL_PARSE_FUN(m2c_guild_apply_alliance_ack);
M_DECL_PARSE_FUN(m2c_guild_get_apply_union_list_ack);
M_DECL_PARSE_FUN(m2c_guild_reply_union_app_ack);
M_DECL_PARSE_FUN(m2c_guild_add_diplomat_ntf);
M_DECL_PARSE_FUN(m2c_guild_del_diplomat_ntf);
M_DECL_PARSE_FUN(m2c_guild_change_storage_right_ntf);
M_DECL_PARSE_FUN(m2c_guild_view_storage_ack);
M_DECL_PARSE_FUN(m2c_guild_add_storage_event_ntf);
M_DECL_PARSE_FUN(m2c_guild_push_item_ack);
M_DECL_PARSE_FUN(m2c_guild_pop_item_ack);
M_DECL_PARSE_FUN(m2c_guild_move_item_ack);
M_DECL_PARSE_FUN(m2c_guild_add_history_event_ntf);
M_DECL_PARSE_FUN(m2c_sync_social_attr_ntf);
M_DECL_PARSE_FUN(m2c_guild_storage_event);
M_DECL_PARSE_FUN(g2c_sync_guild_name);
M_DECL_PARSE_FUN(m2c_guild_join_guild_ntf);
M_DECL_PARSE_FUN(m2c_guild_more_history);
M_DECL_PARSE_FUN(m2c_guild_upgrade_welfare_ntf);
M_DECL_PARSE_FUN(m2c_guild_update_welfare_ntf);
M_DECL_PARSE_FUN(m2c_guild_cancel_welfare_upgrade_ntf);
M_DECL_PARSE_FUN(m2c_guild_upgrade_building_ntf);
M_DECL_PARSE_FUN(m2c_guild_update_building_ntf);
M_DECL_PARSE_FUN(m2c_guild_cancel_building_upgrade_ntf);
M_DECL_PARSE_FUN(g2c_guild_buy_aura_ack);
M_DECL_PARSE_FUN(g2c_guild_contribute_ack);
M_DECL_PARSE_FUN(m2c_guild_contribute_ack);
M_DECL_PARSE_FUN(m2c_guild_get_dungeon_list_ack);
M_DECL_PARSE_FUN(m2c_guild_open_dungeon_ack);
M_DECL_PARSE_FUN(m2c_guild_dungeon_award);
M_DECL_PARSE_FUN(m2c_guild_building_state_ntf);
M_DECL_PARSE_FUN(m2c_guild_battle_achievement);
M_DECL_PARSE_FUN(g2c_sync_purchase_aura_record);
M_DECL_PARSE_FUN(m2c_guild_member_salary_change);
M_DECL_PARSE_FUN(m2c_guild_sync_war_cd_ntf);
M_DECL_PARSE_FUN(g2c_guild_draw_salary_ack);
M_DECL_PARSE_FUN(m2c_guild_del_building_ntf);
M_DECL_PARSE_FUN(m2c_guild_notify_ntf);
M_DECL_PARSE_FUN(m2c_guild_more_chest);
M_DECL_PARSE_FUN(m2c_more_guild_info_ack);
M_DECL_PARSE_FUN(m2c_enroll_sabac);
M_DECL_PARSE_FUN(g2c_sync_sabac_enroll_list);
M_DECL_PARSE_FUN(g2c_occupy_sabac);
M_DECL_PARSE_FUN(m2c_occupy_sabac);
M_DECL_PARSE_FUN(g2c_sabac_state_change);
M_DECL_PARSE_FUN(m2c_sabac_winner_login);
M_DECL_PARSE_FUN(m2c_erect_statue);
M_DECL_PARSE_FUN(g2c_sync_guild);
M_DECL_PARSE_FUN(m2c_guild_war_record);
M_DECL_PARSE_FUN(m2c_guild_join_war_ntf);

#define M_INIT_MSGHANDLER_GUILD	\
	MREG_PARSE_FUN(m2c_query_guild_name_ack);\
	MREG_PARSE_FUN(m2c_create_guild_ack); \
	MREG_PARSE_FUN(m2c_view_guild_list_ack); \
	MREG_PARSE_FUN(m2c_find_guild_ack); \
	MREG_PARSE_FUN(m2c_guild_info_ntf); \
	MREG_PARSE_FUN(m2c_apply_to_join_guild_ack); \
	MREG_PARSE_FUN(m2c_get_apply_guild_list_ack); \
	MREG_PARSE_FUN(m2c_revert_guild_apply_ack); \
	MREG_PARSE_FUN(m2c_guild_add_member_ntf); \
	MREG_PARSE_FUN(m2c_guild_del_member_ntf); \
	MREG_PARSE_FUN(m2c_be_invited_to_join_guild); \
	MREG_PARSE_FUN(m2c_invite_to_join_guild_ack); \
	MREG_PARSE_FUN(m2c_disband_guild_ntf); \
	MREG_PARSE_FUN(m2c_guild_notice_change); \
	MREG_PARSE_FUN(m2c_guild_forbid_chat_ntf); \
	MREG_PARSE_FUN(m2c_guild_change_position_ntf); \
	MREG_PARSE_FUN(m2c_guild_role_leave_ntf); \
	MREG_PARSE_FUN(m2c_guild_role_leave_ack); \
	MREG_PARSE_FUN(m2c_guild_demise_ntf); \
	MREG_PARSE_FUN(m2c_guild_attr_modify_ntf); \
	MREG_PARSE_FUN(m2c_guild_apply_alliance_ack); \
	MREG_PARSE_FUN(m2c_guild_get_apply_union_list_ack); \
	MREG_PARSE_FUN(m2c_guild_reply_union_app_ack); \
	MREG_PARSE_FUN(m2c_guild_add_diplomat_ntf); \
	MREG_PARSE_FUN(m2c_guild_del_diplomat_ntf); \
	MREG_PARSE_FUN(m2c_guild_change_storage_right_ntf); \
	MREG_PARSE_FUN(m2c_guild_view_storage_ack); \
	MREG_PARSE_FUN(m2c_guild_push_item_ack); \
	MREG_PARSE_FUN(m2c_guild_pop_item_ack); \
	MREG_PARSE_FUN(m2c_guild_move_item_ack); \
	MREG_PARSE_FUN(m2c_guild_more_history); \
	MREG_PARSE_FUN(m2c_sync_social_attr_ntf);\
	MREG_PARSE_FUN(m2c_guild_storage_event);\
	MREG_PARSE_FUN(m2c_guild_add_storage_event_ntf);\
	MREG_PARSE_FUN(g2c_sync_guild_name);\
	MREG_PARSE_FUN(m2c_guild_join_guild_ntf); \
	MREG_PARSE_FUN(m2c_guild_add_history_event_ntf); \
	MREG_PARSE_FUN(m2c_guild_upgrade_welfare_ntf);  \
	MREG_PARSE_FUN(m2c_guild_update_welfare_ntf); \
	MREG_PARSE_FUN(m2c_guild_cancel_welfare_upgrade_ntf); \
	MREG_PARSE_FUN(m2c_guild_upgrade_building_ntf); \
	MREG_PARSE_FUN(m2c_guild_update_building_ntf); \
	MREG_PARSE_FUN(m2c_guild_cancel_building_upgrade_ntf); \
	MREG_PARSE_FUN(g2c_guild_buy_aura_ack); \
	MREG_PARSE_FUN(g2c_guild_contribute_ack); \
	MREG_PARSE_FUN(m2c_guild_contribute_ack); \
	MREG_PARSE_FUN(m2c_guild_get_dungeon_list_ack); \
	MREG_PARSE_FUN(m2c_guild_open_dungeon_ack); \
	MREG_PARSE_FUN(m2c_guild_dungeon_award); \
	MREG_PARSE_FUN(m2c_guild_building_state_ntf); \
	MREG_PARSE_FUN(m2c_guild_del_building_ntf); \
	MREG_PARSE_FUN(m2c_guild_battle_achievement); \
	MREG_PARSE_FUN(g2c_sync_purchase_aura_record); \
	MREG_PARSE_FUN(m2c_guild_member_salary_change); \
	MREG_PARSE_FUN(m2c_guild_sync_war_cd_ntf); \
	MREG_PARSE_FUN(g2c_guild_draw_salary_ack); \
	MREG_PARSE_FUN(m2c_guild_notify_ntf); \
	MREG_PARSE_FUN(m2c_guild_more_chest); \
	MREG_PARSE_FUN(m2c_more_guild_info_ack); \
	MREG_PARSE_FUN(m2c_occupy_sabac); \
	MREG_PARSE_FUN(g2c_sync_sabac_enroll_list); \
	MREG_PARSE_FUN(m2c_enroll_sabac); \
	MREG_PARSE_FUN(g2c_sabac_state_change); \
	MREG_PARSE_FUN(m2c_sabac_winner_login); \
	MREG_PARSE_FUN(m2c_erect_statue); \
	MREG_PARSE_FUN(g2c_sync_guild); \
	MREG_PARSE_FUN(m2c_guild_war_record); \
	MREG_PARSE_FUN(m2c_guild_join_war_ntf); \

#endif