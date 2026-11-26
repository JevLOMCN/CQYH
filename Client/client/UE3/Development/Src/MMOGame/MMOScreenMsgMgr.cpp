#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOScreenMsgMgr.h"
#include "MMOFacade.h"
#include "MMOMapDataManager.h"
#include "CommonGameEvents.h"
//#include "MMOChatManager.h"
#include "MMOSkillManager.h"

//=======================================================================================
void MMOScreenMsgMgr::Init()
{
	//MMOUI::RegisterEventHandler( this, GameEvent_UnlockSkillSlot);
	//MMOUI::RegisterEventHandler( this, GameEvent_SkillUiUpdate);
	//MMOUI::RegisterEventHandler( this, GameEvent_SkillLevelUp);
	//MMOUI::RegisterEventHandler( this, GameEvent_FinishLoading);
	//MMOUI::RegisterEventHandler( this, GameEvent_EnterGameSuccess);
}
//=======================================================================================
void MMOScreenMsgMgr::Shutdown()
{
	//MMOUI::UnRegisterEventHandler( this, GameEvent_EnterGameSuccess);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_FinishLoading);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SkillLevelUp);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SkillUiUpdate);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_UnlockSkillSlot);
}
//=======================================================================================
void MMOScreenMsgMgr::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_UnlockSkillSlot:
		{
			MMOGameEvent_OneInt* newUnlockSlotEvt = (MMOGameEvent_OneInt*)evt;
			PushMsg( EScnMsgType_SkillSlot, newUnlockSlotEvt->IntData );
		}
		break;
	case GameEvent_SkillUiUpdate:
		{
			MMOGameEvent_SkillUiUpdate* skillEvent = (MMOGameEvent_SkillUiUpdate*)evt;
			switch(skillEvent->EventType)
			{
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillLearned:
				{
					MMOGameEvent_ProcessDynSkill* newSkillEvt = (MMOGameEvent_ProcessDynSkill*)evt;
					PushMsg( EScnMsgType_Skill, skillEvent->MagicInt );
				}
				break;
			}
		}
		break;
	case GameEvent_SkillLevelUp:
		{
			MMOGameEvent_SkillLevelUp *lvUpSkillEvt = (MMOGameEvent_SkillLevelUp*)evt;

			UINT skillUniqueID = MMOSkillManager::CombinateAbilityID( lvUpSkillEvt->SkillID, lvUpSkillEvt->NewLevel, 0 );
			PushMsg( EScnMsgType_MainSkillLevelUp, skillUniqueID );
		}
		break;
	case GameEvent_FinishLoading:
		{
			TryPlayMsg();
		}
		break;
	case GameEvent_EnterGameSuccess:
		{
			ForceClearMsg();
		}
		break;
	}
}

//
void MMOScreenMsgMgr::PushMsg( EScreenMsgType msgType, INT msgValue )
{
	ScreenMsg msg;
	msg._Type = msgType;
	msg._MsgValue = msgValue;
	_Messages.Push( msg );

	//
	if( _IsPlaying == FALSE ){
		TryPlayMsg();
	}
}

//
void MMOScreenMsgMgr::NotifyEndPlayMsg()
{
	if( _Messages.Num() > 0 ){
		_Messages.Remove( 0, 1 );
	}

	TryPlayMsg();
}

void MMOScreenMsgMgr::TryPlayMsg()
{
	while (TRUE)
	{
		UBOOL canPlay = (  MMO::GetCurrentGameState() == EGameState_InGame );
		if( !canPlay || _Messages.Num() <= 0 ){
			_IsPlaying = FALSE;
			return;
		}

		_IsPlaying = TRUE;

		const ScreenMsg msg = _Messages(0);
		switch( msg._Type )
		{
		case EScnMsgType_Skill:
		case EScnMsgType_SkillSlot:
		case EScnMsgType_MainSkillLevelUp:
			{
				UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
				if( pScreenPopUpBoxMsg == NULL ){
					_Messages.Remove(0, 1);
					continue;
				}
				pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsg( msg._MsgValue, msg._Type );
			}
			break;

		case EScnMsgType_Achievement:
			{
				UMMOUIAchievementHintPanel *pAchievementHint = (UMMOUIAchievementHintPanel*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_AchievementHint );
				if( pAchievementHint == NULL ){
					_Messages.Remove(0, 1);
					continue;
				}
				pAchievementHint->ShowAchievementHintPanel( msg._MsgValue );
			}
			break;
		case EScnMsgType_FBAchievement:
			{

			}
			break;
		case EScnMsgType_FBUnlock:
			{
				//INT curFbMapID = msg._MsgValue;
				//FExportMap mapExportInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( curFbMapID );
				//if( mapExportInfo.ServerMapType != E_est_Dungeon ){
				//	break;
				//}

				//INT curFBDiff = MMOMapDataManager::getSingleton().GetCurFBDiff();

				//INT curMaxDiff = MMOMapDataManager::getSingleton().GetCurFBMaxDiff();
				//INT fbOpened1PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( curFbMapID, EFBDTyp_OnePerson, curMaxDiff );
				//INT fbOpened5PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( curFbMapID, EFBDTyp_FivePerson, curMaxDiff );

				//INT nextDifficulty = esdInvalid;
				//if( curFBDiff >= esdNormal1 && curFBDiff <= esdHeroic1 && fbOpened1PersonDiffMax < esdHeroic1 ){
				//	//1 person FB
				//	nextDifficulty = fbOpened1PersonDiffMax + 1;
				//}
				//else if( curFBDiff >= esdNormal5 && curFBDiff <= esdHeroic5 && fbOpened5PersonDiffMax < esdHeroic5 ){
				//	//5 person FB
				//	nextDifficulty = fbOpened5PersonDiffMax + 1;
				//}

				//INT iMaxDiffMask = MMOMapDataManager::getSingleton().GetDungeonInfoDifftyMask( curFbMapID );
				//INT intMask = nextDifficulty - esdNormal1;
				//if( nextDifficulty == esdInvalid || ( ( iMaxDiffMask & ( 1 << intMask ) ) == 0 ) ){
				//	break;
				//}

				////FString hintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBDiff.FBUnlockHint1") );

				//FString difficultyNameKey = TEXT("MMOGame.FBDiff.FBDiff");
				//difficultyNameKey += appItoa( nextDifficulty - ENUM_SCENE_DIFFICULTY::esdNormal1 );
				//FString strContent = MMOUI::ParseLocalizedPropertyPath( *difficultyNameKey );

				//FString strTitle = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBDiff.FBUnlockHintScreenTitle") ), *mapExportInfo.MapDisplayName );
			}
			break;
		}
		return;
	}
}

//
void MMOScreenMsgMgr::ForceClearMsg()
{
	const TArray<ScreenMsg> messagesToClear = _Messages;
	_Messages.Empty();
	_IsPlaying = FALSE;
	INT msgCnt = messagesToClear.Num();
	for( INT i = 0; i < msgCnt; ++i ){
		const ScreenMsg &msg = messagesToClear(i);
		switch( msg._Type )
		{
		case EScnMsgType_Skill:
		case EScnMsgType_SkillSlot:
		case EScnMsgType_MainSkillLevelUp:
		case EScnMsgType_FBUnlock:
			{
				UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
				if( pScreenPopUpBoxMsg != NULL ){
					pScreenPopUpBoxMsg->Show( FALSE );
				}
			}
			break;

		case EScnMsgType_Achievement:
			{
				UMMOUIAchievementHintPanel *pAchievementHint = (UMMOUIAchievementHintPanel*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_AchievementHint );
				if( pAchievementHint != NULL ){
					pAchievementHint->Show( FALSE );
				}
			}
			break;
		}
	}

}
