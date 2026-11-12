#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOSkillManager.h"
#include "UITextParser.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_SkillTree)

	extern bool g_displayDragDropIcon;

void UMMOGFXPlayer_SkillTree::PostAdvance(float DeltaTime)
{
	//if( _SkillSetting->IsVisible ){
	//	_SkillSetting->PostAdvance( DeltaTime );
	//}
}

//============================================================================================================================
void UMMOGFXPlayer_SkillTree::ASLevelUpSkill( int uniqueSkillID )
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
void UMMOGFXPlayer_SkillTree::ASShowTips_SkillLvUpBtn( UGFxObject *mc, INT uniqueSkillID )
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
void UMMOGFXPlayer_SkillTree::ASHideTips_SkillLvUpBtn()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//============================================================================================================================
void UMMOGFXPlayer_SkillTree::ASShowTips_SkillTreeActiveSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOGFXPlayer_SkillTree::ASHideTips_SkillTreeActiveSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================================
void UMMOGFXPlayer_SkillTree::ASShowTips_SkillTreeRuneSkill( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateShowTipsHTML(TFT_SkillTemplateWithNext, mc);
}

//
void UMMOGFXPlayer_SkillTree::ASHideTips_SkillTreeRuneSkill()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//============================================================================================================================
void UMMOGFXPlayer_SkillTree::ASSetSkill( INT skillSetIdx, INT uniqueSkillID )
{
	UMMOUISkillBar *skillBarPanel = (UMMOUISkillBar*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Skillbar );
	if( skillBarPanel != NULL ){
		INT curCmdKeyInSkillBarIdx = MMOSkillManager::getSingleton().GetSkillbarSlotIdxByKeyCmd( skillSetIdx );
		skillBarPanel->TryToEquipOnSkill( curCmdKeyInSkillBarIdx, uniqueSkillID );
	}
}
//============================================================================================================================
void UMMOGFXPlayer_SkillTree::ASShowSkillDetail( UGFxObject *mc, INT skillUniqueID, INT showPosX, INT showPosY )
{
	//not needed
	//if( _SkillTree == NULL ){
	//	return;
	//}

	//_SkillTree->ShowActiveSkillDetailOf( mc, skillUniqueID, showPosX, showPosY );
}

//
void UMMOGFXPlayer_SkillTree::ASHideActiveDetailSkillsListPanel()
{
	if( _SkillTree == NULL ){
		return;
	}
	_SkillTree->HideActiveSkillDetail();
}
//
void UMMOGFXPlayer_SkillTree::ASOnScroolingActiveSkillList()
{
	if( _SkillTree == NULL ){
		return;
	}
	_SkillTree->HideActiveSkillDetail();
}
//============================================================================================================================
void UMMOGFXPlayer_SkillTree::ASShowLearnedActiveSkills()
{
	if( _SkillTree == NULL ){
		return;
	}

	_SkillTree->ShowLearnedActiveSkills();
}

void UMMOGFXPlayer_SkillTree::ASShowAllActiveSkills()
{
	if( _SkillTree == NULL ){
		return;
	}

	_SkillTree->ShowAllActiveSkills();
}

void UMMOGFXPlayer_SkillTree::ASShowSkillSetting()
{
	if( _SkillTree != NULL && _SkillTree->ActiveSkillsDetailListPanel != NULL ){
		_SkillTree->HideActiveSkillDetail();
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
//-----------------------heaven----------------------------------------------
void UMMOGFXPlayer_SkillTree::ASClickConverter()
{
	if(MMOHeavenManager::getSingleton().GetTodayConvertTimes() >= MMOHeavenManager::getSingleton().GetTodayConvertMaxTimes())
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.HeavenMessage.ConvertTimesLimited") ));
		return;
	}
	if(_SkillTree)
		_SkillTree->ConverterConfirmShow();
}
void UMMOGFXPlayer_SkillTree::ASClickBtnPractiseGas(INT nBtnIndex)
{
	if(_GasPractisePanel)
		_GasPractisePanel->OnClickBtnPractiseGas(nBtnIndex);
}
void UMMOGFXPlayer_SkillTree::ASClickOneKeyMergeGas()
{
	if(_SkillTree)
		_SkillTree->OnClickOneKeyMerge();
}
void UMMOGFXPlayer_SkillTree::ASClickBtnLevelState(INT nLevelIndex,INT nPanelIndex)
{
	if(_SkillTree)
		_SkillTree->OnClickLevelState(nLevelIndex + 1,nPanelIndex + 1);
}
void UMMOGFXPlayer_SkillTree::ASActivateBlood(INT nType)
{
	if(_SkillTree)
		_SkillTree->OnClickActivateBlood(nType);
}
void UMMOGFXPlayer_SkillTree::ASOneKeyGoBlood(INT nType)
{
	if(nType < 0 || nType > MAX_BLOODSEA_PAGE)
		return;
	std::vector<INT> nNeedFreshIndex;
	for(INT n = 0; n < MAX_BLOODSEA_SLOT_PER_PAGE; ++n)
	{
		const BloodSlot* pkSlot = MMOHeavenManager::getSingleton().GetBloodSlot(nType,n+1);
		if(pkSlot)
		{
			if(pkSlot->nValue <= 0)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.CannotAutoFreshBlood") ));
				return;
			}
			else if(pkSlot->nValue != MAX_BLOODSEA_SLOT_NUMBER)
			{
				nNeedFreshIndex.push_back(n+1);
			}
		}
	}
	for(INT n = 0; n < (INT)nNeedFreshIndex.size(); ++n)
	{
		CMessageHandler::GetInstance().Send_HeavenBloodLevel(nNeedFreshIndex[n],nType);
	}
}
void UMMOGFXPlayer_SkillTree::ASClickTab(int nIndex)
{
	if(_SkillTree)
		_SkillTree->UpdatePanel(nIndex);
}
void UMMOGFXPlayer_SkillTree::ASClickExchangeGasItem(INT nGasID)
{
	if(_HeavenGasExchange)
		_HeavenGasExchange->OnClickExchangeGasItem(nGasID);
}
void UMMOGFXPlayer_SkillTree::ASClickOpenExchangePanel()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenGasExchang, "");
}
void UMMOGFXPlayer_SkillTree::ASClickOpenGasPractisePanel()
{
	UMMOUIHeavenGasPractisePanel* pkAim = Cast<UMMOUIHeavenGasPractisePanel>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_HeavenPractise));
	if(pkAim&&pkAim->IsVisible)
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenPractise,FALSE, "");
	else
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeavenPractise,TRUE, "");
}
void UMMOGFXPlayer_SkillTree::ASClickOpenHeavenLog()
{
	if(_SkillTree)
		_SkillTree->UpdateHeavenLog();
}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenLevel(class UGFxObject*mc ,INT nLevel)
{
	if( mc == NULL ){
		return;
	}
	FString hint;

	MMOHeavenManager::getSingleton().GetHeavenTips(hint,nLevel);

	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenLevel()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASClickOneKeyPractise()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	INT nLevel = mainPlayer->ActorAttributes->heavenAttributes.heavenLevel;
	if(nLevel >= MMOHeavenManager::oneKeyPracticeLevel)
		CMessageHandler::GetInstance().Send_PractiseGasOneKey();
	else
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.CannotAutoPractise") ));
}
void UMMOGFXPlayer_SkillTree::ASUpdateNeedTime(int nTime)
{

}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenBloodSlot(class UGFxObject* mc,INT nSlotIndex,INT nPanelIndex)
{
	if( mc == NULL ){
		return;
	}
	FString hint;
	MMOHeavenManager::getSingleton().GetHeavenBloodTips(hint,nPanelIndex+1,nSlotIndex+1);
	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenBloodSlot()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenExpBar(class UGFxObject* mc)
{
	if( mc == NULL ){
		return;
	}
	const HeavenLevelConfig* pkLevel =  MMOHeavenManager::getSingleton().GetHeavenCurLevelData();
	if(pkLevel == NULL)
		return;
	INT nExp = 0;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
		nExp = mainPlayer->ActorAttributes->heavenAttributes.heavenExp;

	FString hint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ExpValueBar") ),nExp,pkLevel->nMaxExp);

	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenExpBar()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenGasSlot(class UGFxObject* mc)
{
	if( mc == NULL ){
		return;
	}
	INT containerId = mc->GetInt(TEXT("containerID"));
	INT nSlotID     = mc->GetInt(TEXT("slotID"));

	return;
	FString hint;
	const GasItem* pkItem = MMOHeavenManager::getSingleton().GetGasItemBySlot(containerId,nSlotID);
	if(pkItem)
	{
		const HeavenGasItemConfig* pkTemp = MMOHeavenManager::getSingleton().GetGasTemplate(pkItem->nTempID);
		if(pkTemp)
		{
			hint += pkTemp->name;
			hint += '\n';
			hint += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ExpValue")),pkItem->nCurExp);
			hint += '\n';
			hint += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasTemplateValue")),pkItem->nTempID);
		}
	}
	mc->SetString( TEXT("tipsHtml"), hint );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenGasSlot()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenGasExchangeSlot(class UGFxObject* mc,INT nGasID)
{
	if(mc == NULL)
		return;
	FString tips;
	MMOHeavenManager::getSingleton().GetHeavenGasTipsByGasID(tips,nGasID);
	mc->SetString( TEXT("tipsHtml"), tips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenExchangeGasSlot()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenGasPower(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	FString tips;
	MMOHeavenManager::getSingleton().GetEquipGasPowerDescrible(tips);
	if(tips.IsEmpty())
		return;
	mc->SetString( TEXT("tipsHtml"), tips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenGasPower()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASShowTips_HeavenHintCtrl(class UGFxObject* mc,INT nIdex)
{
	if (mc == NULL)
		return;
	FString hintTips;
	switch(nIdex)
	{
	case 0:hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintNormalXiuLian"));break;
	case 1:
		{
			hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintVip"));
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(mainPlayer&&mainPlayer->ActorAttributes&&mainPlayer->GetVipType() > 0)
				hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintVip"));
			break;
		}
	case 2:hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSoulPool"));break;
	case 3:hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintXiuLianPool"));break;
	case 4:
		{
			hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSpeedHint1"));
			AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if(mainPlayer&&mainPlayer->ActorAttributes&&mainPlayer->GetVipType() > 0)
				hintTips = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSpeedHint2"));
			break;
		}
	case 5:
		{
			hintTips = TEXT("Converts automatically when the countdown ends");
			break;
		}
	}
	mc->SetString( TEXT("tipsHtml"), hintTips );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}
void UMMOGFXPlayer_SkillTree::ASHideTips_HeavenHintCtrl()
{
	MMOUI::GetUIManager()->delegateHideTips();
}
void UMMOGFXPlayer_SkillTree::ASStartDrag(class UGFxObject* SrcItemSlot,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID)
{
	FString iconImage;

	GasItem*pkGasItem = NULL;
	UMMOAbility *skill = NULL;
	UItem* itemTemp = NULL;

	//whether it is an ability
	if(SrcSlotType == ESPType_SkillTree)
	{
		INT uniqueSkillID = SrcItemSlot->GetInt(TEXT("itemIndex"));
		skill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
		iconImage = FString::Printf(TEXT("img://%s"), *skill->GetAbilityIconName());
	}
	else if(ESPType_HeavenGasEquip == SrcSlotType || ESPType_HeavenGasBag == SrcSlotType || ESPType_HeavenGasBank == SrcSlotType)
	{
		//whether it is realm qi
		pkGasItem = MMOHeavenManager::getSingleton().GetItemBySlot(SrcContainerID,SrcSlotID);
		//needs an icon
		if(pkGasItem)
		{
			const HeavenGasItemConfig*	pkGasTemp = MMOHeavenManager::getSingleton().GetGasTemplate(pkGasItem->nTempID);
			if(pkGasTemp)
				iconImage = FString::Printf(TEXT("../%s"),*(pkGasTemp->Icon0));
		}
	}
	else if (ESPType_HeavenSoul == SrcSlotType)
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEPOSIT, 0);
		if(info)
		{
			itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
			if(itemTemp)
			{
				iconImage = FString::Printf(TEXT("img://%s"), *MMOFacade::getSingleton().GetItemIconNamePersonalized( itemTemp ));
			}
		}
	}

	if(skill || pkGasItem || itemTemp)
	{
		UGFxObject* dropIcon = GetDragIcon();
		if(dropIcon)
		{
			g_displayDragDropIcon = true;
			FIntPoint pt = MMOFacade::GetMouseCursorPosition();
			dropIcon->SetPosition(pt.X , pt.Y);
			dropIcon->SetVisible(true);
			dropIcon->SetInt(TEXT("containerID"), E_CONSLOT_MAX);
			dropIcon->SetInt(TEXT("slotID"), -1);
			dropIcon->SetInt(TEXT("SlotType"), ESPType_Invalid);
			UGFxObject* uiLoader = dropIcon->GetObject( TEXT("IconLoader_mc") );
			if( uiLoader )
			{
				uiLoader->SetString(TEXT("source"), iconImage);
				//uiLoader->SetVisible((pkGasItem == NULL)?false:true);
			}
			UGFxObject* uiGasIcon = dropIcon->GetObject( TEXT("gasIcon_mc") );
			if(uiGasIcon)
			{
				if(_SkillTree&&_SkillTree->_GasPractiseMain)
				{
					_SkillTree->_GasPractiseMain->OnStartDragGas(SrcContainerID,SrcSlotID,TRUE);
				}
				// 
				// 				UGFxObject* gasIcon0 = uiGasIcon->GetObject( TEXT("par_star1") );
				// 				UGFxObject* gasIcon1 = uiGasIcon->GetObject( TEXT("par_star2") );
				// 				UGFxObject* gasIcon2 = uiGasIcon->GetObject( TEXT("par_ripple1") );
				// 				UGFxObject* gasIcon3 = uiGasIcon->GetObject( TEXT("par_ripple2") );
				// 
				// 				if(pkGasItem)
				// 				{
				// 					const HeavenGasItemConfig* pkGas = MMOHeavenManager::getSingleton().GetGasTemplate(pkGasItem->nTempID);
				// 					if(pkGas)
				// 					{
				// 						if(gasIcon0)
				// 							gasIcon0->SetString(TEXT("source"), pkGas->Icon1);
				// 						if(gasIcon1)
				// 							gasIcon1->SetString(TEXT("source"), pkGas->Icon1);
				// 						if(gasIcon2)
				// 							gasIcon2->SetString(TEXT("source"), pkGas->Icon0);
				// 						if(gasIcon3)
				// 							gasIcon3->SetString(TEXT("source"), pkGas->Icon0);
				// 
				// 						TArray<struct FASValue> args;
				// 						FASValue gasColor(EC_EventParm);
				// 						gasColor.Type = AS_Int;
				// 						gasColor.I = pkGas->nQuality;
				// 						args.AddItem(gasColor);
				// 						uiGasIcon->Invoke("SetIconType",args);
				// 					}
				// 				}
				uiGasIcon->SetVisible(false/*(pkGasItem != NULL)*/);
			}
		}

		MMOFacade::ShowMouseCursor(FALSE);
	}
}
//============================================================================================================================
void UMMOGFXPlayer_SkillTree::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_SkillTree::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_SkillTree );
	//MMOUI::GetUIManager()->SetOpenSkillDelegate( OpenSkillPanel );
	//__OnPostAdvance__Delegate = FScriptDelegate(this, TEXT("PostAdvance"));
}

void UMMOGFXPlayer_SkillTree::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);

	//__OnPostAdvance__Delegate = FScriptDelegate(EC_EventParm);

}
