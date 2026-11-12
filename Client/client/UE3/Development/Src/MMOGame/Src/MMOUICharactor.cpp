#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
#include "MMOChatManager.h"
#include "MMOGuildManager.h"
#include "CommonGameEvents.h"
#include "MMOAttributeAccountManager.h"
#include "MMOPowerManager.h"
#include "UILayout.h"
#include "CommonGameEvents.h"
#include "MMOGuideManager.h"
#include "UMMOUIManager.h"
#include "MMOMapDataManager.h"
#include "EquipSlotManager.h"
#include "MasterStudentManager.h"


IMPLEMENT_CLASS(UMMOUICharacter)

#define ValidSetAttributeText( a , b ) if( AttributePanel->GetObject(TEXT(a)) && AttributePanel->GetObject(TEXT(a))->GetObject( TEXT("tf_Text"))  ) { AttributePanel->GetObject(TEXT(a))->GetObject( TEXT("tf_Text"))->SetText(b); }
#define ValidSetAttributeTextColor( a , b ) if( AttributePanel->GetObject(TEXT(a)) && AttributePanel->GetObject(TEXT(a))->GetObject( TEXT("tf_Text")) ) { AttributePanel->GetObject(TEXT(a))->GetObject( TEXT("tf_Text"))->SetInt( TEXT("textColor"), b); }
#define ValidSelectAttrColor( TfName , OriVal , NewVal , InColor , OriColor , DeColor ) \
{ \
	if( NewVal > OriVal ){ ValidSetAttributeTextColor( TfName , InColor ) } \
	else if( NewVal == OriVal){ ValidSetAttributeTextColor( TfName , OriColor ) } \
	else { ValidSetAttributeTextColor( TfName , DeColor ) } \
}
#define ValidSetBaseInfoText( a , b ) if( showPanel->GetObject(TEXT(a)) ) { showPanel->GetObject(TEXT(a))->SetText(b); }

#define itemRecover 13
//------------------------------------------------------------------------

INT g_DelayRefresh=0;
INT g_UpdateArmorRefresh=0;

void UMMOUICharacter::Init()
{
	Super::Init();

	check(EventAdapter == NULL);

	MMOUI::GetUIManager()->SetUpdateTitleDataDelegate(FScriptDelegate(this, TEXT("UpdateTitleData")));

	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_ExpExtraInit);
	evts.AddItem(GameEvent_MainPlayerDataChange);
	evts.AddItem(GameEvent_UpdateTraceRelation);
	evts.AddItem(GameEvent_LeaveSelectingRole);
	evts.AddItem(GameEvent_EnterGameSuccess);
	evts.AddItem(GameEvent_MySpiritLevelUp);
	evts.AddItem(GameEvent_Item_Delete);
	evts.AddItem(GameEvent_ExpExtraChanged);
	evts.AddItem(GameEvent_CharactorPower);

	EventAdapter = new TGameEventAdapter<UMMOUICharacter>(this, evts);

	showPanel  = GetObject( TEXT("showPanel_mc") );
	relationPanel   = GetObject( TEXT("relationPanel_mc") );
	famePanel       = GetObject( TEXT("famePanel_mc") );

	INT slotValue[E_NES_TOTAL] = {};
	slotValue[0] = E_NES_SHOULDER;
	slotValue[1] = E_NES_HEAD;
	slotValue[2] = E_NES_NECK;
	slotValue[3] = E_NES_ARM;
	slotValue[4] = E_NES_BODY;
	slotValue[5] = E_NES_BACK; //cloak
	slotValue[6] = E_NES_WRIST_L;
	slotValue[7] = E_NES_WAIST; //belt
	slotValue[8] = E_NES_WRIST_R;
	slotValue[9] = E_NES_FINGER_L;
	//slotValue[10] = E_NES_LEG;
	slotValue[10] = E_NES_FINGER_R;
	slotValue[11] = E_NES_MAIN_HAND;
	slotValue[12] = E_NES_FOOT;

	EquipSlotList.Empty();
	if( showPanel )
	{
		for( INT i = 0; i < E_NES_TOTAL; ++i )
		{
			FString slotName = TEXT("itemSlot_");
			UGFxObject* slot = showPanel->GetObject( slotName + appItoa(i) );
			if( slot )
			{
				//if (i == itemRecover)
				//{
					//InitRecover();
				//}
				//else
				//{
					slot->SetInt( TEXT("SlotType") , ESPType_Charactor );
					slot->SetInt( TEXT("EquipID") , E_CONSLOT_EQUIPPED );
					slot->SetInt( TEXT("itemid") , slotValue[i] );
				//}

				slot->SetString( TEXT("tipsHtml") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Equipslot.SlotTips") )+appItoa(i) ) );
				EquipSlotList.AddItem(slot);
			}
		}

		//UpdateAttributePanel();
		InitRecover();
	}

	UGFxObject* textData = GFXPlayer->CreateObject( TEXT("Object") );
	textData->SetString( TEXT("battlePointTips") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Charactor.BattlePointTips") ) );
	textData->SetString( TEXT("damagePointTips") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Charactor.DamagePointTips")) );
	textData->SetString( TEXT("lifePointTips") , MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Charactor.LifePointTips")) );
	textData->SetInt( TEXT("panelId") , ESPType_Charactor );
	eventCallSetPanelText(textData);
	
	//
	_EnhcPanel = ConstructObject<UMMOUIEquipSlotEnhancePanel>( UMMOUIEquipSlotEnhancePanel::StaticClass(), this );
	//_EnhcPanel = new UMMOUIEquipSlotEnhancePanel();
	if( _EnhcPanel != NULL )
	{
		_EnhcPanel->InitByParent( this );
	}

	//
	g_layoutList.push_back(new UILayoutLeftRight(this, MMOUI::GetUIManager()->Panel_Bag));

	//
	UGFxObject *enchTabBtn = GetObject( TEXT("tab4_mc") );
	if( enchTabBtn != NULL ){
		UBOOL isToShow = MMOFacade::getSingleton().GetMyLevel() >= 60;
		enchTabBtn->SetVisible( isToShow );
	}

	//UpdateArmor();
}

void UMMOUICharacter::InitRecover()
{
	UGFxObject* slot = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , itemRecover ) );
	if(slot)
	{
		slot->SetInt( TEXT("SlotType") , ESPType_Recover );
		slot->SetInt( TEXT("EquipID") , E_CONSLOT_RECOVER );
		slot->SetInt( TEXT("itemid") , 0 );
		slot->SetString( TEXT("tipsHtml") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.Equipslot.SlotTips") )+appItoa(itemRecover) ) );
		EquipSlotList.AddItem(slot);
	}
}

//
void UMMOUICharacter::UnInit()
{
	Super::UnInit();

	MMOUI::GetUIManager()->SetUpdateTitleDataDelegate(FScriptDelegate(EC_EventParm));

	if( _EnhcPanel != NULL )
	{
		_EnhcPanel->UnInit();
		_EnhcPanel->MarkPendingKill();
		_EnhcPanel = NULL;
	}
}

//
void UMMOUICharacter::UpdateBattlePoint()
{
	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if( data )
	{
		MMOAttributeAccountManager::getSingleton().SetAttrOwnerMode(AttrListenerType_Mine);

		data->SetString( TEXT("damagePoint") , MMOAttributeAccountManager::getSingleton().GetDamagePoint() );
		data->SetString( TEXT("lifePoint") , MMOAttributeAccountManager::getSingleton().GetLifePoint() );
		data->SetString( TEXT("battlePoint") , appItoa( MMOPowerManager::getSingleton().GetPower() ) );

		eventCallUpdateBattlePoint(data);
	}

}

void UMMOUICharacter::UpdateBattlePoint(INT pow)
{
	UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
	if( data )
	{
		MMOAttributeAccountManager::getSingleton().SetAttrOwnerMode(AttrListenerType_Mine);

		data->SetString( TEXT("damagePoint") , MMOAttributeAccountManager::getSingleton().GetDamagePoint() );
		data->SetString( TEXT("lifePoint") , MMOAttributeAccountManager::getSingleton().GetLifePoint() );
		data->SetString( TEXT("battlePoint") , appItoa(pow) );

		eventCallUpdateBattlePoint(data);
	}
}


void UMMOUICharacter::UpdateAttributePanel()
{
	if (IsVisible == FALSE)
		return;

	MMOAttributeAccountManager& AttributeMgr = MMOAttributeAccountManager::getSingleton();

	AttributeGroupArray& attriGroup = AttributeMgr.GetAttributeGroupArray();
	
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	//

	//if( !player || !player->ActorAttributes )
	//{
	//	return;
	//}

	AttributeMgr.SetAttrOwnerMode(AttrListenerType_Mine);

	UGFxObject* dataArray = GFXPlayer->CreateArray();

	for ( INT i = 0 ; i < attriGroup.Num() ; i++ )//skip basic attributes
	{
		UGFxObject* groupData = GFXPlayer->CreateObject( TEXT("Object") );
		UGFxObject* attributeArray = GFXPlayer->CreateArray();

		for ( INT j = 0 ; j < attriGroup(i).AttributeList.Num() ; j++ )
		{
			UGFxObject* itemData = GFXPlayer->CreateObject( TEXT("Object") );
			itemData->SetString( TEXT("itemName") , AttributeMgr.GetAttributeName(i,j) );
			
			itemData->SetString( TEXT("itemValue") , AttributeMgr.GetAttributeValue(i,j) );
						
			attributeArray->SetElementObject( j , itemData );
		}
		
		groupData->SetString( TEXT("titleName") , attriGroup(i).GroupName );
		groupData->SetObject( TEXT("attriList") , attributeArray );

		dataArray->SetElementObject( i , groupData );
		
	}
	eventCallSetAttributeData(dataArray);

	//UpdateBattlePoint();
	QueryPowerCD=0.2f;
	//CMessageHandler::GetInstance().Send_QueryPower(player->ServerID);
}

void UMMOUICharacter::ClearUI()
{
	// TODO: change this 16 if the equipment count changes later
	if( showPanel )
	{
		for (INT i = 0; i < E_NES_TOTAL; ++i)
		{
			FString slotName = TEXT("itemSlot_");
			UGFxObject* slot = showPanel->GetObject( slotName + appItoa(i) );
			INT serverSlotID = slot->GetInt( TEXT("itemid") );
			if( slot )
			{
				GFXPlayer->SetItemSlotData(slot, NULL, ESPType_Charactor, E_CONSLOT_EQUIPPED,serverSlotID);
			}
		}
	}

}

void UMMOUICharacter::ToogleVisible()
{
	ChangeVisible();
	if(IsVisible)
	{
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_CharacterInfo_op"));
	}
	else
	{
		MMOFacade::getSingleton().PlayUISound(TEXT("menu_CharacterInfo_cls"));
	}
}

//
void UMMOUICharacter::ClickTab( INT tabID )
{
	switch( _CurType )
	{
	case EChaPT_Enhance:
		{
			if( _EnhcPanel != NULL )
			{
				_EnhcPanel->PostOnShow( FALSE );
			}
		}
		break;
	}

	//
	_CurType = tabID;

	//
	switch( _CurType )
	{
	case EChaPT_Reputation:
		{
			UpdateReputationData();
		}
		break;
	case EChaPT_Title:
		{
			UpdateTitleData(false);
		}
		break;
	case EChaPT_Money:
		{
			UpdateMoneyData();
		}
		break;
	case EChaPT_Enhance:
		{
			if( _EnhcPanel != NULL )
			{
				_EnhcPanel->PostOnShow( TRUE );
			}
		}
		break;
	}
}
//void UMMOUICharacter::UpdateAttributePanel()
//{
//	//if( !AttributePanel || !Player || !ctrl || !Player->ActorAttributes )
//		return;
//	//UActorAttribute* ActorAttributes = Player->ActorAttributes;
//
//	//PlayerExpChange(Player->ActorAttributes->LevelAttributes.CurrentExp, Player->ActorAttributes->LevelAttributes.MaxExp);
//
//}
//------------------------------------------------------------------------
void UMMOUICharacter::BagSlotChange( INT BagId , INT SlotID )
{
	if(BagId == E_CONSLOT_EQUIPPED)
	{
		g_UpdateArmorRefresh=2;
//		UpdateArmorUI();
		//SysModelEquip();
		//UpdateShowAvatar( SlotID );
	}
	if(BagId == E_CONSLOT_RECOVER)
	{
		g_UpdateArmorRefresh=2;

//		UpdateRecover();
	}
	if( _CurType == EChaPT_Enhance && _EnhcPanel != NULL )
	{
		_EnhcPanel->BagSlotChange( BagId, SlotID );
	}
}

void UMMOUICharacter::ItemChange(int ContainerID, int SlotID)
{
// 	if(ContainerID == E_CONSLOT_EQUIPPED)
// 	{
// 		UpdateArmorUI();
// 		//SysModelEquip();
// 		UpdateShowAvatar( SlotID );
// 	}
}

void UMMOUICharacter::ItemDelete(int ContainerID, int SlotID)
{
// 	if(ContainerID == E_CONSLOT_EQUIPPED)
// 	{
// 		UpdateArmorUI();
// 		//SysModelEquip();
// 		UpdateShowAvatar( SlotID );
// 	}
}
//------------------------------------------------------------------------
void UMMOUICharacter::UpdateArmorUI( )
{
	if(	IsVisible )
	{
		UpdateArmor();
	}
}

void UMMOUICharacter::UpdateArmor()
{
	for( INT i = 0 ; i < E_NES_TOTAL ; i++)
	{
		UGFxObject* mc = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , i )) ;
		if( mc != NULL )
		{
			if (i != itemRecover)
			{
				INT serverSlotID = mc->GetInt( TEXT("itemid") );

				if( serverSlotID >= 0 )
				{
					//UEquip* e = Player->ItemManager->GetEquipBySlot( serverSlotID, true );
					UEquip* e = Cast<UEquip>( MMOFacade::getSingleton().FindItemTemplate( CONSLOT_EQUIPPED , serverSlotID ) );
					GFXPlayer->SetItemSlotData( mc , e , ESPType_Charactor, CONSLOT_EQUIPPED, serverSlotID );	

				}
			}
		}
	}
}
void UMMOUICharacter::UpdateRecover()
{
	if(	IsVisible ){
		UGFxObject* mc = showPanel->GetObject(  FString::Printf( TEXT( "itemSlot_%d") , itemRecover )) ;

		if (mc == NULL || GFXPlayer == NULL)
			return;

		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_RECOVER, 0);
		if(info)
		{
			UItem* itemTemplate = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
			GFXPlayer->SetItemSlotData( mc , itemTemplate , ESPType_Recover, E_CONSLOT_RECOVER, 0 );	
		}
		else
		{
			GFXPlayer->SetItemSlotData( mc , NULL , ESPType_Recover, E_CONSLOT_RECOVER, 0 );	
		}
	}
}
//------------------------------------------------------------------------
void UMMOUICharacter::UpdateShowAvatar( int SlotID )
{
	return;	// no 3d avatar now!
	if( GFXPlayer == NULL || GFXPlayer->ShowAvatar == NULL )
		return;
	UEquip* e = Player->ItemManager->GetEquipBySlot( SlotID, true );
	GFXPlayer->ShowAvatar->ItemManager->RequestTakeOff(SlotID, true);
	if( e )
		GFXPlayer->ShowAvatar->ItemManager->RequestTakeOn( -1, SlotID, true, e );
	GFXPlayer->ShowAvatar->ItemManager->UpdateOwnerData();
}
//------------------------------------------------------------------------
void UMMOUICharacter::PlayerLevelChange(INT lastLv,INT newLv)
{

	PlayerBaseInfoChange();

	//UpdateWeaponUI();
	//UpdateArmorUI();
	//if(!Player)
	//{
	//	return;
	//}
	//if( !paperDollPanel )
	//{
	//	return;
	//}
	//if( NormalEquipPanel )
	//{
	//	NormalEquipPanel->SetVisible(true);
	//}

	////`log("iconfilename:"@MMOPlayer(GetPC().Pawn).ItemManager.EquippedItemList[EET_Normal_MainHand].DisplayIcon.Name );
	////`log("iconfilepath:"@MMOPlayer(GetPC().Pawn).ItemManager.EquippedItemList[EET_Normal_MainHand].DisplayIcon.GetPackageName());
	//if( paperDollPanel->GetObject("titlename_mc") )
	//{
	//	paperDollPanel->GetObject("titlename_mc")->SetText( 
	//		FString::Printf( TEXT( "%d%s(%s)  %s") , lv,
	//		*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Character.Lv") ),
	//		*MMOUI::ParseLocalizedPropertyPath( FString::Printf(TEXT("MMOGame.School.SchoolName%d"), Player->GetClassId())),
	//		*Player->ActorName  ) );
	//}	
}
//------------------------------------------------------------------------
void UMMOUICharacter::EnterScene()
{
	if(GFXPlayer)
	{
		GFXPlayer->NeedInitSceneUI = true;

		//automatically refresh reputation tracking based on map
		INT curMyMapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
		ReputationTemp*   pReputationTemp = MMOFacade::getSingleton().GetReputationTempBySceneID(curMyMapID);
		if( pReputationTemp ){
			MMOUI::GetUIManager()->SetTraceRelationForceID(pReputationTemp->ForceID);
			MMOUI::GetUIManager()->delegateUpdateReputationData();
		}
		else
		{
			//MMOUI::GetUIManager()->SetTraceRelationForceID(0);
		}
	}
}

//------------------------------------------------------------------------
void UMMOUICharacter::ChangeScene()
{
	UMMOUIBase::ChangeScene();
	//CreateShowLevel();

	//automatically refresh reputation tracking based on map
	INT curMyMapID = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	ReputationTemp*   pReputationTemp = MMOFacade::getSingleton().GetReputationTempBySceneID(curMyMapID);
	if( pReputationTemp ){
		MMOUI::GetUIManager()->SetTraceRelationForceID(pReputationTemp->ForceID);
		MMOUI::GetUIManager()->delegateUpdateReputationData();
	}
	else
	{
		//MMOUI::GetUIManager()->SetTraceRelationForceID(0);
	}
}

//
void UMMOUICharacter::NativeInitOnce()
{
	UGFxObject* obj = NULL;
	obj = GetObject("tab0_mc");
	if(obj)
	{
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.CharacterUI.Avatar") ) );
	}
	obj = GetObject("tab1_mc");
	if(obj)
	{
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.CharacterUI.Reputation") ) );
	}
	obj = GetObject("tab2_mc");
	if(obj)
	{
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.CharacterUI.Title") ) );
	}
	obj = GetObject("tab3_mc");
	if(obj)
	{
		obj->SetString( TEXT("label") , MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.CharacterUI.Money") ) );
	}

}
//------------------------------------------------------------------------
void UMMOUICharacter::CreateShowLevel()
{
	if( ! ShowLevel  && GFXPlayer )
	{

		ShowLevel = GFXPlayer->eventGetShowLevel( ShowLevelName );

	}
	if( !ShowLevel || !ShowLevel->RenderTarget )
		return;
	ShowLevel->SetEnabled( true );
	GFXPlayer->SetExternalTexture( TEXT("actorpic -nopack"),ShowLevel->RenderTarget);
}
//------------------------------------------------------------------------
void UMMOUICharacter::EnableCapture(UBOOL b)
{
	CreateShowLevel();

	if( !ShowLevel  )
		return;

	if( b )
	{
		//UMMOUICharacter* player = Cast<UMMOUICharacter>(GFXPlayer);
		if(GFXPlayer->NeedInitSceneUI)
		{
			//SysModelEquip();
			GFXPlayer->NeedInitSceneUI = FALSE;
		}

		ShowLevel->SetEnabled( b );
		
		FName tag( "C_01", FNAME_Find );
		
		if(GFXPlayer->ShowAvatar)
		{
			if( GFXPlayer->ShowAvatar->UseingAvatar )
				tag = GFXPlayer->ShowAvatar->UseingAvatar->PlacementNodeTag;
			// change to dynamic lighting
			if( GFXPlayer->ShowAvatar->LightEnvironment )
				GFXPlayer->ShowAvatar->LightEnvironment->SetEnabled( FALSE );
			ShowLevel->SetShowActor( GFXPlayer->ShowAvatar, tag );
		}
	}
	else
	{
		ShowLevel->SetEnabled( b );
	}
}

//-----------------------------------------------------------------------
void UMMOUICharacter::PlayerTitleChange( INT TitleID )
{
	if( !showPanel || !Player || !Player->ActorAttributes )
		return;
	
	if(	IsVisible )
	{
		MMOUI::GetUIManager()->delegateUpdateTitleData(FALSE);
	}

	if(TitleID>0){
		TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(TitleID);
		if(pTitle){
			ValidSetBaseInfoText( "Title" , pTitle->TitleName );
		}
	}
	else
	{
		ValidSetBaseInfoText( "Title" , TEXT("-"));
	}

	MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_TitlePow );
}
//------------------------------------------------------------------------
void UMMOUICharacter::PlayerPkValueChange( INT PkValue )
{
	//if( !showPanel || !Player || !Player->ActorAttributes )
	//	return;
	if( !IsVisible )
	{
		return;
	}
	//ValidSetBaseInfoText( "PK" , FString::Printf( TEXT("%d"), PkValue ) );

	PlayerAttributeChange( SAIP_PKValue , PkValue );
}
//------------------------------------------------------------------------
void UMMOUICharacter::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{	
		g_DelayRefresh=2;
//		UpdateArmorUI();
//		UpdateRecover();
		//EnableCapture( true );
		//SysModelEquip();
		GotoAndStopI(1); // oscar@snda	

		//UpdateBattlePoint();

		MMOGameEvent_Any evt(GameEvent_RequestMainPlayerData);
		MMOUI::PostGameEvent(&evt);

//		UpdateAttributePanel();

//		if(ResetCharactorDataFlag)
//		{
//			ResetCharactorData();
//		}
		

//		MMOUI::GetUIManager()->delegateUpdateMenuEffect(0,EMenuType_Charactor,FALSE,FALSE);


		ShowPanelAdvancedData* pPanelAdvancedData = MMOFacade::getSingleton().GetShowPanelAdvancedData(FriendlyName.GetNameString());
		CanChangeTabOnPostShow = TRUE;
		if(pPanelAdvancedData) 
		{
			if(!pPanelAdvancedData->TabName.IsEmpty())
			{
				TArray<struct FASValue> vArgs;
				Invoke(pPanelAdvancedData->TabName, vArgs);
				CanChangeTabOnPostShow = FALSE;
			}
			MMOFacade::getSingleton().DelShowPanelAdvancedData(FriendlyName.GetNameString());
		}
		else
		{
			eventCallUCUpdateTabClicked();
		}

//		PlayerBaseInfoChange();

		MMOGameEvent_Guide evtGuide(EGUET_ShowCharacterPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else{
		switch( _CurType )
		{
		case EChaPT_Enhance:
			{
				if( _EnhcPanel != NULL )
				{
					_EnhcPanel->PostOnShow( FALSE );
				}
			}
			break;
		}
	}

}

void UMMOUICharacter::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_MainPlayerDataChange:
		{
			MMOGameEvent_MainPlayerDataChange* e = (MMOGameEvent_MainPlayerDataChange*)evt;
			switch(e->DataChangeEventType)
			{
			case MMOGameEvent_MainPlayerDataChange::EMSET_HP:
				{
					PlayerHPChange(e->CurrentIntValue, e->MaxIntValue);
				}
				break;

			case MMOGameEvent_MainPlayerDataChange::EMSET_MP:
				{
					PlayerMPChange(e->CurrentIntValue, e->MaxIntValue);
				}
				break;
			}
		}
		break;
	case GameEvent_UpdateTraceRelation:
		{
			UpdateTraceReputationDisplay();
		}
		break;
	case GameEvent_LeaveSelectingRole:
		{
			ResetCharactorDataFlag = TRUE;
		}
		break;
	case GameEvent_EnterGameSuccess:
		{
			ResetCharactorDataFlag = TRUE;
		}
		break;
	case GameEvent_MySpiritLevelUp:
	case GameEvent_Item_Delete:
		{
			if( _CurType == EChaPT_Enhance && _EnhcPanel != NULL )
			{
				_EnhcPanel->OnNativeGameEvent( evt );
			}
		}
		break;
	case GameEvent_ExpExtraInit:
		{
			MMOGameEvent_OneInt *e = (MMOGameEvent_OneInt*)evt;
			MMOAttributeAccountManager::getSingleton().setExpEtra(e->IntData);
			UpdateAttributePanel();
		}
		break;
	case GameEvent_ExpExtraChanged:
		{
			MMOGameEvent_OneInt *e = (MMOGameEvent_OneInt*)evt;
			INT expDiff = MMOAttributeAccountManager::getSingleton().getExpEtra() + e->IntData;

			if (expDiff <=0 )
				expDiff = 0;

			MMOAttributeAccountManager::getSingleton().setExpEtra(expDiff);
			UpdateAttributePanel();
		}
		break;
	case GameEvent_CharactorPower:
		{
			MMOGameEvent_UpdateRolePow *e = (MMOGameEvent_UpdateRolePow*)evt;
			if(e->roleId == MMOFacade::getSingleton().GetMainPlayer()->ServerID)
				UpdateBattlePoint(e->pow);
		}

		break;
	}

}

void UMMOUICharacter::PlayerHPChange(INT NewHP, INT MaxHP)
{
	//if( !showPanel || !Player || !Player->ActorAttributes )
	//	return;

	//ValidSetBaseInfoText( "HP" , FString::Printf( TEXT("%d/%d"), NewHP , MaxHP) );


	/*if (NewHP < MaxHP && this->GetVisible())
	{
		UpdateAttributePanel();
	}*/
	
	if (IsVisible)
	{
		eventCallUpdatePlayerHPMP(1, NewHP,MaxHP);
	}
}
//------------------------------------------------------------------------
void UMMOUICharacter::PlayerMPChange(INT NewMP, INT MaxMP)
{
	//if( !showPanel || !Player  || !Player->ActorAttributes )
	//	return;
	//ValidSetBaseInfoText( "MP" , FString::Printf( TEXT("%d/%d"), NewMP , MaxMP) );
	/*if (NewMP < MaxMP && this->GetVisible())
	{
	UpdateAttributePanel();
	}*/
	if (this->GetVisible())
		eventCallUpdatePlayerHPMP(2, NewMP,MaxMP);
}

//
void UMMOUICharacter::PlayerExpChange(INT CurrentExp, INT MaxExp,INT ChangeExp)
{
	//if( !paperDollPanel || !Player  || !Player->ActorAttributes )
	//	return;
	//ValidSetBaseInfoText( "EXP" , FString::Printf( TEXT("%d/%d"), CurrentExp , MaxExp  ) );
	
}
//------------------------------------------------------------------------
void UMMOUICharacter::PlayerAttributeChange(INT AtrrId,INT inValue)
{
	if( !Player  || !Player->ActorAttributes )
		return;
	if( !GFXPlayer || AtrrId == 0 )
		return;

	MMOAttributeAccountManager& attriMgr = MMOAttributeAccountManager::getSingleton();
	AttributeGroupArray& groupList = attriMgr.GetAttributeGroupArray();
	INT classId = Player->GetClassId();



	/*if( attriMgr.IsBattleAttribute(AtrrId) )
	{
		UpdateBattlePoint();
	}*/

	//for ( INT i = 0 ; i < groupList.Num() ; i++ )
	//{
	//	for ( INT j = 0 ; j < groupList(i).AttributeList.Num() ; j++ )
	//	{
	//		StructAttributeItem attriItem = groupList(i).AttributeList(j);
	//		
	//		//if there is a related attribute, update this entry
	//		//if( attriMgr.IsRelationAttribute( i , j , AtrrId ) )
	//		//{
	//			UGFxObject* itemData = GFXPlayer->CreateObject( TEXT("Object") );
	//			itemData->SetInt( TEXT("groupId") , i);
	//			itemData->SetInt( TEXT("indexId") , j );
	//			itemData->SetString( TEXT("itemName") , attriMgr.GetAttributeName(i,j) );
	//			itemData->SetString( TEXT("itemValue") , attriMgr.GetAttributeValue(i,j) );

	//			eventCallSetAttributeItem(itemData);
	//		//}
	//		
	//	}
	//}
	UpdateAttributePanel();

	return;
	//none of the following is needed
	/*
	UActorAttribute* ActorAttributes = Player->ActorAttributes;
	INT OriginValue = MMOFacade::getSingleton().GetAttributeConfigData( MMOFacade::getSingleton().GetMyLevel() , Player->GetClassId() , AtrrId );
	INT PlusValue = MMOFacade::getSingleton().GetAttribute2ConfigData( MMOFacade::getSingleton().GetMyLevel() , Player->GetClassId() , AtrrId );
		
	INT NewValue = ActorAttributes->GetAttribute( AtrrId );

	switch( AtrrId)
	{
	case catStrength:
		ValidSetAttributeText( "str_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "str_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );		
		break;
	case catAgility:
		ValidSetAttributeText( "agi_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "agi_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );		
		break;
	case catIntellect:
		ValidSetAttributeText( "int_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "int_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );	
		break;
	case catStamina:
		ValidSetAttributeText( "sta_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "sta_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );	
		break;
	case catSpirit:
		ValidSetAttributeText( "spi_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "spi_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );	
		break;
	//case catMeleeAtkLo:
	//case catMeleeAtkHi:
	//	OriginValue = OriginValue + PlusValue;
	//	if( Player->GetClassId() != EPCT_PlayerClassD )
	//	{
	//		ValidSetAttributeText( "MeleeAttack_txt" , FString::Printf( TEXT("%d-%d"), ActorAttributes->GetAttribute( catMeleeAtkLo ) ,ActorAttributes->GetAttribute( catMeleeAtkHi )) );
	//		ValidSelectAttrColor( "MeleeAttack_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );	
	//	}
	//	break;
	//case catRangeAtkLo:
	//case catRangeAtkHi:
	//	OriginValue = OriginValue + PlusValue;
	//	if( Player->GetClassId() == EPCT_PlayerClassD )
	//	{
	//		ValidSetAttributeText( "MeleeAttack_txt" , FString::Printf( TEXT("%d-%d"), ActorAttributes->GetAttribute( catRangeAtkLo ) ,ActorAttributes->GetAttribute( catRangeAtkHi )) );
	//		ValidSelectAttrColor( "MeleeAttack_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );	
	//	}
	//	
	//	//ValidSetAttributeText( "ProjAttack_txt" , FString::Printf( TEXT("%d-%d"), ActorAttributes->GetAttribute( catRangeAtkLo ) ,ActorAttributes->GetAttribute( catRangeAtkHi )) );
	//	//ValidSelectAttrColor( "ProjAttack_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

	//	break;
	//case catMagicAtkLo:
	//case catMagicAtkHi:
	//	OriginValue = OriginValue + PlusValue;
	//	ValidSetAttributeText( "MagicAttack_txt" , FString::Printf( TEXT("%d-%d"), ActorAttributes->GetAttribute( catMagicAtkLo ) ,ActorAttributes->GetAttribute( catMagicAtkHi )) );
	//	ValidSelectAttrColor( "MagicAttack_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );
	//			
	//	break;
	case catPhyDef:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "PhyDef_txt" , FString::Printf( TEXT("%d"), NewValue ) );
		ValidSelectAttrColor( "PhyDef_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );
		break;
	case catMagDef:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "MagicDef_txt" , FString::Printf( TEXT("%d"), NewValue ) );
		ValidSelectAttrColor( "MagicDef_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

		break;
	case catPhyCritRate:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "PhyCrt_txt" , FString::Printf( TEXT("%.2f%%"), (FLOAT)NewValue/100 ) );
		ValidSelectAttrColor( "PhyCrt_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );
		
		break;
	case catMagCritRate:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "MagicCrt_txt" , FString::Printf( TEXT("%.2f%%"), (FLOAT)NewValue/100 ) );
		ValidSelectAttrColor( "MagicCrt_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

		break;
	case catMaxHp:		
		OriginValue = ( (FLOAT)ActorAttributes->GetAttribute( catStamina )*100/250+100 )*OriginValue/100 + PlusValue;
		ValidSetAttributeText( "LifePoint_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "LifePoint_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

		break;
	case catMaxMp:
		if( Player->GetClassId() == EPCT_PlayerClassB ) //mage special case
		{
			OriginValue = ( (FLOAT)ActorAttributes->GetAttribute( catSpirit )*100/250+100 )*OriginValue/100 + PlusValue;
		}
		ValidSetAttributeText( "Mana_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "Mana_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

		break;
	case catHpRecovery:
		OriginValue = ( (FLOAT)ActorAttributes->GetAttribute( catStamina )*100/250+100 )*OriginValue/100 + PlusValue;
		ValidSetAttributeText( "LifeUp_txt" , appItoa(NewValue) );
		ValidSelectAttrColor( "LifeUp_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

		break;
	case catMpRecovery:
	case catMpRecoveryRate:
	//case catMpRecoveryPeriod:
		{
			INT ManaUpValue = 0 ;
			INT OriginMpRecovery = MMOFacade::getSingleton().GetAttributeConfigData( MMOFacade::getSingleton().GetMyLevel() , Player->GetClassId() , catMpRecovery );
			INT OriginMpRecoveryRate = MMOFacade::getSingleton().GetAttributeConfigData( MMOFacade::getSingleton().GetMyLevel() , Player->GetClassId() , catMpRecoveryRate );
		 

			INT OriginMaxMp = MMOFacade::getSingleton().GetAttributeConfigData( MMOFacade::getSingleton().GetMyLevel() , Player->GetClassId() , catMaxMp );
			INT OriginMaxMpPlus = MMOFacade::getSingleton().GetAttribute2ConfigData( MMOFacade::getSingleton().GetMyLevel() , Player->GetClassId() , catMaxMp );
			if( Player->GetClassId() == EPCT_PlayerClassB ) //mage special case
			{
				OriginMaxMp = ( (FLOAT)ActorAttributes->GetAttribute( catSpirit )*100/250+100 )*OriginMaxMp/100 + OriginMaxMpPlus;
			}
			ManaUpValue = OriginMpRecovery + OriginMpRecoveryRate * OriginMaxMp/100;

			INT MpRecovery = ActorAttributes->GetAttribute( catMpRecovery );
			INT MpRecoveryRate = ActorAttributes->GetAttribute( catMpRecoveryRate );
			INT MaxMp = ActorAttributes->GetAttribute( catMaxMp );
			NewValue = MpRecovery + MpRecoveryRate*MaxMp/100;

			ValidSetAttributeText( "ManaUp_txt" , FString::Printf( TEXT("%.2f"), abs( (FLOAT)NewValue/10000 ) ) );
			ValidSelectAttrColor( "ManaUp_txt" , ManaUpValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );
		}
		break;
	case catDodgeRate:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "Dodge_txt" , FString::Printf( TEXT("%.2f%%"), (FLOAT)NewValue/100 ) );
		ValidSelectAttrColor( "Dodge_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );

		break;
	case catParryRate:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "Ward_txt" , FString::Printf( TEXT("%.2f%%"), (FLOAT)NewValue/100 ) );
		ValidSelectAttrColor( "Ward_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );
		
		break;
	case catBlockRate:
		OriginValue = OriginValue + PlusValue;
		ValidSetAttributeText( "Block_txt" , FString::Printf( TEXT("%.2f%%"), (FLOAT)NewValue/100 ) );
		ValidSelectAttrColor( "Block_txt" , OriginValue , NewValue , MMOUI::GetUIManager()->IncreaseAttrColor , MMOUI::GetUIManager()->OriginAttrColor , MMOUI::GetUIManager()->DecreaseAttrColor );
		
		break;
	}

	*/
}


UBOOL UMMOUICharacter::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
 { 
	 if(bDefault)return FALSE;
    return FALSE; 
 } 

 UBOOL UMMOUICharacter::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(MMOFacade::getSingleton().GetMainController()->PlayerInput->IsCtrlPressed())
	{
		MMOGameEvent_Chat_AppendInput evt(SlotType,ContainerID,SlotID,bDefault);
		MMOUI::PostGameEvent(&evt);
		return TRUE;
	}
	if(bDefault)
		return FALSE;
	return FALSE;
}

 extern void AskEquipBind(ItemInfo* info);

 //=================================================================================================================================================================
 UBOOL UMMOUICharacter::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
 {
	 switch( _CurType )
	 {
	 case EChaPT_Enhance:
		 {
			 if( _EnhcPanel != NULL )
			 {
				return _EnhcPanel->OnRightMouseClick( mc, SlotType, ContainerID, SlotID, bDefault );
			 }
		 }
		 break;
	 default:
		 {
			 return OnEquipPanelRightMouseClick( mc, SlotType, ContainerID, SlotID, bDefault );
		 }
		 break;
	 }

	 return FALSE;
 }
 
 //----------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUICharacter::OnEquipPanelRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)//default handling: when right-click gets no response, automatically unequip into the bag
	{
		if(SlotType == ESPType_Charactor)
		{
				FClientItemInfo* info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
				if (info == NULL)
					return FALSE;

				UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
				if (item == NULL)
					return FALSE;

				ClientActionCondition_EquipItem checker;
				FClientActionConditionContex contex;
				contex.Item = info;
				contex.ItemTemplate = item;
				INT DestSlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);

				/*if(!checker.CheckClientActionCondition(contex))
				{
				return FALSE;
				}*/

				UEquip* equip = Cast<UEquip>(item);
				if(equip)
				{
					//INT DestSlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);
					if(DestSlot >= 0)
					{
						CMessageHandler::GetInstance().Send_MoveItem(E_CONSLOT_DEFAULT, DestSlot, ContainerID, SlotID);
					}
					else
					{
						FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
						MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
					}
					return TRUE;
				}
				else
				{
					warnf( NAME_Error, TEXT("[UMMOUICharacter::OnRightMouseClick] move equip faild: %s"), *item->GetDisplayName() );
				}
			}
		}
		else if(SlotType == ESPType_Recover)
		{
			FClientItemInfo* info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
			if(info)
			{
				UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
				INT DestSlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);

				//mana blood stone
				if (item->ItemType == TEXT("ITEMTYPE_RECOVER"))
				{
					if(DestSlot >= 0)
						CMessageHandler::GetInstance().Send_MoveItem(E_CONSLOT_DEFAULT, DestSlot, E_CONSLOT_RECOVER, 0);
					return TRUE;
				}
		}
	}
	else{

		if(SlotType == ESPType_Bag)
		{
			if(ContainerID == E_CONSLOT_DEFAULT){
				FClientItemInfo* info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
				if(info)
				{
					UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
					UEquip* equip = Cast<UEquip>(item);
					UItem* itemTemp = NULL;
					INT DestCon = 0, DestSlot = 0;

					if( equip )
					{
						if (equip->GetEquipAppearenceTemplate())
						{
							MMOFacade::getSingleton().SetFingerPosShowType(TRUE);
							
							MMOUI::GetEquippedSlotByEquipType(equip->GetEquipAppearenceTemplate()->EquipType, DestCon, DestSlot);
							if( DestCon == E_CONSLOT_EQUIPPED )
							{
								//equipment that will be swapped in
								ClientActionCondition_EquipItem checker;
								FClientActionConditionContex contex;
								contex.Item = info;
								contex.ItemTemplate = item;
								if(!checker.CheckClientActionCondition(contex))
								{
									return TRUE;
								}

								//equipment that will be swapped out
								FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, DestSlot);
								
								if(destItemInfo)
								{
									itemTemp = MMOFacade::getSingleton().FindItemTemplate(destItemInfo->m_nTemplateID);

									ClientActionCondition_UnEquipItem checker;

									FClientActionConditionContex contex;
									contex.Item = destItemInfo;
									contex.ItemTemplate = itemTemp;

									if(!checker.CheckClientActionCondition(contex))
									{
										return TRUE;
									}
								}
							}
						

							if (CheckWeight(item,itemTemp) == FALSE)
								return FALSE;
							
							
						}
						else if(DestCon == E_CONSLOT_HERO_EQUIPPED)
						{
							ClientActionCondition_Hero_EquipItem checker;
							FClientActionConditionContex contex;
							contex.Item = info;
							contex.ItemTemplate = item;
							if(!checker.CheckClientActionCondition(contex))
							{
								return TRUE;
							}						
						}
						/*else
						{
							return FALSE;
						}*/


						// if it needs binding but is not bound yet
						if(info->m_byBindType != E_BINDTYPE_BINDED
							&& equip->ItemBindType == E_BINDTYPE_BOE)
						{
							AskEquipBind(info);
						}
						else
						{								
							CMessageHandler::GetInstance().Send_MoveItem(DestCon, DestSlot, ContainerID, SlotID);
						}
						MMOFacade::getSingleton().SetFingerPosShowType(FALSE);
						return TRUE;
					}

				}
			}
		}
	}
	return FALSE;
}

BOOL UMMOUICharacter::CheckWeight(UItem* item, UItem* itemDest)
{
	BOOL v = TRUE;
	INT nWeight = 0;

	if (itemDest)
		nWeight = itemDest->Weight;

	//first check whether weight is sufficient and whether it can be worn
	if(item->ItemType == TEXT("ITEMTYPE_EQUIP") && item->ItemSubType != TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
	{
		if (MMOFacade::getSingleton().CheckWeight(item->Weight,nWeight,WT_Armor) == FALSE)
			v =  FALSE;
	}
	else if(item->ItemType == TEXT("ITEMTYPE_EQUIP") && item->ItemSubType == TEXT("ITEMSUBTYPE_EQUIP_WEAPON"))
	{
		if (MMOFacade::getSingleton().CheckWeight(item->Weight,nWeight,WT_Weap) == FALSE)
			v = FALSE;
	}
	return v;
}
//=================================================================================================================================================================
UBOOL UMMOUICharacter::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	switch( _CurType )
	{
	case EChaPT_Enhance:
		{
			if( _EnhcPanel != NULL )
			{
				return _EnhcPanel->OnDragDrop( SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID );
			}
		}
		break;
	default:
		{
			return OnEquipPanelDragDrop( SrcSlotMC, DestSlotMC, SrcSlotType, SrcContainerID, SrcSlotID, DestSlotType, DestContainerID, DestSlotID );
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------
UBOOL UMMOUICharacter::OnEquipPanelDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	// zhaojun@snda
	if (SrcContainerID == DestContainerID && SrcSlotID == DestSlotID)
		return FALSE;

	if((SrcSlotType == ESPType_Charactor || SrcSlotType == ESPType_Recover) && DestSlotType == ESPType_Invalid) // discard
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
		if(info)
		{
			/*ClientActionCondition_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;

			if(!checker.CheckClientActionCondition(contex))
			{
			return TRUE;
			}*/

			//MMOUI::GetUIManager()->delegateDeleteItem(SrcContainerID, SrcSlotID);
			//equipment worn on the body cannot be discarded
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_EquipItem_NotThrow")));

		}

		return TRUE;
	}
	else if(SrcSlotType == ESPType_Charactor && DestSlotType == ESPType_Charactor)  // swap
	{
		//source
		FClientItemInfo* srcItemInfo = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
		if(srcItemInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcItemInfo->m_nTemplateID);
			//UItem* destItemTemp = MMOFacade::getSingleton().FindItemTemplate(DestContainerID, DestSlotID);

			ClientActionCondition_UnEquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = srcItemInfo;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}

		//target
		FClientItemInfo* destItemInfo = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
		if(destItemInfo)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(destItemInfo->m_nTemplateID);
			//UItem* destItemTemp = MMOFacade::getSingleton().FindItemTemplate(DestContainerID, DestSlotID);

			ClientActionCondition_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = destItemInfo;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}

		INT srcSlotEquipType = SrcSlotMC->GetInt(TEXT("itemid"));
		INT destSlotEquipType = DestSlotMC->GetInt(TEXT("itemid"));

		// slots of different types cannot be swapped
		if(srcSlotEquipType != destSlotEquipType)
		{
			return TRUE;
		}

		CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);

		return TRUE;
	}
	else if(SrcSlotType == ESPType_Charactor && DestSlotType == ESPType_Bag) // unequip
	{
		FClientItemInfo* srcItemInfo = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
		if( srcItemInfo )
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(srcItemInfo->m_nTemplateID);
			ClientActionCondition_UnEquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = srcItemInfo;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}
		}

		//target item
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
		if(info) // if the target slot has equipment that is not bound
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
			
			ClientActionCondition_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}

			UEquip* equip = Cast<UEquip>(itemTemp);

			if(itemTemp && equip && info->m_byBindType != E_BINDTYPE_BINDED
				&& equip->ItemBindType == E_BINDTYPE_BOE)
			{
				AskEquipBind(info);
			}
			
			else
			{
				CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
			}
		}
		else
		{
				CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
		}

		return TRUE;
	}
	else if(SrcSlotType == ESPType_Bag && DestSlotType == ESPType_Recover)
	{
		if(SrcContainerID == E_CONSLOT_DEFAULT && DestContainerID == E_CONSLOT_RECOVER)
			CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_RECOVER,0,E_CONSLOT_DEFAULT,SrcSlotID );
	}

	return FALSE;
}

//--------------------------------------------------------------------------
void UMMOUICharacter::PlayerBaseInfoChange()
{
	if( Player && IsVisible)
	{
		const GuildBaseInfo& guildBaseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();

		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();

		UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
		data->SetString( TEXT("name") , player->GetActorName() );
		data->SetString( TEXT("levelText") , appItoa( player->ActorAttributes->LevelAttributes.Level ) + MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Character.Lv") ) );
		data->SetString( TEXT("classText") , MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( player->GetClassId() )  ) );
		data->SetInt( TEXT("classId") , player->GetClassId() );
		data->SetInt( TEXT("sex") , player->GetSex() );

		eventCallUpdateBaseInfo(data);
		
	}
}


INT UMMOUICharacter::KeyToNumber(const FString&Key, INT index)
{
	FString strBuf = MMOUI::ParseLocalizedPropertyPath(Key + FString::Printf(TEXT("%d"),index)	);
	return appAtoi(*strBuf);
}

void UMMOUICharacter::InitReputationData()
{
	FString strBuf ;
	FString strIndex ;
	FString strForceID ;
	INT		i=0,j=0;
	BOOL	bBreak=false;

	//ReputationName.Empty();
	//ReputationValue.Empty();
	//ReputationValue.Push(KeyToNumber(FString("MMOGame.Reputation.ReputationValue") , 0) );
	//ReputationValue.Push(KeyToNumber(FString("MMOGame.Reputation.ReputationValue") , 1) );
	//ReputationName.Push(MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.Reputation.ReputationName0")));
	//ReputationName.Push(MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.Reputation.ReputationName1")));


	ReputationList.Empty();
	for(i=0;i<MAX_REPUTATION_COUNT;i++)
	{
		ReputationTemp*  pReputationTemp = MMOFacade::getSingleton().GetReputationTemp(i);
		if(pReputationTemp==NULL)
		{
			continue;
		}

		FReputationData data ;
		data.TempIndex		= i;
		if(pReputationTemp->ListLevel==0)
		{
			data.type			= 2;
		}
		else{
			data.type			= 0;
		}
		data.ForceName		= pReputationTemp->ForceName;
		data.ForceLevel		= pReputationTemp->ListLevel;
		data.ForceID		= pReputationTemp->ForceID;
		data.Visible		= pReputationTemp->ForceVisible;
		data.ForceTips		= pReputationTemp->ForceTips;
		data.ForceInfo		= pReputationTemp->ForceInfo;
		data.InitValue		= pReputationTemp->InitValue;
		data.MaxValue		= pReputationTemp->MaxValue;


		ReputationList.Push(data);
	}

	for(i=0;i<ReputationList.Num()-1;i++)
	{
		if(ReputationList(i).type==0 && ReputationList(i).ForceLevel<ReputationList(i+1).ForceLevel)
		{
			ReputationList(i).type = 2;
		}
	}

}




void UMMOUICharacter::ResetReputationTypeConfig()
{
	//ReputationTypeConfig.Empty();
	//for(int i=0;i<ReputationList.Num();i++)
	//{
	//	if(ReputationList(i).type==2)
	//	{
	//		ReputationTypeConfig.Push(ReputationList(i).ForceID);
	//	}
	//}
}


void UMMOUICharacter::ChangeReputationType(INT ID,INT ReputationType)
{
	if(ReputationType==0)return;
	for(int i=0;i<ReputationList.Num();i++)
	{
		if(ReputationList(i).ForceID == ID)
		{
			if(ReputationList(i).type != ReputationType)
			{
				ReputationList(i).type = ReputationType;
			}
			break;
		}
	}
}

FString  UMMOUICharacter::GetReputationLevelName(INT ForceID, INT Reputation)
{
	for(int i=0;i<ReputationList.Num();i++)
	{
		if(ReputationList(i).ForceID == ForceID)
		{
			if( Reputation >= ReputationList(i).MaxValue)
			{
				return MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.Reputation.ReputationName1"));
			}
			else
			{
				return MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.Reputation.ReputationName0"));
			}
		}
	}
	return FString("");
}

FString  UMMOUICharacter::GetReputationForceName(INT ForceID)
{
	for(int i=0;i<ReputationList.Num();i++)
	{
		if(ReputationList(i).ForceID == ForceID)
		{
			return ReputationList(i).ForceName;
			break;
		}
	}
	return TEXT("");
}

UBOOL  UMMOUICharacter::GetReputationVisible(INT ForceID)
{
	for(int i=0;i<ReputationList.Num();i++)
	{
		if(ReputationList(i).ForceID == ForceID)
		{
			return ReputationList(i).Visible;
			break;
		}
	}
	return FALSE;
}

INT  UMMOUICharacter::GetReputationLevel(INT ForceID,INT Reputation)
{
	for(int i=0;i<ReputationList.Num();i++)
	{
		if(ReputationList(i).ForceID == ForceID)
		{
			if( Reputation >= ReputationList(i).MaxValue)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 0;
}



//void UMMOUICharacter::DisplayReputationData(INT ForceID, INT ReputationNow, INT ReputationLast )
//{
//	if( !GFXPlayer )
//		return;
//
//	FString ForceName =TEXT("");
//	for(int i = 0; i < ReputationList.Num(); i++)
//	{
//		if(ReputationList(i).ForceID == ForceID)
//		{
//			ForceName = ReputationList(i).ForceName;
//			break;
//		}
//	}
//
//	if(GetReputationLevel(ReputationNow) == GetReputationLevel(ReputationLast))
//	{
//		FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Reputation.ReputationDataAdd"));
//		MMOChatManager::getSingleton().AddSystemMessage( FString::Printf(*msg,*ForceName,ReputationNow-ReputationLast)); 
//	}
//	else
//	{
//		FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Reputation.ReputationDataUpdate"));
//		FString ReputationLevelName = GetReputationLevelName(ReputationNow);
//		MMOChatManager::getSingleton().AddSystemMessage( FString::Printf(*msg,*ForceName,*ReputationLevelName)); 
//		MMOUI::GetUIManager()->delegateAddScreenMessage( FString::Printf(*msg,*ForceName,*ReputationLevelName)); 
//	}
//
//}


void UMMOUICharacter::UpdatePlayerReputation(class AMMOPlayer* Pl,INT ForceID,INT newReputation, INT lastReputation)
{
	if(newReputation==lastReputation)return;
	if(Pl==NULL)return;
	if(Pl->IsMainPlayer()==FALSE)return;

	//reputation prompt
	INT lastLevel	= GetReputationLevel(ForceID,lastReputation);
	INT curLevel	= GetReputationLevel(ForceID,newReputation);
	FString ReputationLevelName	= GetReputationLevelName(ForceID,newReputation);
	FString ForceName	= GetReputationForceName(ForceID);
	UBOOL   bForceVisible = GetReputationVisible(ForceID);

	//automatically update tracked reputation bar
	if(ForceID ==  MMOUI::GetUIManager()->GetTraceRelationForceID())
	{
		UpdateTraceReputationDisplay();
	}

	//if(lastLevel==curLevel){
	//	
	//	FString msgBuf = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Reputation.ReputationDataAdd"));
	//	FString cs  =FString::Printf(  *msgBuf/* TEXT("Reputation increased by %d")*/, newReputation-lastReputation );
	//	FString msg = str2 + cs;
	//	MMOChatManager::getSingleton().AddSystemMessage( msg ); 		
	//}
	//else
	//{
	//	FString msgBuf = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Reputation.ReputationDataUpdate"));
	//	FString msg = str2 + msgBuf/*TEXT("Reputation increased to")*/ + str1;
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( msg );
	//	MMOChatManager::getSingleton().AddSystemMessage( msg ); 
	//}
	if(bForceVisible){
		if(lastLevel==curLevel)
		{
			if(newReputation-lastReputation>0){
				FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Reputation.ReputationDataAdd"));
				//#16373
				//MMOChatManager::getSingleton().AddHintMessage( FString::Printf(*msg,*ForceName,newReputation-lastReputation)); 
			}
			else
			{
				FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Reputation.ReputationDataDec"));
				//#16373
				//MMOChatManager::getSingleton().AddHintMessage( FString::Printf(*msg,*ForceName,lastReputation-newReputation)); 
			}
		}
		else
		{
			//#16373
			//FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Reputation.ReputationDataUpdate"));
			//FString ReputationLevelName = GetReputationLevelName(ForceID,newReputation);
			//MMOChatManager::getSingleton().AddHintMessage( FString::Printf(*msg,*ForceName,*ReputationLevelName)); 
			//MMOUI::GetUIManager()->delegateAddScreenMessage( FString::Printf(*msg,*ForceName,*ReputationLevelName),MsgInfoType_Ex1,TEXT("MMOGame.Reputation.ReputationDataUpdate")); 
		}
	}
}

void UMMOUICharacter::UpdateMoneyData()
{
	if( !GFXPlayer )
		return;

	UGFxObject *DataProvider = GFXPlayer->CreateArray();
	int index = 0;
	for(int i=E_MONEYTYPE_01;i<E_MONEYTYPE_COUNT;i++ )
	{
		UBOOL   Res; 
		FString MoneyEnum		= MMOFacade::getSingleton().FindServerEnumString( i, TEXT("EMONEY_TYPE") );
		FString MoneyEnumName	= FString(TEXT("MMOGame.MoneyUI.")) + MoneyEnum;
		FString MoneyName		=MMOUI::ParseLocalizedPropertyPath( MoneyEnumName , Res);
		//FString MoneyIcon		=MMOUI::ParseLocalizedPropertyPath( MoneyEnumName + FString(TEXT("_IconName")));
		FString MoneyIconTips1	=MMOUI::ParseLocalizedPropertyPath( MoneyEnumName + FString(TEXT("_Tips1")));
		FString MoneyTips1	=MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MoneyUI.MoneyTips1"));
		FString MoneyTips2	=MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MoneyUI.MoneyTips2"));
		if(Res){
		
			UGFxObject *ItemObj = GFXPlayer->CreateObject("Object");
			ItemObj->SetString("label",MoneyName);//   MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Title.TitleName"))+appItoa(title->ID)) );
			ItemObj->SetInt("index",index);
			ItemObj->SetInt("type",0);
			ItemObj->SetInt("deepth",0);
			ItemObj->SetBool("visible",true);

			EMONEY_TYPE MoneyType = EMONEY_TYPE(i);
			INT WeeklyMoney = MMOFacade::getSingleton().GetPlayerMoney_Weekly(MoneyType);
			INT WeeklyMoneyLimit = MMOFacade::getSingleton().GetPlayerMoney_WeeklyLimit(MoneyType);

			INT32 money = MMOFacade::getSingleton().GetPlayerMoney(MoneyType);
			if (E_MONEYTYPE_MASTER_EXP == MoneyType)
			{
				money = MasterStudentManager::getSingleton().GetMasterExpByRatio(money);
			}
			ItemObj->SetInt("Money",money);
			ItemObj->SetInt("WeeklyMoney",WeeklyMoney);
			ItemObj->SetInt("WeeklyMoneyLimit",WeeklyMoneyLimit);
			ItemObj->SetInt("MoneyType",i);
			ItemObj->SetString("MoneyTypeString",MoneyEnum);
			//ItemObj->SetString("MoneyIconName",MoneyIcon);
			ItemObj->SetString("Tips1",MoneyIconTips1);
			if(WeeklyMoneyLimit>0){
				FString Tips = FString::Printf(*MoneyTips1, *MoneyName, WeeklyMoney, WeeklyMoneyLimit );
				ItemObj->SetString("Tips2",Tips);
			}
			else{
				FString Tips = FString::Printf(*MoneyTips2, *MoneyName, WeeklyMoney );
				ItemObj->SetString("Tips2",Tips);
			}

			DataProvider->SetElementObject(index++,ItemObj);
		}
	}
	eventCallUpdateMoneyData(DataProvider);		
}

void UMMOUICharacter::ShowTraceReputationInfoPanel()
{
	//INT ForceID = MMOUI::GetUIManager()->GetTraceRelationForceID();
	//for(INT i = 0; i < ReputationList.Num(); i++)
	//{
	//	if( ReputationList(i).ForceID == ForceID){

	//		ReputationTemp*  pReputationTemp = MMOFacade::getSingleton().GetReputationTemp(ReputationList(i).TempIndex);
	//		if(pReputationTemp)
	//		{
	//			INT nClassId = MMOFacade::getSingleton().GetMainPlayer()->GetClassId();
	//			//eventCallShowReputationInfo( ReputationList(i).ForceName ,   ReputationList(i).ForceInfo+pReputationTemp->ForceAward[nClassId], TRUE );
	//			break;
	//		}
	//	}
	//}
}

void UMMOUICharacter::UpdateTraceReputationDisplay()
{
	INT ForceID = MMOUI::GetUIManager()->GetTraceRelationForceID();
	
	for(int i = 0; i < ReputationList.Num(); i++)
	{
		if( ReputationList(i).ForceID == ForceID){
			UGFxObject * ReputationObj = GFXPlayer->CreateObject("Object");
			ReputationObj->SetString("label",ReputationList(i).ForceName);
			ReputationObj->SetInt("index",i);
			ReputationObj->SetInt("type",ReputationList(i).type);
			ReputationObj->SetInt("deepth",ReputationList(i).ForceLevel);
			ReputationObj->SetString("tips",ReputationList(i).ForceTips);
			if(ReputationList(i).ForceLevel > 0)
			{
				INT		reputation	= MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
				UBOOL	activeState = MMOFacade::getSingleton().GetPlayerActiveState(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
				UBOOL    awardState  = MMOFacade::getSingleton().GetPlayerAwardState(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
				//if configured as invisible, do not display
				activeState = activeState && ReputationList(i).Visible;

				ReputationObj->SetInt("trackForceID",ForceID);
				ReputationObj->SetInt("forceID",ReputationList(i).ForceID);
				ReputationObj->SetInt("colorReputation",1);
				ReputationObj->SetInt("curReputation",reputation);
				ReputationObj->SetBool("visible",activeState);
				ReputationObj->SetBool("awardState",awardState);
				ReputationObj->SetInt("minReputation",ReputationList(i).InitValue );
				ReputationObj->SetInt("maxReputation",ReputationList(i).MaxValue );
			}
			else
			{
				ReputationObj->SetBool("visible",FALSE);
				//ReputationObj->SetBool("visible",ReputationList(i).Visible);
			}

			MMOUI::GetUIManager()->delegateUpdateTraceReputationData(ReputationObj);
			break;
		}
	}
}

void UMMOUICharacter::UpdateReputationData()
{
	//if( !GFXPlayer )
	//	return;

	//UGFxObject *DataProvider = GFXPlayer->CreateArray();
	//INT ForceID = MMOUI::GetUIManager()->GetTraceRelationForceID();

	//for(int i = 0; i < ReputationList.Num(); i++)
	//{
	//	UGFxObject * ItemObj = GFXPlayer->CreateObject("Object");
	//	ItemObj->SetString("label",ReputationList(i).ForceName);
	//	ItemObj->SetInt("index",i);
	//	ItemObj->SetInt("type",ReputationList(i).type);
	//	ItemObj->SetInt("deepth",ReputationList(i).ForceLevel);

	//	ItemObj->SetString("tips",ReputationList(i).ForceTips);
	//	ReputationTemp*  pReputationTemp = MMOFacade::getSingleton().GetReputationTemp(ReputationList(i).TempIndex);
	//	if(pReputationTemp)
	//	{
	//		INT nClassId = MMOFacade::getSingleton().GetMainPlayer()->GetClassId();
	//		ItemObj->SetString("info",ReputationList(i).ForceInfo);//+pReputationTemp->ForceAward[nClassId]);
	//		
	//	}
	//	else{
	//		ItemObj->SetString("info",ReputationList(i).ForceInfo);
	//	}
	//	if(ReputationList(i).ForceLevel > 0)
	//	{
	//		INT		reputation	= MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
	//		BOOL	activeState = MMOFacade::getSingleton().GetPlayerActiveState(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
	//		//if configured as invisible, do not display
	//		activeState = activeState && ReputationList(i).Visible;

	//		for(int j=0;j<ReputationName.Num();j++)
	//		{
	//			if(reputation<=ReputationValue(j+1))
	//			{
	//				ItemObj->SetInt("trackForceID",ForceID);
	//				ItemObj->SetInt("forceID",ReputationList(i).ForceID);
	//				ItemObj->SetInt("colorReputation",j+1);
	//				ItemObj->SetInt("curReputation",reputation);
	//				ItemObj->SetBool("visible",activeState);
	//				ItemObj->SetInt("minReputation",ReputationValue(j));
	//				ItemObj->SetInt("maxReputation",ReputationValue(j+1));
	//				ItemObj->SetString("txtReputation",ReputationName(j));

	//				break;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		ItemObj->SetBool("visible",ReputationList(i).Visible);
	//	}

	//	DataProvider->SetElementObject(i,ItemObj);
	//}
	////RootMc = GetVariableObject("_root") ;
	////RootMc->SetObject("ItemsData",DataProvider);
	//eventCallUpdateItemsData(DataProvider);	
}

FString GetTitleAttrText(TitleData* title)
{
	FString res = TEXT("");
	if(title==NULL)return res;

	for(INT i=0;i<5;i++)
	{
		if(title->AttrID[i]>0)
		{
			const ItemRandomAttr* titleArr = MMOFacade::getSingleton().GetRandomAttributById(title->AttrID[i]);
			if(titleArr){
				res += titleArr->strText + TEXT("\n");

				//FString keyString = FString(TEXT("MMOGame.cat."))+titleArr->attrType;
				//INT ExType = MMOUI::FindMsgTypeByLocalString(keyString);
				//if(ExType==0){
				//	res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+titleArr->attrType) + TEXT(" +") + appItoa(titleArr->value) + TEXT("\n");
				//}
				//else if(ExType==1)
				//{
				//	res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+titleArr->attrType) + TEXT(" +") + appItoa(titleArr->value) + TEXT("%\n");
				//}
			}
		}
	}

	//if(title->AttrValue1>0)res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+title->AttrType1) + TEXT(" +") + appItoa(title->AttrValue1) + TEXT("\n");
	//if(title->AttrValue2>0)res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+title->AttrType2) + TEXT(" +") + appItoa(title->AttrValue2) + TEXT("\n");
	//if(title->AttrValue3>0)res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+title->AttrType3) + TEXT(" +") + appItoa(title->AttrValue3) + TEXT("\n");
	//if(title->AttrValue4>0)res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+title->AttrType4) + TEXT(" +") + appItoa(title->AttrValue4) + TEXT("\n");
	//if(title->AttrValue5>0)res += MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.cat."))+title->AttrType5) + TEXT(" +") + appItoa(title->AttrValue5) + TEXT("\n");

	return res;
}

FString GetTitleConditionText(TitleData* title)
{
	FString res = TEXT("");
	if(title==NULL)return res;

	if(title->ConditionValue1>0)res += MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.ConditionName"))+title->ConditionType1 ) + appItoa(title->ConditionValue1) + TEXT("\n");
	if(title->ConditionValue2>0)res += MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.ConditionName"))+title->ConditionType2 ) + appItoa(title->ConditionValue2) + TEXT("\n");

	return res;
}



void UMMOUICharacter::ASClickTitle( INT TitleId )
{
	TitleData* title =  MMOFacade::getSingleton().GetTitleData(TitleId);
	if(title && title->IsNew)
	{
		title->IsNew = FALSE;
		UpdateTitleData(FALSE);

		// Tutorial
		MMOGameEvent_Guide evtGuide(EGUET_ChooseTitle);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

void UMMOUICharacter::NativeRefreshInfoBox( UGFxObject* data )
{
	if(data)
	{
		eventCallRefreshReputationInfo(data->GetString("label"),data->GetString("info") );
	}
}

void UMMOUICharacter::NativeShowInfoBox(UGFxObject* data )
{
	if(data)
	{
		//UBOOL bShow = (MMOUI::GetUIManager()->GetTraceRelationForceID() == data->GetInt("forceID"));
		//eventCallShowReputationInfo(data->GetString("label"),data->GetString("info"), bShow );
	}
}

void UMMOUICharacter::UpdateTitleData(UBOOL bReset)
{
	int i;
	UGFxObject *DataProvider;
	UGFxObject *ItemObj;

	if( !GFXPlayer )
		return;
	if(GetBool("visible")==FALSE)
	{
		return;
	}

	if(famePanel && famePanel->GetBool("visible")==FALSE )
	{
		return;
	}

	DataProvider = GFXPlayer->CreateArray();

	//DataProvider->Clear()

	MMOFacade::getSingleton().UpdateTitleArray();
	INT count = MMOFacade::getSingleton().GetTitleDataArrayCount();
	INT TitleId = MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->TitleID;
	for(i=0;i<count;i++)
	{
		TitleData* title = MMOFacade::getSingleton().GetTitleDataArray(i);
		if(title)
		{
			ItemObj = GFXPlayer->CreateObject("Object");
			//ItemObj->SetBool("resetSelectFlag",bReset);
			ItemObj->SetString("label",title->TitleName);//   MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Title.TitleName"))+appItoa(title->ID)) );
			ItemObj->SetInt("index",i);
			ItemObj->SetInt("type",0);
			ItemObj->SetInt("deepth",0);
			ItemObj->SetBool("visible",title->ValidTime!=0);
			ItemObj->SetBool("obtained", title->ValidTime!=0);
			
			ItemObj->SetInt("numStar",title->TitleLevel);
			ItemObj->SetBool("isCurrent",(title->ID==TitleId));
			ItemObj->SetBool("isNew", title->IsNew);

			ItemObj->SetString("titleScript", title->TitleInfo);
			ItemObj->SetString("sourceImg",TEXT(""));
			ItemObj->SetString("AttrString",GetTitleAttrText(title));
			ItemObj->SetString("ConditionString",GetTitleConditionText(title));
			ItemObj->SetString("IconName",FString::Printf(TEXT("img://MMO_UITitle.Title_%d"),title->ID));
			
			ItemObj->SetInt("TitleType",title->TitleType);
			ItemObj->SetInt("TitleStyle",title->TitleStyle);
			
			ItemObj->SetInt("ID",title->ID);
			ItemObj->SetInt("ValidTime",title->ValidTime);
			//ItemObj->SetInt("AttrType1",title->AttrType1);
			//ItemObj->SetInt("AttrValue1",title->AttrValue1);
			//ItemObj->SetInt("AttrType2",title->AttrType2);
			//ItemObj->SetInt("AttrValue2",title->AttrValue2);
			//ItemObj->SetInt("AttrType3",title->AttrType3);
			//ItemObj->SetInt("AttrValue3",title->AttrValue3);
			//ItemObj->SetInt("AttrType4",title->AttrType4);
			//ItemObj->SetInt("AttrValue4",title->AttrValue4);
			//ItemObj->SetInt("AttrType5",title->AttrType5);
			//ItemObj->SetInt("AttrValue5",title->AttrValue5);
			//ItemObj->SetInt("ConditionType1",title->ConditionType1);
			//ItemObj->SetInt("ConditionValue1",title->ConditionValue1);
			//ItemObj->SetInt("ConditionType2",title->ConditionType2);
			//ItemObj->SetInt("ConditionValue2",title->ConditionValue2);

			DataProvider->SetElementObject(i,ItemObj);
		}
	}
	eventCallUpdateFameData(DataProvider,bReset);	
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUICharacter::SysModelEquip()
{

	if(GIsPlayInEditorWorld)
	{
		return;
	}

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();

	if( GFXPlayer->ShowAvatar == NULL )
	{
		for( TObjectIterator<AMMOPlayer>It; It; ++It )
		{
			AMMOPlayer* Avatar = (*It);
			if( Avatar && Avatar->ActorName == TEXT("male_model") )
			{
				GFXPlayer->ShowAvatar = Avatar;
				GFXPlayer->ShowAvatar->WorldInfo = Player->WorldInfo;
				break;
			}
		}

	}
	if( GFXPlayer->ShowAvatar != NULL )
	{
		GFXPlayer->ShowAvatar->eventSetMesh( Player->GetClassId() , Player->GetSex() );
		ModelEquip();
	}		


	return;
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUICharacter::ModelEquip()
{
	return;	// no 3d avatar now!
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	for(int i = 0; i < pl->ItemManager->EquippedItemList_Normal.Num(); i++)
	{
		UEquip* tEquip = pl->ItemManager->EquippedItemList_Normal(i);
		if( tEquip != NULL && tEquip->MeshList.Num() > 0 )
		{			
			const UItem* tEquipTemplate = ( MMOFacade::getSingleton().FindItemTemplate( tEquip->ItemId ) );
			if(tEquipTemplate && GFXPlayer->ShowAvatar && GFXPlayer->ShowAvatar->ItemManager)
				GFXPlayer->ShowAvatar->ItemManager->RequestTakeOn( -1, i , true , tEquipTemplate ,false );
		}
	}

	if(GFXPlayer->ShowAvatar->ItemManager)
		GFXPlayer->ShowAvatar->ItemManager->UpdateOwnerData();
}

void UMMOUICharacter::PlayerMoneyChange(BYTE MoneyType,INT DeltaMoney)
{
	if(MoneyType == MONEYTYPE_FATIGUE)
	{		
		//this is a special attribute
		PlayerAttributeChange( SAIP_Fatigue , MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)MoneyType) );

	}
    else if( MONEYTYPE_MASTER_EXP == MoneyType )
    {
        //none currently
        //ValidSetBaseInfoText( "player_MasterExp",  FString::Printf(TEXT("%d"), Money) );
    }

	if(IsVisible){
		UpdateMoneyData();
	}
}

//
void UMMOUICharacter::LockItem( BYTE ContainerID, INT SlotID, INT isToLockIn )
{
	UGFxObject *pItemSlot = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , SlotID ) ) ;
	for( INT i = 0 ; i < E_NES_TOTAL ; i++){
		pItemSlot = showPanel->GetObject( FString::Printf( TEXT( "itemSlot_%d") , i ) ) ;

		if( pItemSlot == NULL ){
			continue;
		}

		INT serverSlotID = pItemSlot->GetInt( TEXT("itemid") );
		if( serverSlotID == SlotID ){
			pItemSlot->SetBool( TEXT("MarkLocked"), isToLockIn >= 0 );
			break;
		}
	}
}

void UMMOUICharacter::ResetCharactorData()
{
	UpdateTitleData(TRUE);
}

//====================================================================================================
//Physique enhancement


//------------------------------------------------------------------------------------------------------------------------
void UMMOUICharacter::PostAdvance ( FLOAT DeltaTime )
{
	//Super::PostAdvance(DeltaTime);


	// 	if(NeedInitSceneUI)
	// 	{
	// 		SysModelEquip();
	// 		NeedInitSceneUI = false;
	// 	}
	if(g_DelayRefresh>0)
	{
		g_DelayRefresh--;
		if(g_DelayRefresh==0)
		{
			g_UpdateArmorRefresh=0;
			UpdateArmorUI();
			UpdateRecover();
			UpdateAttributePanel();

			if(ResetCharactorDataFlag)
			{
				ResetCharactorData();
			}
			MMOUI::GetUIManager()->delegateUpdateMenuEffect(0,EMenuType_Charactor,FALSE,FALSE);
			PlayerBaseInfoChange();
		}
	}
	else
	{
		if(g_UpdateArmorRefresh>0)
		{
			g_UpdateArmorRefresh--;
			if(g_UpdateArmorRefresh==0)
			{
				UpdateArmorUI();
				UpdateRecover();
			}
		}
	}


	FRotator newRot;

	if( TurnButton == TBS_Left && GFXPlayer->ShowAvatar)
	{
		newRot = GFXPlayer->ShowAvatar->Rotation;
		newRot.Yaw -= 200;
		GFXPlayer->ShowAvatar->SetRotation(newRot);
	}
	else	if( TurnButton == TBS_Right && GFXPlayer->ShowAvatar)
	{
		newRot = GFXPlayer->ShowAvatar->Rotation;
		newRot.Yaw += 200;
		GFXPlayer->ShowAvatar->SetRotation(newRot);
	}

	if(QueryPowerCD>0.0f)
	{
		QueryPowerCD-=DeltaTime;
		if(QueryPowerCD<=0.0f)
		{

			AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
			if(player){
				CMessageHandler::GetInstance().Send_QueryPower(player->ServerID);
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------

void UMMOUICharacter::ASTitleTabClick(INT ShowType)
{

}

void UMMOUICharacter::ASUseCurrentTitle( INT TitleId )
{
	//TitleData* pTitle = MMOFacade::getSingleton().GetTitleData(TitleId);
	//if(pTitle){
	//	FString TitleName = pTitle->TitleName;// MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Title.TitleName"))  + appItoa(TitleId) );
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Title.EquipTitleMsg")) +  TitleName );
	//}
	CMessageHandler::GetInstance().Send_EquipTitle(TitleId);
}

//=====================================================================================================
void UMMOUICharacter::ASShowTotalEnhancProp()
{
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	if( tipsPanel != NULL && pMainPlayer != NULL ){
		INT curTotalEnhanceLevel = EquipSlotManager::getSingleton().GetCurEnhanceHoleLv();
		tipsPanel->ShowEquipSlotEnhanceTips( pMainPlayer->GetClassId(), curTotalEnhanceLevel );
	}
}

//----------------------------------------------------------------------------------------------------
void UMMOUICharacter::ASHideTotalEnhancProp()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

//=====================================================================================================
//=====================================================================================================
//Physique enhancement
void UMMOUICharacter::ASSelectEquipSlot(INT slotId)
{
	if( _EnhcPanel == NULL )
	{
		return;
	}

	_EnhcPanel->SelectEnhanceSlot( slotId );
}

//=====================================================================================================
void UMMOUICharacter::ASEnhanceEquipSlot()
{
	if( _EnhcPanel == NULL )
	{
		return;
	}

	_EnhcPanel->ToEnhanceCurSelectedSlot();
}

//=====================================================================================================
void UMMOUICharacter::EnhanceEquipSlotCancel()
{
	EquipSlotManager::getSingleton().ClearFlag(EES_INCONFIRM);
}

//=====================================================================================================
void UMMOUICharacter::EnhanceEquipSlotOK()
{
	EquipSlotManager::getSingleton().ClearFlag(EES_INCONFIRM);

	//
	if( _EnhcPanel == NULL )
	{
		return;
	}

	_EnhcPanel->RealEnhanceCurEquipSlot();
}

//=====================================================================================================
void UMMOUICharacter::ASShowTips_EquipSlot(INT id)
{

}

void UMMOUICharacter::ASHideTips_EquipSlot(INT id)
{

}

//=====================================================================================================
void UMMOUICharacter::ASShowTips_ActiveIcon(class UGFxObject* mc)
{
	if( _EnhcPanel == NULL )
	{
		return;
	}

	_EnhcPanel->ShowActiveTips();
}
//=====================================================================================================
void UMMOUICharacter::ASShowTips_EnhanceGuide( UGFxObject *mc )
{
	if( _EnhcPanel == NULL || mc == NULL )
	{
		return;
	}

	FString tipsText = _EnhcPanel->FormatEnhanceGuideTips();
	mc->SetString( TEXT("tipsHtml"), tipsText );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_PlainText, mc );
}

void UMMOUICharacter::ASHideTips_EnhanceGuide( UGFxObject *mc )
{
	MMOUI::GetUIManager()->delegateHideTips();
}