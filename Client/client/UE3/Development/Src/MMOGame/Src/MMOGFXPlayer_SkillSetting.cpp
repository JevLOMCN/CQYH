#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOSkillManager.h"
#include "UITextParser.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_SkillSetting)

extern bool g_displayDragDropIcon;

//void UMMOGFXPlayer_SkillSetting::PostAdvance(float DeltaTime)
//{
//	//if( _SkillSetting->IsVisible ){
//	//	_SkillSetting->PostAdvance( DeltaTime );
//	//}
//}

//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::ASLevelUpSkill( int uniqueSkillID )
{
	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_SkillLevelUp);
	MMOUI::PostGameEvent(&evtGuide);

	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer && mainPlayer->ActorAttributes->LevelAttributes.Level < SkillLevelUpLimitLv ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ERROR_skill_chara_low_level") ),MsgInfoType_Table,TEXT("MMOGame.SkillTree.ERROR_skill_chara_low_level") );
		return;
	}

	//
	UMMOAbility *skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( skill == NULL ){
		return;
	}
	UINT skillID = 0;
	UINT skillLv = 0;
	UINT skillRuneID = 0;
	MMOSkillManager::DecomposeAbilityID( uniqueSkillID, skillID, skillLv, skillRuneID );

	UMMOAbility *skillNextLevel = MMOSkillManager::getSingleton().FindAbilityTemplate( MMOSkillManager::CombinateAbilityID( skillID, skillLv+1, skillRuneID ) );
	if( skillNextLevel == NULL ){
		return;
	}
	//not enough cultivation points
	INT levelUpPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
	if( levelUpPts < skillNextLevel->LvUpRequirePoints ){
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ERROR_skill_level_up_needPts") ),MsgInfoType_Table,TEXT("MMOGame.SkillTree.ERROR_skill_level_up_needPts") );
		return;
	}
	//hide Tips
	MMOUI::GetUIManager()->delegateHideTips();

	CMessageHandler::GetInstance().Send_SpellLevelUp( (UINT16)skillID );
}

//
void UMMOGFXPlayer_SkillSetting::ASShowTips_SkillLvUpBtn( UGFxObject *mc, INT uniqueSkillID )
{
	if( mc == NULL ){
		return;
	}

	FString hint;
	//hint = UITextParser::HTMLBeginParagraph();
	//hint += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillLvUpHint") );
	//hint += UITextParser::HTMLEndParagraph();
	//cultivation points
	UMMOAbility *skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( skill == NULL ){
		return;
	}
	UMMOAbility *skillNextLevel = MMOSkillManager::getSingleton().GetAbilityByNextLevel( skill );
	if( skillNextLevel == NULL ){
		return;
	}
	//
	INT levelUpPts = skillNextLevel->LvUpRequirePoints;
	INT curLevelUpPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
	UBOOL isPtsLow = ( curLevelUpPts < levelUpPts );

	hint += UITextParser::HTMLBeginParagraph();
	if( isPtsLow ){
		hint += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")));
	}

	hint += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkill_level_up_needPts") ), levelUpPts );

	if( isPtsLow ){
		hint += UITextParser::HTMLEndFont();
	}
	hint += UITextParser::HTMLEndParagraph();

	//character level requirement tip
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	INT limitedLevel = ( SkillLevelUpLimitLv >= skillNextLevel->LvUpRequireRoleLevel ? SkillLevelUpLimitLv : skillNextLevel->LvUpRequireRoleLevel );
	if( mainPlayer && mainPlayer->ActorAttributes->LevelAttributes.Level < limitedLevel ){
		hint += UITextParser::HTMLBeginParagraph();
		hint += UITextParser::HTMLBeginFont( UITextParser::GetTipsColor(TEXT("AlertColor")));
		hint += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.SkillTree.ActiveSkillLvUpLimit") ), limitedLevel );
		hint += UITextParser::HTMLEndFont();
		hint += UITextParser::HTMLEndParagraph();
	}

	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

//
void UMMOGFXPlayer_SkillSetting::ASHideTips_SkillLvUpBtn()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::ASShowTips_SkillTreeActiveSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOGFXPlayer_SkillSetting::ASHideTips_SkillTreeActiveSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::ASShowTips_SkillTreeRuneSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOGFXPlayer_SkillSetting::ASHideTips_SkillTreeRuneSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::ASSetSkill( INT skillSetIdx, INT uniqueSkillID )
{
	UMMOUISkillBar *skillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Skillbar );
	if( skillBarPanel != NULL ){
		INT curCmdKeyInSkillBarIdx = MMOSkillManager::getSingleton().GetSkillbarSlotIdxByKeyCmd( skillSetIdx );
		skillBarPanel->TryToEquipOnSkill( curCmdKeyInSkillBarIdx, uniqueSkillID );
	}
}
//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::ASShowSkillDetail( UGFxObject *mc, INT skillUniqueID, INT showPosX, INT showPosY )
{
	//not needed
	//if( _SkillSetting == NULL ){
	//	return;
	//}

	//_SkillSetting->ShowActiveSkillDetailOf( mc, skillUniqueID, showPosX, showPosY );
}

//
void UMMOGFXPlayer_SkillSetting::ASHideActiveDetailSkillsListPanel()
{
	if( _SkillSetting == NULL ){
		return;
	}
	_SkillSetting->HideActiveSkillDetail();
}
//
void UMMOGFXPlayer_SkillSetting::ASOnScroolingActiveSkillList()
{
	if( _SkillSetting == NULL ){
		return;
	}
	_SkillSetting->HideActiveSkillDetail();
}
//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::ASShowLearnedActiveSkills()
{
	if( _SkillSetting == NULL ){
		return;
	}

	_SkillSetting->ShowLearnedActiveSkills();
}

void UMMOGFXPlayer_SkillSetting::ASShowAllActiveSkills()
{
	if( _SkillSetting == NULL ){
		return;
	}

	_SkillSetting->ShowAllActiveSkills();
}

void UMMOGFXPlayer_SkillSetting::ASShowSkillSetting()
{
	if( _SkillSetting != NULL && _SkillSetting->ActiveSkillsDetailListPanel != NULL ){
		_SkillSetting->HideActiveSkillDetail();
	}

	//
	UMMOUISkillSettingPanel *skillSettingPanel = (UMMOUISkillSettingPanel*)MMOUI::GetUIManager()->FindUIPanel( MMOUI::GetUIManager()->Panel_SkillSetting );
	if( skillSettingPanel == NULL ){
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_SkillSetting,TRUE, "");
	}
	else{
		skillSettingPanel->Show( TRUE );
	}

}
void UMMOGFXPlayer_SkillSetting::ASClickTab(int nIndex)
{
	if(_SkillSetting)
		_SkillSetting->UpdatePanel(nIndex);
}

void UMMOGFXPlayer_SkillSetting::ASStartDrag(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	Super::ASStartDrag( SrcItemSlot, SrcSlotType, SrcContainerID, SrcSlotID );

	//
	switch( SrcSlotType )
	{
	case ESPType_SkillSetting:
		{
			UMMOUISkillBar *pSkillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_Skillbar );
			if( pSkillBarPanel == NULL ){
				break;
			}

			pSkillBarPanel->HintForDraggingItem( ESPType_SkillSetting, 0 );
		}
		break;
	}
}
//============================================================================================================================
void UMMOGFXPlayer_SkillSetting::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_SkillSetting::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_SkillSetting );
}

void UMMOGFXPlayer_SkillSetting::UnInit()
{
	Super::UnInit();

}
