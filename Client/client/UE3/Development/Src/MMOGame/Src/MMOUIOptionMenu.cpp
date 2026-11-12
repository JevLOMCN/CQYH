#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "net/message_handler.h"
#include "MMOMsgBoxManager.h"


IMPLEMENT_CLASS(UMMOUIOptionMenu)


extern void UCSetObjectText(UMMOUIBase* wnd, const TCHAR* ctrl, const FString& txt)
{
    UGFxObject* obj = wnd->GetObject(ctrl);
    if (!obj)
        return;
    obj->SetString(_T("label") , txt);
}

void UMMOUIOptionMenu::PostOnShow(UBOOL v)
{
    Super::PostOnShow(v);

    if( v )
    {
        GFXPlayer->ASPlayUISound(TEXT("menu_System_Op"));
        GFXPlayer->CaptureAllKeys();

        FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
        FString btnText;

		switch( mapInfo.ServerMapType )
		{
		case E_est_BattleField: btnText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SystemOption.BtnText1") ); break;
		case E_est_Dungeon:
		case E_est_GuildDungeon:
			{
				btnText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SystemOption.BtnText2") );
			}
			break;
		case E_est_Arena: btnText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SystemOption.BtnText3") ); break;
		case E_est_Line: btnText = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SystemOption.BtnTextMapLine") ); break;
		}

        eventUCShowExitDuplicateButton( btnText.IsEmpty() ? FALSE : TRUE, btnText );
    }
    else
    {
        GFXPlayer->ASPlayUISound(TEXT("menu_System_Cls"));
        GFXPlayer->ClearCaptureKeys();

        if( ConfirmMsgBox != NULL )
        {
            ConfirmMsgBox->SetVisible( FALSE );
            ConfirmMsgBox = NULL;
        }
    }

	if(v)
	{
		eventShowAsMode();
	}
	else
	{
		eventDismissMode();
	}
}

void UMMOUIOptionMenu::NativeInitOnce()
{
    UCSetObjectText(this, _T("system_config"), MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.SystemConfig")));
    UCSetObjectText(this, _T("return_select"), MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.ReturnSelect")));
    UCSetObjectText(this, _T("return_login"),  MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.ReturnLogin")));
    UCSetObjectText(this, _T("exit_game"),     MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.ExitGame")));
    UCSetObjectText(this, _T("return_game"),   MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.ReturnGame")));
}

void UMMOUIOptionMenu::ReqExitDuplicate( BYTE type )
{
    if( ConfirmMsgBox != NULL )
        return;

    FScriptDelegate funYes(EC_EventParm);
    funYes.Object = this;
    funYes.FunctionName = TEXT( "ReqExitDuplicate_Ok" );

    FScriptDelegate funNo(EC_EventParm);
    funNo.Object = this;
    funNo.FunctionName = TEXT( "ReqExitDuplicate_Cancel" );
    //popup message

    FString msgInfo;
    if( type == E_est_BattleField )
        msgInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.BattleField.BattleErr5") );
    else if( type == E_est_Dungeon || type == E_est_GuildDungeon )
        msgInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBGeneral.FBGeneral6") );
    else if( type == E_est_Arena )
        msgInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Arena.AskLeaveArena") );

    if (msgInfo.IsEmpty())
        Show(FALSE);
    else
        ConfirmMsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( msgInfo, type, funYes, funNo );
}

void UMMOUIOptionMenu::ReqExitDuplicate_Ok( UGFxObject *pMc )
{
    if( pMc == ConfirmMsgBox && ConfirmMsgBox != NULL )
    {
        int type = ConfirmMsgBox->GetInt( TEXT("iteminfoid") );

        if( type == E_est_BattleField )
        {
            //MMOBattleManager::getSingleton().SetBattleState( EBTLS_ENDING );
        }
        if( type == E_est_Arena )
        {
            MMOUI::GetUIManager()->delegateCloseArenaScore();
        }
        CMessageHandler::GetInstance().Send_LeaveScene();
    }

    ConfirmMsgBox = NULL;

    Show( FALSE );
}

void UMMOUIOptionMenu::ReqExitDuplicate_Cancel( UGFxObject *pMc )
{
    ConfirmMsgBox = NULL;
}


//==========================================================================
void UMMOUIOptionMenu::ShowMapLineList()
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();

	FExportMap &curMapInfo = pMapDataManager->GetCurMap();
	UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
	if( canChangeLines == FALSE ){
		FString errorName = TEXT("MMOGame.Option.ErrorNoOtherLine");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}

	//get the lines of the current map
	TArray< MapLineInfo > &sceneLines = pMapDataManager->GetCurMapLines();
	INT curCombinatedSceneID = pMapDataManager->GetCurSceneLineIn();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT linesCnt = sceneLines.Num();
	INT curSelectedIdx = -1;
	FString listItemStringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Option.MapLineListItemFormat") );
	INT listItemIdx = 0;
	for( INT i = 0; i < linesCnt; ++i ){
		INT combinatedSceneID = sceneLines(i)._SceneLineCombineID;
		UINT sceneLineID = 0;
		UINT sceneServerID = 0;
		MMOMapDataManager::ParseServerMapLine( combinatedSceneID, sceneServerID, sceneLineID );
		if( curCombinatedSceneID == combinatedSceneID ){
			continue;
		}

		UGFxObject* listObj = GFXPlayer->CreateObject( TEXT("UI_Minimap.MinimapAS3.SceneLinesDropDownListData") );
		if( listObj == NULL ){
			continue;
		}

		listObj->SetString( TEXT("label"), FString::Printf( *listItemStringFormat, sceneLineID ) );
		listObj->SetInt( TEXT("lineID"), combinatedSceneID );
		listObj->SetInt( TEXT("lineState"), sceneLines(i)._PercentState );

		dataProvider->SetElementObject( listItemIdx++, listObj );
	}

	if( listItemIdx > 0 ){
		eventUCUpdateLine( curMapInfo.MapDisplayName, dataProvider );
	}

	if( linesCnt <= 1 ){
		FString errorName = TEXT("MMOGame.Option.ErrorNoOtherLine");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( errorName ), MsgInfoType_Table, errorName );
		return;
	}
}

void UMMOUIOptionMenu::NativeReturnLogin()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionQuit,TRUE, "");
	UMMOUIOptionQuit* QUI =  Cast<UMMOUIOptionQuit>(FindUIPanel(MMOUI::GetUIManager()->Panel_OptionQuit));
	//QUI->eventUCInitQuitTimer("Will ", 20, " seconds then exit game automatically...");
	if(QUI)
	{
		QUI->_ReturnType = 1;
		UGFxObject* immedObj	=QUI->GetObject(TEXT("quit_immed"));
		if(immedObj)
		{
			immedObj->SetString(TEXT("label"),TEXT("Return Now"));
		}
		UGFxObject* cancelObj	=QUI->GetObject(TEXT("quit_cancel"));
		if(cancelObj)
		{
			cancelObj->SetString(TEXT("label"),TEXT("Cancel Return"));
		}

		QUI->eventUCInitQuitTimer(TEXT("Returning to the login screen in "), 10, TEXT(" seconds"));
	}

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionMenu,FALSE, "");
}

void UMMOUIOptionMenu::ASShowTips_Option(class UGFxObject* mc,const FString& strName)
{
	if( mc == NULL ){
		return;
	}
	FString strDisplay =  FString::Printf(TEXT("MMOGame.Option.SettingsNameTips.%s"),*(strName));
	strDisplay = MMOUI::ParseLocalizedPropertyPath(strDisplay);

	mc->SetString( TEXT("tipsHtml"), strDisplay );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

void UMMOUIOptionMenu::ASHideTips_Option(class UGFxObject* mc)
{
	MMOUI::GetUIManager()->delegateHideTips();
}


void UMMOUIOptionMenu::ASShowMapLineListToChange()
{
	//FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	//FIntPoint tipPos = MMOFacade::GetMouseCursorPosition();
	//MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_ChangeMapLine, tipPos.X, tipPos.Y, 0, curMapInfo.MapDisplayName, 0 );
// 	UMMOUIOptionMenu* _OptionMenu =  Cast<UMMOUIOptionMenu>( MMOUI::GetUIManager()->FindUIPanel(FName(MMOUI::GetUIManager()->Panel_OptionMenu)) );
// 	if( _OptionMenu != NULL){
// 		_OptionMenu->ShowMapLineList();
// 	}

	ShowMapLineList();
}

void UMMOUIOptionMenu::ASChangeSceneLineByOptionMenu( int maplineCombinedID )
{
	//UMMOGFXPlayer_Minimap *pMinimapPlayer = (UMMOGFXPlayer_Minimap*)MMOUI::FindMoviePlayer( MMOUI::GetUIManager()->MoviePlayer_Minimap );
	//if( pMinimapPlayer == NULL ){
	//	return;
	//}
	UMMOUIMinimap *pMinimap = (UMMOUIMinimap*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Minimap );
	if( pMinimap == NULL ){
		return;
	}

	pMinimap->ASChangeSceneLine( maplineCombinedID );
}

//=============================================================
void UMMOUIOptionMenu::ASExitDuplicate()
{
 	FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
// 	UMMOUIOptionMenu* _OptionMenu =  Cast<UMMOUIOptionMenu>( MMOUI::GetUIManager()->FindUIPanel(FName(MMOUI::GetUIManager()->Panel_OptionMenu)) );
// 	if(_OptionMenu){
// 		switch( mapInfo.ServerMapType )
// 		{
// 		case E_est_BattleField: _OptionMenu->ReqExitDuplicate( E_est_BattleField ); break;
// 		case E_est_Dungeon: _OptionMenu->ReqExitDuplicate( E_est_Dungeon ); break;
// 		case E_est_GuildDungeon: _OptionMenu->ReqExitDuplicate( E_est_GuildDungeon ); break;
// 		case E_est_Arena: _OptionMenu->ReqExitDuplicate( E_est_Arena ); break;
// 		case E_est_Line: ASShowMapLineListToChange(); break;
// 		}
// 	}

	
	switch( mapInfo.ServerMapType )
	{
	case E_est_BattleField: ReqExitDuplicate( E_est_BattleField ); break;
	case E_est_Dungeon: ReqExitDuplicate( E_est_Dungeon ); break;
	case E_est_GuildDungeon: ReqExitDuplicate( E_est_GuildDungeon ); break;
	case E_est_Arena: ReqExitDuplicate( E_est_Arena ); break;
	case E_est_Line: ASShowMapLineListToChange(); break;
	}
}

void UMMOUIOptionMenu::ASOpenAssist()
{
	UMMOUIAssistPanel* assistPanel = Cast<UMMOUIAssistPanel>(MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Assist));
	if (assistPanel)
	{
		assistPanel->Show(TRUE);
	}
	Show( FALSE );
}