#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "net/message_handler.h"
#include "MMOMsgBoxManager.h"
#include "TeamManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Option)
// 
// void UMMOGFXPlayer_Option::ASShowMapLineListToChange()
// {
// 	//FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
// 	//FIntPoint tipPos = MMOFacade::GetMouseCursorPosition();
// 	//MMOUI::GetUIManager()->delegateOpenRClickMenu( ERCMType_ChangeMapLine, tipPos.X, tipPos.Y, 0, curMapInfo.MapDisplayName, 0 );
// 	UMMOUIOptionMenu* _OptionMenu =  Cast<UMMOUIOptionMenu>( MMOUI::GetUIManager()->FindUIPanel(FName(MMOUI::GetUIManager()->Panel_OptionMenu)) );
// 	if( _OptionMenu != NULL){
// 		_OptionMenu->ShowMapLineList();
// 	}
// }
// 
// void UMMOGFXPlayer_Option::ASChangeSceneLine( int maplineCombinedID )
// {
// 	UMMOGFXPlayer_Minimap *pMinimapPlayer = (UMMOGFXPlayer_Minimap*)MMOUI::FindMoviePlayer( MMOUI::GetUIManager()->MoviePlayer_Minimap );
// 	if( pMinimapPlayer == NULL ){
// 		return;
// 	}
// 
// 	pMinimapPlayer->ASChangeSceneLine( maplineCombinedID );
// }
// 
// //=============================================================
// void UMMOGFXPlayer_Option::ASExitDuplicate()
// {
//     FExportMap mapInfo = MMOMapDataManager::getSingleton().GetCurMap();
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
// }

void UMMOGFXPlayer_Option::Init()
{
	Super::Init();
	check(FALSE && "UMMOGFXPlayer_Option::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Option);

// 	MMOUI::GetUIManager()->SetShowKeyWarningDelegate(FScriptDelegate(this, TEXT("ShowKeyWarning")));

	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionConfig,FALSE,"");
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionQuit,FALSE,"");
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionMenu,FALSE,"");
}

void UMMOGFXPlayer_Option::UnInit()
{
	Super::UnInit();

// 	FScriptDelegate nullDelgate(EC_EventParm);
// 	MMOUI::GetUIManager()->SetShowKeyWarningDelegate(nullDelgate);

}

// void UMMOGFXPlayer_Option::NativeReturnLogin()
// {
// 	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionQuit,TRUE, "");
// 	UMMOUIOptionQuit* QUI =  Cast<UMMOUIOptionQuit>(FindUIPanel(MMOUI::GetUIManager()->Panel_OptionQuit));
// 	//QUI->eventUCInitQuitTimer("Will auto-exit the game in ", 20, " seconds...");
// 	if(QUI){
// 		UGFxObject* immedObj	=QUI->GetObject(TEXT("quit_immed"));
// 		if(immedObj)
// 		{
// 			immedObj->SetString(TEXT("label"),TEXT("Return now"));
// 		}
// 		UGFxObject* cancelObj	=QUI->GetObject(TEXT("quit_cancel"));
// 		if(cancelObj)
// 		{
// 			cancelObj->SetString(TEXT("label"),TEXT("Cancel return"));
// 		}
// 
// 		QUI->eventUCInitQuitTimer(TEXT("Will in"), 10, TEXT("seconds return to login screen"));
// 	}
// 
// 	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionMenu,FALSE, "");
// 
// }

// void UMMOGFXPlayer_Option::ASOptionQuitImmed()
// {
// 	if( _ReturnType==1 )
// 	{
// 		MMOUI::GetUIManager()->ImmReturnLogin();
// 		//class'MMOUIManager'.static.ShowPanelEx(class'MMOUIManager'.default.Panel_OptionMenu,false, "");
// 	}
// 	else
// 	{
// 		MMOUI::GetUIManager()->ImmQuitGame();
// 	}
// 
// 	if( TeamManager::getSingleton().HasTeam() )
// 	{
// 		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
// 		if(player)
// 		{
// 			CMessageHandler::GetInstance().Send_TeamLeave( player->ServerID );
// 		}		
// 	}
// // }
// void UMMOGFXPlayer_Option::ASShowTips_Option(class UGFxObject* mc,const FString& strName)
// {
// 	if( mc == NULL ){
// 		return;
// 	}
// 	FString strDisplay =  FString::Printf(TEXT("MMOGame.Option.SettingsNameTips.%s"),*(strName));
// 	strDisplay = MMOUI::ParseLocalizedPropertyPath(strDisplay);
// 
// 	mc->SetString( TEXT("tipsHtml"), strDisplay );
// 	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
// }
// void UMMOGFXPlayer_Option::ASHideTips_Option(class UGFxObject* mc)
// {
// 	MMOUI::GetUIManager()->delegateHideTips();
// }