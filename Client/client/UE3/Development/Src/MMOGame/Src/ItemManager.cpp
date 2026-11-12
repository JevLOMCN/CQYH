/*=============================================================================
	ItemManager.cpp
=============================================================================*/

#include "stdafx.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOLoginManager.h"
#include "MMOBuffManager.h"

typedef TArray< UEquipMesh* >	EquipMeshArray;

#define ReplaceMeshAfterMerge	1

/**
* FMergingHelper: manager class that handles merging stuffs.
* this is not supposed to be used by other modules.
*/
class FMergingHelper
{
public:
	FMergingHelper()
	{
		ResetCache();
	}

	~FMergingHelper()
	{
		ResetCache();
	}

	// create and init a new mesh to merge
	UMMOSkeletalMeshMerge* CreateNewMesh()
	{
		//local BodyPart bp;
		UMMOSkeletalMeshMerge* MergedMesh = Cast<UMMOSkeletalMeshMerge>(
			UObject::StaticConstructObject(UMMOSkeletalMeshMerge::StaticClass()));
		//new(Owner) class'MMOSkeletalMeshMerge';

		//MergedMesh->Init( 6 );
		//MergedMesh->Parts.Add( 6 );

		// merge texture size
		MergedMesh->TargetTexSizeX = 512;
		MergedMesh->TargetTexSizeY = 256;
		// face region
		FBodyPart part(EC_EventParm);
		part.Mesh = NULL;
		part.OrigX = 256;
		part.OrigY = 0;
		part.SizeX = 128;
		part.SizeY = 128;
		MergedMesh->Parts.AddItem(part);

		// chest region
		part.Mesh = NULL;
		part.OrigX = 0;
		part.OrigY = 0;
		part.SizeX = 256;
		part.SizeY = 256;
		MergedMesh->Parts.AddItem(part);
		// hands region
		part.Mesh = NULL;
		part.OrigX = 256;
		part.OrigY = 128;
		part.SizeX = 128;
		part.SizeY = 128;
		MergedMesh->Parts.AddItem(part);
		// thigh region
		part.Mesh = NULL;
		part.OrigX = 384;
		part.OrigY = 0;
		part.SizeX = 128;
		part.SizeY = 128;
		MergedMesh->Parts.AddItem(part);
		// feet region
		part.Mesh = NULL;
		part.OrigX = 384;
		part.OrigY = 128;
		part.SizeX = 128;
		part.SizeY = 128;
		MergedMesh->Parts.AddItem(part);
		// skirt region
		part.Mesh = NULL;
		part.OrigX = 128;
		part.OrigY = 128;
		part.SizeX = 128;
		part.SizeY = 128;
		MergedMesh->Parts.AddItem(part);

		return MergedMesh;
	}

	UMMOSkeletalMeshMerge* FindSameMesh( const UMMOSkeletalMeshMerge* other )
	{
		if( other == NULL )
			return NULL;

		for( UINT i = 0; i < TemplateCacheSize; ++i )
		{
			if( other->Equals( MergedMeshTemplates[i] ) )
			{
				return MergedMeshTemplates[i];
			}
		}
		return NULL;
	}

	void ResetCache()
	{
		for( UINT i = 0; i < TemplateCacheSize; ++i )
		{
			MergedMeshTemplates[i] = NULL;
		}
		
		LastAddIdx = TemplateCacheSize - 1;
	}

	void AddToCache( UMMOSkeletalMeshMerge* temp )
	{
		if( temp == NULL )
			return;

		// FIFO
		++LastAddIdx;
		if( LastAddIdx >= TemplateCacheSize )
			LastAddIdx = 0;

		if( MergedMeshTemplates[LastAddIdx] != NULL )
		{
			// remove from root so GC can do work
			MergedMeshTemplates[LastAddIdx]->RemoveFromRoot();
		}

		MergedMeshTemplates[LastAddIdx] = temp;

		// add to root to avoid been deleted while changing scenes
		temp->AddToRoot();
	}

private:
	// the size of the merged mesh template cache
	static const UINT	TemplateCacheSize = 20;

	// idx of the latest added template
	UINT	LastAddIdx;

	// merged mesh template cache
	UMMOSkeletalMeshMerge*	MergedMeshTemplates[TemplateCacheSize];
};

FMergingHelper GMergingHelper;

//const INT Old2NewMeshTypeMap[EBMT_None] = {
//	0,		//EBMT_Face   <DisplayName="EBMT_Face">,	// 0
//	1,		//EBMT_Chest  <DisplayName="EBMT_Chest">,	// 1
//	-1,		//EBMT_Hands  <DisplayName="EBMT_Hands">,	// 2
//	-1,		//EBMT_Thigh  <DisplayName="EBMT_Thigh">,	// 3
//	-1,		//EBMT_Feet  <DisplayName="EBMT_Feet">,	// 4
//	-1,		//EBMT_Waist  <DisplayName="EBMT_Waist">,	// 5
//
//	-1,		//EBMT_NoMerge  <DisplayName="EBMT_NoMerge">,
//
//	// parts don't need merge
//	2,		//EBMT_Hair  <DisplayName="EBMT_Hair">,
//	3,		//EBMT_Helmet  <DisplayName="EBMT_Helmet">,
//	-1,		//EBMT_LShoulder  <DisplayName="EBMT_LShoulder">,
//	-1,		//EBMT_RShoulder  <DisplayName="EBMT_RShoulder">,
//	4,		//EBMT_Weapon  <DisplayName="EBMT_Weapon">,
//	5,		//EBMT_Back	<DisplayName="EBMT_Back">,
//
//	6,		//EBMT_Weapon2	<DisplayName="EBMT_Weapon2>,
//	7,		//EBMT_BackOrnamental	<DisplayName="EBMT_BackOrnamental">,
//};

void UItemManager::SetSilhouette( UBOOL show, FVector sc )
{
	//for( INT i = EBMT_NoMerge + 1; i < EBMT_None; ++i )
	//{
	//	if( CurrentMeshList(i) != NULL )
	//		CurrentMeshList(i)->SetSilhouette( show, sc );
	//}

	if( !UseMergedMesh )
	{
		if( MergedComp != NULL && MergedComp->GetOwner() == Owner )
		{
			MergedComp->UpdateSilhouetteState( show, sc );
			//MergedComp->ShowSilhouette = show;
			//MergedComp->SilhouetteColor = sc;
		}
	}
}

UBOOL UItemManager::PrepareMerge()
{
	if( UseMergedMesh )
		return FALSE;

	EquipMeshArray meshArray[6];

#if ReplaceMeshAfterMerge
	// create a new merge-mesh
	//InitMergedMesh();
#endif	// ReplaceMeshAfterMerge

	// cancel the last merging 
	if( MergedMesh && !MergedMesh->bDoneMerge && bIsMerging )
	{
		MergedMesh->CancelMerge();
		bIsPreparing = FALSE;
		bIsMerging = FALSE;
		bUseCachedTemplate = FALSE;
	}

	MergedMesh = GMergingHelper.CreateNewMesh();

	if(GIsGame)
	{
		// iterate over currently worn equipment
		for( INT i = 0; i < EquippedItemList_Normal.Num(); ++i )
		{
			//INT texSizeX, texSizeY, texOrigX, texOrigY;
			if( EquippedItemList_Normal(i) == NULL )
				continue;

			for( INT j = 0; j < EquippedItemList_Normal(i)->MeshList.Num(); ++j )
			{
				if( EquippedItemList_Normal(i)->MeshList(j)->MeshType >= EBMT_NoMerge )
					continue;

				INT partIdx = (INT)EquippedItemList_Normal(i)->MeshList(j)->MeshType;

				INT k = 0;
				for( ; k < meshArray[partIdx].Num(); ++k )
				{
					if( meshArray[partIdx](k)->Priority > EquippedItemList_Normal(i)->MeshList(j)->Priority )
						break;
				}

				meshArray[partIdx].InsertItem( EquippedItemList_Normal(i)->MeshList(j), k );
			}
		}
	}

	for( INT i = 0; i < DefaultMeshList.Num(); ++i )
	{
		if( DefaultMeshList(i) == NULL )// || DefaultMeshList(i)->MeshType >= EBMT_NoMerge )
			continue;
		
		INT type = DefaultMeshList(i)->MeshType;
		// use shoulder armor in place of the face for merging
		if( ReplaceFaceByShoulder )
		{
			if( type == EBMT_LShoulder )
				type = EBMT_Face;
			else if( type == EBMT_Face )
				continue;
		}

		if( DefaultMeshList(i)->MeshType >= EBMT_NoMerge )
			continue;

		if( meshArray[type].Num() == 0 )
			meshArray[type].AddItem( DefaultMeshList(i) );
		else if( meshArray[type](0)->SkeletalMesh == NULL )
		{ // in case the item has no mesh data, use default mesh
			debugf( TEXT( "[MMO Error] No skeletal mesh set in equip %s." ), 
				*meshArray[type](0)->GetName() );
			meshArray[type](0) = DefaultMeshList(i);
		}
	}

	for( INT i = 0; i < 6; ++i )
	{
		MergedMesh->Parts(i).Mesh = NULL;
		MergedMesh->Parts(i).Textures.Empty(0);
		MergedMesh->Parts(i).Textures_Base.Empty(0);

		if( meshArray[i].Num() == 0 )
		{	// error
			//continue;
			if( i == 5 )
			{	// may have no skirt
				continue;
			}
			if( ReplaceFaceByShoulder && i == EBMT_Face )
			{	// may have no shoulder
				continue;
			}
			return FALSE;
		}

		USkeletalMesh* pMesh = meshArray[i](meshArray[i].Num()-1)->SkeletalMesh;
		MergedMesh->Parts(i).Mesh = pMesh;
		for( INT j = 0; j < meshArray[i].Num(); ++j )
		{
			// the first-layer equipment decides whether a skin texture is needed
			// Belt has no skin texture so drop the i==5 case
			if( j == 0 && i != 5 && ( !ReplaceFaceByShoulder || i != EBMT_Face ) && meshArray[i](j)->bNeedSkinTexture )
			{
				// these two textures should be prepared in advance
				MergedMesh->Parts(i).Textures.AddItem( SkinTextures(i) );
				MergedMesh->Parts(i).Textures_Base.AddItem( SkinTexturesBase(i) );
			}

			if( meshArray[i](j)->RegionTexture )
			{
				MergedMesh->Parts(i).Textures.AddItem( meshArray[i](j)->RegionTexture );
				MergedMesh->Parts(i).Textures_Base.AddItem( meshArray[i](j)->RegionTexture );
			}
			else if( j != 0 )
			{
				debugf( TEXT("Item has no region texture %s."), *meshArray[i](j)->GetName() );
				return FALSE;
			}
		}

		// in case no texture found for this region
		if( MergedMesh->Parts(i).Textures.Num() == 0 )
		{
			debugf( TEXT("Didn't find region texture for part %d."), i );
			MergedMesh->Parts(i).Textures.AddItem( SkinTextures(i) );
			MergedMesh->Parts(i).Textures_Base.AddItem( SkinTexturesBase(i) );
		}
	}

	// avoid doing this in editor, or the artists can't view texture/model changes to the avatar
	if( GIsGame && !GIsPlayInEditorWorld )
	{
		UMMOSkeletalMeshMerge* findMesh = GMergingHelper.FindSameMesh( MergedMesh );
		if( findMesh != NULL )
		{
			// already exists in the cache, no need to carry on
			MergedMesh = findMesh;
			return TRUE;
		}
	}

	return MergedMesh->PrepareTextures();
}

UBOOL UItemManager::CheckPrepareState()
{
	return MergedMesh->LastMergeCancelled() &&
		MergedMesh->IsSourceTexturesFullyLoaded();
}

void UItemManager::SendMoveItem(INT srcconslot,INT srcitemslot,INT desconslot,INT desitemslot)
{
	 CMessageHandler::GetInstance().Send_MoveItem(  desconslot , desitemslot , srcconslot ,srcitemslot);
}
//----------------------------------------------------------------------------------
UItem* UItemManager::GetItemBySlot(BYTE bagType,INT Slot)
{
	const TArray<UItem*>* bag = NULL;

	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
	case E_CONSLOT_HERO_EQUIPPED:
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		bag = &BagBankList;
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	case E_CONSLOT_RESOURCE:
		bag = &BagResourceList;
		break;
	default:
		break;
	}

	if( bag != NULL && bag->Num() > Slot )
	{
		return (*bag)(Slot);
	}
	return NULL;
}
//----------------------------------------------------------------------------------
UItem* UItemManager::GetItemById( int ItemId, BYTE bagType )
{
	const TArray<UItem*>* bag = NULL;

	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
	case E_CONSLOT_HERO_EQUIPPED:
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		bag = &BagBankList;
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	default:
		break;
	}

	if( bag != NULL )
	{
		for( int i = 0; i < bag->Num(); ++i )
		{
			if( (*bag)(i) == NULL )
				continue;
			if( (*bag)(i) && (*bag)(i)->ItemId == ItemId )
				return (*bag)(i);
		}
	}

	return NULL;
}

//add by JimZhang@snda.com
//----------------------------------------------------------------------------------
INT UItemManager::GetItemCountById( int ItemId, BYTE bagType )
{
	const TArray<UItem*>* bag = NULL;
	INT nCount = 0;

	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
	case E_CONSLOT_HERO_EQUIPPED:
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		bag = &BagBankList;
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	default:
		break;
	}

	if( bag != NULL )
	{
		for( int i = 0; i < bag->Num(); ++i )
		{
			if( (*bag)(i) == NULL )
				continue;
			if( (*bag)(i) && (*bag)(i)->ItemId == ItemId )
			{
				if((*bag)(i)->ItemDuration == E_DURTYPE_NONE || (*bag)(i)->ItemDuration == E_DURTYPE_EQUIP )
				{
					nCount ++;
				}
				else
				{
					nCount += (*bag)(i)->DefaultDuration;
				}
			}
		}
	}

	return nCount;
}


int UItemManager::FetchItem( UItem* pItem, BYTE bagType )
{
	TArray<UItem*>* bag = NULL;

	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
	case E_CONSLOT_HERO_EQUIPPED:
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	default:
		break;
	}

	if( bag != NULL )
	{
		for( int i = 0; i < bag->Num(); ++i )
		{
			if( (*bag)(i) == pItem )
			{
				(*bag)(i) = NULL;
				return i;
			}
		}
	}

	return -1;
}

//------------------------------------------------------------------------------------
void UItemManager::DeleteItemBySlot(BYTE bagType,INT slot)
{
	TArray<UItem*>* bag = NULL;

	if( slot < 0 )
		return;
	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
	case E_CONSLOT_HERO_EQUIPPED:
		//EquippedItemList_Normal(slot) = NULL;
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	case E_CONSLOT_RESOURCE:
		bag = &BagResourceList;
		break;
	default:
		break;
	}

	if( !bag )
	{
		return;
	}
	
	if( (*bag)(slot) )
	{
		//delete (*bag)(slot);
		(*bag)(slot) = NULL;
	}
}

void UItemManager::InsertItem( UItem* pItem, BYTE bagType, int slot )
{
	TArray<UItem*>* bag = NULL;

	if( slot < 0 || pItem == NULL )
		return;

	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		break;

	case E_CONSLOT_HERO_EQUIPMENT:
		bag = &BagHeroList;
		break;
	case E_CONSLOT_HERO_EQUIPPED:
		//bag = &EquippedItemList_Hero;
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	default:
		break;
	}

	if( bag != NULL && slot < bag->Num() && (*bag)(slot) == NULL )
	{
		(*bag)(slot) = pItem;
	}
}

int UItemManager::GetEmptyItemSlot( BYTE bagType )
{
	const TArray<UItem*>* bag = NULL;

	switch( bagType )
	{
	case E_CONSLOT_EQUIPPED:
		break;
	case E_CONSLOT_DEFAULT:
		bag = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	bag = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	bag = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		bag = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		break;
	case E_CONSLOT_HERO_EQUIPMENT:
		bag = &BagHeroList;
		break;
	case E_CONSLOT_HERO_EQUIPPED:
		//bag = &EquippedItemList_Hero;
		break;
	case E_CONSLOT_DEPOSIT:
		bag = &BagDepositList;
		break;
	default:
		break;
	}

	if( bag != NULL )
	{
		for( int i = 0; i < bag->Num(); ++i )
		{
			if( (*bag)(i) == NULL )
				return i;
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------------


UEquip* UItemManager::GetEquipBySlot( INT slot, UBOOL normalEquip )
{
	if( slot < 0 )
	{
		return NULL;
	}
	if( normalEquip )
		return EquippedItemList_Normal(slot);
	else
		return EquippedItemList_Avatar(slot);
}
//------------------------------------------------------------------------
UItem* UItemManager::RequestTakeOn(INT SlotId, INT TargetSlotId, UBOOL normalEquip, const class UItem* itemTemplate, UBOOL autoChagneBag)
{
	if (!Owner)
	{
		return NULL;
	}

	UItem* TempItem = 0;
	UItem* OldItem = 0;
	UEquip* TempEquip = 0;
	INT slotInBag = 0;

	UBOOL useAdvancedAppearence = IsAdvancedEquipAppearence(CONSLOT_DEFAULT, SlotId);

	if( SlotId == -1 )
	{
		TempItem = CreateItemByTemplate((UItem*)itemTemplate, useAdvancedAppearence);
	}
	else
	{
		TempItem = BagItemList(SlotId);
	}

	if(TempItem == NULL)
	{
		warnf(NAME_Error, TEXT("Can not find Item by Id:%d"), SlotId);
		return NULL;
	}

	TempEquip = Cast<UEquip>(TempItem);
	if(TempEquip == NULL)
	{
		warnf(NAME_Error, TEXT("Item to take on is not an fit equip:ID[%d] SlotID[%d] %s"),
			TempItem->ItemId, SlotId, *TempItem->GetDisplayName());
		return NULL;
	}

	//if(TempEquip->IsWeapon())
	//{
	//	EquippedItemList_Normal[E_NES_MAIN_HAND] = TempEquip->WeaponType;
	//}


	if( autoChagneBag && SlotId != -1 )
		slotInBag = FetchItem( TempItem, E_CONSLOT_DEFAULT );	// take out from equipment bag

	if( normalEquip )
	{
		if( TargetSlotId >=0 && TargetSlotId < EquippedItemList_Normal.Num())
		{
			OldItem = EquippedItemList_Normal(TargetSlotId);
			EquippedItemList_Normal(TargetSlotId) = TempEquip;
		}
	}
	else
	{
		if( TargetSlotId >=0 && TargetSlotId < EquippedItemList_Avatar.Num())
		{
			OldItem = EquippedItemList_Avatar(TargetSlotId);
			EquippedItemList_Avatar(TargetSlotId) = TempEquip;
		}
	}

	if( OldItem != NULL )
	{
		OldItem->OnTakeOff(Owner);
	}
	TempEquip->OnTakeOn(Owner, useAdvancedAppearence);

	// fix bug #22643
	if (MMOFacade::getSingleton().GetMMOBuffManager())
	{
		BuffDataMap* BuffMap = MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffMap(Owner->ServerID);
		if (BuffMap && BuffMap->Buffs.size() > 0)
		{
			Owner->RebuildAllMeshMaterialList();
			Owner->NeedRefreshMaterial = TRUE;

			// rebuilding the material affects hair color, set it again here
			ApplyHairColor(CurrentHairColorIdx);
		}
	}

	if (TargetSlotId == E_NES_MAIN_HAND && Owner->IsMainPlayer())
	{
		Owner->NotifyRefreshEquipLevel(TargetSlotId);
	}

	if( autoChagneBag && SlotId != -1 && slotInBag != -1 && OldItem != NULL )
		InsertItem( OldItem, E_CONSLOT_DEFAULT, slotInBag );

	if(TempEquip->GetEquipAppearenceTemplate()
		&& (TempEquip->GetEquipAppearenceTemplate()->EquipType == EET_Normal_MainHand) )
	{
		AMMOPlayer* pOwnerPlayer = Cast<AMMOPlayer>(Owner);
		if(pOwnerPlayer)
		{
			pOwnerPlayer->ReflashWeaponSocket();
		}
		Owner->RefreshAnimSet();
	}

	//UpdateOwnerData();

	return TempEquip;
}

UBOOL UItemManager::RequestTakeOff(INT SlotId, UBOOL normalEquip)
{
	if (!Owner)
	{
		return FALSE;
	}

	UItem* TempItem = NULL;
	UEquip* TempEquip = NULL;

	//if( bIsPreparing || bIsMerging )
	//{
	//	`log( "[MMO Error] Merging not finished when trying to take off clothes."@self );
	//	return false;
	//}

	if (normalEquip)
	{
		TempItem = EquippedItemList_Normal(SlotId);
	}
	else
	{
		TempItem = EquippedItemList_Avatar(SlotId);
	}

	if (!TempItem)
	{
		return FALSE;
	}

	TempEquip = Cast<UEquip>(TempItem);
	if (!TempEquip)
	{
		return FALSE;
	}

	TempEquip->OnTakeOff(Owner);

	//if(SlotID == E_NES_MAIN_HAND)
	//{
	//	Owner.ClearEquipEffects();
	//}

	if (normalEquip)
	{
		EquippedItemList_Normal(SlotId) = NULL;
	}
	else
	{
		EquippedItemList_Avatar(SlotId) = NULL;
	}

	// fix bug #22643
	if (MMOFacade::getSingleton().GetMMOBuffManager())
	{
		BuffDataMap* BuffMap = MMOFacade::getSingleton().GetMMOBuffManager()->GetBuffMap(Owner->ServerID);
		if (BuffMap && BuffMap->Buffs.size() > 0)
		{
			Owner->RebuildAllMeshMaterialList();
			Owner->NeedRefreshMaterial = TRUE;

			// rebuilding the material affects hair color, set it again here
			ApplyHairColor(CurrentHairColorIdx);
		}
	}

	if ((TempEquip->GetEquipAppearenceTemplate() != NULL) && 
		(TempEquip->GetEquipAppearenceTemplate()->EquipType == EET_Normal_MainHand))
	{
		if(Owner->bDead && Owner->IsPlayingAnyAnimation())
		{
		}
		else
		{
			Owner->RefreshAnimSet();
		}
	}

	//UpdateOwnerData();

	return TRUE;
}

//------------------------------------------------------------------------
void UItemManager::GetEquippedSlotByEquipType(INT EquipType,INT& tConSlot,INT& tItemSlot)
{
	MMOUI::GetEquippedSlotByEquipType(EquipType, tConSlot, tItemSlot);
}

//------------------------------------------------------------------------
UBOOL UItemManager::IsAdvancedEquipAppearence(BYTE bag, INT slot)
{
	AMMOPlayer* player = Cast<AMMOPlayer>(Owner);
	if (!player)
	{
		return FALSE;
	}

	if (player->IsMainPlayer())
	{
		if (slot == -1)
		{// when entering the character select screen, the equipment's iteminfo is unavailable, the server sends this value separately
			return player->WeaponUpgradePoint >= player->WeaponUpgradeTopLevel;
		}
		else
		{
			FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(bag, slot);
			if (itemInfo)
			{
				INT totalPoint = itemInfo->m_byLevelupPhyPoint + itemInfo->m_byLevelupMagPoint + itemInfo->m_byLevelupTaoPoint;
				return totalPoint >= player->WeaponUpgradeTopLevel;
			}
		}
	}
	else
	{
		return player->WeaponUpgradePoint >= player->WeaponUpgradeTopLevel;
	}

	return FALSE;
}
//------------------------------------------------------------------------
UItem* UItemManager::CreateItemByTemplate(class UItem* itemTemp, UBOOL UseAdvancedAppearence)
{
	UItem* tItem = 0;

	if( itemTemp == NULL)
		return NULL;

	UObject* newObj = StaticConstructObject(itemTemp->GetClass(), GetTransientPackage(), NAME_None, 0, itemTemp);
	tItem = Cast<UItem>(newObj);
	if(tItem != NULL)
	{				
#if 0
		for( UProperty* P=UEquip::StaticClass()->ConstructorLink; P; P=P->ConstructorLinkNext )
		{
			if( P->Offset < tItem->GetClass()->GetDefaultsCount() )
			{
				// skip if SourceOwnerObject != NULL and this is a transient property - in this
				// situation, the new value for the property has already been copied from the class defaults
				// in the block of code above
				if ( !P->HasAnyPropertyFlags(CPF_Transient|CPF_DuplicateTransient) )
				{
					// the heap memory allocated at this address is owned the source object, so
					// zero out the existing data so that the UProperty code doesn't attempt to
					// de-allocate it before allocating the memory for the new value
					//appMemzero( (BYTE*)newObj + P->Offset, P->GetSize() );//bad for bools, but not a real problem because they aren't constructed!!
					P->CopyCompleteValue( (BYTE*)newObj + P->Offset, (BYTE*)itemTemp + P->Offset, NULL, NULL, NULL );
				}
			}
		}
#endif
		tItem->Init(ClassType, GenderType, UseAdvancedAppearence);
		//`Log( "[MMO Debug] Created new item:"@itemTemp );
		return tItem;
	}
	return NULL;
}
//------------------------------------------------------------------------
// moved into FMergeHelpr
/*
void UItemManager::InitMergedMesh()
{
	//local BodyPart bp;
	MergedMesh = Cast<UMMOSkeletalMeshMerge>(
		StaticConstructObject(UMMOSkeletalMeshMerge::StaticClass()));
	//new(Owner) class'MMOSkeletalMeshMerge';

	//MergedMesh->Init( 6 );
	//MergedMesh->Parts.Add( 6 );

	// merge texture size
	MergedMesh->TargetTexSizeX = 512;
	MergedMesh->TargetTexSizeY = 256;
	// face region
	FBodyPart part(EC_EventParm);
	part.Mesh = NULL;
	part.OrigX = 256;
	part.OrigY = 0;
	part.SizeX = 128;
	part.SizeY = 128;
	MergedMesh->Parts.AddItem(part);

	// chest region
	part.Mesh = NULL;
	part.OrigX = 0;
	part.OrigY = 0;
	part.SizeX = 256;
	part.SizeY = 256;
	MergedMesh->Parts.AddItem(part);
	// hands region
	part.Mesh = NULL;
	part.OrigX = 256;
	part.OrigY = 128;
	part.SizeX = 128;
	part.SizeY = 128;
	MergedMesh->Parts.AddItem(part);
	// thigh region
	part.Mesh = NULL;
	part.OrigX = 384;
	part.OrigY = 0;
	part.SizeX = 128;
	part.SizeY = 128;
	MergedMesh->Parts.AddItem(part);
	// feet region
	part.Mesh = NULL;
	part.OrigX = 384;
	part.OrigY = 128;
	part.SizeX = 128;
	part.SizeY = 128;
	MergedMesh->Parts.AddItem(part);
	// skirt region
	part.Mesh = NULL;
	part.OrigX = 128;
	part.OrigY = 128;
	part.SizeX = 128;
	part.SizeY = 128;
	MergedMesh->Parts.AddItem(part);

	bIsPreparing = false;
	bIsMerging = false;
}
*/
//------------------------------------------------------------------------
void UItemManager::Reset()
{
	if( Owner == NULL )
		return;
	UAvatarData* pAvatarData = Owner->UseingAvatar;
	if( pAvatarData == NULL )
		return;

	UBOOL OldUseMergedMesh = UseMergedMesh;
	UseMergedMesh = pAvatarData->UseMergedMesh;

	if( Owner->ActorAttributes != NULL )
	{
		ClassType = Owner->ActorAttributes->ClassType;
		GenderType = Owner->ActorAttributes->GenderType;
	}

	if( CurrentMeshList.Num() == 0 )
		CurrentMeshList.AddZeroed(EBMT_None);
	if( EquippedItemList_Normal.Num() == 0 )
		EquippedItemList_Normal.AddZeroed(E_NES_TOTAL);
	//EquippedItemList_Avatar.AddZeroed(EAES_Count);
	if( EquippedItemList_Hero.Num() == 0 )
		EquippedItemList_Hero.AddZeroed(E_NES_TOTAL);
	if( CurCapacity.Num() == 0 )
		CurCapacity.AddZeroed(E_CONSLOT_COUNT);

	if( OldMeshList.Num() == 0 )
		OldMeshList.AddZeroed(EBMT_NoMerge);

	for(INT i = 0; i < E_NES_TOTAL; ++i )
	{
		if( /*EquippedItemList_Normal.Num() > i &&*/ EquippedItemList_Normal(i) != NULL )
		{
			EquippedItemList_Normal(i)->OnTakeOff( Owner );
			EquippedItemList_Normal(i) = NULL;
		}
	}

	for(INT i = 0; i < DefaultMeshList.Num(); ++i )
	{
		if (DefaultMeshList(i))
		{
			DefaultMeshList(i)->OnTakeOff( Owner, OldUseMergedMesh );
		}
	}

	DefaultMeshList.Remove( 0, DefaultMeshList.Num() );

	for( INT i = 0; i < CurrentMeshList.Num(); ++i )
	{
		if( CurrentMeshList(i) != NULL )
			CurrentMeshList(i)->OnTakeOff( Owner, OldUseMergedMesh );

		CurrentMeshList(i) = NULL;
	}

	if( !OldUseMergedMesh )
	{
		//BagEquipList.Remove( 0, BagEquipList.Num() );

		SkinTextures.Remove( 0, SkinTextures.Num() );
		SkinTexturesBase.Remove( 0, SkinTexturesBase.Num() );

		for( INT i = 0; i < OldMeshList.Num(); ++i )
		{
			OldMeshList(i) = NULL;
		}

		Owner->DetachComponent( MergedComp );
	}
	else
	{
	}

	CurrentHairColorIdx = -1;
	CurrentHairModelIdx = -1;
	CurrentFaceModelIdx = -1;
	CurrentSkinColorIdx = 0;

	/*if( Owner->ActorAttributes != NULL )
	{
	SetHair( Owner->ActorAttributes->HairType, Owner->ActorAttributes->HairColor );
	SetFace( Owner->ActorAttributes->FaceType );
	if( !UseMergedMesh )
	ApplySkinColor( Owner->ActorAttributes->SkinColor );
	}
	else
	{
	SetHair( 0, 0 );
	SetFace( 0 );
	if( !UseMergedMesh )
	ApplySkinColor( 0 );
	}*/

	//debugf(TEXT("[MMO Debug] ItemManager reset."));
}
//------------------------------------------------------------------------
void UItemManager::SetDefaultMeshes(class UAvatarData* InAvatarData,INT InKey)
{
	DefaultMeshList.Remove( 0, DefaultMeshList.Num() );
	DefaultMeshList.AddZeroed( InAvatarData->DefaultMeshListTemplate.Num() );
	if( !UseMergedMesh )
	{
		SkinTextures.Remove( 0, SkinTextures.Num() );
		SkinTextures.AddZeroed( EBMT_Waist );
		SkinTexturesBase.Remove( 0, SkinTexturesBase.Num() );
		SkinTexturesBase.AddZeroed( EBMT_Waist );
	}

	for(INT i = 0; i < InAvatarData->DefaultMeshListTemplate.Num(); ++i)
	{
		UEquipMesh* tEquipMesh = InAvatarData->DefaultMeshListTemplate(i);
		UEquipMesh* RandomEquipMesh = NULL;
		if( tEquipMesh != 0 )
		{
			// try to get a random equipmesh
			for (INT j = 0; j < InAvatarData->RandomMeshListTemplate.Num(); ++j)
			{
				if (tEquipMesh->MeshType == InAvatarData->RandomMeshListTemplate(j).MeshType)
				{
					INT Count = InAvatarData->RandomMeshListTemplate(j).EquipMeshList.Num() + 1;
					INT Idx = InKey % Count;
					if (Idx == 0)
					{
						RandomEquipMesh = tEquipMesh;
					}
					else
					{
						RandomEquipMesh = InAvatarData->RandomMeshListTemplate(j).EquipMeshList(Idx - 1);
					}
					break;
				}
			}

			if (RandomEquipMesh && RandomEquipMesh->MeshType == tEquipMesh->MeshType)
			{
				tEquipMesh = RandomEquipMesh;
			}


			DefaultMeshList(i) = Cast<UEquipMesh>(
				StaticConstructObject(tEquipMesh->GetClass(),
				/*GetTransientPackage()*/this, NAME_None, RF_Public | RF_Transactional, tEquipMesh));

			if( !UseMergedMesh )
				RefreshSkinTexture( i, FALSE );
		}
	}

	// avatar data is configured to use a pre-merged model, so shoulder armor must replace the face for merging here
	ReplaceFaceByShoulder = InAvatarData->MergeShoulder;
}

void UItemManager::RefreshSkinTexture( INT defaultMeshID, UBOOL colorOnly )
{
	if( defaultMeshID >= DefaultMeshList.Num() )
		return;

	UEquipMesh* pMesh = DefaultMeshList( defaultMeshID );
	if( pMesh == NULL || pMesh->MeshType >= 5/*SkinTextures.Num()*/ )
		return;

	// record skin color texture
	// for default equipment, UsedColorTextureID is the id of the texture used to look up Specular/Normal textures
	// the skin color texture actually used is decided by CurrentSkinColorIdx
	if( !colorOnly )
	{
		if( pMesh->UsedColorTextureID >= 0 &&
			pMesh->ColorTextures.Num() > pMesh->UsedColorTextureID )
		{
			SkinTexturesBase(pMesh->MeshType) =
				pMesh->ColorTextures( pMesh->UsedColorTextureID );
		}
		else
		{
			SkinTexturesBase(pMesh->MeshType) = NULL;
			debugf( TEXT("Failed to get skin base texture.") );
		}
	}
	if( CurrentSkinColorIdx >= 0 && pMesh->ColorTextures.Num() > CurrentSkinColorIdx )
	{
		SkinTextures(pMesh->MeshType) =
			pMesh->ColorTextures( CurrentSkinColorIdx );
	}
	else
	{
		//SkinTextures(DefaultMeshList(i)->MeshType) = NULL;
		SkinTextures(pMesh->MeshType) = 
			pMesh->ColorTextures.Num() > 0 ? 
			pMesh->ColorTextures(0) : NULL;
		debugf( TEXT("Skin color idx out of bound of the texture array.") );
	}
}

//void UItemManager::Clear()
//{
//	EquippedItemList_Normal.AddZeroed(E_NES_TOTAL);
//	EquippedItemList_Hero.AddZeroed(E_NES_TOTAL);
//
//	CurrentMeshList.AddZeroed(EBMT_None+1);	// 1 for a second weapon
//	OldMeshList.AddZeroed(EBMT_NoMerge);
//
//	CurCapacity.AddZeroed(E_CONSLOT_COUNT);
//}

//------------------------------------------------------------------------
void UItemManager::Init(class AMMOActor* OwnerTarget)
{
	// initialize array size
	CurrentMeshList.AddZeroed(EBMT_None);
	EquippedItemList_Normal.AddZeroed(E_NES_TOTAL);
	//EquippedItemList_Avatar.AddZeroed(EAES_Count);
	EquippedItemList_Hero.AddZeroed(E_NES_TOTAL);
	CurCapacity.AddZeroed(E_CONSLOT_COUNT);


	OldMeshList.Empty();
	OldMeshList.AddZeroed(EBMT_NoMerge);

 	Owner = OwnerTarget;

	if( !Owner->UseingAvatar )
	{
		// at init time MMOActor has no AvatarData yet, the initialization below is pointless since it is done again on Reset -ming
		return;
	}

	if( Owner->ActorAttributes != NULL )
	{
		ClassType = Owner->ActorAttributes->ClassType;
		GenderType = Owner->ActorAttributes->GenderType;
	}
	else
	{
		debugf( TEXT("[MMO Error] Owner hasn't its attributes, class & gender may be wrong.") );
		ClassType = EPCT_None;
		GenderType = EGT_None;
	}

	// determine whether to use the new outfit-change method
	UseMergedMesh = Owner->UseingAvatar->UseMergedMesh;

	//debugf( TEXT("[MMO Debug] Init Item Manager %s"), *Owner->GetName() );

	//BagEquipList.AddZeroed(Capacity);
	//BagItemList.AddZeroed(Capacity); //item bag
	//BagMaterialList.AddZeroed(Capacity); //material bag
	//BagQuestList.AddZeroed(Capacity); //quest bag
	//BagBankList.AddZeroed(Capacity);

	if( UseMergedMesh )
	{
	}
	else
	{
		//DefaultEquipList.Add(EquipSlotNumber);
		//OldMeshList.AddZeroed(EBMT_NoMerge);

		//InitDefaultEquips();
		//InitMergedMesh();

		if( MergedMesh )
		{
			if( MergedComp )
			{
				MergedComp->SetSkeletalMesh( NULL );
				//Owner->SetHidden( TRUE );
				Owner->SetVisibleByMergeMesh(FALSE);
			}
			MergedMesh = NULL;
		}

		bUseCachedTemplate = FALSE;

		UpdateApperance();
	}
}

int UItemManager::RequestExchangeItem(INT BagId,INT Item1Pos,INT Bag2Id,INT Item2Pos)
{
	UItem* Item1 = NULL;
	UItem* Item2= NULL;
	//local MMOPlayerController pc;
	TArray<class UItem*>* BagList1 = NULL;
	TArray<class UItem*>* BagList2 = NULL;

	switch(BagId)
	{
	case E_CONSLOT_DEFAULT:
		BagList1 = &BagItemList;
		break;
	//case E_CONSLOT_EQUIPMENT:
	//	BagList1 = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:
	//	BagList1 = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:
		BagList1 = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		BagList1 = &BagBankList;
		break;
	case E_CONSLOT_HERO_EQUIPMENT:
		BagList1 = &BagHeroList;
		break;
	case E_CONSLOT_HERO_EQUIPPED:
		//BagList1 = &EquippedItemList_Hero;
		break;
	case E_CONSLOT_DEPOSIT:
		BagList1 = &BagDepositList;
		break;
	case E_CONSLOT_RESOURCE:
		BagList1 = &BagResourceList;
		break;
	}

	switch(Bag2Id)
	{
	case E_CONSLOT_DEFAULT:
		BagList2 = &BagItemList;

		break;
	//case E_CONSLOT_EQUIPMENT:

	//	BagList2 = &BagEquipList;
	//	break;
	//case E_CONSLOT_MATERIAL:

	//	BagList2 = &BagMaterialList;
	//	break;
	/*case E_CONSLOT_QUEST:

		BagList2 = &BagQuestList;
		break;*/
	case E_CONSLOT_BANK:
		BagList2 = &BagBankList;
		break;
	case E_CONSLOT_HERO_EQUIPMENT:
		BagList2 = &BagHeroList;
		break;
	case E_CONSLOT_HERO_EQUIPPED:
		//BagList2 = &EquippedItemList_Hero;
		break;
	case E_CONSLOT_DEPOSIT:
		BagList2 = &BagDepositList;
		break;
	case E_CONSLOT_RESOURCE:
		BagList2 = &BagResourceList;
		break;
	}

	if( !BagList1 || !BagList2)
		return 0;
	Item1 = (*BagList1)(Item1Pos);
	Item2 = (*BagList2)(Item2Pos);
	 (*BagList2)(Item2Pos) = Item1;
	(*BagList1)(Item1Pos)= Item2;
	return 1;
}


void UItemManager::UpdateOwnerData( UBOOL bForce )
{
	if (!Owner)
	{
		return;
	}

	UBOOL origHiden = Owner->bHidden;

	if (Owner->bMainSkelChanged)
	{
		//Owner->SetHidden(TRUE);
		Owner->SetVisibleByMergeMesh(FALSE);
	}

	for (INT i = 0; i < DefaultMeshList.Num(); ++i)
	{
		UEquipMesh* tMesh = DefaultMeshList(i);
		if (!tMesh)
		{
			continue; 
		}

		// ming
		if( CurrentMeshList(tMesh->MeshType) == NULL )
		{
			CurrentMeshList(tMesh->MeshType) = tMesh;
			tMesh->OnTakeOn(Owner, UseMergedMesh);
			//CurrentMeshList(tMesh->MeshType) = tMesh;
		}
	}

	if ((bForce || eventCheckNeedRemerge()) && PrepareMerge())
	{
		if( MergedMesh->bDoneMerge )
		{
			// merging is done, this mesh has already been merged before
			bUseCachedTemplate = TRUE;
			bIsPreparing = FALSE;
			bIsMerging = FALSE;
		}
		else
		{
			// tell the tick function to start merging
			bIsPreparing = TRUE;
			bIsMerging = FALSE;
			MergePrepareTime = 0.f;
			bUseCachedTemplate = FALSE;
		}
	}
	else
	{
		//if (Owner->bMainSkelChanged && Owner->bVisible)
		//{
		//	Owner->SetHidden(origHiden);
		//}
		if (Owner->bMainSkelChanged)
		{
			Owner->SetVisibleByMergeMesh(TRUE);
		}
	}

	Owner->bMainSkelChanged = FALSE;

	if( !UseMergedMesh )
	{
		for (INT i = 0; i < EBMT_NoMerge; ++i)
		{
			OldMeshList(i) = CurrentMeshList(i);
		}
	}
	ApplyEmissiveSilhouetteFactor( GWorld->GetWorldInfo()->EmissiveSilhouetteFactor );
	eventUpdateCurrentAttackRange();
}

void UItemManager::AddToMergeMeshCache()
{
	if( MergedMesh != NULL )
	{
		GMergingHelper.AddToCache( MergedMesh );
	}
}

void UItemManager::PostMerge()
{
	USkeletalMeshComponent* baseComp = Owner->GetMasterComponent();
	if( baseComp == NULL )
		return;

	if( MergedMesh == NULL /*|| !MergedMesh.Check()*/ )
		return;

	if( MergedComp == NULL )
	{
		MergedComp = ConstructObject<USkeletalMeshComponent>( USkeletalMeshComponent::StaticClass() );
	}

	MergedComp->SetSkeletalMesh( MergedMesh );
	MergedComp->SetParentAnimComponent( baseComp );
	MergedComp->SetLightingChannels( baseComp->LightingChannels );
	MergedComp->SetLightEnvironment( Owner->LightEnvironment );
	Owner->LightEnvironment->ResetEnvironment();

	// need to create the MIC again, in case skin color will be different...
	//for( INT i = 0; i < MergedMesh->Materials.Num(); ++i )
	//{
	//	UMaterialInstance* pIns = CastChecked<UMaterialInstanceConstant>( StaticConstructObject(
	//			UMaterialInstanceConstant::StaticClass(), GetOuter() ) );

	//	pIns->SetParent( MergedMesh->Materials(i) );

	//	MergedComp->SetMaterial( i, pIns );
	//}

	Owner->AttachComponent( MergedComp );

	//if( MaterialController != none )
	//	MaterialController.Init( MergedComp );

	Owner->RebuildMaterialControllerList();
	Owner->RefreshAnim();

	UAvatarData* pAvatarData = Owner->GetAvatarData();
	if( pAvatarData )
	{
		if( GIsEditor )
		{	// for test colors in PIE
			ApplySkinColor( pAvatarData->TestSkinColorIdx );
			ApplyHairColor( pAvatarData->TestHairColorIdx );
		}
		else
		{	// to keep the latest skin color after item changes
			ApplySkinColor( Owner->ItemManager->CurrentSkinColorIdx );
			ApplyHairColor( Owner->ItemManager->CurrentHairColorIdx );
		}
		ApplyEmissiveSilhouetteFactor( GWorld->GetWorldInfo()->EmissiveSilhouetteFactor );

		// override the skin specular anyways
		MergedMesh->SetSkinSpecular( pAvatarData->SkinSpecular );

		// fake silhouette specular...
		MergedMesh->SetFakeSilhouetteColor( pAvatarData->SilhouetteColor );
	}

	debugf( TEXT("[MMO Info] Set merged mesh %s for %s. Merge Time: %.2f."),
		*MergedMesh->GetName(), *Owner->GetName(), MergeTime );

	bIsMerging = FALSE;
}

UEquipMesh* UItemManager::GetDefaultEquipMeshByType(BYTE type)
{
	UEquipMesh* mesh = NULL;
	for(int i = 0; i < DefaultMeshList.Num(); i++)
	{
		if(DefaultMeshList(i)->MeshType == type)
		{
			mesh = DefaultMeshList(i);
			break;
		}
	}
	return mesh;
}


void UItemManager::ChangeBySeleteMesh(BYTE type,const FString& path)
{
	/* StaticFindObject is called by default during StaticLoadObject. -ming
	USkeletalMesh* smt = Cast<USkeletalMesh>(UObject::StaticFindObject(USkeletalMesh::StaticClass(),ANY_PACKAGE, *path, TRUE));
	if (smt == NULL)
	{
		smt =  Cast<USkeletalMesh>(UObject::StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *path, NULL, LOAD_Quiet|LOAD_NoWarn, NULL));
	}
	*/
	USkeletalMesh* smt = Cast<USkeletalMesh>(UObject::StaticLoadObject(
		USkeletalMesh::StaticClass(), NULL, *path, NULL, LOAD_Quiet|LOAD_NoWarn, NULL));
	if (!smt)
		return;


	if (Owner)
	{
		UObject* e = Owner->GetEquip(type);
		if (e && e->IsA(USkeletalMeshComponent::StaticClass()))
		{
			((USkeletalMeshComponent*)e)->SetSkeletalMesh(smt);

			((USkeletalMeshComponent*)e)->RebulidMeshMaterialList();

			if (type == EBMT_Hair)
			{
				ApplyHairColor( CurrentHairColorIdx );
			}
		}
	}

	//UEquipMesh* equipMesh = CurrentMeshList(type);
	//if (equipMesh)
	//{
	//	equipMesh->SetSkeletMesh(smt);
	//}
}

void UItemManager::ChangeByEquipMesh(BYTE type,const FString& path)
{
	if( !path.Len() || Owner == NULL || type >= EBMT_None )
		return;

	/* StaticFindObject is called by default during StaticLoadObject. -ming
	UEquipMesh* em = Cast<UEquipMesh>(UObject::StaticFindObject(UEquipMesh::StaticClass(),ANY_PACKAGE, *path, TRUE));
	if (em == NULL)
	{
		em =  Cast<UEquipMesh>(UObject::StaticLoadObject(UEquipMesh::StaticClass(), NULL, *path, NULL, LOAD_Quiet|LOAD_NoWarn, NULL));
	}
	*/
	UEquipMesh* em = Cast<UEquipMesh>(UObject::StaticLoadObject(
		UEquipMesh::StaticClass(), NULL, *path, NULL, LOAD_Quiet|LOAD_NoWarn, NULL));
	// TEMP-DIAG(1.05 backport): remove once class heads are verified
	debugf(NAME_Log, TEXT("[MeshDiag] ChangeByEquipMesh type=%d path=%s loaded=%d skel=%s needSkinTex=%d colorTex=%d usedColorId=%d"), (INT)type, *path, em ? 1 : 0, (em && em->SkeletalMesh) ? *em->SkeletalMesh->GetPathName() : TEXT("NULL"), em ? (INT)em->bNeedSkinTexture : -1, em ? em->ColorTextures.Num() : -1, em ? em->UsedColorTextureID : -1);
	if (!em)
		return;

	if( CurrentMeshList(type) )
	{
		CurrentMeshList(type)->OnTakeOff(Owner, UseMergedMesh);
		CurrentMeshList(type) = NULL;
	}
	// TEMP-DIAG(1.05 backport)
	{
		FString slots;
		for( INT d = 0; d < DefaultMeshList.Num(); ++d ) slots += FString::Printf(TEXT("%d "), DefaultMeshList(d) ? (INT)DefaultMeshList(d)->MeshType : -1);
		debugf(NAME_Log, TEXT("[MeshDiag]   default slots(%d) types: %s useMerged=%d"), DefaultMeshList.Num(), *slots, (INT)UseMergedMesh);
	}
	for( INT i = 0; i < DefaultMeshList.Num(); ++i )
	{
		if( DefaultMeshList(i)->MeshType == type )
		{
			DefaultMeshList(i) = em;
			if( !UseMergedMesh )
				RefreshSkinTexture( i, FALSE );
			break;
		}
	}

	UpdateOwnerData();
}

void UItemManager::ApplyFaceModel(int profession, int sex, int id)
{
	if( CurrentFaceModelIdx == id )
		return;

	FString facePath = MMOLoginManager::getSingleton().GetMeshItemTempletByType(CMT_FACE_MODEL,profession,sex,id);
	debugf(NAME_Log, TEXT("[MeshDiag] ApplyFaceModel class=%d sex=%d id=%d path=%s"), profession, sex, id, *facePath);
	if (facePath.Len())
	{
		ChangeByEquipMesh(EBMT_Face,facePath);
	}

	CurrentFaceModelIdx = id;

	UpdateBeardColor();
}

void UItemManager::ApplyHairModel(int profession, int sex, int id)
{
	//if( CurrentHairModelIdx == id )
		//return;

	FString hairPath = MMOLoginManager::getSingleton().GetMeshItemTempletByType(CMT_HAIR_MODEL,profession,sex,id);
	debugf(NAME_Log, TEXT("[MeshDiag] ApplyHairModel class=%d sex=%d id=%d path=%s"), profession, sex, id, *hairPath);
	if (hairPath.Len())
	{
		ChangeByEquipMesh( EBMT_Hair, hairPath );
		//ChangeBySeleteMesh(EBMT_Hair,hairPath);
	}

	CurrentHairModelIdx = id;
}

void UItemManager::ApplyEmissiveSilhouetteFactor(FLOAT factor)
{
	auto function = []( UMaterialInstance* pMI, float factor ) { pMI->SetScalarParameterValue( FName(TEXT("Sihouette_Factor")), factor ); };
	if ( Owner->IsMainPlayer() )
		return;
	UAvatarData* avatar = Owner->GetAvatarData();
	if (avatar)
	{

		for ( int i = EBMT_Face ; i < EBMT_MAX ; ++i )
		{
			USkeletalMeshComponent* s = Cast<USkeletalMeshComponent>(Owner->GetEquip(i));
			if (s)
			{
				for(INT i = 0; i < s->Materials.Num(); i++)
				{
					UMaterialInstance* pMaterialInstance = ( UMaterialInstance* ) s->Materials(i);
					if(pMaterialInstance)
					{
						function( pMaterialInstance, factor );
					}
				}
			}
		}
	}
}
void UItemManager::ApplySkinColor( INT idx )
{
	if( UseMergedMesh || idx < 0 || idx == CurrentSkinColorIdx || Owner == NULL )
		return;

	CurrentSkinColorIdx = idx;

	for( INT i = 0; i < DefaultMeshList.Num(); ++i )
	{
		// only change color, needn't change base texture
		RefreshSkinTexture( i, TRUE );
	}

	UpdateOwnerData( TRUE );
}

void UItemManager::ApplyHairColor( INT idx )
{
	if (idx < 0 /*|| idx == CurrentHairColorIdx */|| Owner == NULL)
		return;
	UAvatarData* avatar = Owner->GetAvatarData();
	if (avatar)
	{
		if (idx >= avatar->HairColorSet.Num())
		{
			//debugf( TEXT( "ApplyHairColor idx out of range." ) );
			return;
		}

		USkeletalMeshComponent* s = Cast<USkeletalMeshComponent>(Owner->GetEquip(EBMT_Hair));
		if (s)
		{
			//UBOOL bChanged = FALSE;
			for(INT i = 0; i < s->Materials.Num(); i++)
			{
				UMaterialInstance* pMaterialInstance = ( UMaterialInstance* ) s->Materials(i);
				if(pMaterialInstance)
				{
					pMaterialInstance->SetVectorParameterValue( FName(TEXT("HairColor")), avatar->HairColorSet(idx));
					//bChanged = TRUE;
				}
			}

			//if (bChanged)
			//{
			//	s->RebulidMeshMaterialList();
			//}
		}

		CurrentHairColorIdx = idx;
		
		// change beard if needed
		UpdateBeardColor();
	}
}

void UItemManager::UpdateBeardColor()
{
	if( !UseMergedMesh )
		return;

	UAvatarData* avatar = Owner->GetAvatarData();
	if( avatar == NULL )
		return;

	if( CurrentHairColorIdx < 0 || CurrentHairColorIdx >= avatar->HairColorSet.Num() )
		return;

	USkeletalMeshComponent* s = Cast<USkeletalMeshComponent>( Owner->GetEquip(EBMT_Face) );
	if( s )
	{
		for( INT i = 0; i < s->Materials.Num(); ++i )
		{
			UMaterialInstance* pMaterialInstance = Cast<UMaterialInstance>( s->Materials(i) );
			if( pMaterialInstance )
			{
				FLinearColor temp;
				if( pMaterialInstance->GetVectorParameterValue( FName(TEXT("BeardColor")), temp ) )
				{
					pMaterialInstance->SetVectorParameterValue( FName(TEXT("BeardColor")), avatar->HairColorSet(CurrentHairColorIdx) );
				}
			}
		}
	}
}

void UItemManager::SetHair( int ModelID, int ColorID )
{
	ApplyHairModel( ClassType, GenderType, ModelID );
	ApplyHairColor( ColorID );
}

void UItemManager::SetFace( int ModelID )
{
	ApplyFaceModel( ClassType, GenderType, ModelID );
}

void UItemManager::UpdateApperance()
{
	if( Owner == NULL )
		return;

	//if( UseMergedMesh )
	//{
	//}
	//else
	//{
		SetHair( Owner->ActorAttributes->HairType, Owner->ActorAttributes->HairColor );
		SetFace( Owner->ActorAttributes->FaceType );
	//}
}

void UItemManager::HideHelmet( UBOOL bHidden )
{
	if( bHelmetHidden == bHidden )
		return;

	bHelmetHidden = bHidden;
	if( CurrentMeshList( EBMT_Helmet ) )
	{
		AArmorActor* helmet = Cast<AArmorActor>(Owner->GetEquip(EBMT_Helmet));
		if (helmet)
		{
			helmet->SetVisible(!bHidden);
		}
		Owner->SetEquipVisibleFlag( EBMT_Helmet, FALSE );

		CurrentMeshList( EBMT_Helmet )->DoHideCheck( Owner, !bHidden );
	}
}

void UItemManager::ChangeEquipMeshMaterial(BYTE type,const FString& materialPath)
{
	UTexture *Diffuse , *Normal , *Specular;
	UMaterialInterface *newMaterial;

	newMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *materialPath, NULL, LOAD_NoWarn, NULL));
	UEquipMesh* pEquipMesh = Cast<UEquipMesh>( CurrentMeshList( type ) );
	if( pEquipMesh )
	{
		UObject* obj = Owner->GetEquip(pEquipMesh->MeshType);
		if (obj)
		{
			UMeshComponent* ToModifyComponent = NULL;
			if (obj->IsA(UMeshComponent::StaticClass()))
			{
				ToModifyComponent = (UMeshComponent*)obj;
			}
			else if (obj->IsA(AArmorActor::StaticClass()))
			{
				ToModifyComponent = ((AArmorActor*)obj)->MainMesh;
			}

			if (ToModifyComponent)
			{
				for (INT j = 0; j < ToModifyComponent->Materials.Num(); ++j)
				{
					UMaterialInstance* pMaterialInstance = ( UMaterialInstance* ) ToModifyComponent->Materials(j);
					pMaterialInstance->GetTextureParameterValue("Diffuse_Map", Diffuse );
					pMaterialInstance->GetTextureParameterValue("Normal_Map", Normal );
					pMaterialInstance->GetTextureParameterValue("Specular_Map", Specular );
					pMaterialInstance->SetParent( newMaterial );
					pMaterialInstance->SetTextureParameterValue("Diffuse_Map", Diffuse );
					pMaterialInstance->SetTextureParameterValue("Normal_Map", Normal );
					pMaterialInstance->SetTextureParameterValue("Specular_Map", Specular );
				}
			}
		}
	}
}