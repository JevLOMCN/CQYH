#include "stdafx.h"
#include "message_handler.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOArenaManager.h"


// The shared message headers no longer declare these two header-only messages (only their ids), so declare them here.
namespace ExternalMessage
{
	struct C2M_ARENA_FUND : MESSAGE_HEADER
	{
	};
	struct C2G_ARENA_REWARD : MESSAGE_HEADER
	{
	};
}

#define OfflineModeTest 0

//====================================================================================================
//1V1 arena
//====================================================================================================
void CMessageHandler::Send_QueryVSRewardInfo()
{
	C2M_ARENA_FUND msg;
	msg.wMsg = c2m_arena_fund;
	_Send(&msg, sizeof(msg));
}
//====================================================================================================
void CMessageHandler::Send_TryGetVSOneOnOneReward()
{
	C2G_ARENA_REWARD msg;
	msg.wMsg = c2g_arena_reward;
	_Send(&msg, sizeof(msg));
}
//====================================================================================================
void CMessageHandler::Send_QuitVSOneOnOneWaiting()
{
	//C2G_ARENA_REWARD msg;
	//msg.wMsg = c2g_arena_reward;
	//_Send(&msg, sizeof(msg));
}
//====================================================================================================
void CMessageHandler::Parse_m2c_arena_fund(const char* buf, UINT32 size)
{
	M2C_ARENA_FUND* msg = (M2C_ARENA_FUND*)buf;

	//
	MMOGameEvent_VSOneOnOneRewardInfo rwdInfoEvt( msg->llTotalFund, msg->llFundPerTicket );
	MMOUI::PostGameEvent( &rwdInfoEvt );
}
//====================================================================================================
void CMessageHandler::Parse_g2c_arena_result(const char* buf, UINT32 size)
{
	G2C_ARENA_RESULT* msg = (G2C_ARENA_RESULT*)buf;

	//
	MMOGameEvent_VSOneOnOneResult vsRstEvt( msg->byWin, msg->nDmg, msg->nRewardMoney, msg->nScore, msg->nTime );
	MMOUI::PostGameEvent( &vsRstEvt );
}
//====================================================================================================
void CMessageHandler::Parse_g2c_arena_status(const char* buf, UINT32 size)
{
	G2C_ARENA_STATUS* msg = (G2C_ARENA_STATUS*)buf;

	//
	MMOGameEvent_OneInt vsStateEvt( GameEvent_VSOneOnOneState, msg->nStatus );
	MMOUI::PostGameEvent( &vsStateEvt );
}
//====================================================================================================
//1V1 arena END
//====================================================================================================

//====================================================================================================
//Arena
//====================================================================================================

void CMessageHandler::Send_GetArenaData(){}
void CMessageHandler::Send_ApplyForArena(){}
void CMessageHandler::Send_QuitArena(){}
void CMessageHandler::Send_MatchArenaRound(){}
void CMessageHandler::Send_GetArenaReward(){}
#if 0
void CMessageHandler::Send_GetArenaData()
{
	//C2M_ARENA_GET_DATA msg;
	//msg.wMsg = c2m_arena_get_data;
	//_Send(&msg, sizeof(msg));
}

//------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_arena_get_data_ack(const char* buf, UINT32 size)
{
	M2C_ARENA_GET_DATA_ACK* msg = (M2C_ARENA_GET_DATA_ACK*)buf;

	//
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	//
	arenaMgr.SetCurArenaMoney( msg->qwTotalFund );
	arenaMgr.SetArenaHighestScore( msg->byChampionMaxVictoryTimes );
	arenaMgr.SetMyWinLevel( msg->byRank );
	arenaMgr.SetMyWinMoney( msg->nRewardMoney );
	arenaMgr.SetMyWinItem( msg->dwRewardItemID );
	//
	MMOGameEvent_Any arenDataUpdateEvt( GameEvent_ArenaDateUpdated );
	MMOUI::PostGameEvent(&arenDataUpdateEvt);
}
//====================================================================================================
void CMessageHandler::Parse_m2c_arena_match_begin_ntf(const char* buf, UINT32 size)
{
	M2C_ARENA_MATCH_BEGIN_NTF* msg = (M2C_ARENA_MATCH_BEGIN_NTF*)buf;

	//
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	//
	EArenaState curArenaSta = arenaMgr.GetCurArenaState();
	if( curArenaSta == EAnaSta_Applied ){
		arenaMgr.SetCurArenaState( EAnaSta_InGame );
	}
}
//====================================================================================================
void CMessageHandler::Parse_m2c_arena_end(const char* buf, UINT32 size)
{
	M2C_ARENA_END* msg = (M2C_ARENA_END*)buf;

	//
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();

	//
	INT curEntCnt = arenaMgr.GetArenaEnterCnt();
	if( curEntCnt > 0 ){
		//Has participated, so there is a reward
		arenaMgr.SetCurArenaState( EAnaSta_ToGetAward );
	}
	else{
		arenaMgr.SetCurArenaState( EAnaSta_EndNoAward );
	}
}
//====================================================================================================
void CMessageHandler::Send_ApplyForArena()
{
	//C2G_ARENA_JOIN_SEASON msg;
	//msg.wMsg = c2g_arena_join_season;
	//_Send(&msg, sizeof(msg));
}
//---------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_arena_join_new_season_ack(const char* buf, UINT32 size)
{
	G2C_ARENA_JOIN_NEW_SEASON_ACK* msg = (G2C_ARENA_JOIN_NEW_SEASON_ACK*)buf;

	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();
	//
	arenaMgr.SetMySpendForArenaByEntCnt( msg->bySeasonNum );
	//
	if( arenaMgr.IsArenaFightingTime() ){
		arenaMgr.SetCurArenaState( EAnaSta_InGame );
	}
	else{
		arenaMgr.SetCurArenaState( EAnaSta_Applied );
	}
}
//====================================================================================================
void CMessageHandler::Send_QuitArena()
{
	//C2G_ARENA_QUIT_SEASON msg;
	//msg.wMsg = c2g_arena_quit_season;
	//_Send(&msg, sizeof(msg));
}
//---------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_arena_quit_season_ack(const char* buf, UINT32 size)
{
	G2C_ARENA_QUIT_SEASON_ACK* msg = (G2C_ARENA_QUIT_SEASON_ACK*)buf;

	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();
	//
	arenaMgr.SetMySpendForArenaByEntCnt( msg->bySeasonNum );
	arenaMgr.SetMyAllTimeScore( msg->byPointVictoryTimes );
	//
	arenaMgr.SetCurArenaState( EAnaSta_WaitApply );
	
}
//====================================================================================================
void CMessageHandler::Send_MatchArenaRound()
{
	//C2G_ARENA_MATCH msg;
	//msg.wMsg = c2g_arena_match;
	//_Send(&msg, sizeof(msg));

	////
	//MMOArenaManager::getSingleton().SetCurArenaState( EAnaSta_WaitNextRound );
}
//====================================================================================================
void CMessageHandler::Send_GetArenaReward()
{
	//C2G_ARENA_GET_REWARD msg;
	//msg.wMsg = c2g_arena_get_reward;
	//_Send(&msg, sizeof(msg));
}
//====================================================================================================
void CMessageHandler::Parse_g2c_arena_sync_data(const char* buf, UINT32 size)
{
	G2C_ARENA_SYNC_DATA* msg = (G2C_ARENA_SYNC_DATA*)buf;

	//
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();
	arenaMgr.ClearAllCurDynamicData();

	//
	arenaMgr.SetMySpendForArenaByEntCnt( msg->bySeasonNum );
	arenaMgr.SetMyCurHighestScore( msg->byCurSeasonVicTime );
	arenaMgr.SetMyAllTimeScore( msg->byPointSeasonVicTime );

	//
	if( arenaMgr.IsArenaApplyTime() ){
		//msg->byIsJJCMatchTime
		if( msg->byIsInSeason ){
			//Already registered, can fight directly
			arenaMgr.SetCurArenaState( EAnaSta_InGame );
		}
		else{
			//Can still register
			arenaMgr.SetCurArenaState( EAnaSta_WaitApply );
		}
	}
	else if( arenaMgr.IsArenaDay() ){
		if( msg->bySeasonNum > 0 ){
			//Reward not yet claimed
			arenaMgr.SetCurArenaState( EAnaSta_ToGetAward );
		}
		else{
			//Reward claimed
			arenaMgr.SetCurArenaState( EAnaSta_EndNoAward );
		}
	}

}
//====================================================================================================
void CMessageHandler::Parse_g2c_arena_win(const char* buf, UINT32 size)
{
	G2C_ARENA_WIN* msg = (G2C_ARENA_WIN*)buf;

	MMOGameEvent_OneInt arenWinResult( GameEvent_ArenaRoundResult, TRUE );
	MMOUI::PostGameEvent( &arenWinResult );
}
//====================================================================================================
void CMessageHandler::Parse_g2c_arena_defeat(const char* buf, UINT32 size)
{
	G2C_ARENA_DEFEAT* msg = (G2C_ARENA_DEFEAT*)buf;

	MMOGameEvent_OneInt arenWinResult( GameEvent_ArenaRoundResult, FALSE );
	MMOUI::PostGameEvent( &arenWinResult );
}
#endif

//=====================================================================================================
//=====================================================================================================
#if 0
void CMessageHandler::TestEnterArenaQueue( )
{
	MMOArenaManager::getSingleton().OpenArenaQueue( 0 );
}

void CMessageHandler::TestLeaveArenaQueue()
{
	MMOUI::GetUIManager()->delegateLeaveArenaQueue();
}

void CMessageHandler::Send_PrepareArenaEnter( INT mode , INT size )
{
#if OfflineModeTest 
	
	TestEnterArenaQueue( );
	return;
#endif
	
	C2M_JOIN_ARENA msg;
	msg.byType = mode;
	msg.wMsg = c2m_join_arena;	
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ArenaQueueLeave()
{
#if OfflineModeTest 
	
		TestLeaveArenaQueue();
		return;
#endif

	C2M_CANCEL_JOIN_ARENA_REQ msg;
	msg.wMsg = c2m_cancel_join_arena_req;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_ArenaJoinAck( UBOOL agree )
{
	C2M_JOIN_ARENA_CONFIRM_ACK msg;
	msg.wMsg =	c2m_join_arena_confirm_ack;
	msg.byResult = agree?etarAgree:etarDisAgree;
	_Send(&msg, sizeof(msg));
}

//Enter queue
void CMessageHandler::Parse_m2c_join_arena_queue(const char* buf, UINT32 size)
{
	M2C_JOIN_ARENA_QUEUE* msg = (M2C_JOIN_ARENA_QUEUE*)buf;
	
	MMOArenaManager::getSingleton().OpenArenaQueue( msg->dwAverageQueueTime );
	
}

void CMessageHandler::Parse_m2c_cancel_join_arena_ack(const char* buf, UINT32 size)
{
	M2C_CANCEL_JOIN_ARENA_ACK* msg = (M2C_CANCEL_JOIN_ARENA_ACK*)buf;
	
	MMOUI::GetUIManager()->delegateLeaveArenaQueue();
}

//Entered directly
void CMessageHandler::Parse_m2c_join_arena_confirm_req(const char* buf, UINT32 size)
{
	M2C_JOIN_ARENA_CONFIRM_REQ* msg = (M2C_JOIN_ARENA_CONFIRM_REQ*)buf;
	MMOArenaManager::getSingleton().OpenArenaQueue( 0 );
	MMOUI::GetUIManager()->delegateAskEnterArena();
}

void CMessageHandler::Parse_g2c_scene_finish(const char* buf, UINT32 size)
{

}

void CMessageHandler::Parse_m2c_sync_arena_score_for_team(const char* buf, UINT32 size)
{
	//M2C_SYNC_ARENA_SCORE_FOR_TEAM* msg = (M2C_SYNC_ARENA_SCORE_FOR_TEAM*)buf;
	//msg->dwTeamID_win
	//Currently cannot do this
	//msg->
}

void CMessageHandler::Parse_m2c_sync_arena_score_for_role(const char* buf, UINT32 size)
{
	M2C_SYNC_ARENA_SCORE_FOR_ROLE* msg = (M2C_SYNC_ARENA_SCORE_FOR_ROLE*)buf;
	
	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaScore , TRUE,"" );

	UMMOUIArenaScore* score = Cast<UMMOUIArenaScore>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_PVP_ArenaScore ) );
	if(score)
	{
		score->OpenArenaScoreSingle( msg->dwRoleID_win , msg->dwRoleID_lose , msg->nScoreGain , msg->nCurrentScoreWin , msg->nCurrentScoreLose );
	}

	MMOArenaManager::getSingleton().SetSingleScoreData( TRUE , msg->dwRoleID_win , msg->nCurrentScoreWin , msg->nScoreGain );
	MMOArenaManager::getSingleton().SetSingleScoreData( FALSE , msg->dwRoleID_lose , msg->nCurrentScoreLose , msg->nScoreGain );
	CMessageHandler::GetInstance().Send_GetFightSummary();

}
#endif