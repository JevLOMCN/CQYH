#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIFBTask)

void UMMOUIFBTask::PostOnShow(UBOOL show)
{
	Super::PostOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//

	if( show ){
		UpdatePanel();
		UpdatePosition();
	}
	else{
		//ResetUI();
	}
}
//
void UMMOUIFBTask::UpdatePosition()
{
	UMMOUIMinimap *minimapPanel = (UMMOUIMinimap*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Minimap);
	if (minimapPanel == NULL)
	{
		return;
	}

	FLOAT offsetVer;
	if (minimapPanel->IsReputationTrace)
	{
		offsetVer = minimapPanel->PanelBottomWithReputation;
	}
	else
	{
		offsetVer = minimapPanel->PanelBottomWithoutReputation;
	}

	FVector2D newPos;
	GetPosition(newPos.X, newPos.Y);

	newPos.Y = offsetVer + 2;
	SetPosition(newPos.X, newPos.Y);

//	UMMOUIQuestTracePanel* questTracePanel = (UMMOUIQuestTracePanel*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_QuestTrace);
//	if (questTracePanel && questTracePanel->IsVisible)
//	{
//		questTracePanel->UpdatePosition();
//	}
}
//
void UMMOUIFBTask::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (NULL == evt){
		return;
	}

	//
	switch( evt->GetEventType() )
	{
	case EGameEvent::GameEvent_EnterScene:
		{
			FExportMap &curScene = MMOMapDataManager::getSingleton().GetCurMap();
			if( curScene.ServerMapType != E_est_Dungeon ){
				Show( FALSE );
				break;
			}

			UpdatePanel();
		}
		break;
	}
}

//
void UMMOUIFBTask::UpdatePanel()
{
	if( FBTitleTf != NULL ){
		FExportMap &curScene = MMOMapDataManager::getSingleton().GetCurMap();
		FString panelTitle = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBTask.PanelTitle") ), *curScene.MapDisplayName );
		FBTitleTf->SetString( TEXT("text"), panelTitle );
	}
}
//========================================================================================
//void UMMOUIFBTask::OnNativeGameEvent(struct MMOGameEvent* evt)
//
void UMMOUIFBTask::UpdateFBTaskList( INT fbMainTargetIdx )
{
	//FB progress
	TArray<CurSceneTarget> &curSceneTargets = MMOMapDataManager::getSingleton().GetCurSceneProgress();
	if( !curSceneTargets.IsValidIndex( fbMainTargetIdx ) ){
		return;
	}
	CurSceneTarget &curFBProgress = curSceneTargets( fbMainTargetIdx );
	
	UINT itemObjIdx = 0;
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	
	//Total progress
	if( curFBProgress._MainTarget._Template != NULL ){
		const FSceneProgress *pFBMainProgressTemplate = curFBProgress._MainTarget._Template;

		UGFxObject* fbProgressListObj = GFXPlayer->CreateObject( TEXT("UI_FBTask.FBTaskAS3.FBTaskListItemData") );

		fbProgressListObj->SetString( TEXT("TaskTitle"), pFBMainProgressTemplate->Desc );
		fbProgressListObj->SetInt( TEXT("CurInt"), curFBProgress._MainTarget._CurValue );
		fbProgressListObj->SetInt( TEXT("TargetInt"), pFBMainProgressTemplate->TargetValue );
		
		fbProgressListObj->SetBool( TEXT("IsFailed"), FALSE );
		fbProgressListObj->SetInt( TEXT("ID"), pFBMainProgressTemplate->ID );
		fbProgressListObj->SetInt( TEXT("TaskType"), 1 );

		dataProvider->SetElementObject( itemObjIdx, fbProgressListObj );
		++itemObjIdx;
	}

	//Progress details
	curFBProgress._ChildTargets;
	for( INT j = 0; j < UCONST_MaxSceneMinorTarget; ++j ){
		const CurSceneProgress &childProgress = curFBProgress._ChildTargets[j];
		if( childProgress._Template == NULL || childProgress._Template->ID <= 0 || childProgress._Template->TargetValue < 0 ){
			continue;
		}

		UGFxObject* fbProgressListObj = GFXPlayer->CreateObject( TEXT("UI_FBTask.FBTaskAS3.FBTaskListItemData") );

		//FString taskProgressDesc = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBTask.TaskProgressFormat") ), *childProgress._Template->Desc );
		//fbProgressListObj->SetString( TEXT("TaskTitle"), taskProgressDesc );
		fbProgressListObj->SetString( TEXT("TaskTitle"), childProgress._Template->Desc );
		fbProgressListObj->SetInt( TEXT("CurInt"), childProgress._CurValue );
		fbProgressListObj->SetInt( TEXT("TargetInt"), childProgress._Template->TargetValue );

		fbProgressListObj->SetBool( TEXT("IsFailed"), FALSE );
		fbProgressListObj->SetInt( TEXT("ID"), childProgress._Template->ID );
		fbProgressListObj->SetInt( TEXT("TaskType"), 2 );

		dataProvider->SetElementObject( itemObjIdx, fbProgressListObj );
		++itemObjIdx;
	}

	eventUCFlashFBTaskList(dataProvider);

	UpdatePosition();
}

//========================================================================================
void UMMOUIFBTask::Init()
{
	Super::Init();

	check(NULL == EventAdapter);
	if (NULL == EventAdapter){
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_EnterScene);
		EventAdapter = new TGameEventAdapter<UMMOUIFBTask>(this, evts);
	}
	//
	InitMc();
}

//========================================================================================
void UMMOUIFBTask::InitMc()
{
	PanelWidth = GetInt( TEXT("width") );
	PanelHeight = GetInt( TEXT("height") );

	FBTitleTf = GetObject( TEXT("FBTitleTf") );
	FBTaskScrollListMc = GetObject( TEXT("FBTaskScrollListMc") );
}

//===============================================================================================================
//===============================================================================================================
UBOOL UMMOUIFBTask::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(strHoverControllerName==TEXT("FBTaskScrollListMc") ||
			strHoverControllerName==TEXT("FBTitleTf")	)
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}
//===============================================================================================================
//===============================================================================================================