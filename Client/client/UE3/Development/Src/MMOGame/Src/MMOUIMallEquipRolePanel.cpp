#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MallManager.h"
#include "UILayout.h"
#include "UITextParser.h"
#include "LinkManager.h"

IMPLEMENT_CLASS(UMMOUIMallEquipRolePanel)


void UMMOUIMallEquipRolePanel::Init()
{
	Super::Init();

}
void UMMOUIMallEquipRolePanel::UnInit()
{
	Super::UnInit();
	if(ShowLevel)
		ShowLevel->SetEnabled(FALSE);
	ShowPlayer = NULL;
	takeBtn    = NULL;
}

void UMMOUIMallEquipRolePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	bEquipIng = FALSE;
	if(v)
	{
		EnableCapture(TRUE);
		FVector2D scrSz = MMOFacade::GetScreenSize();
		SetPosition((scrSz.X - eventGetWidth())/2-310,(scrSz.Y - eventGetHeight())/2);
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleRoomingTitle")));	
	}
	else
	{
		if(ShowLevel)
			ShowLevel->SetEnabled(FALSE);
		ShowPlayer = NULL;
	}
}

void UMMOUIMallEquipRolePanel::NativeInitLoad()
{	
	Super::NativeInitLoad();
}

void UMMOUIMallEquipRolePanel::OnShowMallEquipPanel(INT nGoodsID) 
{
	const UItem* item = MMOFacade::getSingleton().FindItemTemplate(nGoodsID);
	if(item == NULL)
		return;
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL)
		return;
	//Class mismatch
	EPlayerClass job = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;
	if(!(MMO::CanBeUsedByPlayerClass(job, item->RequireClass)))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleRoomingNoJob")),MsgInfoType_Table,TEXT("MMOGame.Mall.EquipRoleRoomingNoJob"));
		return;
	}
	//Gender mismatch
	EGenderType gender = (EGenderType)mainPlayer->ActorAttributes->GenderType;
	if(!((item->RequireGender == EGenderType::EGT_None) || (item->RequireGender == gender)))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleRoomingNoGender")),MsgInfoType_Table,TEXT("MMOGame.Mall.EquipRoleRoomingNoGender"));
		return;
	}
	/*
	
	*/
	FLOAT fScale = 1.2f;
	if(gender == EGenderType::EGT_Female)
		fScale = 1.4f;
	if((item->ItemType == TEXT("ITEMTYPE_EQUIP")||item->ItemSubType == TEXT("ITEMSUBTYPE_USABLE_MISC"))&&!IsVisible)
		Show(TRUE);
	if(item && ShowPlayer)
	{
		UEquip* TempEquip = (UEquip*)(item);
		if(item->ItemSubType == TEXT("ITEMSUBTYPE_USABLE_MISC"))
		{
			INT nMountID = MallManager::getSingleton().GetMountIDByItemID(nGoodsID);
			if(nMountID > 0)
			{
				ShowPlayer->EquipMount(nMountID);
				bEquipIng = TRUE;
			}
		}
		else if(item->ItemType == TEXT("ITEMTYPE_EQUIP")&&TempEquip&&TempEquip->GetEquipAppearenceTemplate())
		{
			bEquipIng = TRUE;
			if(ShowPlayer->ActorAttributes&&ShowPlayer->ActorAttributes->MountID > 0&&(NES_MAIN_HAND == TempEquip->GetEquipAppearenceTemplate()->EquipType))
				ShowPlayer->UnEquipMount();
			ShowPlayer->ItemManager->RequestTakeOn( -1, TempEquip->GetEquipAppearenceTemplate()->EquipType , true , item ,false );
		}
	}
	if(bEquipIng&&ShowPlayer)
	{
		FName tag( "C_01", FNAME_Find );
		if( ShowPlayer->UseingAvatar )
			tag = ShowPlayer->UseingAvatar->PlacementNodeTag;
		// change to dynamic lighting
		if( ShowPlayer->LightEnvironment )
			ShowPlayer->LightEnvironment->SetEnabled( FALSE );
		ShowLevel->SetShowActor( ShowPlayer, tag );
		if(ShowPlayer->ActorAttributes->MountID > 0)
		{
			fScale = 0.9f;
			FRotator newRot =  ShowPlayer->Rotation;
			newRot.Yaw -= 10000;
			ShowPlayer->SetRotation(newRot);
		}
		ShowPlayer->DrawScale =  fScale;
		if(takeBtn&&bEquipIng)
			takeBtn->SetString( TEXT("label"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleRoomingBtnTakeOff")));
		//ShowPlayer->PlayAnimation(TEXT("Idle"),0,1,TRUE);
		return;
	}
	Show(FALSE);
}

void UMMOUIMallEquipRolePanel::ModelEquip()
{
	if(ShowPlayer == NULL || ShowPlayer->ItemManager == NULL)
		return;
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if(pl == NULL || pl->ItemManager == NULL)
		return;

	for(int i = 0; i < pl->ItemManager->EquippedItemList_Normal.Num(); i++)
	{
		UEquip* tEquip = pl->ItemManager->EquippedItemList_Normal(i);
		if( tEquip != NULL && tEquip->MeshList.Num() > 0 )
		{			
			const UItem* tEquipTemplate = ( MMOFacade::getSingleton().FindItemTemplate( tEquip->ItemId ) );
			if(tEquipTemplate && ShowPlayer)
				ShowPlayer->ItemManager->RequestTakeOn( -1, i , true , tEquipTemplate ,false );
		}
	}

	ShowPlayer->ItemManager->UpdateOwnerData();
}
void UMMOUIMallEquipRolePanel::SysModelEquip()
{
	if(GIsPlayInEditorWorld)
		return;
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL)
		return;

	// Run initialization once when ShowPlayer == NULL
	if( ShowPlayer == NULL )
	{
		eventClonePlayer(mainPlayer);
		if(ShowPlayer)
			ShowPlayer->PawnAnimInfo = mainPlayer->PawnAnimInfo;
	}
	if( ShowPlayer != NULL )
	{
		if(ShowPlayer->ActorAttributes)
			ShowPlayer->ActorAttributes->MountID = mainPlayer->ActorAttributes->MountID;
		ShowPlayer->NativeSetMesh( mainPlayer->GetClassId() , mainPlayer->GetSex() );
		ModelEquip();

		FLOAT fScale = 1.2f;
		EGenderType gender = (EGenderType)mainPlayer->ActorAttributes->GenderType;
		if(gender == EGenderType::EGT_Female)
			fScale = 1.4f;

		if(ShowPlayer->ActorAttributes)
		{
			BOOL bRiding = (ShowPlayer->ActorAttributes->MountID > 0);
			ShowPlayer->HideWeapon(bRiding);
			if(bRiding)
				fScale = 0.9f;		
		}
		ShowPlayer->DrawScale = fScale;
	}		
}
void UMMOUIMallEquipRolePanel::CreateShowLevel()
{
	if(ShowLevel == NULL )
		ShowLevel = GFXPlayer->eventGetShowLevel( TEXT("PawnShowLevel_Mall") );

	if( ShowLevel == NULL || ShowLevel->RenderTarget == NULL)
		return;
	ShowLevel->SetEnabled( true );
	GFXPlayer->SetExternalTexture( TEXT("img_HeroPic -nopack"),ShowLevel->RenderTarget);
}
void UMMOUIMallEquipRolePanel::EnableCapture(UBOOL b)
{
	CreateShowLevel();

	if( !ShowLevel  )
		return;
	if( b )
	{
		SysModelEquip();
		ShowLevel->SetEnabled( b );

		if(ShowPlayer == NULL)
			return;
		FName tag( "C_01", FNAME_Find );
		if( ShowPlayer->UseingAvatar )
			tag = ShowPlayer->UseingAvatar->PlacementNodeTag;
		// change to dynamic lighting
		if( ShowPlayer->LightEnvironment )
			ShowPlayer->LightEnvironment->SetEnabled( FALSE );
		ShowLevel->SetShowActor( ShowPlayer, tag );

		if(ShowPlayer&&ShowPlayer->ActorAttributes)
		{
			if(ShowPlayer->ActorAttributes->MountID > 0)
			{
				FRotator newRot =  ShowPlayer->Rotation;
				newRot.Yaw -= 10000;
				ShowPlayer->SetRotation(newRot);
			}
		}
	}
	else
	{
		ShowLevel->SetEnabled( b );
	}
}
void UMMOUIMallEquipRolePanel::OnUpdateTurn(BYTE turn)
{
	if(ShowPlayer == NULL)
		return;
	FRotator newRot;
	if( turn == TBS_Left)
	{
		newRot = ShowPlayer->Rotation;
		newRot.Yaw -= 200;
		ShowPlayer->SetRotation(newRot);
	}
	else if( turn == TBS_Right)
	{
		newRot = ShowPlayer->Rotation;
		newRot.Yaw += 200;
		ShowPlayer->SetRotation(newRot);
	}
}
void UMMOUIMallEquipRolePanel::OnUpdateRoleEquip(INT nGoodsID)
{
	if(bEquipIng)
	{
		EnableCapture(TRUE);
		if(takeBtn)
			takeBtn->SetString( TEXT("label"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleRoomingBtnTakeOn")));
		bEquipIng = FALSE;
	}
	else	
	{
		OnShowMallEquipPanel(nGoodsID);
		if(takeBtn&&bEquipIng)
			takeBtn->SetString( TEXT("label"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleRoomingBtnTakeOff")));
	}
}
UBOOL UMMOUIMallEquipRolePanel::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	if( IsVisible )
	{
		if(	strHoverControllerName=="mallEquiprole_mc" || strHoverControllerName=="ActorPic_mc")
		{
			if(ShowPlayer != NULL)
			{
				FLOAT fSacle = ShowPlayer->DrawScale;
				if(key == "MouseScrollUp")
				{
					fSacle += 0.05f;
				}
				else if (key == "MouseScrollDown")
				{
					fSacle -= 0.05f;
				}
			 
				if(fSacle > 1.6f)
					fSacle = 1.6f;
				else if (fSacle < 0.6f)
					fSacle = 0.6f;
				ShowPlayer->DrawScale = fSacle;
			}
			return TRUE;
		}
	}
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIMallEquipRolePanel::ASClickSendToChat()
{
	UMMOUIMallPanel* panel = (UMMOUIMallPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Mall);
	if (panel)
	{
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(panel->GoodsID);
		if(item)
		{
			FString txt = LinkManager::HtmlLink_ColorItem(item);
			UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
			if (Chat)
			{
				Chat->eventCallAppendInput(txt);
				MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.EquipRoleGoodsNameToChat")),MsgInfoType_Table,TEXT("MMOGame.Mall.EquipRoleGoodsNameToChat"));
			}
		}
	}
}
void UMMOUIMallEquipRolePanel::ASClickTakeOnOff()
{
	UMMOUIMallPanel* panel = (UMMOUIMallPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Mall);
	if (panel)
	{
		OnUpdateRoleEquip(panel->GoodsID);
	}
}
void UMMOUIMallEquipRolePanel::ASClickTurnRight()
{
	TurnButton = TBS_Right;
}
void UMMOUIMallEquipRolePanel::ASClickTurnLeft()
{
	TurnButton = TBS_Left;
}
void UMMOUIMallEquipRolePanel::ASReleasTurn()
{
	TurnButton = TBS_None;
}
/*
void UMMOUIMallEquipRolePanel::PostAdvance ( FLOAT DeltaTime )
{
	if(TurnButton == TBS_None)
		return;
	UGFxMoviePlayer::PostAdvance(DeltaTime);
	OnUpdateTurn(TurnButton);
}
*/
