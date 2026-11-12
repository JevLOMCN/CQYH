#include "stdafx.h"
#include "ClientSceneVariablesParse.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOBattleManager.h"
#include "MMOChatManager.h"
#include "MMOMapDataManager.h"
#include "MMOArenaManager.h"
using namespace ExternalMessage;

//mapID is the key
typedef TMap< INT, ClientSceneVariableActionTemplate > ClientSceneVariableActionMap;

ClientSceneVariableActionMap g_ClientSceneVariableActionMap;

void InitClientSceneVariableActionList()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("InitClientSceneVariableActionList"), 0);

	//Normal dungeon
	DungeonSceneVariableAction dungeonAction;
	g_ClientSceneVariableActionMap.FindOrAdd( ESceneVType_Dungeon ) = dungeonAction;
	//Capture-the-flag battlefield
	BattleFlagSceneVariableAction battleFlagAction;
	g_ClientSceneVariableActionMap.FindOrAdd( ESceneVType_FlagBattle ) = battleFlagAction;//103
	//Arena
	ArenaSceneVariableAction arenaAction;
	g_ClientSceneVariableActionMap.FindOrAdd( ESceneVType_Arena ) = arenaAction;
}

void UninitClientSceneVariableActionList()
{
	g_ClientSceneVariableActionMap.Empty();
}

bool CheckClientSceneVariableAction( INT mapID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &serverData )
{
	
	FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( mapID );
	if( mapInfo == NULL ){
		return false;

	}

	const ClientSceneVariableActionTemplate *pSceneAction = g_ClientSceneVariableActionMap.Find( mapInfo->ClientSceneVariableType );
	if( pSceneAction == NULL ){
		return false;
	}

	ClientSceneVariableActionTemplate &sceneActions = g_ClientSceneVariableActionMap.FindOrAdd( mapInfo->ClientSceneVariableType );
	return sceneActions.ParseSceneVariable( mapID, serverData );
}

//
void ClientSceneVariableActionTemplate::AddSceneVariableAction( UINT variableIdx, SceneVariableActionFun fun ){
	_ActionMap.FindOrAdd( variableIdx ) = fun;
}

//
bool ClientSceneVariableActionTemplate::ParseSceneVariable( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &serverData )
{
	const SceneVariableActionFun *pActionFun = _ActionMap.Find( serverData.wVarIndex );
	if( pActionFun == NULL ){
		return false;
	}

	return (**pActionFun)( sceneID, serverData );
}

//
bool Act_Battle_State( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT value = data.nValue;

	UMMOUIBattleSummary *pBattleSummary = (UMMOUIBattleSummary*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleSummary );

	if( pBattleSummary == NULL ){
		warnf( NAME_Error, TEXT("Panel_BattleSummary_BattleSummary not found at Act_Battle_State %d"), value );
		return false;
	}

	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	if( pBattleMgr == NULL ){
		return false;
	}

	switch( value )
	{
	//Undefined
	case 0:
		break;
	//Battle starts
	case 1:
		{
			MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
			pBattleMgr->SetBattleTeamScore( EBTLTMS_Team1, 0 );
			pBattleMgr->SetBattleTeamScore( EBTLTMS_Team2, 0 );
			pBattleMgr->SetBattleState( EBTLS_FIGHTING );

			CMessageHandler::GetInstance().Send_GetFightSummary();
			//
			UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );

			if( pBattleScoreBoard != NULL ){
				pBattleScoreBoard->UpdateScore( EBTLTMS_Team1 );
				pBattleScoreBoard->UpdateScore( EBTLTMS_Team2 );

				pBattleScoreBoard->eventForceStopBattleTime();
				//FBattleTemplate* pBattle = pBattleMgr->GetEnteredBattle();
				//if( pBattle != NULL ){
				//	FExportMap *pBattleMap = pBattleMgr->GetEnteredBattleMapData();
				//	if( pBattleMap != NULL ){
				//		pBattleScoreBoard->eventBeginBattleTime( pBattleMap->StandardTime * 60 );
				//	}
				//}
			}//end of if pBattleScoreBoard
			else{
				warnf( NAME_Error, TEXT("Panel_BattleSummary_BattleScoreBoard not found at Act_Battle_State %d"), value );
			}
		}
		break;
	//Red team wins
	case 2:
		{
			pBattleMgr->SetBattleState( EBTLS_ENDING );
			pBattleSummary->NeedToShow();
			pBattleMgr->NotifyBattleEnd( FALSE, EBTLTMS_Team1 );
		}
		break;
	//Blue team wins
	case 3:
		{
			pBattleMgr->SetBattleState( EBTLS_ENDING );
			pBattleSummary->NeedToShow();
			pBattleMgr->NotifyBattleEnd( FALSE, EBTLTMS_Team2 );
		}
		break;
	//Draw
	case 4:
		{
			pBattleMgr->SetBattleState( EBTLS_ENDING );
			pBattleSummary->NeedToShow();
			pBattleMgr->NotifyBattleEnd( TRUE, -1 );
		}
		break;
	}

	return true;
}

//
bool Act_Battle_TeamOneScoreChange( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT value = data.nValue;

	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	pBattleMgr->SetBattleTeamScore( EBTLTMS_Team1, value );
	warnf( TEXT("Act_Battle_TeamTwoScoreChange [EBTLTMS_Team1] Score:%d"), value );
	//
	UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );
	UMMOUIBattleSummary *pBattleSummary = (UMMOUIBattleSummary*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleSummary );

	if( pBattleScoreBoard != NULL ){
		pBattleScoreBoard->UpdateScore( EBTLTMS_Team1 );
	}
	else{
		warnf( NAME_Error, TEXT("Panel_BattleSummary_BattleScoreBoard not found at Act_Battle_TeamOneScoreChange %d"), value );
		return true;
	}

	//
	if( pBattleSummary != NULL ){
		pBattleSummary->UpdateTeamScore( EBTLTMS_Team1 );
	}

	return true;
}

//
bool Act_Battle_TeamTwoScoreChange( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT value = data.nValue;

	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	pBattleMgr->SetBattleTeamScore( EBTLTMS_Team2, value );
	warnf( TEXT("Act_Battle_TeamTwoScoreChange [EBTLTMS_Team2] Score:%d"), value );

	//
	UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );
	UMMOUIBattleSummary *pBattleSummary = (UMMOUIBattleSummary*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleSummary );

	if( pBattleScoreBoard != NULL ){
		pBattleScoreBoard->UpdateScore( EBTLTMS_Team2 );
	}
	else{
		warnf( NAME_Error, TEXT("Panel_BattleSummary_BattleScoreBoard not found at Act_Battle_TeamTwoScoreChange %d"), value );
		return true;
	}

	//
	if( pBattleSummary != NULL ){
		pBattleSummary->UpdateTeamScore( EBTLTMS_Team2 );
	}

	return true;
}



//Sync time
bool Act_Battle_SyncTime( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT SixtySecBeforeStartTime = data.nValue;
	MMOBattleManager &battleMgr = MMOBattleManager::getSingleton();
	battleMgr.SetBattleStartTime( SixtySecBeforeStartTime, 60 );

	warnf( NAME_Warning, TEXT("Act_Battle_SyncTime  SixtySecBeforeStartTime:%d"), SixtySecBeforeStartTime );
	INT sinceStartTimeSec = battleMgr.GetTimeSinceBattleBegin();

	//
	UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );
	if( pBattleScoreBoard != NULL ){// && !pBattleScoreBoard->IsTimeCountDown() ){
		FBattleTemplate* pBattle = battleMgr.GetEnteredBattle();
		if( pBattle != NULL ){
			FExportMap *pBattleMap = battleMgr.GetEnteredBattleMapData();
			if( pBattleMap != NULL && sinceStartTimeSec >= 0 ){
				INT leftSecond = pBattle->ContinueTime / 1000 - sinceStartTimeSec;
				if( leftSecond < 0 ){
					leftSecond = 0;
				}
				//
				warnf( NAME_Warning, TEXT("Act_Battle_SyncTime  sinceStartTime:%d"), sinceStartTimeSec );
				pBattleScoreBoard->eventBeginBattleTime( leftSecond );
			}
		}
	}//end of if pBattleScoreBoard

	return true;
}

//
bool Act_Battle_TeamTwoGetFlag( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	warnf( TEXT("Act_Battle_TeamTwoGetFlag [%d]"), data.nValue );
	return true;
}


bool Act_Battle_TeamOneGetFlag( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	warnf( TEXT("Act_Battle_TeamOneGetFlag [%d]"), data.nValue );
	return true;
}

bool Act_Battle_FlagBattleEvent( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	//FString textKey = TEXT("MMOGame.BattleField.BattleEvent");
	//textKey += appItoa( value );
	//FString hint = MMOUI::ParseLocalizedPropertyPath( textKey );
	//if( !hint.IsEmpty() ){
	//	MMOChatManager::getSingleton().AddSystemMessage( hint );
	//}

	//
	INT value = data.nValue;
	switch( value )
	{
	//before fighting 10sec
	case 3:
		{
			//UMMOGFXPlayer_BattleSummary *pMoviePlayer = (UMMOGFXPlayer_BattleSummary*)MMOUI::FindMoviePlayer( MMOUI::GetUIManager()->MoviePlayer_BattleSummary );
			//if( pMoviePlayer== NULL ){
			//	break;
			//}
			UMMOUIFightingScreenHint *pFightingScreenHint = (UMMOUIFightingScreenHint*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint );

			if( pFightingScreenHint == NULL ){
				warnf( NAME_Error, TEXT("Panel_FightingScreenHint not found at Act_Battle_FlagBattleEvent %d"), value );
				break;
			}

			pFightingScreenHint->Show( TRUE );
			pFightingScreenHint->PlayTenSecCountDownAfter();
		}
		break;

	//fighting start
	case 4:
		{
			MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
			INT sinceStartTimeSec = pBattleMgr->GetTimeSinceBattleBegin();

			//
			UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );
			if( pBattleScoreBoard != NULL ){// && !pBattleScoreBoard->IsTimeCountDown() ){
				FBattleTemplate* pBattle = pBattleMgr->GetEnteredBattle();
				if( pBattle != NULL ){
					FExportMap *pBattleMap = pBattleMgr->GetEnteredBattleMapData();
					if( pBattleMap != NULL ){
						INT leftSecond = pBattle->ContinueTime / 1000 - sinceStartTimeSec;
						if( leftSecond < 0 ){
							leftSecond = 0;
						}
						warnf( NAME_Warning, TEXT("Act_Battle_FlagBattleEvent [fighting start] sinceStartTime:%d"), sinceStartTimeSec );
						pBattleScoreBoard->eventBeginBattleTime( leftSecond );
					}
				}
			}//end of if pBattleScoreBoard
		}
		break;

	//Red team's magic disc dropped on the ground
	case 9:
		{
			TArray<FActorInMapUI*> actorArray;
			MMOMapDataManager::getSingleton().FindMapDymcActorGlobalOfType( EACTMUI_BattleFlag, actorArray );
			INT cnt = actorArray.Num();
			for( INT i = 0; i < cnt; ++i ){
				FActorInMapUI *pActor = actorArray(i);
				if( pActor->_ActorID != EBTLTMS_Team1 ){
					continue;
				}
				pActor->_AidID = -1;

				//
				AMMOActor* nearActor = MMOFacade::getSingleton().GetCharacter( pActor->_AidID );

				if( nearActor != NULL ){
					pActor->_OrigInMapPos = nearActor->Location;
				}

				break;
			}
		}
		break;
	//Red team's magic disc was returned to base
	case 13:
		{
			MMOMapDataManager::getSingleton().RemoveMapDymcActorGlobalOfType( EACTMUI_BattleFlag, EBTLTMS_Team1 );
		}
		break;

	//Blue team's magic disc dropped on the ground
	case 10:
		{
			TArray<FActorInMapUI*> actorArray;
			MMOMapDataManager::getSingleton().FindMapDymcActorGlobalOfType( EACTMUI_BattleFlag, actorArray );
			INT cnt = actorArray.Num();
			for( INT i = 0; i < cnt; ++i ){
				FActorInMapUI *pActor = actorArray(i);
				if( pActor->_ActorID != EBTLTMS_Team2 ){
					continue;
				}
				pActor->_AidID = -1;

				//
				AMMOActor* nearActor = MMOFacade::getSingleton().GetCharacter( pActor->_AidID );

				if( nearActor != NULL ){
					pActor->_OrigInMapPos = nearActor->Location;
				}

				break;
			}
		}
		break;
	//Blue team's magic disc was returned to base
	case 14:
		{
			MMOMapDataManager::getSingleton().RemoveMapDymcActorGlobalOfType( EACTMUI_BattleFlag, EBTLTMS_Team2 );
		}
		break;
	//Red team captured the blue team's magic disc, red team total score +100
	case 11:
		{
			MMOMapDataManager::getSingleton().RemoveMapDymcActorGlobalOfType( EACTMUI_BattleFlag, EBTLTMS_Team1 );
			MMOMapDataManager::getSingleton().RemoveMapDymcActorGlobalOfType( EACTMUI_BattleFlag, EBTLTMS_Team2 );
		}
		break;
	//Blue team captured the red team's magic disc, blue team total score +100
	case 12:
		{
			MMOMapDataManager::getSingleton().RemoveMapDymcActorGlobalOfType( EACTMUI_BattleFlag, EBTLTMS_Team1 );
			MMOMapDataManager::getSingleton().RemoveMapDymcActorGlobalOfType( EACTMUI_BattleFlag, EBTLTMS_Team2 );
		}
		break;
	default:
		break;
	}

	//if( !hint.IsEmpty() && value != 3 && value != 4){
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( hint );
	//}
	return true;
}

//==========================================================================================================================
//Dungeon instance
//==========================================================================================================================
//Sync time
bool Act_Dungeon_SyncStartTime( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
	const FExportMap& curScene = mapDataMgr.GetCurMap();
	if( curScene.ServerID != sceneID ){
		warnf( NAME_Warning, TEXT("Act_Dungeon_SyncStartTime Scene[%d] is NOT current scene"), sceneID );
		return false;
	}

	UINT32 serverDungeonStartTime = (UINT32)data.nValue;
	mapDataMgr.SetDungeonStartTime( serverDungeonStartTime );

	warnf( NAME_Warning, TEXT("Act_Dungeon_SyncStartTime AT:%d"), serverDungeonStartTime );
	INT sinceStartTimeSec = mapDataMgr.GetTimeSinceDungeonBegin();

	//
	UMMOUIMinimap *pMinimap = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( pMinimap != NULL && pMinimap->IsVisible ){
		const FDungeonInfo *fbInfo = mapDataMgr.GetDungeonInfo( curScene.ServerID, mapDataMgr.GetCurFBDiff() );
		if( fbInfo != NULL && fbInfo->StandardTime != 0 ){
			INT leftSecond = fbInfo->StandardTime / 1000;
			if( fbInfo->StandardTime > 0 ){
				leftSecond -= sinceStartTimeSec;
				if( leftSecond < 0 ){
					leftSecond = 0;
				}
			}
			else{//Count up
				leftSecond = -sinceStartTimeSec;
			}
			//
			warnf( NAME_Warning, TEXT("Act_Dungeon_SyncStartTime  sinceStartTime:%d"), sinceStartTimeSec );
			pMinimap->ShowMapTimeCount( leftSecond );
		}
	}

	return true;
}
//
bool Act_Dungeon_State( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	//const FExportMap *pCurScene = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( sceneID );//MMOMapDataManager::getSingleton().GetCurMap();
	//if( pCurScene == NULL ){
	//	warnf( NAME_Warning, TEXT("Act_Dungeon_State Scene[%d] NOT found"), sceneID );
	//}
	const FExportMap& curScene = MMOMapDataManager::getSingleton().GetCurMap();
	if( curScene.ServerID != sceneID ){
		warnf( NAME_Warning, TEXT("Act_Dungeon_State Scene[%d] is NOT current scene"), sceneID );
		return false;
	}

	TArray<CurSceneTarget> &curSceneTargets = MMOMapDataManager::getSingleton().GetCurSceneProgress();

	INT variableID = data.wVarIndex;
	CurSceneProgress *notifySceneTarget = NULL;
	INT mainProgressCnt = curSceneTargets.Num();
	UBOOL mainTargetIdx = -1;
	UBOOL isMainProgress = FALSE;
	for( INT i = 0; i < mainProgressCnt; ++i ){
		CurSceneTarget &mainProgress = curSceneTargets(i);
		if( mainProgress._MainTarget._Template->ID == variableID ){
			notifySceneTarget = &mainProgress._MainTarget;

			mainTargetIdx = i;
			isMainProgress = TRUE;
			break;
		}

		for( INT j = 0; j < UCONST_MaxSceneMinorTarget; ++j ){
			if( mainProgress._ChildTargets[j]._Template->ID == variableID ){
				notifySceneTarget = &mainProgress._ChildTargets[j];

				mainTargetIdx = i;
				break;
			}
		}
	}

	//
	if( notifySceneTarget == NULL || mainTargetIdx < 0 ){
		warnf( NAME_Warning, TEXT("Act_Dungeon_State SceneVariable[%d] NOT Found in current scene"), variableID );
		return false;
	}

	//
	if( isMainProgress == TRUE ){
		if( data.nValue > 0 ){
			UMMOUIFBTask *pFBTask = (UMMOUIFBTask*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBTask );
			if( pFBTask != NULL ){
				pFBTask->UpdateFBTaskList( mainTargetIdx );

				if( !pFBTask->IsVisible ){
					pFBTask->Show( TRUE );
				}
			}
		}
		else if( data.nValue <= 0 ){
			UMMOUIFBTask *pFBTask = (UMMOUIFBTask*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBTask );
			if( pFBTask != NULL && pFBTask->IsVisible){
				pFBTask->Show( FALSE );
			}
		}
	}
	else{
		notifySceneTarget->_CurValue = data.nValue;

		//Calculate the main condition's completion by counting sub-conditions
		CurSceneTarget &mainProgress = curSceneTargets(mainTargetIdx);
		INT completeChildProgressCnt = 0;
		for( INT j = 0; j < UCONST_MaxSceneMinorTarget; ++j ){
			const FSceneProgress *childProgress = mainProgress._ChildTargets[j]._Template;
			if( childProgress->ID >= 0 && mainProgress._ChildTargets[j]._CurValue >= childProgress->TargetValue ){
				notifySceneTarget = &mainProgress._ChildTargets[j];

				++completeChildProgressCnt;
			}
		}
		mainProgress._MainTarget._CurValue = completeChildProgressCnt;

		UMMOUIFBTask *pFBTask = (UMMOUIFBTask*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBTask );
		if( pFBTask != NULL ){
			pFBTask->UpdateFBTaskList( mainTargetIdx );

			if( !pFBTask->IsVisible ){
				pFBTask->Show( TRUE );
			}
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------
bool Act_Dungeon_Achievement( INT sceneID,  const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	const FExportMap& curScene = MMOMapDataManager::getSingleton().GetCurMap();
	if( curScene.ServerID != sceneID ){
		warnf( NAME_Warning, TEXT("Act_Dungeon_Achievement Scene[%d] is NOT current scene"), sceneID );
		return false;
	}

	//
	INT fbAchievementID = data.wVarIndex;
	FBAchievementsState *curFBAchievement = MMOMapDataManager::getSingleton().FindFBAchievementCurrentDungeon( fbAchievementID );
	if( curFBAchievement == NULL ){
		warnf( NAME_Warning, TEXT("Act_Dungeon_State SceneVariable[%d] FBAchievement template is NULL"), fbAchievementID );
		return true;
	}

	//
	const FDungeonTarget *thisTemplate = curFBAchievement->_achievementTemplate;
	if( thisTemplate == NULL ){
		warnf( NAME_Warning, TEXT("Act_Dungeon_State SceneVariable[%d] FBAchievement template is NULL"), fbAchievementID );
		return true;
	}
	////if completed
	//if( thisTemplate->TargetValue <= curFBAchievement->_score ){
	//	warnf( NAME_Warning, TEXT("Act_Dungeon_State SceneVariable[%d] FBAchievement is completed At %d"), fbAchievementID, curFBAchievement->_score );
	//	return true;
	//}

	//curFBAchievement->_score = data.nValue;
	//Notify completion directly
	curFBAchievement->_score = ( data.nValue > 0 ? thisTemplate->TargetValue : 0 );

	//if completed at current
	if( thisTemplate->TargetValue >= curFBAchievement->_score ){
		MMOGameEvent_OneInt evt( GameEvent_FBAchieve, fbAchievementID );
		MMOUI::PostGameEvent(&evt);
	}

	return true;
}
//========================================================================================================
//Arena
//========================================================================================================
bool Act_Arena_Prepare( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	//UINT32 tenSecBeforeStartTime = (UINT32)data.nValue;
	UINT32 startTime = (UINT32)data.nValue;

	//
	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();
	arenaMgr.SetNewArenaRoundState( startTime, 0 );

	UINT32 curRoundLeftSec = 0;
	if( MMO::GetCurrentGameState() == EGameState_InGame ){
		curRoundLeftSec = arenaMgr.GetArenaCurRoundLeftSecond();
		UINT32 defaultTime = arenaMgr.GetArenaRoundDefaultTimeSec();

		if( curRoundLeftSec > defaultTime ){
			INT preSec = (INT)( curRoundLeftSec - defaultTime );

			UMMOUIFightingScreenHint *pFightingScreenHint = (UMMOUIFightingScreenHint*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint );
			if( pFightingScreenHint != NULL ){
				pFightingScreenHint->Show( TRUE );
				pFightingScreenHint->PlayTenSecCountDownAfter( preSec );
			}
		}
	}

	SYSTEMTIME sysTypeTime = MMOFacade::getSingleton().ConvertUtcSecondToSystemTime( startTime );
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("Act_Arena_Prepare  RcvStartTime(%d): %02d-%02d:%02d:%02d( Left:%d )"), startTime, sysTypeTime.wDay, sysTypeTime.wHour, sysTypeTime.wMinute, sysTypeTime.wSecond, curRoundLeftSec );
	CLEAR_WARN_COLOR();

	return true;
}

bool Act_Arena_State( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	SYSTEMTIME sysTypeTime = MMOFacade::getSingleton().GetRealClientTime();
	INT curState = data.nValue;

	switch( curState )
	{
	case ARENA_STAT_READY:
		{
			UMMOUIVSState *pVSState = (UMMOUIVSState*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_VSState );
			if( pVSState == NULL ){
				break;
			}

			if( !pVSState->IsVisible ){
				pVSState->eventCallAsSetRoundTime( 0 );
				pVSState->Show( TRUE );
			}
		}
		break;
	case ARENA_STAT_FIGHT:
		{
			UMMOUIFightingScreenHint *pFightingScreenHint = (UMMOUIFightingScreenHint*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FightingScreenHint );
			if( pFightingScreenHint != NULL ){
				pFightingScreenHint->PlayTenSecCountDownAfter( 0 );
			}

			//
			INT vsSessionIdx = MMOArenaManager::getSingleton().GetCurVSArenaSessionIdx();
			FString hintName = TEXT("MMOGame.Arena.HintCurrentAreanInfo");
			FString hintText = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( hintName ), vsSessionIdx );
			MMOUI::GetUIManager()->delegateAddScreenMessage( hintText, MsgInfoType_Table, hintName );
			MMOChatManager::getSingleton().AddSystemMessage( hintText );
			//
			UMMOUIVSState *pVSState = (UMMOUIVSState*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_VSState );
			if( pVSState != NULL ){
				pVSState->eventCallAsSetRoundTime( -1 );
			}
		}
		break;
	}

	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("Act_Arena_State  ClientServerTime: %02d-%02d:%02d:%02d, State:%d"), 
		sysTypeTime.wDay, sysTypeTime.wHour, sysTypeTime.wMinute, sysTypeTime.wSecond, curState );
	CLEAR_WARN_COLOR();

	return true;
}

bool Act_Arena_PlayerCnt( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT totalCnt = data.nValue;

	return true;
}

bool Act_Arena_Player1( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT playerID = data.nValue;

	VSPKInfo &sidePkInfo = MMOArenaManager::getSingleton().GetVSSidePkInfo( EVSSide_1 );
	//
	sidePkInfo._PerInfo._ActorID = playerID;
	//
	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if( sidePkInfo._PerInfo._ActorID == mainActor->ServerID ){
		sidePkInfo._Name = mainActor->GetActorName();
		MMOArenaManager::getSingleton().SetMyVSSide( EVSSide_1 );
	}
	else{
		PlayerBaseInfo *pPlayerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( sidePkInfo._PerInfo._ActorID );
		if( pPlayerInfo != NULL ){
			sidePkInfo._Name = pPlayerInfo->_RoleName;
		}
	}

	MMOGameEvent_OneInt ntfEvt( GameEvent_VSNotify, EVSNotify_SideInfo );
	MMOUI::PostGameEvent( &ntfEvt );

	return true;
}

bool Act_Arena_Player2( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT playerID = data.nValue;

	VSPKInfo &sidePkInfo = MMOArenaManager::getSingleton().GetVSSidePkInfo( EVSSide_2 );
	//
	sidePkInfo._PerInfo._ActorID = playerID;
	//
	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if( sidePkInfo._PerInfo._ActorID == mainActor->ServerID ){
		sidePkInfo._Name = mainActor->GetActorName();
		MMOArenaManager::getSingleton().SetMyVSSide( EVSSide_2 );
	}
	else{
		PlayerBaseInfo *pPlayerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( sidePkInfo._PerInfo._ActorID );
		if( pPlayerInfo != NULL ){
			sidePkInfo._Name = pPlayerInfo->_RoleName;
		}
	}

	MMOGameEvent_OneInt ntfEvt( GameEvent_VSNotify, EVSNotify_SideInfo );
	MMOUI::PostGameEvent( &ntfEvt );

	return true;
}

bool Act_Arena_PlayerData1( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT usedRecovery = data.nValue;

	VSPKInfo &sidePkInfo = MMOArenaManager::getSingleton().GetVSSidePkInfo( EVSSide_1 );
	//
	sidePkInfo._PerInfo._UsedRecovery = usedRecovery;
	sidePkInfo._UsedRecovery = sidePkInfo._PerInfo._UsedRecovery;

	MMOGameEvent_OneInt ntfEvt( GameEvent_VSNotify, EVSNotify_SideUsedRecovery );
	MMOUI::PostGameEvent( &ntfEvt );

	return true;
}

bool Act_Arena_PlayerData2( INT sceneID , const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
{
	INT usedRecovery = data.nValue;
	VSPKInfo &sidePkInfo = MMOArenaManager::getSingleton().GetVSSidePkInfo( EVSSide_2 );
	//
	sidePkInfo._PerInfo._UsedRecovery = usedRecovery;
	sidePkInfo._UsedRecovery = sidePkInfo._PerInfo._UsedRecovery;

	MMOGameEvent_OneInt ntfEvt( GameEvent_VSNotify, EVSNotify_SideUsedRecovery );
	MMOUI::PostGameEvent( &ntfEvt );

	return true;
}
//bool Act_Arena_SyncTime( INT sceneID, const G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA &data )
//{
//	INT SixtySecBeforeStartTime = data.nValue;
//
//	MMOArenaManager &arenaMgr = MMOArenaManager::getSingleton();
//	arenaMgr.StartArenaTick( SixtySecBeforeStartTime );
//
//	return true;
//}

////
//IMPLEMENT_ACT_DUGEON_STATE(101)
//IMPLEMENT_ACT_DUGEON_STATE(102)
//IMPLEMENT_ACT_DUGEON_STATE(103)
//IMPLEMENT_ACT_DUGEON_STATE(104)
//IMPLEMENT_ACT_DUGEON_STATE(105)
//IMPLEMENT_ACT_DUGEON_STATE(106)
//IMPLEMENT_ACT_DUGEON_STATE(107)
//IMPLEMENT_ACT_DUGEON_STATE(108)
