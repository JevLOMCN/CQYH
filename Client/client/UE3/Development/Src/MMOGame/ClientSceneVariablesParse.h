#ifndef _CLIENTSCENEVARIABLESPARSE_H_
#define _CLIENTSCENEVARIABLESPARSE_H_

#include "net/message_handler.h"

	typedef bool (*SceneVariableActionFun)( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );

	void InitClientSceneVariableActionList();
	void UninitClientSceneVariableActionList();

	bool CheckClientSceneVariableAction( INT mapID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &serverData );

	//
	class ClientSceneVariableActionTemplate
	{
		typedef TMap< UINT, SceneVariableActionFun> FClientSceneVariableActionContainer;
		FClientSceneVariableActionContainer _ActionMap;

	public:

		void AddSceneVariableAction( UINT variableIdx, SceneVariableActionFun fun );
		bool ParseSceneVariable( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &serverData );
	};

	// Default scene handling
	bool Act_Battle_State( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Battle_TeamOneScoreChange( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Battle_TeamTwoScoreChange( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Battle_SyncTime( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	//
	class DefaultSceneVariableAction : public ClientSceneVariableActionTemplate
	{
	public:

		DefaultSceneVariableAction()
		{
			//AddSceneVariableAction( 1, );
		}
	};




	// Scene variable handling for the capture-the-flag battlefield map
	bool Act_Battle_TeamTwoGetFlag( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );//Blue team
	bool Act_Battle_TeamOneGetFlag( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );//Red team
	bool Act_Battle_FlagBattleEvent( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	class BattleFlagSceneVariableAction : public ClientSceneVariableActionTemplate
	{
	public:

		BattleFlagSceneVariableAction()
		{
			AddSceneVariableAction( 0, Act_Battle_State );
			//Blue team score change
			AddSceneVariableAction( 2, Act_Battle_TeamTwoScoreChange );
			//Red team score change
			AddSceneVariableAction( 1, Act_Battle_TeamOneScoreChange );
			//Red team captures flag
			AddSceneVariableAction( 3, Act_Battle_TeamTwoGetFlag );
			//Blue team captures flag
			AddSceneVariableAction( 4, Act_Battle_TeamOneGetFlag );
			//Time sync
			AddSceneVariableAction( 5, Act_Battle_SyncTime );
			//battle event
			AddSceneVariableAction( 10, Act_Battle_FlagBattleEvent );
		}
	};

	//bool Act_Arena_SyncTime( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	//bool Act_Arena_SyncTime( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_Prepare( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_State( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_PlayerCnt( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_Player1( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_Player2( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_PlayerData1( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Arena_PlayerData2( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	class ArenaSceneVariableAction:public ClientSceneVariableActionTemplate
	{
	public:

		ArenaSceneVariableAction()
		{
			AddSceneVariableAction( 2 , Act_Arena_Prepare );
			AddSceneVariableAction( 4 , Act_Arena_State );
			AddSceneVariableAction( 5 , Act_Arena_PlayerCnt );
			AddSceneVariableAction( 6 , Act_Arena_Player1 );
			AddSceneVariableAction( 7 , Act_Arena_Player2 );
			AddSceneVariableAction( 8 , Act_Arena_PlayerData1 );
			AddSceneVariableAction( 9 , Act_Arena_PlayerData2 );
		}

	};

	//Dungeon Scene Variable
//#define IMPLEMENT_ACT_DUGEON_STATE( sceneVariable )			\
//	void Act_Dungeon_State##sceneVariable( INT sceneID,  const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )		\
//	{														\
//		Act_Dungeon_State( sceneID, data );			\
//	}
//
//#define DEFINE_ACT_DUGEON_STATE( sceneVariable )			\
//	void Act_Dungeon_State##sceneVariable( INT sceneID,  const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
//
//	DEFINE_ACT_DUGEON_STATE( 101 );
//	DEFINE_ACT_DUGEON_STATE( 102 );
//	DEFINE_ACT_DUGEON_STATE( 103 );
//	DEFINE_ACT_DUGEON_STATE( 104 );
//	DEFINE_ACT_DUGEON_STATE( 105 );
//	DEFINE_ACT_DUGEON_STATE( 106 );
//	DEFINE_ACT_DUGEON_STATE( 107 );
//	DEFINE_ACT_DUGEON_STATE( 108 );

	bool Act_Dungeon_State( INT sceneID,  const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Dungeon_Achievement( INT sceneID,  const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	bool Act_Dungeon_SyncStartTime( INT sceneID,  const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data );
	class DungeonSceneVariableAction : public ClientSceneVariableActionTemplate
	{
	public:

		DungeonSceneVariableAction()
		{
			AddSceneVariableAction( 0, Act_Dungeon_SyncStartTime );
			//AddSceneVariableAction( 16, Act_Dungeon_State );
			//AddSceneVariableAction( 17, Act_Dungeon_State );
			//AddSceneVariableAction( 18, Act_Dungeon_State );
			//AddSceneVariableAction( 19, Act_Dungeon_State );
			//AddSceneVariableAction( 20, Act_Dungeon_State );
			//AddSceneVariableAction( 21, Act_Dungeon_State );
			//AddSceneVariableAction( 22, Act_Dungeon_State );
			//AddSceneVariableAction( 23, Act_Dungeon_State );
			//AddSceneVariableAction( 24, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 25, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 26, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 27, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 28, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 29, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 30, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 31, Act_Dungeon_Achievement );
			//AddSceneVariableAction( 101, Act_Dungeon_State101 );
			//AddSceneVariableAction( 102, Act_Dungeon_State102 );
			//AddSceneVariableAction( 103, Act_Dungeon_State103 );
			//AddSceneVariableAction( 104, Act_Dungeon_State104 );
			//AddSceneVariableAction( 105, Act_Dungeon_State105 );
			//AddSceneVariableAction( 106, Act_Dungeon_State106 );
			//AddSceneVariableAction( 107, Act_Dungeon_State107 );
			//AddSceneVariableAction( 108, Act_Dungeon_State108 );
		}
	};

	//

#endif
