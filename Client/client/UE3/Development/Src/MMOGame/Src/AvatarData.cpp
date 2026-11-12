#include "stdafx.h"

IMPLEMENT_CLASS(UAvatarData)


FLOAT UAvatarData::GetMoveSpeed(BYTE InMoveType)
{
	if (InMoveType == EMT_Walk)
	{
		return (WalkSpeed > 0.f ? WalkSpeed : DefaultMoveSpeed);
	}
	else if (InMoveType == EMT_Run)
	{
		return (RunSpeed > 0.f ? RunSpeed : DefaultMoveSpeed);
	}
	else if (InMoveType == EMT_Fly)
	{
		return (FlySpeed > 0.f ? FlySpeed : DefaultMoveSpeed);
	}

	return 0.001f;
}

FLOAT UAvatarData::GetToggleMoveAnimCriticalSpeed()
{
	if (RunSpeed <= WalkSpeed)
	{
		return -1.f;
	}

	if (RunSpeed - WalkSpeed > 10.f)
	{
		return WalkSpeed + (RunSpeed - WalkSpeed) * ToggleMoveAnimCriticalValue;
	}
	else
	{
		return -1.f;
	}
}

UPhysicsAsset* UAvatarData::LoadPhysicsAsset()
{
	if (SkeletalMesh == NULL)
	{
		return NULL;
	}

	FString MeshName = SkeletalMesh->GetPathName();
	MeshName += FString(TEXT("_Physics"));
	UPhysicsAsset* PhysicAssetData =Cast<UPhysicsAsset>(UObject::StaticLoadObject(UPhysicsAsset::StaticClass(), NULL, *MeshName, NULL,  LOAD_NoWarn, NULL));
	return PhysicAssetData;
}

void UAvatarData::DynamicLoadPhysicAsset()
{
	//if (bLoadedPhysicAsset && !GIsEditor)
	//{
	//	return;
	//}

	//if (SkeletalMesh == NULL)
	//{
	//	return;
	//}

	//UPhysicsAsset* PhysicAssetData = LoadPhysicsAsset();
	//if (PhysicAssetData != PhysicAsset)
	//{
	//	PhysicAsset = PhysicAssetData;
	//	if (GIsEditor)
	//	{
	//		this->MarkPackageDirty();
	//	}
	//}

	//bLoadedPhysicAsset = TRUE;

	if (SkeletalMesh == NULL)
	{
		return;
	}

	UBOOL bNeedLoadPhysicAsset = FALSE;

	if (PhysicAsset == NULL)
	{
		bNeedLoadPhysicAsset = TRUE;
	}
	else
	{
		FString PhysicAssetName = PhysicAsset->GetName();
		FString MeshName = SkeletalMesh->GetPathName() + FString(TEXT("_Physics"));

		if (PhysicAssetName != MeshName)
		{
			bNeedLoadPhysicAsset = TRUE;
		}
	}

	PhysicAsset = LoadPhysicsAsset();
	if (GIsEditor && PhysicAsset)
	{
		PhysicAsset->MarkPackageDirty();
	}
}

UBOOL UAvatarData::NeedToBeMerged()
{
	if( UseMergedMesh )
		return FALSE;

	UBOOL HasSomePartToMerge = FALSE;
	for( INT i = 0; i < DefaultMeshListTemplate.Num(); ++i )
	{
		if( DefaultMeshListTemplate(i) && ( DefaultMeshListTemplate(i)->MeshType < EBMT_NoMerge ))
		{
			HasSomePartToMerge = TRUE;
			break;
		}
	}

	return HasSomePartToMerge;
}

void UAvatarData::RebulidMesh(USkeletalMeshComponent* BaseSMC, USkeletalMeshComponent* InMergedComponent)
{
	if (!GIsEditor)
	{
		return;
	}

	if (!BaseSMC || !InMergedComponent)
	{
		return;
	}

	if( UseMergedMesh )
		return;

	BaseComponent = BaseSMC;
	MergedComp = InMergedComponent;

	InMergedComponent->SetSkeletalMesh(NULL);

	if (MergedMesh)
	{
		MergedMesh->MarkPendingKill();
	}

	//local BodyPart bp;
	MergedMesh = Cast<UMMOSkeletalMeshMerge>(StaticConstructObject(UMMOSkeletalMeshMerge::StaticClass()));

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


	typedef TArray< UEquipMesh* >	EquipMeshArray;

	EquipMeshArray meshArray[8];

	TArray<UEquipMesh*> &DefaultMeshList = DefaultMeshListTemplate;

	for( INT i = 0; i < DefaultMeshList.Num(); ++i )
	{
		if( DefaultMeshList(i) == NULL )// || ( DefaultMeshList(i)->MeshType >= EBMT_NoMerge ))
			continue;

		INT type = DefaultMeshList(i)->MeshType;
		if( MergeShoulder )
		{	
			if( type == EBMT_LShoulder )
				type = EBMT_Face;
			else if( type == EBMT_Face )
				continue;
		}
		if( type >= EBMT_NoMerge )
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

	SkinTextures.Remove( 0, SkinTextures.Num() );
	SkinTextures.AddZeroed( EBMT_Waist );
	SkinTexturesBase.Remove( 0, SkinTexturesBase.Num() );
	SkinTexturesBase.AddZeroed( EBMT_Waist );

	for(INT i = 0; i < DefaultMeshList.Num(); ++i)
	{
		UEquipMesh* tEquipMesh = DefaultMeshList(i);
		//`Log( "[MMO Debug] Default mesh:"@tEquipMesh );
		if( tEquipMesh != 0 && tEquipMesh->MeshType < EBMT_Waist )
		{
			if( DefaultMeshList(i)->UsedColorTextureID >= 0 &&
				DefaultMeshList(i)->ColorTextures.Num() > DefaultMeshList(i)->UsedColorTextureID )
			{
				SkinTexturesBase(DefaultMeshList(i)->MeshType) =
					DefaultMeshList(i)->ColorTextures( DefaultMeshList(i)->UsedColorTextureID );
			}
			else
			{
				SkinTexturesBase(DefaultMeshList(i)->MeshType) = NULL;
				debugf( TEXT("Failed to get skin base texture.") );
			}
			if( DefaultMeshList(i)->ColorTextures.Num() > TestSkinColorIdx )
			{
				SkinTextures(DefaultMeshList(i)->MeshType) =
					DefaultMeshList(i)->ColorTextures( TestSkinColorIdx );
			}
			else
			{
				SkinTextures(DefaultMeshList(i)->MeshType) = 
					DefaultMeshList(i)->ColorTextures.Num() > 0 ?
					DefaultMeshList(i)->ColorTextures(0) : NULL;
				debugf( TEXT("Skin color idx out of bound of the texture array.") );
			}
		}

		//if( DefaultMeshList(i)->MeshType == EBMT_Hair )
		//{
		//	meshArray[DefaultMeshList(i)->MeshType].AddItem(tEquipMesh );
		//}
	}

	for( INT i = 0; i < 6; ++i )
	{
		MergedMesh->Parts(i).Mesh = NULL;
		MergedMesh->Parts(i).Textures.Empty(0);

		if( meshArray[i].Num() < 1 )
		{	// error
			//continue;
			if( i == 5 )
			{	// may have no skirt
				continue;
			}
			if( MergeShoulder && i == EBMT_Face )
				continue;
			return;
		}

		USkeletalMesh* pMesh = meshArray[i](meshArray[i].Num()-1)->SkeletalMesh;
		MergedMesh->Parts(i).Mesh = pMesh;
		for( INT j = 0; j < meshArray[i].Num(); ++j )
		{
			// Belt has no skin texture so drop the i==5 case
			if( j == 0 && i != 5 && ( !MergeShoulder || i != EBMT_Face ) && meshArray[i](j)->bNeedSkinTexture )
			{
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
				return;
			}

		}
	}


	//if (  HairColorSet.Num() > 0)
	//{
	//	if( meshArray[EBMT_Hair](0))
	//	{
	//		UEquipMesh* defaultHair = meshArray[EBMT_Hair](0);
	//		if (defaultHair)
	//		{
	//			INT idx = Clamp<INT>( TestHairColorIdx, 0, HairColorSet.Num() - 1 );

	//			defaultHair->SetMaterialInstanceParamValue(HairColorSet(idx),"HairColor");
	//		}
	//	}
	//}

	UBOOL bRet = MergedMesh->PrepareTextures();

	if (!bRet)
	{
		bIsPreparing = FALSE;
		bIsMerging = FALSE;
		return;
	}

	bIsPreparing = TRUE;

	
}

void UAvatarData::TickMerge(FLOAT Delta)
{
	if (!GIsEditor)
	{
		return;
	}

	if (bIsPreparing)
	{
		MergedMesh->UpdateState();
		if (MergedMesh->LastMergeCancelled() && MergedMesh->IsSourceTexturesFullyLoaded())
		{
			MergedMesh->DoMerge();
			bIsPreparing = FALSE;
			bIsMerging = TRUE;
		}
	}
	else if (bIsMerging)
	{
		MergedMesh->UpdateState();
		if (MergedMesh->CheckFinish())
		{
			PostRebuildMesh();
		}
	}
}

void UAvatarData::PostRebuildMesh()
{
	if (!GIsEditor)
	{
		return;
	}

	MergedMesh->PostMerge();
	MergedComp->SetSkeletalMesh(MergedMesh);
	MergedComp->SetParentAnimComponent(BaseComponent);
	MergedComp->SetLightingChannels(BaseComponent->LightingChannels);
	MergedComp->SetLightEnvironment(BaseComponent->LightEnvironment);

	INT idx;
	// todo: change skin color
	//if( SkinColorSet.Num() > 0 )
	//{
	//	idx = Clamp<INT>( TestSkinColorIdx, 0, SkinColorSet.Num() - 1 );
	//	MergedMesh->SetSkinColor( SkinColorSet(idx) );
	//}
	if( HairColorSet.Num() > 0 )
	{
		idx = Clamp<INT>( TestHairColorIdx, 0, HairColorSet.Num() - 1 );
		// todo: change hair color here
		/*UEquipMesh* defaultHair = GetDefaultEquipMeshByType(EBMT_Hair);
		if (defaultHair)
		{
			defaultHair->SetHairColor(HairColorSet(idx));
		}*/
	}

	MergedMesh->SetSkinSpecular( SkinSpecular );

	bIsMerging = FALSE;

}

void UAvatarData::ToogleBoneBreak(USkeletalMeshComponent* mesh, UBOOL breakWeight)
{
	if(mesh)
	{
		for (INT LODIdx=0; LODIdx < mesh->LODInfo.Num(); ++LODIdx)
		{
			if (mesh->LODInfo(LODIdx).InstanceWeightUsage == IWU_PartialSwap)
			{
				mesh->ToggleInstanceVertexWeights( breakWeight, LODIdx );
			}
		}
	}
}

UEquipMesh* UAvatarData::GetDefaultEquipMeshByType(BYTE type)
{
	UEquipMesh* mesh = NULL;
	for(int i = 0; i < DefaultMeshListTemplate.Num(); i++)
	{
		if(DefaultMeshListTemplate(i)->MeshType == type)
			mesh = DefaultMeshListTemplate(i);
	}
	return mesh;
}