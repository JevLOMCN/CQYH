#include "stdafx.h"
#include "MMOBattleManager.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
//#include "MMOGameUIClasses.h"
#include "CommonGameEvents.h"
#include "net/message_handler.h"
#include "MMOMapDataManager.h"
#include "DataErrorCheck.h"

using namespace ExternalMessage;

//template<> MMOBattleManager* Singleton<MMOBattleManager>::ms_Singleton = 0;

IMPLEMENT_COMPARE_CONSTREF( FBattlePlayer, __BattlePoints, { return (A._BtlTeamID == B._BtlTeamID) ? (A._Awards._Points < B._Awards._Points ? 1 : -1) : (A._BtlTeamID > B._BtlTeamID ? 1 : -1); } );
IMPLEMENT_COMPARE_CONSTREF( FBattlePlayer, __BattleScore, { return (A._BtlTeamID == B._BtlTeamID) ? (A._Score._Sum < B._Score._Sum ? 1 : -1) : (A._BtlTeamID > B._BtlTeamID ? 1 : -1); } );
IMPLEMENT_COMPARE_CONSTREF( FBattlePlayer, __BattleKills, { return (A._BtlTeamID == B._BtlTeamID) ? (A._Score._Kills < B._Score._Kills ? 1 : -1) : (A._BtlTeamID > B._BtlTeamID ? 1 : -1); } );
IMPLEMENT_COMPARE_CONSTREF( FBattlePlayer, __BattleBeKilled, { return (A._BtlTeamID == B._BtlTeamID) ? (A._Score._BeKilled < B._Score._BeKilled ? 1 : -1) : (A._BtlTeamID > B._BtlTeamID ? 1 : -1); } );


MMOBattleManager::MMOBattleManager(void)
	: _BattlesInfo(0)
{

}
MMOBattleManager::~MMOBattleManager(void)
{

}
//
void MMOBattleManager::Init()
{
	M_BeginLoadData(TEXT("BattleManager"));
	static bool g_loadData = false;
	if(g_loadData)
	{
		return;
	}

	g_loadData = true;

	SCOPE_LOG_TIME(___INIT__, TEXT("MMOBattleManager::Init"), 0);
#if 0 // no such function
	MMOUI::RegisterEventHandler( this, GameEvent_PlayerBaseInfo);

#if 1
	FString cnfgName;
	if(GConfig->GetString(TEXT("SingletonData"), TEXT("MMOBattleTemplate"), cnfgName, GGameIni))
	{
		_BattlesInfo = Cast<UMMOBattleTemplate>(UObject::StaticLoadObject(
			UMMOBattleTemplate::StaticClass(), NULL, *cnfgName, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));

		_BattlesInfo->AddToRoot();
	}
	else
	{
		appErrorf(TEXT("MMOBattleTemplate not load! Check in MMOGame/Config/DefaultGame.ini"));
	}
#else
	TArray<FString> DirArray;
	GConfig->GetArray( TEXT("BattleTemplates"), TEXT("TemplateDir"), DirArray, GGameIni );

	MMO::FormatTempleteDataLoadingDir(DirArray);

	for (INT k = 0; k < DirArray.Num(); ++k)
	{
		const FString& cfgDir = DirArray(k);

		debugIDf(TEXT("TemplateData"), TEXT("===== Loading Battles template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if( !Package )
			{
				warnf( NAME_Error, TEXT("Unable to load %s"), *pkgs(i) );
				continue;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}

			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UMMOBattleTemplate* battle = Cast<UMMOBattleTemplate>(Package->GetNetObjectAtIndex(j));
				if(battle)
				{
					battle->AddToRoot();

					_BattlesInfo = battle;
					//MMOBattleManager::getSingleton().Init( battle );
					break;
				}
			}//end of Objects in package
		}//end of FilesInDirectory
	}//end of DirArray
#endif
	//get actived battles
	UGlobalMapInfo::GetSingleton()->FindAllMapOfType( E_est_BattleField, _ActivedBattles );
#endif
}
//
void MMOBattleManager::Shutdown()
{
	MMOUI::UnRegisterEventHandler( this, GameEvent_PlayerBaseInfo);

	if( _BattlesInfo != NULL ){
		_BattlesInfo->RemoveFromRoot();
	}
}
//
void MMOBattleManager::ResetRandomBattleData()
{
	//zero last random battles
	_RandomBattlesList.Empty();

	if(!_BattlesInfo)
	{
		return;
	}

	
	_BattlesInfo->RevertSelected();
	//Add minimal battles
	INT cnt = GetRandomMin();
	FBattleTemplate *pBattle = NULL;
	for ( INT i = 0 ; i < cnt; ++i){
		pBattle = GetBattleTemplateIn( i );
		if( pBattle == NULL){
			//this is config error
			continue;
		}
		ToggleRandomBattles( pBattle->ID, TRUE );
	}
}
//
void MMOBattleManager::ResetEnteredBattle()
{
	_IsBattleBegin = FALSE;
	_EnteredBattleID = -1;
	_IsDogfall = FALSE;
	_WinTeamID = -1;

	for( TMap<INT,INT>::TIterator it(_BattleTeamScore); it; ++it ){
		it.Value() = 0;
	}
	_BattlePlayerList.Empty();
	//
	_RoleInfoIncompleteList.Empty();

	//
	UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );
	//if( pBattleScoreBoard == NULL ){
	//	MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_BattleSummary );
	//	pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );
	//}

	if( pBattleScoreBoard != NULL ){
		pBattleScoreBoard->ResetShow();
	}
}
//void UBattleManager::UpdatePlayerData( int playerID );
void MMOBattleManager::DelPlayer( INT playerID )
{
	_BattlePlayerList.Remove( playerID );
	//std::map<INT, FBattlePlayer>::iterator it;
	//it = BattlePlayerList.find( playerID );
	//if( it != BattlePlayerList.end() ){
	//	//found,del it
	//	BattlePlayerList.erase( it );
	//}
}
//
const FBattlePlayer *MMOBattleManager::GetPlayer( INT playerID )
{
	return _BattlePlayerList.Find( playerID );

}
//------------------------------------------------------------------------
//
const FBattlePlayer *MMOBattleManager::GetMyBattleScoreInfo()
{
	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( pMainPlayer == NULL ){
		return NULL;
	}

	const FBattlePlayer *pPersonalScore = GetPlayer( pMainPlayer->ServerID );

	return pPersonalScore;
}

//
INT MMOBattleManager::GetWinTeamID()
{
	if( _IsDogfall == TRUE ){
		return 0;
	}
	return _WinTeamID;

	//if( _WinTeamID >= 0 ){
	//	return _WinTeamID;
	//}
	//INT nowHighScore = -1;
	//INT hightScoreTeamID = -1;
	//for( TMap<INT,INT>::TIterator it(_BattleTeamScore); it; ++it ){
	//	INT score = it.Value();
	//	if( nowHighScore < score ){
	//		nowHighScore = score;
	//		hightScoreTeamID = it.Key();
	//	}
	//}

	//return hightScoreTeamID;
}

//
UBOOL MMOBattleManager::IsInBattle()
{
	FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();

	return ( mapInfo.ServerMapType == E_est_BattleField );
	//return ( _BattleState == EBTLS_BEGINING || _BattleState == EBTLS_FIGHTING ) ? TRUE : FALSE; }
}
//------------------------------------------------------------------------
void MMOBattleManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* infoEvt = (MMOGameEvent_PlayerBaseInfo*)evt;
			if(infoEvt == NULL){
				return;
			}

			NotifyPlayerBaseInfo( infoEvt->_RoldID );
		}
		break;
	}

}
//
void MMOBattleManager::NotifyPlayerBaseInfo( UINT32 roleID )
{
	//
	if( !( IsInBattle() || IsBattleEnding() ) ){
		return;
	}
	//
	if( _RoleInfoIncompleteList.Find( roleID ) == NULL ){
		return;
	}

	FBattlePlayer *pBattlePlayer = GetOrAddPlayer( roleID );
	PlayerBaseInfo *pPlayerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo( pBattlePlayer->_ID );
	if( pPlayerInfo == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [GetPlayerBaseInfo result NULL In MMOBattleManager::NotifyPlayerBaseInfo]") );
		return;
	}

	pBattlePlayer->_PlayerName = pPlayerInfo->_RoleName;
	//pBattlePlayer->_ServerName = FString::Printf( TEXT("Server %d"), BattleTestPlayerIdx );
	pBattlePlayer->_Sex = pPlayerInfo->_Sex;
	pBattlePlayer->_ClassID = pPlayerInfo->_RoleClass;
	pBattlePlayer->_Lv = pPlayerInfo->_Level;

	//remove this 
	_RoleInfoIncompleteList.Remove( roleID );

	//if getted all data open summary panel
	UINT leftDataCnt = _RoleInfoIncompleteList.Num();
	if( leftDataCnt == 0 ){
		UMMOUIBattleSummary *pBattleSummary = (UMMOUIBattleSummary*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BattleSummary_BattleSummary );
		pBattleSummary->TryToShow();
	}
	else if( _RoleInfoIncompleteListUnSended.Num() > 0 ){
		TMap< UINT, UBOOL >::TIterator it(_RoleInfoIncompleteListUnSended);
		if( it ){
			MMOFacade::getSingleton().RequestPlayerBaseInfo( it.Key() );

			_RoleInfoIncompleteListUnSended.Remove( it.Key() );
		}
	}
}
//
void MMOBattleManager::NotifyBattleEnd( UBOOL isDogFall, INT winTeamID )
{
	_BattleState = EBTLS_ENDING;
	_IsDogfall = isDogFall;
	_WinTeamID = winTeamID;
	//
	UMMOUIBattleSummary *pBattleSumPanel = NULL;
	pBattleSumPanel = (UMMOUIBattleSummary*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BattleSummary_BattleSummary );
	if( pBattleSumPanel == NULL ){
		return;
	}
	//
	pBattleSumPanel->NotifyBattleEnd();

	//
	UMMOUIBattleScoreBoard *pBattleScoreBoard = (UMMOUIBattleScoreBoard*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_BattleSummary_BattleScoreBoard );

	if( pBattleScoreBoard != NULL ){
		pBattleScoreBoard->eventForceStopBattleTime();
	}
}
//
MMOBattleManager::BattlePlayerIterator MMOBattleManager::BeginIterateBattlePlayer()
{
	BattlePlayerIterator it(_BattlePlayerList);
	return it;
}
//
void MMOBattleManager::SortedPlayersByPointsInTeam()
{
	_BattlePlayerList.ValueSort<COMPARE_CONSTREF_CLASS(FBattlePlayer,__BattlePoints)>();
}
//
void MMOBattleManager::SortedPlayersByScore()
{
	_BattlePlayerList.ValueSort<COMPARE_CONSTREF_CLASS(FBattlePlayer,__BattleScore)>();
}
//
void MMOBattleManager::SortedPlayersByKills()
{
	_BattlePlayerList.ValueSort<COMPARE_CONSTREF_CLASS(FBattlePlayer,__BattleKills)>();
}
//
void MMOBattleManager::SortedPlayersByBeKilled()
{
	_BattlePlayerList.ValueSort<COMPARE_CONSTREF_CLASS(FBattlePlayer,__BattleBeKilled)>();
}
//
eBattlePlayerState MMOBattleManager::CvtToBattleState( INT iState )
{
	return EBPS_ALIVE;
}
//================================================================================================
//Battlefield join
void MMOBattleManager::ResetJoinedBattleData()
{
	_PrepareJoinedBtlID = -1;
	_JoinedBattleID = -1;
	_BattleState = EBTLS_NONE;
}
//
void MMOBattleManager::ToggleRandomBattles( INT id, UBOOL isChoosed )
{
	if( isChoosed == TRUE ){
		//select this battle
		_RandomBattlesList.FindOrAdd( id );
	}
	else{
		//cancel select
		_RandomBattlesList.Remove( id );
	}
	//
	FBattleTemplate* battleInfo = FindBattleTemplateBy( id );
	if( battleInfo != NULL ){
		battleInfo->IsSelected = isChoosed;
	}
}
//
void MMOBattleManager::RandomJoinBattle()
{
	INT randomCnt = _RandomBattlesList.Num();
	if( randomCnt == 0 ){
		//no data,error
		return;
	}

	_PrepareJoinedBtlID = RANDOM_BATTLE_ID;

	UGlobalMapInfo *pGlobalMapInfo = UGlobalMapInfo::GetSingleton();
	UINT selectedBattleBit = 0;
	for( RandomBattleMap::TConstIterator iter(_RandomBattlesList); iter; ++iter )
	{
		INT idxInServerMap = pGlobalMapInfo->FindIdxInMapOfType( iter.Key(), E_est_BattleField );
		selectedBattleBit |= ( 1 << idxInServerMap );
	}
	CMessageHandler::GetInstance().Send_Join_Battles( selectedBattleBit );
}
//
//void MMOBattleManager::TeamJoinBattle( INT battleID )
//{
//	//
//	PrepareJoinedBtlID = battleID;
//	//
//	FBattleTemplate* battleInfo = FindBattleTemplateBy( battleID );
//	if( battleInfo == NULL ){
//		return;
//	}
//	//
//	CMessageHandler::GetInstance().Send_TeamJoin_Battle( battleInfo->Mode, battleID );
//}
//
//void MMOBattleManager::SingleJoinBattle( INT battleID )
//{
//	//
//	PrepareJoinedBtlID = battleID;
//	//
//	CMessageHandler::GetInstance().Send_SingleJoin_Battle( battleID );
//}
void MMOBattleManager::JoinSingleBattle( INT battleID )
{
	//
	_PrepareJoinedBtlID = battleID;
	//
	INT idxInServerMap = UGlobalMapInfo::GetSingleton()->FindIdxInMapOfType( _PrepareJoinedBtlID, E_est_BattleField );
	if( idxInServerMap < 0 ){
		return;
	}

	UINT selectedBattleBit = ( 1 << idxInServerMap );
	CMessageHandler::GetInstance().Send_Join_Battles( selectedBattleBit );
}
//
void MMOBattleManager::NotifyJoinedBattle()
{
	//
	_JoinedBattleID = _PrepareJoinedBtlID;
	_BattleState = EBTLS_QUEUEING;

	::GetLocalTime(&_JoinLocalTime);
	//PrepareJoinedBtlID = -1;
	//
	UMMOUIPVPBattle *pvpBattlePanel = NULL;
	pvpBattlePanel = (UMMOUIPVPBattle*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_PVP_PVPBattle );
	if( pvpBattlePanel == NULL ){
		return;
	}
	//
	pvpBattlePanel->NotifyJoinedBattle( _JoinedBattleID );

	//
	UMMOUIMinimap *minimapPanel = NULL;
	minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( minimapPanel == NULL ){
		return;
	}
	minimapPanel->NotifyJoinedBattleQueue();
}
//
void MMOBattleManager::LeaveWaitingBattle()
{
	//
	CMessageHandler::GetInstance().Send_Leave_WaitingBattle();
}
//
void MMOBattleManager::NotifyLeaveBattle()
{
	//
	_BattleState = EBTLS_NONE;
	_JoinedBattleID = -1;
	_PrepareJoinedBtlID = -1;
	//
	UMMOUIPVPBattle *pvpBattlePanel = NULL;
	pvpBattlePanel = (UMMOUIPVPBattle*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_PVP_PVPBattle );
	if( pvpBattlePanel == NULL ){
		return;
	}
	//
	pvpBattlePanel->NotifyLeaveBattle();

	//
	UMMOUIMinimap *minimapPanel = NULL;
	minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( minimapPanel == NULL ){
		return;
	}
	minimapPanel->NotifyLeaveBattleQueue();
}

//
void MMOBattleManager::NotifyEnterBattle( INT battleID )
{
	//
	//_JoinedBattleID = _PrepareJoinedBtlID;
	_JoinedBattleID = battleID;
	_BattleState = EBTLS_INQUIRING_ENTER;
	//
	UMMOUIPVPBattle *pvpBattlePanel = NULL;
	pvpBattlePanel = (UMMOUIPVPBattle*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PVP_PVPBattle );

	if( pvpBattlePanel == NULL ){
		warnf( NAME_Error, TEXT("UMMOUIPVPBattle not found at NotifyEnterBattle battleID:%d"), battleID );
		return;
	}
	//
	pvpBattlePanel->NotifyEnterBattle();
	////
	//if( pvpPanel->IsVisible ){
	//	pvpPanel->Show( FALSE );
	//}

	//
	UMMOUIMinimap *minimapPanel = NULL;
	minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( minimapPanel == NULL ){
		return;
	}
	minimapPanel->NotifyLeaveBattleQueue();
}
//Confirm join
void MMOBattleManager::ConfirmJoinBattle( UBOOL isAgree )
{
	//clear the data of the selecting battle
	_BattleState = EBTLS_BEGINING;
	_PrepareJoinedBtlID = -1;
	//
	CMessageHandler::GetInstance().Send_Ack_JoinBattle( isAgree );
	//clear the data of last battle players
	ResetEnteredBattle();
}
//Battlefield info
INT MMOBattleManager::GetBattleTemplateCnt()
{
	//return _BattlesInfo->GetBattleTemplateCnt();
	return _ActivedBattles.Num();
}
//
FBattleTemplate* MMOBattleManager::FindBattleTemplateBy( INT battleID )
{
	if(_BattlesInfo)
	{
		return _BattlesInfo->FindBattleTemplateBy( battleID );
	}
	else
	{
		return NULL;
	}
}
//
FBattleTemplate* MMOBattleManager::GetBattleTemplateIn( INT idx )
{
	if( _ActivedBattles.IsValidIndex( idx ) && _BattlesInfo )
	{
		return _BattlesInfo->FindBattleTemplateBy( _ActivedBattles( idx ) );
	}
	return NULL;
}
//
INT MMOBattleManager::GetBattleIdxByID( INT battleID )
{
	INT cnt = _ActivedBattles.Num();
	INT idx = 0;
	for( ; idx < cnt; ++idx ){
		if( _ActivedBattles(idx) == battleID ){
			return idx;
		}
	}

	return -1;
}
//
INT MMOBattleManager::GetCurBattleSidesCnt()
{
	FBattleTemplate* pBattle = GetEnteredBattle();
	if( pBattle == NULL ){
		return 0;
	}

	FExportMap mapExportInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( pBattle->ID );
	//INT sidesCnt = 2;
	//if( mapExportInfo.BattleType == E_btt_10V10V10 ){
	//	sidesCnt = 3;
	//}

	//return sidesCnt;

	return mapExportInfo.SideCount;
}
//
FBattleTemplate* MMOBattleManager::GetEnteredBattle()
{
	FBattleTemplate* pBattle = NULL;
	if( (_EnteredBattleID > 0)  && _BattlesInfo)
	{
		pBattle = _BattlesInfo->FindBattleTemplateBy( _EnteredBattleID );
	}

	return pBattle;
}

//
//
FExportMap *MMOBattleManager::GetEnteredBattleMapData()
{
	FBattleTemplate* pBattle = GetEnteredBattle();
	if( pBattle == NULL ){
		return NULL;
	}

	return UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( pBattle->ID );
}
//
//---------------------------------------------------------------------
void MMOBattleManager::ParseBattleCombatDatas( G2C_SYNC_COMBAT_STAT* serverMsg, UINT dataCnt )
{
	if( serverMsg == NULL ){
		return;
	}

	//clear first
	ClearBattlePlayer();

	TMap< UINT, UBOOL > &roleInfoNeed = GetRoleIncompleteList();

	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	for (UINT i = 0; i < dataCnt; ++i){

		G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA* pSummaryData = ( G2C_SYNC_COMBAT_STAT::ROLE_STAT_DATA* )&serverMsg->DataList[i];
		FBattlePlayer *pBattlePlayer = GetOrAddPlayer( pSummaryData->dwRoleID );

		//0 is own side
		pBattlePlayer->_BtlTeamID = pSummaryData->byFaction;
		pBattlePlayer->_ID = pSummaryData->dwRoleID;

		if( pBattlePlayer->_PlayerName.IsEmpty() ){
			PlayerBaseInfo *pPlayerInfo = mmoFacade.GetPlayerBaseInfo( pBattlePlayer->_ID );

			if( pPlayerInfo != NULL ){
				pBattlePlayer->_PlayerName = pPlayerInfo->_RoleName;
				//pBattlePlayer->_ServerName = FString::Printf( TEXT("Server %d"), BattleTestPlayerIdx );
				pBattlePlayer->_Sex = pPlayerInfo->_Sex;
				pBattlePlayer->_ClassID = pPlayerInfo->_RoleClass;
				pBattlePlayer->_Lv = pPlayerInfo->_Level;
			}
			else{
				//prepare ask for data
				roleInfoNeed.FindOrAdd( pBattlePlayer->_ID ) = TRUE;
			}
		}

		pBattlePlayer->_Score._Sum = pSummaryData->Data[escsScore1];
		pBattlePlayer->_Score._Kills = pSummaryData->Data[escsRoleKillCount];
		pBattlePlayer->_Score._BeKilled = pSummaryData->Data[escsDeathCount];
		pBattlePlayer->_Score._Damages = pSummaryData->Data[escsTotalDamage];
		pBattlePlayer->_Score._Heals = pSummaryData->Data[escsTotalHeal];
		//
		//switch( pBattle->BattleStyle ){
		//case EBS_GetCoreItem: pBattlePlayer->_Score._Kills = 0;
		//	break;
		//default:
		//	break;
		//}
		pBattlePlayer->_Awards._Points = pSummaryData->Data[escsTotalScore];

		pBattlePlayer->_State = EBPS_ALIVE;
		//pBattlePlayer->_IsLeader = 1;
	}

	//
	UINT needCnt = roleInfoNeed.Num();
	if( needCnt == 0 ){
		UMMOUIBattleSummary *pBattleSummary = (UMMOUIBattleSummary*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_BattleSummary_BattleSummary );
		pBattleSummary->Show( TRUE );
	}
	else{
		//Server policy, at most 15 each time
		//UINT sendCnt = 0;
		TMap< UINT, UBOOL >::TIterator it(roleInfoNeed);
		for( ; it ; ++it ){
			MMOFacade::getSingleton().RequestPlayerBaseInfo( it.Key() );
			//++sendCnt;
		}
		//Record those not yet sent
		//_RoleInfoIncompleteListUnSended.Empty();
		//for( ; it; ++it ){
		//	_RoleInfoIncompleteListUnSended.FindOrAdd( it.Key() ) = TRUE;
		//}
	}
	////test
	//for( TMap< UINT, UBOOL >::TIterator it(roleInfoNeed); it; ++it ){
	//	M2C_SYNC_ROLE_INFO* serverMsg = (M2C_SYNC_ROLE_INFO*)ms_MsgBuffer;
	//	serverMsg->wMsg = m2c_sync_role_info;
	//	serverMsg->dwRoleID = it.Key();
	//	strxcpy( serverMsg->szRoleName, TEXT("Player name"), ROLE_NAME_LEN);
	//	serverMsg->byClass = 1;
	//	serverMsg->bySex = 1;
	//	Parse_m2c_sync_role_info( (const char*)serverMsg, sizeof(M2C_SYNC_ROLE_INFO) );
	//}

	////test
	//G2C_SYNC_SCENE_VARIABLES* serverMsg = (G2C_SYNC_SCENE_VARIABLES*)ms_MsgBuffer;
	//serverMsg->wMsg = g2c_sync_scene_variables;
	//UINT size = sizeof(G2C_SYNC_SCENE_VARIABLES) + sizeof(G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA)*2;
	//serverMsg->wSize = size;
	//serverMsg->VarList[0].wVarIndex = 2;
	//serverMsg->VarList[0].nValue = pBattleManager->GetBattleTeamScore( EBTLTMS_Team0 ) + 1;
	//serverMsg->VarList[1].wVarIndex = 3;
	//serverMsg->VarList[1].nValue = pBattleManager->GetBattleTeamScore( EBTLTMS_Team1 ) + 1;
	//Parse_g2c_sync_scene_variables( (const char*)serverMsg, size );
}

//---------------------------------------------------------------------
//test
void MMOBattleManager::TestAddData()
{
	static INT BattleTestPlayerIdx = 0;
	++BattleTestPlayerIdx;
	//for (INT BattleTestPlayerIdx = 1; BattleTestPlayerIdx <= 39; ++BattleTestPlayerIdx)
	//{
	FBattlePlayer *pBattlePlayer = GetOrAddPlayer(BattleTestPlayerIdx);
	pBattlePlayer->_ID = BattleTestPlayerIdx;
	pBattlePlayer->_Sex = BattleTestPlayerIdx % 2;
	pBattlePlayer->_ClassID = BattleTestPlayerIdx % 2;
	pBattlePlayer->_Lv = BattleTestPlayerIdx;

	pBattlePlayer->_Score._Sum = BattleTestPlayerIdx * 1000;
	pBattlePlayer->_Score._Kills = BattleTestPlayerIdx * 100;
	pBattlePlayer->_Score._BeKilled = BattleTestPlayerIdx * 10;
	pBattlePlayer->_Score._Damages = BattleTestPlayerIdx * 10000;
	pBattlePlayer->_Score._Heals = BattleTestPlayerIdx * 1000;

	pBattlePlayer->_Awards._Points = BattleTestPlayerIdx * 300;

	pBattlePlayer->_ServerName = FString::Printf( TEXT("Server %d"), BattleTestPlayerIdx );
	pBattlePlayer->_PlayerName = FString::Printf( TEXT("Player %d"), BattleTestPlayerIdx );

	pBattlePlayer->_State = EBPS_ALIVE;
	pBattlePlayer->_IsLeader = ( BattleTestPlayerIdx % 19 == 0 );
	//0 is own side
	pBattlePlayer->_BtlTeamID = BattleTestPlayerIdx%EBTLTMS_MAX;
	//}
	SetBattleTeamScore( pBattlePlayer->_BtlTeamID, BattleTestPlayerIdx+1 );
	SetBattleTeamScore( pBattlePlayer->_BtlTeamID, BattleTestPlayerIdx+2 );
	//
	//if( BattleTestPlayerIdx >= 26 ){
	//	NotifyBattleEnd();
	//}
}
//end test

//=========================================================================================
INT MMOBattleManager::GetTimeSinceBattleBegin()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	//Local battlefield start time
	__time32_t localBattleStartServerTime = _BattleStartUtcTime;
	_localtime32(&localBattleStartServerTime);

	//Current local time
	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);
	UINT32 curTimeSec = mmoFacade.ConvertSystemTimeToTmTSecond( curTime );

	//Time difference between local and server
	INT32 secDiff = mmoFacade.GetClientToSceneSecondDiff();

	warnf( NAME_Log, TEXT("GetTimeSinceBattleBegin  curTime:H-%d,M-%d,S-%d"), curTime.wHour, curTime.wMinute, curTime.wSecond );
	warnf( NAME_Log, TEXT("GetTimeSinceBattleBegin  curTimeSec:%d, localBattleStartServerTime:%d, secDiff:%d"), curTimeSec, localBattleStartServerTime, secDiff );
	//
	INT32 secSinceStart = curTimeSec - ( localBattleStartServerTime + secDiff );
	_IsBattleBegin = ( secSinceStart >= 0 );
	return secSinceStart;
}