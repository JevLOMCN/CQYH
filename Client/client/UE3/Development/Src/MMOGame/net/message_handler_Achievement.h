#ifndef __MESSAGE_HANDLER_ACHIEVEMENT_H__
#define __MESSAGE_HANDLER_ACHIEVEMENT_H__

public:
	// Achievement acquisition
    void Send_AchievementComplete(UINT32 id);
	// Achievement comparison
	void Send_AchievementCompare(UINT32 roleId);
	// Get reward items
	void Send_ObtainRewardItem(UINT32 id);
	// Monster kill achievement completed
	void Send_KillNpcAchievementComplete(UINT16 id, UINT8 index);
	// Request guild achievement data
	void Send_GuildAchievement();
	// Claim guild reward
	void Send_ObtainGuildReward(UINT32 id);

M_DECL_PARSE_FUN(g2c_sync_achievement_list);
M_DECL_PARSE_FUN(g2c_complete_achievement);
M_DECL_PARSE_FUN(g2c_compare_achievement);
M_DECL_PARSE_FUN(g2c_reward_achievement);
M_DECL_PARSE_FUN(g2c_update_achievement_variable);
M_DECL_PARSE_FUN(g2c_activate_hankbook);
M_DECL_PARSE_FUN(g2c_sync_kill_npc_quest);
M_DECL_PARSE_FUN(g2c_sync_kill_npc_count);
M_DECL_PARSE_FUN(g2c_complete_kill_npc_quest);
M_DECL_PARSE_FUN(g2c_reset_kill_npc_quest);
M_DECL_PARSE_FUN(g2c_sync_guild_achievement);
M_DECL_PARSE_FUN(m2c_guild_achievement_ack);

#define M_INIT_MSGHANDLER_ACHIEVEMENT	\
	MREG_PARSE_FUN(g2c_sync_achievement_list); \
	MREG_PARSE_FUN(g2c_complete_achievement); \
	MREG_PARSE_FUN(g2c_compare_achievement); \
	MREG_PARSE_FUN(g2c_reward_achievement); \
	MREG_PARSE_FUN(g2c_update_achievement_variable); \
	MREG_PARSE_FUN(g2c_activate_hankbook); \
	MREG_PARSE_FUN(g2c_sync_kill_npc_quest); \
	MREG_PARSE_FUN(g2c_sync_kill_npc_count); \
	MREG_PARSE_FUN(g2c_complete_kill_npc_quest); \
	MREG_PARSE_FUN(g2c_reset_kill_npc_quest); \
	MREG_PARSE_FUN(g2c_sync_guild_achievement); \
	MREG_PARSE_FUN(m2c_guild_achievement_ack); \

#endif