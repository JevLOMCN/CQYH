#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIHintSceneWidget)

//========================================================================================
void UMMOUIHintSceneWidget::ShowHintSceneWidget()
{
	MMOMapDataManager &mapDataManager = MMOMapDataManager::getSingleton();

	//
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer == NULL ){
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
		return;
	}
	INT regionIdx = mapDataManager.GetCurPlayerRegionIn( mainPlayer->Location );
	FInMapRegion *pCurReginInCurMap = mapDataManager.GetReginInCurMap( regionIdx );

	FString hintContent;
	if( pCurReginInCurMap == NULL ){
		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_DynamicUI::ShowHintSceneWidget] NoMapRegion in CurSceneMap") );
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
		return;
	}
	else{
		//update map name
		hintContent = pCurReginInCurMap->DisplayName;
		if( hintContent.IsEmpty() ){
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
			return;
		}
	}

	FExportMap &curMapInfo = mapDataManager.GetCurMap();
	UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
	if( canChangeLines == TRUE ){
		FString stringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.SceneLineListString") );
		UINT curSceneLineID = 0;
		UINT curSceneServerID = 0;
		UINT curCombinatedSceneID = mapDataManager.GetCurSceneLineIn();
		MMOMapDataManager::ParseServerMapLine( curCombinatedSceneID, curSceneServerID, curSceneLineID );
		hintContent += FString::Printf( *stringFormat, curSceneLineID );
	}
	
	ShowHintSceneMessage( hintContent, 48 );
}
//
void UMMOUIHintSceneWidget::CloseHintSceneWidget()
{
	Close();
}

//
void UMMOUIHintSceneWidget::ShowHintSceneMessage( const FString &text, INT textSize )
{
	// Do not check hints during cutscenes
	AMMOPlayerController* pCtrl = MMOFacade::getSingleton().GetMainController();
	if(pCtrl && pCtrl->IsPlayingMatinee()==FALSE)
	{
		MMOMapDataManager &mapDataManager = MMOMapDataManager::getSingleton();

		FASValue asHint(EC_EventParm);
		asHint.Type = AS_String;
		asHint.S = text;

		FASValue asHintColor(EC_EventParm);
		asHintColor.Type = AS_Int;
		asHintColor.I = textSize;

		//
		TArray<struct FASValue> asFunArgs;
		asFunArgs.AddItem( asHint );
		asFunArgs.AddItem( asHintColor );
		this->Invoke( TEXT("ShowHintScene"), asFunArgs );

		SetVisible( TRUE );
	}
}

//
void UMMOUIHintSceneWidget::Close()
{
	SetVisible( FALSE );
}

//===============================================================================================================
//===============================================================================================================
UBOOL UMMOUIHintSceneWidget::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if( strHoverControllerName == TEXT("hintScene_mc") || strHoverControllerName == TEXT("tF_hintScene") || strHoverControllerName == TEXT("ScreenPopUpBoxMsgAOGMc"))
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}
//===============================================================================================================
//===============================================================================================================