/********************************************************************
	Created:	2012/06
	Filename: 	MMOUISkillPanel
	file ext:	cpp
	Author:		hemu@snda
	Purpose:	MMOUISkillPanel ability UI, contains the following 4 parts
				-Console ability overview
				-Active  active abilities
				  -Rune rune charge, activation
				  -SkillUp ability proficiency upgrade: heart training, soul training
				-Passive passive abilities
				-Avarta  Avarta abilities
*********************************************************************/

#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "MMOChatManager.h"
#include "MMOMsgBoxManager.h"
#include <deque>
#include "MMOSkillManager.h"


IMPLEMENT_CLASS(UMMOUISkillPanel)

struct sSpellChange
{
	int SlotId;
	int AbilityId;
};

typedef std::deque<sSpellChange, M_UE3_STL_ALLOC(sSpellChange) > SpellChangeDeque;
#if 0
void UMMOUISkillPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_MainPlayerDataChange);
	EventAdapter = new TGameEventAdapter<UMMOUISkillPanel>(this, evts);
}

//do some data updates when displaying
void UMMOUISkillPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_skillbook_op"));
		//If visible, refresh the UI display
		eventIniData();
		eventRefreshData();
	}
	else
	{
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_skillbook_cls"));
	}
}

int UMMOUISkillPanel::GetRealActiveRealSlotId(int activeSlotId )
{
    int realSlotId = 0;
    if( activeSlotId >= 0 && activeSlotId < ActiveRealSlotList.Num() )
    {
        realSlotId = ActiveRealSlotList( activeSlotId ); 
    }
    return realSlotId;
}

void UMMOUISkillPanel::NativeInitOnce()
{
	StrTimeLeft = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Common.TimeLeft") );

	if(!Validate())
		return;


	UGFxObject* obj = GetObject( TEXT("ActiveModify.btn_confirm.textField.textField") );
	if( obj )
	{
		obj->SetText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Common.Ok") ) );
	}
	
	obj = GetObject( TEXT("ActiveModify.btn_cancel.textField.textField") );
	if( obj )
	{
		obj->SetText( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Common.Cancel") ) );
	}
	

	EnableSkillUp( true );
}

//Send Slot switch request
void UMMOUISkillPanel::SendEquipActive(int activeSlotId,int abilityId )
{
	int realSlotId = GetRealActiveRealSlotId( activeSlotId );
	CMessageHandler::GetInstance().Send_EquipSpell( realSlotId, abilityId ) ; 
}

//Send Passive Slot switch request
void UMMOUISkillPanel::SendEquipPassive(int passiveSlotId,int abilityId )
{
	//3~5
	CMessageHandler::GetInstance().Send_EquipSpell( passiveSlotId + ECK_Passive1, abilityId ) ; 
}
 

//Send Avarta Slot switch request
void UMMOUISkillPanel::SendEquipAvarta(int avartaSlotId,int abilityId )
{
	//0~2
	CMessageHandler::GetInstance().Send_EquipSpell( avartaSlotId + ECK_QTE1 , abilityId ) ; 
}

//Activate rune
void UMMOUISkillPanel::SendActivateRune( int inSpellId, int runeSlotId)
{
	CMessageHandler::GetInstance().Send_ActivateRune( inSpellId, runeSlotId ) ; 
}

//Equip Active rune
void UMMOUISkillPanel::SendEquipActiveRune( int activeSlotId, int runeSlotId )
{
	int realSlotId = GetRealActiveRealSlotId( activeSlotId );
	CMessageHandler::GetInstance().Send_EquipRune( realSlotId, runeSlotId ) ; 
}

//Equip Avarta rune
void UMMOUISkillPanel::SendEquipAvartaRune( int avartaSlotId, int runeSlotId)
{
	CMessageHandler::GetInstance().Send_EquipRune( avartaSlotId + ECK_QTE1, runeSlotId ) ; 
}

//Sync rune durability data
void UMMOUISkillPanel::RuneDurSync( int wSpellID, int byRuneSlot, int byDurabilityType, int nDurability )
{
	/*UMMOAbility* tMainActive = MainPlayer->GetMainAbilityById( wSpellID );
	if ( !tMainActive )
		return;
	UMMOAbility* tRune=NULL;

	if( byRuneSlot <=0 || byRuneSlot > tMainActive->SubRuneList.Num() )
		return;
	tRune = tMainActive->SubRuneList( byRuneSlot-1 );
	if( tRune )
	{
		tRune->RuneDurabilityType = byDurabilityType;
		tRune->RuneDurability = nDurability;
	}*/
}

//Rune activation response, needs UI response
void UMMOUISkillPanel::RuneActivate( int wSpellID, int byRuneSlot, int byDurabilityType, int nDurability )
{
	/*RuneDurSync(wSpellID,byRuneSlot,byDurabilityType,nDurability);
	OnSelecteActiveChanged( CurrentSelectedActiveId );*/
}

//Rune equip response
void UMMOUISkillPanel::RuneEquip( int bySlot, int wSpellID, int byRuneSlot )
{
/*
	if(	IsVisible )
	{
		if( byRuneSlot == 0 )
		{
			TArray<struct FASValue> Arguments0;
			if( GetObject("ActiveModify") )
				GetObject("ActiveModify")->Invoke("UnselecteAllRune", Arguments0 );
		}
		RefreshAll();
	}*/
}

//==================================================Total()====================================================== 
UBOOL UMMOUISkillPanel::Validate()
{
	if( MainPlayer == NULL)
	{
		return false;
	}
	if( PC == NULL)
	{
		return false;
	}
	if( KeyMapManager == NULL)
	{
		return false;
	}

	return true;
}

//void UMMOUISkillPanel::Open()
//{
//	ToogleVisible();
//}
//
//void UMMOUISkillPanel::OnOpen()
//{
//	eventIniData();
//	//If visible, refresh the UI display
//	eventRefreshData();
//}

//Reset state after re-login
void UMMOUISkillPanel::EnterLogin( )
{
	//State cleanup
	NetStopSkillPractice( NowNormalPracticeActiveId );
	NetStopSkillPractice( NowFastPracticeActiveId );

	//Reset auspicious light data after re-login
	CurPageType = 0;

	NowActivePageNum = 0;
	NowPassivePageNum = 0;
	NowAvartaPageNum = 0;
	NowAvartaRunePageNum = 0;

	TotalActivePageNum = 0;
	TotalPassivePageNum = 0;
	TotalAvartaPageNum = 0;
	TotalAvartaRunePageNum = 0;

	CurrentActiveSlot = 0;
	CurrentSelectedActiveSlot = 0;
	CurrentSelectedActiveId = 0;
	CurrentSelectedActiveRuneSlot = 0;

	CurrentSelectedAvartaSlot = 0;
	CurrentSelectedAvartaId = 0;
	CurrentSelectedAvartaRuneSlot = 0;

	NowRequestNormalPracticeActiveId = 0;
	NowNormalPracticeActiveId = 0;
	NowNormalPracticeType = 0;
	NowNormalPracticeProgress = 0;

	NowRequestFastPracticeActiveId = 0;
	NowFastPracticeActiveId = 0;
	NowFastPracticeType = 0;
	NowFastPracticeProgress = 0;
	NowPracticeType = 0;
	IsInFastPractice = false;

	StrTimeLeft = "";

	IsRequestCloseNormalSkillPractice = false;
	IsRequestCloseFastSkillPractice = false;
	HasAskStopSkillPractice = false;
}

//SlotChange
void UMMOUISkillPanel::SkillSlotChange()
{
	if( IsVisible )
		RefreshAll();
}

//Refresh
void UMMOUISkillPanel::RefreshAll()
{
	if( ! Validate() )
		return;
	RefreshCurrentPage();
	RefreshPage( ESkill_Console );
}

//Equipment changed
void UMMOUISkillPanel::BagSlotChange(INT BagId,INT SlotId)
{
	if( !IsVisible )
		return;
	//Refresh when rune fragments change
	if( CurPageType == ESkill_Active )
	{
		if( BagId != E_CONSLOT_DEFAULT )
			return;
		ItemInfo* pItemInfo = NULL;
		pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_DEFAULT, SlotId );
		if( pItemInfo == NULL )
			return;
		INT iNewItemID = pItemInfo->m_nTemplateID;

		const TArray<INT> *pVecRuneDebrisIds = MMOSkillManager::getSingleton().GetRuneDebrisIds();
		if( !pVecRuneDebrisIds )
			return;
		for ( int i =0 ;i < pVecRuneDebrisIds->Num(); ++i )
		{
			if(  (*pVecRuneDebrisIds)(i) == iNewItemID )
			{
				RefreshCurrentPage();
				return;
			}
		}
	}
}

//Refresh current page
void UMMOUISkillPanel::RefreshCurrentPage( )
{
	RefreshPage( CurPageType );
}

//Refresh page
void UMMOUISkillPanel::RefreshPage( BYTE tSPT )
{
	if( !IsVisible )
		return;
	switch( tSPT ) 
	{
	case ESkill_Console:
		RefreshCurrentSpell();
		break;
	case ESkill_Active:
		RefreshActiveModify();
		break;
	case ESkill_Passive:
		RefreshPassiveModify();
		break;
	case ESkill_Avarta:
		RefreshAvartaModify();
		break;
	}
}

//Page jump
void UMMOUISkillPanel::PageJump( BYTE tSPT )
{
	int i;
	if( tSPT == CurPageType)
		return;
	CurPageType = tSPT;
	switch( CurPageType ) 
	{
	case ESkill_Console:
		break;
	case ESkill_Active:
		CurrentActiveSlot = -1;
		for( i = 0; i < CurrentActives.Num() ; ++i )
		{
			CurrentActives( i ).ActiveId = RealActives( i ).ActiveId ;
			CurrentActives( i ).IsChanged = false;
		}
		break;
	case ESkill_Passive:
		CurrentPassiveSlot = -1;
		for( i = 0; i < CurrentPassives.Num() ; ++i )
		{
			CurrentPassives( i ).PassiveId = RealPassives( i ).PassiveId ;
			CurrentPassives( i ).IsChanged = false;
		}
		break;
	case ESkill_Avarta:
		CurrentAvartaSlot = -1;
		for( i = 0; i < CurrentAvartas.Num() ; ++i )
		{
			CurrentAvartas( i ).AvartaId = RealAvartas( i ).AvartaId ;
			CurrentAvartas( i ).IsChanged = false;
		}
		break;
	}
}

void UMMOUISkillPanel::SetLockedState( int slotId, UGFxObject* tObj, int uiSlot )
{
    if( ! Validate() )
        return;
    if( !tObj || !GFXPlayer )
        return;
    UBOOL  isLocked =   KeyMapManager->IsSlotLocked( slotId );
    if( tObj->GetObject("ItemSlot") )
    {
        tObj->GetObject("ItemSlot")->SetBool( "isSlotLocked", isLocked );
    }
    tObj->SetBool("enabled",!isLocked );
   
}

//Refresh the whole panel
void UMMOUISkillPanel::RefreshCurrentSpell()
{
	if( ! Validate() )
		return;
	int j;
	int CmdSlotId;
	int abilityId;
    UGFxObject* tObj;

	//`log("===========================================RefreshCurrentSpell ()=======================================");

	//---------------------------1 Current skill panel ---------------------------------------------------------
	
    //1
	//Load the active skills currently equipped on hotkeys
	for( j = 0; j< 6;j++ )
	{
        int realSlotId = GetRealActiveRealSlotId( j );
		tObj = GetObject("SpellConsole")->GetObject( FString::Printf(TEXT("ActiveSkill%d"),j ) );
		if( tObj == NULL )
		{
			break;
		}
		abilityId = KeyMapManager->GetCmdIDBySlotID( realSlotId );
		
        //Set Btn according to ActiveId
		SetSpellBtnByActiveId( tObj, j, abilityId );
		RealActives( j ).ActiveId = abilityId;
        SetLockedState( realSlotId,  tObj, j );
	}	

	//2
	//Load currently equipped passive skills
	for( j = 0; j< 3;j++)
	{
		tObj = GetObject("SpellConsole")->GetObject( FString::Printf(TEXT("PassiveSkill%d"),j ) );
		if( tObj == NULL )
		{
			break;
		}
		CmdSlotId = ECK_Passive1 + j;
		abilityId = KeyMapManager->GetCmdIDBySlotID( CmdSlotId );
		SetSpellBtnByPassiveId( tObj, j, abilityId );
		//Reserve a copy of the real data
		RealPassives( j ).PassiveId = abilityId;
        SetLockedState( CmdSlotId,  tObj, j  );
	}	

	//3
	//Load currently equipped tactical skills
	for( j = 0; j< 3;j++)
	{
		tObj = GetObject("SpellConsole")->GetObject( FString::Printf(TEXT("AvartaSkill%d"),j ) );
		if( tObj == NULL )
		{
			break;
		}
		CmdSlotId = ECK_QTE1 + j;
		abilityId = KeyMapManager->GetCmdIDBySlotID( CmdSlotId );
		SetSpellBtnByAvartaId( tObj,j , abilityId );
		RealAvartas( j ).AvartaId = abilityId;
        SetLockedState( CmdSlotId,  tObj, j  );
	}

    //Show current skill points
    if(  GetObject("SpellConsole") && GetObject("SpellConsole")->GetObject("tF_spellPoint") )
        GetObject("SpellConsole")->GetObject("tF_spellPoint")->SetText( appItoa( PC->SkillPoint ) );

}
//==================================================RefreshActiveModify()====================================================== 

//Set ActiveBtn
void UMMOUISkillPanel::SetSpellBtnByActiveId(class UGFxObject* tObj,INT SlotId,INT abilityId,UBOOL isSubCtrl)
{
#if M_TEMP_ENABLE_UI_Skill_CODE
	UMMOAbility*  ability;
	if( tObj == NULL )
	{
		return;
	}
	//Set Btn according to ActiveId
	ability = MainPlayer->GetMainAbilityById( abilityId );

	//`log("CurrentSpell Active Add Active ()"@tObj@tObj->GetObject("ItemSlot")@"ActiveId::"@abilityId );
	if( ability != NULL && ability->eventGetAbilityId() >=0  )
	{
		tObj->SetVisible(true);
		tObj->SetBool("enabled",true);
		if( tObj->GetObject("ItemSlot") )
		{
// 			UGFxObject* tItem = tObj->GetObject("ItemSlot") ;
// 			FString iconName = ability->GetAbilityIconName();
// 			FString itemType = TEXT("skill");
			//eventSetItem(tItem ,0 , iconName, abilityId, itemType );
			GFXPlayer->eventCallSetItemData( tObj->GetObject("ItemSlot") , 0 , ability->GetAbilityIconName() , ability->eventGetAbilityId() , ESPType_SkillPanel,TEXT("") );
			//GFXPlayer->eventASSetSlotShowData(tObj->GetObject("ItemSlot") ,0 ,ability->GetAbilityIconName() ,ability->eventGetAbilityId());
			tObj->GetObject("ItemSlot")->SetInt( "SlotType" , ESPType_SkillPanel );
		}
		//tF_SpellName skill name
		if( tObj->GetObject("tF_SpellName") )
			tObj->GetObject("tF_SpellName")->SetText( ability->ClientCommonAttribute.AbilityName );
		
		//tF_RuneName rune used
		INT RuneId = ability->GetRuneSlot() ;
		TArray<struct FASValue> Arguments0;
		FASValue ASValue0(EC_EventParm);
		//Icon'
		ASValue0.I = 0;
		ASValue0.Type = AS_Int;
		Arguments0.AddItem(ASValue0);
		//AbilityName
		ASValue0.S = "";
		ASValue0.Type = AS_String;
		Arguments0.AddItem(ASValue0);
		if( RuneId > 0 && ability->SubRuneList.Num() >0 && RuneId <= ability->SubRuneList.Num() )
		{
			UMMOAbility* tRune = ability->SubRuneList( RuneId-1 );
			if( tRune )
			{
				Arguments0(0).I = tRune->GetSkillGuiRuneID(); 
				Arguments0(1).S = tRune->eventGetAbilityName(); 
			}
		}
		tObj->Invoke("SetRuneIcon", Arguments0 );

		//tF_level  level
		if( tObj->GetObject("tF_level") )
			tObj->GetObject("tF_level")->SetText( FString::Printf( TEXT("Lv %d"), ability->SpellCommonAttribute.Level ) );

		if( isSubCtrl )
		{
			tObj->SetBool("selected",false);
			//If this skill is already equipped, then
			if( tObj->GetObject("EquipedCheck") )
			{
				if( IsActiveAlreadyEquiped( abilityId ) >= 0 )
					tObj->GetObject("EquipedCheck")->SetVisible( true );
				else
					tObj->GetObject("EquipedCheck")->SetVisible( false );
			}
			//The corresponding Main skill is selected by default
			if( abilityId ==  CurrentSelectedActiveId )
				tObj->SetBool("selected",true);
			//Record the corresponding abilityId
			tObj->SetInt("ActiveId",abilityId );
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( false );
			//If this skill is not allowed to be equipped by left click
			if( CurrentActiveSlot==0 && ! ability->ClientCommonAttribute.CanLMouseEquip )
			{
				tObj->SetBool("enabled",false);
				if( tObj->GetObject("EquipedCheck") )
					tObj->GetObject("EquipedCheck")->SetVisible( false );
				if( tObj->GetObject("lock_mc") )
					tObj->GetObject("lock_mc")->SetVisible( true );
			}
		}
	}
	else
	{
		if( tObj->GetObject("ItemSlot") )
			eventSetItem(tObj->GetObject("ItemSlot") ,0,"",-1 );
		//tF_SpellName skill name
		if( tObj->GetObject("tF_SpellName") )
			tObj->GetObject("tF_SpellName")->SetText( "" );
		//tF_RuneName rune used
		TArray<struct FASValue> Arguments0;
		FASValue ASValue0(EC_EventParm);
		//Icon'
		ASValue0.S = "";
		ASValue0.Type = AS_String;
		Arguments0.AddItem(ASValue0);
		//AbilityName
		ASValue0.S = "";
		ASValue0.Type = AS_String;
		Arguments0.AddItem(ASValue0);
		tObj->Invoke("SetRuneIcon", Arguments0 );

		//tF_level  level
		if( tObj->GetObject("tF_level") )
			tObj->GetObject("tF_level")->SetText("");
		if( isSubCtrl )
		{
			tObj->SetBool("selected",false);
			tObj->SetBool("enabled",false);
			if( tObj->GetObject("EquipedCheck") )
				tObj->GetObject("EquipedCheck")->SetVisible( false );
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( true );
		}
		else
		{
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( false );
		}
	}

#endif
}

//Select the current CurrentActiveSlot
void UMMOUISkillPanel::SetCurrentActive( int activeSlotId )
{
	UGFxObject*  tObj = NULL;
	PageJump( ESkill_Active );
	
	if( activeSlotId >=0 && activeSlotId < CurrentActives.Num() )
	{
		CurrentActiveSlot = activeSlotId;
		if(  GetObject("ActiveModify") )
		{
			//Currently equipped Active skill
			tObj = GetObject("ActiveModify")->GetObject( TEXT("CurrentSpellSlot") );
			SetSpellBtnByActiveId( tObj,0 , CurrentActives( CurrentActiveSlot ).ActiveId ,false );
			//Cannot switch
			if( tObj )
				tObj->SetBool( "enabled", false );
		}
		//First actively refresh the runes once
		SelectActive( CurrentActiveSlot, CurrentActives( CurrentActiveSlot ).ActiveId );
		RefreshActiveModify();
	}
}

//RefreshActiveModify
void UMMOUISkillPanel::RefreshActiveModify()
{
	int j,k;
	int abilityId;
	int iTotalAbilitiesNum;
	UGFxObject*  tObj = NULL;
	UMMOAbility*  ability;

	//`log("==============================RefreshActiveModify() Opended=================================================");
	if( !Validate() )
		return;
	//--------------------------- Select skill panel ---------------------------------------------------------

	if( ! GetObject("ActiveModify") )
		return;
	//All Active skills learned by the player of the current class
	k = 0;

	iTotalAbilitiesNum = MainPlayer->Abilities.Num() ;
	//Refresh current page number
	TotalActivePageNum = iTotalAbilitiesNum / MaxActiveModifyCount;
	if( iTotalAbilitiesNum % MaxActiveModifyCount > 0 )
		++TotalActivePageNum;

	TArray<struct FASValue> Arguments0;
	FASValue ASValue0(EC_EventParm);
	ASValue0.I = NowActivePageNum+1;
	ASValue0.Type = AS_Int;
	Arguments0.AddItem(ASValue0);
	ASValue0.I = TotalActivePageNum ; 
	ASValue0.Type = AS_Int;
	Arguments0.AddItem(ASValue0);
	//Invoke
	GetObject("ActiveModify")->Invoke("SetActivePageNum", Arguments0 );

    int currentActiveSlot = GetRealActiveRealSlotId( CurrentSelectedActiveSlot );
	//Work out which skills are actually shown
	int iOffset = NowActivePageNum  * MaxActiveModifyCount;
    int activeSlot = 0;;
	for ( j = 0 ;   j < iTotalAbilitiesNum ; ++j )
	{
		ability = MainPlayer->Abilities(j);
		if( ability == NULL )
		{
			continue;
		}

		abilityId = ability->eventGetAbilityId();
		if( abilityId >=0 )
		{
			if(  ( ability->SpellCommonAttribute.CastType == E_spct_Active )
                /*&& ( ability->SpellCommonAttribute.BindSlot == currentActiveSlot )*/
				&& ( ability->SpellCommonAttribute.QTEType == E_spqte_None ) 
                && (  ability->ClientCommonAttribute.ShowOnUI == true ) )
			{
                if( k >= iOffset  &&
                     activeSlot < MaxActiveModifyCount )
                {
                    tObj = GetObject("ActiveModify")->GetObject( FString::Printf(TEXT("spell%d"), activeSlot )  );
                    SetSpellBtnByActiveId( tObj, activeSlot, abilityId, true );
                    ++activeSlot;
                }
                ++k;
			}	
		}
	}
	while( activeSlot < MaxActiveModifyCount )
	{
		tObj = GetObject("ActiveModify")->GetObject( FString::Printf(TEXT("spell%d"), activeSlot ) );
		SetSpellBtnByActiveId( tObj, activeSlot, 0 , true );
		++activeSlot;
	}
	//Refresh runes once
	OnSelecteActiveChanged( CurrentSelectedActiveId );

}

//Select the ActiveId of AllActives( abilityId ) that CurrentActiveSlot switches to
void UMMOUISkillPanel::SelectActive( int tSlotId,int abilityId )
{
	if( CurrentSelectedActiveSlot != tSlotId || CurrentSelectedActiveId != abilityId )
	{
		CurrentSelectedActiveSlot = tSlotId;
		CurrentSelectedActiveId = abilityId;
		OnSelecteActiveChanged( CurrentSelectedActiveId );
	}
}

//Refresh display of runes, skill points, etc.
void UMMOUISkillPanel::OnSelecteActiveChanged( int abilityId )
{
	//Panel is gone, do nothing
	if( !GetObject("ActiveModify") )
		return;
	//If the mental training panel is open, refresh it too
	UGFxObject* tNormalPractice = GetObject("ActiveModify")->GetObject("NormalPractice");
	UGFxObject* tFastPractice = GetObject("ActiveModify")->GetObject("FastPractice");
	if( tNormalPractice && tNormalPractice->GetBool("visible") )
		ASClickNormalPractice( false );
	else if( tFastPractice && tFastPractice->GetBool("visible") && ! IsInFastPractice )
		ASClickFastPractice( false );
	//Refresh all runes corresponding to abilityId
	int iRune = 0;
	//UINT Key = MMO::Facade::CombinateID( abilityId, level ,0 );
	//UMMOAbility* pAbility = MMOSkillManager::getSingleton().FindAbilityTemplate(Key);
	UMMOAbility* tMainActive = MainPlayer->GetMainAbilityById( abilityId );
	//If the currently equipped one is already a rune skill, select its main skill
	if( tMainActive )
	{
		//Confirm the equipped rune
		CurrentSelectedActiveRuneSlot = tMainActive->GetRuneSlot();
	}
	else
		CurrentSelectedActiveRuneSlot = 0;

	//Clear the selected rune bar
	TArray<struct FASValue> Arguments0;
	GetObject("ActiveModify")->Invoke("UnselecteAllRune", Arguments0 );

	//Set current skill points and skill upgrade progress
	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);
	//Icon'
	ASValue0.I = PC->SkillPoint;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);
	if( tMainActive )
		ASValue0.I = tMainActive->SkillPracticeProgress;
	else
		ASValue0.I = 0;
	ASValue0.Type = AS_Int;

	Arguments2.AddItem(ASValue0);
	GetObject("ActiveModify")->Invoke("SetSkillPoint", Arguments2 );

	UMMOAbility* tRune=NULL;
	//show all runes of the current ability
	int byRuneSlot = 0;
	if( tMainActive )
		byRuneSlot = tMainActive->GetRuneSlot();
	if( byRuneSlot <= 0 )
	{
		TArray<struct FASValue> Arguments0;
		GetObject("ActiveModify")->Invoke("UnselecteAllRune", Arguments0 );
	}

	while( tMainActive && iRune < tMainActive->SubRuneList.Num() ) 
	{
		tRune = tMainActive->SubRuneList( iRune );
		SetRuneBtnByAbility( tRune,iRune );
		++iRune;
	} 
	//Do not show the rest
	while( iRune < ActiveRuneCount )
	{
		SetRuneBtnByAbility( NULL, iRune );
        ++iRune;
	}

    //If an empty skill is selected, clear it and do not show the rune button
    UGFxObject* btnClear = GetObject("ActiveModify")->GetObject("btn_clear");
    if( !tMainActive && btnClear )
    {
        btnClear->SetVisible( false );
    }
    else if( btnClear )
    {
        btnClear->SetVisible( true );
    }

	//Current rune fragment count
	const TArray<INT> *pVecRuneDebrisIds = MMOSkillManager::getSingleton().GetRuneDebrisIds();
	if( pVecRuneDebrisIds )
	{
		for ( int i =0 ;i < pVecRuneDebrisIds->Num(); ++i )
		{
			UGFxObject* tObj = GetObject("ActiveModify")->GetObject( FString::Printf(TEXT("tF_runeCost%d"),i )  );
			if( !tObj )
				continue;
			UItem* item;
			item = NULL;
			if( MainPlayer->ItemManager != NULL)
				item = MainPlayer->ItemManager->GetItemById( (*pVecRuneDebrisIds)(i),E_CONSLOT_DEFAULT);
			//If this fragment exists, show the current owned count
			if( item )
				tObj->SetText( appItoa( item->DefaultDuration ) );
			else
				tObj->SetText( appItoa( 0 ) );
		}
	}
}

//Check whether the current rune can be activated
UBOOL UMMOUISkillPanel::CanActiveRune( UMMOAbility* tRune )
{
	//Durability is full, no activation needed
	if( tRune->RuneDurability >= tRune->RuneDurMax )
		return FALSE;
	//Current rune fragment count
	const TArray<INT> *pVecRuneDebrisIds = MMOSkillManager::getSingleton().GetRuneDebrisIds();
	UItem* item;
	if(  MainPlayer->ItemManager == NULL )
		return FALSE;
	if( !pVecRuneDebrisIds )
		return FALSE;
	for ( int i =0 ;i < pVecRuneDebrisIds->Num(); ++i )
	{
		if( tRune->vecRuneDebris(i).ItemCount > 0)
		{
			item = MainPlayer->ItemManager->GetItemById( (*pVecRuneDebrisIds)(i),E_CONSLOT_DEFAULT);
			//If this fragment exists, show the current owned count
			if( item && i < tRune->vecRuneDebris.Num() )
			{
				//Current shard count is less than the minimum activation amount required by the rune
				if( item->DefaultDuration < tRune->vecRuneDebris(i).ItemCount )
					return FALSE;
			}
			else
				return FALSE;
		}	
	}
	return TRUE;
}

//Set RuneBtn
void UMMOUISkillPanel::SetRuneBtnByAbility( UMMOAbility* tRune,int iRuneSlot )
{
	UGFxObject* tObj = GetObject("ActiveModify")->GetObject( FString::Printf(TEXT("btn_rune%d"),iRuneSlot )  );
	//UGFxObject* tObj = GetObject("ActiveModify")->GetObject( FString::Printf(TEXT("spell%d"),iRuneSlot )  );

	UGFxObject* tActiveBtn = GetObject("ActiveModify")->GetObject( FString::Printf(TEXT("btn_active%d"),iRuneSlot )  );
	if( !tObj  )
		return;
	if( tRune != NULL )
	{
		tObj->SetVisible( true );

        if( CurrentSelectedActiveRuneSlot == iRuneSlot+1 )
            tObj->SetBool("selected",TRUE );  
        else
            tObj->SetBool("selected",FALSE );  
		//tObj->SetString("RuneIconName",tRune->GetRuneIconName() );

		TArray<struct FASValue> Arguments0;
		FASValue ASValue0(EC_EventParm);
		//Icon'
		ASValue0.I = tRune->GetSkillGuiRuneID() ; //"img://MMO_UIIcon.Rune.2003_Rune2_"
		ASValue0.Type = AS_Int;
		Arguments0.AddItem(ASValue0);
		//AbilityName
		ASValue0.S = tRune->eventGetAbilityName() ; 
		ASValue0.Type = AS_String;
		Arguments0.AddItem(ASValue0);
		//Invoke
		tObj->Invoke("SetRuneIcon", Arguments0 );
	
		TArray<struct FASValue> Arguments;
		FASValue ASValue(EC_EventParm);
		//ActiveID
		ASValue.I = tRune->eventGetAbilityId();
		ASValue.Type = AS_Int;
		Arguments.AddItem(ASValue);
		//RuneDurability
		ASValue.I = tRune->RuneDurability;
		ASValue.Type = AS_Int;
		Arguments.AddItem(ASValue);
		//RuneDurMax
		ASValue.I = tRune->RuneDurMax;
		ASValue.Type = AS_Int;
		Arguments.AddItem(ASValue);
		//CanActiveRune
		ASValue.B = CanActiveRune( tRune );
		ASValue.Type = AS_Boolean;
		Arguments.AddItem(ASValue);
		//Invoke
		tObj->Invoke("SetCurDur", Arguments );
	}
	else
	{
		tObj->SetVisible( false );
		if( tObj->GetObject("tF_runeName") )
			tObj->GetObject("tF_runeName")->SetText( "" );
		tActiveBtn->SetVisible( false );
	}
}

//Click to equip rune
void UMMOUISkillPanel::EquipRune( int runeSlotId)
{
	/*UMMOAbility* tMainActive = MainPlayer->GetMainAbilityById( CurrentActives( CurrentSelectedActiveSlot ).ActiveId );
	if ( !tMainActive )
		return;
	UMMOAbility* tRune=NULL;
	//show all runes of the current ability
	if( runeSlotId < tMainActive->SubRuneList.Num() ) 
		tRune = tMainActive->SubRuneList( runeSlotId );

	if( CanActiveRune( tRune ) )*/
	CurrentSelectedActiveRuneSlot = runeSlotId +1 ;
	//Send the message after confirmation
	//SendEquipActiveRune( CurrentSelectedActiveSlot,runeSlotId );
}

//Click to activate rune 0~4
void UMMOUISkillPanel::ActivateRune( int runeSlotId)
{
	/*int iOffset = NowActivePageNum  * MaxActiveModifyCount;
	if(  CurrentSelectedActiveSlot + iOffset >= MainPlayer->Abilities.Num() )
		return;*/

	UMMOAbility* tMainActive = MainPlayer->GetMainAbilityById( CurrentSelectedActiveId );
	if ( !tMainActive )
		return;
	UMMOAbility* tRune=NULL;
	//show all runes of the current ability
	if( runeSlotId >= 0 && runeSlotId < tMainActive->SubRuneList.Num() ) 
		tRune = tMainActive->SubRuneList( runeSlotId );
	if( !tRune )
		return;
	//Local Rune index is 0~4, server uses 1~5
	if( CanActiveRune( tRune ) )
		SendActivateRune( CurrentSelectedActiveId, runeSlotId +1  );
}

//Cancel rune equip
void UMMOUISkillPanel::CleanRune( )
{
	SendEquipActiveRune( CurrentSelectedActiveSlot, 0 );
}

//Whether Active is already equipped
int UMMOUISkillPanel::IsActiveAlreadyEquiped( int abilityId )
{
	int i;
	for( i = 0; i < CurrentActives.Num() ; ++i )
	{
		if( CurrentActives( i ).ActiveId ==  abilityId )
			return i;
	}
	return -1;
}

//Tab key directly switches left/right mouse button abilities
void UMMOUISkillPanel::ReplaceHotSkill( )
{
    //Abilities are bound to Slots, Tab switching is removed
    return;

	/*if( Validate() && MMO::GetCurrentGameState() == EGameState_InGame )
	{
		int FirsabilityId  =  KeyMapManager->GetCmdIDBySlotID( GetRealActiveRealSlotId(0) );
		int SecondActiveId =  KeyMapManager->GetCmdIDBySlotID( GetRealActiveRealSlotId(1) );
        
        //EmptyCmd may appear
        if( FirsabilityId < 0 )
        {
            FirsabilityId = 0;
        }

        if( SecondActiveId < 0 )
        {
            SecondActiveId = 0;
        }

        //Switch ability
        SendEquipActive( 0,  0 );
        SendEquipActive( 1,  FirsabilityId );
        SendEquipActive( 0,  SecondActiveId );
	}*/
}


//Ability upgrade---------------------------------------------------------------------------------------start

//Confirm aborting ability upgrade
void UMMOUISkillPanel::StopSkillPracticeOk(UGFxObject* mc)
{
	HasAskStopSkillPractice = false;
	if( mc != NULL )
	{
		INT ActiveId = mc->GetInt("iteminfoid");
		if( NowNormalPracticeActiveId == ActiveId )
			IsRequestCloseNormalSkillPractice = TRUE;
		else if(  NowFastPracticeActiveId == ActiveId )
			IsRequestCloseFastSkillPractice = TRUE;
		ASStopPractice(ActiveId);
	}
}

//Cancel aborting ability upgrade
void UMMOUISkillPanel::StopSkillPracticeCancel(UGFxObject* mc)
{
	HasAskStopSkillPractice = false;
}

//Set mind practice panel
void UMMOUISkillPanel::ASClickNormalPractice(UBOOL IsToggle  )
{
	if( !GetObject("ActiveModify") )
		return;
	UGFxObject* tObj = GetObject("ActiveModify")->GetObject("NormalPractice");
	if( !tObj )
		return;
	UMMOAbility*  ability;
	ability = MainPlayer->GetMainAbilityById( CurrentSelectedActiveId );
	if( !ability  )
		return;
	//Whether currently in soul practice and it is the currently selected ability
	if( ability->SkillPracticeType == SkillPT_Fast  )
	{
		if( !HasAskStopSkillPractice )
		{
			//Prompt whether to cancel the previous soul practice state
			FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.StopSkillPracticeFast") ),
				*(ability->ClientCommonAttribute.AbilityName) );

			FScriptDelegate funcOk(EC_EventParm);
			funcOk.FunctionName = "StopSkillPracticeOk";
			funcOk.Object = this;
			FScriptDelegate funcCancel(EC_EventParm);
			funcCancel.FunctionName = "StopSkillPracticeCancel";
			funcCancel.Object = this;
			MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg , ability->eventGetAbilityId() ,funcOk,  funcCancel );
		}
		return;
	}
	//Not in soul practice but the soul practice panel is open and is the current ability, so close it directly
	if( NowRequestFastPracticeActiveId == CurrentSelectedActiveId && GetObject("ActiveModify")->GetObject("FastPractice") )
		GetObject("ActiveModify")->GetObject("FastPractice")->SetVisible( false );

	//Repeated click Toggle
	if( CurrentSelectedActiveId == NowRequestNormalPracticeActiveId && IsToggle && tObj->GetBool("visible") )
	{
		TArray<struct FASValue> Arguments;
		FASValue ASValue(EC_EventParm);
		Arguments.AddItem(ASValue);
		tObj->Invoke("Close", Arguments );
	}
	
	NowRequestNormalPracticeActiveId = CurrentSelectedActiveId;
	//Set mind practice panel

	//Set ItemSlot
	if( tObj->GetObject("ActiveSlot") )
	{
		GFXPlayer->eventCallSetItemData( tObj->GetObject("ActiveSlot") , 0 , ability->GetAbilityIconName() , ability->eventGetAbilityId() , ESPType_SkillPanel,TEXT("") );
		tObj->GetObject("ActiveSlot")->SetInt( "SlotType" , ESPType_SkillPanel );
	}
	/*
	SetFastPracticeData
	( 
	inActiveId:int,
	HasFastPracticing:Boolean,
	TimeLeft:String,
	inPercent:int,
	inType:int,
	IsPracticing:Boolean 
	)
	*/
	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);
	//inActiveId ability ID
	ASValue0.I = ability->eventGetAbilityId();
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);
	//HasFastPracticing whether another soul practice is in progress
	ASValue0.B = NowFastPracticeActiveId ?true:false;
	ASValue0.Type = AS_Boolean;
	Arguments2.AddItem(ASValue0);
	//TimeLeft remaining upgrade time, refreshed in Tick
	ASValue0.S = StrTimeLeft +  "";  //StrTimeLeft + MMOFacade::getSingleton().TimeDisplay( (DWORD)ability->SkillPracticeEndTime );
	ASValue0.Type = AS_String;
	Arguments2.AddItem(ASValue0);
	//inPercent ability proficiency
	ASValue0.I = ability->SkillPracticeProgress;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);
	//inType practice Type
	ASValue0.I = ability->SkillPracticeCostType > 0 ? ability->SkillPracticeCostType : 1 ;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);
	//IsPracticing whether practicing
	ASValue0.B = ability->SkillPracticeCostType > 0 ? true:false;
	ASValue0.Type = AS_Boolean;
	Arguments2.AddItem(ASValue0);
	tObj->Invoke("SetNormalPracticeData", Arguments2 );

	//Refresh state, may not have started but already has proficiency
	NowNormalPracticeActiveId = CurrentSelectedActiveId;
	NetSkillProgressChange( CurrentSelectedActiveId,ability->SkillPracticeProgress );

}

//Set soul practice panel
void UMMOUISkillPanel::ASClickFastPractice(UBOOL IsToggle  )
{
	if( !GetObject("ActiveModify") )
		return;
	UGFxObject* tObj = GetObject("ActiveModify")->GetObject("FastPractice");
	if( !tObj )
		return;
	UMMOAbility*  ability;
	ability = MainPlayer->GetMainAbilityById( CurrentSelectedActiveId );
	if( !ability )
		return;
	//Whether currently in soul practice and it is the currently selected ability
	if(  ability->SkillPracticeType == SkillPT_Fast  )
	{
		//Prompt whether to cancel the previous soul practice state
		if( !HasAskStopSkillPractice )
		{
			//Prompt whether to cancel the previous soul practice state
			FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.StopSkillPracticeFast") ),
				*(ability->ClientCommonAttribute.AbilityName ) );
			FScriptDelegate funcOk(EC_EventParm);
			funcOk.FunctionName = "StopSkillPracticeOk";
			funcOk.Object = this;
			FScriptDelegate funcCancel(EC_EventParm);
			funcCancel.FunctionName = "StopSkillPracticeCancel";
			funcCancel.Object = this;
			MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg , ability->eventGetAbilityId() ,funcOk,  funcCancel );
		}
		return;
	}
	//If in mind practice
	if( ability->SkillPracticeType == SkillPT_Noraml )
	{
		//Prompt whether to cancel the previous mind practice state
		if( !HasAskStopSkillPractice )
		{
			FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.StopSkillPracticeNormal") ),
				*(ability->ClientCommonAttribute.AbilityName ) );
			FScriptDelegate funcOk(EC_EventParm);
			funcOk.FunctionName = "StopSkillPracticeOk";
			funcOk.Object = this;
			FScriptDelegate funcCancel(EC_EventParm);
			funcCancel.FunctionName = "StopSkillPracticeCancel";
			funcCancel.Object = this;
			MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg , ability->eventGetAbilityId() ,funcOk,  funcCancel );
		}
		return;
	}
	//Not in mind practice but the mind practice panel is open and is the current ability, so close it directly
	if( NowRequestNormalPracticeActiveId == CurrentSelectedActiveId && GetObject("ActiveModify")->GetObject("NormalPractice") )
		GetObject("ActiveModify")->GetObject("NormalPractice")->SetVisible( false );
	
	//Repeated click Toggle
	if( CurrentSelectedActiveId == NowRequestFastPracticeActiveId && IsToggle && tObj->GetBool("visible") )
	{
		TArray<struct FASValue> Arguments;
		FASValue ASValue(EC_EventParm);
		Arguments.AddItem(ASValue);
		tObj->Invoke("Close", Arguments );
	}

	NowRequestFastPracticeActiveId = CurrentSelectedActiveId;

	if( tObj->GetObject("ActiveSlot") )
	{
		GFXPlayer->eventCallSetItemData( tObj->GetObject("ActiveSlot") , 0 , ability->GetAbilityIconName() , ability->eventGetAbilityId() , ESPType_SkillPanel,TEXT("") );
		tObj->GetObject("ActiveSlot")->SetInt( "SlotType" , ESPType_SkillPanel );
	}
	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);
	//inActiveId ability ID
	ASValue0.I = ability->eventGetAbilityId();
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);
	//inPercent ability proficiency
	ASValue0.I = ability->SkillPracticeProgress;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);
	tObj->Invoke("SetFastPracticeData", Arguments2 );

	//Refresh state
	NowFastPracticeActiveId = CurrentSelectedActiveId;
	NetSkillProgressChange( CurrentSelectedActiveId,ability->SkillPracticeProgress );
	
}

//Select mind practice type
void UMMOUISkillPanel::ASChooseNormalParcticeType( int NPType )
{
	NowNormalPracticeType = NPType;
}

//Select soul practice type
void UMMOUISkillPanel::ASChooseFastPracticeType( int FPType )
{
    NowFastPracticeType = FPType;
}

//Start heart training
void UMMOUISkillPanel::ASStartNormalPractice()
{
	if( !Validate() )
		return;
	UMMOAbility*  ability;
	ability = MainPlayer->GetMainAbilityById( NowRequestNormalPracticeActiveId );
	if( ability )
	{
		//@Todo max ability level macro
		if( 0 == ability->LvUpRequirePoints && ability->SpellCommonAttribute.Level >= 5 )
		{
			//LvUpRequirePoints means the current ability is at max level and needs no further upgrade
			//Prompt whether to cancel the previous soul practice state
			FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillAlreadyMax") ),
				*(ability->ClientCommonAttribute.AbilityName) );
			FScriptDelegate funcNULL(EC_EventParm);
			MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg , ability->eventGetAbilityId() ,funcNULL,  funcNULL );
			return;
		}
		CMessageHandler::GetInstance().Send_StartNormalPractice( NowNormalPracticeType ,NowRequestNormalPracticeActiveId ) ; 
	}
}

//Start soul training
void UMMOUISkillPanel::ASStartFastPractice( )
{
	if( !Validate() )
		return;
	UMMOAbility*  ability;
	ability = MainPlayer->GetMainAbilityById( NowRequestFastPracticeActiveId );
	if( ability )
	{
		//@Todo max ability level macro
		if( 0 == ability->LvUpRequirePoints && ability->SpellCommonAttribute.Level >= 5 )
		{
			//LvUpRequirePoints means the current ability is at max level and needs no further upgrade
			//Prompt whether to cancel the previous soul practice state
			FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillAlreadyMax") ),
				*(ability->ClientCommonAttribute.AbilityName) );
			FScriptDelegate funcNULL(EC_EventParm);
			MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg , ability->eventGetAbilityId() ,funcNULL,  funcNULL );
			return;
		}
		CMessageHandler::GetInstance().Send_StartFastPractice(  NowFastPracticeType ,NowRequestFastPracticeActiveId ) ; 
	}
}

//Soul training use medicine
void UMMOUISkillPanel::ASUseFastPracticeItem( int FPItem )
{
	//Start heart training
	CMessageHandler::GetInstance().Send_SpeedUpFastPractice( FPItem ,NowFastPracticeActiveId ) ; 
}

//Abort the ability currently being practiced
void UMMOUISkillPanel::ASStopPractice( INT inActiveId )
{
	CMessageHandler::GetInstance().Send_StopPractice( inActiveId  ) ; 
}

//Abort and close the ability upgrade panel
void UMMOUISkillPanel::ASStopPracticeAndClose( int inActiveId,BYTE spt )
{
	if( spt == SkillPT_Noraml )
		IsRequestCloseNormalSkillPractice = TRUE;
	else if(  spt == SkillPT_Fast )
		IsRequestCloseFastSkillPractice = TRUE;
	ASStopPractice( inActiveId );
}

//Start ability upgrade
void UMMOUISkillPanel::NetStartSkillPractice(int inActiveId,int byPracticeType,int	nInterval,int nEndTime )
{
	if( !GetObject("ActiveModify") )
		return;
	UMMOAbility*  ability;
	ability = MainPlayer->GetMainAbilityById( inActiveId );
	if( ability )
	{
		//ability->SkillProgress = ;
		ability->SkillPracticeType = byPracticeType; 
		ability->SkillPracticeEndTime = (DWORD)nEndTime ;

	}

	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);
	//Whether to start
	ASValue0.B = true;
	ASValue0.Type = AS_Boolean;
	Arguments2.AddItem(ASValue0);

	NowPracticeType = byPracticeType;
	if( SkillPT_Noraml == NowPracticeType )
	{
		
		NowNormalPracticeActiveId = inActiveId;
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("NormalPractice");
		if( tObj )
		{
			tObj->Invoke("SetStarted", Arguments2 );
		}

		if( ability )
			ability->SkillPracticeCostType = NowNormalPracticeType; //last type just selected

	}
	else if( SkillPT_Fast == NowPracticeType )
	{
		IsInFastPractice = true;
		NowFastPracticeActiveId = inActiveId;

		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("FastPractice");
		if( tObj )
		{
			tObj->Invoke("SetStarted", Arguments2 );
		}

		if( ability )
			ability->SkillPracticeCostType = NowFastPracticeType; //last type just selected

	}
}

//End ability upgrade
void UMMOUISkillPanel::NetStopSkillPractice(int inActiveId)
{
    if( !Validate() )
        return;
	if( !GetObject("ActiveModify") )
		return;

	UMMOAbility*  ability = MainPlayer->GetMainAbilityById( inActiveId );
	if( ability )
	{
        ability->SkillPracticeType = 0;
		ability->SkillPracticeCostType = 0;
	}

	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);
	ASValue0.B = false;
	ASValue0.Type = AS_Boolean;
	Arguments2.AddItem(ASValue0);

	if( inActiveId == NowNormalPracticeActiveId )
	{
		NowNormalPracticeActiveId = 0;
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("NormalPractice");
		if( tObj )
		{
			tObj->Invoke("SetStarted", Arguments2 );

			if( IsRequestCloseNormalSkillPractice )
			{
				tObj->Invoke("ClosePanel", Arguments2 );
				IsRequestCloseNormalSkillPractice = FALSE;
			}
		}
	}
	else if( inActiveId == NowFastPracticeActiveId )
	{
		IsInFastPractice = false;
		NowFastPracticeActiveId = 0;
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("FastPractice");
		if( tObj )
		{
			tObj->Invoke("SetStarted", Arguments2 );

			if( IsRequestCloseFastSkillPractice )
			{
				tObj->Invoke("ClosePanel", Arguments2 );
				IsRequestCloseFastSkillPractice = FALSE;
			}
		}
	}
}

//Update mind practice panel
void UMMOUISkillPanel::UpdateNormalPractice( )
{
	if( !GetObject("ActiveModify") )
		return;
	UGFxObject* tObj = GetObject("ActiveModify")->GetObject("NormalPractice");
	if( !tObj )
		return;
	if( !tObj->GetBool("visible") )
		return;
	if( !tObj->GetObject("timeLeft_mc") || ! tObj->GetObject("timeLeft_mc")->GetObject("tF_timeLeft") )
		return;
	UGFxObject* tTimeLeft = tObj->GetObject("timeLeft_mc")->GetObject("tF_timeLeft");
	UMMOAbility*  ability;
	ability = MainPlayer->GetMainAbilityById( CurrentSelectedActiveId );
	if( !ability  )
		return;
	int NowTime = time(NULL);
	int TimeLeft = ability->SkillPracticeEndTime - NowTime;
	//Calculate remaining time
	if( TimeLeft <= 0 )
	{
		TimeLeft = 0;
		//ability->SkillPracticeEndTime = 0;
	}
	FString sSkillPracticeLeftTime =  StrTimeLeft + MMOFacade::getSingleton().TimeDisplay( (DWORD)TimeLeft );
	tTimeLeft->SetText( sSkillPracticeLeftTime  );

}

//PostAdvance
void UMMOUISkillPanel::PostAdvance(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	if( ElapsedTime > UpdateInterval  && MainPlayer )
	{
		UMMOAbility*  ability;
		ability = MainPlayer->GetMainAbilityById( CurrentSelectedActiveId );
		if( !ability  )
			return;
		if( SkillPT_Noraml == ability->SkillPracticeType && ability->SkillPracticeEndTime > 0 )
		{
			UpdateNormalPractice( );
		}
		ElapsedTime = 0;
	}
}

//Ability proficiency reminder
void UMMOUISkillPanel::NetSkillProgressChange( int inActiveId,int nProgress )
{
	if( !Validate() )
		return;
	if( !GetObject("ActiveModify") )
		return;
	UMMOAbility*  ability = MainPlayer->GetMainAbilityById( inActiveId );
	if( !ability )
		return;
	ability->SkillPracticeProgress = nProgress;

	//Refresh upgrade progress display
	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);
	ASValue0.I = ability->SpellCommonAttribute.Level;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);

	ASValue0.I = ability->SkillPracticeProgress ;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);

	ASValue0.I = ability->LvUpRequirePoints ;
	ASValue0.Type = AS_Int;
	Arguments2.AddItem(ASValue0);

	//During mind practice, growth of other abilities may be received; do not refresh the current panel
	if(  inActiveId == NowNormalPracticeActiveId )
	{
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("NormalPractice");
		if( tObj )
		{
			tObj->Invoke("SetPercent", Arguments2 );
		}
	}
	if(  inActiveId == NowFastPracticeActiveId  )
	{
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("FastPractice");
		if( tObj )
		{
			tObj->Invoke("SetPercent", Arguments2 );
		}
	}
}

//MoneyChange ability points are now also a currency
void UMMOUISkillPanel::PlayerMoneyChange(BYTE MoneyType, INT DeltaMoney)
{
    if ( MONEYTYPE_SOUL_PTS == MoneyType )
    {
        PC->SkillPoint = MMOFacade::getSingleton().GetPlayerMoney(MoneyType);
        NetSkillPointChangeNotify( );
    }
}

//Ability upgrade hint
void UMMOUISkillPanel::NetSkillPointChangeNotify( )
{
	if( !Validate() )
		return;
	if( IsVisible && PC )
	{
		//Show current skill points
		if(  GetObject("SpellConsole") && GetObject("SpellConsole")->GetObject("tF_spellPoint") )
			GetObject("SpellConsole")->GetObject("tF_spellPoint")->SetText( appItoa( PC->SkillPoint ) );

		UMMOAbility*  ability = MainPlayer->GetMainAbilityById( CurrentSelectedActiveId );
		if( ability )
		{
			//Set current skill points and skill upgrade progress
			TArray<struct FASValue> Arguments2;
			FASValue ASValue0(EC_EventParm);
			ASValue0.I = PC->SkillPoint;
			ASValue0.Type = AS_Int;
			Arguments2.AddItem(ASValue0);
			ASValue0.I = ability->SkillPracticeProgress;
			ASValue0.Type = AS_Int;
			Arguments2.AddItem(ASValue0);
			GetObject("ActiveModify")->Invoke("SetSkillPoint", Arguments2 );

		}
	}
}

//Ability upgrade feature enabled
void UMMOUISkillPanel::EnableSkillUp( UBOOL isEnable )
{
    if( !GetObject("ActiveModify") )
    {
        return;
    }
    TArray<struct FASValue> Arguments;
    FASValue ASValue0(EC_EventParm);
    ASValue0.B = isEnable;
    ASValue0.Type = AS_Boolean;

    Arguments.AddItem(ASValue0);
    GetObject("ActiveModify")->Invoke("UcEnableSkillUp", Arguments );
}

//Ability upgrade hint
void UMMOUISkillPanel::NetSkillLevelUp( int inActiveId,int byLevel )
{
	if( !GetObject("ActiveModify") )
		return;

	//UMMOAbility*  ability = MainPlayer->GetMainAbilityById( inActiveId );

	//if( !ability )
	//	return;

 //   //Refresh the current level first
	//ability->SpellCommonAttribute.Level = byLevel;

	////Related system prompts
	//FString sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillLvUpNotify") ),
	//	*(ability->ClientCommonAttribute.AbilityName), byLevel );
	//MMOUI::GetUIManager()->delegateAddScreenMessage( sMsg );
	//MMOChatManager::getSingleton().AddSystemMessage( sMsg );

    //Refresh current ability level
    //NetSkillProgressChange( inActiveId, ability->LvUpRequirePoints );

	//and stop the upgrade state display
	TArray<struct FASValue> Arguments2;
	FASValue ASValue0(EC_EventParm);

	//Whether to start
	ASValue0.B = false;
	ASValue0.Type = AS_Boolean;
	Arguments2.AddItem(ASValue0);

	if( inActiveId == NowNormalPracticeActiveId )
	{
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("NormalPractice");
		if( tObj )
		{
			tObj->Invoke("SetStarted", Arguments2 );
		}
	}
	else if( inActiveId == NowFastPracticeActiveId )
	{
		UGFxObject* tObj = GetObject("ActiveModify")->GetObject("FastPractice");
		if( tObj )
		{
			tObj->Invoke("SetStarted", Arguments2 );
		}
	}

}
//Ability upgrade---------------------------------------------------------------------------------------end


//Confirm submit Modify
void UMMOUISkillPanel::ConfirmActive( )
{
	UGFxObject*  tObj = NULL;
	int OtherRef;
	if( CurrentActives( CurrentActiveSlot ).ActiveId != CurrentSelectedActiveId )
	{
		OtherRef = IsActiveAlreadyEquiped( CurrentSelectedActiveId ) ;
		if( OtherRef >= 0 )
		{
			// the corresponding check mark will also be overwritten
			CurrentActives( OtherRef ).IsChanged = true; 
			CurrentActives( OtherRef ).ActiveId = 0;
		}
		CurrentActives( CurrentActiveSlot ).ActiveId = CurrentSelectedActiveId; 
		CurrentActives( CurrentActiveSlot ).IsChanged = true; 
		//RefreshActiveModify();
	}

/*
	int i;
	for( i = 0; i < CurrentActives.Num() ; ++i )
	{
		if( CurrentActives( i ).ActiveId != RealActives( i ).ActiveId  )
			SendEquipActive( i, CurrentActives( i ).ActiveId );
	}*/

	sSpellChange tSC;

	SpellChangeDeque tChangeMsg;
	SpellChangeDeque::iterator tIter;
	int i;
	for( i = 0; i < CurrentActives.Num() ; ++i )
	{
		if( CurrentActives( i ).ActiveId != RealActives( i ).ActiveId  )
		{
			tSC.SlotId = i;
			tSC.AbilityId = CurrentActives( i ).ActiveId  ;
			if( tSC.AbilityId == 0 )
				tChangeMsg.push_front( tSC );
			else
				tChangeMsg.push_back( tSC );
		}
	}
	for( tIter = tChangeMsg.begin(); tIter != tChangeMsg.end() ; ++ tIter )
	{
		/*//If a rune was selected
		if( tIter->AbilityId == CurrentSelectedActiveId && CurrentSelectedActiveRuneSlot > 0 )
		{
			SendEquipActiveRune( CurrentSelectedActiveSlot,CurrentSelectedActiveRuneSlot  );
			continue;
		}*/
		SendEquipActive( tIter->SlotId,  tIter->AbilityId );
	}
	
	//If a rune was selected
	if( CurrentSelectedActiveRuneSlot > 0 )
		SendEquipActiveRune( CurrentActiveSlot,CurrentSelectedActiveRuneSlot   ); //Local Rune index is 0~4, server uses 1~5
	
}

void UMMOUISkillPanel::ActiveLastPage( )
{
	if( NowActivePageNum > 0 )
	{
		--NowActivePageNum;
		RefreshActiveModify();
	}	
}

void UMMOUISkillPanel::ActiveNextPage( )
{
	if( NowActivePageNum < TotalActivePageNum -1 )
	{
		++NowActivePageNum;
		RefreshActiveModify();
	}
}

//==================================================RefreshPassiveModify()====================================================== 


//Set PassiveBtn
void UMMOUISkillPanel::SetSpellBtnByPassiveId(class UGFxObject* tObj,INT SlotId,INT abilityId,UBOOL isSubCtrl)
{
#if M_TEMP_ENABLE_UI_Skill_CODE

	UMMOAbility*  ability;
	if( tObj == NULL )
	{
		//`log("CurrentSpell Passive Active PassiveSkill"$SlotId@"can't found");
		return;
	}
	//Set Btn according to PassiveId
	ability = MainPlayer->GetAbilityById( abilityId );
	//`log("CurrentSpell Passive Add Passive ()"@tObj@tObj->GetObject("ItemSlot")@"PassiveId::"@abilityId );
	if( ability != NULL && ability->eventGetAbilityId() >=0  && ( ability->SpellCommonAttribute.CastType == E_spct_Passive ) )
	{
		tObj->SetVisible(true);
		tObj->SetBool("enabled",true);
		if( tObj->GetObject("ItemSlot") )
		{
			//FString cs = TEXT("skill");
			//eventSetItem(tObj->GetObject("ItemSlot") ,0 ,ability->GetAbilityIconName() ,ability->eventGetAbilityId() ,FString( TEXT("skill") ) );
			GFXPlayer->eventCallSetItemData( tObj->GetObject("ItemSlot") , 0 , ability->GetAbilityIconName() , ability->eventGetAbilityId() , ESPType_SkillPanel,TEXT("") );
			//GFXPlayer->eventASSetSlotShowData(tObj->GetObject("ItemSlot") ,0 ,ability->GetAbilityIconName() ,ability->eventGetAbilityId() );
			tObj->GetObject("ItemSlot")->SetInt( "SlotType" , ESPType_SkillPanel );
		}
			//tF_SpellName skill name
		if( tObj->GetObject("tF_SpellName") )
			tObj->GetObject("tF_SpellName")->SetText( ability->ClientCommonAttribute.AbilityName );
		if( tObj->GetObject("lock_mc") )
			tObj->GetObject("lock_mc")->SetVisible( false );
		//`log("CurrentSpell Passive set SpellSlotId:"@SlotId );
		if( isSubCtrl )
		{
			//If this skill is already equipped, then
			if( tObj->GetObject("EquipedCheck") )
			{
				if( IsPassiveAlreadyEquiped( abilityId ) >= 0 )
					tObj->GetObject("EquipedCheck")->SetVisible( true );
				else
					tObj->GetObject("EquipedCheck")->SetVisible( false );
			}
			//Record the corresponding abilityId
			tObj->SetInt("PassiveId",abilityId );
		}
// 		else if( SlotId > 0 && SlotId < CurrentPassives.Num() )
// 			CurrentPassives(SlotId).PassiveId = abilityId;
	}
	else
	{
		//`log("CurrentSpell Passive Active  is NULL()"@tObj@tObj->GetObject("ItemSlot")@SlotId );
		//tObj->SetVisible(false);
		//tObj->SetBool("enabled",false);
		if( tObj->GetObject("ItemSlot") )
			eventSetItem(tObj->GetObject("ItemSlot") ,0,"",-1 );
		//tF_SpellName skill name
		if( tObj->GetObject("tF_SpellName") )
			tObj->GetObject("tF_SpellName")->SetText( "" );
		if( isSubCtrl )
		{
			tObj->SetBool("enabled",false);
			if( tObj->GetObject("EquipedCheck") )
				tObj->GetObject("EquipedCheck")->SetVisible( false );
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( true );
		}
		else
		{
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( false );
		}
	}
#endif
}

//Select current CurrentPassiveSlot
void UMMOUISkillPanel::SetCurrentPassive( int passiveSlotId )
{
	PageJump( ESkill_Passive );
	if( CurrentPassiveSlot != passiveSlotId )
	{
		CurrentPassiveSlot = passiveSlotId;
		RefreshPassiveModify();
	}
}

//RefreshPassiveModify
void UMMOUISkillPanel::RefreshPassiveModify()
{
	int j,k;
	int abilityId;
	UGFxObject*  tObj = NULL;
	UMMOAbility*  ability;

	//`log("==============================RefreshPassiveModify() Opended=================================================");
	if( !Validate() )
		return;
	//--------------------------- Select skill panel ---------------------------------------------------------

	if( ! GetObject("PassiveModify") )
		return;

	//Currently equipped Passive ability
	for( int i =0 ;i < CurrentPassives.Num(); ++i )
	{
		tObj = GetObject("PassiveModify")->GetObject( FString::Printf(TEXT("CurrentSpellSlot%d"),i ) );
		SetSpellBtnByPassiveId( tObj,i , CurrentPassives(i).PassiveId ,false );
	}

	//All Passive abilities learned by the player's current class
	k = 0;
	for ( j = 0;   j < MainPlayer->Abilities.Num()  ; ++j )
	{
		ability = MainPlayer->Abilities(j);
		if( ability != NULL ){
			continue;
		}

		abilityId = ability->eventGetAbilityId();
		if( abilityId >=0 )
		{
			if( ( k < MaxPassiveModifyCount ) && ( ability->SpellCommonAttribute.CastType == E_spct_Passive )
				&& (  ability->ClientCommonAttribute.ShowOnUI == true ) )
			{
				tObj = GetObject("PassiveModify")->GetObject( FString::Printf(TEXT("spell%d"),k )  );
				SetSpellBtnByPassiveId( tObj,k,abilityId,true );
				++k;
			}	
		}
	}
	while( k< MaxPassiveModifyCount )
	{
		tObj = GetObject("PassiveModify")->GetObject( FString::Printf(TEXT("spell%d"),k ) );
		SetSpellBtnByPassiveId( tObj,k,0 ,true );
		++k;
	}
}

//Select the PassiveId corresponding to AllPassives( abilityId ) to switch CurrentPassiveSlot to
void UMMOUISkillPanel::SelectPassive( int tSlotId,int abilityId )
{
	UGFxObject*  tObj = NULL;
	int OtherRef;
	if( CurrentPassives( CurrentPassiveSlot ).PassiveId != abilityId )
	{
		OtherRef = IsPassiveAlreadyEquiped( abilityId ) ;
		if( OtherRef >= 0 )
		{
			// the corresponding check mark will also be overwritten
			CurrentPassives( OtherRef ).IsChanged = true; 
			CurrentPassives( OtherRef ).PassiveId = 0;
		}
		CurrentPassives( CurrentPassiveSlot ).PassiveId = abilityId; 
		CurrentPassives( CurrentPassiveSlot ).IsChanged = true; 
		RefreshPassiveModify();
	}
}

//Whether Passive is already equipped
int UMMOUISkillPanel::IsPassiveAlreadyEquiped( int abilityId )
{
	int i;
	for( i = 0; i < CurrentPassives.Num() ; ++i )
	{
		if( CurrentPassives( i ).PassiveId ==  abilityId )
			return i;
	}
	return -1;
}

//Confirm submit Modify
void UMMOUISkillPanel::ConfirmPassive( )
{
/*
	int i;
	for( i = 0; i < CurrentPassives.Num() ; ++i )
	{
		if( CurrentPassives( i ).PassiveId != RealPassives( i ).PassiveId  )
			SendEquipPassive( i, CurrentPassives( i ).PassiveId );
	}*/
	sSpellChange tSC;
	SpellChangeDeque tChangeMsg;
	SpellChangeDeque::iterator tIter;
	int i;
	for( i = 0; i < CurrentPassives.Num() ; ++i )
	{
		if( CurrentPassives( i ).PassiveId != RealPassives( i ).PassiveId  )
		{
			tSC.SlotId = i;
			tSC.AbilityId = CurrentPassives( i ).PassiveId  ;
			if( tSC.AbilityId == 0 )
				tChangeMsg.push_front( tSC );
			else
				tChangeMsg.push_back( tSC );
		}
	}
	for( tIter = tChangeMsg.begin(); tIter != tChangeMsg.end() ; ++ tIter )
	{
		SendEquipPassive( tIter->SlotId,  tIter->AbilityId );
	}
}

//==================================================RefreshAvartaModify()====================================================== 


//Set AvartaBtn
void UMMOUISkillPanel::SetSpellBtnByAvartaId(class UGFxObject* tObj,INT SlotId,INT abilityId,UBOOL isSubCtrl)
{
	UMMOAbility*  ability;
	//Set Btn according to AvartaId
	ability = MainPlayer->GetAbilityById( abilityId );
	SetSpellBtnByAvarta( tObj,SlotId, ability,isSubCtrl);
}

//Set AvartaBtn
void UMMOUISkillPanel::SetSpellBtnByAvarta(class UGFxObject* tObj,INT SlotId, UMMOAbility*  ability,UBOOL isSubCtrl,UBOOL isRuneCtrl )
{
	if( tObj == NULL )
	{
		//`log("CurrentSpell Avarta Active AvartaSkill"$SlotId@"can't found");
		return;
	}
	//Avarta ability is a kind of active ability
	if( ability != NULL && ability->eventGetAbilityId() >=0  )
	{
		INT abilityId = ability->eventGetAbilityId();
		tObj->SetVisible(true);
		tObj->SetBool("enabled",true);
		if( tObj->GetObject("ItemSlot") )
		{
			//FString cs = TEXT("skill");
			//eventSetItem(tObj->GetObject("ItemSlot") ,0 ,ability->GetAbilityIconName() ,ability->eventGetAbilityId() ,FString( TEXT("skill") ) );
			GFXPlayer->eventCallSetItemData( tObj->GetObject("ItemSlot") , 0 , ability->GetAbilityIconName() , ability->eventGetAbilityId() , ESPType_SkillPanel,TEXT("") );
			//GFXPlayer->eventASSetSlotShowData(tObj->GetObject("ItemSlot") ,0 ,ability->GetAbilityIconName() ,ability->eventGetAbilityId()/* ,cs*/);
			tObj->GetObject("ItemSlot")->SetInt( "SlotType" , ESPType_SkillPanel );
		}
			//tF_SpellName skill name
		if( tObj->GetObject("tF_SpellName") )
		{
			tObj->GetObject("tF_SpellName")->SetVisible( true );
			tObj->GetObject("tF_SpellName")->SetText( ability->ClientCommonAttribute.AbilityName );
		}
		//`log("CurrentSpell Avarta set SpellSlotId:"@SlotId );
		if( tObj->GetObject("lock_mc") )
			tObj->GetObject("lock_mc")->SetVisible( false );
		if( isSubCtrl )
		{
			//If this ability is already equipped, show equipped (runes not shown)
			if( tObj->GetObject("EquipedCheck") && !isRuneCtrl )
			{
				if( IsAvartaAlreadyEquiped( abilityId ) >= 0 )
					tObj->GetObject("EquipedCheck")->SetVisible( true );
				else
					tObj->GetObject("EquipedCheck")->SetVisible( false );
			}
			//Record the corresponding abilityId
			tObj->SetInt("AvartaId",abilityId );

			//Mark the selected button
			if( isRuneCtrl )
			{
				if( SlotId == CurrentSelectedAvartaRuneSlot )
					tObj->SetBool("selected",true);
				else
					tObj->SetBool("selected",false);
			}
			else
			{
				if( abilityId == CurrentSelectedAvartaId )
					tObj->SetBool("selected",true);
				else
					tObj->SetBool("selected",false);
			}
		}
		// 		else if( SlotId > 0 && SlotId < CurrentAvartas.Num() )
		// 			CurrentAvartas(SlotId).AvartaId = abilityId;
	}
	else
	{
		//`log("CurrentSpell Avarta Active  is NULL()"@tObj@tObj->GetObject("ItemSlot")@SlotId );
		//tObj->SetVisible(false);
		//tObj->SetBool("enabled",false);
		if( tObj->GetObject("ItemSlot") )
			eventSetItem(tObj->GetObject("ItemSlot") ,0,"",-1 );
		//tF_SpellName skill name
		if( tObj->GetObject("tF_SpellName") )
		{
			tObj->GetObject("tF_SpellName")->SetVisible( false );
			//tObj->GetObject("tF_SpellName")->SetText( "" );
		}
		if( isSubCtrl )
		{
			tObj->SetBool("enabled",false);
			if( tObj->GetObject("EquipedCheck") && !isRuneCtrl )
				tObj->GetObject("EquipedCheck")->SetVisible( false );
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( true );
		}
		else
		{
			if( tObj->GetObject("lock_mc") )
				tObj->GetObject("lock_mc")->SetVisible( false );
		}
		//Selection state canceled
		tObj->SetBool("selected",false);
	}
}

//Select current CurrentAvartaSlot
void UMMOUISkillPanel::SetCurrentAvarta( int avartaSlotId )
{
	PageJump( ESkill_Avarta );
	if( CurrentAvartaSlot != avartaSlotId )
	{
		CurrentAvartaSlot = avartaSlotId;
		//After switching, re-confirm the selected Avarta ability
		CurrentSelectedAvartaId = CurrentAvartas( CurrentAvartaSlot ).AvartaId; 
		RefreshAvartaModify();
	}
}

//RefreshAvartaModify
void UMMOUISkillPanel::RefreshAvartaModify()
{
	int j,k;
	int abilityId;
	UGFxObject*  tObj = NULL;
	UMMOAbility*  ability;

	//`log("==============================RefreshAvartaModify() Opended=================================================");
	if( !Validate() )
		return;
	//--------------------------- Select skill panel ---------------------------------------------------------

	if( ! GetObject("AvartaModify") )
		return;

	UINT Key = MMOSkillManager::CombinateAbilityID( CurrentSelectedAvartaId, 1 ,0 );
	UMMOAbility* tMainAvarta = MMOSkillManager::getSingleton().FindAbilityTemplate(Key);
	if( tMainAvarta )
	{
		//Confirm the equipped rune
		CurrentSelectedAvartaRuneSlot = tMainAvarta->GetRuneSlot();
	}
	else
		CurrentSelectedAvartaRuneSlot = 0;

	//Currently equipped Avarta ability
	for( int i =0 ;i < CurrentAvartas.Num(); ++i )
	{
		tObj = GetObject("AvartaModify")->GetObject( FString::Printf(TEXT("CurrentSpellSlot%d"),i ) );
		SetSpellBtnByAvartaId( tObj,i , CurrentAvartas(i).AvartaId ,false );
	}

	//All Avarta abilities learned by the player's current class
	k = 0;
	
	if( GetObject("AvartaModify")->GetObject("NowAvarta") )
	{
		static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESpell_QTEType"), TRUE);
		if (InEnum)
		{
			FString EnumValueName= InEnum->GetEnum(CurrentAvartaSlot  ).ToString();
			FString LocalizedName = LocalizeProperty( *(InEnum->GetName()), *(EnumValueName) );	
			GetObject("AvartaModify")->GetObject("NowAvarta")->SetText( LocalizedName );
		}
	}

/*
	int iTotalAbilitiesNum = MainPlayer->Abilities.Num() ;
	//Refresh current page number
	TotalActivePageNum = iTotalAbilitiesNum / MaxActiveModifyCount;
	if( iTotalAbilitiesNum % MaxActiveModifyCount > 0 )
		++TotalActivePageNum;
	//Work out which skills are actually shown
	int iOffset = NowActivePageNum  * MaxActiveModifyCount;
	for ( j = iOffset ;   j < iTotalAbilitiesNum ; ++j )*/
	for ( j = 0;   j < MainPlayer->Abilities.Num()  ; ++j )
	{
		ability = MainPlayer->Abilities(j);
		if( ability != NULL ){
			continue;
		}

		abilityId = ability->eventGetAbilityId();
		if( abilityId >=0 )
		{
			if( ( k < MaxAvartaModifyCount ) && ( ability->SpellCommonAttribute.CastType == E_spct_Active )
				&& ( ability->SpellCommonAttribute.QTEType == CurrentAvartaSlot+1 ) && (  ability->ClientCommonAttribute.ShowOnUI == true ) )
			{
				tObj = GetObject("AvartaModify")->GetObject( FString::Printf(TEXT("spell%d"),k )  );
				SetSpellBtnByAvartaId( tObj,k,abilityId,true );
				++k;
			}	
		}
	}
	while( k< MaxAvartaModifyCount )
	{
		tObj = GetObject("AvartaModify")->GetObject( FString::Printf(TEXT("spell%d"),k ) );
		SetSpellBtnByAvartaId( tObj,k,0 ,true );
		++k;
	}
	//Refresh runes once
	OnSelecteAvartaChanged( CurrentSelectedAvartaId );
}

//Select the AvartaId corresponding to AllAvartas( abilityId ) to switch CurrentAvartaSlot to
void UMMOUISkillPanel::SelectAvarta( int tSlotId,int abilityId )
{
	UGFxObject*  tObj = NULL;
	int OtherRef;
	if( CurrentAvartas( CurrentAvartaSlot ).AvartaId != abilityId )
	{
		CurrentSelectedAvartaSlot = tSlotId;
		CurrentSelectedAvartaId = abilityId;
		OtherRef = IsAvartaAlreadyEquiped( abilityId ) ;
		if( OtherRef >= 0 )
		{
			// the corresponding check mark will also be overwritten
			CurrentAvartas( OtherRef ).IsChanged = true; 
			CurrentAvartas( OtherRef ).AvartaId = 0;
		}
		CurrentAvartas( CurrentAvartaSlot ).AvartaId = abilityId; 
		CurrentAvartas( CurrentAvartaSlot ).IsChanged = true; 
		RefreshAvartaModify();
	}
}

//Refresh runes once
void UMMOUISkillPanel::OnSelecteAvartaChanged( int  abilityId )
{
	if( !GetObject("AvartaModify") )
		return;

	//Refresh all runes corresponding to abilityId
	int iRune = 0;
	UINT Key = MMOSkillManager::CombinateAbilityID( CurrentSelectedAvartaId, 1 ,0 );
	UMMOAbility* tMainAvarta = MMOSkillManager::getSingleton().FindAbilityTemplate(Key);
	UMMOAbility* tRune = NULL;
	UGFxObject* tObj = NULL;
	//show all runes of the current ability
	int byRuneSlot = 0;
	while( tMainAvarta && iRune < tMainAvarta->SubRuneList.Num() ) 
	{
		tRune = tMainAvarta->SubRuneList( iRune );
		tObj = GetObject("AvartaModify")->GetObject( FString::Printf(TEXT("avarta%d"),iRune) );
		//Treat directly as Main
		SetSpellBtnByAvarta( tObj ,iRune ,tRune ,true , true );
		++iRune;
	} 
	//Do not show the rest
	while( iRune < AvartaRuneCount )
	{
		tObj = GetObject("AvartaModify")->GetObject( FString::Printf(TEXT("avarta%d"),iRune ) );
		SetSpellBtnByAvarta( tObj ,iRune ,NULL ,true , true );
		++iRune;
	}
}

//Ability abilityId has the tSlotId-th rune equipped
void UMMOUISkillPanel::SelectAvartaRune( int tSlotId,int abilityId )
{
	CurrentSelectedAvartaRuneSlot = tSlotId;
	OnSelecteAvartaChanged( CurrentSelectedAvartaId );
}

//Whether Avarta is already equipped
int UMMOUISkillPanel::IsAvartaAlreadyEquiped( int abilityId )
{
	int i;
	for( i = 0; i < CurrentAvartas.Num() ; ++i )
	{
		if( CurrentAvartas( i ).AvartaId ==  abilityId )
			return i;
	}
	return -1;
}

//Confirm submit Modify
void UMMOUISkillPanel::ConfirmAvarta( )
{
	sSpellChange tSC;
	SpellChangeDeque tChangeMsg;
	SpellChangeDeque::iterator tIter;

	for( int i = 0; i < CurrentAvartas.Num() ; ++i )
	{
		if( CurrentAvartas( i ).AvartaId != RealAvartas( i ).AvartaId  )
		{
			tSC.SlotId = i;
			tSC.AbilityId = CurrentAvartas( i ).AvartaId  ;
			if( tSC.AbilityId == 0 )
				tChangeMsg.push_front( tSC );
			else
				tChangeMsg.push_back( tSC );
		}
	}

	for( tIter = tChangeMsg.begin(); tIter != tChangeMsg.end() ; ++ tIter )
	{
		SendEquipAvarta( tIter->SlotId,  tIter->AbilityId );
	}
	//If a rune was selected
    //Local Rune index is 0~4, server uses 1~5
	if( CurrentSelectedAvartaRuneSlot > 0 )
		SendEquipAvartaRune( CurrentSelectedAvartaSlot, CurrentSelectedAvartaRuneSlot  );
}

//Page turn
void UMMOUISkillPanel::AvartaLastPage( )
{
	if( NowAvartaPageNum > 0 )
	{
		--NowAvartaPageNum;
		RefreshAvartaModify();
	}	
}

void UMMOUISkillPanel::AvartaNextPage( )
{
	if( NowAvartaPageNum < TotalAvartaPageNum -1 )
	{
		++NowAvartaPageNum;
		RefreshAvartaModify();
	}
}

void UMMOUISkillPanel::AvartaRuneLastPage( )
{
	if( NowAvartaRunePageNum > 0 )
	{
		--NowAvartaRunePageNum;
		OnSelecteAvartaChanged( CurrentSelectedAvartaId );
	}	
}

void UMMOUISkillPanel::AvartaRuneNextPage( )
{
	if( NowAvartaRunePageNum < TotalAvartaRunePageNum -1 )
	{
		++NowAvartaRunePageNum;
		OnSelecteAvartaChanged( CurrentSelectedAvartaId );
	}
}
#endif