#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ScaleformEngine.h"
#include "FSMIO.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_DynamicUI)
IMPLEMENT_CLASS(UMMOUITransducerInfo)

bool g_displayDragDropIcon = false;



void UMMOGFXPlayer_DynamicUI::Init()
{
	Super::Init();

	FGFxEngine::GetEngine()->SetTopMostMovie(this->pMovie);

	check(FALSE && "UMMOGFXPlayer_DynamicUI::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_DynamicUI);

	//MMOUI::GetUIManager()->SetCommonUIComponentMoviePlayer(this);

//	MMOUI::GetUIManager()->SetShowProgressBarDelegate(FScriptDelegate(this, TEXT("ShowProgressBar")));
//	MMOUI::GetUIManager()->SetCloseProgressBarDelegate(FScriptDelegate(this, TEXT("CloseProgressBar")));

//	MMOUI::GetUIManager()->SetShowProgressBarStrengthDelegate(FScriptDelegate(this, TEXT("ShowProgressBarStrength")));
//	MMOUI::GetUIManager()->SetCloseProgressBarStrengthDelegate(FScriptDelegate(this, TEXT("CloseProgressBarStrength")));

//	MMOUI::GetUIManager()->SetShowProgressBarQTEDelegate(FScriptDelegate(this, TEXT("ShowProgressBarQTE")));
//	MMOUI::GetUIManager()->SetCloseProgressBarQTEDelegate(FScriptDelegate(this, TEXT("CloseProgressBarQTE")));

	//MMOUI::GetUIManager()->SetAddBuffPanelDelegate(FScriptDelegate(this, TEXT("AddBuffPanel")));
	//MMOUI::GetUIManager()->SetDeleteBuffPanelDelegate(FScriptDelegate(this, TEXT("DeleteBuffPanel")));

//	MMOUI::GetUIManager()->SetCloseHintSceneWidgetDelegate(FScriptDelegate(this, TEXT("CloseHintSceneWidget")));
//	MMOUI::GetUIManager()->SetCloseHintLevelUpWidgetDelegate(FScriptDelegate(this, TEXT("CloseHintLevelUpWidget")));

	//MMOUI::GetUIManager()->SetAddSkillLearnedMsgDelegate(FScriptDelegate(this, TEXT("AddSkillLearnedMsg")));

//	MMOUI::GetUIManager()->SetShowHintSceneWidgetDelegate(FScriptDelegate(this, TEXT("ShowHintSceneWidget")));
//	MMOUI::GetUIManager()->SetShowHintLevelUpWidgetDelegate(FScriptDelegate(this, TEXT("ShowHintLevelUpWidget")));

	//MMOUI::GetUIManager()->SetASClickQTEDelegate(FScriptDelegate(this, TEXT("ASClickQTE")));


	//MMOUI::GetUIManager()->SetShowUnlockAOGStringDelegate(FScriptDelegate(this, TEXT("ShowUnlockAOGString")));
	//MMOUI::GetUIManager()->SetHideUnlockAOGStringDelegate(FScriptDelegate(this, TEXT("HideUnlockAOGString")));


	__OnPostAdvance__Delegate = FScriptDelegate(this, TEXT("PostAdvance"));
}

void UMMOGFXPlayer_DynamicUI::UnInit()
{
	Super::UnInit();

	FGFxEngine::GetEngine()->SetTopMostMovie(NULL);

	MMOUI::GetUIManager()->SetCommonUIComponentMoviePlayer(NULL);

//	MMOUI::GetUIManager()->SetShowProgressBarDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetCloseProgressBarDelegate(FScriptDelegate(EC_EventParm));

//	MMOUI::GetUIManager()->SetShowProgressBarStrengthDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetCloseProgressBarStrengthDelegate(FScriptDelegate(EC_EventParm));

//	MMOUI::GetUIManager()->SetShowProgressBarQTEDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetCloseProgressBarQTEDelegate(FScriptDelegate(EC_EventParm));

	//MMOUI::GetUIManager()->SetAddBuffPanelDelegate(FScriptDelegate(EC_EventParm));
	//MMOUI::GetUIManager()->SetDeleteBuffPanelDelegate(FScriptDelegate(EC_EventParm));

//	MMOUI::GetUIManager()->SetCloseHintSceneWidgetDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetCloseHintLevelUpWidgetDelegate(FScriptDelegate(EC_EventParm));

	//MMOUI::GetUIManager()->SetAddSkillLearnedMsgDelegate(FScriptDelegate(EC_EventParm));

//	MMOUI::GetUIManager()->SetShowHintSceneWidgetDelegate(FScriptDelegate(EC_EventParm));
//	MMOUI::GetUIManager()->SetShowHintLevelUpWidgetDelegate(FScriptDelegate(EC_EventParm));

	//MMOUI::GetUIManager()->SetASClickQTEDelegate(FScriptDelegate(EC_EventParm));

	//MMOUI::GetUIManager()->SetShowUnlockAOGStringDelegate(FScriptDelegate(EC_EventParm));
	//MMOUI::GetUIManager()->SetHideUnlockAOGStringDelegate(FScriptDelegate(EC_EventParm));


	__OnPostAdvance__Delegate = FScriptDelegate(EC_EventParm);

	AMMOHud *hud = MMOFacade::getSingleton().GetMyHud();
	if(hud)
	{
		FScriptDelegate fun(EC_EventParm);
		fun.FunctionName = "OnPostRender";
		fun.Object = this;
		hud->vecOnPostRender.RemoveItem(fun);
	}
}

void UMMOGFXPlayer_DynamicUI::OnHUDLoaded()
{
	//add delegate after HUD finishes loading
	AMMOHud *hud = MMOFacade::getSingleton().GetMyHud();

	FScriptDelegate fun(EC_EventParm);
	fun.FunctionName = "OnPostRender";
	fun.Object = this;

	if( hud )
		hud->AddPostRenderDelegate( fun );
}

void UMMOGFXPlayer_DynamicUI::OnPostRender( UCanvas*  cvs, float fDelta )
{
	if(!cvs)
		return;
	if( _QTEBar && _QTEBar->IsVisible )
		_QTEBar->UpdateQTE(cvs);
}

//void UMMOGFXPlayer_DynamicUI::ASClickQTE()
//{
//	if(_QTEBar&&_QTEBar->IsVisible)
//	{
//		eventCloseProgressBarQTE(0);	
//
//		AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();	
//		if(ctrl&&ctrl->GetPlayer())
//		{
//			UMMOAbility*	useAbility	= ctrl->GetPlayer()->GetEquipedAbilityById( ctrl->KeyMapManager->GetCmdIDBySlotID(ECK_QTE1) );// ECK_QTE2) );
//			//UMMOAbility*	useAbility	= ctrl->GetPlayer()->GetAbilityById( _QTEBar->GetTargetAbilityID()); 
//			AMMOActor*		Target = MMOFacade::getSingleton().GetCharacter( _QTEBar->GetTargetActorID());
//			if(useAbility&&Target)
//			{
//				//MMOTargetManager::getSingleton().SetDesireTarget(useAbility, Target);
//				FSMIO::getSingleton().Target.SetDesireTarget(useAbility, Target);
//				//ctrl->AddCmdHuntToUseAbility(useAbility,Target,Target->Location);
//			}
//		}
//	}
//}
// update drag
void UMMOGFXPlayer_DynamicUI::PostAdvance(float DeltaTime)
{
	//iterate
	//Super::PostAdvance(DeltaTime);
	//if(g_displayDragDropIcon)
	//{
	//	FIntPoint pt = MMOFacade::GetMouseCursorPosition();
	//	UGFxObject* dropIcon = GetDragIcon();
	//	//INT w = dropIcon->GetInt(TEXT("width")) / 2;
	//	//INT h = dropIcon->GetInt(TEXT("height")) / 2;
	//	dropIcon->SetPosition(pt.X /*- w*/, pt.Y /*- h*/);
	//}

	//if (NULL != _Tips && _Tips->IsVisible)
	//	_Tips->PostAdvance(DeltaTime);
	//if(_BossBlood)
	//	_BossBlood->TickForDistance(DeltaTime);
}


//void UMMOGFXPlayer_DynamicUI::ASWeatherChangePlayEnd()
//{
//	if (_WeatherChange)
//	{
//		_WeatherChange->SetVisible(FALSE);
//	}
//}

//void UMMOGFXPlayer_DynamicUI::ASRenownUnlockAOGShowEnd()
//{
//	if (_UnlockAOGString)
//	{
//		_UnlockAOGString->SetVisible(FALSE);
//	}
//}

//========================================================================================
//void UMMOGFXPlayer_DynamicUI::ShowHintSceneWidget()
//{
//	if( _Scene == NULL ){
//		return;
//	}
//
//	MMOMapDataManager &mapDataManager = MMOMapDataManager::getSingleton();
//
//	//
//	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
//	if( mainPlayer == NULL ){
//		return;
//	}
//	INT regionIdx = mapDataManager.GetCurPlayerRegionIn( mainPlayer->Location );
//	FInMapRegion *pCurReginInCurMap = mapDataManager.GetReginInCurMap( regionIdx );
//
//	FString hintContent;
//	if( pCurReginInCurMap == NULL ){
//		warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_DynamicUI::ShowHintSceneWidget] NoMapRegion in CurSceneMap") );
//		return;
//	}
//	else{
//		//update map name
//		hintContent = pCurReginInCurMap->DisplayName;
//		if( hintContent.IsEmpty() ){
//			return;
//		}
//	}
//
//	FExportMap &curMapInfo = mapDataManager.GetCurMap();
//	UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
//	if( canChangeLines == TRUE ){
//		FString stringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.SceneLineListString") );
//		UINT curSceneLineID = 0;
//		UINT curSceneServerID = 0;
//		UINT curCombinatedSceneID = mapDataManager.GetCurSceneLineIn();
//		MMOMapDataManager::ParseServerMapLine( curCombinatedSceneID, curSceneServerID, curSceneLineID );
//		hintContent += FString::Printf( *stringFormat, curSceneLineID );
//	}
//
//	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, TRUE, "");
//	_Scene->ShowHintSceneMessage( hintContent, 48 );
//}
////
//void UMMOGFXPlayer_DynamicUI::CloseHintSceneWidget()
//{
//	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
//	//if( _Scene != NULL ){
//	//	_Scene->Close();
//	//}
//}
