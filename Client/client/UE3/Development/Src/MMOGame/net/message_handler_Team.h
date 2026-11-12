#ifndef __MESSAGE_HANDLER_TEAM_H__
#define __MESSAGE_HANDLER_TEAM_H__



	public:
    //team
    void Send_EnterTeam( INT roleId);
    void Send_CreateTeam( const FString& TeamName,INT AssignType,INT TeamMaxCount );
	void Send_TeamLeave( INT RoleId );
    //void Send_GetTeamList(  INT PageNum);
    void Send_TeamAnswer( INT  RoleId , UBOOL Agree, INT TeamOperateType);
    void Send_OperateTeam( INT RoleId , INT TeamOperateType, INT TeamMemberLimit , FString TeamName , INT auctionItemColor );
	void Request_TeamList();
	void Request_PlayerList();
	void Send_QueryTeam(UINT32 roleId);
	void Send_TeamInvite(UINT32 roleId,UINT8 AssignType);

	private:
		//team
		M_DECL_PARSE_FUN(m2c_team_join);
		M_DECL_PARSE_FUN(m2c_team_add_member  ); 
		M_DECL_PARSE_FUN(m2c_team_leave );
		M_DECL_PARSE_FUN(m2c_team_remove_member); 
		/*M_DECL_PARSE_FUN(m2c_team_sync_memberinfo);*/
		M_DECL_PARSE_FUN(m2c_team_changeinfo ); 
		M_DECL_PARSE_FUN(m2c_team_answer); 
		/*M_DECL_PARSE_FUN(g2c_sync_teaminfo ); */
		//M_DECL_PARSE_FUN(g2c_sync_teammember);//The server does not use this protocol yet
		M_DECL_PARSE_FUN(g2c_team_sync_member);
		M_DECL_PARSE_FUN(m2c_team_sync_memberinfo);
		M_DECL_PARSE_FUN(g2c_sync_adjacent_role_noteam);
		M_DECL_PARSE_FUN(g2c_sync_adjacent_team);
		M_DECL_PARSE_FUN(g2c_sync_role_team);
		M_DECL_PARSE_FUN(m2c_team_sync_memberstate);
		M_DECL_PARSE_FUN(g2c_battle_team_change_leader);
		M_DECL_PARSE_FUN(m2c_team_sync_team_info);
        /*MREG_PARSE_FUN(m2c_team_sync_memberinfo);\ team modify*/
        /*MREG_PARSE_FUN(g2c_sync_teaminfo);\ team modify*/
#define M_INIT_MSGHANDLER_TEAM\
		MREG_PARSE_FUN(m2c_team_join);\
		MREG_PARSE_FUN(m2c_team_add_member);\
		MREG_PARSE_FUN(m2c_team_leave);\
		MREG_PARSE_FUN(m2c_team_remove_member);\
		MREG_PARSE_FUN(m2c_team_changeinfo);\
		MREG_PARSE_FUN(m2c_team_answer); \
		MREG_PARSE_FUN(g2c_team_sync_member);\
		MREG_PARSE_FUN(m2c_team_sync_memberinfo);\
		MREG_PARSE_FUN(g2c_sync_adjacent_role_noteam);\
		MREG_PARSE_FUN(g2c_sync_adjacent_team);\
		MREG_PARSE_FUN(g2c_sync_role_team);\
		MREG_PARSE_FUN(m2c_team_sync_memberstate);\
		MREG_PARSE_FUN(m2c_team_sync_team_info);\
		MREG_PARSE_FUN(g2c_battle_team_change_leader);

#endif