#include "stdafx.h"
#include "message_handler.h"
#include "MMOBattleManager.h"
#include "MMOMapDataManager.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "MMOAchievementManager.h"
#include "MMOGameClasses.h"
#include "ClientSceneVariablesParse.h"
#include "FSMIO.h"
//#include "MMOScreenMsgMgr.h"
#include "MMONavigationManager.h"
#include "MMOWeatherManager.h"
#include "ActivityManager.h"
#include "MMOGuideManager.h"
#include "AbilityProcessor.h"
#include "MMOGuildManager.h"
#include "DebugMMO.h"
#include "LoadMapTipsManager.h"
#include "MMOSkillManager.h"
#include "MMOClientVariableManager.h"
#include "Mir2Attack.h"
#include "StreamingPauseRendering.h"
#include "MMOBuffManager.h"
#include "ActivityManager.h"
#include "RankListManager.h"

using namespace ExternalMessage;

#define M_DEBUG_SCENE 0

void UpdateTransducer( int Progress );

// the frame count when game starting
void CMessageHandler::Parse_g2c_sync_frame(const char* buf, UINT32 size)
{
    G2C_SYNC_FRAME* msg = (G2C_SYNC_FRAME*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	warnf(NAME_Warning, TEXT("[CMessageHandler::Parse_g2c_sync_frame] FrameNum[%d]"), msg->nFrame);

	_lockPlayer = false;
    MMOFacade::getSingleton().SetStartServerFrameCount(msg->nFrame);
}

void CMessageHandler::Parse_g2c_enter_scene(const char* buf, UINT32 size)
{
	G2C_ENTER_SCENE* msg = (G2C_ENTER_SCENE*)buf;
	if (!buf || size < sizeof(*msg))
		return;

    FVector pos = FVector(
        msg->wX,
        msg->wY,
        msg->wZ);
    ConvertCoordN2C(pos);

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(NAME_Log, 
		TEXT("[CMessageHandler::Parse_g2c_enter_scene] SceneTempalte[%d] SceneID[%d] PosN(%d, %d, %d) PosC(%.0f, %.0f, %.0f)"),
		msg->nSceneTempID, msg->nSceneID, msg->wX, msg->wY, msg->wZ, pos.X, pos.Y, pos.Z);
	CLEAR_WARN_COLOR();

	//GMalloc->DumpAllocations(*GLog); // oscar@snda

	_lockPlayer = false;

	//MMOFacade::getSingleton().SetCurrentMapID(msg->nSceneID);
	MMOFacade::getSingleton().SetGameStartTime();
	MMOFacade::getSingleton().GetMainController()->bNeedCommitPosition = TRUE;
	
	// to make initial levels loaded first -ming
	//MMOFacade::getSingleton().GetMainController()->BlockNextLevelLoading = TRUE;
	
	/*AMMOGame* game = Cast<AMMOGame>(GWorld->GetWorldInfo()->Game);
	if (game)
	{
		game->SceneType = EST_Game;
	}*/

	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if(!actor)
	{
		return;
	}

	pos.Z += actor->GetCollisionHeight();

	MMOFacade::getSingleton().SetPlayerInitPosition(pos);
	//MMOFacade::getSingleton().DelayPositionSetting = TRUE;

	//actor->SetLocation(pos);	// delay the position setting to prevent waired image during level loading
	actor->EnterGame();

	AMMOGame* game = Cast<AMMOGame>( GWorld->GetWorldInfo()->Game );
	if( game )
	{
		if( !game->JumpInSameScene )
			MMO::ActiveGameState(EGameState_InGame);

		game->StartLongJump( pos, FALSE, TRUE );
	}

	if(MMOFacade::getSingleton().GetMainController())
	{
		//MMOFacade::getSingleton().GetMainControllerAI()->EndCmdAbility();
		//MMOTargetManager::getSingleton().ClearDesireTarget();
		FSMIO::getSingleton().Target.ClearDesireTarget(NULL);
		MMOFacade::getSingleton().GetMainController()->StopMove();
		FSMIO::getSingleton().CDTime.ResetCD_WalkAndRunDelay(0.3f,0.3f);
		FSMIO::getSingleton().CDTime.ResetCD_MoveDelay_Frame(2);
		Mir2Attack::getSingleton().OnEnterGame(TRUE);
		MMOFacade::getSingleton().GetMainController()->OnEnterScene();
	}

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	check(player);
	player->PlayTeleportEnterEffect(actor->GetFootLocation(), actor->Rotation);

	//更新当前的门
	MMOFacade::getSingleton().UpdateDoors( );
	//MMOFacade::getSingleton().SetSceneDifficulty(msg->nSceneDifficulty);
	//MMOFacade::getSingleton().SetSceneLevel(msg->nSceneLevel);

	// MOVED UP -ming
	//MMO::ActiveGameState(EGameState_InGame);

	// 成就
	MMOGameEvent_Achievement evtAch(adEnterScene, msg->nSceneTempID, 0, 0);
	MMOUI::PostGameEvent(&evtAch);

	// notify MMONavigationManager for new map
	MMONavigationManager::getSingleton().EnterScene();

	// 地图
	MMOMapDataManager::getSingleton().SetCurSceneLineIn( msg->nSceneID );

	// 天气系统
	MMOWeatherManager::getSingleton().EnterScene(msg->byWeather);

	// 公会
	MMOGuildManager::getSingleton().EnterScene(msg->nSceneTempID, msg->nSceneID);

	// 新手引导
	MMOGameEvent_Guide evtGuide(EGUET_EnterScene);
	MMOUI::PostGameEvent(&evtGuide);

	MMOGameEvent_Any evt(GameEvent_EnterScene);
	MMOUI::PostGameEvent(&evt);

	g_updateMainPlayerHudName = true;
}

void CMessageHandler::Parse_g2c_leave_scene(const char* buf, UINT32 size)
{
	//MMOFacade::getSingleton().GetMainController()->GotoState(FName(TEXT("Loading")));

	G2C_LEAVE_SCENE* msg = (G2C_LEAVE_SCENE*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	//	warnf(NAME_Warning, TEXT("[CMessageHandler::Parse_g2c_leave_scene] FrameNum[%d]"), msg->nFrame);
	_lockPlayer = true;

	//GMalloc->DumpAllocations(*GLog); // oscar@snda


	AMMOPlayerController* MainController = MMOFacade::getSingleton().GetMainController();
	if (MainController)
	{
		MainController->OnLeaveScene();
	}

	//m_NPCDataCache.Clear();
	MMOFacade::getSingleton().RemoveOjbectToSync();

	// moved to Parse_g2c_changescene_enter -ming
	//MMO::ActiveGameState(EGameState_Loading);
	/*AMMOGame* game = Cast<AMMOGame>(GWorld->GetWorldInfo()->Game);
	if (game)
	{
	game->SceneType = EST_Loading;
	}*/

	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if (actor)
	{
		actor->LeaveGame();
	}

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player && actor)
	{
		player->PlayTeleportLeaveEffect(actor->GetFootLocation(), actor->Rotation);
	}
	//
	//UMMOGFXPlayer_FBEndAll *pFBEndAllPlyer;
	//pFBEndAllPlyer = (UMMOGFXPlayer_FBEndAll*)MMOUI::FindMoviePlayer( TEXT("FBEndAll") );
	//if( pFBEndAllPlyer != NULL ){
	//	pFBEndAllPlyer->CloseAllFBEndUI();
	//}
	FExportMap &mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	INT mapType = mapInfo.ServerMapType ;
	if( mapType == E_est_Dungeon )
	{
		UMMOUIFBBattleComfirm *pFBBattleComfirm = (UMMOUIFBBattleComfirm*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm );
		if( pFBBattleComfirm != NULL ){
			pFBBattleComfirm->CloseAllFBEndUI();
		}
	}

	MMOGameEvent_Any evt(GameEvent_LeaveServerScene);
	MMOUI::PostGameEvent(&evt);

	//// clear ability andd effect processor first
	//AbilityProcessor* ap = MMOFacade::getSingleton().GetAbilityProcessor();
	//if (ap)
	//{
	//	ap->ClearAllAbilityNode();
	//}

	//EffectProcessor::GetSingleton().DeleteAllComponent();

	MMOFacade::getSingleton().ClearAllCharacter();
}


// void CMessageHandler::Parse_g2c_changescene_leave(const char* buf, UINT32 size)
// {
// 	debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_changescene_leave]"));
// 
// 	//閿佸畾瀹㈡埛绔// 	_lockPlayer = true;
// 	//MMOFacade::getSingleton().GetMainController()->GotoState(FName(TEXT("Loading")));
// }

G2C_CHANGESCENE_ENTER ChangeSceneMsg;
void CMessageHandler::Do_g2c_changescene_enter()
{
	G2C_CHANGESCENE_ENTER* msg = &ChangeSceneMsg;

	warnf(NAME_Warning, 
		TEXT("[CMessageHandler::Parse_g2c_changescene_enter] GS[%d] SceneTempalte[%d] SceneID[%d] Pos(%d, %d, %d) Dir(%d)"),
		msg->dwGameServerID, msg->nSceneTempID, msg->nSceneID,
		msg->wDestX, msg->wDestY, msg->wDestZ, msg->wFaceDirection);

	MMONavigationManager::getSingleton().ClearPathEffect();
	if(msg->byIsInJump)
	{
		//MMOFacade::getSingleton().GetMMOBuffManager()->ClearAllBuff();

		AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if (pMainPlayer)
		{
			if (pMainPlayer->ActorAttributes->IsGreyName)
			{
				pMainPlayer->ActorAttributes->IsGreyName = FALSE;
				MMOUI::GetUIManager()->delegateUpdateHintText(pMainPlayer);
			}
			MMOFacade::getSingleton().NotifyLeaveCompact(pMainPlayer->ServerID);
		}
	}

	MMOFacade::getSingleton().GetMainController()->OnEnterScene();

	_lockPlayer = false;

	FVector pos(msg->wDestX, msg->wDestY, msg->wDestZ);
	ConvertCoordN2C(pos);

	// don't teleport if the map ID is the same, ie. to the same map
	AMMOPlayerController* MainController = MMOFacade::getSingleton().GetMainController();
	if( MainController && MainController->CurMapID == msg->nSceneTempID )
	{
		AMMOGame* GameInfo = Cast<AMMOGame>(MainController->WorldInfo->Game);
		if( GameInfo )
			GameInfo->StartLongJump( pos, TRUE, TRUE );

		debugf( TEXT("Long jump in the same scene") );
		return;
	}

	FScriptDelegate FunNone(EC_EventParm);
	MMOUI::GetUIManager()->SetCallBack_PreLoadDelegate(FunNone);
	MMO::ActiveGameState(EGameState_Loading);

	//MMOFacade::getSingleton().SetCurrentMapID(msg->nSceneID);

	//Test 鍔ㄦ�佸壇鏈
	if (GWorld )
	{
		TArray<byte> tSubLevelVisbleGroupIDList;
		//鍙屾柟绾﹀畾锛灞備负榛樿鍙
		tSubLevelVisbleGroupIDList.Push(0);
		tSubLevelVisbleGroupIDList.Push(1);
		GWorld->SubLevelVisbleGroupIDList = tSubLevelVisbleGroupIDList ;
	}
	LoadMapTipsManager::getSingleton().SetNextMapID( msg->nSceneTempID);

	//MMO::ActiveGameState(EGameState_Loading);

	FString stSceneName;
	if( UGlobalMapInfo::GetSingleton()->FindMapName(msg->nSceneTempID, stSceneName) )
	{
		FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( msg->nSceneTempID );
		if( mapInfo.ServerMapType == E_est_BattleField ){
			MMOBattleManager::getSingleton().SetEnteredBattleID( msg->nSceneTempID );
		}
		//MMOFacade::getSingleton().SetCurrentMapName(stSceneName);
		MMOFacade::getSingleton().Teleport(msg->nSceneTempID, pos, ConvertToClientDegree(msg->wFaceDirection));
	}

}
void CMessageHandler::Parse_g2c_changescene_enter(const char* buf, UINT32 size)
{
	//GMalloc->DumpAllocations(*GLog); // oscar@snda

	G2C_CHANGESCENE_ENTER* msg = (G2C_CHANGESCENE_ENTER*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	memcpy(&ChangeSceneMsg,msg,sizeof(G2C_CHANGESCENE_ENTER));
	AMMOPlayerController* MainController = MMOFacade::getSingleton().GetMainController();
	if( MainController && MainController->CurMapID == msg->nSceneTempID )
	{
		Do_g2c_changescene_enter();
		return;
	}

	
	MMOUI::GetUIManager()->SetCallBack_PreLoadDelegate(FScriptDelegate(MMOFacade::getSingleton().GetMainController(), TEXT("CallBack_PreLoad_ChangeScene")));
	MMO::PreActiveGameState(EGameState_Loading);


	//warnf(NAME_Warning, 
	//	TEXT("[CMessageHandler::Parse_g2c_changescene_enter] GS[%d] SceneTempalte[%d] SceneID[%d] Lv[%d] Diff[%d] Pos(%d, %d, %d) Dir(%d)"),
	//	 msg->dwGameServerID, msg->nSceneTempID, msg->nSceneID, msg->nSceneLevel, msg->nSceneDifficulty,
	//	 msg->wDestX, msg->wDestY, msg->wDestZ, msg->wFaceDirection);

	//MMOFacade::getSingleton().SetSceneDifficulty(msg->nSceneDifficulty);
	//MMOFacade::getSingleton().SetSceneLevel(msg->nSceneLevel);

	//warnf(NAME_Warning, 
	//	TEXT("[CMessageHandler::Parse_g2c_changescene_enter] GS[%d] SceneTempalte[%d] SceneID[%d] Pos(%d, %d, %d) Dir(%d)"),
	//	msg->dwGameServerID, msg->nSceneTempID, msg->nSceneID,
	//	msg->wDestX, msg->wDestY, msg->wDestZ, msg->wFaceDirection);

	//MMONavigationManager::getSingleton().ClearPathEffect();


	//MMOFacade::getSingleton().GetMainController()->OnEnterScene();

	//_lockPlayer = false;

	//FVector pos(msg->wDestX, msg->wDestY, msg->wDestZ);
	//ConvertCoordN2C(pos);

	//// don't teleport if the map ID is the same, ie. to the same map
	//AMMOPlayerController* MainController = MMOFacade::getSingleton().GetMainController();
	//if( MainController && MainController->CurMapID == msg->nSceneTempID )
	//{
	//	AMMOGame* GameInfo = Cast<AMMOGame>(MainController->WorldInfo->Game);
	//	if( GameInfo )
	//		GameInfo->StartLongJump( pos, TRUE, TRUE );

	//	debugf( TEXT("Long jump in the same scene") );
	//	return;
	//}

	//MMO::ActiveGameState(EGameState_Loading);

	////MMOFacade::getSingleton().SetCurrentMapID(msg->nSceneID);

	////Test 鍔ㄦ�佸壇鏈
	//if (GWorld )
	//{
	//	TArray<byte> tSubLevelVisbleGroupIDList;
	//	//鍙屾柟绾﹀畾锛灞備负榛樿鍙
	//	tSubLevelVisbleGroupIDList.Push(0);
	//	tSubLevelVisbleGroupIDList.Push(1);
	//	GWorld->SubLevelVisbleGroupIDList = tSubLevelVisbleGroupIDList ;
	//}
	//LoadMapTipsManager::getSingleton().SetNextMapID( msg->nSceneTempID);

	////MMO::ActiveGameState(EGameState_Loading);

	//FString stSceneName;
	//if( UGlobalMapInfo::GetSingleton()->FindMapName(msg->nSceneTempID, stSceneName) )
	//{
	//	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( msg->nSceneTempID );
	//	if( mapInfo.ServerMapType == E_est_BattleField ){
	//		MMOBattleManager::getSingleton().SetEnteredBattleID( msg->nSceneTempID );
	//	}
	//	//MMOFacade::getSingleton().SetCurrentMapName(stSceneName);
	//	MMOFacade::getSingleton().Teleport(msg->nSceneTempID, pos, ConvertToClientDegree(msg->wFaceDirection));
	//}

	////MMO::ActiveGameState(EGameState_Loading);
}


void CMessageHandler::Parse_g2c_sync_scene_variables(const char* buf, UINT32 size)
{
	G2C_SYNC_SCENE_VARIABLES* msg = (G2C_SYNC_SCENE_VARIABLES*)buf;
	if (!buf || size < sizeof(*msg))
		return;
    
	if (msg->wSize < sizeof(*msg) || msg->wSize > size ||
		(msg->wSize - sizeof(*msg)) % sizeof(msg->VarList[0]) != 0)
		return;
	INT count = (msg->wSize - sizeof(G2C_SYNC_SCENE_VARIABLES)) / sizeof(G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA);
	warnf(NAME_Warning, TEXT("[CMessageHandler::Parse_g2c_sync_scene_variables] Count:%d"), count);

	for (INT i = 0; i < count; i++)
    {
		warnf(NAME_Warning, TEXT("\t\t\tSceneVar Idx[%d] ReferID[%d] Val[%d]"), 
			msg->VarList[i].wVarIndex, 0, msg->VarList[i].nValue);

		MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Scene>(msg->VarList[i].wVarIndex, 0, msg->VarList[i].nValue, false);
		if (msg->VarList[i].wVarIndex == 1)
		{
			int Progress = msg->VarList[i].nValue;
			UpdateTransducer(Progress);
		}

		// 更新公会地图场景变量
		INT mapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
		MMOGuildManager::getSingleton().NotifyUpdateSceneVariable(mapID, msg->VarList[i].wVarIndex, msg->VarList[i].nValue);
	}
	
	if(count > 0)
	{
		MMO::SetServerSceneVars(&msg->VarList[0], count);
	}
}

void CMessageHandler::Parse_g2c_sync_role_variables(const char* buf, UINT32 size)
{
	G2C_SYNC_ROLE_VARIABLES* msg = (G2C_SYNC_ROLE_VARIABLES*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	
	if (msg->wSize < sizeof(*msg) || msg->wSize > size ||
		(msg->wSize - sizeof(*msg)) % sizeof(msg->VarList[0]) != 0)
		return;
	INT listSize = (msg->wSize - sizeof(G2C_SYNC_ROLE_VARIABLES)) / sizeof(G2C_SYNC_ROLE_VARIABLES::VARIABLE_DATA);
	for (INT i = 0; i < listSize; ++ i)
	{
		//MMOGameEvent_Achievement evtAch1(adKillNpcIDCount, msg->VarList[i].wVarIndex, msg->VarList[i].nValue, 0);
		//MMOUI::PostGameEvent(&evtAch1);
		//MMOGameEvent_Achievement evtAch3(adKillNpcGroupIDCount, msg->VarList[i].wVarIndex, msg->VarList[i].nValue, 0);
		//MMOUI::PostGameEvent(&evtAch3);
		MMOGameEvent_Achievement evtAch2(adKilledByNpcIDCount, msg->VarList[i].wVarIndex, msg->VarList[i].nValue, 0);
		MMOUI::PostGameEvent(&evtAch2);

		//
		MMOGameEvent_VariableChange varChgEvt(vtRole, 0, msg->VarList[i].wVarIndex, msg->VarList[i].nValue, GameEvent_VariableInit);
		MMOUI::PostGameEvent(&varChgEvt);
	}
}

void CMessageHandler::Parse_g2c_sync_client_variables(const char* buf, UINT32 size)
{
    G2C_SYNC_CLIENT_VARIABLES* msg = (G2C_SYNC_CLIENT_VARIABLES*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	MMOClientVariableManager::getSingleton().SyncClientVariables(msg->nVariableList);
}


void CMessageHandler::Parse_g2c_update_variable(const char* buf, UINT32 size)
{
	G2C_UPDATE_VARIABLE* msg = (G2C_UPDATE_VARIABLE*)buf;
	if (!buf || size < sizeof(*msg))
		return;

    switch (msg->byType)
    {
    case vtRole:
		{
			//SET_WARN_COLOR(COLOR_CYAN);
			//warnf(TEXT("[CMessageHandler::Parse_g2c_update_variable] Role Idx[%d] ReferID[%d] Val[%d]"), 
			//	msg->wIndex, msg->nReferID, msg->nValue);
			//CLEAR_WARN_COLOR();

			// 成就(需求:11627)
			//MMOGameEvent_Achievement evtAch1(adKillNpcIDCount, msg->wIndex, msg->nValue, 0);
			//MMOUI::PostGameEvent(&evtAch1);
			//MMOGameEvent_Achievement evtAch3(adKillNpcGroupIDCount, msg->wIndex, msg->nValue, 0);
			//MMOUI::PostGameEvent(&evtAch3);
			MMOGameEvent_Achievement evtAch2(adKilledByNpcIDCount, msg->wIndex, msg->nValue, 0);
			MMOUI::PostGameEvent(&evtAch2);

			if (msg->wIndex == 194)
			{
				RankListManager::getSingleton().SetMyHeroNum(msg->nValue);
			}
		}
        break;

    case vtScene:
		{
			//SET_WARN_COLOR(COLOR_CYAN);
			//warnf(TEXT("[CMessageHandler::Parse_g2c_update_variable] Scene Idx[%d] ReferID[%d] Val[%d]"), 
			//	msg->wIndex, msg->nReferID, msg->nValue);
			//CLEAR_WARN_COLOR();

			INT mapID = msg->nReferID;
			G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA var;
			var.wVarIndex = msg->wIndex;
			var.nValue = msg->nValue;
			CheckClientSceneVariableAction( mapID, var );

			// 更新公会场景变量
			MMOGuildManager::getSingleton().NotifyUpdateSceneVariable(mapID, var.wVarIndex, var.nValue);

			//
			MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Scene>(msg->wIndex, msg->nReferID, msg->nValue, false);

			//// alpha版本的原临时代码迁移至此处，请客户端人员组织好所有游戏变量，并检查当前地图
			//if (msg->wIndex == 1)
			//{
			//	int Progress = msg->nValue;
			//	UpdateTransducer( Progress );
			//}
		}
        break;

    case vtGuild:
		MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Guild>(msg->wIndex, msg->nReferID, msg->nValue, false);
        break;

    case vtQuest:
		MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_Quest>(msg->wIndex, msg->nReferID, msg->nValue);
        break;

    case vtKillNpcCount:			
		MMO::TActiveKismetEvent_ServerVarChange<USeqEvent_Server2Client_VarChange_KillNPCCount>(msg->wIndex, msg->nReferID, msg->nValue);
        break;

    default:
        break;
    }
	
	MMO::ActiveKismetEvent_ServerVarChange(msg->byType, msg->wIndex, msg->nReferID, msg->nValue);

	MMOGameEvent_VariableChange varChgEvt(msg->byType, msg->nReferID, msg->wIndex, msg->nValue, GameEvent_VariableChanged);
	MMOUI::PostGameEvent(&varChgEvt);
}
//==================================================================================================================
void CMessageHandler::Send_QuerySceneLine()
{
	C2M_QUERY_LINE_REQ msg;
	msg.wMsg = c2m_query_line_req;
	_Send(&msg, sizeof(msg));

	////M2C_QUERY_LINE_ACK* msg = new M2C_QUERY_LINE_ACK;
	//INT lineCnt = ( appRand() % 20 ) + 1;
	//M2C_QUERY_LINE_ACK* msg = (M2C_QUERY_LINE_ACK*)appMalloc( sizeof(M2C_QUERY_LINE_ACK) + lineCnt * sizeof(LINE_SCENE_DATA) );
	//msg->wMsg = m2c_query_line_ack;
	//msg->wSize = sizeof(M2C_QUERY_LINE_ACK) + msg->wCount * sizeof(LINE_SCENE_DATA);

	//FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	//msg->dwSceneTempID = curMapInfo.ServerID;
	//msg->wCount = lineCnt;
	////LINE_SCENE_DATA *pArray = new((char*)msg + sizeof(M2C_QUERY_LINE_ACK)) LINE_SCENE_DATA[msg->wCount];
	//for (UINT32 i = 0; i < msg->wCount; ++i){
	//	msg->LineSceneData[i].byState = E_lss_Normal;
	//	msg->LineSceneData[i].dwSceneID = ( i | ( curMapInfo.ServerID << 16 ) );
	//}
	//Parse_m2c_query_line_ack( (char*)msg, msg->wSize );

	////delete [] pArray;
	////delete msg;
	//appFree( (char*)msg );
}

//--------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Send_ChangeToSceneLine( UINT8 lineID )
{
	C2G_CHANGE_LINE msg;
	msg.wMsg = c2g_change_line;
	msg.byLineIndex = lineID;
	_Send(&msg, sizeof(msg));
}

//--------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_m2c_query_line_ack(const char* buf, UINT32 size)
{
	M2C_QUERY_LINE_ACK* msg = ( M2C_QUERY_LINE_ACK*)buf;
	if (!buf || size < sizeof(*msg) ||
		msg->wCount > (size - sizeof(*msg)) / sizeof(msg->LineSceneData[0]))
		return;

	MMOMapDataManager &mapDataMgr = MMOMapDataManager::getSingleton();
	mapDataMgr.ClearCurMapLines();

	for (UINT32 i = 0; i < msg->wCount; ++i){
		ELINE_SCENE_STATE lineState = (ELINE_SCENE_STATE)msg->LineSceneData[i].byState;
		if( lineState == E_lss_Invalid ){
			//正在销毁的地图线
			continue;
		}
		mapDataMgr.AddNewCurMapLine( msg->LineSceneData[i].dwSceneID, lineState );
	}

	MMOGameEvent_Any evt( GameEvent_UpdateSceneLines );
	MMOUI::PostGameEvent(&evt);
}
//==================================================================================================================
void CMessageHandler::Parse_g2c_sync_flag_pos(const char* buf, UINT32 size)
{
	G2C_SYNC_FLAG_POS* msg = ( G2C_SYNC_FLAG_POS*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();
	MMOBattleManager *pBattleMgr = &MMOBattleManager::getSingleton();
	const FBattlePlayer *pBattlePlayer = pBattleMgr->GetPlayer( msg->dwRoleID );

	//flag type
	INT flagTypeID = EBTLTMS_Team1;
	if( pBattlePlayer != NULL ){
		flagTypeID = pBattlePlayer->_BtlTeamID;
	}
	else{
		flagTypeID = ( ( msg->byFaction == EBTLTMS_Team1 ) ? EBTLTMS_Team2 : EBTLTMS_Team1 );
	}

	INT mapID = pMapDataMgr->GetCurMap().ServerID;

	//
	FActorInMapUI *pFlagUIIcon = pMapDataMgr->FindMapDymcActorGlobal( flagTypeID );
	//if flag not exit now, create it
	if( pFlagUIIcon == NULL ){
		AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if( pMainPlayer == NULL ){
			return;
		}

		//flag name
		FString name;
		const FBattlePlayer *pMyBattleData = pBattleMgr->GetPlayer( pMainPlayer->ServerID );
		if( pMyBattleData != NULL ){
			bool isMyTeamFlag = ( flagTypeID == pMyBattleData->_BtlTeamID );
			if( isMyTeamFlag ){
				name = MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm7");
			}
			else{
				name = MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm8");
			}
		}
		else{
			if( flagTypeID == EBTLTMS_Team1 ){
				name = MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm9");
			}
			else{
				name = MMOUI::ParseLocalizedPropertyPath( "MMOGame.BattleField.BattleNorm10");
			}
		}

		//add it
		pFlagUIIcon = pMapDataMgr->AddOrUpdateMapDymcActorGlobal( flagTypeID, EACTMUI_BattleFlag, mapID, name );
	}

	FVector serverPos( msg->wCurrentX, msg->wCurrentY, msg->wCurrentZ);
	ConvertCoordN2C( serverPos );
	pFlagUIIcon->_OrigInMapPos.X = serverPos.X;
	pFlagUIIcon->_OrigInMapPos.Y = serverPos.Y;
	pFlagUIIcon->_OrigInMapPos.Z = serverPos.Z;
	
	pFlagUIIcon->_AidID = msg->dwRoleID;
	//pFlagUIIcon->_NeedUpdateFlag = EACTUIUF_UpdateAll;
}

//
void CMessageHandler::Parse_g2c_sync_teleport_platform(const char* buf, UINT32 size)
{
	 G2C_SYNC_TELEPORT_PLATFORM* msg = ( G2C_SYNC_TELEPORT_PLATFORM*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	INT cntPer = sizeof( msg->nTeleportPlatform[0] );
	INT cnt = sizeof( msg->nTeleportPlatform ) / cntPer;
	cntPer *= 8;
	//FTeleportNodeObj *pNode;
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();
	for( INT i = 0; i < cnt; ++i ){
		for( INT j = 0; j < cntPer; ++j ){
			//pNode = UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( i * cnt + j );
			//if( pNode == NULL ){
			//	continue;
			//}

			//pNode->IsOpened |= ( ( msg->nTeleportPlatform[i] & ( 1 << j ) ) > 0 ? TRUE : FALSE );
			INT nodeID = i * cnt + j;
			INT isOpenedInServer = ( ( msg->nTeleportPlatform[i] & ( 1 << j ) ) > 0 ? TRUE : FALSE );
			pMapDataManager->NotifyTeleportsNetNodesOpened( nodeID, isOpenedInServer );
		}
	}

	//set data is initiliaze by server
	pMapDataManager->SetGettedServerTeleportData( TRUE );
}

//
void CMessageHandler::Parse_g2c_active_platform(const char* buf, UINT32 size)
{
	G2C_ACTIVE_PLATFORM* msg = (G2C_ACTIVE_PLATFORM*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();
	pMapDataManager->NotifyTeleportsNetNodesOpened( msg->byActivePlatformID, TRUE );
	pMapDataManager->FirstOpeneTeleportsNetNodes( msg->byActivePlatformID );
	//show new teleport net
	UMMOUITeleportNet *pTeleportStation = (UMMOUITeleportNet*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TeleportNet );
	FTeleportNodeObj *pNetNode =  pMapDataManager->GetLastOpendNode();
	if( pTeleportStation != NULL && !pTeleportStation->IsVisible && pNetNode != NULL){
		pTeleportStation->Open( pNetNode->Region, pNetNode->ServerID );
	}
}

//========================================================================
void CMessageHandler::Parse_g2c_activate_revive_pt(const char* buf, UINT32 size)
{
	G2C_ACTIVATE_REVIVE_PT* msg = (G2C_ACTIVATE_REVIVE_PT*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	//
	FVector pos( msg->nX, msg->nY, msg->nZ );
	ConvertCoordN2C(pos);

	MMOGameEvent_ActiveRevivePnt evt( pos );
	MMOUI::PostGameEvent(&evt);
}

//----------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_select_scene_difficulty(const char* buf, UINT32 size)
{
	G2C_SELECT_SCENE_DIFFICULTY* msg = (G2C_SELECT_SCENE_DIFFICULTY*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(NAME_Log, TEXT("[CMessageHandler::Parse_g2c_select_scene_difficulty] SelectedDifficulty[%d] "),  msg->bySelectedDifficulty);
	CLEAR_WARN_COLOR();

	MMOMapDataManager::getSingleton().SetCurFBDiff( msg->bySelectedDifficulty );

	//
	MMOGameEvent_OneInt evt( GameEvent_ChangeFB_Difficulty, msg->bySelectedDifficulty );
	MMOUI::PostGameEvent(&evt);
}
//----------------------------------------------------------------------------
//change the scene difficulty
void CMessageHandler::Parse_g2c_query_scene_difficulty(const char* buf, UINT32 size)
{
	G2C_QUERY_SCENE_DIFFICULTY* msg = (G2C_QUERY_SCENE_DIFFICULTY*)buf;
	if (!buf || size < sizeof(*msg))
		return;

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(NAME_Log, TEXT("[CMessageHandler::Parse_g2c_query_scene_difficulty] SceneTemplate[%d] SelectedDegree[%d] "),  msg->dwSceneTemplateID, msg->byDungeonDifficultyMask);
	CLEAR_WARN_COLOR();

	MMOMapDataManager::getSingleton().SetCurFBMaxDiff( msg->byDungeonDifficultyMask );
	MMOMapDataManager::getSingleton().SetCurFBEnterFrequence( msg->byDungeonEnterCount );
	
	UMMOUIFBEnter *pFBEnter = (UMMOUIFBEnter*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FBEnter_FBEnter );
	if( pFBEnter != NULL ){
		//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_FBEnter_FBEnter,TRUE,"");
		UBOOL bCanSelectFB = TeamManager::getSingleton().HasTeam() ? TeamManager::getSingleton().IsTeamLeader() : true;
		if( !pFBEnter->IsVisible && bCanSelectFB ){
			pFBEnter->Show( TRUE );
		}
	}

}

//reach the goal of this FB
void CMessageHandler::Parse_g2c_scene_finish(const char* buf, UINT32 size)
{
	G2C_SCENE_FINISH* msg = (G2C_SCENE_FINISH*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	
	//initialize the last data
	//MMOFacade::getSingleton().ResetFBSummary();
	FExportMap &mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	INT mapType = mapInfo.ServerMapType ;
	if( mapType == E_est_Dungeon )
	{
		MMOFacade::getSingleton().ResetFBAward();
		//Begin to show fb summary and so on
		//UMMOGFXPlayer_FBEndAll *pFBEndAllPlyer;
		//pFBEndAllPlyer = (UMMOGFXPlayer_FBEndAll*)MMOUI::GetUIManager()->LoadMoviePlayers( MMOUI::GetUIManager()->MoviePlayer_FBEndAll);
		//if( pFBEndAllPlyer != NULL ){
		//	pFBEndAllPlyer->ShowCountDownBar();
		//}

		UMMOUIFBBattleComfirm *pFBBattleComfirm = (UMMOUIFBBattleComfirm*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm );
		if( pFBBattleComfirm != NULL ){
			pFBBattleComfirm->ShowCountDownBar();
		}

		////
		//UMMOUIFBBattleComfirm *pFBBattleConfirm = (UMMOUIFBAward*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FBEndAll_FBBattleComfirm );
		//if( pFBBattleConfirm != NULL ){
		//	pFBBattleConfirm->ShowCountDownBar();
		//}

		//
		//MMOScreenMsgMgr::getSingleton().PushMsg( EScnMsgType_FBUnlock, mapInfo.ServerID );
	}

	//
	MMOGameEvent_OneInt evt( GameEvent_SceneFinished, mapInfo.ServerID );
	MMOUI::PostGameEvent(&evt);
}

//get score of this FB
void CMessageHandler::Parse_g2c_dungeon_finish_data(const char* buf, UINT32 size)
{
	G2C_DUNGEON_FINISH_DATA* msg = (G2C_DUNGEON_FINISH_DATA*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	MMOFacade::getSingleton().SetFBSummary( *msg );
	//
	MMOMapDataManager::getSingleton().ParseDungeonFinishDatasForAchievement( msg );
	////
	//Send_GetFightSummary();
}

//get awarded in this FB
void CMessageHandler::Parse_g2c_dungeon_reward(const char* buf, UINT32 size)
{
	G2C_DUNGEON_REWARD* msg = (G2C_DUNGEON_REWARD*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	MMOFacade::getSingleton().SetFBAward( *msg );
	//
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	//
	UMMOUIFBAward *pFBAward = (UMMOUIFBAward*)MMOUI::FindUIPanel( TEXT("FBAward") );
	//
	if( pFBAward != NULL ){
		//
		if( msg->dwRoleID == pl->ServerID ){
			//my awards
			pFBAward->UpdateMyAward();
		}
		else{
			//others award
			pFBAward->UpdateMemberAward();
		}
	}
}

//
void CMessageHandler::Parse_g2c_sync_dungeon_goal(const char* buf, UINT32 size)
{
	G2C_SYNC_DUNGEON_GOAL* msg = (G2C_SYNC_DUNGEON_GOAL*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	const FExportMap& curScene = MMOMapDataManager::getSingleton().GetCurMap();
	//
	G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA bossKillVar;
	bossKillVar.wVarIndex = MAX_Dungeon_Target_ID_Start + EDgenCnstTgtTyp_TargetKilled;
	bossKillVar.nValue = (UINT32)msg->nBossKillCount;
	CheckClientSceneVariableAction( curScene.ServerID, bossKillVar );
	//
	G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA eliteNpcKill;
	eliteNpcKill.wVarIndex = MAX_Dungeon_Target_ID_Start + EDgenCnstTgtTyp_SpecialKilled;
	eliteNpcKill.nValue = (UINT32)msg->nEliteNpcKillCount;
	CheckClientSceneVariableAction( curScene.ServerID, eliteNpcKill );
}

//Score getted in FB
void CMessageHandler::Parse_g2c_dungeon_finish_score(const char* buf, UINT32 size)
{
	G2C_DUNGEON_FINISH_SCORE* msg = (G2C_DUNGEON_FINISH_SCORE*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	if (msg->byCount > (size - sizeof(*msg)) / sizeof(msg->stScore[0]))
		return;
	MMOFacade::getSingleton().SetFBOtherScore( *msg );
}
//===========================================Send=====================================
//void CMessageHandler::Send_Require_TeleportNetNodesData()
//{
//#if M_DEBUG_SCENE
//	G2C_QUERY_TELEPORT_PLATFORM_ACK *msg = (G2C_QUERY_TELEPORT_PLATFORM_ACK*)ms_MsgBuffer;
//	msg->wMsg = g2c_query_teleport_platform_ack;
//	for( INT i = 0; i < 16; ++i ){
//		msg->nTeleportPlatform[0] = 0xff;
//	}
//	Parse_g2c_query_teleport_platform_ack((const char*)msg, sizeof(G2C_QUERY_TELEPORT_PLATFORM_ACK));
//	return;
//#endif
//	C2G_QUERY_TELEPORT_PLATFORM_REQ msg;
//	msg.wMsg = c2g_query_teleport_platform_req;
//	_Send(&msg, sizeof(msg));
//}

//
void CMessageHandler::Send_TeleportToNetNodes( UINT8 srcNodeID, UINT8 destNodeID )
{
//#if M_DEBUG_SCENE
//	FTeleportNodeObj *pScrNode = UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( srcNodeID );
//	FTeleportNodeObj *pDestNode = UGlobalMapInfo::GetSingleton()->FindTeleportNetNodes( destNodeID );
//
//	MMOUI::GetUIManager()->delegateAddScreenMessage( FString::Printf( TEXT("TeleportNet from %d:%s to %d:%s") , \
//									srcNodeID, pScrNode->DisplayName, destNodeID, pDestNode->DisplayName ) );
//	return;
//#endif
//
//	C2G_TELEPORT_BY_PLATFORM msg;
//	msg.wMsg = c2g_teleport_by_platform;
//	msg.bySrcPlatform = srcNodeID;
//	msg.byTgtPlatform = destNodeID;
//	_Send(&msg, sizeof(msg));
}

//join the battle queue
void CMessageHandler::Send_Join_Battles( UINT selectedBattleBit )
{
	////
	//C2M_JOIN_BATTLE msg;
	//msg.wMsg = c2m_join_battle;
	//msg.dwBattleMatch = selectedBattleBit;
	//_Send(&msg, sizeof(msg));
}

//
void CMessageHandler::Parse_m2c_join_battle_queue(const char* buf, UINT32 size)
{
	M2C_JOIN_BATTLE_QUEUE* msg = (M2C_JOIN_BATTLE_QUEUE*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	MMOBattleManager::getSingleton().NotifyJoinedBattle();
}
	
//leave the joined battle queue
void CMessageHandler::Send_Leave_WaitingBattle()
{
//#if M_DEBUG_SCENE
//	M2C_CANCEL_JOIN_BATTLE_ACK *msg = (M2C_CANCEL_JOIN_BATTLE_ACK*)ms_MsgBuffer;
//	msg->wMsg = m2c_cancel_join_battle_ack;
//	Parse_m2c_cancel_join_battle_ack((const char*)msg, sizeof(M2C_CANCEL_JOIN_BATTLE_ACK));
//	return;
//#else
//	//
//	C2M_CANCEL_JOIN_BATTLE_REQ msg;
//	msg.wMsg = c2m_cancel_join_battle_req;
//	_Send(&msg, sizeof(msg));
//#endif
}

//
void CMessageHandler::Parse_m2c_cancel_join_battle_ack(const char* buf, UINT32 size)
{
	M2C_CANCEL_JOIN_BATTLE_ACK* msg = (M2C_CANCEL_JOIN_BATTLE_ACK*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	MMOBattleManager::getSingleton().NotifyLeaveBattle();
	//
}

//join the battle
void CMessageHandler::Send_Ack_JoinBattle( UBOOL isAgreed )
{
	//C2M_JOIN_BATTLE_CONFIRM_ACK msg;
	//msg.wMsg = c2m_join_battle_confirm_ack;
	//msg.byResult = isAgreed ? E_etar_Agree : E_etar_DisAgree ;
	//_Send(&msg, sizeof(msg));
}

//
void CMessageHandler::Parse_m2c_join_battle_confirm_req(const char* buf, UINT32 size)
{
	M2C_JOIN_BATTLE_CONFIRM_REQ* msg = (M2C_JOIN_BATTLE_CONFIRM_REQ*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	//
	MMOBattleManager::getSingleton().NotifyEnterBattle( msg->dwSceneTempID );
}

//====================================================================================================
//ask for battle summary(or arena summary) data
void CMessageHandler::Send_GetFightSummary()
{
	//C2G_QUERY_COMBAT_STAT msg;
	//msg.wMsg = c2g_query_combat_stat;
	//_Send(&msg, sizeof(msg));
}

//add by zhangrui05@snda.com
void CMessageHandler::Send_UpdateClientVariable(UINT8 index, INT32 value)
{
	C2G_UPDATE_CLIENT_VARIABLE msg;
	msg.wMsg = c2g_update_client_variable;
	msg.byVarIndex = index;
	msg.nVarValue = value;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_UpdateClientVarRange(UINT8 idx_start, UINT8 idx_end, BYTE* buf, size_t size)
{
	if (!buf || idx_start >= idx_end)
		return;
	if (idx_start >= ECVT_End || idx_end >= ECVT_End)
		return;
	if (((idx_end - idx_start + 1)<<2) != size)
		return;
	INT32* addr = (INT32*)buf;
	for (int npos = idx_start; npos <= idx_end; ++npos)
	{
		MMOClientVariableManager::getSingleton().SetClientVariable((EClientVariableType)npos, addr[npos-idx_start]);
	}
}

void CMessageHandler::Parse_g2c_sync_combat_stat(const char* buf, UINT32 size)
{
	G2C_SYNC_COMBAT_STAT* msg = (G2C_SYNC_COMBAT_STAT*)buf;
	if (!buf || size < sizeof(*msg) ||
		(size - sizeof(*msg)) % sizeof(msg->DataList[0]) != 0)
		return;
	INT byCount = ( size - sizeof(G2C_SYNC_COMBAT_STAT) ) / sizeof(msg->DataList[0]);

	//MMOArenaManager::getSingleton().UpdateArenaSummary( msg->DataList , byCount );

	//if this is battle summary data
	MMOBattleManager *pBattleManager = &MMOBattleManager::getSingleton();	
	if( pBattleManager->IsInBattle() || pBattleManager->IsBattleEnding() ){
		pBattleManager->ParseBattleCombatDatas( msg, byCount );
	}

	////
	//MMOMapDataManager &mmoMapDataMgr = MMOMapDataManager::getSingleton();
	//FExportMap &curScene = mmoMapDataMgr.GetCurMap();
	//if( curScene.ServerMapType == E_est_Dungeon ){
	//	//在副本里的战斗数据
	//	mmoMapDataMgr.ParseDungeonCombatDatas( msg, byCount );
	//}
}

void CMessageHandler::Send_CancelMatinee()
{
	C2G_CANCEL_CUTSCENE msg;
	msg.wMsg = c2g_cancel_cutscene;
	_Send(&msg, sizeof(msg));
}

extern FLOAT g_preLoadX;
extern FLOAT g_preLoadY;

void CMessageHandler::Parse_g2c_sync_next_jump_point(const char* buf, UINT32 size)
{
	G2C_SYNC_NEXT_JUMP_POINT* msg = (G2C_SYNC_NEXT_JUMP_POINT*)buf;
	if (!buf || size < sizeof(*msg))
		return;
	FVector pos(msg->wJumpX,msg->wJumpY,msg->wJumpZ);
	ConvertCoordN2C(pos);

	g_preLoadX = pos.X;
	g_preLoadY = pos.Y;
}


