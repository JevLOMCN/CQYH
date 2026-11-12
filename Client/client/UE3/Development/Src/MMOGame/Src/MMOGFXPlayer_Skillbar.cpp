#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgButtonManager.h"
#include "MMOSystemActivateManager.h"
#include "MMOSkillManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_Skillbar)

//------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Skillbar::ASOnPanelClose(const FString& PanelName)
{
//	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Option_InGameMenu);
	//MMOUI::GetUIManager()->delegateShowInGameMenu();
}

//===============================================================================================================
void UMMOGFXPlayer_Skillbar::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	if( SrcSlotType == ESPType_SkillBar || DestSlotType == ESPType_SkillBar )
	{
		Skillbar->OnDragDrop(SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID);
		return;
	}
	else
	{
		UMMOGFXPlayer::OnDragDrop(SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID);
	}
	
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void UMMOGFXPlayer_Skillbar::ASStartDrag( UGFxObject* srcItemSlot, BYTE srcSlotType, BYTE srcContainerID, INT srcSlotID)
{
	Super::ASStartDrag( srcItemSlot, srcSlotType, srcContainerID, srcSlotID );

	//
	if( srcItemSlot == NULL || Skillbar == NULL ){
		return;
	}

	switch( srcSlotType )
	{
	case ESPType_SkillBar:
		{
			INT skillBarContainType = srcItemSlot->GetInt( TEXT("EquipID") );
			if( skillBarContainType == ESBarSlotTp_Skill ){
				INT uniqueSkillID = srcItemSlot->GetInt(TEXT("itemIndex"));

				UMMOAbility *pDragingAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
				if( pDragingAbility != NULL ){
					Skillbar->HintForDraggingSkill( pDragingAbility );
				}
			}
			else if(  skillBarContainType == ESBarSlotTp_Item  ){
				INT itemSlotIdx = srcItemSlot->GetInt( TEXT("slotIndex") );
				Skillbar->HintForDraggingItem( ESPType_SkillBar, itemSlotIdx );
			}
		}
		break;
	//case ESPType_SkillBar:
	//	{
	//		INT skillID = srcItemSlot->GetInt( TEXT("itemIndex") );
	//		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	//		if( mainPlayer != NULL ){
	//			UMMOAbility *dragSkill = mainPlayer->GetEquipedAbilityById( skillID );
	//			Skillbar->HintForDraggingSkill( dragSkill );
	//		}
	//	}
	//	break;
	//case ESPType_ItemBar:
	//	{
	//		//INT fromSlotType = srcItemSlot->GetInt( TEXT("slotType") );
	//		INT itemSlotIdx = srcItemSlot->GetInt( TEXT("slotIndex") );
	//		Skillbar->HintForDraggingItem( ESPType_ItemBar, itemSlotIdx );
	//	}
	//	break;
	}
}

//---------------------------------------------------------------------------------------
void UMMOGFXPlayer_Skillbar::ASSlot_StartDrag(class UGFxObject* mcSlot)
{
	Super::ASSlot_StartDrag( mcSlot );

	if( mcSlot == NULL ){
		return;
	}

	//
	INT slotType = mcSlot->GetInt(TEXT("slotType"));
	switch( slotType )
	{
	//case ESPType_SkillSetting:
	case ESPType_SkillBar:
		{
			INT slotIdx = (INT)mcSlot->GetInt( TEXT("slotIndex") );
			Skillbar->HintForDraggingItem( slotType, slotIdx );
			//INT skillBarContainType = mcSlot->GetInt( TEXT("EquipID") );
			//if( skillBarContainType == ESBarSlotTp_Skill ){
			//	INT uniqueSkillID = mcSlot->GetInt(TEXT("itemIndex"));

			//	UMMOAbility *pDragingAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
			//	if( pDragingAbility != NULL ){
			//		Skillbar->HintForDraggingSkill( pDragingAbility );
			//	}
			//}
			//else if(  skillBarContainType == ESBarSlotTp_Item  ){
			//	INT itemSlotIdx = mcSlot->GetInt( TEXT("slotIndex") );
			//	Skillbar->HintForDraggingItem( ESPType_SkillBar, itemSlotIdx );
			//}
		}
		break;
	//case ESPType_ItemBar:
	//	{
	//		INT itemSlotIdx = datSlot->GetInt(TEXT("slotID"));
	//		Skillbar->HintForDraggingItem( ESPType_ItemBar, itemSlotIdx );
	//	}
	//	break;
	}
}
//==============================================================================================
void UMMOGFXPlayer_Skillbar::FlashHPBar()
{
	if(Skillbar){
		Skillbar->FlashHPBar();
	}
}

//==============================================================================================
void UMMOGFXPlayer_Skillbar::ASClickShoppingMallBtn()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Mall, "");
}
void UMMOGFXPlayer_Skillbar::ASClickActivityBtn()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Activity, "");
}
//========================================================================================================
void UMMOGFXPlayer_Skillbar::ASClickMailIcon()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx(MMOUI::GetUIManager()->Panel_Mail, "");
}

//void UMMOGFXPlayer_Skillbar::ASMsgButtonClick( INT type )
//{
//	MMOMsgButtonManager::getSingleton().ClickButton(type);
//}
//
//void UMMOGFXPlayer_Skillbar::ASMsgListClick( INT pos , INT type )
//{
//	MMOMsgButtonManager::getSingleton().ClickMsgLine( pos , type );
//}

//===================================================================
void UMMOGFXPlayer_Skillbar::ASOnClickNewSkillLearnedBtn()
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASOnClickNewSkillLearnedBtn No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	////
	//Skillbar->OnClickNewSkillLearnedBtn();
}
//===================================================================
//Ability settings
void UMMOGFXPlayer_Skillbar::ASShowTips_SkillSettingActive( UGFxObject *mc )
{
	if( mc == NULL ){
		return;
	}

	INT uniqueSkillID = mc->GetInt( TEXT("UniqueSkillID") );
	if( uniqueSkillID > 0 ){
		MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_SkillTemplate, mc );
	}
}
//===================================================================
void UMMOGFXPlayer_Skillbar::ASHideTips_SkillSettingActive( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}
//===================================================================
void UMMOGFXPlayer_Skillbar::ASSetSkill( int skillKeyType, int skillID )
{
	if( Skillbar == NULL ){
		return;
	}

	Skillbar->TryToEquipOnSkill( skillKeyType, skillID );
}
//===================================================================
void UMMOGFXPlayer_Skillbar::ASCloseSkillList()
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASCloseSkillList No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	//Skillbar->ShowSkillListPanel( FALSE, 0 );
}
//===================================================================
void UMMOGFXPlayer_Skillbar::ASShowPanel( INT idx )
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASShowPanel No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	//Skillbar->ShowSkillListPanel( TRUE, idx );
}

//===================================================================
void UMMOGFXPlayer_Skillbar::ASHideShortcutButtons()
{
	warnf( NAME_Error, TEXT("UMMOGFXPlayer_Skillbar::ASHideShortcutButtons No this function") );
	//if( Skillbar == NULL ){
	//	return;
	//}

	//Skillbar->ShowSkillListPanel( FALSE, 0 );
}
//=================================================================================
void UMMOGFXPlayer_Skillbar::UpdateSkillBarKeyDown(UBOOL bRelease, INT inCmdID, INT slotCmdKey )
{
	if(Skillbar){
		Skillbar->UpdateSkillBarKeyDown( bRelease, inCmdID, slotCmdKey );
	}
}
//=================================================================================
void UMMOGFXPlayer_Skillbar::UpdateMainMenuKeyDown(UBOOL bRelease, INT InCmdID, INT InSlotID)
{
	if( _Menubar == NULL ){
		return;
	}
	
	_Menubar->UpdateMainMenuKeyDown(bRelease, InCmdID, InSlotID);
}
//=================================================================================
/*
void UMMOGFXPlayer_Skillbar::ASMenuClick( INT menuId,INT menuType )
{
	if( _Menubar == NULL ){
		return;
	}
	
	_Menubar->ClickMenu( menuId , menuType );
}

void UMMOGFXPlayer_Skillbar::ASMenuChildClick( INT menuId,INT menuType,INT childMenuId,INT childMenuType )
{
	if( _Menubar == NULL ){
		return;
	}
	
	_Menubar->ClickMenuChild( menuId , menuType , childMenuId , childMenuType );
}
*/
//=================================================================================
// menu bar
void UMMOGFXPlayer_Skillbar::ASActivateNewSystem(INT systemType)
{
	if( _Menubar == NULL ){
		return;
	}
	
	_Menubar->ActivateNewSystem( systemType );
}
//=================================================================================
void UMMOGFXPlayer_Skillbar::ASShowNextNewSystem()
{
	if( _Menubar == NULL ){
		return;
	}

	INT systemType = MMOSystemActivateManager::getSingleton().GetNextSystemType();
	if (systemType == EAST_Tatol){
		return;
	}
	_Menubar->ShowNewSystem( systemType );
}
//===================================================================
void UMMOGFXPlayer_Skillbar::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_Skillbar::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_Skillbar);

	MMOUI::GetUIManager()->SetAddBuffPanelDelegate(FScriptDelegate(this, TEXT("AddBuffPanel")));
	MMOUI::GetUIManager()->SetDeleteBuffPanelDelegate(FScriptDelegate(this, TEXT("DeleteBuffPanel")));
	MMOUI::GetUIManager()->SetUpdateSkillBarKeyDownDelegate(FScriptDelegate(this, TEXT("UpdateSkillBarKeyDown")));
	MMOUI::GetUIManager()->SetUpdateMainMenuKeyDownDelegate(FScriptDelegate(this, TEXT("UpdateMainMenuKeyDown")));
	MMOUI::GetUIManager()->SetFlashHPBarDelegate(FScriptDelegate(this, TEXT("FlashHPBar")));
	MMOUI::GetUIManager()->SetUpdateMenuEffectDelegate(FScriptDelegate(this, TEXT("CallUpdateMenuEffect")));
	
}

void UMMOGFXPlayer_Skillbar::UnInit()
{
	Super::UnInit();

	MMOUI::GetUIManager()->SetAddBuffPanelDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetDeleteBuffPanelDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetUpdateSkillBarKeyDownDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetUpdateMainMenuKeyDownDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetFlashHPBarDelegate(FScriptDelegate(EC_EventParm));
	MMOUI::GetUIManager()->SetUpdateMenuEffectDelegate(FScriptDelegate(EC_EventParm));
}