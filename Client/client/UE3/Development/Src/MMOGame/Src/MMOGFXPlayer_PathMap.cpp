#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_PathMap)
	
void UMMOGFXPlayer_PathMap::BeforeViewUpdate(float DeltaTime)
{
	//Super::PostAdvance(DeltaTime);
	//world map
	//if( _PathMap && _PathMap->IsVisible ){
	//	_PathMap->UpdatePathMap();
	//}
}

//========================================================================================================
void UMMOGFXPlayer_PathMap::ASShowPathMapMarkHint( INT type, INT idx )
{
	warnf( TEXT( "ASShowPathMapMarkHint at Type:%d Idx:%d"), type, idx );

	MMOMapDataManager *pMapDataMgr = &MMOMapDataManager::getSingleton();

	//FString displayText;
	FObjInMapUI *pMapMark = NULL;
	switch( type )
	{
	case EACTMUI_StaticNPC:
	case EACTMUI_TeleportFB:
	case EACTMUI_TeleportMap:
	case EACTMUI_TeleportInDoor:
	case EACTMUI_TeleportNode:
		{
			TArray< FActorInMapUI > &staticMarks = pMapDataMgr->GetCurMapStaticActorPointsList();
			if( staticMarks.IsValidIndex( idx ) ){
				pMapMark = &staticMarks( idx );
			}
		}

		break;

	case EACTMUI_NearMonster:
	case EACTMUI_NearBoss:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorNearByUiID( idx );
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
			}
		}
		break;

	case EACTMUI_GlobalTeammate:
	case EACTMUI_BattleFlag:
		{
			pMapMark = pMapDataMgr->GetMapDymcActorGlobalByUiID( idx );
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
	UGFxObject *pMc = pMapMark->_UiObjInPathMap;
	if( pMc == NULL ){
		return;
	}

	//
	pMc->SetString( TEXT("tipsHtml"), pMapMark->_DisplayName );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, pMc );
}

void UMMOGFXPlayer_PathMap::ASHidePathMapMarkHint()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//==============================================================================================
void UMMOGFXPlayer_PathMap::ASPathMapToMinimap()
{
	UMMOUIMinimap *minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Minimap );
	if( minimapPanel != NULL ){
		minimapPanel->SwitchMinimapMode();
	}

	//
	if( _PathMap ){
		_PathMap->Show( FALSE );
	}
}

//======================================================================================
void UMMOGFXPlayer_PathMap::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_PathMap::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_PathMap);
}

void UMMOGFXPlayer_PathMap::UnInit()
{
	Super::UnInit();

	if( _PathMap != NULL )
	{
		_PathMap->ClearPathMapUI();
	}
}