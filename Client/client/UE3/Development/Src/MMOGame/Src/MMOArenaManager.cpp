#include "stdafx.h"
#include "MMOArenaManager.h"
#include "MMOFacade.h"
#include "MMOGameUIClasses.h"
#include "BattleTeamManager.h"
#include "TeamManager.h"
#include "MMOQuestManager.h"
#include "MMOMapDataManager.h"

#define ARENA_SESSION_IDX_VAR_ID (195)
#define ARENA_REWARD_CNT_VAR_ID (196)
#define ARENA_REWARD_GETTED_CNT_VAR_ID (197)

MMOArenaManager::MMOArenaManager()
{
	_VSOneOnOnePKInfo[EVSSide_1]._SideIdx = EVSSide_1;
	_VSOneOnOnePKInfo[EVSSide_2]._SideIdx = EVSSide_2;
	//
	_CurArenaAllMoney = 0;
	_CurArenaHighestScore = 0;
	_PrizeMoneyPercent = 0;
	_ArenaStartTime = 0;
	_ArenaEndTime = 0;
	_CurArenaState = EAnaSta_Invlid;
}

MMOArenaManager::~MMOArenaManager()
{

}

void MMOArenaManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEvent_VariableInit:
	case GameEvent_VariableChanged:
		{
			MMOGameEvent_VariableChange *varChgEvt = (MMOGameEvent_VariableChange*)evt;
			if( varChgEvt == NULL ){
				break;
			}
			
			if( varChgEvt->VarType == vtRole ){
				switch( varChgEvt->VarID )
				{
				case ARENA_SESSION_IDX_VAR_ID:
					{
						_VSSessionIdx = varChgEvt->VarValue;
					}
					break;
				case ARENA_REWARD_CNT_VAR_ID:
					{
						if( evt->GetEventType() == GameEvent_VariableChanged && _VSRwdCanTryTimes < varChgEvt->VarValue ){
							_VSGettedTryTimes = varChgEvt->VarValue - _VSRwdCanTryTimes;
						}

						_VSRwdCanTryTimes = varChgEvt->VarValue;
					}
					break;
				case ARENA_REWARD_GETTED_CNT_VAR_ID:
					{
						_VSRwdGettedTimes = varChgEvt->VarValue;
					}
					break;
				}

				UMMOUIVSReward *pVSReward = (UMMOUIVSReward*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_VSReward );
				if( pVSReward != NULL ){
					if( pVSReward->IsVisible ){
						pVSReward->UpdateRewardInfo();
					}
				}
			}
		}
		break;
	case GameEvent_VSOneOnOneResult:
		{
			MMOGameEvent_VSOneOnOneResult *resultEvt = (MMOGameEvent_VSOneOnOneResult*)evt;
			if( resultEvt == NULL ){
				break;
			}

			UMMOUIVSResult *pVSResult = (UMMOUIVSResult*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_VSResult );
			if( pVSResult != NULL ){
				pVSResult->_VSResult = resultEvt->_VSResult;
				pVSResult->_Damage = resultEvt->_Damage;
				pVSResult->_vsMoneyDelta = resultEvt->_vsMoneyDelta;
				pVSResult->_ScoreDelta = resultEvt->_ScoreDelta;
				pVSResult->_LastSec = resultEvt->_LastSec;

				pVSResult->UpdateResultInfo();
				if( !pVSResult->IsVisible ){
					pVSResult->Show( TRUE );
				}
			}

			//
			UMMOUIVSState *pVSState = (UMMOUIVSState*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_VSState );
			if( pVSState != NULL ){
				pVSState->eventCallAsSetRoundTime( pVSResult->_LastSec * 1000 );
			}
		}
		break;
	case GameEvent_VSOneOnOneRewardInfo:
		{
			MMOGameEvent_VSOneOnOneRewardInfo *rwdInfoEvt = (MMOGameEvent_VSOneOnOneRewardInfo*)evt;
			if( rwdInfoEvt == NULL ){
				break;
			}

			_VSTotalRwd = rwdInfoEvt->_TotalRwd;
			_VSAvgRwd = rwdInfoEvt->_AvgRwd;

			UMMOUIVSReward *pVSReward = (UMMOUIVSReward*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_VSReward );
			if( pVSReward != NULL ){
				if( !pVSReward->IsVisible ){
					pVSReward->Show( TRUE );
				}
				else{
					pVSReward->UpdateRewardInfo();
				}
			}
		}
		break;
	case GameEvent_VSOneOnOneState:
		{
			MMOGameEvent_OneInt *vsStateEvt = (MMOGameEvent_OneInt*)evt;
			if( vsStateEvt == NULL ){
				break;
			}

			INT newState = vsStateEvt->IntData;
			switch( newState )
			{
			case ARENA_STAT_NONE:
				{
					ClearCurArenaRoundData();
				}
				break;
			case ARENA_STAT_WAIT:
				{
					//new Round will begin
					ClearCurArenaRoundData();
					//
					UMMOUIVSResult *pVSResult = (UMMOUIVSResult*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_VSResult );
					if( pVSResult == NULL ){
						break;
					}
					//
					pVSResult->UpdateTimeOut( EVSTimeCntSta_On );
					if( !pVSResult->IsVisible ){
						pVSResult->Show( TRUE );
					}
				}
				break;
			case ARENA_STAT_FIGHT:
				{
					UMMOUIVSResult *pVSResult = (UMMOUIVSResult*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_VSResult );
					if( pVSResult == NULL ){
						break;
					}

					_VSGettedTryTimes = 0;
					//
					if( _CurVSState == ARENA_STAT_WAIT ){
						//from waiting state to battle start state
						if( pVSResult->IsVisible ){
							pVSResult->Show( FALSE );
						}
						//pVSResult->UpdateTimeOut( EVSTimeCntSta_Invalid );
					}
				}
				break;
			}
			//
			_CurVSState = newState;
		}
		break;
	case GameEvent_EnterSelectingRole:
		{
			ClearAllCurDynamicData();
		}
		break;
	case GameEvent_ServerError:
		{
			MMOGameEvent_ServerError *errorEvt = (MMOGameEvent_ServerError*)evt;
			if( errorEvt != NULL && errorEvt->ErrCode == errArenaMatchFailed ){
				UMMOUIVSResult *pVSResult = (UMMOUIVSResult*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_VSResult );
				if( pVSResult == NULL ){
					break;
				}
				//
				pVSResult->UpdateTimeOut( EVSTimeCntSta_Exceed );
				if( !pVSResult->IsVisible ){
					pVSResult->Show( TRUE );
				}
			}
		}
		break;
	case GameEvent_FinishLoading:
		{
			EArenaState curArenaSta = GetCurArenaState();
			switch( curArenaSta )
			{
			case EAnaSta_InGame:
			case EAnaSta_WaitNextRound:
			case EAnaSta_WaitStartRound:
			case EAnaSta_InRound:
				{
					UMMOUIArenaState *arenaStatePanel = (UMMOUIArenaState*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Arena_ArenaState );
					if( arenaStatePanel == NULL ){
						arenaStatePanel = (UMMOUIArenaState*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Arena_ArenaState );
					}

					if( arenaStatePanel != NULL && !arenaStatePanel->IsVisible ){
						arenaStatePanel->UpdatePanel();
						arenaStatePanel->Show( TRUE );
					}

					//PreStart Animation
					FExportMap &curScene = MMOMapDataManager::getSingleton().GetCurMap();
					if( curScene.ServerMapType == E_est_Arena ){
						UINT32 curRoundLeftSec = GetArenaCurRoundLeftSecond();
						UINT32 defaultTime = GetArenaRoundDefaultTimeSec();
						if( curRoundLeftSec > defaultTime ){
							
							INT preSec = (INT)( curRoundLeftSec - defaultTime );

							UMMOUIFightingScreenHint *pFightingScreenHint = (UMMOUIFightingScreenHint*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint );
							if( pFightingScreenHint != NULL ){
								pFightingScreenHint->Show( TRUE );
								pFightingScreenHint->PlayTenSecCountDownAfter( preSec );
							}
						}
						
					}
				}
				break;
			default:
				{
					UMMOUIArenaState *arenaStatePanel = (UMMOUIArenaState*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Arena_ArenaState );
					if( arenaStatePanel != NULL && arenaStatePanel->IsVisible ){
						arenaStatePanel->Show( FALSE );
					}
				}
				break;
			}
		}
		break;
	case GameEvent_ArenaStateChanged:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;

			//EArenaState curArenaSta = GetCurArenaState();
			//switch( curArenaSta )
			//{
			//case EAnaSta_InGame:
			//case EAnaSta_WaitNextRound:
			//case EAnaSta_WaitStartRound:
			//case EAnaSta_InRound:
			//	{
			//		UMMOUIArenaState *arenaStatePanel = (UMMOUIArenaState*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Arena_ArenaState );
			//		if( arenaStatePanel == NULL ){
			//			arenaStatePanel = (UMMOUIArenaState*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Arena_ArenaState );
			//		}

			//		if( arenaStatePanel != NULL && !arenaStatePanel->IsVisible && ( MMO::GetCurrentGameState() == EGameState_InGame ) ){
			//			arenaStatePanel->UpdatePanel();
			//			arenaStatePanel->Show( TRUE );
			//		}
			//	}
			//	break;
			//}
		}
		break;
	case GameEvent_ArenaRoundResult:
		{
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)evt;
			UBOOL isWin = ( msg->IntData > 0 );
			if( isWin ){
				//victory
				++_MyArenaData._CurHighestScore;

				//
				EArenaState curArenaSta = GetCurArenaState();
				if( curArenaSta == EAnaSta_InRound ){
					SetCurArenaState( EAnaSta_InGame );
				}
				else{
					SET_WARN_COLOR(COLOR_CYAN);
					warnf( NAME_Log, TEXT( "ERROR, Parse_g2c_arena_win current arena state is %d" ), (INT)curArenaSta );
					CLEAR_WARN_COLOR();
				}
			}
			else{
				EArenaState curArenaSta = GetCurArenaState();
				if( curArenaSta == EAnaSta_InRound ){
					SetCurArenaState( EAnaSta_WaitApply );
				}
				else{
					SET_WARN_COLOR(COLOR_CYAN);
					warnf( NAME_Log, TEXT( "ERROR, Parse_g2c_arena_defeat current arena state is %d" ), (INT)curArenaSta );
					CLEAR_WARN_COLOR();
				}
			}

			//
			UMMOUIFightingScreenHint *pFightingScreenHint = (UMMOUIFightingScreenHint*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint );
			if( pFightingScreenHint != NULL && MMO::GetCurrentGameState() == EGameState_InGame ){
				pFightingScreenHint->Show( TRUE );
				if( isWin ){
					pFightingScreenHint->eventCallASPlayWinAnim();
				}
				else{
					pFightingScreenHint->eventCallASPlayLoseAnim( 5 );
				}
			}
		}
		break;
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* infoEvt = (MMOGameEvent_PlayerBaseInfo*)evt;
			if( infoEvt  == NULL ){
				break;
			}

			if( _CurVSState != ARENA_STAT_NONE ){
				EVSSideIdx sideIdx = FindPlayerVSSide( infoEvt->_RoldID );
				if( sideIdx != EVSSide_Total ){
					VSPKInfo &sidePkInfo = GetVSSidePkInfo( sideIdx );
					sidePkInfo._Name = ( infoEvt->_PlayerBaseInfo != NULL ? infoEvt->_PlayerBaseInfo->_RoleName : TEXT("Unknow") );
					//
					UMMOUIVSState *pVSState = (UMMOUIVSState*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_VSState );
					if( pVSState != NULL && pVSState->IsVisible){
						pVSState->UpdateVSPkSideStaticInfo();
					}
				}
			}
		}
		break;
	}
}

//
void MMOArenaManager::Init()
{
	GConfig->GetInt( TEXT("Arena") , TEXT("ArenaBeginHour"), _ArenaBeginH, GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("ArenaEndHour"), _ArenaEndH, GDefaultGameIni );
	//
	//INT percent = 0;
	//GConfig->GetInt( TEXT("Arena") , TEXT("PrizeMnyPercent"), percent, GDefaultGameIni );
	//_PrizeMoneyPercent = (FLOAT)(percent) / 100;
	INT percent;
	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeMnyPercent"), percent, GDefaultGameIni );
	_PrizeMoneyPercent = percent;
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank1Pect"), percent, GDefaultGameIni );
	_PrizeForWin[EArean_WinPrizeRank1]._PerPlayerPect = percent;
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank2Pect"), percent, GDefaultGameIni );
	_PrizeForWin[EArean_WinPrizeRank2]._PerPlayerPect = percent;
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank3Pect"), percent, GDefaultGameIni );
	_PrizeForWin[EArean_WinPrizeRank3]._PerPlayerPect = percent;
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank4Pect"), percent, GDefaultGameIni );
	_PrizeForWin[EArean_WinPrizeRank4]._PerPlayerPect = percent;

	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank1Num"), _PrizeForWin[EArean_WinPrizeRank1]._MaxPlayerGetNum, GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank2Num"), _PrizeForWin[EArean_WinPrizeRank2]._MaxPlayerGetNum, GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank3Num"), _PrizeForWin[EArean_WinPrizeRank3]._MaxPlayerGetNum, GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("WinMnyRank4Num"), _PrizeForWin[EArean_WinPrizeRank4]._MaxPlayerGetNum, GDefaultGameIni );
	//
	INT cfgMny = 0;
	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank1MnyNeed"), cfgMny, GDefaultGameIni );
	_PrizeForRank[EArean_Rank1]._RankMoneyNeed = ( cfgMny > 0 ? (UINT)cfgMny : 0 );
	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank1Award"), _PrizeForRank[EArean_Rank1]._AwardPrizeID, GDefaultGameIni );
	_PrizeForRank[EArean_Rank1]._Desc = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaRank1PrizeDesc") );

	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank2MnyNeed"), cfgMny, GDefaultGameIni );
	_PrizeForRank[EArean_Rank2]._RankMoneyNeed = ( cfgMny > 0 ? (UINT)cfgMny : 0 );
	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank2Award"), _PrizeForRank[EArean_Rank2]._AwardPrizeID, GDefaultGameIni );
	_PrizeForRank[EArean_Rank2]._Desc = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaRank2PrizeDesc") );

	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank3MnyNeed"), cfgMny, GDefaultGameIni );
	_PrizeForRank[EArean_Rank3]._RankMoneyNeed = ( cfgMny > 0 ? (UINT)cfgMny : 0 );
	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank3Award"), _PrizeForRank[EArean_Rank3]._AwardPrizeID, GDefaultGameIni );
	_PrizeForRank[EArean_Rank3]._Desc = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaRank3PrizeDesc") );

	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank4MnyNeed"), cfgMny, GDefaultGameIni );
	_PrizeForRank[EArean_Rank4]._RankMoneyNeed = ( cfgMny > 0 ? (UINT)cfgMny : 0 );
	GConfig->GetInt( TEXT("Arena") , TEXT("PrizeRank4Award"), _PrizeForRank[EArean_Rank4]._AwardPrizeID, GDefaultGameIni );
	_PrizeForRank[EArean_Rank4]._Desc = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.ArenaRank4PrizeDesc") );

	//
	MMOUI::RegisterEventHandler( this, GameEvent_VariableInit );
	MMOUI::RegisterEventHandler( this, GameEvent_VariableChanged );
	MMOUI::RegisterEventHandler( this, GameEvent_VSOneOnOneRewardInfo );
	MMOUI::RegisterEventHandler( this, GameEvent_VSOneOnOneResult );
	MMOUI::RegisterEventHandler( this, GameEvent_VSOneOnOneState );
	MMOUI::RegisterEventHandler( this, GameEvent_ArenaStateChanged );
	MMOUI::RegisterEventHandler( this, GameEvent_ArenaRoundResult );
	MMOUI::RegisterEventHandler( this, GameEvent_EnterSelectingRole );
	MMOUI::RegisterEventHandler( this, GameEvent_FinishLoading );
	MMOUI::RegisterEventHandler( this, GameEvent_ServerError );
	MMOUI::RegisterEventHandler( this, GameEvent_PlayerBaseInfo );
	//
	ClearAllCurDynamicData();
}

void MMOArenaManager::Shutdown()
{
	MMOUI::UnRegisterEventHandler( this, GameEvent_PlayerBaseInfo );
	MMOUI::UnRegisterEventHandler( this, GameEvent_ServerError );
	MMOUI::UnRegisterEventHandler( this, GameEvent_FinishLoading );
	MMOUI::UnRegisterEventHandler( this, GameEvent_EnterSelectingRole );
	MMOUI::UnRegisterEventHandler( this, GameEvent_ArenaRoundResult );
	MMOUI::UnRegisterEventHandler( this, GameEvent_ArenaStateChanged );
	MMOUI::UnRegisterEventHandler( this, GameEvent_VSOneOnOneState );
	MMOUI::UnRegisterEventHandler( this, GameEvent_VSOneOnOneResult );
	MMOUI::UnRegisterEventHandler( this, GameEvent_VSOneOnOneRewardInfo );
	MMOUI::UnRegisterEventHandler( this, GameEvent_VariableChanged );
	MMOUI::UnRegisterEventHandler( this, GameEvent_VariableInit );
}

//============================================================================================================================
void MMOArenaManager::ClearAllCurDynamicData()
{

	//
	_VSRwdCanTryTimes = 0;
	_VSGettedTryTimes = 0;
	_VSRwdGettedTimes = 0;
	//
	SetCurArenaMoney( 0 );
	//
	_MyArenaData._AllTimeHightScore = 0;
	_MyArenaData._CurHighestScore = 0;
	_MyArenaData._SpendMoney = 0;
	_MyArenaData._WinLevel = EArean_WinPrizeRankInvalid;
	_MyArenaData._Rank = 0;
	_MyArenaData._WinMoney = 0;
	_MyArenaData._WinItemID = 0;
	//
	SetCurArenaState( EAnaSta_Invlid );
	//
	ReflashArenaTime();
}
//--------------------------------------------------------------------------------------------------------------------------
void MMOArenaManager::ClearCurArenaRoundData()
{
	_MyVSSide = EVSSide_1;

	for( INT i = EVSSide_1; i < EVSSide_Total; ++i ){
		VSPKInfo &info = GetVSSidePkInfo( i );
		info._Name = TEXT("");
		info._UsedRecovery = 0;
		//
		info._PerInfo._ActorID = 0;
		info._PerInfo._UsedRecovery = 0;
	}
}
//============================================================================================================================
VSPKInfoPersonal &MMOArenaManager::GetVSPersonalPkInfo( INT actorID, EVSSideIdx &inVSSideIdx )
{
	//initialized to invalid
	inVSSideIdx = EVSSide_Total;

	for( INT i = EVSSide_1; i < EVSSide_Total; ++i ){
		VSPKInfo &info = GetVSSidePkInfo( i );
		if( info._PerInfo._ActorID == actorID ){
			inVSSideIdx = (EVSSideIdx)i;
			return info._PerInfo;
		}
	}

	return GetVSSidePkInfo( EVSSide_1 )._PerInfo;
}
//============================================================================================================================
INT MMOArenaManager::GetVSSideUsedRecovery( INT sideIdx )
{
	if( sideIdx < EVSSide_1  && EVSSide_Total <= sideIdx ){
		return -1;
	}

	VSPKInfo &info = GetVSSidePkInfo( sideIdx );
	return info._UsedRecovery;
}
//============================================================================================================================
EVSSideIdx MMOArenaManager::FindPlayerVSSide( INT actorID )
{
	//initialized to invalid
	EVSSideIdx rstVSSideIdx = EVSSide_Total;

	for( INT i = EVSSide_1; i < EVSSide_Total; ++i ){
		VSPKInfo &info = GetVSSidePkInfo( i );
		if( info._PerInfo._ActorID == actorID ){
			rstVSSideIdx = (EVSSideIdx)i;
			break;
		}
	}

	return rstVSSideIdx;
}
//============================================================================================================================
const ArenaRankPrize &MMOArenaManager::GetArenaDefaultPrize( int idx )
{ 
	if( idx < 0 || idx >= EArean_RankMax ){
		idx = EArean_Rank1;
		warnf( NAME_Error, TEXT("MMOArenaManager::GetArenaDefaultPrize idx overflew"), idx );
	}

	return _PrizeForRank[idx];
}
//============================================================================================================================
INT MMOArenaManager::GetArenaCurPrizeRank()
{
	INT curLevel = -1;
	for( INT i = 0; i < EArean_RankMax; ++i ){
		const ArenaRankPrize &defalutPrize = GetArenaDefaultPrize( i );
		UINT64 targetMny = defalutPrize._RankMoneyNeed;
		if( _CurArenaAllMoney >= targetMny ){
			curLevel = i;
		}
	}

	return curLevel;
}
//============================================================================================================================
void MMOArenaManager::SetCurArenaMoney( UINT64 totalMoney )
{
	_CurArenaAllMoney = totalMoney;
	//
	ReflashArenaWinMoney();
}
//============================================================================================================================
void MMOArenaManager::ReflashArenaWinMoney()
{
	UINT64 bonus = _CurArenaAllMoney * (_PrizeMoneyPercent / 10000 );
	//
	for( INT i = EArean_WinPrizeRank1; i < EArean_WinPrizeRankMax; ++i ){
		//FLOAT percent = ( (FLOAT)_PrizeForWin[i]._PerPlayerPect * _PrizeForWin[i]._MaxPlayerGetNum );
		_PrizeForWin[i]._MoneyToAward = bonus * ( _PrizeForWin[i]._PerPlayerPect / 10000 );
	}
}
//--------------------------------------------------------------------------------------------------------------------------
const ArenaWinPrize &MMOArenaManager::GetArenaWinPrizeData( int idx )
{
	if( idx < 0 || idx >= EArean_WinPrizeRankMax ){
		idx = EArean_WinPrizeRank1;
		warnf( NAME_Error, TEXT("MMOArenaManager::ReflashArenaWinMoney idx overflew"), idx );
	}

	return _PrizeForWin[idx];
}

//==========================================================================================================================
void MMOArenaManager::ReflashArenaTime()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	if( IsArenaDay() ){
		//Current local time
		SYSTEMTIME curTime;
		::GetLocalTime(&curTime);
		UINT32 curTimeSec = mmoFacade.ConvertSystemTimeToTmTSecond( curTime );

		//Time difference between local and server
		INT32 secDiff = mmoFacade.GetClientToSceneSecondDiff();
		UINT32 newTime = ( ( secDiff > 0 ) ? ( curTimeSec + (UINT32)secDiff ) : ( (UINT32)( curTimeSec - (UINT32)( -secDiff) ) ) );
		SYSTEMTIME realCurTime = mmoFacade.ConvertUtcSecondToSystemTime( newTime );
		realCurTime.wMinute = 0;
		realCurTime.wSecond = 0;
		//
		realCurTime.wHour = AreanBeginHour();
		UINT32 beingTime = mmoFacade.ConvertSystemTimeToTmTSecond( realCurTime );
		SetStartTimeByServerTime( (__time32_t)beingTime );
		//
		realCurTime.wHour = AreanEndHour();
		UINT32 endTime = mmoFacade.ConvertSystemTimeToTmTSecond( realCurTime );
		SetEndTimeByServerTime( (__time32_t)endTime );
	}
	else{

	}

	//if( IsArenaDay() ){
	//	//current local time
	//	__time32_t enterServerTime = (__time32_t)mmoFacade.GetEnterSceneServerTime();
	//	struct tm* enterServerTM = _localtime32(&enterServerTime);
	//	if( enterServerTM ){
	//		enterServerTM->tm_min = 0;
	//		enterServerTM->tm_sec = 0;
	//		//
	//		enterServerTM->tm_hour = AreanBeginHour90;
	//		__time32_t beingTime = _mktime32( enterServerTM );
	//		SetStartTimeByServerTime( beingTime );
	//		//
	//		enterServerTM->tm_hour = AreanEndHour();
	//		__time32_t endTime = _mktime32( enterServerTM );
	//		SetEndTimeByServerTime( endTime );
	//	}
	//}
}
//==========================================================================================================================
void MMOArenaManager::SetStartTimeByServerTime( __time32_t serverTime )
{
	_ArenaStartTime = serverTime;
}
//-------------------------------------------------------------------------------------------------------------------------
_SYSTEMTIME MMOArenaManager::GetStartTime()
{
	return MMOFacade::getSingleton().ConvertUtcSecondToSystemTime( (UINT32)_ArenaStartTime );
}
//==========================================================================================================================
void MMOArenaManager::SetEndTimeByServerTime( __time32_t serverTime )
{
	_ArenaEndTime = serverTime;
}
//-------------------------------------------------------------------------------------------------------------------------
_SYSTEMTIME MMOArenaManager::GetEndTime()
{
	return MMOFacade::getSingleton().ConvertUtcSecondToSystemTime( (UINT32)_ArenaEndTime );
}
//==========================================================================================================================
UINT32 MMOArenaManager::GetArenaLeftSecond()
{
	UINT32 curTime = MMOFacade::getSingleton().GetRealClientUTCTime();
	//
	UINT32 serverExpireTime = (UINT32)_ArenaEndTime;
	UINT32 leftSecond = 0;
	if( serverExpireTime > curTime ){
		leftSecond = serverExpireTime - ( curTime );
	}

	return leftSecond;
}
//======================================================================================
void MMOArenaManager::SetNewArenaRoundState( UINT32 baseUTCTime, INT modifySec )
{
	UINT32 roundStartTime = ( ( modifySec > 0 ) ? ( baseUTCTime + (UINT32)modifySec ) : ( (UINT32)( baseUTCTime - (UINT32)( -modifySec) ) ) );
	
	//
	UINT32 roundEndTime = roundStartTime + GetArenaRoundDefaultTimeSec();
	SetArenaCurRoundEndTime( (__time32_t)roundEndTime );

	//
	UINT32 curTime = MMOFacade::getSingleton().GetRealClientUTCTime();
	if( curTime >= roundStartTime ){
		//already started
		SetCurArenaState( EAnaSta_InRound );
	}
	else{
		SetCurArenaState( EAnaSta_WaitStartRound );
	}
}
//---------------------------------------------------------------------------------------
void MMOArenaManager::SetArenaCurRoundEndTime( __time32_t utcTime )
{
	_ArenaCurRoundEndTime = utcTime;
}
//---------------------------------------------------------------------------------------
UINT32 MMOArenaManager::GetArenaCurRoundLeftSecond()
{
	UINT32 curTime = MMOFacade::getSingleton().GetRealClientUTCTime();
	//
	UINT32 roundExpireTime = (UINT32)_ArenaCurRoundEndTime;
	UINT32 leftSecond = 0;
	if( roundExpireTime > curTime ){
		leftSecond = roundExpireTime - ( curTime );
	}

	return leftSecond;
}
//======================================================================================
UBOOL MMOArenaManager::IsArenaDay()
{
	//SYSTEMTIME realCurTime = MMOFacade::getSingleton().GetRealClientTime();
	//INT weekDay = realCurTime.wDayOfWeek;
	////if( weekDay == 2 || weekDay == 4 || weekDay == 6 ){
	//	return TRUE;
	////}

	return FALSE;
}
//======================================================================================
UBOOL MMOArenaManager::IsArenaApplyTime()
{
	//SYSTEMTIME realCurTime = MMOFacade::getSingleton().GetRealClientTime();
	//INT weekDay = realCurTime.wDayOfWeek;
	////if( weekDay == 2 || weekDay == 4 || weekDay == 6 ){
	//	if( realCurTime.wHour < AreanEndHour() ){
	//		return TRUE;
	//	}
	////}

	return FALSE;
}
//======================================================================================
UBOOL MMOArenaManager::IsArenaFightingTime()
{
	SYSTEMTIME realCurTime = MMOFacade::getSingleton().GetRealClientTime();
	INT weekDay = realCurTime.wDayOfWeek;
	//if( weekDay == 2 || weekDay == 4 || weekDay == 6 ){
	if( realCurTime.wHour >= AreanBeginHour() && realCurTime.wHour < AreanEndHour() ){
		return TRUE;
	}
	//}

	return FALSE;
}
//======================================================================================
UBOOL MMOArenaManager::CanApplyArena( AMMOPlayerController *playerCtrl, FString &errorKeyStr )
{
	if( playerCtrl == NULL ){
		return FALSE;
	}

	AMMOPlayer* player = Cast<AMMOPlayer>( playerCtrl->Pawn );
	if( player == NULL ){
		return FALSE;
	}

	//level
	INT curLevel = player->GetActorLevel();
	if( curLevel < GetArenaEntryLevel() ){
		errorKeyStr = TEXT("MMOGame.Arena.EntryLvError");
		return FALSE;
	}

	//money
	INT curMny = player->GetMoney(E_MONEYTYPE_02);
	if( (UINT)curMny < GetArenaEntryFee() ){
		errorKeyStr = TEXT("MMOGame.Arena.EntryMoneyError");
		return FALSE;
	}

	//
	//UBOOL isTimeOK = IsArenaApplyTime();
	//if( !isTimeOK ){
	//	errorKeyStr = TEXT("MMOGame.Arena.EntryTimeError");
	//	return FALSE;
	//}

	return TRUE;
}
//==========================================================================================================================
void MMOArenaManager::SetCurArenaState( EArenaState curState )
{
	EArenaState oldState = _CurArenaState;
	_CurArenaState = curState;

	if( _CurArenaState == EAnaSta_WaitApply ){
		//clear to zero
		SetMyCurHighestScore( 0 );
	}

	//MMOGameEvent_Arena arenStaChgEvt( EArean_StateChanged, oldState );
	MMOGameEvent_OneInt arenStaChgEvt( GameEvent_ArenaStateChanged, oldState );
	MMOUI::PostGameEvent(&arenStaChgEvt);
}
//==========================================================================================================================
void MMOArenaManager::SetMyCurHighestScore( INT curScore )
{
	_MyArenaData._CurHighestScore = curScore;
}
//-------------------------------------------------------------------------------------------------------------------------
void MMOArenaManager::SetMyAllTimeScore( INT curScore )
{
	_MyArenaData._AllTimeHightScore = curScore;
}
//-------------------------------------------------------------------------------------------------------------------------
void MMOArenaManager::SetMyWinLevel( INT myCurWinRank )
{
	_MyArenaData._Rank = myCurWinRank;

	if( myCurWinRank == 1 ){
		_MyArenaData._WinLevel = EArean_WinPrizeRank1;
	}
	else if( myCurWinRank > 1 ){
		if( myCurWinRank <= _PrizeForWin[EArean_WinPrizeRank2]._MaxPlayerGetNum ){
			_MyArenaData._WinLevel = EArean_WinPrizeRank2;
		}
		else if( myCurWinRank <= _PrizeForWin[EArean_WinPrizeRank3]._MaxPlayerGetNum ){
			_MyArenaData._WinLevel = EArean_WinPrizeRank3;
		}
		else if( myCurWinRank <= _PrizeForWin[EArean_WinPrizeRank4]._MaxPlayerGetNum ){
			_MyArenaData._WinLevel = EArean_WinPrizeRank4;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void MMOArenaManager::SetMySpendForArena( UINT spendMny )
{
	_MyArenaData._SpendMoney = spendMny;
}
//
void MMOArenaManager::SetMySpendForArenaByEntCnt( UINT entCnt )
{
	UINT32 singleFee = GetArenaEntryFee();
	SetMySpendForArena( singleFee * entCnt );
}
//==========================================================================================================================
#if 0

#define ArenaLevelLimit 30

MMOArenaManager::MMOArenaManager()
	:_ArenaMode(0)
	,_ArenaSize(ArenaSize_1vs1)
	,_HasUpdateArenaSummary(FALSE)
	,_HasUpdatePlayerInfo(FALSE)
	,_StartArenaTick(FALSE)
	,_SpareTime(0)
	,_LastSpareTime(0)
	,_CDMovieTime(0)
	,_CDMoviePlayed(FALSE)
	,_ArenaWaitTime(0)
	,_ArenaStep(ArenaStep_None)
{
	
	Init();
}

MMOArenaManager::~MMOArenaManager()
{

}

void MMOArenaManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* msg = (MMOGameEvent_PlayerBaseInfo*)evt;
			if( msg )
			{
				ArenaPlayerData* playerData = GetArenaPlayer( msg->_RoldID );
				if( playerData )
				{
					PlayerBaseInfo *baseInfo = msg->_PlayerBaseInfo;
					if( baseInfo )
					{
						playerData->classId = baseInfo->_RoleClass;
						playerData->roleName = baseInfo->_RoleName;
						playerData->sex = baseInfo->_Sex;
						playerData->level = baseInfo->_Level;

						CheckAllDataUpate();
					}
				}
			}
		}
		break;
	}
}

void MMOArenaManager::Init()
{
	MMOUI::RegisterEventHandler( this , GameEvent_PlayerBaseInfo );

	TArray<FString> cdTimeArray;
	GConfig->GetArray( TEXT("Arena") , TEXT("CDHint") , cdTimeArray , GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("ArenaFightTime") , _ArenaFightTime , GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("CDMovieTime") , _CDMovieTime , GDefaultGameIni );
	GConfig->GetInt( TEXT("Arena") , TEXT("ArenaWaitTime") , _ArenaWaitTime , GDefaultGameIni );

	_CDHintTime.AddZeroed( cdTimeArray.Num() );
	for ( INT i = 0 ; i < cdTimeArray.Num() ; i++ )
	{
		_CDHintTime(i) = appAtoi( *cdTimeArray(i) );
	}
	_CDHintActived.AddZeroed( cdTimeArray.Num() );
}

ArenaSize  MMOArenaManager::MapArenaSize( INT size )
{
	switch(size)
	{
	case 0:
		return ArenaSize_1vs1;
	case 1:
		return ArenaSize_2vs2;
	case 2:
		return ArenaSize_3vs3;
	case 3:
		return ArenaSize_5vs5;

	}
	return ArenaSize_1vs1;
}



void MMOArenaManager::PrepareArenaEnter( INT modeId , INT sizeId )
{
	_ArenaMode = MapArenaModeByIdx( modeId );	
	_ArenaSize = MapArenaSize( sizeId );

	//determine whether the party size is valid (and all from the same team)
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	TeamManager& teamManager = TeamManager::getSingleton();
	UBOOL valid = TRUE;

	if(!player)
	{
		return;
	}

	if( player->GetActorLevel() < ArenaLevelLimit )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.LevelUnfit")) ,MsgInfoType_Table,TEXT("MMOGame.Arena.LevelUnfit"));
		valid = FALSE;
	}
	else if( _ArenaSize == ArenaSize_1vs1 )
	{
		if( teamManager.HasTeam() )
		{
			valid = FALSE;
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.SingleArenaNeedNoTeam")) ,MsgInfoType_Table,TEXT("MMOGame.Arena.SingleArenaNeedNoTeam"));
		}
		
	}
	else
	{
		if( !teamManager.HasTeam() || teamManager.GetTeamMemberCount() != _ArenaSize )
		{
			valid = FALSE;
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.TeamUnfit")),MsgInfoType_Table,TEXT("MMOGame.Arena.TeamUnfit") );
		}
		else
		{
			for ( INT i = 0 ; i <  teamManager.GetTeamMemberCount() ; i++ )
			{
				MMOTeamMember* mem = teamManager.GetTeamMember(i);
				if(mem)
				{
					if( !BattleTeamManager::getSingleton().HasMember( mem->ActorServerId ) )
					{
						valid = FALSE;
						MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Arena.TeamUnfit")),MsgInfoType_Table,TEXT("MMOGame.Arena.TeamUnfit") );
						break;
					}
				}
			}
		}
	}

	if( !valid )
	{
		return;
	}

	CMessageHandler::GetInstance().Send_PrepareArenaEnter( _ArenaMode , _ArenaSize );
}

void MMOArenaManager::OpenArenaQueue( INT aveTime , UBOOL newQueue )
{

	MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaQueue , TRUE, "" );
	UMMOUIArenaQueue* panel = Cast<UMMOUIArenaQueue>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_PVP_ArenaQueue ) );

	if(newQueue)
	{
		//a new queue entry is set to queued state
		SetArenaStep(ArenaStep_Queuing);
	}

	if( panel )
	{
		panel->OpenArenaQueue( _ArenaMode , (INT)_ArenaSize  );

		if ( _ArenaMode == ArenaSize_1vs1 )
		{
			MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Battlefield);
		}
	}
}

void MMOArenaManager::LeaveArena()
{
	_StartArenaTick = FALSE;
	MMOUI::GetUIManager()->delegateCloseArenaScore();
	CMessageHandler::GetInstance().Send_LeaveScene();
	_ArenaStep = ArenaStep_None;
}

void MMOArenaManager::ClearArena()
{
	_ArenaStep = ArenaStep_None;
	_StartArenaTick = FALSE;
}

INT MMOArenaManager::MapArenaModeByIdx( INT index )
{
	switch(index)
	{
	case 0:
		return artPractice;
	case 1:
		return artRace;
	default:
		return artPractice;
	}
	return artPractice;
}

const ArenaTeamInfo* MMOArenaManager::FindAreanTeam(UINT32 Id)
{
	AllArenaTeamMap::iterator it = _AllArenaTeamMap.find(Id);
	if (it == _AllArenaTeamMap.end())
		return NULL;
	else
		return &(it->second);
}

void MMOArenaManager::AddOrUpdateArenaTeamInfo(M2C_ARENA_TEAM_INFO* msg)
{
	ASSERT_CHECK_ERROR_RETURN(msg);
	AllArenaTeamMap::iterator it = _AllArenaTeamMap.find(msg->dwArenaTeamID);
	if (it == _AllArenaTeamMap.end())
	{
		ArenaTeamInfo arenaTeamInfo;
		arenaTeamInfo.AreanTeamID = msg->dwArenaTeamID;
		arenaTeamInfo.AreanTeamName = UTF8_TO_TCHAR(msg->szName);
		arenaTeamInfo.Captain = UTF8_TO_TCHAR(msg->stMemberList[0].szRoleName);
		arenaTeamInfo.IconID = msg->wIconID;
		for (INT index = 0; index < atptTotal; index++)
		{
			arenaTeamInfo.ArenaStatDataVec.push_back(msg->StatData[index]);
		}
		for (INT index = 0; index < msg->byMemberCount; index++)
		{
			arenaTeamInfo.ArenaMemBaseInfoVec.push_back(msg->stMemberList[index]);
		}
		_AllArenaTeamMap.insert(std::make_pair(msg->dwArenaTeamID, arenaTeamInfo));
	}
	else
	{
		
		it->second.AreanTeamID = msg->dwArenaTeamID;
		it->second.AreanTeamName = UTF8_TO_TCHAR(msg->szName);
		it->second.Captain = UTF8_TO_TCHAR(msg->stMemberList[0].szRoleName);
		it->second.IconID = msg->wIconID;
		it->second.ArenaStatDataVec.clear();
		for (INT index = 0; index < atptTotal; index++)
		{
			it->second.ArenaStatDataVec.push_back(msg->StatData[index]);
		}
		it->second.ArenaMemBaseInfoVec.clear();
		for (INT index = 0; index < msg->byMemberCount; index++)
		{
			it->second.ArenaMemBaseInfoVec.push_back(msg->stMemberList[index]);
		}
	}
}

void MMOArenaManager::SetSingleScoreData( UBOOL win , INT roleId , INT curScore , INT scoreGain )
{
	ArenaPlayerData& role = win?_Winner:_Loser;
	
	role.addScore = scoreGain;
	role.currentScore = curScore;
	role.roleID = roleId;

	PlayerBaseInfo *baseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(roleId);
	if(baseInfo)
	{
		role.roleName = baseInfo->_RoleName;
		role.classId = baseInfo->_RoleClass;
		role.sex = baseInfo->_Sex;
		role.level = baseInfo->_Level;
	}

}

void MMOArenaManager::SetTeamScoreData( UBOOL win , INT teamId , INT curTeamScore , INT scoreGain )
{

}

void MMOArenaManager::StartArena()
{
	_HasUpdateArenaSummary = FALSE;
	_HasUpdatePlayerInfo = FALSE;
	_Winner.roleName.Empty();
	_Loser.roleName.Empty();
}

void MMOArenaManager::StartArenaTick( UINT32 serveTime )
{
		
	__time32_t localBattleStartServerTime = _ArenaWaitTime + serveTime;
	_localtime32(&localBattleStartServerTime);

	//Current local time
	SYSTEMTIME curTime;		
	::GetLocalTime(&curTime);
	UINT32 curTimeSec = MMOFacade::getSingleton().ConvertSystemTimeToTmTSecond( curTime );

	//Time difference between local and server
	INT32 secDiff = MMOFacade::getSingleton().GetClientToSceneSecondDiff();

	INT sinceStartTimeSec = curTimeSec - ( localBattleStartServerTime + secDiff );

	for ( INT i = 0 ; i < _CDHintActived.Num() ; i++ )
	{
		_CDHintActived(i) = FALSE;
	}
	_CDMoviePlayed = FALSE;

	
	_SpareTime = -sinceStartTimeSec;
	_LastSpareTime = _SpareTime;
	_StartArenaTick = TRUE;

	_ArenaStep = ArenaStep_Arening;
}

ArenaPlayerData* MMOArenaManager::GetArenaPlayer( INT roleId )
{
	if( _Winner.roleID == roleId )
	{
		return &_Winner;
	}
	else if( _Loser.roleID == roleId )
	{
		return &_Loser;
	}
	return NULL;
}

void MMOArenaManager::UpdateArenaSummary( G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA* data , INT count )
{
	for ( INT i = 0 ; i < count ; i++ )
	{
		INT roleId = data[i].dwRoleID;
		ArenaPlayerData* arenaPlayer = GetArenaPlayer(roleId);
		if( arenaPlayer )
		{
			arenaPlayer->kill = data[i].Data[escsRoleKillCount];
			arenaPlayer->damage = data[i].Data[escsTotalDamage];
		}
	}
	_HasUpdateArenaSummary = TRUE;
	CheckAllDataUpate();
}

void MMOArenaManager::CheckAllDataUpate()
{
	if( !_HasUpdateArenaSummary )
	{
		return;
	}

	if( _Winner.roleName.IsEmpty() )
	{
		return;
	}

	if( _Loser.roleName.IsEmpty() )
	{
		return;
	}

	AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
	if( !playerCtrl )
	{
		return;
	}
	FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
	if( pMapInfo != NULL )
	{
		INT mapType = pMapInfo->ServerMapType;
		if( mapType == E_est_Arena  )
		{
			MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaScore , TRUE, "" );
		}
	}
}

void MMOArenaManager::TickArena( FLOAT deltaTime )
{
	if( _StartArenaTick ) //arenastep == arening
	{
		AMMOPlayerController* playerCtrl = MMOFacade::getSingleton().GetMainController();
		if( !playerCtrl )
		{
			return;
		}


		_SpareTime -= deltaTime;

		for ( INT i = 0 ; i < _CDHintTime.Num() ; i++ )
		{
			if( !_CDHintActived(i) && ( _SpareTime <= _CDHintTime(i) ) && ( _LastSpareTime > _CDHintTime(i) ) )
			{
				FString txt = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.CDHint") ) , _CDHintTime(i) );
				MMOUI::GetUIManager()->delegateAddScreenMessage( txt ,MsgInfoType_Table,TEXT("MMOGame.Arena.CDHint"));
				_CDHintActived(i) = TRUE;

			}
		}
		

		if( !_CDMoviePlayed && ( _SpareTime <= _CDMovieTime ) && ( _LastSpareTime > _CDMovieTime ) )
		{
			//play 10-second countdown
			
			FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
			if( pMapInfo != NULL )
			{
				INT mapType = pMapInfo->ServerMapType;
				if( mapType == E_est_Arena  )
				{
					MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint , TRUE, "" );
					UMMOUIFightingScreenHint *pFightingScreenHint = Cast<UMMOUIFightingScreenHint>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint ) );

					if( pFightingScreenHint )
					{
						pFightingScreenHint->PlayTenSecCountDownAfter();
					}
				}
			}
			_CDMoviePlayed = TRUE;
		}

		if( _SpareTime <= 0 )
		{
			//open the prompt button
			FExportMap *pMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( playerCtrl->CurMapID );
			if( pMapInfo != NULL )
			{
				INT mapType = pMapInfo->ServerMapType;
				if( mapType == E_est_Arena  )
				{
					MMOUI::GetUIManager()->ShowPanelEx( MMOUI::GetUIManager()->Panel_PVP_ArenaHint , TRUE, "" );
					UMMOUIArenaHint* hint = Cast<UMMOUIArenaHint>( MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_PVP_ArenaHint ) );
					if(hint)
					{
						hint->StartFight( _ArenaFightTime );

					}
				}
			}


			_StartArenaTick = FALSE;
		}

		_LastSpareTime = _SpareTime;
	}
	else if( _ArenaStep == ArenaStep_Queuing )
	{
		_LastQueueTime = _QueueTime;
		_QueueTime += deltaTime;
		
	}
	else if( _ArenaStep == ArenaStep_WaitEnter )
	{
		_LastWaitEnterTime = _WaitEnterTime;
		_WaitEnterTime -= deltaTime;
	}
}

void MMOArenaManager::SetArenaStep( ArenaStep step )
{ 
	_ArenaStep = step;

	if( step == ArenaStep_WaitEnter )
	{
		_WaitEnterTime = 30;
		_LastWaitEnterTime = 30;
	}
	else if( step == ArenaStep_Queuing )
	{
		_QueueTime = 0;
		_LastQueueTime = 0;
	}
}
#endif