#ifndef __MESSAGE_HANDLER_ARENA_H__
#define __MESSAGE_HANDLER_ARENA_H__

public:
	void Send_QueryVSRewardInfo();
	void Send_TryGetVSOneOnOneReward();
	void Send_QuitVSOneOnOneWaiting();

public:
	void Send_GetArenaData();
	void Send_ApplyForArena();
	void Send_QuitArena();
	void Send_MatchArenaRound();
	void Send_GetArenaReward();

private:
	M_DECL_PARSE_FUN(g2c_arena_result);
	M_DECL_PARSE_FUN(g2c_arena_status);
	M_DECL_PARSE_FUN(m2c_arena_fund);

#define M_INIT_MSGHANDLER_ARENA	\
	MREG_PARSE_FUN(g2c_arena_result);\
	MREG_PARSE_FUN(g2c_arena_status);\
	MREG_PARSE_FUN(m2c_arena_fund);

//public:
//
//	//void Send_CreateBattleTeam( const FString& teamName , int iconID );
//
//	void Send_PrepareArenaEnter( INT mode , INT size );
//	//Leave while queuing
//	void Send_ArenaQueueLeave(); 
//	void Send_ArenaJoinAck( UBOOL agree );
//
//	//test
//	void TestEnterArenaQueue();
//	void TestLeaveArenaQueue();

//private:
//	M_DECL_PARSE_FUN(m2c_join_arena_queue);
//	M_DECL_PARSE_FUN(m2c_cancel_join_arena_ack);
//	M_DECL_PARSE_FUN(m2c_join_arena_confirm_req);
//	M_DECL_PARSE_FUN(m2c_sync_arena_score_for_team);
//	M_DECL_PARSE_FUN(m2c_sync_arena_score_for_role);
//	//M_DECL_PARSE_FUN(g2c_scene_finish);
//
//#define M_INIT_MSGHANDLER_ARENA \
//	MREG_PARSE_FUN(m2c_join_arena_queue);\
//	MREG_PARSE_FUN(m2c_cancel_join_arena_ack);\
//	MREG_PARSE_FUN(m2c_join_arena_confirm_req);\
//	MREG_PARSE_FUN(m2c_sync_arena_score_for_team);\
//	MREG_PARSE_FUN(m2c_sync_arena_score_for_role);
//
//	//MREG_PARSE_FUN(g2c_scene_finish);

#endif