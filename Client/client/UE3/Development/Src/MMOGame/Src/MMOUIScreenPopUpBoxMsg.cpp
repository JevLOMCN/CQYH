#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
//#include "MMOScreenMsgMgr.h"
#include "ScreenMsgQueue.h"
#include "MMOSkillManager.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIScreenPopUpBoxMsg)

//
void UMMOUIScreenPopUpBoxMsg::PreOnShow(UBOOL v)
{
	Super::PreOnShow( v );

	if( v ){
		FVector2D scrSz = MMOFacade::GetScreenSize();

		//INT thisWidth = eventGetWidth();
		//INT thisHeight = eventGetHeight();

		//INT panelX = (scrSz.X - thisWidth) / 2;
		//INT panelY = (scrSz.Y - Max(leftH, rightH)) / 2;

		INT panelX = scrSz.X / 2;
		FLOAT curPosX;
		FLOAT curPosY;
		GetPosition( curPosX, curPosY );

		SetPosition( panelX, curPosY );
	}
	
}

//================================================================================================================
void UMMOUIScreenPopUpBoxMsg::PostOnShow(UBOOL v)
{
	//To avoid taking focus
}

//================================================================================================================
//Play gained ability hint
void UMMOUIScreenPopUpBoxMsg::PlayScreenPopUpBoxMsg( int msgId , BYTE type )
{
    TArray<struct FASValue> asArguments;

	FASValue asShowType(EC_EventParm);
	asShowType.Type = AS_Int;
	asShowType.I = type;

    FASValue asTitle(EC_EventParm);
	asTitle.Type = AS_String;

	FASValue asContent(EC_EventParm);
	asContent.Type = AS_String;

	FASValue asIconPath(EC_EventParm);
	asIconPath.Type = AS_String;

	FASValue asExtraString(EC_EventParm);
	asExtraString.Type = AS_String;

	//
	UBOOL isOK = TRUE;
    switch( type )
    {
	case EScnMsgType_Skill:
        {
			isOK = FillUiDataForSkillLearned( msgId, asTitle.S, asContent.S, asIconPath.S, asExtraString.S ); 
        }
		break;
	case EScnMsgType_MainSkillLevelUp:
		{
			isOK = FillUiDataForSkillLevelUp( msgId, asTitle.S, asContent.S, asIconPath.S, asExtraString.S ); 
		}
		break;

	case EScnMsgType_SkillSlot:
		{
			isOK = FillUiDataForSkillSlotUnlock( msgId, asTitle.S, asContent.S, asIconPath.S, asExtraString.S ); 
		}
		break;
	case EScnMsgType_FBAchievement:
		{
			isOK = FillUiDataForFBAchievements( msgId, asTitle.S, asContent.S, asIconPath.S, asExtraString.S ); 
		}
		break;
	case EScnMsgType_FBUnlock:
		{
			isOK = FillUiDataForFBDifficultyUnlock( msgId, asTitle.S, asContent.S, asIconPath.S, asExtraString.S ); 
		}
		break;
    }


    //Decide whether to add and play this message
    if( isOK )
    {
		asArguments.AddItem(asShowType);
		asArguments.AddItem(asTitle);
		asArguments.AddItem(asContent);
		asArguments.AddItem(asIconPath);
		asArguments.AddItem(asExtraString);
		Invoke( TEXT("UCSetMsgInfo") , asArguments );

		Show( TRUE );
    }
    else
	{
		Show( FALSE );

		ScreenMessageQueue::getSingleton().NotifyEndPlayMsg();
    }
  
}
//-----------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIScreenPopUpBoxMsg::FillUiDataForSkillLearned( INT uniqueSkillID, FString &asTitle, FString &asContent, FString &asIconPath, FString &asExtraString )
{
	UBOOL isOK = TRUE;
	asExtraString = TEXT("");

	UMMOAbility* ability = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( ability != NULL && ability->MainSkill != NULL ){
		ability = ability->MainSkill;
	}

	if ( ability )
	{
		if( E_spct_Active == ability->SpellCommonAttribute.CastType )
		{
			if( E_spqte_None == ability->SpellCommonAttribute.QTEType )
				asTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ActiveSkillLearned" ));
			else
				asTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AvartaSkillLearned" ));
		}
		else if( E_spct_Passive == ability->SpellCommonAttribute.CastType  )
		{
			asTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PassiveSkillLearned" ));
		}
		else
		{
			isOK = FALSE;
		}

		if( !isOK )
		{
			asContent = ability->eventGetAbilityName();
			asIconPath = ability->GetAbilityIconName();
		}
	}
	else
	{
		isOK = FALSE;
	}

	return isOK;
}
//-----------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIScreenPopUpBoxMsg::FillUiDataForSkillLevelUp( INT uniqueSkillID, FString &asTitle, FString &asContent, FString &asIconPath, FString &asExtraString )
{
	UBOOL isOK = TRUE;
	asExtraString = TEXT("");

	UINT skillUniqueID = uniqueSkillID;
	UMMOAbility* ability = MMOSkillManager::getSingleton().FindAbilityTemplate( skillUniqueID );
	if( ability != NULL ){
		FString abilityName = ability->eventGetAbilityName();
		asTitle = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillLvUpNotifyScreenTitle") ), *abilityName );

		INT skllNewLevel = ability->SpellCommonAttribute.Level;
		asContent = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillLvUpNotifyScreenText") ), skllNewLevel );

		asIconPath= ability->GetAbilityIconName();
	}
	else
	{
		isOK = FALSE;
	}

	return isOK;
}
//-----------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIScreenPopUpBoxMsg::FillUiDataForSkillSlotUnlock( INT slotIdx, FString &asTitle, FString &asContent, FString &asIconPath, FString &asExtraString )
{
	UBOOL isOK = TRUE;

	asTitle= MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SlotUnlock" ));

	//if( slotIdx >= ECK_QTE1 && slotIdx <= ECK_QTE3 )
	if( slotIdx == ECK_QTE1 )
	{
		asContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AvartaSlotUnlock" ));
	}
	else if( slotIdx >= ECK_Passive1 && slotIdx <= ECK_Passive3 )
	{
		asContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PassiveSlotUnlock" ));
	}
	else if( slotIdx >= ECK_Slot1 && slotIdx <= ECK_Slot6 )
	{
		asContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ActiveSlotUnlock" ));
		FString strLookUp = TEXT("MMOGame.GamePlay.SkillSlot" );
		strLookUp += appItoa( slotIdx );
		asExtraString = MMOUI::ParseLocalizedPropertyPath( strLookUp );
	}
	else
	{
		isOK = FALSE;
	}

	asIconPath  = TEXT("");

	//
	return isOK;
}

//-----------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUIScreenPopUpBoxMsg::FillUiDataForFBAchievements( INT fbAchieveID, FString &asTitle, FString &asContent, FString &asIconPath, FString &asExtraString )
{
	FBAchievementsState *curFBAchievement = MMOMapDataManager::getSingleton().FindFBAchievementCurrentDungeon( fbAchieveID );
	if( curFBAchievement == NULL ){
		return FALSE;
	}

	//
	const FDungeonTarget *thisTemplate = curFBAchievement->_achievementTemplate;
	if( thisTemplate == NULL ){
		return FALSE;
	}

	asTitle = thisTemplate->TargetName;
	asContent = TEXT(""); 
	asIconPath = TEXT("");
	asExtraString = TEXT("");

	return TRUE;
}

//==========================================================================================================================================================================
UBOOL UMMOUIScreenPopUpBoxMsg::FillUiDataForFBDifficultyUnlock( INT curFbMapID, FString &asTitle, FString &asContent, FString &asIconPath, FString &asExtraString )
{
	//FExportMap mapExportInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( curFbMapID );

	//if( mapExportInfo.ServerMapType != E_est_Dungeon ){
	//	return FALSE;
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
	//	return FALSE;
	//}

	////FString hintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBDiff.FBUnlockHint1") );

	//FString difficultyNameKey = TEXT("MMOGame.FBDiff.FBDiff");
	//difficultyNameKey += appItoa( nextDifficulty - ENUM_SCENE_DIFFICULTY::esdNormal1 );
	//asContent = MMOUI::ParseLocalizedPropertyPath( *difficultyNameKey );

	//asTitle = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBDiff.FBUnlockHintScreenTitle") ), *mapExportInfo.MapDisplayName );
	//
	//asIconPath = TEXT("");
	//asExtraString = TEXT("");

	return TRUE;
}

//==========================================================================================================================================================================
void UMMOUIScreenPopUpBoxMsg::PlayScreenPopUpBoxMsgByString( BYTE type, const FString &title, const FString &content, const FString &iconPath, const FString &extraString )
{
	TArray<struct FASValue> asArguments;

	FASValue asShowType(EC_EventParm);
	asShowType.Type = AS_Int;
	asShowType.I = type;

	FASValue asTitle(EC_EventParm);
	asTitle.Type = AS_String;
	asTitle.S = title;

	FASValue asContent(EC_EventParm);
	asContent.Type = AS_String;
	asContent.S = content;

	FASValue asIconPath(EC_EventParm);
	asIconPath.Type = AS_String;
	asIconPath.S = iconPath;

	FASValue asExtraString(EC_EventParm);
	asExtraString.Type = AS_String;
	asExtraString.S = extraString;

	asArguments.AddItem(asShowType);
	asArguments.AddItem(asTitle);
	asArguments.AddItem(asContent);
	asArguments.AddItem(asIconPath);
	asArguments.AddItem(asExtraString);
	Invoke( TEXT("UCSetMsgInfo") , asArguments );

	Show( TRUE );
}

//Gained ability hint playback finished notification
void UMMOUIScreenPopUpBoxMsg::MsgPlayEnd()
{
	Show( FALSE );

	ScreenMessageQueue::getSingleton().NotifyEndPlayMsg();
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIScreenPopUpBoxMsg::ASScreenPopUpBoxMsgPlayEnd()
{
	MsgPlayEnd();
}
//===============================================================================================================
//===============================================================================================================
UBOOL UMMOUIScreenPopUpBoxMsg::SkipCaptureInputKey( const FString& key, BYTE eIE, const FString& strHoverControllerName, FVector2D ClickPosInMovieSpace )
{
	if( IsVisible )
	{
		if( strHoverControllerName == TEXT("fightbg_mc") )
		{
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey( key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}