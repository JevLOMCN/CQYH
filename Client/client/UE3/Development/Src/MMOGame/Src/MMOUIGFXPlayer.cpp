#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOMsgBoxManager.h"
#include "MMOMapDataManager.h"

IMPLEMENT_CLASS(UMMOUIGFXPlayer)

void UMMOUIGFXPlayer::InitMMOUIWidgets()
{
	//TArray<FString> CustomLoadList;
	//GConfig->GetArray(TEXT("LoadInGame"), TEXT("SwfMovie"), CustomLoadList, GUIIni);
	//
	//for (INT i = 0; i < CustomLoadList.Num(); ++i)
	//{
	//	FMovieInfoProxy proxy(EC_EventParm);
	//	proxy.pMovie = LoadEx(*CustomLoadList(i), NULL, FALSE);
	//	if(proxy.pMovie)
	//	{
	//		StartEx(proxy.pMovie);
	//		CustomLoadMovieList.AddItem(proxy);
	//	}
	//	else
	//	{
	//		warnf(NAME_Error, TEXT("!!!!! Load custom swf movie failed:%s"), *CustomLoadList(i));
	//	}
	//}

	//eventRegisterEndEx();
}

// void UMMOUIGFXPlayer::UnInitMoviePlayer()
// {
// 	for(INT i = 0;i< UIObjectList.Num(); i++)
// 	{
// 		UIObjectList(i)->UnitWidget();
// 	}
// }

void UMMOUIGFXPlayer::CloseMMOUIWidgets()
{
	for (INT i = 0; i < CustomLoadMovieList.Num(); ++i)
	{
		CloseEx(CustomLoadMovieList(i).pMovie);
	}

	CustomLoadMovieList.Remove(0, CustomLoadMovieList.Num());
}

//------------------------------------------------------------------------
// void UMMOUIGFXPlayer::RegisterWidget(UMMOUIBase* UIMenu)
// {
// 	printf("RegisterWidget");	
// }

// void UMMOUIGFXPlayer::UseItem( INT ItemSlot )
// {
// 	UItem* SelItem = _Bag->GetItemInCurBag( ItemSlot );
// 	if( SelItem )
// 	{
// 		/*if( SelItem->ItemType == EIT_SPELLBOOK )
// 		{
// 			int iBag = _Bag->GetBagEnum();
// 			SkillBookMsgBox( ItemSlot );
// 			return;
// 		}*/
// 		CMessageHandler::GetInstance().Send_UseItem( 
// 			_Bag->GetBagEnum(), ItemSlot, SelItem->ItemId );
// 	}
// }



// void UMMOUIGFXPlayer::MoveItem(INT SrcBag,INT SrcSlot,INT DesBag,INT DesSlot)
// {
// 	UMMOGFXPlayer_EquipEnhance *pEquipEnhancPly;
// 	pEquipEnhancPly = (UMMOGFXPlayer_EquipEnhance*)MMOUI::FindMoviePlayer( TEXT("EquipEnhance") );
// 	UMMOUIEquipEnhance *pEquipEnhc;
// 	pEquipEnhc = pEquipEnhancPly->_EquipEnhance;
// 
// 	if( pEquipEnhc && pEquipEnhc->GetBool( TEXT("visible") ) &&//equipEnhancPly->IsVisible && 
// 		( pEquipEnhc->CheckInUse( SrcSlot )||pEquipEnhc->CheckInUse( DesSlot ) ) ){
// 			AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
// 			if(ctrl&&ctrl->StringMap){
// 				FScriptDelegate func(EC_EventParm);
// 				func.Object = NULL;
// 				func.FunctionName = "";
// 				MMOUI::GetUIManager()->delegateAddMessageBox( ctrl->StringMap->eventGetStrByName("MMOGame.EnhanceGeneral.EnhcGenera0"), 0, func, func );
// 			}
// 		return;
// 	}
// 	CMessageHandler::GetInstance().Send_MoveItem(DesBag,DesSlot,SrcBag,SrcSlot);
// }

//----------------------------------------------------------------------------------------------------------
void UMMOUIGFXPlayer::DeleteItem( INT ConId , INT SlotId )
{
	//AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	
	//CMessageHandler::GetInstance().Send_DeleteItem( ConId , SlotId , 1 );
}
//
void UMMOUIGFXPlayer::ModelEquip()
{
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if(!pl)
	{
		return;
	}

	if(!pl->ItemManager)
	{
		return;
	}

	//for(int i = 0;i< pl->ItemManager->EquippedItemList_Avatar.Num() ;i++)
	//{
	//	UEquip* tEquip = pl->ItemManager->EquippedItemList_Avatar(i);

	//	if( tEquip != NULL && tEquip->MeshList.Num() >0  )
	//	{						
	//		const UItem* tEquipTemplate = ( MMOFacade::getSingleton().FindItemTemplate( tEquip->ItemId )  );
	//		if(tEquipTemplate && ShowAvatar)
	//			ShowAvatar->ItemManager->RequestTakeOn( -1,i , false , tEquipTemplate ,false );
	//	}
	//}	
	for(int i = 0;i< pl->ItemManager->EquippedItemList_Normal.Num() ;i++)
	{
		UEquip* tEquip = pl->ItemManager->EquippedItemList_Normal(i);
		if( tEquip != NULL && tEquip->MeshList.Num() >0 )
		{			
			const UItem* tEquipTemplate = ( MMOFacade::getSingleton().FindItemTemplate( tEquip->ItemId ) );
			if(tEquipTemplate && ShowAvatar && ShowAvatar->ItemManager)
			{
				ShowAvatar->ItemManager->RequestTakeOn( -1,i , true , tEquipTemplate ,false );
			}
		}
	}	
	
	if(ShowAvatar && ShowAvatar->ItemManager)
	{
		ShowAvatar->ItemManager->UpdateOwnerData();
	}
}

//------------------------------------------------------------------------
INT UMMOUIGFXPlayer::CheckInputCharSize(const FString& TextContent)
{
	int size = appStrlen( TCHAR_TO_UTF8(*TextContent) );
	return size;
}
//------------------------------------------------------------------------
void UMMOUIGFXPlayer::TickHud( UCanvas* cvs ,float fDeltaTime )
{
	//FVector2D Screensize;
	

	if(PlayController == NULL)
	{
		return;
	}
	if(PlayController->Pawn == NULL)
	{
		return;
	}
	if( NeedReInitSceneUI )
	{
		//eventInitSceneUI();
		NeedReInitSceneUI = false;
	}
	//No need to Update when not shown
	if( !MPVisible )
		return;


	////mini Map
	//UMMOGFXPlayer_Minimap *pMinimapPlyer;
	//UMMOUIMinimap *pMinimapPanel;
	//pMinimapPlyer = (UMMOGFXPlayer_Minimap*)MMOUI::FindMoviePlayer( TEXT("Minimap") );
	//pMinimapPanel = pMinimapPlyer->_Minimap;
	//if(pMinimapPanel )
	//{
	//	pMinimapPanel->UpdateMinimap();
	//}
	////world map
	//UMMOGFXPlayer_WorldMap *pWorldMapPlyer;
	//UMMOUIWorldMap *pWorldMapPanel;
	//pWorldMapPlyer = (UMMOGFXPlayer_WorldMap*)MMOUI::FindMoviePlayer( TEXT("WorldMap") );
	//pWorldMapPanel = pWorldMapPlyer->_WorldMap;
	//if( pWorldMapPanel && pWorldMapPanel->IsVisible )
	//{
	//	pWorldMapPanel->UpdateWorldMapContents();
	//}
	//if( SceneHint )
	//	SceneHint->UpdateSceneHint(cvs);
	//if( _TeamMate )
	//	_TeamMate->UpdateTeamMateInfo(fDeltaTime);
	//if( MsgInfoPanel)
	//	MsgInfoPanel->TickWidget(fDeltaTime);
	//if( _ProgressBarQTEWidget && _ProgressBarQTEWidget->IsVisible )
	//	_ProgressBarQTEWidget->UpdateQTE(cvs);

}
//------------------------------------------------------------------------------
void UMMOUIGFXPlayer::MoveByKey( BYTE MoveType,UBOOL MoveFlag)
{
	////Only the button operating system
	//int nIndex=0;
	//for(int i=4;i>=0;i--)
	//{
	//	if(WSADKey[i] == MoveType)
	//	{
	//		nIndex = i;
	//		break;
	//	}
	//}

	//if(MoveFlag)
	//{
	//	for(int i=4;i>0;i--)
	//	{
	//		WSADKey[i] = WSADKey[i-1];
	//	}

	//	WSADKey[0] = MoveType;
	//}
	//else
	//{
	//	for(int i=nIndex;i<4;i++)
	//	{
	//		WSADKey[i] = WSADKey[i+1];
	//	}
	//	WSADKey[4] = 0;
	//}

	//if(PlayController == NULL)
	//{
	//	return;
	//}

	//PlayController->eventSetMoveModeByKey(WSADKey[0]);

}



void UMMOUIGFXPlayer::ASSetPkMode(int PkMode)
{
	INT _PkModeLevel = 8;
	GConfig->GetInt( TEXT("PkMode") , TEXT("PkModeLevel") , _PkModeLevel , GDefaultGameIni );

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player&&player->GetActorLevel()>=_PkModeLevel){
		FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
		if (curMapInfo.OpenMapPkMode)//if (curMapInfo.HideRoleName)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage7")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage7"));
		}
		else{
			CMessageHandler::GetInstance().Send_PkMode((UINT8)PkMode);
		}
	}
	else
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage3")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage3"));
	}

	//AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();	
	//if(ctrl&&ctrl->GetPlayer())
	//{
	//	ctrl->GetPlayer()->ActorAttributes->PkMode = PkMode;	
	//	MMOFacade::getSingleton().NotifyPlayerPkModeChange();
	//}
}

//Add pickup item hint
void UMMOUIGFXPlayer::AddPickItemInfo( int ItemId )
{
	if( PickItemInfoList.Num() <= 0 )
		PlayGetItemInfo( ItemId );
	else if( !mcPickItemInfo->GetBool("visible")  )
		PlayGetItemInfo( ItemId );
	PickItemInfoList.AddItem( ItemId );
}

//Play pickup item hint
void UMMOUIGFXPlayer::PlayGetItemInfo( int ItemId )
{
	const UItem* tItem = MMOFacade::getSingleton().FindItemTemplate( ItemId );
	class UItem* item = const_cast<UItem*>( tItem ); 
	UGFxObject *tObj=NULL;
	UGFxObject *tInfo=NULL;

	if( mcPickItemInfo == NULL )
	{
		mcPickItemInfo = GetVariableObject( TEXT("root.hintGetItem_mc") );
	}
	if( mcPickItemMsg == NULL )
	{
		mcPickItemMsg = GetVariableObject( TEXT("root.hintGetItem_mc.ItemInfo") );
	}
	if( mcPickItemInfo != NULL && mcPickItemMsg != NULL && item != NULL )
	{
		tObj = mcPickItemMsg->GetObject( TEXT("itemSlot") );
		if( tObj )
		{
			eventCallSetItemData( tObj,0, MMOFacade::getSingleton().GetItemIconNamePersonalized( item ),item->ItemId, ESPType_Bag ,TEXT("") );
		}
		tObj = mcPickItemMsg->GetObject( TEXT("ItemName") );
		if( tObj )
		{
			tObj->SetText( item->GetDisplayName()  );
		}
		mcPickItemInfo->GotoAndPlay( TEXT("GetItem") );
	}
	else
		ASGetItemEnded();
}

//Pickup item hint playback finished notification
void UMMOUIGFXPlayer::ASGetItemEnded()
{
	return;
	PickItemInfoList.Remove( 0,1 );
	if( PickItemInfoList.Num()>0 )
		PlayGetItemInfo( PickItemInfoList(0) );
}

//----------------------------------------------------------------------------------------
#if M_TEMP_ENABLE_UI_CODE
void UMMOUIGFXPlayer::MouseRightClickItem(class UGFxObject* mc)
{
	if( !mc )
	{
		return;
	}

	INT McType =  mc->GetInt( TEXT("SlotType") );	
	INT idx = mc->GetFloat( TEXT("itemid") );
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if( !pl )
	{
		return;
	}
	
	switch( McType )
	{
	case ESPType_Bag:
		if( !_Bag )
		{
			return;
		}
		else if( Warehouse && Warehouse->GetBool( TEXT("visible") ) )
		{
			INT DesSlotID = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_BANK);
			if( DesSlotID >= 0 )
			{
				MoveItem( _Bag->GetBagEnum() , idx ,  E_CONSLOT_BANK , DesSlotID);	
			}
		}
		else //equip directly
		{
			switch( _Bag->GetBagEnum() )
			{
			case E_CONSLOT_DEFAULT:
				UseItem( idx );
				break;
			case E_CONSLOT_EQUIPMENT:
				{
					UEquip* eq = Cast<UEquip>( _Bag->GetItemInCurBag( idx ) );
					if( eq != NULL )
					{
						INT ConSlot = 0;
						INT ItemSlot = 0;
						pl->ItemManager->GetEquippedSlotByEquipType( eq->EquipType, ConSlot ,ItemSlot );
						MoveItem( E_CONSLOT_EQUIPMENT , idx , ConSlot , ItemSlot );
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case ESPType_Charactor:
		{
			UEquip* eq = Cast<UEquip>( pl->ItemManager->GetEquipBySlot(idx,true) );
			if( eq != NULL )
			{			
				INT EmptySlot = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_EQUIPMENT);
				if( EmptySlot >= 0 )
				{
					MoveItem( E_CONSLOT_EQUIPPED , idx , E_CONSLOT_EQUIPMENT , EmptySlot );
				}
			}
		}
		break;
	case ESPType_Hero:
		{
			AMMOPlayer* hero = MMOFacade::getSingleton().GetMainHero();
			UEquip* eq = Cast<UEquip>( hero->ItemManager->GetEquipBySlot(idx,true) );
			if( eq != NULL )
			{			
				INT EmptySlot = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_HERO_EQUIPMENT);
				if( EmptySlot >= 0 )
				{
					MoveItem( E_CONSLOT_HERO_EQUIPPED , idx , E_CONSLOT_HERO_EQUIPMENT , EmptySlot );
				}
			}
		}
		break;
	case ESPType_Warehouse:
		{
			UItem* tItem = pl->ItemManager->GetItemBySlot( E_CONSLOT_BANK , idx );
			if(tItem)
			{
				INT EmptySlot = 0;
				switch( tItem->ItemType)
				{
				case E_ITEMTYPE_EQUIP:
					EmptySlot = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_EQUIPMENT);
					if(EmptySlot >= 0)
					{
						MoveItem( E_CONSLOT_BANK , idx , E_CONSLOT_EQUIPMENT , EmptySlot );
					}				
					break;
				case E_ITEMTYPE_QUEST:
					/*EmptySlot = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_QUEST);
					if(EmptySlot >= 0)
					{
						MoveItem( E_CONSLOT_BANK , idx , E_CONSLOT_QUEST , EmptySlot );
					}	*/
					break;
				case E_ITEMTYPE_MATERIAL:
					/*EmptySlot = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_QUEST);
					if(EmptySlot >= 0)
					{
						MoveItem( E_CONSLOT_BANK , idx , E_CONSLOT_QUEST , EmptySlot );
					}*/	
					break;
				default: 
					EmptySlot = MMOFacade::GetSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);
					if(EmptySlot >= 0)
					{
						MoveItem( E_CONSLOT_BANK , idx , E_CONSLOT_DEFAULT , EmptySlot );
					}	
					break;
				}
			}
		}
		break;
		//Enhancement
	case ESPType_Enhance:
		{
			UMMOGFXPlayer_EquipEnhance *pEquipEnhancPly;
			pEquipEnhancPly = (UMMOGFXPlayer_EquipEnhance*)MMOUI::FindMoviePlayer( TEXT("EquipEnhance") );
			UMMOUIEquipEnhance *pEquipEnhc;
			pEquipEnhc = pEquipEnhancPly->_EquipEnhance;

			INT slotIdx = (INT)mc->GetFloat( TEXT("EquipID") );
			//if the Enhance/Unenhance item be token
			if( slotIdx == 0 ){
				pEquipEnhc->eventResetPanelInfo();
			}
			else{//this is the auxiliary material slot event
				pEquipEnhc->SetAuxiliarySlotData( -1, NULL );
				//update
				pEquipEnhc->UpdateEquipInfo();
			}

		}
		break;
	}

}
#endif

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIGFXPlayer::ASThrowSlot( UGFxObject* mc )
{

	INT ConId = 0;
	if( !mc )
	{
		return;
	}
	INT SlotType = mc->GetInt( TEXT("SlotType") );
	INT SlotId      = mc->GetInt( TEXT("itemid") );

	FScriptDelegate Fun(EC_EventParm);
	Fun.Object = this;

	FScriptDelegate FunNone(EC_EventParm);
	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if( !Player || !Player->ItemManager || !PlayController )
	{
		return;
	}

	UItem* Item = NULL;
	UGFxObject* MsgBox = NULL;
	
	FString InputText = FString::Printf( TEXT("%s%s") , *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PleaseInput") ),
		*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.DestroyAffirmText") ) );

	switch( SlotType )
	{
// 	case ESPType_Bag:
// 		ConId = _Bag->GetBagEnum();
// 		Item = Player->ItemManager->GetItemBySlot(ConId,SlotId);
// 		if( Item )
// 		{
// 			if( Item->ItemColor >= EIC_Color4 )
// 			{
// 				Fun.FunctionName = "OnInputThrowItemOk";				
// 				MsgBox = MMOUI::GetUIManager()->delegateAddInputMessageBox( StringMap->eventGetGamePlayStrByName( TEXT("ThrowItem") ),ConId, InputText , Fun ,FunNone );
// 			}
// 			else
// 			{
// 				Fun.FunctionName = "OnThrowItemOk";
// 				MsgBox = MMOUI::GetUIManager()->delegateAddMessageBox( PlayController->StringMap->eventGetGamePlayStrByName( TEXT("ThrowItem") ), ConId , Fun ,FunNone );
// 			}
// 		}
// 
// 		if( MsgBox != NULL )
// 		{
// 			MsgBox->SetInt( TEXT("itemdataid") , SlotId );
// 		}
// 		break;

//		if( MsgBox != NULL )
//		{
//			MsgBox->SetInt( TEXT("itemdataid") , SlotId );
//		}
//		break;
	case ESPType_Charactor:
		ConId = mc->GetInt( TEXT("EquipID"));
		Item = Player->ItemManager->GetEquipBySlot( SlotId , TRUE );
		if( Item )
		{
			if( Item->ItemColor >= E_ITEMCOLOR_BLUE )
			{
				Fun.FunctionName = "OnInputThrowItemOk";
				
				MsgBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ThrowItem") ) , ConId, InputText , Fun ,FunNone, eInputFormatAll );
			}
			else
			{
				Fun.FunctionName = "OnThrowItemOk";
				MsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ThrowItem") ) , ConId , Fun , FunNone );
			}
		}

		if( MsgBox != NULL )
		{
			MsgBox->SetInt( TEXT("itemdataid") , SlotId );
		}
		break;

	case ESPType_Warehouse:
		ConId = E_CONSLOT_BANK;
		Item = Player->ItemManager->GetItemBySlot(ConId,SlotId);
		if( Item )
		{
			if( Item->ItemColor >= E_ITEMCOLOR_BLUE )
			{
				Fun.FunctionName = "OnInputThrowItemOk";				
				MsgBox = MMOMsgBoxManager::getSingleton().AddInputMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ThrowItem") ), ConId, InputText , Fun ,FunNone, eInputFormatAll );
			}
			else
			{
				Fun.FunctionName = "OnThrowItemOk";
				MsgBox = MMOMsgBoxManager::getSingleton().AddMessageBox( MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ThrowItem") ), ConId , Fun ,FunNone );
			}
		}
		if( MsgBox != NULL )
		{
			MsgBox->SetInt( TEXT("itemdataid") , SlotId );
		}
		break;
	}
}
//------------------------------------------------------------------------
void UMMOUIGFXPlayer::SendEnterMap(INT Level)
{
	CMessageHandler::GetInstance().Send_EnterDungeon(Level);
}
//------------------------------------------------------------------------
void UMMOUIGFXPlayer::SendMapDiffChange(INT Level)
{
	CMessageHandler::GetInstance().Send_SelMapDifficulty(Level);
}

void UMMOUIGFXPlayer::NativeASOnUI( UBOOL bOnUI , const FString& McName)
{



}
