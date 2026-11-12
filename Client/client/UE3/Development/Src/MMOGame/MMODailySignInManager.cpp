#include "stdafx.h"
#include "MMODailySignInManager.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "MMOMsgButtonManager.h"
#include "tabfile.h"
#include "MMODataManager.h"

#define ROLE_LEVEL_MAX (99999)
//const static INT MMODailySignInManager::SIGN_IN_LV_REQUIRE = 7;
//==========================================================================================================================
MMODailySignInManager::MMODailySignInManager()
{
	_MyCurSignInState._CurSignInDay = 0;
	_MyCurSignInState._CanSignIn = FALSE;
	_MyCurSignInState._SignInTimestamp = 0;

	_HintSignInMsgButtonIdx = -1;

	_RequipedSignInRoleLevel = 0;
}

//==========================================================================================================================
MMODailySignInManager::~MMODailySignInManager()
{
}

//==========================================================================================================================
void MMODailySignInManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_SyncSignInDay:
		{
			MMOGameEvent_DailySignIn *syncDailySignInEvt = (MMOGameEvent_DailySignIn*)evt;
			INT curSignInDay = syncDailySignInEvt->LastSignInDay;
			if( syncDailySignInEvt->CanCurSigneIn ){
				curSignInDay += 1;
				for( ; curSignInDay <= MMODailySignInManager::SIGN_IN_TOTAL; ++curSignInDay ){
					const SSignInReward * rewardData = GetSignInRewardAtDay( curSignInDay );
					if( rewardData != NULL ){
						break;
					}
				}
			}

			//
			SetMyCurSignInState( curSignInDay, syncDailySignInEvt->CanCurSigneIn, syncDailySignInEvt->IsToBeValid );
		}
		break;
	case GameEvent_SignedInToday:
		{
			const SSignInRewardState &curSignInState = GetMyCurSignInState();
			SetMyCurSignInState( curSignInState._CurSignInDay, FALSE );
			//
			UMMOUIDailySignIn *dailySignInPanel = (UMMOUIDailySignIn*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_DailySignIn );
			if( dailySignInPanel != NULL ){
				dailySignInPanel->NotifyGetDailySignInRewardResult( curSignInState._CurSignInDay );
			}
		}
		break;
	//case GameEvent_SyncGameLeftTime:
	//	{
	//		AutoRefreshSignIn();
	//	}
	//	break;
	case GameEvent_EnterSelectingRole:
		{
			_IsLastInRoleSelecting = TRUE;

			ClearData();
		}
		break;
	case GameEVent_SceneLoadFinish:
		{
			if( _IsLastInRoleSelecting && MMO::GetCurrentGameState() == EGameState_InGame ){
				CheckHintForSignIn();
				_IsLastInRoleSelecting = FALSE;
			}
		}
		break;
	}
}
//
void MMODailySignInManager::ClearData()
{
	_MyCurSignInState._CurSignInDay = 0;
	_MyCurSignInState._CanSignIn = FALSE;
	_MyCurSignInState._SignInTimestamp = 0;
	_MyCurSignInState._IsValid = TRUE;
}
//
void MMODailySignInManager::SetMyCurSignInState( INT dayIdx, UBOOL canSignIn, UBOOL isToValid/* = TRUE*/ )
{
	_MyCurSignInState._CurSignInDay = dayIdx;

	if( _MyCurSignInState._CanSignIn && !canSignIn ){
		//today signed in
		_MyCurSignInState._SignInTimestamp = MMOFacade::getSingleton().GetRealClientUTCTime();
	}
	_MyCurSignInState._CanSignIn = ( canSignIn && isToValid );
	_MyCurSignInState._IsValid = isToValid;

	UMMOUIMenuBar* menuBarUI = (UMMOUIMenuBar*)( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Skillbar_Menubar ) );
	if( menuBarUI != NULL ){
		FMenuInfo* menuBarMenuInfo = menuBarUI->FindMenuInfoByType( EMenuType_DailySignIn, EMenuType_DailySignIn );
		if( menuBarMenuInfo != NULL ){
			if( !isToValid && menuBarMenuInfo->RequireLevel != ROLE_LEVEL_MAX ){
				_RequipedSignInRoleLevel = menuBarMenuInfo->RequireLevel;
				menuBarMenuInfo->RequireLevel = ROLE_LEVEL_MAX;

				menuBarUI->InitMenuSlot();
				return;
			}
			else if( isToValid && menuBarMenuInfo->RequireLevel == ROLE_LEVEL_MAX ){
				menuBarMenuInfo->RequireLevel = _RequipedSignInRoleLevel;

				menuBarUI->InitMenuSlot();
			}
		}
	}

	if( MMO::GetCurrentGameState() == EGameState_InGame ){
		CheckHintForSignIn();
	}
}
//==========================================================================================================================
void MMODailySignInManager::AutoRefreshSignIn()
{
	if( !_MyCurSignInState._IsValid ){
		return;
	}

	if( !_MyCurSignInState._CanSignIn ){
		return;
	}

	if( _MyCurSignInState._SignInTimestamp == 0 ){
		return;
	}

	SYSTEMTIME signedInTime = MMOFacade::getSingleton().ConvertUtcSecondToSystemTime( _MyCurSignInState._SignInTimestamp );

	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);

	if( curTime.wDay - signedInTime.wDay >= 1 && curTime.wMonth != signedInTime.wMonth ){
		//At least a day apart
		CheckHintForSignIn();
	}
}
//==========================================================================================================================
void MMODailySignInManager::CheckHintForSignIn()
{
	if( _MyCurSignInState._CanSignIn && /*_MyCurSignInState._IsValid &&*/
		_MyCurSignInState._CurSignInDay > 0 && _MyCurSignInState._CurSignInDay <= SIGN_IN_TOTAL ){
		//
		AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if( mainPlayer && mainPlayer->GetActorLevel() >= SIGN_IN_LV_REQUIRE ){
			if( _HintSignInMsgButtonIdx < 0 ){
				MMOMsgButtonManager_Parms Parms(EC_EventParm);
				_HintSignInMsgButtonIdx = MMOMsgButtonManager::getSingleton().AddNewButton( _OpenDailySignInPanelDelegate, Parms, E_MsgBtnShow_SignIn );
			}

			//
			MMOGameEvent_OneInt newSignInEvt( GameEvent_NewSignedIn, _MyCurSignInState._CurSignInDay );
			MMOUI::PostGameEvent( &newSignInEvt );
		}
	}
}
//
void MMODailySignInManager::RemoveHintForSignInIf()
{
	if( IsCurShowingHintForSignIn() ){
		//MMOMsgButtonManager::getSingleton().RemoveButton( _HintSignInMsgButtonIdx );
		MMOMsgButtonManager::getSingleton().RemoveOneTypeButtons( E_MsgBtnShow_SignIn );
		//
		UMMOUIMenuBar *menuBarPanel = (UMMOUIMenuBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar_Menubar );
		if( menuBarPanel != NULL ){
			menuBarPanel->ShowMenuBarHint( EMenuType_DailySignIn, EMenuType_Child_DailySignInGetReward, FALSE );
		}
	}
	_HintSignInMsgButtonIdx = -1;
}
//
void MMODailySignInManager::SetOpenDailySignInPanelDelegate( FScriptDelegate fun )
{
	_OpenDailySignInPanelDelegate = fun;
}
//==========================================================================================================================
UBOOL MMODailySignInManager::ReadSignInRewardConfig()
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	CTabFile* tabFile = NULL;
	if( !MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("checkin.txt"), TRUE ) ){
		return FALSE;
	}

	_SignInRewardsCfg.Empty();

	UINT rowCnt = tabFile->GetRowCount();
	UINT startIdx = M_SKIP_ROWS;
	//Actual data starts from row 3; header row, N/A row
	//_SignInRewardsCfg.Add( rowCnt > startIdx ? (rowCnt - startIdx) : 0 );
	for( UINT i = startIdx ; i < rowCnt; ++i )
	{
		INT signInDay = appAtoi( tabFile->GetData( TEXT("Day"), i ) );
		SSignInReward &signInData = _SignInRewardsCfg.FindOrAdd( signInDay );

		signInData._DayRequire = signInDay;
		signInData._ItemID = appAtoi( tabFile->GetData( TEXT("AwardItemID"), i ) );
		signInData._ItemCnt = appAtoi( tabFile->GetData( TEXT("AwardItemCount"), i ) );
		signInData._LevelRequire = appAtoi( tabFile->GetData( TEXT("LevelLimit"), i ) );
	}

	delete tabFile;
	return TRUE; 
}
//==========================================================================================================================
void MMODailySignInManager::Initialize()
{
	MMOUI::RegisterEventHandler( this, GameEvent_SignedInToday );
	MMOUI::RegisterEventHandler( this, GameEvent_SyncSignInDay );
	MMOUI::RegisterEventHandler( this, GameEVent_SceneLoadFinish );
	MMOUI::RegisterEventHandler( this, GameEvent_EnterSelectingRole );
	MMOUI::RegisterEventHandler( this, GameEvent_SyncGameLeftTime);
	//
	ReadSignInRewardConfig();
}

//==========================================================================================================================
void MMODailySignInManager::Shutdown()
{
	FScriptDelegate nullDelgate(EC_EventParm);
	SetOpenDailySignInPanelDelegate( nullDelgate );

	MMOUI::UnRegisterEventHandler( this, GameEvent_SyncGameLeftTime);
	MMOUI::UnRegisterEventHandler( this, GameEvent_EnterSelectingRole );
	MMOUI::UnRegisterEventHandler( this, GameEVent_SceneLoadFinish );
	MMOUI::UnRegisterEventHandler( this, GameEvent_SyncSignInDay );
	MMOUI::UnRegisterEventHandler( this, GameEvent_SignedInToday );
}

//==========================================================================================================================
void MMODailySignInManager::Clear()
{

}
