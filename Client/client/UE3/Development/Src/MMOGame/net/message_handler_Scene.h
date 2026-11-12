#ifndef __MESSAGE_HANDLER_SCENE_H__
#define __MESSAGE_HANDLER_SCENE_H__

public:
	//void Send_TeamJoin_Battle( INT battleMode, INT battleID );
	//void Send_SingleJoin_Battle( INT battleID );
	//void Send_RandomJoin_Battle( UINT selectedBattleBit );
	//void Send_Require_TeleportNetNodesData();
	void Send_TeleportToNetNodes( UINT8 srcNodeID, UINT8 destNodeID );
	void Send_Join_Battles( UINT selectedBattleBit );
	void Send_Leave_WaitingBattle();
	void Send_Ack_JoinBattle( UBOOL isAgreed );
	void Send_GetFightSummary();
	void Send_UpdateClientVariable(UINT8 index, INT32 value);	// add by zhangrui05@snda.com
	void Send_UpdateClientVarRange(UINT8 idx_start, UINT8 idx_end, BYTE* buf, size_t size);
	void Send_QuerySceneLine();
	void Send_ChangeToSceneLine( UINT8 lineID );
	void Send_CancelMatinee();
	void Do_g2c_changescene_enter();

private:
M_DECL_PARSE_FUN(g2c_sync_frame);
M_DECL_PARSE_FUN(g2c_enter_scene);
M_DECL_PARSE_FUN(g2c_leave_scene);
M_DECL_PARSE_FUN(m2c_query_line_ack);

M_DECL_PARSE_FUN(g2c_changescene_enter);
//M_DECL_PARSE_FUN(g2c_changescene_leave);

//Scene variable sync
M_DECL_PARSE_FUN(g2c_sync_scene_variables);
M_DECL_PARSE_FUN(g2c_sync_role_variables);
M_DECL_PARSE_FUN(g2c_sync_client_variables);
M_DECL_PARSE_FUN(g2c_update_variable);
M_DECL_PARSE_FUN(g2c_sync_flag_pos);

//Teleport point
M_DECL_PARSE_FUN(g2c_sync_teleport_platform);
M_DECL_PARSE_FUN(g2c_active_platform);

//respawn point
M_DECL_PARSE_FUN(g2c_activate_revive_pt);

//Enter dungeon
M_DECL_PARSE_FUN(g2c_query_scene_difficulty);
M_DECL_PARSE_FUN(g2c_select_scene_difficulty);
M_DECL_PARSE_FUN(g2c_scene_finish);
M_DECL_PARSE_FUN(g2c_dungeon_finish_data);
M_DECL_PARSE_FUN(g2c_dungeon_finish_score);
M_DECL_PARSE_FUN(g2c_dungeon_reward);
M_DECL_PARSE_FUN(g2c_sync_dungeon_goal);
//Battlefield
M_DECL_PARSE_FUN(m2c_join_battle_queue);
M_DECL_PARSE_FUN(m2c_cancel_join_battle_ack);
M_DECL_PARSE_FUN(m2c_join_battle_confirm_req);
//Battlefield/arena
M_DECL_PARSE_FUN(g2c_sync_combat_stat);

M_DECL_PARSE_FUN(g2c_sync_next_jump_point);

#define M_INIT_MSGHANDLER_SCENE	\
	MREG_PARSE_FUN(g2c_changescene_enter);		\
    MREG_PARSE_FUN(g2c_sync_frame);			    \
	MREG_PARSE_FUN(g2c_sync_teleport_platform);\
	MREG_PARSE_FUN(g2c_active_platform);\
	MREG_PARSE_FUN(g2c_activate_revive_pt);\
    MREG_PARSE_FUN(g2c_enter_scene);			\
	MREG_PARSE_FUN(g2c_leave_scene);			\
	MREG_PARSE_FUN(m2c_query_line_ack);			\
	MREG_PARSE_FUN(g2c_sync_scene_variables);	\
	MREG_PARSE_FUN(g2c_sync_role_variables);	\
    MREG_PARSE_FUN(g2c_sync_client_variables);  \
	MREG_PARSE_FUN(g2c_update_variable);	\
	MREG_PARSE_FUN(g2c_sync_flag_pos);\
	MREG_PARSE_FUN(g2c_query_scene_difficulty);		\
	MREG_PARSE_FUN(g2c_select_scene_difficulty);\
	MREG_PARSE_FUN(g2c_scene_finish);\
	MREG_PARSE_FUN(g2c_dungeon_finish_data);\
	MREG_PARSE_FUN(g2c_dungeon_finish_score);\
	MREG_PARSE_FUN(g2c_dungeon_reward);\
	MREG_PARSE_FUN(g2c_sync_dungeon_goal);\
	MREG_PARSE_FUN(m2c_join_battle_queue);\
	MREG_PARSE_FUN(m2c_cancel_join_battle_ack);\
	MREG_PARSE_FUN(m2c_join_battle_confirm_req);\
	MREG_PARSE_FUN(g2c_sync_combat_stat); \
	MREG_PARSE_FUN(g2c_sync_next_jump_point);
	

#endif