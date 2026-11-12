#include "stdafx.h"
#include "item_def.h"
#include "MMOFacade.h"
#include "MMOGameUIClasses.h"
#include "MMODataManager.h"
#include "MMOEquipManager.h"
#include "MMOSkillManager.h"

IMPLEMENT_CLASS(UItem)
IMPLEMENT_CLASS(UEquip)
IMPLEMENT_CLASS(UEquipAppearence)
IMPLEMENT_CLASS(AEmitterAttachment);
//void AItem::PlayRandomSound(const TArray<class USoundCue*>& soundList)
//{	
//	int idx = (int)RandRange(0, soundList.Num());
//	AActor::PlaySound(soundList(idx));
//}

void UItem::Init(BYTE pc, BYTE gt, UBOOL useAdvancedApperance)
{

}

UBOOL UItem::IsUseblForActor( AMMOActor* actor)
{
	if( actor->ActorAttributes && actor->ActorAttributes->LevelAttributes.Level < RequireLevel )
		return false;
	if( ( RequireGender==EGT_Male || RequireGender==EGT_Female ) &&actor->GetSex() != RequireGender   )
		return false;
	if( ! MMO::CanBeUsedByPlayerClass( EPlayerClass(actor->GetClassId()) ,RequireClass  ) )
		return false;
	return true;
}
//------------------------------------------------------------------------
UBOOL UItem::IsMoney() const
{
	return ItemId <= 100;
}

FString UItem::GetDisplayName() const
{
	return MMOUI::TrimNumber(ItemName);
}

void UItem::GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values)
{
	if( ListInfo.ListName == TEXT("ItemType") )
	{
		TArray<FString> typeArray;
		if( GConfig->GetArray( TEXT("ItemType"), TEXT("ItemType"), typeArray, GGameIni ) > 0 )
		{
			for (INT i = 0; i < typeArray.Num(); ++i)
			{
				if( typeArray(i) != "None" )
					Values.AddItem(typeArray(i));
			}		
		}
	}
	else if( ListInfo.ListName == TEXT("ItemSubType") )
	{
		TArray<FString> subTypeArray;
		FString section = ItemType;
		if( GConfig->GetArray( *section, TEXT("SubType"), subTypeArray, GGameIni ) > 0 )
		{
			for (INT i = 0; i < subTypeArray.Num(); ++i)
			{
				Values.AddItem(subTypeArray(i));
			}		
		}
		else
		{
			Values.AddItem( "None" );
		}
	}
	else if( ListInfo.ListName == TEXT("ItemSubSubType") )
	{
		TArray<FString> subsubTypeArray;
		FString section = ItemSubType;
		if( section == "ITEMSUBTYPE_EQUIP_HAT" ||
			section == "ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR" ||
			section == "ITEMSUBTYPE_EQUIP_CHEST_ARMOR" ||
			section == "ITEMSUBTYPE_EQUIP_PANTS" ||
			section == "ITEMSUBTYPE_EQUIP_WRIST_ARMOR" ||
			section == "ITEMSUBTYPE_EQUIP_BELT" ||
			section == "ITEMSUBTYPE_EQUIP_SHOES" ||
			section == "ITEMSUBTYPE_EQUIP_CLOAK" )
		{
			section = "ITEMSUBTYPE_EQUIP_ARMOR";
		}
		if( GConfig->GetArray( *section, TEXT("SubType"), subsubTypeArray, GGameIni ) > 0 )
		{
			for( INT i = 0; i < subsubTypeArray.Num(); ++i )
			{
				Values.AddItem( subsubTypeArray(i) );
			}
		}
		else
		{
			Values.AddItem( "None" );
		}
	}
}

void UItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if( PropertyChangedEvent.Property->GetName() == "ItemType" )
		ItemSubType = "None";
	else if( PropertyChangedEvent.Property->GetName() == "ItemSubType" )
		ItemSubSubType = "None";
}

//song@snda.com
UBOOL UItem::IsNeedArchetype()
{
	return false;
}
//------------------------------------------------------------------------
void UItem::ExportExcelHeaders(TArray<FString>& Header,TArray<FString>& DefaultValues)
{

}
//------------------------------------------------------------------------
void UItem::DataToExcel(TArray<FString>& Row)
{

}
//------------------------------------------------------------------------
void UItem::ImportFromExcel(const TArray<FString>& Values)
{	
}

int UItem::GetTemplateID() const
{
	return ItemId;
}

void UItem::InitDataToExcel()
{
}


UBOOL UItem::OnTakeOn(AMMOActor* Target, UBOOL UseAdvancedAppearence)
{
	return TRUE;
}

UBOOL UItem::OnTakeOff(class AMMOActor* Target)
{
	return TRUE;
}


FString UItem::GetLargeIconName(bool isMale, int actorClass)
{
	FString FullName;

	if( DisplayLargeIcon_Str.IsEmpty() )
	{
		return GetIconName();
	}
	
	UObject* Obj = GetDisplayLargeIcon();
	FullName = FString( Obj->GetPureName().ToString() );
	while( Obj && Obj->GetOuter() )
	{
		FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
		Obj= Obj->GetOuter();
	}

	return FullName;

}

//--------------------------------------------------------------------------
FString UItem::GetIconName(int nState)
{
	if(DisplayIcon_Str.IsEmpty())
	{
		return TEXT("");
	}

	FString FullName;

	UObject* Obj = GetDisplayIcon();
	if( Obj == NULL )
	{
		return TEXT("");
	}
	FullName = FString(Obj->GetPureName().ToString() );

	//is a soul bottle
	if(ItemDuration == E_DURTYPE_DEPOSIT)
	{
		if(nState > 0)
		{
			INT nPos = FullName.InStr("_1",TRUE);
			if (nPos >= 0)
			{
				INT nIconIndex = (DefaultDuration == nState)?3:2;
				FString strReplace = FString::Printf(TEXT("_%d"),nIconIndex);
				FullName.ReplaceInline(TEXT("_1"),*strReplace);
			}
		}
	}

	while( Obj && Obj->GetOuter() )
	{
		FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
		Obj= Obj->GetOuter();
	}
	//int i = 0;
	//FString str = FString( DisplayIcon->GetOutermost() ->GetOut());// ->TextureFileCacheName.ToString() );
	return FullName;
}

#if WITH_EDITOR

void UItem::CheckForErrors()
{
	Super::CheckForErrors();
	
	if(!DisplayIcon)
	{
		GWarn->MapCheck_Add(MCTYPE_ERROR, this, 
			*LocalizeUnrealEdMMO(TEXT("CheckError_DisplayIcon")),
			NULL);
	}
// 	GWarn->MapCheck_Add(MCTYPE_ERROR, this,
// 		*FString::Printf(TEXT("Sell price %d > Purchase Price %d"),
// 		SellPrice, ),
// 		NULL);
}
#endif

FString UEquipAppearence::GetDetailedDescription( INT InIndex )
{
	switch(InIndex)
	{
	case 0:
		return ConfigDisplayName;

	case 1:
		{
			UEnum* e = FindObject<UEnum>( UTypeDefine::StaticClass(), TEXT("EEquipType"), TRUE );
			//FString enumVal = e->GetEnum(EquipType).ToString();
			const FString& enumVal = e->GetMetaData( TEXT("DisplayName"), EquipType );
			FString cfg = Localize(TEXT("EEquipType"), *enumVal, TEXT("Properties"));
			return cfg;
		}

	case 2:
		{
			UEnum* e = FindObject<UEnum>( UTypeDefine::StaticClass(), TEXT("EWeaponType"), TRUE );
			FString enumVal = e->GetEnum(WeaponType).ToString();
			FString cfg = Localize(TEXT("EWeaponType"), *enumVal, TEXT("Properties"));
			return cfg;
		}

	default:
		return TEXT("");
	}
}

void UEquipAppearence::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	ShowWeaponType = (EquipType == EET_Normal_MainHand);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#if WITH_EDITOR

void UEquipAppearence::CheckForErrors()
{
	Super::CheckForErrors();

	for (INT i = 0; i < MeshListTemplate_F.Num(); ++i)
	{
		if(MeshListTemplate_F(i))
		{
			MeshListTemplate_F(i)->CheckForError(this);
		}
	}

	for (INT i = 0; i < MeshListTemplate_M.Num(); ++i)
	{
		if(MeshListTemplate_M(i))
		{
			MeshListTemplate_M(i)->CheckForError(this);
		}
	}

	for (INT i = 0; i < RandomMeshListTemplate_M.Num(); ++i)
	{
		if(RandomMeshListTemplate_M(i))
		{
			RandomMeshListTemplate_M(i)->CheckForError(this);
		}
	}

	for (INT i = 0; i < RandomMeshListTemplate_F.Num(); ++i)
	{
		if(RandomMeshListTemplate_F(i))
		{
			RandomMeshListTemplate_F(i)->CheckForError(this);
		}
	}
}

void UItem::CollectReferencedResource(TSet<FString>& outResList)
{
	_CollectReferencedResource(this, outResList);
	_CollectReferencedResource(GetDropMesh(), outResList);
	_CollectReferencedResource(GetDisplayIcon(), outResList);
	_CollectReferencedResource(GetDisplayLargeIcon(), outResList);
	_CollectReferencedResource(GetDisplayLargeIconFemale(), outResList);
	_CollectReferencedResource(GetToUseAbility(), outResList);
}

void UEquipAppearence::CollectReferencedResource(TSet<FString>& outResList)
{
	_CollectReferencedResource(this, outResList);

	for (INT i = 0; i < MeshListTemplate_F.Num(); ++i)
	{
		if(MeshListTemplate_F(i))
		{
			MeshListTemplate_F(i)->CollectReferencedResource(outResList);
		}
	}

	for (INT i = 0; i < MeshListTemplate_M.Num(); ++i)
	{
		if(MeshListTemplate_M(i))
		{
			MeshListTemplate_M(i)->CollectReferencedResource(outResList);
		}
	}

	for (INT i = 0; i < RandomMeshListTemplate_M.Num(); ++i)
	{
		if(RandomMeshListTemplate_M(i))
		{
			RandomMeshListTemplate_M(i)->CollectReferencedResource(outResList);
		}
	}

	for (INT i = 0; i < RandomMeshListTemplate_F.Num(); ++i)
	{
		if(RandomMeshListTemplate_F(i))
		{
			RandomMeshListTemplate_F(i)->CollectReferencedResource(outResList);
		}
	}

	if( MeshList_WA_M )
		MeshList_WA_M->CollectReferencedResource( outResList );
	if( MeshList_WA_F )
		MeshList_WA_F->CollectReferencedResource( outResList );
	if( MeshList_MA_M )
		MeshList_MA_M->CollectReferencedResource( outResList );
	if( MeshList_MA_F )
		MeshList_MA_F->CollectReferencedResource( outResList );
	if( MeshList_TA_M )
		MeshList_TA_M->CollectReferencedResource( outResList );
	if( MeshList_TA_F )
		MeshList_TA_F->CollectReferencedResource( outResList );
	if( MeshList_AG_M )
		MeshList_AG_M->CollectReferencedResource( outResList );
	if( MeshList_AG_F )
		MeshList_AG_F->CollectReferencedResource( outResList );
	if( MeshList_AR_M )
		MeshList_AR_M->CollectReferencedResource( outResList );
	if( MeshList_AR_F )
		MeshList_AR_F->CollectReferencedResource( outResList );
	if( MeshList_SP_M )
		MeshList_SP_M->CollectReferencedResource( outResList );
	if( MeshList_SP_F )
		MeshList_SP_F->CollectReferencedResource( outResList );
	if( MeshList_YS_M )
		MeshList_YS_M->CollectReferencedResource( outResList );
	if( MeshList_YS_F )
		MeshList_YS_F->CollectReferencedResource( outResList );

	for( INT i = 0; i < AdditionalMeshList_WA_M.Num(); ++i )
	{
		if( AdditionalMeshList_WA_M(i) )
			AdditionalMeshList_WA_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_WA_F.Num(); ++i )
	{
		if( AdditionalMeshList_WA_F(i) )
			AdditionalMeshList_WA_F(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_MA_M.Num(); ++i )
	{
		if( AdditionalMeshList_MA_M(i) )
			AdditionalMeshList_MA_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_MA_F.Num(); ++i )
	{
		if( AdditionalMeshList_MA_F(i) )
			AdditionalMeshList_MA_F(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_TA_M.Num(); ++i )
	{
		if( AdditionalMeshList_TA_M(i) )
			AdditionalMeshList_TA_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_TA_F.Num(); ++i )
	{
		if( AdditionalMeshList_TA_F(i) )
			AdditionalMeshList_TA_F(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_AG_M.Num(); ++i )
	{
		if( AdditionalMeshList_AG_M(i) )
			AdditionalMeshList_AG_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_AG_F.Num(); ++i )
	{
		if( AdditionalMeshList_AG_F(i) )
			AdditionalMeshList_AG_F(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_AR_M.Num(); ++i )
	{
		if( AdditionalMeshList_AR_M(i) )
			AdditionalMeshList_AR_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_AR_F.Num(); ++i )
	{
		if( AdditionalMeshList_AR_F(i) )
			AdditionalMeshList_AR_F(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_SP_M.Num(); ++i )
	{
		if( AdditionalMeshList_SP_M(i) )
			AdditionalMeshList_SP_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_SP_F.Num(); ++i )
	{
		if( AdditionalMeshList_SP_F(i) )
			AdditionalMeshList_SP_F(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_YS_M.Num(); ++i )
	{
		if( AdditionalMeshList_YS_M(i) )
			AdditionalMeshList_YS_M(i)->CollectReferencedResource( outResList );
	}
	for( INT i = 0; i < AdditionalMeshList_YS_F.Num(); ++i )
	{
		if( AdditionalMeshList_YS_F(i) )
			AdditionalMeshList_YS_F(i)->CollectReferencedResource( outResList );
	}
}

void UEquipMesh::CollectReferencedResource(TSet<FString>& outResList)
{
	TSet<FString> tmpSet;
	_CollectReferencedResource(this, tmpSet);
	outResList = outResList.Union(tmpSet);

	extern void _CollectReferencedResource(UObject* obj, TSet<FString>& outResList);
	if(StaticMesh)
	{
		FString subRes = StaticMesh->GetPathName() + FString(TEXT("_Shell"));
		UObject* SubStaticMesh = StaticLoadObject(UStaticMesh::StaticClass(), NULL, *subRes, NULL, LOAD_NoWarn|LOAD_Quiet, NULL);
		_CollectReferencedResource(SubStaticMesh, outResList);
	}

	if(RegionTexture)
	{

		FString OldName = RegionTexture->GetName();
		FString NewName = OldName.LeftChop(1) + TEXT("N");
		FString ToLoadName;
		if (RegionTexture->IsInExportedPackage())
		{
			ToLoadName = RegionTexture->GetOuter()->GetName();
			ToLoadName = ToLoadName.Replace(*OldName, *NewName, TRUE);
			ToLoadName = ToLoadName + FString(TEXT(".")) + NewName;
		}
		else
		{
			ToLoadName = RegionTexture->GetPathName();
			ToLoadName = ToLoadName.LeftChop(1) + TEXT("N");
		}

		UObject* obj = StaticLoadObject(UTexture2D::StaticClass(), NULL, *ToLoadName, NULL, LOAD_NoWarn|LOAD_Quiet, NULL);
		if(!obj)
		{
			warnf(NAME_Error, TEXT("failed to load: %s"), *ToLoadName);
		}
		_CollectReferencedResource(obj, outResList);

		NewName = OldName.LeftChop(1) + TEXT("S");
		if (RegionTexture->IsInExportedPackage())
		{
			ToLoadName = RegionTexture->GetOuter()->GetName();
			ToLoadName = ToLoadName.Replace(*OldName, *NewName, TRUE);
			ToLoadName = ToLoadName + FString(TEXT(".")) + NewName;
		}
		else
		{
			ToLoadName = RegionTexture->GetPathName();
			ToLoadName = ToLoadName.LeftChop(1) + TEXT("N");
		}
		obj = StaticLoadObject(UTexture2D::StaticClass(), NULL, *ToLoadName, NULL, LOAD_NoWarn|LOAD_Quiet, NULL);
		if(!obj)
		{
			warnf(NAME_Error, TEXT("failed to load: %s"), *ToLoadName);
		}
		_CollectReferencedResource(obj, outResList);
	}
	else
	{
		warnf(NAME_Log, TEXT("======== base texture is NULL:%s"), *GetFullName());
	}
}

#endif


class UTexture2D* UItem::GetDisplayIcon()
{
	if(DisplayIcon)
	{
		return DisplayIcon;
	}

	DisplayIcon = LoadUEObject<UTexture2D>(DisplayIcon_Str, TEXT("item.txt"));

	return DisplayIcon;
}


class UTexture2D* UItem::GetDisplayLargeIcon()
{
	if(DisplayLargeIcon)
	{
		return DisplayLargeIcon;
	}

	DisplayLargeIcon = LoadUEObject<UTexture2D>(DisplayLargeIcon_Str, TEXT("item.txt"));

	return DisplayLargeIcon;
}

class UTexture2D* UItem::GetDisplayLargeIconFemale()
{
	if(DisplayLargeIconFemale)
	{
		return DisplayLargeIconFemale;
	}

	DisplayLargeIconFemale = LoadUEObject<UTexture2D>(DisplayLargeIconFemale_Str, TEXT("item.txt"));

	return DisplayLargeIconFemale;
}

class UStaticMesh* UItem::GetDropMesh()
{
	if(DropMesh)
	{
		return DropMesh;
	}

	DropMesh = LoadUEObject<UStaticMesh>(DropMesh_Str, TEXT("item.txt"));

	return DropMesh;
}

class UMMOAbility* UItem::GetToUseAbility()
{
	if(ToUseAbility)
	{
		return ToUseAbility;
	}

	if( ToUseAbility_SpellID > 0 )
	{
		UINT Key = 0;

		//if( ItemType == TEXT("ITEMTYPE_RUNE") )
		//{
		//	//inscription item, special handling here
		//	MMOEquipManager::getSingleton().GetSkillUniqueIDOnWeapon( Key, ToUseAbility_SpellID );
		//}
		//else
		//{
		Key = MMOSkillManager::CombinateAbilityID( ToUseAbility_SpellID, SpellLevel, 0 );
		//}
		
		ToUseAbility = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
		
		if(ToUseAbility == NULL)
		{
			M_AddDataError(TEXT("item.txt"), FString::Printf(TEXT("Item %s has an invalid ability property, cannot find ability id=%d"), *ItemName, ToUseAbility_SpellID));
		}
	}

	return ToUseAbility;
}

UBOOL UItem::IsWeapon()
{
	return FALSE;
}

UBOOL UItem::IsEquip()
{
	return FALSE;
}