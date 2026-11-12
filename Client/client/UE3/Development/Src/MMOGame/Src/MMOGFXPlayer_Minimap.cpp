#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOBattleManager.h"
#include "MMOMapDataManager.h"
#include "ClientActionConditionCheck.h"
#include "MMONavigationManager.h"
#include "FSMIO.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Minimap)
	
void UMMOGFXPlayer_Minimap::BeforeViewUpdate(float DeltaTime)
{
	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}
	//Super::PostAdvance(DeltaTime);
	//mini Map
	if( _Minimap && _Minimap->IsVisible )
	{
		_Minimap->UpdateMinimap();
		_Minimap->UpdatePingRate(DeltaTime);
	}
}
//========================================================================================================
void UMMOGFXPlayer_Minimap::UpdateMinimap(float DeltaTime)
{
	//if(MMO::GetCurrentGameState() != EGameState_InGame)
	//{
	//	return;
	//}
	////mini Map
	//if( _Minimap && _Minimap->IsVisible ){
	//	_Minimap->UpdateMinimap();
	//}
}
//========================================================================================================
void UMMOGFXPlayer_Minimap::ASMoveToMinimapPos( INT clickPosX, INT clickPosY )
{
	FVector world3DPos = _Minimap->ConvertPosInMapUIToWorld3DPos( clickPosX, clickPosY );

	FExportMap curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	MMOFacade::getSingleton().PlayerMoveToScenePos( curMapInfo, world3DPos );
}
//========================================================================================================
void UMMOGFXPlayer_Minimap::ASShowMapLineDropList()
{
	CMessageHandler::GetInstance().Send_QuerySceneLine();
}
//========================================================================================================
void UMMOGFXPlayer_Minimap::ASChangeSceneLine( int lineID )
{
	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_SwitchLine);
	MMOUI::PostGameEvent(&evtGuide);

	ClientActionCondition_ChangeSceneLine checker;
	FClientActionConditionContex contex;
	if( !checker.CheckClientActionCondition(contex) ){
		if( _Minimap != NULL ){
			_Minimap->UpdateSceneLinesList();
		}
		return;
	}

	INT curCombinatedSceneID = MMOMapDataManager::getSingleton().GetCurSceneLineIn();
	if( curCombinatedSceneID == lineID ){
		FString errorName = TEXT("MMOGame.Minimap.ErrorChangeSceneLineIsCurrent");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}
	//
	_Minimap->PreparingToChangeSceneLine( lineID );
}

//========================================================================================================
void UMMOGFXPlayer_Minimap::ASShowHintMapType( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	//
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	FString stringLookforKey = TEXT("MMOGame.Minimap.SceneType");
	stringLookforKey += appItoa( curMapInfo.ServerMapType );

	mc->SetString( TEXT("tipsHtml"), MMOUI::ParseLocalizedPropertyPath( stringLookforKey ) );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//
void UMMOGFXPlayer_Minimap::ASHideHintMapType()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//========================================================================================================
void UMMOGFXPlayer_Minimap::ASClickMailIcon()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Mail, "");
}

//========================================================================================================
void UMMOGFXPlayer_Minimap::ASShowMinimapMarkHint( INT type, INT idx )
{
	warnf( TEXT( "ASShowMinimapMarkHint at Type:%d Idx:%d"), type, idx );

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	FString tipsText;
	FObjInMapUI *pMapMark = NULL;
	switch( type )
	{
	case EACTMUI_StaticNPC:
	case EACTMUI_TeleportFB:
	case EACTMUI_TeleportMap:
	case EACTMUI_TeleportInDoor:
	case EACTMUI_TeleportNode:
	case EACTMUI_QuestZone:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
			if( staticMarks.IsValidIndex( idx ) ){
				pMapMark = &staticMarks( idx );
			}
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	case EACTMUI_NearMonster:
	case EACTMUI_NearBoss:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	case EACTMUI_NpcGroup:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
			if( staticMarks.IsValidIndex(idx) ){
				pMapMark = &staticMarks(idx);

				if( pMapMark != NULL ){
					INT strPos = pMapMark->_DisplayName.InStr( TEXT(":") );
					if( strPos >= 0 ){
						tipsText = pMapMark->_DisplayName.Left( strPos );
					}
				}
			}
		}
		break;
	case EACTMUI_NearPlayer:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
			//because when this player was added, his info had not arrived yet
			if( pMapMark != NULL ){
				AMMOActor *pPlayer = MMOFacade::getSingleton().GetCharacter(  ((FActorInMapUI*)pMapMark)->_ActorID );
				if( pPlayer != NULL ){
					pMapMark->_DisplayName = pPlayer->GetActorName();
				}
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	case EACTMUI_GlobalTeammate:
	case EACTMUI_BattleFlag:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorGlobalByUiID( idx );
			if( pMapMark != NULL ){
				tipsText = pMapMark->_DisplayName;
			}
		}
		break;
	default:
		break;
	}

	//
	if( pMapMark == NULL ){
		return;
	}

	//
	UGFxObject *pMc = pMapMark->_UiObjInMiniMap;
	if( pMc == NULL ){
		return;
	}

	//
	if( tipsText.IsEmpty() ){
		return;
	}

	pMc->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, pMapMark->_UiObjInMiniMap );
	FSMIO::getSingleton().Trigger.SetShowTips(TRUE,TRUE);
}

void UMMOGFXPlayer_Minimap::ASHideMinimapMarkHint()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================
//
void UMMOGFXPlayer_Minimap::ASClickBattleInfo( INT x , INT y )
{
	if( _Minimap == NULL ){
		return;
	}

	_Minimap->OnClickBattleInfoBtn( x, y );
}

//
void UMMOGFXPlayer_Minimap::ASHintBattleInfo()
{
	if( _Minimap == NULL ){
		return;
	}

	_Minimap->HintBattleInfo();
}

//
void UMMOGFXPlayer_Minimap::ASHideHintBattleInfo()
{
	if( _Minimap == NULL ){
		return;
	}

	_Minimap->HideHintBattleInfo();
}

//==============================================================================================
void UMMOGFXPlayer_Minimap::ASToggleArenaEnt()
{
	UMMOUIArenaEntrance *pArenaEnt = (UMMOUIArenaEntrance*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Arena_ArenaEntrance );
	if( pArenaEnt == NULL ){
		pArenaEnt = (UMMOUIArenaEntrance*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Arena_ArenaEntrance );
		if( pArenaEnt != NULL ){
			pArenaEnt->Show( TRUE );
		}
	}
	else{
		pArenaEnt->Show( !pArenaEnt->IsVisible );
	}
}

//==============================================================================================
void UMMOGFXPlayer_Minimap::ASClickFbInfo( INT x , INT y )
{
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_Dungeon, x, y, 0, curMapInfo.MapDisplayName, 0 );
}
//
void UMMOGFXPlayer_Minimap::ASNotifyFBTimesUp()
{
	if( _Minimap == NULL ){
		return;
	}

	_Minimap->StopFBTimeCountDownAndHide();
}

//==============================================================================================
void UMMOGFXPlayer_Minimap::ASToggleMap()
{
	if( _Minimap == NULL ){
		return;
	}

	_Minimap->ToggleMinimapShowType();
}

//---------------------------------------------------------------
void UMMOGFXPlayer_Minimap::ASShowTipsToggleShowMap()
{
	if( _Minimap == NULL ){
		return;
	}
	_Minimap->ShowTipsToggleShowMap();
}
//==============================================================================================
void UMMOGFXPlayer_Minimap::ASToggleShowMap()
{
	if( _Minimap == NULL ){
		return;
	}

	_Minimap->ToggleShowMap();
}

//==============================================================================================
void UMMOGFXPlayer_Minimap::ASStopPathFind()
{
	MMONavigationManager::getSingleton().StopNavigate();
}

//==============================================================================================
void UMMOGFXPlayer_Minimap::ASClickShoppingMallBtn()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Mall, "");
}
void UMMOGFXPlayer_Minimap::ASClickActivityBtn()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Activity, "");
}
//==============================================================================================
void UMMOGFXPlayer_Minimap::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Minimap::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Minimap);

	MMOUI::GetUIManager()->SetUpdateMinimapDelegate(FScriptDelegate(this, TEXT("UpdateMinimap")));
	MMOUI::GetUIManager()->SetUpdateTraceReputationDataDelegate(FScriptDelegate(this, TEXT("UpdateTraceReputationData")));

}

void UMMOGFXPlayer_Minimap::UnInit()
{
	Super::UnInit();
	MMOUI::GetUIManager()->SetUpdateTraceReputationDataDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetUpdateMinimapDelegate(FScriptDelegate(EC_EventParm));
}

void UMMOGFXPlayer_Minimap::UpdateTraceReputationData( UGFxObject *data)
{
	if( data ){
		UGFxObject * ReputationObj = CreateObject("Object");
		if(ReputationObj){
			ReputationObj->SetString("label",data->GetString("label"));
			ReputationObj->SetInt("index",0);
			ReputationObj->SetInt("type",data->GetInt("type"));
			ReputationObj->SetInt("deepth",data->GetInt("deepth"));
			ReputationObj->SetString("tips",data->GetString("tips"));
			UBOOL bV = data->GetBool("visible");
			ReputationObj->SetBool("visible",bV);
			if(bV)
			{
				ReputationObj->SetInt("colorReputation",data->GetInt("colorReputation"));
				ReputationObj->SetInt("curReputation",data->GetInt("curReputation"));
				ReputationObj->SetInt("minReputation",data->GetInt("minReputation"));
				ReputationObj->SetInt("maxReputation",data->GetInt("maxReputation"));
				ReputationObj->SetString("txtReputation",data->GetString("txtReputation"));
			}


			if( _Minimap )
			{
				_Minimap->IsReputationTrace = bV;
				_Minimap->eventCallUpdateTraceRelation(ReputationObj);

				UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
				if (fbTaskPanel && fbTaskPanel->IsVisible)
				{
					fbTaskPanel->UpdatePosition();
				}
				else
				{
//					UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
//					if (questTracePanel && questTracePanel->IsVisible)
//					{
//						questTracePanel->UpdatePosition();
//					}
				}
			}
		}
	}

}
//============================================================================================
void UMMOGFXPlayer_Minimap::ASShowTips_PinValue(class UGFxObject* mc)
{
	//
	if(mc == NULL)
		return;

	UINT32 nPin = MMOFacade::getSingleton().GetGameWorldPingDelayTime();
	FString nFPS = _Minimap->GetFPS();//MMOFacade::getSingleton().GetGatePingDelayTime();
	FString tipStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FPS.Ping"));
	FString tips = FString::Printf(*tipStr, nPin,*nFPS);
	mc->SetString( TEXT("tipsHtml"),tips);
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
	
	bShowTips = TRUE;

}
void UMMOGFXPlayer_Minimap::ASHideTips_PinValue(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
	bShowTips = FALSE;
}
//===================================================================================================================================
void UMMOGFXPlayer_Minimap::ASShowMiniMapTrace()
{
	/*
	UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
	if (questTracePanel)
	{
		questTracePanel->_IsExpand = TRUE;
		questTracePanel->Show(TRUE);
	}

	UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
	if (fbTaskPanel)
	{
		fbTaskPanel->Show(TRUE);
	}
	*/
	UMMOUIJourneyTracePanel* journeyPanel = (UMMOUIJourneyTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Journey);
	if (journeyPanel)
	{
		journeyPanel->Show(TRUE);
	}
	
}

void UMMOGFXPlayer_Minimap::ASHideMiniMapTrace()
{
	/*
	UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
	if (questTracePanel && questTracePanel->IsVisible)
	{
		questTracePanel->_IsExpand = FALSE;
		questTracePanel->Show(FALSE);
	}

	UMMOUIFBTask* fbTaskPanel = (UMMOUIFBTask*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_FBTask);
	if (fbTaskPanel && fbTaskPanel->IsVisible)
	{
		fbTaskPanel->Show(FALSE);
	}
	*/
	UMMOUIJourneyTracePanel* journeyPanel = (UMMOUIJourneyTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Journey);
	if (journeyPanel)
	{
		journeyPanel->Show(FALSE);
	}
}

//===================================================================================================================================
void UMMOGFXPlayer_Minimap::ASUpdateFPSTips(class UGFxObject* mc)
{
	if (bShowTips)
		ASShowTips_PinValue(mc);
	
}

//===================================================================================================================================
void UMMOGFXPlayer_Minimap::ASShowTips_MapLineInfo( INT stateType )
{
	FString tipsStrName = TEXT("MMOGame.Minimap.MapLineHint");
	tipsStrName += appItoa( stateType );
	FString tipStr = MMOUI::ParseLocalizedPropertyPath( tipsStrName );

	MMOUI::GetUIManager()->delegateShowTextTips( tipStr );
}

//--------------------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Minimap::ASHideTips_MapLineInfo()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//===================================================================================================================================
void UMMOGFXPlayer_Minimap::ASShowTips_MapZone( INT zoneType )
{
	FString tipsStrName = TEXT("MMOGame.Minimap.MapZoneHint");
	tipsStrName += appItoa( zoneType );
	FString tipStr = MMOUI::ParseLocalizedPropertyPath( tipsStrName );

	MMOUI::GetUIManager()->delegateShowTextTips( tipStr );
}

//--------------------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Minimap::ASHideTips_MapZone()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===================================================================================================================================