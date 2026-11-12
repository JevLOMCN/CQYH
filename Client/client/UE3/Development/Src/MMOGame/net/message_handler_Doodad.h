#ifndef __MESSAGE_HANDLER_DOODAD_H__
#define __MESSAGE_HANDLER_DOODAD_H__

public:
	void Send_PickDoodadItem( INT DoodadID );

private:
	M_DECL_PARSE_FUN(g2c_sync_doodad_list);
	M_DECL_PARSE_FUN(g2c_doodad_appear);
	M_DECL_PARSE_FUN(g2c_operate_doodad_begin);
	M_DECL_PARSE_FUN(g2c_operate_doodad_end);
	M_DECL_PARSE_FUN(g2c_sync_doodad_open_count);

//Door mechanism sync
M_DECL_PARSE_FUN(g2c_sync_door_state);


#define M_INIT_MSGHANDLER_DOODAD \
	MREG_PARSE_FUN(g2c_sync_doodad_list); \
	MREG_PARSE_FUN(g2c_doodad_appear);\
	MREG_PARSE_FUN(g2c_operate_doodad_begin); \
	MREG_PARSE_FUN(g2c_operate_doodad_end); \
	MREG_PARSE_FUN(g2c_sync_doodad_open_count); \
	MREG_PARSE_FUN(g2c_sync_door_state); \


#endif