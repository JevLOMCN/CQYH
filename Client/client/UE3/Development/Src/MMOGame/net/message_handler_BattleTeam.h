#ifndef __MESSAGE_HANDLER_BATTLETEAM_H__
#define __MESSAGE_HANDLER_BATTLETEAM_H__

public:
	
	void Send_CreateBattleTeam( const FString& teamName , int iconID );
	//Invite
	void Send_JoinBattleTeam(INT roleID);
	void Send_JoinBattleTeam( FString roleName );
	//Reply to team invitation
	void Send_JoinBattleTeamAnswer(INT roleID);
	void Send_LeaveBattleTeam(INT roleID);
	void Send_RequestBattleTeamInfo( INT teamId );

	//test
	void CreateBattleTeam();
	void InviteBattleTeam( int roleID , FString roleName );	
	void RemoveBattleTeam( int roleID );
	void LeaveBattleTeam();

	//void AcceptJoinBattleTeam(UGFxObject* mc);

private:
M_DECL_PARSE_FUN(m2c_join_arena_team);
M_DECL_PARSE_FUN(m2c_join_arena_team_ask);
M_DECL_PARSE_FUN(m2c_leave_arena_team);
M_DECL_PARSE_FUN(m2c_arena_team_info);
M_DECL_PARSE_FUN(g2c_sync_arena_simple_info);


#define M_INIT_MSGHANDLER_BATTLETEAM \
	MREG_PARSE_FUN(m2c_join_arena_team);\
	MREG_PARSE_FUN(m2c_join_arena_team_ask);\
	MREG_PARSE_FUN(m2c_leave_arena_team);\
	MREG_PARSE_FUN(m2c_arena_team_info);\
	MREG_PARSE_FUN(g2c_sync_arena_simple_info);
	

#endif